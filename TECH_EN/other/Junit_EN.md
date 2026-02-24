# Complete JUnit & Mockito Guide for Interviews

## 1. TDD (Test-Driven Development)

### Principle
**Red → Green → Refactor**

1. **Red**: Write a failing test
2. **Green**: Write the minimum code to make the test pass
3. **Refactor**: Improve the code without breaking the tests

### Advantages
- More reliable and maintainable code
- Living documentation of the code
- Simpler design (YAGNI - You Aren't Gonna Need It)
- Early bug detection

### Test Types (Pyramid)
```
       /\
      /E2E\ 5% - End-to-End Tests (Selenium, Cypress)
     /────\
    /Integ\ 15-20% - Integration Tests (@SpringBootTest)
   /──────\
  / Unit  \ 70-80% - Unit Tests (@Mock)
 /────────\
```

---

## 2. JUnit 5 (Jupiter)

### Main Annotations

| Annotation | Usage | Execution timing |
|------------|-------|-----------------|
| `@Test` | Marks a test method | For each test |
| `@BeforeEach` | Initialization before each test | Before each @Test |
| `@AfterEach` | Cleanup after each test | After each @Test |
| `@BeforeAll` | Setup once (static) | Once before all tests |
| `@AfterAll` | Cleanup once (static) | Once after all tests |
| `@DisplayName` | Human-readable test name | - |
| `@Disabled` | Disables a test | - |
| `@RepeatedTest(n)` | Repeats the test n times | - |
| `@ParameterizedTest` | Test with multiple parameters | - |

### Common Assertions

```java
// Equality
assertEquals(expected, actual);
assertNotEquals(value1, value2);

// Booleans
assertTrue(condition);
assertFalse(condition);

// Nullity
assertNull(object);
assertNotNull(object);

// Exceptions
assertThrows(Exception.class, () -> method());

// Collections
assertIterableEquals(expected, actual);

// Timeout
assertTimeout(Duration.ofSeconds(1), () -> method());

// Multiple assertions (all executed even if one fails)
assertAll(
    () -> assertEquals(1, result.getId()),
    () -> assertEquals("John", result.getName())
);
```

### Complete Example

```java
@DisplayName("User Service Tests")
class UserServiceTest {
    
    private UserService userService;
    private UserRepository userRepository;
    
    @BeforeEach
    void setUp() {
        userRepository = mock(UserRepository.class);
        userService = new UserService(userRepository);
    }
    
    @Test
    @DisplayName("Should create user with valid data")
    void testCreateUser() {
        // Arrange
        User user = new User("John", "john@example.com");
        when(userRepository.save(any())).thenReturn(user);
        
        // Act
        User result = userService.createUser(user);
        
        // Assert
        assertNotNull(result);
        assertEquals("John", result.getName());
    }
    
    @ParameterizedTest
    @ValueSource(ints = {10, 15, 17})
    @DisplayName("Should reject underage users")
    void testUnderageUser(int age) {
        User user = new User("Minor", age);
        assertThrows(IllegalArgumentException.class, 
            () -> userService.createUser(user));
    }
}
```

---

## 3. Mockito - Mocking Framework

### Mockito Annotations

| Annotation | Description | Usage |
|------------|-------------|-------|
| `@Mock` | Creates a mock (fake object) | Pure unit tests |
| `@Spy` | Creates a spy (real object + spying) | Rare partial mocking |
| `@InjectMocks` | Injects mocks into the tested class | Class under test |
| `@Captor` | Captures arguments passed to methods | Advanced verification |

### Mockito Setup

```java
// JUnit 5
@ExtendWith(MockitoExtension.class)
class MyTest {
    @Mock
    private MyRepository repository;
    
    @InjectMocks
    private MyService service;
}

// OR manually
@BeforeEach
void setUp() {
    MockitoAnnotations.openMocks(this);
}
```

### Stubbing (Simulating behavior)

```java
// Return a value
when(repository.findById(1L)).thenReturn(Optional.of(user));

// Throw an exception
when(repository.save(any())).thenThrow(new RuntimeException("Error"));

// Return different values
when(repository.count())
    .thenReturn(1L)
    .thenReturn(2L)
    .thenReturn(3L);

// Response based on argument
when(repository.findById(anyLong())).thenAnswer(invocation -> {
    Long id = invocation.getArgument(0);
    return Optional.of(new User(id, "User" + id));
});

// For void methods
doNothing().when(repository).delete(any());
doThrow(new RuntimeException()).when(repository).delete(any());
```

### Verification (Verifying calls)

```java
// Verify a method was called
verify(repository).save(user);

// Verify number of calls
verify(repository, times(1)).save(any());
verify(repository, never()).delete(any());
verify(repository, atLeast(2)).findAll();
verify(repository, atMost(3)).findById(anyLong());

// Verify call order
InOrder inOrder = inOrder(repository, emailService);
inOrder.verify(repository).save(user);
inOrder.verify(emailService).sendEmail(anyString());

// Verify no other interactions occurred
verifyNoMoreInteractions(repository);

// Capture arguments
ArgumentCaptor<User> captor = ArgumentCaptor.forClass(User.class);
verify(repository).save(captor.capture());
User capturedUser = captor.getValue();
assertEquals("John", capturedUser.getName());
```

### @Mock vs @Spy

```java
// @Mock - Everything is fake
@Mock
private List<String> mockList;

@Test
void testMock() {
    mockList.add("test");
    assertEquals(0, mockList.size()); // Mock returns 0 by default
}

// @Spy - Real object + ability to mock
@Spy
private List<String> spyList = new ArrayList<>();

@Test
void testSpy() {
    spyList.add("test");
    assertEquals(1, spyList.size()); // Real method is called
    
    // You can still stub
    when(spyList.size()).thenReturn(100);
    assertEquals(100, spyList.size());
}
```

---

## 4. Spring Boot Testing

### Spring Test Annotations

| Annotation | Loads | Usage |
|------------|-------|-------|
| `@SpringBootTest` | Full context | E2E integration tests |
| `@WebMvcTest` | Web layer only | Controller tests (fast) |
| `@DataJpaTest` | JPA layer only | Repository tests |
| `@MockBean` | Mock in Spring context | Replace a bean |
| `@Autowired` | Dependency injection | Retrieve a bean |

### Tests with Spring

```java
// Pure unit test (FAST)
@ExtendWith(MockitoExtension.class)
class UserServiceTest {
    @Mock
    private UserRepository repository;
    
    @InjectMocks
    private UserService service;
}

// Spring integration test (SLOW)
@SpringBootTest
class UserServiceIntegrationTest {
    @MockBean
    private UserRepository repository;
    
    @Autowired
    private UserService service;
}

// Controller test (OPTIMAL)
@WebMvcTest(UserController.class)
class UserControllerTest {
    @Autowired
    private MockMvc mockMvc;
    
    @MockBean
    private UserService userService;
}

// Repository test with H2
@DataJpaTest
class UserRepositoryTest {
    @Autowired
    private UserRepository repository;
    
    @Autowired
    private TestEntityManager entityManager;
}
```

---

## 5. MockMvc - Controller Tests

### Setup

```java
@WebMvcTest(UserController.class)
class UserControllerTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    @MockBean
    private UserService userService;
    
    @Autowired
    private ObjectMapper objectMapper; // For JSON
}
```

### HTTP Requests

```java
// GET
mockMvc.perform(get("/api/users/{id}", 1))
    .andExpect(status().isOk())
    .andExpect(jsonPath("$.name").value("John"));

// POST with JSON
User user = new User("John", "john@example.com");
mockMvc.perform(post("/api/users")
        .contentType(MediaType.APPLICATION_JSON)
        .content(objectMapper.writeValueAsString(user)))
    .andExpect(status().isCreated())
    .andExpect(jsonPath("$.id").exists());

// PUT
mockMvc.perform(put("/api/users/{id}", 1)
        .contentType(MediaType.APPLICATION_JSON)
        .content(objectMapper.writeValueAsString(updatedUser)))
    .andExpect(status().isOk());

// DELETE
mockMvc.perform(delete("/api/users/{id}", 1))
    .andExpect(status().isNoContent());

// With parameters
mockMvc.perform(get("/api/users")
        .param("page", "0")
        .param("size", "10"))
    .andExpect(status().isOk());

// With headers
mockMvc.perform(get("/api/users/1")
        .header("Authorization", "Bearer token"))
    .andExpect(status().isOk());
```

### MockMvc Assertions

```java
// HTTP Status
.andExpect(status().isOk())              // 200
.andExpect(status().isCreated())         // 201
.andExpect(status().isNoContent())       // 204
.andExpect(status().isBadRequest())      // 400
.andExpect(status().isNotFound())        // 404

// JSON Path
.andExpect(jsonPath("$.name").value("John"))
.andExpect(jsonPath("$.age").value(25))
.andExpect(jsonPath("$.items", hasSize(3)))
.andExpect(jsonPath("$.email").exists())
.andExpect(jsonPath("$.items[0].name").value("Item1"))

// Content
.andExpect(content().contentType(MediaType.APPLICATION_JSON))
.andExpect(content().string("Success"))

// Headers
.andExpect(header().exists("Location"))
.andExpect(header().string("Content-Type", "application/json"))

// View (for classic MVC)
.andExpect(view().name("index"))
.andExpect(model().attributeExists("users"))

// Debug
.andDo(print()) // Prints the full request/response
```

---

## 6. Database Tests

### H2 In-Memory Database

```yaml
# application-test.yml
spring:
  datasource:
    url: jdbc:h2:mem:testdb
    driver-class-name: org.h2.Driver
  jpa:
    hibernate:
      ddl-auto: create-drop
    show-sql: true
```

### @DataJpaTest

```java
@DataJpaTest
class UserRepositoryTest {
    
    @Autowired
    private UserRepository repository;
    
    @Autowired
    private TestEntityManager entityManager;
    
    @Test
    void findByEmail_WhenExists_ReturnsUser() {
        // Arrange - Insert real data
        User user = new User("John", "john@example.com");
        entityManager.persist(user);
        entityManager.flush();
        
        // Act - Real SQL query on H2
        Optional<User> found = repository.findByEmail("john@example.com");
        
        // Assert
        assertTrue(found.isPresent());
        assertEquals("John", found.get().getName());
    }
}
```

### @Sql for loading scripts

```java
@DataJpaTest
@Sql("/test-data.sql") // Loads the script before the tests
class UserRepositoryTest {
    
    @Test
    void testWithPreloadedData() {
        List<User> users = repository.findAll();
        assertEquals(5, users.size());
    }
}
```

### Transactional & Rollback

```java
@SpringBootTest
@Transactional // Auto-rollback after each test
class UserServiceIntegrationTest {
    
    @Test
    void createUser_SavesInDatabase() {
        userService.createUser(new User("John"));
        // Automatic rollback after the test
    }
}
```

---

## 7. Reflection API

### What is Reflection?

Reflection allows inspecting and manipulating classes, methods and fields **at runtime**.

### Testing use cases

```java
// Access a private field
@Test
void testPrivateField() throws Exception {
    User user = new User();
    
    Field field = User.class.getDeclaredField("id");
    field.setAccessible(true); // Bypass private
    field.set(user, 100L);
    
    assertEquals(100L, user.getId());
}

// Call a private method
@Test
void testPrivateMethod() throws Exception {
    UserService service = new UserService();
    
    Method method = UserService.class.getDeclaredMethod("validate", String.class);
    method.setAccessible(true);
    boolean result = (boolean) method.invoke(service, "test@example.com");
    
    assertTrue(result);
}

// Verify annotations
@Test
void testAnnotations() {
    assertTrue(User.class.isAnnotationPresent(Entity.class));
    
    Field field = User.class.getDeclaredField("email");
    assertTrue(field.isAnnotationPresent(Column.class));
}
```

### ReflectionTestUtils (Spring)

```java
// Simpler than pure Reflection
@Test
void testWithReflectionTestUtils() {
    User user = new User();
    
    // Set private field
    ReflectionTestUtils.setField(user, "id", 100L);
    
    // Get private field
    Long id = (Long) ReflectionTestUtils.getField(user, "id");
    
    // Invoke private method
    Object result = ReflectionTestUtils.invokeMethod(service, "validate", "test");
    
    assertEquals(100L, id);
}
```

---

## 8. Beans and Dependency Injection

### What is a Bean?

A **Bean** is an object managed by the Spring container.

```java
@Service
public class UserService {
    private final UserRepository repository;
    
    // Constructor Injection (RECOMMENDED)
    public UserService(UserRepository repository) {
        this.repository = repository;
    }
}
```

### Bean Types for Tests

```java
// Real bean from Spring context
@Autowired
private UserService userService;

// Mock replacing a bean
@MockBean
private UserRepository userRepository;

// Spy of a real bean
@SpyBean
private EmailService emailService;
```

### Scopes

| Scope | Description | Usage |
|-------|-------------|-------|
| `@Singleton` (default) | Single instance | Services, repositories |
| `@Prototype` | New instance per injection | Stateful objects |
| `@RequestScope` | One instance per HTTP request | Web controllers |
| `@SessionScope` | One instance per user session | Cart, user data |

---

## 9. Best Practices

### Test Structure (AAA Pattern)

```java
@Test
void shouldReturnUserWhenIdExists() {
    // Arrange (Given) - Preparation
    User user = new User(1L, "John");
    when(repository.findById(1L)).thenReturn(Optional.of(user));
    
    // Act (When) - Action
    User result = service.getUserById(1L);
    
    // Assert (Then) - Verification
    assertNotNull(result);
    assertEquals("John", result.getName());
    verify(repository).findById(1L);
}
```

### Test Naming

```java
// ❌ Bad
@Test
void test1() { }

// ✅ Good - Format: should_ExpectedBehavior_When_StateUnderTest
@Test
void shouldReturnUser_WhenIdExists() { }

@Test
void shouldThrowException_WhenUserNotFound() { }

// ✅ Excellent - With @DisplayName
@Test
@DisplayName("Should return user when valid ID is provided")
void testGetUserById() { }
```

### Test Coverage

- **Minimum acceptable**: 70-80%
- **Optimal**: 80-90%
- **100%**: Often unnecessary (cost/benefit)

### What to test?

```
✅ SHOULD TEST:
- Business logic
- Validations
- Calculations
- Edge cases
- Error handling

❌ NOT WORTH TESTING:
- Simple getters/setters
- Auto-generated code
- Trivial methods
- Spring configuration
```

---

## 10. Common Interview Questions

### Q1: Difference between @Mock and @MockBean?

**Answer:**
- `@Mock`: Pure Mockito, fast unit tests, no Spring
- `@MockBean`: Mock in Spring context, replaces an existing bean

```java
// Unit test (fast)
@ExtendWith(MockitoExtension.class)
class Test {
    @Mock private UserRepository repo;
}

// Integration test (slow)
@SpringBootTest
class Test {
    @MockBean private UserRepository repo;
}
```

### Q2: Difference between @WebMvcTest and @SpringBootTest?

**Answer:**
- `@WebMvcTest`: Loads only the web layer, fast, ideal for testing controllers
- `@SpringBootTest`: Loads the entire Spring context, slow, for E2E tests

### Q3: When should you use @Spy?

**Answer:**
Rarely! Only if:
- You are testing legacy code that is hard to refactor
- You want to call real methods but spy on certain calls
- In general, prefer a proper mock or a real object

### Q4: How do you test a private method?

**Answer:**
**You shouldn't!** Private methods are tested indirectly through public methods.

If truly necessary:
- Use `ReflectionTestUtils` (Spring)
- Or extract the logic into a public method of a utility class

### Q5: Explain the Test Pyramid

**Answer:**
```
    E2E (5%)          ← Slow, fragile, expensive
   Integration (15%)  ← Tests with Spring, DB
  Unit Tests (80%)    ← Fast, isolated, numerous
```

The higher you go, the slower and more expensive.

### Q6: Difference between verify() and when()?

**Answer:**
- `when()`: **Stubbing** - define behavior (what the mock returns)
- `verify()`: **Verification** - verify that the method was called

```java
when(repo.save(any())).thenReturn(user);  // Stubbing
verify(repo).save(any());                  // Verification
```

### Q7: How do you test a void method?

**Answer:**
```java
// Stubbing for void
doNothing().when(service).deleteUser(1L);
doThrow(RuntimeException.class).when(service).deleteUser(1L);

// Verification
verify(service).deleteUser(1L);
```

---

## 11. Quick Interview Checklist

### Mockito
- [ ] Explain @Mock, @Spy, @InjectMocks
- [ ] when() vs verify()
- [ ] ArgumentCaptor
- [ ] any(), eq(), anyString()

### JUnit
- [ ] @Test, @BeforeEach, @AfterEach
- [ ] assertEquals, assertThrows, assertAll
- [ ] @ParameterizedTest

### Spring Testing
- [ ] @SpringBootTest vs @WebMvcTest vs @DataJpaTest
- [ ] @MockBean vs @Mock
- [ ] @Autowired in tests

### MockMvc
- [ ] perform(), andExpect()
- [ ] jsonPath()
- [ ] status(), content()

### Concepts
- [ ] TDD (Red-Green-Refactor)
- [ ] Test Pyramid
- [ ] AAA Pattern (Arrange-Act-Assert)
- [ ] Test coverage (70-80%)

---

## 12. Complete Example to Impress

```java
@WebMvcTest(UserController.class)
@DisplayName("User Controller Tests")
class UserControllerTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    @MockBean
    private UserService userService;
    
    @Autowired
    private ObjectMapper objectMapper;
    
    @Nested
    @DisplayName("GET /api/users/{id}")
    class GetUserById {
        
        @Test
        @DisplayName("Should return user when ID exists")
        void shouldReturnUser_WhenIdExists() throws Exception {
            // Arrange
            User user = new User(1L, "John", "john@example.com", 25);
            when(userService.getUserById(1L)).thenReturn(user);
            
            // Act & Assert
            mockMvc.perform(get("/api/users/{id}", 1))
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.id").value(1))
                .andExpect(jsonPath("$.name").value("John"))
                .andExpect(jsonPath("$.email").value("john@example.com"));
            
            verify(userService, times(1)).getUserById(1L);
        }
        
        @Test
        @DisplayName("Should return 404 when user not found")
        void shouldReturn404_WhenUserNotFound() throws Exception {
            // Arrange
            when(userService.getUserById(999L))
                .thenThrow(new ResourceNotFoundException("User not found"));
            
            // Act & Assert
            mockMvc.perform(get("/api/users/{id}", 999))
                .andExpect(status().isNotFound())
                .andExpect(jsonPath("$.message").value("User not found"));
        }
    }
    
    @Nested
    @DisplayName("POST /api/users")
    class CreateUser {
        
        @Test
        @DisplayName("Should create user with valid data")
        void shouldCreateUser_WithValidData() throws Exception {
            // Arrange
            User inputUser = new User(null, "Alice", "alice@example.com", 22);
            User savedUser = new User(1L, "Alice", "alice@example.com", 22);
            when(userService.createUser(any(User.class))).thenReturn(savedUser);
            
            // Act & Assert
            mockMvc.perform(post("/api/users")
                    .contentType(MediaType.APPLICATION_JSON)
                    .content(objectMapper.writeValueAsString(inputUser)))
                .andExpect(status().isCreated())
                .andExpect(jsonPath("$.id").value(1))
                .andExpect(jsonPath("$.name").value("Alice"));
            
            ArgumentCaptor<User> captor = ArgumentCaptor.forClass(User.class);
            verify(userService).createUser(captor.capture());
            assertEquals("Alice", captor.getValue().getName());
        }
        
        @ParameterizedTest
        @ValueSource(ints = {10, 15, 17})
        @DisplayName("Should reject underage users")
        void shouldRejectUnderageUsers(int age) throws Exception {
            User user = new User(null, "Minor", "minor@example.com", age);
            when(userService.createUser(any()))
                .thenThrow(new IllegalArgumentException("Must be 18+"));
            
            mockMvc.perform(post("/api/users")
                    .contentType(MediaType.APPLICATION_JSON)
                    .content(objectMapper.writeValueAsString(user)))
                .andExpect(status().isBadRequest());
        }
    }
}
```

---

## Good to know for the interview

1. **Always favour unit tests** (@Mock) - 80% of your tests
2. **@WebMvcTest for controllers** - fast and efficient
3. **@DataJpaTest for repositories** - with H2
4. **@SpringBootTest only for E2E integration** - slow, use sparingly
5. **Follow the AAA pattern** (Arrange-Act-Assert)
6. **Name tests clearly** - should_ExpectedBehavior_When_Condition
7. **Aim for 70-80% coverage** - not 100%
8. **Never test getters/setters** - waste of time
