# Java Concurrency — Guide pratique pour le backend

Guide personnel de révision • 18 septembre 2026  
Français avec termes techniques en anglais • Exemples Java 21+ sauf indication contraire

Ce guide couvre les 12 sections de la formation de Michael Pogrebinsky, en privilégiant les usages backend Java/Spring Boot : API REST, workers, appels externes, traitements batch et performance. Les exemples sont des extraits pédagogiques ; les méthodes métier représentées par des noms comme `chargerProduit()` sont à adapter. Ce document n'est pas une transcription des vidéos.

## 1. Avis sur les deux formations

### La formation suivie : une bonne base technique

Mon avis est favorable, à partir du programme, des exercices partagés et de plusieurs sources Java du ZIP. Je n'ai pas visionné toutes les vidéos et je n'ai pas exécuté l'ensemble des projets.

Son principal intérêt est de construire un raisonnement correct : reconnaître une donnée partagée, identifier une opération composée, choisir le verrou et comprendre visibilité et ordre mémoire. Ces compétences restent nécessaires avec Spring et les virtual threads.

Pour un poste orienté performance comme FAST, les parties contention, deadlocks, atomic operations, latence et diagnostic sont particulièrement utiles. Pour une API Spring Boot, elles évitent aussi les bugs dans les beans singleton et les caches locaux.

En revanche, hériter de `Thread`, écrire une pile lock-free, multiplier des matrices ou manipuler les pixels n'est pas la priorité de tes futures missions backend. Il faut retenir le problème illustré, pas mémoriser toute l'implémentation.

La promesse « become an expert » est ambitieuse : suivre ce cours donne des bases solides ; l'expertise vient aussi des incidents analysés, des revues de code et des mesures en charge.

### La formation suivante : un complément cohérent

Le programme public de **Java 26: Virtual Threads, Concurrency Masterclass [Hands-On]**, de Vinoth Selvaraj, annonce notamment executors, `CompletableFuture`, contexte d'exécution, un projet Spring Boot et des essais JMeter. Cela correspond davantage aux tâches d'intégration backend que tu souhaites pratiquer. Mon évaluation porte sur ce programme public, pas sur la qualité de chaque vidéo. [Programme Udemy](https://www.udemy.com/course/java-virtual-thread/)

| Formation | Apport principal pour toi | Manière de l'exploiter |
|---|---|---|
| Première | Comprendre la correction et les mécanismes de concurrence | Réviser les bugs et expliquer pourquoi une solution fonctionne |
| Deuxième | Appliquer ces notions à des services backend | Coder le projet et mesurer une variante avec puis sans virtual threads |

Ton choix de les suivre dans cet ordre est pertinent. La deuxième ne remplace pas la première : créer beaucoup de threads légers ne corrige ni une race condition ni une base de données saturée.

Attention au numéro de version du titre : `ScopedValue` est une API standard depuis Java 25 ; `StructuredTaskScope` est encore **preview en Java 26**. Ne traiter aucune API preview comme un contrat stable de production. [ScopedValue](https://docs.oracle.com/en/java/javase/25/docs/api/java.base/java/lang/ScopedValue.html) · [StructuredTaskScope Java 26](https://docs.oracle.com/en/java/javase/26/docs/api/java.base/java/util/concurrent/StructuredTaskScope.html)

## 2. Parcours de lecture et priorités

| Sections du cours | Notions | Priorité backend |
|---|---|---|
| 1 | Process, thread, concurrency, parallelism | Comprendre le modèle, sans approfondir l'OS |
| 2–3 | Création, interruption, daemon, join | Maîtriser le cycle de vie ; préférer les executors en application |
| 4 | Latency, throughput, CPU-bound, I/O-bound | Essentiel |
| 5–6 | Shared state, atomicity, visibility, locks, deadlocks | Essentiel |
| 7 | ReentrantLock, ReadWriteLock | Savoir choisir et utiliser |
| 8 | Semaphore, conditions, coordination | Maîtriser Semaphore et les abstractions JDK ; savoir lire wait/notify |
| 9 | Atomics et CAS | Utiliser les atomics ; laisser les structures lock-free complexes en référence |
| 10–11 | Blocking, async, virtual threads | Essentiel pour les services backend |
| 12 | Limites d'une JVM, distributed systems | Essentiel pour les microservices |

Le guide ajoute executors, `CompletableFuture` et quelques pièges Spring : ce sont des compléments pratiques, pas des thèmes supposés couverts en détail par le premier cours.

## 3. Process, thread et travail concurrent

Un **process** est une instance d'application avec ses ressources. Une JVM s'exécute dans un process. Un **thread** est un flux d'exécution au sein de ce process.

- **Concurrency** : plusieurs tâches progressent sur une même période ; elles peuvent alterner sur un seul cœur.
- **Parallelism** : plusieurs tâches s'exécutent réellement au même instant.
- **Context switch** : changement de tâche exécutée ; sa gestion a un coût.

Un thread n'est pas attaché définitivement à un cœur. Le nombre de threads n'est pas la puissance de calcul disponible. Un conteneur Kubernetes peut aussi avoir un budget CPU bien inférieur aux ressources physiques du serveur.

**Exemple métier :** interroger prix et disponibilité en même temps relève de la concurrence. Répartir un calcul de risque indépendant sur plusieurs cœurs relève du parallélisme.

## 4. Création, arrêt et coordination de base

```java
Thread platform = new Thread(() -> System.out.println("travail"));
platform.start();
platform.join(); // Le thread appelant attend la fin.

Thread virtual = Thread.ofVirtual().start(() -> System.out.println("I/O"));
virtual.join();
```

`start()` démarre l'exécution concurrente ; `run()` appelé directement reste un appel ordinaire dans le thread courant. Un objet `Thread` ne se démarre qu'une fois.

### Interrupt : une demande d'arrêt coopérative

`interrupt()` ne tue pas arbitrairement le thread. Le code doit respecter l'interruption, ou une attente interruptible doit lever `InterruptedException`.

```java
public void runWorker() {
    try {
        while (!Thread.currentThread().isInterrupted()) {
            String job = queue.take(); // BlockingQueue<String> partagée
            process(job);
        }
    } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
        // Fin du worker : ne pas repartir dans la boucle.
    }
}
```

Si la méthode peut propager `InterruptedException`, c'est souvent le choix le plus simple. Si elle l'intercepte sans pouvoir la propager, restaurer le flag et terminer ou appliquer une politique explicite. Ne pas utiliser un catch vide. `Thread.interrupted()` consulte **et efface** le flag ; `isInterrupted()` le consulte sans l'effacer.

### Daemon et join

Un daemon ne maintient pas la JVM en vie. Cela ne signifie pas « moins prioritaire ». Les virtual threads sont toujours daemon ; `setDaemon(false)` sur eux est interdit.

`join()` attend une fin de thread ; il ne lance rien. Pour un résultat métier, `Future<T>` est généralement plus pratique. Un timeout de `join` ou de `get` ne garantit pas l'arrêt du travail.

## 5. Performance : identifier ce qui limite réellement

| Notion | Question à poser |
|---|---|
| Latency | Combien de temps dure une requête ? |
| Throughput | Combien de requêtes sont terminées par seconde ? |
| CPU-bound | Le traitement attend-il surtout du temps CPU ? |
| I/O-bound | Attend-il surtout le réseau, la base ou le stockage ? |
| Contention | Les threads attendent-ils le même verrou ou la même ressource ? |

Deux appels indépendants de 200 ms et 300 ms prennent environ 500 ms en séquence, et environ 300 ms plus les surcoûts en concurrence, si les services supportent la charge. Deux étapes dépendantes ne peuvent pas être parallélisées de cette manière.

La partie séquentielle limite le gain : avec 50 % du temps impossible à paralléliser, même énormément de cœurs ne donnera pas un facteur supérieur à 2 dans le modèle idéal d'Amdahl.

En régime stable, la loi de Little donne un ordre de grandeur : **requêtes en cours ≈ débit × durée moyenne**. À 100 requêtes/s et 0,5 s, environ 50 requêtes sont en cours en moyenne. Ce n'est pas une formule universelle pour dimensionner un pool.

En pratique : mesurer p50, p95, p99, erreurs, CPU, mémoire, GC, taille des files et occupation du pool JDBC. Utiliser `System.nanoTime()` pour mesurer une durée locale ; un test de quelques appels ne remplace pas un test de charge.

## 6. Stack, heap et shared state

Chaque thread a son contexte d'exécution : pile d'appels et variables locales. Les objets peuvent être partagés dans le heap. Pour les virtual threads, la représentation de la pile est gérée différemment par la JVM : retenir surtout l'isolation du contexte, sans dépendre d'une disposition physique simplifiée.

```java
void handle(List<String> sharedReviews) {
    List<String> localReference = sharedReviews;
    localReference.add("avis"); // Modifie bien l'objet partagé !
}
```

Une référence locale ne rend pas l'objet référencé privé. De même, `final List<String>` interdit de réaffecter la référence, mais pas de modifier la liste.

La première stratégie consiste à **réduire le mutable shared state** : valeurs immuables, copies, variables propres à une requête. Un `record` contenant une liste mutable n'est pas profondément immuable ; copier la liste si nécessaire.

## 7. Race condition, data race, visibility et atomicity

Une **race condition** signifie que la correction dépend d'un entrelacement défavorable. Une **data race** est plus précise : accès concurrents conflictuels à une variable, dont au moins une écriture, sans relation de synchronisation adéquate.

```java
count++; // lecture, calcul, écriture : opération composée
```

Deux threads peuvent lire 10 puis écrire chacun 11 : un incrément est perdu. Déclarer `count` volatile ne rend pas cet incrément atomique.

| Besoin | Solution typique |
|---|---|
| Rendre visible un flag d'arrêt | `volatile boolean` |
| Incrémenter un compteur partagé | `AtomicLong.incrementAndGet()` |
| Maintenir plusieurs champs cohérents ensemble | Même verrou pour toutes les opérations concernées |
| Réduire le partage | Immutabilité, copies, confinement |

**Atomicity** : l'opération apparaît indivisible. **Visibility** : une modification est correctement observable par d'autres threads. **Ordering** : les contraintes d'ordre nécessaires sont respectées.

`volatile` fournit des garanties de visibilité et d'ordre ; il ne faut pas le décrire simplement comme « désactiver le cache ». Une lecture volatile observant une écriture établit les garanties mémoire associées. Un déverrouillage et une acquisition ultérieure du même verrou, `start()` et `join()` fournissent également des relations *happens-before*. [Java Memory Model](https://docs.oracle.com/javase/specs/jls/se21/html/jls-17.html)

### Exemple MinMaxMetrics

```java
class MinMaxMetrics {
    private volatile long min = Long.MAX_VALUE;
    private volatile long max = Long.MIN_VALUE;

    public synchronized void addSample(long sample) {
        min = Math.min(min, sample);
        max = Math.max(max, sample);
    }

    public long getMin() { return min; }
    public long getMax() { return max; }
}
```

Le verrou protège les mises à jour composées. Les getters sans verrou ont besoin de la visibilité fournie par les champs volatile. Chaque métrique est lisible indépendamment, mais deux getters ne forment pas un snapshot atomique du couple. Avant le premier sample, les valeurs sont des sentinelles : le contrat doit le préciser.

Pour une moyenne exacte avec count/sum cohérents, protéger le groupe de champs, puis renvoyer un snapshot sous le même verrou. En application, réutiliser aussi les instruments de métriques du projet plutôt que réinventer tout le monitoring.

## 8. Choisir les locks et éviter les deadlocks

### synchronized : le choix simple

```java
class LocalStock {
    private int available = 10;

    public synchronized boolean reserve() {
        if (available == 0) return false;
        available--;
        return true;
    }
}
```

La vérification et la modification sont protégées ensemble. Le verrou est réentrant : le même thread peut le reprendre. Les lectures supplémentaires de `available` doivent suivre le même protocole. Cette classe protège uniquement un stock local à une JVM.

### ReentrantLock : quand des capacités supplémentaires sont nécessaires

```java
if (!lock.tryLock(100, TimeUnit.MILLISECONDS)) {
    throw new IllegalStateException("Ressource occupée");
}
try {
    updateSharedState();
} finally {
    lock.unlock();
}
```

Cet extrait propage `InterruptedException`. `tryLock()` sans délai tente immédiatement ; `lockInterruptibly()` permet d'interrompre l'attente d'acquisition. Ne libérer que si l'acquisition a réussi. `ReentrantLock` n'est pas automatiquement plus rapide que `synchronized`.

### ReadWriteLock : lecteurs concurrents, écriture exclusive

Utile pour un état local lu souvent et modifié rarement, si les sections sont suffisamment significatives pour amortir le coût du verrou. Tous les accès utilisent le même `ReadWriteLock`. Mesurer avant de supposer un gain.

Dans ProductReviewsService, ajout/suppression/ajout d'avis prennent le write lock ; consultations prennent le read lock. Pour renvoyer les avis après libération du verrou, créer un snapshot **sous le read lock** :

```java
return Collections.unmodifiableList(new ArrayList<>(reviews));
```

`Collections.unmodifiableList(reviews)` seul est une vue : un autre thread peut encore modifier l'objet sous-jacent.

### Deadlock

Si A détient lock1 et attend lock2, tandis que B détient lock2 et attend lock1, personne ne progresse. Acquérir les verrous dans un ordre global constant, ou utiliser un verrou commun si l'invariant est commun. Garder les sections courtes ; éviter un appel HTTP sous verrou.

Un autre blocage possible : tous les workers d'un pool attendent des sous-tâches soumises au même pool saturé. Il n'y a pas besoin de deux `synchronized` pour bloquer une architecture.

## 9. Communication : préférer les outils qui expriment le besoin

| Outil | Usage concret | Limite à connaître |
|---|---|---|
| `BlockingQueue` | Transférer des jobs entre producteurs et consommateurs | Borner la capacité ; prévoir l'arrêt |
| `CountDownLatch` | Attendre N événements, par exemple des chargements initiaux | À usage unique |
| `CyclicBarrier` | Attendre les autres participants à chaque étape | Participants et erreurs à gérer |
| `Phaser` | Plusieurs phases avec participants variables | Plus complexe, besoin spécialisé |
| `Semaphore` | Limiter les appels concurrents à une API | Ne limite pas les requêtes par seconde |
| `Condition` | Construire une attente spécifique sous Lock | Protocole manuel à respecter |

### Producer / consumer

```java
BlockingQueue<String> jobs = new ArrayBlockingQueue<>(100);
jobs.put("instruction-123"); // Producteur : attend si pleine.
String job = jobs.take();    // Consommateur : attend si vide.
```

Ces deux opérations s'exécutent normalement dans des tâches distinctes. La file mémorise les éléments, contrairement à une simple notification. `BlockingQueue` n'a pas de close universel : décider d'un protocole d'interruption ou de messages de fin.

### Semaphore : protéger une dépendance lente

```java
private final Semaphore permits = new Semaphore(20);

public String callLimited(Callable<String> call) throws Exception {
    if (!permits.tryAcquire(100, TimeUnit.MILLISECONDS)) {
        throw new RejectedExecutionException("Limite de concurrence atteinte");
    }
    try {
        return call.call(); // L'appel doit aussi avoir son propre timeout.
    } finally {
        permits.release();
    }
}
```

La limite 20 est illustrative, à déterminer selon la capacité de la dépendance. L'attente d'un permis et la durée de l'appel sont deux budgets différents. Une limite par instance est multipliée par le nombre de pods.

### CountDownLatch

Chaque worker appelle `countDown()` lorsqu'il a terminé ; un coordinateur appelle `await()`. Un `countDown()` dans `finally` peut compter une fin, même en échec : collecter les erreurs séparément. « Tout est terminé » ne signifie pas « tout a réussi ».

### wait / notify et Condition : savoir expliquer

```java
synchronized (monitor) {
    while (!ready) {
        monitor.wait();
    }
}
```

Le producteur modifie `ready` sous le même monitor et appelle `notifyAll()`. `wait()` libère ce monitor, puis le reprend avant de retourner. `sleep()` ne libère pas les verrous détenus. Tester dans `while`, car un réveil ne garantit pas la condition.

Avec `ReentrantLock`, les équivalents sont `Condition.await()`, `signal()` et `signalAll()`. Plusieurs conditions peuvent partager un lock, par exemple notEmpty/notFull. Une notification ne se stocke pas : l'état partagé doit mémoriser ce qui s'est produit. Pour un besoin standard, ne pas réécrire une file ou une barrière en production.

## 10. Atomics, CAS et lock-free

```java
AtomicLong max = new AtomicLong(Long.MIN_VALUE);
max.accumulateAndGet(sample, Math::max);
```

La mise à jour combine atomiquement la valeur actuelle et le sample. À l'inverse, `max.set(Math.max(max.get(), sample))` n'est pas atomique dans son ensemble.

| Méthode | Effet |
|---|---|
| `getAndSet(v)` | Remplace et retourne l'ancienne valeur |
| `incrementAndGet()` | Incrémente et retourne la nouvelle valeur |
| `compareAndSet(expected, update)` | Remplace seulement si la valeur correspond ; retourne un boolean |
| `updateAndGet(f)` | Calcule depuis la valeur actuelle |
| `accumulateAndGet(v, f)` | Calcule depuis la valeur actuelle et un argument |

**CAS — Compare And Set** permet une tentative conditionnelle ; une boucle peut réessayer après concurrence. Les fonctions de mise à jour doivent être sans effets de bord : pas d'envoi de message ou de paiement dans une fonction susceptible d'être réévaluée.

`AtomicReference` protège une référence, pas les modifications arbitraires de l'objet pointé. Il est utile avec un état immuable remplacé en entier. `LongAdder` peut convenir à des compteurs statistiques très disputés ; sa somme n'est pas un snapshot atomique pendant des mises à jour concurrentes.

**Lock-free** signifie une garantie de progression globale sans verrou bloquant, pas que chaque thread termine rapidement. **Wait-free** exige une progression individuelle bornée. Retenir la différence ; ne pas consacrer l'essentiel de la révision à une structure custom ou au problème ABA sans besoin réel.

## 11. Executors et Future : séparer tâche et exécution

Un `Runnable` décrit un travail sans résultat ; un `Callable<T>` renvoie un résultat et peut lever une exception. L'executor décide où les exécuter.

| Executor | Usage | Risque |
|---|---|---|
| `newFixedThreadPool(n)` | Nombre fixe de workers | File d'attente non bornée |
| `newCachedThreadPool()` | Réutilisation ou création selon les besoins | Croissance excessive des threads |
| `newSingleThreadExecutor()` | Sérialiser des tâches | File non bornée, goulot d'étranglement |
| `newVirtualThreadPerTaskExecutor()` | Une tâche I/O par virtual thread | Pas de limite automatique des appels externes |

Pour un pool CPU ou une admission contrôlée, un `ThreadPoolExecutor` à file bornée permet de rendre la saturation explicite :

```java
int workers = Math.max(1, Runtime.getRuntime().availableProcessors());
ExecutorService executor = new ThreadPoolExecutor(
    workers, workers, 0L, TimeUnit.MILLISECONDS,
    new ArrayBlockingQueue<>(200),
    new ThreadPoolExecutor.AbortPolicy());
```

La capacité 200 est un exemple. Traiter le rejet et fermer l'executor à l'arrêt. Ne pas employer une politique qui exécute un travail bloquant dans un thread event-loop. Le nombre de cœurs est un point de départ pour du CPU pur, pas un dimensionnement universel. [ThreadPoolExecutor](https://docs.oracle.com/en/java/javase/21/docs/api/java.base/java/util/concurrent/ThreadPoolExecutor.html)

```java
Future<String> future = executor.submit(() -> chargerProduit());
try {
    String value = future.get(500, TimeUnit.MILLISECONDS);
} catch (TimeoutException e) {
    future.cancel(true); // Demande d'annulation, pas arrêt forcé garanti.
    throw e;
}
```

L'extrait laisse propager les autres exceptions. `ExecutionException` enveloppe l'erreur de la tâche. `shutdown()` refuse les nouvelles tâches et laisse finir celles acceptées. `shutdownNow()` tente d'interrompre les tâches en cours. Avec Java 21+, `close()` attend la terminaison : un try-with-resources autour d'un executor peut donc attendre longtemps si une tâche ne finit pas.

Dans un service, gérer l'executor au niveau application ou bean plutôt que créer et détruire un pool par requête.

## 12. Blocking I/O, async et CompletableFuture

`httpClient.send()` attend la réponse. `sendAsync()` retourne un futur. Déléguer `send()` à un pool rend l'appel asynchrone pour son appelant, mais un worker attend toujours : **asynchronous** et **non-blocking** ne sont pas des synonymes absolus.

```java
CompletableFuture<HttpResponse<String>> response =
    httpClient.sendAsync(request, HttpResponse.BodyHandlers.ofString());

CompletableFuture<String> body = response
    .thenApply(HttpResponse::body)
    .orTimeout(800, TimeUnit.MILLISECONDS);
```

Observer le résultat ou gérer l'erreur ; ne pas abandonner le futur. Dans un handler HTTP, prévoir une réponse d'erreur et la fermeture des ressources de l'échange.

| Méthode | Usage |
|---|---|
| `thenApply` | Transformer une valeur |
| `thenCompose` | Enchaîner une opération qui renvoie un futur |
| `thenCombine` | Combiner deux résultats indépendants |
| `thenAccept` | Consommer le résultat, sans valeur de retour |
| `allOf` | Attendre la fin de tous les futurs ; récupérer ensuite leurs valeurs |
| `exceptionally` | Fournir une récupération en cas d'erreur |
| `whenComplete` | Observer résultat/erreur, notamment pour les logs |

Les callbacks sans suffixe Async peuvent s'exécuter dans le thread qui complète le futur ou dans l'appelant si déjà complété. Les variantes Async permettent une autre stratégie d'exécution, avec un executor explicite au besoin. Éviter les appels bloquants incontrôlés dans le common pool.

`get()` et `join()` attendent tous deux ; `get()` est interruptible et utilise des exceptions checked. Un timeout du futur ne garantit pas l'arrêt du travail sous-jacent : configurer aussi les timeouts HTTP/JDBC. `anyOf()` signifie première complétion, éventuellement en échec, pas premier succès. [CompletableFuture](https://docs.oracle.com/en/java/javase/21/docs/api/java.base/java/util/concurrent/CompletableFuture.html)

Le modèle « Thread-Per-Core » du cours décrit une organisation visée ; un pool de taille 8 ne prouve ni la présence de 8 cœurs ni une affinité CPU.

## 13. Virtual threads : intérêt et limites

Un virtual thread est planifié par la JVM sur un platform thread appelé **carrier**. Pendant une attente compatible, il peut être **unmounted** : son état est conservé, le carrier exécute un autre travail. Il sera ensuite remonté, éventuellement sur un autre carrier.

Pour le code, un appel bloquant reste séquentiel. Le bénéfice est de ne pas réserver un thread système à chaque attente. Les virtual threads conviennent aux nombreuses tâches I/O concurrentes, pas à l'accélération automatique d'un calcul CPU. Ils sont disponibles sans preview depuis Java 21. [Guide Oracle](https://docs.oracle.com/en/java/javase/25/core/virtual-threads.html)

```java
// Petit programme Java 21+ ; main peut déclarer throws Exception.
try (ExecutorService executor = Executors.newVirtualThreadPerTaskExecutor()) {
    Future<String> product = executor.submit(() -> chargerProduit());
    Future<String> rating = executor.submit(() -> chargerNote());
    System.out.println(product.get() + " / " + rating.get());
}
```

Les deux tâches sont soumises avant l'attente : elles peuvent progresser ensemble. Cet exemple montre le mécanisme, pas une politique complète de timeout/annulation.

Règles pratiques : une tâche par virtual thread ; pas de pool fixe de virtual threads pour limiter une dépendance ; utiliser plutôt une admission ou un sémaphore. Borner le travail accepté et les temps d'attente. La mémoire, les connexions JDBC et les capacités distantes restent limitées. Les règles de thread safety restent identiques.

### Pinning : vérifier le JDK

Sur Java 21–23, bloquer dans un `synchronized` peut maintenir le virtual thread attaché au carrier. Java 24 apporte la suppression du pinning dû aux moniteurs `synchronized`. Ne pas remplacer mécaniquement tous les moniteurs par des `ReentrantLock` en Java 24+. Les sections critiques longues restent un problème de contention, même sans pinning. [Changements JDK 24](https://docs.oracle.com/en/java/javase/24/migrate/significant-changes-jdk-24.html)

Certains passages natifs peuvent encore limiter le détachement. Diagnostiquer avec les outils du JDK ciblé plutôt que déduire le comportement d'une vidéo ancienne.

## 14. Application à Spring Boot, JPA et workers

### Beans singleton

Un `@Service` singleton peut être appelé par plusieurs requêtes. Ne pas stocker un utilisateur courant, une requête ou un résultat intermédiaire dans un champ mutable. Les dépendances injectées peuvent être partagées si leur contrat le permet ; les données d'une requête restent locales.

### Transactions et contexte

Dans le modèle Spring impératif habituel, la transaction est liée au thread : lancer une tâche dans un executor ne propage pas automatiquement la transaction appelante. Chaque tâche doit avoir une frontière transactionnelle explicite adaptée. [Transactions Spring](https://docs.spring.io/spring-framework/reference/data-access/transaction/declarative/tx-decl-explained.html) Ne pas partager une session Hibernate/EntityManager concret entre tâches concurrentes.

MDC, SecurityContext et ThreadLocal nécessitent aussi une stratégie de propagation/nettoyage. `InheritableThreadLocal` ne résout pas tous les cas avec des pools ; une valeur ne doit pas fuiter vers une autre requête.

### Activation des virtual threads

Sur une version compatible de Spring Boot, à partir de 3.2 et avec Java 21+, la propriété suivante active les intégrations prévues par Boot :

```properties
spring.threads.virtual.enabled=true
```

Ce n'est pas une conversion de tous les executors custom ni une suppression de la capacité maximale de la base. Vérifier le serveur embarqué, les beans personnalisés et la documentation de la version du projet. [Task execution Spring Boot](https://docs.spring.io/spring-boot/reference/features/task-execution-and-scheduling.html)

### Trois décisions métier typiques

| Situation | Décision de départ |
|---|---|
| REST appelle deux services indépendants | Exécuter les appels concurremment avec deadlines et erreurs explicites |
| Worker RabbitMQ écrit en base | Aligner concurrence, prefetch et pool JDBC ; préserver ack et idempotence |
| Batch de calcul lourd | Parallélisme borné, partitions indépendantes, budget CPU mesuré |

Pour une mise à jour de stock ou une instruction financière, une erreur ne doit pas être masquée par un fallback « succès ». Retry et idempotence se conçoivent avec le contrat métier.

## 15. Au-delà de la JVM : limites de la synchronisation locale

Un `synchronized`, un `AtomicLong` ou un `Semaphore` ne protège que l'instance concernée dans une JVM. Avec plusieurs pods, chaque process a son propre état.

Pour une unicité métier partagée, utiliser par exemple une contrainte SQL, une opération conditionnelle en base ou une stratégie d'idempotence. Un verrou distribué n'est pas une substitution automatique et demande un protocole correct.

Exemple conceptuel : réserver un stock par une mise à jour conditionnelle `UPDATE ... SET stock = stock - 1 WHERE id = ? AND stock > 0`, puis vérifier le nombre de lignes modifiées. La cohérence dépend de la transaction et du schéma, pas d'un verrou Java local.

## 16. Ce qu'il faut adapter dans les exemples fournis

L'archive contient 19 projets ZIP, avec 26 fichiers Java hors métadonnées macOS. Inventaire effectué ; lecture ciblée de quatre projets, pas audit exhaustif ni benchmark.

| Exemple examiné | Ce qu'il enseigne | Adaptation pour une mission |
|---|---|---|
| `data-races-example` | Observation incohérente de x/y sans synchronisation | Corriger le protocole ; l'absence de message ne prouve rien |
| `application-metrics-example` | Écrivains synchronisés et lecture volatile | Gérer l'arrêt ; ne pas ignorer les interruptions ; mesurer les durées avec nanoTime |
| `wait-notify-example` | File bornée, producer/consumer, fin de flux | Préférer BlockingQueue ; traiter les erreurs et l'arrêt explicitement |
| `high-performance-io-virtual-threads` | Comparer l'exécution de tâches avec beaucoup d'attentes | Même workload pour les variantes ; peu de logs ; vrais timeouts et observation des Futures |

Les variantes I/O examinées n'ont pas exactement le même nombre de tâches et la même distribution des sleeps. Elles illustrent des mécanismes ; leurs durées brutes ne constituent pas une comparaison contrôlée. `sleep` simule une attente, pas les limites d'un vrai pool JDBC ou d'un service distant.

## 17. Diagnostic et validation utiles

Avant de choisir un nouveau mécanisme, identifier le symptôme : CPU saturé, threads bloqués, dépendance lente, file qui grandit, erreurs ou heap qui augmente.

- **Thread dump** : repérer attentes répétées et deadlocks. Un thread `RUNNABLE` n'est pas une preuve suffisante de forte consommation CPU.
- **JFR** : observer allocations, contention et événements pertinents pour le JDK.
- **JMeter** : tester débit, percentiles et erreurs sur un parcours réaliste.
- **JMH** : microbenchmark isolé ; utile si le choix d'une primitive est réellement critique.
- **Tests concurrents** : provoquer des entrelacements avec des latches/barriers ; éviter les tests fondés uniquement sur `sleep`.

Les tests peuvent révéler des défauts ; quelques tests réussis ne prouvent pas toutes les propriétés de concurrence. Pour comparer deux variantes : même dataset, mêmes quotas, warmup, même dépendance, même charge et plusieurs mesures.

## 18. Mise en pratique pour la deuxième formation

Construire un petit endpoint d'agrégation d'instruction : détail + statut de marché. Utiliser deux services simulés avec délais et erreurs configurables.

1. Version séquentielle, puis deux appels concurrents avec un executor géré.
2. Version virtual threads avec limites de concurrence sur chaque dépendance.
3. Ajouter timeout HTTP, budget global et réponse d'erreur explicite.
4. Simuler une dépendance lente et vérifier que le travail en cours reste maîtrisé.
5. Comparer p95, débit, erreurs, mémoire et connexions ; ne pas s'arrêter au nombre de threads.
6. Vérifier l'arrêt propre et l'absence de contamination du contexte entre requêtes.

Critère de réussite : savoir expliquer où chaque tâche attend, ce qui limite les appels et ce qui se passe en timeout. Un graphique flatteur sans explication de la saturation ne suffit pas.

Pour `ScopedValue`, apprendre le contexte lié à une portée d'exécution et préférer des valeurs immuables. Pour Structured Concurrency, comprendre d'abord le regroupement des sous-tâches, leurs erreurs et leur durée de vie. Pratiquer la preview dans un laboratoire séparé si le projet ne l'autorise pas.

## 19. Fiche de révision pour entretien

| Question | Réponse courte |
|---|---|
| volatile rend-il `++` thread-safe ? | Non, l'opération est composée. |
| Pourquoi un bean singleton peut poser problème ? | Ses champs mutables sont partagés entre appels concurrents. |
| AtomicLong rend-il plusieurs compteurs cohérents ensemble ? | Non, chaque opération atomique porte sur son propre état. |
| wait libère-t-il tous les verrous ? | Non, seulement le monitor sur lequel il attend. |
| Comment éviter un deadlock entre locks ? | Ordre d'acquisition constant, ou verrou commun. |
| Pourquoi un fixed pool peut épuiser la mémoire ? | Sa file par défaut peut grandir sans borne. |
| Virtual threads = calculs plus rapides ? | Non, ils améliorent surtout la capacité à gérer les attentes I/O. |
| Semaphore ou rate limiter ? | Simultanéité contre débit par unité de temps. |
| Une liste unmodifiable est-elle un snapshot ? | Pas nécessairement : il faut une copie. |
| Un timeout annule-t-il toujours l'appel distant ? | Non, il faut gérer l'annulation et les timeouts de la dépendance. |
| Un verrou local protège-t-il plusieurs pods ? | Non. |
| Peut-on mettre en production une API preview sans réfléchir ? | Non, vérifier la politique du projet et le contrat de version. |

## 20. Références et périmètre

Sources de départ : programme des 12 sections fourni, description de Michael Pogrebinsky jointe, exercices partagés et lecture ciblée du ZIP. Les liens officiels ci-dessous permettent de vérifier les contrats ; les exemples de ce guide sont originaux ou adaptés aux problèmes déjà discutés.

- [Programme de la deuxième formation](https://www.udemy.com/course/java-virtual-thread/)
- [Java Memory Model](https://docs.oracle.com/javase/specs/jls/se21/html/jls-17.html)
- [Package java.util.concurrent](https://docs.oracle.com/en/java/javase/21/docs/api/java.base/java/util/concurrent/package-summary.html)
- [ThreadPoolExecutor](https://docs.oracle.com/en/java/javase/21/docs/api/java.base/java/util/concurrent/ThreadPoolExecutor.html)
- [CompletableFuture](https://docs.oracle.com/en/java/javase/21/docs/api/java.base/java/util/concurrent/CompletableFuture.html)
- [Guide Virtual Threads](https://docs.oracle.com/en/java/javase/25/core/virtual-threads.html)
- [Évolutions Java 24, dont synchronized sans pinning](https://docs.oracle.com/en/java/javase/24/migrate/significant-changes-jdk-24.html)
- [ScopedValue Java 25](https://docs.oracle.com/en/java/javase/25/docs/api/java.base/java/lang/ScopedValue.html)
- [StructuredTaskScope Java 26 — preview](https://docs.oracle.com/en/java/javase/26/docs/api/java.base/java/util/concurrent/StructuredTaskScope.html)
- [Spring Boot — task execution](https://docs.spring.io/spring-boot/reference/features/task-execution-and-scheduling.html)

Le JDK et les dépendances réellement utilisés en mission restent la référence pour choisir une API et interpréter les performances.
