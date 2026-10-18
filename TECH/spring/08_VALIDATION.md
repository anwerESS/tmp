
## [**..**](./00_index.md)

## Validation - Points Clés

### **Concept général**
La validation permet de vérifier que les données reçues (dans les requests HTTP) respectent certaines **contraintes** avant de les traiter. Spring Boot utilise **Bean Validation API (JSR-380)** avec l'implémentation **Hibernate Validator**.

### **Comment ça fonctionne**

**Flow de validation :**
1. Client envoie une request avec des données (JSON, form data)
2. **@Valid** ou **@Validated** déclenche la validation sur l'objet
3. Les **constraints annotations** (@NotNull, @Size, etc.) sont vérifiées
4. Si validation échoue → **MethodArgumentNotValidException** est levée
5. Sinon, les données sont traitées normalement

### **Annotations principales**

**@Valid :**
- Déclenche la validation sur un objet
- Utilisé sur les paramètres de méthode controller
- Vérifie toutes les constraints définies dans la classe

```java
@PostMapping("/users")
public ResponseEntity<User> create(@Valid @RequestBody User user) {
    // Si validation échoue, exception automatique
    return ResponseEntity.ok(userService.save(user));
}
```

**Constraints courantes :**

| Annotation | Description |
|------------|-------------|
| `@NotNull` | La valeur ne doit pas être null |
| `@NotEmpty` | String/Collection ne doit pas être vide (null ou empty) |
| `@NotBlank` | String ne doit pas être vide ou whitespace uniquement |
| `@Size(min, max)` | Taille d'une String ou Collection |
| `@Min(value)` | Valeur minimale pour un nombre |
| `@Max(value)` | Valeur maximale pour un nombre |
| `@Email` | Valide un format email |
| `@Pattern(regex)` | Valide avec une regex |
| `@Positive` | Nombre positif (> 0) |
| `@PositiveOrZero` | Nombre positif ou zéro (>= 0) |
| `@Past` | Date dans le passé |
| `@Future` | Date dans le futur |

### **Exemple complet**

**Entity avec constraints :**
```java
public class User {
    
    @NotNull(message = "ID cannot be null")
    private Long id;
    
    @NotBlank(message = "Name is required")
    @Size(min = 2, max = 50, message = "Name must be between 2 and 50 characters")
    private String name;
    
    @NotEmpty(message = "Email is required")
    @Email(message = "Invalid email format")
    private String email;
    
    @Min(value = 18, message = "Must be at least 18 years old")
    @Max(value = 100, message = "Age cannot exceed 100")
    private Integer age;
    
    @Pattern(regexp = "^\\+?[0-9]{10,15}$", message = "Invalid phone number")
    private String phone;
}
```

**Controller :**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @PostMapping
    public ResponseEntity<User> createUser(@Valid @RequestBody User user) {
        User savedUser = userService.save(user);
        return ResponseEntity.status(HttpStatus.CREATED).body(savedUser);
    }
}
```

### **Gestion des erreurs de validation**

**Sans handler personnalisé :**
- Spring retourne automatiquement un **400 Bad Request** avec les détails des erreurs

**Avec @ControllerAdvice (recommandé) :**
```java
@ControllerAdvice
public class ValidationExceptionHandler {
    
    @ExceptionHandler(MethodArgumentNotValidException.class)
    public ResponseEntity<Map<String, String>> handleValidationErrors(
            MethodArgumentNotValidException ex) {
        
        Map<String, String> errors = new HashMap<>();
        
        ex.getBindingResult().getFieldErrors().forEach(error -> {
            errors.put(error.getField(), error.getDefaultMessage());
        });
        
        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(errors);
    }
}
```

**Réponse d'erreur formatée :**
```json
{
  "name": "Name is required",
  "email": "Invalid email format",
  "age": "Must be at least 18 years old"
}
```

### **Validation imbriquée (nested objects)**

Pour valider des objets imbriqués, utiliser **@Valid** sur le champ :

```java
public class Order {
    
    @NotNull
    private Long id;
    
    @Valid  // Déclenche la validation sur l'objet Address
    @NotNull
    private Address address;
    
    @Valid
    @NotEmpty(message = "Order must have at least one item")
    private List<OrderItem> items;
}

public class Address {
    @NotBlank
    private String street;
    
    @NotBlank
    private String city;
}
```

### **Validation de groupe (Group Validation)**

Permet d'appliquer différentes validations selon le contexte :

```java
public class User {
    
    @NotNull(groups = UpdateValidation.class)
    private Long id;
    
    @NotBlank(groups = {CreateValidation.class, UpdateValidation.class})
    private String name;
    
    // Interfaces marqueur pour les groupes
    public interface CreateValidation {}
    public interface UpdateValidation {}
}

// Controller
@PostMapping
public ResponseEntity<User> create(
    @Validated(CreateValidation.class) @RequestBody User user) {
    // Validation pour création
}

@PutMapping("/{id}")
public ResponseEntity<User> update(
    @Validated(UpdateValidation.class) @RequestBody User user) {
    // Validation pour mise à jour
}
```

### **Custom Validator**

Créer une constraint personnalisée :

**1. Annotation :**
```java
@Target({ElementType.FIELD})
@Retention(RetentionPolicy.RUNTIME)
@Constraint(validatedBy = AgeValidator.class)
public @interface ValidAge {
    String message() default "Invalid age";
    Class<?>[] groups() default {};
    Class<? extends Payload>[] payload() default {};
}
```

**2. Validator :**
```java
public class AgeValidator implements ConstraintValidator<ValidAge, Integer> {
    
    @Override
    public boolean isValid(Integer age, ConstraintValidatorContext context) {
        if (age == null) return true;  // @NotNull gère le null
        return age >= 18 && age <= 100;
    }
}
```

**3. Utilisation :**
```java
public class User {
    @ValidAge(message = "Age must be between 18 and 100")
    private Integer age;
}
```

### **Dépendance Maven**

```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-validation</artifactId>
</dependency>
```

### **Points clés**

- **@Valid** déclenche la validation basée sur les constraints annotations
- **Bean Validation API (JSR-380)** avec Hibernate Validator
- **@NotNull** vs **@NotEmpty** vs **@NotBlank** : comprendre les différences
- **MethodArgumentNotValidException** levée en cas d'échec
- **@ControllerAdvice** pour gérer les erreurs globalement
- **Nested validation** avec @Valid sur les objets imbriqués
- **Custom validators** pour des règles métier complexes
- Validation automatique retourne **400 Bad Request**

---


## Questions/Réponses - Validation

### **1. Qu'est-ce que la validation dans Spring Boot et pourquoi est-elle importante ?**

**Réponse :**
La validation permet de vérifier que les données reçues dans une application (via REST API ou forms) respectent certaines **contraintes** avant de les traiter.

**Importance :**
- **Sécurité** : Empêche l'injection de données malveillantes
- **Intégrité des données** : Garantit que seules des données valides sont sauvegardées
- **User experience** : Retourne des messages d'erreur clairs
- **Business rules** : Applique les règles métier (ex: age >= 18)

Spring Boot utilise **Bean Validation API (JSR-380)** avec l'implémentation **Hibernate Validator**.

---

### **2. Comment fonctionne la validation dans Spring Boot ?**

**Réponse :**
**Flow de validation :**

1. Client envoie une request avec des données (JSON)
2. **@Valid** ou **@Validated** sur le paramètre du controller déclenche la validation
3. Spring vérifie toutes les **constraints annotations** (@NotNull, @Size, etc.)
4. Si validation **réussit** : les données sont traitées normalement
5. Si validation **échoue** : **MethodArgumentNotValidException** est levée et Spring retourne **400 Bad Request**

```java
@PostMapping("/users")
public ResponseEntity<User> create(@Valid @RequestBody User user) {
    // Validation automatique avant d'entrer dans la méthode
    return ResponseEntity.ok(userService.save(user));
}
```

---

### **3. Quelle est la différence entre @Valid et @Validated ?**

**Réponse :**

| @Valid | @Validated |
|--------|------------|
| Standard Java (JSR-380) | Spring-specific |
| Pas de support pour les groupes | Support des **validation groups** |
| Utilisé sur les paramètres de méthode | Peut être utilisé au niveau de la classe |
| Validation standard | Validation avancée avec groupes |

**Exemple @Valid :**
```java
@PostMapping("/users")
public User create(@Valid @RequestBody User user) { ... }
```

**Exemple @Validated avec groupes :**
```java
@PostMapping("/users")
public User create(@Validated(CreateGroup.class) @RequestBody User user) { ... }
```

**En pratique :** Utiliser **@Valid** pour la validation simple, **@Validated** pour les validation groups.

---

### **4. Quelles sont les principales annotations de validation ?**

**Réponse :**

**Null checks :**
- **@NotNull** : La valeur ne doit pas être null
- **@NotEmpty** : String/Collection non null ET non vide
- **@NotBlank** : String non null, non vide, et pas que des whitespaces

**Taille :**
- **@Size(min, max)** : Taille d'une String ou Collection
- **@Min(value)** : Valeur minimale (nombres)
- **@Max(value)** : Valeur maximale (nombres)

**Format :**
- **@Email** : Format email valide
- **@Pattern(regex)** : Correspond à une regex

**Nombres :**
- **@Positive** : Nombre > 0
- **@PositiveOrZero** : Nombre >= 0
- **@Negative** : Nombre < 0

**Dates :**
- **@Past** : Date dans le passé
- **@Future** : Date dans le futur
- **@PastOrPresent** : Date passée ou aujourd'hui

---

### **5. Quelle est la différence entre @NotNull, @NotEmpty et @NotBlank ?**

**Réponse :**

| Annotation | null | "" (empty) | "   " (whitespace) |
|------------|------|------------|-------------------|
| @NotNull | ❌ | ✅ | ✅ |
| @NotEmpty | ❌ | ❌ | ✅ |
| @NotBlank | ❌ | ❌ | ❌ |

**Exemples :**
```java
public class User {
    @NotNull
    private String username;  // null = invalid, "" = valid, "  " = valid
    
    @NotEmpty
    private String email;     // null = invalid, "" = invalid, "  " = valid
    
    @NotBlank
    private String name;      // null = invalid, "" = invalid, "  " = invalid
}
```

**Conseil :** Pour les Strings, utiliser **@NotBlank** car c'est la plus stricte.

---

### **6. Comment valider un objet dans un REST Controller ?**

**Réponse :**

**1. Ajouter la dépendance :**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-validation</artifactId>
</dependency>
```

**2. Définir les constraints dans l'entité :**
```java
public class User {
    
    @NotBlank(message = "Name is required")
    @Size(min = 2, max = 50, message = "Name must be between 2 and 50 characters")
    private String name;
    
    @Email(message = "Invalid email format")
    private String email;
    
    @Min(value = 18, message = "Must be at least 18 years old")
    private Integer age;
}
```

**3. Ajouter @Valid dans le controller :**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @PostMapping
    public ResponseEntity<User> create(@Valid @RequestBody User user) {
        User saved = userService.save(user);
        return ResponseEntity.status(HttpStatus.CREATED).body(saved);
    }
}
```

Si validation échoue → **400 Bad Request** automatiquement.

---

### **7. Comment personnaliser les messages d'erreur de validation ?**

**Réponse :**

**Option 1 : Dans l'annotation directement**
```java
@NotBlank(message = "Le nom est obligatoire")
@Size(min = 2, max = 50, message = "Le nom doit contenir entre {min} et {max} caractères")
private String name;
```

**Option 2 : Fichier messages.properties**
```properties
# messages.properties
user.name.notblank=Le nom est obligatoire
user.email.email=Format email invalide
```

```java
@NotBlank(message = "{user.name.notblank}")
private String name;

@Email(message = "{user.email.email}")
private String email;
```

**Option 3 : Messages avec variables**
```java
@Size(min = 2, max = 50, message = "Name must be between {min} and {max} characters")
// {min} et {max} sont remplacés automatiquement
```

---

### **8. Comment gérer les erreurs de validation globalement ?**

**Réponse :**
Utiliser **@ControllerAdvice** avec **@ExceptionHandler** :

```java
@ControllerAdvice
public class ValidationExceptionHandler {
    
    @ExceptionHandler(MethodArgumentNotValidException.class)
    public ResponseEntity<Map<String, String>> handleValidationErrors(
            MethodArgumentNotValidException ex) {
        
        Map<String, String> errors = new HashMap<>();
        
        // Récupérer tous les field errors
        ex.getBindingResult().getFieldErrors().forEach(error -> {
            errors.put(error.getField(), error.getDefaultMessage());
        });
        
        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(errors);
    }
}
```

**Réponse d'erreur formatée :**
```json
{
  "name": "Name is required",
  "email": "Invalid email format",
  "age": "Must be at least 18 years old"
}
```

---

### **9. Comment valider des objets imbriqués (nested objects) ?**

**Réponse :**
Ajouter **@Valid** sur le champ de l'objet imbriqué :

```java
public class Order {
    
    @NotNull(message = "Order ID is required")
    private Long id;
    
    @Valid  // Déclenche la validation sur Address
    @NotNull(message = "Address is required")
    private Address address;
    
    @Valid  // Validation sur chaque élément de la liste
    @NotEmpty(message = "Order must have at least one item")
    private List<OrderItem> items;
}

public class Address {
    @NotBlank(message = "Street is required")
    private String street;
    
    @NotBlank(message = "City is required")
    private String city;
    
    @Pattern(regexp = "^[0-9]{5}$", message = "Invalid postal code")
    private String postalCode;
}

public class OrderItem {
    @NotNull
    private Long productId;
    
    @Positive(message = "Quantity must be positive")
    private Integer quantity;
}
```

**Controller :**
```java
@PostMapping("/orders")
public ResponseEntity<Order> create(@Valid @RequestBody Order order) {
    // Valide Order + Address + tous les OrderItems
    return ResponseEntity.ok(orderService.save(order));
}
```

---

### **10. Qu'est-ce que la validation de groupe (Group Validation) ?**

**Réponse :**
Les **validation groups** permettent d'appliquer différentes validations selon le contexte (création vs mise à jour).

**Définir les groupes :**
```java
public class User {
    
    // ID requis seulement pour update
    @NotNull(groups = UpdateValidation.class)
    private Long id;
    
    // Name requis pour create ET update
    @NotBlank(groups = {CreateValidation.class, UpdateValidation.class})
    private String name;
    
    // Password requis seulement pour create
    @NotBlank(groups = CreateValidation.class)
    @Size(min = 8, groups = CreateValidation.class)
    private String password;
    
    // Interfaces marqueur
    public interface CreateValidation {}
    public interface UpdateValidation {}
}
```

**Controller :**
```java
@PostMapping
public User create(@Validated(CreateValidation.class) @RequestBody User user) {
    // Valide : name + password (pas id)
    return userService.save(user);
}

@PutMapping("/{id}")
public User update(@Validated(UpdateValidation.class) @RequestBody User user) {
    // Valide : id + name (pas password)
    return userService.update(user);
}
```

---

### **11. Comment créer une constraint de validation personnalisée (Custom Validator) ?**

**Réponse :**

**1. Créer l'annotation :**
```java
@Target({ElementType.FIELD})
@Retention(RetentionPolicy.RUNTIME)
@Constraint(validatedBy = PhoneValidator.class)
public @interface ValidPhone {
    String message() default "Invalid phone number";
    Class<?>[] groups() default {};
    Class<? extends Payload>[] payload() default {};
}
```

**2. Créer le validator :**
```java
public class PhoneValidator implements ConstraintValidator<ValidPhone, String> {
    
    @Override
    public void initialize(ValidPhone constraintAnnotation) {
        // Initialisation si nécessaire
    }
    
    @Override
    public boolean isValid(String phone, ConstraintValidatorContext context) {
        if (phone == null || phone.isEmpty()) {
            return true;  // @NotNull gère le null
        }
        
        // Logique de validation personnalisée
        return phone.matches("^\\+?[0-9]{10,15}$");
    }
}
```

**3. Utiliser l'annotation :**
```java
public class User {
    @ValidPhone(message = "Phone number must be 10-15 digits")
    private String phone;
}
```

---

### **12. Comment valider les @RequestParam et @PathVariable ?**

**Réponse :**
Ajouter **@Validated** au niveau de la classe controller :

```java
@RestController
@RequestMapping("/api/users")
@Validated  // Active la validation des paramètres
public class UserController {
    
    @GetMapping("/{id}")
    public User getById(
            @PathVariable 
            @Min(value = 1, message = "ID must be positive") 
            Long id) {
        return userService.findById(id);
    }
    
    @GetMapping
    public List<User> search(
            @RequestParam 
            @NotBlank(message = "Search query is required")
            @Size(min = 3, message = "Query must be at least 3 characters")
            String query) {
        return userService.search(query);
    }
}
```

**Exception levée :** **ConstraintViolationException** (pas MethodArgumentNotValidException)

**Handler :**
```java
@ExceptionHandler(ConstraintViolationException.class)
public ResponseEntity<Map<String, String>> handleConstraintViolation(
        ConstraintViolationException ex) {
    
    Map<String, String> errors = new HashMap<>();
    ex.getConstraintViolations().forEach(violation -> {
        errors.put(violation.getPropertyPath().toString(), 
                   violation.getMessage());
    });
    
    return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(errors);
}
```

---

### **13. Quelle exception est levée en cas d'échec de validation ?**

**Réponse :**
Dépend du contexte :

**1. @Valid sur @RequestBody :**
- **MethodArgumentNotValidException**
- Contient les field errors dans `BindingResult`

**2. @Validated sur @PathVariable/@RequestParam :**
- **ConstraintViolationException**
- Contient les constraint violations

**3. Validation manuelle (Validator) :**
- Pas d'exception automatique
- Retourne un Set<ConstraintViolation>

```java
@Autowired
private Validator validator;

public void validateManually(User user) {
    Set<ConstraintViolation<User>> violations = validator.validate(user);
    
    if (!violations.isEmpty()) {
        // Gérer les erreurs
        violations.forEach(v -> System.out.println(v.getMessage()));
    }
}
```

---

### **14. Comment valider conditionnellement (validation conditionnelle) ?**

**Réponse :**
Créer un **custom validator** avec logique conditionnelle :

```java
@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
@Constraint(validatedBy = AddressValidator.class)
public @interface ValidAddress {
    String message() default "Invalid address";
    Class<?>[] groups() default {};
    Class<? extends Payload>[] payload() default {};
}

public class AddressValidator implements ConstraintValidator<ValidAddress, User> {
    
    @Override
    public boolean isValid(User user, ConstraintValidatorContext context) {
        // Si le pays est France, le code postal est obligatoire
        if ("France".equals(user.getCountry())) {
            return user.getPostalCode() != null && 
                   user.getPostalCode().matches("^[0-9]{5}$");
        }
        return true;  // Pas de validation pour autres pays
    }
}

@ValidAddress  // Au niveau de la classe
public class User {
    private String country;
    private String postalCode;
}
```

---

### **15. Comment valider une List ou un Collection ?**

**Réponse :**

**Valider la collection elle-même :**
```java
public class Order {
    
    @NotEmpty(message = "Order must have at least one item")
    @Size(min = 1, max = 100, message = "Order can have 1-100 items")
    private List<OrderItem> items;
}
```

**Valider chaque élément de la collection :**
```java
public class Order {
    
    @Valid  // Valide chaque OrderItem
    @NotEmpty
    private List<OrderItem> items;
}

public class OrderItem {
    @NotNull
    private Long productId;
    
    @Positive
    private Integer quantity;
}
```

---

### **16. Comment tester la validation dans les tests unitaires ?**

**Réponse :**

```java
@SpringBootTest
class UserValidationTest {
    
    @Autowired
    private Validator validator;
    
    @Test
    void testInvalidUser() {
        User user = new User();
        user.setName("");  // Invalid: blank
        user.setEmail("invalid-email");  // Invalid: format
        user.setAge(15);  // Invalid: < 18
        
        Set<ConstraintViolation<User>> violations = validator.validate(user);
        
        assertEquals(3, violations.size());  // 3 erreurs
        
        // Vérifier les messages
        assertTrue(violations.stream()
            .anyMatch(v -> v.getMessage().equals("Name is required")));
    }
    
    @Test
    void testValidUser() {
        User user = new User();
        user.setName("John Doe");
        user.setEmail("john@example.com");
        user.setAge(25);
        
        Set<ConstraintViolation<User>> violations = validator.validate(user);
        
        assertTrue(violations.isEmpty());  // Aucune erreur
    }
}
```

---

### **17. Quelle est la différence entre validation côté client et côté serveur ?**

**Réponse :**

**Validation côté client (Frontend - JavaScript) :**
- **Avantage** : User experience immédiate, réduit les appels serveur
- **Inconvénient** : Peut être contournée (inspect element, Postman)
- **Usage** : Feedback rapide pour l'utilisateur

**Validation côté serveur (Backend - Spring) :**
- **Avantage** : Sécurisée, ne peut pas être contournée
- **Inconvénient** : Nécessite un round-trip serveur
- **Usage** : Validation authoritative, obligatoire

**Best practice :** **TOUJOURS valider côté serveur** (sécurité), et optionnellement côté client (UX).

---

### **18. Comment valider des données JSON complexes ?**

**Réponse :**
Utiliser **nested validation** avec @Valid :

```java
public class ComplexRequest {
    
    @Valid
    @NotNull
    private PersonalInfo personalInfo;
    
    @Valid
    @NotNull
    private Address address;
    
    @Valid
    @NotEmpty
    private List<PaymentMethod> paymentMethods;
}

public class PersonalInfo {
    @NotBlank
    private String firstName;
    
    @NotBlank
    private String lastName;
    
    @Email
    private String email;
}

public class Address {
    @NotBlank
    private String street;
    
    @NotBlank
    private String city;
    
    @Valid
    private Coordinates coordinates;
}

public class Coordinates {
    @NotNull
    @DecimalMin("-90.0")
    @DecimalMax("90.0")
    private Double latitude;
    
    @NotNull
    @DecimalMin("-180.0")
    @DecimalMax("180.0")
    private Double longitude;
}
```

Toute la hiérarchie sera validée automatiquement avec un seul `@Valid` dans le controller.

---

### **19. Comment désactiver la validation pour certains endpoints ?**

**Réponse :**
Simplement ne pas utiliser `@Valid` ou `@Validated` :

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @PostMapping
    public User create(@Valid @RequestBody User user) {
        // Validation activée
        return userService.save(user);
    }
    
    @PostMapping("/import")
    public List<User> importUsers(@RequestBody List<User> users) {
        // Pas de validation (pour import en masse)
        return userService.saveAll(users);
    }
}
```

---

### **20. Quelles sont les best practices pour la validation ?**

**Réponse :**

**1. Toujours valider côté serveur**
```java
@PostMapping
public User create(@Valid @RequestBody User user) { ... }
```

**2. Utiliser des messages clairs et explicites**
```java
@NotBlank(message = "Name is required and cannot be empty")
```

**3. Valider les nested objects**
```java
@Valid
private Address address;
```

**4. Gérer les erreurs globalement**
```java
@ControllerAdvice
public class ValidationExceptionHandler { ... }
```

**5. Utiliser @NotBlank pour les Strings (plus strict)**
```java
@NotBlank  // Préférer à @NotEmpty ou @NotNull
private String name;
```

**6. Grouper les validations par contexte**
```java
@Validated(CreateValidation.class)
```

**7. Créer des custom validators pour les règles métier complexes**
```java
@ValidBusinessRule
private BusinessData data;
```

**8. Tester la validation dans les tests unitaires**
```java
Set<ConstraintViolation<User>> violations = validator.validate(user);
```

**9. Ne jamais faire confiance aux données entrantes**
- Même si validation frontend existe

**10. Logger les erreurs de validation pour analyse**
```java
log.warn("Validation failed for user: {}", errors);
```

---