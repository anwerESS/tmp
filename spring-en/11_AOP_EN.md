## [**..**](./00_index.md)

## AOP (Aspect Oriented Programming) - Key Points

### **General Concept**

**AOP** is a programming paradigm that allows separating **cross-cutting concerns** from the main business logic.

**Cross-Cutting Concerns**: Features that apply to multiple layers of the application:
- **Logging** (recording method calls)
- **Security** (authentication, authorization)
- **Performance monitoring** (measuring execution time)
- **Transaction management**
- **Exception handling**
- **Caching**

**Problem without AOP:** These concerns are scattered throughout the code (duplication, coupling).

**Solution with AOP:** Centralize these concerns in separate **Aspects**.

---

### **AOP Terminology**

**1. Aspect**
- Class that contains the cross-cutting concern code
- **Aspect = Advice + Pointcut**
- Annotation: `@Aspect`

**2. Advice**
- The **code to execute** (what?)
- Examples: logging logic, authentication logic
- Types: @Before, @After, @AfterReturning, @AfterThrowing, @Around

**3. Pointcut**
- **Expression that identifies** the methods to intercept (where?)
- Defines when the advice should be executed
- Example: `execution(* com.example.service.*.*(..))`

**4. Join Point**
- Execution point in the application (method call, exception, etc.)
- Place where the aspect can be applied

**5. Weaving**
- Process of linking aspects to application code
- Can be done at compile-time, load-time, or runtime

---

### **Main Annotations**

| Annotation | Description | When it runs |
|------------|-------------|--------------|
| **@Before** | Runs **before** the method call | Before the join point |
| **@After** | Runs **after** the method (success OR exception) | Always after the join point |
| **@AfterReturning** | Runs **only if** the method succeeds | After success only |
| **@AfterThrowing** | Runs **only if** the method throws an exception | After exception only |
| **@Around** | Runs **before AND after** the method (most powerful) | Completely surrounds the join point |
| **@Pointcut** | Defines a reusable pointcut expression | N/A (definition only) |

---

### **Basic Configuration**

**1. Maven Dependency:**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-aop</artifactId>
</dependency>
```

**2. Enable AOP (automatic with Spring Boot):**
```java
@SpringBootApplication
@EnableAspectJAutoProxy  // Optional with Spring Boot
public class Application {
    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }
}
```

---

### **Implementation Examples**

**1. @Before - Logging before execution**
```java
@Aspect
@Component
public class LoggingAspect {
    
    private static final Logger log = LoggerFactory.getLogger(LoggingAspect.class);
    
    // Pointcut: all methods in the service package
    @Before("execution(* com.example.service.*.*(..))")
    public void logBefore(JoinPoint joinPoint) {
        log.info("Before method: {} with arguments: {}", 
                joinPoint.getSignature(), 
                Arrays.toString(joinPoint.getArgs()));
    }
}
```

**2. @After - Always executed after (success or exception)**
```java
@After("execution(* com.example.service.*.*(..))")
public void logAfter(JoinPoint joinPoint) {
    log.info("After method: {}", joinPoint.getSignature());
}
```

**3. @AfterReturning - Only after success**
```java
@AfterReturning(
    pointcut = "execution(* com.example.service.*.*(..))",
    returning = "result")
public void logAfterReturning(JoinPoint joinPoint, Object result) {
    log.info("Method {} returned: {}", 
            joinPoint.getSignature(), 
            result);
}
```

**4. @AfterThrowing - Only on exception**
```java
@AfterThrowing(
    pointcut = "execution(* com.example.service.*.*(..))",
    throwing = "exception")
public void logAfterThrowing(JoinPoint joinPoint, Exception exception) {
    log.error("Method {} threw exception: {}", 
             joinPoint.getSignature(), 
             exception.getMessage());
}
```

**5. @Around - Before AND after (the most powerful)**
```java
@Around("execution(* com.example.service.*.*(..))")
public Object logAround(ProceedingJoinPoint joinPoint) throws Throwable {
    // BEFORE execution
    log.info("Before method: {}", joinPoint.getSignature());
    long startTime = System.currentTimeMillis();
    
    Object result = null;
    try {
        // Execute the target method
        result = joinPoint.proceed();
        
        // AFTER execution (success)
        long timeTaken = System.currentTimeMillis() - startTime;
        log.info("Method {} executed in {} ms", 
                joinPoint.getSignature(), 
                timeTaken);
        
    } catch (Exception e) {
        // AFTER execution (exception)
        log.error("Method {} threw exception", joinPoint.getSignature(), e);
        throw e;
    }
    
    return result;
}
```

---

### **Pointcut Expressions**

**Basic syntax:**
```java
execution(modifiers? return-type declaring-type? method-name(params) throws?)
```

**Common examples:**

```java
// All methods in the service package
execution(* com.example.service.*.*(..))

// All methods that return String
execution(String com.example.service.*.*(..))

// Specific method
execution(* com.example.service.UserService.findById(..))

// All methods starting with "save"
execution(* com.example.service.*.save*(..))

// All methods with a single Long parameter
execution(* com.example.service.*.*(Long))

// All methods in the package and sub-packages
execution(* com.example.service..*.*(..))

// All public methods
execution(public * com.example.service.*.*(..))
```

---

### **Best Practice - Common Pointcut Definitions**

Create reusable pointcuts with **@Pointcut**:

```java
@Aspect
@Component
public class CommonPointcuts {
    
    // Pointcut for all methods in the service layer
    @Pointcut("execution(* com.example.service.*.*(..))")
    public void serviceMethods() {}
    
    // Pointcut for all methods in the repository layer
    @Pointcut("execution(* com.example.repository.*.*(..))")
    public void repositoryMethods() {}
    
    // Pointcut for all methods in the controller layer
    @Pointcut("execution(* com.example.controller.*.*(..))")
    public void controllerMethods() {}
    
    // Combined pointcut for all layers
    @Pointcut("serviceMethods() || repositoryMethods() || controllerMethods()")
    public void allLayers() {}
}

@Aspect
@Component
public class LoggingAspect {
    
    // Reuse the defined pointcuts
    @Before("com.example.aop.CommonPointcuts.serviceMethods()")
    public void logBeforeService(JoinPoint joinPoint) {
        log.info("Calling service method: {}", joinPoint.getSignature());
    }
    
    @Before("com.example.aop.CommonPointcuts.allLayers()")
    public void logBeforeAllLayers(JoinPoint joinPoint) {
        log.info("Calling method: {}", joinPoint.getSignature());
    }
}
```

---

### **Timer with @Around - Measuring Execution Time**

```java
@Aspect
@Component
public class PerformanceAspect {
    
    private static final Logger log = LoggerFactory.getLogger(PerformanceAspect.class);
    
    @Around("execution(* com.example.service.*.*(..))")
    public Object measureExecutionTime(ProceedingJoinPoint joinPoint) throws Throwable {
        long startTime = System.currentTimeMillis();
        
        // Execute the method
        Object result = joinPoint.proceed();
        
        long endTime = System.currentTimeMillis();
        long executionTime = endTime - startTime;
        
        log.info("Method {} executed in {} ms", 
                joinPoint.getSignature(), 
                executionTime);
        
        return result;
    }
}
```

---

### **Custom Annotation for Track Time**

**1. Create the annotation:**
```java
@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
public @interface TrackTime {
}
```

**2. Create the Aspect:**
```java
@Aspect
@Component
public class TrackTimeAspect {
    
    private static final Logger log = LoggerFactory.getLogger(TrackTimeAspect.class);
    
    @Around("@annotation(com.example.aop.TrackTime)")
    public Object trackTime(ProceedingJoinPoint joinPoint) throws Throwable {
        long startTime = System.currentTimeMillis();
        
        Object result = joinPoint.proceed();
        
        long timeTaken = System.currentTimeMillis() - startTime;
        
        log.info("Method {} took {} ms to execute", 
                joinPoint.getSignature().getName(), 
                timeTaken);
        
        return result;
    }
}
```

**3. Use the annotation:**
```java
@Service
public class UserService {
    
    @TrackTime  // The aspect will automatically measure execution time
    public User findById(Long id) {
        // Business logic
        return userRepository.findById(id).orElse(null);
    }
    
    @TrackTime
    public User save(User user) {
        return userRepository.save(user);
    }
}
```

---

### **JoinPoint vs ProceedingJoinPoint**

**JoinPoint:**
- Used with @Before, @After, @AfterReturning, @AfterThrowing
- Provides info about the intercepted method
- **Cannot control** the method execution

```java
@Before("execution(* com.example.*.*(..))")
public void logBefore(JoinPoint joinPoint) {
    String methodName = joinPoint.getSignature().getName();
    Object[] args = joinPoint.getArgs();
    // Cannot execute the method
}
```

**ProceedingJoinPoint:**
- Used **only** with @Around
- Extends JoinPoint
- Allows **controlling execution** with `proceed()`

```java
@Around("execution(* com.example.*.*(..))")
public Object logAround(ProceedingJoinPoint joinPoint) throws Throwable {
    // Code before
    Object result = joinPoint.proceed();  // Executes the method
    // Code after
    return result;
}
```

---

### **Aspect Order**

If multiple aspects apply to the same pointcut, use **@Order**:

```java
@Aspect
@Component
@Order(1)  // Executed first
public class LoggingAspect { }

@Aspect
@Component
@Order(2)  // Executed second
public class SecurityAspect { }

@Aspect
@Component
@Order(3)  // Executed last
public class PerformanceAspect { }
```

**@Before execution order:**
1. LoggingAspect
2. SecurityAspect
3. PerformanceAspect
4. **Target method**

**@After execution order (reversed):**
1. **Target method**
2. PerformanceAspect
3. SecurityAspect
4. LoggingAspect

---

### **Common Use Cases**

**1. Logging Aspect:**
```java
@Before("execution(* com.example.service.*.*(..))")
public void logMethodCall(JoinPoint joinPoint) {
    log.info("Method called: {}", joinPoint.getSignature());
}
```

**2. Security Aspect:**
```java
@Before("@annotation(com.example.security.RequiresRole)")
public void checkSecurity(JoinPoint joinPoint) {
    // Check authorization
    if (!hasPermission()) {
        throw new AccessDeniedException("Unauthorized");
    }
}
```

**3. Transaction Aspect:**
```java
@Around("@annotation(org.springframework.transaction.annotation.Transactional)")
public Object manageTransaction(ProceedingJoinPoint joinPoint) throws Throwable {
    // Start transaction
    Object result = joinPoint.proceed();
    // Commit transaction
    return result;
}
```

**4. Exception Handling Aspect:**
```java
@AfterThrowing(pointcut = "execution(* com.example.service.*.*(..))", 
               throwing = "exception")
public void handleException(JoinPoint joinPoint, Exception exception) {
    log.error("Exception in {}: {}", 
             joinPoint.getSignature(), 
             exception.getMessage());
    // Send notification, save to DB, etc.
}
```

**5. Caching Aspect:**
```java
@Around("@annotation(org.springframework.cache.annotation.Cacheable)")
public Object cacheResult(ProceedingJoinPoint joinPoint) throws Throwable {
    // Check cache
    if (cacheExists()) {
        return getCachedResult();
    }
    Object result = joinPoint.proceed();
    // Save to cache
    return result;
}
```

---

### **Advantages and Disadvantages**

**Advantages:**
- ✅ **Separation of concerns**: Business code separated from cross-cutting concerns
- ✅ **Reusability**: One aspect can apply to multiple methods
- ✅ **Maintainability**: Centralized changes
- ✅ **Less duplication**: No need to repeat logging everywhere

**Disadvantages:**
- ❌ **Complexity**: Hard to debug (code executed "magically")
- ❌ **Performance overhead**: Small overhead
- ❌ **Learning curve**: Pointcut syntax to master

---

### **Key Points**

- **AOP** = separation of cross-cutting concerns (logging, security, performance)
- **Aspect** = Advice + Pointcut
- **Advice** = code to execute (what?)
- **Pointcut** = expression to identify methods (where?)
- **@Before**: before the method
- **@After**: after the method (always)
- **@AfterReturning**: after success only
- **@AfterThrowing**: after exception only
- **@Around**: before AND after (most powerful, can control execution)
- **Common Pointcuts**: define reusable pointcuts with @Pointcut
- **Custom annotations**: create annotations like @TrackTime for targeted use
- **JoinPoint**: info about the intercepted method
- **ProceedingJoinPoint**: allows executing the method with proceed()

---

## Questions/Answers - AOP

### **1. What is AOP (Aspect Oriented Programming)?**

**Answer:**
**AOP** is a programming paradigm that allows separating **cross-cutting concerns** from the main business logic of the application.

**Cross-Cutting Concerns** are features that apply to multiple layers of the application:
- **Logging** (recording method calls)
- **Security** (authentication, authorization)
- **Performance monitoring** (measuring execution time)
- **Transaction management**
- **Exception handling**
- **Caching**

**Problem without AOP:**
These concerns are **scattered and duplicated** throughout the code (web layer, business layer, data layer).

**Solution with AOP:**
Centralize these concerns in separate **Aspects**, which are automatically applied to targeted methods without modifying the business code.

**Example:** Instead of manually adding logging in every method, you create an aspect that does it automatically for all methods in a package.

---

### **2. Explain the basic AOP terminology: Aspect, Advice, Pointcut, Join Point**

**Answer:**

**1. Aspect:**
- Class that contains the cross-cutting concern code
- Combination of **Advice + Pointcut**
- Annotation: `@Aspect`

**2. Advice:**
- The **code to execute** (WHAT?)
- Examples: logging logic, authentication check, performance timer
- Types: @Before, @After, @AfterReturning, @AfterThrowing, @Around

**3. Pointcut:**
- **Expression that identifies** the methods to intercept (WHERE? WHEN?)
- Defines at which join points the advice should be applied
- Example: `execution(* com.example.service.*.*(..))`

**4. Join Point:**
- Specific execution point in the application
- Moment where the aspect can intervene (method call, exception thrown, etc.)
- In Spring AOP: mainly method calls

**Formula:** **Aspect = Advice + Pointcut**

**Full example:**
```java
@Aspect                                          // Aspect
@Component
public class LoggingAspect {
    
    @Before(                                     // Advice (type)
        "execution(* com.example.service.*.*(..))"  // Pointcut (expression)
    )
    public void logBefore(JoinPoint joinPoint) {   // Join Point (info)
        System.out.println("Calling: " + joinPoint.getSignature());
    }
}
```

---

### **3. What are the different types of Advice in AOP?**

**Answer:**

| Advice | When it runs | Use Case |
|--------|-------------|----------|
| **@Before** | **Before** the method call | Logging, validation, security check |
| **@After** | **After** the method (success OR exception) | Cleanup, final logging |
| **@AfterReturning** | **Only if** the method succeeds | Log the result, post-processing |
| **@AfterThrowing** | **Only if** an exception is thrown | Exception handling, error logging |
| **@Around** | **Before AND after** (most powerful) | Performance timing, transaction management, caching |

**Details:**

**@Before:**
```java
@Before("execution(* com.example.service.*.*(..))")
public void logBefore(JoinPoint joinPoint) {
    // Runs BEFORE the method
}
```

**@After (finally):**
```java
@After("execution(* com.example.service.*.*(..))")
public void logAfter(JoinPoint joinPoint) {
    // Runs ALWAYS after (success or exception)
    // Like a finally block
}
```

**@AfterReturning:**
```java
@AfterReturning(
    pointcut = "execution(* com.example.service.*.*(..))",
    returning = "result")
public void logAfterReturning(JoinPoint joinPoint, Object result) {
    // Runs ONLY on success
    // Can access the result
}
```

**@AfterThrowing:**
```java
@AfterThrowing(
    pointcut = "execution(* com.example.service.*.*(..))",
    throwing = "exception")
public void logAfterThrowing(JoinPoint joinPoint, Exception exception) {
    // Runs ONLY on exception
    // Can access the exception
}
```

**@Around (most powerful):**
```java
@Around("execution(* com.example.service.*.*(..))")
public Object logAround(ProceedingJoinPoint joinPoint) throws Throwable {
    // Code BEFORE
    Object result = joinPoint.proceed();  // Executes the method
    // Code AFTER
    return result;
}
```

---

### **4. What is the difference between @Before and @Around?**

**Answer:**

| @Before | @Around |
|---------|---------|
| Runs **only before** | Runs **before AND after** |
| Cannot prevent method execution | Can prevent execution (by not calling proceed()) |
| Cannot modify arguments | Can modify arguments |
| Cannot modify the result | Can modify the result |
| Uses **JoinPoint** | Uses **ProceedingJoinPoint** |
| Simpler | More powerful but more complex |

**@Before example:**
```java
@Before("execution(* com.example.service.*.*(..))")
public void logBefore(JoinPoint joinPoint) {
    System.out.println("Before: " + joinPoint.getSignature());
    // The method will execute automatically after
    // Cannot prevent it
}
```

**@Around example:**
```java
@Around("execution(* com.example.service.*.*(..))")
public Object logAround(ProceedingJoinPoint joinPoint) throws Throwable {
    System.out.println("Before: " + joinPoint.getSignature());
    
    // FULL CONTROL over execution
    if (conditionNotMet()) {
        return null;  // Prevents method execution
    }
    
    Object result = joinPoint.proceed();  // Executes the method
    
    System.out.println("After: " + result);
    return result;  // Can modify the result
}
```

**When to use:**
- **@Before**: For simple logging, validation
- **@Around**: For performance timing, transaction management, caching

---

### **5. What is a Pointcut Expression and how to write one?**

**Answer:**
A **Pointcut Expression** is an expression that identifies the methods (join points) where the advice should be applied.

**Basic syntax:**
```
execution(modifiers? return-type declaring-type? method-name(params) throws?)
```

**Common examples:**

```java
// 1. All methods in the service package
execution(* com.example.service.*.*(..))
// * = any return type
// service.* = any class in service
// *.* = any method
// (..) = any parameters

// 2. All methods of a specific class
execution(* com.example.service.UserService.*(..))

// 3. Methods that return String
execution(String com.example.service.*.*(..))

// 4. Methods starting with "find"
execution(* com.example.service.*.find*(..))

// 5. Methods with a Long parameter
execution(* com.example.service.*.*(Long))

// 6. Methods with two parameters (any type)
execution(* com.example.service.*.*(*, *))

// 7. Package and sub-packages (.. after package)
execution(* com.example.service..*.*(..))

// 8. Public methods only
execution(public * com.example.service.*.*(..))
```

**Logical operators:**
```java
// AND (&&)
execution(* com.example.service.*.*(..)) && args(Long)

// OR (||)
execution(* com.example.service.*.*(..)) || execution(* com.example.repository.*.*(..))

// NOT (!)
!execution(* com.example.service.UserService.*(..))
```

---

### **6. How to create a logging Aspect in Spring AOP?**

**Answer:**

**1. Add the dependency:**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-aop</artifactId>
</dependency>
```

**2. Create the Aspect:**
```java
@Aspect
@Component
public class LoggingAspect {
    
    private static final Logger log = LoggerFactory.getLogger(LoggingAspect.class);
    
    // Before execution
    @Before("execution(* com.example.service.*.*(..))")
    public void logBefore(JoinPoint joinPoint) {
        log.info("Calling method: {} with arguments: {}", 
                joinPoint.getSignature().getName(),
                Arrays.toString(joinPoint.getArgs()));
    }
    
    // After success
    @AfterReturning(
        pointcut = "execution(* com.example.service.*.*(..))",
        returning = "result")
    public void logAfterReturning(JoinPoint joinPoint, Object result) {
        log.info("Method {} returned: {}", 
                joinPoint.getSignature().getName(),
                result);
    }
    
    // After an exception
    @AfterThrowing(
        pointcut = "execution(* com.example.service.*.*(..))",
        throwing = "exception")
    public void logAfterThrowing(JoinPoint joinPoint, Exception exception) {
        log.error("Method {} threw exception: {}", 
                 joinPoint.getSignature().getName(),
                 exception.getMessage());
    }
}
```

**3. Automatic usage:**
```java
@Service
public class UserService {
    
    public User findById(Long id) {
        // The logging aspect applies automatically
        return userRepository.findById(id).orElse(null);
    }
}
```

---

### **7. How to measure execution time of a method with @Around?**

**Answer:**

```java
@Aspect
@Component
public class PerformanceAspect {
    
    private static final Logger log = LoggerFactory.getLogger(PerformanceAspect.class);
    
    @Around("execution(* com.example.service.*.*(..))")
    public Object measureExecutionTime(ProceedingJoinPoint joinPoint) throws Throwable {
        // Start time
        long startTime = System.currentTimeMillis();
        
        // Execute the target method
        Object result = joinPoint.proceed();
        
        // End time
        long endTime = System.currentTimeMillis();
        long executionTime = endTime - startTime;
        
        // Log the execution time
        log.info("Method {} executed in {} ms", 
                joinPoint.getSignature().getName(),
                executionTime);
        
        return result;
    }
}
```

**Example output:**
```
INFO - Method findById executed in 45 ms
INFO - Method saveUser executed in 123 ms
```

**Note:** @Around is **required** to measure time because you need to run code before AND after with access to the timing.

---

### **8. What is a Common Pointcut Definition and why use it?**

**Answer:**
A **Common Pointcut Definition** is a reusable pointcut defined with **@Pointcut** in a central class, to avoid duplicating pointcut expressions.

**Without Common Pointcut (duplication):**
```java
@Aspect
@Component
public class LoggingAspect {
    
    @Before("execution(* com.example.service.*.*(..))")
    public void logBefore() { }
    
    @After("execution(* com.example.service.*.*(..))")
    public void logAfter() { }
}

@Aspect
@Component
public class PerformanceAspect {
    
    @Around("execution(* com.example.service.*.*(..))")  // Duplication
    public Object trackTime() { }
}
```

**With Common Pointcut (reuse):**
```java
@Aspect
@Component
public class CommonPointcuts {
    
    // Define reusable pointcuts
    @Pointcut("execution(* com.example.service.*.*(..))")
    public void serviceMethods() {}
    
    @Pointcut("execution(* com.example.repository.*.*(..))")
    public void repositoryMethods() {}
    
    @Pointcut("execution(* com.example.controller.*.*(..))")
    public void controllerMethods() {}
    
    // Combined pointcut
    @Pointcut("serviceMethods() || repositoryMethods() || controllerMethods()")
    public void allLayers() {}
}

@Aspect
@Component
public class LoggingAspect {
    
    // Reuse the pointcuts
    @Before("com.example.aop.CommonPointcuts.serviceMethods()")
    public void logBefore(JoinPoint joinPoint) { }
    
    @After("com.example.aop.CommonPointcuts.serviceMethods()")
    public void logAfter(JoinPoint joinPoint) { }
}

@Aspect
@Component
public class PerformanceAspect {
    
    // Same pointcut, no duplication
    @Around("com.example.aop.CommonPointcuts.allLayers()")
    public Object trackTime(ProceedingJoinPoint joinPoint) throws Throwable { }
}
```

**Advantages:**
- ✅ Avoids duplication
- ✅ Centralizes definitions
- ✅ Easier maintenance
- ✅ Single change if modification is needed

---

### **9. How to create a custom @TrackTime annotation to measure execution time?**

**Answer:**

**Step 1: Create the annotation**
```java
@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
public @interface TrackTime {
}
```

**Step 2: Create the Aspect**
```java
@Aspect
@Component
public class TrackTimeAspect {
    
    private static final Logger log = LoggerFactory.getLogger(TrackTimeAspect.class);
    
    // Pointcut: all methods annotated with @TrackTime
    @Around("@annotation(com.example.aop.TrackTime)")
    public Object trackTime(ProceedingJoinPoint joinPoint) throws Throwable {
        long startTime = System.currentTimeMillis();
        
        // Execute the method
        Object result = joinPoint.proceed();
        
        long timeTaken = System.currentTimeMillis() - startTime;
        
        log.info("Method {} took {} ms to execute", 
                joinPoint.getSignature().getName(),
                timeTaken);
        
        return result;
    }
}
```

**Step 3: Use the annotation**
```java
@Service
public class UserService {
    
    @TrackTime  // The aspect will automatically measure time
    public User findById(Long id) {
        return userRepository.findById(id).orElse(null);
    }
    
    @TrackTime
    public User save(User user) {
        return userRepository.save(user);
    }
    
    // This method will NOT be tracked (no annotation)
    public List<User> findAll() {
        return userRepository.findAll();
    }
}
```

**Advantages:**
- ✅ **Targeted**: applied only to annotated methods
- ✅ **Flexible**: easy to add/remove the annotation
- ✅ **Clear**: you can see directly which methods are tracked
- ✅ **Reusable**: same annotation for the entire application

---

### **10. What is the difference between JoinPoint and ProceedingJoinPoint?**

**Answer:**

| JoinPoint | ProceedingJoinPoint |
|-----------|---------------------|
| Used with @Before, @After, @AfterReturning, @AfterThrowing | Used **only** with @Around |
| Provides info about the method | Extends JoinPoint + execution control |
| **Cannot** execute the method | **Can** execute the method with proceed() |
| Read-only | Full control |

**JoinPoint - Available information:**
```java
@Before("execution(* com.example.service.*.*(..))")
public void logBefore(JoinPoint joinPoint) {
    // Method signature
    String methodName = joinPoint.getSignature().getName();
    String className = joinPoint.getSignature().getDeclaringTypeName();
    
    // Arguments
    Object[] args = joinPoint.getArgs();
    
    // Target object
    Object target = joinPoint.getTarget();
    
    // CANNOT execute or control the method
}
```

**ProceedingJoinPoint - Execution control:**
```java
@Around("execution(* com.example.service.*.*(..))")
public Object logAround(ProceedingJoinPoint joinPoint) throws Throwable {
    // All info from JoinPoint
    String methodName = joinPoint.getSignature().getName();
    Object[] args = joinPoint.getArgs();
    
    // PLUS: Execution control
    
    // Option 1: Execute normally
    Object result = joinPoint.proceed();
    
    // Option 2: Execute with modified arguments
    Object[] newArgs = modifyArgs(args);
    Object result = joinPoint.proceed(newArgs);
    
    // Option 3: Don't execute at all
    if (shouldNotExecute()) {
        return null;  // Short-circuits the method
    }
    
    // Option 4: Modify the result
    Object result = joinPoint.proceed();
    return modifyResult(result);
}
```

---

### **11. How to handle exceptions in an Aspect?**

**Answer:**

**Option 1: @AfterThrowing (logging only)**
```java
@AfterThrowing(
    pointcut = "execution(* com.example.service.*.*(..))",
    throwing = "exception")
public void handleException(JoinPoint joinPoint, Exception exception) {
    log.error("Exception in method {}: {}", 
             joinPoint.getSignature().getName(),
             exception.getMessage());
    
    // Send notification, save to DB, etc.
    // The exception continues to propagate
}
```

**Option 2: @Around (full handling)**
```java
@Around("execution(* com.example.service.*.*(..))")
public Object handleException(ProceedingJoinPoint joinPoint) throws Throwable {
    try {
        return joinPoint.proceed();
        
    } catch (SpecificException e) {
        log.error("Specific exception in {}", joinPoint.getSignature());
        // Handle the exception specifically
        throw new CustomException("Wrapped exception", e);
        
    } catch (Exception e) {
        log.error("Unexpected exception in {}", joinPoint.getSignature());
        // Return a default value instead of propagating
        return getDefaultValue();
    }
}
```

**Option 3: Combined (logging + retry)**
```java
@Around("execution(* com.example.service.*.*(..))")
public Object retryOnException(ProceedingJoinPoint joinPoint) throws Throwable {
    int maxRetries = 3;
    int attempt = 0;
    
    while (attempt < maxRetries) {
        try {
            return joinPoint.proceed();
            
        } catch (TransientException e) {
            attempt++;
            log.warn("Attempt {} failed for {}, retrying...", 
                    attempt, 
                    joinPoint.getSignature().getName());
            
            if (attempt >= maxRetries) {
                throw e;  // Max retries reached
            }
            Thread.sleep(1000);  // Wait before retry
        }
    }
    
    throw new RuntimeException("Max retries exceeded");
}
```

---

### **12. How to apply an Aspect only to methods annotated with a specific annotation?**

**Answer:**

**Create the pointcut with @annotation:**
```java
@Aspect
@Component
public class SecurityAspect {
    
    // Applies only to methods with @RequiresRole
    @Before("@annotation(com.example.security.RequiresRole)")
    public void checkSecurity(JoinPoint joinPoint) {
        log.info("Checking security for: {}", joinPoint.getSignature());
        
        if (!hasPermission()) {
            throw new AccessDeniedException("Unauthorized");
        }
    }
    
    // With access to the annotation
    @Before("@annotation(requiresRole)")
    public void checkRole(JoinPoint joinPoint, RequiresRole requiresRole) {
        String requiredRole = requiresRole.value();
        
        if (!currentUserHasRole(requiredRole)) {
            throw new AccessDeniedException("Requires role: " + requiredRole);
        }
    }
}

// Annotation
@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
public @interface RequiresRole {
    String value() default "USER";
}

// Usage
@Service
public class AdminService {
    
    @RequiresRole("ADMIN")
    public void deleteUser(Long id) {
        // Aspect checks the role before execution
    }
}
```

---

### **13. How to define the execution order of multiple Aspects?**

**Answer:**

Use **@Order** to define priority:

```java
@Aspect
@Component
@Order(1)  // Runs FIRST
public class LoggingAspect {
    
    @Before("execution(* com.example.service.*.*(..))")
    public void log(JoinPoint joinPoint) {
        log.info("1. Logging aspect");
    }
}

@Aspect
@Component
@Order(2)  // Runs SECOND
public class SecurityAspect {
    
    @Before("execution(* com.example.service.*.*(..))")
    public void checkSecurity(JoinPoint joinPoint) {
        log.info("2. Security aspect");
    }
}

@Aspect
@Component
@Order(3)  // Runs LAST
public class PerformanceAspect {
    
    @Before("execution(* com.example.service.*.*(..))")
    public void startTimer(JoinPoint joinPoint) {
        log.info("3. Performance aspect");
    }
}
```

**@Before execution order:**
1. LoggingAspect (Order=1)
2. SecurityAspect (Order=2)
3. PerformanceAspect (Order=3)
4. **Target method**

**@After execution order (REVERSED):**
1. **Target method**
2. PerformanceAspect (Order=3)
3. SecurityAspect (Order=2)
4. LoggingAspect (Order=1)

**@Around execution order (wrapping):**
```
LoggingAspect.before
  SecurityAspect.before
    PerformanceAspect.before
      TARGET METHOD
    PerformanceAspect.after
  SecurityAspect.after
LoggingAspect.after
```

**Note:** The **lower** the number, the **earlier** the aspect runs (for @Before).

---

### **14. How to access a method's arguments inside an Aspect?**

**Answer:**

**Option 1: With JoinPoint.getArgs()**
```java
@Before("execution(* com.example.service.*.*(..))")
public void logArguments(JoinPoint joinPoint) {
    Object[] args = joinPoint.getArgs();
    
    log.info("Method {} called with arguments: {}", 
            joinPoint.getSignature().getName(),
            Arrays.toString(args));
    
    // Access by index
    if (args.length > 0) {
        Object firstArg = args[0];
    }
}
```

**Option 2: With args() in the pointcut**
```java
// Methods with a single Long argument
@Before("execution(* com.example.service.*.*(Long)) && args(id)")
public void logWithId(JoinPoint joinPoint, Long id) {
    log.info("Method called with ID: {}", id);
}

// Methods with multiple specific arguments
@Before("execution(* com.example.service.*.save(..)) && args(user)")
public void logWithUser(JoinPoint joinPoint, User user) {
    log.info("Saving user: {}", user.getName());
}

// Capture multiple arguments
@Before("execution(* com.example.service.*.update(..)) && args(id, user)")
public void logUpdate(JoinPoint joinPoint, Long id, User user) {
    log.info("Updating user {} with ID: {}", user.getName(), id);
}
```

**Option 3: Modify arguments with @Around**
```java
@Around("execution(* com.example.service.*.save(..)) && args(user)")
public Object modifyArguments(ProceedingJoinPoint joinPoint, User user) throws Throwable {
    // Modify the argument before execution
    user.setLastModified(new Date());
    
    // Execute with the modified argument
    return joinPoint.proceed(new Object[]{user});
}
```

---

### **15. How to create an Aspect for transaction management?**

**Answer:**

```java
@Aspect
@Component
public class TransactionAspect {
    
    private static final Logger log = LoggerFactory.getLogger(TransactionAspect.class);
    
    @Autowired
    private PlatformTransactionManager transactionManager;
    
    @Around("@annotation(org.springframework.transaction.annotation.Transactional)")
    public Object manageTransaction(ProceedingJoinPoint joinPoint) throws Throwable {
        
        TransactionStatus status = transactionManager.getTransaction(
                new DefaultTransactionDefinition());
        
        try {
            log.info("Starting transaction for: {}", joinPoint.getSignature());
            
            // Execute the method
            Object result = joinPoint.proceed();
            
            // Commit on success
            transactionManager.commit(status);
            log.info("Transaction committed for: {}", joinPoint.getSignature());
            
            return result;
            
        } catch (Exception e) {
            // Rollback on exception
            transactionManager.rollback(status);
            log.error("Transaction rolled back for: {}", joinPoint.getSignature(), e);
            throw e;
        }
    }
}
```

**Note:** Spring already provides @Transactional; this example is to understand how it works internally.

---

### **16. How to create an Aspect for caching?**

**Answer:**

```java
@Aspect
@Component
public class CachingAspect {
    
    private static final Logger log = LoggerFactory.getLogger(CachingAspect.class);
    private final Map<String, Object> cache = new ConcurrentHashMap<>();
    
    @Around("@annotation(com.example.cache.Cacheable)")
    public Object cacheResult(ProceedingJoinPoint joinPoint) throws Throwable {
        // Create a cache key based on method + arguments
        String cacheKey = generateCacheKey(joinPoint);
        
        // Check if the result is in cache
        if (cache.containsKey(cacheKey)) {
            log.info("Cache hit for: {}", cacheKey);
            return cache.get(cacheKey);
        }
        
        log.info("Cache miss for: {}", cacheKey);
        
        // Execute the method
        Object result = joinPoint.proceed();
        
        // Save to cache
        cache.put(cacheKey, result);
        
        return result;
    }
    
    private String generateCacheKey(ProceedingJoinPoint joinPoint) {
        return joinPoint.getSignature().toString() + 
               Arrays.toString(joinPoint.getArgs());
    }
}

// Custom annotation
@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
public @interface Cacheable {
}

// Usage
@Service
public class UserService {
    
    @Cacheable
    public User findById(Long id) {
        // Result automatically cached
        return userRepository.findById(id).orElse(null);
    }
}
```

---

### **17. What are the limitations of Spring AOP?**

**Answer:**

**1. Proxy-based (limitations):**
- Spring AOP uses **dynamic proxies**
- Works **only on public methods**
- **Internal calls** (self-invocation) do not trigger aspects

```java
@Service
public class UserService {
    
    @Autowired
    private UserService self;  // Inject the proxy
    
    @TrackTime
    public void methodA() {
        self.methodB();  // ✅ Aspect applies (call via proxy)
    }
    
    @TrackTime
    public void methodB() {
        // Logic
    }
}
```

**2. Method execution only:**
- Spring AOP only supports **method execution join points**
- No support for field access, constructor calls, etc.
- AspectJ (full AOP) supports more join points

**3. Performance overhead:**
- Small overhead due to proxies
- Impact on frequently called methods

**4. Only for Spring beans:**
- Aspects only apply to Spring-managed beans
- No aspect on objects created with `new`

```java
// ✅ Works (Spring bean)
@Service
public class UserService {
    @TrackTime
    public void method() { }
}

// ❌ Does NOT work (manually created object)
UserService service = new UserService();
service.method();  // No aspect
```

**5. Debugging complexity:**
- Code executed "magically"
- Stack traces harder to read
- Can hide errors

**Alternative:** Use **AspectJ** (full AOP) if these limitations are a problem.

---

### **18. What is the difference between Spring AOP and AspectJ?**

**Answer:**

| Spring AOP | AspectJ |
|------------|---------|
| **Runtime weaving** (dynamic proxies) | **Compile-time or load-time weaving** |
| Proxy-based | Bytecode manipulation |
| **Method execution only** | All join points (field, constructor, etc.) |
| **Spring beans only** | Any Java object |
| **Public methods** only | All visibilities |
| **Simpler** to configure | More complex |
| **Less performant** (small overhead) | More performant |
| Sufficient for most cases | For advanced needs |

**Spring AOP - Typical use cases:**
```java
@Aspect
@Component
public class LoggingAspect {
    @Before("execution(* com.example.service.*.*(..))")
    public void log(JoinPoint joinPoint) { }
}
```

**AspectJ - Advanced capabilities:**
```java
// Field access
@Before("get(* com.example.User.password)")
public void beforePasswordAccess() { }

// Constructor call
@Before("call(com.example.User.new(..))")
public void beforeUserCreation() { }

// Static initialization
@Before("staticinitialization(com.example.User)")
public void beforeStaticInit() { }
```

**When to use AspectJ:**
- Need field access interception
- Need constructor interception
- Self-invocation must trigger the aspect
- Performance is critical

**When to use Spring AOP:**
- Standard use cases (logging, security, transactions)
- Simplicity of configuration
- Sufficient for 95% of needs

---

### **19. How to test an Aspect in a unit test?**

**Answer:**

**Option 1: Integration test with Spring Context**
```java
@SpringBootTest
class LoggingAspectTest {
    
    @Autowired
    private UserService userService;
    
    @Test
    void testLoggingAspectIsApplied() {
        // Aspect applies automatically
        User user = userService.findById(1L);
        
        // Verify that the method was executed
        assertNotNull(user);
        
        // Verify logs (with a test appender)
        // or mock the logger
    }
}
```

**Option 2: Test the Aspect in isolation with Mockito**
```java
@ExtendWith(MockitoExtension.class)
class LoggingAspectTest {
    
    @InjectMocks
    private LoggingAspect loggingAspect;
    
    @Mock
    private JoinPoint joinPoint;
    
    @Mock
    private Signature signature;
    
    @Test
    void testLogBefore() {
        // Setup
        when(joinPoint.getSignature()).thenReturn(signature);
        when(signature.getName()).thenReturn("findById");
        when(joinPoint.getArgs()).thenReturn(new Object[]{1L});
        
        // Execute
        loggingAspect.logBefore(joinPoint);
        
        // Verify
        verify(joinPoint).getSignature();
        verify(joinPoint).getArgs();
    }
}
```

**Option 3: Test with AspectJ bytecode weaving**
```java
@RunWith(SpringRunner.class)
@ContextConfiguration(classes = {TestConfig.class})
@EnableLoadTimeWeaving
class AspectIntegrationTest {
    
    @Autowired
    private UserService userService;
    
    @Autowired
    private LoggingAspect loggingAspect;
    
    @Test
    void testAspectIsTriggered() {
        // Aspect must apply
        userService.findById(1L);
        
        // Verify that the aspect was executed
        verify(loggingAspect).logBefore(any(JoinPoint.class));
    }
}
```

**Option 4: Test behavior with @Around**
```java
@SpringBootTest
class PerformanceAspectTest {
    
    @Autowired
    private SlowService slowService;
    
    @Test
    void testPerformanceAspectMeasuresTime() {
        // Execute a slow method
        slowService.slowMethod();
        
        // Verify that the time was logged
        // (use a test logger or mock)
    }
}
```

---

### **20. What are the best practices for using AOP?**

**Answer:**

**1. Use AOP only for cross-cutting concerns**
```java
// ✅ Good usage (cross-cutting)
@Aspect - Logging
@Aspect - Security
@Aspect - Performance monitoring
@Aspect - Transaction management

// ❌ Bad usage (business logic)
@Aspect - Calculate the price of a product
@Aspect - Validate specific business rules
```

**2. Create Common Pointcut Definitions**
```java
@Aspect
@Component
public class CommonPointcuts {
    @Pointcut("execution(* com.example.service.*.*(..))")
    public void serviceMethods() {}
    
    @Pointcut("execution(* com.example.repository.*.*(..))")
    public void repositoryMethods() {}
}
```

**3. Use specific pointcuts (not too broad)**
```java
// ❌ Too broad
@Before("execution(* *.*(..))")  // All methods!

// ✅ Specific
@Before("execution(* com.example.service.*.*(..))")
```

**4. Prefer @Before for simple logging, @Around for control**
```java
// Simple logging
@Before("serviceMethods()")
public void log(JoinPoint jp) { }

// Performance timing
@Around("serviceMethods()")
public Object time(ProceedingJoinPoint pjp) throws Throwable { }
```

**5. Use @Order to manage priority**
```java
@Aspect
@Order(1)  // Security first
public class SecurityAspect { }

@Aspect
@Order(2)  // Logging next
public class LoggingAspect { }
```

**6. Log appropriately inside aspects**
```java
@Aspect
@Component
public class LoggingAspect {
    private static final Logger log = LoggerFactory.getLogger(LoggingAspect.class);
    
    @Before("serviceMethods()")
    public void log(JoinPoint jp) {
        log.debug("Calling: {}", jp.getSignature());  // DEBUG level
    }
    
    @AfterThrowing(pointcut = "serviceMethods()", throwing = "ex")
    public void logError(JoinPoint jp, Exception ex) {
        log.error("Error in {}", jp.getSignature(), ex);  // ERROR level
    }
}
```

**7. Document your aspects**
```java
/**
 * Aspect to log all methods in the service layer.
 * Applies to: com.example.service.*.*
 * Order: 2 (after SecurityAspect)
 */
@Aspect
@Component
@Order(2)
public class LoggingAspect { }
```

**8. Avoid complex business logic inside aspects**
```java
// ❌ Bad: business logic in the aspect
@Before("serviceMethods()")
public void complexBusinessLogic(JoinPoint jp) {
    // Complex calculations, DB calls, etc.
}

// ✅ Good: simple logic (logging, basic validation)
@Before("serviceMethods()")
public void simpleLogging(JoinPoint jp) {
    log.info("Method: {}", jp.getSignature());
}
```

**9. Use custom annotations for clarity**
```java
// Clearer than complex pointcuts
@TrackTime
@RequiresRole("ADMIN")
@Cacheable
public void myMethod() { }
```

**10. Test your aspects**
```java
@SpringBootTest
class AspectTest {
    @Test
    void testAspectBehavior() {
        // Verify that the aspect works correctly
    }
}
```

**11. Watch out for performance**
```java
// ❌ Aspect on very frequently called methods
@Around("execution(* com.example.util.StringUtils.*(..))")

// ✅ Aspect on less frequent business methods
@Around("execution(* com.example.service.*.*(..))")
```

**12. Handle exceptions properly in @Around**
```java
@Around("serviceMethods()")
public Object handle(ProceedingJoinPoint pjp) throws Throwable {
    try {
        return pjp.proceed();
    } catch (Exception e) {
        log.error("Error", e);
        throw e;  // Always re-throw
    }
}
```

---

### **21. How to create an Aspect to audit database modifications?**

**Answer:**

```java
@Aspect
@Component
public class AuditAspect {
    
    private static final Logger log = LoggerFactory.getLogger(AuditAspect.class);
    
    @Autowired
    private AuditLogRepository auditLogRepository;
    
    @Autowired
    private SecurityContext securityContext;
    
    // Audit all save/update/delete operations
    @AfterReturning(
        pointcut = "execution(* com.example.repository.*.save(..)) || " +
                   "execution(* com.example.repository.*.delete(..))",
        returning = "result")
    public void auditDataModification(JoinPoint joinPoint, Object result) {
        
        String methodName = joinPoint.getSignature().getName();
        String entityName = extractEntityName(joinPoint);
        Object[] args = joinPoint.getArgs();
        String username = securityContext.getCurrentUsername();
        
        // Create the audit log
        AuditLog auditLog = new AuditLog();
        auditLog.setTimestamp(new Date());
        auditLog.setUsername(username);
        auditLog.setAction(methodName);
        auditLog.setEntityName(entityName);
        auditLog.setEntityId(extractEntityId(args));
        auditLog.setDetails(buildDetails(args));
        
        // Save
        auditLogRepository.save(auditLog);
        
        log.info("Audit: User {} performed {} on {} at {}", 
                username, methodName, entityName, new Date());
    }
    
    private String extractEntityName(JoinPoint joinPoint) {
        String className = joinPoint.getSignature().getDeclaringTypeName();
        return className.substring(className.lastIndexOf('.') + 1)
                       .replace("Repository", "");
    }
    
    private Long extractEntityId(Object[] args) {
        if (args.length > 0 && args[0] != null) {
            try {
                Method getId = args[0].getClass().getMethod("getId");
                return (Long) getId.invoke(args[0]);
            } catch (Exception e) {
                return null;
            }
        }
        return null;
    }
    
    private String buildDetails(Object[] args) {
        return Arrays.stream(args)
                     .map(Object::toString)
                     .collect(Collectors.joining(", "));
    }
}

// Entity for the audit
@Entity
public class AuditLog {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    private Date timestamp;
    private String username;
    private String action;
    private String entityName;
    private Long entityId;
    private String details;
    
    // Getters and Setters
}
```

---

### **22. How to combine multiple pointcuts with logical operators?**

**Answer:**

**Available operators:**
- **&&** (AND): Both conditions must be true
- **||** (OR): At least one condition must be true
- **!** (NOT): Negation of the condition

**Examples:**

```java
@Aspect
@Component
public class CombinedPointcutsAspect {
    
    // AND - Service methods AND that return User
    @Before("execution(* com.example.service.*.*(..)) && " +
            "execution(com.example.model.User com.example.service.*.*(..))")
    public void logServiceMethodsReturningUser(JoinPoint jp) { }
    
    // OR - Service OR Repository methods
    @Before("execution(* com.example.service.*.*(..)) || " +
            "execution(* com.example.repository.*.*(..))")
    public void logServiceOrRepository(JoinPoint jp) { }
    
    // NOT - All methods EXCEPT those starting with "get"
    @Before("execution(* com.example.service.*.*(..)) && " +
            "!execution(* com.example.service.*.get*(..))")
    public void logNonGetterMethods(JoinPoint jp) { }
    
    // Complex combination
    @Before("(execution(* com.example.service.*.*(..)) || " +
            " execution(* com.example.repository.*.*(..))) && " +
            "!execution(* com.example.service.*.find*(..))")
    public void complexPointcut(JoinPoint jp) { }
    
    // With Common Pointcuts
    @Before("CommonPointcuts.serviceMethods() && " +
            "CommonPointcuts.methodsWithLongParameter()")
    public void serviceMethodsWithLongParam(JoinPoint jp) { }
}

// Common Pointcuts
@Aspect
@Component
public class CommonPointcuts {
    
    @Pointcut("execution(* com.example.service.*.*(..))")
    public void serviceMethods() {}
    
    @Pointcut("execution(* com.example.repository.*.*(..))")
    public void repositoryMethods() {}
    
    @Pointcut("execution(* *..*.*(Long))")
    public void methodsWithLongParameter() {}
    
    // Combined pointcut
    @Pointcut("serviceMethods() && methodsWithLongParameter()")
    public void serviceMethodsWithLong() {}
}
```

---

### **23. How to create an Aspect to validate parameters before execution?**

**Answer:**

```java
// Custom annotation
@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
public @interface ValidateParams {
}

@Aspect
@Component
public class ValidationAspect {
    
    private static final Logger log = LoggerFactory.getLogger(ValidationAspect.class);
    
    @Autowired
    private Validator validator;
    
    @Before("@annotation(com.example.validation.ValidateParams)")
    public void validateParameters(JoinPoint joinPoint) {
        Object[] args = joinPoint.getArgs();
        
        for (Object arg : args) {
            if (arg == null) {
                throw new IllegalArgumentException(
                    "Null parameter in " + joinPoint.getSignature());
            }
            
            // Validate with Bean Validation
            Set<ConstraintViolation<Object>> violations = validator.validate(arg);
            
            if (!violations.isEmpty()) {
                StringBuilder errors = new StringBuilder();
                violations.forEach(v -> errors.append(v.getMessage()).append("; "));
                
                throw new ConstraintViolationException(
                    "Validation failed for " + joinPoint.getSignature() + ": " + errors,
                    violations);
            }
        }
        
        log.debug("Parameters validated successfully for: {}", 
                 joinPoint.getSignature());
    }
}

// Usage
@Service
public class UserService {
    
    @ValidateParams
    public User createUser(User user) {
        // Parameters are automatically validated before execution
        return userRepository.save(user);
    }
}
```

---

### **24. How to create an Aspect to limit the number of calls (Rate Limiting)?**

**Answer:**

```java
// Custom annotation
@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
public @interface RateLimit {
    int maxCalls() default 10;
    int timeWindowSeconds() default 60;
}

@Aspect
@Component
public class RateLimitAspect {
    
    private static final Logger log = LoggerFactory.getLogger(RateLimitAspect.class);
    
    // Map to store counters: key = method + user, value = list of timestamps
    private final Map<String, Queue<Long>> callHistory = new ConcurrentHashMap<>();
    
    @Autowired
    private SecurityContext securityContext;
    
    @Around("@annotation(rateLimit)")
    public Object checkRateLimit(ProceedingJoinPoint joinPoint, RateLimit rateLimit) 
            throws Throwable {
        
        String username = securityContext.getCurrentUsername();
        String key = joinPoint.getSignature().toString() + ":" + username;
        
        Queue<Long> timestamps = callHistory.computeIfAbsent(key, 
                                                             k -> new ConcurrentLinkedQueue<>());
        
        long currentTime = System.currentTimeMillis();
        long timeWindowMs = rateLimit.timeWindowSeconds() * 1000L;
        
        // Remove old timestamps (outside the window)
        timestamps.removeIf(timestamp -> currentTime - timestamp > timeWindowMs);
        
        // Check the limit
        if (timestamps.size() >= rateLimit.maxCalls()) {
            log.warn("Rate limit exceeded for user {} on method {}", 
                    username, 
                    joinPoint.getSignature().getName());
            
            throw new RateLimitExceededException(
                String.format("Rate limit exceeded: max %d calls per %d seconds", 
                             rateLimit.maxCalls(), 
                             rateLimit.timeWindowSeconds()));
        }
        
        // Add the current timestamp
        timestamps.add(currentTime);
        
        log.debug("Rate limit check passed for user {} ({}/{} calls)", 
                 username, 
                 timestamps.size(), 
                 rateLimit.maxCalls());
        
        // Execute the method
        return joinPoint.proceed();
    }
}

// Exception
public class RateLimitExceededException extends RuntimeException {
    public RateLimitExceededException(String message) {
        super(message);
    }
}

// Usage
@RestController
@RequestMapping("/api")
public class ApiController {
    
    @GetMapping("/data")
    @RateLimit(maxCalls = 5, timeWindowSeconds = 60)  // Max 5 calls per minute
    public ResponseEntity<Data> getData() {
        // Protected by rate limiting
        return ResponseEntity.ok(dataService.getData());
    }
}
```

---

**Final tip:** Be ready to explain a concrete AOP use case that you have used or would use in a real project. Interviewers love practical examples! 💪
