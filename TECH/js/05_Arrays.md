## [**..**](./00_index.md)

# Chapitre 5 : Arrays (Tableaux)

## 5.1 Bases des Arrays

### Création et accès aux éléments

```javascript
// Création d'un array (méthode literal)
const fruits = ['pomme', 'banane', 'orange'];

// Création avec constructor (moins courant)
const numbers = new Array(1, 2, 3, 4, 5);

// Array vide
const empty = [];

// Array avec taille pré-définie (crée des "holes")
const arr = new Array(5); // [empty × 5]

// Accès aux éléments (zero-indexed)
console.log(fruits[0]);  // 'pomme' - premier élément
console.log(fruits[1]);  // 'banane' - deuxième élément
console.log(fruits[2]);  // 'orange' - troisième élément

// Accès au dernier élément
console.log(fruits[fruits.length - 1]);  // 'orange'

// Modifier un élément
fruits[1] = 'fraise';
console.log(fruits);  // ['pomme', 'fraise', 'orange']

// Ajouter un élément à un index spécifique
fruits[3] = 'kiwi';
console.log(fruits);  // ['pomme', 'fraise', 'orange', 'kiwi']

// Accès hors limites retourne undefined
console.log(fruits[10]);  // undefined
```

### Propriété `length`

```javascript
const colors = ['rouge', 'vert', 'bleu'];

// Obtenir la longueur
console.log(colors.length);  // 3

// Modifier la longueur pour tronquer l'array
colors.length = 2;
console.log(colors);  // ['rouge', 'vert']

// Vider complètement un array
colors.length = 0;
console.log(colors);  // []

// Étendre un array (crée des holes)
const nums = [1, 2, 3];
nums.length = 5;
console.log(nums);  // [1, 2, 3, empty × 2]
```

### Arrays multidimensionnels

```javascript
// Array 2D (matrice)
const matrix = [
  [1, 2, 3],
  [4, 5, 6],
  [7, 8, 9]
];

// Accès aux éléments
console.log(matrix[0][0]);  // 1 - première ligne, première colonne
console.log(matrix[1][2]);  // 6 - deuxième ligne, troisième colonne
console.log(matrix[2][1]);  // 8 - troisième ligne, deuxième colonne

// Array 3D
const cube = [
  [[1, 2], [3, 4]],
  [[5, 6], [7, 8]]
];

console.log(cube[0][1][0]);  // 3

// Parcourir une matrice avec nested loops
for (let i = 0; i < matrix.length; i++) {
  for (let j = 0; j < matrix[i].length; j++) {
    console.log(`Element [${i}][${j}] = ${matrix[i][j]}`);
  }
}

// Exemple pratique : grille de jeu
const grid = [
  ['X', 'O', 'X'],
  ['O', 'X', 'O'],
  ['O', 'X', 'X']
];
```

---

## 5.2 Array Methods Essentiels

### Ajout/Suppression d'éléments

#### `push()` - Ajouter à la fin

```javascript
const stack = [1, 2, 3];

// Ajouter un élément
const newLength = stack.push(4);
console.log(stack);      // [1, 2, 3, 4]
console.log(newLength);  // 4 - retourne la nouvelle longueur

// Ajouter plusieurs éléments
stack.push(5, 6, 7);
console.log(stack);  // [1, 2, 3, 4, 5, 6, 7]

// Usage typique : construction d'un array
const result = [];
for (let i = 1; i <= 5; i++) {
  result.push(i * 2);
}
console.log(result);  // [2, 4, 6, 8, 10]
```

#### `pop()` - Supprimer le dernier

```javascript
const items = ['a', 'b', 'c', 'd'];

// Supprimer et récupérer le dernier élément
const last = items.pop();
console.log(last);   // 'd'
console.log(items);  // ['a', 'b', 'c']

// Vider un array avec pop()
while (items.length > 0) {
  const removed = items.pop();
  console.log(`Removed: ${removed}`);
}
console.log(items);  // []

// Simuler une stack (LIFO - Last In First Out)
const stack = [];
stack.push('first');   // ajouter
stack.push('second');
stack.push('third');
console.log(stack.pop());  // 'third' - retire le dernier ajouté
```

#### `unshift()` - Ajouter au début

```javascript
const queue = [2, 3, 4];

// Ajouter au début
queue.unshift(1);
console.log(queue);  // [1, 2, 3, 4]

// Ajouter plusieurs éléments
queue.unshift(-1, 0);
console.log(queue);  // [-1, 0, 1, 2, 3, 4]

// ⚠️ Performance: unshift() est plus lent que push()
// car il doit réindexer tous les éléments
```

#### `shift()` - Supprimer le premier

```javascript
const line = ['first', 'second', 'third'];

// Supprimer le premier élément
const first = line.shift();
console.log(first);  // 'first'
console.log(line);   // ['second', 'third']

// Simuler une queue (FIFO - First In First Out)
const queue = [];
queue.push('Alice');   // ajouter à la fin
queue.push('Bob');
queue.push('Charlie');
console.log(queue.shift());  // 'Alice' - retire le premier
console.log(queue.shift());  // 'Bob'
```

#### `splice()` - Ajouter/Supprimer à n'importe quelle position

```javascript
// splice(start, deleteCount, ...itemsToAdd)

const arr = [1, 2, 3, 4, 5];

// Supprimer des éléments
const removed = arr.splice(2, 2);  // À partir de l'index 2, supprimer 2 éléments
console.log(removed);  // [3, 4]
console.log(arr);      // [1, 2, 5]

// Ajouter sans supprimer (deleteCount = 0)
arr.splice(2, 0, 'a', 'b', 'c');
console.log(arr);  // [1, 2, 'a', 'b', 'c', 5]

// Remplacer des éléments
const fruits = ['pomme', 'banane', 'orange', 'kiwi'];
fruits.splice(1, 2, 'fraise', 'mangue');
console.log(fruits);  // ['pomme', 'fraise', 'mangue', 'kiwi']

// Supprimer tout à partir d'un index
const nums = [1, 2, 3, 4, 5];
nums.splice(2);  // Supprime à partir de l'index 2
console.log(nums);  // [1, 2]

// Index négatifs (compte depuis la fin)
const letters = ['a', 'b', 'c', 'd', 'e'];
letters.splice(-2, 1);  // Supprime l'avant-dernier
console.log(letters);  // ['a', 'b', 'c', 'e']
```

---

### Recherche d'éléments

#### `indexOf()` et `lastIndexOf()`

```javascript
const numbers = [1, 2, 3, 2, 1];

// indexOf() - trouve la première occurrence
console.log(numbers.indexOf(2));     // 1
console.log(numbers.indexOf(5));     // -1 (non trouvé)
console.log(numbers.indexOf(1));     // 0 (premier 1)

// lastIndexOf() - trouve la dernière occurrence
console.log(numbers.lastIndexOf(1)); // 4 (dernier 1)
console.log(numbers.lastIndexOf(2)); // 3

// Avec un fromIndex (position de départ)
console.log(numbers.indexOf(2, 2));  // 3 (cherche à partir de l'index 2)

// Vérifier si un élément existe
if (numbers.indexOf(3) !== -1) {
  console.log('3 est présent dans l\'array');
}

// Cas pratique : compter les occurrences
function countOccurrences(arr, value) {
  let count = 0;
  let index = arr.indexOf(value);
  
  while (index !== -1) {
    count++;
    index = arr.indexOf(value, index + 1);
  }
  
  return count;
}

console.log(countOccurrences([1, 2, 1, 3, 1, 4], 1));  // 3
```

#### `includes()` - Vérifier l'existence (ES2016)

```javascript
const fruits = ['pomme', 'banane', 'orange'];

// Vérification simple
console.log(fruits.includes('banane'));  // true
console.log(fruits.includes('kiwi'));    // false

// Avec fromIndex
console.log(fruits.includes('pomme', 1)); // false (cherche à partir de l'index 1)

// ✅ Préférer includes() pour vérifier l'existence
// Plus lisible que indexOf() !== -1
if (fruits.includes('pomme')) {
  console.log('Pomme trouvée !');
}

// Gère bien NaN (contrairement à indexOf)
const values = [1, 2, NaN, 4];
console.log(values.includes(NaN));  // true
console.log(values.indexOf(NaN));   // -1

// Case sensitive avec strings
const names = ['Alice', 'Bob', 'Charlie'];
console.log(names.includes('alice'));  // false
```

#### `find()` - Trouver un élément avec condition

```javascript
const users = [
  { id: 1, name: 'Alice', age: 25 },
  { id: 2, name: 'Bob', age: 30 },
  { id: 3, name: 'Charlie', age: 35 }
];

// Trouver le premier utilisateur de plus de 28 ans
const user = users.find(u => u.age > 28);
console.log(user);  // { id: 2, name: 'Bob', age: 30 }

// Retourne undefined si rien n'est trouvé
const notFound = users.find(u => u.age > 40);
console.log(notFound);  // undefined

// Trouver par ID
const userById = users.find(u => u.id === 2);
console.log(userById.name);  // 'Bob'

// Avec index et array (rarement utilisés)
const result = users.find((user, index, array) => {
  console.log(`Checking index ${index}`);
  return user.name === 'Charlie';
});

// Cas pratique : recherche complexe
const products = [
  { name: 'Laptop', price: 1000, inStock: true },
  { name: 'Phone', price: 500, inStock: false },
  { name: 'Tablet', price: 300, inStock: true }
];

const affordableProduct = products.find(p => p.price < 400 && p.inStock);
console.log(affordableProduct);  // { name: 'Tablet', price: 300, inStock: true }
```

#### `findIndex()` - Trouver l'index avec condition

```javascript
const numbers = [5, 12, 8, 130, 44];

// Trouver l'index du premier nombre > 10
const index = numbers.findIndex(n => n > 10);
console.log(index);  // 1

// Retourne -1 si rien n'est trouvé
const notFound = numbers.findIndex(n => n > 200);
console.log(notFound);  // -1

// Cas pratique : mise à jour d'un élément dans un array d'objets
const tasks = [
  { id: 1, title: 'Task 1', done: false },
  { id: 2, title: 'Task 2', done: false },
  { id: 3, title: 'Task 3', done: false }
];

function toggleTask(taskId) {
  const index = tasks.findIndex(t => t.id === taskId);
  
  if (index !== -1) {
    tasks[index].done = !tasks[index].done;
  }
}

toggleTask(2);
console.log(tasks[1]);  // { id: 2, title: 'Task 2', done: true }
```

---

### Transformation d'arrays

#### `slice()` - Extraire une portion (shallow copy)

```javascript
const original = ['a', 'b', 'c', 'd', 'e'];

// Extraire du début à la fin (non incluse)
const sliced = original.slice(1, 4);
console.log(sliced);    // ['b', 'c', 'd']
console.log(original);  // ['a', 'b', 'c', 'd', 'e'] - inchangé

// Sans paramètre de fin (jusqu'à la fin)
console.log(original.slice(2));  // ['c', 'd', 'e']

// Avec index négatifs (compte depuis la fin)
console.log(original.slice(-2));     // ['d', 'e'] - 2 derniers
console.log(original.slice(1, -1));  // ['b', 'c', 'd'] - sans premier et dernier

// Copier un array (shallow copy)
const copy = original.slice();
console.log(copy);  // ['a', 'b', 'c', 'd', 'e']

// ⚠️ Attention avec les objets (shallow copy)
const users = [
  { name: 'Alice', age: 25 },
  { name: 'Bob', age: 30 }
];

const usersCopy = users.slice();
usersCopy[0].age = 99;  // Modifie aussi l'original !
console.log(users[0].age);  // 99

// Cas pratique : pagination
function paginate(array, page, pageSize) {
  const start = (page - 1) * pageSize;
  const end = start + pageSize;
  return array.slice(start, end);
}

const items = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
console.log(paginate(items, 1, 3));  // [1, 2, 3]
console.log(paginate(items, 2, 3));  // [4, 5, 6]
```

#### `concat()` - Fusionner des arrays

```javascript
const arr1 = [1, 2, 3];
const arr2 = [4, 5, 6];

// Concaténer deux arrays
const combined = arr1.concat(arr2);
console.log(combined);  // [1, 2, 3, 4, 5, 6]
console.log(arr1);      // [1, 2, 3] - inchangé

// Concaténer plusieurs arrays
const arr3 = [7, 8];
const result = arr1.concat(arr2, arr3);
console.log(result);  // [1, 2, 3, 4, 5, 6, 7, 8]

// Ajouter des valeurs individuelles
const withValues = arr1.concat(99, 100, arr2);
console.log(withValues);  // [1, 2, 3, 99, 100, 4, 5, 6]

// Alternative moderne : spread operator (...)
const modern = [...arr1, ...arr2];
console.log(modern);  // [1, 2, 3, 4, 5, 6]

// Aplatir un niveau
const nested = arr1.concat([arr2]);
console.log(nested);  // [1, 2, 3, [4, 5, 6]]

// Cas pratique : ajouter des éléments de manière immutable (pour React)
function addItem(array, item) {
  return array.concat(item);
  // ou: return [...array, item];
}

const original = [1, 2, 3];
const updated = addItem(original, 4);
console.log(original);  // [1, 2, 3] - inchangé
console.log(updated);   // [1, 2, 3, 4]
```

#### `join()` - Convertir en string

```javascript
const words = ['Hello', 'World', '!'];

// Joindre avec un séparateur
console.log(words.join(' '));   // 'Hello World !'
console.log(words.join('-'));   // 'Hello-World-!'
console.log(words.join(''));    // 'HelloWorld!'

// Sans paramètre (séparateur par défaut = virgule)
console.log(words.join());      // 'Hello,World,!'

// Cas pratique : créer un chemin de fichier
const pathParts = ['home', 'user', 'documents', 'file.txt'];
const path = pathParts.join('/');
console.log(path);  // 'home/user/documents/file.txt'

// Cas pratique : créer une liste HTML
const items = ['Item 1', 'Item 2', 'Item 3'];
const html = '<li>' + items.join('</li><li>') + '</li>';
console.log(html);  // '<li>Item 1</li><li>Item 2</li><li>Item 3</li>'

// Avec des nombres
const numbers = [1, 2, 3, 4, 5];
console.log(numbers.join(' + '));  // '1 + 2 + 3 + 4 + 5'
```

#### `reverse()` - Inverser l'ordre

```javascript
const numbers = [1, 2, 3, 4, 5];

// ⚠️ reverse() modifie l'array original (mutable)
numbers.reverse();
console.log(numbers);  // [5, 4, 3, 2, 1]

// Pour éviter la mutation
const original = [1, 2, 3, 4, 5];
const reversed = [...original].reverse();
console.log(original);  // [1, 2, 3, 4, 5] - inchangé
console.log(reversed);  // [5, 4, 3, 2, 1]

// Ou avec slice()
const reversed2 = original.slice().reverse();

// Cas pratique : parcourir un array à l'envers
const items = ['a', 'b', 'c', 'd'];
[...items].reverse().forEach(item => {
  console.log(item);  // d, c, b, a
});
```

#### `sort()` - Trier un array

```javascript
// ⚠️ IMPORTANT: sort() modifie l'array original et trie par défaut en string !

// Tri par défaut (alphabétique)
const fruits = ['orange', 'pomme', 'banane'];
fruits.sort();
console.log(fruits);  // ['banane', 'orange', 'pomme']

// ⚠️ Problème avec les nombres (tri alphabétique)
const numbers = [10, 5, 40, 25, 1000, 1];
numbers.sort();
console.log(numbers);  // [1, 10, 1000, 25, 40, 5] - INCORRECT !

// ✅ Solution: fonction de comparaison
const nums = [10, 5, 40, 25, 1000, 1];

// Tri croissant
nums.sort((a, b) => a - b);
console.log(nums);  // [1, 5, 10, 25, 40, 1000]

// Tri décroissant
nums.sort((a, b) => b - a);
console.log(nums);  // [1000, 40, 25, 10, 5, 1]

// Trier des objets
const users = [
  { name: 'Charlie', age: 30 },
  { name: 'Alice', age: 25 },
  { name: 'Bob', age: 35 }
];

// Tri par âge
users.sort((a, b) => a.age - b.age);
console.log(users);
// [{ name: 'Alice', age: 25 }, { name: 'Charlie', age: 30 }, { name: 'Bob', age: 35 }]

// Tri par nom (alphabétique)
users.sort((a, b) => a.name.localeCompare(b.name));
console.log(users);
// [{ name: 'Alice', ... }, { name: 'Bob', ... }, { name: 'Charlie', ... }]

// Tri sans mutation
const original = [3, 1, 2];
const sorted = [...original].sort((a, b) => a - b);
console.log(original);  // [3, 1, 2]
console.log(sorted);    // [1, 2, 3]

// Cas pratique : tri multi-critères
const products = [
  { name: 'Product A', price: 100, rating: 4 },
  { name: 'Product B', price: 100, rating: 5 },
  { name: 'Product C', price: 50, rating: 3 }
];

// Trier par prix puis par rating
products.sort((a, b) => {
  if (a.price !== b.price) {
    return a.price - b.price;  // Prix croissant
  }
  return b.rating - a.rating;  // Rating décroissant si même prix
});
```

---

### Itération sur les arrays

#### `forEach()` - Exécuter une fonction pour chaque élément

```javascript
const numbers = [1, 2, 3, 4, 5];

// Syntaxe de base
numbers.forEach(num => {
  console.log(num * 2);  // 2, 4, 6, 8, 10
});

// Avec index et array
numbers.forEach((num, index, array) => {
  console.log(`Index ${index}: ${num}`);
});

// ⚠️ forEach() ne retourne rien (undefined)
const result = numbers.forEach(n => n * 2);
console.log(result);  // undefined

// On ne peut pas break ou return dans forEach
numbers.forEach(num => {
  if (num === 3) {
    return;  // Continue seulement l'itération actuelle
  }
  console.log(num);  // 1, 2, 4, 5 (pas de 3)
});

// Cas pratique : mise à jour d'éléments DOM
const ids = ['btn1', 'btn2', 'btn3'];
ids.forEach(id => {
  const element = document.getElementById(id);
  if (element) {
    element.classList.add('active');
  }
});

// Cas pratique : accumulation (mais reduce() est meilleur)
let sum = 0;
numbers.forEach(n => {
  sum += n;
});
console.log(sum);  // 15
```

#### `map()` - Transformer chaque élément

```javascript
const numbers = [1, 2, 3, 4, 5];

// Doubler chaque nombre
const doubled = numbers.map(n => n * 2);
console.log(doubled);  // [2, 4, 6, 8, 10]
console.log(numbers);  // [1, 2, 3, 4, 5] - inchangé

// Avec index
const withIndex = numbers.map((n, i) => `${i}: ${n}`);
console.log(withIndex);  // ['0: 1', '1: 2', '2: 3', '3: 4', '4: 5']

// Transformer des objets
const users = [
  { firstName: 'John', lastName: 'Doe' },
  { firstName: 'Jane', lastName: 'Smith' }
];

const fullNames = users.map(user => `${user.firstName} ${user.lastName}`);
console.log(fullNames);  // ['John Doe', 'Jane Smith']

// Extraire une propriété
const ages = users.map(user => user.age);

// Transformer et ajouter des propriétés
const usersWithId = users.map((user, index) => ({
  id: index + 1,
  ...user,
  fullName: `${user.firstName} ${user.lastName}`
}));

// ⚠️ Attention: ne pas utiliser map() si vous n'utilisez pas le résultat
// Utilisez forEach() à la place
numbers.map(n => console.log(n));  // ❌ Mauvais
numbers.forEach(n => console.log(n));  // ✅ Bon

// Cas pratique React : rendre une liste
const items = ['Apple', 'Banana', 'Orange'];
const listItems = items.map((item, index) => (
  <li key={index}>{item}</li>
));
```

#### `filter()` - Filtrer les éléments

```javascript
const numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];

// Filtrer les nombres pairs
const evens = numbers.filter(n => n % 2 === 0);
console.log(evens);  // [2, 4, 6, 8, 10]

// Filtrer les nombres > 5
const greaterThan5 = numbers.filter(n => n > 5);
console.log(greaterThan5);  // [6, 7, 8, 9, 10]

// Avec index
const withoutFirst = numbers.filter((n, i) => i !== 0);
console.log(withoutFirst);  // [2, 3, 4, 5, 6, 7, 8, 9, 10]

// Filtrer des objets
const users = [
  { name: 'Alice', age: 17, active: true },
  { name: 'Bob', age: 25, active: false },
  { name: 'Charlie', age: 30, active: true }
];

// Utilisateurs adultes actifs
const activeAdults = users.filter(u => u.age >= 18 && u.active);
console.log(activeAdults);
// [{ name: 'Charlie', age: 30, active: true }]

// Supprimer les valeurs falsy
const mixed = [0, 1, false, 2, '', 3, null, undefined, 4, NaN];
const truthy = mixed.filter(Boolean);  // Astuce: Boolean comme fonction
console.log(truthy);  // [1, 2, 3, 4]

// Supprimer les doublons (méthode avec filter)
const duplicates = [1, 2, 2, 3, 3, 3, 4];
const unique = duplicates.filter((item, index, array) => 
  array.indexOf(item) === index
);
console.log(unique);  // [1, 2, 3, 4]

// Cas pratique : recherche
function searchUsers(users, query) {
  return users.filter(user => 
    user.name.toLowerCase().includes(query.toLowerCase())
  );
}

const results = searchUsers(users, 'ali');
console.log(results);  // [{ name: 'Alice', ... }]
```

#### `reduce()` - Réduire à une seule valeur

```javascript
// reduce((accumulator, currentValue, index, array) => {...}, initialValue)

const numbers = [1, 2, 3, 4, 5];

// Somme
const sum = numbers.reduce((acc, n) => acc + n, 0);
console.log(sum);  // 15

// Produit
const product = numbers.reduce((acc, n) => acc * n, 1);
console.log(product);  // 120

// Trouver le maximum
const max = numbers.reduce((acc, n) => n > acc ? n : acc);
console.log(max);  // 5

// Compter les occurrences
const fruits = ['pomme', 'banane', 'pomme', 'orange', 'banane', 'pomme'];
const count = fruits.reduce((acc, fruit) => {
  acc[fruit] = (acc[fruit] || 0) + 1;
  return acc;
}, {});
console.log(count);  // { pomme: 3, banane: 2, orange: 1 }

// Grouper par propriété
const people = [
  { name: 'Alice', age: 25 },
  { name: 'Bob', age: 30 },
  { name: 'Charlie', age: 25 }
];

const groupedByAge = people.reduce((acc, person) => {
  const age = person.age;
  if (!acc[age]) {
    acc[age] = [];
  }
  acc[age].push(person.name);
  return acc;
}, {});
console.log(groupedByAge);  // { 25: ['Alice', 'Charlie'], 30: ['Bob'] }

// Aplatir un array (flatten)
const nested = [[1, 2], [3, 4], [5, 6]];
const flat = nested.reduce((acc, arr) => acc.concat(arr), []);
console.log(flat);  // [1, 2, 3, 4, 5, 6]

// Chaîner des opérations (alternative à map + filter)
const result = [1, 2, 3, 4, 5].reduce((acc, n) => {
  if (n % 2 === 0) {  // filter
    acc.push(n * 2);   // map
  }
  return acc;
}, []);
console.log(result);  // [4, 8]

// Calculer une moyenne
const average = numbers.reduce((acc, n, i, array) => {
  acc += n;
  if (i === array.length - 1) {
    return acc / array.length;
  }
  return acc;
}, 0);
console.log(average);  // 3

// Créer un objet à partir d'un array
const users = [
  { id: 1, name: 'Alice' },
  { id: 2, name: 'Bob' }
];

const usersById = users.reduce((acc, user) => {
  acc[user.id] = user;
  return acc;
}, {});
console.log(usersById);
// { 1: { id: 1, name: 'Alice' }, 2: { id: 2, name: 'Bob' } }

// Pipeline de transformations
const pipeline = [
  arr => arr.filter(n => n > 0),
  arr => arr.map(n => n * 2),
  arr => arr.reduce((sum, n) => sum + n, 0)
];

const data = [-1, 2, -3, 4, 5];
const finalResult = pipeline.reduce((acc, fn) => fn(acc), data);
console.log(finalResult);  // 22 (2*2 + 4*2 + 5*2)
```

#### `reduceRight()` - Réduire de droite à gauche

```javascript
const numbers = [1, 2, 3, 4, 5];

// Différence entre reduce et reduceRight
const resultReduce = numbers.reduce((acc, n) => acc - n);
console.log(resultReduce);  // -13 ((((1-2)-3)-4)-5)

const resultReduceRight = numbers.reduceRight((acc, n) => acc - n);
console.log(resultReduceRight);  // -5 ((((5-4)-3)-2)-1)

// Cas pratique : construire une string à l'envers
const words = ['Hello', 'World', 'JavaScript'];
const reversed = words.reduceRight((acc, word) => acc + ' ' + word);
console.log(reversed);  // 'JavaScript World Hello'

// Composition de fonctions (de droite à gauche)
const add5 = x => x + 5;
const multiply2 = x => x * 2;
const subtract3 = x => x - 3;

const functions = [add5, multiply2, subtract3];
const compose = functions.reduceRight((acc, fn) => 
  x => fn(acc(x))
, x => x);

console.log(compose(10));  // add5(multiply2(subtract3(10))) = 19
```

---

### Tests sur les arrays

#### `some()` - Au moins un élément satisfait la condition

```javascript
const numbers = [1, 2, 3, 4, 5];

// Vérifier s'il y a au moins un nombre pair
const hasEven = numbers.some(n => n % 2 === 0);
console.log(hasEven);  // true

// Vérifier s'il y a un nombre > 10
const hasGreaterThan10 = numbers.some(n => n > 10);
console.log(hasGreaterThan10);  // false

// Avec des objets
const users = [
  { name: 'Alice', age: 17 },
  { name: 'Bob', age: 25 },
  { name: 'Charlie', age: 15 }
];

// Y a-t-il au moins un adulte ?
const hasAdult = users.some(u => u.age >= 18);
console.log(hasAdult);  // true

// some() s'arrête dès qu'il trouve un match (optimisation)
const result = [1, 2, 3, 4, 5].some(n => {
  console.log('Checking:', n);
  return n > 3;
});
// Console: Checking: 1, Checking: 2, Checking: 3, Checking: 4
// S'arrête à 4 car c'est le premier > 3

// Cas pratique : validation de formulaire
const formFields = [
  { name: 'email', value: 'test@email.com', valid: true },
  { name: 'password', value: '123', valid: false },
  { name: 'username', value: 'john', valid: true }
];

const hasInvalidField = formFields.some(field => !field.valid);
console.log(hasInvalidField);  // true

// Vérifier si un array contient des valeurs spécifiques
const permissions = ['read', 'write', 'delete'];
const canDelete = permissions.some(p => p === 'delete');
console.log(canDelete);  // true
```

#### `every()` - Tous les éléments satisfont la condition

```javascript
const numbers = [2, 4, 6, 8, 10];

// Vérifier si tous sont pairs
const allEven = numbers.every(n => n % 2 === 0);
console.log(allEven);  // true

// Vérifier si tous sont > 5
const allGreaterThan5 = numbers.every(n => n > 5);
console.log(allGreaterThan5);  // false

// Avec des objets
const users = [
  { name: 'Alice', age: 25, verified: true },
  { name: 'Bob', age: 30, verified: true },
  { name: 'Charlie', age: 35, verified: false }
];

// Tous les utilisateurs sont-ils vérifiés ?
const allVerified = users.every(u => u.verified);
console.log(allVerified);  // false

// Tous les utilisateurs sont-ils adultes ?
const allAdults = users.every(u => u.age >= 18);
console.log(allAdults);  // true

// every() s'arrête dès qu'il trouve un élément qui ne satisfait pas
const result = [2, 4, 6, 7, 8].every(n => {
  console.log('Checking:', n);
  return n % 2 === 0;
});
// Console: Checking: 2, Checking: 4, Checking: 6, Checking: 7
// S'arrête à 7 car c'est le premier qui n'est pas pair

// Cas pratique : validation complète
function validateForm(fields) {
  return fields.every(field => {
    return field.value && field.value.length > 0;
  });
}

const form1 = [
  { name: 'email', value: 'test@email.com' },
  { name: 'password', value: '123456' }
];

const form2 = [
  { name: 'email', value: '' },
  { name: 'password', value: '123456' }
];

console.log(validateForm(form1));  // true
console.log(validateForm(form2));  // false

// Array vide retourne toujours true
console.log([].every(n => n > 10));  // true (vacuous truth)
```

---

### Autres méthodes utiles

#### `flat()` - Aplatir un array (ES2019)

```javascript
// flat(depth) - depth par défaut = 1

// Aplatir un niveau
const arr1 = [1, 2, [3, 4]];
console.log(arr1.flat());  // [1, 2, 3, 4]

// Array déjà plat
const arr2 = [1, 2, 3, 4];
console.log(arr2.flat());  // [1, 2, 3, 4] - inchangé

// Aplatir plusieurs niveaux
const arr3 = [1, [2, [3, [4]]]];
console.log(arr3.flat());     // [1, 2, [3, [4]]] - 1 niveau
console.log(arr3.flat(2));    // [1, 2, 3, [4]] - 2 niveaux
console.log(arr3.flat(3));    // [1, 2, 3, 4] - 3 niveaux
console.log(arr3.flat(Infinity));  // [1, 2, 3, 4] - tous les niveaux

// Supprimer les holes (empty slots)
const arr4 = [1, 2, , 4, 5];
console.log(arr4.flat());  // [1, 2, 4, 5] - supprime les holes

// Cas pratique : aplatir des résultats groupés
const categories = [
  { name: 'Fruits', items: ['pomme', 'banane'] },
  { name: 'Légumes', items: ['carotte', 'tomate'] }
];

const allItems = categories.map(cat => cat.items).flat();
console.log(allItems);  // ['pomme', 'banane', 'carotte', 'tomate']

// Cas pratique : aplatir des arrays d'IDs
const userGroups = [
  { groupId: 1, userIds: [101, 102] },
  { groupId: 2, userIds: [103, 104, 105] }
];

const allUserIds = userGroups.map(g => g.userIds).flat();
console.log(allUserIds);  // [101, 102, 103, 104, 105]
```

#### `flatMap()` - Combiner map et flat

```javascript
// flatMap(callback) = map().flat(1)

const numbers = [1, 2, 3];

// Doubler et dupliquer chaque nombre
const result = numbers.flatMap(n => [n, n * 2]);
console.log(result);  // [1, 2, 2, 4, 3, 6]

// Équivalent avec map + flat
const result2 = numbers.map(n => [n, n * 2]).flat();
console.log(result2);  // [1, 2, 2, 4, 3, 6]

// Filtrer et transformer en même temps
const sentences = ['Hello world', 'How are you'];
const words = sentences.flatMap(sentence => sentence.split(' '));
console.log(words);  // ['Hello', 'world', 'How', 'are', 'you']

// Cas pratique : extraire tous les tags d'articles
const articles = [
  { title: 'Article 1', tags: ['js', 'react'] },
  { title: 'Article 2', tags: ['vue', 'css'] },
  { title: 'Article 3', tags: ['js', 'node'] }
];

const allTags = articles.flatMap(article => article.tags);
console.log(allTags);  // ['js', 'react', 'vue', 'css', 'js', 'node']

// Avec filtrage
const activeTags = articles
  .filter(article => article.published)
  .flatMap(article => article.tags);

// Retourner un array vide pour filtrer
const filtered = [1, 2, 3, 4, 5].flatMap(n => 
  n % 2 === 0 ? [n * 2] : []
);
console.log(filtered);  // [4, 8] - ne garde que les pairs doublés
```

#### `fill()` - Remplir avec une valeur

```javascript
// fill(value, start, end)

// Remplir tout l'array
const arr1 = [1, 2, 3, 4, 5];
arr1.fill(0);
console.log(arr1);  // [0, 0, 0, 0, 0]

// Remplir à partir d'un index
const arr2 = [1, 2, 3, 4, 5];
arr2.fill(0, 2);
console.log(arr2);  // [1, 2, 0, 0, 0]

// Remplir entre deux index
const arr3 = [1, 2, 3, 4, 5];
arr3.fill(0, 1, 3);
console.log(arr3);  // [1, 0, 0, 4, 5]

// Avec index négatifs
const arr4 = [1, 2, 3, 4, 5];
arr4.fill(0, -2);
console.log(arr4);  // [1, 2, 3, 0, 0]

// Créer un array pré-rempli
const zeros = new Array(5).fill(0);
console.log(zeros);  // [0, 0, 0, 0, 0]

const ones = Array(3).fill(1);
console.log(ones);  // [1, 1, 1]

// ⚠️ Attention avec les objets (référence partagée)
const arr5 = new Array(3).fill({});
arr5[0].name = 'Alice';
console.log(arr5);  
// [{ name: 'Alice' }, { name: 'Alice' }, { name: 'Alice' }]
// Tous les objets sont le même !

// Solution : utiliser Array.from() ou map()
const arr6 = Array.from({ length: 3 }, () => ({}));
arr6[0].name = 'Alice';
console.log(arr6);  
// [{ name: 'Alice' }, {}, {}] - objets différents

// Cas pratique : initialiser une grille de jeu
const grid = Array(3).fill(null).map(() => Array(3).fill(0));
console.log(grid);
// [[0, 0, 0], [0, 0, 0], [0, 0, 0]]
```

#### `Array.from()` - Créer un array

```javascript
// Array.from(arrayLike, mapFunction, thisArg)

// Depuis une string
const str = 'hello';
const chars = Array.from(str);
console.log(chars);  // ['h', 'e', 'l', 'l', 'o']

// Depuis un Set
const set = new Set([1, 2, 3, 2, 1]);
const arr = Array.from(set);
console.log(arr);  // [1, 2, 3]

// Depuis un NodeList (DOM)
// const divs = document.querySelectorAll('div');
// const divArray = Array.from(divs);

// Créer un range de nombres
const range = Array.from({ length: 5 }, (_, i) => i);
console.log(range);  // [0, 1, 2, 3, 4]

const range2 = Array.from({ length: 5 }, (_, i) => i + 1);
console.log(range2);  // [1, 2, 3, 4, 5]

// Avec une fonction de mapping
const doubled = Array.from([1, 2, 3], x => x * 2);
console.log(doubled);  // [2, 4, 6]

// Créer un array d'objets
const users = Array.from({ length: 3 }, (_, i) => ({
  id: i + 1,
  name: `User ${i + 1}`
}));
console.log(users);
// [{ id: 1, name: 'User 1' }, { id: 2, name: 'User 2' }, { id: 3, name: 'User 3' }]

// Depuis arguments (dans une fonction)
function sum() {
  const args = Array.from(arguments);
  return args.reduce((acc, n) => acc + n, 0);
}
console.log(sum(1, 2, 3, 4));  // 10

// Cas pratique : générer un alphabet
const alphabet = Array.from({ length: 26 }, (_, i) => 
  String.fromCharCode(65 + i)
);
console.log(alphabet);  // ['A', 'B', 'C', ..., 'Z']

// Cloner un array (shallow copy)
const original = [1, 2, 3];
const clone = Array.from(original);
```

#### `Array.isArray()` - Vérifier si c'est un array

```javascript
// La seule méthode fiable pour vérifier un array

console.log(Array.isArray([1, 2, 3]));  // true
console.log(Array.isArray([]));         // true
console.log(Array.isArray('array'));    // false
console.log(Array.isArray({ length: 2 }));  // false
console.log(Array.isArray(null));       // false
console.log(Array.isArray(undefined));  // false

// ❌ typeof ne fonctionne pas
console.log(typeof [1, 2, 3]);  // 'object'

// ❌ instanceof peut échouer avec des iframes
const arr = [1, 2, 3];
console.log(arr instanceof Array);  // true (mais pas fiable)

// Cas pratique : validation de paramètres
function processItems(items) {
  if (!Array.isArray(items)) {
    throw new Error('Expected an array');
  }
  
  return items.map(item => item * 2);
}

// Cas pratique : normaliser l'input
function ensureArray(value) {
  if (Array.isArray(value)) {
    return value;
  }
  return [value];
}

console.log(ensureArray(5));        // [5]
console.log(ensureArray([1, 2]));   // [1, 2]
```

---

## Cas Pratiques Avancés

### Chaînage de méthodes (Method Chaining)

```javascript
// Exemple complet : traitement de données utilisateurs
const users = [
  { id: 1, name: 'Alice', age: 25, active: true, score: 85 },
  { id: 2, name: 'Bob', age: 17, active: false, score: 65 },
  { id: 3, name: 'Charlie', age: 30, active: true, score: 95 },
  { id: 4, name: 'David', age: 22, active: true, score: 75 },
  { id: 5, name: 'Eve', age: 16, active: true, score: 90 }
];

// Trouver les noms des utilisateurs actifs majeurs avec score > 80, triés par score
const topUsers = users
  .filter(user => user.active)           // uniquement actifs
  .filter(user => user.age >= 18)        // uniquement majeurs
  .filter(user => user.score > 80)       // score > 80
  .sort((a, b) => b.score - a.score)     // tri décroissant par score
  .map(user => user.name);               // extraire les noms

console.log(topUsers);  // ['Charlie', 'Alice']

// Version optimisée (une seule itération avec reduce)
const topUsersOptimized = users.reduce((acc, user) => {
  if (user.active && user.age >= 18 && user.score > 80) {
    acc.push({ name: user.name, score: user.score });
  }
  return acc;
}, [])
.sort((a, b) => b.score - a.score)
.map(user => user.name);

console.log(topUsersOptimized);  // ['Charlie', 'Alice']
```

### Manipulation immutable (important pour React)

```javascript
const todos = [
  { id: 1, text: 'Learn JS', done: false },
  { id: 2, text: 'Learn React', done: false },
  { id: 3, text: 'Build app', done: false }
];

// ✅ Ajouter un item (immutable)
const addTodo = (todos, newTodo) => {
  return [...todos, newTodo];
  // ou: return todos.concat(newTodo);
};

const withNew = addTodo(todos, { id: 4, text: 'Deploy', done: false });
console.log(todos.length);     // 3 - original inchangé
console.log(withNew.length);   // 4

// ✅ Supprimer un item par ID (immutable)
const removeTodo = (todos, id) => {
  return todos.filter(todo => todo.id !== id);
};

const withoutSecond = removeTodo(todos, 2);
console.log(withoutSecond.length);  // 2

// ✅ Modifier un item (immutable)
const toggleTodo = (todos, id) => {
  return todos.map(todo =>
    todo.id === id
      ? { ...todo, done: !todo.done }  // nouveau objet si match
      : todo                            // même référence sinon
  );
};

const toggled = toggleTodo(todos, 2);
console.log(todos[1].done);    // false - original inchangé
console.log(toggled[1].done);  // true - nouveau array

// ✅ Modifier plusieurs items
const markAllDone = (todos) => {
  return todos.map(todo => ({ ...todo, done: true }));
};

// ✅ Réorganiser (déplacer un item)
const moveTodo = (todos, fromIndex, toIndex) => {
  const newTodos = [...todos];
  const [removed] = newTodos.splice(fromIndex, 1);
  newTodos.splice(toIndex, 0, removed);
  return newTodos;
};

const reordered = moveTodo(todos, 0, 2);
console.log(reordered.map(t => t.text));
// ['Learn React', 'Build app', 'Learn JS']
```

### Grouper et agréger des données

```javascript
const orders = [
  { id: 1, customer: 'Alice', amount: 100, status: 'completed' },
  { id: 2, customer: 'Bob', amount: 150, status: 'pending' },
  { id: 3, customer: 'Alice', amount: 200, status: 'completed' },
  { id: 4, customer: 'Charlie', amount: 75, status: 'completed' },
  { id: 5, customer: 'Bob', amount: 50, status: 'cancelled' }
];

// Grouper par customer
const byCustomer = orders.reduce((acc, order) => {
  if (!acc[order.customer]) {
    acc[order.customer] = [];
  }
  acc[order.customer].push(order);
  return acc;
}, {});

console.log(byCustomer);
// {
//   Alice: [{ id: 1, ... }, { id: 3, ... }],
//   Bob: [{ id: 2, ... }, { id: 5, ... }],
//   Charlie: [{ id: 4, ... }]
// }

// Calculer le total par customer
const totalsByCustomer = orders.reduce((acc, order) => {
  if (!acc[order.customer]) {
    acc[order.customer] = 0;
  }
  acc[order.customer] += order.amount;
  return acc;
}, {});

console.log(totalsByCustomer);
// { Alice: 300, Bob: 200, Charlie: 75 }

// Statistiques par status
const statsByStatus = orders.reduce((acc, order) => {
  const status = order.status;
  if (!acc[status]) {
    acc[status] = { count: 0, total: 0 };
  }
  acc[status].count++;
  acc[status].total += order.amount;
  return acc;
}, {});

console.log(statsByStatus);
// {
//   completed: { count: 3, total: 375 },
//   pending: { count: 1, total: 150 },
//   cancelled: { count: 1, total: 50 }
// }
```

### Dédoublonner des arrays complexes

```javascript
// Doublons simples (primitifs)
const numbers = [1, 2, 2, 3, 3, 3, 4];
const unique1 = [...new Set(numbers)];
console.log(unique1);  // [1, 2, 3, 4]

// Doublons avec objets (par propriété)
const users = [
  { id: 1, name: 'Alice' },
  { id: 2, name: 'Bob' },
  { id: 1, name: 'Alice' },  // doublon
  { id: 3, name: 'Charlie' }
];

// Méthode 1 : Map
const uniqueById = [...new Map(users.map(u => [u.id, u])).values()];
console.log(uniqueById);
// [{ id: 1, name: 'Alice' }, { id: 2, name: 'Bob' }, { id: 3, name: 'Charlie' }]

// Méthode 2 : filter avec findIndex
const uniqueById2 = users.filter((user, index, self) =>
  index === self.findIndex(u => u.id === user.id)
);

// Méthode 3 : reduce
const uniqueById3 = users.reduce((acc, user) => {
  if (!acc.find(u => u.id === user.id)) {
    acc.push(user);
  }
  return acc;
}, []);

// Dédoublonner par plusieurs propriétés
const products = [
  { name: 'iPhone', color: 'black', size: '128GB' },
  { name: 'iPhone', color: 'white', size: '128GB' },
  { name: 'iPhone', color: 'black', size: '128GB' },  // doublon exact
  { name: 'Samsung', color: 'blue', size: '256GB' }
];

const uniqueProducts = products.filter((product, index, self) =>
  index === self.findIndex(p =>
    p.name === product.name &&
    p.color === product.color &&
    p.size === product.size
  )
);

console.log(uniqueProducts.length);  // 3
```

### Pagination

```javascript
const items = Array.from({ length: 100 }, (_, i) => `Item ${i + 1}`);

function paginate(array, pageNumber, pageSize) {
  const startIndex = (pageNumber - 1) * pageSize;
  const endIndex = startIndex + pageSize;
  
  return {
    data: array.slice(startIndex, endIndex),
    currentPage: pageNumber,
    pageSize: pageSize,
    totalItems: array.length,
    totalPages: Math.ceil(array.length / pageSize),
    hasNextPage: endIndex < array.length,
    hasPrevPage: pageNumber > 1
  };
}

// Page 1 avec 10 items par page
const page1 = paginate(items, 1, 10);
console.log(page1.data);  // ['Item 1', 'Item 2', ..., 'Item 10']
console.log(page1.totalPages);  // 10
console.log(page1.hasNextPage);  // true

// Page 5
const page5 = paginate(items, 5, 10);
console.log(page5.data);  // ['Item 41', 'Item 42', ..., 'Item 50']
```

### Recherche et filtrage avancés

```javascript
const products = [
  { id: 1, name: 'iPhone 13', category: 'phone', price: 999, inStock: true, tags: ['apple', '5g'] },
  { id: 2, name: 'Samsung Galaxy', category: 'phone', price: 899, inStock: false, tags: ['samsung', '5g'] },
  { id: 3, name: 'iPad Pro', category: 'tablet', price: 1099, inStock: true, tags: ['apple', 'pro'] },
  { id: 4, name: 'MacBook Air', category: 'laptop', price: 1299, inStock: true, tags: ['apple', 'm2'] }
];

// Recherche multi-critères
function searchProducts(products, filters) {
  return products.filter(product => {
    // Filtre par texte (name)
    if (filters.query) {
      const match = product.name.toLowerCase().includes(filters.query.toLowerCase());
      if (!match) return false;
    }
    
    // Filtre par catégorie
    if (filters.category && product.category !== filters.category) {
      return false;
    }
    
    // Filtre par prix (range)
    if (filters.minPrice && product.price < filters.minPrice) {
      return false;
    }
    if (filters.maxPrice && product.price > filters.maxPrice) {
      return false;
    }
    
    // Filtre par disponibilité
    if (filters.inStock !== undefined && product.inStock !== filters.inStock) {
      return false;
    }
    
    // Filtre par tags
    if (filters.tags && filters.tags.length > 0) {
      const hasTag = filters.tags.some(tag => product.tags.includes(tag));
      if (!hasTag) return false;
    }
    
    return true;
  });
}

// Recherche : produits Apple en stock entre 1000 et 1500€
const results = searchProducts(products, {
  tags: ['apple'],
  inStock: true,
  minPrice: 1000,
  maxPrice: 1500
});

console.log(results);
// [{ id: 3, name: 'iPad Pro', ... }, { id: 4, name: 'MacBook Air', ... }]
```

---

# Chapitre 5 : Arrays (Tableaux) - Questions/Réponses Orales

## Questions sur les Bases

### Q1: Quelle est la différence entre `push()` et `unshift()` ?

**Réponse :**
- `push()` ajoute un ou plusieurs éléments **à la fin** du tableau et retourne la nouvelle longueur
- `unshift()` ajoute un ou plusieurs éléments **au début** du tableau et retourne la nouvelle longueur

```javascript
const arr = [1, 2, 3];

arr.push(4);      // [1, 2, 3, 4] - retourne 4
arr.unshift(0);   // [0, 1, 2, 3, 4] - retourne 5
```

**Point important :** `unshift()` est généralement plus lent car il doit réindexer tous les éléments existants.

---

### Q2: Comment supprimer le dernier élément d'un array ? Et le premier ?

**Réponse :**
- `pop()` supprime et retourne le **dernier** élément
- `shift()` supprime et retourne le **premier** élément

```javascript
const fruits = ['pomme', 'banane', 'orange'];

const last = fruits.pop();    // 'orange', fruits = ['pomme', 'banane']
const first = fruits.shift();  // 'pomme', fruits = ['banane']
```

---

### Q3: Quelle est la différence entre `slice()` et `splice()` ?

**Réponse :**
C'est une question très fréquente !

**`slice(start, end)`** :
- **Ne modifie PAS** l'array original
- Retourne un nouveau tableau (shallow copy)
- Prend deux paramètres : index de début et de fin (non inclus)

```javascript
const arr = [1, 2, 3, 4, 5];
const sliced = arr.slice(1, 4);  // [2, 3, 4]
console.log(arr);                 // [1, 2, 3, 4, 5] - inchangé
```

**`splice(start, deleteCount, ...items)`** :
- **Modifie** l'array original (mutable)
- Peut supprimer ET ajouter des éléments
- Retourne les éléments supprimés

```javascript
const arr = [1, 2, 3, 4, 5];
const removed = arr.splice(1, 2, 'a', 'b');  // [2, 3]
console.log(arr);                             // [1, 'a', 'b', 4, 5]
```

---

## Questions sur les Méthodes d'Itération

### Q4: Expliquez la différence entre `map()` et `forEach()`.

**Réponse :**

**`forEach()`** :
- Exécute une fonction pour chaque élément
- **Ne retourne rien** (undefined)
- Utilisé pour les side effects

```javascript
const numbers = [1, 2, 3];
numbers.forEach(num => console.log(num * 2));  // affiche 2, 4, 6
// Ne retourne rien
```

**`map()`** :
- Transforme chaque élément
- **Retourne un nouveau tableau**
- Utilisé pour créer un nouveau array transformé

```javascript
const numbers = [1, 2, 3];
const doubled = numbers.map(num => num * 2);  // [2, 4, 6]
```

**Point clé :** Si vous n'avez pas besoin du résultat, utilisez `forEach()`. Si vous créez un nouveau tableau, utilisez `map()`.

---

### Q5: Comment fonctionne la méthode `filter()` ? Donnez un exemple.

**Réponse :**
`filter()` crée un **nouveau tableau** contenant tous les éléments qui passent un test (fonction qui retourne true/false).

```javascript
const numbers = [1, 2, 3, 4, 5, 6];

// Filtrer les nombres pairs
const evens = numbers.filter(num => num % 2 === 0);
// [2, 4, 6]

// Filtrer les objets
const users = [
  { name: 'Alice', age: 25 },
  { name: 'Bob', age: 17 },
  { name: 'Charlie', age: 30 }
];

const adults = users.filter(user => user.age >= 18);
// [{ name: 'Alice', age: 25 }, { name: 'Charlie', age: 30 }]
```

---

### Q6: Expliquez `reduce()` avec un exemple pratique.

**Réponse :**
`reduce()` réduit un tableau à une **seule valeur** en appliquant une fonction accumulatrice.

**Syntaxe :** `array.reduce((accumulator, currentValue) => {...}, initialValue)`

```javascript
// Exemple 1 : Somme
const numbers = [1, 2, 3, 4, 5];
const sum = numbers.reduce((acc, num) => acc + num, 0);
// 15

// Exemple 2 : Trouver le max
const max = numbers.reduce((acc, num) => num > acc ? num : acc);
// 5

// Exemple 3 : Grouper des objets
const users = [
  { name: 'Alice', role: 'admin' },
  { name: 'Bob', role: 'user' },
  { name: 'Charlie', role: 'admin' }
];

const grouped = users.reduce((acc, user) => {
  if (!acc[user.role]) {
    acc[user.role] = [];
  }
  acc[user.role].push(user.name);
  return acc;
}, {});
// { admin: ['Alice', 'Charlie'], user: ['Bob'] }
```

---

## Questions sur la Recherche

### Q7: Quelle est la différence entre `find()` et `filter()` ?

**Réponse :**

**`find()`** :
- Retourne le **premier élément** qui satisfait la condition
- Retourne `undefined` si rien n'est trouvé
- S'arrête dès qu'il trouve un match

```javascript
const numbers = [1, 2, 3, 4, 5];
const firstEven = numbers.find(num => num % 2 === 0);  // 2
```

**`filter()`** :
- Retourne **tous les éléments** qui satisfont la condition
- Retourne un array vide `[]` si rien n'est trouvé
- Parcourt tout le tableau

```javascript
const numbers = [1, 2, 3, 4, 5];
const allEvens = numbers.filter(num => num % 2 === 0);  // [2, 4]
```

---

### Q8: Comment vérifier si un élément existe dans un array ?

**Réponse :**
Plusieurs méthodes selon le besoin :

```javascript
const fruits = ['pomme', 'banane', 'orange'];

// 1. includes() - retourne boolean
fruits.includes('banane');  // true
fruits.includes('kiwi');    // false

// 2. indexOf() - retourne l'index ou -1
fruits.indexOf('banane');   // 1
fruits.indexOf('kiwi');     // -1

// 3. find() - pour des conditions complexes
const users = [{ id: 1, name: 'Alice' }, { id: 2, name: 'Bob' }];
const found = users.find(user => user.id === 2);  // { id: 2, name: 'Bob' }

// 4. some() - vérifie si au moins un élément satisfait la condition
const hasAdult = users.some(user => user.age >= 18);
```

**Recommandation :** Utilisez `includes()` pour les valeurs primitives, `find()` pour des objets.

---

## Questions sur les Tests

### Q9: Expliquez la différence entre `some()` et `every()`.

**Réponse :**

**`some()`** :
- Retourne `true` si **au moins un** élément satisfait la condition
- S'arrête dès qu'il trouve un match

```javascript
const numbers = [1, 2, 3, 4, 5];
numbers.some(num => num > 4);  // true (car 5 > 4)
numbers.some(num => num > 10); // false
```

**`every()`** :
- Retourne `true` si **tous** les éléments satisfont la condition
- S'arrête dès qu'il trouve un élément qui ne satisfait pas

```javascript
const numbers = [1, 2, 3, 4, 5];
numbers.every(num => num > 0);  // true (tous positifs)
numbers.every(num => num > 3);  // false (1 et 2 ne sont pas > 3)
```

**Mnémonique :** `some` = "au moins un", `every` = "tous"

---

## Questions Avancées

### Q10: Comment supprimer les doublons d'un array ?

**Réponse :**
Plusieurs approches :

```javascript
const numbers = [1, 2, 2, 3, 3, 4, 5, 5];

// Méthode 1 : Set (la plus élégante)
const unique1 = [...new Set(numbers)];  // [1, 2, 3, 4, 5]

// Méthode 2 : filter avec indexOf
const unique2 = numbers.filter((item, index) => 
  numbers.indexOf(item) === index
);

// Méthode 3 : reduce
const unique3 = numbers.reduce((acc, item) => {
  if (!acc.includes(item)) {
    acc.push(item);
  }
  return acc;
}, []);
```

**Meilleure pratique :** Utilisez `Set` pour la simplicité et la performance.

---

### Q11: Comment aplatir (flatten) un array multidimensionnel ?

**Réponse :**

```javascript
const nested = [1, [2, 3], [4, [5, 6]]];

// flat(depth) - ES2019
const flat1 = nested.flat();      // [1, 2, 3, 4, [5, 6]]
const flat2 = nested.flat(2);     // [1, 2, 3, 4, 5, 6]
const flatAll = nested.flat(Infinity);  // aplatit complètement

// flatMap() - combine map + flat(1)
const arr = [1, 2, 3];
const result = arr.flatMap(x => [x, x * 2]);
// [1, 2, 2, 4, 3, 6]
```

---

### Q12: Comment trier un array d'objets ?

**Réponse :**

```javascript
const users = [
  { name: 'Charlie', age: 30 },
  { name: 'Alice', age: 25 },
  { name: 'Bob', age: 35 }
];

// Trier par âge (croissant)
users.sort((a, b) => a.age - b.age);

// Trier par nom (alphabétique)
users.sort((a, b) => a.name.localeCompare(b.name));

// Trier par âge décroissant
users.sort((a, b) => b.age - a.age);
```

**Point important :** `sort()` modifie l'array original. Pour éviter ça :

```javascript
const sorted = [...users].sort((a, b) => a.age - b.age);
```

---

### Q13: Quelle est la différence entre shallow copy et deep copy d'un array ?

**Réponse :**

**Shallow copy** : copie seulement le premier niveau

```javascript
const original = [1, 2, { a: 3 }];

// Méthodes de shallow copy
const copy1 = [...original];
const copy2 = original.slice();
const copy3 = Array.from(original);

copy1[0] = 999;        // n'affecte pas original
copy1[2].a = 999;      // AFFECTE original ! (référence partagée)
```

**Deep copy** : copie tous les niveaux (objets imbriqués)

```javascript
// Méthode simple (mais limitations)
const deepCopy = JSON.parse(JSON.stringify(original));

// Pour React/projets modernes : structuredClone (nouveau)
const deepCopy2 = structuredClone(original);
```

**Point clé pour React :** Vous devez créer de nouvelles références pour que React détecte les changements.

---

### Q14: Comment chaîner plusieurs méthodes d'array ? Donnez un exemple.

**Réponse :**

```javascript
const users = [
  { name: 'Alice', age: 25, active: true },
  { name: 'Bob', age: 17, active: false },
  { name: 'Charlie', age: 30, active: true },
  { name: 'David', age: 22, active: true }
];

// Trouver les noms des utilisateurs actifs majeurs, triés par âge
const result = users
  .filter(user => user.active)           // utilisateurs actifs
  .filter(user => user.age >= 18)        // majeurs
  .sort((a, b) => a.age - b.age)         // tri par âge
  .map(user => user.name);               // extraire les noms

// ['Alice', 'David', 'Charlie']
```

**Conseil :** Attention à la performance - chaque méthode parcourt le tableau. Pour de gros tableaux, parfois une seule boucle `reduce()` est plus efficace.

---

### Q15: Comment créer un array de taille N avec des valeurs spécifiques ?

**Réponse :**

```javascript
// Array de 5 zéros
const zeros = Array(5).fill(0);  // [0, 0, 0, 0, 0]

// Array de 1 à 10
const range = Array.from({ length: 10 }, (_, i) => i + 1);
// [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

// Array de nombres pairs
const evens = Array.from({ length: 5 }, (_, i) => (i + 1) * 2);
// [2, 4, 6, 8, 10]

// Avec spread et keys()
const nums = [...Array(5).keys()];  // [0, 1, 2, 3, 4]
```

---

## Questions Pièges

### Q16: Que se passe-t-il si on fait `arr.length = 0` ?

**Réponse :**
Cela **vide complètement** le tableau.

```javascript
const arr = [1, 2, 3, 4, 5];
arr.length = 0;
console.log(arr);  // []
```

C'est une façon rapide de vider un array, mais attention : toutes les références pointent vers le même array vide.

---

### Q17: Quelle est la différence entre `Array(3)` et `[undefined, undefined, undefined]` ?

**Réponse :**
Piège classique !

```javascript
// Array(3) crée un array avec 3 "trous" (sparse array)
const arr1 = Array(3);
arr1.map(x => 1);  // [undefined × 3] - map ne fonctionne pas !

// Array littéral avec undefined explicites
const arr2 = [undefined, undefined, undefined];
arr2.map(x => 1);  // [1, 1, 1] - fonctionne !

// Solution : utiliser fill()
const arr3 = Array(3).fill(undefined);
arr3.map(x => 1);  // [1, 1, 1]
```

---

### Q18: Comment vérifier si une variable est un array ?

**Réponse :**

```javascript
const arr = [1, 2, 3];

// ✅ Bonne méthode
Array.isArray(arr);  // true

// ❌ Mauvaises méthodes
typeof arr;          // 'object' - pas fiable !
arr instanceof Array; // true, mais problèmes avec iframes
```

**Important pour React :** Toujours utiliser `Array.isArray()` pour valider les props.

---

## Questions Context React

### Q19: Pourquoi ne pas muter directement un array dans React ?

**Réponse :**
React utilise la **comparaison par référence** pour détecter les changements.

```javascript
// ❌ Mauvais - React ne détectera pas le changement
const [items, setItems] = useState([1, 2, 3]);
items.push(4);  // mutation directe
setItems(items);  // même référence !

// ✅ Bon - créer un nouveau array
setItems([...items, 4]);
// ou
setItems(items.concat(4));
```

**Règle d'or :** Traitez les arrays comme immuables dans React (et Redux).

---

### Q20: Comment gérer efficacement une liste d'items dans React ?

**Réponse :**

```javascript
const [items, setItems] = useState([]);

// Ajouter un item
const addItem = (item) => {
  setItems([...items, item]);
  // ou avec callback pour éviter les stale closures
  setItems(prev => [...prev, item]);
};

// Supprimer un item par id
const removeItem = (id) => {
  setItems(items.filter(item => item.id !== id));
};

// Modifier un item
const updateItem = (id, updates) => {
  setItems(items.map(item => 
    item.id === id ? { ...item, ...updates } : item
  ));
};

// Rendre la liste avec key
return (
  <ul>
    {items.map(item => (
      <li key={item.id}>{item.name}</li>
    ))}
  </ul>
);
```

**Points critiques :**
- Toujours utiliser `key` unique dans les listes
- Préférer les callback functions dans `setState` si vous dépendez de l'état précédent
- Ne jamais utiliser l'index comme key si l'ordre peut changer

---

