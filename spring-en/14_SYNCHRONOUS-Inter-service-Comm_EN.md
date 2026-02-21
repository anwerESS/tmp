## [**..**](./00_index.md)

## **Synchronous Inter-Service Communication in Spring Boot Microservices**

Synchronous communication between microservices is a key building block of distributed architectures. Spring Boot provides several mechanisms to implement this kind of communication, each with its own advantages and specific use cases.

### **RestTemplate - The classic approach**

RestTemplate is the traditional way to make synchronous HTTP calls. This API provides a simple interface for CRUD operations:

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

Despite its simplicity, RestTemplate has some limitations: verbose code, manual error handling, and deprecated status since Spring 5. This API is in maintenance mode and is no longer recommended for new development.

### **Spring Cloud OpenFeign - The declarative approach**

Spring Cloud OpenFeign provides a high-level abstraction based on annotated interfaces, significantly reducing boilerplate code:

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

// Usage in a service
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

Advanced configuration with interceptors and error handling:

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

Integration with Circuit Breaker (Resilience4j):

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

### **RestClient - The modern alternative**

Introduced in Spring Framework 6.1, RestClient offers a fluent and modern API for synchronous HTTP calls:

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
    
    // POST request with error handling
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
    
    // GET with ResponseEntity
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

### **WebClient - For reactive programming**

WebClient supports both reactive (non-blocking) and synchronous calls, offering great flexibility:

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
    
    // Synchronous call with block()
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
    
    // Reactive call (non-blocking)
    public Mono<PaymentResponse> processPaymentAsync(PaymentRequest request) {
        return webClient.post()
            .uri("/api/payments")
            .bodyValue(request)
            .retrieve()
            .bodyToMono(PaymentResponse.class)
            .retryWhen(Retry.backoff(3, Duration.ofMillis(100)))
            .timeout(Duration.ofSeconds(5));
    }
    
    // Parallel calls
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
    
    // Data streaming
    public Flux<Notification> streamNotifications() {
        return webClient.get()
            .uri("/api/notifications/stream")
            .accept(MediaType.TEXT_EVENT_STREAM)
            .retrieve()
            .bodyToFlux(Notification.class);
    }
}
```

Advanced configuration with timeout and retry:

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

### **HTTP Interfaces - The modern Spring 6 approach**

HTTP Interfaces combine the simplicity of declarative interfaces with the power of WebClient:

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
    
    // Support for reactive calls
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

Configuration with interceptors and error handling:

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

### **Comparison and technical choice**

| Criteria | RestTemplate | OpenFeign | RestClient | WebClient | HTTP Interfaces |
|---------|-------------|-----------|------------|-----------|-----------------|
| **Status** | Deprecated | Active | Recommended | Active | Modern (Spring 6+) |
| **Approach** | Imperative | Declarative | Fluent | Reactive/Blocking | Declarative |
| **Complexity** | Simple | Medium | Simple | High | Simple |
| **Performance** | Average | Average | Good | Excellent | Excellent |
| **Use Case** | Legacy | Microservices | Simple calls | Reactive/Parallel | Modern projects |

**Recommended use cases:**

- **RestClient**: Simple synchronous HTTP calls in Spring Boot 3+, replaces RestTemplate
- **OpenFeign**: Complex microservice architectures requiring service discovery, load balancing, circuit breaker
- **WebClient**: Reactive applications with Spring WebFlux, parallel calls, data streaming, high performance
- **HTTP Interfaces**: New Spring 6+ projects looking for a modern, type-safe, declarative API

**Common cross-cutting concerns:**

- **Error handling**: Using `@ControllerAdvice` and `@ExceptionHandler` on the server side
- **Timeout configuration**: Setting connection and read timeouts
- **Retry mechanism**: Integration with Resilience4j for automatic retry
- **Circuit Breaker**: Protection against failing services
- **Load Balancing**: Request distribution with Spring Cloud LoadBalancer
- **Service Discovery**: Integration with Eureka or Consul for service resolution
- **Security**: Propagating security context (JWT, OAuth2) via interceptors
- **Observability**: Adding distributed tracing with Micrometer and correlation IDs

---

## **Q&A - Synchronous Inter-Service Communication**

### **General Questions**

**Q1: What are the different approaches to implement synchronous communication between microservices in Spring Boot?**

**A:** Spring Boot provides five main approaches for synchronous communication:
- **RestTemplate**: The historic approach, now deprecated since Spring 5
- **Spring Cloud OpenFeign**: A declarative solution based on annotated interfaces
- **RestClient**: The recommended modern alternative since Spring 6.1 for synchronous calls
- **WebClient**: Supports both reactive and synchronous calls, ideal for Spring WebFlux
- **HTTP Interfaces**: The new declarative approach in Spring 6, combining simplicity and modernity

Each approach has specific use cases depending on the project's needs.

**Q2: Why is RestTemplate deprecated and what is the recommended alternative?**

**A:** RestTemplate has been in maintenance mode since Spring 5 for several reasons:
- Aging API that lacks flexibility
- Does not support reactive programming
- Limited error handling
- Verbose and less intuitive code

The recommended alternatives are:
- **RestClient** for simple synchronous calls (Spring Boot 3+)
- **WebClient** for reactive needs or parallel calls
- **HTTP Interfaces** for a modern, declarative approach

**Q3: What is the main difference between RestClient and WebClient?**

**A:** The main differences are:

**RestClient**:
- Designed specifically for synchronous/blocking calls
- Simpler and more intuitive API
- Better performance for synchronous use cases
- Ideal to replace RestTemplate

**WebClient**:
- Native support for reactive programming (Mono/Flux)
- Non-blocking calls by default
- Better for parallel calls and streaming
- Can be used in synchronous mode with `.block()`
- More complex but more powerful

---

### **Questions about Spring Cloud OpenFeign**

**Q4: How does Spring Cloud OpenFeign work and what are its advantages?**

**A:** OpenFeign works based on the declarative programming principle:
```java
@FeignClient(name = "user-service")
public interface UserClient {
    @GetMapping("/api/users/{id}")
    UserDTO getUser(@PathVariable Long id);
}
```

**Advantages**:
- Significant reduction of boilerplate code
- Native integration with Spring Cloud (service discovery, load balancing)
- Circuit breaker pattern support
- Centralized configuration via `application.yml`
- Automatic serialization/deserialization handling

**Q5: How do you configure an interceptor with Feign to add custom headers?**

**A:** Interceptor configuration:
```java
@Configuration
public class FeignConfig {
    
    @Bean
    public RequestInterceptor requestInterceptor() {
        return requestTemplate -> {
            // Adding headers
            requestTemplate.header("Authorization", "Bearer " + getToken());
            requestTemplate.header("X-Tenant-ID", getTenantId());
            requestTemplate.header("X-Request-ID", UUID.randomUUID().toString());
        };
    }
}

// Applied in the client
@FeignClient(name = "order-service", configuration = FeignConfig.class)
public interface OrderClient {
    @GetMapping("/api/orders/{id}")
    OrderDTO getOrder(@PathVariable Long id);
}
```

**Q6: How do you handle errors with Feign and implement a fallback?**

**A:** Two main approaches:

**1. Custom ErrorDecoder:**
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

**2. Fallback with Circuit Breaker:**
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
        // Fallback behavior
        return PaymentResponse.builder()
            .status("PENDING")
            .message("Payment service temporarily unavailable")
            .build();
    }
}
```

**Q7: How do you configure the retry mechanism with Feign?**

**A:** Retry configuration:
```java
@Configuration
public class FeignConfig {
    
    @Bean
    public Retryer retryer() {
        // period: initial delay between attempts (100ms)
        // maxPeriod: maximum delay (1000ms)
        // maxAttempts: maximum number of attempts (3)
        return new Retryer.Default(100, 1000, 3);
    }
}
```

Configuration in `application.yml`:
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

### **Questions about RestClient**

**Q8: How do you create and configure a RestClient in Spring Boot 3?**

**A:** Basic configuration:
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
                // Adding dynamic headers
                request.getHeaders().setBearerAuth(getToken());
                request.getHeaders().set("X-Request-ID", UUID.randomUUID().toString());
                return execution.execute(request, body);
            })
            .build();
    }
}
```

**Q9: How do you handle errors with RestClient?**

**A:** Error handling with `onStatus`:
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

**Q10: How do you make a call with query parameters and path variables using RestClient?**

**A:** Using URI builders:
```java
// With query parameters
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

// With path variables
public Order getOrder(Long orderId) {
    return restClient.get()
        .uri("/api/orders/{id}", orderId)
        .retrieve()
        .body(Order.class);
}

// Combining both
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

### **Questions about WebClient**

**Q11: What is the difference between a synchronous and asynchronous call with WebClient?**

**A:** 

**Synchronous call (blocking)**:
```java
public User getUserSync(Long userId) {
    return webClient.get()
        .uri("/api/users/{id}", userId)
        .retrieve()
        .bodyToMono(User.class)
        .block(); // Blocks the thread until the response arrives
}
```

**Asynchronous call (non-blocking)**:
```java
public Mono<User> getUserAsync(Long userId) {
    return webClient.get()
        .uri("/api/users/{id}", userId)
        .retrieve()
        .bodyToMono(User.class); // Returns a Mono immediately
}

// Usage
userService.getUserAsync(123L)
    .subscribe(user -> System.out.println("User: " + user));
```

The async approach does not block the thread, allowing other requests to be processed while waiting for the response.

**Q12: How do you make multiple parallel calls with WebClient?**

**A:** Using `Mono.zip` or `Flux.merge`:
```java
public Mono<OrderDetails> getCompleteOrderDetails(Long orderId) {
    // Three parallel calls
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
    
    // Combines results once all are available
    return Mono.zip(orderMono, customerMono, productsMono)
        .map(tuple -> OrderDetails.builder()
            .order(tuple.getT1())
            .customer(tuple.getT2())
            .products(tuple.getT3())
            .build());
}
```

**Q13: How do you configure timeouts with WebClient?**

**A:** Timeout configuration at multiple levels:
```java
@Configuration
public class WebClientConfig {
    
    @Bean
    public WebClient webClientWithTimeout() {
        HttpClient httpClient = HttpClient.create()
            // Connection timeout
            .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, 5000)
            // Global response timeout
            .responseTimeout(Duration.ofSeconds(10))
            // Read/write timeouts
            .doOnConnected(conn -> 
                conn.addHandlerLast(new ReadTimeoutHandler(10))
                    .addHandlerLast(new WriteTimeoutHandler(10))
            );
        
        return WebClient.builder()
            .clientConnector(new ReactorClientHttpConnector(httpClient))
            .build();
    }
}

// Per-request timeout
public Mono<User> getUserWithTimeout(Long userId) {
    return webClient.get()
        .uri("/api/users/{id}", userId)
        .retrieve()
        .bodyToMono(User.class)
        .timeout(Duration.ofSeconds(3)); // Timeout specific to this request
}
```

**Q14: How do you implement a retry mechanism with WebClient?**

**A:** Using `retryWhen`:
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

// With custom Retry
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

**Q15: How do you handle data streaming with WebClient?**

**A:** Using `Flux` for streaming:
```java
// Server-Sent Events (SSE) streaming
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

// Consuming the stream
notificationService.streamNotifications(123L)
    .subscribe(
        notification -> processNotification(notification),
        error -> handleError(error),
        () -> log.info("Stream closed")
    );

// File upload via streaming
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

### **Questions about HTTP Interfaces**

**Q16: What are HTTP Interfaces and when should you use them?**

**A:** HTTP Interfaces is a Spring Framework 6 feature that allows you to define HTTP clients in a declarative way, similar to Spring Data repositories.

**Characteristics**:
- Based on annotated Java interfaces
- Automatic implementation generation at runtime
- Support for both synchronous and reactive calls
- Type-safe and less verbose than other approaches

**When to use it**:
- New Spring Boot 3+ / Spring Framework 6+ projects
- Need for a modern, type-safe API
- Want to reduce boilerplate code
- Projects that need flexibility between sync and async

```java
public interface UserService {
    @GetExchange("/api/users/{id}")
    User getUser(@PathVariable Long id);
    
    @PostExchange("/api/users")
    User createUser(@RequestBody CreateUserRequest request);
    
    // Reactive support
    @GetExchange("/api/users")
    Flux<User> getAllUsers();
}
```

**Q17: How do you configure an HTTP Interface with WebClient?**

**A:** Full configuration:
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
        // Get token from SecurityContext
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

**Q18: How do you handle errors with HTTP Interfaces?**

**A:** Error handling via WebClient filters:
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

### **Advanced Questions**

**Q19: How do you propagate the security context (JWT) between microservices?**

**A:** Propagation via interceptors:

**With RestClient:**
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

**With OpenFeign:**
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

**With WebClient:**
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

**Q20: How do you implement the Circuit Breaker pattern with these different approaches?**

**A:** Integration with Resilience4j:

**Dependency configuration:**
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

**Configuration in application.yml:**
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

**With OpenFeign:**
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

**With RestClient/WebClient:**
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

**Q21: How do you implement Load Balancing with these approaches?**

**A:** Integration with Spring Cloud LoadBalancer:

**Configuration:**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-loadbalancer</artifactId>
</dependency>
```

**With OpenFeign (automatic with service discovery):**
```java
@FeignClient(name = "user-service") // The service name registered in Eureka
public interface UserClient {
    @GetMapping("/api/users/{id}")
    UserDTO getUser(@PathVariable Long id);
}
```

**With RestClient:**
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
            .baseUrl("http://user-service") // Service name, not the actual URL
            .build();
    }
}
```

**With WebClient:**
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

**Q22: How do you manage timeouts properly to avoid cascading failures?**

**A:** Cascading timeout strategy:

```java
@Configuration
public class TimeoutConfig {
    
    // Short timeout for critical operations
    @Bean("fastRestClient")
    public RestClient fastRestClient(RestClient.Builder builder) {
        return builder
            .requestFactory(clientHttpRequestFactory(2000, 2000))
            .build();
    }
    
    // Medium timeout for standard operations
    @Bean("standardRestClient")
    public RestClient standardRestClient(RestClient.Builder builder) {
        return builder
            .requestFactory(clientHttpRequestFactory(5000, 5000))
            .build();
    }
    
    // Long timeout for batch operations
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

// Usage
@Service
public class PaymentService {
    private final RestClient fastRestClient;
    private final RestClient standardRestClient;
    
    public PaymentService(@Qualifier("fastRestClient") RestClient fastRestClient,
                         @Qualifier("standardRestClient") RestClient standardRestClient) {
        this.fastRestClient = fastRestClient;
        this.standardRestClient = standardRestClient;
    }
    
    // Fast critical operation
    public PaymentStatus checkPaymentStatus(Long paymentId) {
        return fastRestClient.get()
            .uri("/api/payments/{id}/status", paymentId)
            .retrieve()
            .body(PaymentStatus.class);
    }
    
    // Standard operation
    public PaymentResponse processPayment(PaymentRequest request) {
        return standardRestClient.post()
            .uri("/api/payments")
            .body(request)
            .retrieve()
            .body(PaymentResponse.class);
    }
}
```

**Cascading timeout configuration:**
```yaml
# application.yml
service:
  timeouts:
    critical: 2000    # Critical operations: 2s
    standard: 5000    # Standard operations: 5s
    batch: 30000      # Batch operations: 30s
    
resilience4j:
  timelimiter:
    instances:
      paymentService:
        timeoutDuration: 3s
        cancelRunningFuture: true
```

**Q23: How do you implement monitoring and distributed tracing for these communications?**

**A:** Integration with Micrometer and tracing:

**Dependency configuration:**
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

**RestClient configuration with observability:**
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
                // Adding correlation ID
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

**application.yml configuration:**
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

**Custom metrics:**
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

**Q24: What is the best approach for handling paginated calls?**

**A:** Pagination implementation with different approaches:

**With RestClient:**
```java
@Service
public class ProductService {
    
    private RestClient restClient;
    
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
    
    // Automatically fetch all pages
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

**With OpenFeign:**
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

**With WebClient (reactive):**
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

**Q25: How do you effectively test HTTP clients?**

**A:** Testing strategies with MockWebServer and WireMock:

**Test with MockWebServer (WebClient):**
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

**Test with WireMock (OpenFeign):**
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

**Integration test with @RestClientTest:**
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

### **Comparison and Choice Questions**

**Q26: When should you choose OpenFeign over RestClient?**

**A:** 

**Choose OpenFeign when:**
- Microservice architecture with Spring Cloud
- Need for service discovery (Eureka, Consul)
- Automatic load balancing required
- Circuit breaker pattern needed
- Multiple services to consume
- Team is used to the declarative approach
- Integration with Spring Cloud Gateway

**Example use case:**
```java
// With OpenFeign - Ideal for microservices
@FeignClient(
    name = "order-service",
    fallback = OrderServiceFallback.class
)
public interface OrderService {
    @GetMapping("/api/orders/{id}")
    Order getOrder(@PathVariable Long id);
}
```

**Choose RestClient when:**
- Monolithic application or simple architecture
- Calls to external APIs (non Spring Cloud)
- Spring Boot 3+ with need for a modern API
- Fine-grained control over each request
- Best performance for synchronous calls
- No need for service discovery

**Example use case:**
```java
// With RestClient - Ideal for external APIs
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

**Q27: What are the best practices for synchronous inter-service communication?**

**A:** 

**1. Timeout management:**
```java
// Always set proper timeouts
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

**2. Retry with exponential backoff:**
```java
// Don't overload a service that's already struggling
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

**3. Circuit Breaker:**
```java
// Avoid cascading failures
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

**4. Idempotence:**
```java
// Use idempotency keys for critical operations
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

**5. Correlation IDs for tracing:**
```java
// Make distributed debugging easier
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

**6. Centralized error handling:**
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

**7. Connection pooling:**
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

**8. Logging and monitoring:**
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

**Q28: How do you handle OAuth2 authentication with these different clients?**

**A:** 

**With RestClient and OAuth2:**
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

// Custom interceptor
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

**With WebClient and OAuth2:**
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

// Usage
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

**OAuth2 configuration in application.yml:**
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

**Q29: How do you optimize the performance of inter-service calls?**

**A:** 

**1. Optimized Connection Pooling:**
```java
@Configuration
public class PerformanceConfig {
    
    @Bean
    public WebClient highPerformanceWebClient() {
        ConnectionProvider provider = ConnectionProvider.builder("optimized")
            .maxConnections(1000)  // Increase based on load
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

**2. Parallel calls:**
```java
// Bad approach - Sequential
public OrderDetails getOrderDetailsSequential(Long orderId) {
    Order order = getOrder(orderId);              // 100ms
    Customer customer = getCustomer(customerId);  // 100ms
    List<Product> products = getProducts(orderId); // 100ms
    // Total: 300ms
    return new OrderDetails(order, customer, products);
}

// Good approach - Parallel
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
    // Total: ~100ms (time of the slowest call)
}
```

**3. Strategic caching:**
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

**4. Response compression:**
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

**5. Bulk operations:**
```java
// Bad approach - Multiple calls
public List<User> getUsersOneByOne(List<Long> userIds) {
    return userIds.stream()
        .map(id -> restClient.get()
            .uri("/api/users/{id}", id)
            .retrieve()
            .body(User.class))
        .collect(Collectors.toList());
    // N HTTP calls
}

// Good approach - Grouped call
public List<User> getUsersBulk(List<Long> userIds) {
    return restClient.post()
        .uri("/api/users/bulk")
        .body(userIds)
        .retrieve()
        .body(new ParameterizedTypeReference<List<User>>() {});
    // 1 HTTP call
}
```

---

This complete Q&A guide covers all the key aspects of synchronous inter-service communication in Spring Boot. It will help you demonstrate your technical expertise during the interview!
