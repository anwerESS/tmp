## [**..**](./00_index.md)

## Spring Boot Actuator - Points Clés

### **Concept général**
Spring Boot Actuator fournit des **production-ready features** pour monitorer et gérer une application Spring Boot. Il expose des **endpoints HTTP** qui donnent des informations sur l'état et les métriques de l'application.

### **Principaux Endpoints**

| Endpoint | Description |
|----------|-------------|
| `/actuator` | Liste tous les endpoints disponibles |
| `/actuator/health` | Application health status (UP/DOWN) |
| `/actuator/info` | Application metadata (version, description) |
| `/actuator/metrics` | Performance metrics (memory, CPU, requests) |
| `/actuator/env` | Environment properties et configuration |
| `/actuator/loggers` | Permet d'ajuster les log levels dynamiquement |
| `/actuator/beans` | Liste complète des Spring beans dans l'application |
| `/actuator/mappings` | Détails des Request Mappings (tous les endpoints REST/MVC) |

### **Configuration (application.yml)**

**Exposer les endpoints :**
```yaml
management:
  endpoints:
    web:
      exposure:
        include: '*'  # Expose tous les endpoints
        # OU spécifique: include: health, info, metrics
```

**Configurer le health endpoint :**
```yaml
management:
  endpoint:
    health:
      show-details: always  # Valeurs possibles: never, always, when-authorized
```

**Activer le shutdown endpoint :**
```yaml
management:
  endpoint:
    shutdown:
      enabled: true  # Permet d'arrêter l'app via POST /actuator/shutdown

server:
  shutdown: graceful  # Graceful shutdown (termine les requêtes en cours)
```

**Configurer les infos applicatives :**
```yaml
management:
  info:
    env:
      enabled: true  # Active les infos depuis les properties

info:
  app:
    name: MyNewApp
    description: App built by us
    version: 1.0.0
# Accessible via /actuator/info
```

### **Use Cases principaux**

1. **Monitoring** : Vérifier la santé de l'application en production (`/health`)
2. **Debugging** : Voir tous les beans Spring chargés (`/beans`)
3. **Performance** : Analyser les métriques (mémoire, threads, requests) (`/metrics`)
4. **Configuration** : Vérifier les properties actives (`/env`)
5. **Troubleshooting** : Ajuster les log levels sans redémarrage (`/loggers`)

### **Sécurité**

⚠️ **Important** : Par défaut, seuls `/health` et `/info` sont exposés. Les autres endpoints peuvent exposer des données sensibles.

**Best practices :**
- Exposer uniquement les endpoints nécessaires en production
- Sécuriser les endpoints avec Spring Security
- Utiliser `show-details: when-authorized` pour `/health`

```yaml
# Exemple sécurisé pour production
management:
  endpoints:
    web:
      exposure:
        include: health, info, metrics  # Limiter les endpoints
```

### **Points clés**

- Actuator fournit des **production-ready features** pour le monitoring
- Expose des endpoints via `/actuator/*`
- **Health endpoint** : vérifie si l'application est UP/DOWN
- **Metrics endpoint** : donne des statistiques de performance
- **Info endpoint** : affiche les métadonnées de l'application
- Configuration via `application.yml` ou `application.properties`
- Permet le **graceful shutdown** de l'application
- Important de **sécuriser les endpoints** en production

---

## Questions/Réponses - Spring Boot Actuator

### **1. Qu'est-ce que Spring Boot Actuator ?**

**Réponse :**
Spring Boot Actuator est un module qui fournit des **production-ready features** pour monitorer et gérer une application Spring Boot. Il expose des **endpoints HTTP** qui permettent de :
- Vérifier le **health** de l'application
- Consulter les **metrics** (performance, mémoire, CPU)
- Voir les **beans** Spring chargés
- Analyser les **mappings** des endpoints
- Modifier les **log levels** dynamiquement

C'est essentiel pour le **monitoring** et le **debugging** en production.

---

### **2. Quels sont les principaux endpoints fournis par Actuator ?**

**Réponse :**
Les endpoints les plus importants sont :

- **`/actuator`** : Liste tous les endpoints disponibles
- **`/actuator/health`** : Status de santé de l'application (UP/DOWN)
- **`/actuator/info`** : Métadonnées de l'application (version, description)
- **`/actuator/metrics`** : Métriques de performance (JVM, HTTP requests, database)
- **`/actuator/env`** : Environment properties et configuration
- **`/actuator/beans`** : Liste complète des Spring beans
- **`/actuator/mappings`** : Tous les Request Mappings (REST/MVC endpoints)
- **`/actuator/loggers`** : Gestion des log levels
- **`/actuator/shutdown`** : Permet d'arrêter l'application (doit être activé)

---

### **3. Comment exposer tous les endpoints Actuator ?**

**Réponse :**
Par défaut, seuls `/health` et `/info` sont exposés. Pour exposer tous les endpoints :

```yaml
management:
  endpoints:
    web:
      exposure:
        include: '*'  # Expose tous les endpoints
```

Pour exposer des endpoints spécifiques :
```yaml
management:
  endpoints:
    web:
      exposure:
        include: health, info, metrics, beans
```

⚠️ **Attention** : En production, il faut limiter les endpoints exposés pour des raisons de sécurité.

---

### **4. Comment configurer le health endpoint pour afficher plus de détails ?**

**Réponse :**
Par défaut, `/actuator/health` montre uniquement le status (UP/DOWN). Pour voir les détails :

```yaml
management:
  endpoint:
    health:
      show-details: always
```

**Valeurs possibles :**
- **`never`** : Pas de détails (seulement UP/DOWN)
- **`always`** : Toujours afficher les détails
- **`when-authorized`** : Afficher uniquement si l'utilisateur est authentifié

**Résultat avec `show-details: always` :**
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

### **5. Comment activer le shutdown endpoint ?**

**Réponse :**
Le **shutdown endpoint** permet d'arrêter l'application via une requête HTTP POST. Il est **désactivé par défaut**.

**Configuration :**
```yaml
management:
  endpoint:
    shutdown:
      enabled: true

server:
  shutdown: graceful  # Termine les requêtes en cours avant d'arrêter
```

**Utilisation :**
```bash
curl -X POST http://localhost:8080/actuator/shutdown
```

⚠️ **Sécurité** : Ce endpoint doit être **protégé** avec Spring Security en production !

---

### **6. Comment afficher des informations personnalisées dans /actuator/info ?**

**Réponse :**
Ajouter des propriétés dans `application.yml` :

```yaml
management:
  info:
    env:
      enabled: true  # Active la lecture des properties 'info.*'

info:
  app:
    name: MySpringApp
    description: Application de gestion utilisateurs
    version: 1.0.0
    developer: Mohamed
```

**Résultat de `/actuator/info` :**
```json
{
  "app": {
    "name": "MySpringApp",
    "description": "Application de gestion utilisateurs",
    "version": "1.0.0",
    "developer": "Mohamed"
  }
}
```

---

### **7. Comment consulter les métriques d'une application ?**

**Réponse :**
Utiliser `/actuator/metrics` qui liste toutes les métriques disponibles :

```bash
GET /actuator/metrics
```

**Résultat :**
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

Pour voir une métrique spécifique :
```bash
GET /actuator/metrics/jvm.memory.used
GET /actuator/metrics/http.server.requests
```

**Use cases :**
- Voir la **mémoire JVM** utilisée
- Compter le nombre de **HTTP requests**
- Monitorer les **threads** actifs
- Vérifier l'utilisation **CPU**

---

### **8. Quelle est la différence entre /health et /info ?**

**Réponse :**
- **`/actuator/health`** : 
  - Indique si l'application fonctionne correctement (UP/DOWN)
  - Vérifie les **dépendances** : database, disk space, external services
  - Utilisé pour les **health checks** (Kubernetes, load balancers)
  
- **`/actuator/info`** :
  - Affiche des **métadonnées statiques** sur l'application
  - Version, nom, description, developer
  - Informations configurées manuellement

**Exemple :**
```json
// /actuator/health
{"status": "UP"}

// /actuator/info
{"app": {"name": "MyApp", "version": "1.0.0"}}
```

---

### **9. Comment voir tous les beans Spring chargés dans l'application ?**

**Réponse :**
Utiliser le endpoint `/actuator/beans` :

```bash
GET /actuator/beans
```

**Résultat :** JSON avec tous les beans, leurs **dependencies**, et leur **scope**.

**Use case :** 
- Debugging : vérifier si un bean est bien créé
- Comprendre les dépendances entre beans
- Identifier les problèmes de configuration

---

### **10. Comment voir tous les endpoints REST de l'application ?**

**Réponse :**
Utiliser `/actuator/mappings` qui liste tous les **Request Mappings** :

```bash
GET /actuator/mappings
```

**Résultat :** Liste de tous les endpoints avec :
- HTTP method (GET, POST, etc.)
- URL pattern
- Controller method
- Produces/Consumes

**Use case :**
- Documenter l'API
- Vérifier les routes disponibles
- Debugging des mappings

---

### **11. Comment changer le log level dynamiquement sans redémarrer l'application ?**

**Réponse :**
Utiliser le endpoint `/actuator/loggers` :

**Voir le log level actuel :**
```bash
GET /actuator/loggers/com.example.myapp
```

**Changer le log level :**
```bash
POST /actuator/loggers/com.example.myapp
Content-Type: application/json

{
  "configuredLevel": "DEBUG"
}
```

**Niveaux disponibles :** TRACE, DEBUG, INFO, WARN, ERROR, OFF

**Use case :** Activer le mode DEBUG en production pour investiguer un bug sans redémarrer.

---

### **12. Comment sécuriser les endpoints Actuator ?**

**Réponse :**
Les endpoints Actuator peuvent exposer des **données sensibles**. Il faut les sécuriser avec **Spring Security** :

```java
@Configuration
public class SecurityConfig {
    
    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
        http
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/actuator/health").permitAll()  // Public
                .requestMatchers("/actuator/**").hasRole("ADMIN") // Admin seulement
                .anyRequest().authenticated()
            )
            .httpBasic();
        return http.build();
    }
}
```

**Best practices :**
- Exposer uniquement `/health` et `/info` publiquement
- Protéger les autres endpoints avec authentication
- En production, limiter les endpoints exposés

---

### **13. Qu'est-ce que le graceful shutdown et comment le configurer ?**

**Réponse :**
Le **graceful shutdown** permet d'arrêter l'application **proprement** en terminant d'abord les requêtes en cours avant de s'arrêter complètement.

**Configuration :**
```yaml
server:
  shutdown: graceful  # Active le graceful shutdown

spring:
  lifecycle:
    timeout-per-shutdown-phase: 30s  # Temps max d'attente (défaut: 30s)
```

**Comportement :**
1. Application reçoit un signal d'arrêt
2. N'accepte plus de nouvelles requêtes
3. Termine les requêtes en cours (max 30s)
4. S'arrête complètement

**Use case :** Éviter de couper des transactions en cours lors d'un déploiement.

---

### **14. Comment consulter les environment properties de l'application ?**

**Réponse :**
Utiliser `/actuator/env` :

```bash
GET /actuator/env
```

**Résultat :** Toutes les properties avec leurs sources :
- `application.yml`
- System properties
- Environment variables
- Command line arguments

**Voir une property spécifique :**
```bash
GET /actuator/env/server.port
```

⚠️ **Attention** : Peut exposer des **secrets** (passwords, API keys). À sécuriser !

---

### **15. Comment monitorer les performances de l'application avec Actuator ?**

**Réponse :**
Utiliser `/actuator/metrics` pour consulter :

**1. Mémoire JVM :**
```bash
GET /actuator/metrics/jvm.memory.used
GET /actuator/metrics/jvm.memory.max
```

**2. HTTP Requests :**
```bash
GET /actuator/metrics/http.server.requests
# Voir count, temps moyen, max
```

**3. CPU Usage :**
```bash
GET /actuator/metrics/system.cpu.usage
```

**4. Threads :**
```bash
GET /actuator/metrics/jvm.threads.live
```

**5. Database Connection Pool :**
```bash
GET /actuator/metrics/hikaricp.connections.active
```

**Intégration avec outils externes :**
- **Prometheus** : scrape les metrics Actuator
- **Grafana** : visualisation des metrics
- **Micrometer** : framework de metrics sous-jacent

---

### **16. Quelle est la dépendance Maven/Gradle pour Actuator ?**

**Réponse :**

**Maven :**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-actuator</artifactId>
</dependency>
```

**Gradle :**
```gradle
implementation 'org.springframework.boot:spring-boot-starter-actuator'
```

C'est la seule dépendance nécessaire pour activer Actuator.

---

### **17. Comment créer un custom health indicator ?**

**Réponse :**
Implémenter l'interface `HealthIndicator` :

```java
@Component
public class CustomHealthIndicator implements HealthIndicator {
    
    @Override
    public Health health() {
        // Vérifier l'état d'un service externe
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
        // Logique de vérification
        return true;
    }
}
```

**Résultat dans `/actuator/health` :**
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

### **18. Quelle est la différence entre Actuator et Spring Admin ?**

**Réponse :**
- **Spring Boot Actuator** :
  - Fournit les **endpoints HTTP** de monitoring
  - Intégré directement dans l'application
  - Retourne du JSON brut

- **Spring Boot Admin** :
  - **Interface web graphique** pour visualiser les données d'Actuator
  - Application séparée qui se connecte aux endpoints Actuator
  - Dashboard avec graphiques, alertes, notifications
  - Permet de monitorer **plusieurs applications** depuis un seul endroit

**Relation :** Spring Admin **consomme** les endpoints Actuator pour afficher les infos graphiquement.

---

### **19. Comment filtrer les metrics retournées par Actuator ?**

**Réponse :**
Utiliser la configuration pour exclure certaines metrics :

```yaml
management:
  metrics:
    enable:
      jvm: true
      system: false  # Désactive les metrics système
```

Ou créer un `MeterFilter` personnalisé :
```java
@Configuration
public class MetricsConfig {
    
    @Bean
    public MeterFilter customMeterFilter() {
        return MeterFilter.deny(id -> {
            // Filtrer les metrics qui commencent par "jvm.gc"
            return id.getName().startsWith("jvm.gc");
        });
    }
}
```

---

### **20. Quels sont les use cases principaux d'Actuator en production ?**

**Réponse :**

**1. Health Checks :**
- Kubernetes liveness/readiness probes
- Load balancer health checks

**2. Monitoring & Alerting :**
- Intégration avec Prometheus/Grafana
- Alertes si memory > 80%

**3. Troubleshooting :**
- Changer log level en DEBUG sans redéployer
- Analyser les beans chargés

**4. Performance Analysis :**
- Identifier les endpoints lents
- Monitorer la mémoire JVM

**5. Audit & Compliance :**
- Tracker les HTTP requests
- Voir les configurations actives

**6. Graceful Deployments :**
- Shutdown graceful pour zero-downtime deploys

---

**Conseil final :** Prépare-toi à expliquer comment tu utiliserais Actuator pour débugger un problème de performance en production (ex: memory leak). C'est une question pratique très courante ! 💪
