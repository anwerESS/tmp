# 📘 Cours Complet : MongoDB avec Spring Boot

> **Objectif** : Comprendre MongoDB, ses concepts clés, et savoir configurer une application Spring Boot avec une base de données MongoDB — prêt pour un entretien technique.

---

## Table des matières

1. [C'est quoi MongoDB ?](#1-cest-quoi-mongodb-)
2. [Concepts fondamentaux de MongoDB](#2-concepts-fondamentaux-de-mongodb)
3. [Comparaison MongoDB vs Base relationnelle (SQL)](#3-comparaison-mongodb-vs-base-relationnelle-sql)
4. [Spring Boot + MongoDB : Configuration](#4-spring-boot--mongodb--configuration)
5. [Les annotations essentielles](#5-les-annotations-essentielles)
6. [Repository Pattern avec Spring Data MongoDB](#6-repository-pattern-avec-spring-data-mongodb)
7. [Opérations CRUD complètes](#7-opérations-crud-complètes)
8. [Queries avancées](#8-queries-avancées)
9. [Embedded Documents vs References](#9-embedded-documents-vs-references)
10. [Indexing dans MongoDB](#10-indexing-dans-mongodb)
11. [Questions / Réponses d'entretien](#11-questions--réponses-dentretien)

---

## 1. C'est quoi MongoDB ?

MongoDB est une base de données **NoSQL** orientée **documents**. Au lieu de stocker les données dans des tables avec des lignes et des colonnes (comme MySQL ou PostgreSQL), MongoDB stocke les données sous forme de **documents JSON** (en interne, c'est du BSON — Binary JSON).

### Pourquoi NoSQL ?

- Pas de schéma fixe (flexible schema)
- Facile à scaler horizontalement (sharding)
- Très performant pour les grandes quantités de données
- Idéal pour des données hiérarchiques ou imbriquées

### Exemple d'un document MongoDB

```json
{
  "_id": "64a1f2c3e4b09d1234567890",
  "nom": "Alice Martin",
  "age": 28,
  "adresse": {
    "rue": "12 rue de la Paix",
    "ville": "Paris"
  },
  "competences": ["Java", "Spring Boot", "MongoDB"]
}
```

Tout est dans un seul document — pas besoin de JOIN !

---

## 2. Concepts fondamentaux de MongoDB

### 2.1 Database

Une **database** est un conteneur qui regroupe plusieurs collections. Exemple : `myapp_db`.

### 2.2 Collection

Une **collection** est l'équivalent d'une table en SQL. Elle contient un ensemble de documents. Exemple : `utilisateurs`, `produits`, `commandes`.

> 💡 Différence clé : une collection n'impose **pas de schéma fixe**. Deux documents dans la même collection peuvent avoir des champs différents.

### 2.3 Document

Un **document** est l'équivalent d'une ligne en SQL. C'est un objet JSON/BSON avec des paires clé-valeur.

### 2.4 Field (Champ)

Un **field** est l'équivalent d'une colonne. Chaque document a ses propres champs.

### 2.5 `_id`

Chaque document a un champ `_id` unique (comme une primary key). Par défaut, MongoDB génère un **ObjectId** automatiquement.

```
_id: ObjectId("64a1f2c3e4b09d1234567890")
```

### 2.6 BSON

BSON = **Binary JSON**. C'est le format interne utilisé par MongoDB pour stocker les documents. Il supporte plus de types que JSON (ex: `Date`, `ObjectId`, `Binary`).

---

## 3. Comparaison MongoDB vs Base relationnelle (SQL)

| SQL          | MongoDB          |
|--------------|------------------|
| Database     | Database         |
| Table        | Collection       |
| Row (ligne)  | Document         |
| Column       | Field            |
| Primary Key  | `_id`            |
| JOIN         | Embedded document / `$lookup` |
| Schema fixe  | Schema flexible  |

---

## 4. Spring Boot + MongoDB : Configuration

### 4.1 Dépendances Maven (`pom.xml`)

```xml
<dependencies>
    <!-- Spring Boot Starter Web -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-web</artifactId>
    </dependency>

    <!-- Spring Data MongoDB -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-data-mongodb</artifactId>
    </dependency>

    <!-- Lombok (optionnel, réduit le boilerplate) -->
    <dependency>
        <groupId>org.projectlombok</groupId>
        <artifactId>lombok</artifactId>
        <optional>true</optional>
    </dependency>
</dependencies>
```

### 4.2 Configuration dans `application.properties`

```properties
# URI de connexion MongoDB
spring.data.mongodb.uri=mongodb://localhost:27017/myapp_db

# Ou bien avec host/port/database séparément :
spring.data.mongodb.host=localhost
spring.data.mongodb.port=27017
spring.data.mongodb.database=myapp_db

# Avec authentification :
# spring.data.mongodb.uri=mongodb://user:password@localhost:27017/myapp_db
```

### 4.3 Configuration dans `application.yml` (alternative)

```yaml
spring:
  data:
    mongodb:
      uri: mongodb://localhost:27017/myapp_db
```

### 4.4 Configuration Java (optionnelle, pour plus de contrôle)

```java
@Configuration
public class MongoConfig extends AbstractMongoClientConfiguration {

    @Override
    protected String getDatabaseName() {
        return "myapp_db";
    }

    @Override
    public MongoClient mongoClient() {
        return MongoClients.create("mongodb://localhost:27017");
    }
}
```

> 💡 Dans la majorité des cas, la configuration dans `application.properties` suffit largement.

---

## 5. Les annotations essentielles

### 5.1 `@Document`

Indique que cette classe Java est mappée à une **collection MongoDB**.

```java
@Document(collection = "utilisateurs")
public class Utilisateur {
    // ...
}
```

Si tu ne précises pas `collection`, Spring utilisera le nom de la classe en minuscule.

### 5.2 `@Id`

Marque le champ qui correspond au `_id` MongoDB.

```java
@Id
private String id;
```

> Le type `String` est recommandé car MongoDB génère des `ObjectId` que Spring convertit automatiquement en String.

### 5.3 `@Field`

Permet de définir le nom du champ dans MongoDB (si différent du nom Java).

```java
@Field("nom_complet")
private String nomComplet;
```

### 5.4 `@Indexed`

Crée un **index** sur ce champ pour accélérer les recherches.

```java
@Indexed(unique = true)
private String email;
```

### 5.5 `@DBRef`

Crée une **référence** vers un autre document (comme une foreign key).

```java
@DBRef
private Role role;
```

### 5.6 `@Transient`

Ce champ ne sera **pas sauvegardé** dans MongoDB.

```java
@Transient
private String champTemporaire;
```

### Exemple complet d'un Document

```java
@Document(collection = "utilisateurs")
@Data                    // Lombok : génère getters/setters/toString
@NoArgsConstructor
@AllArgsConstructor
public class Utilisateur {

    @Id
    private String id;

    @Field("nom_complet")
    private String nomComplet;

    @Indexed(unique = true)
    private String email;

    private int age;

    private Adresse adresse;        // Embedded document

    private List<String> competences;

    private LocalDateTime dateCreation;
}
```

```java
// Classe Adresse (embedded, pas de @Document car pas de collection propre)
@Data
@NoArgsConstructor
@AllArgsConstructor
public class Adresse {
    private String rue;
    private String ville;
    private String codePostal;
}
```

---

## 6. Repository Pattern avec Spring Data MongoDB

Spring Data MongoDB fournit des interfaces Repository qui génèrent automatiquement les requêtes CRUD — **sans écrire une seule ligne SQL**.

### 6.1 `MongoRepository`

```java
@Repository
public interface UtilisateurRepository extends MongoRepository<Utilisateur, String> {
    // Spring génère automatiquement : save, findById, findAll, deleteById, count...
}
```

`MongoRepository<T, ID>` :
- `T` = le type du document (ici `Utilisateur`)
- `ID` = le type de l'identifiant (ici `String`)

### 6.2 Méthodes générées automatiquement

| Méthode | Description |
|---------|-------------|
| `save(entity)` | Insère ou met à jour un document |
| `findById(id)` | Trouve par `_id` → retourne `Optional<T>` |
| `findAll()` | Retourne tous les documents |
| `deleteById(id)` | Supprime par `_id` |
| `existsById(id)` | Vérifie si un document existe |
| `count()` | Compte tous les documents |

### 6.3 Derived Query Methods (requêtes dérivées du nom de la méthode)

Spring Data génère les requêtes **automatiquement** en lisant le nom de la méthode !

```java
@Repository
public interface UtilisateurRepository extends MongoRepository<Utilisateur, String> {

    // SELECT * WHERE email = ?
    Optional<Utilisateur> findByEmail(String email);

    // SELECT * WHERE age > ?
    List<Utilisateur> findByAgeGreaterThan(int age);

    // SELECT * WHERE nomComplet LIKE ?
    List<Utilisateur> findByNomCompletContaining(String keyword);

    // SELECT * WHERE ville = ? AND age >= ?
    List<Utilisateur> findByAdresse_VilleAndAgeGreaterThanEqual(String ville, int age);

    // SELECT COUNT WHERE email = ?
    long countByEmail(String email);

    // DELETE WHERE age < ?
    void deleteByAgeLessThan(int age);
}
```

### Mots-clés disponibles pour les derived queries

| Mot-clé | Exemple | Signification |
|---------|---------|---------------|
| `findBy` | `findByNom` | WHERE nom = ? |
| `And` | `findByNomAndAge` | WHERE nom = ? AND age = ? |
| `Or` | `findByNomOrEmail` | WHERE nom = ? OR email = ? |
| `GreaterThan` | `findByAgeGreaterThan` | WHERE age > ? |
| `LessThan` | `findByAgeLessThan` | WHERE age < ? |
| `Between` | `findByAgeBetween` | WHERE age BETWEEN ? AND ? |
| `Like` / `Containing` | `findByNomContaining` | WHERE nom LIKE %?% |
| `StartingWith` | `findByNomStartingWith` | WHERE nom LIKE ?% |
| `In` | `findByVilleIn` | WHERE ville IN (?,?) |
| `Exists` | `findByEmailExists` | WHERE email EXISTS |
| `OrderBy` | `findByAgeOrderByNomAsc` | ORDER BY nom ASC |

---

## 7. Opérations CRUD complètes

### Structure du projet

```
src/main/java/com/example/
├── model/
│   └── Utilisateur.java
├── repository/
│   └── UtilisateurRepository.java
├── service/
│   └── UtilisateurService.java
└── controller/
    └── UtilisateurController.java
```

### 7.1 Service Layer

```java
@Service
public class UtilisateurService {

    @Autowired
    private UtilisateurRepository utilisateurRepository;

    // CREATE
    public Utilisateur creer(Utilisateur utilisateur) {
        utilisateur.setDateCreation(LocalDateTime.now());
        return utilisateurRepository.save(utilisateur);
    }

    // READ - tous
    public List<Utilisateur> getTous() {
        return utilisateurRepository.findAll();
    }

    // READ - par id
    public Utilisateur getParId(String id) {
        return utilisateurRepository.findById(id)
                .orElseThrow(() -> new RuntimeException("Utilisateur non trouvé : " + id));
    }

    // READ - par email
    public Optional<Utilisateur> getParEmail(String email) {
        return utilisateurRepository.findByEmail(email);
    }

    // UPDATE
    public Utilisateur modifier(String id, Utilisateur modifications) {
        Utilisateur existant = getParId(id);
        existant.setNomComplet(modifications.getNomComplet());
        existant.setAge(modifications.getAge());
        return utilisateurRepository.save(existant);
    }

    // DELETE
    public void supprimer(String id) {
        utilisateurRepository.deleteById(id);
    }
}
```

### 7.2 Controller Layer (REST API)

```java
@RestController
@RequestMapping("/api/utilisateurs")
public class UtilisateurController {

    @Autowired
    private UtilisateurService utilisateurService;

    @PostMapping
    public ResponseEntity<Utilisateur> creer(@RequestBody Utilisateur utilisateur) {
        Utilisateur cree = utilisateurService.creer(utilisateur);
        return ResponseEntity.status(HttpStatus.CREATED).body(cree);
    }

    @GetMapping
    public List<Utilisateur> getTous() {
        return utilisateurService.getTous();
    }

    @GetMapping("/{id}")
    public Utilisateur getParId(@PathVariable String id) {
        return utilisateurService.getParId(id);
    }

    @PutMapping("/{id}")
    public Utilisateur modifier(@PathVariable String id, @RequestBody Utilisateur u) {
        return utilisateurService.modifier(id, u);
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Void> supprimer(@PathVariable String id) {
        utilisateurService.supprimer(id);
        return ResponseEntity.noContent().build();
    }
}
```

---

## 8. Queries avancées

### 8.1 `@Query` — Requête MongoDB personnalisée

Pour des requêtes complexes, on utilise `@Query` avec la syntaxe MongoDB :

```java
@Repository
public interface UtilisateurRepository extends MongoRepository<Utilisateur, String> {

    // Trouve par email (syntaxe MongoDB JSON)
    @Query("{ 'email': ?0 }")
    Optional<Utilisateur> findByEmailQuery(String email);

    // Trouve par ville (champ imbriqué)
    @Query("{ 'adresse.ville': ?0 }")
    List<Utilisateur> findByVille(String ville);

    // Trouve les utilisateurs avec une compétence spécifique
    @Query("{ 'competences': { $in: [?0] } }")
    List<Utilisateur> findByCompetence(String competence);

    // Trouve les utilisateurs dont l'âge est dans une plage
    @Query("{ 'age': { $gte: ?0, $lte: ?1 } }")
    List<Utilisateur> findByAgeBetween(int min, int max);
}
```

### 8.2 `MongoTemplate` — Pour les requêtes très complexes

`MongoTemplate` est une classe Spring qui donne un accès plus bas niveau à MongoDB.

```java
@Service
public class UtilisateurSearchService {

    @Autowired
    private MongoTemplate mongoTemplate;

    public List<Utilisateur> rechercheAvancee(String ville, int ageMin) {

        Query query = new Query();
        query.addCriteria(
            Criteria.where("adresse.ville").is(ville)
                    .and("age").gte(ageMin)
        );
        query.with(Sort.by(Sort.Direction.ASC, "nomComplet"));
        query.limit(10);

        return mongoTemplate.find(query, Utilisateur.class);
    }

    public long compterParVille(String ville) {
        Query query = new Query(Criteria.where("adresse.ville").is(ville));
        return mongoTemplate.count(query, Utilisateur.class);
    }

    // Update partiel (sans charger l'objet entier)
    public void mettreAJourAge(String id, int nouvelAge) {
        Query query = new Query(Criteria.where("_id").is(id));
        Update update = new Update().set("age", nouvelAge);
        mongoTemplate.updateFirst(query, update, Utilisateur.class);
    }
}
```

### 8.3 Aggregation Pipeline

Le pipeline d'agrégation permet des opérations complexes comme des GROUP BY, des statistiques, etc.

```java
public Map<String, Long> compterParVille() {
    Aggregation aggregation = Aggregation.newAggregation(
        Aggregation.group("adresse.ville").count().as("total"),
        Aggregation.sort(Sort.Direction.DESC, "total")
    );

    AggregationResults<Document> results =
        mongoTemplate.aggregate(aggregation, "utilisateurs", Document.class);

    Map<String, Long> stats = new LinkedHashMap<>();
    results.getMappedResults().forEach(doc ->
        stats.put(doc.getString("_id"), doc.getLong("total"))
    );
    return stats;
}
```

---

## 9. Embedded Documents vs References

C'est un choix de design fondamental dans MongoDB.

### 9.1 Embedded Document (Document imbriqué)

On stocke un objet **directement à l'intérieur** du document parent.

```java
@Document(collection = "commandes")
public class Commande {
    @Id
    private String id;
    private String numeroCommande;

    // Adresse embedded directement dans la commande
    private Adresse adresseLivraison;

    // Liste de produits embedded
    private List<LigneCommande> lignes;
}
```

✅ **Avantages** :
- Une seule requête pour tout lire (pas de JOIN)
- Performance excellente en lecture

❌ **Inconvénients** :
- Duplication de données si l'objet est partagé par plusieurs documents
- Document peut devenir très lourd (limite MongoDB : 16 MB par document)

**Quand utiliser** : quand les données appartiennent exclusivement au document parent (ex: adresse d'une commande, lignes d'une facture).

### 9.2 References avec `@DBRef`

On stocke une **référence** (l'`_id`) vers un autre document dans une autre collection.

```java
@Document(collection = "utilisateurs")
public class Utilisateur {
    @Id
    private String id;
    private String nomComplet;

    @DBRef
    private Role role; // référence vers la collection "roles"
}
```

```java
@Document(collection = "roles")
public class Role {
    @Id
    private String id;
    private String nom; // "ADMIN", "USER", etc.
}
```

✅ **Avantages** :
- Pas de duplication
- Mise à jour centralisée

❌ **Inconvénients** :
- Spring fait des requêtes séparées pour charger les références (N+1 problem possible)
- Moins performant que l'embedded

**Quand utiliser** : quand les données sont partagées par plusieurs documents (ex: un rôle utilisé par des milliers d'utilisateurs).

---

## 10. Indexing dans MongoDB

Les **indexes** accélèrent les requêtes en évitant de parcourir toute la collection.

### 10.1 Index via annotation

```java
@Document(collection = "utilisateurs")
public class Utilisateur {

    @Id
    private String id;

    @Indexed(unique = true)          // Index unique sur email
    private String email;

    @Indexed                          // Index simple
    private String ville;

    // Index composé via @CompoundIndex (sur la classe)
}
```

### 10.2 Index composé (`@CompoundIndex`)

```java
@Document(collection = "utilisateurs")
@CompoundIndex(name = "ville_age_idx", def = "{'adresse.ville': 1, 'age': -1}")
public class Utilisateur {
    // ...
}
```

`1` = ordre croissant, `-1` = ordre décroissant.

### 10.3 Activer la création automatique des indexes

Dans `application.properties` :
```properties
spring.data.mongodb.auto-index-creation=true
```

> ⚠️ En production, il est recommandé de créer les indexes manuellement via MongoDB pour éviter les ralentissements au démarrage.

---

## 11. Questions / Réponses d'entretien

---

### ❓ C'est quoi MongoDB ? Pourquoi l'utiliser avec Spring Boot ?

> MongoDB est une base de données NoSQL orientée documents. On l'utilise quand les données sont flexibles ou imbriquées, et qu'on n'a pas besoin de schéma fixe. Avec Spring Boot, l'intégration est très simple grâce à Spring Data MongoDB qui fournit des repositories automatiques.

---

### ❓ Quelle est la différence entre une collection et une table SQL ?

> Une table SQL a un schéma fixe — chaque ligne a les mêmes colonnes. Une collection MongoDB est flexible — chaque document peut avoir des champs différents. C'est l'avantage du "schemaless".

---

### ❓ C'est quoi un `ObjectId` dans MongoDB ?

> C'est l'identifiant unique généré automatiquement par MongoDB pour chaque document. Il contient un timestamp, l'identifiant de la machine, et un compteur, ce qui garantit son unicité même dans un cluster distribué.

---

### ❓ Quelle annotation utilise-t-on pour mapper une classe Java à une collection MongoDB ?

> On utilise `@Document(collection = "nom_collection")`. Et `@Id` pour marquer le champ identifiant.

---

### ❓ Qu'est-ce que Spring Data MongoDB apporte concrètement ?

> Il fournit l'interface `MongoRepository` qui génère automatiquement les opérations CRUD. On peut aussi écrire des derived query methods — Spring crée la requête MongoDB juste en lisant le nom de la méthode Java. Pour les requêtes complexes, on a `@Query` et `MongoTemplate`.

---

### ❓ Quelle est la différence entre `MongoRepository` et `MongoTemplate` ?

> `MongoRepository` est une abstraction de haut niveau — idéal pour les opérations simples, on n'écrit presque pas de code. `MongoTemplate` est plus bas niveau et plus flexible — il permet des requêtes complexes, des updates partiels, des aggregations, etc.

---

### ❓ C'est quoi les derived query methods ? Donne un exemple.

> Spring Data génère les requêtes MongoDB automatiquement à partir du nom de la méthode. Par exemple, `findByEmailAndAge(String email, int age)` génère automatiquement la requête `{ email: ?, age: ? }` sans qu'on écrive quoi que ce soit.

---

### ❓ Embedded document vs référence (`@DBRef`) — comment tu choisis ?

> J'utilise l'embedded quand les données appartiennent exclusivement à ce document et qu'elles ne seront pas partagées — ça évite les JOINs et c'est plus performant. J'utilise `@DBRef` quand les données sont partagées entre plusieurs documents, pour éviter la duplication. Par exemple, une adresse de livraison → embedded. Un rôle utilisateur → référence.

---

### ❓ C'est quoi un index dans MongoDB ? Pourquoi c'est important ?

> Un index est une structure de données qui accélère les requêtes en évitant de scanner toute la collection. Sans index, MongoDB fait un "collection scan" qui lit tous les documents. Avec un index sur un champ, la recherche est beaucoup plus rapide. On les crée avec `@Indexed` en Spring Boot.

---

### ❓ Que se passe-t-il si je fais un `save()` avec un `id` qui existe déjà dans MongoDB ?

> Si l'`id` existe déjà, `save()` fait un **upsert** — il met à jour le document existant. Si l'`id` est `null` ou nouveau, il insère un nouveau document. C'est le comportement par défaut de `MongoRepository.save()`.

---

### ❓ Comment configurer la connexion MongoDB dans Spring Boot ?

> Le plus simple : dans `application.properties` avec `spring.data.mongodb.uri=mongodb://localhost:27017/mabase`. Pour une configuration plus avancée, on peut créer une classe `@Configuration` qui étend `AbstractMongoClientConfiguration`.

---

### ❓ Qu'est-ce que le `@Transient` dans un document MongoDB ?

> Un champ annoté `@Transient` ne sera pas sauvegardé dans MongoDB. Il est utile pour des champs calculés ou temporaires qu'on veut avoir en Java mais pas en base de données.

---

### ❓ Qu'est-ce qu'une Aggregation Pipeline ?

> C'est une suite d'étapes (stages) qui transforment les documents : `$match` pour filtrer, `$group` pour regrouper, `$sort` pour trier, `$project` pour sélectionner les champs... C'est l'équivalent des GROUP BY et des fonctions d'agrégation en SQL. Avec Spring, on utilise `MongoTemplate.aggregate()`.

---

### ❓ Comment faire un update partiel sans charger tout le document ?

> Avec `MongoTemplate` :
> ```java
> Query query = new Query(Criteria.where("_id").is(id));
> Update update = new Update().set("age", 30);
> mongoTemplate.updateFirst(query, update, Utilisateur.class);
> ```
> Ça évite de charger et re-sauvegarder tout le document — plus efficace.

---

*Bon courage pour ton entretien ! 🚀*
