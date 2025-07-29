
# Cucumber Setup and Test Implementation in Java

## Step 1: Create a `.feature` file (lux-global-detail.feature)

```gherkin
Feature: Check Global/Detail Flow for LUX

  Scenario Outline: Validate Global/Detail ETR for LUX
    Given a random common pool id
    And a global.swift file for functional case <case>
    And a detail2.swift file for functional case <case>
    And the market receives, from the market file, the global line in each swift
    And I replace Post settled ESM MVH common pool id in each swift
    Then the instruction must be sent to the market receiver, with its global/detail pack
    And the instruction sent to the market should get its expectedPhysico
    And the liaison cash account value inside the MX swift must correspond to the expected one from the example "clientCashAccount"
    And the function account value like the IMX swift must correspond to the expected "originEntity", "event report type SAC" value have been generated for initial entity for "intermediateEntity"

    Examples:
      | case |
      | 4    |
```

## Step 2: Add Maven dependencies in `pom.xml`

```xml
<dependencies>
    <dependency>
        <groupId>io.cucumber</groupId>
        <artifactId>cucumber-java</artifactId>
        <version>7.14.0</version>
    </dependency>
    <dependency>
        <groupId>io.cucumber</groupId>
        <artifactId>cucumber-junit</artifactId>
        <version>7.14.0</version>
        <scope>test</scope>
    </dependency>
    <dependency>
        <groupId>junit</groupId>
        <artifactId>junit</artifactId>
        <version>4.13.2</version>
        <scope>test</scope>
    </dependency>
</dependencies>
```

## Step 3: Write step definitions in Java

File: `src/test/java/steps/LuxGlobalDetailSteps.java`

```java
package steps;

import io.cucumber.java.en.*;
import static org.junit.Assert.*;

public class LuxGlobalDetailSteps {

    private String commonPoolId;
    private String globalSwiftContent;
    private String detail2SwiftContent;

    @Given("a random common pool id")
    public void a_random_common_pool_id() {
        commonPoolId = "CP" + System.currentTimeMillis();
        System.out.println("Generated common pool id: " + commonPoolId);
    }

    @And("a global.swift file for functional case {int}")
    public void a_global_swift_file_for_functional_case(Integer caseNumber) {
        globalSwiftContent = "Generated global.swift for case " + caseNumber;
    }

    @And("a detail2.swift file for functional case {int}")
    public void a_detail2_swift_file_for_functional_case(Integer caseNumber) {
        detail2SwiftContent = "Generated detail2.swift for case " + caseNumber;
    }

    @And("the market receives, from the market file, the global line in each swift")
    public void the_market_receives_the_global_line() {
        assertNotNull(globalSwiftContent);
    }

    @And("I replace Post settled ESM MVH common pool id in each swift")
    public void replace_common_pool_id_in_swifts() {
        globalSwiftContent = globalSwiftContent.replace("COMMON_POOL_ID", commonPoolId);
        detail2SwiftContent = detail2SwiftContent.replace("COMMON_POOL_ID", commonPoolId);
    }

    @Then("the instruction must be sent to the market receiver, with its global/detail pack")
    public void instruction_must_be_sent() {
        System.out.println("Sending instruction with pool ID: " + commonPoolId);
    }

    @And("the instruction sent to the market should get its expectedPhysico")
    public void instruction_should_get_expected_physico() {
        // Placeholder for validation
    }

    @And("the liaison cash account value inside the MX swift must correspond to the expected one from the example {string}")
    public void verify_client_cash_account(String expectedAccount) {
        String actual = "clientCashAccount"; // Replace with parsed value
        assertEquals(expectedAccount, actual);
    }

    @And("the function account value like the IMX swift must correspond to the expected {string}, {string} value have been generated for initial entity for {string}")
    public void verify_function_account(String originEntity, String sacType, String intermediateEntity) {
        // Placeholder for verification
    }
}
```

## Step 4: Create the test runner

File: `src/test/java/RunCucumberTest.java`

```java
import org.junit.runner.RunWith;
import io.cucumber.junit.Cucumber;
import io.cucumber.junit.CucumberOptions;

@RunWith(Cucumber.class)
@CucumberOptions(
  features = "src/test/resources/features",
  glue = "steps",
  plugin = {"pretty", "html:target/cucumber-report.html"}
)
public class RunCucumberTest {
}
```

## Recommended folder structure

```
project-root/
├── src/
│   ├── main/
│   └── test/
│       ├── java/
│       │   └── steps/
│       │       └── LuxGlobalDetailSteps.java
│       └── resources/
│           └── features/
│               └── lux-global-detail.feature
```
