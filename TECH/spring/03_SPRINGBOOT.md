## [**..**](./00_index.md)

# Spring Boot & Spring Cloud

## 🚀 Spring Boot - Objectifs et Philosophie

### Le Problème que Spring Boot Résout

**Avant Spring Boot (Spring classique) :**
```xml
<!-- Configuration XML massive -->
<bean id="dataSource" ...>
<bean id="entityManagerFactory" ...>
<bean id="transactionManager" ...>
<!-- + configuration Tomcat, dispatcher servlet, view resolver, etc. -->
```
- Configuration XML verbeux et complexe
- Configuration manuelle de tous les composants
- Gestion manuelle des dépendances et versions
- Déploiement sur serveur externe (Tomcat, etc.)
- Temps de setup d'un nouveau projet : plusieurs heures/jours

**Avec Spring Boot :**
```java
@SpringBootApplication
public class Application {
    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }
}
```
- Configuration minimale (zéro XML)
- Auto-configuration intelligente
- Gestion automatique des dépendances
- Serveur embarqué (pas de déploiement externe)
- Temps de setup : quelques minutes

### Objectifs de Spring Boot

1. **Simplifier le développement** : Éliminer la configuration boilerplate
2. **Convention over Configuration** : Valeurs par défaut intelligentes
3. **Production-Ready** : Monitoring, health checks, metrics intégrés
4. **Microservices-Friendly** : Applications autonomes et légères
5. **Rapidité** : Du code à la production en minutes

## 🎯 Spring Boot Starter Projects - La Magie des Starters

### Concept des Starters

Les **Starters** sont des dépendances "tout-en-un" qui incluent automatiquement toutes les bibliothèques nécessaires pour un cas d'usage spécifique.

### Exemples de Starters Principaux

#### 1. **spring-boot-starter-web**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-web</artifactId>
</dependency>
```
**Inclut automatiquement :**
- Spring MVC (pour REST APIs et web)
- Tomcat embarqué (serveur)
- Jackson (JSON serialization)
- Validation API
- Logging (Logback)

**Sans Starter, il faudrait ajouter manuellement :**
```xml
<dependency><artifactId>spring-webmvc</artifactId></dependency>
<dependency><artifactId>tomcat-embed-core</artifactId></dependency>
<dependency><artifactId>jackson-databind</artifactId></dependency>
<dependency><artifactId>hibernate-validator</artifactId></dependency>
<!-- + gérer les versions compatibles ! -->
```

#### 2. **spring-boot-starter-data-jpa**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-data-jpa</artifactId>
</dependency>
```
**Inclut :**
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
**Inclut :**
- Spring Security Core
- Authentication et Authorization
- Protection CSRF
- Session Management

#### 4. **spring-boot-starter-test**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-test</artifactId>
    <scope>test</scope>
</dependency>
```
**Inclut :**
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
**Fonctionnalités Production-Ready :**
- Health checks (`/actuator/health`)
- Metrics (`/actuator/metrics`)
- Application info (`/actuator/info`)
- Environment properties (`/actuator/env`)

### Autres Starters Utiles

| Starter | Usage |
|---------|-------|
| `spring-boot-starter-validation` | Bean Validation (JSR-303) |
| `spring-boot-starter-cache` | Caching (Redis, EhCache) |
| `spring-boot-starter-mail` | Envoi d'emails |
| `spring-boot-starter-thymeleaf` | Moteur de templates |
| `spring-boot-starter-websocket` | WebSockets |
| `spring-boot-starter-batch` | Spring Batch (jobs) |
| `spring-boot-starter-amqp` | RabbitMQ messaging |
| `spring-boot-starter-data-mongodb` | MongoDB |
| `spring-boot-starter-data-redis` | Redis |

### Avantages des Starters

✅ **Gestion des versions** : Compatibilité garantie entre dépendances
✅ **Gain de temps** : Une seule dépendance au lieu de 10+
✅ **Pas de conflits** : Versions testées ensemble
✅ **Simplicité** : Moins d'erreurs de configuration
✅ **Maintenance** : Update facile (une seule version Spring Boot)

## ⚙️ Auto-Configuration - La Vraie Magie

### Principe de l'Auto-Configuration

Spring Boot analyse le **classpath** et configure automatiquement les beans nécessaires selon les dépendances présentes.

### Comment ça Fonctionne ?

#### Exemple : DataSource Auto-Configuration

**Si vous ajoutez :**
```xml
<dependency>
    <artifactId>spring-boot-starter-data-jpa</artifactId>
</dependency>
<dependency>
    <artifactId>h2</artifactId>
</dependency>
```

**Spring Boot configure automatiquement :**
```java
// Ces beans sont créés automatiquement !
@Bean
public DataSource dataSource() { ... }

@Bean
public EntityManagerFactory entityManagerFactory() { ... }

@Bean
public PlatformTransactionManager transactionManager() { ... }
```

**Configuration dans application.properties :**
```properties
spring.datasource.url=jdbc:h2:mem:testdb
spring.datasource.username=sa
spring.datasource.password=
spring.jpa.hibernate.ddl-auto=update
```

### Conditions d'Auto-Configuration

Spring Boot utilise des **annotations conditionnelles** :

#### @ConditionalOnClass
```java
@Configuration
@ConditionalOnClass(DataSource.class)
public class DataSourceAutoConfiguration {
    // Configuration activée seulement si DataSource est sur le classpath
}
```

#### @ConditionalOnMissingBean
```java
@Bean
@ConditionalOnMissingBean
public DataSource dataSource() {
    // Créé seulement si l'utilisateur n'a pas défini son propre DataSource
}
```

#### @ConditionalOnProperty
```java
@Configuration
@ConditionalOnProperty(name = "spring.datasource.enabled", havingValue = "true")
public class DataSourceConfig {
    // Activé seulement si la propriété existe
}
```

### Voir l'Auto-Configuration en Action

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

### Personnaliser ou Désactiver l'Auto-Configuration

#### Désactiver une auto-configuration spécifique :
```java
@SpringBootApplication(exclude = {DataSourceAutoConfiguration.class})
public class Application { }
```

#### Override avec votre propre bean :
```java
@Configuration
public class CustomDataSourceConfig {
    @Bean
    public DataSource dataSource() {
        // Votre configuration personnalisée
        // Spring Boot n'appliquera pas l'auto-configuration
        return new HikariDataSource();
    }
}
```

## 🖥️ Embedded Servers - Production Ready

### Le Problème des Serveurs Externes

**Approche traditionnelle :**
1. Développer l'application (WAR)
2. Installer Tomcat/JBoss sur le serveur
3. Configurer le serveur
4. Déployer le WAR
5. Gérer le serveur séparément

**Inconvénients :**
- Complexité de déploiement
- Dépendance à un serveur externe
- Versions différentes entre dev/prod
- Difficile pour microservices

### Spring Boot : Serveurs Embarqués

**Spring Boot embarque le serveur directement dans le JAR !**

```
my-app.jar
├── application classes
├── dependencies (Spring, etc.)
└── embedded Tomcat
```

**Exécution simple :**
```bash
java -jar my-app.jar
# Application démarrée avec Tomcat embarqué sur port 8080
```

### Serveurs Embarqués Supportés

#### 1. **Tomcat** (par défaut avec spring-boot-starter-web)
```xml
<dependency>
    <artifactId>spring-boot-starter-web</artifactId>
    <!-- Tomcat embarqué inclus -->
</dependency>
```

#### 2. **Jetty** (alternative légère)
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

#### 3. **Undertow** (hautes performances)
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

### Configuration du Serveur Embarqué

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

### Avantages des Serveurs Embarqués

✅ **Simplicité** : Une seule commande pour démarrer
✅ **Portabilité** : Même environnement dev/prod
✅ **Microservices** : Chaque service est autonome
✅ **CI/CD** : Déploiement simplifié
✅ **Conteneurisation** : Parfait pour Docker/Kubernetes
✅ **Pas de dépendance externe** : Tout dans le JAR

### Production-Ready Features (Actuator)

**Endpoints de monitoring :**
```properties
management.endpoints.web.exposure.include=health,metrics,info,env
management.endpoint.health.show-details=always
```

**Endpoints disponibles :**
- `/actuator/health` - État de l'application
- `/actuator/metrics` - Métriques (CPU, mémoire, requêtes)
- `/actuator/info` - Informations sur l'application
- `/actuator/env` - Variables d'environnement
- `/actuator/loggers` - Configuration des logs
- `/actuator/httptrace` - Dernières requêtes HTTP

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

### Vue d'Ensemble

```
Spring Framework (Conteneur IoC + Core)
    ↓
Spring MVC (Module web de Spring)
    ↓
Spring Boot (Simplification de Spring + Spring MVC)
```

### Spring Framework (Core)

**Définition :** Framework Java pour l'Inversion of Control et Dependency Injection

**Composants :**
- Spring Core (IoC Container)
- Spring AOP
- Spring JDBC
- Spring Transaction
- Spring Test

**Exemple :**
```java
@Component
public class UserService {
    @Autowired
    private UserRepository repository;
}
```

**Caractéristiques :**
- Gère le cycle de vie des beans
- Dependency Injection
- Pas spécifique au web
- Configuration XML ou annotations
- Base de tous les modules Spring

### Spring MVC (Module Web)

**Définition :** Module de Spring Framework pour créer des applications web et REST APIs

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

**Exemple :**
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

**Composants clés :**
- `@Controller` / `@RestController`
- `@RequestMapping` / `@GetMapping` / `@PostMapping`
- `@RequestBody` / `@ResponseBody`
- `@PathVariable` / `@RequestParam`
- View Resolvers (pour JSP, Thymeleaf)

**Configuration traditionnelle (sans Boot) :**
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

### Spring Boot (Simplification Totale)

**Définition :** Framework basé sur Spring et Spring MVC qui simplifie drastiquement la configuration et le déploiement

**Exemple complet d'application :**
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

**Fonctionnalités ajoutées :**
- Auto-configuration
- Starter dependencies
- Serveur embarqué
- Production-ready features (Actuator)
- Configuration externalisée (application.properties)
- Profiles
- DevTools (hot reload)

### @SpringBootApplication - La Combinaison Magique

```java
@SpringBootApplication
// Équivaut à :
@SpringBootConfiguration  // = @Configuration
@EnableAutoConfiguration  // Active l'auto-configuration
@ComponentScan           // Scan des @Component
public class Application { }
```

**Décomposition :**
1. **@SpringBootConfiguration** : Marque la classe comme configuration Spring
2. **@EnableAutoConfiguration** : Active la magie de l'auto-configuration
3. **@ComponentScan** : Scanne le package courant et sous-packages

### Tableau Comparatif

| Aspect | Spring | Spring MVC | Spring Boot |
|--------|--------|------------|-------------|
| **Objectif** | IoC & DI | Applications Web | Tout simplifier |
| **Configuration** | XML/Annotations | XML verbose | Minimale (properties) |
| **Serveur** | Externe requis | Externe requis | Embarqué |
| **Setup initial** | Complexe | Très complexe | Rapide (minutes) |
| **Dépendances** | Manuelles | Manuelles + versions | Starters auto |
| **Production** | Configuration manuelle | Configuration manuelle | Actuator intégré |
| **Déploiement** | WAR sur serveur | WAR sur serveur | JAR autonome |
| **Courbe apprentissage** | Moyenne | Élevée | Faible |

### Exemple Concret : REST API

#### Avec Spring MVC (sans Boot)
```xml
<!-- pom.xml - 20+ dépendances à gérer -->
<dependency><artifactId>spring-webmvc</artifactId></dependency>
<dependency><artifactId>jackson-databind</artifactId></dependency>
<dependency><artifactId>tomcat-servlet-api</artifactId></dependency>
<!-- + configuration XML pour DispatcherServlet, ViewResolver, etc. -->
```

#### Avec Spring Boot
```xml
<!-- pom.xml - 1 dépendance -->
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

**Résultat :** Application REST complète en 15 lignes de code !

### Quand Utiliser Quoi ?

**Spring Framework seul :**
- Applications non-web (batch, console)
- Projets nécessitant un contrôle total
- Cas d'usage très spécifiques

**Spring MVC :**
- Maintenance de code legacy
- Projets existants sans Boot
- Rarement recommandé pour nouveaux projets

**Spring Boot :** ✅ **Recommandé pour tous les nouveaux projets**
- REST APIs
- Microservices
- Applications web
- Projets cloud-native
- Prototypes rapides

## ☁️ Spring Cloud - Microservices Architecture

### Objectif de Spring Cloud

**Spring Cloud** est un ensemble de frameworks construits sur Spring Boot pour simplifier le développement de systèmes distribués et d'architectures microservices.

### Problèmes des Microservices Résolus par Spring Cloud

#### 1. **Service Discovery** (Où sont mes services ?)
**Problème :** Comment un service trouve-t-il l'adresse d'un autre service ?

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

**Eureka Client (Service qui s'enregistre) :**
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

#### 2. **Load Balancing** (Répartition de charge)
**Solution : Spring Cloud LoadBalancer**

```java
@RestController
public class OrderController {
    
    @Autowired
    private RestTemplate restTemplate; // Avec LoadBalancer
    
    @GetMapping("/orders/{id}")
    public Order getOrder(@PathVariable Long id) {
        // Appel avec load balancing automatique
        User user = restTemplate.getForObject(
            "http://USER-SERVICE/api/users/123", 
            User.class
        );
        return new Order(id, user);
    }
}
```

#### 3. **API Gateway** (Point d'entrée unique)
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

#### 4. **Configuration Centralisée**
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

**Structure du repo Git :**
```
config-repo/
├── application.properties          # Config commune
├── user-service.properties         # Config user-service
├── user-service-dev.properties     # Config dev
└── user-service-prod.properties    # Config prod
```

**Client récupère sa config :**
```properties
spring.cloud.config.uri=http://localhost:8888
spring.application.name=user-service
spring.profiles.active=dev
```

#### 5. **Circuit Breaker** (Tolérance aux pannes)
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
    
    // Méthode de secours si user-service est down
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

#### 6. **Distributed Tracing** (Traçabilité)
**Solution : Spring Cloud Sleuth + Zipkin**

```xml
<dependency>
    <artifactId>spring-cloud-starter-sleuth</artifactId>
</dependency>
<dependency>
    <artifactId>spring-cloud-sleuth-zipkin</artifactId>
</dependency>
```

**Logs avec trace IDs :**
```
INFO [user-service,a1b2c3d4,e5f6g7h8] UserController: Getting user 123
INFO [order-service,a1b2c3d4,i9j0k1l2] OrderController: Creating order
```

**Visualisation dans Zipkin :**
- Voir le parcours complet d'une requête à travers les services
- Identifier les goulots d'étranglement
- Temps de réponse par service

### Composants Spring Cloud Principaux

| Composant | Fonction |
|-----------|----------|
| **Eureka** | Service Discovery (registry) |
| **Gateway** | API Gateway (routing, filtres) |
| **Config** | Configuration centralisée |
| **LoadBalancer** | Client-side load balancing |
| **Circuit Breaker** | Tolérance aux pannes |
| **Sleuth** | Distributed tracing |
| **OpenFeign** | Client REST déclaratif |
| **Bus** | Event bus pour refresh config |

### Architecture Microservices Complète

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

### OpenFeign - Client REST Simplifié

**Sans Feign (RestTemplate) :**
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

**Avec Feign (déclaratif) :**
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
        return userClient.getUser(id); // Simple et propre !
    }
}
```

### Spring Cloud vs Spring Boot

| Aspect | Spring Boot | Spring Cloud |
|--------|-------------|--------------|
| **Scope** | Application unique | Système distribué |
| **Architecture** | Monolithe ou microservice | Microservices |
| **Complexité** | Simple | Complexe |
| **Use case** | API standalone | Architecture microservices |
| **Dépendances** | Starters de base | Starters Cloud (Eureka, Gateway) |

**Résumé :**
- **Spring Boot** = Créer UNE application rapidement
- **Spring Cloud** = Gérer PLUSIEURS applications (microservices) ensemble

## 📊 Résumé - Choix Technologique

### Pour une Application Simple
```
Spring Boot + Embedded Tomcat
    ↓
REST API prête en minutes
```

### Pour des Microservices
```
Spring Boot (base)
    +
Spring Cloud (Eureka, Gateway, Config, Circuit Breaker)
    ↓
Architecture microservices complète
```

### Évolution Typique

**Phase 1 - MVP :**
- Spring Boot monolithe
- Base de données unique
- Déploiement simple

**Phase 2 - Croissance :**
- Séparation en microservices
- Spring Cloud (Eureka, Gateway)
- Bases de données par service

**Phase 3 - Scale :**
- Circuit Breakers
- Configuration centralisée
- Monitoring distribué (Sleuth/Zipkin)
- Kubernetes pour orchestration



# Questions & Réponses - Spring Boot & Spring Cloud

## 🚀 Spring Boot - Objectifs et Philosophie

**Q1 : Quel est l'objectif principal de Spring Boot ?**
**R :** Simplifier drastiquement le développement d'applications Spring en éliminant la configuration boilerplate. Spring Boot permet de passer du code à la production en minutes grâce à l'auto-configuration, les starters, et les serveurs embarqués.

**Q2 : Quels problèmes Spring Boot résout-il par rapport à Spring classique ?**
**R :**
- Élimine la configuration XML verbeux
- Gère automatiquement les dépendances et versions compatibles
- Fournit un serveur embarqué (pas besoin de Tomcat externe)
- Auto-configuration intelligente des composants
- Temps de setup réduit de plusieurs heures à quelques minutes

**Q3 : Qu'est-ce que "Convention over Configuration" ?**
**R :** Philosophie où Spring Boot fournit des valeurs par défaut intelligentes pour tout. Au lieu de tout configurer manuellement, on accepte les conventions (port 8080, `/actuator/health`, etc.) et on ne configure que ce qui diffère des défauts.

## 🎯 Spring Boot Starter Projects

**Q4 : Qu'est-ce qu'un Spring Boot Starter ?**
**R :** Un Starter est une dépendance "tout-en-un" qui inclut automatiquement toutes les bibliothèques nécessaires pour un cas d'usage spécifique. Exemple : `spring-boot-starter-web` inclut Spring MVC, Tomcat, Jackson, Validation, et Logging.

**Q5 : Que contient spring-boot-starter-web ?**
**R :**
- Spring MVC (pour REST APIs et web)
- Tomcat embarqué (serveur)
- Jackson (JSON serialization/deserialization)
- Validation API (JSR-303)
- Logging (Logback)

**Q6 : Quelle est la différence entre spring-boot-starter-web et spring-boot-starter-data-jpa ?**
**R :**
- **spring-boot-starter-web** : Pour créer des REST APIs et applications web (MVC, Tomcat, Jackson)
- **spring-boot-starter-data-jpa** : Pour l'accès aux données (JPA, Hibernate, Spring Data, Transactions, JDBC)

**Q7 : Quels sont les avantages des Starters ?**
**R :**
- Gestion automatique des versions compatibles
- Une seule dépendance au lieu de 10+
- Pas de conflits de versions
- Gain de temps et moins d'erreurs
- Mise à jour facile (une seule version Spring Boot)

**Q8 : Citez 5 Starters couramment utilisés**
**R :**
1. `spring-boot-starter-web` - REST APIs et web
2. `spring-boot-starter-data-jpa` - Accès données avec JPA
3. `spring-boot-starter-security` - Sécurité et authentification
4. `spring-boot-starter-test` - Tests (JUnit, Mockito)
5. `spring-boot-starter-actuator` - Monitoring production

**Q9 : Qu'est-ce que spring-boot-starter-actuator et pourquoi est-il important ?**
**R :** Actuator fournit des endpoints production-ready pour monitorer l'application : health checks (`/actuator/health`), métriques (`/actuator/metrics`), informations environnement, logs. Essentiel pour la production et le DevOps.

## ⚙️ Auto-Configuration

**Q10 : Comment fonctionne l'auto-configuration de Spring Boot ?**
**R :** Spring Boot analyse le classpath au démarrage et configure automatiquement les beans nécessaires selon les dépendances présentes. Si H2 et JPA sont sur le classpath, il configure automatiquement DataSource, EntityManagerFactory, et TransactionManager.

**Q11 : Qu'est-ce que @ConditionalOnClass ?**
**R :** Annotation qui active une configuration seulement si une classe spécifique est présente sur le classpath. Exemple : `@ConditionalOnClass(DataSource.class)` active la config uniquement si DataSource est disponible.

**Q12 : Qu'est-ce que @ConditionalOnMissingBean ?**
**R :** Annotation qui crée un bean seulement si l'utilisateur n'en a pas déjà défini un. Permet de fournir une configuration par défaut tout en laissant l'utilisateur override. Principe "convention with override possibility".

**Q13 : Comment voir quelles auto-configurations ont été appliquées ?**
**R :** Activer le debug mode avec `debug=true` dans `application.properties`. Spring Boot affichera un rapport détaillé des "Positive matches" (configurations appliquées) et "Negative matches" (configurations ignorées).

**Q14 : Comment désactiver une auto-configuration spécifique ?**
**R :**
```java
@SpringBootApplication(exclude = {DataSourceAutoConfiguration.class})
public class Application { }
```
Ou dans application.properties :
```properties
spring.autoconfigure.exclude=org.springframework.boot.autoconfigure.jdbc.DataSourceAutoConfiguration
```

**Q15 : Comment override une auto-configuration avec ma propre configuration ?**
**R :** Définir simplement votre propre bean du même type. Grâce à `@ConditionalOnMissingBean`, Spring Boot détectera votre bean et n'appliquera pas l'auto-configuration par défaut.

**Q16 : Donnez un exemple d'auto-configuration en action**
**R :** Si vous ajoutez `spring-boot-starter-data-jpa` et `h2` dans le pom.xml, Spring Boot configure automatiquement :
- DataSource (connexion H2 en mémoire)
- EntityManagerFactory (JPA)
- TransactionManager
Sans aucune configuration manuelle !

## 🖥️ Embedded Servers

**Q17 : Qu'est-ce qu'un serveur embarqué (Embedded Server) ?**
**R :** Un serveur (Tomcat, Jetty, Undertow) inclus directement dans le JAR de l'application. L'application devient autonome et peut démarrer avec `java -jar app.jar` sans installer de serveur externe.

**Q18 : Quels sont les avantages des serveurs embarqués ?**
**R :**
- Simplicité : une seule commande pour démarrer
- Portabilité : même environnement dev/prod
- Idéal pour microservices (chaque service autonome)
- CI/CD simplifié
- Parfait pour Docker/Kubernetes
- Pas de gestion de serveur externe

**Q19 : Quel est le serveur embarqué par défaut de Spring Boot ?**
**R :** **Tomcat** est le serveur embarqué par défaut avec `spring-boot-starter-web`. Il est inclus automatiquement sans configuration supplémentaire.

**Q20 : Comment changer de serveur embarqué (Tomcat vers Jetty) ?**
**R :**
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

**Q21 : Citez les 3 serveurs embarqués supportés par Spring Boot**
**R :**
1. **Tomcat** (défaut) - Populaire et robuste
2. **Jetty** - Léger et flexible
3. **Undertow** - Hautes performances

**Q22 : Comment configurer le port du serveur embarqué ?**
**R :**
```properties
# application.properties
server.port=8080

# Ou port aléatoire
server.port=0

# Ou via ligne de commande
java -jar app.jar --server.port=9090
```

**Q23 : Comment activer SSL sur le serveur embarqué ?**
**R :**
```properties
server.ssl.enabled=true
server.ssl.key-store=classpath:keystore.p12
server.ssl.key-store-password=mypassword
server.ssl.key-store-type=PKCS12
```

**Q24 : Quelle est la différence entre JAR et WAR avec Spring Boot ?**
**R :**
- **JAR** (recommandé) : Application autonome avec serveur embarqué, exécutable avec `java -jar`
- **WAR** : Pour déploiement sur serveur externe (Tomcat, JBoss), nécessite `SpringBootServletInitializer`

**Q25 : Comment configurer le context path de l'application ?**
**R :**
```properties
server.servlet.context-path=/api
# Les endpoints seront accessibles via /api/users au lieu de /users
```

## 📱 @SpringBootApplication

**Q26 : Que fait l'annotation @SpringBootApplication ?**
**R :** C'est une méta-annotation combinant 3 annotations :
1. `@SpringBootConfiguration` (= `@Configuration`)
2. `@EnableAutoConfiguration` (active l'auto-configuration)
3. `@ComponentScan` (scan des composants)

**Q27 : Peut-on décomposer @SpringBootApplication en ses 3 annotations ?**
**R :** Oui :
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
Mais `@SpringBootApplication` est plus concis et recommandé.

**Q28 : Que fait @EnableAutoConfiguration exactement ?**
**R :** Active la magie de l'auto-configuration de Spring Boot. Analyse le classpath et configure automatiquement les beans nécessaires selon les dépendances présentes (DataSource, EntityManager, etc.).

**Q29 : Que fait @ComponentScan dans @SpringBootApplication ?**
**R :** Scanne automatiquement le package de la classe principale et tous ses sous-packages pour détecter les composants (`@Component`, `@Service`, `@Repository`, `@Controller`) et les enregistrer comme beans.

## 🔄 Spring vs Spring MVC vs Spring Boot

**Q30 : Quelle est la différence entre Spring Framework et Spring Boot ?**
**R :**
- **Spring Framework** : Conteneur IoC et DI, nécessite configuration manuelle extensive
- **Spring Boot** : Basé sur Spring, ajoute auto-configuration, starters, serveur embarqué, et simplifie tout drastiquement

**Q31 : Qu'est-ce que Spring MVC ?**
**R :** Spring MVC est un module de Spring Framework pour créer des applications web et REST APIs. Il fournit l'architecture MVC (Model-View-Controller) avec annotations comme `@Controller`, `@RequestMapping`, `@RestController`.

**Q32 : Spring Boot inclut-il Spring MVC ?**
**R :** Oui, `spring-boot-starter-web` inclut automatiquement Spring MVC + Tomcat embarqué + Jackson. Spring Boot simplifie l'utilisation de Spring MVC en éliminant toute la configuration XML.

**Q33 : Peut-on utiliser Spring MVC sans Spring Boot ?**
**R :** Oui, mais c'est beaucoup plus complexe. Il faut configurer manuellement DispatcherServlet, ViewResolver, gestion des dépendances, serveur externe, etc. Spring Boot rend tout cela automatique.

**Q34 : Comparaison : combien de lignes pour une REST API ?**
**R :**
- **Spring MVC sans Boot** : ~100+ lignes (XML config + Java)
- **Spring Boot** : ~15 lignes de code Java
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

**Q35 : Quand utiliser Spring Framework seul vs Spring Boot ?**
**R :**
- **Spring seul** : Projets legacy, besoin de contrôle total extrême (rare)
- **Spring Boot** : Tous les nouveaux projets (REST APIs, microservices, web apps) - Recommandé ✅

**Q36 : Spring Boot remplace-t-il Spring Framework ?**
**R :** Non, Spring Boot est construit SUR Spring Framework. Il utilise Spring Core, Spring MVC, etc., mais simplifie leur configuration et utilisation. Spring Boot = Spring + Convention + Auto-configuration.

## ☁️ Spring Cloud - Introduction

**Q37 : Qu'est-ce que Spring Cloud ?**
**R :** Ensemble de frameworks construits sur Spring Boot pour simplifier le développement d'architectures microservices distribuées. Résout les problèmes de service discovery, load balancing, configuration centralisée, circuit breakers, etc.

**Q38 : Quelle est la différence entre Spring Boot et Spring Cloud ?**
**R :**
- **Spring Boot** : Pour créer UNE application autonome
- **Spring Cloud** : Pour gérer PLUSIEURS applications (microservices) qui communiquent ensemble

**Q39 : Citez 5 problèmes que Spring Cloud résout**
**R :**
1. Service Discovery (comment trouver les services ?)
2. Load Balancing (répartir la charge)
3. API Gateway (point d'entrée unique)
4. Configuration centralisée
5. Circuit Breaker (tolérance aux pannes)

**Q40 : Qu'est-ce que Netflix OSS dans Spring Cloud ?**
**R :** Netflix a open-sourcé ses outils microservices (Eureka, Ribbon, Hystrix, Zuul). Spring Cloud les a intégrés et continue de les maintenir (certains sont maintenant remplacés par des alternatives modernes comme Gateway au lieu de Zuul).

## 🔍 Service Discovery

**Q41 : Qu'est-ce que le Service Discovery et pourquoi est-il nécessaire ?**
**R :** Dans une architecture microservices, les services ont des adresses dynamiques (scaling, redéploiements). Service Discovery est un registry central (comme Eureka) où les services s'enregistrent. Les autres services interrogent ce registry pour trouver les adresses disponibles.

**Q42 : Qu'est-ce que Netflix Eureka ?**
**R :** Eureka est un service de registry (Service Discovery). Les microservices s'enregistrent auprès d'Eureka au démarrage, et les autres services interrogent Eureka pour trouver les instances disponibles.

**Q43 : Comment créer un Eureka Server ?**
**R :**
```java
@SpringBootApplication
@EnableEurekaServer
public class EurekaServerApplication {
    public static void main(String[] args) {
        SpringApplication.run(EurekaServerApplication.class, args);
    }
}
```
Avec dépendance : `spring-cloud-starter-netflix-eureka-server`

**Q44 : Comment enregistrer un service auprès d'Eureka ?**
**R :**
```java
@SpringBootApplication
@EnableEurekaClient // Ou @EnableDiscoveryClient
public class UserServiceApplication { }
```
```properties
eureka.client.service-url.defaultZone=http://localhost:8761/eureka
spring.application.name=user-service
```

**Q45 : Comment appeler un service enregistré dans Eureka ?**
**R :**
```java
@Autowired
private RestTemplate restTemplate;

// Utilise le nom du service au lieu de l'URL
User user = restTemplate.getForObject(
    "http://USER-SERVICE/api/users/123",
    User.class
);
// Spring Cloud résout automatiquement l'adresse via Eureka
```

## 🌐 API Gateway

**Q46 : Qu'est-ce qu'un API Gateway et pourquoi en avoir besoin ?**
**R :** Un API Gateway est un point d'entrée unique pour tous les microservices. Il gère le routing, l'authentification, le rate limiting, et évite d'exposer directement tous les services. Les clients communiquent uniquement avec le Gateway.

**Q47 : Qu'est-ce que Spring Cloud Gateway ?**
**R :** Framework de API Gateway de Spring Cloud. Il route les requêtes vers les microservices appropriés, applique des filtres (auth, logging, transformation), et supporte le load balancing.

**Q48 : Comment configurer des routes dans Spring Cloud Gateway ?**
**R :**
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

**Q49 : Quelle est la différence entre Zuul et Spring Cloud Gateway ?**
**R :**
- **Zuul** (Netflix) : Ancien gateway, synchrone, moins performant
- **Spring Cloud Gateway** : Moderne, asynchrone (WebFlux), meilleur performances, recommandé pour nouveaux projets

**Q50 : Quels sont les avantages d'un API Gateway ?**
**R :**
- Point d'entrée unique (simplifie le client)
- Authentification centralisée
- Rate limiting et throttling
- Logging et monitoring centralisés
- Routing intelligent
- Protection des services internes

## ⚙️ Configuration Centralisée

**Q51 : Pourquoi centraliser la configuration des microservices ?**
**R :** Avec 10+ microservices, gérer les configs dans chaque service devient ingérable. Spring Cloud Config centralise toutes les configurations dans un seul endroit (Git repository), permettant de modifier des configs sans redéployer les services.

**Q52 : Comment fonctionne Spring Cloud Config ?**
**R :** 
1. **Config Server** : Serveur Spring Boot qui expose les configurations depuis un Git repository
2. **Config Client** : Les microservices se connectent au Config Server au démarrage pour récupérer leur configuration

**Q53 : Comment créer un Config Server ?**
**R :**
```java
@SpringBootApplication
@EnableConfigServer
public class ConfigServerApplication { }
```
```properties
spring.cloud.config.server.git.uri=https://github.com/myorg/config-repo
```
Dépendance : `spring-cloud-config-server`

**Q54 : Comment organiser les fichiers de configuration dans Git ?**
**R :**
```
config-repo/
├── application.properties          # Config commune à tous
├── user-service.properties         # Config user-service
├── user-service-dev.properties     # Environnement dev
├── user-service-prod.properties    # Environnement prod
└── order-service.properties
```

**Q55 : Comment un microservice récupère sa configuration ?**
**R :**
```properties
# bootstrap.properties (chargé avant application.properties)
spring.cloud.config.uri=http://localhost:8888
spring.application.name=user-service
spring.profiles.active=dev
```
Le service récupère automatiquement `user-service-dev.properties`

**Q56 : Comment rafraîchir la configuration sans redémarrer le service ?**
**R :** Utiliser Spring Cloud Bus avec `@RefreshScope` :
```java
@RestController
@RefreshScope
public class UserController {
    @Value("${message}")
    private String message; // Sera rafraîchi sans redémarrage
}
```
POST vers `/actuator/refresh` ou broadcast via Spring Cloud Bus

## 🔄 Load Balancing

**Q57 : Qu'est-ce que le Client-Side Load Balancing ?**
**R :** Le client (service appelant) choisit lui-même quelle instance appeler parmi celles disponibles dans Eureka. Contrairement au load balancing traditionnel (serveur dédié), la logique est dans le client.

**Q58 : Qu'est-ce que Spring Cloud LoadBalancer ?**
**R :** Module de Spring Cloud qui remplace Netflix Ribbon. Il effectue du load balancing client-side en répartissant les requêtes entre les instances disponibles dans Eureka.

**Q59 : Comment activer le load balancing avec RestTemplate ?**
**R :**
```java
@Configuration
public class Config {
    @Bean
    @LoadBalanced  // Active le load balancing
    public RestTemplate restTemplate() {
        return new RestTemplate();
    }
}
```
Ensuite, utiliser le nom du service :
```java
restTemplate.getForObject("http://USER-SERVICE/api/users/1", User.class);
```

**Q60 : Quels algorithmes de load balancing sont disponibles ?**
**R :**
- **Round Robin** (défaut) : Distribution circulaire
- **Random** : Sélection aléatoire
- **Weighted** : Basé sur des poids configurés
Configurable via `spring.cloud.loadbalancer.*`

## 🛡️ Circuit Breaker

**Q61 : Qu'est-ce qu'un Circuit Breaker et pourquoi est-il important ?**
**R :** Un Circuit Breaker protège un service des cascades de pannes. Si un service distant ne répond plus, au lieu de continuer à l'appeler (timeout répétés), le Circuit Breaker "ouvre" et retourne immédiatement une réponse de fallback, laissant le service distant se rétablir.

**Q62 : Quels sont les 3 états d'un Circuit Breaker ?**
**R :**
1. **CLOSED** : Normal, requêtes passent
2. **OPEN** : Service en panne détectée, requêtes bloquées, fallback retourné
3. **HALF_OPEN** : Test si le service est rétabli

**Q63 : Qu'est-ce que Resilience4j ?**
**R :** Bibliothèque de tolérance aux pannes remplaçant Netflix Hystrix (deprecated). Fournit Circuit Breaker, Retry, Rate Limiter, Bulkhead. Utilisé avec Spring Cloud Circuit Breaker.

**Q64 : Comment implémenter un Circuit Breaker avec Resilience4j ?**
**R :**
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
    
    // Méthode de fallback si user-service est down
    public User fallbackGetUser(Long id, Exception e) {
        return new User(id, "Unknown User");
    }
}
```

**Q65 : Comment configurer un Circuit Breaker ?**
**R :**
```yaml
resilience4j:
  circuitbreaker:
    instances:
      userService:
        failure-rate-threshold: 50        # 50% de failures = OPEN
        wait-duration-in-open-state: 10000 # 10s avant HALF_OPEN
        sliding-window-size: 10           # Fenêtre de 10 appels
        minimum-number-of-calls: 5        # Min 5 calls avant calcul
```

**Q66 : Qu'est-ce qu'une méthode fallback ?**
**R :** Méthode de secours appelée quand le Circuit Breaker est OPEN ou qu'une exception se produit. Permet de retourner une réponse par défaut au lieu de propager l'erreur (ex: utilisateur par défaut, cache, message d'erreur user-friendly).

## 🔎 Distributed Tracing

**Q67 : Qu'est-ce que le Distributed Tracing ?**
**R :** Dans les microservices, une requête traverse plusieurs services. Distributed Tracing assigne un ID unique (trace ID) à chaque requête et la suit à travers tous les services, permettant de voir le parcours complet et identifier les problèmes.

**Q68 : Qu'est-ce que Spring Cloud Sleuth ?**
**R :** Bibliothèque Spring Cloud qui ajoute automatiquement des trace IDs et span IDs aux logs de chaque service. Permet de corréler les logs de différents services pour une même requête.

**Q69 : Que sont les Trace ID et Span ID ?**
**R :**
- **Trace ID** : Identifiant unique pour toute la requête à travers tous les services
- **Span ID** : Identifiant unique pour chaque segment (appel à un service)

**Q70 : Comment Sleuth modifie-t-il les logs ?**
**R :**
```
# Sans Sleuth
INFO UserController: Getting user 123

# Avec Sleuth
INFO [user-service,a1b2c3d4e5f6,g7h8i9j0k1l2] UserController: Getting user 123
#    [app-name,   trace-id,    span-id]
```

**Q71 : Qu'est-ce que Zipkin ?**
**R :** Interface web pour visualiser les traces distribuées. Permet de voir graphiquement le parcours d'une requête à travers les microservices, les temps de réponse de chaque service, et identifier les goulots d'étranglement.

**Q72 : Comment intégrer Zipkin avec Sleuth ?**
**R :**
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
spring.sleuth.sampler.probability=1.0  # Trace 100% des requêtes
```

## 🌐 OpenFeign

**Q73 : Qu'est-ce que OpenFeign ?**
**R :** Client REST déclaratif qui simplifie les appels entre microservices. Au lieu d'utiliser RestTemplate, on définit une interface et Feign génère automatiquement l'implémentation.

**Q74 : Quelle est la différence entre RestTemplate et Feign ?**
**R :**
**RestTemplate (impératif) :**
```java
User user = restTemplate.getForObject(
    "http://USER-SERVICE/api/users/" + id,
    User.class
);
```

**Feign (déclaratif) :**
```java
@FeignClient(name = "user-service")
public interface UserClient {
    @GetMapping("/api/users/{id}")
    User getUser(@PathVariable Long id);
}

// Utilisation
User user = userClient.getUser(id);
```

**Q75 : Comment créer un Feign Client ?**
**R :**
```java
@FeignClient(name = "user-service")  // Nom du service dans Eureka
public interface UserClient {
    
    @GetMapping("/api/users/{id}")
    User getUser(@PathVariable Long id);
    
    @PostMapping("/api/users")
    User createUser(@RequestBody User user);
    
    @DeleteMapping("/api/users/{id}")
    void deleteUser(@PathVariable Long id);
}
```

**Q76 : Comment activer Feign dans Spring Boot ?**
**R :**
```java
@SpringBootApplication
@EnableFeignClients
public class OrderServiceApplication { }
```
Dépendance : `spring-cloud-starter-openfeign`

**Q77 : Feign supporte-t-il le load balancing ?**
**R :** Oui, automatiquement ! Si plusieurs instances de `user-service` sont enregistrées dans Eureka, Feign utilisera Spring Cloud LoadBalancer pour distribuer les appels.

**Q78 : Comment ajouter un Circuit Breaker à Feign ?**
**R :**
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

## 🏗️ Architecture Microservices

**Q79 : Décrivez une architecture microservices complète avec Spring Cloud**
**R :**

**Cette architecture microservices est composée d’une API Gateway qui gère le routage, la sécurité et le filtrage des requêtes.
Les différents microservices communiquent entre eux et s’enregistrent automatiquement dans Eureka (service registry).
La configuration centralisée est assurée par le Config Server, facilitant la gestion des paramètres.
Enfin, Zipkin permet le traçage des appels distribués pour surveiller les performances et détecter les problèmes.**
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
Chaque service communique via Feign avec load balancing et circuit breakers.

**Q80 : Quelle est l'évolution typique d'une application vers les microservices ?**
**R :**
1. **Phase 1 - MVP** : Monolithe Spring Boot simple
2. **Phase 2 - Croissance** : Séparation en microservices + Eureka + Gateway
3. **Phase 3 - Scale** : Ajout Config Server, Circuit Breakers, Tracing
4. **Phase 4 - Production** : Kubernetes, monitoring avancé, CI/CD complet

**Q81 : Spring Cloud est-il obligatoire pour faire des microservices ?**
**R :** Non, mais il simplifie énormément. On peut faire des microservices avec Spring Boot seul, mais il faudra gérer manuellement service discovery, load balancing, configuration, etc. Spring Cloud fournit des solutions éprouvées pour ces problèmes.

**Q82 : Quand NE PAS utiliser des microservices ?**
**R :**
- Petites applications / MVP
- Équipe réduite
- Domaine métier simple
- Pas besoin de scaling indépendant
Pour ces cas, un monolithe Spring Boot est préférable (plus simple, moins de complexité opérationnelle).

**Q83 : Quels sont les défis des microservices ?**
**R :**
- Complexité opérationnelle (monitoring, déploiement)
- Transactions distribuées difficiles
- Tests d'intégration complexes
- Latence réseau
- Debugging plus difficile
Spring Cloud aide à atténuer ces problèmes mais ne les élimine pas complètement.

**Q84 : Comment tester une architecture microservices ?**
**R :**
- **Tests unitaires** : Chaque service individuellement
- **Tests d'intégration** : Avec TestContainers (bases de données, message brokers)
- **Tests contrats** : Spring Cloud Contract pour vérifier les APIs
- **Tests end-to-end** : Environnement complet avec tous les services

**Q85 : Quelle est la différence entre Spring Cloud et Kubernetes ?**
**R :**
- **Spring Cloud** : Framework applicatif (code Java) pour patterns microservices
- **Kubernetes** : Plateforme d'orchestration de conteneurs (infrastructure)
Ils sont complémentaires ! Spring Cloud pour la logique applicative (circuit breakers, config), Kubernetes pour le déploiement et scaling.