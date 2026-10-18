# Guide Complet Cucumber & BDD pour Entretien

## 1. BDD (Behavior-Driven Development)

### Qu'est-ce que le BDD?

**BDD = TDD + Communication**

Le BDD est une approche de développement qui:
- Utilise un **langage naturel** (Gherkin) compréhensible par tous
- Facilite la **collaboration** entre développeurs, testeurs, et business
- Se concentre sur le **comportement** du système, pas l'implémentation
- Produit une **documentation vivante** automatiquement

### BDD vs TDD

| Aspect | TDD | BDD |
|--------|-----|-----|
| Focus | **Comment** (implémentation) | **Quoi/Pourquoi** (comportement) |
| Langage | Code de test | Langage naturel (Gherkin) |
| Audience | Développeurs | Toute l'équipe |
| Tests | Unitaires techniques | Scénarios fonctionnels |

### Les 3 Amigos

Le BDD encourage les discussions entre:
1. **Business** (Product Owner) - Définit QUOI
2. **Développement** (Dev) - Implémente COMMENT
3. **Test** (QA) - Vérifie QUE

---

## 2. Cucumber - Framework BDD

### Qu'est-ce que Cucumber?

Cucumber est un outil qui:
- Exécute des tests écrits en **Gherkin** (langage naturel)
- Relie les scénarios Gherkin au **code Java** (Step Definitions)
- Génère des **rapports** de test lisibles
- Supporte plusieurs langages (Java, JavaScript, Ruby, etc.)

### Architecture Cucumber

```
┌─────────────────────────────────────────────────┐
│  Feature Files (.feature)                       │
│  Scénarios en Gherkin (langage naturel)        │
└─────────────────┬───────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────┐
│  Step Definitions (Java)                        │
│  Code qui exécute chaque step                   │
└─────────────────┬───────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────┐
│  Application Code                               │
│  Code métier testé                              │
└─────────────────────────────────────────────────┘
```

---

## 3. Gherkin - Langage des Scénarios

### Mots-clés Principaux

| Mot-clé | Usage | Description |
|---------|-------|-------------|
| `Feature` | En-tête | Décrit la fonctionnalité testée |
| `Scenario` | Scénario | Un cas de test |
| `Given` | Contexte | État initial (préconditions) |
| `When` | Action | Ce que l'utilisateur fait |
| `Then` | Vérification | Résultat attendu |
| `And` / `But` | Continuation | Enchaîner plusieurs steps |
| `Background` | Setup | Steps communs à tous les scénarios |
| `Scenario Outline` | Template | Scénario avec plusieurs jeux de données |
| `Examples` | Données | Tableau de données pour Scenario Outline |

### Structure d'un Feature File

```gherkin
# language: fr
@tag_feature
Feature: Gestion des utilisateurs
  En tant qu'administrateur
  Je veux gérer les utilisateurs
  Afin de contrôler les accès au système

  Background:
    Given l'application est démarrée
    And je suis connecté en tant qu'administrateur

  @smoke @positive
  Scenario: Créer un nouvel utilisateur avec succès
    Given je suis sur la page de création d'utilisateur
    When je saisis le nom "John Doe"
    And je saisis l'email "john@example.com"
    And je clique sur le bouton "Créer"
    Then l'utilisateur "John Doe" est créé
    And je vois le message "Utilisateur créé avec succès"

  @negative
  Scenario: Impossible de créer un utilisateur avec email invalide
    Given je suis sur la page de création d'utilisateur
    When je saisis le nom "John Doe"
    And je saisis l'email "email_invalide"
    And je clique sur le bouton "Créer"
    Then je vois le message d'erreur "Email invalide"
    And aucun utilisateur n'est créé

  @parametrized
  Scenario Outline: Validation des âges utilisateurs
    Given je crée un utilisateur avec l'âge <age>
    Then le statut de validation est "<statut>"

    Examples:
      | age | statut  |
      | 17  | invalide |
      | 18  | valide   |
      | 25  | valide   |
      | 150 | invalide |
```

### Bonnes Pratiques Gherkin

```gherkin
# ❌ Mauvais - Trop technique
Given je fais un POST à "/api/users" avec {"name": "John"}
When la réponse HTTP est 201
Then la base de données contient 1 user

# ✅ Bon - Langage métier
Given je veux créer un nouvel utilisateur "John"
When je soumets le formulaire de création
Then l'utilisateur "John" apparaît dans la liste
```

**Règles d'or:**
- **Déclaratif**, pas impératif
- **Langage métier**, pas technique
- **Indépendant de l'UI** (pas de "je clique sur le bouton rouge en haut")
- **Un scénario = un comportement**

---

## 4. Step Definitions (Glue Code)

### Annotations Cucumber

| Annotation | Usage | Exemple |
|------------|-------|---------|
| `@Given` | Précondition | État initial du système |
| `@When` | Action | Action de l'utilisateur |
| `@Then` | Vérification | Assertion du résultat |
| `@And` / `@But` | Continuation | Même chose que Given/When/Then |
| `@Before` | Hook | Avant chaque scénario |
| `@After` | Hook | Après chaque scénario |
| `@BeforeStep` | Hook | Avant chaque step |
| `@AfterStep` | Hook | Après chaque step |

### Exemple Complet

```java
// UserSteps.java
package com.example.steps;

import io.cucumber.java.en.*;
import static org.junit.jupiter.api.Assertions.*;

public class UserSteps {
    
    private UserService userService;
    private User currentUser;
    private String errorMessage;
    
    // Constructor injection (recommandé)
    public UserSteps(TestContext context) {
        this.userService = context.getUserService();
    }
    
    @Given("je suis sur la page de création d'utilisateur")
    public void jesuisSurLaPageDeCreation() {
        // Navigation vers la page
        System.out.println("Navigation vers /users/new");
    }
    
    @Given("l'application est démarrée")
    public void applicationDemarree() {
        // Setup initial
        userService.initialize();
    }
    
    @When("je saisis le nom {string}")
    public void jeSaisisLeNom(String nom) {
        if (currentUser == null) {
            currentUser = new User();
        }
        currentUser.setName(nom);
    }
    
    @When("je saisis l'email {string}")
    public void jeSaisisEmail(String email) {
        currentUser.setEmail(email);
    }
    
    @When("je clique sur le bouton {string}")
    public void jeCliqueSurBouton(String bouton) {
        if (bouton.equals("Créer")) {
            try {
                currentUser = userService.createUser(currentUser);
            } catch (InvalidEmailException e) {
                errorMessage = e.getMessage();
            }
        }
    }
    
    @Then("l'utilisateur {string} est créé")
    public void utilisateurEstCree(String nom) {
        assertNotNull(currentUser);
        assertEquals(nom, currentUser.getName());
        assertTrue(currentUser.getId() > 0);
    }
    
    @Then("je vois le message {string}")
    public void jeVoisLeMessage(String message) {
        // Vérifier le message de succès
        assertEquals(message, "Utilisateur créé avec succès");
    }
    
    @Then("je vois le message d'erreur {string}")
    public void jeVoisMessageErreur(String message) {
        assertNotNull(errorMessage);
        assertEquals(message, errorMessage);
    }
    
    @Then("aucun utilisateur n'est créé")
    public void aucunUtilisateurCree() {
        assertNull(currentUser.getId());
    }
}
```

### Expressions Cucumber

```java
// Paramètres simples
@Given("je crée un utilisateur {string}")
public void creerUtilisateur(String nom) { }

// Paramètres multiples
@Given("je crée un utilisateur {string} avec l'email {string}")
public void creerUtilisateur(String nom, String email) { }

// Nombres
@Given("je crée {int} utilisateurs")
public void creerUtilisateurs(int nombre) { }

@Given("le prix est {double} euros")
public void definirPrix(double prix) { }

// Expressions régulières personnalisées
@Given("^je crée un utilisateur avec le code (\\d{4})$")
public void creerAvecCode(String code) { }

// DataTable
@Given("je crée les utilisateurs suivants:")
public void creerUtilisateurs(DataTable dataTable) {
    List<Map<String, String>> rows = dataTable.asMaps();
    for (Map<String, String> row : rows) {
        String nom = row.get("nom");
        String email = row.get("email");
        userService.createUser(nom, email);
    }
}

// DocString (texte multiligne)
@Given("je soumets le JSON suivant:")
public void soumetJSON(String jsonContent) {
    // jsonContent contient le texte multiligne
}
```

### Utilisation DataTable et DocString

```gherkin
# DataTable
Scenario: Créer plusieurs utilisateurs
  Given je crée les utilisateurs suivants:
    | nom       | email                |
    | John Doe  | john@example.com     |
    | Jane Doe  | jane@example.com     |
    | Bob Smith | bob@example.com      |
  Then 3 utilisateurs existent

# DocString (texte multiligne)
Scenario: Créer utilisateur via API
  Given je soumets le JSON suivant:
    """
    {
      "name": "John Doe",
      "email": "john@example.com",
      "age": 25
    }
    """
  Then l'utilisateur est créé
```

---

## 5. Hooks et Configuration

### Hooks Cucumber

```java
package com.example.hooks;

import io.cucumber.java.*;

public class Hooks {
    
    // Avant CHAQUE scénario
    @Before
    public void beforeScenario() {
        System.out.println("Setup avant scénario");
    }
    
    // Après CHAQUE scénario
    @After
    public void afterScenario(Scenario scenario) {
        if (scenario.isFailed()) {
            System.out.println("Scénario échoué: " + scenario.getName());
            // Prendre un screenshot, etc.
        }
    }
    
    // Hooks avec tags (seulement pour certains scénarios)
    @Before("@database")
    public void beforeDatabaseScenario() {
        System.out.println("Initialiser la base de données");
    }
    
    @After("@browser")
    public void afterBrowserScenario() {
        System.out.println("Fermer le navigateur");
    }
    
    // Hooks avec ordre d'exécution
    @Before(order = 1) // S'exécute en premier
    public void setupFirst() { }
    
    @Before(order = 2) // S'exécute en second
    public void setupSecond() { }
    
    // Avant/Après chaque step
    @BeforeStep
    public void beforeStep() {
        System.out.println("Avant step");
    }
    
    @AfterStep
    public void afterStep() {
        System.out.println("Après step");
    }
}
```

### Test Runner (JUnit 5)

```java
package com.example;

import org.junit.platform.suite.api.*;

@Suite
@IncludeEngines("cucumber")
@SelectClasspathResource("features")
@ConfigurationParameter(key = "cucumber.plugin", value = "pretty, html:target/cucumber-reports.html, json:target/cucumber.json")
@ConfigurationParameter(key = "cucumber.glue", value = "com.example.steps")
@ConfigurationParameter(key = "cucumber.filter.tags", value = "@smoke")
public class CucumberTestRunner {
}
```

### Test Runner (JUnit 4 - Legacy)

```java
package com.example;

import io.cucumber.junit.Cucumber;
import io.cucumber.junit.CucumberOptions;
import org.junit.runner.RunWith;

@RunWith(Cucumber.class)
@CucumberOptions(
    features = "src/test/resources/features",
    glue = "com.example.steps",
    plugin = {
        "pretty",
        "html:target/cucumber-reports.html",
        "json:target/cucumber.json",
        "junit:target/cucumber.xml"
    },
    tags = "@smoke and not @skip",
    dryRun = false,
    monochrome = true,
    snippets = CucumberOptions.SnippetType.CAMELCASE
)
public class CucumberTestRunner {
}
```

### Options importantes

| Option | Description | Exemple |
|--------|-------------|---------|
| `features` | Chemin des .feature | `"src/test/resources/features"` |
| `glue` | Package des step definitions | `"com.example.steps"` |
| `plugin` | Formats de rapport | `"html:target/reports.html"` |
| `tags` | Filtrer par tags | `"@smoke and not @wip"` |
| `dryRun` | Vérifier les steps manquants | `true` (sans exécution) |
| `monochrome` | Sortie console lisible | `true` |

---

## 6. Tags et Organisation

### Tags Cucumber

```gherkin
@regression @smoke
Feature: Login

  @positive @critical
  Scenario: Login avec credentials valides
    ...

  @negative
  Scenario: Login avec mot de passe incorrect
    ...

  @wip @skip
  Scenario: Login avec 2FA
    ...
```

### Exécution par Tags

```java
// Exécuter seulement les tests @smoke
@ConfigurationParameter(key = "cucumber.filter.tags", value = "@smoke")

// Opérateurs logiques
@ConfigurationParameter(key = "cucumber.filter.tags", value = "@smoke or @regression")
@ConfigurationParameter(key = "cucumber.filter.tags", value = "@smoke and @critical")
@ConfigurationParameter(key = "cucumber.filter.tags", value = "@smoke and not @skip")
@ConfigurationParameter(key = "cucumber.filter.tags", value = "(@smoke or @regression) and not @wip")
```

### Organisation Recommandée

```
src/test/resources/features/
├── login/
│   ├── login_success.feature
│   └── login_failure.feature
├── user_management/
│   ├── create_user.feature
│   ├── update_user.feature
│   └── delete_user.feature
└── shopping_cart/
    ├── add_to_cart.feature
    └── checkout.feature

src/test/java/
├── steps/
│   ├── LoginSteps.java
│   ├── UserSteps.java
│   └── CartSteps.java
├── hooks/
│   └── Hooks.java
├── context/
│   └── TestContext.java
└── runners/
    ├── SmokeTestRunner.java
    └── RegressionTestRunner.java
```

---

## 7. Partage de Contexte entre Steps

### Problème: Partager l'État

Les step definitions sont instanciées par Cucumber. Comment partager des données?

### Solution 1: Dependency Injection (PicoContainer)

```xml
<!-- pom.xml -->
<dependency>
    <groupId>io.cucumber</groupId>
    <artifactId>cucumber-picocontainer</artifactId>
    <version>7.14.0</version>
    <scope>test</scope>
</dependency>
```

```java
// TestContext.java - Contexte partagé
package com.example.context;

public class TestContext {
    private User currentUser;
    private String errorMessage;
    private UserService userService;
    
    public TestContext() {
        this.userService = new UserService();
    }
    
    // Getters et setters
    public User getCurrentUser() { return currentUser; }
    public void setCurrentUser(User user) { this.currentUser = user; }
    public String getErrorMessage() { return errorMessage; }
    public void setErrorMessage(String msg) { this.errorMessage = msg; }
    public UserService getUserService() { return userService; }
}

// LoginSteps.java
public class LoginSteps {
    private final TestContext context;
    
    public LoginSteps(TestContext context) {
        this.context = context;
    }
    
    @When("je me connecte avec {string}")
    public void login(String email) {
        User user = context.getUserService().login(email);
        context.setCurrentUser(user);
    }
}

// UserSteps.java
public class UserSteps {
    private final TestContext context;
    
    public UserSteps(TestContext context) {
        this.context = context;
    }
    
    @Then("je suis connecté en tant que {string}")
    public void verifyLogin(String nom) {
        assertEquals(nom, context.getCurrentUser().getName());
    }
}
```

### Solution 2: Spring Integration

```xml
<dependency>
    <groupId>io.cucumber</groupId>
    <artifactId>cucumber-spring</artifactId>
    <version>7.14.0</version>
    <scope>test</scope>
</dependency>
```

```java
// CucumberSpringConfiguration.java
@CucumberContextConfiguration
@SpringBootTest
public class CucumberSpringConfiguration {
}

// Steps avec Spring
@Component
public class UserSteps {
    
    @Autowired
    private UserService userService;
    
    @Autowired
    private TestContext context;
    
    @When("je crée un utilisateur")
    public void creerUtilisateur() {
        User user = userService.createUser(new User());
        context.setCurrentUser(user);
    }
}
```

---

## 8. Scenario Outline et Examples

### Scenario Outline - Tests Paramétrés

```gherkin
Feature: Validation des utilisateurs

  Scenario Outline: Validation de l'âge
    Given un utilisateur avec l'âge <age>
    When je valide l'utilisateur
    Then le résultat est "<resultat>"
    And le message est "<message>"

    Examples:
      | age | resultat | message                |
      | 17  | invalide | Doit avoir 18 ans      |
      | 18  | valide   | Utilisateur accepté    |
      | 25  | valide   | Utilisateur accepté    |
      | 65  | valide   | Utilisateur accepté    |
      | 66  | invalide | Âge maximum dépassé    |

  # Plusieurs tables d'exemples avec tags
  @jeunes
  Examples: Utilisateurs jeunes
    | age | resultat | message             |
    | 18  | valide   | Bienvenue!          |
    | 25  | valide   | Bienvenue!          |

  @seniors
  Examples: Utilisateurs seniors
    | age | resultat | message             |
    | 60  | valide   | Bienvenue senior!   |
    | 65  | valide   | Bienvenue senior!   |
```

```java
// Step definition
@Given("un utilisateur avec l'âge {int}")
public void utilisateurAvecAge(int age) {
    context.setUser(new User("John", age));
}

@Then("le résultat est {string}")
public void verifierResultat(String resultat) {
    assertEquals(resultat, context.getValidationResult());
}
```

---

## 9. Rapports et Plugins

### Plugins Disponibles

```java
@ConfigurationParameter(key = "cucumber.plugin", value = 
    "pretty, " +                                    // Console colorée
    "html:target/cucumber-reports.html, " +         // HTML
    "json:target/cucumber.json, " +                 // JSON
    "junit:target/cucumber.xml, " +                 // JUnit XML
    "timeline:target/timeline"                      // Timeline
)
```

### Rapport HTML Avancé (Maven Plugin)

```xml
<!-- pom.xml -->
<plugin>
    <groupId>net.masterthought</groupId>
    <artifactId>maven-cucumber-reporting</artifactId>
    <version>5.7.5</version>
    <executions>
        <execution>
            <id>execution</id>
            <phase>verify</phase>
            <goals>
                <goal>generate</goal>
            </goals>
            <configuration>
                <projectName>Mon Projet</projectName>
                <outputDirectory>${project.build.directory}</outputDirectory>
                <inputDirectory>${project.build.directory}</inputDirectory>
                <jsonFiles>
                    <param>**/*.json</param>
                </jsonFiles>
            </configuration>
        </execution>
    </executions>
</plugin>
```

### Screenshot en cas d'échec

```java
@After
public void afterScenario(Scenario scenario) {
    if (scenario.isFailed()) {
        // Prendre un screenshot (Selenium)
        byte[] screenshot = ((TakesScreenshot) driver).getScreenshotAs(OutputType.BYTES);
        scenario.attach(screenshot, "image/png", "Screenshot");
        
        // Logs
        scenario.log("Erreur détectée: " + scenario.getName());
    }
}
```

---

## 10. Cucumber avec Selenium (Tests UI)

### Setup

```xml
<dependency>
    <groupId>org.seleniumhq.selenium</groupId>
    <artifactId>selenium-java</artifactId>
    <version>4.15.0</version>
</dependency>
```

### Exemple Complet

```gherkin
# login.feature
@ui @selenium
Feature: Login utilisateur

  Scenario: Login avec succès via l'interface web
    Given je suis sur la page de login
    When je saisis l'email "john@example.com"
    And je saisis le mot de passe "password123"
    And je clique sur "Se connecter"
    Then je suis redirigé vers le tableau de bord
    And je vois le message "Bienvenue John"
```

```java
// LoginSteps.java
public class LoginSteps {
    
    private final WebDriver driver;
    private LoginPage loginPage;
    
    public LoginSteps(TestContext context) {
        this.driver = context.getDriver();
    }
    
    @Given("je suis sur la page de login")
    public void ouvrirPageLogin() {
        driver.get("http://localhost:8080/login");
        loginPage = new LoginPage(driver);
    }
    
    @When("je saisis l'email {string}")
    public void saisirEmail(String email) {
        loginPage.enterEmail(email);
    }
    
    @When("je saisis le mot de passe {string}")
    public void saisirMotDePasse(String password) {
        loginPage.enterPassword(password);
    }
    
    @When("je clique sur {string}")
    public void cliquer(String bouton) {
        loginPage.clickLogin();
    }
    
    @Then("je suis redirigé vers le tableau de bord")
    public void verifierRedirection() {
        assertTrue(driver.getCurrentUrl().contains("/dashboard"));
    }
    
    @Then("je vois le message {string}")
    public void verifierMessage(String message) {
        assertTrue(driver.getPageSource().contains(message));
    }
}

// Page Object Model
public class LoginPage {
    private WebDriver driver;
    
    @FindBy(id = "email")
    private WebElement emailInput;
    
    @FindBy(id = "password")
    private WebElement passwordInput;
    
    @FindBy(id = "login-button")
    private WebElement loginButton;
    
    public LoginPage(WebDriver driver) {
        this.driver = driver;
        PageFactory.initElements(driver, this);
    }
    
    public void enterEmail(String email) {
        emailInput.sendKeys(email);
    }
    
    public void enterPassword(String password) {
        passwordInput.sendKeys(password);
    }
    
    public void clickLogin() {
        loginButton.click();
    }
}
```

---

## 11. Cucumber avec API REST

```gherkin
# api.feature
@api
Feature: API Utilisateurs

  Scenario: Créer un utilisateur via API
    Given l'API est disponible
    When j'envoie une requête POST "/api/users" avec:
      """
      {
        "name": "John Doe",
        "email": "john@example.com"
      }
      """
    Then le code de réponse est 201
    And la réponse contient l'id de l'utilisateur
    And l'utilisateur existe dans la base de données
```

```java
// ApiSteps.java
public class ApiSteps {
    
    private Response response;
    private String baseUrl = "http://localhost:8080";
    
    @Given("l'API est disponible")
    public void apiDisponible() {
        // Vérifier que l'API répond
    }
    
    @When("j'envoie une requête POST {string} avec:")
    public void envoyerPost(String endpoint, String body) {
        response = RestAssured
            .given()
                .contentType(ContentType.JSON)
                .body(body)
            .when()
                .post(baseUrl + endpoint);
    }
    
    @Then("le code de réponse est {int}")
    public void verifierCodeReponse(int statusCode) {
        assertEquals(statusCode, response.getStatusCode());
    }
    
    @Then("la réponse contient l'id de l'utilisateur")
    public void verifierIdPresent() {
        assertNotNull(response.jsonPath().get("id"));
    }
}
```

---

## 12. Best Practices Cucumber

### ✅ DO (À Faire)

```gherkin
# 1. Langage métier, pas technique
Given je suis un utilisateur enregistré
When je consulte mon profil
Then je vois mes informations personnelles

# 2. Déclaratif, pas impératif
Given je veux réserver un vol
When je recherche un vol Paris-Londres
Then je vois les vols disponibles

# 3. Un scénario = un comportement
Scenario: Réservation réussie
  Given un vol est disponible
  When je réserve le vol
  Then ma réservation est confirmée

# 4. Utiliser Background pour le setup commun
Background:
  Given je suis connecté
  And je suis sur la page d'accueil

# 5. Tags cohérents
@smoke @positive @api
```

### ❌ DON'T (À Éviter)

```gherkin
# 1. Trop technique
Given je fais un POST à "/api/users"
When la réponse JSON contient {"status": "ok"}

# 2. Trop impératif (détails UI)
Given je clique sur le bouton rouge en haut à droite
When je scroll vers le bas de 500 pixels
Then je vois le div avec la classe "success-message"

# 3. Plusieurs comportements dans un scénario
Scenario: Créer, modifier et supprimer un utilisateur
  # Trop de choses! Diviser en 3 scénarios

# 4. Steps trop spécifiques (pas réutilisables)
Given je crée exactement un utilisateur nommé John avec l'email john@example.com âgé de 25 ans habitant Paris

# 5. Dépendances entre scénarios
Scenario: Test 1 - Créer utilisateur
Scenario: Test 2 - Modifier l'utilisateur créé dans Test 1  # ❌ Dépendance!
```

### Règle des 3C

1. **Clair** (Clear) - Facile à comprendre
2. **Concis** (Concise) - Pas de détails inutiles
3. **Cohérent** (Consistent) - Vocabulaire uniforme

---

## 13. Questions Fréquentes en Entretien

### Q1: Différence entre BDD et TDD?

**Réponse:**
- **TDD**: Focus sur le *comment* (implémentation technique), pour développeurs
- **BDD**: Focus sur le *quoi/pourquoi* (comportement métier), pour toute l'équipe
- BDD utilise un langage naturel (Gherkin) compréhensible par tous

### Q2: Qu'est-ce que Gherkin?

**Réponse:**
Gherkin est un langage structuré mais lisible par les humains qui utilise des mots-clés comme:
- `Given` (contexte initial)
- `When` (action)
- `Then` (résultat attendu)

Il permet d'écrire des tests compréhensibles par les non-techniques.

### Q3: Expliquer Given-When-Then

**Réponse:**
```
Given (État initial) - Les préconditions
When (Action) - Ce que l'utilisateur fait
Then (Résultat) - Ce qui doit se produire
```

Exemple:
```gherkin
Given je suis connecté
When je consulte mon panier
Then je vois mes articles
```

### Q4: Différence entre Scenario et Scenario Outline?

**Réponse:**
- `Scenario`: Un seul cas de test
- `Scenario Outline`: Template avec plusieurs jeux de données (Examples)

```gherkin
Scenario Outline: Test avec plusieurs âges
  Given un utilisateur de <age> ans
  Then le statut est "<statut>"
  
  Examples:
    | age | statut  |
    | 17  | mineur  |
    | 18  | majeur  |
    | 25  | majeur  |
```

### Q5: Qu'est-ce qu'un Step Definition?

**Réponse:**
C'est le code Java qui implémente chaque step Gherkin. Il fait le lien entre le scénario et le code métier.

```java
@Given("je suis connecté")
public void jesuisConnecte() {
    // Code d'implémentation
}
```

### Q6: Comment partager des données entre steps?

**Réponse:**
Utiliser **Dependency Injection** avec PicoContainer ou Spring:

```java
public class TestContext {
    private User currentUser;
    // Getters/setters
}

public class LoginSteps {
    private TestContext context;
    
    public LoginSteps(TestContext context) {
        this.context = context;
    }
}
```

### Q7: Qu'est-ce que le Background?

**Réponse:**
Le `Background` contient les steps communs exécutés avant **chaque** scénario du feature file.

```gherkin
Background:
  Given je suis connecté
  And l'application est initialisée

Scenario: Test 1
  # Background s'exécute avant

Scenario: Test 2
  # Background s'exécute avant aussi
```

### Q8: Différence entre @Before et Background?

**Réponse:**

| Aspect | @Before (Hook) | Background |
|--------|----------------|------------|
| Langage | Java | Gherkin |
| Visible | Non (dans les rapports) | Oui |
| Portée | Tous les scénarios | Feature file uniquement |
| Usage | Setup technique | Steps métier communs |

### Q9: Comment gérer les tests asynchrones?

**Réponse:**
Utiliser des attentes explicites:

```java
@Then("le message apparaît")
public void verifierMessage() {
    WebDriverWait wait = new WebDriverWait(driver, Duration.ofSeconds(10));
    wait.until(ExpectedConditions.visibilityOfElementLocated(By.id("message")));
    assertTrue(driver.findElement(By.id("message")).isDisplayed());
}
```

### Q10: Comment organiser les feature files?

**Réponse:**
Par fonctionnalité métier:

```
features/
├── authentication/
│   ├── login.feature
│   └── logout.feature
├── user_management/
│   ├── create_user.feature
│   └── update_user.feature
└── shopping/
    ├── cart.feature
    └── checkout.feature
```

### Q11: Qu'est-ce que DryRun?

**Réponse:**
`dryRun = true` vérifie que tous les steps ont une définition **sans exécuter** le code.

```java
@CucumberOptions(dryRun = true)
```

Utile pour:
- Vérifier qu'aucun step n'est manquant
- Générer les snippets de code
- Validation rapide

### Q12: Comment filtrer les tests par tags?

**Réponse:**

```java
// Exécuter seulement @smoke
@ConfigurationParameter(key = "cucumber.filter.tags", value = "@smoke")

// Combinaisons
value = "@smoke and @api"           // ET
value = "@smoke or @regression"     // OU
value = "@smoke and not @skip"      // ET NON
value = "(@smoke or @critical) and not @wip"  // Complexe
```

### Q13: Avantages et inconvénients de Cucumber?

**Réponse:**

**Avantages ✅:**
- Communication améliorée (langage naturel)
- Documentation vivante automatique
- Tests compréhensibles par tous
- Collaboration Product Owner / Dev / QA
- Réutilisation des steps

**Inconvénients ❌:**
- Courbe d'apprentissage
- Maintenance des feature files
- Peut être verbeux
- Plus lent que tests unitaires
- Risque de duplication si mal organisé

### Q14: Quand utiliser Cucumber?

**Réponse:**

**OUI ✅ - Tests d'acceptance:**
- Tests fonctionnels E2E
- Validation des user stories
- Tests de régression
- Quand la collaboration métier est importante

**NON ❌ - Tests techniques:**
- Tests unitaires (utiliser JUnit/Mockito)
- Tests de performance
- Tests de sécurité bas niveau
- Validation technique pure

---

## 14. Intégration CI/CD

### Maven Configuration

```xml
<!-- pom.xml -->
<build>
    <plugins>
        <plugin>
            <groupId>org.apache.maven.plugins</groupId>
            <artifactId>maven-surefire-plugin</artifactId>
            <version>3.0.0</version>
            <configuration>
                <includes>
                    <include>**/*Runner.java</include>
                </includes>
                <systemPropertyVariables>
                    <cucumber.filter.tags>@smoke</cucumber.filter.tags>
                </systemPropertyVariables>
            </configuration>
        </plugin>
    </plugins>
</build>
```

### Exécution Maven

```bash
# Exécuter tous les tests
mvn test

# Exécuter seulement les tests @smoke
mvn test -Dcucumber.filter.tags="@smoke"

# Exécuter avec profil spécifique
mvn test -P regression

# Générer le rapport
mvn verify
```

### Jenkins Pipeline

```groovy
pipeline {
    agent any
    
    stages {
        stage('Checkout') {
            steps {
                git 'https://github.com/myrepo/myproject.git'
            }
        }
        
        stage('Run Smoke Tests') {
            steps {
                sh 'mvn clean test -Dcucumber.filter.tags="@smoke"'
            }
        }
        
        stage('Run Regression Tests') {
            steps {
                sh 'mvn clean test -Dcucumber.filter.tags="@regression"'
            }
        }
        
        stage('Generate Reports') {
            steps {
                cucumber buildStatus: 'UNSTABLE',
                    reportTitle: 'Cucumber Report',
                    fileIncludePattern: '**/*.json',
                    trendsLimit: 10
            }
        }
    }
    
    post {
        always {
            junit '**/target/cucumber.xml'
            publishHTML([
                reportDir: 'target',
                reportFiles: 'cucumber-reports.html',
                reportName: 'Cucumber HTML Report'
            ])
        }
    }
}
```

### GitLab CI

```yaml
# .gitlab-ci.yml
stages:
  - test
  - report

smoke_tests:
  stage: test
  script:
    - mvn clean test -Dcucumber.filter.tags="@smoke"
  artifacts:
    when: always
    reports:
      junit: target/cucumber.xml
    paths:
      - target/cucumber-reports.html

regression_tests:
  stage: test
  script:
    - mvn clean test -Dcucumber.filter.tags="@regression"
  only:
    - develop
    - master
```

---

## 15. Exemple Complet E2E

### Feature File

```gherkin
@e2e @shopping
Feature: Processus d'achat complet
  En tant que client
  Je veux acheter des produits
  Afin de recevoir ma commande

  Background:
    Given l'application e-commerce est disponible
    And les produits suivants existent:
      | nom          | prix  | stock |
      | Laptop       | 999   | 10    |
      | Souris       | 29    | 50    |
      | Clavier      | 79    | 30    |

  @smoke @positive
  Scenario: Achat réussi d'un produit
    Given je suis un client enregistré "john@example.com"
    And je suis sur la page d'accueil
    When je recherche "Laptop"
    And je clique sur "Ajouter au panier"
    And je vais sur mon panier
    Then je vois 1 article dans le panier
    And le total est de "999 €"
    When je clique sur "Commander"
    And je saisis mon adresse de livraison:
      | rue       | 123 Rue de la Paix |
      | ville     | Paris              |
      | code postal | 75001            |
    And je valide le paiement avec ma carte bancaire
    Then je vois le message "Commande confirmée"
    And je reçois un email de confirmation
    And la commande apparaît dans mon historique

  @negative
  Scenario: Impossible d'acheter un produit en rupture de stock
    Given je suis sur la page produit "Laptop"
    And le stock est à 0
    Then le bouton "Ajouter au panier" est désactivé
    And je vois le message "Rupture de stock"

  @edge_case
  Scenario Outline: Validation du code promo
    Given je suis connecté
    And mon panier contient un article de <montant> €
    When j'applique le code promo "<code>"
    Then le total est de <montant_final> €
    And le message est "<message>"

    Examples:
      | montant | code       | montant_final | message                 |
      | 100     | PROMO10    | 90            | Réduction de 10% appliquée |
      | 100     | INVALID    | 100           | Code promo invalide     |
      | 50      | PROMO10    | 50            | Montant minimum non atteint |
```

### Step Definitions

```java
// ShoppingSteps.java
package com.example.steps;

import io.cucumber.java.en.*;
import io.cucumber.datatable.DataTable;
import static org.junit.jupiter.api.Assertions.*;

public class ShoppingSteps {
    
    private final TestContext context;
    private final ProductService productService;
    private final CartService cartService;
    private final OrderService orderService;
    
    public ShoppingSteps(TestContext context) {
        this.context = context;
        this.productService = context.getProductService();
        this.cartService = context.getCartService();
        this.orderService = context.getOrderService();
    }
    
    @Given("l'application e-commerce est disponible")
    public void applicationDisponible() {
        assertTrue(productService.isAvailable());
    }
    
    @Given("les produits suivants existent:")
    public void creerProduits(DataTable dataTable) {
        List<Map<String, String>> rows = dataTable.asMaps();
        for (Map<String, String> row : rows) {
            Product product = new Product();
            product.setName(row.get("nom"));
            product.setPrice(Double.parseDouble(row.get("prix")));
            product.setStock(Integer.parseInt(row.get("stock")));
            productService.createProduct(product);
        }
    }
    
    @Given("je suis un client enregistré {string}")
    public void clientEnregistre(String email) {
        User user = new User(email, "password");
        context.setCurrentUser(user);
    }
    
    @Given("je suis sur la page d'accueil")
    public void pageAccueil() {
        context.setCurrentPage("home");
    }
    
    @When("je recherche {string}")
    public void rechercher(String query) {
        List<Product> results = productService.search(query);
        context.setSearchResults(results);
    }
    
    @When("je clique sur {string}")
    public void cliquer(String bouton) {
        if (bouton.equals("Ajouter au panier")) {
            Product product = context.getSearchResults().get(0);
            cartService.addToCart(context.getCurrentUser(), product);
        } else if (bouton.equals("Commander")) {
            Order order = orderService.createOrder(context.getCurrentUser());
            context.setCurrentOrder(order);
        }
    }
    
    @When("je vais sur mon panier")
    public void allerPanier() {
        Cart cart = cartService.getCart(context.getCurrentUser());
        context.setCurrentCart(cart);
    }
    
    @Then("je vois {int} article dans le panier")
    public void verifierNombreArticles(int nombre) {
        assertEquals(nombre, context.getCurrentCart().getItems().size());
    }
    
    @Then("le total est de {string}")
    public void verifierTotal(String montant) {
        String actualTotal = context.getCurrentCart().getTotal() + " €";
        assertEquals(montant, actualTotal);
    }
    
    @When("je saisis mon adresse de livraison:")
    public void saisirAdresse(DataTable dataTable) {
        Map<String, String> address = dataTable.asMap();
        Address deliveryAddress = new Address();
        deliveryAddress.setStreet(address.get("rue"));
        deliveryAddress.setCity(address.get("ville"));
        deliveryAddress.setZipCode(address.get("code postal"));
        context.setDeliveryAddress(deliveryAddress);
    }
    
    @When("je valide le paiement avec ma carte bancaire")
    public void validerPaiement() {
        Payment payment = new Payment("CARD", "1234-5678-9012-3456");
        orderService.processPayment(context.getCurrentOrder(), payment);
    }
    
    @Then("je vois le message {string}")
    public void verifierMessage(String message) {
        assertTrue(context.getDisplayedMessages().contains(message));
    }
    
    @Then("je reçois un email de confirmation")
    public void verifierEmail() {
        String email = context.getCurrentUser().getEmail();
        assertTrue(context.getEmailService().hasReceivedEmail(email));
    }
    
    @Then("la commande apparaît dans mon historique")
    public void verifierHistorique() {
        List<Order> orders = orderService.getOrderHistory(context.getCurrentUser());
        assertTrue(orders.contains(context.getCurrentOrder()));
    }
    
    @Given("le stock est à {int}")
    public void definirStock(int stock) {
        Product product = context.getCurrentProduct();
        product.setStock(stock);
    }
    
    @Then("le bouton {string} est désactivé")
    public void verifierBoutonDesactive(String bouton) {
        assertFalse(context.isButtonEnabled(bouton));
    }
    
    @Given("mon panier contient un article de {double} €")
    public void panierAvecMontant(double montant) {
        Product product = new Product("Test", montant);
        cartService.addToCart(context.getCurrentUser(), product);
    }
    
    @When("j'applique le code promo {string}")
    public void appliquerPromo(String code) {
        try {
            cartService.applyPromoCode(context.getCurrentUser(), code);
        } catch (InvalidPromoCodeException e) {
            context.setErrorMessage(e.getMessage());
        }
    }
}
```

### Hooks

```java
// Hooks.java
package com.example.hooks;

import io.cucumber.java.*;

public class Hooks {
    
    private final TestContext context;
    
    public Hooks(TestContext context) {
        this.context = context;
    }
    
    @Before("@e2e")
    public void setupE2E() {
        System.out.println("🚀 Initialisation test E2E");
        context.initializeDatabase();
        context.startApplication();
    }
    
    @After("@e2e")
    public void cleanupE2E(Scenario scenario) {
        if (scenario.isFailed()) {
            System.out.println("❌ Scénario échoué: " + scenario.getName());
            scenario.log("État du panier: " + context.getCurrentCart());
            scenario.log("Utilisateur: " + context.getCurrentUser());
        }
        context.cleanupDatabase();
    }
    
    @Before("@shopping")
    public void setupShopping() {
        context.initializeCart();
    }
    
    @AfterStep
    public void afterEachStep() {
        // Log pour debug
        System.out.println("Step exécuté");
    }
}
```

---

## 16. Patterns et Anti-Patterns

### ✅ GOOD PATTERNS

#### 1. Page Object Model (avec Selenium)

```java
// LoginPage.java
public class LoginPage {
    private WebDriver driver;
    
    @FindBy(id = "email")
    private WebElement emailField;
    
    @FindBy(id = "password")
    private WebElement passwordField;
    
    @FindBy(id = "submit")
    private WebElement submitButton;
    
    public LoginPage(WebDriver driver) {
        this.driver = driver;
        PageFactory.initElements(driver, this);
    }
    
    public void login(String email, String password) {
        emailField.sendKeys(email);
        passwordField.sendKeys(password);
        submitButton.click();
    }
}

// LoginSteps.java
@When("je me connecte avec {string} et {string}")
public void login(String email, String password) {
    LoginPage page = new LoginPage(context.getDriver());
    page.login(email, password);
}
```

#### 2. Builder Pattern pour données de test

```java
public class UserBuilder {
    private String name = "Default Name";
    private String email = "default@example.com";
    private int age = 25;
    
    public UserBuilder withName(String name) {
        this.name = name;
        return this;
    }
    
    public UserBuilder withEmail(String email) {
        this.email = email;
        return this;
    }
    
    public UserBuilder withAge(int age) {
        this.age = age;
        return this;
    }
    
    public User build() {
        return new User(name, email, age);
    }
}

// Utilisation
@Given("un utilisateur standard")
public void creerUtilisateurStandard() {
    User user = new UserBuilder()
        .withName("John")
        .withEmail("john@example.com")
        .build();
    context.setUser(user);
}
```

#### 3. Steps réutilisables

```java
// ❌ Mauvais - Steps trop spécifiques
@Given("je crée un utilisateur John Doe avec l'email john@example.com")
public void creerJohnDoe() { }

// ✅ Bon - Steps génériques réutilisables
@Given("je crée un utilisateur {string} avec l'email {string}")
public void creerUtilisateur(String nom, String email) { }
```

### ❌ ANTI-PATTERNS

#### 1. Couplage fort avec l'UI

```gherkin
# ❌ Mauvais
Given je clique sur le bouton avec l'id "btn-submit-123"
And je vois le div de classe "alert-success"

# ✅ Bon
Given je soumets le formulaire
And je vois un message de succès
```

#### 2. Steps avec logique métier

```java
// ❌ Mauvais - Logique dans le step
@When("je calcule la réduction")
public void calculerReduction() {
    double price = context.getPrice();
    double discount = price * 0.1; // Logique métier ici!
    context.setDiscount(discount);
}

// ✅ Bon - Déléguer au service
@When("je calcule la réduction")
public void calculerReduction() {
    double discount = discountService.calculate(context.getPrice());
    context.setDiscount(discount);
}
```

#### 3. Scénarios avec dépendances

```gherkin
# ❌ Mauvais - Dépendance entre scénarios
Scenario: Créer un utilisateur
  When je crée l'utilisateur "John"

Scenario: Modifier l'utilisateur créé précédemment
  When je modifie l'utilisateur "John"  # Dépend du scénario précédent!

# ✅ Bon - Scénarios indépendants
Scenario: Modifier un utilisateur existant
  Given un utilisateur "John" existe
  When je modifie l'utilisateur "John"
```

---

## 17. Checklist Entretien Cucumber

### Concepts de base
- [ ] Expliquer BDD vs TDD
- [ ] Qu'est-ce que Gherkin?
- [ ] Given-When-Then
- [ ] Feature, Scenario, Step

### Cucumber Java
- [ ] Step Definitions (@Given, @When, @Then)
- [ ] Hooks (@Before, @After)
- [ ] Test Runner configuration
- [ ] DataTable et DocString

### Avancé
- [ ] Scenario Outline + Examples
- [ ] Background vs @Before
- [ ] Dependency Injection (PicoContainer)
- [ ] Tags et filtrage

### Intégration
- [ ] Cucumber + Selenium
- [ ] Cucumber + REST Assured
- [ ] Cucumber + Spring Boot
- [ ] Rapports (HTML, JSON)

### Best Practices
- [ ] Pattern AAA dans les steps
- [ ] Page Object Model
- [ ] Steps réutilisables
- [ ] Éviter les anti-patterns

---

## 18. Résumé Ultra-Rapide (Pour révision 5min avant l'entretien)

### Gherkin - 3 mots-clés essentiels
```gherkin
Given (contexte) - État initial
When (action) - Ce que fait l'utilisateur  
Then (résultat) - Ce qui doit se produire
```

### Annotations Java - Top 5
```java
@Given("...") - Précondition
@When("...") - Action
@Then("...") - Assertion
@Before - Hook avant scénario
@After - Hook après scénario
```

### Structure projet
```
features/        → Fichiers .feature (Gherkin)
steps/           → Step Definitions (Java)
runners/         → Test Runner (JUnit)
hooks/           → Hooks (@Before/@After)
```

### Options Runner essentielles
```java
features = "chemin/vers/features"
glue = "package.des.steps"
tags = "@smoke and not @skip"
plugin = "html:target/reports.html"
```

### Question piège #1
**Q: Cucumber remplace-t-il JUnit/Mockito?**
**R: NON!** Cucumber est pour tests d'acceptance E2E. JUnit/Mockito pour tests unitaires.

### Pyramide des tests avec Cucumber
```
     E2E (Cucumber)     5%
    Integration        15%
   Unit (JUnit)       80%
```

---

## Bon à savoir pour l'entretien

### Points à mentionner spontanément:
1. **"J'utilise Cucumber pour les tests d'acceptance"** - Montre que tu sais quand l'utiliser
2. **"Je privilégie le langage métier dans Gherkin"** - Bonne pratique
3. **"J'organise mes features par fonctionnalité"** - Organisation
4. **"J'utilise le Page Object Model avec Selenium"** - Pattern connu
5. **"Je filtre mes tests par tags pour la CI/CD"** - Pratique DevOps

### Phrases à éviter:
- ❌ "Cucumber remplace les tests unitaires"
- ❌ "Je mets toute la logique dans les steps"
- ❌ "Mes scénarios décrivent les détails de l'UI"
- ❌ "Je n'utilise pas de hooks"

### Si on te demande un exemple:
Utilise l'exemple e-commerce (section 15) - c'est complet et professionnel!
