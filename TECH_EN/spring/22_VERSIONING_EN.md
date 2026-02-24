## [**..**](./00_index.md)

# API Versioning with Spring Boot


---

### **Why Versioning?**

Versioning is essential to:
- **Avoid breaking changes**: allow existing clients to keep working
- **Facilitate evolution**: introduce new features without impacting existing versions
- **Maintain multiple versions** simultaneously during the transition period
- **Improve maintainability**: isolate legacy code from new implementations

---

### **The 4 Versioning Strategies**

I have worked with four main approaches, each with its own advantages and disadvantages:

---

#### **1. Versioning through URI Path**

**Principle:** Include the version directly in the URL

**Implementation:**
```java
@RestController
@RequestMapping("/api/v1/users")
public class UserControllerV1 {
    @GetMapping("/{id}")
    public UserV1 getUser(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
}

@RestController
@RequestMapping("/api/v2/users")
public class UserControllerV2 {
    @GetMapping("/{id}")
    public UserV2 getUser(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Example requests:**
```
GET /api/v1/users/1
GET /api/v2/users/1
```

**Advantages:**
- ✅ Very simple and explicit
- ✅ Easy to test and debug
- ✅ Compatible with caches and proxies
- ✅ Visible in logs

**Disadvantages:**
- ❌ Different URLs for the same resource
- ❌ Code duplication between controllers

**Usage:** This is the most popular approach (used by Twitter, Stripe, GitHub).

---

#### **2. Versioning through Query Parameters**

**Principle:** Pass the version as a query parameter

**Implementation:**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping(value = "/{id}", params = "version=1")
    public UserV1 getUserV1(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
    
    @GetMapping(value = "/{id}", params = "version=2")
    public UserV2 getUserV2(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Example requests:**
```
GET /api/users/1?version=1
GET /api/users/1?version=2
```

**Advantages:**
- ✅ Identical base URL
- ✅ Easy to implement
- ✅ Optional version (default possible)

**Disadvantages:**
- ❌ Less visible and intuitive
- ❌ Can cause issues with some caches
- ❌ Non-standard REST

**Usage:** Useful for minor variations or feature flags.

---

#### **3. Versioning through Custom Headers**

**Principle:** Use a custom HTTP header to specify the version

**Implementation:**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping(value = "/{id}", headers = "X-API-VERSION=1")
    public UserV1 getUserV1(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
    
    @GetMapping(value = "/{id}", headers = "X-API-VERSION=2")
    public UserV2 getUserV2(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Example requests:**
```
GET /api/users/1
Headers: X-API-VERSION: 1

GET /api/users/1
Headers: X-API-VERSION: 2
```

**Advantages:**
- ✅ Clean and stable URL
- ✅ Separation of concerns (version in metadata)
- ✅ Flexible for additional metadata

**Disadvantages:**
- ❌ Less visible (hidden headers)
- ❌ Harder to test manually
- ❌ Can be ignored by some proxies/caches
- ❌ Non-standard

**Usage:** For internal APIs or when you want to keep URLs clean.

---

#### **4. Versioning through Content Negotiation (Accept Header)**

**Principle:** Use the standard HTTP `Accept` header to negotiate the version via media type

**Implementation:**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping(value = "/{id}", produces = "application/vnd.company.app-v1+json")
    public UserV1 getUserV1(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
    
    @GetMapping(value = "/{id}", produces = "application/vnd.company.app-v2+json")
    public UserV2 getUserV2(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Example requests:**
```
GET /api/users/1
Accept: application/vnd.company.app-v1+json

GET /api/users/1
Accept: application/vnd.company.app-v2+json
```

**Advantages:**
- ✅ Respects REST and HTTP standards
- ✅ Stable and clean URL
- ✅ Allows negotiating both format AND version
- ✅ The most "RESTful" approach

**Disadvantages:**
- ❌ More complex to implement and understand
- ❌ Hard to test with a simple browser
- ❌ Less intuitive for developers

**Usage:** For public APIs strictly following REST (used by the GitHub API).

---

### **Versioning Cleaning & Best Practices**

**Cleanup strategy:**

1. **Deprecation warnings**: Warn clients before removing a version
```java
@GetMapping("/api/v1/users/{id}")
@Deprecated
public ResponseEntity<UserV1> getUserV1(@PathVariable Long id) {
    return ResponseEntity.ok()
        .header("Warning", "299 - \"API v1 is deprecated. Please migrate to v2\"")
        .body(userService.getUserV1(id));
}
```

2. **Sunset Header**: Indicate the end-of-life date
```java
.header("Sunset", "Sat, 31 Dec 2025 23:59:59 GMT")
```

3. **Progressive migration**:
   - Maintain v1 and v2 for 6-12 months
   - Monitor v1 usage via logs/metrics
   - Communicate with clients
   - Remove v1 after complete migration

4. **Documentation**: Document all available versions in Swagger
```java
@Operation(summary = "Get user (v1 - DEPRECATED)", 
           deprecated = true)
```

---

### **Comparison and Recommendations**

| Strategy | Simplicity | RESTful | Visibility | Cache-friendly | Recommendation |
|-----------|------------|---------|------------|----------------|----------------|
| **URI Path** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | **Best general choice** |
| **Query Param** | ⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | For minor variations |
| **Custom Header** | ⭐⭐⭐ | ⭐⭐ | ⭐⭐ | ⭐⭐⭐ | Internal APIs |
| **Content Negotiation** | ⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ | Strictly RESTful public APIs |

**My recommendation:**
- **URI Path** for the majority of projects (simplicity + clarity)
- **Content Negotiation** for public APIs requiring strict REST compliance
- Avoid mixing multiple strategies in the same project

---

### **Complete Example: Migration v1 → v2**

**Scenario:** Adding a `phoneNumber` field in UserV2

**Version 1:**
```java
@Data
public class UserV1 {
    private Long id;
    private String name;
    private String email;
}
```

**Version 2:**
```java
@Data
public class UserV2 {
    private Long id;
    private String name;
    private String email;
    private String phoneNumber; // New field
}
```

**Controllers:**
```java
@RestController
public class UserController {
    
    @GetMapping("/api/v1/users/{id}")
    @Deprecated
    public ResponseEntity<UserV1> getUserV1(@PathVariable Long id) {
        User user = userService.findById(id);
        UserV1 userV1 = mapper.toV1(user);
        return ResponseEntity.ok()
            .header("Warning", "299 - \"Please migrate to v2\"")
            .body(userV1);
    }
    
    @GetMapping("/api/v2/users/{id}")
    public UserV2 getUserV2(@PathVariable Long id) {
        User user = userService.findById(id);
        return mapper.toV2(user);
    }
}
```

---

**Summary:** Versioning is crucial for maintaining evolvable APIs without breaking changes. The **URI Path** strategy is generally the most pragmatic, but the choice depends on context (public vs internal API, REST constraints, acceptable complexity).

Here are complete Q&A pairs to prepare for your interview on versioning:

---

## **Q&A: API Versioning**

### **General Questions**

**Q1: What is API Versioning and why is it important?**

**A:** API Versioning is about managing different versions of an API simultaneously. It is crucial for:
- **Avoiding breaking changes**: existing clients keep working while new features are introduced
- **Enabling progressive evolution**: the API can be improved without forcing all clients to migrate immediately
- **Maintaining backward compatibility**: respecting existing contracts with consumers
- **Facilitating maintenance**: separating legacy code from new implementations

**Concrete example:** If I change the response format of an endpoint (e.g. add a required field or change a type), without versioning, all existing clients would break.

---

**Q2: When should you create a new version of your API?**

**A:** I create a new version on **breaking changes**:
- Removing a field from the response
- Changing a field's type (String → Integer)
- Changing the JSON structure (object → array)
- Changing business logic that affects contracts
- Renaming endpoints or parameters

**I do NOT create a new version for:**
- Adding new optional fields (backward compatible)
- Adding new endpoints
- Bug fixes
- Performance improvements

---

**Q3: What are the 4 main versioning strategies?**

**A:** 
1. **URI Path versioning**: `/api/v1/users`, `/api/v2/users`
2. **Query Parameter versioning**: `/api/users?version=1`
3. **Custom Header versioning**: `X-API-VERSION: 1`
4. **Content Negotiation versioning**: `Accept: application/vnd.company.app-v1+json`

Each has its advantages depending on the project context.

---

### **Questions: URI Path Versioning**

**Q4: How do you implement URI Path versioning in Spring Boot?**

**A:** I create separate controllers for each version:
```java
@RestController
@RequestMapping("/api/v1/users")
public class UserControllerV1 {
    @GetMapping("/{id}")
    public UserV1DTO getUser(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
}

@RestController
@RequestMapping("/api/v2/users")
public class UserControllerV2 {
    @GetMapping("/{id}")
    public UserV2DTO getUser(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Q5: What are the advantages and disadvantages of URI Path versioning?**

**A:** 

**Advantages:**
- ✅ **Very simple**: immediately visible in the URL
- ✅ **Easy to test**: I can test directly in the browser
- ✅ **Cache-friendly**: each version has its own unique URL, no confusion for proxies/CDN
- ✅ **Clear in logs**: you can see exactly which version is being called
- ✅ **Industry standard**: used by Twitter, Stripe, GitHub

**Disadvantages:**
- ❌ **Code duplication**: I must maintain multiple controllers
- ❌ **Different URLs**: the same resource has multiple URLs

**Recommendation:** This is my preferred strategy for 90% of projects because simplicity outweighs the drawbacks.

---

**Q6: How do you handle code duplication between v1 and v2?**

**A:** I use several techniques:

**1. Common Service Layer:**
```java
@Service
public class UserService {
    public User findById(Long id) {
        // Common business logic
    }
}

// Separate mappers for each version
@Component
public class UserMapperV1 {
    public UserV1DTO toDTO(User user) { ... }
}

@Component
public class UserMapperV2 {
    public UserV2DTO toDTO(User user) { ... }
}
```

**2. Controller inheritance:**
```java
public abstract class BaseUserController {
    @Autowired
    protected UserService userService;
    
    protected abstract Object mapToDTO(User user);
}

@RestController
@RequestMapping("/api/v1/users")
public class UserControllerV1 extends BaseUserController {
    protected UserV1DTO mapToDTO(User user) { ... }
}
```

**3. Composition via versioned services:**
```java
public interface UserVersionService {
    Object getUser(Long id);
}

@Service("userServiceV1")
public class UserServiceV1 implements UserVersionService { ... }

@Service("userServiceV2")
public class UserServiceV2 implements UserVersionService { ... }
```

---

### **Questions: Query Parameter Versioning**

**Q7: How do you implement Query Parameter versioning?**

**A:** I use the `params` attribute of the `@GetMapping` annotation:
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping(value = "/{id}", params = "version=1")
    public UserV1DTO getUserV1(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
    
    @GetMapping(value = "/{id}", params = "version=2")
    public UserV2DTO getUserV2(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
    
    // Default version if no parameter
    @GetMapping("/{id}")
    public UserV2DTO getUserDefault(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Requests:**
```
GET /api/users/1?version=1
GET /api/users/1?version=2
GET /api/users/1  (uses v2 by default)
```

---

**Q8: When would you use Query Parameter versioning?**

**A:** I use it mainly for:
- **Minor variations**: different representations of the same resource (e.g. short vs full format)
- **Feature flags**: enabling/disabling certain features
- **A/B testing**: testing different versions of an endpoint

**Example:**
```
GET /api/users/1?format=full
GET /api/users/1?format=summary
```

I **do not use it** for major API versions because it is less visible and non-standard REST.

---

### **Questions: Custom Header Versioning**

**Q9: How do you implement Custom Header versioning?**

**A:** I use Spring's `headers` attribute:
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping(value = "/{id}", headers = "X-API-VERSION=1")
    public UserV1DTO getUserV1(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
    
    @GetMapping(value = "/{id}", headers = "X-API-VERSION=2")
    public UserV2DTO getUserV2(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Request with curl:**
```bash
curl -H "X-API-VERSION: 1" http://localhost:8080/api/users/1
curl -H "X-API-VERSION: 2" http://localhost:8080/api/users/1
```

---

**Q10: What are the advantages of Custom Header versioning?**

**A:** 

**Advantages:**
- ✅ **Clean URL**: `/api/users/1` remains stable
- ✅ **Separation of concerns**: the version is in the metadata, not in the resource
- ✅ **Flexible**: you can add other headers for metadata (X-Client-ID, X-Feature-Flag)

**Disadvantages:**
- ❌ **Less visible**: hard to see which version is being used
- ❌ **Hard to test**: requires a tool like Postman or curl
- ❌ **Can be ignored** by some proxies or caches
- ❌ **Non-standard**: no official HTTP convention

**Usage:** I use it for **internal APIs** where I control the clients, rarely for public APIs.

---

### **Questions: Content Negotiation Versioning**

**Q11: What is Content Negotiation versioning?**

**A:** It is the use of the **standard HTTP `Accept` header** to negotiate not only the format (JSON, XML) but also the API version via a **custom media type**.

**Standard format:** `application/vnd.{vendor}.{resource}-v{version}+{format}`

**Examples:**
```
Accept: application/vnd.company.user-v1+json
Accept: application/vnd.company.user-v2+json
Accept: application/vnd.github.v3+json  (GitHub)
```

---

**Q12: How do you implement Content Negotiation versioning?**

**A:** I use the `produces` attribute:
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping(value = "/{id}", 
                produces = "application/vnd.company.app-v1+json")
    public UserV1DTO getUserV1(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
    
    @GetMapping(value = "/{id}", 
                produces = "application/vnd.company.app-v2+json")
    public UserV2DTO getUserV2(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
    
    // Fallback with generic application/json
    @GetMapping(value = "/{id}", 
                produces = MediaType.APPLICATION_JSON_VALUE)
    public UserV2DTO getUserDefault(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Request:**
```bash
curl -H "Accept: application/vnd.company.app-v1+json" \
     http://localhost:8080/api/users/1
```

---

**Q13: Why is Content Negotiation considered the most "RESTful" method?**

**A:** Because it respects **REST and HTTP principles**:

1. **Uniform Interface**: the URL represents the resource (`/users/1`), not the version
2. **Uses HTTP standards**: the `Accept` header is designed to negotiate representations
3. **Resource/representation separation**: the same resource can have multiple representations (v1, v2, JSON, XML)
4. **Roy Fielding's principle**: "Versioning should be a media type concern, not a URI concern"

**However**, in practice, even popular RESTful APIs (Twitter, Stripe) use URI Path because it is more pragmatic.

---

**Q14: When do you use Content Negotiation versioning?**

**A:** I use it when:
- I am building a **public API** that must strictly follow REST
- The client needs to **negotiate multiple aspects** (version + format)
- The team is experienced with advanced REST concepts

**Example:** GitHub API uses this pattern:
```
Accept: application/vnd.github.v3+json
Accept: application/vnd.github.v3.raw
Accept: application/vnd.github.v3.html
```

---

### **Questions: Versioning Cleaning & Best Practices**

**Q15: How do you manage deprecation of an old version?**

**A:** I follow a 4-step process:

**1. Warning Header:**
```java
@GetMapping("/api/v1/users/{id}")
@Deprecated
public ResponseEntity<UserV1DTO> getUserV1(@PathVariable Long id) {
    UserV1DTO user = userService.getUserV1(id);
    return ResponseEntity.ok()
        .header("Warning", "299 - \"API v1 is deprecated. Migrate to v2 by 2025-12-31\"")
        .header("Sunset", "Sun, 31 Dec 2025 23:59:59 GMT")
        .body(user);
}
```

**2. Swagger Documentation:**
```java
@Operation(
    summary = "Get user by ID (DEPRECATED)", 
    deprecated = true,
    description = "This endpoint is deprecated. Please use /api/v2/users/{id}"
)
```

**3. Monitoring & Metrics:**
```java
@GetMapping("/api/v1/users/{id}")
public ResponseEntity<UserV1DTO> getUserV1(@PathVariable Long id) {
    metricsService.incrementCounter("api.v1.usage");
    logger.warn("API v1 called by client: {}", request.getHeader("User-Agent"));
    // ...
}
```

**4. Communication:**
- Email to clients with migration timeline
- Blog post announcing the deprecation
- 6-12 month transition period

---

**Q16: What is the Sunset Header and how do you use it?**

**A:** The **Sunset Header** (RFC 8594) indicates the date on which an endpoint will be retired:

```java
@GetMapping("/api/v1/users/{id}")
public ResponseEntity<UserV1DTO> getUserV1(@PathVariable Long id) {
    return ResponseEntity.ok()
        .header("Sunset", "Sat, 31 Dec 2025 23:59:59 GMT")
        .header("Link", "</api/v2/users>; rel=\"successor-version\"")
        .body(user);
}
```

Clients can parse this header to automatically alert developers of the upcoming end of life.

---

**Q17: How do you safely remove an old version?**

**A:** I follow this process:

**Phase 1 - Announcement (T-12 months):**
- Add Warning and Sunset headers
- Document in Swagger
- Communicate to clients

**Phase 2 - Monitoring (T-6 months):**
- Track v1 usage via metrics
- Contact clients still using v1
- Provide migration guides

**Phase 3 - Read-Only (T-3 months):**
```java
@PostMapping("/api/v1/users")
public ResponseEntity<?> createUserV1(@RequestBody UserV1DTO user) {
    return ResponseEntity.status(HttpStatus.GONE)
        .body(new ErrorResponse("POST on v1 is disabled. Use v2"));
}

@GetMapping("/api/v1/users/{id}")  // GET still available
public UserV1DTO getUserV1(@PathVariable Long id) {
    return userService.getUserV1(id);
}
```

**Phase 4 - Removal (T-0):**
```java
@GetMapping("/api/v1/users/{id}")
public ResponseEntity<?> getUserV1(@PathVariable Long id) {
    return ResponseEntity.status(HttpStatus.GONE)
        .header("Link", "</api/v2/users/{id}>; rel=\"successor-version\"")
        .body(new ErrorResponse("v1 is retired. Use v2"));
}
```

Or simply delete the controller (HTTP 404).

---

**Q18: What is the difference between HTTP 410 Gone and 404 Not Found for a deprecated API?**

**A:** 
- **404 Not Found**: "This resource does not exist" (possibly a temporary error)
- **410 Gone**: "This resource existed but has been permanently removed" (more appropriate for deprecation)

```java
@GetMapping("/api/v1/users/{id}")
public ResponseEntity<?> getUserV1Retired(@PathVariable Long id) {
    ApiError error = new ApiError(
        HttpStatus.GONE,
        "API v1 has been retired on 2025-12-31",
        "/api/v2/users/" + id
    );
    return new ResponseEntity<>(error, HttpStatus.GONE);
}
```

**410** clearly tells clients they must stop retrying and migrate.

---

### **Questions: Comparison & Strategy Selection**

**Q19: Which versioning strategy do you recommend and why?**

**A:** My recommendation depends on the context:

**For 90% of projects: URI Path** (`/api/v1/users`)
- Simple, clear, industry standard
- Easy to test and debug
- Cache-friendly
- Used by major companies (Stripe, Twitter, AWS)

**For strictly RESTful public APIs: Content Negotiation**
- Respects REST principles
- Stable URL
- Used by GitHub

**For internal APIs with lots of metadata: Custom Headers**
- Flexible for feature flags
- Clean URL

**I NEVER use Query Parameters** for major versioning (only for minor variations).

---

**Q20: How do you test different API versions with Spring Boot?**

**A:** 

**Unit tests with MockMvc:**
```java
@WebMvcTest(UserController.class)
public class UserControllerTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    @Test
    public void testGetUserV1() throws Exception {
        mockMvc.perform(get("/api/v1/users/1"))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.name").value("John"))
            .andExpect(jsonPath("$.phoneNumber").doesNotExist()); // v1 doesn't have this field
    }
    
    @Test
    public void testGetUserV2() throws Exception {
        mockMvc.perform(get("/api/v2/users/1"))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.name").value("John"))
            .andExpect(jsonPath("$.phoneNumber").exists()); // v2 has this field
    }
    
    @Test
    public void testContentNegotiation() throws Exception {
        mockMvc.perform(get("/api/users/1")
                .accept("application/vnd.company.app-v1+json"))
            .andExpect(status().isOk())
            .andExpect(content().contentType("application/vnd.company.app-v1+json"));
    }
    
    @Test
    public void testCustomHeader() throws Exception {
        mockMvc.perform(get("/api/users/1")
                .header("X-API-VERSION", "2"))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.phoneNumber").exists());
    }
}
```

**Integration tests:**
```java
@SpringBootTest(webEnvironment = WebEnvironment.RANDOM_PORT)
public class VersioningIntegrationTest {
    
    @Autowired
    private TestRestTemplate restTemplate;
    
    @Test
    public void testV1AndV2ReturnDifferentStructures() {
        // Test v1
        UserV1DTO v1 = restTemplate.getForObject("/api/v1/users/1", UserV1DTO.class);
        assertNotNull(v1.getName());
        assertNull(v1.getPhoneNumber()); // v1 doesn't have this field
        
        // Test v2
        UserV2DTO v2 = restTemplate.getForObject("/api/v2/users/1", UserV2DTO.class);
        assertNotNull(v2.getName());
        assertNotNull(v2.getPhoneNumber()); // v2 has this field
    }
}
```

---

**Q21: How do you document the different versions in Swagger?**

**A:** 

**OpenAPI configuration with groups:**
```java
@Configuration
public class SwaggerConfig {
    
    @Bean
    public GroupedOpenApi publicApiV1() {
        return GroupedOpenApi.builder()
            .group("v1-public-api")
            .pathsToMatch("/api/v1/**")
            .build();
    }
    
    @Bean
    public GroupedOpenApi publicApiV2() {
        return GroupedOpenApi.builder()
            .group("v2-public-api")
            .pathsToMatch("/api/v2/**")
            .build();
    }
    
    @Bean
    public OpenAPI customOpenAPI() {
        return new OpenAPI()
            .info(new Info()
                .title("User Management API")
                .version("2.0")
                .description("Multi-version API with v1 (deprecated) and v2"));
    }
}
```

**Endpoint documentation:**
```java
@Tag(name = "Users V1 (DEPRECATED)", description = "User management endpoints - Version 1")
@RestController
@RequestMapping("/api/v1/users")
public class UserControllerV1 {
    
    @Operation(
        summary = "Get user by ID",
        deprecated = true,
        description = "⚠️ DEPRECATED: This version will be retired on 2025-12-31. Please use /api/v2/users/{id}"
    )
    @ApiResponse(responseCode = "200", description = "User found")
    @ApiResponse(responseCode = "410", description = "API version retired")
    @GetMapping("/{id}")
    public UserV1DTO getUser(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
}

@Tag(name = "Users V2", description = "User management endpoints - Version 2 (Current)")
@RestController
@RequestMapping("/api/v2/users")
public class UserControllerV2 {
    
    @Operation(summary = "Get user by ID", description = "Returns detailed user information including phone number")
    @GetMapping("/{id}")
    public UserV2DTO getUser(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

Swagger UI will display two separate groups, with v1 clearly marked as deprecated.

---

**Q22: Give a complete example of migrating an API from v1 to v2**

**A:** 

**Scenario:** Adding an `address` field (complex object) to User

**Models:**
```java
// V1 - Simple structure
@Data
public class UserV1DTO {
    private Long id;
    private String name;
    private String email;
}

// V2 - With structured address
@Data
public class UserV2DTO {
    private Long id;
    private String name;
    private String email;
    private AddressDTO address; // New field
}

@Data
public class AddressDTO {
    private String street;
    private String city;
    private String zipCode;
    private String country;
}
```

**Entity (single):**
```java
@Entity
public class User {
    @Id
    private Long id;
    private String name;
    private String email;
    
    // Fields added for v2
    private String street;
    private String city;
    private String zipCode;
    private String country;
}
```

**Mappers:**
```java
@Component
public class UserMapper {
    
    public UserV1DTO toV1DTO(User user) {
        return UserV1DTO.builder()
            .id(user.getId())
            .name(user.getName())
            .email(user.getEmail())
            .build();
        // Address fields are ignored
    }
    
    public UserV2DTO toV2DTO(User user) {
        AddressDTO address = null;
        if (user.getStreet() != null) {
            address = AddressDTO.builder()
                .street(user.getStreet())
                .city(user.getCity())
                .zipCode(user.getZipCode())
                .country(user.getCountry())
                .build();
        }
        
        return UserV2DTO.builder()
            .id(user.getId())
            .name(user.getName())
            .email(user.getEmail())
            .address(address)
            .build();
    }
}
```

**Controllers:**
```java
@RestController
public class UserControllerV1 {
    
    @Autowired
    private UserService userService;
    
    @Autowired
    private UserMapper mapper;
    
    @GetMapping("/api/v1/users/{id}")
    @Deprecated
    public ResponseEntity<UserV1DTO> getUser(@PathVariable Long id) {
        User user = userService.findById(id);
        UserV1DTO dto = mapper.toV1DTO(user);
        
        return ResponseEntity.ok()
            .header("Warning", "299 - \"v1 deprecated. Use v2\"")
            .header("Sunset", "Sat, 31 Dec 2025 23:59:59 GMT")
            .body(dto);
    }
}

@RestController
public class UserControllerV2 {
    
    @Autowired
    private UserService userService;
    
    @Autowired
    private UserMapper mapper;
    
    @GetMapping("/api/v2/users/{id}")
    public UserV2DTO getUser(@PathVariable Long id) {
        User user = userService.findById(id);
        return mapper.toV2DTO(user);
    }
}
```

**Responses:**

v1:
```json
{
  "id": 1,
  "name": "John Doe",
  "email": "john@example.com"
}
```

v2:
```json
{
  "id": 1,
  "name": "John Doe",
  "email": "john@example.com",
  "address": {
    "street": "123 Main St",
    "city": "Paris",
    "zipCode": "75001",
    "country": "France"
  }
}
```

---

**Q23: How do you handle versioning with Spring Data REST?**

**A:** Spring Data REST automatically generates REST endpoints. For versioning:

**Option 1: Projections (recommended):**
```java
// Projection v1
@Projection(name = "v1", types = User.class)
public interface UserV1Projection {
    Long getId();
    String getName();
    String getEmail();
}

// Projection v2
@Projection(name = "v2", types = User.class)
public interface UserV2Projection {
    Long getId();
    String getName();
    String getEmail();
    Address getAddress();
}

// Usage
GET /api/users/1?projection=v1
GET /api/users/1?projection=v2
```

**Option 2: RepositoryRestConfigurer:**
```java
@Configuration
public class RestConfiguration implements RepositoryRestConfigurer {
    
    @Override
    public void configureRepositoryRestConfiguration(RepositoryRestConfiguration config) {
        config.setBasePath("/api/v2");
    }
}
```

But in general, for complex versioning, I prefer **manual controllers** over Spring Data REST.

---

These questions cover all aspects of API versioning. Good luck with your interview! 🚀
