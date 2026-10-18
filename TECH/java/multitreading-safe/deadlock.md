Parfait — **deadlock** est un classique en entretien 👇

---

# 🔹 C’est quoi un Deadlock ?

Un **deadlock (interblocage)** se produit quand :

👉 **plusieurs threads se bloquent mutuellement**
👉 chacun attend une ressource détenue par l’autre
👉 ➜ **aucun ne peut continuer**

💥 résultat : programme bloqué à vie

---

## 🔹 Exemple simple

```java
class Resource {}

Resource r1 = new Resource();
Resource r2 = new Resource();

Thread t1 = new Thread(() -> {
    synchronized (r1) {
        System.out.println("T1 a verrouillé r1");
        try { Thread.sleep(100); } catch (Exception e) {}

        synchronized (r2) {
            System.out.println("T1 a verrouillé r2");
        }
    }
});

Thread t2 = new Thread(() -> {
    synchronized (r2) {
        System.out.println("T2 a verrouillé r2");
        try { Thread.sleep(100); } catch (Exception e) {}

        synchronized (r1) {
            System.out.println("T2 a verrouillé r1");
        }
    }
});

t1.start();
t2.start();
```

---

## 🔥 Ce qui se passe

* `t1` verrouille `r1`
* `t2` verrouille `r2`
* `t1` attend `r2`
* `t2` attend `r1`

👉 💥 **blocage total (deadlock)**

---

## 🔹 Conditions d’un deadlock (important entretien)

Il faut **4 conditions** :

1. **Mutual exclusion** → une ressource = un seul thread
2. **Hold and wait** → thread garde une ressource et attend une autre
3. **No preemption** → pas de retrait forcé
4. **Circular wait** → attente circulaire

👉 si les 4 sont réunies → deadlock possible

---

## 🔹 Solutions

### ✅ 1. Ordre de verrouillage (best pratique)

👉 toujours verrouiller dans le même ordre :

```java
synchronized (r1) {
    synchronized (r2) {
        // OK
    }
}
```

✔️ tous les threads respectent le même ordre → pas de cycle

---

### ✅ 2. `tryLock()` avec timeout

```java
import java.util.concurrent.locks.ReentrantLock;

if (lock1.tryLock()) {
    try {
        if (lock2.tryLock()) {
            try {
                // travail
            } finally {
                lock2.unlock();
            }
        }
    } finally {
        lock1.unlock();
    }
}
```

👉 évite blocage infini

---

### ✅ 3. Timeout / retry

👉 si blocage → abandon + retry

---

### ✅ 4. Réduire les verrous

👉 moins de locks = moins de risques

---

### ✅ 5. Design sans lock

👉 ex :

* immutabilité
* message queue (Kafka / RabbitMQ)
* architecture async

---

## 🔹 Résumé (version entretien)

> Un deadlock est une situation où plusieurs threads restent bloqués car chacun attend une ressource détenue par un autre.
> On l’évite en imposant un ordre de verrouillage, en utilisant des timeouts (`tryLock`), ou en réduisant les dépendances entre ressources.

---

## 🔹 Niveau senior (très important)

👉 En prod (Spring / microservices) :

deadlock fréquent sur :

* base de données (transactions)
* verrous pessimistes

👉 solutions réelles :

* **ordre cohérent des accès DB**
* **transactions courtes**
* **indexation correcte**
* éviter `SELECT ... FOR UPDATE` mal utilisé

---

## 🔹 Diff rapide (souvent demandé)

| Problème       | Description               |
| -------------- | ------------------------- |
| Data race      | accès concurrent non sync |
| Race condition | dépend de l’ordre         |
| Deadlock       | blocage total             |

