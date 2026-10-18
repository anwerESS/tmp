# Architecture Hexagonale — Cours Complet Spring Boot

---

## Table des matières

1. [C'est quoi l'architecture hexagonale ?](#1-cest-quoi-larchitecture-hexagonale-)
2. [Les grands concepts](#2-les-grands-concepts)
3. [Structure des packages](#3-structure-des-packages)
4. [Configuration Spring Boot](#4-configuration-spring-boot)
5. [Exemple complet pas à pas](#5-exemple-complet-pas-à-pas)
6. [Tests dans l'architecture hexagonale](#6-tests-dans-larchitecture-hexagonale)
7. [Avantages et inconvénients](#7-avantages-et-inconvénients)
8. [Questions / Réponses — Format oral](#8-questions--réponses--format-oral)

---

## 1. C'est quoi l'architecture hexagonale ?

L'architecture hexagonale (aussi appelée **Ports & Adapters**) a été inventée par **Alistair Cockburn** en 2005.

L'idée centrale : **le métier (domain) ne doit jamais dépendre des détails techniques** (base de données, API REST, Kafka, etc.).

> Imagine une boîte noire au centre : c'est ton métier. Autour, tu branche ce que tu veux (HTTP, SQL, messaging...) via des prises standardisées = les **ports**.

On l'appelle "hexagonale" car Cockburn représentait l'application comme un hexagone avec des ports sur chaque côté — pas parce qu'il y a forcément 6 côtés.

---

## 2. Les grands concepts

### 2.1 Le Domain (le cœur)

C'est **le cerveau de l'application**. Il contient :

- Les **Entities** : objets métier (ex: `Order`, `Customer`)
- Les **Value Objects** : objets sans identité propre (ex: `Money`, `Address`)
- Les **Domain Services** : logique métier qui ne rentre pas dans une entité
- Les **Use Cases** (ou Application Services) : les cas d'usage, ce que l'app sait faire

**Règle absolue : le domain ne dépend de RIEN d'externe.** Aucun import Spring, aucun import JPA ici.

---

### 2.2 Les Ports

Un **port** est une **interface Java**. Il définit un contrat, pas une implémentation.

Il existe deux types :

#### Inbound Ports (Driving Ports)
- Définissent **ce que l'application peut faire** (les use cases)
- Appelés **depuis l'extérieur** (ex: depuis un controller HTTP)
- Exemple : `interface OrderService { OrderId createOrder(CreateOrderCommand cmd); }`

#### Outbound Ports (Driven Ports)
- Définissent **ce dont l'application a besoin** de l'extérieur (ex: sauvegarder en BDD)
- Implémentés **par l'infrastructure**
- Exemple : `interface OrderRepository { void save(Order order); }`

---

### 2.3 Les Adapters

Un **adapter** est l'**implémentation concrète** d'un port. Il fait la traduction entre le monde extérieur et le domaine.

#### Inbound Adapters (Driving Adapters)
- Initialisent les appels vers le domaine
- Exemples : Controller REST, Consumer Kafka, CLI, Scheduler

#### Outbound Adapters (Driven Adapters)
- Implémentent les outbound ports
- Exemples : Repository JPA, Client HTTP externe, Sender email

---

### 2.4 Schéma global

```
┌─────────────────────────────────────────────────────────┐
│                                                         │
│   [REST Controller]  ──►  [Inbound Port]                │
│   [Kafka Consumer]   ──►  [Use Case / App Service]      │  ◄── Inbound Adapters
│                                                         │
│              ┌──────────────────────┐                   │
│              │       DOMAIN         │                   │
│              │  Entities, UseCases  │                   │
│              │  Value Objects       │                   │
│              └──────────────────────┘                   │
│                                                         │
│   [JPA Repository]  ◄──  [Outbound Port]                │
│   [HTTP Client]     ◄──  [Infrastructure impl]          │  ◄── Outbound Adapters
│                                                         │
└─────────────────────────────────────────────────────────┘
```

---

## 3. Structure des packages

Voici l'organisation recommandée pour un projet Spring Boot :

```
src/main/java/com/example/myapp/
│
├── domain/                          ← Le cœur métier (0 dépendance externe)
│   ├── model/
│   │   ├── Order.java               ← Entity
│   │   ├── OrderId.java             ← Value Object
│   │   └── OrderStatus.java         ← Enum métier
│   ├── port/
│   │   ├── in/                      ← Inbound Ports (interfaces use cases)
│   │   │   ├── CreateOrderUseCase.java
│   │   │   └── GetOrderUseCase.java
│   │   └── out/                     ← Outbound Ports (interfaces infra)
│   │       ├── OrderRepository.java
│   │       └── NotificationPort.java
│   └── service/
│       └── OrderDomainService.java  ← Application Service (implémente les use cases)
│
├── adapter/
│   ├── in/
│   │   ├── web/                     ← REST Controllers
│   │   │   ├── OrderController.java
│   │   │   └── OrderRequest.java    ← DTO entrant
│   │   └── messaging/               ← Consumers Kafka/RabbitMQ
│   │       └── OrderEventConsumer.java
│   └── out/
│       ├── persistence/             ← JPA adapters
│       │   ├── OrderJpaRepository.java
│       │   ├── OrderEntity.java     ← Entité JPA (@Entity)
│       │   └── OrderPersistenceAdapter.java
│       └── notification/            ← Adapteur email/SMS
│           └── EmailNotificationAdapter.java
│
└── config/                          ← Configuration Spring
    ├── BeanConfig.java
    └── SecurityConfig.java
```

> **Règle clé** : le package `domain` ne doit avoir **aucun import** de `adapter`, `config`, ou de librairies externes (Spring, JPA, etc.).

---

## 4. Configuration Spring Boot

### 4.1 Dépendances Maven (pom.xml)

```xml
<dependencies>
    <!-- Spring Boot de base -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-web</artifactId>
    </dependency>

    <!-- JPA pour la persistence -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-data-jpa</artifactId>
    </dependency>

    <!-- Base de données (H2 pour les tests, PostgreSQL en prod) -->
    <dependency>
        <groupId>com.h2database</groupId>
        <artifactId>h2</artifactId>
        <scope>runtime</scope>
    </dependency>

    <!-- Lombok (optionnel mais pratique) -->
    <dependency>
        <groupId>org.projectlombok</groupId>
        <artifactId>lombok</artifactId>
        <optional>true</optional>
    </dependency>

    <!-- Tests -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-test</artifactId>
        <scope>test</scope>
    </dependency>
</dependencies>
```

---

### 4.2 Le Domain — aucune annotation Spring

```java
// domain/model/Order.java
public class Order {
    private final OrderId id;
    private final String customerId;
    private OrderStatus status;
    private final List<OrderLine> lines;

    public Order(OrderId id, String customerId, List<OrderLine> lines) {
        this.id = id;
        this.customerId = customerId;
        this.lines = lines;
        this.status = OrderStatus.PENDING;
    }

    // Logique métier dans l'entité
    public void confirm() {
        if (this.status != OrderStatus.PENDING) {
            throw new IllegalStateException("Impossible de confirmer une commande non PENDING");
        }
        this.status = OrderStatus.CONFIRMED;
    }

    // Getters...
}
```

```java
// domain/model/OrderId.java  — Value Object
public record OrderId(UUID value) {
    public static OrderId generate() {
        return new OrderId(UUID.randomUUID());
    }
}
```

---

### 4.3 Les Inbound Ports (interfaces use cases)

```java
// domain/port/in/CreateOrderUseCase.java
public interface CreateOrderUseCase {
    OrderId createOrder(CreateOrderCommand command);
}
```

```java
// domain/port/in/CreateOrderCommand.java  — Command Object (pas de DTO Spring)
public record CreateOrderCommand(String customerId, List<OrderLineData> lines) {}
```

```java
// domain/port/in/GetOrderUseCase.java
public interface GetOrderUseCase {
    Order getOrder(OrderId orderId);
}
```

---

### 4.4 Les Outbound Ports (interfaces infra)

```java
// domain/port/out/OrderRepository.java
public interface OrderRepository {
    void save(Order order);
    Optional<Order> findById(OrderId id);
}
```

```java
// domain/port/out/NotificationPort.java
public interface NotificationPort {
    void notifyCustomer(String customerId, String message);
}
```

---

### 4.5 L'Application Service (implémentation des use cases)

```java
// domain/service/OrderService.java
// @Service ici est acceptable — c'est un bean Spring mais sans logique infra
@Service
@Transactional
public class OrderService implements CreateOrderUseCase, GetOrderUseCase {

    // Dépendances via les Outbound Ports — jamais via les implémentations directes !
    private final OrderRepository orderRepository;
    private final NotificationPort notificationPort;

    // Injection via constructeur (pas de @Autowired sur les champs)
    public OrderService(OrderRepository orderRepository,
                        NotificationPort notificationPort) {
        this.orderRepository = orderRepository;
        this.notificationPort = notificationPort;
    }

    @Override
    public OrderId createOrder(CreateOrderCommand command) {
        // Logique métier pure
        List<OrderLine> lines = command.lines().stream()
            .map(l -> new OrderLine(l.productId(), l.quantity()))
            .toList();

        Order order = new Order(OrderId.generate(), command.customerId(), lines);
        orderRepository.save(order);
        notificationPort.notifyCustomer(command.customerId(), "Votre commande a été créée !");
        return order.getId();
    }

    @Override
    public Order getOrder(OrderId orderId) {
        return orderRepository.findById(orderId)
            .orElseThrow(() -> new OrderNotFoundException(orderId));
    }
}
```

---

### 4.6 Inbound Adapter — REST Controller

```java
// adapter/in/web/OrderController.java
@RestController
@RequestMapping("/api/orders")
public class OrderController {

    // Dépend du PORT (interface), pas du service concret
    private final CreateOrderUseCase createOrderUseCase;
    private final GetOrderUseCase getOrderUseCase;

    public OrderController(CreateOrderUseCase createOrderUseCase,
                           GetOrderUseCase getOrderUseCase) {
        this.createOrderUseCase = createOrderUseCase;
        this.getOrderUseCase = getOrderUseCase;
    }

    @PostMapping
    public ResponseEntity<OrderResponse> createOrder(@RequestBody @Valid OrderRequest request) {
        // Mapping DTO → Command (le controller fait la traduction)
        CreateOrderCommand command = new CreateOrderCommand(
            request.getCustomerId(),
            request.getLines().stream()
                .map(l -> new OrderLineData(l.getProductId(), l.getQuantity()))
                .toList()
        );
        OrderId orderId = createOrderUseCase.createOrder(command);
        return ResponseEntity.status(HttpStatus.CREATED)
            .body(new OrderResponse(orderId.value().toString()));
    }

    @GetMapping("/{id}")
    public ResponseEntity<OrderResponse> getOrder(@PathVariable UUID id) {
        Order order = getOrderUseCase.getOrder(new OrderId(id));
        return ResponseEntity.ok(OrderResponse.from(order));
    }
}
```

---

### 4.7 Outbound Adapter — JPA Persistence

```java
// adapter/out/persistence/OrderEntity.java  — Entité JPA (jamais dans le domain !)
@Entity
@Table(name = "orders")
public class OrderEntity {
    @Id
    private UUID id;
    private String customerId;

    @Enumerated(EnumType.STRING)
    private OrderStatus status;

    // Mapping vers le domain object
    public Order toDomain() {
        return new Order(new OrderId(this.id), this.customerId, /* lines */ List.of());
    }

    public static OrderEntity fromDomain(Order order) {
        OrderEntity entity = new OrderEntity();
        entity.id = order.getId().value();
        entity.customerId = order.getCustomerId();
        entity.status = order.getStatus();
        return entity;
    }
}
```

```java
// adapter/out/persistence/OrderJpaRepository.java  — Spring Data
@Repository
public interface OrderJpaRepository extends JpaRepository<OrderEntity, UUID> {}
```

```java
// adapter/out/persistence/OrderPersistenceAdapter.java  — Implémente le Outbound Port
@Component
public class OrderPersistenceAdapter implements OrderRepository {

    private final OrderJpaRepository jpaRepository;

    public OrderPersistenceAdapter(OrderJpaRepository jpaRepository) {
        this.jpaRepository = jpaRepository;
    }

    @Override
    public void save(Order order) {
        jpaRepository.save(OrderEntity.fromDomain(order));
    }

    @Override
    public Optional<Order> findById(OrderId id) {
        return jpaRepository.findById(id.value())
            .map(OrderEntity::toDomain);
    }
}
```

---

### 4.8 Outbound Adapter — Notification

```java
// adapter/out/notification/EmailNotificationAdapter.java
@Component
public class EmailNotificationAdapter implements NotificationPort {

    @Override
    public void notifyCustomer(String customerId, String message) {
        // Logique d'envoi d'email (JavaMailSender, SendGrid, etc.)
        System.out.println("Email envoyé à " + customerId + " : " + message);
    }
}
```

---

### 4.9 Configuration Spring — Wiring des beans

Spring fait l'injection automatiquement via `@Service`, `@Component`, `@Repository`. Mais si tu as plusieurs implémentations du même port, tu peux qualifier :

```java
// config/BeanConfig.java
@Configuration
public class BeanConfig {

    // Si tu as plusieurs implémentations de NotificationPort
    @Bean
    @Primary
    public NotificationPort emailNotification() {
        return new EmailNotificationAdapter();
    }

    @Bean
    public NotificationPort smsNotification() {
        return new SmsNotificationAdapter();
    }
}
```

Ou avec `@Qualifier` à l'injection :

```java
public OrderService(@Qualifier("emailNotification") NotificationPort notificationPort,
                    OrderRepository orderRepository) {
    ...
}
```

---

### 4.10 application.properties

```properties
# Base de données
spring.datasource.url=jdbc:h2:mem:testdb
spring.datasource.driver-class-name=org.h2.Driver
spring.jpa.hibernate.ddl-auto=create-drop
spring.jpa.show-sql=true

# Logs
logging.level.com.example.myapp.domain=DEBUG
```

---

## 5. Exemple complet pas à pas

Voici le **flow d'une requête** POST `/api/orders` de bout en bout :

```
1. HTTP Request (POST /api/orders)
         │
         ▼
2. OrderController  [Inbound Adapter]
   - Valide le DTO (@Valid)
   - Convertit OrderRequest → CreateOrderCommand
         │
         ▼
3. CreateOrderUseCase  [Inbound Port = interface]
         │
         ▼ (Spring injecte OrderService)
4. OrderService  [Application Service = Domain]
   - Crée l'entité Order
   - Appelle orderRepository.save(order)  [via Outbound Port]
   - Appelle notificationPort.notifyCustomer(...)  [via Outbound Port]
         │
         ├──────────────────────────────┐
         ▼                              ▼
5a. OrderPersistenceAdapter       5b. EmailNotificationAdapter
    [Outbound Adapter]                 [Outbound Adapter]
    - Convertit Order → OrderEntity    - Envoie l'email
    - Appelle JpaRepository.save(...)
         │
         ▼
6. Base de données (H2/PostgreSQL)
```

---

## 6. Tests dans l'architecture hexagonale

L'architecture hexagonale **facilite énormément les tests** car le domain est isolé.

### 6.1 Unit test du Domain (sans Spring)

```java
// Test pur, pas de Spring, pas de Mockito complexe
class OrderServiceTest {

    // Fake (stub) du repository — pas besoin de Mockito ici
    private final OrderRepository fakeRepository = new InMemoryOrderRepository();
    private final NotificationPort fakeNotification = (customerId, msg) -> {};

    private final CreateOrderUseCase orderService =
        new OrderService(fakeRepository, fakeNotification);

    @Test
    void should_create_order_with_pending_status() {
        CreateOrderCommand command = new CreateOrderCommand("customer-1", List.of());

        OrderId orderId = orderService.createOrder(command);

        assertThat(orderId).isNotNull();
        Order saved = fakeRepository.findById(orderId).orElseThrow();
        assertThat(saved.getStatus()).isEqualTo(OrderStatus.PENDING);
    }
}

// Fake in-memory repository
class InMemoryOrderRepository implements OrderRepository {
    private final Map<OrderId, Order> store = new HashMap<>();

    @Override
    public void save(Order order) { store.put(order.getId(), order); }

    @Override
    public Optional<Order> findById(OrderId id) { return Optional.ofNullable(store.get(id)); }
}
```

### 6.2 Integration test de l'Adapter JPA

```java
@DataJpaTest  // Lance uniquement le contexte JPA
class OrderPersistenceAdapterTest {

    @Autowired
    private OrderJpaRepository jpaRepository;

    private OrderPersistenceAdapter adapter;

    @BeforeEach
    void setUp() {
        adapter = new OrderPersistenceAdapter(jpaRepository);
    }

    @Test
    void should_save_and_retrieve_order() {
        Order order = new Order(OrderId.generate(), "customer-1", List.of());
        adapter.save(order);

        Optional<Order> found = adapter.findById(order.getId());
        assertThat(found).isPresent();
        assertThat(found.get().getCustomerId()).isEqualTo("customer-1");
    }
}
```

### 6.3 Test du Controller (Inbound Adapter)

```java
@WebMvcTest(OrderController.class)  // Lance uniquement le contexte Web
class OrderControllerTest {

    @Autowired
    private MockMvc mockMvc;

    // Mock du use case — on isole le controller
    @MockBean
    private CreateOrderUseCase createOrderUseCase;

    @MockBean
    private GetOrderUseCase getOrderUseCase;

    @Test
    void should_return_201_when_order_created() throws Exception {
        OrderId newId = OrderId.generate();
        when(createOrderUseCase.createOrder(any())).thenReturn(newId);

        mockMvc.perform(post("/api/orders")
                .contentType(MediaType.APPLICATION_JSON)
                .content("""
                    { "customerId": "c1", "lines": [] }
                    """))
            .andExpect(status().isCreated())
            .andExpect(jsonPath("$.orderId").value(newId.value().toString()));
    }
}
```

---

## 7. Avantages et inconvénients

### Avantages

- **Testabilité maximale** : le domain se teste sans Spring, sans base de données
- **Indépendance technologique** : tu peux changer JPA → MongoDB sans toucher au domain
- **Séparation claire** : chaque couche a une responsabilité précise
- **Maintenabilité** : le code métier ne se noie pas dans du code technique
- **Parallélisation** : l'équipe peut travailler sur le domain et les adapters en parallèle

### Inconvénients

- **Verbosité** : beaucoup de classes (ports, adapters, commands, DTOs)
- **Sur-engineering** pour des petits projets CRUD simples
- **Mapping répétitif** : Domain Object → Entity JPA → DTO (plusieurs fois)
- **Courbe d'apprentissage** pour les devs habitués au MVC classique

> **Quand l'utiliser ?** Sur des projets avec une logique métier riche et complexe, pas pour un simple CRUD de 3 tables.

---

## 8. Questions / Réponses — Format oral

---

**Q1 : C'est quoi l'architecture hexagonale en quelques mots ?**

C'est une architecture qui place le métier au centre, totalement isolé des détails techniques. Le domain ne sait pas s'il parle à une base de données, une API REST ou Kafka. Il communique uniquement via des interfaces qu'on appelle des ports.

---

**Q2 : Quelle est la différence entre un port et un adapter ?**

Un port c'est une interface Java, un contrat. Par exemple `OrderRepository` est un port qui dit "je veux pouvoir sauvegarder une commande". L'adapter c'est l'implémentation concrète, par exemple `OrderPersistenceAdapter` qui utilise JPA pour faire le vrai travail.

---

**Q3 : C'est quoi la différence entre un inbound port et un outbound port ?**

Un inbound port c'est ce que l'application expose : ses use cases, ses actions. Par exemple `CreateOrderUseCase`. C'est l'extérieur qui appelle ce port.

Un outbound port c'est ce que l'application demande à l'extérieur. Par exemple `OrderRepository`. C'est le domain qui appelle ce port pour sauvegarder des données.

---

**Q4 : Est-ce que Spring peut être utilisé dans le domain ?**

Non, jamais. Le domain ne doit avoir aucun import Spring, aucune annotation `@Entity`, `@Component` ou autre. Le seul endroit où on peut tolérer `@Service` c'est sur l'application service, mais idéalement même ça on l'évite. Toute la configuration Spring va dans les adapters et la couche config.

---

**Q5 : Comment Spring sait quelle implémentation injecter dans l'application service ?**

Spring scanne les classes annotées `@Component`, `@Repository`, `@Service`. Par exemple `OrderPersistenceAdapter` est annoté `@Component` et implémente `OrderRepository`. Donc quand Spring voit que `OrderService` a besoin d'un `OrderRepository`, il injecte automatiquement `OrderPersistenceAdapter`. S'il y a plusieurs implémentations, on utilise `@Primary` ou `@Qualifier`.

---

**Q6 : Pourquoi on crée une entité JPA séparée de l'entité du domain ?**

Parce que l'entité JPA est une entité technique avec des annotations comme `@Id`, `@Column`, `@OneToMany`. Si on met tout ça dans le domain, notre objet métier devient couplé à JPA. Demain si on change de technologie de persistence, il faudrait modifier le domain. Avec des entités séparées, on fait un mapping dans l'adapter et le domain reste propre.

---

**Q7 : C'est quoi un Command Object ?**

C'est un objet simple (souvent un `record` en Java) qui transporte les données d'une requête vers un use case. Par exemple `CreateOrderCommand` contient le `customerId` et la liste des lignes. C'est différent du DTO HTTP : le command object appartient au domain, le DTO appartient à l'adapter web. L'adapter fait la conversion.

---

**Q8 : Comment tu testes le domain sans Spring ?**

C'est un des grands avantages de cette architecture. Le domain est du pur Java. Je crée de faux adapters en mémoire qui implémentent mes ports, je les injecte dans l'application service via le constructeur, et je teste sans lancer Spring. Les tests sont ultra-rapides.

---

**Q9 : Quelle est la différence avec une architecture MVC classique en couches ?**

Dans une architecture en couches classique (Controller → Service → Repository), le service dépend directement du repository. Si je change de base de données, je dois modifier le service. Avec l'architecture hexagonale, le service dépend d'une interface (le port), pas de l'implémentation. Le domain ne sait pas qu'il y a JPA en dessous. C'est l'inversion de dépendances poussée au maximum.

---

**Q10 : Quand est-ce que tu n'utiliserais PAS l'architecture hexagonale ?**

Pour un projet CRUD simple avec peu de logique métier, c'est souvent du sur-engineering. Tu te retrouves avec beaucoup de classes pour pas grand chose. Je l'utilise quand le métier est complexe, quand il y a beaucoup de règles, quand on prévoit de changer de technologie, ou quand plusieurs équipes travaillent en parallèle sur domain et infrastructure.

---

**Q11 : C'est quoi le lien avec le Domain-Driven Design (DDD) ?**

L'architecture hexagonale et le DDD sont complémentaires mais distincts. Le DDD parle de comment modéliser le domain (Aggregates, Entities, Value Objects, Bounded Contexts...). L'architecture hexagonale parle de comment organiser le code pour isoler ce domain. Dans la pratique, on les combine souvent : le domain hexagonal contient des concepts DDD.

---

**Q12 : Comment tu gères les transactions avec cette architecture ?**

En général, les transactions se gèrent au niveau de l'application service avec `@Transactional`. C'est le seul endroit logique car c'est le service qui orchestre plusieurs appels aux outbound ports. On n'annote pas les adapters directement avec `@Transactional` pour garder le contrôle transactionnel au niveau use case.

---

*Fin du cours — Bonne chance pour l'entretien !*
