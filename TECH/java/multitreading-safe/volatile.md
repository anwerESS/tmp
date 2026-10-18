Le mot-clé **`volatile`** en Java sert à gérer **la visibilité entre threads**.

---

# 🔹 C’est quoi `volatile` ?

👉 Une variable `volatile` garantit que :

1. **tous les threads voient la dernière valeur**
2. pas de cache “local” CPU → lecture directe en mémoire

---

## 🔹 Exemple problème (sans volatile)

```java id="v4q2ys"
class Flag {
    boolean running = true;
}
```

```java id="2z55b2"
Flag flag = new Flag();

Thread t = new Thread(() -> {
    while (flag.running) {
        // boucle infinie possible ❌
    }
});

t.start();

Thread.sleep(1000);
flag.running = false;
```

💥 Problème :

* le thread peut **ne jamais voir `false`**
* boucle infinie

---

## 🔹 Solution avec `volatile`

```java id="p0j4ds"
class Flag {
    volatile boolean running = true;
}
```

👉 maintenant :

* modification visible immédiatement
* le thread s’arrête correctement ✅

---

## 🔹 Ce que `volatile` garantit

### ✅ 1. Visibilité

👉 tous les threads voient la même valeur

---

### ✅ 2. Ordre (happens-before)

👉 empêche certaines réorganisations du CPU

---

## 🔴 Ce que `volatile` NE garantit PAS

👉 ❌ **pas d’atomicité**

```java id="q7x48i"
volatile int count = 0;

count++; // ❌ PAS thread-safe
```

👉 toujours une data race possible

---

## 🔹 Quand utiliser `volatile` ?

### ✔️ Cas typiques

* flag de contrôle (start/stop)
* état simple (boolean, int)
* double-checked locking (avec précautions)

---

## 🔹 Quand NE PAS utiliser ?

❌ si tu fais :

* `++`
* `+=`
* logique métier complexe

👉 utilise :

* `synchronized`
* `AtomicInteger`
* `Lock`

---

## 🔹 Comparaison rapide

| Outil           | Visibilité | Atomicité | Lock |
| --------------- | ---------- | --------- | ---- |
| `volatile`      | ✅          | ❌         | ❌    |
| `synchronized`  | ✅          | ✅         | ✅    |
| `AtomicInteger` | ✅          | ✅         | ❌    |

---

## 🔹 Exemple réel (backend)

👉 arrêt propre d’un worker :

```java id="d6kz7w"
class Worker implements Runnable {
    private volatile boolean running = true;

    public void stop() {
        running = false;
    }

    public void run() {
        while (running) {
            // traitement
        }
    }
}
```

---

## 🔹 Résumé (version entretien)

> `volatile` garantit la visibilité des variables entre threads et empêche certaines optimisations CPU.
> En revanche, il ne garantit pas l’atomicité, donc il ne remplace pas `synchronized` ou les classes atomiques.

---

## 🔹 Niveau senior (réponse parfaite)

> “J’utilise `volatile` pour des flags ou des états simples.
> Pour des opérations atomiques ou critiques, je préfère `AtomicInteger` ou `synchronized`.”
