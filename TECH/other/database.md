## **PostgreSQL - Concepts Fondamentaux et Utilisation**

### **Introduction à PostgreSQL**

PostgreSQL est un système de gestion de base de données relationnelle (SGBDR) open-source, robuste et conforme aux standards ACID (Atomicity, Consistency, Isolation, Durability). Il supporte SQL standard ainsi que des fonctionnalités avancées comme les types de données personnalisés, les procédures stockées, les triggers, et les index sophistiqués.

**Caractéristiques principales :**
- Support complet du SQL standard
- Transactions ACID
- Contraintes d'intégrité référentielle
- Types de données riches (JSON, XML, Array, etc.)
- Extensibilité
- Haute performance et scalabilité

---

### **Types de Données PostgreSQL**

**Types numériques :**
```sql
-- Entiers
SMALLINT        -- -32768 à +32767
INTEGER (INT)   -- -2147483648 à +2147483647
BIGINT          -- -9223372036854775808 à +9223372036854775807

-- Décimaux
DECIMAL(p,s)    -- Précision exacte
NUMERIC(p,s)    -- Identique à DECIMAL
REAL            -- 6 chiffres de précision
DOUBLE PRECISION -- 15 chiffres de précision

-- Séquences auto-incrémentées
SERIAL          -- INTEGER auto-incrémenté
BIGSERIAL       -- BIGINT auto-incrémenté

-- Exemples
CREATE TABLE products (
    id SERIAL PRIMARY KEY,
    quantity INTEGER NOT NULL,
    price NUMERIC(10, 2) NOT NULL,
    discount REAL,
    weight DOUBLE PRECISION
);
```

**Types caractères :**
```sql
CHAR(n)         -- Longueur fixe, complété avec des espaces
VARCHAR(n)      -- Longueur variable avec limite
TEXT            -- Longueur variable sans limite

-- Exemples
CREATE TABLE customers (
    id SERIAL PRIMARY KEY,
    code CHAR(10),              -- Exactement 10 caractères
    name VARCHAR(100) NOT NULL,  -- Max 100 caractères
    description TEXT             -- Illimité
);
```

**Types date et heure :**
```sql
DATE            -- Date seulement (YYYY-MM-DD)
TIME            -- Heure seulement (HH:MI:SS)
TIMESTAMP       -- Date et heure
TIMESTAMPTZ     -- Timestamp avec timezone
INTERVAL        -- Intervalle de temps

-- Exemples
CREATE TABLE orders (
    id SERIAL PRIMARY KEY,
    order_date DATE NOT NULL,
    order_time TIME,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMPTZ,
    delivery_window INTERVAL
);

-- Manipulation des dates
SELECT 
    CURRENT_DATE,
    CURRENT_TIME,
    CURRENT_TIMESTAMP,
    NOW(),
    DATE '2024-01-15',
    TIMESTAMP '2024-01-15 10:30:00',
    INTERVAL '2 days',
    INTERVAL '3 hours 30 minutes';

-- Opérations sur les dates
SELECT 
    CURRENT_DATE + INTERVAL '7 days' AS next_week,
    CURRENT_DATE - INTERVAL '1 month' AS last_month,
    AGE(TIMESTAMP '2024-01-15', TIMESTAMP '2020-01-15') AS age_interval,
    EXTRACT(YEAR FROM CURRENT_DATE) AS current_year,
    EXTRACT(MONTH FROM CURRENT_DATE) AS current_month,
    DATE_PART('day', CURRENT_DATE) AS day_of_month;
```

**Types booléens :**
```sql
BOOLEAN (BOOL)  -- TRUE, FALSE, NULL

CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    is_active BOOLEAN DEFAULT TRUE,
    is_verified BOOLEAN NOT NULL,
    email_confirmed BOOL
);

-- Utilisation
INSERT INTO users (is_active, is_verified) 
VALUES (TRUE, FALSE);

SELECT * FROM users WHERE is_active = TRUE;
SELECT * FROM users WHERE NOT is_verified;
```

**Types avancés :**
```sql
-- JSON/JSONB
JSON            -- Texte JSON stocké tel quel
JSONB           -- JSON binaire (plus performant, indexable)

CREATE TABLE events (
    id SERIAL PRIMARY KEY,
    event_data JSON,
    metadata JSONB
);

INSERT INTO events (event_data, metadata) VALUES 
('{"type": "click", "value": 123}', 
 '{"user_id": 1, "timestamp": "2024-01-15"}');

-- Requêtes JSON
SELECT 
    event_data->>'type' AS event_type,
    metadata->>'user_id' AS user_id
FROM events;

-- Array
CREATE TABLE articles (
    id SERIAL PRIMARY KEY,
    tags TEXT[],
    ratings INTEGER[]
);

INSERT INTO articles (tags, ratings) VALUES 
(ARRAY['tech', 'database', 'sql'], ARRAY[5, 4, 5]);

-- Requêtes Array
SELECT * FROM articles WHERE 'database' = ANY(tags);
SELECT * FROM articles WHERE tags @> ARRAY['tech'];

-- UUID
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE TABLE sessions (
    session_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id INTEGER,
    created_at TIMESTAMP DEFAULT NOW()
);

-- ENUM
CREATE TYPE order_status AS ENUM ('pending', 'processing', 'shipped', 'delivered', 'cancelled');

CREATE TABLE orders (
    id SERIAL PRIMARY KEY,
    status order_status NOT NULL DEFAULT 'pending'
);
```

---

### **Création et Modification de Tables**

**CREATE TABLE - Créer une table :**
```sql
-- Syntaxe de base
CREATE TABLE table_name (
    column1 datatype constraints,
    column2 datatype constraints,
    ...
    table_constraints
);

-- Exemple complet
CREATE TABLE customers (
    customer_id SERIAL PRIMARY KEY,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    phone VARCHAR(20),
    date_of_birth DATE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE,
    
    CONSTRAINT email_format CHECK (email ~* '^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$')
);

-- Table avec foreign key
CREATE TABLE orders (
    order_id SERIAL PRIMARY KEY,
    customer_id INTEGER NOT NULL,
    order_date DATE NOT NULL DEFAULT CURRENT_DATE,
    total_amount NUMERIC(10, 2) NOT NULL CHECK (total_amount >= 0),
    status VARCHAR(20) DEFAULT 'pending',
    
    CONSTRAINT fk_customer
        FOREIGN KEY (customer_id) 
        REFERENCES customers(customer_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
);

-- Table avec contraintes multiples
CREATE TABLE order_items (
    item_id SERIAL PRIMARY KEY,
    order_id INTEGER NOT NULL,
    product_id INTEGER NOT NULL,
    quantity INTEGER NOT NULL CHECK (quantity > 0),
    unit_price NUMERIC(10, 2) NOT NULL CHECK (unit_price >= 0),
    discount NUMERIC(5, 2) DEFAULT 0 CHECK (discount >= 0 AND discount <= 100),
    
    CONSTRAINT fk_order
        FOREIGN KEY (order_id) REFERENCES orders(order_id)
        ON DELETE CASCADE,
    CONSTRAINT fk_product
        FOREIGN KEY (product_id) REFERENCES products(product_id)
        ON DELETE RESTRICT,
    CONSTRAINT unique_order_product
        UNIQUE (order_id, product_id)
);
```

**ALTER TABLE - Modifier une table :**
```sql
-- Ajouter une colonne
ALTER TABLE customers 
ADD COLUMN address TEXT;

ALTER TABLE customers 
ADD COLUMN country VARCHAR(50) DEFAULT 'France';

-- Supprimer une colonne
ALTER TABLE customers 
DROP COLUMN phone;

-- Modifier le type d'une colonne
ALTER TABLE customers 
ALTER COLUMN email TYPE VARCHAR(150);

-- Ajouter une contrainte NOT NULL
ALTER TABLE customers 
ALTER COLUMN last_name SET NOT NULL;

-- Supprimer une contrainte NOT NULL
ALTER TABLE customers 
ALTER COLUMN phone DROP NOT NULL;

-- Ajouter une valeur par défaut
ALTER TABLE orders 
ALTER COLUMN status SET DEFAULT 'pending';

-- Supprimer une valeur par défaut
ALTER TABLE orders 
ALTER COLUMN status DROP DEFAULT;

-- Renommer une colonne
ALTER TABLE customers 
RENAME COLUMN first_name TO firstname;

-- Renommer une table
ALTER TABLE customers 
RENAME TO clients;

-- Ajouter une contrainte
ALTER TABLE orders 
ADD CONSTRAINT check_amount 
CHECK (total_amount >= 0);

-- Supprimer une contrainte
ALTER TABLE orders 
DROP CONSTRAINT check_amount;

-- Ajouter une clé primaire
ALTER TABLE products 
ADD PRIMARY KEY (product_id);

-- Ajouter une clé étrangère
ALTER TABLE orders 
ADD CONSTRAINT fk_customer 
FOREIGN KEY (customer_id) 
REFERENCES customers(customer_id);
```

**DROP TABLE - Supprimer une table :**
```sql
-- Supprimer une table
DROP TABLE customers;

-- Supprimer si existe
DROP TABLE IF EXISTS customers;

-- Supprimer en cascade (supprime aussi les dépendances)
DROP TABLE customers CASCADE;

-- Supprimer plusieurs tables
DROP TABLE IF EXISTS orders, order_items, products;
```

**TRUNCATE - Vider une table :**
```sql
-- Vider complètement une table (plus rapide que DELETE)
TRUNCATE TABLE orders;

-- Vider et réinitialiser les sequences
TRUNCATE TABLE orders RESTART IDENTITY;

-- Vider en cascade
TRUNCATE TABLE customers CASCADE;
```

---

### **Contraintes d'Intégrité**

**PRIMARY KEY - Clé primaire :**
```sql
-- Définition inline
CREATE TABLE products (
    product_id SERIAL PRIMARY KEY,
    name VARCHAR(100)
);

-- Définition en fin de table
CREATE TABLE products (
    product_id INTEGER,
    name VARCHAR(100),
    PRIMARY KEY (product_id)
);

-- Clé primaire composite
CREATE TABLE order_items (
    order_id INTEGER,
    product_id INTEGER,
    quantity INTEGER,
    PRIMARY KEY (order_id, product_id)
);
```

**FOREIGN KEY - Clé étrangère :**
```sql
CREATE TABLE orders (
    order_id SERIAL PRIMARY KEY,
    customer_id INTEGER NOT NULL,
    
    CONSTRAINT fk_customer
        FOREIGN KEY (customer_id) 
        REFERENCES customers(customer_id)
        ON DELETE CASCADE      -- Supprime les commandes si client supprimé
        ON UPDATE CASCADE      -- Met à jour si ID client change
);

-- Options de ON DELETE et ON UPDATE:
-- CASCADE: Propage la modification
-- SET NULL: Met la valeur à NULL
-- SET DEFAULT: Met la valeur par défaut
-- RESTRICT: Empêche la modification
-- NO ACTION: Identique à RESTRICT

CREATE TABLE order_items (
    item_id SERIAL PRIMARY KEY,
    order_id INTEGER,
    product_id INTEGER,
    
    FOREIGN KEY (order_id) REFERENCES orders(order_id) ON DELETE CASCADE,
    FOREIGN KEY (product_id) REFERENCES products(product_id) ON DELETE RESTRICT
);
```

**UNIQUE - Contrainte d'unicité :**
```sql
-- Colonne unique
CREATE TABLE users (
    user_id SERIAL PRIMARY KEY,
    email VARCHAR(100) UNIQUE NOT NULL,
    username VARCHAR(50) UNIQUE
);

-- Contrainte nommée
CREATE TABLE users (
    user_id SERIAL PRIMARY KEY,
    email VARCHAR(100),
    
    CONSTRAINT unique_email UNIQUE (email)
);

-- Contrainte composite (combinaison unique)
CREATE TABLE enrollments (
    student_id INTEGER,
    course_id INTEGER,
    semester VARCHAR(20),
    
    UNIQUE (student_id, course_id, semester)
);
```

**CHECK - Contrainte de vérification :**
```sql
CREATE TABLE products (
    product_id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    price NUMERIC(10, 2) CHECK (price > 0),
    discount NUMERIC(5, 2) CHECK (discount >= 0 AND discount <= 100),
    stock_quantity INTEGER CHECK (stock_quantity >= 0),
    
    -- Contrainte sur plusieurs colonnes
    CONSTRAINT valid_pricing 
        CHECK (price > 0 AND discount < price)
);

-- Exemples de CHECK
CREATE TABLE employees (
    employee_id SERIAL PRIMARY KEY,
    age INTEGER CHECK (age >= 18 AND age <= 65),
    salary NUMERIC(10, 2) CHECK (salary > 0),
    email VARCHAR(100) CHECK (email ~* '^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$'),
    hire_date DATE CHECK (hire_date <= CURRENT_DATE),
    gender CHAR(1) CHECK (gender IN ('M', 'F', 'O'))
);
```

**NOT NULL - Contrainte de non-nullité :**
```sql
CREATE TABLE customers (
    customer_id SERIAL PRIMARY KEY,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    email VARCHAR(100) NOT NULL,
    phone VARCHAR(20)  -- Peut être NULL
);
```

**DEFAULT - Valeur par défaut :**
```sql
CREATE TABLE orders (
    order_id SERIAL PRIMARY KEY,
    order_date DATE DEFAULT CURRENT_DATE,
    status VARCHAR(20) DEFAULT 'pending',
    created_at TIMESTAMP DEFAULT NOW(),
    is_paid BOOLEAN DEFAULT FALSE,
    quantity INTEGER DEFAULT 1
);
```

---

### **Requêtes SELECT de Base**

**SELECT simple :**
```sql
-- Sélectionner toutes les colonnes
SELECT * FROM customers;

-- Sélectionner des colonnes spécifiques
SELECT first_name, last_name, email FROM customers;

-- Sélectionner avec alias
SELECT 
    first_name AS prenom,
    last_name AS nom,
    email AS adresse_email
FROM customers;

-- Calculs dans SELECT
SELECT 
    product_name,
    price,
    quantity,
    price * quantity AS total_value
FROM products;

-- Concaténation
SELECT 
    first_name || ' ' || last_name AS full_name,
    'Customer: ' || customer_id AS customer_ref
FROM customers;
```

**WHERE - Filtrage :**
```sql
-- Comparaisons simples
SELECT * FROM products WHERE price > 100;
SELECT * FROM customers WHERE country = 'France';
SELECT * FROM orders WHERE order_date = '2024-01-15';

-- Opérateurs de comparaison: =, !=, <>, >, <, >=, <=

-- Combinaisons logiques (AND, OR, NOT)
SELECT * FROM products 
WHERE price > 50 AND price < 200;

SELECT * FROM products 
WHERE category = 'Electronics' OR category = 'Books';

SELECT * FROM customers 
WHERE country = 'France' AND city = 'Paris';

SELECT * FROM products 
WHERE NOT (price > 1000);

-- BETWEEN
SELECT * FROM products 
WHERE price BETWEEN 50 AND 200;

SELECT * FROM orders 
WHERE order_date BETWEEN '2024-01-01' AND '2024-12-31';

-- IN
SELECT * FROM customers 
WHERE country IN ('France', 'Germany', 'Spain');

SELECT * FROM products 
WHERE product_id IN (1, 5, 10, 15);

-- LIKE (pattern matching)
SELECT * FROM customers 
WHERE email LIKE '%@gmail.com';

SELECT * FROM products 
WHERE product_name LIKE 'Samsung%';  -- Commence par Samsung

SELECT * FROM customers 
WHERE last_name LIKE '_mith';  -- _mith (Smith, Amith, etc.)

-- ILIKE (case insensitive)
SELECT * FROM customers 
WHERE email ILIKE '%GMAIL.COM';

-- IS NULL / IS NOT NULL
SELECT * FROM customers WHERE phone IS NULL;
SELECT * FROM customers WHERE phone IS NOT NULL;
```

**ORDER BY - Tri :**
```sql
-- Tri ascendant (par défaut)
SELECT * FROM products 
ORDER BY price;

-- Tri descendant
SELECT * FROM products 
ORDER BY price DESC;

-- Tri multiple
SELECT * FROM customers 
ORDER BY country ASC, last_name ASC;

-- Tri avec expression
SELECT * FROM products 
ORDER BY price * quantity DESC;

-- NULLS FIRST / NULLS LAST
SELECT * FROM customers 
ORDER BY phone NULLS LAST;
```

**LIMIT et OFFSET - Pagination :**
```sql
-- Limiter le nombre de résultats
SELECT * FROM products 
LIMIT 10;

-- Pagination (sauter les premiers résultats)
SELECT * FROM products 
ORDER BY product_id
LIMIT 10 OFFSET 20;  -- Page 3 (résultats 21-30)

-- Exemple pagination
-- Page 1: LIMIT 10 OFFSET 0
-- Page 2: LIMIT 10 OFFSET 10
-- Page 3: LIMIT 10 OFFSET 20
```

**DISTINCT - Valeurs uniques :**
```sql
-- Éliminer les doublons
SELECT DISTINCT country FROM customers;

SELECT DISTINCT category FROM products;

-- DISTINCT sur plusieurs colonnes
SELECT DISTINCT country, city FROM customers;
```

---

### **Fonctions d'Agrégation**

**Fonctions de base :**
```sql
-- COUNT: Compter les lignes
SELECT COUNT(*) FROM customers;
SELECT COUNT(phone) FROM customers;  -- Ne compte pas les NULL
SELECT COUNT(DISTINCT country) FROM customers;

-- SUM: Somme
SELECT SUM(total_amount) FROM orders;
SELECT SUM(price * quantity) AS total_value FROM order_items;

-- AVG: Moyenne
SELECT AVG(price) FROM products;
SELECT AVG(total_amount) FROM orders;

-- MIN: Minimum
SELECT MIN(price) FROM products;
SELECT MIN(order_date) FROM orders;

-- MAX: Maximum
SELECT MAX(price) FROM products;
SELECT MAX(order_date) FROM orders;

-- Exemples combinés
SELECT 
    COUNT(*) AS total_products,
    AVG(price) AS average_price,
    MIN(price) AS cheapest,
    MAX(price) AS most_expensive,
    SUM(stock_quantity) AS total_stock
FROM products;
```

**GROUP BY - Regroupement :**
```sql
-- Regrouper par une colonne
SELECT 
    country,
    COUNT(*) AS customer_count
FROM customers
GROUP BY country;

-- Regrouper par plusieurs colonnes
SELECT 
    country,
    city,
    COUNT(*) AS customer_count
FROM customers
GROUP BY country, city;

-- Avec agrégations multiples
SELECT 
    category,
    COUNT(*) AS product_count,
    AVG(price) AS avg_price,
    SUM(stock_quantity) AS total_stock
FROM products
GROUP BY category;

-- Exemples pratiques
-- Commandes par client
SELECT 
    customer_id,
    COUNT(*) AS order_count,
    SUM(total_amount) AS total_spent,
    AVG(total_amount) AS avg_order_value
FROM orders
GROUP BY customer_id;

-- Ventes par mois
SELECT 
    EXTRACT(YEAR FROM order_date) AS year,
    EXTRACT(MONTH FROM order_date) AS month,
    COUNT(*) AS order_count,
    SUM(total_amount) AS monthly_revenue
FROM orders
GROUP BY EXTRACT(YEAR FROM order_date), EXTRACT(MONTH FROM order_date)
ORDER BY year, month;
```

**HAVING - Filtrage après regroupement :**
```sql
-- WHERE filtre AVANT le groupement
-- HAVING filtre APRÈS le groupement

-- Pays avec plus de 100 clients
SELECT 
    country,
    COUNT(*) AS customer_count
FROM customers
GROUP BY country
HAVING COUNT(*) > 100;

-- Clients ayant dépensé plus de 1000€
SELECT 
    customer_id,
    COUNT(*) AS order_count,
    SUM(total_amount) AS total_spent
FROM orders
GROUP BY customer_id
HAVING SUM(total_amount) > 1000;

-- Catégories avec prix moyen > 100€
SELECT 
    category,
    COUNT(*) AS product_count,
    AVG(price) AS avg_price
FROM products
GROUP BY category
HAVING AVG(price) > 100;

-- Combinaison WHERE et HAVING
SELECT 
    category,
    COUNT(*) AS product_count,
    AVG(price) AS avg_price
FROM products
WHERE stock_quantity > 0  -- Filtrer AVANT le groupement
GROUP BY category
HAVING COUNT(*) >= 5;     -- Filtrer APRÈS le groupement
```

---

### **Jointures (JOINS)**

**INNER JOIN - Jointure interne :**
```sql
-- Syntaxe de base
SELECT 
    orders.order_id,
    customers.first_name,
    customers.last_name,
    orders.total_amount
FROM orders
INNER JOIN customers ON orders.customer_id = customers.customer_id;

-- Avec alias
SELECT 
    o.order_id,
    c.first_name,
    c.last_name,
    o.total_amount,
    o.order_date
FROM orders o
INNER JOIN customers c ON o.customer_id = c.customer_id;

-- Jointure multiple
SELECT 
    o.order_id,
    c.first_name || ' ' || c.last_name AS customer_name,
    p.product_name,
    oi.quantity,
    oi.unit_price
FROM orders o
INNER JOIN customers c ON o.customer_id = c.customer_id
INNER JOIN order_items oi ON o.order_id = oi.order_id
INNER JOIN products p ON oi.product_id = p.product_id;
```

**LEFT JOIN (LEFT OUTER JOIN) - Jointure gauche :**
```sql
-- Retourne toutes les lignes de la table de gauche
-- même si pas de correspondance à droite

SELECT 
    c.customer_id,
    c.first_name,
    c.last_name,
    COUNT(o.order_id) AS order_count
FROM customers c
LEFT JOIN orders o ON c.customer_id = o.customer_id
GROUP BY c.customer_id, c.first_name, c.last_name;

-- Trouver les clients sans commande
SELECT 
    c.customer_id,
    c.first_name,
    c.last_name
FROM customers c
LEFT JOIN orders o ON c.customer_id = o.customer_id
WHERE o.order_id IS NULL;
```

**RIGHT JOIN (RIGHT OUTER JOIN) - Jointure droite :**
```sql
-- Retourne toutes les lignes de la table de droite

SELECT 
    o.order_id,
    o.order_date,
    c.first_name,
    c.last_name
FROM orders o
RIGHT JOIN customers c ON o.customer_id = c.customer_id;

-- Équivalent à:
SELECT 
    o.order_id,
    o.order_date,
    c.first_name,
    c.last_name
FROM customers c
LEFT JOIN orders o ON o.customer_id = c.customer_id;
```

**FULL OUTER JOIN - Jointure externe complète :**
```sql
-- Retourne toutes les lignes des deux tables

SELECT 
    c.customer_id,
    c.first_name,
    o.order_id,
    o.order_date
FROM customers c
FULL OUTER JOIN orders o ON c.customer_id = o.customer_id;
```

**CROSS JOIN - Produit cartésien :**
```sql
-- Chaque ligne de la première table avec chaque ligne de la seconde

SELECT 
    c.category_name,
    s.size_name
FROM categories c
CROSS JOIN sizes s;

-- Équivalent à:
SELECT 
    c.category_name,
    s.size_name
FROM categories c, sizes s;
```

**SELF JOIN - Auto-jointure :**
```sql
-- Joindre une table avec elle-même

CREATE TABLE employees (
    employee_id SERIAL PRIMARY KEY,
    name VARCHAR(100),
    manager_id INTEGER
);

-- Trouver les employés avec leur manager
SELECT 
    e.name AS employee_name,
    m.name AS manager_name
FROM employees e
LEFT JOIN employees m ON e.manager_id = m.employee_id;
```

---

### **Sous-requêtes (Subqueries)**

**Sous-requête dans WHERE :**
```sql
-- Trouver les produits plus chers que la moyenne
SELECT product_name, price
FROM products
WHERE price > (SELECT AVG(price) FROM products);

-- Clients ayant passé des commandes
SELECT *
FROM customers
WHERE customer_id IN (
    SELECT DISTINCT customer_id FROM orders
);

-- Produits jamais commandés
SELECT *
FROM products
WHERE product_id NOT IN (
    SELECT DISTINCT product_id FROM order_items
);
```

**Sous-requête dans SELECT :**
```sql
SELECT 
    c.customer_id,
    c.first_name,
    c.last_name,
    (SELECT COUNT(*) 
     FROM orders o 
     WHERE o.customer_id = c.customer_id) AS order_count,
    (SELECT SUM(total_amount) 
     FROM orders o 
     WHERE o.customer_id = c.customer_id) AS total_spent
FROM customers c;
```

**Sous-requête dans FROM (table dérivée) :**
```sql
SELECT 
    avg_prices.category,
    avg_prices.avg_price
FROM (
    SELECT 
        category,
        AVG(price) AS avg_price
    FROM products
    GROUP BY category
) AS avg_prices
WHERE avg_prices.avg_price > 100;
```

**EXISTS et NOT EXISTS :**
```sql
-- Clients ayant au moins une commande
SELECT *
FROM customers c
WHERE EXISTS (
    SELECT 1 FROM orders o 
    WHERE o.customer_id = c.customer_id
);

-- Clients sans commande
SELECT *
FROM customers c
WHERE NOT EXISTS (
    SELECT 1 FROM orders o 
    WHERE o.customer_id = c.customer_id
);
```

**ANY et ALL :**
```sql
-- Prix supérieur à n'importe quel produit de la catégorie 'Books'
SELECT *
FROM products
WHERE price > ANY (
    SELECT price FROM products WHERE category = 'Books'
);

-- Prix supérieur à tous les produits de la catégorie 'Books'
SELECT *
FROM products
WHERE price > ALL (
    SELECT price FROM products WHERE category = 'Books'
);
```

---

### **Opérations de Données (DML)**

**INSERT - Insertion :**
```sql
-- Insertion simple
INSERT INTO customers (first_name, last_name, email)
VALUES ('John', 'Doe', 'john.doe@email.com');

-- Insertion multiple
INSERT INTO customers (first_name, last_name, email) VALUES
('Jane', 'Smith', 'jane.smith@email.com'),
('Bob', 'Johnson', 'bob.johnson@email.com'),
('Alice', 'Williams', 'alice.williams@email.com');

-- Insertion avec toutes les colonnes
INSERT INTO customers 
VALUES (DEFAULT, 'Mike', 'Brown', 'mike.brown@email.com', '0123456789', '1990-05-15', NOW(), NOW(), TRUE);

-- Insertion avec RETURNING
INSERT INTO customers (first_name, last_name, email)
VALUES ('Sarah', 'Davis', 'sarah.davis@email.com')
RETURNING customer_id, created_at;

-- Insertion depuis une autre table
INSERT INTO archived_orders
SELECT * FROM orders WHERE order_date < '2023-01-01';
```

**UPDATE - Mise à jour :**
```sql
-- Mise à jour simple
UPDATE customers
SET email = 'newemail@example.com'
WHERE customer_id = 1;

-- Mise à jour multiple colonnes
UPDATE products
SET 
    price = price * 1.1,
    updated_at = NOW()
WHERE category = 'Electronics';

-- Mise à jour avec calcul
UPDATE order_items
SET total_price = unit_price * quantity;

-- Mise à jour avec RETURNING
UPDATE customers
SET is_active = FALSE
WHERE customer_id = 5
RETURNING *;

-- Mise à jour depuis une autre table
UPDATE products p
SET stock_quantity = stock_quantity - oi.quantity
FROM order_items oi
WHERE p.product_id = oi.product_id AND oi.order_id = 100;
```

**DELETE - Suppression :**
```sql
-- Suppression simple
DELETE FROM customers
WHERE customer_id = 1;

-- Suppression conditionnelle
DELETE FROM orders
WHERE order_date < '2020-01-01';

-- Suppression avec RETURNING
DELETE FROM products
WHERE stock_quantity = 0
RETURNING product_id, product_name;

-- Suppression avec sous-requête
DELETE FROM customers
WHERE customer_id NOT IN (
    SELECT DISTINCT customer_id FROM orders
);

-- Supprimer toutes les lignes (attention !)
DELETE FROM temp_table;
```

---

### **Common Table Expressions (CTE) - WITH**

**CTE simple :**
```sql
-- Définir une requête temporaire réutilisable
WITH customer_orders AS (
    SELECT 
        customer_id,
        COUNT(*) AS order_count,
        SUM(total_amount) AS total_spent
    FROM orders
    GROUP BY customer_id
)
SELECT 
    c.first_name,
    c.last_name,
    co.order_count,
    co.total_spent
FROM customers c
JOIN customer_orders co ON c.customer_id = co.customer_id
WHERE co.total_spent > 1000;
```

**CTE multiple :**
```sql
WITH 
    high_value_customers AS (
        SELECT customer_id, SUM(total_amount) AS total_spent
        FROM orders
        GROUP BY customer_id
        HAVING SUM(total_amount) > 5000
    ),
    recent_orders AS (
        SELECT customer_id, COUNT(*) AS recent_order_count
        FROM orders
        WHERE order_date >= CURRENT_DATE - INTERVAL '30 days'
        GROUP BY customer_id
    )
SELECT 
    c.first_name,
    c.last_name,
    hvc.total_spent,
    COALESCE(ro.recent_order_count, 0) AS recent_orders
FROM customers c
JOIN high_value_customers hvc ON c.customer_id = hvc.customer_id
LEFT JOIN recent_orders ro ON c.customer_id = ro.customer_id;
```

**CTE récursif :**
```sql
-- Exemple: Hiérarchie d'employés
WITH RECURSIVE employee_hierarchy AS (
    -- Cas de base: managers sans supérieur
    SELECT 
        employee_id,
        name,
        manager_id,
        1 AS level,
        name AS path
    FROM employees
    WHERE manager_id IS NULL
    
    UNION ALL
    
    -- Cas récursif
    SELECT 
        e.employee_id,
        e.name,
        e.manager_id,
        eh.level + 1,
        eh.path || ' -> ' || e.name
    FROM employees e
    JOIN employee_hierarchy eh ON e.manager_id = eh.employee_id
)
SELECT * FROM employee_hierarchy
ORDER BY level, name;
```

---

### **Fonctions de Fenêtrage (Window Functions)**

```sql
**ROW_NUMBER, RANK, DENSE_RANK :**
```sql
-- ROW_NUMBER: Numéro de ligne unique
SELECT 
    product_name,
    category,
    price,
    ROW_NUMBER() OVER (ORDER BY price DESC) AS row_num
FROM products;

-- Numérotation par groupe
SELECT 
    product_name,
    category,
    price,
    ROW_NUMBER() OVER (PARTITION BY category ORDER BY price DESC) AS row_num_in_category
FROM products;

-- RANK: Même rang pour valeurs égales, puis saute des rangs
SELECT 
    product_name,
    price,
    RANK() OVER (ORDER BY price DESC) AS rank
FROM products;
-- Ex: 1, 2, 2, 4, 5 (saute le 3)

-- DENSE_RANK: Même rang pour valeurs égales, sans sauter
SELECT 
    product_name,
    price,
    DENSE_RANK() OVER (ORDER BY price DESC) AS dense_rank
FROM products;
-- Ex: 1, 2, 2, 3, 4 (ne saute pas)

-- Exemple pratique: Top 3 produits par catégorie
WITH ranked_products AS (
    SELECT 
        product_name,
        category,
        price,
        ROW_NUMBER() OVER (PARTITION BY category ORDER BY price DESC) AS rank
    FROM products
)
SELECT * FROM ranked_products
WHERE rank <= 3;
```

**Fonctions d'agrégation en fenêtre :**
```sql
-- SUM cumulatif
SELECT 
    order_id,
    order_date,
    total_amount,
    SUM(total_amount) OVER (ORDER BY order_date) AS cumulative_total
FROM orders;

-- Moyenne mobile (3 jours)
SELECT 
    order_date,
    total_amount,
    AVG(total_amount) OVER (
        ORDER BY order_date 
        ROWS BETWEEN 2 PRECEDING AND CURRENT ROW
    ) AS moving_avg_3days
FROM orders;

-- Total par catégorie
SELECT 
    product_name,
    category,
    price,
    SUM(price) OVER (PARTITION BY category) AS category_total,
    price / SUM(price) OVER (PARTITION BY category) * 100 AS percentage_of_category
FROM products;
```

**LAG et LEAD - Valeurs précédentes/suivantes :**
```sql
-- LAG: Valeur de la ligne précédente
SELECT 
    order_date,
    total_amount,
    LAG(total_amount) OVER (ORDER BY order_date) AS previous_day_amount,
    total_amount - LAG(total_amount) OVER (ORDER BY order_date) AS difference
FROM orders;

-- LEAD: Valeur de la ligne suivante
SELECT 
    order_date,
    total_amount,
    LEAD(total_amount) OVER (ORDER BY order_date) AS next_day_amount
FROM orders;

-- Avec offset et valeur par défaut
SELECT 
    order_date,
    total_amount,
    LAG(total_amount, 7, 0) OVER (ORDER BY order_date) AS amount_week_ago
FROM orders;
```

**FIRST_VALUE et LAST_VALUE :**
```sql
-- Première et dernière valeur dans la fenêtre
SELECT 
    product_name,
    category,
    price,
    FIRST_VALUE(price) OVER (
        PARTITION BY category 
        ORDER BY price
    ) AS cheapest_in_category,
    LAST_VALUE(price) OVER (
        PARTITION BY category 
        ORDER BY price
        RANGE BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING
    ) AS most_expensive_in_category
FROM products;
```

**NTH_VALUE - N-ième valeur :**
```sql
-- Obtenir la 2ème valeur la plus élevée
SELECT 
    product_name,
    category,
    price,
    NTH_VALUE(price, 2) OVER (
        PARTITION BY category 
        ORDER BY price DESC
        RANGE BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING
    ) AS second_highest_price
FROM products;
```

**NTILE - Diviser en groupes :**
```sql
-- Diviser en quartiles
SELECT 
    customer_id,
    total_spent,
    NTILE(4) OVER (ORDER BY total_spent DESC) AS quartile
FROM (
    SELECT 
        customer_id,
        SUM(total_amount) AS total_spent
    FROM orders
    GROUP BY customer_id
) customer_totals;

-- Diviser clients en 3 segments (high, medium, low value)
SELECT 
    customer_id,
    total_spent,
    CASE NTILE(3) OVER (ORDER BY total_spent DESC)
        WHEN 1 THEN 'High Value'
        WHEN 2 THEN 'Medium Value'
        WHEN 3 THEN 'Low Value'
    END AS customer_segment
FROM customer_totals;
```

---

### **Index - Optimisation des Performances**

**Types d'index :**
```sql
-- Index B-tree (par défaut) - Le plus courant
CREATE INDEX idx_customers_email ON customers(email);

-- Index sur plusieurs colonnes (composite)
CREATE INDEX idx_orders_customer_date ON orders(customer_id, order_date);

-- Index unique
CREATE UNIQUE INDEX idx_unique_email ON customers(email);

-- Index partiel (avec condition)
CREATE INDEX idx_active_customers ON customers(customer_id)
WHERE is_active = TRUE;

-- Index sur expression
CREATE INDEX idx_lower_email ON customers(LOWER(email));

-- Index HASH (pour égalité uniquement)
CREATE INDEX idx_hash_code ON products USING HASH (product_code);

-- Index GIN (pour arrays, JSON, full-text search)
CREATE INDEX idx_tags ON articles USING GIN(tags);
CREATE INDEX idx_jsonb_data ON events USING GIN(event_data);

-- Index GiST (pour données géométriques)
CREATE INDEX idx_location ON stores USING GIST(location);
```

**Gestion des index :**
```sql
-- Lister les index d'une table
SELECT 
    indexname,
    indexdef
FROM pg_indexes
WHERE tablename = 'customers';

-- Supprimer un index
DROP INDEX idx_customers_email;

-- Supprimer si existe
DROP INDEX IF EXISTS idx_customers_email;

-- Reconstruire un index (maintenance)
REINDEX INDEX idx_customers_email;
REINDEX TABLE customers;

-- Analyser l'utilisation des index
SELECT 
    schemaname,
    tablename,
    indexname,
    idx_scan,
    idx_tup_read,
    idx_tup_fetch
FROM pg_stat_user_indexes
WHERE tablename = 'customers';
```

**EXPLAIN - Analyser les requêtes :**
```sql
-- Plan d'exécution simple
EXPLAIN 
SELECT * FROM customers WHERE email = 'john@example.com';

-- Plan détaillé avec coûts
EXPLAIN ANALYZE
SELECT * FROM customers WHERE email = 'john@example.com';

-- Exemple de lecture du plan:
/*
Seq Scan on customers  (cost=0.00..15.50 rows=1 width=100)
  Filter: (email = 'john@example.com')
  
Index Scan using idx_customers_email on customers  (cost=0.29..8.30 rows=1 width=100)
  Index Cond: (email = 'john@example.com')
*/

-- EXPLAIN ANALYZE avec buffers
EXPLAIN (ANALYZE, BUFFERS)
SELECT 
    c.customer_id,
    c.first_name,
    COUNT(o.order_id) AS order_count
FROM customers c
LEFT JOIN orders o ON c.customer_id = o.customer_id
GROUP BY c.customer_id, c.first_name;
```

**Conseils d'indexation :**
```sql
-- Index sur les colonnes fréquemment utilisées dans WHERE
CREATE INDEX idx_orders_status ON orders(status);

-- Index sur les colonnes de jointure
CREATE INDEX idx_orders_customer_id ON orders(customer_id);
CREATE INDEX idx_order_items_order_id ON order_items(order_id);

-- Index sur les colonnes de tri (ORDER BY)
CREATE INDEX idx_products_price ON products(price DESC);

-- Index couvrant (covering index) - inclut toutes les colonnes nécessaires
CREATE INDEX idx_orders_covering ON orders(customer_id, order_date)
INCLUDE (total_amount, status);

-- ATTENTION: Trop d'index ralentit INSERT/UPDATE/DELETE
-- Ne créer que les index réellement utiles
```

---

### **Transactions**

**Principes ACID :**
```sql
-- Atomicity: Tout ou rien
-- Consistency: Respect des contraintes
-- Isolation: Transactions indépendantes
-- Durability: Modifications persistantes
```

**Utilisation des transactions :**
```sql
-- Transaction simple
BEGIN;
    UPDATE accounts SET balance = balance - 100 WHERE account_id = 1;
    UPDATE accounts SET balance = balance + 100 WHERE account_id = 2;
COMMIT;

-- En cas d'erreur, annuler
BEGIN;
    UPDATE accounts SET balance = balance - 100 WHERE account_id = 1;
    -- Une erreur se produit
    UPDATE accounts SET balance = balance + 100 WHERE account_id = 999;
ROLLBACK;

-- Avec savepoints
BEGIN;
    INSERT INTO orders (customer_id, total_amount) 
    VALUES (1, 100);
    
    SAVEPOINT order_created;
    
    INSERT INTO order_items (order_id, product_id, quantity) 
    VALUES (1, 10, 5);
    
    -- Erreur détectée, revenir au savepoint
    ROLLBACK TO order_created;
    
    -- Continuer avec d'autres opérations
    INSERT INTO order_items (order_id, product_id, quantity) 
    VALUES (1, 11, 3);
COMMIT;
```

**Niveaux d'isolation :**
```sql
-- READ UNCOMMITTED (PostgreSQL ne supporte pas, utilise READ COMMITTED)
-- READ COMMITTED (défaut): Lit uniquement les données commitées
BEGIN TRANSACTION ISOLATION LEVEL READ COMMITTED;
    SELECT * FROM accounts WHERE account_id = 1;
COMMIT;

-- REPEATABLE READ: Voit un snapshot au début de la transaction
BEGIN TRANSACTION ISOLATION LEVEL REPEATABLE READ;
    SELECT * FROM accounts WHERE account_id = 1;
    -- Autres transactions ne peuvent pas modifier cette ligne
    UPDATE accounts SET balance = balance + 100 WHERE account_id = 1;
COMMIT;

-- SERIALIZABLE: Isolation complète
BEGIN TRANSACTION ISOLATION LEVEL SERIALIZABLE;
    SELECT SUM(balance) FROM accounts;
    UPDATE accounts SET balance = balance * 1.05;
COMMIT;
```

**Verrouillage (Locking) :**
```sql
-- Verrouillage en lecture (lecture seule)
BEGIN;
    SELECT * FROM products WHERE product_id = 1 FOR SHARE;
    -- Autres peuvent lire mais pas modifier
COMMIT;

-- Verrouillage en écriture (exclusif)
BEGIN;
    SELECT * FROM products WHERE product_id = 1 FOR UPDATE;
    -- Autres ne peuvent ni lire ni modifier
    UPDATE products SET stock_quantity = stock_quantity - 1 
    WHERE product_id = 1;
COMMIT;

-- FOR UPDATE NOWAIT: Ne pas attendre si verrouillé
BEGIN;
    SELECT * FROM products WHERE product_id = 1 FOR UPDATE NOWAIT;
COMMIT;

-- FOR UPDATE SKIP LOCKED: Sauter les lignes verrouillées
BEGIN;
    SELECT * FROM queue_table 
    WHERE processed = FALSE 
    ORDER BY created_at 
    LIMIT 10
    FOR UPDATE SKIP LOCKED;
COMMIT;
```

---

### **Vues (Views)**

**Créer des vues :**
```sql
-- Vue simple
CREATE VIEW customer_orders_summary AS
SELECT 
    c.customer_id,
    c.first_name,
    c.last_name,
    c.email,
    COUNT(o.order_id) AS total_orders,
    COALESCE(SUM(o.total_amount), 0) AS total_spent,
    MAX(o.order_date) AS last_order_date
FROM customers c
LEFT JOIN orders o ON c.customer_id = o.customer_id
GROUP BY c.customer_id, c.first_name, c.last_name, c.email;

-- Utiliser la vue
SELECT * FROM customer_orders_summary
WHERE total_orders > 5;

-- Vue avec jointures complexes
CREATE VIEW order_details AS
SELECT 
    o.order_id,
    o.order_date,
    c.first_name || ' ' || c.last_name AS customer_name,
    c.email,
    p.product_name,
    oi.quantity,
    oi.unit_price,
    oi.quantity * oi.unit_price AS line_total
FROM orders o
JOIN customers c ON o.customer_id = c.customer_id
JOIN order_items oi ON o.order_id = oi.order_id
JOIN products p ON oi.product_id = p.product_id;
```

**Vues matérialisées (Materialized Views) :**
```sql
-- Vue matérialisée: stocke physiquement les résultats
CREATE MATERIALIZED VIEW monthly_sales_summary AS
SELECT 
    DATE_TRUNC('month', order_date) AS month,
    COUNT(*) AS order_count,
    SUM(total_amount) AS total_revenue,
    AVG(total_amount) AS avg_order_value
FROM orders
GROUP BY DATE_TRUNC('month', order_date)
WITH DATA;

-- Créer un index sur la vue matérialisée
CREATE INDEX idx_monthly_sales_month 
ON monthly_sales_summary(month);

-- Rafraîchir la vue matérialisée
REFRESH MATERIALIZED VIEW monthly_sales_summary;

-- Rafraîchir sans bloquer les lectures
REFRESH MATERIALIZED VIEW CONCURRENTLY monthly_sales_summary;
```

**Modifier et supprimer des vues :**
```sql
-- Modifier une vue
CREATE OR REPLACE VIEW customer_orders_summary AS
SELECT 
    c.customer_id,
    c.first_name,
    c.last_name,
    COUNT(o.order_id) AS total_orders
FROM customers c
LEFT JOIN orders o ON c.customer_id = o.customer_id
GROUP BY c.customer_id, c.first_name, c.last_name;

-- Supprimer une vue
DROP VIEW IF EXISTS customer_orders_summary;

-- Supprimer une vue matérialisée
DROP MATERIALIZED VIEW IF EXISTS monthly_sales_summary;
```

---

### **Fonctions et Procédures Stockées**

**Fonctions :**
```sql
-- Fonction simple
CREATE OR REPLACE FUNCTION calculate_discount(
    original_price NUMERIC,
    discount_percent NUMERIC
)
RETURNS NUMERIC AS $$
BEGIN
    RETURN original_price * (1 - discount_percent / 100);
END;
$$ LANGUAGE plpgsql;

-- Utilisation
SELECT calculate_discount(100, 10);  -- Retourne 90

-- Fonction avec logique conditionnelle
CREATE OR REPLACE FUNCTION get_customer_category(total_spent NUMERIC)
RETURNS VARCHAR AS $$
BEGIN
    IF total_spent >= 10000 THEN
        RETURN 'VIP';
    ELSIF total_spent >= 5000 THEN
        RETURN 'Gold';
    ELSIF total_spent >= 1000 THEN
        RETURN 'Silver';
    ELSE
        RETURN 'Bronze';
    END IF;
END;
$$ LANGUAGE plpgsql;

-- Fonction retournant une table
CREATE OR REPLACE FUNCTION get_customer_orders(p_customer_id INTEGER)
RETURNS TABLE(
    order_id INTEGER,
    order_date DATE,
    total_amount NUMERIC
) AS $$
BEGIN
    RETURN QUERY
    SELECT o.order_id, o.order_date, o.total_amount
    FROM orders o
    WHERE o.customer_id = p_customer_id
    ORDER BY o.order_date DESC;
END;
$$ LANGUAGE plpgsql;

-- Utilisation
SELECT * FROM get_customer_orders(1);

-- Fonction avec boucle
CREATE OR REPLACE FUNCTION calculate_factorial(n INTEGER)
RETURNS BIGINT AS $$
DECLARE
    result BIGINT := 1;
    i INTEGER;
BEGIN
    FOR i IN 1..n LOOP
        result := result * i;
    END LOOP;
    RETURN result;
END;
$$ LANGUAGE plpgsql;
```

**Procédures stockées (PostgreSQL 11+) :**
```sql
-- Procédure avec transaction
CREATE OR REPLACE PROCEDURE transfer_funds(
    from_account INTEGER,
    to_account INTEGER,
    amount NUMERIC
)
LANGUAGE plpgsql AS $$
BEGIN
    -- Vérifier le solde
    IF (SELECT balance FROM accounts WHERE account_id = from_account) < amount THEN
        RAISE EXCEPTION 'Insufficient funds';
    END IF;
    
    -- Débiter
    UPDATE accounts 
    SET balance = balance - amount 
    WHERE account_id = from_account;
    
    -- Créditer
    UPDATE accounts 
    SET balance = balance + amount 
    WHERE account_id = to_account;
    
    -- Logger la transaction
    INSERT INTO transaction_log (from_account, to_account, amount, created_at)
    VALUES (from_account, to_account, amount, NOW());
    
    COMMIT;
END;
$$;

-- Appeler une procédure
CALL transfer_funds(1, 2, 500);

-- Procédure avec gestion d'erreur
CREATE OR REPLACE PROCEDURE process_order(p_order_id INTEGER)
LANGUAGE plpgsql AS $$
BEGIN
    -- Mettre à jour le statut
    UPDATE orders SET status = 'processing' WHERE order_id = p_order_id;
    
    -- Décrémenter le stock
    UPDATE products p
    SET stock_quantity = stock_quantity - oi.quantity
    FROM order_items oi
    WHERE p.product_id = oi.product_id 
    AND oi.order_id = p_order_id;
    
    -- Marquer comme traité
    UPDATE orders SET status = 'completed' WHERE order_id = p_order_id;
    
    COMMIT;
EXCEPTION
    WHEN OTHERS THEN
        ROLLBACK;
        RAISE NOTICE 'Error processing order %: %', p_order_id, SQLERRM;
END;
$$;
```

**Supprimer fonctions et procédures :**
```sql
-- Supprimer une fonction
DROP FUNCTION IF EXISTS calculate_discount(NUMERIC, NUMERIC);

-- Supprimer une procédure
DROP PROCEDURE IF EXISTS transfer_funds(INTEGER, INTEGER, NUMERIC);
```

---

### **Triggers - Déclencheurs**

**Créer un trigger :**
```sql
-- Table d'audit
CREATE TABLE audit_log (
    log_id SERIAL PRIMARY KEY,
    table_name VARCHAR(50),
    operation VARCHAR(10),
    old_data JSONB,
    new_data JSONB,
    changed_by VARCHAR(100),
    changed_at TIMESTAMP DEFAULT NOW()
);

-- Fonction trigger pour audit
CREATE OR REPLACE FUNCTION audit_trigger_function()
RETURNS TRIGGER AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        INSERT INTO audit_log (table_name, operation, new_data, changed_by)
        VALUES (TG_TABLE_NAME, TG_OP, row_to_json(NEW), current_user);
        RETURN NEW;
    ELSIF TG_OP = 'UPDATE' THEN
        INSERT INTO audit_log (table_name, operation, old_data, new_data, changed_by)
        VALUES (TG_TABLE_NAME, TG_OP, row_to_json(OLD), row_to_json(NEW), current_user);
        RETURN NEW;
    ELSIF TG_OP = 'DELETE' THEN
        INSERT INTO audit_log (table_name, operation, old_data, changed_by)
        VALUES (TG_TABLE_NAME, TG_OP, row_to_json(OLD), current_user);
        RETURN OLD;
    END IF;
END;
$$ LANGUAGE plpgsql;

-- Créer le trigger
CREATE TRIGGER customers_audit_trigger
AFTER INSERT OR UPDATE OR DELETE ON customers
FOR EACH ROW
EXECUTE FUNCTION audit_trigger_function();
```

**Trigger BEFORE :**
```sql
-- Auto-update du timestamp
CREATE OR REPLACE FUNCTION update_modified_timestamp()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = NOW();
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER update_customers_timestamp
BEFORE UPDATE ON customers
FOR EACH ROW
EXECUTE FUNCTION update_modified_timestamp();

-- Validation avant insertion
CREATE OR REPLACE FUNCTION validate_product_price()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.price <= 0 THEN
        RAISE EXCEPTION 'Price must be positive';
    END IF;
    
    IF NEW.discount > 50 THEN
        RAISE EXCEPTION 'Discount cannot exceed 50%%';
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER validate_product_before_insert
BEFORE INSERT OR UPDATE ON products
FOR EACH ROW
EXECUTE FUNCTION validate_product_price();
```

**Trigger pour maintenir l'intégrité :**
```sql
-- Mettre à jour le stock automatiquement
CREATE OR REPLACE FUNCTION update_stock_on_order()
RETURNS TRIGGER AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        UPDATE products
        SET stock_quantity = stock_quantity - NEW.quantity
        WHERE product_id = NEW.product_id;
    ELSIF TG_OP = 'DELETE' THEN
        UPDATE products
        SET stock_quantity = stock_quantity + OLD.quantity
        WHERE product_id = OLD.product_id;
    ELSIF TG_OP = 'UPDATE' THEN
        UPDATE products
        SET stock_quantity = stock_quantity + OLD.quantity - NEW.quantity
        WHERE product_id = NEW.product_id;
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER maintain_stock_trigger
AFTER INSERT OR UPDATE OR DELETE ON order_items
FOR EACH ROW
EXECUTE FUNCTION update_stock_on_order();
```

**Gérer les triggers :**
```sql
-- Lister les triggers
SELECT 
    trigger_name,
    event_manipulation,
    event_object_table,
    action_timing
FROM information_schema.triggers
WHERE trigger_schema = 'public';

-- Désactiver un trigger
ALTER TABLE customers DISABLE TRIGGER customers_audit_trigger;

-- Réactiver un trigger
ALTER TABLE customers ENABLE TRIGGER customers_audit_trigger;

-- Supprimer un trigger
DROP TRIGGER IF EXISTS customers_audit_trigger ON customers;
```

---

### **Séquences (Sequences)**

**Créer et utiliser des séquences :**
```sql
-- Créer une séquence
CREATE SEQUENCE order_number_seq
    START WITH 1000
    INCREMENT BY 1
    MINVALUE 1000
    MAXVALUE 999999
    CACHE 20;

-- Obtenir la prochaine valeur
SELECT nextval('order_number_seq');

-- Voir la valeur actuelle (sans incrémenter)
SELECT currval('order_number_seq');

-- Définir une valeur spécifique
SELECT setval('order_number_seq', 5000);

-- Utiliser dans une table
CREATE TABLE orders (
    order_id INTEGER DEFAULT nextval('order_number_seq') PRIMARY KEY,
    customer_id INTEGER,
    order_date DATE
);

-- Ou attacher à une colonne existante
ALTER TABLE orders 
ALTER COLUMN order_id SET DEFAULT nextval('order_number_seq');

-- Lier la séquence à la colonne (pour DROP CASCADE)
ALTER SEQUENCE order_number_seq OWNED BY orders.order_id;
```

**Gérer les séquences :**
```sql
-- Lister les séquences
SELECT sequence_name, last_value, increment_by
FROM information_schema.sequences;

-- Modifier une séquence
ALTER SEQUENCE order_number_seq RESTART WITH 10000;
ALTER SEQUENCE order_number_seq INCREMENT BY 5;

-- Supprimer une séquence
DROP SEQUENCE IF EXISTS order_number_seq;

-- Réinitialiser une séquence au maximum de la table
SELECT setval('order_number_seq', 
    (SELECT MAX(order_id) FROM orders)
);
```

---

### **Expressions Conditionnelles**

**CASE - Expression conditionnelle :**
```sql
-- CASE simple
SELECT 
    product_name,
    price,
    CASE 
        WHEN price < 50 THEN 'Cheap'
        WHEN price BETWEEN 50 AND 200 THEN 'Medium'
        WHEN price > 200 THEN 'Expensive'
        ELSE 'Unknown'
    END AS price_category
FROM products;

-- CASE dans ORDER BY
SELECT * FROM products
ORDER BY 
    CASE 
        WHEN category = 'Electronics' THEN 1
        WHEN category = 'Books' THEN 2
        ELSE 3
    END,
    product_name;

-- CASE dans GROUP BY
SELECT 
    CASE 
        WHEN price < 100 THEN 'Budget'
        WHEN price < 500 THEN 'Mid-range'
        ELSE 'Premium'
    END AS price_range,
    COUNT(*) AS product_count,
    AVG(price) AS avg_price
FROM products
GROUP BY 
    CASE 
        WHEN price < 100 THEN 'Budget'
        WHEN price < 500 THEN 'Mid-range'
        ELSE 'Premium'
    END;

-- CASE dans UPDATE
UPDATE products
SET discount = 
    CASE 
        WHEN stock_quantity > 100 THEN 15
        WHEN stock_quantity > 50 THEN 10
        WHEN stock_quantity > 20 THEN 5
        ELSE 0
    END;
```

**COALESCE - Première valeur non-NULL :**
```sql
-- Remplacer NULL par une valeur par défaut
SELECT 
    customer_id,
    first_name,
    COALESCE(phone, 'No phone') AS phone,
    COALESCE(email, 'No email') AS email
FROM customers;

-- Première valeur non-NULL parmi plusieurs colonnes
SELECT 
    product_id,
    COALESCE(special_price, discounted_price, regular_price) AS final_price
FROM products;
```

**NULLIF - Retourner NULL si égal :**
```sql
-- Éviter la division par zéro
SELECT 
    order_id,
    total_amount,
    item_count,
    total_amount / NULLIF(item_count, 0) AS avg_item_price
FROM orders;

-- Comparer deux valeurs
SELECT 
    NULLIF(old_price, new_price) AS price_changed
FROM price_history;
```

---

### **Fonctions Utiles**

**Fonctions de chaînes :**
```sql
-- Concaténation
SELECT first_name || ' ' || last_name AS full_name FROM customers;
SELECT CONCAT(first_name, ' ', last_name) AS full_name FROM customers;
SELECT CONCAT_WS(', ', last_name, first_name) AS name FROM customers;

-- Casse
SELECT 
    UPPER(first_name) AS uppercase,
    LOWER(last_name) AS lowercase,
    INITCAP(email) AS capitalized
FROM customers;

-- Longueur
SELECT LENGTH(product_name) FROM products;
SELECT CHAR_LENGTH(description) FROM products;

-- Sous-chaîne
SELECT SUBSTRING(product_name FROM 1 FOR 10) AS short_name FROM products;
SELECT LEFT(product_name, 5) AS prefix FROM products;
SELECT RIGHT(product_name, 5) AS suffix FROM products;

-- Position
SELECT POSITION('Samsung' IN product_name) AS position FROM products;
SELECT STRPOS(product_name, 'Pro') AS position FROM products;

-- Remplacement
SELECT REPLACE(product_name, 'Phone', 'Smartphone') FROM products;

-- Trim (supprimer espaces)
SELECT 
    TRIM(product_name) AS trimmed,
    LTRIM(product_name) AS left_trimmed,
    RTRIM(product_name) AS right_trimmed
FROM products;

-- Répéter
SELECT REPEAT('*', 5);  -- *****

-- Padding
SELECT LPAD(CAST(product_id AS TEXT), 6, '0') FROM products;  -- 000123
SELECT RPAD(product_name, 20, '.') FROM products;
```

**Fonctions numériques :**
```sql
-- Arrondis
SELECT 
    ROUND(price, 2) AS rounded,
    CEIL(price) AS ceiling,
    FLOOR(price) AS floor,
    TRUNC(price, 1) AS truncated
FROM products;

-- Valeur absolue
SELECT ABS(-15.5);  -- 15.5

-- Puissance et racine
SELECT POWER(2, 10);  -- 1024
SELECT SQRT(144);  -- 12

-- Modulo
SELECT MOD(17, 5);  -- 2
SELECT 17 % 5;  -- 2

-- Aléatoire
SELECT RANDOM();  -- Entre 0 et 1
SELECT FLOOR(RANDOM() * 100)::INTEGER;  -- Entre 0 et 99
```

**Fonctions de date :**
```sql
-- Date/heure actuelle
SELECT 
    CURRENT_DATE,
    CURRENT_TIME,
    CURRENT_TIMESTAMP,
    NOW(),
    TIMEOFDAY();

-- Extraire des parties
SELECT 
    EXTRACT(YEAR FROM order_date) AS year,
    EXTRACT(MONTH FROM order_date) AS month,
    EXTRACT(DAY FROM order_date) AS day,
    EXTRACT(DOW FROM order_date) AS day_of_week,  -- 0=Sunday
    EXTRACT(DOY FROM order_date) AS day_of_year
FROM orders;

-- DATE_PART (alias de EXTRACT)
SELECT DATE_PART('year', order_date) FROM orders;

-- Tronquer à une unité
SELECT 
    DATE_TRUNC('year', order_date) AS year_start,
    DATE_TRUNC('month', order_date) AS month_start,
    DATE_TRUNC('day', order_date) AS day_start
FROM orders;

-- Différence entre dates
SELECT 
    order_date,
    CURRENT_DATE - order_date AS days_since_order,
    AGE(CURRENT_DATE, order_date) AS age_interval
FROM orders;

-- Formatage
SELECT TO_CHAR(order_date, 'DD/MM/YYYY') FROM orders;
SELECT TO_CHAR(order_date, 'Day, DD Month YYYY') FROM orders;
SELECT TO_CHAR(created_at, 'HH24:MI:SS') FROM orders;

-- Conversion
SELECT TO_DATE('2024-01-15', 'YYYY-MM-DD');
SELECT TO_TIMESTAMP('2024-01-15 10:30:00', 'YYYY-MM-DD HH24:MI:SS');
```

**Fonctions d'agrégation (rappel) :**
```sql
SELECT 
    COUNT(DISTINCT customer_id) AS unique_customers,
    SUM(total_amount) AS total_revenue,
    AVG(total_amount) AS average_order,
    MIN(total_amount) AS smallest_order,
    MAX(total_amount) AS largest_order,
    STDDEV(total_amount) AS standard_deviation,
    VARIANCE(total_amount) AS variance
FROM orders;

-- STRING_AGG: Concaténer des valeurs
SELECT 
    customer_id,
    STRING_AGG(product_name, ', ' ORDER BY product_name) AS products_bought
FROM order_items oi
JOIN products p ON oi.product_id = p.product_id
GROUP BY customer_id;

-- ARRAY_AGG: Créer un array
SELECT 
    category,
    ARRAY_AGG(product_name ORDER BY price DESC) AS products
FROM products
GROUP BY category;
```

**Fonctions JSON/JSONB :**
```sql
-- Extraire des valeurs JSON
SELECT 
    event_data->>'type' AS event_type,
    event_data->>'value' AS event_value,
    event_data->'metadata'->>'user_id' AS user_id
FROM events;

-- Vérifier l'existence d'une clé
SELECT * FROM events
WHERE event_data ? 'type';

-- Contient (opérateur @>)
SELECT * FROM events
WHERE event_data @> '{"type": "click"}';

-- JSON path
SELECT 
    jsonb_path_query(event_data, '$.metadata.user_id') AS user_id
FROM events;

-- Construire du JSON
SELECT 
    json_build_object(
        'customer_id', customer_id,
        'name', first_name || ' ' || last_name,
        'email', email
    ) AS customer_json
FROM customers;

-- JSON aggregate
SELECT 
    customer_id,
    json_agg(
        json_build_object(
            'order_id', order_id,
            'date', order_date,
            'amount', total_amount
        )
    ) AS orders
FROM orders
GROUP BY customer_id;
```

---

### **Opérations sur les Arrays**

**Manipulation d'arrays :**
```sql
-- Créer un array
SELECT ARRAY[1, 2, 3, 4, 5];
SELECT ARRAY['apple', 'banana', 'orange'];

-- Accéder aux éléments (indexé à partir de 1)
SELECT (ARRAY[10, 20, 30, 40])[2];  -- 20
SELECT tags[1] FROM articles;

-- Longueur
SELECT array_length(tags, 1) FROM articles;

-- Concaténation
SELECT ARRAY[1, 2] || ARRAY[3, 4];  -- {1,2,3,4}
SELECT ARRAY[1, 2] || 3;  -- {1,2,3}

-- Opérateurs de recherche
SELECT * FROM articles WHERE 'postgresql' = ANY(tags);
SELECT * FROM articles WHERE tags @> ARRAY['database', 'sql'];
SELECT * FROM articles WHERE tags && ARRAY['tech', 'programming'];

-- Transformer array en lignes
SELECT unnest(ARRAY[1, 2, 3, 4, 5]);

-- Transformer lignes en array
SELECT array_agg(product_id) 
FROM products 
WHERE category = 'Electronics';
```

---

### **Gestion des Valeurs NULL**

**Comprendre NULL :**
```sql
-- NULL n'est pas égal à NULL
SELECT NULL = NULL;  -- Retourne NULL (pas TRUE ni FALSE)

-- Comparaisons avec NULL
SELECT * FROM customers WHERE phone = NULL;     -- Ne retourne rien
SELECT * FROM customers WHERE phone IS NULL;    -- Correct

-- NULL dans les calculs
SELECT 10 + NULL;  -- Retourne NULL
SELECT 10 * NULL;  -- Retourne NULL

-- NULL dans les fonctions d'agrégation
SELECT 
    COUNT(*) AS total_rows,
    COUNT(phone) AS rows_with_phone,  -- Ignore les NULL
    SUM(quantity) AS total_quantity   -- Ignore les NULL
FROM customers;
```

**Gérer les NULL :**
```sql
-- COALESCE: Première valeur non-NULL
SELECT 
    product_name,
    COALESCE(discount_price, regular_price, 0) AS price
FROM products;

-- NULLIF: Retourne NULL si égal
SELECT 
    total_amount / NULLIF(item_count, 0) AS avg_price
FROM orders;

-- IS DISTINCT FROM: Compare même les NULL
SELECT * FROM products 
WHERE discount_price IS DISTINCT FROM previous_discount;

-- NULL dans ORDER BY
SELECT * FROM customers 
ORDER BY phone NULLS LAST;  -- NULL à la fin

SELECT * FROM customers 
ORDER BY phone NULLS FIRST;  -- NULL au début
```

---

### **Opérations Ensemblistes**

**UNION - Union de résultats :**
```sql
-- UNION: Combine et élimine les doublons
SELECT first_name, last_name FROM customers
UNION
SELECT first_name, last_name FROM suppliers;

-- UNION ALL: Combine sans éliminer les doublons (plus rapide)
SELECT customer_id FROM orders WHERE order_date = '2024-01-15'
UNION ALL
SELECT customer_id FROM orders WHERE order_date = '2024-01-16';
```

**INTERSECT - Intersection :**
```sql
-- Clients qui sont aussi fournisseurs
SELECT email FROM customers
INTERSECT
SELECT email FROM suppliers;

-- Produits en stock dans deux entrepôts
SELECT product_id FROM warehouse_a
INTERSECT
SELECT product_id FROM warehouse_b;
```

**EXCEPT - Différence :**
```sql
-- Clients qui n'ont jamais commandé
SELECT customer_id FROM customers
EXCEPT
SELECT DISTINCT customer_id FROM orders;

-- Produits en stock dans A mais pas dans B
SELECT product_id FROM warehouse_a
EXCEPT
SELECT product_id FROM warehouse_b;
```

---

### **Requêtes Avancées et Patterns Utiles**

**Trouver les duplicatas :**
```sql
-- Trouver les emails en double
SELECT 
    email,
    COUNT(*) AS count
FROM customers
GROUP BY email
HAVING COUNT(*) > 1;

-- Trouver toutes les lignes dupliquées avec détails
SELECT *
FROM customers c1
WHERE EXISTS (
    SELECT 1 FROM customers c2
    WHERE c1.email = c2.email 
    AND c1.customer_id > c2.customer_id
);

-- Supprimer les duplicatas (garder le plus récent)
DELETE FROM customers c1
USING customers c2
WHERE c1.customer_id < c2.customer_id
AND c1.email = c2.email;
```

**Pagination efficace :**
```sql
-- Méthode classique (offset peut être lent)
SELECT * FROM products
ORDER BY product_id
LIMIT 20 OFFSET 40;  -- Page 3

-- Keyset pagination (plus performant)
SELECT * FROM products
WHERE product_id > 40
ORDER BY product_id
LIMIT 20;

-- Pagination avec curseur
SELECT * FROM products
WHERE (created_at, product_id) > ('2024-01-15', 100)
ORDER BY created_at, product_id
LIMIT 20;
```

**Générer des séries de nombres/dates :**
```sql
-- Série de nombres
SELECT * FROM generate_series(1, 10);
SELECT * FROM generate_series(1, 100, 10);  -- 1, 11, 21, ...

-- Série de dates
SELECT * FROM generate_series(
    '2024-01-01'::DATE,
    '2024-12-31'::DATE,
    '1 day'::INTERVAL
);

-- Exemple: Rapport avec tous les jours du mois
WITH date_series AS (
    SELECT generate_series(
        DATE_TRUNC('month', CURRENT_DATE),
        DATE_TRUNC('month', CURRENT_DATE) + INTERVAL '1 month' - INTERVAL '1 day',
        '1 day'::INTERVAL
    )::DATE AS date
)
SELECT 
    ds.date,
    COALESCE(COUNT(o.order_id), 0) AS order_count,
    COALESCE(SUM(o.total_amount), 0) AS daily_revenue
FROM date_series ds
LEFT JOIN orders o ON o.order_date = ds.date
GROUP BY ds.date
ORDER BY ds.date;
```

**Pivot (transformer lignes en colonnes) :**
```sql
-- Données de base
SELECT category, product_name, price FROM products;

-- Pivot avec CASE
SELECT 
    product_name,
    MAX(CASE WHEN category = 'Electronics' THEN price END) AS electronics_price,
    MAX(CASE WHEN category = 'Books' THEN price END) AS books_price,
    MAX(CASE WHEN category = 'Clothing' THEN price END) AS clothing_price
FROM products
GROUP BY product_name;

-- Crosstab (nécessite extension tablefunc)
CREATE EXTENSION IF NOT EXISTS tablefunc;

SELECT * FROM crosstab(
    'SELECT product_name, category, price FROM products ORDER BY 1, 2',
    'SELECT DISTINCT category FROM products ORDER BY 1'
) AS ct(product_name TEXT, electronics NUMERIC, books NUMERIC, clothing NUMERIC);
```

**Numérotation de lignes avec gaps :**
```sql
-- Renuméroter les IDs sans gaps
WITH numbered AS (
    SELECT 
        customer_id,
        ROW_NUMBER() OVER (ORDER BY customer_id) AS new_id
    FROM customers
)
UPDATE customers c
SET customer_id = n.new_id
FROM numbered n
WHERE c.customer_id = n.old_customer_id;
```

**Calcul du percentile :**
```sql
-- Percentiles des prix
SELECT 
    PERCENTILE_CONT(0.25) WITHIN GROUP (ORDER BY price) AS q1,
    PERCENTILE_CONT(0.50) WITHIN GROUP (ORDER BY price) AS median,
    PERCENTILE_CONT(0.75) WITHIN GROUP (ORDER BY price) AS q3,
    PERCENTILE_CONT(0.90) WITHIN GROUP (ORDER BY price) AS p90
FROM products;
```

**Running total (total cumulatif) :**
```sql
-- Total cumulatif des ventes par jour
SELECT 
    order_date,
    SUM(total_amount) AS daily_revenue,
    SUM(SUM(total_amount)) OVER (ORDER BY order_date) AS cumulative_revenue
FROM orders
GROUP BY order_date
ORDER BY order_date;
```

**Trouver les N premiers/derniers de chaque groupe :**
```sql
-- Top 3 produits les plus chers par catégorie
WITH ranked AS (
    SELECT 
        product_name,
        category,
        price,
        ROW_NUMBER() OVER (PARTITION BY category ORDER BY price DESC) AS rank
    FROM products
)
SELECT product_name, category, price
FROM ranked
WHERE rank <= 3;
```

**Détection de valeurs aberrantes (outliers) :**
```sql
-- Méthode IQR (Interquartile Range)
WITH stats AS (
    SELECT 
        PERCENTILE_CONT(0.25) WITHIN GROUP (ORDER BY price) AS q1,
        PERCENTILE_CONT(0.75) WITHIN GROUP (ORDER BY price) AS q3
    FROM products
),
bounds AS (
    SELECT 
        q1 - 1.5 * (q3 - q1) AS lower_bound,
        q3 + 1.5 * (q3 - q1) AS upper_bound
    FROM stats
)
SELECT *
FROM products, bounds
WHERE price < lower_bound OR price > upper_bound;
```

---

### **Performance et Bonnes Pratiques**

**Optimisation des requêtes :**
```sql
-- ❌ Mauvais: SELECT *
SELECT * FROM customers;

-- ✅ Bon: Sélectionner uniquement les colonnes nécessaires
SELECT customer_id, first_name, last_name FROM customers;

-- ❌ Mauvais: Fonction dans WHERE (empêche l'utilisation d'index)
SELECT * FROM customers WHERE UPPER(email) = 'JOHN@EXAMPLE.COM';

-- ✅ Bon: Index fonctionnel ou recherche directe
CREATE INDEX idx_email_lower ON customers(LOWER(email));
SELECT * FROM customers WHERE LOWER(email) = 'john@example.com';

-- ❌ Mauvais: OR dans WHERE (peut être lent)
SELECT * FROM products WHERE category = 'Electronics' OR category = 'Books';

-- ✅ Bon: Utiliser IN
SELECT * FROM products WHERE category IN ('Electronics', 'Books');

-- ❌ Mauvais: NOT IN avec NULL possibles
SELECT * FROM products WHERE product_id NOT IN (SELECT product_id FROM discontinued);

-- ✅ Bon: NOT EXISTS
SELECT * FROM products p
WHERE NOT EXISTS (
    SELECT 1 FROM discontinued d WHERE d.product_id = p.product_id
);

-- ❌ Mauvais: DISTINCT sans raison
SELECT DISTINCT * FROM customers;

-- ✅ Bon: GROUP BY si nécessaire
SELECT customer_id FROM orders GROUP BY customer_id;
```

**Batch operations :**
```sql
-- ❌ Mauvais: Insertions individuelles
INSERT INTO orders (customer_id, total_amount) VALUES (1, 100);
INSERT INTO orders (customer_id, total_amount) VALUES (2, 200);
INSERT INTO orders (customer_id, total_amount) VALUES (3, 150);

-- ✅ Bon: Insertion batch
INSERT INTO orders (customer_id, total_amount) VALUES
(1, 100),
(2, 200),
(3, 150);

-- ❌ Mauvais: Updates individuels
UPDATE products SET price = price * 1.1 WHERE product_id = 1;
UPDATE products SET price = price * 1.1 WHERE product_id = 2;

-- ✅ Bon: Update en masse
UPDATE products 
SET price = price * 1.1 
WHERE product_id IN (1, 2, 3, 4, 5);
```

**Utilisation efficace des index :**
```sql
-- Index sur colonnes de recherche fréquente
CREATE INDEX idx_orders_customer ON orders(customer_id);
CREATE INDEX idx_orders_date ON orders(order_date);

-- Index composite (ordre important)
CREATE INDEX idx_orders_customer_date ON orders(customer_id, order_date);
-- Bon pour: WHERE customer_id = X AND order_date = Y
-- Bon pour: WHERE customer_id = X
-- Mauvais pour: WHERE order_date = Y (seul)

-- Index partiel (seulement sur subset des données)
CREATE INDEX idx_active_customers ON customers(customer_id) 
WHERE is_active = TRUE;

-- Index sur expression
CREATE INDEX idx_email_lower ON customers(LOWER(email));
```

**Éviter les N+1 queries :**
```sql
-- ❌ Mauvais: N+1 queries en code applicatif
-- SELECT * FROM customers;
-- Puis pour chaque customer:
-- SELECT * FROM orders WHERE customer_id = ?

-- ✅ Bon: Une seule requête avec JOIN
SELECT 
    c.customer_id,
    c.first_name,
    c.last_name,
    o.order_id,
    o.total_amount
FROM customers c
LEFT JOIN orders o ON c.customer_id = o.customer_id;

-- Ou avec sous-requête agrégée
SELECT 
    c.customer_id,
    c.first_name,
    c.last_name,
    (SELECT COUNT(*) FROM orders WHERE customer_id = c.customer_id) AS order_count
FROM customers c;
```

---

### **Sécurité et Injection SQL**

**Prévention des injections SQL :**
```sql
-- ❌ DANGEREUX: Concaténation de strings (en code applicatif)
-- query = "SELECT * FROM users WHERE email = '" + userInput + "'";

-- ✅ BON: Paramètres préparés (prepared statements)
-- En Java/JDBC:
-- PreparedStatement ps = conn.prepareStatement("SELECT * FROM users WHERE email = ?");
-- ps.setString(1, userInput);

-- En Python:
-- cursor.execute("SELECT * FROM users WHERE email = %s", (user_input,))

-- Exemples d'attaques par injection:
-- Input: ' OR '1'='1
-- Résultat: SELECT * FROM users WHERE email = '' OR '1'='1'
-- Retourne tous les utilisateurs!

-- Input: '; DROP TABLE users; --
-- Résultat: SELECT * FROM users WHERE email = ''; DROP TABLE users; --'
-- Supprime la table!
```

**Principe de moindre privilège :**
```sql
-- Créer un utilisateur avec privilèges limités
CREATE USER app_readonly WITH PASSWORD 'password';

-- Accorder uniquement les privilèges nécessaires
GRANT CONNECT ON DATABASE mydb TO app_readonly;
GRANT USAGE ON SCHEMA public TO app_readonly;
GRANT SELECT ON ALL TABLES IN SCHEMA public TO app_readonly;

-- Pour un utilisateur applicatif (lecture/écriture)
CREATE USER app_user WITH PASSWORD 'secure_password';
GRANT SELECT, INSERT, UPDATE, DELETE ON ALL TABLES IN SCHEMA public TO app_user;
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO app_user;

-- Révoquer des privilèges
REVOKE DELETE ON customers FROM app_user;
```

---

### **Maintenance et Administration**

**Statistiques et informations système :**
```sql
-- Taille des bases de données
SELECT 
    datname,
    pg_size_pretty(pg_database_size(datname)) AS size
FROM pg_database
ORDER BY pg_database_size(datname) DESC;

-- Taille des tables
SELECT 
    schemaname,
    tablename,
    pg_size_pretty(pg_total_relation_size(schemaname||'.'||tablename)) AS size
FROM pg_tables
WHERE schemaname = 'public'
ORDER BY pg_total_relation_size(schemaname||'.'||tablename) DESC;

-- Nombre de lignes par table (estimation rapide)
SELECT 
    schemaname,
    tablename,
    n_live_tup AS estimated_row_count
FROM pg_stat_user_tables
ORDER BY n_live_tup DESC;

-- Tables les plus consultées
SELECT 
    schemaname,
    tablename,
    seq_scan,
    seq_tup_read,
    idx_scan,
    idx_tup_fetch
FROM pg_stat_user_tables
ORDER BY seq_scan + idx_scan DESC;

-- Index inutilisés
SELECT 
    schemaname,
    tablename,
    indexname,
    idx_scan
FROM pg_stat_user_indexes
WHERE idx_scan = 0
AND indexname NOT LIKE '%_pkey';
```

**VACUUM et ANALYZE :**
```sql
-- VACUUM: Libère l'espace des lignes supprimées
VACUUM customers;

-- VACUUM FULL: Plus agressif, reconstruit la table
VACUUM FULL customers;

-- ANALYZE: Met à jour les statistiques pour l'optimiseur
ANALYZE customers;

-- VACUUM ANALYZE: Les deux ensemble
VACUUM ANALYZE customers;

-- Sur toute la base
VACUUM ANALYZE;

-- Autovacuum (activé par défaut)
-- Configuration dans postgresql.conf:
-- autovacuum = on
-- autovacuum_naptime = 1min
```

**Vérifier les connexions actives :**
```sql
-- Connexions actives
SELECT 
    pid,
    usename,
    application_name,
    client_addr,
    backend_start,
    state,
    query
FROM pg_stat_activity
WHERE state = 'active';

-- Nombre de connexions par base
SELECT 
    datname,
    COUNT(*) AS connections
FROM pg_stat_activity
GROUP BY datname;

-- Tuer une connexion
SELECT pg_terminate_backend(pid)
FROM pg_stat_activity
WHERE pid = 12345;
```

---

Cette documentation complète couvre tous les aspects fondamentaux de PostgreSQL : types de données, création et modification de tables, contraintes, requêtes SQL (simples et avancées), jointures, sous-requêtes, fonctions, index, transactions, vues, procédures stockées, triggers, et bonnes pratiques de performance. C'est une base solide pour travailler efficacement avec PostgreSQL dans n'importe quel contexte.