## [**..**](./00_index.md)

# Observability & Monitoring de Microservices Spring Boot


---

## 🔍 Logging & Centralisation des Logs

### Implémentation avec SL4J
- Configuration de **SL4J** comme facade de logging dans les microservices Spring Boot
- Définition de niveaux de logs appropriés (DEBUG, INFO, WARN, ERROR) par package et environnement
- Structuration des logs avec des **MDC (Mapped Diagnostic Context)** pour injecter des informations contextuelles (userId, correlationId, requestId)
- **Exemple** : Ajout automatique du `traceId` dans chaque log pour corréler les événements d'une même requête distribuée

### Stockage et Rotation des Logs
- Configuration du stockage des logs dans des fichiers avec **Logback** ou **Log4j2**
- Mise en place de politiques de rotation automatique (par taille, par date)
- Archivage et compression des logs historiques pour optimiser l'espace disque
- **Exemple** : Rotation quotidienne avec conservation de 30 jours en production, compression gzip des archives

---

## 📦 ELK Stack - Solution Complète de Gestion des Logs

### Architecture ELK Distribuée

#### Filebeat - Collecte Légère des Logs
- Déploiement de **Filebeat** comme agent léger de collecte sur chaque nœud applicatif
- Configuration des **inputs** pour monitorer les fichiers de logs Spring Boot (application.log, error.log)
- Mise en place de **multiline patterns** pour gérer les stack traces Java sur plusieurs lignes
- Configuration du **backpressure** et **buffering** pour gérer les pics de charge
- **Exemple** : Configuration Filebeat pour collecter les logs depuis `/var/log/app/*.log` avec parsing automatique des stack traces Java multilignes

#### Logstash - Pipeline de Transformation
- Conception de **pipelines Logstash** pour enrichir, filtrer et transformer les logs en temps réel
- Utilisation de **filters** avancés :
  - **Grok patterns** pour parser les logs Spring Boot et extraire les champs structurés (timestamp, level, thread, logger, message)
  - **Mutate** pour ajouter/supprimer/renommer des champs
  - **Date** pour parser et normaliser les timestamps
  - **GeoIP** pour enrichir avec la géolocalisation des IPs
  - **Dissect** pour parsing rapide de formats simples
- Mise en place de **conditionals** pour router les logs selon leur contenu
- **Exemple concret de pipeline** :
  ```
  Input: "[2024-11-01 10:30:45] INFO [order-service,a1b2c3,d4e5f6] OrderController - Order created: orderId=12345, userId=user789, amount=150.50"
  
  Après Grok: 
  - timestamp: 2024-11-01 10:30:45
  - level: INFO
  - service: order-service
  - traceId: a1b2c3
  - spanId: d4e5f6
  - class: OrderController
  - orderId: 12345
  - userId: user789
  - amount: 150.50
  ```

#### Elasticsearch - Stockage et Indexation
- Configuration de **cluster Elasticsearch** multi-nœuds pour haute disponibilité
- Définition d'**index templates** et **mappings** optimisés pour les logs Spring Boot
- Mise en place d'**Index Lifecycle Management (ILM)** :
  - **Hot phase** : indices récents (0-7 jours) sur SSD, toutes les réplicas
  - **Warm phase** : indices anciens (7-30 jours) sur disques standards, réduction des réplicas
  - **Cold phase** : indices archivés (30-90 jours) en read-only
  - **Delete phase** : suppression après 90 jours
- Configuration de **sharding** et **replication** adaptés au volume de logs
- Optimisation des performances avec **bulk indexing** et **refresh intervals**
- **Exemple** : Index pattern `logs-spring-boot-{env}-{date}` avec rollover automatique à 50GB ou 30 jours

#### Kibana - Visualisation et Analyse
- Création de **dashboards Kibana** interactifs pour l'analyse des logs :
  - **Overview Dashboard** : volume de logs par service, distribution par level, top errors
  - **Error Analysis Dashboard** : stack traces les plus fréquentes, error trends, impact par service
  - **Performance Dashboard** : latency distributions, slow queries, API response times
  - **Business Dashboard** : metrics métier extraites des logs (orders/hour, payment success rate)
- Configuration de **Discover** avec saved searches pour les requêtes fréquentes
- Mise en place de **Canvas** pour des présentations visuelles avancées
- **Exemple** : Dashboard temps réel montrant les erreurs 5xx par microservice avec drill-down vers les stack traces complètes

### Kibana Alerting & Watchers
- Configuration d'**alertes Kibana** basées sur des requêtes Elasticsearch
- Mise en place de **Watchers** (X-Pack) pour surveillance proactive :
  - Détection d'anomalies dans les patterns de logs
  - Alertes sur augmentation soudaine des erreurs
  - Monitoring de la disponibilité des services
- **Exemple d'alerte** : Trigger si plus de 50 erreurs 5xx en 5 minutes sur le service payment → notification Slack avec extraits de logs et stack traces

### Recherche et Debugging Avancés
- Utilisation du **Query DSL Elasticsearch** pour recherches complexes :
  - **Bool queries** : combinaison de must/should/must_not
  - **Range queries** : filtrage temporel précis
  - **Wildcard/Regex** : recherche de patterns dans les messages
  - **Aggregations** : statistiques et analyses groupées
- **Exemple de requête** : Trouver tous les logs ERROR du service order-service contenant "NullPointerException" dans les 2 dernières heures, groupés par endpoint

---

## 🔄 Comparaison ELK vs Grafana Stack

### ELK Stack (Elastic, Logstash, Kibana, Filebeat)
**Avantages** :
- **Elasticsearch** : moteur de recherche full-text extrêmement puissant
- **Kibana** : UI riche pour exploration et analyse des logs
- Écosystème mature avec large communauté
- Excellent pour queries complexes et analyse de grandes volumétries

**Use cases privilégiés** :
- Debugging approfondi avec recherches full-text complexes
- Analyse forensique de logs historiques
- Compliance et audit trails
- Environnements avec volumes de logs très élevés (TB/jour)

### Grafana Stack (Loki, Promtail, Tempo)

#### Stack Grafana Loki & Promtail
- Déploiement de **Loki** comme système de stockage centralisé de logs, scalable et performant
- Installation et configuration de **Promtail** comme agent de collecte sur chaque nœud applicatif
- Définition de **labels** pertinents (environment, service_name, instance, level) pour faciliter le filtrage
- Création de requêtes **LogQL** avancées pour analyser les logs en temps réel
- **Exemple** : Requête LogQL pour identifier tous les logs ERROR d'un microservice spécifique sur les 15 dernières minutes avec filtrage par keyword

**Avantages** :
- **Coût de stockage réduit** : indexation uniquement des labels, pas du contenu
- **Performance** : requêtes plus rapides pour cas d'usage ciblés
- **Simplicité** : architecture plus légère qu'ELK
- **Intégration native** avec Prometheus et Tempo dans Grafana

**Use cases privilégiés** :
- Monitoring opérationnel en temps réel
- Corrélation logs/metrics/traces dans une UI unique
- Environnements cloud-native et Kubernetes
- Budget et ressources limités

### Approche Hybride (Best of Both Worlds)
Dans certains projets, j'ai implémenté une **approche hybride** :
- **ELK** : pour logs applicatifs détaillés, debugging, compliance (rétention longue)
- **Grafana Loki** : pour logs système, monitoring temps réel, alerting opérationnel
- **Exemple architecture** : Filebeat → Logstash → (Elasticsearch + Loki) avec routage intelligent selon le type de log

---

## 📊 Metrics & Monitoring Applicatif

### Intégration Micrometer & Prometheus
- Configuration de **Micrometer** comme abstraction de metrics dans Spring Boot via `spring-boot-starter-actuator`
- Exposition de l'endpoint `/actuator/prometheus` pour le scraping des métriques
- Instrumentation automatique des métriques standards : JVM (heap, threads, GC), HTTP requests (latency, throughput, error rate), DB connections, cache hit ratio
- **Exemple** : Monitoring de la mémoire heap avec alertes si utilisation > 85%

### Métriques Custom
- Création de **custom metrics** métier avec `@Timed`, `Counter`, `Gauge`, `Timer`
- **Exemple concret** : 
  - `Counter` pour compter le nombre de commandes créées : `order.created.total`
  - `Timer` pour mesurer la latence des appels à une API externe : `external.api.payment.duration`
  - `Gauge` pour monitorer la taille d'une queue de messages : `message.queue.size`

### Configuration Prometheus
- Installation et configuration de **Prometheus** avec définition des jobs de scraping
- Configuration du `scrape_interval` optimisé (15-30s selon les besoins)
- Mise en place de **service discovery** (Kubernetes, Consul) pour découvrir automatiquement les instances
- **Exemple** : Configuration de scraping pour tous les pods Spring Boot labelisés `app=payment-service` dans Kubernetes

### Dashboards Grafana
- Création de **dashboards personnalisés** pour chaque microservice avec panneaux pertinents :
  - **Golden Signals** : latency, traffic, errors, saturation
  - **JVM Metrics** : heap usage, GC pause time, thread count
  - **Business Metrics** : transactions/min, conversion rate, active users
- Utilisation de dashboards communautaires (Spring Boot Statistics, JVM Micrometer)
- Organisation en **folders** par domaine (Infrastructure, Application, Business)
- **Exemple** : Dashboard "Payment Service Overview" avec panels pour error rate 5xx, p95 latency, transaction volume, et corrélation avec les déploiements

### Elastic APM (Alternative pour Metrics & Tracing)
- Déploiement d'**Elastic APM Server** pour monitoring des performances applicatives
- Intégration de l'**agent APM Java** dans les microservices Spring Boot
- Collection automatique de :
  - **Transactions** : latency, throughput par endpoint
  - **Spans** : détail des opérations (DB queries, HTTP calls, méthodes)
  - **Errors** : stack traces et contexte complet
  - **Metrics JVM** : intégré dans la même UI Kibana
- Visualisation dans **Kibana APM UI** avec service maps et dependency graphs
- **Exemple** : Service map montrant les dépendances entre microservices avec latencies moyennes et error rates par lien

---

## 🚨 Alerting & Notifications

### Stratégie d'Alerting
- Définition d'**alert rules** dans Grafana basées sur des seuils critiques et des tendances
- Classification des alertes par sévérité (Critical, Warning, Info)
- Mise en place de périodes de **grace period** et **repeat interval** pour éviter le spam
- **Exemples d'alertes** :
  - **Critical** : Error rate > 5% pendant 5 minutes
  - **Critical** : Latency p99 > 2s pendant 3 minutes
  - **Warning** : Memory heap usage > 80% pendant 10 minutes
  - **Critical** : Service down (pas de scrape depuis 2 minutes)

### Notification Multi-canaux
- Configuration de **notification channels** : Slack, Email, PagerDuty, Microsoft Teams
- Routing différencié selon la sévérité (Critical → PagerDuty + Slack, Warning → Slack seulement)
- Enrichissement des notifications avec contexte (service, environment, runbook links)
- **Exemple** : Alerte Slack avec message formaté incluant le lien direct vers le dashboard Grafana et les logs Loki/Kibana filtrés sur la période d'incident

### Alerting dans ELK Stack
- Configuration de **Kibana Alerts** pour surveillance des logs
- Utilisation d'**Elasticsearch Watchers** pour alertes complexes basées sur aggregations
- **Exemple Watcher** : Détecter si le taux d'erreur sur les paiements dépasse 3% sur 10 minutes → exécuter webhook vers système de ticketing

---

## 🔗 Distributed Tracing

### Implémentation avec OpenTelemetry & Tempo
- Intégration d'**OpenTelemetry** (standard CNCF) comme instrumentation de tracing dans Spring Boot
- Configuration automatique via `spring-cloud-starter-sleuth` et `opentelemetry-java-instrumentation`
- Propagation du **trace context** (traceId, spanId) à travers les appels HTTP, messaging (Kafka, RabbitMQ), et requêtes DB
- Déploiement de **Grafana Tempo** comme backend de stockage de traces distribué et performant

### Elastic APM pour Distributed Tracing
- Alternative avec **Elastic APM** pour tracing distribué dans l'écosystème ELK
- Visualisation des traces dans **Kibana APM** avec timeline détaillée
- **Service Maps** automatiques montrant les dépendances et flows de requêtes
- Intégration native avec les logs Elasticsearch pour corrélation immédiate
- **Exemple** : Clic sur une transaction lente dans APM → affichage du span breakdown → clic sur "View Logs" → logs filtrés automatiquement par traceId dans Kibana Discover

### Analyse des Traces
- Visualisation du **flamegraph** complet d'une requête traversant 5-6 microservices
- Identification des **bottlenecks** et latences anormales dans la chaîne d'appels
- Corrélation automatique traces ↔ logs ↔ metrics via le traceId
- **Exemple concret** : 
  - Requête utilisateur : `POST /api/orders`
  - Trace montrant : API Gateway (50ms) → Order Service (120ms) → Inventory Service (800ms ⚠️) → Payment Service (200ms) → Notification Service (100ms)
  - Identification immédiate que Inventory Service est le bottleneck avec une query DB lente
  - Drill-down vers les logs spécifiques du span problématique

### Sampling Strategy
- Configuration de **sampling** adapté (probabiliste, rate-limiting) pour gérer le volume en production
- **Exemple** : Sampling à 10% en production normale, 100% pour les requêtes en erreur ou lentes (> 1s)
- Dans Elastic APM : configuration du sampling rate par transaction type

---

## 🔧 Intégration et Bonnes Pratiques

### Corrélation Logs - Metrics - Traces
- Implémentation d'un **correlation ID** (traceId) injecté dans tous les logs via MDC
- Configuration des liens directs entre Grafana panels et logs correspondants
- **Exemple workflow de debugging** :
  1. Alerte Grafana : latency p99 élevée sur order-service
  2. Clic sur lien → Dashboard avec métriques détaillées
  3. Identification du timeframe problématique
  4. Clic "Explore Logs" → Loki/Kibana filtré automatiquement par service + timeframe
  5. Clic sur un log avec traceId → Visualisation de la trace complète dans Tempo/APM
  6. Identification de la root cause en 2 minutes

### Structured Logging
- Adoption du **structured logging** (JSON format) pour faciliter le parsing
- Utilisation de **Logstash JSON Encoder** ou **Logback JSON Layout**
- **Exemple** : 
  ```json
  {
    "timestamp": "2024-11-01T10:30:45.123Z",
    "level": "ERROR",
    "service": "payment-service",
    "traceId": "a1b2c3d4e5f6",
    "spanId": "x7y8z9",
    "thread": "http-nio-8080-exec-5",
    "logger": "com.example.PaymentController",
    "message": "Payment processing failed",
    "exception": "java.sql.SQLException: Connection timeout",
    "userId": "user123",
    "orderId": "order456",
    "amount": 150.50
  }
  ```

### Environnements Multiples
- Configuration différenciée par environnement (dev, staging, prod)
- **Dev** : logs verbeux (DEBUG), rétention courte (7 jours), sampling 100%
- **Staging** : logs INFO, rétention moyenne (30 jours), sampling 50%
- **Production** : logs INFO/WARN/ERROR, rétention longue (90 jours), sampling 10-20%

---

## 🎯 Bénéfices & Résultats

Cette implémentation complète d'observability avec double expertise **ELK** et **Grafana Stack** permet :
- **MTTD (Mean Time To Detect)** réduit de 80% grâce aux alertes proactives
- **MTTR (Mean Time To Resolve)** diminué de 60-70% grâce à la corrélation logs/metrics/traces
- **Visibilité end-to-end** sur l'ensemble de l'architecture distribuée
- **Capacity planning** facilité grâce aux métriques historiques et analyses de tendances
- **Amélioration continue** des performances basée sur des données observables
- **Root cause analysis** accélérée avec navigation fluide entre logs/metrics/traces
- **Choix technologique adapté** : capacité à recommander ELK vs Grafana selon contexte (volumétrie, budget, use cases)

---
# Questions/Réponses - Observability & Monitoring Spring Boot

## 🔍 Questions Générales sur l'Observability

### Q1: Qu'est-ce que l'observability et en quoi diffère-t-elle du monitoring traditionnel?

**R:** L'**observability** est la capacité à comprendre l'état interne d'un système en analysant ses outputs externes. Elle repose sur trois piliers fondamentaux :
- **Logs** : enregistrements discrets d'événements
- **Metrics** : mesures agrégées dans le temps
- **Traces** : suivi des requêtes à travers les systèmes distribués

La différence avec le monitoring traditionnel :
- **Monitoring** : répond à des questions connues à l'avance ("Le service est-il up?", "Quelle est la latency moyenne?")
- **Observability** : permet d'explorer et de comprendre des problèmes inconnus, d'investiguer le "pourquoi" derrière les métriques

**Exemple concret** : Le monitoring vous dit "le service a 5% d'erreurs", l'observability vous permet de tracer une requête spécifique qui a échoué, voir tous ses logs, identifier le microservice problématique et la query DB lente responsable.

---

### Q2: Pourquoi avez-vous choisi d'utiliser SL4J plutôt que de logger directement avec Log4j ou Logback?

**R:** **SL4J (Simple Logging Facade for Java)** est une facade/abstraction qui offre plusieurs avantages critiques :

1. **Indépendance de l'implémentation** : Le code applicatif n'est pas couplé à une bibliothèque de logging spécifique
2. **Flexibilité** : Possibilité de changer l'implémentation (Logback, Log4j2, JUL) sans modifier le code
3. **Standardisation** : API simple et uniforme pour toute l'équipe
4. **Performance** : Lazy evaluation avec parameterized messages

**Exemple** :
```java
// Mauvaise pratique - concaténation coûteuse même si DEBUG désactivé
logger.debug("Order details: " + order.toString());

// Bonne pratique SL4J - évaluation paresseuse
logger.debug("Order details: {}", order);
// Le toString() n'est appelé que si DEBUG est activé
```

Dans mes projets, j'utilise **SL4J** comme facade avec **Logback** comme implémentation par défaut (natif dans Spring Boot), permettant une migration future facile si nécessaire.

---

## 📦 Questions sur ELK Stack

### Q3: Expliquez l'architecture complète d'une stack ELK pour microservices Spring Boot

**R:** L'architecture ELK typique que j'implémente suit ce flow :

```
Spring Boot App (SL4J) 
    ↓ (écrit dans fichiers)
/var/log/app/application.log
    ↓ (collecte)
Filebeat (agent léger)
    ↓ (shipping)
Logstash (transformation/enrichissement)
    ↓ (indexation)
Elasticsearch (stockage/recherche)
    ↓ (visualisation)
Kibana (UI/dashboards)
```

**Détails de chaque composant** :
- **Filebeat** : Agent léger (~50MB RAM) qui lit les fichiers de logs, gère le backpressure, garantit la livraison
- **Logstash** : Pipeline de transformation avec filtres Grok pour parser, enrichir (GeoIP, date), filtrer et router
- **Elasticsearch** : Cluster de stockage distribué avec sharding, replication, et index lifecycle management
- **Kibana** : Interface web pour recherche, visualisation, dashboards et alerting

**Alternative simplifiée** : Pour des volumes modérés, Filebeat peut envoyer directement vers Elasticsearch en bypassant Logstash.

---

### Q4: Comment gérez-vous les stack traces Java multilignes dans ELK?

**R:** Les stack traces Java sont un défi majeur car elles s'étendent sur plusieurs lignes mais représentent un seul événement. J'utilise plusieurs techniques :

**1. Configuration Filebeat avec multiline pattern** :
```yaml
filebeat.inputs:
- type: log
  paths:
    - /var/log/app/*.log
  multiline.type: pattern
  multiline.pattern: '^[0-9]{4}-[0-9]{2}-[0-9]{2}'
  multiline.negate: true
  multiline.match: after
```
Ce pattern regroupe toutes les lignes qui ne commencent pas par une date avec la ligne précédente.

**2. Grok pattern Logstash pour parser la stack trace complète** :
```ruby
filter {
  grok {
    match => { 
      "message" => "%{TIMESTAMP_ISO8601:timestamp} %{LOGLEVEL:level}.*?(?<exception>.*Exception[^\n]*(?:\n\s+at .*)*)"
    }
  }
}
```

**Résultat** : Une erreur avec 50 lignes de stack trace devient un seul document Elasticsearch avec un champ `exception` contenant toute la trace, facilitant la recherche et l'analyse.

---

### Q5: Expliquez votre stratégie d'Index Lifecycle Management (ILM) dans Elasticsearch

**R:** L'**ILM** est crucial pour optimiser coûts de stockage et performances. Ma stratégie typique :

**Hot Phase (0-7 jours)** :
- Indices sur SSD haute performance
- 2 réplicas pour haute disponibilité
- Rollover à 50GB ou 1 jour
- Priorité d'allocation : 100
- **Use case** : Recherches fréquentes, debugging actif

**Warm Phase (7-30 jours)** :
- Migration vers disques standards
- Réduction à 1 replica
- Force merge pour optimisation
- Priorité : 50
- **Use case** : Recherches occasionnelles, analyse de tendances

**Cold Phase (30-90 jours)** :
- Indices en read-only
- Migration vers stockage économique
- 0 replicas (searchable snapshots)
- Priorité : 0
- **Use case** : Compliance, audit historique

**Delete Phase (> 90 jours)** :
- Suppression automatique
- **Exception** : Logs de sécurité conservés 1 an pour compliance

**Exemple de configuration** :
```json
PUT _ilm/policy/logs-policy
{
  "policy": {
    "phases": {
      "hot": {
        "actions": {
          "rollover": {
            "max_size": "50GB",
            "max_age": "1d"
          }
        }
      },
      "warm": {
        "min_age": "7d",
        "actions": {
          "shrink": { "number_of_shards": 1 },
          "forcemerge": { "max_num_segments": 1 }
        }
      }
    }
  }
}
```

**Bénéfices** : Réduction de 70% des coûts de stockage sur un projet avec 5TB de logs/mois.

---

### Q6: Comment construisez-vous un Grok pattern pour parser des logs Spring Boot personnalisés?

**R:** Processus que j'utilise pour créer des Grok patterns efficaces :

**1. Analyser un exemple de log réel** :
```
2024-11-01 14:23:45.123 INFO [order-service,a1b2c3d4,e5f6g7h8] 12345 --- [nio-8080-exec-5] c.e.OrderController : Processing order - orderId=ORD-12345, userId=USR-789, amount=250.50, status=SUCCESS
```

**2. Identifier les patterns standards** :
- Date/Time : `TIMESTAMP_ISO8601` ou custom
- Level : `LOGLEVEL`
- Thread : `\[.*?\]`
- Class : `(?:[a-zA-Z$_][a-zA-Z$_0-9]*\.)*[a-zA-Z$_][a-zA-Z$_0-9]*`

**3. Construire le pattern progressivement** :
```ruby
%{TIMESTAMP_ISO8601:timestamp} %{LOGLEVEL:level} \[%{DATA:service},%{DATA:traceId},%{DATA:spanId}\] %{NUMBER:pid} --- \[%{DATA:thread}\] %{DATA:logger} : %{GREEDYDATA:message}
```

**4. Extraire les données métier du message** :
```ruby
grok {
  match => { 
    "message" => "Processing order - orderId=%{DATA:orderId}, userId=%{DATA:userId}, amount=%{NUMBER:amount:float}, status=%{WORD:status}"
  }
}
```

**5. Tester avec Grok Debugger** dans Kibana Dev Tools

**Résultat** : Chaque champ devient searchable et utilisable pour aggregations dans Kibana.

**Astuce** : Créer une bibliothèque de patterns réutilisables pour l'équipe.

---

### Q7: Quelle est la différence entre Filebeat et Logstash, et quand utilisez-vous l'un ou l'autre?

**R:** **Filebeat** et **Logstash** ont des rôles complémentaires mais différents :

**Filebeat** :
- **Nature** : Agent léger de shipping (~50MB RAM, minimal CPU)
- **Rôle** : Collecte et forward des logs avec garantie de livraison
- **Capacités** : Parsing basique, multiline, some processors
- **Déploiement** : Un par machine/pod
- **Avantages** : Très léger, backpressure natif, faible overhead

**Logstash** :
- **Nature** : Pipeline de transformation lourd (~1GB RAM)
- **Rôle** : Transformation complexe, enrichissement, filtrage, routing
- **Capacités** : 200+ plugins, Grok, GeoIP, aggregations, conditionals
- **Déploiement** : Centralisé (3-5 instances)
- **Avantages** : Transformations puissantes, flexibilité maximale

**Mes stratégies de déploiement** :

**Architecture 1 - Haute volumétrie** :
```
Filebeat → Logstash → Elasticsearch
```
- Filebeat collecte et shippe
- Logstash parse et enrichit (parsing complexe, GeoIP, lookups)
- Idéal pour > 100GB logs/jour

**Architecture 2 - Volumétrie modérée** :
```
Filebeat (avec modules) → Elasticsearch
```
- Filebeat parse directement avec modules prédéfinis
- Bypass Logstash pour réduire complexité
- Idéal pour < 50GB logs/jour, patterns standards

**Architecture 3 - Hybride** :
```
Filebeat (parsing simple) → Logstash (enrichissement) → Elasticsearch
```
- Filebeat fait le parsing de base
- Logstash fait seulement l'enrichissement GeoIP et routing
- Équilibre optimal pour 50-100GB logs/jour

**Exemple concret** : Sur un projet e-commerce, j'ai utilisé Filebeat avec modules pour les logs applicatifs simples (90% du volume), et Logstash uniquement pour les logs de paiement nécessitant un enrichissement complexe avec lookup de données client (10% du volume).

---

## 🔄 Questions ELK vs Grafana

### Q8: Quand recommandez-vous ELK plutôt que Grafana Loki, et vice-versa?

**R:** Le choix dépend de plusieurs facteurs critiques :

**Recommandation ELK Stack quand** :
1. **Volumétrie très élevée** : > 500GB logs/jour avec besoin de rétention longue
2. **Recherche full-text complexe** : Regex avancées, fuzzy search, phrase matching
3. **Analyse forensique** : Investigations approfondies, compliance, audit trails
4. **Logs structurés riches** : Beaucoup de champs searchables (50+)
5. **Équipe déjà formée** sur Elasticsearch
6. **Budget infrastructure** suffisant

**Exemple** : Application bancaire avec compliance nécessitant recherche dans 2 ans d'historique de logs avec 100+ champs indexés.

**Recommandation Grafana Loki quand** :
1. **Cloud-native/Kubernetes** : Intégration native, service discovery automatique
2. **Monitoring opérationnel** : Focus sur alerting temps réel, moins sur forensique
3. **Budget limité** : Coût de stockage 10x inférieur (pas d'indexation full-text)
4. **Corrélation logs-metrics-traces** : UI unique Grafana pour tout
5. **Volumétrie modérée** : < 200GB logs/jour
6. **Simplicité** : Équipe réduite, moins de maintenance

**Exemple** : Startup SaaS avec microservices sur Kubernetes, besoin de monitoring opérationnel, budget serré.

**Approche Hybride que j'ai implémentée** :
- **Loki** : Logs système, monitoring temps réel, debugging opérationnel (rétention 30 jours)
- **ELK** : Logs applicatifs détaillés, transactions financières, audit (rétention 1 an)
- **Routage intelligent** : Filebeat décide selon le type de log

**Tableau comparatif** :

| Critère | ELK | Grafana Loki |
|---------|-----|--------------|
| Coût stockage | Élevé | Faible (10x moins) |
| Recherche full-text | Excellent | Basique |
| Performance queries | Rapide | Très rapide (queries simples) |
| Courbe d'apprentissage | Élevée | Faible |
| Corrélation metrics | Via APM | Native Grafana |
| Scalabilité | Excellente | Bonne |

---

### Q9: Comment implémentez-vous la corrélation entre logs, metrics et traces?

**R:** La corrélation est la clé d'une observability efficace. Voici mon implémentation complète :

**1. Trace Context Propagation** :
```java
// Spring Cloud Sleuth auto-configure le traceId/spanId
// Ajout dans MDC pour SL4J
@Configuration
public class LoggingConfig {
    @Bean
    public MDCPropagator mdcPropagator() {
        return new MDCPropagator();
    }
}
```

**2. Pattern de logs avec traceId** :
```xml
<!-- logback-spring.xml -->
<pattern>
  %d{ISO8601} %level [${spring.application.name},%X{traceId},%X{spanId}] %thread %logger{36} : %msg%n
</pattern>
```

**Résultat log** :
```
2024-11-01 14:23:45 INFO [order-service,a1b2c3d4,e5f6g7h8] http-nio-exec-5 OrderController : Order created
```

**3. Dans Grafana - Lien Metrics → Logs** :
```yaml
# Dashboard panel configuration
{
  "links": [
    {
      "title": "View Logs",
      "url": "/explore?left=[\"now-1h\",\"now\",\"Loki\",{\"expr\":\"{service=\\\"order-service\\\"} |~ \\\"${__field.labels.traceId}\\\"\"}]"
    }
  ]
}
```

**4. Dans Loki - Lien Logs → Traces** :
```yaml
# Loki datasource config
derivedFields:
  - datasourceUid: tempo-uid
    matcherRegex: "traceId=(\\w+)"
    name: TraceID
    url: "$${__value.raw}"
```

**5. Dans Kibana - Corrélation via APM** :
- Elastic APM capture automatiquement traceId
- Clic sur transaction → "View Logs" filtre automatiquement Discover par traceId

**Workflow de debugging complet** :
1. **Alerte Prometheus** : "High error rate on order-service"
2. Clic sur dashboard Grafana → identification période 14:20-14:25
3. Clic "Explore Logs" → logs d'erreur dans Loki
4. Identification d'un traceId récurrent : `a1b2c3d4`
5. Clic sur traceId → Trace complète dans Tempo
6. Flamegraph montre : Order Service (50ms) → Inventory Service (3000ms ⚠️)
7. Drill-down vers logs d'Inventory Service avec ce traceId
8. Identification : `SQLException: Deadlock detected`
9. **Root cause trouvée en 90 secondes**

**Astuce** : J'ajoute également le traceId comme tag dans les custom metrics :
```java
meterRegistry.counter("order.created", 
    Tags.of("traceId", tracer.currentSpan().context().traceId())
).increment();
```

---

## 📊 Questions sur Metrics & Prometheus

### Q10: Expliquez comment vous créez et exposez des custom metrics dans Spring Boot

**R:** Voici mon approche complète pour les custom metrics :

**1. Setup de base** :
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-actuator</artifactId>
</dependency>
<dependency>
    <groupId>io.micrometer</groupId>
    <artifactId>micrometer-registry-prometheus</artifactId>
</dependency>
```

```yaml
# application.yml
management:
  endpoints:
    web:
      exposure:
        include: prometheus,health,info
  metrics:
    tags:
      application: ${spring.application.name}
      environment: ${spring.profiles.active}
```

**2. Injection du MeterRegistry** :
```java
@Service
public class OrderService {
    private final MeterRegistry meterRegistry;
    private final Counter orderCreatedCounter;
    private final Timer paymentProcessingTimer;
    
    public OrderService(MeterRegistry meterRegistry) {
        this.meterRegistry = meterRegistry;
        
        // Counter - pour événements discrets
        this.orderCreatedCounter = Counter.builder("orders.created")
            .description("Total number of orders created")
            .tag("type", "online")
            .register(meterRegistry);
            
        // Timer - pour mesurer latency
        this.paymentProcessingTimer = Timer.builder("payment.processing.duration")
            .description("Time to process payment")
            .publishPercentiles(0.5, 0.95, 0.99)
            .register(meterRegistry);
    }
    
    public Order createOrder(OrderRequest request) {
        // Increment counter
        orderCreatedCounter.increment();
        
        // Mesurer durée avec timer
        return paymentProcessingTimer.record(() -> {
            // Logique de traitement paiement
            return processPayment(request);
        });
    }
}
```

**3. Utilisation de Gauge pour valeurs qui fluctuent** :
```java
@Component
public class QueueMonitor {
    private final BlockingQueue<Message> messageQueue;
    
    public QueueMonitor(MeterRegistry registry, BlockingQueue<Message> queue) {
        this.messageQueue = queue;
        
        Gauge.builder("queue.size", messageQueue, Queue::size)
            .description("Current size of message queue")
            .tag("queue", "orders")
            .register(registry);
    }
}
```

**4. Annotation @Timed pour métriques automatiques** :
```java
@RestController
@Timed // Toutes les méthodes du controller sont mesurées
public class OrderController {
    
    @PostMapping("/orders")
    @Timed(value = "orders.create.time", 
           description = "Time to create order",
           percentiles = {0.5, 0.95, 0.99})
    public ResponseEntity<Order> createOrder(@RequestBody OrderRequest request) {
        return ResponseEntity.ok(orderService.create(request));
    }
}
```

**5. Résultat dans Prometheus** :
```
# HELP orders_created_total Total number of orders created
# TYPE orders_created_total counter
orders_created_total{application="order-service",environment="prod",type="online"} 45231.0

# HELP payment_processing_duration_seconds Time to process payment
# TYPE payment_processing_duration_seconds summary
payment_processing_duration_seconds{application="order-service",quantile="0.5"} 0.245
payment_processing_duration_seconds{application="order-service",quantile="0.95"} 0.892
payment_processing_duration_seconds{application="order-service",quantile="0.99"} 1.523
payment_processing_duration_seconds_count 45231
payment_processing_duration_seconds_sum 18234.567
```

**6. Query PromQL dans Grafana** :
```promql
# Taux de création de commandes par minute
rate(orders_created_total[5m]) * 60

# P95 latency du traitement paiement
histogram_quantile(0.95, rate(payment_processing_duration_seconds_bucket[5m]))

# Taux d'erreur
rate(http_server_requests_seconds_count{status=~"5.."}[5m]) 
/ 
rate(http_server_requests_seconds_count[5m]) * 100
```

**Bonnes pratiques** :
- Nommer en snake_case avec suffixe d'unité : `_total`, `_seconds`, `_bytes`
- Limiter la cardinalité des tags (éviter userId, orderId)
- Utiliser Counter pour valeurs croissantes, Gauge pour valeurs fluctuantes
- Timer inclut automatiquement count, sum et histogramme

---

### Q11: Comment configurez-vous Prometheus pour le service discovery dans Kubernetes?

**R:** Dans un environnement Kubernetes, le **service discovery** automatique est essentiel. Voici ma configuration :

**1. Annotations sur les Pods Spring Boot** :
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
spec:
  template:
    metadata:
      annotations:
        prometheus.io/scrape: "true"
        prometheus.io/port: "8080"
        prometheus.io/path: "/actuator/prometheus"
      labels:
        app: order-service
        version: v1.2.3
```

**2. Configuration Prometheus avec kubernetes_sd_configs** :
```yaml
# prometheus.yml
scrape_configs:
  - job_name: 'kubernetes-pods'
    kubernetes_sd_configs:
      - role: pod
        namespaces:
          names:
            - production
            - staging
    
    # Relabeling pour filtrer et enrichir
    relabel_configs:
      # Scraper uniquement les pods avec annotation scrape=true
      - source_labels: [__meta_kubernetes_pod_annotation_prometheus_io_scrape]
        action: keep
        regex: true
      
      # Utiliser le port de l'annotation
      - source_labels: [__meta_kubernetes_pod_annotation_prometheus_io_port]
        action: replace
        target_label: __address__
        regex: ([^:]+)(?::\d+)?
        replacement: $1:$2
      
      # Utiliser le path de l'annotation
      - source_labels: [__meta_kubernetes_pod_annotation_prometheus_io_path]
        action: replace
        target_label: __metrics_path__
        regex: (.+)
      
      # Ajouter des labels utiles
      - source_labels: [__meta_kubernetes_namespace]
        target_label: kubernetes_namespace
      - source_labels: [__meta_kubernetes_pod_name]
        target_label: kubernetes_pod_name
      - source_labels: [__meta_kubernetes_pod_label_app]
        target_label: application
      - source_labels: [__meta_kubernetes_pod_label_version]
        target_label: version
```

**3. Alternative : Prometheus Operator avec ServiceMonitor** :
```yaml
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: order-service-metrics
  namespace: production
spec:
  selector:
    matchLabels:
      app: order-service
  endpoints:
  - port: http
    path: /actuator/prometheus
    interval: 15s
    scrapeTimeout: 10s
```

**4. Résultat - Targets dans Prometheus UI** :
```
Endpoint: http://10.244.1.42:8080/actuator/prometheus
Labels: {
  application="order-service",
  kubernetes_namespace="production",
  kubernetes_pod_name="order-service-7d9f8c-x7k2m",
  version="v1.2.3",
  instance="10.244.1.42:8080"
}
Status: UP (1/1 up)
```

**Avantages** :
- **Auto-discovery** : Nouveaux pods détectés automatiquement
- **Multi-tenant** : Séparation par namespace
- **Version tracking** : Métriques par version pour gradual rollout
- **No configuration** : Pas besoin de modifier Prometheus à chaque nouveau service

**Bonus - Federation pour multi-cluster** :
```yaml
# Prometheus central
scrape_configs:
  - job_name: 'federate-cluster-eu'
    honor_labels: true
    metrics_path: '/federate'
    params:
      'match[]':
        - '{job=~"kubernetes-.*"}'
    static_configs:
      - targets:
        - 'prometheus-eu.example.com:9090'
```

---

## 🔗 Questions sur Distributed Tracing

### Q12: Expliquez comment fonctionne la propagation du trace context entre microservices

**R:** La **trace context propagation** est le mécanisme qui permet de suivre une requête à travers plusieurs microservices. Voici le fonctionnement détaillé :

**1. Anatomie d'un Trace Context** :
Un trace context contient typiquement :
- **TraceId** : Identifiant unique pour toute la requête (ex: `a1b2c3d4e5f6g7h8`)
- **SpanId** : Identifiant du segment actuel (ex: `x1y2z3`)
- **ParentSpanId** : Lien vers le span parent
- **Flags** : Options de sampling, debug, etc.

**2. Propagation via HTTP Headers (W3C Trace Context)** :
```
GET /api/orders/123 HTTP/1.1
Host: order-service
traceparent: 00-a1b2c3d4e5f6g7h8-x1y2z3w4-01
tracestate: vendor1=value1,vendor2=value2
```

Format `traceparent` : `version-traceId-spanId-flags`

**3. Implémentation automatique avec Spring Cloud Sleuth** :
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-sleuth</artifactId>
</dependency>
```

Sleuth instrumenté automatiquement :
- **RestTemplate** : Ajoute headers de trace
- **WebClient** : Propagation réactive
- **Feign clients** : Injection automatique
- **Messaging** (Kafka, RabbitMQ) : Headers dans messages
- **Async methods** : Propagation dans threads

**4. Exemple de flow complet** :

```
User Request → API Gateway
                 TraceId: a1b2c3d4 (créé)
                 SpanId: span-1
                 ↓
               Order Service
                 TraceId: a1b2c3d4 (propagé)
                 SpanId: span-2 (nouveau)
                 ParentSpanId: span-1
                 ↓
               ├→ Inventory Service
               │   TraceId: a1b2c3d4
               │   SpanId: span-3
               │   ParentSpanId: span-2
               │
               └→ Payment Service
                   TraceId: a1b2c3d4
                   SpanId: span-4
                   ParentSpanId: span-2
```

**5. Code Spring Boot avec propagation manuelle** :
```java
@Service
public class OrderService {
    private final RestTemplate restTemplate;
    private final Tracer tracer;
    
    public Order createOrder(OrderRequest request) {
        // Span créé automatiquement par @Traced ou manuellement
        Span span = tracer.nextSpan().name("create-order").start();
        
        try (Tracer.SpanInScope ws = tracer.withSpanInScope(span)) {
            // Ajouter tags au span
            span.tag("order.id", request.getOrderId());
            span.tag("user.id", request.getUserId());
            span.tag("amount", String.valueOf(request.getAmount()));
            
            // Appel HTTP - headers propagés automatiquement
            InventoryResponse inventory = restTemplate.getForObject(
                "http://inventory-service/api/check", 
                InventoryResponse.class
            );
            
            // Ajouter événement au span
            span.event("inventory.checked");
            
            if (inventory.isAvailable()) {
                // Traitement paiement - nouveau span enfant créé auto
                paymentService.processPayment(request);
                span.event("payment.processed");
            }
            
            return order;
        } catch (Exception e) {
            // Marquer l'erreur dans le span
            span.error(e);
            throw e;
        } finally {
            span.finish();
        }
    }
}
```

**6. Propagation dans messages asynchrones (Kafka)** :
```java
@Service
public class OrderPublisher {
    private final KafkaTemplate<String, Order> kafkaTemplate;
    
    public void publishOrder(Order order) {
        // Sleuth injecte automatiquement trace headers dans Kafka headers
        kafkaTemplate.send("orders-topic", order);
    }
}

@Service
public class OrderConsumer {
    @KafkaListener(topics = "orders-topic")
    public void consumeOrder(Order order, 
                            @Header(KafkaHeaders.RECEIVED_MESSAGE_KEY) String key,
                            @Headers MessageHeaders headers) {
        // TraceId extrait automatiquement des headers Kafka
        // Nouveau span créé avec bon parent
        log.info("Processing order: {}", order.getId());
        // Le log contiendra automatiquement le traceId
    }
}
```

**7. Visualisation dans Grafana Tempo** :
La trace complète montre :
```
create-order (250ms total)
├─ inventory-service/check (50ms)
│  └─ postgres/query (45ms)
├─ payment-service/process (180ms)
│  ├─ stripe-api/charge (150ms)
│  └─ postgres/insert (20ms)
└─ kafka/publish (15ms)
```

**8. Configuration OpenTelemetry (standard moderne)** :
```java
@Configuration
public class OpenTelemetryConfig {
    @Bean
    public OpenTelemetry openTelemetry() {
        return AutoConfiguredOpenTelemetrySdk.initialize()
            .getOpenTelemetrySdk();
    }
}
```
## 🔗 Questions sur Distributed Tracing

### Q13: Comment gérez-vous le sampling dans le distributed tracing et pourquoi est-ce important?

**R:** Le **sampling** est crucial pour gérer le volume de traces en production tout en capturant les informations critiques. Sans sampling, stocker 100% des traces peut coûter très cher et surcharger le système.

**1. Types de Sampling** :

**Head-based Sampling (décision au début de la trace)** :
```java
@Configuration
public class TracingConfig {
    @Bean
    public Sampler sampler() {
        // Probability Sampler - 10% des traces
        return Sampler.traceIdRatioBased(0.1);
    }
}
```

**Tail-based Sampling (décision à la fin, plus intelligent)** :
```yaml
# Configuration Tempo
overrides:
  defaults:
    ingestion:
      rate_limit_bytes: 15000000
      burst_size_bytes: 20000000
```

**2. Ma stratégie de sampling adaptative** :

```java
@Component
public class AdaptiveSampler implements Sampler {
    
    @Override
    public SamplingResult shouldSample(
            Context parentContext,
            String traceId,
            String name,
            SpanKind spanKind,
            Attributes attributes,
            List<LinkData> parentLinks) {
        
        // Règle 1: TOUJOURS sampler les erreurs
        if (isError(attributes)) {
            return SamplingResult.recordAndSample();
        }
        
        // Règle 2: TOUJOURS sampler les requêtes lentes
        Duration duration = getDuration(attributes);
        if (duration != null && duration.toMillis() > 1000) {
            return SamplingResult.recordAndSample();
        }
        
        // Règle 3: Sampler 100% des endpoints critiques
        String endpoint = attributes.get(AttributeKey.stringKey("http.route"));
        if (CRITICAL_ENDPOINTS.contains(endpoint)) {
            return SamplingResult.recordAndSample();
        }
        
        // Règle 4: Sampler selon l'environnement
        String env = System.getenv("SPRING_PROFILES_ACTIVE");
        if ("dev".equals(env) || "staging".equals(env)) {
            return SamplingResult.recordAndSample(); // 100% en dev/staging
        }
        
        // Règle 5: Production normale - 5% probabiliste
        return Math.random() < 0.05 
            ? SamplingResult.recordAndSample() 
            : SamplingResult.drop();
    }
}
```

**3. Configuration par environnement** :

```yaml
# application-dev.yml
spring:
  sleuth:
    sampler:
      probability: 1.0  # 100% en dev

# application-staging.yml
spring:
  sleuth:
    sampler:
      probability: 0.5  # 50% en staging

# application-prod.yml
spring:
  sleuth:
    sampler:
      probability: 0.05  # 5% en prod
    
  # Mais forcer le sampling pour certains cas
  trace:
    sampling:
      error: 1.0      # 100% des erreurs
      slow: 1.0       # 100% des requêtes > 1s
      critical-path: 1.0  # 100% des endpoints critiques
```

**4. Sampling Rate Limiter pour contrôler le coût** :
```java
@Configuration
public class RateLimitedSamplingConfig {
    
    @Bean
    public Sampler rateLimitedSampler() {
        // Maximum 100 traces/seconde
        return new RateLimitingSampler(100);
    }
}

class RateLimitingSampler implements Sampler {
    private final RateLimiter rateLimiter;
    
    public RateLimitingSampler(int tracesPerSecond) {
        this.rateLimiter = RateLimiter.create(tracesPerSecond);
    }
    
    @Override
    public SamplingResult shouldSample(...) {
        if (rateLimiter.tryAcquire()) {
            return SamplingResult.recordAndSample();
        }
        return SamplingResult.drop();
    }
}
```

**5. Impact et métriques du sampling** :

**Sans sampling intelligent** (100% traces) :
- Volume : 500M traces/jour
- Coût stockage Tempo : ~$15,000/mois
- Latency overhead : 15-20ms par requête

**Avec sampling adaptatif** (5% base + règles) :
- Volume : 35M traces/jour (7% effectif)
- Coût stockage : ~$1,200/mois (92% réduction)
- Latency overhead : 2-3ms par requête
- **Coverage** : 100% des erreurs et requêtes lentes capturées

**6. Monitoring du sampling** :
```promql
# Taux de sampling effectif
sum(rate(traces_sampled_total[5m])) 
/ 
sum(rate(traces_total[5m])) * 100

# Vérifier qu'on capture bien les erreurs
sum(rate(traces_sampled_total{error="true"}[5m])) 
/ 
sum(rate(traces_total{error="true"}[5m])) * 100
# Doit être proche de 100%
```

**Résultat** : Balance optimale entre coût, performance et visibilité sur les problèmes.

---

### Q14: Comment utilisez-vous le distributed tracing pour identifier des bottlenecks de performance?

**R:** Le tracing distribué est mon outil principal pour le **performance tuning**. Voici ma méthodologie :

**1. Identification des patterns de latence** :

**Dans Grafana Tempo/Jaeger** :
- Filtrer les traces par service : `service.name = "order-service"`
- Trier par durée décroissante
- Identifier le percentile P99 : traces > 2 secondes

**Query TraceQL dans Tempo** :
```traceql
{service.name="order-service" && duration > 2s} 
| select(spanID, duration, resource.service.name)
```

**2. Analyse du Flamegraph** :

Exemple de trace problématique identifiée :
```
POST /api/orders (3.2s total) ⚠️
│
├─ OrderService.createOrder (3.15s)
│  │
│  ├─ InventoryService.checkStock (2.8s) ⚠️⚠️
│  │  │
│  │  ├─ HTTP GET /inventory/check (2.75s)
│  │  │  │
│  │  │  └─ PostgreSQL SELECT (2.7s) 🔴 BOTTLENECK
│  │  │     Query: SELECT * FROM inventory WHERE...
│  │  │
│  │  └─ Response parsing (0.05s)
│  │
│  ├─ PaymentService.process (0.25s) ✅
│  │  ├─ Stripe API call (0.18s)
│  │  └─ Database insert (0.05s)
│  │
│  └─ NotificationService.send (0.08s) ✅
│
└─ Response serialization (0.02s)
```

**Diagnostic immédiat** :
- 87% du temps dans InventoryService
- Query PostgreSQL non optimisée
- Aucun index sur la colonne de recherche

**3. Corrélation trace → logs pour investigation** :
```java
// Dans le span, j'ajoute des attributs détaillés
span.setAttribute("db.statement", sqlQuery);
span.setAttribute("db.rows.returned", rowCount);
span.setAttribute("db.query.plan", executionPlan);

// Puis dans Grafana :
// Clic sur span PostgreSQL → "View Logs"
// Logs filtrés automatiquement montrent :
// "Query plan: Seq Scan on inventory (cost=0.00..1245.67)"
```

**4. Patterns de problèmes fréquents détectés** :

**Pattern 1 : N+1 Query Problem**
```
OrderService.getOrders (5.2s)
├─ Database: SELECT * FROM orders (0.1s)
├─ Loop iteration 1
│  └─ Database: SELECT * FROM order_items WHERE order_id=1 (0.05s)
├─ Loop iteration 2
│  └─ Database: SELECT * FROM order_items WHERE order_id=2 (0.05s)
... (100 iterations)
└─ Total: 100 * 0.05s = 5s 🔴
```

**Solution** : Batch loading ou JOIN
```java
// Avant : N+1 queries
orders.forEach(order -> {
    List<OrderItem> items = itemRepo.findByOrderId(order.getId());
    order.setItems(items);
});

// Après : 1 query avec JOIN
@Query("SELECT o FROM Order o LEFT JOIN FETCH o.items")
List<Order> findAllWithItems();
```

**Pattern 2 : Synchronous Cascading Calls**
```
API Gateway (2.5s total)
├─ UserService.getProfile (0.8s)
└─ Then OrderService.getOrders (0.9s)
    └─ Then PaymentService.getPayments (0.8s)
```

**Solution** : Parallélisation
```java
// Avant : Sequential (2.5s)
UserProfile profile = userService.getProfile(userId);
List<Order> orders = orderService.getOrders(userId);
List<Payment> payments = paymentService.getPayments(userId);

// Après : Parallel (0.9s)
CompletableFuture<UserProfile> profileFuture = 
    CompletableFuture.supplyAsync(() -> userService.getProfile(userId));
CompletableFuture<List<Order>> ordersFuture = 
    CompletableFuture.supplyAsync(() -> orderService.getOrders(userId));
CompletableFuture<List<Payment>> paymentsFuture = 
    CompletableFuture.supplyAsync(() -> paymentService.getPayments(userId));

CompletableFuture.allOf(profileFuture, ordersFuture, paymentsFuture).join();
```

**Pattern 3 : Missing Cache**
```
ProductService.getProductDetails (150ms) - appelé 50 fois/seconde
└─ Database query (140ms)
```

**Solution** : Cache Redis
```java
@Cacheable(value = "products", key = "#productId")
public Product getProductDetails(String productId) {
    return productRepository.findById(productId);
}
// Latency réduite : 150ms → 5ms (cache hit)
```

**5. Dashboard Grafana pour monitoring continu** :

J'ai créé un dashboard "Performance Analysis" avec :
- **Panel 1** : P50, P95, P99 latency par service
- **Panel 2** : Heatmap des durées de traces
- **Panel 3** : Top 10 spans les plus lents
- **Panel 4** : Distribution des durées par operation type (DB, HTTP, Cache)

**Query PromQL pour alerting** :
```promql
# Alerter si P99 latency > 2s pendant 5 minutes
histogram_quantile(0.99, 
  sum(rate(traces_duration_bucket{service="order-service"}[5m])) by (le)
) > 2
```

**6. Cas concret d'optimisation** :

**Avant optimisation** :
- Endpoint `/api/orders` : P99 = 3.2s
- Throughput : 50 req/s max
- User complaints : "Application lente"

**Après analyse tracing et optimisations** :
1. Ajout d'index DB sur colonnes fréquentes : -70% latency
2. Parallélisation de 3 appels séquentiels : -50% latency
3. Cache Redis sur données produits : -80% latency pour cache hits
4. Pagination pour limiter résultats : -30% latency

**Résultats** :
- P99 = 0.4s (87% amélioration)
- Throughput : 300 req/s (6x augmentation)
- Customer satisfaction : +40%

**Métrique clé** : ROI du tracing = (Coût d'infrastructure économisé) / (Coût du tracing)
Dans ce projet : ROI = 15x

---

## 🚨 Questions sur Alerting

### Q15: Quelle est votre stratégie pour éviter l'alert fatigue tout en capturant les vrais problèmes?

**R:** L'**alert fatigue** est un problème majeur dans l'observability. Voici ma stratégie complète :

**1. Principe fondamental : Les 4 Golden Signals**

Je concentre les alertes sur ce qui impacte vraiment les utilisateurs :
- **Latency** : Le service est-il rapide ?
- **Traffic** : Combien d'utilisateurs sont affectés ?
- **Errors** : Y a-t-il des échecs ?
- **Saturation** : Sommes-nous proche des limites ?

**2. Classification des alertes par sévérité** :

```yaml
# alerts.yml - Exemple Prometheus AlertManager

groups:
  - name: critical_alerts
    interval: 30s
    rules:
      # CRITICAL : Page immédiatement
      - alert: ServiceDown
        expr: up{job="spring-boot"} == 0
        for: 2m
        labels:
          severity: critical
          pager: true
        annotations:
          summary: "Service {{ $labels.instance }} is DOWN"
          description: "{{ $labels.instance }} has been down for 2 minutes"
          runbook_url: "https://wiki.company.com/runbooks/service-down"
          
      - alert: HighErrorRate
        expr: |
          (sum(rate(http_server_requests_seconds_count{status=~"5.."}[5m])) 
          / 
          sum(rate(http_server_requests_seconds_count[5m]))) * 100 > 5
        for: 5m
        labels:
          severity: critical
          pager: true
        annotations:
          summary: "High error rate on {{ $labels.service }}"
          description: "Error rate is {{ $value | humanize }}%"
          
  - name: warning_alerts
    interval: 1m
    rules:
      # WARNING : Notifier sans pager
      - alert: HighLatency
        expr: |
          histogram_quantile(0.99, 
            sum(rate(http_server_requests_seconds_bucket[5m])) by (le, service)
          ) > 2
        for: 10m
        labels:
          severity: warning
          pager: false
        annotations:
          summary: "High P99 latency on {{ $labels.service }}"
          
      - alert: HighMemoryUsage
        expr: |
          (jvm_memory_used_bytes{area="heap"} 
          / 
          jvm_memory_max_bytes{area="heap"}) * 100 > 85
        for: 15m
        labels:
          severity: warning
          pager: false
          
  - name: info_alerts
    interval: 5m
    rules:
      # INFO : Juste logger, pas de notification
      - alert: ModerateTraffic
        expr: rate(http_server_requests_seconds_count[5m]) > 100
        for: 10m
        labels:
          severity: info
          pager: false
```

**3. Seuils intelligents basés sur les données historiques** :

```python
# Script pour calculer les seuils dynamiques
# Basé sur P95 des 30 derniers jours + marge

import numpy as np
from prometheus_api_client import PrometheusConnect

prom = PrometheusConnect(url="http://prometheus:9090")

# Récupérer 30 jours de données
latency_data = prom.custom_query_range(
    query='histogram_quantile(0.95, rate(http_server_requests_seconds_bucket[5m]))',
    start_time='30d',
    end_time='now',
    step='1h'
)

# Calculer P95 + 20% de marge
p95_latency = np.percentile(latency_data, 95)
alert_threshold = p95_latency * 1.2

print(f"Recommended alert threshold: {alert_threshold}s")
# Output: "Recommended alert threshold: 1.8s"
```

**4. Grace periods et hysteresis pour éviter flapping** :

```yaml
# Attendre 5 minutes avant d'alerter (éviter blips)
for: 5m

# AlertManager - Grouping et throttling
route:
  group_by: ['alertname', 'service', 'environment']
  group_wait: 30s        # Attendre 30s pour grouper
  group_interval: 5m     # Regrouper les alertes pendant 5m
  repeat_interval: 4h    # Ne pas répéter avant 4h si non résolu
  
  routes:
    - match:
        severity: critical
      receiver: pagerduty-critical
      repeat_interval: 30m  # Répéter toutes les 30m si pas résolu
      
    - match:
        severity: warning
      receiver: slack-warnings
      repeat_interval: 12h
```

**5. Contextual alerting avec enrichissement** :

```yaml
# Grafana Alert avec contexte riche
annotations:
  summary: "🔴 High Error Rate Alert"
  description: |
    Service: {{ $labels.service }}
    Environment: {{ $labels.environment }}
    Current Error Rate: {{ $value | humanize }}%
    Threshold: 5%
    Duration: {{ $duration }}
    
    📊 Quick Links:
    - Dashboard: https://grafana/d/service-overview?var-service={{ $labels.service }}
    - Logs: https://grafana/explore?left=["now-1h","now","Loki",{"expr":"{service=\"{{ $labels.service }}\"} |= \"ERROR\""}]
    - Traces: https://grafana/explore?left=["now-1h","now","Tempo",{"query":"{ service.name=\"{{ $labels.service }}\" && status=error }"}]
    
    📖 Runbook: https://wiki/runbooks/high-error-rate
    
    Recent Deployments:
    {{ range query "changes_total{service=\"{{ $labels.service }}\"}" }}
    - Version {{ .Labels.version }} deployed {{ .Labels.deployed_at }}
    {{ end }}
```

**6. Alert routing intelligent** :

```yaml
# AlertManager routing
receivers:
  - name: 'pagerduty-critical'
    pagerduty_configs:
      - service_key: '<secret>'
        severity: critical
        
  - name: 'slack-warnings'
    slack_configs:
      - api_url: '<webhook>'
        channel: '#alerts-warnings'
        title: "Warning Alert"
        text: "{{ range .Alerts }}{{ .Annotations.description }}{{ end }}"
        
  - name: 'slack-business-hours'
    slack_configs:
      - api_url: '<webhook>'
        channel: '#alerts-business'
        send_resolved: true

# Routing conditionnel
route:
  routes:
    # Nuit/Weekend : Seulement critical
    - match:
        severity: critical
      continue: true
      receiver: pagerduty-critical
      
    # Business hours : Warnings aussi
    - match:
        severity: warning
      receiver: slack-warnings
      time_intervals:
        - business_hours  # 9h-18h Lun-Ven
```

**7. Ma checklist avant de créer une alerte** :

✅ **Est-ce que ça impacte réellement les utilisateurs ?**
- Non → Ne pas alerter, juste logger

✅ **Est-ce actionnable immédiatement ?**
- Non → C'est une métrique à monitorer, pas une alerte

✅ **Le seuil est-il basé sur des données réelles ?**
- Éviter les seuils arbitraires (ex: CPU > 80%)
- Utiliser historique + analyse statistique

✅ **Y a-t-il un runbook clair ?**
- Chaque alerte doit avoir documentation des actions

✅ **L'alerte peut-elle se déclencher pour des raisons bénignes ?**
- Oui → Ajuster seuils ou grace period

✅ **Quelle est la fréquence attendue ?**
- Plus de 1x/jour → Revoir les seuils

**8. Métriques pour monitorer la santé de l'alerting** :

```promql
# Nombre d'alertes par jour
sum(increase(alertmanager_alerts_received_total[24h]))

# Taux de faux positifs (alertes résolues sans action)
sum(increase(alertmanager_alerts_resolved_total{action="none"}[7d]))
/
sum(increase(alertmanager_alerts_total[7d]))

# Temps moyen de résolution
avg(alertmanager_alerts_resolution_duration_seconds)
```

**Résultat de cette approche** :
- **Avant** : 200 alertes/jour, 85% de faux positifs, équipe épuisée
- **Après** : 8 alertes/jour, 95% de vraies alertes, résolution 3x plus rapide
- **Alert fatigue** : Éliminé ✅

---

### Q16: Comment implémentez-vous des alertes basées sur les logs dans Grafana et ELK?

**R:** Les **log-based alerts** sont complémentaires aux metric alerts. Voici mes deux approches :

**1. Alertes Grafana Loki**

**Configuration de l'alerte** :
```yaml
# Grafana Alert Rule
apiVersion: 1
groups:
  - name: log_alerts
    interval: 1m
    rules:
      - uid: log_error_spike
        title: Error Log Spike Detected
        condition: B
        data:
          - refId: A
            queryType: ''
            relativeTimeRange:
              from: 300
              to: 0
            datasourceUid: loki-uid
            model:
              expr: |
                sum(count_over_time({service="order-service"} 
                |= "ERROR" [5m]))
              
          - refId: B
            queryType: ''
            datasourceUid: __expr__
            model:
              type: threshold
              expression: A
              conditions:
                - evaluator:
                    params: [50]  # Plus de 50 erreurs en 5min
                    type: gt
                  operator:
                    type: and
                  query:
                    params: [B]
        
        annotations:
          summary: |
            Detected {{ $values.A }} error logs in last 5 minutes
          description: |
            Service order-service is logging an unusual number of errors.
            
        labels:
          severity: warning
          
        for: 5m  #持续5分钟才触发
```

**Query LogQL avancée pour pattern matching** :
```logql
# Alerte sur pattern spécifique d'erreur
{service="payment-service"} 
|= "ERROR" 
|~ "NullPointerException|OutOfMemoryError|SQLException"
| json
| line_format "{{.level}} - {{.message}}"
```

**2. Alertes Kibana (ELK)**

**A. Kibana Alerting Rules** :
```json
POST _plugins/_alerting/monitors
{
  "type": "monitor",
  "name": "High Error Rate Monitor",
  "enabled": true,
  "schedule": {
    "period": {
      "interval": 5,
      "unit": "MINUTES"
    }
  },
  "inputs": [{
    "search": {
      "indices": ["logs-spring-boot-*"],
      "query": {
        "size": 0,
        "query": {
          "bool": {
            "must": [
              {
                "match": {
                  "level": "ERROR"
                }
              },
              {
                "range": {
                  "@timestamp": {
                    "gte": "now-5m"
                  }
                }
              }
            ]
          }
        },
        "aggs": {
          "error_count": {
            "value_count": {
              "field": "_id"
            }
          }
        }
      }
    }
  }],
  "triggers": [{
    "name": "Error threshold trigger",
    "severity": "2",
    "condition": {
      "script": {
        "source": "ctx.results[0].aggregations.error_count.value > 100",
        "lang": "painless"
      }
    },
    "actions": [{
      "name": "Slack notification",
      "destination_id": "slack-webhook-id",
      "message_template": {
        "source": "Detected {{ctx.results.0.aggregations.error_count.value}} errors in last 5 minutes"
      }
    }]
  }]
}
```

**B. Elasticsearch Watcher (X-Pack)** :
```json
PUT _watcher/watch/payment_failures
{
  "trigger": {
    "schedule": {
      "interval": "1m"
    }
  },
  "input": {
    "search": {
      "request": {
        "indices": ["logs-spring-boot-prod-*"],
        "body": {
          "query": {
            "bool": {
              "must": [
                {
                  "match": {
                    "service": "payment-service"
                  }
                },
                {
                  "match": {
                    "message": "payment failed"
                  }
                },
                {
                  "range": {
                    "@timestamp": {
                      "gte": "now-5m"
                    }
                  }
                }
              ]
            }
          },
          "aggs": {
            "failure_rate": {
              "terms": {
                "field": "payment.provider.keyword",
                "size": 10
              }
            }
          }
        }
      }
    }
  },
  "condition": {
    "compare": {
      "ctx.payload.hits.total": {
        "gt": 20
      }
    }
  },
  "actions": {
    "send_email": {
      "email": {
        "to": "payments-team@company.com",
        "subject": "Payment Failures Alert",
        "body": {
          "html": "<h2>Payment failures detected</h2><p>Total failures: {{ctx.payload.hits.total}}</p>"
        }
      }
    },
    "create_ticket": {
      "webhook": {
        "scheme": "https",
        "host": "jira.company.com",
        "port": 443,
        "method": "post",
        "path": "/rest/api/2/issue",
        "params": {},
        "headers": {
          "Content-Type": "application/json"
        },
        "body": "{\"fields\":{\"project\":{\"key\":\"PAY\"},\"summary\":\"Payment failures spike\",\"issuetype\":{\"name\":\"Bug\"}}}"
      }
    }
  }
}
```

**3. Alertes basées sur anomalies (Machine Learning)** :

**Kibana ML Job pour détection d'anomalies** :
```json
PUT _ml/anomaly_detectors/error_rate_anomaly
{
  "description": "Detect unusual error rates",
  "analysis_config": {
    "bucket_span": "5m",
    "detectors": [{
      "function": "count",
      "by_field_name": "service.keyword",
      "detector_description": "Count of errors by service"
    }],
    "influencers": ["service.keyword", "environment.keyword"]
  },
  "data_description": {
    "time_field": "@timestamp"
  },
  "datafeed_config": {
    "indices": ["logs-spring-boot-*"],
    "query": {
      "match": {
        "level": "ERROR"
      }
    }
  }
}
```

**4. Cas d'usage spécifiques**:

**A. Alerte sur Stack Trace spécifique** :
```logql
# Loki
{service="inventory-service"}
|= "ERROR"
|~ ".*Deadlock.*"
| json
| error_type != ""
```

```json
// Elasticsearch
{
  "query": {
    "bool": {
      "must": [
        {"match": {"level": "ERROR"}},
        {"match_phrase": {"exception.stacktrace": "java.sql.SQLException: Deadlock"}}
      ]
    }
  }
}
```

**B. Alerte sur taux d'augmentation soudaine** :
```logql
# Détection de spike (5x le taux normal)
sum(rate({service="order-service"} |= "ERROR" [5m])) 
> 
5 * sum(rate({service="order-service"} |= "ERROR" [1h] offset 1d))
```

**C. Alerte multi-conditions** :
```json
{
  "query": {
    "bool": {
      "must": [
        {"match": {"service": "payment-service"}},
        {"match": {"level": "ERROR"}},
        {"range": {"@timestamp": {"gte": "now-5m"}}},
        {"script": {
          "script": "doc['response_time'].value > 2000"
        }}
      ]
    }
  }
}
```

**5. Notification enrichie avec contexte** :

**Template Slack pour alerte Grafana** :
```go
{{ define "slack.title" }}
🔴 Error Spike Alert - {{ .Labels.service }}
{{ end }}

{{ define "slack.text" }}
*Service:* {{ .Labels.service }}
*Environment:* {{ .Labels.environment }}
*Error Count:* {{ .Values.A }} errors in last 5 minutes
*Threshold:* 50 errors

*Sample Error Logs:*
{{ range .GetAlerts }}
{{ .Annotations.sample_logs }}
{{ end }}

*Actions:*
• <https://grafana/d/logs?service={{ .Labels.service }}|View Logs>
• <https://grafana/d/service-dashboard?service={{ .Labels.service }}|View Dashboard>
• <https://wiki/runbooks/error-spike|Runbook>

*Recent Changes:*
Check if related to recent deployment
{{ end }}
```

**6. Tests et validation des alertes** :

```bash
# Générer des logs de test
for i in {1..60}; do
  curl -X POST http://order-service/simulate-error
  sleep 1
done

# Vérifier que l'alerte se déclenche dans les 5 minutes
# Vérifier le message dans Slack/PagerDuty
# Valider que les liens fonctionnent
# Confirmer que le runbook est à jour
```

**Best practices log-based alerting** :
✅ Toujours inclure des sample logs dans la notification
✅ Limiter la cardinalité (groupby service, pas par userId)
✅ Utiliser aggregations plutôt que raw count quand possible
✅ Tester les alertes en staging avant prod
✅ Documenter clairement les seuils et leur origine

---

## 🎯 Questions de Mise en Situation

### Q17: Un service en production a soudainement un taux d'erreur de 15%. Comment procédez-vous pour diagnostiquer et résoudre le problème?

**R:** Voici ma méthodologie complète de **war room** pour un incident en production :

**Phase 1 : Détection et Alerte (0-2 minutes)**

**1. Réception de l'alerte** :
```
🔴 CRITICAL ALERT
Service: payment-service
Error Rate: 15.2% (threshold: 5%)
Duration: 8 minutes
Environment: production
Dashboard: https://grafana/d/payment-overview
```

**2. Confirmation rapide** :
```bash
# Vérifier que ce n'est pas un faux positif
# Dans Grafana, vérifier les métriques
```

**Phase 2 : Investigation Initiale (2-5 minutes)**

**1. Consulter le dashboard principal** :

**Dashboard "Service Overview"** montre :
- **Traffic** : ✅ Normal (500 req/min)
- **Latency** : ⚠️ P99 = 3.5s (normale: 0.8s)
- **Error Rate** : 🔴 15.2% (normale: 0.5%)
- **Saturation** : ✅ CPU 45%, Memory 60%

**Première observation** : Latency élevée corrélée avec erreurs → probable timeout ou service dépendant

**2. Analyser la timeline** :
```promql
# Query Grafana pour voir quand ça a commencé
rate(http_server_requests_seconds_count{status=~"5..",service="payment-service"}[1m])
```

**Timeline** :
- 14:45 : Tout normal
- 14:47 : Début de l'augmentation des erreurs
- 14:50 : Pic à 15%
- **Incident start** : ~14:47

**3. Vérifier les déploiements récents** :
```bash
# Consulter l'historique des déploiements
kubectl get events --sort-by='.lastTimestamp' -n production

# Résultat : Aucun déploiement depuis 3 jours ✅
# Donc pas causé par un nouveau code
```

**Phase 3 : Deep Dive avec Logs (5-10 minutes)**

**1. Explorer les logs d'erreur dans Grafana Loki** :
```logql
{service="payment-service", environment="production"} 
|= "ERROR" 
| json 
| __error__="" 
| line_format "{{.timestamp}} {{.level}} {{.message}}"
```

**Top erreurs identifiées** :
```
[14:47:23] ERROR - Payment processing failed: Connection timeout to stripe-api
[14:47:25] ERROR - Payment processing failed: Connection timeout to stripe-api
[14:47:28] ERROR - Payment processing failed: Connection timeout to stripe-api
... (150 occurrences en 5 min)
```

**Pattern clair** : Timeouts vers Stripe API (provider de paiement externe)

**2. Affiner l'analyse** :
```logql
{service="payment-service"} 
|= "stripe-api" 
| json 
| line_format "{{.status}} {{.duration}}ms {{.message}}"
```

**Résultat** :
```
[14:47:23] TIMEOUT 5000ms Connection timeout
[14:47:25] TIMEOUT 5000ms Connection timeout
[14:48:12] SUCCESS 180ms Payment processed
[14:48:15] TIMEOUT 5000ms Connection timeout
```

**Pattern** : ~15% des requêtes vers Stripe timeout (exactement le taux d'erreur observé)

**Phase 4 : Corrélation avec Traces (10-12 minutes)**

**1. Récupérer un traceId d'une requête en erreur** :
```logql
{service="payment-service"} 
|= "ERROR" 
|= "Connection timeout" 
| json 
| line_format "{{.traceId}}"
| limit 1
```

**TraceId** : `a1b2c3d4e5f6g7h8`

**2. Visualiser la trace dans Tempo** :
```
POST /api/payments (5.2s - ERROR)
│
├─ PaymentController.processPayment (5.15s)
│  │
│  ├─ PaymentService.charge (5.1s)
│  │  │
│  │  ├─ HTTP POST https://api.stripe.com/v1/charges (5.0s) 🔴 TIMEOUT
│  │  │  Status: TIMEOUT
│  │  │  Error: java.net.SocketTimeoutException: Read timed out
│  │  │
│  │  └─ Fallback handler (0.08s)
│  │     └─ Database: INSERT INTO failed_payments (0.05s)
│  │
│  └─ Response marshalling (0.02s)
```

**Diagnostic confirmé** : Stripe API ne répond pas dans les 5 secondes (notre timeout configuré)

**Phase 5 : Investigation Externe (12-15 minutes)**

**1. Vérifier le status de Stripe** :
```bash
# Check Stripe Status Page
curl https://status.stripe.com/api/v2/status.json
```

**Résultat** :
```json
{
  "page": {
    "name": "Stripe"
  },
  "status": {
    "indicator": "minor",
    "description": "Partial System Outage"
  },
  "incidents": [{
    "name": "Elevated API Errors",
    "status": "investigating",
    "created_at": "2024-11-01T14:45:00Z",
    "impact": "minor"
  }]
}
```

**ROOT CAUSE IDENTIFIÉE** : Incident chez Stripe (service externe) causant des timeouts partiels

**Phase 6 : Mitigation Immédiate (15-20 minutes)**

**Options d'action** :

**Option 1 : Augmenter le timeout** (❌ Mauvaise idée)
- Augmenterait la latency pour les utilisateurs
- Ne résout pas le problème sous-jacent

**Option 2 : Circuit Breaker** (✅ Meilleure approche immédiate)
```java
// Configuration Resilience4j
@Configuration
public class CircuitBreakerConfig {
    
    @Bean
    public CircuitBreaker stripeCircuitBreaker() {
        CircuitBreakerConfig config = CircuitBreakerConfig.custom()
            .failureRateThreshold(20)           // Ouvrir si > 20% échecs
            .waitDurationInOpenState(Duration.ofSeconds(60))
            .slidingWindowSize(100)
            .build();
            
        return CircuitBreaker.of("stripe-api", config);
    }
}

@Service
public class PaymentService {
    
    @CircuitBreaker(name = "stripe-api", fallbackMethod = "fallbackPayment")
    public Payment processPayment(PaymentRequest request) {
        return stripeClient.charge(request);
    }
    
    // Fallback : Mettre en queue pour retry ultérieur
    private Payment fallbackPayment(PaymentRequest request, Exception e) {
        log.warn("Stripe unavailable, queueing payment for retry", e);
        Payment payment = new Payment();
        payment.setStatus(PaymentStatus.PENDING);
        paymentQueue.enqueue(request);
        return paymentRepository.save(payment);
    }
}
```

**Déploiement du hotfix** :
```bash
# Build rapide
./mvnw clean package -DskipTests

# Deploy avec rolling update
kubectl set image deployment/payment-service \
  payment-service=payment-service:v2.3.1-hotfix \
  -n production

# Monitor le rollout
kubectl rollout status deployment/payment-service -n production
```

**Option 3 : Failover vers provider secondaire** (✅ Long terme)
```java
@Service
public class PaymentService {
    private final StripeClient stripeClient;
    private final PayPalClient paypalClient;
    
    public Payment processPayment(PaymentRequest request) {
        try {
            return stripeClient.charge(request);
        } catch (TimeoutException e) {
            log.warn("Stripe timeout, failing over to PayPal");
            return paypalClient.charge(request);
        }
    }
}
```

**Phase 7 : Monitoring de la Resolution (20-40 minutes)**

**1. Dashboard en temps réel** :
```promql
# Error rate après déploiement hotfix
rate(http_server_requests_seconds_count{status=~"5..",service="payment-service"}[1m])
```

**Évolution** :
- 14:47 : Error rate = 0.5% (baseline)
- 14:50 : Error rate = 15.2% 🔴 (pic)
- 15:05 : Hotfix déployé
- 15:08 : Error rate = 2.1% ⚠️ (amélioration)
- 15:12 : Error rate = 0.3% ✅ (résolu - circuit breaker actif)

**2. Vérifier les métriques métier** :
```promql
# Paiements en succès par minute
sum(rate(payments_successful_total[1m])) * 60
```

**Résultat** : 
- Avant incident : 450 payments/min
- Pendant incident : 380 payments/min (-15%)
- Après mitigation : 448 payments/min (✅ Normal)

**Phase 8 : Post-Mortem Documentation (après résolution)**

**Incident Report** :
```markdown
# Incident Report - Payment Service High Error Rate

## Summary
Payment service experienced 15% error rate due to Stripe API partial outage.

## Timeline
- **14:45** : Stripe begins experiencing issues (external)
- **14:47** : First errors detected in payment-service
- **14:50** : Critical alert triggered (15.2% error rate)
- **14:52** : Incident confirmed, investigation started
- **14:58** : Root cause identified (Stripe API timeouts)
- **15:05** : Hotfix deployed (circuit breaker activation)
- **15:12** : Error rate back to normal
- **15:30** : Stripe reports full recovery

## Impact
- Duration: 25 minutes (14:47 - 15:12)
- Failed payments: ~280 transactions
- Revenue impact: ~$42,000 in pending payments (queued for retry)
- User impact: ~1,200 users affected

## Root Cause
External dependency (Stripe API) partial outage causing 15% of requests to timeout.

## Resolution
1. Immediate: Deployed circuit breaker to prevent cascading failures
2. Queued failed payments for automatic retry
3. All queued payments successfully processed after Stripe recovery

## Action Items
- [ ] Implement multi-provider failover (Stripe → PayPal) - Priority: HIGH
- [ ] Reduce timeout from 5s to 3s for faster failure detection
- [ ] Add synthetic monitoring for Stripe API health
- [ ] Create runbook for payment provider outages
- [ ] Set up automatic alerting on Stripe status page changes

## Lessons Learned
✅ Observability stack performed excellently (MTTD: 3 min)
✅ Circuit breaker pattern proved effective
❌ No failover provider configured
❌ Should have synthetic monitoring for external dependencies
```

**Phase 9 : Prévention Future**

**1. Synthetic Monitoring** :
```yaml
# Grafana Synthetic Monitoring
checks:
  - name: stripe-api-health
    type: http
    target: https://api.stripe.com/v1/health
    interval: 30s
    timeout: 3s
    probes:
      - Paris
      - Frankfurt
    alerts:
      - name: Stripe API Down
        threshold: 2  # 2 probes failed
```

**2. Improved Circuit Breaker Dashboard** :
```promql
# Panels pour monitoring circuit breaker
resilience4j_circuitbreaker_state{name="stripe-api"}
resilience4j_circuitbreaker_failure_rate{name="stripe-api"}
resilience4j_circuitbreaker_calls_total{name="stripe-api",kind="successful"}
resilience4j_circuitbreaker_calls_total{name="stripe-api",kind="failed"}
```

**3. Alertes proactives** :
```yaml
# Alerte si Stripe répond lentement (avant timeout)
- alert: SlowExternalAPI
  expr: |
    histogram_quantile(0.95, 
      sum(rate(http_client_requests_seconds_bucket{service="stripe-api"}[5m])) by (le)
    ) > 2
  for: 5m
  annotations:
    summary: "Stripe API responding slowly"
    description: "P95 latency is {{ $value }}s, potential degradation"
```

**Métriques de Performance de l'Incident** :
- **MTTD (Mean Time To Detect)** : 3 minutes ✅
- **MTTK (Mean Time To Know)** : 11 minutes ✅
- **MTTR (Mean Time To Resolve)** : 25 minutes ✅
- **MTTF (Mean Time To Fix)** : 18 minutes (déploiement hotfix) ✅

**Conclusion** : Stack d'observability complète (Logs + Metrics + Traces) a permis une résolution rapide et efficace.

---

### Q18: Comment configureriez-vous l'observability pour une nouvelle application microservices Spring Boot from scratch?

**R:** Voici mon **blueprint complet** pour setup l'observability d'une nouvelle application :

**Phase 1 : Architecture et Décisions (Jour 1)**

**1. Analyse des besoins** :

**Questions clés** :
- Volumétrie attendue ? (ex: 1000 req/s → 2M logs/jour)
- Budget infrastructure ? (limité → Loki, large → ELK)
- Équipe existante ? (expertise Elastic → ELK)
- Environnement ? (Kubernetes → Grafana Stack natif)
- Compliance ? (rétention longue → ELK)

**Pour mon exemple** : Startup SaaS, Kubernetes, budget modéré, 5000 req/s
→ **Décision : Grafana Stack (Loki + Prometheus + Tempo)**

**2. Stack technique choisie** :
```
Application Layer:
├─ Spring Boot 3.2 + Spring Cloud
├─ SL4J + Logback (structured JSON)
├─ Micrometer + Actuator
└─ OpenTelemetry SDK

Collection Layer:
├─ Promtail (logs → Loki)
├─ Prometheus (metrics scraping)
└─ OpenTelemetry Collector (traces → Tempo)

Storage Layer:
├─ Grafana Loki (logs)
├─ Prometheus (metrics + remote storage Thanos)
└─ Grafana Tempo (traces)

Visualization:
└─ Grafana (unified UI)
```

**Phase 2 : Configuration Spring Boot (Jour 1-2)**

**1. Dependencies (pom.xml)** :
```xml
<properties>
    <spring-cloud.version>2023.0.0</spring-cloud.version>
</properties>

<dependencies>
    <!-- Actuator pour métriques -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-actuator</artifactId>
    </dependency>
    
    <!-- Micrometer pour Prometheus -->
    <dependency>
        <groupId>io.micrometer</groupId>
        <artifactId>micrometer-registry-prometheus</artifactId>
    </dependency>
    
    <!-- Tracing avec OpenTelemetry -->
    <dependency>
        <groupId>io.micrometer</groupId>
        <artifactId>micrometer-tracing-bridge-otel</artifactId>
    </dependency>
    <dependency>
        <groupId>io.opentelemetry</groupId>
        <artifactId>opentelemetry-exporter-otlp</artifactId>
    </dependency>
    
    <!-- Structured Logging -->
    <dependency>
        <groupId>net.logstash.logback</groupId>
        <artifactId>logstash-logback-encoder</artifactId>
        <version>7.4</version>
    </dependency>
</dependencies>
```

**2. Configuration application.yml** :
```yaml
spring:
  application:
    name: order-service
    
  # Actuator endpoints
  management:
    endpoints:
      web:
        exposure:
          include: health,info,prometheus,metrics
    endpoint:
      health:
        show-details: always
    metrics:
      tags:
        application: ${spring.application.name}
        environment: ${ENVIRONMENT:dev}
        version: ${APP_VERSION:unknown}
      distribution:
        percentiles-histogram:
          http.server.requests: true
        percentiles:
          http.server.requests: 0.5,0.95,0.99
    
    # Tracing
    tracing:
      sampling:
        probability: ${TRACING_SAMPLE_RATE:0.1}
    otlp:
      tracing:
        endpoint: http://tempo:4318
        
# Logging
logging:
  level:
    root: INFO
    com.company: DEBUG
  pattern:
    console: "%d{ISO8601} %5p [${spring.application.name:},%X{traceId:-},%X{spanId:-}] %t %c{1}:%L - %m%n"
```

**3. Logback configuration (logback-spring.xml)** :
```xml
<?xml version="1.0" encoding="UTF-8"?>
<configuration>
    <include resource="org/springframework/boot/logging/logback/defaults.xml"/>
    
    <springProperty scope="context" name="appName" source="spring.application.name"/>
    <springProperty scope="context" name="environment" source="ENVIRONMENT" defaultValue="dev"/>
    
    <!-- Console Appender avec JSON format -->
    <appender name="CONSOLE_JSON" class="ch.qos.logback.core.ConsoleAppender">
        <encoder class="net.logstash.logback.encoder.LogstashEncoder">
            <customFields>{"application":"${appName}","environment":"${environment}"}</customFields>
            <fieldNames>
                <timestamp>timestamp</timestamp>
                <message>message</message>
                <logger>logger</logger>
                <thread>thread</thread>
                <level>level</level>
                <stackTrace>exception</stackTrace>
            </fieldNames>
            <includeMdcKeyName>traceId</includeMdcKeyName>
            <includeMdcKeyName>spanId</includeMdcKeyName>
            <includeMdcKeyName>userId</includeMdcKeyName>
        </encoder>
    </appender>
    
    <!-- File Appender pour backup -->
    <appender name="FILE" class="ch.qos.logback.core.rolling.RollingFileAppender">
        <file>/var/log/app/${appName}.log</file>
        <encoder class="net.logstash.logback.encoder.LogstashEncoder"/>
        <rollingPolicy class="ch.qos.logback.core.rolling.TimeBasedRollingPolicy">
            <fileNamePattern>/var/log/app/${appName}-%d{yyyy-MM-dd}.log.gz</fileNamePattern>
            <maxHistory>30</maxHistory>
        </rollingPolicy>
    </appender>
    
    <root level="INFO">
        <appender-ref ref="CONSOLE_JSON"/>
        <appender-ref ref="FILE"/>
    </root>
</configuration>
```

**4. Custom Metrics Configuration** :
```java
@Configuration
public class MetricsConfig {
    
    @Bean
    public MeterRegistryCustomizer<MeterRegistry> metricsCommonTags(
            @Value("${spring.application.name}") String appName) {
        return registry -> registry.config()
            .commonTags(
                "application", appName,
                "environment", System.getenv("ENVIRONMENT")
            );
    }
    
    @Bean
    public TimedAspect timedAspect(MeterRegistry registry) {
        return new TimedAspect(registry);
    }
}
```

**Phase 3 : Infrastructure Kubernetes (Jour 2-3)**

**1. Deployment avec annotations** :
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
  namespace: production
spec:
  replicas: 3
  selector:
    matchLabels:
      app: order-service
  template:
    metadata:
      labels:
        app: order-service
        version: v1.0.0
      annotations:
        # Prometheus scraping
        prometheus.io/scrape: "true"
        prometheus.io/port: "8080"
        prometheus.io/path: "/actuator/prometheus"
    spec:
      containers:
      - name: order-service
        image: order-service:v1.0.0
        ports:
        - containerPort: 8080
          name: http
        env:
        - name: ENVIRONMENT
          value: "production"
        - name: TRACING_SAMPLE_RATE
          value: "0.1"
        - name: JAVA_OPTS
          value: "-Xmx512m -XX:+UseG1GC"
        
        # Health checks
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8080
          initialDelaySeconds: 60
          periodSeconds: 10
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          initialDelaySeconds: 30
          periodSeconds: 5
        
        # Resource limits
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
        
        # Log to stdout
        volumeMounts:
        - name: logs
          mountPath: /var/log/app
      
      # Promtail sidecar pour collecter logs
      - name: promtail
        image: grafana/promtail:latest
        args:
        - -config.file=/etc/promtail/promtail.yaml
        volumeMounts:
        - name: logs
          mountPath: /var/log/app
        - name: promtail-config
          mountPath: /etc/promtail
      
      volumes:
      - name: logs
        emptyDir: {}
      - name: promtail-config
        configMap:
          name: promtail-config
```

**2. Promtail ConfigMap** :
```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: promtail-config
  namespace: production
data:
  promtail.yaml: |
    server:
      http_listen_port: 9080
    
    positions:
      filename: /tmp/positions.yaml
    
    clients:
      - url: http://loki:3100/loki/api/v1/push
    
    scrape_configs:
      - job_name: spring-boot-logs
        static_configs:
          - targets:
              - localhost
            labels:
              job: spring-boot
              __path__: /var/log/app/*.log
        
        pipeline_stages:
          # Parse JSON logs
          - json:
              expressions:
                timestamp: timestamp
                level: level
                message: message
                logger: logger
                thread: thread
                traceId: traceId
                spanId: spanId
          
          # Extract labels
          - labels:
              level:
              traceId:
              spanId:
          
          # Parse timestamp
          - timestamp:
              source: timestamp
              format: RFC3339
```

**3. Prometheus ServiceMonitor** :
```yaml
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: order-service-metrics
  namespace: production
spec:
  selector:
    matchLabels:
      app: order-service
  endpoints:
  - port: http
    path: /actuator/prometheus
    interval: 15s
    scrapeTimeout: 10s
```

**Phase 4 : Grafana Configuration (Jour 3-4)**

**1. Data Sources** :
```yaml
apiVersion: 1
datasources:
  - name: Prometheus
    type: prometheus
    access: proxy
    url: http://prometheus:9090
    isDefault: true
    jsonData:
      timeInterval: 15s
      
  - name: Loki
    type: loki
    access: proxy
    url: http://loki:3100
    jsonData:
      derivedFields:
        - datasourceUid: tempo
          matcherRegex: "traceId=(\\w+)"
          name: TraceID
          url: "$${__value.raw}"
          
  - name: Tempo
    type: tempo
    access: proxy
    url: http://tempo:3200
    jsonData:
      tracesToLogs:
        datasourceUid: loki
        filterByTraceID: true
        filterBySpanID: false
        tags: ['service', 'environment']
      nodeGraph:
        enabled: true
```

**2. Dashboard "Service Overview"** (exporté en JSON) :
```json
{
  "dashboard": {
    "title": "Order Service Overview",
    "tags": ["spring-boot", "microservices"],
    "panels": [
      {
        "title": "Request Rate",
        "targets": [{
          "expr": "sum(rate(http_server_requests_seconds_count{application=\"order-service\"}[5m])) * 60"
        }]
      },
      {
        "title": "Error Rate",
        "targets": [{
          "expr": "(sum(rate(http_server_requests_seconds_count{application=\"order-service\",status=~\"5..\"}[5m])) / sum(rate(http_server_requests_seconds_count{application=\"order-service\"}[5m]))) * 100"
        }]
      },
      {
        "title": "Latency (P50, P95, P99)",
        "targets": [
          {
            "expr": "histogram_quantile(0.50, sum(rate(http_server_requests_seconds_bucket{application=\"order-service\"}[5m])) by (le))",
            "legendFormat": "P50"
          },
          {
            "expr": "histogram_quantile(0.95, sum(rate(http_server_requests_seconds_bucket{application=\"order-service\"}[5m])) by (le))",
            "legendFormat": "P95"
          },
          {
            "expr": "histogram_quantile(0.99, sum(rate(http_server_requests_seconds_bucket{application=\"order-service\"}[5m])) by (le))",
            "legendFormat": "P99"
          }
        ]
      },
      {
        "title": "JVM Memory",
        "targets": [{
          "expr": "jvm_memory_used_bytes{application=\"order-service\",area=\"heap\"} / jvm_memory_max_bytes{application=\"order-service\",area=\"heap\"} * 100"
        }]
      },
      {
        "title": "Recent Error Logs",
        "type": "logs",
        "targets": [{
          "expr": "{application=\"order-service\", level=\"ERROR\"}"
        }]
      }
    ]
  }
}
```

**Phase 5 : Alerting (Jour 4-5)**

**1. Alert Rules** :
```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: grafana-alerts
data:
  alerts.yaml: |
    groups:
      - name: order-service-alerts
        interval: 1m
        rules:
          - alert: OrderServiceDown
            expr: up{job="order-service"} == 0
            for: 2m
            labels:
              severity: critical
            annotations:
              summary: "Order Service is DOWN"
              dashboard_url: "https://grafana/d/order-service"
              
          - alert: HighErrorRate
            expr: |
              (sum(rate(http_server_requests_seconds_count{application="order-service",status=~"5.."}[5m])) 
              / 
              sum(rate(http_server_requests_seconds_count{application="order-service"}[5m]))) * 100 > 5
            for: 5m
            labels:
              severity: critical
            
          - alert: HighLatency
            expr: |
              histogram_quantile(0.99, 
                sum(rate(http_server_requests_seconds_bucket{application="order-service"}[5m])) by (le)
              ) > 2
            for: 10m
            labels:
              severity: warning
```

**2. Notification Channels** :
```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: alertmanager-config
data:
  alertmanager.yml: |
    route:
      receiver: 'slack-notifications'
      group_by: ['alertname', 'service']
      group_wait: 30s
      group_interval: 5m
      repeat_interval: 4h
      
      routes:
        - match:
            severity: critical
          receiver: 'slack-critical'
          repeat_interval: 30m
    
    receivers:
      - name: 'slack-notifications'
        slack_configs:
          - api_url: 'https://hooks.slack.com/services/XXX'
            channel: '#alerts'
            
      - name: 'slack-critical'
        slack_configs:
          - api_url: 'https://hooks.slack.com/services/XXX'
            channel: '#alerts-critical'
```

**Phase 6 : Documentation et Runbooks (Jour 5)**

**1. README Observability** :
```markdown
# Order Service - Observability Guide

## Quick Links
- **Grafana Dashboard**: https://grafana/d/order-service
- **Prometheus**: https://prometheus/graph
- **Loki Logs**: https://grafana/explore (Loki datasource)
- **Tempo Traces**: https://grafana/explore (Tempo datasource)

## Key Metrics
- `http_server_requests_seconds_count` - Request count
- `http_server_requests_seconds_sum` - Total request duration
- `jvm_memory_used_bytes` - JVM memory usage

## Common Queries

### Find error logs
```logql
{application="order-service", level="ERROR"} | json
```

### Get P99 latency
```promql
histogram_quantile(0.99, 
  sum(rate(http_server_requests_seconds_bucket{application="order-service"}[5m])) by (le)
)
```

### Find slow traces
```traceql
{service.name="order-service" && duration > 1s}
```

## Runbooks
- [High Error Rate](./runbooks/high-error-rate.md)
- [High Latency](./runbooks/high-latency.md)
- [Service Down](./runbooks/service-down.md)
```

**Phase 7 : Testing (Jour 5-6)**

**1. Load Testing avec observability** :
```javascript
// load-test.js pour K6
import http from 'k6/http';
import { check, sleep } from 'k6';
import { Rate } from 'k6/metrics';

const errorRate = new Rate('errors');

export const options = {
  stages: [
    { duration: '2m', target: 100 },  // Ramp up
    { duration: '5m', target: 100 },  // Stay at 100 users
    { duration: '2m', target: 0 },    // Ramp down
  ],
  thresholds: {
    'http_req_duration': ['p(95)<500', 'p(99)<1000'],
    'errors': ['rate<0.05'],  // Error rate < 5%
  },
};

export default function () {
  const responses = http.batch([
    ['GET', 'http://order-service/api/orders'],
    ['POST', 'http://order-service/api/orders', JSON.stringify({
      userId: 'user-123',
      items: [{ productId: 'prod-1', quantity: 2 }]
    }), { headers: { 'Content-Type': 'application/json' } }],
  ]);
  
  responses.forEach(res => {
    const result = check(res, {
      'status is 200': (r) => r.status === 200,
      'response time < 500ms': (r) => r.timings.duration < 500,
    });
    errorRate.add(!result);
  });
  
  sleep(1);
}
```

**Pendant le test, vérifier** :
```bash
# Dashboard Grafana : Request rate monte à ~100 req/s
# Logs Loki : Voir les logs défiler en temps réel
# Traces Tempo : Échantillon de traces capturées
# Alertes : Aucune alerte ne doit se déclencher si tout va bien
```

**2. Tests de Chaos Engineering** :
```bash
# Tester la résilience avec Chaos Mesh

# Simuler latency réseau
kubectl apply -f - <<EOF
apiVersion: chaos-mesh.org/v1alpha1
kind: NetworkChaos
metadata:
  name: network-delay
  namespace: production
spec:
  action: delay
  mode: one
  selector:
    namespaces:
      - production
    labelSelectors:
      app: order-service
  delay:
    latency: "500ms"
    correlation: "50"
  duration: "5m"
EOF

# Observer dans Grafana :
# - P99 latency augmente
# - Alertes se déclenchent si configuré
# - Traces montrent la latency ajoutée
```

**3. Validation de la corrélation Logs-Metrics-Traces** :
```bash
# Générer une requête spécifique
curl -X POST http://order-service/api/orders \
  -H "Content-Type: application/json" \
  -d '{"userId":"test-user","items":[{"productId":"test-prod","quantity":1}]}' \
  -v

# Récupérer le traceId de la réponse
# Exemple: traceId=a1b2c3d4e5f6g7h8

# Dans Grafana :
# 1. Chercher la trace dans Tempo avec ce traceId
# 2. Cliquer sur "View Logs" → doit montrer les logs avec ce traceId
# 3. Vérifier que les métriques de cette requête apparaissent dans Prometheus
```

**Phase 8 : Optimisation et Tuning (Jour 6-7)**

**1. Ajustement des sampling rates** :

Après observation du trafic réel :
```yaml
# Initial config (trop conservateur)
spring:
  sleuth:
    sampler:
      probability: 0.1  # 10%

# Après analyse, ajuster :
# - Dev: 100% (debug facile)
# - Staging: 50% (bon équilibre)
# - Production: 5% base + règles adaptatives (coût optimal)
```

**2. Optimisation des index Loki** :
```yaml
# Ajuster les labels pour cardinalité optimale
scrape_configs:
  - job_name: spring-boot-logs
    pipeline_stages:
      - labels:
          # Labels à faible cardinalité (BIEN)
          application:
          environment:
          level:
          # PAS de labels à haute cardinalité (MAL)
          # userId:  ❌
          # requestId: ❌
          # orderId: ❌
```

**3. Tuning Prometheus retention** :
```yaml
# prometheus.yml
global:
  scrape_interval: 15s
  evaluation_interval: 15s

# Retention basée sur le volume
storage:
  tsdb:
    retention.time: 15d  # Données locales
    
# Remote storage Thanos pour long-terme
remote_write:
  - url: http://thanos-receive:19291/api/v1/receive
    queue_config:
      capacity: 10000
      max_samples_per_send: 5000
```

**4. Dashboard Performance Optimization** :
```json
{
  "panels": [
    {
      "title": "Request Rate",
      "targets": [{
        // AVANT (lent)
        "expr": "sum(rate(http_server_requests_seconds_count[5m]))"
        
        // APRÈS (rapide) - pre-aggregated recording rules
        "expr": "job:http_requests:rate5m"
      }]
    }
  ]
}
```

**Recording Rules Prometheus** :
```yaml
# prometheus-rules.yml
groups:
  - name: http_metrics
    interval: 30s
    rules:
      # Pre-aggregate pour dashboard performance
      - record: job:http_requests:rate5m
        expr: sum(rate(http_server_requests_seconds_count[5m])) by (job, application)
        
      - record: job:http_requests_errors:rate5m
        expr: sum(rate(http_server_requests_seconds_count{status=~"5.."}[5m])) by (job, application)
        
      - record: job:http_requests_latency:p99
        expr: histogram_quantile(0.99, sum(rate(http_server_requests_seconds_bucket[5m])) by (le, job))
```

**Phase 9 : Formation et Documentation (Jour 7)**

**1. Guide rapide pour les développeurs** :
```markdown
# Developer Quick Start - Observability

## Adding Custom Metrics

```java
@Service
public class OrderService {
    private final Counter orderCounter;
    private final Timer processingTimer;
    
    public OrderService(MeterRegistry registry) {
        this.orderCounter = registry.counter("orders.created",
            "type", "online");
        this.processingTimer = registry.timer("order.processing.time");
    }
    
    public Order createOrder(OrderRequest request) {
        return processingTimer.record(() -> {
            Order order = processOrder(request);
            orderCounter.increment();
            return order;
        });
    }
}
```

## Adding Contextual Logs

```java
@RestController
public class OrderController {
    
    @PostMapping("/orders")
    public ResponseEntity<Order> createOrder(@RequestBody OrderRequest request) {
        // MDC automatically includes traceId/spanId
        MDC.put("userId", request.getUserId());
        MDC.put("orderId", order.getId());
        
        log.info("Creating order", 
            kv("amount", request.getAmount()),
            kv("itemCount", request.getItems().size()));
        
        try {
            Order order = orderService.create(request);
            log.info("Order created successfully");
            return ResponseEntity.ok(order);
        } catch (Exception e) {
            log.error("Order creation failed", e);
            throw e;
        } finally {
            MDC.clear();
        }
    }
}
```

## Adding Custom Spans

```java
@Service
public class PaymentService {
    private final Tracer tracer;
    
    public Payment processPayment(PaymentRequest request) {
        Span span = tracer.nextSpan().name("payment-processing").start();
        
        try (Tracer.SpanInScope ws = tracer.withSpanInScope(span)) {
            span.tag("payment.provider", "stripe");
            span.tag("payment.amount", String.valueOf(request.getAmount()));
            span.tag("payment.currency", request.getCurrency());
            
            Payment payment = stripeClient.charge(request);
            
            span.event("payment-authorized");
            
            return payment;
        } catch (Exception e) {
            span.error(e);
            throw e;
        } finally {
            span.end();
        }
    }
}
```

## Debugging Tips

### Find your logs
```logql
{application="order-service"} 
| json 
| userId="user-123"
| level="ERROR"
```

### Find slow requests
```traceql
{service.name="order-service" && duration > 1s}
| select(name, duration)
```

### Check your metrics
```promql
# Your request rate
rate(http_server_requests_seconds_count{application="order-service"}[5m])

# Your custom metrics
rate(orders_created_total[5m])
```
```

**2. Runbook Template** :
```markdown
# Runbook: High Error Rate

## Symptoms
- Alert: "HighErrorRate" firing
- Error rate > 5% for 5+ minutes
- Users reporting failed requests

## Quick Checks
1. **Check Dashboard**: https://grafana/d/order-service
   - What's the current error rate?
   - What endpoints are affected?
   - When did it start?

2. **Check Recent Deployments**:
   ```bash
   kubectl rollout history deployment/order-service -n production
   ```
   - Any deployment in last 30 minutes? Consider rollback

3. **Check Error Logs**:
   ```logql
   {application="order-service", level="ERROR"} | json
   ```
   - What error messages are most common?
   - Any stack traces with root cause?

4. **Check Dependent Services**:
   - Is payment-service healthy?
   - Is inventory-service responding?
   - Any external API issues? (check status pages)

## Investigation Steps

### Step 1: Identify Pattern
```promql
# Error rate by endpoint
sum(rate(http_server_requests_seconds_count{status=~"5..",application="order-service"}[5m])) by (uri)
```

### Step 2: Examine Traces
- Find failing traceId from logs
- Open in Tempo: https://grafana/explore
- Identify which span is failing

### Step 3: Check External Dependencies
```promql
# External API latency
histogram_quantile(0.99, rate(http_client_requests_seconds_bucket[5m])) by (uri)
```

## Common Causes & Solutions

### Cause 1: External API Outage
**Symptoms**: Timeouts to specific external service
**Solution**: 
- Enable circuit breaker
- Switch to fallback provider if available
- Queue requests for retry

### Cause 2: Database Connection Pool Exhausted
**Symptoms**: `HikariCP connection timeout`
**Solution**:
```bash
# Check pool metrics
curl http://order-service:8080/actuator/metrics/hikaricp.connections.active

# Increase pool size if needed
kubectl set env deployment/order-service HIKARI_MAX_POOL_SIZE=20
```

### Cause 3: Memory Leak / OOM
**Symptoms**: 
- Heap usage > 90%
- GC pauses increasing
- Eventually OOMKilled

**Solution**:
```bash
# Check heap usage
curl http://order-service:8080/actuator/metrics/jvm.memory.used

# Restart pod if critical
kubectl delete pod order-service-xxx -n production

# Longer term: heap dump analysis
kubectl exec -it order-service-xxx -- jmap -dump:live,format=b,file=/tmp/heap.hprof 1
```

## Escalation
If not resolved in 15 minutes:
1. Page on-call architect: @john-doe
2. Create incident in PagerDuty
3. Start war room: #incident-response

## Post-Incident
- [ ] Create post-mortem document
- [ ] Update runbook with new findings
- [ ] Create Jira tickets for preventive actions
```

**Phase 10 : Monitoring Production (Jour 7 - Ongoing)**

**1. Health Check Dashboard** :
```json
{
  "dashboard": {
    "title": "Production Health Overview",
    "panels": [
      {
        "title": "Services Status",
        "type": "stat",
        "targets": [{
          "expr": "up{job=~\".*-service\"}"
        }],
        "thresholds": {
          "mode": "absolute",
          "steps": [
            { "value": 0, "color": "red" },
            { "value": 1, "color": "green" }
          ]
        }
      },
      {
        "title": "Error Budget (30 days)",
        "type": "gauge",
        "targets": [{
          "expr": "(1 - (sum(increase(http_server_requests_seconds_count{status=~\"5..\"}[30d])) / sum(increase(http_server_requests_seconds_count[30d])))) * 100"
        }],
        "thresholds": {
          "steps": [
            { "value": 0, "color": "red" },
            { "value": 99.5, "color": "orange" },
            { "value": 99.9, "color": "green" }
          ]
        }
      },
      {
        "title": "Observability Pipeline Health",
        "type": "table",
        "targets": [
          {
            "expr": "sum(rate(promtail_sent_entries_total[5m])) by (job)",
            "format": "table",
            "legendFormat": "Logs/s"
          },
          {
            "expr": "sum(rate(prometheus_tsdb_head_samples_appended_total[5m]))",
            "format": "table",
            "legendFormat": "Metrics/s"
          },
          {
            "expr": "sum(rate(tempo_ingester_traces_created_total[5m]))",
            "format": "table",
            "legendFormat": "Traces/s"
          }
        ]
      }
    ]
  }
}
```

**2. Weekly Review Metrics** :
```markdown
# Weekly Observability Review - Template

## Period: [Date Range]

### Volume Metrics
- **Total Requests**: X million
- **Logs Generated**: X GB
- **Traces Captured**: X thousand
- **Metrics Data Points**: X million

### Quality Metrics
- **Uptime**: 99.XX%
- **P99 Latency**: XXX ms
- **Error Rate**: X.XX%
- **MTTD (Mean Time To Detect)**: XX minutes
- **MTTR (Mean Time To Resolve)**: XX minutes

### Incidents
- **Total Incidents**: X
- **Critical**: X
- **Major**: X
- **Minor**: X

### Top Issues (by frequency)
1. Issue A - XX occurrences
2. Issue B - XX occurrences
3. Issue C - XX occurrences

### Action Items
- [ ] Optimize slow query identified in trace XYZ
- [ ] Add alert for new pattern discovered
- [ ] Update dashboard with new business metric

### Cost Analysis
- **Storage**: $XXX
- **Compute**: $XXX
- **Total**: $XXX
- **Change vs last week**: +/- X%
```

**3. Continuous Improvement** :
```yaml
# Backlog items template
observability_improvements:
  
  high_priority:
    - title: "Add synthetic monitoring for critical user journeys"
      effort: 5 days
      impact: "Proactive detection of UX issues"
      
    - title: "Implement distributed tracing for async messaging"
      effort: 3 days
      impact: "Better visibility in event-driven flows"
      
  medium_priority:
    - title: "Create business metrics dashboards for product team"
      effort: 2 days
      impact: "Better product insights"
      
    - title: "Set up log anomaly detection with ML"
      effort: 8 days
      impact: "Detect unknown unknowns"
      
  low_priority:
    - title: "Migrate from Loki to ELK for compliance requirements"
      effort: 15 days
      impact: "Better long-term retention and search"
```

**Phase 11 : Checklist Final (Acceptance Criteria)**

**✅ Configuration Completeness** :
- [x] Structured logging avec JSON format
- [x] Logs incluent traceId/spanId
- [x] MDC configuré pour contexte métier
- [x] Rotation et rétention configurées
- [x] Micrometer + Actuator activés
- [x] Métriques custom ajoutées
- [x] Prometheus scraping configuré
- [x] Percentiles configurés (P50, P95, P99)
- [x] OpenTelemetry SDK intégré
- [x] Trace context propagation testée
- [x] Sampling strategy configurée
- [x] Spans custom pour opérations critiques

**✅ Infrastructure** :
- [x] Promtail déployé et fonctionnel
- [x] Loki stockage configuré avec rétention
- [x] Prometheus scraping tous les services
- [x] Tempo recevant les traces
- [x] Grafana datasources configurées
- [x] Service discovery K8s fonctionnel

**✅ Dashboards & Visualization** :
- [x] Dashboard "Service Overview" créé
- [x] Dashboard "JVM Metrics" créé
- [x] Dashboard "Business Metrics" créé
- [x] Dashboard "Observability Health" créé
- [x] Variables dashboard configurées (service, env)
- [x] Links entre logs/metrics/traces testés

**✅ Alerting** :
- [x] Alertes critiques configurées (service down, high error rate)
- [x] Alertes warning configurées (high latency, high memory)
- [x] Notification channels configurés (Slack, Email)
- [x] Grace periods et throttling configurés
- [x] Alertes testées avec chaos engineering

**✅ Documentation** :
- [x] README observability créé
- [x] Runbooks pour incidents communs
- [x] Guide développeur pour métriques custom
- [x] Architecture diagram mis à jour
- [x] Onboarding document pour nouveaux devs

**✅ Testing & Validation** :
- [x] Load testing avec observabilité activée
- [x] Chaos engineering scenarios testés
- [x] Corrélation logs-metrics-traces validée
- [x] Performance dashboards sous charge
- [x] Alertes déclenchées correctement

**Résultat Final** :
- **Setup Time**: 7 jours (1 semaine)
- **Services Monitored**: 100%
- **Alert Coverage**: 95% des scénarios critiques
- **MTTD Target**: < 5 minutes
- **MTTR Target**: < 30 minutes
- **Team Trained**: 100%
- **Documentation Complete**: ✅
- **Production Ready**: ✅

---

### Q19: Comment gérez-vous l'observability dans un contexte multi-tenant ou multi-environment?

**R:** La gestion de l'observability en **multi-tenant** et **multi-environment** nécessite une architecture et une stratégie spécifiques :

**1. Architecture Multi-Environment**

**Stratégie de séparation** :

```
Environments:
├─ Development
│  ├─ Grafana Stack (shared)
│  ├─ Retention: 7 days
│  └─ Sampling: 100%
├─ Staging
│  ├─ Grafana Stack (shared)
│  ├─ Retention: 30 days
│  └─ Sampling: 50%
└─ Production
   ├─ Grafana Stack (dedicated)
   ├─ Retention: 90 days
   └─ Sampling: 10% + adaptive
```

**A. Labeling Strategy** :
```yaml
# Application configuration
spring:
  application:
    name: order-service
    
management:
  metrics:
    tags:
      # Core labels
      application: ${spring.application.name}
      environment: ${ENVIRONMENT:dev}  # dev, staging, prod
      region: ${AWS_REGION:eu-west-1}
      cluster: ${CLUSTER_NAME:main}
      version: ${APP_VERSION:unknown}
      
      # Business labels (multi-tenant)
      tenant: ${TENANT_ID:default}
      tier: ${SERVICE_TIER:standard}  # free, standard, premium
```

**B. Promtail Configuration Multi-Env** :
```yaml
scrape_configs:
  - job_name: spring-boot-logs
    static_configs:
      - targets:
          - localhost
        labels:
          job: spring-boot
          environment: ${ENVIRONMENT}
          cluster: ${CLUSTER_NAME}
          __path__: /var/log/app/*.log
    
    pipeline_stages:
      - json:
          expressions:
            timestamp: timestamp
            level: level
            tenant: tenant
            
      - labels:
          environment:
          tenant:
          level:
          
      # Filtrer les logs selon l'environnement
      - match:
          selector: '{level="DEBUG"}'
          action: drop
          # Drop DEBUG logs in production
          drop_counter_reason: debug_in_prod
```

**C. Prometheus Federation Multi-Environment** :
```yaml
# Prometheus central pour tous les environments
scrape_configs:
  # Development cluster
  - job_name: 'federate-dev'
    scrape_interval: 30s
    honor_labels: true
    metrics_path: '/federate'
    params:
      'match[]':
        - '{job=~".*-service"}'
    static_configs:
      - targets:
        - 'prometheus-dev.internal:9090'
        labels:
          environment: 'dev'
          
  # Staging cluster
  - job_name: 'federate-staging'
    scrape_interval: 30s
    honor_labels: true
    metrics_path: '/federate'
    params:
      'match[]':
        - '{job=~".*-service"}'
    static_configs:
      - targets:
        - 'prometheus-staging.internal:9090'
        labels:
          environment: 'staging'
          
  # Production cluster
  - job_name: 'federate-prod'
    scrape_interval: 15s  # Plus fréquent pour prod
    honor_labels: true
    metrics_path: '/federate'
    params:
      'match[]':
        - '{job=~".*-service"}'
    static_configs:
      - targets:
        - 'prometheus-prod.internal:9090'
        labels:
          environment: 'production'
```

**2. Architecture Multi-Tenant**

**A. Tenant Isolation Strategy** :

**Option 1 : Logical Separation (Shared Infrastructure)** :
```yaml
# Bon pour : SaaS avec beaucoup de petits tenants
# Prometheus - Tous les tenants dans la même instance
# Loki - Tous les logs avec label tenant
# Grafana - RBAC pour isoler les vues

# Application code
@Service
public class TenantContextFilter implements Filter {
    @Override
    public void doFilter(ServletRequest request, ServletResponse response, FilterChain chain) {
        String tenantId = extractTenantId(request);
        
        // Injecter dans MDC pour logs
        MDC.put("tenant", tenantId);
        
        // Injecter dans metrics
        Metrics.globalRegistry.config().commonTags("tenant", tenantId);
        
        // Injecter dans traces
        Span currentSpan = tracer.currentSpan();
        if (currentSpan != null) {
            currentSpan.tag("tenant", tenantId);
        }
        
        try {
            chain.doFilter(request, response);
        } finally {
            MDC.clear();
        }
    }
}
```

**Query Loki par tenant** :
```logql
# Logs d'un tenant spécifique
{application="order-service", tenant="acme-corp"} | json

# Logs de tous les tenants premium
{application="order-service", tier="premium"} | json

# Aggregation par tenant
sum by (tenant) (count_over_time({application="order-service"} [1h]))
```

**Query Prometheus par tenant** :
```promql
# Request rate par tenant
sum(rate(http_server_requests_seconds_count{tenant="acme-corp"}[5m]))

# Comparer tous les tenants
sum by (tenant) (rate(http_server_requests_seconds_count[5m]))

# Top 10 tenants par volume
topk(10, sum by (tenant) (rate(http_server_requests_seconds_count[5m])))
```

**Option 2 : Physical Separation (Dedicated Infrastructure)** :
```yaml
# Bon pour : Enterprise tenants avec compliance stricte

Tenant A (Enterprise):
├─ Dedicated Grafana Stack
├─ Dedicated K8s Namespace
├─ Dedicated Prometheus
├─ Dedicated Loki
└─ Compliance: HIPAA, SOC2

Tenant B (Enterprise):
├─ Dedicated Grafana Stack
├─ Dedicated K8s Namespace
└─ Compliance: PCI-DSS

Tenants C-Z (SMB):
└─ Shared Grafana Stack (avec RBAC)
```

**B. Grafana RBAC pour Multi-Tenant** :
```yaml
# Grafana datasource avec variable tenant
apiVersion: 1
datasources:
  - name: Loki-Tenant-A
    type: loki
    access: proxy
    url: http://loki:3100
    jsonData:
      # Filter automatique pour ce datasource
      derivedFields:
        - name: tenant
          value: tenant-a
      httpHeaderName1: 'X-Scope-OrgID'
    secureJsonData:
      httpHeaderValue1: 'tenant-a'
```

**Grafana Teams & Permissions** :
```yaml
# Create team for each tenant
resource "grafana_team" "tenant_a" {
  name  = "Tenant-A"
  email = "ops@tenant-a.com"
}

# Create folder for tenant
resource "grafana_folder" "tenant_a" {
  title = "Tenant A Dashboards"
}

# Grant permissions
resource "grafana_folder_permission" "tenant_a" {
  folder_uid = grafana_folder.tenant_a.uid
  
  permissions {
    team_id    = grafana_team.tenant_a.id
    permission = "Edit"
  }
}

# Dashboard with tenant filter
resource "grafana_dashboard" "tenant_a_overview" {
  folder = grafana_folder.tenant_a.id
  config_json = jsonencode({
    title = "Tenant A - Overview"
    templating = {
      list = [
        {
          name = "tenant"
          type = "constant"
          current = {
            value = "tenant-a"
          }
          hide = 2  # Hidden from UI
        }
      ]
    }
    panels = [
      {
        targets = [{
          expr = "rate(http_server_requests_seconds_count{tenant=\"$tenant\"}[5m])"
        }]
      }
    ]
  })
}
```

**C. Cost Allocation par Tenant** :

**Tracking des coûts dans Prometheus** :
```promql
# Volume de données par tenant (logs)
sum by (tenant) (promtail_sent_bytes_total)

# Nombre de métriques par tenant
count by (tenant) ({__name__=~".+"})

# Nombre de traces par tenant
sum by (tenant) (rate(tempo_ingester_traces_created_total[1h]))

# Dashboard "Cost per Tenant"
```

**Alert sur usage excessif** :
```yaml
groups:
  - name: tenant_quotas
    rules:
      - alert: TenantExcessiveLogVolume
        expr: |
          sum by (tenant) (rate(promtail_sent_bytes_total[1h])) > 100000000  # 100MB/h
        for: 1h
        labels:
          severity: warning
        annotations:
          summary: "Tenant {{ $labels.tenant }} exceeding log quota"
          description: "Current rate: {{ $value | humanize }}B/h"
          
      - alert: TenantExcessiveRequestRate
        expr: |
          sum by (tenant) (rate(http_server_requests_seconds_count[5m])) > 1000
        for: 10m
        annotations:
          summary: "Tenant {{ $labels.tenant }} exceeding request quota"
```

**3. Dashboard Multi-Environment/Tenant**

**Dashboard avec template variables** :
```json
{
  "dashboard": {
    "title": "Multi-Environment Service Overview",
    "templating": {
      "list": [
        {
          "name": "environment",
          "type": "query",
          "datasource": "Prometheus",
          "query": "label_values(up, environment)",
          "multi": true,
          "includeAll": true
        },
        {
          "name": "tenant",
          "type": "query",
          "datasource": "Prometheus",
          "query": "label_values(http_server_requests_seconds_count{environment=~\"$environment\"}, tenant)",
          "multi": true,
          "includeAll": true
        },
        {
          "name": "service",
          "type": "query",
          "datasource": "Prometheus",
          "query": "label_values(up{environment=~\"$environment\"}, application)",
          "multi": false
        }
      ]
    },
    "panels": [
      {
        "title": "Request Rate by Environment",
        "targets": [{
          "expr": "sum by (environment) (rate(http_server_requests_seconds_count{environment=~\"$environment\",tenant=~\"$tenant\",application=\"$service\"}[5m]))"
        }]
      },
      {
        "title": "Error Rate Comparison",
        "targets": [{
          "expr": "(sum by (environment) (rate(http_server_requests_seconds_count{status=~\"5..\",environment=~\"$environment\",tenant=~\"$tenant\"}[5m])) / sum by (environment) (rate(http_server_requests_seconds_count{environment=~\"$environment\",tenant=~\"$tenant\"}[5m]))) * 100",
          "legendFormat": "{{ environment }}"
        }]
      },
      {
        "title": "Tenant Comparison",
        "targets": [{
          "expr": "sum by (tenant) (rate(http_server_requests_seconds_count{environment=~\"$environment\",tenant=~\"$tenant\"}[5m]))",
          "legendFormat": "{{ tenant }}"
        }]
      }
    ]
  }
}
```

**4. Best Practices Multi-Tenant/Env**

**✅ DO** :
- Utiliser des labels consistants (environment, tenant) partout
- Implémenter RBAC strict dans Grafana
- Monitorer les quotas et coûts par tenant
- Séparer physiquement les tenants enterprise si compliance requise
- Automatiser l'onboarding de nouveaux tenants
- Documenter clairement les limites par tier (free/standard/premium)
- Implémenter rate limiting par tenant dans l'application
- Auditer régulièrement les accès aux données

**❌ DON'T** :
- Ne pas utiliser des identifiants tenant à haute cardinalité comme labels Prometheus
- Ne pas exposer les données d'un tenant à un autre (même accidentellement)
- Ne pas oublier de nettoyer les données des tenants désactivés
- Ne pas configurer des alertes globales sans filtrage par environnement
- Ne pas mélanger les données prod et non-prod dans le même stockage

**5. Automation du Multi-Tenant**

**Script d'onboarding automatique** :
```bash
#!/bin/bash
# onboard-tenant.sh

TENANT_ID=$1
TENANT_NAME=$2
TIER=$3  # free, standard, premium

echo "Onboarding tenant: $TENANT_NAME ($TENANT_ID)"

# 1. Créer namespace Kubernetes si isolation physique
kubectl create namespace tenant-${TENANT_ID}

# 2. Créer Grafana Team
curl -X POST http://grafana/api/teams \
  -H "Authorization: Bearer ${GRAFANA_TOKEN}" \
  -d "{\"name\":\"${TENANT_NAME}\",\"email\":\"ops@${TENANT_ID}.com\"}"

TEAM_ID=$(curl -s http://grafana/api/teams/search?name=${TENANT_NAME} \
  -H "Authorization: Bearer ${GRAFANA_TOKEN}" | jq -r '.teams[0].id')

# 3. Créer Grafana Folder
curl -X POST http://grafana/api/folders \
  -H "Authorization: Bearer ${GRAFANA_TOKEN}" \
  -d "{\"title\":\"${TENANT_NAME} Dashboards\"}"

FOLDER_UID=$(curl -s http://grafana/api/folders \
  -H "Authorization: Bearer ${GRAFANA_TOKEN}" | jq -r ".[] | select(.title==\"${TENANT_NAME} Dashboards\") | .uid")

# 4. Grant permissions
curl -X POST http://grafana/api/folders/${FOLDER_UID}/permissions \
  -H "Authorization: Bearer ${GRAFANA_TOKEN}" \
  -d "{\"items\":[{\"teamId\":${TEAM_ID},\"permission\":2}]}"  # 2=Edit

# 5. Créer dashboard depuis template
DASHBOARD_JSON=$(cat dashboard-template.json | \
  sed "s/TENANT_ID/${TENANT_ID}/g" | \
  sed "s/TENANT_NAME/${TENANT_NAME}/g")

curl -X POST http://grafana/api/dashboards/db \
  -H "Authorization: Bearer ${GRAFANA_TOKEN}" \
  -d "{\"dashboard\":${DASHBOARD_JSON},\"folderId\":${FOLDER_UID}}"

# 6. Configurer quotas selon tier
case $TIER in
  free)
    LOG_QUOTA="1GB/day"
    METRIC_QUOTA="1000 series"
    TRACE_QUOTA="10000 spans/day"
    ;;
  standard)
    LOG_QUOTA="10GB/day"
    METRIC_QUOTA="10000 series"
    TRACE_QUOTA="100000 spans/day"
    ;;
  premium)
    LOG_QUOTA="unlimited"
    METRIC_QUOTA="unlimited"
    TRACE_QUOTA="unlimited"
    ;;
esac

# 7. Créer ConfigMap avec quotas
kubectl create configmap tenant-${TENANT_ID}-config \
  --from-literal=log_quota=${LOG_QUOTA} \
  --from-literal=metric_quota=${METRIC_QUOTA} \
  --from-literal=trace_quota=${TRACE_QUOTA} \
  -n tenant-${TENANT_ID}

# 8. Configurer alertes spécifiques au tenant
cat <<EOF | kubectl apply -f -
apiVersion: v1
kind: ConfigMap
metadata:
  name: tenant-${TENANT_ID}-alerts
  namespace: monitoring
data:
  alerts.yml: |
    groups:
      - name: ${TENANT_ID}_alerts
        rules:
          - alert: TenantServiceDown
            expr: up{tenant="${TENANT_ID}"} == 0
            for: 2m
            labels:
              severity: critical
              tenant: ${TENANT_ID}
            annotations:
              summary: "Service down for tenant ${TENANT_NAME}"
              tenant_email: "ops@${TENANT_ID}.com"
EOF

echo "✅ Tenant ${TENANT_NAME} onboarded successfully"
echo "📊 Dashboard: http://grafana/d/tenant-${TENANT_ID}"
echo "📧 Alerts will be sent to: ops@${TENANT_ID}.com"
```

**6. Gestion des Secrets Multi-Tenant**

**Vault integration pour credentials séparés** :
```yaml
# vault-config.yml
# Chaque tenant a son propre path dans Vault

path "secret/data/tenants/tenant-a/*" {
  capabilities = ["read", "list"]
}

path "secret/data/tenants/tenant-b/*" {
  capabilities = ["read", "list"]
}
```

**Application configuration** :
```java
@Configuration
public class MultiTenantDataSourceConfig {
    
    @Autowired
    private VaultTemplate vaultTemplate;
    
    @Bean
    public DataSource dataSource() {
        return new TenantRoutingDataSource();
    }
    
    public class TenantRoutingDataSource extends AbstractRoutingDataSource {
        @Override
        protected Object determineCurrentLookupKey() {
            String tenantId = TenantContext.getCurrentTenant();
            
            // Récupérer credentials depuis Vault
            VaultResponse response = vaultTemplate.read(
                "secret/data/tenants/" + tenantId + "/database"
            );
            
            // Configure datasource avec credentials du tenant
            Map<String, Object> data = response.getData();
            HikariConfig config = new HikariConfig();
            config.setJdbcUrl((String) data.get("url"));
            config.setUsername((String) data.get("username"));
            config.setPassword((String) data.get("password"));
            
            return new HikariDataSource(config);
        }
    }
}
```

**7. Monitoring de l'Observability Stack elle-même**

**Meta-monitoring** :
```yaml
# prometheus-meta.yml
# Surveiller la santé de la stack d'observability

groups:
  - name: observability_health
    interval: 1m
    rules:
      # Loki health
      - alert: LokiDown
        expr: up{job="loki"} == 0
        for: 5m
        labels:
          severity: critical
        annotations:
          summary: "Loki is down - logs ingestion stopped"
          
      - alert: LokiHighIngestionLatency
        expr: histogram_quantile(0.99, rate(loki_request_duration_seconds_bucket{route="push"}[5m])) > 1
        for: 10m
        labels:
          severity: warning
        annotations:
          summary: "Loki ingestion latency is high"
          
      # Prometheus health
      - alert: PrometheusTargetsDown
        expr: up == 0
        for: 5m
        labels:
          severity: warning
        annotations:
          summary: "Prometheus target {{ $labels.instance }} is down"
          
      - alert: PrometheusTSDBCompactionsFailing
        expr: rate(prometheus_tsdb_compactions_failed_total[1h]) > 0
        labels:
          severity: critical
          
      # Tempo health
      - alert: TempoIngestionErrors
        expr: rate(tempo_ingester_traces_created_total{status="error"}[5m]) > 0
        for: 5m
        labels:
          severity: warning
          
      # Grafana health
      - alert: GrafanaDown
        expr: up{job="grafana"} == 0
        for: 5m
        labels:
          severity: critical
          
      # Alertmanager health
      - alert: AlertmanagerFailedNotifications
        expr: rate(alertmanager_notifications_failed_total[5m]) > 0
        for: 10m
        labels:
          severity: warning
```

**Dashboard "Observability Platform Health"** :
```json
{
  "dashboard": {
    "title": "Observability Platform Health",
    "panels": [
      {
        "title": "Component Status",
        "type": "stat",
        "targets": [
          {"expr": "up{job=\"prometheus\"}", "legendFormat": "Prometheus"},
          {"expr": "up{job=\"loki\"}", "legendFormat": "Loki"},
          {"expr": "up{job=\"tempo\"}", "legendFormat": "Tempo"},
          {"expr": "up{job=\"grafana\"}", "legendFormat": "Grafana"}
        ]
      },
      {
        "title": "Data Ingestion Rate",
        "targets": [
          {"expr": "rate(prometheus_tsdb_head_samples_appended_total[5m])", "legendFormat": "Metrics/s"},
          {"expr": "rate(loki_ingester_chunks_created_total[5m])", "legendFormat": "Log chunks/s"},
          {"expr": "rate(tempo_ingester_traces_created_total[5m])", "legendFormat": "Traces/s"}
        ]
      },
      {
        "title": "Storage Usage",
        "targets": [
          {"expr": "prometheus_tsdb_storage_blocks_bytes / 1024 / 1024 / 1024", "legendFormat": "Prometheus (GB)"},
          {"expr": "loki_ingester_memory_chunks / 1024 / 1024 / 1024", "legendFormat": "Loki (GB)"},
          {"expr": "tempo_ingester_bytes_received_total / 1024 / 1024 / 1024", "legendFormat": "Tempo (GB)"}
        ]
      },
      {
        "title": "Query Performance",
        "targets": [
          {"expr": "histogram_quantile(0.99, rate(prometheus_engine_query_duration_seconds_bucket[5m]))", "legendFormat": "Prometheus P99"},
          {"expr": "histogram_quantile(0.99, rate(loki_request_duration_seconds_bucket[5m]))", "legendFormat": "Loki P99"},
          {"expr": "histogram_quantile(0.99, rate(tempo_request_duration_seconds_bucket[5m]))", "legendFormat": "Tempo P99"}
        ]
      }
    ]
  }
}
```

---

### Q20: Donnez un exemple d'incident complexe que vous avez résolu grâce à l'observability et expliquez votre démarche

**R:** Voici un **cas réel complexe** que j'ai résolu :

**Contexte** :
- Application e-commerce avec architecture microservices
- 15 microservices Spring Boot
- 50,000 utilisateurs actifs
- Black Friday période critique

**Incident** : "Mystery Performance Degradation"

**Symptômes initiaux** :
- ⚠️ Utilisateurs rapportent lenteur intermittente (pas systématique)
- ⚠️ Certaines commandes prennent 10-15 secondes au lieu de 2-3 secondes
- ⚠️ Pas d'augmentation du taux d'erreur visible
- ⚠️ Problème aléatoire : 1 commande sur 20 affectée

**Phase 1 : Détection (Minute 0-5)**

**Alert reçue** :
```
⚠️ WARNING: High P99 Latency
Service: order-service
P99 Latency: 12.5s (normal: 2.1s)
Affected: ~5% of requests
Time: 09:45 AM
```

**Dashboard Grafana initial** :
```promql
# P99 latency montre des spikes
histogram_quantile(0.99, rate(http_server_requests_seconds_bucket{application="order-service"}[5m]))

# Résultat : Pattern en dents de scie
# 09:30 - 2.1s ✅
# 09:35 - 11.2s 🔴
# 09:40 - 2.3s ✅
# 09:45 - 13.1s 🔴
```

**Observation clé** : Pattern cyclique toutes les 5-10 minutes

**Phase 2 : Investigation initiale (Minute 5-15)**

**1. Check standard ne révèle rien d'anormal** :
```bash
# CPU, Memory, Network normaux
kubectl top pods -n production
# Résultat : Toutes les ressources dans les limites normales

# Aucun déploiement récent
kubectl rollout history deployment/order-service
# Last deployment: 3 days ago

# Pas de problèmes externes évidents
curl https://status.stripe.com/api/v2/status.json
# Résultat : All systems operational
```

**2. Analyse des logs Loki** :
```logql
{application="order-service", level="ERROR"} | json

# Résultat : Quelques lignes intéressantes
[09:35:12] ERROR - HikariPool connection timeout after 10000ms
[09:35:14] ERROR - HikariPool connection timeout after 10000ms
[09:44:23] ERROR - HikariPool connection timeout after 10000ms
```

**Première piste** : Connection pool database exhausted périodiquement

**Phase 3 : Deep Dive avec Traces (Minute 15-25)**

**1. Récupérer un traceId d'une requête lente** :
```logql
{application="order-service"} 
| json 
| duration > 10000
| line_format "{{.traceId}} {{.duration}}ms"
| limit 1

# Résultat : traceId = xyz789abc
```

**2. Visualiser la trace dans Tempo** :
```
POST /api/orders (12.8s total) 🔴
│
├─ OrderController.createOrder (12.75s)
│  │
│  ├─ OrderService.validateOrder (0.05s) ✅
│  │
│  ├─ InventoryService.checkStock (10.2s) 🔴🔴🔴
│  │  │
│  │  ├─ HTTP GET inventory-service/api/check (10.15s)
│  │  │  │
│  │  │  ├─ Wait for connection (10.0s) 🔴 BOTTLENECK
│  │  │  └─ Actual request (0.15s) ✅
│  │  │
│  │  └─ Response parsing (0.02s)
│  │
│  ├─ PaymentService.charge (2.3s) ⚠️
│  │
│  └─ NotificationService.send (0.15s) ✅
```

**Diagnostic** : 
- 10 secondes passées à **attendre une connexion** vers inventory-service
- Pas un problème de performance d'inventory-service lui-même
- Problème de connection pool ou circuit breaker

**3. Investiguer inventory-service** :

**Métriques Actuator** :
```bash
curl http://inventory-service:8080/actuator/metrics/hikaricp.connections.active

# Résultat pendant période lente:
{
  "name": "hikaricp.connections.active",
  "measurements": [{
    "value": 20.0  # Maximum atteint!
  }]
}

curl http://inventory-service:8080/actuator/metrics/hikaricp.connections.max
# Max pool size: 20
```

**Pool database complètement saturé !**

**4. Investiguer les queries DB** :

**Logs inventory-service** :
```logql
{application="inventory-service"} 
| json 
| line_format "{{.sql_query}} {{.duration}}ms"
| duration > 1000

# Patterns répétitifs toutes les 5 minutes :
[09:35:00] SELECT * FROM inventory_sync_log WHERE status='PENDING' - 8500ms 🔴
[09:40:00] SELECT * FROM inventory_sync_log WHERE status='PENDING' - 9200ms 🔴
[09:45:00] SELECT * FROM inventory_sync_log WHERE status='PENDING' - 8800ms 🔴
```

**ROOT CAUSE identifiée** : 
- Job de synchronisation schedulé toutes les 5 minutes
- Query non optimisée qui lock la table pendant 8-10 secondes
- Pendant ce temps, toutes les connexions du pool sont bloquées
- Nouvelles requêtes API attendent jusqu'à épuisement du pool

**Phase 4 : Confirmation avec correlation complète (Minute 25-30)**

**Overlay des trois signaux** :

```
Timeline:
09:30:00 - Sync job starts
         ├─ Metrics: hikaricp.connections.active → 20/20 🔴
         ├─ Logs: "SELECT inventory_sync_log" - 8500ms
         └─ Traces: Multiple requests waiting 10s for connection
         
09:30:10 - Sync job completes
         ├─ Metrics: hikaricp.connections.active → 5/20 ✅
         └─ Latency returns to normal
         
09:35:00 - Sync job starts again (pattern repeats)
```

**Grafana dashboard combiné** :
```promql
# Panel 1: Latency P99
histogram_quantile(0.99, rate(http_server_requests_seconds_bucket[1m]))

# Panel 2: DB connection pool usage
hikaricp_connections_active / hikaricp_connections_max * 100

# Panel 3: Sync job executions
rate(inventory_sync_job_executions_total[1m])

# Correlation parfaite visible !
```

**Phase 5 : Mitigation immédiate (Minute 30-45)**

**Solution temporaire (hotfix)** :
```java
// Avant (problématique)
@Scheduled(fixedRate = 300000)  // Toutes les 5 minutes
public void syncInventory() {
    List<InventoryLog> pending = inventoryLogRepository
        .findByStatus("PENDING");  // Full table scan, pas d'index!
    // Process...
}

// Après (hotfix immédiat)
@Scheduled(fixedRate = 300000)
@Transactional(timeout = 5)  // Timeout pour éviter long locks
public void syncInventory() {
    // Pagination pour éviter gros resultsets
    Pageable pageable = PageRequest.of(0, 100);
    Page<InventoryLog> pending = inventoryLogRepository
        .findByStatusWithLimit("PENDING", pageable);
    // Process...
}
```

**Configuration temporaire augmentation pool** :
```yaml
# application.yml hotfix
spring:
  datasource:
    hikari:
      maximum-pool-size: 40  # Augmenté de 20 → 40
      connection-timeout: 5000  # Réduit de 10s → 5s pour fail-fast
```

**Déploiement** :
```bash
# Build et deploy hotfix
./mvnw clean package -DskipTests
kubectl set image deployment/inventory-service \
  inventory-service=inventory-service:v1.5.2-hotfix

# Monitor le rollout
kubectl rollout status deployment/inventory-service
```

**Résultat immédiat** :
- P99 latency : 12.5s → 2.3s ✅
- Connection pool jamais saturé
- Incident mitigé en 45 minutes

**Phase 6 : Solution long-terme (Post-incident)**

**1. Optimisation query DB** :
```sql
-- Avant : Full table scan
SELECT * FROM inventory_sync_log WHERE status = 'PENDING';
-- Execution time: 8500ms
-- Rows scanned: 5,000,000

-- Créer index
CREATE INDEX idx_inventory_sync_log_status_created 
ON inventory_sync_log(status, created_at);

-- Après : Index scan
SELECT * FROM inventory_sync_log 
WHERE status = 'PENDING' 
ORDER BY created_at 
LIMIT 100;
-- Execution time: 45ms ✅
-- Rows scanned: 100
```

**2. Déplacer le job vers worker dédié** :
```yaml
# Nouvelle architecture
inventory-service:
  - Handles API requests only
  - Pool size: 20 (sufficient)

inventory-worker:
  - Runs sync jobs
  - Dedicated database connection pool
  - No impact on API performance
```

**3. Améliorer l'observability du sync job** :
```java
@Component
public class InventorySyncJob {
    private final MeterRegistry registry;
    private final Timer syncTimer;
    private final Counter syncRecordsCounter;
    
    public InventorySyncJob(MeterRegistry registry) {
        this.syncTimer = Timer.builder("inventory.sync.duration")
            .description("Time to complete inventory sync")
            .register(registry);
            
        this.syncRecordsCounter = Counter.builder("inventory.sync.records")
            .description("Number of records processed")
            .register(registry);
    }
    
    @Scheduled(fixedRate = 300000)
    public void syncInventory() {
        Span span = tracer.nextSpan().name("inventory-sync-job").start();
        
        try (Tracer.SpanInScope ws = tracer.withSpanInScope(span)) {
            syncTimer.record(() -> {
                log.info("Starting inventory sync job");
                
                int processed = 0;
                Pageable pageable = PageRequest.of(0, 100);
                Page<InventoryLog> page;
                
                do {
                    page = inventoryLogRepository.findByStatus("PENDING", pageable);
                    processed += page.getContent().size();
                    
                    // Process batch...
                    
                    span.event("batch-processed", 
                        Tags.of("size", String.valueOf(page.getContent().size())));
                    
                    pageable = page.nextPageable();
                } while (page.hasNext());
                
                syncRecordsCounter.increment(processed);
                span.tag("records.processed", String.valueOf(processed));
                
                log.info("Inventory sync completed, processed {} records", processed);
            });
        } catch (Exception e) {
            span.error(e);
            log.error("Inventory sync failed", e);
            throw e;
        } finally {
            span.end();
        }
    }
}
```

**4. Ajouter alertes proactives** :
```yaml
- alert: DatabaseConnectionPoolHighUsage
  expr: |
    (hikaricp_connections_active / hikaricp_connections_max) > 0.8
  for: 2m
  labels:
    severity: warning
  annotations:
    summary: "DB connection pool usage high on {{ $labels.application }}"
    description: "Usage: {{ $value | humanizePercentage }}"
    
- alert: SlowDatabaseQuery
  expr: |
    histogram_quantile(0.95, 
      rate(hikaricp_connections_usage_seconds_bucket[5m])
    ) > 1
  for: 5m
  labels:
    severity: warning
  annotations:
    summary: "Slow database queries detected"
    
- alert: InventorySyncJobSlow
  expr: |
    rate(inventory_sync_duration_seconds_sum[5m]) / 
    rate(inventory_sync_duration_seconds_count[5m]) > 10
  for: 1m
  labels:
    severity: warning
```

**Phase 7 : Post-Mortem et Learnings**

**Post-Mortem Document** :
```markdown
# Post-Mortem: Mystery Performance Degradation

## Incident Summary
**Date**: Black Friday - 09:45 AM
**Duration**: 45 minutes (detection to mitigation)
**Severity**: Major (5% requests affected, high latency)
**Services Affected**: order-service, inventory-service

## Timeline
- **09:30**: Incident begins (unnoticed)
- **09:45**: P99 latency alert triggers
- **09:50**: Investigation begins
- **10:00**: Database connection pool exhaustion identified
- **10:10**: Root cause confirmed (slow sync job)
- **10:30**: Hotfix deployed
- **10:35**: Metrics return to normal
- **11:00**: Monitoring confirmed stable

## Root Cause
Scheduled inventory sync job (every 5 min) executing slow, unoptimized query:
- No index on `status` column
- Full table scan on 5M rows
- Query takes 8-10 seconds
- Exhausts entire DB connection pool
- API requests queue waiting for connections
- After 10s timeout, users see slow responses

## Impact
- **Users affected**: ~2,500 (5% of traffic during 45min)
- **Orders delayed**: ~180 orders
- **Revenue impact**: Minimal (orders eventually completed)
- **Reputation**: Some negative feedback on social media

## What Went Well ✅
1. **Observability stack performed excellently**:
   - Alerting detected issue within 15 minutes
   - Distributed tracing pinpointed exact bottleneck
   - Correlation logs-metrics-traces led to root cause quickly
   
2. **Team response**:
   - MTTD (Mean Time To Detect): 15 minutes
   - MTTK (Mean Time To Know): 25 minutes
   - MTTR (Mean Time To Resolve): 45 minutes
   
3. **Hotfix deployment**: Clean rollout without incidents

## What Went Wrong ❌
1. **No index on frequently queried column**
   - Database was not optimized for this query pattern
   
2. **No monitoring on background jobs**
   - Sync job had no metrics, traces, or visibility
   
3. **Connection pool sizing not validated under load**
   - Load testing didn't include background jobs
   
4. **No dedicated worker for batch jobs**
   - Background jobs competing with API for resources

## Action Items
- [x] Add database index (Completed same day)
- [x] Deploy hotfix with pagination (Completed same day)
- [ ] Create dedicated worker service for sync jobs (Priority: HIGH, ETA: 1 week)
- [ ] Add comprehensive monitoring to all scheduled jobs (Priority: HIGH, ETA: 3 days)
- [ ] Load testing scenarios including background jobs (Priority: MEDIUM, ETA: 2 weeks)
- [ ] Database query performance audit (Priority: MEDIUM, ETA: 1 week)
- [ ] Runbook for connection pool exhaustion (Priority: LOW, ETA: 1 week)

## Lessons Learned
1. **Observability is critical**: Without logs-metrics-traces correlation, this would have taken hours to debug
2. **Background jobs need monitoring too**: Not just APIs
3. **Load testing must be realistic**: Include all workloads
4. **Database optimization matters**: Indexes are not optional
5. **Resource isolation**: Consider dedicated pools/workers for different workload types

## Long-term Improvements
- Implement APM for automatic slow query detection
- Set up synthetic monitoring for critical user journeys
- Create "observability checklist" for all new features
- Regular performance review meetings

## Cost of Incident
- **Engineering time**: 8 hours (investigation + hotfix + post-mortem)
- **Revenue loss**: ~$0 (orders eventually completed)
- **Customer trust**: Minor impact
- **Total estimated cost**: $2,000 (eng time)

## Prevented by Observability
Without proper observability stack:
- Incident would likely have lasted 4-6 hours
- Much harder to identify root cause
- Potential revenue loss: $50,000+
- **ROI of observability**: 25x in this incident alone
```

**Conclusion de ce cas** :
L'observability complète (Logs + Metrics + Traces + Alerting) a permis :
- ✅ Détection rapide (15 min au lieu de potentiellement heures)
- ✅ Diagnostic précis avec correlation tri-dimensionnelle
- ✅ Resolution rapide (45 min total)
- ✅ Learning pour prévenir futurs incidents similaires

**Sans observability**, cet incident aurait probablement :
- ❌ Duré 4-6 heures (debugging aveugle)
- ❌ Nécessité des restarts aléatoires (trial & error)
- ❌ Impacté beaucoup plus d'utilisateurs
- ❌ Causé perte de revenue significative

---

**Voilà ! Ce résumé complet couvre tous les aspects de l'observability pour un poste développeur Spring Boot. Ces questions/réponses démontrent une expertise approfondie et pratique des trois piliers (logs, metrics, traces) ainsi que des compétences en troubleshooting, architecture, et bonnes pratiques.**

**Points clés à retenir pour l'entretien** :
- ✅ Maîtrise technique des deux écosystèmes (ELK & Grafana)
- ✅ Expérience pratique avec cas réels
- ✅ Comprendre le "pourquoi" pas seulement le "comment"
- ✅ Focus sur business value et ROI
- ✅ Approche méthodique du debugging