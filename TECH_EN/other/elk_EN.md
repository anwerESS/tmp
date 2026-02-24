# Complete ELK, RabbitMQ, Swagger Guide for Interviews

---

# PART 1: ELK STACK + FILEBEAT + ZIPKIN

## 1. ELK Stack - Overview

### What is ELK?

**ELK** = **E**lasticsearch + **L**ogstash + **K**ibana

It is a suite of open-source tools for:
- **Collecting** logs (Logstash/Filebeat)
- **Storing** and indexing logs (Elasticsearch)
- **Visualizing** and analyzing logs (Kibana)

### ELK Architecture

```
┌─────────────┐
│ Application │──┐
└─────────────┘  │
                 │ Logs
┌─────────────┐  │
│  Web Server │──┤
└─────────────┘  │
                 │
┌─────────────┐  │
│  Database   │──┤
└─────────────┘  │
                 ▼
         ┌──────────────┐
         │   Filebeat   │ ← Collects logs (lightweight)
         └──────┬───────┘
                │
                ▼
         ┌──────────────┐
         │   Logstash   │ ← Transforms logs
         └──────┬───────┘
                │
                ▼
         ┌──────────────┐
         │Elasticsearch │ ← Stores/Indexes
         └──────┬───────┘
                │
                ▼
         ┌──────────────┐
         │   Kibana     │ ← Visualizes
         └──────────────┘
```

---

## 2. Elasticsearch

### What is it?

**Elasticsearch** is a distributed search and analytics engine based on Apache Lucene.

### Key Concepts

| Concept | Description | SQL Equivalent |
|---------|-------------|----------------|
| **Index** | Collection of documents | Database |
| **Document** | Basic unit (JSON) | Row |
| **Field** | Document property | Column |
| **Mapping** | Field schema | Table schema |
| **Shard** | Fragment of an index | Partition |
| **Replica** | Copy of a shard | Replication |

### Elasticsearch Architecture

```
Elasticsearch Cluster
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

### Basic Operations

```bash
# Create an index
PUT /logs-2024
{
  "settings": {
    "number_of_shards": 3,
    "number_of_replicas": 1
  }
}

# Index a document
POST /logs-2024/_doc
{
  "timestamp": "2024-10-26T10:30:00",
  "level": "ERROR",
  "service": "user-service",
  "message": "Failed to connect to database",
  "user_id": 12345
}

# Search
GET /logs-2024/_search
{
  "query": {
    "match": {
      "level": "ERROR"
    }
  }
}

# Search with filter
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

# Aggregation (count by level)
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

### Elasticsearch Advantages

✅ **Ultra-fast search** (full-text search)
✅ **Horizontally scalable** (add nodes)
✅ **High availability** (replication)
✅ **Real-time analytics**
✅ **RESTful API**

---

## 3. Logstash

### What is it?

**Logstash** is a data processing pipeline that:
1. **Collects** data from multiple sources
2. **Transforms** data (parsing, enrichment)
3. **Sends** data to Elasticsearch

### Logstash Architecture

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

### Logstash Configuration

```ruby
# logstash.conf

# INPUT - Data sources
input {
  # Receive from Filebeat
  beats {
    port => 5044
  }
  
  # Read files directly
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
  # Parse JSON logs
  json {
    source => "message"
  }
  
  # Parse logs with pattern
  grok {
    match => { 
      "message" => "%{TIMESTAMP_ISO8601:timestamp} %{LOGLEVEL:level} %{GREEDYDATA:msg}" 
    }
  }
  
  # Convert timestamp
  date {
    match => [ "timestamp", "ISO8601" ]
    target => "@timestamp"
  }
  
  # Add fields
  mutate {
    add_field => { "environment" => "production" }
    remove_field => [ "temp_field" ]
  }
  
  # Parse user agent (browsers)
  useragent {
    source => "user_agent"
    target => "ua"
  }
  
  # GeoIP (geolocation)
  geoip {
    source => "client_ip"
  }
}

# OUTPUT - Destination
output {
  # Send to Elasticsearch
  elasticsearch {
    hosts => ["localhost:9200"]
    index => "logs-%{+YYYY.MM.dd}"
  }
  
  # Print to console (debug)
  stdout {
    codec => rubydebug
  }
  
  # Send to file
  file {
    path => "/var/log/logstash/output.log"
  }
}
```

### Grok Patterns (Parsing)

```ruby
# Apache log
%{COMBINEDAPACHELOG}

# Java log
%{TIMESTAMP_ISO8601:timestamp} \[%{LOGLEVEL:level}\] %{JAVACLASS:class} - %{GREEDYDATA:message}

# Custom log
%{IP:client_ip} - - \[%{HTTPDATE:timestamp}\] "%{WORD:method} %{URIPATHPARAM:request}"
```

---

## 4. Kibana

### What is it?

**Kibana** is the visualization interface for Elasticsearch.

### Main Features

1. **Discover** - Explore logs
2. **Visualize** - Create charts
3. **Dashboard** - Dashboards
4. **Canvas** - Presentations
5. **Machine Learning** - Anomaly detection
6. **Alerts** - Notifications

### Visualization Types

| Type | Usage |
|------|-------|
| **Line Chart** | Trends over time |
| **Bar Chart** | Comparisons |
| **Pie Chart** | Proportions |
| **Data Table** | Tabular data |
| **Metric** | KPI (single number) |
| **Heat Map** | Intensity on a grid |
| **Tag Cloud** | Frequent words |
| **Map** | Geographic data |

### Kibana Dashboard Example

```
┌─────────────────────────────────────────────────┐
│ Production Dashboard - 26/10/2024               │
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

### Query DSL in Kibana

```
# Simple search
level: ERROR

# Wildcard search
message: *database*

# Search with operators
level: ERROR AND service: user-service

# Range
timestamp: [2024-10-26 TO 2024-10-27]

# Exists
_exists_: user_id

# Complex search
(level: ERROR OR level: FATAL) AND NOT service: test-service
```

---

## 5. Filebeat

### What is it?

**Filebeat** is a lightweight agent that:
- Reads log files
- Sends data to Logstash or Elasticsearch
- **Lighter than Logstash** (consumes fewer resources)

### Why Filebeat instead of Logstash?

| Aspect | Filebeat | Logstash |
|--------|----------|----------|
| **Weight** | Very light (~10MB) | Heavy (~200MB) |
| **CPU/RAM** | Low | High |
| **Deployment** | On each server | Centralized |
| **Transformation** | Basic | Advanced |
| **Usage** | Collect and send | Parse and transform |

### Architecture with Filebeat

```
Server 1                     Central Server
┌─────────────┐
│ Application │              ┌──────────────┐
│   + Logs    │              │   Logstash   │
└──────┬──────┘              │  (Transform) │
       │                     └──────┬───────┘
   ┌───▼────┐                       │
   │Filebeat├───────────────────────┤
   └────────┘                       │
                                    ▼
Server 2                     ┌─────────────┐
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

### Filebeat Configuration

```yaml
# filebeat.yml

# INPUT - Files to monitor
filebeat.inputs:
  - type: log
    enabled: true
    paths:
      - /var/log/app/*.log
      - /var/log/app/*/*.log
    
    # Additional fields
    fields:
      service: user-service
      environment: production
    
    # Multiline (Java stack traces)
    multiline.pattern: '^[[:space:]]'
    multiline.negate: false
    multiline.match: after
  
  - type: log
    enabled: true
    paths:
      - /var/log/nginx/access.log
    fields:
      log_type: nginx

# OUTPUT - To Logstash
output.logstash:
  hosts: ["logstash:5044"]
  
# OR OUTPUT - Direct to Elasticsearch
# output.elasticsearch:
#   hosts: ["elasticsearch:9200"]
#   index: "filebeat-%{+yyyy.MM.dd}"

# Monitoring
monitoring.enabled: true

# Processors (lightweight transformations)
processors:
  - add_host_metadata: ~
  - add_cloud_metadata: ~
  - add_docker_metadata: ~
```

### Filebeat Modules

Filebeat includes pre-configured modules:

```bash
# List available modules
filebeat modules list

# Enable a module
filebeat modules enable nginx
filebeat modules enable apache
filebeat modules enable mysql

# Nginx module configuration
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

### What is it?

**Zipkin** is a **distributed tracing system** for:
- Tracking requests across microservices
- Identifying bottlenecks
- Debugging latency

### Difference: Logs vs Traces

| Aspect | Logs (ELK) | Traces (Zipkin) |
|--------|-----------|-----------------|
| **Focus** | Individual events | Request flow |
| **Scope** | One service | Multi-services |
| **Usage** | Debug errors | Analyze performance |
| **Data** | Text messages | Spans with timing |

### Zipkin Concepts

```
Trace (ID: abc123) - Complete request
├── Span 1: API Gateway (100ms)
│   ├── Span 2: User Service (50ms)
│   │   └── Span 3: Database Query (30ms)
│   └── Span 4: Order Service (80ms)
│       ├── Span 5: Payment Service (40ms)
│       └── Span 6: Inventory Service (35ms)
```

**Trace** = A complete request
**Span** = An operation within a service

### Zipkin Architecture

```
┌───────────┐     ┌───────────┐     ┌───────────┐
│  Service  │────▶│  Service  │────▶│  Service  │
│     A     │     │     B     │     │     C     │
└─────┬─────┘     └─────┬─────┘     └─────┬─────┘
      │                 │                 │
      │ Sends traces    │                 │
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
                 │     or      │
                 │Elasticsearch│
                 └─────────────┘
```

### Zipkin Configuration with Spring Boot

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
  
  # Sleuth (automatic tracing)
  sleuth:
    sampler:
      probability: 1.0  # 1.0 = 100% of traces (in prod: 0.1 = 10%)
```

### Trace Example

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
        // Span automatically created by Sleuth
        log.info("Fetching user {}", id);
        
        // Create a custom span
        Span span = tracer.nextSpan().name("fetch-user-details");
        try (Tracer.SpanInScope ws = tracer.withSpan(span.start())) {
            // Call to Service B (trace propagated automatically)
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

### Zipkin Interface

```
┌─────────────────────────────────────────────────┐
│ Zipkin UI - Trace: abc123                       │
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

### ELK vs Zipkin - When to use which?

```
┌──────────────────────────────────────────────┐
│ Problem: "The API is slow"                   │
├──────────────────────────────────────────────┤
│ ELK (Logs):                                  │
│ - Search for errors                          │
│ - View exceptions                            │
│ - Analyze messages                           │
│                                              │
│ Zipkin (Traces):                             │
│ - Identify which service is slow             │
│ - Measure time of each call                  │
│ - Visualize the full flow                    │
└──────────────────────────────────────────────┘

→ Use BOTH together for complete debugging!
```

---

## 7. Docker Compose - Complete ELK + Zipkin Stack

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

## 8. Interview Questions - ELK & Zipkin

### Q1: What is ELK and why use it?

**Answer:**
ELK = Elasticsearch + Logstash + Kibana. It is a stack for:
- **Centralizing** logs from all services
- **Searching** quickly through millions of logs
- **Visualizing** with dashboards
- **Alerting** in case of problems

**Advantages:**
- Ultra-fast full-text search
- Scalable (thousands of servers)
- Open-source and flexible

### Q2: Difference between Logstash and Filebeat?

**Answer:**

| Aspect | Filebeat | Logstash |
|--------|----------|----------|
| Weight | Light (~10MB) | Heavy (~200MB) |
| Deployment | On each server | Centralized |
| Function | Collect | Transform |
| CPU/RAM | Low | High |

**Recommended pattern:** Filebeat (collect) → Logstash (transform) → Elasticsearch

### Q3: How is Elasticsearch distributed?

**Answer:**
Elasticsearch uses **shards** (fragments) and **replicas**:
- An index is split into multiple shards
- Each shard has replicas (copies)
- Shards are distributed across different nodes
- **High availability** if a node goes down
- **Performance** with parallel queries

### Q4: What is a document in Elasticsearch?

**Answer:**
A document is the basic unit stored as JSON:
```json
{
  "timestamp": "2024-10-26T10:30:00",
  "level": "ERROR",
  "service": "user-service",
  "message": "Connection refused"
}
```
Equivalent to a row in a SQL database.

### Q5: What are Grok patterns?

**Answer:**
Grok is a pattern matching language for parsing unstructured logs:

```ruby
# Raw log:
10.0.0.1 - - [26/Oct/2024:10:30:00] "GET /api/users HTTP/1.1" 200

# Grok pattern:
%{IP:client_ip} - - \[%{HTTPDATE:timestamp}\] "%{WORD:method} %{URIPATHPARAM:request}" %{NUMBER:status}

# Structured result:
{
  "client_ip": "10.0.0.1",
  "timestamp": "26/Oct/2024:10:30:00",
  "method": "GET",
  "request": "/api/users",
  "status": "200"
}
```

### Q6: Difference between ELK and Zipkin?

**Answer:**

| Aspect | ELK | Zipkin |
|--------|-----|--------|
| Type | Logs | Traces |
| Focus | Events | Request flows |
| Usage | Debug errors | Analyze latency |
| Scope | One service | Multi-services |

**Complementary:** ELK for *what happened*, Zipkin for *where the bottleneck is*.

### Q7: What is a Span in Zipkin?

**Answer:**
A **Span** represents an operation in a service with:
- Operation name
- Start/end timestamp
- Duration
- Tags (metadata)
- Associated service

Multiple spans form a **Trace** (complete request).

### Q8: How do you propagate trace context between services?

**Answer:**
Via **HTTP headers**:
- `X-B3-TraceId`: Unique trace ID
- `X-B3-SpanId`: Current span ID
- `X-B3-ParentSpanId`: Parent span ID

Spring Cloud Sleuth does this **automatically**!

### Q9: Kibana vs Grafana - Difference?

**Answer:**

| Aspect | Kibana | Grafana |
|--------|--------|---------|
| Source | Elasticsearch only | Multiple (Prometheus, MySQL, etc.) |
| Focus | Logs | Metrics |
| Integration | ELK Stack | Monitoring (Prometheus) |

**In enterprise:** Often both! Kibana for logs, Grafana for metrics.

### Q10: Elasticsearch performance - How to optimize?

**Answer:**
1. **Increase shards** to distribute the load
2. **Index Lifecycle Management** (ILM) - delete old logs
3. **Correct mapping** - appropriate field types
4. **Filter before aggregation** - reduce processed data
5. **Cache** - enable query cache
6. **Hardware** - SSD, more RAM

---

# Technical Interview Q&A - Complete Guide


# SECTION 1: ELK STACK & MONITORING

### Q1: What is ELK and what is it used for?

**Answer:**
**ELK** = Elasticsearch + Logstash + Kibana

**Stack for:**
- **Centralizing** logs from all services
- **Searching** quickly through millions of logs
- **Visualizing** with dashboards
- **Alerting** in case of problems

**Usage example:**
```
Application 1 ──┐
Application 2 ──┤──▶ Filebeat ──▶ Logstash ──▶ Elasticsearch
Application 3 ──┘                               ▲
                                                │
                                             Kibana (visualization)
```

### Q2: Difference between Logstash and Filebeat?

**Answer:**

| Aspect | Filebeat | Logstash |
|--------|----------|----------|
| Weight | Light (~10MB) | Heavy (~200MB) |
| CPU/RAM | Low | High |
| Deployment | On each server | Centralized |
| Function | Collect and send | Parse and transform |
| Transformation | Basic | Advanced (Grok, filters) |

**Recommended pattern:**
```
Servers ──▶ Filebeat ──▶ Logstash ──▶ Elasticsearch
           (collect)   (transform)    (storage)
```

### Q3: How does Elasticsearch work?

**Answer:**
Elasticsearch is a distributed search engine:

**Concepts:**
- **Index**: Collection of documents (≈ database)
- **Document**: Basic unit in JSON (≈ row)
- **Shard**: Fragment of an index (for distribution)
- **Replica**: Copy of a shard (for high availability)

**Architecture:**
```
Elasticsearch Cluster
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

### Q4: What is a Grok pattern?

**Answer:**
Grok is a language for parsing unstructured logs:

**Raw log:**
```
192.168.1.1 - - [26/Oct/2024:10:30:00] "GET /api/users HTTP/1.1" 200
```

**Grok pattern:**
```ruby
%{IP:client_ip} - - \[%{HTTPDATE:timestamp}\] "%{WORD:method} %{URIPATHPARAM:request}" %{NUMBER:status}
```

**Result:**
```json
{
  "client_ip": "192.168.1.1",
  "timestamp": "26/Oct/2024:10:30:00",
  "method": "GET",
  "request": "/api/users",
  "status": "200"
}
```

### Q5: Difference between ELK and Zipkin?

**Answer:**

| Aspect | ELK | Zipkin |
|--------|-----|--------|
| Type | **Logs** (events) | **Traces** (flows) |
| Focus | What happened | Where the latency issue is |
| Scope | One service | Multi-services |
| Usage | Debug errors | Analyze performance |
| Data | Text messages | Spans with timing |

**Example:**

**ELK** - Finding an error:
```
ERROR 2024-10-26 10:30:00 [user-service] Database connection failed
```

**Zipkin** - Analyzing latency:
```
Trace: abc123
├── API Gateway: 100ms
├── User Service: 50ms
│   └── Database: 30ms  ← Bottleneck!
└── Order Service: 80ms
```

**Complementary:** Use BOTH!

### Q6: What is a Span in Zipkin?

**Answer:**
A **Span** represents an operation in a service:

```
Span {
  traceId: "abc123",           ← ID of the complete trace
  spanId: "span-1",            ← Unique span ID
  parentSpanId: "span-0",      ← Parent span
  name: "database-query",      ← Operation name
  timestamp: 1635235800000,    ← Start time
  duration: 30,                ← Duration in ms
  tags: {
    "db.type": "mysql",
    "db.query": "SELECT * FROM users"
  }
}
```

Multiple spans = a **Trace** (complete request)

### Q7: How do you configure Zipkin with Spring Boot?

**Answer:**
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

Spring Cloud Sleuth **automatically propagates** the trace context!

### Q8: Kibana vs Grafana?

**Answer:**

| Aspect | Kibana | Grafana |
|--------|--------|---------|
| Data source | Elasticsearch only | Multiple (Prometheus, MySQL, etc.) |
| Focus | **Logs** | **Metrics** |
| Integration | ELK Stack | Monitoring (Prometheus) |
| Visualizations | Advanced for logs | Advanced for time-series |

**In enterprise:** Often BOTH!
- **Kibana** for analyzing logs
- **Grafana** for monitoring metrics (CPU, RAM, requests/sec)

### Q9: How do you optimize Elasticsearch?

**Answer:**

**1. Correct sharding**
```json
PUT /logs-2024
{
  "settings": {
    "number_of_shards": 5,
    "number_of_replicas": 1
  }
}
```

**2. Index Lifecycle Management (ILM)** - Delete old logs
```
Hot (0-7 days) → Warm (7-30 days) → Cold (30-90 days) → Delete
```

**3. Optimized mapping**
```json
{
  "mappings": {
    "properties": {
      "level": { "type": "keyword" },  // Not "text" for enums
      "timestamp": { "type": "date" },
      "message": { "type": "text" }
    }
  }
}
```

**4. Filter before aggregating**
```json
GET /logs/_search
{
  "query": { "range": { "timestamp": { "gte": "now-1h" }}},  ← Filter first
  "aggs": { "by_level": { "terms": { "field": "level" }}}
}
```
