`ThreadLocal` en Java sert à donner **une variable propre à chaque thread**.

👉 Chaque thread a **sa propre copie** de la variable
➡️ pas de partage → pas de concurrence → pas de `synchronized`

---

# 🔹 C’est quoi `ThreadLocal` ?

> Une variable dont **la valeur est isolée par thread**

👉 Même variable → valeurs différentes selon le thread

---

## 🔹 Exemple simple

```java id="z1h3xt"
ThreadLocal<Integer> threadLocal = ThreadLocal.withInitial(() -> 0);

Runnable task = () -> {
    threadLocal.set((int)(Math.random() * 100));
    System.out.println(Thread.currentThread().getName() + " -> " + threadLocal.get());
};

new Thread(task).start();
new Thread(task).start();
```

👉 Chaque thread affiche une valeur différente
➡️ aucune interférence

---

# 🔹 Pourquoi utiliser `ThreadLocal` ?

👉 éviter le partage de données entre threads :

* pas de data race
* pas de lock
* code plus simple

---

# 🔹 Cas d’usage réels (important)

### ✔️ 1. Contexte utilisateur (backend)

* userId
* requestId
* transactionId

---

### ✔️ 2. Connexion DB / session

* garder une connexion par thread

---

### ✔️ 3. Formatters non thread-safe

```java id="i5z5jd"
ThreadLocal<SimpleDateFormat> formatter =
    ThreadLocal.withInitial(() -> new SimpleDateFormat("yyyy-MM-dd"));
```

---

### ✔️ 4. Logging (MDC)

👉 très utilisé avec Spring Framework / Logback

---

# 🔹 Attention ⚠️ (très important)

## ❌ Memory leak avec thread pool

👉 problème classique :

* thread pool réutilise les threads
* `ThreadLocal` garde la valeur en mémoire

➡️ fuite mémoire 💥

---

## ✅ Bonne pratique

```java id="v3k7z2"
try {
    threadLocal.set(value);
    // traitement
} finally {
    threadLocal.remove(); // 🔥 très important
}
```

---

# 🔹 Diff avec variable normale

| Type             | Partage           |
| ---------------- | ----------------- |
| variable normale | partagée          |
| `ThreadLocal`    | isolée par thread |

---

# 🔹 Résumé (version entretien)

> `ThreadLocal` permet d’avoir une variable propre à chaque thread, évitant ainsi les problèmes de concurrence sans utiliser de synchronisation.
> Il est utile pour stocker des contextes comme des identifiants ou des ressources par thread.

---

# 🔹 Niveau senior (ce qui fait la diff)

👉 phrase clé :

> “ThreadLocal est très utile, mais dangereux avec les thread pools si on oublie `remove()`.”

---

👉 En prod (Spring, ton contexte) :

* utilisé indirectement :

  * `SecurityContext`
  * `TransactionContext`
  * logs (MDC)

---

## ⚡ Conclusion

👉 puissant pour éviter la concurrence
👉 ⚠️ dangereux si mal utilisé (memory leak)

