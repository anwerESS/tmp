## [**..**](./00_index.md)

# REST API Design & Filtering

## 🏗️ REST API Design - Architecture et Bonnes Pratiques

### Architecture en Couches

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

**Séparation des responsabilités :**
- **Controller** : Gestion des requêtes/réponses HTTP, validation, mapping DTOs
- **Service** : Logique métier, transactions
- **Repository** : Accès données, requêtes

### Resources REST - Conventions de Nommage

**Best Practices :**
```
GET    /api/users              → Liste des utilisateurs
GET    /api/users/{id}         → Un utilisateur spécifique
POST   /api/users              → Créer un utilisateur
PUT    /api/users/{id}         → Mettre à jour complètement
PATCH  /api/users/{id}         → Mise à jour partielle
DELETE /api/users/{id}         → Supprimer

GET    /api/users/{id}/orders  → Commandes d'un utilisateur (ressources imbriquées)
```

**Règles :**
- Utiliser des **noms** (pas de verbes) : `/users` pas `/getUsers`
- **Pluriel** pour les collections : `/users` pas `/user`
- **Lowercase** avec tirets : `/user-profiles` pas `/UserProfiles`
- **Hiérarchie** pour les relations : `/users/{userId}/orders/{orderId}`

### DTOs (Data Transfer Objects) - Séparation Entity/API

**Pourquoi utiliser des DTOs ?**

```java
// ❌ Mauvaise pratique : Exposer l'entité directement
@RestController
public class UserController {
    
    @GetMapping("/users/{id}")
    public User getUser(@PathVariable Long id) {
        return userService.findById(id);  // Expose TOUT (password, relations, etc.)
    }
}

// ✅ Bonne pratique : Utiliser des DTOs
@RestController
public class UserController {
    
    @GetMapping("/users/{id}")
    public ResponseEntity<UserDTO> getUser(@PathVariable Long id) {
        User user = userService.findById(id);
        UserDTO dto = new UserDTO(user);  // Contrôle sur les champs exposés
        return ResponseEntity.ok(dto);
    }
}
```

**Avantages des DTOs :**
- **Sécurité** : Ne pas exposer champs sensibles (password, internal IDs)
- **Découplage** : API indépendante de la structure DB
- **Flexibilité** : Différents DTOs pour différents endpoints
- **Performance** : Réduire les données transmises
- **Versioning** : API stable même si l'entité change

### Architecture Complète avec DTOs

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
    private String password;  // Sensible !
    
    @OneToMany(mappedBy = "user")
    private List<Order> orders;
    
    @CreatedDate
    private LocalDateTime createdAt;
    
    @LastModifiedDate
    private LocalDateTime updatedAt;
}

// DTOs (Presentation Layer)

// DTO pour la création
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

// DTO pour la réponse (pas de password)
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
        // Pas de password !
    }
}

// DTO pour la liste (encore moins d'infos)
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
    
    // Liste - DTO summary
    @GetMapping
    public ResponseEntity<List<UserSummaryDTO>> getAllUsers() {
        List<User> users = userService.findAll();
        List<UserSummaryDTO> dtos = users.stream()
            .map(UserSummaryDTO::new)
            .collect(Collectors.toList());
        return ResponseEntity.ok(dtos);
    }
    
    // Détails - DTO complet
    @GetMapping("/{id}")
    public ResponseEntity<UserResponse> getUser(@PathVariable Long id) {
        User user = userService.findById(id);
        return ResponseEntity.ok(new UserResponse(user));
    }
    
    // Création - Request DTO
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

### Mapper - Conversion Entity ↔ DTO

**Option 1 : Constructeur dans DTO**
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

**Option 2 : Classe Mapper dédiée**
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

**Option 3 : MapStruct (bibliothèque automatique)**
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

## 🔍 Filtering - Contrôle des Champs Retournés

### Problématique du Filtering

```java
// Sans filtering
@Entity
public class User {
    private Long id;
    private String username;
    private String password;  // ❌ Ne devrait JAMAIS être exposé !
    private String ssn;       // ❌ Données sensibles
}

// JSON retourné (DANGER !)
{
  "id": 1,
  "username": "john",
  "password": "hashed123",  // 😱 Exposé !
  "ssn": "123-45-6789"      // 😱 Données sensibles !
}
```

**Solution : Filtering pour contrôler quels champs sont sérialisés en JSON**

### Static Filtering - Même Filtrage Partout

**Static Filtering** : Le même filtrage est appliqué à tous les endpoints utilisant le bean.

#### Avec @JsonIgnore

```java
@Entity
public class User {
    private Long id;
    private String username;
    
    @JsonIgnore  // Jamais inclus dans la réponse JSON
    private String password;
    
    @JsonIgnore
    private String ssn;
    
    private String email;
}

// JSON retourné (partout où User est utilisé)
{
  "id": 1,
  "username": "john",
  "email": "john@example.com"
  // password et ssn ignorés
}
```

#### Avec @JsonIgnoreProperties (Niveau Classe)

```java
@Entity
@JsonIgnoreProperties({"password", "ssn", "internalId"})
public class User {
    private Long id;
    private String username;
    private String password;     // Ignoré
    private String ssn;          // Ignoré
    private String internalId;   // Ignoré
    private String email;
}

// JSON retourné
{
  "id": 1,
  "username": "john",
  "email": "john@example.com"
}
```

#### Autres Annotations de Static Filtering

```java
@Entity
public class User {
    
    // Inclure seulement à la sérialisation (GET), ignorer à la désérialisation (POST)
    @JsonProperty(access = JsonProperty.Access.READ_ONLY)
    private Long id;
    
    // Ignorer à la sérialisation (GET), inclure à la désérialisation (POST)
    @JsonProperty(access = JsonProperty.Access.WRITE_ONLY)
    private String password;
    
    // Renommer le champ dans le JSON
    @JsonProperty("user_name")
    private String username;
    
    // Format de date personnalisé
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
  // Pas de password
}
```

**Avantages Static Filtering :**
- ✅ Simple et déclaratif
- ✅ Cohérent sur tous les endpoints
- ✅ Performant (pas de traitement runtime)

**Inconvénients :**
- ❌ Pas flexible (même filtrage partout)
- ❌ Si besoin de différents champs selon l'endpoint → problème

### Dynamic Filtering - Filtrage par Endpoint

**Dynamic Filtering** : Personnaliser les champs retournés pour chaque endpoint spécifique.

#### Configuration avec @JsonFilter

```java
// 1. Annoter l'entité avec @JsonFilter
@Entity
@JsonFilter("UserFilter")  // Nom du filtre
public class User {
    private Long id;
    private String username;
    private String email;
    private String password;
    private String phoneNumber;
    private String address;
}
```

#### Implémentation dans le Controller

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @Autowired
    private UserService userService;
    
    // Endpoint 1 : Seulement id et username
    @GetMapping("/summary")
    public ResponseEntity<MappingJacksonValue> getUsersSummary() {
        List<User> users = userService.findAll();
        
        // Créer le filtre dynamique
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .filterOutAllExcept("id", "username");
        
        FilterProvider filters = new SimpleFilterProvider()
            .addFilter("UserFilter", filter);
        
        // Appliquer le filtre
        MappingJacksonValue mapping = new MappingJacksonValue(users);
        mapping.setFilters(filters);
        
        return ResponseEntity.ok(mapping);
    }
    
    // Endpoint 2 : Détails complets SAUF password
    @GetMapping("/{id}")
    public ResponseEntity<MappingJacksonValue> getUser(@PathVariable Long id) {
        User user = userService.findById(id);
        
        // Filtre différent : tout SAUF password
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .serializeAllExcept("password");
        
        FilterProvider filters = new SimpleFilterProvider()
            .addFilter("UserFilter", filter);
        
        MappingJacksonValue mapping = new MappingJacksonValue(user);
        mapping.setFilters(filters);
        
        return ResponseEntity.ok(mapping);
    }
    
    // Endpoint 3 : Seulement certains champs
    @GetMapping("/contacts")
    public ResponseEntity<MappingJacksonValue> getUserContacts() {
        List<User> users = userService.findAll();
        
        // Seulement infos de contact
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

#### Résultats JSON par Endpoint

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
  // password filtré
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

### Méthode Helper pour Simplifier le Code

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
    
    // Utilisation simplifiée
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

### Dynamic Filtering avec @RequestParam (Filtres Dynamiques par Client)

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
    
    // Pas de filtrage si fields non spécifié
    return ResponseEntity.ok(new MappingJacksonValue(users));
}

// Utilisation:
// GET /api/users?fields=id,username
// GET /api/users?fields=username,email,phoneNumber
```

## 📊 Comparaison : Static vs Dynamic Filtering

| Aspect | Static Filtering | Dynamic Filtering |
|--------|------------------|-------------------|
| **Configuration** | Annotations sur l'entité | Code dans le Controller |
| **Flexibilité** | ❌ Même partout | ✅ Personnalisable par endpoint |
| **Performance** | ✅ Meilleure | Légèrement moins performant |
| **Complexité** | ✅ Simple | Plus complexe |
| **Use Case** | Champs toujours cachés (password) | Différents niveaux de détails |
| **Maintenance** | ✅ Facile | Nécessite plus de code |

## 🎯 Recommandations

**Stratégie Hybride (Best Practice) :**

```java
// 1. Static Filtering pour champs TOUJOURS cachés
@Entity
@JsonIgnoreProperties({"password", "ssn"})
@JsonFilter("UserFilter")  // Pour dynamic filtering optionnel
public class User {
    private Long id;
    private String username;
    
    @JsonIgnore
    private String password;  // Double sécurité
    
    private String email;
    private String phoneNumber;
    private String address;
}

// 2. DTOs pour différents niveaux de détails (préféré)
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

// 3. Dynamic Filtering seulement si vraiment nécessaire
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

**Règle d'Or :**
- **Static Filtering** : Sécurité (password, données sensibles)
- **DTOs** : Différents niveaux de détails (summary vs full)
- **Dynamic Filtering** : Cas spéciaux où le client choisit les champs

---

Ce résumé couvre l'architecture REST API, l'utilisation des DTOs pour la séparation des responsabilités, et les deux approches de filtering (static et dynamic) avec leurs cas d'usage respectifs !


# Questions & Réponses - REST API Design & Filtering

## 🏗️ REST API Design & Architecture

**Q1 : Quelles sont les couches d'une architecture REST API typique ?**
**R :**
```
Client → Controller → Service → Repository → Database
```
- **Controller** : Gestion requêtes HTTP, validation, mapping DTOs
- **Service** : Logique métier, transactions
- **Repository** : Accès données, requêtes
- **Séparation des responsabilités** pour maintenabilité et testabilité

**Q2 : Quelles sont les conventions de nommage REST pour les resources ?**
**R :**
- Utiliser des **noms** (pas de verbes) : `/users` pas `/getUsers`
- **Pluriel** pour collections : `/users` pas `/user`
- **Lowercase** avec tirets : `/user-profiles` pas `/UserProfiles`
- **Hiérarchie** pour relations : `/users/{userId}/orders/{orderId}`
- **Pas de trailing slash** : `/users` pas `/users/`

**Q3 : Quelles méthodes HTTP utiliser pour quelles opérations CRUD ?**
**R :**
```
GET    /api/users          → Lire (liste)
GET    /api/users/{id}     → Lire (un élément)
POST   /api/users          → Créer
PUT    /api/users/{id}     → Remplacer complètement
PATCH  /api/users/{id}     → Mise à jour partielle
DELETE /api/users/{id}     → Supprimer
```

**Q4 : Comment structurer les URLs pour les ressources imbriquées (nested resources) ?**
**R :**
```java
// Ressources imbriquées
GET    /users/{userId}/orders           // Commandes d'un utilisateur
GET    /users/{userId}/orders/{orderId} // Une commande spécifique
POST   /users/{userId}/orders           // Créer une commande
DELETE /users/{userId}/orders/{orderId} // Supprimer

// Alternative avec filtrage
GET /orders?userId=5
```
Préférer les ressources imbriquées pour relations fortes, filtrage pour relations faibles.

**Q5 : Qu'est-ce que l'idempotence et quelles méthodes HTTP sont idempotentes ?**
**R :** Une opération est idempotente si l'exécuter plusieurs fois produit le même résultat qu'une seule fois.
- **Idempotentes** : GET, PUT, DELETE (retry safe)
- **Non-idempotentes** : POST (créer 2 fois = 2 ressources)

Important pour la fiabilité des APIs (retry après timeout).

## 📦 DTOs (Data Transfer Objects)

**Q6 : Qu'est-ce qu'un DTO et pourquoi l'utiliser ?**
**R :** Un DTO (Data Transfer Object) est un objet utilisé pour transférer des données entre les couches, spécifiquement entre le Controller et le Client. Il sépare la structure de l'API de la structure interne (Entity).

**Q7 : Pourquoi ne pas exposer les Entities directement dans l'API ?**
**R :**
- ❌ **Sécurité** : Expose des champs sensibles (password, internal IDs)
- ❌ **Couplage** : API dépend de la structure DB
- ❌ **Performance** : Peut exposer des relations entières (lazy loading issues)
- ❌ **Flexibilité** : Impossible d'avoir différentes représentations

**Q8 : Quels sont les avantages des DTOs ?**
**R :**
- ✅ **Sécurité** : Contrôle précis des champs exposés
- ✅ **Découplage** : API indépendante de la DB
- ✅ **Flexibilité** : Différents DTOs pour différents endpoints
- ✅ **Performance** : Réduire les données transmises
- ✅ **Versioning** : API stable même si Entity change
- ✅ **Validation** : Règles différentes pour input/output

**Q9 : Exemple de problème sans DTO**
**R :**
```java
// ❌ Sans DTO - Expose tout
@RestController
public class UserController {
    @GetMapping("/users/{id}")
    public User getUser(@PathVariable Long id) {
        return userService.findById(id);
    }
}

// JSON retourné (DANGER !)
{
  "id": 1,
  "username": "john",
  "password": "hashed123",  // 😱 Exposé !
  "ssn": "123-45-6789",     // 😱 Données sensibles !
  "orders": [...]           // 😱 Relations chargées
}
```

**Q10 : Créez une architecture complète avec DTOs (Request/Response)**
**R :**
```java
// Entity (ne jamais exposer)
@Entity
public class User {
    @Id
    private Long id;
    private String username;
    private String email;
    private String password;  // Sensible
    private LocalDateTime createdAt;
}

// DTO Request (création)
public class CreateUserRequest {
    @NotBlank
    private String username;
    
    @Email
    private String email;
    
    @Size(min = 8)
    private String password;
}

// DTO Response (pas de password)
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

**Q11 : Quelle est la différence entre un Request DTO et un Response DTO ?**
**R :**
- **Request DTO** : Données entrantes (POST/PUT), contient validations, peut inclure password
- **Response DTO** : Données sortantes (GET), exclut champs sensibles, peut inclure champs calculés

**Q12 : Où placer la logique de conversion Entity ↔ DTO ?**
**R :** **3 approches :**

**1. Constructeur dans le DTO**
```java
public class UserResponse {
    public UserResponse(User user) {
        this.id = user.getId();
        this.username = user.getUsername();
    }
}
```

**2. Classe Mapper dédiée**
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

**3. MapStruct (génération automatique)**
```java
@Mapper(componentModel = "spring")
public interface UserMapper {
    UserResponse toResponse(User user);
    List<UserResponse> toResponseList(List<User> users);
}
```

**Q13 : Quand utiliser des DTOs différents pour le même endpoint ?**
**R :**
```java
// DTO Summary pour liste (moins de détails)
public class UserSummaryDTO {
    private Long id;
    private String username;
}

// DTO Detail pour un élément (plus de détails)
public class UserDetailDTO {
    private Long id;
    private String username;
    private String email;
    private String phoneNumber;
    private List<OrderSummaryDTO> recentOrders;
}

// Controller
@GetMapping          // Liste
public List<UserSummaryDTO> getAll() { ... }

@GetMapping("/{id}") // Détails
public UserDetailDTO getOne(@PathVariable Long id) { ... }
```

**Q14 : Comment gérer les relations dans les DTOs (éviter les références circulaires) ?**
**R :**
```java
// Entity avec relation bidirectionnelle
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

// ❌ Problème: JSON infini (User → Orders → User → ...)

// ✅ Solution: DTOs sans référence inverse
public class UserResponse {
    private Long id;
    private String username;
    private List<OrderSummaryDTO> orders;  // Pas de User dans OrderSummaryDTO
}

public class OrderSummaryDTO {
    private Long id;
    private String productName;
    // Pas de User ici !
}
```

## 🔍 Filtering - Général

**Q15 : Qu'est-ce que le filtering dans une REST API ?**
**R :** Le filtering contrôle quels champs d'un objet sont inclus dans la réponse JSON. Permet de :
- Masquer des données sensibles (password, SSN)
- Optimiser les performances (moins de données transférées)
- Fournir différents niveaux de détails selon l'endpoint

**Q16 : Quels sont les deux types de filtering ?**
**R :**
- **Static Filtering** : Même filtrage pour un bean sur tous les endpoints
- **Dynamic Filtering** : Filtrage personnalisé par endpoint spécifique

**Q17 : Quelle est la différence entre Static et Dynamic Filtering ?**
**R :**
| Aspect | Static | Dynamic |
|--------|--------|---------|
| **Configuration** | Annotations sur Entity | Code dans Controller |
| **Flexibilité** | Même partout | Personnalisable |
| **Performance** | Meilleure | Légèrement moins |
| **Complexité** | Simple | Plus complexe |
| **Use Case** | Champs toujours cachés | Différents niveaux |

## 🔒 Static Filtering

**Q18 : Qu'est-ce que le Static Filtering ?**
**R :** Static Filtering applique le même filtrage à tous les endpoints utilisant le bean. Configuré avec des annotations sur l'Entity. Une fois défini, le champ est toujours inclus ou exclu.

**Q19 : Comment utiliser @JsonIgnore ?**
**R :**
```java
@Entity
public class User {
    private Long id;
    private String username;
    
    @JsonIgnore  // Jamais sérialisé en JSON
    private String password;
    
    @JsonIgnore
    private String ssn;
}

// JSON retourné (tous les endpoints)
{
  "id": 1,
  "username": "john"
  // password et ssn toujours ignorés
}
```

**Q20 : Comment utiliser @JsonIgnoreProperties au niveau classe ?**
**R :**
```java
@Entity
@JsonIgnoreProperties({"password", "ssn", "internalId"})
public class User {
    private Long id;
    private String username;
    private String password;     // Ignoré
    private String ssn;          // Ignoré
    private String internalId;   // Ignoré
}

// Équivalent à mettre @JsonIgnore sur chaque champ
```

**Q21 : Quelle est la différence entre @JsonIgnore et @JsonIgnoreProperties ?**
**R :**
- **@JsonIgnore** : Sur le champ, ignore ce champ spécifique
- **@JsonIgnoreProperties** : Sur la classe, liste plusieurs champs à ignorer

Les deux ont le même effet. @JsonIgnoreProperties est plus lisible quand plusieurs champs.

**Q22 : Comment utiliser @JsonProperty avec access control ?**
**R :**
```java
@Entity
public class User {
    // READ_ONLY: Inclus en GET, ignoré en POST/PUT
    @JsonProperty(access = JsonProperty.Access.READ_ONLY)
    private Long id;
    
    @JsonProperty(access = JsonProperty.Access.READ_ONLY)
    private LocalDateTime createdAt;
    
    // WRITE_ONLY: Accepté en POST/PUT, jamais retourné en GET
    @JsonProperty(access = JsonProperty.Access.WRITE_ONLY)
    private String password;
}

// POST Request (accepté)
{
  "username": "john",
  "password": "secret"
}

// GET Response (pas de password)
{
  "id": 1,
  "username": "john",
  "createdAt": "2025-01-15T10:30:00"
}
```

**Q23 : Autres annotations Jackson utiles pour Static Filtering ?**
**R :**
```java
@Entity
public class User {
    // Renommer le champ dans JSON
    @JsonProperty("user_name")
    private String username;
    
    // Format de date personnalisé
    @JsonFormat(pattern = "yyyy-MM-dd HH:mm:ss")
    private LocalDateTime createdAt;
    
    // Inclure seulement si non-null
    @JsonInclude(JsonInclude.Include.NON_NULL)
    private String middleName;
    
    // Inclure seulement si non-empty
    @JsonInclude(JsonInclude.Include.NON_EMPTY)
    private List<String> tags;
}
```

**Q24 : Quand utiliser Static Filtering ?**
**R :** Utilisez Static Filtering pour :
- ✅ Champs **toujours** sensibles (password, SSN, internal IDs)
- ✅ Cohérence garantie sur toute l'API
- ✅ Sécurité par défaut
- ✅ Cas simples où même filtrage partout

**Q25 : Quels sont les inconvénients du Static Filtering ?**
**R :**
- ❌ Pas flexible : impossible d'avoir différents niveaux de détails
- ❌ Si un endpoint a besoin du champ → problème
- ❌ Nécessite des DTOs pour flexibilité

## 🎯 Dynamic Filtering

**Q26 : Qu'est-ce que le Dynamic Filtering ?**
**R :** Dynamic Filtering permet de personnaliser les champs retournés pour chaque endpoint spécifique. Configuré dans le Controller avec `@JsonFilter` et `FilterProvider`. Flexibilité totale par endpoint.

**Q27 : Comment configurer Dynamic Filtering avec @JsonFilter ?**
**R :**
```java
// 1. Annoter l'Entity
@Entity
@JsonFilter("UserFilter")  // Nom du filtre
public class User {
    private Long id;
    private String username;
    private String email;
    private String password;
    private String phoneNumber;
}

// 2. Dans le Controller, appliquer le filtre
@GetMapping("/summary")
public ResponseEntity<MappingJacksonValue> getUsersSummary() {
    List<User> users = userService.findAll();
    
    // Créer le filtre
    SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
        .filterOutAllExcept("id", "username");
    
    FilterProvider filters = new SimpleFilterProvider()
        .addFilter("UserFilter", filter);
    
    // Appliquer
    MappingJacksonValue mapping = new MappingJacksonValue(users);
    mapping.setFilters(filters);
    
    return ResponseEntity.ok(mapping);
}
```

**Q28 : Qu'est-ce que SimpleBeanPropertyFilter ?**
**R :** `SimpleBeanPropertyFilter` est une classe Jackson qui définit quels champs inclure ou exclure :
```java
// Inclure seulement ces champs
SimpleBeanPropertyFilter.filterOutAllExcept("id", "username", "email");

// Exclure ces champs (inclure le reste)
SimpleBeanPropertyFilter.serializeAllExcept("password", "ssn");
```

**Q29 : Qu'est-ce que MappingJacksonValue ?**
**R :** `MappingJacksonValue` est un wrapper qui permet d'appliquer des filtres Jackson dynamiquement avant la sérialisation :
```java
MappingJacksonValue mapping = new MappingJacksonValue(data);
mapping.setFilters(filters);
return ResponseEntity.ok(mapping);
```
Nécessaire pour Dynamic Filtering car le type de retour doit être `MappingJacksonValue`.

**Q30 : Comment implémenter différents filtres pour différents endpoints ?**
**R :**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    // Endpoint 1: Summary (id, username seulement)
    @GetMapping("/summary")
    public ResponseEntity<MappingJacksonValue> getSummary() {
        List<User> users = userService.findAll();
        
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .filterOutAllExcept("id", "username");
        
        return applyFilter(users, filter);
    }
    
    // Endpoint 2: Details (tout sauf password)
    @GetMapping("/{id}")
    public ResponseEntity<MappingJacksonValue> getDetails(@PathVariable Long id) {
        User user = userService.findById(id);
        
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .serializeAllExcept("password");
        
        return applyFilter(user, filter);
    }
    
    // Endpoint 3: Contacts (username, email, phone seulement)
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

**Q31 : Comment simplifier le code de Dynamic Filtering avec une méthode helper ?**
**R :**
```java
@RestController
public class UserController {
    
    // Helper pour simplifier
    private MappingJacksonValue filterFields(Object data, String... fields) {
        SimpleBeanPropertyFilter filter = SimpleBeanPropertyFilter
            .filterOutAllExcept(fields);
        
        FilterProvider filters = new SimpleFilterProvider()
            .addFilter("UserFilter", filter);
        
        MappingJacksonValue mapping = new MappingJacksonValue(data);
        mapping.setFilters(filters);
        
        return mapping;
    }
    
    // Utilisation simplifiée
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

**Q32 : Comment permettre au client de choisir les champs avec @RequestParam ?**
**R :**
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
    
    // Si pas de fields, retourner tout
    return ResponseEntity.ok(new MappingJacksonValue(users));
}

// Utilisation:
// GET /api/users?fields=id,username
// GET /api/users?fields=username,email,phoneNumber
```

**Q33 : Quelle est la différence entre filterOutAllExcept et serializeAllExcept ?**
**R :**
```java
// filterOutAllExcept: Inclure SEULEMENT ces champs
SimpleBeanPropertyFilter.filterOutAllExcept("id", "username");
// JSON: {"id": 1, "username": "john"}

// serializeAllExcept: Exclure ces champs (inclure le reste)
SimpleBeanPropertyFilter.serializeAllExcept("password", "ssn");
// JSON: {"id": 1, "username": "john", "email": "...", ...}
//       (tout sauf password et ssn)
```

**Q34 : Peut-on avoir plusieurs @JsonFilter sur la même entité ?**
**R :** Non, une entité ne peut avoir qu'un seul `@JsonFilter`. Si besoin de filtres différents, utiliser :
1. DTOs différents (recommandé)
2. Le même nom de filtre avec différentes configurations par endpoint

**Q35 : Comment gérer les relations avec Dynamic Filtering ?**
**R :**
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

// Controller - filtrer User ET Orders
@GetMapping("/{id}")
public ResponseEntity<MappingJacksonValue> getUser(@PathVariable Long id) {
    User user = userService.findById(id);
    
    // Filtre pour User
    SimpleBeanPropertyFilter userFilter = SimpleBeanPropertyFilter
        .filterOutAllExcept("id", "username", "orders");
    
    // Filtre pour Order
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

**Q36 : Quand utiliser Dynamic Filtering ?**
**R :** Utilisez Dynamic Filtering pour :
- ✅ Différents niveaux de détails par endpoint
- ✅ API flexible où le client peut choisir les champs
- ✅ Endpoints admin vs user (différents champs)
- ✅ Optimisation performance (charger seulement nécessaire)

**Q37 : Quels sont les inconvénients du Dynamic Filtering ?**
**R :**
- ❌ Plus de code dans les Controllers
- ❌ Moins performant que Static (traitement runtime)
- ❌ Plus difficile à maintenir
- ❌ Type de retour `MappingJacksonValue` (moins intuitif)
- ❌ Complexité avec les relations

## 🔄 Static vs Dynamic - Comparaison

**Q38 : Tableau comparatif complet Static vs Dynamic Filtering**
**R :**
| Critère | Static Filtering | Dynamic Filtering |
|---------|------------------|-------------------|
| **Configuration** | Annotations (@JsonIgnore) | Code (FilterProvider) |
| **Emplacement** | Sur l'Entity | Dans le Controller |
| **Flexibilité** | ❌ Fixe | ✅ Personnalisable |
| **Performance** | ✅ Meilleure | Légèrement moins |
| **Complexité** | ✅ Simple | Plus complexe |
| **Type retour** | Entity/DTO | MappingJacksonValue |
| **Maintenance** | ✅ Facile | Plus de code |
| **Use Case** | Sécurité (password) | Niveaux de détails |

**Q39 : Quelle approche choisir : Static, Dynamic, ou DTOs ?**
**R :**
**Recommandation hiérarchique :**
1. **DTOs** (préféré) : Meilleure pratique, séparation claire
2. **Static Filtering** : Pour champs toujours cachés (sécurité)
3. **Dynamic Filtering** : Seulement si vraiment nécessaire

**Q40 : Peut-on combiner Static et Dynamic Filtering ?**
**R :** Oui, stratégie hybride (best practice) :
```java
@Entity
@JsonIgnoreProperties({"password", "ssn"})  // Static: toujours caché
@JsonFilter("UserFilter")                   // Dynamic: optionnel
public class User {
    private Long id;
    private String username;
    
    @JsonIgnore
    private String password;  // Double sécurité
    
    private String email;
    private String phoneNumber;
}

// Controller
@GetMapping("/summary")
public ResponseEntity<MappingJacksonValue> getSummary() {
    // Dynamic filtering pour username seulement
    // password déjà filtré par Static
    return filterFields(users, "id", "username");
}

@GetMapping("/{id}")
public ResponseEntity<MappingJacksonValue> getDetails(@PathVariable Long id) {
    // Plus de détails, mais password toujours caché
    return filterFields(user, "id", "username", "email", "phoneNumber");
}
```

## 🎯 Bonnes Pratiques

**Q41 : Quelle est la meilleure stratégie globale pour gérer les données exposées ?**
**R :**
```java
// 1. Static Filtering pour sécurité
@Entity
@JsonIgnoreProperties({"password", "ssn"})
public class User {
    @JsonIgnore
    private String password;  // Toujours caché
}

// 2. DTOs pour différents niveaux (préféré)
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

// 3. Dynamic Filtering seulement si nécessaire
@GetMapping("/{id}")
public ResponseEntity<?> getUser(
        @PathVariable Long id,
        @RequestParam(required = false) String fields) {
    
    User user = userService.findById(id);
    
    // Si client spécifie fields → Dynamic
    if (fields != null) {
        return ResponseEntity.ok(filterFields(user, fields.split(",")));
    }
    
    // Sinon → DTO
    return ResponseEntity.ok(new UserDetailDTO(user));
}
```

**Q42 : Règles de sécurité avec filtering**
**R :**
- ✅ **Toujours** Static Filtering pour champs sensibles (password, SSN)
- ✅ **Double sécurité** : DTOs + Static Filtering
- ✅ **Whitelist** (inclure explicitement) plutôt que blacklist
- ❌ **Ne jamais** exposer Entities directement
- ✅ **Tester** toutes les réponses JSON pour fuites de données

**Q43 : Comment tester le filtering ?**
**R :**
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
            .andExpect(jsonPath("$[0].password").doesNotExist())  // Critique
            .andExpect(jsonPath("$[0].ssn").doesNotExist());
    }
    
    @Test
    public void getUserDetails_shouldExposeEmailButNotPassword() throws Exception {
        mockMvc.perform(get("/api/users/1"))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.email").exists())
            .andExpect(jsonPath("$.password").doesNotExist());  // Toujours vérifier
    }
}
```

**Q44 : Performance : quel impact du filtering ?**
**R :**
- **Static Filtering** : Aucun impact (compile-time)
- **Dynamic Filtering** : Impact minimal (quelques ms par requête)
- **DTOs** : Impact léger (mapping Entity → DTO)
- **Gros impact** : Charger des données inutiles (N+1, eager loading)

**Optimisation :** Utiliser projections JPA pour ne charger que les champs nécessaires.

**Q45 : Comment documenter une API avec filtering dans Swagger/OpenAPI ?**
**R :**
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

Voilà 45 questions/réponses complètes couvrant REST API Design, DTOs et Filtering (Static & Dynamic) ! Ces questions vont des concepts de base aux implémentations avancées et bonnes pratiques de production.