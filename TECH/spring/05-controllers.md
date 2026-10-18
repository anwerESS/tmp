
## [**..**](./00_index.md)

# REST Controllers & Endpoints Spring Boot

## 🎮 Controllers - Architecture MVC

### Concept des Controllers

Un **Controller** dans Spring MVC est le composant qui gère les requêtes HTTP entrantes, traite la logique de présentation, et retourne une réponse.

**Architecture typique :**
```
Client (Browser/Postman)
    ↓ HTTP Request
Controller Layer (@RestController)
    ↓ Appel méthodes
Service Layer (@Service)
    ↓ Logique métier
Repository Layer (@Repository)
    ↓ Accès données
Database
```

### @Controller vs @RestController

#### @Controller (Applications Web traditionnelles)
```java
@Controller
public class UserWebController {
    
    @GetMapping("/users")
    public String listUsers(Model model) {
        model.addAttribute("users", userService.findAll());
        return "users"; // Retourne le nom d'une vue (users.jsp, users.html)
    }
}
```
**Usage :** Applications web avec vues (JSP, Thymeleaf)
**Retour :** Nom de la vue à rendre

#### @RestController (REST APIs) ⭐
```java
@RestController
public class UserController {
    
    @GetMapping("/api/users")
    public List<User> listUsers() {
        return userService.findAll(); // Retourne directement JSON
    }
}
```
**Usage :** REST APIs (retour JSON/XML)
**Équivalent à :** `@Controller + @ResponseBody` sur chaque méthode

**@RestController = @Controller + @ResponseBody**

### Structure Complète d'un Controller REST

```java
@RestController
@RequestMapping("/api/users")
@CrossOrigin(origins = "http://localhost:3000") // CORS
@Validated
public class UserController {
    
    @Autowired
    private UserService userService;
    
    // GET - Liste tous les utilisateurs
    @GetMapping
    public ResponseEntity<List<User>> getAllUsers() {
        List<User> users = userService.findAll();
        return ResponseEntity.ok(users);
    }
    
    // GET - Un utilisateur par ID
    @GetMapping("/{id}")
    public ResponseEntity<User> getUserById(@PathVariable Long id) {
        User user = userService.findById(id);
        return ResponseEntity.ok(user);
    }
    
    // POST - Créer un utilisateur
    @PostMapping
    public ResponseEntity<User> createUser(@RequestBody @Valid User user) {
        User created = userService.save(user);
        return ResponseEntity
            .status(HttpStatus.CREATED)
            .body(created);
    }
    
    // PUT - Mettre à jour un utilisateur
    @PutMapping("/{id}")
    public ResponseEntity<User> updateUser(
            @PathVariable Long id,
            @RequestBody @Valid User user) {
        User updated = userService.update(id, user);
        return ResponseEntity.ok(updated);
    }
    
    // DELETE - Supprimer un utilisateur
    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
        userService.delete(id);
        return ResponseEntity.noContent().build();
    }
    
    // PATCH - Mise à jour partielle
    @PatchMapping("/{id}")
    public ResponseEntity<User> partialUpdate(
            @PathVariable Long id,
            @RequestBody Map<String, Object> updates) {
        User updated = userService.partialUpdate(id, updates);
        return ResponseEntity.ok(updated);
    }
}
```

## 🛣️ Endpoints Mapping - Annotations de Routing

### @RequestMapping (Générique)

**Mapping de base :**
```java
@RestController
@RequestMapping("/api") // Préfixe pour tous les endpoints
public class ProductController {
    
    @RequestMapping(value = "/products", method = RequestMethod.GET)
    public List<Product> getProducts() {
        return productService.findAll();
    }
}
```

**Avec multiples méthodes HTTP :**
```java
@RequestMapping(value = "/products/{id}", method = {RequestMethod.GET, RequestMethod.HEAD})
public Product getProduct(@PathVariable Long id) {
    return productService.findById(id);
}
```

**Avec conditions :**
```java
@RequestMapping(
    value = "/products",
    method = RequestMethod.GET,
    params = "active=true",           // Requiert ?active=true
    headers = "X-API-Version=1",      // Requiert header
    consumes = "application/json",    // Accepte JSON
    produces = "application/json"     // Produit JSON
)
public List<Product> getActiveProducts() {
    return productService.findActive();
}
```

### Annotations Spécialisées (Recommandées)

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

#### @PutMapping (Remplacement complet)
```java
@PutMapping("/users/{id}")
public ResponseEntity<User> updateUser(
        @PathVariable Long id,
        @RequestBody User user) {
    User updated = userService.update(id, user);
    return ResponseEntity.ok(updated);
}
```

#### @PatchMapping (Mise à jour partielle)
```java
@PatchMapping("/users/{id}")
public ResponseEntity<User> partialUpdateUser(
        @PathVariable Long id,
        @RequestBody Map<String, Object> updates) {
    // Updates ne contient que les champs modifiés
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

### Patterns d'URL Avancés

```java
// Variables multiples
@GetMapping("/users/{userId}/orders/{orderId}")
public Order getUserOrder(
        @PathVariable Long userId,
        @PathVariable Long orderId) {
    return orderService.findByUserAndId(userId, orderId);
}

// Variables optionnelles avec regex
@GetMapping("/files/{filename:.+}") // .+ capture l'extension
public ResponseEntity<Resource> downloadFile(@PathVariable String filename) {
    Resource file = storageService.loadAsResource(filename);
    return ResponseEntity.ok()
        .header(HttpHeaders.CONTENT_DISPOSITION, 
                "attachment; filename=\"" + file.getFilename() + "\"")
        .body(file);
}

// Wildcards
@GetMapping("/api/**") // Match tous les sous-chemins
public String handleAll() {
    return "Catch-all handler";
}
```

## 📥 Annotations de Paramètres

### @PathVariable (Variables d'URL)

```java
// Basique
@GetMapping("/users/{id}")
public User getUser(@PathVariable Long id) {
    return userService.findById(id);
}

// Nom différent de la variable
@GetMapping("/users/{userId}")
public User getUser(@PathVariable("userId") Long id) {
    return userService.findById(id);
}

// Multiples variables
@GetMapping("/users/{userId}/posts/{postId}")
public Post getUserPost(
        @PathVariable Long userId,
        @PathVariable Long postId) {
    return postService.findByUserAndId(userId, postId);
}

// Optionnel (Spring 4.3+)
@GetMapping({"/users", "/users/{id}"})
public ResponseEntity<?> getUsers(@PathVariable(required = false) Long id) {
    if (id == null) {
        return ResponseEntity.ok(userService.findAll());
    }
    return ResponseEntity.ok(userService.findById(id));
}

// Map de toutes les variables
@GetMapping("/data/{var1}/{var2}")
public String getData(@PathVariable Map<String, String> pathVars) {
    return "var1: " + pathVars.get("var1") + 
           ", var2: " + pathVars.get("var2");
}
```

### @RequestParam (Query Parameters)

```java
// Basique: GET /api/users?name=John
@GetMapping("/users")
public List<User> findUsers(@RequestParam String name) {
    return userService.findByName(name);
}

// Optionnel avec valeur par défaut
@GetMapping("/users")
public List<User> findUsers(
        @RequestParam(required = false, defaultValue = "0") int page,
        @RequestParam(defaultValue = "10") int size) {
    return userService.findAll(page, size);
}

// Nom différent
@GetMapping("/users")
public List<User> findUsers(
        @RequestParam(name = "q") String searchQuery) {
    return userService.search(searchQuery);
}

// Multiples valeurs: GET /api/users?id=1&id=2&id=3
@GetMapping("/users")
public List<User> findUsers(@RequestParam List<Long> id) {
    return userService.findByIds(id);
}

// Map de tous les paramètres
@GetMapping("/search")
public List<User> search(@RequestParam Map<String, String> allParams) {
    // allParams contient tous les query params
    return userService.search(allParams);
}

// Filtrage et pagination
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

### @RequestBody (Corps de la requête)

```java
// JSON → Object
@PostMapping("/users")
public ResponseEntity<User> createUser(@RequestBody User user) {
    User created = userService.save(user);
    return ResponseEntity.status(HttpStatus.CREATED).body(created);
}

// Avec validation
@PostMapping("/users")
public ResponseEntity<User> createUser(@RequestBody @Valid User user) {
    // @Valid déclenche la validation Bean Validation (JSR-303)
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

// Map pour structure flexible
@PostMapping("/data")
public ResponseEntity<String> processData(
        @RequestBody Map<String, Object> data) {
    // data contient la structure JSON complète
    return ResponseEntity.ok("Processed");
}
```

**Exemple de requête avec @RequestBody :**
```bash
POST /api/users
Content-Type: application/json

{
  "name": "John Doe",
  "email": "john@example.com",
  "age": 30
}
```

### @RequestHeader (En-têtes HTTP)

```java
// Header spécifique
@GetMapping("/users")
public List<User> getUsers(
        @RequestHeader("X-API-Key") String apiKey) {
    if (!isValidApiKey(apiKey)) {
        throw new UnauthorizedException();
    }
    return userService.findAll();
}

// Header optionnel
@GetMapping("/users")
public List<User> getUsers(
        @RequestHeader(value = "User-Agent", required = false) String userAgent) {
    log.info("Request from: " + userAgent);
    return userService.findAll();
}

// Tous les headers
@GetMapping("/debug")
public Map<String, String> debugHeaders(@RequestHeader Map<String, String> headers) {
    return headers;
}
```

### Autres Annotations de Paramètres

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

#### @MatrixVariable (Paramètres de matrice)
```java
// URL: /products/category=electronics;brand=apple
@GetMapping("/products/{filters}")
public List<Product> getProducts(
        @MatrixVariable String category,
        @MatrixVariable String brand) {
    return productService.findByCategoryAndBrand(category, brand);
}
```

## 📨 ResponseEntity<> - Contrôle Total de la Réponse

### Concept de ResponseEntity

`ResponseEntity<T>` est un wrapper qui permet de contrôler complètement la réponse HTTP : status code, headers, et body.

### Construction de ResponseEntity

#### Méthodes de Factory (Recommandées)

```java
// 200 OK
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    User user = userService.findById(id);
    return ResponseEntity.ok(user);
    // Équivalent à: new ResponseEntity<>(user, HttpStatus.OK)
}

// 201 CREATED avec Location header
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

#### Builder Pattern (Contrôle complet)

```java
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    User user = userService.findById(id);
    
    return ResponseEntity
        .status(HttpStatus.OK)                    // Status code
        .header("X-Custom-Header", "value")       // Headers personnalisés
        .header(HttpHeaders.CACHE_CONTROL, "max-age=3600")
        .eTag("\"user-" + user.getId() + "\"")    // ETag pour cache
        .lastModified(user.getUpdatedAt())        // Last-Modified
        .body(user);                              // Corps de réponse
}

// Téléchargement de fichier
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

#### Avec Optional (Pattern élégant)

```java
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    return userService.findById(id)
        .map(user -> ResponseEntity.ok(user))
        .orElse(ResponseEntity.notFound().build());
}

// Avec transformation
@GetMapping("/users/{id}")
public ResponseEntity<UserDTO> getUser(@PathVariable Long id) {
    return userService.findById(id)
        .map(user -> new UserDTO(user))
        .map(ResponseEntity::ok)
        .orElse(ResponseEntity.notFound().build());
}
```

### ResponseEntity sans Body

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

## 🚦 HTTP Status Codes - Codes de Réponse

### 2xx Success (Succès)

```java
// 200 OK - Requête réussie
@GetMapping("/users")
public ResponseEntity<List<User>> getUsers() {
    return ResponseEntity.ok(userService.findAll());
}

// 201 CREATED - Ressource créée
@PostMapping("/users")
public ResponseEntity<User> createUser(@RequestBody User user) {
    User created = userService.save(user);
    return ResponseEntity.status(HttpStatus.CREATED).body(created);
}

// 202 ACCEPTED - Requête acceptée, traitement asynchrone
@PostMapping("/jobs")
public ResponseEntity<String> submitJob(@RequestBody Job job) {
    jobService.submitAsync(job);
    return ResponseEntity.accepted().body("Job submitted");
}

// 204 NO CONTENT - Succès sans contenu à retourner
@DeleteMapping("/users/{id}")
public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    userService.delete(id);
    return ResponseEntity.noContent().build();
}
```

### 3xx Redirection

```java
// 301 MOVED PERMANENTLY - Ressource déplacée définitivement
@GetMapping("/old-url")
public ResponseEntity<Void> oldEndpoint() {
    return ResponseEntity
        .status(HttpStatus.MOVED_PERMANENTLY)
        .location(URI.create("/new-url"))
        .build();
}

// 302 FOUND - Redirection temporaire
@GetMapping("/temporary")
public ResponseEntity<Void> temporaryRedirect() {
    return ResponseEntity
        .status(HttpStatus.FOUND)
        .location(URI.create("/actual-location"))
        .build();
}

// 304 NOT MODIFIED - Cache valide
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

### 4xx Client Errors (Erreurs client)

```java
// 400 BAD REQUEST - Requête invalide
@PostMapping("/users")
public ResponseEntity<String> createUser(@RequestBody User user) {
    if (user.getEmail() == null) {
        return ResponseEntity
            .badRequest()
            .body("Email is required");
    }
    return ResponseEntity.ok("Created");
}

// 401 UNAUTHORIZED - Non authentifié
@GetMapping("/profile")
public ResponseEntity<User> getProfile() {
    if (!securityService.isAuthenticated()) {
        return ResponseEntity.status(HttpStatus.UNAUTHORIZED).build();
    }
    return ResponseEntity.ok(userService.getCurrentUser());
}

// 403 FORBIDDEN - Authentifié mais pas autorisé
@DeleteMapping("/users/{id}")
public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    if (!securityService.hasRole("ADMIN")) {
        return ResponseEntity.status(HttpStatus.FORBIDDEN).build();
    }
    userService.delete(id);
    return ResponseEntity.noContent().build();
}

// 404 NOT FOUND - Ressource introuvable
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    return userService.findById(id)
        .map(ResponseEntity::ok)
        .orElse(ResponseEntity.notFound().build());
}

// 409 CONFLICT - Conflit (ex: email déjà existant)
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

// 422 UNPROCESSABLE ENTITY - Validation échouée
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

### 5xx Server Errors (Erreurs serveur)

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

// 503 SERVICE UNAVAILABLE - Service temporairement indisponible
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

### Gestion Globale des Erreurs

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

// Classe ErrorResponse
@Data
@AllArgsConstructor
public class ErrorResponse {
    private int status;
    private String message;
    private LocalDateTime timestamp;
}
```

## 🔐 @PreAuthorize - Sécurité au Niveau Méthode

### Configuration de Base

```java
@Configuration
@EnableGlobalMethodSecurity(prePostEnabled = true)
public class SecurityConfig {
    // Configuration Spring Security
}
```

### Expressions @PreAuthorize

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    // Accès seulement si authentifié
    @PreAuthorize("isAuthenticated()")
    @GetMapping("/profile")
    public User getProfile() {
        return userService.getCurrentUser();
    }
    
    // Accès seulement avec rôle ADMIN
    @PreAuthorize("hasRole('ADMIN')")
    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
        userService.delete(id);
        return ResponseEntity.noContent().build();
    }
    
    // Accès avec un des rôles spécifiés
    @PreAuthorize("hasAnyRole('ADMIN', 'MANAGER')")
    @PutMapping("/{id}")
    public ResponseEntity<User> updateUser(
            @PathVariable Long id,
            @RequestBody User user) {
        return ResponseEntity.ok(userService.update(id, user));
    }
    
    // Accès avec authority spécifique
    @PreAuthorize("hasAuthority('USER_WRITE')")
    @PostMapping
    public ResponseEntity<User> createUser(@RequestBody User user) {
        return ResponseEntity.status(HttpStatus.CREATED)
            .body(userService.save(user));
    }
    
    // Vérification de propriété (l'utilisateur peut modifier ses propres données)
    @PreAuthorize("#id == authentication.principal.id")
    @PutMapping("/{id}/profile")
    public ResponseEntity<User> updateOwnProfile(
            @PathVariable Long id,
            @RequestBody User user) {
        return ResponseEntity.ok(userService.update(id, user));
    }
    
    // Expression complexe avec OR
    @PreAuthorize("hasRole('ADMIN') or #id == authentication.principal.id")
    @GetMapping("/{id}")
    public ResponseEntity<User> getUser(@PathVariable Long id) {
        return userService.findById(id)
            .map(ResponseEntity::ok)
            .orElse(ResponseEntity.notFound().build());
    }
    
    // Expression avec méthode personnalisée
    @PreAuthorize("@userSecurity.canAccessUser(#id)")
    @GetMapping("/{id}/details")
    public ResponseEntity<UserDetails> getUserDetails(@PathVariable Long id) {
        return ResponseEntity.ok(userService.getDetails(id));
    }
}

// Service de sécurité personnalisé
@Service("userSecurity")
public class UserSecurityService {
    public boolean canAccessUser(Long userId) {
        Authentication auth = SecurityContextHolder.getContext().getAuthentication();
        User currentUser = (User) auth.getPrincipal();
        return currentUser.isAdmin() || currentUser.getId().equals(userId);
    }
}
```

### @PostAuthorize (Contrôle après exécution)

```java
// Filtre le résultat après exécution
@PostAuthorize("returnObject.owner == authentication.principal.username")
@GetMapping("/documents/{id}")
public Document getDocument(@PathVariable Long id) {
    return documentService.findById(id);
}
```

### @Secured (Alternative simple)

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

## 📋 Validation avec Bean Validation

### Annotations de Validation

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

### Utilisation dans le Controller

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

### Gestion Automatique des Erreurs


```java
// Gestion automatique avec @RestControllerAdvice
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

### Validation de Groupe

```java
// Interfaces de groupes
public interface CreateValidation {}
public interface UpdateValidation {}

// Entité avec groupes
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

// Controller avec groupes
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

### Validation Personnalisée

```java
// Annotation personnalisée
@Target({ElementType.FIELD})
@Retention(RetentionPolicy.RUNTIME)
@Constraint(validatedBy = UniqueEmailValidator.class)
public @interface UniqueEmail {
    String message() default "Email already exists";
    Class<?>[] groups() default {};
    Class<? extends Payload>[] payload() default {};
}

// Validateur
@Component
public class UniqueEmailValidator implements ConstraintValidator<UniqueEmail, String> {
    
    @Autowired
    private UserRepository userRepository;
    
    @Override
    public boolean isValid(String email, ConstraintValidatorContext context) {
        if (email == null) {
            return true; // @NotNull gère ce cas
        }
        return !userRepository.existsByEmail(email);
    }
}

// Utilisation
@Data
public class CreateUserRequest {
    @NotBlank
    @Email
    @UniqueEmail
    private String email;
}
```

## 🌐 Content Negotiation - Formats de Réponse

### Produces et Consumes

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    // Produit JSON par défaut
    @GetMapping(produces = MediaType.APPLICATION_JSON_VALUE)
    public List<User> getUsersJson() {
        return userService.findAll();
    }
    
    // Produit XML
    @GetMapping(produces = MediaType.APPLICATION_XML_VALUE)
    public List<User> getUsersXml() {
        return userService.findAll();
    }
    
    // Multiples formats
    @GetMapping(produces = {
        MediaType.APPLICATION_JSON_VALUE,
        MediaType.APPLICATION_XML_VALUE
    })
    public List<User> getUsers() {
        return userService.findAll();
    }
    
    // Accepte seulement JSON
    @PostMapping(
        consumes = MediaType.APPLICATION_JSON_VALUE,
        produces = MediaType.APPLICATION_JSON_VALUE
    )
    public ResponseEntity<User> createUser(@RequestBody User user) {
        return ResponseEntity.status(HttpStatus.CREATED).body(userService.save(user));
    }
    
    // Upload de fichier
    @PostMapping(
        value = "/upload",
        consumes = MediaType.MULTIPART_FORM_DATA_VALUE
    )
    public ResponseEntity<String> uploadFile(
            @RequestParam("file") MultipartFile file) {
        String filename = storageService.store(file);
        return ResponseEntity.ok("File uploaded: " + filename);
    }
    
    // Téléchargement de fichier
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

### Content Negotiation par Accept Header

```bash
# Client demande JSON
GET /api/users
Accept: application/json

# Client demande XML
GET /api/users
Accept: application/xml

# Client demande CSV
GET /api/users
Accept: text/csv
```

## 📤 Upload et Download de Fichiers

### Upload Simple

```java
@RestController
@RequestMapping("/api/files")
public class FileController {
    
    @Autowired
    private StorageService storageService;
    
    // Upload un fichier
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
    
    // Upload multiple
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
    
    // Upload avec métadonnées
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

### Download de Fichiers

```java
@RestController
@RequestMapping("/api/files")
public class FileController {
    
    // Téléchargement simple
    @GetMapping("/download/{filename:.+}")
    public ResponseEntity<Resource> downloadFile(@PathVariable String filename) {
        
        Resource file = storageService.loadAsResource(filename);
        
        return ResponseEntity.ok()
            .contentType(MediaType.APPLICATION_OCTET_STREAM)
            .header(HttpHeaders.CONTENT_DISPOSITION, 
                    "attachment; filename=\"" + file.getFilename() + "\"")
            .body(file);
    }
    
    // Téléchargement avec type MIME détecté
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
    
    // Streaming de fichier volumineux
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
    
    // Export CSV
    @GetMapping("/export/users.csv")
    public ResponseEntity<String> exportUsersCsv() {
        String csv = userService.exportToCsv();
        
        return ResponseEntity.ok()
            .contentType(MediaType.parseMediaType("text/csv"))
            .header(HttpHeaders.CONTENT_DISPOSITION, 
                    "attachment; filename=\"users.csv\"")
            .body(csv);
    }
    
    // Export Excel
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

## 🔄 Pagination et Tri

### Avec Spring Data

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @Autowired
    private UserRepository userRepository;
    
    // Pagination basique
    @GetMapping
    public ResponseEntity<Page<User>> getUsers(
            @RequestParam(defaultValue = "0") int page,
            @RequestParam(defaultValue = "10") int size) {
        
        Pageable pageable = PageRequest.of(page, size);
        Page<User> users = userRepository.findAll(pageable);
        
        return ResponseEntity.ok(users);
    }
    
    // Pagination avec tri
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
    
    // Avec Pageable directement
    @GetMapping("/pageable")
    public ResponseEntity<Page<User>> getUsers(Pageable pageable) {
        // Spring gère automatiquement les paramètres:
        // ?page=0&size=10&sort=name,asc&sort=email,desc
        Page<User> users = userRepository.findAll(pageable);
        return ResponseEntity.ok(users);
    }
    
    // Réponse personnalisée
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

### Configuration de Pagination par Défaut

```java
@Configuration
public class WebConfig implements WebMvcConfigurer {
    
    @Override
    public void addArgumentResolvers(List<HandlerMethodArgumentResolver> resolvers) {
        PageableHandlerMethodArgumentResolver resolver = 
            new PageableHandlerMethodArgumentResolver();
        resolver.setMaxPageSize(100);
        resolver.setOneIndexedParameters(false); // Index commence à 0
        resolver.setFallbackPageable(PageRequest.of(0, 20));
        resolvers.add(resolver);
    }
}
```

## 🔍 Filtrage et Recherche

### Filtrage Simple

```java
@RestController
@RequestMapping("/api/products")
public class ProductController {
    
    // Filtrage avec paramètres optionnels
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
    
    // Recherche par mot-clé
    @GetMapping("/search")
    public ResponseEntity<List<Product>> searchProducts(
            @RequestParam String query) {
        List<Product> products = productService.search(query);
        return ResponseEntity.ok(products);
    }
    
    // Recherche avancée avec objet de filtre
    @PostMapping("/search")
    public ResponseEntity<Page<Product>> advancedSearch(
            @RequestBody ProductSearchCriteria criteria,
            Pageable pageable) {
        
        Page<Product> products = productService.search(criteria, pageable);
        return ResponseEntity.ok(products);
    }
}

// Objet de critères
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

### Spécifications JPA (Filtrage Dynamique)

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

### Configuration Globale

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

### CORS par Controller

```java
@RestController
@RequestMapping("/api/users")
@CrossOrigin(origins = "http://localhost:3000")
public class UserController {
    // Tous les endpoints de ce controller acceptent les requêtes de localhost:3000
}
```

### CORS par Endpoint

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

### Configuration CORS Complète

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

## 📊 Exemple Complet - API CRUD

```java
@RestController
@RequestMapping("/api/products")
@Validated
@CrossOrigin(origins = "http://localhost:3000")
public class ProductController {
    
    @Autowired
    private ProductService productService;
    
    // GET - Liste avec pagination, tri et filtrage
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
    
    // GET - Un produit par ID
    @GetMapping("/{id}")
    public ResponseEntity<ProductDTO> getProduct(@PathVariable Long id) {
        return productService.findById(id)
            .map(ResponseEntity::ok)
            .orElse(ResponseEntity.notFound().build());
    }
    
    // POST - Créer un produit
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
    
    // PUT - Mettre à jour complètement
    @PreAuthorize("hasRole('ADMIN')")
    @PutMapping("/{id}")
    public ResponseEntity<ProductDTO> updateProduct(
            @PathVariable Long id,
            @RequestBody @Valid UpdateProductRequest request) {
        
        return productService.update(id, request)
            .map(ResponseEntity::ok)
            .orElse(ResponseEntity.notFound().build());
    }
    
    // PATCH - Mise à jour partielle
    @PreAuthorize("hasRole('ADMIN')")
    @PatchMapping("/{id}")
    public ResponseEntity<ProductDTO> partialUpdate(
            @PathVariable Long id,
            @RequestBody Map<String, Object> updates) {
        
        return productService.partialUpdate(id, updates)
            .map(ResponseEntity::ok)
            .orElse(ResponseEntity.notFound().build());
    }
    
    // DELETE - Supprimer
    @PreAuthorize("hasRole('ADMIN')")
    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteProduct(@PathVariable Long id) {
        boolean deleted = productService.delete(id);
        return deleted ? 
            ResponseEntity.noContent().build() : 
            ResponseEntity.notFound().build();
    }
    
    // Endpoints supplémentaires
    
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
# Questions & Réponses - REST Controllers & Endpoints Spring Boot

## 🎮 Controllers - Architecture MVC

**Q1 : Quelle est la différence entre @Controller et @RestController ?**
**R :**
- **@Controller** : Pour applications web traditionnelles, retourne le nom d'une vue (JSP, Thymeleaf)
- **@RestController** : Pour REST APIs, retourne directement des données (JSON/XML). Équivaut à `@Controller + @ResponseBody` sur chaque méthode

**Q2 : Qu'est-ce que @RestController fait exactement ?**
**R :** `@RestController` est une méta-annotation combinant `@Controller` et `@ResponseBody`. Elle indique que chaque méthode du controller retourne directement des données sérialisées (JSON) dans le corps de la réponse HTTP, plutôt qu'un nom de vue.

**Q3 : Quelle est l'architecture typique d'une application Spring Boot ?**
**R :**
```
Client → Controller (@RestController) 
       → Service (@Service) 
       → Repository (@Repository) 
       → Database
```
- Controller : Gère les requêtes HTTP
- Service : Logique métier
- Repository : Accès aux données

**Q4 : Peut-on avoir @Controller et @RestController dans la même application ?**
**R :** Oui ! Utilisez @Controller pour les pages web (vues) et @RestController pour les APIs REST. C'est courant dans les applications avec une interface web ET une API.

## 🛣️ Endpoints Mapping

**Q5 : Quelle est la différence entre @RequestMapping et @GetMapping ?**
**R :**
- **@RequestMapping** : Annotation générique, nécessite de spécifier la méthode HTTP explicitement
- **@GetMapping** : Spécialisée pour GET, plus concise et lisible (équivaut à `@RequestMapping(method = RequestMethod.GET)`)

**Q6 : Citez les 5 annotations de mapping spécialisées**
**R :**
1. `@GetMapping` - Récupérer des données
2. `@PostMapping` - Créer une ressource
3. `@PutMapping` - Mettre à jour complètement
4. `@PatchMapping` - Mise à jour partielle
5. `@DeleteMapping` - Supprimer une ressource

**Q7 : Comment définir un préfixe commun pour tous les endpoints d'un controller ?**
**R :**
```java
@RestController
@RequestMapping("/api/users")  // Préfixe
public class UserController {
    @GetMapping  // Accessible via /api/users
    @GetMapping("/{id}")  // Accessible via /api/users/{id}
}
```

**Q8 : Quelle est la différence entre PUT et PATCH ?**
**R :**
- **PUT** : Remplacement complet de la ressource (tous les champs requis)
- **PATCH** : Mise à jour partielle (seulement les champs modifiés)

**Q9 : Comment mapper plusieurs URLs sur la même méthode ?**
**R :**
```java
@GetMapping({"/users", "/members", "/people"})
public List<User> getUsers() {
    return userService.findAll();
}
```

**Q10 : Comment ajouter des conditions au mapping (params, headers) ?**
**R :**
```java
@GetMapping(
    value = "/users",
    params = "active=true",           // Requiert ?active=true
    headers = "X-API-Version=1",      // Requiert ce header
    consumes = "application/json",    // Accepte JSON
    produces = "application/json"     // Produit JSON
)
```

## 📥 @PathVariable

**Q11 : Qu'est-ce que @PathVariable et comment l'utiliser ?**
**R :** `@PathVariable` extrait des valeurs de l'URL. Exemple :
```java
@GetMapping("/users/{id}")
public User getUser(@PathVariable Long id) {
    return userService.findById(id);
}
// GET /api/users/123 → id = 123
```

**Q12 : Comment gérer des PathVariable avec des noms différents de la variable Java ?**
**R :**
```java
@GetMapping("/users/{userId}")
public User getUser(@PathVariable("userId") Long id) {
    return userService.findById(id);
}
```

**Q13 : Comment gérer plusieurs PathVariable ?**
**R :**
```java
@GetMapping("/users/{userId}/posts/{postId}")
public Post getUserPost(
        @PathVariable Long userId,
        @PathVariable Long postId) {
    return postService.findByUserAndId(userId, postId);
}
// GET /api/users/5/posts/42
```

**Q14 : Comment rendre un PathVariable optionnel ?**
**R :**
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

**Q15 : Comment récupérer toutes les variables d'URL dans une Map ?**
**R :**
```java
@GetMapping("/data/{var1}/{var2}/{var3}")
public String getData(@PathVariable Map<String, String> pathVars) {
    // pathVars contient toutes les variables
    return pathVars.toString();
}
```

## 📋 @RequestParam

**Q16 : Qu'est-ce que @RequestParam et comment l'utiliser ?**
**R :** `@RequestParam` extrait les paramètres de query string. Exemple :
```java
@GetMapping("/users")
public List<User> findUsers(@RequestParam String name) {
    return userService.findByName(name);
}
// GET /api/users?name=John
```

**Q17 : Comment rendre un RequestParam optionnel avec valeur par défaut ?**
**R :**
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

**Q18 : Quelle est la différence entre @PathVariable et @RequestParam ?**
**R :**
- **@PathVariable** : Variables dans l'URL → `/users/{id}` → `/users/123`
- **@RequestParam** : Query parameters → `/users?id=123`

PathVariable pour identifiants de ressources, RequestParam pour filtres/options.

**Q19 : Comment gérer plusieurs valeurs pour le même paramètre ?**
**R :**
```java
@GetMapping("/users")
public List<User> findUsers(@RequestParam List<Long> id) {
    return userService.findByIds(id);
}
// GET /api/users?id=1&id=2&id=3
```

**Q20 : Comment récupérer tous les query params dans une Map ?**
**R :**
```java
@GetMapping("/search")
public List<User> search(@RequestParam Map<String, String> allParams) {
    // allParams contient tous les query params
    return userService.search(allParams);
}
```

**Q21 : Exemple pratique : pagination et filtrage avec RequestParam**
**R :**
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

**Q22 : Qu'est-ce que @RequestBody et quand l'utiliser ?**
**R :** `@RequestBody` convertit le corps JSON de la requête en objet Java. Utilisé avec POST/PUT/PATCH pour recevoir des données :
```java
@PostMapping("/users")
public User createUser(@RequestBody User user) {
    return userService.save(user);
}
```

**Q23 : Comment valider les données reçues avec @RequestBody ?**
**R :**
```java
@PostMapping("/users")
public ResponseEntity<User> createUser(
        @RequestBody @Valid User user) {
    // @Valid déclenche la validation Bean Validation
    User created = userService.save(user);
    return ResponseEntity.status(HttpStatus.CREATED).body(created);
}
```

**Q24 : Quelle est la différence entre @RequestBody et @RequestParam ?**
**R :**
- **@RequestBody** : Données dans le corps de la requête (JSON/XML), pour structures complexes
- **@RequestParam** : Paramètres dans l'URL (?key=value), pour valeurs simples

**Q25 : Peut-on combiner @RequestBody avec @PathVariable et @RequestParam ?**
**R :** Oui :
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

**Q26 : Comment utiliser un DTO au lieu de l'entité avec @RequestBody ?**
**R :**
```java
@PostMapping("/users")
public ResponseEntity<UserResponse> createUser(
        @RequestBody @Valid CreateUserRequest request) {
    User user = userService.create(request);
    UserResponse response = new UserResponse(user);
    return ResponseEntity.status(HttpStatus.CREATED).body(response);
}
```
Les DTOs séparent l'API externe de la structure interne (sécurité, flexibilité).

## 📤 ResponseEntity<>

**Q27 : Qu'est-ce que ResponseEntity et pourquoi l'utiliser ?**
**R :** `ResponseEntity<T>` est un wrapper permettant de contrôler complètement la réponse HTTP : status code, headers, et body. Offre plus de contrôle qu'un simple retour d'objet.

**Q28 : Quelle est la différence entre retourner un objet directement vs ResponseEntity ?**
**R :**
```java
// Retour direct (status 200 par défaut)
@GetMapping("/users")
public List<User> getUsers() {
    return userService.findAll();
}

// Avec ResponseEntity (contrôle du status)
@GetMapping("/users")
public ResponseEntity<List<User>> getUsers() {
    List<User> users = userService.findAll();
    return ResponseEntity.ok(users);  // Ou .notFound(), .created(), etc.
}
```

**Q29 : Comment retourner un 201 CREATED avec ResponseEntity ?**
**R :**
```java
@PostMapping("/users")
public ResponseEntity<User> createUser(@RequestBody User user) {
    User created = userService.save(user);
    URI location = URI.create("/api/users/" + created.getId());
    return ResponseEntity.created(location).body(created);
}
```

**Q30 : Comment retourner un 204 NO CONTENT ?**
**R :**
```java
@DeleteMapping("/users/{id}")
public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    userService.delete(id);
    return ResponseEntity.noContent().build();
}
```

**Q31 : Comment retourner un 404 NOT FOUND avec Optional ?**
**R :**
```java
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    return userService.findById(id)
        .map(ResponseEntity::ok)
        .orElse(ResponseEntity.notFound().build());
}
```

**Q32 : Comment ajouter des headers personnalisés à la réponse ?**
**R :**
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

**Q33 : Comment gérer le téléchargement de fichiers avec ResponseEntity ?**
**R :**
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

**Q34 : Comment construire une ResponseEntity complexe avec le builder pattern ?**
**R :**
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

**Q35 : Quels sont les principaux status codes 2xx et leur usage ?**
**R :**
- **200 OK** : Requête réussie (GET, PUT)
- **201 CREATED** : Ressource créée (POST)
- **202 ACCEPTED** : Requête acceptée, traitement asynchrone
- **204 NO CONTENT** : Succès sans contenu (DELETE)

**Q36 : Quels sont les principaux status codes 4xx et leur usage ?**
**R :**
- **400 BAD REQUEST** : Requête invalide (validation échouée)
- **401 UNAUTHORIZED** : Non authentifié
- **403 FORBIDDEN** : Authentifié mais pas autorisé
- **404 NOT FOUND** : Ressource introuvable
- **409 CONFLICT** : Conflit (email déjà existant)
- **422 UNPROCESSABLE ENTITY** : Validation métier échouée

**Q37 : Quelle est la différence entre 401 et 403 ?**
**R :**
- **401 UNAUTHORIZED** : "Qui êtes-vous ?" → Pas authentifié, login requis
- **403 FORBIDDEN** : "Je sais qui vous êtes, mais vous n'avez pas le droit" → Authentifié mais pas autorisé

**Q38 : Quand utiliser 400 vs 422 ?**
**R :**
- **400 BAD REQUEST** : Erreur de format/syntaxe (JSON invalide, champ manquant)
- **422 UNPROCESSABLE ENTITY** : Format correct mais validation métier échouée (email déjà utilisé, stock insuffisant)

**Q39 : Comment retourner un 409 CONFLICT ?**
**R :**
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

**Q40 : Comment gérer les erreurs 5xx ?**
**R :**
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
Mais préférez un `@RestControllerAdvice` pour la gestion globale.

## 🛡️ Gestion Globale des Erreurs

**Q41 : Qu'est-ce que @RestControllerAdvice ?**
**R :** `@RestControllerAdvice` est une annotation pour gérer les exceptions globalement dans toute l'application. Évite de répéter le try-catch dans chaque controller.

**Q42 : Comment gérer les exceptions 404 globalement ?**
**R :**
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

**Q43 : Comment gérer automatiquement les erreurs de validation (@Valid) ?**
**R :**
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

**Q44 : Quelle est la différence entre @ControllerAdvice et @RestControllerAdvice ?**
**R :**
- **@ControllerAdvice** : Pour applications web avec vues
- **@RestControllerAdvice** : Pour REST APIs, équivaut à `@ControllerAdvice + @ResponseBody`

## ✅ Validation

**Q45 : Quelles sont les principales annotations de validation Bean Validation ?**
**R :**
- `@NotNull` : Champ non null
- `@NotBlank` : Chaine non vide (pas null, pas blank)
- `@NotEmpty` : Collection/String non vide
- `@Size(min, max)` : Taille
- `@Min` / `@Max` : Valeur numérique
- `@Email` : Format email valide
- `@Pattern(regexp)` : Expression régulière
- `@Past` / `@Future` : Date passée/future

**Q46 : Comment activer la validation dans un controller ?**
**R :**
```java
@PostMapping("/users")
public ResponseEntity<User> createUser(
        @RequestBody @Valid User user) {  // @Valid active la validation
    User created = userService.save(user);
    return ResponseEntity.status(HttpStatus.CREATED).body(created);
}
```

**Q47 : Comment définir des validations sur un DTO ?**
**R :**
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

**Q48 : Comment gérer manuellement les erreurs de validation ?**
**R :**
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

**Q49 : Comment créer une validation personnalisée ?**
**R :**
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

// Validateur
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

**Q50 : Comment utiliser des groupes de validation ?**
**R :**
```java
// Interfaces de groupes
public interface CreateValidation {}
public interface UpdateValidation {}

// Entité
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

**Q51 : Qu'est-ce que @PreAuthorize ?**
**R :** `@PreAuthorize` est une annotation Spring Security pour contrôler l'accès à une méthode AVANT son exécution. Permet de définir des règles d'autorisation avec des expressions SpEL.

**Q52 : Comment activer @PreAuthorize ?**
**R :**
```java
@Configuration
@EnableGlobalMethodSecurity(prePostEnabled = true)
public class SecurityConfig {
    // Configuration
}
```

**Q53 : Comment restreindre un endpoint aux utilisateurs authentifiés ?**
**R :**
```java
@PreAuthorize("isAuthenticated()")
@GetMapping("/profile")
public User getProfile() {
    return userService.getCurrentUser();
}
```

**Q54 : Comment restreindre par rôle ?**
**R :**
```java
@PreAuthorize("hasRole('ADMIN')")
@DeleteMapping("/{id}")
public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    userService.delete(id);
    return ResponseEntity.noContent().build();
}
```

**Q55 : Comment autoriser plusieurs rôles ?**
**R :**
```java
@PreAuthorize("hasAnyRole('ADMIN', 'MANAGER', 'SUPERVISOR')")
@PutMapping("/{id}")
public ResponseEntity<User> updateUser(@PathVariable Long id, @RequestBody User user) {
    return ResponseEntity.ok(userService.update(id, user));
}
```

**Q56 : Comment vérifier qu'un utilisateur modifie ses propres données ?**
**R :**
```java
@PreAuthorize("#id == authentication.principal.id")
@PutMapping("/{id}/profile")
public ResponseEntity<User> updateOwnProfile(
        @PathVariable Long id,
        @RequestBody User user) {
    return ResponseEntity.ok(userService.update(id, user));
}
```

**Q57 : Comment combiner plusieurs conditions avec OR ?**
**R :**
```java
@PreAuthorize("hasRole('ADMIN') or #id == authentication.principal.id")
@GetMapping("/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id) {
    return userService.findById(id)
        .map(ResponseEntity::ok)
        .orElse(ResponseEntity.notFound().build());
}
```

**Q58 : Comment utiliser une méthode personnalisée pour la sécurité ?**
**R :**
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

**Q59 : Quelle est la différence entre @PreAuthorize et @Secured ?**
**R :**
- **@PreAuthorize** : Expressions SpEL complexes, conditions multiples
- **@Secured** : Simple, seulement rôles statiques

`@PreAuthorize` est plus puissant et flexible.

**Q60 : Qu'est-ce que @PostAuthorize ?**
**R :** `@PostAuthorize` vérifie l'autorisation APRÈS l'exécution de la méthode, basé sur le résultat :
```java
@PostAuthorize("returnObject.owner == authentication.principal.username")
@GetMapping("/documents/{id}")
public Document getDocument(@PathVariable Long id) {
    return documentService.findById(id);
}
// Accès autorisé seulement si le document appartient à l'utilisateur
```

## 📤📥 Upload et Download

**Q61 : Comment gérer l'upload d'un fichier ?**
**R :**
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

**Q62 : Comment gérer l'upload de plusieurs fichiers ?**
**R :**
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

**Q63 : Comment télécharger un fichier ?**
**R :**
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

**Q64 : Quelle est la différence entre "attachment" et "inline" pour Content-Disposition ?**
**R :**
- **attachment** : Force le téléchargement (dialogue "Enregistrer sous")
- **inline** : Affiche dans le navigateur si possible (PDF, images)

**Q65 : Comment exporter des données en CSV ?**
**R :**
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

## 📄 Pagination et Tri

**Q66 : Comment implémenter la pagination avec Spring Data ?**
**R :**
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

**Q67 : Comment ajouter le tri à la pagination ?**
**R :**
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

**Q68 : Comment utiliser Pageable directement comme paramètre ?**
**R :**
```java
@GetMapping
public ResponseEntity<Page<User>> getUsers(Pageable pageable) {
    // Spring gère automatiquement:
    // ?page=0&size=10&sort=name,asc&sort=email,desc
    return ResponseEntity.ok(userRepository.findAll(pageable));
}
```

**Q69 : Comment personnaliser la réponse de pagination ?**
**R :**
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

Q70 : Que retourne un objet Page de Spring Data ?**
**R :** Un objet `Page<T>` contient :
- `getContent()` : Liste des éléments de la page actuelle
- `getTotalElements()` : Nombre total d'éléments
- `getTotalPages()` : Nombre total de pages
- `getNumber()` : Numéro de la page actuelle
- `getSize()` : Taille de la page
- `hasNext()` / `hasPrevious()` : Booléens de navigation
- `isFirst()` / `isLast()` : Première/dernière page

## 🔍 Filtrage et Recherche

**Q71 : Comment implémenter un filtrage simple avec RequestParam optionnels ?**
**R :**
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

**Q72 : Qu'est-ce que JPA Specifications et quand l'utiliser ?**
**R :** JPA Specifications permet de construire dynamiquement des requêtes avec des critères conditionnels. Utile pour des filtres complexes et combinables :
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

// Utilisation
Specification<Product> spec = Specification
    .where(ProductSpecifications.hasCategory(category))
    .and(ProductSpecifications.priceBetween(minPrice, maxPrice));

Page<Product> products = productRepository.findAll(spec, pageable);
```

**Q73 : Comment implémenter une recherche textuelle ?**
**R :**
```java
@GetMapping("/search")
public ResponseEntity<List<User>> searchUsers(
        @RequestParam String query) {
    List<User> users = userService.search(query);
    return ResponseEntity.ok(users);
}

// Dans le repository
@Query("SELECT u FROM User u WHERE " +
       "LOWER(u.name) LIKE LOWER(CONCAT('%', :query, '%')) OR " +
       "LOWER(u.email) LIKE LOWER(CONCAT('%', :query, '%'))")
List<User> search(@Param("query") String query);
```

**Q74 : Comment combiner filtrage, recherche et pagination ?**
**R :**
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

**Q75 : Comment créer un endpoint de recherche avancée avec objet de critères ?**
**R :**
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

**Q76 : Qu'est-ce que CORS et pourquoi en a-t-on besoin ?**
**R :** CORS (Cross-Origin Resource Sharing) est un mécanisme de sécurité des navigateurs. Par défaut, un site web ne peut pas faire de requêtes vers une autre origine (domaine, port, protocole différent). CORS permet d'autoriser explicitement ces requêtes.

**Q77 : Comment activer CORS pour un controller spécifique ?**
**R :**
```java
@RestController
@RequestMapping("/api/users")
@CrossOrigin(origins = "http://localhost:3000")
public class UserController {
    // Tous les endpoints acceptent les requêtes de localhost:3000
}
```

**Q78 : Comment activer CORS pour un endpoint spécifique ?**
**R :**
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

**Q79 : Comment configurer CORS globalement ?**
**R :**
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

**Q80 : Quelle est la différence entre allowedOrigins et allowedOriginPatterns ?**
**R :**
- **allowedOrigins** : URLs exactes (ex: "http://localhost:3000")
- **allowedOriginPatterns** : Patterns avec wildcards (ex: "http://*.example.com")

**Q81 : Que signifie allowCredentials(true) dans CORS ?**
**R :** `allowCredentials(true)` permet l'envoi de cookies et headers d'authentification dans les requêtes cross-origin. Nécessaire pour les sessions et JWT dans les cookies.

**Q82 : Pourquoi CORS ne fonctionne-t-il qu'avec le navigateur ?**
**R :** CORS est une protection du navigateur. Les outils comme Postman, cURL, ou les backends ne sont pas concernés. Seuls les navigateurs web appliquent la politique CORS.

## 📋 Content Negotiation

**Q83 : Qu'est-ce que Content Negotiation ?**
**R :** Content Negotiation permet au serveur de retourner différents formats (JSON, XML, CSV) selon ce que le client demande via le header `Accept`.

**Q84 : Comment spécifier le format de réponse avec produces ?**
**R :**
```java
// JSON uniquement
@GetMapping(produces = MediaType.APPLICATION_JSON_VALUE)
public List<User> getUsersJson() {
    return userService.findAll();
}

// XML uniquement
@GetMapping(produces = MediaType.APPLICATION_XML_VALUE)
public List<User> getUsersXml() {
    return userService.findAll();
}

// JSON ou XML selon le header Accept
@GetMapping(produces = {
    MediaType.APPLICATION_JSON_VALUE,
    MediaType.APPLICATION_XML_VALUE
})
public List<User> getUsers() {
    return userService.findAll();
}
```

**Q85 : Comment restreindre le format accepté avec consumes ?**
**R :**
```java
@PostMapping(
    consumes = MediaType.APPLICATION_JSON_VALUE,
    produces = MediaType.APPLICATION_JSON_VALUE
)
public ResponseEntity<User> createUser(@RequestBody User user) {
    return ResponseEntity.status(HttpStatus.CREATED)
        .body(userService.save(user));
}
// Accepte seulement JSON, rejette XML ou autre
```

**Q86 : Comment le client spécifie le format souhaité ?**
**R :** Via le header `Accept` :
```bash
# Demande JSON
GET /api/users
Accept: application/json

# Demande XML
GET /api/users
Accept: application/xml

# Demande CSV
GET /api/users
Accept: text/csv
```

**Q87 : Comment gérer l'upload multipart/form-data ?**
**R :**
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

## 🔒 Headers HTTP

**Q88 : Comment récupérer un header spécifique ?**
**R :**
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

**Q89 : Comment rendre un header optionnel ?**
**R :**
```java
@GetMapping("/users")
public List<User> getUsers(
        @RequestHeader(value = "User-Agent", required = false) 
        String userAgent) {
    log.info("Request from: " + userAgent);
    return userService.findAll();
}
```

**Q90 : Comment récupérer tous les headers ?**
**R :**
```java
@GetMapping("/debug")
public Map<String, String> debugHeaders(
        @RequestHeader Map<String, String> headers) {
    return headers;
}

// Ou avec HttpHeaders
@GetMapping("/debug")
public HttpHeaders debugHeaders(@RequestHeader HttpHeaders headers) {
    return headers;
}
```

**Q91 : Comment ajouter un header personnalisé à la réponse ?**
**R :**
```java
@GetMapping("/data")
public ResponseEntity<String> getData() {
    return ResponseEntity.ok()
        .header("X-Custom-Header", "my-value")
        .header("X-Request-ID", UUID.randomUUID().toString())
        .body("Data");
}
```

**Q92 : Comment gérer les cookies ?**
**R :**
```java
// Lire un cookie
@GetMapping("/profile")
public User getProfile(@CookieValue("sessionId") String sessionId) {
    return userService.findBySession(sessionId);
}

// Définir un cookie dans la réponse
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

## 🎯 Bonnes Pratiques REST

**Q93 : Quelles sont les conventions de nommage REST ?**
**R :**
- Utiliser des noms (pas de verbes) : `/users` pas `/getUsers`
- Utiliser le pluriel : `/users` pas `/user`
- Utiliser des hiérarchies : `/users/{userId}/orders/{orderId}`
- Minuscules et tirets : `/user-profiles` pas `/UserProfiles`
- Pas de trailing slash : `/users` pas `/users/`

**Q94 : Quelles méthodes HTTP utiliser pour quelles opérations ?**
**R :**
- **GET** : Récupérer (lecture, idempotent)
- **POST** : Créer une ressource
- **PUT** : Remplacer complètement une ressource (idempotent)
- **PATCH** : Mise à jour partielle
- **DELETE** : Supprimer (idempotent)

**Q95 : Qu'est-ce que l'idempotence et pourquoi est-ce important ?**
**R :** Une opération est idempotente si l'exécuter plusieurs fois produit le même résultat qu'une seule fois. GET, PUT, DELETE sont idempotents. POST ne l'est pas (créer 2 fois = 2 ressources). Important pour la fiabilité (retry safe).

**Q96 : Comment structurer les URLs pour les relations ?**
**R :**
```java
// Ressources imbriquées
GET    /users/{userId}/orders           // Liste des commandes d'un user
GET    /users/{userId}/orders/{orderId} // Une commande spécifique
POST   /users/{userId}/orders           // Créer une commande pour un user
DELETE /users/{userId}/orders/{orderId} // Supprimer une commande

// Ou ressources indépendantes avec filtrage
GET /orders?userId=5
```

**Q97 : Comment gérer le versioning d'API ?**
**R :**
```java
// Via URL (recommandé)
@RequestMapping("/api/v1/users")
@RequestMapping("/api/v2/users")

// Via header
@GetMapping(value = "/users", headers = "X-API-Version=1")
@GetMapping(value = "/users", headers = "X-API-Version=2")

// Via Accept header
@GetMapping(value = "/users", produces = "application/vnd.myapp.v1+json")
@GetMapping(value = "/users", produces = "application/vnd.myapp.v2+json")
```

**Q98 : Comment documenter une API REST ?**
**R :** Utiliser **Swagger/OpenAPI** (SpringDoc) :
```xml
<dependency>
    <groupId>org.springdoc</groupId>
    <artifactId>springdoc-openapi-starter-webmvc-ui</artifactId>
    <version>2.2.0</version>
</dependency>
```
Documentation auto-générée disponible sur `/swagger-ui.html`

**Q99 : Quelles informations inclure dans une réponse d'erreur ?**
**R :**
```java
@Data
@AllArgsConstructor
public class ErrorResponse {
    private int status;           // 404
    private String error;         // "Not Found"
    private String message;       // "User with id 5 not found"
    private String path;          // "/api/users/5"
    private LocalDateTime timestamp;
    private List<String> details; // Erreurs de validation
}
```

**Q100 : Comment implémenter HATEOAS (liens hypermedia) ?**
**R :** HATEOAS ajoute des liens dans les réponses pour guider le client :
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

// Réponse JSON:
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

## 🔧 Configuration et Optimisation

**Q101 : Comment configurer la taille maximale des uploads ?**
**R :**
```properties
# application.properties
spring.servlet.multipart.max-file-size=10MB
spring.servlet.multipart.max-request-size=50MB
```

**Q102 : Comment activer la compression des réponses ?**
**R :**
```properties
server.compression.enabled=true
server.compression.mime-types=application/json,application/xml,text/html,text/plain
server.compression.min-response-size=1024
```

**Q103 : Comment gérer les timeouts ?**
**R :**
```properties
# Timeout de connexion
spring.mvc.async.request-timeout=30000

# Timeout Tomcat
server.tomcat.connection-timeout=20000
```

**Q104 : Comment activer le cache HTTP ?**
**R :**
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

**Q105 : Comment gérer les requêtes asynchrones ?**
**R :**
```java
@GetMapping("/async")
public CompletableFuture<List<User>> getUsersAsync() {
    return CompletableFuture.supplyAsync(() -> userService.findAll());
}

// Ou avec DeferredResult
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

## 📊 Exemple Complet Récapitulatif

**Q106 : Créez un controller CRUD complet avec toutes les bonnes pratiques**
**R :**
```java
@RestController
@RequestMapping("/api/v1/products")
@Validated
@CrossOrigin(origins = "${app.cors.allowed-origins}")
@Tag(name = "Products", description = "Product management API")
public class ProductController {
    
    @Autowired
    private ProductService productService;
    
    // GET - Liste avec filtrage et pagination
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
    
    // GET - Par ID
    @GetMapping("/{id}")
    @Operation(summary = "Get product by ID")
    public ResponseEntity<ProductDTO> getProduct(@PathVariable Long id) {
        return productService.findById(id)
            .map(product -> ResponseEntity.ok()
                .eTag("\"product-" + product.getVersion() + "\"")
                .body(product))
            .orElse(ResponseEntity.notFound().build());
    }
    
    // POST - Créer
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
    
    // PUT - Mettre à jour
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
    
    // PATCH - Mise à jour partielle
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
    
    // Gestion d'erreurs locale
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
