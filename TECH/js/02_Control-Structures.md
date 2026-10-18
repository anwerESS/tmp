## [**..**](./00_index.md)

# Chapitre 2 : Structures de Contrôle

## 2.1 Conditions

Les structures conditionnelles permettent d'exécuter du code en fonction de conditions spécifiques.

### if, else if, else

```javascript
// Structure de base
const age = 18;

if (age >= 18) {
    console.log("Adult");
}

// if...else
const temperature = 15;

if (temperature > 25) {
    console.log("Hot");
} else {
    console.log("Not hot");
}

// if...else if...else
const score = 85;

if (score >= 90) {
    console.log("Grade A");
} else if (score >= 80) {
    console.log("Grade B");
} else if (score >= 70) {
    console.log("Grade C");
} else if (score >= 60) {
    console.log("Grade D");
} else {
    console.log("Grade F");
}

// Conditions multiples avec opérateurs logiques
const username = "Alice";
const password = "secret123";

if (username === "Alice" && password === "secret123") {
    console.log("Login successful");
} else {
    console.log("Invalid credentials");
}

// Conditions avec OR
const role = "admin";
const hasPermission = true;

if (role === "admin" || hasPermission) {
    console.log("Access granted");
}

// Nested conditions (imbriquées)
const isLoggedIn = true;
const isPremium = false;

if (isLoggedIn) {
    if (isPremium) {
        console.log("Welcome Premium User");
    } else {
        console.log("Welcome Free User");
    }
} else {
    console.log("Please login");
}

// Éviter les nested conditions avec early return
function checkAccess(user) {
    // ❌ Mauvais (trop imbriqué)
    if (user) {
        if (user.isActive) {
            if (user.hasPermission) {
                return "Access granted";
            }
        }
    }
    return "Access denied";
}

function checkAccessBetter(user) {
    // ✅ Bon (early return)
    if (!user) return "Access denied";
    if (!user.isActive) return "Access denied";
    if (!user.hasPermission) return "Access denied";
    
    return "Access granted";
}

// Conditions avec optional chaining
const user = null;

if (user?.isActive) {
    console.log("User is active");
}

// Sans block (une seule instruction) - à éviter
if (age >= 18) console.log("Adult");  // ⚠️ Fonctionne mais déconseillé

// Toujours utiliser des accolades (best practice)
if (age >= 18) {
    console.log("Adult");  // ✅ Meilleure lisibilité
}
```

### switch Statement

```javascript
// switch : utile pour comparer une valeur à plusieurs cas
const day = 3;

switch (day) {
    case 1:
        console.log("Monday");
        break;  // Important! Sans break, continue au cas suivant
    case 2:
        console.log("Tuesday");
        break;
    case 3:
        console.log("Wednesday");
        break;
    case 4:
        console.log("Thursday");
        break;
    case 5:
        console.log("Friday");
        break;
    case 6:
    case 7:  // Grouper plusieurs cas
        console.log("Weekend");
        break;
    default:  // Cas par défaut (optionnel)
        console.log("Invalid day");
}

// Sans break (fall-through) - attention!
const month = 2;

switch (month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        console.log("31 days");
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        console.log("30 days");
        break;
    case 2:
        console.log("28 or 29 days");
        break;
}

// switch avec des strings
const action = "save";

switch (action) {
    case "save":
        console.log("Saving document...");
        // saveDocument();
        break;
    case "load":
        console.log("Loading document...");
        // loadDocument();
        break;
    case "delete":
        console.log("Deleting document...");
        // deleteDocument();
        break;
    default:
        console.log("Unknown action");
}

// switch avec return (pas besoin de break)
function getDayName(dayNumber) {
    switch (dayNumber) {
        case 1: return "Monday";
        case 2: return "Tuesday";
        case 3: return "Wednesday";
        case 4: return "Thursday";
        case 5: return "Friday";
        case 6: return "Saturday";
        case 7: return "Sunday";
        default: return "Invalid day";
    }
}

console.log(getDayName(3));  // "Wednesday"

// switch vs if/else if
// ✅ switch : bon pour comparer UNE variable à plusieurs valeurs
// ✅ if/else : bon pour conditions complexes ou différentes variables

// Exemple où if/else est mieux
const age2 = 25;
const hasLicense = true;

// ❌ switch pas adapté ici
if (age2 >= 18 && hasLicense) {
    console.log("Can drive");
} else {
    console.log("Cannot drive");
}
```

### Truthy et Falsy Values

```javascript
// Les 6 valeurs falsy (rappel)
// false, 0, "", null, undefined, NaN

// Utilisation dans les conditions
const value1 = "";

if (value1) {
    console.log("This won't run");  // "" est falsy
} else {
    console.log("Empty string is falsy");  // ✅ S'exécute
}

// Truthy values dans les conditions
const value2 = "hello";

if (value2) {
    console.log("Non-empty string is truthy");  // ✅ S'exécute
}

// Vérifications courantes
const userInput = "";

// ❌ Peut être problématique si 0 ou false sont valides
if (userInput) {
    console.log("Input provided");
}

// ✅ Plus précis selon le contexte
if (userInput !== "") {
    console.log("Input provided");
}

// Vérifier undefined/null spécifiquement
const data = null;

if (data === null || data === undefined) {
    console.log("No data");
}

// Ou plus court avec nullish coalescing
const displayValue = data ?? "No data available";
console.log(displayValue);

// Arrays et objets vides sont truthy!
const emptyArray = [];
const emptyObject = {};

if (emptyArray) {
    console.log("Empty array is truthy!");  // ✅ S'exécute
}

// Pour vérifier si un array est vraiment vide
if (emptyArray.length === 0) {
    console.log("Array is empty");
}

// Pour vérifier si un objet est vide
if (Object.keys(emptyObject).length === 0) {
    console.log("Object is empty");
}

// Conversion explicite en boolean
const value3 = "0";
console.log(Boolean(value3));  // true (string non vide)

if (!!value3) {  // Double négation = conversion en boolean
    console.log("Truthy");
}

// Pièges courants
console.log(Boolean("false"));  // true ⚠️ (string non vide)
console.log(Boolean("0"));      // true ⚠️ (string non vide)
console.log(Boolean([]));       // true ⚠️ (array vide)
console.log(Boolean({}));       // true ⚠️ (objet vide)
```

---

## 2.2 Boucles

Les boucles permettent d'exécuter du code de manière répétitive.

### for Loop

```javascript
// Structure de base : for (initialisation; condition; incrémentation)
for (let i = 0; i < 5; i++) {
    console.log(i);  // 0, 1, 2, 3, 4
}

// Décrémentation
for (let i = 5; i > 0; i--) {
    console.log(i);  // 5, 4, 3, 2, 1
}

// Incrément différent
for (let i = 0; i < 10; i += 2) {
    console.log(i);  // 0, 2, 4, 6, 8
}

// Parcourir un array
const fruits = ["apple", "banana", "orange"];

for (let i = 0; i < fruits.length; i++) {
    console.log(fruits[i]);
}

// Index en ordre inverse
for (let i = fruits.length - 1; i >= 0; i--) {
    console.log(fruits[i]);  // "orange", "banana", "apple"
}

// Boucles imbriquées (nested loops)
for (let i = 1; i <= 3; i++) {
    for (let j = 1; j <= 3; j++) {
        console.log(`i: ${i}, j: ${j}`);
    }
}
// Affiche : i:1,j:1  i:1,j:2  i:1,j:3  i:2,j:1  etc.

// Créer un tableau 2D (matrix)
const matrix = [];
for (let i = 0; i < 3; i++) {
    matrix[i] = [];
    for (let j = 0; j < 3; j++) {
        matrix[i][j] = i * 3 + j;
    }
}
console.log(matrix);  // [[0,1,2], [3,4,5], [6,7,8]]

// Variables multiples dans for
for (let i = 0, j = 10; i < 5; i++, j--) {
    console.log(`i: ${i}, j: ${j}`);
}

// for sans certaines parties (déconseillé mais possible)
let k = 0;
for (; k < 3;) {
    console.log(k);
    k++;
}

// Boucle infinie (attention!)
// for (;;) {
//     console.log("Infinite loop");
//     // Besoin de break pour sortir
// }
```

### while Loop

```javascript
// while : continue tant que la condition est true
let count = 0;

while (count < 5) {
    console.log(count);
    count++;
}

// while avec array
const numbers = [1, 2, 3, 4, 5];
let index = 0;

while (index < numbers.length) {
    console.log(numbers[index]);
    index++;
}

// while avec condition complexe
let attempts = 0;
let success = false;

while (attempts < 3 && !success) {
    console.log(`Attempt ${attempts + 1}`);
    // Simuler tentative
    success = Math.random() > 0.5;
    attempts++;
}

// while true avec break (pattern courant)
let input = 0;
while (true) {
    console.log(input);
    input++;
    
    if (input >= 5) {
        break;  // Sort de la boucle
    }
}

// Lecture de données jusqu'à trouver une valeur
const data = [10, 20, 30, 40, 50];
let i = 0;
let found = false;

while (i < data.length && !found) {
    if (data[i] === 30) {
        console.log(`Found 30 at index ${i}`);
        found = true;
    }
    i++;
}
```

### do...while Loop

```javascript
// do...while : exécute AU MOINS UNE FOIS, puis vérifie la condition
let counter = 0;

do {
    console.log(counter);
    counter++;
} while (counter < 5);

// Différence avec while : s'exécute même si condition initialement false
let x = 10;

do {
    console.log("Executed once!");  // ✅ S'exécute
    x++;
} while (x < 5);  // Condition false, mais une exécution a eu lieu

// Comparaison : while ne s'exécuterait pas
let y = 10;
while (y < 5) {
    console.log("Never executes");  // ❌ Ne s'exécute pas
    y++;
}

// Cas d'usage typique : menu/validation
let userChoice;
do {
    // Afficher menu
    console.log("1. Option A");
    console.log("2. Option B");
    console.log("3. Exit");
    
    // Simuler entrée utilisateur
    userChoice = Math.floor(Math.random() * 4);
    console.log(`User selected: ${userChoice}`);
    
} while (userChoice !== 3);  // Répète jusqu'à Exit

// Validation d'input
let password;
do {
    // password = prompt("Enter password (min 6 chars):");
    password = "test123";  // Simulation
    console.log("Password entered");
} while (password.length < 6);
```

### for...in Loop (pour objets)

```javascript
// for...in : itère sur les CLÉS d'un objet (ou indices d'array)
const person = {
    name: "John",
    age: 30,
    city: "Paris"
};

for (let key in person) {
    console.log(`${key}: ${person[key]}`);
}
// Affiche :
// name: John
// age: 30
// city: Paris

// Avec hasOwnProperty (bonne pratique)
for (let key in person) {
    if (person.hasOwnProperty(key)) {  // Ignore les propriétés héritées
        console.log(`${key}: ${person[key]}`);
    }
}

// for...in avec array (déconseillé, utiliser for...of à la place)
const colors = ["red", "green", "blue"];

for (let index in colors) {
    console.log(index);         // "0", "1", "2" (strings!)
    console.log(colors[index]); // "red", "green", "blue"
}

// ⚠️ Problème : index est un string avec for...in
for (let i in colors) {
    console.log(typeof i);  // "string"
    console.log(i + 1);     // "01" au lieu de 1 ⚠️
}

// Énumérer les propriétés d'un objet
const car = {
    brand: "Toyota",
    model: "Corolla",
    year: 2020
};

const keys = [];
for (let prop in car) {
    keys.push(prop);
}
console.log(keys);  // ["brand", "model", "year"]

// Alternative moderne : Object.keys()
console.log(Object.keys(car));  // ["brand", "model", "year"]
console.log(Object.values(car)); // ["Toyota", "Corolla", 2020]
console.log(Object.entries(car)); // [["brand","Toyota"], ["model","Corolla"], ["year",2020]]
```

### for...of Loop (pour itérables)

```javascript
// for...of : itère sur les VALEURS d'un iterable (arrays, strings, etc.)
const fruits2 = ["apple", "banana", "orange"];

for (let fruit of fruits2) {
    console.log(fruit);  // "apple", "banana", "orange"
}

// Différence avec for...in
const numbers2 = [10, 20, 30];

// for...in donne les indices (clés)
for (let index in numbers2) {
    console.log(index);  // "0", "1", "2" (strings)
}

// for...of donne les valeurs
for (let value of numbers2) {
    console.log(value);  // 10, 20, 30 (numbers)
}

// for...of avec string (itère sur les caractères)
const text = "Hello";

for (let char of text) {
    console.log(char);  // "H", "e", "l", "l", "o"
}

// for...of avec Set
const uniqueNumbers = new Set([1, 2, 3, 3, 4]);

for (let num of uniqueNumbers) {
    console.log(num);  // 1, 2, 3, 4 (pas de doublons)
}

// for...of avec Map
const userRoles = new Map([
    ["Alice", "admin"],
    ["Bob", "user"],
    ["Charlie", "moderator"]
]);

for (let [name, role] of userRoles) {
    console.log(`${name}: ${role}`);
}

// Obtenir index avec entries()
const animals = ["dog", "cat", "bird"];

for (let [index, animal] of animals.entries()) {
    console.log(`${index}: ${animal}`);
}
// Affiche : 0: dog, 1: cat, 2: bird

// ❌ for...of ne fonctionne PAS avec les objets normaux
const obj = { a: 1, b: 2 };
// for (let value of obj) { }  // TypeError: obj is not iterable

// ✅ Solutions pour objets :
// 1. Utiliser Object.keys()
for (let key of Object.keys(obj)) {
    console.log(`${key}: ${obj[key]}`);
}

// 2. Utiliser Object.values()
for (let value of Object.values(obj)) {
    console.log(value);
}

// 3. Utiliser Object.entries()
for (let [key, value] of Object.entries(obj)) {
    console.log(`${key}: ${value}`);
}

// for...of avec NodeList (DOM)
// const elements = document.querySelectorAll('.item');
// for (let element of elements) {
//     console.log(element.textContent);
// }
```

### break et continue

```javascript
// break : sort complètement de la boucle
for (let i = 0; i < 10; i++) {
    if (i === 5) {
        break;  // Sort de la boucle
    }
    console.log(i);  // 0, 1, 2, 3, 4
}

// continue : passe à l'itération suivante
for (let i = 0; i < 10; i++) {
    if (i % 2 === 0) {
        continue;  // Saute les nombres pairs
    }
    console.log(i);  // 1, 3, 5, 7, 9
}

// break dans while
let j = 0;
while (true) {
    console.log(j);
    j++;
    
    if (j >= 5) {
        break;  // Sort de la boucle infinie
    }
}

// Chercher une valeur dans un array
const numbers3 = [5, 10, 15, 20, 25];
let target = 15;
let foundIndex = -1;

for (let i = 0; i < numbers3.length; i++) {
    if (numbers3[i] === target) {
        foundIndex = i;
        break;  // Pas besoin de continuer
    }
}
console.log(`Found at index: ${foundIndex}`);

// continue pour filtrer
const scores = [45, 78, 92, 55, 88, 33, 95];

console.log("Scores above 70:");
for (let score of scores) {
    if (score <= 70) {
        continue;  // Ignore les scores <= 70
    }
    console.log(score);  // 78, 92, 88, 95
}

// break dans boucles imbriquées (sort seulement de la boucle interne)
for (let i = 0; i < 3; i++) {
    console.log(`Outer: ${i}`);
    
    for (let j = 0; j < 3; j++) {
        if (j === 1) {
            break;  // Sort seulement de la boucle interne
        }
        console.log(`  Inner: ${j}`);
    }
}

// Labels pour sortir de plusieurs boucles
outerLoop: for (let i = 0; i < 3; i++) {
    for (let j = 0; j < 3; j++) {
        if (i === 1 && j === 1) {
            break outerLoop;  // Sort de TOUTES les boucles
        }
        console.log(`i: ${i}, j: ${j}`);
    }
}

// continue avec label
outer: for (let i = 0; i < 3; i++) {
    for (let j = 0; j < 3; j++) {
        if (j === 1) {
            continue outer;  // Continue la boucle externe
        }
        console.log(`i: ${i}, j: ${j}`);
    }
}

// Validation avec continue
const usernames = ["alice", "b", "charlie", "", "david"];

for (let username of usernames) {
    // Sauter les usernames invalides
    if (!username || username.length < 3) {
        continue;
    }
    
    console.log(`Valid username: ${username}`);
}
// Affiche : alice, charlie, david

// break dans switch (déjà vu)
const action2 = "save";
switch (action2) {
    case "save":
        console.log("Saving...");
        break;  // Important!
    case "load":
        console.log("Loading...");
        break;
}
```

---

## Comparaison des Boucles

```javascript
const data2 = [10, 20, 30, 40, 50];

// 1. for classique : contrôle total, index disponible
for (let i = 0; i < data2.length; i++) {
    console.log(i, data2[i]);
}

// 2. while : condition complexe, nombre d'itérations inconnu
let i2 = 0;
while (i2 < data2.length) {
    console.log(data2[i2]);
    i2++;
}

// 3. do...while : au moins une exécution garantie
let i3 = 0;
do {
    console.log(data2[i3]);
    i3++;
} while (i3 < data2.length);

// 4. for...in : clés d'objet (ou indices d'array mais déconseillé)
for (let index in data2) {
    console.log(index, data2[index]);  // index est string!
}

// 5. for...of : valeurs d'iterable (RECOMMANDÉ pour arrays)
for (let value of data2) {
    console.log(value);
}

// 6. forEach (méthode array, pas une structure de contrôle)
data2.forEach((value, index) => {
    console.log(index, value);
});

// Quand utiliser quoi ?
// ✅ for...of : parcourir arrays, strings, Sets, Maps (le plus lisible)
// ✅ for classique : besoin de l'index, modification de l'index
// ✅ for...in : parcourir les clés d'un objet
// ✅ while : condition complexe, nombre d'itérations inconnu
// ✅ do...while : au moins une exécution nécessaire

// Exemples pratiques
const users = [
    { id: 1, name: "Alice", active: true },
    { id: 2, name: "Bob", active: false },
    { id: 3, name: "Charlie", active: true }
];

// for...of pour itérer sur les valeurs
console.log("Active users:");
for (let user of users) {
    if (user.active) {
        console.log(user.name);
    }
}

// for classique si on a besoin de l'index
console.log("Users with index:");
for (let i = 0; i < users.length; i++) {
    console.log(`${i + 1}. ${users[i].name}`);
}

// for...in pour objets
const settings = {
    theme: "dark",
    language: "fr",
    notifications: true
};

console.log("Settings:");
for (let key in settings) {
    console.log(`${key}: ${settings[key]}`);
}
```

---

**Résumé du Chapitre 2 :**

✅ **if/else** : conditions simples et complexes, éviter trop d'imbrication  
✅ **switch** : comparer une valeur à plusieurs cas (penser aux `break`)  
✅ Connaître les **6 valeurs falsy** et leurs pièges  
✅ **for** : boucle classique avec contrôle total  
✅ **while/do...while** : quand le nombre d'itérations est inconnu  
✅ **for...in** : clés d'objets (éviter pour arrays)  
✅ **for...of** : valeurs d'iterables (préféré pour arrays)  
✅ **break** : sortir d'une boucle, **continue** : passer à l'itération suivante  

---

# Questions/Réponses Orales - Chapitre 2 : Structures de Contrôle

## Questions sur Conditions

### Q1 : Quelle est la différence entre `if/else` et le ternary operator ?

**Réponse :**

Le **ternary operator** (`? :`) est une version condensée de `if/else` qui retourne une valeur. Il est idéal pour les affectations simples, tandis que `if/else` est mieux pour la logique complexe.

```javascript
const age = 18;

// if/else classique
let status;
if (age >= 18) {
    status = "adult";
} else {
    status = "minor";
}

// Ternary operator (plus concis)
const status2 = age >= 18 ? "adult" : "minor";

// ✅ Bon usage du ternary : affectations simples
const discount = isPremium ? 20 : 10;
const greeting = isLoggedIn ? "Welcome back" : "Please login";

// ❌ Mauvais usage : logique complexe (difficile à lire)
const result = score > 90 ? "A" : 
               score > 80 ? "B" : 
               score > 70 ? "C" : 
               score > 60 ? "D" : "F";

// ✅ Mieux avec if/else pour la complexité
let grade;
if (score > 90) {
    grade = "A";
} else if (score > 80) {
    grade = "B";
} else if (score > 70) {
    grade = "C";
} else if (score > 60) {
    grade = "D";
} else {
    grade = "F";
}

// Ternary dans JSX/React (très courant)
// return <div>{isLoading ? <Spinner /> : <Content />}</div>;
```

**Règle :** Utiliser le ternary pour des conditions simples sur une ligne, `if/else` pour tout le reste.

---

### Q2 : Quand utiliser `switch` au lieu de `if/else if` ?

**Réponse :**

Utiliser `switch` quand on compare **une seule variable** à **plusieurs valeurs fixes**. Utiliser `if/else if` pour des **conditions complexes** ou **différentes variables**.

```javascript
// ✅ switch : bon usage (une variable, plusieurs valeurs)
const dayNumber = 3;

switch (dayNumber) {
    case 1:
        console.log("Monday");
        break;
    case 2:
        console.log("Tuesday");
        break;
    case 3:
        console.log("Wednesday");
        break;
    case 4:
        console.log("Thursday");
        break;
    case 5:
        console.log("Friday");
        break;
    case 6:
    case 7:  // Grouper plusieurs cas
        console.log("Weekend");
        break;
    default:
        console.log("Invalid day");
}

// ❌ switch : mauvais usage (conditions complexes)
const age = 25;
const hasLicense = true;

// On ne peut pas faire ça proprement avec switch
if (age >= 18 && hasLicense) {
    console.log("Can drive");
} else if (age >= 18 && !hasLicense) {
    console.log("Need license");
} else {
    console.log("Too young");
}

// ✅ switch : grouper plusieurs cas
const month = 2;

switch (month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        console.log("31 days");
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        console.log("30 days");
        break;
    case 2:
        console.log("28 or 29 days");
        break;
}

// Alternative moderne : object lookup (parfois plus lisible)
const daysInMonth = {
    1: 31, 2: 28, 3: 31, 4: 30,
    5: 31, 6: 30, 7: 31, 8: 31,
    9: 30, 10: 31, 11: 30, 12: 31
};

console.log(daysInMonth[month]);  // Plus concis!
```

**Avantages de switch :**
- Plus lisible pour beaucoup de cas
- Intention claire : on teste une seule variable
- Peut être plus performant (optimisation du compilateur)

**Inconvénients :**
- Facile d'oublier `break` (fall-through non désiré)
- Ne supporte pas les conditions complexes

---

### Q3 : Que se passe-t-il si on oublie le `break` dans un `switch` ?

**Réponse :**

Sans `break`, JavaScript continue d'exécuter les cas suivants jusqu'à rencontrer un `break` ou la fin du switch. C'est ce qu'on appelle le **fall-through**.

```javascript
const status = "pending";

// ❌ Sans break (bug probable)
switch (status) {
    case "pending":
        console.log("Order pending");
        // Oubli du break!
    case "processing":
        console.log("Order processing");
        // Oubli du break!
    case "shipped":
        console.log("Order shipped");
        break;
    case "delivered":
        console.log("Order delivered");
        break;
}

// Affiche :
// "Order pending"
// "Order processing"  ⚠️ Non désiré!
// "Order shipped"     ⚠️ Non désiré!

// ✅ Avec break (correct)
switch (status) {
    case "pending":
        console.log("Order pending");
        break;  // ✅
    case "processing":
        console.log("Order processing");
        break;  // ✅
    case "shipped":
        console.log("Order shipped");
        break;  // ✅
    case "delivered":
        console.log("Order delivered");
        break;  // ✅
}

// ✅ Fall-through intentionnel (utile parfois)
const grade = "B";

switch (grade) {
    case "A":
    case "B":
    case "C":
        console.log("Pass");  // Exécuté pour A, B, ou C
        break;
    case "D":
    case "F":
        console.log("Fail");
        break;
}

// Alternative avec return (pas besoin de break)
function getStatusMessage(status) {
    switch (status) {
        case "pending": return "Order pending";
        case "processing": return "Order processing";
        case "shipped": return "Order shipped";
        default: return "Unknown status";
    }
    // Pas besoin de break car return sort de la fonction
}
```

---

### Q4 : Expliquez le concept de "truthy" et "falsy"

**Réponse :**

En JavaScript, toutes les valeurs peuvent être converties en boolean. Il y a **6 valeurs falsy** (deviennent `false`), tout le reste est **truthy** (devient `true`).

**Les 6 valeurs falsy :**

```javascript
// 1. false
if (false) { }  // Ne s'exécute pas

// 2. 0 (zéro)
if (0) { }  // Ne s'exécute pas

// 3. "" (string vide)
if ("") { }  // Ne s'exécute pas

// 4. null
if (null) { }  // Ne s'exécute pas

// 5. undefined
if (undefined) { }  // Ne s'exécute pas

// 6. NaN
if (NaN) { }  // Ne s'exécute pas
```

**TOUT LE RESTE est truthy (pièges courants) :**

```javascript
// ⚠️ Strings non vides (même "0" et "false")
if ("0") {
    console.log("String '0' is truthy!");  // ✅ S'exécute
}

if ("false") {
    console.log("String 'false' is truthy!");  // ✅ S'exécute
}

// ⚠️ Arrays et objets vides
if ([]) {
    console.log("Empty array is truthy!");  // ✅ S'exécute
}

if ({}) {
    console.log("Empty object is truthy!");  // ✅ S'exécute
}

// ⚠️ Nombres négatifs
if (-1) {
    console.log("-1 is truthy!");  // ✅ S'exécute
}

// ⚠️ Infinity
if (Infinity) {
    console.log("Infinity is truthy!");  // ✅ S'exécute
}

// Usage pratique : vérifications
function greet(name) {
    // ❌ Problème si name = "" (falsy)
    if (name) {
        return `Hello ${name}`;
    }
    return "Hello Guest";
}

// ✅ Plus précis
function greetBetter(name) {
    if (name !== undefined && name !== null && name !== "") {
        return `Hello ${name}`;
    }
    return "Hello Guest";
}

// ✅ Ou avec nullish coalescing
function greetBest(name) {
    const displayName = name ?? "Guest";
    return `Hello ${displayName}`;
}

// Vérifier array vide
const items = [];
if (items) {
    console.log("items exists (but might be empty)");
}
if (items.length > 0) {
    console.log("items has elements");  // ✅ Meilleure vérification
}

// Vérifier objet vide
const user = {};
if (user) {
    console.log("user exists (but might be empty)");
}
if (Object.keys(user).length > 0) {
    console.log("user has properties");  // ✅ Meilleure vérification
}
```

---

## Questions sur Boucles

### Q5 : Quelle est la différence entre `for...in` et `for...of` ?

**Réponse :**

`for...in` itère sur les **clés (keys)** d'un objet ou les **indices** d'un array (retourne des strings).

`for...of` itère sur les **valeurs** d'un iterable comme arrays, strings, Sets, Maps.

```javascript
const fruits = ["apple", "banana", "orange"];

// for...in : donne les INDICES (strings)
for (let index in fruits) {
    console.log(typeof index);    // "string"
    console.log(index);           // "0", "1", "2"
    console.log(fruits[index]);   // "apple", "banana", "orange"
}

// for...of : donne les VALEURS
for (let fruit of fruits) {
    console.log(typeof fruit);    // "string"
    console.log(fruit);           // "apple", "banana", "orange"
}

// ⚠️ Piège avec for...in et opérations numériques
for (let i in fruits) {
    console.log(i + 1);  // "01", "11", "21" ⚠️ (string concat!)
}

// Avec objets
const user = {
    name: "Alice",
    age: 30,
    city: "Paris"
};

// ✅ for...in : parfait pour objets
for (let key in user) {
    console.log(`${key}: ${user[key]}`);
}

// ❌ for...of : ne fonctionne PAS avec objets normaux
// for (let value of user) { }  // TypeError: user is not iterable

// ✅ Solutions pour objets avec for...of
for (let key of Object.keys(user)) {
    console.log(`${key}: ${user[key]}`);
}

for (let value of Object.values(user)) {
    console.log(value);
}

for (let [key, value] of Object.entries(user)) {
    console.log(`${key}: ${value}`);
}

// for...of avec autres iterables
const text = "Hello";
for (let char of text) {
    console.log(char);  // "H", "e", "l", "l", "o"
}

const uniqueNums = new Set([1, 2, 3]);
for (let num of uniqueNums) {
    console.log(num);  // 1, 2, 3
}
```

**Résumé :**
- **for...in** → clés/indices (objets, éviter pour arrays)
- **for...of** → valeurs (arrays, strings, Sets, Maps, pas objets normaux)

---

### Q6 : Quelle est la différence entre `while` et `do...while` ?

**Réponse :**

`while` vérifie la condition **avant** d'exécuter le bloc. Si la condition est fausse dès le départ, le bloc ne s'exécute jamais.

`do...while` exécute le bloc **au moins une fois**, puis vérifie la condition.

```javascript
// while : peut ne JAMAIS s'exécuter
let x = 10;
while (x < 5) {
    console.log("Never prints");  // Condition false dès le début
    x++;
}

// do...while : s'exécute AU MOINS UNE FOIS
let y = 10;
do {
    console.log("Prints once: " + y);  // ✅ S'exécute malgré la condition
    y++;
} while (y < 5);  // Condition false, mais une exécution a eu lieu

// Cas d'usage typique : validation d'input
let password;
do {
    // password = prompt("Enter password (min 6 chars):");
    password = "abc";  // Simulation
    console.log("Attempt: " + password);
    
    if (password.length >= 6) {
        password = "valid123";  // Simule un input valide
    }
} while (password.length < 6);

console.log("Valid password entered");

// Équivalent avec while (plus verbeux)
let password2 = "";
while (password2.length < 6) {
    // password2 = prompt("Enter password (min 6 chars):");
    password2 = "valid123";  // Simulation
}

// Menu avec do...while
let choice;
do {
    console.log("=== Menu ===");
    console.log("1. Option A");
    console.log("2. Option B");
    console.log("3. Exit");
    
    choice = Math.floor(Math.random() * 4);  // Simule input
    console.log("Selected: " + choice);
    
    if (choice === 1) {
        console.log("Executing Option A");
    } else if (choice === 2) {
        console.log("Executing Option B");
    }
    
} while (choice !== 3);

console.log("Exited menu");
```

**Quand utiliser :**
- **while** : quand on ne sait pas combien d'itérations, condition peut être false dès le début
- **do...while** : quand on veut garantir au moins une exécution (menus, validation)

---

### Q7 : Comment sortir d'une boucle prématurément ?

**Réponse :**

Avec **`break`** pour sortir complètement de la boucle, ou **`continue`** pour passer à l'itération suivante.

```javascript
// break : sort complètement
for (let i = 0; i < 10; i++) {
    if (i === 5) {
        break;  // Sort de la boucle
    }
    console.log(i);  // 0, 1, 2, 3, 4
}
console.log("After loop");

// continue : passe à l'itération suivante
for (let i = 0; i < 10; i++) {
    if (i % 2 === 0) {
        continue;  // Saute les pairs
    }
    console.log(i);  // 1, 3, 5, 7, 9
}

// Chercher dans un array
const numbers = [5, 10, 15, 20, 25];
let found = false;

for (let num of numbers) {
    if (num === 15) {
        found = true;
        console.log("Found 15!");
        break;  // Pas besoin de continuer
    }
}

// Filtrer avec continue
const scores = [45, 78, 92, 55, 88, 33, 95];

for (let score of scores) {
    if (score < 70) {
        continue;  // Ignore scores < 70
    }
    console.log("Pass: " + score);  // 78, 92, 88, 95
}

// break dans while
let attempts = 0;
while (true) {
    console.log("Attempt " + (attempts + 1));
    attempts++;
    
    if (attempts >= 3) {
        break;  // Sort de la boucle infinie
    }
}

// break dans boucles imbriquées (sort seulement de l'interne)
for (let i = 0; i < 3; i++) {
    console.log("Outer: " + i);
    
    for (let j = 0; j < 3; j++) {
        if (j === 1) {
            break;  // Sort seulement de la boucle j
        }
        console.log("  Inner: " + j);
    }
}
// Affiche : Outer:0, Inner:0, Outer:1, Inner:0, Outer:2, Inner:0

// Labels pour sortir de plusieurs boucles
outerLoop: for (let i = 0; i < 3; i++) {
    for (let j = 0; j < 3; j++) {
        if (i === 1 && j === 1) {
            break outerLoop;  // Sort de TOUTES les boucles
        }
        console.log(`i: ${i}, j: ${j}`);
    }
}

// ⚠️ On ne peut pas utiliser break/continue dans forEach
const arr = [1, 2, 3, 4, 5];
arr.forEach(num => {
    if (num === 3) {
        // break;  // ❌ SyntaxError
        return;  // ⚠️ Sort seulement de cette itération (comme continue)
    }
    console.log(num);  // 1, 2, 4, 5
});

// Pour vraiment arrêter, utiliser for...of
for (let num of arr) {
    if (num === 3) {
        break;  // ✅ Sort de la boucle
    }
    console.log(num);  // 1, 2
}
```

---

### Q8 : Quelle boucle choisir dans quelle situation ?

**Réponse :**

Voici un guide de décision :

```javascript
const data = [10, 20, 30, 40, 50];

// ✅ for...of : parcourir arrays (RECOMMANDÉ, plus lisible)
for (let value of data) {
    console.log(value);
}

// ✅ for classique : besoin de l'index ou contrôle précis
for (let i = 0; i < data.length; i++) {
    console.log(`Index ${i}: ${data[i]}`);
}

// Pour modifier l'index dynamiquement
for (let i = 0; i < data.length; i++) {
    if (data[i] === 20) {
        i++;  // Sauter l'élément suivant
    }
    console.log(data[i]);
}

// ✅ for...in : parcourir les clés d'un objet
const user = { name: "Alice", age: 30, city: "Paris" };
for (let key in user) {
    console.log(`${key}: ${user[key]}`);
}

// ✅ while : nombre d'itérations inconnu, condition complexe
let count = 0;
let sum = 0;
while (sum < 100) {
    sum += Math.floor(Math.random() * 20);
    count++;
}
console.log(`Took ${count} iterations to reach ${sum}`);

// ✅ do...while : au moins une exécution nécessaire
let userInput;
do {
    // userInput = prompt("Enter 'yes' to continue:");
    userInput = "no";  // Simulation
} while (userInput !== "yes");

// ✅ forEach (méthode array, pas structure de contrôle)
data.forEach((value, index) => {
    console.log(`${index}: ${value}`);
});
```

**Résumé des choix :**

| Situation | Boucle recommandée |
|-----------|-------------------|
| Parcourir array (valeurs) | `for...of` |
| Parcourir array (index nécessaire) | `for` classique |
| Parcourir objet | `for...in` ou `Object.entries()` |
| Condition complexe | `while` |
| Au moins 1 exécution | `do...while` |
| Besoin de break/continue | `for` ou `for...of` |

---

## Questions Pratiques / Pièges

### Q9 : Que va afficher ce code ?

```javascript
for (let i = 0; i < 3; i++) {
    setTimeout(() => console.log(i), 100);
}
```

**Réponse :**

```javascript
// Affiche : 0, 1, 2 (après 100ms)

// ✅ Avec let : chaque itération a son propre scope
// Le i est capturé correctement dans la closure

// ⚠️ Si on utilisait var :
for (var j = 0; j < 3; j++) {
    setTimeout(() => console.log(j), 100);
}
// Affiche : 3, 3, 3 ⚠️
// Parce que var a un function scope, pas block scope
// Quand les setTimeout s'exécutent, j vaut déjà 3

// Solution avec var (avant ES6)
for (var k = 0; k < 3; k++) {
    (function(index) {
        setTimeout(() => console.log(index), 100);
    })(k);  // IIFE capture k
}
// Affiche : 0, 1, 2 ✅

// Mais aujourd'hui, juste utiliser let!
```

---

### Q10 : Que va afficher ce code ?

```javascript
const arr = [1, 2, 3];
for (let i in arr) {
    console.log(i + 1);
}
```

**Réponse :**

```javascript
// Affiche : "01", "11", "21" ⚠️

// for...in retourne les indices en tant que STRINGS
// Donc i + 1 fait une concaténation string, pas une addition!

console.log(typeof i);  // "string"

// Solutions :
// 1. Convertir en number
for (let i in arr) {
    console.log(Number(i) + 1);  // 1, 2, 3 ✅
}

// 2. Utiliser parseInt
for (let i in arr) {
    console.log(parseInt(i) + 1);  // 1, 2, 3 ✅
}

// 3. Utiliser unary plus
for (let i in arr) {
    console.log(+i + 1);  // 1, 2, 3 ✅
}

// 4. Utiliser for classique (meilleur choix)
for (let i = 0; i < arr.length; i++) {
    console.log(i + 1);  // 1, 2, 3 ✅
}

// 5. Utiliser for...of avec entries()
for (let [index, value] of arr.entries()) {
    console.log(index + 1);  // 1, 2, 3 ✅
}
```

---

### Q11 : Que va afficher ce code ?

```javascript
let i = 0;
while (i < 5) {
    console.log(i);
    if (i === 3) {
        continue;
    }
    i++;
}
```

**Réponse :**

```javascript
// Boucle INFINIE! Affiche 0, 1, 2, 3, 3, 3, 3... ⚠️

// Problème : quand i === 3, on fait continue
// Mais i++ n'est jamais exécuté, donc i reste 3 pour toujours

// ✅ Solution 1 : incrémenter avant continue
let i = 0;
while (i < 5) {
    console.log(i);
    i++;  // Incrémenter AVANT
    if (i === 3) {
        continue;
    }
}

// ✅ Solution 2 : incrémenter dans la condition de continue
let j = 0;
while (j < 5) {
    console.log(j);
    if (j++ === 3) {  // Post-increment dans la condition
        continue;
    }
}

// ✅ Solution 3 : utiliser for (plus sûr)
for (let k = 0; k < 5; k++) {
    console.log(k);
    if (k === 3) {
        continue;  // Pas de problème, k++ s'exécute toujours
    }
}
```

---

### Q12 : Pourquoi ce code ne fonctionne pas comme prévu ?

```javascript
const numbers = [1, 2, 3, 4, 5];
numbers.forEach(num => {
    if (num === 3) {
        break;  // ❌ Erreur
    }
    console.log(num);
});
```

**Réponse :**

`forEach` ne supporte **pas** `break` ni `continue`. Pour arrêter une boucle prématurément, il faut utiliser `for`, `for...of`, ou `some()`/`every()`.

```javascript
const numbers = [1, 2, 3, 4, 5];

// ❌ forEach ne supporte pas break
// numbers.forEach(num => {
//     if (num === 3) {
//         break;  // SyntaxError
//     }
//     console.log(num);
// });

// ⚠️ return dans forEach = continue (pas break)
numbers.forEach(num => {
    if (num === 3) {
        return;  // Sort seulement de CETTE itération
    }
    console.log(num);  // 1, 2, 4, 5 (saute 3)
});

// ✅ Solution 1 : for...of avec break
for (let num of numbers) {
    if (num === 3) {
        break;  // Arrête la boucle
    }
    console.log(num);  // 1, 2
}

// ✅ Solution 2 : for classique
for (let i = 0; i < numbers.length; i++) {
    if (numbers[i] === 3) {
        break;
    }
    console.log(numbers[i]);  // 1, 2
}

// ✅ Solution 3 : some() (arrête quand return true)
numbers.some(num => {
    if (num === 3) {
        return true;  // Arrête la boucle
    }
    console.log(num);  // 1, 2
    return false;  // Continue
});

// ✅ Solution 4 : every() (arrête quand return false)
numbers.every(num => {
    console.log(num);  // 1, 2
    return num !== 3;  // Continue jusqu'à 3
});
```

---