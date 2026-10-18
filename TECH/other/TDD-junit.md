# Test-Driven Development (TDD) avec JUnit et Java

> **Cours complet et détaillé — du concept à la pratique**

---

## Table des matières

1. [Introduction au Test Unitaire](#1-introduction-au-test-unitaire)
2. [Pourquoi le TDD ?](#2-pourquoi-le-tdd-)
3. [Le cycle TDD : Red → Green → Refactor](#3-le-cycle-tdd--red--green--refactor)
4. [Mise en place de l'environnement](#4-mise-en-place-de-lenvironnement)
5. [JUnit 5 — Fondamentaux](#5-junit-5--fondamentaux)
6. [Étape 1 — RED : Écrire un test qui échoue](#6-étape-1--red--écrire-un-test-qui-échoue)
7. [Étape 2 — GREEN : Faire passer le test](#7-étape-2--green--faire-passer-le-test)
8. [Étape 3 — REFACTOR : Améliorer le code](#8-étape-3--refactor--améliorer-le-code)
9. [Cas pratique complet : Calculatrice bancaire](#9-cas-pratique-complet--calculatrice-bancaire)
10. [Les Assertions avancées](#10-les-assertions-avancées)
11. [Les Annotations JUnit 5 essentielles](#11-les-annotations-junit-5-essentielles)
12. [Mocking avec Mockito](#12-mocking-avec-mockito)
13. [TDD avec des services Spring Boot](#13-tdd-avec-des-services-spring-boot)
14. [Bonnes pratiques TDD](#14-bonnes-pratiques-tdd)
15. [Anti-patterns à éviter](#15-anti-patterns-à-éviter)
16. [Résumé visuel du cycle TDD](#16-résumé-visuel-du-cycle-tdd)

---

## 1. Introduction au Test Unitaire

### Qu'est-ce qu'un test unitaire ?

Un **unit test** est un test automatisé qui vérifie le comportement d'une **unité de code isolée** — généralement une méthode ou une classe — indépendamment des autres composants du système.

Une unité de code bien testée doit :

- Avoir un **comportement prévisible** pour des entrées données
- Être **isolée** des dépendances externes (base de données, API, système de fichiers)
- Être **rapide** à exécuter (moins de quelques millisecondes)
- Être **répétable** : le même test donne toujours le même résultat

### La pyramide des tests

```
          /\
         /  \
        / E2E\          ← Tests End-to-End (rares, lents, coûteux)
       /------\
      /        \
     / Integration\     ← Tests d'intégration (quelques-uns)
    /------------\
   /              \
  /   Unit Tests   \    ← Tests unitaires (nombreux, rapides, peu coûteux)
 /------------------\
```

Les **unit tests** forment la base de la pyramide : ils doivent être **nombreux**, **rapides** et **fiables**.

---

## 2. Pourquoi le TDD ?

### Problèmes du développement classique (Code First)

Sans TDD, le développeur écrit d'abord le code métier, puis (si le temps le permet) les tests. Cette approche entraîne plusieurs problèmes :

- Les tests sont écrits **après coup**, souvent sous pression
- Le code n'est pas naturellement **testable** (couplage fort, dépendances cachées)
- Les bugs sont découverts **tardivement**, ce qui coûte cher à corriger
- La **confiance** dans le code est faible lors des modifications

### Les bénéfices du TDD

| Bénéfice | Description |
|----------|-------------|
| **Design guidé par les tests** | Le code est naturellement découplé et modulaire |
| **Régression automatique** | Chaque modification est validée par la suite de tests |
| **Documentation vivante** | Les tests décrivent le comportement attendu |
| **Confiance accrue** | On peut refactorer sans craindre de casser quelque chose |
| **Feedback immédiat** | On sait immédiatement si une modification est correcte |

### La règle d'or du TDD

> **"Ne jamais écrire une ligne de code de production sans avoir d'abord un test qui échoue."**
> — Kent Beck, créateur du TDD

---

## 3. Le cycle TDD : Red → Green → Refactor

Le TDD repose sur un cycle court et itératif en **3 étapes** :

```
    ┌─────────────────────────────────────────────────┐
    │                                                 │
    │    ① RED          ② GREEN        ③ REFACTOR    │
    │   Écrire un    Écrire le code   Améliorer le   │
    │   test qui     minimal pour     code sans       │
    │   échoue       faire passer     changer son     │
    │                le test          comportement    │
    │                                                 │
    │   🔴 ──────────► 🟢 ──────────► 🔵 ──────┐    │
    │   ▲                                        │    │
    │   └────────────────────────────────────────┘    │
    │              (itération suivante)                │
    └─────────────────────────────────────────────────┘
```

### Règles strictes de chaque étape

**🔴 RED**
- Écrire **un seul test** à la fois
- Le test doit **compiler** (la classe/méthode peut exister vide)
- Le test doit **échouer** pour la bonne raison (assertion failed, pas compilation error)

**🟢 GREEN**
- Écrire le **minimum de code** pour faire passer le test
- Ne pas anticiper les futurs besoins
- Même un code "sale" est acceptable à cette étape

**🔵 REFACTOR**
- Améliorer la structure du code **sans modifier son comportement**
- Les tests doivent **toujours passer** après refactoring
- Supprimer la duplication, améliorer les noms, extraire des méthodes

---

## 4. Mise en place de l'environnement

### Dépendances Maven (`pom.xml`)

```xml
<dependencies>

    <!-- JUnit 5 (Jupiter) -->
    <dependency>
        <groupId>org.junit.jupiter</groupId>
        <artifactId>junit-jupiter</artifactId>
        <version>5.10.0</version>
        <scope>test</scope>
    </dependency>

    <!-- Mockito pour les Mocks -->
    <dependency>
        <groupId>org.mockito</groupId>
        <artifactId>mockito-junit-jupiter</artifactId>
        <version>5.5.0</version>
        <scope>test</scope>
    </dependency>

    <!-- AssertJ pour des assertions fluides -->
    <dependency>
        <groupId>org.assertj</groupId>
        <artifactId>assertj-core</artifactId>
        <version>3.24.2</version>
        <scope>test</scope>
    </dependency>

</dependencies>

<build>
    <plugins>
        <plugin>
            <groupId>org.apache.maven.plugins</groupId>
            <artifactId>maven-surefire-plugin</artifactId>
            <version>3.1.2</version>
        </plugin>
    </plugins>
</build>
```

### Structure du projet

```
mon-projet/
├── src/
│   ├── main/
│   │   └── java/
│   │       └── com/example/
│   │           ├── Calculator.java          ← Code de production
│   │           └── BankAccount.java
│   └── test/
│       └── java/
│           └── com/example/
│               ├── CalculatorTest.java      ← Tests unitaires
│               └── BankAccountTest.java
└── pom.xml
```

> **Convention de nommage** : La classe de test porte le même nom que la classe testée avec le suffixe `Test`. Ex : `Calculator` → `CalculatorTest`.

---

## 5. JUnit 5 — Fondamentaux

### Architecture de JUnit 5

JUnit 5 est composé de trois modules :

- **JUnit Platform** : moteur d'exécution des tests
- **JUnit Jupiter** : API pour écrire les tests (annotations, assertions)
- **JUnit Vintage** : compatibilité avec JUnit 3 et 4

### Anatomie d'une classe de test

```java
import org.junit.jupiter.api.*;
import static org.junit.jupiter.api.Assertions.*;

class CalculatorTest {

    // ① Déclaration de l'objet à tester (Subject Under Test)
    private Calculator calculator;

    // ② Initialisation avant chaque test
    @BeforeEach
    void setUp() {
        calculator = new Calculator();
    }

    // ③ Un test = une méthode annotée @Test
    @Test
    void shouldAddTwoPositiveNumbers() {
        // GIVEN (Arrange) : préparer les données
        int a = 5;
        int b = 3;

        // WHEN (Act) : appeler la méthode testée
        int result = calculator.add(a, b);

        // THEN (Assert) : vérifier le résultat
        assertEquals(8, result);
    }

    // ④ Nettoyage après chaque test (si nécessaire)
    @AfterEach
    void tearDown() {
        // nettoyage éventuel
    }
}
```

### Le pattern AAA (Arrange — Act — Assert)

Chaque test doit suivre ce pattern pour être lisible et maintenable :

```java
@Test
void shouldReturnZeroWhenMultiplyByZero() {
    // ARRANGE — préparer le contexte
    Calculator calculator = new Calculator();
    int number = 42;
    int zero = 0;

    // ACT — exécuter l'action testée
    int result = calculator.multiply(number, zero);

    // ASSERT — vérifier le résultat attendu
    assertEquals(0, result, "Multiplying any number by 0 should return 0");
}
```

---

## 6. Étape 1 — RED : Écrire un test qui échoue

### Principe

On commence **toujours** par écrire le test **avant** le code. À ce stade, le code de production n'existe pas encore (ou est minimal), donc le test doit échouer.

### Exemple : Feature "addition"

#### 1. Écrire le test (la classe `Calculator` n'existe pas encore)

```java
// src/test/java/com/example/CalculatorTest.java

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

class CalculatorTest {

    @Test
    void shouldReturnSumOfTwoNumbers() {
        // ARRANGE
        Calculator calculator = new Calculator();

        // ACT
        int result = calculator.add(2, 3);

        // ASSERT
        assertEquals(5, result);
    }
}
```

#### 2. Résultat de l'exécution (🔴 RED)

```
[ERROR] COMPILATION ERROR
Cannot find symbol: class Calculator
```

> ⚠️ **Important** : une erreur de compilation n'est pas un "bon" RED. Il faut que le code **compile** mais que l'**assertion échoue**. On doit donc créer la classe minimale.

#### 3. Créer la coquille minimale pour compiler

```java
// src/main/java/com/example/Calculator.java

public class Calculator {
    public int add(int a, int b) {
        return 0; // implémentation intentionnellement fausse
    }
}
```

#### 4. Résultat de l'exécution (🔴 RED attendu)

```
[FAIL] CalculatorTest > shouldReturnSumOfTwoNumbers()
expected: <5> but was: <0>
org.opentest4j.AssertionFailedError: expected: <5> but was: <0>
```

✅ **C'est le comportement souhaité.** Le test échoue pour la bonne raison : l'assertion.

---

## 7. Étape 2 — GREEN : Faire passer le test

### Principe

On écrit le **minimum de code** nécessaire pour faire passer le test. Pas plus, pas moins.

#### Code minimal (parfois naïf)

```java
public class Calculator {
    public int add(int a, int b) {
        return 5; // ← code "stupide" mais le test passe !
    }
}
```

> Oui, retourner `5` en dur fait passer le test `add(2, 3)`. C'est acceptable en TDD car le prochain test forcera une vraie implémentation.

#### Résultat (🟢 GREEN)

```
[PASS] CalculatorTest > shouldReturnSumOfTwoNumbers() ✓
Tests run: 1, Failures: 0, Errors: 0
```

#### Forcer une vraie implémentation avec un second test

```java
@Test
void shouldReturnSumOfDifferentNumbers() {
    Calculator calculator = new Calculator();
    assertEquals(10, calculator.add(7, 3)); // ← 5 en dur ne passe plus !
}
```

Maintenant on est forcé d'écrire la vraie implémentation :

```java
public class Calculator {
    public int add(int a, int b) {
        return a + b; // ← vraie implémentation
    }
}
```

#### Résultat (🟢 GREEN)

```
[PASS] CalculatorTest > shouldReturnSumOfTwoNumbers() ✓
[PASS] CalculatorTest > shouldReturnSumOfDifferentNumbers() ✓
Tests run: 2, Failures: 0, Errors: 0
```

---

## 8. Étape 3 — REFACTOR : Améliorer le code

### Principe

Une fois le test au vert, on améliore le code **sans changer son comportement**. Les tests doivent **continuer à passer** après chaque modification.

### Quoi refactorer ?

- Supprimer la **duplication** (DRY — Don't Repeat Yourself)
- Améliorer les **noms** de variables, méthodes, classes
- Extraire des **méthodes privées** pour la lisibilité
- Simplifier la **logique conditionnelle**
- Appliquer les **design patterns** appropriés

### Exemple de refactoring

**Avant refactoring** (code qui fonctionne mais est mal structuré) :

```java
public class BankAccount {
    private double b;  // ← nom peu expressif

    public void d(double a) {  // ← nom incompréhensible
        if (a > 0) {
            b = b + a;
        }
    }

    public double getB() {  // ← nom peu expressif
        return b;
    }
}
```

**Après refactoring** (même comportement, meilleure lisibilité) :

```java
public class BankAccount {
    private double balance;

    public void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
        }
    }

    public double getBalance() {
        return balance;
    }
}
```

Les tests passent toujours, mais le code est **bien meilleur**.

---

## 9. Cas pratique complet : Calculatrice bancaire

Développons une `BankAccount` en TDD pur, étape par étape.

### Spécifications

- Un compte a un solde (`balance`) initialement à 0
- On peut déposer (`deposit`) un montant positif
- On peut retirer (`withdraw`) un montant si le solde est suffisant
- On ne peut pas retirer plus que le solde disponible
- On ne peut pas déposer un montant négatif ou nul

---

### Itération 1 — Dépôt simple

**🔴 RED**

```java
class BankAccountTest {

    @Test
    void initialBalanceShouldBeZero() {
        BankAccount account = new BankAccount();
        assertEquals(0.0, account.getBalance());
    }
}
```

**🟢 GREEN**

```java
public class BankAccount {
    private double balance = 0.0;

    public double getBalance() {
        return balance;
    }
}
```

---

### Itération 2 — Dépôt d'un montant

**🔴 RED**

```java
@Test
void shouldIncreaseBalanceAfterDeposit() {
    BankAccount account = new BankAccount();
    account.deposit(100.0);
    assertEquals(100.0, account.getBalance());
}
```

**🟢 GREEN**

```java
public void deposit(double amount) {
    balance += amount;
}
```

---

### Itération 3 — Dépôt d'un montant négatif (cas d'erreur)

**🔴 RED**

```java
@Test
void shouldThrowExceptionWhenDepositingNegativeAmount() {
    BankAccount account = new BankAccount();

    assertThrows(IllegalArgumentException.class, () -> {
        account.deposit(-50.0);
    });
}
```

**🟢 GREEN**

```java
public void deposit(double amount) {
    if (amount <= 0) {
        throw new IllegalArgumentException("Deposit amount must be positive");
    }
    balance += amount;
}
```

---

### Itération 4 — Retrait

**🔴 RED**

```java
@Test
void shouldDecreaseBalanceAfterWithdraw() {
    BankAccount account = new BankAccount();
    account.deposit(200.0);

    account.withdraw(50.0);

    assertEquals(150.0, account.getBalance());
}
```

**🟢 GREEN**

```java
public void withdraw(double amount) {
    balance -= amount;
}
```

---

### Itération 5 — Retrait insuffisant (cas d'erreur)

**🔴 RED**

```java
@Test
void shouldThrowExceptionWhenWithdrawingMoreThanBalance() {
    BankAccount account = new BankAccount();
    account.deposit(100.0);

    assertThrows(IllegalStateException.class, () -> {
        account.withdraw(200.0);
    });
}
```

**🟢 GREEN**

```java
public void withdraw(double amount) {
    if (amount > balance) {
        throw new IllegalStateException("Insufficient funds");
    }
    balance -= amount;
}
```

---

### 🔵 REFACTOR final — BankAccount complète et propre

```java
// src/main/java/com/example/BankAccount.java

public class BankAccount {

    private double balance;

    public BankAccount() {
        this.balance = 0.0;
    }

    /**
     * Dépose un montant sur le compte.
     * @throws IllegalArgumentException si le montant est négatif ou nul
     */
    public void deposit(double amount) {
        validatePositiveAmount(amount, "Deposit amount must be positive");
        balance += amount;
    }

    /**
     * Retire un montant du compte.
     * @throws IllegalArgumentException si le montant est négatif ou nul
     * @throws IllegalStateException si le solde est insuffisant
     */
    public void withdraw(double amount) {
        validatePositiveAmount(amount, "Withdrawal amount must be positive");
        if (amount > balance) {
            throw new IllegalStateException("Insufficient funds: balance is " + balance);
        }
        balance -= amount;
    }

    public double getBalance() {
        return balance;
    }

    private void validatePositiveAmount(double amount, String errorMessage) {
        if (amount <= 0) {
            throw new IllegalArgumentException(errorMessage);
        }
    }
}
```

### Suite de tests complète

```java
// src/test/java/com/example/BankAccountTest.java

import org.junit.jupiter.api.*;
import static org.junit.jupiter.api.Assertions.*;

class BankAccountTest {

    private BankAccount account;

    @BeforeEach
    void setUp() {
        account = new BankAccount();
    }

    // ─── Tests de l'état initial ───────────────────────────────────────

    @Test
    @DisplayName("Initial balance should be zero")
    void initialBalanceShouldBeZero() {
        assertEquals(0.0, account.getBalance());
    }

    // ─── Tests de deposit ──────────────────────────────────────────────

    @Test
    @DisplayName("Deposit should increase balance")
    void depositShouldIncreaseBalance() {
        account.deposit(100.0);
        assertEquals(100.0, account.getBalance());
    }

    @Test
    @DisplayName("Multiple deposits should accumulate")
    void multipleDepositsShouldAccumulate() {
        account.deposit(100.0);
        account.deposit(50.0);
        assertEquals(150.0, account.getBalance());
    }

    @Test
    @DisplayName("Deposit negative amount should throw IllegalArgumentException")
    void depositNegativeAmountShouldThrow() {
        assertThrows(IllegalArgumentException.class,
            () -> account.deposit(-10.0));
    }

    @Test
    @DisplayName("Deposit zero should throw IllegalArgumentException")
    void depositZeroShouldThrow() {
        assertThrows(IllegalArgumentException.class,
            () -> account.deposit(0.0));
    }

    // ─── Tests de withdraw ─────────────────────────────────────────────

    @Test
    @DisplayName("Withdraw should decrease balance")
    void withdrawShouldDecreaseBalance() {
        account.deposit(200.0);
        account.withdraw(50.0);
        assertEquals(150.0, account.getBalance());
    }

    @Test
    @DisplayName("Withdraw exact balance should set balance to zero")
    void withdrawExactBalanceShouldSetToZero() {
        account.deposit(100.0);
        account.withdraw(100.0);
        assertEquals(0.0, account.getBalance());
    }

    @Test
    @DisplayName("Withdraw more than balance should throw IllegalStateException")
    void withdrawMoreThanBalanceShouldThrow() {
        account.deposit(100.0);
        assertThrows(IllegalStateException.class,
            () -> account.withdraw(200.0));
    }

    @Test
    @DisplayName("Withdraw negative amount should throw IllegalArgumentException")
    void withdrawNegativeAmountShouldThrow() {
        assertThrows(IllegalArgumentException.class,
            () -> account.withdraw(-10.0));
    }
}
```

---

## 10. Les Assertions avancées

### JUnit 5 Assertions

```java
import static org.junit.jupiter.api.Assertions.*;

// Égalité simple
assertEquals(expected, actual);
assertEquals(expected, actual, "message si le test échoue");

// Égalité avec delta pour les doubles
assertEquals(0.1 + 0.2, 0.3, 0.0001);

// Valeurs booléennes
assertTrue(result > 0);
assertFalse(list.isEmpty());

// Valeurs nulles
assertNull(value);
assertNotNull(value);

// Exceptions
assertThrows(IllegalArgumentException.class, () -> {
    account.deposit(-1);
});

// Vérifier le message de l'exception
Exception ex = assertThrows(IllegalArgumentException.class, () -> {
    account.deposit(-1);
});
assertEquals("Deposit amount must be positive", ex.getMessage());

// Assertions multiples groupées (toutes évaluées même si l'une échoue)
assertAll("account state",
    () -> assertEquals(100.0, account.getBalance()),
    () -> assertNotNull(account),
    () -> assertTrue(account.getBalance() > 0)
);

// Timeout
assertTimeout(Duration.ofMillis(100), () -> {
    account.deposit(10);
});
```

### AssertJ — Assertions fluides (recommandé)

AssertJ propose une API plus expressive et lisible :

```java
import static org.assertj.core.api.Assertions.*;

// Égalité
assertThat(result).isEqualTo(5);

// Collections
assertThat(list)
    .hasSize(3)
    .contains("Alice", "Bob")
    .doesNotContain("Charlie");

// Strings
assertThat(message)
    .isNotEmpty()
    .startsWith("Hello")
    .endsWith("World")
    .contains("lo W");

// Exceptions avec AssertJ
assertThatThrownBy(() -> account.deposit(-1))
    .isInstanceOf(IllegalArgumentException.class)
    .hasMessage("Deposit amount must be positive");

// Nombres
assertThat(balance)
    .isPositive()
    .isGreaterThan(0.0)
    .isLessThanOrEqualTo(1000.0)
    .isCloseTo(99.99, within(0.01));

// Objects
assertThat(account)
    .isNotNull()
    .extracting(BankAccount::getBalance)
    .isEqualTo(0.0);
```

---

## 11. Les Annotations JUnit 5 essentielles

### Cycle de vie des tests

```java
class LifecycleTest {

    @BeforeAll
    static void initOnce() {
        // Exécuté UNE SEULE FOIS avant tous les tests de la classe
        // Doit être static (sauf avec @TestInstance(Lifecycle.PER_CLASS))
        System.out.println("Setup global");
    }

    @BeforeEach
    void init() {
        // Exécuté avant CHAQUE méthode @Test
        // Idéal pour réinitialiser l'état
    }

    @Test
    void testOne() { /* ... */ }

    @Test
    void testTwo() { /* ... */ }

    @AfterEach
    void cleanup() {
        // Exécuté après CHAQUE méthode @Test
    }

    @AfterAll
    static void cleanupOnce() {
        // Exécuté UNE SEULE FOIS après tous les tests
    }
}
```

### Annotations utiles

```java
// Ignorer un test temporairement
@Test
@Disabled("En attente de la fonctionnalité JIRA-123")
void skippedTest() { }

// Nommer lisiblement les tests
@Test
@DisplayName("Should throw exception when amount is negative")
void test1() { }

// Tests paramétrés — plusieurs jeux de données en un seul test
@ParameterizedTest
@ValueSource(ints = {-1, 0, -100, Integer.MIN_VALUE})
void shouldRejectInvalidAmounts(int amount) {
    assertThrows(IllegalArgumentException.class,
        () -> account.deposit(amount));
}

// Tests paramétrés avec plusieurs paramètres
@ParameterizedTest
@CsvSource({
    "2, 3, 5",
    "10, 20, 30",
    "0, 0, 0",
    "-5, 5, 0"
})
void shouldAddNumbers(int a, int b, int expected) {
    assertEquals(expected, calculator.add(a, b));
}

// Tagger des tests pour les exécuter sélectivement
@Test
@Tag("fast")
void fastTest() { }

@Test
@Tag("integration")
void integrationTest() { }

// Ordre d'exécution des tests
@TestMethodOrder(MethodOrderer.OrderAnnotation.class)
class OrderedTest {
    @Test @Order(1) void first() { }
    @Test @Order(2) void second() { }
    @Test @Order(3) void third() { }
}

// Répéter un test plusieurs fois (utile pour les tests de performance)
@RepeatedTest(5)
void shouldBeConsistent() {
    assertEquals(5, calculator.add(2, 3));
}
```

### Tests paramétrés avancés avec `@MethodSource`

```java
import java.util.stream.Stream;
import org.junit.jupiter.params.provider.*;

class BankAccountParameterizedTest {

    static Stream<Arguments> depositScenarios() {
        return Stream.of(
            Arguments.of(100.0, 100.0),
            Arguments.of(500.0, 500.0),
            Arguments.of(0.01, 0.01)
        );
    }

    @ParameterizedTest(name = "Deposit {0} should result in balance of {1}")
    @MethodSource("depositScenarios")
    void shouldHandleDepositScenarios(double amount, double expectedBalance) {
        BankAccount account = new BankAccount();
        account.deposit(amount);
        assertEquals(expectedBalance, account.getBalance());
    }
}
```

---

## 12. Mocking avec Mockito

### Pourquoi les Mocks ?

Dans le TDD, on teste une **unité isolée**. Mais les classes ont souvent des **dépendances** (services, repositories, APIs). Les **mocks** permettent de simuler ces dépendances pour :

- Contrôler leur comportement dans les tests
- Éviter les appels réels à la base de données ou au réseau
- Tester des cas difficiles à reproduire (erreurs réseau, timeouts)

### Setup avec Mockito

```java
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.Mock;
import org.mockito.InjectMocks;
import org.mockito.junit.jupiter.MockitoExtension;
import static org.mockito.Mockito.*;

@ExtendWith(MockitoExtension.class)
class UserServiceTest {

    @Mock
    private UserRepository userRepository;  // dépendance simulée

    @InjectMocks
    private UserService userService;        // classe testée avec mock injecté

    @Test
    void shouldReturnUserById() {
        // ARRANGE — configurer le comportement du mock
        User mockUser = new User(1L, "Alice", "alice@example.com");
        when(userRepository.findById(1L)).thenReturn(Optional.of(mockUser));

        // ACT
        User result = userService.getUserById(1L);

        // ASSERT
        assertEquals("Alice", result.getName());
        assertEquals("alice@example.com", result.getEmail());

        // Vérifier que la méthode a bien été appelée
        verify(userRepository, times(1)).findById(1L);
    }

    @Test
    void shouldThrowExceptionWhenUserNotFound() {
        // ARRANGE — le repository retourne vide
        when(userRepository.findById(99L)).thenReturn(Optional.empty());

        // ASSERT + ACT
        assertThrows(UserNotFoundException.class,
            () -> userService.getUserById(99L));
    }
}
```

### Techniques Mockito avancées

```java
// Simuler une exception
when(userRepository.findById(anyLong()))
    .thenThrow(new DataAccessException("DB error"));

// Simuler plusieurs appels successifs
when(service.getStatus())
    .thenReturn("PENDING")
    .thenReturn("PROCESSING")
    .thenReturn("DONE");

// Capturer les arguments passés au mock
ArgumentCaptor<User> captor = ArgumentCaptor.forClass(User.class);
verify(userRepository).save(captor.capture());
assertEquals("Alice", captor.getValue().getName());

// Vérifier qu'une méthode n'a JAMAIS été appelée
verify(emailService, never()).sendEmail(any());

// Mock d'une méthode void
doNothing().when(emailService).sendNotification(any());
doThrow(new RuntimeException()).when(emailService).sendNotification(any());

// Spy — envelopper un vrai objet pour espionner ses appels
BankAccount realAccount = new BankAccount();
BankAccount spyAccount = spy(realAccount);

spyAccount.deposit(100.0);
verify(spyAccount).deposit(100.0);  // vérifie l'appel
assertEquals(100.0, spyAccount.getBalance());  // comportement réel
```

### Exemple complet : Service avec dépendances

```java
// ─── Entités et interfaces ────────────────────────────────────────────

public class Order {
    private Long id;
    private double total;
    private String status;
    // getters / setters ...
}

public interface OrderRepository {
    Optional<Order> findById(Long id);
    Order save(Order order);
}

public interface PaymentGateway {
    boolean processPayment(Long orderId, double amount);
}

// ─── Service à tester ─────────────────────────────────────────────────

public class OrderService {
    private final OrderRepository orderRepository;
    private final PaymentGateway paymentGateway;

    public OrderService(OrderRepository orderRepository,
                        PaymentGateway paymentGateway) {
        this.orderRepository = orderRepository;
        this.paymentGateway = paymentGateway;
    }

    public Order processOrder(Long orderId) {
        Order order = orderRepository.findById(orderId)
            .orElseThrow(() -> new OrderNotFoundException(orderId));

        boolean paid = paymentGateway.processPayment(orderId, order.getTotal());

        if (!paid) {
            throw new PaymentFailedException("Payment failed for order " + orderId);
        }

        order.setStatus("PAID");
        return orderRepository.save(order);
    }
}

// ─── Tests TDD avec Mocks ─────────────────────────────────────────────

@ExtendWith(MockitoExtension.class)
class OrderServiceTest {

    @Mock
    private OrderRepository orderRepository;

    @Mock
    private PaymentGateway paymentGateway;

    @InjectMocks
    private OrderService orderService;

    @Test
    @DisplayName("Should process order and set status to PAID")
    void shouldProcessOrderSuccessfully() {
        // ARRANGE
        Order order = new Order();
        order.setId(1L);
        order.setTotal(99.99);
        order.setStatus("PENDING");

        when(orderRepository.findById(1L)).thenReturn(Optional.of(order));
        when(paymentGateway.processPayment(1L, 99.99)).thenReturn(true);
        when(orderRepository.save(any(Order.class))).thenReturn(order);

        // ACT
        Order result = orderService.processOrder(1L);

        // ASSERT
        assertEquals("PAID", result.getStatus());
        verify(paymentGateway).processPayment(1L, 99.99);
        verify(orderRepository).save(order);
    }

    @Test
    @DisplayName("Should throw OrderNotFoundException when order does not exist")
    void shouldThrowWhenOrderNotFound() {
        // ARRANGE
        when(orderRepository.findById(99L)).thenReturn(Optional.empty());

        // ASSERT + ACT
        assertThrows(OrderNotFoundException.class,
            () -> orderService.processOrder(99L));

        // Vérifier que le payment n'a PAS été tenté
        verify(paymentGateway, never()).processPayment(anyLong(), anyDouble());
    }

    @Test
    @DisplayName("Should throw PaymentFailedException when payment is declined")
    void shouldThrowWhenPaymentFails() {
        // ARRANGE
        Order order = new Order();
        order.setId(1L);
        order.setTotal(500.0);

        when(orderRepository.findById(1L)).thenReturn(Optional.of(order));
        when(paymentGateway.processPayment(1L, 500.0)).thenReturn(false);

        // ASSERT + ACT
        assertThrows(PaymentFailedException.class,
            () -> orderService.processOrder(1L));

        // L'ordre ne doit pas être sauvegardé si le paiement échoue
        verify(orderRepository, never()).save(any());
    }
}
```

---

## 13. TDD avec des services Spring Boot

### Configuration de test Spring Boot

```xml
<!-- pom.xml — dépendance Spring Boot Test -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-test</artifactId>
    <scope>test</scope>
    <!-- Inclut JUnit 5, Mockito, AssertJ, et Spring Test -->
</dependency>
```

### Test de la couche Service (sans contexte Spring)

Avec `@ExtendWith(MockitoExtension.class)`, on teste le service **en pur unitaire**, sans démarrer le contexte Spring :

```java
@ExtendWith(MockitoExtension.class)
class ProductServiceTest {

    @Mock
    private ProductRepository productRepository;

    @InjectMocks
    private ProductService productService;

    @Test
    void shouldReturnAllProducts() {
        // ARRANGE
        List<Product> products = List.of(
            new Product(1L, "Laptop", 999.99),
            new Product(2L, "Mouse", 29.99)
        );
        when(productRepository.findAll()).thenReturn(products);

        // ACT
        List<Product> result = productService.getAllProducts();

        // ASSERT
        assertThat(result).hasSize(2);
        assertThat(result).extracting(Product::getName)
                          .containsExactlyInAnyOrder("Laptop", "Mouse");
    }

    @Test
    void shouldCreateProduct() {
        // ARRANGE
        Product newProduct = new Product(null, "Keyboard", 79.99);
        Product savedProduct = new Product(1L, "Keyboard", 79.99);
        when(productRepository.save(newProduct)).thenReturn(savedProduct);

        // ACT
        Product result = productService.createProduct(newProduct);

        // ASSERT
        assertThat(result.getId()).isEqualTo(1L);
        assertThat(result.getName()).isEqualTo("Keyboard");
        verify(productRepository).save(newProduct);
    }
}
```

### Test de la couche Repository avec `@DataJpaTest`

```java
@DataJpaTest
// Lance uniquement la couche JPA (H2 en mémoire par défaut)
class ProductRepositoryTest {

    @Autowired
    private ProductRepository productRepository;

    @Test
    void shouldFindProductByName() {
        // ARRANGE
        Product product = new Product(null, "Keyboard", 79.99);
        productRepository.save(product);

        // ACT
        Optional<Product> result = productRepository.findByName("Keyboard");

        // ASSERT
        assertThat(result).isPresent();
        assertThat(result.get().getPrice()).isEqualTo(79.99);
    }

    @Test
    void shouldReturnEmptyWhenProductNotFound() {
        Optional<Product> result = productRepository.findByName("Unknown");
        assertThat(result).isEmpty();
    }
}
```

### Test du Controller avec `@WebMvcTest`

```java
@WebMvcTest(ProductController.class)
// Lance uniquement la couche Web (sans base de données)
class ProductControllerTest {

    @Autowired
    private MockMvc mockMvc;

    @MockBean
    private ProductService productService;

    @Autowired
    private ObjectMapper objectMapper;

    @Test
    void shouldReturnAllProducts() throws Exception {
        // ARRANGE
        List<Product> products = List.of(
            new Product(1L, "Laptop", 999.99)
        );
        when(productService.getAllProducts()).thenReturn(products);

        // ACT + ASSERT
        mockMvc.perform(get("/api/products")
                .contentType(MediaType.APPLICATION_JSON))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.length()").value(1))
            .andExpect(jsonPath("$[0].name").value("Laptop"))
            .andExpect(jsonPath("$[0].price").value(999.99));
    }

    @Test
    void shouldCreateProductAndReturn201() throws Exception {
        // ARRANGE
        Product newProduct = new Product(null, "Mouse", 29.99);
        Product savedProduct = new Product(1L, "Mouse", 29.99);
        when(productService.createProduct(any())).thenReturn(savedProduct);

        // ACT + ASSERT
        mockMvc.perform(post("/api/products")
                .contentType(MediaType.APPLICATION_JSON)
                .content(objectMapper.writeValueAsString(newProduct)))
            .andExpect(status().isCreated())
            .andExpect(jsonPath("$.id").value(1))
            .andExpect(jsonPath("$.name").value("Mouse"));
    }

    @Test
    void shouldReturn404WhenProductNotFound() throws Exception {
        when(productService.getProductById(99L))
            .thenThrow(new ProductNotFoundException(99L));

        mockMvc.perform(get("/api/products/99"))
            .andExpect(status().isNotFound());
    }
}
```

---

## 14. Bonnes pratiques TDD

### 1. Un test = une seule assertion logique

```java
// ❌ Mauvais : plusieurs comportements testés en même temps
@Test
void testBankAccount() {
    BankAccount account = new BankAccount();
    account.deposit(100);
    assertEquals(100, account.getBalance());
    account.withdraw(30);
    assertEquals(70, account.getBalance());
    assertThrows(IllegalStateException.class, () -> account.withdraw(200));
}

// ✅ Bon : un comportement par test
@Test
void depositShouldIncreaseBalance() { /* ... */ }

@Test
void withdrawShouldDecreaseBalance() { /* ... */ }

@Test
void withdrawExceedingBalanceShouldThrow() { /* ... */ }
```

### 2. Nommer les tests de façon expressive

```java
// ❌ Noms inutiles
@Test void test1() { }
@Test void testDeposit() { }

// ✅ Noms décrivant le comportement attendu
// Pattern recommandé : should[Behavior]When[Condition]
@Test void shouldIncreaseBalanceWhenDepositIsPositive() { }
@Test void shouldThrowExceptionWhenDepositAmountIsNegative() { }
@Test void shouldReturnZeroWhenAccountIsNew() { }
```

### 3. Isoler les tests avec `@BeforeEach`

```java
class BankAccountTest {

    private BankAccount account;  // ← état partagé

    @BeforeEach
    void setUp() {
        account = new BankAccount();  // ← réinitialisation pour chaque test
    }

    // Chaque test commence avec un compte vierge
    @Test void shouldHaveZeroInitialBalance() { /* ... */ }
    @Test void shouldDepositCorrectly() { /* ... */ }
}
```

### 4. Respecter la règle du bébé pas (Baby Steps)

En TDD, on avance **petit à petit** :

```
Feature complexe → décomposer en comportements simples → tester un à la fois

Exemple : "Gestion d'un panier e-commerce"
  ✅ Étape 1 : Le panier est vide à la création
  ✅ Étape 2 : Ajouter un produit au panier
  ✅ Étape 3 : Le total est correct après ajout
  ✅ Étape 4 : Ajouter deux fois le même produit augmente la quantité
  ✅ Étape 5 : Supprimer un produit
  ✅ Étape 6 : Vider le panier
  ✅ Étape 7 : Appliquer un code promo
  ...
```

### 5. Le test doit toujours échouer d'abord

Avant de passer à l'implémentation, **vérifier que le test échoue**. Sinon, le test ne teste rien !

```java
// Ce test "passe" toujours, même sans implémentation → test inutile ❌
@Test
void alwaysPasses() {
    assertTrue(true);
}

// Ce test doit VRAIMENT tester quelque chose ✅
@Test
void shouldCalculateDiscountedPrice() {
    double price = priceCalculator.applyDiscount(100.0, 10); // 10% de remise
    assertEquals(90.0, price);
}
```

### 6. Règle FIRST pour les tests de qualité

| Lettre | Signification | Description |
|--------|---------------|-------------|
| **F** | Fast | Les tests unitaires s'exécutent en millisecondes |
| **I** | Independent | Chaque test est indépendant des autres |
| **R** | Repeatable | Le même résultat à chaque exécution |
| **S** | Self-Validating | Les tests se valident eux-mêmes (pas de vérification manuelle) |
| **T** | Timely | Écrits avant le code de production |

---

## 15. Anti-patterns à éviter

### ❌ Test fragile (Fragile Test)

```java
// Dépend de l'ordre d'exécution — INTERDIT
@Test
void test2_shouldWithdraw() {
    // Suppose que test1 a déjà fait un dépôt...
    account.withdraw(50);  // ← PROBLÈME : account non initialisé ici
    assertEquals(50, account.getBalance());
}
```

### ❌ Test qui ne teste rien

```java
@Test
void emptyTest() {
    // Aucune assertion — passe toujours, ne sert à rien
    Calculator calculator = new Calculator();
    calculator.add(1, 2);
}
```

### ❌ Trop de mocks

```java
// Trop de mocks = test qui teste les mocks, pas le vrai comportement
@Test
void overMockedTest() {
    when(mock1.doA()).thenReturn(mock2);
    when(mock2.doB()).thenReturn(mock3);
    when(mock3.doC()).thenReturn("result");
    // ...Ce test ne valide rien de réel
}
```

### ❌ Logique dans les tests

```java
// ❌ Les tests ne doivent pas avoir de logique conditionnelle
@Test
void testWithLogic() {
    int result = calculator.add(2, 3);
    if (result == 5) {
        assertTrue(true);  // Logique inutile
    }
}

// ✅ Direct et simple
@Test
void shouldAddTwoNumbers() {
    assertEquals(5, calculator.add(2, 3));
}
```

### ❌ Ignorer des tests avec `@Disabled` de façon permanente

```java
// ❌ Test désactivé sans raison et sans ticket de suivi
@Test
@Disabled
void brokenTest() {
    // TODO: fix someday
}

// ✅ Désactivé temporairement avec justification
@Test
@Disabled("Désactivé en attendant le fix JIRA-456 (sprint 12)")
void temporarilyDisabledTest() {
    // ...
}
```

---

## 16. Résumé visuel du cycle TDD

```
┌────────────────────────────────────────────────────────────────────┐
│                      LE CYCLE TDD                                  │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│   1. ÉCRIRE UN TEST        2. VOIR LE TEST ÉCHOUER                │
│   ──────────────────       ──────────────────────────             │
│   • Identifier le          • S'assurer que le test                │
│     comportement à           compile                              │
│     tester                 • Vérifier que le test                 │
│   • Écrire le test           échoue pour la BONNE                 │
│     AVANT le code            raison (🔴 RED)                      │
│                                                                    │
│   3. ÉCRIRE LE CODE        4. VOIR LE TEST PASSER                 │
│   ─────────────────        ──────────────────────                 │
│   • Minimum de code        • Tous les tests au                    │
│     pour faire passer        vert (🟢 GREEN)                      │
│     le test                • Ne pas optimiser                     │
│   • Pas d'optimisation       encore                               │
│     prématurée                                                     │
│                                                                    │
│   5. REFACTORER            6. VOIR LES TESTS PASSER               │
│   ─────────────            ──────────────────────────             │
│   • Améliorer la           • Tous les tests doivent               │
│     structure                rester au vert (🔵 REFACTOR)         │
│   • Supprimer la           • Revenir en arrière si                │
│     duplication              un test casse                        │
│   • Améliorer les noms                                            │
│                                                                    │
│                    ↕ Recommencer !                                 │
└────────────────────────────────────────────────────────────────────┘
```

### Checklist TDD

Avant chaque commit, vérifier :

- [ ] Le test a été écrit **avant** le code
- [ ] Le test a **échoué** avant l'implémentation
- [ ] Le code minimal a été écrit pour faire passer le test
- [ ] Le refactoring a été effectué
- [ ] **Tous** les tests passent après le refactoring
- [ ] Les tests sont lisibles et bien nommés
- [ ] Pas de duplication dans le code de production

---

## Références et ressources

| Ressource | Description |
|-----------|-------------|
| **Kent Beck — Test-Driven Development by Example** | Le livre fondateur du TDD |
| **Robert C. Martin — Clean Code** | Bonnes pratiques de code propre |
| [junit.org/junit5](https://junit.org/junit5/) | Documentation officielle JUnit 5 |
| [site.mockito.org](https://site.mockito.org/) | Documentation officielle Mockito |
| [assertj.github.io](https://assertj.github.io/doc/) | Documentation officielle AssertJ |

---

*Cours rédigé avec les termes techniques en anglais, conformément aux conventions de l'industrie Java.*
