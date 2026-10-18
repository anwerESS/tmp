Le **cycle de vie d’un thread en Java** décrit les différents états par lesquels passe un thread, depuis sa création
jusqu’à sa fin.

---

## 🔹 Vue globale des états

![Image](https://images.openai.com/static-rsc-4/TvrV-Okrxm_H4VgwVOesfvPHqDxOkS3eKSNot4d4MF7-sRYRyTaIq-ofFTL4g6QCV-6WQ37rJlKWLpLfnzbDnhVsQJOJbqwoyJhuuvuXYZ17fuQ_Lq76bRa8YhbQA8O3zsltCWfOitwwDm3M7KAVblyScekpG4iNrFCfY9wy6RmwioJOLzxcGWysm2wnjoFD?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/lOwQPTuODM732xtac6uHL1Gu1FhDskQse_0aPAIhLdzvjWsMvk8F0u_9NaPA9lOLSxlGDmX7sEUbrF0F0TM6bxk87J4V5MCeD4CZ3ADpoqJf0Ay-XgNUvhIWwmVtq27OcjTNqcvqm3BPURiHN8uNFO8VEbQJnBd7EM-A-UnYEXcvSziVJ2S_28K9ndrHA_UG?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/db1VDCI5NPAte7CCB3KvUjoHGsJEnPj0NmGspkhazVvpozFa7i0pY_3lEQ6sJqWpCXsCZRJVF8YW_ElOPq3ugKLF22QKQ91yOkp3haEuxCxxBLkrhoc9nV_sNhtGZ6TCc7dkdakazTN1mFFTY96h8gBZ015kThZfDwVQ3j8CqL6HIDvsNnwhluYxzQSmpdLo?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/CEs2aGfYnpSPqe0Digp7VselbDrrQb1lqMIPOGfFGtm6CcU7RSQrb3bFe643U_i1XZFQL__MhCG5vvjkvht6B5ma12l10Am6Of0IAWpvycU6M7wbzrpc0OnrkVDE8W7I55eA4E2DhcqKMOXmSMAiN9UWeg9u-v9sQdKuNnfp4YUjI1yYSff4gERgvg3Vzzjb?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/e0ypbmZvx2XmKWGRbbZrlm5iUp3vJc6O1wM8S96rsVzrXTwVEqcwgnuLI6_VAcJMRuULA9bWSsUfd_z1jEI7LEswRoJiYM7wEQk-F7VziHAJhjqsnWVDxVZ0dat8RdDxGAKii_GDMkHC2GIie7IPwvQyuvOiKNYOuZbZInTE5quxhsXamonP0Ssxv-hW-L4X?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/fJv9TxwisWwCUFGqnX7yalsxKSu9EcXuJf3P-GVfV_WFBt4lmunRHjkpQ2E3XEambChlZFhWYjYHE5w7coLwPavh-Ir8Tq-_QwqmKNFetmPC3Mp7yp7SoewS9_JcNFkRSs4X6sOWzxVNuErtKeMZZgE8XUR8VcyxTGHuCFoSS_eHL4B08IHSyzuU2gOsTz23?purpose=fullsize)

---

## 🔹 Les états principaux (`Thread.State`)

### 1. **NEW**

👉 Le thread est créé mais pas encore lancé

```java
Thread t = new Thread();
```

---

### 2. **RUNNABLE**

👉 Le thread est prêt à s’exécuter (ou en cours d’exécution)

```java
t.start();
```

💡 En Java, **RUNNABLE = prêt OU en train de tourner** (pas séparé comme en OS)

---

### 3. **BLOCKED**

👉 Le thread attend un verrou (`synchronized`)

```java
synchronized(lock){
        // un autre thread détient déjà le lock
        }
```

---

### 4. **WAITING**

👉 Le thread attend indéfiniment une action

Exemples :

```java
obj.wait();
thread.

join();
```

---

### 5. **TIMED_WAITING**

👉 Le thread attend pendant un temps limité

```java
Thread.sleep(1000);
thread.

join(1000);
```

---

### 6. **TERMINATED**

👉 Le thread a fini son exécution

---

## 🔹 Exemple simple

```java
Thread t = new Thread(() -> {
    try {
        Thread.sleep(1000);
    } catch (Exception e) {
    }
});

System.out.

println(t.getState()); // NEW

        t.

start();
System.out.

println(t.getState()); // RUNNABLE

        t.

join();
System.out.

println(t.getState()); // TERMINATED
```

---

## 🔹 Transitions importantes

* `NEW → RUNNABLE` → `start()`
* `RUNNABLE → BLOCKED` → lock non disponible
* `RUNNABLE → WAITING` → `wait()` / `join()`
* `RUNNABLE → TIMED_WAITING` → `sleep()`
* `RUNNABLE → TERMINATED` → fin du `run()`

---

## 🔹 Résumé (version entretien)

> Un thread Java passe par plusieurs états : NEW, RUNNABLE, BLOCKED, WAITING, TIMED_WAITING et TERMINATED.
> Ces états permettent de comprendre son comportement, notamment en cas de synchronisation ou d’attente entre threads.

---

## 🔹 Petit + (niveau avancé entretien)

👉 En pratique backend (Spring / microservices) :

* on utilise rarement `Thread` directement
* plutôt :

    * `ExecutorService`
    * `CompletableFuture`
    * pools de threads

👉 mais **le lifecycle reste le même derrière**
