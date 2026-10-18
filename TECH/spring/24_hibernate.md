# 🍃 Cours Complet Hibernate — Du Débutant à l'Expert

> Tutoriel détaillé avec exemples pratiques, en français.  
> Stack : Java 17+, Spring Boot 3, Hibernate 6, PostgreSQL

---

## Table des matières

1. [Introduction — C'est quoi Hibernate ?](#1-introduction)
2. [Configuration et Setup](#2-configuration)
3. [Première entité — Les annotations de base](#3-première-entité)
4. [Les relations entre entités](#4-relations)
5. [LAZY vs EAGER Fetching](#5-lazy-vs-eager)
6. [Le cycle de vie d'une entité](#6-cycle-de-vie)
7. [JPQL — Java Persistence Query Language](#7-jpql)
8. [Criteria API](#8-criteria-api)
9. [Spring Data JPA & Repositories](#9-spring-data-jpa)
10. [Les Caches — First & Second Level](#10-les-caches)
11. [Les Transactions](#11-les-transactions)
12. [Héritage — Inheritance Strategies](#12-héritage)
13. [Types avancés — @Embeddable, @Converter](#13-types-avancés)
14. [Optimistic & Pessimistic Locking](#14-locking)
15. [Performance & Bonnes Pratiques](#15-performance)
16. [Les pièges classiques](#16-pièges)

---

## 1. Introduction

### C'est quoi un ORM ?

Sans ORM, pour sauvegarder un objet Java en base, on écrit du JDBC à la main :

```java
// Sans ORM — boilerplate horrible
Connection conn = DriverManager.getConnection(url, user, pass);
PreparedStatement ps = conn.prepareStatement(
    "INSERT INTO author (name, email) VALUES (?, ?)"
);
ps.setString(1, author.getName());
ps.setString(2, author.getEmail());
ps.executeUpdate();
```

Avec Hibernate :

```java
// Avec Hibernate — 1 ligne
entityManager.persist(author);
```

### JPA vs Hibernate

```
┌─────────────────────────────────────────────────────┐
│                   Ton Application                    │
├─────────────────────────────────────────────────────┤
│            Spring Data JPA (surcouche)               │
├─────────────────────────────────────────────────────┤
│         JPA (specification — interfaces)             │
├─────────────────────────────────────────────────────┤
│      Hibernate (implémentation de JPA)               │
├─────────────────────────────────────────────────────┤
│         JDBC (connexion bas niveau)                  │
├─────────────────────────────────────────────────────┤
│         Base de données (PostgreSQL, MySQL...)        │
└─────────────────────────────────────────────────────┘
```

- **JPA** = contrat/interface (comme une interface Java)
- **Hibernate** = implémentation de ce contrat (comme la classe concrète)
- **Spring Data JPA** = couche de commodité au-dessus de JPA

---

## 2. Configuration

### Dépendances Maven (pom.xml)

```xml
<dependencies>
    <!-- Spring Boot Starter Data JPA (inclut Hibernate) -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-data-jpa</artifactId>
    </dependency>

    <!-- Base de données PostgreSQL -->
    <dependency>
        <groupId>org.postgresql</groupId>
        <artifactId>postgresql</artifactId>
        <scope>runtime</scope>
    </dependency>

    <!-- Pour les tests : base H2 en mémoire -->
    <dependency>
        <groupId>com.h2database</groupId>
        <artifactId>h2</artifactId>
        <scope>test</scope>
    </dependency>

    <!-- Lombok pour réduire le boilerplate -->
    <dependency>
        <groupId>org.projectlombok</groupId>
        <artifactId>lombok</artifactId>
        <optional>true</optional>
    </dependency>
</dependencies>
```

### application.properties

```properties
# ── Connexion à la base ──────────────────────────────────────
spring.datasource.url=jdbc:postgresql://localhost:5432/mydb
spring.datasource.username=postgres
spring.datasource.password=secret
spring.datasource.driver-class-name=org.postgresql.Driver

# ── Hibernate ────────────────────────────────────────────────
# validate  → vérifie que le schéma correspond aux entités (recommandé en PROD)
# update    → met à jour le schéma automatiquement (pratique en DEV)
# create    → recrée le schéma à chaque démarrage (tests uniquement !)
# create-drop → crée puis supprime à l'arrêt (tests)
# none      → ne touche pas au schéma
spring.jpa.hibernate.ddl-auto=update

# Afficher les requêtes SQL dans les logs (DEV uniquement)
spring.jpa.show-sql=true
spring.jpa.properties.hibernate.format_sql=true

# Dialecte — généralement auto-détecté avec Spring Boot 3
# spring.jpa.properties.hibernate.dialect=org.hibernate.dialect.PostgreSQLDialect

# Statistiques de performance (optionnel)
spring.jpa.properties.hibernate.generate_statistics=true
```

### Structure du projet

```
src/
├── main/
│   ├── java/com/example/
│   │   ├── entity/          ← Les entités JPA
│   │   ├── repository/      ← Les repositories Spring Data JPA
│   │   ├── service/         ← La logique métier (@Transactional ici)
│   │   └── dto/             ← Les Data Transfer Objects
│   └── resources/
│       └── application.properties
└── test/
```

---

## 3. Première entité

### Entité simple

```java
package com.example.entity;

import jakarta.persistence.*;
import lombok.*;
import java.time.LocalDate;

@Entity                          // ← Marque la classe comme entité JPA
@Table(name = "authors")         // ← Nom de la table (optionnel si même nom)
@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@ToString(exclude = "books")     // ← Eviter les boucles infinies avec les relations
public class Author {

    @Id                                              // ← Clé primaire
    @GeneratedValue(strategy = GenerationType.IDENTITY)  // ← Auto-increment
    private Long id;

    @Column(name = "first_name", nullable = false, length = 100)
    private String firstName;

    @Column(name = "last_name", nullable = false, length = 100)
    private String lastName;

    @Column(unique = true, nullable = false)
    private String email;

    @Column(name = "birth_date")
    private LocalDate birthDate;

    @Column(columnDefinition = "TEXT")   // ← Type SQL explicite
    private String bio;

    @Transient   // ← Ce champ n'est PAS sauvegardé en base
    private String fullName;

    // Méthode utilitaire
    public String getFullName() {
        return firstName + " " + lastName;
    }
}
```

### Les stratégies de génération d'ID

```java
// 1. IDENTITY — délègue à la base (AUTO_INCREMENT MySQL, SERIAL PostgreSQL)
@GeneratedValue(strategy = GenerationType.IDENTITY)

// 2. SEQUENCE — utilise une séquence SQL (recommandé avec PostgreSQL)
@GeneratedValue(strategy = GenerationType.SEQUENCE, generator = "author_seq")
@SequenceGenerator(
    name = "author_seq",
    sequenceName = "author_sequence",
    allocationSize = 50   // ← Hibernate pré-alloue 50 IDs → moins de requêtes
)

// 3. AUTO — Hibernate choisit selon la base
@GeneratedValue(strategy = GenerationType.AUTO)

// 4. UUID — très utile pour les APIs publiques (pas besoin de la base pour générer)
@Id
@GeneratedValue(strategy = GenerationType.UUID)
private UUID id;

// 5. ID assigné manuellement (pas de @GeneratedValue)
@Id
private String isoCode;   // Ex: "FR", "US" pour un pays
```

### Equals et HashCode — Attention !

C'est un sujet critique avec Hibernate. L'ID auto-généré est `null` avant le premier `persist()`.

```java
@Entity
public class Author {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    // ❌ MAUVAIS — utilise l'id qui peut être null
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Author)) return false;
        Author author = (Author) o;
        return Objects.equals(id, author.id); // null == null → problème !
    }

    // ✅ BON — utilise un identifiant naturel ou UUID
    // Option 1 : utiliser l'email comme clé naturelle
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Author)) return false;
        Author author = (Author) o;
        return email != null && email.equals(author.email);
    }

    // Option 2 : utiliser un UUID généré à la construction
    @Column(nullable = false, updatable = false)
    private UUID uuid = UUID.randomUUID();   // Généré côté Java, toujours non-null

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Author)) return false;
        Author author = (Author) o;
        return uuid.equals(author.uuid);
    }

    @Override
    public int hashCode() {
        return uuid.hashCode();
    }
}
```

---

## 4. Relations

### @ManyToOne — La relation la plus courante

```java
@Entity
@Table(name = "books")
@Getter @Setter @NoArgsConstructor
public class Book {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Column(nullable = false)
    private String title;

    private String isbn;

    @Column(name = "published_date")
    private LocalDate publishedDate;

    // Relation Many-to-One : plusieurs livres → un auteur
    @ManyToOne(fetch = FetchType.LAZY)  // ← Toujours LAZY sur ManyToOne
    @JoinColumn(name = "author_id", nullable = false)  // ← Colonne FK dans la table books
    private Author author;
}
```

### @OneToMany — La collection

```java
@Entity
public class Author {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    // ...

    // Relation One-to-Many : un auteur → plusieurs livres
    @OneToMany(
        mappedBy = "author",           // ← "author" = le champ dans Book qui pointe ici
        cascade = CascadeType.ALL,     // ← Les opérations se propagent aux livres
        orphanRemoval = true,          // ← Si on retire un livre de la liste → DELETE en base
        fetch = FetchType.LAZY         // ← Par défaut pour OneToMany, mais bon de l'expliciter
    )
    private List<Book> books = new ArrayList<>();  // ← Toujours initialiser !

    // ✅ Méthodes helper pour maintenir la cohérence des deux côtés
    public void addBook(Book book) {
        books.add(book);
        book.setAuthor(this);   // ← On met à jour les DEUX côtés
    }

    public void removeBook(Book book) {
        books.remove(book);
        book.setAuthor(null);
    }
}
```

> **Règle d'or** : Dans une relation bidirectionnelle, toujours maintenir les **deux côtés** de la relation avec des méthodes helper.

### @OneToOne

```java
@Entity
public class User {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    private String username;

    // Côté propriétaire (possède la FK)
    @OneToOne(cascade = CascadeType.ALL, fetch = FetchType.LAZY)
    @JoinColumn(name = "profile_id", referencedColumnName = "id")
    private UserProfile profile;
}

@Entity
public class UserProfile {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    private String avatarUrl;
    private String bio;

    // Côté inverse (mappedBy pointe vers le champ dans User)
    @OneToOne(mappedBy = "profile")
    private User user;
}
```

### @ManyToMany

```java
@Entity
public class Student {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    private String name;

    @ManyToMany(cascade = {CascadeType.PERSIST, CascadeType.MERGE})
    // ❌ Jamais CascadeType.REMOVE sur ManyToMany !
    @JoinTable(
        name = "student_course",           // ← Nom de la table de jointure
        joinColumns = @JoinColumn(name = "student_id"),
        inverseJoinColumns = @JoinColumn(name = "course_id")
    )
    private Set<Course> courses = new HashSet<>();  // ← Set plutôt que List pour ManyToMany

    public void enrollIn(Course course) {
        courses.add(course);
        course.getStudents().add(this);
    }
}

@Entity
public class Course {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    private String title;

    @ManyToMany(mappedBy = "courses")   // ← mappedBy = côté inverse
    private Set<Student> students = new HashSet<>();
}
```

### @ManyToMany avec attributs sur la relation

Quand la table de jointure a des colonnes en plus (ex: date d'inscription) :

```java
// On crée une entité intermédiaire
@Entity
@Table(name = "enrollment")
public class Enrollment {

    @EmbeddedId
    private EnrollmentId id = new EnrollmentId();

    @ManyToOne(fetch = FetchType.LAZY)
    @MapsId("studentId")
    private Student student;

    @ManyToOne(fetch = FetchType.LAZY)
    @MapsId("courseId")
    private Course course;

    @Column(name = "enrolled_at")
    private LocalDateTime enrolledAt = LocalDateTime.now();

    private String grade;
}

@Embeddable
public class EnrollmentId implements Serializable {
    private Long studentId;
    private Long courseId;
    // equals() et hashCode() obligatoires !
}
```

---

## 5. LAZY vs EAGER

### Explication visuelle

```
Base de données :
┌──────────────────────┐        ┌──────────────────────┐
│  Author (id=1)       │──────→ │  Books               │
│  name: "Victor Hugo" │        │  - Les Misérables    │
└──────────────────────┘        │  - Notre-Dame...     │
                                └──────────────────────┘

Avec EAGER :
→ SELECT * FROM authors WHERE id = 1
→ SELECT * FROM books WHERE author_id = 1   ← Immédiatement, même si on n'a pas besoin

Avec LAZY :
→ SELECT * FROM authors WHERE id = 1        ← Seulement ça
→ (rien de plus jusqu'à ce qu'on accède à author.getBooks())
```

### Comportement par défaut

| Annotation      | FetchType par défaut |
|-----------------|----------------------|
| `@ManyToOne`    | `EAGER` ⚠️           |
| `@OneToOne`     | `EAGER` ⚠️           |
| `@OneToMany`    | `LAZY` ✅            |
| `@ManyToMany`   | `LAZY` ✅            |

> **Bonne pratique** : Toujours mettre `fetch = FetchType.LAZY` partout, y compris sur `@ManyToOne` et `@OneToOne`. On charge les données explicitement quand on en a besoin.

### Comment charger les données LAZY quand on en a besoin ?

```java
// Option 1 : JOIN FETCH dans JPQL
@Query("SELECT a FROM Author a JOIN FETCH a.books WHERE a.id = :id")
Optional<Author> findByIdWithBooks(@Param("id") Long id);

// Option 2 : @EntityGraph
@EntityGraph(attributePaths = {"books", "books.categories"})
@Query("SELECT a FROM Author a WHERE a.id = :id")
Optional<Author> findByIdWithBooksAndCategories(@Param("id") Long id);

// Option 3 : EntityGraph dynamique dans le service
EntityGraph<Author> graph = entityManager.createEntityGraph(Author.class);
graph.addAttributeNodes("books");
Map<String, Object> hints = new HashMap<>();
hints.put("jakarta.persistence.fetchgraph", graph);
Author author = entityManager.find(Author.class, id, hints);
```

---

## 6. Cycle de vie d'une entité

### Les 4 états

```
new Author()
     │
     │  entityManager.persist()
     ▼
 ┌─────────┐      entityManager.detach()    ┌──────────┐
 │ MANAGED │ ─────────────────────────────→ │ DETACHED │
 │(Tracked)│ ←───────────────────────────── │          │
 └─────────┘      entityManager.merge()     └──────────┘
     │
     │  entityManager.remove()
     ▼
 ┌─────────┐
 │ REMOVED │ ─── commit ──→ DELETE en base
 └─────────┘

État TRANSIENT = new Author() avant tout persist
```

### Démonstration en code

```java
@Service
@Transactional
public class AuthorService {

    @Autowired
    private EntityManager em;

    public void demonstrateCycle() {

        // ── État TRANSIENT ──────────────────────────────
        Author author = new Author();
        author.setFirstName("Victor");
        author.setLastName("Hugo");
        // Hibernate ne connaît pas cet objet, rien en base

        // ── État PERSISTENT / MANAGED ───────────────────
        em.persist(author);
        // Hibernate track cet objet, l'ID est généré
        // INSERT en base au flush/commit

        author.setEmail("victor@hugo.fr");
        // Pas besoin de save() ! Hibernate détecte le changement
        // → UPDATE automatique au flush (dirty checking)

        // ── État DETACHED ───────────────────────────────
        em.detach(author);
        author.setBio("Auteur romantique");
        // Ce changement NE sera PAS sauvegardé !

        // Pour ré-attacher :
        Author managedAuthor = em.merge(author);
        // managedAuthor est MANAGED, author reste DETACHED

        // ── État REMOVED ────────────────────────────────
        em.remove(managedAuthor);
        // DELETE en base au commit
    }
}
```

### flush() et clear() — Cas d'usage

```java
@Transactional
public void importLargeFile(List<AuthorDTO> data) {
    int batchSize = 50;

    for (int i = 0; i < data.size(); i++) {
        Author author = toEntity(data.get(i));
        em.persist(author);

        if (i % batchSize == 0) {
            em.flush();   // ← Envoie les INSERT en base
            em.clear();   // ← Libère la mémoire du contexte de persistance
            // Sans ça, tous les objets s'accumulent en mémoire → OutOfMemoryError !
        }
    }
}
```

---

## 7. JPQL

### Syntaxe de base

```java
// JPQL opère sur les ENTITÉS et leurs CHAMPS Java (pas sur les tables SQL)
// SELECT [entité/champ] FROM [Entité] [alias] WHERE [condition]

// ── Sélection simple ────────────────────────────────────────
TypedQuery<Author> query = em.createQuery(
    "SELECT a FROM Author a", Author.class
);
List<Author> authors = query.getResultList();

// ── WHERE ───────────────────────────────────────────────────
TypedQuery<Author> query = em.createQuery(
    "SELECT a FROM Author a WHERE a.lastName = :lastName", Author.class
);
query.setParameter("lastName", "Hugo");
Author author = query.getSingleResult();

// ── JOIN FETCH — charge les relations en une seule requête ──
List<Author> authors = em.createQuery(
    "SELECT DISTINCT a FROM Author a " +
    "JOIN FETCH a.books b " +
    "WHERE b.publishedDate > :date",
    Author.class
)
.setParameter("date", LocalDate.of(2000, 1, 1))
.getResultList();
// DISTINCT évite les doublons quand un auteur a plusieurs livres

// ── Tri et pagination ────────────────────────────────────────
List<Author> authors = em.createQuery(
    "SELECT a FROM Author a ORDER BY a.lastName ASC, a.firstName ASC",
    Author.class
)
.setFirstResult(0)    // ← OFFSET
.setMaxResults(10)    // ← LIMIT
.getResultList();
```

### Projections — Sélectionner des champs spécifiques

```java
// ── Projection vers un DTO ───────────────────────────────────
// Le DTO doit avoir un constructeur avec les mêmes paramètres
public record AuthorSummaryDTO(Long id, String firstName, String lastName) {}

List<AuthorSummaryDTO> summaries = em.createQuery(
    "SELECT new com.example.dto.AuthorSummaryDTO(a.id, a.firstName, a.lastName) " +
    "FROM Author a",
    AuthorSummaryDTO.class
).getResultList();
// Beaucoup plus performant : SELECT seulement 3 colonnes au lieu de toute la table

// ── Projection vers Object[] ─────────────────────────────────
List<Object[]> results = em.createQuery(
    "SELECT a.firstName, COUNT(b) FROM Author a LEFT JOIN a.books b GROUP BY a.id, a.firstName"
).getResultList();

for (Object[] row : results) {
    String name = (String) row[0];
    Long count  = (Long) row[1];
}
```

### Requêtes de mise à jour et suppression en masse

```java
// UPDATE en masse — évite de charger les entités
int updated = em.createQuery(
    "UPDATE Book b SET b.price = b.price * 0.9 WHERE b.publishedDate < :date"
)
.setParameter("date", LocalDate.of(2010, 1, 1))
.executeUpdate();
// Attention : bypasse le dirty checking et les cascades !

// DELETE en masse
int deleted = em.createQuery(
    "DELETE FROM Book b WHERE b.author.id = :authorId"
)
.setParameter("authorId", 42L)
.executeUpdate();
```

### Fonctions utiles en JPQL

```java
// Fonctions de chaîne
"SELECT a FROM Author a WHERE LOWER(a.lastName) = LOWER(:name)"
"SELECT a FROM Author a WHERE a.email LIKE :pattern"   // pattern = "%@gmail.com"

// Fonctions de date
"SELECT b FROM Book b WHERE YEAR(b.publishedDate) = :year"

// Fonctions d'agrégation
"SELECT COUNT(a), AVG(b.price), MAX(b.price), MIN(b.price), SUM(b.price) FROM ..."

// IS NULL / IS NOT NULL
"SELECT a FROM Author a WHERE a.bio IS NOT NULL"

// IN
"SELECT a FROM Author a WHERE a.id IN :ids"
query.setParameter("ids", List.of(1L, 2L, 3L));

// BETWEEN
"SELECT b FROM Book b WHERE b.price BETWEEN :min AND :max"

// EXISTS
"SELECT a FROM Author a WHERE EXISTS (SELECT b FROM Book b WHERE b.author = a)"
```

---

## 8. Criteria API

La Criteria API est utile pour les requêtes **dynamiques** où les filtres sont optionnels.

```java
@Service
public class BookSearchService {

    @Autowired
    private EntityManager em;

    public List<Book> search(String title, Long authorId, LocalDate fromDate, Double maxPrice) {

        CriteriaBuilder cb = em.getCriteriaBuilder();
        CriteriaQuery<Book> cq = cb.createQuery(Book.class);
        Root<Book> book = cq.from(Book.class);

        // Jointure pour filtrer par auteur
        Join<Book, Author> author = book.join("author", JoinType.LEFT);

        // Construire les prédicats dynamiquement
        List<Predicate> predicates = new ArrayList<>();

        if (title != null && !title.isBlank()) {
            predicates.add(
                cb.like(cb.lower(book.get("title")), "%" + title.toLowerCase() + "%")
            );
        }

        if (authorId != null) {
            predicates.add(cb.equal(author.get("id"), authorId));
        }

        if (fromDate != null) {
            predicates.add(cb.greaterThanOrEqualTo(book.get("publishedDate"), fromDate));
        }

        if (maxPrice != null) {
            predicates.add(cb.lessThanOrEqualTo(book.get("price"), maxPrice));
        }

        // Assembler la requête
        cq.where(predicates.toArray(new Predicate[0]));
        cq.orderBy(cb.asc(book.get("title")));

        return em.createQuery(cq).getResultList();
    }
}
```

### Spring Data JPA Specification — Plus élégant

```java
// Interface Specification — chaque spec est un filtre réutilisable
public class BookSpecifications {

    public static Specification<Book> hasTitle(String title) {
        return (root, query, cb) ->
            title == null ? null :
            cb.like(cb.lower(root.get("title")), "%" + title.toLowerCase() + "%");
    }

    public static Specification<Book> hasAuthor(Long authorId) {
        return (root, query, cb) -> {
            if (authorId == null) return null;
            Join<Book, Author> author = root.join("author");
            return cb.equal(author.get("id"), authorId);
        };
    }

    public static Specification<Book> publishedAfter(LocalDate date) {
        return (root, query, cb) ->
            date == null ? null :
            cb.greaterThanOrEqualTo(root.get("publishedDate"), date);
    }

    public static Specification<Book> maxPrice(Double price) {
        return (root, query, cb) ->
            price == null ? null :
            cb.lessThanOrEqualTo(root.get("price"), price);
    }
}

// Repository doit étendre JpaSpecificationExecutor
public interface BookRepository extends JpaRepository<Book, Long>,
                                         JpaSpecificationExecutor<Book> {}

// Utilisation dans le service — très lisible !
@Service
public class BookService {

    @Autowired
    private BookRepository bookRepository;

    public Page<Book> search(String title, Long authorId, LocalDate fromDate,
                              Double maxPrice, Pageable pageable) {
        Specification<Book> spec = Specification
            .where(hasTitle(title))
            .and(hasAuthor(authorId))
            .and(publishedAfter(fromDate))
            .and(maxPrice(maxPrice));

        return bookRepository.findAll(spec, pageable);
    }
}
```

---

## 9. Spring Data JPA & Repositories

### Les méthodes de base

```java
public interface AuthorRepository extends JpaRepository<Author, Long> {
    // JpaRepository fournit déjà :
    // save(), saveAll(), findById(), findAll(), deleteById(), delete(),
    // count(), existsById(), findAllById(), flush(), saveAndFlush()...
}
```

### Query Methods — Requêtes déduites du nom de la méthode

```java
public interface BookRepository extends JpaRepository<Book, Long> {

    // ── Trouver par un champ ────────────────────────────────
    List<Book> findByTitle(String title);
    Optional<Book> findByIsbn(String isbn);

    // ── Conditions ─────────────────────────────────────────
    List<Book> findByPriceLessThan(Double price);
    List<Book> findByPriceBetween(Double min, Double max);
    List<Book> findByTitleContainingIgnoreCase(String keyword);
    List<Book> findByPublishedDateAfter(LocalDate date);
    List<Book> findByAuthorIsNull();
    List<Book> findByTitleStartingWith(String prefix);

    // ── Combinaisons ────────────────────────────────────────
    List<Book> findByAuthor_LastNameAndPublishedDateAfter(String lastName, LocalDate date);
    // Author_ → navigation dans la relation

    // ── Tri ─────────────────────────────────────────────────
    List<Book> findByAuthorIdOrderByPublishedDateDesc(Long authorId);

    // ── Limiter le résultat ──────────────────────────────────
    List<Book> findTop5ByOrderByPublishedDateDesc();   // 5 plus récents
    Optional<Book> findFirstByTitleContaining(String keyword);

    // ── Exister / Compter ────────────────────────────────────
    boolean existsByIsbn(String isbn);
    long countByAuthorId(Long authorId);

    // ── Supprimer ────────────────────────────────────────────
    void deleteByAuthorId(Long authorId);
    long deleteByPublishedDateBefore(LocalDate date);
}
```

### @Query — Requêtes personnalisées

```java
public interface AuthorRepository extends JpaRepository<Author, Long> {

    // ── JPQL ────────────────────────────────────────────────
    @Query("SELECT a FROM Author a WHERE a.email = :email")
    Optional<Author> findByEmail(@Param("email") String email);

    // ── JPQL avec JOIN FETCH ─────────────────────────────────
    @Query("SELECT DISTINCT a FROM Author a JOIN FETCH a.books WHERE a.id = :id")
    Optional<Author> findByIdWithBooks(@Param("id") Long id);

    // ── Native SQL ──────────────────────────────────────────
    @Query(value = "SELECT * FROM authors WHERE email ILIKE :pattern", nativeQuery = true)
    List<Author> findByEmailPattern(@Param("pattern") String pattern);

    // ── Pagination avec @Query ────────────────────────────────
    @Query(
        value = "SELECT a FROM Author a WHERE a.firstName LIKE :prefix%",
        countQuery = "SELECT COUNT(a) FROM Author a WHERE a.firstName LIKE :prefix%"
    )
    Page<Author> findByFirstNameStartingWith(@Param("prefix") String prefix, Pageable pageable);

    // ── UPDATE avec @Modifying ────────────────────────────────
    @Modifying
    @Transactional   // ← Obligatoire pour les modifications
    @Query("UPDATE Author a SET a.email = :email WHERE a.id = :id")
    int updateEmail(@Param("id") Long id, @Param("email") String email);

    // ── Projection via interface ──────────────────────────────
    List<AuthorNameOnly> findAllByOrderByLastNameAsc();
}

// Interface de projection — Spring Data génère le proxy automatiquement
public interface AuthorNameOnly {
    String getFirstName();
    String getLastName();

    // Méthode calculée avec @Value
    @Value("#{target.firstName + ' ' + target.lastName}")
    String getFullName();
}
```

### Pagination et Tri

```java
// Dans le contrôleur ou le service
Pageable pageable = PageRequest.of(
    0,                          // ← Page (0-indexed)
    10,                         // ← Taille de la page
    Sort.by("lastName").ascending()
         .and(Sort.by("firstName").ascending())
);

Page<Author> page = authorRepository.findAll(pageable);

// Infos utiles sur la page
page.getContent();           // → List<Author> les éléments
page.getTotalElements();     // → nombre total d'auteurs en base
page.getTotalPages();        // → nombre total de pages
page.getNumber();            // → numéro de la page courante
page.isFirst();              // → true si première page
page.isLast();               // → true si dernière page
page.hasNext();              // → true s'il y a une page suivante
```

---

## 10. Les Caches

### First Level Cache — Automatique

```java
@Transactional
public void demonstrateFirstLevelCache() {

    // ── Première requête → SELECT en base ───────────────────
    Author a1 = authorRepository.findById(1L).get();
    // SQL: SELECT * FROM authors WHERE id = 1

    // ── Deuxième requête → depuis le cache, pas de SQL ! ────
    Author a2 = authorRepository.findById(1L).get();
    // Pas de SQL ! Retourné depuis le first level cache

    System.out.println(a1 == a2);  // true ! Même objet en mémoire

    // ── Après la fin de la transaction, le cache est vidé ───
}

// Ici le cache est vide, une nouvelle requête SQL sera faite
```

### Second Level Cache — À configurer

```xml
<!-- pom.xml : ajouter le provider de cache -->
<dependency>
    <groupId>org.hibernate.orm</groupId>
    <artifactId>hibernate-jcache</artifactId>
</dependency>
<dependency>
    <groupId>org.ehcache</groupId>
    <artifactId>ehcache</artifactId>
</dependency>
```

```properties
# application.properties
spring.jpa.properties.hibernate.cache.use_second_level_cache=true
spring.jpa.properties.hibernate.cache.region.factory_class=org.hibernate.cache.jcache.JCacheRegionFactory
spring.jpa.properties.javax.cache.provider=org.ehcache.jsr107.EhcacheCachingProvider
```

```java
@Entity
@Cache(usage = CacheConcurrencyStrategy.READ_WRITE)  // ← Activer le L2 cache pour cette entité
public class Country {

    @Id
    private String isoCode;  // "FR", "US", etc.

    private String name;
    // Les pays ne changent presque jamais → parfait pour le cache
}

// Les collections peuvent aussi être cachées
@Entity
public class Author {

    @OneToMany(mappedBy = "author")
    @Cache(usage = CacheConcurrencyStrategy.READ_WRITE)  // ← Cache sur la collection
    private List<Book> books;
}
```

#### Stratégies de cache

| Stratégie | Usage |
|-----------|-------|
| `READ_ONLY` | Données jamais modifiées (pays, devises). Le plus performant |
| `READ_WRITE` | Données modifiables, cohérence soft. Usage courant |
| `NONSTRICT_READ_WRITE` | Cohérence moins stricte, légèrement plus performant |
| `TRANSACTIONAL` | Cohérence totale, nécessite un cache transactionnel |

### Query Cache

```java
// Cacher le résultat d'une requête JPQL
@QueryHints(@QueryHint(name = "org.hibernate.cacheable", value = "true"))
@Query("SELECT c FROM Country c ORDER BY c.name")
List<Country> findAllCached();
```

---

## 11. Les Transactions

### @Transactional — Le plus important

```java
@Service
public class BookService {

    @Autowired
    private BookRepository bookRepository;

    @Autowired
    private AuthorRepository authorRepository;

    // ── Transaction standard ─────────────────────────────────
    @Transactional
    public Book createBook(BookCreateDTO dto) {
        Author author = authorRepository.findById(dto.getAuthorId())
            .orElseThrow(() -> new EntityNotFoundException("Author not found"));

        Book book = new Book();
        book.setTitle(dto.getTitle());
        book.setAuthor(author);

        return bookRepository.save(book);
        // Si une exception est levée ici → ROLLBACK automatique
    }

    // ── Transaction en lecture seule — optimisation ──────────
    @Transactional(readOnly = true)
    public List<Book> findAll() {
        return bookRepository.findAll();
        // Hibernate désactive le dirty checking → plus léger
    }

    // ── Propagation ─────────────────────────────────────────
    @Transactional(propagation = Propagation.REQUIRES_NEW)
    public void auditLog(String action) {
        // Toujours dans une NOUVELLE transaction
        // Même si appelé depuis une méthode déjà transactionnelle
        // Utile pour les logs : ils ne sont pas rollbackés si la transaction principale échoue
    }

    // ── Rollback sur exception checked ──────────────────────
    @Transactional(rollbackFor = Exception.class)
    public void riskyOperation() throws Exception {
        // Par défaut, rollback uniquement sur RuntimeException
        // rollbackFor = Exception.class → rollback aussi sur les exceptions checked
    }

    // ── Pas de rollback sur certaines exceptions ─────────────
    @Transactional(noRollbackFor = BusinessWarningException.class)
    public void operationWithWarning() {
        // BusinessWarningException ne déclenche pas de rollback
    }
}
```

### Propagation — Les cas courants

```
REQUIRED      → Utilise la transaction existante, ou en crée une (défaut)
REQUIRES_NEW  → Crée toujours une nouvelle transaction (suspend l'existante)
SUPPORTS      → Utilise la transaction si elle existe, sinon sans transaction
NOT_SUPPORTED → Suspend la transaction existante, s'exécute sans transaction
MANDATORY     → Doit être dans une transaction, sinon exception
NEVER         → Ne doit PAS être dans une transaction, sinon exception
NESTED        → Transaction imbriquée avec savepoint
```

### Piège : @Transactional sur méthode privée ou appel interne

```java
@Service
public class AuthorService {

    // ❌ PROBLÈME — appel interne, Spring AOP ne passe pas par le proxy
    public void outerMethod() {
        this.innerTransactionalMethod();  // ← La transaction NE sera PAS créée !
    }

    @Transactional
    private void innerTransactionalMethod() { ... }  // ← private = AOP impossible

    // ✅ SOLUTION — s'injecter soi-même ou déplacer dans un autre bean
    @Autowired
    private AuthorService self;  // Auto-injection du proxy Spring

    public void outerMethod() {
        self.innerTransactionalMethod();  // ← Passe par le proxy → transaction OK
    }

    @Transactional
    public void innerTransactionalMethod() { ... }  // ← public maintenant
}
```

---

## 12. Héritage

### Exemple : une hiérarchie de véhicules

```
Vehicle (abstract)
├── Car
│   ├── numberOfDoors
│   └── trunkVolume
└── Truck
    ├── payload
    └── numberOfAxles
```

### Stratégie 1 : SINGLE_TABLE (défaut)

```java
@Entity
@Table(name = "vehicles")
@Inheritance(strategy = InheritanceType.SINGLE_TABLE)
@DiscriminatorColumn(name = "vehicle_type", discriminatorType = DiscriminatorType.STRING)
@Getter @Setter
public abstract class Vehicle {

    @Id @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    private String brand;
    private String model;
    private Integer year;
}

@Entity
@DiscriminatorValue("CAR")
public class Car extends Vehicle {
    private Integer numberOfDoors;
    private Double trunkVolume;
    // numberOfDoors et trunkVolume seront NULL pour les Truck
}

@Entity
@DiscriminatorValue("TRUCK")
public class Truck extends Vehicle {
    private Double payload;
    private Integer numberOfAxles;
}
```

**Table générée :**
```
vehicles
├── id
├── vehicle_type   ← "CAR" ou "TRUCK"
├── brand
├── model
├── year
├── number_of_doors   ← NULL pour les Truck
├── trunk_volume      ← NULL pour les Truck
├── payload           ← NULL pour les Car
└── number_of_axles   ← NULL pour les Car
```

✅ Avantages : Requêtes simples, pas de JOIN  
⚠️ Inconvénients : Colonnes nullable, table large

---

### Stratégie 2 : JOINED

```java
@Entity
@Table(name = "vehicles")
@Inheritance(strategy = InheritanceType.JOINED)
public abstract class Vehicle {
    @Id @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    private String brand;
    private String model;
}

@Entity
@Table(name = "cars")
@PrimaryKeyJoinColumn(name = "vehicle_id")
public class Car extends Vehicle {
    private Integer numberOfDoors;
}

@Entity
@Table(name = "trucks")
@PrimaryKeyJoinColumn(name = "vehicle_id")
public class Truck extends Vehicle {
    private Double payload;
}
```

**Tables générées :**
```
vehicles : id, brand, model
cars     : vehicle_id (FK → vehicles.id), number_of_doors
trucks   : vehicle_id (FK → vehicles.id), payload
```

✅ Avantages : Schéma propre, pas de NULL  
⚠️ Inconvénients : JOIN à chaque requête

---

### Stratégie 3 : TABLE_PER_CLASS

```java
@Entity
@Inheritance(strategy = InheritanceType.TABLE_PER_CLASS)
public abstract class Vehicle {
    @Id @GeneratedValue(strategy = GenerationType.AUTO) // ← Pas IDENTITY !
    private Long id;
    private String brand;
}

@Entity
@Table(name = "cars")
public class Car extends Vehicle {
    private Integer numberOfDoors;
    // brand est répété dans cette table
}
```

⚠️ Rarement recommandé car les requêtes polymorphiques font des UNION ALL.

---

## 13. Types Avancés

### @Embeddable — Value Objects

```java
@Embeddable
@Getter @Setter @NoArgsConstructor @AllArgsConstructor
public class Address {
    private String street;
    private String city;

    @Column(name = "zip_code", length = 10)
    private String zipCode;

    private String country;
}

@Entity
public class Author {

    @Id @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    private String name;

    @Embedded
    @AttributeOverrides({
        // Optionnel : renommer les colonnes si conflit
        @AttributeOverride(name = "street", column = @Column(name = "home_street")),
        @AttributeOverride(name = "city",   column = @Column(name = "home_city"))
    })
    private Address homeAddress;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "street", column = @Column(name = "work_street")),
        @AttributeOverride(name = "city",   column = @Column(name = "work_city"))
    })
    private Address workAddress;  // Réutiliser la même classe !
}
```

### @Converter — Types personnalisés

```java
// Exemple : stocker une List<String> comme JSON en base
@Converter
public class StringListConverter implements AttributeConverter<List<String>, String> {

    private final ObjectMapper objectMapper = new ObjectMapper();

    @Override
    public String convertToDatabaseColumn(List<String> list) {
        try {
            return list == null ? null : objectMapper.writeValueAsString(list);
        } catch (JsonProcessingException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public List<String> convertToEntityAttribute(String json) {
        try {
            if (json == null) return new ArrayList<>();
            return objectMapper.readValue(json, new TypeReference<List<String>>() {});
        } catch (JsonProcessingException e) {
            throw new RuntimeException(e);
        }
    }
}

@Entity
public class Book {

    @Id @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Convert(converter = StringListConverter.class)
    @Column(columnDefinition = "TEXT")
    private List<String> tags = new ArrayList<>();
    // Stocké comme: ["java", "spring", "hibernate"] en base
}
```

### @Enumerated

```java
public enum BookStatus {
    DRAFT, PUBLISHED, ARCHIVED
}

@Entity
public class Book {

    // ❌ ORDINAL (défaut) — stocke 0, 1, 2 → dangereux si on réordonne l'enum !
    @Enumerated(EnumType.ORDINAL)
    private BookStatus statusOrdinal;

    // ✅ STRING — stocke "DRAFT", "PUBLISHED" → lisible et stable
    @Enumerated(EnumType.STRING)
    @Column(length = 20)
    private BookStatus status;
}
```

### @ElementCollection — Collection de types simples

```java
@Entity
public class Author {

    @Id @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    // Collection de Strings → table séparée author_phones
    @ElementCollection
    @CollectionTable(
        name = "author_phones",
        joinColumns = @JoinColumn(name = "author_id")
    )
    @Column(name = "phone_number")
    private List<String> phoneNumbers = new ArrayList<>();

    // Collection d'objets @Embeddable
    @ElementCollection
    @CollectionTable(name = "author_addresses")
    private List<Address> addresses = new ArrayList<>();
}
```

---

## 14. Locking

### Optimistic Locking — @Version

```java
@Entity
public class Product {

    @Id @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    private String name;
    private Integer stock;

    @Version
    private Long version;   // ← Géré automatiquement par Hibernate
    // Peut aussi être Timestamp ou LocalDateTime
}
```

**Ce qui se passe en base :**
```sql
-- Hibernate ajoute automatiquement la version dans le WHERE
UPDATE products
SET stock = 9, version = 2
WHERE id = 1 AND version = 1;   -- ← Si version = 2 en base → 0 lignes modifiées → exception !
```

```java
@Transactional
public void decreaseStock(Long productId, int quantity) {
    try {
        Product product = productRepository.findById(productId).get();
        product.setStock(product.getStock() - quantity);
        productRepository.save(product);
    } catch (OptimisticLockException | ObjectOptimisticLockingFailureException e) {
        // Quelqu'un d'autre a modifié le produit entre le findById et le save
        throw new ConcurrentModificationException("Stock modifié par un autre utilisateur, réessayez");
    }
}
```

### Pessimistic Locking — Locks SQL

```java
// Lock exclusif — personne ne peut lire/écrire pendant notre transaction
@Lock(LockModeType.PESSIMISTIC_WRITE)
@Query("SELECT p FROM Product p WHERE p.id = :id")
Optional<Product> findByIdForUpdate(@Param("id") Long id);
// SQL: SELECT * FROM products WHERE id = ? FOR UPDATE

// Lock partagé — d'autres peuvent lire mais pas écrire
@Lock(LockModeType.PESSIMISTIC_READ)
Optional<Product> findById(Long id);

// Avec timeout (évite d'attendre indéfiniment)
@Lock(LockModeType.PESSIMISTIC_WRITE)
@QueryHints(@QueryHint(name = "jakarta.persistence.lock.timeout", value = "3000"))
Optional<Product> findByIdWithTimeout(Long id);
```

| Lock | Usage |
|------|-------|
| Optimistic | Conflits rares, lecture intensive (API REST, e-commerce) |
| Pessimistic | Conflits fréquents, opérations critiques (réservations, stocks) |

---

## 15. Performance & Bonnes Pratiques

### Détecter le problème N+1

```java
// ❌ Problème N+1 classique
List<Author> authors = authorRepository.findAll();  // 1 requête
for (Author author : authors) {
    System.out.println(author.getBooks().size());   // N requêtes !
    // Si 100 auteurs → 101 requêtes SQL
}

// ✅ Solution 1 : JOIN FETCH
@Query("SELECT DISTINCT a FROM Author a JOIN FETCH a.books")
List<Author> findAllWithBooks();
// 1 seule requête SQL

// ✅ Solution 2 : @EntityGraph
@EntityGraph(attributePaths = "books")
List<Author> findAll();

// ✅ Solution 3 : @BatchSize
@OneToMany(mappedBy = "author")
@BatchSize(size = 20)  // Hibernate charge les livres par batch de 20 auteurs
private List<Book> books;
// Résultat : au lieu de N requêtes → ceil(N/20) requêtes
```

### Activer les statistiques pour détecter les problèmes

```properties
spring.jpa.properties.hibernate.generate_statistics=true
logging.level.org.hibernate.stat=DEBUG
logging.level.org.hibernate.SQL=DEBUG
logging.level.org.hibernate.orm.jdbc.bind=TRACE  # Affiche les paramètres
```

### Hibernate Validator (Bean Validation)

```java
@Entity
public class Author {

    @Id @GeneratedValue
    private Long id;

    @NotBlank(message = "Le prénom est obligatoire")
    @Size(min = 2, max = 100)
    private String firstName;

    @Email(message = "Email invalide")
    @NotNull
    private String email;

    @Min(value = 0, message = "L'âge ne peut pas être négatif")
    @Max(value = 150)
    private Integer age;

    @Past(message = "La date de naissance doit être dans le passé")
    private LocalDate birthDate;

    @Pattern(regexp = "^\\+?[0-9]{10,15}$", message = "Numéro de téléphone invalide")
    private String phone;
}
```

### DTOs et mapping — Ne pas exposer les entités

```java
// ❌ Mauvaise pratique — exposer l'entité directement en API
@GetMapping("/authors/{id}")
public Author getAuthor(@PathVariable Long id) {
    return authorRepository.findById(id).get();
    // Problème : sérialise toute l'entité, déclenche du LAZY, cycles infinis...
}

// ✅ Bonne pratique — utiliser des DTOs
public record AuthorResponseDTO(
    Long id,
    String firstName,
    String lastName,
    String email,
    int numberOfBooks
) {
    public static AuthorResponseDTO from(Author author) {
        return new AuthorResponseDTO(
            author.getId(),
            author.getFirstName(),
            author.getLastName(),
            author.getEmail(),
            author.getBooks().size()
        );
    }
}

// Ou utiliser MapStruct pour le mapping automatique
@Mapper(componentModel = "spring")
public interface AuthorMapper {
    AuthorResponseDTO toDto(Author author);
    Author toEntity(AuthorCreateDTO dto);
}
```

---

## 16. Les Pièges Classiques

### Piège 1 : toString() avec les relations

```java
// ❌ DANGER — boucle infinie et LazyInitializationException
@ToString  // ← Lombok va inclure tous les champs !
public class Author {
    @OneToMany(mappedBy = "author")
    private List<Book> books;  // ← toString() va appeler getBooks() → LAZY → exception !
}

// ✅ Exclure les collections
@ToString(exclude = "books")
public class Author { ... }
```

### Piège 2 : Modifier une collection détachée

```java
// ❌ Problème
Author author = authorRepository.findById(1L).get();
// Transaction terminée ici, author est DETACHED

author.getBooks().add(new Book());  // LazyInitializationException !
authorRepository.save(author);       // Les books ne sont pas sauvegardés correctement

// ✅ Solution — tout dans la même transaction
@Transactional
public void addBookToAuthor(Long authorId, BookDTO dto) {
    Author author = authorRepository.findById(authorId).get();
    Book book = new Book(dto.title());
    author.addBook(book);  // Utiliser la méthode helper
    // Pas besoin de save() explicite, dirty checking s'en occupe
}
```

### Piège 3 : Open Session In View

```properties
# ❌ Activé par défaut dans Spring Boot — masque les problèmes de LAZY
spring.jpa.open-in-view=true

# ✅ Le désactiver pour forcer les bonnes pratiques
spring.jpa.open-in-view=false
# Avec false : LazyInitializationException levée dès qu'on sort de @Transactional
# → Force à charger les données correctement en amont
```

### Piège 4 : delete() vs deleteById()

```java
// deleteById() fait un findById() puis un delete()
// Si l'entité n'existe pas → EmptyResultDataAccessException

// ✅ Utiliser deleteById() avec vérification
if (authorRepository.existsById(id)) {
    authorRepository.deleteById(id);
}

// Ou directement avec une requête
@Modifying
@Transactional
@Query("DELETE FROM Author a WHERE a.id = :id")
void deleteAuthorById(@Param("id") Long id);
```

### Piège 5 : N+1 silencieux dans les tests

```java
// Activer la détection dans les tests
@DataJpaTest
@Transactional
class AuthorRepositoryTest {

    // Utiliser une lib comme datasource-proxy ou hypersistence-utils
    // pour compter les requêtes SQL dans les tests
    @Test
    void shouldLoadBooksInOneQuery() {
        List<Author> authors = authorRepository.findAllWithBooks();
        // Assert que la liste n'est pas vide
        assertThat(authors).isNotEmpty();
        // Accéder aux books ne doit pas déclencher de requêtes supplémentaires
        authors.forEach(a -> a.getBooks().size());
    }
}
```

### Piège 6 : equals/hashCode et les Sets

```java
// ❌ Si equals/hashCode utilisent l'ID auto-généré :
Set<Author> authors = new HashSet<>();
Author author = new Author();  // id = null
authors.add(author);           // hashCode basé sur null

author = authorRepository.save(author);  // id = 1 maintenant
authors.contains(author);  // false ! Le hashCode a changé → introuvable dans le Set

// ✅ Utiliser un UUID naturel comme expliqué dans la section 3
```

---

## Récapitulatif

```
┌─────────────────────────────────────────────────────────────────┐
│                    AIDE-MÉMOIRE HIBERNATE                        │
├─────────────────────────────────────────────────────────────────┤
│  Entité           │ @Entity, @Table, @Id, @Column, @Transient   │
│  ID               │ IDENTITY, SEQUENCE (recommandé PostgreSQL)   │
│  Relations        │ @OneToMany, @ManyToOne, @ManyToMany          │
│  Fetch            │ Toujours LAZY, JOIN FETCH ou @EntityGraph     │
│  Cycle de vie     │ Transient → Managed → Detached → Removed     │
│  Cache L1         │ Automatique, durée de la session             │
│  Cache L2         │ Optionnel, EhCache, données stables          │
│  Requêtes         │ JPQL ou Spring Data methods ou @Query        │
│  Dynamique        │ Criteria API ou Specification                │
│  Transaction      │ @Transactional sur les services              │
│  Performance      │ Pas de N+1, DTOs, readOnly=true en lecture   │
│  Concurrence      │ @Version (optimistic) ou FOR UPDATE          │
│  Héritage         │ SINGLE_TABLE (perf) ou JOINED (propre)       │
└─────────────────────────────────────────────────────────────────┘
```

---

*Bonne chance pour ton entretien ! Tu es prêt(e) 🚀*
