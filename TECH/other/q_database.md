# Questions SQL pour Entretien Full Stack Java/Angular

## Questions Fondamentales

### 1. Quelle est la différence entre WHERE et HAVING ?
**Réponse :** 
- `WHERE` filtre les lignes **avant** le regroupement (GROUP BY)
- `HAVING` filtre les groupes **après** le regroupement
- `WHERE` ne peut pas utiliser de fonctions d'agrégation, `HAVING` oui

**Exemple :**
```sql
SELECT department, COUNT(*) as nb_employes
FROM employees
WHERE salary > 30000  -- filtre avant regroupement
GROUP BY department
HAVING COUNT(*) > 5;  -- filtre après regroupement
```

### 2. Expliquez les différents types de JOIN
**Réponse :**
- **INNER JOIN** : Retourne uniquement les lignes qui ont des correspondances dans les deux tables
- **LEFT JOIN** : Retourne toutes les lignes de la table de gauche + correspondances de droite (NULL si pas de correspondance)
- **RIGHT JOIN** : Inverse de LEFT JOIN
- **FULL OUTER JOIN** : Retourne toutes les lignes des deux tables
- **CROSS JOIN** : Produit cartésien de deux tables

**Exemple :**
```sql
-- INNER JOIN
SELECT e.name, d.department_name
FROM employees e
INNER JOIN departments d ON e.department_id = d.id;

-- LEFT JOIN
SELECT e.name, d.department_name
FROM employees e
LEFT JOIN departments d ON e.department_id = d.id;
```

### 3. Quelle est la différence entre DELETE, TRUNCATE et DROP ?
**Réponse :**
- **DELETE** : Supprime des lignes spécifiques, peut avoir WHERE, peut être rollback, déclenche les triggers
- **TRUNCATE** : Supprime toutes les lignes rapidement, ne peut pas être rollback (dans certains SGBD), ne déclenche pas les triggers
- **DROP** : Supprime la table entière (structure + données)

```sql
DELETE FROM employees WHERE id = 5;      -- Supprime une ligne
TRUNCATE TABLE employees;                -- Vide la table
DROP TABLE employees;                    -- Supprime la table
```

### 4. Qu'est-ce qu'une clé primaire et une clé étrangère ?
**Réponse :**
- **Clé primaire (PRIMARY KEY)** : Identifie de manière unique chaque ligne, ne peut pas être NULL, une seule par table
- **Clé étrangère (FOREIGN KEY)** : Crée un lien entre deux tables, référence la clé primaire d'une autre table

```sql
CREATE TABLE departments (
    id INT PRIMARY KEY,
    name VARCHAR(100)
);

CREATE TABLE employees (
    id INT PRIMARY KEY,
    name VARCHAR(100),
    department_id INT,
    FOREIGN KEY (department_id) REFERENCES departments(id)
);
```

## Questions sur les Fonctions d'Agrégation

### 5. Quelles sont les principales fonctions d'agrégation ?
**Réponse :**
- `COUNT()` : Compte le nombre de lignes
- `SUM()` : Somme des valeurs
- `AVG()` : Moyenne
- `MIN()` / `MAX()` : Valeurs min/max
- `GROUP_CONCAT()` / `STRING_AGG()` : Concaténation de valeurs

```sql
SELECT 
    department_id,
    COUNT(*) as nb_employes,
    AVG(salary) as salaire_moyen,
    MIN(salary) as salaire_min,
    MAX(salary) as salaire_max,
    SUM(salary) as masse_salariale
FROM employees
GROUP BY department_id;
```

### 6. Comment compter le nombre de valeurs distinctes ?
**Réponse :**
```sql
SELECT COUNT(DISTINCT department_id) as nb_departments
FROM employees;
```

## Questions sur les Sous-requêtes

### 7. Qu'est-ce qu'une sous-requête ? Donnez des exemples
**Réponse :** Une sous-requête est une requête imbriquée dans une autre requête.

```sql
-- Sous-requête dans WHERE
SELECT name, salary
FROM employees
WHERE salary > (SELECT AVG(salary) FROM employees);

-- Sous-requête dans FROM
SELECT dept_stats.department_id, dept_stats.avg_salary
FROM (
    SELECT department_id, AVG(salary) as avg_salary
    FROM employees
    GROUP BY department_id
) as dept_stats
WHERE dept_stats.avg_salary > 50000;

-- Sous-requête corrélée
SELECT e1.name, e1.salary
FROM employees e1
WHERE e1.salary > (
    SELECT AVG(e2.salary)
    FROM employees e2
    WHERE e2.department_id = e1.department_id
);
```

### 8. Différence entre IN, EXISTS et JOIN ?
**Réponse :**
- `IN` : Vérifie si une valeur existe dans un ensemble
- `EXISTS` : Vérifie si une sous-requête retourne au moins une ligne (plus performant pour grandes tables)
- `JOIN` : Combine des tables

```sql
-- IN
SELECT * FROM employees
WHERE department_id IN (SELECT id FROM departments WHERE location = 'Paris');

-- EXISTS
SELECT * FROM employees e
WHERE EXISTS (
    SELECT 1 FROM departments d 
    WHERE d.id = e.department_id AND d.location = 'Paris'
);
```

## Questions sur les Index

### 9. Qu'est-ce qu'un index et pourquoi l'utiliser ?
**Réponse :** Un index améliore la vitesse de recherche des données. C'est comme un index de livre.

**Avantages :**
- Accélère les requêtes SELECT avec WHERE, JOIN, ORDER BY
- Améliore les performances

**Inconvénients :**
- Ralentit les INSERT, UPDATE, DELETE
- Occupe de l'espace disque

```sql
CREATE INDEX idx_employee_name ON employees(name);
CREATE INDEX idx_dept_salary ON employees(department_id, salary);
```

### 10. Différence entre index clustered et non-clustered ?
**Réponse :**
- **Clustered** : Détermine l'ordre physique des données, un seul par table (généralement la clé primaire)
- **Non-clustered** : Index séparé avec pointeurs vers les données, plusieurs possibles par table

## Questions sur les Transactions

### 11. Qu'est-ce qu'une transaction ? Expliquez ACID
**Réponse :** Une transaction est un ensemble d'opérations qui s'exécutent comme une seule unité.

**ACID :**
- **Atomicité** : Tout ou rien
- **Cohérence** : Les données passent d'un état valide à un autre
- **Isolation** : Les transactions sont isolées les unes des autres
- **Durabilité** : Les changements validés sont permanents

```sql
BEGIN TRANSACTION;
UPDATE accounts SET balance = balance - 100 WHERE id = 1;
UPDATE accounts SET balance = balance + 100 WHERE id = 2;
COMMIT;
-- ou ROLLBACK en cas d'erreur
```

### 12. Quels sont les niveaux d'isolation des transactions ?
**Réponse :**
1. **READ UNCOMMITTED** : Peut lire des données non validées (dirty read)
2. **READ COMMITTED** : Lit uniquement les données validées
3. **REPEATABLE READ** : Garantit la même lecture multiple fois
4. **SERIALIZABLE** : Isolation complète, comme si transactions étaient séquentielles

## Questions sur les Contraintes

### 13. Quelles sont les principales contraintes SQL ?
**Réponse :**
- `NOT NULL` : La colonne ne peut pas être NULL
- `UNIQUE` : Toutes les valeurs doivent être différentes
- `PRIMARY KEY` : Combinaison de NOT NULL et UNIQUE
- `FOREIGN KEY` : Lien vers une autre table
- `CHECK` : Vérifie une condition
- `DEFAULT` : Valeur par défaut

```sql
CREATE TABLE employees (
    id INT PRIMARY KEY,
    email VARCHAR(100) UNIQUE NOT NULL,
    age INT CHECK (age >= 18),
    status VARCHAR(20) DEFAULT 'active',
    department_id INT,
    FOREIGN KEY (department_id) REFERENCES departments(id)
);
```

## Questions sur l'Optimisation

### 14. Comment optimiser une requête SQL lente ?
**Réponse :**
1. Utiliser EXPLAIN pour analyser le plan d'exécution
2. Ajouter des index appropriés
3. Éviter SELECT *, spécifier les colonnes nécessaires
4. Limiter les sous-requêtes corrélées
5. Utiliser JOIN au lieu de sous-requêtes quand possible
6. Partitionner les grandes tables
7. Utiliser LIMIT pour limiter les résultats
8. Éviter les fonctions dans WHERE sur colonnes indexées

```sql
-- Mauvais
SELECT * FROM employees WHERE YEAR(hire_date) = 2024;

-- Bon
SELECT id, name, salary FROM employees 
WHERE hire_date >= '2024-01-01' AND hire_date < '2025-01-01';
```

### 15. Qu'est-ce que EXPLAIN et comment l'utiliser ?
**Réponse :** EXPLAIN montre comment la base de données exécute une requête.

```sql
EXPLAIN SELECT e.name, d.department_name
FROM employees e
JOIN departments d ON e.department_id = d.id
WHERE e.salary > 50000;
```

## Questions Avancées

### 16. Qu'est-ce qu'une vue (VIEW) ?
**Réponse :** Une vue est une table virtuelle basée sur une requête SELECT.

```sql
CREATE VIEW high_earners AS
SELECT e.name, e.salary, d.department_name
FROM employees e
JOIN departments d ON e.department_id = d.id
WHERE e.salary > 70000;

-- Utilisation
SELECT * FROM high_earners;
```

### 17. Différence entre UNION et UNION ALL ?
**Réponse :**
- `UNION` : Combine les résultats et **élimine les doublons**
- `UNION ALL` : Combine les résultats et **garde tous les doublons** (plus rapide)

```sql
SELECT name FROM employees WHERE department_id = 1
UNION
SELECT name FROM employees WHERE department_id = 2;

SELECT name FROM employees WHERE department_id = 1
UNION ALL
SELECT name FROM employees WHERE department_id = 2;
```

### 18. Qu'est-ce qu'un trigger ?
**Réponse :** Un trigger est une procédure automatique qui s'exécute lors d'événements (INSERT, UPDATE, DELETE).

```sql
CREATE TRIGGER update_employee_count
AFTER INSERT ON employees
FOR EACH ROW
BEGIN
    UPDATE departments 
    SET employee_count = employee_count + 1
    WHERE id = NEW.department_id;
END;
```

### 19. Qu'est-ce qu'une procédure stockée ?
**Réponse :** Un ensemble d'instructions SQL précompilées et stockées dans la base.

```sql
CREATE PROCEDURE GetEmployeesByDepartment(IN dept_id INT)
BEGIN
    SELECT * FROM employees WHERE department_id = dept_id;
END;

-- Appel
CALL GetEmployeesByDepartment(5);
```

### 20. Expliquez les Window Functions
**Réponse :** Les fonctions fenêtre effectuent des calculs sur un ensemble de lignes liées.

```sql
-- ROW_NUMBER
SELECT name, salary,
    ROW_NUMBER() OVER (ORDER BY salary DESC) as rank
FROM employees;

-- RANK avec PARTITION BY
SELECT name, department_id, salary,
    RANK() OVER (PARTITION BY department_id ORDER BY salary DESC) as dept_rank
FROM employees;

-- Running total
SELECT name, salary,
    SUM(salary) OVER (ORDER BY hire_date) as running_total
FROM employees;
```

## Questions Pratiques de Requêtage

### 21. Trouvez le 2ème salaire le plus élevé
**Réponse :**
```sql
-- Méthode 1 : LIMIT OFFSET
SELECT DISTINCT salary
FROM employees
ORDER BY salary DESC
LIMIT 1 OFFSET 1;

-- Méthode 2 : Sous-requête
SELECT MAX(salary)
FROM employees
WHERE salary < (SELECT MAX(salary) FROM employees);

-- Méthode 3 : Window function
SELECT DISTINCT salary
FROM (
    SELECT salary, DENSE_RANK() OVER (ORDER BY salary DESC) as rank
    FROM employees
) ranked
WHERE rank = 2;
```

### 22. Trouvez les employés qui gagnent plus que la moyenne de leur département
**Réponse :**
```sql
SELECT e.name, e.salary, e.department_id
FROM employees e
WHERE e.salary > (
    SELECT AVG(salary)
    FROM employees e2
    WHERE e2.department_id = e.department_id
);
```

### 23. Trouvez les doublons dans une table
**Réponse :**
```sql
SELECT email, COUNT(*) as count
FROM employees
GROUP BY email
HAVING COUNT(*) > 1;

-- Avec toutes les colonnes des doublons
SELECT e1.*
FROM employees e1
INNER JOIN (
    SELECT email
    FROM employees
    GROUP BY email
    HAVING COUNT(*) > 1
) e2 ON e1.email = e2.email;
```

### 24. Supprimez les doublons en gardant un seul enregistrement
**Réponse :**
```sql
-- Garder l'ID le plus petit
DELETE e1 FROM employees e1
INNER JOIN employees e2
WHERE e1.email = e2.email AND e1.id > e2.id;

-- Avec CTE (Common Table Expression)
WITH CTE AS (
    SELECT id, email,
        ROW_NUMBER() OVER (PARTITION BY email ORDER BY id) as rn
    FROM employees
)
DELETE FROM employees
WHERE id IN (SELECT id FROM CTE WHERE rn > 1);
```

### 25. Calculez le salaire cumulatif par département
**Réponse :**
```sql
SELECT 
    name,
    department_id,
    salary,
    SUM(salary) OVER (
        PARTITION BY department_id 
        ORDER BY hire_date
        ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW
    ) as cumulative_salary
FROM employees;
```

### 26. Trouvez les employés sans manager
**Réponse :**
```sql
SELECT e1.*
FROM employees e1
LEFT JOIN employees e2 ON e1.manager_id = e2.id
WHERE e2.id IS NULL;

-- ou
SELECT * FROM employees
WHERE manager_id IS NULL;
```

### 27. Affichez une hiérarchie d'employés (recursive CTE)
**Réponse :**
```sql
WITH RECURSIVE employee_hierarchy AS (
    -- Anchor: managers sans supérieur
    SELECT id, name, manager_id, 1 as level
    FROM employees
    WHERE manager_id IS NULL
    
    UNION ALL
    
    -- Recursive: employés sous les managers
    SELECT e.id, e.name, e.manager_id, eh.level + 1
    FROM employees e
    INNER JOIN employee_hierarchy eh ON e.manager_id = eh.id
)
SELECT * FROM employee_hierarchy ORDER BY level, name;
```

### 28. Pivotez des données (transformer lignes en colonnes)
**Réponse :**
```sql
-- Pivot manuel
SELECT 
    department_id,
    SUM(CASE WHEN year = 2023 THEN revenue ELSE 0 END) as revenue_2023,
    SUM(CASE WHEN year = 2024 THEN revenue ELSE 0 END) as revenue_2024,
    SUM(CASE WHEN year = 2025 THEN revenue ELSE 0 END) as revenue_2025
FROM sales
GROUP BY department_id;
```

### 29. Trouvez les N premiers enregistrements par groupe
**Réponse :**
```sql
-- Top 3 salaires par département
WITH ranked_employees AS (
    SELECT *,
        ROW_NUMBER() OVER (PARTITION BY department_id ORDER BY salary DESC) as rank
    FROM employees
)
SELECT * FROM ranked_employees
WHERE rank <= 3;
```

### 30. Calculez la différence avec la ligne précédente (LAG/LEAD)
**Réponse :**
```sql
SELECT 
    name,
    salary,
    LAG(salary) OVER (ORDER BY hire_date) as previous_salary,
    salary - LAG(salary) OVER (ORDER BY hire_date) as salary_diff,
    LEAD(salary) OVER (ORDER BY hire_date) as next_salary
FROM employees;
```

## Questions sur les Types de Données

### 31. Quels sont les principaux types de données SQL ?
**Réponse :**
- **Numériques** : INT, BIGINT, DECIMAL, FLOAT, DOUBLE
- **Chaînes** : VARCHAR, CHAR, TEXT
- **Date/Heure** : DATE, TIME, DATETIME, TIMESTAMP
- **Binaires** : BLOB, BINARY
- **Booléens** : BOOLEAN
- **JSON** : JSON (dans MySQL, PostgreSQL)

### 32. Différence entre CHAR et VARCHAR ?
**Réponse :**
- `CHAR(n)` : Longueur fixe, complète avec des espaces, plus rapide pour recherches
- `VARCHAR(n)` : Longueur variable, économise de l'espace

```sql
CHAR(10) stocke 'test' comme 'test      ' (10 caractères)
VARCHAR(10) stocke 'test' comme 'test' (4 caractères)
```

### 33. Différence entre DATETIME et TIMESTAMP ?
**Réponse :**
- `DATETIME` : Stocke date et heure (1000-9999), ne gère pas les fuseaux horaires
- `TIMESTAMP` : Stocke timestamp Unix (1970-2038), converti selon fuseau horaire serveur

## Questions sur la Normalisation

### 34. Qu'est-ce que la normalisation ? Expliquez les formes normales
**Réponse :** La normalisation organise les données pour réduire la redondance.

- **1NF** : Valeurs atomiques (pas de listes), chaque colonne un seul type
- **2NF** : 1NF + pas de dépendance partielle (tous les attributs dépendent de la clé complète)
- **3NF** : 2NF + pas de dépendance transitive (attributs non-clés ne dépendent que de la clé)
- **BCNF** : 3NF renforcée

### 35. Qu'est-ce que la dénormalisation et quand l'utiliser ?
**Réponse :** Ajouter intentionnellement de la redondance pour améliorer les performances en lecture.

**Quand l'utiliser :**
- Data warehouses
- Applications avec beaucoup de lectures, peu d'écritures
- Besoin de performances de requête

## Questions sur les Performances

### 36. Qu'est-ce que le query caching ?
**Réponse :** Le cache stocke les résultats des requêtes pour réutilisation rapide si la même requête est exécutée.

### 37. Différence entre table scan et index scan ?
**Réponse :**
- **Table scan** : Lit toutes les lignes de la table (lent)
- **Index scan** : Utilise l'index pour trouver rapidement les lignes (rapide)

### 38. Qu'est-ce que le query plan ?
**Réponse :** Le plan d'exécution montre comment la base de données va exécuter une requête (ordre des opérations, index utilisés, etc.)

## Questions Spécifiques PostgreSQL/MySQL

### 39. Différences principales entre MySQL et PostgreSQL ?
**Réponse :**
- **PostgreSQL** : Plus conforme aux standards SQL, meilleur support des fonctionnalités avancées (CTE récursifs, window functions), ACID strict
- **MySQL** : Plus rapide pour lectures simples, plus populaire historiquement

### 40. Qu'est-ce qu'un CTE (Common Table Expression) ?
**Réponse :** Une table temporaire nommée utilisable dans une requête.

```sql
WITH department_stats AS (
    SELECT department_id, AVG(salary) as avg_salary
    FROM employees
    GROUP BY department_id
)
SELECT e.name, e.salary, ds.avg_salary
FROM employees e
JOIN department_stats ds ON e.department_id = ds.department_id
WHERE e.salary > ds.avg_salary;
```

## Questions Pièges Fréquentes

### 41. Que retourne cette requête : SELECT NULL = NULL ?
**Réponse :** `NULL` (pas TRUE). Il faut utiliser `IS NULL` ou `IS NOT NULL`.

```sql
-- Incorrect
SELECT * FROM employees WHERE manager_id = NULL;

-- Correct
SELECT * FROM employees WHERE manager_id IS NULL;
```

### 42. Différence entre COUNT(*), COUNT(1) et COUNT(column) ?
**Réponse :**
- `COUNT(*)` : Compte toutes les lignes
- `COUNT(1)` : Équivalent à COUNT(*) dans la plupart des SGBD
- `COUNT(column)` : Compte les valeurs non-NULL de la colonne

```sql
SELECT COUNT(*) FROM employees;        -- Compte tout
SELECT COUNT(manager_id) FROM employees;  -- Ignore les NULL
```

### 43. Que fait COALESCE ?
**Réponse :** Retourne la première valeur non-NULL.

```sql
SELECT name, COALESCE(phone, email, 'No contact') as contact
FROM employees;
```

### 44. Différence entre = et LIKE ?
**Réponse :**
- `=` : Égalité exacte
- `LIKE` : Correspondance de motif avec wildcards (% et _)

```sql
SELECT * FROM employees WHERE name = 'John';
SELECT * FROM employees WHERE name LIKE 'John%';  -- John, Johnny, etc.
SELECT * FROM employees WHERE name LIKE 'J_hn';   -- John, Jahn, etc.
```

### 45. Comment gérer les valeurs NULL dans les calculs ?
**Réponse :** Utiliser COALESCE, IFNULL, ou NULLIF.

```sql
SELECT name, 
    salary + COALESCE(bonus, 0) as total_compensation
FROM employees;
```

## Conseils Finaux pour l'Entretien

### Points à retenir :
1. **Expliquez votre raisonnement** : Ne donnez pas juste la réponse, expliquez pourquoi
2. **Optimisation** : Mentionnez toujours les considérations de performance
3. **Index** : Pensez aux index quand vous écrivez des requêtes
4. **Cas d'usage réels** : Reliez à des situations professionnelles
5. **Trade-offs** : Discutez des avantages/inconvénients de différentes approches
6. **Testabilité** : Mentionnez comment vous testeriez vos requêtes

### Questions à poser à l'employeur :
- Quel SGBD utilisez-vous ? (MySQL, PostgreSQL, Oracle, SQL Server)
- Quelle est la taille typique des bases de données ?
- Utilisez-vous un ORM ? (Hibernate, JPA pour Java)
- Comment gérez-vous les migrations de schéma ?
- Avez-vous des pratiques spécifiques d'optimisation ?

Bonne chance pour votre entretien ! 🚀
