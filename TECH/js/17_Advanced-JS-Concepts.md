## [**..**](./00_index.md)

# Chapitre 17 : Concepts Avancés JavaScript - Résumé Détaillé

## 17.1 Spread et Rest Operators

Les opérateurs `...` (trois points) ont deux usages différents selon le contexte : **spread** (étaler) et **rest** (rassembler).

### Spread Operator avec Arrays

```javascript
// SPREAD avec ARRAYS - Étaler les éléments

const arr1 = [1, 2, 3];
const arr2 = [4, 5, 6];

// Combiner arrays
const combined = [...arr1, ...arr2];
console.log(combined); // [1, 2, 3, 4, 5, 6]

// Ajouter éléments
const withExtra = [...arr1, 7, 8, 9];
console.log(withExtra); // [1, 2, 3, 7, 8, 9]

// Insérer au milieu
const middle = [0, ...arr1, 3.5, ...arr2];
console.log(middle); // [0, 1, 2, 3, 3.5, 4, 5, 6]

// Copier un array (shallow copy)
const copy = [...arr1];
console.log(copy); // [1, 2, 3]
console.log(copy === arr1); // false (nouvelle référence)

// Modifier la copie n'affecte pas l'original
copy.push(4);
console.log(arr1); // [1, 2, 3] (inchangé)
console.log(copy); // [1, 2, 3, 4]

// Spread de string (iterable)
const str = 'hello';
const chars = [...str];
console.log(chars); // ['h', 'e', 'l', 'l', 'o']

// Spread de Set
const set = new Set([1, 2, 3, 2, 1]);
const arrFromSet = [...set];
console.log(arrFromSet); // [1, 2, 3] (doublons supprimés)

// Math.max avec array
const numbers = [10, 5, 20, 15, 8];
const max = Math.max(...numbers);
console.log(max); // 20

const min = Math.min(...numbers);
console.log(min); // 5

// Push multiple éléments
const arr = [1, 2, 3];
const toAdd = [4, 5, 6];
arr.push(...toAdd);
console.log(arr); // [1, 2, 3, 4, 5, 6]

// Dédoublonner un array
const duplicates = [1, 2, 2, 3, 3, 3, 4];
const unique = [...new Set(duplicates)];
console.log(unique); // [1, 2, 3, 4]

// Fusionner et trier
const a = [3, 1, 5];
const b = [4, 2, 6];
const merged = [...a, ...b].sort((x, y) => x - y);
console.log(merged); // [1, 2, 3, 4, 5, 6]

// Spread avec destructuring
const [first, ...rest] = [1, 2, 3, 4, 5];
console.log(first); // 1
console.log(rest);  // [2, 3, 4, 5]
```

### Spread Operator avec Objects

```javascript
// SPREAD avec OBJECTS - Étaler les propriétés

const obj1 = { a: 1, b: 2 };
const obj2 = { c: 3, d: 4 };

// Combiner objets
const combined = { ...obj1, ...obj2 };
console.log(combined); // { a: 1, b: 2, c: 3, d: 4 }

// Ajouter propriétés
const withExtra = { ...obj1, e: 5, f: 6 };
console.log(withExtra); // { a: 1, b: 2, e: 5, f: 6 }

// Copier un objet (shallow copy)
const copy = { ...obj1 };
console.log(copy); // { a: 1, b: 2 }
console.log(copy === obj1); // false

// Override propriétés
const obj = { x: 1, y: 2 };
const updated = { ...obj, y: 10, z: 3 };
console.log(updated); // { x: 1, y: 10, z: 3 }

// Ordre important!
const base = { a: 1, b: 2 };
const override1 = { ...base, a: 10 }; // a est overridé
console.log(override1); // { a: 10, b: 2 }

const override2 = { a: 10, ...base }; // a revient à 1
console.log(override2); // { a: 1, b: 2 }

// Shallow copy (attention nested objects)
const nested = {
    name: 'John',
    address: { city: 'Paris', country: 'France' }
};

const shallowCopy = { ...nested };
shallowCopy.address.city = 'London'; // Modifie l'original!

console.log(nested.address.city); // "London" (modifié!)
console.log(shallowCopy.address === nested.address); // true (même référence)

// Deep copy (solution)
const deepCopy = JSON.parse(JSON.stringify(nested));
// Ou utiliser structuredClone (moderne)
const deepCopy2 = structuredClone(nested);

// Merge avec valeurs par défaut
const defaultConfig = {
    timeout: 5000,
    retries: 3,
    cache: true
};

const userConfig = {
    timeout: 10000
};

const finalConfig = { ...defaultConfig, ...userConfig };
console.log(finalConfig);
// { timeout: 10000, retries: 3, cache: true }

// Supprimer propriété (avec destructuring)
const user = { id: 1, name: 'John', password: 'secret' };
const { password, ...publicUser } = user;
console.log(publicUser); // { id: 1, name: 'John' }

// Spread avec computed property names
const key = 'dynamic';
const obj3 = { ...obj1, [key]: 'value' };
console.log(obj3); // { a: 1, b: 2, dynamic: 'value' }

// Spread multiple objects
const result = {
    ...{ a: 1 },
    ...{ b: 2 },
    ...{ c: 3 },
    d: 4
};
console.log(result); // { a: 1, b: 2, c: 3, d: 4 }

// Conditional spread
const condition = true;
const conditionalObj = {
    base: 'value',
    ...(condition && { extra: 'conditionally added' })
};
console.log(conditionalObj);
// condition true: { base: 'value', extra: 'conditionally added' }
// condition false: { base: 'value' }

// Spread avec arrays dans objet
const data = {
    users: [1, 2, 3],
    posts: [4, 5, 6]
};

const moreData = {
    ...data,
    users: [...data.users, 4, 5], // Ajouter aux users
    comments: [7, 8, 9]
};
console.log(moreData);
// { users: [1, 2, 3, 4, 5], posts: [4, 5, 6], comments: [7, 8, 9] }
```

### Rest Parameters dans Fonctions

```javascript
// REST PARAMETERS - Rassembler arguments dans array

// Rest parameter doit être le DERNIER
function sum(...numbers) {
    return numbers.reduce((total, num) => total + num, 0);
}

console.log(sum(1, 2, 3));        // 6
console.log(sum(1, 2, 3, 4, 5));  // 15

// Combiner paramètres normaux et rest
function multiply(multiplier, ...numbers) {
    return numbers.map(num => num * multiplier);
}

console.log(multiply(2, 1, 2, 3)); // [2, 4, 6]

// Plusieurs paramètres avant rest
function format(template, separator, ...values) {
    return values.join(separator);
}

console.log(format('list', ', ', 'a', 'b', 'c')); // "a, b, c"

// Rest avec destructuring
function processUser({ name, age, ...otherInfo }) {
    console.log('Name:', name);
    console.log('Age:', age);
    console.log('Other:', otherInfo);
}

processUser({
    name: 'John',
    age: 30,
    email: 'john@example.com',
    city: 'Paris'
});
// Name: John
// Age: 30
// Other: { email: 'john@example.com', city: 'Paris' }

// Rest dans array destructuring
const [first, second, ...remaining] = [1, 2, 3, 4, 5];
console.log(first);     // 1
console.log(second);    // 2
console.log(remaining); // [3, 4, 5]

// Skip éléments avec rest
const [, , ...others] = [1, 2, 3, 4, 5];
console.log(others); // [3, 4, 5]

// ⚠️ Rest doit être le dernier
// function bad(...rest, last) { } // ❌ Syntax error
function good(first, ...rest) { }  // ✅

// Différence avec arguments object (old way)
function oldWay() {
    // arguments n'est pas un vrai array
    console.log(arguments); // [1, 2, 3] (array-like)
    // arguments.map() // ❌ Error: map is not a function
    
    // Convertir en array
    const args = Array.from(arguments);
    console.log(args.map(x => x * 2)); // ✅
}

function newWay(...args) {
    // args est un vrai array
    console.log(args.map(x => x * 2)); // ✅ Fonctionne directement
}

oldWay(1, 2, 3);
newWay(1, 2, 3);

// Exemples pratiques

// 1. Logger flexible
function log(level, ...messages) {
    const timestamp = new Date().toISOString();
    console.log(`[${timestamp}] [${level}]`, ...messages);
}

log('INFO', 'Server started', 'Port 3000');
log('ERROR', 'Connection failed', 'Retrying...');

// 2. Créer objet avec propriétés dynamiques
function createUser(name, email, ...roles) {
    return {
        name,
        email,
        roles,
        createdAt: new Date()
    };
}

const user = createUser('John', 'john@example.com', 'admin', 'editor');
console.log(user);
// { name: 'John', email: '...', roles: ['admin', 'editor'], createdAt: ... }

// 3. Wrapper functions
function withLogging(fn) {
    return function(...args) {
        console.log('Calling with:', args);
        const result = fn(...args);
        console.log('Result:', result);
        return result;
    };
}

const add = (a, b) => a + b;
const loggedAdd = withLogging(add);
loggedAdd(2, 3);
// Calling with: [2, 3]
// Result: 5

// 4. Merge multiple arrays
function mergeArrays(...arrays) {
    return arrays.flat();
}

const merged = mergeArrays([1, 2], [3, 4], [5, 6]);
console.log(merged); // [1, 2, 3, 4, 5, 6]

// 5. Conditional parameters
function apiRequest(endpoint, method = 'GET', ...options) {
    const config = {
        url: endpoint,
        method,
        ...Object.assign({}, ...options)
    };
    
    return config;
}

const config = apiRequest('/users', 'POST', { body: { name: 'John' } }, { headers: { 'Content-Type': 'application/json' } });
console.log(config);
```

---

## 17.2 Short-Circuit Evaluation

Les opérateurs logiques `&&`, `||`, et `??` peuvent être utilisés pour des évaluations court-circuitées.

### && (AND) et || (OR)

```javascript
// && (AND) - Retourne première valeur falsy OU dernière valeur

// Si première est falsy, retourne cette valeur (court-circuite)
console.log(false && 'hello');     // false
console.log(0 && 'hello');         // 0
console.log('' && 'hello');        // ""
console.log(null && 'hello');      // null
console.log(undefined && 'hello'); // undefined

// Si toutes truthy, retourne la dernière
console.log(true && 'hello');      // "hello"
console.log(1 && 2 && 3);          // 3
console.log('a' && 'b' && 'c');    // "c"

// || (OR) - Retourne première valeur truthy OU dernière valeur

// Si première est truthy, retourne cette valeur (court-circuite)
console.log(true || 'hello');      // true
console.log(1 || 'hello');         // 1
console.log('first' || 'second');  // "first"

// Si toutes falsy, retourne la dernière
console.log(false || 0 || '');     // ""
console.log(null || undefined);    // undefined

// Si au moins une truthy, retourne la première truthy
console.log(0 || false || 'hello'); // "hello"
console.log(null || 42 || 'world'); // 42

// VALEURS FALSY en JavaScript:
// false, 0, -0, 0n, "", '', ``, null, undefined, NaN

// EXEMPLES PRATIQUES

// 1. Valeur par défaut avec ||
function greet(name) {
    name = name || 'Guest';
    return `Hello, ${name}`;
}

console.log(greet('John'));  // "Hello, John"
console.log(greet());        // "Hello, Guest"
console.log(greet(''));      // "Hello, Guest"

// 2. Conditional execution avec &&
const user = { name: 'John', isAdmin: true };

// Exécuter seulement si condition vraie
user.isAdmin && console.log('Admin access granted');

// Équivalent à:
if (user.isAdmin) {
    console.log('Admin access granted');
}

// 3. Chaîner conditions
const isValid = user && user.isActive && user.hasPermission;

// 4. Render conditionnel (React-like)
const showButton = isLoggedIn && hasPermission && <button>Click</button>;

// 5. Callback optionnel
function doSomething(callback) {
    // ...code...
    callback && callback(); // Appelle seulement si callback existe
}

// 6. Cache ou calculer
let cachedValue;
function getValue() {
    return cachedValue || (cachedValue = expensiveCalculation());
}

// 7. Premier non-null
const result = value1 || value2 || value3 || 'default';

// 8. Guard clauses
function processUser(user) {
    // Early return si user n'existe pas
    if (!user) return;
    // Ou: user || return; (pas valide en JS)
    
    // Process user...
}

// 9. Propriétés imbriquées
const city = user && user.address && user.address.city;

// ⚠️ PIÈGE avec ||

// Problème: 0 est falsy mais peut être une valeur valide
function setCount(count) {
    count = count || 10; // ❌ Si count = 0, devient 10!
    return count;
}

console.log(setCount(5));  // 5 ✅
console.log(setCount(0));  // 10 ❌ (devrait être 0)
console.log(setCount());   // 10 ✅

// Solution: utiliser ?? (voir section suivante)

// Chaîner && et ||
const result = (condition1 && value1) || (condition2 && value2) || defaultValue;

// Exemple complexe
const discount = (
    isPremium && largeOrder && 0.2
) || (
    isRegular && mediumOrder && 0.1
) || 0;
```

### Nullish Coalescing (??)

```javascript
// ?? (NULLISH COALESCING) - Retourne premier NON-nullish

// Nullish = null ou undefined UNIQUEMENT
// Différent de falsy!

// Retourne valeur de droite si gauche est null/undefined
console.log(null ?? 'default');      // "default"
console.log(undefined ?? 'default'); // "default"

// Retourne valeur de gauche si pas null/undefined
console.log(0 ?? 'default');         // 0 ✅
console.log(false ?? 'default');     // false ✅
console.log('' ?? 'default');        // "" ✅
console.log(NaN ?? 'default');       // NaN ✅

// DIFFÉRENCE avec ||

console.log(0 || 10);      // 10 (0 est falsy)
console.log(0 ?? 10);      // 0 (0 n'est pas nullish)

console.log(false || true);  // true (false est falsy)
console.log(false ?? true);  // false (false n'est pas nullish)

console.log('' || 'hello');  // "hello" ('' est falsy)
console.log('' ?? 'hello');  // "" ('' n'est pas nullish)

console.log(null || 'hello');  // "hello"
console.log(null ?? 'hello');  // "hello" (même résultat pour null/undefined)

// EXEMPLES PRATIQUES

// 1. Valeur par défaut pour 0 ou false
function setConfig(options) {
    const timeout = options.timeout ?? 5000; // 0 est valide
    const retry = options.retry ?? 3;        // 0 est valide
    const cache = options.cache ?? true;     // false est valide
    
    return { timeout, retry, cache };
}

console.log(setConfig({ timeout: 0, cache: false }));
// { timeout: 0, retry: 3, cache: false } ✅

// Avec || (incorrect):
// timeout: 0 || 5000 → 5000 ❌
// cache: false || true → true ❌

// 2. Propriétés optionnelles
const user = {
    name: 'John',
    age: 0,        // 0 est un âge valide
    isActive: false // false est un état valide
};

const age = user.age ?? 18;
const active = user.isActive ?? true;

console.log(age);    // 0 ✅
console.log(active); // false ✅

// 3. Paramètres par défaut
function createUser(name, age, role) {
    return {
        name: name ?? 'Anonymous',
        age: age ?? 0,      // 0 est valide
        role: role ?? 'user'
    };
}

console.log(createUser('John', 0, 'admin'));
// { name: 'John', age: 0, role: 'admin' } ✅

// 4. API response avec valeurs nullish
const apiResponse = {
    count: 0,      // 0 résultats est valide
    data: [],      // Array vide est valide
    error: null    // Pas d'erreur
};

const count = apiResponse.count ?? 10;
const hasError = apiResponse.error ?? 'Unknown error';

console.log(count);    // 0 ✅
console.log(hasError); // "Unknown error" ✅

// 5. Configuration avec override
const defaultConfig = {
    timeout: 5000,
    retries: 3,
    cache: true
};

const userConfig = {
    timeout: 0,      // Vouloir désactiver timeout
    cache: false     // Vouloir désactiver cache
};

const config = {
    timeout: userConfig.timeout ?? defaultConfig.timeout,
    retries: userConfig.retries ?? defaultConfig.retries,
    cache: userConfig.cache ?? defaultConfig.cache
};

console.log(config);
// { timeout: 0, retries: 3, cache: false } ✅

// 6. Chaîner ??
const value = option1 ?? option2 ?? option3 ?? 'default';

// 7. Avec optional chaining
const city = user?.address?.city ?? 'Unknown';

// ⚠️ Ne peut pas mélanger ?? avec && ou || sans parenthèses
// const x = a && b ?? c; // ❌ Syntax error
const x = (a && b) ?? c;  // ✅
const y = a && (b ?? c);  // ✅
```

### Optional Chaining (?.)

```javascript
// ?. (OPTIONAL CHAINING) - Accès sûr aux propriétés

// Sans optional chaining
const user = null;
// const city = user.address.city; // ❌ Error: Cannot read property 'address' of null

// Avec vérifications manuelles
const city = user && user.address && user.address.city;

// Avec optional chaining
const city = user?.address?.city; // ✅ undefined (pas d'erreur)

// SYNTAXES

// 1. Propriété: obj?.prop
const name = user?.name;

// 2. Propriété calculée: obj?.[expr]
const key = 'email';
const email = user?.[key];

// 3. Appel de fonction: obj?.method()
const result = user?.getData?.();

// 4. Array: arr?.[index]
const first = users?.[0];

// EXEMPLES DÉTAILLÉS

// Objet imbriqué
const user = {
    name: 'John',
    address: {
        city: 'Paris',
        zipCode: '75001'
    }
};

console.log(user?.address?.city);    // "Paris"
console.log(user?.address?.country); // undefined
console.log(user?.phone?.number);    // undefined (pas d'erreur)

// Sans user
const noUser = null;
console.log(noUser?.address?.city); // undefined

// Méthode optionnelle
const obj = {
    method() {
        return 'result';
    }
};

console.log(obj.method?.());      // "result"
console.log(obj.missing?.());     // undefined
console.log(obj.method?.('arg')); // "result"

// Array avec optional chaining
const users = [
    { name: 'John' },
    { name: 'Jane' }
];

console.log(users?.[0]?.name);  // "John"
console.log(users?.[10]?.name); // undefined

const noArray = null;
console.log(noArray?.[0]); // undefined

// Callback optionnel
function processData(data, callback) {
    // Process data...
    callback?.(data); // Appelle seulement si callback existe
}

processData({ x: 1 }, console.log); // Appelle console.log
processData({ x: 1 });              // Ne fait rien (pas d'erreur)

// API response
const response = await fetch('/api/user');
const data = await response.json();

const userName = data?.user?.name ?? 'Anonymous';
const email = data?.user?.contact?.email ?? 'No email';

// Avec arrays imbriqués
const data = {
    users: [
        {
            name: 'John',
            posts: [
                { title: 'Post 1' },
                { title: 'Post 2' }
            ]
        }
    ]
};

const firstPostTitle = data?.users?.[0]?.posts?.[0]?.title;
console.log(firstPostTitle); // "Post 1"

// Combination avec ?? (nullish coalescing)
const city = user?.address?.city ?? 'Unknown city';
const phone = user?.contact?.phone ?? 'No phone';

// Delete avec optional chaining
delete obj?.property; // Supprime seulement si obj existe

// Avec fonctions
const result = user?.getName?.() ?? 'No name';

// ⚠️ ATTENTION: Court-circuite toute la chaîne
const value = nullValue?.property.method(); // ✅ Pas d'erreur
// Si nullValue est null/undefined, rien après ?. n'est évalué

// Ne fonctionne PAS pour l'affectation
// obj?.prop = value; // ❌ Syntax error

// EXEMPLES REACT

/*
// Access nested state
const userName = state?.user?.profile?.name;

// Optional callback
props?.onSubmit?.(data);

// API data
const posts = response?.data?.posts?.map(post => ({
    id: post?.id,
    title: post?.title ?? 'Untitled'
}));

// Event handler
const handleClick = () => {
    ref?.current?.focus?.();
};
*/

// Combiner tout: ?., ??, &&, ||

const result = (
    config?.api?.url ??
    process.env.API_URL ??
    'http://localhost:3000'
);

const isAdmin = (
    user?.roles?.includes('admin') &&
    user?.isActive !== false
);

const displayName = (
    user?.profile?.displayName ||
    user?.username ||
    'Anonymous'
);
```

---

## 17.3 Error Handling

### try...catch...finally

```javascript
// TRY...CATCH...FINALLY - Gestion d'erreurs

// Syntaxe de base
try {
    // Code qui peut lancer une erreur
    const result = riskyOperation();
} catch (error) {
    // Gère l'erreur
    console.error('Error occurred:', error);
} finally {
    // Toujours exécuté (optionnel)
    console.log('Cleanup');
}

// EXEMPLES

// 1. Parse JSON
function safeJSONParse(jsonString) {
    try {
        return JSON.parse(jsonString);
    } catch (error) {
        console.error('Invalid JSON:', error.message);
        return null;
    }
}

console.log(safeJSONParse('{"name":"John"}'));  // { name: 'John' }
console.log(safeJSONParse('invalid'));          // null

// 2. Fetch avec error handling
async function fetchData(url) {
    try {
        const response = await fetch(url);
        
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        const data = await response.json();
        return { success: true, data };
        
    } catch (error) {
        console.error('Fetch failed:', error);
        return { success: false, error: error.message };
    }
}

// 3. Finally pour cleanup
function readFile(filename) {
    let file;
    
    try {
        file = openFile(filename);
        const content = file.read();
        return content;
        
    } catch (error) {
        console.error('Error reading file:', error);
        return null;
        
    } finally {
        // Toujours exécuté, même si return dans try/catch
        if (file) {
            file.close(); // Cleanup
        }
    }
}

// 4. Multiple catch (différents types d'erreurs)
try {
    // Code
} catch (error) {
    if (error instanceof TypeError) {
        console.error('Type error:', error);
    } else if (error instanceof RangeError) {
        console.error('Range error:', error);
    } else {
        console.error('Other error:', error);
    }
}

// 5. Re-throw error
function processData(data) {
    try {
        validateData(data);
        return transformData(data);
        
    } catch (error) {
        console.error('Processing failed:', error);
        // Re-throw pour laisser l'appelant gérer
        throw error;
    }
}

// 6. Async/await avec try/catch
async function loadUser(id) {
    try {
        const user = await fetchUser(id);
        const posts = await fetchUserPosts(id);
        
        return { user, posts };
        
    } catch (error) {
        console.error('Failed to load user:', error);
        throw new Error(`User ${id} not found`);
    }
}

// 7. Try/catch dans loop
const urls = ['/api/users', '/api/posts', '/api/comments'];

for (const url of urls) {
    try {
        const data = await fetch(url).then(r => r.json());
        console.log('Loaded:', url, data);
    } catch (error) {
        console.error(`Failed to load ${url}:`, error);
        // Continue avec les autres URLs
    }
}

// 8. Finally sans catch
try {
    return performOperation();
} finally {
    cleanup(); // Exécuté même si return
}

// 9. Nested try/catch
try {
    const data = loadData();
    
    try {
        processData(data);
    } catch (processingError) {
        console.error('Processing error:', processingError);
    }
    
} catch (loadingError) {
    console.error('Loading error:', loadingError);
}

// PROPRIÉTÉS D'ERROR

try {
    throw new Error('Something went wrong');
} catch (error) {
    console.log(error.name);    // "Error"
    console.log(error.message); // "Something went wrong"
    console.log(error.stack);   // Stack trace
}
```

### Throwing Errors

```javascript
// THROW - Lancer des erreurs

// Lancer Error basique
throw new Error('Something went wrong');

// Lancer avec message
function divide(a, b) {
    if (b === 0) {
        throw new Error('Division by zero');
    }
    return a / b;
}

try {
    console.log(divide(10, 0));
} catch (error) {
    console.error(error.message); // "Division by zero"
}

// Types d'erreurs built-in

// 1. Error (générique)
throw new Error('Generic error');

// 2. TypeError
function requireString(value) {
    if (typeof value !== 'string') {
        throw new TypeError('Expected a string');
    }
}

// 3. RangeError
function setAge(age) {
    if (age < 0 || age > 150) {
        throw new RangeError('Age must be between 0 and 150');
    }
}

// 4. ReferenceError
// Lancé automatiquement pour variables non définies
// console.log(undefinedVar); // ReferenceError

// 5. SyntaxError
// eval('invalid syntax'); // SyntaxError

// Lancer valeurs non-Error (possible mais pas recommandé)
```javascript
throw 'Error string';  // ❌ Pas recommandé
throw 42;              // ❌ Pas recommandé
throw { code: 500 };   // ❌ Pas recommandé

// ✅ Toujours utiliser Error objects
throw new Error('Message'); // ✅ Recommandé

// VALIDATION avec throw
function validateEmail(email) {
    if (!email) {
        throw new Error('Email is required');
    }
    
    if (!email.includes('@')) {
        throw new Error('Invalid email format');
    }
    
    return true;
}

function registerUser(userData) {
    try {
        validateEmail(userData.email);
        // Continue registration...
        return { success: true };
    } catch (error) {
        return { success: false, error: error.message };
    }
}

// EARLY RETURN pattern
function processOrder(order) {
    if (!order) {
        throw new Error('Order is required');
    }
    
    if (!order.items || order.items.length === 0) {
        throw new Error('Order must have items');
    }
    
    if (order.total < 0) {
        throw new Error('Order total cannot be negative');
    }
    
    // Process order...
    return { success: true };
}
```

### Custom Errors

```javascript
// CUSTOM ERROR CLASSES - Créer erreurs personnalisées

// Classe de base custom error
class CustomError extends Error {
    constructor(message) {
        super(message);
        this.name = this.constructor.name; // Nom de la classe
        
        // Capture stack trace (V8 only)
        if (Error.captureStackTrace) {
            Error.captureStackTrace(this, this.constructor);
        }
    }
}

// Erreur de validation
class ValidationError extends CustomError {
    constructor(message, field) {
        super(message);
        this.field = field;
        this.code = 'VALIDATION_ERROR';
    }
}

// Erreur d'authentification
class AuthenticationError extends CustomError {
    constructor(message) {
        super(message);
        this.code = 'AUTH_ERROR';
        this.statusCode = 401;
    }
}

// Erreur de ressource non trouvée
class NotFoundError extends CustomError {
    constructor(resource, id) {
        super(`${resource} with id ${id} not found`);
        this.resource = resource;
        this.id = id;
        this.statusCode = 404;
    }
}

// Erreur réseau
class NetworkError extends CustomError {
    constructor(message, url) {
        super(message);
        this.url = url;
        this.code = 'NETWORK_ERROR';
    }
}

// UTILISATION

function validateUser(user) {
    if (!user.email) {
        throw new ValidationError('Email is required', 'email');
    }
    
    if (!user.password) {
        throw new ValidationError('Password is required', 'password');
    }
    
    if (user.password.length < 8) {
        throw new ValidationError('Password must be at least 8 characters', 'password');
    }
}

// Gérer différents types d'erreurs
async function login(credentials) {
    try {
        validateUser(credentials);
        
        const response = await fetch('/api/login', {
            method: 'POST',
            body: JSON.stringify(credentials)
        });
        
        if (response.status === 401) {
            throw new AuthenticationError('Invalid credentials');
        }
        
        if (response.status === 404) {
            throw new NotFoundError('User', credentials.email);
        }
        
        if (!response.ok) {
            throw new NetworkError('Login failed', '/api/login');
        }
        
        return await response.json();
        
    } catch (error) {
        // Gérer selon le type
        if (error instanceof ValidationError) {
            console.error(`Validation error in field ${error.field}:`, error.message);
            return { success: false, field: error.field, message: error.message };
        }
        
        if (error instanceof AuthenticationError) {
            console.error('Authentication failed:', error.message);
            return { success: false, message: 'Invalid credentials' };
        }
        
        if (error instanceof NotFoundError) {
            console.error('User not found:', error.message);
            return { success: false, message: 'User not found' };
        }
        
        if (error instanceof NetworkError) {
            console.error('Network error:', error.message);
            return { success: false, message: 'Network error, please try again' };
        }
        
        // Erreur inconnue
        console.error('Unexpected error:', error);
        return { success: false, message: 'An unexpected error occurred' };
    }
}

// Error avec données supplémentaires
class ApiError extends CustomError {
    constructor(message, statusCode, details = {}) {
        super(message);
        this.statusCode = statusCode;
        this.details = details;
        this.timestamp = new Date();
    }
    
    toJSON() {
        return {
            name: this.name,
            message: this.message,
            statusCode: this.statusCode,
            details: this.details,
            timestamp: this.timestamp
        };
    }
}

// Utilisation
try {
    throw new ApiError('Bad Request', 400, {
        field: 'email',
        reason: 'Invalid format'
    });
} catch (error) {
    console.log(error.toJSON());
    // {
    //   name: 'ApiError',
    //   message: 'Bad Request',
    //   statusCode: 400,
    //   details: { field: 'email', reason: 'Invalid format' },
    //   timestamp: ...
    // }
}

// Error Factory
class ErrorFactory {
    static validation(message, field) {
        return new ValidationError(message, field);
    }
    
    static notFound(resource, id) {
        return new NotFoundError(resource, id);
    }
    
    static authentication(message) {
        return new AuthenticationError(message);
    }
}

// Utilisation
throw ErrorFactory.validation('Invalid email', 'email');
throw ErrorFactory.notFound('User', 123);

// EXEMPLE REACT

/*
class ErrorBoundary extends React.Component {
    constructor(props) {
        super(props);
        this.state = { hasError: false, error: null };
    }
    
    static getDerivedStateFromError(error) {
        return { hasError: true, error };
    }
    
    componentDidCatch(error, errorInfo) {
        if (error instanceof ValidationError) {
            console.log('Validation error:', error.field, error.message);
        } else if (error instanceof NetworkError) {
            console.log('Network error:', error.url);
        } else {
            console.error('Unexpected error:', error, errorInfo);
        }
    }
    
    render() {
        if (this.state.hasError) {
            return <h1>Something went wrong.</h1>;
        }
        
        return this.props.children;
    }
}
*/
```

---

## 17.4 Strict Mode

```javascript
// STRICT MODE - Mode strict JavaScript

// Activer strict mode pour tout le script
'use strict';

// Ou pour une fonction spécifique
function strictFunction() {
    'use strict';
    // Code en strict mode
}

// DIFFÉRENCES EN STRICT MODE

// 1. Variables doivent être déclarées
// Sans strict mode
function loose() {
    x = 10; // Crée variable globale (mauvais!)
}

// Avec strict mode
function strict() {
    'use strict';
    // x = 10; // ❌ ReferenceError: x is not defined
    const x = 10; // ✅ Doit déclarer
}

// 2. this = undefined dans fonctions
// Sans strict mode
function looseThis() {
    console.log(this); // window (navigateur) ou global (Node.js)
}

// Avec strict mode
function strictThis() {
    'use strict';
    console.log(this); // undefined
}

looseThis();  // window/global
strictThis(); // undefined

// 3. Pas de delete sur variables
'use strict';

const obj = { x: 1 };
delete obj.x; // ✅ OK (propriété d'objet)

const y = 2;
// delete y; // ❌ SyntaxError: Delete of an unqualified identifier

// 4. Pas de noms de paramètres dupliqués
// Sans strict mode
function loose(a, a, b) { // OK mais confus
    return a + b;
}

// Avec strict mode
/*
function strict(a, a, b) { // ❌ SyntaxError
    'use strict';
    return a + b;
}
*/

// 5. Octals interdits
// Sans strict mode
const num1 = 010; // 8 en décimal (octal)

// Avec strict mode
'use strict';
// const num2 = 010; // ❌ SyntaxError
const num2 = 0o10;  // ✅ Notation ES6 explicite

// 6. with statement interdit
// Sans strict mode
with (Math) {
    const x = cos(3 * PI); // Confus!
}

// Avec strict mode
'use strict';
/*
with (Math) { // ❌ SyntaxError
    const x = cos(3 * PI);
}
*/

// 7. eval a son propre scope
'use strict';

eval('var x = 10');
// console.log(x); // ❌ ReferenceError (x est local à eval)

// 8. arguments et paramètres découplés
function loose(a) {
    a = 2;
    console.log(arguments[0]); // 2 (lié au paramètre)
}

function strict(a) {
    'use strict';
    a = 2;
    console.log(arguments[0]); // 1 (pas lié)
}

loose(1);  // 2
strict(1); // 1

// 9. Mots réservés pour le futur
'use strict';

// Ces mots ne peuvent pas être utilisés comme noms de variables:
// implements, interface, let, package, private, protected, public, static, yield

/*
const implements = 1; // ❌ SyntaxError
const interface = 2;  // ❌ SyntaxError
const private = 3;    // ❌ SyntaxError
*/

// 10. this dans méthodes
'use strict';

const obj = {
    name: 'John',
    greet() {
        console.log(this.name); // "John" (this = obj)
    }
};

obj.greet(); // "John"

const greet = obj.greet;
greet(); // undefined (this = undefined en strict mode)

// MODULES ES6 = TOUJOURS STRICT MODE

// module.js (automatiquement en strict mode)
export function test() {
    // Pas besoin de 'use strict'
    // Déjà en strict mode!
    console.log(this); // undefined
}

// AVANTAGES DU STRICT MODE

// ✅ Détecte erreurs communes
// ✅ Empêche variables globales accidentelles
// ✅ Code plus sûr et prévisible
// ✅ Facilite optimisations moteur JS
// ✅ Prépare pour futures versions JS
// ✅ Obligatoire dans classes ES6

// Classes ES6 = automatiquement strict
class MyClass {
    constructor() {
        // Strict mode automatique
        // x = 10; // ❌ Error
        console.log(this); // Instance de MyClass
    }
}

// RECOMMANDATIONS

// ✅ Toujours utiliser strict mode
// ✅ Utiliser modules ES6 (strict par défaut)
// ✅ Utiliser linter (ESLint) pour détecter problèmes
// ✅ Déclarer toutes les variables
// ✅ Éviter 'with' et 'eval'
```

---

## 17.5 JSON

### JSON.stringify()

```javascript
// JSON.stringify() - Convertir objet JavaScript en JSON string

// Syntaxe de base
const obj = { name: 'John', age: 30 };
const json = JSON.stringify(obj);
console.log(json); // '{"name":"John","age":30}'

// PARAMÈTRES: JSON.stringify(value, replacer, space)

// 1. VALUE - Valeur à convertir

// Objects
const user = {
    name: 'John',
    age: 30,
    city: 'Paris'
};
console.log(JSON.stringify(user));
// '{"name":"John","age":30,"city":"Paris"}'

// Arrays
const arr = [1, 2, 3, 'four', true];
console.log(JSON.stringify(arr));
// '[1,2,3,"four",true]'

// Types supportés
console.log(JSON.stringify('hello'));   // '"hello"'
console.log(JSON.stringify(123));       // '123'
console.log(JSON.stringify(true));      // 'true'
console.log(JSON.stringify(null));      // 'null'

// Types ignorés
console.log(JSON.stringify(undefined)); // undefined
console.log(JSON.stringify(function(){})); // undefined
console.log(JSON.stringify(Symbol('x'))); // undefined

// Dans objects, propriétés ignorées
const obj2 = {
    name: 'John',
    fn: function() {},      // Ignoré
    sym: Symbol('x'),       // Ignoré
    undef: undefined        // Ignoré
};
console.log(JSON.stringify(obj2));
// '{"name":"John"}'

// 2. REPLACER - Filtrer/transformer propriétés

// Replacer function
const data = {
    name: 'John',
    password: 'secret',
    age: 30
};

const filtered = JSON.stringify(data, (key, value) => {
    // Filtrer password
    if (key === 'password') {
        return undefined; // Exclure
    }
    return value;
});
console.log(filtered); // '{"name":"John","age":30}'

// Replacer array (whitelist)
const whitelisted = JSON.stringify(data, ['name', 'age']);
console.log(whitelisted); // '{"name":"John","age":30}'

// Transformer valeurs
const transformed = JSON.stringify(data, (key, value) => {
    if (typeof value === 'string') {
        return value.toUpperCase();
    }
    return value;
});
console.log(transformed); // '{"name":"JOHN","password":"SECRET","age":30}'

// 3. SPACE - Indentation (pretty print)

// Number (espaces)
console.log(JSON.stringify(user, null, 2));
/*
{
  "name": "John",
  "age": 30,
  "city": "Paris"
}
*/

// String (caractère personnalisé)
console.log(JSON.stringify(user, null, '\t'));
/*
{
    "name": "John",
    "age": 30,
    "city": "Paris"
}
*/

// MÉTHODE toJSON()

// Objects peuvent définir leur propre serialization
const customObj = {
    name: 'John',
    password: 'secret',
    toJSON() {
        // Contrôler ce qui est serialisé
        return {
            name: this.name
            // password exclu
        };
    }
};

console.log(JSON.stringify(customObj)); // '{"name":"John"}'

// Date a un toJSON() built-in
const now = new Date();
console.log(JSON.stringify(now)); // '"2024-11-07T15:30:00.000Z"'

// GESTION DE RÉFÉRENCES CIRCULAIRES

const circular = { name: 'obj' };
circular.self = circular;

// JSON.stringify(circular); // ❌ TypeError: Converting circular structure

// Solution: replacer function
const seen = new WeakSet();
const safeStringify = JSON.stringify(circular, (key, value) => {
    if (typeof value === 'object' && value !== null) {
        if (seen.has(value)) {
            return '[Circular]';
        }
        seen.add(value);
    }
    return value;
});
console.log(safeStringify); // '{"name":"obj","self":"[Circular]"}'

// EXEMPLES PRATIQUES

// 1. Deep clone (simple objects)
function deepClone(obj) {
    return JSON.parse(JSON.stringify(obj));
}

const original = { x: 1, nested: { y: 2 } };
const cloned = deepClone(original);
cloned.nested.y = 10;
console.log(original.nested.y); // 2 (inchangé)

// 2. Comparer objets
function areEqual(obj1, obj2) {
    return JSON.stringify(obj1) === JSON.stringify(obj2);
}

console.log(areEqual({ a: 1, b: 2 }, { a: 1, b: 2 })); // true
console.log(areEqual({ a: 1, b: 2 }, { b: 2, a: 1 })); // false (ordre différent)

// 3. Sauvegarder dans localStorage
const userData = { name: 'John', preferences: { theme: 'dark' } };
localStorage.setItem('user', JSON.stringify(userData));

// 4. Envoyer dans API
async function saveUser(user) {
    const response = await fetch('/api/users', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(user)
    });
    return response.json();
}

// 5. Logger objets de manière lisible
console.log(JSON.stringify(data, null, 2));

// 6. Masquer données sensibles
function sanitize(obj) {
    return JSON.stringify(obj, (key, value) => {
        const sensitiveKeys = ['password', 'token', 'secret'];
        if (sensitiveKeys.includes(key)) {
            return '***';
        }
        return value;
    });
}

const user = { name: 'John', password: 'secret123' };
console.log(sanitize(user)); // '{"name":"John","password":"***"}'
```

### JSON.parse()

```javascript
// JSON.parse() - Convertir JSON string en objet JavaScript

// Syntaxe de base
const json = '{"name":"John","age":30}';
const obj = JSON.parse(json);
console.log(obj.name); // "John"
console.log(obj.age);  // 30

// PARAMÈTRES: JSON.parse(text, reviver)

// 1. TEXT - JSON string à parser

// Object
const userJson = '{"name":"John","age":30}';
const user = JSON.parse(userJson);
console.log(user); // { name: 'John', age: 30 }

// Array
const arrJson = '[1,2,3,"four",true]';
const arr = JSON.parse(arrJson);
console.log(arr); // [1, 2, 3, 'four', true]

// Primitives
console.log(JSON.parse('123'));     // 123
console.log(JSON.parse('"hello"')); // "hello"
console.log(JSON.parse('true'));    // true
console.log(JSON.parse('null'));    // null

// ❌ Invalid JSON
try {
    JSON.parse('undefined'); // ❌ SyntaxError
} catch (error) {
    console.error('Invalid JSON:', error.message);
}

try {
    JSON.parse("{'name':'John'}"); // ❌ Single quotes
} catch (error) {
    console.error('Invalid JSON:', error.message);
}

// 2. REVIVER - Transformer valeurs pendant parsing

// Reviver function
const data = JSON.parse('{"name":"John","age":30}', (key, value) => {
    console.log(`Key: ${key}, Value: ${value}`);
    return value;
});
// Key: name, Value: John
// Key: age, Value: 30
// Key: , Value: [object Object]

// Transformer dates
const jsonWithDate = '{"name":"John","createdAt":"2024-11-07T15:30:00.000Z"}';
const parsed = JSON.parse(jsonWithDate, (key, value) => {
    // Convertir strings ISO en Date objects
    if (key === 'createdAt') {
        return new Date(value);
    }
    return value;
});

console.log(parsed.createdAt); // Date object
console.log(parsed.createdAt instanceof Date); // true

// Transformer nombres
const numJson = '{"price":"19.99","quantity":"5"}';
const nums = JSON.parse(numJson, (key, value) => {
    if (key === 'price' || key === 'quantity') {
        return parseFloat(value);
    }
    return value;
});

console.log(typeof nums.price); // 'number'
console.log(nums.price + nums.quantity); // 24.99

// VALIDATION

// Vérifier si string est valid JSON
function isValidJSON(str) {
    try {
        JSON.parse(str);
        return true;
    } catch (error) {
        return false;
    }
}

console.log(isValidJSON('{"name":"John"}')); // true
console.log(isValidJSON('{name:"John"}'));   // false

// Parse avec valeur par défaut
function safeParse(jsonString, defaultValue = {}) {
    try {
        return JSON.parse(jsonString);
    } catch (error) {
        console.error('Parse error:', error.message);
        return defaultValue;
    }
}

// EXEMPLES PRATIQUES

// 1. Récupérer de localStorage
const userJson = localStorage.getItem('user');
const user = userJson ? JSON.parse(userJson) : null;

// 2. API response
async function fetchUser(id) {
    const response = await fetch(`/api/users/${id}`);
    const json = await response.text();
    return JSON.parse(json);
}

// Ou plus simple (response.json() fait le parse)
async function fetchUser2(id) {
    const response = await fetch(`/api/users/${id}`);
    return await response.json(); // Parse automatique
}

// 3. Deep clone avec dates
function deepCloneWithDates(obj) {
    return JSON.parse(
        JSON.stringify(obj),
        (key, value) => {
            // Reconvertir strings ISO en Dates
            if (typeof value === 'string' && /^\d{4}-\d{2}-\d{2}T/.test(value)) {
                return new Date(value);
            }
            return value;
        }
    );
}

// 4. Config file
const configJson = `{
    "apiUrl": "https://api.example.com",
    "timeout": 5000,
    "retries": 3
}`;

const config = JSON.parse(configJson);

// 5. Parse et valider
function parseAndValidate(jsonString) {
    try {
        const data = JSON.parse(jsonString);
        
        // Validation
        if (!data.name || !data.email) {
            throw new Error('Missing required fields');
        }
        
        return { success: true, data };
        
    } catch (error) {
        return { success: false, error: error.message };
    }
}

// LIMITES ET CONSIDÉRATIONS

// ❌ Ne peut pas parser:
// - Functions
// - undefined
// - Symbols
// - Circular references

// ❌ Perd information de type:
const original = {
    date: new Date(),
    regex: /test/,
    func: () => {}
};

const json = JSON.stringify(original);
const parsed = JSON.parse(json);

console.log(parsed.date); // String, pas Date!
console.log(parsed.regex); // {} (objet vide)
console.log(parsed.func); // undefined

// ✅ Solution: reviver function
const betterParsed = JSON.parse(json, (key, value) => {
    if (key === 'date') {
        return new Date(value);
    }
    return value;
});

// PERFORMANCE

// Pour grandes structures de données, considérer:
// - Streamed parsing (libraries externes)
// - Web Workers pour parsing en background
// - Chunked parsing pour très gros JSON

// BIG JSON (>10MB)
async function parseHugeJSON(url) {
    const response = await fetch(url);
    const reader = response.body.getReader();
    // ... streaming parse
}
```

---

**Points clés à retenir:**
- **Spread/Rest (`...`)**: étaler (spread) ou rassembler (rest)
- **Short-circuit**: `&&`, `||` pour évaluation conditionnelle
- **Nullish coalescing (`??`)**: seulement null/undefined
- **Optional chaining (`?.`)**: accès sûr aux propriétés
- **try/catch/finally**: gestion d'erreurs robuste
- **Custom errors**: classes Error personnalisées
- **Strict mode**: toujours activer (`'use strict'`)
- **JSON**: `stringify()` et `parse()` avec options

**Pour React:**
- Spread pour props: `<Component {...props} />`
- Rest pour récupérer props restants
- `?.` pour données API optionnelles
- `??` pour valeurs par défaut (0, false valides)
- Error boundaries pour custom errors
- JSON pour localStorage state

---

# Questions/Réponses - Chapitre 17 : Concepts Avancés JavaScript

## Questions sur Spread et Rest Operators

### Q1 : Quelle est la différence entre le spread operator et le rest operator ?

**Réponse :**
Bien qu'ils utilisent la même syntaxe `...`, ils ont des usages opposés :

**Spread operator** : **décompose** un itérable (array, object) en éléments individuels
**Rest operator** : **regroupe** plusieurs éléments en un array

```javascript
// SPREAD - décomposer
const arr1 = [1, 2, 3];
const arr2 = [...arr1, 4, 5]; // [1, 2, 3, 4, 5]

const obj1 = { a: 1, b: 2 };
const obj2 = { ...obj1, c: 3 }; // { a: 1, b: 2, c: 3 }

// REST - regrouper
function somme(...nombres) { // Tous les arguments dans un array
  return nombres.reduce((sum, n) => sum + n, 0);
}
somme(1, 2, 3, 4); // 10

const [premier, deuxieme, ...reste] = [1, 2, 3, 4, 5];
// premier = 1, deuxieme = 2, reste = [3, 4, 5]
```

**Point clé :** 
- Spread = expansion (à droite du =)
- Rest = condensation (à gauche du = ou dans les paramètres de fonction)

---

### Q2 : Comment utiliser le spread operator pour cloner un objet ou un array ?

**Réponse :**
Le spread operator permet de créer des **shallow copies** (copies superficielles).

```javascript
// ARRAY
const original = [1, 2, 3];
const clone = [...original];

clone.push(4);
console.log(original); // [1, 2, 3] - non affecté
console.log(clone);    // [1, 2, 3, 4]

// OBJECT
const user = { nom: 'Jean', age: 30 };
const copie = { ...user };

copie.age = 31;
console.log(user.age);  // 30 - non affecté
console.log(copie.age); // 31
```

**⚠️ Attention : Shallow copy (copie superficielle)**
```javascript
const original = { 
  nom: 'Jean', 
  adresse: { ville: 'Paris', code: 75001 } 
};

const copie = { ...original };

// Les objets imbriqués sont toujours liés !
copie.adresse.ville = 'Lyon';
console.log(original.adresse.ville); // 'Lyon' - affecté !

// Pour une deep copy :
const deepCopy = JSON.parse(JSON.stringify(original));
// Ou utiliser une bibliothèque comme lodash _.cloneDeep()
```

**En entretien, mentionner :** Spread fait une copie superficielle, pas une copie profonde.

---

### Q3 : Comment fusionner plusieurs arrays ou objets avec le spread operator ?

**Réponse :**

**Arrays :**
```javascript
const arr1 = [1, 2, 3];
const arr2 = [4, 5, 6];
const arr3 = [7, 8, 9];

// Fusion
const fusion = [...arr1, ...arr2, ...arr3];
// [1, 2, 3, 4, 5, 6, 7, 8, 9]

// Avec des éléments supplémentaires
const complet = [0, ...arr1, 3.5, ...arr2];
// [0, 1, 2, 3, 3.5, 4, 5, 6]
```

**Objects :**
```javascript
const defaut = { theme: 'dark', langue: 'fr' };
const user = { nom: 'Jean', langue: 'en' };
const preferences = { notifications: true };

// Fusion (les propriétés de droite écrasent celles de gauche)
const config = { ...defaut, ...user, ...preferences };
// { theme: 'dark', langue: 'en', nom: 'Jean', notifications: true }
```

**Ordre important :**
```javascript
const obj1 = { a: 1, b: 2 };
const obj2 = { b: 3, c: 4 };

console.log({ ...obj1, ...obj2 }); // { a: 1, b: 3, c: 4 } - obj2.b écrase
console.log({ ...obj2, ...obj1 }); // { b: 2, c: 4, a: 1 } - obj1.b écrase
```

---

### Q4 : Donnez des exemples d'utilisation du rest operator dans les fonctions.

**Réponse :**

**1. Nombre variable d'arguments :**
```javascript
function somme(...nombres) {
  return nombres.reduce((total, n) => total + n, 0);
}

somme(1, 2, 3);        // 6
somme(1, 2, 3, 4, 5); // 15
```

**2. Séparer le premier argument du reste :**
```javascript
function logger(niveau, ...messages) {
  console.log(`[${niveau}]`, ...messages);
}

logger('INFO', 'Serveur démarré', 'Port 3000');
// [INFO] Serveur démarré Port 3000
```

**3. Dans le destructuring :**
```javascript
const [premier, deuxieme, ...autres] = [1, 2, 3, 4, 5];
// premier = 1, deuxieme = 2, autres = [3, 4, 5]

const { nom, age, ...autresInfos } = { 
  nom: 'Jean', 
  age: 30, 
  ville: 'Paris', 
  pays: 'France' 
};
// nom = 'Jean', age = 30, autresInfos = { ville: 'Paris', pays: 'France' }
```

**⚠️ Rest doit toujours être le dernier paramètre :**
```javascript
function maFonction(a, b, ...reste) {} // ✅ Valide
function maFonction(a, ...reste, b) {} // ❌ SyntaxError
```

---

## Questions sur Short-Circuit Evaluation

### Q5 : Qu'est-ce que le short-circuit evaluation et comment fonctionne-t-il avec && et || ?

**Réponse :**
Le **short-circuit evaluation** signifie que JavaScript arrête l'évaluation dès qu'il connaît le résultat final.

**Avec && (AND logique) :**
- Si la partie gauche est **falsy**, retourne la valeur gauche (sans évaluer la droite)
- Si la partie gauche est **truthy**, retourne la valeur droite

```javascript
// Valeur retournée, pas juste true/false
const result1 = true && 'Hello';  // 'Hello'
const result2 = false && 'Hello'; // false
const result3 = 0 && 'Hello';     // 0 (falsy)
const result4 = 'Hi' && 'Hello';  // 'Hello'

// Usage pratique : exécution conditionnelle
const user = { nom: 'Jean' };
user.admin && console.log('Admin'); // N'exécute que si admin est truthy

// Éviter les erreurs
const valeur = objet && objet.propriete && objet.propriete.sousProp;
```

**Avec || (OR logique) :**
- Si la partie gauche est **truthy**, retourne la valeur gauche (sans évaluer la droite)
- Si la partie gauche est **falsy**, retourne la valeur droite

```javascript
const result1 = true || 'Hello';  // true
const result2 = false || 'Hello'; // 'Hello'
const result3 = 0 || 'Défaut';    // 'Défaut'
const result4 = 'Hi' || 'Hello';  // 'Hi'

// Usage pratique : valeurs par défaut
const nom = user.nom || 'Anonyme';
const port = process.env.PORT || 3000;
```

---

### Q6 : Quelle est la différence entre || et ?? (nullish coalescing) ?

**Réponse :**
La différence est dans ce qu'ils considèrent comme "vide".

**|| (OR)** : considère TOUTES les valeurs falsy comme "vides"
- `false`, `0`, `''`, `null`, `undefined`, `NaN`

**?? (Nullish Coalescing)** : considère SEULEMENT `null` et `undefined` comme "vides"

```javascript
// Différence avec 0
const count1 = 0 || 10;  // 10 (0 est falsy)
const count2 = 0 ?? 10;  // 0 (0 n'est pas null/undefined)

// Différence avec ''
const name1 = '' || 'Défaut';  // 'Défaut' ('' est falsy)
const name2 = '' ?? 'Défaut';  // '' ('' n'est pas null/undefined)

// Différence avec false
const flag1 = false || true;  // true (false est falsy)
const flag2 = false ?? true;  // false (false n'est pas null/undefined)

// Avec null/undefined - même comportement
const val1 = null || 'Défaut';  // 'Défaut'
const val2 = null ?? 'Défaut';  // 'Défaut'
```

**Cas d'usage pour ?? :**
```javascript
// Quand 0 ou '' sont des valeurs valides
function afficherScore(score) {
  // ❌ Problème avec ||
  const affichage1 = score || 'Pas de score'; // 0 devient "Pas de score"
  
  // ✅ Solution avec ??
  const affichage2 = score ?? 'Pas de score'; // 0 reste 0
  
  return affichage2;
}

afficherScore(0);         // 0 (correct avec ??)
afficherScore(undefined); // 'Pas de score'
```

**En entretien, souligner :** `??` est plus précis quand 0, '', ou false sont des valeurs légitimes.

---

### Q7 : Qu'est-ce que l'optional chaining (?.) et comment l'utiliser ?

**Réponse :**
L'**optional chaining** (`?.`) permet d'accéder à des propriétés imbriquées sans risquer une erreur si une valeur intermédiaire est `null` ou `undefined`.

**Sans optional chaining (ancien code) :**
```javascript
const user = { nom: 'Jean' }; // Pas de propriété adresse

// ❌ Erreur : Cannot read property 'ville' of undefined
const ville = user.adresse.ville; 

// ✅ Protection manuelle (verbeux)
const ville2 = user && user.adresse && user.adresse.ville;
```

**Avec optional chaining :**
```javascript
const ville = user?.adresse?.ville; // undefined (pas d'erreur)

// Si user est null/undefined, tout s'arrête immédiatement
const ville2 = user?.adresse?.ville ?? 'Inconnue'; // 'Inconnue'
```

**Cas d'usage multiples :**

1. **Propriétés d'objet :**
```javascript
const nom = user?.profil?.nom;
```

2. **Appels de méthodes :**
```javascript
const resultat = objet.methode?.(); // Appelle seulement si methode existe
user.onUpdate?.(); // Callback optionnel
```

3. **Accès aux arrays :**
```javascript
const premier = tableau?.[0]; // undefined si tableau est null/undefined
const valeur = objet?.['cle-dynamique'];
```

4. **Combinaison avec nullish coalescing :**
```javascript
const config = {
  api: {
    timeout: 5000
  }
};

const timeout = config?.api?.timeout ?? 3000; // 5000
const retries = config?.api?.retries ?? 3;    // 3 (défaut)
```

**⚠️ Ne s'arrête que sur null/undefined :**
```javascript
const user = { adresse: { ville: null } };
const ville = user?.adresse?.ville; // null (pas undefined)
// ville existe mais vaut null
```

---

## Questions sur Error Handling

### Q8 : Comment fonctionne try...catch...finally en JavaScript ?

**Réponse :**

**Structure :**
```javascript
try {
  // Code qui peut échouer
} catch (erreur) {
  // Gestion de l'erreur
} finally {
  // Toujours exécuté (optionnel)
}
```

**Comportement détaillé :**

```javascript
function exemple() {
  try {
    console.log('1. Début du try');
    
    throw new Error('Oups !');
    
    console.log('2. Ceci ne s\'exécute jamais');
    
  } catch (err) {
    console.log('3. Dans catch :', err.message);
    
  } finally {
    console.log('4. Toujours exécuté (finally)');
  }
  
  console.log('5. Après try/catch');
}

// Affichage :
// 1. Début du try
// 3. Dans catch : Oups !
// 4. Toujours exécuté (finally)
// 5. Après try/catch
```

**Finally s'exécute même avec return :**
```javascript
function avecReturn() {
  try {
    console.log('Try');
    return 'Valeur du try';
  } catch (err) {
    console.log('Catch');
  } finally {
    console.log('Finally'); // S'exécute AVANT le return
  }
}

avecReturn();
// Try
// Finally
// Retourne : 'Valeur du try'
```

**Usage typique avec async/await :**
```javascript
async function chargerDonnees() {
  try {
    afficherLoader(); // Montrer le spinner
    
    const response = await fetch('/api/data');
    if (!response.ok) throw new Error('Erreur HTTP');
    
    const data = await response.json();
    return data;
    
  } catch (erreur) {
    console.error('Erreur :', erreur);
    afficherErreur(erreur.message);
    
  } finally {
    cacherLoader(); // Toujours cacher le spinner
  }
}
```

---

### Q9 : Comment créer et lancer une erreur personnalisée ?

**Réponse :**

**1. Lancer une erreur simple :**
```javascript
function diviser(a, b) {
  if (b === 0) {
    throw new Error('Division par zéro impossible');
  }
  return a / b;
}

try {
  diviser(10, 0);
} catch (err) {
  console.error(err.message); // 'Division par zéro impossible'
}
```

**2. Créer une classe d'erreur personnalisée :**
```javascript
class ValidationError extends Error {
  constructor(message) {
    super(message);
    this.name = 'ValidationError'; // Important pour identifier le type
  }
}

class NotFoundError extends Error {
  constructor(resource) {
    super(`${resource} non trouvé`);
    this.name = 'NotFoundError';
    this.statusCode = 404;
  }
}

// Utilisation
function validerEmail(email) {
  if (!email.includes('@')) {
    throw new ValidationError('Email invalide');
  }
  return true;
}

try {
  validerEmail('mauvais-email');
} catch (err) {
  if (err instanceof ValidationError) {
    console.error('Erreur de validation :', err.message);
  } else {
    console.error('Autre erreur :', err);
  }
}
```

**3. Erreurs avec données supplémentaires :**
```javascript
class APIError extends Error {
  constructor(message, statusCode, details) {
    super(message);
    this.name = 'APIError';
    this.statusCode = statusCode;
    this.details = details;
    this.timestamp = new Date();
  }
}

async function appelAPI() {
  const response = await fetch('/api/data');
  
  if (!response.ok) {
    const errorData = await response.json();
    throw new APIError(
      'Échec de l\'API',
      response.status,
      errorData
    );
  }
  
  return await response.json();
}

try {
  await appelAPI();
} catch (err) {
  if (err instanceof APIError) {
    console.error(`Erreur API ${err.statusCode}:`, err.message);
    console.error('Détails :', err.details);
  }
}
```

---

### Q10 : Quelle est la différence entre throw Error et throw string ?

**Réponse :**
On peut techniquement `throw` n'importe quelle valeur, mais utiliser `Error` est la bonne pratique.

```javascript
// ❌ Mauvaise pratique : throw string
throw 'Une erreur !';

// ❌ Mauvaise pratique : throw number
throw 404;

// ✅ Bonne pratique : throw Error
throw new Error('Une erreur !');
```

**Pourquoi utiliser Error ?**

1. **Stack trace** (trace de la pile d'appels) :
```javascript
function fonctionA() {
  fonctionB();
}

function fonctionB() {
  throw new Error('Erreur ici');
}

try {
  fonctionA();
} catch (err) {
  console.error(err.stack);
  // Error: Erreur ici
  //   at fonctionB (...)
  //   at fonctionA (...)
  // Avec string, pas de stack trace !
}
```

2. **Propriétés standard** :
```javascript
try {
  throw new Error('Mon erreur');
} catch (err) {
  console.log(err.message); // 'Mon erreur'
  console.log(err.name);    // 'Error'
  console.log(err.stack);   // Stack trace complète
}
```

3. **instanceof pour identifier** :
```javascript
try {
  // ...
} catch (err) {
  if (err instanceof Error) {
    // C'est une vraie erreur
  } else {
    // Quelque chose d'autre a été lancé
  }
}
```

**Types d'erreurs natifs :**
```javascript
// TypeError : mauvais type
const obj = null;
obj.propriete; // TypeError: Cannot read property of null

// ReferenceError : variable non définie
console.log(variableInexistante); // ReferenceError

// SyntaxError : syntaxe invalide
eval('var a = ;'); // SyntaxError

// RangeError : valeur hors limites
new Array(-1); // RangeError
```

---

## Questions sur Strict Mode

### Q11 : Qu'est-ce que le strict mode et quels sont ses avantages ?

**Réponse :**
Le **strict mode** (`'use strict'`) est un mode qui applique des règles JavaScript plus strictes pour éviter les erreurs communes.

**Activation :**
```javascript
// Global (tout le fichier)
'use strict';

function maFonction() {
  // Code en strict mode
}

// Ou local (seulement dans une fonction)
function autreFonction() {
  'use strict';
  // Code en strict mode
}
```

**Principales différences :**

**1. Variables doivent être déclarées :**
```javascript
'use strict';

// ❌ Erreur en strict mode
maVariable = 10; // ReferenceError: maVariable is not defined

// ✅ OK
let maVariable = 10;
```

**2. Interdiction d'utiliser des mots réservés :**
```javascript
'use strict';

// ❌ Erreurs
var let = 10;      // SyntaxError
var private = 20;  // SyntaxError
```

**3. this vaut undefined dans les fonctions :**
```javascript
'use strict';

function maFonction() {
  console.log(this); // undefined (au lieu de window)
}

maFonction();

// Sans strict mode
function sanStrict() {
  console.log(this); // window (en navigateur)
}
```

**4. Pas de duplication de paramètres :**
```javascript
'use strict';

// ❌ Erreur en strict mode
function test(a, a, b) { // SyntaxError: Duplicate parameter name
  return a + b;
}
```

**5. Interdiction de delete sur variables :**
```javascript
'use strict';

let x = 10;
delete x; // SyntaxError

// delete fonctionne seulement sur les propriétés d'objets
const obj = { prop: 1 };
delete obj.prop; // ✅ OK
```

**6. eval a son propre scope :**
```javascript
'use strict';

eval('var x = 10');
console.log(x); // ReferenceError: x is not defined

// Sans strict mode, x serait accessible
```

**Avantages :**
- Détecte les erreurs plus tôt
- Améliore la performance (optimisations du moteur JS)
- Prépare au code moderne (ES6 modules sont automatiquement en strict)
- Évite les comportements étranges de JavaScript

**Note :** Les modules ES6 et les classes sont automatiquement en strict mode.

---

## Questions sur JSON

### Q12 : Comment fonctionne JSON.stringify() et quelles sont ses options ?

**Réponse :**
`JSON.stringify()` convertit un objet JavaScript en string JSON.

**Syntaxe complète :**
```javascript
JSON.stringify(value, replacer, space)
```

**Usage basique :**
```javascript
const user = {
  nom: 'Jean',
  age: 30,
  actif: true
};

const json = JSON.stringify(user);
// '{"nom":"Jean","age":30,"actif":true}'
```

**Paramètre `replacer` (filtrer/transformer) :**

1. **Array (liste blanche de propriétés) :**
```javascript
const user = {
  nom: 'Jean',
  age: 30,
  motDePasse: 'secret123',
  email: 'jean@exemple.com'
};

// Seulement nom et email
const json = JSON.stringify(user, ['nom', 'email']);
// '{"nom":"Jean","email":"jean@exemple.com"}'
```

2. **Fonction (transformation personnalisée) :**
```javascript
const data = {
  nom: 'Jean',
  age: 30,
  dateCreation: new Date('2024-01-01')
};

const json = JSON.stringify(data, (key, value) => {
  // Exclure age
  if (key === 'age') return undefined;
  
  // Formater les dates
  if (value instanceof Date) {
    return value.toISOString();
  }
  
  return value;
});
// '{"nom":"Jean","dateCreation":"2024-01-01T00:00:00.000Z"}'
```

**Paramètre `space` (formatage lisible) :**
```javascript
const user = { nom: 'Jean', age: 30 };

// Sans space
JSON.stringify(user);
// '{"nom":"Jean","age":30}'

// Avec indentation (2 espaces)
JSON.stringify(user, null, 2);
/*
{
  "nom": "Jean",
  "age": 30
}
*/

// Avec caractère personnalisé
JSON.stringify(user, null, '\t'); // Tabulation
```

**Valeurs non sérialisables :**
```javascript
const obj = {
  fonction: () => {},           // Ignorée
  undefined: undefined,         // Ignorée
  symbol: Symbol('test'),       // Ignorée
  date: new Date(),            // Convertie en string
  regex: /test/,               // Devient {}
  nombre: NaN,                 // Devient null
  infini: Infinity,            // Devient null
  normal: 'OK'                 // Gardée
};

JSON.stringify(obj);
// '{"date":"2024-...","regex":{},"nombre":null,"infini":null,"normal":"OK"}'
```

**Méthode toJSON personnalisée :**
```javascript
class User {
  constructor(nom, age, motDePasse) {
    this.nom = nom;
    this.age = age;
    this.motDePasse = motDePasse;
  }
  
  // Contrôle la sérialisation
  toJSON() {
    return {
      nom: this.nom,
      age: this.age
      // motDePasse exclu
    };
  }
}

const user = new User('Jean', 30, 'secret');
JSON.stringify(user);
// '{"nom":"Jean","age":30}'
```

---

### Q13 : Comment fonctionne JSON.parse() et la fonction reviver ?

**Réponse :**
`JSON.parse()` convertit une string JSON en objet JavaScript.

**Syntaxe :**
```javascript
JSON.parse(text, reviver)
```

**Usage basique :**
```javascript
const json = '{"nom":"Jean","age":30}';
const user = JSON.parse(json);

console.log(user.nom); // 'Jean'
console.log(user.age); // 30
```

**Paramètre `reviver` (transformation) :**

```javascript
const json = '{"nom":"Jean","dateNaissance":"1990-01-15T00:00:00.000Z"}';

const user = JSON.parse(json, (key, value) => {
  // Convertir les strings qui ressemblent à des dates
  if (typeof value === 'string' && /^\d{4}-\d{2}-\d{2}T/.test(value)) {
    return new Date(value);
  }
  return value;
});

console.log(user.dateNaissance); // Date object
console.log(user.dateNaissance.getFullYear()); // 1990
```

**Gestion d'erreurs :**
```javascript
function parseJSON(jsonString) {
  try {
    return JSON.parse(jsonString);
  } catch (err) {
    console.error('JSON invalide :', err.message);
    return null;
  }
}

parseJSON('{"nom":"Jean"}');        // OK
parseJSON('{nom: "Jean"}');         // null (guillemets manquants)
parseJSON('{"nom":"Jean",}');       // null (virgule finale)
parseJSON('undefined');             // null
```

**Reviver avancé :**
```javascript
const json = `{
  "nom": "Jean",
  "age": 30,
  "createdAt": "2024-01-01T00:00:00.000Z",
  "scores": "[10,20,30]"
}`;

const user = JSON.parse(json, (key, value) => {
  // Dates
  if (key === 'createdAt') {
    return new Date(value);
  }
  
  // Arrays stringifiés
  if (key === 'scores' && typeof value === 'string') {
    return JSON.parse(value);
  }
  
  return value;
});

console.log(user.createdAt instanceof Date); // true
console.log(Array.isArray(user.scores));     // true
```

---

## Questions Pratiques et Avancées

### Q14 : Comment faire une deep copy (copie profonde) d'un objet en JavaScript ?

**Réponse :**
Il existe plusieurs méthodes selon les besoins.

**1. JSON.parse(JSON.stringify()) - Simple mais limité :**
```javascript
const original = {
  nom: 'Jean',
  adresse: {
    ville: 'Paris',
    code: 75001
  },
  hobbies: ['lecture', 'sport']
};

const copie = JSON.parse(JSON.stringify(original));

copie.adresse.ville = 'Lyon';
console.log(original.adresse.ville); // 'Paris' - non affecté ✅

// ⚠️ Limitations :
const avecFonctions = {
  nom: 'Jean',
  sayHi: () => 'Hello',      // Perdue
  date: new Date(),          // Devient string
  undefined: undefined,      // Perdue
  symbol: Symbol('test')     // Perdu
};

const copie2 = JSON.parse(JSON.stringify(avecFonctions));
// { nom: 'Jean', date: '2024-...' }
```

**2. structuredClone() - Moderne (ES2022) :**
```javascript
const original = {
  nom: 'Jean',
  date: new Date(),
  map: new Map([['key', 'value']]),
  set: new Set([1, 2, 3]),
  nested: { deep: { value: 42 } }
};

const copie = structuredClone(original);

// ✅ Preserve les types (Date, Map, Set, etc.)
console.log(copie.date instanceof Date); // true
console.log(copie.map instanceof Map);   // true

// ⚠️ Ne clone pas les fonctions
const avecFonction = { fn: () => {} };
structuredClone(avecFonction); // Erreur
```

**3. Fonction récursive personnalisée :**
```javascript
function deepClone(obj, hash = new WeakMap()) {
  // Gérer null, undefined, primitifs
  if (obj === null || typeof obj !== 'object') {
    return obj;
  }
  
  // Gérer les références circulaires
  if (hash.has(obj)) {
    return hash.get(obj);
  }
  
  // Gérer Date
  if (obj instanceof Date) {
    return new Date(obj);
  }
  
  // Gérer Array
  if (Array.isArray(obj)) {
    const arrCopy = [];
    hash.set(obj, arrCopy);
    obj.forEach((item, index) => {
      arrCopy[index] = deepClone(item, hash);
    });
    return arrCopy;
  }
  
  // Gérer Object
  const objCopy = {};
  hash.set(obj, objCopy);
  Object.keys(obj).forEach(key => {
    objCopy[key] = deepClone(obj[key], hash);
  });
  
  return objCopy;
}

// Test
const original = {
  nom: 'Jean',
  nested: { value: 42 },
  arr: [1, 2, { deep: 3 }]
};

const copie = deepClone(original);
```

**4. Lodash (bibliothèque) :**
```javascript
import _ from 'lodash';

const copie = _.cloneDeep(original);
// Clone tout : fonctions, symboles, références circulaires, etc.
```

**Comparaison :**
| Méthode | Avantages | Inconvénients |
|---------|-----------|---------------|
| JSON.parse/stringify | Simple, natif, rapide | Perd fonctions, Date, undefined, Symbol |
| structuredClone | Natif, préserve types (Date, Map, Set) | Pas de fonctions, support récent |
| Fonction récursive | Contrôle total | Complexe, doit gérer tous les cas |
| Lodash cloneDeep | Le plus complet | Dépendance externe |

**En entretien, recommander :**
- `structuredClone()` pour la plupart des cas modernes
- JSON pour des objets simples
- Lodash pour des besoins complexes

---

### Q15 : Qu'est-ce qu'un truthy et un falsy value ? Listez toutes les valeurs falsy.

**Réponse :**
En JavaScript, toutes les valeurs ont une valeur booléenne implicite en contexte boolean.

**Les 7 valeurs falsy (exhaustif) :**
```javascript
if (false) {}         // 1. false
if (0) {}            // 2. 0
if (-0) {}           // 3. -0
if (0n) {}           // 4. 0n (BigInt zéro)
if ('') {}           // 5. '' (empty string)
if (null) {}         // 6. null
if (undefined) {}    // 7. undefined
if (NaN) {}          // 8. NaN

// TOUTES les autres valeurs sont truthy
```

**Valeurs truthy (exemples) :**
```javascript
if (true) {}           // ✅
if (1) {}              // ✅
if (-1) {}             // ✅
if ('0') {}            // ✅ String '0' est truthy !
if ('false') {}        // ✅ String 'false' est truthy !
if ([]) {}             // ✅ Array vide est truthy !
if ({}) {}             // ✅ Object vide est truthy !
if (function(){}) {}   // ✅
if (new Date()) {}     // ✅
```

**Pièges courants :**
```javascript
// ⚠️ '0' est truthy mais 0 est falsy
if ('0') {
  console.log('Exécuté'); // S'exécute !
}

// ⚠️ [] et {} sont truthy
const arr = [];
if (arr) {
  console.log('Array vide est truthy'); // S'exécute !
}

// Pour tester si array est vide :
if (arr.length) {} // ❌ 0 est falsy
if (arr.length > 0) {} // ✅

// ⚠️ NaN n'est égal à rien, même pas lui-même
console.log(NaN === NaN); // false
console.log(isNaN(NaN));  // true ✅
```

**Conversion explicite en boolean :**
```javascript
Boolean(0);           // false
Boolean('');          // false
Boolean(null);        // false
Boolean(undefined);   // false
Boolean(NaN);         // false

Boolean('hello');     // true
Boolean(42);          // true
Boolean([]);          // true
Boolean({});          // true

// Double négation (!! - raccourci)
!!'hello';            // true
!!0;                  // false
!![];                 // true
```

---

### Q16 : Quelle est la différence entre == et === ?

**Réponse :**

**== (égalité faible)** : compare les valeurs après **coercition de type**
**=== (égalité stricte)** : compare les valeurs ET les types, **sans coercition**

```javascript
// Différence principale
console.log(5 == '5');   // true (string converti en number)
console.log(5 === '5');  // false (types différents)

console.log(true == 1);   // true (true converti en 1)
console.log(true === 1);  // false (types différents)

console.log(null == undefined);   // true (cas spécial)
console.log(null === undefined);  // false (types différents)
```

**Règles de coercition avec == :**

```javascript
// String et Number
'10' == 10;          // true (string → number)
'' == 0;             // true ('' → 0)
'0' == 0;            // true ('0' → 0)

// Boolean et Number
true == 1;           // true (true → 1)
false == 0;          // true (false → 0)

// Null et Undefined
null == undefined;   // true (cas spécial)
null == 0;           // false (null ne devient pas 0)
undefined == 0;      // false

// Objects
[] == false;         // true ([] → '' → 0, false → 0)
[1] == 1;           // true ([1] → '1' → 1)
['1'] == 1;         // true
```

**Pièges avec == :**
```javascript
// Comportements étranges
'' == '0';           // false
0 == '';             // true
0 == '0';            // true

'' == false;         // true
'0' == false;        // true
'' == 0;             // true

// Problème de transitivité brisée
'0' == false;        // true
false == 0;          // true
'0' == 0;            // true
// Mais :
'0' != '';           // true (incohérent !)
```

**Recommandation (best practice) :**
```javascript
// ✅ TOUJOURS utiliser === (égalité stricte)
if (valeur === 5) {}
if (nom === 'Jean') {}
if (user === null) {}

// ❌ Éviter == sauf cas très spécifique
// Seul cas acceptable : tester null/undefined ensemble
if (valeur == null) { // Attrape null ET undefined
  // Équivalent à : valeur === null || valeur === undefined
}

// Même chose pour != vs !==
if (valeur !== 5) {} // ✅
if (valeur != 5) {}  // ❌
```

**En entretien, souligner :** 
- Utiliser === par défaut (ESLint le recommande)
- == peut causer des bugs subtils
- Seule exception : `valeur == null` pour détecter null/undefined

---

### Q17 : Expliquez le concept de hoisting en JavaScript.

**Réponse :**
Le **hoisting** (remontée) est le comportement JavaScript qui déplace les déclarations en haut de leur scope lors de la compilation.

**Variables avec var :**
```javascript
console.log(x); // undefined (pas d'erreur)
var x = 5;
console.log(x); // 5

// Ce que JavaScript fait en réalité :
var x;              // Déclaration remontée
console.log(x);     // undefined
x = 5;              // Assignation reste en place
console.log(x);     // 5
```

**Variables avec let/const (Temporal Dead Zone) :**
```javascript
console.log(y); // ReferenceError: Cannot access 'y' before initialization
let y = 10;

// let/const sont "hissées" mais dans une "Temporal Dead Zone"
// On ne peut pas y accéder avant la déclaration
```

**Fonctions (function declarations) :**
```javascript
// ✅ Fonctionne - fonction entièrement hissée
saluer(); // 'Bonjour !'

function saluer() {
  console.log('Bonjour !');
}

// ❌ Function expressions ne sont PAS hissées
direAuRevoir(); // TypeError: direAuRevoir is not a function

var direAuRevoir = function() {
  console.log('Au revoir !');
};

// Équivalent à :
var direAuRevoir;           // Hissée
direAuRevoir();             // undefined() → TypeError
direAuRevoir = function() {};
```

**Comparaison var, let, const :**
```javascript
// VAR - hoisted, initialisée à undefined
console.log(a); // undefined
var a = 1;

// LET - hoisted, mais TDZ (Temporal Dead Zone)
console.log(b); // ReferenceError
let b = 2;

// CONST - hoisted, mais TDZ
console.log(c); // ReferenceError
const c = 3;
```

**Classes :**
```javascript
const user = new User(); // ReferenceError
class User {}

// Classes sont hissées mais dans TDZ, comme let/const
```

**Scope et hoisting :**
```javascript
function exemple() {
  console.log(x); // undefined (x hissée dans le scope de la fonction)
  
  if (true) {
    var x = 10;   // var ignore le block scope
  }
  
  console.log(x); // 10
}

function exemple2() {
  console.log(y); // ReferenceError
  
  if (true) {
    let y = 10;   // let respecte le block scope
  }
  
  console.log(y); // ReferenceError
}
```

**Best practices :**
```javascript
// ✅ Déclarer les variables en haut du scope
function bonneApproche() {
  let x, y, z;
  
  x = 1;
  y = 2;
  z = x + y;
}

// ✅ Utiliser const par défaut, let si nécessaire
const MAX = 100;
let compteur = 0;

// ❌ Éviter var
var ancienCode = 'non recommandé';
```

---

### Q18 : Qu'est-ce que le debouncing et le throttling ? Quelle est la différence ?

**Réponse :**
Ce sont deux techniques pour **limiter le nombre d'exécutions** d'une fonction dans le temps.

**DEBOUNCING** : attend que l'utilisateur ait **fini** d'agir
- Exécute la fonction seulement après un délai sans nouvelle invocation
- Utile pour : recherche en temps réel, redimensionnement de fenêtre, validation de formulaire

```javascript
function debounce(func, delay) {
  let timeoutId;
  
  return function(...args) {
    // Annuler le timeout précédent
    clearTimeout(timeoutId);
    
    // Créer un nouveau timeout
    timeoutId = setTimeout(() => {
      func.apply(this, args);
    }, delay);
  };
}

// Utilisation : recherche
const input = document.querySelector('#search');
const rechercher = debounce((terme) => {
  console.log('Recherche:', terme);
  // Appel API ici
}, 500);

input.addEventListener('input', (e) => {
  rechercher(e.target.value);
});

// Si l'utilisateur tape "react" rapidement :
// r → timeout annulé
// re → timeout annulé
// rea → timeout annulé
// reac → timeout annulé
// react → attend 500ms → EXÉCUTE
```

**THROTTLING** : limite à **une exécution par intervalle**
- Garantit que la fonction s'exécute au maximum une fois par période
- Utile pour : scroll events, mouse move, resize

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

// Utilisation : scroll tracking
const logScroll = throttle(() => {
  console.log('Position:', window.scrollY);
}, 1000);

window.addEventListener('scroll', logScroll);

// Si l'utilisateur scroll continuellement :
// 0ms → EXÉCUTE
// 500ms → ignoré
// 1000ms → EXÉCUTE
// 1500ms → ignoré
// 2000ms → EXÉCUTE
```

**Différence visuelle :**
```javascript
// Événements : ||||||||||||||||||||||||||||

// Sans limitation :
// Exécutions : ||||||||||||||||||||||||||||

// DEBOUNCE (500ms) :
// Exécutions :                            | (à la fin)

// THROTTLE (500ms) :
// Exécutions : |    |    |    |    |    | (régulier)
```

**Comparaison :**
| Aspect | Debounce | Throttle |
|--------|----------|----------|
| Quand | Après l'inactivité | À intervalles réguliers |
| Fréquence | Une fois à la fin | Multiple fois pendant |
| Use case | Recherche, validation | Scroll, resize, drag |

**Implémentation avec leading/trailing :**
```javascript
// Debounce avec option immediate (exécute au début)
function debounceAdvanced(func, delay, immediate = false) {
  let timeoutId;
  
  return function(...args) {
    const callNow = immediate && !timeoutId;
    
    clearTimeout(timeoutId);
    timeoutId = setTimeout(() => {
      timeoutId = null;
      if (!immediate) func.apply(this, args);
    }, delay);
    
    if (callNow) func.apply(this, args);
  };
}

// Throttle avec leading et trailing
function throttleAdvanced(func, delay, options = {}) {
  let timeoutId;
  let lastCall = 0;
  const { leading = true, trailing = true } = options;
  
  return function(...args) {
    const now = Date.now();
    
    if (!lastCall && !leading) lastCall = now;
    
    const remaining = delay - (now - lastCall);
    
    if (remaining <= 0) {
      if (timeoutId) {
        clearTimeout(timeoutId);
        timeoutId = null;
      }
      lastCall = now;
      func.apply(this, args);
    } else if (!timeoutId && trailing) {
      timeoutId = setTimeout(() => {
        lastCall = leading ? Date.now() : 0;
        timeoutId = null;
        func.apply(this, args);
      }, remaining);
    }
  };
}
```

---

### Q19 : Comment gérer les références circulaires en JavaScript ?

**Réponse :**
Les **références circulaires** surviennent quand deux objets se référencent mutuellement.

**Problème :**
```javascript
const obj1 = { nom: 'A' };
const obj2 = { nom: 'B' };

obj1.ref = obj2;
obj2.ref = obj1; // Référence circulaire

// ❌ JSON.stringify échoue
JSON.stringify(obj1); // TypeError: Converting circular structure to JSON
```

**Solution 1 : Utiliser un Set/WeakMap pour tracker :**
```javascript
function stringifyAvecCirculaires(obj) {
  const seen = new WeakSet();
  
  return JSON.stringify(obj, (key, value) => {
    if (typeof value === 'object' && value !== null) {
      if (seen.has(value)) {
        return '[Circular]'; // Remplacer par un marqueur
      }
      seen.add(value);
    }
    return value;
  });
}

const obj1 = { nom: 'A' };
const obj2 = { nom: 'B' };
obj1.ref = obj2;
obj2.ref = obj1;

console.log(stringifyAvecCirculaires(obj1));
// {"nom":"A","ref":{"nom":"B","ref":"[Circular]"}}
```

**Solution 2 : Deep clone avec protection :**
```javascript
function deepCloneAvecCirculaires(obj, hash = new WeakMap()) {
  // Primitives et null
  if (obj === null || typeof obj !== 'object') {
    return obj;
  }
  
  // Déjà cloné (référence circulaire détectée)
  if (hash.has(obj)) {
    return hash.get(obj);
  }
  
  // Cloner
  const clone = Array.isArray(obj) ? [] : {};
  hash.set(obj, clone); // Enregistrer avant de parcourir
  
  for (let key in obj) {
    if (obj.hasOwnProperty(key)) {
      clone[key] = deepCloneAvecCirculaires(obj[key], hash);
    }
  }
  
  return clone;
}

// Test
const original = { nom: 'Parent' };
original.self = original; // Référence circulaire

const copie = deepCloneAvecCirculaires(original);
console.log(copie.nom); // 'Parent'
console.log(copie.self === copie); // true (structure préservée)
```

**Solution 3 : Bibliothèques (flatted, circular-json) :**
```javascript
import { stringify, parse } from 'flatted';

const obj1 = { nom: 'A' };
const obj2 = { nom: 'B' };
obj1.ref = obj2;
obj2.ref = obj1;

const json = stringify(obj1);
const restored = parse(json);
// Préserve les références circulaires
```

**Détection de références circulaires :**
```javascript
function hasCircularReference(obj, seen = new WeakSet()) {
  if (typeof obj !== 'object' || obj === null) {
    return false;
  }
  
  if (seen.has(obj)) {
    return true; // Circulaire détectée !
  }
  
  seen.add(obj);
  
  for (let key in obj) {
    if (obj.hasOwnProperty(key)) {
      if (hasCircularReference(obj[key], seen)) {
        return true;
      }
    }
  }
  
  return false;
}

// Test
const safe = { a: 1, b: { c: 2 } };
console.log(hasCircularReference(safe)); // false

const circular = { a: 1 };
circular.self = circular;
console.log(hasCircularReference(circular)); // true
```

---

### Q20 : Qu'est-ce que le "type coercion" et donnez des exemples de conversions implicites.

**Réponse :**
Le **type coercion** est la conversion automatique (implicite) d'un type vers un autre par JavaScript.

**1. String coercion (avec +) :**
```javascript
// Si un opérande est string, l'autre est converti en string
'5' + 3;          // '53' (number → string)
'Hello' + true;   // 'Hellotrue'
'Value: ' + null; // 'Value: null'
'Sum: ' + [1,2];  // 'Sum: 1,2'

// Autres opérateurs convertissent en number
'5' - 3;          // 2 (string → number)
'5' * 2;          // 10
'10' / 2;         // 5
```

**2. Number coercion :**
```javascript
// Unary + convertit en number
+'5';             // 5
+true;            // 1
+false;           // 0
+null;            // 0
+undefined;       // NaN
+'hello';         // NaN

// Opérateurs arithmétiques
'5' - 2;          // 3
'5' * '2';        // 10
true + true;      // 2 (true → 1)
false + 5;        // 5 (false → 0)
```

**3. Boolean coercion :**
```javascript
// Dans if, while, !, &&, ||
if ('hello') {}      // true (non-empty string)
if (0) {}            // false
if ([]) {}           // true (même vide !)

// Logical operators
'hello' && 5;        // 5 (retourne la valeur, pas boolean)
0 || 'default';      // 'default'
!!0;                 // false (double négation)
```

**4. Cas particuliers :**
```javascript
// Array vers string (join implicite)
[1, 2, 3] + '';      // '1,2,3'
[] + [];             // '' (empty string)
[] + {};             // '[object Object]'
{} + [];             // 0 (!) - {} interprété comme bloc

// Object vers primitive
const obj = { 
  valueOf() { return 42; },
  toString() { return 'hello'; }
};
obj + 1;             // 43 (valueOf utilisé)
String(obj);         // 'hello' (toString utilisé)

// Comparaisons
null == undefined;   // true
null == 0;           // false
'' == 0;             // true ('' → 0)
'0' == false;        // true ('0' → 0, false → 0)
```

**5. Règles de conversion vers primitives :**
```javascript
// ToPrimitive algorithm
// 1. Si déjà primitive, retourner
// 2. Appeler valueOf()
// 3. Appeler toString()
// 4. TypeError si aucun ne retourne primitive

const obj = {
  valueOf() {
    console.log('valueOf appelé');
    return 10;
  },
  toString() {
    console.log('toString appelé');
    return '20';
  }
};

console.log(obj + 5);  // valueOf appelé → 15
console.log(`${obj}`); // toString appelé → '20'
```

**Pièges courants :**
```javascript
// Addition vs concaténation
1 + 2 + '3';         // '33' ((1+2) + '3')
'3' + 1 + 2;         // '312' (('3'+1) + 2)

// Comparaison
[] == ![];           // true (bizarre !)
// [] → '' → 0, ![] → false → 0, donc 0 == 0

// Array avec un élément
[5] == 5;            // true ([5] → '5' → 5)
[5,6] == '5,6';      // true

// Null et undefined
null + 1;            // 1 (null → 0)
undefined + 1;       // NaN (undefined → NaN)
```

**Best practices pour éviter les pièges :**
```javascript
// ✅ Conversion explicite
Number('5') + 3;     // 8
String(5) + '3';     // '53'
Boolean(0);          // false

// ✅ Utiliser ===
5 === '5';           // false (pas de coercion)

// ✅ ParseInt avec radix
parseInt('10', 10);  // 10
parseInt('10', 2);   // 2 (binaire)

// ❌ Éviter les conversions implicites ambiguës
if (value) {}        // OK pour truthy/falsy
if (value == true) {} // ❌ Dangereux
if (value === true) {} // ✅ Explicite
```

---

Voilà **20 questions/réponses** complètes pour le **Chapitre 17 : Concepts Avancés** ! Ces questions couvrent :
- Spread/Rest operators
- Short-circuit evaluation (&&, ||, ??)
- Optional chaining (?.)
- Error handling (try/catch/finally, throw)
- Strict mode
- JSON (stringify/parse)
- Deep copy
- Truthy/Falsy
- == vs ===
- Hoisting
- Debouncing/Throttling
- Références circulaires
- Type coercion

Tu veux :
- Plus de questions sur ce chapitre ?
- Questions sur un autre chapitre (DOM, OOP, Modules, etc.) ?
- Des exercices pratiques de coding ?