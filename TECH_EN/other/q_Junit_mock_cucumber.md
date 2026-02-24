# Interview Questions - Unit Testing & Cucumber

## 🧪 ADVANCED UNIT TESTING

**Q117: What is Test-Driven Development (TDD)?**
Methodology: write test before code. Red-Green-Refactor cycle: 1) Test fails (Red), 2) Minimum code to pass (Green), 3) Improve code (Refactor). Guarantees testability, better design, living documentation.

**Q118: Difference between Unit Test, Integration Test, and E2E Test?**
Unit: tests an isolated unit (method/class), mocks dependencies, fast, numerous. Integration: tests component interactions (service+repository), real DB, medium. E2E: tests the full system (UI→DB), slow, few. Test pyramid.

**Q119: What is the test pyramid?**
Many unit tests (base), fewer integration tests (middle), few E2E tests (top). Cost/speed increase toward the top. 70% unit, 20% integration, 10% E2E recommended.

**Q120: What is the AAA pattern (Arrange-Act-Assert)?**
Clear test structure: Arrange (prepare data/mocks), Act (execute tested action), Assert (verify result). Improves readability. Alternative: Given-When-Then (BDD).

**Q121: What is Mockito?**
Java mocking framework. Create mock/stub objects. when().thenReturn() for behavior. verify() for interactions. @Mock, @InjectMocks annotations. Essential for test isolation.

**Q122: Difference between Mock, Stub, and Spy?**
Mock: simulated object, verifies interactions (verify). Stub: returns predefined data, no verification. Spy: partial real object, some methods mocked, others real. Spy with Mockito.spy().

**Q123: How to create a Mock with Mockito?**
Manual: MyService mock = Mockito.mock(MyService.class). Annotation: @Mock MyService service + @ExtendWith(MockitoExtension.class). when(service.findById(1L)).thenReturn(user). No real implementation.

**Q124: What is @InjectMocks?**
Injects mocks into the tested object. @InjectMocks UserController controller; @Mock UserService service; Mockito automatically injects service into controller. Alternative: manual constructor injection.

**Q125: How to verify that a method was called?**
verify(mock).methodName(args). verify(mock, times(2)).method(). verify(mock, never()).method(). verify(mock, atLeastOnce()). verifyNoInteractions(mock). verifyNoMoreInteractions(mock).

**Q126: What is ArgumentCaptor?**
Captures arguments passed to mocks. ArgumentCaptor<User> captor = ArgumentCaptor.forClass(User.class). verify(mock).save(captor.capture()). User captured = captor.getValue(). Useful for complex argument assertions.

**Q127: What is ArgumentMatchers?**
Flexible argument matchers. any(), anyString(), anyInt(), eq(), isNull(), argThat(). Example: when(service.findBy(anyString(), eq(25))).thenReturn(users). Note: all args must be matched or none.

**Q128: How to mock void methods?**
doNothing().when(mock).voidMethod(). doThrow(Exception.class).when(mock).method(). doAnswer(invocation -> {...}).when(mock).method(). Verify with verify(mock).method().

**Q129: How to mock static methods?**
Mockito 3.4+: try (MockedStatic<MyClass> mock = mockStatic(MyClass.class)) { mock.when(MyClass::staticMethod).thenReturn(value); }. Scope limited to try-with-resources. Alternative: PowerMock (legacy).

**Q130: What is @Spy vs @Mock?**
@Mock: completely simulated object, no real logic. @Spy: real object, real methods called unless stubbed. when() vs doReturn().when() for spy. Spy useful for partial mocking.

**Q131: How to test exceptions?**
JUnit 5: assertThrows(Exception.class, () -> method()). Exception e = assertThrows(...); assertEquals("message", e.getMessage()). Mockito: when().thenThrow(new Exception()). @Test(expected=...) JUnit 4 (obsolete).

**Q132: What is @ParameterizedTest?**
Test with multiple input values. @ValueSource, @CsvSource, @MethodSource, @EnumSource. Example: @ParameterizedTest @ValueSource(ints={1,2,3}) void test(int value). Reduces duplication.

**Q133: Difference between @BeforeEach and @BeforeAll?**
@BeforeEach: runs before each test, instance variables. @BeforeAll: runs once before all tests, static, expensive setup. @AfterEach/@AfterAll for cleanup. JUnit 5 annotations.

**Q134: What is @Nested in JUnit 5?**
Groups related tests in an inner class. Hierarchical structure. Improves organization and readability. @DisplayName for descriptive names. Specific setup per group.

**Q135: How to test asynchronous code?**
CompletableFuture: get() with timeout. Awaitility library: await().atMost(5, SECONDS).until(() -> condition). CountDownLatch for threads. @Async tests with @SpringBootTest.

**Q136: What is Test Coverage?**
Percentage of code executed by tests. Tools: JaCoCo, Cobertura. Metrics: line, branch, method coverage. 80%+ recommended but quality > quantity. Does not guarantee absence of bugs.

**Q137: How to generate a coverage report?**
Maven: jacoco-maven-plugin, mvn test jacoco:report. HTML report in target/site/jacoco. Gradle: jacoco plugin, jacocoTestReport task. CI/CD integration (SonarQube, Codecov).

**Q138: What is Mutation Testing?**
Tests the quality of tests by modifying code (mutants). Tests must detect mutations. PIT/Pitest for Java. Mutation score = killed mutants / total. Complementary to coverage.

**Q139: Best practices for unit tests?**
FIRST: Fast, Independent, Repeatable, Self-validating, Timely. One assert per test (ideally). Descriptive names. No complex logic. Isolate dependencies. Deterministic tests. Mock external systems (DB, API).

**Q140: How to name test methods?**
should_ExpectedBehavior_When_StateUnderTest. given_Precondition_when_Action_then_Result. testMethodName_ExpectedResult. Descriptive, readable. Team convention. @DisplayName in JUnit 5 for full sentences.

**Q141: What is a Test Double?**
Generic term for test objects. Types: Dummy (passed but not used), Stub (returns data), Spy (records calls), Mock (verifies behavior), Fake (simplified implementation). Mockito covers most of them.

**Q142: How to test private methods?**
Generally: DO NOT test directly. Test via public methods. If necessary: Reflection (discouraged), make package-private, extract into a separate class. Need to test private = code smell.

**Q143: What is AssertJ?**
Fluent assertion library. Readable syntax: assertThat(actual).isEqualTo(expected).isNotNull(). Chained assertions. Better error messages. assertThat(list).hasSize(3).contains("a", "b"). Included in spring-boot-starter-test.

**Q144: Difference between assertEquals and assertThat?**
assertEquals: native JUnit, order matters (expected, actual). assertThat: AssertJ/Hamcrest, fluent, readable, better messages. assertThat is more modern and expressive. Prefer assertThat.

**Q145: How to test with a database?**
@DataJpaTest + H2 in-memory. @Sql scripts for data. TestEntityManager for CRUD. @Transactional auto rollback. Testcontainers for real DB (Docker). Avoid using production DB.

**Q146: What is Testcontainers?**
Java library for testing with Docker containers. Real DB (PostgreSQL, MySQL), Redis, Kafka, etc. @Container annotation. Lifecycle managed automatically. More realistic tests than in-memory.

**Q147: How to configure Testcontainers?**
Add testcontainers dependency. @Testcontainers on class. @Container static container = new PostgreSQLContainer<>("postgres:14"). Configure spring.datasource.* from container. DynamicPropertySource for properties.

**Q148: What is @DirtiesContext?**
Reloads the ApplicationContext after the test. Useful if a test modifies beans/state. Expensive, avoid if possible. ClassMode: BEFORE/AFTER_CLASS/EACH_TEST_METHOD. MethodMode: BEFORE/AFTER_METHOD.

**Q149: How to test Scheduled tasks?**
Disable scheduling: spring.task.scheduling.enabled=false in test. Call the method directly. AwaitableJobRunner pattern. @MockBean for dependencies. Verify logic, not timing.

**Q150: How to test Spring Events?**
@ApplicationEventPublisher in tests. @EventListener on test method. Verify mock listeners. @TransactionalEventListener tested with @Transactional. Capture events with a custom listener.

## 🥒 CUCUMBER & BDD

**Q151: What is Cucumber?**
BDD (Behavior-Driven Development) framework. Tests in natural language (Gherkin). Business/tech collaboration. Features describe behaviors. Step definitions (Java) execute them. Executable specifications.

**Q152: What is BDD (Behavior-Driven Development)?**
Behavior-focused agile methodology. Product Owner/Devs/QA collaboration. Executable specifications. Given-When-Then syntax. Tests = documentation. TDD + ubiquitous language.

**Q153: What is Gherkin?**
Language for writing Cucumber scenarios. Keywords: Feature, Scenario, Given, When, Then, And, But, Background, Scenario Outline, Examples. Readable by non-technical people. .feature files.

**Q154: Structure of a .feature file?**
```gherkin
Feature: Feature description
  
  Scenario: Scenario description
    Given initial context
    When action is performed
    Then expected result
    And additional step
```
Feature groups related scenarios. Scenario = test case.

**Q155: Difference between Scenario and Scenario Outline?**
Scenario: single test with fixed values. Scenario Outline: template with parameters, Examples table. Avoids duplication of similar scenarios. Data-driven testing.

**Q156: Scenario Outline example?**
```gherkin
Scenario Outline: Verify login
  Given a user with username "<username>"
  When they enter the password "<password>"
  Then they should be "<result>"
  
  Examples:
    | username | password | result    |
    | john     | pass123  | logged in |
    | jane     | wrong    | rejected  |
```

**Q157: What are Step Definitions?**
Java code implementing Gherkin steps. @Given, @When, @Then annotations. Parameters extracted via regex or Cucumber Expressions. Class with methods mapping to steps. Glue between feature and code.

**Q158: How to write a Step Definition?**
```java
@Given("a user with email {string}")
public void aUserWithEmail(String email) {
    user = new User(email);
}

@When("they create an account")
public void theyCreateAnAccount() {
    response = userService.createAccount(user);
}

@Then("the account should be created successfully")
public void accountCreatedSuccessfully() {
    assertEquals(201, response.getStatus());
}
```

**Q159: What are Cucumber Expressions?**
Alternative to regex for parameters. Simpler: {int}, {float}, {string}, {word}. Example: "I have {int} apples" instead of "I have (\\d+) apples". Type-safe, readable.

**Q160: How to share state between steps?**
Shared context/world class. Dependency injection (PicoContainer, Spring). Instance attributes on step definitions class. ScenarioContext pattern. Avoid static (parallelization issues).

**Q161: What are Cucumber Hooks?**
Methods executed before/after scenarios. @Before, @After, @BeforeStep, @AfterStep. Setup/cleanup. Order with order parameter. @Before(order=1). Conditional with tags.

**Q162: How to use Cucumber Tags?**
Organize/filter scenarios. @tag on Feature/Scenario. Example: @smoke, @regression, @wip. Runner: tags = "@smoke and not @wip". CI/CD run a subset of tests.

**Q163: What is Background in Cucumber?**
Steps common to all scenarios in a feature. Runs before each Scenario. Avoids repeating Given steps. Example: common login, data setup.

**Q164: How to configure Cucumber with JUnit 5?**
```java
@Suite
@IncludeEngines("cucumber")
@SelectClasspathResource("features")
@ConfigurationParameter(key = PLUGIN_PROPERTY_NAME, value = "pretty")
@ConfigurationParameter(key = GLUE_PROPERTY_NAME, value = "com.example.steps")
public class CucumberTest {
}
```
Dependency: cucumber-junit-platform-engine.

**Q165: How to configure Cucumber with Spring Boot?**
Dependency: cucumber-spring. @CucumberContextConfiguration + @SpringBootTest on config class. Steps autowire Spring beans. @DirtiesContext if necessary. Integration testing.

**Q166: Difference between @CucumberContextConfiguration and @SpringBootTest?**
@CucumberContextConfiguration: marks the Cucumber-Spring config class. @SpringBootTest: loads the Spring ApplicationContext. Combine on the same class for integration. One class per project.

**Q167: How to generate Cucumber reports?**
Plugin in runner: "html:target/cucumber-reports.html", "json:target/cucumber.json", "junit:target/cucumber.xml". Cucumber Reports (Maven plugin) for advanced HTML. Jenkins Cucumber Reports plugin.

**Q168: What are Data Tables in Cucumber?**
Data tables inside steps. Example:
```gherkin
Given the following users:
  | name | email          |
  | John | john@email.com |
  | Jane | jane@email.com |
```
Step definition: DataTable dataTable parameter. dataTable.asMaps(), asLists().

**Q169: How to parameterize Step Definitions with Data Tables?**
```java
@Given("the following users:")
public void theFollowingUsers(DataTable dataTable) {
    List<Map<String, String>> rows = dataTable.asMaps();
    rows.forEach(row -> {
        String name = row.get("name");
        String email = row.get("email");
        users.add(new User(name, email));
    });
}
```

**Q170: What are Doc Strings in Cucumber?**
Multi-line text inside steps. Delimited by """. Example:
```gherkin
When I submit the following JSON:
  """
  {
    "name": "John",
    "email": "john@test.com"
  }
  """
```
Step definition: String docString parameter.

**Q171: How to handle authentication in Cucumber?**
Background with login step. Store token in ScenarioContext. RestAssured RequestSpecification with auth. @Before hook for setup. Avoid hardcoded credentials.

**Q172: How to test a REST API with Cucumber?**
RestAssured in step definitions. Given spec, When request, Then response. Example:
```java
@When("I make a GET request to {string}")
public void makeGetRequest(String endpoint) {
    response = given().get(endpoint);
}

@Then("the status code should be {int}")
public void verifyStatus(int status) {
    response.then().statusCode(status);
}
```

**Q173: What is the Page Object pattern with Cucumber?**
Separate UI logic from steps. Page Object classes for web pages. Step definitions delegate to Page Objects. Maintainability, reusability. Selenium + Cucumber.

**Q174: How to parallelize Cucumber tests?**
JUnit 5: @Execution(PARALLEL). Maven Surefire: forkCount/parallel. Cucumber 7+: parallel execution. Watch out for shared state. Thread-safe contexts. Configuration depends on runner.

**Q175: Cucumber best practices?**
Reusable steps, correct granularity (not too atomic nor too broad). Avoid business logic in steps. One scenario = one behavior. Tags for organization. Background for common setup. Explicit names. Business-readable Gherkin.

**Q176: Difference between Imperative and Declarative Gherkin?**
Imperative (bad): details UI clicks/actions. "Click button, enter text". Declarative (good): describes intent. "Create an account". More maintainable, abstracts implementation.

**Q177: How to debug Cucumber tests?**
Breakpoints in step definitions. Logs in steps. @Before hook with Scenario.log(). Screenshots on failure. Cucumber dry-run to verify mapping. IDE support (IntelliJ IDEA).

**Q178: What is cucumber.properties?**
Cucumber configuration file. src/test/resources/cucumber.properties. Configures glue, plugin, features path, tags. Alternative: @ConfigurationParameter annotations.

**Q179: How to manage test data with Cucumber?**
Test data builders. JSON/CSV files. @Before hooks setup. Database seeding via @Sql. Test fixtures. Avoid hardcoding in features.

**Q180: How to integrate Cucumber into CI/CD?**
Maven/Gradle task runs tests. JSON reports parsed. Jenkins Cucumber Reports plugin. GitLab CI artifacts. Conditional execution by tags. Parallelization for speed.

**Q181: What are Custom Parameter Types?**
Custom types for parameters. Alternative to enums/complex objects. @ParameterType annotation. Transform String to object. Improves readability and reusability.

**Q182: Custom Parameter Type example?**
```java
@ParameterType("admin|user|guest")
public UserRole role(String value) {
    return UserRole.valueOf(value.toUpperCase());
}

// In feature: Given I am a {role}
// Step: public void iAmRole(UserRole role)
```

**Q183: How to handle multiple environments?**
Spring Profiles (@ActiveProfiles). Properties configuration per env. Environment variables. @ConfigurationParameter. Cucumber tags per env. Maven build profiles.

**Q184: Difference between Cucumber JVM and Cucumber Ruby?**
Cucumber JVM: Java/Kotlin/Scala, Java step definitions, Spring integration. Cucumber Ruby: original, Ruby steps, RSpec integration. Same Gherkin syntax. JVM more common in Java shops.

**Q185: How to test WebSocket with Cucumber?**
StompSession in context. @Before connect, @After disconnect. Steps for send/receive messages. Awaitility for async. Mock WebSocket server if necessary.

**Q186: What are tagged Examples?**
Tag individual Examples rows. Run a subset of data. Example:
```gherkin
Examples:
  | username | password |
  @valid
  | john     | pass123  |
  @invalid
  | jane     | wrong    |
```
Run: @valid only.

**Q187: How to handle waits/timeouts in Cucumber?**
Awaitility in steps. Selenium implicit waits. Retry logic. Polling conditions. await().atMost(10, SECONDS).until(() -> condition). Avoid Thread.sleep().

**Q188: How to measure coverage with Cucumber?**
JaCoCo works. Cucumber = integration tests, coverage differs from unit tests. Per-feature coverage possible. Mutation testing less suited.

**Q189: Limitations of Cucumber?**
Setup/maintenance overhead. Not suited for unit tests. Can become verbose. Step duplication if poorly organized. Long performance tests. Team learning curve.

**Q190: When to use Cucumber vs classic tests?**
Cucumber: acceptance tests, business collaboration, living documentation, E2E scenarios. Classic tests: unit tests, performance, exhaustive coverage. Combine as needed.

## 🎯 ADVANCED TESTING QUESTIONS

**Q191: What is Contract Testing?**
Tests the contract between services (consumer/provider). Pact framework. Consumer defines expectations, Provider validates. Avoids breaking API changes. Alternative to costly integration tests. Microservices.

**Q192: What is Chaos Engineering testing?**
Tests resilience by introducing failures. Chaos Monkey (Netflix). Simulate network, CPU, memory failures. Verifies graceful degradation. Production testing.

**Q193: How to test performance in Spring Boot?**
JMeter, Gatling for load testing. @SpringBootTest with profiling. Actuator metrics. Microbenchmark with JMH. Database query performance. Response time assertions.

**Q194: What is Approval Testing?**
Compares output to an approved snapshot. Useful for complex data (JSON, HTML). ApprovalTests library. Approve once, detect changes. Complements classic assertions.

**Q195: How to test application security?**
OWASP dependency check. Security unit tests (@WithMockUser). SQL injection tests. XSS prevention. CSRF token validation. Authorization tests. Penetration testing tools.

**Q196: What is Property-Based Testing?**
Generate random inputs, verify properties. Quickcheck (Haskell), jqwik (Java). Finds edge cases. Example: reverse(reverse(x)) == x. Complements example-based testing.

**Q197: How to test legacy code without tests?**
Characterization tests (capture current behavior). Progressive refactoring with tests. Seams (modification points). Test harness. Approval testing. Book: "Working Effectively with Legacy Code".

**Q198: What are Flaky Tests?**
Unstable tests (pass/fail randomly). Causes: timing, execution order, shared state, external dependencies. Solutions: isolate, fixtures, deterministic data, retry logic (last resort).

**Q199: How to detect and fix Flaky Tests?**
Run N times. Analyze logs/patterns. Isolate dependencies. Fixed data/time. Turn off parallel execution. Awaitility for async. CI/CD tracking. Quarantine flaky tests.

**Q200: What is Test Observability?**
Understanding why tests fail. Detailed logs. Screenshots on failure. Clear stack traces. Test metrics. CI/CD reporting. Easier debugging.

---

## 💡 ADDITIONAL TIPS

**Testing Best Practices:**
- Test behavior, not implementation
- Fast and deterministic tests
- Total independence between tests
- Explicit names and documentation
- Refactor tests like production code
- Coverage is not quality
- Tests = first line of documentation

**Cucumber Best Practices:**
- Write features BEFORE implementation
- Involve Product Owner in writing
- Short and focused scenarios
- Avoid technical details in Gherkin
- Maximize step reuse
- Tags for smart organization
- Keep features up to date

**Avoid:**
- Tests dependent on execution order
- Complex logic in tests
- Excessive mocks (brittleness)
- Tests too tightly coupled to implementation
- Ignoring broken tests
- Tests without asserts
- Copy-paste without understanding

Good luck with your interview! 🚀💪






























<br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br />