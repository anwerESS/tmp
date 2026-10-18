## [**..**](./00_index.md)

# Chapitre 14 : Generators

## 14.1 Introduction aux Generators

Les **generators** sont des fonctions spéciales qui peuvent être mises en pause et reprises. Elles permettent de produire une séquence de valeurs de manière paresseuse (lazy evaluation).

### Syntaxe `function*`

```javascript
// Déclaration d'un generator avec function*
function* simpleGenerator() {
    yield 1;
    yield 2;
    yield 3;
}

// Créer une instance du generator
const gen = simpleGenerator();

// Les generators retournent un iterator
console.log(gen.next()); // { value: 1, done: false }
console.log(gen.next()); // { value: 2, done: false }
console.log(gen.next()); // { value: 3, done: false }
console.log(gen.next()); // { value: undefined, done: true }

// Une fois terminé, toujours done: true
console.log(gen.next()); // { value: undefined, done: true }

// Expression de generator
const generatorExpr = function* () {
    yield 'a';
    yield 'b';
};

// Generator method dans un objet
const obj = {
    *myGenerator() {
        yield 1;
        yield 2;
    }
};

// Generator dans une classe
class MyClass {
    *generate() {
        yield 'hello';
        yield 'world';
    }
}

const instance = new MyClass();
const classGen = instance.generate();
console.log(classGen.next()); // { value: 'hello', done: false }

// Différence avec fonction normale
function normalFunction() {
    return [1, 2, 3]; // Retourne tout d'un coup
}

function* generatorFunction() {
    yield 1; // Retourne une valeur à la fois
    yield 2;
    yield 3;
}

// La fonction normale s'exécute complètement
const result = normalFunction();
console.log(result); // [1, 2, 3]

// Le generator se met en pause à chaque yield
const genResult = generatorFunction();
console.log(genResult.next().value); // 1
console.log(genResult.next().value); // 2
```

### Yield Keyword

```javascript
// yield retourne une valeur et met en pause
function* countToFive() {
    console.log('Start');
    yield 1;
    console.log('After 1');
    yield 2;
    console.log('After 2');
    yield 3;
    console.log('After 3');
    yield 4;
    console.log('After 4');
    yield 5;
    console.log('End');
}

const counter = countToFive();

console.log(counter.next()); 
// Log: "Start"
// Return: { value: 1, done: false }

console.log(counter.next()); 
// Log: "After 1"
// Return: { value: 2, done: false }

console.log(counter.next()); 
// Log: "After 2"
// Return: { value: 3, done: false }

// yield peut retourner n'importe quelle valeur
function* mixedValues() {
    yield 42;
    yield 'hello';
    yield { name: 'John' };
    yield [1, 2, 3];
    yield true;
    yield null;
}

const mixed = mixedValues();
console.log(mixed.next().value); // 42
console.log(mixed.next().value); // "hello"
console.log(mixed.next().value); // { name: 'John' }

// yield avec expressions
function* withExpressions() {
    yield 1 + 1;           // 2
    yield Math.random();   // Nombre aléatoire
    yield new Date();      // Date actuelle
    
    const x = 10;
    yield x * 2;           // 20
}

// yield sans valeur (undefined)
function* noValue() {
    yield;
    yield;
    yield;
}

const nv = noValue();
console.log(nv.next()); // { value: undefined, done: false }

// return dans un generator
function* withReturn() {
    yield 1;
    yield 2;
    return 'finished'; // Valeur finale
    yield 3; // Jamais atteint
}

const wr = withReturn();
console.log(wr.next()); // { value: 1, done: false }
console.log(wr.next()); // { value: 2, done: false }
console.log(wr.next()); // { value: 'finished', done: true }
console.log(wr.next()); // { value: undefined, done: true }
```

### Iterator Protocol

```javascript
// Les generators implémentent l'iterator protocol
function* myGenerator() {
    yield 1;
    yield 2;
    yield 3;
}

const gen = myGenerator();

// Un generator a une méthode next()
console.log(typeof gen.next); // "function"

// Un generator est iterable (a Symbol.iterator)
console.log(typeof gen[Symbol.iterator]); // "function"

// Le generator lui-même est son propre iterator
console.log(gen[Symbol.iterator]() === gen); // true

// Peut être utilisé avec for...of
function* colors() {
    yield 'red';
    yield 'green';
    yield 'blue';
}

for (const color of colors()) {
    console.log(color);
}
// "red"
// "green"
// "blue"

// Peut être utilisé avec spread operator
const colorArray = [...colors()];
console.log(colorArray); // ['red', 'green', 'blue']

// Peut être utilisé avec destructuring
const [first, second, third] = colors();
console.log(first);  // 'red'
console.log(second); // 'green'
console.log(third);  // 'blue'

// Peut être utilisé avec Array.from()
const arr = Array.from(colors());
console.log(arr); // ['red', 'green', 'blue']

// Créer un objet iterable custom
const iterableObject = {
    *[Symbol.iterator]() {
        yield 1;
        yield 2;
        yield 3;
    }
};

for (const num of iterableObject) {
    console.log(num);
}
// 1
// 2
// 3

console.log([...iterableObject]); // [1, 2, 3]

// Exemple: Range generator (comme Python)
function* range(start, end, step = 1) {
    for (let i = start; i < end; i += step) {
        yield i;
    }
}

console.log([...range(0, 10)]); // [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
console.log([...range(0, 10, 2)]); // [0, 2, 4, 6, 8]

for (const num of range(5, 10)) {
    console.log(num); // 5, 6, 7, 8, 9
}

// Exemple: Infinite sequence
function* fibonacci() {
    let a = 0, b = 1;
    while (true) {
        yield a;
        [a, b] = [b, a + b];
    }
}

// Prendre seulement les 10 premiers
const fib = fibonacci();
const first10 = [];
for (let i = 0; i < 10; i++) {
    first10.push(fib.next().value);
}
console.log(first10); 
// [0, 1, 1, 2, 3, 5, 8, 13, 21, 34]
```

---

## 14.2 Utilisation Pratique

### `next()` Method

```javascript
// next() retourne un objet { value, done }
function* simple() {
    yield 'a';
    yield 'b';
    yield 'c';
}

const gen = simple();

// Première appel
const result1 = gen.next();
console.log(result1); // { value: 'a', done: false }
console.log(result1.value); // 'a'
console.log(result1.done);  // false

// Deuxième appel
const result2 = gen.next();
console.log(result2); // { value: 'b', done: false }

// Appels suivants...
console.log(gen.next()); // { value: 'c', done: false }
console.log(gen.next()); // { value: undefined, done: true }

// Boucle manuelle avec next()
function* numbers() {
    yield 1;
    yield 2;
    yield 3;
}

const numGen = numbers();
let result = numGen.next();

while (!result.done) {
    console.log(result.value);
    result = numGen.next();
}
// 1
// 2
// 3

// Vérifier si generator est terminé
function isGeneratorDone(generator) {
    const result = generator.next();
    return result.done;
}
```

### Passing Values to Generators

```javascript
// next() peut passer une valeur au generator
function* echo() {
    const value1 = yield 'Ready';
    console.log('Received:', value1);
    
    const value2 = yield 'Waiting';
    console.log('Received:', value2);
    
    const value3 = yield 'Last';
    console.log('Received:', value3);
}

const echoGen = echo();

console.log(echoGen.next());        // { value: 'Ready', done: false }
console.log(echoGen.next('Hello')); // Log: "Received: Hello"
                                     // { value: 'Waiting', done: false }
console.log(echoGen.next('World')); // Log: "Received: World"
                                     // { value: 'Last', done: false }
console.log(echoGen.next('!'));     // Log: "Received: !"
                                     // { value: undefined, done: true }

// ⚠️ Premier next() ne peut pas passer de valeur
function* test() {
    const x = yield 1;
    console.log('x:', x);
}

const t = test();
console.log(t.next('ignored')); // { value: 1, done: false }
                                 // 'ignored' est ignoré
console.log(t.next('received')); // Log: "x: received"
                                  // { value: undefined, done: true }

// Exemple: Generator qui accumule
function* accumulator() {
    let total = 0;
    
    while (true) {
        const value = yield total;
        if (value !== undefined) {
            total += value;
        }
    }
}

const acc = accumulator();
console.log(acc.next());      // { value: 0, done: false }
console.log(acc.next(5));     // { value: 5, done: false }
console.log(acc.next(10));    // { value: 15, done: false }
console.log(acc.next(3));     // { value: 18, done: false }

// Exemple: Generator avec état
function* stateMachine() {
    let state = 'idle';
    
    while (true) {
        const action = yield state;
        
        switch (action) {
            case 'start':
                state = 'running';
                break;
            case 'pause':
                state = 'paused';
                break;
            case 'stop':
                state = 'stopped';
                return state;
            default:
                state = 'idle';
        }
    }
}

const machine = stateMachine();
console.log(machine.next());          // { value: 'idle', done: false }
console.log(machine.next('start'));   // { value: 'running', done: false }
console.log(machine.next('pause'));   // { value: 'paused', done: false }
console.log(machine.next('stop'));    // { value: 'stopped', done: true }

// Generator bidirectionnel
function* conversation() {
    const name = yield 'What is your name?';
    const age = yield `Hello ${name}, what is your age?`;
    yield `${name} is ${age} years old`;
}

const conv = conversation();
console.log(conv.next().value);        // "What is your name?"
console.log(conv.next('John').value);  // "Hello John, what is your age?"
console.log(conv.next(30).value);      // "John is 30 years old"
```

### `yield*` Delegation

```javascript
// yield* délègue à un autre generator ou iterable
function* generator1() {
    yield 1;
    yield 2;
}

function* generator2() {
    yield 'a';
    yield* generator1(); // Déléguer à generator1
    yield 'b';
}

const gen = generator2();
console.log([...gen]); // ['a', 1, 2, 'b']

// yield* avec array (iterable)
function* withArray() {
    yield 'start';
    yield* [1, 2, 3]; // Déléguer à l'array
    yield 'end';
}

console.log([...withArray()]); 
// ['start', 1, 2, 3, 'end']

// yield* avec string (iterable)
function* withString() {
    yield* 'hello';
}

console.log([...withString()]); 
// ['h', 'e', 'l', 'l', 'o']

// yield* retourne la valeur de return
function* inner() {
    yield 1;
    yield 2;
    return 'finished';
}

function* outer() {
    const result = yield* inner();
    console.log('Inner returned:', result);
    yield 3;
}

const o = outer();
console.log(o.next()); // { value: 1, done: false }
console.log(o.next()); // { value: 2, done: false }
console.log(o.next()); // Log: "Inner returned: finished"
                        // { value: 3, done: false }

// Composition de generators
function* numbers() {
    yield 1;
    yield 2;
    yield 3;
}

function* letters() {
    yield 'a';
    yield 'b';
    yield 'c';
}

function* combined() {
    yield* numbers();
    yield* letters();
}

console.log([...combined()]); 
// [1, 2, 3, 'a', 'b', 'c']

// Parcours d'arbre avec yield*
const tree = {
    value: 1,
    children: [
        {
            value: 2,
            children: [
                { value: 4, children: [] },
                { value: 5, children: [] }
            ]
        },
        {
            value: 3,
            children: [
                { value: 6, children: [] }
            ]
        }
    ]
};

function* traverseTree(node) {
    yield node.value;
    
    for (const child of node.children) {
        yield* traverseTree(child); // Récursif avec yield*
    }
}

console.log([...traverseTree(tree)]); 
// [1, 2, 4, 5, 3, 6]

// Flatten array avec yield*
function* flatten(array) {
    for (const item of array) {
        if (Array.isArray(item)) {
            yield* flatten(item); // Récursif
        } else {
            yield item;
        }
    }
}

const nested = [1, [2, [3, 4], 5], 6, [7, 8]];
console.log([...flatten(nested)]); 
// [1, 2, 3, 4, 5, 6, 7, 8]
```

### Cas d'Usage (Lazy Evaluation, Infinite Sequences)

```javascript
// LAZY EVALUATION - Calcul à la demande

// Mauvaise approche: Tout calculer d'avance
function eagerRange(n) {
    const result = [];
    for (let i = 0; i < n; i++) {
        result.push(i * i); // Calcule tout immédiatement
    }
    return result;
}

const eager = eagerRange(1000000); // Consomme beaucoup de mémoire
console.log(eager[0]); // Utilise seulement le premier

// Bonne approche: Generator (lazy)
function* lazyRange(n) {
    for (let i = 0; i < n; i++) {
        yield i * i; // Calcule seulement quand demandé
    }
}

const lazy = lazyRange(1000000); // Pas de calcul encore
console.log(lazy.next().value); // Calcule seulement le premier

// INFINITE SEQUENCES

// Séquence infinie de nombres
function* infiniteSequence(start = 0) {
    let current = start;
    while (true) {
        yield current++;
    }
}

const seq = infiniteSequence(1);
console.log(seq.next().value); // 1
console.log(seq.next().value); // 2
console.log(seq.next().value); // 3

// Fibonacci infini
function* fibonacciInfinite() {
    let [a, b] = [0, 1];
    while (true) {
        yield a;
        [a, b] = [b, a + b];
    }
}

// Prendre N premiers éléments
function take(generator, n) {
    const result = [];
    for (let i = 0; i < n; i++) {
        const { value, done } = generator.next();
        if (done) break;
        result.push(value);
    }
    return result;
}

const fib = fibonacciInfinite();
console.log(take(fib, 10)); 
// [0, 1, 1, 2, 3, 5, 8, 13, 21, 34]

// Nombres premiers infinis
function* primes() {
    yield 2;
    
    const isPrime = (num) => {
        for (let i = 2; i <= Math.sqrt(num); i++) {
            if (num % i === 0) return false;
        }
        return true;
    };
    
    let num = 3;
    while (true) {
        if (isPrime(num)) {
            yield num;
        }
        num += 2; // Sauter nombres pairs
    }
}

const primeGen = primes();
console.log(take(primeGen, 10)); 
// [2, 3, 5, 7, 11, 13, 17, 19, 23, 29]

// ID unique generator
function* idGenerator() {
    let id = 1;
    while (true) {
        yield id++;
    }
}

const ids = idGenerator();
console.log(ids.next().value); // 1
console.log(ids.next().value); // 2
console.log(ids.next().value); // 3

// Pagination lazy loading
function* paginatedData(fetchPage) {
    let page = 1;
    
    while (true) {
        const data = fetchPage(page);
        if (data.length === 0) break;
        
        yield* data; // Déléguer à l'array
        page++;
    }
}

// Simulation de fetch
function mockFetchPage(page) {
    if (page > 3) return [];
    return Array.from({ length: 10 }, (_, i) => `Item ${(page - 1) * 10 + i + 1}`);
}

const paginated = paginatedData(mockFetchPage);
console.log(take(paginated, 25)); 
// Items 1-25 (3 pages chargées)

// Stream de données
function* dataStream(data) {
    for (const item of data) {
        // Traitement lazy
        const processed = item.toUpperCase();
        yield processed;
    }
}

const stream = dataStream(['hello', 'world', 'javascript']);
console.log([...stream]); 
// ['HELLO', 'WORLD', 'JAVASCRIPT']

// Pipeline de transformations
function* map(iterable, fn) {
    for (const item of iterable) {
        yield fn(item);
    }
}

function* filter(iterable, predicate) {
    for (const item of iterable) {
        if (predicate(item)) {
            yield item;
        }
    }
}

// Chaîner les transformations
const nums = infiniteSequence(1);
const doubled = map(nums, x => x * 2);
const evens = filter(doubled, x => x % 4 === 0);

console.log(take(evens, 5)); 
// [4, 8, 12, 16, 20]

// Random number generator
function* randomNumbers(min = 0, max = 100) {
    while (true) {
        yield Math.floor(Math.random() * (max - min + 1)) + min;
    }
}

const random = randomNumbers(1, 10);
console.log(take(random, 5)); 
// [7, 3, 9, 1, 5] (exemple)

// Date sequence
function* dateSequence(start, step = 1) {
    const current = new Date(start);
    
    while (true) {
        yield new Date(current);
        current.setDate(current.getDate() + step);
    }
}

const dates = dateSequence('2024-11-01');
const next7Days = take(dates, 7);
console.log(next7Days.map(d => d.toLocaleDateString()));

// Retry mechanism avec backoff
function* retryWithBackoff(maxRetries = 3) {
    let delay = 1000;
    
    for (let i = 0; i < maxRetries; i++) {
        yield delay;
        delay *= 2; // Exponentiel backoff
    }
}

async function fetchWithRetry(url) {
    const retries = retryWithBackoff(5);
    
    for (const delay of retries) {
        try {
            const response = await fetch(url);
            if (response.ok) return response;
        } catch (error) {
            console.log(`Retry after ${delay}ms`);
            await new Promise(resolve => setTimeout(resolve, delay));
        }
    }
    
    throw new Error('Max retries reached');
}
```

---

## Exemples Pratiques Avancés

### Iterator Helpers

```javascript
// Créer helpers pour manipuler generators

// take - Prendre N premiers éléments
function* take(iterable, n) {
    let count = 0;
    for (const item of iterable) {
        if (count++ >= n) break;
        yield item;
    }
}

// skip - Sauter N premiers éléments
function* skip(iterable, n) {
    let count = 0;
    for (const item of iterable) {
        if (count++ < n) continue;
        yield item;
    }
}

// map - Transformer chaque élément
function* map(iterable, fn) {
    for (const item of iterable) {
        yield fn(item);
    }
}

// filter - Filtrer éléments
function* filter(iterable, predicate) {
    for (const item of iterable) {
        if (predicate(item)) {
            yield item;
        }
    }
}

// reduce (pas generator, consomme tout)
function reduce(iterable, reducer, initialValue) {
    let accumulator = initialValue;
    
    for (const item of iterable) {
        accumulator = reducer(accumulator, item);
    }
    
    return accumulator;
}

// chunk - Grouper par N éléments
function* chunk(iterable, size) {
    let batch = [];
    
    for (const item of iterable) {
        batch.push(item);
        
        if (batch.length === size) {
            yield batch;
            batch = [];
        }
    }
    
    if (batch.length > 0) {
        yield batch;
    }
}

// zip - Combiner plusieurs iterables
function* zip(...iterables) {
    const iterators = iterables.map(it => it[Symbol.iterator]());
    
    while (true) {
        const results = iterators.map(it => it.next());
        
        if (results.some(r => r.done)) break;
        
        yield results.map(r => r.value);
    }
}

// Utilisation combinée
function* numbers() {
    let i = 1;
    while (true) yield i++;
}

const result = take(
    filter(
        map(numbers(), x => x * 2),
        x => x % 3 === 0
    ),
    5
);

console.log([...result]); 
// [6, 12, 18, 24, 30]

// Exemple avec chunk
const data = range(1, 11);
const chunked = chunk(data, 3);
console.log([...chunked]); 
// [[1, 2, 3], [4, 5, 6], [7, 8, 9], [10]]

// Exemple avec zip
const names = ['Alice', 'Bob', 'Charlie'];
const ages = [25, 30, 35];
const cities = ['Paris', 'London', 'Tokyo'];

const zipped = zip(names, ages, cities);
console.log([...zipped]); 
// [['Alice', 25, 'Paris'], ['Bob', 30, 'London'], ['Charlie', 35, 'Tokyo']]
```

### Async Generators

```javascript
// Generator asynchrone avec async function*
async function* asyncGenerator() {
    yield await Promise.resolve(1);
    yield await Promise.resolve(2);
    yield await Promise.resolve(3);
}

// Utilisation avec for await...of
(async () => {
    for await (const value of asyncGenerator()) {
        console.log(value);
    }
})();
// 1
// 2
// 3

// Fetch data avec pagination asynchrone
async function* fetchPages(baseUrl, maxPages = 5) {
    for (let page = 1; page <= maxPages; page++) {
        const response = await fetch(`${baseUrl}?page=${page}`);
        const data = await response.json();
        
        if (data.length === 0) break;
        
        yield* data; // Déléguer aux items
    }
}

// Utilisation
(async () => {
    const pages = fetchPages('/api/users', 3);
    
    for await (const user of pages) {
        console.log(user);
    }
})();

// Stream de données en temps réel
async function* eventStream(eventSource) {
    while (true) {
        const event = await new Promise(resolve => {
            eventSource.once('data', resolve);
        });
        
        yield event;
    }
}

// Polling avec délai
async function* poll(fn, interval = 1000) {
    while (true) {
        yield await fn();
        await new Promise(resolve => setTimeout(resolve, interval));
    }
}

// Utilisation
(async () => {
    const poller = poll(() => fetch('/api/status').then(r => r.json()), 5000);
    
    for await (const status of poller) {
        console.log('Status:', status);
        
        if (status.complete) break;
    }
})();
```

### State Machines

```javascript
// Implémentation d'une state machine avec generator
function* trafficLight() {
    while (true) {
        console.log('🔴 Red');
        yield 'red';
        
        console.log('🟢 Green');
        yield 'green';
        
        console.log('🟡 Yellow');
        yield 'yellow';
    }
}

const light = trafficLight();

// Changer état
setInterval(() => {
    light.next();
}, 2000);

// State machine plus complexe
function* orderStateMachine() {
    console.log('State: PENDING');
    const paymentAction = yield 'PENDING';
    
    if (paymentAction === 'PAY') {
        console.log('State: PAID');
        const shipAction = yield 'PAID';
        
        if (shipAction === 'SHIP') {
            console.log('State: SHIPPED');
            const deliverAction = yield 'SHIPPED';
            
            if (deliverAction === 'DELIVER') {
                console.log('State: DELIVERED');
                yield 'DELIVERED';
            }
        }
    } else if (paymentAction === 'CANCEL') {
        console.log('State: CANCELLED');
        yield 'CANCELLED';
    }
}

const order = orderStateMachine();
console.log(order.next());           // { value: 'PENDING', done: false }
console.log(order.next('PAY'));      // { value: 'PAID', done: false }
console.log(order.next('SHIP'));     // { value: 'SHIPPED', done: false }
console.log(order.next('DELIVER'));  // { value: 'DELIVERED', done: false }
```

### Testing et Error Handling

```javascript
// throw() method - Lancer une erreur dans generator
function* errorHandling() {
    try {
        yield 1;
        yield 2;
        yield 3;
    } catch (error) {
        console.log('Caught:', error.message);
        yield 'error handled';
    }
}

const eh = errorHandling();
console.log(eh.next());                // { value: 1, done: false }
console.log(eh.throw(new Error('Oops'))); // Log: "Caught: Oops"
                                           // { value: 'error handled', done: false }

// return() method - Terminer generator prématurément
function* withReturn() {
    try {
        yield 1;
        yield 2;
        yield 3;
    } finally {
        console.log('Cleanup');
    }
}

const wr = withReturn();
console.log(wr.next());     // { value: 1, done: false }
console.log(wr.return(99)); // Log: "Cleanup"
                             // { value: 99, done: true }
console.log(wr.next());     // { value: undefined, done: true }

// Generator avec try/finally pour cleanup
function* withCleanup(resource) {
    try {
        console.log('Opening resource');
        yield resource;
        console.log('Using resource');
    } finally {
        console.log('Closing resource');
    }
}

const wc = withCleanup('file.txt');
console.log(wc.next());  // "Opening resource"
wc.return();              // "Closing resource"
```

---

**Points clés à retenir:**
- Generators = fonctions pausables avec `function*`
- `yield` retourne valeur et met en pause
- `next()` reprend exécution
- Lazy evaluation = calcul à la demande
- Séquences infinies possibles
- `yield*` pour délégation
- Iterable protocol (for...of, spread)
- Utile pour: pagination, streams, state machines

**Pour React:**
- Moins utilisé directement
- Compréhension importante pour Redux Saga
- Concepts similaires avec async/await
- Utile pour data streaming

---

# Questions/Réponses - Chapitre 14 : Generators

## Questions Niveau Junior

### Q1: Qu'est-ce qu'un generator et comment le déclarer ?

**Réponse:**
```javascript
// Un generator est une fonction spéciale qui peut être mise en pause 
// et reprises, et qui produit une séquence de valeurs

// Déclaration avec function* (noter l'astérisque)
function* myGenerator() {
    yield 1;
    yield 2;
    yield 3;
}

// Créer une instance du generator
const gen = myGenerator();

// Appeler next() pour obtenir la prochaine valeur
console.log(gen.next()); // { value: 1, done: false }
console.log(gen.next()); // { value: 2, done: false }
console.log(gen.next()); // { value: 3, done: false }
console.log(gen.next()); // { value: undefined, done: true }

// Différence avec fonction normale
function normalFunction() {
    return [1, 2, 3]; // Retourne tout d'un coup
}

function* generatorFunction() {
    yield 1; // Retourne une valeur à la fois
    yield 2;
    yield 3;
}

// Fonction normale s'exécute complètement
const result = normalFunction();
console.log(result); // [1, 2, 3] (immédiatement)

// Generator produit valeurs à la demande
const genResult = generatorFunction();
console.log(genResult.next().value); // 1
console.log(genResult.next().value); // 2
// Peut s'arrêter ici, 3 n'est jamais calculé si pas demandé

// Autres syntaxes de déclaration

// Expression de generator
const genExpr = function* () {
    yield 'a';
    yield 'b';
};

// Method dans objet
const obj = {
    *myGen() {
        yield 1;
        yield 2;
    }
};

const objGen = obj.myGen();
console.log(objGen.next().value); // 1

// Method dans classe
class MyClass {
    *generate() {
        yield 'hello';
        yield 'world';
    }
}

const instance = new MyClass();
const classGen = instance.generate();
console.log(classGen.next().value); // "hello"
```

**Points clés:**
- Déclaration avec `function*` (astérisque)
- Utilise `yield` pour retourner valeurs
- Appeler `next()` pour avancer
- Retourne `{ value, done }`
- Exécution paresseuse (lazy)
- Peut être mis en pause et repris

---

### Q2: À quoi sert le mot-clé `yield` ?

**Réponse:**
```javascript
// yield retourne une valeur et MET EN PAUSE le generator

function* counter() {
    console.log('Start');
    yield 1;
    console.log('After 1');
    yield 2;
    console.log('After 2');
    yield 3;
    console.log('End');
}

const gen = counter();

console.log(gen.next()); 
// Log: "Start"
// Return: { value: 1, done: false }
// S'ARRÊTE ici jusqu'au prochain next()

console.log(gen.next()); 
// Log: "After 1"
// Return: { value: 2, done: false }
// S'ARRÊTE encore

console.log(gen.next()); 
// Log: "After 2"
// Return: { value: 3, done: false }

console.log(gen.next()); 
// Log: "End"
// Return: { value: undefined, done: true }

// yield peut retourner n'importe quelle valeur
function* mixed() {
    yield 42;              // Number
    yield 'hello';         // String
    yield { x: 1 };        // Object
    yield [1, 2, 3];       // Array
    yield true;            // Boolean
    yield null;            // Null
}

const m = mixed();
console.log(m.next().value); // 42
console.log(m.next().value); // "hello"
console.log(m.next().value); // { x: 1 }

// yield avec expressions
function* withExpressions() {
    yield 1 + 1;           // 2
    yield Math.random();   // Calculé à l'appel de next()
    
    const x = 10;
    yield x * 2;           // 20
}

const we = withExpressions();
console.log(we.next().value); // 2
console.log(we.next().value); // Nombre aléatoire

// yield sans valeur (retourne undefined)
function* noValue() {
    yield;
    yield;
    yield;
}

const nv = noValue();
console.log(nv.next()); // { value: undefined, done: false }

// return termine le generator
function* withReturn() {
    yield 1;
    yield 2;
    return 'finished'; // Termine le generator
    yield 3; // Jamais atteint!
}

const wr = withReturn();
console.log(wr.next()); // { value: 1, done: false }
console.log(wr.next()); // { value: 2, done: false }
console.log(wr.next()); // { value: 'finished', done: true }
console.log(wr.next()); // { value: undefined, done: true }

// yield dans boucle
function* countdown(n) {
    while (n > 0) {
        yield n;
        n--;
    }
}

console.log([...countdown(5)]); // [5, 4, 3, 2, 1]

// yield conditionnellement
function* evenNumbers(max) {
    for (let i = 0; i <= max; i++) {
        if (i % 2 === 0) {
            yield i;
        }
    }
}

console.log([...evenNumbers(10)]); // [0, 2, 4, 6, 8, 10]
```

**Points clés:**
- `yield` = retourner valeur + pause
- Reprend à la ligne après yield au prochain next()
- Peut yielder n'importe quelle valeur
- `return` termine le generator
- yield peut être dans boucles/conditions

---

### Q3: Comment utiliser un generator avec `for...of` ?

**Réponse:**
```javascript
// Les generators sont ITERABLES, donc compatibles for...of

function* colors() {
    yield 'red';
    yield 'green';
    yield 'blue';
}

// Utiliser avec for...of
for (const color of colors()) {
    console.log(color);
}
// "red"
// "green"
// "blue"

// Équivalent avec next() (plus verbeux)
const gen = colors();
let result = gen.next();
while (!result.done) {
    console.log(result.value);
    result = gen.next();
}

// Les generators sont aussi compatibles avec:

// 1. Spread operator
const colorArray = [...colors()];
console.log(colorArray); // ['red', 'green', 'blue']

// 2. Destructuring
const [first, second, third] = colors();
console.log(first);  // 'red'
console.log(second); // 'green'
console.log(third);  // 'blue'

// 3. Array.from()
const arr = Array.from(colors());
console.log(arr); // ['red', 'green', 'blue']

// Pourquoi ça marche?
// Generators implémentent Symbol.iterator
function* myGen() {
    yield 1;
    yield 2;
}

const g = myGen();
console.log(typeof g[Symbol.iterator]); // "function"
console.log(g[Symbol.iterator]() === g); // true (le generator est son propre iterator)

// Exemple pratique: Itérer sur range
function* range(start, end, step = 1) {
    for (let i = start; i < end; i += step) {
        yield i;
    }
}

// Facile à utiliser avec for...of
for (const num of range(0, 10, 2)) {
    console.log(num);
}
// 0, 2, 4, 6, 8

// Ou avec spread
console.log([...range(5, 10)]); // [5, 6, 7, 8, 9]

// Exemple: Filtrer en temps réel
function* filterEven(numbers) {
    for (const num of numbers) {
        if (num % 2 === 0) {
            yield num;
        }
    }
}

const nums = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
for (const even of filterEven(nums)) {
    console.log(even); // 2, 4, 6, 8, 10
}

// Créer objet iterable custom
const iterableObj = {
    *[Symbol.iterator]() {
        yield 'a';
        yield 'b';
        yield 'c';
    }
};

for (const letter of iterableObj) {
    console.log(letter); // a, b, c
}

console.log([...iterableObj]); // ['a', 'b', 'c']

// ⚠️ ATTENTION: Le generator est consommé après for...of
const gen2 = colors();
for (const color of gen2) {
    console.log(color); // red, green, blue
}

// Réutiliser le même generator ne fonctionne pas
for (const color of gen2) {
    console.log(color); // Rien! (déjà consommé)
}

// Il faut créer une nouvelle instance
for (const color of colors()) {
    console.log(color); // red, green, blue (nouvelle instance)
}
```

**Points clés:**
- Generators implémentent iterator protocol
- Compatible avec `for...of`, spread, destructuring
- `Symbol.iterator` retourne le generator lui-même
- Generator consommé après itération
- Créer nouvelle instance pour réutiliser

---

### Q4: Quelle est la différence entre un generator et un array ?

**Réponse:**
```javascript
// ARRAY - Toutes les valeurs en mémoire immédiatement
function createArray(n) {
    const arr = [];
    for (let i = 0; i < n; i++) {
        arr.push(i * i); // Calcule TOUT immédiatement
    }
    return arr;
}

const bigArray = createArray(1000000); 
// ❌ 1 million de valeurs en mémoire!
// ❌ Calcul fait même si on n'utilise pas tout

console.log(bigArray[0]); // Utilise seulement la première
// Les 999,999 autres valeurs gaspillent de la mémoire

// GENERATOR - Calcul à la demande (lazy)
function* createGenerator(n) {
    for (let i = 0; i < n; i++) {
        yield i * i; // Calcule SEULEMENT quand demandé
    }
}

const bigGenerator = createGenerator(1000000);
// ✅ Aucun calcul fait encore!
// ✅ Pas de mémoire utilisée

console.log(bigGenerator.next().value); // 0 (calcule seulement celui-ci)
console.log(bigGenerator.next().value); // 1 (calcule seulement celui-ci)
// Les autres ne sont jamais calculés si pas demandés

// COMPARAISON PRATIQUE

// Array: Charge tout en mémoire
function getAllUsers() {
    return [
        { id: 1, name: 'Alice' },
        { id: 2, name: 'Bob' },
        // ... 10,000 users
    ];
}

const users = getAllUsers(); // Tout en mémoire d'un coup
console.log(users[0]); // Utilise seulement le premier

// Generator: Charge à la demande
function* getUsersLazy() {
    // Pourrait fetch depuis DB un par un
    yield { id: 1, name: 'Alice' };
    yield { id: 2, name: 'Bob' };
    // ...
}

const usersGen = getUsersLazy();
console.log(usersGen.next().value); // Charge seulement le premier

// SÉQUENCES INFINIES

// Array: IMPOSSIBLE (mémoire infinie!)
// const infiniteArray = [1, 2, 3, ...]; // ❌ Impossible

// Generator: POSSIBLE!
function* infiniteSequence() {
    let i = 0;
    while (true) {
        yield i++;
    }
}

const infinite = infiniteSequence();
console.log(infinite.next().value); // 0
console.log(infinite.next().value); // 1
console.log(infinite.next().value); // 2
// Peut continuer indéfiniment sans problème!

// PERFORMANCE

console.time('Array');
const arr = createArray(1000000);
console.timeEnd('Array'); // ~100ms + beaucoup de mémoire

console.time('Generator');
const gen = createGenerator(1000000);
console.timeEnd('Generator'); // ~0ms (instantané!)

// Utiliser seulement 10 premiers

// Array: Gaspille 999,990 calculs
const arr10 = createArray(1000000).slice(0, 10);

// Generator: Calcule seulement 10
function take(generator, n) {
    const result = [];
    for (let i = 0; i < n; i++) {
        result.push(generator.next().value);
    }
    return result;
}

const gen10 = take(createGenerator(1000000), 10);

// QUAND UTILISER QUOI?

// Utiliser ARRAY quand:
// - Petite quantité de données
// - Besoin accès aléatoire (arr[100])
// - Besoin longueur (.length)
// - Besoin méthodes array (map, filter, sort)
// - Données utilisées plusieurs fois

// Utiliser GENERATOR quand:
// - Grande quantité de données
// - Séquence infinie
// - Calculs coûteux
// - Lecture séquentielle uniquement
// - Données utilisées une seule fois
// - Streaming de données

// Exemple concret: Fibonacci
// Array (limité)
function fibArray(n) {
    const arr = [0, 1];
    for (let i = 2; i < n; i++) {
        arr[i] = arr[i - 1] + arr[i - 2];
    }
    return arr;
}

console.log(fibArray(10)); // [0, 1, 1, 2, 3, 5, 8, 13, 21, 34]

// Generator (infini)
function* fibGenerator() {
    let [a, b] = [0, 1];
    while (true) {
        yield a;
        [a, b] = [b, a + b];
    }
}

const fib = fibGenerator();
console.log(take(fib, 10)); // [0, 1, 1, 2, 3, 5, 8, 13, 21, 34]
// Peut continuer infiniment!
```

**Points clés:**
- Array = toutes valeurs en mémoire
- Generator = calcul à la demande (lazy)
- Generator plus efficace pour grandes données
- Generator permet séquences infinies
- Array pour accès aléatoire, Generator pour séquentiel
- Generator consommé après utilisation

---

### Q5: Comment créer un range (séquence de nombres) avec un generator ?

**Réponse:**
```javascript
// Range simple (de start à end)
function* range(start, end, step = 1) {
    for (let i = start; i < end; i += step) {
        yield i;
    }
}

// Utilisation
console.log([...range(0, 10)]); 
// [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

console.log([...range(5, 10)]); 
// [5, 6, 7, 8, 9]

console.log([...range(0, 20, 2)]); 
// [0, 2, 4, 6, 8, 10, 12, 14, 16, 18]

console.log([...range(10, 0, -1)]); 
// [10, 9, 8, 7, 6, 5, 4, 3, 2, 1]

// Avec for...of
for (const num of range(1, 6)) {
    console.log(num); // 1, 2, 3, 4, 5
}

// Range infini (à partir de start)
function* rangeFrom(start = 0, step = 1) {
    let current = start;
    while (true) {
        yield current;
        current += step;
    }
}

const infinite = rangeFrom(1);
console.log(infinite.next().value); // 1
console.log(infinite.next().value); // 2
console.log(infinite.next().value); // 3

// Prendre seulement N premiers
function take(generator, n) {
    const result = [];
    for (let i = 0; i < n; i++) {
        const { value, done } = generator.next();
        if (done) break;
        result.push(value);
    }
    return result;
}

console.log(take(rangeFrom(10, 5), 5)); 
// [10, 15, 20, 25, 30]

// Range avec conditions
function* rangeWhere(start, end, predicate) {
    for (let i = start; i < end; i++) {
        if (predicate(i)) {
            yield i;
        }
    }
}

// Seulement nombres pairs
const evens = rangeWhere(0, 20, n => n % 2 === 0);
console.log([...evens]); 
// [0, 2, 4, 6, 8, 10, 12, 14, 16, 18]

// Seulement multiples de 3
const multiplesOf3 = rangeWhere(0, 30, n => n % 3 === 0);
console.log([...multiplesOf3]); 
// [0, 3, 6, 9, 12, 15, 18, 21, 24, 27]

// Range de dates
function* dateRange(start, end, days = 1) {
    const current = new Date(start);
    const endDate = new Date(end);
    
    while (current <= endDate) {
        yield new Date(current);
        current.setDate(current.getDate() + days);
    }
}

const dates = dateRange('2024-11-01', '2024-11-07');
for (const date of dates) {
    console.log(date.toLocaleDateString());
}
// 01/11/2024, 02/11/2024, ..., 07/11/2024

// Range de lettres
function* charRange(start, end) {
    const startCode = start.charCodeAt(0);
    const endCode = end.charCodeAt(0);
    
    for (let code = startCode; code <= endCode; code++) {
        yield String.fromCharCode(code);
    }
}

console.log([...charRange('a', 'z')].join('')); 
// "abcdefghijklmnopqrstuvwxyz"

console.log([...charRange('A', 'E')]); 
// ['A', 'B', 'C', 'D', 'E']

// Range réutilisable (objet)
class Range {
    constructor(start, end, step = 1) {
        this.start = start;
        this.end = end;
        this.step = step;
    }
    
    *[Symbol.iterator]() {
        for (let i = this.start; i < this.end; i += this.step) {
            yield i;
        }
    }
}

const r = new Range(0, 10, 2);
console.log([...r]); // [0, 2, 4, 6, 8]

// Réutilisable!
console.log([...r]); // [0, 2, 4, 6, 8]

// Exemples d'utilisation pratiques

// 1. Créer array de N éléments
function createArray(n, value) {
    return [...range(0, n)].map(() => value);
}
console.log(createArray(5, 0)); // [0, 0, 0, 0, 0]

// 2. Somme de 1 à N
function sumTo(n) {
    let sum = 0;
    for (const num of range(1, n + 1)) {
        sum += num;
    }
    return sum;
}
console.log(sumTo(10)); // 55

// 3. Table de multiplication
function* multiplicationTable(n) {
    for (const i of range(1, 11)) {
        yield `${n} x ${i} = ${n * i}`;
    }
}

for (const line of multiplicationTable(7)) {
    console.log(line);
}
// 7 x 1 = 7
// 7 x 2 = 14
// ...

// 4. Pagination
function* paginate(items, pageSize) {
    for (const start of range(0, items.length, pageSize)) {
        yield items.slice(start, start + pageSize);
    }
}

const items = [...range(1, 26)]; // [1..25]
for (const page of paginate(items, 10)) {
    console.log('Page:', page);
}
// Page: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
// Page: [11, 12, 13, 14, 15, 16, 17, 18, 19, 20]
// Page: [21, 22, 23, 24, 25]
```

**Points clés:**
- `range(start, end, step)` comme Python
- Boucle `for` avec `yield` dans chaque itération
- Infini possible avec `while (true)`
- Réutilisable avec `*[Symbol.iterator]()`
- Fonctionne avec for...of et spread
- Memory-efficient pour grandes plages

---

## Questions Niveau Intermédiaire

### Q6: Comment passer des valeurs à un generator avec `next()` ?

**Réponse:**
```javascript
// next() peut PASSER une valeur au generator
// La valeur devient le résultat de l'expression yield

function* echo() {
    console.log('Starting');
    
    const value1 = yield 'Ready'; // Attend une valeur
    console.log('Received:', value1);
    
    const value2 = yield 'Waiting';
    console.log('Received:', value2);
    
    const value3 = yield 'Last';
    console.log('Received:', value3);
}

const gen = echo();

console.log(gen.next());        
// Log: "Starting"
// Return: { value: 'Ready', done: false }

console.log(gen.next('Hello')); 
// Log: "Received: Hello"
// Return: { value: 'Waiting', done: false }

console.log(gen.next('World')); 
// Log: "Received: World"
// Return: { value: 'Last', done: false }

console.log(gen.next('!')); 
// Log: "Received: !"
// Return: { value: undefined, done: true }

// ⚠️ IMPORTANT: Premier next() ne peut PAS passer de valeur
function* test() {
    const x = yield 1;
    console.log('x:', x);
}

const t = test();
console.log(t.next('ignored')); 
// { value: 1, done: false }
// 'ignored' est ignoré (pas de yield précédent)

console.log(t.next('received')); 
// Log: "x: received"
// { value: undefined, done: true }

// Exemple: Accumulator
function* accumulator(initial = 0) {
    let total = initial;
    
    while (true) {
        const value = yield total;
        if (value !== undefined) {
            total += value;
        }
    }
}

const acc = accumulator(0);
console.log(acc.next());      // { value: 0, done: false }
console.log(acc.next(5));     // { value: 5, done: false }
console.log(acc.next(10));    // { value: 15, done: false }
console.log(acc.next(3));     // { value: 18, done: false }
console.log(acc.next());      // { value: 18, done: false } (pas de valeur)

// Exemple: Conversation
function* conversation() {
    const name = yield 'What is your name?';
    const age = yield `Hello ${name}, what is your age?`;
    const city = yield `${name}, ${age} years old. Where do you live?`;
    yield `${name}, ${age}, from ${city}. Nice to meet you!`;
}

const conv = conversation();
console.log(conv.next().value);        // "What is your name?"
console.log(conv.next('John').value);  // "Hello John, what is your age?"
console.log(conv.next(30).value);      // "John, 30 years old. Where do you live?"
console.log(conv.next('Paris').value); // "John, 30, from Paris. Nice to meet you!"

// Exemple: State Machine
function* stateMachine() {
    let state = 'idle';
    
    while (true) {
        const action = yield state;
        
        switch (action) {
            case 'start':
                state = 'running';
                break;
            case 'pause':
                state = 'paused';
                break;
            case 'stop':
                state = 'stopped';
                return state; // Termine
            case 'reset':
                state = 'idle';
                break;
            default:
                console.log('Unknown action:', action);
        }
    }
}

const machine = stateMachine();
console.log(machine.next());          // { value: 'idle', done: false }
console.log(machine.next('start'));   // { value: 'running', done: false }
console.log(machine.next('pause'));   // { value: 'paused', done: false }
console.log(machine.next('start'));   // { value: 'running', done: false }
console.log(machine.next('stop'));    // { value: 'stopped', done: true }

// Exemple: Calculator
function* calculator() {
    let result = 0;
    
    while (true) {
        const input = yield result;
        
        if (!input) continue;
        
        const [operation, value] = input.split(' ');
        const num = parseFloat(value);
        
        switch (operation) {
            case 'add':
                result += num;
                break;
            case 'subtract':
                result -= num;
                break;
            case 'multiply':
                result *= num;
                break;
            case 'divide':
                result /= num;
                break;
            case 'clear':
                result = 0;
                break;
        }
    }
}

const calc = calculator();
console.log(calc.next());                // { value: 0, done: false }
console.log(calc.next('add 10'));        // { value: 10, done: false }
console.log(calc.next('multiply 3'));    // { value: 30, done: false }
console.log(calc.next('subtract 5'));    // { value: 25, done: false }
console.log(calc.next('divide 5'));      // { value: 5, done: false }
console.log(calc.next('clear'));         // { value: 0, done: false }

// Exemple: Bidirectional communication
function* pingPong() {
    let count = 0;
    
    while (true) {
        const received = yield `pong ${count}`;
        
        if (received === 'ping') {
            count++;
        } else if (received === 'stop') {
            return 'Game over';
        }
    }
}

const game = pingPong();
console.log(game.next());          // { value: 'pong 0', done: false }
console.log(game.next('ping'));    // { value: 'pong 1', done: false }
console.log(game.next('ping'));    // { value: 'pong 2', done: false }
console.log(game.next('ping'));    // { value: 'pong 3', done: false }
console.log(game.next('stop'));    // { value: 'Game over', done: true }
```

**Points clés:**
- `next(value)` passe valeur au yield précédent
- Premier `next()` ne peut pas passer de valeur
- Valeur devient résultat de l'expression `yield`
- Permet communication bidirectionnelle
- Utile pour state machines, conversations
- `yield` peut être affecté à une variable

---

### Q7: Qu'est-ce que `yield*` et à quoi sert-il ?

**Réponse:**
```javascript
// yield* DÉLÈGUE à un autre generator ou iterable

// Sans yield* (manuel)
function* generator1() {
    yield 1;
    yield 2;
}

function* generator2Manual() {
    yield 'a';
    // Déléguer manuellement
    for (const value of generator1()) {
        yield value;
    }
    yield 'b';
}

console.log([...generator2Manual()]); 
// ['a', 1, 2, 'b']

// Avec yield* (plus simple)
function* generator2() {
    yield 'a';
    yield* generator1(); // Déléguer automatiquement
    yield 'b';
}

console.log([...generator2()]); 
// ['a', 1, 2, 'b']

// yield* avec array (iterable)
function* withArray() {
    yield 'start';
    yield* [1, 2, 3]; // Déléguer à l'array
    yield 'end';
}

console.log([...withArray()]); 
// ['start', 1, 2, 3, 'end']

// yield* avec string (iterable)
function* withString() {
    yield* 'hello';
}

console.log([...withString()]); 
// ['h', 'e', 'l', 'l', 'o']

// yield* RETOURNE la valeur de return
function* inner() {
    yield 1;
    yield 2;
    return 'finished'; // Valeur retournée
}

function* outer() {
    const result = yield* inner();
    console.log('Inner returned:', result);
    yield 3;
}

const gen = outer();
console.log(gen.next()); // { value: 1, done: false }
console.log(gen.next()); // { value: 2, done: false }
console.log(gen.next()); // Log: "Inner returned: finished"
                          // { value: 3, done: false }
console.log(gen.next()); // { value: undefined, done: true }

// Composition de generators
function* numbers() {
    yield 1;
    yield 2;
    yield 3;
}

function* letters() {
    yield 'a';
    yield 'b';
    yield 'c';
}

function* combined() {
    yield* numbers();
    yield* letters();
}

console.log([...combined()]); 
// [1, 2, 3, 'a', 'b', 'c']

// Parcours d'arbre avec yield* (récursif)
const tree = {
    value: 1,
    children: [
        {
            value: 2,
            children: [
                { value: 4, children: [] },
                { value: 5, children: [] }
            ]
        },
        {
            value: 3,
            children: [
                { value: 6, children: [] }
            ]
        }
    ]
};

function* traverseTree(node) {
    yield node.value;
    
    for (const child of node.children) {
        yield* traverseTree(child); // Récursion avec yield*
    }
}

console.log([...traverseTree(tree)]); 
// [1, 2, 4, 5, 3, 6]

// Flatten array avec yield* (récursif)
function* flatten(array) {
    for (const item of array) {
        if (Array.isArray(item)) {
            yield* flatten(item); // Récursion
        } else {
            yield item;
        }
    }
}

const nested = [1, [2, [3, 4], 5], 6, [7, [8, 9]]];
console.log([...flatten(nested)]); 
// [1, 2, 3, 4, 5, 6, 7, 8, 9]

// yield* avec Set
function* withSet() {
    yield 'start';
    yield* new Set([1, 2, 3, 2, 1]); // Set est iterable
    yield 'end';
}

console.log([...withSet()]); 
// ['start', 1, 2, 3, 'end'] (doublons supprimés)

// yield* avec Map
function* withMap() {
    const map = new Map([
        ['a', 1],
        ['b', 2],
        ['c', 3]
    ]);
    
    yield* map; // Déléguer au Map
}

console.log([...withMap()]); 
// [['a', 1], ['b', 2], ['c', 3]]

// Exemple: Merge plusieurs sources
function* mergeGenerators(...generators) {
    for (const gen of generators) {
        yield* gen;
    }
}

function* gen1() { yield 1; yield 2; }
function* gen2() { yield 'a'; yield 'b'; }
function* gen3() { yield true; yield false; }

const merged = mergeGenerators(gen1(), gen2(), gen3());
console.log([...merged]); 
// [1, 2, 'a', 'b', true, false]

// Exemple: Read file line by line (simulation)
function* readLines(text) {
    yield* text.split('\n');
}

const fileContent = 'line 1\nline 2\nline 3';
for (const line of readLines(fileContent)) {
    console.log('Line:', line);
}
// Line: line 1
// Line: line 2
// Line: line 3

// Exemple: Pagination avec yield*
function* page1() {
    yield { id: 1, name: 'Item 1' };
    yield { id: 2, name: 'Item 2' };
}

function* page2() {
    yield { id: 3, name: 'Item 3' };
    yield { id: 4, name: 'Item 4' };
}

function* allPages() {
    yield* page1();
    yield* page2();
}

for (const item of allPages()) {
    console.log(item);
}

// Exemple: Generator de ranges combinés
function* range(start, end) {
    for (let i = start; i < end; i++) {
        yield i;
    }
}

function* multiRange(...ranges) {
    for (const [start, end] of ranges) {
        yield* range(start, end);
    }
}

const multi = multiRange([1, 4], [10, 13], [20, 23]);
console.log([...multi]); 
// [1, 2, 3, 10, 11, 12, 20, 21, 22]

// Différence yield vs yield*
function* withYield() {
    yield [1, 2, 3]; // Yield l'array entier
}

function* withYieldStar() {
    yield* [1, 2, 3]; // Yield chaque élément
}

console.log([...withYield()]);     // [[1, 2, 3]]
console.log([...withYieldStar()]); // [1, 2, 3]
```

**Points clés:**
- `yield*` délègue à autre generator/iterable
- Plus simple que boucler manuellement
- Retourne valeur de `return` du generator délégué
- Fonctionne avec: generators, arrays, strings, Set, Map
- Utile pour composition et récursion
- `yield array` ≠ `yield* array`

---

### Q8: Comment créer une séquence infinie avec un generator ?

**Réponse:**
```javascript
// Séquence infinie = while(true) avec yield

// Nombres infinis à partir de N
function* infiniteSequence(start = 0) {
    let current = start;
    while (true) {
        yield current++;
    }
}

const seq = infiniteSequence(1);
console.log(seq.next().value); // 1
console.log(seq.next().value); // 2
console.log(seq.next().value); // 3
// Peut continuer indéfiniment!

// Helper pour prendre N premiers
function take(generator, n) {
    const result = [];
    for (let i = 0; i < n; i++) {
        const { value, done } = generator.next();
        if (done) break;
        result.push(value);
    }
    return result;
}

console.log(take(infiniteSequence(10), 5)); 
// [10, 11, 12, 13, 14]

// Fibonacci infini
function* fibonacci() {
    let [a, b] = [0, 1];
    while (true) {
        yield a;
        [a, b] = [b, a + b];
    }
}

const fib = fibonacci();
console.log(take(fib, 10)); 
// [0, 1, 1, 2, 3, 5, 8, 13, 21, 34]

// Nombres premiers infinis
function* primes() {
    yield 2;
    
    const isPrime = (num) => {
        for (let i = 2; i <= Math.sqrt(num); i++) {
            if (num % i === 0) return false;
        }
        return true;
    };
    
    let num = 3;
    while (true) {
        if (isPrime(num)) {
            yield num;
        }
        num += 2; // Sauter nombres pairs
    }
}

const primesGen = primes();
console.log(take(primesGen, 10)); 
// [2, 3, 5, 7, 11, 13, 17, 19, 23, 29]

// Random numbers infini
function* randomNumbers(min = 0, max = 100) {
    while (true) {
        yield Math.floor(Math.random() * (max - min + 1)) + min;
    }
}

const random = randomNumbers(1, 10);
console.log(take(random, 5)); 
// [7, 3, 9, 1, 5] (exemple)

// Cycle infini (répète une séquence)
function* cycle(array) {
    while (true) {
        for (const item of array) {
            yield item;
        }
    }
}

const colors = cycle(['red', 'green', 'blue']);
console.log(take(colors, 8)); 
// ['red', 'green', 'blue', 'red', 'green', 'blue', 'red', 'green']

// Repeat infini (répète une valeur)
function* repeat(value) {
    while (true) {
        yield value;
    }
}

const zeros = repeat(0);
console.log(take(zeros, 5)); 
// [0, 0, 0, 0, 0]

// Date sequence infinie
function* dateSequence(start = new Date(), step = 1) {
    const current = new Date(start);
    
    while (true) {
        yield new Date(current);
        current.setDate(current.getDate() + step);
    }
}

const dates = dateSequence('2024-11-01');
const next7Days = take(dates, 7);
console.log(next7Days.map(d => d.toLocaleDateString()));

// ID generator infini
function* idGenerator(prefix = 'id') {
    let id = 1;
    while (true) {
        yield `${prefix}-${id++}`;
    }
}

const ids = idGenerator('user');
console.log(ids.next().value); // "user-1"
console.log(ids.next().value); // "user-2"
console.log(ids.next().value); // "user-3"

// Timer infini (tick chaque N secondes)
function* timer(intervalMs = 1000) {
    let tick = 0;
    while (true) {
        yield tick++;
        // En pratique, utiliser avec setInterval
    }
}

// Pagination infinie
function* infinitePagination(fetchPage) {
    let page = 1;
    
    while (true) {
        const data = fetchPage(page);
        
        if (data.length === 0) break; // Arrêter si plus de données
        
        yield* data;
        page++;
    }
}

// Simulation
function mockFetchPage(page) {
    if (page > 3) return [];
    return Array.from({ length: 10 }, (_, i) => 
        `Item ${(page - 1) * 10 + i + 1}`
    );
}

const paginated = infinitePagination(mockFetchPage);
console.log(take(paginated, 25)); 
// Items 1-25

// Combiner plusieurs séquences infinies
function* interleave(...generators) {
    const gens = generators.map(g => g[Symbol.iterator]());
    
    while (true) {
        for (const gen of gens) {
            const { value, done } = gen.next();
            if (!done) {
                yield value;
            }
        }
    }
}

const seq1 = infiniteSequence(1);
const seq2 = infiniteSequence(100);
const interleaved = interleave(seq1, seq2);

console.log(take(interleaved, 10)); 
// [1, 100, 2, 101, 3, 102, 4, 103, 5, 104]

// takeWhile - Prendre tant que condition vraie
function* takeWhile(generator, predicate) {
    for (const value of generator) {
        if (!predicate(value)) break;
        yield value;
    }
}

const nums = infiniteSequence(1);
const lessThan10 = takeWhile(nums, n => n < 10);
console.log([...lessThan10]); 
// [1, 2, 3, 4, 5, 6, 7, 8, 9]

// skipWhile - Sauter tant que condition vraie
function* skipWhile(generator, predicate) {
    let skipping = true;
    
    for (const value of generator) {
        if (skipping && predicate(value)) continue;
        skipping = false;
        yield value;
    }
}

// ⚠️ ATTENTION: Boucle infinie si pas de limite!
// Ne PAS faire: [...infiniteSequence()]
// Ne PAS faire: for (const n of infiniteSequence()) { }

// ✅ Toujours utiliser take(), takeWhile(), ou break
for (const n of infiniteSequence(1)) {
    if (n > 5) break; // Important!
    console.log(n);
}
// 1, 2, 3, 4, 5

// Exemple pratique: Stream processing
function* dataStream() {
    let id = 1;
    while (true) {
        yield {
            id: id++,
            timestamp: Date.now(),
            value: Math.random()
        };
    }
}

const stream = dataStream();
const first10Events = take(stream, 10);
console.log(first10Events);
```

**Points clés:**
- `while (true)` avec `yield` = séquence infinie
- **Toujours** utiliser `take()` ou `break` pour limiter
- Memory-efficient (calcul à la demande)
- Utile: ID generator, streams, pagination
- ⚠️ Ne jamais spread/iterate sans limite
- `takeWhile()` pour condition d'arrêt

---

### Q9: Comment gérer les erreurs dans un generator ?

**Réponse:**
```javascript
// Les generators ont 3 méthodes: next(), throw(), return()

// 1. THROW() - Lancer une erreur dans le generator
function* withErrorHandling() {
    try {
        yield 1;
        yield 2;
        yield 3;
    } catch (error) {
        console.log('Caught in generator:', error.message);
        yield 'error handled';
    }
}

const gen = withErrorHandling();
console.log(gen.next());  // { value: 1, done: false }

// Lancer une erreur dans le generator
console.log(gen.throw(new Error('Oops'))); 
// Log: "Caught in generator: Oops"
// { value: 'error handled', done: false }

console.log(gen.next());  // { value: undefined, done: true }

// Si erreur pas catchée, elle se propage
function* noErrorHandling() {
    yield 1;
    yield 2;
}

const gen2 = noErrorHandling();
console.log(gen2.next()); // { value: 1, done: false }

try {
    gen2.throw(new Error('Not caught'));
} catch (error) {
    console.log('Caught outside:', error.message);
    // "Caught outside: Not caught"
}

// 2. RETURN() - Terminer generator prématurément
function* withReturn() {
    try {
        yield 1;
        yield 2;
        yield 3;
    } finally {
        console.log('Cleanup');
    }
}

const gen3 = withReturn();
console.log(gen3.next());     // { value: 1, done: false }
console.log(gen3.return(99)); // Log: "Cleanup"
                               // { value: 99, done: true }
console.log(gen3.next());     // { value: undefined, done: true }

// try/finally pour cleanup
function* withCleanup(resource) {
    try {
        console.log('Opening resource:', resource);
        yield resource;
        console.log('Using resource');
    } finally {
        console.log('Closing resource:', resource);
    }
}

const gen4 = withCleanup('file.txt');
console.log(gen4.next()); // "Opening resource: file.txt"
                          // { value: 'file.txt', done: false }
gen4.return();            // "Closing resource: file.txt"

// Exemple pratique: Retry mechanism
function* retryOperation(operation, maxRetries = 3) {
    let attempts = 0;
    
    while (attempts < maxRetries) {
        try {
            const result = yield operation();
            return result; // Success!
        } catch (error) {
            attempts++;
            console.log(`Attempt ${attempts} failed:`, error.message);
            
            if (attempts >= maxRetries) {
                throw new Error(`Failed after ${maxRetries} attempts`);
            }
            
            yield `Retrying... (${attempts}/${maxRetries})`;
        }
    }
}

// Simulation
function unreliableOperation() {
    if (Math.random() > 0.7) {
        return 'Success!';
    }
    throw new Error('Random failure');
}

const retry = retryOperation(unreliableOperation, 3);
let result = retry.next();

while (!result.done) {
    console.log(result.value);
    result = retry.next();
}

// Validation avec generator
function* validateForm(data) {
    try {
        // Valider email
        if (!data.email || !data.email.includes('@')) {
            throw new Error('Invalid email');
        }
        yield { field: 'email', valid: true };
        
        // Valider password
        if (!data.password || data.password.length < 8) {
            throw new Error('Password too short');
        }
        yield { field: 'password', valid: true };
        
        // Valider age
        if (!data.age || data.age < 18) {
            throw new Error('Must be 18+');
        }
        yield { field: 'age', valid: true };
        
        return { valid: true, message: 'All valid' };
        
    } catch (error) {
        return { valid: false, error: error.message };
    }
}

const formData = {
    email: 'user@example.com',
    password: 'pass123',
    age: 25
};

const validation = validateForm(formData);
for (const result of validation) {
    console.log(result);
}

// Error boundaries pour generators
function* safeGenerator(generator) {
    try {
        yield* generator;
    } catch (error) {
        console.error('Generator error:', error);
        yield { error: true, message: error.message };
    }
}

function* riskyGenerator() {
    yield 1;
    yield 2;
    throw new Error('Something went wrong');
    yield 3; // Jamais atteint
}

const safe = safeGenerator(riskyGenerator());
for (const value of safe) {
    console.log(value);
}
// 1
// 2
// { error: true, message: 'Something went wrong' }

// Async error handling
async function* asyncGeneratorWithErrors() {
    try {
        yield await Promise.resolve(1);
        yield await Promise.resolve(2);
        yield await Promise.reject(new Error('Async error'));
        yield 3; // Jamais atteint
    } catch (error) {
        console.log('Async error caught:', error.message);
        yield 'recovered';
    }
}

(async () => {
    for await (const value of asyncGeneratorWithErrors()) {
        console.log(value);
    }
})();
// 1
// 2
// "Async error caught: Async error"
// "recovered"

// Resource management avec generator
function* managedResource(resource) {
    let handle = null;
    
    try {
        handle = resource.open();
        console.log('Resource opened');
        
        yield handle;
        
    } catch (error) {
        console.error('Error:', error);
        throw error;
        
    } finally {
        if (handle) {
            resource.close(handle);
            console.log('Resource closed');
        }
    }
}

// Simulation
const mockResource = {
    open() {
        console.log('Opening...');
        return { id: 1 };
    },
    close(handle) {
        console.log('Closing...', handle.id);
    }
};

const managed = managedResource(mockResource);
const res = managed.next();
console.log('Using resource:', res.value);
managed.return(); // Force cleanup

// State machine avec error states
function* stateMachineWithErrors() {
    let state = 'idle';
    
    while (true) {
        try {
            const action = yield state;
            
            switch (state) {
                case 'idle':
                    if (action === 'start') {
                        state = 'running';
                    } else {
                        throw new Error('Invalid transition from idle');
                    }
                    break;
                    
                case 'running':
                    if (action === 'pause') {
                        state = 'paused';
                    } else if (action === 'stop') {
                        state = 'stopped';
                        return state;
                    } else {
                        throw new Error('Invalid transition from running');
                    }
                    break;
                    
                case 'paused':
                    if (action === 'resume') {
                        state = 'running';
                    } else if (action === 'stop') {
                        state = 'stopped';
                        return state;
                    } else {
                        throw new Error('Invalid transition from paused');
                    }
                    break;
            }
            
        } catch (error) {
            console.error('State machine error:', error.message);
            state = 'error';
            yield state;
            state = 'idle'; // Reset
        }
    }
}

const machine = stateMachineWithErrors();
console.log(machine.next());           // idle
console.log(machine.next('start'));    // running
console.log(machine.next('invalid')); // error, then idle
```

**Points clés:**
- `throw()` lance erreur dans generator
- `return()` termine generator prématurément
- `try/catch` dans generator pour gérer erreurs
- `finally` pour cleanup garanti
- Erreur non catchée se propage à l'appelant
- Utile pour: retry, validation, resource management

---

### Q10: Quels sont les cas d'usage pratiques des generators en React ?

**Réponse:**
```javascript
// Les generators sont moins utilisés directement en React
// mais comprendre le concept est important

// CAS D'USAGE 1: Redux Saga (middleware Redux)
// Redux Saga utilise generators pour side effects

// Exemple de saga (pas React pur, mais Redux)
/*
import { call, put, takeEvery } from 'redux-saga/effects';

function* fetchUserSaga(action) {
    try {
        // Pause et attend la réponse
        const user = yield call(api.fetchUser, action.payload);
        
        // Dispatch action avec résultat
        yield put({ type: 'FETCH_USER_SUCCESS', user });
        
    } catch (error) {
        yield put({ type: 'FETCH_USER_ERROR', error });
    }
}

// Watcher saga
function* watchFetchUser() {
    yield takeEvery('FETCH_USER_REQUEST', fetchUserSaga);
}
*/

// CAS D'USAGE 2: Pagination lazy loading
function* usePagination(fetchPage) {
    let page = 1;
    
    while (true) {
        const data = fetchPage(page);
        if (data.length === 0) break;
        
        yield data;
        page++;
    }
}

// Utilisation dans composant (conceptuel)
/*
function InfiniteList() {
    const [items, setItems] = useState([]);
    const paginatorRef = useRef(null);
    
    useEffect(() => {
        paginatorRef.current = usePagination(fetchFromAPI);
    }, []);
    
    const loadMore = () => {
        const { value, done } = paginatorRef.current.next();
        if (!done) {
            setItems(prev => [...prev, ...value]);
        }
    };
    
    return (
        <div>
            {items.map(item => <Item key={item.id} {...item} />)}
            <button onClick={loadMore}>Load More</button>
        </div>
    );
}
*/

// CAS D'USAGE 3: ID Generator hook
function useIdGenerator(prefix = 'id') {
    const generatorRef = useRef(null);
    
    if (!generatorRef.current) {
        generatorRef.current = (function* () {
            let id = 1;
            while (true) {
                yield `${prefix}-${id++}`;
            }
        })();
    }
    
    const generateId = useCallback(() => {
        return generatorRef.current.next().value;
    }, []);
    
    return generateId;
}

// Utilisation
/*
function FormWithMultipleInputs() {
    const generateId = useIdGenerator('input');
    
    const [fields, setFields] = useState([
        { id: generateId(), value: '' }
    ]);
    
    const addField = () => {
        setFields([...fields, { id: generateId(), value: '' }]);
    };
    
    return (
        <div>
            {fields.map(field => (
                <input key={field.id} id={field.id} />
            ))}
            <button onClick={addField}>Add Field</button>
        </div>
    );
}
*/

// CAS D'USAGE 4: Sequence animations
function* useAnimationSequence() {
    yield { opacity: 0, y: -20 };
    yield { opacity: 0.5, y: -10 };
    yield { opacity: 1, y: 0 };
}

/*
function AnimatedComponent() {
    const [style, setStyle] = useState({});
    const sequenceRef = useRef(null);
    
    useEffect(() => {
        sequenceRef.current = useAnimationSequence();
        
        const interval = setInterval(() => {
            const { value, done } = sequenceRef.current.next();
            
            if (done) {
                clearInterval(interval);
            } else {
                setStyle(value);
            }
        }, 300);
        
        return () => clearInterval(interval);
    }, []);
    
    return <div style={style}>Animated</div>;
}
*/

// CAS D'USAGE 5: State machine
function* useStateMachine(initialState) {
    let state = initialState;
    
    while (true) {
        const action = yield state;
        
        // Transitions
        switch (state) {
            case 'idle':
                if (action === 'start') state = 'loading';
                break;
            case 'loading':
                if (action === 'success') state = 'success';
                if (action === 'error') state = 'error';
                break;
            case 'success':
            case 'error':
                if (action === 'reset') state = 'idle';
                break;
        }
    }
}

/*
function DataFetcher() {
    const machineRef = useRef(null);
    const [state, setState] = useState('idle');
    
    useEffect(() => {
        machineRef.current = useStateMachine('idle');
    }, []);
    
    const dispatch = (action) => {
        const result = machineRef.current.next(action);
        setState(result.value);
    };
    
    const fetchData = async () => {
        dispatch('start');
        
        try {
            await api.fetchData();
            dispatch('success');
        } catch (error) {
            dispatch('error');
        }
    };
    
    return (
        <div>
            <p>State: {state}</p>
            {state === 'idle' && <button onClick={fetchData}>Fetch</button>}
            {state === 'loading' && <Spinner />}
            {state === 'success' && <Data />}
            {state === 'error' && <button onClick={() => dispatch('reset')}>Retry</button>}
        </div>
    );
}
*/

// CAS D'USAGE 6: Iterator pour données
function* useDataIterator(data) {
    for (const item of data) {
        yield item;
    }
}

/*
function Carousel({ items }) {
    const [current, setCurrent] = useState(null);
    const iteratorRef = useRef(null);
    
    useEffect(() => {
        iteratorRef.current = useDataIterator(items);
        next();
    }, [items]);
    
    const next = () => {
        const { value, done } = iteratorRef.current.next();
        if (done) {
            // Restart
            iteratorRef.current = useDataIterator(items);
            next();
        } else {
            setCurrent(value);
        }
    };
    
    return (
        <div>
            <div>{current?.name}</div>
            <button onClick={next}>Next</button>
        </div>
    );
}
*/

// POURQUOI MOINS UTILISÉ EN REACT ?

// 1. Hooks sont plus simples pour la plupart des cas
// 2. Async/await plus intuitif que generators pour async
// 3. State management avec useState/useReducer suffit souvent

// QUAND UTILISER ?

// ✅ Redux Saga (middleware Redux)
// ✅ Séquences complexes (animations, wizards)
// ✅ State machines complexes
// ✅ Itération lazy sur grandes données
// ✅ ID generation

// ❌ Simple data fetching (utiliser useEffect + async/await)
// ❌ Simple state (utiliser useState)
// ❌ Forms (utiliser libraries comme Formik)

// Exemple complet: Wizard avec generator
/*
function* wizardSteps() {
    yield { step: 1, component: PersonalInfo };
    yield { step: 2, component: Address };
    yield { step: 3, component: Payment };
    yield { step: 4, component: Confirmation };
}

function Wizard() {
    const [currentStep, setCurrentStep] = useState(null);
    const wizardRef = useRef(null);
    
    useEffect(() => {
        wizardRef.current = wizardSteps();
        next();
    }, []);
    
    const next = () => {
        const { value, done } = wizardRef.current.next();
        if (!done) {
            setCurrentStep(value);
        }
    };
    
    const Component = currentStep?.component;
    
    return (
        <div>
            <h2>Step {currentStep?.step}/4</h2>
            {Component && <Component onNext={next} />}
        </div>
    );
}
*/
```

**Points clés:**
- Generators moins utilisés directement en React
- Redux Saga = cas d'usage principal
- Utile pour: séquences, state machines, pagination
- Hooks souvent plus simples
- Async/await préféré pour async
- Comprendre concept important pour Redux Saga

---

**Résumé final pour entretien:**

**Generators:**
- Fonctions pausables avec `function*`
- `yield` retourne valeur et pause
- `next()` reprend exécution
- Lazy evaluation (calcul à la demande)
- Séquences infinies possibles
- `yield*` pour délégation
- Compatible for...of, spread

**Use cases:**
- Redux Saga (très important!)
- Séquences infinies
- Pagination lazy
- State machines
- ID generation
- Iterator patterns

**Avantages:**
- Memory-efficient
- Contrôle d'exécution
- Composition facile