## [**..**](./00_index.md)

# Spring Boot & Spring Cloud

## 🚀 Spring Boot - Goals and Philosophy

### The Problem Spring Boot Solves

**Before Spring Boot (classic Spring) :**
```xml
<!-- Massive XML configuration -->
<bean id="dataSource" ...>
<bean id="entityManagerFactory" ...>
<bean id="transactionManager" ...>
<!-- + Tomcat config, dispatcher servlet, view resolver, etc. -->
```
- Verbose and complex XML configuration
- Manual configuration of all components
- Manual dependency and version management
- Deployment on an external server (Tomcat, etc.)
- New project setup time : several hours/days

**With Spring Boot :**
```java
@SpringBootApplication
public class Application {
    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }
}
```
- Minimal configuration (zero XML)
- Intelligent auto-configuration
- Automatic dependency management
- Embedded server (no external deployment)
- Setup time : a few minutes

### Spring Boot Goals

1. **Simplify development** : Eliminate boilerplate configuration
2. **Convention over Configuration** : Intelligent default values
3. **Production-Ready** : Built-in monitoring, health checks, metrics
4. **Microservices-Friendly** : Autonomous and lightweight applications
5. **Speed** : From code to production in minutes

## 🎯 Spring Boot Starter Projects - The Magic of Starters

### Starter Concept

**Starters** are "all-in-one" dependencies that automatically include all the libraries needed for a specific use case.

### Main Starter Examples

#### 1. **spring-boot-starter-web**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-web</artifactId>
</dependency>
```
**Automatically includes :**
- Spring MVC (for REST APIs and web)
- Embedded Tomcat (server)
- Jackson (JSON serialization)
- Validation API
- Logging (Logback)

**Without the Starter, you would need to add manually :**
```xml
<dependency><artifactId>spring-webmvc</artifactId></dependency>
<dependency><artifactId>tomcat-embed-core</artifactId></dependency>
<dependency><artifactId>jackson-databind</artifactId></dependency>
<dependency><artifactId>hibernate-validator</artifactId></dependency>
<!-- + manage compatible versions! -->
```

#### 2. **spring-boot-starter-data-jpa**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-data-jpa</artifactId>
</dependency>
```
**Includes :**
- Spring Data JPA
- Hibernate (ORM)
- Spring Transaction Management
- JDBC

#### 3. **spring-boot-starter-security**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-security</artifactId>
</dependency>
```
**Includes :**
- Spring Security Core
- Authentication and Authorization
- CSRF Protection
- Session Management

#### 4. **spring-boot-starter-test**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-test</artifactId>
    <scope>test</scope>
</dependency>
```
**Includes :**
- JUnit 5
- Mockito
- AssertJ
- Spring Test
- Hamcrest

#### 5. **spring-boot-starter-actuator**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-actuator</artifactId>
</dependency>
```
**Production-Ready features :**
- Health checks (`/actuator/health`)
- Metrics (`/actuator/metrics`)
- Application info (`/actuator/info`)
- Environment properties (`/actuator/env`)

### Other Useful Starters

| Starter | Usage |
|---------|-------|
| `spring-boot-starter-validation` | Bean Validation (JSR-303) |
| `spring-boot-starter-cache` | Caching (Redis, EhCache) |
| `spring-boot-starter-mail` | Sending emails |
| `spring-boot-starter-thymeleaf` | Template engine |
| `spring-boot-starter-websocket` | WebSockets |
| `spring-boot-starter-batch` | Spring Batch (jobs) |
| `spring-boot-starter-amqp` | RabbitMQ messaging |
| `spring-boot-starter-data-mongodb` | MongoDB |
| `spring-boot-starter-data-redis` | Redis |

### Advantages of Starters

✅ **Version management** : Guaranteed compatibility between dependencies
✅ **Time saving** : One dependency instead of 10+
✅ **No conflicts** : Versions tested together
✅ **Simplicity** : Fewer configuration errors
✅ **Maintenance** : Easy update (one single Spring Boot version)

## ⚙️ Auto-Configuration - The Real Magic

### The Auto-Configuration Principle

Spring Boot analyzes the **classpath** and automatically configures the necessary beans based on the dependencies present.

### How Does It Work?

#### Example : DataSource Auto-Configuration

**If you add :**
```xml
<dependency>
    <artifactId>spring-boot-starter-data-jpa</artifactId>
</dependency>
<dependency>
    <artifactId>h2</artifactId>
</dependency>
```

**Spring Boot automatically configures :**
```java
// These beans are created automatically!
@Bean
public DataSource dataSource() { ... }

@Bean
public EntityManagerFactory entityManagerFactory() { ... }

@Bean
public PlatformTransactionManager transactionManager() { ... }
```

**Configuration in application.properties :**
```properties
spring.datasource.url=jdbc:h2:mem:testdb
spring.datasource.username=sa
spring.datasource.password=
spring.jpa.hibernate.ddl-auto=update
```

### Auto-Configuration Conditions

Spring Boot uses **conditional annotations** :

#### @ConditionalOnClass
```java
@Configuration
@ConditionalOnClass(DataSource.class)
public class DataSourceAutoConfiguration {
    // Configuration activated only if DataSource is on the classpath
}
```

#### @ConditionalOnMissingBean
```java
@Bean
@ConditionalOnMissingBean
public DataSource dataSource() {
    // Created only if the user hasn't defined their own DataSource
}
```

#### @ConditionalOnProperty
```java
@Configuration
@ConditionalOnProperty(name = "spring.datasource.enabled", havingValue = "true")
public class DataSourceConfig {
    // Activated only if the property exists
}
```

### Seeing Auto-Configuration in Action

**Debug mode :**
```properties
# application.properties
debug=true
```

**Output :**
```
============================
CONDITIONS EVALUATION REPORT
============================

Positive matches:
-----------------
   DataSourceAutoConfiguration matched:
      - @ConditionalOnClass found required classes (DataSource)

Negative matches:
-----------------
   RedisAutoConfiguration did not match:
      - @ConditionalOnClass did not find required class (RedisClient)
```

### Customizing or Disabling Auto-Configuration

#### Disable a specific auto-configuration :
```java
@SpringBootApplication(exclude = {DataSourceAutoConfiguration.class})
public class Application { }
```

#### Override with your own bean :
```java
@Configuration
public class CustomDataSourceConfig {
    @Bean
    public DataSource dataSource() {
        // Your custom configuration
        // Spring Boot will not apply the auto-configuration
        return new HikariDataSource();
    }
}
```

## 🖥️ Embedded Servers - Production Ready

### The Problem with External Servers

**Traditional approach :**
1. Develop the application (WAR)
2. Install Tomcat/JBoss on the server
3. Configure the server
4. Deploy the WAR
5. Manage the server separately

**Disadvantages :**
- Deployment complexity
- Dependency on an external server
- Different versions between dev/prod
- Hard to use for microservices

### Spring Boot : Embedded Servers

**Spring Boot embeds the server directly in the JAR!**

```
my-app.jar
├── application classes
├── dependencies (Spring, etc.)
└── embedded Tomcat
```

**Simple execution :**
```bash
java -jar my-app.jar
# Application started with embedded Tomcat on port 8080
```

### Supported Embedded Servers

#### 1. **Tomcat** (default with spring-boot-starter-web)
```xml
<dependency>
    <artifactId>spring-boot-starter-web</artifactId>
    <!-- Embedded Tomcat included -->
</dependency>
```

#### 2. **Jetty** (lightweight alternative)
```xml
<dependency>
    <artifactId>spring-boot-starter-web</artifactId>
    <exclusions>
        <exclusion>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-tomcat</artifactId>
        </exclusion>
    </exclusions>
</dependency>
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-jetty</artifactId>
</dependency>
```

#### 3. **Undertow** (high performance)
```xml
<dependency>
    <artifactId>spring-boot-starter-web</artifactId>
    <exclusions>
        <exclusion>
            <artifactId>spring-boot-starter-tomcat</artifactId>
        </exclusion>
    </exclusions>
</dependency>
<dependency>
    <artifactId>spring-boot-starter-undertow</artifactId>
</dependency>
```

### Embedded Server Configuration

**application.properties :**
```properties
# Port
server.port=8080

# Context path
server.servlet.context-path=/api

# SSL
server.ssl.enabled=true
server.ssl.key-store=classpath:keystore.p12
server.ssl.key-store-password=password

# Compression
server.compression.enabled=true
server.compression.mime-types=application/json,text/html

# Timeouts
server.tomcat.connection-timeout=20000
server.tomcat.max-threads=200

# Error pages
server.error.whitelabel.enabled=false
```

### Advantages of Embedded Servers

✅ **Simplicity** : Single command to start
✅ **Portability** : Same dev/prod environment
✅ **Microservices** : Each service is autonomous
✅ **CI/CD** : Simplified deployment
✅ **Containerization** : Perfect for Docker/Kubernetes
✅ **No external dependency** : Everything in the JAR

### Production-Ready Features (Actuator)

**Monitoring endpoints :**
```properties
management.endpoints.web.exposure.include=health,metrics,info,env
management.endpoint.health.show-details=always
```

**Available endpoints :**
- `/actuator/health` - Application status
- `/actuator/metrics` - Metrics (CPU, memory, requests)
- `/actuator/info` - Application information
- `/actuator/env` - Environment variables
- `/actuator/loggers` - Log configuration
- `/actuator/httptrace` - Last HTTP requests

**Custom Health Indicator :**
```java
@Component
public class DatabaseHealthIndicator implements HealthIndicator {
    @Override
    public Health health() {
        if (isDatabaseUp()) {
            return Health.up().withDetail("database", "Available").build();
        }
        return Health.down().withDetail("database", "Unavailable").build();
    }
}
```

## 🔄 Spring vs Spring MVC vs Spring Boot

### Overview

```
Spring Framework (IoC Container + Core)
    ↓
Spring MVC (Web module of Spring)
    ↓
Spring Boot (Simplification of Spring + Spring MVC)
```

### Spring Framework (Core)

**Definition :** Java framework for Inversion of Control and Dependency Injection

**Components :**
- Spring Core (IoC Container)
- Spring AOP
- Spring JDBC
- Spring Transaction
- Spring Test

**Example :**
```java
@Component
public class UserService {
    @Autowired
    private UserRepository repository;
}
```

**Characteristics :**
- Manages bean lifecycle
- Dependency Injection
- Not web-specific
- XML or annotation configuration
- Base for all Spring modules

### Spring MVC (Web Module)

**Definition :** Spring Framework module for creating web applications and REST APIs

**Architecture :**
```
Client Request
    ↓
DispatcherServlet (Front Controller)
    ↓
Controller (@RestController)
    ↓
Service Layer
    ↓
Repository Layer
    ↓
Response (JSON/HTML)
```

**Example :**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @Autowired
    private UserService userService;
    
    @GetMapping("/{id}")
    public User getUser(@PathVariable Long id) {
        return userService.findById(id);
    }
    
    @PostMapping
    public User createUser(@RequestBody User user) {
        return userService.save(user);
    }
}
```

**Key components :**
- `@Controller` / `@RestController`
- `@RequestMapping` / `@GetMapping` / `@PostMapping`
- `@RequestBody` / `@ResponseBody`
- `@PathVariable` / `@RequestParam`
- View Resolvers (for JSP, Thymeleaf)

**Traditional configuration (without Boot) :**
```xml
<!-- web.xml -->
<servlet>
    <servlet-name>dispatcher</servlet-name>
    <servlet-class>org.springframework.web.servlet.DispatcherServlet</servlet-class>
</servlet>

<!-- dispatcher-servlet.xml -->
<bean class="org.springframework.web.servlet.view.InternalResourceViewResolver">
    <property name="prefix" value="/WEB-INF/views/"/>
    <property name="suffix" value=".jsp"/>
</bean>
```

### Spring Boot (Total Simplification)

**Definition :** Framework built on Spring and Spring MVC that drastically simplifies configuration and deployment

**Complete application example :**
```java
@SpringBootApplication
@RestController
public class Application {
    
    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }
    
    @GetMapping("/hello")
    public String hello() {
        return "Hello World!";
    }
}
```

**Added features :**
- Auto-configuration
- Starter dependencies
- Embedded server
- Production-ready features (Actuator)
- Externalized configuration (application.properties)
- Profiles
- DevTools (hot reload)

### @SpringBootApplication - The Magic Combination

```java
@SpringBootApplication
// Equivalent to :
@SpringBootConfiguration  // = @Configuration
@EnableAutoConfiguration  // Enables auto-configuration
@ComponentScan           // Scans @Component classes
public class Application { }
```

**Breakdown :**
1. **@SpringBootConfiguration** : Marks the class as a Spring configuration
2. **@EnableAutoConfiguration** : Enables the auto-configuration magic
3. **@ComponentScan** : Scans the current package and sub-packages

### Comparison Table

| Aspect | Spring | Spring MVC | Spring Boot |
|--------|--------|------------|-------------|
| **Goal** | IoC & DI | Web applications | Simplify everything |
| **Configuration** | XML/Annotations | Verbose XML | Minimal (properties) |
| **Server** | External required | External required | Embedded |
| **Initial setup** | Complex | Very complex | Fast (minutes) |
| **Dependencies** | Manual | Manual + versions | Auto starters |
| **Production** | Manual configuration | Manual configuration | Built-in Actuator |
| **Deployment** | WAR on server | WAR on server | Standalone JAR |
| **Learning curve** | Medium | High | Low |

### Concrete Example : REST API

#### With Spring MVC (without Boot)
```xml
<!-- pom.xml - 20+ dependencies to manage -->
<dependency><artifactId>spring-webmvc</artifactId></dependency>
<dependency><artifactId>jackson-databind</artifactId></dependency>
<dependency><artifactId>tomcat-servlet-api</artifactId></dependency>
<!-- + XML configuration for DispatcherServlet, ViewResolver, etc. -->
```

#### With Spring Boot
```xml
<!-- pom.xml - 1 dependency -->
<dependency>
    <artifactId>spring-boot-starter-web</artifactId>
</dependency>
```

```java
@SpringBootApplication
@RestController
public class Application {
    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }
    
    @GetMapping("/api/users")
    public List<User> getUsers() {
        return Arrays.asList(new User("John"), new User("Jane"));
    }
}
```

**Result :** Complete REST application in 15 lines of code!

### When to Use What?

**Spring Framework alone :**
- Non-web applications (batch, console)
- Projects requiring total control
- Very specific use cases

**Spring MVC :**
- Maintaining legacy code
- Existing projects without Boot
- Rarely recommended for new projects

**Spring Boot :** ✅ **Recommended for all new projects**
- REST APIs
- Microservices
- Web applications
- Cloud-native projects
- Quick prototypes

## ☁️ Spring Cloud - Microservices Architecture

### Spring Cloud Goal

**Spring Cloud** is a set of frameworks built on Spring Boot to simplify the development of distributed systems and microservices architectures.

### Microservices Problems Solved by Spring Cloud

#### 1. **Service Discovery** (Where are my services?)
**Problem :** How does a service find the address of another service?

**Solution : Spring Cloud Netflix Eureka**
```xml
<dependency>
    <artifactId>spring-cloud-starter-netflix-eureka-server</artifactId>
</dependency>
```

**Eureka Server (Service Registry) :**
```java
@SpringBootApplication
@EnableEurekaServer
public class EurekaServerApplication { }
```

**Eureka Client (Service that registers itself) :**
```java
@SpringBootApplication
@EnableEurekaClient
public class UserServiceApplication { }
```

```properties
# application.properties
eureka.client.service-url.defaultZone=http://localhost:8761/eureka
spring.application.name=user-service
```

#### 2. **Load Balancing** (Distributing traffic)
**Solution : Spring Cloud LoadBalancer**

```java
@RestController
public class OrderController {
    
    @Autowired
    private RestTemplate restTemplate; // With LoadBalancer
    
    @GetMapping("/orders/{id}")
    public Order getOrder(@PathVariable Long id) {
        // Call with automatic load balancing
        User user = restTemplate.getForObject(
            "http://USER-SERVICE/api/users/123", 
            User.class
        );
        return new Order(id, user);
    }
}
```

#### 3. **API Gateway** (Single entry point)
**Solution : Spring Cloud Gateway**

```xml
<dependency>
    <artifactId>spring-cloud-starter-gateway</artifactId>
</dependency>
```

```java
@SpringBootApplication
public class ApiGatewayApplication { }
```

```yaml
# application.yml
spring:
  cloud:
    gateway:
      routes:
        - id: user-service
          uri: lb://USER-SERVICE
          predicates:
            - Path=/api/users/**
        - id: order-service
          uri: lb://ORDER-SERVICE
          predicates:
            - Path=/api/orders/**
```

**Architecture :**
```
Client
  ↓
API Gateway (port 8080)
  ↓
├── User Service (lb://USER-SERVICE)
├── Order Service (lb://ORDER-SERVICE)
└── Product Service (lb://PRODUCT-SERVICE)
```

#### 4. **Centralized Configuration**
**Solution : Spring Cloud Config**

```xml
<dependency>
    <artifactId>spring-cloud-config-server</artifactId>
</dependency>
```

**Config Server :**
```java
@SpringBootApplication
@EnableConfigServer
public class ConfigServerApplication { }
```

```properties
spring.cloud.config.server.git.uri=https://github.com/myorg/config-repo
```

**Git repo structure :**
```
config-repo/
├── application.properties          # Common config
├── user-service.properties         # user-service config
├── user-service-dev.properties     # Dev config
└── user-service-prod.properties    # Prod config
```

**Client retrieves its config :**
```properties
spring.cloud.config.uri=http://localhost:8888
spring.application.name=user-service
spring.profiles.active=dev
```

#### 5. **Circuit Breaker** (Fault tolerance)
**Solution : Resilience4j**

```xml
<dependency>
    <artifactId>spring-cloud-starter-circuitbreaker-resilience4j</artifactId>
</dependency>
```

```java
@Service
public class OrderService {
    
    @Autowired
    private RestTemplate restTemplate;
    
    @CircuitBreaker(name = "userService", fallbackMethod = "fallbackGetUser")
    public User getUser(Long id) {
        return restTemplate.getForObject(
            "http://USER-SERVICE/api/users/" + id, 
            User.class
        );
    }
    
    // Fallback method if user-service is down
    public User fallbackGetUser(Long id, Exception e) {
        return new User(id, "Unknown User");
    }
}
```

```yaml
# application.yml
resilience4j:
  circuitbreaker:
    instances:
      userService:
        failure-rate-threshold: 50
        wait-duration-in-open-state: 10000
        sliding-window-size: 10
```

#### 6. **Distributed Tracing** (Traceability)
**Solution : Spring Cloud Sleuth + Zipkin**

```xml
<dependency>
    <artifactId>spring-cloud-starter-sleuth</artifactId>
</dependency>
<dependency>
    <artifactId>spring-cloud-sleuth-zipkin</artifactId>
</dependency>
```

**Logs with trace IDs :**
```
INFO [user-service,a1b2c3d4,e5f6g7h8] UserController: Getting user 123
INFO [order-service,a1b2c3d4,i9j0k1l2] OrderController: Creating order
```

**Visualization in Zipkin :**
- See the full path of a request through the services
- Identify bottlenecks
- Response time per service

### Main Spring Cloud Components

| Component | Function |
|-----------|----------|
| **Eureka** | Service Discovery (registry) |
| **Gateway** | API Gateway (routing, filters) |
| **Config** | Centralized configuration |
| **LoadBalancer** | Client-side load balancing |
| **Circuit Breaker** | Fault tolerance |
| **Sleuth** | Distributed tracing |
| **OpenFeign** | Declarative REST client |
| **Bus** | Event bus for config refresh |

### Complete Microservices Architecture

```
┌─────────────────────────────────────────────────┐
│              API Gateway (8080)                  │
│         Spring Cloud Gateway                     │
└───────────────────┬─────────────────────────────┘
                    │
    ┌───────────────┼───────────────┐
    │               │               │
┌───▼────┐    ┌────▼─────┐   ┌────▼─────┐
│ User   │    │ Order    │   │ Product  │
│Service │    │ Service  │   │ Service  │
│ :8081  │    │ :8082    │   │ :8083    │
└───┬────┘    └────┬─────┘   └────┬─────┘
    │              │              │
    └──────────────┼──────────────┘
                   │
         ┌─────────▼──────────┐
         │  Eureka Server     │
         │  (Service Registry)│
         │      :8761         │
         └────────────────────┘
         
         ┌────────────────────┐
         │  Config Server     │
         │  (Centralized)     │
         │      :8888         │
         └────────────────────┘
```

### OpenFeign - Simplified REST Client

**Without Feign (RestTemplate) :**
```java
@Service
public class OrderService {
    @Autowired
    private RestTemplate restTemplate;
    
    public User getUser(Long id) {
        return restTemplate.getForObject(
            "http://USER-SERVICE/api/users/" + id,
            User.class
        );
    }
}
```

**With Feign (declarative) :**
```java
@FeignClient(name = "user-service")
public interface UserClient {
    
    @GetMapping("/api/users/{id}")
    User getUser(@PathVariable Long id);
    
    @PostMapping("/api/users")
    User createUser(@RequestBody User user);
}

@Service
public class OrderService {
    @Autowired
    private UserClient userClient;
    
    public User getUser(Long id) {
        return userClient.getUser(id); // Simple and clean!
    }
}
```

### Spring Cloud vs Spring Boot

| Aspect | Spring Boot | Spring Cloud |
|--------|-------------|--------------|
| **Scope** | Single application | Distributed system |
| **Architecture** | Monolith or microservice | Microservices |
| **Complexity** | Simple | Complex |
| **Use case** | Standalone API | Microservices architecture |
| **Dependencies** | Basic starters | Cloud starters (Eureka, Gateway) |

**Summary :**
- **Spring Boot** = Quickly create ONE application
- **Spring Cloud** = Manage MULTIPLE applications (microservices) together

## 📊 Summary - Technology Choices

### For a Simple Application
```
Spring Boot + Embedded Tomcat
    ↓
REST API ready in minutes
```

### For Microservices
```
Spring Boot (base)
    +
Spring Cloud (Eureka, Gateway, Config, Circuit Breaker)
    ↓
Complete microservices architecture
```

### Typical Evolution

**Phase 1 - MVP :**
- Spring Boot monolith
- Single database
- Simple deployment

**Phase 2 - Growth :**
- Split into microservices
- Spring Cloud (Eureka, Gateway)
- Database per service

**Phase 3 - Scale :**
- Circuit Breakers
- Centralized configuration
- Distributed monitoring (Sleuth/Zipkin)
- Kubernetes for orchestration




# Q&A - Spring Boot & Spring Cloud

## 🚀 Spring Boot - Goals and Philosophy

**Q1 : What is the main goal of Spring Boot?**
**A :** To drastically simplify Spring application development by eliminating boilerplate configuration. Spring Boot allows going from code to production in minutes thanks to auto-configuration, starters, and embedded servers.

**Q2 : What problems does Spring Boot solve compared to classic Spring?**
**A :**
- Eliminates verbose XML configuration
- Automatically manages dependencies and compatible versions
- Provides an embedded server (no need for external Tomcat)
- Intelligent auto-configuration of components
- Setup time reduced from several hours to a few minutes

**Q3 : What is "Convention over Configuration"?**
**A :** A philosophy where Spring Boot provides intelligent default values for everything. Instead of manually configuring everything, you accept the conventions (port 8080, `/actuator/health`, etc.) and only configure what differs from the defaults.

## 🎯 Spring Boot Starter Projects

**Q4 : What is a Spring Boot Starter?**
**A :** A Starter is an "all-in-one" dependency that automatically includes all libraries needed for a specific use case. Example : `spring-boot-starter-web` includes Spring MVC, Tomcat, Jackson, Validation, and Logging.

**Q5 : What does spring-boot-starter-web contain?**
**A :**
- Spring MVC (for REST APIs and web)
- Embedded Tomcat (server)
- Jackson (JSON serialization/deserialization)
- Validation API (JSR-303)
- Logging (Logback)

**Q6 : What is the difference between spring-boot-starter-web and spring-boot-starter-data-jpa?**
**A :**
- **spring-boot-starter-web** : For creating REST APIs and web applications (MVC, Tomcat, Jackson)
- **spring-boot-starter-data-jpa** : For data access (JPA, Hibernate, Spring Data, Transactions, JDBC)

**Q7 : What are the advantages of Starters?**
**A :**
- Automatic management of compatible versions
- One dependency instead of 10+
- No version conflicts
- Time saving and fewer errors
- Easy update (one single Spring Boot version)

**Q8 : Name 5 commonly used Starters**
**A :**
1. `spring-boot-starter-web` - REST APIs and web
2. `spring-boot-starter-data-jpa` - Data access with JPA
3. `spring-boot-starter-security` - Security and authentication
4. `spring-boot-starter-test` - Testing (JUnit, Mockito)
5. `spring-boot-starter-actuator` - Production monitoring

**Q9 : What is spring-boot-starter-actuator and why is it important?**
**A :** Actuator provides production-ready endpoints to monitor the application : health checks (`/actuator/health`), metrics (`/actuator/metrics`), environment information, logs. Essential for production and DevOps.

## ⚙️ Auto-Configuration

**Q10 : How does Spring Boot auto-configuration work?**
**A :** Spring Boot analyzes the classpath at startup and automatically configures the necessary beans based on the dependencies present. If H2 and JPA are on the classpath, it automatically configures DataSource, EntityManagerFactory, and TransactionManager.

**Q11 : What is @ConditionalOnClass?**
**A :** An annotation that activates a configuration only if a specific class is present on the classpath. Example : `@ConditionalOnClass(DataSource.class)` activates the config only if DataSource is available.

**Q12 : What is @ConditionalOnMissingBean?**
**A :** An annotation that creates a bean only if the user hasn't already defined one. Allows providing a default configuration while letting the user override it. The "convention with override possibility" principle.

**Q13 : How do you see which auto-configurations have been applied?**
**A :** Enable debug mode with `debug=true` in `application.properties`. Spring Boot will display a detailed report of "Positive matches" (applied configurations) and "Negative matches" (ignored configurations).

**Q14 : How do you disable a specific auto-configuration?**
**A :**
```java
@SpringBootApplication(exclude = {DataSourceAutoConfiguration.class})
public class Application { }
```
Or in application.properties :
```properties
spring.autoconfigure.exclude=org.springframework.boot.autoconfigure.jdbc.DataSourceAutoConfiguration
```

**Q15 : How do you override an auto-configuration with your own?**
**A :** Simply define your own bean of the same type. Thanks to `@ConditionalOnMissingBean`, Spring Boot will detect your bean and will not apply the default auto-configuration.

**Q16 : Give an example of auto-configuration in action**
**A :** If you add `spring-boot-starter-data-jpa` and `h2` in pom.xml, Spring Boot automatically configures :
- DataSource (in-memory H2 connection)
- EntityManagerFactory (JPA)
- TransactionManager
With no manual configuration at all!

## 🖥️ Embedded Servers

**Q17 : What is an Embedded Server?**
**A :** A server (Tomcat, Jetty, Undertow) included directly in the application JAR. The application becomes autonomous and can start with `java -jar app.jar` without installing an external server.

**Q18 : What are the advantages of embedded servers?**
**A :**
- Simplicity : single command to start
- Portability : same dev/prod environment
- Ideal for microservices (each service autonomous)
- Simplified CI/CD
- Perfect for Docker/Kubernetes
- No external server to manage

**Q19 : What is the default embedded server in Spring Boot?**
**A :** **Tomcat** is the default embedded server with `spring-boot-starter-web`. It is included automatically with no extra configuration.

**Q20 : How do you switch embedded servers (Tomcat to Jetty)?**
**A :**
```xml
<dependency>
    <artifactId>spring-boot-starter-web</artifactId>
    <exclusions>
        <exclusion>
            <artifactId>spring-boot-starter-tomcat</artifactId>
        </exclusion>
    </exclusions>
</dependency>
<dependency>
    <artifactId>spring-boot-starter-jetty</artifactId>
</dependency>
```

**Q21 : Name the 3 embedded servers supported by Spring Boot**
**A :**
1. **Tomcat** (default) - Popular and robust
2. **Jetty** - Lightweight and flexible
3. **Undertow** - High performance

**Q22 : How do you configure the embedded server port?**
**A :**
```properties
# application.properties
server.port=8080

# Or random port
server.port=0

# Or via command line
java -jar app.jar --server.port=9090
```

**Q23 : How do you enable SSL on the embedded server?**
**A :**
```properties
server.ssl.enabled=true
server.ssl.key-store=classpath:keystore.p12
server.ssl.key-store-password=mypassword
server.ssl.key-store-type=PKCS12
```

**Q24 : What is the difference between JAR and WAR with Spring Boot?**
**A :**
- **JAR** (recommended) : Standalone application with embedded server, executable with `java -jar`
- **WAR** : For deployment on an external server (Tomcat, JBoss), requires `SpringBootServletInitializer`

**Q25 : How do you configure the application context path?**
**A :**
```properties
server.servlet.context-path=/api
# Endpoints will be accessible via /api/users instead of /users
```

## 📱 @SpringBootApplication

**Q26 : What does the @SpringBootApplication annotation do?**
**A :** It is a meta-annotation combining 3 annotations :
1. `@SpringBootConfiguration` (= `@Configuration`)
2. `@EnableAutoConfiguration` (enables auto-configuration)
3. `@ComponentScan` (scans components)

**Q27 : Can you break down @SpringBootApplication into its 3 annotations?**
**A :** Yes :
```java
@SpringBootConfiguration
@EnableAutoConfiguration
@ComponentScan
public class Application {
    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }
}
```
But `@SpringBootApplication` is more concise and recommended.

**Q28 : What does @EnableAutoConfiguration do exactly?**
**A :** Enables the Spring Boot auto-configuration magic. Analyzes the classpath and automatically configures the necessary beans based on the dependencies present (DataSource, EntityManager, etc.).

**Q29 : What does @ComponentScan do inside @SpringBootApplication?**
**A :** Automatically scans the main class package and all its sub-packages to detect components (`@Component`, `@Service`, `@Repository`, `@Controller`) and register them as beans.

## 🔄 Spring vs Spring MVC vs Spring Boot

**Q30 : What is the difference between Spring Framework and Spring Boot?**
**A :**
- **Spring Framework** : IoC container and DI, requires extensive manual configuration
- **Spring Boot** : Built on Spring, adds auto-configuration, starters, embedded server, and simplifies everything drastically

**Q31 : What is Spring MVC?**
**A :** Spring MVC is a Spring Framework module for creating web applications and REST APIs. It provides the MVC architecture (Model-View-Controller) with annotations like `@Controller`, `@RequestMapping`, `@RestController`.

**Q32 : Does Spring Boot include Spring MVC?**
**A :** Yes, `spring-boot-starter-web` automatically includes Spring MVC + embedded Tomcat + Jackson. Spring Boot simplifies the use of Spring MVC by eliminating all XML configuration.

**Q33 : Can you use Spring MVC without Spring Boot?**
**A :** Yes, but it is much more complex. You need to manually configure DispatcherServlet, ViewResolver, dependency management, external server, etc. Spring Boot makes all of this automatic.

**Q34 : Comparison : how many lines for a REST API?**
**A :**
- **Spring MVC without Boot** : ~100+ lines (XML config + Java)
- **Spring Boot** : ~15 lines of Java code
```java
@SpringBootApplication
@RestController
public class Application {
    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }
    
    @GetMapping("/hello")
    public String hello() {
        return "Hello World!";
    }
}
```

**Q35 : When to use Spring Framework alone vs Spring Boot?**
**A :**
- **Spring alone** : Legacy projects, need for extreme total control (rare)
- **Spring Boot** : All new projects (REST APIs, microservices, web apps) - Recommended ✅

**Q36 : Does Spring Boot replace Spring Framework?**
**A :** No, Spring Boot is built ON TOP of Spring Framework. It uses Spring Core, Spring MVC, etc., but simplifies their configuration and usage. Spring Boot = Spring + Convention + Auto-configuration.

## ☁️ Spring Cloud - Introduction

**Q37 : What is Spring Cloud?**
**A :** A set of frameworks built on Spring Boot to simplify the development of distributed microservices architectures. It solves problems of service discovery, load balancing, centralized configuration, circuit breakers, etc.

**Q38 : What is the difference between Spring Boot and Spring Cloud?**
**A :**
- **Spring Boot** : For creating ONE standalone application
- **Spring Cloud** : For managing MULTIPLE applications (microservices) that communicate together

**Q39 : Name 5 problems that Spring Cloud solves**
**A :**
1. Service Discovery (how to find services?)
2. Load Balancing (distribute traffic)
3. API Gateway (single entry point)
4. Centralized configuration
5. Circuit Breaker (fault tolerance)

**Q40 : What is Netflix OSS in Spring Cloud?**
**A :** Netflix open-sourced its microservices tools (Eureka, Ribbon, Hystrix, Zuul). Spring Cloud integrated them and continues to maintain them (some are now replaced by modern alternatives like Gateway instead of Zuul).

## 🔍 Service Discovery

**Q41 : What is Service Discovery and why is it needed?**
**A :** In a microservices architecture, services have dynamic addresses (scaling, redeployments). Service Discovery is a central registry (like Eureka) where services register themselves. Other services query this registry to find available addresses.

**Q42 : What is Netflix Eureka?**
**A :** Eureka is a registry service (Service Discovery). Microservices register with Eureka at startup, and other services query Eureka to find available instances.

**Q43 : How do you create a Eureka Server?**
**A :**
```java
@SpringBootApplication
@EnableEurekaServer
public class EurekaServerApplication {
    public static void main(String[] args) {
        SpringApplication.run(EurekaServerApplication.class, args);
    }
}
```
With dependency : `spring-cloud-starter-netflix-eureka-server`

**Q44 : How do you register a service with Eureka?**
**A :**
```java
@SpringBootApplication
@EnableEurekaClient // Or @EnableDiscoveryClient
public class UserServiceApplication { }
```
```properties
eureka.client.service-url.defaultZone=http://localhost:8761/eureka
spring.application.name=user-service
```

**Q45 : How do you call a service registered in Eureka?**
**A :**
```java
@Autowired
private RestTemplate restTemplate;

// Use the service name instead of the URL
User user = restTemplate.getForObject(
    "http://USER-SERVICE/api/users/123",
    User.class
);
// Spring Cloud automatically resolves the address via Eureka
```

## 🌐 API Gateway

**Q46 : What is an API Gateway and why do you need one?**
**A :** An API Gateway is a single entry point for all microservices. It handles routing, authentication, rate limiting, and avoids exposing all services directly. Clients communicate only with the Gateway.

**Q47 : What is Spring Cloud Gateway?**
**A :** Spring Cloud's API Gateway framework. It routes requests to the appropriate microservices, applies filters (auth, logging, transformation), and supports load balancing.

**Q48 : How do you configure routes in Spring Cloud Gateway?**
**A :**
```yaml
spring:
  cloud:
    gateway:
      routes:
        - id: user-service
          uri: lb://USER-SERVICE  # lb = load balanced
          predicates:
            - Path=/api/users/**
        - id: order-service
          uri: lb://ORDER-SERVICE
          predicates:
            - Path=/api/orders/**
```

**Q49 : What is the difference between Zuul and Spring Cloud Gateway?**
**A :**
- **Zuul** (Netflix) : Old gateway, synchronous, less performant
- **Spring Cloud Gateway** : Modern, asynchronous (WebFlux), better performance, recommended for new projects

**Q50 : What are the advantages of an API Gateway?**
**A :**
- Single entry point (simplifies the client)
- Centralized authentication
- Rate limiting and throttling
- Centralized logging and monitoring
- Intelligent routing
- Protection of internal services

## ⚙️ Centralized Configuration

**Q51 : Why centralize the configuration of microservices?**
**A :** With 10+ microservices, managing configs in each service becomes unmanageable. Spring Cloud Config centralizes all configurations in one place (Git repository), allowing configs to be changed without redeploying services.

**Q52 : How does Spring Cloud Config work?**
**A :** 
1. **Config Server** : Spring Boot server that exposes configurations from a Git repository
2. **Config Client** : Microservices connect to the Config Server at startup to retrieve their configuration

**Q53 : How do you create a Config Server?**
**A :**
```java
@SpringBootApplication
@EnableConfigServer
public class ConfigServerApplication { }
```
```properties
spring.cloud.config.server.git.uri=https://github.com/myorg/config-repo
```
Dependency : `spring-cloud-config-server`

**Q54 : How do you organize configuration files in Git?**
**A :**
```
config-repo/
├── application.properties          # Common config for all services
├── user-service.properties         # user-service config
├── user-service-dev.properties     # Dev environment
├── user-service-prod.properties    # Prod environment
└── order-service.properties
```

**Q55 : How does a microservice retrieve its configuration?**
**A :**
```properties
# bootstrap.properties (loaded before application.properties)
spring.cloud.config.uri=http://localhost:8888
spring.application.name=user-service
spring.profiles.active=dev
```
The service automatically retrieves `user-service-dev.properties`

**Q56 : How do you refresh configuration without restarting the service?**
**A :** Use Spring Cloud Bus with `@RefreshScope` :
```java
@RestController
@RefreshScope
public class UserController {
    @Value("${message}")
    private String message; // Will be refreshed without restart
}
```
POST to `/actuator/refresh` or broadcast via Spring Cloud Bus

## 🔄 Load Balancing

**Q57 : What is Client-Side Load Balancing?**
**A :** The client (calling service) itself chooses which instance to call among those available in Eureka. Unlike traditional load balancing (dedicated server), the logic is in the client.

**Q58 : What is Spring Cloud LoadBalancer?**
**A :** A Spring Cloud module that replaces Netflix Ribbon. It performs client-side load balancing by distributing requests among available instances in Eureka.

**Q59 : How do you enable load balancing with RestTemplate?**
**A :**
```java
@Configuration
public class Config {
    @Bean
    @LoadBalanced  // Enables load balancing
    public RestTemplate restTemplate() {
        return new RestTemplate();
    }
}
```
Then use the service name :
```java
restTemplate.getForObject("http://USER-SERVICE/api/users/1", User.class);
```

**Q60 : What load balancing algorithms are available?**
**A :**
- **Round Robin** (default) : Circular distribution
- **Random** : Random selection
- **Weighted** : Based on configured weights
Configurable via `spring.cloud.loadbalancer.*`

## 🛡️ Circuit Breaker

**Q61 : What is a Circuit Breaker and why is it important?**
**A :** A Circuit Breaker protects a service from failure cascades. If a remote service stops responding, instead of continuing to call it (repeated timeouts), the Circuit Breaker "opens" and immediately returns a fallback response, letting the remote service recover.

**Q62 : What are the 3 states of a Circuit Breaker?**
**A :**
1. **CLOSED** : Normal, requests pass through
2. **OPEN** : Service failure detected, requests blocked, fallback returned
3. **HALF_OPEN** : Testing if the service has recovered

**Q63 : What is Resilience4j?**
**A :** A fault tolerance library replacing Netflix Hystrix (deprecated). Provides Circuit Breaker, Retry, Rate Limiter, Bulkhead. Used with Spring Cloud Circuit Breaker.

**Q64 : How do you implement a Circuit Breaker with Resilience4j?**
**A :**
```java
@Service
public class OrderService {
    
    @CircuitBreaker(name = "userService", fallbackMethod = "fallbackGetUser")
    public User getUser(Long id) {
        return restTemplate.getForObject(
            "http://USER-SERVICE/api/users/" + id,
            User.class
        );
    }
    
    // Fallback method if user-service is down
    public User fallbackGetUser(Long id, Exception e) {
        return new User(id, "Unknown User");
    }
}
```

**Q65 : How do you configure a Circuit Breaker?**
**A :**
```yaml
resilience4j:
  circuitbreaker:
    instances:
      userService:
        failure-rate-threshold: 50        # 50% failures = OPEN
        wait-duration-in-open-state: 10000 # 10s before HALF_OPEN
        sliding-window-size: 10           # Window of 10 calls
        minimum-number-of-calls: 5        # Min 5 calls before calculation
```

**Q66 : What is a fallback method?**
**A :** A backup method called when the Circuit Breaker is OPEN or an exception occurs. Allows returning a default response instead of propagating the error (e.g., default user, cache, user-friendly error message).

## 🔎 Distributed Tracing

**Q67 : What is Distributed Tracing?**
**A :** In microservices, a request goes through multiple services. Distributed Tracing assigns a unique ID (trace ID) to each request and follows it through all services, allowing you to see the complete path and identify problems.

**Q68 : What is Spring Cloud Sleuth?**
**A :** A Spring Cloud library that automatically adds trace IDs and span IDs to each service's logs. Allows correlating logs from different services for the same request.

**Q69 : What are Trace ID and Span ID?**
**A :**
- **Trace ID** : Unique identifier for the entire request across all services
- **Span ID** : Unique identifier for each segment (call to a service)

**Q70 : How does Sleuth change the logs?**
**A :**
```
# Without Sleuth
INFO UserController: Getting user 123

# With Sleuth
INFO [user-service,a1b2c3d4e5f6,g7h8i9j0k1l2] UserController: Getting user 123
#    [app-name,   trace-id,    span-id]
```

**Q71 : What is Zipkin?**
**A :** A web interface for visualizing distributed traces. Allows you to graphically see the path of a request through microservices, the response time of each service, and identify bottlenecks.

**Q72 : How do you integrate Zipkin with Sleuth?**
**A :**
```xml
<dependency>
    <artifactId>spring-cloud-starter-sleuth</artifactId>
</dependency>
<dependency>
    <artifactId>spring-cloud-sleuth-zipkin</artifactId>
</dependency>
```
```properties
spring.zipkin.base-url=http://localhost:9411
spring.sleuth.sampler.probability=1.0  # Trace 100% of requests
```

## 🌐 OpenFeign

**Q73 : What is OpenFeign?**
**A :** A declarative REST client that simplifies calls between microservices. Instead of using RestTemplate, you define an interface and Feign automatically generates the implementation.

**Q74 : What is the difference between RestTemplate and Feign?**
**A :**
**RestTemplate (imperative) :**
```java
User user = restTemplate.getForObject(
    "http://USER-SERVICE/api/users/" + id,
    User.class
);
```

**Feign (declarative) :**
```java
@FeignClient(name = "user-service")
public interface UserClient {
    @GetMapping("/api/users/{id}")
    User getUser(@PathVariable Long id);
}

// Usage
User user = userClient.getUser(id);
```

**Q75 : How do you create a Feign Client?**
**A :**
```java
@FeignClient(name = "user-service")  // Service name in Eureka
public interface UserClient {
    
    @GetMapping("/api/users/{id}")
    User getUser(@PathVariable Long id);
    
    @PostMapping("/api/users")
    User createUser(@RequestBody User user);
    
    @DeleteMapping("/api/users/{id}")
    void deleteUser(@PathVariable Long id);
}
```

**Q76 : How do you enable Feign in Spring Boot?**
**A :**
```java
@SpringBootApplication
@EnableFeignClients
public class OrderServiceApplication { }
```
Dependency : `spring-cloud-starter-openfeign`

**Q77 : Does Feign support load balancing?**
**A :** Yes, automatically! If multiple instances of `user-service` are registered in Eureka, Feign will use Spring Cloud LoadBalancer to distribute the calls.

**Q78 : How do you add a Circuit Breaker to Feign?**
**A :**
```java
@FeignClient(name = "user-service", fallback = UserClientFallback.class)
public interface UserClient {
    @GetMapping("/api/users/{id}")
    User getUser(@PathVariable Long id);
}

@Component
public class UserClientFallback implements UserClient {
    @Override
    public User getUser(Long id) {
        return new User(id, "Fallback User");
    }
}
```

## 🏗️ Microservices Architecture

**Q79 : Describe a complete microservices architecture with Spring Cloud**
**A :**

**This microservices architecture is made up of an API Gateway that handles routing, security, and request filtering.
The different microservices communicate with each other and automatically register in Eureka (service registry).
Centralized configuration is provided by the Config Server, making it easy to manage settings.
Finally, Zipkin enables distributed call tracing to monitor performance and detect problems.**
```
Client
  ↓
API Gateway (Spring Cloud Gateway) - Port 8080
  ↓
├── User Service - Port 8081
├── Order Service - Port 8082
└── Product Service - Port 8083
  ↓
Eureka Server (Service Registry) - Port 8761
Config Server (Configuration) - Port 8888
Zipkin (Tracing) - Port 9411
```
Each service communicates via Feign with load balancing and circuit breakers.

**Q80 : What is the typical evolution of an application toward microservices?**
**A :**
1. **Phase 1 - MVP** : Simple Spring Boot monolith
2. **Phase 2 - Growth** : Split into microservices + Eureka + Gateway
3. **Phase 3 - Scale** : Add Config Server, Circuit Breakers, Tracing
4. **Phase 4 - Production** : Kubernetes, advanced monitoring, full CI/CD

**Q81 : Is Spring Cloud required to build microservices?**
**A :** No, but it simplifies things enormously. You can build microservices with Spring Boot alone, but you'll need to manually handle service discovery, load balancing, configuration, etc. Spring Cloud provides proven solutions to these problems.

**Q82 : When should you NOT use microservices?**
**A :**
- Small applications / MVP
- Small team
- Simple business domain
- No need for independent scaling
For these cases, a Spring Boot monolith is preferable (simpler, less operational complexity).

**Q83 : What are the challenges of microservices?**
**A :**
- Operational complexity (monitoring, deployment)
- Distributed transactions are hard
- Complex integration tests
- Network latency
- Harder debugging
Spring Cloud helps reduce these problems but doesn't eliminate them completely.

**Q84 : How do you test a microservices architecture?**
**A :**
- **Unit tests** : Each service individually
- **Integration tests** : With TestContainers (databases, message brokers)
- **Contract tests** : Spring Cloud Contract to verify APIs
- **End-to-end tests** : Full environment with all services

**Q85 : What is the difference between Spring Cloud and Kubernetes?**
**A :**
- **Spring Cloud** : Application framework (Java code) for microservices patterns
- **Kubernetes** : Container orchestration platform (infrastructure)
They are complementary! Spring Cloud for application logic (circuit breakers, config), Kubernetes for deployment and scaling.
