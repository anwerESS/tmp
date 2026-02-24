## [**..**](./00_index.md)

## **API Gateway & Spring Cloud Gateway**

During my microservices projects with Spring Boot, I implemented API Gateways to centrally manage routing, security, and cross-cutting concerns.

---

### **1. What is an API Gateway?**

An **API Gateway** is a single entry point that acts as a **reverse proxy** between clients and backend microservices. It intercepts all requests to apply cross-cutting processing before routing to the appropriate service.

**Problems without a Gateway:**
```
Client Mobile → Service A (Port 8081)
Client Mobile → Service B (Port 8082)
Client Mobile → Service C (Port 8083)

❌ Client doit connaître tous les endpoints
❌ Gestion de la sécurité dupliquée dans chaque service
❌ CORS configuré partout
❌ Rate limiting complexe
❌ Pas de monitoring centralisé
```

**With a Gateway:**
```
Client → API Gateway (Port 8080) → Route vers services appropriés
                ↓
        - Authentication/Authorization
        - Rate Limiting
        - Load Balancing
        - Logging/Monitoring
        - Request/Response transformation
```

---

### **2. Gateway Patterns (Design Patterns)**

I have worked with three main API Gateway patterns:

#### **Pattern 1: Single Entry Point**

**Concept:** A single Gateway for the ENTIRE application; all clients go through it.

```
                    ┌─────────────────┐
    Web App ───────►│                 │
    Mobile App ────►│   API Gateway   │──► Service A
    Third Party ───►│  (Port 8080)    │──► Service B
                    │                 │──► Service C
                    └─────────────────┘
```

**Advantages:**
- ✅ Simple architecture
- ✅ Centralized configuration
- ✅ Unified monitoring

**Disadvantages:**
- ❌ Single Point of Failure
- ❌ Can become a bottleneck
- ❌ All clients get the same processing

**Use case:** Medium-sized applications with similar clients.

---

#### **Pattern 2: Backend for Frontend (BFF)**

**Concept:** A specific Gateway for **each type of client** (Web, Mobile, IoT).

```
Web App ──────► Web Gateway ──────► Service A, B, C
                (Port 8081)          (Format optimisé Web)

Mobile App ───► Mobile Gateway ────► Service A, B, C
                (Port 8082)          (Format optimisé Mobile)

IoT Devices ──► IoT Gateway ───────► Service A, B, C
                (Port 8083)          (Format optimisé IoT)
```

**Advantages:**
- ✅ **Optimized responses** per client type (Mobile = lightweight data, Web = full data)
- ✅ **Platform-specific logic**
- ✅ **Independent evolution**: change Mobile Gateway without affecting Web

**Concrete example:**
```java
// Mobile Gateway - Données simplifiées
GET /api/products → {id, name, price, thumbnail}

// Web Gateway - Données complètes
GET /api/products → {id, name, price, description, images[], reviews[], stock}
```

**Use case:** Applications with very different clients (rich Web vs lightweight Mobile).

---

#### **Pattern 3: Aggregation Gateway**

**Concept:** The Gateway **aggregates responses** from multiple microservices into a single response for the client.

**Problem without aggregation:**
```
Client → GET /users/123
Client → GET /users/123/orders
Client → GET /users/123/payments
→ 3 appels réseau, lenteur
```

**With Aggregation Gateway:**
```
Client → GET /api/users/123/full-profile

Gateway:
  1. Call User Service → user data
  2. Call Order Service → orders data
  3. Call Payment Service → payments data
  4. Aggregate → Retourne tout en une réponse

Response:
{
  "user": {...},
  "orders": [...],
  "payments": [...]
}
```

**Implementation with Spring Cloud Gateway:**
```java
@Configuration
public class AggregationRouteConfig {
    
    @Bean
    public RouteLocator aggregationRoutes(RouteLocatorBuilder builder) {
        return builder.routes()
            .route("user-full-profile", r -> r
                .path("/api/users/{userId}/full-profile")
                .filters(f -> f
                    .filter(new UserProfileAggregationFilter())
                )
                .uri("lb://USER-SERVICE")
            )
            .build();
    }
}

// Custom Aggregation Filter
public class UserProfileAggregationFilter implements GatewayFilter {
    
    @Autowired
    private WebClient.Builder webClientBuilder;
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        String userId = exchange.getRequest().getURI().getPath().split("/")[3];
        
        // Appels parallèles vers plusieurs services
        Mono<User> userMono = webClientBuilder.build()
            .get()
            .uri("lb://USER-SERVICE/api/users/" + userId)
            .retrieve()
            .bodyToMono(User.class);
        
        Mono<List<Order>> ordersMono = webClientBuilder.build()
            .get()
            .uri("lb://ORDER-SERVICE/api/orders?userId=" + userId)
            .retrieve()
            .bodyToFlux(Order.class)
            .collectList();
        
        Mono<PaymentInfo> paymentMono = webClientBuilder.build()
            .get()
            .uri("lb://PAYMENT-SERVICE/api/payments/user/" + userId)
            .retrieve()
            .bodyToMono(PaymentInfo.class);
        
        // Agréger les réponses
        return Mono.zip(userMono, ordersMono, paymentMono)
            .flatMap(tuple -> {
                UserFullProfile profile = new UserFullProfile();
                profile.setUser(tuple.getT1());
                profile.setOrders(tuple.getT2());
                profile.setPaymentInfo(tuple.getT3());
                
                // Écrire la réponse agrégée
                exchange.getResponse().getHeaders().setContentType(MediaType.APPLICATION_JSON);
                DataBuffer buffer = exchange.getResponse().bufferFactory()
                    .wrap(new ObjectMapper().writeValueAsBytes(profile));
                
                return exchange.getResponse().writeWith(Mono.just(buffer));
            });
    }
}
```

**Advantages:**
- ✅ **Reduction of network calls** on the client side (3 calls → 1)
- ✅ **Improved performance** (parallel calls on the gateway side)
- ✅ **Client-side simplicity**

**Disadvantages:**
- ❌ Business logic in the Gateway (violation of responsibilities)
- ❌ Increased coupling between services

**Use case:** Mobile apps with slow connections, to reduce latency and battery consumption.

---

### **3. Spring Cloud Gateway - Architecture & Concepts**

**Spring Cloud Gateway** is the modern API Gateway solution in the Spring ecosystem (replaces Netflix Zuul).

**Dependency:**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-gateway</artifactId>
</dependency>
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-netflix-eureka-client</artifactId>
</dependency>
```

**Spring Cloud Gateway Architecture:**
```
Client Request
    ↓
[Gateway Handler Mapping] → Trouve la route correspondante
    ↓
[Gateway Web Handler] → Applique les filtres
    ↓
[Pre-filters] → Traitements AVANT routing
    ↓
[Proxy Filter] → Envoie requête au microservice
    ↓
Microservice (traitement)
    ↓
[Post-filters] → Traitements APRÈS réponse
    ↓
Client Response
```

**Key concepts:**

**1. Route:**
Configuration defining:
- **ID**: Unique route identifier
- **URI**: Destination (http://... or lb://SERVICE-NAME)
- **Predicates**: Conditions to match the request
- **Filters**: Processing to apply

**2. Predicates:**
Conditions to trigger a route:
- Path : `/api/users/**`
- Method : `GET`, `POST`
- Header : `X-Request-Id`
- Query : `?version=v1`
- Host : `api.myapp.com`
- DateTime: Activate route at certain times

**3. Filters:**
Pre/post routing processing:
- **Pre-filters**: AddRequestHeader, RewritePath, RateLimiter
- **Post-filters**: AddResponseHeader, ModifyResponseBody

---

### **4. Route Configuration: YAML vs Java Code**

#### **YAML Configuration (Declarative) - Recommended**

```yaml
spring:
  application:
    name: gateway-server
  cloud:
    gateway:
      discovery:
        locator:
          enabled: true  # Auto-découverte services depuis Eureka
          lower-case-service-id: true  # user-service au lieu de USER-SERVICE
      
      routes:
        # Route 1: User Service
        - id: user-service
          uri: lb://USER-SERVICE  # lb = load balanced via Eureka
          predicates:
            - Path=/api/users/**
          filters:
            - RewritePath=/api/users/(?<segment>.*), /${segment}
            - AddRequestHeader=X-Gateway, Spring-Cloud-Gateway
            - AddResponseHeader=X-Response-Time, ${responseTime}
        
        # Route 2: Order Service avec méthode spécifique
        - id: order-service-get
          uri: lb://ORDER-SERVICE
          predicates:
            - Path=/api/orders/**
            - Method=GET
          filters:
            - RewritePath=/api/orders/(?<segment>.*), /${segment}
        
        # Route 3: Order Service POST avec rate limiting
        - id: order-service-post
          uri: lb://ORDER-SERVICE
          predicates:
            - Path=/api/orders/**
            - Method=POST
          filters:
            - name: RequestRateLimiter
              args:
                redis-rate-limiter.replenishRate: 10  # 10 requêtes/sec
                redis-rate-limiter.burstCapacity: 20
        
        # Route 4: Product Service avec Header condition
        - id: product-service-v2
          uri: lb://PRODUCT-SERVICE
          predicates:
            - Path=/api/products/**
            - Header=X-API-Version, v2
          filters:
            - RewritePath=/api/products/(?<segment>.*), /v2/${segment}
        
        # Route 5: Direct HTTP (pas de load balancing)
        - id: external-api
          uri: http://external-api.com:8080
          predicates:
            - Path=/external/**
          filters:
            - RewritePath=/external/(?<segment>.*), /${segment}

# Eureka Client Configuration
eureka:
  client:
    service-url:
      defaultZone: http://localhost:8761/eureka/
    fetch-registry: true
    register-with-eureka: true
  instance:
    prefer-ip-address: true

server:
  port: 8080
```

**YAML Advantages:**
- ✅ Simple and readable
- ✅ No need to recompile to change routes
- ✅ Externalized configuration (Config Server)

---

#### **Java Code Configuration (Programmatic)**

```java
@Configuration
public class GatewayConfig {
    
    @Bean
    public RouteLocator customRoutes(RouteLocatorBuilder builder) {
        return builder.routes()
            // Route 1: User Service
            .route("user-service", r -> r
                .path("/api/users/**")
                .filters(f -> f
                    .rewritePath("/api/users/(?<segment>.*)", "/${segment}")
                    .addRequestHeader("X-Gateway", "Spring-Cloud-Gateway")
                    .addResponseHeader("X-Powered-By", "Spring Boot")
                )
                .uri("lb://USER-SERVICE")
            )
            
            // Route 2: Order Service avec predicates multiples
            .route("order-service", r -> r
                .path("/api/orders/**")
                .and()
                .method(HttpMethod.GET)
                .and()
                .header("X-Request-Type", "standard")
                .filters(f -> f
                    .rewritePath("/api/orders/(?<segment>.*)", "/${segment}")
                    .circuitBreaker(config -> config
                        .setName("orderServiceCircuitBreaker")
                        .setFallbackUri("forward:/fallback/orders")
                    )
                )
                .uri("lb://ORDER-SERVICE")
            )
            
            // Route 3: Product Service avec custom filter
            .route("product-service", r -> r
                .path("/api/products/**")
                .filters(f -> f
                    .filter(new AuthenticationFilter())
                    .filter(new LoggingFilter())
                    .rewritePath("/api/products/(?<segment>.*)", "/${segment}")
                )
                .uri("lb://PRODUCT-SERVICE")
            )
            
            // Route 4: Retry avec backoff
            .route("payment-service", r -> r
                .path("/api/payments/**")
                .filters(f -> f
                    .retry(config -> config
                        .setRetries(3)
                        .setStatuses(HttpStatus.INTERNAL_SERVER_ERROR)
                        .setBackoff(Duration.ofSeconds(1), Duration.ofSeconds(5), 2, true)
                    )
                )
                .uri("lb://PAYMENT-SERVICE")
            )
            
            .build();
    }
}
```

**Java Code Advantages:**
- ✅ Complex conditional logic
- ✅ Full access to the Spring API
- ✅ Custom filters easily integrated
- ✅ Type-safe

**My recommendation:** 
- **YAML** for simple routes and standard configuration
- **Java Code** for complex logic and custom filters

---

### **5. Gateway Filters - Types and Implementation**

#### **Built-in Filters (Predefined Filters)**

**A. Request Transformation:**
```yaml
filters:
  # Ajouter header à la requête
  - AddRequestHeader=X-User-Id, ${userId}
  - AddRequestParameter=source, gateway
  
  # Modifier path
  - RewritePath=/api/(?<segment>.*), /${segment}
  - PrefixPath=/v1
  - StripPrefix=1  # Enlever le premier segment du path
  
  # Set request path
  - SetPath=/api/v2/{segment}
```

**B. Response Transformation:**
```yaml
filters:
  # Ajouter header à la réponse
  - AddResponseHeader=X-Response-Time, 150ms
  - AddResponseHeader=X-Gateway-Version, 1.0
  
  # Modifier response body
  - name: ModifyResponseBody
    args:
      rewriteFunction: com.example.ResponseRewriter
  
  # Déduplication
  - DedupeResponseHeader=Access-Control-Allow-Origin
```

**C. Rate Limiting (with Redis):**
```yaml
filters:
  - name: RequestRateLimiter
    args:
      redis-rate-limiter.replenishRate: 10  # Tokens ajoutés/seconde
      redis-rate-limiter.burstCapacity: 20   # Capacité max du bucket
      redis-rate-limiter.requestedTokens: 1  # Tokens par requête
      key-resolver: "#{@userKeyResolver}"    # Bean Spring pour résoudre la clé
```

```java
@Bean
public KeyResolver userKeyResolver() {
    return exchange -> Mono.just(
        exchange.getRequest().getHeaders().getFirst("X-User-Id")
    );
}
```

**D. Circuit Breaker (Resilience4j):**
```yaml
filters:
  - name: CircuitBreaker
    args:
      name: userServiceCircuitBreaker
      fallbackUri: forward:/fallback/users
```

```java
@RestController
public class FallbackController {
    
    @GetMapping("/fallback/users")
    public ResponseEntity<String> userFallback() {
        return ResponseEntity.ok("User service is temporarily unavailable");
    }
}
```

**E. Retry Logic:**
```yaml
filters:
  - name: Retry
    args:
      retries: 3
      statuses: BAD_GATEWAY, INTERNAL_SERVER_ERROR
      methods: GET, POST
      backoff:
        firstBackoff: 50ms
        maxBackoff: 500ms
        factor: 2
        basedOnPreviousValue: false
```

---

#### **Custom Filters (Personalized Filters)**

**1. Global Filter (applied to all routes):**

```java
@Component
public class GlobalLoggingFilter implements GlobalFilter, Ordered {
    
    private static final Logger logger = LoggerFactory.getLogger(GlobalLoggingFilter.class);
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        // PRE-FILTER: Avant routing
        ServerHttpRequest request = exchange.getRequest();
        logger.info("Incoming request: {} {}", request.getMethod(), request.getURI());
        logger.info("Headers: {}", request.getHeaders());
        
        // Ajouter timestamp pour calculer temps de réponse
        exchange.getAttributes().put("startTime", System.currentTimeMillis());
        
        // POST-FILTER: Après réponse
        return chain.filter(exchange).then(Mono.fromRunnable(() -> {
            Long startTime = exchange.getAttribute("startTime");
            Long duration = System.currentTimeMillis() - startTime;
            
            ServerHttpResponse response = exchange.getResponse();
            logger.info("Response status: {} - Duration: {}ms", 
                response.getStatusCode(), duration);
            
            // Ajouter header avec temps de réponse
            response.getHeaders().add("X-Response-Time", duration + "ms");
        }));
    }
    
    @Override
    public int getOrder() {
        return -1; // Haute priorité (s'exécute en premier)
    }
}
```

**2. GatewayFilter (applied to specific routes):**

```java
@Component
public class AuthenticationFilter implements GatewayFilter, Ordered {
    
    @Autowired
    private JwtUtil jwtUtil;
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = exchange.getRequest();
        
        // Vérifier si route nécessite authentication
        if (!request.getHeaders().containsKey(HttpHeaders.AUTHORIZATION)) {
            return onError(exchange, "Missing authorization header", HttpStatus.UNAUTHORIZED);
        }
        
        String authHeader = request.getHeaders().getFirst(HttpHeaders.AUTHORIZATION);
        
        if (authHeader == null || !authHeader.startsWith("Bearer ")) {
            return onError(exchange, "Invalid authorization header", HttpStatus.UNAUTHORIZED);
        }
        
        String token = authHeader.substring(7);
        
        try {
            // Valider JWT
            if (!jwtUtil.validateToken(token)) {
                return onError(exchange, "Invalid or expired token", HttpStatus.UNAUTHORIZED);
            }
            
            // Extraire user info et ajouter aux headers pour downstream services
            String username = jwtUtil.extractUsername(token);
            String userId = jwtUtil.extractUserId(token);
            
            ServerHttpRequest modifiedRequest = exchange.getRequest().mutate()
                .header("X-User-Id", userId)
                .header("X-Username", username)
                .build();
            
            return chain.filter(exchange.mutate().request(modifiedRequest).build());
            
        } catch (Exception e) {
            return onError(exchange, "Token validation failed", HttpStatus.UNAUTHORIZED);
        }
    }
    
    private Mono<Void> onError(ServerWebExchange exchange, String message, HttpStatus status) {
        ServerHttpResponse response = exchange.getResponse();
        response.setStatusCode(status);
        response.getHeaders().setContentType(MediaType.APPLICATION_JSON);
        
        String errorBody = String.format("{\"error\": \"%s\"}", message);
        DataBuffer buffer = response.bufferFactory().wrap(errorBody.getBytes());
        
        return response.writeWith(Mono.just(buffer));
    }
    
    @Override
    public int getOrder() {
        return 0;
    }
}
```

**3. GatewayFilterFactory (for YAML configuration):**

```java
@Component
public class CustomHeaderGatewayFilterFactory 
        extends AbstractGatewayFilterFactory<CustomHeaderGatewayFilterFactory.Config> {
    
    public CustomHeaderGatewayFilterFactory() {
        super(Config.class);
    }
    
    @Override
    public GatewayFilter apply(Config config) {
        return (exchange, chain) -> {
            // Ajouter header custom
            ServerHttpRequest request = exchange.getRequest().mutate()
                .header(config.getHeaderName(), config.getHeaderValue())
                .build();
            
            return chain.filter(exchange.mutate().request(request).build());
        };
    }
    
    public static class Config {
        private String headerName;
        private String headerValue;
        
        // Getters & Setters
    }
}
```

**Usage in YAML:**
```yaml
filters:
  - name: CustomHeader
    args:
      headerName: X-Custom-Header
      headerValue: MyValue
```

---

### **6. URI Types: HTTP vs Load Balanced (lb://)**

**Direct HTTP (without load balancing):**
```yaml
routes:
  - id: external-api
    uri: http://external-api.com:8080  # Direct HTTP call
    predicates:
      - Path=/external/**
```

**Advantages:**
- ✅ Simple for external services
- ✅ No need for service discovery

**Disadvantages:**
- ❌ No load balancing
- ❌ No automatic failover
- ❌ Hardcoded URL

---

**Load Balanced via Eureka (lb://):**
```yaml
routes:
  - id: user-service
    uri: lb://USER-SERVICE  # Service name depuis Eureka
    predicates:
      - Path=/api/users/**
```

**How it works:**
```
1. Gateway reçoit requête → /api/users/123
2. Match route avec uri: lb://USER-SERVICE
3. Gateway contacte Eureka → "Donne-moi les instances de USER-SERVICE"
4. Eureka répond → [instance1:8081, instance2:8082, instance3:8083]
5. LoadBalancer (Round Robin) → Choisit instance2:8082
6. Gateway forward requête → http://instance2:8082/users/123
```

**Advantages:**
- ✅ **Automatic load balancing** (Round Robin by default)
- ✅ **Dynamic service discovery**
- ✅ **Failover**: if instance is down, uses another
- ✅ **Transparent scaling**: add instances without Gateway config change

**Eureka Configuration in Gateway:**
```yaml
eureka:
  client:
    service-url:
      defaultZone: http://localhost:8761/eureka/
    fetch-registry: true      # Récupérer liste des services
    register-with-eureka: true # S'enregistrer comme service
  instance:
    prefer-ip-address: true
    instance-id: ${spring.application.name}:${random.value}
```

---

### **7. RewritePath Filter - Path Rewriting**

**Problem:** The client calls `/api/users/123` but the User microservice exposes `/users/123` (without `/api`).

**Solution: RewritePath Filter**

#### **With YAML:**

```yaml
routes:
  # Exemple 1: Enlever préfixe /api
  - id: user-service
    uri: lb://USER-SERVICE
    predicates:
      - Path=/api/users/**
    filters:
      - RewritePath=/api/users/(?<segment>.*), /${segment}
      # Transforme: /api/users/123 → /users/123

  # Exemple 2: Ajouter version au path
  - id: product-service-v2
    uri: lb://PRODUCT-SERVICE
    predicates:
      - Path=/api/products/**
    filters:
      - RewritePath=/api/products/(?<segment>.*), /v2/products/${segment}
      # Transforme: /api/products/123 → /v2/products/123

  # Exemple 3: Combiner avec StripPrefix
  - id: order-service
    uri: lb://ORDER-SERVICE
    predicates:
      - Path=/gateway/api/orders/**
    filters:
      - StripPrefix=2  # Enlève 2 premiers segments
      # Transforme: /gateway/api/orders/123 → /orders/123

  # Exemple 4: Remplacer complètement le path
  - id: legacy-api
    uri: http://legacy-system.com
    predicates:
      - Path=/new/api/users/**
    filters:
      - RewritePath=/new/api/users/(?<id>.*), /legacy/user?userId=${id}
      # Transforme: /new/api/users/123 → /legacy/user?userId=123
```

**RewritePath Syntax:**
```
RewritePath=<regex>, <replacement>
```
- `(?<name>.*)` : Named capture group
- `${name}`: Reference to the captured group

---

#### **With Java Code:**

```java
@Bean
public RouteLocator rewriteRoutes(RouteLocatorBuilder builder) {
    return builder.routes()
        // Enlever préfixe /api
        .route("user-service", r -> r
            .path("/api/users/**")
            .filters(f -> f
                .rewritePath("/api/users/(?<segment>.*)", "/${segment}")
            )
            .uri("lb://USER-SERVICE")
        )
        
        // Réécriture complexe avec paramètres
        .route("search-service", r -> r
            .path("/api/search/{category}/{query}")
            .filters(f -> f
                .rewritePath(
                    "/api/search/(?<category>.*)/(?<query>.*)", 
                    "/search?cat=${category}&q=${query}"
                )
            )
            .uri("lb://SEARCH-SERVICE")
        )
        
        .build();
}
```

---

### **8. Eureka Integration with Gateway**

**Architecture:**
```
Eureka Server (Port 8761)
    ↓
Gateway (Port 8080) ← S'enregistre et fetch registry
    ↓
User Service (8081, 8082, 8083) ← S'enregistrent
Order Service (8091, 8092) ← S'enregistrent
Product Service (8101) ← S'enregistre
```

**Complete configuration:**

**gateway-server/application.yml :**
```yaml
spring:
  application:
    name: gateway-server
  cloud:
    gateway:
      discovery:
        locator:
          enabled: true  # Auto-routing depuis Eureka
          lower-case-service-id: true
      
      routes:
        # Routes spécifiques (priorité sur auto-routing)
        - id: user-service
          uri: lb://USER-SERVICE
          predicates:
            - Path=/api/users/**
          filters:
            - RewritePath=/api/users/(?<segment>.*), /${segment}
            - name: CircuitBreaker
              args:
                name: userServiceCB
                fallbackUri: forward:/fallback/users

eureka:
  client:
    service-url:
      defaultZone: http://localhost:8761/eureka/
    fetch-registry: true
    register-with-eureka: true
  instance:
    prefer-ip-address: true

server:
  port: 8080
```

**Auto-routing:**
With `discovery.locator.enabled: true`, Gateway automatically creates routes:
```
USER-SERVICE → http://gateway:8080/user-service/**
ORDER-SERVICE → http://gateway:8080/order-service/**
```

---

### **9. Cross-Cutting Concerns**

**Cross-cutting concerns** are features common to all microservices that are centralized in the Gateway:

#### **1. Authentication & Authorization**

```java
@Component
@Order(1)
public class AuthenticationGlobalFilter implements GlobalFilter {
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = exchange.getRequest();
        
        // Chemins publics (pas d'auth)
        if (isPublicPath(request.getPath().value())) {
            return chain.filter(exchange);
        }
        
        // Vérifier JWT token
        String token = extractToken(request);
        if (token == null || !jwtUtil.validateToken(token)) {
            return unauthorizedResponse(exchange);
        }
        
        // Ajouter user info aux headers
        String userId = jwtUtil.extractUserId(token);
        ServerHttpRequest modifiedRequest = request.mutate()
            .header("X-User-Id", userId)
            .build();
        
        return chain.filter(exchange.mutate().request(modifiedRequest).build());
    }
}
```

---

#### **2. Logging & Monitoring**

```java
@Component
@Order(2)
public class LoggingGlobalFilter implements GlobalFilter {
    
    private static final Logger logger = LoggerFactory.getLogger(LoggingGlobalFilter.class);
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = exchange.getRequest();
        
        // Générer correlation ID
        String correlationId = UUID.randomUUID().toString();
        exchange.getAttributes().put("correlationId", correlationId);
        
        logger.info("[{}] Request: {} {} from {}",
            correlationId,
            request.getMethod(),
            request.getURI(),
            request.getRemoteAddress()
        );
        
        long startTime = System.currentTimeMillis();
        
        return chain.filter(exchange).then(Mono.fromRunnable(() -> {
            long duration = System.currentTimeMillis() - startTime;
            logger.info("[{}] Response: Status {} - Duration: {}ms",
                correlationId,
                exchange.getResponse().getStatusCode(),
                duration
            );
            
            // Ajouter correlation ID à la réponse
            exchange.getResponse().getHeaders().add("X-Correlation-Id", correlationId);
            exchange.getResponse().getHeaders().add("X-Response-Time", duration + "ms");
        }));
    }
}
```

---

#### **3. Rate Limiting (Protection against abuse)**

```yaml
spring:
  cloud:
    gateway:
      routes:
        - id: rate-limited-service
          uri: lb://USER-SERVICE
          predicates:
            - Path=/api/users/**
          filters:
            - name: RequestRateLimiter
              args:
                redis-rate-limiter.replenishRate: 10
                redis-rate-limiter.burstCapacity: 20
                key-resolver: "#{@ipKeyResolver}"
```

```java
@Configuration
public class RateLimiterConfig {
    
    // Rate limit par IP
    @Bean
    public KeyResolver ipKeyResolver() {
        return exchange -> Mono.just(
            exchange.getRequest().getRemoteAddress().getAddress().getHostAddress()
        );
    }
    
    // Rate limit par User ID
    @Bean
    public KeyResolver userKeyResolver() {
        return exchange -> Mono.justOrEmpty(
            exchange.getRequest().getHeaders().getFirst("X-User-Id")
        );
    }
    
    // Rate limit par API Key
    @Bean
    public KeyResolver apiKeyResolver() {
        return exchange -> Mono.justOrEmpty(
            exchange.getRequest().getHeaders().getFirst("X-API-Key")
        );
    }
}
```

---

#### **4. CORS Handling**

```java
@Configuration
public class CorsConfig {
    
    @Bean
    public CorsWebFilter corsWebFilter() {
        CorsConfiguration corsConfig = new CorsConfiguration();
        corsConfig.setAllowedOrigins(Arrays.asList("http://localhost:3000", "https://myapp.com"));
        corsConfig.setAllowedMethods(Arrays.asList("GET", "POST", "PUT", "DELETE", "OPTIONS"));
        corsConfig.setAllowedHeaders(Arrays.asList("*"));
        corsConfig.setAllowCredentials(true);
        corsConfig.setMaxAge(3600L);
        
        UrlBasedCorsConfigurationSource source = new UrlBasedCorsConfigurationSource();
        source.registerCorsConfiguration("/**", corsConfig);
        
        return new CorsWebFilter(source);
    }
}
```

---

#### **5. Request/Response Transformation**

```java
@Component
@Order(3)
public class HeaderEnrichmentFilter implements GlobalFilter {
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        // Enrichir les headers de requête pour downstream services
        ServerHttpRequest modifiedRequest = exchange.getRequest().mutate()
            .header("X-Gateway-Version", "1.0")
            .header("X-Request-Id", UUID.randomUUID().toString())
            .header("X-Forwarded-For", getClientIp(exchange))
            .header("X-Timestamp", Instant.now().toString())
            .build();
        
        return chain.filter(exchange.mutate().request(modifiedRequest).build())
            .then(Mono.fromRunnable(() -> {
                // Ajouter headers de réponse
                ServerHttpResponse response = exchange.getResponse();
                response.getHeaders().add("X-Powered-By", "Spring Cloud Gateway");
                response.getHeaders().add("X-Content-Type-Options", "nosniff");
                response.getHeaders().add("X-Frame-Options", "DENY");
                response.getHeaders().add("X-XSS-Protection", "1; mode=block");
            }));
    }
    
    private String getClientIp(ServerWebExchange exchange) {
        String xff = exchange.getRequest().getHeaders().getFirst("X-Forwarded-For");
        if (xff != null) {
            return xff.split(",")[0].trim();
        }
        return exchange.getRequest().getRemoteAddress().getAddress().getHostAddress();
    }
}
```

---

#### **6. Error Handling & Fallback**

```java
@RestController
public class FallbackController {
    
    @GetMapping("/fallback/users")
    public ResponseEntity<Map<String, Object>> userServiceFallback() {
        Map<String, Object> response = new HashMap<>();
        response.put("message", "User service is temporarily unavailable");
        response.put("status", "SERVICE_UNAVAILABLE");
        response.put("timestamp", Instant.now());
        return ResponseEntity.status(HttpStatus.SERVICE_UNAVAILABLE).body(response);
    }
    
    @GetMapping("/fallback/orders")
    public ResponseEntity<Map<String, Object>> orderServiceFallback() {
        Map<String, Object> response = new HashMap<>();
        response.put("message", "Order service is temporarily unavailable");
        response.put("status", "SERVICE_UNAVAILABLE");
        return ResponseEntity.status(HttpStatus.SERVICE_UNAVAILABLE).body(response);
    }
}

// Global Error Handler
@Component
public class GlobalErrorWebExceptionHandler extends AbstractErrorWebExceptionHandler {
    
    public GlobalErrorWebExceptionHandler(
            ErrorAttributes errorAttributes,
            ResourceProperties resourceProperties,
            ApplicationContext applicationContext
    ) {
        super(errorAttributes, resourceProperties, applicationContext);
    }
    
    @Override
    protected RouterFunction<ServerResponse> getRoutingFunction(ErrorAttributes errorAttributes) {
        return RouterFunctions.route(RequestPredicates.all(), this::renderErrorResponse);
    }
    
    private Mono<ServerResponse> renderErrorResponse(ServerRequest request) {
        Map<String, Object> errorPropertiesMap = getErrorAttributes(request, ErrorAttributeOptions.defaults());
        
        return ServerResponse.status(HttpStatus.BAD_REQUEST)
            .contentType(MediaType.APPLICATION_JSON)
            .body(BodyInserters.fromValue(errorPropertiesMap));
    }
}
```

---

#### **7. Circuit Breaker Pattern**

```yaml
spring:
  cloud:
    gateway:
      routes:
        - id: user-service-with-cb
          uri: lb://USER-SERVICE
          predicates:
            - Path=/api/users/**
          filters:
            - name: CircuitBreaker
              args:
                name: userServiceCB
                fallbackUri: forward:/fallback/users
            - name: Retry
              args:
                retries: 3
                statuses: BAD_GATEWAY, INTERNAL_SERVER_ERROR
                methods: GET
                backoff:
                  firstBackoff: 100ms
                  maxBackoff: 1000ms
                  factor: 2

# Resilience4j Configuration
resilience4j:
  circuitbreaker:
    instances:
      userServiceCB:
        sliding-window-size: 10
        failure-rate-threshold: 50
        wait-duration-in-open-state: 10000
        permitted-number-of-calls-in-half-open-state: 3
        automatic-transition-from-open-to-half-open-enabled: true
  timelimiter:
    instances:
      userServiceCB:
        timeout-duration: 3s
```

---

#### **8. Request/Response Caching**

```java
@Component
public class ResponseCacheFilter implements GlobalFilter, Ordered {
    
    @Autowired
    private RedisTemplate<String, String> redisTemplate;
    
    private static final Set<String> CACHEABLE_PATHS = Set.of(
        "/api/products",
        "/api/categories"
    );
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = exchange.getRequest();
        String path = request.getPath().value();
        
        // Only cache GET requests
        if (!request.getMethod().equals(HttpMethod.GET) || !isCacheable(path)) {
            return chain.filter(exchange);
        }
        
        String cacheKey = generateCacheKey(request);
        
        // Check cache
        String cachedResponse = redisTemplate.opsForValue().get(cacheKey);
        if (cachedResponse != null) {
            // Return cached response
            ServerHttpResponse response = exchange.getResponse();
            response.setStatusCode(HttpStatus.OK);
            response.getHeaders().add("X-Cache", "HIT");
            
            DataBuffer buffer = response.bufferFactory().wrap(cachedResponse.getBytes());
            return response.writeWith(Mono.just(buffer));
        }
        
        // Cache miss - continue to service
        exchange.getResponse().getHeaders().add("X-Cache", "MISS");
        
        // Cache response (après avoir reçu la réponse)
        return chain.filter(exchange).then(Mono.fromRunnable(() -> {
            // Store in cache with TTL
            // Note: Real implementation would need to capture response body
            redisTemplate.opsForValue().set(cacheKey, "response", 5, TimeUnit.MINUTES);
        }));
    }
    
    private boolean isCacheable(String path) {
        return CACHEABLE_PATHS.stream().anyMatch(path::startsWith);
    }
    
    private String generateCacheKey(ServerHttpRequest request) {
        return "gateway:cache:" + request.getPath().value() + ":" + 
               request.getQueryParams().toString();
    }
    
    @Override
    public int getOrder() {
        return 0;
    }
}
```

---

#### **9. Request Validation**

```java
@Component
@Order(4)
public class RequestValidationFilter implements GlobalFilter {
    
    private static final Set<String> REQUIRED_HEADERS = Set.of(
        "X-API-Version",
        "Content-Type"
    );
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = exchange.getRequest();
        
        // Skip validation for public paths
        if (isPublicPath(request.getPath().value())) {
            return chain.filter(exchange);
        }
        
        // Validate required headers
        for (String header : REQUIRED_HEADERS) {
            if (!request.getHeaders().containsKey(header)) {
                return badRequestResponse(exchange, "Missing required header: " + header);
            }
        }
        
        // Validate API version
        String apiVersion = request.getHeaders().getFirst("X-API-Version");
        if (!isValidApiVersion(apiVersion)) {
            return badRequestResponse(exchange, "Unsupported API version: " + apiVersion);
        }
        
        // Validate request size (prevent large payloads)
        String contentLength = request.getHeaders().getFirst("Content-Length");
        if (contentLength != null) {
            long size = Long.parseLong(contentLength);
            if (size > 10_000_000) { // 10 MB limit
                return badRequestResponse(exchange, "Request payload too large");
            }
        }
        
        return chain.filter(exchange);
    }
    
    private boolean isValidApiVersion(String version) {
        return version != null && (version.equals("v1") || version.equals("v2"));
    }
    
    private boolean isPublicPath(String path) {
        return path.startsWith("/public") || path.startsWith("/health");
    }
    
    private Mono<Void> badRequestResponse(ServerWebExchange exchange, String message) {
        ServerHttpResponse response = exchange.getResponse();
        response.setStatusCode(HttpStatus.BAD_REQUEST);
        response.getHeaders().setContentType(MediaType.APPLICATION_JSON);
        
        String body = String.format("{\"error\": \"%s\", \"timestamp\": \"%s\"}", 
            message, Instant.now());
        
        DataBuffer buffer = response.bufferFactory().wrap(body.getBytes());
        return response.writeWith(Mono.just(buffer));
    }
}
```

---

### **10. Configuration Complète d'un Gateway Server**

**Project structure:**
```
gateway-server/
├── src/main/java/
│   └── com/example/gateway/
│       ├── GatewayServerApplication.java
│       ├── config/
│       │   ├── GatewayConfig.java
│       │   ├── SecurityConfig.java
│       │   ├── CorsConfig.java
│       │   └── RateLimiterConfig.java
│       ├── filter/
│       │   ├── AuthenticationFilter.java
│       │   ├── LoggingFilter.java
│       │   ├── ResponseTimeFilter.java
│       │   └── HeaderEnrichmentFilter.java
│       ├── controller/
│       │   └── FallbackController.java
│       └── util/
│           └── JwtUtil.java
├── src/main/resources/
│   └── application.yml
└── pom.xml
```

**pom.xml :**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<project>
    <parent>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-parent</artifactId>
        <version>3.2.0</version>
    </parent>
    
    <groupId>com.example</groupId>
    <artifactId>gateway-server</artifactId>
    <version>1.0.0</version>
    
    <properties>
        <java.version>17</java.version>
        <spring-cloud.version>2023.0.0</spring-cloud.version>
    </properties>
    
    <dependencies>
        <!-- Spring Cloud Gateway -->
        <dependency>
            <groupId>org.springframework.cloud</groupId>
            <artifactId>spring-cloud-starter-gateway</artifactId>
        </dependency>
        
        <!-- Eureka Client -->
        <dependency>
            <groupId>org.springframework.cloud</groupId>
            <artifactId>spring-cloud-starter-netflix-eureka-client</artifactId>
        </dependency>
        
        <!-- Circuit Breaker -->
        <dependency>
            <groupId>org.springframework.cloud</groupId>
            <artifactId>spring-cloud-starter-circuitbreaker-reactor-resilience4j</artifactId>
        </dependency>
        
        <!-- Redis (pour Rate Limiting & Caching) -->
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-data-redis-reactive</artifactId>
        </dependency>
        
        <!-- JWT -->
        <dependency>
            <groupId>io.jsonwebtoken</groupId>
            <artifactId>jjwt-api</artifactId>
            <version>0.11.5</version>
        </dependency>
        <dependency>
            <groupId>io.jsonwebtoken</groupId>
            <artifactId>jjwt-impl</artifactId>
            <version>0.11.5</version>
            <scope>runtime</scope>
        </dependency>
        <dependency>
            <groupId>io.jsonwebtoken</groupId>
            <artifactId>jjwt-jackson</artifactId>
            <version>0.11.5</version>
            <scope>runtime</scope>
        </dependency>
        
        <!-- Actuator (Monitoring) -->
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-actuator</artifactId>
        </dependency>
        
        <!-- Micrometer (Metrics) -->
        <dependency>
            <groupId>io.micrometer</groupId>
            <artifactId>micrometer-registry-prometheus</artifactId>
        </dependency>
    </dependencies>
    
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
</project>
```

**Complete application.yml:**
```yaml
spring:
  application:
    name: gateway-server
  
  # Redis Configuration (pour Rate Limiting)
  redis:
    host: localhost
    port: 6379
  
  # Cloud Gateway Configuration
  cloud:
    gateway:
      # Auto-discovery depuis Eureka
      discovery:
        locator:
          enabled: true
          lower-case-service-id: true
      
      # Global CORS Configuration
      globalcors:
        cors-configurations:
          '[/**]':
            allowed-origins:
              - "http://localhost:3000"
              - "https://myapp.com"
            allowed-methods:
              - GET
              - POST
              - PUT
              - DELETE
              - OPTIONS
            allowed-headers: "*"
            allow-credentials: true
            max-age: 3600
      
      # Routes Configuration
      routes:
        # User Service
        - id: user-service
          uri: lb://USER-SERVICE
          predicates:
            - Path=/api/users/**
          filters:
            - RewritePath=/api/users/(?<segment>.*), /${segment}
            - name: CircuitBreaker
              args:
                name: userServiceCB
                fallbackUri: forward:/fallback/users
            - name: RequestRateLimiter
              args:
                redis-rate-limiter.replenishRate: 10
                redis-rate-limiter.burstCapacity: 20
                key-resolver: "#{@userKeyResolver}"
            - name: Retry
              args:
                retries: 3
                statuses: BAD_GATEWAY
                methods: GET
        
        # Order Service
        - id: order-service
          uri: lb://ORDER-SERVICE
          predicates:
            - Path=/api/orders/**
            - Method=GET,POST
          filters:
            - RewritePath=/api/orders/(?<segment>.*), /${segment}
            - name: CircuitBreaker
              args:
                name: orderServiceCB
                fallbackUri: forward:/fallback/orders
        
        # Product Service (avec versioning)
        - id: product-service-v1
          uri: lb://PRODUCT-SERVICE
          predicates:
            - Path=/api/v1/products/**
          filters:
            - RewritePath=/api/v1/products/(?<segment>.*), /v1/products/${segment}
        
        - id: product-service-v2
          uri: lb://PRODUCT-SERVICE
          predicates:
            - Path=/api/v2/products/**
            - Header=X-API-Version, v2
          filters:
            - RewritePath=/api/v2/products/(?<segment>.*), /v2/products/${segment}
        
        # Payment Service (sécurisé, retry agressif)
        - id: payment-service
          uri: lb://PAYMENT-SERVICE
          predicates:
            - Path=/api/payments/**
          filters:
            - RewritePath=/api/payments/(?<segment>.*), /${segment}
            - name: Retry
              args:
                retries: 5
                statuses: INTERNAL_SERVER_ERROR, BAD_GATEWAY
                methods: POST
                backoff:
                  firstBackoff: 100ms
                  maxBackoff: 2000ms
                  factor: 2
                  basedOnPreviousValue: true
        
        # Health Check (public)
        - id: health-check
          uri: lb://USER-SERVICE
          predicates:
            - Path=/actuator/health
          filters:
            - RewritePath=/actuator/health, /actuator/health

# Eureka Client Configuration
eureka:
  client:
    service-url:
      defaultZone: http://localhost:8761/eureka/
    fetch-registry: true
    register-with-eureka: true
    registry-fetch-interval-seconds: 5
  instance:
    prefer-ip-address: true
    instance-id: ${spring.application.name}:${random.value}
    lease-renewal-interval-in-seconds: 5
    lease-expiration-duration-in-seconds: 10

# Resilience4j Circuit Breaker Configuration
resilience4j:
  circuitbreaker:
    instances:
      userServiceCB:
        sliding-window-size: 10
        failure-rate-threshold: 50
        wait-duration-in-open-state: 10s
        permitted-number-of-calls-in-half-open-state: 3
        automatic-transition-from-open-to-half-open-enabled: true
        minimum-number-of-calls: 5
      
      orderServiceCB:
        sliding-window-size: 10
        failure-rate-threshold: 60
        wait-duration-in-open-state: 15s
        permitted-number-of-calls-in-half-open-state: 5
  
  timelimiter:
    instances:
      userServiceCB:
        timeout-duration: 3s
      orderServiceCB:
        timeout-duration: 5s

# Actuator Configuration (Monitoring)
management:
  endpoints:
    web:
      exposure:
        include: health,info,metrics,prometheus,gateway
  endpoint:
    health:
      show-details: always
  metrics:
    export:
      prometheus:
        enabled: true

# JWT Configuration
jwt:
  secret: ${JWT_SECRET:myVerySecureSecretKeyForJwtTokenGenerationWithAtLeast256BitsForHS256Algorithm}

# Server Configuration
server:
  port: 8080

# Logging Configuration
logging:
  level:
    org.springframework.cloud.gateway: DEBUG
    com.example.gateway: DEBUG
  pattern:
    console: "%d{yyyy-MM-dd HH:mm:ss} - %msg%n"
```

**GatewayServerApplication.java :**
```java
package com.example.gateway;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;

@SpringBootApplication
@EnableDiscoveryClient
public class GatewayServerApplication {
    
    public static void main(String[] args) {
        SpringApplication.run(GatewayServerApplication.class, args);
    }
}
```

**ResponseTimeFilter.java (Complete Custom Filter):**
```java
package com.example.gateway.filter;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.cloud.gateway.filter.GatewayFilterChain;
import org.springframework.cloud.gateway.filter.GlobalFilter;
import org.springframework.core.Ordered;
import org.springframework.http.server.reactive.ServerHttpRequest;
import org.springframework.http.server.reactive.ServerHttpResponse;
import org.springframework.stereotype.Component;
import org.springframework.web.server.ServerWebExchange;
import reactor.core.publisher.Mono;

import java.util.UUID;

@Component
public class ResponseTimeFilter implements GlobalFilter, Ordered {
    
    private static final Logger logger = LoggerFactory.getLogger(ResponseTimeFilter.class);
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        // Generate correlation ID
        String correlationId = UUID.randomUUID().toString();
        
        // Add to request attributes
        exchange.getAttributes().put("correlationId", correlationId);
        exchange.getAttributes().put("startTime", System.currentTimeMillis());
        
        ServerHttpRequest request = exchange.getRequest();
        
        logger.info("[{}] Incoming request: {} {} from {}",
            correlationId,
            request.getMethod(),
            request.getURI(),
            request.getRemoteAddress()
        );
        
        // Continue filter chain
        return chain.filter(exchange).then(Mono.fromRunnable(() -> {
            Long startTime = exchange.getAttribute("startTime");
            if (startTime != null) {
                long duration = System.currentTimeMillis() - startTime;
                
                ServerHttpResponse response = exchange.getResponse();
                
                logger.info("[{}] Response: Status {} - Duration: {}ms - Path: {}",
                    correlationId,
                    response.getStatusCode(),
                    duration,
                    request.getPath()
                );
                
                // Add headers
                response.getHeaders().add("X-Correlation-Id", correlationId);
                response.getHeaders().add("X-Response-Time", duration + "ms");
            }
        }));
    }
    
    @Override
    public int getOrder() {
        return Ordered.LOWEST_PRECEDENCE;
    }
}
```

---

### **11. Testing Gateway**

**Manual test with curl:**
```bash
# Test basic routing
curl -X GET http://localhost:8080/api/users/1

# Test avec JWT
curl -X GET http://localhost:8080/api/users/1 \
  -H "Authorization: Bearer eyJhbGc..."

# Test rate limiting (envoyer 25 requêtes rapidement)
for i in {1..25}; do
  curl -X GET http://localhost:8080/api/users/1
done

# Test avec custom headers
curl -X GET http://localhost:8080/api/v2/products/1 \
  -H "X-API-Version: v2" \
  -H "X-User-Id: 123"

# Test fallback (shutdown service)
curl -X GET http://localhost:8080/api/users/1
# → Should return fallback response
```

**Integration Test:**
```java
@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
@AutoConfigureWebTestClient
class GatewayIntegrationTest {
    
    @Autowired
    private WebTestClient webTestClient;
    
    @Test
    void testUserServiceRouting() {
        webTestClient.get()
            .uri("/api/users/1")
            .header("Authorization", "Bearer " + generateTestToken())
            .exchange()
            .expectStatus().isOk()
            .expectHeader().exists("X-Correlation-Id")
            .expectHeader().exists("X-Response-Time");
    }
    
    @Test
    void testRateLimiting() {
        // Send 25 requests (limit is 20)
        for (int i = 0; i < 25; i++) {
            WebTestClient.ResponseSpec response = webTestClient.get()
                .uri("/api/users/1")
                .exchange();
            
            if (i < 20) {
                response.expectStatus().isOk();
            } else {
                response.expectStatus().isEqualTo(HttpStatus.TOO_MANY_REQUESTS);
            }
        }
    }
    
    @Test
    void testRewritePath() {
        webTestClient.get()
            .uri("/api/users/123")
            .exchange()
            .expectStatus().isOk();
        
        // Verify path was rewritten to /users/123 (check logs or mock service)
    }
    
    @Test
    void testCircuitBreakerFallback() {
        // Simulate service down scenario
        webTestClient.get()
            .uri("/api/users/1")
            .exchange()
            .expectStatus().is5xxServerError()
            .expectBody()
            .jsonPath("$.message").isEqualTo("User service is temporarily unavailable");
    }
}
```

---

### **12. Best Practices & Recommandations**

**✅ DO:**
- Use **lb://** for internal services (load balancing + service discovery)
- Implement **Circuit Breaker** to avoid cascade failures
- Add **correlation IDs** to trace requests
- Configure reasonable **timeouts** (3-5s)
- Use **Rate Limiting** to protect services
- Centralize **authentication/authorization** in the Gateway
- Log all requests/responses for monitoring
- Use **YAML** for simple configuration, **Java** for complex logic

**❌ DON'T:**
- Do not put **business logic** in the Gateway
- Avoid **heavy transformations** (XML parsing, etc.)
- Do not make **DB calls** from the Gateway
- Avoid **synchronous blocking calls** (use reactive)
- Do not hardcode URLs (use service discovery)
- Avoid exposing **stack traces** in production

---

**Summary of mastered skills:**

✅ **Gateway Patterns**: Single Entry Point, BFF, Aggregation  
✅ **Spring Cloud Gateway**: Routes, Predicates, Filters  
✅ **Service Discovery**: Integration with Eureka (lb://)  
✅ **Cross-Cutting Concerns**: Auth, Logging, Rate Limiting, CORS, Circuit Breaker  
✅ **Path Rewriting**: RewritePath, StripPrefix  
✅ **Configuration**: YAML vs Java Code  
✅ **Custom Filters**: Global & Gateway Filters  
✅ **Resilience**: Circuit Breaker, Retry, Fallback  

These skills cover all the requirements for implementing a robust and scalable API Gateway in a modern microservices architecture. 🚀



---

Here are complete Q&A pairs to prepare for your interview on API Gateway and Spring Cloud Gateway:

---

## **Q&A: API Gateway & Spring Cloud Gateway**

### **General Questions about API Gateway**

**Q1: What is an API Gateway and why do we need it in a microservices architecture?**

**A:** 

An **API Gateway** is a single entry point that acts as a **reverse proxy** between clients and backend microservices.

**Problems without a Gateway:**
```
Client Mobile → Service A (http://serviceA:8081)
Client Mobile → Service B (http://serviceB:8082)
Client Mobile → Service C (http://serviceC:8083)

Problèmes :
❌ Client doit connaître tous les endpoints
❌ Sécurité dupliquée dans chaque service
❌ CORS configuré partout
❌ Pas de rate limiting centralisé
❌ Difficile de monitorer
❌ Changement d'URL = update tous les clients
```

**With a Gateway:**
```
Tous les clients → API Gateway (http://gateway:8080)
                        ↓
                Route intelligemment vers services
```

**Gateway Responsibilities:**
- ✅ **Routing**: Direct requests to the right service
- ✅ **Authentication/Authorization**: Verify JWT, permissions
- ✅ **Rate Limiting**: Limit abuse/attacks
- ✅ **Load Balancing**: Distribute load between instances
- ✅ **Circuit Breaking**: Avoid cascade failures
- ✅ **Logging/Monitoring**: Trace all requests
- ✅ **Request/Response Transformation**: Adapt formats
- ✅ **CORS Handling**: Centralized configuration
- ✅ **SSL Termination**: Handle HTTPS at the Gateway

**Advantages:**
- Client-side simplification (single URL)
- Centralized cross-cutting concerns
- Enhanced security
- Simplified scalability

---

**Q2: What is the difference between an API Gateway and a Load Balancer?**

**A:** 

| Aspect | Load Balancer | API Gateway |
|--------|--------------|-------------|
| **OSI Layer** | Layer 4 (Transport - TCP/UDP) | Layer 7 (Application - HTTP/HTTPS) |
| **Routing** | Simple (round-robin, IP hash) | Intelligent (path, headers, methods) |
| **Features** | Load distribution | Routing + Security + Transformation |
| **Content awareness** | No (does not see HTTP) | Yes (understands HTTP requests) |
| **Use case** | Distribute traffic between identical servers | Orchestrate different microservices |

**Load Balancer Example (Nginx):**
```nginx
upstream backend {
    server server1:8080;
    server server2:8080;
    server server3:8080;
}

server {
    location / {
        proxy_pass http://backend;  # Round-robin simple
    }
}
```

**API Gateway Example (Spring Cloud Gateway):**
```yaml
routes:
  - id: user-service
    uri: lb://USER-SERVICE
    predicates:
      - Path=/api/users/**
      - Header=X-API-Version, v2
    filters:
      - RewritePath=/api/users/(?<segment>.*), /${segment}
      - AddRequestHeader=X-Gateway, Gateway-1
```

**In summary:** Load Balancer = simple distribution, API Gateway = intelligent orchestration + security + transformations.

---

**Q3: What are the three main Gateway Patterns and when to use them?**

**A:** 

**1. Single Entry Point**

```
Web, Mobile, IoT → Gateway Unique → Service A, B, C
```

**Characteristics:**
- A single Gateway for all clients
- Configuration centralisée
- The simplest architecture

**Advantages:**
- ✅ Easy to maintain
- ✅ Unified monitoring
- ✅ Centralized configuration

**Disadvantages:**
- ❌ Single Point of Failure (SPOF)
- ❌ Can become a bottleneck
- ❌ All clients get the same processing

**When to use it:** Small/medium-sized applications with similar clients.

---

**2. Backend for Frontend (BFF)**

```
Web App → Web Gateway → Services (format Web)
Mobile App → Mobile Gateway → Services (format Mobile optimisé)
IoT → IoT Gateway → Services (format IoT minimal)
```

**Characteristics:**
- A specific Gateway per client type
- Responses optimized for the client

**Concrete example:**
```java
// Mobile Gateway - Données légères
GET /api/products → {
    id: 1, 
    name: "iPhone", 
    price: 999, 
    thumbnail: "small.jpg"
}

// Web Gateway - Données complètes
GET /api/products → {
    id: 1,
    name: "iPhone 15 Pro Max",
    description: "...",
    price: 999,
    images: ["large1.jpg", "large2.jpg"],
    reviews: [...],
    specifications: {...}
}
```

**Advantages:**
- ✅ **Optimized performance** per platform
- ✅ Independent evolution (change Mobile without affecting Web)
- ✅ Client-specific logic

**Disadvantages:**
- ❌ More complex (multiple Gateways)
- ❌ Potential duplicated code

**When to use it:** Very different clients (rich Web, lightweight Mobile, minimal IoT).

---

**3. Aggregation Gateway**

```
Client → Gateway → Agrège réponses de Services A, B, C → Retourne tout en une fois
```

**Problem solved:**
Without aggregation, the mobile client must make 3 calls:
```
GET /users/123
GET /users/123/orders
GET /users/123/payments
→ 3 appels réseau = lent, consomme batterie
```

With aggregation:
```
GET /api/users/123/full-profile
→ Gateway appelle 3 services en parallèle
→ Agrège résultats
→ 1 seule réponse complète
```

**Implementation:**
```java
@GetMapping("/users/{id}/full-profile")
public Mono<UserFullProfile> getUserFullProfile(@PathVariable Long id) {
    Mono<User> userMono = userService.getUser(id);
    Mono<List<Order>> ordersMono = orderService.getUserOrders(id);
    Mono<PaymentInfo> paymentMono = paymentService.getUserPayments(id);
    
    return Mono.zip(userMono, ordersMono, paymentMono)
        .map(tuple -> new UserFullProfile(
            tuple.getT1(), // user
            tuple.getT2(), // orders
            tuple.getT3()  // payments
        ));
}
```

**Advantages:**
- ✅ **Latency reduction**: 3 parallel calls instead of 3 sequential
- ✅ **Fewer network round-trips** on the client side
- ✅ Improved mobile performance

**Disadvantages:**
- ❌ **Business logic in Gateway** (SRP violation)
- ❌ Increased coupling between services
- ❌ Increased complexity

**When to use it:** Mobile applications with slow connections, need to optimize latency.

---

### **Questions about Spring Cloud Gateway**

**Q4: What is the difference between Netflix Zuul and Spring Cloud Gateway?**

**A:** 

| Aspect | Netflix Zuul (Zuul 1) | Spring Cloud Gateway |
|--------|----------------------|---------------------|
| **Architecture** | Blocking (Servlet) | Non-blocking (Reactive - WebFlux) |
| **Performance** | 1 thread per request | Asynchronous, event-loop |
| **Scalability** | Limited (threads) | Excellent (reactive) |
| **Spring Boot 3** | ❌ Not supported | ✅ Supported |
| **Maintenance** | Netflix deprecated | Actively maintained by Spring |
| **Filters** | Simple Pre/Post filters | Pre/Post + Order + Conditional |
| **Programming Model** | Synchronous | Reactive (Mono/Flux) |

**Migration Zuul → Gateway:**
```java
// Zuul (deprecated)
@EnableZuulProxy
public class ZuulConfig {
    // Configuration via properties
}

// Spring Cloud Gateway (moderne)
@EnableDiscoveryClient
public class GatewayConfig {
    @Bean
    public RouteLocator routes(RouteLocatorBuilder builder) {
        return builder.routes()
            .route(r -> r.path("/api/**").uri("lb://SERVICE"))
            .build();
    }
}
```

**Recommendation:** Use **Spring Cloud Gateway** for all new projects (better performance, reactive, supported).

---

**Q5: Explain the architecture of Spring Cloud Gateway with its main components**

**A:** 

**Spring Cloud Gateway Architecture:**

```
Client Request
    ↓
[DispatcherHandler] → Entry point
    ↓
[RoutePredicateHandlerMapping] → Trouve route matching
    ↓
[FilteringWebHandler] → Applique filters
    ↓
┌──────────────────────────────┐
│  Gateway Filter Chain        │
│  ┌────────────────────────┐  │
│  │ Pre-Filters            │  │ (avant routing)
│  │ - Authentication       │  │
│  │ - Rate Limiting        │  │
│  │ - Logging              │  │
│  └────────────────────────┘  │
│            ↓                  │
│  [Proxy Filter]              │ → Forward to microservice
│            ↓                  │
│  Microservice Processing     │
│            ↓                  │
│  ┌────────────────────────┐  │
│  │ Post-Filters           │  │ (après réponse)
│  │ - Add Headers          │  │
│  │ - Modify Response      │  │
│  │ - Metrics              │  │
│  └────────────────────────┘  │
└──────────────────────────────┘
    ↓
Client Response
```

**Key components:**

**1. Route:**
Configuration defining how to route a request.
```java
Route {
    id: "user-service",
    uri: "lb://USER-SERVICE",
    predicates: [Path=/api/users/**],
    filters: [RewritePath, AddHeader],
    order: 0
}
```

**2. Predicate:**
Condition to match a request (equivalent to an `if` condition).
```yaml
predicates:
  - Path=/api/users/**           # URI path matching
  - Method=GET,POST               # HTTP method
  - Header=X-Request-Id, \d+     # Header exists with regex
  - Query=version, v1             # Query parameter
  - Cookie=sessionId, abc.*       # Cookie matching
  - Host=**.myapp.com             # Host matching
  - After=2024-01-01T00:00:00Z   # Time-based
```

**3. Filter:**
Pre/post routing processing.

**Pre-Filters (before):**
- Authenticate user (JWT)
- Validate request
- Rate limiting
- Add headers
- Rewrite path

**Post-Filters (after):**
- Add headers de réponse
- Modify response body
- Log metrics
- Circuit breaker handling

---

**Q6: How do you configure routes in Spring Cloud Gateway? YAML vs Java Code**

**A:** 

**Option 1: YAML Configuration (Declarative - Recommended for simplicity)**

```yaml
spring:
  cloud:
    gateway:
      routes:
        # Route simple
        - id: user-service
          uri: lb://USER-SERVICE
          predicates:
            - Path=/api/users/**
          filters:
            - RewritePath=/api/users/(?<segment>.*), /${segment}
        
        # Route avec multiples predicates
        - id: order-service-get
          uri: lb://ORDER-SERVICE
          predicates:
            - Path=/api/orders/**
            - Method=GET
            - Header=X-API-Version, v2
          filters:
            - RewritePath=/api/orders/(?<segment>.*), /${segment}
            - AddRequestHeader=X-Gateway, Spring-Cloud-Gateway
        
        # Route avec rate limiting
        - id: payment-service
          uri: lb://PAYMENT-SERVICE
          predicates:
            - Path=/api/payments/**
          filters:
            - name: RequestRateLimiter
              args:
                redis-rate-limiter.replenishRate: 10
                redis-rate-limiter.burstCapacity: 20
```

**YAML Advantages:**
- ✅ Readable and simple
- ✅ No need to recompile
- ✅ Externalizable (Config Server)
- ✅ Ideal for simple routes

---

**Option 2: Java Code Configuration (Programmatic - For complex logic)**

```java
@Configuration
public class GatewayConfig {
    
    @Bean
    public RouteLocator customRoutes(RouteLocatorBuilder builder) {
        return builder.routes()
            // Route 1: Simple routing
            .route("user-service", r -> r
                .path("/api/users/**")
                .filters(f -> f
                    .rewritePath("/api/users/(?<segment>.*)", "/${segment}")
                    .addRequestHeader("X-Gateway", "Gateway-1")
                )
                .uri("lb://USER-SERVICE")
            )
            
            // Route 2: Multiples predicates avec AND
            .route("order-service", r -> r
                .path("/api/orders/**")
                .and()
                .method(HttpMethod.GET, HttpMethod.POST)
                .and()
                .header("X-Request-Type", "standard")
                .filters(f -> f
                    .rewritePath("/api/orders/(?<segment>.*)", "/${segment}")
                    .circuitBreaker(c -> c
                        .setName("orderServiceCB")
                        .setFallbackUri("forward:/fallback/orders")
                    )
                )
                .uri("lb://ORDER-SERVICE")
            )
            
            // Route 3: Custom filter
            .route("product-service", r -> r
                .path("/api/products/**")
                .filters(f -> f
                    .filter(new AuthenticationFilter())  // Custom filter
                    .filter(new LoggingFilter())
                    .retry(config -> config
                        .setRetries(3)
                        .setStatuses(HttpStatus.INTERNAL_SERVER_ERROR)
                        .setBackoff(Duration.ofMillis(100), Duration.ofSeconds(1), 2, true)
                    )
                )
                .uri("lb://PRODUCT-SERVICE")
            )
            
            // Route 4: Conditional routing (logique complexe)
            .route("conditional-route", r -> r
                .path("/api/**")
                .and()
                .predicate(exchange -> {
                    // Logique custom complexe
                    String userAgent = exchange.getRequest()
                        .getHeaders().getFirst("User-Agent");
                    return userAgent != null && userAgent.contains("Mobile");
                })
                .filters(f -> f.prefixPath("/mobile"))
                .uri("lb://MOBILE-OPTIMIZED-SERVICE")
            )
            
            .build();
    }
}
```

**Java Code Advantages:**
- ✅ Complex conditional logic
- ✅ Full API access
- ✅ Type-safe (compilation checks)
- ✅ Custom filters easily integrated
- ✅ Easy unit tests

---

**My recommendation:**
- **YAML**: For 80% of routes (simple, standard)
- **Java Code**: For complex logic, custom filters, dynamic conditions

**Both can be combined:**
```java
// Java Code pour routes complexes
@Configuration
public class ComplexRoutesConfig { }

// YAML pour routes simples
spring:
  cloud:
    gateway:
      routes:
        - id: simple-route
          uri: lb://SERVICE
```

---

**Q7: What is a Predicate in Spring Cloud Gateway? Give examples**

**A:** 

A **Predicate** is a **boolean condition** that determines whether a route should be activated for a given request. It is the equivalent of an `if (condition)` for routing.

**Built-in Predicates:**

**1. Path Predicate (most common):**
```yaml
predicates:
  - Path=/api/users/**        # Match tous les paths commençant par /api/users/
  - Path=/api/{segment}       # Capture de variable
  - Path=/api/v{version}/**   # Match /api/v1/, /api/v2/
```

**2. Method Predicate:**
```yaml
predicates:
  - Method=GET                # Uniquement GET
  - Method=GET,POST           # GET OU POST
```

**3. Header Predicate:**
```yaml
predicates:
  - Header=X-Request-Id, \d+  # Header existe ET matche regex
  - Header=X-API-Version, v2  # Header a valeur spécifique
```

**4. Query Predicate:**
```yaml
predicates:
  - Query=version, v1         # ?version=v1
  - Query=debug               # ?debug (peu importe valeur)
```

**5. Cookie Predicate:**
```yaml
predicates:
  - Cookie=sessionId, abc.*   # Cookie existe ET matche regex
```

**6. Host Predicate:**
```yaml
predicates:
  - Host=**.myapp.com         # Matche api.myapp.com, admin.myapp.com
  - Host=localhost:8080
```

**7. RemoteAddr Predicate (IP-based routing):**
```yaml
predicates:
  - RemoteAddr=192.168.1.0/24  # Subnet CIDR
```

**8. Weight Predicate (A/B Testing, Canary Deployments):**
```yaml
routes:
  - id: service-v1
    uri: lb://SERVICE-V1
    predicates:
      - Path=/api/**
      - Weight=group1, 8        # 80% du trafic

  - id: service-v2
    uri: lb://SERVICE-V2
    predicates:
      - Path=/api/**
      - Weight=group1, 2        # 20% du trafic (canary)
```

**9. DateTime Predicates:**
```yaml
predicates:
  - After=2024-01-01T00:00:00Z           # Activer après date
  - Before=2024-12-31T23:59:59Z          # Désactiver après date
  - Between=2024-01-01T00:00:00Z,2024-12-31T23:59:59Z  # Entre deux dates
```

**Complete example - Sophisticated routing:**
```yaml
routes:
  # Route 1: API v2 pour clients premium
  - id: premium-v2-api
    uri: lb://PREMIUM-SERVICE-V2
    predicates:
      - Path=/api/v2/**
      - Header=X-User-Type, premium
      - Method=GET,POST
      - After=2024-01-01T00:00:00Z
    filters:
      - RewritePath=/api/v2/(?<segment>.*), /${segment}
  
  # Route 2: API v1 pour clients standard
  - id: standard-v1-api
    uri: lb://STANDARD-SERVICE-V1
    predicates:
      - Path=/api/v1/**
      - Header=X-User-Type, standard
    filters:
      - RewritePath=/api/v1/(?<segment>.*), /${segment}
```

**Combining Predicates (AND logic):**
```yaml
predicates:
  - Path=/api/orders/**
  - Method=POST
  - Header=Content-Type, application/json
# Tous doivent matcher (AND)
```

---

**Q8: What is the RewritePath Filter and how do you use it?**

**A:** 

**RewritePath** is a filter that **transforms the request path** before routing it to the microservice.

**Problem:** 
```
Client appelle:        Gateway → /api/users/123
Service attend:        /users/123 (sans /api)
→ Sans RewritePath → 404 Not Found
```

**Solution: RewritePath Filter**

**Syntax:**
```yaml
filters:
  - RewritePath=<regex>, <replacement>
```

**Practical examples:**

**1. Remove /api prefix:**
```yaml
routes:
  - id: user-service
    uri: lb://USER-SERVICE
    predicates:
      - Path=/api/users/**
    filters:
      - RewritePath=/api/users/(?<segment>.*), /${segment}

# Transformation:
# /api/users/123 → /users/123
# /api/users/123/orders → /users/123/orders
```

**2. Add version prefix:**
```yaml
routes:
  - id: product-service-v2
    uri: lb://PRODUCT-SERVICE
    predicates:
      - Path=/api/products/**
    filters:
      - RewritePath=/api/products/(?<segment>.*), /v2/products/${segment}

# Transformation:
# /api/products/123 → /v2/products/123
```

**3. Completely replace the path:**
```yaml
routes:
  - id: legacy-api
    uri: http://legacy-system.com
    predicates:
      - Path=/new/api/users/(?<id>.*)
    filters:
      - RewritePath=/new/api/users/(?<id>.*), /legacy/user?userId=${id}

# Transformation:
# /new/api/users/123 → /legacy/user?userId=123
```

**4. Combine with StripPrefix:**
```yaml
routes:
  - id: order-service
    uri: lb://ORDER-SERVICE
    predicates:
      - Path=/gateway/api/orders/**
    filters:
      - StripPrefix=2  # Enlève 2 premiers segments

# Transformation:
# /gateway/api/orders/123 → /orders/123
# (enlève /gateway et /api)
```

**5. Conditional routing with version:**
```yaml
routes:
  # V1 API
  - id: api-v1
    uri: lb://SERVICE-V1
    predicates:
      - Path=/api/v1/users/**
    filters:
      - RewritePath=/api/v1/users/(?<segment>.*), /users/${segment}
  
  # V2 API
  - id: api-v2
    uri: lb://SERVICE-V2
    predicates:
      - Path=/api/v2/users/**
    filters:
      - RewritePath=/api/v2/users/(?<segment>.*), /users/${segment}

# /api/v1/users/123 → SERVICE-V1 → /users/123
# /api/v2/users/123 → SERVICE-V2 → /users/123
```

**In Java Code:**
```java
@Bean
public RouteLocator rewriteRoutes(RouteLocatorBuilder builder) {
    return builder.routes()
        .route("user-service", r -> r
            .path("/api/users/**")
            .filters(f -> f
                .rewritePath("/api/users/(?<segment>.*)", "/${segment}")
            )
            .uri("lb://USER-SERVICE")
        )
        
        // Réécriture avec capture de multiples groupes
        .route("search-service", r -> r
            .path("/api/search/{category}/{query}")
            .filters(f -> f
                .rewritePath(
                    "/api/search/(?<category>.*)/(?<query>.*)",
                    "/search?cat=${category}&q=${query}"
                )
            )
            .uri("lb://SEARCH-SERVICE")
        )
        
        .build();
}
```

**Regex Capture Groups:**
- `(?<name>.*)` : Named capture group (recommandé)
- `${name}`: Reference to the captured group
- `.*`: Match any character

---

**Q9: What is the difference between uri: http://... and uri: lb://... ?**

**A:** 

**http:// (Direct HTTP Call)**

```yaml
routes:
  - id: external-api
    uri: http://external-service.com:8080
    predicates:
      - Path=/external/**
```

**How it works:**
```
Client → Gateway → Direct HTTP call → http://external-service.com:8080
```

**Characteristics:**
- ❌ No load balancing
- ❌ Pas de service discovery
- ❌ URL hardcodée
- ❌ No automatic failover
- ✅ Simple for external services
- ✅ Pas besoin d'Eureka

**Use cases:**
- External third-party services (public APIs)
- Legacy non-Eureka services
- Services with a fixed URL

---

**lb:// (Load Balanced via Service Discovery)**

```yaml
routes:
  - id: user-service
    uri: lb://USER-SERVICE  # Nom du service dans Eureka
    predicates:
      - Path=/api/users/**
```

**How it works:**
```
1. Client → Gateway → /api/users/123
2. Gateway → "Je dois router vers lb://USER-SERVICE"
3. Gateway → Contacte Eureka → "Donne-moi les instances de USER-SERVICE"
4. Eureka → Répond: [instance1:8081, instance2:8082, instance3:8083]
5. LoadBalancer (Round Robin) → Choisit instance2:8082
6. Gateway → Forward requête → http://instance2:8082/users/123
7. Si instance2 down → Retry avec instance1 ou instance3
```

**Characteristics:**
- ✅ **Automatic load balancing** (Round Robin, Random, Weighted)
- ✅ **Dynamic service discovery** (via Eureka, Consul, Zookeeper)
- ✅ **Failover**: if instance is down, uses another
- ✅ **Transparent scaling**: add instances without changing config
- ✅ **Health checks**: unhealthy instances automatically excluded
- ❌ Requires Eureka (or another service registry)

**Required Eureka Configuration:**
```yaml
eureka:
  client:
    service-url:
      defaultZone: http://localhost:8761/eureka/
    fetch-registry: true      # Récupérer liste services
    register-with-eureka: true # S'enregistrer comme client
```

---

**Direct comparison:**

```yaml
# Scénario 1: Service externe (Stripe API)
- id: stripe-api
  uri: http://api.stripe.com
  # ✅ Bon choix : service externe, URL fixe

# Scénario 2: Microservice interne
- id: user-service
  uri: lb://USER-SERVICE
  # ✅ Bon choix : load balancing + failover

# ❌ MAUVAIS : Microservice avec URL hardcodée
- id: user-service-bad
  uri: http://user-service-instance1:8081
  # Problème : pas de load balancing, si instance down → tout casse
```

---

**Available Load Balancing Algorithms:**
```java
@Configuration
public class LoadBalancerConfig {
    
    // Round Robin (défaut)
    @Bean
    public ReactorLoadBalancer<ServiceInstance> roundRobinLoadBalancer(
            Environment environment,
            LoadBalancerClientFactory loadBalancerClientFactory
    ) {
        String name = environment.getProperty(LoadBalancerClientFactory.PROPERTY_NAME);
        return new RoundRobinLoadBalancer(
            loadBalancerClientFactory.getLazyProvider(name, ServiceInstanceListSupplier.class),
            name
        );
    }
    
    // Random
    @Bean
    public ReactorLoadBalancer<ServiceInstance> randomLoadBalancer() {
        return new RandomLoadBalancer(...);
    }
}
```

---

**Q10: How do you integrate Eureka with Spring Cloud Gateway?**

**A:** 

**Architecture:**
```
Eureka Server (Port 8761)
    ↑
    ├─ Gateway Server (Port 8080) → S'enregistre + Fetch registry
    ├─ User Service (8081, 8082, 8083) → S'enregistrent
    ├─ Order Service (8091, 8092) → S'enregistrent
    └─ Product Service (8101) → S'enregistre
```

**Complete configuration:**

**1. Dependencies (pom.xml):**
```xml
<dependencies>
    <!-- Spring Cloud Gateway -->
    <dependency>
        <groupId>org.springframework.cloud</groupId>
        <artifactId>spring-cloud-starter-gateway</artifactId>
    </dependency>
    
    <!-- Eureka Client -->
    <dependency>
        <groupId>org.springframework.cloud</groupId>
        <artifactId>spring-cloud-starter-netflix-eureka-client</artifactId>
    </dependency>
</dependencies>
```

**2. Application Class:**
```java
@SpringBootApplication
@EnableDiscoveryClient  // Active Eureka client
public class GatewayServerApplication {
    public static void main(String[] args) {
        SpringApplication.run(GatewayServerApplication.class, args);
    }
}
```

**3. application.yml:**
```yaml
spring:
  application:
    name: gateway-server
  
  cloud:
    gateway:
      # Option 1: Auto-discovery (crée routes automatiquement)
      discovery:
        locator:
          enabled: true                    # Active auto-routing
          lower-case-service-id: true      # user-service au lieu de USER-SERVICE
      
      # Option 2: Routes manuelles (plus de contrôle)
      routes:
        - id: user-service
          uri: lb://USER-SERVICE  # lb:// = load balanced via Eureka
          predicates:
            - Path=/api/users/**
          filters:
            - RewritePath=/api/users/(?<segment>.*), /${segment}
        
        - id: order-service
          uri: lb://ORDER-SERVICE
          predicates:
            - Path=/api/orders/**
          filters:
            - RewritePath=/api/orders/(?<segment>.*), /${segment}
        
        - id: product-service
          uri: lb://PRODUCT-SERVICE
          predicates:
            - Path=/api/products/**
          filters:
            - RewritePath=/api/products/(?<segment>.*), /${segment}

# Eureka Client Configuration
eureka:
  client:
    service-url:
      defaultZone: http://localhost:8761/eureka/  # Eureka Server URL
    fetch-registry: true                          # Récupérer liste des services
    register-with-eureka: true                    # S'enregistrer auprès d'Eureka
    registry-fetch-interval-seconds: 5            # Fréquence de refresh (5s)
  instance:
    prefer-ip-address: true                       # Utiliser IP au lieu de hostname
    instance-id: ${spring.application.name}:${random.value}  # ID unique
    lease-renewal-interval-in-seconds: 5          # Heartbeat toutes les 5s
    lease-expiration-duration-in-seconds: 10      # Considéré down après 10s

server:
  port: 8080

logging:
  level:
    org.springframework.cloud.gateway: DEBUG
    com.netflix.eureka: DEBUG
```

---

**4. Auto-Discovery vs Manual Routes:**

**Option A: Auto-Discovery (discovery.locator.enabled: true)**

Gateway automatically creates routes for all Eureka services:
```
USER-SERVICE → http://gateway:8080/user-service/**
ORDER-SERVICE → http://gateway:8080/order-service/**
PRODUCT-SERVICE → http://gateway:8080/product-service/**
```

**Example:**
```bash
# Service USER-SERVICE expose /users/123
# Via Gateway avec auto-discovery:
curl http://localhost:8080/user-service/users/123
```

**Advantages:**
- ✅ Zero configuration
- ✅ New services automatically routed
- ✅ Simple for dev/test

**Disadvantages:**
- ❌ No control over paths
- ❌ No custom filters
- ❌ URL contains service name (exposes internal architecture)

---

**Option B: Manual Routes (recommended in production)**

```yaml
routes:
  - id: user-service
    uri: lb://USER-SERVICE
    predicates:
      - Path=/api/users/**
    filters:
      - RewritePath=/api/users/(?<segment>.*), /${segment}
      - name: CircuitBreaker
        args:
          name: userServiceCB
      - name: RequestRateLimiter
```

**Advantages:**
- ✅ Full control over routing
- ✅ Custom filters
- ✅ Clean paths (/api/users instead of /user-service)
- ✅ Enhanced security (hidden architecture)

---

**5. Verify Eureka integration:**

**Eureka Dashboard : http://localhost:8761**
```
Instances currently registered with Eureka:
- GATEWAY-SERVER (1 instance)
  - gateway-server:random-id → UP (1) - localhost:8080
- USER-SERVICE (3 instances)
  - user-service:8081 → UP (1)
  - user-service:8082 → UP (1)
  - user-service:8083 → UP (1)
- ORDER-SERVICE (2 instances)
  - order-service:8091 → UP (1)
  - order-service:8092 → UP (1)
```

**Gateway Logs (at startup):**
```
DiscoveryClient_GATEWAY-SERVER - registration status: 204
Fetched 3 services from Eureka: [USER-SERVICE, ORDER-SERVICE, PRODUCT-SERVICE]
Mapped [/api/users/**] onto lb://USER-SERVICE
```

---

**6. Test Load Balancing:**

```bash
# Lancer 3 instances USER-SERVICE
java -jar user-service.jar --server.port=8081
java -jar user-service.jar --server.port=8082
java -jar user-service.jar --server.port=8083

# Faire plusieurs requêtes via Gateway
for i in {1..9}; do
  curl http://localhost:8080/api/users/123
done

# Gateway distribue (Round Robin):
# Request 1 → user-service:8081
# Request 2 → user-service:8082
# Request 3 → user-service:8083
# Request 4 → user-service:8081 (recommence)
# ...
```

---

**7. Automatic failover:**

```bash
# Scénario : 3 instances UP
curl http://localhost:8080/api/users/123  # → instance1:8081 ✅

# Shutdown instance1
kill <PID_8081>

# Eureka détecte (après 10s)
# Gateway refresh registry (toutes les 5s)

# Nouvelles requêtes vont vers instances UP uniquement
curl http://localhost:8080/api/users/123  # → instance2:8082 ✅
curl http://localhost:8080/api/users/123  # → instance3:8083 ✅
# instance1 n'est plus appelée
```

---

### **Questions about Gateway Filters**

**Q11: What are the different types of filters in Spring Cloud Gateway?**

**A:** 

There are **3 types of filters**:

**1. Global Filters (applied to ALL routes)**

```java
@Component
public class LoggingGlobalFilter implements GlobalFilter, Ordered {
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        // S'applique à TOUTES les requêtes
        logger.info("Request: {} {}", 
            exchange.getRequest().getMethod(),
            exchange.getRequest().getURI()
        );
        
        return chain.filter(exchange);
    }
    
    @Override
    public int getOrder() {
        return -1; // Ordre d'exécution (négatif = priorité haute)
    }
}
```

**Use cases:**
- Logging all requests/responses
- Authentication/Authorization
- Correlation ID injection
- Metrics collection
- Security headers

---

**2. GatewayFilter (applied to specific routes)**

```java
@Component
public class AuthenticationFilter implements GatewayFilter, Ordered {
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        // S'applique uniquement aux routes où ce filter est configuré
        String token = exchange.getRequest().getHeaders().getFirst("Authorization");
        
        if (!jwtUtil.validateToken(token)) {
            exchange.getResponse().setStatusCode(HttpStatus.UNAUTHORIZED);
            return exchange.getResponse().setComplete();
        }
        
        return chain.filter(exchange);
    }
    
    @Override
    public int getOrder() {
        return 0;
    }
}
```

**Configuration:**
```java
@Bean
public RouteLocator routes(RouteLocatorBuilder builder) {
    return builder.routes()
        .route("secure-route", r -> r
            .path("/api/secure/**")
            .filters(f -> f.filter(new AuthenticationFilter())) // Filter spécifique
            .uri("lb://SECURE-SERVICE")
        )
        .build();
}
```

---

**3. GatewayFilterFactory (for YAML configuration)**

```java
@Component
public class CustomHeaderGatewayFilterFactory 
        extends AbstractGatewayFilterFactory<CustomHeaderGatewayFilterFactory.Config> {
    
    public CustomHeaderGatewayFilterFactory() {
        super(Config.class);
    }
    
    @Override
    public GatewayFilter apply(Config config) {
        return (exchange, chain) -> {
            ServerHttpRequest modifiedRequest = exchange.getRequest().mutate()
                .header(config.getHeaderName(), config.getHeaderValue())
                .build();
            
            return chain.filter(exchange.mutate().request(modifiedRequest).build());
        };
    }
    
    @Override
    public List<String> shortcutFieldOrder() {
        return Arrays.asList("headerName", "headerValue");
    }
    
    public static class Config {
        private String headerName;
        private String headerValue;
        // Getters & Setters
    }
}
```

**YAML Configuration:**
```yaml
filters:
  # Syntaxe longue
  - name: CustomHeader
    args:
      headerName: X-Custom-Header
      headerValue: MyValue
  
  # Syntaxe courte (via shortcutFieldOrder)
  - CustomHeader=X-Custom-Header, MyValue
```

---

**Built-in Filters (predefined):**

**Request Transformation:**
```yaml
filters:
  - AddRequestHeader=X-Request-Id, ${requestId}
  - AddRequestParameter=source, gateway
  - RemoveRequestHeader=X-Sensitive-Data
  - SetRequestHeader=X-Forwarded-For, ${clientIp}
  - PrefixPath=/api/v1
  - RewritePath=/api/(?<segment>.*), /${segment}
  - StripPrefix=1
```

**Response Transformation:**
```yaml
filters:
  - AddResponseHeader=X-Response-Time, 150ms
  - RemoveResponseHeader=X-Internal-Info
  - SetResponseHeader=Cache-Control, no-cache
  - DedupeResponseHeader=Access-Control-Allow-Origin
```

**Resilience:**
```yaml
filters:
  - name: CircuitBreaker
    args:
      name: myCircuitBreaker
      fallbackUri: forward:/fallback
  
  - name: Retry
    args:
      retries: 3
      statuses: BAD_GATEWAY
```

**Security:**
```yaml
filters:
  - name: RequestRateLimiter
    args:
      redis-rate-limiter.replenishRate: 10
      redis-rate-limiter.burstCapacity: 20
```

---

**Q12: How do you implement a Custom Global Filter for authentication?**

**A:** 

**Scenario:** Verify JWT token for all requests (except public routes).

```java
@Component
@Order(1)  // S'exécute en premier
public class AuthenticationGlobalFilter implements GlobalFilter {
    
    private static final Logger logger = LoggerFactory.getLogger(AuthenticationGlobalFilter.class);
    
    @Autowired
    private JwtUtil jwtUtil;
    
    // Routes publiques (pas d'auth requise)
    private static final List<String> PUBLIC_PATHS = Arrays.asList(
        "/auth/login",
        "/auth/register",
        "/public",
        "/actuator/health"
    );
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = exchange.getRequest();
        String path = request.getPath().value();
        
        // Vérifier si route publique
        if (isPublicPath(path)) {
            logger.info("Public path accessed: {}", path);
            return chain.filter(exchange);
        }
        
        // Extraire Authorization header
        String authHeader = request.getHeaders().getFirst(HttpHeaders.AUTHORIZATION);
        
        if (authHeader == null || !authHeader.startsWith("Bearer ")) {
            logger.warn("Missing or invalid Authorization header for path: {}", path);
            return onError(exchange, "Missing authorization token", HttpStatus.UNAUTHORIZED);
        }
        
        String token = authHeader.substring(7);
        
        try {
            // Valider JWT
            if (!jwtUtil.validateToken(token)) {
                logger.warn("Invalid JWT token for path: {}", path);
                return onError(exchange, "Invalid or expired token", HttpStatus.UNAUTHORIZED);
            }
            
            // Extraire user info
            String userId = jwtUtil.extractUserId(token);
            String username = jwtUtil.extractUsername(token);
            List<String> roles = jwtUtil.extractRoles(token);
            
            logger.info("Authenticated user: {} (ID: {}) accessing: {}", username, userId, path);
            
            // Ajouter user info aux headers pour downstream services
            ServerHttpRequest modifiedRequest = request.mutate()
                .header("X-User-Id", userId)
                .header("X-Username", username)
                .header("X-User-Roles", String.join(",", roles))
                .build();
            
            // Continuer avec requête modifiée
            return chain.filter(exchange.mutate().request(modifiedRequest).build());
            
        } catch (ExpiredJwtException e) {
            logger.error("JWT token expired for path: {}", path);
            return onError(exchange, "Token expired", HttpStatus.UNAUTHORIZED);
        } catch (Exception e) {
            logger.error("JWT validation error: {}", e.getMessage());
            return onError(exchange, "Token validation failed", HttpStatus.UNAUTHORIZED);
        }
    }
    
    private boolean isPublicPath(String path) {
        return PUBLIC_PATHS.stream().anyMatch(path::startsWith);
    }
    
    private Mono<Void> onError(ServerWebExchange exchange, String message, HttpStatus status) {
        ServerHttpResponse response = exchange.getResponse();
        response.setStatusCode(status);
        response.getHeaders().setContentType(MediaType.APPLICATION_JSON);
        
        ErrorResponse errorResponse = new ErrorResponse(
            status.value(),
            message,
            Instant.now().toString()
        );
        
        try {
            byte[] bytes = new ObjectMapper().writeValueAsBytes(errorResponse);
            DataBuffer buffer = response.bufferFactory().wrap(bytes);
            return response.writeWith(Mono.just(buffer));
        } catch (Exception e) {
            return response.setComplete();
        }
    }
}

@Data
@AllArgsConstructor
class ErrorResponse {
    private int status;
    private String message;
    private String timestamp;
}
```

**JwtUtil (helper):**
```java
@Component
public class JwtUtil {
    
    @Value("${jwt.secret}")
    private String secret;
    
    private Key getSigningKey() {
        byte[] keyBytes = Decoders.BASE64.decode(secret);
        return Keys.hmacShaKeyFor(keyBytes);
    }
    
    public boolean validateToken(String token) {
        try {
            Jwts.parserBuilder()
                .setSigningKey(getSigningKey())
                .build()
                .parseClaimsJws(token);
            return true;
        } catch (Exception e) {
            return false;
        }
    }
    
    public String extractUserId(String token) {
        Claims claims = extractAllClaims(token);
        return claims.get("userId", String.class);
    }
    
    public String extractUsername(String token) {
        return extractAllClaims(token).getSubject();
    }
    
    public List<String> extractRoles(String token) {
        Claims claims = extractAllClaims(token);
        return claims.get("roles", List.class);
    }
    
    private Claims extractAllClaims(String token) {
        return Jwts.parserBuilder()
            .setSigningKey(getSigningKey())
            .build()
            .parseClaimsJws(token)
            .getBody();
    }
}
```

**Test:**
```bash
# Sans token → 401
curl http://localhost:8080/api/users/123

# Avec token invalide → 401
curl -H "Authorization: Bearer invalid-token" \
     http://localhost:8080/api/users/123

# Avec token valide → 200 + User info headers ajoutés
curl -H "Authorization: Bearer eyJhbGc..." \
     http://localhost:8080/api/users/123

# Route publique (pas de token nécessaire) → 200
curl http://localhost:8080/auth/login
```

---

**Q13: How do you implement Rate Limiting with Redis?**

**A:** 

**Dependencies:**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-data-redis-reactive</artifactId>
</dependency>
```

**Redis Configuration:**
```yaml
spring:
  redis:
    host: localhost
    port: 6379
    password: # si configuré
```

**Rate Limiter Configuration:**
```yaml
spring:
  cloud:
    gateway:
      routes:
        - id: user-service
          uri: lb://USER-SERVICE
          predicates:
            - Path=/api/users/**
          filters:
            - name: RequestRateLimiter
              args:
                redis-rate-limiter.replenishRate: 10    # 10 tokens/seconde
                redis-rate-limiter.burstCapacity: 20    # Max 20 tokens stockés
                redis-rate-limiter.requestedTokens: 1   # 1 token par requête
                key-resolver: "#{@userKeyResolver}"     # Bean pour résoudre clé
```

**Key Resolvers (limiting strategies):**

**1. Rate limit by IP:**
```java
@Configuration
public class RateLimiterConfig {
    
    @Bean
    public KeyResolver ipKeyResolver() {
        return exchange -> Mono.just(
            exchange.getRequest()
                .getRemoteAddress()
                .getAddress()
                .getHostAddress()
        );
    }
}
```

**2. Rate limit by User ID:**
```java
@Bean
public KeyResolver userKeyResolver() {
    return exchange -> Mono.justOrEmpty(
        exchange.getRequest()
            .getHeaders()
            .getFirst("X-User-Id")
    );
}
```

**3. Rate limit by API Key:**
```java
@Bean
public KeyResolver apiKeyResolver() {
    return exchange -> Mono.justOrEmpty(
        exchange.getRequest()
            .getHeaders()
            .getFirst("X-API-Key")
    );
}
```

**4. Combined rate limit (User + Endpoint):**
```java
@Bean
public KeyResolver compositeKeyResolver() {
    return exchange -> {
        String userId = exchange.getRequest().getHeaders().getFirst("X-User-Id");
        String path = exchange.getRequest().getPath().value();
        
        return Mono.just(userId + ":" + path);
    };
}
```

---

**Token Bucket Algorithm:**

```
Bucket Capacity: 20 tokens
Replenish Rate: 10 tokens/seconde

t=0s:  Bucket = 20 tokens (plein)
User fait 15 requêtes → Bucket = 5 tokens
t=1s:  Bucket = 5 + 10 = 15 tokens (replenish)
User fait 20 requêtes → Bucket = 0 tokens (15 passent, 5 rejetées)
t=2s:  Bucket = 0 + 10 = 10 tokens
```

**Response when limit is reached:**
```
HTTP/1.1 429 Too Many Requests
X-RateLimit-Remaining: 0
X-RateLimit-Burst-Capacity: 20
X-RateLimit-Replenish-Rate: 10
Retry-After: 1
```

---

**Advanced configuration - Different limits per endpoint:**

```yaml
routes:
  # Endpoint public - limite stricte
  - id: public-api
    uri: lb://PUBLIC-SERVICE
    predicates:
      - Path=/api/public/**
    filters:
      - name: RequestRateLimiter
        args:
          redis-rate-limiter.replenishRate: 5
          redis-rate-limiter.burstCapacity: 10
          key-resolver: "#{@ipKeyResolver}"
  
  # Endpoint authentifié - limite généreuse
  - id: authenticated-api
    uri: lb://USER-SERVICE
    predicates:
      - Path=/api/users/**
    filters:
      - name: RequestRateLimiter
        args:
          redis-rate-limiter.replenishRate: 50
          redis-rate-limiter.burstCapacity: 100
          key-resolver: "#{@userKeyResolver}"
  
  # Endpoint premium - pas de limite
  - id: premium-api
    uri: lb://PREMIUM-SERVICE
    predicates:
      - Path=/api/premium/**
      - Header=X-User-Tier, premium
    # Pas de RequestRateLimiter
```

---

**Custom Rate Limiter Response:**

```java
@Component
public class CustomRateLimiterFilter implements GlobalFilter, Ordered {
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        return chain.filter(exchange).onErrorResume(throwable -> {
            if (throwable instanceof RateLimitExceededException) {
                ServerHttpResponse response = exchange.getResponse();
                response.setStatusCode(HttpStatus.TOO_MANY_REQUESTS);
                response.getHeaders().setContentType(MediaType.APPLICATION_JSON);
                
                String body = "{\"error\":\"Rate limit exceeded\",\"retryAfter\":\"1 second\"}";
                DataBuffer buffer = response.bufferFactory().wrap(body.getBytes());
                
                return response.writeWith(Mono.just(buffer));
            }
            return Mono.error(throwable);
        });
    }
    
    @Override
    public int getOrder() {
        return -1;
    }
}
```

---

**Q14: How do you implement Circuit Breaker with Resilience4j?**

**A:** 

**Dependencies:**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-circuitbreaker-reactor-resilience4j</artifactId>
</dependency>
```

**Configuration:**
```yaml
spring:
  cloud:
    gateway:
      routes:
        - id: user-service
          uri: lb://USER-SERVICE
          predicates:
            - Path=/api/users/**
          filters:
            - name: CircuitBreaker
              args:
                name: userServiceCB
                fallbackUri: forward:/fallback/users
            - name: Retry
              args:
                retries: 3
                statuses: BAD_GATEWAY, INTERNAL_SERVER_ERROR
                methods: GET
                backoff:
                  firstBackoff: 100ms
                  maxBackoff: 1000ms
                  factor: 2
                  basedOnPreviousValue: true

# Resilience4j Configuration
resilience4j:
  circuitbreaker:
    instances:
      userServiceCB:
        sliding-window-size: 10                    # Fenêtre de 10 requêtes
        failure-rate-threshold: 50                 # 50% d'échecs → OPEN
        wait-duration-in-open-state: 10s           # Attendre 10s avant HALF-OPEN
        permitted-number-of-calls-in-half-open-state: 3  # Tester avec 3 requêtes
        automatic-transition-from-open-to-half-open-enabled: true
        minimum-number-of-calls: 5                 # Min 5 appels avant calcul
        slow-call-duration-threshold: 3s           # > 3s = slow call
        slow-call-rate-threshold: 60               # 60% slow calls → OPEN
  
  timelimiter:
    instances:
      userServiceCB:
        timeout-duration: 3s                       # Timeout après 3s
```

**Circuit Breaker States:**

```
CLOSED (Normal)
  ↓
10 requêtes: 6 échecs (60% > 50%)
  ↓
OPEN (Service considéré DOWN)
  ↓ (attendre 10s)
HALF-OPEN (Test si service recovered)
  ↓
3 requêtes de test:
  - Si toutes réussissent → CLOSED
  - Si une échoue → OPEN (retour)
```

**Fallback Controller:**
```java
@RestController
public class FallbackController {
    
    @GetMapping("/fallback/users")
    public ResponseEntity<Map<String, Object>> userServiceFallback(
            ServerWebExchange exchange
    ) {
        // Retrieve the exception if available
        Throwable exception = exchange.getAttribute("executionException");
        
        Map<String, Object> response = new HashMap<>();
        response.put("status", "SERVICE_UNAVAILABLE");
        response.put("message", "User service is temporarily unavailable");
        response.put("timestamp", Instant.now());
        
        if (exception != null) {
            response.put("error", exception.getMessage());
        }
        
        // Return cached or default data
        response.put("data", getCachedOrDefaultData());
        
        return ResponseEntity
            .status(HttpStatus.SERVICE_UNAVAILABLE)
            .body(response);
    }
    
    @GetMapping("/fallback/orders")
    public ResponseEntity<Map<String, Object>> orderServiceFallback() {
        Map<String, Object> response = new HashMap<>();
        response.put("status", "SERVICE_UNAVAILABLE");
        response.put("message", "Order service is temporarily unavailable");
        response.put("retryAfter", "30 seconds");
        
        return ResponseEntity
            .status(HttpStatus.SERVICE_UNAVAILABLE)
            .body(response);
    }
    
    private Object getCachedOrDefaultData() {
        // Retourner données depuis cache Redis si disponibles
        // Ou données par défaut
        return Map.of("users", Collections.emptyList());
    }
}
```

---

**Circuit Breaker Monitoring:**

```java
@RestController
@RequestMapping("/actuator/circuitbreakers")
public class CircuitBreakerMonitoringController {
    
    @Autowired
    private CircuitBreakerRegistry circuitBreakerRegistry;
    
    @GetMapping
    public Map<String, Object> getAllCircuitBreakers() {
        Map<String, Object> result = new HashMap<>();
        
        circuitBreakerRegistry.getAllCircuitBreakers().forEach(cb -> {
            CircuitBreaker.Metrics metrics = cb.getMetrics();
            
            Map<String, Object> cbInfo = new HashMap<>();
            cbInfo.put("state", cb.getState().toString());
            cbInfo.put("failureRate", metrics.getFailureRate());
            cbInfo.put("slowCallRate", metrics.getSlowCallRate());
            cbInfo.put("numberOfSuccessfulCalls", metrics.getNumberOfSuccessfulCalls());
            cbInfo.put("numberOfFailedCalls", metrics.getNumberOfFailedCalls());
            cbInfo.put("numberOfSlowCalls", metrics.getNumberOfSlowCalls());
            
            result.put(cb.getName(), cbInfo);
        });
        
        return result;
    }
    
    @PostMapping("/{name}/reset")
    public ResponseEntity<String> resetCircuitBreaker(@PathVariable String name) {
        circuitBreakerRegistry.circuitBreaker(name).reset();
        return ResponseEntity.ok("Circuit breaker reset: " + name);
    }
}
```

**Response monitoring:**
```bash
curl http://localhost:8080/actuator/circuitbreakers

{
  "userServiceCB": {
    "state": "CLOSED",
    "failureRate": 0.0,
    "slowCallRate": 0.0,
    "numberOfSuccessfulCalls": 25,
    "numberOfFailedCalls": 0,
    "numberOfSlowCalls": 0
  },
  "orderServiceCB": {
    "state": "OPEN",
    "failureRate": 100.0,
    "numberOfSuccessfulCalls": 0,
    "numberOfFailedCalls": 10
  }
}
```

---

### **Questions about Cross-Cutting Concerns**

**Q15: What are Cross-Cutting Concerns and which ones do you manage in the Gateway?**

**A:** 

**Cross-Cutting Concerns** are features **common to all microservices** that are centralized in the Gateway to avoid duplication and simplify maintenance.

**The main Cross-Cutting Concerns managed in the Gateway:**

**1. Authentication & Authorization**
```java
@Component
@Order(1)
public class AuthenticationFilter implements GlobalFilter {
    // Vérifier JWT pour toutes requêtes
    // Ajouter user info aux headers
}
```

**2. Logging & Monitoring**
```java
@Component
@Order(2)
public class LoggingFilter implements GlobalFilter {
    // Logger toutes requêtes/réponses
    // Générer correlation IDs
    // Calculer temps de réponse
}
```

**3. Rate Limiting**
```yaml
filters:
  - RequestRateLimiter  # Limiter abus
```

**4. CORS Handling**
```yaml
globalcors:
  cors-configurations:
    '[/**]':
      allowed-origins: ["http://localhost:3000"]
```

**5. Security Headers**
```java
response.getHeaders().add("X-Content-Type-Options", "nosniff");
response.getHeaders().add("X-Frame-Options", "DENY");
response.getHeaders().add("X-XSS-Protection", "1; mode=block");
```

**6. Request/Response Transformation**
```yaml
filters:
  - RewritePath
  - AddRequestHeader
  - AddResponseHeader
```

**7. Circuit Breaking & Resilience**
```yaml
filters:
  - CircuitBreaker
  - Retry
```

**8. Caching**
```java
// Cache réponses GET fréquentes
```

**9. Request Validation**
```java
// Valider headers, payload size, API version
```

**10. Metrics & Tracing**
```java
// Prometheus metrics
// Distributed tracing (Zipkin/Jaeger)
```

---

**Advantages of centralizing in Gateway:**
- ✅ **DRY**: Code written once
- ✅ **Maintenance**: Change in one place
- ✅ **Consistency**: Uniform behavior
- ✅ **Performance**: Global optimizations
- ✅ **Security**: Defense in depth

**Disadvantages:**
- ❌ **Single Point of Failure**: Gateway down = everything down (→ need HA)
- ❌ **Potential bottleneck**: Everything goes through Gateway (→ horizontal scaling)
- ❌ **Added latency**: Extra hop (minimal with reactive)

---

**Q16: How do you implement Correlation ID to trace distributed requests?**

**A:** 

**Problem:**
```
Client → Gateway → User Service → Order Service → Payment Service

Error in Payment Service → How to trace the entire log chain?
```

**Solution: Correlation ID**

A unique ID generated at the Gateway and propagated to all services.

**Implementation:**

```java
@Component
@Order(0)  // S'exécute en premier
public class CorrelationIdFilter implements GlobalFilter {
    
    private static final Logger logger = LoggerFactory.getLogger(CorrelationIdFilter.class);
    private static final String CORRELATION_ID_HEADER = "X-Correlation-Id";
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = exchange.getRequest();
        
        // 1. Vérifier si Correlation ID existe déjà (requête en cascade)
        String correlationId = request.getHeaders().getFirst(CORRELATION_ID_HEADER);
        
        // 2. Si pas de Correlation ID, en générer un nouveau
        if (correlationId == null || correlationId.isEmpty()) {
            correlationId = UUID.randomUUID().toString();
            logger.info("Generated new Correlation ID: {}", correlationId);
        } else {
            logger.info("Reusing existing Correlation ID: {}", correlationId);
        }
        
        // 3. Stocker dans exchange attributes pour accès ultérieur
        exchange.getAttributes().put("correlationId", correlationId);
        
        // 4. Ajouter aux headers de la requête vers downstream services
        ServerHttpRequest modifiedRequest = request.mutate()
            .header(CORRELATION_ID_HEADER, correlationId)
            .build();
        
        // 5. Logger la requête avec Correlation ID
        logger.info("[{}] Request: {} {} from {}",
            correlationId,
            request.getMethod(),
            request.getURI(),
            request.getRemoteAddress()
        );
        
        // 6. Continuer la chaîne
        return chain.filter(exchange.mutate().request(modifiedRequest).build())
            .then(Mono.fromRunnable(() -> {
                // 7. Ajouter Correlation ID à la réponse
                ServerHttpResponse response = exchange.getResponse();
                response.getHeaders().add(CORRELATION_ID_HEADER, correlationId);
                
                // 8. Logger la réponse
                logger.info("[{}] Response: Status {} - Path: {}",
                    correlationId,
                    response.getStatusCode(),
                    request.getPath()
                );
            }));
    }
    
    @Override
    public int getOrder() {
        return Ordered.HIGHEST_PRECEDENCE; // Priorité maximale
    }
}
```

**In Microservices (User Service, Order Service, etc.):**

```java
@Component
public class CorrelationIdInterceptor implements HandlerInterceptor {
    
    private static final Logger logger = LoggerFactory.getLogger(CorrelationIdInterceptor.class);
    private static final String CORRELATION_ID_HEADER = "X-Correlation-Id";
    
    @Override
    public boolean preHandle(HttpServletRequest request, 
                            HttpServletResponse response, 
                            Object handler) {
        
        // Extraire Correlation ID depuis header
        String correlationId = request.getHeader(CORRELATION_ID_HEADER);
        
        if (correlationId != null) {
            // Stocker dans MDC pour logging automatique
            MDC.put("correlationId", correlationId);
            
            // Ajouter à la réponse
            response.setHeader(CORRELATION_ID_HEADER, correlationId);
            
            logger.info("[{}] Received request in User Service: {} {}",
                correlationId,
                request.getMethod(),
                request.getRequestURI()
            );
        }
        
        return true;
    }
    
    @Override
    public void afterCompletion(HttpServletRequest request, 
                               HttpServletResponse response, 
                               Object handler, 
                               Exception ex) {
        // Nettoyer MDC après traitement
        MDC.clear();
    }
}

// Configuration
@Configuration
public class WebMvcConfig implements WebMvcConfigurer {
    
    @Autowired
    private CorrelationIdInterceptor correlationIdInterceptor;
    
    @Override
    public void addInterceptors(InterceptorRegistry registry) {
        registry.addInterceptor(correlationIdInterceptor);
    }
}
```

**Logback Configuration to include Correlation ID:**

```xml
<!-- logback-spring.xml -->
<configuration>
    <appender name="CONSOLE" class="ch.qos.logback.core.ConsoleAppender">
        <encoder>
            <pattern>
                %d{yyyy-MM-dd HH:mm:ss} [%thread] %-5level [%X{correlationId}] %logger{36} - %msg%n
            </pattern>
        </encoder>
    </appender>
    
    <root level="INFO">
        <appender-ref ref="CONSOLE"/>
    </root>
</configuration>
```

**Propagation between services (RestTemplate/WebClient):**

```java
// RestTemplate avec interceptor
@Configuration
public class RestTemplateConfig {
    
    @Bean
    public RestTemplate restTemplate() {
        RestTemplate restTemplate = new RestTemplate();
        restTemplate.setInterceptors(Collections.singletonList(
            new CorrelationIdPropagationInterceptor()
        ));
        return restTemplate;
    }
}

public class CorrelationIdPropagationInterceptor implements ClientHttpRequestInterceptor {
    
    @Override
    public ClientHttpResponse intercept(HttpRequest request, 
                                       byte[] body, 
                                       ClientHttpRequestExecution execution) throws IOException {
        
        // Récupérer Correlation ID depuis MDC
        String correlationId = MDC.get("correlationId");
        
        if (correlationId != null) {
            // Propager aux requêtes sortantes
            request.getHeaders().add("X-Correlation-Id", correlationId);
        }
        
        return execution.execute(request, body);
    }
}

// WebClient (reactive) avec filter
@Configuration
public class WebClientConfig {
    
    @Bean
    public WebClient webClient() {
        return WebClient.builder()
            .filter((request, next) -> {
                String correlationId = MDC.get("correlationId");
                
                if (correlationId != null) {
                    ClientRequest filteredRequest = ClientRequest.from(request)
                        .header("X-Correlation-Id", correlationId)
                        .build();
                    return next.exchange(filteredRequest);
                }
                
                return next.exchange(request);
            })
            .build();
    }
}
```

**Result - Traced logs:**

```
Gateway:
2024-11-01 10:00:00 [abc-123] Gateway - Request: GET /api/users/1
2024-11-01 10:00:00 [abc-123] Gateway - Routing to USER-SERVICE

User Service:
2024-11-01 10:00:01 [abc-123] UserService - Received request: GET /users/1
2024-11-01 10:00:01 [abc-123] UserService - Calling ORDER-SERVICE for user orders

Order Service:
2024-11-01 10:00:02 [abc-123] OrderService - Received request: GET /orders?userId=1
2024-11-01 10:00:02 [abc-123] OrderService - Found 3 orders for user 1

User Service:
2024-11-01 10:00:03 [abc-123] UserService - Response: User with 3 orders

Gateway:
2024-11-01 10:00:04 [abc-123] Gateway - Response: Status 200 - Duration: 4000ms
```

**With Correlation ID, you can:**
- ✅ Trace a request through all microservices
- ✅ Debug errors quickly
- ✅ Analyze end-to-end performance
- ✅ Correlate logs, metrics, and traces

---

**Q17: How do you implement Response Time Monitoring in the Gateway?**

**A:** 

```java
@Component
@Order(10)  // S'exécute après auth/logging filters
public class ResponseTimeFilter implements GlobalFilter {
    
    private static final Logger logger = LoggerFactory.getLogger(ResponseTimeFilter.class);
    
    @Autowired
    private MeterRegistry meterRegistry;  // Micrometer pour metrics
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        // 1. Capturer timestamp de début
        long startTime = System.currentTimeMillis();
        exchange.getAttributes().put("startTime", startTime);
        
        ServerHttpRequest request = exchange.getRequest();
        String path = request.getPath().value();
        String method = request.getMethod().name();
        
        // 2. Continuer la chaîne
        return chain.filter(exchange).then(Mono.fromRunnable(() -> {
            // 3. Calculer durée après réponse
            Long start = exchange.getAttribute("startTime");
            if (start != null) {
                long duration = System.currentTimeMillis() - start;
                
                ServerHttpResponse response = exchange.getResponse();
                HttpStatus statusCode = response.getStatusCode();
                
                // 4. Logger
                logger.info("Response Time - Method: {}, Path: {}, Status: {}, Duration: {}ms",
                    method,
                    path,
                    statusCode,
                    duration
                );
                
                // 5. Ajouter header
                response.getHeaders().add("X-Response-Time", duration + "ms");
                
                // 6. Enregistrer metric (Prometheus/Grafana)
                recordMetric(method, path, statusCode != null ? statusCode.value() : 0, duration);
                
                // 7. Alert si trop lent
                if (duration > 3000) {  // > 3 secondes
                    logger.warn("SLOW REQUEST DETECTED - Method: {}, Path: {}, Duration: {}ms",
                        method, path, duration);
                }
            }
        }));
    }
    
    private void recordMetric(String method, String path, int status, long duration) {
        // Enregistrer dans Micrometer pour Prometheus
        Timer.builder("gateway.request.duration")
            .tag("method", method)
            .tag("path", sanitizePath(path))
            .tag("status", String.valueOf(status))
            .register(meterRegistry)
            .record(duration, TimeUnit.MILLISECONDS);
        
        // Counter pour nombre de requêtes
        Counter.builder("gateway.request.count")
            .tag("method", method)
            .tag("path", sanitizePath(path))
            .tag("status", String.valueOf(status))
            .register(meterRegistry)
            .increment();
    }
    
    private String sanitizePath(String path) {
        // Remplacer IDs dynamiques pour éviter cardinalité élevée
        // /api/users/123 → /api/users/{id}
        return path.replaceAll("\\d+", "{id}");
    }
    
    @Override
    public int getOrder() {
        return 10;
    }
}
```

**Actuator + Prometheus Configuration:**

```yaml
management:
  endpoints:
    web:
      exposure:
        include: health,info,metrics,prometheus
  endpoint:
    health:
      show-details: always
  metrics:
    export:
      prometheus:
        enabled: true
    distribution:
      percentiles-histogram:
        gateway.request.duration: true
      percentiles:
        gateway.request.duration: 0.5, 0.95, 0.99
```

**Prometheus Queries (examples):**

```promql
# Latence moyenne par endpoint
rate(gateway_request_duration_sum[5m]) / rate(gateway_request_duration_count[5m])

# P95 latency
histogram_quantile(0.95, sum(rate(gateway_request_duration_bucket[5m])) by (le, path))

# Requêtes par seconde
rate(gateway_request_count[1m])

# Taux d'erreur 5xx
rate(gateway_request_count{status=~"5.."}[5m]) / rate(gateway_request_count[5m])
```

**Grafana Dashboard (panel examples):**
- Graph: P50/P95/P99 latency per endpoint
- Graph: Throughput (requests/second)
- Stat: 4xx/5xx error rate
- Table: Top 10 slowest endpoints
- Heatmap: Latency distribution

---

**Q18: How do you handle errors globally in the Gateway?**

**A:** 

**1. Global Error Handler:**

```java
@Component
public class GlobalErrorWebExceptionHandler extends AbstractErrorWebExceptionHandler {
    
    private static final Logger logger = LoggerFactory.getLogger(GlobalErrorWebExceptionHandler.class);
    
    public GlobalErrorWebExceptionHandler(
            ErrorAttributes errorAttributes,
            ResourceProperties resourceProperties,
            ApplicationContext applicationContext
    ) {
        super(errorAttributes, resourceProperties, applicationContext);
    }
    
    @Override
    protected RouterFunction<ServerResponse> getRoutingFunction(ErrorAttributes errorAttributes) {
        return RouterFunctions.route(RequestPredicates.all(), this::renderErrorResponse);
    }
    
    private Mono<ServerResponse> renderErrorResponse(ServerRequest request) {
        Map<String, Object> errorAttributes = getErrorAttributes(request, ErrorAttributeOptions.defaults());
        Throwable error = getError(request);
        
        // Extraire Correlation ID
        String correlationId = request.exchange().getAttribute("correlationId");
        
        // Logger l'erreur
        logger.error("[{}] Error occurred: {}", correlationId, error.getMessage(), error);
        
        // Construire réponse d'erreur
        ErrorResponse errorResponse = buildErrorResponse(error, correlationId, errorAttributes);
        
        // Déterminer status HTTP
        HttpStatus status = determineHttpStatus(error);
        
        return ServerResponse
            .status(status)
            .contentType(MediaType.APPLICATION_JSON)
            .body(BodyInserters.fromValue(errorResponse));
    }
    
    private ErrorResponse buildErrorResponse(Throwable error, String correlationId, Map<String, Object> errorAttributes) {
        ErrorResponse response = new ErrorResponse();
        response.setCorrelationId(correlationId);
        response.setTimestamp(Instant.now().toString());
        response.setMessage(error.getMessage());
        response.setError(error.getClass().getSimpleName());
        
        // Ne pas exposer stack trace en production
        if (!isProduction()) {
            response.setStackTrace(getStackTraceString(error));
        }
        
        return response;
    }
    
    private HttpStatus determineHttpStatus(Throwable error) {
        if (error instanceof ResponseStatusException) {
            return ((ResponseStatusException) error).getStatus();
        } else if (error instanceof TimeoutException) {
            return HttpStatus.GATEWAY_TIMEOUT;
        } else if (error instanceof ConnectException) {
            return HttpStatus.SERVICE_UNAVAILABLE;
        } else if (error instanceof IllegalArgumentException) {
            return HttpStatus.BAD_REQUEST;
        }
        
        return HttpStatus.INTERNAL_SERVER_ERROR;
    }
    
    private String getStackTraceString(Throwable error) {
        StringWriter sw = new StringWriter();
        error.printStackTrace(new PrintWriter(sw));
        return sw.toString();
    }
    
    private boolean isProduction() {
        // Check environment
        return false; // Simplified
    }
}

@Data
class ErrorResponse {
    private String correlationId;
    private String timestamp;
    private String message;
    private String error;
    private String stackTrace;
}
```

---

**2. Exception-specific Handlers:**

```java
@Component
@Order(5)
public class ExceptionHandlingFilter implements GlobalFilter {
    
    private static final Logger logger = LoggerFactory.getLogger(ExceptionHandlingFilter.class);
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        return chain.filter(exchange)
            .onErrorResume(TimeoutException.class, ex -> {
                logger.error("Timeout occurred: {}", ex.getMessage());
                return handleTimeoutException(exchange, ex);
            })
            .onErrorResume(ConnectException.class, ex -> {
                logger.error("Service unavailable: {}", ex.getMessage());
                return handleServiceUnavailableException(exchange, ex);
            })
            .onErrorResume(Exception.class, ex -> {
                logger.error("Unexpected error: {}", ex.getMessage(), ex);
                return handleGenericException(exchange, ex);
            });
    }
    
    private Mono<Void> handleTimeoutException(ServerWebExchange exchange, TimeoutException ex) {
        ServerHttpResponse response = exchange.getResponse();
        response.setStatusCode(HttpStatus.GATEWAY_TIMEOUT);
        
        String body = "{\"error\":\"Request timeout\",\"message\":\"Service took too long to respond\"}";
        return writeResponse(response, body);
    }
    
    private Mono<Void> handleServiceUnavailableException(ServerWebExchange exchange, ConnectException ex) {
        ServerHttpResponse response = exchange.getResponse();
        response.setStatusCode(HttpStatus.SERVICE_UNAVAILABLE);
        
        String body = "{\"error\":\"Service unavailable\",\"message\":\"Unable to connect to downstream service\"}";
        return writeResponse(response, body);
    }
    
    private Mono<Void> handleGenericException(ServerWebExchange exchange, Exception ex) {
        ServerHttpResponse response = exchange.getResponse();
        response.setStatusCode(HttpStatus.INTERNAL_SERVER_ERROR);
        
        String body = String.format(
            "{\"error\":\"Internal server error\",\"message\":\"%s\"}",
            ex.getMessage()
        );
        return writeResponse(response, body);
    }
    
    private Mono<Void> writeResponse(ServerHttpResponse response, String body) {
        response.getHeaders().setContentType(MediaType.APPLICATION_JSON);
        DataBuffer buffer = response.bufferFactory().wrap(body.getBytes());
        return response.writeWith(Mono.just(buffer));
    }
    
    @Override
    public int getOrder() {
        return 5;
    }
}
```

---

**Q19: Give a complete example of a Gateway configuration in production**

**A:** 

**application.yml (production-ready):**

```yaml
spring:
  application:
    name: gateway-server
  
  # Redis pour Rate Limiting
  redis:
    host: ${REDIS_HOST:localhost}
    port: ${REDIS_PORT:6379}
    password: ${REDIS_PASSWORD:}
    timeout: 2000ms
  
  # Cloud Gateway Configuration
  cloud:
    gateway:
      # Discovery
      discovery:
        locator:
          enabled: false  # Désactiver auto-discovery en prod (routes manuelles)
      
      # Global filters
      default-filters:
        - DedupeResponseHeader=Access-Control-Allow-Origin Access-Control-Allow-Credentials, RETAIN_UNIQUE
      
      # Global CORS
      globalcors:
        cors-configurations:
          '[/**]':
            allowed-origins:
              - https://myapp.com
              - https://www.myapp.com
            allowed-methods:
              - GET
              - POST
              - PUT
              - DELETE
              - OPTIONS
            allowed-headers: "*"
            allow-credentials: true
            max-age: 3600
      
      # Routes
      routes:
        # User Service
        - id: user-service
          uri: lb://USER-SERVICE
          predicates:
            - Path=/api/v1/users/**
            - Method=GET,POST,PUT,DELETE
          filters:
            - RewritePath=/api/v1/users/(?<segment>.*), /users/${segment}
            - name: CircuitBreaker
              args:
                name: userServiceCB
                fallbackUri: forward:/fallback/users
            - name: RequestRateLimiter
              args:
                redis-rate-limiter.replenishRate: 50
                redis-rate-limiter.burstCapacity: 100
                key-resolver: "#{@userKeyResolver}"
            - name: Retry
              args:
                retries: 3
                statuses: BAD_GATEWAY, SERVICE_UNAVAILABLE
                methods: GET
                backoff:
                  firstBackoff: 100ms
                  maxBackoff: 1000ms
                  factor: 2
        
        # Order Service
        - id: order-service
          uri: lb://ORDER-SERVICE
          predicates:
            - Path=/api/v1/orders/**
          filters:
            - RewritePath=/api/v1/orders/(?<segment>.*), /orders/${segment}
            - name: CircuitBreaker
              args:
                name: orderServiceCB
                fallbackUri: forward:/fallback/orders
            - name: RequestRateLimiter
              args:
                redis-rate-limiter.replenishRate: 30
                redis-rate-limiter.burstCapacity: 60
        
        # Product Service (public, rate limité)
        - id: product-service-public
          uri: lb://PRODUCT-SERVICE
          predicates:
            - Path=/api/v1/products/**
            - Method=GET
          filters:
            - RewritePath=/api/v1/products/(?<segment>.*), /products/${segment}
            - name: RequestRateLimiter
              args:
                redis-rate-limiter.replenishRate: 10
                redis-rate-limiter.burstCapacity: 20
                key-resolver: "#{@ipKeyResolver}"
        
        # Payment Service (très sécurisé)
        - id: payment-service
          uri: lb://PAYMENT-SERVICE
          predicates:
            - Path=/api/v1/payments/**
          filters:
            - RewritePath=/api/v1/payments/(?<segment>.*), /payments/${segment}
            - name: CircuitBreaker
              args:
                name: paymentServiceCB
                fallbackUri: forward:/fallback/payments
            - name: Retry
              args:
                retries: 5
                statuses: INTERNAL_SERVER_ERROR
                methods: POST
                backoff:
                  firstBackoff: 200ms
                  maxBackoff: 3000ms
                  factor: 2

# Eureka Client
eureka:
  client:
    service-url:
      defaultZone: ${EUREKA_URL:http://localhost:8761/eureka/}
    fetch-registry: true
    register-with-eureka: true
    registry-fetch-interval-seconds: 5
    healthcheck:
      enabled: true
  instance:
    prefer-ip-address: true
    instance-id: ${spring.application.name}:${spring.cloud.client.hostname}:${random.value}
    lease-renewal-interval-in-seconds: 5
    lease-expiration-duration-in-seconds: 10
    metadata-map:
      zone: ${ZONE:zone1}

# Resilience4j Circuit Breaker
resilience4j:
  circuitbreaker:
    instances:
      userServiceCB:
        sliding-window-type: COUNT_BASED
        sliding-window-size: 10
        failure-rate-threshold: 50
        wait-duration-in-open-state: 30s
        permitted-number-of-calls-in-half-open-state: 5
        automatic-transition-from-open-to-half-open-enabled: true
        minimum-number-of-calls: 5
        slow-call-duration-threshold: 3s
        slow-call-rate-threshold: 60
      
      orderServiceCB:
        sliding-window-size: 10
        failure-rate-threshold: 60
        wait-duration-in-open-state: 20s
      
      paymentServiceCB:
        sliding-window-size: 20
        failure-rate-threshold: 40
        wait-duration-in-open-state: 60s
  
  timelimiter:
    instances:
      userServiceCB:
        timeout-duration: 5s
      orderServiceCB:
        timeout-duration: 10s
      paymentServiceCB:
        timeout-duration: 15s

# Actuator (Monitoring)
management:
  endpoints:
    web:
      exposure:
        include: health,info,metrics,prometheus,gateway
      base-path: /actuator
  endpoint:
    health:
      show-details: always
      probes:
        enabled: true  # Kubernetes liveness/readiness
  health:
    circuitbreakers:
      enabled: true
  metrics:
    export:
      prometheus:
        enabled: true
    distribution:
      percentiles-histogram:
        http.server.requests: true
      percentiles:
        http.server.requests: 0.5, 0.95, 0.99
    tags:
      application: ${spring.application.name}
      environment: ${ENVIRONMENT:production}

# JWT Configuration
jwt:
  secret: ${JWT_SECRET}
  expiration: 3600000  # 1 hour

# Server Configuration
server:
  port: ${PORT:8080}
  shutdown: graceful  # Graceful shutdown
  compression:
    enabled: true
    mime-types: application/json,application/xml,text/html,text/xml,text/plain
  http2:
    enabled: true  # HTTP/2 support

# Logging
logging:
  level:
    root: INFO
    org.springframework.cloud.gateway: INFO
    com.example.gateway: DEBUG
  pattern:
    console: "%d{yyyy-MM-dd HH:mm:ss} [%thread] %-5level [%X{correlationId}] %logger{36} - %msg%n"
  file:
    name: /var/log/gateway/gateway-server.log
    max-size: 10MB
    max-history: 30

# Spring Boot Configuration
spring.lifecycle.timeout-per-shutdown-phase: 30s
```

---

**Q20: What are the best practices for an API Gateway in production?**

**A:** 

**1. High Availability (HA)**
```yaml
# Déployer multiple instances du Gateway
Gateway Instance 1 (zone-a) ←┐
Gateway Instance 2 (zone-b) ←├─ Load Balancer (Nginx/AWS ALB)
Gateway Instance 3 (zone-c) ←┘

# Kubernetes Deployment
apiVersion: apps/v1
kind: Deployment
metadata:
  name: gateway-server
spec:
  replicas: 3  # Minimum 3 instances
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxSurge: 1
      maxUnavailable: 0
```

---

**2. Horizontal Scaling**
```yaml
# Horizontal Pod Autoscaler (Kubernetes)
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: gateway-hpa
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: gateway-server
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
```

---

**3. Health Checks & Readiness**
```yaml
# Kubernetes Probes
livenessProbe:
  httpGet:
    path: /actuator/health/liveness
    port: 8080
  initialDelaySeconds: 30
  periodSeconds: 10
  failureThreshold: 3

readinessProbe:
  httpGet:
    path: /actuator/health/readiness
    port: 8080
  initialDelaySeconds: 10
  periodSeconds: 5
  failureThreshold: 3
```

```java
// Custom Health Indicator
@Component
public class DownstreamServicesHealthIndicator implements HealthIndicator {
    
    @Override
    public Health health() {
        // Vérifier que services critiques sont UP
        boolean userServiceUp = checkService("USER-SERVICE");
        boolean orderServiceUp = checkService("ORDER-SERVICE");
        
        if (userServiceUp && orderServiceUp) {
            return Health.up()
                .withDetail("user-service", "UP")
                .withDetail("order-service", "UP")
                .build();
        }
        
        return Health.down()
            .withDetail("user-service", userServiceUp ? "UP" : "DOWN")
            .withDetail("order-service", orderServiceUp ? "UP" : "DOWN")
            .build();
    }
}
```

---

**4. Security Best Practices**

```java
// HTTPS obligatoire en production
server:
  ssl:
    enabled: true
    key-store: classpath:keystore.p12
    key-store-password: ${KEYSTORE_PASSWORD}
    key-store-type: PKCS12
    key-alias: gateway

// Security Headers
@Component
public class SecurityHeadersFilter implements GlobalFilter {
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        return chain.filter(exchange).then(Mono.fromRunnable(() -> {
            ServerHttpResponse response = exchange.getResponse();
            response.getHeaders().add("Strict-Transport-Security", "max-age=31536000; includeSubDomains");
            response.getHeaders().add("X-Content-Type-Options", "nosniff");
            response.getHeaders().add("X-Frame-Options", "DENY");
            response.getHeaders().add("X-XSS-Protection", "1; mode=block");
            response.getHeaders().add("Content-Security-Policy", "default-src 'self'");
        }));
    }
}
```

---

**5. Monitoring & Observability**

```yaml
# Prometheus + Grafana
management:
  metrics:
    export:
      prometheus:
        enabled: true

# Dashboards à monitorer:
- Latency (P50, P95, P99)
- Throughput (req/sec)
- Error rate (4xx, 5xx)
- Circuit breaker states
- Rate limit hits
- CPU/Memory usage
```

```java
// Custom Metrics
@Component
public class MetricsFilter implements GlobalFilter {
    @Autowired
    private MeterRegistry meterRegistry;
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        Counter.builder("gateway.requests")
            .tag("path", sanitizePath(exchange.getRequest().getPath().value()))
            .register(meterRegistry)
            .increment();
        
        return chain.filter(exchange);
    }
}
```

---

**6. Strategic Caching**

```java
// Cache réponses GET fréquentes
@Component
public class ResponseCachingFilter implements GlobalFilter {
    @Autowired
    private RedisTemplate<String, String> redisTemplate;
    
    // Cache GET /api/products, /api/categories
    private static final Set<String> CACHEABLE_PATHS = Set.of("/api/products", "/api/categories");
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        // Check cache → Si hit, retourner cached response
        // Si miss → Call service → Cache response (TTL 5min)
        return chain.filter(exchange);
    }
}
```

---

**7. Graceful Shutdown**

```yaml
spring:
  lifecycle:
    timeout-per-shutdown-phase: 30s  # Attendre 30s avant shutdown

server:
  shutdown: graceful  # Graceful shutdown enabled
```

```java
// Shutdown Hook personnalisé
@Component
public class GracefulShutdownHandler {
    
    private static final Logger logger = LoggerFactory.getLogger(GracefulShutdownHandler.class);
    
    @PreDestroy
    public void onShutdown() {
        logger.info("Gateway shutting down gracefully...");
        
        // 1. Désenregistrer du Service Registry (Eureka)
        logger.info("Deregistering from Eureka...");
        
        // 2. Attendre que requêtes en cours se terminent
        logger.info("Waiting for in-flight requests to complete...");
        
        // 3. Fermer connexions Redis
        logger.info("Closing Redis connections...");
        
        logger.info("Gateway shutdown complete");
    }
}
```

---

**8. Strategic Rate Limiting**

```yaml
# Différentes limites selon type de client
routes:
  # Public API - stricte
  - id: public-api
    filters:
      - name: RequestRateLimiter
        args:
          redis-rate-limiter.replenishRate: 10
          redis-rate-limiter.burstCapacity: 20
          key-resolver: "#{@ipKeyResolver}"
  
  # Authenticated users - généreuse
  - id: authenticated-api
    filters:
      - name: RequestRateLimiter
        args:
          redis-rate-limiter.replenishRate: 100
          redis-rate-limiter.burstCapacity: 200
          key-resolver: "#{@userKeyResolver}"
  
  # Premium users - très généreuse
  - id: premium-api
    predicates:
      - Header=X-User-Tier, premium
    filters:
      - name: RequestRateLimiter
        args:
          redis-rate-limiter.replenishRate: 500
          redis-rate-limiter.burstCapacity: 1000
```

---

**9. Optimal Circuit Breaker Configuration**

```yaml
resilience4j:
  circuitbreaker:
    instances:
      # Service critique (stricte)
      paymentServiceCB:
        sliding-window-size: 20
        failure-rate-threshold: 30     # 30% échecs → OPEN
        wait-duration-in-open-state: 60s  # 1 minute
        slow-call-duration-threshold: 5s
        slow-call-rate-threshold: 50
      
      # Service non-critique (plus tolérant)
      notificationServiceCB:
        sliding-window-size: 10
        failure-rate-threshold: 70     # 70% échecs → OPEN
        wait-duration-in-open-state: 10s
```

---

**10. Timeout Configuration**

```yaml
# Global timeout
spring:
  cloud:
    gateway:
      httpclient:
        connect-timeout: 1000        # 1 seconde connexion
        response-timeout: 5s         # 5 secondes réponse

# Per-route timeout
routes:
  - id: slow-service
    uri: lb://SLOW-SERVICE
    filters:
      - name: RequestTimeout
        args:
          timeout: 30s  # Override global timeout
```

---

**11. Request/Response Size Limits**

```yaml
server:
  max-http-header-size: 16KB       # Max header size
  
spring:
  codec:
    max-in-memory-size: 10MB       # Max request/response body in memory

# Custom validation
@Component
public class RequestSizeValidationFilter implements GlobalFilter {
    private static final long MAX_PAYLOAD_SIZE = 10_000_000; // 10 MB
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        String contentLength = exchange.getRequest().getHeaders().getFirst("Content-Length");
        
        if (contentLength != null) {
            long size = Long.parseLong(contentLength);
            if (size > MAX_PAYLOAD_SIZE) {
                return rejectRequest(exchange, "Payload too large");
            }
        }
        
        return chain.filter(exchange);
    }
}
```

---

**12. Logging Best Practices**

```yaml
logging:
  level:
    # Production: INFO
    root: INFO
    org.springframework.cloud.gateway: INFO
    
    # Debug specific packages si besoin
    com.example.gateway.filter: DEBUG
  
  # Structured logging (JSON)
  pattern:
    console: '{"timestamp":"%d{yyyy-MM-dd HH:mm:ss}","level":"%level","correlationId":"%X{correlationId}","logger":"%logger","message":"%msg"}%n'
  
  # Log rotation
  file:
    name: /var/log/gateway/gateway.log
    max-size: 100MB
    max-history: 30
    total-size-cap: 3GB
```

```java
// Sensitive data masking
@Component
public class SensitiveDataMaskingFilter implements GlobalFilter {
    
    private static final Set<String> SENSITIVE_HEADERS = Set.of(
        "Authorization",
        "X-API-Key",
        "Cookie"
    );
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = exchange.getRequest();
        
        // Logger avec masking
        Map<String, String> headers = new HashMap<>();
        request.getHeaders().forEach((key, values) -> {
            if (SENSITIVE_HEADERS.contains(key)) {
                headers.put(key, "***MASKED***");
            } else {
                headers.put(key, String.join(",", values));
            }
        });
        
        logger.info("Request headers: {}", headers);
        
        return chain.filter(exchange);
    }
}
```

---

**13. Environment-specific Configuration**

```yaml
# application.yml (commun)
spring:
  application:
    name: gateway-server
  profiles:
    active: ${ENVIRONMENT:dev}

---
# application-dev.yml
spring:
  config:
    activate:
      on-profile: dev
logging:
  level:
    root: DEBUG
eureka:
  client:
    service-url:
      defaultZone: http://localhost:8761/eureka/

---
# application-staging.yml
spring:
  config:
    activate:
      on-profile: staging
logging:
  level:
    root: INFO
eureka:
  client:
    service-url:
      defaultZone: http://eureka-staging.internal:8761/eureka/

---
# application-prod.yml
spring:
  config:
    activate:
      on-profile: prod
logging:
  level:
    root: WARN
    com.example: INFO
eureka:
  client:
    service-url:
      defaultZone: http://eureka-prod.internal:8761/eureka/
```

---

**14. Backup & Disaster Recovery**

```yaml
# Redis Backup pour Rate Limiting
redis:
  sentinel:
    master: mymaster
    nodes:
      - redis-sentinel-1:26379
      - redis-sentinel-2:26379
      - redis-sentinel-3:26379

# Eureka Multiple Zones
eureka:
  client:
    service-url:
      defaultZone: http://eureka1:8761/eureka/,http://eureka2:8761/eureka/,http://eureka3:8761/eureka/
```

---

**15. Documentation & API Versioning**

```yaml
# Routes avec versioning clair
routes:
  # API v1 (deprecated)
  - id: api-v1-users
    uri: lb://USER-SERVICE-V1
    predicates:
      - Path=/api/v1/users/**
    filters:
      - AddResponseHeader=X-API-Version, v1
      - AddResponseHeader=X-API-Deprecated, true
      - AddResponseHeader=X-API-Sunset, 2025-12-31
  
  # API v2 (current)
  - id: api-v2-users
    uri: lb://USER-SERVICE-V2
    predicates:
      - Path=/api/v2/users/**
    filters:
      - AddResponseHeader=X-API-Version, v2
```

---

**16. Testing Strategy**

```java
// Integration Tests
@SpringBootTest(webEnvironment = WebEnvironment.RANDOM_PORT)
@AutoConfigureWebTestClient
class GatewayIntegrationTest {
    
    @Autowired
    private WebTestClient webTestClient;
    
    @Test
    void testRouting() {
        webTestClient.get()
            .uri("/api/v1/users/1")
            .header("Authorization", "Bearer " + validToken)
            .exchange()
            .expectStatus().isOk()
            .expectHeader().exists("X-Correlation-Id")
            .expectHeader().valueEquals("X-API-Version", "v1");
    }
    
    @Test
    void testRateLimiting() {
        // Envoyer 25 requêtes (limite = 20)
        for (int i = 0; i < 25; i++) {
            WebTestClient.ResponseSpec response = webTestClient.get()
                .uri("/api/v1/users/1")
                .exchange();
            
            if (i < 20) {
                response.expectStatus().isOk();
            } else {
                response.expectStatus().isEqualTo(HttpStatus.TOO_MANY_REQUESTS);
            }
        }
    }
    
    @Test
    void testCircuitBreaker() {
        // Simuler service down
        // Vérifier fallback appelé
    }
}

// Load Testing (Gatling)
class GatewayLoadTest extends Simulation {
    
    val httpProtocol = http.baseUrl("http://localhost:8080")
    
    val scn = scenario("Gateway Load Test")
        .exec(
            http("Get Users")
                .get("/api/v1/users")
                .header("Authorization", "Bearer ${token}")
                .check(status.is(200))
        )
    
    setUp(
        scn.inject(
            rampUsers(1000).during(60.seconds)  // 1000 users sur 60s
        )
    ).protocols(httpProtocol)
}
```

---

**17. Security Scanning**

```bash
# Dependency Check
mvn org.owasp:dependency-check-maven:check

# Container Scanning (Trivy)
trivy image gateway-server:latest

# SAST (SonarQube)
mvn sonar:sonar
```

---

**18. Performance Tuning**

```yaml
# JVM Options
JAVA_OPTS: >
  -Xms2g
  -Xmx2g
  -XX:+UseG1GC
  -XX:MaxGCPauseMillis=200
  -XX:+HeapDumpOnOutOfMemoryError
  -XX:HeapDumpPath=/var/log/gateway/heapdump.hprof

# Netty (Gateway uses Netty)
spring:
  cloud:
    gateway:
      httpclient:
        pool:
          max-connections: 500        # Max connexions pool
          max-idle-time: 30s          # Idle timeout
        wiretap: false                # Désactiver wiretap en prod (perf)

# Redis Connection Pool
spring:
  redis:
    lettuce:
      pool:
        max-active: 20
        max-idle: 10
        min-idle: 5
        max-wait: 2000ms
```

---

**19. Cost Optimization**

```yaml
# Auto-scaling basé sur usage
# Scale down pendant heures creuses
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
spec:
  minReplicas: 3  # Heures pleines
  maxReplicas: 10
  behavior:
    scaleDown:
      stabilizationWindowSeconds: 300  # 5 min avant scale down
      policies:
      - type: Percent
        value: 50  # Scale down max 50% à la fois
        periodSeconds: 60

# Cache agressif pour réduire appels downstream
filters:
  - name: LocalResponseCache
    args:
      size: 1000  # Cache 1000 responses
      timeToLive: 5m
```

---

**20. Disaster Recovery Plan**

```yaml
# Backup Configuration
backup:
  # Gateway config backup quotidien
  schedule: "0 2 * * *"  # 2am daily
  
  # Metrics/Logs retention
  metrics-retention: 30d
  logs-retention: 90d

# Multi-Region Deployment
regions:
  - us-east-1:
      instances: 3
      priority: primary
  - eu-west-1:
      instances: 3
      priority: secondary
  - ap-southeast-1:
      instances: 2
      priority: tertiary

# Failover automatique
dns:
  health-check: /actuator/health
  failover-ttl: 60s
```

---

**Production Checklist:**

✅ **High Availability**: Minimum 3 instances, multi-zone  
✅ **Security**: HTTPS, JWT validation, Security headers  
✅ **Monitoring**: Prometheus + Grafana, Distributed tracing  
✅ **Resilience**: Circuit breakers, Retry, Timeouts  
✅ **Performance**: Caching, Connection pooling, JVM tuning  
✅ **Scalability**: Horizontal scaling, Load balancing  
✅ **Logging**: Structured logs, Correlation IDs, Log rotation  
✅ **Rate Limiting**: Protection against abuse  
✅ **Health Checks**: Liveness/Readiness probes  
✅ **Graceful Shutdown**: No dropped requests  
✅ **Disaster Recovery**: Backups, Multi-region  
✅ **Documentation**: API versioning, Deprecation notices  
✅ **Testing**: Unit, Integration, Load tests  
✅ **Security Scanning**: OWASP, Container scanning  
✅ **Cost Optimization**: Auto-scaling, Resource limits  

---

That covers **all aspects of Spring Cloud Gateway**: patterns, configuration, filters, Eureka integration, cross-cutting concerns, resilience, monitoring, and production best practices. You are perfectly prepared for your API Gateway interview! 🚀🔒