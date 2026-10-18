Bonne question — et important en entretien :
👉 **data race ⊂ race condition** (la data race est un cas particulier)

---

# 🔹 C’est quoi une race condition ?

Une **race condition** se produit quand :

👉 **le résultat dépend de l’ordre d’exécution des threads**

➡️ Si l’ordre change → le résultat change (bug possible)

---

## 🔹 Exemple classique (bug logique)

```java
class BankAccount {
    int balance = 100;

    void withdraw(int amount) {
        if (balance >= amount) {
            // ⚠️ fenêtre critique
            balance = balance - amount;
        }
    }
}
```

```java
BankAccount acc = new BankAccount();

Thread t1 = new Thread(() -> acc.withdraw(80));
Thread t2 = new Thread(() -> acc.withdraw(80));

t1.start();
t2.start();
```

---

## 🔥 Problème

👉 Les deux threads peuvent voir `balance = 100`
👉 Les deux passent le `if`
👉 Résultat final :

```text
balance = -60 ❌
```

💥 argent “créé” → bug critique

---

## 🔹 Pourquoi ?

👉 L’opération complète n’est **pas atomique**

```text
check (balance >= amount)
+
update (balance -= amount)
```

➡️ deux threads s’intercalent → incohérence

---

## 🔹 Solutions

### ✅ 1. `synchronized`

```java
synchronized void withdraw(int amount) {
    if (balance >= amount) {
        balance -= amount;
    }
}
```

---

### ✅ 2. `ReentrantLock`

```java
lock.lock();
try {
    if (balance >= amount) {
        balance -= amount;
    }
} finally {
    lock.unlock();
}
```

---

### ✅ 3. Approche atomique (CAS)

```java
AtomicInteger balance = new AtomicInteger(100);

void withdraw(int amount) {
    while (true) {
        int current = balance.get();
        if (current < amount) return;

        if (balance.compareAndSet(current, current - amount)) {
            return;
        }
    }
}
```

---

### ✅ 4. Design sans état partagé (best)

👉 Exemple backend :

* une requête → une transaction
* isolation DB (`SELECT FOR UPDATE`)
* éviter logique concurrente en mémoire

---

## 🔹 Différence avec data race (important entretien)

| Concept            | Description                                     |
| ------------------ | ----------------------------------------------- |
| **Race condition** | problème logique dépendant de l’ordre           |
| **Data race**      | accès concurrent non synchronisé à une variable |

👉 Donc :

> Toute data race est une race condition
> mais toute race condition n’est pas forcément une data race

---

## 🔹 Résumé (version entretien)

> Une race condition survient lorsque le comportement d’un programme dépend de l’ordre d’exécution des threads, ce qui peut entraîner des résultats incohérents.
> On la résout avec synchronisation (`synchronized`, locks), opérations atomiques, ou en évitant le partage d’état.

---

## 🔹 Niveau senior (ce qui fait la différence)

👉 En backend réel (Spring / microservices) :

* problème fréquent sur :

  * paiements
  * stocks
  * réservations

👉 solutions pro :

* **verrou DB (pessimistic locking)**
* **optimistic locking (versioning)**
* **idempotency keys**
* **queues (Kafka / RabbitMQ)** pour sérialiser
