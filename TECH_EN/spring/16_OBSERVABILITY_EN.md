## [**..**](./00_index.md)

# Observability & Monitoring of Spring Boot Microservices


---

## 🔍 Logging & Log Centralization

### Implementation with SL4J
- Configuration of **SL4J** as logging facade in Spring Boot microservices
- Definition of appropriate log levels (DEBUG, INFO, WARN, ERROR) per package and environment
- Structuring logs with **MDC (Mapped Diagnostic Context)** to inject contextual information (userId, correlationId, requestId)
- **Example**: Automatic injection of `traceId` into each log to correlate events from the same distributed request

### Log Storage and Rotation
- Configuration of log storage to files with **Logback** or **Log4j2**
- Setting up automatic rotation policies (by size, by date)
- Archiving and compressing historical logs to optimize disk space
- **Example**: Daily rotation with 30-day retention in production, gzip compression of archives

---

## 📦 ELK Stack - Complete Log Management Solution

### Distributed ELK Architecture

#### Filebeat - Lightweight Log Collection
- Deploying **Filebeat** as a lightweight collection agent on each application node
- Configuration of **inputs** to monitor Spring Boot log files (application.log, error.log)
- Setting up **multiline patterns** to handle multi-line Java stack traces
- Configuration of **backpressure** and **buffering** to handle load spikes
- **Example**: Filebeat configuration to collect logs from `/var/log/app/*.log` with automatic parsing of multi-line Java stack traces

#### Logstash - Transformation Pipeline
- Design of **Logstash pipelines** to enrich, filter and transform logs in real time
- Using advanced **filters**:
  - **Grok patterns** to parse Spring Boot logs and extract structured fields (timestamp, level, thread, logger, message)
  - **Mutate** to add/remove/rename fields
  - **Date** to parse and normalize timestamps
  - **GeoIP** to enrich with IP geolocation
  - **Dissect** for fast parsing of simple formats
- Setting up **conditionals** to route logs based on their content
- **Concrete pipeline example**:
  ```
  Input: "[2024-11-01 10:30:45] INFO [order-service,a1b2c3,d4e5f6] OrderController - Order created: orderId=12345, userId=user789, amount=150.50"
  
  After Grok: 
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

#### Elasticsearch - Storage and Indexing
- Configuration of multi-node **Elasticsearch cluster** for high availability
- Definition of **index templates** and **mappings** optimized for Spring Boot logs
- Setting up **Index Lifecycle Management (ILM)**:
  - **Hot phase**: recent indices (0-7 days) on SSD, all replicas
  - **Warm phase**: older indices (7-30 days) on standard disks, reduced replicas
  - **Cold phase**: archived indices (30-90 days) in read-only
  - **Delete phase**: deletion after 90 days
- Configuration of **sharding** and **replication** adapted to log volume
- Performance optimization with **bulk indexing** and **refresh intervals**
- **Example**: Index pattern `logs-spring-boot-{env}-{date}` with automatic rollover at 50GB or 30 days

#### Kibana - Visualization and Analysis
- Creation of interactive **Kibana dashboards** for log analysis:
  - **Overview Dashboard**: log volume per service, distribution by level, top errors
  - **Error Analysis Dashboard**: most frequent stack traces, error trends, impact by service
  - **Performance Dashboard**: latency distributions, slow queries, API response times
  - **Business Dashboard**: business metrics extracted from logs (orders/hour, payment success rate)
- Configuration of **Discover** with saved searches for frequent queries
- Setting up **Canvas** for advanced visual presentations
- **Example**: Real-time dashboard showing 5xx errors per microservice with drill-down to complete stack traces

### Kibana Alerting & Watchers
- Configuration of **Kibana alerts** based on Elasticsearch queries
- Setting up **Watchers** (X-Pack) for proactive monitoring:
  - Anomaly detection in log patterns
  - Alerts on sudden increase in errors
  - Service availability monitoring
- **Alert example**: Trigger if more than 50 5xx errors in 5 minutes on the payment service → Slack notification with log excerpts and stack traces

### Advanced Search and Debugging
- Using **Elasticsearch Query DSL** for complex searches:
  - **Bool queries**: combination of must/should/must_not
  - **Range queries**: precise time filtering
  - **Wildcard/Regex**: pattern search in messages
  - **Aggregations**: statistics and grouped analyses
- **Query example**: Find all ERROR logs from order-service containing "NullPointerException" in the last 2 hours, grouped by endpoint

---

## 🔄 ELK vs Grafana Stack Comparison

### ELK Stack (Elastic, Logstash, Kibana, Filebeat)
**Advantages**:
- **Elasticsearch**: extremely powerful full-text search engine
- **Kibana**: rich UI for log exploration and analysis
- Mature ecosystem with a large community
- Excellent for complex queries and large volume analysis

**Best use cases**:
- Deep debugging with complex full-text searches
- Forensic analysis of historical logs
- Compliance and audit trails
- Environments with very high log volumes (TB/day)

### Grafana Stack (Loki, Promtail, Tempo)

#### Grafana Loki & Promtail Stack
- Deploying **Loki** as a centralized, scalable and performant log storage system
- Installation and configuration of **Promtail** as collection agent on each application node
- Definition of relevant **labels** (environment, service_name, instance, level) to facilitate filtering
- Creation of advanced **LogQL** queries to analyze logs in real time
- **Example**: LogQL query to identify all ERROR logs from a specific microservice in the last 15 minutes with keyword filtering

**Advantages**:
- **Reduced storage cost**: only labels are indexed, not content
- **Performance**: faster queries for targeted use cases
- **Simplicity**: lighter architecture than ELK
- **Native integration** with Prometheus and Tempo in Grafana

**Best use cases**:
- Real-time operational monitoring
- Logs/metrics/traces correlation in a single UI
- Cloud-native and Kubernetes environments
- Limited budget and resources

### Hybrid Approach (Best of Both Worlds)
In some projects, I implemented a **hybrid approach**:
- **ELK**: for detailed application logs, debugging, compliance (long retention)
- **Grafana Loki**: for system logs, real-time monitoring, operational alerting
- **Architecture example**: Filebeat → Logstash → (Elasticsearch + Loki) with intelligent routing based on log type

---

## 📊 Application Metrics & Monitoring

### Micrometer & Prometheus Integration
- Configuration of **Micrometer** as metrics abstraction in Spring Boot via `spring-boot-starter-actuator`
- Exposing the `/actuator/prometheus` endpoint for metrics scraping
- Automatic instrumentation of standard metrics: JVM (heap, threads, GC), HTTP requests (latency, throughput, error rate), DB connections, cache hit ratio
- **Example**: Heap memory monitoring with alerts if usage > 85%

### Custom Metrics
- Creation of business **custom metrics** with `@Timed`, `Counter`, `Gauge`, `Timer`
- **Concrete example**: 
  - `Counter` to count the number of created orders: `order.created.total`
  - `Timer` to measure latency of calls to an external API: `external.api.payment.duration`
  - `Gauge` to monitor the size of a message queue: `message.queue.size`

### Prometheus Configuration
- Installation and configuration of **Prometheus** with definition of scraping jobs
- Configuration of the optimized `scrape_interval` (15-30s depending on needs)
- Setting up **service discovery** (Kubernetes, Consul) to automatically discover instances
- **Example**: Scraping configuration for all Spring Boot pods labeled `app=payment-service` in Kubernetes

### Grafana Dashboards
- Creation of **custom dashboards** for each microservice with relevant panels:
  - **Golden Signals**: latency, traffic, errors, saturation
  - **JVM Metrics**: heap usage, GC pause time, thread count
  - **Business Metrics**: transactions/min, conversion rate, active users
- Use of community dashboards (Spring Boot Statistics, JVM Micrometer)
- Organization in **folders** by domain (Infrastructure, Application, Business)
- **Example**: "Payment Service Overview" dashboard with panels for 5xx error rate, p95 latency, transaction volume, and correlation with deployments

### Elastic APM (Alternative for Metrics & Tracing)
- Deployment of **Elastic APM Server** for application performance monitoring
- Integration of the **Java APM agent** into Spring Boot microservices
- Automatic collection of:
  - **Transactions**: latency, throughput per endpoint
  - **Spans**: detail of operations (DB queries, HTTP calls, methods)
  - **Errors**: stack traces and full context
  - **JVM Metrics**: integrated in the same Kibana UI
- Visualization in **Kibana APM UI** with service maps and dependency graphs
- **Example**: Service map showing dependencies between microservices with average latencies and error rates per link

---

## 🚨 Alerting & Notifications

### Alerting Strategy
- Definition of **alert rules** in Grafana based on critical thresholds and trends
- Classification of alerts by severity (Critical, Warning, Info)
- Setting up **grace period** and **repeat interval** to avoid spam
- **Alert examples**:
  - **Critical**: Error rate > 5% for 5 minutes
  - **Critical**: Latency p99 > 2s for 3 minutes
  - **Warning**: Memory heap usage > 80% for 10 minutes
  - **Critical**: Service down (no scrape for 2 minutes)

### Multi-channel Notifications
- Configuration of **notification channels**: Slack, Email, PagerDuty, Microsoft Teams
- Differentiated routing based on severity (Critical → PagerDuty + Slack, Warning → Slack only)
- Enriching notifications with context (service, environment, runbook links)
- **Example**: Slack alert with formatted message including a direct link to the Grafana dashboard and Loki/Kibana logs filtered for the incident period

### Alerting in ELK Stack
- Configuration of **Kibana Alerts** for log monitoring
- Using **Elasticsearch Watchers** for complex aggregation-based alerts
- **Watcher example**: Detect if payment error rate exceeds 3% over 10 minutes → execute webhook to ticketing system

---

## 🔗 Distributed Tracing

### Implementation with OpenTelemetry & Tempo
- Integration of **OpenTelemetry** (CNCF standard) as tracing instrumentation in Spring Boot
- Automatic configuration via `spring-cloud-starter-sleuth` and `opentelemetry-java-instrumentation`
- Propagation of **trace context** (traceId, spanId) across HTTP calls, messaging (Kafka, RabbitMQ), and DB queries
- Deployment of **Grafana Tempo** as distributed and performant trace storage backend

### Elastic APM for Distributed Tracing
- Alternative with **Elastic APM** for distributed tracing in the ELK ecosystem
- Trace visualization in **Kibana APM** with detailed timeline
- Automatic **Service Maps** showing dependencies and request flows
- Native integration with Elasticsearch logs for immediate correlation
- **Example**: Click on a slow transaction in APM → span breakdown displayed → click "View Logs" → logs automatically filtered by traceId in Kibana Discover

### Trace Analysis
- Visualization of the complete **flamegraph** of a request traversing 5-6 microservices
- Identification of **bottlenecks** and abnormal latencies in the call chain
- Automatic correlation traces ↔ logs ↔ metrics via traceId
- **Concrete example**: 
  - User request: `POST /api/orders`
  - Trace showing: API Gateway (50ms) → Order Service (120ms) → Inventory Service (800ms ⚠️) → Payment Service (200ms) → Notification Service (100ms)
  - Immediate identification that Inventory Service is the bottleneck with a slow DB query
  - Drill-down to the specific logs of the problematic span

### Sampling Strategy
- Configuration of appropriate **sampling** (probabilistic, rate-limiting) to manage production volume
- **Example**: 10% sampling in normal production, 100% for error or slow requests (> 1s)
- In Elastic APM: configuration of sampling rate per transaction type

---

## 🔧 Integration and Best Practices

### Logs - Metrics - Traces Correlation
- Implementation of a **correlation ID** (traceId) injected into all logs via MDC
- Configuration of direct links between Grafana panels and their corresponding logs
- **Debugging workflow example**:
  1. Grafana alert: high p99 latency on order-service
  2. Click link → Dashboard with detailed metrics
  3. Identification of the problematic timeframe
  4. Click "Explore Logs" → Loki/Kibana automatically filtered by service + timeframe
  5. Click on a log with traceId → Full trace visualization in Tempo/APM
  6. Root cause identified in 2 minutes

### Structured Logging
- Adoption of **structured logging** (JSON format) to facilitate parsing
- Using **Logstash JSON Encoder** or **Logback JSON Layout**
- **Example** : 
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

### Multiple Environments
- Differentiated configuration per environment (dev, staging, prod)
- **Dev**: verbose logs (DEBUG), short retention (7 days), 100% sampling
- **Staging**: INFO logs, medium retention (30 days), 50% sampling
- **Production**: INFO/WARN/ERROR logs, long retention (90 days), 10-20% sampling

---

## 🎯 Benefits & Results

This complete observability implementation with dual expertise in **ELK** and **Grafana Stack** enables:
- **MTTD (Mean Time To Detect)** reduced by 80% thanks to proactive alerts
- **MTTR (Mean Time To Resolve)** reduced by 60-70% thanks to logs/metrics/traces correlation
- **End-to-end visibility** across the entire distributed architecture
- **Capacity planning** made easier with historical metrics and trend analysis
- **Continuous performance improvement** based on observable data
- **Accelerated root cause analysis** with smooth navigation between logs/metrics/traces
- **Adapted technology choice**: ability to recommend ELK vs Grafana based on context (volume, budget, use cases)

---
# Q&A - Observability & Monitoring Spring Boot

## 🔍 General Questions about Observability

### Q1: What is observability and how is it different from traditional monitoring?

**A:** **Observability** is the ability to understand the internal state of a system by analyzing its external outputs. It is based on three fundamental pillars:
- **Logs**: discrete records of events
- **Metrics**: aggregated measurements over time
- **Traces**: tracking of requests across distributed systems

The difference from traditional monitoring:
- **Monitoring**: answers questions known in advance ("Is the service up?", "What is the average latency?")
- **Observability**: allows you to explore and understand unknown problems, investigate the "why" behind the metrics

**Concrete example**: Monitoring tells you "the service has 5% errors", observability lets you trace a specific failed request, see all its logs, identify the problematic microservice and the slow DB query responsible.

---

### Q2: Why did you choose to use SL4J instead of logging directly with Log4j or Logback?

**A:** **SL4J (Simple Logging Facade for Java)** is a facade/abstraction that offers several critical advantages:

1. **Implementation independence**: Application code is not coupled to a specific logging library
2. **Flexibility**: Ability to change the implementation (Logback, Log4j2, JUL) without modifying the code
3. **Standardization**: Simple and uniform API for the whole team
4. **Performance**: Lazy evaluation with parameterized messages

**Example** :
```java
// Bad practice - expensive concatenation even when DEBUG is disabled
logger.debug("Order details: " + order.toString());

// Good practice SL4J - lazy evaluation
logger.debug("Order details: {}", order);
// toString() is only called if DEBUG is enabled
```

In my projects, I use **SL4J** as facade with **Logback** as the default implementation (native in Spring Boot), allowing for easy future migration if needed.

---

## 📦 Questions about ELK Stack

### Q3: Explain the complete architecture of an ELK stack for Spring Boot microservices

**A:** The typical ELK architecture I implement follows this flow:

```
Spring Boot App (SL4J) 
    ↓ (writes to files)
/var/log/app/application.log
    ↓ (collection)
Filebeat (agent léger)
    ↓ (shipping)
Logstash (transformation/enrichissement)
    ↓ (indexing)
Elasticsearch (stockage/recherche)
    ↓ (visualization)
Kibana (UI/dashboards)
```

**Details of each component**:
- **Filebeat**: Lightweight agent (~50MB RAM) that reads log files, manages backpressure, guarantees delivery
- **Logstash**: Transformation pipeline with Grok filters to parse, enrich (GeoIP, date), filter and route
- **Elasticsearch**: Distributed storage cluster with sharding, replication, and index lifecycle management
- **Kibana**: Web interface for search, visualization, dashboards and alerting

**Simplified alternative**: For moderate volumes, Filebeat can send directly to Elasticsearch, bypassing Logstash.

---

### Q4: How do you handle multi-line Java stack traces in ELK?

**A:** Java stack traces are a major challenge because they span multiple lines but represent a single event. I use several techniques:

**1. Filebeat configuration with multiline pattern**:
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
This pattern groups all lines that don't start with a date with the previous line.

**2. Logstash Grok pattern to parse the complete stack trace**:
```ruby
filter {
  grok {
    match => { 
      "message" => "%{TIMESTAMP_ISO8601:timestamp} %{LOGLEVEL:level}.*?(?<exception>.*Exception[^\n]*(?:\n\s+at .*)*)"
    }
  }
}
```

**Result**: An error with 50 lines of stack trace becomes a single Elasticsearch document with an `exception` field containing the entire trace, making search and analysis easier.

---

### Q5: Explain your Index Lifecycle Management (ILM) strategy in Elasticsearch

**A:** **ILM** is critical for optimizing storage costs and performance. My typical strategy:

**Hot Phase (0-7 days)**:
- Indices on high-performance SSD
- 2 replicas for high availability
- Rollover at 50GB or 1 day
- Allocation priority: 100
- **Use case**: Frequent searches, active debugging

**Warm Phase (7-30 days)**:
- Migration to standard disks
- Reduction to 1 replica
- Force merge for optimization
- Priority: 50
- **Use case**: Occasional searches, trend analysis

**Cold Phase (30-90 days)**:
- Indices in read-only
- Migration to cheaper storage
- 0 replicas (searchable snapshots)
- Priority: 0
- **Use case**: Compliance, historical audit

**Delete Phase (> 90 days)**:
- Automatic deletion
- **Exception**: Security logs kept 1 year for compliance

**Configuration example**:
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

**Benefits**: 70% reduction in storage costs on a project with 5TB of logs/month.

---

### Q6: How do you build a Grok pattern to parse custom Spring Boot logs?

**A:** Here's the process I use to create efficient Grok patterns:

**1. Analyze a real log example**:
```
2024-11-01 14:23:45.123 INFO [order-service,a1b2c3d4,e5f6g7h8] 12345 --- [nio-8080-exec-5] c.e.OrderController : Processing order - orderId=ORD-12345, userId=USR-789, amount=250.50, status=SUCCESS
```

**2. Identify standard patterns**:
- Date/Time : `TIMESTAMP_ISO8601` ou custom
- Level : `LOGLEVEL`
- Thread : `\[.*?\]`
- Class : `(?:[a-zA-Z$_][a-zA-Z$_0-9]*\.)*[a-zA-Z$_][a-zA-Z$_0-9]*`

**3. Build the pattern progressively**:
```ruby
%{TIMESTAMP_ISO8601:timestamp} %{LOGLEVEL:level} \[%{DATA:service},%{DATA:traceId},%{DATA:spanId}\] %{NUMBER:pid} --- \[%{DATA:thread}\] %{DATA:logger} : %{GREEDYDATA:message}
```

**4. Extract business data from the message**:
```ruby
grok {
  match => { 
    "message" => "Processing order - orderId=%{DATA:orderId}, userId=%{DATA:userId}, amount=%{NUMBER:amount:float}, status=%{WORD:status}"
  }
}
```

**5. Test with Grok Debugger** in Kibana Dev Tools

**Result**: Each field becomes searchable and usable for aggregations in Kibana.

**Tip**: Create a library of reusable patterns for the team.

---

### Q7: What is the difference between Filebeat and Logstash, and when do you use each?

**A:** **Filebeat** and **Logstash** have complementary but different roles:

**Filebeat** :
- **Nature**: Lightweight shipping agent (~50MB RAM, minimal CPU)
- **Role**: Collects and forwards logs with delivery guarantee
- **Capabilities**: Basic parsing, multiline, some processors
- **Deployment**: One per machine/pod
- **Advantages**: Very lightweight, native backpressure, low overhead

**Logstash** :
- **Nature**: Heavy transformation pipeline (~1GB RAM)
- **Role**: Complex transformation, enrichment, filtering, routing
- **Capabilities**: 200+ plugins, Grok, GeoIP, aggregations, conditionals
- **Deployment**: Centralized (3-5 instances)
- **Advantages**: Powerful transformations, maximum flexibility

**My deployment strategies**:

**Architecture 1 - High volume**:
```
Filebeat → Logstash → Elasticsearch
```
- Filebeat collects and ships
- Logstash parses and enriches (complex parsing, GeoIP, lookups)
- Ideal for > 100GB logs/day

**Architecture 2 - Moderate volume**:
```
Filebeat (avec modules) → Elasticsearch
```
- Filebeat parses directly with predefined modules
- Bypasses Logstash to reduce complexity
- Ideal for < 50GB logs/day, standard patterns

**Architecture 3 - Hybrid**:
```
Filebeat (parsing simple) → Logstash (enrichissement) → Elasticsearch
```
- Filebeat does basic parsing
- Logstash only does GeoIP enrichment and routing
- Optimal balance for 50-100GB logs/day

**Concrete example**: On an e-commerce project, I used Filebeat with modules for simple application logs (90% of the volume), and Logstash only for payment logs requiring complex enrichment with customer data lookup (10% of the volume).

---

## 🔄 ELK vs Grafana Questions

### Q8: When do you recommend ELK over Grafana Loki, and vice-versa?

**A:** The choice depends on several critical factors:

**Recommend ELK Stack when**:
1. **Very high volume**: > 500GB logs/day with long retention needs
2. **Complex full-text search**: Advanced regex, fuzzy search, phrase matching
3. **Forensic analysis**: Deep investigations, compliance, audit trails
4. **Rich structured logs**: Many searchable fields (50+)
5. **Team already trained** on Elasticsearch
6. **Sufficient infrastructure budget**

**Example**: Banking application with compliance requiring search across 2 years of log history with 100+ indexed fields.

**Recommend Grafana Loki when**:
1. **Cloud-native/Kubernetes**: Native integration, automatic service discovery
2. **Operational monitoring**: Focus on real-time alerting, less on forensics
3. **Limited budget**: 10x lower storage cost (no full-text indexing)
4. **Logs-metrics-traces correlation**: Single Grafana UI for everything
5. **Moderate volume**: < 200GB logs/day
6. **Simplicity**: Small team, less maintenance

**Example**: SaaS startup with microservices on Kubernetes, needing operational monitoring, tight budget.

**Hybrid approach I implemented**:
- **Loki**: System logs, real-time monitoring, operational debugging (30-day retention)
- **ELK**: Detailed application logs, financial transactions, audit (1-year retention)
- **Smart routing**: Filebeat decides based on the log type

**Comparison table**:

| Criteria | ELK | Grafana Loki |
|---------|-----|--------------|
| Storage cost | High | Low (10x less) |
| Full-text search | Excellent | Basic |
| Query performance | Fast | Very fast (simple queries) |
| Learning curve | High | Low |
| Metrics correlation | Via APM | Native Grafana |
| Scalability | Excellent | Good |

---

### Q9: How do you implement correlation between logs, metrics and traces?

**A:** Correlation is the key to effective observability. Here is my complete implementation:

**1. Trace Context Propagation**:
```java
// Spring Cloud Sleuth auto-configures traceId/spanId
// Added to MDC for SL4J
@Configuration
public class LoggingConfig {
    @Bean
    public MDCPropagator mdcPropagator() {
        return new MDCPropagator();
    }
}
```

**2. Log pattern with traceId**:
```xml
<!-- logback-spring.xml -->
<pattern>
  %d{ISO8601} %level [${spring.application.name},%X{traceId},%X{spanId}] %thread %logger{36} : %msg%n
</pattern>
```

**Log result** :
```
2024-11-01 14:23:45 INFO [order-service,a1b2c3d4,e5f6g7h8] http-nio-exec-5 OrderController : Order created
```

**3. In Grafana - Metrics → Logs link**:
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

**4. In Loki - Logs → Traces link**:
```yaml
# Loki datasource config
derivedFields:
  - datasourceUid: tempo-uid
    matcherRegex: "traceId=(\\w+)"
    name: TraceID
    url: "$${__value.raw}"
```

**5. In Kibana - Correlation via APM**:
- Elastic APM automatically captures traceId
- Click on transaction → "View Logs" automatically filters Discover by traceId

**Complete debugging workflow**:
1. **Prometheus alert**: "High error rate on order-service"
2. Click on Grafana dashboard → identify period 14:20-14:25
3. Click "Explore Logs" → error logs in Loki
4. Identify a recurring traceId: `a1b2c3d4`
5. Click on traceId → Full trace in Tempo
6. Flamegraph shows: Order Service (50ms) → Inventory Service (3000ms ⚠️)
7. Drill-down to Inventory Service logs with this traceId
8. Found: `SQLException: Deadlock detected`
9. **Root cause found in 90 seconds**

**Tip**: I also add the traceId as a tag in custom metrics:
```java
meterRegistry.counter("order.created", 
    Tags.of("traceId", tracer.currentSpan().context().traceId())
).increment();
```

---

## 📊 Questions about Metrics & Prometheus

### Q10: Explain how you create and expose custom metrics in Spring Boot

**A:** Here is my complete approach for custom metrics:

**1. Basic setup**:
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

**2. MeterRegistry injection**:
```java
@Service
public class OrderService {
    private final MeterRegistry meterRegistry;
    private final Counter orderCreatedCounter;
    private final Timer paymentProcessingTimer;
    
    public OrderService(MeterRegistry meterRegistry) {
        this.meterRegistry = meterRegistry;
        
        // Counter - for discrete events
        this.orderCreatedCounter = Counter.builder("orders.created")
            .description("Total number of orders created")
            .tag("type", "online")
            .register(meterRegistry);
            
        // Timer - to measure latency
        this.paymentProcessingTimer = Timer.builder("payment.processing.duration")
            .description("Time to process payment")
            .publishPercentiles(0.5, 0.95, 0.99)
            .register(meterRegistry);
    }
    
    public Order createOrder(OrderRequest request) {
        // Increment counter
        orderCreatedCounter.increment();
        
        // Measure duration with timer
        return paymentProcessingTimer.record(() -> {
            // Payment processing logic
            return processPayment(request);
        });
    }
}
```

**3. Using Gauge for fluctuating values**:
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

**4. @Timed annotation for automatic metrics**:
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

**5. Result in Prometheus**:
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

**6. PromQL query in Grafana**:
```promql
# Order creation rate per minute
rate(orders_created_total[5m]) * 60

# P95 latency of payment processing
histogram_quantile(0.95, rate(payment_processing_duration_seconds_bucket[5m]))

# Error rate
rate(http_server_requests_seconds_count{status=~"5.."}[5m]) 
/ 
rate(http_server_requests_seconds_count[5m]) * 100
```

**Best practices**:
- Name in snake_case with unit suffix: `_total`, `_seconds`, `_bytes`
- Limit tag cardinality (avoid userId, orderId)
- Use Counter for increasing values, Gauge for fluctuating values
- Timer automatically includes count, sum and histogram

---

### Q11: How do you configure Prometheus for service discovery in Kubernetes?

**A:** In a Kubernetes environment, automatic **service discovery** is essential. Here is my configuration:

**1. Annotations on Spring Boot Pods**:
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

**2. Prometheus configuration with kubernetes_sd_configs**:
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
    
    # Relabeling to filter and enrich
    relabel_configs:
      # Scrape only pods with annotation scrape=true
      - source_labels: [__meta_kubernetes_pod_annotation_prometheus_io_scrape]
        action: keep
        regex: true
      
      # Use the port from the annotation
      - source_labels: [__meta_kubernetes_pod_annotation_prometheus_io_port]
        action: replace
        target_label: __address__
        regex: ([^:]+)(?::\d+)?
        replacement: $1:$2
      
      # Use the path from the annotation
      - source_labels: [__meta_kubernetes_pod_annotation_prometheus_io_path]
        action: replace
        target_label: __metrics_path__
        regex: (.+)
      
      # Add useful labels
      - source_labels: [__meta_kubernetes_namespace]
        target_label: kubernetes_namespace
      - source_labels: [__meta_kubernetes_pod_name]
        target_label: kubernetes_pod_name
      - source_labels: [__meta_kubernetes_pod_label_app]
        target_label: application
      - source_labels: [__meta_kubernetes_pod_label_version]
        target_label: version
```

**3. Alternative: Prometheus Operator with ServiceMonitor**:
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

**4. Result - Targets in Prometheus UI**:
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

**Advantages**:
- **Auto-discovery**: New pods detected automatically
- **Multi-tenant**: Separation by namespace
- **Version tracking**: Metrics per version for gradual rollout
- **No configuration**: No need to modify Prometheus for each new service

**Bonus - Federation for multi-cluster**:
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

## 🔗 Questions about Distributed Tracing

### Q12: Explain how trace context propagation works between microservices

**A:** **Trace context propagation** is the mechanism that allows tracking a request across multiple microservices. Here is how it works in detail:

**1. Anatomy of a Trace Context**:
A trace context typically contains:
- **TraceId**: Unique identifier for the entire request (e.g.: `a1b2c3d4e5f6g7h8`)
- **SpanId**: Identifier of the current segment (e.g.: `x1y2z3`)
- **ParentSpanId**: Link to the parent span
- **Flags**: Sampling, debug options, etc.

**2. Propagation via HTTP Headers (W3C Trace Context)**:
```
GET /api/orders/123 HTTP/1.1
Host: order-service
traceparent: 00-a1b2c3d4e5f6g7h8-x1y2z3w4-01
tracestate: vendor1=value1,vendor2=value2
```

Format `traceparent`: `version-traceId-spanId-flags`

**3. Automatic implementation with Spring Cloud Sleuth**:
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-sleuth</artifactId>
</dependency>
```

Sleuth automatically instruments:
- **RestTemplate**: Adds trace headers
- **WebClient**: Reactive propagation
- **Feign clients**: Automatic injection
- **Messaging** (Kafka, RabbitMQ): Headers in messages
- **Async methods**: Propagation in threads

**4. Complete flow example**:

```
User Request → API Gateway
                 TraceId: a1b2c3d4 (created)
                 SpanId: span-1
                 ↓
               Order Service
                 TraceId: a1b2c3d4 (propagated)
                 SpanId: span-2 (new)
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

**5. Spring Boot code with manual propagation**:
```java
@Service
public class OrderService {
    private final RestTemplate restTemplate;
    private final Tracer tracer;
    
    public Order createOrder(OrderRequest request) {
        // Span created automatically by @Traced or manually
        Span span = tracer.nextSpan().name("create-order").start();
        
        try (Tracer.SpanInScope ws = tracer.withSpanInScope(span)) {
            // Add tags to span
            span.tag("order.id", request.getOrderId());
            span.tag("user.id", request.getUserId());
            span.tag("amount", String.valueOf(request.getAmount()));
            
            // HTTP call - headers propagated automatically
            InventoryResponse inventory = restTemplate.getForObject(
                "http://inventory-service/api/check", 
                InventoryResponse.class
            );
            
            // Add event to span
            span.event("inventory.checked");
            
            if (inventory.isAvailable()) {
                // Payment processing - new child span auto-created
                paymentService.processPayment(request);
                span.event("payment.processed");
            }
            
            return order;
        } catch (Exception e) {
            // Mark the error in the span
            span.error(e);
            throw e;
        } finally {
            span.finish();
        }
    }
}
```

**6. Propagation in asynchronous messages (Kafka)**:
```java
@Service
public class OrderPublisher {
    private final KafkaTemplate<String, Order> kafkaTemplate;
    
    public void publishOrder(Order order) {
        // Sleuth automatically injects trace headers into Kafka headers
        kafkaTemplate.send("orders-topic", order);
    }
}

@Service
public class OrderConsumer {
    @KafkaListener(topics = "orders-topic")
    public void consumeOrder(Order order, 
                            @Header(KafkaHeaders.RECEIVED_MESSAGE_KEY) String key,
                            @Headers MessageHeaders headers) {
        // TraceId automatically extracted from Kafka headers
        // New span created with correct parent
        log.info("Processing order: {}", order.getId());
        // Log will automatically contain the traceId
    }
}
```

**7. Visualization in Grafana Tempo**:
The complete trace shows:
```
create-order (250ms total)
├─ inventory-service/check (50ms)
│  └─ postgres/query (45ms)
├─ payment-service/process (180ms)
│  ├─ stripe-api/charge (150ms)
│  └─ postgres/insert (20ms)
└─ kafka/publish (15ms)
```

**8. OpenTelemetry configuration (modern standard)**:
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
## 🔗 Questions about Distributed Tracing

### Q13: How do you manage sampling in distributed tracing and why is it important?

**A:** **Sampling** is critical to manage the volume of traces in production while capturing critical information. Without sampling, storing 100% of traces can be very expensive and overload the system.

**1. Types of Sampling**:

**Head-based Sampling (decision at trace start)**:
```java
@Configuration
public class TracingConfig {
    @Bean
    public Sampler sampler() {
        // Probability Sampler - 10% of traces
        return Sampler.traceIdRatioBased(0.1);
    }
}
```

**Tail-based Sampling (decision at the end, smarter)**:
```yaml
# Configuration Tempo
overrides:
  defaults:
    ingestion:
      rate_limit_bytes: 15000000
      burst_size_bytes: 20000000
```

**2. My adaptive sampling strategy**:

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
        
        // Rule 1: ALWAYS sample errors
        if (isError(attributes)) {
            return SamplingResult.recordAndSample();
        }
        
        // Rule 2: ALWAYS sample slow requests
        Duration duration = getDuration(attributes);
        if (duration != null && duration.toMillis() > 1000) {
            return SamplingResult.recordAndSample();
        }
        
        // Rule 3: Sample 100% of critical endpoints
        String endpoint = attributes.get(AttributeKey.stringKey("http.route"));
        if (CRITICAL_ENDPOINTS.contains(endpoint)) {
            return SamplingResult.recordAndSample();
        }
        
        // Rule 4: Sample based on environment
        String env = System.getenv("SPRING_PROFILES_ACTIVE");
        if ("dev".equals(env) || "staging".equals(env)) {
            return SamplingResult.recordAndSample(); // 100% in dev/staging
        }
        
        // Rule 5: Normal production - 5% probabilistic
        return Math.random() < 0.05 
            ? SamplingResult.recordAndSample() 
            : SamplingResult.drop();
    }
}
```

**3. Configuration per environment**:

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
      error: 1.0      # 100% of errors
      slow: 1.0       # 100% of requests > 1s
      critical-path: 1.0  # 100% of critical endpoints
```

**4. Sampling Rate Limiter to control cost**:
```java
@Configuration
public class RateLimitedSamplingConfig {
    
    @Bean
    public Sampler rateLimitedSampler() {
        // Maximum 100 traces/second
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

**5. Sampling impact and metrics**:

**Without smart sampling** (100% traces):
- Volume: 500M traces/day
- Tempo storage cost: ~$15,000/month
- Latency overhead: 15-20ms per request

**With adaptive sampling** (5% base + rules):
- Volume: 35M traces/day (7% effective)
- Storage cost: ~$1,200/month (92% reduction)
- Latency overhead: 2-3ms per request
- **Coverage**: 100% of errors and slow requests captured

**6. Sampling monitoring**:
```promql
# Effective sampling rate
sum(rate(traces_sampled_total[5m])) 
/ 
sum(rate(traces_total[5m])) * 100

# Verify that we are capturing errors
sum(rate(traces_sampled_total{error="true"}[5m])) 
/ 
sum(rate(traces_total{error="true"}[5m])) * 100
# Should be close to 100%
```

**Result**: Optimal balance between cost, performance, and visibility on problems.

---

### Q14: How do you use distributed tracing to identify performance bottlenecks?

**A:** Distributed tracing is my main tool for **performance tuning**. Here is my methodology:

**1. Identifying latency patterns**:

**In Grafana Tempo/Jaeger**:
- Filter traces by service: `service.name = "order-service"`
- Sort by decreasing duration
- Identify P99 percentile: traces > 2 seconds

**TraceQL query in Tempo**:
```traceql
{service.name="order-service" && duration > 2s} 
| select(spanID, duration, resource.service.name)
```

**2. Flamegraph Analysis**:

Example of problematic trace identified:
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

**Immediate diagnosis**:
- 87% of time in InventoryService
- Unoptimized PostgreSQL query
- No index on the search column

**3. Trace → logs correlation for investigation**:
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

**4. Frequently detected problem patterns**:

**Pattern 1: N+1 Query Problem**
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

**Solution**: Batch loading or JOIN
```java
// Before: N+1 queries
orders.forEach(order -> {
    List<OrderItem> items = itemRepo.findByOrderId(order.getId());
    order.setItems(items);
});

// After: 1 query with JOIN
@Query("SELECT o FROM Order o LEFT JOIN FETCH o.items")
List<Order> findAllWithItems();
```

**Pattern 2: Synchronous Cascading Calls**
```
API Gateway (2.5s total)
├─ UserService.getProfile (0.8s)
└─ Then OrderService.getOrders (0.9s)
    └─ Then PaymentService.getPayments (0.8s)
```

**Solution**: Parallelization
```java
// Before: Sequential (2.5s)
UserProfile profile = userService.getProfile(userId);
List<Order> orders = orderService.getOrders(userId);
List<Payment> payments = paymentService.getPayments(userId);

// After: Parallel (0.9s)
CompletableFuture<UserProfile> profileFuture = 
    CompletableFuture.supplyAsync(() -> userService.getProfile(userId));
CompletableFuture<List<Order>> ordersFuture = 
    CompletableFuture.supplyAsync(() -> orderService.getOrders(userId));
CompletableFuture<List<Payment>> paymentsFuture = 
    CompletableFuture.supplyAsync(() -> paymentService.getPayments(userId));

CompletableFuture.allOf(profileFuture, ordersFuture, paymentsFuture).join();
```

**Pattern 3: Missing Cache**
```
ProductService.getProductDetails (150ms) - appelé 50 fois/seconde
└─ Database query (140ms)
```

**Solution**: Redis Cache
```java
@Cacheable(value = "products", key = "#productId")
public Product getProductDetails(String productId) {
    return productRepository.findById(productId);
}
// Latency reduced: 150ms → 5ms (cache hit)
```

**5. Grafana dashboard for continuous monitoring**:

I created a "Performance Analysis" dashboard with:
- **Panel 1**: P50, P95, P99 latency per service
- **Panel 2**: Trace duration heatmap
- **Panel 3**: Top 10 slowest spans
- **Panel 4**: Duration distribution by operation type (DB, HTTP, Cache)

**PromQL query for alerting**:
```promql
# Alert if P99 latency > 2s for 5 minutes
histogram_quantile(0.99, 
  sum(rate(traces_duration_bucket{service="order-service"}[5m])) by (le)
) > 2
```

**6. Concrete optimization case**:

**Before optimization**:
- Endpoint `/api/orders` : P99 = 3.2s
- Throughput : 50 req/s max
- User complaints: "Application slow"

**After tracing analysis and optimizations**:
1. Added DB index on frequent columns: -70% latency
2. Parallelized 3 sequential calls: -50% latency
3. Redis cache on product data: -80% latency for cache hits
4. Pagination to limit results: -30% latency

**Results**:
- P99 = 0.4s (87% amélioration)
- Throughput: 300 req/s (6x increase)
- Customer satisfaction: +40%

**Key metric**: Tracing ROI = (Infrastructure cost saved) / (Tracing cost)
In this project: ROI = 15x

---

## 🚨 Questions about Alerting

### Q15: What is your strategy to avoid alert fatigue while capturing real problems?

**A:** **Alert fatigue** is a major problem in observability. Here is my complete strategy:

**1. Fundamental principle: The 4 Golden Signals**

I focus alerts on what truly impacts users:
- **Latency**: Is the service fast?
- **Traffic**: How many users are affected?
- **Errors**: Are there failures?
- **Saturation**: Are we close to the limits?

**2. Alert classification by severity**:

```yaml
# alerts.yml - Example Prometheus AlertManager

groups:
  - name: critical_alerts
    interval: 30s
    rules:
      # CRITICAL: Page immediately
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
      # WARNING: Notify without paging
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
      # INFO: Just log, no notification
      - alert: ModerateTraffic
        expr: rate(http_server_requests_seconds_count[5m]) > 100
        for: 10m
        labels:
          severity: info
          pager: false
```

**3. Smart thresholds based on historical data**:

```python
# Script to calculate dynamic thresholds
# Based on P95 of the last 30 days + margin

import numpy as np
from prometheus_api_client import PrometheusConnect

prom = PrometheusConnect(url="http://prometheus:9090")

# Retrieve 30 days of data
latency_data = prom.custom_query_range(
    query='histogram_quantile(0.95, rate(http_server_requests_seconds_bucket[5m]))',
    start_time='30d',
    end_time='now',
    step='1h'
)

# Calculate P95 + 20% margin
p95_latency = np.percentile(latency_data, 95)
alert_threshold = p95_latency * 1.2

print(f"Recommended alert threshold: {alert_threshold}s")
# Output: "Recommended alert threshold: 1.8s"
```

**4. Grace periods and hysteresis to avoid flapping**:

```yaml
# Wait 5 minutes before alerting (avoid blips)
for: 5m

# AlertManager - Grouping and throttling
route:
  group_by: ['alertname', 'service', 'environment']
  group_wait: 30s        # Wait 30s to group
  group_interval: 5m     # Regroup alerts for 5m
  repeat_interval: 4h    # Don't repeat for 4h if unresolved
  
  routes:
    - match:
        severity: critical
      receiver: pagerduty-critical
      repeat_interval: 30m  # Repeat every 30m if not resolved
      
    - match:
        severity: warning
      receiver: slack-warnings
      repeat_interval: 12h
```

**5. Contextual alerting with enrichment**:

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

**6. Intelligent alert routing**:

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
    # Night/Weekend: Only critical
    - match:
        severity: critical
      continue: true
      receiver: pagerduty-critical
      
    # Business hours: Warnings too
    - match:
        severity: warning
      receiver: slack-warnings
      time_intervals:
        - business_hours  # 9am-6pm Mon-Fri
```

**7. My checklist before creating an alert**:

✅ **Does it actually impact users?**
- No → Don't alert, just log

✅ **Is it immediately actionable?**
- No → It's a metric to monitor, not an alert

✅ **Is the threshold based on real data?**
- Avoid arbitrary thresholds (e.g.: CPU > 80%)
- Use history + statistical analysis

✅ **Is there a clear runbook?**
- Each alert must have action documentation

✅ **Can the alert trigger for benign reasons?**
- Yes → Adjust thresholds or grace period

✅ **What is the expected frequency?**
- More than 1x/day → Review thresholds

**8. Metrics to monitor alerting health**:

```promql
# Number of alerts per day
sum(increase(alertmanager_alerts_received_total[24h]))

# False positive rate (alerts resolved without action)
sum(increase(alertmanager_alerts_resolved_total{action="none"}[7d]))
/
sum(increase(alertmanager_alerts_total[7d]))

# Average resolution time
avg(alertmanager_alerts_resolution_duration_seconds)
```

**Result of this approach**:
- **Before**: 200 alerts/day, 85% false positives, exhausted team
- **After**: 8 alerts/day, 95% real alerts, resolution 3x faster
- **Alert fatigue**: Eliminated ✅

---

### Q16: How do you implement log-based alerts in Grafana and ELK?

**A:** **Log-based alerts** complement metric alerts. Here are my two approaches:

**1. Grafana Loki Alerts**

**Alert configuration**:
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

**Advanced LogQL query for pattern matching**:
```logql
# Alert on specific error pattern
{service="payment-service"} 
|= "ERROR" 
|~ "NullPointerException|OutOfMemoryError|SQLException"
| json
| line_format "{{.level}} - {{.message}}"
```

**2. Kibana Alerts (ELK)**

**A. Kibana Alerting Rules**:
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

**B. Elasticsearch Watcher (X-Pack)**:
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

**3. Anomaly-based alerts (Machine Learning)**:

**Kibana ML Job for anomaly detection**:
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

**4. Specific use cases**:

**A. Alert on specific Stack Trace**:
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

**B. Alert on sudden increase rate**:
```logql
# Spike detection (5x the normal rate)
sum(rate({service="order-service"} |= "ERROR" [5m])) 
> 
5 * sum(rate({service="order-service"} |= "ERROR" [1h] offset 1d))
```

**C. Multi-condition alert**:
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

**5. Enriched notification with context**:

**Slack template for Grafana alert**:
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

**6. Alert testing and validation**:

```bash
# Generate test logs
for i in {1..60}; do
  curl -X POST http://order-service/simulate-error
  sleep 1
done

# Verify the alert triggers within 5 minutes
# Check the message in Slack/PagerDuty
# Validate that links work
# Confirm the runbook is up to date
```

**Log-based alerting best practices**:
✅ Always include sample logs in the notification
✅ Limit cardinality (groupby service, not by userId)
✅ Use aggregations rather than raw count when possible
✅ Test alerts in staging before prod
✅ Clearly document thresholds and their origin

---

## 🎯 Situational / Practical Questions

### Q17: A production service suddenly has a 15% error rate. How do you diagnose and fix the problem?

**A:** Here is my complete **war room** methodology for a production incident:

**Phase 1: Detection and Alert (0-2 minutes)**

**1. Alert received**:
```
🔴 CRITICAL ALERT
Service: payment-service
Error Rate: 15.2% (threshold: 5%)
Duration: 8 minutes
Environment: production
Dashboard: https://grafana/d/payment-overview
```

**2. Quick confirmation**:
```bash
# Verify it's not a false positive
# In Grafana, check the metrics
```

**Phase 2: Initial Investigation (2-5 minutes)**

**1. Check the main dashboard**:

**"Service Overview" Dashboard** shows:
- **Traffic**: ✅ Normal (500 req/min)
- **Latency**: ⚠️ P99 = 3.5s (normal: 0.8s)
- **Error Rate**: 🔴 15.2% (normal: 0.5%)
- **Saturation**: ✅ CPU 45%, Memory 60%

**First observation**: High latency correlated with errors → probable timeout or dependent service

**2. Analyze the timeline**:
```promql
# Grafana query to see when it started
rate(http_server_requests_seconds_count{status=~"5..",service="payment-service"}[1m])
```

**Timeline** :
- 14:45 : Tout normal
- 14:47: Start of error increase
- 14:50 : Pic à 15%
- **Incident start**: ~14:47

**3. Check recent deployments**:
```bash
# Check deployment history
kubectl get events --sort-by='.lastTimestamp' -n production

# Result: No deployment in 3 days ✅
# Therefore not caused by new code
```

**Phase 3: Deep Dive with Logs (5-10 minutes)**

**1. Explore error logs in Grafana Loki**:
```logql
{service="payment-service", environment="production"} 
|= "ERROR" 
| json 
| __error__="" 
| line_format "{{.timestamp}} {{.level}} {{.message}}"
```

**Top errors identified**:
```
[14:47:23] ERROR - Payment processing failed: Connection timeout to stripe-api
[14:47:25] ERROR - Payment processing failed: Connection timeout to stripe-api
[14:47:28] ERROR - Payment processing failed: Connection timeout to stripe-api
... (150 occurrences en 5 min)
```

**Clear pattern**: Timeouts to Stripe API (external payment provider)

**2. Refine the analysis**:
```logql
{service="payment-service"} 
|= "stripe-api" 
| json 
| line_format "{{.status}} {{.duration}}ms {{.message}}"
```

**Result** :
```
[14:47:23] TIMEOUT 5000ms Connection timeout
[14:47:25] TIMEOUT 5000ms Connection timeout
[14:48:12] SUCCESS 180ms Payment processed
[14:48:15] TIMEOUT 5000ms Connection timeout
```

**Pattern**: ~15% of requests to Stripe timeout (exactly the observed error rate)

**Phase 4: Correlation with Traces (10-12 minutes)**

**1. Get a traceId from an error request**:
```logql
{service="payment-service"} 
|= "ERROR" 
|= "Connection timeout" 
| json 
| line_format "{{.traceId}}"
| limit 1
```

**TraceId** : `a1b2c3d4e5f6g7h8`

**2. Visualize the trace in Tempo**:
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

**Diagnosis confirmed**: Stripe API is not responding within 5 seconds (our configured timeout)

**Phase 5: External Investigation (12-15 minutes)**

**1. Check Stripe status**:
```bash
# Check Stripe Status Page
curl https://status.stripe.com/api/v2/status.json
```

**Result** :
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

**ROOT CAUSE IDENTIFIED**: Stripe incident (external service) causing partial timeouts

**Phase 6: Immediate Mitigation (15-20 minutes)**

**Action options**:

**Option 1: Increase the timeout** (❌ Bad idea)
- Would increase latency for users
- Does not solve the underlying problem

**Option 2: Circuit Breaker** (✅ Best immediate approach)
```java
// Configuration Resilience4j
@Configuration
public class CircuitBreakerConfig {
    
    @Bean
    public CircuitBreaker stripeCircuitBreaker() {
        CircuitBreakerConfig config = CircuitBreakerConfig.custom()
            .failureRateThreshold(20)           // Open if > 20% failures
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
    
    // Fallback: Queue for later retry
    private Payment fallbackPayment(PaymentRequest request, Exception e) {
        log.warn("Stripe unavailable, queueing payment for retry", e);
        Payment payment = new Payment();
        payment.setStatus(PaymentStatus.PENDING);
        paymentQueue.enqueue(request);
        return paymentRepository.save(payment);
    }
}
```

**Hotfix deployment**:
```bash
# Quick build
./mvnw clean package -DskipTests

# Deploy with rolling update
kubectl set image deployment/payment-service \
  payment-service=payment-service:v2.3.1-hotfix \
  -n production

# Monitor the rollout
kubectl rollout status deployment/payment-service -n production
```

**Option 3: Failover to secondary provider** (✅ Long term)
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

**Phase 7: Resolution Monitoring (20-40 minutes)**

**1. Real-time dashboard**:
```promql
# Error rate after hotfix deployment
rate(http_server_requests_seconds_count{status=~"5..",service="payment-service"}[1m])
```

**Evolution**:
- 14:47 : Error rate = 0.5% (baseline)
- 14:50 : Error rate = 15.2% 🔴 (pic)
- 15:05 : Hotfix déployé
- 15:08: Error rate = 2.1% ⚠️ (improvement)
- 15:12: Error rate = 0.3% ✅ (resolved - circuit breaker active)

**2. Check business metrics**:
```promql
# Successful payments per minute
sum(rate(payments_successful_total[1m])) * 60
```

**Result** : 
- Avant incident : 450 payments/min
- Pendant incident : 380 payments/min (-15%)
- Après mitigation : 448 payments/min (✅ Normal)

**Phase 8: Post-Mortem Documentation (after resolution)**

**Incident Report**:
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

**Phase 9: Future Prevention**

**1. Synthetic Monitoring**:
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

**2. Improved Circuit Breaker Dashboard**:
```promql
# Panels for circuit breaker monitoring
resilience4j_circuitbreaker_state{name="stripe-api"}
resilience4j_circuitbreaker_failure_rate{name="stripe-api"}
resilience4j_circuitbreaker_calls_total{name="stripe-api",kind="successful"}
resilience4j_circuitbreaker_calls_total{name="stripe-api",kind="failed"}
```

**3. Proactive alerts**:
```yaml
# Alert if Stripe responds slowly (before timeout)
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

**Incident Performance Metrics**:
- **MTTD (Mean Time To Detect)** : 3 minutes ✅
- **MTTK (Mean Time To Know)** : 11 minutes ✅
- **MTTR (Mean Time To Resolve)** : 25 minutes ✅
- **MTTF (Mean Time To Fix)** : 18 minutes (déploiement hotfix) ✅

**Conclusion**: Complete observability stack (Logs + Metrics + Traces) enabled fast and efficient resolution.

---

### Q18: How would you configure observability for a new Spring Boot microservices application from scratch?

**A:** Here is my **complete blueprint** to set up observability for a new application:

**Phase 1: Architecture and Decisions (Day 1)**

**1. Requirements analysis**:

**Key questions**:
- Expected volume? (e.g.: 1000 req/s → 2M logs/day)
- Infrastructure budget? (limited → Loki, large → ELK)
- Existing team? (Elastic expertise → ELK)
- Environment? (Kubernetes → native Grafana Stack)
- Compliance? (long retention → ELK)

**For my example**: SaaS startup, Kubernetes, moderate budget, 5000 req/s
→ **Decision: Grafana Stack (Loki + Prometheus + Tempo)**

**2. Chosen tech stack**:
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

**Phase 2: Spring Boot Configuration (Day 1-2)**

**1. Dependencies (pom.xml)**:
```xml
<properties>
    <spring-cloud.version>2023.0.0</spring-cloud.version>
</properties>

<dependencies>
    <!-- Actuator for metrics -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-actuator</artifactId>
    </dependency>
    
    <!-- Micrometer for Prometheus -->
    <dependency>
        <groupId>io.micrometer</groupId>
        <artifactId>micrometer-registry-prometheus</artifactId>
    </dependency>
    
    <!-- Tracing with OpenTelemetry -->
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

**2. application.yml configuration**:
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

**3. Logback configuration (logback-spring.xml)**:
```xml
<?xml version="1.0" encoding="UTF-8"?>
<configuration>
    <include resource="org/springframework/boot/logging/logback/defaults.xml"/>
    
    <springProperty scope="context" name="appName" source="spring.application.name"/>
    <springProperty scope="context" name="environment" source="ENVIRONMENT" defaultValue="dev"/>
    
    <!-- Console Appender with JSON format -->
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
    
    <!-- File Appender for backup -->
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

**4. Custom Metrics Configuration**:
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

**Phase 3: Kubernetes Infrastructure (Day 2-3)**

**1. Deployment with annotations**:
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
      
      # Promtail sidecar to collect logs
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

**2. Promtail ConfigMap**:
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

**3. Prometheus ServiceMonitor**:
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

**Phase 4: Grafana Configuration (Day 3-4)**

**1. Data Sources**:
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

**2. "Service Overview" Dashboard** (exported as JSON):
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

**Phase 5: Alerting (Day 4-5)**

**1. Alert Rules**:
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

**2. Notification Channels**:
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

**Phase 6: Documentation and Runbooks (Day 5)**

**1. Observability README**:
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

**Phase 7: Testing (Day 5-6)**

**1. Load Testing with observability**:
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

**During the test, verify**:
```bash
# Grafana dashboard: Request rate rises to ~100 req/s
# Loki logs: Watch logs stream in real time
# Tempo traces: Sample of captured traces
# Alerts: No alert should trigger if everything is fine
```

**2. Chaos Engineering Tests**:
```bash
# Test resilience with Chaos Mesh

# Simulate network latency
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

# Watch in Grafana:
# - P99 latency increases
# - Alerts trigger if configured
# - Traces show the added latency
```

**3. Logs-Metrics-Traces correlation validation**:
```bash
# Generate a specific request
curl -X POST http://order-service/api/orders \
  -H "Content-Type: application/json" \
  -d '{"userId":"test-user","items":[{"productId":"test-prod","quantity":1}]}' \
  -v

# Retrieve the traceId from the response
# Example: traceId=a1b2c3d4e5f6g7h8

# In Grafana:
# 1. Find the trace in Tempo with this traceId
# 2. Click "View Logs" → should show logs with this traceId
# 3. Verify that metrics for this request appear in Prometheus
```

**Phase 8: Optimization and Tuning (Day 6-7)**

**1. Sampling rate adjustment**:

After observing real traffic:
```yaml
# Initial config (too conservative)
spring:
  sleuth:
    sampler:
      probability: 0.1  # 10%

# After analysis, adjust:
# - Dev: 100% (easy debug)
# - Staging: 50% (good balance)
# - Production: 5% base + adaptive rules (optimal cost)
```

**2. Loki index optimization**:
```yaml
# Adjust labels for optimal cardinality
scrape_configs:
  - job_name: spring-boot-logs
    pipeline_stages:
      - labels:
          # Low cardinality labels (GOOD)
          application:
          environment:
          level:
          # NO high cardinality labels (BAD)
          # userId:  ❌
          # requestId: ❌
          # orderId: ❌
```

**3. Prometheus retention tuning**:
```yaml
# prometheus.yml
global:
  scrape_interval: 15s
  evaluation_interval: 15s

# Retention based on volume
storage:
  tsdb:
    retention.time: 15d  # Données locales
    
# Thanos remote storage for long-term
remote_write:
  - url: http://thanos-receive:19291/api/v1/receive
    queue_config:
      capacity: 10000
      max_samples_per_send: 5000
```

**4. Dashboard Performance Optimization**:
```json
{
  "panels": [
    {
      "title": "Request Rate",
      "targets": [{
        // BEFORE (slow)
        "expr": "sum(rate(http_server_requests_seconds_count[5m]))"
        
        // AFTER (fast) - pre-aggregated recording rules
        "expr": "job:http_requests:rate5m"
      }]
    }
  ]
}
```

**Prometheus Recording Rules**:
```yaml
# prometheus-rules.yml
groups:
  - name: http_metrics
    interval: 30s
    rules:
      # Pre-aggregate for dashboard performance
      - record: job:http_requests:rate5m
        expr: sum(rate(http_server_requests_seconds_count[5m])) by (job, application)
        
      - record: job:http_requests_errors:rate5m
        expr: sum(rate(http_server_requests_seconds_count{status=~"5.."}[5m])) by (job, application)
        
      - record: job:http_requests_latency:p99
        expr: histogram_quantile(0.99, sum(rate(http_server_requests_seconds_bucket[5m])) by (le, job))
```

**Phase 9: Training and Documentation (Day 7)**

**1. Quick guide for developers**:
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

**2. Runbook Template**:
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

**Phase 10: Production Monitoring (Day 7 - Ongoing)**

**1. Health Check Dashboard**:
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

**2. Weekly Review Metrics**:
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

**3. Continuous Improvement**:
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

**Phase 11: Final Checklist (Acceptance Criteria)**

**✅ Configuration Completeness**:
- [x] Structured logging with JSON format
- [x] Logs include traceId/spanId
- [x] MDC configured for business context
- [x] Rotation and retention configured
- [x] Micrometer + Actuator enabled
- [x] Custom metrics added
- [x] Prometheus scraping configured
- [x] Percentiles configured (P50, P95, P99)
- [x] OpenTelemetry SDK integrated
- [x] Trace context propagation tested
- [x] Sampling strategy configured
- [x] Custom spans for critical operations

**✅ Infrastructure**:
- [x] Promtail deployed and working
- [x] Loki storage configured with retention
- [x] Prometheus scraping all services
- [x] Tempo receiving traces
- [x] Grafana datasources configured
- [x] K8s service discovery working

**✅ Dashboards & Visualization**:
- [x] "Service Overview" dashboard created
- [x] "JVM Metrics" dashboard created
- [x] "Business Metrics" dashboard created
- [x] "Observability Health" dashboard created
- [x] Dashboard variables configured (service, env)
- [x] Links between logs/metrics/traces tested

**✅ Alerting**:
- [x] Critical alerts configured (service down, high error rate)
- [x] Warning alerts configured (high latency, high memory)
- [x] Notification channels configured (Slack, Email)
- [x] Grace periods and throttling configured
- [x] Alerts tested with chaos engineering

**✅ Documentation**:
- [x] Observability README created
- [x] Runbooks for common incidents
- [x] Developer guide for custom metrics
- [x] Architecture diagram updated
- [x] Onboarding document for new developers

**✅ Testing & Validation**:
- [x] Load testing with observability enabled
- [x] Chaos engineering scenarios tested
- [x] Logs-metrics-traces correlation validated
- [x] Performance dashboards under load
- [x] Alerts triggered correctly

**Final Result**:
- **Setup Time**: 7 days (1 week)
- **Services Monitored**: 100%
- **Alert Coverage**: 95% of critical scenarios
- **MTTD Target**: < 5 minutes
- **MTTR Target**: < 30 minutes
- **Team Trained**: 100%
- **Documentation Complete**: ✅
- **Production Ready**: ✅

---

### Q19: How do you manage observability in a multi-tenant or multi-environment context?

**A:** Managing observability in **multi-tenant** and **multi-environment** requires a specific architecture and strategy:

**1. Multi-Environment Architecture**

**Separation strategy**:

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

**A. Labeling Strategy**:
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

**B. Multi-Env Promtail Configuration**:
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
          
      # Filter logs based on environment
      - match:
          selector: '{level="DEBUG"}'
          action: drop
          # Drop DEBUG logs in production
          drop_counter_reason: debug_in_prod
```

**C. Multi-Environment Prometheus Federation**:
```yaml
# Central Prometheus for all environments
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
    scrape_interval: 15s  # More frequent for prod
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

**2. Multi-Tenant Architecture**

**A. Tenant Isolation Strategy**:

**Option 1: Logical Separation (Shared Infrastructure)**:
```yaml
# Good for: SaaS with many small tenants
# Prometheus - All tenants in the same instance
# Loki - All logs with tenant label
# Grafana - RBAC to isolate views

# Application code
@Service
public class TenantContextFilter implements Filter {
    @Override
    public void doFilter(ServletRequest request, ServletResponse response, FilterChain chain) {
        String tenantId = extractTenantId(request);
        
        // Inject into MDC for logs
        MDC.put("tenant", tenantId);
        
        // Inject into metrics
        Metrics.globalRegistry.config().commonTags("tenant", tenantId);
        
        // Inject into traces
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

**Loki query by tenant**:
```logql
# Logs of a specific tenant
{application="order-service", tenant="acme-corp"} | json

# Logs of all premium tenants
{application="order-service", tier="premium"} | json

# Aggregation by tenant
sum by (tenant) (count_over_time({application="order-service"} [1h]))
```

**Prometheus query by tenant**:
```promql
# Request rate by tenant
sum(rate(http_server_requests_seconds_count{tenant="acme-corp"}[5m]))

# Compare all tenants
sum by (tenant) (rate(http_server_requests_seconds_count[5m]))

# Top 10 tenants by volume
topk(10, sum by (tenant) (rate(http_server_requests_seconds_count[5m])))
```

**Option 2: Physical Separation (Dedicated Infrastructure)**:
```yaml
# Good for: Enterprise tenants with strict compliance

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

**B. Grafana RBAC for Multi-Tenant**:
```yaml
# Grafana datasource with tenant variable
apiVersion: 1
datasources:
  - name: Loki-Tenant-A
    type: loki
    access: proxy
    url: http://loki:3100
    jsonData:
      # Automatic filter for this datasource
      derivedFields:
        - name: tenant
          value: tenant-a
      httpHeaderName1: 'X-Scope-OrgID'
    secureJsonData:
      httpHeaderValue1: 'tenant-a'
```

**Grafana Teams & Permissions**:
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

**C. Cost Allocation per Tenant**:

**Cost tracking in Prometheus**:
```promql
# Data volume by tenant (logs)
sum by (tenant) (promtail_sent_bytes_total)

# Number of metrics by tenant
count by (tenant) ({__name__=~".+"})

# Number of traces by tenant
sum by (tenant) (rate(tempo_ingester_traces_created_total[1h]))

# "Cost per Tenant" Dashboard
```

**Alert on excessive usage**:
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

**3. Multi-Environment/Tenant Dashboard**

**Dashboard with template variables**:
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

**4. Multi-Tenant/Env Best Practices**

**✅ DO**:
- Use consistent labels (environment, tenant) everywhere
- Implement strict RBAC in Grafana
- Monitor quotas and costs per tenant
- Physically separate enterprise tenants if compliance required
- Automate onboarding of new tenants
- Clearly document limits per tier (free/standard/premium)
- Implement rate limiting per tenant in the application
- Regularly audit data access

**❌ DON'T**:
- Don't use high cardinality tenant identifiers as Prometheus labels
- Don't expose one tenant's data to another (even accidentally)
- Don't forget to clean up data from disabled tenants
- Don't configure global alerts without filtering by environment
- Don't mix prod and non-prod data in the same storage

**5. Multi-Tenant Automation**

**Automatic onboarding script**:
```bash
#!/bin/bash
# onboard-tenant.sh

TENANT_ID=$1
TENANT_NAME=$2
TIER=$3  # free, standard, premium

echo "Onboarding tenant: $TENANT_NAME ($TENANT_ID)"

# 1. Create Kubernetes namespace for physical isolation
kubectl create namespace tenant-${TENANT_ID}

# 2. Create Grafana Team
curl -X POST http://grafana/api/teams \
  -H "Authorization: Bearer ${GRAFANA_TOKEN}" \
  -d "{\"name\":\"${TENANT_NAME}\",\"email\":\"ops@${TENANT_ID}.com\"}"

TEAM_ID=$(curl -s http://grafana/api/teams/search?name=${TENANT_NAME} \
  -H "Authorization: Bearer ${GRAFANA_TOKEN}" | jq -r '.teams[0].id')

# 3. Create Grafana Folder
curl -X POST http://grafana/api/folders \
  -H "Authorization: Bearer ${GRAFANA_TOKEN}" \
  -d "{\"title\":\"${TENANT_NAME} Dashboards\"}"

FOLDER_UID=$(curl -s http://grafana/api/folders \
  -H "Authorization: Bearer ${GRAFANA_TOKEN}" | jq -r ".[] | select(.title==\"${TENANT_NAME} Dashboards\") | .uid")

# 4. Grant permissions
curl -X POST http://grafana/api/folders/${FOLDER_UID}/permissions \
  -H "Authorization: Bearer ${GRAFANA_TOKEN}" \
  -d "{\"items\":[{\"teamId\":${TEAM_ID},\"permission\":2}]}"  # 2=Edit

# 5. Create dashboard from template
DASHBOARD_JSON=$(cat dashboard-template.json | \
  sed "s/TENANT_ID/${TENANT_ID}/g" | \
  sed "s/TENANT_NAME/${TENANT_NAME}/g")

curl -X POST http://grafana/api/dashboards/db \
  -H "Authorization: Bearer ${GRAFANA_TOKEN}" \
  -d "{\"dashboard\":${DASHBOARD_JSON},\"folderId\":${FOLDER_UID}}"

# 6. Configure quotas based on tier
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

# 7. Create ConfigMap with quotas
kubectl create configmap tenant-${TENANT_ID}-config \
  --from-literal=log_quota=${LOG_QUOTA} \
  --from-literal=metric_quota=${METRIC_QUOTA} \
  --from-literal=trace_quota=${TRACE_QUOTA} \
  -n tenant-${TENANT_ID}

# 8. Configure tenant-specific alerts
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

**6. Multi-Tenant Secret Management**

**Vault integration for separate credentials**:
```yaml
# vault-config.yml
# Each tenant has its own path in Vault

path "secret/data/tenants/tenant-a/*" {
  capabilities = ["read", "list"]
}

path "secret/data/tenants/tenant-b/*" {
  capabilities = ["read", "list"]
}
```

**Application configuration**:
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
            
            // Retrieve credentials from Vault
            VaultResponse response = vaultTemplate.read(
                "secret/data/tenants/" + tenantId + "/database"
            );
            
            // Configure datasource with tenant credentials
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

**7. Monitoring the Observability Stack Itself**

**Meta-monitoring**:
```yaml
# prometheus-meta.yml
# Monitor the health of the observability stack

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

**"Observability Platform Health" Dashboard**:
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

### Q20: Give an example of a complex incident you resolved using observability and explain your approach

**A:** Here is a **complex real case** I resolved:

**Context**:
- E-commerce application with microservices architecture
- 15 Spring Boot microservices
- 50,000 active users
- Black Friday critical period

**Incident**: "Mystery Performance Degradation"

**Initial symptoms**:
- ⚠️ Users report intermittent slowness (not systematic)
- ⚠️ Some orders take 10-15 seconds instead of 2-3 seconds
- ⚠️ No visible increase in error rate
- ⚠️ Random problem: 1 order out of 20 affected

**Phase 1: Detection (Minute 0-5)**

**Alert received**:
```
⚠️ WARNING: High P99 Latency
Service: order-service
P99 Latency: 12.5s (normal: 2.1s)
Affected: ~5% of requests
Time: 09:45 AM
```

**Initial Grafana dashboard**:
```promql
# P99 latency shows spikes
histogram_quantile(0.99, rate(http_server_requests_seconds_bucket{application="order-service"}[5m]))

# Result: Sawtooth pattern
# 09:30 - 2.1s ✅
# 09:35 - 11.2s 🔴
# 09:40 - 2.3s ✅
# 09:45 - 13.1s 🔴
```

**Key observation**: Cyclic pattern every 5-10 minutes

**Phase 2: Initial Investigation (Minute 5-15)**

**1. Standard check reveals nothing abnormal**:
```bash
# CPU, Memory, Network normal
kubectl top pods -n production
# Result: All resources within normal limits

# No recent deployment
kubectl rollout history deployment/order-service
# Last deployment: 3 days ago

# No obvious external issues
curl https://status.stripe.com/api/v2/status.json
# Result: All systems operational
```

**2. Loki log analysis**:
```logql
{application="order-service", level="ERROR"} | json

# Result: Some interesting lines
[09:35:12] ERROR - HikariPool connection timeout after 10000ms
[09:35:14] ERROR - HikariPool connection timeout after 10000ms
[09:44:23] ERROR - HikariPool connection timeout after 10000ms
```

**First lead**: Connection pool database periodically exhausted

**Phase 3: Deep Dive with Traces (Minute 15-25)**

**1. Get a traceId from a slow request**:
```logql
{application="order-service"} 
| json 
| duration > 10000
| line_format "{{.traceId}} {{.duration}}ms"
| limit 1

# Result : traceId = xyz789abc
```

**2. Visualize the trace in Tempo**:
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
- 10 seconds spent **waiting for a connection** to inventory-service
- Not a performance problem with inventory-service itself
- Connection pool or circuit breaker problem

**3. Investigate inventory-service**:

**Actuator metrics**:
```bash
curl http://inventory-service:8080/actuator/metrics/hikaricp.connections.active

# Result during slow period:
{
  "name": "hikaricp.connections.active",
  "measurements": [{
    "value": 20.0  # Maximum atteint!
  }]
}

curl http://inventory-service:8080/actuator/metrics/hikaricp.connections.max
# Max pool size: 20
```

**Database pool completely saturated!**

**4. Investigate DB queries**:

**Inventory-service logs**:
```logql
{application="inventory-service"} 
| json 
| line_format "{{.sql_query}} {{.duration}}ms"
| duration > 1000

# Repetitive patterns every 5 minutes:
[09:35:00] SELECT * FROM inventory_sync_log WHERE status='PENDING' - 8500ms 🔴
[09:40:00] SELECT * FROM inventory_sync_log WHERE status='PENDING' - 9200ms 🔴
[09:45:00] SELECT * FROM inventory_sync_log WHERE status='PENDING' - 8800ms 🔴
```

**ROOT CAUSE identifiée** : 
- Sync job scheduled every 5 minutes
- Unoptimized query that locks the table for 8-10 seconds
- During this time, all pool connections are blocked
- New API requests wait until pool is exhausted

**Phase 4: Confirmation with full correlation (Minute 25-30)**

**Overlay of three signals**:

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

**Combined Grafana dashboard**:
```promql
# Panel 1: Latency P99
histogram_quantile(0.99, rate(http_server_requests_seconds_bucket[1m]))

# Panel 2: DB connection pool usage
hikaricp_connections_active / hikaricp_connections_max * 100

# Panel 3: Sync job executions
rate(inventory_sync_job_executions_total[1m])

# Perfect correlation visible!
```

**Phase 5: Immediate Mitigation (Minute 30-45)**

**Temporary solution (hotfix)**:
```java
// Before (problematic)
@Scheduled(fixedRate = 300000)  // Every 5 minutes
public void syncInventory() {
    List<InventoryLog> pending = inventoryLogRepository
        .findByStatus("PENDING");  // Full table scan, no index!
    // Process...
}

// After (immediate hotfix)
@Scheduled(fixedRate = 300000)
@Transactional(timeout = 5)  // Timeout pour éviter long locks
public void syncInventory() {
    // Pagination to avoid large resultsets
    Pageable pageable = PageRequest.of(0, 100);
    Page<InventoryLog> pending = inventoryLogRepository
        .findByStatusWithLimit("PENDING", pageable);
    // Process...
}
```

**Temporary pool size increase configuration**:
```yaml
# application.yml hotfix
spring:
  datasource:
    hikari:
      maximum-pool-size: 40  # Increased from 20 → 40
      connection-timeout: 5000  # Reduced from 10s → 5s for fail-fast
```

**Deployment**:
```bash
# Build and deploy hotfix
./mvnw clean package -DskipTests
kubectl set image deployment/inventory-service \
  inventory-service=inventory-service:v1.5.2-hotfix

# Monitor the rollout
kubectl rollout status deployment/inventory-service
```

**Immediate result**:
- P99 latency: 12.5s → 2.3s ✅
- Connection pool never saturated
- Incident mitigated in 45 minutes

**Phase 6: Long-term solution (Post-incident)**

**1. DB query optimization**:
```sql
-- Before: Full table scan
SELECT * FROM inventory_sync_log WHERE status = 'PENDING';
-- Execution time: 8500ms
-- Rows scanned: 5,000,000

-- Create index
CREATE INDEX idx_inventory_sync_log_status_created 
ON inventory_sync_log(status, created_at);

-- After: Index scan
SELECT * FROM inventory_sync_log 
WHERE status = 'PENDING' 
ORDER BY created_at 
LIMIT 100;
-- Execution time: 45ms ✅
-- Rows scanned: 100
```

**2. Move the job to a dedicated worker**:
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

**3. Improve sync job observability**:
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

**4. Add proactive alerts**:
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

**Phase 7: Post-Mortem and Learnings**

**Post-Mortem Document**:
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

**Conclusion of this case**:
Complete observability (Logs + Metrics + Traces + Alerting) enabled:
- ✅ Fast detection (15 min instead of potentially hours)
- ✅ Precise diagnosis with three-dimensional correlation
- ✅ Fast resolution (45 min total)
- ✅ Learning to prevent similar future incidents

**Without observability**, this incident would likely have:
- ❌ Lasted 4-6 hours (blind debugging)
- ❌ Required random restarts (trial & error)
- ❌ Impacted many more users
- ❌ Caused significant revenue loss

---

**That's it! This complete summary covers all aspects of observability for a Spring Boot developer role. These Q&As demonstrate deep and practical expertise in the three pillars (logs, metrics, traces) as well as skills in troubleshooting, architecture, and best practices.**

**Key points to remember for the interview**:
- ✅ Technical mastery of both ecosystems (ELK & Grafana)
- ✅ Practical experience with real cases
- ✅ Understand the "why" not just the "how"
- ✅ Focus on business value and ROI
- ✅ Methodical debugging approach