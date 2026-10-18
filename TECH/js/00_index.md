# Programme Complet : JavaScript pour Développeur Backend
---

## [**Chapitre 1 : Fondamentaux du Langage**](./01_JS-Fundamentals.md)

### 1.1 Introduction et Setup
- Qu'est-ce que JavaScript ?
- ECMAScript et versions (ES5, ES6+)
- Console du navigateur et outils de développement
- Premier script JavaScript

### 1.2 Variables et Déclarations
- `var`, `let`, `const` : différences et portée (scope)
- Hoisting (remontée des variables)
- Block scope vs Function scope
- Bonnes pratiques de déclaration

### 1.3 Types de Données
- Types primitifs : `string`, `number`, `boolean`, `null`, `undefined`, `symbol`, `bigint`
- Type complexe : `object`
- `typeof` operator
- Type coercion (conversion implicite)
- Conversion explicite

### 1.4 Opérateurs
- Opérateurs arithmétiques (+, -, *, /, %, **)
- Opérateurs d'affectation (=, +=, -=, etc.)
- Opérateurs de comparaison (==, ===, !=, !==, <, >, etc.)
- Opérateurs logiques (&&, ||, !)
- Opérateurs unaires (++, --, +, -, typeof, delete)
- Opérateur ternaire (condition ? true : false)
- Nullish coalescing operator (??)
- Optional chaining operator (?.)

---

## [**Chapitre 2 : Structures de Contrôle**](./02_Control-Structures.md)

### 2.1 Conditions
- `if`, `else if`, `else`
- `switch` statement
- Truthy et Falsy values

### 2.2 Boucles
- `for` loop
- `while` et `do...while`
- `for...in` (pour objets)
- `for...of` (pour itérables)
- `break` et `continue`

---

## [**Chapitre 3 : Fonctions**](./03_Functions.md)

### 3.1 Déclaration et Utilisation
- Function declaration
- Function expression
- Arrow functions (=>)
- Paramètres et arguments
- Return values
- Default parameters

### 3.2 Concepts Avancés
- Function scope et closures
- Callback functions
- Higher-Order Functions (map, filter, reduce, forEach, find, some, every)
- Pure functions vs side effects
- IIFE (Immediately Invoked Function Expression)

---

## [**Chapitre 4 : Types Complexes - Chaînes et Nombres**](./04_Complex-Types-Strings-Numbers.md)

### 4.1 String Methods
- Manipulation : `slice()`, `substring()`, `substr()`, `replace()`, `replaceAll()`
- Casse : `toLowerCase()`, `toUpperCase()`
- Recherche : `indexOf()`, `lastIndexOf()`, `includes()`, `startsWith()`, `endsWith()`
- Découpage : `split()`, `trim()`, `trimStart()`, `trimEnd()`
- Template literals (backticks)
- `charAt()`, `charCodeAt()`, `concat()`, `repeat()`, `padStart()`, `padEnd()`

### 4.2 Number Methods et Math Object
- `parseInt()`, `parseFloat()`, `Number()`
- `toFixed()`, `toPrecision()`, `toString()`
- `isNaN()`, `isFinite()`, `isInteger()`
- **Math Object** : `Math.round()`, `Math.ceil()`, `Math.floor()`, `Math.max()`, `Math.min()`, `Math.random()`, `Math.pow()`, `Math.sqrt()`, `Math.abs()`

---

## [**Chapitre 5 : Arrays (Tableaux)**](./05_Arrays.md)

### 5.1 Bases des Arrays
- Création et accès aux éléments
- Propriété `length`
- Arrays multidimensionnels

### 5.2 Array Methods Essentiels
- Ajout/Suppression : `push()`, `pop()`, `shift()`, `unshift()`, `splice()`
- Recherche : `indexOf()`, `lastIndexOf()`, `includes()`, `find()`, `findIndex()`
- Transformation : `slice()`, `concat()`, `join()`, `reverse()`, `sort()`
- Itération : `forEach()`, `map()`, `filter()`, `reduce()`, `reduceRight()`
- Tests : `some()`, `every()`
- Autres : `flat()`, `flatMap()`, `fill()`, `from()`, `isArray()`

---

## [**Chapitre 6 : Objects (Objets)**](./06_Objects.md)

### 6.1 Fondamentaux des Objets
- Création d'objets (literal, constructor)
- Accès aux propriétés (dot notation, bracket notation)
- Ajout/Modification/Suppression de propriétés
- Méthodes d'objets
- `this` keyword

### 6.2 Manipulation d'Objets
- `Object.keys()`, `Object.values()`, `Object.entries()`
- `Object.assign()`, spread operator (`...`)
- `Object.freeze()`, `Object.seal()`
- `hasOwnProperty()`, `in` operator
- Computed property names

---

## [**Chapitre 7 : Destructuring**](./07_Destructuring.md)

### 7.1 Array Destructuring
- Syntaxe de base
- Valeurs par défaut
- Skipping items
- Rest operator (...)

### 7.2 Object Destructuring
- Syntaxe de base
- Renommage de variables
- Valeurs par défaut
- Nested destructuring
- Destructuring dans les paramètres de fonction

---

## [**Chapitre 8 : Structures de Données Modernes**](./08_Modern-Data-Structures.md)

### 8.1 Set et WeakSet
- Création et utilisation de Set
- Méthodes : `add()`, `delete()`, `has()`, `clear()`, `size`
- Itération sur un Set
- Cas d'usage (dédoublonnage)
- WeakSet et différences

### 8.2 Map et WeakMap
- Création et utilisation de Map
- Méthodes : `set()`, `get()`, `has()`, `delete()`, `clear()`, `size`
- Itération sur un Map (`keys()`, `values()`, `entries()`)
- Map vs Object
- WeakMap et différences

---

## [**Chapitre 9 : DOM (Document Object Model)**](./09_DOM.md)

### 9.1 Introduction au DOM
- Qu'est-ce que le DOM ?
- Structure arborescente
- Nodes, Elements, Attributes

### 9.2 Sélection d'Éléments (DOM Selectors)
- `getElementById()`
- `getElementsByClassName()`
- `getElementsByTagName()`
- `querySelector()`, `querySelectorAll()`
- Différences et performances

### 9.3 Manipulation de Contenu et Attributs
- Lire/Modifier le contenu : `innerHTML`, `textContent`, `innerText`
- Attributs : `getAttribute()`, `setAttribute()`, `removeAttribute()`
- Vérifier les attributs : `hasAttribute()`
- Data attributes (`dataset`)

### 9.4 Création et Manipulation d'Éléments
- `createElement()`, `createTextNode()`
- `appendChild()`, `append()`, `prepend()`
- `insertBefore()`, `insertAdjacentElement()`
- `replaceChild()`, `removeChild()`, `remove()`

### 9.5 Gestion des Children
- `children`, `childNodes`
- `firstChild`, `lastChild`, `firstElementChild`, `lastElementChild`
- `parentNode`, `parentElement`

### 9.6 DOM Traversing (Navigation)
- `nextSibling`, `previousSibling`
- `nextElementSibling`, `previousElementSibling`
- `closest()` (recherche d'ancêtre)

### 9.7 Clonage d'Éléments
- `cloneNode(deep)`
- Cas d'usage

### 9.8 Manipulation CSS
- `style` property (inline styles)
- `classList` : `add()`, `remove()`, `toggle()`, `contains()`, `replace()`
- `getComputedStyle()`
- Modification des classes : `className`

### 9.9 Événements DOM
- Qu'est-ce qu'un événement ?
- Types d'événements (click, submit, keydown, mouseover, etc.)
- `addEventListener()` vs inline handlers
- Event object : `target`, `currentTarget`, `preventDefault()`, `stopPropagation()`
- Event bubbling et capturing
- Event delegation
- `removeEventListener()`

### 9.10 Simulation d'Événements
- `click()`, `focus()`, `blur()`
- `dispatchEvent()` avec `CustomEvent`

---

## [**Chapitre 10 : BOM (Browser Object Model)**](./10_BOM.md)

### 10.1 Window Object
- Introduction au BOM
- Propriétés globales (`window.innerWidth`, `window.innerHeight`, etc.)

### 10.2 Dialogues Utilisateur
- `alert()`
- `confirm()`
- `prompt()`

### 10.3 Timers
- `setTimeout()`, `clearTimeout()`
- `setInterval()`, `clearInterval()`

### 10.4 Window Location
- `window.location` : `href`, `protocol`, `host`, `pathname`, `search`, `hash`
- Navigation : `assign()`, `replace()`, `reload()`

### 10.5 Window History
- `history.back()`, `history.forward()`, `history.go()`
- `history.pushState()`, `history.replaceState()`

### 10.6 Window Actions
- `window.open()`, `window.close()`
- `window.print()`
- `window.stop()`
- `scrollTo()`, `scrollBy()`, `scroll()`
- `focus()`, `blur()`

### 10.7 Web Storage API
- **Local Storage** : `localStorage.setItem()`, `getItem()`, `removeItem()`, `clear()`
- **Session Storage** : `sessionStorage` (même API)
- Différences et cas d'usage
- Stockage JSON : `JSON.stringify()`, `JSON.parse()`

---

## [**Chapitre 11 : Expressions Régulières (RegEx)**](./11_RegEx.md)

### 11.1 Introduction aux RegEx
- Syntaxe de base
- Création : literal (`/pattern/`) vs constructor (`new RegExp()`)

### 11.2 Patterns et Flags
- Caractères spéciaux : `.`, `^`, `$`, `*`, `+`, `?`, `[]`, `{}`, `()`, `|`, `\`
- Flags : `g`, `i`, `m`, `s`, `u`, `y`
- Character classes et ranges

### 11.3 Méthodes RegEx
- `test()` (retourne boolean)
- `exec()` (retourne array)
- String methods : `match()`, `matchAll()`, `search()`, `replace()`, `split()`

---

## [**Chapitre 12 : Programmation Orientée Objet (OOP)**](./12_OOP.md)

### 12.1 Prototypes
- Prototype chain
- `__proto__` vs `prototype`
- `Object.create()`

### 12.2 Classes ES6
- Syntaxe `class`
- Constructor
- Méthodes et propriétés
- Getters et Setters
- Static methods

### 12.3 Héritage
- `extends` keyword
- `super` keyword
- Method overriding

### 12.4 Encapsulation
- Private fields (`#`)
- Private methods

---

## [**Chapitre 13 : Date et Time**](./13_Date-and-Time.md)

### 13.1 Date Object
- Création : `new Date()`
- Timestamps

### 13.2 Méthodes Date
- Get methods : `getFullYear()`, `getMonth()`, `getDate()`, `getDay()`, `getHours()`, `getMinutes()`, `getSeconds()`, `getTime()`
- Set methods : `setFullYear()`, `setMonth()`, `setDate()`, etc.
- `toLocaleDateString()`, `toLocaleTimeString()`, `toISOString()`
- `Date.now()`, `Date.parse()`

---

## [**Chapitre 14 : Generators**](./14_Generators.md)

### 14.1 Introduction aux Generators
- Syntaxe `function*`
- Yield keyword
- Iterator protocol

### 14.2 Utilisation Pratique
- `next()` method
- Passing values to generators
- `yield*` delegation
- Cas d'usage (lazy evaluation, infinite sequences)

---

## [**Chapitre 15 : Modules JavaScript**](./15_JS-Modules.md)

### 15.1 Système de Modules ES6
- `export` (named exports, default export)
- `import` syntax
- Renommage (`as`)
- Import dynamique (`import()`)

### 15.2 Organisation du Code
- Structure de fichiers
- Bonnes pratiques
- Module scope

---

## [**Chapitre 16 : Programmation Asynchrone**](./16_Asynchronous-Programming.md)

### 16.1 Introduction à l'Asynchrone
- Call stack et Event Loop
- Callback Queue / Task Queue
- Microtasks vs Macrotasks

### 16.2 Callbacks
- Callback functions
- Callback hell
- Error handling avec callbacks

### 16.3 Promises
- Création de Promises (`new Promise()`)
- States : pending, fulfilled, rejected
- `.then()`, `.catch()`, `.finally()`
- Chaining promises
- Error handling

### 16.4 Méthodes Statiques des Promises
- `Promise.all()` (toutes réussies)
- `Promise.allSettled()` (toutes terminées)
- `Promise.race()` (première terminée)
- `Promise.any()` (première réussie)

### 16.5 Async/Await
- Syntaxe `async function`
- Keyword `await`
- Error handling avec `try...catch`
- Async/await vs Promises
- Parallel execution avec `Promise.all()`

### 16.6 Fetch API et AJAX
- Introduction à `fetch()`
- Making GET requests
- Making POST requests (avec body, headers)
- Handling responses : `.json()`, `.text()`, `.blob()`
- Error handling avec fetch
- Headers et options
- CORS basics

---

## [**Chapitre 17 : Concepts Avancés JavaScript**](./17_Advanced-JS-Concepts.md)

### 17.1 Spread et Rest Operators
- Spread operator avec arrays et objects
- Rest parameters dans les fonctions

### 17.2 Short-Circuit Evaluation
- `&&` et `||` pour valeurs
- Nullish coalescing (`??`) vs OR (`||`)

### 17.3 Error Handling
- `try...catch...finally`
- Throwing errors (`throw`)
- Custom errors (extending Error)

### 17.4 Strict Mode
- `'use strict'`
- Différences et avantages

### 17.5 JSON
- `JSON.stringify()` et options
- `JSON.parse()` et reviver function

---

## [**Annexes et Bonnes Pratiques**](./18_annexe_best-practise.md)

### A. Conventions de Code
- Naming conventions
- Indentation et formatage
- Comments et documentation

### B. Debugging
- `console` methods : `log()`, `error()`, `warn()`, `table()`, `time()`, `timeEnd()`
- Breakpoints dans le navigateur
- Debugger statement

### C. Performance
- Éviter les fuites mémoire
- Optimisation des boucles
- Debouncing et Throttling

### D. Transition vers React
- Concepts JavaScript essentiels pour React
- JSX est du JavaScript
- Composants = Fonctions
- Hooks = Closures + State management

---