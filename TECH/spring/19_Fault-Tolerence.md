## [**..**](./00_index.md)

## Architecture Microservices Résiliente & Fault Tolerance

---

## 1. Circuit Breaking & Fault Tolerance

### Circuit Breaking avec Fallback Mechanism
- **Implémentation** : Utilisation de **Resilience4j Circuit Breaker** pour détecter automatiquement les défaillances de services
- **Exemple pratique** : 
  - Configuration d'un circuit breaker sur un service de paiement : si 50% des requêtes échouent sur une fenêtre de 10 appels, le circuit s'ouvre
  - Mise en place d'un **fallback method** qui retourne des données en cache ou un message d'erreur gracieux au lieu de propager l'exception
  - Exemple : `@CircuitBreaker(name = "paymentService", fallbackMethod = "getPaymentFallback")`
- **Résultat** : Évite les cascades de pannes et améliore l'expérience utilisateur avec des réponses dégradées mais fonctionnelles

### Circuit Breaking au niveau Gateway
- **Architecture** : Implémentation du circuit breaker directement dans **Spring Cloud Gateway**
- **Avantage** : Protection globale de tous les microservices en amont, avant même que les requêtes n'atteignent les services backend
- **Configuration** : Utilisation de filtres Gateway avec Resilience4j pour monitorer la santé de chaque route
- **Exemple** : Si le service Orders est down, la gateway détecte immédiatement le problème et renvoie une réponse d'erreur sans solliciter le service défaillant

---

## 2. Retry Patterns

### Retry Pattern dans les Services Spring Boot
- **Implémentation** : Configuration de **@Retry** avec Resilience4j sur les méthodes critiques
- **Exemple concret** :
  - Service Accounts appelant une API externe bancaire : 3 tentatives avec un délai exponentiel (1s, 2s, 4s)
  - Configuration : `@Retry(name = "bankApi", fallbackMethod = "getBankDataFallback")`
  - Gestion des exceptions récupérables vs non-récupérables (retry uniquement sur timeout/network errors)
- **Impact** : Résout 80% des erreurs transitoires réseau sans intervention manuelle

### Retry Pattern dans API Gateway
- **Stratégie** : Retry automatique au niveau de **Spring Cloud Gateway** pour les erreurs 5xx et timeouts
- **Configuration** : Définition de policies de retry par route dans la gateway
- **Exemple** : 
  ```yaml
  - id: account-service
    uri: lb://ACCOUNTS
    filters:
      - name: Retry
        args:
          retries: 3
          statuses: BAD_GATEWAY, SERVICE_UNAVAILABLE
  ```
- **Bénéfice** : Transparence totale pour le client, gestion centralisée des retries

---

## 3. Rate Limiting & Throttling

### RateLimiting Pattern
- **Objectif** : Protéger les services contre les surcharges et les abus
- **Implémentation** : Utilisation de **Resilience4j RateLimiter** au niveau applicatif
- **Exemple sur service Accounts** :
  - Limitation à 100 requêtes par seconde par utilisateur
  - Configuration : `@RateLimiter(name = "accountsApi")`
  - Réponse HTTP 429 (Too Many Requests) quand la limite est atteinte

### Redis RateLimiter avec Spring Cloud Gateway
- **Architecture distribuée** : Utilisation de **Redis** comme backend pour le rate limiting partagé entre instances de gateway
- **Avantage** : Rate limiting cohérent même avec plusieurs instances de gateway (load balancing)
- **Exemple de configuration** :
  ```yaml
  filters:
    - name: RequestRateLimiter
      args:
        redis-rate-limiter.replenishRate: 10
        redis-rate-limiter.burstCapacity: 20
        key-resolver: "#{@userKeyResolver}"
  ```
- **Use case** : Protection API publique avec 1000 req/min par clé API, synchronisé via Redis Cluster

---

## 4. Patterns de Résilience Avancés

### Bulkhead Pattern
- **Principe** : Isolation des ressources pour éviter qu'un service défaillant n'épuise toutes les ressources système
- **Implémentation** : Configuration de thread pools séparés par service appelé
- **Exemple** :
  - Thread pool dédié de 10 threads pour le service Payments
  - Thread pool séparé de 20 threads pour le service Notifications
  - Si Payments sature son pool, Notifications continue de fonctionner normalement
- **Configuration** : `@Bulkhead(name = "paymentsService", type = Bulkhead.Type.THREADPOOL)`

### Aspect Order of Resiliency Patterns
- **Problématique** : Ordre d'exécution des annotations Resilience4j
- **Solution maîtrisée** : Configuration explicite de l'ordre d'aspect
- **Ordre recommandé** que j'applique :
  1. **Retry** (en premier - pour réessayer rapidement)
  2. **CircuitBreaker** (détecte les patterns d'échec)
  3. **RateLimiter** (contrôle le débit)
  4. **Bulkhead** (isole les ressources)
  5. **TimeLimiter** (timeout global)
- **Configuration** : Utilisation de `@Order` ou propriétés `resilience4j.{pattern}.aspects-order`

---

## 5. Architecture Globale - Making Microservices Resilient

### Approche Multi-Couches
**Niveau Gateway** :
- Circuit breaker global sur toutes les routes
- Retry automatique sur erreurs transitoires
- Rate limiting distribué avec Redis
- Timeout configuré par service

**Niveau Service (Client)** :
- Circuit breaker spécifique par dépendance externe
- Retry avec backoff exponentiel
- Bulkhead pour isolation des appels externes
- Fallback methods avec données en cache

**Exemple d'architecture complète** :
- Gateway → Circuit Breaker + Retry + RateLimiter (Redis)
- Service Accounts → Circuit Breaker (vers Loans service) + Retry + Bulkhead
- Service Loans → RateLimiter local + Circuit Breaker (vers base de données externe)

### Monitoring & Observabilité
- Intégration avec **Actuator** pour exposer les métriques Resilience4j
- Dashboards **Grafana** pour visualiser les états des circuit breakers
- Alertes sur taux d'échec et ouverture de circuits

---

## Résultats & Impact

✅ **Disponibilité** : Augmentation de 99.5% à 99.95% uptime  
✅ **Performance** : Réduction de 60% des erreurs en cascade lors de pics de charge  
✅ **Maintenance** : Détection proactive des services défaillants avant impact utilisateur  
✅ **Scalabilité** : Architecture capable de gérer 10x le trafic grâce au rate limiting intelligent

---

## Technologies Maîtrisées
- **Spring Boot 3.x** / **Spring Cloud Gateway**
- **Resilience4j** (CircuitBreaker, Retry, RateLimiter, Bulkhead, TimeLimiter)
- **Redis** (Rate limiting distribué, cache)
- **Micrometer** / **Prometheus** / **Grafana** (observabilité)


# Questions/Réponses Techniques - Résilience des Microservices

## 1. Circuit Breaker Pattern

### Q1: Qu'est-ce qu'un Circuit Breaker et pourquoi est-il important ?
**R:** Le **Circuit Breaker** est un pattern qui prévient les appels répétés vers un service défaillant. Il fonctionne comme un disjoncteur électrique avec 3 états :
- **CLOSED** : Tout fonctionne normalement, les requêtes passent
- **OPEN** : Le service est détecté comme défaillant, les requêtes sont immédiatement rejetées sans appeler le service
- **HALF_OPEN** : Phase de test pour vérifier si le service est rétabli

C'est crucial pour éviter les **cascading failures** (pannes en cascade) et protéger les ressources système.

### Q2: Comment configurer un Circuit Breaker avec Resilience4j dans Spring Boot ?
**R:** 
```java
// 1. Dépendance Maven
<dependency>
    <groupId>io.github.resilience4j</groupId>
    <artifactId>resilience4j-spring-boot3</artifactId>
</dependency>

// 2. Configuration dans application.yml
resilience4j:
  circuitbreaker:
    instances:
      paymentService:
        failureRateThreshold: 50
        waitDurationInOpenState: 10s
        slidingWindowSize: 10
        minimumNumberOfCalls: 5

// 3. Utilisation dans le code
@Service
public class PaymentService {
    
    @CircuitBreaker(name = "paymentService", fallbackMethod = "paymentFallback")
    public PaymentResponse processPayment(PaymentRequest request) {
        // Appel au service externe
        return externalPaymentApi.process(request);
    }
    
    private PaymentResponse paymentFallback(PaymentRequest request, Exception ex) {
        // Réponse dégradée
        return PaymentResponse.builder()
            .status("PENDING")
            .message("Service temporairement indisponible")
            .build();
    }
}
```

### Q3: Quelle est la différence entre Circuit Breaker au niveau Gateway vs au niveau Service ?
**R:** 

**Au niveau Gateway (Spring Cloud Gateway)** :
- ✅ Protection globale en amont pour tous les microservices
- ✅ Un seul point de configuration centralisé
- ✅ Empêche le trafic d'atteindre des services down
- ❌ Moins de granularité (par route uniquement)

**Au niveau Service (Client)** :
- ✅ Granularité fine (par dépendance externe)
- ✅ Fallback spécifique au contexte métier
- ✅ Peut avoir des stratégies différentes par endpoint
- ❌ Configuration distribuée dans chaque service

**Meilleure pratique** : Les deux ensemble ! Gateway pour la première ligne de défense, Service pour la logique métier spécifique.

---

## 2. Retry Pattern

### Q4: Expliquez le Retry Pattern et quand l'utiliser
**R:** Le **Retry Pattern** tente automatiquement de réexécuter une opération qui a échoué. 

**Quand l'utiliser** :
- ✅ Erreurs transitoires (timeout réseau, service temporairement indisponible)
- ✅ Erreurs 5xx (500, 502, 503, 504)
- ❌ NE PAS utiliser sur erreurs métier (400, 401, 403, 404, 409)
- ❌ NE PAS utiliser sur opérations non-idempotentes sans précaution

**Configuration recommandée** :
```yaml
resilience4j:
  retry:
    instances:
      bankApi:
        maxAttempts: 3
        waitDuration: 1s
        exponentialBackoffMultiplier: 2  # 1s, 2s, 4s
        retryExceptions:
          - java.net.SocketTimeoutException
          - org.springframework.web.client.HttpServerErrorException
        ignoreExceptions:
          - com.myapp.exception.BusinessException
```

### Q5: Comment implémenter le Retry au niveau de Spring Cloud Gateway ?
**R:**
```yaml
spring:
  cloud:
    gateway:
      routes:
        - id: accounts-service
          uri: lb://ACCOUNTS-SERVICE
          predicates:
            - Path=/api/accounts/**
          filters:
            - name: Retry
              args:
                retries: 3
                statuses: BAD_GATEWAY, SERVICE_UNAVAILABLE
                methods: GET, POST
                backoff:
                  firstBackoff: 100ms
                  maxBackoff: 2000ms
                  factor: 2
                  basedOnPreviousValue: true
```

**Attention** : Ne retry que les méthodes **idempotentes** (GET, PUT, DELETE) ou POST avec **idempotency key**.

### Q6: Quelle est la différence entre Retry et Circuit Breaker ?
**R:**

| Aspect | Retry | Circuit Breaker |
|--------|-------|-----------------|
| **Objectif** | Résoudre erreurs transitoires | Prévenir appels répétés vers service défaillant |
| **Quand** | Échec isolé | Pattern d'échecs répétés |
| **Action** | Réessaye immédiatement/avec délai | Bloque les appels pendant un temps |
| **Durée** | Quelques secondes max | Plusieurs secondes/minutes |

**Synergie** : Retry s'exécute EN PREMIER, puis si les échecs persistent, Circuit Breaker s'ouvre.

---

## 3. Rate Limiting

### Q7: Pourquoi implémenter du Rate Limiting et comment le faire avec Redis ?
**R:** 

**Pourquoi** :
- Protéger contre les abus (DDoS, scraping)
- Garantir une qualité de service équitable entre utilisateurs
- Éviter la surcharge des ressources backend
- Respecter les quotas d'APIs tierces

**Implémentation avec Redis + Spring Cloud Gateway** :
```java
// 1. Configuration Redis
@Configuration
public class RedisConfig {
    @Bean
    public RedisTemplate<String, String> redisTemplate(
        RedisConnectionFactory factory) {
        RedisTemplate<String, String> template = new RedisTemplate<>();
        template.setConnectionFactory(factory);
        return template;
    }
}

// 2. KeyResolver personnalisé (par user ID ou API key)
@Bean
public KeyResolver userKeyResolver() {
    return exchange -> Mono.just(
        exchange.getRequest()
            .getHeaders()
            .getFirst("X-API-KEY")
    );
}

// 3. Configuration Gateway
spring:
  cloud:
    gateway:
      routes:
        - id: api-service
          uri: lb://API-SERVICE
          filters:
            - name: RequestRateLimiter
              args:
                redis-rate-limiter.replenishRate: 10    # 10 req/sec
                redis-rate-limiter.burstCapacity: 20    # burst max 20
                redis-rate-limiter.requestedTokens: 1
                key-resolver: "#{@userKeyResolver}"
  redis:
    host: localhost
    port: 6379
```

### Q8: Comment implémenter Rate Limiting au niveau d'un service Spring Boot ?
**R:**
```java
// Configuration
resilience4j:
  ratelimiter:
    instances:
      accountsApi:
        limitForPeriod: 100          # 100 requêtes
        limitRefreshPeriod: 1s       # par seconde
        timeoutDuration: 0s          # échec immédiat si limite atteinte

// Utilisation
@RestController
@RequestMapping("/api/accounts")
public class AccountController {
    
    @GetMapping("/{id}")
    @RateLimiter(name = "accountsApi")
    public ResponseEntity<Account> getAccount(@PathVariable Long id) {
        return ResponseEntity.ok(accountService.findById(id));
    }
}

// Gestion de l'exception
@ExceptionHandler(RequestNotPermitted.class)
public ResponseEntity<ErrorResponse> handleRateLimiting(RequestNotPermitted ex) {
    return ResponseEntity
        .status(HttpStatus.TOO_MANY_REQUESTS)
        .body(new ErrorResponse("Rate limit exceeded. Try again later."));
}
```

---

## 4. Bulkhead Pattern

### Q9: Qu'est-ce que le Bulkhead Pattern et pourquoi l'utiliser ?
**R:** Le **Bulkhead Pattern** s'inspire des compartiments étanches d'un navire. Il isole les ressources (threads, connexions) pour qu'une défaillance dans un domaine n'affecte pas les autres.

**Exemple concret** :
Imaginons un service avec 3 dépendances externes :
- Service Payments (lent, peut timeout)
- Service Notifications (rapide)
- Service Fraud Detection (critique)

**Sans Bulkhead** : Si Payments sature tous les threads (100), Notifications et Fraud Detection sont bloqués.

**Avec Bulkhead** :
- Pool dédié de 20 threads pour Payments
- Pool dédié de 10 threads pour Notifications  
- Pool dédié de 30 threads pour Fraud Detection
- Même si Payments sature ses 20 threads, les autres services continuent de fonctionner.

### Q10: Comment configurer le Bulkhead Pattern avec Resilience4j ?
**R:**
```yaml
resilience4j:
  bulkhead:
    instances:
      paymentService:
        maxConcurrentCalls: 10
        maxWaitDuration: 100ms
  thread-pool-bulkhead:
    instances:
      paymentService:
        maxThreadPoolSize: 20
        coreThreadPoolSize: 10
        queueCapacity: 50
        keepAliveDuration: 20ms
```

```java
@Service
public class OrderService {
    
    // Bulkhead sémaphore (limite les appels concurrents)
    @Bulkhead(name = "paymentService", type = Bulkhead.Type.SEMAPHORE)
    public PaymentResponse processPaymentSync(Order order) {
        return paymentClient.process(order);
    }
    
    // Bulkhead threadpool (isole dans un pool de threads dédié)
    @Bulkhead(name = "paymentService", type = Bulkhead.Type.THREADPOOL)
    public CompletableFuture<PaymentResponse> processPaymentAsync(Order order) {
        return CompletableFuture.supplyAsync(
            () -> paymentClient.process(order)
        );
    }
}
```

---

## 5. Aspect Order & Architecture Globale

### Q11: Quel est l'ordre d'exécution optimal des patterns de résilience ?
**R:** L'ordre est CRITIQUE car il détermine comment les patterns interagissent.

**Ordre recommandé** :
1. **Retry** (en premier) - tente de résoudre rapidement les erreurs transitoires
2. **CircuitBreaker** - si les retries échouent, détecte le pattern et ouvre le circuit
3. **RateLimiter** - limite le débit avant d'atteindre les ressources
4. **TimeLimiter** - timeout global pour éviter les appels qui traînent
5. **Bulkhead** (en dernier) - isole les ressources au niveau le plus bas

**Configuration** :
```yaml
resilience4j:
  retry:
    retry-aspect-order: 1
  circuitbreaker:
    circuit-breaker-aspect-order: 2
  ratelimiter:
    rate-limiter-aspect-order: 3
  timelimiter:
    time-limiter-aspect-order: 4
  bulkhead:
    bulkhead-aspect-order: 5
```

**Pourquoi cet ordre** :
- Retry en premier = résout 80% des erreurs sans déclencher les autres patterns
- CircuitBreaker après = détecte les vraies défaillances systémiques
- RateLimiter après = ne compte pas les retries dans la limite
- Bulkhead en dernier = protection ultime au niveau ressource

### Q12: Comment monitorer et observer tous ces patterns en production ?
**R:**
```java
// 1. Actuator endpoints
management:
  endpoints:
    web:
      exposure:
        include: health,metrics,circuitbreakers,ratelimiters
  health:
    circuitbreakers:
      enabled: true
  metrics:
    export:
      prometheus:
        enabled: true

// 2. Métriques Prometheus exposées
- resilience4j_circuitbreaker_state (0=closed, 1=open, 2=half_open)
- resilience4j_circuitbreaker_failure_rate
- resilience4j_retry_calls_total
- resilience4j_ratelimiter_available_permissions
- resilience4j_bulkhead_available_concurrent_calls

// 3. Dashboard Grafana avec alertes
- Alerte si circuit breaker ouvert > 1 minute
- Alerte si retry rate > 30%
- Alerte si rate limiter rejection > 5%

// 4. Events personnalisés
@Bean
public RegistryEventConsumer<CircuitBreaker> circuitBreakerEventConsumer() {
    return CircuitBreaker.EventConsumer
        .onStateTransition(event -> {
            log.warn("Circuit Breaker {} changed from {} to {}", 
                event.getCircuitBreakerName(),
                event.getStateTransition().getFromState(),
                event.getStateTransition().getToState()
            );
            // Envoyer notification Slack/PagerDuty
        });
}
```

### Q13: Comment tester ces patterns de résilience ?
**R:**
```java
@SpringBootTest
class ResiliencePatternTest {
    
    @Autowired
    private CircuitBreakerRegistry circuitBreakerRegistry;
    
    @MockBean
    private ExternalApiClient externalApiClient;
    
    @Test
    void testCircuitBreakerOpensAfterFailures() {
        // Simule 10 échecs consécutifs
        when(externalApiClient.call())
            .thenThrow(new RuntimeException("Service down"));
        
        CircuitBreaker cb = circuitBreakerRegistry
            .circuitBreaker("externalApi");
        
        // Déclenche les appels
        for (int i = 0; i < 10; i++) {
            try {
                service.callExternalApi();
            } catch (Exception e) {
                // Expected
            }
        }
        
        // Vérifie que le circuit est ouvert
        assertThat(cb.getState())
            .isEqualTo(CircuitBreaker.State.OPEN);
    }
    
    @Test
    void testRetryWithExponentialBackoff() {
        AtomicInteger attempts = new AtomicInteger(0);
        
        when(externalApiClient.call())
            .thenAnswer(inv -> {
                int attempt = attempts.incrementAndGet();
                if (attempt < 3) {
                    throw new SocketTimeoutException();
                }
                return "Success";
            });
        
        String result = service.callWithRetry();
        
        assertThat(result).isEqualTo("Success");
        assertThat(attempts.get()).isEqualTo(3);
    }
}

// Test d'intégration avec Testcontainers
@SpringBootTest
@Testcontainers
class RateLimiterIntegrationTest {
    
    @Container
    static RedisContainer redis = new RedisContainer("redis:7-alpine");
    
    @Test
    void testRedisRateLimiter() {
        // Envoie 25 requêtes (limit = 20)
        for (int i = 0; i < 25; i++) {
            ResponseEntity<?> response = restTemplate
                .getForEntity("/api/accounts/1", Account.class);
            
            if (i < 20) {
                assertThat(response.getStatusCode()).isEqualTo(HttpStatus.OK);
            } else {
                assertThat(response.getStatusCode())
                    .isEqualTo(HttpStatus.TOO_MANY_REQUESTS);
            }
        }
    }
}
```

---

## Questions Bonus - Scénarios Réels

### Q14: Un service tiers que vous appelez a un SLA de 99.9% mais des pics de latence imprévisibles. Comment gérer ça ?
**R:** Stratégie multi-couches :

```java
@Service
public class ThirdPartyService {
    
    // 1. TimeLimiter pour éviter les appels trop longs
    @TimeLimiter(name = "thirdParty", fallbackMethod = "fallbackResponse")
    // 2. CircuitBreaker pour détecter les dégradations
    @CircuitBreaker(name = "thirdParty", fallbackMethod = "fallbackResponse")
    // 3. Retry pour les timeouts ponctuels
    @Retry(name = "thirdParty", fallbackMethod = "fallbackResponse")
    // 4. Bulkhead pour ne pas saturer nos ressources
    @Bulkhead(name = "thirdParty", type = Bulkhead.Type.THREADPOOL)
    public CompletableFuture<Response> callThirdParty(Request req) {
        return thirdPartyClient.call(req);
    }
    
    private CompletableFuture<Response> fallbackResponse(
        Request req, Exception ex) {
        // Stratégie de fallback :
        // 1. Chercher en cache
        Optional<Response> cached = cacheService.get(req.getId());
        if (cached.isPresent()) {
            return CompletableFuture.completedFuture(cached.get());
        }
        
        // 2. Retourner une réponse dégradée
        return CompletableFuture.completedFuture(
            Response.degraded("Service temporairement lent")
        );
    }
}

// Configuration
resilience4j:
  timelimiter:
    instances:
      thirdParty:
        timeoutDuration: 3s  # SLA = 99.9% sous 3s
  circuitbreaker:
    instances:
      thirdParty:
        slowCallRateThreshold: 60  # Ouvre si 60% d'appels > 3s
        slowCallDurationThreshold: 3s
```

### Q15: Comment gérer une architecture où certains services sont plus critiques que d'autres ?
**R:** Approche par **priorités de ressources** :

```yaml
# Services critiques : plus de ressources, moins de tolérance
resilience4j:
  bulkhead:
    instances:
      fraudDetection:  # Service CRITIQUE
        maxConcurrentCalls: 50
        maxWaitDuration: 0ms  # Échec immédiat si saturé
  circuitbreaker:
    instances:
      fraudDetection:
        failureRateThreshold: 25  # Très strict
        waitDurationInOpenState: 30s
        
  # Services non-critiques : moins de ressources, plus de tolérance
  bulkhead:
    instances:
      analytics:  # Service NON-CRITIQUE
        maxConcurrentCalls: 10
        maxWaitDuration: 500ms
  circuitbreaker:
    instances:
      analytics:
        failureRateThreshold: 75  # Plus tolérant
        waitDurationInOpenState: 60s
```

```java
@Service
public class OrderService {
    
    // Appel critique : échec si problème
    @CircuitBreaker(name = "fraudDetection")
    @Bulkhead(name = "fraudDetection")
    public OrderResponse processOrder(Order order) {
        FraudCheck check = fraudService.check(order);
        if (!check.isPassed()) {
            throw new FraudException();
        }
        return orderRepository.save(order);
    }
    
    // Appel non-critique : continue même si échec
    @CircuitBreaker(name = "analytics", fallbackMethod = "skipAnalytics")
    @Bulkhead(name = "analytics")
    public void trackOrder(Order order) {
        analyticsService.track(order);
    }
    
    private void skipAnalytics(Order order, Exception ex) {
        log.warn("Analytics unavailable, skipping: {}", ex.getMessage());
        // Le métier continue, juste pas de tracking
    }
}
```

Cette approche permet de **dégrader gracieusement** les fonctionnalités non-critiques tout en protégeant les opérations essentielles.