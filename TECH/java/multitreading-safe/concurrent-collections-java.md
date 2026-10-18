# Concurrent Collections & Thread-Safe Programming en Java
### Low Latency Programming — Guide Complet

---

## Table des matières

1. [Introduction : Pourquoi les Concurrent Collections ?](#1-introduction)
2. [ConcurrentHashMap](#2-concurrenthashmap)
3. [ConcurrentLinkedQueue & ConcurrentLinkedDeque](#3-concurrentlinkedqueue--concurrentlinkeddeque)
4. [CopyOnWriteArrayList & CopyOnWriteArraySet](#4-copyonwritearraylist--copyonwritearrayset)
5. [BlockingQueue : ArrayBlockingQueue & LinkedBlockingQueue](#5-blockingqueue)
6. [LinkedTransferQueue & SynchronousQueue](#6-linkedtransferqueue--synchronousqueue)
7. [ConcurrentSkipListMap & ConcurrentSkipListSet](#7-concurrentskiplistmap--concurrentskiplistset)
8. [Atomic Variables](#8-atomic-variables)
9. [LongAdder & LongAccumulator](#9-longadder--longaccumulator)
10. [Lock-Free & Wait-Free Patterns](#10-lock-free--wait-free-patterns)
11. [Memory Model & Happens-Before](#11-java-memory-model--happens-before)
12. [Bonnes pratiques Low Latency](#12-bonnes-pratiques-low-latency)
13. [Comparatif & Choisir la bonne structure](#13-comparatif--choisir-la-bonne-structure)

---

## 1. Introduction

En programmation **multi-threadée**, partager des données entre threads sans synchronisation correcte mène à des **race conditions**, des **data races**, et des comportements non déterministes.

### Problèmes des collections classiques

```java
// DANGEREUX en multi-thread
Map<String, Integer> map = new HashMap<>();

// Thread A
map.put("key", 1);

// Thread B (concurrent) → ConcurrentModificationException ou pire
map.put("key", 2);
```

### Collections synchronisées classiques (à éviter en low latency)

```java
// synchronizedMap → verrou global sur TOUTE l'opération
Map<String, Integer> map = Collections.synchronizedMap(new HashMap<>());
```

> **Problème** : Un verrou global (`synchronized`) bloque tous les threads, même pour des opérations en lecture. C'est catastrophique pour la **latency**.

### Ce qu'offre le package `java.util.concurrent`

| Besoin | Solution |
|---|---|
| Map concurrent | `ConcurrentHashMap` |
| Queue non-bloquante | `ConcurrentLinkedQueue` |
| Queue bloquante | `ArrayBlockingQueue`, `LinkedBlockingQueue` |
| Liste avec lecture dominante | `CopyOnWriteArrayList` |
| Map triée concurrent | `ConcurrentSkipListMap` |
| Compteur thread-safe | `AtomicLong`, `LongAdder` |

---

## 2. ConcurrentHashMap

### Principe : Segment Locking → Node Locking (Java 8+)

Avant Java 8, `ConcurrentHashMap` divisait la map en **segments** (buckets groupés), chacun avec son propre verrou. Depuis Java 8, il utilise le **CAS (Compare-And-Swap)** et des verrous au niveau du **nœud individuel** (bin-level locking).

```
Map interne :
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│  0  │  1  │  2  │  3  │  4  │  5  │  6  │  7  │  ← buckets (Node[])
└──┬──┴─────┴─────┴──┬──┴─────┴─────┴─────┴─────┘
   │                  │
   ▼                  ▼
Node(k,v)→Node    Node(k,v)
```

- **Lecture** : totalement **lock-free** (via `volatile` reads)
- **Écriture** : verrou uniquement sur le **bin** concerné → haute concurrence

### API principale

```java
ConcurrentHashMap<String, Integer> map = new ConcurrentHashMap<>();

// Opérations basiques (thread-safe)
map.put("order", 42);
Integer val = map.get("order");

// putIfAbsent — atomique
map.putIfAbsent("price", 100);

// compute — atomique, évite le pattern check-then-act
map.compute("count", (key, oldVal) -> oldVal == null ? 1 : oldVal + 1);

// merge — atomique
map.merge("total", 10, Integer::sum);

// computeIfAbsent — lazy init thread-safe
map.computeIfAbsent("cache", key -> loadFromDB(key));

// computeIfPresent
map.computeIfPresent("count", (k, v) -> v + 1);
```

### Opérations en bulk (Java 8+)

```java
ConcurrentHashMap<String, Long> latencies = new ConcurrentHashMap<>();

// forEach parallèle — parallelismThreshold = 1 → max parallélisme
latencies.forEach(1, (key, value) ->
    System.out.println(key + " → " + value + "ms"));

// search — s'arrête dès qu'un résultat est trouvé
String slowService = latencies.search(1, (k, v) -> v > 1000 ? k : null);

// reduce
long totalLatency = latencies.reduceValues(1, Long::sum);

// mappingCount() — préférer à size() pour large maps
long count = latencies.mappingCount();
```

### Anti-pattern fréquent : Check-Then-Act

```java
// MAUVAIS — non atomique même avec ConcurrentHashMap
if (!map.containsKey("key")) {
    map.put("key", computeValue()); // race condition possible !
}

// BON — atomique
map.computeIfAbsent("key", k -> computeValue());
```

### ConcurrentHashMap vs Hashtable vs synchronizedMap

| | `Hashtable` | `synchronizedMap` | `ConcurrentHashMap` |
|---|---|---|---|
| Lecture concurrente | Bloquée | Bloquée | **Lock-free** |
| Écriture concurrente | Verrou global | Verrou global | Verrou par bin |
| null keys/values | ✗ | ✓ (HashMap) | ✗ |
| Iterator | fail-fast | fail-fast | **weakly consistent** |
| Performance | Faible | Faible | **Haute** |

> **Weakly consistent iterator** : ne lève pas `ConcurrentModificationException`, mais peut ne pas refléter les modifications postérieures à sa création.

---

## 3. ConcurrentLinkedQueue & ConcurrentLinkedDeque

### ConcurrentLinkedQueue — Non-Blocking, Lock-Free

Implémentée avec l'algorithme de **Michael & Scott (1996)**, basé sur le CAS. C'est une queue FIFO **unbounded** (taille illimitée).

```
head → [A] → [B] → [C] → null ← tail
```

```java
ConcurrentLinkedQueue<String> queue = new ConcurrentLinkedQueue<>();

// Producteur
queue.offer("event-1");  // toujours true (unbounded)
queue.add("event-2");

// Consommateur
String event = queue.poll();   // null si vide
String peeked = queue.peek();  // lit sans retirer

// Vérification
boolean empty = queue.isEmpty();
int size = queue.size();  // attention : O(n) !
```

> ⚠️ `size()` est **O(n)** — à éviter en hot path. Utiliser `isEmpty()` à la place.

### Algorithme CAS interne (simplifié)

```java
// Principe du CAS pour enqueue
public boolean offer(E item) {
    Node<E> newNode = new Node<>(item);
    for (;;) {  // spin loop
        Node<E> t = tail;
        Node<E> next = t.next;
        if (next == null) {
            // CAS : si tail.next == null, on pose notre nœud
            if (UNSAFE.compareAndSwapObject(t, nextOffset, null, newNode)) {
                // On tente de faire avancer tail
                UNSAFE.compareAndSwapObject(this, tailOffset, t, newNode);
                return true;
            }
        } else {
            // Un autre thread est en train d'enqueue, on l'aide
            UNSAFE.compareAndSwapObject(this, tailOffset, t, next);
        }
    }
}
```

### ConcurrentLinkedDeque — Double-Ended Queue

```java
ConcurrentLinkedDeque<String> deque = new ConcurrentLinkedDeque<>();

// En tête
deque.addFirst("priority-event");
String first = deque.pollFirst();

// En queue
deque.addLast("normal-event");
String last = deque.pollLast();

// Peek sans retrait
String head = deque.peekFirst();
String tail = deque.peekLast();
```

**Cas d'usage** : work-stealing, priority queues custom, buffers de messages.

---

## 4. CopyOnWriteArrayList & CopyOnWriteArraySet

### Principe : Immutabilité à l'écriture

À chaque modification (add, remove, set), un **nouveau tableau** est créé (copie de l'ancien + modification). Les lectures ne sont **jamais bloquées** car elles opèrent sur un snapshot immuable.

```
Avant write:  [A, B, C]      ← lectures en cours
Après write:  [A, B, C, D]   ← nouveau tableau atomiquement substitué
```

```java
CopyOnWriteArrayList<String> list = new CopyOnWriteArrayList<>();

// Écriture — crée une copie du tableau interne
list.add("handler-1");
list.add("handler-2");

// Lecture — lock-free, snapshot de la référence courante
for (String handler : list) {  // iterate sans ConcurrentModificationException
    handler.process();
}

// Opérations bulk
list.addAllAbsent(newHandlers);  // ajoute seulement les absents
```

### Quand utiliser CopyOnWriteArrayList ?

✅ **Idéal pour** :
- Listes de **listeners / observers** (modifiées rarement, lues souvent)
- **Event handlers** en système de trading
- Configurations qui se mettent rarement à jour

❌ **À éviter si** :
- Écritures fréquentes (coût mémoire élevé, GC pressure)
- Grande taille de liste (chaque write = copie complète)

```java
// Pattern classique : liste de listeners
CopyOnWriteArrayList<MarketDataListener> listeners = new CopyOnWriteArrayList<>();

// Un seul thread d'admin ajoute des listeners
listeners.add(new PriceUpdateListener());

// Des milliers de threads appellent les listeners sans lock
listeners.forEach(l -> l.onTick(tick));
```

---

## 5. BlockingQueue

Les `BlockingQueue` permettent de bloquer le thread si la queue est vide (poll) ou pleine (put). Parfaites pour le pattern **Producer-Consumer**.

### ArrayBlockingQueue — Bounded, Backed by Array

```java
// Capacité fixe : 1024 slots
ArrayBlockingQueue<Order> queue = new ArrayBlockingQueue<>(1024);

// Producteur
queue.put(order);              // bloque si plein
boolean added = queue.offer(order);  // retourne false si plein
queue.offer(order, 100, TimeUnit.MICROSECONDS);  // timeout

// Consommateur
Order o = queue.take();        // bloque si vide
Order o2 = queue.poll();       // null si vide
Order o3 = queue.poll(50, TimeUnit.MICROSECONDS);  // timeout
```

> **Choix de fair vs unfair** : par défaut, `ArrayBlockingQueue` est **unfair** (plus performant). Mode fair garantit FIFO pour les threads en attente mais coûte plus cher.

```java
// fair = true → FIFO entre threads bloqués (plus lent)
ArrayBlockingQueue<Order> fair = new ArrayBlockingQueue<>(1024, true);
```

### LinkedBlockingQueue — Optionally Bounded

```java
// Bounded
LinkedBlockingQueue<Event> bounded = new LinkedBlockingQueue<>(10_000);

// Unbounded (capacité = Integer.MAX_VALUE)
LinkedBlockingQueue<Event> unbounded = new LinkedBlockingQueue<>();
```

**Différence clé avec ArrayBlockingQueue** :
- `LinkedBlockingQueue` : **deux verrous séparés** (putLock + takeLock) → meilleur débit avec un producteur ET un consommateur
- `ArrayBlockingQueue` : **un seul verrou** → simpler, moins de mémoire

### PriorityBlockingQueue — Tri par priorité

```java
PriorityBlockingQueue<Order> pq = new PriorityBlockingQueue<>(
    1024,
    Comparator.comparingInt(Order::getPriority)
);

pq.put(new Order("low", 3));
pq.put(new Order("high", 1));  // sortira en premier

Order next = pq.take();  // retourne Order("high", 1)
```

### DelayQueue — Messages différés

```java
class DelayedMessage implements Delayed {
    private final long readyTime;

    @Override
    public long getDelay(TimeUnit unit) {
        return unit.convert(readyTime - System.nanoTime(), TimeUnit.NANOSECONDS);
    }

    @Override
    public int compareTo(Delayed o) {
        return Long.compare(this.readyTime, ((DelayedMessage)o).readyTime);
    }
}

DelayQueue<DelayedMessage> delayQueue = new DelayQueue<>();
delayQueue.put(new DelayedMessage(System.nanoTime() + TimeUnit.SECONDS.toNanos(5)));

// Bloque jusqu'à ce que le message soit "ready"
DelayedMessage msg = delayQueue.take();
```

---

## 6. LinkedTransferQueue & SynchronousQueue

### SynchronousQueue — Rendez-vous entre threads

Capacité = **zéro**. Chaque `put` bloque jusqu'à ce qu'un thread fasse un `take`, et vice versa. C'est un **handoff direct**.

```java
SynchronousQueue<Task> handoff = new SynchronousQueue<>();

// Thread A (producteur) — bloque jusqu'au consommateur
handoff.put(task);

// Thread B (consommateur) — bloque jusqu'au producteur
Task t = handoff.take();
```

Utilisé en interne par `Executors.newCachedThreadPool()`.

### LinkedTransferQueue — Le meilleur des deux mondes

Si un consommateur attend déjà → **handoff direct** (comme SynchronousQueue).  
Sinon → l'élément est mis en queue (comme LinkedBlockingQueue).

```java
LinkedTransferQueue<Event> ltq = new LinkedTransferQueue<>();

// transfer() bloque jusqu'à ce qu'un consommateur prenne l'élément
ltq.transfer(event);

// tryTransfer() — non-bloquant : true si handoff direct, false sinon
boolean transferred = ltq.tryTransfer(event);

// tryTransfer avec timeout
boolean ok = ltq.tryTransfer(event, 100, TimeUnit.MICROSECONDS);

// offer() — met en queue sans attendre (classique)
ltq.offer(event);
```

---

## 7. ConcurrentSkipListMap & ConcurrentSkipListSet

### Skip List : structure de données probabiliste

Une **skip list** est une liste chaînée multi-niveaux qui permet des opérations `O(log n)` en moyenne, **sans verrou global** grâce au CAS.

```
Niveau 3 :  [HEAD] ─────────────────────────── [D] ──── [null]
Niveau 2 :  [HEAD] ──────── [B] ─────────────── [D] ──── [null]
Niveau 1 :  [HEAD] ── [A] ── [B] ── [C] ── [D] ── [E] ── [null]
```

### ConcurrentSkipListMap

```java
// Triée par clé naturelle
ConcurrentSkipListMap<Long, Order> orderBook = new ConcurrentSkipListMap<>();

orderBook.put(100L, new Order("BUY", 100));
orderBook.put(101L, new Order("BUY", 101));
orderBook.put(99L, new Order("SELL", 99));

// Navigation (toutes ces opérations sont thread-safe)
Map.Entry<Long, Order> first = orderBook.firstEntry();   // plus petite clé
Map.Entry<Long, Order> last = orderBook.lastEntry();     // plus grande clé

// floorEntry — clé ≤ k
Map.Entry<Long, Order> floor = orderBook.floorEntry(100L);

// ceilingEntry — clé ≥ k
Map.Entry<Long, Order> ceiling = orderBook.ceilingEntry(100L);

// subMap — vue d'une plage
ConcurrentNavigableMap<Long, Order> range = orderBook.subMap(99L, 102L);

// headMap / tailMap
ConcurrentNavigableMap<Long, Order> below = orderBook.headMap(100L);
ConcurrentNavigableMap<Long, Order> above = orderBook.tailMap(100L);

// pollFirstEntry / pollLastEntry — atomique
Map.Entry<Long, Order> removed = orderBook.pollFirstEntry();
```

**Cas d'usage typique** : Order Book en trading (bids/asks triés par prix).

### ConcurrentSkipListSet

```java
ConcurrentSkipListSet<String> sortedSet = new ConcurrentSkipListSet<>();
sortedSet.add("C");
sortedSet.add("A");
sortedSet.add("B");

String first = sortedSet.first();  // "A"
String last = sortedSet.last();    // "C"
sortedSet.headSet("B");            // ["A"]
```

### Comparaison TreeMap vs ConcurrentSkipListMap

| | `TreeMap` | `ConcurrentSkipListMap` |
|---|---|---|
| Thread-safe | ✗ | ✓ |
| Algorithme | Red-Black Tree | Skip List |
| Complexité | O(log n) | O(log n) |
| Lock | Non applicable | **Lock-free** (CAS) |
| Navigation | ✓ | ✓ |

---

## 8. Atomic Variables

Le package `java.util.concurrent.atomic` fournit des variables atomiques via des **CPU instructions** (CAS, `LOCK XADD`, etc.) sans verrou applicatif.

### Types atomiques principaux

```java
AtomicInteger counter = new AtomicInteger(0);
AtomicLong timestamp = new AtomicLong(System.nanoTime());
AtomicBoolean flag = new AtomicBoolean(false);
AtomicReference<String> ref = new AtomicReference<>("initial");
```

### Opérations clés

```java
AtomicInteger ai = new AtomicInteger(10);

// Get & set
int val = ai.get();
ai.set(20);

// getAndIncrement / getAndDecrement
int before = ai.getAndIncrement();  // retourne 20, valeur devient 21
int after  = ai.incrementAndGet();  // valeur devient 22, retourne 22

// addAndGet
int result = ai.addAndGet(5);  // 22 + 5 = 27

// compareAndSet — CAS natif
boolean success = ai.compareAndSet(27, 100);  // si val == 27, set à 100

// compareAndExchange (Java 9+) — retourne la valeur observée
int witness = ai.compareAndExchangeAcquire(100, 200);
```

### AtomicReference — Objets immuables thread-safe

```java
AtomicReference<MarketData> marketRef = new AtomicReference<>(new MarketData(...));

// Mise à jour atomique avec CAS
MarketData current, updated;
do {
    current = marketRef.get();
    updated = current.withNewPrice(newPrice);  // crée un nouvel objet
} while (!marketRef.compareAndSet(current, updated));
```

### AtomicReferenceArray & AtomicIntegerArray

```java
AtomicIntegerArray prices = new AtomicIntegerArray(1000);

// Accès atomique par index
prices.set(0, 100);
int p = prices.get(0);
prices.compareAndSet(0, 100, 101);
prices.getAndAdd(0, 5);
```

### VarHandle (Java 9+) — Remplaçant moderne de Unsafe

```java
class Order {
    private volatile int status;

    private static final VarHandle STATUS;
    static {
        try {
            STATUS = MethodHandles.lookup()
                .findVarHandle(Order.class, "status", int.class);
        } catch (Exception e) { throw new Error(e); }
    }

    public boolean trySetProcessing() {
        // CAS avec sémantique mémoire explicite
        return STATUS.compareAndSet(this, 0, 1);
    }

    public void setDone() {
        STATUS.setRelease(this, 2);
    }

    public int getStatus() {
        return (int) STATUS.getAcquire(this);
    }
}
```

---

## 9. LongAdder & LongAccumulator

### Problème de AtomicLong sous haute contention

Avec beaucoup de threads en contention sur un `AtomicLong`, les CAS échouent et les threads retentent → **spin loop** coûteux.

```
Thread 1 ─→ CAS fail ─→ retry ─→ CAS fail ─→ retry ─→ CAS ok
Thread 2 ─→ CAS fail ─→ retry ─→ CAS ok
Thread 3 ─→ CAS ok
```

### LongAdder — Distribution de la contention

`LongAdder` maintient un tableau de **cellules** (`Cell[]`), chaque thread mappé sur une cellule différente. La valeur réelle = base + sum(cells).

```
Thread 1 → Cell[0] : +1, +1, +1
Thread 2 → Cell[1] : +1, +1
Thread 3 → Cell[2] : +1

sum() = base + cell[0] + cell[1] + cell[2]
```

```java
LongAdder counter = new LongAdder();

// Incrémenter (très rapide, peu de contention)
counter.increment();
counter.add(5);

// Lire la somme (moins précis en temps réel — snapshot)
long total = counter.sum();

// sumThenReset — atomique
long snapshot = counter.sumThenReset();

// reset
counter.reset();
```

### LongAdder vs AtomicLong

| | `AtomicLong` | `LongAdder` |
|---|---|---|
| Faible contention | ✓ Rapide | Overhead des cells |
| Haute contention | CAS spin | **Bien meilleur** |
| Lecture fréquente | Précise | **Approximative** |
| Cas d'usage | Séquences, IDs | Compteurs stats |

### LongAccumulator — Généralisation de LongAdder

```java
// Accumulateur avec une fonction custom (ex : max)
LongAccumulator maxLatency = new LongAccumulator(Long::max, 0L);

maxLatency.accumulate(150);
maxLatency.accumulate(200);
maxLatency.accumulate(50);

long max = maxLatency.get();  // 200

// Autre exemple : min
LongAccumulator minPrice = new LongAccumulator(Long::min, Long.MAX_VALUE);

// DoubleAdder & DoubleAccumulator pour les doubles
DoubleAdder totalAmount = new DoubleAdder();
totalAmount.add(99.5);
totalAmount.add(50.0);
double sum = totalAmount.sum();
```

---

## 10. Lock-Free & Wait-Free Patterns

### Définitions importantes

| Terme | Définition |
|---|---|
| **Blocking** | Un thread peut bloquer indéfiniment (verrous classiques) |
| **Lock-free** | Au moins un thread progresse en permanence |
| **Wait-free** | Chaque thread progresse en un nombre fini d'étapes |
| **Obstruction-free** | Un thread isolé progresse toujours |

### Pattern : Lock-Free Stack

```java
public class LockFreeStack<T> {
    private final AtomicReference<Node<T>> top = new AtomicReference<>();

    static class Node<T> {
        final T value;
        Node<T> next;
        Node(T v) { this.value = v; }
    }

    public void push(T value) {
        Node<T> newNode = new Node<>(value);
        Node<T> current;
        do {
            current = top.get();
            newNode.next = current;
        } while (!top.compareAndSet(current, newNode));
    }

    public T pop() {
        Node<T> current, next;
        do {
            current = top.get();
            if (current == null) return null;
            next = current.next;
        } while (!top.compareAndSet(current, next));
        return current.value;
    }
}
```

### Pattern : Disruptor (LMAX) — Ring Buffer Lock-Free

Le **Disruptor** est une librairie de Martin Fowler / LMAX pour les systèmes de trading haute fréquence. Principe :

```
                    ┌─────────────────────────────────┐
                    │         Ring Buffer              │
                    │  [0][1][2][3][4][5][6][7]        │
                    │   ↑                   ↑          │
                    │ Consumer          Producer       │
                    └─────────────────────────────────┘
```

- Tableau de taille puissance de 2 (pour masquage binaire rapide)
- **Pas de GC pressure** (objets pré-alloués)
- Cache-friendly (accès séquentiels)
- Séquences basées sur `AtomicLong` / `Unsafe`

```java
// Utilisation avec la librairie Disruptor
Disruptor<TradeEvent> disruptor = new Disruptor<>(
    TradeEvent::new,
    1024,          // ring buffer size (puissance de 2)
    DaemonThreadFactory.INSTANCE,
    ProducerType.SINGLE,
    new BusySpinWaitStrategy()  // stratégie d'attente (low latency)
);

disruptor.handleEventsWith((event, sequence, endOfBatch) -> {
    processOrder(event);
});

RingBuffer<TradeEvent> ring = disruptor.start();

// Publication
long seq = ring.next();
try {
    TradeEvent event = ring.get(seq);
    event.setOrderId(orderId);
    event.setPrice(price);
} finally {
    ring.publish(seq);
}
```

### False Sharing & Cache Line Padding

Un problème majeur en low latency : deux variables sur la **même cache line** (64 bytes) invalidée par des threads différents.

```java
// MAUVAIS : sequence et padding sur la même cache line
class BadSequence {
    volatile long value = 0;  // 8 bytes
    // autres champs proches...
}

// BON : padding pour éviter le false sharing
@Contended  // Java 8+ (nécessite -XX:-RestrictContended)
class PaddedSequence {
    volatile long value = 0;
}

// Ou manuellement :
class ManualPaddedSequence {
    long p1, p2, p3, p4, p5, p6, p7;  // 56 bytes de padding
    volatile long value = 0;           // 8 bytes → cache line complète
    long p8, p9, p10, p11, p12, p13, p14;
}
```

---

## 11. Java Memory Model & Happens-Before

### Règles Happens-Before fondamentales

Le **Java Memory Model (JMM)** définit quand les écriture d'un thread sont **visibles** aux autres.

```
Relation happens-before (→) :
- unlock(m) → lock(m)
- write(volatile x) → read(volatile x)
- Thread.start() → toute action dans le thread démarré
- fin d'un thread → Thread.join()
- x.notify() → attente réveillée par x.wait()
```

### volatile — Visibilité sans atomicité

```java
class FlagExample {
    private volatile boolean running = true;  // garantit la visibilité

    public void stop() {
        running = false;  // visible immédiatement par tous les threads
    }

    public void run() {
        while (running) {  // lit toujours la valeur fraîche
            doWork();
        }
    }
}
```

> `volatile` garantit la **visibilité** mais PAS l'**atomicité** des opérations composées (`i++` sur un volatile reste non-atomique).

### Sémantiques mémoire des Atomics (Java 9+ VarHandle)

```java
// Plain — aucune garantie mémoire
STATUS.set(this, value);                    // plain
int v = (int) STATUS.get(this);            // plain

// Opaque — ordre cohérent pour la variable seule
STATUS.setOpaque(this, value);
int v = (int) STATUS.getOpaque(this);

// Acquire/Release — synchronisation légère
STATUS.setRelease(this, value);   // toutes les lectures précédentes
int v = (int) STATUS.getAcquire(this); // toutes les lectures suivantes

// Volatile — full memory fence (le plus coûteux)
STATUS.setVolatile(this, value);
int v = (int) STATUS.getVolatile(this);
```

---

## 12. Bonnes pratiques Low Latency

### 1. Préférer les structures lock-free

```java
// ✅ Lock-free
ConcurrentLinkedQueue<Event> queue = new ConcurrentLinkedQueue<>();

// ❌ Éviter synchronized sur hot path
synchronized (lock) {
    list.add(event);
}
```

### 2. Éviter l'allocation en hot path (GC pressure)

```java
// ❌ Crée un objet à chaque tick
queue.offer(new MarketTick(price, volume));

// ✅ Object pool
MarketTick tick = pool.borrow();
tick.set(price, volume);
queue.offer(tick);
// ... après traitement :
pool.recycle(tick);
```

### 3. Dimensionner correctement les queues

```java
// Puissance de 2 → masquage binaire au lieu de modulo
// 1024, 2048, 4096, 8192...
ArrayBlockingQueue<Order> q = new ArrayBlockingQueue<>(4096);
```

### 4. Choisir la bonne WaitStrategy (Disruptor)

| Strategy | CPU | Latency | Cas d'usage |
|---|---|---|---|
| `BusySpinWaitStrategy` | 100% | Ultra-faible | Trading HFT |
| `YieldingWaitStrategy` | Élevé | Faible | Latency sensible |
| `SleepingWaitStrategy` | Faible | Plus haute | Throughput |
| `BlockingWaitStrategy` | Minimal | Haute | CPU limité |

### 5. Éviter la contention sur les compteurs

```java
// ❌ Haute contention sous charge
AtomicLong counter = new AtomicLong();
// Des centaines de threads font counter.incrementAndGet()

// ✅ LongAdder pour les stats
LongAdder counter = new LongAdder();
counter.increment();  // Peu de contention
```

### 6. Thread affinity & CPU pinning

```java
// Avec la librairie Java Thread Affinity
try (AffinityLock lock = AffinityLock.acquireCore()) {
    // Ce thread tourne sur un cœur dédié
    while (running) {
        processEvents();
    }
}
```

### 7. Pré-allocation et warm-up JIT

```java
// Warm-up : exécuter le hot path au démarrage
// pour que le JIT compile les chemins critiques
for (int i = 0; i < 100_000; i++) {
    processOrder(warmupOrder);
}
// Après le warm-up, le code est compilé et optimisé
```

---

## 13. Comparatif & Choisir la bonne structure

### Arbre de décision

```
Besoin d'une Map concurrent ?
├── Lecture dominante, peu d'écritures ?
│   └── ConcurrentHashMap (avec compute/merge)
├── Map triée nécessaire ?
│   └── ConcurrentSkipListMap
└── Sinon → ConcurrentHashMap

Besoin d'une Queue/List concurrent ?
├── Non-bloquante (pas d'attente) ?
│   ├── FIFO → ConcurrentLinkedQueue
│   └── Double-ended → ConcurrentLinkedDeque
├── Bloquante (Producer-Consumer) ?
│   ├── Bounded, 1 prod / 1 cons → ArrayBlockingQueue
│   ├── Bounded, multi prod/cons → LinkedBlockingQueue
│   ├── Handoff direct → SynchronousQueue
│   └── Handoff + queue → LinkedTransferQueue
└── Lectures >> Écritures ?
    └── CopyOnWriteArrayList

Besoin d'un compteur concurrent ?
├── Faible contention, lecture précise → AtomicLong
├── Haute contention, stats → LongAdder
└── Opération custom (max, min) → LongAccumulator
```

### Tableau comparatif final

| Structure | Lock-Free | Bounded | Ordered | Latency | Cas d'usage principal |
|---|---|---|---|---|---|
| `ConcurrentHashMap` | Lecture oui | ✗ | ✗ | Très faible | Cache, registry |
| `ConcurrentSkipListMap` | ✓ | ✗ | ✓ | Faible | Order book, index trié |
| `ConcurrentLinkedQueue` | ✓ | ✗ | FIFO | Ultra-faible | Event bus, task queue |
| `ArrayBlockingQueue` | ✗ | ✓ | FIFO | Faible | Producer-consumer borné |
| `LinkedBlockingQueue` | ✗ | Opt. | FIFO | Faible | Thread pool queues |
| `LinkedTransferQueue` | ✓ | ✗ | FIFO | Ultra-faible | Handoff + queue |
| `SynchronousQueue` | ✓ | 0 | N/A | Ultra-faible | Thread handoff direct |
| `CopyOnWriteArrayList` | Lecture oui | ✗ | ✓ | Read: ultra-faible | Listeners, config |
| `AtomicLong` | ✓ | N/A | N/A | Très faible | Séquences, IDs |
| `LongAdder` | ✓ | N/A | N/A | Ultra-faible | Compteurs stats HPC |

---

## Ressources complémentaires

- **Livre** : *Java Concurrency in Practice* — Brian Goetz
- **Livre** : *The Art of Multiprocessor Programming* — Herlihy & Shavit
- **Librairie** : LMAX Disruptor — [github.com/LMAX-Exchange/disruptor](https://github.com/LMAX-Exchange/disruptor)
- **Librairie** : JCTools — collections concurrentes optimisées pour JVM
- **JEP** : Project Loom (Virtual Threads, Java 21+)
- **Tool** : JMH (Java Microbenchmark Harness) pour mesurer la latency réelle

---

# Questions / Réponses Orales — Concurrent Collections & Thread-Safe Java
### Low Latency Programming — Préparation Entretien

---

## Table des matières

1. [Fondamentaux & Concepts Généraux](#1-fondamentaux--concepts-généraux)
2. [ConcurrentHashMap](#2-concurrenthashmap)
3. [ConcurrentLinkedQueue & ConcurrentLinkedDeque](#3-concurrentlinkedqueue--concurrentlinkeddeque)
4. [CopyOnWriteArrayList & CopyOnWriteArraySet](#4-copyonwritearraylist--copyonwritearrayset)
5. [BlockingQueue](#5-blockingqueue)
6. [LinkedTransferQueue & SynchronousQueue](#6-linkedtransferqueue--synchronousqueue)
7. [ConcurrentSkipListMap & ConcurrentSkipListSet](#7-concurrentskiplistmap--concurrentskiplistset)
8. [Atomic Variables & VarHandle](#8-atomic-variables--varhandle)
9. [LongAdder & LongAccumulator](#9-longadder--longaccumulator)
10. [Lock-Free, Wait-Free & Disruptor](#10-lock-free-wait-free--disruptor)
11. [Java Memory Model & Happens-Before](#11-java-memory-model--happens-before)
12. [Low Latency — Bonnes pratiques](#12-low-latency--bonnes-pratiques)
13. [Questions Avancées & Scénarios](#13-questions-avancées--scénarios)

---

## 1. Fondamentaux & Concepts Généraux

---

**Q1. Pourquoi ne pas utiliser `HashMap` en environnement multi-thread ?**

`HashMap` n'est pas thread-safe. En cas d'accès concurrent, on peut avoir des **data races** sur les buckets internes, des **ConcurrentModificationException**, ou pire, une **boucle infinie** lors d'un rehashing concurrent (bug historique de Java < 8). Le résultat est non déterministe et imprévisible.

---

**Q2. Quelle est la différence entre `Collections.synchronizedMap` et `ConcurrentHashMap` ?**

`synchronizedMap` pose un **verrou global** (`synchronized`) sur toute la map à chaque opération, y compris les lectures. Cela crée un goulot d'étranglement : un seul thread à la fois.

`ConcurrentHashMap` utilise un **verrou au niveau du bin** (nœud individuel) pour les écritures, et des lectures **totalement lock-free** grâce à `volatile`. Le débit est bien supérieur sous haute concurrence.

---

**Q3. Qu'est-ce que le CAS (Compare-And-Swap) ?**

Le **CAS** est une instruction atomique CPU (`CMPXCHG` sur x86) qui : lit une valeur, la compare à une valeur attendue, et si elles sont égales, la remplace par une nouvelle valeur — le tout de manière **atomique et indivisible**.

En Java, il est exposé via `AtomicInteger.compareAndSet(expected, update)`. C'est la base de toutes les structures **lock-free**.

```
CAS(adresse, expected, new) :
  si *adresse == expected → *adresse = new, retourne true
  sinon → retourne false (aucune modification)
```

---

**Q4. Quelle est la différence entre lock-free, wait-free et blocking ?**

- **Blocking** : un thread peut bloquer indéfiniment en attendant un verrou (ex : `synchronized`).
- **Lock-free** : au moins un thread dans le système fait toujours des progrès. Les autres peuvent "spinner" mais le système avance.
- **Wait-free** : chaque thread individuel termine son opération en un nombre **fini** d'étapes, quoi qu'il arrive.

En pratique, les structures de `java.util.concurrent` sont majoritairement **lock-free**.

---

**Q5. Qu'est-ce qu'une `race condition` ?**

C'est une situation où le résultat d'un programme dépend de l'**ordre d'exécution** de threads concurrents. Le cas le plus classique est le **check-then-act** :

```java
if (!map.containsKey(k)) {   // Thread A vérifie
    // Thread B insert ici !
    map.put(k, value);        // Thread A insère en double
}
```

La solution : utiliser des opérations **atomiques** comme `computeIfAbsent`.

---

**Q6. Qu'est-ce qu'un `data race` ? Est-ce la même chose qu'une `race condition` ?**

Non, ce sont deux choses distinctes.

- **Data race** : deux threads accèdent à la même variable **sans synchronisation**, et au moins l'un écrit. Le Java Memory Model rend le comportement **indéfini**.
- **Race condition** : bug logique dû à un ordre d'exécution imprévu, même avec une synchronisation partielle.

Un programme peut avoir une race condition sans data race (ex : `AtomicInteger` mal utilisé), ou un data race sans race condition visible (par chance).

---

**Q7. Qu'est-ce que le package `java.util.concurrent` apporte par rapport à `synchronized` ?**

`java.util.concurrent` offre :
- Des structures de données **thread-safe optimisées** (`ConcurrentHashMap`, `ConcurrentLinkedQueue`...)
- Des **primitives de synchronisation** évoluées (`ReentrantLock`, `Semaphore`, `CountDownLatch`...)
- Des **executors** et thread pools
- Des **atomic variables** basées sur CAS
- Des **collections bloquantes** pour le pattern Producer-Consumer

Le tout avec de meilleures performances que `synchronized` pur dans la majorité des cas de concurrence élevée.

---

**Q8. Qu'est-ce qu'un `weakly consistent iterator` ?**

C'est le type d'itérateur utilisé par les collections de `java.util.concurrent`. Contrairement au `fail-fast iterator` de `ArrayList` ou `HashMap`, il :
- Ne lève **jamais** `ConcurrentModificationException`
- Peut refléter ou non les modifications faites **après** sa création
- Garantit que chaque élément est visité **au plus une fois**

C'est un compromis acceptable en environnement concurrent car on évite les exceptions au prix d'une vue légèrement "stale" de la collection.

---

**Q9. Qu'est-ce que le `false sharing` ?**

Le **false sharing** survient quand deux variables distinctes partagent la **même cache line** (64 bytes sur x86). Quand un thread modifie sa variable, il invalide la cache line entière pour tous les autres threads, même si leurs variables ne sont pas modifiées.

Résultat : des **cache misses** constants et des ralentissements majeurs en low latency.

Solution : **padding** manuel ou annotation `@Contended` (Java 8+).

---

**Q10. Qu'est-ce que la `GC pressure` et pourquoi est-ce critique en low latency ?**

La **GC pressure** désigne la fréquence à laquelle le **Garbage Collector** est déclenché, principalement à cause d'allocations fréquentes d'objets à courte durée de vie.

En low latency, un GC pause (même de quelques millisecondes) est inacceptable. On cherche à **minimiser les allocations** en hot path : object pooling, réutilisation de buffers, structures pré-allouées (ex : Disruptor).

---

## 2. ConcurrentHashMap

---

**Q11. Comment `ConcurrentHashMap` gère-t-il la concurrence en Java 8+ ?**

En Java 8+, `ConcurrentHashMap` abandonne le segment locking de Java 7 pour deux mécanismes :
- **CAS** sur le premier nœud d'un bin vide (insertion sans verrou)
- **`synchronized` sur le premier nœud** du bin si celui-ci est non vide (verrou fin, au niveau du bin)

Les **lectures** sont totalement **lock-free** grâce à des champs `volatile`. Ainsi, N threads peuvent lire en parallèle sans aucun contention.

---

**Q12. Pourquoi `ConcurrentHashMap` n'accepte-t-il pas les clés ou valeurs `null` ?**

C'est un choix de design délibéré de Doug Lea. Dans un contexte concurrent, `null` crée une ambiguïté : si `map.get(key)` retourne `null`, est-ce parce que la clé n'existe pas, ou parce que la valeur associée est `null` ?

Avec `HashMap`, on peut appeler `containsKey()` pour lever l'ambiguïté. Mais en contexte concurrent, entre `get()` et `containsKey()`, un autre thread peut modifier la map. `ConcurrentHashMap` élimine ce problème en interdisant `null`.

---

**Q13. Quelle est la différence entre `put` et `putIfAbsent` dans `ConcurrentHashMap` ?**

- `put(k, v)` : insère ou remplace la valeur, **toujours**.
- `putIfAbsent(k, v)` : insère **uniquement si la clé est absente**, de manière **atomique**. Retourne la valeur existante si la clé est déjà présente, `null` sinon.

`putIfAbsent` évite le pattern non-atomique `if (!map.containsKey(k)) map.put(k, v)`.

---

**Q14. Quand utiliser `compute` vs `merge` vs `computeIfAbsent` ?**

- **`computeIfAbsent(k, fn)`** : calcule et insère une valeur **seulement si la clé est absente**. Idéal pour le lazy initialization (cache).
- **`computeIfPresent(k, fn)`** : met à jour la valeur **seulement si la clé existe**. Idéal pour incrémenter un compteur existant.
- **`compute(k, fn)`** : appelé **toujours**, que la clé existe ou non. La fonction reçoit `(key, oldValue)` où `oldValue` peut être `null`.
- **`merge(k, v, fn)`** : si la clé est absente, insère `v`. Si elle existe, applique `fn(existingValue, v)`. Idéal pour des opérations comme `Integer::sum`.

---

**Q15. Pourquoi `size()` n'est-il pas toujours fiable sur `ConcurrentHashMap` ?**

Parce que la taille peut changer entre le moment où elle est calculée et le moment où elle est retournée — d'autres threads peuvent avoir inséré ou supprimé des éléments entretemps. `size()` retourne une **estimation à un instant T**.

Pour des maps de grande taille, il faut préférer `mappingCount()` qui retourne un `long` et est mieux adapté aux grandes collections.

---

**Q16. Expliquez le `parallelismThreshold` dans les opérations bulk comme `forEach` ou `reduce`.**

C'est le seuil à partir duquel les opérations bulk sont parallélisées. Il représente le nombre **minimum d'éléments par tâche parallèle**.

- `parallelismThreshold = 1` → parallélisme maximal (utilise tous les cœurs via le ForkJoinPool commun).
- `parallelismThreshold = Long.MAX_VALUE` → exécution séquentielle.

En pratique, on choisit un seuil qui compense l'overhead du fork/join par rapport au gain de parallélisme.

---

**Q17. Comment éviter le problème de "check-then-act" avec `ConcurrentHashMap` ?**

En remplaçant systématiquement les patterns non-atomiques par des opérations atomiques :

```java
// ❌ Non atomique
if (!map.containsKey(k)) map.put(k, new ArrayList<>());
map.get(k).add(item);

// ✅ Atomique
map.computeIfAbsent(k, key -> new ArrayList<>()).add(item);
```

La règle : toute séquence `get → condition → put` doit être remplacée par `compute`, `merge` ou `putIfAbsent`.

---

**Q18. Quelle est la différence entre le segment locking de Java 7 et le bin locking de Java 8 dans `ConcurrentHashMap` ?**

En **Java 7**, la map était divisée en **16 segments** par défaut, chacun étant un mini-`HashMap` avec son propre `ReentrantLock`. La concurrence maximale était donc de 16 threads en écriture simultanée.

En **Java 8**, chaque **bin individuel** (tête de liste chaînée) possède son propre verrou `synchronized`. Avec potentiellement des milliers de bins, la concurrence est bien plus fine, et le CAS est utilisé pour les bins vides (sans verrou du tout).

---

## 3. ConcurrentLinkedQueue & ConcurrentLinkedDeque

---

**Q19. Sur quel algorithme est basé `ConcurrentLinkedQueue` ?**

Sur l'algorithme de **Michael & Scott (1996)**, un algorithme de queue FIFO **lock-free** basé sur le CAS. Il utilise deux pointeurs atomiques, `head` et `tail`, et permet à plusieurs threads de `offer` et `poll` simultanément sans verrou.

La propriété clé : si un CAS échoue (un autre thread a agi), le thread recommence la tentative — c'est le spin loop caractéristique des algorithmes lock-free.

---

**Q20. Pourquoi `size()` est-il O(n) sur `ConcurrentLinkedQueue` ?**

Parce qu'il n'y a **pas de compteur de taille maintenu** en interne. Maintenir un compteur atomique imposerait un CAS supplémentaire sur chaque `offer` et `poll`, créant de la contention. La décision de design est de sacrifier `size()` pour maximiser les performances des opérations courantes.

À la place, utiliser `isEmpty()` qui vérifie juste si `head.next == null`, une opération O(1).

---

**Q21. Quelle est la différence entre `offer`, `add`, `poll` et `peek` ?**

| Méthode | Comportement si plein/vide |
|---|---|
| `offer(e)` | Retourne `false` si plein (toujours `true` pour unbounded) |
| `add(e)` | Lève `IllegalStateException` si plein |
| `poll()` | Retourne `null` si vide |
| `remove()` | Lève `NoSuchElementException` si vide |
| `peek()` | Retourne `null` si vide, **sans retirer** l'élément |
| `element()` | Lève `NoSuchElementException` si vide, sans retirer |

---

**Q22. Quand choisir `ConcurrentLinkedDeque` plutôt que `ConcurrentLinkedQueue` ?**

Quand on a besoin d'accéder aux **deux extrémités** de la structure : insérer ou retirer à la tête ET à la queue. Cas d'usage typiques :
- **Work-stealing** : les workers consomment par la queue, et peuvent "voler" des tâches par la tête des autres workers.
- **LRU cache** : déplacer un élément accédé en tête de la deque.
- **Double-ended buffers** dans les systèmes de messaging.

---

## 4. CopyOnWriteArrayList & CopyOnWriteArraySet

---

**Q23. Comment fonctionne `CopyOnWriteArrayList` en interne ?**

Chaque opération d'écriture (`add`, `remove`, `set`) :
1. Acquiert un **verrou** (`ReentrantLock`)
2. Crée une **copie complète** du tableau interne
3. Applique la modification sur la copie
4. Remplace la référence interne par le nouveau tableau (via un champ `volatile`)
5. Libère le verrou

Les lectures n'acquièrent **jamais de verrou** car elles opèrent sur un snapshot du tableau, qui est immuable une fois publié.

---

**Q24. Pourquoi `CopyOnWriteArrayList` est-il déconseillé pour des listes à écriture fréquente ?**

Parce que chaque écriture implique une **copie complète du tableau** en mémoire. Pour une liste de 10 000 éléments, chaque `add` alloue un nouveau tableau de 10 001 éléments. Cela crée :
- Une **GC pressure** élevée (anciens tableaux à collecter)
- Un coût d'écriture **O(n)** au lieu de O(1)

Il est idéal **uniquement** pour les listes où les lectures dominent massivement (ratio > 100:1 lecture/écriture).

---

**Q25. Un `CopyOnWriteArrayList` peut-il lancer une `ConcurrentModificationException` ?**

**Non.** C'est l'un de ses atouts majeurs. L'itérateur travaille sur un snapshot immuable du tableau capturé au moment de l'appel à `iterator()`. Les modifications ultérieures n'affectent pas l'itération en cours.

La contrepartie : l'itérateur peut ne pas refléter les modifications récentes.

---

**Q26. Quel est le pattern typique d'utilisation de `CopyOnWriteArrayList` ?**

Le pattern **listeners/observers** :

```java
CopyOnWriteArrayList<EventListener> listeners = new CopyOnWriteArrayList<>();

// Thread admin — rare
listeners.add(new LoggingListener());
listeners.remove(oldListener);

// Milliers de threads en hot path — frequent, lock-free
for (EventListener l : listeners) {
    l.onEvent(event);
}
```

Les ajouts/suppressions de listeners sont rares. La notification est fréquente et lock-free.

---

## 5. BlockingQueue

---

**Q27. Qu'est-ce que le pattern Producer-Consumer et quel rôle joue `BlockingQueue` ?**

Le **Producer-Consumer** est un pattern où un ou plusieurs threads producteurs génèrent des tâches, et un ou plusieurs threads consommateurs les traitent. `BlockingQueue` joue le rôle de **buffer thread-safe** entre eux :

- Le producteur appelle `put()` — se **bloque** si la queue est pleine (backpressure).
- Le consommateur appelle `take()` — se **bloque** si la queue est vide.

Ce pattern découple la vitesse de production et de consommation.

---

**Q28. Quelle est la différence entre `ArrayBlockingQueue` et `LinkedBlockingQueue` ?**

- **`ArrayBlockingQueue`** : tableau fixe, **toujours bounded**, **un seul verrou** partagé entre put et take. Moins de mémoire, plus simple.
- **`LinkedBlockingQueue`** : liste chaînée, optionnellement bounded, **deux verrous séparés** (putLock + takeLock). Meilleur débit quand il y a un producteur ET un consommateur actifs simultanément.

En pratique, `LinkedBlockingQueue` est souvent plus rapide en débit pur, mais `ArrayBlockingQueue` est plus prévisible en mémoire.

---

**Q29. Qu'est-ce que le mode `fair` dans `ArrayBlockingQueue` et quand l'utiliser ?**

En mode **fair** (`new ArrayBlockingQueue<>(n, true)`), les threads bloqués en attente sont servis dans l'ordre **FIFO**. Cela garantit qu'aucun thread n'est "affamé" (starvation).

En mode **unfair** (défaut), l'OS choisit arbitrairement quel thread réveiller, ce qui est plus rapide mais peut causer de la starvation.

On utilise le mode fair dans des systèmes où l'**équité entre threads** est une exigence fonctionnelle, au prix d'une performance légèrement réduite.

---

**Q30. Quelle est la différence entre `put` et `offer` dans une `BlockingQueue` ?**

- `put(e)` : **bloque indéfiniment** si la queue est pleine.
- `offer(e)` : retourne **immédiatement** `false` si la queue est pleine (non-bloquant).
- `offer(e, timeout, unit)` : attend au maximum `timeout` avant de retourner `false`.

De même côté consommateur :
- `take()` : bloque indéfiniment si vide.
- `poll()` : retourne `null` immédiatement si vide.
- `poll(timeout, unit)` : attend au maximum `timeout`.

---

**Q31. À quoi sert `PriorityBlockingQueue` ?**

C'est une `BlockingQueue` **non bornée** qui ordonne ses éléments selon leur priorité naturelle (Comparable) ou un Comparator fourni. Le `take()` retourne toujours l'élément de plus haute priorité.

Cas d'usage : traitement d'ordres financiers où les ordres "market" ont priorité sur les ordres "limit", files de tâches avec niveaux de priorité.

> Attention : pas de garantie d'équité entre éléments de même priorité.

---

**Q32. Comment fonctionne `DelayQueue` ?**

`DelayQueue` est une queue où les éléments ne peuvent être retirés **qu'après leur délai d'expiration**. Chaque élément implémente `Delayed` et sa méthode `getDelay()` retourne le temps restant avant d'être disponible.

En interne, elle est basée sur un `PriorityQueue` (l'élément avec le délai le plus court est en tête).

Cas d'usage : schedulers de tâches différées, timeouts, retry avec backoff.

---

## 6. LinkedTransferQueue & SynchronousQueue

---

**Q33. Comment fonctionne `SynchronousQueue` ?**

`SynchronousQueue` a une capacité de **zéro**. C'est un **point de rendez-vous** entre deux threads :
- Le producteur appelle `put()` et **bloque** jusqu'à ce qu'un consommateur appelle `take()`.
- Le consommateur appelle `take()` et **bloque** jusqu'à ce qu'un producteur appelle `put()`.

Le transfert est **direct** entre les deux threads, sans stockage intermédiaire. C'est ce qu'utilise `Executors.newCachedThreadPool()` en interne.

---

**Q34. Quelle est la différence entre `LinkedTransferQueue` et `SynchronousQueue` ?**

`LinkedTransferQueue` combine les deux comportements :

- Si un consommateur attend déjà → **handoff direct** (comme `SynchronousQueue`).
- Si aucun consommateur n'attend → l'élément est **mis en queue** (comme `LinkedBlockingQueue`).

`transfer(e)` bloque jusqu'à ce qu'un consommateur prenne l'élément.
`tryTransfer(e)` tente le handoff direct sans bloquer.

`LinkedTransferQueue` est généralement préféré à `SynchronousQueue` quand on veut un comportement hybride.

---

**Q35. Dans quel cas utiliser `LinkedTransferQueue` plutôt que `LinkedBlockingQueue` ?**

Quand on veut s'assurer que l'élément est **réellement pris en charge** par un consommateur avant de continuer. Avec `LinkedBlockingQueue`, `put()` retourne dès que l'élément est dans la queue, pas nécessairement quand il est traité. Avec `transfer()`, on a la garantie du **handoff effectif**.

Utile pour des systèmes où on veut éviter d'accumuler des éléments non traités.

---

## 7. ConcurrentSkipListMap & ConcurrentSkipListSet

---

**Q36. Qu'est-ce qu'une Skip List et comment garantit-elle O(log n) ?**

Une **Skip List** est une liste chaînée multi-niveaux. Le niveau 1 contient tous les éléments (liste complète). Chaque niveau supérieur contient un **sous-ensemble aléatoire** des éléments du niveau inférieur (environ 50% par niveau).

La recherche commence au niveau le plus haut et descend : on avance tant que l'élément suivant est inférieur à la cible, puis on descend d'un niveau. En moyenne, cela donne **O(log n)** comparaisons, comme un arbre binaire équilibré, mais sans la complexité de rééquilibrage.

---

**Q37. Pourquoi `ConcurrentSkipListMap` est-il thread-safe sans verrou global ?**

Parce qu'il utilise le **CAS** pour toutes ses modifications. Insérer ou supprimer un nœud dans la skip list se fait via des pointeurs, et chaque mise à jour de pointeur est une opération CAS atomique. Si le CAS échoue (un autre thread a modifié la structure), l'opération recommence.

Il n'y a pas de verrou global : plusieurs threads peuvent naviguer et modifier des parties différentes de la skip list simultanément.

---

**Q38. Quelle est la différence entre `ConcurrentSkipListMap` et `ConcurrentHashMap` ?**

| | `ConcurrentHashMap` | `ConcurrentSkipListMap` |
|---|---|---|
| Ordre des clés | Aucun | **Trié** (naturel ou Comparator) |
| Complexité | O(1) amortie | O(log n) |
| Navigation | Non | `firstKey`, `lastKey`, `headMap`, `tailMap`, `floorKey`, `ceilingKey`... |
| Cas d'usage | Cache, registry | Order book, index trié |

Si l'ordre n'est pas nécessaire, `ConcurrentHashMap` est plus rapide. Si on a besoin de navigation par plage ou de tri, `ConcurrentSkipListMap` est le choix.

---

**Q39. Donnez un cas d'usage concret de `ConcurrentSkipListMap` en finance.**

Un **order book** en trading. Les prix des ordres d'achat (bids) sont triés du plus haut au plus bas, les ordres de vente (asks) du plus bas au plus haut. `ConcurrentSkipListMap` permet :
- `firstEntry()` → meilleur ask (prix le plus bas à la vente)
- `lastEntry()` → meilleur bid (prix le plus haut à l'achat)
- `subMap(minPrice, maxPrice)` → tous les ordres dans une plage de prix
- `pollFirstEntry()` → exécuter le meilleur ordre de manière atomique

Le tout de façon thread-safe et triée, parfait pour des market makers multi-threadés.

---

**Q40. Qu'est-ce que `headMap`, `tailMap` et `subMap` dans `ConcurrentSkipListMap` ?**

Ce sont des **vues** (views) de la map, pas des copies :

- `headMap(toKey)` : tous les éléments dont la clé est **strictement inférieure** à `toKey`.
- `tailMap(fromKey)` : tous les éléments dont la clé est **supérieure ou égale** à `fromKey`.
- `subMap(fromKey, toKey)` : tous les éléments dont la clé est dans `[fromKey, toKey[`.

Ces vues sont **live** : elles reflètent les modifications de la map originale et sont elles-mêmes thread-safe.

---

## 8. Atomic Variables & VarHandle

---

**Q41. Quelle est la différence entre `volatile` et `AtomicInteger` ?**

- `volatile` garantit la **visibilité** : toute écriture sur une variable `volatile` est immédiatement visible par les autres threads. Mais les opérations composées comme `i++` (qui est `read + add + write`) restent **non atomiques**.
- `AtomicInteger` garantit à la fois la **visibilité ET l'atomicité** des opérations composées grâce au CAS. `incrementAndGet()` est une seule opération atomique indivisible.

---

**Q42. Expliquez le problème ABA du CAS.**

Le problème **ABA** survient quand :
1. Thread 1 lit la valeur **A**.
2. Thread 2 change la valeur de A → B → A.
3. Thread 1 fait son CAS : il voit **A** et le CAS réussit — mais la valeur a changé entre-temps.

Pour certaines structures (ex : lock-free stack), le retour à A peut causer des bugs.

Solution : utiliser `AtomicStampedReference` ou `AtomicMarkableReference` qui associent un **version stamp** à la valeur, empêchant le CAS de réussir si la version a changé, même si la valeur est revenue à A.

---

**Q43. Qu'est-ce que `compareAndExchange` (Java 9+) et en quoi diffère-t-il de `compareAndSet` ?**

- `compareAndSet(expected, update)` → retourne `boolean` (succès ou échec).
- `compareAndExchange(expected, update)` → retourne la **valeur observée** (witness value), qu'il y ait succès ou échec.

`compareAndExchange` est plus utile dans les boucles de retry car on obtient directement la valeur actuelle sans avoir à refaire un `get()` séparé :

```java
int witness = ai.compareAndExchange(expected, newValue);
if (witness != expected) {
    // witness contient la valeur réelle → on peut réutiliser directement
}
```

---

**Q44. Qu'est-ce que `VarHandle` (Java 9+) et pourquoi remplace-t-il `Unsafe` ?**

`VarHandle` est une API standard et sécurisée pour manipuler des variables avec différentes **sémantiques mémoire** (plain, opaque, acquire/release, volatile). Il remplace `sun.misc.Unsafe` qui était :
- Non documenté et non officiel
- Dangereux (accès direct à la mémoire)
- Susceptible d'être supprimé

`VarHandle` permet de choisir précisément la sémantique mémoire nécessaire, ce qui est crucial en low latency pour éviter des memory barriers inutiles.

---

**Q45. Expliquez les sémantiques mémoire : plain, opaque, acquire/release, volatile.**

Du plus faible (moins coûteux) au plus fort (plus de garanties) :

- **Plain** : aucune garantie de visibilité ni d'ordre. Comme un accès normal non `volatile`.
- **Opaque** : cohérence pour la variable elle-même uniquement. L'écriture finira par être vue.
- **Acquire** (pour les lectures) / **Release** (pour les écritures) : synchronisation unidirectionnelle. Un `setRelease` garantit que toutes les écritures précédentes sont visibles pour un thread qui fait un `getAcquire` sur la même variable.
- **Volatile** : full memory fence bidirectionnel. Le plus coûteux, équivalent à `volatile` Java.

En pratique : utiliser **Acquire/Release** pour les handoffs entre threads, **Volatile** seulement quand c'est strictement nécessaire.

---

**Q46. Qu'est-ce qu'un `AtomicReference` et quand l'utiliser ?**

`AtomicReference<T>` permet des mises à jour atomiques d'une **référence d'objet**. Utile pour les patterns où on maintient un objet immuable et on le remplace atomiquement :

```java
AtomicReference<Config> config = new AtomicReference<>(initialConfig);

// Mise à jour atomique sans verrou
Config current, updated;
do {
    current = config.get();
    updated = current.withNewValue(x);
} while (!config.compareAndSet(current, updated));
```

Pattern classique : configuration hot-reload, state machines, snapshots immuables.

---

**Q47. Qu'est-ce qu'`AtomicIntegerFieldUpdater` et quel est son avantage ?**

`AtomicIntegerFieldUpdater` permet d'appliquer des opérations atomiques sur un **champ `volatile int`** d'un objet existant, **sans wrapper** l'entier dans un `AtomicInteger`.

Avantage : économie mémoire significative quand on a des millions d'objets avec un champ atomique. Chaque `AtomicInteger` est un objet Java (header d'objet = 12-16 bytes). Un `int volatile` ne coûte que 4 bytes.

```java
private static final AtomicIntegerFieldUpdater<Order> STATUS_UPDATER =
    AtomicIntegerFieldUpdater.newUpdater(Order.class, "status");

volatile int status = 0;  // dans Order

STATUS_UPDATER.compareAndSet(orderInstance, 0, 1);
```

---

## 9. LongAdder & LongAccumulator

---

**Q48. Pourquoi `LongAdder` est-il plus performant que `AtomicLong` sous haute contention ?**

`AtomicLong` utilise un seul entier partagé. Sous haute contention, les CAS de N threads entrent en compétition et échouent en cascade → **spin loop** coûteux.

`LongAdder` maintient un tableau de **cellules** (Cell[]), chaque thread opérant sur sa cellule dédiée (mappée par le hash du thread). Les CAS sont distribués → **quasi-zéro contention**. La valeur finale est la **somme** de la base et de toutes les cellules.

---

**Q49. Quel est l'inconvénient de `LongAdder` par rapport à `AtomicLong` ?**

- `sum()` est une **approximation** : elle additionne toutes les cellules, mais pendant ce temps d'autres threads peuvent les modifier. La valeur lue peut ne pas être atomiquement cohérente.
- `LongAdder` consomme **plus de mémoire** (tableau de cellules) et a un overhead légèrement plus élevé à **faible contention** (là où `AtomicLong` est optimal).

Règle : utiliser `AtomicLong` pour les séquences et IDs (précision requise), `LongAdder` pour les compteurs statistiques.

---

**Q50. Qu'est-ce que `sumThenReset()` dans `LongAdder` et à quoi sert-il ?**

`sumThenReset()` effectue de manière **quasi-atomique** : lit la somme totale, puis remet les cellules à zéro.

Utile pour les **métriques périodiques** : toutes les secondes, on lit le compteur et on le remet à zéro pour compter sur la prochaine fenêtre. C'est plus efficace que de créer un nouveau `LongAdder` à chaque période.

> Quasi-atomique car entre le reset de la base et le reset des cellules, d'autres threads peuvent incrémenter.

---

**Q51. Comment fonctionne `LongAccumulator` et quand l'utiliser ?**

`LongAccumulator` est une généralisation de `LongAdder` pour n'importe quelle **opération binaire associative et commutative** : max, min, multiplication, AND, OR...

```java
LongAccumulator maxVal = new LongAccumulator(Long::max, Long.MIN_VALUE);
maxVal.accumulate(150);  // max(MIN_VALUE, 150) = 150
maxVal.accumulate(200);  // max(150, 200) = 200
long result = maxVal.get();  // 200
```

Idéal pour les statistiques en temps réel : latence maximale observée, prix max/min, compteurs personnalisés.

---

## 10. Lock-Free, Wait-Free & Disruptor

---

**Q52. Implémentez un compteur thread-safe lock-free.**

```java
public class LockFreeCounter {
    private final AtomicLong count = new AtomicLong(0);

    public long increment() {
        return count.incrementAndGet();  // CAS-based, lock-free
    }

    public long get() {
        return count.get();
    }
}
```

Pour une version wait-free sous faible contention, c'est déjà wait-free en pratique car un seul CAS réussit toujours (pas de retry si pas de contention).

---

**Q53. Qu'est-ce que le Disruptor et pourquoi est-il utilisé en trading ?**

Le **LMAX Disruptor** est une librairie de messaging inter-thread ultra-performante basée sur un **Ring Buffer** pré-alloué.

Avantages par rapport aux `BlockingQueue` classiques :
- **Pas d'allocation en hot path** : les objets sont pré-alloués → zéro GC pressure.
- **Cache-friendly** : accès séquentiels dans le ring buffer → préfetching CPU optimal.
- **Lock-free** : producteurs et consommateurs avancent leurs séquences via des `AtomicLong`.
- **Throughput** : plusieurs millions d'événements par seconde avec latences en microsecondes.

---

**Q54. Qu'est-ce qu'un Ring Buffer et comment fonctionne-t-il ?**

Un **Ring Buffer** est un tableau circulaire de taille fixe (puissance de 2). On utilise des **séquences** (entiers toujours croissants) pour pointer les positions :

```
Taille = 8
Position = séquence & (taille - 1)  → masquage binaire (pas de modulo !)

Séquence 0 → index 0
Séquence 8 → index 0 (cycle suivant)
```

Le producteur avance sa séquence pour réclamer un slot. Le consommateur avance la sienne après traitement. Le producteur ne peut pas dépasser le consommateur (overwrite).

---

**Q55. Quelles sont les différentes `WaitStrategy` du Disruptor et quand les choisir ?**

- **`BusySpinWaitStrategy`** : boucle vide active (100% CPU). Latence ultra-faible. Pour le trading HFT sur des cœurs dédiés.
- **`YieldingWaitStrategy`** : `Thread.yield()` entre les tentatives. CPU élevé mais moins que busy spin. Pour la latence faible sans cœur dédié.
- **`SleepingWaitStrategy`** : yield puis sleep. Faible CPU. Pour le throughput avec latence acceptable.
- **`BlockingWaitStrategy`** : verrou classique. CPU minimal. Pour les systèmes non temps-réel.

---

**Q56. Expliquez le problème du `false sharing` dans le Disruptor et comment il est résolu.**

Dans le Disruptor, si les séquences du producteur et du consommateur sont proches en mémoire, elles partagent la même cache line. Quand le producteur met à jour sa séquence, il invalide la cache line du consommateur → **false sharing** → latence.

Solution : **padding** de 56 bytes autour de chaque séquence pour occuper une cache line complète de 64 bytes à elle seule :

```java
class PaddedSequence {
    long p1, p2, p3, p4, p5, p6, p7;   // 56 bytes padding
    volatile long value = -1;           // 8 bytes
    long p8, p9, p10, p11, p12, p13;   // 48 bytes padding
}
```

---

## 11. Java Memory Model & Happens-Before

---

**Q57. Qu'est-ce que le Java Memory Model (JMM) ?**

Le **JMM** définit les règles de visibilité des données entre threads. Chaque thread peut avoir une **copie locale** (en cache CPU) des variables. Le JMM précise dans quelles conditions une écriture faite par un thread devient **visible** à un autre.

Sans ces règles, le compilateur et le CPU peuvent **réordonner** les instructions pour optimiser, causant des comportements inattendus en multi-thread.

---

**Q58. Qu'est-ce que la relation happens-before ?**

**Happens-before** est une relation d'ordre partiel définie par le JMM. Si l'action A happens-before l'action B, alors les effets de A sont **garantis visibles** à B.

Les règles clés :
- Un `unlock` happens-before le prochain `lock` du même moniteur
- Une écriture `volatile` happens-before toute lecture suivante de la même variable
- `Thread.start()` happens-before toute action dans le thread démarré
- Toute action dans un thread happens-before `Thread.join()` qui retourne

---

**Q59. Un champ `volatile` garantit-il l'atomicité ?**

Non. `volatile` garantit uniquement la **visibilité**. Pour les types `long` et `double`, il garantit aussi l'**atomicité de la lecture/écriture** (sinon une JVM 32-bit peut couper en deux opérations 32-bit). Mais des opérations composées comme `volatile i++` restent non-atomiques.

Pour l'atomicité des opérations composées → utiliser `AtomicInteger`, `AtomicLong`, etc.

---

**Q60. Qu'est-ce qu'un memory barrier (fence) et comment `volatile` en crée-t-il ?**

Un **memory barrier** est une instruction CPU qui empêche le réordonnancement des instructions au-delà de ce point. Il force le vidage des buffers d'écriture et le rechargement depuis la mémoire principale.

Une écriture sur un champ `volatile` insère un **StoreStore barrier** avant et un **StoreLoad barrier** après.
Une lecture d'un champ `volatile` insère un **LoadLoad barrier** avant et un **LoadStore barrier** après.

Ces barriers garantissent la cohérence mais ont un coût en CPU. D'où l'intérêt des sémantiques Acquire/Release de `VarHandle` qui insèrent des barriers plus légers.

---

**Q61. Pourquoi est-il dangereux de publier un objet sans synchronisation ?**

À cause de la **publication partielle** (unsafe publication). Si un thread crée un objet et publie sa référence sans synchronisation, un autre thread peut voir la référence **avant** que les champs de l'objet soient initialisés (à cause du réordonnancement CPU/compilateur).

```java
// DANGEREUX
Order order = new Order();  // Le compilateur peut réordonner
sharedRef = order;          // Autre thread voit sharedRef non null
                             // mais order.price peut être encore 0 !

// SÉCURISÉ
volatile Order sharedRef;  // ou AtomicReference, ou synchronized
```

---

**Q62. Qu'est-ce que le `double-checked locking` et est-il safe en Java ?**

Le **double-checked locking** est un pattern de lazy initialization :

```java
private volatile Singleton instance;

public Singleton getInstance() {
    if (instance == null) {          // 1er check sans lock
        synchronized (this) {
            if (instance == null) {  // 2ème check avec lock
                instance = new Singleton();
            }
        }
    }
    return instance;
}
```

Avec **`volatile`** sur le champ, il est **safe depuis Java 5**. Sans `volatile`, il est dangereux car un thread peut voir une instance partiellement initialisée.

Alternativement, utiliser **Initialization-on-demand holder** :

```java
private static class Holder {
    static final Singleton INSTANCE = new Singleton();
}
public static Singleton getInstance() { return Holder.INSTANCE; }
```

---

## 12. Low Latency — Bonnes pratiques

---

**Q63. Quels sont les principaux ennemis de la low latency en Java ?**

1. **GC pauses** : collections GC qui suspendent les threads (stop-the-world).
2. **Lock contention** : threads bloqués en attente de verrous.
3. **Context switching** : le système d'exploitation interrompt un thread pour en exécuter un autre.
4. **Cache misses** : accès à la mémoire principale (100-300 cycles) vs cache L1 (4 cycles).
5. **False sharing** : invalidation de cache lines partagées.
6. **JIT compilation** : warm-up nécessaire avant que le JIT optimise le hot path.
7. **System calls** : I/O, allocations de mémoire système.

---

**Q64. Qu'est-ce que le CPU pinning et pourquoi est-il utile en low latency ?**

Le **CPU pinning** (ou thread affinity) consiste à dédier un **cœur CPU physique** à un thread spécifique. Le thread ne sera jamais migré sur un autre cœur.

Avantages :
- **Pas de cache warm-up** après migration : le thread garde ses données en L1/L2.
- **Pas de context switching** si le cœur est dédié.
- **Prévisibilité** des latences (moins de jitter).

En Java, via la librairie **Java Thread Affinity** de Peter Lawrey.

---

**Q65. Pourquoi préférer `BusySpinWaitStrategy` en HFT malgré son coût CPU ?**

En **High Frequency Trading**, chaque microseconde compte. `BusySpinWaitStrategy` boucle activement en vérifiant si de nouveaux événements sont disponibles, sans jamais `sleep` ou `yield`. Cela élimine la **latence de réveil** d'un thread endormi (qui peut prendre plusieurs microsecondes).

On accepte de "brûler" 100% d'un cœur CPU en échange d'une **latence minimale et prévisible** (quelques centaines de nanosecondes).

---

**Q66. Qu'est-ce que le JIT warm-up et comment s'y préparer ?**

Au démarrage, la JVM **interprète** le bytecode. Le JIT compiler surveille le hot path et le compile en code natif après un seuil d'invocations (JIT compilation). Pendant cette phase, les performances sont dégradées.

Stratégie :
1. Exécuter le hot path des milliers de fois au démarrage avec des données fictives.
2. Forcer la compilation avec `-XX:CompileThreshold=1000`.
3. Utiliser `-XX:+PrintCompilation` pour vérifier que les méthodes critiques sont compilées.
4. Éviter la **déoptimisation** (chargement de nouvelles classes, casts inattendus).

---

**Q67. Qu'est-ce que l'object pooling et pourquoi est-il important en low latency ?**

L'**object pooling** consiste à pré-allouer un ensemble d'objets au démarrage et à les réutiliser plutôt que de les créer/détruire. Cela élimine :
- Le coût d'**allocation** en hot path
- La **GC pressure** (moins d'objets à collecter)

```java
// Pattern pool simple
Queue<Order> pool = new ArrayDeque<>(1000);
// Pré-allouer
for (int i = 0; i < 1000; i++) pool.offer(new Order());

// En hot path
Order o = pool.poll();  // emprunter
o.reset();              // réinitialiser
process(o);
pool.offer(o);          // retourner
```

---

**Q68. Quelle taille doit avoir un Ring Buffer et pourquoi ?**

Toujours une **puissance de 2** (512, 1024, 2048, 4096...).

La raison : le calcul d'index `séquence % taille` (modulo) est une division, coûteuse en CPU. Si la taille est une puissance de 2, on utilise un **masque binaire** à la place :

```
séquence % 1024        → division (lent)
séquence & (1024 - 1)  → AND binaire (ultra-rapide)
```

La différence de performance peut atteindre **10x** sur les CPUs modernes.

---

## 13. Questions Avancées & Scénarios

---

**Q69. Scénario : Vous avez un système avec 1 producteur et 100 consommateurs. Quelle structure choisir ?**

Pour **1 producteur → N consommateurs** en multicast (tous reçoivent chaque message) :
- **Disruptor** avec `handleEventsWith()` est idéal : les consommateurs partagent le même ring buffer et avancent chacun leur séquence indépendamment.

Pour **1 producteur → N consommateurs** en répartition (chaque message va à UN consommateur) :
- `ConcurrentLinkedQueue` si non-bloquant.
- `LinkedTransferQueue` si on veut garantir le handoff.
- `ArrayBlockingQueue` si on veut la backpressure (bloquer le producteur si trop d'avance).

---

**Q70. Scénario : Vous avez un compteur de requêtes incrémenté par 1000 threads. `AtomicLong` ou `LongAdder` ?**

**`LongAdder`** sans hésiter. Avec 1000 threads en contention sur un `AtomicLong`, les CAS s'échoueront en cascade et causeront des spin loops coûteux.

`LongAdder` distribue les incréments sur un tableau de cellules → contention quasi-nulle. La lecture avec `sum()` est légèrement approximative mais acceptable pour un compteur de requêtes.

---

**Q71. Scénario : Vous devez partager une configuration immuable entre threads, mise à jour rare. Quelle approche ?**

```java
// Pattern : AtomicReference + objet immuable
private final AtomicReference<Config> configRef = new AtomicReference<>(Config.DEFAULT);

// Thread de config (rare)
public void updateConfig(Config newConfig) {
    configRef.set(newConfig);  // écriture volatile, visible immédiatement
}

// Threads métier (fréquent, lock-free)
public void processRequest() {
    Config cfg = configRef.get();  // lecture volatile
    // utiliser cfg...
}
```

L'objet `Config` est **immuable** (tous les champs `final`). `AtomicReference` garantit la visibilité atomique de la nouvelle référence. Pas de verrou, pas de copie.

---

**Q72. Scénario : Comment implémenter un cache thread-safe avec expiration ?**

```java
ConcurrentHashMap<String, CacheEntry> cache = new ConcurrentHashMap<>();

public String get(String key) {
    CacheEntry entry = cache.computeIfAbsent(key, k -> load(k));
    if (entry.isExpired()) {
        cache.compute(key, (k, current) -> {
            // Double-check dans le compute (atomique)
            if (current != null && current.isExpired()) {
                return load(k);  // remplacer
            }
            return current;  // un autre thread a déjà mis à jour
        });
        return cache.get(key).value;
    }
    return entry.value;
}
```

Pour une expiration en arrière-plan, combiner avec un `ScheduledExecutorService` qui appelle `cache.entrySet().removeIf(e -> e.getValue().isExpired())`.

---

**Q73. Quelle est la différence entre `synchronized` et `ReentrantLock` ?**

| | `synchronized` | `ReentrantLock` |
|---|---|---|
| Try-lock | ✗ | `tryLock()` ✓ |
| Timeout | ✗ | `tryLock(timeout)` ✓ |
| Interruptible | ✗ | `lockInterruptibly()` ✓ |
| Multiple conditions | ✗ | `newCondition()` ✓ |
| Fair | ✗ | `new ReentrantLock(true)` ✓ |
| Performance (JDK moderne) | Similaire | Similaire |
| Lisibilité | Plus simple | Verbeux (unlock en finally) |

En pratique, `synchronized` est préféré pour sa simplicité. `ReentrantLock` est choisi quand on a besoin de tryLock, timeout ou multiple conditions.

---

**Q74. Qu'est-ce que `StampedLock` (Java 8+) et quand est-il plus adapté que `ReentrantReadWriteLock` ?**

`StampedLock` est un verrou haute performance qui supporte trois modes :
- **Write lock** : exclusif classique.
- **Read lock** : partagé classique (comme `ReadWriteLock`).
- **Optimistic read** : tente une lecture sans verrou, puis valide que personne n'a écrit.

L'**optimistic read** est sa force : si les lectures sont très fréquentes et les écritures rares, on peut lire sans poser de verrou du tout (cas le plus fréquent), ce qui est bien plus rapide qu'un `ReadLock`.

```java
StampedLock lock = new StampedLock();

long stamp = lock.tryOptimisticRead();  // pas de verrou
double x = this.x;
double y = this.y;
if (!lock.validate(stamp)) {           // a-t-on été interrompus ?
    stamp = lock.readLock();           // prendre un vrai read lock
    try { x = this.x; y = this.y; }
    finally { lock.unlockRead(stamp); }
}
```

---

**Q75. Comment mesurer la latence réelle d'une structure de données en Java ?**

Avec **JMH (Java Microbenchmark Harness)** :

```java
@BenchmarkMode(Mode.AverageTime)
@OutputTimeUnit(TimeUnit.NANOSECONDS)
@Warmup(iterations = 5, time = 1)
@Measurement(iterations = 10, time = 1)
@Fork(2)
public class QueueBenchmark {

    @Benchmark
    public void concurrentLinkedQueue(Blackhole bh) {
        queue.offer("event");
        bh.consume(queue.poll());
    }

    @Benchmark
    public void arrayBlockingQueue(Blackhole bh) throws Exception {
        abq.offer("event");
        bh.consume(abq.poll());
    }
}
```

Ne **jamais** utiliser `System.currentTimeMillis()` pour du microbenchmarking (résolution insuffisante). Utiliser `System.nanoTime()` au minimum, ou mieux, JMH qui gère le warm-up, la JIT compilation et l'élimination du dead code (via `Blackhole`).

---

**Q76. Qu'est-ce que `JCTools` et en quoi est-ce mieux que les collections standard pour le low latency ?**

**JCTools** (Java Concurrent Tools) est une librairie open-source de collections concurrentes optimisées pour la JVM, allant plus loin que `java.util.concurrent` :

- **MpscArrayQueue** : Multi-Producer Single-Consumer, lock-free, bounded.
- **SpscArrayQueue** : Single-Producer Single-Consumer, wait-free, le plus rapide possible.
- **MpmcArrayQueue** : Multi-Producer Multi-Consumer, lock-free.

Ces structures évitent du padding, utilisent le CAS de manière optimale, et sont conçues pour minimiser la GC pressure et le false sharing. Elles sont utilisées en production dans des systèmes HFT.

---

**Q77. Quand est-il acceptable d'utiliser `synchronized` en low latency ?**

Quand :
1. Le bloc `synchronized` couvre une opération **très courte** (quelques nanosecondes).
2. La **contention est faible** (les threads se chevauchent rarement sur ce verrou).
3. La **JVM peut biaiser** le verrou (`biased locking`) : si un seul thread acquiert toujours ce verrou, la JVM l'optimise pour un coût quasi-nul.

En dehors du hot path critique (ex : initialisation, configuration), `synchronized` est tout à fait acceptable et préférable à une sur-ingénierie lock-free.

---

**Q78. Qu'est-ce que le `biased locking` et est-il toujours actif en Java 21 ?**

Le **biased locking** était une optimisation JVM : si un verrou est acquis toujours par le même thread, la JVM "biaise" le verrou vers ce thread. Les acquisitions suivantes ne coûtent quasiment rien (pas de CAS).

En **Java 15**, il a été marqué **deprecated**, et en **Java 21** (avec Project Loom et les optimisations de `synchronized`), il a été **supprimé**. La JVM moderne optimise `synchronized` différemment, et les performances sont au moins équivalentes sans biased locking.

---

**Q79. Comment choisir entre `ConcurrentHashMap` et une solution basée sur `ReadWriteLock` ?**

`ConcurrentHashMap` est presque toujours préférable pour les maps :
- Pas de code de synchronisation à maintenir
- Granularité fine (bin-level locking)
- Lectures totalement lock-free

`ReadWriteLock` peut être justifié pour des **structures custom** où plusieurs opérations doivent être atomiques ensemble (ex : lecture + modification + écriture d'une structure composite). Dans ce cas :
- `StampedLock` > `ReentrantReadWriteLock` > `synchronized` en termes de performance à forte charge lecture.

---

**Q80. Citez les principales différences entre les Virtual Threads (Project Loom, Java 21) et les Platform Threads pour la low latency.**

| | Platform Threads | Virtual Threads |
|---|---|---|
| Coût de création | Élevé (~1MB stack) | Très faible (~1KB) |
| Nombre pratique | Milliers | **Millions** |
| Blocage I/O | Bloque le thread OS | **Yield automatique** |
| CPU-bound | Optimal | Pas d'avantage |
| Latence I/O | Réduite par thread pools | **Mieux** |
| Latence pure (calcul) | Optimal | Pas d'avantage direct |

Pour la **low latency CPU-bound** (trading HFT), les Virtual Threads n'apportent pas d'avantage direct. Ils brillent pour les workloads **I/O-bound** à haute concurrence (services REST, DB queries), où on peut avoir des millions de requêtes simultanées sans les coûts des thread pools classiques.

---

*Fin des questions-réponses. Ces 80 questions couvrent l'ensemble du chapitre sur les Concurrent Collections et le Thread-Safe programming en Java pour le low latency.*
