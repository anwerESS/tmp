## [**..**](./00_index.md)

# Chapitre 6 : Objects (Objets)

## 6.1 Fondamentaux des Objets

### Création d'objets

```javascript
// Méthode 1 : Object literal (la plus courante)
const person = {
  name: 'Alice',
  age: 25,
  city: 'Paris'
};

// Méthode 2 : Constructor Object
const person2 = new Object();
person2.name = 'Bob';
person2.age = 30;

// Méthode 3 : Object.create()
const personProto = {
  greet() {
    return `Hello, ${this.name}`;
  }
};
const person3 = Object.create(personProto);
person3.name = 'Charlie';

// Object vide
const empty = {};

// Object avec propriétés calculées (computed property names)
const key = 'email';
const user = {
  name: 'Alice',
  [key]: 'alice@email.com',  // propriété dynamique
  ['age' + 'Value']: 25      // ageValue: 25
};
console.log(user.email);      // 'alice@email.com'
console.log(user.ageValue);   // 25

// Shorthand property names (ES6)
const name = 'Bob';
const age = 30;
const user2 = {
  name,    // équivalent à name: name
  age      // équivalent à age: age
};
console.log(user2);  // { name: 'Bob', age: 30 }

// Shorthand method names
const calculator = {
  // Ancienne syntaxe
  add: function(a, b) {
    return a + b;
  },
  // Nouvelle syntaxe (ES6)
  subtract(a, b) {
    return a - b;
  },
  multiply(a, b) {
    return a * b;
  }
};
```

### Accès aux propriétés

```javascript
const user = {
  firstName: 'John',
  lastName: 'Doe',
  age: 30,
  'home address': 'Paris',  // propriété avec espace
  email: 'john@email.com'
};

// Dot notation (notation pointée)
console.log(user.firstName);  // 'John'
console.log(user.age);        // 30

// Bracket notation (notation entre crochets)
console.log(user['firstName']);     // 'John'
console.log(user['home address']);  // 'Paris' - nécessaire pour les espaces

// Accès dynamique avec bracket notation
const key = 'email';
console.log(user[key]);  // 'john@email.com'

// Propriété inexistante retourne undefined
console.log(user.phone);  // undefined

// Vérifier si une propriété existe
console.log('email' in user);        // true
console.log('phone' in user);        // false
console.log(user.hasOwnProperty('email'));  // true

// Optional chaining (?.) - ES2020
const address = user.address?.street?.number;  // undefined (pas d'erreur)
// Sans optional chaining:
// const address = user.address.street.number;  // ❌ Error: Cannot read property 'street' of undefined

// Nested objects (objets imbriqués)
const company = {
  name: 'Tech Corp',
  address: {
    street: '123 Main St',
    city: 'Paris',
    country: 'France',
    coordinates: {
      lat: 48.8566,
      lng: 2.3522
    }
  }
};

console.log(company.address.city);                    // 'Paris'
console.log(company.address.coordinates.lat);         // 48.8566
console.log(company.address.zipCode?.toString());     // undefined (safe)
```

### Ajout/Modification/Suppression de propriétés

```javascript
const user = {
  name: 'Alice',
  age: 25
};

// Ajouter une propriété
user.email = 'alice@email.com';
user['phone'] = '123456789';
console.log(user);
// { name: 'Alice', age: 25, email: 'alice@email.com', phone: '123456789' }

// Modifier une propriété
user.age = 26;
user['name'] = 'Alice Smith';
console.log(user.age);   // 26
console.log(user.name);  // 'Alice Smith'

// Supprimer une propriété
delete user.phone;
console.log(user.phone);  // undefined
console.log('phone' in user);  // false

// delete retourne true si réussi, false sinon
const success = delete user.email;
console.log(success);  // true

// On ne peut pas delete des propriétés héritées
delete user.toString;  // ne supprime pas la méthode héritée
console.log(typeof user.toString);  // 'function'

// Ajouter plusieurs propriétés à la fois
Object.assign(user, {
  city: 'Paris',
  country: 'France',
  active: true
});

// Ou avec spread operator
const updatedUser = {
  ...user,
  city: 'Lyon',
  premium: true
};

// Propriétés avec valeurs undefined vs supprimées
const obj = {
  a: 1,
  b: undefined
};
console.log('b' in obj);  // true - la propriété existe avec valeur undefined
delete obj.b;
console.log('b' in obj);  // false - la propriété n'existe plus
```

### Méthodes d'objets

```javascript
// Méthodes : fonctions définies dans un objet
const calculator = {
  value: 0,
  
  // Méthode avec function keyword
  add: function(n) {
    this.value += n;
    return this;  // pour le chaining
  },
  
  // Méthode shorthand (ES6)
  subtract(n) {
    this.value -= n;
    return this;
  },
  
  multiply(n) {
    this.value *= n;
    return this;
  },
  
  // Getter
  get result() {
    return this.value;
  },
  
  // Setter
  set result(val) {
    this.value = val;
  },
  
  // Méthode avec arrow function (⚠️ pas de this propre)
  reset: () => {
    // this ici ne pointe PAS vers calculator
    console.log(this);  // window (ou undefined en strict mode)
  },
  
  // Bonne pratique pour reset
  clear() {
    this.value = 0;
    return this;
  }
};

// Utilisation
calculator.add(5).multiply(2).subtract(3);
console.log(calculator.result);  // 7

// Utilisation du setter
calculator.result = 100;
console.log(calculator.value);  // 100

// Method chaining
calculator.clear().add(10).multiply(3);
console.log(calculator.result);  // 30

// Exemple pratique : API fluide
const query = {
  filters: {},
  sortBy: null,
  limit: null,
  
  where(field, value) {
    this.filters[field] = value;
    return this;
  },
  
  orderBy(field) {
    this.sortBy = field;
    return this;
  },
  
  take(n) {
    this.limit = n;
    return this;
  },
  
  execute() {
    console.log('Executing query:', this);
    return { ...this };
  }
};

const result = query
  .where('age', 25)
  .where('active', true)
  .orderBy('name')
  .take(10)
  .execute();
```

### Le keyword `this`

```javascript
// this fait référence au contexte d'exécution

// Dans un objet
const person = {
  name: 'Alice',
  age: 25,
  
  greet() {
    console.log(`Hello, I'm ${this.name}`);  // this = person
  },
  
  // Arrow function n'a pas son propre this
  greetArrow: () => {
    console.log(`Hello, I'm ${this.name}`);  // this = window/global
  },
  
  delayedGreet() {
    // Problème classique avec callback
    setTimeout(function() {
      console.log(this.name);  // undefined - this = window
    }, 1000);
  },
  
  delayedGreetFixed() {
    // Solution 1 : arrow function
    setTimeout(() => {
      console.log(this.name);  // 'Alice' - this hérité
    }, 1000);
  },
  
  delayedGreetFixed2() {
    // Solution 2 : bind
    setTimeout(function() {
      console.log(this.name);  // 'Alice'
    }.bind(this), 1000);
  },
  
  delayedGreetFixed3() {
    // Solution 3 : variable self
    const self = this;
    setTimeout(function() {
      console.log(self.name);  // 'Alice'
    }, 1000);
  }
};

person.greet();  // "Hello, I'm Alice"
person.greetArrow();  // "Hello, I'm undefined"

// this peut changer selon comment la fonction est appelée
const greet = person.greet;
greet();  // "Hello, I'm undefined" - this = window

// Pour fixer this, utiliser bind
const boundGreet = person.greet.bind(person);
boundGreet();  // "Hello, I'm Alice"

// Dans une classe ES6
class User {
  constructor(name) {
    this.name = name;
  }
  
  greet() {
    console.log(`Hello, ${this.name}`);
  }
}

const user = new User('Bob');
user.greet();  // "Hello, Bob"

// Cas pratique : event handlers (important pour React)
const button = {
  text: 'Click me',
  
  // ❌ Problème
  handleClick: function() {
    console.log(this.text);  // undefined si appelé par addEventListener
  },
  
  // ✅ Solution avec arrow function
  handleClickFixed: () => {
    // Mais attention : this n'est pas 'button' ici !
  },
  
  // ✅ Meilleure solution : bind dans le constructor
  init() {
    this.handleClick = this.handleClick.bind(this);
  }
};
```

---

## 6.2 Manipulation d'Objets

### `Object.keys()`, `Object.values()`, `Object.entries()`

```javascript
const user = {
  name: 'Alice',
  age: 25,
  email: 'alice@email.com',
  active: true
};

// Object.keys() - retourne un array des clés
const keys = Object.keys(user);
console.log(keys);  // ['name', 'age', 'email', 'active']

// Object.values() - retourne un array des valeurs
const values = Object.values(user);
console.log(values);  // ['Alice', 25, 'alice@email.com', true]

// Object.entries() - retourne un array de [clé, valeur]
const entries = Object.entries(user);
console.log(entries);
// [
//   ['name', 'Alice'],
//   ['age', 25],
//   ['email', 'alice@email.com'],
//   ['active', true]
// ]

// Itérer sur un objet avec for...of
for (const key of Object.keys(user)) {
  console.log(`${key}: ${user[key]}`);
}

// Itérer avec entries
for (const [key, value] of Object.entries(user)) {
  console.log(`${key}: ${value}`);
}

// Cas pratique : transformer un objet en array
const products = {
  laptop: 1200,
  phone: 800,
  tablet: 500
};

const productArray = Object.entries(products).map(([name, price]) => ({
  name,
  price,
  tax: price * 0.2
}));

console.log(productArray);
// [
//   { name: 'laptop', price: 1200, tax: 240 },
//   { name: 'phone', price: 800, tax: 160 },
//   { name: 'tablet', price: 500, tax: 100 }
// ]

// Cas pratique : filtrer les propriétés d'un objet
const filteredEntries = Object.entries(user)
  .filter(([key, value]) => typeof value === 'string');

const filteredUser = Object.fromEntries(filteredEntries);
console.log(filteredUser);
// { name: 'Alice', email: 'alice@email.com' }

// Compter les propriétés
const count = Object.keys(user).length;
console.log(count);  // 4

// Vérifier si un objet est vide
const isEmpty = obj => Object.keys(obj).length === 0;
console.log(isEmpty({}));    // true
console.log(isEmpty(user));  // false
```

### `Object.assign()` - Copier et fusionner des objets

```javascript
// Object.assign(target, ...sources)

// Copier un objet (shallow copy)
const original = { a: 1, b: 2 };
const copy = Object.assign({}, original);
console.log(copy);  // { a: 1, b: 2 }

// Fusionner plusieurs objets
const obj1 = { a: 1, b: 2 };
const obj2 = { c: 3, d: 4 };
const obj3 = { e: 5 };

const merged = Object.assign({}, obj1, obj2, obj3);
console.log(merged);  // { a: 1, b: 2, c: 3, d: 4, e: 5 }

// Les propriétés sont écrasées par ordre
const person1 = { name: 'Alice', age: 25 };
const person2 = { name: 'Bob', city: 'Paris' };

const result = Object.assign({}, person1, person2);
console.log(result);  // { name: 'Bob', age: 25, city: 'Paris' }
// 'name' de person2 écrase 'name' de person1

// Ajouter des propriétés à un objet existant
const user = { name: 'Alice' };
Object.assign(user, { age: 25, email: 'alice@email.com' });
console.log(user);  // { name: 'Alice', age: 25, email: 'alice@email.com' }

// ⚠️ Shallow copy : les objets imbriqués sont copiés par référence
const original2 = {
  name: 'Alice',
  address: {
    city: 'Paris'
  }
};

const copy2 = Object.assign({}, original2);
copy2.address.city = 'Lyon';  // Modifie aussi l'original !
console.log(original2.address.city);  // 'Lyon' ⚠️

// Pour une deep copy, utiliser d'autres méthodes
const deepCopy = JSON.parse(JSON.stringify(original2));
// Ou structuredClone (nouveau)
const deepCopy2 = structuredClone(original2);

// Cas pratique : options par défaut
function createUser(options) {
  const defaults = {
    role: 'user',
    active: true,
    notifications: true
  };
  
  return Object.assign({}, defaults, options);
}

const user1 = createUser({ name: 'Alice', role: 'admin' });
console.log(user1);
// { role: 'admin', active: true, notifications: true, name: 'Alice' }

const user2 = createUser({ name: 'Bob' });
console.log(user2);
// { role: 'user', active: true, notifications: true, name: 'Bob' }
```

### Spread operator (`...`) avec objets

```javascript
// Alternative moderne à Object.assign()

const obj1 = { a: 1, b: 2 };
const obj2 = { c: 3, d: 4 };

// Fusionner des objets
const merged = { ...obj1, ...obj2 };
console.log(merged);  // { a: 1, b: 2, c: 3, d: 4 }

// Copier un objet
const original = { name: 'Alice', age: 25 };
const copy = { ...original };
console.log(copy);  // { name: 'Alice', age: 25 }

// Ajouter/Modifier des propriétés
const user = { name: 'Alice', age: 25 };
const updated = { ...user, age: 26, city: 'Paris' };
console.log(updated);  // { name: 'Alice', age: 26, city: 'Paris' }

// Ordre important : dernier écrase le premier
const person = { name: 'Alice', age: 25 };
const changes = { age: 30, email: 'alice@email.com' };

const result1 = { ...person, ...changes };
console.log(result1);  // { name: 'Alice', age: 30, email: 'alice@email.com' }

const result2 = { ...changes, ...person };
console.log(result2);  // { age: 25, email: 'alice@email.com', name: 'Alice' }

// Supprimer une propriété (en créant un nouvel objet)
const { age, ...withoutAge } = user;
console.log(withoutAge);  // { name: 'Alice' }

// ⚠️ Shallow copy seulement
const nested = {
  name: 'Alice',
  address: { city: 'Paris' }
};

const copy2 = { ...nested };
copy2.address.city = 'Lyon';
console.log(nested.address.city);  // 'Lyon' - modifié !

// Pour nested objects, spread à chaque niveau
const deepCopy = {
  ...nested,
  address: { ...nested.address }
};

deepCopy.address.city = 'Marseille';
console.log(nested.address.city);  // 'Lyon' - inchangé ✅

// Cas pratique React : mise à jour immutable du state
const state = {
  user: {
    name: 'Alice',
    age: 25,
    settings: {
      theme: 'dark',
      notifications: true
    }
  }
};

// Mettre à jour settings.theme de manière immutable
const newState = {
  ...state,
  user: {
    ...state.user,
    settings: {
      ...state.user.settings,
      theme: 'light'
    }
  }
};

console.log(state.user.settings.theme);     // 'dark' - inchangé
console.log(newState.user.settings.theme);  // 'light'

// Fusionner des arrays dans un objet
const arrays = {
  numbers: [1, 2, 3],
  letters: ['a', 'b']
};

const moreArrays = {
  numbers: [4, 5],
  symbols: ['@', '#']
};

const combined = {
  ...arrays,
  ...moreArrays,
  numbers: [...arrays.numbers, ...moreArrays.numbers]  // fusionner les arrays
};

console.log(combined);
// {
//   numbers: [1, 2, 3, 4, 5],
//   letters: ['a', 'b'],
//   symbols: ['@', '#']
// }
```

### `Object.freeze()` et `Object.seal()`

```javascript
// Object.freeze() - rend l'objet complètement immutable

const user = {
  name: 'Alice',
  age: 25
};

Object.freeze(user);

// On ne peut plus modifier
user.age = 30;  // Ignoré (ou Error en strict mode)
console.log(user.age);  // 25

// On ne peut plus ajouter
user.email = 'alice@email.com';  // Ignoré
console.log(user.email);  // undefined

// On ne peut plus supprimer
delete user.name;  // Ignoré
console.log(user.name);  // 'Alice'

// Vérifier si un objet est frozen
console.log(Object.isFrozen(user));  // true

// ⚠️ Freeze shallow : objets imbriqués ne sont pas frozen
const company = {
  name: 'Tech Corp',
  address: {
    city: 'Paris'
  }
};

Object.freeze(company);
company.name = 'New Name';  // Ignoré
company.address.city = 'Lyon';  // ✅ Fonctionne ! (nested object pas frozen)
console.log(company.address.city);  // 'Lyon'

// Deep freeze (fonction personnalisée)
function deepFreeze(obj) {
  Object.freeze(obj);
  
  Object.values(obj).forEach(value => {
    if (typeof value === 'object' && value !== null) {
      deepFreeze(value);
    }
  });
  
  return obj;
}

const data = {
  user: {
    name: 'Alice',
    settings: {
      theme: 'dark'
    }
  }
};

deepFreeze(data);
data.user.settings.theme = 'light';  // Ignoré
console.log(data.user.settings.theme);  // 'dark'

// Object.seal() - empêche ajout/suppression mais permet modification

const product = {
  name: 'Laptop',
  price: 1000
};

Object.seal(product);

// ✅ Modification possible
product.price = 1200;
console.log(product.price);  // 1200

// ❌ Ajout impossible
product.brand = 'Dell';  // Ignoré
console.log(product.brand);  // undefined

// ❌ Suppression impossible
delete product.name;  // Ignoré
console.log(product.name);  // 'Laptop'

// Vérifier si sealed
console.log(Object.isSealed(product));  // true

// Comparaison freeze vs seal
const obj1 = { a: 1 };
Object.freeze(obj1);
// - Pas de modification
// - Pas d'ajout
// - Pas de suppression

const obj2 = { b: 2 };
Object.seal(obj2);
// - ✅ Modification possible
// - Pas d'ajout
// - Pas de suppression

// Cas pratique : constantes configurables
const CONFIG = Object.freeze({
  API_URL: 'https://api.example.com',
  TIMEOUT: 5000,
  MAX_RETRIES: 3
});

// CONFIG.API_URL = 'autre-url';  // Ignoré - protection
```

### `hasOwnProperty()` et `in` operator

```javascript
const user = {
  name: 'Alice',
  age: 25
};

// hasOwnProperty() - vérifie les propriétés propres (non héritées)
console.log(user.hasOwnProperty('name'));      // true
console.log(user.hasOwnProperty('age'));       // true
console.log(user.hasOwnProperty('toString'));  // false (hérité)
console.log(user.hasOwnProperty('email'));     // false (n'existe pas)

// in operator - vérifie les propriétés propres ET héritées
console.log('name' in user);      // true
console.log('toString' in user);  // true (hérité de Object.prototype)
console.log('email' in user);     // false

// Différence importante
console.log('toString' in user);              // true
console.log(user.hasOwnProperty('toString')); // false

// Cas avec prototype
const animal = {
  type: 'animal',
  breathe() {
    return 'breathing';
  }
};

const dog = Object.create(animal);
dog.name = 'Rex';
dog.breed = 'Labrador';

console.log(dog.hasOwnProperty('name'));    // true (propre)
console.log(dog.hasOwnProperty('type'));    // false (hérité)
console.log('type' in dog);                 // true (hérité mais accessible)

// Itérer uniquement sur les propriétés propres
for (const key in dog) {
  if (dog.hasOwnProperty(key)) {
    console.log(`${key}: ${dog[key]}`);
  }
}
// Affiche: name: Rex, breed: Labrador (pas 'type' ni 'breathe')

// Alternative moderne : Object.keys() (retourne seulement les propriétés propres)
Object.keys(dog).forEach(key => {
  console.log(`${key}: ${dog[key]}`);
});

// Cas pratique : vérification sécurisée de propriété
function hasProperty(obj, prop) {
  return Object.prototype.hasOwnProperty.call(obj, prop);
}

// Plus sûr que obj.hasOwnProperty() car fonctionne même si
// l'objet a une propriété 'hasOwnProperty' personnalisée
const strange = {
  name: 'test',
  hasOwnProperty: 'not a function'
};

// strange.hasOwnProperty('name');  // ❌ Error
console.log(hasProperty(strange, 'name'));  // ✅ true

// Cas pratique : copier uniquement les propriétés propres
function copyOwnProperties(source) {
  const target = {};
  
  for (const key in source) {
    if (source.hasOwnProperty(key)) {
      target[key] = source[key];
    }
  }
  
  return target;
}

const dogCopy = copyOwnProperties(dog);
console.log(dogCopy);  // { name: 'Rex', breed: 'Labrador' }
```

### Computed property names

```javascript
// Créer des propriétés avec des noms dynamiques

const fieldName = 'email';
const fieldValue = 'alice@email.com';

// Ancienne méthode
const user1 = {};
user1[fieldName] = fieldValue;
console.log(user1);  // { email: 'alice@email.com' }

// Nouvelle syntaxe ES6 (computed property names)
const user2 = {
  name: 'Alice',
  [fieldName]: fieldValue
};
console.log(user2);  // { name: 'Alice', email: 'alice@email.com' }

// Avec expressions
const prefix = 'user_';
const user3 = {
  [prefix + 'name']: 'Bob',
  [prefix + 'age']: 30,
  [prefix + 'id']: 123
};
console.log(user3);
// { user_name: 'Bob', user_age: 30, user_id: 123 }

// Avec des fonctions
const key = () => 'dynamicKey';
const obj = {
  [key()]: 'value'
};
console.log(obj);  // { dynamicKey: 'value' }

// Cas pratique : créer un objet à partir d'un array de paires
const pairs = [
  ['name', 'Alice'],
  ['age', 25],
  ['city', 'Paris']
];

const result = pairs.reduce((acc, [key, value]) => {
  return {
    ...acc,
    [key]: value
  };
}, {});

console.log(result);  // { name: 'Alice', age: 25, city: 'Paris' }

// Alternative avec Object.fromEntries (plus simple)
const result2 = Object.fromEntries(pairs);
console.log(result2);  // { name: 'Alice', age: 25, city: 'Paris' }

// Cas pratique : créer un enum-like object
const createEnum = (...keys) => {
  return keys.reduce((acc, key) => {
    return {
      ...acc,
      [key]: key
    };
  }, {});
};

const STATUS = createEnum('PENDING', 'APPROVED', 'REJECTED');
console.log(STATUS);
// { PENDING: 'PENDING', APPROVED: 'APPROVED', REJECTED: 'REJECTED' }

// Cas pratique : grouper par propriété dynamique
const products = [
  { name: 'Product 1', category: 'electronics' },
  { name: 'Product 2', category: 'books' },
  { name: 'Product 3', category: 'electronics' }
];

function groupBy(array, key) {
  return array.reduce((acc, item) => {
    const groupKey = item[key];
    return {
      ...acc,
      [groupKey]: [...(acc[groupKey] || []), item]
    };
  }, {});
}

const grouped = groupBy(products, 'category');
console.log(grouped);
// {
//   electronics: [{ name: 'Product 1', ... }, { name: 'Product 3', ... }],
//   books: [{ name: 'Product 2', ... }]
// }

// Avec méthodes
const methodName = 'greet';
const person = {
  name: 'Alice',
  [methodName]() {
    return `Hello, I'm ${this.name}`;
  },
  [`${methodName}Formal`]() {
    return `Good day, my name is ${this.name}`;
  }
};

console.log(person.greet());        // "Hello, I'm Alice"
console.log(person.greetFormal());  // "Good day, my name is Alice"

// Avec Symbol (clés uniques)
const uniqueKey = Symbol('id');
const obj2 = {
  [uniqueKey]: 123,
  name: 'Test'
};

console.log(obj2[uniqueKey]);  // 123
console.log(Object.keys(obj2));  // ['name'] - Symbol n'apparaît pas
```

---

## Cas Pratiques Avancés

### Cloner des objets (shallow vs deep)

```javascript
// SHALLOW COPY - copie superficielle

const original = {
  name: 'Alice',
  age: 25,
  address: {
    city: 'Paris',
    country: 'France'
  },
  hobbies: ['reading', 'coding']
};

// Méthode 1 : Spread operator
const copy1 = { ...original };

// Méthode 2 : Object.assign()
const copy2 = Object.assign({}, original);

// Méthode 3 : Object.create() avec Object.getPrototypeOf()
const copy3 = Object.create(
  Object.getPrototypeOf(original),
  Object.getOwnPropertyDescriptors(original)
);

// ⚠️ Problème avec shallow copy
copy1.name = 'Bob';  // OK - primitive
copy1.address.city = 'Lyon';  // ❌ Modifie aussi l'original !
copy1.hobbies.push('gaming');  // ❌ Modifie aussi l'original !

console.log(original.name);          // 'Alice' - inchangé ✅
console.log(original.address.city);  // 'Lyon' - modifié ❌
console.log(original.hobbies);       // ['reading', 'coding', 'gaming']


// DEEP COPY - copie profonde

// Méthode 1 : JSON.parse/stringify (simple mais limitations)
const deepCopy1 = JSON.parse(JSON.stringify(original));

deepCopy1.address.city = 'Marseille';
console.log(original.address.city);  // 'Lyon' - inchangé ✅

// ⚠️ Limitations de JSON.parse/stringify :
const problematic = {
  date: new Date(),
  regex: /test/g,
  func: () => 'hello',
  undefined: undefined,
  symbol: Symbol('id'),
  circular: null
};
problematic.circular = problematic;  // référence circulaire

const jsonCopy = JSON.parse(JSON.stringify(problematic));
console.log(jsonCopy);
// {
//   date: "2024-11-06T..." (string, pas Date),
//   regex: {} (objet vide),
//   // func, undefined, symbol disparaissent !
//   // circular causerait une erreur
// }

// Méthode 2 : structuredClone() - moderne, plus fiable (ES2021)
const deepCopy2 = structuredClone(original);

deepCopy2.address.city = 'Nice';
deepCopy2.hobbies.push('swimming');
console.log(original.address.city);  // 'Lyon' - inchangé ✅
console.log(original.hobbies);       // ['reading', 'coding', 'gaming'] - inchangé ✅

// structuredClone gère mieux les types complexes
const complex = {
  date: new Date(),
  map: new Map([['key', 'value']]),
  set: new Set([1, 2, 3]),
  arrayBuffer: new ArrayBuffer(8)
};

const clonedComplex = structuredClone(complex);
console.log(clonedComplex.date instanceof Date);  // true ✅
console.log(clonedComplex.map instanceof Map);    // true ✅

// ⚠️ structuredClone ne copie pas les fonctions ni les symboles
const withFunc = {
  name: 'Test',
  greet: () => 'hello'
};

const clonedFunc = structuredClone(withFunc);
console.log(clonedFunc.greet);  // undefined - fonction perdue

// Méthode 3 : Deep clone récursif (fonction personnalisée)
function deepClone(obj, hash = new WeakMap()) {
  // Gérer les primitives et null
  if (obj === null || typeof obj !== 'object') {
    return obj;
  }
  
  // Gérer les références circulaires
  if (hash.has(obj)) {
    return hash.get(obj);
  }
  
  // Gérer les types spéciaux
  if (obj instanceof Date) {
    return new Date(obj);
  }
  
  if (obj instanceof RegExp) {
    return new RegExp(obj);
  }
  
  if (obj instanceof Map) {
    const mapCopy = new Map();
    hash.set(obj, mapCopy);
    obj.forEach((value, key) => {
      mapCopy.set(key, deepClone(value, hash));
    });
    return mapCopy;
  }
  
  if (obj instanceof Set) {
    const setCopy = new Set();
    hash.set(obj, setCopy);
    obj.forEach(value => {
      setCopy.add(deepClone(value, hash));
    });
    return setCopy;
  }
  
  // Gérer les arrays
  if (Array.isArray(obj)) {
    const arrCopy = [];
    hash.set(obj, arrCopy);
    obj.forEach((item, index) => {
      arrCopy[index] = deepClone(item, hash);
    });
    return arrCopy;
  }
  
  // Gérer les objets
  const objCopy = {};
  hash.set(obj, objCopy);
  Object.keys(obj).forEach(key => {
    objCopy[key] = deepClone(obj[key], hash);
  });
  
  return objCopy;
}

// Test avec référence circulaire
const circular = { name: 'Test' };
circular.self = circular;

const clonedCircular = deepClone(circular);
console.log(clonedCircular.self === clonedCircular);  // true
console.log(clonedCircular !== circular);  // true - copie distincte

// Comparaison des méthodes
const testObj = {
  name: 'Alice',
  nested: {
    value: 42
  }
};

// Performance (du plus rapide au plus lent, généralement)
console.time('spread');
const s = { ...testObj };
console.timeEnd('spread');  // ~0.01ms

console.time('Object.assign');
const a = Object.assign({}, testObj);
console.timeEnd('Object.assign');  // ~0.01ms

console.time('JSON');
const j = JSON.parse(JSON.stringify(testObj));
console.timeEnd('JSON');  // ~0.1ms

console.time('structuredClone');
const sc = structuredClone(testObj);
console.timeEnd('structuredClone');  // ~0.05ms

console.time('custom deepClone');
const dc = deepClone(testObj);
console.timeEnd('custom deepClone');  // ~0.2ms
```

### Fusionner des objets profondément (deep merge)

```javascript
// Fusionner des objets imbriqués de manière récursive

function deepMerge(target, ...sources) {
  if (!sources.length) return target;
  
  const source = sources.shift();
  
  if (isObject(target) && isObject(source)) {
    for (const key in source) {
      if (isObject(source[key])) {
        // Si la clé n'existe pas dans target, la créer
        if (!target[key]) {
          Object.assign(target, { [key]: {} });
        }
        // Fusionner récursivement
        deepMerge(target[key], source[key]);
      } else {
        // Copier la valeur
        Object.assign(target, { [key]: source[key] });
      }
    }
  }
  
  // Continuer avec les sources restantes
  return deepMerge(target, ...sources);
}

function isObject(item) {
  return item && typeof item === 'object' && !Array.isArray(item);
}

// Test
const config1 = {
  api: {
    url: 'https://api.example.com',
    timeout: 5000
  },
  features: {
    darkMode: true
  }
};

const config2 = {
  api: {
    timeout: 3000,
    retry: 3
  },
  features: {
    notifications: true
  },
  user: {
    name: 'Alice'
  }
};

const merged = deepMerge({}, config1, config2);
console.log(merged);
// {
//   api: {
//     url: 'https://api.example.com',
//     timeout: 3000,
//     retry: 3
//   },
//   features: {
//     darkMode: true,
//     notifications: true
//   },
//   user: {
//     name: 'Alice'
//   }
// }

// Cas pratique : configuration avec valeurs par défaut
const defaultConfig = {
  theme: {
    colors: {
      primary: '#007bff',
      secondary: '#6c757d'
    },
    fontSize: 16
  },
  features: {
    search: true,
    export: false
  }
};

const userConfig = {
  theme: {
    colors: {
      primary: '#ff0000'  // Override seulement primary
    }
  },
  features: {
    export: true  // Override export
  }
};

const finalConfig = deepMerge({}, defaultConfig, userConfig);
console.log(finalConfig);
// {
//   theme: {
//     colors: {
//       primary: '#ff0000',     // overridden
//       secondary: '#6c757d'    // conservé
//     },
//     fontSize: 16              // conservé
//   },
//   features: {
//     search: true,             // conservé
//     export: true              // overridden
//   }
// }
```

### Comparer des objets

```javascript
// Comparaison shallow (superficielle)

function shallowEqual(obj1, obj2) {
  // Vérifier si c'est la même référence
  if (obj1 === obj2) return true;
  
  // Vérifier si les deux sont des objets
  if (typeof obj1 !== 'object' || typeof obj2 !== 'object' ||
      obj1 === null || obj2 === null) {
    return false;
  }
  
  // Vérifier le nombre de clés
  const keys1 = Object.keys(obj1);
  const keys2 = Object.keys(obj2);
  
  if (keys1.length !== keys2.length) return false;
  
  // Comparer chaque valeur (shallow)
  for (const key of keys1) {
    if (obj1[key] !== obj2[key]) {
      return false;
    }
  }
  
  return true;
}

// Tests
const a = { x: 1, y: 2 };
const b = { x: 1, y: 2 };
const c = { x: 1, y: 3 };

console.log(shallowEqual(a, b));  // true
console.log(shallowEqual(a, c));  // false

// ⚠️ Shallow : ne compare pas les objets imbriqués
const d = { x: 1, nested: { z: 3 } };
const e = { x: 1, nested: { z: 3 } };
console.log(shallowEqual(d, e));  // false (nested !== nested)

// Comparaison deep (profonde)

function deepEqual(obj1, obj2) {
  // Vérifier si c'est la même référence
  if (obj1 === obj2) return true;
  
  // Vérifier null et undefined
  if (obj1 == null || obj2 == null) return false;
  
  // Vérifier les types
  if (typeof obj1 !== typeof obj2) return false;
  
  // Si ce n'est pas un objet, comparer directement
  if (typeof obj1 !== 'object') return obj1 === obj2;
  
  // Gérer les arrays
  if (Array.isArray(obj1) && Array.isArray(obj2)) {
    if (obj1.length !== obj2.length) return false;
    
    for (let i = 0; i < obj1.length; i++) {
      if (!deepEqual(obj1[i], obj2[i])) return false;
    }
    
    return true;
  }
  
  // Gérer les objets
  const keys1 = Object.keys(obj1);
  const keys2 = Object.keys(obj2);
  
  if (keys1.length !== keys2.length) return false;
  
  for (const key of keys1) {
    if (!keys2.includes(key)) return false;
    if (!deepEqual(obj1[key], obj2[key])) return false;
  }
  
  return true;
}

// Tests
const obj1 = { x: 1, nested: { y: 2, z: [1, 2, 3] } };
const obj2 = { x: 1, nested: { y: 2, z: [1, 2, 3] } };
const obj3 = { x: 1, nested: { y: 2, z: [1, 2, 4] } };

console.log(deepEqual(obj1, obj2));  // true
console.log(deepEqual(obj1, obj3));  // false

// Alternative simple avec JSON (limitations)
function jsonEqual(obj1, obj2) {
  return JSON.stringify(obj1) === JSON.stringify(obj2);
}

// ⚠️ Problème : l'ordre des clés compte
const f = { a: 1, b: 2 };
const g = { b: 2, a: 1 };
console.log(jsonEqual(f, g));  // false (mais logiquement égaux)
console.log(deepEqual(f, g));  // true ✅
```

### Transformer des objets

```javascript
// Mapper les valeurs d'un objet

function mapObject(obj, fn) {
  return Object.fromEntries(
    Object.entries(obj).map(([key, value]) => [key, fn(value, key)])
  );
}

const prices = {
  laptop: 1000,
  phone: 500,
  tablet: 300
};

// Ajouter la TVA (20%)
const pricesWithTax = mapObject(prices, price => price * 1.2);
console.log(pricesWithTax);
// { laptop: 1200, phone: 600, tablet: 360 }

// Formatter les valeurs
const formatted = mapObject(prices, price => `${price}€`);
console.log(formatted);
// { laptop: '1000€', phone: '500€', tablet: '300€' }

// Filtrer les propriétés d'un objet

function filterObject(obj, predicate) {
  return Object.fromEntries(
    Object.entries(obj).filter(([key, value]) => predicate(value, key))
  );
}

const products = {
  laptop: { price: 1000, inStock: true },
  phone: { price: 500, inStock: false },
  tablet: { price: 300, inStock: true }
};

// Garder seulement les produits en stock
const available = filterObject(products, product => product.inStock);
console.log(available);
// { laptop: { price: 1000, inStock: true }, tablet: { price: 300, inStock: true } }

// Garder les produits < 600€
const affordable = filterObject(products, product => product.price < 600);
console.log(affordable);
// { phone: { price: 500, inStock: false }, tablet: { price: 300, inStock: true } }

// Omettre des propriétés (pick/omit pattern)

function omit(obj, ...keysToOmit) {
  return Object.fromEntries(
    Object.entries(obj).filter(([key]) => !keysToOmit.includes(key))
  );
}

function pick(obj, ...keysToPick) {
  return Object.fromEntries(
    Object.entries(obj).filter(([key]) => keysToPick.includes(key))
  );
}

const user = {
  id: 1,
  name: 'Alice',
  email: 'alice@email.com',
  password: 'secret123',
  role: 'admin'
};

// Supprimer les champs sensibles
const safeUser = omit(user, 'password');
console.log(safeUser);
// { id: 1, name: 'Alice', email: 'alice@email.com', role: 'admin' }

// Garder seulement certains champs
const publicUser = pick(user, 'id', 'name');
console.log(publicUser);
// { id: 1, name: 'Alice' }

// Renommer des clés

function renameKeys(obj, keyMap) {
  return Object.fromEntries(
    Object.entries(obj).map(([key, value]) => [
      keyMap[key] || key,  // Utiliser le nouveau nom ou garder l'ancien
      value
    ])
  );
}

const apiResponse = {
  user_name: 'Alice',
  user_email: 'alice@email.com',
  user_age: 25
};

const renamedUser = renameKeys(apiResponse, {
  user_name: 'name',
  user_email: 'email',
  user_age: 'age'
});

console.log(renamedUser);
// { name: 'Alice', email: 'alice@email.com', age: 25 }

// Inverser clés et valeurs

function invertObject(obj) {
  return Object.fromEntries(
    Object.entries(obj).map(([key, value]) => [value, key])
  );
}

const errorCodes = {
  NOT_FOUND: 404,
  UNAUTHORIZED: 401,
  FORBIDDEN: 403
};

const codeToError = invertObject(errorCodes);
console.log(codeToError);
// { 404: 'NOT_FOUND', 401: 'UNAUTHORIZED', 403: 'FORBIDDEN' }

// Aplatir un objet imbriqué (flatten)

function flattenObject(obj, prefix = '') {
  return Object.keys(obj).reduce((acc, key) => {
    const prefixedKey = prefix ? `${prefix}.${key}` : key;
    
    if (typeof obj[key] === 'object' && obj[key] !== null && !Array.isArray(obj[key])) {
      // Récursif pour les objets imbriqués
      Object.assign(acc, flattenObject(obj[key], prefixedKey));
    } else {
      // Ajouter la valeur
      acc[prefixedKey] = obj[key];
    }
    
    return acc;
  }, {});
}

const nested = {
  user: {
    name: 'Alice',
    address: {
      city: 'Paris',
      country: 'France'
    }
  },
  settings: {
    theme: 'dark'
  }
};

const flattened = flattenObject(nested);
console.log(flattened);
// {
//   'user.name': 'Alice',
//   'user.address.city': 'Paris',
//   'user.address.country': 'France',
//   'settings.theme': 'dark'
// }

// Déaplatir (unflatten) - inverse de flatten

function unflattenObject(obj) {
  const result = {};
  
  for (const [path, value] of Object.entries(obj)) {
    const keys = path.split('.');
    let current = result;
    
    for (let i = 0; i < keys.length - 1; i++) {
      const key = keys[i];
      if (!current[key]) {
        current[key] = {};
      }
      current = current[key];
    }
    
    current[keys[keys.length - 1]] = value;
  }
  
  return result;
}

const unflattened = unflattenObject(flattened);
console.log(unflattened);
// {
//   user: {
//     name: 'Alice',
//     address: {
//       city: 'Paris',
//       country: 'France'
//     }
//   },
//   settings: {
//     theme: 'dark'
//   }
// }
```

### Validation et nettoyage d'objets

```javascript
// Supprimer les propriétés undefined/null

function removeEmpty(obj) {
  return Object.fromEntries(
    Object.entries(obj)
      .filter(([_, value]) => value != null)  // != null catch undefined et null
  );
}

const data = {
  name: 'Alice',
  age: 25,
  email: null,
  phone: undefined,
  city: 'Paris'
};

const cleaned = removeEmpty(data);
console.log(cleaned);
// { name: 'Alice', age: 25, city: 'Paris' }

// Supprimer les propriétés falsy

function removeFalsy(obj) {
  return Object.fromEntries(
    Object.entries(obj).filter(([_, value]) => Boolean(value))
  );
}

const mixedData = {
  name: 'Alice',
  age: 0,          // falsy mais valide
  email: '',       // falsy
  active: false,   // falsy mais valide
  city: 'Paris'
};

const cleanedFalsy = removeFalsy(mixedData);
console.log(cleanedFalsy);
// { name: 'Alice', city: 'Paris' }
// ⚠️ age, active supprimés car falsy

// Mieux : supprimer seulement null/undefined/''

function removeNullish(obj) {
  return Object.fromEntries(
    Object.entries(obj).filter(([_, value]) => 
      value !== null && value !== undefined && value !== ''
    )
  );
}

const cleanedNullish = removeNullish(mixedData);
console.log(cleanedNullish);
// { name: 'Alice', age: 0, active: false, city: 'Paris' } ✅

// Deep clean (récursif)

function deepClean(obj) {
  if (Array.isArray(obj)) {
    return obj
      .map(item => deepClean(item))
      .filter(item => item != null);
  }
  
  if (typeof obj === 'object' && obj !== null) {
    return Object.fromEntries(
      Object.entries(obj)
        .map(([key, value]) => [key, deepClean(value)])
        .filter(([_, value]) => value != null)
    );
  }
  
  return obj;
}

const deepNested = {
  user: {
    name: 'Alice',
    email: null,
    settings: {
      theme: 'dark',
      notifications: undefined,
      privacy: {
        public: true,
        details: null
      }
    }
  },
  items: [1, null, 2, undefined, 3]
};

const deepCleaned = deepClean(deepNested);
console.log(deepCleaned);
// {
//   user: {
//     name: 'Alice',
//     settings: {
//       theme: 'dark',
//       privacy: {
//         public: true
//       }
//     }
//   },
//   items: [1, 2, 3]
// }

// Valider la structure d'un objet

function validateSchema(obj, schema) {
  const errors = [];
  
  for (const [key, rules] of Object.entries(schema)) {
    const value = obj[key];
    
    // Required
    if (rules.required && (value === undefined || value === null)) {
      errors.push(`${key} is required`);
      continue;
    }
    
    // Type check
    if (value !== undefined && rules.type && typeof value !== rules.type) {
      errors.push(`${key} must be of type ${rules.type}`);
    }
    
    // Min/Max for numbers
    if (rules.type === 'number' && value !== undefined) {
      if (rules.min !== undefined && value < rules.min) {
        errors.push(`${key} must be >= ${rules.min}`);
      }
      if (rules.max !== undefined && value > rules.max) {
        errors.push(`${key} must be <= ${rules.max}`);
      }
    }
    
    // MinLength/MaxLength for strings
    if (rules.type === 'string' && value !== undefined) {
      if (rules.minLength && value.length < rules.minLength) {
        errors.push(`${key} must be at least ${rules.minLength} characters`);
      }
      if (rules.maxLength && value.length > rules.maxLength) {
        errors.push(`${key} must be at most ${rules.maxLength} characters`);
      }
    }
    
    // Pattern for strings
    if (rules.pattern && value !== undefined) {
      if (!rules.pattern.test(value)) {
        errors.push(`${key} format is invalid`);
      }
    }
  }
  
  return {
    valid: errors.length === 0,
    errors
  };
}

// Définir un schéma
const userSchema = {
  name: {
    required: true,
    type: 'string',
    minLength: 2,
    maxLength: 50
  },
  email: {
    required: true,
    type: 'string',
    pattern: /^[^\s@]+@[^\s@]+\.[^\s@]+$/
  },
  age: {
    required: false,
    type: 'number',
    min: 0,
    max: 150
  }
};

// Valider un objet
const validUser = {
  name: 'Alice',
  email: 'alice@email.com',
  age: 25
};

const validation1 = validateSchema(validUser, userSchema);
console.log(validation1);
// { valid: true, errors: [] }

const invalidUser = {
  name: 'A',  // trop court
  email: 'invalid-email',  // format invalide
  age: 200  // trop grand
};

const validation2 = validateSchema(invalidUser, userSchema);
console.log(validation2);
// {
//   valid: false,
//   errors: [
//     'name must be at least 2 characters',
//     'email format is invalid',
//     'age must be <= 150'
//   ]
// }
```

### Patterns utiles pour React

```javascript
// Mise à jour immutable d'objet imbriqué

const state = {
  user: {
    profile: {
      name: 'Alice',
      settings: {
        theme: 'dark',
        notifications: true
      }
    },
    stats: {
      posts: 10
    }
  }
};

// ❌ Mutation (mauvais pour React)
state.user.profile.settings.theme = 'light';

// ✅ Immutable update (bon pour React)
const newState = {
  ...state,
  user: {
    ...state.user,
    profile: {
      ...state.user.profile,
      settings: {
        ...state.user.profile.settings,
        theme: 'light'
      }
    }
  }
};

// Helper pour mise à jour immutable profonde
function updateNested(obj, path, value) {
  const keys = path.split('.');
  const lastKey = keys.pop();
  
  let result = { ...obj };
  let current = result;
  
  // Créer les objets imbriqués
  for (const key of keys) {
    current[key] = { ...current[key] };
    current = current[key];
  }
  
  // Mettre à jour la valeur finale
  current[lastKey] = value;
  
  return result;
}

const updated = updateNested(state, 'user.profile.settings.theme', 'light');
console.log(state.user.profile.settings.theme);     // 'dark' - inchangé
console.log(updated.user.profile.settings.theme);   // 'light'

// Helper pour mise à jour avec fonction
function updateNestedWith(obj, path, updater) {
  const keys = path.split('.');
  const lastKey = keys.pop();
  
  let result = { ...obj };
  let current = result;
  let original = obj;
  
  for (const key of keys) {
    current[key] = { ...current[key] };
    current = current[key];
    original = original[key];
  }
  
  // Appliquer la fonction de mise à jour
  current[lastKey] = updater(original[lastKey]);
  
  return result;
}

// Incrémenter un compteur
const incremented = updateNestedWith(state, 'user.stats.posts', count => count + 1);
console.log(state.user.stats.posts);        // 10
console.log(incremented.user.stats.posts);  // 11

// Gérer des listes d'objets (CRUD immutable)

const todos = [
  { id: 1, text: 'Learn JS', done: false },
  { id: 2, text: 'Learn React', done: false },
  { id: 3, text: 'Build app', done: false }
];

// CREATE - Ajouter un todo
const newTodo = { id: 4, text: 'Deploy app', done: false };
const withNewTodo = [...todos, newTodo];

// READ - Trouver un todo
const todo = todos.find(t => t.id === 2);

// UPDATE - Modifier un todo
const updatedTodos = todos.map(todo =>
  todo.id === 2
    ? { ...todo, done: true }
    : todo
);

// DELETE - Supprimer un todo
const filteredTodos = todos.filter(todo => todo.id !== 2);

// UPDATE multiple - Marquer tous comme done
const allDone = todos.map(todo => ({ ...todo, done: true }));

// Cas complexe : mise à jour conditionnelle
const complexUpdate = todos.map(todo => {
  if (todo.id === 2) {
    return { ...todo, done: true, priority: 'high' };
  }
  if (todo.done) {
    return { ...todo, archived: true };
  }
  return todo;
});
```

---

# Chapitre 6 : Objects (Objets) - Questions/Réponses Orales

## Questions sur les Fondamentaux

### Q1: Quelle est la différence entre dot notation et bracket notation pour accéder aux propriétés d'un objet ?

**Réponse :**

**Dot notation** (`.`) :
- Syntaxe simple et lisible
- Ne fonctionne pas avec les noms de propriétés contenant des espaces ou caractères spéciaux
- Ne permet pas l'accès dynamique

**Bracket notation** (`[]`) :
- Permet les noms avec espaces ou caractères spéciaux
- Permet l'accès dynamique avec des variables
- Plus flexible mais moins lisible

```javascript
const user = {
  name: 'Alice',
  'home address': 'Paris'
};

// Dot notation
console.log(user.name);  // 'Alice'

// Bracket notation
console.log(user['name']);          // 'Alice'
console.log(user['home address']);  // 'Paris' - nécessaire ici

// Accès dynamique (seulement avec bracket)
const key = 'name';
console.log(user[key]);  // 'Alice' - fonctionne
// console.log(user.key);  // undefined - cherche la propriété 'key'
```

**Quand utiliser bracket notation :**
- Propriétés avec caractères spéciaux
- Accès dynamique (la clé est dans une variable)
- Propriétés calculées
- Boucles

---

### Q2: Qu'est-ce que le keyword `this` dans un objet ? Comment son comportement change-t-il ?

**Réponse :**

`this` fait référence au **contexte d'exécution** de la fonction. Dans un objet, `this` pointe vers l'objet lui-même, mais son comportement peut changer selon comment la fonction est appelée.

```javascript
const person = {
  name: 'Alice',
  
  // Méthode normale : this = person
  greet() {
    console.log(`Hello, ${this.name}`);
  },
  
  // Arrow function : this = contexte parent (pas person)
  greetArrow: () => {
    console.log(`Hello, ${this.name}`);  // undefined
  }
};

person.greet();       // "Hello, Alice" ✅
person.greetArrow();  // "Hello, undefined" ❌

// this change si on extrait la méthode
const greet = person.greet;
greet();  // "Hello, undefined" - this = window/global

// Solution : bind
const boundGreet = person.greet.bind(person);
boundGreet();  // "Hello, Alice" ✅
```

**Points clés :**
- Méthodes normales : `this` = objet
- Arrow functions : `this` hérité du scope parent
- Fonction extraite : `this` perdu (devient global)
- `bind()`, `call()`, `apply()` permettent de fixer `this`

---

### Q3: Comment créer un objet en JavaScript ? Quelles sont les différentes méthodes ?

**Réponse :**

```javascript
// 1. Object literal (la plus courante)
const obj1 = {
  name: 'Alice',
  age: 25
};

// 2. Constructor Object
const obj2 = new Object();
obj2.name = 'Bob';

// 3. Object.create() (avec prototype)
const proto = { greet() { return 'Hello'; } };
const obj3 = Object.create(proto);
obj3.name = 'Charlie';

// 4. Constructor function (ancienne syntaxe)
function Person(name, age) {
  this.name = name;
  this.age = age;
}
const obj4 = new Person('David', 30);

// 5. Class ES6 (moderne)
class User {
  constructor(name, age) {
    this.name = name;
    this.age = age;
  }
}
const obj5 = new User('Eve', 28);
```

**Recommandation :** Object literal pour les objets simples, Classes ES6 pour les objets complexes avec méthodes.

---

### Q4: Comment vérifier si une propriété existe dans un objet ?

**Réponse :**

Plusieurs méthodes selon le besoin :

```javascript
const user = {
  name: 'Alice',
  age: 25
};

// 1. in operator - vérifie propriétés propres ET héritées
console.log('name' in user);      // true
console.log('toString' in user);  // true (hérité)

// 2. hasOwnProperty() - vérifie seulement propriétés propres
console.log(user.hasOwnProperty('name'));      // true
console.log(user.hasOwnProperty('toString'));  // false

// 3. Vérifier si !== undefined
console.log(user.name !== undefined);  // true
// ⚠️ Problème si la valeur est réellement undefined
const obj = { prop: undefined };
console.log(obj.prop !== undefined);  // false (mais la prop existe)

// 4. Object.keys() ou Object.hasOwn() (ES2022)
console.log(Object.keys(user).includes('name'));  // true
console.log(Object.hasOwn(user, 'name'));         // true
```

**Recommandation :**
- Utilisez `hasOwnProperty()` ou `Object.hasOwn()` pour les propriétés propres
- Utilisez `in` si vous voulez inclure les propriétés héritées

---

### Q5: Quelle est la différence entre `Object.keys()`, `Object.values()` et `Object.entries()` ?

**Réponse :**

Ces trois méthodes permettent d'itérer sur un objet mais retournent des formats différents :

```javascript
const user = {
  name: 'Alice',
  age: 25,
  city: 'Paris'
};

// Object.keys() - retourne un array des CLÉS
const keys = Object.keys(user);
console.log(keys);  // ['name', 'age', 'city']

// Object.values() - retourne un array des VALEURS
const values = Object.values(user);
console.log(values);  // ['Alice', 25, 'Paris']

// Object.entries() - retourne un array de paires [clé, valeur]
const entries = Object.entries(user);
console.log(entries);
// [['name', 'Alice'], ['age', 25], ['city', 'Paris']]

// Utilisation pratique
// Itérer avec keys
Object.keys(user).forEach(key => {
  console.log(`${key}: ${user[key]}`);
});

// Itérer avec entries (meilleur)
Object.entries(user).forEach(([key, value]) => {
  console.log(`${key}: ${value}`);
});
```

**Point important :** Ces méthodes retournent seulement les **propriétés propres** (pas les héritées) et **énumérables**.

---

## Questions sur la Manipulation

### Q6: Qu'est-ce qu'une shallow copy et une deep copy ? Quelle est la différence ?

**Réponse :**

**Shallow copy** : Copie seulement le premier niveau. Les objets/arrays imbriqués sont copiés par **référence**.

**Deep copy** : Copie tous les niveaux. Crée de nouveaux objets pour tous les niveaux imbriqués.

```javascript
const original = {
  name: 'Alice',
  address: {
    city: 'Paris'
  }
};

// SHALLOW COPY
const shallow = { ...original };

shallow.name = 'Bob';  // OK
shallow.address.city = 'Lyon';  // ⚠️ Modifie aussi original !

console.log(original.name);          // 'Alice' - inchangé ✅
console.log(original.address.city);  // 'Lyon' - modifié ❌

// DEEP COPY
const deep = JSON.parse(JSON.stringify(original));
// Ou avec structuredClone (moderne)
const deep2 = structuredClone(original);

deep.address.city = 'Marseille';
console.log(original.address.city);  // 'Lyon' - inchangé ✅
```

**Limitations de JSON.parse/stringify :**
- Perd les fonctions
- Perd undefined
- Perd les Symbols
- Perd les Dates (converties en string)
- Ne gère pas les références circulaires

**Pour React :** Toujours traiter les objets comme immutables = créer de nouvelles copies.

---

### Q7: Comment fusionner plusieurs objets en JavaScript ?

**Réponse :**

Plusieurs méthodes selon le besoin :

```javascript
const obj1 = { a: 1, b: 2 };
const obj2 = { c: 3, d: 4 };
const obj3 = { e: 5 };

// 1. Spread operator (moderne, préféré)
const merged1 = { ...obj1, ...obj2, ...obj3 };
console.log(merged1);  // { a: 1, b: 2, c: 3, d: 4, e: 5 }

// 2. Object.assign()
const merged2 = Object.assign({}, obj1, obj2, obj3);

// Les propriétés sont écrasées de gauche à droite
const person1 = { name: 'Alice', age: 25 };
const person2 = { name: 'Bob', city: 'Paris' };

const result = { ...person1, ...person2 };
console.log(result);
// { name: 'Bob', age: 25, city: 'Paris' }
// 'name' de person2 écrase celui de person1
```

**⚠️ Important :** Ces méthodes font une **shallow merge**. Pour un deep merge (objets imbriqués), il faut une fonction récursive.

```javascript
// Deep merge exemple
const config1 = {
  api: { url: 'https://api.com', timeout: 5000 }
};

const config2 = {
  api: { timeout: 3000, retry: 3 }
};

// Shallow merge - PERD l'URL !
const shallow = { ...config1, ...config2 };
console.log(shallow);
// { api: { timeout: 3000, retry: 3 } } ❌

// Il faut un deep merge pour conserver l'URL
```

---

### Q8: Comment supprimer une propriété d'un objet ?

**Réponse :**

```javascript
const user = {
  name: 'Alice',
  age: 25,
  email: 'alice@email.com'
};

// 1. delete operator (mutation)
delete user.email;
console.log(user);  // { name: 'Alice', age: 25 }

// 2. Destructuring (immutable)
const { email, ...userWithoutEmail } = user;
console.log(userWithoutEmail);  // { name: 'Alice', age: 25 }
console.log(user);  // original inchangé

// Supprimer plusieurs propriétés
const { email: e, age: a, ...rest } = user;
console.log(rest);  // { name: 'Alice' }

// Supprimer dynamiquement
function omit(obj, ...keys) {
  const copy = { ...obj };
  keys.forEach(key => delete copy[key]);
  return copy;
}

const cleaned = omit(user, 'email', 'age');
console.log(cleaned);  // { name: 'Alice' }
```

**Pour React :** Préférez la destructuring (immutable) plutôt que `delete` (mutable).

---

### Q9: Qu'est-ce que `Object.freeze()` ? Quelle est la différence avec `Object.seal()` ?

**Réponse :**

**`Object.freeze()`** : Rend l'objet **complètement immutable**
- ❌ Pas de modification
- ❌ Pas d'ajout
- ❌ Pas de suppression

**`Object.seal()`** : Empêche ajout/suppression mais permet modification
- ✅ Modification possible
- ❌ Pas d'ajout
- ❌ Pas de suppression

```javascript
// FREEZE
const frozen = { name: 'Alice', age: 25 };
Object.freeze(frozen);

frozen.age = 30;           // Ignoré
frozen.email = 'test';     // Ignoré
delete frozen.name;        // Ignoré
console.log(frozen);       // { name: 'Alice', age: 25 } - inchangé

console.log(Object.isFrozen(frozen));  // true

// SEAL
const sealed = { name: 'Bob', age: 30 };
Object.seal(sealed);

sealed.age = 35;           // ✅ Fonctionne
sealed.email = 'test';     // ❌ Ignoré
delete sealed.name;        // ❌ Ignoré
console.log(sealed);       // { name: 'Bob', age: 35 }

console.log(Object.isSealed(sealed));  // true
```

**⚠️ Important :** Ces méthodes sont **shallow** - les objets imbriqués ne sont pas affectés.

```javascript
const obj = {
  name: 'Alice',
  address: { city: 'Paris' }
};

Object.freeze(obj);
obj.address.city = 'Lyon';  // ✅ Fonctionne ! (nested pas frozen)
console.log(obj.address.city);  // 'Lyon'
```

---

### Q10: Comment itérer sur les propriétés d'un objet ?

**Réponse :**

Plusieurs méthodes :

```javascript
const user = {
  name: 'Alice',
  age: 25,
  city: 'Paris'
};

// 1. for...in (itère sur toutes les propriétés énumérables)
for (const key in user) {
  console.log(`${key}: ${user[key]}`);
}

// Attention aux propriétés héritées
for (const key in user) {
  if (user.hasOwnProperty(key)) {
    console.log(`${key}: ${user[key]}`);
  }
}

// 2. Object.keys() + forEach
Object.keys(user).forEach(key => {
  console.log(`${key}: ${user[key]}`);
});

// 3. Object.entries() + for...of (le plus moderne)
for (const [key, value] of Object.entries(user)) {
  console.log(`${key}: ${value}`);
}

// 4. Object.values() (si on n'a besoin que des valeurs)
Object.values(user).forEach(value => {
  console.log(value);
});

// 5. Object.entries() + destructuring + forEach
Object.entries(user).forEach(([key, value]) => {
  console.log(`${key}: ${value}`);
});
```

**Recommandation :** Utilisez `Object.entries()` avec `for...of` ou `forEach` - c'est le plus moderne et lisible.

---

## Questions Avancées

### Q11: Comment créer un objet avec des propriétés calculées (computed property names) ?

**Réponse :**

Les computed property names permettent de créer des propriétés avec des noms **dynamiques** lors de la création de l'objet.

```javascript
// Syntaxe ES6
const key = 'email';
const value = 'alice@email.com';

const user = {
  name: 'Alice',
  [key]: value,  // propriété calculée
  ['age' + 'Value']: 25  // ageValue
};

console.log(user);
// { name: 'Alice', email: 'alice@email.com', ageValue: 25 }

// Avec expressions
const prefix = 'user_';
const data = {
  [prefix + 'name']: 'Bob',
  [prefix + 'id']: 123
};
console.log(data);
// { user_name: 'Bob', user_id: 123 }

// Avec méthodes
const methodName = 'greet';
const person = {
  name: 'Alice',
  [methodName]() {
    return `Hello, ${this.name}`;
  }
};

console.log(person.greet());  // "Hello, Alice"

// Cas pratique : créer un objet à partir d'un array
const pairs = [['name', 'Alice'], ['age', 25]];
const obj = pairs.reduce((acc, [key, value]) => ({
  ...acc,
  [key]: value
}), {});
```

**Utilité :** Très utile pour créer des objets dynamiquement, notamment dans les reducers Redux ou les transformations de données.

---

### Q12: Comment comparer deux objets en JavaScript ?

**Réponse :**

**Problème :** `===` compare les **références**, pas le contenu.

```javascript
const obj1 = { name: 'Alice', age: 25 };
const obj2 = { name: 'Alice', age: 25 };

console.log(obj1 === obj2);  // false (références différentes)
console.log(obj1 === obj1);  // true (même référence)
```

**Solutions :**

**1. Comparaison shallow (premier niveau)**
```javascript
function shallowEqual(obj1, obj2) {
  const keys1 = Object.keys(obj1);
  const keys2 = Object.keys(obj2);
  
  if (keys1.length !== keys2.length) return false;
  
  return keys1.every(key => obj1[key] === obj2[key]);
}

console.log(shallowEqual(obj1, obj2));  // true
```

**2. Comparaison deep (récursive)**
```javascript
function deepEqual(obj1, obj2) {
  if (obj1 === obj2) return true;
  
  if (typeof obj1 !== 'object' || typeof obj2 !== 'object' ||
      obj1 === null || obj2 === null) {
    return false;
  }
  
  const keys1 = Object.keys(obj1);
  const keys2 = Object.keys(obj2);
  
  if (keys1.length !== keys2.length) return false;
  
  return keys1.every(key => deepEqual(obj1[key], obj2[key]));
}

const nested1 = { user: { name: 'Alice' } };
const nested2 = { user: { name: 'Alice' } };
console.log(deepEqual(nested1, nested2));  // true
```

**3. JSON.stringify (simple mais limitations)**
```javascript
console.log(JSON.stringify(obj1) === JSON.stringify(obj2));  // true

// ⚠️ Problème : ordre des clés
const a = { x: 1, y: 2 };
const b = { y: 2, x: 1 };
console.log(JSON.stringify(a) === JSON.stringify(b));  // false
```

**Recommandation :** Utilisez une bibliothèque comme Lodash (`_.isEqual()`) pour des comparaisons robustes en production.

---

### Q13: Comment transformer/mapper les valeurs d'un objet ?

**Réponse :**

JavaScript n'a pas de méthode `map()` native pour les objets, mais on peut la créer :

```javascript
// Helper function pour mapper un objet
function mapObject(obj, fn) {
  return Object.fromEntries(
    Object.entries(obj).map(([key, value]) => [key, fn(value, key)])
  );
}

const prices = {
  laptop: 1000,
  phone: 500,
  tablet: 300
};

// Ajouter 20% de TVA
const withTax = mapObject(prices, price => price * 1.2);
console.log(withTax);
// { laptop: 1200, phone: 600, tablet: 360 }

// Formater en string
const formatted = mapObject(prices, price => `${price}€`);
console.log(formatted);
// { laptop: '1000€', phone: '500€', tablet: '300€' }

// Avec accès à la clé
const labeled = mapObject(prices, (price, key) => ({
  name: key,
  price: price,
  category: 'electronics'
}));
```

**Alternative manuelle :**
```javascript
const doubled = Object.fromEntries(
  Object.entries(prices).map(([key, value]) => [key, value * 2])
);
```

---

### Q14: Comment filtrer les propriétés d'un objet ?

**Réponse :**

```javascript
// Helper function pour filtrer
function filterObject(obj, predicate) {
  return Object.fromEntries(
    Object.entries(obj).filter(([key, value]) => predicate(value, key))
  );
}

const products = {
  laptop: { price: 1000, inStock: true },
  phone: { price: 500, inStock: false },
  tablet: { price: 300, inStock: true }
};

// Garder seulement les produits en stock
const available = filterObject(products, product => product.inStock);
console.log(available);
// { laptop: {...}, tablet: {...} }

// Supprimer les valeurs null/undefined
const data = {
  name: 'Alice',
  age: 25,
  email: null,
  phone: undefined
};

const cleaned = filterObject(data, value => value != null);
console.log(cleaned);
// { name: 'Alice', age: 25 }

// Pattern pick/omit
function pick(obj, ...keys) {
  return Object.fromEntries(
    Object.entries(obj).filter(([key]) => keys.includes(key))
  );
}

function omit(obj, ...keys) {
  return Object.fromEntries(
    Object.entries(obj).filter(([key]) => !keys.includes(key))
  );
}

const user = {
  id: 1,
  name: 'Alice',
  password: 'secret',
  email: 'alice@email.com'
};

const safeUser = omit(user, 'password');
// { id: 1, name: 'Alice', email: 'alice@email.com' }

const publicUser = pick(user, 'id', 'name');
// { id: 1, name: 'Alice' }
```

---

### Q15: Comment gérer des objets imbriqués de manière immutable dans React ?

**Réponse :**

C'est crucial pour React car il utilise la comparaison par référence pour détecter les changements.

```javascript
const state = {
  user: {
    profile: {
      name: 'Alice',
      settings: {
        theme: 'dark',
        notifications: true
      }
    }
  }
};

// ❌ MAUVAIS - Mutation directe
state.user.profile.settings.theme = 'light';  // React ne détectera pas

// ✅ BON - Mise à jour immutable
const newState = {
  ...state,
  user: {
    ...state.user,
    profile: {
      ...state.user.profile,
      settings: {
        ...state.user.profile.settings,
        theme: 'light'
      }
    }
  }
};

// Helper function pour simplifier
function updateNested(obj, path, value) {
  const keys = path.split('.');
  const lastKey = keys.pop();
  
  let result = { ...obj };
  let current = result;
  
  for (const key of keys) {
    current[key] = { ...current[key] };
    current = current[key];
  }
  
  current[lastKey] = value;
  return result;
}

// Usage
const updated = updateNested(
  state,
  'user.profile.settings.theme',
  'light'
);

console.log(state.user.profile.settings.theme);    // 'dark'
console.log(updated.user.profile.settings.theme);  // 'light'

// Avec fonction de mise à jour
function updateNestedWith(obj, path, updater) {
  const keys = path.split('.');
  const lastKey = keys.pop();
  
  let result = { ...obj };
  let current = result;
  let original = obj;
  
  for (const key of keys) {
    current[key] = { ...current[key] };
    current = current[key];
    original = original[key];
  }
  
  current[lastKey] = updater(original[lastKey]);
  return result;
}

// Incrémenter un compteur
const incremented = updateNestedWith(
  state,
  'user.stats.posts',
  count => count + 1
);
```

**Bibliothèques utiles :**
- **Immer** : simplifie les mises à jour immutables
- **Lodash** : `_.set()`, `_.get()`, `_.cloneDeep()`

---

### Q16: Expliquez `Object.assign()` vs spread operator. Quelle est la meilleure pratique ?

**Réponse :**

Les deux font essentiellement la même chose (shallow copy/merge), mais avec des syntaxes différentes.

```javascript
const obj1 = { a: 1, b: 2 };
const obj2 = { c: 3, d: 4 };

// Object.assign()
const merged1 = Object.assign({}, obj1, obj2);

// Spread operator
const merged2 = { ...obj1, ...obj2 };

console.log(merged1);  // { a: 1, b: 2, c: 3, d: 4 }
console.log(merged2);  // { a: 1, b: 2, c: 3, d: 4 }
```

**Différences :**

```javascript
// Object.assign modifie le premier argument
const target = {};
Object.assign(target, obj1, obj2);
console.log(target);  // { a: 1, b: 2, c: 3, d: 4 } - modifié

// Spread crée toujours un nouvel objet
const spread = { ...obj1, ...obj2 };
// obj1 et obj2 inchangés
```

**Meilleure pratique :**
- **Préférez spread operator** : plus moderne, plus lisible, plus court
- **Object.assign** : si vous avez besoin de compatibilité avec vieux navigateurs (avant ES6)

```javascript
// Spread est plus flexible
const merged = {
  ...defaults,
  ...options,
  timestamp: Date.now(),  // ajouter facilement
  ...overrides
};

// Équivalent avec Object.assign (moins lisible)
const merged2 = Object.assign(
  {},
  defaults,
  options,
  { timestamp: Date.now() },
  overrides
);
```

---

### Q17: Comment vérifier si un objet est vide ?

**Réponse :**

Plusieurs méthodes :

```javascript
const empty = {};
const notEmpty = { name: 'Alice' };

// 1. Object.keys() (le plus courant)
function isEmpty1(obj) {
  return Object.keys(obj).length === 0;
}

console.log(isEmpty1(empty));     // true
console.log(isEmpty1(notEmpty));  // false

// 2. for...in (plus rapide, s'arrête dès qu'il trouve une propriété)
function isEmpty2(obj) {
  for (const key in obj) {
    if (obj.hasOwnProperty(key)) {
      return false;
    }
  }
  return true;
}

// 3. Object.entries()
function isEmpty3(obj) {
  return Object.entries(obj).length === 0;
}

// 4. JSON.stringify (simple mais pas optimal)
function isEmpty4(obj) {
  return JSON.stringify(obj) === '{}';
}

// ⚠️ Attention aux cas particuliers
console.log(isEmpty1(null));  // Error: Cannot convert undefined or null
console.log(isEmpty1([]));    // true (array vide)

// Version robuste
function isEmptyObject(obj) {
  return obj && 
         typeof obj === 'object' &&
         !Array.isArray(obj) &&
         Object.keys(obj).length === 0;
}

console.log(isEmptyObject({}));      // true
console.log(isEmptyObject(null));    // false
console.log(isEmptyObject([]));      // false
console.log(isEmptyObject({ a: 1 })); // false
```

**Recommandation :** Utilisez `Object.keys().length === 0` pour la lisibilité, ou `for...in` pour la performance sur de gros objets.

---

### Q18: Qu'est-ce que optional chaining (`?.`) et comment l'utiliser ?

**Réponse :**

Optional chaining (`?.`) permet d'accéder aux propriétés imbriquées **sans erreur** si une valeur intermédiaire est `null` ou `undefined`.

```javascript
const user = {
  name: 'Alice',
  address: {
    city: 'Paris'
  }
};

// ❌ Sans optional chaining
console.log(user.address.city);  // 'Paris' - OK
console.log(user.company.name);  // Error: Cannot read property 'name' of undefined

// ✅ Avec optional chaining
console.log(user.address?.city);   // 'Paris'
console.log(user.company?.name);   // undefined (pas d'erreur)

// Chaîner plusieurs niveaux
const country = user.address?.location?.country;
console.log(country);  // undefined (pas d'erreur)

// Avec méthodes
const upper = user.profile?.getName?.();
// Appelle getName() seulement si profile et getName existent

// Avec arrays
const firstPost = user.posts?.[0];
// Accède au premier élément seulement si posts existe

// Cas pratique : données d'API
function displayUserInfo(userData) {
  const name = userData?.user?.profile?.name ?? 'Anonymous';
  const email = userData?.user?.contact?.email ?? 'No email';
  const city = userData?.user?.address?.city ?? 'Unknown';
  
  console.log(`${name} (${email}) from ${city}`);
}

displayUserInfo({});  // "Anonymous (No email) from Unknown"
displayUserInfo({
  user: {
    profile: { name: 'Alice' },
    address: { city: 'Paris' }
  }
});  // "Alice (No email) from Paris"
```

**Combiné avec nullish coalescing (`??`) :**
```javascript
// Fournir une valeur par défaut
const theme = user.settings?.theme ?? 'light';
const fontSize = user.settings?.fontSize ?? 16;
```

**Avantages :**
- Code plus court et lisible
- Évite les vérifications `if` répétitives
- Sécurise l'accès aux données d'API

---

### Q19: Comment aplatir (flatten) un objet imbriqué ?

**Réponse :**

Aplatir un objet signifie transformer une structure imbriquée en structure plate avec des clés composées.

```javascript
function flattenObject(obj, prefix = '') {
  return Object.keys(obj).reduce((acc, key) => {
    const prefixedKey = prefix ? `${prefix}.${key}` : key;
    
    if (typeof obj[key] === 'object' && 
        obj[key] !== null && 
        !Array.isArray(obj[key])) {
      // Récursif pour les objets imbriqués
      Object.assign(acc, flattenObject(obj[key], prefixedKey));
    } else {
      // Ajouter la valeur
      acc[prefixedKey] = obj[key];
    }
    
    return acc;
  }, {});
}

// Test
const nested = {
  user: {
    name: 'Alice',
    age: 25,
    address: {
      city: 'Paris',
      country: 'France',
      coordinates: {
        lat: 48.8566,
        lng: 2.3522
      }
    }
  },
  settings: {
    theme: 'dark',
    notifications: true
  }
};

const flattened = flattenObject(nested);
console.log(flattened);
// {
//   'user.name': 'Alice',
//   'user.age': 25,
//   'user.address.city': 'Paris',
//   'user.address.country': 'France',
//   'user.address.coordinates.lat': 48.8566,
//   'user.address.coordinates.lng': 2.3522,
//   'settings.theme': 'dark',
//   'settings.notifications': true
// }

// Inverse : unflatten (déaplatir)
function unflattenObject(obj) {
  const result = {};
  
  for (const [path, value] of Object.entries(obj)) {
    const keys = path.split('.');
    let current = result;
    
    for (let i = 0; i < keys.length - 1; i++) {
      const key = keys[i];
      if (!current[key]) {
        current[key] = {};
      }
      current = current[key];
    }
    
    current[keys[keys.length - 1]] = value;
  }
  
  return result;
}

const unflattened = unflattenObject(flattened);
console.log(unflattened);
// Retourne la structure originale
```

**Cas d'usage :**
- Formulaires avec champs imbriqués
- Validation de données
- Transformation de données d'API
- Stockage en base de données plate

---

### Q20: Comment gérer des listes d'objets (CRUD immutable) pour React ?

**Réponse :**

Pour React, toutes les opérations doivent être **immutables** (créer de nouveaux arrays/objets).

```javascript
const [todos, setTodos] = useState([
  { id: 1, text: 'Learn JS', done: false },
  { id: 2, text: 'Learn React', done: false },
  { id: 3, text: 'Build app', done: false }
]);

// CREATE - Ajouter un todo
function addTodo(text) {
  const newTodo = {
    id: Date.now(),
    text,
    done: false
  };
  
  // ✅ Immutable
  setTodos([...todos, newTodo]);
  // ou avec callback pour éviter les stale closures
  setTodos(prevTodos => [...prevTodos, newTodo]);
}

// READ - Trouver un todo (pas besoin de setState)
function getTodo(id) {
  return todos.find(todo => todo.id === id);
}

// UPDATE - Modifier un todo
function toggleTodo(id) {
  setTodos(todos.map(todo =>
    todo.id === id
      ? { ...todo, done: !todo.done }  // nouveau objet si match
      : todo                            // même référence sinon
  ));
}

// Modifier plusieurs propriétés
function updateTodo(id, updates) {
  setTodos(todos.map(todo =>
    todo.id === id
      ? { ...todo, ...updates }
      : todo
  ));
}

// DELETE - Supprimer un todo
function deleteTodo(id) {
  setTodos(todos.filter(todo => todo.id !== id));
}

// BULK UPDATE - Modifier plusieurs todos
function markAllDone() {
  setTodos(todos.map(todo => ({ ...todo, done: true })));
}

function clearCompleted() {
  setTodos(todos.filter(todo => !todo.done));
}

// REORDER - Déplacer un todo
function moveTodo(fromIndex, toIndex) {
  setTodos(prevTodos => {
    const newTodos = [...prevTodos];
    const [removed] = newTodos.splice(fromIndex, 1);
    newTodos.splice(toIndex, 0, removed);
    return newTodos;
  });
}

// Rendu avec key unique
return (
  <ul>
    {todos.map(todo => (
      <li key={todo.id}>
        {todo.text}
        <button onClick={() => toggleTodo(todo.id)}>Toggle</button>
        <button onClick={() => deleteTodo(todo.id)}>Delete</button>
      </li>
    ))}
  </ul>
);
```

**Points critiques :**
- ✅ Toujours utiliser `key` unique (id, pas index)
- ✅ Créer de nouveaux objets/arrays (spread, map, filter)
- ✅ Utiliser la forme callback de `setState` si vous dépendez de l'état précédent
- ❌ Ne jamais muter directement (`push`, `splice`, modifications d'objets)

---

## Questions Pièges

### Q21: Que se passe-t-il avec cette syntaxe : `const { a, b } = { a: 1 }` ?

**Réponse :**

C'est du **destructuring** avec une valeur manquante.

```javascript
const { a, b } = { a: 1 };

console.log(a);  // 1
console.log(b);  // undefined (propriété n'existe pas)

// Avec valeurs par défaut
const { x, y = 10 } = { x: 5 };
console.log(x);  // 5
console.log(y);  // 10 (valeur par défaut)

// Piège classique
const { name } = null;  // Error: Cannot destructure property 'name' of 'null'

// Solution avec optional chaining
const obj = null;
const { name } = obj ?? {};  // Pas d'erreur, name = undefined
```

---

### Q22: Quelle est la différence entre ces deux syntaxes ?

```javascript
const obj = { name: 'Alice' };
delete obj.name;

const obj2 = { name: 'Alice' };
obj2.name = undefined;
```

**Réponse :**

Très différent !

```javascript
// delete - SUPPRIME la propriété
const obj1 = { name: 'Alice', age: 25 };
delete obj1.name;

console.log(obj1);              // { age: 25 }
console.log('name' in obj1);    // false - propriété n'existe plus
console.log(obj1.name);         // undefined
console.log(Object.keys(obj1)); // ['age']

// = undefined - la propriété EXISTE toujours
const obj2 = { name: 'Alice', age: 25 };
obj2.name = undefined;

console.log(obj2);              // { name: undefined, age: 25 }
console.log('name' in obj2);    // true - propriété existe
console.log(obj2.name);         // undefined
console.log(Object.keys(obj2)); // ['name', 'age']

// Impact sur JSON.stringify
console.log(JSON.stringify(obj1)); // {"age":25}
console.log(JSON.stringify(obj2)); // {"age":25} - undefined disparaît aussi !

// Mais itération différente
for (const key in obj1) {
  console.log(key);  // 'age' seulement
}

for (const key in obj2) {
  console.log(key);  // 'name' et 'age'
}
```

**Différences clés :**
- `delete` : supprime complètement la propriété
- `= undefined` : la propriété existe avec la valeur undefined
- Utilisez `delete` pour vraiment supprimer
- `hasOwnProperty()` et `in` se comportent différemment

---

### Q23: Que retourne `typeof null` et pourquoi ?

**Réponse :**

```javascript
console.log(typeof null);  // 'object' ❌

// C'est un BUG historique de JavaScript !
// null devrait retourner 'null', pas 'object'
```

**Pourquoi c'est important :**

```javascript
// Mauvaise vérification d'objet
function isObject(value) {
  return typeof value === 'object';  // ❌ Problème !
}

console.log(isObject({}));     // true ✅
console.log(isObject(null));   // true ❌ - faux positif
console.log(isObject([]));     // true ❌ - array n'est pas un plain object

// Bonne vérification
function isObject(value) {
  return value !== null && 
         typeof value === 'object' && 
         !Array.isArray(value);
}

console.log(isObject({}));     // true ✅
console.log(isObject(null));   // false ✅
console.log(isObject([]));     // false ✅
```

**Point historique :** En JavaScript, `null` représente l'absence intentionnelle d'objet, mais `typeof null` retourne `'object'` à cause d'un bug dans la première implémentation qui n'a jamais été corrigé pour des raisons de compatibilité.

---

### Q24: Expliquez ce code : `const obj = {}; obj.a.b = 1;`

**Réponse :**

```javascript
const obj = {};
obj.a.b = 1;  // ❌ Error: Cannot set property 'b' of undefined

// Pourquoi ? obj.a n'existe pas (undefined)
console.log(obj.a);  // undefined
// On essaie de faire : undefined.b = 1 → Error

// Solutions :

// 1. Créer d'abord obj.a
obj.a = {};
obj.a.b = 1;
console.log(obj);  // { a: { b: 1 } }

// 2. Tout en une fois
const obj2 = {
  a: {
    b: 1
  }
};

// 3. Optional chaining pour lecture (pas écriture)
console.log(obj.a?.b);  // undefined (pas d'erreur)
// Mais on ne peut pas écrire avec ?.
// obj.a?.b = 1;  // Syntaxe invalide

// 4. Helper function pour écriture sécurisée
function setNested(obj, path, value) {
  const keys = path.split('.');
  const lastKey = keys.pop();
  
  let current = obj;
  for (const key of keys) {
    if (!current[key]) {
      current[key] = {};
    }
    current = current[key];
  }
  
  current[lastKey] = value;
  return obj;
}

const obj3 = {};
setNested(obj3, 'a.b.c.d', 'value');
console.log(obj3);  // { a: { b: { c: { d: 'value' } } } }
```

**Piège fréquent en React :** Essayer d'accéder à des propriétés avant que les données soient chargées.

---

### Q25: Que se passe-t-il ici ?

```javascript
const a = { x: 1 };
const b = a;
b.x = 2;
console.log(a.x);  // ?
```

**Réponse :**

```javascript
const a = { x: 1 };
const b = a;  // b et a pointent vers le MÊME objet (référence)
b.x = 2;

console.log(a.x);  // 2 ❌ - a est aussi modifié !
console.log(a === b);  // true - même référence

// Les objets sont passés par RÉFÉRENCE, pas par valeur
```

**Visualisation :**

```javascript
// Mémoire :
// a → { x: 1 } ← b
// Ils pointent tous les deux vers le même objet en mémoire

// Quand on fait b.x = 2 :
// a → { x: 2 } ← b
// Le changement est visible par les deux références
```

**Pour créer une copie indépendante :**

```javascript
const a = { x: 1 };
const b = { ...a };  // ou Object.assign({}, a)
b.x = 2;

console.log(a.x);  // 1 ✅ - a est inchangé
console.log(b.x);  // 2
console.log(a === b);  // false - objets différents

// ⚠️ Attention : shallow copy seulement
const nested = { x: { y: 1 } };
const copy = { ...nested };
copy.x.y = 2;
console.log(nested.x.y);  // 2 ❌ - x est partagé !
```

**Important pour React :** C'est pourquoi on doit toujours créer de nouveaux objets/arrays au lieu de les muter directement.

---

### Q26: Différence entre `Object.keys()` et `for...in` ?

**Réponse :**

```javascript
// Créer un objet avec prototype
const parent = {
  inherited: 'from parent'
};

const child = Object.create(parent);
child.own = 'own property';

// Object.keys() - seulement les propriétés PROPRES
console.log(Object.keys(child));  // ['own']

// for...in - propriétés propres ET héritées
for (const key in child) {
  console.log(key);  // 'own', 'inherited'
}

// Pour avoir le même comportement que Object.keys()
for (const key in child) {
  if (child.hasOwnProperty(key)) {
    console.log(key);  // 'own' seulement
  }
}
```

**Différences principales :**

| Critère | Object.keys() | for...in |
|---------|--------------|----------|
| Propriétés héritées | ❌ Non | ✅ Oui |
| Retourne un array | ✅ Oui | ❌ Non (itération) |
| Propriétés non-énumérables | ❌ Non | ❌ Non |
| Performance | Rapide | Plus lent |

```javascript
// Exemple avec propriété non-énumérable
const obj = {};
Object.defineProperty(obj, 'hidden', {
  value: 'secret',
  enumerable: false  // non-énumérable
});
obj.visible = 'public';

console.log(Object.keys(obj));  // ['visible']

for (const key in obj) {
  console.log(key);  // 'visible' seulement
}

console.log(obj.hidden);  // 'secret' - existe mais invisible
```

**Recommandation :** Utilisez `Object.keys()`, `Object.values()` ou `Object.entries()` pour plus de clarté et éviter les propriétés héritées par surprise.

---

### Q27: Que fait ce code ? `const {a, ...rest} = {a: 1, b: 2, c: 3}`

**Réponse :**

C'est du **rest operator** dans le destructuring d'objet.

```javascript
const {a, ...rest} = {a: 1, b: 2, c: 3};

console.log(a);     // 1
console.log(rest);  // { b: 2, c: 3 }

// rest collecte toutes les propriétés NON destructurées
```

**Cas d'usage :**

```javascript
// 1. Supprimer des propriétés (immutable)
const user = {
  id: 1,
  name: 'Alice',
  password: 'secret123',
  email: 'alice@email.com'
};

const { password, ...safeUser } = user;
console.log(safeUser);
// { id: 1, name: 'Alice', email: 'alice@email.com' }

// 2. Séparer certaines props du reste
const { id, ...userData } = user;
console.log(id);        // 1
console.log(userData);  // { name: 'Alice', password: 'secret123', ... }

// 3. Props en React
function MyComponent({ title, className, ...otherProps }) {
  return (
    <div className={className} {...otherProps}>
      <h1>{title}</h1>
    </div>
  );
}

// 4. Extraire plusieurs propriétés
const { name, email, ...settings } = {
  name: 'Alice',
  email: 'alice@email.com',
  theme: 'dark',
  notifications: true,
  language: 'fr'
};

console.log(name);      // 'Alice'
console.log(email);     // 'alice@email.com'
console.log(settings);  // { theme: 'dark', notifications: true, language: 'fr' }

// ⚠️ rest doit être le dernier
const { a, ...rest, b } = obj;  // ❌ SyntaxError
const { a, b, ...rest } = obj;  // ✅ OK
```

**Différence avec spread :**

```javascript
// rest (...) dans destructuring - COLLECTE
const { a, ...rest } = obj;  // rest collecte

// spread (...) dans objet literal - ÉTALE
const merged = { ...obj1, ...obj2 };  // spread étale
```

---

### Q28: Pourquoi ce code ne fonctionne pas ? `const obj = { 0: 'a', 1: 'b' }; console.log(obj.0);`

**Réponse :**

```javascript
const obj = { 0: 'a', 1: 'b' };

console.log(obj.0);  // ❌ SyntaxError: Unexpected number

// Pourquoi ? Les noms de propriétés numériques ne peuvent pas
// être accédés avec dot notation

// ✅ Solution : utiliser bracket notation
console.log(obj[0]);   // 'a'
console.log(obj['0']); // 'a'
console.log(obj[1]);   // 'b'

// Les clés numériques sont automatiquement converties en strings
console.log(Object.keys(obj));  // ['0', '1'] - strings
```

**Règles des noms de propriétés :**

```javascript
const obj = {
  validName: 1,        // ✅ Identifiant valide
  'with-dash': 2,      // ✅ Avec quotes
  'with space': 3,     // ✅ Avec quotes
  123: 4,              // ✅ Nombre (converti en string)
  [Symbol()]: 5        // ✅ Symbol
};

// Accès
obj.validName;          // ✅ dot notation
obj['with-dash'];       // ✅ bracket (obligatoire)
obj['with space'];      // ✅ bracket (obligatoire)
obj[123];               // ✅ bracket (obligatoire)
obj.123;                // ❌ SyntaxError
```

---

### Q29: Que se passe-t-il avec `JSON.parse(JSON.stringify(obj))` ?

**Réponse :**

C'est une technique courante pour faire une **deep copy**, mais elle a des **limitations importantes** :

```javascript
const original = {
  string: 'hello',
  number: 42,
  boolean: true,
  null: null,
  nested: { a: 1 },
  array: [1, 2, 3]
};

const copy = JSON.parse(JSON.stringify(original));
// ✅ Fonctionne bien pour les types simples

console.log(copy);  // Copie complète
console.log(copy === original);  // false - objets différents
```

**❌ Limitations (perte de données) :**

```javascript
const problematic = {
  // ❌ undefined disparaît
  undef: undefined,
  
  // ❌ Functions disparaissent
  func: () => 'hello',
  
  // ❌ Symbols disparaissent
  sym: Symbol('id'),
  
  // ❌ Date devient string
  date: new Date(),
  
  // ❌ RegExp devient objet vide
  regex: /test/g,
  
  // ❌ Infinity/NaN deviennent null
  inf: Infinity,
  nan: NaN,
  
  // ❌ Map/Set deviennent objets vides
  map: new Map([['key', 'value']]),
  set: new Set([1, 2, 3])
};

const result = JSON.parse(JSON.stringify(problematic));
console.log(result);
// {
//   date: "2024-11-06T..." (string!),
//   regex: {},
//   inf: null,
//   nan: null,
//   map: {},
//   set: {}
//   // undef, func, sym ont disparu !
// }

// ❌ Références circulaires causent une erreur
const circular = { name: 'test' };
circular.self = circular;
JSON.stringify(circular);  // Error: Converting circular structure to JSON
```

**Alternatives modernes :**

```javascript
// 1. structuredClone() - ES2021 (recommandé)
const copy1 = structuredClone(original);
// ✅ Gère Date, Map, Set, ArrayBuffer, etc.
// ✅ Gère les références circulaires
// ❌ Ne copie pas les fonctions

// 2. Bibliothèques
import _ from 'lodash';
const copy2 = _.cloneDeep(original);
// ✅ Plus robuste, gère presque tout
```

**Quand utiliser JSON.parse/stringify :**
- ✅ Données simples (strings, numbers, booleans, arrays, plain objects)
- ✅ Données d'API (JSON)
- ❌ Objets avec méthodes
- ❌ Instances de classes
- ❌ Types complexes (Date, RegExp, etc.)

---

### Q30: Expliquez la différence entre ces deux objets :

```javascript
const obj1 = { toString: () => 'custom' };
const obj2 = { toString() { return 'custom'; } };
```

**Réponse :**

Les deux créent une méthode `toString`, mais avec des différences importantes :

```javascript
// Arrow function
const obj1 = {
  name: 'Alice',
  toString: () => 'custom'
};

// Method shorthand
const obj2 = {
  name: 'Alice',
  toString() {
    return 'custom';
  }
};

// Différence 1 : this
const obj3 = {
  name: 'Alice',
  toString: () => `Name: ${this.name}`  // ❌ this = undefined
};

const obj4 = {
  name: 'Alice',
  toString() {
    return `Name: ${this.name}`;  // ✅ this = obj4
  }
};

console.log(obj3.toString());  // "Name: undefined"
console.log(obj4.toString());  // "Name: Alice"

// Différence 2 : prototype
console.log(obj1.toString.prototype);  // undefined (arrow n'a pas prototype)
console.log(obj2.toString.prototype);  // {} (méthode normale a prototype)

// Différence 3 : arguments
const obj5 = {
  sum: (...nums) => nums.reduce((a, b) => a + b, 0),
  sum2() {
    // arguments disponible (mais préférez rest params)
    return Array.from(arguments).reduce((a, b) => a + b, 0);
  }
};

// Différence 4 : super
class Parent {
  greet() {
    return 'Hello from parent';
  }
}

class Child extends Parent {
  greet1: () => super.greet()  // ❌ SyntaxError
  
  greet2() {
    return super.greet();  // ✅ Fonctionne
  }
}
```

**Recommandation :**
- ✅ Utilisez **method shorthand** pour les méthodes d'objets (besoin de `this`, `super`, etc.)
- ✅ Utilisez **arrow functions** pour les callbacks ou quand vous voulez capturer `this` du scope parent

```javascript
// Bon usage
const user = {
  name: 'Alice',
  
  // Méthode : method shorthand
  greet() {
    return `Hello, ${this.name}`;
  },
  
  // Callback : arrow function (capture this)
  delayedGreet() {
    setTimeout(() => {
      console.log(this.greet());  // this = user
    }, 1000);
  }
};
```
