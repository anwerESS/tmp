# 📚 Annexes et Bonnes Pratiques JavaScript

---

## 🎯 A. Conventions de Code

### **📝 Naming Conventions**

#### **Variables et Fonctions : camelCase**

```javascript
// ✅ CORRECT
let userName = 'Jean';
let totalPrice = 150;
const isActive = true;

function calculateTotal() {}
function getUserById(id) {}

// ❌ INCORRECT
let user_name = 'Jean';      // snake_case (Python style)
let UserName = 'Jean';        // PascalCase (réservé aux classes)
let TOTAL_PRICE = 150;        // SCREAMING_SNAKE_CASE (réservé aux constantes)
```

#### **Classes et Constructeurs : PascalCase**

```javascript
// ✅ CORRECT
class UserAccount {}
class ShoppingCart {}
class PaymentProcessor {}

// ❌ INCORRECT
class userAccount {}
class shopping_cart {}
```

#### **Constantes : SCREAMING_SNAKE_CASE**

```javascript
// ✅ CORRECT - Valeurs vraiment constantes
const MAX_USERS = 100;
const API_BASE_URL = 'https://api.exemple.com';
const DEFAULT_TIMEOUT = 5000;

// ✅ ACCEPTABLE - Objets/arrays non modifiés
const CONFIG = { apiKey: 'abc123' };
const ROUTES = ['/home', '/about'];

// Les propriétés restent en camelCase
const DATABASE_CONFIG = {
  host: 'localhost',
  port: 5432,
  maxConnections: 10
};
```

#### **Variables Privées : préfixe underscore**

```javascript
class User {
  constructor(name) {
    this.name = name;
    this._password = null;  // Convention : propriété "privée"
  }
  
  _hashPassword(password) {  // Méthode "privée"
    return hash(password);
  }
}

// Note : Avec les champs privés ES2022
class UserModerne {
  #password;  // Vraiment privé (syntaxe officielle)
  
  constructor(password) {
    this.#password = password;
  }
}
```

#### **Booléens : préfixes interrogatifs**

```javascript
// ✅ CORRECT - Préfixes is, has, should, can
const isActive = true;
const hasPermission = false;
const shouldUpdate = true;
const canEdit = false;
const wasDeleted = true;

// ❌ INCORRECT - Pas clair
const active = true;
const permission = false;
```

#### **Fonctions : verbes d'action**

```javascript
// ✅ CORRECT - Commence par un verbe
function getUser() {}
function setConfig() {}
function calculateTotal() {}
function validateEmail() {}
function fetchData() {}
function handleClick() {}

// ❌ INCORRECT - Pas d'action claire
function user() {}
function total() {}
```

---

### **🎨 Indentation et Formatage**

#### **Indentation : 2 espaces (standard JS)**

```javascript
// ✅ CORRECT
function exemple() {
  if (condition) {
    console.log('Test');
  }
}

// ❌ INCORRECT - Mélange d'indentations
function exemple() {
  if (condition) {
      console.log('Incohérent');
  }
}
```

#### **Accolades : style K&R**

```javascript
// ✅ CORRECT - Accolade ouvrante sur la même ligne
function test() {
  if (condition) {
    doSomething();
  } else {
    doSomethingElse();
  }
}

// ⚠️ DANGER - Automatic Semicolon Insertion
function getObject() {
  return    // ❌ JavaScript insère ; ici !
  {
    value: 42
  };
}
// Retourne undefined au lieu de l'objet !
```

#### **Espacement**

```javascript
// ✅ CORRECT
const sum = a + b;
const result = (x * y) / z;
if (age >= 18 && isActive) {}

// Pas d'espace entre fonction et parenthèse
function test() {}
const fn = () => {};

// Espace après les virgules
const arr = [1, 2, 3];
function calculate(a, b, c) {}

// ❌ INCORRECT
const sum=a+b;
const arr = [1,2,3];
function test () {}
```

---

### **💬 Comments et Documentation**

#### **Commentaires inline**

```javascript
// ✅ CORRECT - Explique le "pourquoi", pas le "quoi"
const RETRY_DELAY = 1000; // Délai entre tentatives pour éviter rate limiting

// Workaround pour bug IE11 avec flexbox
element.style.display = '-ms-flexbox';

// ❌ INCORRECT - Redondant
const age = 30; // Déclare la variable age à 30
i++; // Incrémente i
```

#### **JSDoc : documentation formelle**

```javascript
/**
 * Récupère un utilisateur par son ID
 * 
 * @param {number} userId - L'ID de l'utilisateur
 * @param {Object} options - Options de récupération
 * @param {boolean} options.includeDeleted - Inclure les utilisateurs supprimés
 * @returns {Promise<User>} L'utilisateur trouvé
 * @throws {NotFoundError} Si l'utilisateur n'existe pas
 * 
 * @example
 * const user = await getUserById(123);
 * console.log(user.name);
 */
async function getUserById(userId, options = {}) {
  // Implémentation...
}
```

#### **Tags spéciaux**

```javascript
// TODO: Ajouter la validation d'email
function createUser(data) {}

// FIXME: Bug avec les dates avant 1970
function formatDate(date) {}

// HACK: Solution temporaire en attendant correction API
const response = await fetch(url).then(r => r.json() || {});

// NOTE: Cette fonction sera dépréciée dans la v3.0
function oldMethod() {}
```

---

## 🔍 B. Debugging

### **Console Methods**

#### **console.log() - Affichage basique**

```javascript
const user = { name: 'Jean', age: 30 };

console.log('User:', user);
console.log(`User ${user.name} has age ${user.age}`);

// Objets avec labels
console.log({ user }); // { user: { name: 'Jean', age: 30 } }
```

#### **console.table() - Tableaux formatés**

```javascript
const users = [
  { id: 1, name: 'Alice', age: 25, role: 'Admin' },
  { id: 2, name: 'Bob', age: 30, role: 'User' },
  { id: 3, name: 'Charlie', age: 35, role: 'Moderator' }
];

// Affiche un tableau formaté très lisible
console.table(users);

// Sélectionner des colonnes spécifiques
console.table(users, ['name', 'role']);
```

#### **console.time() / timeEnd() - Mesure de performance**

```javascript
console.time('FetchUsers');

await fetch('https://api.example.com/users')
  .then(r => r.json())
  .then(users => {
    console.log('Users loaded:', users.length);
  });

console.timeEnd('FetchUsers');
// Affiche : FetchUsers: 234.56ms
```

#### **console.group() - Groupement**

```javascript
console.group('User Details');
  console.log('Name:', user.name);
  console.log('Email:', user.email);
  console.log('Permissions:', user.permissions);
console.groupEnd();

// Groupes imbriqués
console.group('API Calls');
  console.log('Fetching users...');
  
  console.group('User 1');
    console.log('ID:', 1);
    console.log('Name:', 'Alice');
  console.groupEnd();
console.groupEnd();
```

#### **Autres méthodes utiles**

```javascript
// console.error() - Affiche en rouge
console.error('Erreur critique !');

// console.warn() - Affiche en jaune/orange
console.warn('Cette fonction sera dépréciée');

// console.assert() - N'affiche que si false
console.assert(age >= 0, 'Age doit être positif', { age });

// console.trace() - Stack trace complète
console.trace('Voici la stack trace');

// console.count() - Compteur
console.count('function called'); // function called: 1
console.count('function called'); // function called: 2
```

---

### **🐛 Breakpoints et Debugger**

#### **debugger statement**

```javascript
function problematicFunction(data) {
  const processed = preProcess(data);
  
  debugger; // Le code s'arrête ICI si DevTools est ouvert
  
  const result = complexCalculation(processed);
  return result;
}
```

#### **Debugger conditionnel**

```javascript
function processItem(item) {
  // S'arrête seulement pour un item spécifique
  if (item.id === 123) {
    debugger;
  }
  
  return doSomething(item);
}
```

#### **Breakpoints dans DevTools**

```javascript
// 1. Ouvrir DevTools (F12)
// 2. Onglet "Sources" (Chrome) ou "Debugger" (Firefox)
// 3. Cliquer sur le numéro de ligne pour ajouter un breakpoint

function calculateTotal(items) {
  let total = 0;
  
  for (let item of items) {  // ← Cliquer ici pour breakpoint
    total += item.price;
  }
  
  return total;
}

// Breakpoint conditionnel : clic droit → "Add conditional breakpoint"
// Exemple : user.id === 123
```

---

## ⚡ C. Performance

### **💾 Éviter les Fuites Mémoire**

#### **1. Event Listeners non nettoyés**

```javascript
// ❌ MAUVAIS - Fuite mémoire
function setupButton() {
  const button = document.querySelector('#myButton');
  
  button.addEventListener('click', () => {
    console.log('Clicked');
  });
  // Si on supprime le button du DOM, il reste en mémoire !
}

// ✅ BON - Nettoyer les listeners
function setupButtonCorrect() {
  const button = document.querySelector('#myButton');
  
  const handleClick = () => console.log('Clicked');
  button.addEventListener('click', handleClick);
  
  // Cleanup
  return () => {
    button.removeEventListener('click', handleClick);
  };
}

const cleanup = setupButtonCorrect();
// Plus tard...
cleanup(); // Libère la mémoire
```

#### **2. Timers non annulés**

```javascript
// ❌ MAUVAIS - L'interval continue indéfiniment
function startPolling() {
  setInterval(() => {
    fetch('/api/status').then(r => r.json());
  }, 5000);
}

// ✅ BON - Stocker et annuler
function startPollingCorrect() {
  const intervalId = setInterval(() => {
    fetch('/api/status').then(r => r.json());
  }, 5000);
  
  return () => clearInterval(intervalId);
}

const stopPolling = startPollingCorrect();
// Plus tard...
stopPolling();
```

#### **3. Closures gardant des références**

```javascript
// ❌ MAUVAIS - hugeArray reste en mémoire
function createHugeClosure() {
  const hugeArray = new Array(1000000).fill('data');
  
  return function() {
    console.log('Doing something');
    // hugeArray est capturé par la closure
  };
}

// ✅ BON - Extraire seulement ce qui est nécessaire
function createSmallClosure() {
  const hugeArray = new Array(1000000).fill('data');
  const neededValue = hugeArray[0];
  
  return function() {
    console.log('Value:', neededValue);
    // Seulement neededValue est gardé
  };
}
```

---

### **🚀 Optimisation des Boucles**

#### **1. Éviter les calculs répétés**

```javascript
// ❌ LENT - Calcule length à chaque itération
for (let i = 0; i < items.length; i++) {
  process(items[i]);
}

// ✅ RAPIDE - Cache la length
const length = items.length;
for (let i = 0; i < length; i++) {
  process(items[i]);
}

// ✅ MEILLEUR - forEach (lisible et optimisé)
items.forEach(item => process(item));
```

#### **2. Réduire les accès DOM**

```javascript
// ❌ LENT - Accès DOM dans la boucle
for (let i = 0; i < 1000; i++) {
  document.getElementById('result').innerHTML += `<div>${i}</div>`;
  // Reflow/repaint à chaque itération !
}

// ✅ RAPIDE - Construire string puis un seul accès DOM
let html = '';
for (let i = 0; i < 1000; i++) {
  html += `<div>${i}</div>`;
}
document.getElementById('result').innerHTML = html;

// ✅ ENCORE MIEUX - Document Fragment
const fragment = document.createDocumentFragment();
for (let i = 0; i < 1000; i++) {
  const div = document.createElement('div');
  div.textContent = i;
  fragment.appendChild(div);
}
document.getElementById('result').appendChild(fragment);
```

#### **3. Break early (sortie anticipée)**

```javascript
const users = [/*... 10000 users*/];

// ❌ Parcourt tout même si trouvé
let found = null;
users.forEach(user => {
  if (user.id === 123) found = user;
});

// ✅ S'arrête dès qu'on trouve
for (let user of users) {
  if (user.id === 123) {
    found = user;
    break; // Économise du temps
  }
}

// ✅ Ou utiliser find()
const found = users.find(user => user.id === 123);
```

---

### **🎯 Debouncing et Throttling**

#### **Debounce - Attendre la fin**

```javascript
/**
 * Exécute la fonction après que l'utilisateur ait arrêté d'agir
 * Use case : recherche en temps réel, validation
 */
function debounce(func, delay) {
  let timeoutId;
  
  return function(...args) {
    clearTimeout(timeoutId);
    timeoutId = setTimeout(() => func.apply(this, args), delay);
  };
}

// Exemple : Recherche
const searchInput = document.querySelector('#search');

const performSearch = (query) => {
  fetch(`/api/search?q=${query}`)
    .then(r => r.json())
    .then(results => displayResults(results));
};

const debouncedSearch = debounce(performSearch, 500);
searchInput.addEventListener('input', (e) => {
  debouncedSearch(e.target.value);
});
```

#### **Throttle - Limiter la fréquence**

```javascript
/**
 * Exécute la fonction maximum une fois par intervalle
 * Use case : scroll, resize, mouse move
 */
function throttle(func, delay) {
  let lastCall = 0;
  
  return function(...args) {
    const now = Date.now();
    
    if (now - lastCall >= delay) {
      lastCall = now;
      func.apply(this, args);
    }
  };
}

// Exemple : Scroll tracking
const throttledScroll = throttle(() => {
  console.log('Scroll position:', window.scrollY);
}, 1000);

window.addEventListener('scroll', throttledScroll);
```

#### **Comparaison visuelle**

```javascript
// Événements : ||||||||||||||||||||||||||||||||||||

// SANS limitation :
// Exécutions  : ||||||||||||||||||||||||||||||||||||

// DEBOUNCE (500ms) :
// Exécutions  :                                    | (une fois à la fin)

// THROTTLE (500ms) :
// Exécutions  : |         |         |         |    (régulièrement)
```

---

## ⚛️ D. Transition vers React

### **Concepts JavaScript Essentiels**

#### **1. Destructuring (omniprésent)**

```javascript
// Props destructuring
function UserCard({ name, age, email }) {
  return (
    <div>
      <h2>{name}</h2>
      <p>Age: {age}</p>
      <p>Email: {email}</p>
    </div>
  );
}

// Array destructuring avec useState
const [count, setCount] = useState(0);
const [user, setUser] = useState(null);

// Destructuring imbriqué
function Profile({ user: { name, address: { city } } }) {
  return <div>{name} from {city}</div>;
}
```

#### **2. Arrow Functions**

```javascript
// Event handlers
<button onClick={() => console.log('Clicked')}>Click</button>

// Map pour rendering
{users.map(user => (
  <UserCard key={user.id} user={user} />
))}

// Callbacks
useEffect(() => {
  fetchData();
}, []);
```

#### **3. Template Literals**

```javascript
// JSX avec expressions
<div className={`card ${isActive ? 'active' : ''}`}>
  <h2>{`Hello, ${user.name}!`}</h2>
  <p>You have {count} notifications</p>
</div>

// API calls
fetch(`/api/users/${userId}`)
```

#### **4. Spread Operator**

```javascript
// Props spreading
const userProps = { name: 'Jean', age: 30 };
<UserCard {...userProps} />

// State updates (immutabilité)
const [user, setUser] = useState({ name: 'Jean', age: 30 });
setUser({ ...user, age: 31 }); // ✅ Crée une nouvelle référence

// Arrays
const [items, setItems] = useState([1, 2, 3]);
setItems([...items, 4]); // Ajoute sans muter
```

#### **5. Ternary Operator**

```javascript
// Rendering conditionnel
{isLoading ? (
  <Spinner />
) : (
  <UserList users={users} />
)}

// Inline dans JSX
<button disabled={count === 0 ? true : false}>
  {count > 0 ? 'Submit' : 'No items'}
</button>
```

#### **6. Short-Circuit Evaluation**

```javascript
// Rendering conditionnel avec &&
{isLoggedIn && <Dashboard />}
{error && <ErrorMessage error={error} />}

// Valeurs par défaut avec ||
const displayName = user.name || 'Anonymous';

// Nullish coalescing avec ??
const port = config.port ?? 3000;
```

#### **7. Array Methods**

```javascript
// map - Le plus utilisé en React
{users.map(user => (
  <UserCard key={user.id} user={user} />
))}

// filter
{products.filter(p => p.inStock).map(p => (
  <ProductCard key={p.id} product={p} />
))}

// reduce
const total = cart.reduce((sum, item) => sum + item.price, 0);

// find
const user = users.find(u => u.id === selectedId);

// some / every
const hasAdmin = users.some(u => u.role === 'admin');
const allActive = users.every(u => u.isActive);
```

---

### **JSX est du JavaScript**

```javascript
// JSX n'est que du sucre syntaxique
<div className="container">Hello</div>

// Est compilé en :
React.createElement('div', { className: 'container' }, 'Hello')

// Expressions JavaScript dans JSX
<div>
  {2 + 2} {/* 4 */}
  {user.name.toUpperCase()}
  {items.length > 0 && <List items={items} />}
</div>

// Tout est une expression
const element = (
  <div>
    {(() => {
      if (isLoading) return <Spinner />;
      if (error) return <Error />;
      return <Content />;
    })()}
  </div>
);
```

---

### **Composants = Fonctions**

```javascript
// Un composant React est juste une fonction
function Button({ label, onClick }) {
  return <button onClick={onClick}>{label}</button>;
}

// Équivalent à :
const Button = (props) => {
  return <button onClick={props.onClick}>{props.label}</button>;
};

// Composant avec logique
function UserProfile({ userId }) {
  const [user, setUser] = useState(null);
  const [loading, setLoading] = useState(true);
  
  useEffect(() => {
    fetch(`/api/users/${userId}`)
      .then(r => r.json())
      .then(data => {
        setUser(data);
        setLoading(false);
      });
  }, [userId]);
  
  if (loading) return <div>Loading...</div>;
  if (!user) return <div>User not found</div>;
  
  return (
    <div>
      <h2>{user.name}</h2>
      <p>{user.email}</p>
    </div>
  );
}
```

---

### **Hooks = Closures + State Management**

#### **useState - Closure sur la variable d'état**

```javascript
function Counter() {
  const [count, setCount] = useState(0);
  
  // setCount a accès à count via closure
  const increment = () => {
    setCount(count + 1); // Utilise la valeur actuelle
    
    // Ou avec fonction (accès à la dernière valeur)
    setCount(prev => prev + 1);
  };
  
  return (
    <div>
      <p>Count: {count}</p>
      <button onClick={increment}>+1</button>
    </div>
  );
}
```

#### **useEffect - Closures et dépendances**

```javascript
function SearchResults({ query }) {
  const [results, setResults] = useState([]);
  
  useEffect(() => {
    // Cette fonction a accès à query via closure
    fetch(`/api/search?q=${query}`)
      .then(r => r.json())
      .then(data => setResults(data));
    
    // Cleanup function (aussi une closure)
    return () => {
      console.log('Cleanup for query:', query);
    };
  }, [query]); // Re-exécute quand query change
  
  return (
    <ul>
      {results.map(r => <li key={r.id}>{r.title}</li>)}
    </ul>
  );
}
```

#### **useCallback - Mémorisation de fonction**

```javascript
function Parent() {
  const [count, setCount] = useState(0);
  
  // Sans useCallback : nouvelle fonction à chaque render
  // const handleClick = () => setCount(count + 1);
  
  // Avec useCallback : fonction mémorisée
  const handleClick = useCallback(() => {
    setCount(c => c + 1);
  }, []); // Dépendances vides = fonction stable
  
  return <Child onClick={handleClick} />;
}
```

#### **Hooks personnalisés - Réutilisation de logique**

```javascript
// Custom hook = fonction utilisant d'autres hooks
function useFetch(url) {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  
  useEffect(() => {
    setLoading(true);
    
    fetch(url)
      .then(r => r.json())
      .then(data => {
        setData(data);
        setLoading(false);
      })
      .catch(err => {
        setError(err);
        setLoading(false);
      });
  }, [url]);
  
  return { data, loading, error };
}

// Utilisation
function UserList() {
  const { data: users, loading, error } = useFetch('/api/users');
  
  if (loading) return <div>Loading...</div>;
  if (error) return <div>Error: {error.message}</div>;
  
  return (
    <ul>
      {users.map(user => <li key={user.id}>{user.name}</li>)}
    </ul>
  );
}
```

---

### **🎯 Points Clés pour React**

```javascript
// 1. Immutabilité (ne pas muter les states)
// ❌ MAUVAIS
const [user, setUser] = useState({ name: 'Jean', age: 30 });
user.age = 31; // Mutation !
setUser(user); // React ne détecte pas le changement

// ✅ BON
setUser({ ...user, age: 31 }); // Nouvelle référence

// 2. Pure functions (composants sans side effects)
// ✅ BON - Pur (même props = même rendu)
function Greeting({ name }) {
  return <h1>Hello, {name}!</h1>;
}

// ❌ MAUVAIS - Side effect dans le render
function BadComponent() {
  fetch('/api/data'); // ❌ Ne pas faire !
  return <div>Content</div>;
}

// ✅ BON - Side effects dans useEffect
function GoodComponent() {
  useEffect(() => {
    fetch('/api/data');
  }, []);
  
  return <div>Content</div>;
}

// 3. Closures et stale state
function Counter() {
  const [count, setCount] = useState(0);
  
  useEffect(() => {
    const interval = setInterval(() => {
      // ❌ Stale closure - count vaut toujours 0
      setCount(count + 1);
      
      // ✅ Solution - fonction de mise à jour
      setCount(c => c + 1);
    }, 1000);
    
    return () => clearInterval(interval);
  }, []); // Pas de count dans les dépendances
  
  return <div>{count}</div>;
}
```

---

# Questions/Réponses - Annexes et Bonnes Pratiques

---

## 📋 Questions sur Conventions de Code

### Q1 : Quelles sont les conventions de nommage en JavaScript et pourquoi sont-elles importantes ?

**Réponse :**
JavaScript utilise plusieurs conventions selon le type d'élément :

**camelCase** pour variables et fonctions :
```javascript
let userName = 'Jean';
function calculateTotal() {}
```

**PascalCase** pour classes et constructeurs :
```javascript
class UserAccount {}
class ShoppingCart {}
```

**SCREAMING_SNAKE_CASE** pour constantes :
```javascript
const MAX_USERS = 100;
const API_BASE_URL = 'https://api.exemple.com';
```

**Préfixes pour booléens** (is, has, can, should) :
```javascript
const isActive = true;
const hasPermission = false;
const canEdit = true;
```

**Importance :**
- Améliore la lisibilité du code
- Facilite la collaboration en équipe
- Permet de comprendre rapidement le type et le rôle d'une variable
- Standard dans l'industrie (ESLint, Prettier le vérifient)

**En entretien, souligner :** La cohérence est plus importante que la convention elle-même. Dans une équipe, on suit les conventions établies.

---

### Q2 : Pourquoi utilise-t-on des préfixes comme "is", "has", "can" pour les variables booléennes ?

**Réponse :**
Ces préfixes rendent le code auto-documenté et plus lisible en formant des questions en anglais.

```javascript
// ✅ CLAIR - On comprend que c'est un boolean
const isActive = true;
const hasPermission = false;
const canEdit = user.role === 'admin';
const shouldUpdate = data.modified;

// Utilisation dans des conditions
if (isActive) {}           // Se lit naturellement
if (hasPermission) {}
if (canEdit) {}

// ❌ MOINS CLAIR
const active = true;       // String ? Boolean ? Object ?
const permission = false;
const edit = user.role === 'admin';

if (active) {}             // Moins explicite
```

**Autres préfixes courants :**
- `was` / `were` : pour actions passées (`wasDeleted`, `wereNotified`)
- `should` : pour décisions (`shouldRefresh`, `shouldValidate`)
- `will` : pour actions futures (`willExpire`)

**En React, très utilisé :**
```javascript
const [isLoading, setIsLoading] = useState(false);
const [hasError, setHasError] = useState(false);
const [canSubmit, setCanSubmit] = useState(true);
```

---

### Q3 : Quelle est la différence entre un commentaire qui explique le "quoi" et le "pourquoi" ?

**Réponse :**
Le code doit expliquer le **"quoi"** (ce qu'il fait), les commentaires doivent expliquer le **"pourquoi"** (la raison).

```javascript
// ❌ MAUVAIS - Explique le "quoi" (redondant)
// Incrémente i
i++;

// Ajoute user à users array
users.push(user);

// Déclare une variable age à 30
const age = 30;

// ✅ BON - Explique le "pourquoi"
// Délai de 1s entre tentatives pour éviter rate limiting de l'API
const RETRY_DELAY = 1000;

// Workaround pour bug IE11 avec flexbox
element.style.display = '-ms-flexbox';

// On utilise setTimeout au lieu de setInterval pour éviter
// l'empilement des appels si la requête prend plus de 5s
setTimeout(function poll() {
  fetch('/api/status')
    .then(r => r.json())
    .then(() => setTimeout(poll, 5000));
}, 5000);

// Cache le résultat car cette opération est coûteuse (O(n²))
const cachedResult = expensiveCalculation(data);
```

**Principe : Si le code est bien écrit, il se lit comme du langage naturel**

```javascript
// Pas besoin de commentaire ici
function calculateTotalWithTax(items, taxRate) {
  const subtotal = items.reduce((sum, item) => sum + item.price, 0);
  return subtotal * (1 + taxRate);
}

// Meilleur qu'un commentaire "Calcule le total avec taxes"
```

**Quand commenter :**
- Algorithmes complexes
- Workarounds et hacks temporaires
- Décisions business non évidentes
- TODOs et FIXMEs
- Code contre-intuitif mais nécessaire

---

## 🔍 Questions sur Debugging

### Q4 : Quelles sont les méthodes console les plus utiles au-delà de console.log() ?

**Réponse :**
Il existe de nombreuses méthodes console pour différents cas d'usage :

**1. console.table() - Pour visualiser des données structurées**
```javascript
const users = [
  { id: 1, name: 'Alice', role: 'Admin' },
  { id: 2, name: 'Bob', role: 'User' },
  { id: 3, name: 'Charlie', role: 'Moderator' }
];

console.table(users);
// Affiche un tableau formaté très lisible

console.table(users, ['name', 'role']); // Colonnes spécifiques
```

**2. console.time() / timeEnd() - Mesurer la performance**
```javascript
console.time('DatabaseQuery');
await database.query('SELECT * FROM users');
console.timeEnd('DatabaseQuery');
// DatabaseQuery: 234.56ms
```

**3. console.group() - Organiser les logs**
```javascript
console.group('User Registration');
  console.log('Validating email...');
  console.log('Hashing password...');
  console.log('Creating user...');
console.groupEnd();

// Groupes repliés par défaut
console.groupCollapsed('Debug Info');
  console.log('Détails techniques...');
console.groupEnd();
```

**4. console.error() / warn() - Niveaux de sévérité**
```javascript
console.error('Erreur critique !'); // Rouge
console.warn('Attention !');        // Jaune/Orange
console.info('Information');        // Bleu (certains navigateurs)
```

**5. console.assert() - Tests inline**
```javascript
const age = -5;
console.assert(age >= 0, 'Age must be positive', { age });
// N'affiche que si la condition est false
```

**6. console.trace() - Stack trace**
```javascript
function a() { b(); }
function b() { c(); }
function c() { console.trace('Call stack:'); }
a(); // Affiche toute la chaîne d'appels
```

**En entretien, montrer qu'on connaît les outils adaptés à chaque situation.**

---

### Q5 : Qu'est-ce que le debugger statement et quand l'utiliser ?

**Réponse :**
Le `debugger` statement est un breakpoint programmatique qui met en pause l'exécution du code si les DevTools sont ouverts.

```javascript
function calculateDiscount(price, category) {
  let discount = 0;
  
  if (category === 'premium') {
    discount = 0.20;
  } else if (category === 'regular') {
    discount = 0.10;
  }
  
  debugger; // Le code s'arrête ICI
  
  return price * (1 - discount);
}
```

**Avantages :**
- Pas besoin d'ouvrir les DevTools pour localiser le fichier
- Peut être conditionnel
- Utile dans du code généré ou minifié
- Pratique pour déboguer des boucles

**Cas d'usage :**

**1. Debug conditionnel**
```javascript
function processItems(items) {
  items.forEach((item, index) => {
    // S'arrête seulement à l'item 50
    if (index === 50) {
      debugger;
    }
    
    // S'arrête seulement pour un item spécifique
    if (item.id === '123abc') {
      debugger;
    }
  });
}
```

**2. Dans des callbacks async**
```javascript
fetch('/api/data')
  .then(response => response.json())
  .then(data => {
    debugger; // Inspecter la réponse API
    processData(data);
  });
```

**3. Dans du code temporaire**
```javascript
// Pendant le développement
if (process.env.NODE_ENV === 'development') {
  debugger;
}
```

**⚠️ Important : RETIRER avant production !**
```javascript
// ❌ Ne JAMAIS laisser en production
function deployToProduction() {
  debugger; // Causera une pause pour tous les utilisateurs !
}

// ✅ Protéger avec une condition
if (process.env.NODE_ENV === 'development') {
  debugger;
}
```

**Pendant le debug, on peut :**
- Inspecter les variables dans la console
- Modifier les valeurs à la volée
- Exécuter du code arbitraire
- Step over / step into / step out

---

### Q6 : Comment déboguer efficacement dans le navigateur avec les DevTools ?

**Réponse :**
Les DevTools offrent plusieurs outils puissants pour le debugging :

**1. Breakpoints classiques**
- Ouvrir DevTools (F12)
- Onglet Sources/Debugger
- Cliquer sur le numéro de ligne

**2. Breakpoints conditionnels**
```javascript
// Clic droit sur la ligne → "Add conditional breakpoint"
function processUsers(users) {
  for (let user of users) {
    updateUser(user); // Breakpoint : user.id === 123
    // S'arrête SEULEMENT quand user.id vaut 123
  }
}
```

**3. Event Listener Breakpoints**
- Pause automatique sur clicks, submits, etc.
- Utile pour identifier quel code gère un événement

**4. XHR/Fetch Breakpoints**
```javascript
// S'arrête automatiquement avant/après les requêtes
// DevTools → Sources → XHR/fetch Breakpoints
// Ajouter pattern : "api/users"

fetch('https://api.example.com/users') // S'arrête ici
  .then(r => r.json());
```

**5. Exception Breakpoints**
- Pause on all exceptions
- Pause on uncaught exceptions
- Utile pour trouver les erreurs silencieuses

**6. DOM Breakpoints**
- Break on subtree modifications
- Break on attribute modifications
- Break on node removal
```javascript
// Clic droit sur élément → Break on → ...
element.classList.add('active'); // Pause si breakpoint sur attributes
```

**7. Contrôles pendant le debug**
- **Step over (F10)** : Ligne suivante
- **Step into (F11)** : Entre dans la fonction
- **Step out (Shift+F11)** : Sort de la fonction
- **Continue (F8)** : Continue jusqu'au prochain breakpoint

**8. Watch Expressions**
```javascript
// Ajouter des expressions à surveiller
// Exemple : user.name, items.length, isValid
```

**9. Call Stack**
- Voir toute la chaîne d'appels
- Naviguer dans les frames précédentes

**10. Scope Variables**
- Inspecter Local, Closure, Global scopes
- Voir toutes les variables accessibles

**Best practice :** Combiner plusieurs techniques selon le problème.

---

## ⚡ Questions sur Performance

### Q7 : Qu'est-ce qu'une fuite mémoire (memory leak) et comment les éviter en JavaScript ?

**Réponse :**
Une **fuite mémoire** survient quand la mémoire allouée n'est jamais libérée, même si elle n'est plus utilisée. Le garbage collector ne peut pas libérer la mémoire si des références persistent.

**Causes principales :**

**1. Event listeners non supprimés**
```javascript
// ❌ FUITE - Le listener garde l'élément en mémoire
function attachHandler() {
  const button = document.querySelector('#btn');
  
  button.addEventListener('click', () => {
    console.log('Clicked');
  });
  
  // Si on supprime le button du DOM mais pas le listener,
  // le button reste en mémoire
}

// ✅ SOLUTION - Nettoyer les listeners
function attachHandlerCorrect() {
  const button = document.querySelector('#btn');
  const handler = () => console.log('Clicked');
  
  button.addEventListener('click', handler);
  
  // Retourner une fonction de cleanup
  return () => {
    button.removeEventListener('click', handler);
  };
}

const cleanup = attachHandlerCorrect();
// Quand on n'en a plus besoin :
cleanup();
```

**2. Timers non annulés**
```javascript
// ❌ FUITE - L'interval continue indéfiniment
function startPolling() {
  setInterval(() => {
    fetch('/api/status');
  }, 5000);
  // Même si on change de page, l'interval continue !
}

// ✅ SOLUTION - Stocker et annuler
function startPollingCorrect() {
  const intervalId = setInterval(() => {
    fetch('/api/status');
  }, 5000);
  
  return () => clearInterval(intervalId);
}

const stop = startPollingCorrect();
// Plus tard :
stop();
```

**3. Closures capturant de gros objets**
```javascript
// ❌ FUITE - hugeArray reste en mémoire
function createHandler() {
  const hugeArray = new Array(1000000).fill('data');
  
  return function() {
    console.log('Handler called');
    // hugeArray est capturé par la closure
  };
}

// ✅ SOLUTION - N'extraire que ce qui est nécessaire
function createHandlerCorrect() {
  const hugeArray = new Array(1000000).fill('data');
  const firstItem = hugeArray[0]; // Extraire seulement ce qu'on utilise
  
  return function() {
    console.log('First item:', firstItem);
    // Seulement firstItem est gardé
  };
}
```

**4. Références circulaires (rares avec les GC modernes)**
```javascript
// ❌ Potentiellement problématique
const obj1 = {};
const obj2 = {};
obj1.ref = obj2;
obj2.ref = obj1; // Circulaire

// ✅ Briser le cycle quand terminé
obj1.ref = null;
obj2.ref = null;
```

**5. Cache sans limite**
```javascript
// ❌ FUITE - Cache grandit indéfiniment
const cache = {};
function getCached(key) {
  if (!cache[key]) {
    cache[key] = expensiveOperation(key);
  }
  return cache[key];
}

// ✅ SOLUTION - Cache avec limite (LRU)
class LRUCache {
  constructor(maxSize = 100) {
    this.maxSize = maxSize;
    this.cache = new Map();
  }
  
  set(key, value) {
    if (this.cache.size >= this.maxSize) {
      const firstKey = this.cache.keys().next().value;
      this.cache.delete(firstKey); // Supprimer le plus ancien
    }
    this.cache.set(key, value);
  }
}
```

**Détecter les fuites mémoire :**
- DevTools → Memory → Take Heap Snapshot
- Comparer plusieurs snapshots
- Chercher les objets qui ne devraient plus exister
- Profiler la mémoire pendant l'utilisation

**En React :**
```javascript
useEffect(() => {
  const handler = () => {};
  window.addEventListener('scroll', handler);
  
  // Cleanup obligatoire
  return () => {
    window.removeEventListener('scroll', handler);
  };
}, []);
```

---

### Q8 : Comment optimiser les performances des boucles en JavaScript ?

**Réponse :**
Plusieurs techniques permettent d'optimiser les boucles :

**1. Cacher les propriétés length**
```javascript
const items = [...]; // 10000 éléments

// ❌ LENT - Accède à .length à chaque itération
for (let i = 0; i < items.length; i++) {
  process(items[i]);
}

// ✅ RAPIDE - Cache la length
const len = items.length;
for (let i = 0; i < len; i++) {
  process(items[i]);
}
```

**2. Minimiser les accès DOM**
```javascript
// ❌ TRÈS LENT - Reflow/repaint à chaque itération
for (let i = 0; i < 1000; i++) {
  document.getElementById('list').innerHTML += `<li>${i}</li>`;
}

// ✅ RAPIDE - Construire puis un seul accès DOM
let html = '';
for (let i = 0; i < 1000; i++) {
  html += `<li>${i}</li>`;
}
document.getElementById('list').innerHTML = html;

// ✅ ENCORE MIEUX - DocumentFragment
const fragment = document.createDocumentFragment();
for (let i = 0; i < 1000; i++) {
  const li = document.createElement('li');
  li.textContent = i;
  fragment.appendChild(li);
}
document.getElementById('list').appendChild(fragment);
```

**3. Choisir la bonne méthode d'itération**
```javascript
const numbers = [1, 2, 3, ..., 1000000];

// Performance (du plus rapide au plus lent) :
// 1. for loop classique (le plus rapide)
for (let i = 0; i < numbers.length; i++) {}

// 2. for...of (rapide et lisible)
for (let num of numbers) {}

// 3. forEach (légèrement plus lent, mais lisible)
numbers.forEach(num => {});

// 4. map/filter/reduce (créent de nouveaux arrays)
numbers.map(num => num * 2);
```

**4. Break early (sortie anticipée)**
```javascript
const users = [/* 100000 users */];

// ❌ Parcourt TOUT l'array
let found = null;
users.forEach(user => {
  if (user.id === 123) found = user;
});

// ✅ S'arrête dès qu'on trouve
for (let user of users) {
  if (user.id === 123) {
    found = user;
    break; // Économise potentiellement 99999 itérations
  }
}

// ✅ Ou utiliser find()
const found = users.find(user => user.id === 123);
```

**5. Éviter les calculs répétés dans la boucle**
```javascript
// ❌ Calcule à chaque itération
for (let i = 0; i < items.length; i++) {
  const result = expensiveFunction() * items[i];
  // expensiveFunction() est appelée items.length fois !
}

// ✅ Calculer une fois avant la boucle
const constant = expensiveFunction();
for (let i = 0; i < items.length; i++) {
  const result = constant * items[i];
}
```

**6. Éviter les chaînages inutiles**
```javascript
// ❌ LENT - 3 passes sur l'array
const result = numbers
  .map(n => n * 2)
  .filter(n => n > 10)
  .map(n => n + 1);

// ✅ RAPIDE - Une seule passe
const result = numbers.reduce((acc, n) => {
  const doubled = n * 2;
  if (doubled > 10) {
    acc.push(doubled + 1);
  }
  return acc;
}, []);
```

**7. Utiliser les bonnes structures de données**
```javascript
// Recherche dans array : O(n)
const users = [/* 10000 users */];
const user = users.find(u => u.id === 123); // Peut prendre du temps

// Recherche dans Map : O(1)
const usersMap = new Map(users.map(u => [u.id, u]));
const user = usersMap.get(123); // Instantané
```

**En entretien, mentionner :**
- Profiler d'abord (performance.now(), console.time())
- Optimiser seulement si nécessaire
- Lisibilité vs performance : trouver l'équilibre

---

### Q9 : Quelle est la différence entre debouncing et throttling, et quand utiliser chacun ?

**Réponse :**
Ce sont deux techniques de **rate limiting** pour contrôler la fréquence d'exécution d'une fonction.

**DEBOUNCING : Attend la fin de l'action**
- Exécute la fonction seulement après un délai sans nouvelle invocation
- "Attendre que l'utilisateur ait fini"

```javascript
function debounce(func, delay) {
  let timeoutId;
  return function(...args) {
    clearTimeout(timeoutId);
    timeoutId = setTimeout(() => func.apply(this, args), delay);
  };
}

// Exemple : recherche autocomplete
const search = debounce((query) => {
  fetch(`/api/search?q=${query}`);
}, 500);

searchInput.addEventListener('input', (e) => {
  search(e.target.value);
});

// Si l'utilisateur tape "react" :
// r    → timer démarre
// re   → timer reset
// rea  → timer reset
// reac → timer reset
// react → timer reset
// (pause 500ms) → EXÉCUTE
```

**THROTTLING : Limite la fréquence**
- Exécute la fonction au maximum une fois par intervalle
- "Maximum X fois par seconde"

```javascript
function throttle(func, delay) {
  let lastCall = 0;
  return function(...args) {
    const now = Date.now();
    if (now - lastCall >= delay) {
      lastCall = now;
      func.apply(this, args);
    }
  };
}

// Exemple : scroll tracking
const trackScroll = throttle(() => {
  console.log('Position:', window.scrollY);
}, 1000);

window.addEventListener('scroll', trackScroll);

// Si l'utilisateur scroll continuellement :
// 0ms    → EXÉCUTE
// 500ms  → ignoré
// 1000ms → EXÉCUTE
// 1500ms → ignoré
// 2000ms → EXÉCUTE
```

**Comparaison visuelle :**
```
Événements : ||||||||||||||||||||||||||||||||||||

Sans limitation :
||||||||||||||||||||||||||||||||||||

DEBOUNCE (500ms) :
                                    | (une fois à la fin)

THROTTLE (500ms) :
|         |         |         |    (régulièrement)
```

**Quand utiliser DEBOUNCE :**
- ✅ Recherche en temps réel / Autocomplete
- ✅ Validation de formulaire
- ✅ Sauvegarde automatique (auto-save)
- ✅ Resize de fenêtre (ajustement final)
- ✅ API calls après saisie utilisateur

**Quand utiliser THROTTLE :**
- ✅ Scroll events (infinite scroll, parallax)
- ✅ Mouse move / tracking
- ✅ Window resize (animations pendant le resize)
- ✅ Jeux / animations (limiter la fréquence de mise à jour)
- ✅ Progress tracking

**Cas pratiques :**

```javascript
// 1. DEBOUNCE : Recherche autocomplete
const searchBox = document.querySelector('#search');
const autocomplete = debounce(async (query) => {
  const results = await fetch(`/api/search?q=${query}`)
    .then(r => r.json());
  displaySuggestions(results);
}, 300);

// 2. THROTTLE : Infinite scroll
const loadMore = throttle(() => {
  const bottom = document.documentElement.scrollHeight - 
    (window.scrollY + window.innerHeight);
  if (bottom < 200) fetchNextPage();
}, 200);

// 3. DEBOUNCE : Auto-save
const autoSave = debounce((content) => {
  localStorage.setItem('draft', content);
}, 2000);

// 4. THROTTLE : Mouse tracking (heatmap)
const trackMouse = throttle((e) => {
  sendAnalytics(e.clientX, e.clientY);
}, 100);
```

**En entretien, souligner :**
- Debounce = "Attend que l'utilisateur ait fini"
- Throttle = "Limite la fréquence"
- Choisir selon le besoin métier
- Peut combiner les deux dans certains cas

---

## ⚛️ Questions sur React et JavaScript

### Q10 : Quels concepts JavaScript sont essentiels pour bien comprendre React ?

**Réponse :**
React repose fortement sur des concepts JavaScript modernes :

**1. Destructuring (omniprésent)**
```javascript
// Props destructuring
function UserCard({ name, age, email }) {
  return <div>{name}, {age}</div>;
}

// useState destructuring
const [count, setCount] = useState(0);
const [user, setUser] = useState(null);

// Destructuring imbriqué
function Profile({ user: { name, address: { city } } }) {
  return <div>{name} from {city}</div>;
}
```

**2. Arrow Functions**
```javascript
// Event handlers
<button onClick={() => setCount(count + 1)}>+</button>

// Map pour rendering
{users.map(user => (
  <UserCard key={user.id} {...user} />
))}

// useEffect callbacks
useEffect(() => {
  fetchData();
}, []);
```

**3. Spread Operator**
```javascript
// Props spreading
const props = { name: 'Jean', age: 30 };
<UserCard {...props} />

// State updates (immutabilité)
setUser({ ...user, age: 31 });
setItems([...items, newItem]);

// Merge d'objets
const config = { ...defaultConfig, ...userConfig };
```

**4. Ternary et Short-Circuit**
```javascript
// Rendering conditionnel
{isLoading ? <Spinner /> : <Content />}
{error && <ErrorMessage error={error} />}
{user?.profile?.avatar && <Avatar src={user.profile.avatar} />}

// Valeurs par défaut
const name = user.name || 'Anonymous';
const port = config.port ?? 3000;
```

**5. Array Methods**
```javascript
// map - Le plus utilisé
{products.map(p => <ProductCard key={p.id} product={p} />)}

// filter
{users.filter(u => u.isActive).map(u => <User key={u.id} {...u} />)}

// reduce
const total = cart.reduce((sum, item) => sum + item.price, 0);

// find, some, every
const admin = users.find(u => u.role === 'admin');
const hasErrors = fields.some(f => !f.isValid);
```

**6. Template Literals**
```javascript
<div className={`card ${isActive ? 'active' : ''}`}>
  <h2>{`Hello, ${user.name}!`}</h2>
</div>

// API calls
fetch(`/api/users/${userId}/posts/${postId}`)
```

**7. Promises et Async/Await**
```javascript
useEffect(() => {
  async function fetchUser() {
    const response = await fetch('/api/user');
    const data = await response.json();
    setUser(data);
  }
  fetchUser();
}, []);
```

**8. Closures** (fondamental pour les hooks)
```javascript
function Counter() {
  const [count, setCount] = useState(0);
  
  // Cette fonction a accès à count via closure
  const increment = () => {
    setCount(count + 1);
  };
  
  return <button onClick={increment}>{count}</button>;
}
```

**En entretien, mentionner :**
- React n'est QUE du JavaScript
- Comprendre JS moderne = comprendre React
- Les hooks utilisent massivement les closures

---

### Q11 : Expliquez comment JSX est du JavaScript.

**Réponse :**
JSX n'est qu'un **sucre syntaxique** pour `React.createElement()`. C'est du JavaScript qui ressemble à du HTML.

**Transformation :**
```javascript
// JSX (ce qu'on écrit)
const element = <h1 className="title">Hello, {name}!</h1>;

// JavaScript pur (ce qui est exécuté)
const element = React.createElement(
  'h1',
  { className: 'title' },
  'Hello, ',
  name,
  '!'
);
```

**JSX est une expression JavaScript**
```javascript
// ✅ On peut assigner JSX à une variable
const greeting = <h1>Hello!</h1>;

// ✅ On peut retourner JSX d'une fonction
function Welcome() {
  return <h1>Welcome!</h1>;
}

// ✅ On peut passer JSX en argument
const element = createElement(<div>Content</div>);

// ✅ On peut utiliser JSX dans des ternaires
const content = isLoggedIn ? <Dashboard /> : <Login />;

// ✅ On peut stocker JSX dans un array
const items = [
  <li key="1">Item 1</li>,
  <li key="2">Item 2</li>
];
```

**Expressions JavaScript dans JSX**
```javascript
function App() {
  const name = 'Jean';
  const age = 30;
  const items = ['A', 'B', 'C'];
  
  return (
    <div>
      {/* Variables */}
      <h1>{name}</h1>
      
      {/* Expressions */}
      <p>Next year: {age + 1}</p>
      <p>Uppercase: {name.toUpperCase()}</p>
      
      {/* Ternaire */}
      <p>{age >= 18 ? 'Adult' : 'Minor'}</p>
      
      {/* Short-circuit */}
      {isLoggedIn && <Dashboard />}
      
      {/* Map */}
      <ul>
        {items.map((item, i) => <li key={i}>{item}</li>)}
      </ul>
      
      {/* IIFE pour logique complexe */}
      {(() => {
        if (status === 'loading') return <Spinner />;
        if (status === 'error') return <Error />;
        return <Content />;
      })()}
    </div>
  );
}
```

**JSX n'accepte que des expressions, pas des statements**
```javascript
// ❌ INVALIDE - if/else est un statement
<div>
  {if (condition) { return <p>True</p> }}
</div>

// ✅ VALIDE - Ternaire est une expression
<div>
  {condition ? <p>True</p> : <p>False</p>}
</div>

// ❌ INVALIDE - for loop est un statement
<ul>
  {for (let i = 0; i < 10; i++) { <li>{i}</li> }}
</ul>

// ✅ VALIDE - map est une expression
<ul>
  {Array.from({length: 10}).map((_, i) => <li key={i}>{i}</li>)}
</ul>
```

**Attributs en JSX = props JavaScript**
```javascript
// JSX
<input 
  type="text" 
  value={inputValue} 
  onChange={handleChange}
  disabled={isDisabled}
  className="input-field"
/>

// Compilé en :
React.createElement('input', {
  type: 'text',
  value: inputValue,
  onChange: handleChange,
  disabled: isDisabled,
  className: 'input-field'
});
```

**Différences JSX vs HTML**
```javascript
// HTML : class, for
// JSX : className, htmlFor (car ce sont des mots réservés JS)
<label htmlFor="name" className="label">Name:</label>

// HTML : style="color: red"
// JSX : style={{color: 'red'}} (objet JavaScript)
<div style={{backgroundColor: 'blue', fontSize: '16px'}}>Content</div>

// HTML : onclick="handleClick()"
// JSX : onClick={handleClick} (référence de fonction)
<button onClick={handleClick}>Click</button>

// HTML : value, checked sont des attributs
// JSX : ce sont des props contrôlées
<input value={value} onChange={e => setValue(e.target.value)} />
```

**En entretien, souligner :**
- JSX = JavaScript, pas un langage template
- Babel transforme JSX en appels React.createElement
- Tout ce qui fonctionne en JS fonctionne dans les {}
- JSX rend React plus lisible mais n'est pas obligatoire

---

### Q12 : Pourquoi dit-on que les composants React sont "juste des fonctions" ?

**Réponse :**
Un composant React est littéralement une fonction JavaScript qui retourne du JSX (qui lui-même est du JavaScript).

**Composant = Fonction**
```javascript
// Composant React basique
function Welcome(props) {
  return <h1>Hello, {props.name}!</h1>;
}

// C'est juste une fonction qui :
// 1. Prend des arguments (props)
// 2. Retourne du JSX (React elements)
// 3. Peut contenir de la logique JavaScript

// Utilisation
<Welcome name="Jean" /> // Appel de fonction déguisé

// Équivalent à (simplifié) :
Welcome({ name: 'Jean' })
```

**Composant avec logique**
```javascript
function UserProfile({ userId }) {
  // Variables locales
  const [user, setUser] = useState(null);
  const [loading, setLoading] = useState(true);
  
  // Side effects
  useEffect(() => {
    fetchUser(userId).then(data => {
      setUser(data);
      setLoading(false);
    });
  }, [userId]);
  
  // Logique conditionnelle
  if (loading) return <Spinner />;
  if (!user) return <NotFound />;
  
  // Calculs
  const age = calculateAge(user.birthDate);
  const initials = user.name.split(' ').map(n => n[0]).join('');
  
  // Return JSX
  return (
    <div className="profile">
      <Avatar initials={initials} />
      <h2>{user.name}</h2>
      <p>Age: {age}</p>
    </div>
  );
}

// C'est toujours juste une fonction JavaScript !
```

**Arrow function = aussi un composant**
```javascript
// Function declaration
function Button({ label, onClick }) {
  return <button onClick={onClick}>{label}</button>;
}

// Arrow function (exactement pareil)
const Button = ({ label, onClick }) => {
  return <button onClick={onClick}>{label}</button>;
};

// Arrow function avec return implicite
const Button = ({ label, onClick }) => (
  <button onClick={onClick}>{label}</button>
);
```

**Props = paramètres de fonction**
```javascript
// Sans destructuring
function Card(props) {
  return (
    <div>
      <h2>{props.title}</h2>
      <p>{props.description}</p>
    </div>
  );
}

// Avec destructuring (plus courant)
function Card({ title, description }) {
  return (
    <div>
      <h2>{title}</h2>
      <p>{description}</p>
    </div>
  );
}

// Props avec valeurs par défaut
function Card({ title = 'Untitled', description = '' }) {
  return (
    <div>
      <h2>{title}</h2>
      <p>{description}</p>
    </div>
  );
}

// Utilisation
<Card title="Hello" description="World" />
// Équivalent à :
Card({ title: 'Hello', description: 'World' })
```

**Composants = composables (comme des fonctions)**
```javascript
// Petits composants
function Avatar({ src, alt }) {
  return <img src={src} alt={alt} className="avatar" />;
}

function UserInfo({ name, email }) {
  return (
    <div>
      <h3>{name}</h3>
      <p>{email}</p>
    </div>
  );
}

// Composés en composant plus grand
function UserCard({ user }) {
  return (
    <div className="card">
      <Avatar src={user.avatar} alt={user.name} />
      <UserInfo name={user.name} email={user.email} />
    </div>
  );
}

// Exactement comme composer des fonctions en JS
```

**Pure functions (recommandé)**
```javascript
// ✅ Pure - Même props = même rendu
function Greeting({ name }) {
  return <h1>Hello, {name}!</h1>;
}

// ❌ Impure - Side effects dans le render
function BadComponent() {
  fetch('/api/data'); // ❌ Ne pas faire !
  Math.random(); // ❌ Non déterministe
  console.log('Rendering'); // ❌ Side effect
  
  return <div>Content</div>;
}

// ✅ Side effects dans useEffect
function GoodComponent() {
  useEffect(() => {
    fetch('/api/data'); // ✅ Dans useEffect
  }, []);
  
  return <div>Content</div>;
}
```

**Higher-Order Components = Higher-Order Functions**
```javascript
// HOC = Fonction qui prend un composant et retourne un composant
function withLoading(Component) {
  return function WithLoadingComponent({ isLoading, ...props }) {
    if (isLoading) return <Spinner />;
    return <Component {...props} />;
  };
}

// Utilisation
const UserListWithLoading = withLoading(UserList);

// C'est comme les HOF en JS
const numbers = [1, 2, 3];
const doubled = numbers.map(n => n * 2); // map est une HOF
```

**En entretien, mentionner :**
- Composants = fonctions JavaScript ordinaires
- Props = arguments, JSX = valeur de retour
- Peuvent utiliser toutes les features JS (variables, conditions, boucles)
- Penser "fonctionnel" aide à comprendre React
- Les hooks renforcent cette approche fonctionnelle

---

### Q13 : Comment les hooks React utilisent-ils les closures JavaScript ?

**Réponse :**
Les hooks sont **entièrement basés sur les closures**. Chaque hook "capture" les valeurs de son environnement via les closures.

**useState et closures**
```javascript
function Counter() {
  const [count, setCount] = useState(0);
  
  // Cette fonction a accès à count via closure
  const increment = () => {
    console.log('Current count:', count); // Closure sur count
    setCount(count + 1);
  };
  
  return (
    <div>
      <p>{count}</p>
      <button onClick={increment}>+1</button>
    </div>
  );
}

// À chaque render, une nouvelle closure est créée
// Render 1 : count = 0, increment capture count = 0
// Render 2 : count = 1, increment capture count = 1
// etc.
```

**Problème : Stale closures**
```javascript
function Counter() {
  const [count, setCount] = useState(0);
  
  useEffect(() => {
    const interval = setInterval(() => {
      // ❌ Stale closure - count vaut toujours 0
      console.log('Count:', count); // Toujours 0
      setCount(count + 1); // Ajoute 1 à 0, toujours
    }, 1000);
    
    return () => clearInterval(interval);
  }, []); // count n'est pas dans les dépendances
  
  return <div>{count}</div>; // Reste à 1
}

// Pourquoi ? La closure capture count = 0 au premier render
// L'interval n'est jamais recréé ([] dépendances)
// Donc il garde toujours count = 0
```

**Solutions au stale closure**

**1. Fonction de mise à jour**
```javascript
function Counter() {
  const [count, setCount] = useState(0);
  
  useEffect(() => {
    const interval = setInterval(() => {
      // ✅ Fonction de mise à jour - accès à la dernière valeur
      setCount(prevCount => {
        console.log('Count:', prevCount); // Valeur actuelle
        return prevCount + 1;
      });
    }, 1000);
    
    return () => clearInterval(interval);
  }, []); // Pas besoin de count dans les dépendances
  
  return <div>{count}</div>; // S'incrémente correctement
}
```

**2. Ajouter dans les dépendances**
```javascript
function Counter() {
  const [count, setCount] = useState(0);
  
  useEffect(() => {
    const interval = setInterval(() => {
      console.log('Count:', count); // Toujours la bonne valeur
      setCount(count + 1);
    }, 1000);
    
    return () => clearInterval(interval);
  }, [count]); // ✅ Recrée l'interval quand count change
  
  return <div>{count}</div>;
}
```

**useEffect et closures**
```javascript
function UserProfile({ userId }) {
  const [user, setUser] = useState(null);
  
  useEffect(() => {
    // Cette fonction capture userId via closure
    console.log('Fetching user:', userId);
    
    fetch(`/api/users/${userId}`)
      .then(r => r.json())
      .then(data => setUser(data));
    
    // Cleanup a aussi accès à userId via closure
    return () => {
      console.log('Cleanup for user:', userId);
    };
  }, [userId]); // Re-exécute quand userId change
  
  return <div>{user?.name}</div>;
}

// Chaque fois que userId change :
// 1. Cleanup de l'ancien effect (avec l'ancien userId)
// 2. Nouvel effect (avec le nouveau userId)
```

**useCallback - mémoriser des fonctions**
```javascript
function Parent() {
  const [count, setCount] = useState(0);
  
  // ❌ Sans useCallback - nouvelle fonction à chaque render
  const handleClick = () => {
    console.log('Count:', count); // Closure sur count
    setCount(count + 1);
  };
  
  // ✅ Avec useCallback - fonction mémorisée
  const handleClickMemo = useCallback(() => {
    console.log('Count:', count);
    setCount(count + 1);
  }, [count]); // Recrée seulement si count change
  
  return <Child onClick={handleClickMemo} />;
}

// Pourquoi ? React.memo(Child) peut skip le re-render
// si handleClickMemo est la même référence
```

**useMemo - mémoriser des valeurs calculées**
```javascript
function ExpensiveComponent({ items }) {
  const [filter, setFilter] = useState('');
  
  // ❌ Sans useMemo - recalcule à chaque render
  const filteredItems = items.filter(item => 
    item.name.includes(filter)
  );
  
  // ✅ Avec useMemo - recalcule seulement si items ou filter change
  const filteredItemsMemo = useMemo(() => {
    console.log('Filtering...');
    return items.filter(item => item.name.includes(filter));
  }, [items, filter]); // Closures sur items et filter
  
  return (
    <div>
      <input value={filter} onChange={e => setFilter(e.target.value)} />
      {filteredItemsMemo.map(item => <div key={item.id}>{item.name}</div>)}
    </div>
  );
}
```

**useRef - échapper aux closures**
```javascript
function Timer() {
  const [count, setCount] = useState(0);
  const countRef = useRef(count);
  
  // Mettre à jour la ref à chaque render
  useEffect(() => {
    countRef.current = count;
  });
  
  useEffect(() => {
    const interval = setInterval(() => {
      // ✅ countRef.current a toujours la dernière valeur
      console.log('Count:', countRef.current);
    }, 1000);
    
    return () => clearInterval(interval);
  }, []); // Pas besoin de count dans les dépendances
  
  return <div>{count}</div>;
}

// useRef persiste entre les renders
// .current n'est pas capturé par closure
```

**Custom hooks et closures**
```javascript
function useDebounce(value, delay) {
  const [debouncedValue, setDebouncedValue] = useState(value);
  
  useEffect(() => {
    // Closure sur value et delay
    const handler = setTimeout(() => {
      setDebouncedValue(value);
    }, delay);
    
    // Cleanup a accès à handler via closure
    return () => {
      clearTimeout(handler);
    };
  }, [value, delay]); // Dépendances importantes
  
  return debouncedValue;
}

// Utilisation
function SearchBox() {
  const [query, setQuery] = useState('');
  const debouncedQuery = useDebounce(query, 500);
  
  useEffect(() => {
    if (debouncedQuery) {
      // Closure sur debouncedQuery
      fetch(`/api/search?q=${debouncedQuery}`)
        .then(r => r.json())
        .then(results => setResults(results));
    }
  }, [debouncedQuery]);
  
  return <input value={query} onChange={e => setQuery(e.target.value)} />;
}
```

**En entretien, souligner :**
- Hooks = closures JavaScript appliquées à React
- Stale closures = problème courant avec useEffect
- Fonction de mise à jour = solution élégante
- useRef = échapper aux closures quand nécessaire
- Comprendre les closures = comprendre les hooks

---

### Q14 : Pourquoi l'immutabilité est-elle importante en React ?

**Réponse :**
React se base sur la **comparaison de références** pour détecter les changements. L'immutabilité garantit que React détecte correctement les mises à jour.

**Problème avec la mutation**
```javascript
function TodoList() {
  const [todos, setTodos] = useState([
    { id: 1, text: 'Tâche 1', done: false },
    { id: 2, text: 'Tâche 2', done: false }
  ]);
  
  const toggleTodo = (id) => {
    // ❌ MUTATION - Modifie directement l'array
    const todo = todos.find(t => t.id === id);
    todo.done = !todo.done; // Mutation de l'objet
    setTodos(todos); // Même référence !
    
    // React compare :
    // oldTodos === newTodos → true (même référence)
    // Donc React ne re-render PAS ! Le UI ne se met pas à jour
  };
  
  return (
    <ul>
      {todos.map(todo => (
        <li key={todo.id}>
          <input 
            type="checkbox" 
            checked={todo.done}
            onChange={() => toggleTodo(todo.id)}
          />
          {todo.text}
        </li>
      ))}
    </ul>
  );
}
```

**Solution : Immutabilité**
```javascript
function TodoList() {
  const [todos, setTodos] = useState([
    { id: 1, text: 'Tâche 1', done: false },
    { id: 2, text: 'Tâche 2', done: false }
  ]);
  
  const toggleTodo = (id) => {
    // ✅ IMMUTABILITÉ - Crée un nouveau array
    setTodos(todos.map(todo => 
      todo.id === id 
        ? { ...todo, done: !todo.done } // Nouveau objet
        : todo // Garde la même référence pour les autres
    ));
    
    // React compare :
    // oldTodos !== newTodos → false (différente référence)
    // React détecte le changement et re-render ✅
  };
  
  return (
    <ul>
      {todos.map(todo => (
        <li key={todo.id}>
          <input 
            type="checkbox" 
            checked={todo.done}
            onChange={() => toggleTodo(todo.id)}
          />
          {todo.text}
        </li>
      ))}
    </ul>
  );
}
```

**Patterns d'immutabilité pour Arrays**

**1. Ajouter un élément**
```javascript
const [items, setItems] = useState([1, 2, 3]);

// ❌ MUTATION
items.push(4);
setItems(items);

// ✅ IMMUTABILITÉ
setItems([...items, 4]); // Spread operator
setItems(items.concat(4)); // concat crée un nouveau array
```

**2. Supprimer un élément**
```javascript
// ❌ MUTATION
items.splice(index, 1);
setItems(items);

// ✅ IMMUTABILITÉ
setItems(items.filter((_, i) => i !== index));
setItems(items.filter(item => item.id !== targetId));
```

**3. Modifier un élément**
```javascript
// ❌ MUTATION
items[index] = newValue;
setItems(items);

// ✅ IMMUTABILITÉ
setItems(items.map((item, i) => 
  i === index ? newValue : item
));

setItems(items.map(item =>
  item.id === targetId ? { ...item, status: 'done' } : item
));
```

**4. Insérer un élément**
```javascript
// ❌ MUTATION
items.splice(index, 0, newItem);
setItems(items);

// ✅ IMMUTABILITÉ
setItems([
  ...items.slice(0, index),
  newItem,
  ...items.slice(index)
]);
```

**Patterns d'immutabilité pour Objects**

**1. Modifier une propriété**
```javascript
const [user, setUser] = useState({ name: 'Jean', age: 30, city: 'Paris' });

// ❌ MUTATION
user.age = 31;
setUser(user);

// ✅ IMMUTABILITÉ
setUser({ ...user, age: 31 });

// Plusieurs propriétés
setUser({ ...user, age: 31, city: 'Lyon' });
```

**2. Objets imbriqués**
```javascript
const [user, setUser] = useState({
  name: 'Jean',
  address: {
    city: 'Paris',
    country: 'France'
  }
});

// ❌ MUTATION (même avec spread au premier niveau)
const newUser = { ...user };
newUser.address.city = 'Lyon'; // Mutation de l'objet imbriqué !
setUser(newUser);

// ✅ IMMUTABILITÉ - Spread imbriqué
setUser({
  ...user,
  address: {
    ...user.address,
    city: 'Lyon'
  }
});
```

**3. Ajouter/supprimer une propriété**
```javascript
// Ajouter
setUser({ ...user, email: 'jean@example.com' });

// Supprimer (destructuring)
const { email, ...userWithoutEmail } = user;
setUser(userWithoutEmail);

// Ou avec delete (mais créer nouveau objet d'abord)
const newUser = { ...user };
delete newUser.email;
setUser(newUser);
```

**Cas complexes**

**Array d'objets imbriqué**
```javascript
const [users, setUsers] = useState([
  { id: 1, name: 'Alice', profile: { bio: 'Hello' } },
  { id: 2, name: 'Bob', profile: { bio: 'Hi' } }
]);

// Modifier le bio de l'utilisateur avec id: 1
setUsers(users.map(user =>
  user.id === 1
    ? {
        ...user,
        profile: {
          ...user.profile,
          bio: 'Updated bio'
        }
      }
    : user
));
```

**State avec plusieurs niveaux**
```javascript
const [state, setState] = useState({
  ui: {
    theme: 'dark',
    sidebar: {
      open: false,
      width: 200
    }
  },
  data: {
    users: [],
    posts: []
  }
});

// Modifier sidebar.open
setState({
  ...state,
  ui: {
    ...state.ui,
    sidebar: {
      ...state.ui.sidebar,
      open: true
    }
  }
});

// Conseil : Éviter les structures trop profondes
// Mieux : Utiliser plusieurs useState ou useReducer
```

**Pourquoi l'immutabilité est importante**

**1. Détection de changements (React)**
```javascript
// React utilise Object.is() pour comparer
const prevState = [1, 2, 3];
const nextState = prevState; // Même référence

Object.is(prevState, nextState); // true
// React ne détecte pas de changement

const newState = [...prevState, 4]; // Nouvelle référence
Object.is(prevState, newState); // false
// React détecte le changement ✅
```

**2. Performance avec React.memo()**
```javascript
// Composant mémorisé
const ExpensiveComponent = React.memo(({ data }) => {
  console.log('Rendering ExpensiveComponent');
  return <div>{data.value}</div>;
});

function Parent() {
  const [data, setData] = useState({ value: 42 });
  
  // ❌ MUTATION - Re-render même si valeur identique
  const updateBad = () => {
    data.value = 42; // Même valeur
    setData(data); // Même référence, mais React re-render quand même
  };
  
  // ✅ IMMUTABILITÉ - Skip re-render si valeur identique
  const updateGood = () => {
    setData({ value: 42 }); // Nouvelle référence
    // React.memo compare et skip si value identique
  };
}
```

**3. Time-travel debugging**
```javascript
// Avec immutabilité, on peut garder l'historique
const [history, setHistory] = useState([initialState]);
const [currentStep, setCurrentStep] = useState(0);

const currentState = history[currentStep];

// Ajouter un nouvel état
const addState = (newState) => {
  const newHistory = history.slice(0, currentStep + 1);
  setHistory([...newHistory, newState]);
  setCurrentStep(currentStep + 1);
};

// Undo
const undo = () => {
  if (currentStep > 0) {
    setCurrentStep(currentStep - 1);
  }
};

// Redo
const redo = () => {
  if (currentStep < history.length - 1) {
    setCurrentStep(currentStep + 1);
  }
};
```

**4. Prédictibilité et debugging**
```javascript
// Avec mutation - Difficile à debugger
function updateUser(user) {
  user.age++; // Qui a modifié user ? Quand ?
  user.lastUpdated = new Date();
  return user; // Même référence
}

// Avec immutabilité - Clair et traçable
function updateUser(user) {
  return {
    ...user,
    age: user.age + 1,
    lastUpdated: new Date()
  }; // Nouvelle référence, ancien user intact
}

// On peut comparer facilement
const oldUser = { name: 'Jean', age: 30 };
const newUser = updateUser(oldUser);

console.log(oldUser); // { name: 'Jean', age: 30 } - Intact
console.log(newUser); // { name: 'Jean', age: 31, lastUpdated: ... }
```

**Bibliothèques pour l'immutabilité**

**Immer (recommandé)**
```javascript
import produce from 'immer';

const [todos, setTodos] = useState([...]);

// Syntaxe "mutable" qui produit un résultat immutable
setTodos(produce(todos, draft => {
  // On peut "muter" draft, Immer gère l'immutabilité
  draft[0].done = true;
  draft.push({ id: 4, text: 'New', done: false });
}));

// Équivalent immutable manuel :
setTodos([
  { ...todos[0], done: true },
  ...todos.slice(1),
  { id: 4, text: 'New', done: false }
]);
```

**En entretien, souligner :**
- Immutabilité = nouvelle référence pour chaque changement
- React compare les références, pas les valeurs profondes
- Spread operator `...` est la solution la plus courante
- Attention aux objets/arrays imbriqués
- Immer simplifie les cas complexes

---

### Q15 : Expliquez la différence entre map(), filter() et reduce() en React.

**Réponse :**
Ces trois méthodes d'array sont **essentielles en React** pour transformer et afficher des données.

**map() - Transformer et afficher des listes**

```javascript
// Le plus utilisé en React pour le rendering
function UserList() {
  const users = [
    { id: 1, name: 'Alice', age: 25 },
    { id: 2, name: 'Bob', age: 30 },
    { id: 3, name: 'Charlie', age: 35 }
  ];
  
  // map transforme chaque élément en JSX
  return (
    <ul>
      {users.map(user => (
        <li key={user.id}>
          {user.name} - {user.age} ans
        </li>
      ))}
    </ul>
  );
}

// ⚠️ TOUJOURS utiliser une key unique
// ❌ MAUVAIS
{users.map((user, index) => (
  <li key={index}>{user.name}</li> // index comme key = problèmes
))}

// ✅ BON
{users.map(user => (
  <li key={user.id}>{user.name}</li> // ID unique
))}
```

**Cas d'usage map() en React**
```javascript
// 1. Listes simples
{products.map(p => <ProductCard key={p.id} product={p} />)}

// 2. Transformation de données
{prices.map(price => <div key={price.id}>${price.toFixed(2)}</div>)}

// 3. Créer des options de select
<select>
  {countries.map(country => (
    <option key={country.code} value={country.code}>
      {country.name}
    </option>
  ))}
</select>

// 4. Navigation
<nav>
  {menuItems.map(item => (
    <Link key={item.path} to={item.path}>
      {item.label}
    </Link>
  ))}
</nav>
```

**filter() - Afficher conditionnellement**

```javascript
function ActiveUsersList() {
  const users = [
    { id: 1, name: 'Alice', isActive: true },
    { id: 2, name: 'Bob', isActive: false },
    { id: 3, name: 'Charlie', isActive: true }
  ];
  
  // filter sélectionne seulement les utilisateurs actifs
  return (
    <ul>
      {users
        .filter(user => user.isActive)
        .map(user => (
          <li key={user.id}>{user.name}</li>
        ))}
    </ul>
  );
  // Affiche seulement Alice et Charlie
}
```

**Cas d'usage filter() en React**
```javascript
// 1. Recherche/filtrage
function SearchResults({ products, query }) {
  return (
    <div>
      {products
        .filter(p => p.name.toLowerCase().includes(query.toLowerCase()))
        .map(p => <ProductCard key={p.id} product={p} />)}
    </div>
  );
}

// 2. Filtrage par catégorie
function ProductList({ products, selectedCategory }) {
  return (
    <div>
      {products
        .filter(p => !selectedCategory || p.category === selectedCategory)
        .map(p => <ProductCard key={p.id} product={p} />)}
    </div>
  );
}

// 3. Supprimer les éléments null/undefined
function SafeList({ items }) {
  return (
    <ul>
      {items
        .filter(item => item != null) // Supprime null et undefined
        .map((item, index) => <li key={index}>{item}</li>)}
    </ul>
  );
}

// 4. Filtrage complexe
{posts
  .filter(post => post.published && post.views > 1000)
  .map(post => <PostCard key={post.id} post={post} />)}
```

**reduce() - Calculer des valeurs agrégées**

```javascript
function ShoppingCart({ items }) {
  // reduce calcule le total
  const total = items.reduce((sum, item) => sum + item.price * item.quantity, 0);
  
  const itemCount = items.reduce((count, item) => count + item.quantity, 0);
  
  return (
    <div>
      <h2>Panier ({itemCount} articles)</h2>
      <ul>
        {items.map(item => (
          <li key={item.id}>
            {item.name} - {item.quantity} × ${item.price}
          </li>
        ))}
      </ul>
      <p>Total: ${total.toFixed(2)}</p>
    </div>
  );
}
```

**Cas d'usage reduce() en React**
```javascript
// 1. Calcul de totaux
const total = cart.reduce((sum, item) => sum + item.price, 0);

// 2. Grouper par catégorie
function ProductsByCategory({ products }) {
  const grouped = products.reduce((acc, product) => {
    const category = product.category;
    if (!acc[category]) {
      acc[category] = [];
    }
    acc[category].push(product);
    return acc;
  }, {});
  
  return (
    <div>
      {Object.entries(grouped).map(([category, items]) => (
        <div key={category}>
          <h3>{category}</h3>
          <ul>
            {items.map(item => (
              <li key={item.id}>{item.name}</li>
            ))}
          </ul>
        </div>
      ))}
    </div>
  );
}

// 3. Compter les occurrences
const statusCounts = orders.reduce((acc, order) => {
  acc[order.status] = (acc[order.status] || 0) + 1;
  return acc;
}, {});
// { pending: 5, completed: 12, cancelled: 2 }

// 4. Transformer array en objet (indexation)
const usersById = users.reduce((acc, user) => {
  acc[user.id] = user;
  return acc;
}, {});
// { 1: {id: 1, name: 'Alice'}, 2: {id: 2, name: 'Bob'}, ... }

// 5. Aplatir un array imbriqué
const allComments = posts.reduce((acc, post) => {
  return acc.concat(post.comments);
}, []);
```

**Combiner map, filter, reduce**

```javascript
function Dashboard({ transactions }) {
  // Chaîner plusieurs opérations
  const monthlyIncome = transactions
    .filter(t => t.type === 'income') // Seulement les revenus
    .filter(t => isThisMonth(t.date))  // Ce mois-ci
    .map(t => t.amount)                // Extraire les montants
    .reduce((sum, amount) => sum + amount, 0); // Sommer
  
  return (
    <div>
      <h2>Revenus du mois</h2>
      <p>${monthlyIncome.toFixed(2)}</p>
    </div>
  );
}

// Filtrage et groupement
function TaskList({ tasks }) {
  const tasksByStatus = tasks
    .filter(task => !task.archived)
    .reduce((acc, task) => {
      if (!acc[task.status]) acc[task.status] = [];
      acc[task.status].push(task);
      return acc;
    }, {});
  
  return (
    <div>
      {['todo', 'in-progress', 'done'].map(status => (
        <div key={status}>
          <h3>{status}</h3>
          {(tasksByStatus[status] || []).map(task => (
            <TaskCard key={task.id} task={task} />
          ))}
        </div>
      ))}
    </div>
  );
}
```

**Performance : Mémorisation avec useMemo**

```javascript
function ExpensiveList({ items, filterQuery }) {
  // ❌ Sans useMemo - Refiltré à chaque render
  const filteredItems = items.filter(item => 
    item.name.includes(filterQuery)
  );
  
  // ✅ Avec useMemo - Recalculé seulement si items ou filterQuery change
  const filteredItemsMemo = useMemo(() => {
    console.log('Filtering...'); // Debug
    return items.filter(item => item.name.includes(filterQuery));
  }, [items, filterQuery]);
  
  return (
    <ul>
      {filteredItemsMemo.map(item => (
        <li key={item.id}>{item.name}</li>
      ))}
    </ul>
  );
}
```

**Comparaison rapide**

| Méthode | Retourne | Use case React | Exemple |
|---------|----------|----------------|---------|
| **map()** | Nouveau array (même taille) | Rendering de listes | `{users.map(u => <User />)}` |
| **filter()** | Nouveau array (≤ taille) | Affichage conditionnel | `{users.filter(u => u.active)}` |
| **reduce()** | Une seule valeur | Calculs, agrégations | `total = items.reduce((s, i) => s + i.price, 0)` |

**En entretien, souligner :**
- map() = indispensable pour rendering de listes
- filter() + map() = pattern très courant
- reduce() = pour calculs et transformations complexes
- useMemo() pour optimiser les calculs coûteux
- Toujours utiliser des keys uniques avec map()

---