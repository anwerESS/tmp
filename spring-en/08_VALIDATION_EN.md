
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

# Questions/Answers - Chapter 7: Beyond Classes

## Questions about Interfaces

**Q1: Does this code compile?**
```java
public interface Example {
    void method();
}
```

**A:** Yes, it compiles.

Interface methods are **public abstract** by default. It is equivalent to:
```java
public interface Example {
    public abstract void method();
}
```

---

**Q2: Does this code compile?**
```java
public interface Example {
    private void method() {
        System.out.println("Hello");
    }
}
```

**A:** Yes, it compiles (Java 9+).

**Private methods** are allowed in interfaces since Java 9 (as helpers for default methods).

---

**Q3: Does this code compile?**
```java
public interface Example {
    int x;
}
```

**A:** No, **compilation error**: "variable x might not have been initialized".

Interface variables must be **initialized** (they are `public static final` by default).

**Correct**:
```java
public interface Example {
    int x = 10;  // OK
}
```

---

**Q4: What are these variables?**
```java
public interface Config {
    int MAX = 100;
    String NAME = "App";
}
```

**A:** They are **constants** (implicitly `public static final`).

Equivalent to:
```java
public interface Config {
    public static final int MAX = 100;
    public static final String NAME = "App";
}

// Usage
System.out.println(Config.MAX);   // 100
System.out.println(Config.NAME);  // "App"
```

---

**Q5: Does this code compile?**
```java
public interface A { }
public interface B { }
public interface C extends A, B { }
```

**A:** Yes, it compiles.

An **interface** can extend **multiple interfaces** (multiple inheritance).

---

**Q6: Does this code compile?**
```java
public class A { }
public class B { }
public class C extends A, B { }
```

**A:** No, **compilation error**: "'{' expected".

A **class** can only extend **one class** (single inheritance).

---

**Q7: Does this code compile?**
```java
public interface Volant {
    void voler();
}

public class Oiseau implements Volant {
    // No implementation
}
```

**A:** No, **compilation error**: "Oiseau is not abstract and does not override abstract method voler() in Volant".

A **concrete class** must implement **all** abstract methods of the interface.

**Correct**:
```java
public class Oiseau implements Volant {
    @Override
    public void voler() {
        System.out.println("Flying");
    }
}
```

---

**Q8: What is the difference between these two?**
```java
// Version 1
public interface A {
    void method();
}

// Version 2
public interface B {
    default void method() {
        System.out.println("Default");
    }
}
```

**A:**
- **Interface A**: **abstract** method (no implementation), classes must implement it
- **Interface B**: **default method** (with implementation), classes can use it or override it

```java
class Test1 implements A {
    @Override
    public void method() { }  // Required
}

class Test2 implements B {
    // No need to override, inherits the default method
}
```

---

**Q9: How to resolve this conflict?**
```java
public interface A {
    default void method() {
        System.out.println("A");
    }
}

public interface B {
    default void method() {
        System.out.println("B");
    }
}

public class C implements A, B {
    // ?
}
```

**A:** **Compilation error** if the conflict is not resolved.

**Solutions**:
```java
public class C implements A, B {
    @Override
    public void method() {
        A.super.method();  // Calls A's version
        // OR
        B.super.method();  // Calls B's version
        // OR
        System.out.println("C");  // Own implementation
    }
}
```

---

**Q10: Are interface static methods inherited?**
```java
public interface Calculateur {
    static int add(int a, int b) {
        return a + b;
    }
}

public class MyClass implements Calculateur {
    public void test() {
        int result = add(5, 3);  // ?
    }
}
```

**A:** No, **compilation error**: "cannot find symbol: method add".

**Static methods** of an interface are **not inherited**. They must be called via the interface name:

```java
int result = Calculateur.add(5, 3);  // OK
```

---

## Questions about Enums

**Q11: What does this method return?**
```java
public enum Jour {
    LUNDI, MARDI, MERCREDI
}

Jour jour = Jour.MARDI;
System.out.println(jour.ordinal());
```

**A:** `1`

`ordinal()` returns the **position** (0-based):
- LUNDI = 0
- MARDI = 1
- MERCREDI = 2

---

**Q12: Does this code compile?**
```java
public enum Couleur {
    ROUGE, VERT, BLEU;
    
    private int code;
}
```

**A:** No, **compilation error**: "missing method body, or declare abstract".

If you declare a field, you must provide a **constructor**.

**Correct**:
```java
public enum Couleur {
    ROUGE(1), VERT(2), BLEU(3);
    
    private int code;
    
    Couleur(int code) {
        this.code = code;
    }
    
    public int getCode() {
        return code;
    }
}
```

---

**Q13: Does this code compile?**
```java
public enum Jour {
    LUNDI, MARDI;
    
    public Jour() {
        System.out.println("Constructor");
    }
}
```

**A:** No, **compilation error**: "modifier public not allowed here".

An enum constructor must be **private** (or package-private by default).

**Correct**:
```java
private Jour() {  // or without modifier
    System.out.println("Constructor");
}
```

---

**Q14: What is printed?**
```java
public enum Jour {
    LUNDI, MARDI, MERCREDI
}

for (Jour j : Jour.values()) {
    System.out.println(j);
}
```

**A:**
```
LUNDI
MARDI
MERCREDI
```

`values()` returns an **array** of all enum constants.

---

**Q15: What happens?**
```java
Jour jour = Jour.valueOf("LUNDI");    // ?
Jour jour2 = Jour.valueOf("lundi");   // ?
```

**A:**
```java
jour   // OK: LUNDI
jour2  // IllegalArgumentException (case-sensitive)
```

`valueOf()` is **case-sensitive** and throws an exception if the value doesn't exist.

---

**Q16: Does this code compile?**
```java
public enum Operation {
    ADDITION {
        public int calculate(int a, int b) {
            return a + b;
        }
    };
    
    public abstract int calculate(int a, int b);
}
```

**A:** No, **compilation error**: "enum types cannot have abstract methods".

If an enum has an **abstract method**, **all** constants must implement it.

**Correct**:
```java
public enum Operation {
    ADDITION {
        public int calculate(int a, int b) {
            return a + b;
        }
    },
    SOUSTRACTION {
        public int calculate(int a, int b) {
            return a - b;
        }
    };
    
    public abstract int calculate(int a, int b);
}
```

---

**Q17: Can you create an enum with new?**
```java
Jour jour = new Jour();
```

**A:** No, **compilation error**: "enum types may not be instantiated".

Enums **cannot** be instantiated with `new`. You use the predefined constants.

---

**Q18: What is the difference?**
```java
Jour jour = Jour.LUNDI;
System.out.println(jour.name());
System.out.println(jour.toString());
```

**A:** By default, **no difference**. Both print `"LUNDI"`.

But `toString()` can be **overridden**, not `name()`:
```java
public enum Jour {
    LUNDI, MARDI;
    
    @Override
    public String toString() {
        return "Day: " + name();
    }
}

Jour j = Jour.LUNDI;
System.out.println(j.name());      // "LUNDI"
System.out.println(j.toString());  // "Day: LUNDI"
```

---

## Questions about Sealed Classes

**Q19: Does this code compile? (Java 17+)**
```java
public sealed class Animal permits Chien, Chat {
}

public class Chien extends Animal { }
public class Chat extends Animal { }
```

**A:** No, **compilation error**: "class Chien must be declared either final, sealed, or non-sealed".

Subclasses of a sealed class must be **final**, **sealed**, or **non-sealed**.

**Correct**:
```java
public final class Chien extends Animal { }
public final class Chat extends Animal { }
```

---

**Q20: What is the difference between these three?**
```java
public sealed class A permits B, C, D { }

public final class B extends A { }
public sealed class C extends A permits E { }
public non-sealed class D extends A { }
```

**A:**
- **B (final)**: **cannot** be extended
- **C (sealed)**: controls its own subclasses (must specify `permits`)
- **D (non-sealed)**: **opens** inheritance (anyone can extend it)

```java
// public class F extends B { }  // ERROR: B is final
public class E extends C { }     // OK if permitted by C
public class F extends D { }     // OK: D is non-sealed
```

---

**Q21: Why use sealed classes?**

**A:** **Advantages**:
1. **Hierarchy control**: limits possible subtypes
2. **Exhaustive pattern matching**: the compiler knows all possible types
3. **Documentation**: permitted subtypes are explicit
4. **Security**: prevents unwanted extension

**Example**:
```java
public sealed interface Result permits Success, Error { }

public record Success(String data) implements Result { }
public record Error(String message) implements Result { }

// Exhaustive pattern matching (no need for default)
public static void handle(Result r) {
    switch (r) {
        case Success s -> System.out.println(s.data());
        case Error e -> System.out.println(e.message());
    }
}
```

---

## Questions about Records

**Q22: What does this record generate automatically?**
```java
public record Point(int x, int y) { }
```

**A:** Generates automatically:
1. **Constructor**: `public Point(int x, int y)`
2. **Getters**: `x()` and `y()` (not `getX()`)
3. **equals()**: value-based comparison
4. **hashCode()**: based on values
5. **toString()**: format `Point[x=3, y=4]`
6. All fields are **private final**

---

**Q23: Does this code compile?**
```java
public record Point(int x, int y) {
    private int z;
}
```

**A:** No, **compilation error**: "record cannot declare instance fields".

A record **cannot** declare **instance variables** outside the header.

**Allowed** (static variables):
```java
public record Point(int x, int y) {
    private static int counter;  // OK: static
}
```

---

**Q24: How to validate data in a record?**
```java
public record Point(int x, int y) {
    // ?
}
```

**A:** Use a **compact constructor**:
```java
public record Point(int x, int y) {
    public Point {  // Compact constructor
        if (x < 0 || y < 0) {
            throw new IllegalArgumentException("Negative coordinates");
        }
    }
}
```

---

**Q25: Is this record immutable?**
```java
public record Person(String name, List<String> emails) { }

List<String> list = new ArrayList<>();
list.add("a@mail.com");
Person p = new Person("Alice", list);
list.add("b@mail.com");  // ?
```

**A:** No, **not completely immutable**.

The problem: the external list can be modified after creation.

**Solution** (defensive copy):
```java
public record Person(String name, List<String> emails) {
    public Person {
        emails = List.copyOf(emails);  // Immutable copy
    }
}
```

---

**Q26: Does this code compile?**
```java
public record Point(int x, int y) {
    public void setX(int x) {
        this.x = x;
    }
}
```

**A:** No, **compilation error**: "cannot assign a value to final variable x".

Record fields are **final** (immutable). You cannot create setters.

---

**Q27: Can you extend a record?**
```java
public record Point(int x, int y) { }
public record Point3D(int x, int y, int z) extends Point { }
```

**A:** No, **compilation error**: "no extends clause allowed for record".

Records are **implicitly final** and **cannot** be extended (nor extend other classes).

But they can **implement interfaces**:
```java
public record Point(int x, int y) implements Comparable<Point> {
    @Override
    public int compareTo(Point other) {
        return Integer.compare(this.x, other.x);
    }
}
```

---

**Q28: What is the difference between these getters?**
```java
public record Person(String name) {
    public String name() {       // ?
        return name;
    }
    
    public String getName() {    // ?
        return name;
    }
}
```

**A:**
- `name()`: **Getter generated** automatically by the record
- `getName()`: **Additional custom method**

Records generate getters **without** the "get" prefix (modern style).

---

## Questions about Nested Classes

**Q29: What is the difference between an inner class and a static nested class?**
```java
public class Outer {
    private int x = 10;
    
    // Inner class
    public class Inner {
        public void method() {
            System.out.println(x);  // ?
        }
    }
    
    // Static nested class
    public static class Nested {
        public void method() {
            // System.out.println(x);  // ?
        }
    }
}
```

**A:**
- **Inner class**: can access `x` (instance members of the outer class)
- **Static nested class**: **cannot** access `x` (only static members)

---

**Q30: How to create an inner class from outside?**
```java
public class Outer {
    public class Inner { }
}

// Creation?
```

**A:** Requires an **instance** of the outer class:
```java
Outer ext = new Outer();
Outer.Inner inner = ext.new Inner();  // Special syntax
```

For a **static nested class**:
```java
Outer.Nested nested = new Outer.Nested();  // No instance needed
```

---

**Q31: Does this code compile?**
```java
public class Outer {
    public class Inner {
        public static void method() {
            System.out.println("Hello");
        }
    }
}
```

**A:** No, **compilation error**: "illegal static declaration in inner class".

An **inner class** (non-static) **cannot** have **static** members (except constants).

**Correct** (with static nested):
```java
public static class Nested {
    public static void method() {  // OK
        System.out.println("Hello");
    }
}
```

---

**Q32: What can local classes access?**
```java
public class Example {
    private int x = 10;
    
    public void method() {
        int y = 20;
        final int z = 30;
        
        class Local {
            public void display() {
                System.out.println(x);  // ?
                System.out.println(y);  // ?
                System.out.println(z);  // ?
            }
        }
    }
}
```

**A:** A **local class** can access:
- ✅ `x`: members of the outer class
- ✅ `y`: **effectively final** local variables (never modified)
- ✅ `z`: **final** local variables

If `y` was modified after its declaration, compilation error.

---

**Q33: What is the syntax for an anonymous class?**
```java
public interface Calculator {
    int calculate(int a, int b);
}

// Creation?
```

**A:**
```java
Calculator calc = new Calculator() {
    @Override
    public int calculate(int a, int b) {
        return a + b;
    }
};

int result = calc.calculate(5, 3);  // 8
```

Syntax: `new TypeOrInterface() { implementation }`

---

**Q34: Can you give a name to an anonymous class?**
```java
Runnable r = new Runnable MyRunnable() {
    @Override
    public void run() { }
};
```

**A:** No, **compilation error**.

An **anonymous class** has **no name** by definition. That's why it's called "anonymous".

---

## Questions about Polymorphism

**Q35: What is printed?**
```java
public class Animal {
    public void eat() {
        System.out.println("Animal eats");
    }
}

public class Dog extends Animal {
    @Override
    public void eat() {
        System.out.println("Dog eats");
    }
    
    public void bark() {
        System.out.println("Woof!");
    }
}

Animal a = new Dog();
a.eat();
a.bark();
```

**A:**
```java
a.eat();   // "Dog eats" (polymorphism)
a.bark();  // Compilation ERROR: Animal doesn't have bark()
```

The method called depends on the **object type** (Dog), but **compilation** checks the **reference type** (Animal).

---

**Q36: How to call bark()?**
```java
Animal a = new Dog();
// a.bark();  // Error

// Solution?
```

**A:** **Cast** to the real type:
```java
((Dog)a).bark();  // OK: "Woof!"

// Or with check
if (a instanceof Dog dog) {  // Pattern matching (Java 16+)
    dog.bark();
}
```

---

**Q37: What happens?**
```java
Animal a = new Cat();
Dog c = (Dog)a;
```

**A:** **ClassCastException** at runtime.

The real object is a `Cat`, you cannot cast it to `Dog`.

**Solution** (check before):
```java
if (a instanceof Dog) {
    Dog c = (Dog)a;
} else {
    System.out.println("Not a dog");
}
```

---

**Q38: Which method is called?**
```java
public class Parent {
    public static void method() {
        System.out.println("Parent");
    }
}

public class Child extends Parent {
    public static void method() {
        System.out.println("Child");
    }
}

Parent p = new Child();
p.method();
```

**A:** `"Parent"`

**Static** methods are **not polymorphic**. The method called depends on the **reference type**, not the object type.

---

**Q39: Does this code compile?**
```java
Object obj = "Hello";
String s = obj;
```

**A:** No, **compilation error**: "incompatible types: Object cannot be converted to String".

Even if the object is really a String, the **reference type** is `Object`. A **cast** is needed for downcasting.

**Correct**:
```java
String s = (String)obj;  // OK
```

---

**Q40: What is the difference?**
```java
// Version 1
Animal a = new Dog();
Dog c = (Dog)a;

// Version 2
Animal a = new Dog();
if (a instanceof Dog c) {
    // Use c
}
```

**A:**
- **Version 1**: Cast without check (risk of ClassCastException)
- **Version 2**: **Pattern matching** (Java 16+) - checks and casts in one line, safer

---

## Trick Questions

**Q41: How many abstract methods does this interface have?**
```java
@FunctionalInterface
public interface Calculator {
    int calculate(int a, int b);
    
    default void display() {
        System.out.println("Calculator");
    }
    
    static void log() {
        System.out.println("Log");
    }
}
```

**A:** **Only 1** abstract method: `calculate()`.

**default** and **static** methods do **not** count for `@FunctionalInterface` (which requires exactly 1 abstract method).

---

**Q42: Does this code compile?**
```java
@FunctionalInterface
public interface Invalid {
    void method1();
    void method2();
}
```

**A:** No, **compilation error**: "Invalid is not a functional interface".

A **functional interface** must have **exactly 1** abstract method.

---

**Q43: What does this expression return?**
```java
public enum Day {
    MONDAY, TUESDAY, WEDNESDAY
}

Day[] days = Day.values();
System.out.println(days.length);
```

**A:** `3`

`values()` returns an **array** containing all enum constants.

---

**Q44: Does this code compile?**
```java
public interface A {
    default void method() {
        System.out.println("A");
    }
}

public interface B extends A {
    default void method() {
        System.out.println("B");
    }
}
```

**A:** Yes, it compiles.

An interface can **override** a default method inherited from another interface.

---

**Q45: What is the value of x after these lines?**
```java
public interface Config {
    int MAX = 100;
}

Config.MAX = 200;
```

**A:** **Compilation error**: "cannot assign a value to final variable MAX".

Interface variables are **implicitly final** (constants), they cannot be modified.

---

**Q46: Can this record be modified?**
```java
public record Point(int x, int y) { }

Point p = new Point(3, 4);
```

**A:** No, records are **immutable**.

You cannot modify `x` or `y`. To "modify", you must create a **new** Point:
```java
Point p2 = new Point(p.x() + 1, p.y());  // New point (4, 4)
```

---

**Q47: What is printed?**
```java
public class Outer {
    private int x = 10;
    
    public void method() {
        int x = 20;
        
        class Local {
            public void display() {
                System.out.println(x);
            }
        }
        
        new Local().display();
    }
}

new Outer().method();
```

**A:** `20`

The local class accesses the **local variable** `x` (20), not the instance variable.

To access the instance variable:
```java
System.out.println(Outer.this.x);  // 10
```

---

**Q48: Does this code compile?**
```java
public sealed class Animal { }

public final class Dog extends Animal { }
```

**A:** No, **compilation error**: "sealed class must have permits clause".

A sealed class must specify its subclasses with `permits`.

**Correct**:
```java
public sealed class Animal permits Dog { }
public final class Dog extends Animal { }
```

---

**Q49: How many interfaces can a class implement?**
```java
public class C implements A, B, C, D, E { }
```

**A:** **Unlimited** (as many as you want).

A class can implement **multiple interfaces** (no limit), but can only extend **one class**.

---

**Q50: What does instanceof return for null?**
```java
String s = null;
System.out.println(s instanceof String);
System.out.println(s instanceof Object);
```

**A:**
```java
false
false
```

`null` is not an instance of **any type**, so `instanceof` always returns `false` for `null`.

---

**Q51: Does this code compile?**
```java
public enum Season {
    SPRING, SUMMER, AUTUMN, WINTER
}

Season s = Season.SPRING;
switch (s) {
    case Season.SPRING:
        System.out.println("Spring");
        break;
}
```

**A:** No, **compilation error**: "an enum switch case label must be the unqualified name of an enumeration constant".

In a `switch` on an enum, use the **simple name** (without the type):

**Correct**:
```java
switch (s) {
    case SPRING:  // Not Season.SPRING
        System.out.println("Spring");
        break;
}
```

---

**Q52: What is the difference?**
```java
// Version 1
List<String> list = new ArrayList<>();

// Version 2
ArrayList<String> list = new ArrayList<>();
```

**A:**
- **Version 1**: Reference type = **interface** (`List`) - more flexible
- **Version 2**: Reference type = **concrete class** (`ArrayList`) - less flexible

**Best practice**: Use the interface as the reference type (Version 1) for polymorphism.

```java
List<String> list = new ArrayList<>();
// Later, easy to change the implementation:
list = new LinkedList<>();  // OK with Version 1
```

---

**Q53: Does this record correctly override equals()?**
```java
public record Point(int x, int y) {
    @Override
    public boolean equals(Point other) {
        return this.x == other.x && this.y == other.y;
    }
}
```

**A:** No, this is **not an override**.

The correct signature of `equals()` is `equals(Object)`, not `equals(Point)`. This is an **overload**, not an override.

The record automatically generates the correct `equals(Object)`, so this custom method just creates a useless overload.

---

**Q54: How many times is the enum constructor called?**
```java
public enum Color {
    RED, GREEN, BLUE;
    
    private Color() {
        System.out.println("Constructor");
    }
}

public static void main(String[] args) {
    Color c1 = Color.RED;
    Color c2 = Color.RED;
    Color c3 = Color.GREEN;
}
```

**A:** **3 times** (once per enum constant).

Enum constructors are called **only once** when the class is loaded (when constants are created), not on each use.

Output:
```
Constructor
Constructor
Constructor
```

---

**Q55: Is this code valid?**
```java
public interface A {
    private void helper() {
        System.out.println("Helper");
    }
}

public class B implements A {
    public void test() {
        helper();  // ?
    }
}
```

**A:** No, **compilation error**: "cannot find symbol: method helper()".

**Private methods** of an interface are **not inherited** by implementing classes. They are only for internal use (by the interface's default methods).

---
