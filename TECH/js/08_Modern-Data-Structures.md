## [**..**](./00_index.md)

# Chapitre 8 : Structures de Données Modernes

## 8.1 Set et WeakSet

### Introduction aux Sets

Un **Set** est une collection de **valeurs uniques**. Contrairement aux arrays, un Set ne peut pas contenir de doublons.

```javascript
// Créer un Set vide
const emptySet = new Set();

// Créer un Set avec des valeurs initiales
const numbers = new Set([1, 2, 3, 4, 5]);
console.log(numbers);  // Set(5) { 1, 2, 3, 4, 5 }

// Set élimine automatiquement les doublons
const withDuplicates = new Set([1, 2, 2, 3, 3, 3, 4]);
console.log(withDuplicates);  // Set(4) { 1, 2, 3, 4 }

// Set peut contenir n'importe quel type de valeur
const mixed = new Set([1, 'hello', true, null, undefined, { a: 1 }]);
console.log(mixed.size);  // 6

// Créer un Set depuis un string (chaque caractère)
const chars = new Set('hello');
console.log(chars);  // Set(4) { 'h', 'e', 'l', 'o' }

// Set vide vs Array vide
console.log(new Set().size);  // 0
console.log([].length);       // 0
```

### Méthodes de Set

#### `add()` - Ajouter une valeur

```javascript
const fruits = new Set();

// Ajouter des éléments
fruits.add('apple');
fruits.add('banana');
fruits.add('orange');
console.log(fruits);  // Set(3) { 'apple', 'banana', 'orange' }

// Ajouter un doublon (ignoré)
fruits.add('apple');
console.log(fruits.size);  // 3 (toujours 3, pas de doublon)

// add() retourne le Set (permet le chaining)
fruits.add('grape').add('melon').add('kiwi');
console.log(fruits.size);  // 6

// Ajouter différents types
const data = new Set();
data.add(1);
data.add('1');  // String '1' !== Number 1
data.add(true);
console.log(data);  // Set(3) { 1, '1', true }

// ⚠️ Avec les objets, chaque objet est unique (référence)
const set = new Set();
set.add({ name: 'Alice' });
set.add({ name: 'Alice' });  // Différente référence !
console.log(set.size);  // 2 (deux objets différents)

// Même objet = même référence
const obj = { name: 'Bob' };
set.add(obj);
set.add(obj);  // Même référence
console.log(set.size);  // 3 (obj ajouté une seule fois)
```

#### `delete()` - Supprimer une valeur

```javascript
const numbers = new Set([1, 2, 3, 4, 5]);

// Supprimer un élément
numbers.delete(3);
console.log(numbers);  // Set(4) { 1, 2, 4, 5 }

// delete() retourne true si l'élément existait, false sinon
console.log(numbers.delete(4));   // true (4 existait)
console.log(numbers.delete(10));  // false (10 n'existait pas)

// Supprimer un élément qui n'existe pas (pas d'erreur)
numbers.delete(100);  // Rien ne se passe

// Avec des objets (par référence)
const obj1 = { id: 1 };
const obj2 = { id: 2 };
const objSet = new Set([obj1, obj2]);

objSet.delete(obj1);  // ✅ Fonctionne (même référence)
console.log(objSet.size);  // 1

objSet.delete({ id: 2 });  // ❌ Ne fonctionne pas (différente référence)
console.log(objSet.size);  // 1 (obj2 toujours présent)
```

#### `has()` - Vérifier l'existence

```javascript
const colors = new Set(['red', 'green', 'blue']);

// Vérifier si un élément existe
console.log(colors.has('red'));     // true
console.log(colors.has('yellow'));  // false

// has() est très rapide (O(1))
// Plus rapide que array.includes() pour de grandes collections

// Cas pratique : validation
function isValidRole(role) {
  const validRoles = new Set(['admin', 'user', 'guest', 'moderator']);
  return validRoles.has(role);
}

console.log(isValidRole('admin'));   // true
console.log(isValidRole('hacker'));  // false

// Avec des objets (par référence)
const obj = { name: 'Alice' };
const objSet = new Set([obj]);

console.log(objSet.has(obj));              // true
console.log(objSet.has({ name: 'Alice' })); // false (différente référence)

// Cas pratique : éviter les doublons
const visited = new Set();

function visit(url) {
  if (visited.has(url)) {
    console.log(`Already visited: ${url}`);
    return;
  }
  
  visited.add(url);
  console.log(`Visiting: ${url}`);
}

visit('/home');    // Visiting: /home
visit('/about');   // Visiting: /about
visit('/home');    // Already visited: /home
```

#### `clear()` - Vider le Set

```javascript
const numbers = new Set([1, 2, 3, 4, 5]);

console.log(numbers.size);  // 5

// Vider complètement le Set
numbers.clear();

console.log(numbers.size);  // 0
console.log(numbers);       // Set(0) {}

// clear() ne retourne rien (undefined)
const result = numbers.clear();
console.log(result);  // undefined
```

#### `size` - Obtenir la taille

```javascript
const letters = new Set(['a', 'b', 'c']);

// size est une PROPRIÉTÉ, pas une méthode
console.log(letters.size);  // 3 (pas letters.size())

// size se met à jour automatiquement
letters.add('d');
console.log(letters.size);  // 4

letters.delete('a');
console.log(letters.size);  // 3

letters.clear();
console.log(letters.size);  // 0

// Différence avec Array.length
const arr = [1, 2, 3];
console.log(arr.length);  // 3 (propriété modifiable)
arr.length = 10;          // On peut modifier
console.log(arr.length);  // 10

const set = new Set([1, 2, 3]);
console.log(set.size);    // 3 (propriété en lecture seule)
// set.size = 10;         // ❌ Ne fait rien (read-only)
```

### Itération sur un Set

```javascript
const fruits = new Set(['apple', 'banana', 'orange']);

// 1. for...of (le plus courant)
for (const fruit of fruits) {
  console.log(fruit);
}
// apple
// banana
// orange

// 2. forEach() (similaire aux arrays)
fruits.forEach((value, valueAgain, set) => {
  console.log(value);
  // valueAgain = value (pour compatibilité avec Map)
  // set = le Set lui-même
});

// Usage pratique de forEach
fruits.forEach(fruit => {
  console.log(fruit.toUpperCase());
});
// APPLE
// BANANA
// ORANGE

// 3. Convertir en array puis itérer
const fruitsArray = [...fruits];
fruitsArray.forEach((fruit, index) => {
  console.log(`${index + 1}. ${fruit}`);
});
// 1. apple
// 2. banana
// 3. orange

// 4. Méthodes d'itération
// values() - retourne un iterator des valeurs
for (const value of fruits.values()) {
  console.log(value);
}

// keys() - identique à values() pour un Set
for (const key of fruits.keys()) {
  console.log(key);  // même résultat que values()
}

// entries() - retourne [value, value] pour compatibilité avec Map
for (const [key, value] of fruits.entries()) {
  console.log(key, value);  // key === value
}
// apple apple
// banana banana
// orange orange

// Cas pratique : map-like transformation
const numbers = new Set([1, 2, 3, 4, 5]);

// Transformer avec Array.from()
const doubled = new Set(
  Array.from(numbers, n => n * 2)
);
console.log(doubled);  // Set(5) { 2, 4, 6, 8, 10 }

// Ou avec spread + map
const tripled = new Set(
  [...numbers].map(n => n * 3)
);
console.log(tripled);  // Set(5) { 3, 6, 9, 12, 15 }

// Filtrer un Set
const evenNumbers = new Set(
  [...numbers].filter(n => n % 2 === 0)
);
console.log(evenNumbers);  // Set(2) { 2, 4 }
```

### Cas d'usage des Sets

#### 1. Supprimer les doublons d'un array

```javascript
// Méthode la plus simple et élégante
const numbersWithDuplicates = [1, 2, 2, 3, 3, 3, 4, 5, 5];
const unique = [...new Set(numbersWithDuplicates)];
console.log(unique);  // [1, 2, 3, 4, 5]

// Avec Array.from()
const unique2 = Array.from(new Set(numbersWithDuplicates));
console.log(unique2);  // [1, 2, 3, 4, 5]

// Avec des strings
const text = 'hello world';
const uniqueChars = [...new Set(text)].join('');
console.log(uniqueChars);  // 'helo wrd'

// Avec des objets (attention aux références !)
const users = [
  { id: 1, name: 'Alice' },
  { id: 2, name: 'Bob' },
  { id: 1, name: 'Alice' }  // Doublon logique mais référence différente
];

// ❌ Ceci ne fonctionne pas (objets différents)
const uniqueUsers = [...new Set(users)];
console.log(uniqueUsers.length);  // 3 (toujours 3)

// ✅ Solution : dédoublonner par propriété
const uniqueById = [...new Map(users.map(u => [u.id, u])).values()];
console.log(uniqueById);
// [{ id: 1, name: 'Alice' }, { id: 2, name: 'Bob' }]
```

#### 2. Opérations d'ensemble (union, intersection, différence)

```javascript
const setA = new Set([1, 2, 3, 4, 5]);
const setB = new Set([4, 5, 6, 7, 8]);

// UNION (tous les éléments des deux sets)
const union = new Set([...setA, ...setB]);
console.log(union);  // Set(8) { 1, 2, 3, 4, 5, 6, 7, 8 }

// INTERSECTION (éléments communs)
const intersection = new Set(
  [...setA].filter(x => setB.has(x))
);
console.log(intersection);  // Set(2) { 4, 5 }

// DIFFÉRENCE (éléments dans A mais pas dans B)
const difference = new Set(
  [...setA].filter(x => !setB.has(x))
);
console.log(difference);  // Set(3) { 1, 2, 3 }

// DIFFÉRENCE SYMÉTRIQUE (éléments dans A ou B mais pas dans les deux)
const symmetricDifference = new Set([
  ...[...setA].filter(x => !setB.has(x)),
  ...[...setB].filter(x => !setA.has(x))
]);
console.log(symmetricDifference);  // Set(6) { 1, 2, 3, 6, 7, 8 }

// Fonctions utilitaires
function union(setA, setB) {
  return new Set([...setA, ...setB]);
}

function intersection(setA, setB) {
  return new Set([...setA].filter(x => setB.has(x)));
}

function difference(setA, setB) {
  return new Set([...setA].filter(x => !setB.has(x)));
}

function symmetricDifference(setA, setB) {
  const diff1 = difference(setA, setB);
  const diff2 = difference(setB, setA);
  return union(diff1, diff2);
}

// Vérifier si un Set est un sous-ensemble d'un autre
function isSubset(subset, superset) {
  return [...subset].every(elem => superset.has(elem));
}

const set1 = new Set([1, 2]);
const set2 = new Set([1, 2, 3, 4]);
console.log(isSubset(set1, set2));  // true
console.log(isSubset(set2, set1));  // false
```

#### 3. Tracking des valeurs uniques

```javascript
// Cas pratique : compter les visiteurs uniques
const visitors = new Set();

function trackVisitor(userId) {
  visitors.add(userId);
  console.log(`Total unique visitors: ${visitors.size}`);
}

trackVisitor('user123');  // Total unique visitors: 1
trackVisitor('user456');  // Total unique visitors: 2
trackVisitor('user123');  // Total unique visitors: 2 (déjà compté)

// Cas pratique : tags uniques
const articleTags = new Set();

function addTag(tag) {
  articleTags.add(tag.toLowerCase());
}

addTag('JavaScript');
addTag('React');
addTag('javascript');  // Doublon (lowercase)
addTag('Node.js');

console.log([...articleTags]);
// ['javascript', 'react', 'node.js']

// Cas pratique : IDs uniques générés
const generatedIds = new Set();

function generateUniqueId() {
  let id;
  do {
    id = Math.random().toString(36).substr(2, 9);
  } while (generatedIds.has(id));
  
  generatedIds.add(id);
  return id;
}

console.log(generateUniqueId());  // 'x7k9m2p1q'
console.log(generateUniqueId());  // 'a3f8n4r7s' (toujours unique)
```

#### 4. Performance : recherche rapide

```javascript
// Set.has() est O(1) - très rapide
// Array.includes() est O(n) - plus lent pour grandes collections

// Benchmark avec grande collection
const arrayLarge = Array.from({ length: 100000 }, (_, i) => i);
const setLarge = new Set(arrayLarge);

console.time('Array.includes');
arrayLarge.includes(99999);
console.timeEnd('Array.includes');  // ~1-2ms

console.time('Set.has');
setLarge.has(99999);
console.timeEnd('Set.has');  // ~0.001ms (beaucoup plus rapide)

// Cas pratique : filtrer avec liste noire
const blacklist = new Set(['spam', 'fake', 'scam', 'phishing']);

function isValidWord(word) {
  return !blacklist.has(word.toLowerCase());
}

const words = ['hello', 'spam', 'world', 'fake', 'javascript'];
const validWords = words.filter(isValidWord);
console.log(validWords);  // ['hello', 'world', 'javascript']
```

### WeakSet

Un **WeakSet** est similaire à un Set mais avec des différences importantes :

```javascript
// WeakSet ne peut contenir QUE des objets (pas de primitives)
const weakSet = new WeakSet();

const obj1 = { id: 1 };
const obj2 = { id: 2 };

// Ajouter des objets
weakSet.add(obj1);
weakSet.add(obj2);

// ❌ On ne peut pas ajouter de primitives
// weakSet.add(1);        // TypeError
// weakSet.add('hello');  // TypeError
// weakSet.add(true);     // TypeError

// Méthodes disponibles : add(), delete(), has()
console.log(weakSet.has(obj1));  // true
weakSet.delete(obj2);
console.log(weakSet.has(obj2));  // false

// ❌ Pas de méthodes : size, clear(), itération
// console.log(weakSet.size);     // undefined
// weakSet.clear();               // TypeError
// for (const item of weakSet) {} // TypeError
```

**Différences clés avec Set :**

| Critère | Set | WeakSet |
|---------|-----|---------|
| Types acceptés | Tous types | Objets seulement |
| Propriété `size` | ✅ Oui | ❌ Non |
| Itération | ✅ Oui | ❌ Non |
| Garbage Collection | Références fortes | Références faibles |

**Garbage Collection avec WeakSet :**

```javascript
let obj = { name: 'Alice' };
const weakSet = new WeakSet();
const normalSet = new Set();

weakSet.add(obj);
normalSet.add(obj);

// obj est référencé par :
// - la variable 'obj'
// - weakSet (référence faible)
// - normalSet (référence forte)

obj = null;  // On supprime la référence de la variable

// Maintenant :
// - weakSet ne garde PAS l'objet en vie (référence faible)
//   → l'objet peut être garbage collected
// - normalSet GARDE l'objet en vie (référence forte)
//   → l'objet ne sera PAS garbage collected

// WeakSet permet au garbage collector de nettoyer l'objet
// si plus aucune autre référence forte n'existe
```

**Cas d'usage de WeakSet :**

```javascript
// 1. Marquer des objets sans empêcher leur suppression
const processedObjects = new WeakSet();

function processData(data) {
  if (processedObjects.has(data)) {
    console.log('Already processed');
    return;
  }
  
  // Traiter les données
  console.log('Processing:', data);
  
  // Marquer comme traité
  processedObjects.add(data);
}

let data1 = { id: 1, value: 'test' };
processData(data1);  // Processing: { id: 1, value: 'test' }
processData(data1);  // Already processed

// Si data1 n'est plus utilisé ailleurs, il peut être garbage collected
data1 = null;

// 2. Stocker des métadonnées sur des objets DOM
const clickedElements = new WeakSet();

document.querySelectorAll('button').forEach(button => {
  button.addEventListener('click', function() {
    if (clickedElements.has(this)) {
      console.log('Button already clicked');
      return;
    }
    
    clickedElements.add(this);
    console.log('First click on this button');
  });
});

// Si un bouton est retiré du DOM, il peut être garbage collected
// car WeakSet n'empêche pas sa suppression

// 3. Éviter les fuites mémoire dans les caches
class DataCache {
  constructor() {
    this.processed = new WeakSet();
  }
  
  process(obj) {
    if (this.processed.has(obj)) {
      return 'cached';
    }
    
    // Traitement coûteux
    this.processed.add(obj);
    return 'processed';
  }
}

const cache = new DataCache();
let item = { data: 'important' };

cache.process(item);  // 'processed'
cache.process(item);  // 'cached'

// Quand item n'est plus utilisé, il peut être garbage collected
// WeakSet ne crée pas de fuite mémoire
item = null;
```

---

## 8.2 Map et WeakMap

### Introduction aux Maps

Un **Map** est une collection de paires **clé-valeur** où les clés peuvent être de n'importe quel type (pas seulement des strings comme pour les objets).

```javascript
// Créer un Map vide
const emptyMap = new Map();

// Créer un Map avec des valeurs initiales
const users = new Map([
  ['user1', { name: 'Alice', age: 25 }],
  ['user2', { name: 'Bob', age: 30 }],
  ['user3', { name: 'Charlie', age: 35 }]
]);

console.log(users.size);  // 3

// Map peut avoir des clés de N'IMPORTE QUEL type
const mixedKeys = new Map();
mixedKeys.set('string-key', 'value1');       // String comme clé
mixedKeys.set(42, 'value2');                  // Number comme clé
mixedKeys.set(true, 'value3');                // Boolean comme clé
mixedKeys.set({ id: 1 }, 'value4');          // Object comme clé
mixedKeys.set([1, 2], 'value5');             // Array comme clé
mixedKeys.set(function() {}, 'value6');      // Function comme clé

console.log(mixedKeys.size);  // 6

// Map vs Object : différence majeure
const obj = {};
const map = new Map();

// Object convertit les clés en strings
obj[1] = 'one';
obj['1'] = 'ONE';
console.log(obj[1]);  // 'ONE' (écrasé car 1 === '1' pour object)

// Map garde les types distincts
map.set(1, 'one');
map.set('1', 'ONE');
console.log(map.get(1));    // 'one'
console.log(map.get('1'));  // 'ONE' (deux entrées différentes)
```

### Méthodes de Map

#### `set()` - Ajouter ou modifier une paire clé-valeur

```javascript
const settings = new Map();

// Ajouter des paires clé-valeur
settings.set('theme', 'dark');
settings.set('language', 'fr');
settings.set('fontSize', 16);

console.log(settings);
// Map(3) { 'theme' => 'dark', 'language' => 'fr', 'fontSize' => 16 }

// set() retourne le Map (permet le chaining)
settings
  .set('notifications', true)
  .set('autoSave', false)
  .set('timeout', 5000);

console.log(settings.size);  // 6

// Modifier une valeur existante
settings.set('theme', 'light');  // Écrase 'dark'
console.log(settings.get('theme'));  // 'light'

// Avec des objets comme clés
const user1 = { id: 1, name: 'Alice' };
const user2 = { id: 2, name: 'Bob' };

const userScores = new Map();
userScores.set(user1, 95);
userScores.set(user2, 87);

console.log(userScores.get(user1));  // 95

// ⚠️ Avec objets, attention aux références
userScores.set({ id: 1 }, 100);  // Différente référence !
console.log(userScores.size);  // 3 (nouvelle entrée)
console.log(userScores.get(user1));  // 95 (toujours)
console.log(userScores.get({ id: 1 }));  // undefined (pas la même référence)
```

#### `get()` - Récupérer une valeur

```javascript
const config = new Map([
  ['host', 'localhost'],
  ['port', 3000],
  ['ssl', false]
]);

// Récupérer une valeur
console.log(config.get('host'));  // 'localhost'
console.log(config.get('port'));  // 3000
console.log(config.get('ssl'));   // false

// Clé inexistante retourne undefined
console.log(config.get('timeout'));  // undefined

// Avec différents types de clés
const map = new Map();
map.set(1, 'number one');
map.set('1', 'string one');

console.log(map.get(1));    // 'number one'
console.log(map.get('1'));  // 'string one'
console.log(map.get(2));    // undefined

// Avec objets comme clés
const key1 = { id: 1 };
const key2 = { id: 2 };

const objMap = new Map();
objMap.set(key1, 'value1');
objMap.set(key2, 'value2');

console.log(objMap.get(key1));  // 'value1'
console.log(objMap.get(key2));  // 'value2'
console.log(objMap.get({ id: 1 }));  // undefined (différente référence)

// Valeur par défaut si clé n'existe pas
function getWithDefault(map, key, defaultValue) {
  return map.has(key) ? map.get(key) : defaultValue;
}

console.log(getWithDefault(config, 'timeout', 5000));  // 5000
```

#### `has()` - Vérifier l'existence d'une clé

```javascript
const users = new Map([
  ['alice', { age: 25 }],
  ['bob', { age: 30 }]
]);

// Vérifier si une clé existe
console.log(users.has('alice'));    // true
console.log(users.has('charlie'));  // false

// has() est O(1) - très rapide
// Plus rapide que 'key' in object pour de grandes collections

// Cas pratique : vérifier avant d'accéder
if (users.has('alice')) {
  const user = users.get('alice');
  console.log(user.age);  // 25
}

// Avec différents types de clés
const map = new Map();
map.set(1, 'one');
map.set('1', 'ONE');
map.set(true, 'yes');

console.log(map.has(1));      // true
console.log(map.has('1'));    // true
console.log(map.has(true));   // true
console.log(map.has(false));  // false

// Avec objets comme clés (par référence)
const obj = { id: 1 };
const objMap = new Map();
objMap.set(obj, 'value');

console.log(objMap.has(obj));         // true
console.log(objMap.has({ id: 1 }));   // false (différente référence)
```

#### `delete()` - Supprimer une paire clé-valeur

```javascript
const scores = new Map([
  ['alice', 95],
  ['bob', 87],
  ['charlie', 92]
]);

// Supprimer une entrée
scores.delete('bob');
console.log(scores.size);  // 2
console.log(scores.has('bob'));  // false

// delete() retourne true si la clé existait, false sinon
console.log(scores.delete('charlie'));  // true
console.log(scores.delete('dave'));     // false (n'existait pas)

// Supprimer tous les éléments un par un
for (const key of scores.keys()) {
  scores.delete(key);
}
console.log(scores.size);  // 0

// Avec objets comme clés
const key = { id: 1 };
const map = new Map();
map.set(key, 'value');

map.delete(key);  // ✅ Fonctionne (même référence)
console.log(map.size);  // 0

map.set(key, 'value');
map.delete({ id: 1 });  // ❌ Ne fonctionne pas (différente référence)
console.log(map.size);  // 1 (toujours présent)
```

#### `clear()` - Vider le Map

```javascript
const data = new Map([
  ['a', 1],
  ['b', 2],
  ['c', 3]
]);

console.log(data.size);  // 3

// Vider complètement le Map
data.clear();

console.log(data.size);  // 0
console.log(data);       // Map(0) {}

// clear() ne retourne rien (undefined)
const result = data.clear();
console.log(result);  // undefined
```

#### `size` - Obtenir le nombre d'entrées

```javascript
const map = new Map();

console.log(map.size);  // 0

map.set('a', 1);
console.log(map.size);  // 1

map.set('b', 2);
map.set('c', 3);
console.log(map.size);  // 3

map.delete('b');
console.log(map.size);  // 2

map.clear();
console.log(map.size);  // 0

// size est une propriété (pas une méthode)
// console.log(map.size());  // ❌ Error

// Différence avec Object
const obj = { a: 1, b: 2, c: 3 };
console.log(Object.keys(obj).length);  // 3 (calcul nécessaire)

const map2 = new Map([['a', 1], ['b', 2], ['c', 3]]);
console.log(map2.size);  // 3 (direct, O(1))
```

### Itération sur un Map

```javascript
const users = new Map([
  ['alice', { age: 25, city: 'Paris' }],
  ['bob', { age: 30, city: 'London' }],
  ['charlie', { age: 35, city: 'New York' }]
]);

// 1. for...of avec entries() (par défaut)
for (const [key, value] of users) {
  console.log(`${key}:`, value);
}
// alice: { age: 25, city: 'Paris' }
// bob: { age: 30, city: 'London' }
// charlie: { age: 35, city: 'New York' }

// 2. entries() explicite (identique)
for (const [username, userInfo] of users.entries()) {
  console.log(`${username} is ${userInfo.age} years old`);
}
// alice is 25 years old
// bob is 30 years old
// charlie is 35 years old

// 3. keys() - seulement les clés
for (const key of users.keys()) {
  console.log(key);
}
// alice
// bob
// charlie

// 4. values() - seulement les valeurs
for (const value of users.values()) {
  console.log(value);
}
// { age: 25, city: 'Paris' }
// { age: 30, city: 'London' }
// { age: 35, city: 'New York' }

// 5. forEach() - similaire aux arrays
users.forEach((value, key, map) => {
  console.log(`${key}: ${value.city}`);
});
// alice: Paris
// bob: London
// charlie: New York

// ⚠️ Ordre des paramètres : (value, key) - différent de Object.entries()
// Object.entries() : [key, value]
// Map.forEach() : (value, key)

users.forEach((value, key) => {
  console.log(key, value);
});

// 6. Convertir en array pour utiliser les méthodes d'array
const entries = [...users];
console.log(entries);
// [
//   ['alice', { age: 25, city: 'Paris' }],
//   ['bob', { age: 30, city: 'London' }],
//   ['charlie', { age: 35, city: 'New York' }]
// ]

// Array.from() aussi
const keysArray = Array.from(users.keys());
console.log(keysArray);  // ['alice', 'bob', 'charlie']

const valuesArray = Array.from(users.values());
console.log(valuesArray);
// [{ age: 25, city: 'Paris' }, { age: 30, city: 'London' }, ...]

// 7. Filtrer un Map
const adults = new Map(
  [...users].filter(([key, value]) => value.age >= 30)
);
console.log(adults);
// Map(2) { 'bob' => { age: 30, ... }, 'charlie' => { age: 35, ... } }

// 8. Transformer les valeurs d'un Map
const ages = new Map(
  [...users].map(([key, value]) => [key, value.age])
);
console.log(ages);
// Map(3) { 'alice' => 25, 'bob' => 30, 'charlie' => 35 }

// 9. Réduire un Map
const totalAge = [...users.values()].reduce((sum, user) => sum + user.age, 0);
console.log(totalAge);  // 90
```

### Map vs Object : Différences clés

```javascript
// 1. TYPES DE CLÉS

// Object : clés sont TOUJOURS des strings (ou Symbols)
const obj = {};
obj[1] = 'number';
obj['1'] = 'string';
console.log(obj[1]);  // 'string' (1 converti en '1')
console.log(Object.keys(obj));  // ['1'] - une seule clé

// Map : clés peuvent être N'IMPORTE QUEL type
const map = new Map();
map.set(1, 'number');
map.set('1', 'string');
console.log(map.get(1));   // 'number'
console.log(map.get('1')); // 'string'
console.log(map.size);     // 2 - deux clés distinctes

// 2. ORDRE DES CLÉS

// Object : ordre non garanti (sauf pour les strings numériques)
const obj2 = { z: 1, a: 2, m: 3, 5: 4, 1: 5 };
console.log(Object.keys(obj2));  // ['1', '5', 'z', 'a', 'm']
// Nombres d'abord (ordre croissant), puis insertion order

// Map : ordre d'insertion TOUJOURS garanti
const map2 = new Map();
map2.set('z', 1);
map2.set('a', 2);
map2.set('m', 3);
console.log([...map2.keys()]);  // ['z', 'a', 'm'] - ordre d'insertion

// 3. TAILLE

// Object : calculer manuellement
const obj3 = { a: 1, b: 2, c: 3 };
console.log(Object.keys(obj3).length);  // 3 - O(n)

// Map : propriété size
const map3 = new Map([['a', 1], ['b', 2], ['c', 3]]);
console.log(map3.size);  // 3 - O(1)

// 4. ITÉRATION

// Object : nécessite Object.keys/values/entries
const obj4 = { a: 1, b: 2, c: 3 };
for (const [key, value] of Object.entries(obj4)) {
  console.log(key, value);
}

// Map : itérable nativement
const map4 = new Map([['a', 1], ['b', 2], ['c', 3]]);
for (const [key, value] of map4) {
  console.log(key, value);
}

// 5. PERFORMANCES

// Map est généralement plus rapide pour :
// - Ajouts/suppressions fréquents
// - Grandes collections
// - Recherches de clés

// Object est plus rapide pour :
// - Petites collections
// - Accès par propriété statique (obj.prop)

// 6. PROTOTYPE

// Object : hérite de Object.prototype
const obj5 = {};
console.log('toString' in obj5);  // true (hérité)

// Map : pas de pollution du prototype
const map5 = new Map();
console.log(map5.get('toString'));  // undefined

// 7. SÉRIALISATION

// Object : JSON.stringify fonctionne
const obj6 = { name: 'Alice', age: 25 };
console.log(JSON.stringify(obj6));  // '{"name":"Alice","age":25}'

// Map : JSON.stringify ne fonctionne pas directement
const map6 = new Map([['name', 'Alice'], ['age', 25]]);
console.log(JSON.stringify(map6));  // '{}'

// Solution : convertir en array
console.log(JSON.stringify([...map6]));
// '[["name","Alice"],["age",25]]'

// Ou en object
console.log(JSON.stringify(Object.fromEntries(map6)));
// '{"name":"Alice","age":25}'
```

**Quand utiliser Map vs Object :**

```javascript
// ✅ Utilisez MAP quand :
// - Les clés ne sont pas des strings
// - Vous devez préserver l'ordre d'insertion
// - Ajouts/suppressions fréquents
// - Vous devez itérer souvent
// - Vous avez besoin de la taille facilement

// Exemple : cache avec clés objets
const cache = new Map();

function getCachedData(user) {
  if (cache.has(user)) {
    return cache.get(user);
  }
  
  const data = fetchData(user);  // Opération coûteuse
  cache.set(user, data);
  return data;
}

// ✅ Utilisez OBJECT quand :
// - Toutes les clés sont des strings
// - Vous avez besoin de JSON.stringify/parse
// - Structure de données fixe
// - Accès par propriété statique (obj.prop)
// - Petite collection

// Exemple : configuration statique
const config = {
  host: 'localhost',
  port: 3000,
  ssl: false
};

console.log(config.host);  // Accès simple
```

### Cas d'usage pratiques des Maps

#### 1. Cache/Memoization

```javascript
// Cache avec Map (plus efficace qu'avec Object)
const fibCache = new Map();

function fibonacci(n) {
  if (n <= 1) return n;
  
  if (fibCache.has(n)) {
    return fibCache.get(n);
  }
  
  const result = fibonacci(n - 1) + fibonacci(n - 2);
  fibCache.set(n, result);
  return result;
}

console.log(fibonacci(10));  // 55
console.log(fibCache);
// Map(9) { 2 => 1, 3 => 2, 4 => 3, 5 => 5, 6 => 8, ... }

// Cache avec timeout
class TimedCache {
  constructor(ttl = 60000) {  // 60 secondes par défaut
    this.cache = new Map();
    this.ttl = ttl;
  }
  
  set(key, value) {
    const expiresAt = Date.now() + this.ttl;
    this.cache.set(key, { value, expiresAt });
  }
  
  get(key) {
    const item = this.cache.get(key);
    
    if (!item) return undefined;
    
    if (Date.now() > item.expiresAt) {
      this.cache.delete(key);
      return undefined;
    }
    
    return item.value;
  }
  
  has(key) {
    return this.get(key) !== undefined;
  }
}

const cache = new TimedCache(5000);  // 5 secondes
cache.set('user', { name: 'Alice' });
console.log(cache.get('user'));  // { name: 'Alice' }
// Après 5 secondes...
// console.log(cache.get('user'));  // undefined
```

#### 2. Compteurs et groupement

```javascript
// Compter les occurrences
const words = ['apple', 'banana', 'apple', 'orange', 'banana', 'apple'];
const wordCount = new Map();

words.forEach(word => {
  wordCount.set(word, (wordCount.get(word) || 0) + 1);
});

console.log(wordCount);
// Map(3) { 'apple' => 3, 'banana' => 2, 'orange' => 1 }

// Grouper par propriété
const users = [
  { name: 'Alice', role: 'admin' },
  { name: 'Bob', role: 'user' },
  { name: 'Charlie', role: 'admin' },
  { name: 'Dave', role: 'user' }
];

const usersByRole = new Map();

users.forEach(user => {
  if (!usersByRole.has(user.role)) {
    usersByRole.set(user.role, []);
  }
  usersByRole.get(user.role).push(user);
});

console.log(usersByRole);
// Map(2) {
//   'admin' => [{ name: 'Alice', role: 'admin' }, { name: 'Charlie', role: 'admin' }],
//   'user' => [{ name: 'Bob', role: 'user' }, { name: 'Dave', role: 'user' }]
// }

// Fonction utilitaire de groupement
function groupBy(array, keyFn) {
  const grouped = new Map();
  
  array.forEach(item => {
    const key = keyFn(item);
    if (!grouped.has(key)) {
      grouped.set(key, []);
    }
    grouped.get(key).push(item);
  });
  
  return grouped;
}

const byRole = groupBy(users, user => user.role);
console.log(byRole);
```

#### 3. Métadonnées sur des objets

```javascript
// Associer des métadonnées à des objets DOM
const elementMetadata = new Map();

document.querySelectorAll('.interactive').forEach(element => {
  elementMetadata.set(element, {
    clickCount: 0,
    lastClicked: null,
    originalColor: element.style.backgroundColor
  });
  
  element.addEventListener('click', function() {
    const meta = elementMetadata.get(this);
    meta.clickCount++;
    meta.lastClicked = new Date();
    elementMetadata.set(this, meta);
    
    console.log(`Clicked ${meta.clickCount} times`);
  });
});

// Associer des données à des objets quelconques
const userPermissions = new Map();

const user1 = { id: 1, name: 'Alice' };
const user2 = { id: 2, name: 'Bob' };

userPermissions.set(user1, ['read', 'write', 'delete']);
userPermissions.set(user2, ['read']);

function hasPermission(user, permission) {
  const permissions = userPermissions.get(user) || [];
  return permissions.includes(permission);
}

console.log(hasPermission(user1, 'delete'));  // true
console.log(hasPermission(user2, 'delete'));  // false
```

#### 4. LRU Cache (Least Recently Used)

```javascript
class LRUCache {
  constructor(maxSize = 100) {
    this.cache = new Map();
    this.maxSize = maxSize;
  }
  
  get(key) {
    if (!this.cache.has(key)) {
      return undefined;
    }
    
    // Déplacer à la fin (le plus récent)
    const value = this.cache.get(key);
    this.cache.delete(key);
    this.cache.set(key, value);
    
    return value;
  }
  
  set(key, value) {
    // Si la clé existe, la supprimer d'abord
    if (this.cache.has(key)) {
      this.cache.delete(key);
    }
    
    // Ajouter à la fin
    this.cache.set(key, value);
    
    // Si dépassement, supprimer le plus ancien (premier)
    if (this.cache.size > this.maxSize) {
      const firstKey = this.cache.keys().next().value;
      this.cache.delete(firstKey);
    }
  }
  
  has(key) {
    return this.cache.has(key);
  }
  
  clear() {
    this.cache.clear();
  }
  
  get size() {
    return this.cache.size;
  }
}

const cache = new LRUCache(3);
cache.set('a', 1);
cache.set('b', 2);
cache.set('c', 3);
console.log(cache.size);  // 3

cache.set('d', 4);  // 'a' est éjecté (le plus ancien)
console.log(cache.has('a'));  // false
console.log(cache.has('b'));  // true

cache.get('b');  // 'b' devient le plus récent
cache.set('e', 5);  // 'c' est éjecté (pas 'b')
console.log(cache.has('c'));  // false
console.log(cache.has('b'));  // true
```

#### 5. Convertir entre Map et Object

```javascript
// Object → Map
const obj = {
  name: 'Alice',
  age: 25,
  city: 'Paris'
};

const mapFromObj = new Map(Object.entries(obj));
console.log(mapFromObj);
// Map(3) { 'name' => 'Alice', 'age' => 25, 'city' => 'Paris' }

// Map → Object
const map = new Map([
  ['name', 'Bob'],
  ['age', 30],
  ['city', 'London']
]);

const objFromMap = Object.fromEntries(map);
console.log(objFromMap);
// { name: 'Bob', age: 30, city: 'London' }

// Filtrer et convertir
const filteredObj = Object.fromEntries(
  [...map].filter(([key, value]) => typeof value === 'string')
);
console.log(filteredObj);
// { name: 'Bob', city: 'London' }

// Transformer et convertir
const transformedObj = Object.fromEntries(
  [...map].map(([key, value]) => [key.toUpperCase(), value])
);
console.log(transformedObj);
// { NAME: 'Bob', AGE: 30, CITY: 'London' }
```

### WeakMap

Un **WeakMap** est similaire à un Map mais avec des différences importantes :

```javascript
// WeakMap n'accepte QUE des objets comme clés (pas de primitives)
const weakMap = new WeakMap();

const obj1 = { id: 1 };
const obj2 = { id: 2 };

// Ajouter des paires clé-valeur
weakMap.set(obj1, 'value1');
weakMap.set(obj2, 'value2');

// ❌ On ne peut pas utiliser de primitives comme clés
// weakMap.set('key', 'value');  // TypeError
// weakMap.set(123, 'value');    // TypeError
// weakMap.set(true, 'value');   // TypeError

// Méthodes disponibles : set(), get(), has(), delete()
console.log(weakMap.get(obj1));  // 'value1'
console.log(weakMap.has(obj2));  // true
weakMap.delete(obj2);
console.log(weakMap.has(obj2));  // false

// ❌ Pas de méthodes : size, clear(), keys(), values(), entries(), itération
// console.log(weakMap.size);      // undefined
// weakMap.clear();                // TypeError
// for (const item of weakMap) {}  // TypeError
```

**Différences clés avec Map :**

```javascript
// MAP
const map = new Map();
map.set('key', 'value');           // ✅ Accepte tous types de clés
console.log(map.size);             // ✅ Propriété size
for (const [k, v] of map) {}       // ✅ Itérable
map.clear();                       // ✅ Méthode clear()

// WEAKMAP
const weakMap = new WeakMap();
// weakMap.set('key', 'value');    // ❌ Seulement objets comme clés
// console.log(weakMap.size);      // ❌ Pas de size
// for (const [k, v] of weakMap){} // ❌ Pas itérable
// weakMap.clear();                // ❌ Pas de clear()
```

**Garbage Collection avec WeakMap :**

```javascript
let user = { name: 'Alice' };
const weakMap = new WeakMap();
const normalMap = new Map();

weakMap.set(user, 'some metadata');
normalMap.set(user, 'some metadata');

// user est référencé par :
// - la variable 'user'
// - weakMap (référence faible)
// - normalMap (référence forte)

user = null;  // On supprime la référence de la variable

// Maintenant :
// - weakMap ne garde PAS l'objet en vie (référence faible)
//   → l'objet et ses données peuvent être garbage collected
// - normalMap GARDE l'objet en vie (référence forte)
//   → l'objet ne sera PAS garbage collected

// WeakMap permet d'éviter les fuites mémoire
```

**Cas d'usage de WeakMap :**

```javascript
// 1. Données privées pour des objets
const privateData = new WeakMap();

class User {
  constructor(name, password) {
    this.name = name;
    // Stocker le password de manière "privée"
    privateData.set(this, { password });
  }
  
  checkPassword(input) {
    const data = privateData.get(this);
    return data.password === input;
  }
}

const user = new User('Alice', 'secret123');
console.log(user.name);              // 'Alice'
console.log(user.password);          // undefined (pas accessible)
console.log(user.checkPassword('secret123'));  // true

// Quand user est supprimé, ses données privées le sont aussi

// 2. Métadonnées sur des éléments DOM
const domMetadata = new WeakMap();

function attachMetadata(element, data) {
  domMetadata.set(element, data);
}

function getMetadata(element) {
  return domMetadata.get(element);
}

const button = document.querySelector('button');
attachMetadata(button, {
  clickCount: 0,
  createdAt: new Date()
});

// Si le bouton est retiré du DOM et plus aucune référence n'existe,
// ses métadonnées seront automatiquement garbage collected

// 3. Cache sans fuite mémoire
const cache = new WeakMap();

function processData(obj) {
  if (cache.has(obj)) {
    console.log('Using cached result');
    return cache.get(obj);
  }
  
  console.log('Computing result');
  const result = expensiveOperation(obj);
  cache.set(obj, result);
  return result;
}

function expensiveOperation(obj) {
  // Opération coûteuse
  return { processed: true, data: obj };
}

let data = { value: 42 };
processData(data);  // Computing result
processData(data);  // Using cached result

// Quand data n'est plus référencé, il est garbage collected
// avec ses données en cache
data = null;

// 4. Observer pattern sans fuites mémoire
const listeners = new WeakMap();

function subscribe(obj, callback) {
  if (!listeners.has(obj)) {
    listeners.set(obj, []);
  }
  listeners.get(obj).push(callback);
}

function notify(obj, event) {
  const callbacks = listeners.get(obj);
  if (callbacks) {
    callbacks.forEach(cb => cb(event));
  }
}

let myObject = { name: 'test' };
subscribe(myObject, event => console.log('Event:', event));
notify(myObject, 'update');  // Event: update

// Quand myObject n'est plus utilisé, ses listeners sont automatiquement nettoyés
myObject = null;
```

**Différence entre WeakMap et Map pour éviter les fuites :**

```javascript
// ❌ FUITE MÉMOIRE avec Map
const cache = new Map();

function createManyObjects() {
  for (let i = 0; i < 1000000; i++) {
    const obj = { id: i, data: new Array(1000) };
    cache.set(obj, computeExpensiveValue(obj));
  }
}

createManyObjects();
// Tous les objets restent en mémoire même s'ils ne sont plus utilisés !
// Map garde des références fortes

// ✅ PAS DE FUITE avec WeakMap
const betterCache = new WeakMap();

function createManyObjectsSafe() {
  for (let i = 0; i < 1000000; i++) {
    const obj = { id: i, data: new Array(1000) };
    betterCache.set(obj, computeExpensiveValue(obj));
  }
}

createManyObjectsSafe();
// Les objets qui ne sont plus référencés ailleurs peuvent être garbage collected
// WeakMap utilise des références faibles
```

---

# Chapitre 8 : Structures de Données Modernes - Questions/Réponses Orales

## Questions sur Set

### Q1: Qu'est-ce qu'un Set et quelle est sa caractéristique principale ?

**Réponse :**

Un **Set** est une collection de **valeurs uniques**. Sa caractéristique principale est qu'il ne peut pas contenir de doublons - chaque valeur n'apparaît qu'une seule fois.

```javascript
// Créer un Set
const numbers = new Set([1, 2, 3, 4, 5]);
console.log(numbers);  // Set(5) { 1, 2, 3, 4, 5 }

// Les doublons sont automatiquement éliminés
const withDuplicates = new Set([1, 2, 2, 3, 3, 3, 4]);
console.log(withDuplicates);  // Set(4) { 1, 2, 3, 4 }

// Essayer d'ajouter un doublon (ignoré)
numbers.add(3);  // Déjà présent
console.log(numbers.size);  // 5 (toujours 5)

// Set peut contenir n'importe quel type
const mixed = new Set([1, 'hello', true, null, { a: 1 }]);
console.log(mixed.size);  // 5
```

**Cas d'usage principal :** Supprimer les doublons d'un array

```javascript
const arr = [1, 2, 2, 3, 3, 3, 4, 5, 5];
const unique = [...new Set(arr)];
console.log(unique);  // [1, 2, 3, 4, 5]
```

---

### Q2: Comment ajouter, supprimer et vérifier l'existence d'un élément dans un Set ?

**Réponse :**

```javascript
const fruits = new Set();

// AJOUTER - add()
fruits.add('apple');
fruits.add('banana');
fruits.add('orange');
console.log(fruits);  // Set(3) { 'apple', 'banana', 'orange' }

// add() retourne le Set (permet le chaining)
fruits.add('grape').add('melon').add('kiwi');

// VÉRIFIER - has()
console.log(fruits.has('apple'));   // true
console.log(fruits.has('mango'));   // false

// has() est très rapide (O(1))
// Plus rapide que array.includes() pour grandes collections

// SUPPRIMER - delete()
fruits.delete('banana');
console.log(fruits.has('banana'));  // false

// delete() retourne true si l'élément existait, false sinon
console.log(fruits.delete('grape'));  // true
console.log(fruits.delete('xyz'));    // false (n'existait pas)

// VIDER - clear()
fruits.clear();
console.log(fruits.size);  // 0

// TAILLE - size (propriété, pas méthode)
const colors = new Set(['red', 'green', 'blue']);
console.log(colors.size);  // 3
```

**Point important :** `size` est une propriété, pas une méthode (contrairement à `length` pour les arrays).

---

### Q3: Quelle est la méthode la plus simple pour supprimer les doublons d'un array ?

**Réponse :**

Utiliser un **Set** avec le spread operator :

```javascript
const numbers = [1, 2, 2, 3, 3, 3, 4, 5, 5];

// Méthode la plus simple et élégante
const unique = [...new Set(numbers)];
console.log(unique);  // [1, 2, 3, 4, 5]

// Alternative avec Array.from()
const unique2 = Array.from(new Set(numbers));
console.log(unique2);  // [1, 2, 3, 4, 5]

// Avec des strings
const text = 'hello world';
const uniqueChars = [...new Set(text)];
console.log(uniqueChars);  // ['h', 'e', 'l', 'o', ' ', 'w', 'r', 'd']

// En une ligne, rejoindre en string
const uniqueStr = [...new Set(text)].join('');
console.log(uniqueStr);  // 'helo wrd'
```

**⚠️ Attention avec les objets :**

```javascript
// Ceci ne fonctionne PAS pour dédoublonner des objets
const users = [
  { id: 1, name: 'Alice' },
  { id: 2, name: 'Bob' },
  { id: 1, name: 'Alice' }  // Doublon logique mais référence différente
];

const uniqueUsers = [...new Set(users)];
console.log(uniqueUsers.length);  // 3 (toujours 3 objets)

// Solution : utiliser Map avec une clé unique
const uniqueById = [...new Map(users.map(u => [u.id, u])).values()];
console.log(uniqueById.length);  // 2
```

---

### Q4: Comment itérer sur un Set ?

**Réponse :**

Plusieurs méthodes :

```javascript
const fruits = new Set(['apple', 'banana', 'orange']);

// 1. for...of (le plus courant)
for (const fruit of fruits) {
  console.log(fruit);
}
// apple
// banana
// orange

// 2. forEach()
fruits.forEach(fruit => {
  console.log(fruit.toUpperCase());
});
// APPLE
// BANANA
// ORANGE

// ⚠️ forEach a 3 paramètres : (value, valueAgain, set)
fruits.forEach((value, valueAgain, set) => {
  console.log(value === valueAgain);  // true
  // valueAgain = value (pour compatibilité avec Map)
});

// 3. Convertir en array
const fruitsArray = [...fruits];
fruitsArray.forEach((fruit, index) => {
  console.log(`${index + 1}. ${fruit}`);
});
// 1. apple
// 2. banana
// 3. orange

// 4. values() - retourne un iterator
for (const value of fruits.values()) {
  console.log(value);
}

// 5. keys() - identique à values() pour Set
for (const key of fruits.keys()) {
  console.log(key);  // même résultat
}

// 6. entries() - retourne [value, value]
for (const [key, value] of fruits.entries()) {
  console.log(key, value);  // key === value
}
```

**Transformation avec map/filter :**

```javascript
const numbers = new Set([1, 2, 3, 4, 5]);

// Map (transformer)
const doubled = new Set([...numbers].map(n => n * 2));
console.log(doubled);  // Set(5) { 2, 4, 6, 8, 10 }

// Filter (filtrer)
const evens = new Set([...numbers].filter(n => n % 2 === 0));
console.log(evens);  // Set(2) { 2, 4 }

// Reduce
const sum = [...numbers].reduce((acc, n) => acc + n, 0);
console.log(sum);  // 15
```

---

### Q5: Comment effectuer des opérations d'ensemble (union, intersection, différence) avec des Sets ?

**Réponse :**

```javascript
const setA = new Set([1, 2, 3, 4, 5]);
const setB = new Set([4, 5, 6, 7, 8]);

// UNION (tous les éléments des deux sets)
const union = new Set([...setA, ...setB]);
console.log(union);  // Set(8) { 1, 2, 3, 4, 5, 6, 7, 8 }

// INTERSECTION (éléments communs)
const intersection = new Set(
  [...setA].filter(x => setB.has(x))
);
console.log(intersection);  // Set(2) { 4, 5 }

// DIFFÉRENCE (éléments dans A mais pas dans B)
const difference = new Set(
  [...setA].filter(x => !setB.has(x))
);
console.log(difference);  // Set(3) { 1, 2, 3 }

// DIFFÉRENCE SYMÉTRIQUE (dans A ou B mais pas dans les deux)
const symmetricDiff = new Set([
  ...[...setA].filter(x => !setB.has(x)),
  ...[...setB].filter(x => !setA.has(x))
]);
console.log(symmetricDiff);  // Set(6) { 1, 2, 3, 6, 7, 8 }

// Vérifier si un Set est un sous-ensemble d'un autre
function isSubset(subset, superset) {
  return [...subset].every(elem => superset.has(elem));
}

const set1 = new Set([1, 2]);
const set2 = new Set([1, 2, 3, 4]);
console.log(isSubset(set1, set2));  // true
```

**Fonctions utilitaires :**

```javascript
function union(setA, setB) {
  return new Set([...setA, ...setB]);
}

function intersection(setA, setB) {
  return new Set([...setA].filter(x => setB.has(x)));
}

function difference(setA, setB) {
  return new Set([...setA].filter(x => !setB.has(x)));
}

function symmetricDifference(setA, setB) {
  const diff1 = difference(setA, setB);
  const diff2 = difference(setB, setA);
  return union(diff1, diff2);
}
```

---

### Q6: Quelle est la différence entre Set et WeakSet ?

**Réponse :**

**Différences principales :**

```javascript
// SET
const set = new Set();
set.add(1);              // ✅ Accepte tous types
set.add('hello');        // ✅
set.add({ id: 1 });      // ✅
console.log(set.size);   // ✅ Propriété size
for (const item of set) {} // ✅ Itérable
set.clear();             // ✅ Méthode clear()

// WEAKSET
const weakSet = new WeakSet();
// weakSet.add(1);       // ❌ TypeError - seulement objets
// weakSet.add('hello'); // ❌ TypeError
const obj = { id: 1 };
weakSet.add(obj);        // ✅ Objets seulement
// console.log(weakSet.size); // ❌ undefined
// for (const item of weakSet) {} // ❌ TypeError - pas itérable
// weakSet.clear();      // ❌ TypeError
```

**Tableau comparatif :**

| Critère | Set | WeakSet |
|---------|-----|---------|
| Types acceptés | Tous types | Objets uniquement |
| Propriété `size` | ✅ Oui | ❌ Non |
| Itération | ✅ Oui | ❌ Non |
| `clear()` | ✅ Oui | ❌ Non |
| Garbage Collection | Références fortes | Références faibles |
| Méthodes | `add`, `delete`, `has`, `clear`, `size` | `add`, `delete`, `has` |

**Garbage Collection (différence clé) :**

```javascript
let obj = { name: 'Alice' };
const set = new Set();
const weakSet = new WeakSet();

set.add(obj);
weakSet.add(obj);

obj = null;  // On supprime la référence

// Set : l'objet reste en mémoire (référence forte)
// WeakSet : l'objet peut être garbage collected (référence faible)
```

**Cas d'usage de WeakSet :**

```javascript
// Marquer des objets sans empêcher leur suppression
const processedObjects = new WeakSet();

function processData(data) {
  if (processedObjects.has(data)) {
    console.log('Already processed');
    return;
  }
  
  console.log('Processing:', data);
  processedObjects.add(data);
}

let data = { id: 1, value: 'test' };
processData(data);  // Processing: { id: 1, value: 'test' }
processData(data);  // Already processed

// Si data n'est plus utilisé ailleurs, il peut être garbage collected
data = null;
```

**Quand utiliser WeakSet :**
- ✅ Pour éviter les fuites mémoire
- ✅ Pour marquer des objets temporairement
- ✅ Quand vous n'avez pas besoin d'itérer ou de connaître la taille
- ❌ Si vous avez besoin d'itérer sur les éléments
- ❌ Si vous avez besoin de `size` ou `clear()`

---

## Questions sur Map

### Q7: Qu'est-ce qu'un Map et quelle est sa différence principale avec un Object ?

**Réponse :**

Un **Map** est une collection de paires **clé-valeur** où les clés peuvent être de **n'importe quel type** (pas seulement des strings comme pour les objets).

**Différence principale : TYPES DE CLÉS**

```javascript
// OBJECT - clés converties en strings
const obj = {};
obj[1] = 'number';
obj['1'] = 'string';
console.log(obj[1]);  // 'string' (écrasé car 1 === '1')
console.log(Object.keys(obj));  // ['1'] - une seule clé

// MAP - clés gardent leur type
const map = new Map();
map.set(1, 'number');
map.set('1', 'string');
console.log(map.get(1));   // 'number'
console.log(map.get('1')); // 'string'
console.log(map.size);     // 2 - deux clés distinctes

// Map accepte N'IMPORTE QUEL type de clé
const mixedKeys = new Map();
mixedKeys.set('string', 1);           // String
mixedKeys.set(42, 2);                  // Number
mixedKeys.set(true, 3);                // Boolean
mixedKeys.set({ id: 1 }, 4);          // Object
mixedKeys.set([1, 2], 5);             // Array
mixedKeys.set(() => {}, 6);           // Function

console.log(mixedKeys.size);  // 6
```

**Autres différences importantes :**

```javascript
// 1. ORDRE DES CLÉS
// Object : ordre non garanti
const obj2 = { z: 1, a: 2, m: 3 };
console.log(Object.keys(obj2));  // Ordre peut varier

// Map : ordre d'insertion garanti
const map2 = new Map();
map2.set('z', 1);
map2.set('a', 2);
map2.set('m', 3);
console.log([...map2.keys()]);  // ['z', 'a', 'm'] - ordre d'insertion

// 2. TAILLE
// Object : calculer manuellement (O(n))
console.log(Object.keys(obj2).length);  // 3

// Map : propriété size (O(1))
console.log(map2.size);  // 3

// 3. ITÉRATION
// Object : nécessite Object.entries()
for (const [key, value] of Object.entries(obj2)) {
  console.log(key, value);
}

// Map : itérable nativement
for (const [key, value] of map2) {
  console.log(key, value);
}

// 4. POLLUTION DU PROTOTYPE
// Object : hérite de Object.prototype
const obj3 = {};
console.log('toString' in obj3);  // true (hérité)

// Map : pas de pollution
const map3 = new Map();
console.log(map3.get('toString'));  // undefined
```

---

### Q8: Comment ajouter, récupérer et supprimer des éléments dans un Map ?

**Réponse :**

```javascript
const users = new Map();

// AJOUTER - set(key, value)
users.set('alice', { age: 25, city: 'Paris' });
users.set('bob', { age: 30, city: 'London' });
users.set('charlie', { age: 35, city: 'New York' });

console.log(users.size);  // 3

// set() retourne le Map (permet le chaining)
users
  .set('dave', { age: 40, city: 'Tokyo' })
  .set('eve', { age: 28, city: 'Berlin' });

console.log(users.size);  // 5

// RÉCUPÉRER - get(key)
const alice = users.get('alice');
console.log(alice);  // { age: 25, city: 'Paris' }

// Clé inexistante retourne undefined
const unknown = users.get('frank');
console.log(unknown);  // undefined

// VÉRIFIER - has(key)
console.log(users.has('alice'));  // true
console.log(users.has('frank'));  // false

// SUPPRIMER - delete(key)
users.delete('bob');
console.log(users.has('bob'));  // false

// delete() retourne true si la clé existait
console.log(users.delete('charlie'));  // true
console.log(users.delete('xyz'));      // false

// VIDER - clear()
users.clear();
console.log(users.size);  // 0

// TAILLE - size
const config = new Map([
  ['host', 'localhost'],
  ['port', 3000]
]);
console.log(config.size);  // 2
```

**Avec objets comme clés (attention aux références) :**

```javascript
const user1 = { id: 1, name: 'Alice' };
const user2 = { id: 2, name: 'Bob' };

const scores = new Map();
scores.set(user1, 95);
scores.set(user2, 87);

console.log(scores.get(user1));  // 95

// ⚠️ Nouvelle référence = différente clé
console.log(scores.get({ id: 1 }));  // undefined (pas la même référence)

// Pour récupérer, il faut la même référence
const key = { id: 1 };
scores.set(key, 100);
console.log(scores.get(key));  // 100 ✅
```

---

### Q9: Comment itérer sur un Map ?

**Réponse :**

```javascript
const users = new Map([
  ['alice', { age: 25, city: 'Paris' }],
  ['bob', { age: 30, city: 'London' }],
  ['charlie', { age: 35, city: 'New York' }]
]);

// 1. for...of (le plus courant)
for (const [key, value] of users) {
  console.log(`${key}:`, value);
}
// alice: { age: 25, city: 'Paris' }
// bob: { age: 30, city: 'London' }
// charlie: { age: 35, city: 'New York' }

// 2. entries() explicite (identique à for...of)
for (const [username, info] of users.entries()) {
  console.log(`${username} is ${info.age} years old`);
}

// 3. keys() - seulement les clés
for (const key of users.keys()) {
  console.log(key);
}
// alice
// bob
// charlie

// 4. values() - seulement les valeurs
for (const value of users.values()) {
  console.log(value);
}
// { age: 25, city: 'Paris' }
// { age: 30, city: 'London' }
// { age: 35, city: 'New York' }

// 5. forEach()
users.forEach((value, key, map) => {
  console.log(`${key}: ${value.city}`);
});
// alice: Paris
// bob: London
// charlie: New York

// ⚠️ Ordre des paramètres : (value, key) - différent de Object.entries()
```

**Transformation avec Array methods :**

```javascript
// Convertir en array
const entries = [...users];
console.log(entries);
// [['alice', {...}], ['bob', {...}], ['charlie', {...}]]

const keysArray = [...users.keys()];
console.log(keysArray);  // ['alice', 'bob', 'charlie']

const valuesArray = [...users.values()];

// Filtrer un Map
const adults = new Map(
  [...users].filter(([key, value]) => value.age >= 30)
);
console.log(adults.size);  // 2

// Transformer les valeurs
const ages = new Map(
  [...users].map(([key, value]) => [key, value.age])
);
console.log(ages);
// Map(3) { 'alice' => 25, 'bob' => 30, 'charlie' => 35 }

// Réduire
const totalAge = [...users.values()].reduce((sum, user) => sum + user.age, 0);
console.log(totalAge);  // 90
```

---

### Q10: Quand utiliser Map au lieu d'Object ?

**Réponse :**

**✅ Utilisez MAP quand :**

1. **Les clés ne sont pas des strings**
```javascript
// Clés avec des types différents
const cache = new Map();
cache.set(1, 'one');
cache.set('1', 'ONE');
cache.set(true, 'yes');

// Objets comme clés
const user1 = { id: 1 };
const userScores = new Map();
userScores.set(user1, 95);
```

2. **Vous devez préserver l'ordre d'insertion**
```javascript
const ordered = new Map();
ordered.set('z', 1);
ordered.set('a', 2);
ordered.set('m', 3);
console.log([...ordered.keys()]);  // ['z', 'a', 'm'] - ordre garanti
```

3. **Ajouts/suppressions fréquents**
```javascript
// Map est plus rapide pour add/delete
const map = new Map();
for (let i = 0; i < 1000000; i++) {
  map.set(i, i * 2);
}
// Plus rapide qu'avec Object
```

4. **Vous devez itérer souvent**
```javascript
// Map est nativement itérable
for (const [key, value] of map) {
  console.log(key, value);
}
```

5. **Vous avez besoin de la taille facilement**
```javascript
console.log(map.size);  // O(1) - instant
console.log(Object.keys(obj).length);  // O(n) - doit compter
```

**✅ Utilisez OBJECT quand :**

1. **Toutes les clés sont des strings**
```javascript
const config = {
  host: 'localhost',
  port: 3000,
  ssl: false
};
```

2. **Vous avez besoin de JSON.stringify/parse**
```javascript
const obj = { name: 'Alice', age: 25 };
JSON.stringify(obj);  // '{"name":"Alice","age":25}'

const map = new Map([['name', 'Alice']]);
JSON.stringify(map);  // '{}' - ne fonctionne pas
```

3. **Structure de données fixe**
```javascript
const user = {
  id: 1,
  name: 'Alice',
  email: 'alice@email.com'
};
// Structure connue à l'avance
```

4. **Accès par propriété statique**
```javascript
console.log(config.host);  // Simple et lisible
console.log(map.get('host'));  // Plus verbeux
```

5. **Petite collection**
```javascript
// Pour 5-10 propriétés, Object est suffisant
const settings = { theme: 'dark', fontSize: 16 };
```

**Exemple pratique de choix :**

```javascript
// ❌ Mauvais : Object avec clés dynamiques
const cache = {};
function cacheData(user, data) {
  cache[user] = data;  // user converti en string '[object Object]'
}

// ✅ Bon : Map avec objets comme clés
const cache = new Map();
function cacheData(user, data) {
  cache.set(user, data);  // user garde son identité
}
```

---

### Q11: Quelle est la différence entre Map et WeakMap ?

**Réponse :**

**Différences principales :**

```javascript
// MAP
const map = new Map();
map.set('key', 'value');         // ✅ Accepte tous types de clés
map.set(1, 'one');                // ✅
const obj = { id: 1 };
map.set(obj, 'data');             // ✅
console.log(map.size);            // ✅ Propriété size
for (const [k, v] of map) {}     // ✅ Itérable
map.clear();                      // ✅ Méthode clear()

// WEAKMAP
const weakMap = new WeakMap();
// weakMap.set('key', 'value');  // ❌ TypeError - seulement objets
// weakMap.set(1, 'one');         // ❌ TypeError
const obj2 = { id: 2 };
weakMap.set(obj2, 'data');        // ✅ Objets seulement comme clés
// console.log(weakMap.size);     // ❌ undefined
// for (const [k, v] of weakMap){} // ❌ TypeError - pas itérable
// weakMap.clear();               // ❌ TypeError
```

**Tableau comparatif :**

| Critère | Map | WeakMap |
|---------|-----|---------|
| Types de clés | Tous types | Objets uniquement |
| Propriété `size` | ✅ Oui | ❌ Non |
| Itération | ✅ Oui | ❌ Non |
| `clear()` | ✅ Oui | ❌ Non |
| Garbage Collection | Références fortes | Références faibles |
| Méthodes | `set`, `get`, `has`, `delete`, `clear`, `size`, `keys`, `values`, `entries` | `set`, `get`, `has`, `delete` |

**Garbage Collection (différence clé) :**

```javascript
let user = { name: 'Alice' };
const map = new Map();
const weakMap = new WeakMap();

map.set(user, 'some data');
weakMap.set(user, 'some data');

// user est référencé par :
// - la variable 'user'
// - map (référence forte)
// - weakMap (référence faible)

user = null;  // On supprime la référence

// Map : l'objet reste en mémoire (référence forte)
// → l'objet NE SERA PAS garbage collected

// WeakMap : l'objet peut être garbage collected (référence faible)
// → l'objet peut être supprimé de la mémoire
```

**Cas d'usage de WeakMap :**

```javascript
// 1. Données privées pour des classes
const privateData = new WeakMap();

class User {
  constructor(name, password) {
    this.name = name;
    // Stocker password de manière "privée"
    privateData.set(this, { password });
  }
  
  checkPassword(input) {
    const data = privateData.get(this);
    return data.password === input;
  }
}

const user = new User('Alice', 'secret123');
console.log(user.password);  // undefined (pas accessible)
console.log(user.checkPassword('secret123'));  // true

// Quand user est supprimé, ses données privées le sont aussi

// 2. Métadonnées sur des éléments DOM
const domMetadata = new WeakMap();

function attachMetadata(element, data) {
  domMetadata.set(element, data);
}

const button = document.querySelector('button');
attachMetadata(button, { clickCount: 0, createdAt: new Date() });

// Si le bouton est retiré du DOM, ses métadonnées sont garbage collected

// 3. Cache sans fuite mémoire
const cache = new WeakMap();

function processData(obj) {
  if (cache.has(obj)) {
    return cache.get(obj);
  }
  
  const result = expensiveOperation(obj);
  cache.set(obj, result);
  return result;
}

// Quand obj n'est plus utilisé, il est automatiquement nettoyé du cache
```

**Quand utiliser WeakMap :**
- ✅ Pour éviter les fuites mémoire
- ✅ Pour stocker des métadonnées sur des objets
- ✅ Pour des caches temporaires
- ✅ Pour des données privées
- ❌ Si vous avez besoin d'itérer
- ❌ Si vous avez besoin de `size` ou `clear()`

---

### Q12: Comment convertir entre Map, Object et Array ?

**Réponse :**

```javascript
// OBJECT → MAP
const obj = {
  name: 'Alice',
  age: 25,
  city: 'Paris'
};

const mapFromObj = new Map(Object.entries(obj));
console.log(mapFromObj);
// Map(3) { 'name' => 'Alice', 'age' => 25, 'city' => 'Paris' }

// MAP → OBJECT
const map = new Map([
  ['name', 'Bob'],
  ['age', 30],
  ['city', 'London']
]);

const objFromMap = Object.fromEntries(map);
console.log(objFromMap);
// { name: 'Bob', age: 30, city: 'London' }

// MAP → ARRAY
const entries = [...map];
// ou
const entries2 = Array.from(map);
console.log(entries);
// [['name', 'Bob'], ['age', 30], ['city', 'London']]

const keys = [...map.keys()];
console.log(keys);  // ['name', 'age', 'city']

const values = [...map.values()];
console.log(values);  // ['Bob', 30, 'London']

// ARRAY → MAP
const arr = [['a', 1], ['b', 2], ['c', 3]];
const mapFromArr = new Map(arr);
console.log(mapFromArr);
// Map(3) { 'a' => 1, 'b' => 2, 'c' => 3 }

// CONVERSIONS AVEC TRANSFORMATION
// Filtrer et convertir
const filtered = Object.fromEntries(
  [...map].filter(([key, value]) => typeof value === 'string')
);
console.log(filtered);  // { name: 'Bob', city: 'London' }

// Transformer et convertir
const transformed = Object.fromEntries(
  [...map].map(([key, value]) => [key.toUpperCase(), value])
);
console.log(transformed);
// { NAME: 'Bob', AGE: 30, CITY: 'London' }
```

---

## Questions Pièges

### Q13: Que se passe-t-il avec ce code ?

```javascript
const map = new Map();
const obj = { id: 1 };
map.set(obj, 'value');
console.log(map.get({ id: 1 }));  // ?
```

**Réponse :**

```javascript
const map = new Map();
const obj = { id: 1 };
map.set(obj, 'value');
console.log(map.get({ id: 1 }));  // undefined

// Pourquoi ? { id: 1 } est un NOUVEL objet (différente référence)
// Map compare les clés par RÉFÉRENCE, pas par valeur

// Pour que ça fonctionne :
console.log(map.get(obj));  // 'value' ✅ (même référence)

// Démonstration
const key1 = { id: 1 };
const key2 = { id: 1 };
console.log(key1 === key2);  // false (références différentes)

map.set(key1, 'value1');
map.set(key2, 'value2');
console.log(map.size);  // 2 (deux entrées différentes)
```

**Solution si vous voulez comparer par valeur :**

```javascript
// Utiliser un ID unique comme clé
const users = new Map();
const user = { id: 1, name: 'Alice' };

users.set(user.id, user);  // Utiliser l'ID comme clé
console.log(users.get(1));  // { id: 1, name: 'Alice' } ✅

// Ou convertir l'objet en string JSON (attention aux limitations)
const map2 = new Map();
const obj = { id: 1, name: 'Alice' };
const key = JSON.stringify(obj);
map2.set(key, 'value');
console.log(map2.get(JSON.stringify({ id: 1, name: 'Alice' })));  // 'value'
```

---

### Q14: Pourquoi `Set.size` est-il une propriété et pas une méthode ?

**Réponse :**

```javascript
const set = new Set([1, 2, 3]);

// ✅ Correct
console.log(set.size);  // 3

// ❌ Incorrect
// console.log(set.size());  // TypeError: set.size is not a function

// Pourquoi une propriété ?
// - Cohérence avec Map (Map.size aussi)
// - Performance : O(1) - pas besoin de calculer
// - Différence avec Array.length (qui est aussi une propriété)
```

**Différence avec Array :**

```javascript
const arr = [1, 2, 3];
const set = new Set([1, 2, 3]);

// Array.length est une propriété modifiable
console.log(arr.length);  // 3
arr.length = 10;
console.log(arr.length);  // 10 ✅ (modifié)

// Set.size est une propriété en lecture seule
console.log(set.size);  // 3
set.size = 10;  // Ignoré en mode non-strict, TypeError en strict
console.log(set.size);  // 3 (inchangé)
```

---

### Q15: Peut-on utiliser NaN comme clé dans un Set ou un Map ?

**Réponse :**

**OUI**, et c'est traité de manière spéciale :

```javascript
// Set avec NaN
const set = new Set();
set.add(NaN);
set.add(NaN);  // Doublon ?
console.log(set.size);  // 1 (un seul NaN)

console.log(set.has(NaN));  // true

// Map avec NaN comme clé
const map = new Map();
map.set(NaN, 'value1');
map.set(NaN, 'value2');  // Écrase
console.log(map.size);  // 1
console.log(map.get(NaN));  // 'value2'

// ⚠️ Comportement spécial car NaN !== NaN en JavaScript
console.log(NaN === NaN);  // false
console.log(NaN == NaN);   // false

// Mais Set et Map traitent NaN comme égal à lui-même
```

**Avec des objets (différent) :**

```javascript
const set = new Set();
set.add({});
set.add({});
console.log(set.size);  // 2 (deux objets différents)

// Comparaison
console.log({} === {});  // false (références différentes)
console.log(NaN === NaN);  // false (mais traité spécialement par Set/Map)
```

---

### Q16: Quelle est la différence de performance entre Set.has() et Array.includes() ?

**Réponse :**

**Set.has() est BEAUCOUP plus rapide pour grandes collections :**

```javascript
// Créer de grandes collections
const arrayLarge = Array.from({ length: 100000 }, (_, i) => i);
const setLarge = new Set(arrayLarge);

// Rechercher le dernier élément
console.time('Array.includes');
arrayLarge.includes(99999);
console.timeEnd('Array.includes');  // ~1-2ms

console.time('Set.has');
setLarge.has(99999);
console.timeEnd('Set.has');  // ~0.001ms (beaucoup plus rapide !)

// Complexité algorithmique
// Array.includes() : O(n) - parcourt tout l'array dans le pire cas
// Set.has() : O(1) - accès direct via hash table
```

**Quand utiliser quoi :**

```javascript
// ❌ Mauvais : recherches fréquentes dans un array
const blacklist = ['spam', 'fake', 'scam', 'phishing'];

function isBlacklisted(word) {
  return blacklist.includes(word.toLowerCase());  // O(n) à chaque fois
}

// ✅ Bon : convertir en Set pour recherches fréquentes
const blacklistSet = new Set(['spam', 'fake', 'scam', 'phishing']);

function isBlacklisted(word) {
  return blacklistSet.has(word.toLowerCase());  // O(1)
}

// Benchmark
const words = Array.from({ length: 10000 }, () => 
  Math.random() > 0.5 ? 'valid' : 'spam'
);

console.time('With Array');
words.forEach(word => blacklist.includes(word));
console.timeEnd('With Array');  // ~50ms

console.time('With Set');
words.forEach(word => blacklistSet.has(word));
console.timeEnd('With Set');  // ~1ms
```

**Règle générale :**
- Petite collection (< 50 éléments) : Array ou Set, peu de différence
- Grande collection + recherches fréquentes : Set est beaucoup mieux
- Besoin d'ordre + index : Array
- Besoin d'unicité : Set

---

### Q17: Peut-on avoir une fonction comme clé dans un Map ?

**Réponse :**

**OUI, Map accepte n'importe quel type de clé, y compris les fonctions :**

```javascript
const map = new Map();

// Fonctions comme clés
function fn1() { return 1; }
function fn2() { return 2; }

map.set(fn1, 'function 1');
map.set(fn2, 'function 2');

console.log(map.get(fn1));  // 'function 1'
console.log(map.get(fn2));  // 'function 2'
console.log(map.size);      // 2

// Arrow functions aussi
const arrow1 = () => 'a';
const arrow2 = () => 'b';

map.set(arrow1, 'arrow 1');
map.set(arrow2, 'arrow 2');

console.log(map.get(arrow1));  // 'arrow 1'

// ⚠️ Attention aux références (comme pour les objets)
map.set(() => 'test', 'value');
console.log(map.get(() => 'test'));  // undefined (différente fonction)

// Cas d'usage : memoization de fonctions
const memoCache = new Map();

function memoize(fn) {
  return function(...args) {
    if (!memoCache.has(fn)) {
      memoCache.set(fn, new Map());
    }
    
    const fnCache = memoCache.get(fn);
    const key = JSON.stringify(args);
    
    if (fnCache.has(key)) {
      console.log('Using cache');
      return fnCache.get(key);
    }
    
    console.log('Computing');
    const result = fn(...args);
    fnCache.set(key, result);
    return result;
  };
}

const slowFunction = (n) => {
  // Opération lente
  return n * 2;
};

const memoized = memoize(slowFunction);
console.log(memoized(5));  // Computing → 10
console.log(memoized(5));  // Using cache → 10
```

---

### Q18: Comment vider un Set ou Map rapidement ?

**Réponse :**

Plusieurs méthodes :

```javascript
// SET
const set = new Set([1, 2, 3, 4, 5]);

// Méthode 1 : clear() (la plus simple)
set.clear();
console.log(set.size);  // 0

// Méthode 2 : créer un nouveau Set (rapide)
let set2 = new Set([1, 2, 3, 4, 5]);
set2 = new Set();  // Nouveau Set vide
console.log(set2.size);  // 0

// Méthode 3 : delete en boucle (lent, pas recommandé)
const set3 = new Set([1, 2, 3, 4, 5]);
for (const item of set3) {
  set3.delete(item);
}
console.log(set3.size);  // 0

// MAP
const map = new Map([['a', 1], ['b', 2]]);

// Méthode 1 : clear() (la plus simple)
map.clear();
console.log(map.size);  // 0

// Méthode 2 : créer un nouveau Map
let map2 = new Map([['a', 1], ['b', 2]]);
map2 = new Map();
console.log(map2.size);  // 0

// Performance : clear() vs nouveau Set/Map
const largeSet = new Set(Array.from({ length: 1000000 }, (_, i) => i));

console.time('clear()');
largeSet.clear();
console.timeEnd('clear()');  // ~1ms

let largeSet2 = new Set(Array.from({ length: 1000000 }, (_, i) => i));

console.time('new Set()');
largeSet2 = new Set();
console.timeEnd('new Set()');  // ~0.001ms (plus rapide)
```

**Recommandation :**
- `clear()` : si vous réutilisez le même Set/Map
- Nouveau Set/Map : si vous créez une nouvelle variable (plus rapide)

---

### Q19: Peut-on utiliser des Symbols comme clés dans Set ou Map ?

**Réponse :**

**OUI, Symbols fonctionnent parfaitement :**

```javascript
// SET avec Symbols
const set = new Set();
const sym1 = Symbol('id');
const sym2 = Symbol('id');

set.add(sym1);
set.add(sym2);  // Différent même si description identique
console.log(set.size);  // 2

console.log(set.has(sym1));  // true
console.log(set.has(sym2));  // true
console.log(set.has(Symbol('id')));  // false (nouveau Symbol)

// MAP avec Symbols comme clés
const map = new Map();
const key1 = Symbol('user');
const key2 = Symbol('admin');

map.set(key1, { name: 'Alice' });
map.set(key2, { name: 'Bob' });

console.log(map.get(key1));  // { name: 'Alice' }
console.log(map.size);       // 2

// Symbol.for() crée des Symbols globaux réutilisables
const globalSym = Symbol.for('app.user');
map.set(globalSym, 'global value');

console.log(map.get(Symbol.for('app.user')));  // 'global value' ✅
// Fonctionne car Symbol.for() retourne le même Symbol

// Cas d'usage : clés privées/cachées
const PRIVATE_KEY = Symbol('private');

const config = new Map();
config.set('public', 'visible');
config.set(PRIVATE_KEY, 'hidden');

// Itérer sur Map montre TOUTES les clés, y compris Symbols
for (const [key, value] of config) {
  console.log(typeof key, value);
}
// string visible
// symbol hidden

// Mais Object.keys() ignore les Symbols (différence avec Object)
const obj = {
  public: 'visible',
  [PRIVATE_KEY]: 'hidden'
};

console.log(Object.keys(obj));  // ['public'] - Symbol ignoré
```

---

### Q20: Comment implémenter un cache LRU (Least Recently Used) avec Map ?

**Réponse :**

Un **LRU Cache** garde les éléments les plus récemment utilisés et éjecte les plus anciens quand il est plein. Map est parfait car il garde l'ordre d'insertion.

```javascript
class LRUCache {
  constructor(maxSize = 100) {
    this.cache = new Map();
    this.maxSize = maxSize;
  }
  
  get(key) {
    if (!this.cache.has(key)) {
      return undefined;
    }
    
    // Déplacer à la fin (le plus récent)
    const value = this.cache.get(key);
    this.cache.delete(key);
    this.cache.set(key, value);
    
    return value;
  }
  
  set(key, value) {
    // Si la clé existe déjà, la supprimer
    if (this.cache.has(key)) {
      this.cache.delete(key);
    }
    
    // Ajouter à la fin (le plus récent)
    this.cache.set(key, value);
    
    // Si dépassement, supprimer le plus ancien (premier)
    if (this.cache.size > this.maxSize) {
      const firstKey = this.cache.keys().next().value;
      this.cache.delete(firstKey);
    }
  }
  
  has(key) {
    return this.cache.has(key);
  }
  
  clear() {
    this.cache.clear();
  }
  
  get size() {
    return this.cache.size;
  }
  
  // Méthode debug
  print() {
    console.log('Cache (newest → oldest):');
    const entries = [...this.cache.entries()].reverse();
    entries.forEach(([key, value]) => {
      console.log(`  ${key}: ${value}`);
    });
  }
}

// Test
const cache = new LRUCache(3);

cache.set('a', 1);
cache.set('b', 2);
cache.set('c', 3);
cache.print();
// Cache: a→1, b→2, c→3

cache.set('d', 4);  // 'a' est éjecté (le plus ancien)
cache.print();
// Cache: b→2, c→3, d→4

console.log(cache.has('a'));  // false (éjecté)

cache.get('b');  // Accès à 'b' → devient le plus récent
cache.set('e', 5);  // 'c' est éjecté (pas 'b')
cache.print();
// Cache: b→2, d→4, e→5

console.log(cache.has('c'));  // false (éjecté)
console.log(cache.has('b'));  // true (toujours là car accédé récemment)
```

**Cas d'usage :**

```javascript
// Cache d'API
const apiCache = new LRUCache(50);

async function fetchUser(userId) {
  if (apiCache.has(userId)) {
    console.log('Cache hit');
    return apiCache.get(userId);
  }
  
  console.log('Cache miss - fetching from API');
  const response = await fetch(`/api/users/${userId}`);
  const user = await response.json();
  
  apiCache.set(userId, user);
  return user;
}

// Premier appel : cache miss
await fetchUser(1);  // Cache miss - fetching from API

// Deuxième appel : cache hit
await fetchUser(1);  // Cache hit
```

---
