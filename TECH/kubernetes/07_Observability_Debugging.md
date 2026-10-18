## [**..**](./00_index.md)


# Chapitre 7 : Observability & Debugging

## 7.1. Logging (kubectl logs, Aggregation)

### Qu'est-ce que le Logging dans Kubernetes ?

Le **logging** permet de collecter et consulter les logs des applications tournant dans les Pods pour :
- Debugging des erreurs
- Audit des actions
- Monitoring du comportement applicatif

**Architecture de logging :**
```
Application → stdout/stderr → Container Runtime → kubectl logs
                                    ↓
                            Log Aggregation System
                         (ELK, Fluentd, Loki, etc.)
```

---

### kubectl logs - Commandes de base

**Voir les logs d'un Pod :**
```bash
# Logs du Pod (un seul container)
kubectl logs my-pod

# Logs d'un container spécifique (multi-container Pod)
kubectl logs my-pod -c my-container

# Suivre les logs en temps réel (-f = follow)
kubectl logs -f my-pod

# Dernières 100 lignes
kubectl logs --tail=100 my-pod

# Logs depuis les 5 dernières minutes
kubectl logs --since=5m my-pod

# Logs avec timestamps
kubectl logs --timestamps my-pod

# Logs du Pod précédent (après crash/restart)
kubectl logs my-pod --previous
```

---

### Multi-container Pods

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: multi-container-app
spec:
  containers:
  - name: app
    image: myapp:1.0
    # App logs sur stdout
  
  - name: sidecar-logger
    image: fluent/fluentd:v1.14
    # Collecte et forwarde les logs
```

**Consulter les logs :**
```bash
# Container "app"
kubectl logs multi-container-app -c app

# Container "sidecar-logger"
kubectl logs multi-container-app -c sidecar-logger

# Tous les containers en même temps
kubectl logs multi-container-app --all-containers=true

# Suivre les logs de tous les containers
kubectl logs -f multi-container-app --all-containers=true
```

---

### Logs d'un Deployment

```bash
# Logs de tous les Pods d'un Deployment
kubectl logs deployment/my-app

# Suivre les logs
kubectl logs -f deployment/my-app

# Logs d'un seul replica
kubectl logs deployment/my-app --max-log-requests=1

# Logs avec selector
kubectl logs -l app=my-app
```

---

### Best Practices - Application Logging

**✅ Toujours logger sur stdout/stderr**

**❌ Mauvais (logs dans fichiers) :**
```java
@SpringBootApplication
public class Application {
    private static final Logger logger = LoggerFactory.getLogger(Application.class);
    
    public static void main(String[] args) {
        // Logs dans /var/log/app.log
        // ❌ Pas accessible via kubectl logs
        SpringApplication.run(Application.class, args);
    }
}
```

**✅ Bon (logs sur stdout) :**
```java
@SpringBootApplication
public class Application {
    private static final Logger logger = LoggerFactory.getLogger(Application.class);
    
    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }
}

@RestController
public class OrderController {
    private static final Logger logger = LoggerFactory.getLogger(OrderController.class);
    
    @PostMapping("/orders")
    public Order createOrder(@RequestBody Order order) {
        logger.info("Creating order: id={}, amount={}", order.getId(), order.getAmount());
        // Logs automatiquement sur stdout
        return orderService.create(order);
    }
}
```

**application.yml (Spring Boot) :**
```yaml
logging:
  level:
    root: INFO
    com.example.myapp: DEBUG
  pattern:
    console: "%d{yyyy-MM-dd HH:mm:ss} - %msg%n"
  # ✅ Logs sur console (stdout)
```

---

### Structured Logging (JSON)

**Pourquoi JSON ?**
- Parsable par log aggregators (Elasticsearch, Loki)
- Facilite le filtering et searching
- Meilleure intégration avec observability tools

**Spring Boot avec Logback JSON :**

**pom.xml :**
```xml
<dependency>
    <groupId>net.logstash.logback</groupId>
    <artifactId>logstash-logback-encoder</artifactId>
    <version>7.3</version>
</dependency>
```

**logback-spring.xml :**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<configuration>
    <appender name="JSON" class="ch.qos.logback.core.ConsoleAppender">
        <encoder class="net.logstash.logback.encoder.LogstashEncoder">
            <includeMdcKeyName>traceId</includeMdcKeyName>
            <includeMdcKeyName>userId</includeMdcKeyName>
        </encoder>
    </appender>
    
    <root level="INFO">
        <appender-ref ref="JSON"/>
    </root>
</configuration>
```

**Java Code :**
```java
@RestController
public class OrderController {
    private static final Logger logger = LoggerFactory.getLogger(OrderController.class);
    
    @PostMapping("/orders")
    public Order createOrder(@RequestBody Order order) {
        MDC.put("orderId", order.getId());
        MDC.put("userId", order.getUserId());
        
        logger.info("Order created successfully");
        
        MDC.clear();
        return order;
    }
}
```

**Output JSON :**
```json
{
  "@timestamp": "2024-11-18T10:30:45.123Z",
  "level": "INFO",
  "logger_name": "com.example.OrderController",
  "message": "Order created successfully",
  "thread_name": "http-nio-8080-exec-1",
  "orderId": "ORD-12345",
  "userId": "user-789"
}
```

---

### Log Aggregation - ELK Stack

**Architecture :**
```
Pods (stdout) → Fluentd/Fluent Bit → Elasticsearch → Kibana
```

**Fluentd DaemonSet :**
```yaml
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: fluentd
  namespace: kube-system
spec:
  selector:
    matchLabels:
      app: fluentd
  template:
    metadata:
      labels:
        app: fluentd
    spec:
      serviceAccountName: fluentd
      containers:
      - name: fluentd
        image: fluent/fluentd-kubernetes-daemonset:v1-debian-elasticsearch
        env:
        - name: FLUENT_ELASTICSEARCH_HOST
          value: "elasticsearch.logging.svc.cluster.local"
        - name: FLUENT_ELASTICSEARCH_PORT
          value: "9200"
        volumeMounts:
        - name: varlog
          mountPath: /var/log
        - name: varlibdockercontainers
          mountPath: /var/lib/docker/containers
          readOnly: true
      volumes:
      - name: varlog
        hostPath:
          path: /var/log
      - name: varlibdockercontainers
        hostPath:
          path: /var/lib/docker/containers
```

---

### Log Aggregation - Loki (Grafana)

**Architecture :**
```
Pods → Promtail (DaemonSet) → Loki → Grafana
```

**Avantages Loki vs Elasticsearch :**
- ✅ Plus léger (moins de ressources)
- ✅ Pas d'indexation complète (stockage moins cher)
- ✅ Intégration native avec Grafana
- ✅ Label-based (comme Prometheus)

**Promtail DaemonSet :**
```yaml
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: promtail
  namespace: logging
spec:
  selector:
    matchLabels:
      app: promtail
  template:
    metadata:
      labels:
        app: promtail
    spec:
      serviceAccountName: promtail
      containers:
      - name: promtail
        image: grafana/promtail:2.9.0
        args:
        - -config.file=/etc/promtail/promtail.yaml
        volumeMounts:
        - name: config
          mountPath: /etc/promtail
        - name: varlog
          mountPath: /var/log
        - name: pods
          mountPath: /var/lib/docker/containers
          readOnly: true
      volumes:
      - name: config
        configMap:
          name: promtail-config
      - name: varlog
        hostPath:
          path: /var/log
      - name: pods
        hostPath:
          path: /var/lib/docker/containers
```

**Promtail ConfigMap :**
```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: promtail-config
  namespace: logging
data:
  promtail.yaml: |
    server:
      http_listen_port: 9080
    
    clients:
    - url: http://loki:3100/loki/api/v1/push
    
    scrape_configs:
    - job_name: kubernetes-pods
      kubernetes_sd_configs:
      - role: pod
      
      relabel_configs:
      # Extraire namespace
      - source_labels: [__meta_kubernetes_namespace]
        target_label: namespace
      
      # Extraire pod name
      - source_labels: [__meta_kubernetes_pod_name]
        target_label: pod
      
      # Extraire container name
      - source_labels: [__meta_kubernetes_pod_container_name]
        target_label: container
      
      # Extraire app label
      - source_labels: [__meta_kubernetes_pod_label_app]
        target_label: app
```

**Query dans Grafana :**
```promql
# Tous les logs de l'app "my-app"
{app="my-app"}

# Logs d'erreur
{app="my-app"} |= "ERROR"

# Logs contenant "order"
{namespace="prod", app="my-app"} |= "order"

# Logs JSON - filter par field
{app="my-app"} | json | orderId="ORD-12345"

# Rate de logs d'erreur
rate({app="my-app"} |= "ERROR" [5m])
```

---

### Retention & Rotation

**⚠️ Problème : Logs remplissent le disque**

**Solution 1 : Log rotation au niveau Container Runtime**
```yaml
# Docker daemon.json
{
  "log-driver": "json-file",
  "log-opts": {
    "max-size": "10m",
    "max-file": "3"
  }
}
```

**Solution 2 : Kubernetes max-log-size**
```yaml
# kubelet config
containerLogMaxSize: 10Mi
containerLogMaxFiles: 5
```

**Solution 3 : Log aggregation + retention policy**
```yaml
# Loki retention (exemple)
limits_config:
  retention_period: 744h  # 31 jours

# Elasticsearch ILM
# - Hot tier : 7 jours
# - Warm tier : 30 jours
# - Delete : après 90 jours
```

---

## 7.2. Monitoring (Metrics Server, Probes)

### Metrics Server

**Qu'est-ce que Metrics Server ?**

Le **Metrics Server** collecte les métriques de ressources (CPU, Memory) des Nodes et Pods.

**Utilisation :**
- `kubectl top`
- Horizontal Pod Autoscaler (HPA)
- Vertical Pod Autoscaler (VPA)

---

### Installation Metrics Server

```bash
# Installation
kubectl apply -f https://github.com/kubernetes-sigs/metrics-server/releases/latest/download/components.yaml

# Vérifier
kubectl get deployment metrics-server -n kube-system

# Attendre que le Pod soit Ready
kubectl get pods -n kube-system -l k8s-app=metrics-server
```

---

### kubectl top

**Nodes :**
```bash
# Utilisation CPU/Memory des Nodes
kubectl top nodes

# Output:
# NAME       CPU(cores)   CPU%   MEMORY(bytes)   MEMORY%
# node-1     2500m        25%    16Gi            50%
# node-2     1800m        18%    12Gi            37%
```

**Pods :**
```bash
# Utilisation des Pods
kubectl top pods

# Dans un namespace spécifique
kubectl top pods -n prod

# Trier par CPU
kubectl top pods --sort-by=cpu

# Trier par Memory
kubectl top pods --sort-by=memory

# Avec containers
kubectl top pods --containers
```

**Output exemple :**
```
NAME                    CPU(cores)   MEMORY(bytes)
my-app-7d4f5c6b9-abc123  250m        512Mi
my-app-7d4f5c6b9-def456  300m        600Mi
postgres-0               150m        2Gi
```

---

### Prometheus & Grafana (Production Monitoring)

**Architecture :**
```
Application (Metrics Endpoint)
    ↓
Prometheus (Scraping)
    ↓
Grafana (Visualization)
```

---

### Spring Boot Actuator + Micrometer

**Exposer les métriques pour Prometheus :**

**pom.xml :**
```xml
<!-- Spring Boot Actuator -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-actuator</artifactId>
</dependency>

<!-- Micrometer Prometheus -->
<dependency>
    <groupId>io.micrometer</groupId>
    <artifactId>micrometer-registry-prometheus</artifactId>
</dependency>
```

**application.yml :**
```yaml
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
```

**Endpoints disponibles :**
```bash
# Health check
curl http://localhost:8080/actuator/health

# Métriques Prometheus format
curl http://localhost:8080/actuator/prometheus

# Métriques détaillées
curl http://localhost:8080/actuator/metrics
curl http://localhost:8080/actuator/metrics/jvm.memory.used
```

---

### Métriques custom avec Micrometer

```java
@Service
public class OrderService {
    private final MeterRegistry meterRegistry;
    private final Counter orderCreatedCounter;
    private final Timer orderProcessingTimer;
    
    public OrderService(MeterRegistry meterRegistry) {
        this.meterRegistry = meterRegistry;
        
        // Counter : nombre de commandes créées
        this.orderCreatedCounter = Counter.builder("orders.created")
            .description("Total orders created")
            .tag("status", "success")
            .register(meterRegistry);
        
        // Timer : temps de traitement
        this.orderProcessingTimer = Timer.builder("orders.processing.time")
            .description("Order processing time")
            .register(meterRegistry);
    }
    
    public Order createOrder(Order order) {
        return orderProcessingTimer.record(() -> {
            // Business logic
            Order created = orderRepository.save(order);
            
            // Incrémenter le counter
            orderCreatedCounter.increment();
            
            // Gauge : valeur actuelle
            meterRegistry.gauge("orders.pending", 
                orderRepository.countByStatus("PENDING"));
            
            return created;
        });
    }
}
```

**Métriques exposées :**
```prometheus
# Counter
orders_created_total{status="success"} 1234

# Timer
orders_processing_time_seconds_sum 45.6
orders_processing_time_seconds_count 1234
orders_processing_time_seconds_max 2.5

# Gauge
orders_pending 42
```

---

### ServiceMonitor (Prometheus Operator)

```yaml
apiVersion: v1
kind: Service
metadata:
  name: my-app
  labels:
    app: my-app
spec:
  selector:
    app: my-app
  ports:
  - name: http
    port: 8080
  - name: metrics  # ← Port pour Prometheus
    port: 8080
---
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: my-app-monitor
  labels:
    app: my-app
spec:
  selector:
    matchLabels:
      app: my-app
  endpoints:
  - port: metrics
    path: /actuator/prometheus
    interval: 30s  # Scrape toutes les 30 secondes
```

---

### Prometheus Queries (PromQL)

```promql
# CPU usage d'un Pod
container_cpu_usage_seconds_total{pod="my-app-xxx"}

# Memory usage
container_memory_usage_bytes{pod="my-app-xxx"}

# Request rate (last 5 minutes)
rate(http_server_requests_seconds_count{app="my-app"}[5m])

# P95 latency
histogram_quantile(0.95, 
  rate(http_server_requests_seconds_bucket{app="my-app"}[5m])
)

# Error rate
rate(http_server_requests_seconds_count{app="my-app",status="500"}[5m])

# JVM Memory used
jvm_memory_used_bytes{app="my-app", area="heap"}

# Custom metric
rate(orders_created_total[5m])
```

---

### Grafana Dashboard

**Dashboard pour Spring Boot App :**

**Panels clés :**
```yaml
1. Request Rate (QPS)
   Query: rate(http_server_requests_seconds_count{app="my-app"}[5m])

2. Latency P50, P95, P99
   Query: histogram_quantile(0.95, rate(http_server_requests_seconds_bucket[5m]))

3. Error Rate
   Query: rate(http_server_requests_seconds_count{status=~"5.."}[5m])

4. JVM Heap Memory
   Query: jvm_memory_used_bytes{area="heap"}

5. Thread Count
   Query: jvm_threads_live{app="my-app"}

6. Garbage Collection Time
   Query: rate(jvm_gc_pause_seconds_sum[5m])

7. CPU Usage
   Query: rate(process_cpu_seconds_total{app="my-app"}[5m])

8. Active Database Connections
   Query: hikaricp_connections_active{app="my-app"}
```

---

## 7.3. Health Checks (Liveness, Readiness, Startup Probes)

### Qu'est-ce que les Probes ?

Les **probes** permettent à Kubernetes de vérifier l'état de santé des Pods :

**3 types de probes :**
1. **Liveness Probe** : Le Pod est-il vivant ?
2. **Readiness Probe** : Le Pod est-il prêt à recevoir du trafic ?
3. **Startup Probe** : Le Pod a-t-il démarré ?

---

### Liveness Probe

**But :** Détecter si un Pod est bloqué (deadlock, out of memory, etc.)

**Action si échec :** Kubernetes **redémarre le container**

**Exemple HTTP :**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: my-app
spec:
  containers:
  - name: app
    image: myapp:1.0
    livenessProbe:
      httpGet:
        path: /actuator/health/liveness
        port: 8080
      initialDelaySeconds: 30  # Attendre 30s après start
      periodSeconds: 10        # Vérifier toutes les 10s
      timeoutSeconds: 5        # Timeout de 5s
      successThreshold: 1      # 1 succès = OK
      failureThreshold: 3      # 3 échecs = RESTART
```

**Spring Boot Actuator :**
```yaml
# application.yml
management:
  endpoint:
    health:
      probes:
        enabled: true
  health:
    livenessState:
      enabled: true
```

**Endpoint :**
```bash
curl http://localhost:8080/actuator/health/liveness
# {"status":"UP"}
```

---

### Readiness Probe

**But :** Savoir si le Pod peut traiter des requêtes

**Action si échec :** Kubernetes **retire le Pod du Service** (pas de redémarrage)

**Exemple :**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: my-app
spec:
  containers:
  - name: app
    image: myapp:1.0
    readinessProbe:
      httpGet:
        path: /actuator/health/readiness
        port: 8080
      initialDelaySeconds: 10
      periodSeconds: 5
      timeoutSeconds: 3
      successThreshold: 1
      failureThreshold: 3
```

**Use case :**
- Attente connexion à la base de données
- Chargement de configuration
- Warm-up du cache

**Spring Boot :**
```java
@Component
public class DatabaseHealthIndicator implements HealthIndicator {
    
    @Autowired
    private DataSource dataSource;
    
    @Override
    public Health health() {
        try {
            Connection connection = dataSource.getConnection();
            connection.close();
            return Health.up().build();
        } catch (Exception e) {
            return Health.down(e).build();
        }
    }
}
```

---

### Startup Probe

**But :** Donner plus de temps aux apps qui démarrent lentement

**Action si échec :** Kubernetes **redémarre le container**

**Exemple :**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: slow-app
spec:
  containers:
  - name: app
    image: legacy-app:1.0
    startupProbe:
      httpGet:
        path: /actuator/health
        port: 8080
      initialDelaySeconds: 0
      periodSeconds: 10
      timeoutSeconds: 5
      successThreshold: 1
      failureThreshold: 30  # 30 * 10s = 5 minutes max
    
    # Liveness & Readiness activées APRÈS succès de startupProbe
    livenessProbe:
      httpGet:
        path: /actuator/health/liveness
        port: 8080
      periodSeconds: 10
    
    readinessProbe:
      httpGet:
        path: /actuator/health/readiness
        port: 8080
      periodSeconds: 5
```

---

### Types de Probes

**1. HTTP GET**
```yaml
livenessProbe:
  httpGet:
    path: /health
    port: 8080
    httpHeaders:
    - name: Custom-Header
      value: MyValue
```

**2. TCP Socket**
```yaml
livenessProbe:
  tcpSocket:
    port: 5432  # PostgreSQL
  initialDelaySeconds: 15
  periodSeconds: 10
```

**3. Exec Command**
```yaml
livenessProbe:
  exec:
    command:
    - cat
    - /tmp/healthy
  initialDelaySeconds: 5
  periodSeconds: 5
```

**4. gRPC (Kubernetes 1.24+)**
```yaml
livenessProbe:
  grpc:
    port: 50051
  initialDelaySeconds: 10
```

---

### Configuration complète

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app
spec:
  replicas: 3
  selector:
    matchLabels:
      app: my-app
  template:
    metadata:
      labels:
        app: my-app
    spec:
      containers:
      - name: app
        image: myapp:1.0
        ports:
        - containerPort: 8080
        
        # Startup : App lente à démarrer
        startupProbe:
          httpGet:
            path: /actuator/health
            port: 8080
          initialDelaySeconds: 0
          periodSeconds: 10
          failureThreshold: 30  # 5 minutes max
        
        # Liveness : Deadlock detection
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8080
          initialDelaySeconds: 0  # startupProbe prend le relai
          periodSeconds: 10
          timeoutSeconds: 5
          failureThreshold: 3
        
        # Readiness : Traffic routing
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          initialDelaySeconds: 0
          periodSeconds: 5
          timeoutSeconds: 3
          failureThreshold: 3
```

---

### Best Practices - Probes

**✅ Toujours définir readiness & liveness**
```yaml
# Minimum pour production
readinessProbe: ...
livenessProbe: ...
```

**✅ Endpoints dédiés (pas le endpoint métier)**
```yaml
# ✅ Bon
path: /actuator/health/liveness

# ❌ Mauvais
path: /api/orders  # Endpoint métier
```

**✅ Liveness ≠ Readiness**
```java
// Liveness : Basique (app alive)
@Component("livenessProbe")
public class LivenessProbe implements HealthIndicator {
    @Override
    public Health health() {
        // Simple check
        return Health.up().build();
    }
}

// Readiness : Dépendances (DB, cache, etc.)
@Component("readinessProbe")
public class ReadinessProbe implements HealthIndicator {
    @Autowired private DataSource dataSource;
    @Autowired private RedisTemplate redis;
    
    @Override
    public Health health() {
        // Check DB
        // Check Redis
        // Check external services
        return Health.up().build();
    }
}
```

**✅ timeoutSeconds < periodSeconds**
```yaml
periodSeconds: 10
timeoutSeconds: 5  # Laisse 5s de marge
```

**✅ failureThreshold adapté**
```yaml
# App rapide
failureThreshold: 3  # 3 * 10s = 30s

# App lente (DB queries)
failureThreshold: 5  # 5 * 10s = 50s
```

**❌ Éviter les probes trop fréquentes**
```yaml
# ❌ Trop fréquent
periodSeconds: 1  # Surcharge

# ✅ Bon
periodSeconds: 10
```

---

## 7.4. Debugging Techniques (describe, events, exec)

### kubectl describe

**Le plus important pour debugging !**

```bash
# Décrire un Pod
kubectl describe pod my-app-xxx

# Output contient:
# - Status du Pod
# - Events (erreurs, warnings)
# - Resource requests/limits
# - Probes configuration
# - Conditions
# - Containers status
```

**Exemple d'output :**
```
Name:         my-app-7d4f5c6b9-abc123
Namespace:    prod
Status:       Running

Containers:
  app:
    Image:      myapp:1.0
    State:      Running
    Ready:      True
    Restart Count:  0

Conditions:
  Type              Status
  Initialized       True
  Ready             True
  ContainersReady   True
  PodScheduled      True

Events:
  Type    Reason     Age   From               Message
  ----    ------     ----  ----               -------
  Normal  Scheduled  2m    default-scheduler  Successfully assigned prod/my-app-...
  Normal  Pulling    2m    kubelet            Pulling image "myapp:1.0"
  Normal  Pulled     2m    kubelet            Successfully pulled image
  Normal  Created    2m    kubelet            Created container app
  Normal  Started    2m    kubelet            Started container app
```

---

### kubectl get events

**Voir tous les events d'un namespace :**
```bash
# Events récents
kubectl get events --sort-by='.lastTimestamp'

# Events dans namespace prod
kubectl get events -n prod

# Events d'un Pod spécifique
kubectl get events --field-selector involvedObject.name=my-app-xxx

# Events de type Warning
kubectl get events --field-selector type=Warning

# Suivre les events en temps réel
kubectl get events --watch
```

**Events courants :**
```
# ✅ Normal
- Scheduled : Pod assigné à un Node
- Pulling : Téléchargement de l'image
- Created : Container créé
- Started : Container démarré

# ⚠️ Warning
- Failed : Container failed
- BackOff : CrashLoopBackOff
- FailedScheduling : Pas de Node disponible
- Unhealthy : Probe failed

# ❌ Error
- ImagePullBackOff : Image introuvable
- OOMKilled : Out of memory
```

---

### kubectl exec

**Exécuter des commandes dans un Pod :**

```bash
# Shell interactif
kubectl exec -it my-app-xxx -- /bin/sh
kubectl exec -it my-app-xxx -- /bin/bash

# Commande simple
kubectl exec my-app-xxx -- ls -la /app
kubectl exec my-app-xxx -- ps aux
kubectl exec my-app-xxx -- env

# Multi-container Pod
kubectl exec -it my-app-xxx -c sidecar -- /bin/sh

# Vérifier connectivité réseau
kubectl exec my-app-xxx -- ping postgres-service
kubectl exec my-app-xxx -- curl http://backend-service:8080/health

# Vérifier fichiers
kubectl exec my-app-xxx -- cat /app/config/application.properties
```

---

### Debugging Patterns

**1. Pod ne démarre pas (ImagePullBackOff)**

```bash
# Voir le problème
kubectl describe pod my-app-xxx
# Events:
#   Warning  Failed     2m    kubelet  Failed to pull image "myapp:1.0": rpc error: code = Unknown desc = Error response from daemon: pull access denied

# Causes possibles:
# - Image n'existe pas
# - Typo dans image name/tag
# - Private registry sans credentials

# Solutions:
# 1. Vérifier image name
kubectl get pod my-app-xxx -o jsonpath='{.spec.containers[0].image}'

# 2. Créer ImagePullSecret
kubectl create secret docker-registry regcred \
  --docker-server=myregistry.com \
  --docker-username=myuser \
  --docker-password=mypassword

# 3. Utiliser dans Deployment
spec:
  template:
    spec:
      imagePullSecrets:
      - name: regcred
```

---

**2. Pod en CrashLoopBackOff**

```bash
# Voir les logs
kubectl logs my-app-xxx
kubectl logs my-app-xxx --previous  # Logs avant crash

# Exemple d'erreur Java
# Error: Could not find or load main class com.example.Application

# Describe pour voir le restart count
kubectl describe pod my-app-xxx
# Restart Count: 5
# Last State: Terminated
#   Reason: Error
#   Exit Code: 1

# Causes possibles:
# - Configuration error
# - Missing environment variables
# - Database connection failed
# - OOMKilled

# Solutions:
# 1. Vérifier exit code
kubectl get pod my-app-xxx -o jsonpath='{.status.containerStatuses[0].lastState.terminated.exitCode}'

# 2. Augmenter resources si OOMKilled
resources:
  limits:
    memory: "1Gi"  # Augmenté
```

---

**3. Pod Running mais pas Ready**

```bash
# Voir le status
kubectl get pods
# NAME          READY   STATUS    RESTARTS
# my-app-xxx    0/1     Running   0

# Describe
kubectl describe pod my-app-xxx
# Readiness probe failed: HTTP probe failed with statuscode: 503

# Causes possibles:
# - Readiness probe mal configuré
# - App pas encore prête (DB connection)
# - Endpoint health retourne erreur

# Solutions:
# 1. Voir les logs
kubectl logs my-app-xxx

# 2. Tester le endpoint manuellement
kubectl exec my-app-xxx -- curl http://localhost:8080/actuator/health

# 3. Augmenter initialDelaySeconds
readinessProbe:
  initialDelaySeconds: 30  # Augmenté
```

---

**4. Service ne route pas le trafic**

```bash
# Vérifier les endpoints
kubectl get endpoints my-app-service

# Output vide = Aucun Pod Ready
# NAME              ENDPOINTS
# my-app-service    <none>

# Vérifier les labels
kubectl get pods --show-labels

# Vérifier selector du Service
kubectl get service my-app-service -o yaml | grep selector -A 2
# selector:
#   app: my-app

# Solutions:
# 1. Corriger les labels du Pod
metadata:
  labels:
    app: my-app  # Doit matcher selector

# 2. Vérifier readiness probe
```

---

**5. DNS ne résout pas**

```bash
# Tester DNS depuis un Pod
kubectl run -it --rm debug --image=busybox --restart=Never -- sh

# Dans le Pod:
nslookup my-service
nslookup my-service.prod.svc.cluster.local

# Si échec:
# 1. Vérifier CoreDNS
kubectl get pods -n kube-system -l k8s-app=kube-dns

# 2. Vérifier Service
kubectl get service my-service

# 3. Tester avec IP directe
kubectl get endpoints my-service
curl http://10.244.1.5:8080
```

---

**6. Connection refused entre Pods**

```bash
# Debug depuis un Pod
kubectl exec -it my-app-xxx -- sh

# Tester connectivité
curl http://backend-service:8080
# curl: (7) Failed to connect to backend-service port 8080: Connection refused

# Causes possibles:
# - Port incorrect
# - NetworkPolicy bloque
# - Service mal configuré

# Solutions:
# 1. Vérifier Service port
kubectl get service backend-service
# PORT(S)
# 8080/TCP

# 2. Vérifier que le Pod écoute sur le bon port
kubectl exec backend-xxx -- netstat -tlnp
# tcp  0  0  :::8080  :::*  LISTEN

# 3. Vérifier NetworkPolicies
kubectl get networkpolicies
kubectl describe networkpolicy allow-frontend
```

---

### kubectl port-forward

**Accéder à un Pod depuis votre machine locale :**

```bash
# Forward port 8080 du Pod vers localhost:8080
kubectl port-forward pod/my-app-xxx 8080:8080

# Accéder
curl http://localhost:8080/actuator/health

# Forward vers un Service
kubectl port-forward service/my-app 8080:8080

# Forward avec port local différent
kubectl port-forward pod/my-app-xxx 9090:8080

# Forward dans un namespace
kubectl port-forward -n prod pod/my-app-xxx 8080:8080
```

**Use case :**
- Débugger sans exposer via Ingress
- Accès temporaire à une base de données
- Tester une app localement

---

### kubectl cp

**Copier fichiers entre local et Pod :**

```bash
# Copier FROM Pod TO local
kubectl cp my-app-xxx:/app/logs/app.log ./app.log

# Copier FROM local TO Pod
kubectl cp ./config.json my-app-xxx:/app/config/

# Multi-container Pod
kubectl cp my-app-xxx:/tmp/file -c sidecar ./file

# Copier un répertoire
kubectl cp my-app-xxx:/app/logs ./logs-backup
```

**Use case :**
- Récupérer logs/dumps
- Injecter config temporaire
- Analyser core dumps

---

### kubectl debug (Kubernetes 1.23+)

**Créer un Pod debug avec outils supplémentaires :**

```bash
# Créer une copie du Pod avec image debug
kubectl debug my-app-xxx -it --image=busybox

# Créer un ephemeral container dans le Pod existant
kubectl debug my-app-xxx -it --image=busybox --target=app

# Debug avec image contenant outils (curl, netstat, etc.)
kubectl debug my-app-xxx -it --image=nicolaka/netshoot

# Dans le debug container:
curl http://localhost:8080/actuator/health
netstat -tlnp
ps aux
```

---

### Debugging distroless images

**Problème : Images distroless n'ont pas de shell**

```dockerfile
# Image distroless (pas de shell)
FROM gcr.io/distroless/java17-debian11
COPY target/app.jar /app/app.jar
ENTRYPOINT ["java", "-jar", "/app/app.jar"]
```

```bash
# ❌ Impossible
kubectl exec my-app-xxx -it -- /bin/sh
# error: unable to upgrade connection: container distroless does not support exec

# ✅ Solution: kubectl debug
kubectl debug my-app-xxx -it --image=busybox --target=app --share-processes

# Ou ajouter un debug container temporaire
kubectl debug my-app-xxx -it --image=nicolaka/netshoot --target=app
```

---

### Debugging Java Apps

**Heap dump :**
```bash
# Générer heap dump
kubectl exec my-app-xxx -- jmap -dump:format=b,file=/tmp/heapdump.hprof 1

# Copier localement
kubectl cp my-app-xxx:/tmp/heapdump.hprof ./heapdump.hprof

# Analyser avec VisualVM ou Eclipse MAT
```

**Thread dump :**
```bash
# Thread dump
kubectl exec my-app-xxx -- jstack 1 > threaddump.txt

# Ou via signal
kubectl exec my-app-xxx -- kill -3 1
kubectl logs my-app-xxx  # Thread dump dans logs
```

**JVM metrics :**
```bash
# Voir JVM flags
kubectl exec my-app-xxx -- java -XX:+PrintFlagsFinal -version

# Memory info
kubectl exec my-app-xxx -- jmap -heap 1

# GC info
kubectl exec my-app-xxx -- jstat -gc 1 1000 10
```

---

### Stern (multi-pod logs)

**Installation :**
```bash
# macOS
brew install stern

# Linux
wget https://github.com/stern/stern/releases/download/v1.25.0/stern_1.25.0_linux_amd64.tar.gz
```

**Usage :**
```bash
# Logs de tous les Pods d'un Deployment
stern my-app

# Avec namespace
stern my-app -n prod

# Regex pattern
stern "^my-app-.*"

# Filtrer par container
stern my-app -c app

# Avec timestamps
stern my-app --timestamps

# Exclure logs
stern my-app --exclude "health check"

# Coloré par Pod
stern my-app --color always
```

---

## 📝 Best Practices - Observability

### Logging

✅ **Toujours sur stdout/stderr**
```java
// ✅ Bon
logger.info("Order created");

// ❌ Mauvais
FileAppender.append("/var/log/app.log", "Order created");
```

✅ **Structured logging (JSON)**
```json
{"level":"INFO","timestamp":"2024-11-18T10:30:45Z","message":"Order created","orderId":"123"}
```

✅ **Log aggregation en production**
```yaml
# Loki, ELK, Datadog, etc.
```

✅ **Correlation IDs (trace ID)**
```java
MDC.put("traceId", request.getHeader("X-Trace-Id"));
logger.info("Processing request");
MDC.clear();
```

---

### Monitoring

✅ **Metrics Server installé**
```bash
kubectl top nodes
kubectl top pods
```

✅ **Actuator endpoints exposés**
```yaml
management:
  endpoints:
    web:
      exposure:
        include: health,info,prometheus,metrics
```

✅ **Métriques custom métier**
```java
Counter.builder("orders.created").register(meterRegistry);
Timer.builder("orders.processing.time").register(meterRegistry);
```

✅ **Prometheus + Grafana en production**
```yaml
# ServiceMonitor pour scraping
# Dashboard Grafana
```

---

### Health Checks

✅ **Toujours readiness + liveness**
```yaml
readinessProbe: ...
livenessProbe: ...
```

✅ **Startup probe pour apps lentes**
```yaml
startupProbe:
  failureThreshold: 30  # 5 minutes max
```

✅ **Endpoints dédiés**
```yaml
# ✅ Bon
/actuator/health/liveness
/actuator/health/readiness

# ❌ Mauvais
/api/orders
```

✅ **Timeouts appropriés**
```yaml
periodSeconds: 10
timeoutSeconds: 5
failureThreshold: 3
```

---

### Debugging

✅ **kubectl describe first**
```bash
kubectl describe pod my-app-xxx
```

✅ **Events monitoring**
```bash
kubectl get events --watch
```

✅ **Structured logs for parsing**
```json
{"level":"ERROR","exception":"NullPointerException","stacktrace":"..."}
```

✅ **Debug containers pour distroless**
```bash
kubectl debug my-app-xxx -it --image=nicolaka/netshoot
```

---

## 🎯 Checklist Production - Observability

```yaml
observability-checklist:
  logging:
    ✓ Logs sur stdout/stderr
    ✓ Structured logging (JSON)
    ✓ Log aggregation configurée (Loki/ELK)
    ✓ Retention policy définie
    ✓ Correlation IDs dans logs
  
  monitoring:
    ✓ Metrics Server installé
    ✓ Prometheus + Grafana déployés
    ✓ ServiceMonitor configuré
    ✓ Métriques custom exposées
    ✓ Alerts configurées (PagerDuty/Slack)
  
  health-checks:
    ✓ Readiness probe configurée
    ✓ Liveness probe configurée
    ✓ Startup probe si app lente
    ✓ Endpoints dédiés (/health)
    ✓ Timeouts appropriés
  
  debugging:
    ✓ Labels standardisés
    ✓ Events monitoring
    ✓ Debug tools image disponible
    ✓ Logs accessibles (kubectl logs)
    ✓ Port-forward possible si besoin
```

---

# Chapitre 7 : Questions/Réponses d'Entretien - Observability & Debugging

## 📊 Logging

### Q1 : Comment collectez-vous les logs de vos applications Spring Boot dans Kubernetes ?

**Réponse :**

**Architecture complète :**
```
Spring Boot App → stdout → Container Runtime → Log Aggregator → Storage → Visualisation
```

**1. Configuration Spring Boot**

**pom.xml :**
```xml
<!-- Logback JSON encoder -->
<dependency>
    <groupId>net.logstash.logback</groupId>
    <artifactId>logstash-logback-encoder</artifactId>
    <version>7.3</version>
</dependency>
```

**logback-spring.xml :**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<configuration>
    <appender name="JSON_CONSOLE" class="ch.qos.logback.core.ConsoleAppender">
        <encoder class="net.logstash.logback.encoder.LogstashEncoder">
            <includeContext>true</includeContext>
            <includeMdcKeyName>traceId</includeMdcKeyName>
            <includeMdcKeyName>userId</includeMdcKeyName>
            <includeMdcKeyName>orderId</includeMdcKeyName>
        </encoder>
    </appender>
    
    <root level="INFO">
        <appender-ref ref="JSON_CONSOLE"/>
    </root>
    
    <logger name="com.example.myapp" level="DEBUG"/>
</configuration>
```

**Java Code :**
```java
@RestController
@Slf4j
public class OrderController {
    
    @PostMapping("/orders")
    public Order createOrder(@RequestBody OrderRequest request) {
        // Ajouter contexte dans MDC
        MDC.put("orderId", request.getId());
        MDC.put("userId", request.getUserId());
        
        try {
            log.info("Creating new order", 
                kv("amount", request.getAmount()),
                kv("items", request.getItems().size()));
            
            Order order = orderService.create(request);
            
            log.info("Order created successfully");
            return order;
            
        } catch (Exception e) {
            log.error("Failed to create order", e);
            throw e;
        } finally {
            MDC.clear();
        }
    }
}
```

**Output JSON :**
```json
{
  "@timestamp": "2024-11-18T10:30:45.123Z",
  "level": "INFO",
  "logger_name": "com.example.OrderController",
  "message": "Creating new order",
  "thread_name": "http-nio-8080-exec-1",
  "orderId": "ORD-12345",
  "userId": "user-789",
  "amount": 150.00,
  "items": 3
}
```

---

**2. Deployment Kubernetes**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
spec:
  replicas: 3
  template:
    metadata:
      labels:
        app: order-service
        version: v1.0
    spec:
      containers:
      - name: app
        image: order-service:1.0
        # Logs automatiquement sur stdout
        
        env:
        - name: SPRING_PROFILES_ACTIVE
          value: "prod"
        
        # Important: Ne pas écrire logs dans fichiers
        volumeMounts:
        - name: tmp
          mountPath: /tmp  # Pour fichiers temporaires uniquement
      
      volumes:
      - name: tmp
        emptyDir: {}
```

---

**3. Log Aggregation - Loki + Promtail**

**Promtail DaemonSet :**
```yaml
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: promtail
  namespace: logging
spec:
  selector:
    matchLabels:
      app: promtail
  template:
    metadata:
      labels:
        app: promtail
    spec:
      serviceAccountName: promtail
      containers:
      - name: promtail
        image: grafana/promtail:2.9.0
        args:
        - -config.file=/etc/promtail/promtail.yaml
        volumeMounts:
        - name: config
          mountPath: /etc/promtail
        - name: pods
          mountPath: /var/log/pods
          readOnly: true
        - name: docker
          mountPath: /var/lib/docker/containers
          readOnly: true
      
      volumes:
      - name: config
        configMap:
          name: promtail-config
      - name: pods
        hostPath:
          path: /var/log/pods
      - name: docker
        hostPath:
          path: /var/lib/docker/containers
```

**ConfigMap Promtail :**
```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: promtail-config
  namespace: logging
data:
  promtail.yaml: |
    server:
      http_listen_port: 9080
    
    clients:
    - url: http://loki:3100/loki/api/v1/push
    
    scrape_configs:
    - job_name: kubernetes-pods
      kubernetes_sd_configs:
      - role: pod
      
      pipeline_stages:
      # Parser JSON logs
      - json:
          expressions:
            level: level
            timestamp: "@timestamp"
            logger: logger_name
            message: message
            traceId: traceId
            userId: userId
      
      # Extraire labels
      - labels:
          level:
          traceId:
          userId:
      
      relabel_configs:
      - source_labels: [__meta_kubernetes_namespace]
        target_label: namespace
      - source_labels: [__meta_kubernetes_pod_name]
        target_label: pod
      - source_labels: [__meta_kubernetes_pod_label_app]
        target_label: app
```

---

**4. Query dans Grafana**

```promql
# Tous les logs de order-service
{app="order-service"}

# Logs d'erreur uniquement
{app="order-service"} | json | level="ERROR"

# Logs d'un user spécifique
{app="order-service"} | json | userId="user-789"

# Logs d'une commande
{app="order-service"} | json | orderId="ORD-12345"

# Rate d'erreurs
rate({app="order-service"} | json | level="ERROR" [5m])

# Top 10 erreurs
topk(10, sum by (message) (
  count_over_time({app="order-service"} | json | level="ERROR" [24h])
))
```

---

**5. Consultation quotidienne**

```bash
# Logs en temps réel
kubectl logs -f deployment/order-service

# Logs avec stern (multi-pods)
stern order-service

# Logs d'un Pod spécifique
kubectl logs order-service-7d4f5c6b9-abc123

# Logs après crash
kubectl logs order-service-7d4f5c6b9-abc123 --previous

# Logs dans Grafana
# Dashboard → Explore → Loki
{app="order-service"} |= "ERROR"
```

---

**Avantages de cette approche :**
- ✅ Logs structurés (JSON) faciles à parser
- ✅ Correlation via traceId
- ✅ Centralisés dans Grafana
- ✅ Searchable et filtrable
- ✅ Alerting possible sur patterns d'erreurs
- ✅ Retention policy configurable

---

### Q2 : Expliquez la différence entre les 3 types de probes et donnez des exemples d'utilisation.

**Réponse :**

**Les 3 types de probes :**

| Probe | But | Action si échec | Quand utiliser |
|-------|-----|-----------------|----------------|
| **Liveness** | Le Pod est-il vivant ? | **Redémarre** le container | Détecter deadlocks, hangs |
| **Readiness** | Prêt à recevoir du trafic ? | **Retire du Service** | Warm-up, dépendances temporaires down |
| **Startup** | A-t-il terminé de démarrer ? | **Redémarre** le container | Apps lentes à démarrer |

---

**1. Liveness Probe - Détection de deadlocks**

**Problème :** Application bloquée (deadlock, out of memory, boucle infinie)

**Solution :** Kubernetes redémarre automatiquement

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
spec:
  template:
    spec:
      containers:
      - name: app
        image: order-service:1.0
        
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8080
          initialDelaySeconds: 30  # Attendre le démarrage
          periodSeconds: 10        # Check toutes les 10s
          timeoutSeconds: 5        # Timeout 5s
          failureThreshold: 3      # 3 échecs = RESTART
```

**Spring Boot endpoint :**
```java
// application.yml
management:
  endpoint:
    health:
      probes:
        enabled: true
  health:
    livenessState:
      enabled: true

// Endpoint: /actuator/health/liveness
// {"status":"UP"}
```

**Custom liveness check :**
```java
@Component
public class CustomLivenessProbe implements HealthIndicator {
    
    @Override
    public Health health() {
        // Check basique : app alive
        try {
            // Vérifier que le thread pool n'est pas saturé
            ThreadPoolExecutor executor = (ThreadPoolExecutor) executorService;
            if (executor.getActiveCount() >= executor.getMaximumPoolSize()) {
                return Health.down()
                    .withDetail("reason", "Thread pool exhausted")
                    .build();
            }
            
            return Health.up().build();
        } catch (Exception e) {
            return Health.down(e).build();
        }
    }
}
```

---

**2. Readiness Probe - Gestion du trafic**

**Problème :** App démarre mais n'est pas prête (DB connection en cours, cache warmup)

**Solution :** Kubernetes attend que le Pod soit Ready avant de router du trafic

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
spec:
  template:
    spec:
      containers:
      - name: app
        image: order-service:1.0
        
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          initialDelaySeconds: 10  # Courte attente
          periodSeconds: 5         # Check fréquents
          timeoutSeconds: 3
          failureThreshold: 3      # 3 échecs = Retire du Service
```

**Spring Boot endpoint :**
```java
// application.yml
management:
  health:
    readinessState:
      enabled: true

// Endpoint: /actuator/health/readiness
```

**Custom readiness check :**
```java
@Component
public class DatabaseReadinessProbe implements HealthIndicator {
    
    @Autowired
    private DataSource dataSource;
    
    @Autowired
    private RedisTemplate<String, String> redisTemplate;
    
    @Override
    public Health health() {
        Health.Builder builder = Health.up();
        
        // Check database
        try (Connection conn = dataSource.getConnection()) {
            if (!conn.isValid(2)) {
                builder.down().withDetail("database", "Connection invalid");
            }
        } catch (Exception e) {
            builder.down().withDetail("database", e.getMessage());
        }
        
        // Check Redis
        try {
            redisTemplate.opsForValue().get("health-check");
        } catch (Exception e) {
            builder.down().withDetail("redis", e.getMessage());
        }
        
        // Check external service
        try {
            restTemplate.getForEntity("http://payment-service/health", String.class);
        } catch (Exception e) {
            builder.down().withDetail("payment-service", e.getMessage());
        }
        
        return builder.build();
    }
}
```

**Scénario Rolling Update :**
```
1. Nouveau Pod démarre
2. Readiness probe = FAIL (DB connection en cours)
3. Kubernetes N'ENVOIE PAS de trafic
4. DB connectée
5. Readiness probe = SUCCESS
6. Kubernetes ENVOIE du trafic
7. Ancien Pod terminé
```

---

**3. Startup Probe - Apps lentes**

**Problème :** Application legacy qui met 2-3 minutes à démarrer

**Sans startup probe :**
```yaml
livenessProbe:
  initialDelaySeconds: 180  # ❌ Attendre 3 minutes pour TOUS les checks
```

**Avec startup probe :**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: legacy-app
spec:
  template:
    spec:
      containers:
      - name: app
        image: legacy-app:1.0
        
        # Startup : Donner du temps au démarrage
        startupProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 0
          periodSeconds: 10
          failureThreshold: 30      # 30 * 10s = 5 minutes MAX
        
        # Liveness : Activée APRÈS succès de startupProbe
        livenessProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 0    # Pas besoin d'attendre
          periodSeconds: 10
          failureThreshold: 3       # 30s max
        
        # Readiness : Activée APRÈS succès de startupProbe
        readinessProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 0
          periodSeconds: 5
```

**Avantages :**
- ✅ Liveness ne tue pas le Pod pendant démarrage
- ✅ Liveness rapide après démarrage (30s au lieu de 3 min)
- ✅ Protection contre vraies pannes après démarrage

---

**Configuration complète - Exemple production**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
spec:
  replicas: 3
  template:
    metadata:
      labels:
        app: order-service
    spec:
      containers:
      - name: app
        image: order-service:1.0
        ports:
        - containerPort: 8080
        
        # 1. Startup : App Spring Boot (20-30s démarrage)
        startupProbe:
          httpGet:
            path: /actuator/health
            port: 8080
          initialDelaySeconds: 0
          periodSeconds: 5
          timeoutSeconds: 3
          failureThreshold: 12      # 12 * 5s = 60s max
        
        # 2. Liveness : Deadlock detection
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8080
          initialDelaySeconds: 0
          periodSeconds: 10
          timeoutSeconds: 5
          successThreshold: 1
          failureThreshold: 3       # 3 * 10s = 30s
        
        # 3. Readiness : Dependencies check
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          initialDelaySeconds: 0
          periodSeconds: 5
          timeoutSeconds: 3
          successThreshold: 1
          failureThreshold: 3       # 3 * 5s = 15s
        
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
```

---

**Timeline exemple :**
```
T=0s     : Pod créé
T=0-20s  : Startup probe CHECK (app démarre)
T=20s    : Startup probe SUCCESS
T=20s+   : Liveness + Readiness activées
T=22s    : Readiness SUCCESS (DB connected)
T=22s+   : Pod reçoit du trafic
T=30s+   : Liveness check toutes les 10s
```

---

**Comparaison des échecs :**

**Liveness fails :**
```bash
kubectl describe pod order-service-xxx
# Events:
#   Warning  Unhealthy  1m   kubelet  Liveness probe failed: HTTP probe failed
#   Normal   Killing    1m   kubelet  Container app failed liveness probe, will be restarted

kubectl get pods
# NAME                READY   STATUS    RESTARTS
# order-service-xxx   0/1     Running   5        # ← Restart count augmente
```

**Readiness fails :**
```bash
kubectl describe pod order-service-xxx
# Events:
#   Warning  Unhealthy  1m   kubelet  Readiness probe failed: HTTP probe failed

kubectl get pods
# NAME                READY   STATUS    RESTARTS
# order-service-xxx   0/1     Running   0        # ← Pas de restart

kubectl get endpoints order-service
# NAME              ENDPOINTS
# order-service     10.244.1.5:8080,10.244.1.6:8080  # ← Pod xxx absent
```

**Startup fails :**
```bash
kubectl describe pod order-service-xxx
# Events:
#   Warning  Unhealthy  5m   kubelet  Startup probe failed: HTTP probe failed
#   Normal   Killing    5m   kubelet  Container failed to start within timeout

# Pod restarté comme liveness
```

---

**Best Practices :**

✅ **Toujours les 3 en production**
```yaml
startupProbe: ...   # Pour apps lentes
livenessProbe: ...  # Deadlock detection
readinessProbe: ... # Traffic routing
```

✅ **Endpoints différents**
```yaml
livenessProbe:
  path: /actuator/health/liveness    # Basique
readinessProbe:
  path: /actuator/health/readiness   # + Dependencies
```

✅ **Timeouts cohérents**
```yaml
periodSeconds: 10
timeoutSeconds: 5  # < periodSeconds
failureThreshold: 3
```

❌ **Ne PAS checker dependencies dans liveness**
```java
// ❌ Mauvais dans liveness
public Health health() {
    checkDatabase();  // Si DB down → Pod restart → Cascade restart
}

// ✅ Bon : Database check dans readiness uniquement
```

---

### Q3 : Comment debuggez-vous un Pod qui est en CrashLoopBackOff ?

**Réponse :**

**Approche systématique en 6 étapes :**

---

**1. Identifier le problème - kubectl describe**

```bash
# Voir les events
kubectl describe pod my-app-xxx

# Output clé:
# State: Waiting
#   Reason: CrashLoopBackOff
# Last State: Terminated
#   Reason: Error
#   Exit Code: 1
#   Started: 10:30:00
#   Finished: 10:30:05
# Restart Count: 8
```

**Exit codes courants :**
```
0   : Success (normal)
1   : General application error
2   : Misuse of shell command
126 : Command cannot execute
127 : Command not found
137 : SIGKILL (OOMKilled)
143 : SIGTERM (graceful shutdown)
```

---

**2. Consulter les logs**

```bash
# Logs actuels
kubectl logs my-app-xxx

# Logs du container précédent (avant crash)
kubectl logs my-app-xxx --previous

# Si multi-container
kubectl logs my-app-xxx -c app --previous
```

**Exemples d'erreurs courantes :**

**A. Java - ClassNotFoundException**
```
Error: Could not find or load main class com.example.Application
Caused by: java.lang.ClassNotFoundException: com.example.Application
```

**Cause :** JAR mal packagé, mauvais ENTRYPOINT

**Solution :**
```dockerfile
# ✅ Correct Dockerfile
FROM openjdk:17-slim
COPY target/myapp-1.0.jar /app/app.jar
WORKDIR /app
ENTRYPOINT ["java", "-jar", "app.jar"]
```

---

**B. Spring Boot - Failed to configure DataSource**
```
***************************
APPLICATION FAILED TO START
***************************

Description:
Failed to configure a DataSource: 'url' attribute is not specified

Action:
Consider the following:
  - Verify that your database is running
  - Ensure that your connection URL is correct
```

**Cause :** Variables d'environnement manquantes

**Solution :**
```yaml
env:
- name: SPRING_DATASOURCE_URL
  value: "jdbc:postgresql://postgres:5432/mydb"
- name: SPRING_DATASOURCE_USERNAME
  valueFrom:
    secretKeyRef:
      name: db-credentials
      key: username
- name: SPRING_DATASOURCE_PASSWORD
  valueFrom:
    secretKeyRef:
      name: db-credentials
      key: password
```

---

**C. Port already in use**
```
***************************
APPLICATION FAILED TO START
***************************

Description:
Web server failed to start. Port 8080 was already in use.

Action:
Identify and stop the process that's listening on port 8080
```

**Cause :** Mauvaise configuration du container

**Solution :**
```yaml
# Vérifier containerPort
ports:
- containerPort: 8080  # Doit matcher app.port

# Vérifier qu'un seul process écoute
# Bug courant: script qui lance 2x l'app
```

---

**3. Vérifier Exit Code = 137 (OOMKilled)**

```bash
kubectl describe pod my-app-xxx
# Last State: Terminated
#   Reason: OOMKilled  # ← Out of Memory
#   Exit Code: 137
```

**Cause :** Memory limit trop bas

```bash
# Voir l'utilisation réelle
kubectl top pod my-app-xxx
# NAME         CPU    MEMORY
# my-app-xxx   250m   950Mi  # ← Proche de la limite !
```

**Solution :**
```yaml
resources:
  requests:
    memory: "512Mi"  # Minimum
  limits:
    memory: "2Gi"    # ✅ Augmenté

# + JVM tuning
env:
- name: JAVA_OPTS
  value: "-Xms512m -Xmx1536m"  # 75% de la limite
```

---

**4. Vérifier les ConfigMaps/Secrets**

```bash
# Lister les ConfigMaps/Secrets montés
kubectl describe pod my-app-xxx | grep -A 5 "Mounts"

# Vérifier qu'ils existent
kubectl get configmap app-config
kubectl get secret db-credentials

# Vérifier le contenu
kubectl get configmap app-config -o yaml
```

**Erreur courante :**
```yaml
# ConfigMap n'existe pas
volumeMounts:
- name: config
  mountPath: /app/config
volumes:
- name: config
  configMap:
    name: app-config  # ← N'existe pas !
```

**Solution :**
```bash
# Créer le ConfigMap manquant
kubectl create configmap app-config \
  --from-file=application.properties
```

---

**5. Exec dans un container similaire (debug)**

```bash
# Impossible de exec dans un Pod qui crash
kubectl exec my-app-xxx -- sh
# error: unable to upgrade connection: container is not running

# Solution: Créer un Pod debug avec la même image
kubectl run -it debug --image=myapp:1.0 --rm --restart=Never -- sh

# Dans le Pod debug:
ls -la /app
cat /app/config/application.properties
java -jar /app/app.jar  # Tester manuellement
```

---

**6. Modifier temporairement le Deployment**

**Technique : Empêcher l'app de crasher**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app
spec:
  template:
    spec:
      containers:
      - name: app
        image: myapp:1.0
        # Override entrypoint temporairement
        command: ["sleep"]
        args: ["3600"]  # Sleep 1h
```

```bash
# Appliquer
kubectl apply -f deployment-debug.yaml

# Pod reste Running
kubectl get pods
# NAME         READY   STATUS    RESTARTS
# my-app-xxx   1/1     Running   0

# Exec dedans
kubectl exec -it my-app-xxx -- sh

# Debug
ls -la /app
env | grep SPRING
cat /app/config/application.properties
java -jar /app/app.jar  # Tester manuellement
```

---

**Cas concrets résolus :**

**Cas 1 : JAR corrompu**
```bash
kubectl logs my-app-xxx --previous
# Error: Invalid or corrupt jarfile /app/app.jar

# Debug
kubectl exec -it my-app-xxx -- sh
ls -lh /app/app.jar
# -rw-r--r-- 1 root root 0 Nov 18 10:00 app.jar  # ← 0 bytes !

# Solution: Rebuild image correctement
docker build -t myapp:1.0 .
docker push myapp:1.0
kubectl rollout restart deployment/my-app
```

---

**Cas 2 : Permission denied**
```bash
kubectl logs my-app-xxx --previous
# /bin/sh: /app/entrypoint.sh: Permission denied

# Cause: Script pas exécutable
# Solution Dockerfile:
COPY entrypoint.sh /app/
RUN chmod +x /app/entrypoint.sh
ENTRYPOINT ["/app/entrypoint.sh"]
```

---

**Cas 3 : DNS resolution failed**
```bash
kubectl logs my-app-xxx --previous
# Caused by: java.net.UnknownHostException: postgres-service

# Vérifier DNS
kubectl exec -it debug-pod -- nslookup postgres-service
# Server:    10.96.0.10
# Address:   10.96.0.10:53
# 
# ** server can't find postgres-service: NXDOMAIN

# Cause: Service n'existe pas ou mauvais namespace
kubectl get service postgres-service -n prod  # ← Dans autre namespace !

# Solution:
env:
- name: SPRING_DATASOURCE_URL
  value: "jdbc:postgresql://postgres-service.prod.svc.cluster.local:5432/mydb"
#                                          ^^^^  ← Ajouter namespace
```

---

**Checklist debugging CrashLoopBackOff :**

```yaml
crashloop-debug-checklist:
  1_describe:
    ✓ kubectl describe pod → Exit code
    ✓ Events → Erreurs
    ✓ Restart count
  
  2_logs:
    ✓ kubectl logs --previous
    ✓ Identifier l'erreur exacte
  
  3_resources:
    ✓ Exit code 137 ? → OOMKilled
    ✓ kubectl top pod
    ✓ Augmenter memory limits
  
  4_configuration:
    ✓ ConfigMaps existent ?
    ✓ Secrets existent ?
    ✓ Variables d'environnement correctes ?
  
  5_image:
    ✓ Image existe et est pullable ?
    ✓ ENTRYPOINT correct ?
    ✓ JAR/Binary présent et valide ?
  
  6_dependencies:
    ✓ Database accessible ?
    ✓ DNS résout les services ?
    ✓ Network policies permettent connexion ?
```

---

## 🎯 Conseils pour l'Entretien

### Points clés à maîtriser :
1. **Logging** : stdout/stderr, JSON structuré, log aggregation
2. **Monitoring** : Metrics Server, Prometheus, Actuator
3. **Probes** : 3 types, différences, use cases
4. **Debugging** : kubectl describe, logs, events, exec

### Red flags à éviter :
- ❌ Logs dans fichiers (pas dans stdout)
- ❌ Pas de health checks configurés
- ❌ Liveness check les dependencies externes
- ❌ Ne pas savoir utiliser kubectl describe

### Points bonus :
- ✅ Log aggregation (Loki/ELK) en place
- ✅ Métriques custom avec Micrometer
- ✅ Correlation IDs dans logs
- ✅ Grafana dashboards pour monitoring
- ✅ Debugging avec ephemeral containers

---

# Chapitre 7 : Questions/Réponses d'Entretien - Observability & Debugging

## 📊 LOGGING

### Q1 : Comment collectez-vous les logs de vos applications Spring Boot dans Kubernetes ?

**Réponse :**

**Architecture complète :**
```
Spring Boot App → stdout/stderr → Container Runtime → Log Aggregator → Storage → Visualisation
```

**1. Configuration Spring Boot**

**pom.xml :**
```xml
<dependency>
    <groupId>net.logstash.logback</groupId>
    <artifactId>logstash-logback-encoder</artifactId>
    <version>7.3</version>
</dependency>
```

**logback-spring.xml :**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<configuration>
    <appender name="JSON_CONSOLE" class="ch.qos.logback.core.ConsoleAppender">
        <encoder class="net.logstash.logback.encoder.LogstashEncoder">
            <includeMdcKeyName>traceId</includeMdcKeyName>
            <includeMdcKeyName>userId</includeMdcKeyName>
        </encoder>
    </appender>
    
    <root level="INFO">
        <appender-ref ref="JSON_CONSOLE"/>
    </root>
</configuration>
```

**Java Code :**
```java
@RestController
@Slf4j
public class OrderController {
    
    @PostMapping("/orders")
    public ResponseEntity<Order> createOrder(@RequestBody OrderRequest request) {
        MDC.put("orderId", request.getId());
        MDC.put("userId", request.getUserId());
        
        try {
            log.info("Creating new order");
            Order order = orderService.create(request);
            return ResponseEntity.ok(order);
        } finally {
            MDC.clear();
        }
    }
}
```

**2. Log Aggregation - Loki + Promtail**

```yaml
# Promtail DaemonSet
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: promtail
  namespace: logging
spec:
  selector:
    matchLabels:
      app: promtail
  template:
    metadata:
      labels:
        app: promtail
    spec:
      containers:
      - name: promtail
        image: grafana/promtail:2.9.0
        volumeMounts:
        - name: pods
          mountPath: /var/log/pods
          readOnly: true
      volumes:
      - name: pods
        hostPath:
          path: /var/log/pods
```

**3. Query dans Grafana**

```promql
# Tous les logs de order-service
{app="order-service"}

# Logs d'erreur
{app="order-service"} | json | level="ERROR"

# Logs d'un user spécifique
{app="order-service"} | json | userId="user-789"
```

---

## 🔍 HEALTH CHECKS

### Q2 : Expliquez la différence entre les 3 types de probes et donnez des exemples d'utilisation

**Réponse :**

**Comparaison :**

| Probe | But | Action si échec | Quand utiliser |
|-------|-----|-----------------|----------------|
| **Liveness** | Pod vivant ? | **Redémarre** container | Deadlocks, hangs |
| **Readiness** | Prêt pour trafic ? | **Retire du Service** | Warm-up, dependencies |
| **Startup** | Démarrage terminé ? | **Redémarre** container | Apps lentes |

---

**1. Liveness Probe - Détection de deadlocks**

```yaml
livenessProbe:
  httpGet:
    path: /actuator/health/liveness
    port: 8080
  initialDelaySeconds: 30
  periodSeconds: 10
  timeoutSeconds: 5
  failureThreshold: 3      # 3 échecs = RESTART
```

**Spring Boot :**
```yaml
# application.yml
management:
  health:
    livenessState:
      enabled: true
```

```java
@Component
public class CustomLivenessProbe implements HealthIndicator {
    @Override
    public Health health() {
        // Check basique : app alive
        return Health.up().build();
    }
}
```

---

**2. Readiness Probe - Gestion du trafic**

```yaml
readinessProbe:
  httpGet:
    path: /actuator/health/readiness
    port: 8080
  initialDelaySeconds: 10
  periodSeconds: 5
  failureThreshold: 3      # 3 échecs = Retire du Service
```

```java
@Component
public class DatabaseReadinessProbe implements HealthIndicator {
    
    @Autowired
    private DataSource dataSource;
    
    @Override
    public Health health() {
        try (Connection conn = dataSource.getConnection()) {
            if (conn.isValid(2)) {
                return Health.up().build();
            }
        } catch (Exception e) {
            return Health.down().withDetail("database", e.getMessage()).build();
        }
        return Health.down().build();
    }
}
```

**Scénario Rolling Update :**
```
1. Nouveau Pod démarre
2. Readiness = FAIL (DB connection en cours)
3. K8s N'ENVOIE PAS de trafic
4. DB connectée
5. Readiness = SUCCESS
6. K8s ENVOIE du trafic
```

---

**3. Startup Probe - Apps lentes**

```yaml
startupProbe:
  httpGet:
    path: /actuator/health
    port: 8080
  initialDelaySeconds: 0
  periodSeconds: 10
  failureThreshold: 30      # 30 * 10s = 5 minutes MAX

livenessProbe:
  httpGet:
    path: /actuator/health/liveness
    port: 8080
  initialDelaySeconds: 0    # Activée APRÈS succès startup
  periodSeconds: 10
```

**Avantages :**
- ✅ Liveness ne tue pas le Pod pendant démarrage
- ✅ Liveness rapide après démarrage

---

**Configuration complète production :**

```yaml
containers:
- name: app
  image: order-service:1.0
  
  startupProbe:
    httpGet:
      path: /actuator/health
      port: 8080
    initialDelaySeconds: 10
    periodSeconds: 10
    failureThreshold: 18  # 180s max
  
  livenessProbe:
    httpGet:
      path: /actuator/health/liveness
      port: 8080
    periodSeconds: 10
    failureThreshold: 3
  
  readinessProbe:
    httpGet:
      path: /actuator/health/readiness
      port: 8080
    periodSeconds: 5
    failureThreshold: 3
```

**Best Practices :**

✅ Endpoints différents
```yaml
livenessProbe:
  path: /actuator/health/liveness    # Basique
readinessProbe:
  path: /actuator/health/readiness   # + Dependencies
```

❌ Ne PAS checker dependencies dans liveness
```java
// ❌ Mauvais dans liveness
public Health health() {
    checkDatabase();  // Si DB down → Pod restart → Cascade
}
```

---

## 🐛 DEBUGGING

### Q3 : Comment debuggez-vous un Pod qui est en CrashLoopBackOff ?

**Réponse :**

**Approche systématique :**

---

**1. kubectl describe**

```bash
kubectl describe pod my-app-xxx

# Output clé:
# State: Waiting
#   Reason: CrashLoopBackOff
# Last State: Terminated
#   Reason: Error
#   Exit Code: 1
# Restart Count: 8
```

**Exit codes :**
```
0   : Success
1   : General error
137 : SIGKILL (OOMKilled)
143 : SIGTERM
```

---

**2. Consulter les logs**

```bash
# Logs du container précédent (avant crash)
kubectl logs my-app-xxx --previous

# Multi-container
kubectl logs my-app-xxx -c app --previous
```

**Erreurs courantes :**

**A. ClassNotFoundException**
```
Error: Could not find or load main class com.example.Application
```

**Solution :**
```dockerfile
FROM openjdk:17-slim
COPY target/myapp-1.0.jar /app/app.jar
WORKDIR /app
ENTRYPOINT ["java", "-jar", "app.jar"]
```

**B. Failed to configure DataSource**
```
Failed to configure a DataSource: 'url' attribute is not specified
```

**Solution :**
```yaml
env:
- name: SPRING_DATASOURCE_URL
  value: "jdbc:postgresql://postgres:5432/mydb"
- name: SPRING_DATASOURCE_USERNAME
  valueFrom:
    secretKeyRef:
      name: db-credentials
      key: username
```

---

**3. Vérifier OOMKilled (Exit Code 137)**

```bash
kubectl describe pod my-app-xxx
# Reason: OOMKilled
# Exit Code: 137

kubectl top pod my-app-xxx
# MEMORY: 950Mi  ← Proche limite !
```

**Solution :**
```yaml
resources:
  limits:
    memory: "2Gi"    # Augmenté
env:
- name: JAVA_OPTS
  value: "-Xms512m -Xmx1536m"
```

---

**4. Vérifier ConfigMaps/Secrets**

```bash
# Vérifier qu'ils existent
kubectl get configmap app-config
kubectl get secret db-credentials

# Voir contenu
kubectl get configmap app-config -o yaml
```

---

**5. Debug avec Pod temporaire**

```bash
# Impossible exec dans Pod qui crash
kubectl exec my-app-xxx -- sh
# error: container is not running

# Solution: Pod debug
kubectl run -it debug --image=myapp:1.0 --rm --restart=Never -- sh

# Tester manuellement
ls -la /app
cat /app/config/application.properties
java -jar /app/app.jar
```

---

**6. Override entrypoint temporairement**

```yaml
spec:
  containers:
  - name: app
    image: myapp:1.0
    command: ["sleep"]
    args: ["3600"]  # Sleep 1h
```

```bash
kubectl apply -f deployment-debug.yaml

# Pod reste Running
kubectl exec -it my-app-xxx -- sh

# Debug
java -jar /app/app.jar
```

---

**Cas concrets :**

**JAR corrompu :**
```bash
ls -lh /app/app.jar
# 0 bytes !

# Rebuild
docker build -t myapp:1.0 .
kubectl rollout restart deployment/my-app
```

**Permission denied :**
```dockerfile
COPY entrypoint.sh /app/
RUN chmod +x /app/entrypoint.sh
```

**DNS failed :**
```bash
nslookup postgres-service
# NXDOMAIN

# Service dans autre namespace
env:
- name: SPRING_DATASOURCE_URL
  value: "jdbc:postgresql://postgres-service.prod.svc.cluster.local:5432/mydb"
```

---

## 📈 MONITORING

### Q4 : Comment exposez-vous des métriques custom dans Spring Boot pour Prometheus ?

**Réponse :**

**1. Dépendances**

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

---

**2. Configuration**

```yaml
# application.yml
management:
  endpoints:
    web:
      exposure:
        include: health,prometheus,metrics
  metrics:
    tags:
      application: ${spring.application.name}
      environment: ${ENVIRONMENT:dev}
```

---

**3. Métriques custom**

```java
@Service
public class OrderService {
    
    private final MeterRegistry meterRegistry;
    private final Counter orderCreatedCounter;
    private final Timer orderProcessingTimer;
    
    public OrderService(MeterRegistry meterRegistry) {
        this.meterRegistry = meterRegistry;
        
        // Counter
        this.orderCreatedCounter = Counter.builder("orders.created")
            .description("Total orders created")
            .tag("status", "success")
            .register(meterRegistry);
        
        // Timer
        this.orderProcessingTimer = Timer.builder("orders.processing.time")
            .description("Order processing time")
            .register(meterRegistry);
    }
    
    public Order createOrder(OrderRequest request) {
        return orderProcessingTimer.record(() -> {
            Order order = processOrder(request);
            
            orderCreatedCounter.increment();
            
            // Gauge
            meterRegistry.gauge("orders.pending", 
                orderRepository.countByStatus("PENDING"));
            
            return order;
        });
    }
}
```

---

**4. Endpoint Prometheus**

```bash
curl http://localhost:8080/actuator/prometheus

# Output:
orders_created_total{status="success"} 1234.0
orders_processing_time_seconds_sum 456.78
orders_processing_time_seconds_count 1234.0
orders_pending 42.0
```

---

**5. ServiceMonitor Kubernetes**

```yaml
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: order-service-monitor
spec:
  selector:
    matchLabels:
      app: order-service
  endpoints:
  - port: metrics
    path: /actuator/prometheus
    interval: 30s
```

---

**6. Queries Prometheus**

```promql
# Request rate
rate(orders_created_total[5m])

# Average processing time
rate(orders_processing_time_seconds_sum[5m]) / 
rate(orders_processing_time_seconds_count[5m])

# P95 latency
histogram_quantile(0.95, 
  rate(orders_processing_time_seconds_bucket[5m])
)

# Orders pending
orders_pending
```

---

**Types de métriques :**

| Type | Usage | Exemple |
|------|-------|---------|
| **Counter** | Valeur qui augmente | `orders_created_total` |
| **Gauge** | Valeur instantanée | `orders_pending` |
| **Timer** | Distribution durées | `orders_processing_time` |
| **Summary** | Distribution valeurs | `orders_amount` |

---

## 🔧 PERFORMANCE

### Q5 : Comment investiguer un memory leak dans une application Java sur Kubernetes ?

**Réponse :**

**1. Identifier le problème**

```bash
# Utilisation mémoire
kubectl top pods

# Output:
# NAME                    MEMORY
# order-service-xxx       1950Mi  # ← Proche limite !

# Historique restarts
kubectl describe pod order-service-xxx | grep "Last State"
# Last State: Terminated
#   Reason: OOMKilled
#   Exit Code: 137
```

---

**2. Générer Heap Dump**

**A. Automatique lors OOMKill**

```yaml
env:
- name: JAVA_OPTS
  value: |
    -XX:+HeapDumpOnOutOfMemoryError
    -XX:HeapDumpPath=/tmp/heapdump.hprof
    -XX:+ExitOnOutOfMemoryError

volumeMounts:
- name: heapdumps
  mountPath: /tmp

volumes:
- name: heapdumps
  emptyDir: {}
```

**B. Manuellement**

```bash
# Générer heap dump
kubectl exec order-service-xxx -- jmap -dump:format=b,file=/tmp/heapdump.hprof 1

# Copier localement
kubectl cp order-service-xxx:/tmp/heapdump.hprof ./heapdump.hprof
```

---

**3. Analyser avec Eclipse MAT**

```
Ouvrir heapdump.hprof dans MAT

Leak Suspects Report:
┌─────────────────────────────────────────────┐
│ 1,200 MB (65%) retained by:                 │
│ java.util.ArrayList                         │
│ ← com.example.service.OrderCache           │
│ ← Static field                              │
│                                             │
│ 1,500,000 instances of Order               │
│ Never cleared from cache!                   │
└─────────────────────────────────────────────┘
```

---

**4. Analyser avec jmap**

```bash
# Histogram
kubectl exec order-service-xxx -- jmap -histo:live 1 | head -n 30

# Output:
#  num     #instances         #bytes  class name
#    1:       1500000      180000000  com.example.model.Order
#    2:        800000       96000000  java.lang.String

# GC stats
kubectl exec order-service-xxx -- jstat -gc 1 1000 10

# Old Gen plein = problème !
```

---

**5. Causes courantes**

**A. Cache non boundé**

```java
// ❌ Mauvais
private static final Map<Long, Order> cache = new HashMap<>();

public Order getOrder(Long id) {
    Order cached = cache.get(id);
    if (cached == null) {
        cached = orderRepository.findById(id).orElseThrow();
        cache.put(id, cached);  // Jamais nettoyé !
    }
    return cached;
}

// ✅ Bon
private final LoadingCache<Long, Order> cache;

public OrderService() {
    this.cache = Caffeine.newBuilder()
        .maximumSize(10_000)
        .expireAfterWrite(Duration.ofHours(1))
        .build(id -> orderRepository.findById(id).orElseThrow());
}
```

---

**B. Static collections**

```java
// ❌ Mauvais
private static final List<Order> processedOrders = new ArrayList<>();

// ✅ Bon
@Component
public class OrderProcessor {
    private final List<Order> processedOrders = new ArrayList<>();
}
```

---

**C. Listeners jamais removed**

```java
// ❌ Mauvais
private final List<OrderListener> listeners = new ArrayList<>();

public void addListener(OrderListener listener) {
    listeners.add(listener);  // Jamais removed !
}

// ✅ Bon
private final Set<OrderListener> listeners = 
    Collections.newSetFromMap(new WeakHashMap<>());

public void removeListener(OrderListener listener) {
    listeners.remove(listener);
}
```

---

**6. Solutions**

```yaml
# Augmenter limits (temporaire)
resources:
  limits:
    memory: "4Gi"

# JVM tuning
env:
- name: JAVA_OPTS
  value: |
    -Xms1g -Xmx3g
    -XX:+UseG1GC
    -XX:MaxGCPauseMillis=200
```

**Fixer le leak :**
- Utiliser caches boundés (Caffeine)
- Nettoyer listeners
- Éviter static collections
- Fermer streams/resources

---

**7. Monitoring continu**

```promql
# Heap usage %
jvm_memory_used_bytes{area="heap"} / 
jvm_memory_max_bytes{area="heap"} * 100

# GC time
rate(jvm_gc_pause_seconds_sum[5m])
```

---

## ⚡ STARTUP

### Q6 : Votre app Spring Boot met 2 minutes à démarrer, causant des timeouts. Comment résolvez-vous ?

**Réponse :**

**Solutions :**

---

**1. Startup Probe (Recommandé)**

```yaml
containers:
- name: app
  
  startupProbe:
    httpGet:
      path: /actuator/health
      port: 8080
    initialDelaySeconds: 10
    periodSeconds: 10
    failureThreshold: 20      # 20 * 10s = 200s

  readinessProbe:
    httpGet:
      path: /actuator/health/readiness
      port: 8080
    initialDelaySeconds: 0
    periodSeconds: 5
  
  livenessProbe:
    httpGet:
      path: /actuator/health/liveness
      port: 8080
    initialDelaySeconds: 0
    periodSeconds: 10
```

---

**2. Lazy initialization**

```yaml
# application.yml
spring:
  main:
    lazy-initialization: true
```

**Impact :** Réduit démarrage de 30-50%

---

**3. Désactiver features inutiles**

```yaml
spring:
  jmx:
    enabled: false
  devtools:
    restart:
      enabled: false
  autoconfigure:
    exclude:
      - org.springframework.boot.autoconfigure.jdbc.DataSourceAutoConfiguration
```

---

**4. Async initialization**

```java
@Configuration
@EnableAsync
public class AsyncConfig {
    @Bean
    public Executor taskExecutor() {
        ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(4);
        executor.initialize();
        return executor;
    }
}

@Component
public class DataLoader {
    
    @Async
    @EventListener(ApplicationReadyEvent.class)
    public void loadData() {
        // Chargement en background
        referenceDataService.loadAll();
    }
}
```

---

**5. JVM tuning**

```yaml
env:
- name: JAVA_OPTS
  value: |
    -Xms1g -Xmx2g
    -XX:+TieredCompilation
    -XX:TieredStopAtLevel=1
    -XX:+UseParallelGC
```

---

**Comparaison solutions :**

| Solution | Réduction | Complexité |
|----------|-----------|------------|
| Startup Probe | 0% (+ temps) | Faible ✅ |
| Lazy init | 30-50% | Faible ✅ |
| Async init | 20-40% | Moyenne ✅ |
| JVM tuning | 10-20% | Faible ✅ |

---

## 🔗 TRACING

### Q7 : Comment mettez-vous en place du distributed tracing avec OpenTelemetry/Jaeger ?

**Réponse :**

**Architecture :**
```
Spring Boot → OpenTelemetry → Jaeger Collector → Jaeger UI
```

---

**1. Dépendances**

```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-sleuth</artifactId>
</dependency>

<dependency>
    <groupId>io.micrometer</groupId>
    <artifactId>micrometer-tracing-bridge-otel</artifactId>
</dependency>
```

---

**2. Configuration**

```yaml
# application.yml
spring:
  sleuth:
    sampler:
      probability: 1.0  # 100% dev, 0.1 prod

management:
  tracing:
    enabled: true
    sampling:
      probability: 1.0
  otlp:
    tracing:
      endpoint: http://jaeger-collector:4317
```

---

**3. Code avec tracing**

```java
@Service
public class OrderService {
    
    @Autowired
    private Tracer tracer;
    
    @NewSpan("order.create")
    public Order createOrder(OrderRequest request) {
        
        // Span validation
        Span validationSpan = tracer.spanBuilder("order.validation")
            .startSpan();
        
        try (Scope scope = tracer.withSpan(validationSpan)) {
            validateOrder(request);
            validationSpan.tag("validation", "success");
        } finally {
            validationSpan.end();
        }
        
        Order order = orderRepository.save(new Order(request));
        
        // Span payment
        Span paymentSpan = tracer.spanBuilder("payment.process")
            .startSpan();
        
        try (Scope scope = tracer.withSpan(paymentSpan)) {
            PaymentResponse payment = paymentClient.processPayment(order);
            paymentSpan.tag("paymentId", payment.getId());
        } catch (Exception e) {
            paymentSpan.tag("error", "true");
            throw e;
        } finally {
            paymentSpan.end();
        }
        
        return order;
    }
}
```

---

**4. Visualisation Jaeger**

```
Trace: abc123def456
Duration: 245ms

┌─ order-service: POST /api/orders (245ms)
│  ├─ order.validation (5ms)
│  ├─ orderRepository.save (15ms)
│  ├─ payment.process (200ms)
│  │  └─ payment-service: POST /api/payments
│  └─ orderRepository.save (25ms)
```

---

**5. Correlation logs ↔ traces**

```java
@Component
public class TraceIdMDCFilter extends OncePerRequestFilter {
    
    @Autowired
    private Tracer tracer;
    
    @Override
    protected void doFilterInternal(HttpServletRequest request,
                                   HttpServletResponse response,
                                   FilterChain filterChain) {
        
        Span currentSpan = tracer.currentSpan();
        if (currentSpan != null) {
            TraceContext context = currentSpan.context();
            MDC.put("traceId", context.traceIdString());
        }
        
        try {
            filterChain.doFilter(request, response);
        } finally {
            MDC.clear();
        }
    }
}
```

**Logs avec trace ID :**
```json
{
  "level": "INFO",
  "message": "Creating order",
  "traceId": "abc123def456",
  "spanId": "789012ghi345"
}
```

---

## 📝 LOG OPTIMIZATION

### Q8 : Comment gérez-vous les logs multi-lignes (stack traces Java) dans Kubernetes ?

**Réponse :**

**Problème :** Stack traces splitées en plusieurs lignes

---

**Solution 1 : Logback JSON**

```xml
<configuration>
    <appender name="JSON_CONSOLE" class="ch.qos.logback.core.ConsoleAppender">
        <encoder class="net.logstash.logback.encoder.LogstashEncoder">
            <throwableConverter class="net.logstash.logback.stacktrace.ShortenedThrowableConverter">
                <maxDepthPerThrowable>30</maxDepthPerThrowable>
            </throwableConverter>
        </encoder>
    </appender>
</configuration>
```

**Output :**
```json
{
  "level": "ERROR",
  "message": "Exception occurred",
  "stack_trace": "java.lang.NullPointerException...\n\tat com.example..."
}
```

---

**Solution 2 : Promtail multiline**

```yaml
pipeline_stages:
# Regrouper stack traces
- multiline:
    firstline: '^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}'
    max_wait_time: 3s

- json:
    expressions:
      exception: stack_trace
```

---

### Q9 : Quelle est votre stratégie pour monitorer et alerter sur les erreurs applicatives ?

**Réponse :**

**Architecture :**
```
App → Metrics → Prometheus → AlertManager → PagerDuty/Slack
    → Logs → Loki → Grafana Alerts
```

---

**1. Métriques d'erreurs**

```java
@Service
public class OrderService {
    
    public Order createOrder(OrderRequest request) {
        try {
            Order order = processOrder(request);
            
            meterRegistry.counter("orders.created",
                "status", "success").increment();
            
            return order;
            
        } catch (ValidationException e) {
            meterRegistry.counter("orders.created",
                "status", "failed",
                "error_type", "validation").increment();
            throw e;
            
        } catch (Exception e) {
            meterRegistry.counter("orders.created",
                "status", "failed",
                "error_type", "unexpected").increment();
            throw e;
        }
    }
}
```

---

**2. Prometheus Alerts**

```yaml
groups:
- name: order-service-errors
  rules:
  
  # High error rate
  - alert: HighOrderErrorRate
    expr: |
      sum(rate(orders_created_total{status="failed"}[5m])) /
      sum(rate(orders_created_total[5m])) > 0.05
    for: 5m
    labels:
      severity: warning
    annotations:
      summary: "High error rate (>5%)"
  
  # Critical error rate
  - alert: CriticalOrderErrorRate
    expr: |
      sum(rate(orders_created_total{status="failed"}[5m])) /
      sum(rate(orders_created_total[5m])) > 0.25
    for: 2m
    labels:
      severity: critical
    annotations:
      summary: "CRITICAL: >25% errors"
```

---

**3. AlertManager**

```yaml
route:
  routes:
  # Critical → PagerDuty
  - match:
      severity: critical
    receiver: 'pagerduty'
  
  # Warning → Slack
  - match:
      severity: warning
    receiver: 'slack-warnings'

receivers:
- name: 'pagerduty'
  pagerduty_configs:
  - service_key: '<key>'

- name: 'slack-warnings'
  slack_configs:
  - api_url: '<webhook>'
    channel: '#alerts'
```

---

**4. Log-based alerts (Loki)**

```yaml
# Alert: ERROR logs spike
- alert: ErrorLogsSpike
  expr: |
    sum(rate({app="order-service"} | json | level="ERROR" [5m])) > 10
  for: 5m
  annotations:
    summary: "{{ $value }} error logs/s"
```

---

### Q10 : Comment réduire le volume de logs en production (sampling) ?

**Réponse :**

**Solutions :**

---

**1. Sampling programmatique**

```java
@Component
public class LogSampler {
    
    private final Random random = new Random();
    private final double samplingRate;
    
    public LogSampler(@Value("${logging.sampling.rate:0.1}") double samplingRate) {
        this.samplingRate = samplingRate;
    }
    
    public boolean shouldLog() {
        return random.nextDouble() < samplingRate;
    }
}

@Service
@Slf4j
public class OrderService {
    
    @Autowired
    private LogSampler logSampler;
    
    public void processOrder(Order order) {
        // Log critique : toujours
        log.info("Processing order: {}", order.getId());
        
        // Log verbeux : sampled (10%)
        if (logSampler.shouldLog()) {
            log.debug("Order details: {}", order);
        }
    }
}
```

---

**2. Configuration par environnement**

```yaml
# application-dev.yml
logging:
  level:
    root: DEBUG
  sampling:
    rate: 1.0  # 100% en dev

# application-prod.yml
logging:
  level:
    root: WARN
    com.example.myapp: INFO
  sampling:
    rate: 0.1  # 10% en prod
```

---

**3. Filtering au niveau Promtail**

```yaml
pipeline_stages:
# Drop logs DEBUG
- match:
    selector: '{level="DEBUG"}'
    action: drop

# Sample 10% des logs INFO
- match:
    selector: '{level="INFO"}'
    stages:
    - sampling:
        rate: 0.1

# Garder tous WARN/ERROR
- match:
    selector: '{level=~"WARN|ERROR"}'
    action: keep

# Drop health checks
- match:
    selector: '{message=~".*actuator/health.*"}'
    action: drop
```

---

**4. Filtering par endpoint**

```java
@Component
public class RequestLoggingFilter extends OncePerRequestFilter {
    
    private static final Set<String> NOISY_ENDPOINTS = Set.of(
        "/actuator/health",
        "/actuator/metrics",
        "/favicon.ico"
    );
    
    @Override
    protected void doFilterInternal(HttpServletRequest request,
                                   HttpServletResponse response,
                                   FilterChain filterChain) {
        
        String uri = request.getRequestURI();
        
        // Skip logging pour endpoints bruyants
        if (NOISY_ENDPOINTS.stream().anyMatch(uri::startsWith)) {
            filterChain.doFilter(request, response);
            return;
        }
        
        // Log normal
        log.info("Request: {} {}", request.getMethod(), uri);
    }
}
```

---

**5. Sampling basé sur trace ID**

```java
public boolean shouldLog() {
    Span currentSpan = tracer.currentSpan();
    if (currentSpan == null) return true;
    
    String traceId = currentSpan.context().traceIdString();
    
    // Sample basé sur dernier digit
    int lastDigit = Integer.parseInt(traceId.substring(traceId.length() - 1), 16);
    return lastDigit < 2;  // 12.5% sampling
}
```

**Avantage :** Tous les logs d'une même trace ensemble

---

**6. Conditional logging**

```java
public Order createOrder(OrderRequest request) {
    // Logs détaillés seulement pour gros montants
    boolean detailedLogging = request.getAmount().compareTo(new BigDecimal("1000")) > 0;
    
    if (detailedLogging) {
        log.info("High value order: {}", request);
    }
    
    return processOrder(request);
}
```

---

**7. Retention policies**

```yaml
# Loki
limits_config:
  retention_period: 744h  # 31 jours

# Par niveau
# DEBUG : 7 jours
# INFO : 31 jours
# ERROR : 90 jours
```

---

**Stratégie par environnement :**

| Env | Log Level | Sampling | Retention |
|-----|-----------|----------|-----------|
| **Dev** | DEBUG | 100% | 7 jours |
| **Staging** | INFO | 50% | 14 jours |
| **Prod** | WARN | 10% | 31 jours |
| **Prod (ERROR)** | ERROR | 100% | 90 jours |

---

## 🔐 SECURITY

### Q11 : Comment gérez-vous les logs sensibles (PII, secrets) dans Kubernetes ?

**Réponse :**

**Solutions de sécurité :**

---

**1. Masking au niveau applicatif**

```java
@Component
public class SensitiveDataMasker {
    
    private static final Pattern EMAIL_PATTERN = 
        Pattern.compile("([a-zA-Z0-9._%+-]+)@([a-zA-Z0-9.-]+\\.[a-zA-Z]{2,})");
    
    private static final Pattern CREDIT_CARD_PATTERN = 
        Pattern.compile("\\b\\d{4}[\\s-]?\\d{4}[\\s-]?\\d{4}[\\s-]?\\d{4}\\b");
    
    public String mask(String input) {
        if (input == null) return null;
        
        String result = input;
        
        // Mask email: john.doe@example.com → j***@example.com
        result = EMAIL_PATTERN.matcher(result).replaceAll(mr -> {
            String local = mr.group(1);
            String domain = mr.group(2);
            return local.charAt(0) + "***@" + domain;
        });
        
        // Mask credit card
        result = CREDIT_CARD_PATTERN.matcher(result)
            .replaceAll("****-****-****-****");
        
        return result;
    }
}

@RestController
public class OrderController {
    
    @Autowired
    private SensitiveDataMasker masker;
    
    @PostMapping("/orders")
    public ResponseEntity<Order> createOrder(@RequestBody OrderRequest request) {
        
        // ✅ Mask avant log
        log.info("Creating order for: {}", masker.mask(request.getEmail()));
        
        return ResponseEntity.ok(orderService.createOrder(request));
    }
}
```

---

**2. DTOs pour logging**

```java
// ❌ Mauvais
@Entity
public class User {
    private String email;
    private String password;  // ← Sensible !
    
    @Override
    public String toString() {
        return "User{email=" + email + ", password=" + password + "}";
    }
}

@Service
public class UserService {
    public User createUser(User user) {
        log.info("Creating user: {}", user);  // ← DANGER !
        return userRepository.save(user);
    }
}

// ✅ Bon
public class UserLogDto {
    private String email;
    private String userId;
    // Pas de password !
    
    public static UserLogDto from(User user) {
        return new UserLogDto(
            maskEmail(user.getEmail()),
            user.getId()
        );
    }
}

@Service
public class UserService {
    public User createUser(User user) {
        log.info("Creating user: {}", UserLogDto.from(user));  // ✅ Safe
        return userRepository.save(user);
    }
}
```

---

**3. @JsonIgnore**

```java
@Entity
public class User {
    
    private String email;
    
    @JsonIgnore  // Jamais sérialisé
    private String password;
    
    @JsonIgnore
    private String apiToken;
    
    @JsonProperty(access = JsonProperty.Access.WRITE_ONLY)
    private String creditCard;
}
```

---

**4. Filtering Promtail**

```yaml
pipeline_stages:
# Replace sensitive patterns
- replace:
    expression: '(password|token|apiKey)=\S+'
    replace: '$1=***REDACTED***'

- replace:
    expression: '\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}\b'
    replace: '***@***.***'

- replace:
    expression: '\b\d{4}[-\s]?\d{4}[-\s]?\d{4}[-\s]?\d{4}\b'
    replace: '****-****-****-****'

# Drop logs avec secrets
- match:
    selector: '{message=~".*AWS_SECRET_ACCESS_KEY.*"}'
    action: drop
```

---

**5. RBAC sur logs**

```yaml
# Limiter accès logs en prod
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: log-reader
  namespace: prod
rules:
# Peut lire logs
- apiGroups: [""]
  resources: ["pods/log"]
  verbs: ["get", "list"]

# ❌ Pas d'exec (éviter dump secrets)
```

---

**6. Secrets via SecretKeyRef**

```yaml
# ❌ Mauvais
env:
- name: DATABASE_PASSWORD
  value: "SuperSecret123"  # ← Visible kubectl describe !

# ✅ Bon
env:
- name: DATABASE_PASSWORD
  valueFrom:
    secretKeyRef:
      name: db-credentials
      key: password
```

---

**Checklist sécurité :**

```yaml
log-security:
  prevention:
    ✓ Masking automatique (email, phone, CC)
    ✓ DTOs dédiés pour logging
    ✓ @JsonIgnore sur champs sensibles
    ✓ Pas de log passwords/tokens
  
  detection:
    ✓ Audit régulier logs
    ✓ Patterns détection (regex)
    ✓ Alerts données sensibles
  
  filtering:
    ✓ Promtail masking
    ✓ Drop logs secrets
  
  access-control:
    ✓ RBAC sur pods/log
    ✓ Pas d'exec prod
    ✓ Least privilege
```

---

## 🚀 PERFORMANCE

### Q12 : Comment debuggez-vous des problèmes de performance (latence élevée) ?

**Réponse :**

**Approche systématique :**

---

**1. Métriques globales**

```promql
# P95 latency
histogram_quantile(0.95, 
  rate(http_server_requests_seconds_bucket{app="order-service"}[5m])
)

# Si > 1s → Problème !
```

---

**2. Analyser traces (Jaeger)**

```
Trace total: 2500ms

┌─ POST /api/orders (2500ms)
│  ├─ order.validation (5ms)
│  ├─ database.findUser (50ms)
│  ├─ payment-service (2000ms) ← SLOW!
│  │  ├─ fraud-check (1800ms) ← ROOT CAUSE
│  │  └─ payment-gateway (200ms)
│  └─ database.insert (45ms)
```

**Root cause :** `fraud-check` prend 72% du temps

---

**3. Profiler Java (JFR)**

```bash
# Activer JFR
kubectl exec order-service-xxx -- \
  jcmd 1 JFR.start duration=60s filename=/tmp/recording.jfr

# Copier
kubectl cp order-service-xxx:/tmp/recording.jfr ./recording.jfr

# Analyser avec JMC
```

**Analyse :**
```
Hot Methods:
1. FraudCheckService.check() - 45% CPU
2. HttpClient.execute() - 30% CPU

Full GC: 2 pauses, avg 500ms ← Problème !
```

---

**4. N+1 Query Problem**

```bash
# Activer SQL logging
kubectl logs order-service-xxx | grep "Hibernate:"

# Output:
# select * from orders where user_id = ?
# select * from order_items where order_id = ?  # ← N fois !
# select * from order_items where order_id = ?
```

**Solution :**

```java
// ❌ Mauvais : N+1
@Entity
public class Order {
    @OneToMany(mappedBy = "order")
    private List<OrderItem> items;  // Lazy
}

public List<OrderDto> getOrders() {
    List<Order> orders = orderRepository.findAll();  // 1 query
    return orders.stream()
        .map(o -> new OrderDto(o, o.getItems()))  // N queries !
        .collect(Collectors.toList());
}

// ✅ Bon : Fetch join
@Query("SELECT o FROM Order o LEFT JOIN FETCH o.items")
List<Order> findAllWithItems();
```

---

**5. CPU throttling**

```bash
kubectl top pods
# order-service-xxx    950m  # ← Proche limite 1000m !

# Vérifier throttling
kubectl exec order-service-xxx -- cat /sys/fs/cgroup/cpu/cpu.stat
# nr_throttled 50000  # ← Throttled 50% !
```

**Solution :**
```yaml
resources:
  limits:
    cpu: "2000m"  # ✅ Augmenté
```

---

**6. Latence réseau**

```bash
kubectl exec order-service-xxx -- sh

# Test latency
time curl http://payment-service:8080/health
# real 0m2.5s  # ← 2.5s pour health check !

# Service lui-même lent
```

---

**7. Métriques par dépendance**

```java
@Service
public class OrderService {
    
    public Order createOrder(OrderRequest request) {
        
        // Mesurer database
        Timer.Sample dbSample = Timer.start(meterRegistry);
        Order order = orderRepository.save(new Order(request));
        dbSample.stop(Timer.builder("order.database.save").register(meterRegistry));
        
        // Mesurer fraud check
        Timer.Sample fraudSample = Timer.start(meterRegistry);
        FraudCheckResult fraud = fraudCheckService.check(order);
        fraudSample.stop(Timer.builder("order.fraud.check").register(meterRegistry));
        
        // Mesurer payment
        Timer.Sample paymentSample = Timer.start(meterRegistry);
        PaymentResponse payment = paymentClient.processPayment(order);
        paymentSample.stop(Timer.builder("order.payment.process").register(meterRegistry));
        
        return order;
    }
}
```

**Queries :**
```promql
histogram_quantile(0.95, rate(order_database_save_seconds_bucket[5m]))
# → 50ms

histogram_quantile(0.95, rate(order_fraud_check_seconds_bucket[5m]))
# → 1800ms ← BOTTLENECK !

histogram_quantile(0.95, rate(order_payment_process_seconds_bucket[5m]))
# → 200ms
```

---

**8. Optimisations**

**A. Asynchrone**

```java
// ❌ Avant : Synchrone (2.5s)
public Order createOrder(OrderRequest request) {
    Order order = orderRepository.save(new Order(request));
    FraudCheckResult fraud = fraudCheckService.check(order);  // 1.8s
    PaymentResponse payment = paymentClient.processPayment(order);  // 0.2s
    return order;
}

// ✅ Après : Asynchrone (0.3s)
public Order createOrder(OrderRequest request) {
    Order order = new Order(request);
    order.setStatus(OrderStatus.PENDING);
    order = orderRepository.save(order);
    
    CompletableFuture.runAsync(() -> {
        FraudCheckResult fraud = fraudCheckService.check(order);
        if (fraud.isPassed()) {
            PaymentResponse payment = paymentClient.processPayment(order);
            order.setStatus(OrderStatus.CONFIRMED);
        }
        orderRepository.save(order);
    });
    
    return order;  // Retour immédiat
}
```

**B. Cache**

```java
@Service
public class FraudCheckService {
    
    private final LoadingCache<String, FraudCheckResult> cache;
    
    @PostConstruct
    public void init() {
        this.cache = Caffeine.newBuilder()
            .maximumSize(10_000)
            .expireAfterWrite(Duration.ofMinutes(5))
            .build(userId -> performFraudCheck(userId));
    }
    
    public FraudCheckResult check(Order order) {
        // Cache hit : 1ms au lieu de 1800ms !
        return cache.get(order.getUserId());
    }
}
```

**C. Connection pool**

```yaml
spring:
  datasource:
    hikari:
      maximum-pool-size: 20  # Augmenté
      minimum-idle: 10
```

---

**Résultats :**

| Métrique | Avant | Après | Amélioration |
|----------|-------|-------|--------------|
| P95 latency | 2500ms | 450ms | -82% |
| Throughput | 50 req/s | 200 req/s | +300% |
| Error rate | 5% | 0.5% | -90% |

---

## 🔄 GRACEFUL SHUTDOWN

### Q13 : Comment gérez-vous le graceful shutdown d'une application Spring Boot dans Kubernetes ?

**Réponse :**

**Problème :** Sans graceful shutdown, requêtes en cours coupées lors rolling update

---

**1. Configuration Spring Boot**

```yaml
# application.yml
server:
  shutdown: graceful  # ← Important !

spring:
  lifecycle:
    timeout-per-shutdown-phase: 30s

management:
  endpoint:
    health:
      probes:
        enabled: true
```

---

**2. Kubernetes Deployment**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
spec:
  template:
    spec:
      terminationGracePeriodSeconds: 60  # ← Important !
      
      containers:
      - name: app
        image: order-service:1.0
        
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          periodSeconds: 5
          failureThreshold: 1  # Réagit vite
        
        # PreStop hook
        lifecycle:
          preStop:
            exec:
              command: ["/bin/sh", "-c", "sleep 10"]
```

---

**3. Séquence de shutdown**

```
1. kubectl delete pod order-service-xxx
   ↓
2. K8s retire Pod des endpoints
   ↓
3. preStop hook : sleep 10s
   (Laisse temps à K8s de propager)
   ↓
4. SIGTERM envoyé
   ↓
5. Spring Boot graceful shutdown
   - Arrête nouvelles requêtes
   - Readiness probe DOWN
   - Attend requêtes en cours (max 30s)
   ↓
6. Si pas terminé après 60s
   → SIGKILL (force)
```

---

**4. Code Java**

```java
@RestController
public class OrderController {
    
    private final AtomicInteger activeRequests = new AtomicInteger(0);
    
    @PostMapping("/orders")
    public ResponseEntity<Order> createOrder(@RequestBody OrderRequest request) {
        activeRequests.incrementAndGet();
        
        try {
            log.info("Processing order, active: {}", activeRequests.get());
            return ResponseEntity.ok(orderService.createOrder(request));
        } finally {
            activeRequests.decrementAndGet();
        }
    }
    
    @PreDestroy
    public void onShutdown() {
        log.info("Shutdown initiated, waiting for {} requests", activeRequests.get());
        
        long startTime = System.currentTimeMillis();
        while (activeRequests.get() > 0) {
            if (System.currentTimeMillis() - startTime > 30000) {
                log.warn("Timeout, {} requests still active", activeRequests.get());
                break;
            }
            
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }
        
        log.info("All requests completed");
    }
}
```

---

**5. Health indicator shutdown**

```java
@Component
public class ShutdownHealthIndicator implements HealthIndicator {
    
    private volatile boolean shuttingDown = false;
    
    @Override
    public Health health() {
        if (shuttingDown) {
            return Health.down()
                .withDetail("reason", "Shutting down")
                .build();
        }
        return Health.up().build();
    }
    
    @EventListener(ContextClosedEvent.class)
    public void onShutdown() {
        log.info("Marking as shutting down");
        shuttingDown = true;
        
        // Attendre que readiness détecte
        try {
            Thread.sleep(5000);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}
```

---

**6. Vérification**

```bash
# Terminal 1: Load test
while true; do
  curl -X POST http://order-service/api/orders;
  sleep 0.1;
done

# Terminal 2: Rolling update
kubectl rollout restart deployment/order-service

# Terminal 3: Observer logs
kubectl logs -f order-service-xxx

# Output:
# Processing order, active: 5
# Received SIGTERM
# Marking as shutting down
# Readiness probe DOWN
# Waiting for 5 requests
# Active: 3
# Active: 1
# Active: 0
# All requests completed
# Graceful shutdown done
```

**✅ Aucune erreur côté client !**

---

**7. Best Practices**

✅ **terminationGracePeriodSeconds > shutdown timeout**
```yaml
terminationGracePeriodSeconds: 60
spring.lifecycle.timeout-per-shutdown-phase: 30s
# 60 > 30 ✅
```

✅ **preStop hook avec sleep**
```yaml
lifecycle:
  preStop:
    exec:
      command: ["/bin/sh", "-c", "sleep 10"]
```

✅ **Readiness réagit vite**
```yaml
readinessProbe:
  periodSeconds: 5
  failureThreshold: 1
```

---

**Timeline exemple :**
```
T=0s    : kubectl delete pod
T=0s    : K8s retire du Service
T=0s    : preStop: sleep 10s
T=10s   : SIGTERM
T=10s   : Spring shutdown
T=10s   : Readiness DOWN
T=15s   : Requêtes terminées
T=15s   : Exit 0

✅ Zero downtime !
```

---

## 🎯 Récapitulatif

### Toutes les questions couvertes :

1. ✅ **Q1** : Collecte logs Spring Boot
2. ✅ **Q2** : Différences Liveness/Readiness/Startup
3. ✅ **Q3** : Debug CrashLoopBackOff
4. ✅ **Q4** : Métriques custom Prometheus
5. ✅ **Q5** : Investigation memory leak
6. ✅ **Q6** : App lente au démarrage
7. ✅ **Q7** : Distributed tracing
8. ✅ **Q8** : Logs multi-lignes
9. ✅ **Q9** : Stratégie monitoring/alerting
10. ✅ **Q10** : Log sampling/filtering
11. ✅ **Q11** : Sécurité logs (PII/secrets)
12. ✅ **Q12** : Debug problèmes performance
13. ✅ **Q13** : Graceful shutdown

### Points clés maîtrisés :

**Observability :**
- 📊 Logs structurés (JSON) + aggregation (Loki)
- 📈 Métriques custom (Prometheus/Micrometer)
- 🔗 Distributed tracing (OpenTelemetry/Jaeger)
- 🔍 Correlation logs ↔ traces

**Debugging :**
- 🐛 CrashLoopBackOff (logs, describe, exec)
- 💾 Memory leak (heap dump, MAT, JFR)
- ⚡ Performance (tracing, profiling, N+1 queries)
- 🔧 Outils (kubectl, jmap, jstat, JFR)

**Production Ready :**
- ✅ Health checks (3 probes)
- 🚀 Graceful shutdown
- 🔐 Sécurité logs (masking PII)
- 📉 Log optimization (sampling)
- 🚨 Alerting (Prometheus + AlertManager)

**Prêt pour l'entretien ! 🎯**

---