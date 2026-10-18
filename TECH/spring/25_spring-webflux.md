# Spring WebFlux & Programmation Réactive — Tutoriel Complet

> **Niveau** : Intermédiaire à Avancé | **Durée estimée** : 3-4h de lecture et pratique

---

## Table des matières

1. [Introduction à la programmation réactive](#1-introduction-à-la-programmation-réactive)
2. [Reactive Streams & Project Reactor](#2-reactive-streams--project-reactor)
3. [Spring WebFlux — Architecture](#3-spring-webflux--architecture)
4. [Mise en place du projet](#4-mise-en-place-du-projet)
5. [Mono et Flux — Les types fondamentaux](#5-mono-et-flux--les-types-fondamentaux)
6. [Créer des endpoints avec WebFlux](#6-créer-des-endpoints-avec-webflux)
7. [WebClient — Le client HTTP réactif](#7-webclient--le-client-http-réactif)
8. [Gestion des erreurs](#8-gestion-des-erreurs)
9. [Opérateurs avancés](#9-opérateurs-avancés)
10. [Tests avec WebTestClient](#10-tests-avec-webtestclient)
11. [Bonnes pratiques & Pièges courants](#11-bonnes-pratiques--pièges-courants)

---

## 1. Introduction à la programmation réactive

### Le problème du modèle impératif traditionnel

Dans le modèle traditionnel de Spring MVC (basé sur les Servlets), chaque requête HTTP monopolise **un thread** pendant toute sa durée de traitement. Lorsque ce thread effectue une opération I/O (appel à une base de données, appel HTTP externe), il est **bloqué** en attente de la réponse.

```
Thread 1 : [Traitement]-[BLOQUÉ en attente DB]-[Réponse]
Thread 2 : [Traitement]-[BLOQUÉ en attente API]-[Réponse]
Thread 3 : [Traitement]-[BLOQUÉ en attente API]-[Réponse]
...
Thread N : En attente dans la pool
```

**Conséquence** : Pour supporter 1000 requêtes simultanées, il faut 1000 threads. Or chaque thread consomme de la mémoire (512 Ko à 1 Mo par défaut en JVM), ce qui rend le système coûteux en ressources et peu scalable.

### Le modèle réactif : non-bloquant et event-driven

La programmation réactive résout ce problème avec un modèle **non-bloquant** basé sur les événements. Un petit nombre de threads (souvent égal au nombre de cœurs CPU) gèrent un grand nombre de requêtes en n'étant **jamais bloqués** :

```
Thread 1 : [Req A]→[délègue I/O]→[Req B]→[délègue I/O]→[callback A]→[callback B]
```

Quand une opération I/O est lancée, le thread continue à traiter d'autres requêtes. Quand la réponse arrive (événement), un callback reprend le traitement.

### Les 4 principes du Reactive Manifesto

- **Responsive** : le système répond dans des délais raisonnables et prévisibles.
- **Resilient** : le système reste disponible en cas de défaillance partielle.
- **Elastic** : le système s'adapte à la charge (scale up/down).
- **Message Driven** : la communication est asynchrone et basée sur les messages.

### Quand utiliser la programmation réactive ?

| Cas d'usage | Réactif recommandé ? |
|---|---|
| Nombreux appels I/O simultanés (API gateway, BFF) | ✅ Oui |
| Streaming de données en temps réel | ✅ Oui |
| Microservices avec forte concurrence | ✅ Oui |
| Application CRUD simple avec peu de charge | ⚠️ Pas forcément |
| Logique de calcul CPU-intensive | ❌ Non (préférer les threads classiques) |

---

## 2. Reactive Streams & Project Reactor

### La spécification Reactive Streams

**Reactive Streams** est une spécification (intégrée dans le JDK 9 via `java.util.concurrent.Flow`) qui définit une API standard pour les flux de données asynchrones avec **backpressure**.

Elle définit 4 interfaces :

```java
// Le publisher produit des éléments à la demande
public interface Publisher<T> {
    void subscribe(Subscriber<? super T> subscriber);
}

// Le subscriber consomme les éléments
public interface Subscriber<T> {
    void onSubscribe(Subscription s);
    void onNext(T t);
    void onError(Throwable t);
    void onComplete();
}

// Lie un Publisher à un Subscriber
public interface Subscription {
    void request(long n); // demande N éléments (backpressure)
    void cancel();
}

// Publisher et Subscriber à la fois
public interface Processor<T, R> extends Subscriber<T>, Publisher<R> {}
```

### Le concept de Backpressure

La **backpressure** est un mécanisme crucial qui permet au consommateur de signaler au producteur à quelle vitesse il peut traiter les données. Cela évite de **submerger** un consommateur lent avec trop de données.

```
Producer          Subscriber
   |                   |
   |---subscribe()---->|
   |                   |
   |<--request(10)-----|  "Je peux traiter 10 éléments"
   |                   |
   |---onNext(item1)-->|
   |---onNext(item2)-->|
   |        ...        |
   |---onNext(item10)->|
   |                   |
   |<--request(5)------|  "J'ai fini, j'en veux 5 de plus"
   |        ...        |
   |---onComplete()---->|
```

### Project Reactor

**Project Reactor** est l'implémentation de Reactive Streams utilisée par Spring WebFlux. Elle fournit deux types principaux :

- **`Mono<T>`** : un flux qui émet **0 ou 1** élément, puis se termine (ou erreur).
- **`Flux<T>`** : un flux qui émet **0 à N** éléments, puis se termine (ou erreur).

```
Mono :  ---[item]---[COMPLETE]
        ou ---[COMPLETE] (vide)
        ou ---[ERROR]

Flux :  ---[item1]---[item2]---[item3]---[COMPLETE]
        ou ---[item1]---[ERROR]
        ou ---[COMPLETE] (vide)
```

---

## 3. Spring WebFlux — Architecture

### Spring MVC vs Spring WebFlux

```
Spring MVC                         Spring WebFlux
─────────────────────────────      ─────────────────────────────
Servlet API (Servlet Container)    Reactive HTTP (Netty / Undertow)
Thread-per-request                 Event loop (non-blocking)
Synchrone / Bloquant               Asynchrone / Non-bloquant
Spring Data JPA (JDBC bloquant)    Spring Data R2DBC / MongoDB Reactive
RestTemplate                       WebClient
```

> **Important** : Spring WebFlux peut aussi fonctionner sur un conteneur Servlet (comme Tomcat) via le support `javax.servlet.AsyncContext`. Mais pour tirer pleinement parti du modèle non-bloquant, Netty est recommandé.

### Les deux modèles de programmation WebFlux

WebFlux supporte deux façons d'écrire des endpoints :

**1. Annotated Controllers** (familier pour les développeurs Spring MVC) :
```java
@RestController
public class UserController {
    @GetMapping("/users/{id}")
    public Mono<User> getUser(@PathVariable String id) { ... }
}
```

**2. Functional Endpoints** (RouterFunction + HandlerFunction) :
```java
RouterFunction<ServerResponse> routes = route()
    .GET("/users/{id}", handler::getUser)
    .build();
```

Ce tutoriel couvre principalement le style annoté, plus courant en entreprise.

---

## 4. Mise en place du projet

### Dépendances Maven (`pom.xml`)

```xml
<parent>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-parent</artifactId>
    <version>3.2.0</version>
</parent>

<dependencies>
    <!-- WebFlux (inclut Netty par défaut) -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-webflux</artifactId>
    </dependency>

    <!-- Optionnel : base de données réactive R2DBC + H2 -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-data-r2dbc</artifactId>
    </dependency>
    <dependency>
        <groupId>io.r2dbc</groupId>
        <artifactId>r2dbc-h2</artifactId>
        <scope>runtime</scope>
    </dependency>

    <!-- Tests -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-test</artifactId>
        <scope>test</scope>
    </dependency>
    <dependency>
        <groupId>io.projectreactor</groupId>
        <artifactId>reactor-test</artifactId>
        <scope>test</scope>
    </dependency>
</dependencies>
```

### Structure du projet

```
src/
├── main/
│   ├── java/com/exemple/demo/
│   │   ├── DemoApplication.java
│   │   ├── controller/
│   │   │   └── UserController.java
│   │   ├── service/
│   │   │   └── UserService.java
│   │   ├── repository/
│   │   │   └── UserRepository.java
│   │   ├── model/
│   │   │   └── User.java
│   │   └── config/
│   │       └── WebClientConfig.java
│   └── resources/
│       └── application.yml
└── test/
    └── java/com/exemple/demo/
        └── controller/
            └── UserControllerTest.java
```

---

## 5. Mono et Flux — Les types fondamentaux

### Créer un Mono

```java
// Depuis une valeur
Mono<String> mono1 = Mono.just("Hello");

// Vide
Mono<String> mono2 = Mono.empty();

// Depuis une erreur
Mono<String> mono3 = Mono.error(new RuntimeException("Erreur!"));

// Depuis un Callable (exécuté à la subscription)
Mono<String> mono4 = Mono.fromCallable(() -> computeValue());

// Depuis un CompletableFuture
Mono<String> mono5 = Mono.fromFuture(myFuture);

// Depuis un Optional
Optional<String> opt = Optional.of("valeur");
Mono<String> mono6 = Mono.justOrEmpty(opt);
```

### Créer un Flux

```java
// Depuis des valeurs
Flux<Integer> flux1 = Flux.just(1, 2, 3, 4, 5);

// Depuis une collection
List<String> list = List.of("a", "b", "c");
Flux<String> flux2 = Flux.fromIterable(list);

// Depuis un range
Flux<Integer> flux3 = Flux.range(1, 10); // 1 à 10 inclus

// Intervalles (utile pour le streaming)
Flux<Long> flux4 = Flux.interval(Duration.ofSeconds(1)); // émet 0, 1, 2... chaque seconde

// Infini : génération programmatique
Flux<String> flux5 = Flux.generate(sink -> {
    sink.next("valeur");
    // sink.complete() pour terminer
});
```

### Les opérateurs essentiels

#### Transformation

```java
// map : transforme chaque élément (synchrone)
Flux<Integer> longueurs = Flux.just("hello", "world", "!")
    .map(String::length); // [5, 5, 1]

// flatMap : transforme chaque élément en Publisher et aplatit (asynchrone)
Flux<User> users = Flux.just("1", "2", "3")
    .flatMap(id -> userRepository.findById(id)); // appels parallèles

// flatMapSequential : comme flatMap mais préserve l'ordre
Flux<User> usersOrdered = Flux.just("1", "2", "3")
    .flatMapSequential(id -> userRepository.findById(id));

// switchMap : annule le flux précédent si un nouvel élément arrive (utile pour autocomplete)
Flux<Result> results = searchTerms
    .switchMap(term -> searchService.search(term));
```

#### Filtrage

```java
Flux<Integer> numbers = Flux.range(1, 10);

// filter
Flux<Integer> pairs = numbers.filter(n -> n % 2 == 0); // [2, 4, 6, 8, 10]

// take : prendre les N premiers
Flux<Integer> premiers = numbers.take(3); // [1, 2, 3]

// skip : ignorer les N premiers
Flux<Integer> reste = numbers.skip(7); // [8, 9, 10]

// distinct : dédoublonner
Flux<String> unique = Flux.just("a", "b", "a", "c").distinct(); // [a, b, c]

// takeUntil : prendre jusqu'à une condition
Flux<Integer> jusqua5 = numbers.takeUntil(n -> n >= 5); // [1, 2, 3, 4, 5]
```

#### Combinaison

```java
Flux<String> flux1 = Flux.just("A", "B", "C");
Flux<String> flux2 = Flux.just("1", "2", "3");

// zip : combine élément par élément
Flux<String> zipped = Flux.zip(flux1, flux2, (a, b) -> a + b); // [A1, B2, C3]

// merge : fusionne sans ordre garanti
Flux<String> merged = Flux.merge(flux1, flux2); // ordre d'arrivée

// concat : concatène en séquence
Flux<String> concatenated = Flux.concat(flux1, flux2); // [A, B, C, 1, 2, 3]

// combineLatest : émet à chaque fois qu'un flux émet, avec la dernière valeur de l'autre
Flux<String> combined = Flux.combineLatest(flux1, flux2, (a, b) -> a + "+" + b);
```

#### Gestion des erreurs (voir section dédiée)

```java
flux.onErrorReturn("valeur par défaut")
    .onErrorResume(e -> fallbackFlux)
    .retry(3)
    .retryWhen(Retry.backoff(3, Duration.ofSeconds(1)));
```

---

## 6. Créer des endpoints avec WebFlux

### Le modèle User

```java
package com.exemple.demo.model;

import org.springframework.data.annotation.Id;
import org.springframework.data.relational.core.mapping.Table;

@Table("users")
public class User {
    @Id
    private Long id;
    private String name;
    private String email;

    // Constructeurs, getters, setters...
    public User() {}

    public User(String name, String email) {
        this.name = name;
        this.email = email;
    }

    // getters/setters...
}
```

### Le Repository réactif

```java
package com.exemple.demo.repository;

import com.exemple.demo.model.User;
import org.springframework.data.repository.reactive.ReactiveCrudRepository;
import reactor.core.publisher.Flux;

public interface UserRepository extends ReactiveCrudRepository<User, Long> {
    // Requête dérivée du nom de méthode
    Flux<User> findByName(String name);

    // Requête personnalisée
    @Query("SELECT * FROM users WHERE email LIKE :domain")
    Flux<User> findByEmailDomain(String domain);
}
```

### Le Service

```java
package com.exemple.demo.service;

import com.exemple.demo.model.User;
import com.exemple.demo.repository.UserRepository;
import org.springframework.stereotype.Service;
import reactor.core.publisher.Flux;
import reactor.core.publisher.Mono;

@Service
public class UserService {

    private final UserRepository userRepository;

    public UserService(UserRepository userRepository) {
        this.userRepository = userRepository;
    }

    public Flux<User> findAll() {
        return userRepository.findAll();
    }

    public Mono<User> findById(Long id) {
        return userRepository.findById(id)
            .switchIfEmpty(Mono.error(new UserNotFoundException(id)));
    }

    public Mono<User> create(User user) {
        return Mono.just(user)
            .flatMap(u -> {
                // Validation métier
                if (u.getEmail() == null || u.getEmail().isBlank()) {
                    return Mono.error(new IllegalArgumentException("Email requis"));
                }
                return userRepository.save(u);
            });
    }

    public Mono<User> update(Long id, User updatedUser) {
        return userRepository.findById(id)
            .switchIfEmpty(Mono.error(new UserNotFoundException(id)))
            .flatMap(existingUser -> {
                existingUser.setName(updatedUser.getName());
                existingUser.setEmail(updatedUser.getEmail());
                return userRepository.save(existingUser);
            });
    }

    public Mono<Void> delete(Long id) {
        return userRepository.findById(id)
            .switchIfEmpty(Mono.error(new UserNotFoundException(id)))
            .flatMap(userRepository::delete);
    }
}
```

### Le Controller

```java
package com.exemple.demo.controller;

import com.exemple.demo.model.User;
import com.exemple.demo.service.UserService;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.web.bind.annotation.*;
import reactor.core.publisher.Flux;
import reactor.core.publisher.Mono;

import java.time.Duration;

@RestController
@RequestMapping("/api/users")
public class UserController {

    private final UserService userService;

    public UserController(UserService userService) {
        this.userService = userService;
    }

    // GET /api/users — retourne tous les utilisateurs
    @GetMapping(produces = MediaType.APPLICATION_JSON_VALUE)
    public Flux<User> getAllUsers() {
        return userService.findAll();
    }

    // GET /api/users/{id}
    @GetMapping("/{id}")
    public Mono<User> getUserById(@PathVariable Long id) {
        return userService.findById(id);
    }

    // POST /api/users
    @PostMapping
    @ResponseStatus(HttpStatus.CREATED)
    public Mono<User> createUser(@RequestBody User user) {
        return userService.create(user);
    }

    // PUT /api/users/{id}
    @PutMapping("/{id}")
    public Mono<User> updateUser(@PathVariable Long id, @RequestBody User user) {
        return userService.update(id, user);
    }

    // DELETE /api/users/{id}
    @DeleteMapping("/{id}")
    @ResponseStatus(HttpStatus.NO_CONTENT)
    public Mono<Void> deleteUser(@PathVariable Long id) {
        return userService.delete(id);
    }

    // ─── Server-Sent Events (SSE) ────────────────────────────────────────────
    // Streaming : émet un utilisateur toutes les secondes (utile pour le temps réel)
    @GetMapping(value = "/stream", produces = MediaType.TEXT_EVENT_STREAM_VALUE)
    public Flux<User> streamUsers() {
        return userService.findAll()
            .delayElements(Duration.ofSeconds(1)); // simule un flux en temps réel
    }
}
```

> **Note sur le SSE (Server-Sent Events)** : en utilisant `MediaType.TEXT_EVENT_STREAM_VALUE`, Spring WebFlux ouvre une connexion persistante avec le client et envoie les éléments du `Flux` au fur et à mesure. C'est idéal pour les tableaux de bord en temps réel, les notifications, etc.

---

## 7. WebClient — Le client HTTP réactif

`WebClient` est le remplaçant non-bloquant de `RestTemplate`. Il est **fluent**, **non-bloquant** et intégré nativement avec Project Reactor.

### Configuration du WebClient

```java
package com.exemple.demo.config;

import io.netty.channel.ChannelOption;
import io.netty.handler.timeout.ReadTimeoutHandler;
import io.netty.handler.timeout.WriteTimeoutHandler;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.client.reactive.ReactorClientHttpConnector;
import org.springframework.web.reactive.function.client.WebClient;
import reactor.netty.http.client.HttpClient;

import java.time.Duration;
import java.util.concurrent.TimeUnit;

@Configuration
public class WebClientConfig {

    @Bean
    public WebClient webClient() {
        // Configuration du connecteur Netty avec timeouts
        HttpClient httpClient = HttpClient.create()
            .option(ChannelOption.CONNECT_TIMEOUT_MILLIS, 5000) // 5s pour établir la connexion
            .responseTimeout(Duration.ofSeconds(10))             // 10s pour la réponse complète
            .doOnConnected(conn ->
                conn.addHandlerLast(new ReadTimeoutHandler(10, TimeUnit.SECONDS))
                    .addHandlerLast(new WriteTimeoutHandler(10, TimeUnit.SECONDS))
            );

        return WebClient.builder()
            .baseUrl("https://api.exemple.com")
            .clientConnector(new ReactorClientHttpConnector(httpClient))
            .defaultHeader("Accept", "application/json")
            .defaultHeader("Content-Type", "application/json")
            // Intercepteur pour logger les requêtes/réponses
            .filter((request, next) -> {
                System.out.println("Request: " + request.method() + " " + request.url());
                return next.exchange(request);
            })
            .build();
    }
}
```

### Requêtes GET

```java
@Service
public class ExternalUserService {

    private final WebClient webClient;

    public ExternalUserService(WebClient webClient) {
        this.webClient = webClient;
    }

    // Récupérer un objet unique (Mono)
    public Mono<User> getUserById(Long id) {
        return webClient.get()
            .uri("/users/{id}", id)
            .retrieve()
            .bodyToMono(User.class);
    }

    // Récupérer une liste (Flux)
    public Flux<User> getAllUsers() {
        return webClient.get()
            .uri("/users")
            .retrieve()
            .bodyToFlux(User.class);
    }

    // Avec query parameters
    public Flux<User> searchUsers(String name, int page, int size) {
        return webClient.get()
            .uri(uriBuilder -> uriBuilder
                .path("/users")
                .queryParam("name", name)
                .queryParam("page", page)
                .queryParam("size", size)
                .build())
            .retrieve()
            .bodyToFlux(User.class);
    }

    // Récupérer la réponse complète (headers, status code, body)
    public Mono<ResponseEntity<User>> getUserWithResponseEntity(Long id) {
        return webClient.get()
            .uri("/users/{id}", id)
            .retrieve()
            .toEntity(User.class);
    }
}
```

### Requêtes POST, PUT, DELETE

```java
// POST — créer une ressource
public Mono<User> createUser(User user) {
    return webClient.post()
        .uri("/users")
        .bodyValue(user) // body directement depuis l'objet
        .retrieve()
        .bodyToMono(User.class);
}

// POST avec Publisher comme body
public Mono<User> createUserFromMono(Mono<User> userMono) {
    return webClient.post()
        .uri("/users")
        .body(userMono, User.class) // body depuis un Mono/Flux
        .retrieve()
        .bodyToMono(User.class);
}

// PUT — mettre à jour
public Mono<User> updateUser(Long id, User user) {
    return webClient.put()
        .uri("/users/{id}", id)
        .bodyValue(user)
        .retrieve()
        .bodyToMono(User.class);
}

// PATCH — mise à jour partielle
public Mono<User> patchUser(Long id, Map<String, Object> updates) {
    return webClient.patch()
        .uri("/users/{id}", id)
        .bodyValue(updates)
        .retrieve()
        .bodyToMono(User.class);
}

// DELETE
public Mono<Void> deleteUser(Long id) {
    return webClient.delete()
        .uri("/users/{id}", id)
        .retrieve()
        .bodyToMono(Void.class);
}
```

### Headers dynamiques et authentification

```java
// Headers par requête
public Mono<User> getAuthenticatedUser(String token, Long id) {
    return webClient.get()
        .uri("/users/{id}", id)
        .header("Authorization", "Bearer " + token)
        .header("X-Request-ID", UUID.randomUUID().toString())
        .retrieve()
        .bodyToMono(User.class);
}

// Authentification Basic
public Mono<Data> getWithBasicAuth(String username, String password) {
    return webClient.get()
        .uri("/protected/data")
        .headers(headers -> headers.setBasicAuth(username, password))
        .retrieve()
        .bodyToMono(Data.class);
}
```

### exchange() vs retrieve()

```java
// retrieve() : simple, gère automatiquement les erreurs 4xx/5xx
Mono<User> user = webClient.get()
    .uri("/users/1")
    .retrieve() // lance WebClientResponseException pour 4xx/5xx
    .bodyToMono(User.class);

// exchangeToMono() : accès complet à la réponse, gestion manuelle
Mono<User> userManual = webClient.get()
    .uri("/users/1")
    .exchangeToMono(response -> {
        if (response.statusCode().is2xxSuccessful()) {
            return response.bodyToMono(User.class);
        } else if (response.statusCode().value() == 404) {
            return Mono.error(new UserNotFoundException(1L));
        } else {
            return response.createError(); // crée l'exception standard
        }
    });
```

> **Préférer `retrieve()`** dans la grande majorité des cas. Utiliser `exchangeToMono()` seulement quand un contrôle fin de la réponse est nécessaire. Avec `exchangeToMono()`, **vous êtes responsable** de consommer ou libérer le body pour éviter les fuites mémoire.

### Appels parallèles avec WebClient

L'un des grands avantages de WebClient : déclencher plusieurs appels HTTP **en parallèle** de manière simple.

```java
// Deux appels en parallèle avec zip
public Mono<UserProfile> buildUserProfile(Long userId) {
    Mono<User> userMono = webClient.get()
        .uri("/users/{id}", userId)
        .retrieve()
        .bodyToMono(User.class);

    Mono<List<Order>> ordersMono = webClient.get()
        .uri("/users/{id}/orders", userId)
        .retrieve()
        .bodyToFlux(Order.class)
        .collectList();

    // Lance les deux appels en parallèle et combine les résultats
    return Mono.zip(userMono, ordersMono, UserProfile::new);
}

// N appels en parallèle avec flatMap
public Flux<UserDetails> fetchAllUsersDetails(List<Long> userIds) {
    return Flux.fromIterable(userIds)
        .flatMap(id -> webClient.get()
            .uri("/users/{id}/details", id)
            .retrieve()
            .bodyToMono(UserDetails.class)
        , 10); // concurrency = 10 appels max en parallèle
}
```

---

## 8. Gestion des erreurs

### Les opérateurs de gestion d'erreur

```java
// onErrorReturn : retourner une valeur par défaut en cas d'erreur
Mono<User> safe = userService.findById(1L)
    .onErrorReturn(new User("Inconnu", "inconnu@exemple.com"));

// onErrorResume : fallback vers un autre Publisher
Mono<User> withFallback = userService.findById(1L)
    .onErrorResume(UserNotFoundException.class, e ->
        userService.findDefaultUser()
    );

// onErrorMap : transformer l'erreur en une autre
Mono<User> mapped = userService.findById(1L)
    .onErrorMap(DataAccessException.class, e ->
        new ServiceUnavailableException("Service DB indisponible", e)
    );

// doOnError : effet de bord sur l'erreur (log, metric) sans la gérer
Mono<User> withLog = userService.findById(1L)
    .doOnError(e -> log.error("Erreur lors de la récupération: {}", e.getMessage()));
```

### Retry avec backoff exponentiel

```java
public Mono<Data> fetchWithRetry(String url) {
    return webClient.get()
        .uri(url)
        .retrieve()
        .bodyToMono(Data.class)
        .retryWhen(
            Retry.backoff(3, Duration.ofSeconds(1)) // 3 essais, délai initial 1s
                .maxBackoff(Duration.ofSeconds(10))  // délai max 10s
                .jitter(0.5)                         // aléatoire pour éviter les tempêtes
                .filter(e -> e instanceof WebClientResponseException.ServiceUnavailable)
                .onRetryExhaustedThrow((spec, signal) ->
                    new ServiceUnavailableException("Service indisponible après 3 tentatives")
                )
        );
}
```

### Global Error Handler avec @ControllerAdvice

```java
@RestControllerAdvice
public class GlobalErrorHandler {

    @ExceptionHandler(UserNotFoundException.class)
    @ResponseStatus(HttpStatus.NOT_FOUND)
    public Mono<ErrorResponse> handleUserNotFound(UserNotFoundException e) {
        return Mono.just(new ErrorResponse(
            HttpStatus.NOT_FOUND.value(),
            e.getMessage(),
            LocalDateTime.now()
        ));
    }

    @ExceptionHandler(WebClientResponseException.class)
    public Mono<ResponseEntity<ErrorResponse>> handleWebClientError(
            WebClientResponseException e) {
        ErrorResponse error = new ErrorResponse(
            e.getStatusCode().value(),
            "Erreur lors de l'appel externe : " + e.getMessage(),
            LocalDateTime.now()
        );
        return Mono.just(ResponseEntity.status(e.getStatusCode()).body(error));
    }

    @ExceptionHandler(Exception.class)
    @ResponseStatus(HttpStatus.INTERNAL_SERVER_ERROR)
    public Mono<ErrorResponse> handleGenericError(Exception e) {
        return Mono.just(new ErrorResponse(
            500,
            "Erreur interne du serveur",
            LocalDateTime.now()
        ));
    }
}
```

---

## 9. Opérateurs avancés

### collectList et collectMap

```java
// Collecter tous les éléments d'un Flux dans une List (bloque la réactivité !)
Mono<List<User>> userList = userRepository.findAll()
    .collectList(); // à utiliser avec précaution, charge tout en mémoire

// Collecter dans une Map
Mono<Map<Long, User>> userMap = userRepository.findAll()
    .collectMap(User::getId);

// Collecter dans une Map multi-valeurs (MultiValueMap)
Mono<Map<String, List<User>>> byDomain = userRepository.findAll()
    .collectMultimap(user -> extractDomain(user.getEmail()));
```

### buffer et window

```java
// buffer : regroupe les éléments en listes de taille fixe
Flux<List<User>> batches = userRepository.findAll()
    .buffer(100); // lots de 100 utilisateurs (utile pour le batch processing)

// buffer par temps
Flux<List<Event>> byTime = eventFlux
    .bufferTimeout(50, Duration.ofSeconds(5)); // max 50 éléments ou 5 secondes

// window : comme buffer mais retourne des Flux au lieu de listes
Flux<Flux<User>> windows = userRepository.findAll()
    .window(100);
```

### Gestion du contexte réactif (Context)

Le `Context` de Reactor est l'équivalent réactif du `ThreadLocal`. Il permet de propager des informations (ex: `userId`, `traceId`) à travers la chaîne réactive.

```java
// Écrire dans le Context
Mono<User> withContext = userService.findById(1L)
    .contextWrite(ctx -> ctx.put("traceId", UUID.randomUUID().toString()));

// Lire le Context dans un opérateur
Mono<User> readContext = Mono.deferContextual(ctx -> {
    String traceId = ctx.get("traceId");
    log.info("TraceId: {}", traceId);
    return userService.findById(1L);
});
```

### Scheduler et gestion des threads

```java
// publishOn : change le thread à partir de ce point dans la chaîne
Flux<Data> result = dataFlux
    .publishOn(Schedulers.boundedElastic()) // threads pour I/O bloquant
    .map(data -> callBlockingApi(data));    // exécuté sur le scheduler ci-dessus

// subscribeOn : change le thread depuis la source
Mono<Data> fromBlocking = Mono.fromCallable(() -> blockingDatabaseCall())
    .subscribeOn(Schedulers.boundedElastic()); // source exécutée sur ce scheduler

// Les schedulers disponibles :
// Schedulers.parallel()         → threads CPU (N = nb de cœurs), pour calculs
// Schedulers.boundedElastic()   → threads I/O, taille variable, pour appels bloquants
// Schedulers.single()           → un seul thread réutilisé
// Schedulers.immediate()        → thread appelant (pas de changement)
```

> **Règle d'or** : si vous devez absolument appeler une API bloquante (JDBC, appel système), encapsulez-la dans `Mono.fromCallable(...)` avec `.subscribeOn(Schedulers.boundedElastic())` pour ne pas bloquer l'event loop de Netty.

---

## 10. Tests avec WebTestClient

### Test unitaire du Controller

```java
@WebFluxTest(UserController.class)
class UserControllerTest {

    @Autowired
    private WebTestClient webTestClient;

    @MockBean
    private UserService userService;

    @Test
    void shouldReturnUser_whenUserExists() {
        User mockUser = new User("Alice", "alice@exemple.com");
        mockUser.setId(1L);

        when(userService.findById(1L)).thenReturn(Mono.just(mockUser));

        webTestClient.get()
            .uri("/api/users/1")
            .exchange()
            .expectStatus().isOk()
            .expectBody(User.class)
            .value(user -> {
                assertThat(user.getName()).isEqualTo("Alice");
                assertThat(user.getEmail()).isEqualTo("alice@exemple.com");
            });
    }

    @Test
    void shouldReturn404_whenUserNotFound() {
        when(userService.findById(99L))
            .thenReturn(Mono.error(new UserNotFoundException(99L)));

        webTestClient.get()
            .uri("/api/users/99")
            .exchange()
            .expectStatus().isNotFound();
    }

    @Test
    void shouldReturnAllUsers() {
        List<User> users = List.of(
            new User("Alice", "alice@exemple.com"),
            new User("Bob", "bob@exemple.com")
        );

        when(userService.findAll()).thenReturn(Flux.fromIterable(users));

        webTestClient.get()
            .uri("/api/users")
            .exchange()
            .expectStatus().isOk()
            .expectBodyList(User.class)
            .hasSize(2)
            .contains(users.get(0), users.get(1));
    }

    @Test
    void shouldCreateUser() {
        User inputUser = new User("Charlie", "charlie@exemple.com");
        User savedUser = new User("Charlie", "charlie@exemple.com");
        savedUser.setId(3L);

        when(userService.create(any(User.class))).thenReturn(Mono.just(savedUser));

        webTestClient.post()
            .uri("/api/users")
            .bodyValue(inputUser)
            .exchange()
            .expectStatus().isCreated()
            .expectBody(User.class)
            .value(user -> assertThat(user.getId()).isEqualTo(3L));
    }
}
```

### Test avec StepVerifier (test de flux réactifs)

`StepVerifier` est l'outil principal de `reactor-test` pour tester des `Mono` et `Flux` de manière déclarative.

```java
import reactor.test.StepVerifier;

class UserServiceTest {

    private UserService userService;
    private UserRepository userRepository;

    @BeforeEach
    void setup() {
        userRepository = mock(UserRepository.class);
        userService = new UserService(userRepository);
    }

    @Test
    void shouldFindUserById() {
        User user = new User("Alice", "alice@exemple.com");
        when(userRepository.findById(1L)).thenReturn(Mono.just(user));

        StepVerifier.create(userService.findById(1L))
            .expectNextMatches(u -> u.getName().equals("Alice"))
            .verifyComplete(); // vérifie que le flux se termine sans erreur
    }

    @Test
    void shouldThrowWhenUserNotFound() {
        when(userRepository.findById(99L)).thenReturn(Mono.empty());

        StepVerifier.create(userService.findById(99L))
            .expectError(UserNotFoundException.class)
            .verify();
    }

    @Test
    void shouldReturnAllUsers() {
        List<User> users = List.of(
            new User("Alice", "alice@exemple.com"),
            new User("Bob", "bob@exemple.com"),
            new User("Charlie", "charlie@exemple.com")
        );
        when(userRepository.findAll()).thenReturn(Flux.fromIterable(users));

        StepVerifier.create(userService.findAll())
            .expectNextCount(3)
            .verifyComplete();
    }

    @Test
    void shouldTestWithVirtualTime() {
        // Test d'un flux qui émet toutes les secondes sans vraiment attendre
        StepVerifier.withVirtualTime(() -> Flux.interval(Duration.ofSeconds(1)).take(3))
            .thenAwait(Duration.ofSeconds(3)) // avance le temps virtuel de 3s
            .expectNext(0L, 1L, 2L)
            .verifyComplete();
    }
}
```

---

## 11. Bonnes pratiques & Pièges courants

### ✅ Bonnes pratiques

**1. Ne jamais bloquer dans la chaîne réactive**
```java
// ❌ MAUVAIS : block() dans un contexte réactif
public Mono<User> getUserBad(Long id) {
    User user = userRepository.findById(id).block(); // BLOQUE le thread Netty !
    return Mono.just(user);
}

// ✅ BON : rester dans la chaîne réactive
public Mono<User> getUserGood(Long id) {
    return userRepository.findById(id);
}
```

**2. Ne jamais subscribe() dans le service ou controller**
```java
// ❌ MAUVAIS : subscribe() dans le service
public void saveUser(User user) {
    userRepository.save(user).subscribe(); // fire-and-forget incontrôlable
}

// ✅ BON : retourner le Publisher, Spring s'occupe de la subscription
public Mono<User> saveUser(User user) {
    return userRepository.save(user);
}
```

**3. Préférer flatMap à map pour les opérations asynchrones**
```java
// ❌ MAUVAIS : map retourne un Mono<Mono<User>>
Flux<Mono<User>> wrong = userIds.map(id -> userRepository.findById(id));

// ✅ BON : flatMap aplatit en Flux<User>
Flux<User> correct = userIds.flatMap(id -> userRepository.findById(id));
```

**4. Limiter la concurrence dans flatMap**
```java
// ❌ Potentiellement illimité (peut saturer les connexions)
ids.flatMap(id -> webClient.get().uri("/data/{id}", id).retrieve().bodyToMono(Data.class));

// ✅ Limité à 10 appels simultanés
ids.flatMap(
    id -> webClient.get().uri("/data/{id}", id).retrieve().bodyToMono(Data.class),
    10 // concurrency parameter
);
```

**5. Utiliser switchIfEmpty pour les cas vides**
```java
// ❌ verbose
return userRepository.findById(id)
    .flatMap(user -> user != null ? Mono.just(user) : Mono.error(new NotFoundException()));

// ✅ idiomatique
return userRepository.findById(id)
    .switchIfEmpty(Mono.error(new UserNotFoundException(id)));
```

### ⚠️ Pièges courants

**1. Lazy evaluation : un Mono/Flux n'exécute rien tant qu'il n'est pas subscribed**
```java
// Ce code ne fait RIEN — personne ne subscribe !
Mono<User> mono = userRepository.save(user);
// Il faut retourner mono ou appeler .subscribe() (ou laisser Spring le faire)
```

**2. collectList() annule l'intérêt du streaming**
```java
// ❌ Charge TOUS les éléments en mémoire avant de répondre
return userRepository.findAll().collectList();

// ✅ Stream au client au fur et à mesure
return userRepository.findAll();
```

**3. Context perdu avec les opérations parallèles**
```java
// Le Context Reactor NE se propage PAS vers les CompletableFuture ou Threads externes
// Utiliser Mono.fromCallable + subscribeOn pour les opérations bloquantes
```

**4. Ne pas confondre map et flatMap**
```java
// map : T → R (transformation synchrone)
// flatMap : T → Publisher<R> (transformation asynchrone, aplatit le résultat)
```

### Checklist avant mise en production

- [ ] Aucun appel à `.block()` dans les couches service/controller
- [ ] Aucun appel à `.subscribe()` dans les couches service/controller
- [ ] Timeouts configurés sur le `WebClient`
- [ ] Retry avec backoff sur les appels externes critiques
- [ ] Gestion globale des erreurs (`@RestControllerAdvice`)
- [ ] Tests avec `StepVerifier` pour les flux complexes
- [ ] Concurrence limitée sur les `flatMap` vers des ressources externes
- [ ] Pas de `collectList()` sur des requêtes pouvant retourner de très grandes collections

---

## Récapitulatif

| Concept | Classe / Annotation | Description |
|---|---|---|
| Flux 0-1 élément | `Mono<T>` | Résultat unique ou vide |
| Flux 0-N éléments | `Flux<T>` | Stream de résultats |
| Client HTTP réactif | `WebClient` | Remplace `RestTemplate` |
| Test de flux | `StepVerifier` | Vérifie les Publisher |
| Test d'endpoint | `WebTestClient` | Teste les controllers |
| Scheduler I/O | `Schedulers.boundedElastic()` | Pour code bloquant |
| Scheduler CPU | `Schedulers.parallel()` | Pour calculs |
| Retry intelligent | `Retry.backoff(...)` | Réessai avec backoff exponentiel |
| Context propagation | `Mono.deferContextual()` | Remplace `ThreadLocal` |

---

*Références utiles :*
- [Documentation officielle Spring WebFlux](https://docs.spring.io/spring-framework/docs/current/reference/html/web-reactive.html)
- [Documentation Project Reactor](https://projectreactor.io/docs/core/release/reference/)
- [Guide Reactor — Opérateurs](https://projectreactor.io/docs/core/release/reference/#which-operator)




# Questions / Réponses orales — Spring WebFlux & Programmation Réactive

> 18 questions classées par thème et par difficulté : 🟢 Facile · 🟡 Moyen · 🔴 Difficile

---

## Fondamentaux

---

### Q1 🟢 — Qu'est-ce que la programmation réactive et pourquoi l'utilise-t-on ?

**Réponse :**

La programmation réactive est un paradigme de programmation asynchrone et non-bloquant basé sur des flux de données (*data streams*) et la propagation des changements. On l'utilise principalement pour gérer efficacement un grand nombre de requêtes I/O simultanées sans multiplier les threads.

Dans le modèle classique *thread-per-request*, chaque requête monopolise un thread pendant toute sa durée. Si ce thread attend une réponse de base de données ou d'une API externe, il est bloqué et ne peut rien faire d'autre. Cela force à avoir des pools de threads très larges, coûteux en mémoire.

Avec la programmation réactive, quand une opération I/O est déclenchée, le thread continue à traiter d'autres requêtes. Quand la réponse arrive (un événement), un *callback* reprend le traitement. Un petit nombre de threads peut ainsi gérer des milliers de requêtes simultanées.

---

### Q2 🟢 — Quelle est la différence entre un Mono et un Flux ?

**Réponse :**

Ce sont les deux types fondamentaux de Project Reactor :

`Mono<T>` représente un flux qui émet **au plus 1 élément** puis se termine, ou se termine en erreur. C'est l'équivalent réactif d'un `Optional` ou d'un `CompletableFuture`. On l'utilise pour les opérations qui retournent un résultat unique : trouver un utilisateur par son ID, sauvegarder un objet, etc.

`Flux<T>` représente un flux qui émet **0 à N éléments** puis se termine, ou se termine en erreur. C'est l'équivalent réactif d'une `List` ou d'un `Stream`, mais de manière asynchrone. On l'utilise pour retourner des collections, des listes paginées, des flux de données en temps réel, etc.

---

### Q3 🟡 — Qu'est-ce que la backpressure et pourquoi est-ce important ?

**Réponse :**

La *backpressure* est un mécanisme de contrôle de flux qui permet au *Subscriber* (consommateur) de signaler au *Publisher* (producteur) combien d'éléments il est capable de traiter à un instant donné.

Sans backpressure, un producteur rapide peut submerger un consommateur lent et provoquer des `OutOfMemoryError` car les éléments s'accumulent en mémoire.

Dans la spécification *Reactive Streams*, cela fonctionne via la méthode `request(n)` de l'interface `Subscription`. Le *Subscriber* demande explicitement N éléments, le *Publisher* n'en envoie pas plus. Quand le consommateur est prêt, il en redemande.

C'est l'un des piliers qui différencie les *Reactive Streams* d'un simple système de *callbacks* ou de *CompletableFuture*.

---

### Q4 🟢 — Qu'est-ce que Project Reactor et quel est son lien avec Spring WebFlux ?

**Réponse :**

Project Reactor est une implémentation de la spécification *Reactive Streams* développée par VMware (ex-Pivotal). Elle fournit les types `Mono` et `Flux` ainsi que des centaines d'opérateurs pour composer des pipelines réactifs de manière déclarative.

Spring WebFlux utilise Project Reactor comme fondation : les *controllers*, les *repositories* et le `WebClient` s'appuient directement sur `Mono` et `Flux`. WebFlux s'exécute par défaut sur **Netty**, un serveur *event-driven* et non-bloquant, bien qu'il puisse aussi fonctionner sur Tomcat ou Undertow.

---

### Q5 🟡 — Qu'est-ce que la lazy evaluation dans le contexte de Reactor ?

**Réponse :**

Dans Project Reactor, un `Mono` ou un `Flux` ne fait **rien** tant que personne ne s'y abonne (*subscribe*). On dit que les *publishers* sont *lazy* (paresseux), par opposition aux *eager* (impatients) comme les `CompletableFuture` qui démarrent immédiatement à leur création.

Concrètement, si on écrit `Mono<User> mono = userRepository.save(user)` sans jamais retourner ce `Mono` ni appeler `.subscribe()`, aucune requête en base n'est exécutée.

C'est pourquoi dans un *controller* Spring WebFlux, on **retourne** le `Mono` ou le `Flux` — c'est Spring qui se charge de la *subscription* au moment d'envoyer la réponse HTTP.

---

## Spring WebFlux

---

### Q6 🟢 — Quelle est la différence principale entre Spring MVC et Spring WebFlux ?

**Réponse :**

Spring MVC est basé sur les *Servlets* et utilise un modèle *thread-per-request* : chaque requête occupe un thread jusqu'à sa complétion. Quand le thread attend une réponse I/O, il est bloqué et ne peut pas servir d'autres requêtes. Ce modèle est simple mais pas très scalable sous forte charge I/O.

Spring WebFlux utilise un modèle *event-loop* non-bloquant, généralement avec Netty, et un petit nombre de threads. Les opérations I/O sont déléguées et le thread continue à traiter d'autres requêtes. WebFlux est idéal pour les applications avec beaucoup de concurrence I/O (API gateway, BFF, microservices), mais n'apporte pas de bénéfice pour des calculs CPU intensifs.

Il est important de noter qu'on ne peut pas mélanger du code bloquant (JDBC, appels synchrones) dans un pipeline WebFlux sans l'isoler explicitement sur un *Scheduler* dédié.

---

### Q7 🟡 — Comment implémenter un endpoint SSE (Server-Sent Events) avec Spring WebFlux ?

**Réponse :**

En utilisant `MediaType.TEXT_EVENT_STREAM_VALUE` dans l'annotation `@GetMapping` et en retournant un `Flux` :

```java
@GetMapping(value = "/stream", produces = MediaType.TEXT_EVENT_STREAM_VALUE)
public Flux<User> streamUsers() {
    return userService.findAll()
        .delayElements(Duration.ofSeconds(1));
}
```

Spring WebFlux maintient une connexion HTTP persistante ouverte avec le client et envoie chaque élément du `Flux` au fur et à mesure qu'il est émis. Le client (navigateur ou autre) reçoit les données en temps réel sans avoir à faire de *polling*. C'est idéal pour les notifications, les tableaux de bord live, les flux de logs, etc.

---

### Q8 🔴 — Qu'est-ce que le Context de Reactor et à quoi sert-il ?

**Réponse :**

Le `Context` de Reactor est l'équivalent réactif du `ThreadLocal`. Dans un pipeline réactif, l'exécution peut sauter d'un thread à l'autre, donc le `ThreadLocal` classique ne fonctionne pas : la valeur est perdue lors du changement de thread.

Le `Context` est un objet **immuable** propagé vers le haut de la chaîne (du *subscriber* vers la source). On l'utilise pour transporter des métadonnées transversales comme un `traceId`, un token de sécurité ou un `userId` à travers toute la chaîne réactive, sans les passer explicitement en paramètre de chaque méthode.

```java
// Écrire dans le Context
mono.contextWrite(ctx -> ctx.put("traceId", UUID.randomUUID().toString()));

// Lire le Context
Mono.deferContextual(ctx -> {
    String traceId = ctx.get("traceId");
    return doSomething(traceId);
});
```

---

## Opérateurs

---

### Q9 🟡 — Quelle est la différence entre map et flatMap ?

**Réponse :**

`map` est une transformation **synchrone** : elle transforme chaque élément `T` en un élément `R`. La fonction passée à `map` retourne directement une valeur, pas un *Publisher*.

`flatMap` est une transformation **asynchrone** : la fonction retourne un `Publisher<R>` (Mono ou Flux) que `flatMap` souscrit et aplatit pour obtenir directement les éléments `R`. C'est l'opérateur à utiliser dès que la transformation implique un appel asynchrone comme une requête en base de données ou un appel HTTP.

Une erreur classique est d'utiliser `map` avec une fonction qui retourne un `Mono`, ce qui produit un `Flux<Mono<R>>` — un flux de *publishers* jamais souscrits. Rien ne s'exécute.

---

### Q10 🟡 — Quelle est la différence entre flatMap et flatMapSequential ?

**Réponse :**

`flatMap` souscrit à tous les *inner publishers* **en parallèle** et émet les éléments dans l'ordre d'arrivée — le premier à répondre est le premier émis. L'ordre de sortie peut donc différer de l'ordre d'entrée.

`flatMapSequential` souscrit aussi en parallèle pour la performance, mais met en *buffer* les résultats pour les réémettre dans **l'ordre original d'entrée**. C'est le meilleur compromis quand l'ordre est important mais qu'on veut quand même la concurrence.

Il existe aussi `concatMap` qui souscrit séquentiellement (attend la fin de chaque *inner publisher* avant de passer au suivant) : pas de parallélisme, mais ordre garanti et pas de *buffering*.

---

### Q11 🟢 — À quoi sert l'opérateur switchIfEmpty ?

**Réponse :**

`switchIfEmpty` permet de définir une source de fallback si le `Mono` ou le `Flux` principal se termine **sans émettre d'élément** (flux vide). C'est l'opérateur idiomatique pour gérer les cas "not found" dans un *repository* réactif :

```java
return userRepository.findById(id)
    .switchIfEmpty(Mono.error(new UserNotFoundException(id)));
```

Sans `switchIfEmpty`, si l'utilisateur n'est pas trouvé, le `Mono` se termine silencieusement et Spring WebFlux renvoie une réponse 200 OK avec un body vide, ce qui n'est généralement pas le comportement souhaité.

---

### Q12 🔴 — Quand utiliser publishOn vs subscribeOn ?

**Réponse :**

`subscribeOn(Scheduler)` modifie le thread d'exécution depuis la **source**. Peu importe où on le place dans la chaîne, il affecte toujours le thread sur lequel la source commence à s'exécuter. Utilisé principalement pour exécuter du code bloquant sur `Schedulers.boundedElastic()` :

```java
Mono.fromCallable(() -> blockingDatabaseCall())
    .subscribeOn(Schedulers.boundedElastic());
```

`publishOn(Scheduler)` modifie le thread d'exécution **à partir de ce point** vers le bas dans la chaîne. Les opérateurs avant lui s'exécutent sur le thread précédent, ceux après sur le nouveau *Scheduler*. Utile pour séparer la partie I/O d'une partie CPU.

La règle d'or : pour isoler un appel bloquant dans un pipeline réactif, utiliser `Mono.fromCallable` + `subscribeOn(Schedulers.boundedElastic())`.

---

## WebClient

---

### Q13 🟢 — Pourquoi utiliser WebClient plutôt que RestTemplate ?

**Réponse :**

`RestTemplate` est synchrone et bloquant : l'appel HTTP bloque le thread appelant jusqu'à la réception complète de la réponse. Dans un contexte WebFlux, cela bloquerait le thread de l'*event loop* de Netty et annulerait tous les bénéfices du modèle réactif. Il est d'ailleurs officiellement déprécié depuis Spring 5.

`WebClient` est asynchrone et non-bloquant : il retourne un `Mono` ou `Flux` qui s'intègre naturellement dans un pipeline réactif. Aucun thread n'est bloqué pendant l'attente de la réponse. Il supporte en plus la *backpressure*, les *retry* avec *backoff*, les *timeouts* configurables, et une API *fluent* bien plus expressive.

---

### Q14 🟡 — Quelle est la différence entre retrieve() et exchangeToMono() dans WebClient ?

**Réponse :**

`retrieve()` est l'API simple et recommandée. Elle décode automatiquement le body et lance une `WebClientResponseException` pour les codes HTTP 4xx et 5xx. Convient à la grande majorité des cas.

`exchangeToMono()` (ou `exchangeToFlux()`) donne accès à la réponse complète — status code, headers, body — et laisse le développeur gérer lui-même la logique de traitement. À utiliser quand on a besoin de traitement conditionnel selon le status code, par exemple retourner un type différent selon que la réponse est 200 ou 206.

Attention avec `exchangeToMono()` : on est **responsable** de consommer ou libérer le body de la réponse. Si on ne le fait pas, on risque des fuites de ressources (connexions non libérées dans le pool Netty).

---

### Q15 🟡 — Comment effectuer plusieurs appels HTTP en parallèle avec WebClient ?

**Réponse :**

En combinant plusieurs `Mono` retournés par `WebClient` avec l'opérateur `Mono.zip()` :

```java
Mono<User> userMono = webClient.get().uri("/users/{id}", id)
    .retrieve().bodyToMono(User.class);

Mono<List<Order>> ordersMono = webClient.get().uri("/orders/{id}", id)
    .retrieve().bodyToFlux(Order.class).collectList();

return Mono.zip(userMono, ordersMono, UserProfile::new);
```

Les deux appels HTTP sont déclenchés simultanément. Le résultat final n'est produit que lorsque les deux ont répondu.

Pour N appels en parallèle sur une liste d'identifiants, on utilise `Flux.fromIterable(ids).flatMap(id -> ..., concurrencyLimit)`. Le deuxième paramètre de `flatMap` limite le nombre d'appels simultanés pour ne pas saturer le pool de connexions.

---

### Q16 🔴 — Comment configurer un retry avec backoff exponentiel sur un WebClient ?

**Réponse :**

En utilisant l'opérateur `retryWhen` combiné avec `Retry.backoff()` de Reactor :

```java
webClient.get().uri("/data")
    .retrieve()
    .bodyToMono(Data.class)
    .retryWhen(
        Retry.backoff(3, Duration.ofSeconds(1))
            .maxBackoff(Duration.ofSeconds(10))
            .jitter(0.5)
            .filter(e -> e instanceof WebClientResponseException.ServiceUnavailable)
            .onRetryExhaustedThrow((spec, signal) ->
                new ServiceUnavailableException("Service indisponible après 3 tentatives"))
    );
```

Cela effectue jusqu'à 3 nouvelles tentatives avec un délai exponentiel (1s, 2s, 4s), plafonné à 10s. Le paramètre `jitter` ajoute une part d'aléatoire pour éviter que tous les clients retentent exactement au même moment (*thundering herd*). On filtre sur le type d'erreur pour ne retry que les erreurs transitoires.

---

## Erreurs & Tests

---

### Q17 🟢 — Quels sont les opérateurs principaux de gestion d'erreur dans Reactor ?

**Réponse :**

Les opérateurs clés sont :

`onErrorReturn(value)` : retourne une valeur statique par défaut en cas d'erreur, le flux se termine normalement ensuite.

`onErrorResume(fn)` : bascule vers un *Publisher* de fallback. C'est le plus flexible car le fallback peut lui-même faire un appel asynchrone.

`onErrorMap(fn)` : transforme l'erreur en une autre exception sans la gérer, utile pour traduire des exceptions techniques en exceptions métier.

`doOnError(fn)` : effet de bord (log, métriques) sans modifier l'erreur. L'erreur continue de se propager normalement.

`retry(n)` : réessaie N fois immédiatement.

`retryWhen(spec)` : réessaie selon une spécification avancée — *backoff* exponentiel, filtre sur le type d'erreur, nombre maximum de tentatives.

---

### Q18 🟡 — Comment tester un pipeline réactif avec StepVerifier ?

**Réponse :**

`StepVerifier` est la classe principale de `reactor-test` pour tester des `Mono` et `Flux` de façon **déclarative et synchrone**. On décrit les étapes attendues du flux, puis on appelle `verify()` ou `verifyComplete()` pour déclencher la *subscription* et vérifier :

```java
StepVerifier.create(userService.findById(1L))
    .expectNextMatches(u -> u.getName().equals("Alice"))
    .verifyComplete();

StepVerifier.create(userService.findById(99L))
    .expectError(UserNotFoundException.class)
    .verify();
```

Pour tester des flux temporels (ex: `Flux.interval`) sans vraiment attendre, on utilise `StepVerifier.withVirtualTime()` combiné à `.thenAwait(Duration)` qui avance une horloge virtuelle sans bloquer le test.

Pour les *controllers*, on utilise `WebTestClient` qui permet de faire de vraies requêtes HTTP contre le *controller* en test :

```java
webTestClient.get().uri("/api/users/1")
    .exchange()
    .expectStatus().isOk()
    .expectBody(User.class)
    .value(user -> assertThat(user.getName()).isEqualTo("Alice"));
```

---

### Q19 🔴 — Quels sont les pièges les plus courants à éviter avec Spring WebFlux ?

**Réponse :**

Les trois pièges principaux à connaître absolument :

**1. Appeler `.block()` dans la chaîne réactive.** `block()` bloque le thread appelant jusqu'à la complétion du *publisher*. Si ce thread est un thread Netty de l'*event loop*, on bloque toute la gestion des requêtes et on détruit les performances. `block()` n'est acceptable que dans un `main()` ou dans un test.

**2. Appeler `.subscribe()` dans les services ou controllers.** Cela crée un effet *fire-and-forget* incontrôlable : les erreurs sont silencieuses, la *backpressure* est ignorée. Spring WebFlux gère lui-même la *subscription* quand le *controller* retourne un `Mono` ou `Flux`.

**3. Utiliser `map` au lieu de `flatMap` pour des opérations asynchrones.** Cela produit un `Flux<Mono<T>>` — un flux de *publishers* jamais souscrits. Rien ne s'exécute, le développeur obtient un résultat vide sans message d'erreur.

Un quatrième piège fréquent : appeler `collectList()` sur un grand `Flux` sans y réfléchir, ce qui charge tous les éléments en mémoire et annule le bénéfice du *streaming*.
