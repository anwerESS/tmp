## [**..**](./00_index.md)

## Configuration Management & Spring Cloud Config - Key Points

### **General Concept**

**Configuration Management** in Spring Boot allows you to manage configurations in a centralized way and modify them without redeploying the application. **Spring Cloud Config Server** provides a solution to externalize and centralize the configurations of multiple microservices.

**Problem without Config Server :**
- Configurations scattered across each microservice
- Requires redeployment to change a configuration
- Difficult to manage different environments (dev, qa, prod)

**Solution with Config Server :**
- Configurations centralized in one place
- Dynamic changes without redeployment
- Easy management of profiles (dev, qa, prod)

---

### **Spring Boot Profiles**

**Profiles** allow you to define different configurations depending on the environment.

**Activating a profile :**
```yaml
# application.yml
spring:
  profiles:
    active: prod  # Can be : dev, qa, prod
```

**Configuration files by profile :**
```
application.yml          # Common configuration
application-dev.yml      # Dev configuration
application-qa.yml       # QA configuration
application-prod.yml     # Prod configuration
```

---

### **3 approaches to read configurations**

**1. @Value (for simple properties)**
```java
@RestController
public class AccountController {
    
    @Value("${build.version}")
    private String buildVersion;
    
    @Value("${accounts.message}")
    private String message;
    
    @GetMapping("/info")
    public String getInfo() {
        return "Version: " + buildVersion + ", Message: " + message;
    }
}
```

**2. Environment Interface (programmatic access)**
```java
@RestController
public class AccountController {
    
    @Autowired
    private Environment environment;
    
    @GetMapping("/info")
    public String getInfo() {
        String buildVersion = environment.getProperty("build.version");
        String message = environment.getProperty("accounts.message");
        return "Version: " + buildVersion;
    }
}
```

**3. @ConfigurationProperties (for structured configurations - BEST PRACTICE)**
```java
@Component
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfigProperties {
    
    private String message;
    private String buildVersion;
    private ContactDetails contactDetails;
    private List<String> onCallSupport;
    
    // Getters and Setters
    
    public static class ContactDetails {
        private String name;
        private String email;
        // Getters and Setters
    }
}

// Usage in the controller
@RestController
public class AccountController {
    
    @Autowired
    private AccountsConfigProperties configProperties;
    
    @GetMapping("/contact-info")
    public AccountsConfigProperties getContactInfo() {
        return configProperties;
    }
}
```

---

### **Precedence Order (priority order of configurations)**

From highest to lowest priority :

1. **Command line arguments** : `java -jar app.jar --server.port=9090`
2. **JVM system properties** : `java -Dserver.port=9090 -jar app.jar`
3. **OS environment variables** : `export SERVER_PORT=9090`
4. **application-{profile}.yml** (active profile)
5. **application.yml** (main file)
6. **@PropertySource**
7. **Default properties**

**Example :**
```bash
# Command line (highest priority)
java -jar app.jar --server.port=8081

# JVM system properties
java -Dserver.port=8082 -jar app.jar

# Environment variable
export SERVER_PORT=8083
java -jar app.jar

# application.yml (lowest priority)
server:
  port: 8080
```

---

### **Spring Cloud Config Server - Architecture**

**Config Server** : Centralized server that stores configurations
**Config Clients** : Microservices that fetch their configurations from the Config Server

```
┌─────────────────────────────────────────┐
│     Spring Cloud Config Server          │
│         (Port 8071)                      │
│  ┌───────────────────────────────────┐  │
│  │   Configuration Sources:          │  │
│  │   - Classpath                     │  │
│  │   - Filesystem                    │  │
│  │   - Git Repository                │  │
│  └───────────────────────────────────┘  │
└─────────────────────────────────────────┘
              ▲        ▲        ▲
              │        │        │
    ┌─────────┴─┐  ┌──┴───┐  ┌─┴─────┐
    │ Accounts  │  │ Loans │  │ Cards │
    │   MS      │  │  MS   │  │  MS   │
    └───────────┘  └───────┘  └───────┘
    Config Clients
```

---

### **Configuration 1 : Config Server from Classpath**

**1. Dependencies (Config Server) :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-config-server</artifactId>
</dependency>

<dependencyManagement>
    <dependencies>
        <dependency>
            <groupId>org.springframework.cloud</groupId>
            <artifactId>spring-cloud-dependencies</artifactId>
            <version>${spring-cloud.version}</version>
            <type>pom</type>
            <scope>import</scope>
        </dependency>
    </dependencies>
</dependencyManagement>
```

**2. Enable Config Server :**
```java
@SpringBootApplication
@EnableConfigServer  // Enables the Config Server
public class ConfigServerApplication {
    public static void main(String[] args) {
        SpringApplication.run(ConfigServerApplication.class, args);
    }
}
```

**3. Configuration (application.yml) :**
```yaml
server:
  port: 8071  # Config Server port

spring:
  profiles:
    active: native  # "native" profile to load from classpath
  cloud:
    config:
      server:
        native:
          search-locations: classpath:/config  # Folder containing configs
```

**4. File structure :**
```
src/main/resources/
└── config/
    ├── accounts.yml         # Common accounts config
    ├── accounts-dev.yml     # Dev accounts config
    ├── accounts-qa.yml      # QA accounts config
    ├── accounts-prod.yml    # Prod accounts config
    ├── loans.yml
    ├── loans-dev.yml
    ├── loans-qa.yml
    └── loans-prod.yml
```

**5. Config Client (microservice) :**
```yaml
# microservice application.yml
spring:
  application:
    name: accounts  # Service name
  config:
    import: "optional:configserver:http://localhost:8071"  # Config Server URL
  profiles:
    active: prod  # Active profile
```

---

### **Configuration 2 : Config Server from Filesystem**

**Configuration (application.yml) :**
```yaml
server:
  port: 8071

spring:
  profiles:
    active: native
  cloud:
    config:
      server:
        native:
          search-locations: "file:///home/anweress/spring-config/config"  # Absolute path
```

**Advantage :** Easy to modify files directly on the filesystem without rebuild.

---

### **Configuration 3 : Config Server from Git Repository (BEST PRACTICE)**

**Configuration (application.yml) :**
```yaml
server:
  port: 8071

spring:
  profiles:
    active: git  # "git" profile
  cloud:
    config:
      server:
        git:
          uri: "https://github.com/anwerESS/udemy-microservices-config.git"
          default-label: main  # Default branch
          timeout: 5
          clone-on-start: true  # Clone on startup
          force-pull: true      # Force pull on every request
```

**Advantages :**
- ✅ **Version control** of configurations
- ✅ **Team collaboration**
- ✅ **Change history**
- ✅ **Easy rollback**
- ✅ **Centralization** in the cloud

---

### **Accessing configurations from the Config Server**

**URL format :**
```
http://localhost:8071/{application}/{profile}
```

**Examples :**
```bash
# Prod configuration for accounts
curl http://localhost:8071/accounts/prod

# QA configuration for loans
curl http://localhost:8071/loans/qa

# Dev configuration for cards
curl http://localhost:8071/cards/dev
```

**JSON Response :**
```json
{
  "name": "accounts",
  "profiles": ["prod"],
  "label": null,
  "version": "28128b0bb42454a869b7ac213e6c716714a40c24",
  "state": "",
  "propertySources": [
    {
      "name": "https://github.com/.../accounts-prod.yml",
      "source": {
        "build.version": "1.0",
        "accounts.message": "Welcome to EazyBank accounts prod APIs",
        "accounts.contactDetails.name": "Reine Aishwarya - Product Owner",
        "accounts.contactDetails.email": "aishwarya@eazybank.com"
      }
    },
    {
      "name": "https://github.com/.../accounts.yml",
      "source": {
        "build.version": "3.0",
        "accounts.message": "Welcome to EazyBank accounts local APIs"
      }
    }
  ]
}
```

**Note :** Configurations are **merged** : profile-specific (prod) overrides common configs.

---

### **Refresh Configurations at Runtime (3 approaches)**

**Problem :** By default, configurations are loaded at startup. If you change a config, you need to restart the microservice.

---

#### **Approach 1 : Refresh Actuator (manual)**

**1. Add @RefreshScope on beans :**
```java
@RestController
@RefreshScope  // Enables dynamic refresh
public class AccountController {
    
    @Autowired
    private AccountsConfigProperties configProperties;
    
    @GetMapping("/contact-info")
    public AccountsConfigProperties getContactInfo() {
        return configProperties;
    }
}
```

**2. Expose the refresh endpoint :**
```yaml
management:
  endpoints:
    web:
      exposure:
        include: refresh  # Exposes /actuator/refresh
```

**3. Call the endpoint after a config change :**
```bash
# Change the config in Git
# Then call refresh for each microservice
curl -X POST http://localhost:8080/actuator/refresh
curl -X POST http://localhost:8081/actuator/refresh
curl -X POST http://localhost:8082/actuator/refresh
```

**Response :**
```json
[
  "config.client.version",
  "accounts.message"
]
```

**Downside :** Must call refresh for **each microservice** manually.

---

#### **Approach 2 : Spring Cloud Bus + RabbitMQ (automatic)**

**Architecture :**
```
┌─────────────────────┐
│   Config Server     │
│   Change Config     │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│     RabbitMQ        │  ←── Message Bus
│   (Message Broker)  │
└──────────┬──────────┘
           │
    ┌──────┴──────┬──────────┐
    ▼             ▼          ▼
┌─────────┐  ┌─────────┐  ┌─────────┐
│Accounts │  │  Loans  │  │  Cards  │
│   MS    │  │   MS    │  │   MS    │
└─────────┘  └─────────┘  └─────────┘
 Auto Refresh (all at the same time)
```

**1. Dependency (all microservices) :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-bus-amqp</artifactId>
</dependency>
```

**2. RabbitMQ configuration (all microservices) :**
```yaml
spring:
  rabbitmq:
    host: "localhost"
    port: 5672
    username: "guest"
    password: "guest"

management:
  endpoints:
    web:
      exposure:
        include: busrefresh  # Exposes /actuator/busrefresh
```

**3. Start RabbitMQ :**
```bash
docker run -it --rm --name rabbitmq \
  -p 5672:5672 \
  -p 15672:15672 \
  rabbitmq:4.0-management

# RabbitMQ UI : http://localhost:15672 (guest/guest)
```

**4. Refresh all microservices at once :**
```bash
# Change the config in Git
# Call busrefresh on ANY microservice
curl -X POST http://localhost:8080/actuator/busrefresh

# All microservices are automatically refreshed via RabbitMQ
```

**Advantages :**
- ✅ **A single call** refreshes all microservices
- ✅ **Automatic** via message bus
- ✅ **Scalable** for dozens of microservices

---

#### **Approach 3 : Spring Cloud Bus + GitHub Webhook (100% automatic)**

**1. Add Spring Cloud Config Monitor (Config Server) :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-config-monitor</artifactId>
</dependency>
```

**Note :** This dependency automatically exposes the `/monitor` endpoint.

**2. Configuration is the same as approach 2 (RabbitMQ)**

**3. Configure GitHub Webhook :**
- Go to the GitHub repository → Settings → Webhooks
- Add webhook :
  - **Payload URL** : `http://<your-host>:8071/monitor`
  - **Content type** : `application/json`
  - **Events** : Push events

**4. Automatic flow :**
```
Developer push config → GitHub → Webhook → Config Server /monitor
                                              ↓
                                           RabbitMQ
                                              ↓
                                    All microservices
                                    refreshed automatically
```

**Advantages :**
- ✅ **100% automatic** : no manual action needed
- ✅ **Real-time** : refresh on Git push
- ✅ **Production-ready**

---

### **Encryption of sensitive configurations**

To secure passwords, secrets, etc.

**1. Generate an encryption key :**
```yaml
# application.yml (Config Server)
encrypt:
  key: "MySecretEncryptionKey123"  # Symmetric key
```

**2. Encrypt a value :**
```bash
curl http://localhost:8071/encrypt -d "mySecretPassword"
# Result : AQBhZGF...encrypted-value...
```

**3. Use it in config files :**
```yaml
# accounts-prod.yml
database:
  password: '{cipher}AQBhZGF...encrypted-value...'
```

**4. Automatic decryption by clients**

---

### **Key Points**

**Configuration Management :**
- **3 reading approaches** : @Value, Environment, @ConfigurationProperties (best practice)
- **Precedence Order** : Command line > JVM props > Env vars > application.yml
- **Profiles** : dev, qa, prod for different environments

**Spring Cloud Config Server :**
- **Centralizes** configurations for all microservices
- **3 sources** : Classpath, Filesystem, Git (best practice)
- **@EnableConfigServer** to enable the Config Server
- **spring.config.import** to connect clients

**Refresh Configurations :**
1. **Manual** : /actuator/refresh (one by one)
2. **Spring Cloud Bus** : /actuator/busrefresh (all at once via RabbitMQ)
3. **GitHub Webhook** : 100% automatic with /monitor endpoint

**Advantages :**
- ✅ Changes without redeployment
- ✅ Centralized management
- ✅ Version control (Git)
- ✅ Multiple environments (profiles)
- ✅ Security (encryption)

---

## Questions/Answers - Configuration Management & Spring Cloud Config

### **1. What is Configuration Management in Spring Boot and why is it important ?**

**Answer :**
**Configuration Management** is the process of managing and organizing an application's configurations in a centralized and flexible way.

**Why it matters :**
- **Separation** : Separates configuration from code
- **Multiple environments** : Different configs for dev, qa, prod
- **Changes without redeployment** : Modify configs on the fly
- **Centralization** : Centralized management for microservices
- **Security** : Encryption of sensitive data
- **Version control** : History of changes

**Without Config Management :**
- Configurations hardcoded in the code
- Redeployment needed for every change
- Difficult to manage multiple environments
- Risk of errors (wrong config in prod)

**With Config Management :**
- Externalized configurations
- Dynamic changes
- Profiles for each environment
- Audit trail with Git

---

### **2. What are the different ways to read configurations in Spring Boot ?**

**Answer :**

**1. @Value (for simple properties)**
```java
@RestController
public class AccountController {
    
    @Value("${build.version}")
    private String buildVersion;
    
    @Value("${accounts.message}")
    private String message;
    
    @GetMapping("/info")
    public String getInfo() {
        return "Version: " + buildVersion;
    }
}
```

**Advantages :** Simple, direct
**Disadvantages :** Verbose for many properties, no validation

**2. Environment Interface (programmatic access)**
```java
@RestController
public class AccountController {
    
    @Autowired
    private Environment environment;
    
    @GetMapping("/info")
    public String getInfo() {
        String version = environment.getProperty("build.version");
        String message = environment.getProperty("accounts.message");
        return "Version: " + version;
    }
}
```

**Advantages :** Dynamic access, flexible
**Disadvantages :** Less type-safe, more verbose

**3. @ConfigurationProperties (BEST PRACTICE)**
```java
@Component
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfigProperties {
    
    private String message;
    private String buildVersion;
    private ContactDetails contactDetails;
    private List<String> onCallSupport;
    
    // Getters and Setters
    
    @Data
    public static class ContactDetails {
        private String name;
        private String email;
    }
}

// Usage
@RestController
public class AccountController {
    
    @Autowired
    private AccountsConfigProperties config;
    
    @GetMapping("/contact")
    public ContactDetails getContact() {
        return config.getContactDetails();
    }
}
```

**Advantages :**
- ✅ Type-safe
- ✅ Hierarchical structure (nested objects)
- ✅ Validation (@Validated + @NotNull, @Min, etc.)
- ✅ IDE auto-completion
- ✅ Reusable

**Recommendation :** Use **@ConfigurationProperties** for complex structured configurations.

---

### **3. Explain the Precedence Order of configurations in Spring Boot**

**Answer :**
Spring Boot loads configurations from multiple sources. In case of conflict, the priority order (from highest to lowest) is :

**1. Command line arguments** (highest priority)
```bash
java -jar app.jar --server.port=9090 --spring.profiles.active=prod
```

**2. JVM system properties**
```bash
java -Dserver.port=9091 -Dspring.profiles.active=prod -jar app.jar
```

**3. OS environment variables**
```bash
export SERVER_PORT=9092
export SPRING_PROFILES_ACTIVE=prod
java -jar app.jar
```

**4. application-{profile}.properties/yml** (active profile)
```yaml
# application-prod.yml
server:
  port: 9093
```

**5. application.properties/yml** (main file)
```yaml
# application.yml
server:
  port: 8080
```

**6. @PropertySource**
```java
@Configuration
@PropertySource("classpath:custom.properties")
public class CustomConfig { }
```

**7. Default properties**

**Practical example :**
```yaml
# application.yml
server:
  port: 8080

# application-prod.yml
server:
  port: 8081

# Command line
java -jar app.jar --server.port=8082

# Result : port 8082 (command line wins)
```

**Note :** The rule is "**last wins**" - the source with the highest priority overrides the others.

---

### **4. What are Spring Profiles and how to use them ?**

**Answer :**
**Spring Profiles** allow you to define different configurations depending on the environment (dev, qa, prod).

**File structure :**
```
application.yml           # Common configuration
application-dev.yml       # Override for dev
application-qa.yml        # Override for qa
application-prod.yml      # Override for prod
```

**Activating a profile :**

**Option 1 : In application.yml**
```yaml
spring:
  profiles:
    active: prod
```

**Option 2 : Command line**
```bash
java -jar app.jar --spring.profiles.active=prod
```

**Option 3 : Environment variable**
```bash
export SPRING_PROFILES_ACTIVE=prod
java -jar app.jar
```

**Option 4 : JVM property**
```bash
java -Dspring.profiles.active=prod -jar app.jar
```

**Example of configuration by profile :**
```yaml
# application.yml (common)
spring:
  application:
    name: accounts
  
build:
  version: "3.0"

---
# application-dev.yml
spring:
  datasource:
    url: jdbc:mysql://localhost:3306/dev_db
    username: dev_user
    password: dev_pass

accounts:
  message: "Welcome to dev environment"

---
# application-prod.yml
spring:
  datasource:
    url: jdbc:mysql://prod-server:3306/prod_db
    username: prod_user
    password: prod_pass

accounts:
  message: "Welcome to production"
```

**Conditional bean by profile :**
```java
@Configuration
public class DataSourceConfig {
    
    @Bean
    @Profile("dev")
    public DataSource devDataSource() {
        // Dev configuration
        return new HikariDataSource();
    }
    
    @Bean
    @Profile("prod")
    public DataSource prodDataSource() {
        // Prod configuration
        return new HikariDataSource();
    }
}
```

---

### **5. What is Spring Cloud Config Server and why use it ?**

**Answer :**
**Spring Cloud Config Server** is a centralized server that provides configurations for all microservices in a distributed architecture.

**Architecture :**
```
┌─────────────────────────────────────────┐
│   Spring Cloud Config Server            │
│         (Port 8071)                      │
│  ┌───────────────────────────────────┐  │
│  │  Configuration Sources:           │  │
│  │  - Classpath                      │  │
│  │  - Filesystem                     │  │
│  │  - Git Repository (recommended)   │  │
│  └───────────────────────────────────┘  │
└─────────────────────────────────────────┘
              ▲        ▲        ▲
              │        │        │
    ┌─────────┴─┐  ┌──┴───┐  ┌─┴─────┐
    │ Accounts  │  │ Loans │  │ Cards │
    │   MS      │  │  MS   │  │  MS   │
    └───────────┘  └───────┘  └───────┘
```

**Why use it :**

**Without Config Server :**
- Each microservice has its own configs
- Change = redeploy each service
- No centralization
- Difficult to maintain consistency

**With Config Server :**
- ✅ **Centralization** : all configs in one place
- ✅ **Version control** : history with Git
- ✅ **Multiple environments** : dev, qa, prod
- ✅ **Dynamic refresh** : without redeployment
- ✅ **Security** : encryption of secrets
- ✅ **Audit** : who changed what and when
- ✅ **Rollback** : easy to revert

**Use case :** In an architecture with 10+ microservices, changing a database URL would require 10 redeployments. With Config Server, a single centralized change.

---

### **6. How to configure a Spring Cloud Config Server from a Git repository ?**

**Answer :**

**Step 1 : Create the Config Server**

**Dependencies (pom.xml) :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-config-server</artifactId>
</dependency>

<dependencyManagement>
    <dependencies>
        <dependency>
            <groupId>org.springframework.cloud</groupId>
            <artifactId>spring-cloud-dependencies</artifactId>
            <version>2023.0.0</version>
            <type>pom</type>
            <scope>import</scope>
        </dependency>
    </dependencies>
</dependencyManagement>
```

**Main class :**
```java
@SpringBootApplication
@EnableConfigServer  // Enables the Config Server
public class ConfigServerApplication {
    public static void main(String[] args) {
        SpringApplication.run(ConfigServerApplication.class, args);
    }
}
```

**Configuration (application.yml) :**
```yaml
server:
  port: 8071

spring:
  application:
    name: configserver
  profiles:
    active: git  # Enables the Git profile
  cloud:
    config:
      server:
        git:
          uri: "https://github.com/anwerESS/microservices-config.git"
          default-label: main  # Default branch
          timeout: 5
          clone-on-start: true  # Clone on startup
          force-pull: true      # Force pull on every request
```

**Step 2 : Create the Git repository**

Repository structure :
```
microservices-config/
├── accounts.yml
├── accounts-dev.yml
├── accounts-qa.yml
├── accounts-prod.yml
├── loans.yml
├── loans-dev.yml
├── loans-qa.yml
├── loans-prod.yml
├── cards.yml
├── cards-dev.yml
├── cards-qa.yml
└── cards-prod.yml
```

**Example file (accounts-prod.yml) :**
```yaml
build:
  version: "1.0"

accounts:
  message: "Welcome to EazyBank accounts prod APIs"
  contactDetails:
    name: "Reine Aishwarya - Product Owner"
    email: "aishwarya@eazybank.com"
  onCallSupport:
    - "(453) 392-4829"
    - "(236) 203-0384"
```

**Step 3 : Configure Config Clients (microservices)**

**Dependency :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-config</artifactId>
</dependency>
```

**Configuration (application.yml) :**
```yaml
spring:
  application:
    name: accounts  # Service name
  profiles:
    active: prod    # Active profile
  config:
    import: "optional:configserver:http://localhost:8071"  # Config Server URL
```

**Step 4 : Test**
```bash
# Start the Config Server
mvn spring-boot:run

# Access the configurations
curl http://localhost:8071/accounts/prod
curl http://localhost:8071/loans/qa
curl http://localhost:8071/cards/dev
```

---

### **7. What is the difference between Classpath, Filesystem, and Git as config sources ?**

**Answer :**

| Source | Configuration | Advantages | Disadvantages | Use Case |
|--------|--------------|-----------|---------------|----------|
| **Classpath** | `search-locations: classpath:/config` | ✅ Simple<br>✅ No external dependency | ❌ Configs inside the JAR<br>❌ No version control<br>❌ Change = rebuild | Dev/Testing |
| **Filesystem** | `search-locations: file:///path/to/config` | ✅ Easy to modify<br>✅ No rebuild | ❌ No version control<br>❌ Depends on local filesystem<br>❌ Not scalable | Local Dev |
| **Git** | `uri: https://github.com/...` | ✅ **Version control**<br>✅ **Collaboration**<br>✅ **History**<br>✅ **Rollback**<br>✅ **Cloud centralization** | ❌ Network dependency<br>❌ More complex | **Production (RECOMMENDED)** |

**Classpath configuration :**
```yaml
spring:
  profiles:
    active: native
  cloud:
    config:
      server:
        native:
          search-locations: classpath:/config
```

**Filesystem configuration :**
```yaml
spring:
  profiles:
    active: native
  cloud:
    config:
      server:
        native:
          search-locations: "file:///home/user/spring-config/config"
```

**Git configuration :**
```yaml
spring:
  profiles:
    active: git
  cloud:
    config:
      server:
        git:
          uri: "https://github.com/user/config-repo.git"
          default-label: main
```

**Recommendation :** Use **Git** for production as it provides version control, collaboration, and history.

---

### **8. How does runtime configuration refresh work (without restarting) ?**

**Answer :**
By default, Spring loads configurations at startup. To refresh without restarting, use **@RefreshScope** + **Actuator**.

**Step 1 : Add @RefreshScope**
```java
@RestController
@RefreshScope  // Enables dynamic refresh
public class AccountController {
    
    @Autowired
    private AccountsConfigProperties config;
    
    @GetMapping("/contact-info")
    public AccountsConfigProperties getContactInfo() {
        return config;
    }
}

@Component
@ConfigurationProperties(prefix = "accounts")
@RefreshScope  // Important also on the config bean
public class AccountsConfigProperties {
    private String message;
    private ContactDetails contactDetails;
    // ...
}
```

**Step 2 : Expose the refresh endpoint**
```yaml
management:
  endpoints:
    web:
      exposure:
        include: refresh
```

**Step 3 : Refresh workflow**
```bash
# 1. Change the config in Git
git commit -m "Update accounts message"
git push

# 2. Call the refresh endpoint
curl -X POST http://localhost:8080/actuator/refresh

# 3. New config is loaded without restart
```

**Endpoint response :**
```json
[
  "config.client.version",
  "accounts.message",
  "accounts.contactDetails.name"
]
```

**Limitation :** You have to call `/actuator/refresh` on **each instance** of each microservice.

**Solution :** Use **Spring Cloud Bus** for automatic refresh.

---

### **9. What is Spring Cloud Bus and how to use it for automatic refresh ?**

**Answer :**
**Spring Cloud Bus** connects all microservices via a **message broker** (RabbitMQ or Kafka) to automatically propagate refresh events.

**Architecture :**
```
┌───────────────┐
│ Config Server │ → Change config
└───────┬───────┘
        │
        ▼
┌───────────────┐
│   RabbitMQ    │ ←── Message Bus
└───────┬───────┘
        │
   ┌────┴────┬────────┬────────┐
   ▼         ▼        ▼        ▼
┌────────┐ ┌────┐ ┌─────┐ ┌─────┐
│Accounts│ │Loans│ │Cards│ │ ... │
│  MS    │ │ MS │ │ MS  │ │     │
└────────┘ └────┘ └─────┘ └─────┘
    All refreshed automatically
```

**Configuration :**

**1. Dependency (all microservices) :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-bus-amqp</artifactId>
</dependency>
```

**2. RabbitMQ configuration (all microservices) :**
```yaml
spring:
  rabbitmq:
    host: "localhost"
    port: 5672
    username: "guest"
    password: "guest"

management:
  endpoints:
    web:
      exposure:
        include: busrefresh  # Exposes /actuator/busrefresh
```

**3. Start RabbitMQ :**
```bash
docker run -it --rm --name rabbitmq \
  -p 5672:5672 \
  -p 15672:15672 \
  rabbitmq:4.0-management
```

**4. Refresh all services at once :**
```bash
# Change config in Git
git commit -m "Update config"
git push

# Call busrefresh on ANY microservice
curl -X POST http://localhost:8080/actuator/busrefresh

# All microservices connected to RabbitMQ are refreshed automatically
```

**Advantages :**
- ✅ **A single call** refreshes all services
- ✅ **Scalable** : works with hundreds of instances
- ✅ **Automatic** via message bus

---

### **10. How to fully automate refresh with GitHub Webhook ?**

**Answer :**
With **Spring Cloud Config Monitor** + **GitHub Webhook**, the refresh happens **automatically** as soon as you push to Git.

**Architecture :**
```
Developer → Git Push
     ↓
GitHub Webhook
     ↓
Config Server /monitor endpoint
     ↓
RabbitMQ
     ↓
All microservices refreshed automatically
```

**Configuration :**

**1. Add Spring Cloud Config Monitor (Config Server) :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-config-monitor</artifactId>
</dependency>
```

**Note :** This dependency automatically exposes the `/monitor` endpoint.

**2. RabbitMQ configuration (same as before)**

**3. Configure GitHub Webhook :**
- Go to GitHub repository → Settings → Webhooks
- Add webhook :
  - **Payload URL** : `http://your-server:8071/monitor`
  - **Content type** : `application/json`
  - **Events** : Just the push event
  - **Active** : ✅

**4. Automatic flow :**
```
1. Developer changes config and pushes Git
2. GitHub sends webhook to /monitor
3. Config Server receives the webhook
4. Config Server publishes event to RabbitMQ
5. All microservices receive the event
6. All microservices refresh automatically
```

**Advantages :**
- ✅ **100% automatic** : no manual action needed
- ✅ **Real-time** : refresh on push
- ✅ **Production-ready**
- ✅ **Audit trail** : Git history + webhook logs

---

### **11. How to secure sensitive configurations (passwords, secrets) ?**

**Answer :**
Use **Encryption** in Spring Cloud Config Server.

**Option 1 : Symmetric encryption (shared key)**

**1. Configuration (Config Server) :**
```yaml
encrypt:
  key: "MySecretEncryptionKey123"  # Symmetric encryption key
```

**2. Encrypt a value :**
```bash
curl http://localhost:8071/encrypt -d "mySecretPassword"
# Result : AQBhZGFzZGFz...encrypted-value...zxcvbnm
```

**3. Use in config files :**
```yaml
# accounts-prod.yml
spring:
  datasource:
    password: '{cipher}AQBhZGFzZGFz...encrypted-value...zxcvbnm'
```

**4. Automatic decryption :**
Clients receive the **decrypted** value automatically.

**Option 2 : Asymmetric encryption (public/private key pair)**

**1. Generate a keystore :**
```bash
keytool -genkeypair -alias configserver -keyalg RSA \
  -keystore config-server.jks -storepass password
```

**2. Configuration (Config Server) :**
```yaml
encrypt:
  key-store:
    location: classpath:/config-server.jks
    password: password
    alias: configserver
```

**3. Encrypt with the public key :**
```bash
curl http://localhost:8071/encrypt -d "mySecretPassword"
```

**Asymmetric advantages :**
- ✅ Public key to encrypt (can be shared)
- ✅ Private key to decrypt (kept secret)
- ✅ More secure

---

### **12. How to test that a microservice is fetching its config from the Config Server ?**

**Answer :**

**Test 1 : Verify connection to Config Server**
```bash
# Start the Config Server
cd configserver
mvn spring-boot:run

# Verify that the Config Server returns configs
curl http://localhost:8071/accounts/prod

# Should return JSON with the configurations
```

**Test 2 : Create a test endpoint in the microservice**
```java
@RestController
@RefreshScope
public class ConfigTestController {
    
    @Value("${build.version}")
    private String buildVersion;
    
    @Autowired
    private AccountsConfigProperties config;
    
    @GetMapping("/config-test")
    public Map<String, Object> testConfig() {
        Map<String, Object> result = new HashMap<>();
        result.put("buildVersion", buildVersion);
        result.put("message", config.getMessage());
        result.put("contactDetails", config.getContactDetails());
        return result;
    }
}
```

**Test 3 : Start the microservice and call the endpoint**
```bash
# Start the microservice
cd accounts
mvn spring-boot:run

# Test the endpoint
curl http://localhost:8080/config-test
```

**Test 4 : Test dynamic refresh**
```bash
# 1. Change a config in Git (e.g. message)
# 2. Git Push
git commit -m "Update message"
git push

# 3. Refresh the microservice
curl -X POST http://localhost:8080/actuator/refresh

# 4. Verify that the config has changed
curl http://localhost:8080/config-test
```

**Test 5 : Automated tests**
```java
@SpringBootTest
@AutoConfigureMockMvc
class ConfigIntegrationTest {
    
    @Autowired
    private AccountsConfigProperties config;
    
    @Test
    void testConfigLoaded() {
        assertNotNull(config);
        assertNotNull(config.getMessage());
        assertEquals("1.0", config.getBuildVersion());
    }
    
    @Test
    void testConfigFromProfile() {
        // Verify that the correct profile config is loaded
        assertTrue(config.getMessage().contains("prod"));
    }
}
```

---

### **13. What happens if the Config Server is down ?**

**Answer :**
If the Config Server is unavailable when a microservice starts, several strategies can be used :

**Option 1 : Fail Fast (default)**
```yaml
spring:
  cloud:
    config:
      fail-fast: true  # Fails immediately if Config Server is down
```
→ The microservice **does not start** if Config Server is down.

**Option 2 : Graceful Fallback**
```yaml
spring:
  config:
    import: "optional:configserver:http://localhost:8071"  # "optional" keyword
  cloud:
    config:
      fail-fast: false  # Continue even if Config Server is down
```
→ The microservice starts with **local configs** (application.yml).

**Option 3 : Retry with backoff**
```yaml
spring:
  cloud:
    config:
      fail-fast: true
      retry:
        initial-interval: 1000  # 1 second
        max-attempts: 6         # 6 attempts
        max-interval: 2000      # 2 seconds max
        multiplier: 1.1         # Exponential multiplier
```
→ Retries the connection several times before failing.

**Production Best Practice :**
- ✅ Deploy **multiple instances** of Config Server (HA)
- ✅ Use a **load balancer** in front of Config Server
- ✅ Enable **retries** with backoff
- ✅ Have **local fallback configs**

```yaml
spring:
  config:
    import:
      - "optional:configserver:http://config-server-1:8071"
      - "optional:configserver:http://config-server-2:8071"
  cloud:
    config:
      retry:
        max-attempts: 10
```

---

### **14. How to manage different configurations for different environments (dev, qa, prod) ?**

**Answer :**

**Structure in Git :**
```
config-repo/
├── accounts.yml           # Common config (all envs)
├── accounts-dev.yml       # Override for dev
├── accounts-qa.yml        # Override for qa
├── accounts-prod.yml      # Override for prod
├── loans.yml
├── loans-dev.yml
├── loans-qa.yml
└── loans-prod.yml
```

**Example configs :**

**accounts.yml (common) :**
```yaml
build:
  version: "3.0"

accounts:
  message: "Welcome to EazyBank accounts"
  contactDetails:
    name: "John Doe - Developer"
    email: "john@eazybank.com"
```

**accounts-dev.yml :**
```yaml
spring:
  datasource:
    url: jdbc:mysql://localhost:3306/dev_db
    username: dev_user
    password: dev_pass

accounts:
  message: "Welcome to DEV environment"
  
logging:
  level:
    root: DEBUG  # Detailed logs in dev
```

**accounts-qa.yml :**
```yaml
spring:
  datasource:
    url: jdbc:mysql://qa-server:3306/qa_db
    username: qa_user
    password: qa_pass

accounts:
  message: "Welcome to QA environment"
  contactDetails:
    name: "Smitha Ray - QA Lead"
    email: "smitha@eazybank.com"

logging:
  level:
    root: INFO
```

**accounts-prod.yml :**
```yaml
spring:
  datasource:
    url: jdbc:mysql://prod-server:3306/prod_db
    username: '{cipher}AQBhZGF...encrypted...'
    password: '{cipher}XYZ123...encrypted...'

accounts:
  message: "Welcome to PRODUCTION"
  contactDetails:
    name: "Reine Aishwarya - Product Owner"
    email: "aishwarya@eazybank.com"

logging:
  level:
    root: WARN  # Minimal logs in prod
```

**Activating the profile in the microservice :**

**Dev :**
```bash
java -jar accounts.jar --spring.profiles.active=dev
```

**QA :**
```bash
java -jar accounts.jar --spring.profiles.active=qa
```

**Prod :**
```bash
java -jar accounts.jar --spring.profiles.active=prod
```

**Configuration merge :**
The Config Server **merges** automatically :
1. accounts.yml (base)
2. accounts-{profile}.yml (override)

```
accounts.yml
    ↓
accounts-prod.yml (override)
    ↓
Final result
```

---

### **15. What are the best practices for Spring Cloud Config ?**

**Answer :**

**1. Use Git as a source (not classpath/filesystem)**
```yaml
spring:
  cloud:
    config:
      server:
        git:
          uri: "https://github.com/org/config-repo.git"
```
✅ Version control, collaboration, history

**2. Separate configs by service AND by environment**
```
accounts.yml
accounts-dev.yml
accounts-qa.yml
accounts-prod.yml
```

**3. Use @ConfigurationProperties (not @Value)**
```java
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfig {
    private String message;
    private ContactDetails contactDetails;
}
```
✅ Type-safe, validation, structure

**4. Enable encryption for secrets**
```yaml
encrypt:
  key: "MySecretKey"

# In config
database:
  password: '{cipher}encrypted-value'
```

**5. Implement automatic refresh with Bus + Webhook**
```yaml
management:
  endpoints:
    web:
      exposure:
        include: busrefresh
```

**6. Use @RefreshScope on beans that change**
```java
@Component
@RefreshScope
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfig {
    // Config that can change at runtime
}
```

**7. Deploy Config Server in High Availability (HA)**
```yaml
spring:
  config:
    import:
      - "configserver:http://config-server-1:8071"
      - "configserver:http://config-server-2:8071"
```

**8. Use retry mechanisms**
```yaml
spring:
  cloud:
    config:
      retry:
        max-attempts: 6
        initial-interval: 1000
```

**9. Secure the Config Server**
```java
@Configuration
public class SecurityConfig {
    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
        http
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/actuator/**").hasRole("ADMIN")
                .anyRequest().authenticated()
            )
            .httpBasic();
        return http.build();
    }
}
```

**10. Never commit plain secrets to Git**
```yaml
# ❌ Bad
database:
  password: "mySecretPassword123"

# ✅ Good
database:
  password: '{cipher}AQBhZGFzZGFz...encrypted...'
```

**11. Document configurations**
```yaml
# accounts.yml
# Configuration for the Accounts microservice
# Last modified : 2025-10-30
# Owner : Team Banking

accounts:
  message: "Welcome message displayed to users"
  contactDetails:
    name: "Contact person name"
    email: "Contact email"
```

**12. Test configurations**
```java
@SpringBootTest
class ConfigTest {
    @Autowired
    private AccountsConfig config;
    
    @Test
    void testConfigLoaded() {
        assertNotNull(config.getMessage());
    }
}
```

**13. Use sensible default values**
```yaml
# accounts.yml (base with default values)
accounts:
  timeout: 5000
  retry-attempts: 3
  
# accounts-prod.yml (override only what changes)
accounts:
  timeout: 10000  # Higher timeout in prod
```

**14. Log config changes**
```java
@Component
@RefreshScope
public class ConfigChangeListener {
    
    private static final Logger log = LoggerFactory.getLogger(ConfigChangeListener.class);
    
    @EventListener
    public void onRefresh(RefreshScopeRefreshedEvent event) {
        log.info("Configuration refreshed for: {}", event.getName());
    }
}
```

**15. Have a rollback plan**
```bash
# Git makes rollback easy
git revert <commit-hash>
git push

# Then refresh the services
curl -X POST http://localhost:8071/actuator/busrefresh
```

---

### **16. How to validate configurations with @ConfigurationProperties ?**

**Answer :**
Use **@Validated** + **Bean Validation** to automatically validate configurations at load time.

```java
@Component
@ConfigurationProperties(prefix = "accounts")
@Validated  // Enables validation
public class AccountsConfigProperties {
    
    @NotBlank(message = "Message cannot be blank")
    private String message;
    
    @NotBlank(message = "Build version is required")
    @Pattern(regexp = "\\d+\\.\\d+", message = "Version format should be X.Y")
    private String buildVersion;
    
    @Valid  // Validates nested objects
    @NotNull(message = "Contact details are required")
    private ContactDetails contactDetails;
    
    @NotEmpty(message = "At least one support number is required")
    @Size(min = 1, max = 5, message = "Support numbers should be between 1 and 5")
    private List<String> onCallSupport;
    
    @Min(value = 1000, message = "Timeout must be at least 1000ms")
    @Max(value = 60000, message = "Timeout cannot exceed 60000ms")
    private Integer timeout = 5000;
    
    // Getters and Setters
    
    @Data
    public static class ContactDetails {
        
        @NotBlank(message = "Contact name is required")
        private String name;
        
        @Email(message = "Invalid email format")
        @NotBlank(message = "Contact email is required")
        private String email;
    }
}
```

**If validation fails at startup :**
```
***************************
APPLICATION FAILED TO START
***************************

Description:

Binding to target org.springframework.boot.context.properties.bind.BindException: 
Failed to bind properties under 'accounts' to AccountsConfigProperties

Reason: 
- accounts.message: Message cannot be blank
- accounts.contactDetails.email: Invalid email format
```

**Advantages :**
- ✅ **Fail fast** : error at startup, not in production
- ✅ **Documentation** : constraints are in the code
- ✅ **Type safety** : type errors caught early

---

### **17. How to manage configurations in a Kubernetes environment ?**

**Answer :**

**Option 1 : ConfigMaps (non-sensitive configs)**

**1. Create a ConfigMap :**
```yaml
# accounts-configmap.yml
apiVersion: v1
kind: ConfigMap
metadata:
  name: accounts-config
data:
  application.yml: |
    spring:
      profiles:
        active: prod
      config:
        import: "configserver:http://configserver-service:8071"
    
    accounts:
      message: "Welcome to Kubernetes deployment"
```

**2. Mount the ConfigMap in the pod :**
```yaml
# accounts-deployment.yml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: accounts
spec:
  template:
    spec:
      containers:
      - name: accounts
        image: accounts:1.0
        volumeMounts:
        - name: config-volume
          mountPath: /config
      volumes:
      - name: config-volume
        configMap:
          name: accounts-config
```

**Option 2 : Secrets (sensitive configs)**

**1. Create a Secret :**
```bash
kubectl create secret generic accounts-secret \
  --from-literal=db-password='mySecretPassword' \
  --from-literal=api-key='abc123xyz'
```

**2. Use the Secret :**
```yaml
# accounts-deployment.yml
spec:
  containers:
  - name: accounts
    env:
    - name: DB_PASSWORD
      valueFrom:
        secretKeyRef:
          name: accounts-secret
          key: db-password
    - name: API_KEY
      valueFrom:
        secretKeyRef:
          name: accounts-secret
          key: api-key
```

**Option 3 : Config Server in Kubernetes**

**1. Deploy Config Server :**
```yaml
# configserver-deployment.yml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: configserver
spec:
  replicas: 2  # High Availability
  template:
    spec:
      containers:
      - name: configserver
        image: configserver:1.0
        ports:
        - containerPort: 8071
---
apiVersion: v1
kind: Service
metadata:
  name: configserver-service
spec:
  selector:
    app: configserver
  ports:
  - port: 8071
    targetPort: 8071
```

**2. Clients connect to the service :**
```yaml
spring:
  config:
    import: "configserver:http://configserver-service:8071"
```

**Best Practice :** Combine ConfigMaps (non-sensitive configs) + Secrets (credentials) + Config Server (centralization).

---

### **18. How to handle configuration migrations (structure changes) ?**

**Answer :**

**Scenario :** Rename a property without breaking old clients.

**Old config :**
```yaml
accounts:
  contact-name: "John Doe"
  contact-email: "john@example.com"
```

**New config (improved structure) :**
```yaml
accounts:
  contactDetails:
    name: "John Doe"
    email: "john@example.com"
```

**Solution : Progressive migration**

**Step 1 : Support both formats temporarily**
```java
@Component
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfig {
    
    // New structure
    private ContactDetails contactDetails;
    
    // Old structure (deprecated)
    @Deprecated
    private String contactName;
    
    @Deprecated
    private String contactEmail;
    
    // Getters and Setters
    
    // Automatic migration
    @PostConstruct
    public void migrate() {
        if (contactDetails == null && contactName != null) {
            // Migrate old format to new one
            contactDetails = new ContactDetails();
            contactDetails.setName(contactName);
            contactDetails.setEmail(contactEmail);
            
            log.warn("Using deprecated config format. Please update to contactDetails structure.");
        }
    }
}
```

**Step 2 : Transition phase (3-6 months)**
- Both formats work
- Warning logs for the old format
- Migration documentation

**Step 3 : Remove the old format**
```java
@Component
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfig {
    
    // Only the new structure
    private ContactDetails contactDetails;
}
```

**Alternative : Use @DeprecatedConfigurationProperty**
```java
@Component
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfig {
    
    private ContactDetails contactDetails;
    
    @Deprecated
    @DeprecatedConfigurationProperty(replacement = "accounts.contactDetails.name")
    public void setContactName(String name) {
        if (contactDetails == null) {
            contactDetails = new ContactDetails();
        }
        contactDetails.setName(name);
    }
}
```

---

### **19. How to debug configuration problems ?**

**Answer :**

**Problem 1 : Config not loaded**

**Checks :**
```bash
# 1. Verify that the Config Server is running
curl http://localhost:8071/actuator/health

# 2. Verify that the Config Server returns configs
curl http://localhost:8071/accounts/prod

# 3. Check microservice logs at startup
# Should see : "Located property source: ..."
```

**Enable debug logs :**
```yaml
logging:
  level:
    org.springframework.cloud.config: DEBUG
    org.springframework.web: DEBUG
```

**Problem 2 : Wrong profile loaded**

**Check the active profile :**
```java
@RestController
public class DebugController {
    
    @Autowired
    private Environment environment;
    
    @GetMapping("/debug/profiles")
    public String[] getActiveProfiles() {
        return environment.getActiveProfiles();
    }
    
    @GetMapping("/debug/property")
    public String getProperty(@RequestParam String key) {
        return environment.getProperty(key);
    }
}
```

**Problem 3 : Refresh not working**

**Checklist :**
```java
// 1. @RefreshScope on the bean ?
@Component
@RefreshScope  // ← Important
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfig { }

// 2. Endpoint exposed ?
// application.yml
management:
  endpoints:
    web:
      exposure:
        include: refresh  // ← Important

// 3. Config changed in Git and pushed ?
git status
git log

// 4. Call refresh
curl -X POST http://localhost:8080/actuator/refresh

// 5. Check logs
// Should see : "Refreshing beans..."
```

**Problem 4 : RabbitMQ connection failed**

```bash
# Check that RabbitMQ is running
docker ps | grep rabbitmq

# Check connections
curl http://localhost:15672/api/connections

# RabbitMQ logs
docker logs rabbitmq

# Test connection
telnet localhost 5672
```

**Useful debugging endpoint :**
```java
@RestController
public class ConfigDebugController {
    
    @Autowired
    private Environment environment;
    
    @Autowired
    private AccountsConfig config;
    
    @GetMapping("/debug/config")
    public Map<String, Object> debugConfig() {
        Map<String, Object> debug = new HashMap<>();
        
        // Active profiles
        debug.put("activeProfiles", environment.getActiveProfiles());
        
        // Config Server URI
        debug.put("configServerUri", environment.getProperty("spring.config.import"));
        
        // Current config
        debug.put("currentConfig", config);
        
        // All "accounts.*" properties
        Map<String, String> accountsProps = new HashMap<>();
        MutablePropertySources sources = ((ConfigurableEnvironment) environment).getPropertySources();
        sources.forEach(source -> {
            if (source instanceof EnumerablePropertySource) {
                for (String name : ((EnumerablePropertySource<?>) source).getPropertyNames()) {
                    if (name.startsWith("accounts.")) {
                        accountsProps.put(name, environment.getProperty(name));
                    }
                }
            }
        });
        debug.put("accountsProperties", accountsProps);
        
        return debug;
    }
}
```

---

### **20. What are the alternatives to Spring Cloud Config ?**

**Answer :**

| Solution | Description | Advantages | Disadvantages |
|----------|-------------|-----------|---------------|
| **Spring Cloud Config** | Centralized config server | ✅ Native Spring integration<br>✅ Git support<br>✅ Dynamic refresh | ❌ Additional infrastructure<br>❌ Single point of failure |
| **Consul** | Service mesh with KV store | ✅ Service discovery included<br>✅ Health checks<br>✅ Multi-datacenter | ❌ More complex<br>❌ Learning curve |
| **Vault** (HashiCorp) | Secrets management | ✅ Advanced security<br>✅ Automatic rotation<br>✅ Audit logs | ❌ Focus on secrets (not general configs)<br>❌ Heavier |
| **etcd** | Distributed KV store | ✅ Highly available<br>✅ Performant<br>✅ Used by Kubernetes | ❌ No direct Spring integration<br>❌ Low-level API |
| **Apache ZooKeeper** | Coordination service | ✅ Mature<br>✅ Reliable | ❌ Complex to maintain<br>❌ Older technology |
| **AWS Systems Manager Parameter Store** | AWS managed | ✅ Fully managed<br>✅ AWS integration<br>✅ Encryption | ❌ AWS lock-in<br>❌ Cost |
| **Kubernetes ConfigMaps + Secrets** | K8s native | ✅ K8s integration<br>✅ Simple<br>✅ No additional infra | ❌ No version control<br>❌ No native dynamic refresh |

**Comparison for Spring Boot :**

**Spring Cloud Config :**
```yaml
spring:
  config:
    import: "configserver:http://localhost:8071"
```

**Consul :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-consul-config</artifactId>
</dependency>
```
```yaml
spring:
  cloud:
    consul:
      host: localhost
      port: 8500
      config:
        enabled: true
```

**Vault :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-vault-config</artifactId>
</dependency>
```
```yaml
spring:
  cloud:
    vault:
      host: localhost
      port: 8200
      authentication: TOKEN
      token: ${VAULT_TOKEN}
```

**AWS Parameter Store :**
```xml
<dependency>
    <groupId>io.awspring.cloud</groupId>
    <artifactId>spring-cloud-aws-parameter-store-config</artifactId>
</dependency>
```
```yaml
aws:
  paramstore:
    enabled: true
    prefix: /config
    profile-separator: _
```

**Recommendation :**
- **Spring Cloud Config** : Good starting point, well integrated with Spring
- **Consul** : If you need service discovery + config
- **Vault** : If security of secrets is the main focus
- **K8s ConfigMaps** : If already in Kubernetes and needs are simple

---

### **21. How to manage configurations in a multi-tenant environment ?**

**Answer :**

**Scenario :** SaaS application with multiple clients (tenants), each with their own configuration.

**Approach 1 : Profiles per tenant**

**Git structure :**
```
config-repo/
├── accounts.yml               # Common config
├── accounts-tenant-acme.yml   # Config for ACME Corp
├── accounts-tenant-globex.yml # Config for Globex
└── accounts-tenant-initech.yml # Config for Initech
```

**Dynamic activation :**
```java
@Component
public class TenantProfileResolver {
    
    @Autowired
    private Environment environment;
    
    public String resolveTenantProfile(String tenantId) {
        return "tenant-" + tenantId;
    }
}

// At runtime
String tenantId = extractTenantFromRequest();  // Ex: "acme"
String profile = "tenant-" + tenantId;  // "tenant-acme"
```

**Approach 2 : Hierarchical config in Git**

**Structure :**
```
config-repo/
├── common/
│   └── accounts.yml         # Common config
├── tenants/
│   ├── acme/
│   │   └── accounts.yml     # ACME config
│   ├── globex/
│   │   └── accounts.yml     # Globex config
│   └── initech/
│       └── accounts.yml     # Initech config
```

**Config Server :**
```yaml
spring:
  cloud:
    config:
      server:
        git:
          uri: "https://github.com/org/config-repo.git"
          search-paths: "common,tenants/{profile}"
```

**Approach 3 : Database-backed configs**

**1. Create a config table :**
```sql
CREATE TABLE tenant_configs (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    tenant_id VARCHAR(50) NOT NULL,
    config_key VARCHAR(200) NOT NULL,
    config_value TEXT,
    UNIQUE KEY uk_tenant_key (tenant_id, config_key)
);

INSERT INTO tenant_configs VALUES
(1, 'acme', 'accounts.message', 'Welcome ACME Corp'),
(2, 'acme', 'accounts.maxTransactions', '1000'),
(3, 'globex', 'accounts.message', 'Welcome Globex'),
(4, 'globex', 'accounts.maxTransactions', '5000');
```

**2. Config Provider :**
```java
@Component
public class DatabaseConfigProvider {
    
    @Autowired
    private JdbcTemplate jdbcTemplate;
    
    public Map<String, String> getConfigForTenant(String tenantId) {
        String sql = "SELECT config_key, config_value FROM tenant_configs WHERE tenant_id = ?";
        
        Map<String, String> config = new HashMap<>();
        jdbcTemplate.query(sql, rs -> {
            config.put(rs.getString("config_key"), rs.getString("config_value"));
        }, tenantId);
        
        return config;
    }
}
```

**3. Dynamic Property Source :**
```java
@Component
public class TenantConfigPropertySource extends PropertySource<Map<String, String>> {
    
    private final DatabaseConfigProvider configProvider;
    private final String tenantId;
    
    public TenantConfigPropertySource(String tenantId, DatabaseConfigProvider provider) {
        super("tenantConfig-" + tenantId);
        this.tenantId = tenantId;
        this.configProvider = provider;
        this.source = configProvider.getConfigForTenant(tenantId);
    }
    
    @Override
    public Object getProperty(String name) {
        return source.get(name);
    }
}
```

**Approach 4 : Hybrid (Git + Database)**

```java
@Configuration
public class MultiTenantConfigStrategy {
    
    // Common config from Git (via Config Server)
    @Bean
    @ConfigurationProperties(prefix = "accounts.common")
    public CommonConfig commonConfig() {
        return new CommonConfig();
    }
    
    // Tenant-specific config from Database
    @Bean
    public TenantConfigProvider tenantConfigProvider() {
        return new TenantConfigProvider();
    }
}

@Service
public class ConfigService {
    
    @Autowired
    private CommonConfig commonConfig;
    
    @Autowired
    private TenantConfigProvider tenantProvider;
    
    public Config getConfigForTenant(String tenantId) {
        Config config = new Config();
        
        // Merge common + tenant-specific
        config.setCommon(commonConfig);
        config.setTenantSpecific(tenantProvider.getConfig(tenantId));
        
        return config;
    }
}
```

---

**Final tip :** Be ready to explain the complete architecture of a centralized configuration solution with automatic refresh. Interviewers love diagrams and concrete examples ! 💪

**Key points to remember :**
- Spring Cloud Config centralizes configurations
- 3 sources : Classpath, Filesystem, **Git (best practice)**
- 3 ways to read : @Value, Environment, **@ConfigurationProperties (best practice)**
- Refresh : Manual (actuator), **Spring Cloud Bus (recommended)**, GitHub Webhook (automatic)
- Precedence Order : Command line > JVM > Env vars > Profile > Base
- Encryption for secrets
- High Availability with multiple Config Server instances
