# 🍃 Hibernate — Questions & Réponses pour Entretien

> Réponses en français oral, mots techniques en anglais.

---

## 📌 1. C'est quoi Hibernate ?

**Q : Qu'est-ce qu'Hibernate ?**

Hibernate c'est un framework Java qui s'occupe de faire le lien entre le monde objet et la base de données relationnelle. On appelle ça un ORM — *Object Relational Mapping*. Concrètement, au lieu d'écrire du SQL à la main, on travaille avec des objets Java, et Hibernate se charge de générer les requêtes SQL tout seul. Il implémente la spec JPA — *Java Persistence API* — qui est le standard Java pour la persistance des données.

---

## 📌 2. JPA vs Hibernate

**Q : Quelle est la différence entre JPA et Hibernate ?**

JPA c'est une *specification*, c'est-à-dire un ensemble d'interfaces et de règles définies par Java. Hibernate, lui, c'est une *implementation* de cette spec. C'est comme une interface et sa classe concrète. Donc on peut coder contre JPA et utiliser Hibernate en dessous, ou remplacer Hibernate par un autre provider comme EclipseLink si on veut. Dans la pratique avec Spring Boot, on utilise souvent directement les annotations JPA mais c'est Hibernate qui tourne derrière.

---

## 📌 3. Les annotations de base

**Q : Quelles sont les annotations Hibernate/JPA les plus importantes ?**

Les principales :
- `@Entity` : indique que la classe est une entité mappée à une table
- `@Table` : pour préciser le nom de la table si c'est différent du nom de la classe
- `@Id` : pour marquer la clé primaire
- `@GeneratedValue` : pour dire comment l'ID est généré (auto, séquence, identity...)
- `@Column` : pour configurer une colonne (nom, longueur, nullable...)
- `@Transient` : pour dire qu'un champ ne doit PAS être persisté en base

---

## 📌 4. Les stratégies de génération d'ID

**Q : Quelles sont les différentes stratégies pour `@GeneratedValue` ?**

Il y en a quatre :
- `AUTO` : Hibernate choisit tout seul selon la base de données
- `IDENTITY` : la base génère l'ID elle-même, comme avec `AUTO_INCREMENT` en MySQL
- `SEQUENCE` : on utilise une séquence SQL, très courant avec PostgreSQL. On peut définir sa propre séquence avec `@SequenceGenerator`
- `TABLE` : Hibernate utilise une table spéciale pour gérer les IDs — c'est peu performant, on l'évite en général

---

## 📌 5. Le cycle de vie d'une entité

**Q : Quels sont les états possibles d'une entité dans Hibernate ?**

Une entité peut être dans quatre états :
- **Transient** : l'objet vient d'être créé avec `new`, Hibernate n'en sait rien, rien en base
- **Persistent** (ou *Managed*) : l'objet est dans la *session* Hibernate, il est tracké, toute modification sera synchronisée en base
- **Detached** : l'objet était persistent mais la session a été fermée. Il n'est plus tracké
- **Removed** : on a appelé `remove()`, il sera supprimé en base à la prochaine synchro

---

## 📌 6. La Session et le EntityManager

**Q : C'est quoi la Session Hibernate et l'EntityManager JPA ?**

C'est globalement la même chose. La `Session` c'est l'API native Hibernate, et l'`EntityManager` c'est l'équivalent dans la spec JPA. Dans Spring Boot on utilise `EntityManager` ou les repositories Spring Data JPA. C'est lui qui gère le *first level cache*, les transactions, et la synchro avec la base.

---

## 📌 7. Le First Level Cache

**Q : C'est quoi le first level cache ?**

Le *first level cache*, c'est le cache au niveau de la *session* ou de l'`EntityManager`. C'est activé automatiquement, on ne peut pas le désactiver. Ça veut dire que si on fait deux `findById()` avec le même ID dans la même transaction, Hibernate ne fait qu'une seule requête SQL — la deuxième fois il retourne directement l'objet depuis le cache. Ce cache est limité à la durée de vie de la session, donc il est détruit à la fin de la transaction.

---

## 📌 8. Le Second Level Cache

**Q : C'est quoi le second level cache ?**

Le *second level cache* c'est un cache partagé entre toutes les sessions. Il n'est pas activé par défaut, il faut le configurer. On peut utiliser des providers comme EhCache ou Caffeine. Il est utile pour des données qui ne changent pas souvent, comme des données de référence. On marque les entités à mettre en cache avec `@Cache` ou `@Cacheable`. Attention, ça peut créer des problèmes de cohérence si plusieurs instances de l'appli tournent en parallèle.

---

## 📌 9. Les relations entre entités

**Q : Comment on modélise les relations entre entités avec Hibernate ?**

Il y a quatre types de relations :
- `@OneToOne` : un utilisateur a un seul profil
- `@OneToMany` / `@ManyToOne` : un auteur a plusieurs livres, un livre appartient à un auteur
- `@ManyToMany` : un étudiant peut avoir plusieurs cours et un cours peut avoir plusieurs étudiants

Pour `@ManyToMany`, Hibernate crée automatiquement une table de jointure. On peut la personnaliser avec `@JoinTable`.

---

## 📌 10. FetchType LAZY vs EAGER

**Q : C'est quoi la différence entre LAZY et EAGER ?**

C'est comment Hibernate charge les données liées.
- `EAGER` : les données associées sont chargées immédiatement en même temps que l'entité principale. C'est pratique mais ça peut charger beaucoup trop de données
- `LAZY` : les données associées ne sont chargées que quand on y accède vraiment. C'est plus performant mais ça peut causer le fameux problème *LazyInitializationException* si on accède à la relation en dehors de la session

Par défaut : `@OneToMany` et `@ManyToMany` sont LAZY, `@ManyToOne` et `@OneToOne` sont EAGER.

---

## 📌 11. Le problème N+1

**Q : C'est quoi le problème N+1 ?**

C'est un des problèmes les plus courants avec les ORMs. Imagine qu'on charge 10 auteurs, ça fait 1 requête. Puis pour chaque auteur on accède à ses livres — ça fait 10 requêtes de plus. Donc au total 11 requêtes au lieu de 2. Plus N est grand, plus c'est catastrophique pour les perfs.

Pour le résoudre on peut :
- Utiliser `JOIN FETCH` dans une requête JPQL
- Utiliser `@EntityGraph` pour préciser les relations à charger
- Utiliser `@BatchSize` pour regrouper les chargements
- Ou passer à une requête native/DTO directement

---

## 📌 12. JPQL

**Q : C'est quoi JPQL ?**

JPQL c'est le *Java Persistence Query Language*. C'est un langage de requête similaire au SQL, mais au lieu d'opérer sur des tables et colonnes, il opère sur des entités et leurs attributs. Par exemple : `SELECT a FROM Author a WHERE a.name = :name`. Hibernate traduit ensuite ça en SQL. L'avantage c'est que c'est indépendant de la base de données.

---

## 📌 13. Criteria API

**Q : C'est quoi la Criteria API ?**

La *Criteria API* c'est une façon de construire des requêtes dynamiquement de manière type-safe, en Java, sans écrire de String JPQL. Utile quand les filtres sont optionnels et que la requête varie. Mais le code est assez verbeux. Avec Spring Data JPA on a aussi `Specification` qui est une surcouche plus pratique.

---

## 📌 14. CascadeType

**Q : C'est quoi le CascadeType dans Hibernate ?**

Le *cascade* permet de propager automatiquement certaines opérations d'une entité parente à ses enfants. Par exemple avec `CascadeType.PERSIST`, quand on sauvegarde un auteur, ses livres sont aussi sauvegardés automatiquement. Les principales valeurs :
- `PERSIST` : propage le save
- `MERGE` : propage le update
- `REMOVE` : propage le delete — attention à utiliser avec prudence !
- `REFRESH` : recharge les enfants depuis la base
- `ALL` : applique tout — très pratique mais peut être dangereux avec `REMOVE`

---

## 📌 15. OrphanRemoval

**Q : C'est quoi `orphanRemoval` ?**

`orphanRemoval = true` ça veut dire que si on retire un élément de la collection parent, Hibernate le supprime automatiquement en base. Par exemple si on retire un livre de la liste d'un auteur, le livre sera delete en base. C'est différent de `CascadeType.REMOVE` qui supprime les enfants quand le parent est supprimé.

---

## 📌 16. Le dirty checking

**Q : C'est quoi le dirty checking dans Hibernate ?**

Le *dirty checking* c'est le mécanisme par lequel Hibernate détecte automatiquement les modifications faites sur une entité *managed*. Quand la transaction se termine, Hibernate compare l'état actuel de l'entité avec l'état qu'il avait au moment du chargement (*snapshot*). Si c'est différent, il génère automatiquement un `UPDATE`. Donc on n'a pas besoin d'appeler `save()` explicitement si l'entité est déjà dans le contexte de persistance.

---

## 📌 17. flush() et clear()

**Q : À quoi servent `flush()` et `clear()` ?**

- `flush()` : force la synchro entre le contexte de persistance et la base de données *sans committer* la transaction. Utile si on veut que les changements soient visibles dans une requête SQL exécutée juste après
- `clear()` : vide le contexte de persistance, toutes les entités passent en état *detached*. Utile dans les traitements batch pour libérer la mémoire

---

## 📌 18. Les stratégies d'héritage

**Q : Quelles sont les stratégies d'héritage dans Hibernate ?**

Il y en a trois :
- `SINGLE_TABLE` : toutes les sous-classes dans une seule table, avec une colonne discriminante. C'est le plus performant en lecture, mais la table a beaucoup de colonnes nullables
- `TABLE_PER_CLASS` : chaque classe concrète a sa propre table avec toutes ses colonnes. Problème avec les requêtes polymorphiques
- `JOINED` : une table par classe avec des jointures. C'est plus propre en termes de modèle mais les requêtes font des JOIN

On choisit selon les besoins : SINGLE_TABLE pour les perfs, JOINED pour la propreté du schéma.

---

## 📌 19. @Embeddable et @Embedded

**Q : C'est quoi `@Embeddable` ?**

`@Embeddable` c'est pour créer des *value objects* — des objets Java qui ne sont pas des entités à part entière mais qui sont intégrés dans une entité. Par exemple une classe `Address` avec rue, ville, code postal. On l'annote `@Embeddable` et dans l'entité `User` on met `@Embedded Address address`. Les colonnes d'`Address` seront dans la même table que `User`.

---

## 📌 20. @Version et l'optimistic locking

**Q : C'est quoi l'optimistic locking ?**

L'*optimistic locking* c'est une stratégie pour gérer la concurrence. On ajoute un champ `@Version` dans l'entité — un numéro ou un timestamp. Quand deux utilisateurs lisent et modifient le même enregistrement, Hibernate vérifie au moment du save que la version n'a pas changé entre la lecture et l'écriture. Si quelqu'un d'autre a modifié entre-temps, une `OptimisticLockException` est lancée. C'est plus léger que le *pessimistic locking* qui pose des locks en base.

---

## 📌 21. Les transactions

**Q : Comment Hibernate gère les transactions ?**

Hibernate s'appuie sur les transactions de la base de données. Dans Spring Boot, on utilise `@Transactional` de Spring, qui démarre une transaction au début de la méthode et la commit ou rollback à la fin. Hibernate suit cette transaction. Si une `RuntimeException` est lancée, Spring fait un rollback automatique.

---

## 📌 22. Les propriétés importantes dans application.properties

**Q : Quelles sont les configs Hibernate importantes dans Spring Boot ?**

Les principales :
- `spring.jpa.hibernate.ddl-auto` : `create`, `update`, `validate`, `none` — définit ce qu'Hibernate fait avec le schéma au démarrage. En prod on utilise `validate` ou `none`
- `spring.jpa.show-sql=true` : pour afficher les requêtes SQL générées dans les logs
- `spring.jpa.properties.hibernate.format_sql=true` : pour formater le SQL affiché
- `spring.jpa.properties.hibernate.dialect` : pour préciser la base de données. Souvent auto-détecté avec Spring Boot

---

## 📌 23. Native Query

**Q : Quand on utilise une native query avec Hibernate ?**

Une *native query* c'est du SQL pur, pas du JPQL. On l'utilise quand on a besoin de fonctionnalités spécifiques à la base de données, des fonctions SQL avancées, ou quand les perfs sont critiques et qu'on veut contrôler exactement le SQL exécuté. Dans Spring Data JPA, on peut la définir avec `@Query(nativeQuery = true)`.

---

## 📌 24. Projection et DTO

**Q : C'est quoi une projection dans Hibernate/Spring Data JPA ?**

Au lieu de charger une entité complète avec tous ses champs, une *projection* ou *DTO* (Data Transfer Object) permet de sélectionner uniquement les colonnes dont on a besoin. C'est bien pour les perfs. Dans Spring Data JPA on peut faire ça avec des interfaces (interface-based projection) ou des classes (class-based projection avec `new` en JPQL).

---

## 📌 25. LazyInitializationException

**Q : C'est quoi une LazyInitializationException et comment on la résout ?**

C'est une exception qui se produit quand on essaie d'accéder à une collection LAZY *en dehors* de la session Hibernate — par exemple dans une couche de présentation après la fermeture de la transaction. Pour la résoudre :
- Utiliser `JOIN FETCH` ou `@EntityGraph` pour charger les données dans la requête
- Utiliser le pattern *Open Session in View* (mais c'est controversé car ça étend trop la session)
- Retourner des DTOs plutôt que des entités vers la couche de présentation — c'est l'approche recommandée

---

## 📌 26. @NamedQuery

**Q : C'est quoi `@NamedQuery` ?**

`@NamedQuery` permet de définir des requêtes JPQL directement sur l'entité, nommées et compilées au démarrage. L'avantage c'est que les erreurs de syntaxe sont détectées au boot et non à l'exécution. Aujourd'hui avec Spring Data JPA et `@Query`, on les utilise moins, mais ça reste valide.

---

## 📌 27. Pagination

**Q : Comment on fait de la pagination avec Hibernate/Spring Data JPA ?**

Dans Spring Data JPA, on utilise `Pageable` et `Page` dans les méthodes des repositories. On passe un objet `PageRequest.of(page, size, sort)` et le framework génère automatiquement les requêtes avec `LIMIT` et `OFFSET`. Hibernate traduit ça en SQL selon la base de données utilisée.

---

## 📌 28. Performance et bonnes pratiques

**Q : Quelles sont les bonnes pratiques de performance avec Hibernate ?**

Les principales :
- Toujours préférer le *fetch LAZY* pour les collections
- Utiliser `JOIN FETCH` ou `@EntityGraph` pour les cas où on a besoin des données liées
- Éviter le problème N+1 — activer les logs SQL en dev pour le détecter
- Utiliser des *projections/DTOs* pour les requêtes en lecture seule
- Utiliser `@BatchSize` ou les *batch fetching* pour optimiser les chargements multiples
- Ne pas abuser de `CascadeType.ALL` sans réfléchir
- En batch processing, appeler régulièrement `flush()` et `clear()` pour éviter les `OutOfMemoryError`

---

## 📌 29. Spring Data JPA

**Q : C'est quoi Spring Data JPA et son rapport avec Hibernate ?**

Spring Data JPA c'est une couche au-dessus de JPA/Hibernate qui simplifie encore plus la persistance. Il génère automatiquement l'implémentation des repositories à partir de simples interfaces. On peut déclarer des méthodes comme `findByNameAndAge()` et Spring Data JPA génère la requête tout seul. Hibernate reste le provider JPA en dessous, Spring Data JPA s'occupe juste de la plomberie.

---

## 📌 30. Les pièges classiques à l'entretien

**Q : Quels sont les erreurs classiques avec Hibernate ?**

Les plus fréquentes :
- **N+1 problem** : charger des collections en boucle sans `JOIN FETCH`
- **LazyInitializationException** : accéder à une relation LAZY hors session
- **Equals/hashCode mal implémentés** : sur les entités, ne pas utiliser l'ID généré dans `equals()` car il peut être null avant le premier save. Utiliser un UUID naturel ou une clé métier
- **Utiliser `@Transactional` sur des méthodes private** : Spring AOP ne peut pas intercepter les méthodes privées, la transaction ne sera pas créée
- **ddl-auto=create en prod** : catastrophique, ça recrée le schéma à chaque démarrage

---

*Bonne chance pour ton entretien ! 🚀*
