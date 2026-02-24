# Complete Cucumber & BDD Interview Guide

## 1. BDD (Behavior-Driven Development)

### What is BDD?

**BDD = TDD + Communication**

BDD is a development approach that:
- Uses a **natural language** (Gherkin) understandable by everyone
- Facilitates **collaboration** between developers, testers, and business
- Focuses on the **behavior** of the system, not the implementation
- Automatically produces **living documentation**

### BDD vs TDD

| Aspect | TDD | BDD |
|--------|-----|-----|
| Focus | **How** (implementation) | **What/Why** (behavior) |
| Language | Test code | Natural language (Gherkin) |
| Audience | Developers | Whole team |
| Tests | Technical unit tests | Functional scenarios |

### The 3 Amigos

BDD encourages discussions between:
1. **Business** (Product Owner) - Defines WHAT
2. **Development** (Dev) - Implements HOW
3. **Test** (QA) - Verifies THAT

---

## 2. Cucumber - BDD Framework

### What is Cucumber?

Cucumber is a tool that:
- Executes tests written in **Gherkin** (natural language)
- Links Gherkin scenarios to **Java code** (Step Definitions)
- Generates readable test **reports**
- Supports multiple languages (Java, JavaScript, Ruby, etc.)

### Cucumber Architecture

```
┌─────────────────────────────────────────────────┐
│  Feature Files (.feature)                       │
│  Scenarios in Gherkin (natural language)        │
└─────────────────┬───────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────┐
│  Step Definitions (Java)                        │
│  Code that executes each step                   │
└─────────────────┬───────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────┐
│  Application Code                               │
│  Tested business code                           │
└─────────────────────────────────────────────────┘
```

---

## 3. Gherkin - Scenario Language

### Main Keywords

| Keyword | Usage | Description |
|---------|-------|-------------|
| `Feature` | Header | Describes the tested feature |
| `Scenario` | Scenario | A test case |
| `Given` | Context | Initial state (preconditions) |
| `When` | Action | What the user does |
| `Then` | Verification | Expected result |
| `And` / `But` | Continuation | Chain multiple steps |
| `Background` | Setup | Steps common to all scenarios |
| `Scenario Outline` | Template | Scenario with multiple data sets |
| `Examples` | Data | Data table for Scenario Outline |

### Feature File Structure

```gherkin
# language: fr
@tag_feature
Feature: User Management
  As an administrator
  I want to manage users
  In order to control system access

  Background:
    Given the application is started
    And I am logged in as an administrator

  @smoke @positive
  Scenario: Successfully create a new user
    Given I am on the user creation page
    When I enter the name "John Doe"
    And I enter the email "john@example.com"
    And I click the "Create" button
    Then the user "John Doe" is created
    And I see the message "User created successfully"

  @negative
  Scenario: Cannot create a user with an invalid email
    Given I am on the user creation page
    When I enter the name "John Doe"
    And I enter the email "invalid_email"
    And I click the "Create" button
    Then I see the error message "Invalid email"
    And no user is created

  @parametrized
  Scenario Outline: User age validation
    Given I create a user with age <age>
    Then the validation status is "<status>"

    Examples:
      | age | status   |
      | 17  | invalid  |
      | 18  | valid    |
      | 25  | valid    |
      | 150 | invalid  |
```

### Gherkin Best Practices

```gherkin
# ❌ Bad - Too technical
Given I make a POST to "/api/users" with {"name": "John"}
When the HTTP response is 201
Then the database contains 1 user

# ✅ Good - Business language
Given I want to create a new user "John"
When I submit the creation form
Then the user "John" appears in the list
```

**Golden rules:**
- **Declarative**, not imperative
- **Business language**, not technical
- **UI-independent** (not "I click the red button at the top")
- **One scenario = one behavior**

---

## 4. Step Definitions (Glue Code)

### Cucumber Annotations

| Annotation | Usage | Example |
|------------|-------|---------|
| `@Given` | Precondition | Initial state of the system |
| `@When` | Action | User action |
| `@Then` | Verification | Result assertion |
| `@And` / `@But` | Continuation | Same as Given/When/Then |
| `@Before` | Hook | Before each scenario |
| `@After` | Hook | After each scenario |
| `@BeforeStep` | Hook | Before each step |
| `@AfterStep` | Hook | After each step |

### Complete Example

```java
// UserSteps.java
package com.example.steps;

import io.cucumber.java.en.*;
import static org.junit.jupiter.api.Assertions.*;

public class UserSteps {
    
    private UserService userService;
    private User currentUser;
    private String errorMessage;
    
    // Constructor injection (recommended)
    public UserSteps(TestContext context) {
        this.userService = context.getUserService();
    }
    
    @Given("I am on the user creation page")
    public void iAmOnUserCreationPage() {
        // Navigate to the page
        System.out.println("Navigating to /users/new");
    }
    
    @Given("the application is started")
    public void applicationStarted() {
        // Initial setup
        userService.initialize();
    }
    
    @When("I enter the name {string}")
    public void iEnterName(String name) {
        if (currentUser == null) {
            currentUser = new User();
        }
        currentUser.setName(name);
    }
    
    @When("I enter the email {string}")
    public void iEnterEmail(String email) {
        currentUser.setEmail(email);
    }
    
    @When("I click the {string} button")
    public void iClickButton(String button) {
        if (button.equals("Create")) {
            try {
                currentUser = userService.createUser(currentUser);
            } catch (InvalidEmailException e) {
                errorMessage = e.getMessage();
            }
        }
    }
    
    @Then("the user {string} is created")
    public void userIsCreated(String name) {
        assertNotNull(currentUser);
        assertEquals(name, currentUser.getName());
        assertTrue(currentUser.getId() > 0);
    }
    
    @Then("I see the message {string}")
    public void iSeeMessage(String message) {
        // Check success message
        assertEquals(message, "User created successfully");
    }
    
    @Then("I see the error message {string}")
    public void iSeeErrorMessage(String message) {
        assertNotNull(errorMessage);
        assertEquals(message, errorMessage);
    }
    
    @Then("no user is created")
    public void noUserCreated() {
        assertNull(currentUser.getId());
    }
}
```

### Cucumber Expressions

```java
// Simple parameters
@Given("I create a user {string}")
public void createUser(String name) { }

// Multiple parameters
@Given("I create a user {string} with email {string}")
public void createUser(String name, String email) { }

// Numbers
@Given("I create {int} users")
public void createUsers(int count) { }

@Given("the price is {double} euros")
public void setPrice(double price) { }

// Custom regular expressions
@Given("^je crée un utilisateur avec le code (\\d{4})$")
public void creerAvecCode(String code) { }

// DataTable
@Given("I create the following users:")
public void createUsers(DataTable dataTable) {
    List<Map<String, String>> rows = dataTable.asMaps();
    for (Map<String, String> row : rows) {
        String name = row.get("name");
        String email = row.get("email");
        userService.createUser(name, email);
    }
}

// DocString (multiline text)
@Given("I submit the following JSON:")
public void submitJSON(String jsonContent) {
    // jsonContent contains the multiline text
}
```

### Using DataTable and DocString

```gherkin
# DataTable
Scenario: Create multiple users
  Given I create the following users:
    | nom       | email                |
    | John Doe  | john@example.com     |
    | Jane Doe  | jane@example.com     |
    | Bob Smith | bob@example.com      |
  Then 3 users exist

# DocString (multiline text)
Scenario: Create user via API
  Given I submit the following JSON:
    """
    {
      "name": "John Doe",
      "email": "john@example.com",
      "age": 25
    }
    """
  Then the user is created
```

---

## 5. Hooks and Configuration

### Cucumber Hooks

```java
package com.example.hooks;

import io.cucumber.java.*;

public class Hooks {
    
    // Before EACH scenario
    @Before
    public void beforeScenario() {
        System.out.println("Setup before scenario");
    }
    
    // After EACH scenario
    @After
    public void afterScenario(Scenario scenario) {
        if (scenario.isFailed()) {
            System.out.println("Scenario failed: " + scenario.getName());
            // Take a screenshot, etc.
        }
    }
    
    // Tagged hooks (only for certain scenarios)
    @Before("@database")
    public void beforeDatabaseScenario() {
        System.out.println("Initialize the database");
    }
    
    @After("@browser")
    public void afterBrowserScenario() {
        System.out.println("Close the browser");
    }
    
    // Hooks with execution order
    @Before(order = 1) // Executes first
    public void setupFirst() { }
    
    @Before(order = 2) // Executes second
    public void setupSecond() { }
    
    // Before/After each step
    @BeforeStep
    public void beforeStep() {
        System.out.println("Before step");
    }
    
    @AfterStep
    public void afterStep() {
        System.out.println("After step");
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

### Important Options

| Option | Description | Example |
|--------|-------------|---------|
| `features` | Path to .feature files | `"src/test/resources/features"` |
| `glue` | Step definitions package | `"com.example.steps"` |
| `plugin` | Report formats | `"html:target/reports.html"` |
| `tags` | Filter by tags | `"@smoke and not @wip"` |
| `dryRun` | Check for missing steps | `true` (without execution) |
| `monochrome` | Readable console output | `true` |

---

## 6. Tags and Organization

### Cucumber Tags

```gherkin
@regression @smoke
Feature: Login

  @positive @critical
  Scenario: Login with valid credentials
    ...

  @negative
  Scenario: Login with incorrect password
    ...

  @wip @skip
  Scenario: Login with 2FA
    ...
```

### Running by Tags

```java
// Run only @smoke tests
@ConfigurationParameter(key = "cucumber.filter.tags", value = "@smoke")

// Logical operators
@ConfigurationParameter(key = "cucumber.filter.tags", value = "@smoke or @regression")
@ConfigurationParameter(key = "cucumber.filter.tags", value = "@smoke and @critical")
@ConfigurationParameter(key = "cucumber.filter.tags", value = "@smoke and not @skip")
@ConfigurationParameter(key = "cucumber.filter.tags", value = "(@smoke or @regression) and not @wip")
```

### Recommended Organization

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

## 7. Sharing Context Between Steps

### Problem: Sharing State

Step definitions are instantiated by Cucumber. How do we share data?

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
// TestContext.java - Shared context
package com.example.context;

public class TestContext {
    private User currentUser;
    private String errorMessage;
    private UserService userService;
    
    public TestContext() {
        this.userService = new UserService();
    }
    
    // Getters and setters
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
    
    @When("I log in with {string}")
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
    
    @Then("I am logged in as {string}")
    public void verifyLogin(String name) {
        assertEquals(name, context.getCurrentUser().getName());
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

// Steps with Spring
@Component
public class UserSteps {
    
    @Autowired
    private UserService userService;
    
    @Autowired
    private TestContext context;
    
    @When("I create a user")
    public void createUser() {
        User user = userService.createUser(new User());
        context.setCurrentUser(user);
    }
}
```

---

## 8. Scenario Outline and Examples

### Scenario Outline - Parameterized Tests

```gherkin
Feature: User Validation

  Scenario Outline: Age validation
    Given a user with age <age>
    When I validate the user
    Then the result is "<result>"
    And the message is "<message>"

    Examples:
      | age | result   | message                |
      | 17  | invalid  | Doit avoir 18 ans      |
      | 18  | valid    | Utilisateur accepté    |
      | 25  | valid    | Utilisateur accepté    |
      | 65  | valid    | User accepted          |
      | 66  | invalid  | Maximum age exceeded   |

  # Multiple example tables with tags
  @jeunes
  Examples: Young users
    | age | resultat | message             |
    | 18  | valid    | Welcome!            |
    | 25  | valid    | Welcome!            |

  @seniors
  Examples: Senior users
    | age | resultat | message             |
    | 60  | valid    | Welcome senior!     |
    | 65  | valid    | Welcome senior!     |
```

```java
// Step definition
@Given("a user with age {int}")
public void userWithAge(int age) {
    context.setUser(new User("John", age));
}

@Then("the result is {string}")
public void verifyResult(String result) {
    assertEquals(result, context.getValidationResult());
}
```

---

## 9. Reports and Plugins

### Available Plugins

```java
@ConfigurationParameter(key = "cucumber.plugin", value = 
    "pretty, " +                                    // Colored console
    "html:target/cucumber-reports.html, " +         // HTML
    "json:target/cucumber.json, " +                 // JSON
    "junit:target/cucumber.xml, " +                 // JUnit XML
    "timeline:target/timeline"                      // Timeline
)
```

### Advanced HTML Report (Maven Plugin)

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
                <projectName>My Project</projectName>
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

### Screenshot on Failure

```java
@After
public void afterScenario(Scenario scenario) {
    if (scenario.isFailed()) {
        // Take a screenshot (Selenium)
        byte[] screenshot = ((TakesScreenshot) driver).getScreenshotAs(OutputType.BYTES);
        scenario.attach(screenshot, "image/png", "Screenshot");
        
        // Logs
        scenario.log("Error detected: " + scenario.getName());
    }
}
```

---

## 10. Cucumber with Selenium (UI Tests)

### Setup

```xml
<dependency>
    <groupId>org.seleniumhq.selenium</groupId>
    <artifactId>selenium-java</artifactId>
    <version>4.15.0</version>
</dependency>
```

### Complete Example

```gherkin
# login.feature
@ui @selenium
Feature: User Login

  Scenario: Successful login via the web interface
    Given I am on the login page
    When je saisis l'email "john@example.com"
    And I enter the password "password123"
    And I click on "Login"
    Then I am redirected to the dashboard
    And I see the message "Welcome John"
```

```java
// LoginSteps.java
public class LoginSteps {
    
    private final WebDriver driver;
    private LoginPage loginPage;
    
    public LoginSteps(TestContext context) {
        this.driver = context.getDriver();
    }
    
    @Given("I am on the login page")
    public void openLoginPage() {
        driver.get("http://localhost:8080/login");
        loginPage = new LoginPage(driver);
    }
    
    @When("I enter the email {string}")
    public void enterEmail(String email) {
        loginPage.enterEmail(email);
    }
    
    @When("I enter the password {string}")
    public void enterPassword(String password) {
        loginPage.enterPassword(password);
    }
    
    @When("I click on {string}")
    public void click(String button) {
        loginPage.clickLogin();
    }
    
    @Then("I am redirected to the dashboard")
    public void verifyRedirection() {
        assertTrue(driver.getCurrentUrl().contains("/dashboard"));
    }
    
    @Then("I see the message {string}")
    public void verifyMessage(String message) {
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

## 11. Cucumber with REST API

```gherkin
# api.feature
@api
Feature: Users API

  Scenario: Create a user via API
    Given the API is available
    When I send a POST request to "/api/users" with:
      """
      {
        "name": "John Doe",
        "email": "john@example.com"
      }
      """
    Then the response code is 201
    And the response contains the user's id
    And the user exists in the database
```

```java
// ApiSteps.java
public class ApiSteps {
    
    private Response response;
    private String baseUrl = "http://localhost:8080";
    
    @Given("the API is available")
    public void apiAvailable() {
        // Check that the API responds
    }
    
    @When("I send a POST request to {string} with:")
    public void sendPost(String endpoint, String body) {
        response = RestAssured
            .given()
                .contentType(ContentType.JSON)
                .body(body)
            .when()
                .post(baseUrl + endpoint);
    }
    
    @Then("the response code is {int}")
    public void verifyResponseCode(int statusCode) {
        assertEquals(statusCode, response.getStatusCode());
    }
    
    @Then("the response contains the user's id")
    public void verifyIdPresent() {
        assertNotNull(response.jsonPath().get("id"));
    }
}
```

---

## 12. Cucumber Best Practices

### ✅ DO

```gherkin
# 1. Business language, not technical
Given I am a registered user
When I view my profile
Then I see my personal information

# 2. Declarative, not imperative
Given I want to book a flight
When I search for a Paris-London flight
Then I see the available flights

# 3. One scenario = one behavior
Scenario: Successful booking
  Given a flight is available
  When I book the flight
  Then my booking is confirmed

# 4. Use Background for common setup
Background:
  Given I am logged in
  And I am on the home page

# 5. Consistent tags
@smoke @positive @api
```

### ❌ DON'T

```gherkin
# 1. Too technical
Given I make a POST to "/api/users"
When the JSON response contains {"status": "ok"}

# 2. Too imperative (UI details)
Given I click the red button at the top right
When I scroll down 500 pixels
Then I see the div with class "success-message"

# 3. Multiple behaviors in one scenario
Scenario: Create, update and delete a user
  # Too many things! Split into 3 scenarios

# 4. Too specific steps (not reusable)
Given I create exactly one user named John with email john@example.com aged 25 living in Paris

# 5. Dependencies between scenarios
Scenario: Test 1 - Create user
Scenario: Test 2 - Update the user created in Test 1  # ❌ Dependency!
```

### The 3C Rule

1. **Clear** - Easy to understand
2. **Concise** - No unnecessary details
3. **Consistent** - Uniform vocabulary

---

## 13. Frequently Asked Interview Questions

### Q1: Difference between BDD and TDD?

**Answer:**
- **TDD**: Focuses on *how* (technical implementation), for developers
- **BDD**: Focuses on *what/why* (business behavior), for the whole team
- BDD uses a natural language (Gherkin) understandable by everyone

### Q2: What is Gherkin?

**Answer:**
Gherkin is a structured but human-readable language that uses keywords like:
- `Given` (initial context)
- `When` (action)
- `Then` (expected result)

It allows writing tests understandable by non-technical people.

### Q3: Explain Given-When-Then

**Answer:**
```
Given (Initial state) - The preconditions
When (Action) - What the user does
Then (Result) - What should happen
```

Exemple:
```gherkin
Given I am logged in
When I view my cart
Then I see my items
```

### Q4: Difference between Scenario and Scenario Outline?

**Answer:**
- `Scenario`: A single test case
- `Scenario Outline`: Template with multiple data sets (Examples)

```gherkin
Scenario Outline: Test with multiple ages
  Given a user aged <age>
  Then the status is "<status>"
  
  Examples:
    | age | statut  |
    | 17  | minor   |
    | 18  | adult   |
    | 25  | adult   |
```

### Q5: What is a Step Definition?

**Answer:**
It is the Java code that implements each Gherkin step. It links the scenario to the business code.

```java
@Given("je suis connecté")
public void jesuisConnecte() {
    // Implementation code
}
```

### Q6: How to share data between steps?

**Answer:**
Use **Dependency Injection** with PicoContainer or Spring:

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

### Q7: What is Background?

**Answer:**
The `Background` contains common steps executed before **each** scenario in the feature file.

```gherkin
Background:
  Given I am logged in
  And the application is initialized

Scenario: Test 1
  # Background runs before

Scenario: Test 2
  # Background runs before aussi
```

### Q8: Difference between @Before and Background?

**Answer:**

| Aspect | @Before (Hook) | Background |
|--------|----------------|------------|
| Language | Java | Gherkin |
| Visible | No (in reports) | Yes |
| Scope | All scenarios | Feature file only |
| Usage | Technical setup | Common business steps |

### Q9: How to handle async tests?

**Answer:**
Use explicit waits:

```java
@Then("le message apparaît")
public void verifierMessage() {
    WebDriverWait wait = new WebDriverWait(driver, Duration.ofSeconds(10));
    wait.until(ExpectedConditions.visibilityOfElementLocated(By.id("message")));
    assertTrue(driver.findElement(By.id("message")).isDisplayed());
}
```

### Q10: How to organize feature files?

**Answer:**
By business feature:

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

### Q11: What is DryRun?

**Answer:**
`dryRun = true` verifies that all steps have a definition **without executing** the code.

```java
@CucumberOptions(dryRun = true)
```

Useful for:
- Checking that no step is missing
- Generating code snippets
- Quick validation

### Q12: How to filter tests by tags?

**Answer:**

```java
// Run only @smoke
@ConfigurationParameter(key = "cucumber.filter.tags", value = "@smoke")

// Combinations
value = "@smoke and @api"           // AND
value = "@smoke or @regression"     // OR
value = "@smoke and not @skip"      // AND NOT
value = "(@smoke or @critical) and not @wip"  // Complex
```

### Q13: Advantages and disadvantages of Cucumber?

**Answer:**

**Advantages ✅:**
- Improved communication (natural language)
- Automatic living documentation
- Tests understandable by everyone
- Product Owner / Dev / QA collaboration
- Step reuse

**Disadvantages ❌:**
- Learning curve
- Feature file maintenance
- Can be verbose
- Slower than unit tests
- Risk of duplication if poorly organized

### Q14: When to use Cucumber?

**Answer:**

**YES ✅ - Acceptance tests:**
- E2E functional tests
- User story validation
- Regression tests
- When business collaboration is important

**NO ❌ - Technical tests:**
- Unit tests (use JUnit/Mockito)
- Performance tests
- Low-level security tests
- Pure technical validation

---

## 14. CI/CD Integration

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

### Maven Execution

```bash
# Run all tests
mvn test

# Run only @smoke tests
mvn test -Dcucumber.filter.tags="@smoke"

# Run with specific profile
mvn test -P regression

# Generate the report
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

## 15. Complete E2E Example

### Feature File

```gherkin
@e2e @shopping
Feature: Complete purchase process
  As a customer
  I want to buy products
  In order to receive my order

  Background:
    Given the e-commerce application is available
    And the following products exist:
      | name         | price | stock |
      | Laptop       | 999   | 10    |
      | Mouse        | 29    | 50    |
      | Keyboard     | 79    | 30    |

  @smoke @positive
  Scenario: Successful product purchase
    Given I am a registered customer "john@example.com"
    And I am on the home page
    When I search for "Laptop"
    And I click on "Add to cart"
    And I go to my cart
    Then I see 1 item in the cart
    And the total is "999 €"
    When I click on "Order"
    And I enter my delivery address:
      | street    | 123 Peace Street   |
      | city      | Paris              |
      | zip code  | 75001              |
    And I confirm payment with my bank card
    Then I see the message "Order confirmed"
    And I receive a confirmation email
    And the order appears in my history

  @negative
  Scenario: Cannot buy an out-of-stock product
    Given I am on the product page "Laptop"
    And the stock is 0
    Then the "Add to cart" button is disabled
    And I see the message "Out of stock"

  @edge_case
  Scenario Outline: Promo code validation
    Given I am logged in
    And my cart contains an item of <amount> €
    When I apply the promo code "<code>"
    Then the total is <final_amount> €
    And the message is "<message>"

    Examples:
      | amount  | code       | final_amount  | message                    |
      | 100     | PROMO10    | 90            | 10% discount applied       |
      | 100     | INVALID    | 100           | Invalid promo code         |
      | 50      | PROMO10    | 50            | Minimum amount not reached |
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
    
    @Given("the e-commerce application is available")
    public void applicationAvailable() {
        assertTrue(productService.isAvailable());
    }
    
    @Given("the following products exist:")
    public void createProducts(DataTable dataTable) {
        List<Map<String, String>> rows = dataTable.asMaps();
        for (Map<String, String> row : rows) {
            Product product = new Product();
            product.setName(row.get("name"));
            product.setPrice(Double.parseDouble(row.get("price")));
            product.setStock(Integer.parseInt(row.get("stock")));
            productService.createProduct(product);
        }
    }
    
    @Given("I am a registered customer {string}")
    public void registeredCustomer(String email) {
        User user = new User(email, "password");
        context.setCurrentUser(user);
    }
    
    @Given("I am on the home page")
    public void homePage() {
        context.setCurrentPage("home");
    }
    
    @When("I search for {string}")
    public void search(String query) {
        List<Product> results = productService.search(query);
        context.setSearchResults(results);
    }
    
    @When("I click on {string}")
    public void click(String button) {
        if (button.equals("Add to cart")) {
            Product product = context.getSearchResults().get(0);
            cartService.addToCart(context.getCurrentUser(), product);
        } else if (button.equals("Order")) {
            Order order = orderService.createOrder(context.getCurrentUser());
            context.setCurrentOrder(order);
        }
    }
    
    @When("I go to my cart")
    public void goToCart() {
        Cart cart = cartService.getCart(context.getCurrentUser());
        context.setCurrentCart(cart);
    }
    
    @Then("I see {int} item in the cart")
    public void verifyItemCount(int count) {
        assertEquals(count, context.getCurrentCart().getItems().size());
    }
    
    @Then("the total is {string}")
    public void verifyTotal(String amount) {
        String actualTotal = context.getCurrentCart().getTotal() + " €";
        assertEquals(amount, actualTotal);
    }
    
    @When("I enter my delivery address:")
    public void enterAddress(DataTable dataTable) {
        Map<String, String> address = dataTable.asMap();
        Address deliveryAddress = new Address();
        deliveryAddress.setStreet(address.get("street"));
        deliveryAddress.setCity(address.get("city"));
        deliveryAddress.setZipCode(address.get("zip code"));
        context.setDeliveryAddress(deliveryAddress);
    }
    
    @When("I confirm payment with my bank card")
    public void confirmPayment() {
        Payment payment = new Payment("CARD", "1234-5678-9012-3456");
        orderService.processPayment(context.getCurrentOrder(), payment);
    }
    
    @Then("I see the message {string}")
    public void verifyMessage(String message) {
        assertTrue(context.getDisplayedMessages().contains(message));
    }
    
    @Then("I receive a confirmation email")
    public void verifyEmail() {
        String email = context.getCurrentUser().getEmail();
        assertTrue(context.getEmailService().hasReceivedEmail(email));
    }
    
    @Then("the order appears in my history")
    public void verifyHistory() {
        List<Order> orders = orderService.getOrderHistory(context.getCurrentUser());
        assertTrue(orders.contains(context.getCurrentOrder()));
    }
    
    @Given("the stock is {int}")
    public void setStock(int stock) {
        Product product = context.getCurrentProduct();
        product.setStock(stock);
    }
    
    @Then("the {string} button is disabled")
    public void verifyButtonDisabled(String button) {
        assertFalse(context.isButtonEnabled(button));
    }
    
    @Given("my cart contains an item of {double} €")
    public void cartWithAmount(double amount) {
        Product product = new Product("Test", amount);
        cartService.addToCart(context.getCurrentUser(), product);
    }
    
    @When("I apply the promo code {string}")
    public void applyPromo(String code) {
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
        System.out.println("🚀 Initializing E2E test");
        context.initializeDatabase();
        context.startApplication();
    }
    
    @After("@e2e")
    public void cleanupE2E(Scenario scenario) {
        if (scenario.isFailed()) {
            System.out.println("❌ Scenario failed: " + scenario.getName());
            scenario.log("Cart state: " + context.getCurrentCart());
            scenario.log("User: " + context.getCurrentUser());
        }
        context.cleanupDatabase();
    }
    
    @Before("@shopping")
    public void setupShopping() {
        context.initializeCart();
    }
    
    @AfterStep
    public void afterEachStep() {
        // Log for debug
        System.out.println("Step executed");
    }
}
```

---

## 16. Patterns and Anti-Patterns

### ✅ GOOD PATTERNS

#### 1. Page Object Model (with Selenium)

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

#### 2. Builder Pattern for test data

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

// Usage
@Given("un utilisateur standard")
public void creerUtilisateurStandard() {
    User user = new UserBuilder()
        .withName("John")
        .withEmail("john@example.com")
        .build();
    context.setUser(user);
}
```

#### 3. Reusable steps

```java
// ❌ Bad - Too specific steps
@Given("je crée un utilisateur John Doe avec l'email john@example.com")
public void creerJohnDoe() { }

// ✅ Good - Generic reusable steps
@Given("I create a user {string} with email {string}")
public void createUser(String name, String email) { }
```

### ❌ ANTI-PATTERNS

#### 1. Strong coupling with the UI

```gherkin
# ❌ Bad
Given I click the button with id "btn-submit-123"
And I see the div with class "alert-success"

# ✅ Good
Given I submit the form
And I see a success message
```

#### 2. Steps with business logic

```java
// ❌ Bad - Logic in the step
@When("je calcule la réduction")
public void calculerReduction() {
    double price = context.getPrice();
    double discount = price * 0.1; // Logique métier ici!
    context.setDiscount(discount);
}

// ✅ Good - Delegate to the service
@When("je calcule la réduction")
public void calculerReduction() {
    double discount = discountService.calculate(context.getPrice());
    context.setDiscount(discount);
}
```

#### 3. Scenarios with dependencies

```gherkin
# ❌ Bad - Dépendance entre scénarios
Scenario: Create a user
  When I create the user "John"

Scenario: Update the previously created user
  When I update the user "John"  # Depends on previous scenario!

# ✅ Good - Scénarios indépendants
Scenario: Update an existing user
  Given a user "John" exists
  When I update the user "John"
```

---

## 17. Cucumber Interview Checklist

### Basic concepts
- [ ] Explain BDD vs TDD
- [ ] What is Gherkin?
- [ ] Given-When-Then
- [ ] Feature, Scenario, Step

### Cucumber Java
- [ ] Step Definitions (@Given, @When, @Then)
- [ ] Hooks (@Before, @After)
- [ ] Test Runner configuration
- [ ] DataTable and DocString

### Advanced
- [ ] Scenario Outline + Examples
- [ ] Background vs @Before
- [ ] Dependency Injection (PicoContainer)
- [ ] Tags and filtering

### Integration
- [ ] Cucumber + Selenium
- [ ] Cucumber + REST Assured
- [ ] Cucumber + Spring Boot
- [ ] Reports (HTML, JSON)

### Best Practices
- [ ] AAA pattern in steps
- [ ] Page Object Model
- [ ] Reusable steps
- [ ] Avoid anti-patterns

---

## 18. Quick Summary (For a 5-minute review before the interview)

### Gherkin - 3 essential keywords
```gherkin
Given (context) - Initial state
When (action) - What the user does  
Then (result) - What should happen
```

### Java Annotations - Top 5
```java
@Given("...") - Precondition
@When("...") - Action
@Then("...") - Assertion
@Before - Hook before scenario
@After - Hook after scenario
```

### Project structure
```
features/        → .feature files (Gherkin)
steps/           → Step Definitions (Java)
runners/         → Test Runner (JUnit)
hooks/           → Hooks (@Before/@After)
```

### Essential Runner Options
```java
features = "path/to/features"
glue = "steps.package"
tags = "@smoke and not @skip"
plugin = "html:target/reports.html"
```

### Trick question #1
**Q: Does Cucumber replace JUnit/Mockito?**
**A: NO!** Cucumber is for E2E acceptance tests. JUnit/Mockito for unit tests.

### Test pyramid with Cucumber
```
     E2E (Cucumber)     5%
    Integration        15%
   Unit (JUnit)       80%
```

---

## Good to know for the interview

### Points to mention spontaneously:
1. **"I use Cucumber for acceptance tests"** - Shows you know when to use it
2. **"I prefer business language in Gherkin"** - Good practice
3. **"I organize my features by functionality"** - Organization
4. **"I use the Page Object Model with Selenium"** - Known pattern
5. **"I filter my tests by tags for CI/CD"** - DevOps practice

### Phrases to avoid:
- ❌ "Cucumber replaces unit tests"
- ❌ "I put all the logic in the steps"
- ❌ "My scenarios describe UI details"
- ❌ "I don't use hooks"

### If asked for an example:
Use the e-commerce example (section 15) - it is complete and professional!
