## [**..**](./00_index.md)

# Chapitre 7 : Destructuring

## 7.1 Array Destructuring

### Syntaxe de base

```javascript
// Sans destructuring (ancienne méthode)
const colors = ['red', 'green', 'blue'];
const first = colors[0];
const second = colors[1];
const third = colors[2];

// Avec destructuring (moderne)
const [first, second, third] = ['red', 'green', 'blue'];
console.log(first);   // 'red'
console.log(second);  // 'green'
console.log(third);   // 'blue'

// Déclarer les variables avant
let a, b, c;
[a, b, c] = [1, 2, 3];
console.log(a, b, c);  // 1 2 3

// Avec const/let
const [x, y] = [10, 20];
let [m, n] = [100, 200];

// Destructuring partiel
const numbers = [1, 2, 3, 4, 5];
const [first, second] = numbers;  // On prend seulement les 2 premiers
console.log(first);   // 1
console.log(second);  // 2

// Array vide
const [] = [];  // Valide mais inutile

// Plus d'éléments que de variables
const [one, two] = [1, 2, 3, 4, 5];
console.log(one);  // 1
console.log(two);  // 2
// 3, 4, 5 sont ignorés

// Plus de variables que d'éléments
const [a, b, c] = [1, 2];
console.log(a);  // 1
console.log(b);  // 2
console.log(c);  // undefined
```

### Valeurs par défaut

```javascript
// Sans valeur par défaut
const [a, b, c] = [1, 2];
console.log(c);  // undefined

// Avec valeur par défaut
const [x, y, z = 3] = [1, 2];
console.log(x);  // 1
console.log(y);  // 2
console.log(z);  // 3 (valeur par défaut)

// La valeur par défaut est utilisée seulement si undefined
const [m, n = 10] = [5, undefined];
console.log(m);  // 5
console.log(n);  // 10 (undefined → valeur par défaut)

const [p, q = 10] = [5, null];
console.log(p);  // 5
console.log(q);  // null (null n'active PAS la valeur par défaut)

// Valeurs par défaut avec expressions
const [a = 1, b = a * 2] = [];
console.log(a);  // 1
console.log(b);  // 2 (calculé depuis a)

// Valeur par défaut avec fonction
function getDefault() {
  console.log('Getting default value');
  return 42;
}

const [value = getDefault()] = [100];
// getDefault() n'est PAS appelé car value = 100

const [value2 = getDefault()] = [];
// Affiche "Getting default value"
// value2 = 42

// Cas pratique : options de configuration
function createUser([name = 'Anonymous', age = 18, role = 'user'] = []) {
  return { name, age, role };
}

console.log(createUser(['Alice', 25, 'admin']));
// { name: 'Alice', age: 25, role: 'admin' }

console.log(createUser(['Bob']));
// { name: 'Bob', age: 18, role: 'user' }

console.log(createUser());
// { name: 'Anonymous', age: 18, role: 'user' }
```

### Skipping items (ignorer des éléments)

```javascript
// Ignorer des éléments avec des virgules
const numbers = [1, 2, 3, 4, 5];

const [first, , third] = numbers;
console.log(first);  // 1
console.log(third);  // 3 (on a sauté le 2)

// Ignorer plusieurs éléments
const [a, , , d] = [1, 2, 3, 4, 5];
console.log(a);  // 1
console.log(d);  // 4

// Ignorer les premiers
const [, , third, fourth] = [1, 2, 3, 4];
console.log(third);   // 3
console.log(fourth);  // 4

// Ignorer les derniers (utiliser rest)
const [first, second] = [1, 2, 3, 4, 5];
// 3, 4, 5 sont automatiquement ignorés

// Cas pratique : obtenir des coordonnées
const coordinates = [48.8566, 2.3522, 35];  // lat, lng, altitude
const [latitude, longitude] = coordinates;  // on ignore l'altitude
console.log(latitude, longitude);  // 48.8566 2.3522

// Cas pratique : split avec destructuring
const fullName = 'John Michael Doe';
const [firstName, , lastName] = fullName.split(' ');
console.log(firstName);  // 'John'
console.log(lastName);   // 'Doe' (on a ignoré 'Michael')

// Pattern avec holes
const [, second, , fourth, , sixth] = [1, 2, 3, 4, 5, 6, 7, 8];
console.log(second, fourth, sixth);  // 2 4 6
```

### Rest operator (...) dans destructuring

```javascript
// Rest operator collecte les éléments restants dans un array
const numbers = [1, 2, 3, 4, 5];

const [first, ...rest] = numbers;
console.log(first);  // 1
console.log(rest);   // [2, 3, 4, 5]

// Rest doit être le dernier élément
const [a, b, ...others] = [1, 2, 3, 4, 5];
console.log(a);       // 1
console.log(b);       // 2
console.log(others);  // [3, 4, 5]

// ❌ Rest ne peut pas être au milieu
// const [first, ...middle, last] = [1, 2, 3, 4, 5];  // SyntaxError

// Rest avec array vide
const [x, ...remaining] = [1];
console.log(x);          // 1
console.log(remaining);  // [] - array vide

// Rest quand il n'y a rien à collecter
const [y, z, ...nothing] = [1, 2];
console.log(y);       // 1
console.log(z);       // 2
console.log(nothing); // []

// Combiner skip et rest
const [first, , third, ...rest] = [1, 2, 3, 4, 5, 6];
console.log(first);  // 1
console.log(third);  // 3
console.log(rest);   // [4, 5, 6]

// Cas pratique : extraire le premier et garder le reste
function processItems(items) {
  const [currentItem, ...remainingItems] = items;
  
  console.log('Processing:', currentItem);
  console.log('Remaining:', remainingItems);
  
  return { current: currentItem, remaining: remainingItems };
}

processItems(['apple', 'banana', 'orange', 'grape']);
// Processing: apple
// Remaining: ['banana', 'orange', 'grape']

// Cas pratique : séparer head et tail
const [head, ...tail] = [1, 2, 3, 4, 5];
console.log('Head:', head);  // 1
console.log('Tail:', tail);  // [2, 3, 4, 5]

// Cas pratique : cloner un array (shallow copy)
const original = [1, 2, 3, 4, 5];
const [...clone] = original;
console.log(clone);  // [1, 2, 3, 4, 5]
console.log(clone === original);  // false - copies différentes

// Équivalent à :
const clone2 = [...original];

// Cas pratique : fonction avec arguments variables
function sum(first, ...numbers) {
  return numbers.reduce((acc, n) => acc + n, first);
}

console.log(sum(1, 2, 3, 4, 5));  // 15
```

### Destructuring nested arrays (arrays imbriqués)

```javascript
// Array 2D
const matrix = [
  [1, 2],
  [3, 4],
  [5, 6]
];

// Destructuring simple
const [firstRow, secondRow] = matrix;
console.log(firstRow);   // [1, 2]
console.log(secondRow);  // [3, 4]

// Destructuring imbriqué
const [[a, b], [c, d]] = [
  [1, 2],
  [3, 4]
];
console.log(a);  // 1
console.log(b);  // 2
console.log(c);  // 3
console.log(d);  // 4

// Destructuring profond
const nested = [1, [2, [3, [4, 5]]]];
const [first, [second, [third, [fourth, fifth]]]] = nested;
console.log(first, second, third, fourth, fifth);  // 1 2 3 4 5

// Mélanger skip et nested
const data = [
  [1, 2, 3],
  [4, 5, 6],
  [7, 8, 9]
];

const [[, , third], , [seventh]] = data;
console.log(third);    // 3
console.log(seventh);  // 7

// Avec rest
const [[first, ...restOfFirst], ...restOfArray] = [
  [1, 2, 3],
  [4, 5, 6],
  [7, 8, 9]
];
console.log(first);         // 1
console.log(restOfFirst);   // [2, 3]
console.log(restOfArray);   // [[4, 5, 6], [7, 8, 9]]

// Cas pratique : coordonnées multiples
const locations = [
  ['Paris', [48.8566, 2.3522]],
  ['London', [51.5074, -0.1278]],
  ['New York', [40.7128, -74.0060]]
];

for (const [city, [lat, lng]] of locations) {
  console.log(`${city}: ${lat}, ${lng}`);
}
// Paris: 48.8566, 2.3522
// London: 51.5074, -0.1278
// New York: 40.7128, -74.006

// Cas pratique : données d'API imbriquées
const apiResponse = [
  'success',
  {
    user: { id: 1, name: 'Alice' },
    posts: [
      { id: 101, title: 'Post 1' },
      { id: 102, title: 'Post 2' }
    ]
  }
];

// On peut combiner array et object destructuring
const [status, { user, posts: [firstPost] }] = apiResponse;
console.log(status);      // 'success'
console.log(user);        // { id: 1, name: 'Alice' }
console.log(firstPost);   // { id: 101, title: 'Post 1' }
```

### Cas pratiques Array Destructuring

```javascript
// 1. Swapping variables (échanger des variables)
let a = 1;
let b = 2;

// Ancienne méthode
let temp = a;
a = b;
b = temp;

// Avec destructuring (élégant)
[a, b] = [b, a];
console.log(a, b);  // 2 1

// Swap multiple variables
let x = 1, y = 2, z = 3;
[x, y, z] = [z, x, y];
console.log(x, y, z);  // 3 1 2

// 2. Retourner multiple valeurs d'une fonction
function getUser() {
  return ['Alice', 25, 'alice@email.com'];
}

const [name, age, email] = getUser();
console.log(name, age, email);  // Alice 25 alice@email.com

// Avec valeurs par défaut
function getCoordinates() {
  return [48.8566, 2.3522];  // pas d'altitude
}

const [lat, lng, alt = 0] = getCoordinates();
console.log(lat, lng, alt);  // 48.8566 2.3522 0

// 3. Split et destructuring
const fullName = 'John Doe';
const [firstName, lastName] = fullName.split(' ');
console.log(firstName, lastName);  // John Doe

const date = '2024-11-06';
const [year, month, day] = date.split('-');
console.log(year, month, day);  // 2024 11 06

// 4. Itérer sur des arrays de paires
const entries = [
  ['name', 'Alice'],
  ['age', 25],
  ['city', 'Paris']
];

for (const [key, value] of entries) {
  console.log(`${key}: ${value}`);
}
// name: Alice
// age: 25
// city: Paris

// Avec Object.entries()
const user = { name: 'Alice', age: 25 };
for (const [key, value] of Object.entries(user)) {
  console.log(`${key} = ${value}`);
}

// 5. Destructuring dans les paramètres de fonction
function printCoordinates([x, y]) {
  console.log(`X: ${x}, Y: ${y}`);
}

printCoordinates([10, 20]);  // X: 10, Y: 20

// Avec valeurs par défaut
function drawPoint([x = 0, y = 0] = []) {
  console.log(`Drawing point at (${x}, ${y})`);
}

drawPoint([5, 10]);  // Drawing point at (5, 10)
drawPoint([5]);      // Drawing point at (5, 0)
drawPoint();         // Drawing point at (0, 0)

// 6. Extraire des valeurs de regex match
const url = 'https://example.com:8080/path';
const pattern = /^(https?):\/\/([^:]+):(\d+)(.*)$/;
const [, protocol, domain, port, path] = url.match(pattern) || [];

console.log(protocol);  // https
console.log(domain);    // example.com
console.log(port);      // 8080
console.log(path);      // /path

// 7. Pagination
function paginate(array, page, size) {
  const start = (page - 1) * size;
  const end = start + size;
  const items = array.slice(start, end);
  
  return [
    items,
    page,
    Math.ceil(array.length / size),
    array.length
  ];
}

const data = Array.from({ length: 100 }, (_, i) => i + 1);
const [items, currentPage, totalPages, totalItems] = paginate(data, 3, 10);

console.log('Items:', items);         // [21, 22, ..., 30]
console.log('Page:', currentPage);    // 3
console.log('Total pages:', totalPages);  // 10
console.log('Total items:', totalItems);  // 100

// 8. Destructuring avec Promise.all
async function fetchData() {
  const [users, posts, comments] = await Promise.all([
    fetch('/api/users').then(r => r.json()),
    fetch('/api/posts').then(r => r.json()),
    fetch('/api/comments').then(r => r.json())
  ]);
  
  return { users, posts, comments };
}

// 9. React useState
import { useState } from 'react';

function Component() {
  const [count, setCount] = useState(0);
  const [name, setName] = useState('');
  const [isOpen, setIsOpen] = useState(false);
  
  // useState retourne un array [state, setter]
  // qu'on destructure immédiatement
}
```

---

## 7.2 Object Destructuring

### Syntaxe de base

```javascript
// Sans destructuring (ancienne méthode)
const user = {
  name: 'Alice',
  age: 25,
  email: 'alice@email.com'
};

const name = user.name;
const age = user.age;
const email = user.email;

// Avec destructuring (moderne)
const { name, age, email } = user;
console.log(name);   // 'Alice'
console.log(age);    // 25
console.log(email);  // 'alice@email.com'

// Les noms doivent correspondre aux propriétés
const { name, city } = { name: 'Alice', age: 25 };
console.log(name);  // 'Alice'
console.log(city);  // undefined (propriété n'existe pas)

// Ordre n'a pas d'importance
const { age, name } = { name: 'Bob', age: 30 };
console.log(name);  // 'Bob'
console.log(age);   // 30

// Déclarer les variables avant
let x, y;
({ x, y } = { x: 10, y: 20 });  // ⚠️ Parenthèses obligatoires !
console.log(x, y);  // 10 20

// Sans parenthèses = erreur
// { x, y } = { x: 10, y: 20 };  // ❌ SyntaxError

// Destructuring partiel
const person = {
  firstName: 'John',
  lastName: 'Doe',
  age: 30,
  city: 'Paris'
};

const { firstName, lastName } = person;  // On prend seulement 2 propriétés
console.log(firstName, lastName);  // John Doe

// Object vide
const {} = {};  // Valide mais inutile
```

### Renommage de variables

```javascript
// Renommer pendant le destructuring
const user = {
  name: 'Alice',
  age: 25
};

// Syntaxe : { propertyName: newVariableName }
const { name: userName, age: userAge } = user;
console.log(userName);  // 'Alice'
console.log(userAge);   // 25
// console.log(name);   // ❌ ReferenceError: name is not defined

// Utile pour éviter les conflits de noms
const { name: firstName } = { name: 'Alice' };
const { name: lastName } = { name: 'Smith' };
console.log(firstName, lastName);  // Alice Smith

// Cas pratique : API avec noms non conventionnels
const apiResponse = {
  user_name: 'Alice',
  user_age: 25,
  user_email: 'alice@email.com'
};

const {
  user_name: name,
  user_age: age,
  user_email: email
} = apiResponse;

console.log(name, age, email);  // Alice 25 alice@email.com

// Renommer avec valeurs par défaut
const { name: userName = 'Anonymous', score: userScore = 0 } = { name: 'Bob' };
console.log(userName);   // 'Bob'
console.log(userScore);  // 0 (valeur par défaut)
```

### Valeurs par défaut

```javascript
// Sans valeur par défaut
const { name, age, city } = { name: 'Alice', age: 25 };
console.log(name);  // 'Alice'
console.log(age);   // 25
console.log(city);  // undefined

// Avec valeur par défaut
const { name, age, city = 'Paris' } = { name: 'Alice', age: 25 };
console.log(city);  // 'Paris' (valeur par défaut)

// La valeur par défaut est utilisée seulement si undefined
const { a = 1, b = 2, c = 3 } = { a: 0, b: null, c: undefined };
console.log(a);  // 0 (valeur fournie, même si falsy)
console.log(b);  // null (null n'active PAS la valeur par défaut)
console.log(c);  // 3 (undefined → valeur par défaut)

// Valeur par défaut avec expression
const { x = 10, y = x * 2 } = {};
console.log(x);  // 10
console.log(y);  // 20 (calculé depuis x)

// Valeur par défaut avec fonction
function getDefaultName() {
  console.log('Getting default name');
  return 'Anonymous';
}

const { name = getDefaultName() } = { name: 'Alice' };
// getDefaultName() n'est PAS appelé car name existe

const { name: name2 = getDefaultName() } = {};
// Affiche "Getting default name"
// name2 = 'Anonymous'

// Cas pratique : options de fonction
function createServer(options = {}) {
  const {
    port = 3000,
    host = 'localhost',
    protocol = 'http',
    timeout = 5000
  } = options;
  
  console.log(`Server running on ${protocol}://${host}:${port}`);
  console.log(`Timeout: ${timeout}ms`);
}

createServer({ port: 8080 });
// Server running on http://localhost:8080
// Timeout: 5000ms

createServer();
// Server running on http://localhost:3000
// Timeout: 5000ms

// Combiner renommage et valeur par défaut
const config = { max_retries: 5 };
const { max_retries: maxRetries = 3, timeout = 1000 } = config;
console.log(maxRetries);  // 5
console.log(timeout);     // 1000
```

### Nested destructuring (destructuring imbriqué)

```javascript
// Object imbriqué
const user = {
  name: 'Alice',
  age: 25,
  address: {
    street: '123 Main St',
    city: 'Paris',
    country: 'France'
  }
};

// Destructuring simple
const { name, address } = user;
console.log(name);     // 'Alice'
console.log(address);  // { street: '123 Main St', city: 'Paris', country: 'France' }

// Destructuring imbriqué
const {
  name,
  address: { city, country }
} = user;

console.log(name);     // 'Alice'
console.log(city);     // 'Paris'
console.log(country);  // 'France'
// console.log(address);  // ❌ ReferenceError: address is not defined
// address n'est pas extrait, seulement city et country

// Pour avoir address ET ses propriétés
const {
  name,
  address,
  address: { city, country }
} = user;

console.log(address);  // { street: '123 Main St', city: 'Paris', country: 'France' }
console.log(city);     // 'Paris'

// Destructuring profond
const data = {
  user: {
    profile: {
      personal: {
        name: 'Alice',
        age: 25
      },
      contact: {
        email: 'alice@email.com',
        phone: '123456789'
      }
    }
  }
};

const {
  user: {
    profile: {
      personal: { name, age },
      contact: { email }
    }
  }
} = data;

console.log(name, age, email);  // Alice 25 alice@email.com

// Avec valeurs par défaut
const userData = {
  name: 'Bob',
  address: {
    city: 'London'
    // pas de country
  }
};

const {
  name,
  address: {
    city,
    country = 'UK'  // valeur par défaut pour propriété imbriquée
  }
} = userData;

console.log(city, country);  // London UK

// Avec renommage
const {
  address: {
    city: userCity,
    country: userCountry = 'Unknown'
  }
} = userData;

console.log(userCity, userCountry);  // London UK

// ⚠️ Attention aux propriétés manquantes
const incomplete = {
  name: 'Alice'
  // pas d'address
};

// Ceci cause une erreur
// const { address: { city } } = incomplete;
// ❌ TypeError: Cannot destructure property 'city' of 'undefined'

// Solution : valeur par défaut pour l'objet parent
const {
  address: { city } = {}  // valeur par défaut = objet vide
} = incomplete;

console.log(city);  // undefined (pas d'erreur)

// Ou avec valeur par défaut complète
const {
  address: { city: finalCity = 'Unknown' } = {}
} = incomplete;

console.log(finalCity);  // 'Unknown'

// Cas pratique : données d'API
const apiData = {
  status: 'success',
  data: {
    user: {
      id: 1,
      profile: {
        name: 'Alice',
        avatar: 'avatar.jpg'
      },
      stats: {
        posts: 42,
        followers: 1337
      }
    }
  }
};

const {
  status,
  data: {
    user: {
      id,
      profile: { name, avatar },
      stats: { posts, followers }
    }
  }
} = apiData;

console.log(status, id, name, posts, followers);
// success 1 Alice 42 1337
```

### Destructuring dans les paramètres de fonction

```javascript
// Ancienne méthode
function greetOld(user) {
  console.log(`Hello, ${user.name}!`);
  console.log(`You are ${user.age} years old.`);
}

// Avec destructuring dans les paramètres
function greet({ name, age }) {
  console.log(`Hello, ${name}!`);
  console.log(`You are ${age} years old.`);
}

greet({ name: 'Alice', age: 25 });
// Hello, Alice!
// You are 25 years old.

// Avec valeurs par défaut
function createUser({ name = 'Anonymous', age = 18, role = 'user' } = {}) {
  return { name, age, role };
}

console.log(createUser({ name: 'Alice', age: 25 }));
// { name: 'Alice', age: 25, role: 'user' }

console.log(createUser({ name: 'Bob' }));
// { name: 'Bob', age: 18, role: 'user' }

console.log(createUser());
// { name: 'Anonymous', age: 18, role: 'user' }

// Destructuring imbriqué dans les paramètres
function displayAddress({ address: { city, country } }) {
  console.log(`${city}, ${country}`);
}

displayAddress({
  name: 'Alice',
  address: {
    city: 'Paris',
    country: 'France'
  }
});
// Paris, France

// Avec rest operator
function processUser({ id, name, ...otherProps }) {
  console.log('ID:', id);
  console.log('Name:', name);
  console.log('Other props:', otherProps);
}

processUser({
  id: 1,
  name: 'Alice',
  age: 25,
  email: 'alice@email.com',
  city: 'Paris'
});
// ID: 1
// Name: Alice
// Other props: { age: 25, email: 'alice@email.com', city: 'Paris' }

// Cas pratique : options de configuration
function fetchData({
  url,
  method = 'GET',
  headers = {},
  body = null,
  timeout = 5000
}) {
  console.log(`Fetching ${url} with ${method}`);
  console.log('Headers:', headers);
  console.log('Timeout:', timeout);
  
  // ... fetch logic
}

fetchData({
  url: '/api/users',
  headers: { 'Authorization': 'Bearer token' }
});
// Fetching /api/users with GET
// Headers: { Authorization: 'Bearer token' }
// Timeout: 5000

// Cas pratique React : Props destructuring
function UserCard({ user: { name, age, avatar }, onEdit, onDelete }) {
  return (
    <div className="user-card">
      <img src={avatar} alt={name} />
      <h2>{name}</h2>
      <p>Age: {age}</p>
      <button onClick={onEdit}>Edit</button>
      <button onClick={onDelete}>Delete</button>
    </div>
  );
}

// Ou plus simple
function UserCard({ name, age, avatar, onEdit, onDelete }) {
  return (
    <div className="user-card">
      <img src={avatar} alt={name} />
      <h2>{name}</h2>
      <p>Age: {age}</p>
      <button onClick={onEdit}>Edit</button>
      <button onClick={onDelete}>Delete</button>
    </div>
  );
}
```

### Rest operator (...) avec objects

```javascript
// Rest collecte les propriétés restantes
const user = {
  id: 1,
  name: 'Alice',
  age: 25,
  email: 'alice@email.com',
  city: 'Paris'
};

const { id, name, ...otherInfo } = user;
console.log(id);         // 1
console.log(name);       // 'Alice'
console.log(otherInfo);  // { age: 25, email: 'alice@email.com', city: 'Paris' }

// Rest doit être le dernier
const { id, ...rest, name } = user;  // ❌ SyntaxError
const { id, name, ...rest } = user;  // ✅ OK

// Cas pratique : supprimer des propriétés (immutable)
const userData = {
  id: 1,
  name: 'Alice',
  password: 'secret123',
  email: 'alice@email.com',
  role: 'admin'
};

// Supprimer password
const { password, ...safeUser } = userData;
console.log(safeUser);
// { id: 1, name: 'Alice', email: 'alice@email.com', role: 'admin' }

// Supprimer plusieurs propriétés
const { password: pwd, role, ...publicUser } = userData;
console.log(publicUser);
// { id: 1, name: 'Alice', email: 'alice@email.com' }

// Cas pratique React : forward props
function Button({ variant, size, ...htmlProps }) {
  const className = `btn btn-${variant} btn-${size}`;
  
  return <button className={className} {...htmlProps} />;
}

// Usage
<Button 
  variant="primary"
  size="large"
  onClick={handleClick}
  disabled={isLoading}
  type="submit"
/>
// variant et size sont utilisés pour le className
// onClick, disabled, type sont passés directement au <button>

// Cas pratique : Séparer les props en catégories
function ComplexComponent({ 
  title, 
  description,
  onSave,
  onCancel,
  ...formProps 
}) {
  return (
    <div>
      <h1>{title}</h1>
      <p>{description}</p>
      <form {...formProps}>
        {/* form content */}
      </form>
      <div>
        <button onClick={onSave}>Save</button>
        <button onClick={onCancel}>Cancel</button>
      </div>
    </div>
  );
}

// Cas pratique : Merge objects en gardant certaines propriétés
const defaults = {
  theme: 'light',
  fontSize: 16,
  language: 'en',
  notifications: true
};

const userSettings = {
  fontSize: 18,
  language: 'fr'
};

// Garder theme et notifications des defaults, le reste de userSettings
const { theme, notifications, ...rest } = defaults;
const finalSettings = {
  theme,
  notifications,
  ...userSettings
};

console.log(finalSettings);
// { theme: 'light', notifications: true, fontSize: 18, language: 'fr' }

// Nested rest
const data = {
  user: {
    id: 1,
    name: 'Alice',
    age: 25,
    email: 'alice@email.com'
  },
  settings: {
    theme: 'dark',
    notifications: true
  }
};

const {
  user: { id, name, ...userRest },
  ...otherData
} = data;

console.log(id, name);      // 1 'Alice'
console.log(userRest);      // { age: 25, email: 'alice@email.com' }
console.log(otherData);     // { settings: { theme: 'dark', notifications: true } }
```

### Combiner Array et Object Destructuring

```javascript
// Array d'objets
const users = [
  { id: 1, name: 'Alice', age: 25 },
  { id: 2, name: 'Bob', age: 30 },
  { id: 3, name: 'Charlie', age: 35 }
];

// Destructurer le premier objet
const [firstUser] = users;
console.log(firstUser);  // { id: 1, name: 'Alice', age: 25 }

// Destructurer les propriétés du premier objet
const [{ name, age }] = users;
console.log(name, age);  // Alice 25

// Destructurer plusieurs objets
const [{ name: name1 }, { name: name2 }] = users;
console.log(name1, name2);  // Alice Bob

// Object contenant des arrays
const data = {
  users: ['Alice', 'Bob', 'Charlie'],
  scores: [85, 92, 78]
};

const { users: [first, second], scores: [score1, score2] } = data;
console.log(first, second);    // Alice Bob
console.log(score1, score2);   // 85 92

// Cas complexe : nested array et object
const response = {
  status: 'success',
  data: {
    users: [
      { id: 1, name: 'Alice', posts: [{ title: 'Post 1' }, { title: 'Post 2' }] },
      { id: 2, name: 'Bob', posts: [{ title: 'Post 3' }] }
    ]
  }
};

const {
  status,
  data: {
    users: [
      { name: firstName, posts: [firstPost] },
      { name: secondName }
    ]
  }
} = response;

console.log(status);      // 'success'
console.log(firstName);   // 'Alice'
console.log(firstPost);   // { title: 'Post 1' }
console.log(secondName);  // 'Bob'

// Itérer avec destructuring
const products = [
  { id: 1, name: 'Laptop', price: 1000 },
  { id: 2, name: 'Phone', price: 500 },
  { id: 3, name: 'Tablet', price: 300 }
];

// forEach avec destructuring
products.forEach(({ name, price }) => {
  console.log(`${name}: $${price}`);
});
// Laptop: $1000
// Phone: $500
// Tablet: $300

// map avec destructuring
const productNames = products.map(({ name }) => name);
console.log(productNames);  // ['Laptop', 'Phone', 'Tablet']

// filter avec destructuring
const expensive = products.filter(({ price }) => price > 400);
console.log(expensive);
// [{ id: 1, name: 'Laptop', price: 1000 }, { id: 2, name: 'Phone', price: 500 }]

// for...of avec destructuring
for (const { id, name, price } of products) {
  console.log(`#${id}: ${name} - $${price}`);
}
// #1: Laptop - $1000
// #2: Phone - $500
// #3: Tablet - $300

// Object.entries avec destructuring
const config = {
  host: 'localhost',
  port: 3000,
  protocol: 'http'
};

for (const [key, value] of Object.entries(config)) {
  console.log(`${key}: ${value}`);
}
// host: localhost
// port: 3000
// protocol: http

// Cas pratique : Promise.all avec destructuring
async function loadData() {
  const [
    { data: users },
    { data: posts },
    { data: comments }
  ] = await Promise.all([
    fetch('/api/users').then(r => r.json()),
    fetch('/api/posts').then(r => r.json()),
    fetch('/api/comments').then(r => r.json())
  ]);
  
  return { users, posts, comments };
}

// Cas pratique React : liste avec destructuring
function UserList({ users }) {
  return (
    <ul>
      {users.map(({ id, name, email }) => (
        <li key={id}>
          {name} - {email}
        </li>
      ))}
    </ul>
  );
}
```

### Cas pratiques avancés

```javascript
// 1. Fonction avec options complexes
function createChart({
  type = 'bar',
  data: { labels, values },
  options: {
    title = 'Chart',
    colors = ['blue', 'red', 'green'],
    legend: { show: showLegend = true, position: legendPosition = 'top' } = {}
  } = {}
}) {
  console.log('Type:', type);
  console.log('Labels:', labels);
  console.log('Values:', values);
  console.log('Title:', title);
  console.log('Colors:', colors);
  console.log('Show legend:', showLegend);
  console.log('Legend position:', legendPosition);
}

createChart({
  data: {
    labels: ['Jan', 'Feb', 'Mar'],
    values: [10, 20, 30]
  },
  options: {
    title: 'Sales 2024',
    legend: { position: 'bottom' }
  }
});
// Type: bar
// Labels: ['Jan', 'Feb', 'Mar']
// Values: [10, 20, 30]
// Title: Sales 2024
// Colors: ['blue', 'red', 'green']
// Show legend: true
// Legend position: bottom

// 2. Transformer des données d'API
const apiUsers = [
  {
    id: 1,
    first_name: 'Alice',
    last_name: 'Smith',
    contact: {
      email: 'alice@email.com',
      phone: '123456789'
    }
  },
  {
    id: 2,
    first_name: 'Bob',
    last_name: 'Jones',
    contact: {
      email: 'bob@email.com',
      phone: '987654321'
    }
  }
];

// Transformer en format plus propre
const cleanUsers = apiUsers.map(({
  id,
  first_name: firstName,
  last_name: lastName,
  contact: { email, phone }
}) => ({
  id,
  fullName: `${firstName} ${lastName}`,
  email,
  phone
}));

console.log(cleanUsers);
// [
//   { id: 1, fullName: 'Alice Smith', email: 'alice@email.com', phone: '123456789' },
//   { id: 2, fullName: 'Bob Jones', email: 'bob@email.com', phone: '987654321' }
// ]

// 3. Validation avec destructuring
function validateUser({
  name = throwError('Name is required'),
  email = throwError('Email is required'),
  age = 18
}) {
  return { name, email, age };
}

function throwError(message) {
  throw new Error(message);
}

try {
  validateUser({ name: 'Alice' });  // Error: Email is required
} catch (error) {
  console.error(error.message);
}

// 4. Swap de propriétés d'objets
let user1 = { name: 'Alice', role: 'admin' };
let user2 = { name: 'Bob', role: 'user' };

// Échanger les rôles
({ role: user1.role, role: user2.role } = { role: user2.role, role: user1.role });
console.log(user1);  // { name: 'Alice', role: 'user' }
console.log(user2);  // { name: 'Bob', role: 'admin' }

// Plus simple avec array destructuring
[user1.role, user2.role] = [user2.role, user1.role];

// 5. Extraire des propriétés conditionnellement
function getUserInfo(user, includePrivate = false) {
  const { name, email } = user;
  
  if (includePrivate) {
    const { password, ...rest } = user;
    return rest;  // Tout sauf password
  }
  
  return { name, email };  // Seulement public
}

const user = {
  id: 1,
  name: 'Alice',
  email: 'alice@email.com',
  password: 'secret',
  role: 'admin'
};

console.log(getUserInfo(user, false));
// { name: 'Alice', email: 'alice@email.com' }

console.log(getUserInfo(user, true));
// { id: 1, name: 'Alice', email: 'alice@email.com', role: 'admin' }

// 6. Destructuring dynamique
function getProperty(obj, path) {
  const keys = path.split('.');
  let result = obj;
  
  for (const key of keys) {
    result = result?.[key];
  }
  
  return result;
}

const data = {
  user: {
    profile: {
      name: 'Alice'
    }
  }
};

console.log(getProperty(data, 'user.profile.name'));  // 'Alice'

// 7. Destructuring avec computed property names
const key = 'email';
const { [key]: userEmail } = { name: 'Alice', email: 'alice@email.com' };
console.log(userEmail);  // 'alice@email.com'

// Renommer avec computed property
const fieldName = 'user_email';
const { [fieldName]: email } = { user_email: 'test@email.com' };
console.log(email);  // 'test@email.com'

// 8. Destructuring dans catch
try {
  throw { code: 404, message: 'Not found', details: { url: '/api/users' } };
} catch ({ code, message, details: { url } }) {
  console.log(`Error ${code}: ${message} at ${url}`);
  // Error 404: Not found at /api/users
}

// 9. Créer un subset d'objet
function pick(obj, ...keys) {
  return keys.reduce((acc, key) => {
    if (key in obj) {
      acc[key] = obj[key];
    }
    return acc;
  }, {});
}

const fullUser = {
  id: 1,
  name: 'Alice',
  email: 'alice@email.com',
  password: 'secret',
  role: 'admin',
  createdAt: '2024-01-01'
};

const publicUser = pick(fullUser, 'id', 'name', 'email');
console.log(publicUser);
// { id: 1, name: 'Alice', email: 'alice@email.com' }

// 10. Pattern matching (simulé)
function handleResponse(response) {
  const { status, data, error } = response;
  
  switch (status) {
    case 'success': {
      const { user, token } = data;
      console.log('Login successful:', user.name);
      console.log('Token:', token);
      break;
    }
    case 'error': {
      const { code, message } = error;
      console.error(`Error ${code}: ${message}`);
      break;
    }
    default:
      console.log('Unknown status');
  }
}

handleResponse({
  status: 'success',
  data: {
    user: { id: 1, name: 'Alice' },
    token: 'abc123'
  }
});
// Login successful: Alice
// Token: abc123
```

---

# Chapitre 7 : Destructuring - Questions/Réponses Orales

## Questions sur Array Destructuring

### Q1: Qu'est-ce que le destructuring et pourquoi l'utiliser ?

**Réponse :**

Le destructuring est une syntaxe qui permet d'**extraire des valeurs** d'arrays ou d'objets et de les assigner à des variables de manière concise.

```javascript
// Sans destructuring (ancienne méthode)
const colors = ['red', 'green', 'blue'];
const first = colors[0];
const second = colors[1];
const third = colors[2];

// Avec destructuring (moderne)
const [first, second, third] = ['red', 'green', 'blue'];
console.log(first, second, third);  // red green blue
```

**Avantages :**
- ✅ Code plus court et lisible
- ✅ Moins de répétition
- ✅ Extraction en une seule ligne
- ✅ Parfait pour les fonctions qui retournent plusieurs valeurs
- ✅ Essentiel en React (useState, props, etc.)

---

### Q2: Comment échanger (swap) deux variables avec le destructuring ?

**Réponse :**

C'est l'un des cas d'usage les plus élégants du destructuring :

```javascript
let a = 1;
let b = 2;

// Ancienne méthode (avec variable temporaire)
let temp = a;
a = b;
b = temp;
console.log(a, b);  // 2 1

// Avec destructuring (sans variable temporaire)
[a, b] = [b, a];
console.log(a, b);  // 1 2

// Swap multiple variables
let x = 1, y = 2, z = 3;
[x, y, z] = [z, x, y];
console.log(x, y, z);  // 3 1 2

// Cas pratique : trier deux valeurs
let min = 10, max = 5;
if (min > max) {
  [min, max] = [max, min];
}
console.log(min, max);  // 5 10
```

**Point clé :** Pas besoin de variable temporaire, JavaScript gère tout en interne.

---

### Q3: Comment ignorer certains éléments lors du destructuring d'un array ?

**Réponse :**

On utilise des **virgules vides** pour ignorer des éléments :

```javascript
const numbers = [1, 2, 3, 4, 5];

// Ignorer le deuxième élément
const [first, , third] = numbers;
console.log(first, third);  // 1 3

// Ignorer plusieurs éléments
const [a, , , d] = [1, 2, 3, 4, 5];
console.log(a, d);  // 1 4

// Ignorer les premiers
const [, , third, fourth] = [1, 2, 3, 4];
console.log(third, fourth);  // 3 4

// Cas pratique : split avec destructuring
const fullName = 'John Michael Doe';
const [firstName, , lastName] = fullName.split(' ');
console.log(firstName, lastName);  // John Doe (on ignore 'Michael')

// Cas pratique : coordonnées
const coords = [48.8566, 2.3522, 35];  // lat, lng, altitude
const [lat, lng] = coords;  // on ignore automatiquement l'altitude
console.log(lat, lng);  // 48.8566 2.3522
```

---

### Q4: Qu'est-ce que le rest operator (`...`) dans le destructuring ? Comment l'utiliser ?

**Réponse :**

Le rest operator **collecte les éléments restants** dans un array.

```javascript
const numbers = [1, 2, 3, 4, 5];

// Extraire le premier, collecter le reste
const [first, ...rest] = numbers;
console.log(first);  // 1
console.log(rest);   // [2, 3, 4, 5]

// ⚠️ rest doit être le DERNIER élément
const [a, b, ...others] = [1, 2, 3, 4, 5];
console.log(a, b, others);  // 1 2 [3, 4, 5]

// ❌ Ceci ne fonctionne pas
// const [first, ...middle, last] = [1, 2, 3, 4, 5];  // SyntaxError

// Combiner skip et rest
const [first, , third, ...rest] = [1, 2, 3, 4, 5, 6];
console.log(first, third, rest);  // 1 3 [4, 5, 6]

// Si rien à collecter, rest = []
const [x, y, ...remaining] = [1, 2];
console.log(remaining);  // []
```

**Cas pratiques :**

```javascript
// 1. Séparer head et tail
const [head, ...tail] = [1, 2, 3, 4, 5];
console.log('Head:', head);  // 1
console.log('Tail:', tail);  // [2, 3, 4, 5]

// 2. Fonction avec arguments variables
function sum(first, ...numbers) {
  return numbers.reduce((acc, n) => acc + n, first);
}
console.log(sum(1, 2, 3, 4, 5));  // 15

// 3. Cloner un array
const original = [1, 2, 3];
const [...clone] = original;
console.log(clone === original);  // false
```

**Important :** `...` signifie **rest** dans le destructuring (collecte), mais **spread** dans un array literal (étale).

---

### Q5: Comment définir des valeurs par défaut dans le destructuring d'array ?

**Réponse :**

On utilise `=` pour définir des valeurs par défaut qui s'appliquent **seulement si la valeur est undefined**.

```javascript
// Sans valeur par défaut
const [a, b, c] = [1, 2];
console.log(c);  // undefined

// Avec valeur par défaut
const [x, y, z = 3] = [1, 2];
console.log(x, y, z);  // 1 2 3

// ⚠️ La valeur par défaut s'applique seulement pour undefined
const [m, n = 10] = [5, null];
console.log(m, n);  // 5 null (null n'active PAS la valeur par défaut)

const [p, q = 10] = [5, undefined];
console.log(p, q);  // 5 10 (undefined active la valeur par défaut)

// Valeur par défaut avec expression
const [a = 1, b = a * 2] = [];
console.log(a, b);  // 1 2

// Valeur par défaut avec fonction
function getDefault() {
  console.log('Getting default');
  return 42;
}

const [value = getDefault()] = [100];
// getDefault() n'est PAS appelé (car value = 100)

const [value2 = getDefault()] = [];
// Affiche "Getting default", value2 = 42
```

**Cas pratique : options de fonction**

```javascript
function createUser([name = 'Anonymous', age = 18, role = 'user'] = []) {
  return { name, age, role };
}

console.log(createUser(['Alice', 25, 'admin']));
// { name: 'Alice', age: 25, role: 'admin' }

console.log(createUser(['Bob']));
// { name: 'Bob', age: 18, role: 'user' }

console.log(createUser());
// { name: 'Anonymous', age: 18, role: 'user' }
```

---

### Q6: Comment destructurer des arrays imbriqués ?

**Réponse :**

On utilise des **crochets imbriqués** pour accéder aux éléments des sous-arrays.

```javascript
// Array 2D
const matrix = [
  [1, 2],
  [3, 4],
  [5, 6]
];

// Destructuring simple
const [firstRow, secondRow] = matrix;
console.log(firstRow);   // [1, 2]
console.log(secondRow);  // [3, 4]

// Destructuring imbriqué
const [[a, b], [c, d]] = [
  [1, 2],
  [3, 4]
];
console.log(a, b, c, d);  // 1 2 3 4

// Destructuring profond
const nested = [1, [2, [3, [4, 5]]]];
const [first, [second, [third, [fourth, fifth]]]] = nested;
console.log(first, second, third, fourth, fifth);  // 1 2 3 4 5

// Mélanger skip et nested
const data = [
  [1, 2, 3],
  [4, 5, 6],
  [7, 8, 9]
];

const [[, , third], , [seventh]] = data;
console.log(third, seventh);  // 3 7

// Avec rest
const [[first, ...restOfFirst], ...restOfArray] = [
  [1, 2, 3],
  [4, 5, 6]
];
console.log(first);          // 1
console.log(restOfFirst);    // [2, 3]
console.log(restOfArray);    // [[4, 5, 6]]
```

**Cas pratique : coordonnées multiples**

```javascript
const locations = [
  ['Paris', [48.8566, 2.3522]],
  ['London', [51.5074, -0.1278]]
];

for (const [city, [lat, lng]] of locations) {
  console.log(`${city}: ${lat}, ${lng}`);
}
// Paris: 48.8566, 2.3522
// London: 51.5074, -0.1278
```

---

## Questions sur Object Destructuring

### Q7: Quelle est la différence entre array destructuring et object destructuring ?

**Réponse :**

**Array destructuring :**
- Basé sur la **position** (ordre important)
- Utilise des **crochets** `[]`
- Les noms de variables sont libres

**Object destructuring :**
- Basé sur les **noms de propriétés** (ordre n'importe pas)
- Utilise des **accolades** `{}`
- Les noms de variables doivent correspondre aux propriétés (sauf si renommage)

```javascript
// ARRAY - basé sur la position
const [first, second] = [1, 2, 3];
console.log(first, second);  // 1 2 (peu importe le nom)

const [a, b] = [1, 2, 3];  // même résultat
console.log(a, b);  // 1 2

// OBJECT - basé sur le nom des propriétés
const { name, age } = { name: 'Alice', age: 25, city: 'Paris' };
console.log(name, age);  // Alice 25

// L'ordre n'a pas d'importance
const { age, name } = { name: 'Bob', age: 30 };
console.log(name, age);  // Bob 30

// Le nom DOIT correspondre
const { firstName } = { name: 'Alice' };
console.log(firstName);  // undefined (propriété n'existe pas)
```

**Tableau comparatif :**

| Critère | Array | Object |
|---------|-------|--------|
| Syntaxe | `[]` | `{}` |
| Basé sur | Position | Nom de propriété |
| Ordre | Important | Pas important |
| Noms variables | Libres | Doivent correspondre |
| Renommage | Pas besoin | Possible avec `:` |

---

### Q8: Comment renommer des variables lors du destructuring d'objet ?

**Réponse :**

On utilise la syntaxe `{ propertyName: newVariableName }` :

```javascript
const user = {
  name: 'Alice',
  age: 25
};

// Renommer pendant le destructuring
const { name: userName, age: userAge } = user;
console.log(userName);  // 'Alice'
console.log(userAge);   // 25
// console.log(name);   // ❌ ReferenceError (name n'existe pas)

// Utile pour éviter les conflits
const { name: firstName } = { name: 'Alice' };
const { name: lastName } = { name: 'Smith' };
console.log(firstName, lastName);  // Alice Smith

// Renommer avec valeurs par défaut
const { name: userName = 'Anonymous', score: userScore = 0 } = { name: 'Bob' };
console.log(userName, userScore);  // Bob 0
```

**Cas pratique : données d'API**

```javascript
// API retourne des noms non conventionnels
const apiResponse = {
  user_name: 'Alice',
  user_age: 25,
  user_email: 'alice@email.com'
};

// Renommer en camelCase
const {
  user_name: name,
  user_age: age,
  user_email: email
} = apiResponse;

console.log(name, age, email);  // Alice 25 alice@email.com

// Créer un objet propre
const cleanUser = { name, age, email };
```

---

### Q9: Comment destructurer des objets imbriqués ?

**Réponse :**

On utilise des **accolades imbriquées** pour accéder aux propriétés des sous-objets.

```javascript
const user = {
  name: 'Alice',
  age: 25,
  address: {
    street: '123 Main St',
    city: 'Paris',
    country: 'France'
  }
};

// Destructuring simple (obtenir l'objet address)
const { name, address } = user;
console.log(address);  // { street: '123 Main St', city: 'Paris', country: 'France' }

// Destructuring imbriqué (obtenir les propriétés de address)
const {
  name,
  address: { city, country }
} = user;

console.log(name);     // 'Alice'
console.log(city);     // 'Paris'
console.log(country);  // 'France'
// console.log(address);  // ❌ ReferenceError (address n'a pas été extrait)

// Pour avoir address ET ses propriétés
const {
  name,
  address,
  address: { city, country }
} = user;

console.log(address);  // { street: '123 Main St', city: 'Paris', country: 'France' }
console.log(city);     // 'Paris'
```

**⚠️ Attention aux propriétés manquantes :**

```javascript
const incomplete = {
  name: 'Alice'
  // pas d'address
};

// Ceci cause une erreur
// const { address: { city } } = incomplete;
// ❌ TypeError: Cannot destructure property 'city' of 'undefined'

// ✅ Solution : valeur par défaut pour l'objet parent
const {
  address: { city } = {}
} = incomplete;

console.log(city);  // undefined (pas d'erreur)

// Ou avec valeur par défaut complète
const {
  address: { city: finalCity = 'Unknown' } = {}
} = incomplete;

console.log(finalCity);  // 'Unknown'
```

**Cas pratique : données d'API profondes**

```javascript
const apiData = {
  status: 'success',
  data: {
    user: {
      id: 1,
      profile: {
        name: 'Alice',
        avatar: 'avatar.jpg'
      },
      stats: {
        posts: 42,
        followers: 1337
      }
    }
  }
};

const {
  status,
  data: {
    user: {
      id,
      profile: { name, avatar },
      stats: { posts, followers }
    }
  }
} = apiData;

console.log(status, id, name, posts, followers);
// success 1 Alice 42 1337
```

---

### Q10: Comment utiliser le rest operator avec les objets ?

**Réponse :**

Le rest operator collecte les propriétés restantes dans un nouvel objet.

```javascript
const user = {
  id: 1,
  name: 'Alice',
  age: 25,
  email: 'alice@email.com',
  city: 'Paris'
};

// Extraire id et name, collecter le reste
const { id, name, ...otherInfo } = user;
console.log(id, name);      // 1 'Alice'
console.log(otherInfo);     // { age: 25, email: 'alice@email.com', city: 'Paris' }

// ⚠️ rest doit être le dernier
const { id, ...rest, name } = user;  // ❌ SyntaxError
const { id, name, ...rest } = user;  // ✅ OK
```

**Cas pratique #1 : Supprimer des propriétés (immutable)**

```javascript
const userData = {
  id: 1,
  name: 'Alice',
  password: 'secret123',
  email: 'alice@email.com',
  role: 'admin'
};

// Supprimer password
const { password, ...safeUser } = userData;
console.log(safeUser);
// { id: 1, name: 'Alice', email: 'alice@email.com', role: 'admin' }

// Supprimer plusieurs propriétés
const { password: pwd, role, ...publicUser } = userData;
console.log(publicUser);
// { id: 1, name: 'Alice', email: 'alice@email.com' }
```

**Cas pratique #2 : React props forwarding**

```javascript
function Button({ variant, size, ...htmlProps }) {
  const className = `btn btn-${variant} btn-${size}`;
  
  return <button className={className} {...htmlProps} />;
}

// Usage
<Button 
  variant="primary" 
  size="large"
  onClick={handleClick}
  disabled={isLoading}
  type="submit"
/>
// variant et size → className
// onClick, disabled, type → passés au <button>
```

---

### Q11: Comment destructurer dans les paramètres de fonction ?

**Réponse :**

On peut destructurer directement dans la signature de la fonction.

```javascript
// Ancienne méthode
function greetOld(user) {
  console.log(`Hello, ${user.name}!`);
}

// Avec destructuring
function greet({ name, age }) {
  console.log(`Hello, ${name}!`);
  console.log(`You are ${age} years old.`);
}

greet({ name: 'Alice', age: 25 });
// Hello, Alice!
// You are 25 years old.

// Avec valeurs par défaut
function createUser({ 
  name = 'Anonymous', 
  age = 18, 
  role = 'user' 
} = {}) {
  return { name, age, role };
}

console.log(createUser({ name: 'Alice', age: 25 }));
// { name: 'Alice', age: 25, role: 'user' }

console.log(createUser({ name: 'Bob' }));
// { name: 'Bob', age: 18, role: 'user' }

console.log(createUser());
// { name: 'Anonymous', age: 18, role: 'user' }
```

**Avec rest operator :**

```javascript
function processUser({ id, name, ...otherProps }) {
  console.log('ID:', id);
  console.log('Name:', name);
  console.log('Other:', otherProps);
}

processUser({
  id: 1,
  name: 'Alice',
  age: 25,
  email: 'alice@email.com',
  city: 'Paris'
});
// ID: 1
// Name: Alice
// Other: { age: 25, email: 'alice@email.com', city: 'Paris' }
```

**Destructuring imbriqué dans les paramètres :**

```javascript
function displayAddress({ 
  name,
  address: { city, country } 
}) {
  console.log(`${name} from ${city}, ${country}`);
}

displayAddress({
  name: 'Alice',
  address: {
    city: 'Paris',
    country: 'France'
  }
});
// Alice from Paris, France
```

**Cas pratique React : Props destructuring**

```javascript
// Sans destructuring
function UserCard(props) {
  return (
    <div>
      <h2>{props.name}</h2>
      <p>Age: {props.age}</p>
      <button onClick={props.onEdit}>Edit</button>
    </div>
  );
}

// Avec destructuring (plus lisible)
function UserCard({ name, age, onEdit }) {
  return (
    <div>
      <h2>{name}</h2>
      <p>Age: {age}</p>
      <button onClick={onEdit}>Edit</button>
    </div>
  );
}

// Avec valeurs par défaut
function UserCard({ 
  name = 'Unknown', 
  age = 0, 
  avatar = 'default.jpg',
  onEdit 
}) {
  return (
    <div>
      <img src={avatar} alt={name} />
      <h2>{name}</h2>
      <p>Age: {age}</p>
      <button onClick={onEdit}>Edit</button>
    </div>
  );
}
```

---

### Q12: Comment combiner array et object destructuring ?

**Réponse :**

On peut mélanger les deux syntaxes pour des structures de données complexes.

```javascript
// Array d'objets
const users = [
  { id: 1, name: 'Alice', age: 25 },
  { id: 2, name: 'Bob', age: 30 },
  { id: 3, name: 'Charlie', age: 35 }
];

// Destructurer le premier objet
const [{ name, age }] = users;
console.log(name, age);  // Alice 25

// Destructurer plusieurs objets
const [{ name: name1 }, { name: name2 }] = users;
console.log(name1, name2);  // Alice Bob

// Object contenant des arrays
const data = {
  users: ['Alice', 'Bob', 'Charlie'],
  scores: [85, 92, 78]
};

const { 
  users: [first, second], 
  scores: [score1, score2] 
} = data;

console.log(first, second);    // Alice Bob
console.log(score1, score2);   // 85 92
```

**Cas complexe :**

```javascript
const response = {
  status: 'success',
  data: {
    users: [
      { id: 1, name: 'Alice', posts: [{ title: 'Post 1' }] },
      { id: 2, name: 'Bob' }
    ]
  }
};

const {
  status,
  data: {
    users: [
      { name: firstName, posts: [firstPost] },
      { name: secondName }
    ]
  }
} = response;

console.log(status);      // 'success'
console.log(firstName);   // 'Alice'
console.log(firstPost);   // { title: 'Post 1' }
console.log(secondName);  // 'Bob'
```

**Dans les boucles :**

```javascript
const products = [
  { id: 1, name: 'Laptop', price: 1000 },
  { id: 2, name: 'Phone', price: 500 }
];

// forEach avec destructuring
products.forEach(({ name, price }) => {
  console.log(`${name}: $${price}`);
});
// Laptop: $1000
// Phone: $500

// for...of avec destructuring
for (const { id, name, price } of products) {
  console.log(`#${id}: ${name} - $${price}`);
}

// map avec destructuring
const names = products.map(({ name }) => name);
console.log(names);  // ['Laptop', 'Phone']
```

---

## Questions Pièges

### Q13: Que se passe-t-il ici ? `{ a, b } = { a: 1, b: 2 }`

**Réponse :**

```javascript
{ a, b } = { a: 1, b: 2 };  // ❌ SyntaxError: Unexpected token '='

// Pourquoi ? JavaScript interprète `{ a, b }` comme un BLOC de code,
// pas comme du destructuring !
```

**Solutions :**

```javascript
// 1. Avec const/let/var (recommandé)
const { a, b } = { a: 1, b: 2 };
console.log(a, b);  // 1 2

// 2. Avec parenthèses (si variables déjà déclarées)
let a, b;
({ a, b } = { a: 1, b: 2 });  // ✅ Fonctionne
console.log(a, b);  // 1 2

// Les parenthèses forcent JavaScript à interpréter comme une expression
```

**Piège similaire avec array (pas de problème) :**

```javascript
[a, b] = [1, 2];  // ✅ Pas de problème avec les arrays
// Car [] ne peut pas être un bloc de code
```

---

### Q14: Quelle est la différence entre ces deux syntaxes ?

```javascript
const { name: userName } = user;
const { name = 'Anonymous' } = user;
```

**Réponse :**

**Très différent !**

```javascript
const user = { name: 'Alice', age: 25 };

// 1. Renommage (name → userName)
const { name: userName } = user;
console.log(userName);  // 'Alice'
// console.log(name);   // ❌ ReferenceError (name n'existe pas)

// 2. Valeur par défaut
const { name = 'Anonymous' } = user;
console.log(name);  // 'Alice' (propriété existe)

const { email = 'no-email' } = user;
console.log(email);  // 'no-email' (propriété n'existe pas)

// Combiner les deux
const { name: userName = 'Anonymous' } = user;
// name → userName, avec valeur par défaut si undefined
```

**Tableau comparatif :**

| Syntaxe | Signification | Exemple |
|---------|---------------|---------|
| `{ name }` | Extraire `name` | `name = user.name` |
| `{ name: userName }` | Renommer en `userName` | `userName = user.name` |
| `{ name = 'default' }` | Valeur par défaut | `name = user.name ?? 'default'` |
| `{ name: userName = 'default' }` | Renommer + défaut | `userName = user.name ?? 'default'` |

---

### Q15: Que retourne ce code ?

```javascript
const [a, b] = [1];
console.log(a, b);  // ?
```

**Réponse :**

```javascript
const [a, b] = [1];
console.log(a);  // 1
console.log(b);  // undefined

// Si plus de variables que d'éléments, les variables supplémentaires = undefined
```

**Avec valeur par défaut :**

```javascript
const [a, b = 2] = [1];
console.log(a);  // 1
console.log(b);  // 2 (valeur par défaut)

const [x, y = 10] = [5, undefined];
console.log(x);  // 5
console.log(y);  // 10 (undefined → valeur par défaut)

const [m, n = 10] = [5, null];
console.log(m);  // 5
console.log(n);  // null (null n'active PAS la valeur par défaut)
```

---

### Q16: Pourquoi ce code ne fonctionne-t-il pas ?

```javascript
const [first, ...middle, last] = [1, 2, 3, 4, 5];
```

**Réponse :**

```javascript
const [first, ...middle, last] = [1, 2, 3, 4, 5];
// ❌ SyntaxError: Rest element must be last element

// Le rest operator (...) DOIT être le dernier élément !
```

**Solutions :**

```javascript
// ✅ Rest en dernier
const [first, ...rest] = [1, 2, 3, 4, 5];
console.log(first);  // 1
console.log(rest);   // [2, 3, 4, 5]

const [first, second, ...rest] = [1, 2, 3, 4, 5];
console.log(first, second, rest);  // 1 2 [3, 4, 5]

// Si vous voulez le dernier élément séparément
const [first, ...rest] = [1, 2, 3, 4, 5];
const last = rest[rest.length - 1];
console.log(first, last);  // 1 5

// Ou avec slice
const arr = [1, 2, 3, 4, 5];
const first = arr[0];
const last = arr[arr.length - 1];
const middle = arr.slice(1, -1);
console.log(first, middle, last);  // 1 [2, 3, 4] 5
```

---

### Q17: Que fait ce code ?

```javascript
const { a: { b } } = { a: null };
```

**Réponse :**

```javascript
const { a: { b } } = { a: null };
// ❌ TypeError: Cannot destructure property 'b' of 'null'

// On essaie d'accéder à b dans null, ce qui est impossible
```

**Solutions :**

```javascript
// 1. Vérifier avant
const obj = { a: null };
if (obj.a) {
  const { a: { b } } = obj;
}

// 2. Valeur par défaut pour l'objet parent
const { a: { b } = {} } = { a: null };
console.log(b);  // undefined (pas d'erreur)

// 3. Optional chaining (pour lecture, pas destructuring)
const obj = { a: null };
const b = obj.a?.b;
console.log(b);  // undefined

// 4. Valeur par défaut complète
const {
  a: { b = 'default' } = { b: 'default' }
} = { a: null };
console.log(b);  // 'default'
```

**Important pour React :** Toujours prévoir des valeurs par défaut quand on destructure des données d'API qui peuvent être null/undefined.

---

### Q18: Quelle est la différence entre rest et spread operator ?

**Réponse :**

**Même syntaxe (`...`) mais utilisations OPPOSÉES :**

**Rest (collecte) :**
- Utilisé dans le **destructuring** ou les **paramètres de fonction**
- **Collecte** plusieurs éléments en un seul array/object

**Spread (étale) :**
- Utilisé dans les **array literals** `[]` ou **object literals** `{}`
- **Étale** un array/object en éléments individuels

```javascript
// REST - Collecte (dans destructuring)
const [first, ...rest] = [1, 2, 3, 4, 5];
console.log(first);  // 1
console.log(rest);   // [2, 3, 4, 5] - collecte le reste

const { name, ...otherProps } = { name: 'Alice', age: 25, city: 'Paris' };
console.log(name);        // 'Alice'
console.log(otherProps);  // { age: 25, city: 'Paris' } - collecte le reste

// REST - Collecte (dans paramètres de fonction)
function sum(first, ...numbers) {
  return numbers.reduce((acc, n) => acc + n, first);
}
console.log(sum(1, 2, 3, 4, 5));  // 15
// ...numbers collecte [2, 3, 4, 5]

// SPREAD - Étale (dans array literal)
const arr1 = [1, 2, 3];
const arr2 = [...arr1, 4, 5];  // Étale arr1
console.log(arr2);  // [1, 2, 3, 4, 5]

// SPREAD - Étale (dans object literal)
const user = { name: 'Alice', age: 25 };
const updatedUser = { ...user, city: 'Paris' };  // Étale user
console.log(updatedUser);  // { name: 'Alice', age: 25, city: 'Paris' }

// SPREAD - Étale (dans appel de fonction)
const numbers = [1, 2, 3];
console.log(Math.max(...numbers));  // 3
// Équivalent à Math.max(1, 2, 3)
```

**Mnémonique :**
- **Rest** = à **gauche** de `=` → collecte
- **Spread** = à **droite** de `=` → étale

```javascript
// REST (à gauche)
const [...copy] = [1, 2, 3];          // collecte
const { ...obj } = { a: 1, b: 2 };    // collecte

// SPREAD (à droite)
const copy = [...[1, 2, 3]];          // étale
const obj = { ...{ a: 1, b: 2 } };    // étale
```

---

### Q19: Comment destructurer des propriétés avec des noms dynamiques (computed property names) ?

**Réponse :**

On utilise des **crochets** `[]` pour les noms de propriétés calculés :

```javascript
const key = 'email';
const user = {
  name: 'Alice',
  email: 'alice@email.com',
  age: 25
};

// Destructuring avec computed property name
const { [key]: userEmail } = user;
console.log(userEmail);  // 'alice@email.com'

// Avec renommage
const fieldName = 'user_email';
const apiData = { user_email: 'test@email.com' };
const { [fieldName]: email } = apiData;
console.log(email);  // 'test@email.com'

// Avec plusieurs propriétés
const prop1 = 'firstName';
const prop2 = 'lastName';
const person = {
  firstName: 'John',
  lastName: 'Doe',
  age: 30
};

const { [prop1]: first, [prop2]: last } = person;
console.log(first, last);  // John Doe

// Cas pratique : extraire des champs dynamiquement
function extractFields(obj, ...fields) {
  return fields.reduce((acc, field) => {
    acc[field] = obj[field];
    return acc;
  }, {});
}

const user = { id: 1, name: 'Alice', email: 'alice@email.com', password: 'secret' };
const safe = extractFields(user, 'id', 'name', 'email');
console.log(safe);  // { id: 1, name: 'Alice', email: 'alice@email.com' }
```

---

### Q20: Que se passe-t-il avec ce code ?

```javascript
const user = { name: 'Alice' };
const { name, name } = user;
```

**Réponse :**

```javascript
const { name, name } = user;
// ❌ SyntaxError: Identifier 'name' has already been declared

// On ne peut pas déclarer la même variable deux fois
```

**Solutions si on veut deux noms différents :**

```javascript
// Renommer la deuxième occurrence
const { name, name: userName } = user;
console.log(name);      // 'Alice'
console.log(userName);  // 'Alice'

// Ou créer deux variables séparées
const { name: firstName } = user;
const { name: displayName } = user;
console.log(firstName, displayName);  // Alice Alice
```

---

### Q21: Comment destructurer un array retourné par une fonction ?

**Réponse :**

Directement dans l'assignation :

```javascript
// Fonction qui retourne un array
function getCoordinates() {
  return [48.8566, 2.3522];
}

// Destructuring direct
const [lat, lng] = getCoordinates();
console.log(lat, lng);  // 48.8566 2.3522

// Avec valeurs par défaut
function getUser() {
  return ['Alice'];  // pas d'âge
}

const [name, age = 18] = getUser();
console.log(name, age);  // Alice 18

// Avec rest
function getNumbers() {
  return [1, 2, 3, 4, 5];
}

const [first, second, ...rest] = getNumbers();
console.log(first, second, rest);  // 1 2 [3, 4, 5]

// Cas pratique : useState en React
function useState(initialValue) {
  let state = initialValue;
  
  const setState = (newValue) => {
    state = newValue;
  };
  
  return [state, setState];  // Retourne un array
}

// Destructuring immédiat
const [count, setCount] = useState(0);
const [name, setName] = useState('');
const [isOpen, setIsOpen] = useState(false);

// Les noms sont libres car c'est un array
const [value, updateValue] = useState(0);  // Fonctionne aussi
```

---

### Q22: Peut-on utiliser le destructuring avec des types primitifs ?

**Réponse :**

**Avec arrays : OUI** (strings sont itérables)

**Avec objects : NON** (primitives n'ont pas de propriétés énumérables)

```javascript
// STRINGS (itérables comme arrays)
const str = 'hello';
const [first, second, ...rest] = str;
console.log(first);   // 'h'
console.log(second);  // 'e'
console.log(rest);    // ['l', 'l', 'o']

// NUMBERS - ne fonctionne pas
const num = 123;
// const [a, b, c] = num;  // ❌ TypeError: num is not iterable

// OBJECTS avec primitives
const { length } = 'hello';
console.log(length);  // 5 (propriété de String.prototype)

const { toFixed } = 123;
console.log(typeof toFixed);  // 'function' (méthode de Number.prototype)

// Mais pas de destructuring pratique avec primitives
```

**Point clé :** Le destructuring est principalement pour arrays et objects, pas pour les primitives.

---

### Q23: Comment destructurer des valeurs d'une Map ou d'un Set ?

**Réponse :**

**Map** et **Set** sont **itérables**, on peut donc utiliser array destructuring avec `for...of` ou spread :

```javascript
// MAP
const map = new Map([
  ['name', 'Alice'],
  ['age', 25],
  ['city', 'Paris']
]);

// Avec for...of et destructuring
for (const [key, value] of map) {
  console.log(`${key}: ${value}`);
}
// name: Alice
// age: 25
// city: Paris

// Spread en array puis destructuring
const [[firstKey, firstValue], [secondKey, secondValue]] = [...map];
console.log(firstKey, firstValue);    // name Alice
console.log(secondKey, secondValue);  // age 25

// Destructurer entries, keys, values
const [firstEntry] = map.entries();
console.log(firstEntry);  // ['name', 'Alice']

const [firstKey] = map.keys();
console.log(firstKey);  // 'name'

const [firstValue] = map.values();
console.log(firstValue);  // 'Alice'

// SET
const set = new Set([1, 2, 3, 4, 5]);

// Avec for...of
for (const value of set) {
  console.log(value);
}

// Array destructuring
const [first, second, ...rest] = set;
console.log(first);   // 1
console.log(second);  // 2
console.log(rest);    // [3, 4, 5]

// Spread en array
const arr = [...set];
console.log(arr);  // [1, 2, 3, 4, 5]
```

---

### Q24: Comment destructurer des paramètres de callback dans les méthodes d'array ?

**Réponse :**

C'est très courant et très utile :

```javascript
const users = [
  { id: 1, name: 'Alice', age: 25, active: true },
  { id: 2, name: 'Bob', age: 30, active: false },
  { id: 3, name: 'Charlie', age: 35, active: true }
];

// map avec destructuring
const names = users.map(({ name }) => name);
console.log(names);  // ['Alice', 'Bob', 'Charlie']

// Plusieurs propriétés
const summaries = users.map(({ name, age }) => `${name} (${age})`);
console.log(summaries);
// ['Alice (25)', 'Bob (30)', 'Charlie (35)']

// filter avec destructuring
const activeUsers = users.filter(({ active }) => active);
console.log(activeUsers);
// [{ id: 1, name: 'Alice', ... }, { id: 3, name: 'Charlie', ... }]

// find avec destructuring
const bob = users.find(({ name }) => name === 'Bob');
console.log(bob);

// reduce avec destructuring
const totalAge = users.reduce((sum, { age }) => sum + age, 0);
console.log(totalAge);  // 90

// forEach avec destructuring
users.forEach(({ name, age, active }) => {
  console.log(`${name} (${age}): ${active ? 'Active' : 'Inactive'}`);
});

// Destructuring avec index et array
users.forEach(({ name }, index, array) => {
  console.log(`${index + 1}. ${name} (total: ${array.length})`);
});

// Cas pratique : Object.entries
const config = {
  host: 'localhost',
  port: 3000,
  ssl: true
};

Object.entries(config).forEach(([key, value]) => {
  console.log(`${key} = ${value}`);
});
// host = localhost
// port = 3000
// ssl = true

// Transformer en nouveau format
const transformed = Object.entries(config).map(([key, value]) => ({
  setting: key,
  value: value,
  type: typeof value
}));

console.log(transformed);
// [
//   { setting: 'host', value: 'localhost', type: 'string' },
//   { setting: 'port', value: 3000, type: 'number' },
//   { setting: 'ssl', value: true, type: 'boolean' }
// ]
```

---

### Q25: Comment destructurer des objets dans des conditions if ?

**Réponse :**

On ne peut pas destructurer directement dans un `if`, mais il y a des patterns utiles :

```javascript
const user = {
  name: 'Alice',
  role: 'admin',
  permissions: ['read', 'write', 'delete']
};

// ❌ Ceci ne fonctionne pas
// if (const { role } = user) { }  // SyntaxError

// ✅ Solution 1 : Destructurer avant
const { role, permissions } = user;
if (role === 'admin') {
  console.log('User is admin');
}

// ✅ Solution 2 : Dans le bloc
if (user.role === 'admin') {
  const { name, permissions } = user;
  console.log(`${name} has permissions:`, permissions);
}

// ✅ Solution 3 : Fonction avec destructuring
function checkAdmin({ role, name }) {
  if (role === 'admin') {
    console.log(`${name} is an admin`);
    return true;
  }
  return false;
}

checkAdmin(user);

// Pattern courant : vérifier l'existence
const response = {
  status: 'success',
  data: { user: { name: 'Alice' } }
};

// Vérifier avant de destructurer
if (response.data?.user) {
  const { data: { user: { name } } } = response;
  console.log(name);
}

// Ou avec valeurs par défaut
const {
  data: { user: { name = 'Unknown' } = {} } = {}
} = response;

// Pattern avec switch
function handleResponse(response) {
  switch (response.status) {
    case 'success': {
      const { data: { user, token } } = response;
      console.log('Login successful:', user.name);
      break;
    }
    case 'error': {
      const { error: { code, message } } = response;
      console.error(`Error ${code}: ${message}`);
      break;
    }
  }
}
```

---

### Q26: Peut-on destructurer des getters/setters ?

**Réponse :**

**OUI**, le destructuring fonctionne avec les getters :

```javascript
const user = {
  firstName: 'Alice',
  lastName: 'Smith',
  
  // Getter
  get fullName() {
    return `${this.firstName} ${this.lastName}`;
  },
  
  // Setter
  set fullName(name) {
    const [first, last] = name.split(' ');
    this.firstName = first;
    this.lastName = last;
  }
};

// Destructuring appelle le getter
const { fullName } = user;
console.log(fullName);  // 'Alice Smith'

// Le getter est exécuté au moment du destructuring
const { firstName, fullName: name } = user;
console.log(name);  // 'Alice Smith'

// Exemple avec computed property
const obj = {
  _value: 42,
  
  get value() {
    console.log('Getter called');
    return this._value;
  }
};

const { value } = obj;
// Affiche "Getter called"
console.log(value);  // 42

// ⚠️ Le setter n'est PAS utilisé dans le destructuring
// Destructuring est seulement pour LIRE, pas pour ÉCRIRE
```

---

### Q27: Comment destructurer avec des valeurs calculées/transformées ?

**Réponse :**

On ne peut pas transformer directement dans le destructuring, mais on peut combiner avec d'autres techniques :

```javascript
const user = {
  firstName: 'alice',
  lastName: 'smith',
  age: 25
};

// ❌ On ne peut pas faire de transformation dans le destructuring
// const { firstName: firstName.toUpperCase() } = user;  // SyntaxError

// ✅ Solution 1 : Destructurer puis transformer
const { firstName, lastName } = user;
const fullName = `${firstName.toUpperCase()} ${lastName.toUpperCase()}`;
console.log(fullName);  // 'ALICE SMITH'

// ✅ Solution 2 : Fonction wrapper
function getUserDisplay({ firstName, lastName, age }) {
  return {
    fullName: `${firstName} ${lastName}`.toUpperCase(),
    ageGroup: age < 30 ? 'young' : 'adult'
  };
}

const display = getUserDisplay(user);
console.log(display);
// { fullName: 'ALICE SMITH', ageGroup: 'young' }

// ✅ Solution 3 : Destructuring puis map/transform
const users = [
  { firstName: 'alice', lastName: 'smith' },
  { firstName: 'bob', lastName: 'jones' }
];

const formatted = users.map(({ firstName, lastName }) => ({
  firstName: firstName.charAt(0).toUpperCase() + firstName.slice(1),
  lastName: lastName.charAt(0).toUpperCase() + lastName.slice(1)
}));

console.log(formatted);
// [
//   { firstName: 'Alice', lastName: 'Smith' },
//   { firstName: 'Bob', lastName: 'Jones' }
// ]

// ✅ Solution 4 : Valeur par défaut avec transformation
function processData({ value = Date.now() } = {}) {
  return value;
}

console.log(processData());           // timestamp actuel
console.log(processData({ value: 100 }));  // 100
```

---

### Q28: Comment gérer les erreurs de destructuring ?

**Réponse :**

Le destructuring peut échouer si la structure ne correspond pas :

```javascript
// Erreur : destructurer null/undefined
try {
  const { name } = null;  // TypeError
} catch (error) {
  console.error('Error:', error.message);
}

// ✅ Solution 1 : Valeur par défaut pour l'objet entier
const { name } = null ?? {};  // name = undefined (pas d'erreur)

// ✅ Solution 2 : Vérifier avant
const data = null;
if (data) {
  const { name } = data;
  console.log(name);
}

// ✅ Solution 3 : Optional chaining (lecture seulement)
const data = null;
const name = data?.name;  // undefined (pas d'erreur)

// ✅ Solution 4 : try-catch
function safeDestructure(obj) {
  try {
    const { name, age } = obj;
    return { name, age };
  } catch (error) {
    return { name: 'Unknown', age: 0 };
  }
}

console.log(safeDestructure(null));
// { name: 'Unknown', age: 0 }

// Erreur : propriété imbriquée manquante
const user = { name: 'Alice' };  // pas d'address

try {
  const { address: { city } } = user;  // TypeError
} catch (error) {
  console.error('Error:', error.message);
}

// ✅ Solution : valeur par défaut pour objet parent
const { address: { city } = {} } = user;
console.log(city);  // undefined (pas d'erreur)

// Pattern défensif complet
function getUserCity(user) {
  try {
    const {
      address: {
        city = 'Unknown'
      } = {}
    } = user ?? {};
    
    return city;
  } catch (error) {
    console.error('Failed to get city:', error);
    return 'Unknown';
  }
}

console.log(getUserCity(null));           // 'Unknown'
console.log(getUserCity({}));             // 'Unknown'
console.log(getUserCity({ address: {} })); // 'Unknown'
console.log(getUserCity({ address: { city: 'Paris' } })); // 'Paris'
```

---

### Q29: Comment destructurer des propriétés avec des caractères spéciaux dans le nom ?

**Réponse :**

Utilisez des **quotes** et **bracket notation** :

```javascript
const obj = {
  'first-name': 'Alice',
  'user age': 25,
  'email@address': 'alice@email.com',
  '123-id': 'abc'
};

// ❌ Ceci ne fonctionne pas
// const { first-name } = obj;  // SyntaxError

// ✅ Avec quotes et renommage
const {
  'first-name': firstName,
  'user age': age,
  'email@address': email,
  '123-id': id
} = obj;

console.log(firstName, age, email, id);
// Alice 25 alice@email.com abc

// Cas pratique : données d'API avec noms bizarres
const apiResponse = {
  'user-name': 'Bob',
  'user-email': 'bob@email.com',
  'is-active': true
};

const {
  'user-name': name,
  'user-email': email,
  'is-active': active
} = apiResponse;

// Créer un objet propre
const cleanData = { name, email, active };
console.log(cleanData);
// { name: 'Bob', email: 'bob@email.com', active: true }
```

---

### Q30: Quelles sont les meilleures pratiques pour le destructuring ?

**Réponse :**

**✅ Bonnes pratiques :**

```javascript
// 1. Utilisez des valeurs par défaut pour éviter undefined
function createUser({ name = 'Anonymous', age = 18 } = {}) {
  return { name, age };
}

// 2. Destructurez seulement ce dont vous avez besoin
const user = { id: 1, name: 'Alice', email: 'alice@email.com', password: 'secret' };
const { name, email } = user;  // Pas besoin de tout destructurer

// 3. Utilisez des noms descriptifs lors du renommage
const { user_name: userName, user_email: userEmail } = apiData;
// Pas : const { user_name: un, user_email: ue } = apiData;

// 4. Protégez-vous contre null/undefined avec valeurs par défaut
const { address: { city } = {} } = user;

// 5. Préférez le destructuring dans les paramètres de fonction
function greet({ name, age }) {  // ✅ Lisible
  console.log(`Hello ${name}, age ${age}`);
}
// Plutôt que
function greet(user) {  // ❌ Moins lisible
  console.log(`Hello ${user.name}, age ${user.age}`);
}

// 6. Utilisez rest pour le forward de props (React)
function Button({ variant, size, ...props }) {
  return <button className={`btn-${variant} btn-${size}`} {...props} />;
}
```

**❌ Mauvaises pratiques :**

```javascript
// 1. Destructuring trop profond (illisible)
const {
  user: {
    profile: {
      address: {
        coordinates: {
          latitude
        }
      }
    }
  }
} = data;  // ❌ Trop profond

// Mieux : en plusieurs étapes
const { user } = data;
const { profile } = user;
const { address } = profile;
const { coordinates } = address;
const { latitude } = coordinates;

// 2. Destructurer des objets potentiellement null sans protection
const { name } = user;  // ❌ Si user peut être null

// Mieux :
const { name } = user ?? {};  // ✅

// 3. Noms non descriptifs
const { n, e, a } = user;  // ❌ Quoi ?
const { name, email, age } = user;  // ✅ Clair

// 4. Destructurer tout inutilement
const { id, name, email, age, city, country, phone, address } = user;
// Si vous n'utilisez que name et email

// Mieux :
const { name, email } = user;
```

---