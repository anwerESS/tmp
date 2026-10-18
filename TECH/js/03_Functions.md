## [**..**](./00_index.md)

# Chapitre 3 : Fonctions

## 3.1 Déclaration et Utilisation

Les fonctions sont des blocs de code réutilisables qui effectuent une tâche spécifique.

### Function Declaration

```javascript
// Déclaration de fonction (function declaration)
function greet() {
    console.log("Hello!");
}

greet();  // Appel de la fonction → "Hello!"

// Fonction avec paramètres
function greetUser(name) {
    console.log(`Hello ${name}!`);
}

greetUser("Alice");  // "Hello Alice!"
greetUser("Bob");    // "Hello Bob!"

// Plusieurs paramètres
function add(a, b) {
    return a + b;
}

const result = add(5, 3);
console.log(result);  // 8

// Return value
function multiply(x, y) {
    return x * y;
    console.log("Never executed");  // Code après return n'est jamais exécuté
}

const product = multiply(4, 5);
console.log(product);  // 20

// Fonction sans return (retourne undefined)
function sayHello() {
    console.log("Hello");
    // Pas de return
}

const value = sayHello();  // Affiche "Hello"
console.log(value);        // undefined

// Return anticipé (early return)
function checkAge(age) {
    if (age < 0) {
        return "Invalid age";
    }
    
    if (age < 18) {
        return "Minor";
    }
    
    return "Adult";
}

console.log(checkAge(15));   // "Minor"
console.log(checkAge(25));   // "Adult"
console.log(checkAge(-5));   // "Invalid age"

// Hoisting : function declarations sont remontées
sayHi();  // ✅ Fonctionne! (hoisted)

function sayHi() {
    console.log("Hi!");
}
```

### Function Expression

```javascript
// Function expression : fonction assignée à une variable
const greet2 = function() {
    console.log("Hello from expression!");
};

greet2();  // "Hello from expression!"

// ⚠️ Function expressions ne sont PAS hoisted
// greet3();  // ❌ ReferenceError: Cannot access before initialization

const greet3 = function() {
    console.log("Hi!");
};

greet3();  // ✅ Fonctionne

// Named function expression (utile pour recursion/debugging)
const factorial = function fact(n) {
    if (n <= 1) return 1;
    return n * fact(n - 1);  // Peut s'appeler elle-même via 'fact'
};

console.log(factorial(5));  // 120

// Function expression avec paramètres
const subtract = function(a, b) {
    return a - b;
};

console.log(subtract(10, 3));  // 7

// Fonction comme valeur
const operations = {
    add: function(a, b) { return a + b; },
    multiply: function(a, b) { return a * b; }
};

console.log(operations.add(2, 3));       // 5
console.log(operations.multiply(2, 3));  // 6
```

### Arrow Functions

```javascript
// Arrow function (ES6) : syntaxe concise
const greet4 = () => {
    console.log("Hello from arrow!");
};

greet4();  // "Hello from arrow!"

// Avec un paramètre (parenthèses optionnelles)
const square = x => {
    return x * x;
};

console.log(square(5));  // 25

// Return implicite (sans accolades, une seule expression)
const square2 = x => x * x;
console.log(square2(5));  // 25

// Plusieurs paramètres
const add2 = (a, b) => a + b;
console.log(add2(3, 4));  // 7

// Return d'objet (attention aux accolades!)
// ❌ Faux
// const makeUser = name => { name: name };  // Interprété comme block, pas objet

// ✅ Correct : entourer l'objet de parenthèses
const makeUser = name => ({ name: name });
console.log(makeUser("Alice"));  // { name: "Alice" }

// Ou version longue
const makeUser2 = name => {
    return { name: name };
};

// Arrow function sans paramètres
const getRandom = () => Math.random();
console.log(getRandom());

// Arrow function multilignes
const processData = (data) => {
    const cleaned = data.trim();
    const upper = cleaned.toUpperCase();
    return upper;
};

console.log(processData("  hello  "));  // "HELLO"

// Comparaison syntaxes
// Function declaration
function sum1(a, b) {
    return a + b;
}

// Function expression
const sum2 = function(a, b) {
    return a + b;
};

// Arrow function
const sum3 = (a, b) => a + b;

console.log(sum1(1, 2));  // 3
console.log(sum2(1, 2));  // 3
console.log(sum3(1, 2));  // 3

// ⚠️ Différence importante : arrow functions n'ont pas leur propre 'this'
// (on verra ça plus tard)
```

### Paramètres et Arguments

```javascript
// Paramètres vs Arguments
// Paramètres : variables dans la déclaration
// Arguments : valeurs passées lors de l'appel

function example(param1, param2) {  // param1, param2 = paramètres
    console.log(param1, param2);
}

example("arg1", "arg2");  // "arg1", "arg2" = arguments

// Trop d'arguments (ignorés)
function showName(name) {
    console.log(name);
}

showName("Alice", "Bob", "Charlie");  // "Alice" (les autres ignorés)

// Pas assez d'arguments (undefined)
function showFullName(firstName, lastName) {
    console.log(firstName, lastName);
}

showFullName("Alice");  // "Alice undefined"

// Arguments object (fonction classique seulement, pas arrow)
function showAllArgs() {
    console.log(arguments);  // Array-like object
    console.log(arguments.length);
    console.log(arguments[0]);
}

showAllArgs(1, 2, 3);  // Arguments[1, 2, 3], 3, 1

// ⚠️ arguments n'existe pas dans arrow functions
const arrowFunc = () => {
    // console.log(arguments);  // ❌ ReferenceError
};

// Conversion arguments en array
function sumAll() {
    const argsArray = Array.from(arguments);
    return argsArray.reduce((sum, num) => sum + num, 0);
}

console.log(sumAll(1, 2, 3, 4));  // 10

// Ou avec spread operator
function sumAll2() {
    return [...arguments].reduce((sum, num) => sum + num, 0);
}
```

### Default Parameters

```javascript
// Paramètres par défaut (ES6)
function greet5(name = "Guest") {
    console.log(`Hello ${name}!`);
}

greet5("Alice");  // "Hello Alice!"
greet5();         // "Hello Guest!"
greet5(undefined);// "Hello Guest!" (undefined → valeur par défaut)
greet5(null);     // "Hello null!" ⚠️ (null n'active pas le défaut)

// Plusieurs paramètres par défaut
function createUser(name = "Anonymous", age = 0, country = "Unknown") {
    return { name, age, country };
}

console.log(createUser());                      // { name: "Anonymous", age: 0, country: "Unknown" }
console.log(createUser("Alice"));              // { name: "Alice", age: 0, country: "Unknown" }
console.log(createUser("Bob", 25));            // { name: "Bob", age: 25, country: "Unknown" }
console.log(createUser("Charlie", 30, "USA")); // { name: "Charlie", age: 30, country: "USA" }

// Valeur par défaut peut être une expression
function getTimestamp() {
    return Date.now();
}

function logMessage(message, timestamp = getTimestamp()) {
    console.log(`[${timestamp}] ${message}`);
}

logMessage("Hello");  // [1234567890] Hello (timestamp actuel)

// Valeurs par défaut avec paramètres précédents
function calculatePrice(price, tax = price * 0.2) {
    return price + tax;
}

console.log(calculatePrice(100));      // 120 (100 + 20)
console.log(calculatePrice(100, 10));  // 110 (100 + 10 custom tax)

// Avant ES6 : pattern avec ||
function greetOld(name) {
    name = name || "Guest";  // ⚠️ Problème si name = "" ou 0
    console.log(`Hello ${name}`);
}

// Meilleure pratique avant ES6
function greetBetter(name) {
    if (name === undefined) {
        name = "Guest";
    }
    console.log(`Hello ${name}`);
}

// Avec nullish coalescing (ES2020)
function greetModern(name) {
    name = name ?? "Guest";  // ✅ Parfait
    console.log(`Hello ${name}`);
}

greetModern("");     // "Hello !" (empty string préservé)
greetModern(0);      // "Hello 0!" (0 préservé)
greetModern(null);   // "Hello Guest!"
greetModern(undefined); // "Hello Guest!"
```

---

## 3.2 Concepts Avancés

### Function Scope et Closures

```javascript
// Function scope : variables déclarées dans une fonction
// sont accessibles seulement dans cette fonction

function outer() {
    const outerVar = "I'm outer";
    
    console.log(outerVar);  // ✅ Accessible
    
    function inner() {
        const innerVar = "I'm inner";
        console.log(outerVar);  // ✅ Accessible (scope parent)
        console.log(innerVar);  // ✅ Accessible
    }
    
    inner();
    // console.log(innerVar);  // ❌ ReferenceError
}

outer();
// console.log(outerVar);  // ❌ ReferenceError

// Closure : fonction qui "se souvient" de son environnement lexical
function createCounter() {
    let count = 0;  // Variable privée
    
    return function() {
        count++;
        return count;
    };
}

const counter = createCounter();
console.log(counter());  // 1
console.log(counter());  // 2
console.log(counter());  // 3

// count n'est pas accessible de l'extérieur
// console.log(count);  // ❌ ReferenceError

// Chaque appel à createCounter crée un nouveau scope
const counter2 = createCounter();
console.log(counter2());  // 1 (nouveau compteur indépendant)
console.log(counter());   // 4 (ancien compteur continue)

// Closure avec paramètres
function createMultiplier(multiplier) {
    return function(number) {
        return number * multiplier;
    };
}

const double = createMultiplier(2);
const triple = createMultiplier(3);

console.log(double(5));  // 10
console.log(triple(5));  // 15

// Closure pratique : private variables
function createBankAccount(initialBalance) {
    let balance = initialBalance;  // Variable privée
    
    return {
        deposit: function(amount) {
            if (amount > 0) {
                balance += amount;
                return balance;
            }
        },
        withdraw: function(amount) {
            if (amount > 0 && amount <= balance) {
                balance -= amount;
                return balance;
            }
            return "Insufficient funds";
        },
        getBalance: function() {
            return balance;
        }
    };
}

const account = createBankAccount(100);
console.log(account.getBalance());  // 100
console.log(account.deposit(50));   // 150
console.log(account.withdraw(30));  // 120
// console.log(account.balance);    // undefined (privée!)

// Closure dans une boucle (piège classique)
// ❌ Problème avec var
for (var i = 0; i < 3; i++) {
    setTimeout(function() {
        console.log(i);  // 3, 3, 3 ⚠️
    }, 100);
}

// ✅ Solution avec let (block scope)
for (let j = 0; j < 3; j++) {
    setTimeout(function() {
        console.log(j);  // 0, 1, 2 ✅
    }, 100);
}

// ✅ Solution avec closure (avant ES6)
for (var k = 0; k < 3; k++) {
    (function(index) {
        setTimeout(function() {
            console.log(index);  // 0, 1, 2 ✅
        }, 100);
    })(k);
}
```

### Callback Functions

```javascript
// Callback : fonction passée en argument à une autre fonction

// Exemple simple
function processData(data, callback) {
    const result = data * 2;
    callback(result);  // Appel du callback avec le résultat
}

processData(5, function(result) {
    console.log("Result:", result);  // "Result: 10"
});

// Avec arrow function
processData(10, result => console.log("Result:", result));  // "Result: 20"

// Callbacks multiples
function calculate(a, b, onSuccess, onError) {
    if (typeof a !== 'number' || typeof b !== 'number') {
        onError("Invalid input");
        return;
    }
    
    const result = a + b;
    onSuccess(result);
}

calculate(5, 3, 
    result => console.log("Success:", result),
    error => console.log("Error:", error)
);  // "Success: 8"

calculate("5", 3,
    result => console.log("Success:", result),
    error => console.log("Error:", error)
);  // "Error: Invalid input"

// Callbacks avec arrays (très courant)
const numbers = [1, 2, 3, 4, 5];

// forEach : exécute callback pour chaque élément
numbers.forEach(function(num) {
    console.log(num * 2);
});  // 2, 4, 6, 8, 10

// map : transforme chaque élément
const doubled = numbers.map(function(num) {
    return num * 2;
});
console.log(doubled);  // [2, 4, 6, 8, 10]

// filter : filtre les éléments
const evens = numbers.filter(function(num) {
    return num % 2 === 0;
});
console.log(evens);  // [2, 4]

// Event listeners (DOM)
// document.getElementById('btn').addEventListener('click', function() {
//     console.log('Button clicked!');
// });

// Callback hell (problème à éviter)
setTimeout(function() {
    console.log("Step 1");
    setTimeout(function() {
        console.log("Step 2");
        setTimeout(function() {
            console.log("Step 3");
            // ❌ Difficile à lire et maintenir
        }, 1000);
    }, 1000);
}, 1000);

// Solution moderne : Promises et async/await (on verra plus tard)
```

### Higher-Order Functions

```javascript
// Higher-Order Function : fonction qui :
// 1. Prend une fonction en paramètre, OU
// 2. Retourne une fonction

// 1. Prendre une fonction en paramètre
function repeat(n, action) {
    for (let i = 0; i < n; i++) {
        action(i);
    }
}

repeat(3, index => console.log(`Iteration ${index}`));
// Iteration 0
// Iteration 1
// Iteration 2

// 2. Retourner une fonction
function createGreeting(greeting) {
    return function(name) {
        return `${greeting}, ${name}!`;
    };
}

const sayHello2 = createGreeting("Hello");
const sayBonjour = createGreeting("Bonjour");

console.log(sayHello2("Alice"));   // "Hello, Alice!"
console.log(sayBonjour("Pierre")); // "Bonjour, Pierre!"

// map : Higher-Order Function classique
const nums = [1, 2, 3, 4, 5];

const squared = nums.map(function(num) {
    return num * num;
});
console.log(squared);  // [1, 4, 9, 16, 25]

// Avec arrow function (plus concis)
const squared2 = nums.map(num => num * num);

// filter : filtre selon une condition
const evenNumbers = nums.filter(num => num % 2 === 0);
console.log(evenNumbers);  // [2, 4]

// reduce : réduit à une seule valeur
const sum = nums.reduce((accumulator, current) => {
    return accumulator + current;
}, 0);  // 0 = valeur initiale
console.log(sum);  // 15

// reduce étape par étape
// Iteration 1: accumulator=0, current=1 → return 0+1=1
// Iteration 2: accumulator=1, current=2 → return 1+2=3
// Iteration 3: accumulator=3, current=3 → return 3+3=6
// Iteration 4: accumulator=6, current=4 → return 6+4=10
// Iteration 5: accumulator=10, current=5 → return 10+5=15

// reduce : trouver le max
const max = nums.reduce((max, current) => {
    return current > max ? current : max;
});
console.log(max);  // 5

// forEach : exécute une action pour chaque élément
nums.forEach((num, index) => {
    console.log(`Index ${index}: ${num}`);
});

// find : trouve le premier élément qui satisfait la condition
const firstEven = nums.find(num => num % 2 === 0);
console.log(firstEven);  // 2

// findIndex : trouve l'index du premier élément
const indexOfThree = nums.findIndex(num => num === 3);
console.log(indexOfThree);  // 2

// some : vérifie si AU MOINS UN élément satisfait la condition
const hasEven = nums.some(num => num % 2 === 0);
console.log(hasEven);  // true

const hasNegative = nums.some(num => num < 0);
console.log(hasNegative);  // false

// every : vérifie si TOUS les éléments satisfont la condition
const allPositive = nums.every(num => num > 0);
console.log(allPositive);  // true

const allEven = nums.every(num => num % 2 === 0);
console.log(allEven);  // false

// Chaînage de Higher-Order Functions
const result2 = nums
    .filter(num => num % 2 === 0)  // [2, 4]
    .map(num => num * num)          // [4, 16]
    .reduce((sum, num) => sum + num, 0);  // 20

console.log(result2);  // 20

// Exemple pratique : traitement de données
const users2 = [
    { name: "Alice", age: 25, active: true },
    { name: "Bob", age: 30, active: false },
    { name: "Charlie", age: 35, active: true },
    { name: "David", age: 20, active: true }
];

// Récupérer les noms des utilisateurs actifs de plus de 21 ans
const activeAdultNames = users2
    .filter(user => user.active)
    .filter(user => user.age > 21)
    .map(user => user.name);

console.log(activeAdultNames);  // ["Alice", "Charlie"]

// Calculer l'âge moyen des utilisateurs actifs
const activeUsers = users2.filter(user => user.active);
const totalAge = activeUsers.reduce((sum, user) => sum + user.age, 0);
const averageAge = totalAge / activeUsers.length;

console.log(averageAge);  // 26.666...
```

### Pure Functions et Side Effects

```javascript
// Pure Function : 
// 1. Même input → toujours même output
// 2. Pas de side effects (ne modifie rien en dehors)

// ✅ Pure function
function add3(a, b) {
    return a + b;
}

console.log(add3(2, 3));  // Toujours 5
console.log(add3(2, 3));  // Toujours 5

// ✅ Pure function
function multiply2(x, y) {
    const result = x * y;
    return result;
}

// ❌ Impure : dépend d'une variable externe
let tax = 0.2;

function calculateTotal(price) {
    return price + (price * tax);  // Dépend de 'tax'
}

console.log(calculateTotal(100));  // 120
tax = 0.1;
console.log(calculateTotal(100));  // 110 ⚠️ Résultat différent!

// ✅ Pure : pas de dépendance externe
function calculateTotal2(price, taxRate) {
    return price + (price * taxRate);
}

console.log(calculateTotal2(100, 0.2));  // Toujours 120
console.log(calculateTotal2(100, 0.2));  // Toujours 120

// ❌ Impure : modifie l'argument (side effect)
function addToArray(arr, item) {
    arr.push(item);  // Modifie l'array original!
    return arr;
}

const myArray = [1, 2, 3];
addToArray(myArray, 4);
console.log(myArray);  // [1, 2, 3, 4] ⚠️ Modifié!

// ✅ Pure : retourne un nouveau array
function addToArrayPure(arr, item) {
    return [...arr, item];  // Nouveau array
}

const myArray2 = [1, 2, 3];
const newArray = addToArrayPure(myArray2, 4);
console.log(myArray2);  // [1, 2, 3] ✅ Inchangé
console.log(newArray);  // [1, 2, 3, 4] ✅ Nouveau

// ❌ Impure : side effects (console, DOM, API calls, etc.)
function logAndDouble(x) {
    console.log(x);  // Side effect: console
    return x * 2;
}

// ❌ Impure : utilise Math.random() (non déterministe)
function getRandomNumber() {
    return Math.random();  // Résultat différent à chaque appel
}

// ❌ Impure : utilise Date.now() (non déterministe)
function getCurrentTime() {
    return Date.now();  // Résultat différent à chaque appel
}

// Avantages des pure functions :
// 1. Faciles à tester
// 2. Faciles à comprendre
// 3. Pas de bugs liés aux side effects
// 4. Peuvent être memoizées (cache des résultats)
// 5. Peuvent être exécutées en parallèle

// Exemple pratique
const products = [
    { id: 1, name: "Laptop", price: 1000 },
    { id: 2, name: "Mouse", price: 50 },
    { id: 3, name: "Keyboard", price: 100 }
];

// ❌ Impure : modifie l'array original
function applyDiscount(products, discount) {
    products.forEach(product => {
        product.price = product.price * (1 - discount);
    });
    return products;
}

// ✅ Pure : retourne un nouveau array
function applyDiscountPure(products, discount) {
    return products.map(product => ({
        ...product,
        price: product.price * (1 - discount)
    }));
}

const discountedProducts = applyDiscountPure(products, 0.1);
console.log(products[0].price);           // 1000 ✅ Original inchangé
console.log(discountedProducts[0].price); // 900 ✅ Nouveau array
```

### IIFE (Immediately Invoked Function Expression)

```javascript
// IIFE : fonction qui s'exécute immédiatement après sa définition

// Syntaxe de base
(function() {
    console.log("IIFE executed!");
})();

// Avec paramètres
(function(name) {
    console.log(`Hello ${name}!`);
})("Alice");

// Arrow function IIFE
(() => {
    console.log("Arrow IIFE!");
})();

// Pourquoi utiliser IIFE ?
// 1. Créer un scope privé (avant let/const)

// ❌ Sans IIFE : variables globales
var counter1 = 0;
counter1++;
console.log(counter1);  // Variables polluent le scope global

// ✅ Avec IIFE : variables privées
(function() {
    var counter2 = 0;
    counter2++;
    console.log(counter2);  // 1
})();

// console.log(counter2);  // ❌ ReferenceError (privé)

// 2. Éviter les conflits de noms
(function() {
    var name = "Alice";
    console.log(name);  // "Alice"
})();

(function() {
    var name = "Bob";
    console.log(name);  // "Bob" (pas de conflit)
})();

// 3. Module pattern (avant ES6 modules)
const calculator = (function() {
    // Variables privées
    let result = 0;
    
    // Fonctions privées
    function log(message) {
        console.log(`[Calculator] ${message}`);
    }
    
    // API publique
    return {
        add: function(x) {
            result += x;
            log(`Added ${x}, result: ${result}`);
            return this;  // Pour le chaînage
        },
        subtract: function(x) {
            result -= x;
            log(`Subtracted ${x}, result: ${result}`);
            return this;
        },
        getResult: function() {
            return result;
        },
        reset: function() {
            result = 0;
            log("Reset");
            return this;
        }
    };
})();

calculator.add(10).subtract(3).add(5);
console.log(calculator.getResult());  // 12
// console.log(calculator.result);    // undefined (privé)

// IIFE avec return value
const value2 = (function() {
    const x = 10;
    const y = 20;
    return x + y;
})();

console.log(value2);  // 30

// IIFE dans une boucle (solution ancien style)
for (var i = 0; i < 3; i++) {
    (function(index) {
        setTimeout(function() {
            console.log(index);  // 0, 1, 2
        }, 100);
    })(i);
}

// Aujourd'hui, on utilise plutôt let
for (let j = 0; j < 3; j++) {
    setTimeout(function() {
        console.log(j);  // 0, 1, 2
    }, 100);
}

// IIFE moins utilisé aujourd'hui grâce à :
// - let/const (block scope)
// - ES6 modules
// - Build tools (webpack, etc.)
```

---

**Résumé du Chapitre 3 :**

✅ **Function declaration** : hoisted, peut être appelée avant déclaration  
✅ **Function expression** : pas hoisted, assignée à une variable  
✅ **Arrow functions** : syntaxe concise, pas de `this` propre, return implicite  
✅ **Default parameters** : valeurs par défaut pour paramètres manquants  
✅ **Closures** : fonction qui "se souvient" de son environnement lexical  
✅ **Callbacks** : fonctions passées en argument (asynchrone, events, arrays)  
✅ **Higher-Order Functions** : `map`, `filter`, `reduce`, `forEach`, `find`, `some`, `every`  
✅ **Pure functions** : même input → même output, pas de side effects  
✅ **IIFE** : fonction auto-exécutée (moins utilisé avec ES6+)  


---

# Questions/Réponses Orales - Chapitre 3 : Fonctions

## Questions sur Déclaration et Utilisation

### Q1 : Quelle est la différence entre function declaration et function expression ?

**Réponse :**

La **function declaration** est hoisted (remontée), donc elle peut être appelée avant sa déclaration dans le code. La **function expression** n'est pas hoisted, elle doit être déclarée avant d'être utilisée.

```javascript
// ✅ Function declaration - hoisted
sayHello();  // Fonctionne!

function sayHello() {
    console.log("Hello");
}

// ❌ Function expression - pas hoisted
// sayBye();  // ReferenceError: Cannot access before initialization

const sayBye = function() {
    console.log("Bye");
};

sayBye();  // ✅ Fonctionne maintenant

// Autre différence : nommage
// Function declaration doit avoir un nom
function namedFunc() { }  // ✅

// Function expression peut être anonyme
const anonymous = function() { };  // ✅

// Named function expression (utile pour recursion/debugging)
const factorial = function fact(n) {
    if (n <= 1) return 1;
    return n * fact(n - 1);  // Peut s'appeler elle-même
};

console.log(factorial(5));  // 120
// console.log(fact(5));    // ❌ ReferenceError (fact existe seulement dans la fonction)
```

**Quand utiliser quoi ?**
- **Function declaration** : fonctions globales, utilitaires, quand le hoisting est utile
- **Function expression** : callbacks, méthodes d'objets, quand on veut contrôler la portée

---

### Q2 : Quelles sont les différences entre les arrow functions et les fonctions classiques ?

**Réponse :**

Les **arrow functions** ont plusieurs différences importantes :

1. **Syntaxe plus concise**
2. **Pas de binding de `this`** (hérite du contexte parent)
3. **Pas d'objet `arguments`**
4. **Ne peuvent pas être constructeurs** (pas de `new`)
5. **Return implicite** pour expressions simples

```javascript
// 1. Syntaxe
// Fonction classique
function add(a, b) {
    return a + b;
}

// Arrow function
const add2 = (a, b) => a + b;

// 2. this (différence MAJEURE)
const obj = {
    name: "Alice",
    
    // Fonction classique : 'this' = obj
    greet: function() {
        console.log(`Hello, I'm ${this.name}`);
    },
    
    // Arrow function : 'this' hérite du contexte parent
    greetArrow: () => {
        console.log(`Hello, I'm ${this.name}`);  // undefined!
    },
    
    // Exemple avec setTimeout
    delayedGreet: function() {
        // ❌ Problème avec fonction classique
        setTimeout(function() {
            console.log(this.name);  // undefined (this = window/global)
        }, 100);
        
        // ✅ Solution avec arrow function
        setTimeout(() => {
            console.log(this.name);  // "Alice" (hérite du this parent)
        }, 100);
    }
};

obj.greet();        // "Hello, I'm Alice"
obj.greetArrow();   // "Hello, I'm undefined"
obj.delayedGreet();

// 3. arguments object
function classicFunc() {
    console.log(arguments);  // ✅ Existe
}

const arrowFunc = () => {
    // console.log(arguments);  // ❌ ReferenceError
};

classicFunc(1, 2, 3);  // Arguments[1, 2, 3]

// Solution avec rest parameters
const arrowWithRest = (...args) => {
    console.log(args);  // ✅ [1, 2, 3]
};

arrowWithRest(1, 2, 3);

// 4. Constructeur
function Person(name) {
    this.name = name;
}

const person1 = new Person("Alice");  // ✅ Fonctionne

const PersonArrow = (name) => {
    this.name = name;
};

// const person2 = new PersonArrow("Bob");  // ❌ TypeError: not a constructor

// 5. Return implicite
const square = x => x * x;  // Return implicite
console.log(square(5));  // 25

// ⚠️ Attention avec les objets
// const makeUser = name => { name: name };  // ❌ Interprété comme block

const makeUser = name => ({ name: name });  // ✅ Parenthèses obligatoires
console.log(makeUser("Alice"));  // { name: "Alice" }
```

**Quand utiliser arrow functions ?**
- ✅ Callbacks, array methods (map, filter, etc.)
- ✅ Fonctions courtes et concises
- ✅ Quand on veut hériter du `this` parent
- ❌ Méthodes d'objets (problème avec `this`)
- ❌ Event handlers qui ont besoin de `this`
- ❌ Constructeurs

---

### Q3 : Comment fonctionnent les paramètres par défaut ?

**Réponse :**

Les **default parameters** (ES6) permettent de définir des valeurs par défaut pour les paramètres. Ils s'activent si le paramètre est `undefined`, mais **pas** si c'est `null`.

```javascript
// Syntaxe de base
function greet(name = "Guest") {
    console.log(`Hello ${name}`);
}

greet("Alice");    // "Hello Alice"
greet();           // "Hello Guest"
greet(undefined);  // "Hello Guest" (undefined → défaut)
greet(null);       // "Hello null" ⚠️ (null n'active pas le défaut)
greet("");         // "Hello " ⚠️ (string vide n'active pas le défaut)

// Plusieurs paramètres par défaut
function createUser(name = "Anonymous", age = 0, role = "user") {
    return { name, age, role };
}

console.log(createUser());                    // { name: "Anonymous", age: 0, role: "user" }
console.log(createUser("Alice"));            // { name: "Alice", age: 0, role: "user" }
console.log(createUser("Bob", 25));          // { name: "Bob", age: 25, role: "user" }
console.log(createUser("Charlie", 30, "admin")); // { name: "Charlie", age: 30, role: "admin" }

// Valeur par défaut peut être une expression
function getTimestamp() {
    return Date.now();
}

function log(message, time = getTimestamp()) {
    console.log(`[${time}] ${message}`);
}

log("Hello");  // [1234567890] Hello

// Valeur par défaut peut utiliser les paramètres précédents
function calculatePrice(price, quantity = 1, tax = price * 0.2) {
    return price * quantity + tax;
}

console.log(calculatePrice(100));        // 120 (100 * 1 + 20)
console.log(calculatePrice(100, 2));     // 220 (100 * 2 + 20)
console.log(calculatePrice(100, 2, 10)); // 210 (100 * 2 + 10 custom)

// Avant ES6 (ancienne méthode)
function greetOld(name) {
    name = name || "Guest";  // ⚠️ Problème si name = "" ou 0
    console.log(`Hello ${name}`);
}

greetOld("");  // "Hello Guest" ⚠️ (string vide devient Guest)

// Avec nullish coalescing (ES2020) - pour null ET undefined
function greetModern(name) {
    name = name ?? "Guest";
    console.log(`Hello ${name}`);
}

greetModern(undefined);  // "Hello Guest"
greetModern(null);       // "Hello Guest"
greetModern("");         // "Hello " ✅
greetModern(0);          // "Hello 0" ✅
```

---

## Questions sur Concepts Avancés

### Q4 : Qu'est-ce qu'une closure et donnez un exemple pratique

**Réponse :**

Une **closure** est une fonction qui "se souvient" des variables de son environnement lexical (scope parent), même après que la fonction parent ait terminé son exécution.

```javascript
// Exemple simple
function outer() {
    const message = "Hello";  // Variable du scope parent
    
    function inner() {
        console.log(message);  // Accède à 'message' (closure)
    }
    
    return inner;
}

const myFunc = outer();  // outer() s'est terminée
myFunc();  // "Hello" ✅ inner "se souvient" de message

// Exemple pratique 1 : Compteur privé
function createCounter() {
    let count = 0;  // Variable privée
    
    return {
        increment: function() {
            count++;
            return count;
        },
        decrement: function() {
            count--;
            return count;
        },
        getCount: function() {
            return count;
        }
    };
}

const counter = createCounter();
console.log(counter.increment());  // 1
console.log(counter.increment());  // 2
console.log(counter.decrement());  // 1
console.log(counter.getCount());   // 1

// 'count' n'est pas accessible directement
console.log(counter.count);  // undefined ✅ (privé)

// Chaque appel à createCounter crée un nouveau scope
const counter2 = createCounter();
console.log(counter2.getCount());  // 0 (indépendant de counter)
console.log(counter.getCount());   // 1 (premier compteur inchangé)

// Exemple pratique 2 : Factory functions
function createMultiplier(multiplier) {
    return function(number) {
        return number * multiplier;  // Se souvient de 'multiplier'
    };
}

const double = createMultiplier(2);
const triple = createMultiplier(3);
const times10 = createMultiplier(10);

console.log(double(5));    // 10
console.log(triple(5));    // 15
console.log(times10(5));   // 50

// Exemple pratique 3 : Données privées (encapsulation)
function createBankAccount(initialBalance) {
    let balance = initialBalance;  // Privée
    
    return {
        deposit(amount) {
            if (amount > 0) {
                balance += amount;
                console.log(`Deposited ${amount}. New balance: ${balance}`);
            }
        },
        withdraw(amount) {
            if (amount > 0 && amount <= balance) {
                balance -= amount;
                console.log(`Withdrew ${amount}. New balance: ${balance}`);
            } else {
                console.log("Insufficient funds");
            }
        },
        getBalance() {
            return balance;
        }
    };
}

const account = createBankAccount(1000);
account.deposit(500);     // "Deposited 500. New balance: 1500"
account.withdraw(200);    // "Withdrew 200. New balance: 1300"
console.log(account.getBalance());  // 1300

// Impossible de modifier balance directement
account.balance = 999999;  // N'a aucun effet
console.log(account.getBalance());  // 1300 ✅

// Piège classique avec closures dans les boucles
// ❌ Avec var (bug)
for (var i = 0; i < 3; i++) {
    setTimeout(function() {
        console.log(i);  // 3, 3, 3 ⚠️
    }, 100);
}

// ✅ Avec let (résout le problème)
for (let j = 0; j < 3; j++) {
    setTimeout(function() {
        console.log(j);  // 0, 1, 2 ✅
    }, 100);
}

// ✅ Avec closure (solution avant ES6)
for (var k = 0; k < 3; k++) {
    (function(index) {
        setTimeout(function() {
            console.log(index);  // 0, 1, 2 ✅
        }, 100);
    })(k);
}
```

**Pourquoi les closures sont utiles ?**
- Encapsulation (données privées)
- Factory functions
- Event handlers avec contexte
- Callbacks avec état
- Module pattern

---

### Q5 : Expliquez la différence entre `map`, `filter` et `reduce`

**Réponse :**

Ces trois méthodes sont des **Higher-Order Functions** qui transforment des arrays :

- **`map`** : transforme chaque élément → retourne un **nouveau array de même longueur**
- **`filter`** : filtre selon condition → retourne un **nouveau array (≤ longueur originale)**
- **`reduce`** : réduit à une seule valeur → retourne **une valeur unique**

```javascript
const numbers = [1, 2, 3, 4, 5];

// MAP : transforme chaque élément
const doubled = numbers.map(num => num * 2);
console.log(doubled);  // [2, 4, 6, 8, 10]

// Array original inchangé
console.log(numbers);  // [1, 2, 3, 4, 5]

// map avec objets
const users = [
    { name: "Alice", age: 25 },
    { name: "Bob", age: 30 },
    { name: "Charlie", age: 35 }
];

const names = users.map(user => user.name);
console.log(names);  // ["Alice", "Bob", "Charlie"]

// map avec index
const withIndex = numbers.map((num, index) => `${index}: ${num}`);
console.log(withIndex);  // ["0: 1", "1: 2", "2: 3", "3: 4", "4: 5"]

// FILTER : garde seulement les éléments qui passent la condition
const evens = numbers.filter(num => num % 2 === 0);
console.log(evens);  // [2, 4]

const odds = numbers.filter(num => num % 2 !== 0);
console.log(odds);  // [1, 3, 5]

// filter avec objets
const adults = users.filter(user => user.age >= 30);
console.log(adults);  // [{ name: "Bob", age: 30 }, { name: "Charlie", age: 35 }]

// filter peut retourner array vide
const overHundred = numbers.filter(num => num > 100);
console.log(overHundred);  // []

// REDUCE : réduit à une seule valeur
// Syntaxe : reduce((accumulator, current) => ..., initialValue)

// Somme
const sum = numbers.reduce((acc, num) => acc + num, 0);
console.log(sum);  // 15

// Étapes de reduce pour la somme :
// acc=0, num=1 → return 0+1=1
// acc=1, num=2 → return 1+2=3
// acc=3, num=3 → return 3+3=6
// acc=6, num=4 → return 6+4=10
// acc=10, num=5 → return 10+5=15

// Produit
const product = numbers.reduce((acc, num) => acc * num, 1);
console.log(product);  // 120 (1*2*3*4*5)

// Maximum
const max = numbers.reduce((max, num) => num > max ? num : max);
console.log(max);  // 5

// Compter occurrences
const fruits = ["apple", "banana", "apple", "orange", "banana", "apple"];

const count = fruits.reduce((acc, fruit) => {
    acc[fruit] = (acc[fruit] || 0) + 1;
    return acc;
}, {});

console.log(count);  // { apple: 3, banana: 2, orange: 1 }

// Convertir array en objet
const usersArray = [
    { id: 1, name: "Alice" },
    { id: 2, name: "Bob" }
];

const usersById = usersArray.reduce((acc, user) => {
    acc[user.id] = user;
    return acc;
}, {});

console.log(usersById);
// { 1: { id: 1, name: "Alice" }, 2: { id: 2, name: "Bob" } }

// CHAÎNAGE : combiner map, filter, reduce
const result = numbers
    .filter(num => num % 2 === 0)  // [2, 4]
    .map(num => num * num)          // [4, 16]
    .reduce((sum, num) => sum + num, 0);  // 20

console.log(result);  // 20

// Exemple pratique : traitement de commandes
const orders = [
    { id: 1, amount: 100, status: "paid" },
    { id: 2, amount: 200, status: "paid" },
    { id: 3, amount: 150, status: "pending" },
    { id: 4, amount: 300, status: "paid" }
];

// Total des commandes payées
const totalPaid = orders
    .filter(order => order.status === "paid")
    .reduce((sum, order) => sum + order.amount, 0);

console.log(totalPaid);  // 600
```

**Résumé :**
- **`map(fn)`** : transforme → `[1, 2, 3]` → `[2, 4, 6]` (même longueur)
- **`filter(fn)`** : filtre → `[1, 2, 3, 4]` → `[2, 4]` (≤ longueur)
- **`reduce(fn, init)`** : réduit → `[1, 2, 3, 4, 5]` → `15` (une valeur)

---

### Q6 : Qu'est-ce qu'une pure function et pourquoi c'est important ?

**Réponse :**

Une **pure function** est une fonction qui respecte deux règles :

1. **Déterministe** : même input → toujours même output
2. **Pas de side effects** : ne modifie rien en dehors d'elle-même (pas de mutation, pas d'I/O)

```javascript
// ✅ PURE FUNCTION
function add(a, b) {
    return a + b;
}

console.log(add(2, 3));  // Toujours 5
console.log(add(2, 3));  // Toujours 5

// ✅ PURE
function double(arr) {
    return arr.map(x => x * 2);  // Nouveau array
}

const nums = [1, 2, 3];
const result1 = double(nums);
console.log(nums);     // [1, 2, 3] ✅ Original inchangé
console.log(result1);  // [2, 4, 6]

// ❌ IMPURE : dépend d'une variable externe
let discount = 0.1;

function applyDiscount(price) {
    return price * (1 - discount);  // Dépend de 'discount'
}

console.log(applyDiscount(100));  // 90
discount = 0.2;
console.log(applyDiscount(100));  // 80 ⚠️ Résultat différent!

// ✅ PURE : tout en paramètres
function applyDiscountPure(price, discountRate) {
    return price * (1 - discountRate);
}

console.log(applyDiscountPure(100, 0.1));  // Toujours 90
console.log(applyDiscountPure(100, 0.1));  // Toujours 90

// ❌ IMPURE : modifie l'argument (mutation)
function addItem(arr, item) {
    arr.push(item);  // Modifie l'array original!
    return arr;
}

const items = [1, 2, 3];
addItem(items, 4);
console.log(items);  // [1, 2, 3, 4] ⚠️ Original modifié

// ✅ PURE : retourne nouveau array
function addItemPure(arr, item) {
    return [...arr, item];  // Spread operator
}

const items2 = [1, 2, 3];
const newItems = addItemPure(items2, 4);
console.log(items2);    // [1, 2, 3] ✅ Original intact
console.log(newItems);  // [1, 2, 3, 4] ✅ Nouveau

// ❌ IMPURE : side effects (console, DOM, etc.)
function logAndDouble(x) {
    console.log(x);  // Side effect!
    return x * 2;
}

// ❌ IMPURE : non déterministe
function getRandomPrice() {
    return Math.random() * 100;  // Résultat différent à chaque fois
}

// Exemple pratique : gestion de state
const user = {
    name: "Alice",
    age: 25,
    address: {
        city: "Paris"
    }
};

// ❌ IMPURE : mutation
function updateAge(user, newAge) {
    user.age = newAge;  // Modifie l'objet original
    return user;
}

// ✅ PURE : immutabilité
function updateAgePure(user, newAge) {
    return {
        ...user,
        age: newAge
    };
}

const updatedUser = updateAgePure(user, 27);
console.log(user.age);         // 25 ✅ Original intact
console.log(updatedUser.age);  // 27 ✅ Nouveau objet

// ✅ PURE : nested object update
function updateCity(user, newCity) {
    return {
        ...user,
        address: {
            ...user.address,
            city: newCity
        }
    };
}
```

**Avantages des pure functions :**
- Plus facile à tester
- Plus facile à comprendre
- Plus facile à maintenir
- Moins de bugs
- Peut être optimisé (memoization)
- Peut être parallélisé

---

### Q7 : Qu'est-ce qu'un callback et donnez des exemples d'utilisation

**Réponse :**

Un **callback** est une fonction passée en argument à une autre fonction, qui sera appelée (called back) plus tard. C'est un pattern fondamental pour la programmation asynchrone et les événements.

```javascript
// Exemple simple
function processData(data, callback) {
    const result = data * 2;
    callback(result);  // Appel du callback
}

processData(5, function(result) {
    console.log("Result:", result);  // "Result: 10"
});

// Avec arrow function
processData(10, result => console.log("Result:", result));  // "Result: 20"

// Cas d'usage 1 : Array methods
const numbers = [1, 2, 3, 4, 5];

// forEach : callback pour chaque élément
numbers.forEach(function(num) {
    console.log(num);
});

// map : callback transforme chaque élément
const doubled = numbers.map(function(num) {
    return num * 2;
});
console.log(doubled);  // [2, 4, 6, 8, 10]

// filter : callback teste chaque élément
const evens = numbers.filter(function(num) {
    return num % 2 === 0;
});
console.log(evens);  // [2, 4]

// Cas d'usage 2 : Timers (asynchrone)
console.log("Start");

setTimeout(function() {
    console.log("After 2 seconds");
}, 2000);

console.log("End");

// Affiche :
// "Start"
// "End"
// "After 2 seconds" (après 2 secondes)

// Cas d'usage 3 : Callbacks multiples (success/error)
function fetchData(url, onSuccess, onError) {
    // Simuler une requête
    const success = Math.random() > 0.5;
    
    setTimeout(function() {
        if (success) {
            onSuccess({ data: "Some data from " + url });
        } else {
            onError("Failed to fetch " + url);
        }
    }, 1000);
}

fetchData(
    "https://api.example.com/data",
    function(response) {
        console.log("Success:", response);
    },
    function(error) {
        console.log("Error:", error);
    }
);

// Problème : Callback Hell (pyramide de la mort)
// ❌ Code difficile à lire et maintenir
getData(function(a) {
    getMoreData(a, function(b) {
        getMoreData(b, function(c) {
            getMoreData(c, function(d) {
                console.log(d);
                // 😱 Trop imbriqué!
            });
        });
    });
});

// Solution moderne : Promises et async/await (on verra plus tard)
```

**Types de callbacks :**
1. **Synchrone** : exécuté immédiatement (array methods)
2. **Asynchrone** : exécuté plus tard (setTimeout, events, AJAX)

---

## Questions Pratiques / Pièges

### Q8 : Que va afficher ce code ?

```javascript
function outer() {
    var x = 10;
    
    return function inner() {
        console.log(x);
    };
}

const fn = outer();
fn();
```

**Réponse :**

```javascript
// Affiche : 10

// Explication : CLOSURE
// inner() "se souvient" de la variable x de son scope parent
// Même après que outer() ait terminé son exécution

function outer() {
    var x = 10;  // Variable du scope parent
    
    return function inner() {
        console.log(x);  // Accède à x via closure
    };
}

const fn = outer();  // outer() s'exécute et retourne inner
// outer() a terminé, mais x est toujours accessible
fn();  // 10 ✅
```

---

### Q9 : Que va afficher ce code ?

```javascript
const arr = [1, 2, 3];

const result = arr.map(num => {
    num * 2;
});

console.log(result);
```

**Réponse :**

```javascript
// Affiche : [undefined, undefined, undefined]

// Problème : pas de return!
const result = arr.map(num => {
    num * 2;  // ❌ Pas de return, donc undefined
});

// Solutions :
// 1. Return explicite
const result1 = arr.map(num => {
    return num * 2;  // ✅
});

// 2. Return implicite (sans accolades)
const result2 = arr.map(num => num * 2);  // ✅

console.log(result1);  // [2, 4, 6]
console.log(result2);  // [2, 4, 6]
```

---

### Q10 : Que va afficher ce code ?

```javascript
for (var i = 0; i < 3; i++) {
    setTimeout(() => console.log(i), 0);
}

for (let j = 0; j < 3; j++) {
    setTimeout(() => console.log(j), 0);
}
```

**Réponse :**

```javascript
// Premier loop (var) : 3, 3, 3
// Deuxième loop (let) : 0, 1, 2

// Explication :

// var : function scope
// Quand les setTimeout s'exécutent, la boucle est déjà terminée
// i vaut 3 pour toutes les closures
for (var i = 0; i < 3; i++) {
    setTimeout(() => console.log(i), 0);  // 3, 3, 3
}

// let : block scope
// Chaque itération a son propre j
// Les closures capturent des j différents
for (let j = 0; j < 3; j++) {
    setTimeout(() => console.log(j), 0);  // 0, 1, 2
}

// Solution avec var (avant ES6) : IIFE
for (var k = 0; k < 3; k++) {
    (function(index) {
        setTimeout(() => console.log(index), 0);  // 0, 1, 2
    })(k);
}
```

---

**Résumé Chapitre 3 :**

✅ **Function declaration** vs **expression** : hoisting  
✅ **Arrow functions** : syntaxe concise, pas de `this`, return implicite  
✅ **Default parameters** : valeurs par défaut (undefined → défaut, null non)  
✅ **Closures** : fonction qui se souvient de son environnement  
✅ **Callbacks** : fonction passée en argument  
✅ **Higher-Order Functions** : `map`, `filter`, `reduce`, `forEach`  
✅ **Pure functions** : déterministe + pas de side effects  

---
