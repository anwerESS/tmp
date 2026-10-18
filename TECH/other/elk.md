# Guide Complet ELK, RabbitMQ, Swagger pour Entretien

---

# PARTIE 1 : ELK STACK + FILEBEAT + ZIPKIN

## 1. ELK Stack - Vue d'ensemble

### Qu'est-ce que ELK?

**ELK** = **E**lasticsearch + **L**ogstash + **K**ibana

C'est une suite d'outils open-source pour:
- **Collecter** les logs (Logstash/Filebeat)
- **Stocker** et indexer les logs (Elasticsearch)
- **Visualiser** et analyser les logs (Kibana)

### Architecture ELK

```
┌─────────────┐
│ Application │──┐
└─────────────┘  │
                 │ Logs
┌─────────────┐  │
│ Serveur Web │──┤
└─────────────┘  │
                 │
┌─────────────┐  │
│ Base données│──┤
└─────────────┘  │
                 ▼
         ┌──────────────┐
         │   Filebeat   │ ← Collecte logs (léger)
         └──────┬───────┘
                │
                ▼
         ┌──────────────┐
         │   Logstash   │ ← Transforme logs
         └──────┬───────┘
                │
                ▼
         ┌──────────────┐
         │Elasticsearch │ ← Stocke/Indexe
         └──────┬───────┘
                │
                ▼
         ┌──────────────┐
         │   Kibana     │ ← Visualise
         └──────────────┘
```

---

## 2. Elasticsearch

### Qu'est-ce que c'est?

**Elasticsearch** est un moteur de recherche et d'analyse distribué basé sur Apache Lucene.

### Concepts clés

| Concept | Description | Équivalent SQL |
|---------|-------------|----------------|
| **Index** | Collection de documents | Base de données |
| **Document** | Unité de base (JSON) | Ligne (row) |
| **Field** | Propriété d'un document | Colonne |
| **Mapping** | Schéma des champs | Schéma de table |
| **Shard** | Fragment d'un index | Partition |
| **Replica** | Copie d'un shard | Réplication |

### Architecture Elasticsearch

```
Cluster Elasticsearch
├── Node 1 (Master)
│   ├── Index "logs-2024"
│   │   ├── Shard 0 (Primary)
│   │   └── Shard 1 (Primary)
│
├── Node 2 (Data)
│   ├── Shard 0 (Replica)
│   └── Shard 2 (Primary)
│
└── Node 3 (Data)
    ├── Shard 1 (Replica)
    └── Shard 2 (Replica)
```

### Opérations de base

```bash
# Créer un index
PUT /logs-2024
{
  "settings": {
    "number_of_shards": 3,
    "number_of_replicas": 1
  }
}

# Indexer un document
POST /logs-2024/_doc
{
  "timestamp": "2024-10-26T10:30:00",
  "level": "ERROR",
  "service": "user-service",
  "message": "Failed to connect to database",
  "user_id": 12345
}

# Rechercher
GET /logs-2024/_search
{
  "query": {
    "match": {
      "level": "ERROR"
    }
  }
}

# Recherche avec filtre
GET /logs-2024/_search
{
  "query": {
    "bool": {
      "must": [
        { "match": { "service": "user-service" }},
        { "range": { "timestamp": { "gte": "2024-10-26" }}}
      ]
    }
  }
}

# Agrégation (compter par niveau)
GET /logs-2024/_search
{
  "size": 0,
  "aggs": {
    "by_level": {
      "terms": {
        "field": "level.keyword"
      }
    }
  }
}
```

### Avantages Elasticsearch

✅ **Recherche ultra-rapide** (full-text search)
✅ **Scalable horizontalement** (ajouter des nodes)
✅ **Haute disponibilité** (réplication)
✅ **Analyse en temps réel**
✅ **RESTful API**

---

## 3. Logstash

### Qu'est-ce que c'est?

**Logstash** est un pipeline de traitement de données qui:
1. **Collecte** les données de multiples sources
2. **Transforme** les données (parsing, enrichissement)
3. **Envoie** les données vers Elasticsearch

### Architecture Logstash

```
Input → Filter → Output

┌────────────┐    ┌──────────┐    ┌─────────────┐
│  Sources   │───▶│ Logstash │───▶│Elasticsearch│
└────────────┘    └──────────┘    └─────────────┘
   - Files           - Parse          - Index
   - Beats           - Enrich
   - Syslog          - Transform
   - TCP/UDP
```

### Configuration Logstash

```ruby
# logstash.conf

# INPUT - Sources de données
input {
  # Recevoir de Filebeat
  beats {
    port => 5044
  }
  
  # Lire des fichiers directement
  file {
    path => "/var/log/app/*.log"
    start_position => "beginning"
  }
  
  # TCP
  tcp {
    port => 5000
    codec => json
  }
}

# FILTER - Transformation
filter {
  # Parser les logs JSON
  json {
    source => "message"
  }
  
  # Parser les logs avec pattern
  grok {
    match => { 
      "message" => "%{TIMESTAMP_ISO8601:timestamp} %{LOGLEVEL:level} %{GREEDYDATA:msg}" 
    }
  }
  
  # Convertir le timestamp
  date {
    match => [ "timestamp", "ISO8601" ]
    target => "@timestamp"
  }
  
  # Ajouter des champs
  mutate {
    add_field => { "environment" => "production" }
    remove_field => [ "temp_field" ]
  }
  
  # Parser user agent (navigateurs)
  useragent {
    source => "user_agent"
    target => "ua"
  }
  
  # GeoIP (localisation)
  geoip {
    source => "client_ip"
  }
}

# OUTPUT - Destination
output {
  # Envoyer vers Elasticsearch
  elasticsearch {
    hosts => ["localhost:9200"]
    index => "logs-%{+YYYY.MM.dd}"
  }
  
  # Afficher dans console (debug)
  stdout {
    codec => rubydebug
  }
  
  # Envoyer vers fichier
  file {
    path => "/var/log/logstash/output.log"
  }
}
```

### Grok Patterns (Parsing)

```ruby
# Log Apache
%{COMBINEDAPACHELOG}

# Log Java
%{TIMESTAMP_ISO8601:timestamp} \[%{LOGLEVEL:level}\] %{JAVACLASS:class} - %{GREEDYDATA:message}

# Log personnalisé
%{IP:client_ip} - - \[%{HTTPDATE:timestamp}\] "%{WORD:method} %{URIPATHPARAM:request}"
```

---

## 4. Kibana

### Qu'est-ce que c'est?

**Kibana** est l'interface de visualisation pour Elasticsearch.

### Fonctionnalités principales

1. **Discover** - Explorer les logs
2. **Visualize** - Créer des graphiques
3. **Dashboard** - Tableaux de bord
4. **Canvas** - Présentations
5. **Machine Learning** - Détection d'anomalies
6. **Alerts** - Notifications

### Types de visualisations

| Type | Usage |
|------|-------|
| **Line Chart** | Tendances dans le temps |
| **Bar Chart** | Comparaisons |
| **Pie Chart** | Proportions |
| **Data Table** | Données tabulaires |
| **Metric** | KPI (nombre unique) |
| **Heat Map** | Intensité sur grille |
| **Tag Cloud** | Mots fréquents |
| **Map** | Données géographiques |

### Exemple Dashboard Kibana

```
┌─────────────────────────────────────────────────┐
│ Dashboard Production - 26/10/2024               │
├─────────────────────────────────────────────────┤
│                                                 │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
│  │Total Logs│  │  Errors  │  │ Warnings │     │
│  │ 125,430  │  │   342    │  │  1,254   │     │
│  └──────────┘  └──────────┘  └──────────┘     │
│                                                 │
│  ┌────────────────────────────────────────┐    │
│  │ Logs over time (Line Chart)            │    │
│  │        ╱╲      ╱╲                       │    │
│  │       ╱  ╲    ╱  ╲    ╱╲                │    │
│  │  ────╯    ╲──╯    ╲──╯  ╲───            │    │
│  └────────────────────────────────────────┘    │
│                                                 │
│  ┌──────────────────┐  ┌──────────────────┐   │
│  │ Top Services     │  │ Error Types      │   │
│  │ user-service: 45%│  │ Database: 60%    │   │
│  │ order-svc: 30%   │  │ Network: 25%     │   │
│  │ payment-svc: 25% │  │ Timeout: 15%     │   │
│  └──────────────────┘  └──────────────────┘   │
└─────────────────────────────────────────────────┘
```

### Query DSL dans Kibana

```
# Recherche simple
level: ERROR

# Recherche avec wildcard
message: *database*

# Recherche avec opérateurs
level: ERROR AND service: user-service

# Range
timestamp: [2024-10-26 TO 2024-10-27]

# Existe
_exists_: user_id

# Recherche complexe
(level: ERROR OR level: FATAL) AND NOT service: test-service
```

---

## 5. Filebeat

### Qu'est-ce que c'est?

**Filebeat** est un agent léger qui:
- Lit les fichiers de logs
- Envoie les données vers Logstash ou Elasticsearch
- **Plus léger que Logstash** (consomme moins de ressources)

### Pourquoi Filebeat plutôt que Logstash?

| Aspect | Filebeat | Logstash |
|--------|----------|----------|
| **Poids** | Très léger (~10MB) | Lourd (~200MB) |
| **CPU/RAM** | Faible | Élevé |
| **Déploiement** | Sur chaque serveur | Centralisé |
| **Transformation** | Basique | Avancée |
| **Usage** | Collecter et envoyer | Parser et transformer |

### Architecture avec Filebeat

```
Serveur 1                    Serveur Central
┌─────────────┐
│ Application │              ┌──────────────┐
│   + Logs    │              │   Logstash   │
└──────┬──────┘              │  (Transform) │
       │                     └──────┬───────┘
   ┌───▼────┐                       │
   │Filebeat├───────────────────────┤
   └────────┘                       │
                                    ▼
Serveur 2                    ┌─────────────┐
┌─────────────┐              │Elasticsearch│
│ Application │              │   (Store)   │
│   + Logs    │              └──────┬──────┘
└──────┬──────┘                     │
       │                            ▼
   ┌───▼────┐              ┌──────────────┐
   │Filebeat├──────────────│   Kibana     │
   └────────┘              │  (Visualize) │
                           └──────────────┘
```

### Configuration Filebeat

```yaml
# filebeat.yml

# INPUT - Fichiers à surveiller
filebeat.inputs:
  - type: log
    enabled: true
    paths:
      - /var/log/app/*.log
      - /var/log/app/*/*.log
    
    # Champs additionnels
    fields:
      service: user-service
      environment: production
    
    # Multiline (stack traces Java)
    multiline.pattern: '^[[:space:]]'
    multiline.negate: false
    multiline.match: after
  
  - type: log
    enabled: true
    paths:
      - /var/log/nginx/access.log
    fields:
      log_type: nginx

# OUTPUT - Vers Logstash
output.logstash:
  hosts: ["logstash:5044"]
  
# OU OUTPUT - Direct vers Elasticsearch
# output.elasticsearch:
#   hosts: ["elasticsearch:9200"]
#   index: "filebeat-%{+yyyy.MM.dd}"

# Monitoring
monitoring.enabled: true

# Processeurs (transformations légères)
processors:
  - add_host_metadata: ~
  - add_cloud_metadata: ~
  - add_docker_metadata: ~
```

### Modules Filebeat

Filebeat inclut des modules préconfigurés:

```bash
# Lister les modules disponibles
filebeat modules list

# Activer un module
filebeat modules enable nginx
filebeat modules enable apache
filebeat modules enable mysql

# Configuration du module Nginx
# modules.d/nginx.yml
- module: nginx
  access:
    enabled: true
    var.paths: ["/var/log/nginx/access.log"]
  error:
    enabled: true
    var.paths: ["/var/log/nginx/error.log"]
```

---

## 6. Zipkin

### Qu'est-ce que c'est?

**Zipkin** est un **distributed tracing system** pour:
- Suivre les requêtes à travers les microservices
- Identifier les goulots d'étranglement
- Débugger la latence

### Différence Logs vs Traces

| Aspect | Logs (ELK) | Traces (Zipkin) |
|--------|-----------|-----------------|
| **Focus** | Événements individuels | Flux de requêtes |
| **Scope** | Un service | Multi-services |
| **Usage** | Débugger erreurs | Analyser performance |
| **Données** | Messages texte | Spans avec timing |

### Concepts Zipkin

```
Trace (ID: abc123) - Requête complète
├── Span 1: API Gateway (100ms)
│   ├── Span 2: User Service (50ms)
│   │   └── Span 3: Database Query (30ms)
│   └── Span 4: Order Service (80ms)
│       ├── Span 5: Payment Service (40ms)
│       └── Span 6: Inventory Service (35ms)
```

**Trace** = Une requête complète
**Span** = Une opération dans un service

### Architecture Zipkin

```
┌───────────┐     ┌───────────┐     ┌───────────┐
│  Service  │────▶│  Service  │────▶│  Service  │
│     A     │     │     B     │     │     C     │
└─────┬─────┘     └─────┬─────┘     └─────┬─────┘
      │                 │                 │
      │ Envoie traces   │                 │
      └─────────────────┼─────────────────┘
                        ▼
                 ┌─────────────┐
                 │   Zipkin    │
                 │   Server    │
                 └──────┬──────┘
                        │
                        ▼
                 ┌─────────────┐
                 │  Cassandra  │
                 │     ou      │
                 │Elasticsearch│
                 └─────────────┘
```

### Configuration Zipkin avec Spring Boot

```xml
<!-- pom.xml -->
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-zipkin</artifactId>
</dependency>
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-sleuth-zipkin</artifactId>
</dependency>
```

```yaml
# application.yml
spring:
  application:
    name: user-service
  
  # Zipkin configuration
  zipkin:
    base-url: http://localhost:9411
  
  # Sleuth (trace automatique)
  sleuth:
    sampler:
      probability: 1.0  # 1.0 = 100% des traces (en prod: 0.1 = 10%)
```

### Exemple de Trace

```java
// Service A
@RestController
public class UserController {
    
    @Autowired
    private RestTemplate restTemplate;
    
    @Autowired
    private Tracer tracer; // Sleuth tracer
    
    @GetMapping("/user/{id}")
    public User getUser(@PathVariable Long id) {
        // Span automatique créé par Sleuth
        log.info("Fetching user {}", id);
        
        // Créer un span custom
        Span span = tracer.nextSpan().name("fetch-user-details");
        try (Tracer.SpanInScope ws = tracer.withSpan(span.start())) {
            // Appel vers Service B (trace propagé automatiquement)
            Orders orders = restTemplate.getForObject(
                "http://order-service/orders?userId=" + id, 
                Orders.class
            );
            
            span.tag("user.id", id.toString());
            span.tag("orders.count", String.valueOf(orders.size()));
            
            return new User(id, orders);
        } finally {
            span.end();
        }
    }
}
```

### Interface Zipkin

```
┌─────────────────────────────────────────────────┐
│ Zipkin UI - Trace: abc123                      │
├─────────────────────────────────────────────────┤
│                                                 │
│ Total Duration: 250ms                           │
│                                                 │
│ api-gateway          ████████████████ 100ms    │
│   user-service       ████████ 50ms              │
│     db-query         ████ 30ms                  │
│   order-service      ████████████ 80ms          │
│     payment-service  ██████ 40ms                │
│     inventory-svc    █████ 35ms                 │
│                                                 │
│ Timeline:                                       │
│ 0ms    50ms   100ms  150ms  200ms  250ms        │
│ |──────|──────|──────|──────|──────|            │
│ └──api-gateway──────────────────────┘           │
│   └─user-service──┘                             │
│     └db┘                                        │
│   └─order-service────────┘                      │
│     └payment┘                                   │
│     └inventory┘                                 │
└─────────────────────────────────────────────────┘
```

### ELK vs Zipkin - Quand utiliser quoi?

```
┌──────────────────────────────────────────────┐
│ Problème: "L'API est lente"                  │
├──────────────────────────────────────────────┤
│ ELK (Logs):                                  │
│ - Chercher les erreurs                       │
│ - Voir les exceptions                        │
│ - Analyser les messages                      │
│                                              │
│ Zipkin (Traces):                             │
│ - Identifier quel service est lent           │
│ - Mesurer le temps de chaque appel           │
│ - Visualiser le flux complet                 │
└──────────────────────────────────────────────┘

→ Utiliser les DEUX ensemble pour debug complet!
```

---

## 7. Docker Compose - Stack complète ELK + Zipkin

```yaml
version: '3.8'

services:
  # Elasticsearch
  elasticsearch:
    image: docker.elastic.co/elasticsearch/elasticsearch:8.10.0
    container_name: elasticsearch
    environment:
      - discovery.type=single-node
      - "ES_JAVA_OPTS=-Xms512m -Xmx512m"
      - xpack.security.enabled=false
    ports:
      - "9200:9200"
    volumes:
      - elasticsearch-data:/usr/share/elasticsearch/data
    networks:
      - elk

  # Logstash
  logstash:
    image: docker.elastic.co/logstash/logstash:8.10.0
    container_name: logstash
    ports:
      - "5044:5044"  # Beats input
      - "5000:5000"  # TCP input
    volumes:
      - ./logstash/pipeline:/usr/share/logstash/pipeline
      - ./logstash/config/logstash.yml:/usr/share/logstash/config/logstash.yml
    depends_on:
      - elasticsearch
    networks:
      - elk

  # Kibana
  kibana:
    image: docker.elastic.co/kibana/kibana:8.10.0
    container_name: kibana
    ports:
      - "5601:5601"
    environment:
      - ELASTICSEARCH_HOSTS=http://elasticsearch:9200
    depends_on:
      - elasticsearch
    networks:
      - elk

  # Filebeat
  filebeat:
    image: docker.elastic.co/beats/filebeat:8.10.0
    container_name: filebeat
    user: root
    volumes:
      - ./filebeat/filebeat.yml:/usr/share/filebeat/filebeat.yml
      - /var/log:/var/log:ro
      - /var/lib/docker/containers:/var/lib/docker/containers:ro
    depends_on:
      - logstash
    networks:
      - elk

  # Zipkin
  zipkin:
    image: openzipkin/zipkin:latest
    container_name: zipkin
    ports:
      - "9411:9411"
    networks:
      - elk

volumes:
  elasticsearch-data:
    driver: local

networks:
  elk:
    driver: bridge
```

---

## 8. Questions d'Entretien - ELK & Zipkin

### Q1: Qu'est-ce que ELK et pourquoi l'utiliser?

**Réponse:**
ELK = Elasticsearch + Logstash + Kibana. C'est une stack pour:
- **Centraliser** les logs de tous les services
- **Rechercher** rapidement dans des millions de logs
- **Visualiser** avec des dashboards
- **Alerter** en cas de problèmes

**Avantages:**
- Recherche full-text ultra-rapide
- Scalable (des milliers de serveurs)
- Open-source et flexible

### Q2: Différence entre Logstash et Filebeat?

**Réponse:**

| Aspect | Filebeat | Logstash |
|--------|----------|----------|
| Poids | Léger (~10MB) | Lourd (~200MB) |
| Déploiement | Sur chaque serveur | Centralisé |
| Fonction | Collecter | Transformer |
| CPU/RAM | Faible | Élevé |

**Pattern recommandé:** Filebeat (collecte) → Logstash (transformation) → Elasticsearch

### Q3: Comment Elasticsearch est-il distribué?

**Réponse:**
Elasticsearch utilise des **shards** (fragments) et **replicas**:
- Un index est divisé en plusieurs shards
- Chaque shard a des replicas (copies)
- Les shards sont distribués sur différents nodes
- **Haute disponibilité** si un node tombe
- **Performance** avec requêtes parallèles

### Q4: Qu'est-ce qu'un document dans Elasticsearch?

**Réponse:**
Un document est l'unité de base stockée en JSON:
```json
{
  "timestamp": "2024-10-26T10:30:00",
  "level": "ERROR",
  "service": "user-service",
  "message": "Connection refused"
}
```
Équivalent à une ligne dans une base SQL.

### Q5: Grok patterns - C'est quoi?

**Réponse:**
Grok est un langage de pattern matching pour parser les logs non structurés:

```ruby
# Log brut:
10.0.0.1 - - [26/Oct/2024:10:30:00] "GET /api/users HTTP/1.1" 200

# Pattern Grok:
%{IP:client_ip} - - \[%{HTTPDATE:timestamp}\] "%{WORD:method} %{URIPATHPARAM:request}" %{NUMBER:status}

# Résultat structuré:
{
  "client_ip": "10.0.0.1",
  "timestamp": "26/Oct/2024:10:30:00",
  "method": "GET",
  "request": "/api/users",
  "status": "200"
}
```

### Q6: Différence entre ELK et Zipkin?

**Réponse:**

| Aspect | ELK | Zipkin |
|--------|-----|--------|
| Type | Logs | Traces |
| Focus | Événements | Flux de requêtes |
| Usage | Débugger erreurs | Analyser latence |
| Scope | Un service | Multi-services |

**Complémentaires:** ELK pour *quoi s'est passé*, Zipkin pour *où est le goulot d'étranglement*.

### Q7: Qu'est-ce qu'un Span dans Zipkin?

**Réponse:**
Un **Span** représente une opération dans un service avec:
- Nom de l'opération
- Timestamp de début/fin
- Durée
- Tags (métadonnées)
- Service associé

Plusieurs spans forment une **Trace** (requête complète).

### Q8: Comment propager le contexte de trace entre services?

**Réponse:**
Via des **headers HTTP**:
- `X-B3-TraceId`: ID unique de la trace
- `X-B3-SpanId`: ID du span actuel
- `X-B3-ParentSpanId`: ID du span parent

Spring Cloud Sleuth le fait **automatiquement**!

### Q9: Kibana vs Grafana - Différence?

**Réponse:**

| Aspect | Kibana | Grafana |
|--------|--------|---------|
| Source | Elasticsearch uniquement | Multiples (Prometheus, MySQL, etc.) |
| Focus | Logs | Métriques |
| Intégration | ELK Stack | Monitoring (Prometheus) |

**En entreprise:** Souvent les deux! Kibana pour logs, Grafana pour métriques.

### Q10: Performance Elasticsearch - Comment optimiser?

**Réponse:**
1. **Augmenter les shards** pour distribuer la charge
2. **Index lifecycle management** (ILM) - supprimer vieux logs
3. **Mapping correct** - types de champs appropriés
4. **Filtrer avant aggregation** - réduire données traitées
5. **Cache** - activer query cache
6. **Hardware** - SSD, plus de RAM

---

# Questions/Réponses d'Entretien Technique - Guide Complet


# SECTION 1: ELK STACK & MONITORING

### Q1: Qu'est-ce que ELK et à quoi ça sert?

**Réponse:**
**ELK** = Elasticsearch + Logstash + Kibana

**Stack pour:**
- **Centraliser** les logs de tous les services
- **Rechercher** rapidement dans des millions de logs
- **Visualiser** avec des dashboards
- **Alerter** en cas de problèmes

**Exemple d'usage:**
```
Application 1 ──┐
Application 2 ──┤──▶ Filebeat ──▶ Logstash ──▶ Elasticsearch
Application 3 ──┘                               ▲
                                                │
                                             Kibana (visualisation)
```

### Q2: Différence entre Logstash et Filebeat?

**Réponse:**

| Aspect | Filebeat | Logstash |
|--------|----------|----------|
| Poids | Léger (~10MB) | Lourd (~200MB) |
| CPU/RAM | Faible | Élevé |
| Déploiement | Sur chaque serveur | Centralisé |
| Fonction | Collecter et envoyer | Parser et transformer |
| Transformation | Basique | Avancée (Grok, filters) |

**Pattern recommandé:**
```
Serveurs ──▶ Filebeat ──▶ Logstash ──▶ Elasticsearch
           (collecte)   (transform)    (stockage)
```

### Q3: Comment fonctionne Elasticsearch?

**Réponse:**
Elasticsearch est un moteur de recherche distribué:

**Concepts:**
- **Index**: Collection de documents (≈ base de données)
- **Document**: Unité de base en JSON (≈ ligne)
- **Shard**: Fragment d'un index (pour distribution)
- **Replica**: Copie d'un shard (pour haute disponibilité)

**Architecture:**
```
Cluster Elasticsearch
├── Node 1 (Master)
│   ├── Shard 0 (Primary)
│   └── Shard 1 (Primary)
├── Node 2
│   ├── Shard 0 (Replica)
│   └── Shard 2 (Primary)
└── Node 3
    ├── Shard 1 (Replica)
    └── Shard 2 (Replica)
```

### Q4: Qu'est-ce qu'un Grok pattern?

**Réponse:**
Grok est un langage pour parser des logs non structurés:

**Log brut:**
```
192.168.1.1 - - [26/Oct/2024:10:30:00] "GET /api/users HTTP/1.1" 200
```

**Pattern Grok:**
```ruby
%{IP:client_ip} - - \[%{HTTPDATE:timestamp}\] "%{WORD:method} %{URIPATHPARAM:request}" %{NUMBER:status}
```

**Résultat:**
```json
{
  "client_ip": "192.168.1.1",
  "timestamp": "26/Oct/2024:10:30:00",
  "method": "GET",
  "request": "/api/users",
  "status": "200"
}
```

### Q5: Différence entre ELK et Zipkin?

**Réponse:**

| Aspect | ELK | Zipkin |
|--------|-----|--------|
| Type | **Logs** (événements) | **Traces** (flux) |
| Focus | Quoi s'est passé | Où est le problème de latence |
| Scope | Un service | Multi-services |
| Usage | Débugger erreurs | Analyser performance |
| Données | Messages texte | Spans avec timing |

**Exemple:**

**ELK** - Chercher erreur:
```
ERROR 2024-10-26 10:30:00 [user-service] Database connection failed
```

**Zipkin** - Analyser latence:
```
Trace: abc123
├── API Gateway: 100ms
├── User Service: 50ms
│   └── Database: 30ms  ← Goulot!
└── Order Service: 80ms
```

**Complémentaires:** Utiliser les DEUX!

### Q6: Qu'est-ce qu'un Span dans Zipkin?

**Réponse:**
Un **Span** représente une opération dans un service:

```
Span {
  traceId: "abc123",           ← ID de la trace complète
  spanId: "span-1",            ← ID unique du span
  parentSpanId: "span-0",      ← Span parent
  name: "database-query",      ← Nom de l'opération
  timestamp: 1635235800000,    ← Début
  duration: 30,                ← Durée en ms
  tags: {
    "db.type": "mysql",
    "db.query": "SELECT * FROM users"
  }
}
```

Plusieurs spans = une **Trace** (requête complète)

### Q7: Comment configurer Zipkin avec Spring Boot?

**Réponse:**
```xml
<!-- pom.xml -->
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-zipkin</artifactId>
</dependency>
```

```yaml
# application.yml
spring:
  application:
    name: user-service
  zipkin:
    base-url: http://localhost:9411
  sleuth:
    sampler:
      probability: 1.0  # 100% traces (prod: 0.1 = 10%)
```

Spring Cloud Sleuth **propage automatiquement** le contexte de trace!

### Q8: Kibana vs Grafana?

**Réponse:**

| Aspect | Kibana | Grafana |
|--------|--------|---------|
| Source de données | Elasticsearch uniquement | Multiple (Prometheus, MySQL, etc.) |
| Focus | **Logs** | **Métriques** |
| Intégration | ELK Stack | Monitoring (Prometheus) |
| Visualisations | Avancées pour logs | Avancées pour time-series |

**En entreprise:** Souvent les DEUX!
- **Kibana** pour analyser les logs
- **Grafana** pour monitorer les métriques (CPU, RAM, requêtes/sec)

### Q9: Comment optimiser Elasticsearch?

**Réponse:**

**1. Sharding correct**
```json
PUT /logs-2024
{
  "settings": {
    "number_of_shards": 5,
    "number_of_replicas": 1
  }
}
```

**2. Index Lifecycle Management (ILM)** - Supprimer vieux logs
```
Hot (0-7 days) → Warm (7-30 days) → Cold (30-90 days) → Delete
```

**3. Mapping optimisé**
```json
{
  "mappings": {
    "properties": {
      "level": { "type": "keyword" },  // Pas "text" pour enum
      "timestamp": { "type": "date" },
      "message": { "type": "text" }
    }
  }
}
```

**4. Filtrer avant agréger**
```json
GET /logs/_search
{
  "query": { "range": { "timestamp": { "gte": "now-1h" }}},  ← Filtre
  "aggs": { "by_level": { "terms": { "field": "level" }}}
}
```

---