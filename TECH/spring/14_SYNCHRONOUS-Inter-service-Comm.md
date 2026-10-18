## [**..**](./00_index.md)

## **Communication Inter-Services Synchrone dans les Microservices Spring Boot**

La communication synchrone entre microservices constitue un pilier fondamental des architectures distribuées. Spring Boot offre plusieurs mécanismes pour implémenter ce type de communication, chacun ayant ses avantages et cas d'usage spécifiques.

### **RestTemplate - L'approche classique**

RestTemplate représente la méthode traditionnelle pour effectuer des appels HTTP synchrones. Cette API propose une interface simple pour les opérations CRUD :

```java
RestTemplate restTemplate = new RestTemplate();

// GET request
User user = restTemplate.getForObject(
    "http://user-service/api/users/{id}", 
    User.class, 
    userId
);

// POST request
CreateUserRequest request = new CreateUserRequest("John", "john@example.com");
User createdUser = restTemplate.postForObject(
    "http://user-service/api/users", 
    request, 
    User.class
);

// PUT request
restTemplate.put("http://user-service/api/users/{id}", updateRequest, userId);

// DELETE request
restTemplate.delete("http://user-service/api/users/{id}", userId);
```

Malgré sa simplicité, RestTemplate présente certaines limitations : verbosité du code, gestion manuelle des erreurs, et statut deprecated depuis Spring 5. Cette API est en mode maintenance et n'est plus recommandée pour les nouveaux développements.

### **Spring Cloud OpenFeign - L'approche déclarative**

Spring Cloud OpenFeign apporte une abstraction de haut niveau basée sur des interfaces annotées, réduisant considérablement le boilerplate code :

```java
@FeignClient(name = "order-service", url = "http://order-service")
public interface OrderClient {
    
    @GetMapping("/api/orders/{id}")
    OrderDTO getOrder(@PathVariable("id") Long id);
    
    @GetMapping("/api/orders")
    List<OrderDTO> getAllOrders(@RequestParam("status") String status);
    
    @PostMapping("/api/orders")
    OrderDTO createOrder(@RequestBody CreateOrderRequest request);
    
    @PutMapping("/api/orders/{id}")
    OrderDTO updateOrder(@PathVariable Long id, @RequestBody UpdateOrderRequest request);
    
    @DeleteMapping("/api/orders/{id}")
    void deleteOrder(@PathVariable Long id);
}

// Utilisation dans un service
@Service
public class OrderService {
    private final OrderClient orderClient;
    
    public OrderService(OrderClient orderClient) {
        this.orderClient = orderClient;
    }
    
    public OrderDTO processOrder(Long orderId) {
        return orderClient.getOrder(orderId);
    }
}
```

Configuration avancée avec interceptors et error handling :

```java
@Configuration
public class FeignConfig {
    
    @Bean
    public RequestInterceptor requestInterceptor() {
        return requestTemplate -> {
            requestTemplate.header("Authorization", "Bearer " + getToken());
            requestTemplate.header("X-Request-ID", UUID.randomUUID().toString());
        };
    }
    
    @Bean
    public ErrorDecoder errorDecoder() {
        return (methodKey, response) -> {
            if (response.status() == 404) {
                return new ResourceNotFoundException("Resource not found");
            }
            if (response.status() == 503) {
                return new ServiceUnavailableException("Service unavailable");
            }
            return new FeignException.FeignServerException(
                response.status(), 
                response.reason(), 
                response.request(), 
                response.body()
            );
        };
    }
    
    @Bean
    public Retryer retryer() {
        return new Retryer.Default(100, 1000, 3);
    }
}
```

Intégration avec Circuit Breaker (Resilience4j) :

```java
@FeignClient(
    name = "payment-service",
    fallback = PaymentClientFallback.class
)
public interface PaymentClient {
    @PostMapping("/api/payments")
    PaymentResponse processPayment(@RequestBody PaymentRequest request);
}

@Component
public class PaymentClientFallback implements PaymentClient {
    @Override
    public PaymentResponse processPayment(PaymentRequest request) {
        return PaymentResponse.builder()
            .status("PENDING")
            .message("Payment service temporarily unavailable")
            .build();
    }
}
```

### **RestClient - L'alternative moderne**

Introduit dans Spring Framework 6.1, RestClient offre une API fluide et moderne pour les appels HTTP synchrones :

```java
@Configuration
public class RestClientConfig {
    @Bean
    public RestClient restClient() {
        return RestClient.builder()
            .baseUrl("http://product-service")
            .defaultHeader("Content-Type", "application/json")
            .requestInterceptor((request, body, execution) -> {
                request.getHeaders().setBearerAuth(getToken());
                return execution.execute(request, body);
            })
            .build();
    }
}

@Service
public class ProductService {
    private final RestClient restClient;
    
    public ProductService(RestClient restClient) {
        this.restClient = restClient;
    }
    
    // GET request
    public Product getProduct(Long id) {
        return restClient.get()
            .uri("/api/products/{id}", id)
            .retrieve()
            .body(Product.class);
    }
    
    // POST request avec gestion d'erreur
    public Product createProduct(CreateProductRequest request) {
        return restClient.post()
            .uri("/api/products")
            .contentType(MediaType.APPLICATION_JSON)
            .body(request)
            .retrieve()
            .onStatus(HttpStatusCode::is4xxClientError, (req, res) -> {
                throw new BadRequestException("Invalid product data");
            })
            .onStatus(HttpStatusCode::is5xxServerError, (req, res) -> {
                throw new ServiceException("Product service error");
            })
            .body(Product.class);
    }
    
    // GET avec ResponseEntity
    public ResponseEntity<List<Product>> getProducts(String category) {
        return restClient.get()
            .uri(uriBuilder -> uriBuilder
                .path("/api/products")
                .queryParam("category", category)
                .build())
            .retrieve()
            .toEntity(new ParameterizedTypeReference<List<Product>>() {});
    }
    
    // PUT request
    public Product updateProduct(Long id, UpdateProductRequest request) {
        return restClient.put()
            .uri("/api/products/{id}", id)
            .body(request)
            .retrieve()
            .body(Product.class);
    }
    
    // DELETE request
    public void deleteProduct(Long id) {
        restClient.delete()
            .uri("/api/products/{id}", id)
            .retrieve()
            .toBodilessEntity();
    }
}
```

### **WebClient - Pour la programmation réactive**

WebClient supporte à la fois les appels réactifs (non-bloquants) et synchrones, offrant une grande flexibilité :

```java
@Configuration
public class WebClientConfig {
    @Bean
    public WebClient webClient() {
        return WebClient.builder()
            .baseUrl("http://payment-service")
            .defaultHeader(HttpHeaders.CONTENT_TYPE, MediaType.APPLICATION_JSON_VALUE)
            .filter(ExchangeFilterFunction.ofRequestProcessor(clientRequest -> {
                ClientRequest filtered = ClientRequest.from(clientRequest)
                    .header("Authorization", "Bearer " + getToken())
                    .build();
                return Mono.just(filtered);
            }))
            .build();
    }
}

@Service
public class PaymentService {
    private final WebClient webClient;
    
    public PaymentService(WebClient webClient) {
        this.webClient = webClient;
    }
    
    // Appel synchrone avec block()
    public PaymentResponse processPaymentSync(PaymentRequest request) {
        return webClient.post()
            .uri("/api/payments")
            .bodyValue(request)
            .retrieve()
            .onStatus(HttpStatusCode::isError, response -> 
                response.bodyToMono(String.class)
                    .flatMap(error -> Mono.error(
                        new PaymentException("Payment failed: " + error)
                    ))
            )
            .bodyToMono(PaymentResponse.class)
            .timeout(Duration.ofSeconds(5))
            .block();
    }
    
    // Appel réactif (non-bloquant)
    public Mono<PaymentResponse> processPaymentAsync(PaymentRequest request) {
        return webClient.post()
            .uri("/api/payments")
            .bodyValue(request)
            .retrieve()
            .bodyToMono(PaymentResponse.class)
            .retryWhen(Retry.backoff(3, Duration.ofMillis(100)))
            .timeout(Duration.ofSeconds(5));
    }
    
    // Appels parallèles
    public Mono<OrderSummary> getOrderSummary(Long orderId) {
        Mono<Order> orderMono = webClient.get()
            .uri("/api/orders/{id}", orderId)
            .retrieve()
            .bodyToMono(Order.class);
            
        Mono<List<Payment>> paymentsMono = webClient.get()
            .uri("/api/payments?orderId={id}", orderId)
            .retrieve()
            .bodyToFlux(Payment.class)
            .collectList();
            
        Mono<ShippingInfo> shippingMono = webClient.get()
            .uri("/api/shipping/{id}", orderId)
            .retrieve()
            .bodyToMono(ShippingInfo.class);
        
        return Mono.zip(orderMono, paymentsMono, shippingMono)
            .map(tuple -> new OrderSummary(
                tuple.getT1(), 
                tuple.getT2(), 
                tuple.getT3()
            ));
    }
    
    // Streaming de données
    public Flux<Notification> streamNotifications() {
        return webClient.get()
            .uri("/api/notifications/stream")
            .accept(MediaType.TEXT_EVENT_STREAM)
            .retrieve()
            .bodyToFlux(Notification.class);
    }
}
```

Configuration avancée avec timeout et retry :

```java
@Configuration
public class WebClientAdvancedConfig {
    
    @Bean
    public WebClient webClientWithTimeout() {
        HttpClient httpClient = HttpClient.create()
            .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, 5000)
            .responseTimeout(Duration.ofSeconds(5))
            .doOnConnected(conn -> 
                conn.addHandlerLast(new ReadTimeoutHandler(5))
                    .addHandlerLast(new WriteTimeoutHandler(5))
            );
        
        return WebClient.builder()
            .clientConnector(new ReactorClientHttpConnector(httpClient))
            .build();
    }
}
```

### **HTTP Interfaces - L'approche moderne de Spring 6**

HTTP Interfaces combine la simplicité des interfaces déclaratives avec la puissance de WebClient :

```java
public interface NotificationService {
    
    @GetExchange("/api/notifications/{id}")
    Notification getNotification(@PathVariable String id);
    
    @GetExchange("/api/notifications")
    List<Notification> getAllNotifications(@RequestParam("userId") Long userId);
    
    @PostExchange("/api/notifications")
    Notification sendNotification(@RequestBody NotificationRequest request);
    
    @PutExchange("/api/notifications/{id}")
    Notification updateNotification(
        @PathVariable String id, 
        @RequestBody UpdateNotificationRequest request
    );
    
    @DeleteExchange("/api/notifications/{id}")
    void deleteNotification(@PathVariable String id);
    
    // Support des appels réactifs
    @GetExchange("/api/notifications/stream")
    Flux<Notification> streamNotifications();
}

@Configuration
public class HttpInterfaceConfig {
    
    @Bean
    public NotificationService notificationService() {
        WebClient webClient = WebClient.builder()
            .baseUrl("http://notification-service")
            .defaultHeader("Content-Type", "application/json")
            .build();
        
        HttpServiceProxyFactory factory = HttpServiceProxyFactory
            .builder(WebClientAdapter.forClient(webClient))
            .build();
        
        return factory.createClient(NotificationService.class);
    }
}

@Service
public class NotificationHandler {
    private final NotificationService notificationService;
    
    public NotificationHandler(NotificationService notificationService) {
        this.notificationService = notificationService;
    }
    
    public void sendUserNotification(Long userId, String message) {
        NotificationRequest request = new NotificationRequest(userId, message);
        Notification result = notificationService.sendNotification(request);
    }
}
```

Configuration avec interceptors et error handling :

```java
@Configuration
public class HttpInterfaceAdvancedConfig {
    
    @Bean
    public CustomerService customerService() {
        WebClient webClient = WebClient.builder()
            .baseUrl("http://customer-service")
            .filter((request, next) -> {
                ClientRequest filtered = ClientRequest.from(request)
                    .header("Authorization", "Bearer " + getToken())
                    .header("X-Correlation-ID", UUID.randomUUID().toString())
                    .build();
                return next.exchange(filtered);
            })
            .filter((request, next) -> 
                next.exchange(request)
                    .onErrorResume(WebClientResponseException.class, ex -> {
                        if (ex.getStatusCode().value() == 404) {
                            return Mono.error(new ResourceNotFoundException());
                        }
                        return Mono.error(ex);
                    })
            )
            .build();
        
        HttpServiceProxyFactory factory = HttpServiceProxyFactory
            .builder(WebClientAdapter.forClient(webClient))
            .customArgumentResolver(new CustomArgumentResolver())
            .build();
        
        return factory.createClient(CustomerService.class);
    }
}
```

### **Comparaison et choix technique**

| Critère | RestTemplate | OpenFeign | RestClient | WebClient | HTTP Interfaces |
|---------|-------------|-----------|------------|-----------|-----------------|
| **Statut** | Deprecated | Actif | Recommandé | Actif | Moderne (Spring 6+) |
| **Approche** | Impérative | Déclarative | Fluide | Réactive/Bloquante | Déclarative |
| **Complexité** | Simple | Moyenne | Simple | Élevée | Simple |
| **Performance** | Moyenne | Moyenne | Bonne | Excellente | Excellente |
| **Use Case** | Legacy | Microservices | Appels simples | Réactif/Parallèle | Projets modernes |

**Cas d'usage recommandés :**

- **RestClient** : Appels HTTP synchrones simples dans Spring Boot 3+, remplacement de RestTemplate
- **OpenFeign** : Architectures microservices complexes nécessitant service discovery, load balancing, circuit breaker
- **WebClient** : Applications réactives avec Spring WebFlux, appels parallèles, streaming de données, haute performance
- **HTTP Interfaces** : Nouveaux projets Spring 6+ recherchant une API moderne, type-safe et déclarative

**Aspects transversaux communs :**

- **Gestion d'erreur** : Utilisation de `@ControllerAdvice` et `@ExceptionHandler` côté serveur
- **Timeout configuration** : Configuration des timeouts de connexion et de lecture
- **Retry mechanism** : Intégration avec Resilience4j pour retry automatique
- **Circuit Breaker** : Protection contre les services défaillants
- **Load Balancing** : Répartition des appels avec Spring Cloud LoadBalancer
- **Service Discovery** : Intégration avec Eureka ou Consul pour la résolution de services
- **Security** : Propagation du contexte de sécurité (JWT, OAuth2) via interceptors
- **Observability** : Ajout de tracing distribué avec Micrometer et correlation IDs

---

## **Questions/Réponses - Communication Inter-Services Synchrone**

### **Questions Générales**

**Q1 : Quelles sont les différentes approches pour implémenter la communication synchrone entre microservices dans Spring Boot ?**

**R :** Spring Boot offre cinq approches principales pour la communication synchrone :
- **RestTemplate** : L'approche historique, maintenant deprecated depuis Spring 5
- **Spring Cloud OpenFeign** : Une solution déclarative basée sur des interfaces annotées
- **RestClient** : L'alternative moderne recommandée depuis Spring 6.1 pour les appels synchrones
- **WebClient** : Support des appels réactifs et synchrones, idéal pour Spring WebFlux
- **HTTP Interfaces** : La nouvelle approche déclarative de Spring 6 combinant simplicité et modernité

Chaque approche a ses cas d'usage spécifiques selon les besoins du projet.

**Q2 : Pourquoi RestTemplate est-il deprecated et quelle est l'alternative recommandée ?**

**R :** RestTemplate est en mode maintenance depuis Spring 5 pour plusieurs raisons :
- API vieillissante et peu flexible
- Ne supporte pas la programmation réactive
- Gestion d'erreur limitée
- Code verbose et moins intuitif

Les alternatives recommandées sont :
- **RestClient** pour les appels synchrones simples (Spring Boot 3+)
- **WebClient** pour les besoins réactifs ou les appels parallèles
- **HTTP Interfaces** pour une approche moderne et déclarative

**Q3 : Quelle est la différence principale entre RestClient et WebClient ?**

**R :** Les différences principales sont :

**RestClient** :
- Conçu spécifiquement pour les appels synchrones/bloquants
- API plus simple et intuitive
- Meilleure performance pour les cas d'usage synchrones
- Idéal pour remplacer RestTemplate

**WebClient** :
- Support natif de la programmation réactive (Mono/Flux)
- Appels non-bloquants par défaut
- Meilleur pour les appels parallèles et le streaming
- Peut être utilisé en mode synchrone avec `.block()`
- Plus complexe mais plus puissant

---

### **Questions sur Spring Cloud OpenFeign**

**Q4 : Comment fonctionne Spring Cloud OpenFeign et quels sont ses avantages ?**

**R :** OpenFeign fonctionne selon le principe de programmation déclarative :
```java
@FeignClient(name = "user-service")
public interface UserClient {
    @GetMapping("/api/users/{id}")
    UserDTO getUser(@PathVariable Long id);
}
```

**Avantages** :
- Réduction significative du boilerplate code
- Intégration native avec Spring Cloud (service discovery, load balancing)
- Support du circuit breaker pattern
- Configuration centralisée via `application.yml`
- Gestion automatique de la sérialisation/désérialisation

**Q5 : Comment configurer un interceptor avec Feign pour ajouter des headers personnalisés ?**

**R :** Configuration d'un interceptor :
```java
@Configuration
public class FeignConfig {
    
    @Bean
    public RequestInterceptor requestInterceptor() {
        return requestTemplate -> {
            // Ajout de headers
            requestTemplate.header("Authorization", "Bearer " + getToken());
            requestTemplate.header("X-Tenant-ID", getTenantId());
            requestTemplate.header("X-Request-ID", UUID.randomUUID().toString());
        };
    }
}

// Application dans le client
@FeignClient(name = "order-service", configuration = FeignConfig.class)
public interface OrderClient {
    @GetMapping("/api/orders/{id}")
    OrderDTO getOrder(@PathVariable Long id);
}
```

**Q6 : Comment gérer les erreurs avec Feign et implémenter un fallback ?**

**R :** Deux approches principales :

**1. ErrorDecoder personnalisé :**
```java
@Bean
public ErrorDecoder errorDecoder() {
    return (methodKey, response) -> {
        switch (response.status()) {
            case 404:
                return new ResourceNotFoundException("Resource not found");
            case 503:
                return new ServiceUnavailableException("Service unavailable");
            case 401:
                return new UnauthorizedException("Unauthorized access");
            default:
                return new FeignException.FeignServerException(
                    response.status(),
                    response.reason(),
                    response.request(),
                    response.body()
                );
        }
    };
}
```

**2. Fallback avec Circuit Breaker :**
```java
@FeignClient(
    name = "payment-service",
    fallback = PaymentClientFallback.class
)
public interface PaymentClient {
    @PostMapping("/api/payments")
    PaymentResponse processPayment(@RequestBody PaymentRequest request);
}

@Component
public class PaymentClientFallback implements PaymentClient {
    @Override
    public PaymentResponse processPayment(PaymentRequest request) {
        // Comportement de repli
        return PaymentResponse.builder()
            .status("PENDING")
            .message("Payment service temporarily unavailable")
            .build();
    }
}
```

**Q7 : Comment configurer le retry mechanism avec Feign ?**

**R :** Configuration du retry :
```java
@Configuration
public class FeignConfig {
    
    @Bean
    public Retryer retryer() {
        // period: délai initial entre les tentatives (100ms)
        // maxPeriod: délai maximum (1000ms)
        // maxAttempts: nombre maximum de tentatives (3)
        return new Retryer.Default(100, 1000, 3);
    }
}
```

Configuration dans `application.yml` :
```yaml
feign:
  client:
    config:
      default:
        connectTimeout: 5000
        readTimeout: 5000
        loggerLevel: full
  circuitbreaker:
    enabled: true
```

---

### **Questions sur RestClient**

**Q8 : Comment créer et configurer un RestClient dans Spring Boot 3 ?**

**R :** Configuration de base :
```java
@Configuration
public class RestClientConfig {
    
    @Bean
    public RestClient restClient(RestClient.Builder builder) {
        return builder
            .baseUrl("http://api-service")
            .defaultHeader("Content-Type", "application/json")
            .defaultHeader("Accept", "application/json")
            .requestInterceptor((request, body, execution) -> {
                // Ajout de headers dynamiques
                request.getHeaders().setBearerAuth(getToken());
                request.getHeaders().set("X-Request-ID", UUID.randomUUID().toString());
                return execution.execute(request, body);
            })
            .build();
    }
}
```

**Q9 : Comment gérer les erreurs avec RestClient ?**

**R :** Gestion d'erreur avec `onStatus` :
```java
public Product createProduct(CreateProductRequest request) {
    return restClient.post()
        .uri("/api/products")
        .body(request)
        .retrieve()
        .onStatus(HttpStatusCode::is4xxClientError, (req, res) -> {
            String errorBody = new String(res.getBody().readAllBytes());
            throw new BadRequestException("Invalid request: " + errorBody);
        })
        .onStatus(HttpStatusCode::is5xxServerError, (req, res) -> {
            throw new ServiceException("Service error: " + res.getStatusCode());
        })
        .onStatus(status -> status.value() == 404, (req, res) -> {
            throw new ResourceNotFoundException("Product not found");
        })
        .body(Product.class);
}
```

**Q10 : Comment effectuer un appel avec query parameters et path variables avec RestClient ?**

**R :** Utilisation des URI builders :
```java
// Avec query parameters
public List<Product> searchProducts(String category, BigDecimal minPrice) {
    return restClient.get()
        .uri(uriBuilder -> uriBuilder
            .path("/api/products")
            .queryParam("category", category)
            .queryParam("minPrice", minPrice)
            .queryParam("available", true)
            .build())
        .retrieve()
        .body(new ParameterizedTypeReference<List<Product>>() {});
}

// Avec path variables
public Order getOrder(Long orderId) {
    return restClient.get()
        .uri("/api/orders/{id}", orderId)
        .retrieve()
        .body(Order.class);
}

// Combinaison des deux
public List<OrderItem> getOrderItems(Long orderId, String status) {
    return restClient.get()
        .uri(uriBuilder -> uriBuilder
            .path("/api/orders/{orderId}/items")
            .queryParam("status", status)
            .build(orderId))
        .retrieve()
        .body(new ParameterizedTypeReference<List<OrderItem>>() {});
}
```

---

### **Questions sur WebClient**

**Q11 : Quelle est la différence entre un appel synchrone et asynchrone avec WebClient ?**

**R :** 

**Appel synchrone (bloquant)** :
```java
public User getUserSync(Long userId) {
    return webClient.get()
        .uri("/api/users/{id}", userId)
        .retrieve()
        .bodyToMono(User.class)
        .block(); // Bloque le thread jusqu'à la réponse
}
```

**Appel asynchrone (non-bloquant)** :
```java
public Mono<User> getUserAsync(Long userId) {
    return webClient.get()
        .uri("/api/users/{id}", userId)
        .retrieve()
        .bodyToMono(User.class); // Retourne immédiatement un Mono
}

// Utilisation
userService.getUserAsync(123L)
    .subscribe(user -> System.out.println("User: " + user));
```

L'approche asynchrone ne bloque pas le thread, permettant de traiter d'autres requêtes pendant l'attente de la réponse.

**Q12 : Comment effectuer plusieurs appels parallèles avec WebClient ?**

**R :** Utilisation de `Mono.zip` ou `Flux.merge` :
```java
public Mono<OrderDetails> getCompleteOrderDetails(Long orderId) {
    // Trois appels en parallèle
    Mono<Order> orderMono = webClient.get()
        .uri("/api/orders/{id}", orderId)
        .retrieve()
        .bodyToMono(Order.class);
        
    Mono<Customer> customerMono = webClient.get()
        .uri("/api/customers/{id}", customerId)
        .retrieve()
        .bodyToMono(Customer.class);
        
    Mono<List<Product>> productsMono = webClient.get()
        .uri("/api/orders/{id}/products", orderId)
        .retrieve()
        .bodyToFlux(Product.class)
        .collectList();
    
    // Combine les résultats une fois tous disponibles
    return Mono.zip(orderMono, customerMono, productsMono)
        .map(tuple -> OrderDetails.builder()
            .order(tuple.getT1())
            .customer(tuple.getT2())
            .products(tuple.getT3())
            .build());
}
```

**Q13 : Comment configurer les timeouts avec WebClient ?**

**R :** Configuration des timeouts à plusieurs niveaux :
```java
@Configuration
public class WebClientConfig {
    
    @Bean
    public WebClient webClientWithTimeout() {
        HttpClient httpClient = HttpClient.create()
            // Timeout de connexion
            .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, 5000)
            // Timeout de réponse global
            .responseTimeout(Duration.ofSeconds(10))
            // Timeouts de lecture/écriture
            .doOnConnected(conn -> 
                conn.addHandlerLast(new ReadTimeoutHandler(10))
                    .addHandlerLast(new WriteTimeoutHandler(10))
            );
        
        return WebClient.builder()
            .clientConnector(new ReactorClientHttpConnector(httpClient))
            .build();
    }
}

// Timeout au niveau de la requête
public Mono<User> getUserWithTimeout(Long userId) {
    return webClient.get()
        .uri("/api/users/{id}", userId)
        .retrieve()
        .bodyToMono(User.class)
        .timeout(Duration.ofSeconds(3)); // Timeout spécifique à cette requête
}
```

**Q14 : Comment implémenter un retry mechanism avec WebClient ?**

**R :** Utilisation de `retryWhen` :
```java
public Mono<PaymentResponse> processPaymentWithRetry(PaymentRequest request) {
    return webClient.post()
        .uri("/api/payments")
        .bodyValue(request)
        .retrieve()
        .bodyToMono(PaymentResponse.class)
        .retryWhen(Retry.backoff(3, Duration.ofMillis(100))
            .maxBackoff(Duration.ofSeconds(2))
            .filter(throwable -> throwable instanceof WebClientResponseException.ServiceUnavailable)
            .onRetryExhaustedThrow((retryBackoffSpec, retrySignal) -> 
                new ServiceException("Payment service unavailable after retries")
            )
        )
        .timeout(Duration.ofSeconds(10));
}

// Avec Retry personnalisé
public Mono<Order> getOrderWithCustomRetry(Long orderId) {
    return webClient.get()
        .uri("/api/orders/{id}", orderId)
        .retrieve()
        .bodyToMono(Order.class)
        .retryWhen(Retry.fixedDelay(3, Duration.ofSeconds(1))
            .doBeforeRetry(signal -> 
                log.warn("Retry attempt {} for order {}", 
                    signal.totalRetries(), orderId)
            )
        );
}
```

**Q15 : Comment gérer le streaming de données avec WebClient ?**

**R :** Utilisation de `Flux` pour le streaming :
```java
// Streaming Server-Sent Events (SSE)
public Flux<Notification> streamNotifications(Long userId) {
    return webClient.get()
        .uri("/api/notifications/stream/{userId}", userId)
        .accept(MediaType.TEXT_EVENT_STREAM)
        .retrieve()
        .bodyToFlux(Notification.class)
        .doOnNext(notification -> log.info("Received: {}", notification))
        .doOnError(error -> log.error("Stream error", error))
        .doOnComplete(() -> log.info("Stream completed"));
}

// Consommation du stream
notificationService.streamNotifications(123L)
    .subscribe(
        notification -> processNotification(notification),
        error -> handleError(error),
        () -> log.info("Stream closed")
    );

// Upload de fichiers en streaming
public Mono<UploadResponse> uploadFileStream(FilePart filePart) {
    return webClient.post()
        .uri("/api/files/upload")
        .contentType(MediaType.MULTIPART_FORM_DATA)
        .body(BodyInserters.fromMultipartData("file", filePart))
        .retrieve()
        .bodyToMono(UploadResponse.class);
}
```

---

### **Questions sur HTTP Interfaces**

**Q16 : Qu'est-ce que HTTP Interfaces et quand l'utiliser ?**

**R :** HTTP Interfaces est une fonctionnalité de Spring Framework 6 qui permet de définir des clients HTTP de manière déclarative, similaire à Spring Data repositories.

**Caractéristiques** :
- Basé sur des interfaces Java annotées
- Génération automatique de l'implémentation au runtime
- Support des appels synchrones et réactifs
- Type-safe et moins verbeux que les autres approches

**Quand l'utiliser** :
- Nouveaux projets Spring Boot 3+ / Spring Framework 6+
- Besoin d'une API moderne et type-safe
- Volonté de réduire le boilerplate code
- Projets nécessitant flexibilité entre sync et async

```java
public interface UserService {
    @GetExchange("/api/users/{id}")
    User getUser(@PathVariable Long id);
    
    @PostExchange("/api/users")
    User createUser(@RequestBody CreateUserRequest request);
    
    // Support réactif
    @GetExchange("/api/users")
    Flux<User> getAllUsers();
}
```

**Q17 : Comment configurer un HTTP Interface avec WebClient ?**

**R :** Configuration complète :
```java
@Configuration
public class HttpInterfaceConfig {
    
    @Bean
    public UserService userService() {
        WebClient webClient = WebClient.builder()
            .baseUrl("http://user-service")
            .defaultHeader("Content-Type", "application/json")
            .filter((request, next) -> {
                ClientRequest filtered = ClientRequest.from(request)
                    .header("Authorization", "Bearer " + getToken())
                    .header("X-Correlation-ID", generateCorrelationId())
                    .build();
                return next.exchange(filtered);
            })
            .build();
        
        HttpServiceProxyFactory factory = HttpServiceProxyFactory
            .builder(WebClientAdapter.forClient(webClient))
            .build();
        
        return factory.createClient(UserService.class);
    }
    
    private String getToken() {
        // Récupération du token depuis SecurityContext
        return SecurityContextHolder.getContext()
            .getAuthentication()
            .getCredentials()
            .toString();
    }
    
    private String generateCorrelationId() {
        return UUID.randomUUID().toString();
    }
}
```

**Q18 : Comment gérer les erreurs avec HTTP Interfaces ?**

**R :** Gestion d'erreur via les filters WebClient :
```java
@Configuration
public class HttpInterfaceErrorHandling {
    
    @Bean
    public OrderService orderService() {
        WebClient webClient = WebClient.builder()
            .baseUrl("http://order-service")
            .filter((request, next) -> 
                next.exchange(request)
                    .onErrorResume(WebClientResponseException.NotFound.class, ex -> {
                        log.warn("Order not found: {}", ex.getMessage());
                        return Mono.error(new ResourceNotFoundException("Order not found"));
                    })
                    .onErrorResume(WebClientResponseException.BadRequest.class, ex -> {
                        log.error("Bad request: {}", ex.getResponseBodyAsString());
                        return Mono.error(new ValidationException(ex.getResponseBodyAsString()));
                    })
                    .onErrorResume(WebClientResponseException.ServiceUnavailable.class, ex -> {
                        log.error("Service unavailable");
                        return Mono.error(new ServiceUnavailableException("Order service unavailable"));
                    })
            )
            .filter((request, next) -> 
                next.exchange(request)
                    .retryWhen(Retry.backoff(3, Duration.ofMillis(100)))
                    .timeout(Duration.ofSeconds(5))
            )
            .build();
        
        HttpServiceProxyFactory factory = HttpServiceProxyFactory
            .builder(WebClientAdapter.forClient(webClient))
            .build();
        
        return factory.createClient(OrderService.class);
    }
}
```

---

### **Questions Avancées**

**Q19 : Comment propager le contexte de sécurité (JWT) entre microservices ?**

**R :** Propagation via interceptors :

**Avec RestClient :**
```java
@Configuration
public class SecurityPropagationConfig {
    
    @Bean
    public RestClient securedRestClient(RestClient.Builder builder) {
        return builder
            .requestInterceptor((request, body, execution) -> {
                String token = extractToken();
                if (token != null) {
                    request.getHeaders().setBearerAuth(token);
                }
                return execution.execute(request, body);
            })
            .build();
    }
    
    private String extractToken() {
        Authentication authentication = SecurityContextHolder
            .getContext()
            .getAuthentication();
            
        if (authentication instanceof JwtAuthenticationToken jwtAuth) {
            return jwtAuth.getToken().getTokenValue();
        }
        return null;
    }
}
```

**Avec OpenFeign :**
```java
@Configuration
public class FeignSecurityConfig {
    
    @Bean
    public RequestInterceptor oauth2FeignRequestInterceptor() {
        return requestTemplate -> {
            Authentication authentication = SecurityContextHolder
                .getContext()
                .getAuthentication();
                
            if (authentication != null && authentication.getCredentials() != null) {
                String token = authentication.getCredentials().toString();
                requestTemplate.header("Authorization", "Bearer " + token);
            }
        };
    }
}
```

**Avec WebClient :**
```java
@Bean
public WebClient securedWebClient() {
    return WebClient.builder()
        .filter((request, next) -> {
            return Mono.deferContextual(contextView -> {
                String token = contextView.getOrDefault("token", "");
                ClientRequest filtered = ClientRequest.from(request)
                    .header("Authorization", "Bearer " + token)
                    .build();
                return next.exchange(filtered);
            });
        })
        .build();
}
```

**Q20 : Comment implémenter le Circuit Breaker pattern avec ces différentes approches ?**

**R :** Intégration avec Resilience4j :

**Configuration des dépendances :**
```xml
<dependency>
    <groupId>io.github.resilience4j</groupId>
    <artifactId>resilience4j-spring-boot3</artifactId>
</dependency>
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-circuitbreaker-resilience4j</artifactId>
</dependency>
```

**Configuration dans application.yml :**
```yaml
resilience4j:
  circuitbreaker:
    instances:
      paymentService:
        registerHealthIndicator: true
        slidingWindowSize: 10
        minimumNumberOfCalls: 5
        permittedNumberOfCallsInHalfOpenState: 3
        automaticTransitionFromOpenToHalfOpenEnabled: true
        waitDurationInOpenState: 5s
        failureRateThreshold: 50
        eventConsumerBufferSize: 10
```

**Avec OpenFeign :**
```java
@FeignClient(
    name = "payment-service",
    fallback = PaymentServiceFallback.class
)
public interface PaymentService {
    @PostMapping("/api/payments")
    PaymentResponse processPayment(@RequestBody PaymentRequest request);
}

@Component
public class PaymentServiceFallback implements PaymentService {
    @Override
    public PaymentResponse processPayment(PaymentRequest request) {
        return PaymentResponse.builder()
            .status("PENDING")
            .message("Payment service temporarily unavailable")
            .build();
    }
}
```

**Avec RestClient/WebClient :**
```java
@Service
public class OrderService {
    private final CircuitBreakerFactory circuitBreakerFactory;
    private final RestClient restClient;
    
    public OrderService(CircuitBreakerFactory circuitBreakerFactory, 
                       RestClient restClient) {
        this.circuitBreakerFactory = circuitBreakerFactory;
        this.restClient = restClient;
    }
    
    public Order createOrder(CreateOrderRequest request) {
        CircuitBreaker circuitBreaker = circuitBreakerFactory.create("orderService");
        
        return circuitBreaker.run(
            () -> restClient.post()
                .uri("/api/orders")
                .body(request)
                .retrieve()
                .body(Order.class),
            throwable -> getFallbackOrder(request)
        );
    }
    
    private Order getFallbackOrder(CreateOrderRequest request) {
        return Order.builder()
            .status("PENDING")
            .message("Order creation delayed")
            .build();
    }
}
```

**Q21 : Comment implémenter le Load Balancing avec ces approches ?**

**R :** Intégration avec Spring Cloud LoadBalancer :

**Configuration :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-loadbalancer</artifactId>
</dependency>
```

**Avec OpenFeign (automatique avec service discovery) :**
```java
@FeignClient(name = "user-service") // Le nom du service enregistré dans Eureka
public interface UserClient {
    @GetMapping("/api/users/{id}")
    UserDTO getUser(@PathVariable Long id);
}
```

**Avec RestClient :**
```java
@Configuration
public class LoadBalancedRestClientConfig {
    
    @Bean
    @LoadBalanced
    public RestClient.Builder restClientBuilder() {
        return RestClient.builder();
    }
    
    @Bean
    public RestClient loadBalancedRestClient(RestClient.Builder builder) {
        return builder
            .baseUrl("http://user-service") // Nom du service, pas l'URL exacte
            .build();
    }
}
```

**Avec WebClient :**
```java
@Configuration
public class LoadBalancedWebClientConfig {
    
    @Bean
    @LoadBalanced
    public WebClient.Builder loadBalancedWebClientBuilder() {
        return WebClient.builder();
    }
    
    @Bean
    public WebClient loadBalancedWebClient(WebClient.Builder builder) {
        return builder
            .baseUrl("http://order-service")
            .build();
    }
}
```

**Q22 : Comment gérer les timeouts de manière optimale pour éviter les cascading failures ?**

**R :** Stratégie de timeout en cascade :

```java
@Configuration
public class TimeoutConfig {
    
    // Timeout court pour les opérations critiques
    @Bean("fastRestClient")
    public RestClient fastRestClient(RestClient.Builder builder) {
        return builder
            .requestFactory(clientHttpRequestFactory(2000, 2000))
            .build();
    }
    
    // Timeout moyen pour les opérations standards
    @Bean("standardRestClient")
    public RestClient standardRestClient(RestClient.Builder builder) {
        return builder
            .requestFactory(clientHttpRequestFactory(5000, 5000))
            .build();
    }
    
    // Timeout long pour les opérations batch
    @Bean("slowRestClient")
    public RestClient slowRestClient(RestClient.Builder builder) {
        return builder
            .requestFactory(clientHttpRequestFactory(30000, 30000))
            .build();
    }
    
    private ClientHttpRequestFactory clientHttpRequestFactory(
            int connectTimeout, 
            int readTimeout) {
        HttpComponentsClientHttpRequestFactory factory = 
            new HttpComponentsClientHttpRequestFactory();
        factory.setConnectTimeout(connectTimeout);
        factory.setConnectionRequestTimeout(readTimeout);
        return factory;
    }
}

// Utilisation
@Service
public class PaymentService {
    private final RestClient fastRestClient;
    private final RestClient standardRestClient;
    
    public PaymentService(@Qualifier("fastRestClient") RestClient fastRestClient,
                         @Qualifier("standardRestClient") RestClient standardRestClient) {
        this.fastRestClient = fastRestClient;
        this.standardRestClient = standardRestClient;
    }
    
    // Opération rapide critique
    public PaymentStatus checkPaymentStatus(Long paymentId) {
        return fastRestClient.get()
            .uri("/api/payments/{id}/status", paymentId)
            .retrieve()
            .body(PaymentStatus.class);
    }
    
    // Opération standard
    public PaymentResponse processPayment(PaymentRequest request) {
        return standardRestClient.post()
            .uri("/api/payments")
            .body(request)
            .retrieve()
            .body(PaymentResponse.class);
    }
}
```

**Configuration des timeouts en cascade :**
```yaml
# application.yml
service:
  timeouts:
    critical: 2000    # Opérations critiques: 2s
    standard: 5000    # Opérations standard: 5s
    batch: 30000      # Opérations batch: 30s
    
resilience4j:
  timelimiter:
    instances:
      paymentService:
        timeoutDuration: 3s
        cancelRunningFuture: true
```

**Q23 : Comment implémenter le monitoring et le tracing distribué pour ces communications ?**

**R :** Intégration avec Micrometer et tracing :

**Configuration des dépendances :**
```xml
<dependency>
    <groupId>io.micrometer</groupId>
    <artifactId>micrometer-tracing-bridge-brave</artifactId>
</dependency>
<dependency>
    <groupId>io.zipkin.reporter2</groupId>
    <artifactId>zipkin-reporter-brave</artifactId>
</dependency>
```

**Configuration RestClient avec observability :**
```java
@Configuration
public class ObservabilityConfig {
    
    @Bean
    public RestClient observableRestClient(
            RestClient.Builder builder,
            ObservationRegistry observationRegistry) {
        return builder
            .observationRegistry(observationRegistry)
            .requestInterceptor((request, body, execution) -> {
                // Ajout de correlation ID
                String correlationId = MDC.get("correlationId");
                if (correlationId == null) {
                    correlationId = UUID.randomUUID().toString();
                    MDC.put("correlationId", correlationId);
                }
                request.getHeaders().set("X-Correlation-ID", correlationId);
                
                // Logging
                log.info("Outgoing request: {} {}", 
                    request.getMethod(), 
                    request.getURI());
                
                ClientHttpResponse response = execution.execute(request, body);
                
                log.info("Response status: {}", response.getStatusCode());
                return response;
            })
            .build();
    }
}
```

**Configuration application.yml :**
```yaml
management:
  endpoints:
    web:
      exposure:
        include: health,info,metrics,prometheus
  metrics:
    distribution:
      percentiles-histogram:
        http.client.requests: true
  tracing:
    sampling:
      probability: 1.0
  zipkin:
    tracing:
      endpoint: http://localhost:9411/api/v2/spans
```

**Custom metrics :**
```java
@Service
public class MonitoredOrderService {
    private final RestClient restClient;
    private final MeterRegistry meterRegistry;
    private final Counter orderCreationCounter;
    private final Timer orderCreationTimer;
    
    public MonitoredOrderService(RestClient restClient, 
                                MeterRegistry meterRegistry) {
        this.restClient = restClient;
        this.meterRegistry = meterRegistry;
        
        this.orderCreationCounter = Counter.builder("orders.created")
            .description("Number of orders created")
            .tag("service", "order-service")
            .register(meterRegistry);
            
        this.orderCreationTimer = Timer.builder("orders.creation.duration")
            .description("Order creation duration")
            .register(meterRegistry);
    }
    
    public Order createOrder(CreateOrderRequest request) {
        return orderCreationTimer.record(() -> {
            try {
                Order order = restClient.post()
                    .uri("/api/orders")
                    .body(request)
                    .retrieve()
                    .body(Order.class);
                    
                orderCreationCounter.increment();
                return order;
            } catch (Exception e) {
                meterRegistry.counter("orders.creation.errors",
                    "error", e.getClass().getSimpleName()
                ).increment();
                throw e;
            }
        });
    }
}
```

**Q24 : Quelle est la meilleure approche pour gérer les appels avec pagination ?**

**R :** Implémentation de la pagination avec différentes approches :

**Avec RestClient :**
```java
@Service
public class ProductService {
    private final RestClient restClient;
    
    public Page<Product> getProducts(int page, int size, String sortBy) {
        return restClient.get()
            .uri(uriBuilder -> uriBuilder
                .path("/api/products")
                .queryParam("page", page)
                .queryParam("size", size)
                .queryParam("sort", sortBy)
                .build())
            .retrieve()
            .body(new ParameterizedTypeReference<PageImpl<Product>>() {});
    }
    
    // Récupération de toutes les pages automatiquement
    public List<Product> getAllProducts() {
        List<Product> allProducts = new ArrayList<>();
        int page = 0;
        Page<Product> productPage;
        
        do {
            productPage = getProducts(page, 100, "id");
            allProducts.addAll(productPage.getContent());
            page++;
        } while (!productPage.isLast());
        
        return allProducts;
    }
}
```

**Avec OpenFeign :**
```java
@FeignClient(name = "product-service")
public interface ProductClient {
    
    @GetMapping("/api/products")
    Page<Product> getProducts(
        @RequestParam("page") int page,
        @RequestParam("size") int size,
        @RequestParam("sort") String sort
    );
}

@Service
public class ProductService {
    private final ProductClient productClient;
    
    public Stream<Product> streamAllProducts() {
        return StreamSupport.stream(
            new PagedResourceIterable(0, 50),
            false
        );
    }
    
    private class PagedResourceIterable implements Iterable<Product> {
        private final int pageSize;
        private int currentPage;
        
        public PagedResourceIterable(int startPage, int pageSize) {
            this.currentPage = startPage;
            this.pageSize = pageSize;
        }
        
        @Override
        public Iterator<Product> iterator() {
            return new Iterator<>() {
                private Page<Product> currentPageData = null;
                private Iterator<Product> currentIterator = null;
                
                @Override
                public boolean hasNext() {
                    if (currentIterator != null && currentIterator.hasNext()) {
                        return true;
                    }
                    
                    if (currentPageData != null && currentPageData.isLast()) {
                        return false;
                    }
                    
                    currentPageData = productClient.getProducts(
                        currentPage++, pageSize, "id"
                    );
                    currentIterator = currentPageData.getContent().iterator();
                    
                    return currentIterator.hasNext();
                }
                
                @Override
                public Product next() {
                    return currentIterator.next();
                }
            };
        }
    }
}
```

**Avec WebClient (réactif) :**
```java
@Service
public class ReactiveProductService {
    private final WebClient webClient;
    
    public Flux<Product> getAllProductsReactive() {
        return fetchPage(0)
            .expand(page -> {
                if (page.isLast()) {
                    return Mono.empty();
                }
                return fetchPage(page.getNumber() + 1);
            })
            .flatMap(page -> Flux.fromIterable(page.getContent()));
    }
    
    private Mono<Page<Product>> fetchPage(int pageNumber) {
        return webClient.get()
            .uri(uriBuilder -> uriBuilder
                .path("/api/products")
                .queryParam("page", pageNumber)
                .queryParam("size", 100)
                .build())
            .retrieve()
            .bodyToMono(new ParameterizedTypeReference<PageImpl<Product>>() {})
            .doOnNext(page -> log.info("Fetched page {} of {}", 
                page.getNumber(), page.getTotalPages()));
    }
}
```

**Q25 : Comment tester efficacement les clients HTTP ?**

**R :** Stratégies de test avec MockWebServer et WireMock :

**Test avec MockWebServer (WebClient) :**
```java
@SpringBootTest
class WebClientTest {
    private MockWebServer mockWebServer;
    private WebClient webClient;
    
    @BeforeEach
    void setUp() throws IOException {
        mockWebServer = new MockWebServer();
        mockWebServer.start();
        
        webClient = WebClient.builder()
            .baseUrl(mockWebServer.url("/").toString())
            .build();
    }
    
    @AfterEach
    void tearDown() throws IOException {
        mockWebServer.shutdown();
    }
    
    @Test
    void shouldGetUser() {
        // Given
        User expectedUser = new User(1L, "John Doe", "john@example.com");
        mockWebServer.enqueue(new MockResponse()
            .setBody(asJsonString(expectedUser))
            .addHeader("Content-Type", "application/json")
            .setResponseCode(200));
        
        // When
        User actualUser = webClient.get()
            .uri("/api/users/1")
            .retrieve()
            .bodyToMono(User.class)
            .block();
        
        // Then
        assertThat(actualUser).isEqualTo(expectedUser);
        
        RecordedRequest recordedRequest = mockWebServer.takeRequest();
        assertThat(recordedRequest.getMethod()).isEqualTo("GET");
        assertThat(recordedRequest.getPath()).isEqualTo("/api/users/1");
    }
    
    @Test
    void shouldHandleNotFoundError() {
        // Given
        mockWebServer.enqueue(new MockResponse()
            .setResponseCode(404)
            .setBody("User not found"));
        
        // When/Then
        assertThatThrownBy(() -> 
            webClient.get()
                .uri("/api/users/999")
                .retrieve()
                .bodyToMono(User.class)
                .block()
        ).isInstanceOf(WebClientResponseException.NotFound.class);
    }
}
```

**Test avec WireMock (OpenFeign) :**
```java
@SpringBootTest
@AutoConfigureWireMock(port = 0)
class FeignClientTest {
    
    @Autowired
    private UserClient userClient;
    
    @Test
    void shouldGetUser() {
        // Given
        User expectedUser = new User(1L, "John Doe", "john@example.com");
        
        stubFor(get(urlEqualTo("/api/users/1"))
            .willReturn(aResponse()
                .withStatus(200)
                .withHeader("Content-Type", "application/json")
                .withBody(asJsonString(expectedUser))));
        
        // When
        User actualUser = userClient.getUser(1L);
        
        // Then
        assertThat(actualUser).isEqualTo(expectedUser);
        
        verify(getRequestedFor(urlEqualTo("/api/users/1"))
            .withHeader("Content-Type", equalTo("application/json")));
    }
    
    @Test
    void shouldRetryOnServerError() {
        // Given
        stubFor(get(urlEqualTo("/api/users/1"))
            .inScenario("Retry Scenario")
            .whenScenarioStateIs(STARTED)
            .willReturn(aResponse().withStatus(503))
            .willSetStateTo("First Retry"));
            
        stubFor(get(urlEqualTo("/api/users/1"))
            .inScenario("Retry Scenario")
            .whenScenarioStateIs("First Retry")
            .willReturn(aResponse().withStatus(503))
            .willSetStateTo("Second Retry"));
            
        stubFor(get(urlEqualTo("/api/users/1"))
            .inScenario("Retry Scenario")
            .whenScenarioStateIs("Second Retry")
            .willReturn(aResponse()
                .withStatus(200)
                .withBody(asJsonString(new User(1L, "John", "john@example.com")))));
        
        // When
        User user = userClient.getUser(1L);
        
        // Then
        assertThat(user).isNotNull();
        verify(exactly(3), getRequestedFor(urlEqualTo("/api/users/1")));
    }
}
```

**Test d'intégration avec @RestClientTest :**
```java
@RestClientTest
class RestClientIntegrationTest {
    
    @Autowired
    private MockRestServiceServer mockServer;
    
    @Autowired
    private RestClient.Builder restClientBuilder;
    
    private RestClient restClient;
    
    @BeforeEach
    void setUp() {
        restClient = restClientBuilder
            .baseUrl("http://localhost:8080")
            .build();
    }
    
    @Test
    void shouldCreateOrder() {
        // Given
        CreateOrderRequest request = new CreateOrderRequest(/* ... */);
        Order expectedOrder = new Order(1L, "CREATED", /* ... */);
        
        mockServer.expect(requestTo("http://localhost:8080/api/orders"))
            .andExpect(method(HttpMethod.POST))
            .andExpect(content().json(asJsonString(request)))
            .andRespond(withSuccess(asJsonString(expectedOrder), 
                MediaType.APPLICATION_JSON));
        
        // When
        Order actualOrder = restClient.post()
            .uri("/api/orders")
            .body(request)
            .retrieve()
            .body(Order.class);
        
        // Then
        assertThat(actualOrder).isEqualTo(expectedOrder);
        mockServer.verify();
    }
}
```

---

### **Questions de Comparaison et Choix**

**Q26 : Dans quel cas choisir OpenFeign plutôt que RestClient ?**

**R :** 

**Choisir OpenFeign quand :**
- Architecture microservices avec Spring Cloud
- Besoin de service discovery (Eureka, Consul)
- Load balancing automatique requis
- Circuit breaker pattern nécessaire
- Multiples services à consommer
- Équipe habituée à l'approche déclarative
- Intégration avec Spring Cloud Gateway

**Exemple de cas d'usage** :
```java
// Avec OpenFeign - Idéal pour microservices
@FeignClient(
    name = "order-service",
    fallback = OrderServiceFallback.class
)
public interface OrderService {
    @GetMapping("/api/orders/{id}")
    Order getOrder(@PathVariable Long id);
}
```

**Choisir RestClient quand :**
- Application monolithique ou architecture simple
- Appels vers APIs externes (non Spring Cloud)
- Spring Boot 3+ avec besoin d'une API moderne
- Contrôle fin sur chaque requête
- Performance optimale pour appels synchrones
- Pas besoin de service discovery

**Exemple de cas d'usage** :
```java
// Avec RestClient - Idéal pour APIs externes
@Service
public class ExternalPaymentService {
    private final RestClient restClient;
    
    public PaymentResponse processPayment(PaymentRequest request) {
        return restClient.post()
            .uri("/api/v1/payments")
            .header("API-Key", apiKey)
            .body(request)
            .retrieve()
            .body(PaymentResponse.class);
    }
}
```

**Q27 : Quelles sont les meilleures pratiques pour la communication inter-services synchrone ?**

**R :** 

**1. Gestion des timeouts :**
```java
// Toujours définir des timeouts appropriés
@Configuration
public class ClientConfig {
    @Bean
    public RestClient restClient() {
        return RestClient.builder()
            .requestFactory(new HttpComponentsClientHttpRequestFactory() {{
                setConnectTimeout(2000);
                setConnectionRequestTimeout(5000);
            }})
            .build();
    }
}
```

**2. Retry avec backoff exponentiel :**
```java
// Ne pas surcharger un service déjà en difficulté
public Order getOrder(Long orderId) {
    return webClient.get()
        .uri("/api/orders/{id}", orderId)
        .retrieve()
        .bodyToMono(Order.class)
        .retryWhen(Retry.backoff(3, Duration.ofMillis(100))
            .maxBackoff(Duration.ofSeconds(2)))
        .block();
}
```

**3. Circuit Breaker :**
```java
// Éviter les cascading failures
@Service
public class ProductService {
    private final CircuitBreaker circuitBreaker;
    
    public Product getProduct(Long id) {
        return circuitBreaker.executeSupplier(() -> 
            restClient.get()
                .uri("/api/products/{id}", id)
                .retrieve()
                .body(Product.class)
        );
    }
}
```

**4. Idempotence :**
```java
// Utiliser des idempotency keys pour les opérations critiques
public PaymentResponse processPayment(PaymentRequest request) {
    String idempotencyKey = UUID.randomUUID().toString();
    
    return restClient.post()
        .uri("/api/payments")
        .header("Idempotency-Key", idempotencyKey)
        .body(request)
        .retrieve()
        .body(PaymentResponse.class);
}
```

**5. Correlation IDs pour le tracing :**
```java
// Faciliter le debugging distribué
@Component
public class CorrelationIdInterceptor implements ClientHttpRequestInterceptor {
    @Override
    public ClientHttpResponse intercept(HttpRequest request, byte[] body, 
                                       ClientHttpRequestExecution execution) {
        String correlationId = MDC.get("correlationId");
        if (correlationId == null) {
            correlationId = UUID.randomUUID().toString();
        }
        request.getHeaders().set("X-Correlation-ID", correlationId);
        return execution.execute(request, body);
    }
}
```

**6. Gestion centralisée des erreurs :**
```java
@ControllerAdvice
public class GlobalExceptionHandler {
    
    @ExceptionHandler(WebClientResponseException.class)
    public ResponseEntity<ErrorResponse> handleWebClientException(
            WebClientResponseException ex) {
        ErrorResponse error = ErrorResponse.builder()
            .status(ex.getStatusCode().value())
            .message(ex.getMessage())
            .timestamp(LocalDateTime.now())
            .build();
        return ResponseEntity.status(ex.getStatusCode()).body(error);
    }
}
```

**7. Connection pooling :**
```java
@Configuration
public class ConnectionPoolConfig {
    @Bean
    public WebClient webClient() {
        ConnectionProvider provider = ConnectionProvider.builder("custom")
            .maxConnections(500)
            .maxIdleTime(Duration.ofSeconds(20))
            .maxLifeTime(Duration.ofSeconds(60))
            .pendingAcquireTimeout(Duration.ofSeconds(60))
            .evictInBackground(Duration.ofSeconds(120))
            .build();
            
        HttpClient httpClient = HttpClient.create(provider);
        
        return WebClient.builder()
            .clientConnector(new ReactorClientHttpConnector(httpClient))
            .build();
    }
}
```

**8. Logging et monitoring :**
```java
@Component
public class LoggingInterceptor implements ClientHttpRequestInterceptor {
    private static final Logger log = LoggerFactory.getLogger(LoggingInterceptor.class);
    
    @Override
    public ClientHttpResponse intercept(HttpRequest request, byte[] body,
                                       ClientHttpRequestExecution execution) 
            throws IOException {
        long startTime = System.currentTimeMillis();
        
        log.info("Request: {} {} ", request.getMethod(), request.getURI());
        
        ClientHttpResponse response = execution.execute(request, body);
        
        long duration = System.currentTimeMillis() - startTime;
        log.info("Response: {} in {}ms", response.getStatusCode(), duration);
        
        return response;
    }
}
```

**Q28 : Comment gérer l'authentification OAuth2 avec ces différents clients ?**

**R :** 

**Avec RestClient et OAuth2 :**
```java
@Configuration
public class OAuth2RestClientConfig {
    
    @Bean
    public RestClient oauth2RestClient(OAuth2AuthorizedClientManager clientManager) {
        OAuth2ClientHttpRequestInterceptor interceptor = 
            new OAuth2ClientHttpRequestInterceptor(clientManager);
            
        return RestClient.builder()
            .requestInterceptor(interceptor)
            .build();
    }
    
    @Bean
    public OAuth2AuthorizedClientManager authorizedClientManager(
            ClientRegistrationRepository clientRegistrationRepository,
            OAuth2AuthorizedClientRepository authorizedClientRepository) {
        
        OAuth2AuthorizedClientProvider authorizedClientProvider =
            OAuth2AuthorizedClientProviderBuilder.builder()
                .clientCredentials()
                .refreshToken()
                .build();
                
        DefaultOAuth2AuthorizedClientManager authorizedClientManager =
            new DefaultOAuth2AuthorizedClientManager(
                clientRegistrationRepository, 
                authorizedClientRepository
            );
            
        authorizedClientManager.setAuthorizedClientProvider(authorizedClientProvider);
        
        return authorizedClientManager;
    }
}

// Interceptor personnalisé
public class OAuth2ClientHttpRequestInterceptor 
        implements ClientHttpRequestInterceptor {
    
    private final OAuth2AuthorizedClientManager clientManager;
    
    @Override
    public ClientHttpResponse intercept(HttpRequest request, byte[] body,
                                       ClientHttpRequestExecution execution) 
            throws IOException {
        OAuth2AuthorizeRequest authorizeRequest = 
            OAuth2AuthorizeRequest.withClientRegistrationId("my-client")
                .principal("service-account")
                .build();
                
        OAuth2AuthorizedClient authorizedClient = 
            clientManager.authorize(authorizeRequest);
            
        if (authorizedClient != null) {
            String token = authorizedClient.getAccessToken().getTokenValue();
            request.getHeaders().setBearerAuth(token);
        }
        
        return execution.execute(request, body);
    }
}
```

**Avec WebClient et OAuth2 :**
```java
@Configuration
public class OAuth2WebClientConfig {
    
    @Bean
    public WebClient oauth2WebClient(
            ReactiveClientRegistrationRepository clientRegistrations,
            ServerOAuth2AuthorizedClientRepository authorizedClients) {
        
        ServerOAuth2AuthorizedClientExchangeFilterFunction oauth2 = 
            new ServerOAuth2AuthorizedClientExchangeFilterFunction(
                clientRegistrations, 
                authorizedClients
            );
            
        oauth2.setDefaultClientRegistrationId("my-client");
        
        return WebClient.builder()
            .filter(oauth2)
            .build();
    }
}

// Utilisation
@Service
public class SecuredApiService {
    private final WebClient webClient;
    
    public Mono<Data> fetchSecuredData() {
        return webClient.get()
            .uri("/api/secured/data")
            .attributes(clientRegistrationId("my-client"))
            .retrieve()
            .bodyToMono(Data.class);
    }
}
```

**Configuration OAuth2 dans application.yml :**
```yaml
spring:
  security:
    oauth2:
      client:
        registration:
          my-client:
            client-id: ${CLIENT_ID}
            client-secret: ${CLIENT_SECRET}
            authorization-grant-type: client_credentials
            scope: read,write
        provider:
          my-provider:
            token-uri: https://auth-server.com/oauth/token
```

**Q29 : Comment optimiser les performances des appels inter-services ?**

**R :** 

**1. Connection Pooling optimisé :**
```java
@Configuration
public class PerformanceConfig {
    
    @Bean
    public WebClient highPerformanceWebClient() {
        ConnectionProvider provider = ConnectionProvider.builder("optimized")
            .maxConnections(1000)  // Augmenter selon la charge
            .pendingAcquireMaxCount(2000)
            .pendingAcquireTimeout(Duration.ofSeconds(45))
            .maxIdleTime(Duration.ofSeconds(30))
            .build();
            
        HttpClient httpClient = HttpClient.create(provider)
            .option(ChannelOption.SO_KEEPALIVE, true)
            .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, 3000);
            
        return WebClient.builder()
            .clientConnector(new ReactorClientHttpConnector(httpClient))
            .build();
    }
}
```

**2. Appels parallèles :**
```java
// Mauvaise approche - Séquentiel
public OrderDetails getOrderDetailsSequential(Long orderId) {
    Order order = getOrder(orderId);          // 100ms
    Customer customer = getCustomer(customerId);  // 100ms
    List<Product> products = getProducts(orderId); // 100ms
    // Total: 300ms
    return new OrderDetails(order, customer, products);
}

// Bonne approche - Parallèle
public Mono<OrderDetails> getOrderDetailsParallel(Long orderId) {
    Mono<Order> orderMono = getOrderAsync(orderId);
    Mono<Customer> customerMono = getCustomerAsync(customerId);
    Mono<List<Product>> productsMono = getProductsAsync(orderId);
    
    return Mono.zip(orderMono, customerMono, productsMono)
        .map(tuple -> new OrderDetails(
            tuple.getT1(), 
            tuple.getT2(), 
            tuple.getT3()
        ));
    // Total: ~100ms (temps du plus lent)
}
```

**3. Caching stratégique :**
```java
@Service
@CacheConfig(cacheNames = "products")
public class ProductService {
    
    @Cacheable(key = "#id", unless = "#result == null")
    public Product getProduct(Long id) {
        return restClient.get()
            .uri("/api/products/{id}", id)
            .retrieve()
            .body(Product.class);
    }
    
    @CacheEvict(key = "#id")
    public void invalidateProduct(Long id) {
        // Cache invalidation
    }
}

@Configuration
@EnableCaching
public class CacheConfig {
    
    @Bean
    public CacheManager cacheManager() {
        return CaffeineCacheManager.builder()
            .caffeine(Caffeine.newBuilder()
                .maximumSize(1000)
                .expireAfterWrite(Duration.ofMinutes(10))
                .recordStats())
            .build();
    }
}
```

**4. Response compression :**
```java
@Bean
public WebClient compressedWebClient() {
    return WebClient.builder()
        .defaultHeader(HttpHeaders.ACCEPT_ENCODING, "gzip, deflate")
        .codecs(configurer -> configurer
            .defaultCodecs()
            .maxInMemorySize(16 * 1024 * 1024)) // 16MB buffer
        .build();
}
```

**5. Bulk operations :**
```java
// Mauvaise approche - Multiples appels
public List<User> getUsersOneByOne(List<Long> userIds) {
    return userIds.stream()
        .map(id -> restClient.get()
            .uri("/api/users/{id}", id)
            .retrieve()
            .body(User.class))
        .collect(Collectors.toList());
    // N appels HTTP
}

// Bonne approche - Appel groupé
public List<User> getUsersBulk(List<Long> userIds) {
    return restClient.post()
        .uri("/api/users/bulk")
        .body(userIds)
        .retrieve()
        .body(new ParameterizedTypeReference<List<User>>() {});
    // 1 appel HTTP
}
```

---

Ce guide complet de questions/réponses couvre tous les aspects essentiels de la communication inter-services synchrone dans Spring Boot. Il vous permettra de démontrer votre expertise technique lors de l'entretien !