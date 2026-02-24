## [**..**](./00_index.md)

## Spring Boot Actuator - Key Points

### **General Concept**
Spring Boot Actuator provides **production-ready features** to monitor and manage a Spring Boot application. It exposes **HTTP endpoints** that give information about the application's state and metrics.

### **Main Endpoints**

| Endpoint | Description |
|----------|-------------|
| `/actuator` | Lists all available endpoints |
| `/actuator/health` | Application health status (UP/DOWN) |
| `/actuator/info` | Application metadata (version, description) |
| `/actuator/metrics` | Performance metrics (memory, CPU, requests) |
| `/actuator/env` | Environment properties and configuration |
| `/actuator/loggers` | Allows dynamic adjustment of log levels |
| `/actuator/beans` | Full list of Spring beans in the application |
| `/actuator/mappings` | Details of Request Mappings (all REST/MVC endpoints) |

### **Configuration (application.yml)**

**Expose endpoints:**
```yaml
management:
  endpoints:
    web:
      exposure:
        include: '*'  # Exposes all endpoints
        # OR specific: include: health, info, metrics
```

**Configure the health endpoint:**
```yaml
management:
  endpoint:
    health:
      show-details: always  # Possible values: never, always, when-authorized
```

**Enable the shutdown endpoint:**
```yaml
management:
  endpoint:
    shutdown:
      enabled: true  # Allows stopping the app via POST /actuator/shutdown

server:
  shutdown: graceful  # Graceful shutdown (finishes ongoing requests)
```

**Configure application info:**
```yaml
management:
  info:
    env:
      enabled: true  # Enables info from properties

info:
  app:
    name: MyNewApp
    description: App built by us
    version: 1.0.0
# Accessible via /actuator/info
```

### **Main Use Cases**

1. **Monitoring** : Check application health in production (`/health`)
2. **Debugging** : See all loaded Spring beans (`/beans`)
3. **Performance** : Analyze metrics (memory, threads, requests) (`/metrics`)
4. **Configuration** : Check active properties (`/env`)
5. **Troubleshooting** : Adjust log levels without restart (`/loggers`)

### **Security**

⚠️ **Important** : By default, only `/health` and `/info` are exposed. Other endpoints can expose sensitive data.

**Best practices:**
- Only expose the necessary endpoints in production
- Secure endpoints with Spring Security
- Use `show-details: when-authorized` for `/health`

```yaml
# Secure example for production
management:
  endpoints:
    web:
      exposure:
        include: health, info, metrics  # Limit endpoints
```

### **Key Points**

- Actuator provides **production-ready features** for monitoring
- Exposes endpoints via `/actuator/*`
- **Health endpoint** : checks if the application is UP/DOWN
- **Metrics endpoint** : gives performance statistics
- **Info endpoint** : shows application metadata
- Configuration via `application.yml` or `application.properties`
- Allows **graceful shutdown** of the application
- Important to **secure endpoints** in production

---

## Questions/Answers - Spring Boot Actuator

### **1. What is Spring Boot Actuator?**

**Answer:**
Spring Boot Actuator is a module that provides **production-ready features** to monitor and manage a Spring Boot application. It exposes **HTTP endpoints** that allow you to:
- Check the application **health**
- View **metrics** (performance, memory, CPU)
- See loaded Spring **beans**
- Analyze endpoint **mappings**
- Modify **log levels** dynamically

It is essential for **monitoring** and **debugging** in production.

---

### **2. What are the main endpoints provided by Actuator?**

**Answer:**
The most important endpoints are:

- **`/actuator`** : Lists all available endpoints
- **`/actuator/health`** : Application health status (UP/DOWN)
- **`/actuator/info`** : Application metadata (version, description)
- **`/actuator/metrics`** : Performance metrics (JVM, HTTP requests, database)
- **`/actuator/env`** : Environment properties and configuration
- **`/actuator/beans`** : Full list of Spring beans
- **`/actuator/mappings`** : All Request Mappings (REST/MVC endpoints)
- **`/actuator/loggers`** : Log level management
- **`/actuator/shutdown`** : Allows stopping the application (must be enabled)

---

### **3. How to expose all Actuator endpoints?**

**Answer:**
By default, only `/health` and `/info` are exposed. To expose all endpoints:

```yaml
management:
  endpoints:
    web:
      exposure:
        include: '*'  # Exposes all endpoints
```

To expose specific endpoints:
```yaml
management:
  endpoints:
    web:
      exposure:
        include: health, info, metrics, beans
```

⚠️ **Warning** : In production, you should limit exposed endpoints for security reasons.

---

### **4. How to configure the health endpoint to show more details?**

**Answer:**
By default, `/actuator/health` only shows the status (UP/DOWN). To see details:

```yaml
management:
  endpoint:
    health:
      show-details: always
```

**Possible values:**
- **`never`** : No details (only UP/DOWN)
- **`always`** : Always show details
- **`when-authorized`** : Show only if the user is authenticated

**Result with `show-details: always`:**
```json
{
  "status": "UP",
  "components": {
    "db": {
      "status": "UP",
      "details": {
        "database": "MySQL",
        "validationQuery": "isValid()"
      }
    },
    "diskSpace": {
      "status": "UP",
      "details": {
        "total": 500GB,
        "free": 200GB
      }
    }
  }
}
```

---

### **5. How to enable the shutdown endpoint?**

**Answer:**
The **shutdown endpoint** allows stopping the application via an HTTP POST request. It is **disabled by default**.

**Configuration:**
```yaml
management:
  endpoint:
    shutdown:
      enabled: true

server:
  shutdown: graceful  # Finishes ongoing requests before stopping
```

**Usage:**
```bash
curl -X POST http://localhost:8080/actuator/shutdown
```

⚠️ **Security** : This endpoint must be **protected** with Spring Security in production!

---

### **6. How to show custom information in /actuator/info?**

**Answer:**
Add properties in `application.yml`:

```yaml
management:
  info:
    env:
      enabled: true  # Enables reading of 'info.*' properties

info:
  app:
    name: MySpringApp
    description: User management application
    version: 1.0.0
    developer: Mohamed
```

**Result of `/actuator/info`:**
```json
{
  "app": {
    "name": "MySpringApp",
    "description": "User management application",
    "version": "1.0.0",
    "developer": "Mohamed"
  }
}
```

---

### **7. How to check an application's metrics?**

**Answer:**
Use `/actuator/metrics` which lists all available metrics:

```bash
GET /actuator/metrics
```

**Result:**
```json
{
  "names": [
    "jvm.memory.used",
    "jvm.threads.live",
    "http.server.requests",
    "system.cpu.usage"
  ]
}
```

To see a specific metric:
```bash
GET /actuator/metrics/jvm.memory.used
GET /actuator/metrics/http.server.requests
```

**Use cases:**
- See used **JVM memory**
- Count the number of **HTTP requests**
- Monitor active **threads**
- Check **CPU** usage

---

### **8. What is the difference between /health and /info?**

**Answer:**
- **`/actuator/health`** : 
  - Shows if the application is working correctly (UP/DOWN)
  - Checks **dependencies**: database, disk space, external services
  - Used for **health checks** (Kubernetes, load balancers)
  
- **`/actuator/info`** :
  - Shows **static metadata** about the application
  - Version, name, description, developer
  - Information configured manually

**Example:**
```json
// /actuator/health
{"status": "UP"}

// /actuator/info
{"app": {"name": "MyApp", "version": "1.0.0"}}
```

---

### **9. How to see all Spring beans loaded in the application?**

**Answer:**
Use the `/actuator/beans` endpoint:

```bash
GET /actuator/beans
```

**Result:** JSON with all beans, their **dependencies**, and their **scope**.

**Use case:** 
- Debugging: check if a bean is properly created
- Understand dependencies between beans
- Identify configuration issues

---

### **10. How to see all REST endpoints of the application?**

**Answer:**
Use `/actuator/mappings` which lists all **Request Mappings**:

```bash
GET /actuator/mappings
```

**Result:** List of all endpoints with:
- HTTP method (GET, POST, etc.)
- URL pattern
- Controller method
- Produces/Consumes

**Use case:**
- Document the API
- Check available routes
- Debugging mappings

---

### **11. How to change log level dynamically without restarting the application?**

**Answer:**
Use the `/actuator/loggers` endpoint:

**See current log level:**
```bash
GET /actuator/loggers/com.example.myapp
```

**Change log level:**
```bash
POST /actuator/loggers/com.example.myapp
Content-Type: application/json

{
  "configuredLevel": "DEBUG"
}
```

**Available levels:** TRACE, DEBUG, INFO, WARN, ERROR, OFF

**Use case:** Enable DEBUG mode in production to investigate a bug without restarting.

---

### **12. How to secure Actuator endpoints?**

**Answer:**
Actuator endpoints can expose **sensitive data**. You need to secure them with **Spring Security**:

```java
@Configuration
public class SecurityConfig {
    
    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
        http
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/actuator/health").permitAll()  // Public
                .requestMatchers("/actuator/**").hasRole("ADMIN") // Admin only
                .anyRequest().authenticated()
            )
            .httpBasic();
        return http.build();
    }
}
```

**Best practices:**
- Only expose `/health` and `/info` publicly
- Protect other endpoints with authentication
- In production, limit exposed endpoints

---

### **13. What is graceful shutdown and how to configure it?**

**Answer:**
**Graceful shutdown** allows stopping the application **cleanly** by first finishing ongoing requests before fully shutting down.

**Configuration:**
```yaml
server:
  shutdown: graceful  # Enables graceful shutdown

spring:
  lifecycle:
    timeout-per-shutdown-phase: 30s  # Max wait time (default: 30s)
```

**Behavior:**
1. Application receives a stop signal
2. No longer accepts new requests
3. Finishes ongoing requests (max 30s)
4. Fully stops

**Use case:** Avoid cutting ongoing transactions during a deployment.

---

### **14. How to check the application's environment properties?**

**Answer:**
Use `/actuator/env`:

```bash
GET /actuator/env
```

**Result:** All properties with their sources:
- `application.yml`
- System properties
- Environment variables
- Command line arguments

**See a specific property:**
```bash
GET /actuator/env/server.port
```

⚠️ **Warning** : Can expose **secrets** (passwords, API keys). Must be secured!

---

### **15. How to monitor application performance with Actuator?**

**Answer:**
Use `/actuator/metrics` to check:

**1. JVM Memory:**
```bash
GET /actuator/metrics/jvm.memory.used
GET /actuator/metrics/jvm.memory.max
```

**2. HTTP Requests:**
```bash
GET /actuator/metrics/http.server.requests
# See count, average time, max
```

**3. CPU Usage:**
```bash
GET /actuator/metrics/system.cpu.usage
```

**4. Threads:**
```bash
GET /actuator/metrics/jvm.threads.live
```

**5. Database Connection Pool:**
```bash
GET /actuator/metrics/hikaricp.connections.active
```

**Integration with external tools:**
- **Prometheus** : scrapes Actuator metrics
- **Grafana** : metrics visualization
- **Micrometer** : underlying metrics framework

---

### **16. What is the Maven/Gradle dependency for Actuator?**

**Answer:**

**Maven:**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-actuator</artifactId>
</dependency>
```

**Gradle:**
```gradle
implementation 'org.springframework.boot:spring-boot-starter-actuator'
```

This is the only dependency needed to enable Actuator.

---

### **17. How to create a custom health indicator?**

**Answer:**
Implement the `HealthIndicator` interface:

```java
@Component
public class CustomHealthIndicator implements HealthIndicator {
    
    @Override
    public Health health() {
        // Check the state of an external service
        boolean isServiceUp = checkExternalService();
        
        if (isServiceUp) {
            return Health.up()
                .withDetail("service", "External API")
                .withDetail("status", "Available")
                .build();
        } else {
            return Health.down()
                .withDetail("service", "External API")
                .withDetail("error", "Service unavailable")
                .build();
        }
    }
    
    private boolean checkExternalService() {
        // Check logic
        return true;
    }
}
```

**Result in `/actuator/health`:**
```json
{
  "status": "UP",
  "components": {
    "custom": {
      "status": "UP",
      "details": {
        "service": "External API",
        "status": "Available"
      }
    }
  }
}
```

---

### **18. What is the difference between Actuator and Spring Admin?**

**Answer:**
- **Spring Boot Actuator** :
  - Provides **HTTP endpoints** for monitoring
  - Built directly into the application
  - Returns raw JSON

- **Spring Boot Admin** :
  - **Graphical web interface** to visualize Actuator data
  - Separate application that connects to Actuator endpoints
  - Dashboard with charts, alerts, notifications
  - Allows monitoring **multiple applications** from one place

**Relationship:** Spring Admin **consumes** Actuator endpoints to display info graphically.

---

### **19. How to filter metrics returned by Actuator?**

**Answer:**
Use configuration to exclude certain metrics:

```yaml
management:
  metrics:
    enable:
      jvm: true
      system: false  # Disables system metrics
```

Or create a custom `MeterFilter`:
```java
@Configuration
public class MetricsConfig {
    
    @Bean
    public MeterFilter customMeterFilter() {
        return MeterFilter.deny(id -> {
            // Filter metrics that start with "jvm.gc"
            return id.getName().startsWith("jvm.gc");
        });
    }
}
```

---

### **20. What are the main use cases for Actuator in production?**

**Answer:**

**1. Health Checks:**
- Kubernetes liveness/readiness probes
- Load balancer health checks

**2. Monitoring & Alerting:**
- Integration with Prometheus/Grafana
- Alerts if memory > 80%

**3. Troubleshooting:**
- Change log level to DEBUG without redeploying
- Analyze loaded beans

**4. Performance Analysis:**
- Identify slow endpoints
- Monitor JVM memory

**5. Audit & Compliance:**
- Track HTTP requests
- See active configurations

**6. Graceful Deployments:**
- Graceful shutdown for zero-downtime deploys

---

**Final tip:** Be ready to explain how you would use Actuator to debug a performance issue in production (e.g.: memory leak). It's a very common practical question! 💪
