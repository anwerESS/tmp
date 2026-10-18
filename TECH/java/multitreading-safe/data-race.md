Une **data race (condition de concurrence)** arrive quand **plusieurs threads accèdent à une même donnée en même temps**, et **au moins un thread la modifie**, sans synchronisation.

👉 Résultat : **valeurs incohérentes / bugs aléatoires**

---

## 🔹 Exemple simple (problème)

```java
class Counter {
    int count = 0;

    void increment() {
        count++; // ❌ non thread-safe
    }
}
```

```java
Counter c = new Counter();

Thread t1 = new Thread(() -> {
    for (int i = 0; i < 1000; i++) c.increment();
});

Thread t2 = new Thread(() -> {
    for (int i = 0; i < 1000; i++) c.increment();
});

t1.start();
t2.start();

t1.join();
t2.join();

System.out.println(c.count); // ❌ attendu: 2000, mais souvent < 2000
```

---

## 🔹 Pourquoi ça arrive ?

`count++` n’est **pas atomique** :

```java
count = count + 1;
```

👉 En réalité :

1. lire `count`
2. ajouter 1
3. écrire `count`

💥 Si deux threads font ça en même temps → perte d’incrément

---

## 🔹 Solutions

### ✅ 1. `synchronized` (simple et sûr)

```java
class Counter {
    int count = 0;

    synchronized void increment() {
        count++;
    }
}
```

👉 garantit qu’un seul thread exécute la méthode à la fois

---

### ✅ 2. `AtomicInteger` (meilleure perf)

```java
import java.util.concurrent.atomic.AtomicInteger;

class Counter {
    AtomicInteger count = new AtomicInteger(0);

    void increment() {
        count.incrementAndGet();
    }
}
```

👉 opération **atomique sans verrou**

---

### ✅ 3. `ReentrantLock` (plus flexible)

```java
import java.util.concurrent.locks.ReentrantLock;

class Counter {
    int count = 0;
    ReentrantLock lock = new ReentrantLock();

    void increment() {
        lock.lock();
        try {
            count++;
        } finally {
            lock.unlock();
        }
    }
}
```

---

### ✅ 4. Éviter le partage (best practice)

👉 solution la plus propre :

* variables locales
* immutabilité (`final`)
* design sans état partagé

---

## 🔹 Résumé (version entretien)

> Une data race se produit lorsque plusieurs threads accèdent simultanément à une donnée partagée, dont au moins un en écriture, sans synchronisation.
> Elle peut être résolue avec `synchronized`, `AtomicInteger`, ou des locks, ou en évitant le partage de données.

---

## 🔹 Niveau senior (ce que les recruteurs aiment)

👉 Différence clé :

* `synchronized` → simple mais bloquant
* `Atomic` → performant (CAS, lock-free)
* `Lock` → plus de contrôle (timeout, tryLock)

👉 Et surtout :

💡 **“Le meilleur moyen d’éviter une data race, c’est de ne pas partager l’état.”**

