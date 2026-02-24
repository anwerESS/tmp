## [**..**](./00_index.md)

## Resilient Microservices Architecture & Fault Tolerance

---

## 1. Circuit Breaking & Fault Tolerance

### Circuit Breaking with Fallback Mechanism
- **Implementation** : Using **Resilience4j Circuit Breaker** to automatically detect service failures
- **Practical example** :
  - Configuring a circuit breaker on a payment service : if 50% of requests fail over a 10-call window, the circuit opens
  - Setting up a **fallback method** that returns cached data or a graceful error message instead of propagating the exception
  - Example : `@CircuitBreaker(name = "paymentService", fallbackMethod = "getPaymentFallback")`
- **Result** : Prevents failure cascades and improves the user experience with degraded but functional responses

### Circuit Breaking at the Gateway Level
- **Architecture** : Implementing the circuit breaker directly in **Spring Cloud Gateway**
- **Advantage** : Global protection for all upstream microservices, before requests even reach the backend services
- **Configuration** : Using Gateway filters with Resilience4j to monitor the health of each route
- **Example** : If the Orders service is down, the gateway immediately detects the problem and returns an error response without hitting the failing service

---

## 2. Retry Patterns

### Retry Pattern in Spring Boot Services
- **Implementation** : Configuring **@Retry** with Resilience4j on critical methods
- **Concrete example** :
  - Accounts service calling an external banking API : 3 attempts with exponential backoff (1s, 2s, 4s)
  - Configuration : `@Retry(name = "bankApi", fallbackMethod = "getBankDataFallback")`
  - Handling recoverable vs non-recoverable exceptions (retry only on timeout/network errors)
- **Impact** : Resolves 80% of transient network errors without manual intervention

### Retry Pattern in API Gateway
- **Strategy** : Automatic retry at the **Spring Cloud Gateway** level for 5xx errors and timeouts
- **Configuration** : Defining retry policies per route in the gateway
- **Example** :
  ```yaml
  - id: account-service
    uri: lb://ACCOUNTS
    filters:
      - name: Retry
        args:
          retries: 3
          statuses: BAD_GATEWAY, SERVICE_UNAVAILABLE
  ```
- **Benefit** : Completely transparent for the client, centralized retry management

---

## 3. Rate Limiting & Throttling

### RateLimiting Pattern
- **Goal** : Protect services from overload and abuse
- **Implementation** : Using **Resilience4j RateLimiter** at the application level
- **Example on Accounts service** :
  - Limit of 100 requests per second per user
  - Configuration : `@RateLimiter(name = "accountsApi")`
  - HTTP 429 (Too Many Requests) response when the limit is reached

### Redis RateLimiter with Spring Cloud Gateway
- **Distributed architecture** : Using **Redis** as a backend for rate limiting shared across gateway instances
- **Advantage** : Consistent rate limiting even with multiple gateway instances (load balancing)
- **Configuration example** :
  ```yaml
  filters:
    - name: RequestRateLimiter
      args:
        redis-rate-limiter.replenishRate: 10
        redis-rate-limiter.burstCapacity: 20
        key-resolver: "#{@userKeyResolver}"
  ```
- **Use case** : Public API protection with 1000 req/min per API key, synchronized via Redis Cluster

---

## 4. Advanced Resilience Patterns

### Bulkhead Pattern
- **Principle** : Resource isolation to prevent a failing service from exhausting all system resources
- **Implementation** : Configuring separate thread pools per called service
- **Example** :
  - Dedicated thread pool of 10 threads for the Payments service
  - Separate thread pool of 20 threads for the Notifications service
  - If Payments saturates its pool, Notifications continues to function normally
- **Configuration** : `@Bulkhead(name = "paymentsService", type = Bulkhead.Type.THREADPOOL)`

### Aspect Order of Resiliency Patterns
- **Challenge** : Execution order of Resilience4j annotations
- **Solution** : Explicit configuration of the aspect order
- **Recommended order** :
  1. **Retry** (first - to quickly retry)
  2. **CircuitBreaker** (detects failure patterns)
  3. **RateLimiter** (controls throughput)
  4. **Bulkhead** (isolates resources)
  5. **TimeLimiter** (global timeout)
- **Configuration** : Using `@Order` or `resilience4j.{pattern}.aspects-order` properties

---

## 5. Global Architecture - Making Microservices Resilient

### Multi-Layer Approach
**Gateway Level** :
- Global circuit breaker on all routes
- Automatic retry on transient errors
- Distributed rate limiting with Redis
- Timeout configured per service

**Service Level (Client)** :
- Specific circuit breaker per external dependency
- Retry with exponential backoff
- Bulkhead for isolation of external calls
- Fallback methods with cached data

**Complete architecture example** :
- Gateway → Circuit Breaker + Retry + RateLimiter (Redis)
- Accounts Service → Circuit Breaker (to Loans service) + Retry + Bulkhead
- Loans Service → Local RateLimiter + Circuit Breaker (to external database)

### Monitoring & Observability
- Integration with **Actuator** to expose Resilience4j metrics
- **Grafana** dashboards to visualize circuit breaker states
- Alerts on failure rate and circuit opening

---

## Results & Impact

✅ **Availability** : Increased uptime from 99.5% to 99.95%
✅ **Performance** : 60% reduction in cascade errors during traffic spikes
✅ **Maintenance** : Proactive detection of failing services before user impact
✅ **Scalability** : Architecture capable of handling 10x traffic through intelligent rate limiting

---

## Mastered Technologies
- **Spring Boot 3.x** / **Spring Cloud Gateway**
- **Resilience4j** (CircuitBreaker, Retry, RateLimiter, Bulkhead, TimeLimiter)
- **Redis** (distributed rate limiting, cache)
- **Micrometer** / **Prometheus** / **Grafana** (observability)


# Technical Q&A - Microservices Resilience

## 1. Circuit Breaker Pattern

### Q1: What is a Circuit Breaker and why is it important?
**A:** The **Circuit Breaker** is a pattern that prevents repeated calls to a failing service. It works like an electrical breaker with 3 states :
- **CLOSED** : Everything works normally, requests pass through
- **OPEN** : The service is detected as failing, requests are immediately rejected without calling the service
- **HALF_OPEN** : Test phase to check if the service has recovered

It is crucial for preventing **cascading failures** and protecting system resources.

### Q2: How do you configure a Circuit Breaker with Resilience4j in Spring Boot?
**A:**
```java
// 1. Maven dependency
<dependency>
    <groupId>io.github.resilience4j</groupId>
    <artifactId>resilience4j-spring-boot3</artifactId>
</dependency>

// 2. Configuration in application.yml
resilience4j:
  circuitbreaker:
    instances:
      paymentService:
        failureRateThreshold: 50
        waitDurationInOpenState: 10s
        slidingWindowSize: 10
        minimumNumberOfCalls: 5

// 3. Usage in code
@Service
public class PaymentService {
    
    @CircuitBreaker(name = "paymentService", fallbackMethod = "paymentFallback")
    public PaymentResponse processPayment(PaymentRequest request) {
        // Call to external service
        return externalPaymentApi.process(request);
    }
    
    private PaymentResponse paymentFallback(PaymentRequest request, Exception ex) {
        // Degraded response
        return PaymentResponse.builder()
            .status("PENDING")
            .message("Service temporarily unavailable")
            .build();
    }
}
```

### Q3: What is the difference between Circuit Breaker at Gateway level vs Service level?
**A:**

**At Gateway level (Spring Cloud Gateway)** :
- ✅ Global upstream protection for all microservices
- ✅ Single centralized configuration point
- ✅ Prevents traffic from reaching down services
- ❌ Less granularity (per route only)

**At Service level (Client)** :
- ✅ Fine granularity (per external dependency)
- ✅ Business-context-specific fallback
- ✅ Can have different strategies per endpoint
- ❌ Distributed configuration in each service

**Best practice** : Both together! Gateway as the first line of defense, Service for specific business logic.

---

## 2. Retry Pattern

### Q4: Explain the Retry Pattern and when to use it
**A:** The **Retry Pattern** automatically attempts to re-execute a failed operation.

**When to use it** :
- ✅ Transient errors (network timeout, service temporarily unavailable)
- ✅ 5xx errors (500, 502, 503, 504)
- ❌ DO NOT use on business errors (400, 401, 403, 404, 409)
- ❌ DO NOT use on non-idempotent operations without precaution

**Recommended configuration** :
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

### Q5: How do you implement Retry at the Spring Cloud Gateway level?
**A:**
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

**Warning** : Only retry **idempotent** methods (GET, PUT, DELETE) or POST with an **idempotency key**.

### Q6: What is the difference between Retry and Circuit Breaker?
**A:**

| Aspect | Retry | Circuit Breaker |
|--------|-------|-----------------|
| **Goal** | Resolve transient errors | Prevent repeated calls to a failing service |
| **When** | Isolated failure | Repeated failure pattern |
| **Action** | Retries immediately/with delay | Blocks calls for a period of time |
| **Duration** | A few seconds max | Several seconds/minutes |

**Synergy** : Retry executes FIRST, then if failures persist, Circuit Breaker opens.

---

## 3. Rate Limiting

### Q7: Why implement Rate Limiting and how to do it with Redis?
**A:**

**Why** :
- Protect against abuse (DDoS, scraping)
- Guarantee fair quality of service between users
- Avoid backend resource overload
- Respect third-party API quotas

**Implementation with Redis + Spring Cloud Gateway** :
```java
// 1. Redis configuration
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

// 2. Custom KeyResolver (by user ID or API key)
@Bean
public KeyResolver userKeyResolver() {
    return exchange -> Mono.just(
        exchange.getRequest()
            .getHeaders()
            .getFirst("X-API-KEY")
    );
}

// 3. Gateway configuration
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
                redis-rate-limiter.burstCapacity: 20    # max burst 20
                redis-rate-limiter.requestedTokens: 1
                key-resolver: "#{@userKeyResolver}"
  redis:
    host: localhost
    port: 6379
```

### Q8: How do you implement Rate Limiting at a Spring Boot service level?
**A:**
```java
// Configuration
resilience4j:
  ratelimiter:
    instances:
      accountsApi:
        limitForPeriod: 100          # 100 requests
        limitRefreshPeriod: 1s       # per second
        timeoutDuration: 0s          # immediate failure if limit reached

// Usage
@RestController
@RequestMapping("/api/accounts")
public class AccountController {
    
    @GetMapping("/{id}")
    @RateLimiter(name = "accountsApi")
    public ResponseEntity<Account> getAccount(@PathVariable Long id) {
        return ResponseEntity.ok(accountService.findById(id));
    }
}

// Exception handling
@ExceptionHandler(RequestNotPermitted.class)
public ResponseEntity<ErrorResponse> handleRateLimiting(RequestNotPermitted ex) {
    return ResponseEntity
        .status(HttpStatus.TOO_MANY_REQUESTS)
        .body(new ErrorResponse("Rate limit exceeded. Try again later."));
}
```

---

## 4. Bulkhead Pattern

### Q9: What is the Bulkhead Pattern and why use it?
**A:** The **Bulkhead Pattern** is inspired by the watertight compartments of a ship. It isolates resources (threads, connections) so that a failure in one area does not affect others.

**Concrete example** :
Imagine a service with 3 external dependencies :
- Payments service (slow, may timeout)
- Notifications service (fast)
- Fraud Detection service (critical)

**Without Bulkhead** : If Payments saturates all threads (100), Notifications and Fraud Detection are blocked.

**With Bulkhead** :
- Dedicated pool of 20 threads for Payments
- Dedicated pool of 10 threads for Notifications
- Dedicated pool of 30 threads for Fraud Detection
- Even if Payments saturates its 20 threads, the other services continue to function.

### Q10: How do you configure the Bulkhead Pattern with Resilience4j?
**A:**
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
    
    // Semaphore bulkhead (limits concurrent calls)
    @Bulkhead(name = "paymentService", type = Bulkhead.Type.SEMAPHORE)
    public PaymentResponse processPaymentSync(Order order) {
        return paymentClient.process(order);
    }
    
    // Thread pool bulkhead (isolates in a dedicated thread pool)
    @Bulkhead(name = "paymentService", type = Bulkhead.Type.THREADPOOL)
    public CompletableFuture<PaymentResponse> processPaymentAsync(Order order) {
        return CompletableFuture.supplyAsync(
            () -> paymentClient.process(order)
        );
    }
}
```

---

## 5. Aspect Order & Global Architecture

### Q11: What is the optimal execution order for resilience patterns?
**A:** The order is CRITICAL as it determines how the patterns interact.

**Recommended order** :
1. **Retry** (first) - attempts to quickly resolve transient errors
2. **CircuitBreaker** - if retries fail, detects the pattern and opens the circuit
3. **RateLimiter** - limits throughput before reaching resources
4. **TimeLimiter** - global timeout to avoid lingering calls
5. **Bulkhead** (last) - isolates resources at the lowest level

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

**Why this order** :
- Retry first = resolves 80% of errors without triggering other patterns
- CircuitBreaker after = detects real systemic failures
- RateLimiter after = retries are not counted against the limit
- Bulkhead last = ultimate protection at the resource level

### Q12: How do you monitor and observe all these patterns in production?
**A:**
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

// 2. Prometheus metrics exposed
- resilience4j_circuitbreaker_state (0=closed, 1=open, 2=half_open)
- resilience4j_circuitbreaker_failure_rate
- resilience4j_retry_calls_total
- resilience4j_ratelimiter_available_permissions
- resilience4j_bulkhead_available_concurrent_calls

// 3. Grafana dashboard with alerts
- Alert if circuit breaker open > 1 minute
- Alert if retry rate > 30%
- Alert if rate limiter rejection > 5%

// 4. Custom events
@Bean
public RegistryEventConsumer<CircuitBreaker> circuitBreakerEventConsumer() {
    return CircuitBreaker.EventConsumer
        .onStateTransition(event -> {
            log.warn("Circuit Breaker {} changed from {} to {}", 
                event.getCircuitBreakerName(),
                event.getStateTransition().getFromState(),
                event.getStateTransition().getToState()
            );
            // Send Slack/PagerDuty notification
        });
}
```

### Q13: How do you test these resilience patterns?
**A:**
```java
@SpringBootTest
class ResiliencePatternTest {
    
    @Autowired
    private CircuitBreakerRegistry circuitBreakerRegistry;
    
    @MockBean
    private ExternalApiClient externalApiClient;
    
    @Test
    void testCircuitBreakerOpensAfterFailures() {
        // Simulates 10 consecutive failures
        when(externalApiClient.call())
            .thenThrow(new RuntimeException("Service down"));
        
        CircuitBreaker cb = circuitBreakerRegistry
            .circuitBreaker("externalApi");
        
        // Trigger the calls
        for (int i = 0; i < 10; i++) {
            try {
                service.callExternalApi();
            } catch (Exception e) {
                // Expected
            }
        }
        
        // Verify the circuit is open
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

// Integration test with Testcontainers
@SpringBootTest
@Testcontainers
class RateLimiterIntegrationTest {
    
    @Container
    static RedisContainer redis = new RedisContainer("redis:7-alpine");
    
    @Test
    void testRedisRateLimiter() {
        // Sends 25 requests (limit = 20)
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

## Bonus Questions - Real-World Scenarios

### Q14: A third-party service you call has a 99.9% SLA but unpredictable latency spikes. How do you handle that?
**A:** Multi-layer strategy :

```java
@Service
public class ThirdPartyService {
    
    // 1. TimeLimiter to avoid overly long calls
    @TimeLimiter(name = "thirdParty", fallbackMethod = "fallbackResponse")
    // 2. CircuitBreaker to detect degradations
    @CircuitBreaker(name = "thirdParty", fallbackMethod = "fallbackResponse")
    // 3. Retry for occasional timeouts
    @Retry(name = "thirdParty", fallbackMethod = "fallbackResponse")
    // 4. Bulkhead to avoid saturating our resources
    @Bulkhead(name = "thirdParty", type = Bulkhead.Type.THREADPOOL)
    public CompletableFuture<Response> callThirdParty(Request req) {
        return thirdPartyClient.call(req);
    }
    
    private CompletableFuture<Response> fallbackResponse(
        Request req, Exception ex) {
        // Fallback strategy:
        // 1. Look in cache
        Optional<Response> cached = cacheService.get(req.getId());
        if (cached.isPresent()) {
            return CompletableFuture.completedFuture(cached.get());
        }
        
        // 2. Return a degraded response
        return CompletableFuture.completedFuture(
            Response.degraded("Service temporarily slow")
        );
    }
}

// Configuration
resilience4j:
  timelimiter:
    instances:
      thirdParty:
        timeoutDuration: 3s  # SLA = 99.9% under 3s
  circuitbreaker:
    instances:
      thirdParty:
        slowCallRateThreshold: 60  # Opens if 60% of calls > 3s
        slowCallDurationThreshold: 3s
```

### Q15: How do you handle an architecture where some services are more critical than others?
**A:** **Resource priority** approach :

```yaml
# Critical services: more resources, less tolerance
resilience4j:
  bulkhead:
    instances:
      fraudDetection:  # CRITICAL service
        maxConcurrentCalls: 50
        maxWaitDuration: 0ms  # Immediate failure if saturated
  circuitbreaker:
    instances:
      fraudDetection:
        failureRateThreshold: 25  # Very strict
        waitDurationInOpenState: 30s
        
  # Non-critical services: fewer resources, more tolerance
  bulkhead:
    instances:
      analytics:  # NON-CRITICAL service
        maxConcurrentCalls: 10
        maxWaitDuration: 500ms
  circuitbreaker:
    instances:
      analytics:
        failureRateThreshold: 75  # More tolerant
        waitDurationInOpenState: 60s
```

```java
@Service
public class OrderService {
    
    // Critical call: fail if problem
    @CircuitBreaker(name = "fraudDetection")
    @Bulkhead(name = "fraudDetection")
    public OrderResponse processOrder(Order order) {
        FraudCheck check = fraudService.check(order);
        if (!check.isPassed()) {
            throw new FraudException();
        }
        return orderRepository.save(order);
    }
    
    // Non-critical call: continue even if failure
    @CircuitBreaker(name = "analytics", fallbackMethod = "skipAnalytics")
    @Bulkhead(name = "analytics")
    public void trackOrder(Order order) {
        analyticsService.track(order);
    }
    
    private void skipAnalytics(Order order, Exception ex) {
        log.warn("Analytics unavailable, skipping: {}", ex.getMessage());
        // Business continues, just no tracking
    }
}
```

This approach allows **graceful degradation** of non-critical features while protecting essential operations.
