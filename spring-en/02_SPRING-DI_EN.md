## [**..**](./00_index.md)

# Java Spring Framework Core Concepts

## 🔗 Loose Coupling vs Tight Coupling

### Tight Coupling - ❌ Avoid This
```java
// Bad example - direct dependency
public class GameRunner {
    private MarioGame game = new MarioGame(); // Coupled to MarioGame
    
    public void run() {
        game.up();
        game.down();
    }
}
```
**Problems :**
- Can't change the game without modifying the code
- Hard to test (can't mock MarioGame)
- Rigid and non-reusable code

### Loose Coupling - ✅ Recommended
```java
// Good example - dependency via interface
public interface Game {
    void up();
    void down();
}

public class GameRunner {
    private Game game; // Depends on the interface, not the implementation
    
    public GameRunner(Game game) {
        this.game = game;
    }
    
    public void run() {
        game.up();
        game.down();
    }
}
```
**Advantages :**
- Easy to switch implementations (MarioGame, PacmanGame...)
- Testable (inject mocks)
- Flexible and maintainable code

**Spring achieves Loose Coupling through :**
- **Dependency Injection (DI)**
- **Inversion of Control (IoC)**

## 🏭 Spring IoC Container - Inversion of Control

### Main Concept
**IoC** means that Spring manages the creation and lifecycle of objects (beans) instead of the developer manually instantiating them with `new`.

**Without Spring :**
```java
MarioGame game = new MarioGame();
GameRunner runner = new GameRunner(game);
```

**With Spring IoC :**
```java
ApplicationContext context = new AnnotationConfigApplicationContext(AppConfig.class);
GameRunner runner = context.getBean(GameRunner.class);
// Spring automatically created and injected the dependencies
```

### Two IoC Container Implementations

#### 1. **BeanFactory** (Base interface)
- Simplest IoC container
- Lazy initialization (beans created on demand)
- Fewer features
- Rarely used directly

#### 2. **ApplicationContext** (Recommended) ⭐
- Extends BeanFactory
- Eager initialization (beans created at startup)
- Advanced features :
  - Internationalization (i18n)
  - Event publishing
  - Spring AOP integration
  - Web application context

**ApplicationContext types :**
```java
// Annotation-based configuration (modern)
ApplicationContext context = new AnnotationConfigApplicationContext(AppConfig.class);

// XML configuration (old way)
ApplicationContext context = new ClassPathXmlApplicationContext("beans.xml");

// For web applications
ApplicationContext context = new WebApplicationContext();
```

**Getting beans :**
```java
// By type
GameRunner runner = context.getBean(GameRunner.class);

// By name
GameRunner runner = (GameRunner) context.getBean("gameRunner");

// By name and type
GameRunner runner = context.getBean("gameRunner", GameRunner.class);
```

## 🫘 Java Bean vs POJO vs Spring Bean

### POJO (Plain Old Java Object)
```java
public class Person {
    private String name;
    private int age;
    
    // Constructors, getters, setters
}
```
- Simple Java class with no constraints
- No specific restrictions
- Doesn't extend any specific class
- Doesn't implement any required interface

### Java Bean (Strict conventions)
```java
public class Person implements Serializable {
    private String name;
    private int age;
    
    // Required no-argument constructor
    public Person() {}
    
    // Required getters/setters
    public String getName() { return name; }
    public void setName(String name) { this.name = name; }
    public int getAge() { return age; }
    public void setAge(int age) { this.age = age; }
}
```
**Strict rules :**
- ✅ Must be `Serializable`
- ✅ No-argument constructor
- ✅ Private properties with getters/setters
- ✅ Strict naming conventions

### Spring Bean
```java
@Component
public class GameRunner {
    // ...
}
```
- **Definition** : Any object managed by the Spring IoC Container
- Created, configured, and managed by Spring
- Can be a simple POJO
- Doesn't need to follow Java Bean conventions

**Lifecycle managed by Spring :**
1. Instantiation
2. Dependency injection
3. Initialization (`@PostConstruct`)
4. In use
5. Destruction (`@PreDestroy`)

## 📦 Spring Framework Modules

Spring Framework is modular and contains several modules :

- **Spring Core** : IoC Container, Dependency Injection
- **Spring MVC** : Web applications (REST APIs, Controllers)
- **Spring JDBC** : Database access
- **Spring ORM** : JPA, Hibernate integration
- **Spring AOP** : Aspect-Oriented Programming
- **Spring Test** : Unit and integration testing
- **Spring Security** : Authentication and authorization
- **Spring Data** : Repositories and simplified data access

## 💉 Dependency Injection - The 3 Approaches

### 1. Constructor-based Injection (✅ Recommended)
```java
@Component
public class GameRunner {
    private final Game game;
    
    @Autowired // Optional if only one constructor
    public GameRunner(Game game) {
        this.game = game;
    }
}
```
**Advantages :**
- Mandatory and immutable dependencies (`final`)
- Easy to test
- Thread-safe
- **Spring recommends this approach**

### 2. Setter-based Injection
```java
@Component
public class GameRunner {
    private Game game;
    
    @Autowired
    public void setGame(Game game) {
        this.game = game;
    }
}
```
**Usage :**
- Optional dependencies
- Allows changing the dependency after creation
- Less safe than Constructor injection

### 3. Field Injection (⚠️ Not recommended)
```java
@Component
public class GameRunner {
    @Autowired
    private Game game;
}
```
**Disadvantages :**
- Can't use `final` fields
- Hard to test (requires reflection)
- Hides dependencies
- **Avoid in production**

## 🎯 Main Annotations

### @Configuration
```java
@Configuration
public class AppConfig {
    @Bean
    public Game game() {
        return new MarioGame();
    }
    
    @Bean
    public GameRunner gameRunner(Game game) {
        return new GameRunner(game);
    }
}
```
- Marks a Spring configuration class
- Contains `@Bean` methods to define beans
- Processed by the Spring Container at startup
- Replaces XML configuration

### @Bean
```java
@Bean
public DataSource dataSource() {
    return new HikariDataSource();
}
```
- Declares that a method produces a bean managed by Spring
- Used in `@Configuration` classes
- Allows fine control over bean creation
- Bean name = method name (by default)

### @Component
```java
@Component
public class MarioGame implements Game {
    // ...
}
```
- Marks a class as a Spring component
- Automatically detected by `@ComponentScan`
- Spring automatically creates a bean
- Specialized annotations : `@Service`, `@Repository`, `@Controller`

### @ComponentScan
```java
@Configuration
@ComponentScan("com.example.game")
public class AppConfig {
    // Spring scans the package to find @Component classes
}
```
- Enables automatic component scanning
- Looks for `@Component`, `@Service`, `@Repository`, `@Controller`
- Automatically registers found beans
- Recursively scans sub-packages

### @Autowired
```java
@Component
public class GameRunner {
    @Autowired
    private Game game; // Spring automatically injects
}
```
- Automatic dependency injection by Spring
- Can be used on : constructors, setters, fields
- Looks for a compatible bean by type
- Optional on constructors (since Spring 4.3)

### @Primary
```java
@Component
@Primary
public class MarioGame implements Game {
    // If several Game implementations exist,
    // MarioGame will be injected by default
}
```
- Gives priority to a bean when multiple candidates exist
- Resolves autowiring ambiguities
- Used when there are multiple beans of the same type
- Only one `@Primary` per type

### @Qualifier
```java
@Component
@Qualifier("mario")
public class MarioGame implements Game { }

@Component
@Qualifier("pacman")
public class PacmanGame implements Game { }

// Injection with qualifier
@Component
public class GameRunner {
    @Autowired
    @Qualifier("mario")
    private Game game; // Forces injection of MarioGame
}
```
- Specifies which bean to inject when multiple candidates exist
- More precise than `@Primary`
- Used with `@Autowired` to remove ambiguity
- Can be used on fields, parameters, methods

**Difference @Primary vs @Qualifier :**
- **@Primary** : Global default choice
- **@Qualifier** : Explicit choice at the injection point

### @Lazy
```java
@Component
@Lazy
public class HeavyService {
    public HeavyService() {
        System.out.println("Heavy initialization...");
    }
}
```
- Deferred initialization of the bean
- Bean created only on first use
- By default : beans created at startup (eager)
- Useful for heavy beans that are rarely used

### @Scope
```java
@Component
@Scope(ConfigurableBeanFactory.SCOPE_PROTOTYPE)
public class PrototypeBean {
    // New instance on each injection
}

@Component
@Scope(ConfigurableBeanFactory.SCOPE_SINGLETON) // Default
public class SingletonBean {
    // Single instance in the container
}
```

**Available scopes :**
- **SINGLETON** (default) : One instance per IoC Container
- **PROTOTYPE** : New instance on each request
- **REQUEST** : One instance per HTTP request (Web)
- **SESSION** : One instance per HTTP session (Web)
- **APPLICATION** : One instance per ServletContext (Web)

### @PostConstruct
```java
@Component
public class DatabaseConnection {
    @PostConstruct
    public void init() {
        System.out.println("Connecting to database...");
        // Executed after dependency injection
    }
}
```
- Method executed after dependency injection
- Used for bean initialization
- Automatically called by Spring
- Alternative : `InitializingBean` interface

### @PreDestroy
```java
@Component
public class DatabaseConnection {
    @PreDestroy
    public void cleanup() {
        System.out.println("Closing database connection...");
        // Executed before bean destruction
    }
}
```
- Method executed before bean destruction
- Used for resource cleanup
- Called when the container shuts down
- Alternative : `DisposableBean` interface

## 🌐 Jakarta CDI Annotations (Standard Java EE)

### @Named (equivalent to @Component)
```java
@Named
public class MarioGame implements Game {
    // CDI bean, compatible with Spring
}
```
- Jakarta CDI annotation
- Equivalent to Spring's `@Component`
- Java EE/Jakarta EE standard
- Spring supports it for compatibility

### @Inject (equivalent to @Autowired)
```java
@Named
public class GameRunner {
    @Inject
    private Game game; // CDI injection
}
```
- Jakarta CDI annotation for injection
- Equivalent to Spring's `@Autowired`
- More portable standard
- No `required` attribute

**Spring vs Jakarta CDI :**
- Spring : `@Component` + `@Autowired`
- Jakarta : `@Named` + `@Inject`
- Spring supports both for compatibility

## 🔄 @Component vs @Bean - When to use which?

### @Component (Automatic scan)
```java
@Component
public class MarioGame implements Game {
    // Spring automatically creates the bean
}
```
**Use @Component when :**
- ✅ You control the source code of the class
- ✅ Simple configuration with no complex logic
- ✅ Automatic scan is desired
- ✅ Classes in your own application

**Examples :** Services, Repositories, Controllers

### @Bean (Explicit configuration)
```java
@Configuration
public class AppConfig {
    @Bean
    public DataSource dataSource() {
        HikariConfig config = new HikariConfig();
        config.setJdbcUrl("jdbc:mysql://localhost/db");
        // Complex configuration
        return new HikariDataSource(config);
    }
}
```
**Use @Bean when :**
- ✅ Third-party classes (external libraries)
- ✅ Complex configuration needed
- ✅ Custom creation logic
- ✅ Conditions to evaluate before creation
- ✅ Fine control required

**Examples :** DataSource, RestTemplate, ObjectMapper

## 📝 XML Configuration vs Annotations

### XML Configuration (Old way)

**beans.xml :**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xsi:schemaLocation="http://www.springframework.org/schema/beans
       http://www.springframework.org/schema/beans/spring-beans.xsd">

    <!-- Bean definitions -->
    <bean id="marioGame" class="com.example.MarioGame"/>
    
    <bean id="gameRunner" class="com.example.GameRunner">
        <!-- Constructor injection -->
        <constructor-arg ref="marioGame"/>
    </bean>
    
    <!-- Setter injection -->
    <bean id="person" class="com.example.Person">
        <property name="name" value="Mario"/>
        <property name="age" value="30"/>
    </bean>
    
    <!-- Component scan in XML -->
    <context:component-scan base-package="com.example"/>
</beans>
```

**Loading :**
```java
ApplicationContext context = new ClassPathXmlApplicationContext("beans.xml");
GameRunner runner = context.getBean(GameRunner.class);
```

### Annotation-based Configuration (Modern) ⭐

```java
@Configuration
@ComponentScan("com.example")
public class AppConfig {
    
    @Bean
    public Game game() {
        return new MarioGame();
    }
    
    @Bean
    public GameRunner gameRunner(Game game) {
        return new GameRunner(game);
    }
}
```

**Loading :**
```java
ApplicationContext context = new AnnotationConfigApplicationContext(AppConfig.class);
GameRunner runner = context.getBean(GameRunner.class);
```

### XML vs Annotations Comparison

| Criteria | XML | Annotations |
|---------|-----|-------------|
| **Readability** | Verbose | Concise |
| **Maintenance** | Hard | Easy |
| **Type-safety** | ❌ No | ✅ Yes |
| **Refactoring** | Risky | Safe (IDE) |
| **Config/code separation** | ✅ Yes | ❌ No |
| **Learning curve** | Complex | Simple |
| **Modern standard** | ❌ Outdated | ✅ Current |

**Recommendation :** Use annotations for new projects. XML is no longer used except for maintaining legacy code.

### Hybrid Configuration (If needed)
```java
@Configuration
@ImportResource("classpath:legacy-beans.xml") // Import XML
@ComponentScan("com.example")
public class AppConfig {
    @Bean
    public Game game() {
        return new MarioGame();
    }
}
```

## 📊 Best Practices Summary

✅ **DO :**
- Prefer Constructor Injection
- Use `@Component` for your own classes
- Use `@Bean` for third-party classes
- Annotation-based configuration
- Only one `@Primary` per type
- `@Qualifier` to remove ambiguities
- `@Lazy` for heavy beans
- `@PostConstruct`/`@PreDestroy` for lifecycle

❌ **AVOID :**
- Field Injection
- XML configuration (except legacy)
- Multiple `@Primary` for the same type
- PROTOTYPE scope without a good reason
- Mutable beans in Singleton scope




# Q&A - Java Spring Framework Core

## 🔗 Loose Coupling vs Tight Coupling

**Q1 : What is Tight Coupling and why is it a problem?**
**A :** Tight Coupling means a class directly depends on a concrete implementation rather than an abstraction. Example : `private MarioGame game = new MarioGame();`. Problems : can't change the implementation without modifying the code, hard to test, rigid and non-reusable code.

**Q2 : How does Spring help achieve Loose Coupling?**
**A :** Spring uses Inversion of Control (IoC) and Dependency Injection to achieve Loose Coupling. Instead of instantiating dependencies with `new`, Spring injects them automatically via interfaces, making it easy to swap implementations without changing the code.

**Q3 : Give a concrete example of converting Tight Coupling code to Loose Coupling**
**A :** 
- **Tight Coupling :** `private MarioGame game = new MarioGame();` - directly depends on MarioGame
- **Loose Coupling :** `private Game game;` with constructor injection - depends on the Game interface, the implementation is injected by Spring

## 🏭 Spring IoC Container

**Q4 : What is Inversion of Control (IoC)?**
**A :** IoC means the control of object creation and management is inverted : instead of the developer creating objects with `new`, it's the Spring framework (IoC Container) that creates, configures, and manages their lifecycle.

**Q5 : What is the difference between BeanFactory and ApplicationContext?**
**A :**
- **BeanFactory** : Basic IoC container, lazy initialization (beans created on demand), limited features
- **ApplicationContext** : Extends BeanFactory, eager initialization (beans created at startup), advanced features (i18n, events, AOP). ApplicationContext is recommended for all use cases.

**Q6 : How do you get a bean from the ApplicationContext?**
**A :**
```java
// By type
GameRunner runner = context.getBean(GameRunner.class);

// By name
GameRunner runner = (GameRunner) context.getBean("gameRunner");

// By name and type (recommended)
GameRunner runner = context.getBean("gameRunner", GameRunner.class);
```

**Q7 : What types of ApplicationContext are available?**
**A :**
- **AnnotationConfigApplicationContext** : Annotation-based configuration (modern)
- **ClassPathXmlApplicationContext** : XML configuration (legacy)
- **WebApplicationContext** : For web applications

## 🫘 Java Bean vs POJO vs Spring Bean

**Q8 : What is the difference between POJO, Java Bean, and Spring Bean?**
**A :**
- **POJO** : Simple Java class with no constraints or specific rules
- **Java Bean** : POJO following strict conventions (Serializable, empty constructor, getters/setters)
- **Spring Bean** : Any object managed by the Spring IoC Container, can be a simple POJO

**Q9 : What are the strict rules of a Java Bean?**
**A :**
1. Must implement `Serializable`
2. Required no-argument constructor
3. Private properties with public getters/setters
4. Naming conventions (getName, setName)

**Q10 : Does a Spring Bean need to follow Java Bean conventions?**
**A :** No, a Spring Bean can be any Java class (POJO). It doesn't need to follow strict Java Bean conventions. Spring just manages its lifecycle.

## 📦 Spring Modules

**Q11 : Name 5 main modules of the Spring Framework**
**A :**
1. **Spring Core** : IoC Container, Dependency Injection
2. **Spring MVC** : Web applications, REST APIs
3. **Spring JDBC** : Simplified database access
4. **Spring Security** : Authentication and authorization
5. **Spring Test** : Unit and integration testing support

## 💉 Dependency Injection

**Q12 : What are the 3 Dependency Injection approaches in Spring?**
**A :**
1. **Constructor-based Injection** : Via constructor (✅ recommended)
2. **Setter-based Injection** : Via setter methods
3. **Field Injection** : Directly on fields with @Autowired (⚠️ not recommended)

**Q13 : Why is Constructor Injection recommended?**
**A :** 
- Mandatory and immutable dependencies (`final` fields)
- Thread-safe and easy to test
- Makes dependencies explicit
- Impossible to have an object in an invalid state
- Official Spring recommendation

**Q14 : What are the downsides of Field Injection?**
**A :**
- Can't use `final` (mutable dependencies)
- Hard to test (requires reflection)
- Hides class dependencies
- Can't instantiate the class without Spring
- Avoid in production

**Q15 : When should you use Setter Injection?**
**A :** For optional dependencies or when you want to allow reconfiguring the bean after creation. Constructor Injection is still preferred for mandatory dependencies.

## 🎯 Core Annotations

**Q16 : What is the difference between @Configuration and @Component?**
**A :**
- **@Configuration** : Configuration class containing @Bean methods to create beans programmatically
- **@Component** : Marks the class itself as a bean, detected by component scan

**Q17 : Explain the role of @Bean**
**A :** @Bean is used in a @Configuration class to declare that a method produces a bean managed by Spring. Allows fine control over bean creation. Mainly used for third-party classes or complex configuration.

**Q18 : What is @ComponentScan and how does it work?**
**A :** @ComponentScan enables automatic package scanning to detect classes annotated with @Component, @Service, @Repository, @Controller. Spring automatically registers them as beans. The scan is recursive over sub-packages.

**Q19 : Is @Autowired required on constructors?**
**A :** No, since Spring 4.3, @Autowired is optional if the class has only one constructor. Spring automatically uses it for injection.

## 🎯 @Primary vs @Qualifier

**Q20 : What is the difference between @Primary and @Qualifier?**
**A :**
- **@Primary** : Defines the default bean when multiple candidates exist. Global choice.
- **@Qualifier** : Explicitly specifies which bean to inject at the injection point. Local and precise choice.

**Q21 : When should you use @Primary?**
**A :** When you have multiple implementations of an interface and one is clearly the default choice for most cases. Example : primary vs secondary database.

**Q22 : How do you use @Qualifier with @Autowired?**
**A :**
```java
@Component
@Qualifier("mario")
public class MarioGame implements Game { }

@Autowired
@Qualifier("mario")
private Game game; // Forces injection of MarioGame
```

**Q23 : Can you have multiple @Primary for the same type?**
**A :** No, only one @Primary per type is allowed, otherwise Spring throws an ambiguity exception. Use @Qualifier for multiple choices.

## 📊 Scopes and Lifecycle

**Q24 : What are the main Bean Scopes in Spring?**
**A :**
- **SINGLETON** (default) : One instance per IoC Container
- **PROTOTYPE** : New instance on each request
- **REQUEST** : One instance per HTTP request (Web)
- **SESSION** : One instance per HTTP session (Web)
- **APPLICATION** : One instance per ServletContext (Web)

**Q25 : What is the difference between Singleton and Prototype scope?**
**A :**
- **Singleton** : Spring creates a single instance reused everywhere (default)
- **Prototype** : Spring creates a new instance on each injection or getBean()

**Q26 : What is @Lazy and when should you use it?**
**A :** @Lazy tells Spring to create the bean only on first use rather than at startup. Useful for heavy beans that are rarely used or to avoid circular dependencies.

**Q27 : Explain @PostConstruct and @PreDestroy**
**A :**
- **@PostConstruct** : Method executed after dependency injection, used for initialization
- **@PreDestroy** : Method executed before bean destruction, used for resource cleanup

**Q28 : In what order does the bean lifecycle execute?**
**A :**
1. Bean instantiation
2. Dependency injection (@Autowired)
3. @PostConstruct
4. Bean ready to use
5. @PreDestroy (on shutdown)
6. Destruction

## 🌐 Jakarta CDI

**Q29 : What is the difference between Spring annotations and Jakarta CDI?**
**A :**
- **Spring** : @Component + @Autowired (Spring-specific)
- **Jakarta CDI** : @Named + @Inject (Java EE/Jakarta standard)
Spring supports both for portability.

**Q30 : Why use @Named and @Inject instead of @Component and @Autowired?**
**A :** For portability. @Named and @Inject are Jakarta EE standards that work with different containers (Spring, CDI Weld, etc.). Use them if your code needs to be portable across frameworks.

## 🔄 @Component vs @Bean

**Q31 : When should you use @Component vs @Bean?**
**A :**
- **@Component** : For your own classes (controlled source code), automatic detection
- **@Bean** : For third-party classes (libraries), complex configuration, custom creation logic

**Q32 : Can you combine @Component and @Bean in the same application?**
**A :** Yes, it's even recommended. Use @Component for your services/repositories, and @Bean in @Configuration for DataSource, RestTemplate, ObjectMapper, etc.

**Q33 : Give a typical example of @Bean usage**
**A :**
```java
@Configuration
public class AppConfig {
    @Bean
    public DataSource dataSource() {
        HikariConfig config = new HikariConfig();
        config.setJdbcUrl("jdbc:mysql://localhost/db");
        config.setUsername("user");
        config.setPassword("pass");
        return new HikariDataSource(config);
    }
}
```
Used because HikariDataSource is a third-party class requiring complex configuration.

## 📝 XML Configuration vs Annotations

**Q34 : What are the advantages of annotations over XML?**
**A :**
- More concise and readable code
- Type-safe (errors caught at compile time)
- Safe refactoring with IDE
- Easier maintenance
- Modern standard (XML is outdated)

**Q35 : Are there cases where XML is preferred?**
**A :** Almost never for new projects. XML can only be used to maintain legacy code or if you want a strict separation between configuration and code (very rare today).

**Q36 : How do you migrate from XML configuration to annotations?**
**A :**
1. Create a @Configuration class
2. Replace `<bean>` with @Bean or @Component
3. Replace `<context:component-scan>` with @ComponentScan
4. Replace ClassPathXmlApplicationContext with AnnotationConfigApplicationContext

**Q37 : Example of an XML bean equivalent in annotations?**
**A :**
**XML :**
```xml
<bean id="gameRunner" class="com.example.GameRunner">
    <constructor-arg ref="marioGame"/>
</bean>
```
**Annotations :**
```java
@Component
public class GameRunner {
    @Autowired
    public GameRunner(Game game) { }
}
```

## 🎓 Advanced Questions

**Q38 : What happens if Spring finds multiple beans of the same type without @Primary or @Qualifier?**
**A :** Spring throws a `NoUniqueBeanDefinitionException` at startup because it can't determine which bean to inject. Solution : add @Primary to one bean or use @Qualifier at the injection point.

**Q39 : Can you inject a Prototype bean into a Singleton bean?**
**A :** Yes, but be careful : the Prototype will be created only once during injection into the Singleton. To get a new instance on each use, use `ApplicationContext.getBean()` or a Provider.

**Q40 : What is the difference between @Component, @Service, @Repository, and @Controller?**
**A :**
- **@Component** : Generic annotation for any Spring component
- **@Service** : Specialization for the service layer (business logic)
- **@Repository** : Specialization for the DAO layer (data access), adds exception translation
- **@Controller** : Specialization for MVC controllers
Technically identical, but improve readability and allow specific processing.

**Q41 : How do you test a class with Dependency Injection?**
**A :** With Constructor Injection, it's simple :
```java
// Unit test
Game mockGame = Mockito.mock(Game.class);
GameRunner runner = new GameRunner(mockGame); // No need for Spring
```
This is why Constructor Injection is recommended for testability.

**Q42 : What does context.getBean() do exactly?**
**A :** `context.getBean()` retrieves a bean from the Spring IoC Container. For a Singleton, it always returns the same instance. For a Prototype, it creates a new instance on each call.

**Q43 : When is the ApplicationContext created in a Spring Boot application?**
**A :** During application startup with `SpringApplication.run()`. Spring scans components, creates beans, injects dependencies, then the application is ready.

**Q44 : Can you have multiple @Configuration classes?**
**A :** Yes, you can have multiple @Configuration classes. Spring processes all of them. Use @Import to explicitly reference other configurations if needed.

**Q45 : What is the best practice : a single @Configuration or multiple?**
**A :** Multiple @Configuration classes organized by functional domain (DatabaseConfig, SecurityConfig, WebConfig). Improves readability and maintenance.
