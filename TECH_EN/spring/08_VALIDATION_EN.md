
## [**..**](./00_index.md)

## Validation - Key Points

### **General Concept**
Validation allows checking that data received (in HTTP requests) follows certain **constraints** before processing it. Spring Boot uses **Bean Validation API (JSR-380)** with the **Hibernate Validator** implementation.

### **How it works**

**Validation flow:**
1. Client sends a request with data (JSON, form data)
2. **@Valid** or **@Validated** triggers validation on the object
3. The **constraint annotations** (@NotNull, @Size, etc.) are checked
4. If validation fails → **MethodArgumentNotValidException** is thrown
5. Otherwise, the data is processed normally

### **Main Annotations**

**@Valid:**
- Triggers validation on an object
- Used on controller method parameters
- Checks all constraints defined in the class

```java
@PostMapping("/users")
public ResponseEntity<User> create(@Valid @RequestBody User user) {
    // If validation fails, exception is thrown automatically
    return ResponseEntity.ok(userService.save(user));
}
```

**Common Constraints:**

| Annotation | Description |
|------------|-------------|
| `@NotNull` | Value must not be null |
| `@NotEmpty` | String/Collection must not be empty (null or empty) |
| `@NotBlank` | String must not be empty or whitespace only |
| `@Size(min, max)` | Size of a String or Collection |
| `@Min(value)` | Minimum value for a number |
| `@Max(value)` | Maximum value for a number |
| `@Email` | Validates an email format |
| `@Pattern(regex)` | Validates with a regex |
| `@Positive` | Positive number (> 0) |
| `@PositiveOrZero` | Positive number or zero (>= 0) |
| `@Past` | Date in the past |
| `@Future` | Date in the future |

### **Full Example**

**Entity with constraints:**
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

**Controller:**
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

### **Handling Validation Errors**

**Without a custom handler:**
- Spring automatically returns a **400 Bad Request** with error details

**With @ControllerAdvice (recommended):**
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

**Formatted error response:**
```json
{
  "name": "Name is required",
  "email": "Invalid email format",
  "age": "Must be at least 18 years old"
}
```

### **Nested Object Validation**

To validate nested objects, use **@Valid** on the field:

```java
public class Order {
    
    @NotNull
    private Long id;
    
    @Valid  // Triggers validation on the Address object
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

### **Group Validation**

Allows applying different validations depending on the context:

```java
public class User {
    
    @NotNull(groups = UpdateValidation.class)
    private Long id;
    
    @NotBlank(groups = {CreateValidation.class, UpdateValidation.class})
    private String name;
    
    // Marker interfaces for groups
    public interface CreateValidation {}
    public interface UpdateValidation {}
}

// Controller
@PostMapping
public ResponseEntity<User> create(
    @Validated(CreateValidation.class) @RequestBody User user) {
    // Validation for creation
}

@PutMapping("/{id}")
public ResponseEntity<User> update(
    @Validated(UpdateValidation.class) @RequestBody User user) {
    // Validation for update
}
```

### **Custom Validator**

Create a custom constraint:

**1. Annotation:**
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

**2. Validator:**
```java
public class AgeValidator implements ConstraintValidator<ValidAge, Integer> {
    
    @Override
    public boolean isValid(Integer age, ConstraintValidatorContext context) {
        if (age == null) return true;  // @NotNull handles null
        return age >= 18 && age <= 100;
    }
}
```

**3. Usage:**
```java
public class User {
    @ValidAge(message = "Age must be between 18 and 100")
    private Integer age;
}
```

### **Maven Dependency**

```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-validation</artifactId>
</dependency>
```

### **Key Points**

- **@Valid** triggers validation based on constraint annotations
- **Bean Validation API (JSR-380)** with Hibernate Validator
- **@NotNull** vs **@NotEmpty** vs **@NotBlank** : understand the differences
- **MethodArgumentNotValidException** thrown on failure
- **@ControllerAdvice** to handle errors globally
- **Nested validation** with @Valid on nested objects
- **Custom validators** for complex business rules
- Automatic validation returns **400 Bad Request**

---


## Questions/Answers - Validation

### **1. What is validation in Spring Boot and why is it important?**

**Answer:**
Validation allows checking that data received by an application (via REST API or forms) follows certain **constraints** before processing it.

**Importance:**
- **Security**: Prevents injection of malicious data
- **Data integrity**: Ensures only valid data is saved
- **User experience**: Returns clear error messages
- **Business rules**: Enforces business rules (e.g.: age >= 18)

Spring Boot uses **Bean Validation API (JSR-380)** with the **Hibernate Validator** implementation.

---

### **2. How does validation work in Spring Boot?**

**Answer:**
**Validation flow:**

1. Client sends a request with data (JSON)
2. **@Valid** or **@Validated** on the controller parameter triggers validation
3. Spring checks all **constraint annotations** (@NotNull, @Size, etc.)
4. If validation **succeeds**: data is processed normally
5. If validation **fails**: **MethodArgumentNotValidException** is thrown and Spring returns **400 Bad Request**

```java
@PostMapping("/users")
public ResponseEntity<User> create(@Valid @RequestBody User user) {
    // Automatic validation before entering the method
    return ResponseEntity.ok(userService.save(user));
}
```

---

### **3. What is the difference between @Valid and @Validated?**

**Answer:**

| @Valid | @Validated |
|--------|------------|
| Standard Java (JSR-380) | Spring-specific |
| No support for groups | Supports **validation groups** |
| Used on method parameters | Can be used at class level |
| Standard validation | Advanced validation with groups |

**@Valid example:**
```java
@PostMapping("/users")
public User create(@Valid @RequestBody User user) { ... }
```

**@Validated with groups example:**
```java
@PostMapping("/users")
public User create(@Validated(CreateGroup.class) @RequestBody User user) { ... }
```

**In practice:** Use **@Valid** for simple validation, **@Validated** for validation groups.

---

### **4. What are the main validation annotations?**

**Answer:**

**Null checks:**
- **@NotNull** : Value must not be null
- **@NotEmpty** : String/Collection not null AND not empty
- **@NotBlank** : String not null, not empty, and not just whitespace

**Size:**
- **@Size(min, max)** : Size of a String or Collection
- **@Min(value)** : Minimum value (numbers)
- **@Max(value)** : Maximum value (numbers)

**Format:**
- **@Email** : Valid email format
- **@Pattern(regex)** : Matches a regex

**Numbers:**
- **@Positive** : Number > 0
- **@PositiveOrZero** : Number >= 0
- **@Negative** : Number < 0

**Dates:**
- **@Past** : Date in the past
- **@Future** : Date in the future
- **@PastOrPresent** : Past date or today

---

### **5. What is the difference between @NotNull, @NotEmpty and @NotBlank?**

**Answer:**

| Annotation | null | "" (empty) | "   " (whitespace) |
|------------|------|------------|-------------------|
| @NotNull | ❌ | ✅ | ✅ |
| @NotEmpty | ❌ | ❌ | ✅ |
| @NotBlank | ❌ | ❌ | ❌ |

**Examples:**
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

**Tip:** For Strings, use **@NotBlank** because it's the strictest.

---

### **6. How to validate an object in a REST Controller?**

**Answer:**

**1. Add the dependency:**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-validation</artifactId>
</dependency>
```

**2. Define constraints in the entity:**
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

**3. Add @Valid in the controller:**
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

If validation fails → **400 Bad Request** automatically.

---

### **7. How to customize validation error messages?**

**Answer:**

**Option 1: Directly in the annotation**
```java
@NotBlank(message = "Name is required")
@Size(min = 2, max = 50, message = "Name must be between {min} and {max} characters")
private String name;
```

**Option 2: messages.properties file**
```properties
# messages.properties
user.name.notblank=Name is required
user.email.email=Invalid email format
```

```java
@NotBlank(message = "{user.name.notblank}")
private String name;

@Email(message = "{user.email.email}")
private String email;
```

**Option 3: Messages with variables**
```java
@Size(min = 2, max = 50, message = "Name must be between {min} and {max} characters")
// {min} and {max} are replaced automatically
```

---

### **8. How to handle validation errors globally?**

**Answer:**
Use **@ControllerAdvice** with **@ExceptionHandler**:

```java
@ControllerAdvice
public class ValidationExceptionHandler {
    
    @ExceptionHandler(MethodArgumentNotValidException.class)
    public ResponseEntity<Map<String, String>> handleValidationErrors(
            MethodArgumentNotValidException ex) {
        
        Map<String, String> errors = new HashMap<>();
        
        // Get all field errors
        ex.getBindingResult().getFieldErrors().forEach(error -> {
            errors.put(error.getField(), error.getDefaultMessage());
        });
        
        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(errors);
    }
}
```

**Formatted error response:**
```json
{
  "name": "Name is required",
  "email": "Invalid email format",
  "age": "Must be at least 18 years old"
}
```

---

### **9. How to validate nested objects?**

**Answer:**
Add **@Valid** on the nested object field:

```java
public class Order {
    
    @NotNull(message = "Order ID is required")
    private Long id;
    
    @Valid  // Triggers validation on Address
    @NotNull(message = "Address is required")
    private Address address;
    
    @Valid  // Validates each element in the list
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

**Controller:**
```java
@PostMapping("/orders")
public ResponseEntity<Order> create(@Valid @RequestBody Order order) {
    // Validates Order + Address + all OrderItems
    return ResponseEntity.ok(orderService.save(order));
}
```

---

### **10. What is Group Validation?**

**Answer:**
**Validation groups** allow applying different validations depending on the context (create vs update).

**Define the groups:**
```java
public class User {
    
    // ID required only for update
    @NotNull(groups = UpdateValidation.class)
    private Long id;
    
    // Name required for create AND update
    @NotBlank(groups = {CreateValidation.class, UpdateValidation.class})
    private String name;
    
    // Password required only for create
    @NotBlank(groups = CreateValidation.class)
    @Size(min = 8, groups = CreateValidation.class)
    private String password;
    
    // Marker interfaces
    public interface CreateValidation {}
    public interface UpdateValidation {}
}
```

**Controller:**
```java
@PostMapping
public User create(@Validated(CreateValidation.class) @RequestBody User user) {
    // Validates: name + password (not id)
    return userService.save(user);
}

@PutMapping("/{id}")
public User update(@Validated(UpdateValidation.class) @RequestBody User user) {
    // Validates: id + name (not password)
    return userService.update(user);
}
```

---

### **11. How to create a custom validation constraint (Custom Validator)?**

**Answer:**

**1. Create the annotation:**
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

**2. Create the validator:**
```java
public class PhoneValidator implements ConstraintValidator<ValidPhone, String> {
    
    @Override
    public void initialize(ValidPhone constraintAnnotation) {
        // Initialization if needed
    }
    
    @Override
    public boolean isValid(String phone, ConstraintValidatorContext context) {
        if (phone == null || phone.isEmpty()) {
            return true;  // @NotNull handles null
        }
        
        // Custom validation logic
        return phone.matches("^\\+?[0-9]{10,15}$");
    }
}
```

**3. Use the annotation:**
```java
public class User {
    @ValidPhone(message = "Phone number must be 10-15 digits")
    private String phone;
}
```

---

### **12. How to validate @RequestParam and @PathVariable?**

**Answer:**
Add **@Validated** at the controller class level:

```java
@RestController
@RequestMapping("/api/users")
@Validated  // Enables parameter validation
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

**Exception thrown:** **ConstraintViolationException** (not MethodArgumentNotValidException)

**Handler:**
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

### **13. What exception is thrown when validation fails?**

**Answer:**
Depends on the context:

**1. @Valid on @RequestBody:**
- **MethodArgumentNotValidException**
- Contains field errors in `BindingResult`

**2. @Validated on @PathVariable/@RequestParam:**
- **ConstraintViolationException**
- Contains constraint violations

**3. Manual validation (Validator):**
- No automatic exception
- Returns a Set<ConstraintViolation>

```java
@Autowired
private Validator validator;

public void validateManually(User user) {
    Set<ConstraintViolation<User>> violations = validator.validate(user);
    
    if (!violations.isEmpty()) {
        // Handle errors
        violations.forEach(v -> System.out.println(v.getMessage()));
    }
}
```

---

### **14. How to do conditional validation?**

**Answer:**
Create a **custom validator** with conditional logic:

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
        // If the country is France, postal code is required
        if ("France".equals(user.getCountry())) {
            return user.getPostalCode() != null && 
                   user.getPostalCode().matches("^[0-9]{5}$");
        }
        return true;  // No validation for other countries
    }
}

@ValidAddress  // At class level
public class User {
    private String country;
    private String postalCode;
}
```

---

### **15. How to validate a List or Collection?**

**Answer:**

**Validate the collection itself:**
```java
public class Order {
    
    @NotEmpty(message = "Order must have at least one item")
    @Size(min = 1, max = 100, message = "Order can have 1-100 items")
    private List<OrderItem> items;
}
```

**Validate each element in the collection:**
```java
public class Order {
    
    @Valid  // Validates each OrderItem
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

### **16. How to test validation in unit tests?**

**Answer:**

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
        
        assertEquals(3, violations.size());  // 3 errors
        
        // Check messages
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
        
        assertTrue(violations.isEmpty());  // No errors
    }
}
```

---

### **17. What is the difference between client-side and server-side validation?**

**Answer:**

**Client-side validation (Frontend - JavaScript):**
- **Advantage**: Immediate user experience, reduces server calls
- **Disadvantage**: Can be bypassed (inspect element, Postman)
- **Usage**: Quick feedback for the user

**Server-side validation (Backend - Spring):**
- **Advantage**: Secure, cannot be bypassed
- **Disadvantage**: Requires a server round-trip
- **Usage**: Authoritative validation, mandatory

**Best practice:** **ALWAYS validate server-side** (security), and optionally client-side (UX).

---

### **18. How to validate complex JSON data?**

**Answer:**
Use **nested validation** with @Valid:

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

The entire hierarchy will be validated automatically with a single `@Valid` in the controller.

---

### **19. How to disable validation for certain endpoints?**

**Answer:**
Simply don't use `@Valid` or `@Validated`:

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @PostMapping
    public User create(@Valid @RequestBody User user) {
        // Validation enabled
        return userService.save(user);
    }
    
    @PostMapping("/import")
    public List<User> importUsers(@RequestBody List<User> users) {
        // No validation (for bulk import)
        return userService.saveAll(users);
    }
}
```

---

### **20. What are the best practices for validation?**

**Answer:**

**1. Always validate server-side**
```java
@PostMapping
public User create(@Valid @RequestBody User user) { ... }
```

**2. Use clear and explicit messages**
```java
@NotBlank(message = "Name is required and cannot be empty")
```

**3. Validate nested objects**
```java
@Valid
private Address address;
```

**4. Handle errors globally**
```java
@ControllerAdvice
public class ValidationExceptionHandler { ... }
```

**5. Use @NotBlank for Strings (stricter)**
```java
@NotBlank  // Prefer over @NotEmpty or @NotNull
private String name;
```

**6. Group validations by context**
```java
@Validated(CreateValidation.class)
```

**7. Create custom validators for complex business rules**
```java
@ValidBusinessRule
private BusinessData data;
```

**8. Test validation in unit tests**
```java
Set<ConstraintViolation<User>> violations = validator.validate(user);
```

**9. Never trust incoming data**
- Even if frontend validation exists

**10. Log validation errors for analysis**
```java
log.warn("Validation failed for user: {}", errors);
```

---