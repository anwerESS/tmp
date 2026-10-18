## [**..**](./00_index.md)


# Chapitre 11 : Spring Boot & Java sur Kubernetes

## 11.1. Containerization d'Applications Spring Boot

### Création d'un Dockerfile optimisé

**Dockerfile multi-stage avec Layered JARs :**

```dockerfile
# ==========================================
# Stage 1 : Build avec Maven
# ==========================================
FROM maven:3.9-eclipse-temurin-17 AS builder

WORKDIR /app

# Copier pom.xml en premier (layer cache optimisation)
COPY pom.xml .
COPY .mvn .mvn

# Télécharger dependencies (rarement modifié, bien caché)
RUN mvn dependency:go-offline -B

# Copier le code source
COPY src ./src

# Build l'application
RUN mvn clean package -DskipTests -B

# Extraire les layers Spring Boot
WORKDIR /app/target
RUN java -Djarmode=layertools -jar *.jar extract

# ==========================================
# Stage 2 : Runtime optimisé
# ==========================================
FROM eclipse-temurin:17-jre-alpine

# Métadata
LABEL maintainer="dev-team@company.com"
LABEL version="1.0"
LABEL description="Spring Boot application"

# Créer user non-root (sécurité)
RUN addgroup -g 1001 -S appuser && \
    adduser -u 1001 -S appuser -G appuser

WORKDIR /app

# Copier les layers dans l'ordre optimal
# (les moins susceptibles de changer en premier)
COPY --from=builder --chown=appuser:appuser /app/target/dependencies/ ./
COPY --from=builder --chown=appuser:appuser /app/target/spring-boot-loader/ ./
COPY --from=builder --chown=appuser:appuser /app/target/snapshot-dependencies/ ./
COPY --from=builder --chown=appuser:appuser /app/target/application/ ./

# Switch vers user non-root
USER appuser

# Variables d'environnement par défaut
ENV JAVA_OPTS="-XX:+UseContainerSupport \
               -XX:MaxRAMPercentage=75.0 \
               -XX:InitialRAMPercentage=50.0 \
               -XX:+UseG1GC \
               -XX:MaxGCPauseMillis=200 \
               -XX:+UseStringDeduplication \
               -XX:+ParallelRefProcEnabled \
               -Djava.security.egd=file:/dev/./urandom \
               -Dfile.encoding=UTF-8"

ENV SPRING_PROFILES_ACTIVE=prod

# Port exposé
EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=60s --retries=3 \
    CMD wget --no-verbose --tries=1 --spider http://localhost:8080/actuator/health/liveness || exit 1

# Entry point
ENTRYPOINT ["sh", "-c", "java $JAVA_OPTS org.springframework.boot.loader.JarLauncher"]
```

---

### Configuration des Layers Spring Boot

**pom.xml configuration :**

```xml
<build>
    <plugins>
        <plugin>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-maven-plugin</artifactId>
            <version>3.2.0</version>
            <configuration>
                <!-- Activer layering -->
                <layers>
                    <enabled>true</enabled>
                    <configuration>${project.basedir}/src/layers.xml</configuration>
                </layers>
            </configuration>
        </plugin>
    </plugins>
</build>
```

**layers.xml personnalisé :**

```xml
<layers xmlns="http://www.springframework.org/schema/boot/layers"
        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xsi:schemaLocation="http://www.springframework.org/schema/boot/layers
                            https://www.springframework.org/schema/boot/layers/layers-2.7.xsd">
    <application>
        <into layer="spring-boot-loader">
            <include>org/springframework/boot/loader/**</include>
        </into>
        <into layer="application"/>
    </application>
    <dependencies>
        <!-- Dependencies snapshot (changent souvent) -->
        <into layer="snapshot-dependencies">
            <include>*:*:*SNAPSHOT</include>
        </into>
        <!-- Dependencies internes société -->
        <into layer="company-dependencies">
            <include>com.company:*:*</include>
        </into>
        <!-- Toutes les autres dependencies (rarement modifiées) -->
        <into layer="dependencies"/>
    </dependencies>
    <layerOrder>
        <layer>dependencies</layer>
        <layer>spring-boot-loader</layer>
        <layer>snapshot-dependencies</layer>
        <layer>company-dependencies</layer>
        <layer>application</layer>
    </layerOrder>
</layers>
```

**Vérifier les layers :**

```bash
# Extraire et visualiser les layers
java -Djarmode=layertools -jar target/myapp.jar list

# Output:
# dependencies
# spring-boot-loader
# snapshot-dependencies
# company-dependencies
# application
```

---

### .dockerignore pour optimisation

```dockerignore
# Build artifacts
target/
!target/*.jar
*.class

# IDE files
.idea/
.vscode/
*.iml
*.ipr
*.iws
.project
.classpath
.settings/

# Version control
.git/
.gitignore
.gitattributes

# Maven
.mvn/wrapper/maven-wrapper.jar
mvn
mvnw
mvnw.cmd

# Logs
*.log
logs/

# OS files
.DS_Store
Thumbs.db

# Documentation
README.md
docs/
*.md

# Tests
src/test/

# Other
node_modules/
.env
.env.local
```

---

### Build et test local

```bash
# Build avec Maven
mvn clean package

# Build image Docker
docker build -t my-spring-app:latest .

# Tester localement
docker run -p 8080:8080 \
  -e SPRING_PROFILES_ACTIVE=dev \
  -e SPRING_DATASOURCE_URL=jdbc:postgresql://host.docker.internal:5432/mydb \
  my-spring-app:latest

# Vérifier health
curl http://localhost:8080/actuator/health
```

---

## 11.2. Configuration Externalisée (Spring Cloud Kubernetes)

### Installation Spring Cloud Kubernetes

**pom.xml :**

```xml
<dependencies>
    <!-- Spring Boot Starter -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-web</artifactId>
    </dependency>
    
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-actuator</artifactId>
    </dependency>

    <!-- Spring Cloud Kubernetes -->
    <dependency>
        <groupId>org.springframework.cloud</groupId>
        <artifactId>spring-cloud-starter-kubernetes-fabric8-config</artifactId>
        <version>3.1.0</version>
    </dependency>
    
    <dependency>
        <groupId>org.springframework.cloud</groupId>
        <artifactId>spring-cloud-starter-kubernetes-fabric8</artifactId>
        <version>3.1.0</version>
    </dependency>
</dependencies>
```

---

### Configuration avec ConfigMaps

**application.yml :**

```yaml
spring:
  application:
    name: my-spring-app
  
  cloud:
    kubernetes:
      # Activer reload automatique des ConfigMaps
      reload:
        enabled: true
        mode: polling
        period: 5000  # Check every 5s
        strategy: refresh  # ou restart_context
      
      # Configuration des ConfigMaps
      config:
        enabled: true
        sources:
          # ConfigMap avec même nom que l'application
          - name: ${spring.application.name}
            namespace: production
          # ConfigMap partagé
          - name: common-config
            namespace: production
      
      # Secrets
      secrets:
        enabled: true
        sources:
          - name: ${spring.application.name}-secret
            namespace: production

# Actuator endpoints
management:
  endpoints:
    web:
      exposure:
        include: health,info,metrics,prometheus,refresh,env
  endpoint:
    health:
      probes:
        enabled: true
      show-details: when-authorized
```

---

**ConfigMap Kubernetes :**

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: my-spring-app
  namespace: production
  labels:
    app: my-spring-app
data:
  application.yaml: |
    server:
      port: 8080
      compression:
        enabled: true
    
    spring:
      datasource:
        url: jdbc:postgresql://postgres-service:5432/mydb
        hikari:
          maximum-pool-size: 10
          minimum-idle: 5
          connection-timeout: 30000
      
      jpa:
        hibernate:
          ddl-auto: validate
        show-sql: false
        properties:
          hibernate:
            format_sql: false
            dialect: org.hibernate.dialect.PostgreSQLDialect
      
      redis:
        host: redis-service
        port: 6379
        timeout: 2000ms
    
    logging:
      level:
        root: INFO
        com.company.myapp: DEBUG
    
    # Custom properties
    app:
      features:
        new-payment-gateway: true
        enhanced-search: false
      cache:
        ttl: 3600
```

---

**Secret Kubernetes :**

```yaml
apiVersion: v1
kind: Secret
metadata:
  name: my-spring-app-secret
  namespace: production
type: Opaque
stringData:
  database-username: "app_user"
  database-password: "SuperSecurePassword123!"
  redis-password: "RedisPass456!"
  jwt-secret: "MyJWTSecretKey789"
```

**Utilisation dans Spring Boot :**

```java
@Configuration
public class DataSourceConfig {
    
    @Value("${spring.datasource.url}")
    private String url;
    
    @Value("${database-username}")  // Depuis Secret
    private String username;
    
    @Value("${database-password}")  // Depuis Secret
    private String password;
    
    @Bean
    public DataSource dataSource() {
        HikariConfig config = new HikariConfig();
        config.setJdbcUrl(url);
        config.setUsername(username);
        config.setPassword(password);
        return new HikariDataSource(config);
    }
}
```

---

### Reload automatique de configuration

**Avec @RefreshScope :**

```java
@RestController
@RefreshScope  // Cette bean sera recréée quand config change
public class FeatureController {
    
    @Value("${app.features.new-payment-gateway}")
    private boolean newPaymentGatewayEnabled;
    
    @Value("${app.cache.ttl}")
    private int cacheTtl;
    
    @GetMapping("/features")
    public Map<String, Object> getFeatures() {
        return Map.of(
            "newPaymentGateway", newPaymentGatewayEnabled,
            "cacheTtl", cacheTtl
        );
    }
}

// Après modification du ConfigMap, appeler:
// POST http://localhost:8080/actuator/refresh
// Les valeurs @RefreshScope seront mises à jour !
```

---

**Avec @ConfigurationProperties (recommandé) :**

```java
@Configuration
@ConfigurationProperties(prefix = "app.features")
@RefreshScope
public class FeatureProperties {
    
    private boolean newPaymentGateway;
    private boolean enhancedSearch;
    
    // Getters & Setters
    public boolean isNewPaymentGateway() {
        return newPaymentGateway;
    }
    
    public void setNewPaymentGateway(boolean newPaymentGateway) {
        this.newPaymentGateway = newPaymentGateway;
    }
}

@Service
public class PaymentService {
    
    @Autowired
    private FeatureProperties featureProperties;
    
    public void processPayment() {
        if (featureProperties.isNewPaymentGateway()) {
            // New implementation
        } else {
            // Old implementation
        }
    }
}
```

---

### RBAC nécessaire pour Spring Cloud Kubernetes

**ServiceAccount :**

```yaml
apiVersion: v1
kind: ServiceAccount
metadata:
  name: my-spring-app-sa
  namespace: production
---
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: configmap-reader
  namespace: production
rules:
- apiGroups: [""]
  resources: ["configmaps", "secrets"]
  verbs: ["get", "list", "watch"]
---
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: my-spring-app-configmap-reader
  namespace: production
subjects:
- kind: ServiceAccount
  name: my-spring-app-sa
  namespace: production
roleRef:
  kind: Role
  name: configmap-reader
  apiGroup: rbac.authorization.k8s.io
```

**Deployment avec ServiceAccount :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
  namespace: production
spec:
  replicas: 3
  selector:
    matchLabels:
      app: my-spring-app
  template:
    metadata:
      labels:
        app: my-spring-app
    spec:
      serviceAccountName: my-spring-app-sa  # Important !
      containers:
      - name: app
        image: my-spring-app:v1.0.0
        ports:
        - containerPort: 8080
        env:
        - name: SPRING_CLOUD_KUBERNETES_CONFIG_NAMESPACE
          valueFrom:
            fieldRef:
              fieldPath: metadata.namespace
```

---

## 11.3. Graceful Shutdown

### Configuration Spring Boot

**application.yml :**

```yaml
server:
  port: 8080
  shutdown: graceful  # Activation graceful shutdown

spring:
  lifecycle:
    timeout-per-shutdown-phase: 30s  # Temps max pour terminer les requêtes
```

---

### Configuration au niveau code

**Listener pour gérer le shutdown :**

```java
@Component
@Slf4j
public class GracefulShutdownListener {
    
    @PreDestroy
    public void onShutdown() {
        log.info("Application is shutting down gracefully...");
        
        // Fermer connexions, flush caches, etc.
        cleanupResources();
        
        log.info("Graceful shutdown completed");
    }
    
    private void cleanupResources() {
        // Close database connections pool
        // Flush Redis cache
        // Complete pending tasks
        // etc.
    }
}
```

---

**Custom handler pour requêtes en cours :**

```java
@Component
public class RequestTracker {
    
    private final AtomicInteger activeRequests = new AtomicInteger(0);
    
    @EventListener
    public void onShutdown(ContextClosedEvent event) {
        log.info("Waiting for {} active requests to complete", 
                 activeRequests.get());
        
        // Attendre max 30s que les requêtes se terminent
        int waited = 0;
        while (activeRequests.get() > 0 && waited < 30000) {
            try {
                Thread.sleep(100);
                waited += 100;
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }
        
        log.info("Shutdown complete. {} requests were still active", 
                 activeRequests.get());
    }
}

@Component
public class RequestCounterInterceptor implements HandlerInterceptor {
    
    @Autowired
    private RequestTracker requestTracker;
    
    @Override
    public boolean preHandle(HttpServletRequest request, 
                            HttpServletResponse response, 
                            Object handler) {
        requestTracker.activeRequests.incrementAndGet();
        return true;
    }
    
    @Override
    public void afterCompletion(HttpServletRequest request, 
                               HttpServletResponse response, 
                               Object handler, 
                               Exception ex) {
        requestTracker.activeRequests.decrementAndGet();
    }
}
```

---

### Configuration Kubernetes pour Graceful Shutdown

**Deployment avec lifecycle hooks :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  replicas: 3
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxSurge: 1
      maxUnavailable: 0  # Zero downtime
  template:
    spec:
      containers:
      - name: app
        image: my-spring-app:v1.0.0
        ports:
        - containerPort: 8080
        
        # Lifecycle hooks
        lifecycle:
          preStop:
            exec:
              command:
              - sh
              - -c
              - |
                # 1. Arrêter de recevoir nouveau trafic
                echo "Entering PreStop hook"
                
                # 2. Attendre que le endpoint soit retiré du Service
                sleep 10
                
                # 3. Notifier Spring Boot de commencer le shutdown
                curl -X POST http://localhost:8080/actuator/shutdown || true
                
                # 4. Attendre que les requêtes en cours se terminent
                sleep 20
                
                echo "PreStop hook completed"
        
        # Probes
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8080
          initialDelaySeconds: 60
          periodSeconds: 10
          timeoutSeconds: 5
          failureThreshold: 3
        
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          initialDelaySeconds: 30
          periodSeconds: 5
          timeoutSeconds: 3
          failureThreshold: 3
        
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
      
      # Grace period pour termination
      terminationGracePeriodSeconds: 60
```

---

### Sequence complète du Graceful Shutdown

```
1. Kubernetes envoie SIGTERM au pod
   ↓
2. PreStop hook s'exécute
   ↓
   2a. Sleep 10s (laisser le Service retirer l'endpoint)
   ↓
   2b. POST /actuator/shutdown (trigger Spring Boot shutdown)
   ↓
   2c. Sleep 20s (attendre fin des requêtes)
   ↓
3. Spring Boot refuse nouvelles requêtes
   ↓
4. Spring Boot termine les requêtes en cours (max 30s)
   ↓
5. @PreDestroy hooks s'exécutent
   ↓
6. Application se ferme proprement
   ↓
7. Si pas terminé après 60s (terminationGracePeriodSeconds)
   → Kubernetes envoie SIGKILL (force kill)
```

---

### Activer endpoint shutdown (avec sécurité)

**application.yml :**

```yaml
management:
  endpoint:
    shutdown:
      enabled: true  # ⚠️ Attention : désactiver en prod !
  endpoints:
    web:
      exposure:
        include: health,info,metrics,shutdown
  
  # Sécuriser avec authentification
  security:
    enabled: true
```

**Sécuriser avec Spring Security :**

```java
@Configuration
@EnableWebSecurity
public class ActuatorSecurityConfig {
    
    @Bean
    public SecurityFilterChain actuatorSecurity(HttpSecurity http) throws Exception {
        http
            .requestMatchers("/actuator/shutdown")
            .authenticated()
            .and()
            .httpBasic();
        return http.build();
    }
}
```

---

## 11.4. JVM Tuning pour Containers

### Flags JVM essentiels pour Kubernetes

**Flags de base :**

```bash
JAVA_OPTS="
  # ===== Container awareness =====
  -XX:+UseContainerSupport
  -XX:MaxRAMPercentage=75.0
  -XX:InitialRAMPercentage=50.0
  -XX:MinRAMPercentage=25.0
  
  # ===== Garbage Collector =====
  -XX:+UseG1GC
  -XX:MaxGCPauseMillis=200
  -XX:G1ReservePercent=10
  -XX:+ParallelRefProcEnabled
  
  # ===== String optimizations =====
  -XX:+UseStringDeduplication
  -XX:+OptimizeStringConcat
  
  # ===== Performance =====
  -XX:+TieredCompilation
  -XX:TieredStopAtLevel=1
  -Djava.security.egd=file:/dev/./urandom
  
  # ===== Debugging (dev only) =====
  -XX:+HeapDumpOnOutOfMemoryError
  -XX:HeapDumpPath=/tmp/heapdump.hprof
  -XX:+PrintGCDetails
  -XX:+PrintGCDateStamps
  -Xlog:gc*:file=/tmp/gc.log:time,uptime:filecount=5,filesize=10M
  
  # ===== Encoding =====
  -Dfile.encoding=UTF-8
  
  # ===== Network =====
  -Djava.net.preferIPv4Stack=true
"
```

---

### Configuration selon taille de containers

**Small container (512MB RAM, 0.5 CPU) :**

```yaml
resources:
  requests:
    memory: "512Mi"
    cpu: "500m"
  limits:
    memory: "512Mi"
    cpu: "500m"

env:
- name: JAVA_OPTS
  value: "-XX:+UseContainerSupport 
          -XX:MaxRAMPercentage=75.0 
          -XX:+UseSerialGC 
          -Xss256k"
```

**Medium container (1GB RAM, 1 CPU) :**

```yaml
resources:
  requests:
    memory: "1Gi"
    cpu: "1000m"
  limits:
    memory: "1Gi"
    cpu: "1000m"

env:
- name: JAVA_OPTS
  value: "-XX:+UseContainerSupport 
          -XX:MaxRAMPercentage=75.0 
          -XX:+UseG1GC 
          -XX:MaxGCPauseMillis=200"
```

**Large container (4GB RAM, 2 CPU) :**

```yaml
resources:
  requests:
    memory: "4Gi"
    cpu: "2000m"
  limits:
    memory: "4Gi"
    cpu: "2000m"

env:
- name: JAVA_OPTS
  value: "-XX:+UseContainerSupport 
          -XX:MaxRAMPercentage=75.0 
          -XX:+UseG1GC 
          -XX:MaxGCPauseMillis=100 
          -XX:+ParallelRefProcEnabled 
          -XX:ConcGCThreads=2"
```

---

### Choix du Garbage Collector

**Serial GC (containers très petits < 512MB) :**

```bash
-XX:+UseSerialGC
# Avantages : Faible overhead, simple
# Inconvénients : Stop-the-world complet
# Use case : Micro-services légers
```

**G1 GC (recommandé, 512MB - 8GB) :**

```bash
-XX:+UseG1GC
-XX:MaxGCPauseMillis=200
-XX:G1HeapRegionSize=16m
-XX:InitiatingHeapOccupancyPercent=45
# Avantages : Pauses prévisibles, bon compromis
# Inconvénients : Overhead léger
# Use case : 95% des cas
```

**ZGC (containers larges > 8GB) :**

```bash
-XX:+UseZGC
-XX:ZCollectionInterval=5
# Avantages : Pauses < 10ms, scalable
# Inconvénients : Java 17+ requis, overhead mémoire
# Use case : Applications critiques, faible latence
```

**Shenandoah GC (alternative à ZGC) :**

```bash
-XX:+UseShenandoahGC
# Avantages : Pauses ultra-courtes
# Inconvénients : Pas dans OpenJDK standard
# Use case : Latency-sensitive apps
```

---

### Monitoring JVM dans Kubernetes

**Exposition métriques JVM via Actuator :**

```yaml
# application.yml
management:
  metrics:
    enable:
      jvm: true
      process: true
      system: true
    export:
      prometheus:
        enabled: true
    tags:
      application: ${spring.application.name}
      environment: ${spring.profiles.active}
```

**Métriques JVM importantes :**

```java
@Component
public class JvmMetricsExporter {
    
    private final MeterRegistry registry;
    
    public JvmMetricsExporter(MeterRegistry registry) {
        this.registry = registry;
        
        // Custom metrics
        Gauge.builder("jvm.heap.used.percentage", this::getHeapUsedPercentage)
            .description("Percentage of heap used")
            .register(registry);
        
        Gauge.builder("jvm.gc.pause.max", this::getMaxGcPause)
            .description("Max GC pause time in ms")
            .register(registry);
    }
    
    private double getHeapUsedPercentage() {
        MemoryUsage heapUsage = ManagementFactory.getMemoryMXBean()
            .getHeapMemoryUsage();
        return (double) heapUsage.getUsed() / heapUsage.getMax() * 100;
    }
    
    private double getMaxGcPause() {
        return ManagementFactory.getGarbageCollectorMXBeans().stream()
            .mapToLong(gc -> gc.getCollectionTime())
            .max()
            .orElse(0);
    }
}
```

---

### Debugging OOM (Out Of Memory)

**Heap dump automatique :**

```yaml
env:
- name: JAVA_OPTS
  value: "-XX:+HeapDumpOnOutOfMemoryError 
          -XX:HeapDumpPath=/tmp/heapdump.hprof 
          -XX:+ExitOnOutOfMemoryError"

# Volume pour stocker heap dump
volumeMounts:
- name: heap-dump
  mountPath: /tmp

volumes:
- name: heap-dump
  emptyDir:
    sizeLimit: 2Gi  # Taille suffisante pour heap dump
```

**Récupérer heap dump après OOM :**

```bash
# Lister les pods
kubectl get pods -n production | grep my-app

# Copier heap dump depuis pod crashé
kubectl cp production/my-app-abc123:/tmp/heapdump.hprof ./heapdump.hprof

# Analyser avec Eclipse MAT ou VisualVM
```

---

## 11.5. Actuator Endpoints & Health Checks

### Configuration Actuator complète

**pom.xml :**

```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-actuator</artifactId>
</dependency>

<!-- Pour métriques Prometheus -->
<dependency>
    <groupId>io.micrometer</groupId>
    <artifactId>micrometer-registry-prometheus</artifactId>
</dependency>
```

---

**application.yml :**

```yaml
management:
  # Changer le port management (optionnel)
  server:
    port: 9090  # Séparé du port applicatif
  
  # Endpoints à exposer
  endpoints:
    web:
      base-path: /actuator
      exposure:
        include: health,info,metrics,prometheus,env,loggers,threaddump,heapdump
        exclude: shutdown  # Désactiver en prod
  
  # Configuration Health
  endpoint:
    health:
      probes:
        enabled: true  # Active /health/liveness et /health/readiness
      show-details: when-authorized
      show-components: when-authorized
      group:
        liveness:
          include: livenessState,ping
        readiness:
          include: readinessState,db,redis
  
  # Configuration Info
  info:
    env:
      enabled: true
    git:
      mode: full
    build:
      enabled: true
  
  # Métriques
  metrics:
    enable:
      jvm: true
      process: true
      system: true
      http: true
      logback: true
    distribution:
      percentiles-histogram:
        http.server.requests: true
    tags:
      application: ${spring.application.name}
      environment: ${spring.profiles.active}
    export:
      prometheus:
        enabled: true

# Info application
info:
  app:
    name: ${spring.application.name}
    description: My Spring Boot Application
    version: @project.version@
    encoding: @project.build.sourceEncoding@
    java:
      version: @java.version@
```

---

### Health Indicators personnalisés

**Custom Health Indicator :**

```java
@Component
public class ExternalApiHealthIndicator implements HealthIndicator {
    
    @Autowired
    private RestTemplate restTemplate;
    
    @Value("${external.api.url}")
    private String externalApiUrl;
    
    @Override
    public Health health() {
        try {
            // Vérifier si l'API externe répond
            ResponseEntity<String> response = restTemplate.getForEntity(
                externalApiUrl + "/health", 
                String.class
            );
            
            if (response.getStatusCode().is2xxSuccessful()) {
                return Health.up()
                    .withDetail("external-api", "Available")
                    .withDetail("status", response.getStatusCode())
                    .build();
            } else {
                return Health.down()
                    .withDetail("external-api", "Unavailable")
                    .withDetail("status", response.getStatusCode())
                    .build();
            }
        } catch (Exception e) {
            return Health.down()
                .withDetail("external-api", "Unreachable")
                .withException(e)
                .build();
        }
    }
}
```

**Health Indicator avec Reactive :**

```java
@Component
public class DatabaseHealthIndicator implements ReactiveHealthIndicator {
    
    @Autowired
    private R2dbcEntityTemplate template;
    
    @Override
    public Mono<Health> health() {
        return template.getDatabaseClient()
            .sql("SELECT 1")
            .fetch()
            .one()
            .map(result -> Health.up()
                .withDetail("database", "available")
                .build())
            .onErrorResume(ex -> Mono.just(Health.down()
                .withDetail("database", "unavailable")
                .withException(ex)
                .build()));
    }
}
```

**Health avec timeout :**

```java
@Component
public class SlowServiceHealthIndicator implements HealthIndicator {
    
    @Autowired
    private SlowExternalService slowService;
    
    @Override
    public Health health() {
        try {
            // Timeout de 3 secondes max
            CompletableFuture<Boolean> future = CompletableFuture.supplyAsync(() -> {
                return slowService.isAvailable();
            });
            
            boolean available = future.get(3, TimeUnit.SECONDS);
            
            return available ? 
                Health.up().withDetail("slow-service", "available").build() :
                Health.down().withDetail("slow-service", "unavailable").build();
                
        } catch (TimeoutException e) {
            return Health.down()
                .withDetail("slow-service", "timeout after 3s")
                .build();
        } catch (Exception e) {
            return Health.down()
                .withDetail("slow-service", "error")
                .withException(e)
                .build();
        }
    }
}
```

---

### Liveness vs Readiness Probes

**Liveness Probe** : "L'application est-elle vivante ?"
- Si échec → Kubernetes **redémarre** le pod
- Vérifications : deadlocks, threads bloqués, crash interne

**Readiness Probe** : "L'application est-elle prête à recevoir du trafic ?"
- Si échec → Kubernetes **retire** le pod du Service (pas de redémarrage)
- Vérifications : DB connectée, dépendances disponibles, cache chargé

```java
@Component
public class LivenessStateHealthIndicator implements HealthIndicator {
    
    @Override
    public Health health() {
        // Vérifications simples pour liveness
        // NE PAS vérifier les dépendances externes !
        
        // Vérifier que l'app n'est pas deadlockée
        ThreadMXBean threadBean = ManagementFactory.getThreadMXBean();
        long[] deadlockedThreads = threadBean.findDeadlockedThreads();
        
        if (deadlockedThreads != null && deadlockedThreads.length > 0) {
            return Health.down()
                .withDetail("deadlock", "Detected " + deadlockedThreads.length + " deadlocked threads")
                .build();
        }
        
        return Health.up()
            .withDetail("liveness", "Application is alive")
            .build();
    }
}

@Component
public class ReadinessStateHealthIndicator implements HealthIndicator {
    
    @Autowired
    private DataSource dataSource;
    
    @Autowired
    private RedisTemplate<String, String> redisTemplate;
    
    @Override
    public Health health() {
        Health.Builder builder = new Health.Builder();
        
        // Vérifier DB
        try {
            dataSource.getConnection().close();
            builder.up().withDetail("database", "connected");
        } catch (Exception e) {
            builder.down().withDetail("database", "disconnected");
        }
        
        // Vérifier Redis
        try {
            redisTemplate.opsForValue().get("health-check");
            builder.up().withDetail("redis", "connected");
        } catch (Exception e) {
            builder.down().withDetail("redis", "disconnected");
        }
        
        return builder.build();
    }
}
```

---

### Configuration Kubernetes probes

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  replicas: 3
  template:
    spec:
      containers:
      - name: app
        image: my-spring-app:v1.0.0
        ports:
        - name: http
          containerPort: 8080
        - name: management
          containerPort: 9090
        
        # Startup probe (pour apps qui démarrent lentement)
        startupProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 9090
          initialDelaySeconds: 0
          periodSeconds: 5
          failureThreshold: 30  # 30 * 5s = 150s max pour démarrer
        
        # Liveness probe (détecte si l'app est morte)
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 9090
            scheme: HTTP
          initialDelaySeconds: 60
          periodSeconds: 10
          timeoutSeconds: 5
          successThreshold: 1
          failureThreshold: 3  # 3 échecs = restart
        
        # Readiness probe (détecte si l'app est prête)
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 9090
            scheme: HTTP
          initialDelaySeconds: 30
          periodSeconds: 5
          timeoutSeconds: 3
          successThreshold: 1
          failureThreshold: 3  # 3 échecs = retrait du Service
        
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
```

---

### Métriques Prometheus

**Endpoint /actuator/prometheus :**

```bash
# Scrape par Prometheus
curl http://localhost:9090/actuator/prometheus

# Output exemple:
# HELP jvm_memory_used_bytes The amount of used memory
# TYPE jvm_memory_used_bytes gauge
jvm_memory_used_bytes{area="heap",id="G1 Eden Space",} 5.24288E7
jvm_memory_used_bytes{area="heap",id="G1 Old Gen",} 1.048576E8

# HELP http_server_requests_seconds  
# TYPE http_server_requests_seconds summary
http_server_requests_seconds_count{method="GET",status="200",uri="/api/users",} 1523.0
http_server_requests_seconds_sum{method="GET",status="200",uri="/api/users",} 45.234

# HELP process_cpu_usage The "recent cpu usage" for the JVM process
# TYPE process_cpu_usage gauge
process_cpu_usage 0.15
```

---

**ServiceMonitor pour Prometheus Operator :**

```yaml
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: my-spring-app-metrics
  namespace: production
  labels:
    app: my-spring-app
spec:
  selector:
    matchLabels:
      app: my-spring-app
  endpoints:
  - port: management
    path: /actuator/prometheus
    interval: 30s
    scrapeTimeout: 10s
```

---

**Custom metrics business :**

```java
@Service
public class OrderService {
    
    private final Counter orderCounter;
    private final Timer orderProcessingTimer;
    private final Gauge orderQueueSize;
    
    public OrderService(MeterRegistry registry) {
        // Counter : nombre total de commandes
        this.orderCounter = Counter.builder("orders.created")
            .description("Total number of orders created")
            .tag("type", "order")
            .register(registry);
        
        // Timer : temps de traitement des commandes
        this.orderProcessingTimer = Timer.builder("orders.processing.time")
            .description("Time to process an order")
            .tag("type", "order")
            .register(registry);
        
        // Gauge : taille de la queue
        this.orderQueueSize = Gauge.builder("orders.queue.size", this::getQueueSize)
            .description("Current order queue size")
            .register(registry);
    }
    
    public Order createOrder(OrderRequest request) {
        // Incrémenter counter
        orderCounter.increment();
        
        // Mesurer temps de traitement
        return orderProcessingTimer.record(() -> {
            // Business logic
            Order order = processOrder(request);
            return order;
        });
    }
    
    private int getQueueSize() {
        // Retourner taille actuelle de la queue
        return orderQueue.size();
    }
}
```

---

### Info Endpoint personnalisé

**Git info automatique :**

```xml
<!-- pom.xml -->
<plugin>
    <groupId>pl.project13.maven</groupId>
    <artifactId>git-commit-id-plugin</artifactId>
    <version>4.9.10</version>
    <executions>
        <execution>
            <goals>
                <goal>revision</goal>
            </goals>
        </execution>
    </executions>
    <configuration>
        <generateGitPropertiesFile>true</generateGitPropertiesFile>
        <generateGitPropertiesFilename>
            ${project.build.outputDirectory}/git.properties
        </generateGitPropertiesFilename>
    </configuration>
</plugin>
```

**Custom InfoContributor :**

```java
@Component
public class CustomInfoContributor implements InfoContributor {
    
    @Override
    public void contribute(Info.Builder builder) {
        builder.withDetail("custom", Map.of(
            "team", "Backend Team",
            "contact", "backend@company.com",
            "documentation", "https://docs.company.com"
        ));
        
        builder.withDetail("dependencies", Map.of(
            "database", "PostgreSQL 15",
            "cache", "Redis 7",
            "message-broker", "RabbitMQ 3.12"
        ));
        
        builder.withDetail("features", Map.of(
            "payment-v2", true,
            "enhanced-search", false
        ));
    }
}
```

**Response /actuator/info :**

```json
{
  "app": {
    "name": "my-spring-app",
    "description": "My Spring Boot Application",
    "version": "1.2.3",
    "encoding": "UTF-8",
    "java": {
      "version": "17.0.5"
    }
  },
  "git": {
    "branch": "main",
    "commit": {
      "id": "a3f7b21",
      "time": "2024-11-18T10:30:00Z"
    }
  },
  "build": {
    "artifact": "my-spring-app",
    "name": "my-spring-app",
    "time": "2024-11-18T10:35:00.000Z",
    "version": "1.2.3"
  },
  "custom": {
    "team": "Backend Team",
    "contact": "backend@company.com",
    "documentation": "https://docs.company.com"
  },
  "dependencies": {
    "database": "PostgreSQL 15",
    "cache": "Redis 7",
    "message-broker": "RabbitMQ 3.12"
  },
  "features": {
    "payment-v2": true,
    "enhanced-search": false
  }
}
```

---

## Exemples Complets d'Intégration

### Exemple 1 : Application Spring Boot complète sur Kubernetes

**Structure du projet :**

```
my-spring-app/
├── src/
│   ├── main/
│   │   ├── java/
│   │   │   └── com/company/myapp/
│   │   │       ├── MyApplication.java
│   │   │       ├── config/
│   │   │       │   ├── DatabaseConfig.java
│   │   │       │   ├── RedisConfig.java
│   │   │       │   └── SecurityConfig.java
│   │   │       ├── controller/
│   │   │       │   └── UserController.java
│   │   │       ├── service/
│   │   │       │   └── UserService.java
│   │   │       └── health/
│   │   │           ├── CustomHealthIndicator.java
│   │   │           └── DatabaseHealthIndicator.java
│   │   └── resources/
│   │       ├── application.yml
│   │       ├── application-dev.yml
│   │       ├── application-prod.yml
│   │       └── db/
│   │           └── migration/
│   │               └── V1__initial_schema.sql
│   └── test/
│       └── java/
├── k8s/
│   ├── deployment.yaml
│   ├── service.yaml
│   ├── configmap.yaml
│   ├── secret.yaml
│   ├── ingress.yaml
│   └── hpa.yaml
├── Dockerfile
├── .dockerignore
└── pom.xml
```

---

**MyApplication.java :**

```java
package com.company.myapp;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.cache.annotation.EnableCaching;
import org.springframework.scheduling.annotation.EnableAsync;
import org.springframework.scheduling.annotation.EnableScheduling;

@SpringBootApplication
@EnableCaching
@EnableAsync
@EnableScheduling
public class MyApplication {
    
    public static void main(String[] args) {
        SpringApplication.run(MyApplication.class, args);
    }
}
```

---

**application.yml (commun) :**

```yaml
spring:
  application:
    name: my-spring-app
  
  # JPA
  jpa:
    open-in-view: false
    hibernate:
      ddl-auto: validate
    properties:
      hibernate:
        jdbc:
          batch_size: 20
        order_inserts: true
        order_updates: true

  # Jackson
  jackson:
    serialization:
      write-dates-as-timestamps: false
    time-zone: UTC
    default-property-inclusion: non_null

# Actuator
management:
  server:
    port: 9090
  endpoints:
    web:
      exposure:
        include: health,info,metrics,prometheus
  endpoint:
    health:
      probes:
        enabled: true
      show-details: when-authorized
  metrics:
    export:
      prometheus:
        enabled: true
    tags:
      application: ${spring.application.name}

# Logging
logging:
  pattern:
    console: "%d{yyyy-MM-dd HH:mm:ss} - %logger{36} - %msg%n"
  level:
    root: INFO
    com.company.myapp: DEBUG
```

---

**application-prod.yml :**

```yaml
spring:
  datasource:
    url: jdbc:postgresql://postgres-service:5432/mydb
    username: ${DATABASE_USERNAME}
    password: ${DATABASE_PASSWORD}
    hikari:
      maximum-pool-size: 20
      minimum-idle: 5
      connection-timeout: 30000
      idle-timeout: 600000
      max-lifetime: 1800000
  
  redis:
    host: redis-service
    port: 6379
    password: ${REDIS_PASSWORD}
    timeout: 2000ms
    lettuce:
      pool:
        max-active: 8
        max-idle: 8
        min-idle: 2

logging:
  level:
    root: WARN
    com.company.myapp: INFO

server:
  tomcat:
    threads:
      max: 200
      min-spare: 10
    max-connections: 10000
    accept-count: 100
```

---

**Deployment complet :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
  namespace: production
  labels:
    app: my-spring-app
    version: v1.2.3
spec:
  replicas: 3
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxSurge: 1
      maxUnavailable: 0
  selector:
    matchLabels:
      app: my-spring-app
  template:
    metadata:
      labels:
        app: my-spring-app
        version: v1.2.3
      annotations:
        prometheus.io/scrape: "true"
        prometheus.io/port: "9090"
        prometheus.io/path: "/actuator/prometheus"
    spec:
      serviceAccountName: my-spring-app-sa
      
      # Init container pour migration DB
      initContainers:
      - name: wait-for-postgres
        image: busybox:1.35
        command:
        - sh
        - -c
        - |
          until nc -z postgres-service 5432; do
            echo "Waiting for postgres..."
            sleep 2
          done
          echo "Postgres is ready!"
      
      containers:
      - name: app
        image: registry.company.com/my-spring-app:v1.2.3
        imagePullPolicy: IfNotPresent
        
        ports:
        - name: http
          containerPort: 8080
          protocol: TCP
        - name: management
          containerPort: 9090
          protocol: TCP
        
        env:
        - name: SPRING_PROFILES_ACTIVE
          value: "prod"
        
        - name: DATABASE_USERNAME
          valueFrom:
            secretKeyRef:
              name: db-credentials
              key: username
        
        - name: DATABASE_PASSWORD
          valueFrom:
            secretKeyRef:
              name: db-credentials
              key: password
        
        - name: REDIS_PASSWORD
          valueFrom:
            secretKeyRef:
              name: redis-credentials
              key: password
        
        - name: JAVA_OPTS
          value: "-XX:+UseContainerSupport 
                  -XX:MaxRAMPercentage=75.0 
                  -XX:+UseG1GC 
                  -XX:MaxGCPauseMillis=200 
                  -XX:+HeapDumpOnOutOfMemoryError 
                  -XX:HeapDumpPath=/tmp/heapdump.hprof"
        
        # Probes
        startupProbe:
          httpGet:
            path: /actuator/health/liveness
            port: management
          initialDelaySeconds: 0
          periodSeconds: 5
          failureThreshold: 30
        
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: management
          initialDelaySeconds: 60
          periodSeconds: 10
          timeoutSeconds: 5
          failureThreshold: 3
        
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: management
          initialDelaySeconds: 30
          periodSeconds: 5
          timeoutSeconds: 3
          failureThreshold: 3
        
        # Resources
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
        
        # Lifecycle
        lifecycle:
          preStop:
            exec:
              command:
              - sh
              - -c
              - sleep 10 && curl -X POST http://localhost:9090/actuator/shutdown || true
        
        # Volume mounts
        volumeMounts:
        - name: tmp
          mountPath: /tmp
        - name: logs
          mountPath: /app/logs
      
      volumes:
      - name: tmp
        emptyDir: {}
      - name: logs
        emptyDir: {}
      
      # Security
      securityContext:
        runAsNonRoot: true
        runAsUser: 1001
        fsGroup: 1001
      
      # Affinity
      affinity:
        podAntiAffinity:
          preferredDuringSchedulingIgnoredDuringExecution:
          - weight: 100
            podAffinityTerm:
              labelSelector:
                matchExpressions:
                - key: app
                  operator: In
                  values:
                  - my-spring-app
              topologyKey: kubernetes.io/hostname
      
      terminationGracePeriodSeconds: 60
---
apiVersion: v1
kind: Service
metadata:
  name: my-spring-app
  namespace: production
  labels:
    app: my-spring-app
spec:
  type: ClusterIP
  ports:
  - port: 80
    targetPort: http
    protocol: TCP
    name: http
  - port: 9090
    targetPort: management
    protocol: TCP
    name: management
  selector:
    app: my-spring-app
---
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: my-spring-app-hpa
  namespace: production
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: my-spring-app
  minReplicas: 3
  maxReplicas: 10
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
  - type: Resource
    resource:
      name: memory
      target:
        type: Utilization
        averageUtilization: 80
  behavior:
    scaleDown:
      stabilizationWindowSeconds: 300
      policies:
      - type: Percent
        value: 50
        periodSeconds: 60
    scaleUp:
      stabilizationWindowSeconds: 0
      policies:
      - type: Percent
        value: 100
        periodSeconds: 30
      - type: Pods
        value: 2
        periodSeconds: 30
      selectPolicy: Max
```

---

## Récapitulatif du Chapitre 11

✅ **Points clés :**

**Containerization :**
- Multi-stage Dockerfile avec layered JARs
- Alpine/Distroless pour images légères
- User non-root pour sécurité
- Optimisation du cache Docker

**Configuration :**
- Spring Cloud Kubernetes pour ConfigMaps/Secrets
- @RefreshScope pour reload dynamique
- RBAC nécessaire pour accès API Kubernetes
- Séparation par environnement

**Graceful Shutdown :**
- server.shutdown=graceful
- PreStop hooks Kubernetes
- terminationGracePeriodSeconds
- Gestion des requêtes en cours

**JVM Tuning :**
- -XX:+UseContainerSupport
- -XX:MaxRAMPercentage
- Choix GC selon taille container
- Heap dumps pour debugging OOM

**Actuator & Health :**
- Liveness vs Readiness probes
- Custom Health Indicators
- Métriques Prometheus
- Info endpoint avec Git/Build info

---

## Questions d'entretien fréquentes

1. **Comment optimisez-vous une image Docker Spring Boot ?**
   → Multi-stage build, layered JARs, Alpine/Distroless

2. **Quelle est la différence entre Liveness et Readiness probe ?**
   → Liveness = restart si KO, Readiness = retrait Service si KO

3. **Comment gérez-vous la configuration dans Kubernetes ?**
   → ConfigMaps + Spring Cloud Kubernetes + @RefreshScope

4. **Quels flags JVM utilisez-vous dans des containers ?**
   → UseContainerSupport, MaxRAMPercentage, UseG1GC

5. **Comment implémentez-vous le graceful shutdown ?**
   → server.shutdown=graceful + preStop hook + terminationGracePeriodSeconds

6. **Comment exposez-vous des métriques business ?**
   → Micrometer + custom Counter/Timer/Gauge + /actuator/prometheus

---

## Commandes utiles

```bash
# Build image
docker build -t my-app:v1.0.0 .

# Test local
docker run -p 8080:8080 -e SPRING_PROFILES_ACTIVE=dev my-app:v1.0.0

# Deploy
kubectl apply -f k8s/

# Vérifier health
kubectl exec -it <pod> -- curl http://localhost:9090/actuator/health

# Voir métriques
kubectl port-forward svc/my-spring-app 9090:9090
curl http://localhost:9090/actuator/prometheus

# Logs
kubectl logs -f deployment/my-spring-app

# Heap dump
kubectl exec <pod> -- jcmd 1 GC.heap_dump /tmp/heapdump.hprof
kubectl cp <pod>:/tmp/heapdump.hprof ./heapdump.hprof
```

---

# 🎯 Chapitre 11 : Questions/Réponses d'Entretien
## Spring Boot & Java sur Kubernetes

---

## 📋 Section 1 : Containerization d'Applications Spring Boot

### Q1 : Comment optimisez-vous une image Docker pour une application Spring Boot ?

**Réponse :**

J'utilise plusieurs techniques pour optimiser les images Docker Spring Boot :

**1. Multi-stage build**

Séparation build et runtime pour réduire la taille :

```dockerfile
# Stage 1 : Build (Maven + JDK)
FROM maven:3.9-eclipse-temurin-17 AS builder
WORKDIR /app
COPY pom.xml .
RUN mvn dependency:go-offline
COPY src ./src
RUN mvn clean package -DskipTests

# Stage 2 : Runtime (JRE only)
FROM eclipse-temurin:17-jre-alpine
WORKDIR /app
COPY --from=builder /app/target/*.jar app.jar
ENTRYPOINT ["java", "-jar", "app.jar"]
```

**Résultat :** Image passe de ~650MB (avec JDK) à ~180MB (JRE Alpine)

---

**2. Layered JARs (Spring Boot 2.3+)**

Les dependencies changent rarement, donc on les met dans un layer séparé :

```xml
<!-- pom.xml -->
<plugin>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-maven-plugin</artifactId>
    <configuration>
        <layers>
            <enabled>true</enabled>
        </layers>
    </configuration>
</plugin>
```

```dockerfile
# Extraire les layers
FROM builder AS extractor
WORKDIR /app/target
RUN java -Djarmode=layertools -jar *.jar extract

# Runtime avec layers séparés
FROM eclipse-temurin:17-jre-alpine
WORKDIR /app
COPY --from=extractor /app/target/dependencies/ ./
COPY --from=extractor /app/target/spring-boot-loader/ ./
COPY --from=extractor /app/target/snapshot-dependencies/ ./
COPY --from=extractor /app/target/application/ ./
```

**Avantages :**
- Dependencies (95% du JAR) : mis en cache, rebuild rapide
- Application code (5%) : seule layer rebuildée à chaque changement
- **Build time réduit de 80%** après le premier build

---

**3. Image de base optimale**

Comparaison des options :

| Image de base | Taille | Sécurité | Use case |
|---------------|--------|----------|----------|
| `eclipse-temurin:17` | ~420MB | Moyen | Dev/test |
| `eclipse-temurin:17-jre` | ~280MB | Moyen | Standard |
| `eclipse-temurin:17-jre-alpine` | ~180MB | Bon | Production |
| `gcr.io/distroless/java17` | ~200MB | Excellent | Production sécurisée |

**Mon choix :**
- **Dev/Staging** : Alpine (facile à debugger avec shell)
- **Production** : Distroless (pas de shell = surface d'attaque minimale)

---

**4. User non-root (sécurité)**

```dockerfile
FROM eclipse-temurin:17-jre-alpine

# Créer user dédié
RUN addgroup -g 1001 appuser && \
    adduser -u 1001 -D -G appuser appuser

WORKDIR /app
COPY --chown=appuser:appuser app.jar .

USER appuser

ENTRYPOINT ["java", "-jar", "app.jar"]
```

---

**Résultat final :**

| Métrique | Avant | Après | Gain |
|----------|-------|-------|------|
| Taille image | 650 MB | 180 MB | -72% |
| Build time (after 1st) | 8 min | 2 min | -75% |
| Layers Docker | 3-4 | 7-8 | Cache optimal |
| Sécurité | ⚠️ Root user | ✅ Non-root | ✅ |

---

### Q2 : Expliquez les Spring Boot Layered JARs et leur intérêt pour Kubernetes

**Réponse :**

Les **Layered JARs** permettent de diviser un JAR Spring Boot en plusieurs couches (layers) pour optimiser le cache Docker.

**Problème sans layers :**

```
myapp.jar (150 MB)
├── Dependencies: 140 MB (Spring, Hibernate, etc.)
└── Application code: 10 MB

→ Changement dans le code = rebuild des 150 MB !
```

**Solution avec layers :**

```
Layer 1: dependencies/ (140 MB)          ← Rarement modifié
Layer 2: spring-boot-loader/ (200 KB)    ← Jamais modifié
Layer 3: snapshot-dependencies/ (5 MB)   ← Parfois modifié
Layer 4: application/ (10 MB)            ← Souvent modifié

→ Changement code = rebuild seulement Layer 4 (10 MB) !
```

**Vérifier les layers :**

```bash
java -Djarmode=layertools -jar myapp.jar list

# Output:
# dependencies
# spring-boot-loader
# snapshot-dependencies
# application
```

**Dockerfile optimisé :**

```dockerfile
FROM maven:3.9-eclipse-temurin-17 AS builder
WORKDIR /app
COPY pom.xml .
RUN mvn dependency:go-offline
COPY src ./src
RUN mvn package -DskipTests

WORKDIR /app/target
RUN java -Djarmode=layertools -jar *.jar extract

FROM eclipse-temurin:17-jre-alpine
WORKDIR /app

COPY --from=builder /app/target/dependencies/ ./
COPY --from=builder /app/target/spring-boot-loader/ ./
COPY --from=builder /app/target/snapshot-dependencies/ ./
COPY --from=builder /app/target/application/ ./

ENTRYPOINT ["java", "org.springframework.boot.loader.JarLauncher"]
```

**Exemple concret :**

```
Déploiement v1.0.0 → v1.0.1 (changement code seulement)

Sans layers:
- Download 180 MB par node
- 3 nodes × 180 MB = 540 MB transférés
- Temps: ~2 minutes

Avec layers:
- Download 10 MB par node (layer application seulement)
- 3 nodes × 10 MB = 30 MB transférés
- Temps: ~15 secondes

→ 18x plus rapide !
```

---

### Q3 : Alpine vs Distroless : lequel choisir pour production ?

**Réponse :**

**Alpine Linux :**

**Avantages :**
- ✅ **Très léger** : ~180 MB pour JRE
- ✅ **Shell disponible** : `sh` pour debugging
- ✅ **Package manager** : `apk add` pour installer outils
- ✅ **Facilité de debug** : `kubectl exec -it pod -- sh`

**Inconvénients :**
- ⚠️ **Surface d'attaque** : Shell + package manager = risques sécurité
- ⚠️ **musl libc** : Incompatibilités possibles (vs glibc standard)

**Quand utiliser :**
- Environnements dev/staging
- Applications nécessitant debugging fréquent

---

**Distroless (Google) :**

**Avantages :**
- ✅ **Sécurité maximale** : Pas de shell, pas de package manager
- ✅ **Surface d'attaque minimale** : Seulement JRE + dépendances runtime
- ✅ **Conformité** : Recommandé pour environnements réglementés

**Inconvénients :**
- ❌ **Debugging complexe** : Pas de `kubectl exec`
- ❌ **Taille légèrement plus grande** : ~200 MB

**Quand utiliser :**
- **Production** (recommandé)
- Applications critiques (finance, santé)

---

**Comparaison :**

| Image | Taille | Shell | Package manager | Libc |
|-------|--------|-------|-----------------|------|
| `eclipse-temurin:17-jre-alpine` | ~180 MB | ✅ | ✅ apk | musl |
| `gcr.io/distroless/java17` | ~200 MB | ❌ | ❌ | glibc |

**Ma stratégie :**

```yaml
# Dev/Staging: Alpine
dev:
  image: my-app:alpine

# Production: Distroless
prod:
  image: my-app:distroless
```

**Debugging avec Distroless (Kubernetes 1.23+) :**

```bash
kubectl debug my-pod -it --image=busybox --target=app
```

---

## 📋 Section 2 : Configuration Externalisée

### Q4 : Comment gérez-vous la configuration d'une application Spring Boot dans Kubernetes ?

**Réponse :**

J'utilise une approche **multi-niveau** combinant plusieurs mécanismes :

**1. ConfigMaps pour configuration non-sensible**

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: my-app-config
  namespace: production
data:
  application.yaml: |
    spring:
      datasource:
        url: jdbc:postgresql://postgres-service:5432/mydb
        hikari:
          maximum-pool-size: 20
    
    logging:
      level:
        root: INFO
        com.company: DEBUG
    
    app:
      features:
        new-payment: true
```

**Montage dans le pod :**

```yaml
spec:
  containers:
  - name: app
    volumeMounts:
    - name: config
      mountPath: /config
      readOnly: true
    env:
    - name: SPRING_CONFIG_LOCATION
      value: "classpath:/application.yml,file:/config/application.yaml"
  volumes:
  - name: config
    configMap:
      name: my-app-config
```

---

**2. Secrets pour données sensibles**

```yaml
apiVersion: v1
kind: Secret
metadata:
  name: my-app-secret
  namespace: production
type: Opaque
stringData:
  database-username: "app_user"
  database-password: "SuperSecurePassword123!"
```

**Injection comme variables d'environnement :**

```yaml
spec:
  containers:
  - name: app
    env:
    - name: SPRING_DATASOURCE_USERNAME
      valueFrom:
        secretKeyRef:
          name: my-app-secret
          key: database-username
    - name: SPRING_DATASOURCE_PASSWORD
      valueFrom:
        secretKeyRef:
          name: my-app-secret
          key: database-password
```

---

**3. Spring Cloud Kubernetes (méthode avancée)**

**pom.xml :**

```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-kubernetes-fabric8-config</artifactId>
    <version>3.1.0</version>
</dependency>
```

**application.yml :**

```yaml
spring:
  application:
    name: my-app
  
  cloud:
    kubernetes:
      reload:
        enabled: true
        mode: polling
        period: 5000
      config:
        enabled: true
        sources:
          - name: my-app-config
      secrets:
        enabled: true
        sources:
          - name: my-app-secret
```

**RBAC nécessaire :**

```yaml
apiVersion: v1
kind: ServiceAccount
metadata:
  name: my-app-sa
---
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: configmap-reader
rules:
- apiGroups: [""]
  resources: ["configmaps", "secrets"]
  verbs: ["get", "list", "watch"]
---
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: my-app-binding
subjects:
- kind: ServiceAccount
  name: my-app-sa
roleRef:
  kind: Role
  name: configmap-reader
  apiGroup: rbac.authorization.k8s.io
```

---

### Q5 : Comment implémentez-vous le reload dynamique de configuration sans redémarrer les pods ?

**Réponse :**

J'utilise **Spring Cloud Kubernetes** avec **@RefreshScope**.

**Configuration :**

```yaml
spring:
  cloud:
    kubernetes:
      reload:
        enabled: true
        mode: polling
        period: 5000
        strategy: refresh
```

**Code application :**

```java
@RestController
@RefreshScope
public class FeatureController {
    
    @Value("${app.features.new-payment}")
    private boolean newPaymentEnabled;
    
    @GetMapping("/features")
    public Map<String, Object> getFeatures() {
        return Map.of("newPayment", newPaymentEnabled);
    }
}
```

**Test du reload :**

```bash
# 1. Vérifier config actuelle
curl http://my-app/features
# {"newPayment":false}

# 2. Modifier ConfigMap
kubectl edit configmap my-app-config

# 3. Attendre 5 secondes

# 4. Vérifier nouvelle config
curl http://my-app/features
# {"newPayment":true}

# Aucun restart ! 🎉
```

---

## 📋 Section 3 : Graceful Shutdown

### Q6 : Comment implémentez-vous le graceful shutdown d'une application Spring Boot dans Kubernetes ?

**Réponse :**

Le graceful shutdown garantit que les requêtes en cours se terminent proprement.

**Configuration Spring Boot :**

```yaml
server:
  shutdown: graceful

spring:
  lifecycle:
    timeout-per-shutdown-phase: 30s
```

**Configuration Kubernetes :**

```yaml
apiVersion: apps/v1
kind: Deployment
spec:
  template:
    spec:
      containers:
      - name: app
        lifecycle:
          preStop:
            exec:
              command:
              - sh
              - -c
              - |
                echo "PreStop hook started"
                sleep 10
                echo "PreStop hook completed"
        
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
      
      terminationGracePeriodSeconds: 60
```

**Séquence complète :**

```
T+0s   : kubectl delete pod
T+0s   : Readiness probe échoue → Pod retiré du Service
T+0s   : PreStop hook : sleep 10s
T+10s  : SIGTERM envoyé → Spring Boot arrête nouvelles requêtes
T+10s  : Requêtes en cours continuent (max 30s)
T+40s  : Toutes requêtes terminées → Pod supprimé
Si T+60s : SIGKILL (force kill)
```

**Code Java :**

```java
@Component
@Slf4j
public class GracefulShutdownListener {
    
    private final AtomicInteger activeRequests = new AtomicInteger(0);
    
    @PreDestroy
    public void onShutdown() {
        log.info("Shutting down. Active requests: {}", activeRequests.get());
        
        int waited = 0;
        while (activeRequests.get() > 0 && waited < 30000) {
            log.info("Waiting for {} requests", activeRequests.get());
            Thread.sleep(1000);
            waited += 1000;
        }
        
        log.info("Shutdown complete");
    }
}
```

---

## 📋 Section 4 : JVM Tuning

### Q7 : Quels sont les flags JVM essentiels pour Kubernetes ?

**Réponse :**

**Flags essentiels :**

```bash
# Container awareness
-XX:+UseContainerSupport
-XX:MaxRAMPercentage=75.0
-XX:InitialRAMPercentage=50.0

# Garbage Collector
-XX:+UseG1GC
-XX:MaxGCPauseMillis=200

# Optimizations
-XX:+UseStringDeduplication
-Djava.security.egd=file:/dev/./urandom

# Debugging
-XX:+HeapDumpOnOutOfMemoryError
-XX:HeapDumpPath=/tmp/heapdump.hprof
```

**Par taille de container :**

```yaml
# Small (512MB)
env:
- name: JAVA_OPTS
  value: "-XX:+UseContainerSupport 
          -XX:MaxRAMPercentage=75.0 
          -XX:+UseSerialGC"

# Medium (2GB)
env:
- name: JAVA_OPTS
  value: "-XX:+UseContainerSupport 
          -XX:MaxRAMPercentage=75.0 
          -XX:+UseG1GC 
          -XX:MaxGCPauseMillis=200"

# Large (8GB+)
env:
- name: JAVA_OPTS
  value: "-XX:+UseContainerSupport 
          -XX:MaxRAMPercentage=75.0 
          -XX:+UseZGC"
```

---

### Q8 : Comment debuggez-vous un OOMKilled dans Kubernetes ?

**Réponse :**

**Étape 1 : Identifier le problème**

```bash
kubectl get pods
# my-pod  0/1  OOMKilled  3  5m

kubectl describe pod my-pod
# Warning OOMKilled Container exceeded memory limit

kubectl logs my-pod --previous
```

**Étape 2 : Récupérer heap dump**

```bash
kubectl cp my-pod:/tmp/heapdump.hprof ./heapdump.hprof
```

**Étape 3 : Analyser avec Eclipse MAT**

Ouvrir heapdump.hprof dans Eclipse MAT :
- Leak Suspects Report
- Dominator Tree
- Top Consumers

**Causes fréquentes :**

**1. Memory leak :**

```java
// ❌ Problème
private Map<Long, User> cache = new HashMap<>();

public User getUser(Long id) {
    return cache.computeIfAbsent(id, this::loadUser);
}

// ✅ Solution
@Cacheable(value = "users")
public User getUser(Long id) {
    return userRepository.findById(id).orElse(null);
}
```

**2. Query sans pagination :**

```java
// ❌ Problème
List<User> users = userRepository.findAll();

// ✅ Solution
Page<User> users = userRepository.findAll(PageRequest.of(0, 100));
```

---

## 📋 Section 5 : Actuator & Health Checks

### Q9 : Différence entre Liveness et Readiness probes ?

**Réponse :**

**Liveness Probe** : "L'application est-elle vivante ?"

- Si échec → Kubernetes **redémarre** le pod
- Vérifications : deadlocks, freeze interne
- **NE PAS** vérifier dépendances externes

```yaml
livenessProbe:
  httpGet:
    path: /actuator/health/liveness
    port: 8080
  initialDelaySeconds: 60
  periodSeconds: 10
  failureThreshold: 3
```

```java
@Component
public class LivenessHealthIndicator implements HealthIndicator {
    
    @Override
    public Health health() {
        ThreadMXBean threadBean = ManagementFactory.getThreadMXBean();
        long[] deadlocked = threadBean.findDeadlockedThreads();
        
        if (deadlocked != null && deadlocked.length > 0) {
            return Health.down().build();
        }
        
        return Health.up().build();
    }
}
```

---

**Readiness Probe** : "L'application est-elle prête ?"

- Si échec → Kubernetes **retire** du Service (pas de restart)
- Vérifications : DB, Redis, APIs externes

```yaml
readinessProbe:
  httpGet:
    path: /actuator/health/readiness
    port: 8080
  initialDelaySeconds: 30
  periodSeconds: 5
  failureThreshold: 3
```

```java
@Component
public class ReadinessHealthIndicator implements HealthIndicator {
    
    @Autowired
    private DataSource dataSource;
    
    @Override
    public Health health() {
        try (Connection conn = dataSource.getConnection()) {
            if (conn.isValid(2)) {
                return Health.up().build();
            }
        } catch (Exception e) {
            return Health.down().build();
        }
        return Health.down().build();
    }
}
```

---

**Comparaison :**

| Aspect | Liveness | Readiness |
|--------|----------|-----------|
| Question | App vivante ? | App prête ? |
| Échec | Restart pod | Retrait Service |
| Vérifications | Internes | Externes |
| Fréquence | 10s | 5s |

---

### Q10 : Comment exposez-vous des métriques business pour Prometheus ?

**Réponse :**

**Configuration :**

```xml
<dependency>
    <groupId>io.micrometer</groupId>
    <artifactId>micrometer-registry-prometheus</artifactId>
</dependency>
```

```yaml
management:
  endpoints:
    web:
      exposure:
        include: prometheus
  metrics:
    export:
      prometheus:
        enabled: true
```

**Métriques custom :**

```java
@Service
public class OrderService {
    
    private final Counter orderCounter;
    private final Timer orderTimer;
    
    public OrderService(MeterRegistry registry) {
        this.orderCounter = Counter.builder("orders.created")
            .description("Total orders created")
            .register(registry);
        
        this.orderTimer = Timer.builder("orders.processing.time")
            .description("Order processing time")
            .register(registry);
    }
    
    public Order createOrder(OrderRequest request) {
        orderCounter.increment();
        
        return orderTimer.record(() -> {
            return processOrder(request);
        });
    }
}
```

**Endpoint Prometheus :**

```bash
curl http://localhost:8080/actuator/prometheus

# orders_created_total 1523.0
# orders_processing_time_seconds_count 1523.0
# orders_processing_time_seconds_sum 45.234
```

---

## 🎓 Conseils Finaux

**Questions à poser au recruteur :**

1. Quelle version de Java utilisez-vous ?
2. Comment gérez-vous les configurations ?
3. Utilisez-vous Layered JARs ?
4. Quelle stratégie de monitoring (Prometheus, Datadog) ?
5. Comment sont gérés les secrets (External Secrets, Vault) ?

**Points clés à mentionner :**

✅ Layered JARs pour optimisation build
✅ Distroless pour production sécurisée
✅ Graceful shutdown obligatoire
✅ JVM tuning avec UseContainerSupport
✅ Distinction Liveness/Readiness
✅ Métriques business avec Micrometer

**Erreurs à éviter :**

❌ Utiliser -Xmx au lieu de MaxRAMPercentage
❌ Vérifier DB dans Liveness probe
❌ Oublier graceful shutdown
❌ Ne pas prévoir heap dumps
❌ Root user dans containers