## [**..**](./00_index.md)

# Spring JDBC, JPA, Hibernate & Auditing

## 🗄️ Spring JDBC - Basic Database Access

### Spring JDBC Concept

**Spring JDBC** simplifies database access by eliminating traditional JDBC boilerplate code (connection management, ResultSet, exceptions).

### Traditional JDBC (The Problem)

```java
// Pure JDBC - lots of boilerplate code
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
    // Error handling
} finally {
    // Close rs, ps, conn (forgetting = memory leak)
    if (rs != null) rs.close();
    if (ps != null) ps.close();
    if (conn != null) conn.close();
}
```

### Spring JDBC with JdbcTemplate

```java
@Repository
public class CourseJdbcRepository {
    
    @Autowired
    private JdbcTemplate jdbcTemplate;
    
    // Simple query
    public Course findById(Long id) {
        String sql = "SELECT * FROM course WHERE id = ?";
        return jdbcTemplate.queryForObject(sql, 
            new BeanPropertyRowMapper<>(Course.class), 
            id);
    }
    
    // List query
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
    
    // Custom RowMapper
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

**Spring JDBC advantages :**
- Automatic connection management
- Automatic exception handling (translated into DataAccessException)
- Less boilerplate code
- No manual resource closing

**Disadvantages :**
- Manual SQL queries (verbose)
- Manual mapping between ResultSet and objects
- No automatic relationship management

## 🏛️ JPA (Jakarta Persistence API) - ORM Standard

### JPA Concept

**JPA** is a Java specification for ORM (Object-Relational Mapping). It defines how to map Java objects to database tables and vice versa.

**JPA is a specification, not an implementation!**

### Basic JPA Entity

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
    
    // Constructors, getters, setters
}
```

### EntityManager - Basic JPA API

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
    
    // Query - SELECT with JPQL
    public List<Course> findAll() {
        TypedQuery<Course> query = entityManager.createQuery(
            "SELECT c FROM Course c", Course.class);
        return query.getResultList();
    }
    
    // Query with parameters
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
// JPQL - uses entity and property names (not SQL tables/columns)
"SELECT c FROM Course c WHERE c.author = :author"

// Equivalent SQL
"SELECT * FROM course WHERE author = ?"

// JPQL with joins
"SELECT c FROM Course c JOIN c.students s WHERE s.name = :name"

// JPQL with aggregations
"SELECT c.author, COUNT(c) FROM Course c GROUP BY c.author"
```

### The Magic of JPA

**1. Automatic Object-Relational Mapping**
```java
Course course = new Course("Spring Boot", "John");
entityManager.persist(course);
// JPA automatically generates: INSERT INTO course (name, author) VALUES (?, ?)
```

**2. Automatic Relationship Management**
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

// JPA automatically manages foreign keys and joins
```

**3. Lazy Loading**
```java
@Entity
public class Course {
    @OneToMany(fetch = FetchType.LAZY)
    private List<Review> reviews;  // Loaded only when accessed
}
```

**4. Caching (First-Level Cache)**
```java
Course c1 = entityManager.find(Course.class, 1L);  // SELECT from DB
Course c2 = entityManager.find(Course.class, 1L);  // From cache, no SELECT
```

**5. Dirty Checking**
```java
@Transactional
public void updateCourse(Long id) {
    Course course = entityManager.find(Course.class, id);
    course.setName("Updated Name");
    // No need to call save()!
    // JPA detects the change and does UPDATE automatically
}
```

## 🚀 Spring Data JPA - Ultimate Simplification

### Spring Data JPA Concept

Spring Data JPA adds an abstraction layer on top of JPA, eliminating even the need to write queries for basic CRUD operations.

### Repository Interface

```java
public interface CourseRepository extends JpaRepository<Course, Long> {
    // No implementation needed! Spring generates the code automatically
}
```

**Automatically inherited methods :**
```java
// Basic CRUD
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

// Pagination and sorting
findAll(Pageable pageable)
findAll(Sort sort)
```

### Query Methods - Automatic Generation

```java
public interface CourseRepository extends JpaRepository<Course, Long> {
    
    // Spring automatically generates the query from the method name!
    
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
    // SELECT COUNT(*) FROM course WHERE name = ? (returns boolean)
    
    void deleteByAuthor(String author);
    // DELETE FROM course WHERE author = ?
}
```

### @Query - Custom Queries

```java
public interface CourseRepository extends JpaRepository<Course, Long> {
    
    // Custom JPQL
    @Query("SELECT c FROM Course c WHERE c.author = :author")
    List<Course> findCoursesByAuthor(@Param("author") String author);
    
    // JPQL with multiple parameters
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
    
    // Projection (only certain fields)
    @Query("SELECT c.name FROM Course c WHERE c.author = :author")
    List<String> findCourseNames(@Param("author") String author);
    
    // DTO Projection
    @Query("SELECT new com.example.dto.CourseDTO(c.id, c.name) FROM Course c")
    List<CourseDTO> findAllDTOs();
}
```

### Pagination and Sorting with Spring Data JPA

```java
public interface CourseRepository extends JpaRepository<Course, Long> {
    
    // Automatically generated pagination methods
    Page<Course> findByAuthor(String author, Pageable pageable);
    
    List<Course> findByAuthor(String author, Sort sort);
}

// Usage in Service
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

### Specifications (Dynamic Queries)

```java
public interface CourseRepository extends JpaRepository<Course, Long>, 
                                         JpaSpecificationExecutor<Course> {
}

// Specifications
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

// Usage
Specification<Course> spec = Specification
    .where(CourseSpecifications.hasAuthor(author))
    .and(CourseSpecifications.nameContains(keyword));

List<Course> courses = repository.findAll(spec);
```

## 🐘 Hibernate - JPA Implementation

### JPA vs Hibernate - Fundamental Difference

```
JPA (Jakarta Persistence API)
    ↓ Specification (interfaces, annotations)
    
Hibernate
    ↓ JPA Implementation
    
Other JPA implementations:
    - EclipseLink
    - OpenJPA
```

**JPA** : Specification (set of interfaces and annotations)
- `@Entity`, `@Id`, `@Column`, `EntityManager`, etc.

**Hibernate** : Concrete implementation of JPA
- Provides the code that makes JPA work
- Adds extra features

### Comparison Table

| Aspect | JPA | Hibernate |
|--------|-----|-----------|
| **Nature** | Specification | Implementation |
| **Annotations** | `@Entity`, `@Id`, `@Column` | Supports JPA + Hibernate-specific annotations |
| **Package** | `jakarta.persistence.*` | `org.hibernate.*` |
| **Portability** | ✅ Portable between implementations | ❌ Hibernate-specific |
| **Features** | Standard | JPA + extra features |

### Code Example

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

// Hibernate Specific (not portable)
import org.hibernate.annotations.*;

@Entity
@DynamicUpdate  // Hibernate-only annotation
@SelectBeforeUpdate  // Hibernate-only annotation
public class Course {
    @Id
    @GeneratedValue(generator = "custom-generator")
    @GenericGenerator(name = "custom-generator", strategy = "uuid2")  // Hibernate
    private String id;
    
    @Formula("(SELECT COUNT(*) FROM review r WHERE r.course_id = id)")  // Hibernate
    private int reviewCount;
}
```

### Extra Hibernate Features

**1. Second-Level Cache**
```java
@Entity
@Cacheable
@org.hibernate.annotations.Cache(usage = CacheConcurrencyStrategy.READ_WRITE)
public class Course {
    // Cache shared between sessions
}
```

**2. Batch Processing**
```properties
spring.jpa.properties.hibernate.jdbc.batch_size=20
spring.jpa.properties.hibernate.order_inserts=true
spring.jpa.properties.hibernate.order_updates=true
```

**3. Hibernate-Specific Annotations**
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

### Hibernate Configuration in Spring Boot

```properties
# application.properties

# Show SQL queries
spring.jpa.show-sql=true
spring.jpa.properties.hibernate.format_sql=true

# Log level
logging.level.org.hibernate.SQL=DEBUG
logging.level.org.hibernate.type.descriptor.sql.BasicBinder=TRACE

# DDL (table creation)
spring.jpa.hibernate.ddl-auto=update
# - none: Nothing
# - validate: Validates the schema
# - update: Updates the schema
# - create: Drops and recreates
# - create-drop: Drops on shutdown

# Dialect
spring.jpa.properties.hibernate.dialect=org.hibernate.dialect.H2Dialect

# Performance
spring.jpa.properties.hibernate.jdbc.batch_size=20
spring.jpa.properties.hibernate.jdbc.fetch_size=50
```

## 📊 Auditing - Automatic Traceability

### Auditing Concept

Auditing allows automatically tracking who created/modified an entity and when, without manual code in each service.

### Basic Configuration

```java
@Configuration
@EnableJpaAuditing
public class JpaConfig {
    
    @Bean
    public AuditorAware<String> auditorProvider() {
        return () -> {
            // Get the current user from Spring Security
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

### Auditable Base Class

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
    
    // Getters and setters
}
```

### Usage in Entities

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
    
    // createdAt, createdBy, updatedAt, updatedBy inherited from BaseEntity
    // Automatically managed by JPA Auditing
}
```

### Annotation Explanations

#### @MappedSuperclass
- Marks this class as a superclass for other entities
- Its fields will be inherited by child entities
- The class itself is NOT an entity (no table)
- Fields will appear in child entity tables

#### @EntityListeners(AuditingEntityListener.class)
- Enables auditing for the entity
- `AuditingEntityListener` is provided by Spring Data JPA
- Automatically fills fields annotated with @CreatedDate, @CreatedBy, etc.

#### @CreatedDate
- Automatically fills the field on creation (INSERT)
- Type : `LocalDateTime`, `Instant`, `Date`, `Long` (timestamp)

#### @CreatedBy
- Automatically fills the field with the creator's username
- Requires `AuditorAware<T>` to provide the current user
- Type : `String`, `Long`, or custom type (User)

#### @LastModifiedDate
- Automatically updates the field on modifications (UPDATE)
- Not updated on creation

#### @LastModifiedBy
- Automatically updates with the modifying user
- Not updated on creation

#### @Column
```java
@Column(updatable = false)  // Cannot be modified after creation (createdAt, createdBy)
@Column(insertable = false)  // Cannot be manually inserted (updatedAt, updatedBy)
```

### Complete Example with Auditing

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

// Entity using auditing
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
        
        // createdAt and createdBy filled automatically
        return repository.save(product);
    }
    
    @Transactional
    public Product updateProduct(Long id, String name) {
        Product product = repository.findById(id)
            .orElseThrow(() -> new NotFoundException("Product not found"));
        
        product.setName(name);
        
        // updatedAt and updatedBy filled automatically
        return repository.save(product);
    }
}
```

### Result in Database

```sql
-- After creation
INSERT INTO product (name, price, created_at, created_by, version)
VALUES ('Laptop', 999.99, '2025-01-15 10:30:00', 'john.doe', 0);

-- After modification
UPDATE product 
SET name = 'Gaming Laptop', 
    updated_at = '2025-01-16 14:20:00',
    updated_by = 'jane.smith',
    version = 1
WHERE id = 1 AND version = 0;  -- Optimistic locking
```

## 📁 schema.sql - Schema Initialization

### schema.sql Concept

SQL file automatically executed by Spring Boot at startup to initialize the database schema.

### Location

```
src/main/resources/schema.sql
```

### schema.sql Example

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

### data.sql - Initial Data

```sql
-- data.sql (test data)

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

# Enable schema.sql execution (disabled by default if JPA/Hibernate manages schema)
spring.sql.init.mode=always

# Execution order
spring.jpa.defer-datasource-initialization=true

# If you use Hibernate to create the schema, disable schema.sql
spring.jpa.hibernate.ddl-auto=none
```

### When to Use schema.sql vs Hibernate DDL

**Use schema.sql when :**
- Complex schema with specific constraints
- Migrating an existing database
- Need for full control over SQL
- Production (no Hibernate DDL)

**Use Hibernate DDL (`ddl-auto`) when :**
- Quick prototyping
- Local development
- Tests (with in-memory H2)

**Production Best Practice :**
Use migration tools like **Flyway** or **Liquibase** instead of schema.sql

## 🗃️ H2 Console - In-Memory Database

### H2 Configuration

```properties
# application.properties

# Enable H2 Console
spring.h2.console.enabled=true
spring.h2.console.path=/h2-console

# H2 Configuration
spring.datasource.url=jdbc:h2:mem:testdb
spring.datasource.driverClassName=org.h2.Driver
spring.datasource.username=sa
spring.datasource.password=

# JPA/Hibernate
spring.jpa.database-platform=org.hibernate.dialect.H2Dialect
spring.jpa.show-sql=true
```

### Accessing H2 Console

```
URL: http://localhost:8080/h2-console

JDBC URL: jdbc:h2:mem:testdb
Username: sa
Password: (empty)
```

### Uses of H2

- In-memory database (fast)
- Perfect for development and tests
- Built-in web console
- Standard SQL compatible
- No external configuration needed

---

# Q&A - Spring JDBC, JPA, Hibernate & Auditing

## 🗄️ Spring JDBC

**Q1 : What is the main problem with traditional JDBC?**
**A :** Traditional JDBC requires a lot of boilerplate code : manual management of connections, ResultSet, PreparedStatement, resource closing (memory leak risk), verbose exception handling. Lots of repetitive code for simple operations.

**Q2 : How does Spring JDBC simplify database access?**
**A :** Spring JDBC provides `JdbcTemplate` which :
- Automatically manages connections and closes them
- Translates SQLExceptions into DataAccessException (unchecked)
- Eliminates boilerplate code
- Provides utility methods (`query`, `update`, `queryForObject`)

**Q3 : What is JdbcTemplate?**
**A :** `JdbcTemplate` is the central class of Spring JDBC that simplifies SQL query execution. It automatically manages resources (connections, statements) and exceptions.

**Q4 : How do you do a SELECT query with JdbcTemplate?**
**A :**
```java
// Single object
Course course = jdbcTemplate.queryForObject(
    "SELECT * FROM course WHERE id = ?",
    new BeanPropertyRowMapper<>(Course.class),
    id
);

// List of objects
List<Course> courses = jdbcTemplate.query(
    "SELECT * FROM course",
    new BeanPropertyRowMapper<>(Course.class)
);
```

**Q5 : How do you do an INSERT/UPDATE/DELETE with JdbcTemplate?**
**A :**
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

**Q6 : What is a RowMapper?**
**A :** A `RowMapper` is an interface that defines how to map a ResultSet row to a Java object :
```java
jdbcTemplate.query("SELECT * FROM course", (rs, rowNum) -> {
    Course course = new Course();
    course.setId(rs.getLong("id"));
    course.setName(rs.getString("name"));
    return course;
});
```

**Q7 : When should you use Spring JDBC instead of JPA?**
**A :** Use Spring JDBC for :
- Complex optimized SQL queries
- Full control over SQL
- Legacy applications with existing SQL
- Critical performance (no ORM overhead)
- Specific databases (stored procedures)

## 🏛️ JPA (Jakarta Persistence API)

**Q8 : What is JPA?**
**A :** JPA (Jakarta Persistence API) is a Java **specification** for ORM (Object-Relational Mapping). It defines how to map Java objects to database tables. JPA is NOT an implementation, it is a standard.

**Q9 : What is the difference between JPA and Hibernate?**
**A :**
- **JPA** : Specification (interfaces, annotations) - package `jakarta.persistence.*`
- **Hibernate** : Concrete implementation of JPA - provides the code that makes JPA work

JPA = Interface, Hibernate = Implementation (like JDBC = Interface, MySQL Driver = Implementation)

**Q10 : What are the main JPA annotations?**
**A :**
- `@Entity` : Marks a class as a JPA entity (table)
- `@Table(name="...")` : Specifies the table name
- `@Id` : Primary key
- `@GeneratedValue` : Automatic ID generation
- `@Column` : Column configuration
- `@Transient` : Field not persisted in the database

**Q11 : What is EntityManager?**
**A :** `EntityManager` is the main JPA API for interacting with the database. It manages the lifecycle of entities (persist, merge, find, remove) and the persistence context.

**Q12 : What is the difference between persist() and merge()?**
**A :**
- **persist()** : For new entities (INSERT). Exception if the entity already exists
- **merge()** : For detached entities (UPDATE). Can also INSERT if the entity doesn't exist

```java
// New entity
Course course = new Course("Spring Boot");
entityManager.persist(course);  // INSERT

// Update
course.setName("Updated");
entityManager.merge(course);  // UPDATE
```

**Q13 : What is JPQL?**
**A :** JPQL (Java Persistence Query Language) is an object-oriented query language for JPA. It uses entity and property names instead of SQL tables and columns :
```java
// JPQL (object-oriented)
"SELECT c FROM Course c WHERE c.author = :author"

// Equivalent SQL
"SELECT * FROM course WHERE author = ?"
```

**Q14 : What is JPA's First-Level Cache?**
**A :** The First-Level Cache is a cache at the EntityManager (session) level. If you retrieve the same entity twice in the same transaction, JPA returns the same instance from the cache without a SQL query :
```java
Course c1 = entityManager.find(Course.class, 1L);  // SELECT
Course c2 = entityManager.find(Course.class, 1L);  // No SELECT, from cache
assertSame(c1, c2);  // Same instance!
```

**Q15 : What is Dirty Checking?**
**A :** Dirty Checking is JPA's ability to automatically detect entity modifications and do an UPDATE without explicitly calling `save()` or `merge()` :
```java
@Transactional
public void updateCourse(Long id) {
    Course course = entityManager.find(Course.class, id);
    course.setName("Updated");
    // No need to call save()!
    // JPA detects the change and does UPDATE automatically
}
```

**Q16 : What is Lazy Loading?**
**A :** Lazy Loading means relationships are only loaded when accessed, not at initial load time :
```java
@Entity
public class Course {
    @OneToMany(fetch = FetchType.LAZY)  // Default for @OneToMany
    private List<Review> reviews;  // Not loaded immediately
}

Course course = entityManager.find(Course.class, 1L);  // SELECT course
List<Review> reviews = course.getReviews();  // SELECT reviews (now)
```

**Q17 : What is the difference between FetchType.LAZY and FetchType.EAGER?**
**A :**
- **LAZY** : Data loaded on demand (when accessed). Saves queries
- **EAGER** : Data loaded immediately with the parent entity. Can cause performance issues (N+1)

**Q18 : What are the JPA relationship types?**
**A :**
- `@OneToOne` : One-to-one (User ↔ Profile)
- `@OneToMany` : One-to-many (Course → Reviews)
- `@ManyToOne` : Many-to-one (Reviews → Course)
- `@ManyToMany` : Many-to-many (Students ↔ Courses)

**Q19 : How does @GeneratedValue work?**
**A :**
```java
@Id
@GeneratedValue(strategy = GenerationType.IDENTITY)  // DB auto-increment
private Long id;

// Other strategies:
// - AUTO (default, chosen automatically)
// - IDENTITY (auto-increment)
// - SEQUENCE (DB sequence)
// - TABLE (dedicated table for IDs)
// - UUID (with Hibernate)
```

**Q20 : What is CascadeType?**
**A :** `CascadeType` propagates operations from the parent entity to the children :
```java
@OneToMany(cascade = CascadeType.ALL)
private List<Review> reviews;

// If we delete the Course, Reviews are also deleted
// If we persist the Course, Reviews are also persisted

// Types:
// - ALL (all operations)
// - PERSIST, MERGE, REMOVE, REFRESH, DETACH
```

## 🚀 Spring Data JPA

**Q21 : What is Spring Data JPA?**
**A :** Spring Data JPA is an abstraction layer on top of JPA that eliminates the need to write repository implementations. It automatically generates code for basic and complex queries.

**Q22 : How do you create a Spring Data JPA repository?**
**A :**
```java
public interface CourseRepository extends JpaRepository<Course, Long> {
    // No implementation needed!
    // Spring generates the code automatically
}
```

**Q23 : What methods are automatically available with JpaRepository?**
**A :**
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

// Pagination and sorting
findAll(Pageable)
findAll(Sort)
```

**Q24 : How do Query Methods (derived methods) work?**
**A :** Spring Data JPA automatically generates queries from the method name :
```java
// Spring automatically generates the SQL!
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

**Q25 : What keywords are supported in Query Methods?**
**A :**
- **And / Or** : `findByNameAndAuthor`, `findByNameOrAuthor`
- **Containing / Like** : `findByNameContaining` (LIKE %x%)
- **StartingWith / EndingWith** : `findByNameStartingWith` (LIKE x%)
- **GreaterThan / LessThan** : `findByIdGreaterThan`
- **Between** : `findByPriceBetween`
- **In** : `findByAuthorIn(List<String>)`
- **IsNull / IsNotNull** : `findByAuthorIsNull`
- **OrderBy** : `findByAuthorOrderByNameAsc`

**Q26 : How do you write a custom query with @Query?**
**A :**
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

**Q27 : What is the difference between JPQL and Native SQL in @Query?**
**A :**
- **JPQL** : Uses entity and property names (object-oriented). Portable between databases
- **Native SQL** : Pure SQL, uses table and column names. Database-specific

**Q28 : How do you implement pagination with Spring Data JPA?**
**A :**
```java
// Repository
Page<Course> findByAuthor(String author, Pageable pageable);

// Service
public Page<Course> getCourses(int page, int size) {
    Pageable pageable = PageRequest.of(page, size, 
                                       Sort.by("name").ascending());
    return repository.findAll(pageable);
}

// Page contains:
// - getContent() : list of items
// - getTotalElements() : total items
// - getTotalPages() : number of pages
// - getNumber() : current page number
```

**Q29 : How do you sort results with Spring Data JPA?**
**A :**
```java
// Simple sort
Sort sort = Sort.by("name").ascending();
List<Course> courses = repository.findAll(sort);

// Multiple sort
Sort sort = Sort.by("author").ascending()
                .and(Sort.by("name").descending());

// With pagination
Pageable pageable = PageRequest.of(0, 10, sort);
Page<Course> page = repository.findAll(pageable);
```

**Q30 : What is JpaSpecificationExecutor and when should you use it?**
**A :** `JpaSpecificationExecutor` allows building dynamic queries with conditional criteria :
```java
public interface CourseRepository extends JpaRepository<Course, Long>, 
                                         JpaSpecificationExecutor<Course> {
}

// Dynamic specification
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
Useful for complex dynamic filters.

**Q31 : How do you query with multiple tables (joins)?**
**A :**
```java
@Query("SELECT c FROM Course c JOIN c.reviews r WHERE r.rating >= :minRating")
List<Course> findCoursesWithHighRatings(@Param("minRating") int minRating);

// Or with fetch join (avoids N+1)
@Query("SELECT DISTINCT c FROM Course c LEFT JOIN FETCH c.reviews")
List<Course> findAllWithReviews();
```

**Q32 : What is the N+1 problem and how do you avoid it?**
**A :** The N+1 problem happens with Lazy Loading :
```java
List<Course> courses = courseRepository.findAll();  // 1 query
for (Course course : courses) {
    course.getReviews().size();  // N queries (1 per course)!
}
// Total: 1 + N queries
```

**Solutions :**
```java
// Fetch Join
@Query("SELECT c FROM Course c LEFT JOIN FETCH c.reviews")
List<Course> findAllWithReviews();  // 1 single query

// EntityGraph
@EntityGraph(attributePaths = {"reviews"})
List<Course> findAll();
```

**Q33 : How do you use @EntityGraph?**
**A :**
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

// Or inline
@EntityGraph(attributePaths = {"reviews", "author"})
Optional<Course> findById(Long id);
```

## 🐘 Hibernate

**Q34 : Is Hibernate required to use JPA?**
**A :** No, JPA is a specification that can be implemented by several providers : Hibernate (the most popular), EclipseLink, OpenJPA. Spring Boot uses Hibernate by default.

**Q35 : What are Hibernate's unique features (not in JPA)?**
**A :**
- Second-Level Cache
- Optimized Batch Processing
- Specific annotations (`@Formula`, `@CreationTimestamp`, `@UpdateTimestamp`)
- Custom ID generators
- Envers (audit history)

**Q36 : How do you enable Hibernate's Second-Level Cache?**
**A :**
```java
// Configuration
spring.jpa.properties.hibernate.cache.use_second_level_cache=true
spring.jpa.properties.hibernate.cache.region.factory_class=org.hibernate.cache.jcache.JCacheRegionFactory

// Entity
@Entity
@Cacheable
@org.hibernate.annotations.Cache(usage = CacheConcurrencyStrategy.READ_WRITE)
public class Course {
    // Cache shared between all sessions
}
```

**Q37 : What does spring.jpa.hibernate.ddl-auto mean?**
**A :**
- **none** : Nothing, schema managed manually
- **validate** : Validates that the schema matches the entities
- **update** : Updates the schema (adds columns/tables, does not delete)
- **create** : Drops and recreates the schema on each startup
- **create-drop** : Creates at startup, drops at shutdown

**⚠️ Production : always use `none` or `validate`!**

**Q38 : How do you display the SQL queries generated by Hibernate?**
**A :**
```properties
# Show SQL
spring.jpa.show-sql=true

# Format the SQL
spring.jpa.properties.hibernate.format_sql=true

# Log parameters
logging.level.org.hibernate.SQL=DEBUG
logging.level.org.hibernate.type.descriptor.sql.BasicBinder=TRACE
```

**Q39 : How do you optimize performance with Hibernate Batch Processing?**
**A :**
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
            entityManager.flush();  // Sends the batch
            entityManager.clear();  // Clear cache
        }
    }
}
```

**Q40 : What is Hibernate's @Formula annotation?**
**A :**
```java
@Entity
public class Course {
    @Id
    private Long id;
    
    // SQL calculation in the entity (no physical column)
    @Formula("(SELECT COUNT(*) FROM review r WHERE r.course_id = id)")
    private int reviewCount;
    
    @Formula("(SELECT AVG(r.rating) FROM review r WHERE r.course_id = id)")
    private Double averageRating;
}
```

**Q41 : What are @CreationTimestamp and @UpdateTimestamp?**
**A :**
```java
@Entity
public class Course {
    @org.hibernate.annotations.CreationTimestamp
    private LocalDateTime createdAt;  // Auto-filled on creation
    
    @org.hibernate.annotations.UpdateTimestamp
    private LocalDateTime updatedAt;  // Auto-updated on each modification
}
```
Hibernate alternative to JPA Auditing annotations.

## 📊 Auditing

**Q42 : What is JPA Auditing?**
**A :** JPA Auditing automatically tracks who created/modified an entity and when, without manual code. Uses annotations `@CreatedDate`, `@CreatedBy`, `@LastModifiedDate`, `@LastModifiedBy`.

**Q43 : How do you enable Auditing in Spring Boot?**
**A :**
```java
@Configuration
@EnableJpaAuditing
public class JpaConfig {
    
    @Bean
    public AuditorAware<String> auditorProvider() {
        return () -> {
            // Get the current user
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

**Q44 : What is @MappedSuperclass?**
**A :** `@MappedSuperclass` marks a class as a superclass for other entities. Its fields will be inherited and included in the tables of child entities. The class itself is NOT an entity (no table).

**Q45 : What is @EntityListeners(AuditingEntityListener.class)?**
**A :** Enables auditing for the entity. `AuditingEntityListener` is a Spring Data JPA class that automatically fills fields annotated with `@CreatedDate`, `@CreatedBy`, `@LastModifiedDate`, `@LastModifiedBy`.

**Q46 : Create a complete auditable base class**
**A :**
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

// Usage
@Entity
public class Course extends BaseEntity {
    @Id
    @GeneratedValue
    private Long id;
    
    private String name;
    // createdAt, createdBy, updatedAt, updatedBy inherited
}
```

**Q47 : What is the difference between updatable=false and insertable=false?**
**A :**
- **updatable=false** : Column cannot be modified after creation (for `createdAt`, `createdBy`)
- **insertable=false** : Column cannot be manually inserted (for `updatedAt`, `updatedBy`)

**Q48 : How do you implement AuditorAware with Spring Security?**
**A :**
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

**Q49 : Can you use a custom type for @CreatedBy instead of String?**
**A :** Yes :
```java
@MappedSuperclass
@EntityListeners(AuditingEntityListener.class)
public abstract class BaseEntity {
    
    @CreatedBy
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "created_by_user_id")
    private User createdBy;  // Custom type
    
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
        // Return the full User object
        return Optional.of(getCurrentUser());
    }
}
```

**Q50 : What is @Version and what is it used for?**
**A :** `@Version` implements Optimistic Locking. JPA automatically increments this field on each UPDATE and verifies that nobody else modified the entity in between :
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
WHERE id = ? AND version = ?  // If version changed → OptimisticLockException
```

## 📁 schema.sql and data.sql

**Q51 : What is schema.sql and what is it used for?**
**A :** `schema.sql` is a file placed in `src/main/resources/` containing SQL DDL commands (CREATE TABLE, ALTER, etc.). Spring Boot executes it automatically at startup to initialize the database schema.

**Q52 : What is the difference between schema.sql and data.sql?**
**A :**
- **schema.sql** : Database structure (CREATE TABLE, indexes, constraints)
- **data.sql** : Initial data (INSERT INTO)

**Q53 : Example of schema.sql to create tables**
**A :**
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

**Q54 : How do you enable schema.sql execution?**
**A :**
```properties
# application.properties

# Always execute schema.sql
spring.sql.init.mode=always

# Defer init after Hibernate (if needed)
spring.jpa.defer-datasource-initialization=true

# Disable Hibernate DDL
spring.jpa.hibernate.ddl-auto=none
```

**Q55 : When should you use schema.sql vs Hibernate DDL auto?**
**A :**
- **schema.sql** : Production, complex schema, full control, migrating existing DB
- **Hibernate DDL** : Development, prototyping, tests

**Production : Always use Flyway or Liquibase, never schema.sql or Hibernate DDL!**

**Q56 : What are Flyway and Liquibase?**
**A :** Versioned and incremental database migration tools :
- **Flyway** : Numbered SQL scripts (V1__Initial.sql, V2__Add_column.sql)
- **Liquibase** : XML/YAML/JSON changes

They track applied migrations and allow rollbacks.

**Q57 : How do you use Flyway with Spring Boot?**
**A :**
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
Spring Boot automatically runs the migrations at startup.

## 🗃️ H2 Console

**Q58 : What is H2 Database?**
**A :** H2 is a Java relational database :
- **In-memory** (fast, data lost on shutdown)
- **Or on file** (persistent)
- Built-in web console
- Perfect for development and tests
- Standard SQL compatible

**Q59 : How do you configure H2 in Spring Boot?**
**A :**
```properties
# application.properties

# Enable H2 Console
spring.h2.console.enabled=true
spring.h2.console.path=/h2-console

# H2 Configuration
spring.datasource.url=jdbc:h2:mem:testdb
spring.datasource.driverClassName=org.h2.Driver
spring.datasource.username=sa
spring.datasource.password=

# JPA
spring.jpa.database-platform=org.hibernate.dialect.H2Dialect
spring.jpa.show-sql=true
spring.jpa.hibernate.ddl-auto=create-drop
```

**Q60 : How do you access H2 Console?**
**A :**
```
URL: http://localhost:8080/h2-console

JDBC URL: jdbc:h2:mem:testdb
Username: sa
Password: (empty)
```

**Q61 : How do you persist H2 data to a file?**
**A :**
```properties
# In-memory (default, data lost)
spring.datasource.url=jdbc:h2:mem:testdb

# On file (persistent)
spring.datasource.url=jdbc:h2:file:./data/testdb

# File with absolute path
spring.datasource.url=jdbc:h2:file:/path/to/data/testdb
```

**Q62 : Why is using H2 in production not recommended?**
**A :** H2 is designed for development and tests, not production :
- Limited performance under load
- Fewer features than PostgreSQL/MySQL
- Not optimized for high concurrency
- Limited support and community
- Lack of professional tooling

**Production : PostgreSQL, MySQL, Oracle, SQL Server**

## 🔄 Transactions

**Q63 : What is a transaction and why is it important?**
**A :** A transaction is a unit of work that respects ACID :
- **Atomicity** : All or nothing (either all operations succeed, or none do)
- **Consistency** : Consistent database state
- **Isolation** : Transactions don't interfere with each other
- **Durability** : Changes are permanent after commit

**Q64 : How do you use @Transactional?**
**A :**
```java
@Service
public class CourseService {
    
    @Transactional  // Entire method in one transaction
    public void createCourseWithReviews(Course course, List<Review> reviews) {
        courseRepository.save(course);
        reviewRepository.saveAll(reviews);
        // If exception → automatic ROLLBACK
        // Otherwise → COMMIT at the end
    }
    
    @Transactional(readOnly = true)  // Optimization for read-only operations
    public Course findById(Long id) {
        return courseRepository.findById(id)
            .orElseThrow(() -> new NotFoundException("Course not found"));
    }
}
```

**Q65 : What happens without @Transactional?**
**A :** Without `@Transactional`, each repository operation is in its own transaction :
```java
public void createCourseWithReviews(Course course, List<Review> reviews) {
    courseRepository.save(course);  // Transaction 1 - COMMIT
    // Exception here!
    reviewRepository.saveAll(reviews);  // Never executed
    // Problem: course created but not reviews → inconsistent state
}
```

**Q66 : What are the transaction isolation levels?**
**A :**
```java
// From least to most isolated (and most to least performant)

@Transactional(isolation = Isolation.READ_UNCOMMITTED)
// Can read uncommitted data (dirty reads)

@Transactional(isolation = Isolation.READ_COMMITTED)  // Default
// Reads only committed data

@Transactional(isolation = Isolation.REPEATABLE_READ)
// Guarantees that re-reading the same data gives the same result

@Transactional(isolation = Isolation.SERIALIZABLE)
// Full isolation, sequential transactions
```

**Q67 : What is transaction propagation?**
**A :**
```java
@Transactional(propagation = Propagation.REQUIRED)  // Default
// Uses existing transaction or creates a new one

@Transactional(propagation = Propagation.REQUIRES_NEW)
// Suspends current transaction and creates a new one

@Transactional(propagation = Propagation.MANDATORY)
// Must be called within an existing transaction (otherwise exception)

@Transactional(propagation = Propagation.NEVER)
// Must NOT be called within a transaction (otherwise exception)

@Transactional(propagation = Propagation.SUPPORTS)
// Executes within a transaction if one exists, otherwise without
```

**Q68 : How do you handle rollbacks with @Transactional?**
**A :**
```java
// Default: rollback only for RuntimeException and Error
@Transactional
public void method() {
    throw new RuntimeException();  // ROLLBACK
}

// Rollback for checked exceptions too
@Transactional(rollbackFor = Exception.class)
public void method() throws Exception {
    throw new Exception();  // ROLLBACK
}

// Do not rollback for specific exceptions
@Transactional(noRollbackFor = SpecificException.class)
public void method() {
    throw new SpecificException();  // COMMIT anyway
}

// Manual rollback
@Transactional
public void method() {
    try {
        // code
    } catch (Exception e) {
        TransactionAspectSupport.currentTransactionStatus().setRollbackOnly();
    }
}
```

**Q69 : Why doesn't @Transactional work on private methods?**
**A :** `@Transactional` uses AOP proxies. Proxies can only intercept external calls to the class. Internal calls (private method or this.method()) bypass the proxy :
```java
@Service
public class CourseService {
    
    public void publicMethod() {
        this.transactionalMethod();  // ❌ No transaction!
        // The proxy is not used
    }
    
    @Transactional
    private void transactionalMethod() {
        // @Transactional ignored (private method)
    }
}

// Solution: make it public or inject self
@Service
public class CourseService {
    
    @Autowired
    private CourseService self;  // Proxy
    
    public void publicMethod() {
        self.transactionalMethod();  // ✅ Transaction active
    }
    
    @Transactional
    public void transactionalMethod() {
        // Transaction works
    }
}
```

## 🔍 Advanced JPA Relationships

**Q70 : How do you map a @OneToOne relationship?**
**A :**
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
    
    @OneToOne(mappedBy = "profile")  // Bidirectional
    private User user;
}
```

**Q71 : How do you map a @ManyToOne relationship?**
**A :**
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

**Q72 : How do you map a @ManyToMany relationship?**
**A :**
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

// Join table created automatically:
// student_course (student_id, course_id)
```

**Q73 : How do you add attributes to a @ManyToMany join table?**
**A :** Create a dedicated entity :
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
    
    private LocalDate enrollmentDate;  // Extra attribute
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

**Q74 : What is orphanRemoval?**
**A :** `orphanRemoval = true` automatically deletes child entities that are no longer referenced :
```java
@Entity
public class Course {
    @OneToMany(mappedBy = "course", 
               cascade = CascadeType.ALL, 
               orphanRemoval = true)
    private List<Review> reviews = new ArrayList<>();
}

// Usage
Course course = courseRepository.findById(1L);
course.getReviews().remove(0);  // Review removed from the list
courseRepository.save(course);
// Review automatically DELETED from DB (orphan removal)
```

**Q75 : What is the difference between CascadeType.REMOVE and orphanRemoval?**
**A :**
- **CascadeType.REMOVE** : Deletes children when the parent is deleted
- **orphanRemoval = true** : Deletes children when they are removed from the collection

```java
// CascadeType.REMOVE
courseRepository.delete(course);  // Reviews deleted
course.getReviews().remove(review);  // Review NOT deleted

// orphanRemoval = true
courseRepository.delete(course);  // Reviews deleted
course.getReviews().remove(review);  // Review deleted too
```

## 📊 Projections and DTOs

**Q76 : What is a JPA Projection?**
**A :** A Projection allows retrieving only certain fields from an entity instead of the whole entity :
```java
// Interface Projection
public interface CourseNameOnly {
    String getName();
    String getAuthor();
}

// Repository
public interface CourseRepository extends JpaRepository<Course, Long> {
    List<CourseNameOnly> findAllProjectedBy();
    // SELECT name, author FROM course (not all columns)
}

// Usage
List<CourseNameOnly> courses = repository.findAllProjectedBy();
courses.forEach(c -> System.out.println(c.getName()));
```

**Q77 : What is a DTO Projection?**
**A :** Using a DTO (Data Transfer Object) instead of an interface :
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

// Repository with @Query
@Query("SELECT new com.example.dto.CourseDTO(c.id, c.name) FROM Course c")
List<CourseDTO> findAllDTOs();

// Usage
List<CourseDTO> dtos = repository.findAllDTOs();
```

**Q78 : What is the difference between Interface Projection and DTO Projection?**
**A :**
- **Interface Projection** : Simpler, Spring dynamically generates the implementation
- **DTO Projection** : More control, can add business logic, serializable

## 🛠️ Optimizations and Best Practices

**Q79 : How do you avoid the N+1 problem with @BatchSize?**
**A :**
```java
@Entity
public class Course {
    @OneToMany(mappedBy = "course")
    @BatchSize(size = 10)  // Loads 10 relationships at a time
    private List<Review> reviews;
}

// Instead of N+1 queries, does 1 + (N/10) queries
```

**Q80 : When should you use FetchType.LAZY vs EAGER?**
**A :**
- **LAZY** (default for @OneToMany, @ManyToMany) : Preferred, loads on demand
- **EAGER** (default for @ManyToOne, @OneToOne) : Avoid, can cause N+1

**Best Practice : Always LAZY, use fetch join when needed**

**Q81 : How do you use @Query with pagination?**
**A :**
```java
@Query("SELECT c FROM Course c WHERE c.author = :author")
Page<Course> findByAuthorCustom(@Param("author") String author, Pageable pageable);

// Spring automatically adds LIMIT/OFFSET
// Also does a COUNT query for the total
```

**Q82 : How do you temporarily disable auditing for certain operations?**
**A :**
```java
@Service
public class BulkImportService {
    
    @PersistenceContext
    private EntityManager entityManager;
    
    @Transactional
    public void bulkImport(List<Course> courses) {
        // Temporarily disable auditing for performance
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

**Q83 : How do you handle soft deletes (logical deletion)?**
**A :**
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

// Or with standard JPA
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

**Q84 : How do you implement full auditing with Hibernate Envers?**
**A :**
```xml
<!-- pom.xml -->
<dependency>
    <groupId>org.hibernate</groupId>
    <artifactId>hibernate-envers</artifactId>
</dependency>
```
```java
@Entity
@Audited  // Enables full audit (history of all modifications)
public class Course {
    @Id
    private Long id;
    
    private String name;
    private String author;
}

// Tables automatically created:
// - course (normal table)
// - course_AUD (history with REV and REVTYPE)
// - REVINFO (revision information)

// Retrieve history
AuditReader reader = AuditReaderFactory.get(entityManager);

// All revisions
List<Number> revisions = reader.getRevisions(Course.class, courseId);

// Entity at a specific revision
Course courseAtRevision = reader.find(Course.class, courseId, revisionNumber);

// Full history
List<Course> history = reader.createQuery()
    .forRevisionsOfEntity(Course.class, false, true)
    .add(AuditEntity.id().eq(courseId))
    .getResultList();
```

**Q85 : How do you use @QueryHints to optimize performance?**
**A :**
```java
@QueryHints(@QueryHint(name = "org.hibernate.cacheable", value = "true"))
@Query("SELECT c FROM Course c WHERE c.author = :author")
List<Course> findByAuthorCacheable(@Param("author") String author);

// Other useful hints
@QueryHints({
    @QueryHint(name = "org.hibernate.fetchSize", value = "50"),
    @QueryHint(name = "org.hibernate.readOnly", value = "true")
})
List<Course> findAllOptimized();
```

## 🎯 Practical Scenarios

**Q86 : How do you implement a full-text search system?**
**A :**
```java
// Option 1: LIKE (basic)
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

// Full-text search
List<Course> results = Search.session(entityManager)
    .search(Course.class)
    .where(f -> f.match()
        .fields("name", "author")
        .matching(query))
    .fetchHits(20);
```

**Q87 : How do you implement a Many-to-Many tag system with attributes?**
**A :**
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

**Q88 : How do you implement a hierarchical comment system (parent/child)?**
**A :**
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
    // Only root comments (without parent)
    List<Comment> findByParentIsNull();
    
    // With fetch join to avoid N+1
    @Query("SELECT c FROM Comment c LEFT JOIN FETCH c.replies WHERE c.parent IS NULL")
    List<Comment> findRootCommentsWithReplies();
}
```

**Q89 : How do you implement a view counter with optimistic locking?**
**A :**
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
                return;  // Success
            } catch (OptimisticLockException e) {
                attempt++;
                if (attempt >= maxRetries) {
                    throw e;
                }
                // Retry with backoff
                Thread.sleep(100 * attempt);
            }
        }
    }
}
```

**Q90 : How do you create an entity with a composite key?**
**A :**
```java
// Composite key class
@Embeddable
public class EnrollmentId implements Serializable {
    private Long studentId;
    private Long courseId;
    
    // Constructors, equals, hashCode
}

// Entity with composite key
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

// Alternative with @IdClass
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
