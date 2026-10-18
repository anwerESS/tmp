# Questions d'Entretien Spring Boot - Guide Complet

Je vais vous préparer avec un maximum de questions sur Spring Boot !

## 🌱 FONDAMENTAUX SPRING BOOT

**Q1: Qu'est-ce que Spring Boot ?**
Framework Java basé sur Spring pour créer applications standalone rapidement. Auto-configuration, serveur embarqué, gestion dépendances simplifiée via starters, convention over configuration. Élimine la configuration XML complexe.

**Q2: Différence entre Spring et Spring Boot ?**
Spring : framework complet, configuration manuelle (XML/annotations), serveur externe nécessaire. Spring Boot : sur Spring, auto-configuration, embedded server, starters, production-ready features, CLI, moins de boilerplate.

**Q3: Quels sont les avantages de Spring Boot ?**
Démarrage rapide, auto-configuration intelligente, starters pour dépendances, serveur embarqué (Tomcat/Jetty/Undertow), actuators pour monitoring, pas de XML, deployment simplifié (JAR exécutable), DevTools pour rechargement.

**Q4: Qu'est-ce qu'un starter Spring Boot ?**
Dépendance Maven/Gradle regroupant bibliothèques liées. Exemples : spring-boot-starter-web (web apps), -data-jpa (JPA), -security (sécurité), -test (tests). Gère versions compatibles automatiquement.

**Q5: Qu'est-ce que l'auto-configuration ?**
Mécanisme configurant automatiquement beans selon classpath. Annotations conditionnelles (@ConditionalOnClass, @ConditionalOnMissingBean). Fichier META-INF/spring.factories. Peut être désactivée/personnalisée.

**Q6: Qu'est-ce que @SpringBootApplication ?**
Annotation composite combinant : @Configuration (classe config), @EnableAutoConfiguration (active auto-config), @ComponentScan (scan packages). Point d'entrée application avec main().

**Q7: Comment fonctionne SpringApplication.run() ?**
Démarre ApplicationContext, charge configuration, active auto-configuration, démarre serveur embarqué, scanne composants, injecte dépendances. Retourne ConfigurableApplicationContext.

**Q8: Qu'est-ce qu'un ApplicationContext ?**
Conteneur IoC central de Spring. Gère beans, lifecycle, injection dépendances, événements. Types : AnnotationConfigApplicationContext, WebApplicationContext. Accessible via @Autowired.

**Q9: Comment personnaliser le port du serveur ?**
application.properties : `server.port=8081` ou application.yml. Ligne commande : `--server.port=8081`. Programmatique : WebServerFactoryCustomizer.

**Q10: Différence entre application.properties et application.yml ?**
Properties : format clé=valeur, plat. YAML : format hiérarchique, lisible, supporte listes/maps, indentation importante. Même fonctionnalités, préférence personnelle.

## 🎯 DEPENDENCY INJECTION & IoC

**Q11: Qu'est-ce que l'IoC (Inversion of Control) ?**
Principe où framework contrôle flux et création objets, pas le développeur. Spring gère instanciation et injection dépendances. Découple composants, testabilité.

**Q12: Qu'est-ce que la Dependency Injection ?**
Pattern pour fournir dépendances à objet depuis l'extérieur. Types : constructor, setter, field injection. Spring injecte via @Autowired, constructeur préféré.

**Q13: Différence entre @Autowired, @Inject et @Resource ?**
@Autowired : Spring, par type puis nom, required=false possible. @Inject : JSR-330 standard, par type. @Resource : JSR-250, par nom puis type. Préférer @Autowired ou constructeur.

**Q14: Quelle injection privilégier (constructor vs field vs setter) ?**
Constructor : recommandé, immutabilité, testabilité, dépendances obligatoires évidentes, pas de @Autowired nécessaire (Spring 4.3+). Field : concis mais non testable. Setter : dépendances optionnelles.

**Q15: Comment résoudre ambiguïté avec plusieurs beans du même type ?**
@Primary (bean par défaut), @Qualifier("nomBean"), @Profile, injection List<Type> (tous beans), nommer bean spécifiquement, conditional annotations.

**Q16: Qu'est-ce que @Qualifier ?**
Spécifie quel bean injecter en cas d'ambiguïté. Utilisé avec @Autowired. Exemple : @Qualifier("mysqlRepo"). Alternative : nom du bean dans @Service/@Repository.

**Q17: Qu'est-ce que @Primary ?**
Marque bean comme prioritaire quand plusieurs du même type. Utilisé sans @Qualifier. Un seul @Primary par type recommandé.

## 🔧 BEANS & CONFIGURATION

**Q18: Qu'est-ce qu'un Bean Spring ?**
Objet géré par conteneur Spring IoC. Instancié, assemblé, géré par Spring. Déclaré via @Component, @Service, @Repository, @Controller ou @Bean dans @Configuration.

**Q19: Différence entre @Component, @Service, @Repository et @Controller ?**
@Component : stéréotype générique. @Service : couche métier, logique business. @Repository : couche données, gestion exceptions SQL. @Controller : couche web, MVC. Sémantique différente, techniquement similaires.

**Q20: Qu'est-ce que @Configuration et @Bean ?**
@Configuration : classe contenant définitions beans. @Bean : méthode produisant bean géré par Spring. Alternative à XML. Utile pour bibliothèques tierces.

**Q21: Différence entre @Component et @Bean ?**
@Component : sur classe, auto-détection via component scan. @Bean : sur méthode dans @Configuration, contrôle manuel instanciation, utile pour classes externes.

**Q22: Quels sont les scopes des beans ?**
- Singleton (défaut) : une instance par conteneur
- Prototype : nouvelle instance à chaque demande
- Request : instance par requête HTTP (web)
- Session : instance par session HTTP
- Application : instance par ServletContext
- WebSocket : instance par WebSocket

**Q23: Différence entre Singleton et Prototype ?**
Singleton : partagé, state dangereux en multithread, performance. Prototype : nouvelle instance, state isolé, overhead création. @Scope("prototype") pour activer.

**Q24: Comment injecter bean Prototype dans Singleton ?**
Problème : prototype créé une fois. Solutions : @Lookup method injection, ObjectFactory<T>, ApplicationContext.getBean(), @Scope avec proxyMode.

**Q25: Qu'est-ce que @Lazy ?**
Initialise bean à la première utilisation au lieu du démarrage. Réduit temps démarrage, utile pour beans coûteux. Peut cacher erreurs configuration.

**Q26: Qu'est-ce que le lifecycle d'un bean ?**
1. Instanciation, 2. Injection dépendances, 3. setBeanName (BeanNameAware), 4. setBeanFactory, 5. post-processor before init, 6. @PostConstruct/InitializingBean, 7. init-method, 8. post-processor after init, 9. Bean ready, 10. @PreDestroy/DisposableBean, 11. destroy-method.

**Q27: Qu'est-ce que @PostConstruct et @PreDestroy ?**
@PostConstruct : méthode exécutée après injection dépendances, initialisation. @PreDestroy : avant destruction bean, cleanup. JSR-250 annotations. Alternative : InitializingBean/DisposableBean.

**Q28: Comment injecter valeurs depuis application.properties ?**
@Value("${property.name}") : injection simple. @ConfigurationProperties(prefix="app") : binding objet complet avec validation. Environment.getProperty().

**Q29: Qu'est-ce que @ConfigurationProperties ?**
Bind propriétés externes à POJO. Type-safe, validation JSR-303, auto-complétion IDE. Nécessite @EnableConfigurationProperties ou @Component. Préférable à multiples @Value.

**Q30: Comment activer validation sur @ConfigurationProperties ?**
Ajouter spring-boot-starter-validation. Annotations JSR-303 : @NotNull, @Min, @Max, @Pattern. @Validated sur classe. Échec démarrage si invalide.

## 🌐 SPRING MVC & REST

**Q31: Qu'est-ce que Spring MVC ?**
Framework web basé sur pattern MVC. DispatcherServlet central, Controllers traitent requêtes, Views rendent réponses. Support REST, validation, data binding.

**Q32: Comment fonctionne DispatcherServlet ?**
Front Controller recevant toutes requêtes. Délègue à HandlerMapping (trouve controller), HandlerAdapter (exécute), ViewResolver (résout vue), ExceptionHandler. Central dans Spring MVC.

**Q33: Différence entre @Controller et @RestController ?**
@Controller : retourne vues (HTML), nécessite @ResponseBody pour JSON. @RestController : combine @Controller + @ResponseBody, retourne directement objets sérialisés JSON/XML. Pour APIs REST.

**Q34: Qu'est-ce que @RequestMapping ?**
Map requêtes HTTP à méthodes controller. Attributs : path, method (GET/POST/PUT/DELETE), params, headers, consumes, produces. Variants : @GetMapping, @PostMapping, etc.

**Q35: Différence entre @RequestMapping et @GetMapping/@PostMapping ?**
@RequestMapping : générique, spécifier method. @GetMapping/@PostMapping/@PutMapping/@DeleteMapping/@PatchMapping : raccourcis spécialisés, plus lisibles. Préférer variants.

**Q36: Qu'est-ce que @PathVariable ?**
Extrait valeur depuis URI template. Exemple : @GetMapping("/users/{id}") avec @PathVariable Long id. URI : /users/123 → id=123. Optional avec required=false.

**Q37: Différence entre @PathVariable et @RequestParam ?**
@PathVariable : partie URI (/users/123). @RequestParam : query parameter (/users?id=123). PathVariable pour ressources RESTful, RequestParam pour filtres/options.

**Q38: Qu'est-ce que @RequestBody ?**
Désérialise corps requête HTTP en objet Java. Utilisé pour POST/PUT avec JSON/XML. Jackson/JAXB gère conversion. Validation avec @Valid.

**Q39: Qu'est-ce que @ResponseBody ?**
Sérialise objet retour en corps réponse HTTP. Conversion automatique JSON/XML. Inclus dans @RestController. Pas nécessaire si @RestController.

**Q40: Qu'est-ce que ResponseEntity ?**
Représente réponse HTTP complète : body, status, headers. Contrôle précis réponse. Exemple : ResponseEntity.ok(body), ResponseEntity.status(201).body(created), ResponseEntity.notFound().build().

**Q41: Différence entre @ResponseBody et ResponseEntity ?**
@ResponseBody : annotation, sérialise retour, status 200 par défaut. ResponseEntity : classe, contrôle total (body + status + headers). ResponseEntity plus flexible.

**Q42: Comment gérer les exceptions globalement ?**
@ControllerAdvice ou @RestControllerAdvice : classe gérant exceptions de tous controllers. @ExceptionHandler(Exception.class) : méthode par type exception. Retourne ResponseEntity avec détails erreur.

**Q43: Qu'est-ce que @ExceptionHandler ?**
Méthode gérant exception spécifique dans controller ou @ControllerAdvice. Paramètre : exception. Retour : ResponseEntity, ModelAndView, ou vue. Évite try-catch répétitif.

**Q44: Comment valider les données d'entrée ?**
Ajouter spring-boot-starter-validation. Annotations sur DTOs : @NotNull, @NotBlank, @Size, @Email, @Min, @Max, @Pattern. @Valid sur @RequestBody. MethodArgumentNotValidException si échec.

**Q45: Qu'est-ce que @Valid vs @Validated ?**
@Valid : JSR-303, validation cascade objets imbriqués, sur paramètres/attributs. @Validated : Spring, support validation groups, sur classes/paramètres. @Validated pour groupes, sinon @Valid.

**Q46: Comment gérer CORS dans Spring Boot ?**
Global : @CrossOrigin sur controllers/méthodes. Configuration : WebMvcConfigurer.addCorsMappings(). Properties : management.endpoints.web.cors.*. Production : proxy/gateway préférable.

**Q47: Qu'est-ce que Content Negotiation ?**
Détermine format réponse selon Accept header. Spring supporte JSON, XML, etc. Configurable via produces dans @RequestMapping. ContentNegotiationConfigurer.

## 💾 SPRING DATA JPA

**Q48: Qu'est-ce que Spring Data JPA ?**
Abstraction sur JPA simplifiant accès données. Repositories avec méthodes générées automatiquement. Query methods, @Query, specifications. Réduit boilerplate CRUD.

**Q49: Qu'est-ce qu'une Entity ?**
Classe représentant table base données. Annotée @Entity. @Id pour clé primaire. @Table(name="...") optionnel. Attributs = colonnes. Gérée par EntityManager.

**Q50: Annotations principales pour Entity ?**
@Entity (marque entité), @Table (nom table), @Id (PK), @GeneratedValue (auto-increment), @Column (config colonne), @Transient (ignore), @Temporal (dates), @Enumerated (enums).

**Q51: Différence entre @Id et @GeneratedValue ?**
@Id : marque attribut comme clé primaire. @GeneratedValue : stratégie génération valeur (AUTO, IDENTITY, SEQUENCE, TABLE). Souvent ensemble.

**Q52: Stratégies de @GeneratedValue ?**
- AUTO : JPA choisit (défaut)
- IDENTITY : auto-increment DB (MySQL)
- SEQUENCE : séquence DB (PostgreSQL, Oracle)
- TABLE : table génératrice
IDENTITY populaire mais SEQUENCE meilleur performance batch.

**Q53: Qu'est-ce qu'un Repository ?**
Interface d'accès données. Extends JpaRepository, CrudRepository ou PagingAndSortingRepository. Spring génère implémentation. Méthodes CRUD prêtes : save, findById, findAll, delete.

**Q54: Différence entre JpaRepository, CrudRepository et PagingAndSortingRepository ?**
CrudRepository : CRUD basique. PagingAndSortingRepository : + pagination/tri. JpaRepository : + batch operations (saveAll, flush, deleteInBatch), méthodes JPA. JpaRepository recommandé.

**Q55: Qu'est-ce que les Query Methods ?**
Méthodes repository générées depuis nom méthode. Exemple : findByNameAndAge(String name, int age) → SELECT * FROM ... WHERE name=? AND age=?. Conventions : findBy, countBy, deleteBy, existsBy.

**Q56: Mots-clés des Query Methods ?**
And, Or, Between, LessThan, GreaterThan, Like, In, NotIn, OrderBy, IgnoreCase, IsNull, IsNotNull, True, False. Exemple : findByAgeGreaterThanAndNameLike.

**Q57: Qu'est-ce que @Query ?**
Définit requête JPQL/SQL personnalisée. @Query("SELECT u FROM User u WHERE u.email = ?1") ou nommé :email. nativeQuery=true pour SQL natif. Plus contrôle que query methods.

**Q58: Différence entre JPQL et SQL natif ?**
JPQL : requêtes sur entités/attributs Java, portable, orienté objet. SQL natif : requêtes sur tables/colonnes DB, spécifique base, performance optimisée. Préférer JPQL sauf besoin spécifique.

**Q59: Qu'est-ce que @Modifying ?**
Avec @Query pour UPDATE/DELETE. @Modifying @Query("UPDATE User u SET u.status = ?1"). Nécessite @Transactional. clearAutomatically=true vide persistence context.

**Q60: Comment gérer les relations entre entities ?**
@OneToOne, @OneToMany, @ManyToOne, @ManyToMany. Attributs : mappedBy (côté inverse), cascade (propagation opérations), fetch (LAZY/EAGER), orphanRemoval. @JoinColumn configure FK.

**Q61: Différence entre @OneToMany et @ManyToOne ?**
@OneToMany : côté "un" (parent), collection. @ManyToOne : côté "plusieurs" (enfant), référence. Souvent ensemble, bidirectionnels avec mappedBy sur @OneToMany.

**Q62: Qu'est-ce que mappedBy ?**
Côté inverse relation bidirectionnelle, ne possède pas FK. Exemple : @OneToMany(mappedBy="user") signifie User ne gère pas FK, c'est l'autre entité. Évite duplicata.

**Q63: Différence entre FetchType.LAZY et EAGER ?**
LAZY : charge relation à la demande (défaut pour collections). EAGER : charge immédiatement (défaut pour @ManyToOne/@OneToOne). LAZY préférable, évite N+1 problem avec JOIN FETCH.

**Q64: Qu'est-ce que le N+1 problem ?**
Problème performance : requête initiale + requête par élément relation. 1 requête entités + N requêtes relations = N+1. Solutions : JOIN FETCH, @EntityGraph, Batch Fetching.

**Q65: Comment résoudre le N+1 problem ?**
@Query avec JOIN FETCH, @EntityGraph(attributePaths={"relation"}), @BatchSize, initialiser collection dans transaction, DTO projections. Monitoring avec show-sql.

**Q66: Qu'est-ce que @EntityGraph ?**
Définit quelles relations charger. Alternative JOIN FETCH. @EntityGraph(attributePaths={"orders", "orders.items"}). Type : FETCH (seulement spécifiées), LOAD (+ eager par défaut).

**Q67: Qu'est-ce que CascadeType ?**
Propage opérations parent → enfants. ALL (toutes), PERSIST (save), MERGE (update), REMOVE (delete), REFRESH, DETACH. cascade={CascadeType.PERSIST, CascadeType.REMOVE}.

**Q68: Différence entre CascadeType.REMOVE et orphanRemoval ?**
REMOVE : supprime enfants si parent supprimé. orphanRemoval : supprime enfants si retirés de collection parent. orphanRemoval plus strict, implique REMOVE.

**Q69: Qu'est-ce qu'une transaction ?**
Unité de travail atomique : tout réussit ou tout échoue (ACID). @Transactional gère begin, commit, rollback automatiquement. Propagation, isolation configurables.

**Q70: Qu'est-ce que @Transactional ?**
Active gestion transactionnelle. Niveau classe ou méthode. Rollback sur RuntimeException par défaut. Attributs : propagation, isolation, timeout, readOnly, rollbackFor.

**Q71: Niveaux de propagation des transactions ?**
- REQUIRED (défaut) : rejoint existante ou crée
- REQUIRES_NEW : suspend existante, crée nouvelle
- MANDATORY : requiert existante, erreur sinon
- SUPPORTS : optionnel
- NOT_SUPPORTED : suspend existante
- NEVER : erreur si existante
- NESTED : sauvegarde si existante

**Q72: Niveaux d'isolation des transactions ?**
- READ_UNCOMMITTED : dirty reads
- READ_COMMITTED : évite dirty reads
- REPEATABLE_READ : évite non-repeatable reads
- SERIALIZABLE : complet, lent
Défaut dépend DB. Compromis cohérence/performance.

**Q73: Qu'est-ce que Pagination et Sorting ?**
Pageable paramètre : page, size, sort. Méthode repository retourne Page<T> ou Slice<T>. Exemple : findAll(PageRequest.of(0, 10, Sort.by("name"))). Page contient totalElements, Slice non.

**Q74: Différence entre Page et Slice ?**
Page : compte total (requête COUNT), hasNext/hasPrevious, navigation complète. Slice : pas de count, seulement hasNext, performance. Slice si count inutile.

**Q75: Qu'est-ce que les Projections ?**
Requêtes retournant subset attributs. Interface projection : méthodes getters. Classe projection : DTO. @Value("#{target.firstName + ' ' + target.lastName}"). Réduit données transférées.

**Q76: Qu'est-ce que les Specifications ?**
Criteria API type-safe pour requêtes dynamiques. Implement Specification<T>, méthode toPredicate(). Combinable avec and(), or(). Utile pour filtres complexes conditionnels.

**Q77: Qu'est-ce que les Auditing ?**
Traçabilité : qui/quand créé/modifié. @EnableJpaAuditing, @EntityListeners(AuditingEntityListener.class). Annotations : @CreatedDate, @LastModifiedDate, @CreatedBy, @LastModifiedBy. AuditorAware pour utilisateur.

## 🔐 SPRING SECURITY

**Q78: Qu'est-ce que Spring Security ?**
Framework sécurité complet. Authentification (qui êtes-vous ?), autorisation (que pouvez-vous faire ?), protection CSRF, session management, encryption. Configurable via SecurityFilterChain.

**Q79: Comment fonctionne Spring Security ?**
Chaîne de filtres (FilterChainProxy). DelegatingFilterProxy intercepte requêtes. Filtres : Authentication, Authorization, CSRF, Session, CORS. SecurityContext stocke authentification.

**Q80: Comment configurer Spring Security ?**
Classe @Configuration avec SecurityFilterChain bean. http.authorizeHttpRequests(), formLogin(), httpBasic(), csrf(). PasswordEncoder bean pour cryptage. UserDetailsService pour utilisateurs.

**Q81: Qu'est-ce que UserDetailsService ?**
Interface chargeant utilisateur par username. Méthode loadUserByUsername() retourne UserDetails. Implémentation custom ou JPA repository. Utilisé par AuthenticationProvider.

**Q82: Qu'est-ce que UserDetails ?**
Interface représentant utilisateur authentifié. Méthodes : getUsername(), getPassword(), getAuthorities(), isEnabled(), isAccountNonExpired(). Implémentation : User class ou custom.

**Q83: Différence entre Authentication et Authorization ?**
Authentication : vérifier identité (login). Authorization : vérifier permissions (accès ressources). Authentication avant authorization.

**Q84: Comment encoder les mots de passe ?**
PasswordEncoder interface. Implémentations : BCryptPasswordEncoder (recommandé), Pbkdf2PasswordEncoder, SCryptPasswordEncoder. Bean : @Bean PasswordEncoder passwordEncoder() { return new BCryptPasswordEncoder(); }

**Q85: Qu'est-ce que @PreAuthorize et @PostAuthorize ?**
Sécurité méthode via SpEL. @EnableGlobalMethodSecurity(prePostEnabled=true). @PreAuthorize("hasRole('ADMIN')") avant exécution. @PostAuthorize vérifie après, filtre retour. @Secured alternative.

**Q86: Différence entre hasRole() et hasAuthority() ?**
hasRole('ADMIN') cherche "ROLE_ADMIN". hasAuthority('ADMIN') cherche "ADMIN". Roles automatiquement préfixés ROLE_. Authorities flexibles. Roles = groupes authorities.

**Q87: Qu'est-ce que JWT ?**
JSON Web Token. Token autosuffisant contenant claims (payload), signé. Stateless authentication. Structure : header.payload.signature (Base64). Pas de session serveur.

**Q88: Comment implémenter JWT avec Spring Security ?**
Dépendance jjwt. Filtre JwtAuthenticationFilter extrait/valide token. JwtUtil génère/parse tokens. SecurityFilterChain configure filter avant UsernamePasswordAuthenticationFilter. Stateless session.

**Q89: Qu'est-ce que CSRF et comment le gérer ?**
Cross-Site Request Forgery. Attaque soumettant requête avec cookies victime. Spring génère token CSRF, vérifie sur POST/PUT/DELETE. Désactiver pour APIs stateless : csrf().disable().

**Q90: Qu'est-ce que CORS avec Spring Security ?**
Cross-Origin Resource Sharing. Sécurité navigateur. Spring Security filtre CORS. Configuration : http.cors(). CorsConfigurationSource bean définit origines, méthodes, headers autorisés.

## ⚙️ CONFIGURATION & PROPERTIES

**Q91: Comment externaliser la configuration ?**
application.properties/yml, variables environnement, arguments ligne commande, Config Server. Ordre priorité : commande > env > fichier. @PropertySource pour fichiers custom.

**Q92: Qu'est-ce que les Profiles Spring ?**
Configurations environnement-spécifiques (dev, test, prod). application-{profile}.properties. Activer : spring.profiles.active=prod. @Profile sur beans. Permet configurations conditionnelles.

**Q93: Comment activer un profile ?**
application.properties : spring.profiles.active=dev,prod. Ligne commande : --spring.profiles.active=dev. Variable env : SPRING_PROFILES_ACTIVE. IDE run configuration.

**Q94: Qu'est-ce que @Profile ?**
Conditional registration beans selon profile actif. @Profile("dev") sur classe/méthode @Bean. Logique : AND implicite, OR avec tableau, NOT avec "!". Exemple : @Profile({"dev", "test"}).

**Q95: Comment gérer secrets/mots de passe ?**
Variables environnement (12-factor app), Vault, AWS Secrets Manager, Azure Key Vault, Config Server encrypted. Jasypt pour encryption. Jamais commit secrets dans code/repo.

**Q96: Qu'est-ce que Spring Cloud Config ?**
Serveur configuration centralisé. Stockage Git/filesystem. Clients récupèrent config au démarrage. Refresh dynamique via /actuator/refresh. Encryption/decryption secrets. Versionning config.

**Q97: Comment recharger configuration sans restart ?**
@RefreshScope sur beans. Actuator endpoint /actuator/refresh. POST requête déclenche reload. Alternative : Config Server avec bus refresh. Attention : pas tous beans (singletons initialisés).

## 📊 ACTUATOR & MONITORING

**Q98: Qu'est-ce que Spring Boot Actuator ?**
Production-ready features : health checks, metrics, info, env. Endpoints HTTP/JMX. /actuator/health, /metrics, /info, /loggers, /beans, /mappings. Monitoring et management application.

**Q99: Endpoints Actuator principaux ?**
- /health : status application
- /metrics : métriques (CPU, mémoire, requêtes)
- /info : infos app custom
- /env : propriétés environnement
- /loggers : niveaux logs
- /httptrace : requêtes HTTP récentes
- /threaddump : threads JVM
- /heapdump : heap dump

**Q100: Comment sécuriser Actuator endpoints ?**
Spring Security : autoriser seulement ADMIN. management.endpoints.web.exposure.include=health,info. management.endpoint.health.show-details=when-authorized. Filtrer IPs, VPN, ne pas exposer publiquement.

**Q101: Comment personnaliser /health endpoint ?**
Implémenter HealthIndicator. Méthode health() retourne Health (UP/DOWN/OUT_OF_SERVICE). Custom status. Composite de plusieurs indicators. AbstractHealthIndicator pour facilité.

**Q102: Comment ajouter custom metrics ?**
MeterRegistry autowired. Counter, Timer, Gauge. Exemple : meterRegistry.counter("custom.metric", "tag", "value").increment(). Micrometer abstraction, backend (Prometheus, Graphite).

**Q103: Qu'est-ce que Micrometer ?**
Abstraction metrics dimensionnelles. Backend-agnostic (Prometheus, Graphite, Datadog, New Relic). Intégré Spring Boot. Types : Counter, Gauge, Timer, DistributionSummary. Tags pour dimensions.

## 🧪 TESTING

**Q104: Comment tester avec Spring Boot ?**
spring-boot-starter-test : JUnit 5, Mockito, AssertJ, Hamcrest. @SpringBootTest charge ApplicationContext complet. @WebMvcTest pour controllers. @DataJpaTest pour repositories. MockMvc pour tests web.

**Q105: Qu'est-ce que @SpringBootTest ?**
Charge ApplicationContext complet, tests d'intégration. webEnvironment : MOCK (défaut), RANDOM_PORT, DEFINED_PORT, NONE. Lent mais teste vraie config. @TestConfiguration pour overrides.

**Q106: Différence entre @SpringBootTest et @WebMvcTest ?**
@SpringBootTest : contexte complet, tous beans, lent. @WebMvcTest : seulement MVC (controllers), auto-configure MockMvc, mock services, rapide. WebMvcTest pour tests controllers isolés.

**Q107: Qu'est-ce que @DataJpaTest ?**
Tests repositories JPA isolés. Configure H2 en mémoire, transaction rollback auto. Seulement beans JPA chargés. TestEntityManager pour setup. Rapide, pas de services/controllers.

**Q108: Qu'est-ce que MockMvc ?**
Simuler requêtes HTTP sans serveur. perform(get("/api/users")), andExpect(status().isOk()), andExpect(jsonPath("$.name").value("John")). Auto-configuré avec @WebMvcTest ou autowired.

**Q109: Comment mocker des beans dans tests ?**
@MockBean : remplace bean dans contexte par mock Mockito. @SpyBean : spy sur bean réel. when(service.method()).thenReturn(). Verify interactions. Utile isoler unités.

**Q110: Qu'est-ce que @TestConfiguration ?**
Configuration test spécifique. Override beans production. Détectée automatiquement par @SpringBootTest ou import explicite. Permet mock beans, custom config tests.

**Q111: Comment tester avec base de données en mémoire ?**
H2 dependency (scope test). Crée DB automatiquement avec @DataJpaTest. spring.datasource.url=jdbc:h2:mem:testdb. Import schema.sql/data.sql. Lightweight, rapide, reset entre tests.

**Q112: Comment tester APIs REST ?**
MockMvc ou TestRestTemplate/@SpringBootTest avec RANDOM_PORT. RestAssured pour BDD. given().when().then() syntax. Assert status, body, headers. Integration tests end-to-end.

**Q113: Qu'est-ce que @Sql et @SqlGroup ?**
Exécute scripts SQL avant test. @Sql("/data.sql") ou @Sql(scripts="...", executionPhase=BEFORE/AFTER). @SqlGroup pour plusieurs. Utile setup données tests.

## 🚀 DEPLOYMENT & BUILD

**Q114: Comment packager application Spring Boot ?**
Maven : mvn clean package produit JAR exécutable (ou WAR). spring-boot-maven-plugin repackage avec dépendances embarquées. Gradle : bootJar task. JAR contient Manifest pointant Main.

**Q115: Différence entre JAR et WAR ?**
JAR : standalone, serveur embarqué (Tomcat/Jetty), java -jar app.jar. WAR : déploiement serveur externe (Tomcat, WildFly), extends SpringBootServletInitializer. JAR recommandé, moderne.

**Q116: Comment créer image Docker pour Spring Boot ?**
Dockerfile : FROM openjdk:17, COPY jar, EXPOSE port, ENTRYPOINT java -jar. Layered JARs pour cache. spring-boot:build-image (Buildpacks) sans Dockerfile. Multi
























































<br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br />








