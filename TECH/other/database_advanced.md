# PostgreSQL — Support complet de révision SQL, performance et transactions

> **Objectif :** réviser les notions SQL travaillées avec les tables `utilisateur` et `commande`, comprendre les plans d’exécution PostgreSQL, les index, les vues, les transactions, les niveaux d’isolation et les verrous.
>
> Le support reprend et réorganise les exemples dispersés dans les fichiers d’entraînement et dans la discussion, en corrigeant les petites erreurs de syntaxe lorsque nécessaire.

---

## Sommaire

1. [Jeu de données et modèle](#1-jeu-de-données-et-modèle)
2. [SELECT, filtres, tri et jointures](#2-select-filtres-tri-et-jointures)
3. [Agrégations, GROUP BY et HAVING](#3-agrégations-group-by-et-having)
4. [Dates, EXTRACT et CASE](#4-dates-extract-et-case)
5. [Sous-requêtes](#5-sous-requêtes)
6. [EXISTS, NOT EXISTS, IN, ANY et ALL](#6-exists-not-exists-in-any-et-all)
7. [Fonctions fenêtre : ROW_NUMBER](#7-fonctions-fenêtre--row_number)
8. [Index](#8-index)
9. [Plans d’exécution : EXPLAIN / EXPLAIN ANALYZE](#9-plans-dexécution--explain--explain-analyze)
10. [Vues et vues matérialisées](#10-vues-et-vues-matérialisées)
11. [Transactions et ACID](#11-transactions-et-acid)
12. [Niveaux d’isolation](#12-niveaux-disolation)
13. [Locks / verrous](#13-locks--verrous)
14. [DELETE vs TRUNCATE](#14-delete-vs-truncate)
15. [Fiche entretien](#15-fiche-entretien)
16. [Exercices de révision avec solutions](#16-exercices-de-révision-avec-solutions)

---

# 1. Jeu de données et modèle

## 1.1 Tables utilisées

Nous travaillons principalement avec deux tables :

```sql
CREATE TABLE utilisateur (
    id INTEGER PRIMARY KEY,
    prenom VARCHAR(50) NOT NULL,
    nom VARCHAR(50) NOT NULL,
    email VARCHAR(120) NOT NULL UNIQUE,
    ville VARCHAR(80) NOT NULL
);

CREATE TABLE commande (
    utilisateur_id INTEGER NOT NULL,
    date_achat DATE NOT NULL,
    num_facture VARCHAR(20) NOT NULL PRIMARY KEY,
    prix_total DECIMAL(10, 2) NOT NULL
);
```

Relation logique :

```text
utilisateur
-----------
id PK
prenom
nom
email UNIQUE
ville
     1
     │
     │
     │ N
commande
--------
utilisateur_id
date_achat
num_facture PK
prix_total
```

Dans le script d’entraînement original, `commande.utilisateur_id` n’a volontairement **pas** de contrainte `FOREIGN KEY`.

Cela permet notamment d’avoir cette ligne :

```sql
(99, '2013-12-20', 'A00135', 55.55)
```

alors qu’aucun utilisateur `99` n’existe.

C’est utile pour apprendre à détecter une commande orpheline avec un `LEFT JOIN`.

En production, on définirait normalement :

```sql
ALTER TABLE commande
ADD CONSTRAINT fk_commande_utilisateur
FOREIGN KEY (utilisateur_id)
REFERENCES utilisateur(id);
```

Mais il faudrait auparavant supprimer ou corriger les lignes orphelines.

---

## 1.2 Clés importantes

### `PRIMARY KEY`

```sql
id INTEGER PRIMARY KEY
```

Une clé primaire :

- identifie une ligne de manière unique ;
- interdit les doublons ;
- interdit `NULL` ;
- crée automatiquement un index unique sous PostgreSQL.

### `UNIQUE`

```sql
email VARCHAR(120) UNIQUE
```

Garantit l’unicité de l’email et crée également un index unique sous PostgreSQL.

### `FOREIGN KEY`

```sql
FOREIGN KEY (utilisateur_id)
REFERENCES utilisateur(id)
```

Garantit l’intégrité référentielle :

```text
commande.utilisateur_id
        ↓
doit exister dans
        ↓
utilisateur.id
```

---

# 2. SELECT, filtres, tri et jointures

## 2.1 SELECT simple

```sql
SELECT *
FROM utilisateur;
```

Sélection de colonnes précises :

```sql
SELECT id, nom, prenom
FROM utilisateur;
```

Alias :

```sql
SELECT
    nom AS nom_utilisateur,
    prenom AS prenom_utilisateur
FROM utilisateur;
```

Concaténation PostgreSQL :

```sql
SELECT
    CONCAT(nom, ' ', prenom) AS utilisateur
FROM utilisateur;
```

---

## 2.2 WHERE

```sql
SELECT *
FROM commande
WHERE prix_total > 200;
```

Plusieurs conditions :

```sql
SELECT *
FROM commande
WHERE prix_total > 200
  AND date_achat >= '2013-06-01';
```

Avec `OR` :

```sql
SELECT *
FROM utilisateur
WHERE ville = 'Paris'
   OR ville = 'Lyon';
```

---

## 2.3 IN

Même requête, plus lisible :

```sql
SELECT *
FROM utilisateur
WHERE ville IN ('Paris', 'Lyon');
```

---

## 2.4 LIKE

Nom commençant par `M` :

```sql
SELECT *
FROM utilisateur
WHERE nom LIKE 'M%';
```

Rappels :

```text
'M%'    → commence par M
'%M'    → finit par M
'%mar%' → contient "mar"
'___'   → exactement 3 caractères avec _
```

PostgreSQL propose aussi `ILIKE` pour une recherche insensible à la casse :

```sql
SELECT *
FROM utilisateur
WHERE nom ILIKE 'm%';
```

---

## 2.5 ORDER BY

```sql
SELECT *
FROM commande
ORDER BY prix_total DESC;
```

Plusieurs colonnes :

```sql
SELECT *
FROM utilisateur
ORDER BY ville ASC, nom ASC;
```

---

## 2.6 DISTINCT

Si une jointure retourne plusieurs lignes pour le même utilisateur :

```sql
SELECT DISTINCT
    u.id,
    u.nom,
    u.prenom
FROM utilisateur u
JOIN commande c
    ON c.utilisateur_id = u.id
WHERE c.prix_total > 500;
```

`DISTINCT` supprime les lignes identiques du résultat.

À ne pas confondre avec `GROUP BY` :

```text
DISTINCT
→ supprimer les doublons du résultat

GROUP BY
→ former des groupes pour faire des agrégations
```

---

## 2.7 INNER JOIN

Afficher les commandes avec leur utilisateur :

```sql
SELECT
    c.num_facture,
    c.date_achat,
    c.prix_total,
    u.prenom,
    u.nom
FROM commande c
INNER JOIN utilisateur u
    ON u.id = c.utilisateur_id;
```

`INNER JOIN` ne conserve que les lignes ayant une correspondance des deux côtés.

La commande de `utilisateur_id = 99` disparaît ici.

---

## 2.8 LEFT JOIN

Afficher toutes les commandes, même celles sans utilisateur correspondant :

```sql
SELECT
    c.num_facture,
    c.utilisateur_id,
    u.prenom,
    u.nom,
    c.prix_total
FROM commande c
LEFT JOIN utilisateur u
    ON u.id = c.utilisateur_id;
```

Pour la commande de l’utilisateur `99` :

```text
u.prenom = NULL
u.nom    = NULL
```

---

## 2.9 Détecter les utilisateurs sans commande

```sql
SELECT
    u.id,
    u.prenom,
    u.nom
FROM utilisateur u
LEFT JOIN commande c
    ON c.utilisateur_id = u.id
WHERE c.num_facture IS NULL;
```

Principe :

```text
tous les utilisateurs
       ↓ LEFT JOIN
commandes si elles existent
       ↓
WHERE commande IS NULL
       ↓
utilisateurs sans commande
```

---

## 2.10 Détecter les commandes orphelines

```sql
SELECT c.*
FROM commande c
LEFT JOIN utilisateur u
    ON u.id = c.utilisateur_id
WHERE u.id IS NULL;
```

Dans le jeu de données, cette requête trouve la commande dont :

```text
utilisateur_id = 99
```

---

# 3. Agrégations, GROUP BY et HAVING

## 3.1 Fonctions d’agrégation

Les principales :

```sql
COUNT(...)
SUM(...)
AVG(...)
MIN(...)
MAX(...)
```

Exemple :

```sql
SELECT
    COUNT(*) AS nombre_commandes,
    SUM(prix_total) AS total,
    AVG(prix_total) AS moyenne,
    MIN(prix_total) AS minimum,
    MAX(prix_total) AS maximum
FROM commande;
```

---

## 3.2 À quoi sert GROUP BY ?

`GROUP BY` rassemble les lignes ayant les mêmes valeurs pour une ou plusieurs colonnes.

Exemple : total des commandes par utilisateur.

```sql
SELECT
    u.id,
    u.nom,
    u.prenom,
    SUM(c.prix_total) AS total_commandes
FROM utilisateur u
JOIN commande c
    ON c.utilisateur_id = u.id
GROUP BY u.id, u.nom, u.prenom;
```

Mentalement :

```text
Aimee
├── 203.14
├── 124.00
└── 42.42
    ↓ SUM
  369.56

Esmee
├── 149.45
├── 235.35
└── 15.00
    ↓ SUM
  399.80
```

---

## 3.3 Pourquoi `GROUP BY c.prix_total` était faux ?

Requête incorrecte pour obtenir la somme par utilisateur :

```sql
SELECT
    u.nom,
    u.prenom,
    SUM(c.prix_total)
FROM utilisateur u
JOIN commande c
    ON u.id = c.utilisateur_id
GROUP BY c.prix_total;
```

Ici tu demandes :

> Regroupe les commandes qui ont le même **prix**.

Alors que ton besoin est :

> Regroupe les commandes qui appartiennent au même **utilisateur**.

Il faut donc :

```sql
GROUP BY u.id, u.nom, u.prenom;
```

---

## 3.4 GROUP BY avec plusieurs colonnes

Oui, c’est très courant :

```sql
GROUP BY u.id, u.nom, u.prenom
```

ou :

```sql
GROUP BY
    EXTRACT(YEAR FROM date_achat),
    EXTRACT(MONTH FROM date_achat)
```

Le groupe correspond alors à une combinaison :

```text
2013 + janvier
2013 + février
2013 + mars
...
```

### Règle pratique

Dans une requête agrégée, une colonne du `SELECT` doit généralement être :

- soit dans une fonction d’agrégation ;
- soit dans le `GROUP BY`.

Exemple :

```sql
SELECT
    u.nom,                     -- GROUP BY
    u.prenom,                  -- GROUP BY
    SUM(c.prix_total)          -- agrégation
FROM ...
GROUP BY u.nom, u.prenom;
```

> PostgreSQL accepte certains cas particuliers basés sur une dépendance fonctionnelle, notamment autour de la clé primaire. Pour un SQL clair et portable, mets explicitement les colonnes non agrégées dans le `GROUP BY`.

---

## 3.5 Utilisateurs avec zéro commande

Pour garder les utilisateurs n’ayant aucune commande :

```sql
SELECT
    u.id,
    u.nom,
    u.prenom,
    COALESCE(SUM(c.prix_total), 0) AS total_commandes
FROM utilisateur u
LEFT JOIN commande c
    ON c.utilisateur_id = u.id
GROUP BY u.id, u.nom, u.prenom;
```

Pourquoi `COALESCE` ?

```sql
COALESCE(SUM(c.prix_total), 0)
```

signifie :

```text
si SUM(...) est NULL
→ retourne 0
```

---

## 3.6 WHERE vs HAVING

`WHERE` filtre les lignes **avant** le regroupement.

`HAVING` filtre les groupes **après** le `GROUP BY`.

Exemple :

```sql
SELECT
    u.id,
    u.nom,
    u.prenom,
    SUM(c.prix_total) AS total
FROM utilisateur u
JOIN commande c
    ON c.utilisateur_id = u.id
WHERE u.ville <> 'Paris'
GROUP BY u.id, u.nom, u.prenom
HAVING SUM(c.prix_total) >= 300;
```

Ordre logique :

```text
FROM / JOIN
    ↓
WHERE
    ↓
GROUP BY
    ↓
HAVING
    ↓
SELECT
    ↓
DISTINCT
    ↓
ORDER BY
    ↓
LIMIT
```

Exemple classique :

```sql
SELECT
    u.ville,
    COUNT(c.num_facture) AS nombre_commandes
FROM utilisateur u
JOIN commande c
    ON c.utilisateur_id = u.id
GROUP BY u.ville
HAVING COUNT(c.num_facture) > 2;
```

---

# 4. Dates, EXTRACT et CASE

## 4.1 Extraire l’année sous PostgreSQL

MySQL accepte :

```sql
YEAR(date_achat)
```

PostgreSQL utilise plutôt :

```sql
EXTRACT(YEAR FROM date_achat)
```

Exemple :

```sql
SELECT
    EXTRACT(YEAR FROM date_achat) AS annee
FROM commande;
```

---

## 4.2 Extraire le mois

```sql
SELECT
    EXTRACT(MONTH FROM date_achat) AS mois
FROM commande;
```

Résultat :

```text
1
2
3
...
12
```

---

## 4.3 Afficher le nom du mois avec CASE

```sql
SELECT
    CASE EXTRACT(MONTH FROM c.date_achat)
        WHEN 1 THEN 'January'
        WHEN 2 THEN 'February'
        WHEN 3 THEN 'March'
        WHEN 4 THEN 'April'
        WHEN 5 THEN 'May'
        WHEN 6 THEN 'June'
        WHEN 7 THEN 'July'
        WHEN 8 THEN 'August'
        WHEN 9 THEN 'September'
        WHEN 10 THEN 'October'
        WHEN 11 THEN 'November'
        WHEN 12 THEN 'December'
    END AS mois
FROM commande c;
```

Le `CASE` est particulièrement utile lorsque tu veux une correspondance personnalisée.

---

## 4.4 Alternative PostgreSQL : TO_CHAR

```sql
SELECT
    TO_CHAR(date_achat, 'FMMonth') AS mois
FROM commande;
```

Exemples :

```text
FMMonth → January
FMMONTH → JANUARY
FMMon   → Jan
```

---

## 4.5 Total des ventes par année et mois

```sql
SELECT
    EXTRACT(YEAR FROM date_achat) AS annee,
    EXTRACT(MONTH FROM date_achat) AS mois,
    COUNT(*) AS nombre_commandes,
    SUM(prix_total) AS total_mois
FROM commande
GROUP BY
    EXTRACT(YEAR FROM date_achat),
    EXTRACT(MONTH FROM date_achat)
ORDER BY annee, mois;
```

---

# 5. Sous-requêtes

Une sous-requête est une requête SQL imbriquée dans une autre.

---

## 5.1 Sous-requête scalaire indépendante

Question :

> Afficher les commandes dont le montant est supérieur à la moyenne de toutes les commandes.

```sql
SELECT *
FROM commande
WHERE prix_total > (
    SELECT AVG(prix_total)
    FROM commande
);
```

La sous-requête :

```sql
SELECT AVG(prix_total)
FROM commande
```

retourne une seule valeur.

Conceptuellement :

```text
AVG = 220
   ↓
WHERE prix_total > 220
```

Cette sous-requête ne dépend pas de la requête principale.

---

## 5.2 Sous-requête corrélée

Question :

> Pour chaque utilisateur, afficher sa commande la plus chère et sa date.

```sql
SELECT
    CONCAT(u.nom, ' ', u.prenom) AS utilisateur,
    c.date_achat,
    c.prix_total
FROM utilisateur u
JOIN commande c
    ON c.utilisateur_id = u.id
WHERE c.prix_total = (
    SELECT MAX(c2.prix_total)
    FROM commande c2
    WHERE c2.utilisateur_id = u.id
);
```

La sous-requête dépend de :

```sql
u.id
```

qui appartient à la requête extérieure.

C’est donc une **sous-requête corrélée**.

---

## 5.3 Pourquoi `c2` et pas `c` ?

Dans la requête extérieure :

```sql
commande c
```

représente la commande actuellement testée.

Dans la sous-requête :

```sql
commande c2
```

représente une **nouvelle lecture** de la table `commande` afin de chercher toutes les commandes de l’utilisateur et calculer son maximum.

```text
c  → commande courante à tester
c2 → commandes utilisées pour calculer MAX
u  → utilisateur courant de la requête extérieure
```

On garde `u` parce que la sous-requête ne relit pas la table `utilisateur`. Elle réutilise simplement l’utilisateur courant.

---

## 5.4 Cas d’égalité sur le maximum

La requête :

```sql
WHERE c.prix_total = (
    SELECT MAX(...)
)
```

peut retourner plusieurs lignes pour un utilisateur si deux commandes ont exactement le même prix maximum.

Exemple :

```text
Utilisateur 5
500 €
500 €
200 €
```

Les deux commandes à `500` seront retournées.

Si tu veux strictement une seule ligne, il faut définir une règle supplémentaire, par exemple la date la plus récente, et utiliser `ROW_NUMBER()`.

---

# 6. EXISTS, NOT EXISTS, IN, ANY et ALL

## 6.1 EXISTS

Question :

> Afficher les utilisateurs ayant au moins une commande supérieure à 500 €.

```sql
SELECT
    u.id,
    u.nom,
    u.prenom
FROM utilisateur u
WHERE EXISTS (
    SELECT 1
    FROM commande c
    WHERE c.utilisateur_id = u.id
      AND c.prix_total > 500
);
```

`EXISTS` pose simplement la question :

> Existe-t-il au moins une ligne qui respecte la condition ?

Le contenu de `SELECT` n’est pas important pour l’existence, d’où :

```sql
SELECT 1
```

---

## 6.2 Pourquoi EXISTS est naturel ici ?

Le besoin métier est :

> au moins une commande existe.

`EXISTS` exprime directement cette intention.

Une alternative équivalente avec `JOIN` :

```sql
SELECT DISTINCT
    u.id,
    u.nom,
    u.prenom
FROM utilisateur u
JOIN commande c
    ON c.utilisateur_id = u.id
WHERE c.prix_total > 500;
```

Le `DISTINCT` est nécessaire si un utilisateur possède plusieurs commandes > 500.

---

## 6.3 NOT EXISTS

Question :

> Afficher les utilisateurs sans aucune commande.

```sql
SELECT
    u.id,
    u.nom,
    u.prenom
FROM utilisateur u
WHERE NOT EXISTS (
    SELECT 1
    FROM commande c
    WHERE c.utilisateur_id = u.id
);
```

À retenir :

```text
EXISTS     → au moins une ligne
NOT EXISTS → aucune ligne
```

---

## 6.4 IN

Même idée :

```sql
SELECT
    u.id,
    u.nom,
    u.prenom
FROM utilisateur u
WHERE u.id IN (
    SELECT c.utilisateur_id
    FROM commande c
    WHERE c.prix_total > 500
);
```

Les optimiseurs modernes peuvent produire des plans très proches pour `IN`, `EXISTS` ou certaines jointures.

Choisis surtout la forme qui exprime le mieux l’intention.

---

## 6.5 ANY

Question :

> Afficher les commandes dont le prix est supérieur à au moins une commande de l’utilisateur 1.

```sql
SELECT
    c.num_facture,
    c.prix_total
FROM commande c
WHERE c.prix_total > ANY (
    SELECT c2.prix_total
    FROM commande c2
    WHERE c2.utilisateur_id = 1
);
```

L’utilisateur 1 possède :

```text
203.14
124.00
42.42
```

`> ANY` signifie :

> supérieur à **au moins une** de ces valeurs.

Il suffit donc ici d’être supérieur à `42.42`.

Pour un ensemble non vide sans `NULL`, on peut retenir :

```text
x > ANY (...)  ≈ x > MIN(...)
```

---

## 6.6 ALL

Question :

> Afficher les commandes dont le prix est supérieur à toutes les commandes de l’utilisateur 1.

```sql
SELECT
    c.num_facture,
    c.prix_total
FROM commande c
WHERE c.prix_total > ALL (
    SELECT c2.prix_total
    FROM commande c2
    WHERE c2.utilisateur_id = 1
);
```

Il faut être supérieur à :

```text
203.14
124.00
42.42
```

donc supérieur à `203.14`.

Pour un ensemble non vide sans `NULL` :

```text
x > ALL (...) ≈ x > MAX(...)
```

---

## 6.7 `= ANY` et `IN`

```sql
u.id = ANY (
    SELECT utilisateur_id
    FROM commande
)
```

est proche de :

```sql
u.id IN (
    SELECT utilisateur_id
    FROM commande
)
```

---

## 6.8 Pièges avec ANY / ALL

Les raccourcis avec `MIN` et `MAX` sont pratiques pour raisonner, mais attention :

- un ensemble vide a une sémantique particulière ;
- la présence de `NULL` peut produire `UNKNOWN`.

À connaître :

```text
ANY sur ensemble vide → FALSE
ALL sur ensemble vide → TRUE
```

Pour les cas simples d’entretien, retiens surtout :

```text
ANY = au moins une valeur
ALL = toutes les valeurs
```

---

# 7. Fonctions fenêtre : ROW_NUMBER

Une fonction fenêtre permet de calculer une information par groupe **sans réduire immédiatement les lignes** comme le ferait `GROUP BY`.

Question :

> Pour chaque utilisateur, retourner une seule commande : la plus chère.

```sql
SELECT
    nom,
    prenom,
    date_achat,
    prix_total
FROM (
    SELECT
        u.nom,
        u.prenom,
        c.date_achat,
        c.prix_total,
        ROW_NUMBER() OVER (
            PARTITION BY u.id
            ORDER BY c.prix_total DESC
        ) AS rn
    FROM utilisateur u
    JOIN commande c
        ON c.utilisateur_id = u.id
) t
WHERE rn = 1;
```

Explication :

```sql
PARTITION BY u.id
```

crée un groupe logique par utilisateur.

```sql
ORDER BY c.prix_total DESC
```

classe les commandes de la plus chère à la moins chère.

```sql
ROW_NUMBER()
```

attribue :

```text
203.14 → 1
124.00 → 2
42.42  → 3
```

Puis :

```sql
WHERE rn = 1
```

garde la première.

### Ajouter une règle en cas d’égalité

```sql
ROW_NUMBER() OVER (
    PARTITION BY u.id
    ORDER BY c.prix_total DESC, c.date_achat DESC
)
```

Ici, si deux commandes ont le même prix, on garde la plus récente.

---

# 8. Index

## 8.1 Pourquoi un index ?

Sans index :

```text
table
↓
parcourir potentiellement toutes les lignes
```

Avec un index adapté :

```text
valeur recherchée
↓
index
↓
localisation des lignes
```

Un index peut accélérer :

- `WHERE`;
- `JOIN`;
- certains `ORDER BY`;
- certaines agrégations ;
- vérification d’unicité ;
- contrôles de clés étrangères côté table enfant lors de modifications du parent.

Mais un index :

- occupe de l’espace ;
- ralentit les `INSERT`, `UPDATE`, `DELETE`;
- n’est pas toujours utilisé.

---

## 8.2 PRIMARY KEY

```sql
id INTEGER PRIMARY KEY
```

PostgreSQL crée automatiquement un index unique.

Tu n’as donc normalement pas besoin de :

```sql
CREATE INDEX idx_utilisateur_id
ON utilisateur(id);
```

Ce serait redondant.

---

## 8.3 UNIQUE

```sql
email VARCHAR(120) UNIQUE
```

crée également un index unique.

Il sert à :

- vérifier rapidement l’unicité ;
- accélérer une recherche comme :

```sql
SELECT *
FROM utilisateur
WHERE email = 'aime.marechal@example.com';
```

---

## 8.4 FOREIGN KEY

Important sous PostgreSQL :

> La colonne référencée, souvent la `PRIMARY KEY`, est indexée ; la colonne de clé étrangère côté enfant n’est pas automatiquement indexée.

Exemple :

```text
utilisateur.id
→ PRIMARY KEY
→ index automatique

commande.utilisateur_id
→ FOREIGN KEY
→ pas d’index automatique PostgreSQL
```

On ajoute souvent :

```sql
CREATE INDEX idx_commande_utilisateur
ON commande(utilisateur_id);
```

Cela aide :

```sql
SELECT *
FROM commande
WHERE utilisateur_id = 5;
```

et :

```sql
SELECT *
FROM utilisateur u
JOIN commande c
    ON c.utilisateur_id = u.id;
```

Cela aide aussi PostgreSQL lorsqu’il doit vérifier les références pendant certaines opérations sur la table parent.

---

## 8.5 Index sur `prix_total`

```sql
CREATE INDEX idx_commande_prix
ON commande(prix_total);
```

Peut aider :

```sql
SELECT *
FROM commande
WHERE prix_total > 500;
```

Mais avec seulement 33 lignes, PostgreSQL peut préférer un `Seq Scan`.

C’est normal.

---

## 8.6 Index composite

Pour la sous-requête :

```sql
WHERE c2.utilisateur_id = u.id
```

puis :

```sql
MAX(c2.prix_total)
```

un index intéressant peut être :

```sql
CREATE INDEX idx_commande_user_prix
ON commande(utilisateur_id, prix_total DESC);
```

Cet ordre permet de regrouper les entrées d’un utilisateur puis d’avoir les prix ordonnés.

---

## 8.7 Importance de l’ordre des colonnes

Index :

```sql
CREATE INDEX idx_user_date
ON commande(utilisateur_id, date_achat);
```

Très utile pour :

```sql
WHERE utilisateur_id = 5
  AND date_achat = '2013-03-02'
```

et souvent pour :

```sql
WHERE utilisateur_id = 5
```

Mais beaucoup moins adapté à :

```sql
WHERE date_achat = '2013-03-02'
```

seul.

Règle simplifiée :

> Dans un index B-tree composite, l’ordre des colonnes est important.

---

## 8.8 Sélectivité

Un index est particulièrement intéressant lorsqu’une condition retourne une petite partie des lignes.

Exemple :

```text
10 000 000 lignes
→ 30 lignes recherchées
```

L’index peut être très efficace.

Mais si la requête récupère :

```text
8 000 000 lignes sur 10 000 000
```

un `Seq Scan` peut être meilleur.

---

# 9. Plans d’exécution : EXPLAIN / EXPLAIN ANALYZE

## 9.1 Pourquoi analyser un plan ?

Quand une requête est lente, on veut savoir :

- comment PostgreSQL accède aux tables ;
- s’il utilise un index ;
- quel type de jointure il choisit ;
- combien de lignes il estime ;
- combien de lignes il retourne réellement ;
- quelles étapes coûtent le plus cher ;
- quelles étapes sont répétées.

---

## 9.2 EXPLAIN

```sql
EXPLAIN
SELECT *
FROM commande
WHERE prix_total > 500;
```

`EXPLAIN` **n’exécute pas réellement** le `SELECT`.

Il montre le plan estimé.

Exemple observé dans le fichier d’entraînement :

```text
Seq Scan on commande  (cost=0.00..19.25 rows=247 width=82)
  Filter: (prix_total > '500'::numeric)
```

---

## 9.3 Pourquoi `rows=247` alors qu’il n’y avait que 33 lignes ?

Parce que :

```text
rows=247
```

dans `EXPLAIN` est une **estimation**, pas le nombre réel de lignes.

Si la table vient d’être créée ou si les statistiques sont mauvaises, les estimations peuvent être très éloignées.

Mettre à jour les statistiques :

```sql
ANALYZE commande;
ANALYZE utilisateur;
```

`ANALYZE commande` signifie :

> recalcule les statistiques utilisées par l’optimiseur.

À ne pas confondre avec :

```sql
EXPLAIN ANALYZE ...
```

---

## 9.4 EXPLAIN ANALYZE

```sql
EXPLAIN ANALYZE
SELECT *
FROM commande
WHERE prix_total > 500;
```

Cette fois, PostgreSQL **exécute réellement** la requête.

Exemple réel :

```text
Seq Scan on commande
(cost=0.00..19.25 rows=247 width=82)
(actual time=0.009..0.011 rows=4 loops=1)

Filter: (prix_total > '500'::numeric)
Rows Removed by Filter: 29

Planning Time: 0.039 ms
Execution Time: 0.018 ms
```

---

## 9.5 Lire `Seq Scan`

```text
Seq Scan on commande
```

signifie :

> parcourir séquentiellement la table.

Avec 33 lignes, c’est parfaitement acceptable.

---

## 9.6 Lire `cost`

```text
cost=0.00..19.25
```

Le premier nombre correspond au coût estimé avant de commencer à produire les résultats.

Le deuxième correspond au coût total estimé.

Important :

```text
cost ≠ millisecondes
```

Ce sont des unités internes utilisées par l’optimiseur pour comparer les plans.

---

## 9.7 Lire `rows`

Dans :

```text
cost=... rows=247
```

`rows=247` = estimation.

Dans :

```text
actual ... rows=4
```

`rows=4` = résultat réel.

Comparer :

```text
estimé : 247
réel   : 4
```

permet de détecter une mauvaise estimation.

---

## 9.8 Lire `width`

```text
width=82
```

est la taille moyenne estimée d’une ligne, en octets.

Ce n’est généralement pas le premier indicateur à analyser en entretien.

---

## 9.9 Lire `actual time`

```text
actual time=0.009..0.011
```

Le premier nombre :

```text
0.009 ms
```

≈ temps jusqu’à la première ligne produite par ce nœud.

Le deuxième :

```text
0.011 ms
```

≈ temps jusqu’à la fin de ce nœud.

---

## 9.10 `loops`

```text
loops=1
```

signifie que le nœud a été exécuté une fois.

Si :

```text
loops=100000
```

une opération qui semble très rapide par exécution peut devenir coûteuse au total.

### Détail PostgreSQL important

Quand `loops > 1`, les valeurs :

```text
actual time
rows
Rows Removed by Filter
```

sont généralement affichées **par boucle / en moyenne par boucle**.

Il faut donc prendre `loops` en compte pour évaluer le travail cumulé.

---

## 9.11 Rows Removed by Filter

```text
Rows Removed by Filter: 29
```

signifie :

```text
33 lignes lues
├── 4 gardées
└── 29 rejetées
```

Sur 33 lignes, aucun problème.

Sur 50 millions de lignes pour n’en garder que 10, cela devient un signal important.

---

## 9.12 Pourquoi l’index n’est pas utilisé ?

Après :

```sql
CREATE INDEX idx_commande_prix
ON commande(prix_total);
```

le plan observé reste :

```text
Seq Scan on commande
(cost=0.00..1.41 rows=11 width=82)
(actual time=0.010..0.012 rows=4 loops=1)
```

Pourquoi ?

Parce que la table est très petite.

PostgreSQL compare :

```text
Seq Scan
→ lire directement 33 lignes
```

à :

```text
Index Scan
→ parcourir l’index
→ retrouver les références
→ accéder aux lignes de la table
```

Pour 33 lignes, le `Seq Scan` peut coûter moins cher.

Règle fondamentale :

```text
Seq Scan  ≠ mauvais automatiquement
Index Scan ≠ meilleur automatiquement
```

---

## 9.13 Tester pédagogiquement sans Seq Scan

Pour comprendre le comportement de l’index, on peut temporairement faire :

```sql
SET enable_seqscan = off;
```

Puis :

```sql
EXPLAIN ANALYZE
SELECT *
FROM commande
WHERE prix_total > 500;
```

Et remettre :

```sql
SET enable_seqscan = on;
```

> À utiliser pour apprendre ou diagnostiquer, pas comme « optimisation » permanente.

---

## 9.14 Index Scan

Exemple possible :

```text
Index Scan using idx_commande_prix on commande
  Index Cond: (prix_total > 500)
```

`Index Cond` indique que la condition est utilisée directement pour naviguer dans l’index.

---

## 9.15 Index Only Scan

```text
Index Only Scan
```

PostgreSQL peut parfois répondre en utilisant principalement l’index sans récupérer toutes les colonnes dans la table.

Cela dépend notamment :

- des colonnes demandées ;
- de la visibilité MVCC ;
- de l’état de la visibility map.

---

## 9.16 Bitmap Scan

On peut rencontrer :

```text
Bitmap Index Scan
        ↓
Bitmap Heap Scan
```

Souvent utile lorsque plusieurs lignes correspondent à une condition : PostgreSQL récupère d’abord les emplacements via l’index, puis lit efficacement les pages de la table.

---

## 9.17 Hash Join

Plan observé sur la requête « maximum par utilisateur » :

```text
Hash Join
  Hash Cond: (...)
  -> Seq Scan on commande c
  -> Hash
       -> Seq Scan on utilisateur u
```

Principe simplifié :

```text
petite table utilisateur
       ↓
construction d’une table de hash
       ↓
parcours de commande
       ↓
recherche rapide par clé
```

Un `Hash Join` est fréquent pour une jointure d’égalité :

```sql
u.id = c.utilisateur_id
```

---

## 9.18 Nested Loop

```text
Nested Loop
```

Principe :

```text
pour chaque ligne de A
    chercher les lignes correspondantes dans B
```

Avec un bon index :

```text
Seq Scan utilisateur
    ↓
pour chaque utilisateur
    ↓
Index Scan commande(utilisateur_id)
```

peut être très performant.

Mais si la partie interne est exécutée énormément de fois sans index adapté, cela peut être coûteux.

---

## 9.19 Merge Join

```text
Merge Join
```

Les deux côtés sont triés selon la clé de jointure, puis parcourus de manière coordonnée.

Il peut être intéressant lorsque les données sont déjà triées ou peuvent être obtenues efficacement dans l’ordre.

---

## 9.20 HashAggregate

Avec :

```sql
SELECT
    utilisateur_id,
    SUM(prix_total)
FROM commande
GROUP BY utilisateur_id;
```

on peut voir :

```text
HashAggregate
  Group Key: utilisateur_id
  -> Seq Scan on commande
```

Cela correspond à :

```text
lire
↓
regrouper par utilisateur
↓
calculer SUM
```

---

## 9.21 Sort

Avec :

```sql
SELECT *
FROM commande
ORDER BY prix_total DESC;
```

on peut voir :

```text
Sort
  Sort Key: prix_total DESC
  -> Seq Scan on commande
```

Un gros tri sur beaucoup de lignes peut coûter cher.

---

## 9.22 EXPLAIN (ANALYZE, BUFFERS)

Pour une analyse plus sérieuse :

```sql
EXPLAIN (ANALYZE, BUFFERS)
SELECT *
FROM commande
WHERE prix_total > 500;
```

Exemple d’information :

```text
Buffers: shared hit=10 read=3
```

Simplification :

```text
shared hit
→ bloc déjà disponible dans le cache

read
→ bloc lu depuis le stockage par PostgreSQL
```

---

## 9.23 Analyser le vrai plan de la sous-requête corrélée

Requête :

```sql
SELECT
    CONCAT(u.nom, ' ', u.prenom) AS utilisateur,
    CASE EXTRACT(MONTH FROM c.date_achat)
        WHEN 1 THEN 'January'
        WHEN 2 THEN 'February'
        WHEN 3 THEN 'March'
        WHEN 4 THEN 'April'
        WHEN 5 THEN 'May'
        WHEN 6 THEN 'June'
        WHEN 7 THEN 'July'
        WHEN 8 THEN 'August'
        WHEN 9 THEN 'September'
        WHEN 10 THEN 'October'
        WHEN 11 THEN 'November'
        WHEN 12 THEN 'December'
    END AS mois,
    c.prix_total
FROM utilisateur u
JOIN commande c
    ON u.id = c.utilisateur_id
WHERE c.prix_total = (
    SELECT MAX(c2.prix_total)
    FROM commande c2
    WHERE c2.utilisateur_id = u.id
);
```

Plan réel observé :

```text
Hash Join
  (actual ... rows=15 loops=1)

  -> Seq Scan on commande c
       (actual ... rows=33 loops=1)

  -> Hash
       -> Seq Scan on utilisateur u
            (actual ... rows=15 loops=1)

       SubPlan 1
         -> Aggregate
              (actual ... rows=1 loops=30)

              -> Seq Scan on commande c2
                   (actual ... rows=2 loops=30)
                   Filter: (utilisateur_id = u.id)
                   Rows Removed by Filter: 31
```

### Ce qu’il faut remarquer

1. `commande c` est lue une fois.
2. `utilisateur u` est lu une fois puis placé dans une structure de hash.
3. Le `SubPlan 1` correspond à :

```sql
SELECT MAX(c2.prix_total)
FROM commande c2
WHERE c2.utilisateur_id = u.id
```

4. Le plan indique :

```text
loops=30
```

donc ce sous-plan est évalué plusieurs fois dans cette stratégie d’exécution.

5. À chaque boucle, le `Seq Scan` sur `c2` élimine en moyenne beaucoup de lignes :

```text
Rows Removed by Filter: 31
```

Sur 33 lignes, c’est négligeable.

Sur des millions de lignes, on examinerait :

```sql
CREATE INDEX idx_commande_user_prix
ON commande(utilisateur_id, prix_total DESC);
```

ou une réécriture avec `ROW_NUMBER()`.

> Le nombre exact de boucles dépend du plan choisi par PostgreSQL. Le point important est de repérer un sous-plan corrélé exécuté plusieurs fois.

---

## 9.24 Planning Time vs Execution Time

```text
Planning Time: 0.269 ms
Execution Time: 0.139 ms
```

`Planning Time` :

> temps pour choisir le plan.

`Execution Time` :

> temps réel pour exécuter la requête.

---

## 9.25 Attention à EXPLAIN ANALYZE sur UPDATE / DELETE

Cette commande :

```sql
EXPLAIN ANALYZE
DELETE FROM commande
WHERE prix_total < 100;
```

**exécute réellement le `DELETE`**.

Pour tester sans conserver la modification :

```sql
BEGIN;

EXPLAIN ANALYZE
DELETE FROM commande
WHERE prix_total < 100;

ROLLBACK;
```

---

## 9.26 Checklist d’analyse d’une requête lente

Quand une requête est lente, regarde dans cet ordre :

1. `Execution Time`.
2. Le nœud qui prend le plus de temps.
3. `Seq Scan` / `Index Scan` / `Bitmap`.
4. `estimated rows` vs `actual rows`.
5. `Rows Removed by Filter`.
6. `loops`.
7. Type de jointure : `Nested Loop`, `Hash Join`, `Merge Join`.
8. Tri ou agrégation lourde.
9. `BUFFERS`.
10. Statistiques (`ANALYZE`) et index disponibles.
11. Réécriture éventuelle de la requête.

---

# 10. Vues et vues matérialisées

## 10.1 VIEW

Une vue stocke essentiellement la **requête**, pas son résultat.

Exemple :

```sql
CREATE VIEW vue_total_commandes AS
SELECT
    u.id,
    u.nom,
    u.prenom,
    COALESCE(SUM(c.prix_total), 0) AS total_commandes
FROM utilisateur u
LEFT JOIN commande c
    ON c.utilisateur_id = u.id
GROUP BY u.id, u.nom, u.prenom;
```

Utilisation :

```sql
SELECT *
FROM vue_total_commandes;
```

Si les données de `commande` changent, la vue retourne le résultat basé sur les données actuelles.

---

## 10.2 MATERIALIZED VIEW

Une vue matérialisée stocke physiquement le résultat calculé :

```sql
CREATE MATERIALIZED VIEW mv_total_commandes AS
SELECT
    u.id,
    u.nom,
    u.prenom,
    COALESCE(SUM(c.prix_total), 0) AS total_commandes
FROM utilisateur u
LEFT JOIN commande c
    ON c.utilisateur_id = u.id
GROUP BY u.id, u.nom, u.prenom;
```

Lecture :

```sql
SELECT *
FROM mv_total_commandes;
```

---

## 10.3 Rafraîchissement

Une vue matérialisée n’est pas automatiquement à jour :

```sql
REFRESH MATERIALIZED VIEW mv_total_commandes;
```

---

## 10.4 Index sur vue matérialisée

Possible :

```sql
CREATE INDEX idx_mv_total
ON mv_total_commandes(total_commandes);
```

Une vue classique n’a pas ses propres données, donc on indexe les tables sous-jacentes.

---

## 10.5 Refresh concurrent

```sql
CREATE UNIQUE INDEX idx_mv_user
ON mv_total_commandes(id);
```

Puis :

```sql
REFRESH MATERIALIZED VIEW CONCURRENTLY mv_total_commandes;
```

L’intérêt est de permettre des lectures pendant le rafraîchissement, sous les conditions imposées par PostgreSQL, notamment un index `UNIQUE` approprié.

---

## 10.6 Comparaison

| Critère | VIEW | MATERIALIZED VIEW |
|---|---|---|
| Stocke le résultat | Non | Oui |
| Données fraîches automatiquement | Oui | Non |
| Besoin de `REFRESH` | Non | Oui |
| Index propres | Non | Oui |
| Lecture d’une agrégation lourde | Peut recalculer | Souvent plus rapide |
| Stockage disque | Faible | Oui |
| Usage typique | abstraction | reporting / statistiques |

Raccourci :

```text
VIEW
= requête stockée

MATERIALIZED VIEW
= résultat stocké
```

---

# 11. Transactions et ACID

## 11.1 Transaction

Une transaction regroupe plusieurs opérations en une unité logique.

```sql
BEGIN;

UPDATE ...
INSERT ...
DELETE ...

COMMIT;
```

Si quelque chose échoue :

```sql
ROLLBACK;
```

---

## 11.2 ACID

ACID signifie :

| Lettre | Signification | Idée |
|---|---|---|
| A | Atomicity | tout ou rien |
| C | Consistency | règles de cohérence respectées |
| I | Isolation | gestion des transactions concurrentes |
| D | Durability | après COMMIT, les données persistent |

---

## 11.3 Atomicity

Exemple de transfert :

```sql
BEGIN;

UPDATE compte
SET solde = solde - 100
WHERE id = 1;

UPDATE compte
SET solde = solde + 100
WHERE id = 2;

COMMIT;
```

Si la deuxième étape échoue :

```sql
ROLLBACK;
```

On évite :

```text
compte 1 débité
mais compte 2 non crédité
```

---

## 11.4 Consistency

La transaction doit laisser la base dans un état conforme aux règles :

```text
PRIMARY KEY
UNIQUE
FOREIGN KEY
NOT NULL
CHECK
```

Exemple :

```sql
CHECK (solde >= 0)
```

---

## 11.5 Isolation

Définit comment les transactions concurrentes peuvent voir ou influencer les données des autres.

C’est le sujet des niveaux :

```text
READ UNCOMMITTED
READ COMMITTED
REPEATABLE READ
SERIALIZABLE
```

---

## 11.6 Durability

Après :

```sql
COMMIT;
```

les données doivent rester enregistrées même après un crash.

PostgreSQL utilise notamment le **WAL** (*Write-Ahead Log*).

---

# 12. Niveaux d’isolation

## 12.1 Les anomalies à connaître

| Anomalie | Signification |
|---|---|
| Dirty Read | lire une donnée non commitée |
| Non-repeatable Read | relire une ligne et obtenir une autre valeur |
| Phantom Read | refaire une requête et voir de nouvelles lignes |
| Serialization anomaly | résultat impossible à expliquer par un ordre séquentiel correct |

---

## 12.2 READ UNCOMMITTED

Théoriquement, permet les lectures sales.

Exemple conceptuel :

```text
Transaction A
UPDATE solde = 500
pas encore COMMIT

Transaction B
lit 500

Transaction A
ROLLBACK
```

B aurait lu une valeur qui n’a jamais été validée.

### PostgreSQL

PostgreSQL ne fournit pas de vraie lecture `READ UNCOMMITTED`.

Il traite ce niveau comme `READ COMMITTED`.

---

## 12.3 READ COMMITTED

Niveau par défaut PostgreSQL.

Session A :

```sql
BEGIN;

SELECT solde
FROM compte
WHERE id = 1;
-- 1000
```

Session B :

```sql
BEGIN;

UPDATE compte
SET solde = 500
WHERE id = 1;

COMMIT;
```

Session A relit :

```sql
SELECT solde
FROM compte
WHERE id = 1;
-- peut maintenant voir 500
```

Donc :

```text
premier SELECT  → 1000
deuxième SELECT → 500
```

C’est un **non-repeatable read**.

Sous `READ COMMITTED`, chaque instruction voit un snapshot des données commitées au début de cette instruction.

---

## 12.4 REPEATABLE READ

Session A :

```sql
BEGIN TRANSACTION ISOLATION LEVEL REPEATABLE READ;

SELECT solde
FROM compte
WHERE id = 1;
-- 1000
```

Session B :

```sql
UPDATE compte
SET solde = 500
WHERE id = 1;

COMMIT;
```

Session A :

```sql
SELECT solde
FROM compte
WHERE id = 1;
-- continue à voir 1000
```

La transaction A garde un snapshot cohérent.

PostgreSQL `REPEATABLE READ` empêche également les phantom reads classiques grâce à son modèle de snapshot, ce qui est plus fort que le minimum historique du standard SQL.

---

## 12.5 Phantom Read

Exemple :

```sql
SELECT COUNT(*)
FROM commande
WHERE prix_total > 500;
-- 4
```

Une autre transaction insère :

```sql
INSERT INTO commande (
    utilisateur_id,
    date_achat,
    num_facture,
    prix_total
)
VALUES (
    1,
    CURRENT_DATE,
    'A00999',
    900
);

COMMIT;
```

Une nouvelle exécution peut théoriquement voir :

```text
5
```

sous un niveau qui permet le phantom.

---

## 12.6 SERIALIZABLE

C’est le niveau le plus fort.

Objectif :

> Le résultat concurrent doit être équivalent à une exécution correcte des transactions l’une après l’autre.

PostgreSQL utilise notamment **Serializable Snapshot Isolation (SSI)**.

Cela ne signifie pas :

> verrouiller toute la base.

PostgreSQL peut laisser les transactions progresser, détecter une dépendance dangereuse et annuler l’une d’elles avec une erreur de sérialisation.

L’application doit alors généralement :

```text
ROLLBACK
↓
réessayer la transaction
```

---

## 12.7 Définir le niveau

```sql
BEGIN TRANSACTION ISOLATION LEVEL REPEATABLE READ;
```

ou :

```sql
BEGIN;

SET TRANSACTION ISOLATION LEVEL SERIALIZABLE;
```

Voir le niveau actuel :

```sql
SHOW transaction_isolation;
```

Par défaut PostgreSQL :

```text
read committed
```

---

## 12.8 Résumé

| Niveau | Dirty Read | Non-repeatable Read | Phantom Read |
|---|---:|---:|---:|
| READ UNCOMMITTED | possible théoriquement | possible | possible |
| READ COMMITTED | non | possible | possible |
| REPEATABLE READ | non | non | non sous PostgreSQL pour le cas classique |
| SERIALIZABLE | non | non | non |

---

# 13. Locks / verrous

## 13.1 Isolation ≠ lock explicite

Un niveau d’isolation répond à :

> Qu’est-ce que ma transaction peut voir et quelles anomalies sont acceptées ?

Un verrou explicite répond plutôt à :

> Qui peut modifier cette donnée maintenant ?

---

## 13.2 SELECT ... FOR UPDATE

Exemple :

```sql
BEGIN;

SELECT *
FROM compte
WHERE id = 1
FOR UPDATE;
```

La ligne est verrouillée vis-à-vis des modifications concurrentes incompatibles.

Puis :

```sql
UPDATE compte
SET solde = solde - 100
WHERE id = 1;

COMMIT;
```

Une autre transaction essayant de modifier la même ligne peut attendre jusqu’au `COMMIT` ou `ROLLBACK`.

---

## 13.3 Pourquoi utiliser FOR UPDATE ?

Workflow classique :

```text
1. lire le solde
2. vérifier une règle métier
3. modifier le solde
```

Sans verrou, une autre transaction pourrait modifier la ligne entre 1 et 3.

Avec :

```sql
SELECT ... FOR UPDATE
```

on protège cette séquence.

---

## 13.4 UPDATE verrouille déjà

Un :

```sql
UPDATE compte
SET solde = 500
WHERE id = 1;
```

prend déjà automatiquement un verrou sur la ligne modifiée jusqu’à la fin de la transaction.

Donc on ne fait pas obligatoirement `FOR UPDATE` avant chaque `UPDATE`.

`FOR UPDATE` est surtout utile lorsqu’on doit **lire puis décider puis modifier**.

---

## 13.5 Autres verrous de ligne PostgreSQL

```sql
FOR UPDATE
FOR NO KEY UPDATE
FOR SHARE
FOR KEY SHARE
```

Pour l’entretien, maîtrise en priorité :

```sql
FOR UPDATE
```

---

## 13.6 Verrou de table

```sql
LOCK TABLE commande
IN ACCESS EXCLUSIVE MODE;
```

C’est beaucoup plus agressif.

Règle pratique :

> Verrouiller le moins de données possible et pendant le moins longtemps possible.

---

## 13.7 Pessimistic locking

On verrouille avant le conflit :

```sql
SELECT *
FROM compte
WHERE id = 1
FOR UPDATE;
```

Mentalité :

> Je pense qu’un conflit est probable, je bloque la modification concurrente.

---

## 13.8 Optimistic locking

On ne bloque pas immédiatement.

On stocke une version :

```text
id | solde | version
1  | 1000  | 5
```

Puis :

```sql
UPDATE compte
SET solde = 900,
    version = 6
WHERE id = 1
  AND version = 5;
```

Si une autre transaction a déjà changé la ligne, la version n’est plus `5`, donc aucune ligne n’est modifiée.

En JPA :

```java
@Version
private Long version;
```

---

## 13.9 Risque de deadlock

Exemple :

```text
Transaction A
lock ligne 1
puis veut ligne 2

Transaction B
lock ligne 2
puis veut ligne 1
```

Les deux s’attendent mutuellement.

PostgreSQL détecte le deadlock et annule une transaction.

Bonne pratique :

> Accéder aux ressources partagées dans un ordre cohérent.

---

# 14. DELETE vs TRUNCATE

## 14.1 DELETE

```sql
DELETE FROM commande
WHERE prix_total < 100;
```

Peut utiliser un `WHERE`.

Sans `WHERE` :

```sql
DELETE FROM commande;
```

supprime toutes les lignes.

---

## 14.2 TRUNCATE

```sql
TRUNCATE TABLE commande;
```

Vide toute la table.

Pas de :

```sql
WHERE ...
```

`TRUNCATE` est généralement plus adapté lorsqu’on veut vider complètement une table.

---

## 14.3 ROLLBACK d’un TRUNCATE

Sous PostgreSQL :

```sql
BEGIN;

TRUNCATE TABLE commande;

ROLLBACK;
```

Les données sont restaurées.

Donc :

```text
PostgreSQL
TRUNCATE dans une transaction
→ rollback possible
```

Attention : le comportement n’est pas identique dans tous les SGBD. Par exemple, certains moteurs traitent `TRUNCATE` comme un DDL avec commit implicite.

---

## 14.4 Lock de TRUNCATE

Sous PostgreSQL, `TRUNCATE` prend un verrou fort (`ACCESS EXCLUSIVE`) sur la table.

Il faut donc aussi penser à l’impact sur les transactions concurrentes.

---

# 15. Fiche entretien

## 15.1 GROUP BY

> `GROUP BY` regroupe les lignes ayant les mêmes valeurs pour permettre des agrégations comme `SUM`, `COUNT`, `AVG`, `MIN` ou `MAX`. Les colonnes non agrégées du `SELECT` doivent généralement être présentes dans le `GROUP BY`.

---

## 15.2 WHERE vs HAVING

> `WHERE` filtre les lignes avant l’agrégation. `HAVING` filtre les groupes après `GROUP BY`.

---

## 15.3 Sous-requête corrélée

> Une sous-requête corrélée dépend d’une colonne de la requête extérieure. Elle peut donc être réévaluée plusieurs fois selon le plan d’exécution.

---

## 15.4 EXISTS

> `EXISTS` est adapté lorsqu’on veut simplement savoir si au moins une ligne correspond à une condition. Pour ce besoin, il exprime souvent mieux l’intention qu’un `JOIN + DISTINCT`.

---

## 15.5 ANY / ALL

```text
ANY = au moins une
ALL = toutes
```

Pour un ensemble simple, non vide et sans `NULL` :

```text
> ANY ≈ > MIN
> ALL ≈ > MAX
= ANY ≈ IN
```

---

## 15.6 Index

> Une `PRIMARY KEY` et une contrainte `UNIQUE` créent automatiquement un index sous PostgreSQL. Une clé étrangère côté enfant n’est pas automatiquement indexée, donc on l’indexe souvent si elle sert dans les jointures ou les filtres.

---

## 15.7 Pourquoi Seq Scan malgré un index ?

> La présence d’un index ne garantit pas son utilisation. PostgreSQL choisit le plan qu’il estime le moins coûteux. Pour une petite table ou une requête qui retourne beaucoup de lignes, un `Seq Scan` peut être plus rapide.

---

## 15.8 EXPLAIN vs EXPLAIN ANALYZE

> `EXPLAIN` montre le plan estimé sans exécuter le `SELECT`. `EXPLAIN ANALYZE` exécute réellement la requête et ajoute les temps, les lignes réelles et les boucles.

---

## 15.9 Vue vs vue matérialisée

> Une vue stocke la requête et reflète les données actuelles. Une vue matérialisée stocke physiquement le résultat, ce qui peut accélérer les lectures lourdes, mais il faut la rafraîchir.

---

## 15.10 ACID

```text
A → Atomicity
C → Consistency
I → Isolation
D → Durability
```

---

## 15.11 Niveaux d’isolation

> Plus le niveau est fort, plus on limite les anomalies concurrentes. PostgreSQL utilise `READ COMMITTED` par défaut. `SERIALIZABLE` vise un résultat équivalent à une exécution séquentielle et peut provoquer une erreur de sérialisation qu’il faut réessayer.

---

## 15.12 SERIALIZABLE vs lock

> `SERIALIZABLE` est un niveau d’isolation, pas simplement un verrou. Pour verrouiller explicitement une ligne, on utilise notamment `SELECT ... FOR UPDATE`.

---

# 16. Exercices de révision avec solutions

## Exercice 1 — Total des commandes par utilisateur

**Question :**

Afficher le nom complet et le total dépensé par chaque utilisateur ayant au moins une commande.

<details>
<summary>Solution</summary>

```sql
SELECT
    CONCAT(u.nom, ' ', u.prenom) AS utilisateur,
    SUM(c.prix_total) AS total
FROM utilisateur u
JOIN commande c
    ON c.utilisateur_id = u.id
GROUP BY u.id, u.nom, u.prenom
ORDER BY total DESC;
```

</details>

---

## Exercice 2 — Utilisateurs avec zéro commande

**Question :**

Afficher tous les utilisateurs avec leur total, y compris ceux sans commande.

<details>
<summary>Solution</summary>

```sql
SELECT
    u.id,
    u.nom,
    u.prenom,
    COALESCE(SUM(c.prix_total), 0) AS total
FROM utilisateur u
LEFT JOIN commande c
    ON c.utilisateur_id = u.id
GROUP BY u.id, u.nom, u.prenom;
```

</details>

---

## Exercice 3 — HAVING

**Question :**

Afficher les utilisateurs ayant passé au moins 3 commandes.

<details>
<summary>Solution</summary>

```sql
SELECT
    u.id,
    u.nom,
    u.prenom,
    COUNT(c.num_facture) AS nombre_commandes
FROM utilisateur u
JOIN commande c
    ON c.utilisateur_id = u.id
GROUP BY u.id, u.nom, u.prenom
HAVING COUNT(c.num_facture) >= 3;
```

</details>

---

## Exercice 4 — Maximum par utilisateur

**Question :**

Afficher la date et le montant de la commande la plus chère de chaque utilisateur.

<details>
<summary>Solution</summary>

```sql
SELECT
    CONCAT(u.nom, ' ', u.prenom) AS utilisateur,
    c.date_achat,
    c.prix_total
FROM utilisateur u
JOIN commande c
    ON c.utilisateur_id = u.id
WHERE c.prix_total = (
    SELECT MAX(c2.prix_total)
    FROM commande c2
    WHERE c2.utilisateur_id = u.id
);
```

</details>

---

## Exercice 5 — Même maximum avec ROW_NUMBER

<details>
<summary>Solution</summary>

```sql
SELECT
    utilisateur,
    date_achat,
    prix_total
FROM (
    SELECT
        CONCAT(u.nom, ' ', u.prenom) AS utilisateur,
        c.date_achat,
        c.prix_total,
        ROW_NUMBER() OVER (
            PARTITION BY u.id
            ORDER BY c.prix_total DESC, c.date_achat DESC
        ) AS rn
    FROM utilisateur u
    JOIN commande c
        ON c.utilisateur_id = u.id
) t
WHERE rn = 1;
```

</details>

---

## Exercice 6 — Supérieur à la moyenne

**Question :**

Afficher les commandes dont le montant dépasse la moyenne globale.

<details>
<summary>Solution</summary>

```sql
SELECT *
FROM commande
WHERE prix_total > (
    SELECT AVG(prix_total)
    FROM commande
);
```

</details>

---

## Exercice 7 — EXISTS

**Question :**

Afficher les utilisateurs ayant au moins une commande > 500 €.

<details>
<summary>Solution</summary>

```sql
SELECT
    u.id,
    u.nom,
    u.prenom
FROM utilisateur u
WHERE EXISTS (
    SELECT 1
    FROM commande c
    WHERE c.utilisateur_id = u.id
      AND c.prix_total > 500
);
```

</details>

---

## Exercice 8 — NOT EXISTS

**Question :**

Afficher les utilisateurs n’ayant jamais passé de commande.

<details>
<summary>Solution</summary>

```sql
SELECT
    u.id,
    u.nom,
    u.prenom
FROM utilisateur u
WHERE NOT EXISTS (
    SELECT 1
    FROM commande c
    WHERE c.utilisateur_id = u.id
);
```

</details>

---

## Exercice 9 — ANY

**Question :**

Afficher les commandes plus chères qu’au moins une commande de l’utilisateur 1.

<details>
<summary>Solution</summary>

```sql
SELECT
    c.num_facture,
    c.prix_total
FROM commande c
WHERE c.prix_total > ANY (
    SELECT c2.prix_total
    FROM commande c2
    WHERE c2.utilisateur_id = 1
);
```

</details>

---

## Exercice 10 — ALL

**Question :**

Afficher les commandes plus chères que toutes les commandes de l’utilisateur 1.

<details>
<summary>Solution</summary>

```sql
SELECT
    c.num_facture,
    c.prix_total
FROM commande c
WHERE c.prix_total > ALL (
    SELECT c2.prix_total
    FROM commande c2
    WHERE c2.utilisateur_id = 1
);
```

</details>

---

## Exercice 11 — Mois en lettres

**Question :**

Afficher le mois de la commande la plus chère de chaque utilisateur.

<details>
<summary>Solution</summary>

```sql
SELECT
    CONCAT(u.nom, ' ', u.prenom) AS utilisateur,
    CASE EXTRACT(MONTH FROM c.date_achat)
        WHEN 1 THEN 'January'
        WHEN 2 THEN 'February'
        WHEN 3 THEN 'March'
        WHEN 4 THEN 'April'
        WHEN 5 THEN 'May'
        WHEN 6 THEN 'June'
        WHEN 7 THEN 'July'
        WHEN 8 THEN 'August'
        WHEN 9 THEN 'September'
        WHEN 10 THEN 'October'
        WHEN 11 THEN 'November'
        WHEN 12 THEN 'December'
    END AS mois,
    c.prix_total
FROM utilisateur u
JOIN commande c
    ON c.utilisateur_id = u.id
WHERE c.prix_total = (
    SELECT MAX(c2.prix_total)
    FROM commande c2
    WHERE c2.utilisateur_id = u.id
);
```

</details>

---

## Exercice 12 — Index

**Question :**

Quel index proposer pour accélérer la recherche des commandes d’un utilisateur ?

<details>
<summary>Solution</summary>

```sql
CREATE INDEX idx_commande_utilisateur
ON commande(utilisateur_id);
```

Sous PostgreSQL, cet index n’est pas créé automatiquement simplement parce que `utilisateur_id` est une clé étrangère.

</details>

---

## Exercice 13 — Plan d’exécution

**Question :**

Pourquoi PostgreSQL peut-il faire un `Seq Scan` après avoir créé :

```sql
CREATE INDEX idx_commande_prix
ON commande(prix_total);
```

<details>
<summary>Solution</summary>

Parce que la table est minuscule : environ 33 lignes dans le jeu d’entraînement. Lire toute la table peut coûter moins cher que parcourir l’index puis accéder aux lignes.

`Seq Scan` n’est donc pas automatiquement un problème.

</details>

---

## Exercice 14 — Vue matérialisée

**Question :**

Créer une vue matérialisée contenant le total par utilisateur.

<details>
<summary>Solution</summary>

```sql
CREATE MATERIALIZED VIEW mv_total_commandes AS
SELECT
    u.id,
    u.nom,
    u.prenom,
    COALESCE(SUM(c.prix_total), 0) AS total
FROM utilisateur u
LEFT JOIN commande c
    ON c.utilisateur_id = u.id
GROUP BY u.id, u.nom, u.prenom;
```

Rafraîchir :

```sql
REFRESH MATERIALIZED VIEW mv_total_commandes;
```

</details>

---

## Exercice 15 — Lock pessimiste

**Question :**

Lire un compte afin de vérifier son solde avant modification tout en empêchant une modification concurrente incompatible.

<details>
<summary>Solution</summary>

```sql
BEGIN;

SELECT solde
FROM compte
WHERE id = 1
FOR UPDATE;

-- vérification métier

UPDATE compte
SET solde = solde - 80
WHERE id = 1;

COMMIT;
```

</details>

---

# Annexe A — Commandes PostgreSQL utiles

```sql
-- Voir le niveau d’isolation
SHOW transaction_isolation;

-- Mettre à jour les statistiques
ANALYZE commande;
ANALYZE utilisateur;

-- Plan estimé
EXPLAIN
SELECT ...;

-- Plan réel
EXPLAIN ANALYZE
SELECT ...;

-- Plan réel avec informations de buffers
EXPLAIN (ANALYZE, BUFFERS)
SELECT ...;

-- Voir les index d’une table
SELECT indexname, indexdef
FROM pg_indexes
WHERE tablename = 'commande';

-- Transaction
BEGIN;
COMMIT;
ROLLBACK;

-- Verrou pessimiste
SELECT ...
FOR UPDATE;

-- Rafraîchir une vue matérialisée
REFRESH MATERIALIZED VIEW mv_total_commandes;
```

---

# Annexe B — Carte mentale

```text
SQL
│
├── Lecture
│   ├── SELECT
│   ├── WHERE
│   ├── JOIN
│   ├── DISTINCT
│   └── ORDER BY
│
├── Agrégations
│   ├── COUNT / SUM / AVG / MIN / MAX
│   ├── GROUP BY
│   └── HAVING
│
├── Sous-requêtes
│   ├── indépendante
│   ├── corrélée
│   ├── EXISTS / NOT EXISTS
│   ├── IN
│   └── ANY / ALL
│
├── Fonctions fenêtre
│   └── ROW_NUMBER
│
├── Performance
│   ├── index
│   ├── EXPLAIN
│   ├── EXPLAIN ANALYZE
│   ├── BUFFERS
│   └── ANALYZE statistiques
│
├── Abstraction / reporting
│   ├── VIEW
│   └── MATERIALIZED VIEW
│
└── Transactions
    ├── ACID
    ├── Isolation
    │   ├── READ COMMITTED
    │   ├── REPEATABLE READ
    │   └── SERIALIZABLE
    └── Locks
        ├── FOR UPDATE
        ├── pessimistic locking
        └── optimistic locking
```

---

# À retenir en une minute

```text
GROUP BY
→ regrouper pour agréger

HAVING
→ filtrer après GROUP BY

EXISTS
→ au moins une ligne existe

ANY
→ au moins une valeur

ALL
→ toutes les valeurs

PRIMARY KEY / UNIQUE
→ index automatique PostgreSQL

FOREIGN KEY côté enfant
→ index non automatique PostgreSQL

EXPLAIN
→ plan estimé

EXPLAIN ANALYZE
→ exécute + mesures réelles

Seq Scan
→ pas forcément mauvais

VIEW
→ requête stockée

MATERIALIZED VIEW
→ résultat stocké + REFRESH

ACID
→ Atomicity, Consistency, Isolation, Durability

SERIALIZABLE
→ niveau d’isolation, pas simplement un lock

SELECT ... FOR UPDATE
→ verrou pessimiste explicite
```
