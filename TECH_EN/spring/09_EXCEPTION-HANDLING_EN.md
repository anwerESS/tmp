## [**..**](./00_index.md)

## Exception Handling - Key Points

### **General Concept**
**Exception Handling** in Spring Boot allows managing errors in a centralized way and returning appropriate HTTP responses to the client with custom error messages instead of Spring's default errors.

### **Main Annotations**

**@ResponseStatus:**
- Associates an **HTTP status code** with a specific exception
- When the exception is thrown, Spring automatically returns the specified status code

```java
@ResponseStatus(code = HttpStatus.NOT_FOUND)
public class ResourceNotFoundException extends RuntimeException {
    public ResourceNotFoundException(String message) {
        super(message);
    }
}
```

**@ControllerAdvice:**
- Creates a **global exception handler** for the entire application
- Centralizes exception handling logic
- Applies to all controllers

**@ExceptionHandler:**
- Handles specific exceptions thrown by controller methods
- Allows defining custom logic for each exception type
- Returns a custom response instead of a default error

### **Exception Handling Architecture**

**1. Create custom exceptions:**
```java
@ResponseStatus(code = HttpStatus.NOT_FOUND)
public class ResourceNotFoundException extends RuntimeException {
    public ResourceNotFoundException(String resourceName, String fieldName, Long fieldValue) {
        super(String.format("%s not found with %s : '%s'", resourceName, fieldName, fieldValue));
    }
}

@ResponseStatus(code = HttpStatus.BAD_REQUEST)
public class BlogAPIException extends RuntimeException {
    private HttpStatus status;
    private String message;
    
    public BlogAPIException(HttpStatus status, String message) {
        this.status = status;
        this.message = message;
    }
}
```

**2. Create an ErrorDetails class (DTO for errors):**
```java
public class ErrorDetails {
    private Date timestamp;
    private String message;
    private String details;
    
    public ErrorDetails(Date timestamp, String message, String details) {
        this.timestamp = timestamp;
        this.message = message;
        this.details = details;
    }
    
    // Getters and Setters
}
```

**3. Create a Global Exception Handler with @ControllerAdvice:**
```java
@ControllerAdvice
public class GlobalExceptionHandler {

    // Handle specific exception - ResourceNotFoundException
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleResourceNotFoundException(
            ResourceNotFoundException exception,
            WebRequest webRequest) {
        
        ErrorDetails errorDetails = new ErrorDetails(
                new Date(),
                exception.getMessage(),
                webRequest.getDescription(false));
        
        return new ResponseEntity<>(errorDetails, HttpStatus.NOT_FOUND);
    }

    // Handle specific exception - BlogAPIException
    @ExceptionHandler(BlogAPIException.class)
    public ResponseEntity<ErrorDetails> handleBlogAPIException(
            BlogAPIException exception,
            WebRequest webRequest) {
        
        ErrorDetails errorDetails = new ErrorDetails(
                new Date(),
                exception.getMessage(),
                webRequest.getDescription(false));
        
        return new ResponseEntity<>(errorDetails, HttpStatus.BAD_REQUEST);
    }

    // Global exception handler - catch all other exceptions
    @ExceptionHandler(Exception.class)
    public ResponseEntity<ErrorDetails> handleGlobalException(
            Exception exception,
            WebRequest webRequest) {
        
        ErrorDetails errorDetails = new ErrorDetails(
                new Date(),
                exception.getMessage(),
                webRequest.getDescription(false));
        
        return new ResponseEntity<>(errorDetails, HttpStatus.INTERNAL_SERVER_ERROR);
    }
}
```

### **Usage in Controllers**

```java
@RestController
@RequestMapping("/api/posts")
public class PostController {
    
    @Autowired
    private PostService postService;
    
    @GetMapping("/{id}")
    public ResponseEntity<Post> getPostById(@PathVariable Long id) {
        Post post = postService.findById(id);
        
        if (post == null) {
            // Throws the exception, caught by GlobalExceptionHandler
            throw new ResourceNotFoundException("Post", "id", id);
        }
        
        return ResponseEntity.ok(post);
    }
    
    @PostMapping
    public ResponseEntity<Post> createPost(@Valid @RequestBody Post post) {
        if (postService.existsByTitle(post.getTitle())) {
            // Throws a business exception
            throw new BlogAPIException(HttpStatus.BAD_REQUEST, 
                                      "Post with this title already exists");
        }
        
        Post savedPost = postService.save(post);
        return ResponseEntity.status(HttpStatus.CREATED).body(savedPost);
    }
}
```

### **Types of Exception Handling**

**1. Specific Exception Handling:**
- Handle specific exceptions with dedicated handlers
- Allows custom responses depending on the error type
- Example: ResourceNotFoundException → 404, BlogAPIException → 400

**2. Global Exception Handling:**
- Catch-all handler for all unhandled exceptions
- Generally returns 500 Internal Server Error
- Avoids exposing stack traces to the client

### **Error Response Format**

**JSON response example:**
```json
{
  "timestamp": "2025-10-30T10:15:30.000+00:00",
  "message": "Post not found with id : '123'",
  "details": "uri=/api/posts/123"
}
```

### **Handling Hierarchy**

**Priority order:**
1. **@ExceptionHandler** most specific (ResourceNotFoundException)
2. **@ExceptionHandler** less specific (RuntimeException)
3. **@ExceptionHandler** global (Exception.class)

Spring uses the most specific handler available.

### **WebRequest**

The **WebRequest** object provides information about the HTTP request:
```java
webRequest.getDescription(false)  // URI without query params
webRequest.getDescription(true)   // Full URI with query params
```

### **Best Practices**

1. **Create custom exceptions** for business errors
2. **Use @ControllerAdvice** to centralize handling
3. **Return structured ErrorDetails** with timestamp, message, details
4. **Never expose stack traces** to the client in production
5. **Use the right HTTP status codes** (404, 400, 500, etc.)
6. **Log exceptions** for debugging
7. **Handle validation exceptions** (MethodArgumentNotValidException)
8. **Create a global handler** (Exception.class) as a safety net

### **Key Points**

- **@ResponseStatus** : Associates a status code with an exception
- **@ControllerAdvice** : Global exception handler for the entire application
- **@ExceptionHandler** : Handles specific exceptions
- **ErrorDetails** : DTO to structure error responses
- **WebRequest** : Information about the HTTP request
- **Specific vs Global handlers** : Specific handlers for certain exceptions, global for the rest
- **HTTP Status Codes** : 404 (Not Found), 400 (Bad Request), 500 (Internal Server Error)
- Avoid exposing **stack traces** in production


---

## Questions/Answers - Exception Handling

### **1. What is Exception Handling in Spring Boot and why is it important?**

**Answer:**
**Exception Handling** is the mechanism for managing errors that occur in an application and returning appropriate HTTP responses to the client.

**Importance:**
- **User Experience**: Returns clear and understandable error messages
- **Security**: Avoids exposing stack traces and internal details
- **Centralization**: Handles all exceptions in one place
- **Consistency**: Uniform error response format across the entire application
- **Debugging**: Makes it easier to identify and fix problems
- **Appropriate HTTP Status Codes**: 404, 400, 500, etc.

Without exception handling, Spring returns default errors that are hard to understand for the client.

---

### **2. What are the main annotations for handling exceptions in Spring?**

**Answer:**

**@ResponseStatus:**
- Associates an HTTP status code with a custom exception
- Automatically returned when the exception is thrown

**@ControllerAdvice:**
- Creates a global exception handler
- Centralizes exception handling for all controllers
- Works with @ExceptionHandler

**@ExceptionHandler:**
- Defines a method that handles a specific exception type
- Can be used in a @Controller or @ControllerAdvice
- Returns a custom response

**Example:**
```java
@ResponseStatus(HttpStatus.NOT_FOUND)
public class ResourceNotFoundException extends RuntimeException { }

@ControllerAdvice
public class GlobalExceptionHandler {
    
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleNotFound(...) { }
}
```

---

### **3. What is the difference between @ControllerAdvice and @RestControllerAdvice?**

**Answer:**

| @ControllerAdvice | @RestControllerAdvice |
|--------------------|----------------------|
| For classic MVC controllers | For REST controllers |
| Must use @ResponseBody explicitly | @ResponseBody implicit on all methods |
| Can return views | Always returns JSON/XML |

**Equivalence:**
```java
@RestControllerAdvice = @ControllerAdvice + @ResponseBody
```

**Usage:**
```java
// For REST API
@RestControllerAdvice
public class GlobalExceptionHandler {
    @ExceptionHandler(ResourceNotFoundException.class)
    public ErrorDetails handleNotFound(...) {
        // Returns JSON automatically
    }
}

// For traditional MVC
@ControllerAdvice
public class GlobalExceptionHandler {
    @ExceptionHandler(ResourceNotFoundException.class)
    @ResponseBody  // Required to return JSON
    public ErrorDetails handleNotFound(...) { }
}
```

---

### **4. How to create a custom exception in Spring Boot?**

**Answer:**

**Step 1: Create the exception class**
```java
@ResponseStatus(code = HttpStatus.NOT_FOUND)
public class ResourceNotFoundException extends RuntimeException {
    
    private String resourceName;
    private String fieldName;
    private Long fieldValue;
    
    public ResourceNotFoundException(String resourceName, String fieldName, Long fieldValue) {
        super(String.format("%s not found with %s : '%s'", resourceName, fieldName, fieldValue));
        this.resourceName = resourceName;
        this.fieldName = fieldName;
        this.fieldValue = fieldValue;
    }
    
    // Getters
}
```

**Step 2: Use it in the controller**
```java
@GetMapping("/{id}")
public Post getPostById(@PathVariable Long id) {
    return postRepository.findById(id)
            .orElseThrow(() -> new ResourceNotFoundException("Post", "id", id));
}
```

**Step 3: Handle with @ExceptionHandler**
```java
@ControllerAdvice
public class GlobalExceptionHandler {
    
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleResourceNotFoundException(
            ResourceNotFoundException ex, WebRequest request) {
        
        ErrorDetails error = new ErrorDetails(
                new Date(),
                ex.getMessage(),
                request.getDescription(false));
        
        return new ResponseEntity<>(error, HttpStatus.NOT_FOUND);
    }
}
```

---

### **5. What is the ErrorDetails class and why use it?**

**Answer:**
**ErrorDetails** is a **DTO (Data Transfer Object)** that structures error responses in a consistent way for the client.

**Definition:**
```java
public class ErrorDetails {
    private Date timestamp;
    private String message;
    private String details;
    
    public ErrorDetails(Date timestamp, String message, String details) {
        this.timestamp = timestamp;
        this.message = message;
        this.details = details;
    }
    
    // Getters and Setters
}
```

**Advantages:**
- **Uniform format** for all errors
- **Useful information**: timestamp, clear message, details (URI)
- **Makes client-side debugging easier**
- **Professional**: consistent JSON structure

**JSON response example:**
```json
{
  "timestamp": "2025-10-30T10:15:30.000+00:00",
  "message": "Post not found with id : '123'",
  "details": "uri=/api/posts/123"
}
```

---

### **6. How to implement a complete Global Exception Handler?**

**Answer:**

```java
@RestControllerAdvice
public class GlobalExceptionHandler {

    // 1. Handle specific exception - ResourceNotFoundException (404)
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleResourceNotFoundException(
            ResourceNotFoundException exception,
            WebRequest webRequest) {
        
        ErrorDetails errorDetails = new ErrorDetails(
                new Date(),
                exception.getMessage(),
                webRequest.getDescription(false));
        
        return new ResponseEntity<>(errorDetails, HttpStatus.NOT_FOUND);
    }

    // 2. Handle specific exception - BlogAPIException (400)
    @ExceptionHandler(BlogAPIException.class)
    public ResponseEntity<ErrorDetails> handleBlogAPIException(
            BlogAPIException exception,
            WebRequest webRequest) {
        
        ErrorDetails errorDetails = new ErrorDetails(
                new Date(),
                exception.getMessage(),
                webRequest.getDescription(false));
        
        return new ResponseEntity<>(errorDetails, HttpStatus.BAD_REQUEST);
    }

    // 3. Handle validation exceptions (400)
    @ExceptionHandler(MethodArgumentNotValidException.class)
    public ResponseEntity<Map<String, String>> handleValidationErrors(
            MethodArgumentNotValidException ex) {
        
        Map<String, String> errors = new HashMap<>();
        ex.getBindingResult().getFieldErrors().forEach(error -> {
            errors.put(error.getField(), error.getDefaultMessage());
        });
        
        return new ResponseEntity<>(errors, HttpStatus.BAD_REQUEST);
    }

    // 4. Handle method argument type mismatch (400)
    @ExceptionHandler(MethodArgumentTypeMismatchException.class)
    public ResponseEntity<ErrorDetails> handleMethodArgumentTypeMismatch(
            MethodArgumentTypeMismatchException ex,
            WebRequest request) {
        
        String message = String.format("Parameter '%s' should be of type %s", 
                                      ex.getName(), 
                                      ex.getRequiredType().getSimpleName());
        
        ErrorDetails errorDetails = new ErrorDetails(
                new Date(),
                message,
                request.getDescription(false));
        
        return new ResponseEntity<>(errorDetails, HttpStatus.BAD_REQUEST);
    }

    // 5. Global exception handler - catch all (500)
    @ExceptionHandler(Exception.class)
    public ResponseEntity<ErrorDetails> handleGlobalException(
            Exception exception,
            WebRequest webRequest) {
        
        ErrorDetails errorDetails = new ErrorDetails(
                new Date(),
                exception.getMessage(),
                webRequest.getDescription(false));
        
        return new ResponseEntity<>(errorDetails, HttpStatus.INTERNAL_SERVER_ERROR);
    }
}
```

---

### **7. What is the difference between Exception and RuntimeException?**

**Answer:**

| Exception (Checked) | RuntimeException (Unchecked) |
|---------------------|------------------------------|
| Must be caught or declared (throws) | Not required to catch |
| Compiler forces handling | No compile-time check |
| Examples: IOException, SQLException | Examples: NullPointerException, IllegalArgumentException |
| For predictable errors | For programming errors |

**For Spring REST:**
```java
// Use RuntimeException for custom exceptions
public class ResourceNotFoundException extends RuntimeException {
    // No need for "throws" in methods
}

// Avoid Exception (checked)
public class ResourceNotFoundException extends Exception {
    // Forces "throws ResourceNotFoundException" everywhere
}
```

**Best practice:** Extend **RuntimeException** for custom REST exceptions.

---

### **8. How to handle 404 errors (Resource Not Found)?**

**Answer:**

**1. Create the exception:**
```java
@ResponseStatus(code = HttpStatus.NOT_FOUND)
public class ResourceNotFoundException extends RuntimeException {
    
    public ResourceNotFoundException(String resourceName, String fieldName, Object fieldValue) {
        super(String.format("%s not found with %s : '%s'", 
                           resourceName, fieldName, fieldValue));
    }
}
```

**2. Throw the exception in the service/controller:**
```java
@Service
public class PostService {
    
    public Post findById(Long id) {
        return postRepository.findById(id)
                .orElseThrow(() -> new ResourceNotFoundException("Post", "id", id));
    }
}
```

**3. Handle with @ExceptionHandler:**
```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleResourceNotFoundException(
            ResourceNotFoundException ex, WebRequest request) {
        
        ErrorDetails error = new ErrorDetails(
                new Date(),
                ex.getMessage(),
                request.getDescription(false));
        
        return new ResponseEntity<>(error, HttpStatus.NOT_FOUND);
    }
}
```

**Client response:**
```json
HTTP/1.1 404 Not Found
{
  "timestamp": "2025-10-30T10:15:30.000+00:00",
  "message": "Post not found with id : '123'",
  "details": "uri=/api/posts/123"
}
```

---

### **9. How to handle validation errors (400 Bad Request)?**

**Answer:**

When @Valid fails, Spring throws **MethodArgumentNotValidException**.

**Handler:**
```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    
    @ExceptionHandler(MethodArgumentNotValidException.class)
    public ResponseEntity<Map<String, String>> handleValidationErrors(
            MethodArgumentNotValidException ex) {
        
        Map<String, String> errors = new HashMap<>();
        
        ex.getBindingResult().getFieldErrors().forEach(error -> {
            errors.put(error.getField(), error.getDefaultMessage());
        });
        
        return new ResponseEntity<>(errors, HttpStatus.BAD_REQUEST);
    }
}
```

**Client response:**
```json
HTTP/1.1 400 Bad Request
{
  "name": "Name is required",
  "email": "Invalid email format",
  "age": "Must be at least 18 years old"
}
```

**Alternative with ErrorDetails:**
```java
@ExceptionHandler(MethodArgumentNotValidException.class)
public ResponseEntity<ErrorDetails> handleValidationErrors(
        MethodArgumentNotValidException ex, WebRequest request) {
    
    StringBuilder message = new StringBuilder("Validation failed: ");
    ex.getBindingResult().getFieldErrors().forEach(error -> {
        message.append(error.getField())
               .append(" - ")
               .append(error.getDefaultMessage())
               .append("; ");
    });
    
    ErrorDetails error = new ErrorDetails(
            new Date(),
            message.toString(),
            request.getDescription(false));
    
    return new ResponseEntity<>(error, HttpStatus.BAD_REQUEST);
}
```

---

### **10. What is WebRequest and how to use it?**

**Answer:**
**WebRequest** is a Spring interface that provides information about the current HTTP request inside exception handlers.

**Useful methods:**
```java
@ExceptionHandler(ResourceNotFoundException.class)
public ResponseEntity<ErrorDetails> handle(
        ResourceNotFoundException ex, 
        WebRequest request) {
    
    // URI without query parameters
    String uri = request.getDescription(false); // "uri=/api/posts/123"
    
    // URI with query parameters
    String fullUri = request.getDescription(true); // "uri=/api/posts/123?sort=asc"
    
    // Headers
    String userAgent = request.getHeader("User-Agent");
    
    // Parameters
    String sortParam = request.getParameter("sort");
    
    ErrorDetails error = new ErrorDetails(
            new Date(),
            ex.getMessage(),
            uri);
    
    return new ResponseEntity<>(error, HttpStatus.NOT_FOUND);
}
```

**Typical usage:** Include the URI in ErrorDetails for debugging.

---

### **11. How to handle multiple exception types in a single handler?**

**Answer:**

**Option 1: List of exceptions in @ExceptionHandler**
```java
@ExceptionHandler({ResourceNotFoundException.class, EntityNotFoundException.class})
public ResponseEntity<ErrorDetails> handleNotFoundExceptions(
        RuntimeException ex, WebRequest request) {
    
    ErrorDetails error = new ErrorDetails(
            new Date(),
            ex.getMessage(),
            request.getDescription(false));
    
    return new ResponseEntity<>(error, HttpStatus.NOT_FOUND);
}
```

**Option 2: Parent exception**
```java
// If all custom exceptions extend BusinessException
@ExceptionHandler(BusinessException.class)
public ResponseEntity<ErrorDetails> handleBusinessExceptions(
        BusinessException ex, WebRequest request) {
    
    ErrorDetails error = new ErrorDetails(
            new Date(),
            ex.getMessage(),
            request.getDescription(false));
    
    return new ResponseEntity<>(error, ex.getHttpStatus());
}
```

---

### **12. What is the exception handler resolution hierarchy?**

**Answer:**
Spring uses the **most specific** handler available:

**Priority order (from most specific to most general):**
1. **Exact exception** (ResourceNotFoundException.class)
2. **Parent exception** (RuntimeException.class)
3. **Very general exception** (Exception.class)

**Example:**
```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    
    // 1. Most specific - will be used for ResourceNotFoundException
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleResourceNotFound(...) {
        return new ResponseEntity<>(..., HttpStatus.NOT_FOUND);
    }
    
    // 2. Less specific - for all RuntimeExceptions except ResourceNotFoundException
    @ExceptionHandler(RuntimeException.class)
    public ResponseEntity<ErrorDetails> handleRuntimeException(...) {
        return new ResponseEntity<>(..., HttpStatus.BAD_REQUEST);
    }
    
    // 3. Catch-all - for all other exceptions
    @ExceptionHandler(Exception.class)
    public ResponseEntity<ErrorDetails> handleGlobalException(...) {
        return new ResponseEntity<>(..., HttpStatus.INTERNAL_SERVER_ERROR);
    }
}
```

**Rule:** Always define a global handler (Exception.class) as a safety net.

---

### **13. How to create an exception with a dynamic status code?**

**Answer:**

```java
public class APIException extends RuntimeException {
    
    private HttpStatus status;
    private String message;
    
    public APIException(HttpStatus status, String message) {
        super(message);
        this.status = status;
        this.message = message;
    }
    
    public HttpStatus getStatus() {
        return status;
    }
    
    @Override
    public String getMessage() {
        return message;
    }
}

// Handler
@ExceptionHandler(APIException.class)
public ResponseEntity<ErrorDetails> handleAPIException(
        APIException ex, WebRequest request) {
    
    ErrorDetails error = new ErrorDetails(
            new Date(),
            ex.getMessage(),
            request.getDescription(false));
    
    // Uses the status from the exception
    return new ResponseEntity<>(error, ex.getStatus());
}

// Usage
throw new APIException(HttpStatus.FORBIDDEN, "Access denied");
throw new APIException(HttpStatus.CONFLICT, "Resource already exists");
```

---

### **14. How to avoid exposing stack traces in production?**

**Answer:**

**Problem:** By default, Spring can expose detailed stack traces that reveal the application's internal structure.

**Solutions:**

**1. Never use exception.printStackTrace() in the handler**
```java
// ❌ Bad
@ExceptionHandler(Exception.class)
public ResponseEntity<ErrorDetails> handle(Exception ex) {
    ex.printStackTrace();  // Exposed in logs
    ...
}

// ✅ Good
@ExceptionHandler(Exception.class)
public ResponseEntity<ErrorDetails> handle(Exception ex, WebRequest request) {
    // Log with logger
    log.error("Error occurred: {}", ex.getMessage());
    
    ErrorDetails error = new ErrorDetails(
            new Date(),
            "An error occurred",  // Generic message
            request.getDescription(false));
    
    return new ResponseEntity<>(error, HttpStatus.INTERNAL_SERVER_ERROR);
}
```

**2. application.properties configuration**
```properties
# Disables stack traces in responses
server.error.include-stacktrace=never
server.error.include-message=never
server.error.include-binding-errors=never

# Or production mode
server.error.include-stacktrace=on-param  # Only if ?trace=true
```

**3. Generic messages for unexpected exceptions**
```java
@ExceptionHandler(Exception.class)
public ResponseEntity<ErrorDetails> handleGlobalException(
        Exception ex, WebRequest request) {
    
    // Log the full error
    log.error("Unexpected error", ex);
    
    // Return generic message to client
    ErrorDetails error = new ErrorDetails(
            new Date(),
            "An unexpected error occurred. Please contact support.",
            request.getDescription(false));
    
    return new ResponseEntity<>(error, HttpStatus.INTERNAL_SERVER_ERROR);
}
```

---

### **15. How to handle exceptions in async methods (@Async)?**

**Answer:**

Exceptions in @Async methods are not caught by @ControllerAdvice because they run in a different thread.

**Solution: AsyncUncaughtExceptionHandler**
```java
@Configuration
@EnableAsync
public class AsyncConfig implements AsyncConfigurer {
    
    @Override
    public AsyncUncaughtExceptionHandler getAsyncUncaughtExceptionHandler() {
        return new CustomAsyncExceptionHandler();
    }
}

public class CustomAsyncExceptionHandler implements AsyncUncaughtExceptionHandler {
    
    private static final Logger log = LoggerFactory.getLogger(CustomAsyncExceptionHandler.class);
    
    @Override
    public void handleUncaughtException(Throwable ex, Method method, Object... params) {
        log.error("Async exception in method: {}", method.getName(), ex);
        log.error("Parameters: {}", Arrays.toString(params));
        
        // Send notification, save to DB, etc.
    }
}
```

---

### **16. How to test exception handling in unit tests?**

**Answer:**

```java
@SpringBootTest
@AutoConfigureMockMvc
class GlobalExceptionHandlerTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    @Test
    void testResourceNotFoundException() throws Exception {
        mockMvc.perform(get("/api/posts/999"))
                .andExpect(status().isNotFound())
                .andExpect(jsonPath("$.message").value("Post not found with id : '999'"))
                .andExpect(jsonPath("$.timestamp").exists())
                .andExpect(jsonPath("$.details").value("uri=/api/posts/999"));
    }
    
    @Test
    void testValidationException() throws Exception {
        String invalidUser = "{\"name\":\"\",\"email\":\"invalid\"}";
        
        mockMvc.perform(post("/api/users")
                .contentType(MediaType.APPLICATION_JSON)
                .content(invalidUser))
                .andExpect(status().isBadRequest())
                .andExpect(jsonPath("$.name").exists())
                .andExpect(jsonPath("$.email").exists());
    }
    
    @Test
    void testGlobalException() throws Exception {
        // Simulate an unexpected exception
        mockMvc.perform(get("/api/error-endpoint"))
                .andExpect(status().isInternalServerError())
                .andExpect(jsonPath("$.message").exists());
    }
}
```

---

### **17. What is the difference between @ExceptionHandler at controller level vs @ControllerAdvice?**

**Answer:**

**@ExceptionHandler in @Controller:**
- Handles exceptions **only for that controller**
- Local scope

```java
@RestController
@RequestMapping("/api/posts")
public class PostController {
    
    // Only handles exceptions from PostController
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleNotFound(ResourceNotFoundException ex) {
        ...
    }
}
```

**@ExceptionHandler in @ControllerAdvice:**
- Handles exceptions **for all controllers**
- Global scope

```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    
    // Handles exceptions from all controllers
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleNotFound(ResourceNotFoundException ex) {
        ...
    }
}
```

**Best practice:** Use **@ControllerAdvice** to centralize exception handling.

---

### **18. How to handle authentication and authorization exceptions?**

**Answer:**

```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    
    // 401 Unauthorized - Not authenticated
    @ExceptionHandler(AuthenticationException.class)
    public ResponseEntity<ErrorDetails> handleAuthenticationException(
            AuthenticationException ex, WebRequest request) {
        
        ErrorDetails error = new ErrorDetails(
                new Date(),
                "Authentication failed: " + ex.getMessage(),
                request.getDescription(false));
        
        return new ResponseEntity<>(error, HttpStatus.UNAUTHORIZED);
    }
    
    // 403 Forbidden - Authenticated but not authorized
    @ExceptionHandler(AccessDeniedException.class)
    public ResponseEntity<ErrorDetails> handleAccessDeniedException(
            AccessDeniedException ex, WebRequest request) {
        
        ErrorDetails error = new ErrorDetails(
                new Date(),
                "Access denied: " + ex.getMessage(),
                request.getDescription(false));
        
        return new ResponseEntity<>(error, HttpStatus.FORBIDDEN);
    }
    
    // Custom exception for invalid JWT
    @ExceptionHandler(InvalidJwtException.class)
    public ResponseEntity<ErrorDetails> handleInvalidJwtException(
            InvalidJwtException ex, WebRequest request) {
        
        ErrorDetails error = new ErrorDetails(
                new Date(),
                "Invalid or expired JWT token",
                request.getDescription(false));
        
        return new ResponseEntity<>(error, HttpStatus.UNAUTHORIZED);
    }
}
```

---

### **19. How to log exceptions correctly?**

**Answer:**

```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    
    private static final Logger log = LoggerFactory.getLogger(GlobalExceptionHandler.class);
    
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleResourceNotFoundException(
            ResourceNotFoundException ex, WebRequest request) {
        
        // Log WARNING for client errors (4xx)
        log.warn("Resource not found: {} - URI: {}", 
                ex.getMessage(), 
                request.getDescription(false));
        
        ErrorDetails error = new ErrorDetails(
                new Date(),
                ex.getMessage(),
                request.getDescription(false));
        
        return new ResponseEntity<>(error, HttpStatus.NOT_FOUND);
    }
    
    @ExceptionHandler(Exception.class)
    public ResponseEntity<ErrorDetails> handleGlobalException(
            Exception ex, WebRequest request) {
        
        // Log ERROR for server errors (5xx)
        log.error("Unexpected error occurred - URI: {}", 
                 request.getDescription(false), 
                 ex);  // Includes the stack trace
        
        ErrorDetails error = new ErrorDetails(
                new Date(),
                "An unexpected error occurred",
                request.getDescription(false));
        
        return new ResponseEntity<>(error, HttpStatus.INTERNAL_SERVER_ERROR);
    }
}
```

**Best practices:**
- **4xx errors (client)** → log.warn()
- **5xx errors (server)** → log.error() with stack trace
- Include the URI and relevant parameters
- Never log sensitive data (passwords, tokens)

---

### **20. What are the best practices for Exception Handling?**

**Answer:**

**1. Use @ControllerAdvice to centralize**
```java
@RestControllerAdvice
public class GlobalExceptionHandler { ... }
```

**2. Create specific custom exceptions**
```java
ResourceNotFoundException, DuplicateResourceException, etc.
```

**3. Use ErrorDetails to format responses**
```java
{ "timestamp", "message", "details" }
```

**4. Never expose stack traces to the client**
```java
server.error.include-stacktrace=never
```

**5. Use the right HTTP status codes**
- 400 Bad Request (validation)
- 401 Unauthorized (authentication)
- 403 Forbidden (authorization)
- 404 Not Found (resource)
- 409 Conflict (duplicate)
- 500 Internal Server Error (unexpected)

**6. Always have a global exception handler**
```java
@ExceptionHandler(Exception.class)
```

**7. Log exceptions correctly**
- WARN for 4xx
- ERROR for 5xx with stack trace

**8. Clear and actionable error messages**
```java
"Post not found with id : '123'" ✅
"Error occurred" ❌
```

**9. Include the URI in ErrorDetails for debugging**
```java
webRequest.getDescription(false)
```

**10. Test exception handling**
```java
@Test void testResourceNotFoundException() { ... }
```

---

**Final tip:** Be ready to implement a complete exception handler with multiple exception types during the interview. It's a very common practical question! 💪
