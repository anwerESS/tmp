## [**..**](./00_index.md)

# Spring JDBC, JPA, Hibernate & Auditing

## 🗄️ Spring JDBC - Accès Base de Données Basique

### Concept de Spring JDBC

**Spring JDBC** simplifie l'accès aux bases de données en éliminant le code boilerplate JDBC traditionnel (gestion des connexions, ResultSet, exceptions).

### JDBC Traditionnel (Problématique)

```java
// JDBC pur - beaucoup de code boilerplate
Connection conn = null;
PreparedStatement ps = null;
ResultSet rs = null;
try {
    conn = DriverManager.getConnection(url, user, password);
    ps = conn.prepareStatement("SELECT * FROM course WHERE id = ?");
    ps.setLong(1, id);
    rs = ps.executeQuery();
    if (rs.next()) {
        Course course = new Course();
        course.setId(rs.getLong("id"));
        course.setName(rs.getString("name"));
        return course;
    }
} catch (SQLException e) {
    // Gestion d'erreur
} finally {
    // Fermer rs, ps, conn (oubli = fuite mémoire)
    if (rs != null) rs.close();
    if (ps != null) ps.close();
    if (conn != null) conn.close();
}
```

### Spring JDBC avec JdbcTemplate

```java
@Repository
public class CourseJdbcRepository {
    
    @Autowired
    private JdbcTemplate jdbcTemplate;
    
    // Query simple
    public Course findById(Long id) {
        String sql = "SELECT * FROM course WHERE id = ?";
        return jdbcTemplate.queryForObject(sql, 
            new BeanPropertyRowMapper<>(Course.class), 
            id);
    }
    
    // Query liste
    public List<Course> findAll() {
        String sql = "SELECT * FROM course";
        return jdbcTemplate.query(sql, 
            new BeanPropertyRowMapper<>(Course.class));
    }
    
    // Insert
    public int insert(Course course) {
        String sql = "INSERT INTO course (id, name, author) VALUES (?, ?, ?)";
        return jdbcTemplate.update(sql, 
            course.getId(), 
            course.getName(), 
            course.getAuthor());
    }
    
    // Update
    public int update(Course course) {
        String sql = "UPDATE course SET name = ?, author = ? WHERE id = ?";
        return jdbcTemplate.update(sql, 
            course.getName(), 
            course.getAuthor(), 
            course.getId());
    }
    
    // Delete
    public int deleteById(Long id) {
        String sql = "DELETE FROM course WHERE id = ?";
        return jdbcTemplate.update(sql, id);
    }
    
    // RowMapper personnalisé
    public List<Course> findAllCustom() {
        String sql = "SELECT * FROM course";
        return jdbcTemplate.query(sql, (rs, rowNum) -> {
            Course course = new Course();
            course.setId(rs.getLong("id"));
            course.setName(rs.getString("name"));
            course.setAuthor(rs.getString("author"));
            return course;
        });
    }
}
```

**Avantages Spring JDBC :**
- Gestion automatique des connexions
- Gestion automatique des exceptions (traduites en DataAccessException)
- Moins de code boilerplate
- Pas de fermeture manuelle des ressources

**Inconvénients :**
- Requêtes SQL manuelles (verbeux)
- Mapping manuel entre ResultSet et objets
- Pas de gestion automatique des relations

## 🏛️ JPA (Jakarta Persistence API) - ORM Standard

### Concept de JPA

**JPA** est une spécification Java pour l'ORM (Object-Relational Mapping). Elle définit comment mapper des objets Java vers des tables de base de données et vice-versa.

**JPA est une spécification, pas une implémentation !**

### Entité JPA Basique

```java
@Entity
@Table(name = "course")
public class Course {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    @Column(name = "name", nullable = false, length = 100)
    private String name;
    
    @Column(name = "author")
    private String author;
    
    // Constructeurs, getters, setters
}
```

### EntityManager - API JPA de Base

```java
@Repository
@Transactional
public class CourseJpaRepository {
    
    @PersistenceContext
    private EntityManager entityManager;
    
    // Persist - INSERT
    public Course save(Course course) {
        if (course.getId() == null) {
            entityManager.persist(course);  // INSERT
        } else {
            course = entityManager.merge(course);  // UPDATE
        }
        return course;
    }
    
    // Find - SELECT
    public Course findById(Long id) {
        return entityManager.find(Course.class, id);
    }
    
    // Query - SELECT avec JPQL
    public List<Course> findAll() {
        TypedQuery<Course> query = entityManager.createQuery(
            "SELECT c FROM Course c", Course.class);
        return query.getResultList();
    }
    
    // Query avec paramètres
    public List<Course> findByAuthor(String author) {
        TypedQuery<Course> query = entityManager.createQuery(
            "SELECT c FROM Course c WHERE c.author = :author", Course.class);
        query.setParameter("author", author);
        return query.getResultList();
    }
    
    // Delete
    public void deleteById(Long id) {
        Course course = entityManager.find(Course.class, id);
        if (course != null) {
            entityManager.remove(course);
        }
    }
    
    // Native SQL Query
    public List<Course> findByNativeQuery(String name) {
        Query query = entityManager.createNativeQuery(
            "SELECT * FROM course WHERE name LIKE ?", Course.class);
        query.setParameter(1, "%" + name + "%");
        return query.getResultList();
    }
}
```

### JPQL (Java Persistence Query Language)

```java
// JPQL - utilise les noms d'entité et de propriétés (pas les tables/colonnes SQL)
"SELECT c FROM Course c WHERE c.author = :author"

// Équivalent SQL
"SELECT * FROM course WHERE author = ?"

// JPQL avec jointures
"SELECT c FROM Course c JOIN c.students s WHERE s.name = :name"

// JPQL avec agrégations
"SELECT c.author, COUNT(c) FROM Course c GROUP BY c.author"
```

### La Magie de JPA

**1. Mapping Objet-Relationnel Automatique**
```java
Course course = new Course("Spring Boot", "John");
entityManager.persist(course);
// JPA génère automatiquement: INSERT INTO course (name, author) VALUES (?, ?)
```

**2. Gestion Automatique des Relations**
```java
@Entity
public class Course {
    @Id
    @GeneratedValue
    private Long id;
    
    private String name;
    
    @ManyToOne
    @JoinColumn(name = "author_id")
    private Author author;
    
    @OneToMany(mappedBy = "course", cascade = CascadeType.ALL)
    private List<Review> reviews = new ArrayList<>();
}

// JPA gère automatiquement les foreign keys et les jointures
```

**3. Lazy Loading**
```java
@Entity
public class Course {
    @OneToMany(fetch = FetchType.LAZY)
    private List<Review> reviews;  // Chargées seulement quand accédées
}
```

**4. Caching (First-Level Cache)**
```java
Course c1 = entityManager.find(Course.class, 1L);  // SELECT depuis DB
Course c2 = entityManager.find(Course.class, 1L);  // Depuis le cache, pas de SELECT
```

**5. Dirty Checking**
```java
@Transactional
public void updateCourse(Long id) {
    Course course = entityManager.find(Course.class, id);
    course.setName("Updated Name");
    // Pas besoin d'appeler save() !
    // JPA détecte le changement et fait UPDATE automatiquement
}
```

## 🚀 Spring Data JPA - Simplification Ultime

### Concept de Spring Data JPA

Spring Data JPA ajoute une couche d'abstraction sur JPA, éliminant même le besoin d'écrire des requêtes pour les opérations CRUD basiques.

### Repository Interface

```java
public interface CourseRepository extends JpaRepository<Course, Long> {
    // Pas besoin d'implémentation ! Spring génère le code automatiquement
}
```

**Méthodes héritées automatiquement :**
```java
// CRUD de base
save(entity)
saveAll(entities)
findById(id)
findAll()
findAllById(ids)
count()
existsById(id)
deleteById(id)
delete(entity)
deleteAll()

// Pagination et tri
findAll(Pageable pageable)
findAll(Sort sort)
```

### Query Methods - Génération Automatique

```java
public interface CourseRepository extends JpaRepository<Course, Long> {
    
    // Spring génère automatiquement la requête depuis le nom de la méthode !
    
    List<Course> findByName(String name);
    // SELECT * FROM course WHERE name = ?
    
    List<Course> findByAuthor(String author);
    // SELECT * FROM course WHERE author = ?
    
    List<Course> findByNameAndAuthor(String name, String author);
    // SELECT * FROM course WHERE name = ? AND author = ?
    
    List<Course> findByNameOrAuthor(String name, String author);
    // SELECT * FROM course WHERE name = ? OR author = ?
    
    List<Course> findByNameContaining(String keyword);
    // SELECT * FROM course WHERE name LIKE %keyword%
    
    List<Course> findByNameStartingWith(String prefix);
    // SELECT * FROM course WHERE name LIKE prefix%
    
    List<Course> findByIdGreaterThan(Long id);
    // SELECT * FROM course WHERE id > ?
    
    List<Course> findByNameOrderByAuthorAsc(String name);
    // SELECT * FROM course WHERE name = ? ORDER BY author ASC
    
    Long countByAuthor(String author);
    // SELECT COUNT(*) FROM course WHERE author = ?
    
    boolean existsByName(String name);
    // SELECT COUNT(*) FROM course WHERE name = ? (retourne boolean)
    
    void deleteByAuthor(String author);
    // DELETE FROM course WHERE author = ?
}
```

### @Query - Requêtes Personnalisées

```java
public interface CourseRepository extends JpaRepository<Course, Long> {
    
    // JPQL personnalisée
    @Query("SELECT c FROM Course c WHERE c.author = :author")
    List<Course> findCoursesByAuthor(@Param("author") String author);
    
    // JPQL avec plusieurs paramètres
    @Query("SELECT c FROM Course c WHERE c.name LIKE %:keyword% AND c.author = :author")
    List<Course> searchCourses(@Param("keyword") String keyword, 
                               @Param("author") String author);
    
    // Native SQL Query
    @Query(value = "SELECT * FROM course WHERE author = ?1", nativeQuery = true)
    List<Course> findByAuthorNative(String author);
    
    // Update Query
    @Modifying
    @Query("UPDATE Course c SET c.name = :name WHERE c.id = :id")
    int updateCourseName(@Param("id") Long id, @Param("name") String name);
    
    // Delete Query
    @Modifying
    @Query("DELETE FROM Course c WHERE c.author = :author")
    int deleteByAuthorCustom(@Param("author") String author);
    
    // Projection (seulement certains champs)
    @Query("SELECT c.name FROM Course c WHERE c.author = :author")
    List<String> findCourseNames(@Param("author") String author);
    
    // DTO Projection
    @Query("SELECT new com.example.dto.CourseDTO(c.id, c.name) FROM Course c")
    List<CourseDTO> findAllDTOs();
}
```

### Pagination et Tri avec Spring Data JPA

```java
public interface CourseRepository extends JpaRepository<Course, Long> {
    
    // Méthodes de pagination générées automatiquement
    Page<Course> findByAuthor(String author, Pageable pageable);
    
    List<Course> findByAuthor(String author, Sort sort);
}

// Utilisation dans le Service
@Service
public class CourseService {
    
    @Autowired
    private CourseRepository repository;
    
    public Page<Course> getCourses(int page, int size) {
        Pageable pageable = PageRequest.of(page, size, Sort.by("name").ascending());
        return repository.findAll(pageable);
    }
    
    public Page<Course> getCoursesByAuthor(String author, int page, int size) {
        Pageable pageable = PageRequest.of(page, size);
        return repository.findByAuthor(author, pageable);
    }
    
    public List<Course> getAllSorted() {
        Sort sort = Sort.by("author").ascending()
                        .and(Sort.by("name").descending());
        return repository.findAll(sort);
    }
}
```

### Specifications (Requêtes Dynamiques)

```java
public interface CourseRepository extends JpaRepository<Course, Long>, 
                                         JpaSpecificationExecutor<Course> {
}

// Spécifications
public class CourseSpecifications {
    
    public static Specification<Course> hasAuthor(String author) {
        return (root, query, cb) -> 
            author == null ? null : cb.equal(root.get("author"), author);
    }
    
    public static Specification<Course> nameContains(String keyword) {
        return (root, query, cb) -> 
            keyword == null ? null : 
            cb.like(cb.lower(root.get("name")), "%" + keyword.toLowerCase() + "%");
    }
}

// Utilisation
Specification<Course> spec = Specification
    .where(CourseSpecifications.hasAuthor(author))
    .and(CourseSpecifications.nameContains(keyword));

List<Course> courses = repository.findAll(spec);
```

## 🐘 Hibernate - Implémentation JPA

### JPA vs Hibernate - Différence Fondamentale

```
JPA (Jakarta Persistence API)
    ↓ Spécification (interface, annotations)
    
Hibernate
    ↓ Implémentation de JPA
    
Autres implémentations JPA:
    - EclipseLink
    - OpenJPA
```

**JPA** : Spécification (ensemble d'interfaces et d'annotations)
- `@Entity`, `@Id`, `@Column`, `EntityManager`, etc.

**Hibernate** : Implémentation concrète de JPA
- Fournit le code qui fait fonctionner JPA
- Ajoute des fonctionnalités supplémentaires

### Tableau Comparatif

| Aspect | JPA | Hibernate |
|--------|-----|-----------|
| **Nature** | Spécification | Implémentation |
| **Annotations** | `@Entity`, `@Id`, `@Column` | Supporte JPA + annotations spécifiques Hibernate |
| **Package** | `jakarta.persistence.*` | `org.hibernate.*` |
| **Portabilité** | ✅ Portable entre implémentations | ❌ Spécifique Hibernate |
| **Fonctionnalités** | Standard | JPA + fonctionnalités supplémentaires |

### Exemple de Code

```java
// JPA Standard (portable)
import jakarta.persistence.*;

@Entity
@Table(name = "course")
public class Course {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    @Column(name = "name")
    private String name;
}

// Hibernate Spécifique (non portable)
import org.hibernate.annotations.*;

@Entity
@DynamicUpdate  // Annotation Hibernate uniquement
@SelectBeforeUpdate  // Annotation Hibernate uniquement
public class Course {
    @Id
    @GeneratedValue(generator = "custom-generator")
    @GenericGenerator(name = "custom-generator", strategy = "uuid2")  // Hibernate
    private String id;
    
    @Formula("(SELECT COUNT(*) FROM review r WHERE r.course_id = id)")  // Hibernate
    private int reviewCount;
}
```

### Fonctionnalités Supplémentaires d'Hibernate

**1. Caching de Second Niveau**
```java
@Entity
@Cacheable
@org.hibernate.annotations.Cache(usage = CacheConcurrencyStrategy.READ_WRITE)
public class Course {
    // Cache partagé entre sessions
}
```

**2. Batch Processing**
```properties
spring.jpa.properties.hibernate.jdbc.batch_size=20
spring.jpa.properties.hibernate.order_inserts=true
spring.jpa.properties.hibernate.order_updates=true
```

**3. Annotations Hibernate Spécifiques**
```java
@Entity
public class Course {
    @Id
    @GeneratedValue
    private Long id;
    
    @org.hibernate.annotations.CreationTimestamp
    private LocalDateTime createdAt;
    
    @org.hibernate.annotations.UpdateTimestamp
    private LocalDateTime updatedAt;
    
    @org.hibernate.annotations.Formula("(SELECT COUNT(*) FROM review WHERE course_id = id)")
    private int reviewCount;
}
```

### Configuration Hibernate dans Spring Boot

```properties
# application.properties

# Afficher les requêtes SQL
spring.jpa.show-sql=true
spring.jpa.properties.hibernate.format_sql=true

# Niveau de log
logging.level.org.hibernate.SQL=DEBUG
logging.level.org.hibernate.type.descriptor.sql.BasicBinder=TRACE

# DDL (création de tables)
spring.jpa.hibernate.ddl-auto=update
# - none: Rien
# - validate: Valide le schéma
# - update: Met à jour le schéma
# - create: Drop et recrée
# - create-drop: Drop à la fin

# Dialect
spring.jpa.properties.hibernate.dialect=org.hibernate.dialect.H2Dialect

# Performance
spring.jpa.properties.hibernate.jdbc.batch_size=20
spring.jpa.properties.hibernate.jdbc.fetch_size=50
```

## 📊 Auditing - Traçabilité Automatique

### Concept d'Auditing

L'auditing permet de tracer automatiquement qui a créé/modifié une entité et quand, sans code manuel dans chaque service.

### Configuration de Base

```java
@Configuration
@EnableJpaAuditing
public class JpaConfig {
    
    @Bean
    public AuditorAware<String> auditorProvider() {
        return () -> {
            // Récupérer l'utilisateur actuel depuis Spring Security
            Authentication authentication = SecurityContextHolder
                .getContext()
                .getAuthentication();
            
            if (authentication == null || !authentication.isAuthenticated()) {
                return Optional.of("system");
            }
            
            return Optional.of(authentication.getName());
        };
    }
}
```

### Classe de Base Auditable

```java
@MappedSuperclass
@EntityListeners(AuditingEntityListener.class)
public abstract class BaseEntity {
    
    @CreatedDate
    @Column(name = "created_at", nullable = false, updatable = false)
    private LocalDateTime createdAt;
    
    @CreatedBy
    @Column(name = "created_by", nullable = false, updatable = false)
    private String createdBy;
    
    @LastModifiedDate
    @Column(name = "updated_at", insertable = false)
    private LocalDateTime updatedAt;
    
    @LastModifiedBy
    @Column(name = "updated_by", insertable = false)
    private String updatedBy;
    
    // Getters et setters
}
```

### Utilisation dans les Entités

```java
@Entity
@Table(name = "course")
public class Course extends BaseEntity {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    @Column(name = "name", nullable = false)
    private String name;
    
    @Column(name = "author")
    private String author;
    
    // createdAt, createdBy, updatedAt, updatedBy hérités de BaseEntity
    // Gérés automatiquement par JPA Auditing
}
```

### Explication des Annotations

#### @MappedSuperclass
- Indique que cette classe est une superclass pour d'autres entités
- Ses champs seront hérités par les entités enfants
- La classe elle-même n'est PAS une entité (pas de table)
- Les champs apparaîtront dans les tables des entités enfants

#### @EntityListeners(AuditingEntityListener.class)
- Active l'auditing pour l'entité
- `AuditingEntityListener` est fourni par Spring Data JPA
- Remplit automatiquement les champs annotés avec @CreatedDate, @CreatedBy, etc.

#### @CreatedDate
- Remplit automatiquement le champ lors de la création (INSERT)
- Type : `LocalDateTime`, `Instant`, `Date`, `Long` (timestamp)

#### @CreatedBy
- Remplit automatiquement le champ avec l'utilisateur créateur
- Nécessite `AuditorAware<T>` pour fournir l'utilisateur actuel
- Type : `String`, `Long`, ou type custom (User)

#### @LastModifiedDate
- Met à jour automatiquement le champ lors de modifications (UPDATE)
- Pas mis à jour lors de la création

#### @LastModifiedBy
- Met à jour automatiquement avec l'utilisateur modificateur
- Pas mis à jour lors de la création

#### @Column
```java
@Column(updatable = false)  // Ne peut pas être modifié après création (createdAt, createdBy)
@Column(insertable = false)  // Ne peut pas être inséré manuellement (updatedAt, updatedBy)
```

### Exemple Complet avec Auditing

```java
// Configuration
@Configuration
@EnableJpaAuditing(auditorAwareRef = "auditorProvider")
public class JpaConfig {
    
    @Bean
    public AuditorAware<String> auditorProvider() {
        return () -> Optional.ofNullable(SecurityContextHolder
            .getContext()
            .getAuthentication())
            .filter(Authentication::isAuthenticated)
            .map(Authentication::getName)
            .or(() -> Optional.of("system"));
    }
}

// BaseEntity
@MappedSuperclass
@EntityListeners(AuditingEntityListener.class)
@Getter
@Setter
public abstract class AuditableEntity {
    
    @CreatedDate
    @Column(name = "created_at", nullable = false, updatable = false)
    private LocalDateTime createdAt;
    
    @CreatedBy
    @Column(name = "created_by", length = 50, updatable = false)
    private String createdBy;
    
    @LastModifiedDate
    @Column(name = "updated_at", insertable = false)
    private LocalDateTime updatedAt;
    
    @LastModifiedBy
    @Column(name = "updated_by", length = 50, insertable = false)
    private String updatedBy;
    
    @Version  // Optimistic Locking
    private Long version;
}

// Entité utilisant l'auditing
@Entity
@Table(name = "product")
public class Product extends AuditableEntity {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    private String name;
    private Double price;
}

// Service
@Service
public class ProductService {
    
    @Autowired
    private ProductRepository repository;
    
    @Transactional
    public Product createProduct(String name, Double price) {
        Product product = new Product();
        product.setName(name);
        product.setPrice(price);
        
        // createdAt et createdBy remplis automatiquement
        return repository.save(product);
    }
    
    @Transactional
    public Product updateProduct(Long id, String name) {
        Product product = repository.findById(id)
            .orElseThrow(() -> new NotFoundException("Product not found"));
        
        product.setName(name);
        
        // updatedAt et updatedBy remplis automatiquement
        return repository.save(product);
    }
}
```

### Résultat en Base de Données

```sql
-- Après création
INSERT INTO product (name, price, created_at, created_by, version)
VALUES ('Laptop', 999.99, '2025-01-15 10:30:00', 'john.doe', 0);

-- Après modification
UPDATE product 
SET name = 'Gaming Laptop', 
    updated_at = '2025-01-16 14:20:00',
    updated_by = 'jane.smith',
    version = 1
WHERE id = 1 AND version = 0;  -- Optimistic locking
```

## 📁 schema.sql - Initialisation du Schéma

### Concept de schema.sql

Fichier SQL exécuté automatiquement par Spring Boot au démarrage pour initialiser le schéma de la base de données.

### Emplacement

```
src/main/resources/schema.sql
```

### Exemple de schema.sql

```sql
-- schema.sql

-- Drop tables if exist (optional)
DROP TABLE IF EXISTS review;
DROP TABLE IF EXISTS course;

-- Create course table
CREATE TABLE course (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    author VARCHAR(50) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- Create review table
CREATE TABLE review (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    rating INT NOT NULL CHECK (rating BETWEEN 1 AND 5),
    comment VARCHAR(500),
    course_id BIGINT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (course_id) REFERENCES course(id) ON DELETE CASCADE
);

-- Create indexes
CREATE INDEX idx_course_author ON course(author);
CREATE INDEX idx_review_course ON review(course_id);
```

### data.sql - Données Initiales

```sql
-- data.sql (données de test)

INSERT INTO course (name, author) VALUES
('Spring Boot Fundamentals', 'John Doe'),
('JPA and Hibernate', 'Jane Smith'),
('Microservices Architecture', 'Bob Johnson');

INSERT INTO review (rating, comment, course_id) VALUES
(5, 'Excellent course!', 1),
(4, 'Very informative', 1),
(5, 'Best course ever', 2);
```

### Configuration

```properties
# application.properties

# Activer l'exécution de schema.sql (désactivé par défaut si JPA/Hibernate gère le schéma)
spring.sql.init.mode=always

# Ordre d'exécution
spring.jpa.defer-datasource-initialization=true

# Si vous utilisez Hibernate pour créer le schéma, désactiver schema.sql
spring.jpa.hibernate.ddl-auto=none
```

### Quand Utiliser schema.sql vs Hibernate DDL

**Utiliser schema.sql quand :**
- Schéma complexe avec contraintes spécifiques
- Migration de base de données existante
- Besoin de contrôle total sur le SQL
- Production (pas de Hibernate DDL)

**Utiliser Hibernate DDL (`ddl-auto`) quand :**
- Prototypage rapide
- Développement local
- Tests (avec H2 en mémoire)

**Best Practice Production :**
Utiliser des outils de migration comme **Flyway** ou **Liquibase** au lieu de schema.sql

## 🗃️ H2 Console - Base de Données en Mémoire

### Configuration H2

```properties
# application.properties

# Activer H2 Console
spring.h2.console.enabled=true
spring.h2.console.path=/h2-console

# Configuration H2
spring.datasource.url=jdbc:h2:mem:testdb
spring.datasource.driverClassName=org.h2.Driver
spring.datasource.username=sa
spring.datasource.password=

# JPA/Hibernate
spring.jpa.database-platform=org.hibernate.dialect.H2Dialect
spring.jpa.show-sql=true
```

### Accès à H2 Console

```
URL: http://localhost:8080/h2-console

JDBC URL: jdbc:h2:mem:testdb
Username: sa
Password: (vide)
```

### Utilité de H2

- Base de données en mémoire (rapide)
- Parfait pour développement et tests
- Console web intégrée
- Compatible SQL standard
- Pas de configuration externe nécessaire

---

# Questions & Réponses - Spring JDBC, JPA, Hibernate & Auditing

## 🗄️ Spring JDBC

**Q1 : Quel est le problème principal du JDBC traditionnel ?**
**R :** JDBC traditionnel nécessite énormément de code boilerplate : gestion manuelle des connexions, ResultSet, PreparedStatement, fermeture des ressources (risque de fuite mémoire), gestion d'exceptions verbeux. Beaucoup de code répétitif pour des opérations simples.

**Q2 : Comment Spring JDBC simplifie-t-il l'accès aux bases de données ?**
**R :** Spring JDBC fournit `JdbcTemplate` qui :
- Gère automatiquement les connexions et leur fermeture
- Traduit les SQLExceptions en DataAccessException (unchecked)
- Élimine le code boilerplate
- Fournit des méthodes utilitaires (`query`, `update`, `queryForObject`)

**Q3 : Qu'est-ce que JdbcTemplate ?**
**R :** `JdbcTemplate` est la classe centrale de Spring JDBC qui simplifie l'exécution de requêtes SQL. Elle gère automatiquement les ressources (connexions, statements) et les exceptions.

**Q4 : Comment faire une requête SELECT avec JdbcTemplate ?**
**R :**
```java
// Un seul objet
Course course = jdbcTemplate.queryForObject(
    "SELECT * FROM course WHERE id = ?",
    new BeanPropertyRowMapper<>(Course.class),
    id
);

// Liste d'objets
List<Course> courses = jdbcTemplate.query(
    "SELECT * FROM course",
    new BeanPropertyRowMapper<>(Course.class)
);
```

**Q5 : Comment faire un INSERT/UPDATE/DELETE avec JdbcTemplate ?**
**R :**
```java
// INSERT
int rows = jdbcTemplate.update(
    "INSERT INTO course (name, author) VALUES (?, ?)",
    "Spring Boot", "John Doe"
);

// UPDATE
int rows = jdbcTemplate.update(
    "UPDATE course SET name = ? WHERE id = ?",
    "New Name", 1L
);

// DELETE
int rows = jdbcTemplate.update("DELETE FROM course WHERE id = ?", 1L);
```

**Q6 : Qu'est-ce qu'un RowMapper ?**
**R :** Un `RowMapper` est une interface qui définit comment mapper une ligne de ResultSet vers un objet Java :
```java
jdbcTemplate.query("SELECT * FROM course", (rs, rowNum) -> {
    Course course = new Course();
    course.setId(rs.getLong("id"));
    course.setName(rs.getString("name"));
    return course;
});
```

**Q7 : Quand utiliser Spring JDBC plutôt que JPA ?**
**R :** Utiliser Spring JDBC pour :
- Requêtes SQL complexes optimisées
- Contrôle total sur le SQL
- Applications legacy avec SQL existant
- Performance critique (pas d'overhead ORM)
- Bases de données spécifiques (procédures stockées)

## 🏛️ JPA (Jakarta Persistence API)

**Q8 : Qu'est-ce que JPA ?**
**R :** JPA (Jakarta Persistence API) est une **spécification** Java pour l'ORM (Object-Relational Mapping). Elle définit comment mapper des objets Java vers des tables de base de données. JPA n'est PAS une implémentation, c'est un standard.

**Q9 : Quelle est la différence entre JPA et Hibernate ?**
**R :**
- **JPA** : Spécification (interfaces, annotations) - package `jakarta.persistence.*`
- **Hibernate** : Implémentation concrète de JPA - fournit le code qui fait fonctionner JPA

JPA = Interface, Hibernate = Implémentation (comme JDBC = Interface, MySQL Driver = Implémentation)

**Q10 : Quelles sont les principales annotations JPA ?**
**R :**
- `@Entity` : Marque une classe comme entité JPA (table)
- `@Table(name="...")` : Spécifie le nom de la table
- `@Id` : Clé primaire
- `@GeneratedValue` : Génération automatique de l'ID
- `@Column` : Configuration de la colonne
- `@Transient` : Champ non persisté en base

**Q11 : Qu'est-ce que l'EntityManager ?**
**R :** `EntityManager` est l'API JPA principale pour interagir avec la base de données. Il gère le cycle de vie des entités (persist, merge, find, remove) et le context de persistance.

**Q12 : Quelle est la différence entre persist() et merge() ?**
**R :**
- **persist()** : Pour entités nouvelles (INSERT). Exception si l'entité existe déjà
- **merge()** : Pour entités détachées (UPDATE). Peut aussi INSERT si l'entité n'existe pas

```java
// Nouvelle entité
Course course = new Course("Spring Boot");
entityManager.persist(course);  // INSERT

// Mise à jour
course.setName("Updated");
entityManager.merge(course);  // UPDATE
```

**Q13 : Qu'est-ce que JPQL ?**
**R :** JPQL (Java Persistence Query Language) est un langage de requête orienté objet pour JPA. Il utilise les noms d'entités et de propriétés au lieu des tables et colonnes SQL :
```java
// JPQL (orienté objet)
"SELECT c FROM Course c WHERE c.author = :author"

// SQL équivalent
"SELECT * FROM course WHERE author = ?"
```

**Q14 : Qu'est-ce que le First-Level Cache de JPA ?**
**R :** Le First-Level Cache est un cache au niveau de l'EntityManager (session). Si on récupère la même entité deux fois dans la même transaction, JPA retourne la même instance depuis le cache sans requête SQL :
```java
Course c1 = entityManager.find(Course.class, 1L);  // SELECT
Course c2 = entityManager.find(Course.class, 1L);  // Pas de SELECT, depuis cache
assertSame(c1, c2);  // Même instance !
```

**Q15 : Qu'est-ce que le Dirty Checking ?**
**R :** Dirty Checking est la capacité de JPA à détecter automatiquement les modifications d'une entité et faire un UPDATE sans appel explicite à `save()` ou `merge()` :
```java
@Transactional
public void updateCourse(Long id) {
    Course course = entityManager.find(Course.class, id);
    course.setName("Updated");
    // Pas besoin de save() !
    // JPA détecte le changement et fait UPDATE automatiquement
}
```

**Q16 : Qu'est-ce que le Lazy Loading ?**
**R :** Lazy Loading signifie que les relations ne sont chargées que lorsqu'on y accède, pas lors du chargement initial :
```java
@Entity
public class Course {
    @OneToMany(fetch = FetchType.LAZY)  // Par défaut pour @OneToMany
    private List<Review> reviews;  // Pas chargé immédiatement
}

Course course = entityManager.find(Course.class, 1L);  // SELECT course
List<Review> reviews = course.getReviews();  // SELECT reviews (maintenant)
```

**Q17 : Quelle est la différence entre FetchType.LAZY et FetchType.EAGER ?**
**R :**
- **LAZY** : Données chargées à la demande (quand accédées). Économise les requêtes
- **EAGER** : Données chargées immédiatement avec l'entité parente. Peut causer des problèmes de performance (N+1)

**Q18 : Quels sont les types de relations JPA ?**
**R :**
- `@OneToOne` : Un-à-un (User ↔ Profile)
- `@OneToMany` : Un-à-plusieurs (Course → Reviews)
- `@ManyToOne` : Plusieurs-à-un (Reviews → Course)
- `@ManyToMany` : Plusieurs-à-plusieurs (Students ↔ Courses)

**Q19 : Comment fonctionne @GeneratedValue ?**
**R :**
```java
@Id
@GeneratedValue(strategy = GenerationType.IDENTITY)  // Auto-increment DB
private Long id;

// Autres stratégies:
// - AUTO (par défaut, choisi automatiquement)
// - IDENTITY (auto-increment)
// - SEQUENCE (séquence DB)
// - TABLE (table dédiée pour IDs)
// - UUID (avec Hibernate)
```

**Q20 : Qu'est-ce que CascadeType ?**
**R :** `CascadeType` propage les opérations de l'entité parente vers les enfants :
```java
@OneToMany(cascade = CascadeType.ALL)
private List<Review> reviews;

// Si on delete le Course, les Reviews sont aussi deleted
// Si on persist le Course, les Reviews sont aussi persistées

// Types:
// - ALL (toutes les opérations)
// - PERSIST, MERGE, REMOVE, REFRESH, DETACH
```

## 🚀 Spring Data JPA

**Q21 : Qu'est-ce que Spring Data JPA ?**
**R :** Spring Data JPA est une couche d'abstraction au-dessus de JPA qui élimine le besoin d'écrire des implémentations de repository. Il génère automatiquement le code des requêtes basiques et complexes.

**Q22 : Comment créer un repository Spring Data JPA ?**
**R :**
```java
public interface CourseRepository extends JpaRepository<Course, Long> {
    // Pas d'implémentation nécessaire !
    // Spring génère le code automatiquement
}
```

**Q23 : Quelles méthodes sont disponibles automatiquement avec JpaRepository ?**
**R :**
```java
// CRUD
save(entity)
saveAll(entities)
findById(id)
findAll()
count()
existsById(id)
deleteById(id)
delete(entity)
deleteAll()

// Pagination et tri
findAll(Pageable)
findAll(Sort)
```

**Q24 : Comment fonctionnent les Query Methods (méthodes dérivées) ?**
**R :** Spring Data JPA génère automatiquement les requêtes depuis le nom de la méthode :
```java
// Spring génère automatiquement le SQL !
List<Course> findByName(String name);
// → SELECT * FROM course WHERE name = ?

List<Course> findByNameAndAuthor(String name, String author);
// → SELECT * FROM course WHERE name = ? AND author = ?

List<Course> findByNameContaining(String keyword);
// → SELECT * FROM course WHERE name LIKE %keyword%

List<Course> findByIdGreaterThan(Long id);
// → SELECT * FROM course WHERE id > ?

Long countByAuthor(String author);
// → SELECT COUNT(*) FROM course WHERE author = ?
```

**Q25 : Quels sont les mots-clés supportés dans les Query Methods ?**
**R :**
- **And / Or** : `findByNameAndAuthor`, `findByNameOrAuthor`
- **Containing / Like** : `findByNameContaining` (LIKE %x%)
- **StartingWith / EndingWith** : `findByNameStartingWith` (LIKE x%)
- **GreaterThan / LessThan** : `findByIdGreaterThan`
- **Between** : `findByPriceBetween`
- **In** : `findByAuthorIn(List<String>)`
- **IsNull / IsNotNull** : `findByAuthorIsNull`
- **OrderBy** : `findByAuthorOrderByNameAsc`

**Q26 : Comment écrire une requête personnalisée avec @Query ?**
**R :**
```java
// JPQL
@Query("SELECT c FROM Course c WHERE c.author = :author")
List<Course> findCoursesByAuthor(@Param("author") String author);

// Native SQL
@Query(value = "SELECT * FROM course WHERE author = ?1", nativeQuery = true)
List<Course> findByAuthorNative(String author);

// Update Query
@Modifying
@Query("UPDATE Course c SET c.name = :name WHERE c.id = :id")
int updateCourseName(@Param("id") Long id, @Param("name") String name);
```

**Q27 : Quelle est la différence entre JPQL et Native SQL dans @Query ?**
**R :**
- **JPQL** : Utilise les noms d'entités et de propriétés (orienté objet). Portable entre DB
- **Native SQL** : SQL pur, utilise les noms de tables et colonnes. Spécifique à la DB

**Q28 : Comment implémenter la pagination avec Spring Data JPA ?**
**R :**
```java
// Repository
Page<Course> findByAuthor(String author, Pageable pageable);

// Service
public Page<Course> getCourses(int page, int size) {
    Pageable pageable = PageRequest.of(page, size, 
                                       Sort.by("name").ascending());
    return repository.findAll(pageable);
}

// Page contient:
// - getContent() : liste des éléments
// - getTotalElements() : total d'éléments
// - getTotalPages() : nombre de pages
// - getNumber() : numéro page actuelle
```

**Q29 : Comment trier les résultats avec Spring Data JPA ?**
**R :**
```java
// Tri simple
Sort sort = Sort.by("name").ascending();
List<Course> courses = repository.findAll(sort);

// Tri multiple
Sort sort = Sort.by("author").ascending()
                .and(Sort.by("name").descending());

// Avec pagination
Pageable pageable = PageRequest.of(0, 10, sort);
Page<Course> page = repository.findAll(pageable);
```

**Q30 : Qu'est-ce que JpaSpecificationExecutor et quand l'utiliser ?**
**R :** `JpaSpecificationExecutor` permet de construire des requêtes dynamiques avec des critères conditionnels :
```java
public interface CourseRepository extends JpaRepository<Course, Long>, 
                                         JpaSpecificationExecutor<Course> {
}

// Spécification dynamique
Specification<Course> spec = (root, query, cb) -> {
    List<Predicate> predicates = new ArrayList<>();
    
    if (author != null) {
        predicates.add(cb.equal(root.get("author"), author));
    }
    if (keyword != null) {
        predicates.add(cb.like(root.get("name"), "%" + keyword + "%"));
    }
    
    return cb.and(predicates.toArray(new Predicate[0]));
};

List<Course> courses = repository.findAll(spec);
```
Utile pour des filtres dynamiques complexes.

**Q31 : Comment faire un query avec plusieurs tables (jointures) ?**
**R :**
```java
@Query("SELECT c FROM Course c JOIN c.reviews r WHERE r.rating >= :minRating")
List<Course> findCoursesWithHighRatings(@Param("minRating") int minRating);

// Ou avec fetch join (évite N+1)
@Query("SELECT DISTINCT c FROM Course c LEFT JOIN FETCH c.reviews")
List<Course> findAllWithReviews();
```

**Q32 : Qu'est-ce que le problème N+1 et comment l'éviter ?**
**R :** Le problème N+1 se produit avec Lazy Loading :
```java
List<Course> courses = courseRepository.findAll();  // 1 query
for (Course course : courses) {
    course.getReviews().size();  // N queries (1 par course) !
}
// Total: 1 + N queries
```

**Solutions :**
```java
// Fetch Join
@Query("SELECT c FROM Course c LEFT JOIN FETCH c.reviews")
List<Course> findAllWithReviews();  // 1 seule query

// EntityGraph
@EntityGraph(attributePaths = {"reviews"})
List<Course> findAll();
```

**Q33 : Comment utiliser @EntityGraph ?**
**R :**
```java
@Entity
@NamedEntityGraph(
    name = "Course.reviews",
    attributeNodes = @NamedAttributeNode("reviews")
)
public class Course { ... }

// Repository
@EntityGraph(value = "Course.reviews")
List<Course> findAll();

// Ou inline
@EntityGraph(attributePaths = {"reviews", "author"})
Optional<Course> findById(Long id);
```

## 🐘 Hibernate

**Q34 : Hibernate est-il obligatoire pour utiliser JPA ?**
**R :** Non, JPA est une spécification qui peut être implémentée par plusieurs fournisseurs : Hibernate (le plus populaire), EclipseLink, OpenJPA. Spring Boot utilise Hibernate par défaut.

**Q35 : Quelles sont les fonctionnalités uniques d'Hibernate (non JPA) ?**
**R :**
- Second-Level Cache
- Batch Processing optimisé
- Annotations spécifiques (`@Formula`, `@CreationTimestamp`, `@UpdateTimestamp`)
- Custom ID generators
- Envers (audit historique)

**Q36 : Comment activer le Second-Level Cache d'Hibernate ?**
**R :**
```java
// Configuration
spring.jpa.properties.hibernate.cache.use_second_level_cache=true
spring.jpa.properties.hibernate.cache.region.factory_class=org.hibernate.cache.jcache.JCacheRegionFactory

// Entité
@Entity
@Cacheable
@org.hibernate.annotations.Cache(usage = CacheConcurrencyStrategy.READ_WRITE)
public class Course {
    // Cache partagé entre toutes les sessions
}
```

**Q37 : Que signifie spring.jpa.hibernate.ddl-auto ?**
**R :**
- **none** : Rien, schéma géré manuellement
- **validate** : Valide que le schéma correspond aux entités
- **update** : Met à jour le schéma (ajoute colonnes/tables, ne supprime pas)
- **create** : Drop et recrée le schéma à chaque démarrage
- **create-drop** : Crée au démarrage, drop à l'arrêt

**⚠️ Production : toujours `none` ou `validate` !**

**Q38 : Comment afficher les requêtes SQL générées par Hibernate ?**
**R :**
```properties
# Afficher SQL
spring.jpa.show-sql=true

# Formater le SQL
spring.jpa.properties.hibernate.format_sql=true

# Logger les paramètres
logging.level.org.hibernate.SQL=DEBUG
logging.level.org.hibernate.type.descriptor.sql.BasicBinder=TRACE
```

**Q39 : Comment optimiser les performances avec Hibernate Batch Processing ?**
**R :**
```properties
spring.jpa.properties.hibernate.jdbc.batch_size=20
spring.jpa.properties.hibernate.order_inserts=true
spring.jpa.properties.hibernate.order_updates=true
```
```java
@Transactional
public void saveInBatch(List<Course> courses) {
    for (int i = 0; i < courses.size(); i++) {
        entityManager.persist(courses.get(i));
        if (i % 20 == 0) {
            entityManager.flush();  // Envoie le batch
            entityManager.clear();  // Clear cache
        }
    }
}
```

**Q40 : Qu'est-ce que l'annotation @Formula d'Hibernate ?**
**R :**
```java
@Entity
public class Course {
    @Id
    private Long id;
    
    // Calcul SQL dans l'entité (pas de colonne physique)
    @Formula("(SELECT COUNT(*) FROM review r WHERE r.course_id = id)")
    private int reviewCount;
    
    @Formula("(SELECT AVG(r.rating) FROM review r WHERE r.course_id = id)")
    private Double averageRating;
}
```

**Q41 : Qu'est-ce que @CreationTimestamp et @UpdateTimestamp ?**
**R :**
```java
@Entity
public class Course {
    @org.hibernate.annotations.CreationTimestamp
    private LocalDateTime createdAt;  // Auto-rempli à la création
    
    @org.hibernate.annotations.UpdateTimestamp
    private LocalDateTime updatedAt;  // Auto-mis à jour à chaque modification
}
```
Alternative Hibernate aux annotations JPA Auditing.

## 📊 Auditing

**Q42 : Qu'est-ce que l'Auditing JPA ?**
**R :** L'Auditing JPA permet de tracer automatiquement qui a créé/modifié une entité et quand, sans code manuel. Utilise les annotations `@CreatedDate`, `@CreatedBy`, `@LastModifiedDate`, `@LastModifiedBy`.

**Q43 : Comment activer l'Auditing dans Spring Boot ?**
**R :**
```java
@Configuration
@EnableJpaAuditing
public class JpaConfig {
    
    @Bean
    public AuditorAware<String> auditorProvider() {
        return () -> {
            // Récupérer l'utilisateur actuel
            Authentication auth = SecurityContextHolder
                .getContext().getAuthentication();
            
            if (auth == null || !auth.isAuthenticated()) {
                return Optional.of("system");
            }
            
            return Optional.of(auth.getName());
        };
    }
}
```

**Q44 : Qu'est-ce que @MappedSuperclass ?**
**R :** `@MappedSuperclass` indique qu'une classe est une superclass pour d'autres entités. Ses champs seront hérités et inclus dans les tables des entités enfants. La classe elle-même n'est PAS une entité (pas de table).

**Q45 : Qu'est-ce que @EntityListeners(AuditingEntityListener.class) ?**
**R :** Active l'auditing pour l'entité. `AuditingEntityListener` est une classe Spring Data JPA qui remplit automatiquement les champs annotés avec `@CreatedDate`, `@CreatedBy`, `@LastModifiedDate`, `@LastModifiedBy`.

**Q46 : Créez une classe de base auditable complète**
**R :**
```java
@MappedSuperclass
@EntityListeners(AuditingEntityListener.class)
@Getter
@Setter
public abstract class BaseEntity {
    
    @CreatedDate
    @Column(name = "created_at", nullable = false, updatable = false)
    private LocalDateTime createdAt;
    
    @CreatedBy
    @Column(name = "created_by", updatable = false)
    private String createdBy;
    
    @LastModifiedDate
    @Column(name = "updated_at", insertable = false)
    private LocalDateTime updatedAt;
    
    @LastModifiedBy
    @Column(name = "updated_by", insertable = false)
    private String updatedBy;
    
    @Version
    private Long version;  // Optimistic Locking
}

// Utilisation
@Entity
public class Course extends BaseEntity {
    @Id
    @GeneratedValue
    private Long id;
    
    private String name;
    // createdAt, createdBy, updatedAt, updatedBy hérités
}
```

**Q47 : Quelle est la différence entre updatable=false et insertable=false ?**
**R :**
- **updatable=false** : Colonne ne peut pas être modifiée après création (pour `createdAt`, `createdBy`)
- **insertable=false** : Colonne ne peut pas être insérée manuellement (pour `updatedAt`, `updatedBy`)

**Q48 : Comment implémenter AuditorAware avec Spring Security ?**
**R :**
```java
@Component
public class AuditorAwareImpl implements AuditorAware<String> {
    
    @Override
    public Optional<String> getCurrentAuditor() {
        return Optional.ofNullable(SecurityContextHolder.getContext())
            .map(SecurityContext::getAuthentication)
            .filter(Authentication::isAuthenticated)
            .map(Authentication::getName);
    }
}

@Configuration
@EnableJpaAuditing(auditorAwareRef = "auditorAwareImpl")
public class JpaConfig { }
```

**Q49 : Peut-on utiliser un type personnalisé pour @CreatedBy au lieu de String ?**
**R :** Oui :
```java
@MappedSuperclass
@EntityListeners(AuditingEntityListener.class)
public abstract class BaseEntity {
    
    @CreatedBy
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "created_by_user_id")
    private User createdBy;  // Type personnalisé
    
    @LastModifiedBy
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "updated_by_user_id")
    private User updatedBy;
}

// AuditorAware
@Component
public class AuditorAwareImpl implements AuditorAware<User> {
    @Override
    public Optional<User> getCurrentAuditor() {
        // Retourner l'objet User complet
        return Optional.of(getCurrentUser());
    }
}
```

**Q50 : Qu'est-ce que @Version et à quoi sert-il ?**
**R :** `@Version` implémente l'Optimistic Locking. JPA incrémente automatiquement ce champ à chaque UPDATE et vérifie que personne d'autre n'a modifié l'entité entre-temps :
```java
@Entity
public class Course {
    @Id
    private Long id;
    
    @Version
    private Long version;  // 0, 1, 2, 3...
}

// Update
UPDATE course 
SET name = ?, version = version + 1 
WHERE id = ? AND version = ?  // Si version a changé → OptimisticLockException
```

## 📁 schema.sql et data.sql

**Q51 : Qu'est-ce que schema.sql et à quoi sert-il ?**
**R :** `schema.sql` est un fichier placé dans `src/main/resources/` contenant des commandes SQL DDL (CREATE TABLE, ALTER, etc.). Spring Boot l'exécute automatiquement au démarrage pour initialiser le schéma de la base de données.

**Q52 : Quelle est la différence entre schema.sql et data.sql ?**
**R :**
- **schema.sql** : Structure de la base (CREATE TABLE, indexes, contraintes)
- **data.sql** : Données initiales (INSERT INTO)

**Q53 : Exemple de schema.sql pour créer des tables**
**R :**
```sql
-- schema.sql
DROP TABLE IF EXISTS review;
DROP TABLE IF EXISTS course;

CREATE TABLE course (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    author VARCHAR(50) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE review (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    rating INT NOT NULL CHECK (rating BETWEEN 1 AND 5),
    comment VARCHAR(500),
    course_id BIGINT NOT NULL,
    FOREIGN KEY (course_id) REFERENCES course(id) ON DELETE CASCADE
);

CREATE INDEX idx_course_author ON course(author);
```

**Q54 : Comment activer l'exécution de schema.sql ?**
**R :**
```properties
# application.properties

# Toujours exécuter schema.sql
spring.sql.init.mode=always

# Déférer l'init après Hibernate (si nécessaire)
spring.jpa.defer-datasource-initialization=true

# Désactiver Hibernate DDL
spring.jpa.hibernate.ddl-auto=none
```

**Q55 : Quand utiliser schema.sql vs Hibernate DDL auto ?**
**R :**
- **schema.sql** : Production, schéma complexe, contrôle total, migration de DB existante
- **Hibernate DDL** : Développement, prototypage, tests

**Production : Toujours utiliser Flyway ou Liquibase, jamais schema.sql ou Hibernate DDL !**

**Q56 : Qu'est-ce que Flyway et Liquibase ?**
**R :** Outils de migration de base de données versionnés et incrémentaux :
- **Flyway** : Scripts SQL numérotés (V1__Initial.sql, V2__Add_column.sql)
- **Liquibase** : Changements XML/YAML/JSON

Ils tracent les migrations appliquées et permettent des rollbacks.

**Q57 : Comment utiliser Flyway avec Spring Boot ?**
**R :**
```xml
<!-- pom.xml -->
<dependency>
    <groupId>org.flywaydb</groupId>
    <artifactId>flyway-core</artifactId>
</dependency>
```
```
src/main/resources/db/migration/
├── V1__Create_course_table.sql
├── V2__Create_review_table.sql
└── V3__Add_rating_column.sql
```
```sql
-- V1__Create_course_table.sql
CREATE TABLE course (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    author VARCHAR(50) NOT NULL
);
```
Spring Boot exécute automatiquement les migrations au démarrage.

## 🗃️ H2 Console

**Q58 : Qu'est-ce que H2 Database ?**
**R :** H2 est une base de données relationnelle Java :
- **En mémoire** (rapide, données perdues à l'arrêt)
- **Ou sur fichier** (persistante)
- Console web intégrée
- Parfait pour développement et tests
- Compatible SQL standard

**Q59 : Comment configurer H2 dans Spring Boot ?**
**R :**
```properties
# application.properties

# Activer H2 Console
spring.h2.console.enabled=true
spring.h2.console.path=/h2-console

# Configuration H2
spring.datasource.url=jdbc:h2:mem:testdb
spring.datasource.driverClassName=org.h2.Driver
spring.datasource.username=sa
spring.datasource.password=

# JPA
spring.jpa.database-platform=org.hibernate.dialect.H2Dialect
spring.jpa.show-sql=true
spring.jpa.hibernate.ddl-auto=create-drop
```

**Q60 : Comment accéder à H2 Console ?**
**R :**
```
URL: http://localhost:8080/h2-console

JDBC URL: jdbc:h2:mem:testdb
Username: sa
Password: (vide)
```

**Q61 : Comment persister les données H2 sur fichier ?**
**R :**
```properties
# En mémoire (par défaut, données perdues)
spring.datasource.url=jdbc:h2:mem:testdb

# Sur fichier (persistant)
spring.datasource.url=jdbc:h2:file:./data/testdb

# Fichier avec chemin absolu
spring.datasource.url=jdbc:h2:file:/path/to/data/testdb
```

**Q62 : Pourquoi utiliser H2 en production est-il déconseillé ?**
**R :** H2 est conçu pour le développement et les tests, pas la production :
- Performances limitées sous charge
- Moins de fonctionnalités que PostgreSQL/MySQL
- Pas optimisé pour la concurrence élevée
- Support et communauté limités
- Manque de tooling professionnel

**Production : PostgreSQL, MySQL, Oracle, SQL Server**

## 🔄 Transactions

**Q63 : Qu'est-ce qu'une transaction et pourquoi est-elle importante ?**
**R :** Une transaction est une unité de travail qui respecte ACID :
- **Atomicity** : Tout ou rien (soit toutes les opérations réussissent, soit aucune)
- **Consistency** : État cohérent de la DB
- **Isolation** : Les transactions ne s'interfèrent pas
- **Durability** : Changements permanents après commit

**Q64 : Comment utiliser @Transactional ?**
**R :**
```java
@Service
public class CourseService {
    
    @Transactional  // Toute la méthode dans une transaction
    public void createCourseWithReviews(Course course, List<Review> reviews) {
        courseRepository.save(course);
        reviewRepository.saveAll(reviews);
        // Si exception → ROLLBACK automatique
        // Sinon → COMMIT à la fin
    }
    
    @Transactional(readOnly = true)  // Optimisation pour lectures seules
    public Course findById(Long id) {
        return courseRepository.findById(id)
            .orElseThrow(() -> new NotFoundException("Course not found"));
    }
}
```

**Q65 : Que se passe-t-il sans @Transactional ?**
**R :** Sans `@Transactional`, chaque opération repository est dans sa propre transaction :
```java
public void createCourseWithReviews(Course course, List<Review> reviews) {
    courseRepository.save(course);  // Transaction 1 - COMMIT
    // Exception ici !
    reviewRepository.saveAll(reviews);  // Jamais exécuté
    // Problème: course créé mais pas reviews → état incohérent
}
```

**Q66 : Quels sont les niveaux d'isolation de transaction ?**
**R :**
```java
// Du moins au plus isolé (et du plus au moins performant)

@Transactional(isolation = Isolation.READ_UNCOMMITTED)
// Peut lire des données non commitées (dirty reads)

@Transactional(isolation = Isolation.READ_COMMITTED)  // Par défaut
// Lit seulement les données commitées

@Transactional(isolation = Isolation.REPEATABLE_READ)
// Garantit que relire les mêmes données donne le même résultat

@Transactional(isolation = Isolation.SERIALIZABLE)
// Isolation totale, transactions séquentielles
```

**Q67 : Qu'est-ce que la propagation de transaction ?**
**R :**
```java
@Transactional(propagation = Propagation.REQUIRED)  // Par défaut
// Utilise transaction existante ou en crée une nouvelle

@Transactional(propagation = Propagation.REQUIRES_NEW)
// Suspend transaction actuelle et en crée une nouvelle

@Transactional(propagation = Propagation.MANDATORY)
// Doit être appelée dans une transaction existante (sinon exception)

@Transactional(propagation = Propagation.NEVER)
// Ne doit PAS être appelée dans une transaction (sinon exception)

@Transactional(propagation = Propagation.SUPPORTS)
// Exécute dans transaction si elle existe, sinon sans transaction
```

**Q68 : Comment gérer les rollbacks avec @Transactional ?**
**R :**
```java
// Par défaut: rollback seulement pour RuntimeException et Error
@Transactional
public void method() {
    throw new RuntimeException();  // ROLLBACK
}

// Rollback pour exceptions checked aussi
@Transactional(rollbackFor = Exception.class)
public void method() throws Exception {
    throw new Exception();  // ROLLBACK
}

// Ne pas rollback pour certaines exceptions
@Transactional(noRollbackFor = SpecificException.class)
public void method() {
    throw new SpecificException();  // COMMIT quand même
}

// Rollback manuel
@Transactional
public void method() {
    try {
        // code
    } catch (Exception e) {
        TransactionAspectSupport.currentTransactionStatus().setRollbackOnly();
    }
}
```

**Q69 : Pourquoi @Transactional ne fonctionne pas sur les méthodes privées ?**
**R :** `@Transactional` utilise des proxies AOP. Les proxies ne peuvent intercepter que les appels externes à la classe. Les appels internes (méthode privée ou this.method()) contournent le proxy :
```java
@Service
public class CourseService {
    
    public void publicMethod() {
        this.transactionalMethod();  // ❌ Pas de transaction !
        // Le proxy n'est pas utilisé
    }
    
    @Transactional
    private void transactionalMethod() {
        // @Transactional ignoré (méthode privée)
    }
}

// Solution: rendre publique ou injecter self
@Service
public class CourseService {
    
    @Autowired
    private CourseService self;  // Proxy
    
    public void publicMethod() {
        self.transactionalMethod();  // ✅ Transaction active
    }
    
    @Transactional
    public void transactionalMethod() {
        // Transaction fonctionne
    }
}
```

## 🔍 Relations JPA Avancées

**Q70 : Comment mapper une relation @OneToOne ?**
**R :**
```java
@Entity
public class User {
    @Id
    @GeneratedValue
    private Long id;
    
    @OneToOne(cascade = CascadeType.ALL)
    @JoinColumn(name = "profile_id", referencedColumnName = "id")
    private Profile profile;
}

@Entity
public class Profile {
    @Id
    @GeneratedValue
    private Long id;
    
    @OneToOne(mappedBy = "profile")  // Bidirectionnel
    private User user;
}
```

**Q71 : Comment mapper une relation @ManyToOne ?**
**R :**
```java
@Entity
public class Review {
    @Id
    @GeneratedValue
    private Long id;
    
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "course_id")  // Foreign key
    private Course course;
}

@Entity
public class Course {
    @Id
    @GeneratedValue
    private Long id;
    
    @OneToMany(mappedBy = "course", cascade = CascadeType.ALL)
    private List<Review> reviews = new ArrayList<>();
}
```

**Q72 : Comment mapper une relation @ManyToMany ?**
**R :**
```java
@Entity
public class Student {
    @Id
    @GeneratedValue
    private Long id;
    
    @ManyToMany
    @JoinTable(
        name = "student_course",
        joinColumns = @JoinColumn(name = "student_id"),
        inverseJoinColumns = @JoinColumn(name = "course_id")
    )
    private List<Course> courses = new ArrayList<>();
}

@Entity
public class Course {
    @Id
    @GeneratedValue
    private Long id;
    
    @ManyToMany(mappedBy = "courses")
    private List<Student> students = new ArrayList<>();
}

// Table de jointure créée automatiquement:
// student_course (student_id, course_id)
```

**Q73 : Comment ajouter des attributs à une table de jointure @ManyToMany ?**
**R :** Créer une entité dédiée :
```java
@Entity
public class Enrollment {
    @Id
    @GeneratedValue
    private Long id;
    
    @ManyToOne
    @JoinColumn(name = "student_id")
    private Student student;
    
    @ManyToOne
    @JoinColumn(name = "course_id")
    private Course course;
    
    private LocalDate enrollmentDate;  // Attribut supplémentaire
    private String grade;
}

@Entity
public class Student {
    @OneToMany(mappedBy = "student")
    private List<Enrollment> enrollments;
}

@Entity
public class Course {
    @OneToMany(mappedBy = "course")
    private List<Enrollment> enrollments;
}
```

**Q74 : Qu'est-ce que orphanRemoval ?**
**R :** `orphanRemoval = true` supprime automatiquement les entités enfants qui ne sont plus référencées :
```java
@Entity
public class Course {
    @OneToMany(mappedBy = "course", 
               cascade = CascadeType.ALL, 
               orphanRemoval = true)
    private List<Review> reviews = new ArrayList<>();
}

// Utilisation
Course course = courseRepository.findById(1L);
course.getReviews().remove(0);  // Review supprimée de la liste
courseRepository.save(course);
// Review automatiquement DELETE de la DB (orphan removal)
```

**Q75 : Quelle est la différence entre CascadeType.REMOVE et orphanRemoval ?**
**R :**
- **CascadeType.REMOVE** : Supprime les enfants quand le parent est supprimé
- **orphanRemoval = true** : Supprime les enfants quand ils sont retirés de la collection

```java
// CascadeType.REMOVE
courseRepository.delete(course);  // Reviews supprimées
course.getReviews().remove(review);  // Review PAS supprimée

// orphanRemoval = true
courseRepository.delete(course);  // Reviews supprimées
course.getReviews().remove(review);  // Review supprimée aussi
```

## 📊 Projections et DTOs

**Q76 : Qu'est-ce qu'une Projection JPA ?**
**R :** Une Projection permet de récupérer seulement certains champs d'une entité au lieu de toute l'entité :
```java
// Interface Projection
public interface CourseNameOnly {
    String getName();
    String getAuthor();
}

// Repository
public interface CourseRepository extends JpaRepository<Course, Long> {
    List<CourseNameOnly> findAllProjectedBy();
    // SELECT name, author FROM course (pas toutes les colonnes)
}

// Utilisation
List<CourseNameOnly> courses = repository.findAllProjectedBy();
courses.forEach(c -> System.out.println(c.getName()));
```

**Q77 : Qu'est-ce qu'une DTO Projection ?**
**R :** Utiliser un DTO (Data Transfer Object) au lieu d'une interface :
```java
// DTO
public class CourseDTO {
    private Long id;
    private String name;
    
    public CourseDTO(Long id, String name) {
        this.id = id;
        this.name = name;
    }
    // Getters
}

// Repository avec @Query
@Query("SELECT new com.example.dto.CourseDTO(c.id, c.name) FROM Course c")
List<CourseDTO> findAllDTOs();

// Utilisation
List<CourseDTO> dtos = repository.findAllDTOs();
```

**Q78 : Quelle est la différence entre Interface Projection et DTO Projection ?**
**R :**
- **Interface Projection** : Plus simple, Spring génère l'implémentation dynamiquement
- **DTO Projection** : Plus de contrôle, peut ajouter logique métier, sérialisable

## 🛠️ Optimisations et Bonnes Pratiques

**Q79 : Comment éviter le problème N+1 avec @BatchSize ?**
**R :**
```java
@Entity
public class Course {
    @OneToMany(mappedBy = "course")
    @BatchSize(size = 10)  // Charge 10 relations à la fois
    private List<Review> reviews;
}

// Au lieu de N+1 queries, fait 1 + (N/10) queries
```

**Q80 : Quand utiliser FetchType.LAZY vs EAGER ?**
**R :**
- **LAZY** (défaut pour @OneToMany, @ManyToMany) : Préféré, charge à la demande
- **EAGER** (défaut pour @ManyToOne, @OneToOne) : Éviter, peut causer N+1

**Best Practice : Toujours LAZY, utiliser fetch join quand nécessaire**

**Q81 : Comment utiliser @Query avec pagination ?**
**R :**
```java
@Query("SELECT c FROM Course c WHERE c.author = :author")
Page<Course> findByAuthorCustom(@Param("author") String author, Pageable pageable);

// Spring ajoute automatiquement LIMIT/OFFSET
// Fait aussi un COUNT query pour le total
```

**Q82 : Comment désactiver temporairement l'auditing pour certaines opérations ?**
**R :**
```java
@Service
public class BulkImportService {
    
    @PersistenceContext
    private EntityManager entityManager;
    
    @Transactional
    public void bulkImport(List<Course> courses) {
        // Désactiver auditing temporairement pour performance
        Session session = entityManager.unwrap(Session.class);
        session.setDefaultReadOnly(true);
        
        for (Course course : courses) {
            entityManager.persist(course);
            if (courses.indexOf(course) % 50 == 0) {
                entityManager.flush();
                entityManager.clear();
            }
        }
    }
}
```

**Q83 : Comment gérer les soft deletes (suppression logique) ?**
**R :**
```java
@Entity
@Where(clause = "deleted = false")  // Hibernate specific
@SQLDelete(sql = "UPDATE course SET deleted = true WHERE id = ?")
public class Course {
    @Id
    private Long id;
    
    private String name;
    
    private boolean deleted = false;
}

// Ou avec JPA standard
@Entity
public class Course {
    @Id
    private Long id;
    
    private String name;
    
    @Column(name = "deleted")
    private boolean deleted = false;
}

// Repository
public interface CourseRepository extends JpaRepository<Course, Long> {
    @Query("SELECT c FROM Course c WHERE c.deleted = false")
    List<Course> findAllActive();
    
    @Modifying
    @Query("UPDATE Course c SET c.deleted = true WHERE c.id = :id")
    void softDelete(@Param("id") Long id);
}
```

**Q84 : Comment implémenter un audit complet avec Hibernate Envers ?**
**R :**
```xml
<!-- pom.xml -->
<dependency>
    <groupId>org.hibernate</groupId>
    <artifactId>hibernate-envers</artifactId>
</dependency>
```
```java
@Entity
@Audited  // Active l'audit complet (historique de toutes les modifications)
public class Course {
    @Id
    private Long id;
    
    private String name;
    private String author;
}

// Tables créées automatiquement:
// - course (table normale)
// - course_AUD (historique avec REV et REVTYPE)
// - REVINFO (informations sur les révisions)

// Récupérer l'historique
AuditReader reader = AuditReaderFactory.get(entityManager);

// Toutes les révisions
List<Number> revisions = reader.getRevisions(Course.class, courseId);

// Entité à une révision spécifique
Course courseAtRevision = reader.find(Course.class, courseId, revisionNumber);

// Historique complet
List<Course> history = reader.createQuery()
    .forRevisionsOfEntity(Course.class, false, true)
    .add(AuditEntity.id().eq(courseId))
    .getResultList();
```

**Q85 : Comment utiliser @QueryHints pour optimiser les performances ?**
**R :**
```java
@QueryHints(@QueryHint(name = "org.hibernate.cacheable", value = "true"))
@Query("SELECT c FROM Course c WHERE c.author = :author")
List<Course> findByAuthorCacheable(@Param("author") String author);

// Autres hints utiles
@QueryHints({
    @QueryHint(name = "org.hibernate.fetchSize", value = "50"),
    @QueryHint(name = "org.hibernate.readOnly", value = "true")
})
List<Course> findAllOptimized();
```

## 🎯 Scénarios Pratiques

**Q86 : Comment implémenter un système de recherche full-text ?**
**R :**
```java
// Option 1: LIKE (basique)
@Query("SELECT c FROM Course c WHERE " +
       "LOWER(c.name) LIKE LOWER(CONCAT('%', :query, '%')) OR " +
       "LOWER(c.author) LIKE LOWER(CONCAT('%', :query, '%'))")
List<Course> search(@Param("query") String query);

// Option 2: Hibernate Search (Lucene/Elasticsearch)
@Entity
@Indexed  // Hibernate Search
public class Course {
    @Id
    private Long id;
    
    @Field
    private String name;
    
    @Field
    private String author;
}

// Recherche full-text
List<Course> results = Search.session(entityManager)
    .search(Course.class)
    .where(f -> f.match()
        .fields("name", "author")
        .matching(query))
    .fetchHits(20);
```

**Q87 : Comment implémenter un système de tags Many-to-Many avec attributs ?**
**R :**
```java
@Entity
public class Article {
    @Id
    private Long id;
    
    @OneToMany(mappedBy = "article", cascade = CascadeType.ALL)
    private List<ArticleTag> articleTags = new ArrayList<>();
    
    public void addTag(Tag tag, User tagger) {
        ArticleTag articleTag = new ArticleTag(this, tag, tagger);
        articleTags.add(articleTag);
    }
}

@Entity
public class Tag {
    @Id
    private Long id;
    
    private String name;
    
    @OneToMany(mappedBy = "tag")
    private List<ArticleTag> articleTags;
}

@Entity
public class ArticleTag {
    @Id
    @GeneratedValue
    private Long id;
    
    @ManyToOne
    @JoinColumn(name = "article_id")
    private Article article;
    
    @ManyToOne
    @JoinColumn(name = "tag_id")
    private Tag tag;
    
    @ManyToOne
    private User taggedBy;
    
    @CreatedDate
    private LocalDateTime taggedAt;
}
```

**Q88 : Comment implémenter un système de commentaires hiérarchiques (parent/child) ?**
**R :**
```java
@Entity
public class Comment {
    @Id
    @GeneratedValue
    private Long id;
    
    private String content;
    
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "parent_id")
    private Comment parent;
    
    @OneToMany(mappedBy = "parent", cascade = CascadeType.ALL)
    private List<Comment> replies = new ArrayList<>();
    
    @ManyToOne
    private User author;
}

// Repository
public interface CommentRepository extends JpaRepository<Comment, Long> {
    // Seulement les commentaires racines (sans parent)
    List<Comment> findByParentIsNull();
    
    // Avec fetch join pour éviter N+1
    @Query("SELECT c FROM Comment c LEFT JOIN FETCH c.replies WHERE c.parent IS NULL")
    List<Comment> findRootCommentsWithReplies();
}
```

**Q89 : Comment implémenter un compteur de vues avec optimistic locking ?**
**R :**
```java
@Entity
public class Article {
    @Id
    private Long id;
    
    private String title;
    
    private Long viewCount = 0L;
    
    @Version
    private Long version;
    
    public void incrementViews() {
        this.viewCount++;
    }
}

@Service
public class ArticleService {
    
    @Autowired
    private ArticleRepository repository;
    
    @Transactional
    public void incrementViews(Long articleId) {
        int maxRetries = 3;
        int attempt = 0;
        
        while (attempt < maxRetries) {
            try {
                Article article = repository.findById(articleId)
                    .orElseThrow();
                article.incrementViews();
                repository.save(article);
                return;  // Succès
            } catch (OptimisticLockException e) {
                attempt++;
                if (attempt >= maxRetries) {
                    throw e;
                }
                // Retry avec backoff
                Thread.sleep(100 * attempt);
            }
        }
    }
}
```

**Q90 : Comment créer une entité avec composite key (clé composite) ?**
**R :**
```java
// Classe de clé composite
@Embeddable
public class EnrollmentId implements Serializable {
    private Long studentId;
    private Long courseId;
    
    // Constructeurs, equals, hashCode
}

// Entité avec clé composite
@Entity
public class Enrollment {
    @EmbeddedId
    private EnrollmentId id;
    
    @ManyToOne
    @MapsId("studentId")
    @JoinColumn(name = "student_id")
    private Student student;
    
    @ManyToOne
    @MapsId("courseId")
    @JoinColumn(name = "course_id")
    private Course course;
    
    private LocalDate enrollmentDate;
}

// Alternative avec @IdClass
@IdClass(EnrollmentId.class)
@Entity
public class Enrollment {
    @Id
    private Long studentId;
    
    @Id
    private Long courseId;
    
    @ManyToOne
    @JoinColumn(name = "student_id", insertable = false, updatable = false)
    private Student student;
    
    @ManyToOne
    @JoinColumn(name = "course_id", insertable = false, updatable = false)
    private Course course;
}
```

---
