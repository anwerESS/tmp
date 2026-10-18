# Questions d'Entretien - Unit Testing & Cucumber

## 🧪 UNIT TESTING AVANCÉ

**Q117: Qu'est-ce que le Test-Driven Development (TDD) ?**
Méthodologie : écrire test avant code. Cycle Red-Green-Refactor : 1) Test échoue (Red), 2) Code minimum pour passer (Green), 3) Améliorer code (Refactor). Garantit testabilité, meilleure conception, documentation vivante.

**Q118: Différence entre Unit Test, Integration Test et E2E Test ?**
Unit : teste unité isolée (méthode/classe), mocks dépendances, rapide, nombreux. Integration : teste interaction composants (service+repository), vraie DB, moyen. E2E : teste système complet (UI→DB), lent, peu nombreux. Pyramide tests.

**Q119: Qu'est-ce que la pyramide des tests ?**
Beaucoup de unit tests (base), moins d'integration tests (milieu), peu de E2E tests (sommet). Coût/vitesse augmentent vers sommet. 70% unit, 20% integration, 10% E2E recommandé.

**Q120: Qu'est-ce que le pattern AAA (Arrange-Act-Assert) ?**
Structure test claire : Arrange (préparer données/mocks), Act (exécuter action testée), Assert (vérifier résultat). Améliore lisibilité. Alternative : Given-When-Then (BDD).

**Q121: Qu'est-ce que Mockito ?**
Framework mocking Java. Créer mocks/stubs objets. when().thenReturn() pour comportement. verify() pour interactions. @Mock, @InjectMocks annotations. Essentiel isolation tests.

**Q122: Différence entre Mock, Stub et Spy ?**
Mock : objet simulé, vérifie interactions (verify). Stub : retourne données prédéfinies, pas de vérification. Spy : objet réel partiel, certaines méthodes mockées, autres réelles. Spy avec Mockito.spy().

**Q123: Comment créer un Mock avec Mockito ?**
Manuel : MyService mock = Mockito.mock(MyService.class). Annotation : @Mock MyService service + @ExtendWith(MockitoExtension.class). when(service.findById(1L)).thenReturn(user). Pas d'implémentation réelle.

**Q124: Qu'est-ce que @InjectMocks ?**
Injecte mocks dans objet testé. @InjectMocks UserController controller; @Mock UserService service; Mockito injecte service dans controller automatiquement. Alternative injection manuelle constructeur.

**Q125: Comment vérifier qu'une méthode a été appelée ?**
verify(mock).methodName(args). verify(mock, times(2)).method(). verify(mock, never()).method(). verify(mock, atLeastOnce()). verifyNoInteractions(mock). verifyNoMoreInteractions(mock).

**Q126: Qu'est-ce que ArgumentCaptor ?**
Capture arguments passés aux mocks. ArgumentCaptor<User> captor = ArgumentCaptor.forClass(User.class). verify(mock).save(captor.capture()). User captured = captor.getValue(). Utile assertions complexes arguments.

**Q127: Qu'est-ce que ArgumentMatchers ?**
Matchers flexibles pour arguments. any(), anyString(), anyInt(), eq(), isNull(), argThat(). Exemple : when(service.findBy(anyString(), eq(25))).thenReturn(users). Attention : tous args matchés ou aucun.

**Q128: Comment mocker méthodes void ?**
doNothing().when(mock).voidMethod(). doThrow(Exception.class).when(mock).method(). doAnswer(invocation -> {...}).when(mock).method(). Verify avec verify(mock).method().

**Q129: Comment mocker méthodes statiques ?**
Mockito 3.4+ : try (MockedStatic<MyClass> mock = mockStatic(MyClass.class)) { mock.when(MyClass::staticMethod).thenReturn(value); }. Scope limité try-with-resources. Alternative : PowerMock (legacy).

**Q130: Qu'est-ce que @Spy vs @Mock ?**
@Mock : objet complètement simulé, pas de logique réelle. @Spy : objet réel, méthodes réelles appelées sauf si stubbées. when() vs doReturn().when() pour spy. Spy utile mock partiel.

**Q131: Comment tester les exceptions ?**
JUnit 5 : assertThrows(Exception.class, () -> method()). Exception e = assertThrows(...); assertEquals("message", e.getMessage()). Mockito : when().thenThrow(new Exception()). @Test(expected=...) JUnit 4 (obsolète).

**Q132: Qu'est-ce que @ParameterizedTest ?**
Test avec multiples valeurs d'entrée. @ValueSource, @CsvSource, @MethodSource, @EnumSource. Exemple : @ParameterizedTest @ValueSource(ints={1,2,3}) void test(int value). Réduit duplication.

**Q133: Différence entre @BeforeEach et @BeforeAll ?**
@BeforeEach : exécuté avant chaque test, instance variables. @BeforeAll : une fois avant tous tests, static, setup coûteux. @AfterEach/@AfterAll pour cleanup. JUnit 5 annotations.

**Q134: Qu'est-ce que @Nested dans JUnit 5 ?**
Grouper tests liés dans classe interne. Structure hiérarchique. Améliore organisation, lisibilité. @DisplayName pour noms descriptifs. Setup spécifique par groupe.

**Q135: Comment tester code asynchrone ?**
CompletableFuture : get() avec timeout. Awaitility library : await().atMost(5, SECONDS).until(() -> condition). CountDownLatch pour threads. @Async tests avec @SpringBootTest.

**Q136: Qu'est-ce que Test Coverage ?**
Pourcentage code exécuté par tests. Outils : JaCoCo, Cobertura. Métriques : line, branch, method coverage. 80%+ recommandé mais qualité > quantité. Ne garantit pas absence bugs.

**Q137: Comment générer rapport de couverture ?**
Maven : jacoco-maven-plugin, mvn test jacoco:report. Rapport HTML dans target/site/jacoco. Gradle : jacoco plugin, jacocoTestReport task. CI/CD intégration (SonarQube, Codecov).

**Q138: Qu'est-ce que Mutation Testing ?**
Teste qualité tests en modifiant code (mutants). Tests doivent détecter mutations. PIT/Pitest pour Java. Mutation score = mutants tués / total. Complémentaire au coverage.

**Q139: Best practices pour les tests unitaires ?**
FIRST : Fast, Independent, Repeatable, Self-validating, Timely. Un assert par test (idéalement). Noms descriptifs. Pas de logique complexe. Isoler dépendances. Tests déterministes. Mock externe (DB, API).

**Q140: Comment nommer les méthodes de test ?**
should_ExpectedBehavior_When_StateUnderTest. given_Precondition_when_Action_then_Result. testMethodName_ExpectedResult. Descriptif, lisible. Convention équipe. @DisplayName JUnit 5 pour phrases.

**Q141: Qu'est-ce que le Test Double ?**
Terme générique pour objets test. Types : Dummy (passé non utilisé), Stub (retourne données), Spy (enregistre appels), Mock (vérifie comportement), Fake (implémentation simplifiée). Mockito couvre la plupart.

**Q142: Comment tester méthodes privées ?**
Généralement : NE PAS tester directement. Tester via méthodes publiques. Si nécessaire : Reflection (déconseillé), rendre package-private, extraire en classe séparée. Besoin tester privé = code smell.

**Q143: Qu'est-ce que AssertJ ?**
Fluent assertion library. Syntaxe lisible : assertThat(actual).isEqualTo(expected).isNotNull(). Assertions chaînées. Meilleurs messages erreur. assertThat(list).hasSize(3).contains("a", "b"). Inclus spring-boot-starter-test.

**Q144: Différence entre assertEquals et assertThat ?**
assertEquals : JUnit natif, ordre important (expected, actual). assertThat : AssertJ/Hamcrest, fluent, lisible, meilleurs messages. assertThat plus moderne, expressif. Préférer assertThat.

**Q145: Comment tester avec base de données ?**
@DataJpaTest + H2 en mémoire. @Sql scripts pour données. TestEntityManager pour CRUD. @Transactional rollback auto. Testcontainers pour vraie DB (Docker). Avoid DB réelle prod.

**Q146: Qu'est-ce que Testcontainers ?**
Library Java pour tests avec Docker containers. Vraie DB (PostgreSQL, MySQL), Redis, Kafka, etc. @Container annotation. Lifecycle géré automatiquement. Tests plus réalistes qu'en mémoire.

**Q147: Comment configurer Testcontainers ?**
Dependency testcontainers. @Testcontainers sur classe. @Container static container = new PostgreSQLContainer<>("postgres:14"). Configure spring.datasource.* depuis container. DynamicPropertySource pour properties.

**Q148: Qu'est-ce que @DirtiesContext ?**
Recharge ApplicationContext après test. Utile si test modifie beans/state. Coûteux, éviter si possible. ClassMode : BEFORE/AFTER_CLASS/EACH_TEST_METHOD. MethodMode : BEFORE/AFTER_METHOD.

**Q149: Comment tester Scheduled tasks ?**
Désactiver scheduling : spring.task.scheduling.enabled=false en test. Appeler méthode directement. AwaitableJobRunner pattern. @MockBean pour dépendances. Vérifier logique, pas timing.

**Q150: Comment tester Events Spring ?**
@ApplicationEventPublisher dans tests. @EventListener sur méthode test. Verify mock listeners. @TransactionalEventListener teste avec @Transactional. Capture events avec custom listener.

## 🥒 CUCUMBER & BDD

**Q151: Qu'est-ce que Cucumber ?**
Framework BDD (Behavior-Driven Development). Tests en langage naturel (Gherkin). Collaboration métier/tech. Features décrivent comportements. Step definitions (Java) exécutent. Executable specifications.

**Q152: Qu'est-ce que BDD (Behavior-Driven Development) ?**
Méthodologie agile axée comportement. Collaboration Product Owner/Devs/QA. Spécifications exécutables. Given-When-Then syntax. Tests = documentation. TDD + ubiquitous language.

**Q153: Qu'est-ce que Gherkin ?**
Langage pour écrire scénarios Cucumber. Mots-clés : Feature, Scenario, Given, When, Then, And, But, Background, Scenario Outline, Examples. Lisible non-techniques. Fichiers .feature.

**Q154: Structure d'un fichier .feature ?**
```gherkin
Feature: Description fonctionnalité
  
  Scenario: Description scénario
    Given contexte initial
    When action effectuée
    Then résultat attendu
    And étape additionnelle
```
Feature regroupe scénarios liés. Scenario = cas test.

**Q155: Différence entre Scenario et Scenario Outline ?**
Scenario : test unique avec valeurs fixes. Scenario Outline : template avec paramètres, Examples table. Évite duplication scénarios similaires. Data-driven testing.

**Q156: Exemple de Scenario Outline ?**
```gherkin
Scenario Outline: Vérifier login
  Given un utilisateur avec username "<username>"
  When il entre le password "<password>"
  Then il devrait être "<résultat>"
  
  Examples:
    | username | password | résultat |
    | john     | pass123  | connecté |
    | jane     | wrong    | refusé   |
```

**Q157: Qu'est-ce que les Step Definitions ?**
Code Java implémentant étapes Gherkin. Annotations @Given, @When, @Then. Paramètres extraits via regex ou Cucumber Expressions. Classe avec méthodes mappant steps. Glue entre feature et code.

**Q158: Comment écrire une Step Definition ?**
```java
@Given("un utilisateur avec l'email {string}")
public void unUtilisateurAvecEmail(String email) {
    user = new User(email);
}

@When("il crée un compte")
public void ilCreeUnCompte() {
    response = userService.createAccount(user);
}

@Then("le compte devrait être créé avec succès")
public void compteCreéSuccès() {
    assertEquals(201, response.getStatus());
}
```

**Q159: Qu'est-ce que les Cucumber Expressions ?**
Alternative regex pour paramètres. Plus simples : {int}, {float}, {string}, {word}. Exemple : "j'ai {int} pommes" au lieu de "j'ai (\\d+) pommes". Type-safe, lisibles.

**Q160: Comment partager état entre steps ?**
Classe context/world partagée. Injection dépendances (PicoContainer, Spring). Attributs instance classe step definitions. ScenarioContext pattern. Avoid static (problèmes parallélisation).

**Q161: Qu'est-ce que les Hooks Cucumber ?**
Méthodes exécutées avant/après scénarios. @Before, @After, @BeforeStep, @AfterStep. Setup/cleanup. Ordre avec order parameter. @Before(order=1). Conditional avec tags.

**Q162: Comment utiliser les Tags Cucumber ?**
Organiser/filtrer scénarios. @tag sur Feature/Scenario. Exemple : @smoke, @regression, @wip. Runner : tags = "@smoke and not @wip". CI/CD exécuter subset tests.

**Q163: Qu'est-ce que Background dans Cucumber ?**
Steps communes à tous scénarios d'une feature. Exécuté avant chaque Scenario. Évite répétition Given. Exemple : login commun, setup données.

**Q164: Comment configurer Cucumber avec JUnit 5 ?**
```java
@Suite
@IncludeEngines("cucumber")
@SelectClasspathResource("features")
@ConfigurationParameter(key = PLUGIN_PROPERTY_NAME, value = "pretty")
@ConfigurationParameter(key = GLUE_PROPERTY_NAME, value = "com.example.steps")
public class CucumberTest {
}
```
Dependency : cucumber-junit-platform-engine.

**Q165: Comment configurer Cucumber avec Spring Boot ?**
Dependency : cucumber-spring. @CucumberContextConfiguration + @SpringBootTest sur classe config. Steps autowire beans Spring. @DirtiesContext si nécessaire. Integration testing.

**Q166: Différence entre @CucumberContextConfiguration et @SpringBootTest ?**
@CucumberContextConfiguration : marque classe config Cucumber-Spring. @SpringBootTest : charge ApplicationContext Spring. Combiner sur même classe pour intégration. Une seule classe par projet.

**Q167: Comment générer rapports Cucumber ?**
Plugin dans runner : "html:target/cucumber-reports.html", "json:target/cucumber.json", "junit:target/cucumber.xml". Cucumber Reports (Maven plugin) pour HTML avancé. Jenkins Cucumber Reports plugin.

**Q168: Qu'est-ce que les Data Tables dans Cucumber ?**
Tables de données dans steps. Exemple :
```gherkin
Given les utilisateurs suivants:
  | nom  | email          |
  | John | john@email.com |
  | Jane | jane@email.com |
```
Step definition : DataTable dataTable parameter. dataTable.asMaps(), asLists().

**Q169: Comment paramétrer Step Definitions avec Data Tables ?**
```java
@Given("les utilisateurs suivants:")
public void lesUtilisateursSuivants(DataTable dataTable) {
    List<Map<String, String>> rows = dataTable.asMaps();
    rows.forEach(row -> {
        String nom = row.get("nom");
        String email = row.get("email");
        users.add(new User(nom, email));
    });
}
```

**Q170: Qu'est-ce que les Doc Strings dans Cucumber ?**
Texte multi-lignes dans steps. Délimité par """. Exemple :
```gherkin
When je soumets le JSON suivant:
  """
  {
    "name": "John",
    "email": "john@test.com"
  }
  """
```
Step definition : String docString parameter.

**Q171: Comment gérer l'authentification dans Cucumber ?**
Background avec login step. Stocker token dans ScenarioContext. RestAssured RequestSpecification avec auth. @Before hook pour setup. Éviter credentials hardcodés.

**Q172: Comment tester API REST avec Cucumber ?**
RestAssured dans step definitions. Given spec, When request, Then response. Exemple :
```java
@When("je fais un GET sur {string}")
public void getFaireRequete(String endpoint) {
    response = given().get(endpoint);
}

@Then("le status code devrait être {int}")
public void verifierStatus(int status) {
    response.then().statusCode(status);
}
```

**Q173: Qu'est-ce que le pattern Page Object avec Cucumber ?**
Séparer logique UI des steps. Classes Page Object pour pages web. Step definitions délèguent à Page Objects. Maintenabilité, réutilisabilité. Selenium + Cucumber.

**Q174: Comment paralléliser tests Cucumber ?**
JUnit 5 : @Execution(PARALLEL). Maven Surefire : forkCount/parallel. Cucumber 7+ : parallel execution. Attention shared state. Thread-safe contexts. Configuration selon runner.

**Q175: Best practices Cucumber ?**
Steps réutilisables, granularité correcte (ni trop atomique ni trop gros). Éviter logique métier dans steps. Un scénario = un comportement. Tags pour organisation. Background pour setup commun. Noms explicites. Gherkin lisible business.

**Q176: Différence entre Imperative et Declarative Gherkin ?**
Imperative (mauvais) : détaille clics/actions UI. "Cliquer bouton, entrer texte". Declarative (bon) : décrit intention. "Créer un compte". Plus maintenable, abstraction implémentation.

**Q177: Comment débugger tests Cucumber ?**
Breakpoints dans step definitions. Logs dans steps. @Before hook avec Scenario.log(). Screenshots sur échec. Cucumber dry-run pour vérifier mapping. IDE support (IntelliJ IDEA).

**Q178: Qu'est-ce que cucumber.properties ?**
Fichier configuration Cucumber. src/test/resources/cucumber.properties. Configure glue, plugin, features path, tags. Alternative : annotations @ConfigurationParameter.

**Q179: Comment gérer données de test avec Cucumber ?**
Test data builders. Fichiers JSON/CSV. @Before hooks setup. Database seeding via @Sql. Test fixtures. Éviter hardcoding dans features.

**Q180: Comment intégrer Cucumber dans CI/CD ?**
Maven/Gradle task exécute tests. Rapports JSON parsés. Jenkins Cucumber Reports plugin. GitLab CI artifacts. Exécution conditionnelle par tags. Parallélisation pour vitesse.

**Q181: Qu'est-ce que les Custom Parameter Types ?**
Types personnalisés pour parameters. Alternative enums/objets complexes. @ParameterType annotation. Transformer String en objet. Améliore lisibilité, réutilisabilité.

**Q182: Exemple Custom Parameter Type ?**
```java
@ParameterType("admin|user|guest")
public UserRole role(String value) {
    return UserRole.valueOf(value.toUpperCase());
}

// Dans feature: Given je suis un {role}
// Step: public void jeSuisRole(UserRole role)
```

**Q183: Comment gérer plusieurs environnements ?**
Profiles Spring (@ActiveProfiles). Configuration properties par env. Variables environnement. @ConfigurationParameter. Cucumber tags par env. Build profiles Maven.

**Q184: Différence entre Cucumber JVM et Cucumber Ruby ?**
Cucumber JVM : Java/Kotlin/Scala, step definitions Java, Spring integration. Cucumber Ruby : original, Ruby steps, RSpec integration. Même Gherkin syntax. JVM plus commun Java shops.

**Q185: Comment tester WebSocket avec Cucumber ?**
StompSession dans context. @Before connect, @After disconnect. Steps pour send/receive messages. Awaitility pour async. Mock WebSocket server si nécessaire.

**Q186: Qu'est-ce que les Examples avec tags ?**
Taguer lignes Examples individuellement. Exécuter subset données. Exemple :
```gherkin
Examples:
  | username | password |
  @valid
  | john     | pass123  |
  @invalid
  | jane     | wrong    |
```
Run : @valid uniquement.

**Q187: Comment gérer attentes/timeouts Cucumber ?**
Awaitility dans steps. Implicit waits Selenium. Retry logic. Polling conditions. await().atMost(10, SECONDS).until(() -> condition). Éviter Thread.sleep().

**Q188: Comment mesurer couverture avec Cucumber ?**
JaCoCo fonctionne. Cucumber = integration tests, couverture différente unit tests. Coverage par feature possible. Mutation testing moins adapté.

**Q189: Limites de Cucumber ?**
Overhead setup/maintenance. Pas adapté unit tests. Peut devenir verbeux. Duplication steps si mal organisé. Performance tests longs. Courbe apprentissage équipe.

**Q190: Quand utiliser Cucumber vs tests classiques ?**
Cucumber : acceptance tests, collaboration métier, documentation vivante, E2E scenarios. Tests classiques : unit tests, performance, coverage exhaustif. Combiner selon besoin.

## 🎯 QUESTIONS AVANCÉES TESTING

**Q191: Qu'est-ce que Contract Testing ?**
Teste contrat entre services (consumer/provider). Pact framework. Consumer définit attentes, Provider valide. Évite breaking changes API. Alternative integration tests coûteux. Microservices.

**Q192: Qu'est-ce que le Chaos Engineering testing ?**
Teste résilience en introduisant pannes. Chaos Monkey (Netflix). Simuler failures réseau, CPU, mémoire. Vérifie graceful degradation. Production testing.

**Q193: Comment tester performance dans Spring Boot ?**
JMeter, Gatling pour load testing. @SpringBootTest avec profiling. Actuator metrics. Microbenchmark avec JMH. Database query performance. Response time assertions.

**Q194: Qu'est-ce que Approval Testing ?**
Compare output à snapshot approuvé. Utile données complexes (JSON, HTML). ApprovalTests library. Approve once, detect changes. Complément assertions classiques.

**Q195: Comment tester sécurité applicative ?**
OWASP dependency check. Security unit tests (@WithMockUser). SQL injection tests. XSS prevention. CSRF token validation. Authorization tests. Penetration testing tools.

**Q196: Qu'est-ce que Property-Based Testing ?**
Générer inputs aléatoires, vérifier propriétés. Quickcheck (Haskell), jqwik (Java). Trouve edge cases. Exemple : reverse(reverse(x)) == x. Complémentaire example-based.

**Q197: Comment tester code legacy sans tests ?**
Characterization tests (capture comportement actuel). Refactor progressif avec tests. Seams (points modification). Test harness. Approval testing. Livre "Working Effectively with Legacy Code".

**Q198: Qu'est-ce que les Flaky Tests ?**
Tests instables (passent/échouent aléatoirement). Causes : timing, ordre exécution, state partagé, dépendances externes. Solutions : isoler, fixtures, deterministic data, retry logic (dernier recours).

**Q199: Comment détecter et fixer Flaky Tests ?**
Exécuter N fois. Analyser logs/patterns. Isoler dépendances. Fixed data/time. Parallel execution off. Awaitility pour async. CI/CD tracking. Quarantine flaky tests.

**Q200: Qu'est-ce que Test Observability ?**
Comprendre pourquoi tests échouent. Logs détaillés. Screenshots sur échec. Stack traces claires. Metrics tests. CI/CD reporting. Debugging facilité.

---

## 💡 CONSEILS SUPPLÉMENTAIRES

**Testing Best Practices :**
- Tester comportement, pas implémentation
- Tests rapides et déterministes
- Indépendance totale entre tests
- Noms explicites et documentation
- Refactorer tests comme code production
- Coverage n'est pas qualité
- Tests = première ligne documentation

**Cucumber Best Practices :**
- Écrire features AVANT implémentation
- Impliquer Product Owner dans rédaction
- Scenarios courts et focalisés
- Éviter détails techniques dans Gherkin
- Réutiliser steps maximalement
- Tags pour organisation intelligente
- Maintenir features à jour

**Éviter :**
- Tests dépendants ordre exécution
- Logique complexe dans tests
- Mocks excessifs (brittleness)
- Tests trop couplés implémentation
- Ignorer tests cassés
- Tests sans asserts
- Copy-paste sans comprendre

Bon courage pour votre entretien ! 🚀💪






























<br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br />