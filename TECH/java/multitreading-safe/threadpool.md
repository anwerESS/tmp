Un **thread pool** (pool de threads) est un mécanisme qui permet de **réutiliser un ensemble de threads déjà créés** au lieu d’en créer un nouveau à chaque tâche.

👉 En clair :
**tu as une “réserve” de threads prêts à travailler**, et tu leur envoies des tâches.

---

## 🔹 Pourquoi utiliser un thread pool ?

Créer un thread est **coûteux** (CPU + mémoire).
Si tu fais ça à chaque requête :

```java
new Thread(() -> doWork()).start();
```

👉 ton application va :

* consommer beaucoup de ressources
* devenir instable sous charge
* risquer des crashs (trop de threads)

---

## 🔹 Principe

![Image](https://images.openai.com/static-rsc-4/FOh0kQ4_DSywwx7bBygYNpS8XpTmyFh7OE97Gl69pEKSnh3VVq4HtrWku_ht3DdfNCPQQZcvRkPYMMCFhsMp8eR00CFhrWTN5z3OTPaHBfIkcfKhcBjUL7IEqskLkxebrA9Tz6CaaK4iCSEkX6u79uHosHbYHGdxLBHxXNtrDf1O7KO_pWGwhSBgNIqwOvM7?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/fxHqnzwOWo6LUiQQpx55sa2kzc5vZxcahYzF4puMsyHG-yCscgXAZoEVbcp4SNpnyIVk0yevRP3F7BskuAfiRTCTESNzlngCV_P08q7bYohwjp-c6p-JISGVSqSGlPTBSShvKkJpSDrliq3s4OGTYpqGw_BwWS4SjIiqblpKO1tuFA0t7YMvtOUvg3XThF_D?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/j3MF3znSPMvAabI-fMhLz70F6hLo-8sAbmi-lV9FcZoxxBS96zTuHkCgRpWBN1H-_YUp9_tTLye5rEwHxYOhfKeAruaec2QO2s-D6NmmcDXtkOanTJAqJsZlBh0kCAzS7ouIqtQpPgesXHil5C-OB06gOiHr6N15Fse-pdBpmiCRVkcb6kevmYdAF-weKWLV?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/KR3doydp5DP-1tc4J_SCkM5hQY6xj7TZuSaSqGpI33StW4mm7z-RRhgaEKN1LJqQI-OneRaymk1hIf-dzzfdQuD7cMM_5oFYHZMEg3_uUSm1jE1qLF8DMmzzg_SQlnkqac5w43U_kdJUFBjQdYGAb2zpQ9wYRsProeMLi2uiHcFa0DnN5NG4u8mF4BcH6SMw?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/1o8UDqEz9VtwGEZO7FoEWEBMWXeV_Je7YqNkF3lMe4K2u0ltv5GYi83e8aIg4NOlrXSrjIDrAWvZA3w-dkW7A1bFPHp8fkWC_lt_RDoYQn3a9Z797F20ll2jVO-F4DHcx1JALUEfAtYWspx0FOeaMgp4HP6ZRsp7jmiiDjSRCiyS5USenIRiJTokqrdM5cgE?purpose=fullsize)

1. Tu envoies des tâches
2. Elles vont dans une **queue**
3. Des threads du pool les exécutent
4. Les threads sont **réutilisés**

---

## 🔹 Exemple simple (Java)

```java
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

ExecutorService pool = Executors.newFixedThreadPool(3);

for (int i = 0; i < 10; i++) {
    int taskId = i;
    pool.submit(() -> {
        System.out.println("Task " + taskId + " exécutée par " + Thread.currentThread().getName());
    });
}

pool.shutdown();
```

---

## 🔹 Types de thread pools

### 1. Fixed thread pool

```java
Executors.newFixedThreadPool(5);
```

👉 nombre de threads fixe

---

### 2. Cached thread pool

```java
Executors.newCachedThreadPool();
```

👉 crée des threads à la demande (⚠️ peut exploser)

---

### 3. Single thread

```java
Executors.newSingleThreadExecutor();
```

👉 un seul thread (ordre garanti)

---

### 4. Scheduled pool

```java
Executors.newScheduledThreadPool(2);
```

👉 pour tâches planifiées (cron-like)

---

## 🔹 Cas réel (backend)

Dans ton contexte (Spring / microservices / RabbitMQ) :

👉 thread pool utilisé pour :

* traiter des requêtes HTTP
* consommer des messages (RabbitMQ / Kafka)
* exécuter des tâches async (`@Async`)
* batch processing

Exemple Spring :

```java
@Async
public void process() {
    // exécuté dans un thread pool
}
```

---

## 🔹 Bonnes pratiques (niveau senior)

👉 Toujours configurer explicitement :

```java
new ThreadPoolExecutor(
    corePoolSize,
    maxPoolSize,
    keepAliveTime,
    TimeUnit.SECONDS,
    new LinkedBlockingQueue<>()
);
```

👉 Points clés :

* taille du pool adaptée (CPU vs I/O)
* queue limitée (éviter OOM)
* gestion du rejet (`RejectedExecutionHandler`)

---

## 🔹 Résumé (version entretien)

> Un thread pool est un ensemble de threads réutilisables qui exécutent des tâches de manière efficace.
> Il permet d’améliorer les performances, de limiter la consommation de ressources et de mieux gérer la concurrence.

