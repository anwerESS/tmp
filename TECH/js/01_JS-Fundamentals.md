## [**..**](./00_index.md)

# Chapitre 1 : Fondamentaux du Langage

## 1.1 Introduction et Setup

JavaScript est un langage de programmation **interprété** et **dynamiquement typé**. Il s'exécute principalement dans le navigateur (client-side) mais aussi côté serveur avec Node.js.

**ECMAScript (ES)** est la spécification standard de JavaScript :
- **ES5** (2009) : version stable, compatible partout
- **ES6/ES2015** : révolution majeure (arrow functions, classes, modules, etc.)
- **ES2016+** : nouvelles fonctionnalités chaque année

```javascript
// Premier script JavaScript
console.log("Hello World!"); // Affiche dans la console du navigateur

// La console du navigateur permet de tester du code JS
// F12 (Windows) ou Cmd+Option+I (Mac) pour ouvrir les DevTools
```

---

## 1.2 Variables et Déclarations

### `var` (ancien, à éviter)

```javascript
// var : function-scoped et hoisted
var name = "John";
var name = "Jane"; // ✅ Redéclaration possible (problématique!)

if (true) {
    var age = 25;
}
console.log(age); // ✅ 25 - var n'a pas de block scope

// Hoisting : les déclarations var sont remontées
console.log(city); // ✅ undefined (pas d'erreur!)
var city = "Paris";
```

### `let` (moderne, recommandé)

```javascript
// let : block-scoped, pas de redéclaration
let username = "Alice";
// let username = "Bob"; // ❌ SyntaxError: Identifier 'username' has already been declared

if (true) {
    let score = 100;
    console.log(score); // ✅ 100
}
// console.log(score); // ❌ ReferenceError: score is not defined

// Hoisting différent avec let
// console.log(country); // ❌ ReferenceError: Cannot access before initialization
let country = "France";
```

### `const` (moderne, préféré quand possible)

```javascript
// const : block-scoped, doit être initialisé, pas de réaffectation
const PI = 3.14159;
// PI = 3.14; // ❌ TypeError: Assignment to constant variable

// IMPORTANT : const empêche la réaffectation, pas la mutation
const person = { name: "Bob", age: 30 };
person.age = 31; // ✅ Mutation d'objet OK
person.city = "London"; // ✅ Ajout de propriété OK
// person = {}; // ❌ Réaffectation interdite

const colors = ["red", "blue"];
colors.push("green"); // ✅ Modification du contenu OK
// colors = []; // ❌ Réaffectation interdite

// Toujours initialiser const
// const MAX_SIZE; // ❌ SyntaxError: Missing initializer
```

### Bonnes Pratiques

```javascript
// ✅ Préférer const par défaut
const API_URL = "https://api.example.com";
const MAX_RETRIES = 3;

// ✅ Utiliser let quand la valeur change
let counter = 0;
counter++;

// ❌ Éviter var (sauf code legacy)
// var oldVariable = "avoid";

// ✅ Déclarer les variables en haut du scope
const basePrice = 100;
let discount = 0;
let finalPrice;

// ... logique ...
finalPrice = basePrice - discount;
```

---

## 1.3 Types de Données

JavaScript a **7 types primitifs** et **1 type complexe**.

### Types Primitifs

```javascript
// 1. String (chaîne de caractères)
const firstName = "John";
const lastName = 'Doe';
const greeting = `Hello ${firstName}`; // Template literal (ES6)

// 2. Number (nombres entiers et décimaux)
const integer = 42;
const float = 3.14;
const negative = -10;
const infinity = Infinity;
const notANumber = NaN; // Not a Number

// 3. Boolean (true/false)
const isActive = true;
const isCompleted = false;

// 4. Undefined (variable déclarée mais non initialisée)
let undefinedVar;
console.log(undefinedVar); // undefined

// 5. Null (absence intentionnelle de valeur)
let emptyValue = null;

// 6. Symbol (identifiant unique, ES6)
const uniqueId = Symbol("id");
const anotherId = Symbol("id");
console.log(uniqueId === anotherId); // false (toujours unique)

// 7. BigInt (grands entiers, ES2020)
const bigNumber = 9007199254740991n; // Note le 'n' à la fin
const anotherBig = BigInt(9007199254740991);
```

### Type Complexe

```javascript
// Object (objets, arrays, functions, etc.)
const user = {
    name: "Alice",
    age: 25
};

const numbers = [1, 2, 3, 4, 5];

const myFunction = function() {
    return "Hello";
};
```

### typeof Operator

```javascript
// typeof retourne le type de la valeur
console.log(typeof "Hello");        // "string"
console.log(typeof 42);             // "number"
console.log(typeof 3.14);           // "number"
console.log(typeof true);           // "boolean"
console.log(typeof undefined);      // "undefined"
console.log(typeof null);           // "object" ⚠️ Bug historique!
console.log(typeof Symbol());       // "symbol"
console.log(typeof 10n);            // "bigint"
console.log(typeof {});             // "object"
console.log(typeof []);             // "object" ⚠️
console.log(typeof function(){});   // "function"

// Pour vérifier si c'est un array
console.log(Array.isArray([]));     // true
console.log(Array.isArray({}));     // false

// Pour vérifier null correctement
const value = null;
console.log(value === null);        // true
```

### Type Coercion (Conversion Implicite)

```javascript
// JavaScript convertit automatiquement les types si nécessaire

// String coercion
console.log("5" + 3);           // "53" (number → string)
console.log("Hello" + true);    // "Hellotrue"

// Number coercion
console.log("5" - 3);           // 2 (string → number)
console.log("10" * "2");        // 20
console.log("10" / "2");        // 5
console.log("5" - true);        // 4 (true → 1)
console.log("5" - false);       // 5 (false → 0)

// Boolean coercion (dans les conditions)
if ("hello") {                  // string non-vide → true
    console.log("Truthy!");
}

if (0) {                        // 0 → false
    // Ce code ne s'exécute pas
}

// Comparaison avec coercion (==)
console.log(5 == "5");          // true ⚠️ (coercion!)
console.log(true == 1);         // true
console.log(false == 0);        // true
console.log(null == undefined); // true ⚠️

// Comparaison stricte sans coercion (===) - RECOMMANDÉ
console.log(5 === "5");         // false ✅
console.log(true === 1);        // false
console.log(null === undefined);// false
```

### Conversion Explicite

```javascript
// String conversion
const num = 42;
const str1 = String(num);          // "42"
const str2 = num.toString();       // "42"
const str3 = "" + num;             // "42" (coercion)

// Number conversion
const text = "123";
const num1 = Number(text);         // 123
const num2 = parseInt(text);       // 123 (entier uniquement)
const num3 = parseFloat("3.14");   // 3.14
const num4 = +text;                // 123 (unary plus)

console.log(Number("abc"));        // NaN (conversion échouée)
console.log(parseInt("123abc"));   // 123 (parse jusqu'au premier non-digit)
console.log(Number("123abc"));     // NaN

// Boolean conversion
const bool1 = Boolean(1);          // true
const bool2 = Boolean(0);          // false
const bool3 = Boolean("hello");    // true
const bool4 = Boolean("");         // false
const bool5 = !!value;             // double négation = conversion

// Valeurs Falsy (deviennent false en boolean)
console.log(Boolean(false));       // false
console.log(Boolean(0));           // false
console.log(Boolean(""));          // false
console.log(Boolean(null));        // false
console.log(Boolean(undefined));   // false
console.log(Boolean(NaN));         // false

// TOUT LE RESTE est Truthy (devient true)
console.log(Boolean("0"));         // true ⚠️
console.log(Boolean("false"));     // true ⚠️
console.log(Boolean([]));          // true
console.log(Boolean({}));          // true
```

---

## 1.4 Opérateurs

### Opérateurs Arithmétiques

```javascript
// Opérateurs de base
const a = 10;
const b = 3;

console.log(a + b);  // 13 (addition)
console.log(a - b);  // 7  (soustraction)
console.log(a * b);  // 30 (multiplication)
console.log(a / b);  // 3.333... (division)
console.log(a % b);  // 1  (modulo - reste de la division)
console.log(a ** b); // 1000 (exponentiation - ES2016)

// Division par zéro
console.log(10 / 0);  // Infinity
console.log(-10 / 0); // -Infinity

// Concaténation de strings avec +
console.log("Hello" + " " + "World"); // "Hello World"
console.log("Number: " + 42);         // "Number: 42"
```

### Opérateurs d'Affectation

```javascript
let x = 10;

// Affectations composées
x += 5;   // x = x + 5  → 15
x -= 3;   // x = x - 3  → 12
x *= 2;   // x = x * 2  → 24
x /= 4;   // x = x / 4  → 6
x %= 4;   // x = x % 4  → 2
x **= 3;  // x = x ** 3 → 8

// Affectation simple
let y = x;  // y vaut 8
```

### Opérateurs de Comparaison

```javascript
// Égalité avec coercion (à éviter)
console.log(5 == "5");     // true ⚠️
console.log(1 == true);    // true ⚠️

// Égalité stricte (recommandé)
console.log(5 === "5");    // false ✅
console.log(5 === 5);      // true

// Inégalité
console.log(5 != "5");     // false (coercion)
console.log(5 !== "5");    // true (strict)

// Comparaisons numériques
console.log(10 > 5);       // true
console.log(10 < 5);       // false
console.log(10 >= 10);     // true
console.log(10 <= 9);      // false

// Comparaison de strings (ordre lexicographique)
console.log("apple" < "banana");   // true
console.log("Z" < "a");            // true (majuscules < minuscules en ASCII)
```

### Opérateurs Logiques

```javascript
// AND (&&) - retourne le premier falsy ou la dernière valeur
console.log(true && true);        // true
console.log(true && false);       // false
console.log("hello" && "world");  // "world"
console.log("hello" && 0);        // 0
console.log(0 && "hello");        // 0

// OR (||) - retourne le premier truthy ou la dernière valeur
console.log(false || true);       // true
console.log(false || false);      // false
console.log(null || "default");   // "default"
console.log("value" || "default");// "value"

// NOT (!) - inverse la valeur boolean
console.log(!true);               // false
console.log(!false);              // true
console.log(!"hello");            // false (car "hello" est truthy)
console.log(!0);                  // true (car 0 est falsy)

// Short-circuit evaluation
let count = 0;
true || count++;  // count++ ne s'exécute pas (true est déjà truthy)
console.log(count); // 0

false && count++; // count++ ne s'exécute pas (false est déjà falsy)
console.log(count); // 0
```

### Opérateurs Unaires

```javascript
// Increment et Decrement
let i = 5;

// Post-increment (retourne puis incrémente)
console.log(i++);  // 5 (retourne 5, puis i devient 6)
console.log(i);    // 6

// Pre-increment (incrémente puis retourne)
console.log(++i);  // 7 (i devient 7, puis retourne 7)
console.log(i);    // 7

// Pareil pour decrement
console.log(i--);  // 7 (retourne 7, puis i devient 6)
console.log(--i);  // 5 (i devient 5, puis retourne 5)

// Unary plus (convertit en number)
console.log(+"42");     // 42
console.log(+true);     // 1
console.log(+"hello");  // NaN

// Unary minus (convertit en number négatif)
console.log(-"42");     // -42
console.log(-true);     // -1

// typeof (déjà vu)
console.log(typeof 42); // "number"

// delete (supprime une propriété d'objet)
const obj = { name: "John", age: 30 };
delete obj.age;
console.log(obj);       // { name: "John" }
```

### Opérateur Ternaire

```javascript
// Syntaxe : condition ? valueIfTrue : valueIfFalse
const age = 18;
const status = age >= 18 ? "adult" : "minor";
console.log(status); // "adult"

// Équivalent avec if/else
let status2;
if (age >= 18) {
    status2 = "adult";
} else {
    status2 = "minor";
}

// Ternaire imbriqué (à utiliser avec modération)
const score = 85;
const grade = score >= 90 ? "A" : 
              score >= 80 ? "B" : 
              score >= 70 ? "C" : "F";
console.log(grade); // "B"

// Utilisation dans des expressions
const price = 100;
const discount = price > 50 ? 10 : 5;
const total = price - discount;
console.log(total); // 90
```

### Nullish Coalescing Operator (??)

```javascript
// ?? retourne le côté droit si le côté gauche est null ou undefined
// (différent de || qui teste les valeurs falsy)

const userInput = null;
const defaultValue = "default";

// Avec ?? (nullish coalescing)
const result1 = userInput ?? defaultValue;
console.log(result1); // "default"

// Comparaison avec ||
const value1 = 0 || "default";        // "default" (0 est falsy)
const value2 = 0 ?? "default";        // 0 (0 n'est ni null ni undefined) ✅

const value3 = "" || "default";       // "default" ("" est falsy)
const value4 = "" ?? "default";       // "" ("" n'est ni null ni undefined) ✅

const value5 = false || "default";    // "default" (false est falsy)
const value6 = false ?? "default";    // false ✅

// Cas pratiques
const config = {
    timeout: 0,        // 0 est une valeur valide
    retries: null      // null = non défini
};

const timeout = config.timeout ?? 3000;  // 0 ✅
const retries = config.retries ?? 3;     // 3 ✅

// Avec || on aurait eu :
const timeoutBad = config.timeout || 3000;  // 3000 ❌ (0 est falsy)
```

### Optional Chaining Operator (?.)

```javascript
// ?. permet d'accéder aux propriétés imbriquées sans erreur si undefined/null

const user = {
    name: "Alice",
    address: {
        city: "Paris",
        zipCode: "75001"
    }
};

// Accès normal
console.log(user.address.city); // "Paris"

// Sans optional chaining
const user2 = { name: "Bob" }; // pas d'address
// console.log(user2.address.city); // ❌ TypeError: Cannot read property 'city' of undefined

// Solution classique
const city1 = user2.address && user2.address.city;
console.log(city1); // undefined

// Avec optional chaining (ES2020) - RECOMMANDÉ
const city2 = user2.address?.city;
console.log(city2); // undefined ✅

// Chaînage multiple
const country = user2.address?.country?.code;
console.log(country); // undefined

// Avec des méthodes
const result = user.getName?.(); // undefined si getName n'existe pas
console.log(result);

// Avec des arrays
const users = [{ name: "Alice" }];
console.log(users[0]?.name);     // "Alice"
console.log(users[1]?.name);     // undefined
console.log(users[1]?.address?.city); // undefined

// Combinaison avec nullish coalescing
const userCity = user2.address?.city ?? "Unknown";
console.log(userCity); // "Unknown"
```

### Exemples Pratiques

```javascript
// Calcul avec opérateurs
const calculateTotal = (price, quantity, taxRate) => {
    const subtotal = price * quantity;
    const tax = subtotal * taxRate;
    const total = subtotal + tax;
    return total;
};

console.log(calculateTotal(10, 5, 0.2)); // 60

// Validation avec opérateurs logiques
const isValidUser = (username, age) => {
    return username && 
           username.length >= 3 && 
           age >= 18 && 
           age <= 120;
};

console.log(isValidUser("Alice", 25));  // true
console.log(isValidUser("Al", 25));     // false

// Valeurs par défaut
function greet(name, greeting) {
    // Ancienne méthode
    name = name || "Guest";
    
    // Nouvelle méthode (plus précise)
    greeting = greeting ?? "Hello";
    
    return `${greeting}, ${name}!`;
}

console.log(greet());              // "Hello, Guest!"
console.log(greet("Alice"));       // "Hello, Alice!"
console.log(greet("Bob", "Hi"));   // "Hi, Bob!"

// Safe property access
const getUserEmail = (user) => {
    return user?.contact?.email ?? "no-email@example.com";
};

console.log(getUserEmail({ contact: { email: "alice@test.com" } })); // "alice@test.com"
console.log(getUserEmail({})); // "no-email@example.com"
console.log(getUserEmail(null)); // "no-email@example.com"
```

---

**Résumé du Chapitre 1 :**

✅ Utiliser `const` par défaut, `let` si nécessaire, éviter `var`  
✅ Connaître les 7 types primitifs + Object  
✅ Utiliser `===` au lieu de `==` pour les comparaisons  
✅ Comprendre truthy/falsy values  
✅ Maîtriser `??` pour les valeurs par défaut et `?.` pour l'accès sécurisé  
✅ Privilégier les conversions explicites plutôt que la coercion

---

# Questions/Réponses Orales - Chapitre 1 : Fondamentaux du Langage

## Questions sur Variables et Déclarations

### Q1 : Quelle est la différence entre `var`, `let` et `const` ?

**Réponse :**

`var` est l'ancienne façon de déclarer des variables, elle a un **function scope** et subit le **hoisting**. On peut la redéclarer, ce qui peut causer des bugs.

`let` introduit en ES6 a un **block scope**, ne peut pas être redéclarée dans le même scope, et évite beaucoup de problèmes de `var`.

`const` fonctionne comme `let` mais empêche la **réaffectation**. Attention, `const` empêche de changer la référence, mais pas de modifier le contenu d'un objet ou d'un array.

```javascript
// var : function scope
function testVar() {
    if (true) {
        var x = 10;
    }
    console.log(x); // ✅ 10 - accessible en dehors du block
}

// let : block scope
function testLet() {
    if (true) {
        let y = 10;
    }
    // console.log(y); // ❌ ReferenceError
}

// const : empêche la réaffectation
const arr = [1, 2, 3];
arr.push(4);        // ✅ OK - modification du contenu
// arr = [5, 6];    // ❌ TypeError - réaffectation interdite
```

**Best practice :** Utiliser `const` par défaut, `let` seulement si la valeur doit changer, éviter `var` complètement.

---

### Q2 : Qu'est-ce que le hoisting ?

**Réponse :**

Le **hoisting** est un mécanisme JavaScript où les déclarations de variables et de fonctions sont "remontées" (hoisted) en haut de leur scope lors de la compilation, mais pas leurs initialisations.

```javascript
// Avec var - hoisting
console.log(username); // undefined (pas d'erreur!)
var username = "Alice";
// Équivalent à :
// var username;
// console.log(username);
// username = "Alice";

// Avec let/const - temporal dead zone
// console.log(age); // ❌ ReferenceError: Cannot access before initialization
let age = 25;

// Les fonctions sont aussi hoisted (déclarations)
sayHello(); // ✅ Fonctionne!
function sayHello() {
    console.log("Hello");
}

// Mais pas les function expressions
// greet(); // ❌ TypeError: greet is not a function
var greet = function() {
    console.log("Hi");
};
```

---

### Q3 : Pouvez-vous modifier un objet déclaré avec `const` ?

**Réponse :**

Oui ! `const` empêche la **réaffectation** de la variable, mais pas la **mutation** de son contenu.

```javascript
const user = {
    name: "John",
    age: 30
};

// ✅ Mutation OK - on modifie le contenu
user.age = 31;
user.city = "Paris";
console.log(user); // { name: "John", age: 31, city: "Paris" }

// ❌ Réaffectation interdite - on change la référence
// user = { name: "Jane" }; // TypeError

// Pareil pour les arrays
const numbers = [1, 2, 3];
numbers.push(4);      // ✅ OK
numbers[0] = 10;      // ✅ OK
// numbers = [5, 6];  // ❌ TypeError

// Pour rendre un objet vraiment immutable
const frozenUser = Object.freeze({ name: "Alice" });
frozenUser.age = 25; // ❌ Silencieusement ignoré (ou erreur en strict mode)
```

---

## Questions sur Types de Données

### Q4 : Quels sont les types de données en JavaScript ?

**Réponse :**

JavaScript a **7 types primitifs** et **1 type complexe** :

**Primitifs :**
1. **String** : `"hello"`, `'world'`, `` `template` ``
2. **Number** : `42`, `3.14`, `Infinity`, `NaN`
3. **Boolean** : `true`, `false`
4. **Undefined** : variable déclarée mais non initialisée
5. **Null** : absence intentionnelle de valeur
6. **Symbol** : identifiant unique (ES6)
7. **BigInt** : grands entiers (ES2020)

**Complexe :**
- **Object** : objets, arrays, functions, dates, etc.

```javascript
// Exemples
const str = "text";              // string
const num = 42;                  // number
const bool = true;               // boolean
let notDefined;                  // undefined
const empty = null;              // null
const id = Symbol("id");         // symbol
const big = 9007199254740991n;   // bigint
const obj = { key: "value" };    // object
const arr = [1, 2, 3];           // object (array)
```

---

### Q5 : Quelle est la différence entre `null` et `undefined` ?

**Réponse :**

`undefined` signifie qu'une variable a été déclarée mais **n'a pas encore de valeur assignée**. C'est la valeur par défaut.

`null` est une **valeur assignée intentionnellement** pour représenter "aucune valeur" ou "vide".

```javascript
// undefined - automatique
let x;
console.log(x);              // undefined
console.log(typeof x);       // "undefined"

function noReturn() {}
console.log(noReturn());     // undefined

const obj = { name: "John" };
console.log(obj.age);        // undefined (propriété n'existe pas)

// null - intentionnel
let y = null;
console.log(y);              // null
console.log(typeof y);       // "object" ⚠️ (bug historique)

// Comparaison
console.log(undefined == null);   // true (coercion)
console.log(undefined === null);  // false (types différents)

// Usage pratique
let user = null;  // "pas encore de user"
// ... après connexion ...
user = { name: "Alice" };
```

---

### Q6 : Qu'est-ce que `NaN` et comment le détecter ?

**Réponse :**

`NaN` signifie **"Not a Number"** et représente une valeur numérique invalide. Paradoxalement, `typeof NaN` retourne `"number"`.

La particularité de `NaN` est qu'il n'est **égal à rien, même pas à lui-même**.

```javascript
// Opérations qui produisent NaN
console.log(0 / 0);              // NaN
console.log(Math.sqrt(-1));      // NaN
console.log(Number("hello"));    // NaN
console.log(parseInt("abc"));    // NaN

// NaN n'est pas égal à lui-même !
console.log(NaN === NaN);        // false ⚠️
console.log(NaN == NaN);         // false

// Pour vérifier NaN, utiliser isNaN() ou Number.isNaN()
console.log(isNaN(NaN));         // true
console.log(isNaN("hello"));     // true ⚠️ (coercion!)
console.log(isNaN(123));         // false

// Number.isNaN() est plus strict (recommandé)
console.log(Number.isNaN(NaN));      // true
console.log(Number.isNaN("hello"));  // false ✅ (pas de coercion)
console.log(Number.isNaN(123));      // false

// Usage pratique
const result = parseInt(userInput);
if (Number.isNaN(result)) {
    console.log("Invalid number");
}
```

---

## Questions sur Type Coercion

### Q7 : Quelle est la différence entre `==` et `===` ?

**Réponse :**

`==` fait une **comparaison avec type coercion** (conversion automatique des types), tandis que `===` fait une **comparaison stricte** sans conversion.

**Il faut toujours utiliser `===`** pour éviter des bugs difficiles à détecter.

```javascript
// == avec coercion (à éviter)
console.log(5 == "5");           // true ⚠️
console.log(1 == true);          // true ⚠️
console.log(0 == false);         // true ⚠️
console.log(null == undefined);  // true ⚠️
console.log("" == false);        // true ⚠️

// === sans coercion (recommandé)
console.log(5 === "5");          // false ✅
console.log(1 === true);         // false ✅
console.log(0 === false);        // false ✅
console.log(null === undefined); // false ✅
console.log("" === false);       // false ✅

// Cas problématique avec ==
const userInput = "0";
if (userInput == false) {  // true! ⚠️
    console.log("This runs unexpectedly");
}

// Solution avec ===
if (userInput === false) {  // false ✅
    // Ne s'exécute pas
}
```

---

### Q8 : Quelles sont les valeurs "falsy" en JavaScript ?

**Réponse :**

Il y a exactement **6 valeurs falsy** en JavaScript. Toutes les autres valeurs sont **truthy**.

```javascript
// Les 6 valeurs falsy
console.log(Boolean(false));      // false
console.log(Boolean(0));          // false
console.log(Boolean(""));         // false (string vide)
console.log(Boolean(null));       // false
console.log(Boolean(undefined));  // false
console.log(Boolean(NaN));        // false

// TOUT LE RESTE est truthy (attention aux pièges!)
console.log(Boolean("0"));        // true ⚠️ (string non vide)
console.log(Boolean("false"));    // true ⚠️ (string non vide)
console.log(Boolean([]));         // true ⚠️ (array vide)
console.log(Boolean({}));         // true ⚠️ (objet vide)
console.log(Boolean(-1));         // true (nombre non-zéro)
console.log(Boolean(Infinity));   // true

// Usage pratique
function processInput(value) {
    if (!value) {  // Vérifie si falsy
        return "No value provided";
    }
    return value;
}

console.log(processInput(""));       // "No value provided"
console.log(processInput(0));        // "No value provided"
console.log(processInput("hello"));  // "hello"
```

---

### Q9 : Que fait l'opérateur `+` avec différents types ?

**Réponse :**

L'opérateur `+` a deux comportements :
- **Concaténation** si au moins un opérande est un string
- **Addition numérique** sinon

```javascript
// Concaténation (dès qu'il y a un string)
console.log("5" + 3);        // "53" (string)
console.log(5 + "3");        // "53" (string)
console.log("Hello" + true); // "Hellotrue"
console.log("" + 42);        // "42" (conversion en string)

// Addition numérique
console.log(5 + 3);          // 8 (number)
console.log(true + true);    // 2 (true = 1)
console.log(true + false);   // 1

// Ordre d'évaluation important
console.log(1 + 2 + "3");    // "33" (d'abord 1+2=3, puis "3"+"3")
console.log("1" + 2 + 3);    // "123" (d'abord "1"+"2", puis "12"+"3")

// Autres opérateurs forcent la conversion en number
console.log("5" - 3);        // 2 (number)
console.log("10" * "2");     // 20 (number)
console.log("10" / "2");     // 5 (number)
console.log("5" - true);     // 4 (true = 1)

// Unary plus pour convertir en number
console.log(+"42");          // 42 (number)
console.log(+true);          // 1
```

---

## Questions sur Opérateurs

### Q10 : Quelle est la différence entre `||` et `??` ?

**Réponse :**

`||` (OR) retourne le premier **truthy** value ou la dernière valeur.  
`??` (nullish coalescing) retourne le côté droit seulement si le côté gauche est **null ou undefined**.

`??` est plus précis quand `0`, `""` ou `false` sont des valeurs valides.

```javascript
// || teste les valeurs falsy
console.log(0 || 10);           // 10 (0 est falsy)
console.log("" || "default");   // "default" ("" est falsy)
console.log(false || true);     // true (false est falsy)
console.log(null || "value");   // "value"

// ?? teste seulement null/undefined
console.log(0 ?? 10);           // 0 ✅ (0 n'est ni null ni undefined)
console.log("" ?? "default");   // "" ✅
console.log(false ?? true);     // false ✅
console.log(null ?? "value");   // "value"
console.log(undefined ?? "val");// "value"

// Cas pratique : configuration
const settings = {
    timeout: 0,      // 0 est une valeur valide!
    retries: null,   // non configuré
    debug: false     // false est une valeur valide!
};

// Avec || - PROBLÈME
const timeout1 = settings.timeout || 5000;  // 5000 ❌ (0 est falsy)
const debug1 = settings.debug || true;      // true ❌ (false est falsy)

// Avec ?? - CORRECT
const timeout2 = settings.timeout ?? 5000;  // 0 ✅
const debug2 = settings.debug ?? true;      // false ✅
const retries = settings.retries ?? 3;      // 3 ✅
```

---

### Q11 : Qu'est-ce que l'optional chaining (`?.`) ?

**Réponse :**

L'**optional chaining** (`?.`) permet d'accéder à des propriétés imbriquées sans risque d'erreur si une valeur intermédiaire est `null` ou `undefined`. Il retourne `undefined` au lieu de lancer une erreur.

```javascript
// Sans optional chaining
const user = {
    name: "Alice",
    address: {
        city: "Paris"
    }
};

// Si address n'existe pas
const user2 = { name: "Bob" };
// console.log(user2.address.city); // ❌ TypeError!

// Solution classique (verbeux)
const city1 = user2.address && user2.address.city;
console.log(city1); // undefined

// Avec optional chaining (ES2020)
const city2 = user2.address?.city;
console.log(city2); // undefined ✅

// Chaînage multiple
const country = user2.address?.country?.code;
console.log(country); // undefined

// Avec des méthodes
const result = user.getName?.();
console.log(result); // undefined (getName n'existe pas)

// Avec des arrays
const users = [{ name: "Alice" }];
console.log(users[0]?.name);      // "Alice"
console.log(users[5]?.name);      // undefined
console.log(users[5]?.address?.city); // undefined

// Combinaison avec ?? pour valeur par défaut
const userCity = user2.address?.city ?? "Unknown City";
console.log(userCity); // "Unknown City"

// API response typique
const apiResponse = {
    data: {
        user: {
            profile: {
                email: "test@example.com"
            }
        }
    }
};

const email = apiResponse.data?.user?.profile?.email ?? "no-email";
console.log(email); // "test@example.com"
```

---

### Q12 : Expliquez le "short-circuit evaluation"

**Réponse :**

Le **short-circuit evaluation** signifie que JavaScript arrête l'évaluation d'une expression logique dès qu'il connaît le résultat final.

Avec `&&` : si le premier est falsy, le reste n'est pas évalué.  
Avec `||` : si le premier est truthy, le reste n'est pas évalué.

```javascript
// && (AND) - s'arrête au premier falsy
console.log(false && console.log("Never printed"));  // false
console.log(true && console.log("Printed"));         // prints puis undefined

let count = 0;
true || count++;   // count++ ne s'exécute pas
console.log(count); // 0

false && count++;  // count++ ne s'exécute pas
console.log(count); // 0

// Usage pratique : exécution conditionnelle
const user = { name: "Alice" };
user && console.log(user.name);  // "Alice" (s'exécute si user existe)

// Valeur par défaut
const username = inputValue || "Guest";

// Appel de fonction conditionnel
isLoggedIn && redirectToDashboard();

// Chaînage
const isValid = 
    username && 
    username.length > 3 && 
    age >= 18;

// && retourne la première valeur falsy ou la dernière valeur
console.log("hello" && "world");    // "world"
console.log("hello" && 0);          // 0
console.log("" && "world");         // ""

// || retourne la première valeur truthy ou la dernière valeur
console.log("hello" || "world");    // "hello"
console.log(0 || "world");          // "world"
console.log(null || undefined);     // undefined
```

---

### Q13 : Quelle est la différence entre `i++` et `++i` ?

**Réponse :**

`i++` est le **post-increment** : il retourne la valeur actuelle, puis incrémente.  
`++i` est le **pre-increment** : il incrémente d'abord, puis retourne la nouvelle valeur.

```javascript
// Post-increment (i++)
let a = 5;
console.log(a++);  // 5 (retourne 5, puis a devient 6)
console.log(a);    // 6

// Pre-increment (++i)
let b = 5;
console.log(++b);  // 6 (b devient 6, puis retourne 6)
console.log(b);    // 6

// Post-decrement (i--)
let c = 5;
console.log(c--);  // 5 (retourne 5, puis c devient 4)
console.log(c);    // 4

// Pre-decrement (--i)
let d = 5;
console.log(--d);  // 4 (d devient 4, puis retourne 4)
console.log(d);    // 4

// Dans une boucle, pas de différence visible
for (let i = 0; i < 3; i++) {   // ou ++i
    console.log(i);  // 0, 1, 2
}

// Différence dans une affectation
let x = 5;
let y = x++;  // y = 5, x = 6
console.log(y, x);  // 5, 6

let p = 5;
let q = ++p;  // p = 6, q = 6
console.log(q, p);  // 6, 6

// Erreur commune
let arr = [10, 20, 30];
let index = 0;
console.log(arr[index++]);  // 10 (utilise index 0, puis index devient 1)
console.log(arr[index]);    // 20 (index est maintenant 1)
```

---

## Questions Pratiques / Pièges

### Q14 : Que va afficher ce code ?

```javascript
console.log(typeof null);
console.log(typeof []);
console.log(typeof function(){});
```

**Réponse :**

```javascript
console.log(typeof null);         // "object" ⚠️ (bug historique!)
console.log(typeof []);           // "object" (les arrays sont des objets)
console.log(typeof function(){}); // "function"

// Pour vérifier correctement
console.log(null === null);       // true (vérifier null)
console.log(Array.isArray([]));   // true (vérifier array)
```

`typeof null` retourne `"object"` à cause d'un **bug historique** de JavaScript qui ne peut plus être corrigé (breaking change).

---

### Q15 : Que va afficher ce code ?

```javascript
const obj = { x: 1 };
const obj2 = obj;
obj2.x = 2;
console.log(obj.x);
```

**Réponse :**

```javascript
console.log(obj.x);  // 2
```

Les objets sont passés **par référence**. `obj2` pointe vers le même objet en mémoire que `obj`. Modifier `obj2.x` modifie aussi `obj.x`.

```javascript
// Pour créer une vraie copie (shallow copy)
const obj3 = { ...obj };  // ou Object.assign({}, obj)
obj3.x = 3;
console.log(obj.x);   // 2 (non modifié)
console.log(obj3.x);  // 3
```

---

### Q16 : Que va afficher ce code ?

```javascript
console.log("5" + 2 - 1);
console.log("5" - 2 + 1);
```

**Réponse :**

```javascript
console.log("5" + 2 - 1);  // "51" (nombre)
// Étapes : "5" + 2 = "52" (concat), puis "52" - 1 = 51 (number)

console.log("5" - 2 + 1);  // 4 (nombre)
// Étapes : "5" - 2 = 3 (number), puis 3 + 1 = 4 (number)
```

Rappel : `+` concatène si un string est présent, les autres opérateurs (`-`, `*`, `/`) forcent la conversion en number.

---