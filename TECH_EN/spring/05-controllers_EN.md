
## [**..**](./00_index.md)

# REST Controllers & Endpoints Spring Boot

## 🎮 Controllers - MVC Architecture

### Controller Concept

A **Controller** in Spring MVC is the component that handles incoming HTTP requests, processes presentation logic, and returns a response.

**Typical architecture :**
```
Client (Browser/Postman)
    ↓ HTTP Request
Controller Layer (@RestController)
    ↓ Method calls
Service Layer (@Service)
    ↓ Business logic
Repository Layer (@Repository)
    ↓ Data access
Database
```

### @Controller vs @RestController

#### @Controller (Traditional Web Applications)
```java
@Controller
public class UserWebController {
    
    @GetMapping("/users")
    public String listUsers(Model model) {
        model.addAttribute("users", userService.findAll());
        return "users"; // Returns a view name (users.jsp, users.html)
    }
}
```
**Usage :** Web applications with views (JSP, Thymeleaf)
**Returns :** Name of the view to render

#### @RestController (REST APIs) ⭐
```java
@RestController
public class UserController {
    
    @GetMapping("/api/users")
    public List<User> listUsers() {
        return userService.findAll(); // Returns JSON directly
    }
}
```
**Usage :** REST APIs (JSON/XML response)
**Equivalent to :** `@Controller + @ResponseBody` on each method

**@RestController = @Controller + @ResponseBody**

### Complete Structure of a REST Controller

```java
@RestController
@RequestMapping("/api/users")
@CrossOrigin(origins = "http://localhost:3000") // CORS
@Validated
public class UserController {
    
    @Autowired
    private UserService userService;
    
    // GET - List all users
    @GetMapping
    public ResponseEntity<List<User>> getAllUsers() {
        List<User> users = userService.findAll();
        return ResponseEntity.ok(users);
    }
    
    // GET - One user by ID
    @GetMapping("/{id}")
    public ResponseEntity<User> getUserById(@PathVariable Long id) {
        User user = userService.findById(id);
        return ResponseEntity.ok(user);
    }
    
    // POST - Create a user
    @PostMapping
    public ResponseEntity<User> createUser(@RequestBody @Valid User user) {
        User created = userService.save(user);
        return ResponseEntity
            .status(HttpStatus.CREATED)
            .body(created);
    }
    
    // PUT - Update a user
    @PutMapping("/{id}")
    public ResponseEntity<User> updateUser(
            @PathVariable Long id,
            @RequestBody @Valid User user) {
        User updated = userService.update(id, user);
        return ResponseEntity.ok(updated);
    }
    
    // DELETE - Delete a user
    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
        userService.delete(id);
        return ResponseEntity.noContent().build();
    }
    
    // PATCH - Partial update
    @PatchMapping("/{id}")
    public ResponseEntity<User> partialUpdate(
            @PathVariable Long id,
            @RequestBody Map<String, Object> updates) {
        User updated = userService.partialUpdate(id, updates);
        return ResponseEntity.ok(updated);
    }
}
```

## 🛣️ Endpoints Mapping - Routing Annotations

### @RequestMapping (Generic)

**Basic mapping :**
```java
@RestController
@RequestMapping("/api") // Prefix for all endpoints
public class ProductController {
    
    @RequestMapping(value = "/products", method = RequestMethod.GET)
    public List<Product> getProducts() {
        return productService.findAll();
    }
}
```

**With multiple HTTP methods :**
```java
@RequestMapping(value = "/products/{id}", method = {RequestMethod.GET, RequestMethod.HEAD})
public Product getProduct(@PathVariable Long id) {
    return productService.findById(id);
}
```

**With conditions :**
```java
@RequestMapping(
    value = "/products",
    method = RequestMethod.GET,
    params = "active=true",           // Requires ?active=true
    headers = "X-API-Version=1",      // Requires header
    consumes = "application/json",    // Accepts JSON
    produces = "application/json"     // Produces JSON
)
public List<Product> getActiveProducts() {
    return productService.findActive();
}
```

### Specialized Annotations (Recommended)

#### @GetMapping
```java
@GetMapping("/users")           // GET /api/users
@GetMapping("/users/{id}")      // GET /api/users/123
@GetMapping(value = "/users", params = "active") // GET /api/users?active=true
```

#### @PostMapping
```java
@PostMapping("/users")
public ResponseEntity<User> createUser(@RequestBody User user) {
    User created = userService.save(user);
    URI location = ServletUriComponentsBuilder
        .fromCurrentRequest()
        .path("/{id}")
        .buildAndExpand(created.getId())
        .toUri();
    return ResponseEntity.created(location).body(created);
}
```

#### @PutMapping (Full replacement)
```java
@PutMapping("/users/{id}")
public ResponseEntity<User> updateUser(
        @PathVariable Long id,
        @RequestBody User user) {
    User updated = userService.update(id, user);
    return ResponseEntity.ok(updated);
}
```

#### @PatchMapping (Partial update)
```java
@PatchMapping("/users/{id}")
public ResponseEntity<User> partialUpdateUser(
        @PathVariable Long id,
        @RequestBody Map<String, Object> updates) {
    // Updates contains only the modified fields
    User updated = userService.partialUpdate(id, updates);
    return ResponseEntity.ok(updated);
}
```

#### @DeleteMapping
```java
@DeleteMapping("/users/{id}")
public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    userService.delete(id);
    return ResponseEntity.noContent().build(); // 204 No Content
}
```

### Advanced URL Patterns

```java
// Multiple variables
@GetMapping("/users/{userId}/orders/{orderId}")
public Order getUserOrder(
        @PathVariable Long userId,
        @PathVariable Long orderId) {
    return orderService.findByUserAndId(userId, orderId);
}

// Optional variables with regex
@GetMapping("/files/{filename:.+}") // .+ captures the extension
public ResponseEntity<Resource> downloadFile(@PathVariable String filename) {
    Resource file = storageService.loadAsResource(filename);
    return ResponseEntity.ok()
        .header(HttpHeaders.CONTENT_DISPOSITION, 
                "attachment; filename=\"" + file.getFilename() + "\"")
        .body(file);
}

// Wildcards
@GetMapping("/api/**") // Matches all sub-paths
public String handleAll() {
    return "Catch-all handler";
}
```

## 📥 Parameter Annotations

### @PathVariable (URL Variables)

```java
// Basic
@GetMapping("/users/{id}")
public User getUser(@PathVariable Long id) {
    return userService.findById(id);
}

// Different variable name
@GetMapping("/users/{userId}")
public User getUser(@PathVariable("userId") Long id) {
    return userService.findById(id);
}

// Multiple variables
@GetMapping("/users/{userId}/posts/{postId}")
public Post getUserPost(
        @PathVariable Long userId,
        @PathVariable Long postId) {
    return postService.findByUserAndId(userId, postId);
}

// Optional (Spring 4.3+)
@GetMapping({"/users", "/users/{id}"})
public ResponseEntity<?> getUsers(@PathVariable(required = false) Long id) {
    if (id == null) {
        return ResponseEntity.ok(userService.findAll());
    }
    return ResponseEntity.ok(userService.findById(id));
}

// Map of all variables
@GetMapping("/data/{var1}/{var2}")
public String getData(@PathVariable Map<String, String> pathVars) {
    return "var1: " + pathVars.get("var1") + 
           ", var2: " + pathVars.get("var2");
}
```

### @RequestParam (Query Parameters)

```java
// Basic: GET /api/users?name=John
@GetMapping("/users")
public List<User> findUsers(@RequestParam String name) {
    return userService.findByName(name);
}

// Optional with default value
@GetMapping("/users")
public List<User> findUsers(
        @RequestParam(required = false, defaultValue = "0") int page,
        @RequestParam(defaultValue = "10") int size) {
    return userService.findAll(page, size);
}

// Different name
@GetMapping("/users")
public List<User> findUsers(
        @RequestParam(name = "q") String searchQuery) {
    return userService.search(searchQuery);
}

// Multiple values: GET /api/users?id=1&id=2&id=3
@GetMapping("/users")
public List<User> findUsers(@RequestParam List<Long> id) {
    return userService.findByIds(id);
}

// Map of all parameters
@GetMapping("/search")
public List<User> search(@RequestParam Map<String, String> allParams) {
    // allParams contains all query params
    return userService.search(allParams);
}

// Filtering and pagination
@GetMapping("/products")
public List<Product> getProducts(
        @RequestParam(required = false) String category,
        @RequestParam(required = false) Double minPrice,
        @RequestParam(required = false) Double maxPrice,
        @RequestParam(defaultValue = "0") int page,
        @RequestParam(defaultValue = "20") int size,
        @RequestParam(defaultValue = "name") String sortBy) {
    return productService.findWithFilters(
        category, minPrice, maxPrice, page, size, sortBy
    );
}
```

### @RequestBody (Request Body)

```java
// JSON → Object
@PostMapping("/users")
public ResponseEntity<User> createUser(@RequestBody User user) {
    User created = userService.save(user);
    return ResponseEntity.status(HttpStatus.CREATED).body(created);
}

// With validation
@PostMapping("/users")
public ResponseEntity<User> createUser(@RequestBody @Valid User user) {
    // @Valid triggers Bean Validation (JSR-303)
    User created = userService.save(user);
    return ResponseEntity.status(HttpStatus.CREATED).body(created);
}

// DTO (Data Transfer Object)
@PostMapping("/users")
public ResponseEntity<UserResponse> createUser(
        @RequestBody @Valid CreateUserRequest request) {
    User user = userService.create(request);
    UserResponse response = new UserResponse(user);
    return ResponseEntity.status(HttpStatus.CREATED).body(response);
}

// Map for flexible structure
@PostMapping("/data")
public ResponseEntity<String> processData(
        @RequestBody Map<String, Object> data) {
    // data contains the full JSON structure
    return ResponseEntity.ok("Processed");
}
```

**Example request with @RequestBody :**
```bash
POST /api/users
Content-Type: application/json

{
  "name": "John Doe",
  "email": "john@example.com",
  "age": 30
}
```

### @RequestHeader (HTTP Headers)

```java
// Specific header
@GetMapping("/users")
public List<User> getUsers(
        @RequestHeader("X-API-Key") String apiKey) {
    if (!isValidApiKey(apiKey)) {
        throw new UnauthorizedException();
    }
    return userService.findAll();
}

// Optional header
@GetMapping("/users")
public List<User> getUsers(
        @RequestHeader(value = "User-Agent", required = false) String userAgent) {
    log.info("Request from: " + userAgent);
    return userService.findAll();
}

// All headers
@GetMapping("/debug")
public Map<String, String> debugHeaders(@RequestHeader Map<String, String> headers) {
    return headers;
}
```

### Other Parameter Annotations

#### @CookieValue
```java
@GetMapping("/profile")
public User getProfile(@CookieValue("sessionId") String sessionId) {
    return userService.findBySession(sessionId);
}
```

#### @SessionAttribute
```java
@GetMapping("/cart")
public Cart getCart(@SessionAttribute("cart") Cart cart) {
    return cart;
}
```

#### @RequestAttribute
```java
@GetMapping("/data")
public String getData(@RequestAttribute("requestId") String requestId) {
    return "Request ID: " + requestId;
}
```

#### @MatrixVariable (Matrix Parameters)
```java
// URL: /products/category=electronics;brand=apple
@GetMapping("/products/{filters}")
public List<Product> getProducts(
        @MatrixVariable String category,
        @MatrixVariable String brand) {
    return productService.findByCategoryAndBrand(category, brand);
}
```

## 📨 ResponseEntity<> - Full Response Control

### ResponseEntity Concept

`ResponseEntity<T>` is a wrapper that allows full control over the HTTP response : status code, headers, and body.

### Building ResponseEntity

#### Factory Methods (Recommended)

```java
// 200 OK
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    User user = userService.findById(id);
    return ResponseEntity.ok(user);
    // Equivalent to: new ResponseEntity<>(user, HttpStatus.OK)
}

// 201 CREATED with Location header
@PostMapping("/users")
public ResponseEntity<User> createUser(@RequestBody User user) {
    User created = userService.save(user);
    URI location = URI.create("/api/users/" + created.getId());
    return ResponseEntity.created(location).body(created);
}

// 204 NO CONTENT
@DeleteMapping("/users/{id}")
public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    userService.delete(id);
    return ResponseEntity.noContent().build();
}

// 404 NOT FOUND
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    return userService.findById(id)
        .map(ResponseEntity::ok)
        .orElse(ResponseEntity.notFound().build());
}

// 202 ACCEPTED
@PostMapping("/jobs")
public ResponseEntity<Job> submitJob(@RequestBody Job job) {
    Job submitted = jobService.submit(job);
    return ResponseEntity.accepted().body(submitted);
}

// 400 BAD REQUEST
@PostMapping("/users")
public ResponseEntity<String> createUser(@RequestBody User user) {
    if (user.getEmail() == null) {
        return ResponseEntity
            .badRequest()
            .body("Email is required");
    }
    return ResponseEntity.ok("User created");
}
```

#### Builder Pattern (Full control)

```java
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    User user = userService.findById(id);
    
    return ResponseEntity
        .status(HttpStatus.OK)                    // Status code
        .header("X-Custom-Header", "value")       // Custom headers
        .header(HttpHeaders.CACHE_CONTROL, "max-age=3600")
        .eTag("\"user-" + user.getId() + "\"")    // ETag for cache
        .lastModified(user.getUpdatedAt())        // Last-Modified
        .body(user);                              // Response body
}

// File download
@GetMapping("/files/{filename}")
public ResponseEntity<Resource> downloadFile(@PathVariable String filename) {
    Resource file = storageService.load(filename);
    
    return ResponseEntity.ok()
        .contentType(MediaType.APPLICATION_OCTET_STREAM)
        .header(HttpHeaders.CONTENT_DISPOSITION, 
                "attachment; filename=\"" + file.getFilename() + "\"")
        .body(file);
}
```

#### With Optional (Elegant pattern)

```java
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    return userService.findById(id)
        .map(user -> ResponseEntity.ok(user))
        .orElse(ResponseEntity.notFound().build());
}

// With transformation
@GetMapping("/users/{id}")
public ResponseEntity<UserDTO> getUser(@PathVariable Long id) {
    return userService.findById(id)
        .map(user -> new UserDTO(user))
        .map(ResponseEntity::ok)
        .orElse(ResponseEntity.notFound().build());
}
```

### ResponseEntity without Body

```java
// 204 No Content
@DeleteMapping("/users/{id}")
public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    userService.delete(id);
    return ResponseEntity.noContent().build();
}

// 304 Not Modified (cache)
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(
        @PathVariable Long id,
        @RequestHeader(value = "If-None-Match", required = false) String ifNoneMatch) {
    User user = userService.findById(id);
    String etag = "\"user-" + user.getId() + "\"";
    
    if (etag.equals(ifNoneMatch)) {
        return ResponseEntity.status(HttpStatus.NOT_MODIFIED).build();
    }
    
    return ResponseEntity.ok()
        .eTag(etag)
        .body(user);
}
```

## 🚦 HTTP Status Codes - Response Codes

### 2xx Success

```java
// 200 OK - Successful request
@GetMapping("/users")
public ResponseEntity<List<User>> getUsers() {
    return ResponseEntity.ok(userService.findAll());
}

// 201 CREATED - Resource created
@PostMapping("/users")
public ResponseEntity<User> createUser(@RequestBody User user) {
    User created = userService.save(user);
    return ResponseEntity.status(HttpStatus.CREATED).body(created);
}

// 202 ACCEPTED - Request accepted, async processing
@PostMapping("/jobs")
public ResponseEntity<String> submitJob(@RequestBody Job job) {
    jobService.submitAsync(job);
    return ResponseEntity.accepted().body("Job submitted");
}

// 204 NO CONTENT - Success with no content to return
@DeleteMapping("/users/{id}")
public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    userService.delete(id);
    return ResponseEntity.noContent().build();
}
```

### 3xx Redirection

```java
// 301 MOVED PERMANENTLY - Resource moved permanently
@GetMapping("/old-url")
public ResponseEntity<Void> oldEndpoint() {
    return ResponseEntity
        .status(HttpStatus.MOVED_PERMANENTLY)
        .location(URI.create("/new-url"))
        .build();
}

// 302 FOUND - Temporary redirect
@GetMapping("/temporary")
public ResponseEntity<Void> temporaryRedirect() {
    return ResponseEntity
        .status(HttpStatus.FOUND)
        .location(URI.create("/actual-location"))
        .build();
}

// 304 NOT MODIFIED - Cache still valid
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(
        @PathVariable Long id,
        @RequestHeader(value = "If-None-Match", required = false) String etag) {
    User user = userService.findById(id);
    String currentEtag = "\"" + user.hashCode() + "\"";
    
    if (currentEtag.equals(etag)) {
        return ResponseEntity.status(HttpStatus.NOT_MODIFIED).build();
    }
    
    return ResponseEntity.ok().eTag(currentEtag).body(user);
}
```

### 4xx Client Errors

```java
// 400 BAD REQUEST - Invalid request
@PostMapping("/users")
public ResponseEntity<String> createUser(@RequestBody User user) {
    if (user.getEmail() == null) {
        return ResponseEntity
            .badRequest()
            .body("Email is required");
    }
    return ResponseEntity.ok("Created");
}

// 401 UNAUTHORIZED - Not authenticated
@GetMapping("/profile")
public ResponseEntity<User> getProfile() {
    if (!securityService.isAuthenticated()) {
        return ResponseEntity.status(HttpStatus.UNAUTHORIZED).build();
    }
    return ResponseEntity.ok(userService.getCurrentUser());
}

// 403 FORBIDDEN - Authenticated but not authorized
@DeleteMapping("/users/{id}")
public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    if (!securityService.hasRole("ADMIN")) {
        return ResponseEntity.status(HttpStatus.FORBIDDEN).build();
    }
    userService.delete(id);
    return ResponseEntity.noContent().build();
}

// 404 NOT FOUND - Resource not found
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    return userService.findById(id)
        .map(ResponseEntity::ok)
        .orElse(ResponseEntity.notFound().build());
}

// 409 CONFLICT - Conflict (e.g., email already exists)
@PostMapping("/users")
public ResponseEntity<String> createUser(@RequestBody User user) {
    if (userService.emailExists(user.getEmail())) {
        return ResponseEntity
            .status(HttpStatus.CONFLICT)
            .body("Email already exists");
    }
    userService.save(user);
    return ResponseEntity.status(HttpStatus.CREATED).build();
}

// 422 UNPROCESSABLE ENTITY - Validation failed
@PostMapping("/users")
public ResponseEntity<Map<String, String>> createUser(
        @RequestBody @Valid User user,
        BindingResult result) {
    if (result.hasErrors()) {
        Map<String, String> errors = new HashMap<>();
        result.getFieldErrors().forEach(error -> 
            errors.put(error.getField(), error.getDefaultMessage())
        );
        return ResponseEntity
            .status(HttpStatus.UNPROCESSABLE_ENTITY)
            .body(errors);
    }
    userService.save(user);
    return ResponseEntity.status(HttpStatus.CREATED).build();
}

// 429 TOO MANY REQUESTS - Rate limiting
@GetMapping("/api/data")
public ResponseEntity<String> getData() {
    if (rateLimiter.isLimitExceeded()) {
        return ResponseEntity
            .status(HttpStatus.TOO_MANY_REQUESTS)
            .header("Retry-After", "3600")
            .body("Rate limit exceeded");
    }
    return ResponseEntity.ok("Data");
}
```

### 5xx Server Errors

```java
// 500 INTERNAL SERVER ERROR
@GetMapping("/users")
public ResponseEntity<List<User>> getUsers() {
    try {
        return ResponseEntity.ok(userService.findAll());
    } catch (Exception e) {
        log.error("Error fetching users", e);
        return ResponseEntity
            .status(HttpStatus.INTERNAL_SERVER_ERROR)
            .build();
    }
}

// 503 SERVICE UNAVAILABLE - Service temporarily unavailable
@GetMapping("/external-data")
public ResponseEntity<String> getExternalData() {
    if (!externalService.isAvailable()) {
        return ResponseEntity
            .status(HttpStatus.SERVICE_UNAVAILABLE)
            .header("Retry-After", "300")
            .body("Service temporarily unavailable");
    }
    return ResponseEntity.ok(externalService.getData());
}
```

### Global Error Handling

```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorResponse> handleNotFound(
            ResourceNotFoundException ex) {
        ErrorResponse error = new ErrorResponse(
            HttpStatus.NOT_FOUND.value(),
            ex.getMessage(),
            LocalDateTime.now()
        );
        return ResponseEntity.status(HttpStatus.NOT_FOUND).body(error);
    }
    
    @ExceptionHandler(ValidationException.class)
    public ResponseEntity<ErrorResponse> handleValidation(
            ValidationException ex) {
        ErrorResponse error = new ErrorResponse(
            HttpStatus.BAD_REQUEST.value(),
            ex.getMessage(),
            LocalDateTime.now()
        );
        return ResponseEntity.badRequest().body(error);
    }
    
    @ExceptionHandler(Exception.class)
    public ResponseEntity<ErrorResponse> handleGeneral(Exception ex) {
        ErrorResponse error = new ErrorResponse(
            HttpStatus.INTERNAL_SERVER_ERROR.value(),
            "An unexpected error occurred",
            LocalDateTime.now()
        );
        return ResponseEntity
            .status(HttpStatus.INTERNAL_SERVER_ERROR)
            .body(error);
    }
}

// ErrorResponse class
@Data
@AllArgsConstructor
public class ErrorResponse {
    private int status;
    private String message;
    private LocalDateTime timestamp;
}
```

## 🔐 @PreAuthorize - Method-Level Security

### Basic Configuration

```java
@Configuration
@EnableGlobalMethodSecurity(prePostEnabled = true)
public class SecurityConfig {
    // Spring Security configuration
}
```

### @PreAuthorize Expressions

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    // Access only if authenticated
    @PreAuthorize("isAuthenticated()")
    @GetMapping("/profile")
    public User getProfile() {
        return userService.getCurrentUser();
    }
    
    // Access only with ADMIN role
    @PreAuthorize("hasRole('ADMIN')")
    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
        userService.delete(id);
        return ResponseEntity.noContent().build();
    }
    
    // Access with one of the specified roles
    @PreAuthorize("hasAnyRole('ADMIN', 'MANAGER')")
    @PutMapping("/{id}")
    public ResponseEntity<User> updateUser(
            @PathVariable Long id,
            @RequestBody User user) {
        return ResponseEntity.ok(userService.update(id, user));
    }
    
    // Access with specific authority
    @PreAuthorize("hasAuthority('USER_WRITE')")
    @PostMapping
    public ResponseEntity<User> createUser(@RequestBody User user) {
        return ResponseEntity.status(HttpStatus.CREATED)
            .body(userService.save(user));
    }
    
    // Ownership check (user can edit their own data)
    @PreAuthorize("#id == authentication.principal.id")
    @PutMapping("/{id}/profile")
    public ResponseEntity<User> updateOwnProfile(
            @PathVariable Long id,
            @RequestBody User user) {
        return ResponseEntity.ok(userService.update(id, user));
    }
    
    // Complex expression with OR
    @PreAuthorize("hasRole('ADMIN') or #id == authentication.principal.id")
    @GetMapping("/{id}")
    public ResponseEntity<User> getUser(@PathVariable Long id) {
        return userService.findById(id)
            .map(ResponseEntity::ok)
            .orElse(ResponseEntity.notFound().build());
    }
    
    // Expression with custom method
    @PreAuthorize("@userSecurity.canAccessUser(#id)")
    @GetMapping("/{id}/details")
    public ResponseEntity<UserDetails> getUserDetails(@PathVariable Long id) {
        return ResponseEntity.ok(userService.getDetails(id));
    }
}

// Custom security service
@Service("userSecurity")
public class UserSecurityService {
    public boolean canAccessUser(Long userId) {
        Authentication auth = SecurityContextHolder.getContext().getAuthentication();
        User currentUser = (User) auth.getPrincipal();
        return currentUser.isAdmin() || currentUser.getId().equals(userId);
    }
}
```

### @PostAuthorize (Post-execution control)

```java
// Filters the result after execution
@PostAuthorize("returnObject.owner == authentication.principal.username")
@GetMapping("/documents/{id}")
public Document getDocument(@PathVariable Long id) {
    return documentService.findById(id);
}
```

### @Secured (Simple alternative)

```java
@Secured("ROLE_ADMIN")
@DeleteMapping("/{id}")
public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    userService.delete(id);
    return ResponseEntity.noContent().build();
}

@Secured({"ROLE_ADMIN", "ROLE_MANAGER"})
@PutMapping("/{id}")
public ResponseEntity<User> updateUser(@PathVariable Long id, @RequestBody User user) {
    return ResponseEntity.ok(userService.update(id, user));
}
```

## 📋 Validation with Bean Validation

### Validation Annotations

```java
@Data
public class CreateUserRequest {
    
    @NotNull(message = "Name is required")
    @Size(min = 2, max = 50, message = "Name must be between 2 and 50 characters")
    private String name;
    
    @NotBlank(message = "Email is required")
    @Email(message = "Email must be valid")
    private String email;
    
    @NotNull
    @Min(value = 18, message = "Must be at least 18 years old")
    @Max(value = 120, message = "Age must be realistic")
    private Integer age;
    
    @Pattern(regexp = "^\\+?[1-9]\\d{1,14}$", message = "Invalid phone number")
    private String phone;
    
    @Past(message = "Birth date must be in the past")
    private LocalDate birthDate;
    
    @Size(min = 8, message = "Password must be at least 8 characters")
    private String password;
}
```

### Usage in the Controller

```java
@RestController
@RequestMapping("/api/users")
@Validated
public class UserController {
    
    @PostMapping
    public ResponseEntity<?> createUser(
            @RequestBody @Valid CreateUserRequest request,
            BindingResult bindingResult) {
        
        if (bindingResult.hasErrors()) {
            Map<String, String> errors = new HashMap<>();
            bindingResult.getFieldErrors().forEach(error ->
                errors.put(error.getField(), error.getDefaultMessage())
            );
            return ResponseEntity
                .status(HttpStatus.BAD_REQUEST)
                .body(errors);
        }
        
        User user = userService.create(request);
        return ResponseEntity
            .status(HttpStatus.CREATED)
            .body(user);
    }
}
```

### Automatic Error Handling

```java
// Automatic handling with @RestControllerAdvice
@RestControllerAdvice
public class ValidationExceptionHandler {
    
    @ExceptionHandler(MethodArgumentNotValidException.class)
    public ResponseEntity<Map<String, Object>> handleValidationErrors(
            MethodArgumentNotValidException ex) {
        
        Map<String, String> errors = new HashMap<>();
        ex.getBindingResult().getFieldErrors().forEach(error ->
            errors.put(error.getField(), error.getDefaultMessage())
        );
        
        Map<String, Object> response = new HashMap<>();
        response.put("timestamp", LocalDateTime.now());
        response.put("status", HttpStatus.BAD_REQUEST.value());
        response.put("errors", errors);
        
        return ResponseEntity
            .status(HttpStatus.BAD_REQUEST)
            .body(response);
    }
    
    @ExceptionHandler(ConstraintViolationException.class)
    public ResponseEntity<Map<String, Object>> handleConstraintViolation(
            ConstraintViolationException ex) {
        
        Map<String, String> errors = new HashMap<>();
        ex.getConstraintViolations().forEach(violation -> {
            String propertyPath = violation.getPropertyPath().toString();
            String message = violation.getMessage();
            errors.put(propertyPath, message);
        });
        
        Map<String, Object> response = new HashMap<>();
        response.put("timestamp", LocalDateTime.now());
        response.put("status", HttpStatus.BAD_REQUEST.value());
        response.put("errors", errors);
        
        return ResponseEntity
            .status(HttpStatus.BAD_REQUEST)
            .body(response);
    }
}
```

### Validation Groups

```java
// Group interfaces
public interface CreateValidation {}
public interface UpdateValidation {}

// Entity with groups
@Data
public class User {
    
    @Null(groups = CreateValidation.class, message = "ID must be null for creation")
    @NotNull(groups = UpdateValidation.class, message = "ID is required for update")
    private Long id;
    
    @NotBlank(groups = {CreateValidation.class, UpdateValidation.class})
    @Size(min = 2, max = 50)
    private String name;
    
    @NotBlank(groups = CreateValidation.class)
    @Email
    private String email;
    
    @NotBlank(groups = CreateValidation.class)
    @Size(min = 8)
    private String password;
}

// Controller with groups
@PostMapping
public ResponseEntity<User> createUser(
        @RequestBody @Validated(CreateValidation.class) User user) {
    return ResponseEntity.status(HttpStatus.CREATED).body(userService.save(user));
}

@PutMapping("/{id}")
public ResponseEntity<User> updateUser(
        @PathVariable Long id,
        @RequestBody @Validated(UpdateValidation.class) User user) {
    return ResponseEntity.ok(userService.update(id, user));
}
```

### Custom Validation

```java
// Custom annotation
@Target({ElementType.FIELD})
@Retention(RetentionPolicy.RUNTIME)
@Constraint(validatedBy = UniqueEmailValidator.class)
public @interface UniqueEmail {
    String message() default "Email already exists";
    Class<?>[] groups() default {};
    Class<? extends Payload>[] payload() default {};
}

// Validator
@Component
public class UniqueEmailValidator implements ConstraintValidator<UniqueEmail, String> {
    
    @Autowired
    private UserRepository userRepository;
    
    @Override
    public boolean isValid(String email, ConstraintValidatorContext context) {
        if (email == null) {
            return true; // @NotNull handles this case
        }
        return !userRepository.existsByEmail(email);
    }
}

// Usage
@Data
public class CreateUserRequest {
    @NotBlank
    @Email
    @UniqueEmail
    private String email;
}
```

## 🌐 Content Negotiation - Response Formats

### Produces and Consumes

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    // Produces JSON by default
    @GetMapping(produces = MediaType.APPLICATION_JSON_VALUE)
    public List<User> getUsersJson() {
        return userService.findAll();
    }
    
    // Produces XML
    @GetMapping(produces = MediaType.APPLICATION_XML_VALUE)
    public List<User> getUsersXml() {
        return userService.findAll();
    }
    
    // Multiple formats
    @GetMapping(produces = {
        MediaType.APPLICATION_JSON_VALUE,
        MediaType.APPLICATION_XML_VALUE
    })
    public List<User> getUsers() {
        return userService.findAll();
    }
    
    // Accepts only JSON
    @PostMapping(
        consumes = MediaType.APPLICATION_JSON_VALUE,
        produces = MediaType.APPLICATION_JSON_VALUE
    )
    public ResponseEntity<User> createUser(@RequestBody User user) {
        return ResponseEntity.status(HttpStatus.CREATED).body(userService.save(user));
    }
    
    // File upload
    @PostMapping(
        value = "/upload",
        consumes = MediaType.MULTIPART_FORM_DATA_VALUE
    )
    public ResponseEntity<String> uploadFile(
            @RequestParam("file") MultipartFile file) {
        String filename = storageService.store(file);
        return ResponseEntity.ok("File uploaded: " + filename);
    }
    
    // File download
    @GetMapping(
        value = "/export",
        produces = "text/csv"
    )
    public ResponseEntity<String> exportUsers() {
        String csv = userService.exportToCsv();
        return ResponseEntity.ok()
            .header(HttpHeaders.CONTENT_DISPOSITION, "attachment; filename=users.csv")
            .body(csv);
    }
}
```

### Content Negotiation via Accept Header

```bash
# Client requests JSON
GET /api/users
Accept: application/json

# Client requests XML
GET /api/users
Accept: application/xml

# Client requests CSV
GET /api/users
Accept: text/csv
```

## 📤 File Upload and Download

### Simple Upload

```java
@RestController
@RequestMapping("/api/files")
public class FileController {
    
    @Autowired
    private StorageService storageService;
    
    // Upload a file
    @PostMapping("/upload")
    public ResponseEntity<Map<String, String>> uploadFile(
            @RequestParam("file") MultipartFile file) {
        
        if (file.isEmpty()) {
            return ResponseEntity.badRequest().body(
                Map.of("error", "File is empty")
            );
        }
        
        String filename = storageService.store(file);
        
        Map<String, String> response = new HashMap<>();
        response.put("filename", filename);
        response.put("size", String.valueOf(file.getSize()));
        response.put("contentType", file.getContentType());
        
        return ResponseEntity.ok(response);
    }
    
    // Multiple upload
    @PostMapping("/upload-multiple")
    public ResponseEntity<List<String>> uploadMultipleFiles(
            @RequestParam("files") MultipartFile[] files) {
        
        List<String> filenames = new ArrayList<>();
        
        for (MultipartFile file : files) {
            if (!file.isEmpty()) {
                String filename = storageService.store(file);
                filenames.add(filename);
            }
        }
        
        return ResponseEntity.ok(filenames);
    }
    
    // Upload with metadata
    @PostMapping("/upload-with-metadata")
    public ResponseEntity<FileMetadata> uploadWithMetadata(
            @RequestParam("file") MultipartFile file,
            @RequestParam("description") String description,
            @RequestParam("category") String category) {
        
        String filename = storageService.store(file);
        
        FileMetadata metadata = new FileMetadata();
        metadata.setFilename(filename);
        metadata.setDescription(description);
        metadata.setCategory(category);
        metadata.setSize(file.getSize());
        metadata.setUploadDate(LocalDateTime.now());
        
        return ResponseEntity.status(HttpStatus.CREATED).body(metadata);
    }
}
```

### File Download

```java
@RestController
@RequestMapping("/api/files")
public class FileController {
    
    // Simple download
    @GetMapping("/download/{filename:.+}")
    public ResponseEntity<Resource> downloadFile(@PathVariable String filename) {
        
        Resource file = storageService.loadAsResource(filename);
        
        return ResponseEntity.ok()
            .contentType(MediaType.APPLICATION_OCTET_STREAM)
            .header(HttpHeaders.CONTENT_DISPOSITION, 
                    "attachment; filename=\"" + file.getFilename() + "\"")
            .body(file);
    }
    
    // Download with detected MIME type
    @GetMapping("/view/{filename:.+}")
    public ResponseEntity<Resource> viewFile(@PathVariable String filename) {
        
        Resource file = storageService.loadAsResource(filename);
        String contentType = storageService.getContentType(filename);
        
        return ResponseEntity.ok()
            .contentType(MediaType.parseMediaType(contentType))
            .header(HttpHeaders.CONTENT_DISPOSITION, 
                    "inline; filename=\"" + file.getFilename() + "\"")
            .body(file);
    }
    
    // Streaming large file
    @GetMapping("/stream/{filename:.+}")
    public ResponseEntity<StreamingResponseBody> streamFile(
            @PathVariable String filename) {
        
        StreamingResponseBody stream = outputStream -> {
            Path path = storageService.load(filename);
            Files.copy(path, outputStream);
        };
        
        return ResponseEntity.ok()
            .contentType(MediaType.APPLICATION_OCTET_STREAM)
            .header(HttpHeaders.CONTENT_DISPOSITION, 
                    "attachment; filename=\"" + filename + "\"")
            .body(stream);
    }
    
    // CSV export
    @GetMapping("/export/users.csv")
    public ResponseEntity<String> exportUsersCsv() {
        String csv = userService.exportToCsv();
        
        return ResponseEntity.ok()
            .contentType(MediaType.parseMediaType("text/csv"))
            .header(HttpHeaders.CONTENT_DISPOSITION, 
                    "attachment; filename=\"users.csv\"")
            .body(csv);
    }
    
    // Excel export
    @GetMapping("/export/users.xlsx")
    public ResponseEntity<byte[]> exportUsersExcel() {
        byte[] excelBytes = userService.exportToExcel();
        
        return ResponseEntity.ok()
            .contentType(MediaType.parseMediaType(
                "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"))
            .header(HttpHeaders.CONTENT_DISPOSITION, 
                    "attachment; filename=\"users.xlsx\"")
            .body(excelBytes);
    }
}
```

## 🔄 Pagination and Sorting

### With Spring Data

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @Autowired
    private UserRepository userRepository;
    
    // Basic pagination
    @GetMapping
    public ResponseEntity<Page<User>> getUsers(
            @RequestParam(defaultValue = "0") int page,
            @RequestParam(defaultValue = "10") int size) {
        
        Pageable pageable = PageRequest.of(page, size);
        Page<User> users = userRepository.findAll(pageable);
        
        return ResponseEntity.ok(users);
    }
    
    // Pagination with sorting
    @GetMapping("/sorted")
    public ResponseEntity<Page<User>> getUsersSorted(
            @RequestParam(defaultValue = "0") int page,
            @RequestParam(defaultValue = "10") int size,
            @RequestParam(defaultValue = "id") String sortBy,
            @RequestParam(defaultValue = "ASC") String direction) {
        
        Sort.Direction sortDirection = Sort.Direction.fromString(direction);
        Pageable pageable = PageRequest.of(page, size, Sort.by(sortDirection, sortBy));
        Page<User> users = userRepository.findAll(pageable);
        
        return ResponseEntity.ok(users);
    }
    
    // With Pageable directly
    @GetMapping("/pageable")
    public ResponseEntity<Page<User>> getUsers(Pageable pageable) {
        // Spring automatically handles parameters:
        // ?page=0&size=10&sort=name,asc&sort=email,desc
        Page<User> users = userRepository.findAll(pageable);
        return ResponseEntity.ok(users);
    }
    
    // Custom response
    @GetMapping("/custom")
    public ResponseEntity<Map<String, Object>> getUsersCustom(
            @RequestParam(defaultValue = "0") int page,
            @RequestParam(defaultValue = "10") int size) {
        
        Page<User> userPage = userRepository.findAll(PageRequest.of(page, size));
        
        Map<String, Object> response = new HashMap<>();
        response.put("users", userPage.getContent());
        response.put("currentPage", userPage.getNumber());
        response.put("totalItems", userPage.getTotalElements());
        response.put("totalPages", userPage.getTotalPages());
        response.put("hasNext", userPage.hasNext());
        response.put("hasPrevious", userPage.hasPrevious());
        
        return ResponseEntity.ok(response);
    }
}
```

### Default Pagination Configuration

```java
@Configuration
public class WebConfig implements WebMvcConfigurer {
    
    @Override
    public void addArgumentResolvers(List<HandlerMethodArgumentResolver> resolvers) {
        PageableHandlerMethodArgumentResolver resolver = 
            new PageableHandlerMethodArgumentResolver();
        resolver.setMaxPageSize(100);
        resolver.setOneIndexedParameters(false); // Index starts at 0
        resolver.setFallbackPageable(PageRequest.of(0, 20));
        resolvers.add(resolver);
    }
}
```

## 🔍 Filtering and Search

### Simple Filtering

```java
@RestController
@RequestMapping("/api/products")
public class ProductController {
    
    // Filtering with optional parameters
    @GetMapping
    public ResponseEntity<List<Product>> getProducts(
            @RequestParam(required = false) String category,
            @RequestParam(required = false) Double minPrice,
            @RequestParam(required = false) Double maxPrice,
            @RequestParam(required = false) String brand,
            @RequestParam(required = false) Boolean inStock) {
        
        List<Product> products = productService.findWithFilters(
            category, minPrice, maxPrice, brand, inStock
        );
        
        return ResponseEntity.ok(products);
    }
    
    // Search by keyword
    @GetMapping("/search")
    public ResponseEntity<List<Product>> searchProducts(
            @RequestParam String query) {
        List<Product> products = productService.search(query);
        return ResponseEntity.ok(products);
    }
    
    // Advanced search with filter object
    @PostMapping("/search")
    public ResponseEntity<Page<Product>> advancedSearch(
            @RequestBody ProductSearchCriteria criteria,
            Pageable pageable) {
        
        Page<Product> products = productService.search(criteria, pageable);
        return ResponseEntity.ok(products);
    }
}

// Criteria object
@Data
public class ProductSearchCriteria {
    private String name;
    private String category;
    private Double minPrice;
    private Double maxPrice;
    private List<String> brands;
    private Boolean inStock;
    private LocalDate startDate;
    private LocalDate endDate;
}
```

### JPA Specifications (Dynamic Filtering)

```java
// Specification
public class ProductSpecifications {
    
    public static Specification<Product> hasCategory(String category) {
        return (root, query, cb) -> 
            category == null ? null : cb.equal(root.get("category"), category);
    }
    
    public static Specification<Product> priceBetween(Double min, Double max) {
        return (root, query, cb) -> {
            if (min != null && max != null) {
                return cb.between(root.get("price"), min, max);
            } else if (min != null) {
                return cb.greaterThanOrEqualTo(root.get("price"), min);
            } else if (max != null) {
                return cb.lessThanOrEqualTo(root.get("price"), max);
            }
            return null;
        };
    }
    
    public static Specification<Product> nameContains(String keyword) {
        return (root, query, cb) -> 
            keyword == null ? null : 
            cb.like(cb.lower(root.get("name")), "%" + keyword.toLowerCase() + "%");
    }
}

// Repository
public interface ProductRepository extends JpaRepository<Product, Long>, 
                                          JpaSpecificationExecutor<Product> {
}

// Controller
@RestController
@RequestMapping("/api/products")
public class ProductController {
    
    @GetMapping("/filter")
    public ResponseEntity<Page<Product>> filterProducts(
            @RequestParam(required = false) String category,
            @RequestParam(required = false) Double minPrice,
            @RequestParam(required = false) Double maxPrice,
            @RequestParam(required = false) String keyword,
            Pageable pageable) {
        
        Specification<Product> spec = Specification
            .where(ProductSpecifications.hasCategory(category))
            .and(ProductSpecifications.priceBetween(minPrice, maxPrice))
            .and(ProductSpecifications.nameContains(keyword));
        
        Page<Product> products = productRepository.findAll(spec, pageable);
        return ResponseEntity.ok(products);
    }
}
```

## 🌍 CORS (Cross-Origin Resource Sharing)

### Global Configuration

```java
@Configuration
public class WebConfig implements WebMvcConfigurer {
    
    @Override
    public void addCorsMappings(CorsRegistry registry) {
        registry.addMapping("/api/**")
            .allowedOrigins("http://localhost:3000", "https://myapp.com")
            .allowedMethods("GET", "POST", "PUT", "DELETE", "PATCH")
            .allowedHeaders("*")
            .allowCredentials(true)
            .maxAge(3600);
    }
}
```

### CORS per Controller

```java
@RestController
@RequestMapping("/api/users")
@CrossOrigin(origins = "http://localhost:3000")
public class UserController {
    // All endpoints of this controller accept requests from localhost:3000
}
```

### CORS per Endpoint

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @CrossOrigin(origins = {"http://localhost:3000", "https://myapp.com"})
    @GetMapping
    public List<User> getUsers() {
        return userService.findAll();
    }
}
```

### Full CORS Configuration

```java
@Configuration
public class CorsConfig {
    
    @Bean
    public CorsConfigurationSource corsConfigurationSource() {
        CorsConfiguration configuration = new CorsConfiguration();
        configuration.setAllowedOrigins(Arrays.asList(
            "http://localhost:3000",
            "https://myapp.com"
        ));
        configuration.setAllowedMethods(Arrays.asList(
            "GET", "POST", "PUT", "DELETE", "PATCH", "OPTIONS"
        ));
        configuration.setAllowedHeaders(Arrays.asList(
            "Authorization",
            "Content-Type",
            "X-Requested-With"
        ));
        configuration.setExposedHeaders(Arrays.asList(
            "Authorization",
            "X-Total-Count"
        ));
        configuration.setAllowCredentials(true);
        configuration.setMaxAge(3600L);
        
        UrlBasedCorsConfigurationSource source = new UrlBasedCorsConfigurationSource();
        source.registerCorsConfiguration("/api/**", configuration);
        return source;
    }
}
```

## 📊 Complete Example - CRUD API

```java
@RestController
@RequestMapping("/api/products")
@Validated
@CrossOrigin(origins = "http://localhost:3000")
public class ProductController {
    
    @Autowired
    private ProductService productService;
    
    // GET - List with pagination, sorting and filtering
    @GetMapping
    public ResponseEntity<Page<ProductDTO>> getProducts(
            @RequestParam(required = false) String category,
            @RequestParam(required = false) Double minPrice,
            @RequestParam(required = false) Double maxPrice,
            @RequestParam(required = false) String search,
            @RequestParam(defaultValue = "0") int page,
            @RequestParam(defaultValue = "20") int size,
            @RequestParam(defaultValue = "id") String sortBy,
            @RequestParam(defaultValue = "ASC") String direction) {
        
        Pageable pageable = PageRequest.of(
            page, size, 
            Sort.by(Sort.Direction.fromString(direction), sortBy)
        );
        
        Page<ProductDTO> products = productService.findAll(
            category, minPrice, maxPrice, search, pageable
        );
        
        return ResponseEntity.ok(products);
    }
    
    // GET - One product by ID
    @GetMapping("/{id}")
    public ResponseEntity<ProductDTO> getProduct(@PathVariable Long id) {
        return productService.findById(id)
            .map(ResponseEntity::ok)
            .orElse(ResponseEntity.notFound().build());
    }
    
    // POST - Create a product
    @PreAuthorize("hasRole('ADMIN')")
    @PostMapping
    public ResponseEntity<ProductDTO> createProduct(
            @RequestBody @Valid CreateProductRequest request) {
        
        ProductDTO created = productService.create(request);
        
        URI location = ServletUriComponentsBuilder
            .fromCurrentRequest()
            .path("/{id}")
            .buildAndExpand(created.getId())
            .toUri();
        
        return ResponseEntity.created(location).body(created);
    }
    
    // PUT - Full update
    @PreAuthorize("hasRole('ADMIN')")
    @PutMapping("/{id}")
    public ResponseEntity<ProductDTO> updateProduct(
            @PathVariable Long id,
            @RequestBody @Valid UpdateProductRequest request) {
        
        return productService.update(id, request)
            .map(ResponseEntity::ok)
            .orElse(ResponseEntity.notFound().build());
    }
    
    // PATCH - Partial update
    @PreAuthorize("hasRole('ADMIN')")
    @PatchMapping("/{id}")
    public ResponseEntity<ProductDTO> partialUpdate(
            @PathVariable Long id,
            @RequestBody Map<String, Object> updates) {
        
        return productService.partialUpdate(id, updates)
            .map(ResponseEntity::ok)
            .orElse(ResponseEntity.notFound().build());
    }
    
    // DELETE
    @PreAuthorize("hasRole('ADMIN')")
    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteProduct(@PathVariable Long id) {
        boolean deleted = productService.delete(id);
        return deleted ? 
            ResponseEntity.noContent().build() : 
            ResponseEntity.notFound().build();
    }
    
    // Extra endpoints
    
    @GetMapping("/categories")
    public ResponseEntity<List<String>> getCategories() {
        return ResponseEntity.ok(productService.getAllCategories());
    }
    
    @GetMapping("/featured")
    public ResponseEntity<List<ProductDTO>> getFeaturedProducts() {
        return ResponseEntity.ok(productService.findFeatured());
    }
    
    @PostMapping("/{id}/favorite")
    @PreAuthorize("isAuthenticated()")
    public ResponseEntity<Void> addToFavorites(@PathVariable Long id) {
        productService.addToFavorites(id);
        return ResponseEntity.ok().build();
    }
    
    @DeleteMapping("/{id}/favorite")
    @PreAuthorize("isAuthenticated()")
    public ResponseEntity<Void> removeFromFavorites(@PathVariable Long id) {
        productService.removeFromFavorites(id);
        return ResponseEntity.noContent().build();
    }
}
```

---
# Q&A - REST Controllers & Endpoints Spring Boot

## 🎮 Controllers - MVC Architecture

**Q1 : What is the difference between @Controller and @RestController?**
**A :**
- **@Controller** : For traditional web applications, returns a view name (JSP, Thymeleaf)
- **@RestController** : For REST APIs, returns data directly (JSON/XML). Equivalent to `@Controller + @ResponseBody` on each method

**Q2 : What does @RestController do exactly?**
**A :** `@RestController` is a meta-annotation combining `@Controller` and `@ResponseBody`. It indicates that each controller method directly returns serialized data (JSON) in the HTTP response body, rather than a view name.

**Q3 : What is the typical architecture of a Spring Boot application?**
**A :**
```
Client → Controller (@RestController) 
       → Service (@Service) 
       → Repository (@Repository) 
       → Database
```
- Controller : Handles HTTP requests
- Service : Business logic
- Repository : Data access

**Q4 : Can you have both @Controller and @RestController in the same application?**
**A :** Yes! Use @Controller for web pages (views) and @RestController for REST APIs. This is common in applications that have both a web UI AND an API.

## 🛣️ Endpoints Mapping

**Q5 : What is the difference between @RequestMapping and @GetMapping?**
**A :**
- **@RequestMapping** : Generic annotation, requires explicitly specifying the HTTP method
- **@GetMapping** : Specialized for GET, more concise and readable (equivalent to `@RequestMapping(method = RequestMethod.GET)`)

**Q6 : Name the 5 specialized mapping annotations**
**A :**
1. `@GetMapping` - Retrieve data
2. `@PostMapping` - Create a resource
3. `@PutMapping` - Full update
4. `@PatchMapping` - Partial update
5. `@DeleteMapping` - Delete a resource

**Q7 : How do you define a common prefix for all endpoints in a controller?**
**A :**
```java
@RestController
@RequestMapping("/api/users")  // Prefix
public class UserController {
    @GetMapping  // Accessible via /api/users
    @GetMapping("/{id}")  // Accessible via /api/users/{id}
}
```

**Q8 : What is the difference between PUT and PATCH?**
**A :**
- **PUT** : Full replacement of the resource (all fields required)
- **PATCH** : Partial update (only the modified fields)

**Q9 : How do you map multiple URLs to the same method?**
**A :**
```java
@GetMapping({"/users", "/members", "/people"})
public List<User> getUsers() {
    return userService.findAll();
}
```

**Q10 : How do you add conditions to the mapping (params, headers)?**
**A :**
```java
@GetMapping(
    value = "/users",
    params = "active=true",           // Requires ?active=true
    headers = "X-API-Version=1",      // Requires this header
    consumes = "application/json",    // Accepts JSON
    produces = "application/json"     // Produces JSON
)
```

## 📥 @PathVariable

**Q11 : What is @PathVariable and how do you use it?**
**A :** `@PathVariable` extracts values from the URL. Example :
```java
@GetMapping("/users/{id}")
public User getUser(@PathVariable Long id) {
    return userService.findById(id);
}
// GET /api/users/123 → id = 123
```

**Q12 : How do you handle a PathVariable with a different name than the Java variable?**
**A :**
```java
@GetMapping("/users/{userId}")
public User getUser(@PathVariable("userId") Long id) {
    return userService.findById(id);
}
```

**Q13 : How do you handle multiple PathVariables?**
**A :**
```java
@GetMapping("/users/{userId}/posts/{postId}")
public Post getUserPost(
        @PathVariable Long userId,
        @PathVariable Long postId) {
    return postService.findByUserAndId(userId, postId);
}
// GET /api/users/5/posts/42
```

**Q14 : How do you make a PathVariable optional?**
**A :**
```java
@GetMapping({"/users", "/users/{id}"})
public ResponseEntity<?> getUsers(
        @PathVariable(required = false) Long id) {
    if (id == null) {
        return ResponseEntity.ok(userService.findAll());
    }
    return ResponseEntity.ok(userService.findById(id));
}
```

**Q15 : How do you get all URL variables in a Map?**
**A :**
```java
@GetMapping("/data/{var1}/{var2}/{var3}")
public String getData(@PathVariable Map<String, String> pathVars) {
    // pathVars contains all variables
    return pathVars.toString();
}
```

## 📋 @RequestParam

**Q16 : What is @RequestParam and how do you use it?**
**A :** `@RequestParam` extracts query string parameters. Example :
```java
@GetMapping("/users")
public List<User> findUsers(@RequestParam String name) {
    return userService.findByName(name);
}
// GET /api/users?name=John
```

**Q17 : How do you make a RequestParam optional with a default value?**
**A :**
```java
@GetMapping("/users")
public List<User> getUsers(
        @RequestParam(required = false, defaultValue = "0") int page,
        @RequestParam(defaultValue = "10") int size) {
    return userService.findAll(page, size);
}
// GET /api/users → page=0, size=10
// GET /api/users?page=2&size=20 → page=2, size=20
```

**Q18 : What is the difference between @PathVariable and @RequestParam?**
**A :**
- **@PathVariable** : Variables in the URL → `/users/{id}` → `/users/123`
- **@RequestParam** : Query parameters → `/users?id=123`

PathVariable for resource identifiers, RequestParam for filters/options.

**Q19 : How do you handle multiple values for the same parameter?**
**A :**
```java
@GetMapping("/users")
public List<User> findUsers(@RequestParam List<Long> id) {
    return userService.findByIds(id);
}
// GET /api/users?id=1&id=2&id=3
```

**Q20 : How do you get all query params in a Map?**
**A :**
```java
@GetMapping("/search")
public List<User> search(@RequestParam Map<String, String> allParams) {
    // allParams contains all query params
    return userService.search(allParams);
}
```

**Q21 : Practical example : pagination and filtering with RequestParam**
**A :**
```java
@GetMapping("/products")
public List<Product> getProducts(
        @RequestParam(required = false) String category,
        @RequestParam(required = false) Double minPrice,
        @RequestParam(required = false) Double maxPrice,
        @RequestParam(defaultValue = "0") int page,
        @RequestParam(defaultValue = "20") int size) {
    return productService.findWithFilters(
        category, minPrice, maxPrice, page, size
    );
}
// GET /api/products?category=electronics&minPrice=100&page=1&size=10
```

## 📨 @RequestBody

**Q22 : What is @RequestBody and when should you use it?**
**A :** `@RequestBody` converts the JSON body of the request into a Java object. Used with POST/PUT/PATCH to receive data :
```java
@PostMapping("/users")
public User createUser(@RequestBody User user) {
    return userService.save(user);
}
```

**Q23 : How do you validate data received with @RequestBody?**
**A :**
```java
@PostMapping("/users")
public ResponseEntity<User> createUser(
        @RequestBody @Valid User user) {
    // @Valid triggers Bean Validation
    User created = userService.save(user);
    return ResponseEntity.status(HttpStatus.CREATED).body(created);
}
```

**Q24 : What is the difference between @RequestBody and @RequestParam?**
**A :**
- **@RequestBody** : Data in the request body (JSON/XML), for complex structures
- **@RequestParam** : Parameters in the URL (?key=value), for simple values

**Q25 : Can you combine @RequestBody with @PathVariable and @RequestParam?**
**A :** Yes :
```java
@PutMapping("/users/{id}")
public User updateUser(
        @PathVariable Long id,
        @RequestParam(defaultValue = "false") boolean notify,
        @RequestBody User user) {
    return userService.update(id, user, notify);
}
// PUT /api/users/5?notify=true
// Body: {"name": "John", "email": "john@example.com"}
```

**Q26 : How do you use a DTO instead of the entity with @RequestBody?**
**A :**
```java
@PostMapping("/users")
public ResponseEntity<UserResponse> createUser(
        @RequestBody @Valid CreateUserRequest request) {
    User user = userService.create(request);
    UserResponse response = new UserResponse(user);
    return ResponseEntity.status(HttpStatus.CREATED).body(response);
}
```
DTOs separate the external API from the internal structure (security, flexibility).

## 📤 ResponseEntity<>

**Q27 : What is ResponseEntity and why use it?**
**A :** `ResponseEntity<T>` is a wrapper for full control over the HTTP response : status code, headers, and body. Offers more control than simply returning an object.

**Q28 : What is the difference between returning an object directly vs ResponseEntity?**
**A :**
```java
// Direct return (status 200 by default)
@GetMapping("/users")
public List<User> getUsers() {
    return userService.findAll();
}

// With ResponseEntity (status control)
@GetMapping("/users")
public ResponseEntity<List<User>> getUsers() {
    List<User> users = userService.findAll();
    return ResponseEntity.ok(users);  // Or .notFound(), .created(), etc.
}
```

**Q29 : How do you return a 201 CREATED with ResponseEntity?**
**A :**
```java
@PostMapping("/users")
public ResponseEntity<User> createUser(@RequestBody User user) {
    User created = userService.save(user);
    URI location = URI.create("/api/users/" + created.getId());
    return ResponseEntity.created(location).body(created);
}
```

**Q30 : How do you return a 204 NO CONTENT?**
**A :**
```java
@DeleteMapping("/users/{id}")
public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    userService.delete(id);
    return ResponseEntity.noContent().build();
}
```

**Q31 : How do you return a 404 NOT FOUND with Optional?**
**A :**
```java
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    return userService.findById(id)
        .map(ResponseEntity::ok)
        .orElse(ResponseEntity.notFound().build());
}
```

**Q32 : How do you add custom headers to the response?**
**A :**
```java
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    User user = userService.findById(id);
    
    return ResponseEntity.ok()
        .header("X-Custom-Header", "value")
        .header(HttpHeaders.CACHE_CONTROL, "max-age=3600")
        .eTag("\"user-" + user.getId() + "\"")
        .body(user);
}
```

**Q33 : How do you handle file downloads with ResponseEntity?**
**A :**
```java
@GetMapping("/files/{filename}")
public ResponseEntity<Resource> downloadFile(@PathVariable String filename) {
    Resource file = storageService.loadAsResource(filename);
    
    return ResponseEntity.ok()
        .contentType(MediaType.APPLICATION_OCTET_STREAM)
        .header(HttpHeaders.CONTENT_DISPOSITION, 
                "attachment; filename=\"" + file.getFilename() + "\"")
        .body(file);
}
```

**Q34 : How do you build a complex ResponseEntity with the builder pattern?**
**A :**
```java
return ResponseEntity
    .status(HttpStatus.OK)
    .header("X-Custom", "value")
    .contentType(MediaType.APPLICATION_JSON)
    .eTag("\"abc123\"")
    .lastModified(Instant.now())
    .body(data);
```

## 🚦 HTTP Status Codes

**Q35 : What are the main 2xx status codes and their usage?**
**A :**
- **200 OK** : Successful request (GET, PUT)
- **201 CREATED** : Resource created (POST)
- **202 ACCEPTED** : Request accepted, async processing
- **204 NO CONTENT** : Success with no content (DELETE)

**Q36 : What are the main 4xx status codes and their usage?**
**A :**
- **400 BAD REQUEST** : Invalid request (validation failed)
- **401 UNAUTHORIZED** : Not authenticated
- **403 FORBIDDEN** : Authenticated but not authorized
- **404 NOT FOUND** : Resource not found
- **409 CONFLICT** : Conflict (email already exists)
- **422 UNPROCESSABLE ENTITY** : Business validation failed

**Q37 : What is the difference between 401 and 403?**
**A :**
- **401 UNAUTHORIZED** : "Who are you?" → Not authenticated, login required
- **403 FORBIDDEN** : "I know who you are, but you don't have permission" → Authenticated but not authorized

**Q38 : When should you use 400 vs 422?**
**A :**
- **400 BAD REQUEST** : Format/syntax error (invalid JSON, missing field)
- **422 UNPROCESSABLE ENTITY** : Correct format but business validation failed (email already used, insufficient stock)

**Q39 : How do you return a 409 CONFLICT?**
**A :**
```java
@PostMapping("/users")
public ResponseEntity<String> createUser(@RequestBody User user) {
    if (userService.emailExists(user.getEmail())) {
        return ResponseEntity
            .status(HttpStatus.CONFLICT)
            .body("Email already exists");
    }
    userService.save(user);
    return ResponseEntity.status(HttpStatus.CREATED).build();
}
```

**Q40 : How do you handle 5xx errors?**
**A :**
```java
@GetMapping("/users")
public ResponseEntity<List<User>> getUsers() {
    try {
        return ResponseEntity.ok(userService.findAll());
    } catch (Exception e) {
        log.error("Error fetching users", e);
        return ResponseEntity
            .status(HttpStatus.INTERNAL_SERVER_ERROR)
            .build();
    }
}
```
But prefer a `@RestControllerAdvice` for global handling.

## 🛡️ Global Error Handling

**Q41 : What is @RestControllerAdvice?**
**A :** `@RestControllerAdvice` is an annotation for globally handling exceptions across the entire application. Avoids repeating try-catch in every controller.

**Q42 : How do you globally handle 404 exceptions?**
**A :**
```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorResponse> handleNotFound(
            ResourceNotFoundException ex) {
        ErrorResponse error = new ErrorResponse(
            HttpStatus.NOT_FOUND.value(),
            ex.getMessage(),
            LocalDateTime.now()
        );
        return ResponseEntity.status(HttpStatus.NOT_FOUND).body(error);
    }
}
```

**Q43 : How do you automatically handle validation errors (@Valid)?**
**A :**
```java
@RestControllerAdvice
public class ValidationExceptionHandler {
    
    @ExceptionHandler(MethodArgumentNotValidException.class)
    public ResponseEntity<Map<String, Object>> handleValidation(
            MethodArgumentNotValidException ex) {
        
        Map<String, String> errors = new HashMap<>();
        ex.getBindingResult().getFieldErrors().forEach(error ->
            errors.put(error.getField(), error.getDefaultMessage())
        );
        
        Map<String, Object> response = new HashMap<>();
        response.put("timestamp", LocalDateTime.now());
        response.put("status", HttpStatus.BAD_REQUEST.value());
        response.put("errors", errors);
        
        return ResponseEntity.badRequest().body(response);
    }
}
```

**Q44 : What is the difference between @ControllerAdvice and @RestControllerAdvice?**
**A :**
- **@ControllerAdvice** : For web applications with views
- **@RestControllerAdvice** : For REST APIs, equivalent to `@ControllerAdvice + @ResponseBody`

## ✅ Validation

**Q45 : What are the main Bean Validation annotations?**
**A :**
- `@NotNull` : Field not null
- `@NotBlank` : String not empty (not null, not blank)
- `@NotEmpty` : Collection/String not empty
- `@Size(min, max)` : Size
- `@Min` / `@Max` : Numeric value
- `@Email` : Valid email format
- `@Pattern(regexp)` : Regular expression
- `@Past` / `@Future` : Past/future date

**Q46 : How do you enable validation in a controller?**
**A :**
```java
@PostMapping("/users")
public ResponseEntity<User> createUser(
        @RequestBody @Valid User user) {  // @Valid enables validation
    User created = userService.save(user);
    return ResponseEntity.status(HttpStatus.CREATED).body(created);
}
```

**Q47 : How do you define validations on a DTO?**
**A :**
```java
@Data
public class CreateUserRequest {
    @NotBlank(message = "Name is required")
    @Size(min = 2, max = 50)
    private String name;
    
    @NotBlank
    @Email(message = "Email must be valid")
    private String email;
    
    @NotNull
    @Min(18)
    @Max(120)
    private Integer age;
}
```

**Q48 : How do you manually handle validation errors?**
**A :**
```java
@PostMapping("/users")
public ResponseEntity<?> createUser(
        @RequestBody @Valid User user,
        BindingResult bindingResult) {
    
    if (bindingResult.hasErrors()) {
        Map<String, String> errors = new HashMap<>();
        bindingResult.getFieldErrors().forEach(error ->
            errors.put(error.getField(), error.getDefaultMessage())
        );
        return ResponseEntity.badRequest().body(errors);
    }
    
    return ResponseEntity.ok(userService.save(user));
}
```

**Q49 : How do you create a custom validation?**
**A :**
```java
// Annotation
@Target({ElementType.FIELD})
@Retention(RetentionPolicy.RUNTIME)
@Constraint(validatedBy = UniqueEmailValidator.class)
public @interface UniqueEmail {
    String message() default "Email already exists";
    Class<?>[] groups() default {};
    Class<? extends Payload>[] payload() default {};
}

// Validator
@Component
public class UniqueEmailValidator 
        implements ConstraintValidator<UniqueEmail, String> {
    
    @Autowired
    private UserRepository userRepository;
    
    @Override
    public boolean isValid(String email, ConstraintValidatorContext context) {
        return email != null && !userRepository.existsByEmail(email);
    }
}

// Usage
@Data
public class CreateUserRequest {
    @NotBlank
    @Email
    @UniqueEmail
    private String email;
}
```

**Q50 : How do you use validation groups?**
**A :**
```java
// Group interfaces
public interface CreateValidation {}
public interface UpdateValidation {}

// Entity
@Data
public class User {
    @Null(groups = CreateValidation.class)
    @NotNull(groups = UpdateValidation.class)
    private Long id;
    
    @NotBlank(groups = {CreateValidation.class, UpdateValidation.class})
    private String name;
}

// Controller
@PostMapping
public ResponseEntity<User> create(
        @RequestBody @Validated(CreateValidation.class) User user) {
    return ResponseEntity.ok(userService.save(user));
}

@PutMapping("/{id}")
public ResponseEntity<User> update(
        @PathVariable Long id,
        @RequestBody @Validated(UpdateValidation.class) User user) {
    return ResponseEntity.ok(userService.update(id, user));
}
```

## 🔐 @PreAuthorize

**Q51 : What is @PreAuthorize?**
**A :** `@PreAuthorize` is a Spring Security annotation to control access to a method BEFORE its execution. Allows defining authorization rules with SpEL expressions.

**Q52 : How do you enable @PreAuthorize?**
**A :**
```java
@Configuration
@EnableGlobalMethodSecurity(prePostEnabled = true)
public class SecurityConfig {
    // Configuration
}
```

**Q53 : How do you restrict an endpoint to authenticated users?**
**A :**
```java
@PreAuthorize("isAuthenticated()")
@GetMapping("/profile")
public User getProfile() {
    return userService.getCurrentUser();
}
```

**Q54 : How do you restrict by role?**
**A :**
```java
@PreAuthorize("hasRole('ADMIN')")
@DeleteMapping("/{id}")
public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    userService.delete(id);
    return ResponseEntity.noContent().build();
}
```

**Q55 : How do you allow multiple roles?**
**A :**
```java
@PreAuthorize("hasAnyRole('ADMIN', 'MANAGER', 'SUPERVISOR')")
@PutMapping("/{id}")
public ResponseEntity<User> updateUser(@PathVariable Long id, @RequestBody User user) {
    return ResponseEntity.ok(userService.update(id, user));
}
```

**Q56 : How do you verify a user is modifying their own data?**
**A :**
```java
@PreAuthorize("#id == authentication.principal.id")
@PutMapping("/{id}/profile")
public ResponseEntity<User> updateOwnProfile(
        @PathVariable Long id,
        @RequestBody User user) {
    return ResponseEntity.ok(userService.update(id, user));
}
```

**Q57 : How do you combine multiple conditions with OR?**
**A :**
```java
@PreAuthorize("hasRole('ADMIN') or #id == authentication.principal.id")
@GetMapping("/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    return userService.findById(id)
        .map(ResponseEntity::ok)
        .orElse(ResponseEntity.notFound().build());
}
```

**Q58 : How do you use a custom method for security?**
**A :**
```java
@PreAuthorize("@userSecurity.canAccessUser(#id)")
@GetMapping("/{id}/details")
public ResponseEntity<UserDetails> getUserDetails(@PathVariable Long id) {
    return ResponseEntity.ok(userService.getDetails(id));
}

// Service
@Service("userSecurity")
public class UserSecurityService {
    public boolean canAccessUser(Long userId) {
        Authentication auth = SecurityContextHolder
            .getContext().getAuthentication();
        User currentUser = (User) auth.getPrincipal();
        return currentUser.isAdmin() || 
               currentUser.getId().equals(userId);
    }
}
```

**Q59 : What is the difference between @PreAuthorize and @Secured?**
**A :**
- **@PreAuthorize** : Complex SpEL expressions, multiple conditions
- **@Secured** : Simple, static roles only

`@PreAuthorize` is more powerful and flexible.

**Q60 : What is @PostAuthorize?**
**A :** `@PostAuthorize` checks authorization AFTER method execution, based on the result :
```java
@PostAuthorize("returnObject.owner == authentication.principal.username")
@GetMapping("/documents/{id}")
public Document getDocument(@PathVariable Long id) {
    return documentService.findById(id);
}
// Access allowed only if the document belongs to the user
```

## 📤📥 Upload and Download

**Q61 : How do you handle a file upload?**
**A :**
```java
@PostMapping("/upload")
public ResponseEntity<Map<String, String>> uploadFile(
        @RequestParam("file") MultipartFile file) {
    
    if (file.isEmpty()) {
        return ResponseEntity.badRequest()
            .body(Map.of("error", "File is empty"));
    }
    
    String filename = storageService.store(file);
    
    Map<String, String> response = Map.of(
        "filename", filename,
        "size", String.valueOf(file.getSize()),
        "contentType", file.getContentType()
    );
    
    return ResponseEntity.ok(response);
}
```

**Q62 : How do you handle uploading multiple files?**
**A :**
```java
@PostMapping("/upload-multiple")
public ResponseEntity<List<String>> uploadMultiple(
        @RequestParam("files") MultipartFile[] files) {
    
    List<String> filenames = new ArrayList<>();
    for (MultipartFile file : files) {
        if (!file.isEmpty()) {
            filenames.add(storageService.store(file));
        }
    }
    
    return ResponseEntity.ok(filenames);
}
```

**Q63 : How do you download a file?**
**A :**
```java
@GetMapping("/download/{filename:.+}")
public ResponseEntity<Resource> downloadFile(@PathVariable String filename) {
    Resource file = storageService.loadAsResource(filename);
    
    return ResponseEntity.ok()
        .contentType(MediaType.APPLICATION_OCTET_STREAM)
        .header(HttpHeaders.CONTENT_DISPOSITION, 
                "attachment; filename=\"" + file.getFilename() + "\"")
        .body(file);
}
```

**Q64 : What is the difference between "attachment" and "inline" for Content-Disposition?**
**A :**
- **attachment** : Forces download ("Save as" dialog)
- **inline** : Displays in browser if possible (PDF, images)

**Q65 : How do you export data as CSV?**
**A :**
```java
@GetMapping("/export/users.csv")
public ResponseEntity<String> exportCsv() {
    String csv = userService.exportToCsv();
    
    return ResponseEntity.ok()
        .contentType(MediaType.parseMediaType("text/csv"))
        .header(HttpHeaders.CONTENT_DISPOSITION, 
                "attachment; filename=\"users.csv\"")
        .body(csv);
}
```

## 📄 Pagination and Sorting

**Q66 : How do you implement pagination with Spring Data?**
**A :**
```java
@GetMapping
public ResponseEntity<Page<User>> getUsers(
        @RequestParam(defaultValue = "0") int page,
        @RequestParam(defaultValue = "10") int size) {
    
    Pageable pageable = PageRequest.of(page, size);
    Page<User> users = userRepository.findAll(pageable);
    
    return ResponseEntity.ok(users);
}
// GET /api/users?page=0&size=10
```

**Q67 : How do you add sorting to pagination?**
**A :**
```java
@GetMapping
public ResponseEntity<Page<User>> getUsers(
        @RequestParam(defaultValue = "0") int page,
        @RequestParam(defaultValue = "10") int size,
        @RequestParam(defaultValue = "id") String sortBy,
        @RequestParam(defaultValue = "ASC") String direction) {
    
    Sort.Direction sortDir = Sort.Direction.fromString(direction);
    Pageable pageable = PageRequest.of(page, size, 
                                       Sort.by(sortDir, sortBy));
    
    return ResponseEntity.ok(userRepository.findAll(pageable));
}
// GET /api/users?page=0&size=10&sortBy=name&direction=DESC
```

**Q68 : How do you use Pageable directly as a parameter?**
**A :**
```java
@GetMapping
public ResponseEntity<Page<User>> getUsers(Pageable pageable) {
    // Spring automatically handles:
    // ?page=0&size=10&sort=name,asc&sort=email,desc
    return ResponseEntity.ok(userRepository.findAll(pageable));
}
```

**Q69 : How do you customize the pagination response?**
**A :**
```java
@GetMapping
public ResponseEntity<Map<String, Object>> getUsers(
        @RequestParam(defaultValue = "0") int page,
        @RequestParam(defaultValue = "10") int size) {
    
    Page<User> userPage = userRepository.findAll(PageRequest.of(page, size));
    
    Map<String, Object> response = Map.of(
        "users", userPage.getContent(),
        "currentPage", userPage.getNumber(),
        "totalItems", userPage.getTotalElements(),
        "totalPages", userPage.getTotalPages(),
        "hasNext", userPage.hasNext(),
        "hasPrevious", userPage.hasPrevious()
    );
    
    return ResponseEntity.ok(response);
}
```

**Q70 : What does a Spring Data Page object return?**
**A :** A `Page<T>` object contains :
- `getContent()` : List of items on the current page
- `getTotalElements()` : Total number of items
- `getTotalPages()` : Total number of pages
- `getNumber()` : Current page number
- `getSize()` : Page size
- `hasNext()` / `hasPrevious()` : Navigation booleans
- `isFirst()` / `isLast()` : First/last page

## 🔍 Filtering and Search

**Q71 : How do you implement simple filtering with optional RequestParams?**
**A :**
```java
@GetMapping("/products")
public ResponseEntity<List<Product>> getProducts(
        @RequestParam(required = false) String category,
        @RequestParam(required = false) Double minPrice,
        @RequestParam(required = false) Double maxPrice,
        @RequestParam(required = false) Boolean inStock) {
    
    List<Product> products = productService.findWithFilters(
        category, minPrice, maxPrice, inStock
    );
    
    return ResponseEntity.ok(products);
}
// GET /api/products?category=electronics&minPrice=100&maxPrice=500
```

**Q72 : What are JPA Specifications and when should you use them?**
**A :** JPA Specifications allows dynamically building queries with conditional criteria. Useful for complex and combinable filters :
```java
public class ProductSpecifications {
    public static Specification<Product> hasCategory(String category) {
        return (root, query, cb) -> 
            category == null ? null : cb.equal(root.get("category"), category);
    }
    
    public static Specification<Product> priceBetween(Double min, Double max) {
        return (root, query, cb) -> {
            if (min != null && max != null) {
                return cb.between(root.get("price"), min, max);
            }
            return null;
        };
    }
}

// Usage
Specification<Product> spec = Specification
    .where(ProductSpecifications.hasCategory(category))
    .and(ProductSpecifications.priceBetween(minPrice, maxPrice));

Page<Product> products = productRepository.findAll(spec, pageable);
```

**Q73 : How do you implement a text search?**
**A :**
```java
@GetMapping("/search")
public ResponseEntity<List<User>> searchUsers(
        @RequestParam String query) {
    List<User> users = userService.search(query);
    return ResponseEntity.ok(users);
}

// In the repository
@Query("SELECT u FROM User u WHERE " +
       "LOWER(u.name) LIKE LOWER(CONCAT('%', :query, '%')) OR " +
       "LOWER(u.email) LIKE LOWER(CONCAT('%', :query, '%'))")
List<User> search(@Param("query") String query);
```

**Q74 : How do you combine filtering, search and pagination?**
**A :**
```java
@GetMapping("/products")
public ResponseEntity<Page<Product>> getProducts(
        @RequestParam(required = false) String category,
        @RequestParam(required = false) String search,
        @RequestParam(required = false) Double minPrice,
        @RequestParam(required = false) Double maxPrice,
        Pageable pageable) {
    
    Specification<Product> spec = Specification
        .where(ProductSpecifications.hasCategory(category))
        .and(ProductSpecifications.nameContains(search))
        .and(ProductSpecifications.priceBetween(minPrice, maxPrice));
    
    Page<Product> products = productRepository.findAll(spec, pageable);
    return ResponseEntity.ok(products);
}
// GET /api/products?category=electronics&search=laptop&minPrice=500&page=0&size=20&sort=price,asc
```

**Q75 : How do you create an advanced search endpoint with a criteria object?**
**A :**
```java
@PostMapping("/search")
public ResponseEntity<Page<Product>> advancedSearch(
        @RequestBody ProductSearchCriteria criteria,
        Pageable pageable) {
    
    Page<Product> products = productService.search(criteria, pageable);
    return ResponseEntity.ok(products);
}

// DTO
@Data
public class ProductSearchCriteria {
    private String name;
    private List<String> categories;
    private Double minPrice;
    private Double maxPrice;
    private Boolean inStock;
    private LocalDate startDate;
    private LocalDate endDate;
}
```

## 🌐 CORS

**Q76 : What is CORS and why is it needed?**
**A :** CORS (Cross-Origin Resource Sharing) is a browser security mechanism. By default, a website cannot make requests to a different origin (different domain, port, or protocol). CORS explicitly allows these cross-origin requests.

**Q77 : How do you enable CORS for a specific controller?**
**A :**
```java
@RestController
@RequestMapping("/api/users")
@CrossOrigin(origins = "http://localhost:3000")
public class UserController {
    // All endpoints accept requests from localhost:3000
}
```

**Q78 : How do you enable CORS for a specific endpoint?**
**A :**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @CrossOrigin(origins = {"http://localhost:3000", "https://myapp.com"})
    @GetMapping
    public List<User> getUsers() {
        return userService.findAll();
    }
}
```

**Q79 : How do you configure CORS globally?**
**A :**
```java
@Configuration
public class WebConfig implements WebMvcConfigurer {
    
    @Override
    public void addCorsMappings(CorsRegistry registry) {
        registry.addMapping("/api/**")
            .allowedOrigins("http://localhost:3000", "https://myapp.com")
            .allowedMethods("GET", "POST", "PUT", "DELETE", "PATCH")
            .allowedHeaders("*")
            .allowCredentials(true)
            .maxAge(3600);
    }
}
```

**Q80 : What is the difference between allowedOrigins and allowedOriginPatterns?**
**A :**
- **allowedOrigins** : Exact URLs (e.g., "http://localhost:3000")
- **allowedOriginPatterns** : Patterns with wildcards (e.g., "http://*.example.com")

**Q81 : What does allowCredentials(true) mean in CORS?**
**A :** `allowCredentials(true)` allows cookies and authentication headers to be sent in cross-origin requests. Required for sessions and JWT in cookies.

**Q82 : Why does CORS only apply in the browser?**
**A :** CORS is a browser protection mechanism. Tools like Postman, cURL, or backends are not affected. Only web browsers enforce the CORS policy.

## 📋 Content Negotiation

**Q83 : What is Content Negotiation?**
**A :** Content Negotiation allows the server to return different formats (JSON, XML, CSV) based on what the client requests via the `Accept` header.

**Q84 : How do you specify the response format with produces?**
**A :**
```java
// JSON only
@GetMapping(produces = MediaType.APPLICATION_JSON_VALUE)
public List<User> getUsersJson() {
    return userService.findAll();
}

// XML only
@GetMapping(produces = MediaType.APPLICATION_XML_VALUE)
public List<User> getUsersXml() {
    return userService.findAll();
}

// JSON or XML based on Accept header
@GetMapping(produces = {
    MediaType.APPLICATION_JSON_VALUE,
    MediaType.APPLICATION_XML_VALUE
})
public List<User> getUsers() {
    return userService.findAll();
}
```

**Q85 : How do you restrict the accepted format with consumes?**
**A :**
```java
@PostMapping(
    consumes = MediaType.APPLICATION_JSON_VALUE,
    produces = MediaType.APPLICATION_JSON_VALUE
)
public ResponseEntity<User> createUser(@RequestBody User user) {
    return ResponseEntity.status(HttpStatus.CREATED)
        .body(userService.save(user));
}
// Accepts only JSON, rejects XML or other formats
```

**Q86 : How does the client specify the desired format?**
**A :** Via the `Accept` header :
```bash
# Requests JSON
GET /api/users
Accept: application/json

# Requests XML
GET /api/users
Accept: application/xml

# Requests CSV
GET /api/users
Accept: text/csv
```

**Q87 : How do you handle multipart/form-data uploads?**
**A :**
```java
@PostMapping(
    value = "/upload",
    consumes = MediaType.MULTIPART_FORM_DATA_VALUE
)
public ResponseEntity<String> uploadFile(
        @RequestParam("file") MultipartFile file,
        @RequestParam("description") String description) {
    
    String filename = storageService.store(file);
    return ResponseEntity.ok("Uploaded: " + filename);
}
```

## 🔒 HTTP Headers

**Q88 : How do you retrieve a specific header?**
**A :**
```java
@GetMapping("/users")
public List<User> getUsers(
        @RequestHeader("X-API-Key") String apiKey) {
    if (!isValidApiKey(apiKey)) {
        throw new UnauthorizedException();
    }
    return userService.findAll();
}
```

**Q89 : How do you make a header optional?**
**A :**
```java
@GetMapping("/users")
public List<User> getUsers(
        @RequestHeader(value = "User-Agent", required = false) 
        String userAgent) {
    log.info("Request from: " + userAgent);
    return userService.findAll();
}
```

**Q90 : How do you retrieve all headers?**
**A :**
```java
@GetMapping("/debug")
public Map<String, String> debugHeaders(
        @RequestHeader Map<String, String> headers) {
    return headers;
}

// Or with HttpHeaders
@GetMapping("/debug")
public HttpHeaders debugHeaders(@RequestHeader HttpHeaders headers) {
    return headers;
}
```

**Q91 : How do you add a custom header to the response?**
**A :**
```java
@GetMapping("/data")
public ResponseEntity<String> getData() {
    return ResponseEntity.ok()
        .header("X-Custom-Header", "my-value")
        .header("X-Request-ID", UUID.randomUUID().toString())
        .body("Data");
}
```

**Q92 : How do you handle cookies?**
**A :**
```java
// Read a cookie
@GetMapping("/profile")
public User getProfile(@CookieValue("sessionId") String sessionId) {
    return userService.findBySession(sessionId);
}

// Set a cookie in the response
@GetMapping("/login")
public ResponseEntity<String> login() {
    ResponseCookie cookie = ResponseCookie.from("sessionId", "abc123")
        .httpOnly(true)
        .secure(true)
        .path("/")
        .maxAge(Duration.ofDays(7))
        .build();
    
    return ResponseEntity.ok()
        .header(HttpHeaders.SET_COOKIE, cookie.toString())
        .body("Logged in");
}
```

## 🎯 REST Best Practices

**Q93 : What are the REST naming conventions?**
**A :**
- Use nouns (not verbs) : `/users` not `/getUsers`
- Use plural : `/users` not `/user`
- Use hierarchies : `/users/{userId}/orders/{orderId}`
- Lowercase and hyphens : `/user-profiles` not `/UserProfiles`
- No trailing slash : `/users` not `/users/`

**Q94 : Which HTTP methods should you use for which operations?**
**A :**
- **GET** : Retrieve (read, idempotent)
- **POST** : Create a resource
- **PUT** : Fully replace a resource (idempotent)
- **PATCH** : Partial update
- **DELETE** : Delete (idempotent)

**Q95 : What is idempotence and why does it matter?**
**A :** An operation is idempotent if executing it multiple times produces the same result as executing it once. GET, PUT, DELETE are idempotent. POST is not (creating twice = 2 resources). Important for reliability (retry safe).

**Q96 : How do you structure URLs for relationships?**
**A :**
```java
// Nested resources
GET    /users/{userId}/orders           // List orders for a user
GET    /users/{userId}/orders/{orderId} // A specific order
POST   /users/{userId}/orders           // Create an order for a user
DELETE /users/{userId}/orders/{orderId} // Delete an order

// Or independent resources with filtering
GET /orders?userId=5
```

**Q97 : How do you handle API versioning?**
**A :**
```java
// Via URL (recommended)
@RequestMapping("/api/v1/users")
@RequestMapping("/api/v2/users")

// Via header
@GetMapping(value = "/users", headers = "X-API-Version=1")
@GetMapping(value = "/users", headers = "X-API-Version=2")

// Via Accept header
@GetMapping(value = "/users", produces = "application/vnd.myapp.v1+json")
@GetMapping(value = "/users", produces = "application/vnd.myapp.v2+json")
```

**Q98 : How do you document a REST API?**
**A :** Use **Swagger/OpenAPI** (SpringDoc) :
```xml
<dependency>
    <groupId>org.springdoc</groupId>
    <artifactId>springdoc-openapi-starter-webmvc-ui</artifactId>
    <version>2.2.0</version>
</dependency>
```
Auto-generated documentation available at `/swagger-ui.html`

**Q99 : What information should you include in an error response?**
**A :**
```java
@Data
@AllArgsConstructor
public class ErrorResponse {
    private int status;           // 404
    private String error;         // "Not Found"
    private String message;       // "User with id 5 not found"
    private String path;          // "/api/users/5"
    private LocalDateTime timestamp;
    private List<String> details; // Validation errors
}
```

**Q100 : How do you implement HATEOAS (hypermedia links)?**
**A :** HATEOAS adds links to responses to guide the client :
```java
@GetMapping("/{id}")
public EntityModel<User> getUser(@PathVariable Long id) {
    User user = userService.findById(id);
    
    return EntityModel.of(user,
        linkTo(methodOn(UserController.class).getUser(id)).withSelfRel(),
        linkTo(methodOn(UserController.class).getAllUsers()).withRel("users"),
        linkTo(methodOn(OrderController.class).getUserOrders(id)).withRel("orders")
    );
}

// JSON response:
{
  "id": 1,
  "name": "John",
  "_links": {
    "self": {"href": "/api/users/1"},
    "users": {"href": "/api/users"},
    "orders": {"href": "/api/users/1/orders"}
  }
}
```

## 🔧 Configuration and Optimization

**Q101 : How do you configure the maximum upload size?**
**A :**
```properties
# application.properties
spring.servlet.multipart.max-file-size=10MB
spring.servlet.multipart.max-request-size=50MB
```

**Q102 : How do you enable response compression?**
**A :**
```properties
server.compression.enabled=true
server.compression.mime-types=application/json,application/xml,text/html,text/plain
server.compression.min-response-size=1024
```

**Q103 : How do you handle timeouts?**
**A :**
```properties
# Connection timeout
spring.mvc.async.request-timeout=30000

# Tomcat timeout
server.tomcat.connection-timeout=20000
```

**Q104 : How do you enable HTTP caching?**
**A :**
```java
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    User user = userService.findById(id);
    
    return ResponseEntity.ok()
        .cacheControl(CacheControl.maxAge(1, TimeUnit.HOURS))
        .eTag("\"user-" + user.getVersion() + "\"")
        .lastModified(user.getUpdatedAt())
        .body(user);
}
```

**Q105 : How do you handle asynchronous requests?**
**A :**
```java
@GetMapping("/async")
public CompletableFuture<List<User>> getUsersAsync() {
    return CompletableFuture.supplyAsync(() -> userService.findAll());
}

// Or with DeferredResult
@GetMapping("/deferred")
public DeferredResult<List<User>> getUsersDeferred() {
    DeferredResult<List<User>> result = new DeferredResult<>(5000L);
    
    CompletableFuture.supplyAsync(() -> userService.findAll())
        .whenComplete((users, ex) -> {
            if (ex != null) {
                result.setErrorResult(ex);
            } else {
                result.setResult(users);
            }
        });
    
    return result;
}
```

## 📊 Complete Summary Example

**Q106 : Create a complete CRUD controller with all best practices**
**A :**
```java
@RestController
@RequestMapping("/api/v1/products")
@Validated
@CrossOrigin(origins = "${app.cors.allowed-origins}")
@Tag(name = "Products", description = "Product management API")
public class ProductController {
    
    @Autowired
    private ProductService productService;
    
    // GET - List with filtering and pagination
    @GetMapping
    @Operation(summary = "Get all products", description = "Returns paginated list of products")
    public ResponseEntity<Page<ProductDTO>> getAllProducts(
            @RequestParam(required = false) String category,
            @RequestParam(required = false) Double minPrice,
            @RequestParam(required = false) Double maxPrice,
            @RequestParam(required = false) String search,
            @PageableDefault(size = 20, sort = "id") Pageable pageable) {
        
        Page<ProductDTO> products = productService.findAll(
            category, minPrice, maxPrice, search, pageable
        );
        
        return ResponseEntity.ok()
            .cacheControl(CacheControl.maxAge(5, TimeUnit.MINUTES))
            .body(products);
    }
    
    // GET - By ID
    @GetMapping("/{id}")
    @Operation(summary = "Get product by ID")
    public ResponseEntity<ProductDTO> getProduct(@PathVariable Long id) {
        return productService.findById(id)
            .map(product -> ResponseEntity.ok()
                .eTag("\"product-" + product.getVersion() + "\"")
                .body(product))
            .orElse(ResponseEntity.notFound().build());
    }
    
    // POST - Create
    @PreAuthorize("hasRole('ADMIN')")
    @PostMapping
    @Operation(summary = "Create new product")
    public ResponseEntity<ProductDTO> createProduct(
            @RequestBody @Valid CreateProductRequest request) {
        
        ProductDTO created = productService.create(request);
        
        URI location = ServletUriComponentsBuilder
            .fromCurrentRequest()
            .path("/{id}")
            .buildAndExpand(created.getId())
            .toUri();
        
        return ResponseEntity.created(location).body(created);
    }
    
    // PUT - Update
    @PreAuthorize("hasRole('ADMIN')")
    @PutMapping("/{id}")
    @Operation(summary = "Update product")
    public ResponseEntity<ProductDTO> updateProduct(
            @PathVariable Long id,
            @RequestBody @Valid UpdateProductRequest request) {
        
        return productService.update(id, request)
            .map(ResponseEntity::ok)
            .orElse(ResponseEntity.notFound().build());
    }
    
    // PATCH - Partial update
    @PreAuthorize("hasRole('ADMIN')")
    @PatchMapping("/{id}")
    @Operation(summary = "Partially update product")
    public ResponseEntity<ProductDTO> partialUpdate(
            @PathVariable Long id,
            @RequestBody Map<String, Object> updates) {
        
        return productService.partialUpdate(id, updates)
            .map(ResponseEntity::ok)
            .orElse(ResponseEntity.notFound().build());
    }
    
    // DELETE
    @PreAuthorize("hasRole('ADMIN')")
    @DeleteMapping("/{id}")
    @Operation(summary = "Delete product")
    public ResponseEntity<Void> deleteProduct(@PathVariable Long id) {
        boolean deleted = productService.delete(id);
        return deleted ? 
            ResponseEntity.noContent().build() : 
            ResponseEntity.notFound().build();
    }
    
    // Local error handling
    @ExceptionHandler(ProductNotFoundException.class)
    public ResponseEntity<ErrorResponse> handleNotFound(
            ProductNotFoundException ex) {
        ErrorResponse error = new ErrorResponse(
            HttpStatus.NOT_FOUND.value(),
            ex.getMessage(),
            LocalDateTime.now()
        );
        return ResponseEntity.status(HttpStatus.NOT_FOUND).body(error);
    }
}
```

---
