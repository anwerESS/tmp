# Guide de Préparation : Apache Kafka & Kafka Connect
## Pour développeur Full Stack avec expérience RabbitMQ

---

## Table des matières
1. [Introduction et Positionnement](#1-introduction-et-positionnement)
2. [Architecture Kafka](#2-architecture-kafka)
3. [Concepts Fondamentaux](#3-concepts-fondamentaux)
4. [Kafka vs RabbitMQ](#4-kafka-vs-rabbitmq)
5. [Kafka Connect](#5-kafka-connect)
6. [Cas d'Usage Pratiques](#6-cas-dusage-pratiques)
7. [Points Clés pour l'Entretien](#7-points-clés-pour-lentretien)

---

## 1. Introduction et Positionnement

### Qu'est-ce que Kafka ?

Apache Kafka est une **plateforme de streaming distribué** développée initialement par LinkedIn, puis open-sourcée en 2011.

**Définition simple :** Kafka est un système de messaging publish-subscribe distribué, conçu pour gérer des flux de données massifs en temps réel avec haute disponibilité et durabilité.

### Différence fondamentale avec RabbitMQ

| Aspect | RabbitMQ | Kafka |
|--------|----------|-------|
| **Paradigme** | Message Broker traditionnel | Event Streaming Platform |
| **Modèle** | Push (broker pousse vers consommateurs) | Pull (consommateurs tirent du broker) |
| **Persistance** | Messages supprimés après consommation | Messages conservés (retention configurable) |
| **Usage principal** | Tâches asynchrones, RPC | Logs, événements, streaming de données |

---

## 2. Architecture Kafka

### 2.1 Composants Principaux

```
┌─────────────────────────────────────────────────────────┐
│                    KAFKA CLUSTER                         │
│                                                          │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐              │
│  │ Broker 1 │  │ Broker 2 │  │ Broker 3 │              │
│  │          │  │          │  │          │              │
│  │ Topic A  │  │ Topic A  │  │ Topic B  │              │
│  │ Part. 0  │  │ Part. 1  │  │ Part. 0  │              │
│  └──────────┘  └──────────┘  └──────────┘              │
│                                                          │
│  ┌───────────────────────────────────────┐              │
│  │        ZooKeeper (coordination)        │              │
│  │    (ou KRaft dans versions récentes)   │              │
│  └───────────────────────────────────────┘              │
└─────────────────────────────────────────────────────────┘
         ▲                           │
         │                           ▼
    ┌─────────┐              ┌──────────────┐
    │Producer │              │  Consumer    │
    │         │              │  Group       │
    └─────────┘              └──────────────┘
```

### 2.2 Les Brokers

- **Broker** = Serveur Kafka (comme un nœud RabbitMQ)
- Un cluster Kafka contient plusieurs brokers pour la haute disponibilité
- Chaque broker gère un sous-ensemble des partitions
- Identifié par un ID unique

### 2.3 ZooKeeper / KRaft

**ZooKeeper (ancien système):**
- Coordonne le cluster
- Gère les métadonnées
- Élit les leaders de partitions

**KRaft (nouveau, depuis Kafka 3.0):**
- Remplace ZooKeeper
- Mode natif de Kafka
- Simplifie l'architecture

---

## 3. Concepts Fondamentaux

### 3.1 Topics

**Définition :** Un topic est une catégorie/flux de messages (équivalent à une queue RabbitMQ, mais avec des différences importantes).

```
Topic "commandes-clients"
│
├─ Partition 0 : [msg1] [msg2] [msg5] ...
├─ Partition 1 : [msg3] [msg6] [msg8] ...
└─ Partition 2 : [msg4] [msg7] [msg9] ...
```

**Points clés :**
- Nom unique dans le cluster
- Divisé en **partitions** pour la scalabilité
- Conservation configurable (temps ou taille)

### 3.2 Partitions

**Concept crucial de Kafka !**

- Chaque topic est divisé en **partitions** (minimum 1)
- Une partition = séquence ordonnée et immuable de messages
- Les partitions permettent le **parallélisme**
- Chaque message dans une partition a un **offset** unique

```
Partition 0:  [0] [1] [2] [3] [4] [5] [6] ...
              │                           │
           offset 0                   offset 6
```

**Pourquoi les partitions ?**
1. **Scalabilité** : Distribuer la charge sur plusieurs brokers
2. **Parallélisme** : Plusieurs consommateurs en parallèle
3. **Ordre garanti** : Dans une partition (pas entre partitions)

### 3.3 Offset

- **Offset** = Position d'un message dans une partition
- Numéro séquentiel unique et croissant
- Le consommateur gère son propre offset (différent de RabbitMQ où le broker gère l'acknowledgment)

### 3.4 Replication Factor

```
Topic "paiements" - Replication Factor = 3

Broker 1 (Leader):    [msg1] [msg2] [msg3]
Broker 2 (Follower):  [msg1] [msg2] [msg3]  (réplique)
Broker 3 (Follower):  [msg1] [msg2] [msg3]  (réplique)
```

- Chaque partition a **un leader** et **N-1 followers**
- Le leader gère toutes les lectures/écritures
- Les followers répliquent les données
- En cas de panne du leader, un follower devient leader

---

## 4. Kafka vs RabbitMQ

### 4.1 Tableau Comparatif Détaillé

| Critère | RabbitMQ | Kafka |
|---------|----------|-------|
| **Modèle de messaging** | Queue traditionnelle (AMQP) | Log distribué / Event Stream |
| **Débit** | ~20k msg/sec | ~1M+ msg/sec |
| **Latence** | <1 ms | ~5-10 ms |
| **Persistance** | Optionnelle | Par défaut (tous les messages) |
| **Durée de rétention** | Jusqu'à consommation + ACK | Configurable (ex: 7 jours, 1 mois) |
| **Ordre des messages** | Par queue | Par partition uniquement |
| **Consommation** | Message supprimé après ACK | Message conservé, offset avance |
| **Rejeu (replay)** | ❌ Impossible | ✅ Possible (retour en arrière sur offset) |
| **Routage** | Complexe (exchanges, bindings) | Simple (clé de partition) |
| **Consumer groups** | Basique | Avancé avec rééquilibrage automatique |
| **Cas d'usage** | Task queues, RPC, workflows | Event sourcing, logs, analytics, streaming |

### 4.2 Quand utiliser quoi ?

**Choisir RabbitMQ si :**
- Besoin de routage complexe (topic, fanout, headers exchanges)
- Latence ultra-faible requise
- Tâches courtes à distribuer (workers)
- Pattern request-reply (RPC)
- Messages doivent être supprimés après traitement

**Choisir Kafka si :**
- Volumes massifs de données
- Besoin de rejouer les événements (event sourcing)
- Architecture event-driven
- Streaming analytics en temps réel
- Plusieurs consommateurs indépendants du même flux
- Besoin de conserver l'historique

### 4.3 Architecture des Consommateurs

**RabbitMQ :**
```
Queue "tasks"
    │
    ├──→ Worker 1 (reçoit msg A)
    ├──→ Worker 2 (reçoit msg B)
    └──→ Worker 3 (reçoit msg C)

→ Chaque message à UN seul consommateur
```

**Kafka :**
```
Topic "events" (3 partitions)
    │
    ├─ P0 ──→ Consumer 1 (Consumer Group "app-1")
    ├─ P1 ──→ Consumer 2 (Consumer Group "app-1")
    └─ P2 ──→ Consumer 3 (Consumer Group "app-1")

ET EN PARALLÈLE :

    ├─ P0 ──→ Consumer A (Consumer Group "app-2")
    ├─ P1 ──→ Consumer B (Consumer Group "app-2")
    └─ P2 ──→ Consumer C (Consumer Group "app-2")

→ Plusieurs consumer groups INDÉPENDANTS
→ Chaque groupe reçoit TOUS les messages
```

---

## 5. Kafka Connect

### 5.1 Qu'est-ce que Kafka Connect ?

**Kafka Connect** est un framework pour **connecter Kafka avec des systèmes externes** (bases de données, fichiers, APIs, etc.) sans écrire de code.

```
┌─────────────┐       ┌──────────────┐       ┌─────────────┐
│  Postgres   │──────▶│    KAFKA     │──────▶│ Elasticsearch│
│   (Source)  │       │   CONNECT    │       │    (Sink)   │
└─────────────┘       │              │       └─────────────┘
                      │  Connectors  │
                      │  Transforms  │
                      └──────────────┘
```

### 5.2 Types de Connectors

#### Source Connectors (vers Kafka)
Récupèrent des données depuis des systèmes externes et les publient dans Kafka.

**Exemples :**
- **JDBC Source** : Lit depuis une base de données (MySQL, Postgres, Oracle)
- **Debezium** : Capture de changements de données (CDC - Change Data Capture)
- **File Source** : Lit des fichiers
- **Twitter Source** : Flux Twitter

```java
// Configuration d'un JDBC Source Connector
{
  "name": "postgres-source",
  "config": {
    "connector.class": "io.confluent.connect.jdbc.JdbcSourceConnector",
    "connection.url": "jdbc:postgresql://localhost:5432/mydb",
    "mode": "incrementing",
    "incrementing.column.name": "id",
    "topic.prefix": "postgres-"
  }
}
```

#### Sink Connectors (depuis Kafka)
Consomment des données de Kafka et les écrivent dans des systèmes externes.

**Exemples :**
- **JDBC Sink** : Écrit dans une base de données
- **Elasticsearch Sink** : Indexe dans Elasticsearch
- **S3 Sink** : Sauvegarde dans AWS S3
- **HDFS Sink** : Écrit dans Hadoop

```java
// Configuration d'un Elasticsearch Sink Connector
{
  "name": "elastic-sink",
  "config": {
    "connector.class": "io.confluent.connect.elasticsearch.ElasticsearchSinkConnector",
    "topics": "commandes",
    "connection.url": "http://localhost:9200",
    "type.name": "_doc"
  }
}
```

### 5.3 Architecture Kafka Connect

```
┌────────────────────────────────────────────┐
│         Kafka Connect Cluster               │
│                                             │
│  ┌────────────┐  ┌────────────┐            │
│  │  Worker 1  │  │  Worker 2  │            │
│  │            │  │            │            │
│  │ Source     │  │ Sink       │            │
│  │ Connector  │  │ Connector  │            │
│  │   Task 1   │  │   Task 1   │            │
│  │   Task 2   │  │   Task 2   │            │
│  └────────────┘  └────────────┘            │
└────────────────────────────────────────────┘
```

**Workers :**
- Exécutent les connectors et leurs tasks
- Mode standalone (1 worker) ou distributed (cluster)

**Tasks :**
- Unités de parallélisme
- Un connector peut avoir plusieurs tasks

### 5.4 Avantages de Kafka Connect

✅ **Pas de code** : Configuration déclarative (JSON)  
✅ **Scalabilité** : Distribution automatique des tasks  
✅ **Fault tolerance** : Redémarrage automatique en cas d'échec  
✅ **Gestion des offsets** : Automatique et fiable  
✅ **Écosystème riche** : 100+ connectors disponibles  

### 5.5 Kafka Connect vs Code Custom

**Sans Kafka Connect (code Java):**
```java
// Producer manuel
Properties props = new Properties();
props.put("bootstrap.servers", "localhost:9092");
Producer<String, String> producer = new KafkaProducer<>(props);

// Lire depuis DB
ResultSet rs = statement.executeQuery("SELECT * FROM users");
while (rs.next()) {
    ProducerRecord<String, String> record = 
        new ProducerRecord<>("users-topic", rs.getString("id"), rs.getString("data"));
    producer.send(record);
}
// Gérer les offsets, les erreurs, la scalabilité...
```

**Avec Kafka Connect:**
```json
{
  "name": "jdbc-source",
  "config": {
    "connector.class": "JdbcSourceConnector",
    "connection.url": "jdbc:mysql://localhost/mydb",
    "mode": "incrementing",
    "incrementing.column.name": "id",
    "topic.prefix": "db-"
  }
}
```

---

## 6. Cas d'Usage Pratiques

### 6.1 Event Sourcing

**Scénario :** Application bancaire

```
Transaction créée ──▶ Kafka Topic "transactions"
                              │
                              ├──▶ Service comptabilité
                              ├──▶ Service notifications
                              ├──▶ Service analytics
                              └──▶ Data Lake (archivage)
```

**Avantage :** L'événement est conservé, peut être rejoué pour reconstruire l'état.

### 6.2 CDC (Change Data Capture) avec Kafka Connect

**Scénario :** Synchronisation de bases de données

```
┌──────────────┐
│ Postgres     │ ──▶ Debezium Source Connector
│ (Write DB)   │              │
└──────────────┘              ▼
                         Kafka Topic
                              │
                              ▼
                    Elasticsearch Sink
                              │
                              ▼
                      ┌──────────────┐
                      │Elasticsearch │
                      │  (Read DB)   │
                      └──────────────┘
```

**Résultat :** Toute modification dans Postgres est automatiquement répliquée dans Elasticsearch en temps réel.

### 6.3 Microservices Event-Driven

**Scénario :** E-commerce

```
Service Commande ──▶ Topic "order-created"
                              │
                              ├──▶ Service Stock (décrémenter)
                              ├──▶ Service Paiement (facturer)
                              ├──▶ Service Notification (email)
                              └──▶ Service Analytics (stats)
```

**Pattern Saga :** Chaque service publie son résultat, permettant une orchestration événementielle.

### 6.4 Log Aggregation

**Scénario :** Centralisation des logs de microservices

```
App1 logs ──┐
App2 logs ──┼──▶ Kafka Topic "logs" ──▶ Elasticsearch
App3 logs ──┘                        └──▶ Kibana (visualisation)
```

---

## 7. Points Clés pour l'Entretien

### 7.1 Message d'Ouverture

> "Je n'ai pas eu l'opportunité de travailler directement avec Kafka en production, mais j'ai une solide expérience avec RabbitMQ qui m'a permis de bien comprendre les systèmes de messaging distribués. J'ai étudié Kafka en profondeur et je comprends ses différences fondamentales : architecture log-based, partitionnement pour la scalabilité, et persistance durable des événements."

### 7.2 Questions Probables et Réponses

**Q : Quelle est la différence entre Kafka et RabbitMQ ?**

**R :** "Les principales différences sont :
- **Architecture** : Kafka est un log distribué tandis que RabbitMQ est un broker traditionnel
- **Persistance** : Kafka conserve tous les messages avec une rétention configurable, RabbitMQ les supprime après consommation
- **Modèle** : Kafka utilise un modèle pull où les consommateurs tirent les messages, RabbitMQ pousse les messages
- **Scalabilité** : Kafka gère beaucoup mieux les très hauts débits grâce au partitionnement
- **Rejeu** : Avec Kafka on peut rejouer les événements en revenant en arrière sur l'offset, impossible avec RabbitMQ

Dans mon expérience avec RabbitMQ, j'ai géré des workflows asynchrones et du task queuing. Kafka serait plus adapté pour de l'event sourcing ou du streaming de données massives."

---

**Q : Expliquez le concept de partition dans Kafka.**

**R :** "Une partition est une subdivision d'un topic qui permet le parallélisme et la scalabilité. Chaque partition :
- Est une séquence ordonnée et immuable de messages
- A un offset unique pour chaque message
- Est répliquée sur plusieurs brokers pour la haute disponibilité
- Peut être consommée par un seul consommateur dans un consumer group

Le nombre de partitions détermine le niveau de parallélisme possible. Par exemple, avec 3 partitions, on peut avoir jusqu'à 3 consommateurs en parallèle dans un même consumer group."

---

**Q : Qu'est-ce que Kafka Connect et pourquoi l'utiliser ?**

**R :** "Kafka Connect est un framework qui simplifie l'intégration de Kafka avec des systèmes externes. Au lieu d'écrire du code custom pour lire/écrire depuis des bases de données ou des APIs, on configure des connectors :

- **Source Connectors** récupèrent des données (ex: JDBC pour lire une DB)
- **Sink Connectors** écrivent des données (ex: Elasticsearch Sink)

Les avantages sont :
- Configuration déclarative sans code
- Gestion automatique de la scalabilité et des erreurs
- Écosystème riche de connectors prêts à l'emploi

C'est similaire conceptuellement aux plugins RabbitMQ, mais beaucoup plus puissant pour l'intégration de données."

---

**Q : Comment garantir l'ordre des messages dans Kafka ?**

**R :** "L'ordre est garanti **au niveau de la partition**, pas du topic entier. Pour garantir l'ordre :
1. Utiliser une clé de partition cohérente (ex: userId)
2. Les messages avec la même clé iront dans la même partition
3. Un seul consommateur traite une partition dans un consumer group

Par exemple, pour des commandes clients : utiliser customerId comme clé garantit que toutes les commandes d'un client sont traitées dans l'ordre."

---

**Q : Quelle est votre approche pour apprendre Kafka rapidement ?**

**R :** "Je combine plusieurs approches :
1. Comprendre les concepts fondamentaux et l'architecture
2. Faire le parallèle avec mes connaissances RabbitMQ
3. Mettre en place un environnement local pour tester
4. Étudier des cas d'usage concrets proches de nos besoins métier
5. Lire la documentation officielle et les best practices

Ma compréhension des systèmes distribués et de la messagerie asynchrone avec RabbitMQ me donne une excellente base pour être opérationnel rapidement sur Kafka."

---

### 7.3 Montrer votre Capacité d'Apprentissage

**Points à souligner :**

✅ "J'ai une base solide en messaging avec RabbitMQ"  
✅ "Je comprends les différences architecturales fondamentales"  
✅ "Je vois les avantages de Kafka pour l'event sourcing et le streaming"  
✅ "Je suis prêt à mettre les mains dans le code rapidement"  

### 7.4 Questions à Poser à l'Interviewer

Montrez votre intérêt :

1. "Quel est votre use case principal pour Kafka dans l'équipe ?"
2. "Utilisez-vous Kafka Connect pour l'intégration ou du code custom ?"
3. "Quelle est votre stratégie de partitionnement ?"
4. "Comment gérez-vous le monitoring et l'observabilité de Kafka ?"

---

## 8. Aide-Mémoire Rapide

### Vocabulaire Clé

- **Broker** : Serveur Kafka
- **Topic** : Catégorie de messages
- **Partition** : Subdivision d'un topic
- **Offset** : Position d'un message dans une partition
- **Producer** : Publie des messages
- **Consumer** : Lit des messages
- **Consumer Group** : Groupe de consommateurs partageant la charge
- **Replication Factor** : Nombre de copies d'une partition
- **Leader** : Broker gérant les lectures/écritures d'une partition
- **Follower** : Broker répliquant une partition

### Commandes Utiles (à connaître)

```bash
# Créer un topic
kafka-topics --create --topic mon-topic --partitions 3 --replication-factor 2

# Lister les topics
kafka-topics --list

# Décrire un topic
kafka-topics --describe --topic mon-topic

# Produire un message
kafka-console-producer --topic mon-topic

# Consommer des messages
kafka-console-consumer --topic mon-topic --from-beginning

# Lister les consumer groups
kafka-consumer-groups --list

# Décrire un consumer group
kafka-consumer-groups --describe --group mon-groupe
```

### Configuration Producer (Java)

```java
Properties props = new Properties();
props.put("bootstrap.servers", "localhost:9092");
props.put("key.serializer", "org.apache.kafka.common.serialization.StringSerializer");
props.put("value.serializer", "org.apache.kafka.common.serialization.StringSerializer");
props.put("acks", "all"); // Attendre confirmation de toutes les répliques

KafkaProducer<String, String> producer = new KafkaProducer<>(props);
ProducerRecord<String, String> record = new ProducerRecord<>("mon-topic", "cle", "valeur");
producer.send(record);
```

### Configuration Consumer (Java)

```java
Properties props = new Properties();
props.put("bootstrap.servers", "localhost:9092");
props.put("group.id", "mon-consumer-group");
props.put("key.deserializer", "org.apache.kafka.common.serialization.StringDeserializer");
props.put("value.deserializer", "org.apache.kafka.common.serialization.StringDeserializer");
props.put("enable.auto.commit", "true");
props.put("auto.offset.reset", "earliest"); // ou "latest"

KafkaConsumer<String, String> consumer = new KafkaConsumer<>(props);
consumer.subscribe(Arrays.asList("mon-topic"));

while (true) {
    ConsumerRecords<String, String> records = consumer.poll(Duration.ofMillis(100));
    for (ConsumerRecord<String, String> record : records) {
        System.out.printf("offset = %d, key = %s, value = %s%n", 
                          record.offset(), record.key(), record.value());
    }
}
```

---

## 9. Ressources pour Approfondir

### Documentation Officielle
- [Kafka Documentation](https://kafka.apache.org/documentation/)
- [Confluent Documentation](https://docs.confluent.io/)

### Tutoriels Pratiques
- Kafka Quickstart (kafka.apache.org)
- Confluent Developer (developer.confluent.io)

### Comparaisons
- "Kafka vs RabbitMQ" - Articles techniques
- "When to use Kafka vs Message Queue"

---

## Conclusion

Vous avez maintenant une base solide pour discuter de Kafka en entretien. Les points essentiels :

1. ✅ **Architecture** : Brokers, Topics, Partitions, Réplication
2. ✅ **Différences RabbitMQ** : Log vs Queue, Persistance, Scalabilité
3. ✅ **Kafka Connect** : Intégration sans code avec systèmes externes
4. ✅ **Use Cases** : Event sourcing, CDC, Microservices

**Votre atout :** Votre expérience RabbitMQ vous donne une excellente compréhension des systèmes de messaging. Mettez en avant votre capacité à faire des parallèles et votre volonté d'apprendre.

Bonne chance pour votre entretien ! 🚀
