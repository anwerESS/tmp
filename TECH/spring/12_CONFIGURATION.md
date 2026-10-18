## [**..**](./00_index.md)

## Configuration Management & Spring Cloud Config - Points Clés

### **Concept général**

La **Configuration Management** dans Spring Boot permet de gérer les configurations de manière centralisée et de les modifier sans redéployer l'application. **Spring Cloud Config Server** fournit une solution pour externaliser et centraliser les configurations de plusieurs microservices.

**Problème sans Config Server :**
- Configurations dispersées dans chaque microservice
- Nécessite un redéploiement pour changer une configuration
- Difficile de gérer différents environnements (dev, qa, prod)

**Solution avec Config Server :**
- Configurations centralisées dans un seul endroit
- Changement dynamique sans redéploiement
- Gestion facile des profiles (dev, qa, prod)

---

### **Spring Boot Profiles**

Les **profiles** permettent de définir différentes configurations selon l'environnement.

**Activation d'un profile :**
```yaml
# application.yml
spring:
  profiles:
    active: prod  # Peut être : dev, qa, prod
```

**Fichiers de configuration par profile :**
```
application.yml          # Configuration commune
application-dev.yml      # Configuration dev
application-qa.yml       # Configuration qa  
application-prod.yml     # Configuration prod
```

---

### **3 approches pour lire les configurations**

**1. @Value (pour des propriétés simples)**
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

**2. Environment Interface (accès programmatique)**
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

**3. @ConfigurationProperties (pour des configurations structurées - BEST PRACTICE)**
```java
@Component
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfigProperties {
    
    private String message;
    private String buildVersion;
    private ContactDetails contactDetails;
    private List<String> onCallSupport;
    
    // Getters et Setters
    
    public static class ContactDetails {
        private String name;
        private String email;
        // Getters et Setters
    }
}

// Usage dans le controller
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

### **Precedence Order (ordre de priorité des configurations)**

Du plus prioritaire au moins prioritaire :

1. **Command line arguments** : `java -jar app.jar --server.port=9090`
2. **JVM system properties** : `java -Dserver.port=9090 -jar app.jar`
3. **OS environment variables** : `export SERVER_PORT=9090`
4. **application-{profile}.yml** (profil actif)
5. **application.yml** (fichier principal)
6. **@PropertySource**
7. **Default properties**

**Exemple :**
```bash
# Command line (plus haute priorité)
java -jar app.jar --server.port=8081

# JVM system properties
java -Dserver.port=8082 -jar app.jar

# Environment variable
export SERVER_PORT=8083
java -jar app.jar

# application.yml (plus basse priorité)
server:
  port: 8080
```

---

### **Spring Cloud Config Server - Architecture**

**Config Server** : Serveur centralisé qui stocke les configurations
**Config Clients** : Microservices qui récupèrent leurs configurations depuis le Config Server

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

**1. Dépendances (Config Server) :**
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

**2. Activer Config Server :**
```java
@SpringBootApplication
@EnableConfigServer  // Active le Config Server
public class ConfigServerApplication {
    public static void main(String[] args) {
        SpringApplication.run(ConfigServerApplication.class, args);
    }
}
```

**3. Configuration (application.yml) :**
```yaml
server:
  port: 8071  # Port du Config Server

spring:
  profiles:
    active: native  # Profile "native" pour charger depuis classpath
  cloud:
    config:
      server:
        native:
          search-locations: classpath:/config  # Dossier contenant les configs
```

**4. Structure des fichiers :**
```
src/main/resources/
└── config/
    ├── accounts.yml         # Config commune accounts
    ├── accounts-dev.yml     # Config dev accounts
    ├── accounts-qa.yml      # Config qa accounts
    ├── accounts-prod.yml    # Config prod accounts
    ├── loans.yml
    ├── loans-dev.yml
    ├── loans-qa.yml
    └── loans-prod.yml
```

**5. Config Client (microservice) :**
```yaml
# application.yml du microservice
spring:
  application:
    name: accounts  # Nom du service
  config:
    import: "optional:configserver:http://localhost:8071"  # URL du Config Server
  profiles:
    active: prod  # Profile actif
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
          search-locations: "file:///home/anweress/spring-config/config"  # Chemin absolu
```

**Avantage :** Facile de modifier les fichiers directement sur le filesystem sans rebuild.

---

### **Configuration 3 : Config Server from Git Repository (BEST PRACTICE)**

**Configuration (application.yml) :**
```yaml
server:
  port: 8071

spring:
  profiles:
    active: git  # Profile "git"
  cloud:
    config:
      server:
        git:
          uri: "https://github.com/anwerESS/udemy-microservices-config.git"
          default-label: main  # Branche par défaut
          timeout: 5
          clone-on-start: true  # Clone au démarrage
          force-pull: true      # Force le pull à chaque demande
```

**Avantages :**
- ✅ **Version control** des configurations
- ✅ **Collaboration** en équipe
- ✅ **Historique** des changements
- ✅ **Rollback** facile
- ✅ **Centralisation** dans le cloud

---

### **Accès aux configurations depuis le Config Server**

**Format URL :**
```
http://localhost:8071/{application}/{profile}
```

**Exemples :**
```bash
# Configuration prod pour accounts
curl http://localhost:8071/accounts/prod

# Configuration qa pour loans
curl http://localhost:8071/loans/qa

# Configuration dev pour cards
curl http://localhost:8071/cards/dev
```

**Réponse JSON :**
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

**Note :** Les configurations sont **mergées** : profile-specific (prod) override common configs.

---

### **Refresh Configurations at Runtime (3 approches)**

**Problème :** Par défaut, les configurations sont chargées au démarrage. Si on change une config, il faut redémarrer le microservice.

---

#### **Approche 1 : Refresh Actuator (manuel)**

**1. Ajouter @RefreshScope sur les beans :**
```java
@RestController
@RefreshScope  // Permet le refresh dynamique
public class AccountController {
    
    @Autowired
    private AccountsConfigProperties configProperties;
    
    @GetMapping("/contact-info")
    public AccountsConfigProperties getContactInfo() {
        return configProperties;
    }
}
```

**2. Exposer le endpoint refresh :**
```yaml
management:
  endpoints:
    web:
      exposure:
        include: refresh  # Expose /actuator/refresh
```

**3. Appeler le endpoint après changement de config :**
```bash
# Changer la config dans Git
# Puis appeler refresh pour chaque microservice
curl -X POST http://localhost:8080/actuator/refresh
curl -X POST http://localhost:8081/actuator/refresh
curl -X POST http://localhost:8082/actuator/refresh
```

**Réponse :**
```json
[
  "config.client.version",
  "accounts.message"
]
```

**Inconvénient :** Doit appeler refresh pour **chaque microservice** manuellement.

---

#### **Approche 2 : Spring Cloud Bus + RabbitMQ (automatique)**

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
 Auto Refresh (tous en même temps)
```

**1. Dépendance (tous les microservices) :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-bus-amqp</artifactId>
</dependency>
```

**2. Configuration RabbitMQ (tous les microservices) :**
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
        include: busrefresh  # Expose /actuator/busrefresh
```

**3. Démarrer RabbitMQ :**
```bash
docker run -it --rm --name rabbitmq \
  -p 5672:5672 \
  -p 15672:15672 \
  rabbitmq:4.0-management

# UI RabbitMQ : http://localhost:15672 (guest/guest)
```

**4. Refresh tous les microservices en une fois :**
```bash
# Changer la config dans Git
# Appeler busrefresh sur N'IMPORTE QUEL microservice
curl -X POST http://localhost:8080/actuator/busrefresh

# Tous les microservices sont automatiquement refreshés via RabbitMQ
```

**Avantages :**
- ✅ **Un seul appel** refresh tous les microservices
- ✅ **Automatique** via message bus
- ✅ **Scalable** pour des dizaines de microservices

---

#### **Approche 3 : Spring Cloud Bus + GitHub Webhook (100% automatique)**

**1. Ajouter Spring Cloud Config Monitor (Config Server) :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-config-monitor</artifactId>
</dependency>
```

**Note :** Cette dépendance expose automatiquement le endpoint `/monitor`.

**2. Configuration identique à l'approche 2 (RabbitMQ)**

**3. Configurer GitHub Webhook :**
- Aller dans le repository GitHub → Settings → Webhooks
- Add webhook :
  - **Payload URL** : `http://<your-host>:8071/monitor`
  - **Content type** : `application/json`
  - **Events** : Push events

**4. Flow automatique :**
```
Developer push config → GitHub → Webhook → Config Server /monitor
                                              ↓
                                           RabbitMQ
                                              ↓
                                    Tous les microservices
                                    refreshés automatiquement
```

**Avantages :**
- ✅ **100% automatique** : aucune action manuelle
- ✅ **Temps réel** : refresh dès le push Git
- ✅ **Production-ready**

---

### **Encryption des configurations sensibles**

Pour sécuriser les mots de passe, secrets, etc.

**1. Générer une clé de chiffrement :**
```yaml
# application.yml (Config Server)
encrypt:
  key: "MySecretEncryptionKey123"  # Clé symétrique
```

**2. Chiffrer une valeur :**
```bash
curl http://localhost:8071/encrypt -d "mySecretPassword"
# Résultat : AQBhZGF...encrypted-value...
```

**3. Utiliser dans les fichiers de config :**
```yaml
# accounts-prod.yml
database:
  password: '{cipher}AQBhZGF...encrypted-value...'
```

**4. Déchiffrement automatique par les clients**

---

### **Points clés**

**Configuration Management :**
- **3 approches de lecture** : @Value, Environment, @ConfigurationProperties (best practice)
- **Precedence Order** : Command line > JVM props > Env vars > application.yml
- **Profiles** : dev, qa, prod pour différents environnements

**Spring Cloud Config Server :**
- **Centralise** les configurations de tous les microservices
- **3 sources** : Classpath, Filesystem, Git (best practice)
- **@EnableConfigServer** pour activer le Config Server
- **spring.config.import** pour connecter les clients

**Refresh Configurations :**
1. **Manual** : /actuator/refresh (un par un)
2. **Spring Cloud Bus** : /actuator/busrefresh (tous en une fois via RabbitMQ)
3. **GitHub Webhook** : 100% automatique avec /monitor endpoint

**Avantages :**
- ✅ Changements sans redéploiement
- ✅ Gestion centralisée
- ✅ Version control (Git)
- ✅ Environnements multiples (profiles)
- ✅ Sécurité (encryption)

---

## Questions/Réponses - Configuration Management & Spring Cloud Config

### **1. Qu'est-ce que la Configuration Management dans Spring Boot et pourquoi est-elle importante ?**

**Réponse :**
La **Configuration Management** est le processus de gestion et d'organisation des configurations d'une application de manière centralisée et flexible.

**Importance :**
- **Séparation** : Sépare la configuration du code
- **Environnements multiples** : Différentes configs pour dev, qa, prod
- **Changements sans redéploiement** : Modifier les configs à la volée
- **Centralisation** : Gestion centralisée pour les microservices
- **Sécurité** : Encryption des données sensibles
- **Version control** : Historique des changements

**Sans Config Management :**
- Configurations hardcodées dans le code
- Redéploiement nécessaire pour chaque changement
- Difficile de gérer plusieurs environnements
- Risque d'erreurs (mauvaise config en prod)

**Avec Config Management :**
- Configurations externalisées
- Changements dynamiques
- Profiles pour chaque environnement
- Audit trail avec Git

---

### **2. Quelles sont les différentes façons de lire les configurations dans Spring Boot ?**

**Réponse :**

**1. @Value (pour propriétés simples)**
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

**Avantages :** Simple, direct
**Inconvénients :** Verbeux pour plusieurs propriétés, pas de validation

**2. Environment Interface (accès programmatique)**
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

**Avantages :** Accès dynamique, flexible
**Inconvénients :** Moins type-safe, plus verbeux

**3. @ConfigurationProperties (BEST PRACTICE)**
```java
@Component
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfigProperties {
    
    private String message;
    private String buildVersion;
    private ContactDetails contactDetails;
    private List<String> onCallSupport;
    
    // Getters et Setters
    
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

**Avantages :**
- ✅ Type-safe
- ✅ Structure hiérarchique (nested objects)
- ✅ Validation (@Validated + @NotNull, @Min, etc.)
- ✅ Auto-completion dans IDE
- ✅ Réutilisable

**Recommendation :** Utiliser **@ConfigurationProperties** pour des configurations structurées complexes.

---

### **3. Expliquez l'ordre de priorité (Precedence Order) des configurations dans Spring Boot**

**Réponse :**
Spring Boot charge les configurations depuis plusieurs sources. En cas de conflit, l'ordre de priorité (du plus haut au plus bas) est :

**1. Command line arguments** (plus haute priorité)
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

**4. application-{profile}.properties/yml** (profile actif)
```yaml
# application-prod.yml
server:
  port: 9093
```

**5. application.properties/yml** (fichier principal)
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

**Exemple pratique :**
```yaml
# application.yml
server:
  port: 8080

# application-prod.yml
server:
  port: 8081

# Command line
java -jar app.jar --server.port=8082

# Résultat : port 8082 (command line gagne)
```

**Note :** La règle est "**last wins**" - la source avec la plus haute priorité écrase les autres.

---

### **4. Qu'est-ce que Spring Profiles et comment les utiliser ?**

**Réponse :**
Les **Spring Profiles** permettent de définir différentes configurations selon l'environnement (dev, qa, prod).

**Structure des fichiers :**
```
application.yml           # Configuration commune
application-dev.yml       # Surcharge pour dev
application-qa.yml        # Surcharge pour qa
application-prod.yml      # Surcharge pour prod
```

**Activation d'un profile :**

**Option 1 : Dans application.yml**
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

**Exemple de configuration par profile :**
```yaml
# application.yml (commun)
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

**Bean conditionnel par profile :**
```java
@Configuration
public class DataSourceConfig {
    
    @Bean
    @Profile("dev")
    public DataSource devDataSource() {
        // Configuration dev
        return new HikariDataSource();
    }
    
    @Bean
    @Profile("prod")
    public DataSource prodDataSource() {
        // Configuration prod
        return new HikariDataSource();
    }
}
```

---

### **5. Qu'est-ce que Spring Cloud Config Server et pourquoi l'utiliser ?**

**Réponse :**
**Spring Cloud Config Server** est un serveur centralisé qui fournit les configurations pour tous les microservices d'une architecture distribuée.

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

**Pourquoi l'utiliser :**

**Sans Config Server :**
- Chaque microservice a ses propres configs
- Changement = redéploiement de chaque service
- Pas de centralisation
- Difficile de maintenir la cohérence

**Avec Config Server :**
- ✅ **Centralisation** : toutes les configs au même endroit
- ✅ **Version control** : historique avec Git
- ✅ **Environnements multiples** : dev, qa, prod
- ✅ **Refresh dynamique** : sans redéploiement
- ✅ **Sécurité** : encryption des secrets
- ✅ **Audit** : qui a changé quoi et quand
- ✅ **Rollback** : retour arrière facile

**Use case :** Dans une architecture avec 10+ microservices, changer une URL de database nécessiterait 10 redéploiements. Avec Config Server, un seul changement centralisé.

---

### **6. Comment configurer un Spring Cloud Config Server depuis un Git repository ?**

**Réponse :**

**Étape 1 : Créer le Config Server**

**Dépendances (pom.xml) :**
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
@EnableConfigServer  // Active le Config Server
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
    active: git  # Active le profile Git
  cloud:
    config:
      server:
        git:
          uri: "https://github.com/anwerESS/microservices-config.git"
          default-label: main  # Branche par défaut
          timeout: 5
          clone-on-start: true  # Clone au démarrage
          force-pull: true      # Force pull à chaque requête
```

**Étape 2 : Créer le Git repository**

Structure du repository :
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

**Exemple de fichier (accounts-prod.yml) :**
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

**Étape 3 : Configurer les Config Clients (microservices)**

**Dépendance :**
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
    name: accounts  # Nom du service
  profiles:
    active: prod    # Profile actif
  config:
    import: "optional:configserver:http://localhost:8071"  # URL du Config Server
```

**Étape 4 : Tester**
```bash
# Démarrer le Config Server
mvn spring-boot:run

# Accéder aux configurations
curl http://localhost:8071/accounts/prod
curl http://localhost:8071/loans/qa
curl http://localhost:8071/cards/dev
```

---

### **7. Quelle est la différence entre Classpath, Filesystem, et Git comme source de configuration ?**

**Réponse :**

| Source | Configuration | Avantages | Inconvénients | Use Case |
|--------|--------------|-----------|---------------|----------|
| **Classpath** | `search-locations: classpath:/config` | ✅ Simple<br>✅ Pas de dépendance externe | ❌ Configs dans le JAR<br>❌ Pas de version control<br>❌ Changement = rebuild | Dev/Testing |
| **Filesystem** | `search-locations: file:///path/to/config` | ✅ Facile à modifier<br>✅ Pas de rebuild | ❌ Pas de version control<br>❌ Dépend du filesystem local<br>❌ Pas scalable | Dev local |
| **Git** | `uri: https://github.com/...` | ✅ **Version control**<br>✅ **Collaboration**<br>✅ **Historique**<br>✅ **Rollback**<br>✅ **Centralisation cloud** | ❌ Dépendance réseau<br>❌ Plus complexe | **Production (RECOMMENDED)** |

**Configuration Classpath :**
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

**Configuration Filesystem :**
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

**Configuration Git :**
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

**Recommendation :** Utiliser **Git** pour production car il offre version control, collaboration, et historique.

---

### **8. Comment fonctionne le refresh des configurations à la volée (sans redémarrage) ?**

**Réponse :**
Par défaut, Spring charge les configurations au démarrage. Pour refresher sans redémarrer, on utilise **@RefreshScope** + **Actuator**.

**Étape 1 : Ajouter @RefreshScope**
```java
@RestController
@RefreshScope  // Permet le refresh dynamique
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
@RefreshScope  // Important aussi sur le bean de config
public class AccountsConfigProperties {
    private String message;
    private ContactDetails contactDetails;
    // ...
}
```

**Étape 2 : Exposer le endpoint refresh**
```yaml
management:
  endpoints:
    web:
      exposure:
        include: refresh
```

**Étape 3 : Workflow de refresh**
```bash
# 1. Changer la config dans Git
git commit -m "Update accounts message"
git push

# 2. Appeler le endpoint refresh
curl -X POST http://localhost:8080/actuator/refresh

# 3. La nouvelle config est chargée sans redémarrage
```

**Réponse du endpoint :**
```json
[
  "config.client.version",
  "accounts.message",
  "accounts.contactDetails.name"
]
```

**Limitation :** Il faut appeler `/actuator/refresh` sur **chaque instance** de chaque microservice.

**Solution :** Utiliser **Spring Cloud Bus** pour refresh automatique.

---

### **9. Qu'est-ce que Spring Cloud Bus et comment l'utiliser pour le refresh automatique ?**

**Réponse :**
**Spring Cloud Bus** relie tous les microservices via un **message broker** (RabbitMQ ou Kafka) pour propager les events de refresh automatiquement.

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
    Tous refreshés automatiquement
```

**Configuration :**

**1. Dépendance (tous les microservices) :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-bus-amqp</artifactId>
</dependency>
```

**2. Configuration RabbitMQ (tous les microservices) :**
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
        include: busrefresh  # Expose /actuator/busrefresh
```

**3. Démarrer RabbitMQ :**
```bash
docker run -it --rm --name rabbitmq \
  -p 5672:5672 \
  -p 15672:15672 \
  rabbitmq:4.0-management
```

**4. Refresh tous les services en une fois :**
```bash
# Changer config dans Git
git commit -m "Update config"
git push

# Appeler busrefresh sur N'IMPORTE QUEL microservice
curl -X POST http://localhost:8080/actuator/busrefresh

# Tous les microservices connectés à RabbitMQ sont refreshés automatiquement
```

**Avantages :**
- ✅ **Un seul appel** refresh tous les services
- ✅ **Scalable** : fonctionne avec des centaines d'instances
- ✅ **Automatique** via message bus

---

### **10. Comment automatiser complètement le refresh avec GitHub Webhook ?**

**Réponse :**
Avec **Spring Cloud Config Monitor** + **GitHub Webhook**, le refresh se fait **automatiquement** dès qu'on push dans Git.

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
Tous les microservices refreshés automatiquement
```

**Configuration :**

**1. Ajouter Spring Cloud Config Monitor (Config Server) :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-config-monitor</artifactId>
</dependency>
```

**Note :** Cette dépendance expose automatiquement le endpoint `/monitor`.

**2. Configuration RabbitMQ (identique à avant)**

**3. Configurer GitHub Webhook :**
- Aller dans GitHub repository → Settings → Webhooks
- Add webhook :
  - **Payload URL** : `http://your-server:8071/monitor`
  - **Content type** : `application/json`
  - **Events** : Just the push event
  - **Active** : ✅

**4. Flow automatique :**
```
1. Developer change config et push Git
2. GitHub envoie webhook à /monitor
3. Config Server reçoit le webhook
4. Config Server publie event dans RabbitMQ
5. Tous les microservices reçoivent l'event
6. Tous les microservices se refreshent automatiquement
```

**Avantages :**
- ✅ **100% automatique** : aucune action manuelle
- ✅ **Temps réel** : refresh dès le push
- ✅ **Production-ready**
- ✅ **Audit trail** : Git history + webhook logs

---

### **11. Comment sécuriser les configurations sensibles (passwords, secrets) ?**

**Réponse :**
Utiliser **Encryption** dans Spring Cloud Config Server.

**Option 1 : Symmetric encryption (clé partagée)**

**1. Configuration (Config Server) :**
```yaml
encrypt:
  key: "MySecretEncryptionKey123"  # Clé de chiffrement symétrique
```

**2. Chiffrer une valeur :**
```bash
curl http://localhost:8071/encrypt -d "mySecretPassword"
# Résultat : AQBhZGFzZGFz...encrypted-value...zxcvbnm
```

**3. Utiliser dans les fichiers de config :**
```yaml
# accounts-prod.yml
spring:
  datasource:
    password: '{cipher}AQBhZGFzZGFz...encrypted-value...zxcvbnm'
```

**4. Déchiffrement automatique :**
Les clients reçoivent la valeur **déchiffrée** automatiquement.

**Option 2 : Asymmetric encryption (paire de clés publique/privée)**

**1. Générer un keystore :**
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

**3. Chiffrer avec la clé publique :**
```bash
curl http://localhost:8071/encrypt -d "mySecretPassword"
```

**Avantages asymétrique :**
- ✅ Clé publique pour chiffrer (peut être partagée)
- ✅ Clé privée pour déchiffrer (gardée secrète)
- ✅ Plus sécurisé

---

### **12. Comment tester qu'un microservice récupère bien sa configuration depuis le Config Server ?**

**Réponse :**

**Test 1 : Vérifier la connexion au Config Server**
```bash
# Démarrer le Config Server
cd configserver
mvn spring-boot:run

# Vérifier que le Config Server retourne les configs
curl http://localhost:8071/accounts/prod

# Devrait retourner JSON avec les configurations
```

**Test 2 : Créer un endpoint de test dans le microservice**
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

**Test 3 : Démarrer le microservice et appeler l'endpoint**
```bash
# Démarrer le microservice
cd accounts
mvn spring-boot:run

# Tester l'endpoint
curl http://localhost:8080/config-test
```

**Test 4 : Tester le refresh dynamique**
```bash
# 1. Changer une config dans Git (ex: message)
# 2. Push Git
git commit -m "Update message"
git push

# 3. Refresh le microservice
curl -X POST http://localhost:8080/actuator/refresh

# 4. Vérifier que la config a changé
curl http://localhost:8080/config-test
```

**Test 5 : Tests automatisés**
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
        // Vérifier que la config du bon profile est chargée
        assertTrue(config.getMessage().contains("prod"));
    }
}
```

---

### **13. Que se passe-t-il si le Config Server est down ?**

**Réponse :**
Si le Config Server est indisponible au démarrage d'un microservice, plusieurs stratégies :

**Option 1 : Fail Fast (défaut)**
```yaml
spring:
  cloud:
    config:
      fail-fast: true  # Échoue immédiatement si Config Server down
```
→ Le microservice **ne démarre pas** si Config Server est down.

**Option 2 : Graceful Fallback**
```yaml
spring:
  config:
    import: "optional:configserver:http://localhost:8071"  # "optional" keyword
  cloud:
    config:
      fail-fast: false  # Continue même si Config Server down
```
→ Le microservice démarre avec les **configs locales** (application.yml).

**Option 3 : Retry avec backoff**
```yaml
spring:
  cloud:
    config:
      fail-fast: true
      retry:
        initial-interval: 1000  # 1 seconde
        max-attempts: 6         # 6 tentatives
        max-interval: 2000      # 2 secondes max
        multiplier: 1.1         # Multiplicateur exponentiel
```
→ Réessaie de se connecter plusieurs fois avant d'échouer.

**Best Practice Production :**
- ✅ Déployer **plusieurs instances** du Config Server (HA)
- ✅ Utiliser un **load balancer** devant le Config Server
- ✅ Activer les **retries** avec backoff
- ✅ Avoir des **configs locales de fallback**

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

### **14. Comment gérer différentes configurations pour différents environnements (dev, qa, prod) ?**

**Réponse :**

**Structure dans Git :**
```
config-repo/
├── accounts.yml           # Config commune (tous les envs)
├── accounts-dev.yml       # Surcharge pour dev
├── accounts-qa.yml        # Surcharge pour qa
├── accounts-prod.yml      # Surcharge pour prod
├── loans.yml
├── loans-dev.yml
├── loans-qa.yml
└── loans-prod.yml
```

**Exemple de configs :**

**accounts.yml (commun) :**
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
    root: DEBUG  # Logs détaillés en dev
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
    root: WARN  # Logs minimaux en prod
```

**Activation du profile dans le microservice :**

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

**Merge des configurations :**
Le Config Server **merge** automatiquement :
1. accounts.yml (base)
2. accounts-{profile}.yml (surcharge)

```
accounts.yml
    ↓
accounts-prod.yml (override)
    ↓
Résultat final
```

---

### **15. Quelles sont les best practices pour Spring Cloud Config ?**

**Réponse :**

**1. Utiliser Git comme source (pas classpath/filesystem)**
```yaml
spring:
  cloud:
    config:
      server:
        git:
          uri: "https://github.com/org/config-repo.git"
```
✅ Version control, collaboration, historique

**2. Séparer les configs par service ET par environnement**
```
accounts.yml
accounts-dev.yml
accounts-qa.yml
accounts-prod.yml
```

**3. Utiliser @ConfigurationProperties (pas @Value)**
```java
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfig {
    private String message;
    private ContactDetails contactDetails;
}
```
✅ Type-safe, validation, structure

**4. Activer l'encryption pour les secrets**
```yaml
encrypt:
  key: "MySecretKey"

# Dans config
database:
  password: '{cipher}encrypted-value'
```

**5. Implémenter le refresh automatique avec Bus + Webhook**
```yaml
management:
  endpoints:
    web:
      exposure:
        include: busrefresh
```

**6. Utiliser @RefreshScope sur les beans qui changent**
```java
@Component
@RefreshScope
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfig {
    // Config qui peut changer à runtime
}
```

**7. Déployer le Config Server en High Availability (HA)**
```yaml
spring:
  config:
    import:
      - "configserver:http://config-server-1:8071"
      - "configserver:http://config-server-2:8071"
```

**8. Utiliser les retry mechanisms**
```yaml
spring:
  cloud:
    config:
      retry:
        max-attempts: 6
        initial-interval: 1000
```

**9. Sécuriser le Config Server**
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

**10. Ne jamais committer de secrets en clair dans Git**
```yaml
# ❌ Mauvais
database:
  password: "mySecretPassword123"

# ✅ Bon
database:
  password: '{cipher}AQBhZGFzZGFz...encrypted...'
```

**11. Documenter les configurations**
```yaml
# accounts.yml
# Configuration pour le microservice Accounts
# Dernière modification : 2025-10-30
# Owner : Team Banking

accounts:
  message: "Welcome message displayed to users"
  contactDetails:
    name: "Contact person name"
    email: "Contact email"
```

**12. Tester les configurations**
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

**13. Utiliser des configs par défaut sensées**
```yaml
# accounts.yml (base avec des valeurs par défaut)
accounts:
  timeout: 5000
  retry-attempts: 3
  
# accounts-prod.yml (surcharge seulement ce qui change)
accounts:
  timeout: 10000  # Plus de timeout en prod
```

**14. Logger les changements de config**
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

**15. Avoir un plan de rollback**
```bash
# Git permet de rollback facilement
git revert <commit-hash>
git push

# Puis refresh les services
curl -X POST http://localhost:8071/actuator/busrefresh
```

---

### **16. Comment valider les configurations avec @ConfigurationProperties ?**

**Réponse :**
Utiliser **@Validated** + **Bean Validation** pour valider automatiquement les configurations au chargement.

```java
@Component
@ConfigurationProperties(prefix = "accounts")
@Validated  // Active la validation
public class AccountsConfigProperties {
    
    @NotBlank(message = "Message cannot be blank")
    private String message;
    
    @NotBlank(message = "Build version is required")
    @Pattern(regexp = "\\d+\\.\\d+", message = "Version format should be X.Y")
    private String buildVersion;
    
    @Valid  // Valide les nested objects
    @NotNull(message = "Contact details are required")
    private ContactDetails contactDetails;
    
    @NotEmpty(message = "At least one support number is required")
    @Size(min = 1, max = 5, message = "Support numbers should be between 1 and 5")
    private List<String> onCallSupport;
    
    @Min(value = 1000, message = "Timeout must be at least 1000ms")
    @Max(value = 60000, message = "Timeout cannot exceed 60000ms")
    private Integer timeout = 5000;
    
    // Getters et Setters
    
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

**Si la validation échoue au démarrage :**
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

**Avantages :**
- ✅ **Fail fast** : erreur au démarrage, pas en production
- ✅ **Documentation** : les contraintes sont dans le code
- ✅ **Type safety** : erreurs de type détectées tôt

---

### **17. Comment gérer les configurations dans un environnement Kubernetes ?**

**Réponse :**

**Option 1 : ConfigMaps (configs non sensibles)**

**1. Créer un ConfigMap :**
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

**2. Monter le ConfigMap dans le pod :**
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

**Option 2 : Secrets (configs sensibles)**

**1. Créer un Secret :**
```bash
kubectl create secret generic accounts-secret \
  --from-literal=db-password='mySecretPassword' \
  --from-literal=api-key='abc123xyz'
```

**2. Utiliser le Secret :**
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

**Option 3 : Config Server dans Kubernetes**

**1. Déployer Config Server :**
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

**2. Clients se connectent au service :**
```yaml
spring:
  config:
    import: "configserver:http://configserver-service:8071"
```

**Best Practice :** Combiner ConfigMaps (configs non sensibles) + Secrets (credentials) + Config Server (centralisation).

---

### **18. Comment gérer les migrations de configuration (changement de structure) ?**

**Réponse :**

**Scénario :** Renommer une propriété sans casser les anciens clients.

**Ancienne config :**
```yaml
accounts:
  contact-name: "John Doe"
  contact-email: "john@example.com"
```

**Nouvelle config (structure améliorée) :**
```yaml
accounts:
  contactDetails:
    name: "John Doe"
    email: "john@example.com"
```

**Solution : Migration progressive**

**Étape 1 : Supporter les deux formats temporairement**
```java
@Component
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfig {
    
    // Nouvelle structure
    private ContactDetails contactDetails;
    
    // Ancienne structure (deprecated)
    @Deprecated
    private String contactName;
    
    @Deprecated
    private String contactEmail;
    
    // Getters et Setters
    
    // Migration automatique
    @PostConstruct
    public void migrate() {
        if (contactDetails == null && contactName != null) {
            // Migrer l'ancien format vers le nouveau
            contactDetails = new ContactDetails();
            contactDetails.setName(contactName);
            contactDetails.setEmail(contactEmail);
            
            log.warn("Using deprecated config format. Please update to contactDetails structure.");
        }
    }
}
```

**Étape 2 : Phase de transition (3-6 mois)**
- Les deux formats fonctionnent
- Logs de warning pour l'ancien format
- Documentation de migration

**Étape 3 : Supprimer l'ancien format**
```java
@Component
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfig {
    
    // Seulement la nouvelle structure
    private ContactDetails contactDetails;
}
```

**Alternative : Utiliser @DeprecatedConfigurationProperty**
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

### **19. Comment débugger les problèmes de configuration ?**

**Réponse :**

**Problème 1 : Config non chargée**

**Vérifications :**
```bash
# 1. Vérifier que le Config Server fonctionne
curl http://localhost:8071/actuator/health

# 2. Vérifier que le Config Server retourne les configs
curl http://localhost:8071/accounts/prod

# 3. Vérifier les logs du microservice au démarrage
# Doit voir : "Located property source: ..."
```

**Activer les logs de debug :**
```yaml
logging:
  level:
    org.springframework.cloud.config: DEBUG
    org.springframework.web: DEBUG
```

**Problème 2 : Mauvais profile chargé**

**Vérifier le profile actif :**
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

**Problème 3 : Refresh ne fonctionne pas**

**Checklist :**
```java
// 1. @RefreshScope sur le bean ?
@Component
@RefreshScope  // ← Important
@ConfigurationProperties(prefix = "accounts")
public class AccountsConfig { }

// 2. Endpoint exposé ?
// application.yml
management:
  endpoints:
    web:
      exposure:
        include: refresh  // ← Important

// 3. Config changée dans Git et pushed ?
git status
git log

// 4. Appeler refresh
curl -X POST http://localhost:8080/actuator/refresh

// 5. Vérifier les logs
// Doit voir : "Refreshing beans..."
```

**Problème 4 : RabbitMQ connection failed**

```bash
# Vérifier que RabbitMQ tourne
docker ps | grep rabbitmq

# Vérifier les connexions
curl http://localhost:15672/api/connections

# Logs RabbitMQ
docker logs rabbitmq

# Test de connexion
telnet localhost 5672
```

**Endpoint de debugging utile :**
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
        
        // Profiles actifs
        debug.put("activeProfiles", environment.getActiveProfiles());
        
        // Config Server URI
        debug.put("configServerUri", environment.getProperty("spring.config.import"));
        
        // Config actuelle
        debug.put("currentConfig", config);
        
        // Toutes les properties "accounts.*"
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

### **20. Quelles sont les alternatives à Spring Cloud Config ?**

**Réponse :**

| Solution | Description | Avantages | Inconvénients |
|----------|-------------|-----------|---------------|
| **Spring Cloud Config** | Config server centralisé | ✅ Intégration Spring native<br>✅ Git support<br>✅ Refresh dynamique | ❌ Infrastructure supplémentaire<br>❌ Point de défaillance unique |
| **Consul** | Service mesh avec KV store | ✅ Service discovery inclus<br>✅ Health checks<br>✅ Multi-datacenter | ❌ Plus complexe<br>❌ Courbe d'apprentissage |
| **Vault** (HashiCorp) | Gestion de secrets | ✅ Sécurité avancée<br>✅ Rotation automatique<br>✅ Audit logs | ❌ Focus sur secrets (pas configs générales)<br>❌ Plus lourd |
| **etcd** | Distributed KV store | ✅ Hautement disponible<br>✅ Performant<br>✅ Utilisé par Kubernetes | ❌ Pas d'intégration Spring directe<br>❌ API bas niveau |
| **Apache ZooKeeper** | Coordination service | ✅ Mature<br>✅ Reliable | ❌ Complexe à maintenir<br>❌ Technologie ancienne |
| **AWS Systems Manager Parameter Store** | AWS managed | ✅ Fully managed<br>✅ Intégration AWS<br>✅ Encryption | ❌ Lock-in AWS<br>❌ Coût |
| **Kubernetes ConfigMaps + Secrets** | K8s native | ✅ Intégration K8s<br>✅ Simple<br>✅ Pas d'infra supplémentaire | ❌ Pas de version control<br>❌ Pas de refresh dynamique natif |

**Comparaison pour Spring Boot :**

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
- **Spring Cloud Config** : Bon choix pour commencer, bien intégré avec Spring
- **Consul** : Si besoin de service discovery + config
- **Vault** : Si focus sur la sécurité des secrets
- **K8s ConfigMaps** : Si déjà dans Kubernetes et besoins simples

---

### **21. Comment gérer les configurations dans un environnement multi-tenant ?**

**Réponse :**

**Scénario :** Application SaaS avec plusieurs clients (tenants), chacun avec sa propre configuration.

**Approche 1 : Profiles par tenant**

**Structure Git :**
```
config-repo/
├── accounts.yml               # Config commune
├── accounts-tenant-acme.yml   # Config pour ACME Corp
├── accounts-tenant-globex.yml # Config pour Globex
└── accounts-tenant-initech.yml # Config pour Initech
```

**Activation dynamique :**
```java
@Component
public class TenantProfileResolver {
    
    @Autowired
    private Environment environment;
    
    public String resolveTenantProfile(String tenantId) {
        return "tenant-" + tenantId;
    }
}

// Au runtime
String tenantId = extractTenantFromRequest();  // Ex: "acme"
String profile = "tenant-" + tenantId;  // "tenant-acme"
```

**Approche 2 : Config hierarchique dans Git**

**Structure :**
```
config-repo/
├── common/
│   └── accounts.yml         # Config commune
├── tenants/
│   ├── acme/
│   │   └── accounts.yml     # Config ACME
│   ├── globex/
│   │   └── accounts.yml     # Config Globex
│   └── initech/
│       └── accounts.yml     # Config Initech
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

**Approche 3 : Database-backed configs**

**1. Créer une table de configuration :**
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

**Approche 4 : Hybrid (Git + Database)**

```java
@Configuration
public class MultiTenantConfigStrategy {
    
    // Config commune depuis Git (via Config Server)
    @Bean
    @ConfigurationProperties(prefix = "accounts.common")
    public CommonConfig commonConfig() {
        return new CommonConfig();
    }
    
    // Config spécifique tenant depuis Database
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

**Conseil final :** Prépare-toi à expliquer l'architecture complète d'une solution de configuration centralisée avec refresh automatique. Les interviewers aiment les schémas et les exemples concrets ! 💪

**Points clés à retenir :**
- Spring Cloud Config centralise les configurations
- 3 sources : Classpath, Filesystem, **Git (best practice)**
- 3 façons de lire : @Value, Environment, **@ConfigurationProperties (best practice)**
- Refresh : Manual (actuator), **Spring Cloud Bus (recommended)**, GitHub Webhook (automatique)
- Precedence Order : Command line > JVM > Env vars > Profile > Base
- Encryption pour les secrets
- High Availability avec plusieurs instances Config Server