## [**..**](./00_index.md)

## Exception Handling - Points Clés

### **Concept général**
L'**Exception Handling** dans Spring Boot permet de gérer les erreurs de manière centralisée et de retourner des réponses HTTP appropriées au client avec des messages d'erreur personnalisés au lieu des erreurs par défaut de Spring.

### **Annotations principales**

**@ResponseStatus :**
- Associe un **HTTP status code** à une exception spécifique
- Quand l'exception est levée, Spring retourne automatiquement le status code spécifié

```java
@ResponseStatus(code = HttpStatus.NOT_FOUND)
public class ResourceNotFoundException extends RuntimeException {
    public ResourceNotFoundException(String message) {
        super(message);
    }
}
```

**@ControllerAdvice :**
- Crée un **global exception handler** pour toute l'application
- Centralise la logique de gestion des exceptions
- Applicable à tous les controllers

**@ExceptionHandler :**
- Gère des exceptions spécifiques levées par les controller methods
- Permet de définir une logique personnalisée pour chaque type d'exception
- Retourne une réponse personnalisée au lieu d'une erreur par défaut

### **Architecture de gestion des exceptions**

**1. Créer des custom exceptions :**
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

**2. Créer une classe ErrorDetails (DTO pour les erreurs) :**
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
    
    // Getters et Setters
}
```

**3. Créer un Global Exception Handler avec @ControllerAdvice :**
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

### **Utilisation dans les Controllers**

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
            // Lève l'exception, capturée par GlobalExceptionHandler
            throw new ResourceNotFoundException("Post", "id", id);
        }
        
        return ResponseEntity.ok(post);
    }
    
    @PostMapping
    public ResponseEntity<Post> createPost(@Valid @RequestBody Post post) {
        if (postService.existsByTitle(post.getTitle())) {
            // Lève une exception métier
            throw new BlogAPIException(HttpStatus.BAD_REQUEST, 
                                      "Post with this title already exists");
        }
        
        Post savedPost = postService.save(post);
        return ResponseEntity.status(HttpStatus.CREATED).body(savedPost);
    }
}
```

### **Types de gestion d'exceptions**

**1. Specific Exception Handling :**
- Gérer des exceptions spécifiques avec des handlers dédiés
- Permet des réponses personnalisées selon le type d'erreur
- Exemple : ResourceNotFoundException → 404, BlogAPIException → 400

**2. Global Exception Handling :**
- Catch-all handler pour toutes les exceptions non gérées
- Retourne généralement 500 Internal Server Error
- Évite d'exposer les stack traces au client

### **Format de réponse d'erreur**

**Exemple de réponse JSON :**
```json
{
  "timestamp": "2025-10-30T10:15:30.000+00:00",
  "message": "Post not found with id : '123'",
  "details": "uri=/api/posts/123"
}
```

### **Hiérarchie de gestion**

**Order de priorité :**
1. **@ExceptionHandler** le plus spécifique (ResourceNotFoundException)
2. **@ExceptionHandler** moins spécifique (RuntimeException)
3. **@ExceptionHandler** global (Exception.class)

Spring utilise le handler le plus spécifique disponible.

### **WebRequest**

L'objet **WebRequest** fournit des informations sur la request HTTP :
```java
webRequest.getDescription(false)  // URI sans query params
webRequest.getDescription(true)   // URI complète avec query params
```

### **Best Practices**

1. **Créer des custom exceptions** pour les erreurs métier
2. **Utiliser @ControllerAdvice** pour centraliser la gestion
3. **Retourner des ErrorDetails** structurés avec timestamp, message, details
4. **Ne jamais exposer les stack traces** au client en production
5. **Utiliser les bons HTTP status codes** (404, 400, 500, etc.)
6. **Logger les exceptions** pour le debugging
7. **Handle validation exceptions** (MethodArgumentNotValidException)
8. **Créer un handler global** (Exception.class) comme filet de sécurité

### **Points clés**

- **@ResponseStatus** : Associe un status code à une exception
- **@ControllerAdvice** : Global exception handler pour toute l'application
- **@ExceptionHandler** : Gère des exceptions spécifiques
- **ErrorDetails** : DTO pour structurer les réponses d'erreur
- **WebRequest** : Informations sur la HTTP request
- **Specific vs Global handlers** : Handlers spécifiques pour certaines exceptions, global pour le reste
- **HTTP Status Codes** : 404 (Not Found), 400 (Bad Request), 500 (Internal Server Error)
- Éviter d'exposer les **stack traces** en production


---

## Questions/Réponses - Exception Handling

### **1. Qu'est-ce que l'Exception Handling dans Spring Boot et pourquoi est-ce important ?**

**Réponse :**
L'**Exception Handling** est le mécanisme permettant de gérer les erreurs qui surviennent dans une application et de retourner des réponses HTTP appropriées au client.

**Importance :**
- **User Experience** : Retourne des messages d'erreur clairs et compréhensibles
- **Sécurité** : Évite d'exposer les stack traces et détails internes
- **Centralisation** : Gère toutes les exceptions au même endroit
- **Cohérence** : Format de réponse d'erreur uniforme pour toute l'application
- **Debugging** : Facilite l'identification et la résolution des problèmes
- **HTTP Status Codes appropriés** : 404, 400, 500, etc.

Sans exception handling, Spring retourne des erreurs par défaut difficiles à comprendre pour le client.

---

### **2. Quelles sont les principales annotations pour gérer les exceptions dans Spring ?**

**Réponse :**

**@ResponseStatus :**
- Associe un HTTP status code à une exception personnalisée
- Automatiquement retourné quand l'exception est levée

**@ControllerAdvice :**
- Crée un global exception handler
- Centralise la gestion des exceptions pour tous les controllers
- Fonctionne avec @ExceptionHandler

**@ExceptionHandler :**
- Définit une méthode qui gère un type d'exception spécifique
- Peut être utilisé dans un @Controller ou @ControllerAdvice
- Retourne une réponse personnalisée

**Exemple :**
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

### **3. Quelle est la différence entre @ControllerAdvice et @RestControllerAdvice ?**

**Réponse :**

| @ControllerAdvice | @RestControllerAdvice |
|--------------------|----------------------|
| Pour controllers MVC classiques | Pour REST controllers |
| Doit utiliser @ResponseBody explicitement | @ResponseBody implicite sur toutes les méthodes |
| Peut retourner des views | Retourne toujours du JSON/XML |

**Équivalence :**
```java
@RestControllerAdvice = @ControllerAdvice + @ResponseBody
```

**Usage :**
```java
// Pour REST API
@RestControllerAdvice
public class GlobalExceptionHandler {
    @ExceptionHandler(ResourceNotFoundException.class)
    public ErrorDetails handleNotFound(...) {
        // Retourne JSON automatiquement
    }
}

// Pour MVC traditionnel
@ControllerAdvice
public class GlobalExceptionHandler {
    @ExceptionHandler(ResourceNotFoundException.class)
    @ResponseBody  // Nécessaire pour retourner JSON
    public ErrorDetails handleNotFound(...) { }
}
```

---

### **4. Comment créer une custom exception dans Spring Boot ?**

**Réponse :**

**Étape 1 : Créer la classe d'exception**
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

**Étape 2 : Utiliser dans le controller**
```java
@GetMapping("/{id}")
public Post getPostById(@PathVariable Long id) {
    return postRepository.findById(id)
            .orElseThrow(() -> new ResourceNotFoundException("Post", "id", id));
}
```

**Étape 3 : Gérer avec @ExceptionHandler**
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

### **5. Qu'est-ce que la classe ErrorDetails et pourquoi l'utiliser ?**

**Réponse :**
**ErrorDetails** est un **DTO (Data Transfer Object)** qui structure les réponses d'erreur de manière cohérente pour le client.

**Définition :**
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
    
    // Getters et Setters
}
```

**Avantages :**
- **Format uniforme** pour toutes les erreurs
- **Informations utiles** : timestamp, message clair, détails (URI)
- **Facilite le debugging** côté client
- **Professionnel** : structure JSON cohérente

**Exemple de réponse JSON :**
```json
{
  "timestamp": "2025-10-30T10:15:30.000+00:00",
  "message": "Post not found with id : '123'",
  "details": "uri=/api/posts/123"
}
```

---

### **6. Comment implémenter un Global Exception Handler complet ?**

**Réponse :**

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

### **7. Quelle est la différence entre Exception et RuntimeException ?**

**Réponse :**

| Exception (Checked) | RuntimeException (Unchecked) |
|---------------------|------------------------------|
| Doit être catchée ou déclarée (throws) | Pas obligatoire de catcher |
| Compilateur force la gestion | Pas de vérification à la compilation |
| Exemples : IOException, SQLException | Exemples : NullPointerException, IllegalArgumentException |
| Pour erreurs prévisibles | Pour erreurs de programmation |

**Pour Spring REST :**
```java
// Utiliser RuntimeException pour les custom exceptions
public class ResourceNotFoundException extends RuntimeException {
    // Pas besoin de "throws" dans les méthodes
}

// Éviter Exception (checked)
public class ResourceNotFoundException extends Exception {
    // Force "throws ResourceNotFoundException" partout
}
```

**Best practice :** Étendre **RuntimeException** pour les custom exceptions REST.

---

### **8. Comment gérer les erreurs 404 (Resource Not Found) ?**

**Réponse :**

**1. Créer l'exception :**
```java
@ResponseStatus(code = HttpStatus.NOT_FOUND)
public class ResourceNotFoundException extends RuntimeException {
    
    public ResourceNotFoundException(String resourceName, String fieldName, Object fieldValue) {
        super(String.format("%s not found with %s : '%s'", 
                           resourceName, fieldName, fieldValue));
    }
}
```

**2. Lever l'exception dans le service/controller :**
```java
@Service
public class PostService {
    
    public Post findById(Long id) {
        return postRepository.findById(id)
                .orElseThrow(() -> new ResourceNotFoundException("Post", "id", id));
    }
}
```

**3. Gérer avec @ExceptionHandler :**
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

**Réponse au client :**
```json
HTTP/1.1 404 Not Found
{
  "timestamp": "2025-10-30T10:15:30.000+00:00",
  "message": "Post not found with id : '123'",
  "details": "uri=/api/posts/123"
}
```

---

### **9. Comment gérer les erreurs de validation (400 Bad Request) ?**

**Réponse :**

Quand @Valid échoue, Spring lève **MethodArgumentNotValidException**.

**Handler :**
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

**Réponse au client :**
```json
HTTP/1.1 400 Bad Request
{
  "name": "Name is required",
  "email": "Invalid email format",
  "age": "Must be at least 18 years old"
}
```

**Alternative avec ErrorDetails :**
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

### **10. Qu'est-ce que WebRequest et comment l'utiliser ?**

**Réponse :**
**WebRequest** est une interface Spring qui fournit des informations sur la HTTP request courante dans les exception handlers.

**Méthodes utiles :**
```java
@ExceptionHandler(ResourceNotFoundException.class)
public ResponseEntity<ErrorDetails> handle(
        ResourceNotFoundException ex, 
        WebRequest request) {
    
    // URI sans query parameters
    String uri = request.getDescription(false); // "uri=/api/posts/123"
    
    // URI avec query parameters
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

**Usage typique :** Inclure l'URI dans l'ErrorDetails pour le debugging.

---

### **11. Comment gérer plusieurs types d'exceptions dans un seul handler ?**

**Réponse :**

**Option 1 : Liste d'exceptions dans @ExceptionHandler**
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

**Option 2 : Exception parente**
```java
// Si toutes les custom exceptions étendent BusinessException
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

### **12. Quelle est la hiérarchie de résolution des exception handlers ?**

**Réponse :**
Spring utilise le handler **le plus spécifique** disponible :

**Order de priorité (du plus spécifique au plus général) :**
1. **Exception exacte** (ResourceNotFoundException.class)
2. **Exception parente** (RuntimeException.class)
3. **Exception très générale** (Exception.class)

**Exemple :**
```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    
    // 1. Plus spécifique - sera utilisé pour ResourceNotFoundException
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleResourceNotFound(...) {
        return new ResponseEntity<>(..., HttpStatus.NOT_FOUND);
    }
    
    // 2. Moins spécifique - pour toutes les RuntimeException sauf ResourceNotFoundException
    @ExceptionHandler(RuntimeException.class)
    public ResponseEntity<ErrorDetails> handleRuntimeException(...) {
        return new ResponseEntity<>(..., HttpStatus.BAD_REQUEST);
    }
    
    // 3. Catch-all - pour toutes les autres exceptions
    @ExceptionHandler(Exception.class)
    public ResponseEntity<ErrorDetails> handleGlobalException(...) {
        return new ResponseEntity<>(..., HttpStatus.INTERNAL_SERVER_ERROR);
    }
}
```

**Règle :** Toujours définir un handler global (Exception.class) comme filet de sécurité.

---

### **13. Comment créer une exception avec un status code dynamique ?**

**Réponse :**

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
    
    // Utilise le status de l'exception
    return new ResponseEntity<>(error, ex.getStatus());
}

// Usage
throw new APIException(HttpStatus.FORBIDDEN, "Access denied");
throw new APIException(HttpStatus.CONFLICT, "Resource already exists");
```

---

### **14. Comment éviter d'exposer les stack traces en production ?**

**Réponse :**

**Problème :** Par défaut, Spring peut exposer des stack traces détaillées qui révèlent la structure interne de l'application.

**Solutions :**

**1. Ne jamais utiliser exception.printStackTrace() dans le handler**
```java
// ❌ Mauvais
@ExceptionHandler(Exception.class)
public ResponseEntity<ErrorDetails> handle(Exception ex) {
    ex.printStackTrace();  // Exposé dans les logs
    ...
}

// ✅ Bon
@ExceptionHandler(Exception.class)
public ResponseEntity<ErrorDetails> handle(Exception ex, WebRequest request) {
    // Log avec logger
    log.error("Error occurred: {}", ex.getMessage());
    
    ErrorDetails error = new ErrorDetails(
            new Date(),
            "An error occurred",  // Message générique
            request.getDescription(false));
    
    return new ResponseEntity<>(error, HttpStatus.INTERNAL_SERVER_ERROR);
}
```

**2. Configuration application.properties**
```properties
# Désactive les stack traces dans les responses
server.error.include-stacktrace=never
server.error.include-message=never
server.error.include-binding-errors=never

# Ou mode production
server.error.include-stacktrace=on-param  # Uniquement si ?trace=true
```

**3. Messages génériques pour les exceptions non prévues**
```java
@ExceptionHandler(Exception.class)
public ResponseEntity<ErrorDetails> handleGlobalException(
        Exception ex, WebRequest request) {
    
    // Log l'erreur complète
    log.error("Unexpected error", ex);
    
    // Retourne message générique au client
    ErrorDetails error = new ErrorDetails(
            new Date(),
            "An unexpected error occurred. Please contact support.",
            request.getDescription(false));
    
    return new ResponseEntity<>(error, HttpStatus.INTERNAL_SERVER_ERROR);
}
```

---

### **15. Comment gérer les exceptions dans les méthodes asynchrones (@Async) ?**

**Réponse :**

Les exceptions dans les méthodes @Async ne sont pas capturées par @ControllerAdvice car elles s'exécutent dans un thread différent.

**Solution : AsyncUncaughtExceptionHandler**
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
        
        // Envoyer notification, sauvegarder en DB, etc.
    }
}
```

---

### **16. Comment tester l'exception handling dans les tests unitaires ?**

**Réponse :**

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
        // Simuler une exception inattendue
        mockMvc.perform(get("/api/error-endpoint"))
                .andExpect(status().isInternalServerError())
                .andExpect(jsonPath("$.message").exists());
    }
}
```

---

### **17. Quelle est la différence entre @ExceptionHandler au niveau controller vs @ControllerAdvice ?**

**Réponse :**

**@ExceptionHandler dans @Controller :**
- Gère les exceptions **uniquement pour ce controller**
- Portée locale

```java
@RestController
@RequestMapping("/api/posts")
public class PostController {
    
    // Gère uniquement les exceptions de PostController
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleNotFound(ResourceNotFoundException ex) {
        ...
    }
}
```

**@ExceptionHandler dans @ControllerAdvice :**
- Gère les exceptions **pour tous les controllers**
- Portée globale

```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    
    // Gère les exceptions de tous les controllers
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleNotFound(ResourceNotFoundException ex) {
        ...
    }
}
```

**Best practice :** Utiliser **@ControllerAdvice** pour centraliser la gestion des exceptions.

---

### **18. Comment gérer les exceptions liées à l'authentification et l'autorisation ?**

**Réponse :**

```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    
    // 401 Unauthorized - Non authentifié
    @ExceptionHandler(AuthenticationException.class)
    public ResponseEntity<ErrorDetails> handleAuthenticationException(
            AuthenticationException ex, WebRequest request) {
        
        ErrorDetails error = new ErrorDetails(
                new Date(),
                "Authentication failed: " + ex.getMessage(),
                request.getDescription(false));
        
        return new ResponseEntity<>(error, HttpStatus.UNAUTHORIZED);
    }
    
    // 403 Forbidden - Authentifié mais pas autorisé
    @ExceptionHandler(AccessDeniedException.class)
    public ResponseEntity<ErrorDetails> handleAccessDeniedException(
            AccessDeniedException ex, WebRequest request) {
        
        ErrorDetails error = new ErrorDetails(
                new Date(),
                "Access denied: " + ex.getMessage(),
                request.getDescription(false));
        
        return new ResponseEntity<>(error, HttpStatus.FORBIDDEN);
    }
    
    // Custom exception pour JWT invalide
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

### **19. Comment logger les exceptions correctement ?**

**Réponse :**

```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    
    private static final Logger log = LoggerFactory.getLogger(GlobalExceptionHandler.class);
    
    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorDetails> handleResourceNotFoundException(
            ResourceNotFoundException ex, WebRequest request) {
        
        // Log WARNING pour les erreurs client (4xx)
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
        
        // Log ERROR pour les erreurs serveur (5xx)
        log.error("Unexpected error occurred - URI: {}", 
                 request.getDescription(false), 
                 ex);  // Inclut la stack trace
        
        ErrorDetails error = new ErrorDetails(
                new Date(),
                "An unexpected error occurred",
                request.getDescription(false));
        
        return new ResponseEntity<>(error, HttpStatus.INTERNAL_SERVER_ERROR);
    }
}
```

**Best practices :**
- **4xx errors (client)** → log.warn()
- **5xx errors (server)** → log.error() avec stack trace
- Inclure l'URI et les paramètres pertinents
- Ne jamais logger de données sensibles (passwords, tokens)

---

### **20. Quelles sont les best practices pour l'Exception Handling ?**

**Réponse :**

**1. Utiliser @ControllerAdvice pour centraliser**
```java
@RestControllerAdvice
public class GlobalExceptionHandler { ... }
```

**2. Créer des custom exceptions spécifiques**
```java
ResourceNotFoundException, DuplicateResourceException, etc.
```

**3. Utiliser ErrorDetails pour formater les réponses**
```java
{ "timestamp", "message", "details" }
```

**4. Ne jamais exposer les stack traces au client**
```java
server.error.include-stacktrace=never
```

**5. Utiliser les bons HTTP status codes**
- 400 Bad Request (validation)
- 401 Unauthorized (authentication)
- 403 Forbidden (authorization)
- 404 Not Found (resource)
- 409 Conflict (duplicate)
- 500 Internal Server Error (unexpected)

**6. Toujours avoir un global exception handler**
```java
@ExceptionHandler(Exception.class)
```

**7. Logger les exceptions correctement**
- WARN pour 4xx
- ERROR pour 5xx avec stack trace

**8. Messages d'erreur clairs et actionnables**
```java
"Post not found with id : '123'" ✅
"Error occurred" ❌
```

**9. Inclure l'URI dans ErrorDetails pour debugging**
```java
webRequest.getDescription(false)
```

**10. Tester l'exception handling**
```java
@Test void testResourceNotFoundException() { ... }
```

---

**Conseil final :** Prépare-toi à implémenter un exception handler complet avec plusieurs types d'exceptions lors de l'entretien. C'est une question pratique très courante ! 💪
