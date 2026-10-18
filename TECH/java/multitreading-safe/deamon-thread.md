En Java, quand tu fais :

```java
thread.setDaemon(true);
```

👉 tu déclares que ce thread est un **thread “daemon” (arrière-plan)**.

---

## 🔹 C’est quoi un thread daemon ?

Un **daemon thread** est un thread qui tourne en arrière-plan pour supporter le programme, mais :

👉 **il ne bloque pas l’arrêt de l’application**

---

## 🔹 Comportement clé

* Si **tous les threads “normaux” (user threads)** sont terminés
  ➡️ **la JVM s’arrête automatiquement**
  ➡️ même si des threads daemon sont encore en cours

💥 Donc un daemon peut être **coupé brutalement**

---

## 🔹 Exemple

```java
Thread t = new Thread(() -> {
    while (true) {
        System.out.println("Daemon en cours...");
        try {
            Thread.sleep(1000);
        } catch (Exception e) {
        }
    }
});

t.

setDaemon(true); // 👈 important AVANT start
t.

start();

System.out.

println("Main terminé");
```

---

### 🔹 Résultat

👉 Tu verras peut-être :

```
Main terminé
Daemon en cours...
```

Puis le programme s’arrête rapidement
➡️ le daemon est **tué automatiquement**

---

## 🔹 Important ⚠️

```java
t.setDaemon(true);
t.

start();
```

✔️ Il faut appeler `setDaemon(true)` **AVANT** `start()`
❌ Sinon → exception :

```
IllegalThreadStateException
```

---

## 🔹 Cas d’usage typiques

Threads daemon = tâches de fond :

* Garbage Collector (ex: Java Virtual Machine interne)
* monitoring
* nettoyage de cache
* logs
* heartbeat

---

## 🔹 À éviter

❌ Ne pas utiliser daemon pour :

* traitement métier important
* sauvegarde de données
* opérations critiques

👉 car il peut être interrompu sans warning

---

## 🔹 Résumé (version entretien)

> Un daemon thread est un thread d’arrière-plan qui ne bloque pas l’arrêt de la JVM.
> Dès que tous les threads utilisateur sont terminés, la JVM s’arrête et les daemon threads sont stoppés
> automatiquement.

