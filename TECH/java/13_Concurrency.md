## [**..**](./00_index.md)

# Chapter 13: Concurrency

## 1. Introducing Threads

**Thread** = unité d'exécution la plus petite dans un processus. Permet d'exécuter plusieurs tâches **simultanément**.

### Thread Basics

```java
// Processus vs Thread
// Processus = programme en exécution (JVM)
// Thread = sous-unité d'exécution dans un processus

// Types de threads
// 1. User thread - thread normal
// 2. Daemon thread - thread en arrière-plan (meurt avec le programme)

// Main thread
public static void main(String[] args) {
    // main() s'exécute dans le "main thread"
    System.out.println("Main thread: " + Thread.currentThread().getName());
}
```

### Créer un Thread (Méthode 1: Runnable)

```java
// Runnable interface
@FunctionalInterface
public interface Runnable {
    void run();
}

// Exemple 1: Classe qui implémente Runnable
class MyTask implements Runnable {
    @Override
    public void run() {
        System.out.println("Task running in: " + Thread.currentThread().getName());
    }
}

Thread thread = new Thread(new MyTask());
thread.start();  // Démarre le thread

// Exemple 2: Anonymous class
Thread thread2 = new Thread(new Runnable() {
    @Override
    public void run() {
        System.out.println("Anonymous task");
    }
});
thread2.start();

// Exemple 3: Lambda (Java 8+)
Thread thread3 = new Thread(() -> {
    System.out.println("Lambda task");
});
thread3.start();

// Exemple 4: Method reference
Thread thread4 = new Thread(System.out::println);
thread4.start();
```

### Créer un Thread (Méthode 2: extends Thread)

```java
// Extends Thread (moins recommandé)
class MyThread extends Thread {
    @Override
    public void run() {
        System.out.println("MyThread running");
    }
}

MyThread thread = new MyThread();
thread.start();

// ATTENTION: start() vs run()
thread.start();  // Crée un NOUVEAU thread et appelle run()
thread.run();    // Appelle run() dans le thread ACTUEL (pas de parallélisme!)

// Exemple
Thread t = new Thread(() -> System.out.println("Thread: " + Thread.currentThread().getName()));
t.start();  // Output: Thread: Thread-0 (nouveau thread)
t.run();    // Output: Thread: main (thread actuel)
```

### Thread Properties

```java
Thread thread = new Thread(() -> {
    System.out.println("Task");
});

// Nom
thread.setName("MyWorker");
String name = thread.getName();

// Priorité (1-10, 5 par défaut)
thread.setPriority(Thread.MAX_PRIORITY);  // 10
thread.setPriority(Thread.MIN_PRIORITY);  // 1
thread.setPriority(Thread.NORM_PRIORITY); // 5

// Daemon
thread.setDaemon(true);  // Daemon thread
boolean isDaemon = thread.isDaemon();

// État
Thread.State state = thread.getState();
// NEW, RUNNABLE, BLOCKED, WAITING, TIMED_WAITING, TERMINATED

// Thread actuel
Thread current = Thread.currentThread();
System.out.println("Current thread: " + current.getName());

// ID
long id = thread.getId();
```

### Thread Lifecycle

```java
// États d'un thread
// NEW → RUNNABLE → RUNNING → TERMINATED
//          ↓           ↓
//       BLOCKED    WAITING/TIMED_WAITING

Thread thread = new Thread(() -> {
    try {
        Thread.sleep(1000);
    } catch (InterruptedException e) {
        e.printStackTrace();
    }
});

System.out.println(thread.getState());  // NEW

thread.start();
System.out.println(thread.getState());  // RUNNABLE

Thread.sleep(500);
System.out.println(thread.getState());  // TIMED_WAITING

Thread.sleep(600);
System.out.println(thread.getState());  // TERMINATED
```

### Polling with Sleep

```java
// Sleep - pause le thread actuel
public class PollingExample {
    private static int counter = 0;
    
    public static void main(String[] args) throws InterruptedException {
        // Thread qui incrémente
        new Thread(() -> {
            for (int i = 0; i < 5; i++) {
                counter++;
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
        
        // Main thread qui poll
        while (counter < 5) {
            System.out.println("Waiting... counter = " + counter);
            Thread.sleep(250);  // Attendre 250ms avant de re-vérifier
        }
        System.out.println("Done! counter = " + counter);
    }
}
```

---

## 2. Creating Threads with the Concurrency API

### ExecutorService

```java
import java.util.concurrent.*;

// ExecutorService = thread pool manager
// Avantages:
// - Réutilisation de threads
// - Gestion automatique
// - Plus efficace que new Thread()

// Créer ExecutorService
// 1. Single thread
ExecutorService single = Executors.newSingleThreadExecutor();

// 2. Fixed thread pool
ExecutorService fixed = Executors.newFixedThreadPool(5);  // 5 threads

// 3. Cached thread pool (illimité)
ExecutorService cached = Executors.newCachedThreadPool();

// 4. Scheduled thread pool
ScheduledExecutorService scheduled = Executors.newScheduledThreadPool(3);
```

### Soumettre des Tâches

```java
ExecutorService executor = Executors.newSingleThreadExecutor();

// execute() - fire and forget (void)
executor.execute(() -> {
    System.out.println("Task executed");
});

// submit() - retourne Future
Future<?> future = executor.submit(() -> {
    System.out.println("Task submitted");
});

// submit() avec Callable (retourne résultat)
Future<Integer> result = executor.submit(() -> {
    return 42;
});

// Récupérer le résultat
try {
    Integer value = result.get();  // Bloquant
    System.out.println("Result: " + value);  // 42
} catch (InterruptedException | ExecutionException e) {
    e.printStackTrace();
}

// IMPORTANT: Shutdown
executor.shutdown();  // Arrête après tâches en cours
// executor.shutdownNow();  // Arrête immédiatement
```

### Waiting for Results

```java
ExecutorService executor = Executors.newSingleThreadExecutor();

// Callable - peut retourner valeur et throw exception
Callable<String> task = () -> {
    Thread.sleep(1000);
    return "Task completed";
};

Future<String> future = executor.submit(task);

// Vérifier si terminé
if (future.isDone()) {
    System.out.println("Task done");
}

// Attendre avec timeout
try {
    String result = future.get(2, TimeUnit.SECONDS);  // Max 2 secondes
    System.out.println(result);
} catch (TimeoutException e) {
    System.out.println("Timeout!");
    future.cancel(true);  // Annuler la tâche
}

executor.shutdown();
```

### Scheduling Tasks

```java
ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(2);

// schedule() - exécute une fois après délai
scheduler.schedule(() -> {
    System.out.println("Task après 3 secondes");
}, 3, TimeUnit.SECONDS);

// scheduleAtFixedRate() - répète à intervalle fixe
scheduler.scheduleAtFixedRate(() -> {
    System.out.println("Task toutes les 2 secondes");
}, 0, 2, TimeUnit.SECONDS);  // délai initial 0, période 2s

// scheduleWithFixedDelay() - délai entre fin et début
scheduler.scheduleWithFixedDelay(() -> {
    System.out.println("Task avec délai fixe");
    Thread.sleep(1000);  // Prend 1s
}, 0, 2, TimeUnit.SECONDS);  // 2s APRÈS la fin de la tâche

// Shutdown
scheduler.shutdown();
```

### invokeAll et invokeAny

```java
ExecutorService executor = Executors.newFixedThreadPool(3);

List<Callable<String>> tasks = Arrays.asList(
    () -> { Thread.sleep(1000); return "Task 1"; },
    () -> { Thread.sleep(500); return "Task 2"; },
    () -> { Thread.sleep(2000); return "Task 3"; }
);

// invokeAll() - exécute toutes, attend toutes
List<Future<String>> results = executor.invokeAll(tasks);
for (Future<String> future : results) {
    System.out.println(future.get());  // Task 1, Task 2, Task 3
}

// invokeAny() - retourne le premier qui termine
String first = executor.invokeAny(tasks);
System.out.println(first);  // "Task 2" (le plus rapide)

executor.shutdown();
```

---

## 3. Writing Thread-Safe Code

### Race Condition

```java
// Problème: Race condition
public class Counter {
    private int count = 0;
    
    public void increment() {
        count++;  // NON thread-safe!
        // Décomposé en:
        // 1. Lire count
        // 2. Incrémenter
        // 3. Écrire count
    }
    
    public int getCount() {
        return count;
    }
}

// Utilisation concurrent
Counter counter = new Counter();
ExecutorService executor = Executors.newFixedThreadPool(10);

for (int i = 0; i < 100; i++) {
    executor.execute(() -> counter.increment());
}

executor.shutdown();
executor.awaitTermination(1, TimeUnit.SECONDS);

System.out.println(counter.getCount());  // Pas toujours 100!
```

### Synchronized Keyword

```java
// Solution 1: synchronized method
public class SafeCounter {
    private int count = 0;
    
    public synchronized void increment() {
        count++;  // Thread-safe
    }
    
    public synchronized int getCount() {
        return count;
    }
}

// Solution 2: synchronized block
public class SafeCounter2 {
    private int count = 0;
    private final Object lock = new Object();
    
    public void increment() {
        synchronized(lock) {
            count++;
        }
    }
    
    public int getCount() {
        synchronized(lock) {
            return count;
        }
    }
}

// Solution 3: synchronized sur this
public class SafeCounter3 {
    private int count = 0;
    
    public void increment() {
        synchronized(this) {
            count++;
        }
    }
}
```

### Lock Interface

```java
import java.util.concurrent.locks.*;

// Lock - plus flexible que synchronized
public class SafeCounterWithLock {
    private int count = 0;
    private final Lock lock = new ReentrantLock();
    
    public void increment() {
        lock.lock();
        try {
            count++;
        } finally {
            lock.unlock();  // TOUJOURS dans finally
        }
    }
    
    // tryLock() - non bloquant
    public boolean tryIncrement() {
        if (lock.tryLock()) {
            try {
                count++;
                return true;
            } finally {
                lock.unlock();
            }
        }
        return false;  // Lock pas disponible
    }
    
    // tryLock avec timeout
    public boolean tryIncrementWithTimeout() throws InterruptedException {
        if (lock.tryLock(1, TimeUnit.SECONDS)) {
            try {
                count++;
                return true;
            } finally {
                lock.unlock();
            }
        }
        return false;
    }
}
```

### Atomic Classes

```java
import java.util.concurrent.atomic.*;

// Atomic classes - thread-safe sans lock
public class AtomicCounter {
    private AtomicInteger count = new AtomicInteger(0);
    
    public void increment() {
        count.incrementAndGet();  // Thread-safe, atomic
    }
    
    public int getCount() {
        return count.get();
    }
}

// Atomic classes disponibles
AtomicInteger atomicInt = new AtomicInteger(0);
AtomicLong atomicLong = new AtomicLong(0L);
AtomicBoolean atomicBool = new AtomicBoolean(false);
AtomicReference<String> atomicRef = new AtomicReference<>("initial");

// Opérations atomiques
atomicInt.incrementAndGet();  // ++i
atomicInt.getAndIncrement();  // i++
atomicInt.decrementAndGet();  // --i
atomicInt.getAndDecrement();  // i--
atomicInt.addAndGet(5);       // i += 5
atomicInt.getAndAdd(5);       // temp = i; i += 5; return temp

// compareAndSet (CAS)
boolean success = atomicInt.compareAndSet(10, 20);
// Si valeur actuelle == 10, mettre à 20 et retourner true
// Sinon, retourner false (pas de changement)
```

### Volatile Keyword

```java
// volatile - garantit visibilité entre threads
public class VolatileExample {
    private volatile boolean flag = false;
    
    public void setFlag() {
        flag = true;  // Visible immédiatement aux autres threads
    }
    
    public void waitForFlag() {
        while (!flag) {
            // Attendre
        }
        System.out.println("Flag is true!");
    }
}

// Sans volatile:
// - Thread peut cacher la valeur
// - Boucle infinie possible

// Avec volatile:
// - Lecture/écriture directement en mémoire
// - Pas de cache
// - Visibilité garantie

// ATTENTION: volatile ne garantit PAS l'atomicité
private volatile int count = 0;

public void increment() {
    count++;  // PAS atomic, même avec volatile
    // Utiliser AtomicInteger à la place
}
```

---

## 4. Using Concurrent Collections

### ConcurrentHashMap

```java
import java.util.concurrent.*;

// ConcurrentHashMap - thread-safe HashMap
Map<String, Integer> map = new ConcurrentHashMap<>();

// Opérations thread-safe
map.put("key1", 1);
map.get("key1");
map.remove("key1");

// putIfAbsent - atomic
Integer prev = map.putIfAbsent("key2", 2);
// Si key2 n'existe pas, l'ajoute et retourne null
// Si key2 existe, ne fait rien et retourne la valeur existante

// Méthodes atomiques
map.compute("key3", (k, v) -> (v == null) ? 1 : v + 1);  // Incrémente ou met à 1
map.merge("key4", 1, Integer::sum);  // Merge avec fonction

// ATTENTION: Ces opérations NE sont PAS atomiques ensemble
if (!map.containsKey("key5")) {  // Thread 1 vérifie
    // Thread 2 peut ajouter ici
    map.put("key5", 5);  // Thread 1 ajoute
}

// Utiliser putIfAbsent à la place (atomic)
map.putIfAbsent("key5", 5);
```

### CopyOnWriteArrayList

```java
import java.util.concurrent.*;

// CopyOnWriteArrayList - thread-safe pour lecture intensive
List<String> list = new CopyOnWriteArrayList<>();

// Ajout - copie toute la liste
list.add("item1");  // Lent (copie)

// Lecture - pas de lock
String item = list.get(0);  // Rapide

// Cas d'usage:
// - Beaucoup de lectures, peu d'écritures
// - Listeners, observers
// - Configuration

// Itération - snapshot
for (String s : list) {
    // Itère sur snapshot (pas affecté par modifications concurrent)
}

// ATTENTION: Modifications pendant itération pas visibles
list.add("item2");
Iterator<String> it = list.iterator();
list.add("item3");  // Pas vu par iterator
while (it.hasNext()) {
    System.out.println(it.next());  // item2 pas affiché
}
```

### BlockingQueue

```java
import java.util.concurrent.*;

// BlockingQueue - queue thread-safe avec blocage
BlockingQueue<String> queue = new LinkedBlockingQueue<>(10);  // Capacité 10

// Producer
new Thread(() -> {
    try {
        queue.put("item1");  // Bloque si queue pleine
        queue.offer("item2", 1, TimeUnit.SECONDS);  // Timeout si pleine
    } catch (InterruptedException e) {
        e.printStackTrace();
    }
}).start();

// Consumer
new Thread(() -> {
    try {
        String item = queue.take();  // Bloque si queue vide
        String item2 = queue.poll(1, TimeUnit.SECONDS);  // Timeout si vide
    } catch (InterruptedException e) {
        e.printStackTrace();
    }
}).start();

// Autres implémentations
BlockingQueue<String> arrayQueue = new ArrayBlockingQueue<>(10);  // Array-based
BlockingQueue<String> linkedQueue = new LinkedBlockingQueue<>();  // Linked-based (illimité)
BlockingQueue<String> priorityQueue = new PriorityBlockingQueue<>();  // Avec priorité
```

### ConcurrentSkipListMap et ConcurrentSkipListSet

```java
// ConcurrentSkipListMap - sorted, thread-safe
ConcurrentSkipListMap<String, Integer> map = new ConcurrentSkipListMap<>();
map.put("z", 3);
map.put("a", 1);
map.put("m", 2);

map.forEach((k, v) -> System.out.println(k + "=" + v));
// a=1, m=2, z=3 (ordre alphabétique)

// ConcurrentSkipListSet - sorted set, thread-safe
ConcurrentSkipListSet<Integer> set = new ConcurrentSkipListSet<>();
set.add(30);
set.add(10);
set.add(20);

set.forEach(System.out::println);  // 10, 20, 30 (trié)
```

### Synchronized Collections (Legacy)

```java
// Collections.synchronizedXxx - wrapper thread-safe
List<String> list = Collections.synchronizedList(new ArrayList<>());
Set<String> set = Collections.synchronizedSet(new HashSet<>());
Map<String, Integer> map = Collections.synchronizedMap(new HashMap<>());

// ATTENTION: Itération nécessite synchronisation manuelle
synchronized(list) {
    for (String item : list) {
        System.out.println(item);
    }
}

// Préférer CopyOnWriteArrayList, ConcurrentHashMap, etc.
```

---

## 5. Identifying Threading Problems

### Deadlock

```java
// Deadlock - 2+ threads bloqués en attendant l'autre
public class DeadlockExample {
    private final Object lock1 = new Object();
    private final Object lock2 = new Object();
    
    public void method1() {
        synchronized(lock1) {
            System.out.println("Method1: Holding lock1");
            try { Thread.sleep(10); } catch (InterruptedException e) {}
            
            synchronized(lock2) {  // Attend lock2
                System.out.println("Method1: Holding lock1 & lock2");
            }
        }
    }
    
    public void method2() {
        synchronized(lock2) {
            System.out.println("Method2: Holding lock2");
            try { Thread.sleep(10); } catch (InterruptedException e) {}
            
            synchronized(lock1) {  // Attend lock1
                System.out.println("Method2: Holding lock1 & lock2");
            }
        }
    }
    
    public static void main(String[] args) {
        DeadlockExample example = new DeadlockExample();
        
        new Thread(example::method1).start();
        new Thread(example::method2).start();
        
        // Thread1: lock1 → attend lock2
        // Thread2: lock2 → attend lock1
        // → DEADLOCK
    }
}

// Solution: Toujours acquérir les locks dans le MÊME ordre
public void method1() {
    synchronized(lock1) {
        synchronized(lock2) {
            // ...
        }
    }
}

public void method2() {
    synchronized(lock1) {  // Même ordre que method1
        synchronized(lock2) {
            // ...
        }
    }
}
```

### Starvation

```java
// Starvation - thread ne reçoit jamais de ressources
public class StarvationExample {
    public static void main(String[] args) {
        final Object lock = new Object();
        
        // Thread haute priorité (accapare la ressource)
        Thread highPriority = new Thread(() -> {
            while (true) {
                synchronized(lock) {
                    // Travail
                }
            }
        });
        highPriority.setPriority(Thread.MAX_PRIORITY);
        highPriority.start();
        
        // Thread basse priorité (affamé)
        Thread lowPriority = new Thread(() -> {
            synchronized(lock) {
                System.out.println("Never executed!");  // Jamais exécuté
            }
        });
        lowPriority.setPriority(Thread.MIN_PRIORITY);
        lowPriority.start();
    }
}

// Solution: Éviter priorités extrêmes, utiliser fair locks
Lock lock = new ReentrantLock(true);  // Fair = FIFO
```

### Livelock

```java
// Livelock - threads actifs mais bloqués (cèdent constamment)
public class LivelockExample {
    static class Person {
        private String name;
        private boolean isPolite = true;
        
        public Person(String name) {
            this.name = name;
        }
        
        public void passDoorway(Person other) {
            while (other.isPolite) {
                System.out.println(name + ": After you!");
                // Cède le passage
                try { Thread.sleep(10); } catch (InterruptedException e) {}
            }
        }
    }
    
    public static void main(String[] args) {
        Person person1 = new Person("Alice");
        Person person2 = new Person("Bob");
        
        new Thread(() -> person1.passDoorway(person2)).start();
        new Thread(() -> person2.passDoorway(person1)).start();
        
        // Les deux cèdent constamment → Livelock
    }
}

// Solution: Introduire aléatoire ou timeout
```

### Race Condition

```java
// Race condition - résultat dépend du timing
public class RaceConditionExample {
    private int counter = 0;
    
    public void increment() {
        int temp = counter;  // 1. Lire
        temp++;              // 2. Modifier
        counter = temp;      // 3. Écrire
        
        // Thread 1: lire 0
        // Thread 2: lire 0 (avant que Thread1 écrive)
        // Thread 1: écrire 1
        // Thread 2: écrire 1
        // Résultat: 1 au lieu de 2
    }
}

// Solution: synchronized, Lock, ou Atomic
public synchronized void increment() {
    counter++;
}
```

---

## 6. Working with Parallel Streams

### Parallel Streams Basics

```java
import java.util.stream.*;

List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

// Stream séquentiel
numbers.stream()
    .map(n -> n * 2)
    .forEach(System.out::println);

// Stream parallèle
numbers.parallelStream()
    .map(n -> n * 2)
    .forEach(System.out::println);  // Ordre non garanti

// Ou convertir
numbers.stream()
    .parallel()
    .map(n -> n * 2)
    .forEach(System.out::println);

// Vérifier si parallèle
boolean isParallel = numbers.parallelStream().isParallel();  // true
```

### Quand Utiliser Parallel Streams

```java
// BON usage: Grande collection, opérations CPU-intensive
List<Integer> numbers = IntStream.range(1, 1000000)
    .boxed()
    .collect(Collectors.toList());

long sum = numbers.parallelStream()
    .mapToLong(n -> n * n)  // Calcul intensif
    .sum();

// MAUVAIS usage: Petite collection
List<Integer> small = Arrays.asList(1, 2, 3);
small.parallelStream()  // Overhead > gain
    .forEach(System.out::println);

// MAUVAIS usage: Opérations I/O
files.parallelStream()
    .forEach(file -> {
        // Lecture fichier (I/O bound, pas CPU bound)
    });

// MAUVAIS usage: État partagé mutable
List<Integer> result = new ArrayList<>();  // PAS thread-safe
numbers.parallelStream()
    .forEach(result::add);  // DANGER: race condition
```

### Parallel Stream Performance

```java
// Comparaison performance
List<Integer> numbers = IntStream.range(1, 10000000)
    .boxed()
    .collect(Collectors.toList());

// Séquentiel
long start = System.currentTimeMillis();
long sum1 = numbers.stream()
    .mapToLong(n -> n * n)
    .sum();
long end = System.currentTimeMillis();
System.out.println("Sequential: " + (end - start) + "ms");

// Parallèle
start = System.currentTimeMillis();
long sum2 = numbers.parallelStream()
    .mapToLong(n -> n * n)
    .sum();
end = System.currentTimeMillis();
System.out.println("Parallel: " + (end - start) + "ms");

// Résultats typiques:
// Sequential: 150ms
// Parallel: 50ms
```

### Parallel Reduction

```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5);

// reduce() avec parallel - opération doit être associative
int sum = numbers.parallelStream()
    .reduce(0, Integer::sum);  // OK: + est associatif

// DANGER: opération non-associative
int wrong = numbers.parallelStream()
    .reduce(0, (a, b) -> a - b);  // FAUX: - n'est pas associatif
// Résultat imprévisible

// collect() thread-safe
List<Integer> result = numbers.parallelStream()
    .filter(n -> n % 2 == 0)
    .collect(Collectors.toList());  // Thread-safe
```

### forEachOrdered vs forEach

```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5);

// forEach - ordre non garanti en parallel
numbers.parallelStream()
    .forEach(System.out::println);
// Peut afficher: 3, 1, 5, 2, 4

// forEachOrdered - préserve l'ordre (plus lent)
numbers.parallelStream()
    .forEachOrdered(System.out::println);
// Affiche toujours: 1, 2, 3, 4, 5
```

### Parallel Stream Pitfalls

```java
// Piège 1: État partagé mutable
List<Integer> list = new ArrayList<>();  // PAS thread-safe
IntStream.range(1, 100)
    .parallel()
    .forEach(list::add);  // DANGER
System.out.println(list.size());  // Pas toujours 99!

// Solution: utiliser collect
List<Integer> safeList = IntStream.range(1, 100)
    .parallel()
    .boxed()
    .collect(Collectors.toList());

// Piège 2: Stateful operations
numbers.parallelStream()
    .map(n -> n + threadLocalVariable)  // DANGER
    .forEach(System.out::println);

// Piège 3: Side effects non thread-safe
int[] counter = {0};
numbers.parallelStream()
    .forEach(n -> counter[0]++);  // DANGER: race condition
System.out.println(counter[0]);  // Pas toujours correct

// Solution: utiliser count() ou collect()
long count = numbers.parallelStream().count();
```

---

## Points Clés

### Threads
- **Thread**: Créer avec `Runnable` ou extends `Thread`
- **start() vs run()**: `start()` crée nouveau thread, `run()` exécute dans thread actuel
- **ExecutorService**: Pool de threads réutilisables
- **Callable**: Comme Runnable mais retourne valeur

### Thread Safety
- **synchronized**: Lock sur méthode ou block
- **Lock**: Plus flexible que synchronized
- **Atomic classes**: Thread-safe sans lock (AtomicInteger, etc.)
- **volatile**: Garantit visibilité (pas atomicité)

### Concurrent Collections
- **ConcurrentHashMap**: Thread-safe HashMap
- **CopyOnWriteArrayList**: Thread-safe pour lecture intensive
- **BlockingQueue**: Queue avec blocage (put/take)

### Threading Problems
- **Deadlock**: Threads bloqués en attendant l'autre
- **Starvation**: Thread ne reçoit jamais de ressources
- **Livelock**: Threads actifs mais bloqués
- **Race condition**: Résultat dépend du timing

### Parallel Streams
- **parallelStream()**: Traitement parallèle
- **Utiliser si**: Grande collection + CPU-intensive
- **Éviter si**: Petite collection, I/O, état mutable
- **forEachOrdered**: Préserve l'ordre en parallel

---

# Questions/Réponses - Chapter 13: Concurrency

## Questions sur les Bases des Threads

**Q1: Quelle est la différence entre start() et run()?**
```java
Thread thread = new Thread(() -> {
    System.out.println("Thread: " + Thread.currentThread().getName());
});

thread.start();
thread.run();
```

**R:**
- **start()**: Crée un **nouveau thread** et appelle `run()` dedans
- **run()**: Exécute `run()` dans le **thread actuel** (pas de parallélisme)

```java
Thread t = new Thread(() -> System.out.println(Thread.currentThread().getName()));

t.start();  // Output: Thread-0 (nouveau thread)
t.run();    // Output: main (thread actuel)

// start() peut être appelé UNE SEULE FOIS
t.start();  // IllegalThreadStateException
```

---

**Q2: Ce code compile-t-il?**
```java
Thread thread = new Thread(() -> {
    System.out.println("Task");
});
thread.start();
thread.start();
```

**R:** Compile mais lance **IllegalThreadStateException** au runtime.

Un thread ne peut être démarré qu'**une seule fois**. Après `start()`, l'état est **RUNNABLE** ou supérieur, et on ne peut plus appeler `start()`.

---

**Q3: Quelle est la différence entre Runnable et Callable?**

**R:**

| **Runnable** | **Callable** |
|--------------|--------------|
| `void run()` | `V call() throws Exception` |
| Ne retourne rien | Retourne une valeur |
| Ne peut pas throw checked exception | Peut throw exception |
| Avec `Thread` ou `ExecutorService` | Seulement avec `ExecutorService` |

```java
// Runnable
Runnable task1 = () -> {
    System.out.println("No return value");
};

// Callable
Callable<Integer> task2 = () -> {
    return 42;  // Retourne valeur
};

// Utilisation
Thread t = new Thread(task1);  // OK
// Thread t2 = new Thread(task2);  // ERREUR: Thread n'accepte pas Callable

ExecutorService executor = Executors.newSingleThreadExecutor();
executor.execute(task1);  // OK
Future<Integer> future = executor.submit(task2);  // OK
Integer result = future.get();  // 42
```

---

**Q4: Que fait Thread.sleep()?**
```java
Thread.sleep(1000);
```

**R:** **Pause** le thread **actuel** pendant 1000 millisecondes (1 seconde).

```java
System.out.println("Start");
Thread.sleep(2000);  // Pause 2 secondes
System.out.println("End");

// ATTENTION: sleep() est statique, agit sur le thread ACTUEL
thread.sleep(1000);  // Pause le thread ACTUEL, pas "thread"!

// Lève InterruptedException (checked)
try {
    Thread.sleep(1000);
} catch (InterruptedException e) {
    e.printStackTrace();
}
```

---

**Q5: Ce code a-t-il un problème?**
```java
public static void main(String[] args) {
    Thread thread = new Thread(() -> {
        for (int i = 0; i < 5; i++) {
            System.out.println(i);
        }
    });
    thread.start();
    System.out.println("Main done");
}
```

**R:** Pas d'erreur, mais **ordre non déterministe**.

```
Possible output 1:
Main done
0
1
2
3
4

Possible output 2:
0
1
Main done
2
3
4

// Le main thread et le nouveau thread s'exécutent en parallèle
```

---

**Q6: Quelle est la différence entre user thread et daemon thread?**

**R:**

| **User Thread** | **Daemon Thread** |
|-----------------|-------------------|
| JVM attend sa fin | JVM ne l'attend pas |
| Application normale | Tâche d'arrière-plan |
| Ex: main thread | Ex: Garbage Collector |

```java
Thread userThread = new Thread(() -> {
    while (true) {
        // Boucle infinie
    }
});
userThread.start();
// JVM ne se termine JAMAIS (attend userThread)

Thread daemonThread = new Thread(() -> {
    while (true) {
        // Boucle infinie
    }
});
daemonThread.setDaemon(true);  // Daemon
daemonThread.start();
// JVM se termine quand main() finit (ignore daemonThread)

// ATTENTION: setDaemon() AVANT start()
Thread t = new Thread(() -> {});
t.start();
t.setDaemon(true);  // IllegalThreadStateException
```

---

## Questions sur ExecutorService

**Q7: Quelle est la différence entre ces ExecutorService?**
```java
// Version 1
ExecutorService single = Executors.newSingleThreadExecutor();

// Version 2
ExecutorService fixed = Executors.newFixedThreadPool(1);
```

**R:** **Presque identique** pour 1 thread, mais:

```java
// newSingleThreadExecutor() - 1 thread, non reconfigurable
ExecutorService single = Executors.newSingleThreadExecutor();
// Ne peut PAS changer le pool size

// newFixedThreadPool(1) - 1 thread, mais reconfigurable
ExecutorService fixed = Executors.newFixedThreadPool(1);
// Peut être casté en ThreadPoolExecutor et modifié
ThreadPoolExecutor pool = (ThreadPoolExecutor) fixed;
pool.setCorePoolSize(5);  // Possible

// newSingleThreadExecutor retourne un wrapper non modifiable
```

---

**Q8: Quelle est la différence entre execute() et submit()?**

**R:**

| **execute()** | **submit()** |
|---------------|--------------|
| `void execute(Runnable)` | `Future<?> submit(Runnable)` |
| Fire-and-forget | Retourne Future |
| Pas de résultat | Peut récupérer résultat/exception |

```java
ExecutorService executor = Executors.newSingleThreadExecutor();

// execute() - pas de retour
executor.execute(() -> {
    System.out.println("Task");
});

// submit() - retourne Future
Future<?> future = executor.submit(() -> {
    System.out.println("Task");
});

// Vérifier si terminé
if (future.isDone()) {
    System.out.println("Done");
}

// submit() avec Callable - récupérer résultat
Future<Integer> result = executor.submit(() -> 42);
System.out.println(result.get());  // 42
```

---

**Q9: Que se passe-t-il si on oublie shutdown()?**
```java
ExecutorService executor = Executors.newSingleThreadExecutor();
executor.submit(() -> System.out.println("Task"));
// Pas de shutdown()
```

**R:** La **JVM ne se termine pas**.

```java
// Sans shutdown()
public static void main(String[] args) {
    ExecutorService executor = Executors.newSingleThreadExecutor();
    executor.submit(() -> System.out.println("Task"));
    // Programme ne se termine jamais (thread pool actif)
}

// Avec shutdown()
public static void main(String[] args) {
    ExecutorService executor = Executors.newSingleThreadExecutor();
    executor.submit(() -> System.out.println("Task"));
    executor.shutdown();  // Programme se termine
}
```

---

**Q10: Quelle est la différence entre shutdown() et shutdownNow()?**

**R:**

**shutdown()**: 
- N'accepte plus de nouvelles tâches
- Termine les tâches en cours
- Retourne immédiatement (non bloquant)

**shutdownNow()**:
- N'accepte plus de nouvelles tâches
- Essaie d'**interrompre** les tâches en cours
- Retourne liste des tâches en attente

```java
ExecutorService executor = Executors.newSingleThreadExecutor();

executor.submit(() -> {
    try {
        Thread.sleep(5000);
        System.out.println("Task done");
    } catch (InterruptedException e) {
        System.out.println("Interrupted");
    }
});

// shutdown() - attend la fin
executor.shutdown();
// "Task done" affiché après 5 secondes

// shutdownNow() - interrompt
executor.shutdownNow();
// "Interrupted" affiché immédiatement

// Vérifier si terminé
boolean terminated = executor.isTerminated();

// Attendre la fin
executor.awaitTermination(10, TimeUnit.SECONDS);
```

---

**Q11: Que retourne Future.get()?**
```java
ExecutorService executor = Executors.newSingleThreadExecutor();

Future<Integer> future = executor.submit(() -> {
    Thread.sleep(2000);
    return 42;
});

Integer result = future.get();
System.out.println(result);
```

**R:** `get()` **bloque** jusqu'à ce que la tâche soit terminée, puis retourne le résultat.

```java
System.out.println("Start: " + System.currentTimeMillis());

Future<Integer> future = executor.submit(() -> {
    Thread.sleep(2000);
    return 42;
});

System.out.println("Submitted: " + System.currentTimeMillis());
Integer result = future.get();  // Bloque 2 secondes
System.out.println("Result: " + result);
System.out.println("End: " + System.currentTimeMillis());

// Output:
// Start: 1000
// Submitted: 1001
// Result: 42
// End: 3001  (2 secondes après)

// get() avec timeout
try {
    Integer result = future.get(1, TimeUnit.SECONDS);  // Max 1 seconde
} catch (TimeoutException e) {
    System.out.println("Timeout!");
    future.cancel(true);  // Annuler la tâche
}
```

---

**Q12: Quelle est la différence entre scheduleAtFixedRate() et scheduleWithFixedDelay()?**

**R:**

**scheduleAtFixedRate()**: Intervalle **entre DÉBUTS** de tâches
```java
ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);

scheduler.scheduleAtFixedRate(() -> {
    System.out.println("Start: " + System.currentTimeMillis());
    Thread.sleep(1000);  // Prend 1s
    System.out.println("End: " + System.currentTimeMillis());
}, 0, 2, TimeUnit.SECONDS);

// Output:
// Start: 0ms
// End: 1000ms
// Start: 2000ms    ← 2s après premier DÉBUT
// End: 3000ms
// Start: 4000ms    ← 2s après deuxième DÉBUT
```

**scheduleWithFixedDelay()**: Délai **entre FIN et DÉBUT**
```java
scheduler.scheduleWithFixedDelay(() -> {
    System.out.println("Start: " + System.currentTimeMillis());
    Thread.sleep(1000);  // Prend 1s
    System.out.println("End: " + System.currentTimeMillis());
}, 0, 2, TimeUnit.SECONDS);

// Output:
// Start: 0ms
// End: 1000ms
// Start: 3000ms    ← 2s après première FIN
// End: 4000ms
// Start: 6000ms    ← 2s après deuxième FIN
```

---

**Q13: Que fait invokeAll()?**
```java
ExecutorService executor = Executors.newFixedThreadPool(3);

List<Callable<Integer>> tasks = Arrays.asList(
    () -> { Thread.sleep(1000); return 1; },
    () -> { Thread.sleep(2000); return 2; },
    () -> { Thread.sleep(500); return 3; }
);

List<Future<Integer>> results = executor.invokeAll(tasks);
```

**R:** `invokeAll()` exécute **toutes** les tâches et attend qu'**elles soient toutes terminées**.

```java
long start = System.currentTimeMillis();

List<Future<Integer>> results = executor.invokeAll(tasks);
// Attend 2000ms (la plus longue)

for (Future<Integer> future : results) {
    System.out.println(future.get());  // Ne bloque pas (déjà terminé)
}

long end = System.currentTimeMillis();
System.out.println("Temps total: " + (end - start) + "ms");  // ~2000ms

// IMPORTANT: invokeAll() bloque jusqu'à ce que TOUTES soient terminées
```

---

**Q14: Quelle est la différence entre invokeAll() et invokeAny()?**

**R:**

**invokeAll()**: Exécute **toutes**, attend **toutes**, retourne **toutes**
```java
List<Future<Integer>> results = executor.invokeAll(tasks);
// Attend que toutes soient terminées
// Retourne List<Future<Integer>>
```

**invokeAny()**: Exécute **toutes**, attend **la première**, retourne **une seule**
```java
Integer first = executor.invokeAny(tasks);
// Attend la première qui termine
// Retourne Integer (pas Future)
// Annule les autres tâches
```

```java
List<Callable<Integer>> tasks = Arrays.asList(
    () -> { Thread.sleep(1000); return 1; },
    () -> { Thread.sleep(500); return 2; },   // La plus rapide
    () -> { Thread.sleep(2000); return 3; }
);

// invokeAny() - retourne 2 (le premier)
Integer result = executor.invokeAny(tasks);
System.out.println(result);  // 2
// Temps: ~500ms
```

---

## Questions sur Thread Safety

**Q15: Ce code est-il thread-safe?**
```java
public class Counter {
    private int count = 0;
    
    public void increment() {
        count++;
    }
    
    public int getCount() {
        return count;
    }
}
```

**R:** **NON**, pas thread-safe.

```java
// count++ se décompose en 3 opérations:
// 1. temp = count    (lire)
// 2. temp = temp + 1 (incrémenter)
// 3. count = temp    (écrire)

// Race condition:
// Thread 1: lire count = 0
// Thread 2: lire count = 0
// Thread 1: écrire count = 1
// Thread 2: écrire count = 1
// Résultat: count = 1 (au lieu de 2)

// Test
Counter counter = new Counter();
ExecutorService executor = Executors.newFixedThreadPool(10);

for (int i = 0; i < 100; i++) {
    executor.execute(counter::increment);
}

executor.shutdown();
executor.awaitTermination(1, TimeUnit.SECONDS);

System.out.println(counter.getCount());  // Pas toujours 100!
```

---

**Q16: Comment rendre ce code thread-safe?**
```java
public class Counter {
    private int count = 0;
    
    public void increment() {
        count++;
    }
}
```

**R:** Plusieurs solutions:

**Solution 1: synchronized method**
```java
public synchronized void increment() {
    count++;
}
```

**Solution 2: synchronized block**
```java
private final Object lock = new Object();

public void increment() {
    synchronized(lock) {
        count++;
    }
}
```

**Solution 3: Lock**
```java
private final Lock lock = new ReentrantLock();

public void increment() {
    lock.lock();
    try {
        count++;
    } finally {
        lock.unlock();
    }
}
```

**Solution 4: AtomicInteger**
```java
private AtomicInteger count = new AtomicInteger(0);

public void increment() {
    count.incrementAndGet();
}
```

---

**Q17: Quelle est la différence entre synchronized et Lock?**

**R:**

| **synchronized** | **Lock** |
|------------------|----------|
| Plus simple | Plus flexible |
| Acquire automatique | lock() explicite |
| Release automatique | unlock() explicite (finally) |
| Pas de tryLock | tryLock() disponible |
| Pas de timeout | tryLock(timeout) disponible |
| Pas interruptible | lockInterruptibly() disponible |

```java
// synchronized - simple
public synchronized void method() {
    // Code thread-safe
}

// Lock - flexible
private final Lock lock = new ReentrantLock();

public void method() {
    lock.lock();
    try {
        // Code thread-safe
    } finally {
        lock.unlock();  // TOUJOURS dans finally
    }
}

// tryLock - non bloquant
if (lock.tryLock()) {
    try {
        // Acquis
    } finally {
        lock.unlock();
    }
} else {
    // Pas acquis
}

// tryLock avec timeout
if (lock.tryLock(1, TimeUnit.SECONDS)) {
    try {
        // Acquis dans 1 seconde
    } finally {
        lock.unlock();
    }
}
```

---

**Q18: Ce code a-t-il un problème?**
```java
private final Lock lock = new ReentrantLock();

public void method() {
    lock.lock();
    try {
        if (condition) {
            return;  // Return dans le try
        }
        // Code
    } finally {
        lock.unlock();
    }
}
```

**R:** **Aucun problème**. Le `finally` est **toujours exécuté**, même avec `return`.

```java
public void method() {
    lock.lock();
    try {
        if (condition) {
            return;  // finally exécuté AVANT le return
        }
        // Code
    } finally {
        lock.unlock();  // Toujours exécuté
    }
}
```

---

**Q19: Quelle est la différence entre AtomicInteger et volatile int?**

**R:**

**volatile int**: Garantit **visibilité** (pas atomicité)
```java
private volatile int count = 0;

public void increment() {
    count++;  // PAS atomic (3 opérations)
    // Race condition possible
}
```

**AtomicInteger**: Garantit **visibilité + atomicité**
```java
private AtomicInteger count = new AtomicInteger(0);

public void increment() {
    count.incrementAndGet();  // Atomic
    // Thread-safe
}
```

```java
// volatile - bon usage
private volatile boolean flag = false;

public void setFlag() {
    flag = true;  // Simple assignment (atomic)
}

public void waitForFlag() {
    while (!flag) {  // Voit la valeur à jour
        // Attendre
    }
}

// volatile - mauvais usage
private volatile int count = 0;

public void increment() {
    count++;  // PAS atomic!
}
```

---

**Q20: Que fait AtomicInteger.compareAndSet()?**
```java
AtomicInteger atomic = new AtomicInteger(10);
boolean success = atomic.compareAndSet(10, 20);
```

**R:** **Compare-And-Set** (CAS): Compare la valeur actuelle, et si égale, la remplace.

```java
AtomicInteger atomic = new AtomicInteger(10);

// CAS: Si valeur == 10, mettre à 20
boolean success1 = atomic.compareAndSet(10, 20);
System.out.println(success1);  // true
System.out.println(atomic.get());  // 20

// CAS: Si valeur == 10, mettre à 30
boolean success2 = atomic.compareAndSet(10, 30);
System.out.println(success2);  // false (valeur est 20, pas 10)
System.out.println(atomic.get());  // 20 (inchangé)

// Utilisation: Retry loop
while (!atomic.compareAndSet(expectedValue, newValue)) {
    expectedValue = atomic.get();  // Réessayer avec nouvelle valeur
}
```

---

## Questions sur Concurrent Collections

**Q21: Quelle est la différence entre HashMap et ConcurrentHashMap?**

**R:**

**HashMap**: PAS thread-safe
```java
Map<String, Integer> map = new HashMap<>();

// Thread 1: put
map.put("key", 1);

// Thread 2: get (simultané)
Integer value = map.get("key");

// Peut causer: ConcurrentModificationException, données corrompues
```

**ConcurrentHashMap**: Thread-safe
```java
Map<String, Integer> map = new ConcurrentHashMap<>();

// Thread 1 et 2 peuvent opérer simultanément
map.put("key", 1);
Integer value = map.get("key");

// Thread-safe sans lock externe
```

---

**Q22: Ce code est-il thread-safe?**
```java
ConcurrentHashMap<String, Integer> map = new ConcurrentHashMap<>();

if (!map.containsKey("key")) {
    map.put("key", 1);
}
```

**R:** **NON**, pas atomic!

```java
// Thread 1: vérifie containsKey → false
// Thread 2: vérifie containsKey → false
// Thread 1: put("key", 1)
// Thread 2: put("key", 1)
// Résultat: écrasement possible

// Solution: putIfAbsent (atomic)
map.putIfAbsent("key", 1);

// Ou compute
map.compute("key", (k, v) -> (v == null) ? 1 : v + 1);

// Ou merge
map.merge("key", 1, Integer::sum);
```

---

**Q23: Quelle est la différence entre ArrayList et CopyOnWriteArrayList?**

**R:**

**ArrayList**: PAS thread-safe
```java
List<String> list = new ArrayList<>();

// Thread 1: add
list.add("item");

// Thread 2: iterate
for (String s : list) {  // ConcurrentModificationException
    System.out.println(s);
}
```

**CopyOnWriteArrayList**: Thread-safe pour lecture intensive
```java
List<String> list = new CopyOnWriteArrayList<>();

// Thread 1: add (lent - copie la liste)
list.add("item");

// Thread 2: iterate (rapide - pas de lock)
for (String s : list) {  // OK, pas d'exception
    System.out.println(s);
}

// Modifications pendant itération pas visibles
list.add("item1");
Iterator<String> it = list.iterator();  // Snapshot
list.add("item2");
while (it.hasNext()) {
    System.out.println(it.next());  // "item2" pas vu
}
```

---

**Q24: Quand utiliser CopyOnWriteArrayList?**

**R:** Quand **beaucoup de lectures**, **peu d'écritures**.

```java
// BON usage: Listeners/Observers
List<EventListener> listeners = new CopyOnWriteArrayList<>();

// Ajout rare
public void addListener(EventListener listener) {
    listeners.add(listener);  // Lent mais rare
}

// Lecture fréquente
public void notifyListeners(Event event) {
    for (EventListener listener : listeners) {  // Rapide, thread-safe
        listener.onEvent(event);
    }
}

// MAUVAIS usage: Beaucoup d'écritures
List<String> list = new CopyOnWriteArrayList<>();
for (int i = 0; i < 10000; i++) {
    list.add("item" + i);  // Très lent (10000 copies)
}
```

---

**Q25: Que fait BlockingQueue.put() vs offer()?**

**R:**

**put()**: **Bloque** si queue pleine
```java
BlockingQueue<String> queue = new LinkedBlockingQueue<>(2);  // Capacité 2

queue.put("item1");  // OK
queue.put("item2");  // OK
queue.put("item3");  // BLOQUE jusqu'à ce qu'une place se libère
```

**offer()**: **Ne bloque pas**, retourne boolean
```java
BlockingQueue<String> queue = new LinkedBlockingQueue<>(2);

queue.offer("item1");  // true
queue.offer("item2");  // true
queue.offer("item3");  // false (queue pleine)

// offer() avec timeout
boolean added = queue.offer("item3", 1, TimeUnit.SECONDS);
// Attend max 1 seconde, retourne false si timeout
```

---

**Q26: Quelle est la différence entre take() et poll()?**

**R:**

**take()**: **Bloque** si queue vide
```java
BlockingQueue<String> queue = new LinkedBlockingQueue<>();

String item = queue.take();  // BLOQUE jusqu'à ce qu'un élément soit disponible
```

**poll()**: **Ne bloque pas**, retourne null si vide
```java
String item = queue.poll();  // null si vide

// poll() avec timeout
String item2 = queue.poll(1, TimeUnit.SECONDS);
// Attend max 1 seconde, retourne null si timeout
```

---

## Questions sur Threading Problems

**Q27: Qu'est-ce qu'un deadlock?**

**R:** **Deadlock** = 2+ threads bloqués en attendant l'autre (cercle vicieux).

```java
public class DeadlockExample {
    private final Object lock1 = new Object();
    private final Object lock2 = new Object();
    
    public void method1() {
        synchronized(lock1) {
            System.out.println("Method1: holding lock1");
            
            synchronized(lock2) {  // Attend lock2
                System.out.println("Method1: holding both");
            }
        }
    }
    
    public void method2() {
        synchronized(lock2) {
            System.out.println("Method2: holding lock2");
            
            synchronized(lock1) {  // Attend lock1
                System.out.println("Method2: holding both");
            }
        }
    }
    
    public static void main(String[] args) {
        DeadlockExample example = new DeadlockExample();
        
        new Thread(example::method1).start();
        new Thread(example::method2).start();
        
        // Thread1: lock1 → attend lock2
        // Thread2: lock2 → attend lock1
        // → DEADLOCK (les deux bloqués)
    }
}

// Output:
// Method1: holding lock1
// Method2: holding lock2
// (puis silence - deadlock)
```

**Solution**: Acquérir les locks dans le **même ordre**.

```java
// Les DEUX méthodes acquièrent lock1 puis lock2
public void method1() {
    synchronized(lock1) {
        synchronized(lock2) {
            // Code
        }
    }
}

public void method2() {
    synchronized(lock1) {  // Même ordre
        synchronized(lock2) {
            // Code
        }
    }
}
```

---

**Q28: Qu'est-ce qu'une race condition?**

**R:** **Race condition** = résultat dépend du **timing** des threads (non déterministe).

```java
public class RaceCondition {
    private int balance = 100;
    
    public void withdraw(int amount) {
        if (balance >= amount) {  // 1. Vérifier
            // Context switch possible ici
            balance -= amount;     // 2. Retirer
        }
    }
}

// Thread 1: withdraw(80)
// Thread 1: vérifier balance (100 >= 80) ✓
// Thread 2: withdraw(80)
// Thread 2: vérifier balance (100 >= 80) ✓
// Thread 1: balance = 20
// Thread 2: balance = -60  (NÉGATIF!)

// Solution: synchronized
public synchronized void withdraw(int amount) {
    if (balance >= amount) {
        balance -= amount;
    }
}
```

---

**Q29: Qu'est-ce que la starvation?**

**R:** **Starvation** = thread ne reçoit **jamais** de ressources (affamé).

```java
public class Starvation {
    public static void main(String[] args) {
        final Object lock = new Object();
        
        // Thread haute priorité (accapare le lock)
        Thread high = new Thread(() -> {
            while (true) {
                synchronized(lock) {
                    // Travail continu
                }
            }
        });
        high.setPriority(Thread.MAX_PRIORITY);
        high.start();
        
        // Thread basse priorité (affamé)
        Thread low = new Thread(() -> {
            synchronized(lock) {
                System.out.println("Never executed!");
            }
        });
        low.setPriority(Thread.MIN_PRIORITY);
        low.start();
        
        // low thread n'obtient jamais le lock
    }
}

// Solution: Fair locks
Lock lock = new ReentrantLock(true);  // Fair = FIFO
```

---

**Q30: Qu'est-ce qu'un livelock?**

**R:** **Livelock** = threads actifs mais **bloqués** (cèdent constamment).

```java
public class Livelock {
    static class Person {
        private String name;
        
        public Person(String name) {
            this.name = name;
        }
        
        public void passDoorway(Person other, Doorway doorway) {
            while (doorway.isOccupied(other)) {
                System.out.println(name + ": After you!");
                try { Thread.sleep(10); } catch (InterruptedException e) {}
            }
            doorway.enter(this);
        }
    }
    
    // Alice et Bob cèdent constamment le passage
    // Les deux sont actifs mais aucun n'avance
}
```

---

## Questions sur Parallel Streams

**Q31: Quelle est la différence?**
```java
// Version 1
list.stream().forEach(System.out::println);

// Version 2
list.parallelStream().forEach(System.out::println);
```

**R:**
- **Version 1**: Exécution **séquentielle**, ordre **garanti**
- **Version 2**: Exécution **parallèle**, ordre **non garanti**

```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5);

// Sequential
numbers.stream().forEach(System.out::println);
// Output: 1, 2, 3, 4, 5 (toujours)

// Parallel
numbers.parallelStream().forEach(System.out::println);
// Output: 3, 1, 5, 2, 4 (ordre variable)
```

---

**Q32: Ce code est-il correct?**

```java
List<Integer> result = new ArrayList<>();
IntStream.range(1, 100)
    .parallel()
    .forEach(result::add);
```

**R:** **NON**, ArrayList n'est **pas thread-safe**.

```java
// Problème: Race condition
List<Integer> result = new ArrayList<>();
IntStream.range(1, 100)
    .parallel()
    .forEach(result::add);  // DANGER!

System.out.println(result.size());  // Pas toujours 99!
// Peut causer: ArrayIndexOutOfBoundsException, données perdues

// Solution 1: collect (thread-safe)
List<Integer> result = IntStream.range(1, 100)
    .parallel()
    .boxed()
    .collect(Collectors.toList());  // Thread-safe

// Solution 2: CopyOnWriteArrayList (lent)
List<Integer> result = new CopyOnWriteArrayList<>();
IntStream.range(1, 100)
    .parallel()
    .forEach(result::add);  // Thread-safe mais lent

// Solution 3: Collections.synchronizedList (lent)
List<Integer> result = Collections.synchronizedList(new ArrayList<>());
IntStream.range(1, 100)
    .parallel()
    .forEach(result::add);  // Thread-safe mais lent
```

---

**Q33: Quand utiliser parallelStream()?**

**R:** **Utiliser si**:
- Collection **grande** (> 10,000 éléments typiquement)
- Opérations **CPU-intensive** (calculs lourds)
- Opérations **indépendantes** (pas d'état partagé)
- Ordre **non important**

**Éviter si**:
- Collection **petite** (overhead > gain)
- Opérations **rapides** (< 1ms par élément)
- Opérations **I/O-bound** (lecture fichier, DB, réseau)
- **État mutable partagé**

```java
// BON usage: Grande collection, CPU-intensive
List<Integer> numbers = IntStream.range(1, 10000000)
    .boxed()
    .collect(Collectors.toList());

long sum = numbers.parallelStream()
    .mapToLong(n -> expensiveCalculation(n))  // Calcul lourd
    .sum();

// MAUVAIS usage: Petite collection
List<Integer> small = Arrays.asList(1, 2, 3, 4, 5);
small.parallelStream()  // Overhead > gain
    .forEach(System.out::println);

// MAUVAIS usage: I/O-bound
files.parallelStream()
    .forEach(file -> readFile(file));  // Limité par I/O, pas CPU

// MAUVAIS usage: État mutable
int[] counter = {0};
numbers.parallelStream()
    .forEach(n -> counter[0]++);  // Race condition!
```

---

**Q34: Quelle est la différence entre forEach() et forEachOrdered()?**
```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5);

// Version 1
numbers.parallelStream().forEach(System.out::println);

// Version 2
numbers.parallelStream().forEachOrdered(System.out::println);
```

**R:**
- **forEach()**: Ordre **non garanti** en parallel (rapide)
- **forEachOrdered()**: Ordre **préservé** même en parallel (plus lent)

```java
// forEach - ordre aléatoire
numbers.parallelStream()
    .forEach(System.out::println);
// Peut afficher: 3, 1, 5, 2, 4

// forEachOrdered - ordre préservé
numbers.parallelStream()
    .forEachOrdered(System.out::println);
// Affiche toujours: 1, 2, 3, 4, 5
```

---

**Q35: Ce code a-t-il un problème avec parallel stream?**
```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5);

int sum = numbers.parallelStream()
    .reduce(0, (a, b) -> a - b);
```

**R:** **OUI**, la soustraction n'est **pas associative**.

```java
// Opération associative (OK pour parallel)
int sum = numbers.parallelStream()
    .reduce(0, Integer::sum);  // OK: (a + b) + c = a + (b + c)

// Opération non-associative (PROBLÈME pour parallel)
int diff = numbers.parallelStream()
    .reduce(0, (a, b) -> a - b);  // FAUX: (a - b) - c ≠ a - (b - c)

// Résultat imprévisible:
// Thread 1: (0 - 1) - 2 = -3
// Thread 2: (0 - 3) - 4 = -7
// Combine: -3 - (-7) - 5 = ?  (imprévisible)

// RÈGLE: L'opération doit être associative ET commutative
// Associative: (a op b) op c = a op (b op c)
// Commutative: a op b = b op a
```

---

**Q36: Comment vérifier si un stream est parallèle?**
```java
Stream<Integer> stream = list.parallelStream();
```

**R:** Utiliser `isParallel()`:

```java
Stream<Integer> seq = list.stream();
System.out.println(seq.isParallel());  // false

Stream<Integer> par = list.parallelStream();
System.out.println(par.isParallel());  // true

// Convertir
Stream<Integer> nowParallel = seq.parallel();
Stream<Integer> nowSequential = par.sequential();

// Le dernier appel gagne
Stream<Integer> result = list.stream()
    .parallel()
    .sequential()
    .parallel();  // Parallèle (dernier appel)
```

---

**Q37: Ce code avec parallel stream est-il correct?**
```java
List<String> words = Arrays.asList("a", "b", "c");

words.parallelStream()
    .map(String::toUpperCase)
    .forEach(word -> System.out.println(Thread.currentThread().getName() + ": " + word));
```

**R:** **Correct**, mais affiche threads différents.

```java
// Output possible:
// ForkJoinPool.commonPool-worker-1: A
// ForkJoinPool.commonPool-worker-2: C
// main: B

// Parallel stream utilise ForkJoinPool.commonPool
// Nombre de threads = Runtime.getRuntime().availableProcessors()
```

---

**Q38: Comment limiter le nombre de threads dans parallel stream?**

**R:** **Pas directement possible** avec parallelStream(). Il utilise le **ForkJoinPool commun**.

```java
// parallelStream() utilise le pool commun
list.parallelStream()  // ForkJoinPool.commonPool()
    .forEach(System.out::println);

// Nombre de threads = CPU cores
int cores = Runtime.getRuntime().availableProcessors();

// Workaround: Custom ForkJoinPool (pas recommandé)
ForkJoinPool customPool = new ForkJoinPool(4);  // 4 threads
customPool.submit(() -> 
    list.parallelStream()
        .forEach(System.out::println)
).get();

// Meilleur: Utiliser ExecutorService avec CompletableFuture
ExecutorService executor = Executors.newFixedThreadPool(4);
List<CompletableFuture<Void>> futures = list.stream()
    .map(item -> CompletableFuture.runAsync(() -> process(item), executor))
    .collect(Collectors.toList());

CompletableFuture.allOf(futures.toArray(new CompletableFuture[0])).join();
executor.shutdown();
```

---

**Q39: Ce code a-t-il un problème de performance?**
```java
List<Integer> numbers = IntStream.range(1, 10)
    .boxed()
    .collect(Collectors.toList());

numbers.parallelStream()
    .map(n -> n * 2)
    .forEach(System.out::println);
```

**R:** **OUI**, petite collection (9 éléments) avec parallel stream.

```java
// Overhead du parallélisme > gain
// - Création threads
// - Split/merge
// - Synchronisation

// Pour 9 éléments, sequential est plus rapide
numbers.stream()  // Sequential
    .map(n -> n * 2)
    .forEach(System.out::println);

// Parallel utile pour grandes collections
List<Integer> large = IntStream.range(1, 1000000)
    .boxed()
    .collect(Collectors.toList());

large.parallelStream()  // Maintenant utile
    .map(n -> expensiveCalculation(n))
    .forEach(System.out::println);
```

---

**Q40: Que se passe-t-il avec findAny() en parallel?**
```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5);

Optional<Integer> any1 = numbers.stream().findAny();
Optional<Integer> any2 = numbers.parallelStream().findAny();
```

**R:**
- **Sequential**: `findAny()` = `findFirst()` (retourne premier)
- **Parallel**: `findAny()` retourne **n'importe quel** élément (non déterministe)

```java
// Sequential - toujours le premier
Optional<Integer> seq = numbers.stream().findAny();
System.out.println(seq.get());  // Toujours 1

// Parallel - aléatoire
Optional<Integer> par = numbers.parallelStream().findAny();
System.out.println(par.get());  // Peut être 1, 2, 3, 4, ou 5

// findFirst() préserve l'ordre même en parallel
Optional<Integer> first = numbers.parallelStream().findFirst();
System.out.println(first.get());  // Toujours 1
```

---

## Questions Avancées et Cas Pratiques

**Q41: Ce code a-t-il un problème?**
```java
public class BrokenCounter {
    private int count = 0;
    
    public synchronized void increment() {
        count++;
    }
    
    public int getCount() {  // PAS synchronized
        return count;
    }
}
```

**R:** **OUI**, `getCount()` devrait aussi être synchronized.

```java
// Problème: Visibilité
// Thread 1: increment() (synchronized)
// Thread 2: getCount() (pas synchronized)
// Thread 2 peut voir une vieille valeur de count

// Solution 1: synchronized getCount
public synchronized int getCount() {
    return count;
}

// Solution 2: volatile
private volatile int count = 0;

public synchronized void increment() {
    count++;
}

public int getCount() {  // Pas besoin de synchronized avec volatile
    return count;
}

// Solution 3: AtomicInteger (meilleure)
private AtomicInteger count = new AtomicInteger(0);

public void increment() {
    count.incrementAndGet();
}

public int getCount() {
    return count.get();
}
```

---

**Q42: Ce lock est-il bien utilisé?**
```java
private final Lock lock = new ReentrantLock();

public void method() {
    lock.lock();
    if (condition) {
        lock.unlock();
        return;
    }
    // Code
    lock.unlock();
}
```

**R:** **NON**, mauvaise gestion du unlock.

```java
// Problème: Si exception entre lock et unlock, le lock n'est jamais libéré

// Solution: TOUJOURS unlock dans finally
public void method() {
    lock.lock();
    try {
        if (condition) {
            return;  // finally exécuté avant return
        }
        // Code
    } finally {
        lock.unlock();  // Toujours exécuté
    }
}
```

---

**Q43: Quelle est la différence entre wait() et sleep()?**

**R:**

| **wait()** | **sleep()** |
|------------|-------------|
| Méthode d'**Object** | Méthode de **Thread** |
| Doit être dans **synchronized** | N'importe où |
| **Libère** le lock | **Garde** le lock |
| Réveil par **notify()** | Réveil après **timeout** |
| **InterruptedException** | **InterruptedException** |

```java
// wait() - libère le lock
synchronized(lock) {
    lock.wait();  // Libère le lock, attend notify()
}

// sleep() - garde le lock
synchronized(lock) {
    Thread.sleep(1000);  // Garde le lock pendant 1 seconde
}

// Exemple Producer-Consumer
private final Object lock = new Object();
private Queue<String> queue = new LinkedList<>();

// Producer
public void produce(String item) {
    synchronized(lock) {
        queue.add(item);
        lock.notify();  // Réveille un consumer
    }
}

// Consumer
public String consume() throws InterruptedException {
    synchronized(lock) {
        while (queue.isEmpty()) {
            lock.wait();  // Attend un item
        }
        return queue.poll();
    }
}
```

---

**Q44: Ce code a-t-il un problème?**
```java
private static int counter = 0;

public static synchronized void increment() {
    counter++;
}
```

**R:** **Potentiellement**. `synchronized static` lock sur la **classe** (pas l'instance).

```java
// synchronized static - lock sur Class object
public static synchronized void increment() {
    counter++;
}
// Équivalent à:
public static void increment() {
    synchronized(BrokenCounter.class) {
        counter++;
    }
}

// synchronized instance - lock sur this
public synchronized void increment() {
    counter++;
}
// Équivalent à:
public void increment() {
    synchronized(this) {
        counter++;
    }
}

// Problème: Si on mélange static et instance synchronized
// Ils n'utilisent PAS le même lock!
public static synchronized void staticMethod() {
    // Lock sur Class
}

public synchronized void instanceMethod() {
    // Lock sur this
}

// Ces deux méthodes peuvent s'exécuter en PARALLÈLE
```

---

**Q45: Comment implémenter un cache thread-safe simple?**

**R:**

```java
import java.util.concurrent.ConcurrentHashMap;

public class SimpleCache<K, V> {
    private final ConcurrentHashMap<K, V> cache = new ConcurrentHashMap<>();
    
    // Get ou compute si absent
    public V get(K key, Function<K, V> loader) {
        return cache.computeIfAbsent(key, loader);
    }
    
    // Put
    public void put(K key, V value) {
        cache.put(key, value);
    }
    
    // Remove
    public void remove(K key) {
        cache.remove(key);
    }
    
    // Clear
    public void clear() {
        cache.clear();
    }
}

// Utilisation
SimpleCache<String, User> userCache = new SimpleCache<>();

User user = userCache.get("user123", id -> loadUserFromDB(id));
// Si "user123" existe, retourne du cache
// Sinon, appelle loadUserFromDB() et met en cache
```

---

**Q46: Comment implémenter Producer-Consumer avec BlockingQueue?**

**R:**

```java
import java.util.concurrent.*;

public class ProducerConsumer {
    private static final BlockingQueue<String> queue = new LinkedBlockingQueue<>(10);
    
    // Producer
    static class Producer implements Runnable {
        @Override
        public void run() {
            try {
                for (int i = 0; i < 20; i++) {
                    String item = "Item-" + i;
                    queue.put(item);  // Bloque si queue pleine
                    System.out.println("Produced: " + item);
                    Thread.sleep(100);
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
    
    // Consumer
    static class Consumer implements Runnable {
        @Override
        public void run() {
            try {
                while (true) {
                    String item = queue.take();  // Bloque si queue vide
                    System.out.println("Consumed: " + item);
                    Thread.sleep(200);
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
    
    public static void main(String[] args) {
        ExecutorService executor = Executors.newFixedThreadPool(3);
        
        executor.submit(new Producer());
        executor.submit(new Consumer());
        executor.submit(new Consumer());
        
        // Producer produit plus vite que consumers consomment
        // BlockingQueue gère automatiquement la synchronisation
    }
}
```

---

**Q47: Comment gérer l'interruption d'un thread?**

**R:**

```java
// Thread interruptible
public class InterruptibleTask implements Runnable {
    @Override
    public void run() {
        try {
            while (!Thread.currentThread().isInterrupted()) {
                // Travail
                System.out.println("Working...");
                Thread.sleep(1000);  // Throws InterruptedException si interrompu
            }
        } catch (InterruptedException e) {
            // Thread interrompu pendant sleep
            System.out.println("Interrupted during sleep");
            Thread.currentThread().interrupt();  // Restaurer le flag
        }
        System.out.println("Thread stopped");
    }
}

// Utilisation
Thread thread = new Thread(new InterruptibleTask());
thread.start();

Thread.sleep(3000);
thread.interrupt();  // Interrompre le thread

// IMPORTANT: 
// - InterruptedException efface le flag interrupted
// - Restaurer avec Thread.currentThread().interrupt()
// - Vérifier régulièrement Thread.interrupted() ou isInterrupted()
```

---

**Q48: Comment attendre qu'un ExecutorService termine toutes ses tâches?**

**R:**

```java
ExecutorService executor = Executors.newFixedThreadPool(5);

// Soumettre tâches
for (int i = 0; i < 10; i++) {
    executor.submit(() -> {
        Thread.sleep(1000);
        System.out.println("Task done");
    });
}

// Méthode 1: shutdown + awaitTermination
executor.shutdown();  // Pas de nouvelles tâches
boolean finished = executor.awaitTermination(10, TimeUnit.SECONDS);
if (finished) {
    System.out.println("All tasks completed");
} else {
    System.out.println("Timeout");
    executor.shutdownNow();  // Forcer l'arrêt
}

// Méthode 2: CountDownLatch
int taskCount = 10;
CountDownLatch latch = new CountDownLatch(taskCount);

for (int i = 0; i < taskCount; i++) {
    executor.submit(() -> {
        try {
            // Travail
        } finally {
            latch.countDown();
        }
    });
}

latch.await();  // Attendre que toutes comptent down à 0
System.out.println("All tasks completed");
executor.shutdown();

// Méthode 3: invokeAll
List<Callable<Void>> tasks = new ArrayList<>();
for (int i = 0; i < 10; i++) {
    tasks.add(() -> {
        Thread.sleep(1000);
        return null;
    });
}

executor.invokeAll(tasks);  // Attend que toutes terminent
System.out.println("All tasks completed");
executor.shutdown();
```

---

**Q49: Comment implémenter un compteur thread-safe avec timeout?**

**R:**

```java
import java.util.concurrent.locks.*;

public class TimedCounter {
    private int count = 0;
    private final Lock lock = new ReentrantLock();
    
    // Increment avec timeout
    public boolean increment(long timeout, TimeUnit unit) 
            throws InterruptedException {
        if (lock.tryLock(timeout, unit)) {
            try {
                count++;
                return true;
            } finally {
                lock.unlock();
            }
        }
        return false;  // Timeout
    }
    
    public int getCount() {
        lock.lock();
        try {
            return count;
        } finally {
            lock.unlock();
        }
    }
}

// Utilisation
TimedCounter counter = new TimedCounter();

ExecutorService executor = Executors.newFixedThreadPool(10);
for (int i = 0; i < 100; i++) {
    executor.submit(() -> {
        try {
            boolean success = counter.increment(1, TimeUnit.SECONDS);
            if (!success) {
                System.out.println("Timeout!");
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    });
}

executor.shutdown();
executor.awaitTermination(10, TimeUnit.SECONDS);
System.out.println("Count: " + counter.getCount());
```

---

**Q50: Comment éviter ce deadlock?**
```java
public class Transfer {
    public void transfer(Account from, Account to, int amount) {
        synchronized(from) {
            synchronized(to) {
                from.withdraw(amount);
                to.deposit(amount);
            }
        }
    }
}

// Thread 1: transfer(A, B, 100)
// Thread 2: transfer(B, A, 50)
// → Deadlock
```

**R:** **Ordonner les locks** par un critère stable (ex: ID).

```java
public class SafeTransfer {
    public void transfer(Account from, Account to, int amount) {
        // Toujours lock dans l'ordre des IDs
        Account first = from.getId() < to.getId() ? from : to;
        Account second = from.getId() < to.getId() ? to : from;
        
        synchronized(first) {
            synchronized(second) {
                from.withdraw(amount);
                to.deposit(amount);
            }
        }
    }
}

// Thread 1: transfer(A, B, 100) → lock A puis B
// Thread 2: transfer(B, A, 50)  → lock A puis B (même ordre)
// → Pas de deadlock
```

---

**Q51: Comment implémenter un ReadWriteLock?**

**R:**

```java
import java.util.concurrent.locks.*;

public class SharedResource {
    private final ReadWriteLock rwLock = new ReentrantReadWriteLock();
    private final Lock readLock = rwLock.readLock();
    private final Lock writeLock = rwLock.writeLock();
    private int value = 0;
    
    // Lecture - plusieurs threads simultanés
    public int read() {
        readLock.lock();
        try {
            System.out.println("Reading: " + value);
            return value;
        } finally {
            readLock.unlock();
        }
    }
    
    // Écriture - un seul thread à la fois
    public void write(int newValue) {
        writeLock.lock();
        try {
            System.out.println("Writing: " + newValue);
            value = newValue;
        } finally {
            writeLock.unlock();
        }
    }
}

// Avantages:
// - Plusieurs readers simultanés (performance)
// - Writer exclusif (sécurité)
// - Pas de starvation avec fair lock
ReadWriteLock fairLock = new ReentrantReadWriteLock(true);
```

---

**Q52: Comment utiliser CompletableFuture pour parallélisme?**

**R:**

```java
import java.util.concurrent.*;

// Tâche asynchrone simple
CompletableFuture<String> future = CompletableFuture.supplyAsync(() -> {
    Thread.sleep(1000);
    return "Result";
});

String result = future.get();  // Bloque jusqu'au résultat

// Chaîner des opérations
CompletableFuture<String> chain = CompletableFuture.supplyAsync(() -> "Hello")
    .thenApply(s -> s + " World")
    .thenApply(String::toUpperCase);

System.out.println(chain.get());  // "HELLO WORLD"

// Combiner plusieurs futures
CompletableFuture<Integer> future1 = CompletableFuture.supplyAsync(() -> 10);
CompletableFuture<Integer> future2 = CompletableFuture.supplyAsync(() -> 20);

CompletableFuture<Integer> combined = future1.thenCombine(future2, (a, b) -> a + b);
System.out.println(combined.get());  // 30

// Exécuter en parallèle et attendre toutes
List<CompletableFuture<String>> futures = Arrays.asList(
    CompletableFuture.supplyAsync(() -> "Task1"),
    CompletableFuture.supplyAsync(() -> "Task2"),
    CompletableFuture.supplyAsync(() -> "Task3")
);

CompletableFuture<Void> allOf = CompletableFuture.allOf(
    futures.toArray(new CompletableFuture[0])
);

allOf.join();  // Attendre toutes
System.out.println("All tasks completed");
```

---