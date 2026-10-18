# Virtual Threads en Java

> **Objectif entretien :** comprendre les Virtual Threads, savoir expliquer leur intérêt, leur fonctionnement, leurs limites et être capable d’écrire du code simple avec eux.

Les **Virtual Threads** sont devenus une fonctionnalité finale du JDK avec **Java 21 via JEP 444**. Ce sont des instances de `java.lang.Thread`, mais contrairement aux threads classiques, ils sont gérés principalement par la JVM plutôt que directement associés pendant toute leur durée de vie à un thread du système d’exploitation.

---

# 1. Le problème avec les threads classiques

Traditionnellement, Java utilise des **Platform Threads**.

```java
Thread thread = new Thread(() -> {
    System.out.println("Hello");
});

thread.start();
```

Un `Platform Thread` est essentiellement associé à un **thread du système d'exploitation**.

```text
Java Platform Thread
        │
        ▼
     OS Thread
        │
        ▼
       CPU
```

Les threads système sont relativement coûteux en ressources. Une application ne peut donc pas raisonnablement créer un thread système pour chaque petite tâche lorsque le nombre de tâches devient très important.

C'est historiquement pour cette raison qu'on utilise des **thread pools**.

```java
ExecutorService executor =
        Executors.newFixedThreadPool(10);
```

Par exemple :

```text
1000 requêtes HTTP
       │
       ▼
  Thread Pool
   10 threads
       │
       ▼
      CPU
```

Seulement 10 tâches peuvent alors utiliser simultanément les 10 workers.

---

# 2. Le problème des opérations bloquantes

Prenons :

```java
public User loadUser(long id) {
    return repository.findById(id);
}
```

Le thread peut passer beaucoup de temps à attendre :

```text
Thread
  │
  ├── exécute du code
  │
  ├── appel DB
  │
  ├──────── attend la DB ────────┐
  │                              │
  └── reprend ◄──────────────────┘
```

Pendant cette attente, un Platform Thread reste une ressource coûteuse.

C'est particulièrement problématique pour les applications réalisant beaucoup d'I/O :

- appels SQL ;
- appels HTTP ;
- fichiers ;
- sockets ;
- microservices ;
- APIs REST.

Les Virtual Threads sont justement conçus pour des tâches qui passent beaucoup de temps **bloquées en attente d'I/O**.

---

# 3. Qu'est-ce qu'un Virtual Thread ?

Un Virtual Thread est un thread Java léger géré par la JVM.

```text
Virtual Thread 1 ─┐
Virtual Thread 2 ─┤
Virtual Thread 3 ─┤
Virtual Thread 4 ─┤
Virtual Thread 5 ─┤
       ...        │
                  ▼
          Platform Threads
             │ │ │ │
             ▼ ▼ ▼ ▼
             OS Threads
```

La JVM peut avoir un très grand nombre de Virtual Threads et les exécuter sur un nombre beaucoup plus faible de Platform Threads. Oracle indique qu'une JVM peut supporter un très grand nombre de Virtual Threads, potentiellement des millions selon la charge et les ressources disponibles.

Le Platform Thread qui exécute temporairement un Virtual Thread est souvent appelé :

> **Carrier Thread**

On peut donc visualiser :

```text
Virtual Thread
      │
      ▼
Carrier Thread
      │
      ▼
 OS Thread
      │
      ▼
     CPU
```

---

# 4. Le principe fondamental : mount / unmount

Supposons :

```java
var user = repository.findById(id);
```

Si cette opération bloque sur une I/O, la JVM peut suspendre le Virtual Thread.

```text
Virtual Thread A
      │
      ▼
Carrier Thread 1
      │
      ▼
appel DB
      │
      ▼
 attente
```

Au lieu de garder inutilement le carrier :

```text
Virtual Thread A
      │
      X  unmount
```

Le Carrier Thread peut être utilisé pour un autre Virtual Thread :

```text
Carrier Thread 1
      │
      ▼
Virtual Thread B
```

Lorsque l'I/O du Virtual Thread A est terminée :

```text
Virtual Thread A
      │
      ▼
Carrier disponible
      │
      ▼
continue son exécution
```

Oracle décrit ce comportement : lorsqu'un Virtual Thread rencontre certaines opérations bloquantes, le runtime peut suspendre le Virtual Thread et libérer le thread OS sous-jacent pour exécuter d'autres Virtual Threads.

---

# 5. Virtual Threads ≠ threads plus rapides

C'est probablement **la question la plus importante en entretien**.

Les Virtual Threads ne rendent pas une opération individuelle plus rapide.

```java
heavyCpuCalculation();
```

Si cette méthode prend :

```text
500 ms
```

elle ne va pas soudainement prendre :

```text
50 ms
```

avec un Virtual Thread.

Oracle insiste sur ce point :

> Virtual Threads = **scale / throughput**, pas amélioration directe de la vitesse ou de la latence d'une tâche.

Ils permettent surtout d'avoir :

```text
beaucoup plus de tâches concurrentes
```

notamment lorsque ces tâches passent beaucoup de temps à attendre.

---

# 6. CPU-bound vs I/O-bound

Il faut absolument connaître cette distinction.

## I/O-bound

Exemple :

```java
var user = database.findUser(id);

var account = bankApi.getAccount(user.id());

Files.readString(path);
```

La majorité du temps est passée à attendre.

```text
CPU : ██░░░░░░░░░░░░
I/O :   █████████████
```

### Excellent candidat pour les Virtual Threads

```text
Virtual Threads ✅
```

---

## CPU-bound

Exemple :

```java
for (long i = 0; i < 10_000_000_000L; i++) {
    calculate(i);
}
```

Ici :

```text
CPU : ███████████████
I/O :
```

Le CPU est occupé en permanence.

Les Virtual Threads ne permettent pas de créer davantage de cœurs CPU. Ils ne sont donc pas destinés aux opérations CPU intensives de longue durée.

Pour du calcul CPU parallèle, on continue généralement à raisonner selon :

```text
nombre de CPU / nombre de cores
```

avec par exemple :

```java
Executors.newFixedThreadPool(
    Runtime.getRuntime().availableProcessors()
);
```

ou des mécanismes comme :

```java
ForkJoinPool
```

selon le problème.

---

# 7. Créer un Virtual Thread

Depuis Java 21 :

```java
Thread thread = Thread.ofVirtual()
        .start(() -> {
            System.out.println("Hello Virtual Thread");
        });

thread.join();
```

`Thread.ofVirtual()` fournit un builder permettant de créer des Virtual Threads.

---

# 8. Donner un nom au Virtual Thread

```java
Thread thread = Thread.ofVirtual()
        .name("payment-thread")
        .start(() -> {
            System.out.println(
                Thread.currentThread()
            );
        });
```

On peut aussi générer plusieurs noms :

```java
var factory = Thread.ofVirtual()
        .name("worker-", 0)
        .factory();

Thread t1 = factory.newThread(() -> process());
Thread t2 = factory.newThread(() -> process());
```

On obtient par exemple :

```text
worker-0
worker-1
```

---

# 9. Vérifier si un thread est virtuel

```java
Thread thread = Thread.ofVirtual()
        .start(() -> {

            System.out.println(
                Thread.currentThread().isVirtual()
            );

        });
```

Résultat :

```text
true
```

Pour un Platform Thread :

```java
Thread thread = Thread.ofPlatform()
        .start(() -> {

            System.out.println(
                Thread.currentThread().isVirtual()
            );

        });
```

Résultat :

```text
false
```

---

# 10. La méthode recommandée avec ExecutorService

Très souvent, on utilisera :

```java
Executors.newVirtualThreadPerTaskExecutor()
```

Exemple :

```java
try (ExecutorService executor =
         Executors.newVirtualThreadPerTaskExecutor()) {

    Future<String> future = executor.submit(() -> {

        Thread.sleep(1000);

        return "Hello";
    });

    System.out.println(future.get());
}
```

Avec cet executor :

```text
1 tâche
   ↓
1 Virtual Thread

1 tâche
   ↓
1 Virtual Thread

1 tâche
   ↓
1 Virtual Thread
```

Chaque tâche soumise reçoit son propre Virtual Thread. Cet executor n'est donc pas un pool classique de quelques Virtual Threads réutilisés.

---

# 11. Le changement de philosophie

Avec les Platform Threads :

```java
Executors.newFixedThreadPool(20);
```

on pense :

```text
20 threads
    +
10000 tâches
```

Les tâches doivent attendre qu'un thread devienne disponible.

Avec les Virtual Threads :

```java
Executors.newVirtualThreadPerTaskExecutor();
```

la philosophie devient :

```text
1 tâche
=
1 Virtual Thread
```

Le guide Oracle recommande précisément de représenter chaque tâche concurrente par un Virtual Thread plutôt que de créer un pool de Virtual Threads.

---

# 12. Ne pas créer de pool de Virtual Threads

Ceci va à l'encontre du modèle :

```java
ExecutorService executor =
    Executors.newFixedThreadPool(
        10,
        Thread.ofVirtual().factory()
    );
```

Pourquoi limiter artificiellement le nombre de Virtual Threads à 10 ?

Leur principal intérêt est justement qu'ils sont beaucoup moins coûteux que les Platform Threads.

On préfère :

```java
ExecutorService executor =
    Executors.newVirtualThreadPerTaskExecutor();
```

Oracle recommande explicitement de **ne pas pooler les Virtual Threads**.

---

# 13. Exemple concret : appels HTTP

Imaginons :

```java
String getUser() {
    return callUserService();
}

String getOrders() {
    return callOrderService();
}
```

Les deux appels prennent environ 500 ms.

Version séquentielle :

```java
String user = getUser();

String orders = getOrders();
```

Temps approximatif :

```text
500 ms
+
500 ms
=
1000 ms
```

On peut les lancer concurremment :

```java
try (var executor =
         Executors.newVirtualThreadPerTaskExecutor()) {

    Future<String> user =
        executor.submit(this::getUser);

    Future<String> orders =
        executor.submit(this::getOrders);

    System.out.println(user.get());
    System.out.println(orders.get());
}
```

Conceptuellement :

```text
             Request
                │
         ┌──────┴──────┐
         ▼             ▼
     User API       Order API
      500 ms          500 ms
         │             │
         └──────┬──────┘
                ▼
             Result
```

Si les appels sont réellement indépendants, leur temps d'attente peut se chevaucher.

---

# 14. Exemple avec JDBC

```java
public User findUser(long id) {

    return jdbcTemplate.queryForObject(
        "SELECT * FROM users WHERE id = ?",
        mapper,
        id
    );
}
```

Avec des Virtual Threads, le code peut rester :

```text
simple
+
synchrone
+
bloquant
```

Il n'est pas nécessaire de transformer automatiquement tout le code en programmation réactive pour bénéficier d'une forte concurrence sur des workloads adaptés.

C'est d'ailleurs un des objectifs des Virtual Threads : permettre de conserver le modèle simple **thread-per-request** avec des APIs bloquantes lorsque ce modèle est adapté.

---

# 15. Exemple : traitement de 10 000 tâches

```java
try (var executor =
         Executors.newVirtualThreadPerTaskExecutor()) {

    for (int i = 0; i < 10_000; i++) {

        int id = i;

        executor.submit(() -> {

            Thread.sleep(1000);

            System.out.println(
                "Task " + id
            );

            return null;
        });
    }
}
```

Créer 10 000 Platform Threads serait beaucoup plus coûteux.

Avec les Virtual Threads, ce type de charge composée principalement d'attentes est précisément le scénario visé.

---

# 16. Pourquoi `Thread.sleep()` ne bloque pas forcément un carrier

Avec un Virtual Thread :

```java
Thread.sleep(1000);
```

le Virtual Thread peut être suspendu.

Conceptuellement :

```text
VT-1
 │
 ▼
Carrier-1
 │
 ▼
sleep()
 │
 ▼
VT-1 suspendu
```

Le Carrier Thread peut alors exécuter autre chose :

```text
Carrier-1
   │
   ▼
 VT-2
```

C'est très différent de l'idée historique selon laquelle :

```text
1 thread bloqué
=
1 OS thread immobilisé
```

pour toute la durée de l'attente.

---

# 17. Platform Thread vs Virtual Thread

| Critère | Platform Thread | Virtual Thread |
|---|---|---|
| Gestion principale | OS/JVM | JVM |
| Lié durablement à un OS Thread | Oui, typiquement | Non |
| Coût | Relativement élevé | Beaucoup plus léger |
| Quantité envisageable | Limitée | Très grande |
| Pool généralement utile | Oui | Non |
| I/O bloquante massive | Peut mal scaler | Excellent cas d'utilisation |
| CPU intensif | Adapté | Pas d'avantage particulier |
| API | `Thread` | `Thread` |
| Introduit comme fonctionnalité finale | historique | Java 21 |

Les Virtual Threads restent des objets `Thread`, ce qui permet notamment d'utiliser les mécanismes classiques tels que les exceptions, les stack traces et le debugging avec un modèle familier.

---

# 18. Virtual Threads vs CompletableFuture

Avant les Virtual Threads, on pouvait écrire :

```java
CompletableFuture<String> user =
    CompletableFuture.supplyAsync(
        this::loadUser
    );

CompletableFuture<String> account =
    CompletableFuture.supplyAsync(
        this::loadAccount
    );

return user.thenCombine(
        account,
        (u, a) -> u + a
);
```

Avec les Virtual Threads :

```java
try (var executor =
         Executors.newVirtualThreadPerTaskExecutor()) {

    Future<String> user =
        executor.submit(this::loadUser);

    Future<String> account =
        executor.submit(this::loadAccount);

    return user.get() + account.get();
}
```

Les Virtual Threads permettent souvent de revenir à un code plus proche du style impératif classique.

Oracle montre d'ailleurs que les applications peuvent bénéficier des Virtual Threads tout en utilisant des APIs bloquantes simples plutôt que d'être obligées de transformer chaque opération en pipeline asynchrone.

Mais :

```text
Virtual Threads
≠
remplacement total de CompletableFuture
```

`CompletableFuture` reste utile lorsque l'on veut exprimer explicitement :

```java
thenApply(...)
thenCompose(...)
thenCombine(...)
allOf(...)
exceptionally(...)
```

c'est-à-dire de la **composition asynchrone**.

---

# 19. Virtual Threads vs Reactive Programming

Exemple réactif :

```java
Mono<User> user =
    webClient.get()
        .uri("/users/1")
        .retrieve()
        .bodyToMono(User.class);
```

Une architecture réactive repose généralement sur :

```text
peu de threads
+
I/O non bloquantes
+
event loop
```

Alors que les Virtual Threads permettent :

```text
beaucoup de threads légers
+
code bloquant classique
+
1 tâche ≈ 1 thread
```

On peut donc retenir :

```text
Reactive
→ éviter le blocage

Virtual Threads
→ rendre le blocage beaucoup moins coûteux
```

Ce sont deux modèles différents. Oracle déconseille également de supposer qu'un framework entièrement asynchrone tirera automatiquement un bénéfice important de l'ajout de Virtual Threads.

---

# 20. Très important : Virtual Threads et `synchronized`

C'est une question d'entretien intéressante parce que la réponse dépend de la **version Java**.

### Java 21

Dans l'implémentation initiale de Java 21, effectuer une opération bloquante à l'intérieur d'un :

```java
synchronized
```

pouvait empêcher le Virtual Thread de libérer son Carrier Thread.

Cela s'appelle :

```text
Pinning
```

Exemple conceptuel :

```java
synchronized (lock) {

    callRemoteService();
}
```

Si `callRemoteService()` bloque longtemps :

```text
Virtual Thread
       │
       ▼
Carrier Thread
       │
       X
  reste occupé
```

La documentation Java 21 signalait donc ce cas comme une limite pouvant réduire la scalabilité.

---

# 21. Mais attention : changement avec Java 24

Cette réponse est importante pour montrer que tu maîtrises les versions récentes.

Avec **JDK 24**, le **JEP 491 — Synchronize Virtual Threads without Pinning** a modifié l'implémentation afin que les Virtual Threads puissent généralement se détacher de leur carrier même lorsqu'ils sont dans des régions `synchronized`.

Donc en entretien :

```text
Java 21 :
synchronized + opération bloquante
→ risque important de pinning

Java 24+ :
ce problème lié aux monitors synchronized
a été largement supprimé grâce au JEP 491
```

Il reste cependant certaines situations susceptibles de provoquer du pinning, notamment l'exécution de certains appels natifs.

---

# 22. Limiter une ressource : utiliser Semaphore

Un piège classique serait :

> « J'ai trop de Virtual Threads, donc je vais créer un pool de 10 Virtual Threads. »

Non.

Supposons qu'une API externe accepte seulement :

```text
10 requêtes simultanées
```

On utilise plutôt :

```java
private final Semaphore semaphore =
    new Semaphore(10);

public String callExternalApi()
        throws InterruptedException {

    semaphore.acquire();

    try {
        return externalApi.call();
    } finally {
        semaphore.release();
    }
}
```

Le pool n'est pas utilisé pour limiter les Virtual Threads eux-mêmes.

Le `Semaphore` limite la **ressource externe** :

```text
1000 Virtual Threads

        │
        ▼

Semaphore(10)

        │
        ▼

maximum 10 appels simultanés
```

C'est explicitement le modèle recommandé dans le guide d'adoption d'Oracle.

---

# 23. Exemple avec un pool de connexions DB

Supposons :

```text
10000 Virtual Threads
```

mais HikariCP possède :

```text
20 connexions
```

Tu ne feras toujours au maximum qu'environ :

```text
20 opérations DB utilisant simultanément une connexion
```

Les autres Virtual Threads attendront une connexion.

C'est normal.

Le pool de connexions reste nécessaire parce qu'une connexion DB est une **ressource limitée**. Oracle précise qu'un pool de connexions agit déjà comme un mécanisme de limitation de concurrence ; il n'est donc généralement pas nécessaire d'ajouter en plus un `Semaphore` pour limiter exactement la même ressource.

---

# 24. ThreadLocal et Virtual Threads

Les Virtual Threads supportent :

```java
ThreadLocal
```

mais il faut faire attention.

Exemple acceptable conceptuellement :

```java
ThreadLocal<String> requestId;
```

pour stocker un petit contexte associé à une requête.

En revanche, ce type de pattern peut devenir mauvais :

```java
ThreadLocal<ExpensiveObject> cache =
    ThreadLocal.withInitial(
        ExpensiveObject::new
    );
```

Pourquoi ?

Avec un pool classique :

```text
20 threads
→ environ 20 objets
```

Avec énormément de Virtual Threads :

```text
100 000 Virtual Threads
→ potentiellement énormément d'objets
```

Oracle déconseille donc notamment l'utilisation de `ThreadLocal` pour mettre en cache des objets lourds supposés être réutilisés par un petit pool de threads.

---

# 25. Virtual Threads avec Spring Boot

Spring Boot supporte directement les Virtual Threads avec Java 21+.

Dans :

```properties
application.properties
```

on peut activer :

```properties
spring.threads.virtual.enabled=true
```

La documentation actuelle de Spring Boot indique que cette option utilise les Virtual Threads pour plusieurs mécanismes auto-configurés d'exécution de tâches ; les propriétés classiques de dimensionnement du thread pool n'ont alors plus le même rôle, puisque l'approche n'est plus basée sur un pool traditionnel de workers.

Exemple :

```properties
spring.threads.virtual.enabled=true
```

Puis ton controller peut rester extrêmement classique :

```java
@RestController
@RequestMapping("/users")
public class UserController {

    private final UserService userService;

    public UserController(
        UserService userService
    ) {
        this.userService = userService;
    }

    @GetMapping("/{id}")
    public User getUser(
            @PathVariable long id) {

        return userService.findById(id);
    }
}
```

Et :

```java
@Service
public class UserService {

    private final UserRepository repository;

    public User findById(long id) {

        return repository
            .findById(id)
            .orElseThrow();
    }
}
```

Cela permet de conserver un modèle Spring MVC/JPA/JDBC très naturel lorsqu'il convient à l'application.

La documentation Spring Boot actuelle recommande Java 21 minimum pour les Virtual Threads et indique que **Java 24 ou plus récent offre une meilleure expérience**, notamment grâce aux améliorations autour du pinning.

---

# 26. Exemple microservices

Imaginons un service :

```text
Payment Service
```

qui doit appeler :

```text
Customer Service
Account Service
Fraud Service
```

On peut faire :

```java
public PaymentContext loadContext(
        long customerId) throws Exception {

    try (var executor =
             Executors.newVirtualThreadPerTaskExecutor()) {

        Future<Customer> customer =
            executor.submit(
                () -> customerClient.get(customerId)
            );

        Future<Account> account =
            executor.submit(
                () -> accountClient.get(customerId)
            );

        Future<FraudScore> fraud =
            executor.submit(
                () -> fraudClient.check(customerId)
            );

        return new PaymentContext(
            customer.get(),
            account.get(),
            fraud.get()
        );
    }
}
```

Architecture :

```text
                  Payment Service
                        │
            ┌───────────┼───────────┐
            │           │           │
            ▼           ▼           ▼
       Customer      Account      Fraud
       Service       Service      Service
```

Chaque appel peut être exécuté dans son propre Virtual Thread.

C'est un scénario particulièrement intéressant dans des architectures microservices composées de nombreux appels réseau bloquants indépendants.

---

# 27. Exception et Future

Avec :

```java
Future<String> future =
    executor.submit(() -> {
        throw new RuntimeException("Boom");
    });
```

Puis :

```java
future.get();
```

on récupère typiquement :

```java
ExecutionException
```

qui encapsule l'exception produite dans la tâche.

Donc les règles classiques de :

```java
Future
ExecutorService
InterruptedException
ExecutionException
```

restent pertinentes.

---

# 28. Virtual Threads et interruption

Un Virtual Thread reste un `Thread`.

On peut donc l'interrompre :

```java
Thread thread =
    Thread.ofVirtual().start(() -> {

        try {
            Thread.sleep(10_000);
        } catch (InterruptedException e) {

            Thread.currentThread()
                .interrupt();
        }

    });

thread.interrupt();
```

Les bonnes pratiques d'interruption Java restent donc importantes.

---

# 29. Monitoring et debugging

Les Virtual Threads restent observables comme des threads Java.

Oracle fournit notamment du support via :

```text
JDK Flight Recorder
jcmd
thread dumps
```

et `jcmd` peut produire un dump contenant les Platform Threads et les Virtual Threads.

Exemple :

```bash
jcmd <PID> Thread.dump_to_file \
    -format=json threads.json
```

---

# 30. Ce qu'il ne faut PAS faire

### ❌ Utiliser les Virtual Threads pour accélérer du CPU pur

```java
calculatePrimeNumbers();
```

Ce n'est pas leur objectif.

---

### ❌ Créer un pool de Virtual Threads

```java
newFixedThreadPool(
    100,
    Thread.ofVirtual().factory()
);
```

On préfère généralement :

```java
Executors
    .newVirtualThreadPerTaskExecutor();
```

---

### ❌ Penser que « Virtual Thread = plus rapide »

Non :

```text
latency ≠ forcément meilleure
```

Le principal objectif est :

```text
scalability
+
throughput
```

sur les workloads fortement concurrents et I/O-bound.

---

### ❌ Supprimer les pools de connexions DB

Virtual Threads :

```text
ressource logicielle légère
```

Connexion DB :

```text
ressource externe limitée
```

Les deux problèmes sont différents.

---

### ❌ Utiliser un ThreadLocal lourd par Virtual Thread

Cela peut conduire à une consommation mémoire importante si le nombre de Virtual Threads est très élevé.

---

# 31. Question d'entretien : « Qu'est-ce qu'un Virtual Thread ? »

### Réponse courte

> Un Virtual Thread est un thread Java léger géré par la JVM. Contrairement à un Platform Thread, il n'est pas associé en permanence à un thread système. Lorsqu'il bloque sur certaines opérations I/O, la JVM peut le suspendre et utiliser le carrier thread pour exécuter un autre Virtual Thread. Cela permet de gérer beaucoup plus de tâches concurrentes, particulièrement dans les applications I/O-bound.

---

# 32. « Quel est l'intérêt principal des Virtual Threads ? »

> Le principal intérêt est d'améliorer la scalabilité et le throughput des applications ayant beaucoup de tâches concurrentes qui passent du temps à attendre des I/O, comme les appels HTTP, JDBC ou les sockets. Ils permettent de conserver un modèle de programmation synchrone simple sans immobiliser un Platform Thread pour chaque tâche en attente.

---

# 33. « Est-ce qu'un Virtual Thread est plus rapide ? »

> Non. Un Virtual Thread n'exécute pas le code CPU plus rapidement. Son intérêt est qu'il est beaucoup plus léger et peut être suspendu lorsqu'il attend certaines I/O, ce qui permet à la JVM d'utiliser efficacement un nombre limité de Platform Threads pour un très grand nombre de tâches.

---

# 34. « Quand utiliser les Virtual Threads ? »

Très bon cas :

```text
REST API
     ↓
appel DB
     ↓
appel microservice
     ↓
appel externe
```

Donc :

```text
I/O intensive
+
beaucoup de requêtes concurrentes
```

Exemples :

```text
microservices
serveurs HTTP
JDBC
appels REST
sockets
fichiers
```

---

# 35. « Quand ne sont-ils pas particulièrement utiles ? »

> Pour des tâches longues et principalement CPU-bound. Dans ce cas, la limite reste le nombre de cœurs CPU et créer énormément de Virtual Threads ne donne pas davantage de puissance de calcul.

---

# 36. « Quelle est la différence entre Platform Thread et Virtual Thread ? »

Réponse orale :

> Un Platform Thread est généralement associé à un thread du système d'exploitation pendant sa durée de vie, ce qui en fait une ressource relativement coûteuse. Un Virtual Thread est géré par la JVM et peut être monté puis démonté d'un Platform Thread. On peut donc avoir beaucoup plus de Virtual Threads que de Platform Threads.

---

# 37. « C'est quoi un Carrier Thread ? »

> Le Carrier Thread est le Platform Thread utilisé temporairement pour exécuter un Virtual Thread.

```text
Virtual Thread
      ↓
Carrier Thread
      ↓
OS Thread
```

---

# 38. « Que se passe-t-il lorsqu'un Virtual Thread fait une I/O bloquante ? »

Réponse :

> Lorsque l'opération est supportée par le runtime, la JVM peut suspendre le Virtual Thread et libérer son carrier thread. Celui-ci peut alors exécuter un autre Virtual Thread. Lorsque l'I/O est terminée, le premier Virtual Thread peut être planifié de nouveau.

---

# 39. « Faut-il créer un pool de Virtual Threads ? »

> Non, généralement non. Contrairement aux Platform Threads, les Virtual Threads sont suffisamment légers pour représenter directement les tâches. Avec `Executors.newVirtualThreadPerTaskExecutor()`, chaque tâche reçoit son propre Virtual Thread.

---

# 40. « Comment limiter les appels vers un service externe ? »

Pas :

```java
newFixedThreadPool(10)
```

juste pour limiter le nombre de Virtual Threads.

Mais par exemple :

```java
Semaphore semaphore =
    new Semaphore(10);
```

Puis :

```java
semaphore.acquire();

try {
    callService();
} finally {
    semaphore.release();
}
```

---

# 41. « Virtual Threads ou CompletableFuture ? »

Bonne réponse :

> Ils ne répondent pas exactement au même problème. Les Virtual Threads permettent d'exécuter beaucoup de tâches concurrentes avec du code bloquant et impératif simple. `CompletableFuture` fournit surtout un modèle de composition asynchrone avec des opérations comme `thenApply`, `thenCompose` ou `thenCombine`. Les Virtual Threads peuvent réduire le besoin de code asynchrone complexe dans certains cas, mais ils ne remplacent pas toutes les utilisations de `CompletableFuture`.

---

# 42. « Virtual Threads ou WebFlux ? »

Réponse possible :

> WebFlux repose principalement sur un modèle réactif avec des opérations non bloquantes et peu de threads. Les Virtual Threads permettent au contraire de conserver un modèle synchrone et bloquant avec énormément de threads légers. Pour une application Spring MVC/JDBC classique très I/O-bound, les Virtual Threads peuvent être une solution beaucoup plus simple à adopter.

---

# 43. « C'est quoi le pinning ? »

Réponse complète :

> Le pinning signifie qu'un Virtual Thread ne peut momentanément pas se détacher de son Carrier Thread alors qu'il est bloqué. C'était notamment un problème avec les blocs `synchronized` dans Java 21. JDK 24 a fortement amélioré cette situation avec le JEP 491, qui permet aux Virtual Threads de se détacher lors de blocages dans des régions `synchronized`. Certains appels natifs peuvent encore entraîner du pinning.

C'est une **très bonne réponse d'entretien** parce qu'elle montre que tu connais la différence entre Java 21 et les versions plus récentes.

---

# 44. Exemple complet à retenir

```java
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class VirtualThreadExample {

    public static void main(String[] args)
            throws Exception {

        try (ExecutorService executor =
                 Executors
                     .newVirtualThreadPerTaskExecutor()) {

            Future<String> user =
                executor.submit(() -> {
                    Thread.sleep(500);
                    return "User";
                });

            Future<String> account =
                executor.submit(() -> {
                    Thread.sleep(500);
                    return "Account";
                });

            System.out.println(user.get());
            System.out.println(account.get());
        }
    }
}
```

À retenir :

```text
2 tâches
   ↓
2 Virtual Threads
   ↓
les deux peuvent attendre simultanément
   ↓
les Carrier Threads restent disponibles
```

---

# 45. Schéma mental à retenir

```text
                  Application
                       │
                       ▼

       ┌─────────────────────────────┐
       │       Virtual Threads       │
       │                             │
       │ VT1 VT2 VT3 VT4 ... VT10000│
       └──────────────┬──────────────┘
                      │
                      │ JVM Scheduler
                      ▼
            ┌───────────────────┐
            │ Platform Threads  │
            │ P1 P2 P3 P4 ...   │
            └─────────┬─────────┘
                      │
                      ▼
                OS Threads
                      │
                      ▼
                    CPU
```

Quand :

```text
VT1 → attend DB
```

la JVM peut faire :

```text
VT1
 ↓
suspendu

P1
 ↓
VT200
```

C'est cette réutilisation efficace des Platform Threads qui permet de scaler sur de nombreuses opérations bloquantes.

---

# 46. Les 10 points à connaître absolument pour l'entretien

1. Virtual Threads sont finalisés depuis **Java 21 / JEP 444**.
2. Un Virtual Thread reste un `java.lang.Thread`.
3. Il est géré principalement par la JVM.
4. Il est exécuté temporairement sur un **Carrier Thread**.
5. Lors de nombreuses opérations bloquantes, il peut être suspendu sans immobiliser le carrier.
6. Ils sont particulièrement adaptés aux applications **I/O-bound**.
7. Ils n'accélèrent pas directement les traitements **CPU-bound**.
8. On préfère généralement **un Virtual Thread par tâche**, pas un pool de Virtual Threads.
9. Pour limiter l'accès à une ressource externe, utiliser par exemple `Semaphore`, un connection pool ou un autre mécanisme de back-pressure adapté.
10. Avec **Java 24**, JEP 491 a fortement réduit le problème historique de pinning lié à `synchronized`.

---

# 47. Réponse de 30 secondes à mémoriser

> Les Virtual Threads ont été introduits comme fonctionnalité finale en Java 21 avec Project Loom. Ce sont des threads légers gérés par la JVM. Contrairement aux Platform Threads, ils ne sont pas attachés en permanence à un thread système. Lorsqu'un Virtual Thread attend une opération I/O, la JVM peut généralement le suspendre et utiliser le Carrier Thread pour exécuter une autre tâche. Ils permettent donc d'avoir énormément de tâches concurrentes avec un modèle de programmation synchrone simple. Leur principal intérêt est le throughput pour les applications I/O-bound, pas l'accélération des traitements CPU.

---

# 48. Questions flash d'entretien

### 1. Depuis quelle version les Virtual Threads sont-ils finalisés ?

```text
Java 21
```

JEP :

```text
JEP 444
```

---

### 2. Virtual Thread est-il un `Thread` ?

```text
Oui.
```

```java
java.lang.Thread
```

---

### 3. API principale ?

```java
Thread.ofVirtual()
```

ou :

```java
Executors.newVirtualThreadPerTaskExecutor()
```

---

### 4. Virtual Threads pour CPU-bound ?

```text
Pas particulièrement.
```

---

### 5. Virtual Threads pour JDBC ?

```text
Oui, très bon cas potentiel car JDBC est généralement bloquant.
```

---

### 6. Virtual Threads pour appels REST bloquants ?

```text
Oui.
```

---

### 7. Faut-il pooler les Virtual Threads ?

```text
Non, généralement.
```

---

### 8. Comment limiter 10 appels simultanés ?

```java
new Semaphore(10);
```

---

### 9. Les Virtual Threads améliorent la latence ?

```text
Pas nécessairement.
```

Ils visent surtout :

```text
throughput
+
scalabilité
```

---

### 10. Que signifie pinning ?

```text
Un Virtual Thread reste attaché
à son Carrier Thread pendant
une opération bloquante.
```

---

### 11. `synchronized` provoque-t-il toujours du pinning ?

```text
Java 21 :
cela pouvait être un problème important.

Java 24+ :
JEP 491 a supprimé ce problème
pour la plupart des cas liés
aux monitors synchronized.
```

---

# 49. Question piège

### Interviewer

> Nous avons remplacé notre pool de 200 threads par 100 000 Virtual Threads. Est-ce que notre base de données peut maintenant traiter 100 000 requêtes simultanément ?

### Réponse

> Non. Les Virtual Threads éliminent principalement la limitation liée au coût des threads applicatifs. Ils ne suppriment pas les limites des ressources externes. Si le connection pool contient 20 connexions, seules environ 20 opérations pourront utiliser simultanément une connexion DB ; les autres Virtual Threads attendront une connexion disponible.

C'est exactement la distinction à avoir en tête :

```text
Virtual Thread
=
ressource d'exécution

DB Connection
=
ressource externe limitée
```

---

# 50. Résumé final

```text
Platform Threads

Task
 ↓
Platform Thread
 ↓
OS Thread
 ↓
CPU

→ relativement coûteux
→ nombre limité
→ pools nécessaires
```

contre :

```text
Virtual Threads

Task
 ↓
Virtual Thread
 ↓
Carrier Thread
 ↓
OS Thread
 ↓
CPU

→ léger
→ très grand nombre possible
→ un thread par tâche
→ idéal pour beaucoup d'I/O
```

La phrase essentielle à retenir est :

> **Virtual Threads allow us to keep the simple thread-per-request programming model while scaling to a very large number of concurrent I/O-bound tasks.**

Et en français :

> **Les Virtual Threads permettent de conserver un modèle simple avec un thread par tâche tout en supportant un très grand nombre de tâches concurrentes, particulièrement lorsqu'elles passent beaucoup de temps en attente d'I/O.**