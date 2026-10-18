# Guide Complet JUnit & Mockito pour Entretien

## 1. TDD (Test-Driven Development)

### Principe
**Red → Green → Refactor**

1. **Red**: Écrire un test qui échoue
2. **Green**: Écrire le code minimum pour que le test passe
3. **Refactor**: Améliorer le code sans casser les tests

### Avantages
- Code plus fiable et maintenable
- Documentation vivante du code
- Design plus simple (YAGNI - You Aren't Gonna Need It)
- Détection précoce des bugs

### Types de Tests (Pyramide)
```
       /\
      /E2E\ 5% - Tests End-to-End (Selenium, Cypress)
     /────\
    /Integ\ 15-20% - Tests d'intégration (@SpringBootTest)
   /──────\
  / Unit  \ 70-80% - Tests unitaires (@Mock)
 /────────\
```

---

## 2. JUnit 5 (Jupiter)

### Annotations Principales

| Annotation | Usage | Moment d'exécution |
|------------|-------|-------------------|
| `@Test` | Marque une méthode de test | Pour chaque test |
| `@BeforeEach` | Initialisation avant chaque test | Avant chaque @Test |
| `@AfterEach` | Nettoyage après chaque test | Après chaque @Test |
| `@BeforeAll` | Setup une seule fois (static) | Une fois avant tous les tests |
| `@AfterAll` | Cleanup une seule fois (static) | Une fois après tous les tests |
| `@DisplayName` | Nom lisible du test | - |
| `@Disabled` | Désactive un test | - |
| `@RepeatedTest(n)` | Répète le test n fois | - |
| `@ParameterizedTest` | Test avec plusieurs paramètres | - |

### Assertions Courantes

```java
// Égalité
assertEquals(expected, actual);
assertNotEquals(value1, value2);

// Booléens
assertTrue(condition);
assertFalse(condition);

// Nullité
assertNull(object);
assertNotNull(object);

// Exceptions
assertThrows(Exception.class, () -> method());

// Collections
assertIterableEquals(expected, actual);

// Timeout
assertTimeout(Duration.ofSeconds(1), () -> method());

// Assertions multiples (toutes exécutées même si une échoue)
assertAll(
    () -> assertEquals(1, result.getId()),
    () -> assertEquals("John", result.getName())
);
```

### Exemple Complet

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

### Annotations Mockito

| Annotation     | Description                                        | Usage                 |
| -------------- | -------------------------------------------------- | --------------------- |
| `@Mock`        | Creates a mock (fake object)                       | Pure unit tests       |
| `@Spy`         | Creates a spy (real object with spying capability) | Rare partial mocking  |
| `@InjectMocks` | Injects mocks into the class under test            | Class being tested    |
| `@Captor`      | Captures arguments passed to methods               | Advanced verification |


### Setup Mockito

```java
// JUnit 5
@ExtendWith(MockitoExtension.class)
class MyTest {
    @Mock
    private MyRepository repository;
    
    @InjectMocks
    private MyService service;
}

// OU manuellement
@BeforeEach
void setUp() {
    MockitoAnnotations.openMocks(this);
}
```

### Stubbing (Simuler le comportement)

```java
// Retourner une valeur
when(repository.findById(1L)).thenReturn(Optional.of(user));

// Lancer une exception
when(repository.save(any())).thenThrow(new RuntimeException("Error"));

// Retourner des valeurs différentes
when(repository.count())
    .thenReturn(1L)
    .thenReturn(2L)
    .thenReturn(3L);

// Réponse basée sur l'argument
when(repository.findById(anyLong())).thenAnswer(invocation -> {
    Long id = invocation.getArgument(0);
    return Optional.of(new User(id, "User" + id));
});

// Pour les méthodes void
doNothing().when(repository).delete(any());
doThrow(new RuntimeException()).when(repository).delete(any());
```

### Verification (Vérifier les appels)

```java
// Vérifier qu'une méthode a été appelée
verify(repository).save(user);

// Vérifier le nombre d'appels
verify(repository, times(1)).save(any());
verify(repository, never()).delete(any());
verify(repository, atLeast(2)).findAll();
verify(repository, atMost(3)).findById(anyLong());

// Vérifier l'ordre des appels
InOrder inOrder = inOrder(repository, emailService);
inOrder.verify(repository).save(user);
inOrder.verify(emailService).sendEmail(anyString());

// Vérifier qu'aucune autre interaction n'a eu lieu
verifyNoMoreInteractions(repository);

// Capturer les arguments
ArgumentCaptor<User> captor = ArgumentCaptor.forClass(User.class);
verify(repository).save(captor.capture());
User capturedUser = captor.getValue();
assertEquals("John", capturedUser.getName());
```

### @Mock vs @Spy

```java
// @Mock - Tout est fake
@Mock
private List<String> mockList;

@Test
void testMock() {
    mockList.add("test");
    assertEquals(0, mockList.size()); // Mock retourne 0 par défaut
}

// @Spy - Objet réel + possibilité de mocker
@Spy
private List<String> spyList = new ArrayList<>();

@Test
void testSpy() {
    spyList.add("test");
    assertEquals(1, spyList.size()); // Vraie méthode appelée
    
    // On peut quand même stubber
    when(spyList.size()).thenReturn(100);
    assertEquals(100, spyList.size());
}
```

---

## 4. Spring Boot Testing

### Annotations Spring Test

| Annotation | Charge | Utilisation |
|------------|--------|-------------|
| `@SpringBootTest` | Contexte complet | Tests d'intégration E2E |
| `@WebMvcTest` | Couche Web uniquement | Tests de contrôleurs (rapide) |
| `@DataJpaTest` | Couche JPA uniquement | Tests de repositories |
| `@MockBean` | Mock dans le contexte Spring | Remplacer un bean |
| `@Autowired` | Injection de dépendance | Récupérer un bean |

### Tests avec Spring

```java
// Test unitaire pur (RAPIDE)
@ExtendWith(MockitoExtension.class)
class UserServiceTest {
    @Mock
    private UserRepository repository;
    
    @InjectMocks
    private UserService service;
}

// Test d'intégration Spring (LENT)
@SpringBootTest
class UserServiceIntegrationTest {
    @MockBean
    private UserRepository repository;
    
    @Autowired
    private UserService service;
}

// Test de contrôleur (OPTIMAL)
@WebMvcTest(UserController.class)
class UserControllerTest {
    @Autowired
    private MockMvc mockMvc;
    
    @MockBean
    private UserService userService;
}

// Test de repository avec H2
@DataJpaTest
class UserRepositoryTest {
    @Autowired
    private UserRepository repository;
    
    @Autowired
    private TestEntityManager entityManager;
}
```

---

## 5. MockMvc - Tests de Contrôleurs

### Setup

```java
@WebMvcTest(UserController.class)
class UserControllerTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    @MockBean
    private UserService userService;
    
    @Autowired
    private ObjectMapper objectMapper; // Pour JSON
}
```

### Requêtes HTTP

```java
// GET
mockMvc.perform(get("/api/users/{id}", 1))
    .andExpect(status().isOk())
    .andExpect(jsonPath("$.name").value("John"));

// POST avec JSON
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

// Avec paramètres
mockMvc.perform(get("/api/users")
        .param("page", "0")
        .param("size", "10"))
    .andExpect(status().isOk());

// Avec headers
mockMvc.perform(get("/api/users/1")
        .header("Authorization", "Bearer token"))
    .andExpect(status().isOk());
```

### Assertions MockMvc

```java
// Status HTTP
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

// View (pour MVC classique)
.andExpect(view().name("index"))
.andExpect(model().attributeExists("users"))

// Debug
.andDo(print()) // Affiche la requête/réponse complète
```

---

## 6. Tests de Base de Données

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
        // Arrange - Insérer des données réelles
        User user = new User("John", "john@example.com");
        entityManager.persist(user);
        entityManager.flush();
        
        // Act - Requête SQL réelle sur H2
        Optional<User> found = repository.findByEmail("john@example.com");
        
        // Assert
        assertTrue(found.isPresent());
        assertEquals("John", found.get().getName());
    }
}
```

### @Sql pour charger des scripts

```java
@DataJpaTest
@Sql("/test-data.sql") // Charge le script avant les tests
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
@Transactional // Auto-rollback après chaque test
class UserServiceIntegrationTest {
    
    @Test
    void createUser_SavesInDatabase() {
        userService.createUser(new User("John"));
        // Rollback automatique après le test
    }
}
```

---

## 7. Reflection API

### Qu'est-ce que la Reflection?

La Reflection permet d'inspecter et manipuler les classes, méthodes et champs **à l'exécution**.

### Cas d'usage en testing

```java
// Accéder à un champ privé
@Test
void testPrivateField() throws Exception {
    User user = new User();
    
    Field field = User.class.getDeclaredField("id");
    field.setAccessible(true); // Contourner private
    field.set(user, 100L);
    
    assertEquals(100L, user.getId());
}

// Appeler une méthode privée
@Test
void testPrivateMethod() throws Exception {
    UserService service = new UserService();
    
    Method method = UserService.class.getDeclaredMethod("validate", String.class);
    method.setAccessible(true);
    boolean result = (boolean) method.invoke(service, "test@example.com");
    
    assertTrue(result);
}

// Vérifier les annotations
@Test
void testAnnotations() {
    assertTrue(User.class.isAnnotationPresent(Entity.class));
    
    Field field = User.class.getDeclaredField("email");
    assertTrue(field.isAnnotationPresent(Column.class));
}
```

### ReflectionTestUtils (Spring)

```java
// Plus simple que la Reflection pure
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

## 8. Beans et Injection de Dépendances

### Qu'est-ce qu'un Bean?

Un **Bean** est un objet géré par le conteneur Spring.

```java
@Service
public class UserService {
    private final UserRepository repository;
    
    // Constructor Injection (RECOMMANDÉ)
    public UserService(UserRepository repository) {
        this.repository = repository;
    }
}
```

### Types de Beans pour les Tests

```java
// Bean réel du contexte Spring
@Autowired
private UserService userService;

// Mock remplaçant un bean
@MockBean
private UserRepository userRepository;

// Spy d'un bean réel
@SpyBean
private EmailService emailService;
```

### Portées (Scopes)

| Scope | Description | Utilisation |
|-------|-------------|-------------|
| `@Singleton` (défaut) | Une seule instance | Services, repositories |
| `@Prototype` | Nouvelle instance à chaque injection | Objets stateful |
| `@RequestScope` | Une instance par requête HTTP | Controllers web |
| `@SessionScope` | Une instance par session utilisateur | Panier, user data |

---

## 9. Best Practices

### Structure d'un Test (AAA Pattern)

```java
@Test
void shouldReturnUserWhenIdExists() {
    // Arrange (Given) - Préparation
    User user = new User(1L, "John");
    when(repository.findById(1L)).thenReturn(Optional.of(user));
    
    // Act (When) - Action
    User result = service.getUserById(1L);
    
    // Assert (Then) - Vérification
    assertNotNull(result);
    assertEquals("John", result.getName());
    verify(repository).findById(1L);
}
```

### Nommage des Tests

```java
// ❌ Mauvais
@Test
void test1() { }

// ✅ Bon - Format: should_ExpectedBehavior_When_StateUnderTest
@Test
void shouldReturnUser_WhenIdExists() { }

@Test
void shouldThrowException_WhenUserNotFound() { }

// ✅ Excellent - Avec @DisplayName
@Test
@DisplayName("Should return user when valid ID is provided")
void testGetUserById() { }
```

### Test Coverage

- **Minimum acceptable**: 70-80%
- **Optimal**: 80-90%
- **100%**: Souvent inutile (coût/bénéfice)

### Que tester?

```
✅ À TESTER:
- Logique métier
- Validations
- Calculs
- Cas limites (edge cases)
- Gestion d'erreurs

❌ PAS LA PEINE:
- Getters/setters simples
- Code généré automatiquement
- Méthodes triviales
- Configuration Spring
```

---

## 10. Questions Fréquentes en Entretien

### Q1: Différence entre @Mock et @MockBean?

**Réponse:**
- `@Mock`: Mockito pur, tests unitaires rapides, pas de Spring
- `@MockBean`: Mock dans le contexte Spring, remplace un bean existant

```java
// Unit test (rapide)
@ExtendWith(MockitoExtension.class)
class Test {
    @Mock private UserRepository repo;
}

// Integration test (lent)
@SpringBootTest
class Test {
    @MockBean private UserRepository repo;
}
```

### Q2: Différence entre @WebMvcTest et @SpringBootTest?

**Réponse:**
- `@WebMvcTest`: Charge uniquement la couche web, rapide, idéal pour tester les contrôleurs
- `@SpringBootTest`: Charge tout le contexte Spring, lent, pour tests E2E

### Q3: Quand utiliser @Spy?

**Réponse:**
Rarement! Seulement si:
- Vous testez du code legacy difficile à refactorer
- Vous voulez appeler les vraies méthodes mais espionner certains appels
- En général, préférer un vrai mock ou un vrai objet

### Q4: Comment tester une méthode privée?

**Réponse:**
**Vous ne devriez pas!** Les méthodes privées sont testées indirectement via les méthodes publiques.

Si vraiment nécessaire:
- Utiliser `ReflectionTestUtils` (Spring)
- Ou extraire la logique dans une méthode publique d'une classe utilitaire

### Q5: Expliquer le Test Pyramid

**Réponse:**
```
    E2E (5%)      ← Lents, fragiles, coûteux
   Integration (15%) ← Tests avec Spring, DB
  Unit Tests (80%)  ← Rapides, isolés, nombreux
```

Plus on monte, plus c'est lent et coûteux.

### Q6: Différence entre verify() et when()?

**Réponse:**
- `when()`: **Stubbing** - définir le comportement (ce que le mock retourne)
- `verify()`: **Verification** - vérifier que la méthode a été appelée

```java
when(repo.save(any())).thenReturn(user);  // Stubbing
verify(repo).save(any());                  // Verification
```

### Q7: Comment tester une méthode void?

**Réponse:**
```java
// Stubbing pour void
doNothing().when(service).deleteUser(1L);
doThrow(RuntimeException.class).when(service).deleteUser(1L);

// Verification
verify(service).deleteUser(1L);
```

---

## 11. Checklist Rapide pour l'Entretien

### Mockito
- [ ] Expliquer @Mock, @Spy, @InjectMocks
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
- [ ] @Autowired dans les tests

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

## 12. Exemple Complet pour Impressionner

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

## Bon à savoir pour l'entretien

1. **Toujours privilégier les tests unitaires** (@Mock) - 80% de tes tests
2. **@WebMvcTest pour les contrôleurs** - rapide et efficace
3. **@DataJpaTest pour les repositories** - avec H2
4. **@SpringBootTest seulement pour l'intégration E2E** - lent, à utiliser avec parcimonie
5. **Suivre le pattern AAA** (Arrange-Act-Assert)
6. **Nommer les tests clairement** - should_ExpectedBehavior_When_Condition
7. **Viser 70-80% de coverage** - pas 100%
8. **Ne jamais tester les getters/setters** - perte de temps
