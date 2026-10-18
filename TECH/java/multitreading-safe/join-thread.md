En Java, la méthode **`join()`** sur un thread sert à dire :

👉 **“Attends que ce thread termine avant de continuer”**

---

### 🔹 Exemple simple

```java
Thread t = new Thread(() -> {
    System.out.println("Thread en cours...");
    try { Thread.sleep(2000); } catch (Exception e) {}
    System.out.println("Thread terminé");
});

t.start();

t.join(); // 👈 le thread principal attend ici

System.out.println("Suite du programme");
```

---

### 🔹 Ce qui se passe

1. `t.start()` → lance le thread
2. `t.join()` → bloque le thread courant (ex: main)
3. Le programme **attend que `t` finisse**
4. Ensuite seulement → `"Suite du programme"` s’exécute

---

### 🔹 Sans `join()`

Sans `join()`, le `main` **ne va pas attendre**, donc l’ordre peut être :

```
Suite du programme
Thread en cours...
Thread terminé
```

---

### 🔹 Avec `join()`

Avec `join()`, tu garantis :

```
Thread en cours...
Thread terminé
Suite du programme
```

---

### 🔹 Variantes utiles

```java
t.join(1000); // attend max 1 seconde
```

👉 Si le thread n’est pas fini après 1s → le programme continue quand même.

---

### 🔹 En résumé (version entretien)

> `join()` permet de bloquer le thread courant jusqu’à la fin d’un autre thread.
> C’est utile pour synchroniser l’exécution et garantir un ordre entre les threads.
