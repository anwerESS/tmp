## [**..**](./00_index.md)

# REST API Design & Filtering

## 🏗️ REST API Design - Architecture and Best Practices

### Layered Architecture

```
Client (Browser/Mobile)
    ↓ HTTP Request
Controller Layer (@RestController)
    ↓ DTOs
Service Layer (@Service)
    ↓ Business Logic
Repository Layer (@Repository)
    ↓ SQL/JPQL
Database
```

**Separation of concerns:**
- **Controller** : Handles HTTP requests/responses, validation, DTO mapping
- **Service** : Business logic, transactions
- **Repository** : Data access, queries

### REST Resources - Naming Conventions

**Best Practices:**
```
GET    /api/users              → List of users
GET    /api/users/{id}         → A specific user
POST   /api/users              → Create a user
PUT    /api/users/{id}         → Full update
PATCH  /api/users/{id}         → Partial update
DELETE /api/users/{id}         → Delete

GET    /api/users/{id}/orders  → Orders of a user (nested resources)
```

**Rules:**
- Use **nouns** (not verbs): `/users` not `/getUsers`
- **Plural** for collections: `/users` not `/user`
- **Lowercase** with hyphens: `/user-profiles` not `/UserProfiles`
- **Hierarchy** for relations: `/users/{userId}/orders/{orderId}`

### DTOs (Data Transfer Objects) - Entity/API Separation

**Why use DTOs?**

```java
// ❌ Bad practice: Expose the entity directly
@RestController
public class UserController {
    
    @GetMapping("/users/{id}")
    public User getUser(@PathVariable Long id) {
        return userService.findById(id);  // Exposes EVERYTHING (password, relations, etc.)
    }
}

// ✅ Good practice: Use DTOs
@RestController
public class UserController {
    
    @GetMapping("/users/{id}")
    public ResponseEntity<UserDTO> getUser(@PathVariable Long id) {
        User user = userService.findById(id);
        UserDTO dto = new UserDTO(user);  // Control over exposed fields
        return ResponseEntity.ok(dto);
    }
}
```

**Advantages of DTOs:**
- **Security**: Don't expose sensitive fields (password, internal IDs)
- **Decoupling**: API independent from DB structure
- **Flexibility**: Different DTOs for different endpoints
- **Performance**: Reduce transmitted data
- **Versioning**: Stable API even if the entity changes

### Full Architecture with DTOs

```java
// Entity (Domain Layer)
@Entity
@Table(name = "users")
public class User {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    private String username;
    private String email;
    private String password;  // Sensitive!
    
    @OneToMany(mappedBy = "user")
    private List<Order> orders;
    
    @CreatedDate
    private LocalDateTime createdAt;
    
    @LastModifiedDate
    private LocalDateTime updatedAt;
}

// DTOs (Presentation Layer)

// DTO for creation
public class CreateUserRequest {
    @NotBlank
    @Size(min = 3, max = 50)
    private String username;
    
    @NotBlank
    @Email
    private String email;
    
    @NotBlank
    @Size(min = 8)
    private String password;
    
    // Getters, setters
}

// DTO for response (no password)
public class UserResponse {
    private Long id;
    private String username;
    private String email;
    private LocalDateTime createdAt;
    
    public UserResponse(User user) {
        this.id = user.getId();
        this.username = user.getUsername();
        this.email = user.getEmail();
        this.createdAt = user.getCreatedAt();
        // No password!
    }
}

// DTO for list (even less info)
public class UserSummaryDTO {
    private Long id;
    private String username;
    
    public UserSummaryDTO(User user) {
        this.id = user.getId();
        this.username = user.getUsername();
    }
}

// Controller
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @Autowired
    private UserService userService;
    
    // List - summary DTO
    @GetMapping
    public ResponseEntity<List<UserSummaryDTO>> getAllUsers() {
        List<User> users = userService.findAll();
        List<UserSummaryDTO> dtos = users.stream()
            .map(UserSummaryDTO::new)
            .collect(Collectors.toList());
        return ResponseEntity.ok(dtos);
    }
    
    // Details - full DTO
    @GetMapping("/{id}")
    public ResponseEntity<UserResponse> getUser(@PathVariable Long id) {
        User user = userService.findById(id);
        return ResponseEntity.ok(new UserResponse(user));
    }
    
    // Creation - Request DTO
    @PostMapping
    public ResponseEntity<UserResponse> createUser(
            @RequestBody @Valid CreateUserRequest request) {
        User user = userService.create(request);
        return ResponseEntity
            .status(HttpStatus.CREATED)
            .body(new UserResponse(user));
    }
}
```

### Mapper - Entity ↔ DTO Conversion

**Option 1: Constructor in the DTO**
```java
public class UserResponse {
    private Long id;
    private String username;
    
    public UserResponse(User user) {
        this.id = user.getId();
        this.username = user.getUsername();
    }
}
```

**Option 2: Dedicated Mapper class**
```java
@Component
public class UserMapper {
    
    public UserResponse toResponse(User user) {
        UserResponse dto = new UserResponse();
        dto.setId(user.getId());
        dto.setUsername(user.getUsername());
        dto.setEmail(user.getEmail());
        return dto;
    }
    
    public User toEntity(CreateUserRequest request) {
        User user = new User();
        user.setUsername(request.getUsername());
        user.setEmail(request.getEmail());
        user.setPassword(passwordEncoder.encode(request.getPassword()));
        return user;
    }
}
```

**Option 3: MapStruct (automatic library)**
```java
@Mapper(componentModel = "spring")
public interface UserMapper {
    
    UserResponse toResponse(User user);
    
    @Mapping(target = "id", ignore = true)
    @Mapping(target = "password", source = "password", qualifiedByName = "encodePassword")
    User toEntity(CreateUserRequest request);
    
    List<UserResponse> toResponseList(List<User> users);
}
```

## 🔍 Filtering - Controlling Returned Fields

### The Filtering Problem

```java
// Without filtering
@Entity
public class User {
    private Long id;
    private String username;
    private String password;  // ❌ Should NEVER be exposed!
    private String ssn;       // ❌ Sensitive data
}

// JSON returned (DANGER!)
{
  "id": 1,
  "username": "john",
  "password": "hashed123",  // 😱 Exposed!
  "ssn": "123-45-6789"      // 😱 Sensitive data!
}
```

**Solution: Filtering to control which fields are serialized to JSON**

### Static Filtering - Same Filtering Everywhere

**Static Filtering**: The same filtering is applied to all endpoints using the bean.

#### With @JsonIgnore

```java
@Entity
public class User {
    private Long id;
    private String username;
    
    @JsonIgnore  // Never included in the JSON response
    private String password;
    
    @JsonIgnore
    private String ssn;
    
    private String email;
}

// JSON returned (on all endpoints)
{
  "id": 1,
  "username": "john",
  "email": "john@example.com"
  // password and ssn ignored
}
```

#### With @JsonIgnoreProperties (Class Level)

```java
@Entity
@JsonIgnoreProperties({"password", "ssn", "internalId"})
public class User {
    private Long id;
    private String username;
    private String password;     // Ignored
    private String ssn;          // Ignored
    private String internalId;   // Ignored
    private String email;
}

// JSON returned
{
  "id": 1,
  "username": "john",
  "email": "john@example.com"
}
```

#### Other Static Filtering Annotations

```java
@Entity
public class User {
    
    // Include only on serialization (GET), ignore on deserialization (POST)
    @JsonProperty(access = JsonProperty.Access.READ_ONLY)
    private Long id;
    
    // Ignore on serialization (GET), include on deserialization (POST)
    @JsonProperty(access = JsonProperty.Access.WRITE_ONLY)
    private String password;
    
    // Rename the field in JSON
    @JsonProperty("user_name")
    private String username;
    
    // Custom date format
    @JsonFormat(pattern = "yyyy-MM-dd HH:mm:ss")
    private LocalDateTime createdAt;
}

// POST JSON
{
  "user_name": "john",
  "password": "secret123"
}

// GET JSON
{
  "id": 1,
  "user_name": "john",
  "createdAt": "2025-01-15 10:30:00"
  // No password
}
```

**Advantages of Static Filtering:**
- ✅ Simple and declarative
- ✅ Consistent across all endpoints
- ✅ Performant (no runtime processing)

**Disadvantages:**
- ❌ Not flexible (same filtering everywhere)
- ❌ If you need different fields per endpoint → problem

### Dynamic Filtering - Per Endpoint Filtering

**Dynamic Filtering**: Customize the fields returned for each specific endpoint.

#### Configuration with @JsonFilter

```java
// 1. Annotate the entity with @JsonFilter
@Entity
@JsonFilter("UserFilter")  // Filter name
public class User {
    private Long id;
    private String username;
    private String email;
    private String password;
    private String phoneNumber;
    private String address;
}
```

#### Implementation in the Controller

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @Autowired
    private UserService userService;
    
    // Endpoint 1: Only id and username
    @GetMapping("/summary")
    public ResponseEntity<MappingJacksonValue> getUsersSummary() {
        List<User> users = userService.findAll();
        
        // Create the dynamic filter
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .filterOutAllExcept("id", "username");
        
        FilterProvider filters = new SimpleFilterProvider()
            .addFilter("UserFilter", filter);
        
        // Apply the filter
        MappingJacksonValue mapping = new MappingJacksonValue(users);
        mapping.setFilters(filters);
        
        return ResponseEntity.ok(mapping);
    }
    
    // Endpoint 2: Full details EXCEPT password
    @GetMapping("/{id}")
    public ResponseEntity<MappingJacksonValue> getUser(@PathVariable Long id) {
        User user = userService.findById(id);
        
        // Different filter: everything EXCEPT password
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .serializeAllExcept("password");
        
        FilterProvider filters = new SimpleFilterProvider()
            .addFilter("UserFilter", filter);
        
        MappingJacksonValue mapping = new MappingJacksonValue(user);
        mapping.setFilters(filters);
        
        return ResponseEntity.ok(mapping);
    }
    
    // Endpoint 3: Only certain fields
    @GetMapping("/contacts")
    public ResponseEntity<MappingJacksonValue> getUserContacts() {
        List<User> users = userService.findAll();
        
        // Only contact info
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .filterOutAllExcept("username", "email", "phoneNumber");
        
        FilterProvider filters = new SimpleFilterProvider()
            .addFilter("UserFilter", filter);
        
        MappingJacksonValue mapping = new MappingJacksonValue(users);
        mapping.setFilters(filters);
        
        return ResponseEntity.ok(mapping);
    }
}
```

#### JSON Results per Endpoint

```javascript
// GET /api/users/summary
[
  {
    "id": 1,
    "username": "john"
  },
  {
    "id": 2,
    "username": "jane"
  }
]

// GET /api/users/1
{
  "id": 1,
  "username": "john",
  "email": "john@example.com",
  "phoneNumber": "+1234567890",
  "address": "123 Main St"
  // password filtered
}

// GET /api/users/contacts
[
  {
    "username": "john",
    "email": "john@example.com",
    "phoneNumber": "+1234567890"
  }
]
```

### Helper Method to Simplify Code

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @Autowired
    private UserService userService;
    
    // Helper method
    private MappingJacksonValue applyFilter(Object data, String... fields) {
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .filterOutAllExcept(fields);
        
        FilterProvider filters = new SimpleFilterProvider()
            .addFilter("UserFilter", filter);
        
        MappingJacksonValue mapping = new MappingJacksonValue(data);
        mapping.setFilters(filters);
        
        return mapping;
    }
    
    // Simplified usage
    @GetMapping("/summary")
    public ResponseEntity<MappingJacksonValue> getUsersSummary() {
        List<User> users = userService.findAll();
        return ResponseEntity.ok(applyFilter(users, "id", "username"));
    }
    
    @GetMapping("/{id}")
    public ResponseEntity<MappingJacksonValue> getUser(@PathVariable Long id) {
        User user = userService.findById(id);
        return ResponseEntity.ok(applyFilter(user, 
            "id", "username", "email", "phoneNumber", "address"));
    }
}
```

### Dynamic Filtering with @RequestParam (Client-Chosen Filters)

```java
@GetMapping
public ResponseEntity<MappingJacksonValue> getUsers(
        @RequestParam(required = false) String fields) {
    
    List<User> users = userService.findAll();
    
    if (fields != null && !fields.isEmpty()) {
        String[] fieldArray = fields.split(",");
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .filterOutAllExcept(fieldArray);
        
        FilterProvider filters = new SimpleFilterProvider()
            .addFilter("UserFilter", filter);
        
        MappingJacksonValue mapping = new MappingJacksonValue(users);
        mapping.setFilters(filters);
        
        return ResponseEntity.ok(mapping);
    }
    
    // No filtering if fields not specified
    return ResponseEntity.ok(new MappingJacksonValue(users));
}

// Usage:
// GET /api/users?fields=id,username
// GET /api/users?fields=username,email,phoneNumber
```

## 📊 Comparison: Static vs Dynamic Filtering

| Aspect | Static Filtering | Dynamic Filtering |
|--------|------------------|-------------------|
| **Configuration** | Annotations on the entity | Code in the Controller |
| **Flexibility** | ❌ Same everywhere | ✅ Customizable per endpoint |
| **Performance** | ✅ Better | Slightly less performant |
| **Complexity** | ✅ Simple | More complex |
| **Use Case** | Fields always hidden (password) | Different levels of detail |
| **Maintenance** | ✅ Easy | Requires more code |

## 🎯 Recommendations

**Hybrid Strategy (Best Practice):**

```java
// 1. Static Filtering for fields ALWAYS hidden
@Entity
@JsonIgnoreProperties({"password", "ssn"})
@JsonFilter("UserFilter")  // For optional dynamic filtering
public class User {
    private Long id;
    private String username;
    
    @JsonIgnore
    private String password;  // Double security
    
    private String email;
    private String phoneNumber;
    private String address;
}

// 2. DTOs for different levels of detail (preferred)
public class UserSummaryDTO {
    private Long id;
    private String username;
}

public class UserDetailDTO {
    private Long id;
    private String username;
    private String email;
    private String phoneNumber;
}

// 3. Dynamic Filtering only when really needed
@GetMapping("/{id}")
public ResponseEntity<MappingJacksonValue> getUser(
        @PathVariable Long id,
        @RequestParam(required = false) String fields) {
    
    User user = userService.findById(id);
    
    if (fields != null) {
        return ResponseEntity.ok(applyFilter(user, fields.split(",")));
    }
    
    return ResponseEntity.ok(new UserDetailDTO(user));
}
```

**Golden Rule:**
- **Static Filtering**: Security (password, sensitive data)
- **DTOs**: Different levels of detail (summary vs full)
- **Dynamic Filtering**: Special cases where the client chooses the fields

---

This summary covers REST API architecture, the use of DTOs for separation of concerns, and both filtering approaches (static and dynamic) with their respective use cases!


# Questions & Answers - REST API Design & Filtering

## 🏗️ REST API Design & Architecture

**Q1: What are the layers of a typical REST API architecture?**
**A:**
```
Client → Controller → Service → Repository → Database
```
- **Controller**: Handles HTTP requests, validation, DTO mapping
- **Service**: Business logic, transactions
- **Repository**: Data access, queries
- **Separation of concerns** for maintainability and testability

**Q2: What are the REST naming conventions for resources?**
**A:**
- Use **nouns** (not verbs): `/users` not `/getUsers`
- **Plural** for collections: `/users` not `/user`
- **Lowercase** with hyphens: `/user-profiles` not `/UserProfiles`
- **Hierarchy** for relations: `/users/{userId}/orders/{orderId}`
- **No trailing slash**: `/users` not `/users/`

**Q3: Which HTTP methods to use for which CRUD operations?**
**A:**
```
GET    /api/users          → Read (list)
GET    /api/users/{id}     → Read (single item)
POST   /api/users          → Create
PUT    /api/users/{id}     → Full replace
PATCH  /api/users/{id}     → Partial update
DELETE /api/users/{id}     → Delete
```

**Q4: How to structure URLs for nested resources?**
**A:**
```java
// Nested resources
GET    /users/{userId}/orders           // Orders of a user
GET    /users/{userId}/orders/{orderId} // A specific order
POST   /users/{userId}/orders           // Create an order
DELETE /users/{userId}/orders/{orderId} // Delete

// Alternative with filtering
GET /orders?userId=5
```
Prefer nested resources for strong relations, filtering for weak relations.

**Q5: What is idempotence and which HTTP methods are idempotent?**
**A:** An operation is idempotent if executing it multiple times produces the same result as executing it once.
- **Idempotent**: GET, PUT, DELETE (retry safe)
- **Non-idempotent**: POST (creating twice = 2 resources)

Important for API reliability (retry after timeout).

## 📦 DTOs (Data Transfer Objects)

**Q6: What is a DTO and why use it?**
**A:** A DTO (Data Transfer Object) is an object used to transfer data between layers, specifically between the Controller and the Client. It separates the API structure from the internal structure (Entity).

**Q7: Why not expose Entities directly in the API?**
**A:**
- ❌ **Security**: Exposes sensitive fields (password, internal IDs)
- ❌ **Coupling**: API depends on the DB structure
- ❌ **Performance**: Can expose entire relations (lazy loading issues)
- ❌ **Flexibility**: Impossible to have different representations

**Q8: What are the advantages of DTOs?**
**A:**
- ✅ **Security**: Precise control over exposed fields
- ✅ **Decoupling**: API independent from DB
- ✅ **Flexibility**: Different DTOs for different endpoints
- ✅ **Performance**: Reduce transmitted data
- ✅ **Versioning**: Stable API even if Entity changes
- ✅ **Validation**: Different rules for input/output

**Q9: Example of a problem without DTOs**
**A:**
```java
// ❌ Without DTO - Exposes everything
@RestController
public class UserController {
    @GetMapping("/users/{id}")
    public User getUser(@PathVariable Long id) {
        return userService.findById(id);
    }
}

// JSON returned (DANGER!)
{
  "id": 1,
  "username": "john",
  "password": "hashed123",  // 😱 Exposed!
  "ssn": "123-45-6789",     // 😱 Sensitive data!
  "orders": [...]           // 😱 Relations loaded
}
```

**Q10: Create a complete architecture with DTOs (Request/Response)**
**A:**
```java
// Entity (never expose)
@Entity
public class User {
    @Id
    private Long id;
    private String username;
    private String email;
    private String password;  // Sensitive
    private LocalDateTime createdAt;
}

// Request DTO (creation)
public class CreateUserRequest {
    @NotBlank
    private String username;
    
    @Email
    private String email;
    
    @Size(min = 8)
    private String password;
}

// Response DTO (no password)
public class UserResponse {
    private Long id;
    private String username;
    private String email;
    private LocalDateTime createdAt;
    
    public UserResponse(User user) {
        this.id = user.getId();
        this.username = user.getUsername();
        this.email = user.getEmail();
        this.createdAt = user.getCreatedAt();
    }
}

// Controller
@PostMapping("/users")
public ResponseEntity<UserResponse> createUser(
        @RequestBody @Valid CreateUserRequest request) {
    User user = userService.create(request);
    return ResponseEntity
        .status(HttpStatus.CREATED)
        .body(new UserResponse(user));
}
```

**Q11: What is the difference between a Request DTO and a Response DTO?**
**A:**
- **Request DTO**: Incoming data (POST/PUT), contains validations, can include password
- **Response DTO**: Outgoing data (GET), excludes sensitive fields, can include computed fields

**Q12: Where to place the Entity ↔ DTO conversion logic?**
**A:** **3 approaches:**

**1. Constructor in the DTO**
```java
public class UserResponse {
    public UserResponse(User user) {
        this.id = user.getId();
        this.username = user.getUsername();
    }
}
```

**2. Dedicated Mapper class**
```java
@Component
public class UserMapper {
    public UserResponse toResponse(User user) {
        // Mapping logic
    }
    
    public User toEntity(CreateUserRequest request) {
        // Mapping logic
    }
}
```

**3. MapStruct (automatic generation)**
```java
@Mapper(componentModel = "spring")
public interface UserMapper {
    UserResponse toResponse(User user);
    List<UserResponse> toResponseList(List<User> users);
}
```

**Q13: When to use different DTOs for the same endpoint?**
**A:**
```java
// Summary DTO for list (less details)
public class UserSummaryDTO {
    private Long id;
    private String username;
}

// Detail DTO for a single item (more details)
public class UserDetailDTO {
    private Long id;
    private String username;
    private String email;
    private String phoneNumber;
    private List<OrderSummaryDTO> recentOrders;
}

// Controller
@GetMapping          // List
public List<UserSummaryDTO> getAll() { ... }

@GetMapping("/{id}") // Details
public UserDetailDTO getOne(@PathVariable Long id) { ... }
```

**Q14: How to handle relations in DTOs (avoid circular references)?**
**A:**
```java
// Entity with bidirectional relation
@Entity
public class User {
    @OneToMany(mappedBy = "user")
    private List<Order> orders;
}

@Entity
public class Order {
    @ManyToOne
    private User user;
}

// ❌ Problem: infinite JSON (User → Orders → User → ...)

// ✅ Solution: DTOs without back reference
public class UserResponse {
    private Long id;
    private String username;
    private List<OrderSummaryDTO> orders;  // No User in OrderSummaryDTO
}

public class OrderSummaryDTO {
    private Long id;
    private String productName;
    // No User here!
}
```

## 🔍 Filtering - General

**Q15: What is filtering in a REST API?**
**A:** Filtering controls which fields of an object are included in the JSON response. It allows:
- Hiding sensitive data (password, SSN)
- Optimizing performance (less data transferred)
- Providing different levels of detail per endpoint

**Q16: What are the two types of filtering?**
**A:**
- **Static Filtering**: Same filtering for a bean on all endpoints
- **Dynamic Filtering**: Custom filtering per specific endpoint

**Q17: What is the difference between Static and Dynamic Filtering?**
**A:**
| Aspect | Static | Dynamic |
|--------|--------|---------|
| **Configuration** | Annotations on Entity | Code in Controller |
| **Flexibility** | Same everywhere | Customizable |
| **Performance** | Better | Slightly less |
| **Complexity** | Simple | More complex |
| **Use Case** | Fields always hidden | Different levels |

## 🔒 Static Filtering

**Q18: What is Static Filtering?**
**A:** Static Filtering applies the same filtering to all endpoints using the bean. Configured with annotations on the Entity. Once defined, the field is always included or excluded.

**Q19: How to use @JsonIgnore?**
**A:**
```java
@Entity
public class User {
    private Long id;
    private String username;
    
    @JsonIgnore  // Never serialized to JSON
    private String password;
    
    @JsonIgnore
    private String ssn;
}

// JSON returned (all endpoints)
{
  "id": 1,
  "username": "john"
  // password and ssn always ignored
}
```

**Q20: How to use @JsonIgnoreProperties at class level?**
**A:**
```java
@Entity
@JsonIgnoreProperties({"password", "ssn", "internalId"})
public class User {
    private Long id;
    private String username;
    private String password;     // Ignored
    private String ssn;          // Ignored
    private String internalId;   // Ignored
}

// Equivalent to putting @JsonIgnore on each field
```

**Q21: What is the difference between @JsonIgnore and @JsonIgnoreProperties?**
**A:**
- **@JsonIgnore**: On the field, ignores that specific field
- **@JsonIgnoreProperties**: On the class, lists multiple fields to ignore

Both have the same effect. @JsonIgnoreProperties is more readable when there are several fields.

**Q22: How to use @JsonProperty with access control?**
**A:**
```java
@Entity
public class User {
    // READ_ONLY: Included on GET, ignored on POST/PUT
    @JsonProperty(access = JsonProperty.Access.READ_ONLY)
    private Long id;
    
    @JsonProperty(access = JsonProperty.Access.READ_ONLY)
    private LocalDateTime createdAt;
    
    // WRITE_ONLY: Accepted on POST/PUT, never returned on GET
    @JsonProperty(access = JsonProperty.Access.WRITE_ONLY)
    private String password;
}

// POST Request (accepted)
{
  "username": "john",
  "password": "secret"
}

// GET Response (no password)
{
  "id": 1,
  "username": "john",
  "createdAt": "2025-01-15T10:30:00"
}
```

**Q23: Other useful Jackson annotations for Static Filtering?**
**A:**
```java
@Entity
public class User {
    // Rename the field in JSON
    @JsonProperty("user_name")
    private String username;
    
    // Custom date format
    @JsonFormat(pattern = "yyyy-MM-dd HH:mm:ss")
    private LocalDateTime createdAt;
    
    // Include only if non-null
    @JsonInclude(JsonInclude.Include.NON_NULL)
    private String middleName;
    
    // Include only if non-empty
    @JsonInclude(JsonInclude.Include.NON_EMPTY)
    private List<String> tags;
}
```

**Q24: When to use Static Filtering?**
**A:** Use Static Filtering for:
- ✅ Fields that are **always** sensitive (password, SSN, internal IDs)
- ✅ Guaranteed consistency across the entire API
- ✅ Security by default
- ✅ Simple cases where the same filtering applies everywhere

**Q25: What are the disadvantages of Static Filtering?**
**A:**
- ❌ Not flexible: impossible to have different levels of detail
- ❌ If one endpoint needs the field → problem
- ❌ Requires DTOs for flexibility

## 🎯 Dynamic Filtering

**Q26: What is Dynamic Filtering?**
**A:** Dynamic Filtering allows customizing the fields returned for each specific endpoint. Configured in the Controller with `@JsonFilter` and `FilterProvider`. Full flexibility per endpoint.

**Q27: How to configure Dynamic Filtering with @JsonFilter?**
**A:**
```java
// 1. Annotate the Entity
@Entity
@JsonFilter("UserFilter")  // Filter name
public class User {
    private Long id;
    private String username;
    private String email;
    private String password;
    private String phoneNumber;
}

// 2. In the Controller, apply the filter
@GetMapping("/summary")
public ResponseEntity<MappingJacksonValue> getUsersSummary() {
    List<User> users = userService.findAll();
    
    // Create the filter
    SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
        .filterOutAllExcept("id", "username");
    
    FilterProvider filters = new SimpleFilterProvider()
        .addFilter("UserFilter", filter);
    
    // Apply
    MappingJacksonValue mapping = new MappingJacksonValue(users);
    mapping.setFilters(filters);
    
    return ResponseEntity.ok(mapping);
}
```

**Q28: What is SimpleBeanPropertyFilter?**
**A:** `SimpleBeanPropertyFilter` is a Jackson class that defines which fields to include or exclude:
```java
// Include only these fields
SimpleBeanPropertyFilter.filterOutAllExcept("id", "username", "email");

// Exclude these fields (include the rest)
SimpleBeanPropertyFilter.serializeAllExcept("password", "ssn");
```

**Q29: What is MappingJacksonValue?**
**A:** `MappingJacksonValue` is a wrapper that allows applying Jackson filters dynamically before serialization:
```java
MappingJacksonValue mapping = new MappingJacksonValue(data);
mapping.setFilters(filters);
return ResponseEntity.ok(mapping);
```
Required for Dynamic Filtering because the return type must be `MappingJacksonValue`.

**Q30: How to implement different filters for different endpoints?**
**A:**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    // Endpoint 1: Summary (id, username only)
    @GetMapping("/summary")
    public ResponseEntity<MappingJacksonValue> getSummary() {
        List<User> users = userService.findAll();
        
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .filterOutAllExcept("id", "username");
        
        return applyFilter(users, filter);
    }
    
    // Endpoint 2: Details (everything except password)
    @GetMapping("/{id}")
    public ResponseEntity<MappingJacksonValue> getDetails(@PathVariable Long id) {
        User user = userService.findById(id);
        
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .serializeAllExcept("password");
        
        return applyFilter(user, filter);
    }
    
    // Endpoint 3: Contacts (username, email, phone only)
    @GetMapping("/contacts")
    public ResponseEntity<MappingJacksonValue> getContacts() {
        List<User> users = userService.findAll();
        
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .filterOutAllExcept("username", "email", "phoneNumber");
        
        return applyFilter(users, filter);
    }
    
    // Helper method
    private ResponseEntity<MappingJacksonValue> applyFilter(
            Object data, SimpleBeanPropertyFilter filter) {
        
        FilterProvider filters = new SimpleFilterProvider()
            .addFilter("UserFilter", filter);
        
        MappingJacksonValue mapping = new MappingJacksonValue(data);
        mapping.setFilters(filters);
        
        return ResponseEntity.ok(mapping);
    }
}
```

**Q31: How to simplify Dynamic Filtering code with a helper method?**
**A:**
```java
@RestController
public class UserController {
    
    // Helper to simplify
    private MappingJacksonValue filterFields(Object data, String... fields) {
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .filterOutAllExcept(fields);
        
        FilterProvider filters = new SimpleFilterProvider()
            .addFilter("UserFilter", filter);
        
        MappingJacksonValue mapping = new MappingJacksonValue(data);
        mapping.setFilters(filters);
        
        return mapping;
    }
    
    // Simplified usage
    @GetMapping("/summary")
    public ResponseEntity<MappingJacksonValue> getSummary() {
        List<User> users = userService.findAll();
        return ResponseEntity.ok(filterFields(users, "id", "username"));
    }
    
    @GetMapping("/{id}")
    public ResponseEntity<MappingJacksonValue> getDetails(@PathVariable Long id) {
        User user = userService.findById(id);
        return ResponseEntity.ok(filterFields(user, 
            "id", "username", "email", "phoneNumber"));
    }
}
```

**Q32: How to let the client choose fields with @RequestParam?**
**A:**
```java
@GetMapping
public ResponseEntity<MappingJacksonValue> getUsers(
        @RequestParam(required = false) String fields) {
    
    List<User> users = userService.findAll();
    
    if (fields != null && !fields.isEmpty()) {
        String[] fieldArray = fields.split(",");
        
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .filterOutAllExcept(fieldArray);
        
        FilterProvider filters = new SimpleFilterProvider()
            .addFilter("UserFilter", filter);
        
        MappingJacksonValue mapping = new MappingJacksonValue(users);
        mapping.setFilters(filters);
        
        return ResponseEntity.ok(mapping);
    }
    
    // If no fields, return everything
    return ResponseEntity.ok(new MappingJacksonValue(users));
}

// Usage:
// GET /api/users?fields=id,username
// GET /api/users?fields=username,email,phoneNumber
```

**Q33: What is the difference between filterOutAllExcept and serializeAllExcept?**
**A:**
```java
// filterOutAllExcept: Include ONLY these fields
SimpleBeanPropertyFilter.filterOutAllExcept("id", "username");
// JSON: {"id": 1, "username": "john"}

// serializeAllExcept: Exclude these fields (include the rest)
SimpleBeanPropertyFilter.serializeAllExcept("password", "ssn");
// JSON: {"id": 1, "username": "john", "email": "...", ...}
//       (everything except password and ssn)
```

**Q34: Can you have multiple @JsonFilter on the same entity?**
**A:** No, an entity can only have one `@JsonFilter`. If you need different filters, use:
1. Different DTOs (recommended)
2. The same filter name with different configurations per endpoint

**Q35: How to handle relations with Dynamic Filtering?**
**A:**
```java
@Entity
@JsonFilter("UserFilter")
public class User {
    private Long id;
    private String username;
    
    @OneToMany(mappedBy = "user")
    private List<Order> orders;
}

@Entity
@JsonFilter("OrderFilter")
public class Order {
    private Long id;
    private String productName;
}

// Controller - filter User AND Orders
@GetMapping("/{id}")
public ResponseEntity<MappingJacksonValue> getUser(@PathVariable Long id) {
    User user = userService.findById(id);
    
    // Filter for User
    SimpleBeanPropertyFilter userFilter = SimpleBeanPropertyFilter
        .filterOutAllExcept("id", "username", "orders");
    
    // Filter for Order
    SimpleBeanPropertyFilter orderFilter = SimpleBeanPropertyFilter
        .filterOutAllExcept("id", "productName");
    
    FilterProvider filters = new SimpleFilterProvider()
        .addFilter("UserFilter", userFilter)
        .addFilter("OrderFilter", orderFilter);
    
    MappingJacksonValue mapping = new MappingJacksonValue(user);
    mapping.setFilters(filters);
    
    return ResponseEntity.ok(mapping);
}
```

**Q36: When to use Dynamic Filtering?**
**A:** Use Dynamic Filtering for:
- ✅ Different levels of detail per endpoint
- ✅ Flexible API where the client can choose fields
- ✅ Admin vs user endpoints (different fields)
- ✅ Performance optimization (load only what's needed)

**Q37: What are the disadvantages of Dynamic Filtering?**
**A:**
- ❌ More code in the Controllers
- ❌ Less performant than Static (runtime processing)
- ❌ Harder to maintain
- ❌ Return type `MappingJacksonValue` (less intuitive)
- ❌ Complexity with relations

## 🔄 Static vs Dynamic - Comparison

**Q38: Full comparison table Static vs Dynamic Filtering**
**A:**
| Criteria | Static Filtering | Dynamic Filtering |
|---------|------------------|-------------------|
| **Configuration** | Annotations (@JsonIgnore) | Code (FilterProvider) |
| **Location** | On the Entity | In the Controller |
| **Flexibility** | ❌ Fixed | ✅ Customizable |
| **Performance** | ✅ Better | Slightly less |
| **Complexity** | ✅ Simple | More complex |
| **Return type** | Entity/DTO | MappingJacksonValue |
| **Maintenance** | ✅ Easy | More code |
| **Use Case** | Security (password) | Levels of detail |

**Q39: Which approach to choose: Static, Dynamic, or DTOs?**
**A:**
**Hierarchical recommendation:**
1. **DTOs** (preferred): Best practice, clear separation
2. **Static Filtering**: For fields always hidden (security)
3. **Dynamic Filtering**: Only when truly necessary

**Q40: Can you combine Static and Dynamic Filtering?**
**A:** Yes, hybrid strategy (best practice):
```java
@Entity
@JsonIgnoreProperties({"password", "ssn"})  // Static: always hidden
@JsonFilter("UserFilter")                   // Dynamic: optional
public class User {
    private Long id;
    private String username;
    
    @JsonIgnore
    private String password;  // Double security
    
    private String email;
    private String phoneNumber;
}

// Controller
@GetMapping("/summary")
public ResponseEntity<MappingJacksonValue> getSummary() {
    // Dynamic filtering for username only
    // password already filtered by Static
    return filterFields(users, "id", "username");
}

@GetMapping("/{id}")
public ResponseEntity<MappingJacksonValue> getDetails(@PathVariable Long id) {
    // More details, but password always hidden
    return filterFields(user, "id", "username", "email", "phoneNumber");
}
```

## 🎯 Best Practices

**Q41: What is the best overall strategy for managing exposed data?**
**A:**
```java
// 1. Static Filtering for security
@Entity
@JsonIgnoreProperties({"password", "ssn"})
public class User {
    @JsonIgnore
    private String password;  // Always hidden
}

// 2. DTOs for different levels (preferred)
public class UserSummaryDTO {
    private Long id;
    private String username;
}

public class UserDetailDTO {
    private Long id;
    private String username;
    private String email;
    private String phoneNumber;
}

// 3. Dynamic Filtering only when needed
@GetMapping("/{id}")
public ResponseEntity<?> getUser(
        @PathVariable Long id,
        @RequestParam(required = false) String fields) {
    
    User user = userService.findById(id);
    
    // If client specifies fields → Dynamic
    if (fields != null) {
        return ResponseEntity.ok(filterFields(user, fields.split(",")));
    }
    
    // Otherwise → DTO
    return ResponseEntity.ok(new UserDetailDTO(user));
}
```

**Q42: Security rules with filtering**
**A:**
- ✅ **Always** Static Filtering for sensitive fields (password, SSN)
- ✅ **Double security**: DTOs + Static Filtering
- ✅ **Whitelist** (include explicitly) rather than blacklist
- ❌ **Never** expose Entities directly
- ✅ **Test** all JSON responses for data leaks

**Q43: How to test filtering?**
**A:**
```java
@SpringBootTest
@AutoConfigureMockMvc
public class UserControllerTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    @Test
    public void getUserSummary_shouldNotExposePassword() throws Exception {
        mockMvc.perform(get("/api/users/summary"))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$[0].id").exists())
            .andExpect(jsonPath("$[0].username").exists())
            .andExpect(jsonPath("$[0].password").doesNotExist())  // Critical
            .andExpect(jsonPath("$[0].ssn").doesNotExist());
    }
    
    @Test
    public void getUserDetails_shouldExposeEmailButNotPassword() throws Exception {
        mockMvc.perform(get("/api/users/1"))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.email").exists())
            .andExpect(jsonPath("$.password").doesNotExist());  // Always check
    }
}
```

**Q44: Performance: what is the impact of filtering?**
**A:**
- **Static Filtering**: No impact (compile-time)
- **Dynamic Filtering**: Minimal impact (a few ms per request)
- **DTOs**: Light impact (Entity → DTO mapping)
- **Big impact**: Loading unnecessary data (N+1, eager loading)

**Optimization:** Use JPA projections to load only the necessary fields.

**Q45: How to document an API with filtering in Swagger/OpenAPI?**
**A:**
```java
@RestController
@RequestMapping("/api/users")
@Tag(name = "Users", description = "User management API")
public class UserController {
    
    @Operation(
        summary = "Get user summary",
        description = "Returns minimal user info (id, username only)"
    )
    @ApiResponse(
        responseCode = "200",
        description = "Successful operation",
        content = @Content(
            schema = @Schema(implementation = UserSummaryDTO.class)
        )
    )
    @GetMapping("/summary")
    public ResponseEntity<MappingJacksonValue> getSummary() {
        // Dynamic filtering
    }
    
    @Operation(summary = "Get user with custom fields")
    @Parameter(
        name = "fields",
        description = "Comma-separated list of fields to include",
        example = "id,username,email"
    )
    @GetMapping
    public ResponseEntity<MappingJacksonValue> getUsers(
            @RequestParam(required = false) String fields) {
        // ...
    }
}
```

---

Here are 45 complete questions and answers covering REST API Design, DTOs, and Filtering (Static & Dynamic)! These questions range from basic concepts to advanced implementations and production best practices.
