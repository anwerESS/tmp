## [**..**](./00_index.md)

## AOP (Aspect Oriented Programming) - Points Clés

### **Concept général**

**AOP** est un paradigme de programmation qui permet de séparer les **cross-cutting concerns** (préoccupations transversales) de la logique métier principale.

**Cross-Cutting Concerns** : Fonctionnalités qui s'appliquent à plusieurs layers de l'application :
- **Logging** (enregistrer les appels de méthodes)
- **Security** (authentification, autorisation)
- **Performance monitoring** (mesurer le temps d'exécution)
- **Transaction management**
- **Exception handling**
- **Caching**

**Problème sans AOP :** Ces concerns sont dispersés dans tout le code (duplication, couplage).

**Solution avec AOP :** Centraliser ces concerns dans des **Aspects** séparés.

---

### **Terminologie AOP**

**1. Aspect**
- Classe qui contient le code des cross-cutting concerns
- **Aspect = Advice + Pointcut**
- Annotation : `@Aspect`

**2. Advice**
- Le **code à exécuter** (what?)
- Exemples : logging logic, authentication logic
- Types : @Before, @After, @AfterReturning, @AfterThrowing, @Around

**3. Pointcut**
- **Expression qui identifie** les méthodes à intercepter (where?)
- Définit quand l'advice doit être exécuté
- Exemple : `execution(* com.example.service.*.*(..))`

**4. Join Point**
- Point d'exécution dans l'application (appel de méthode, exception, etc.)
- Endroit où l'aspect peut être appliqué

**5. Weaving**
- Processus de liaison des aspects au code applicatif
- Peut être fait à compile-time, load-time, ou runtime

---

### **Annotations principales**

| Annotation | Description | Quand s'exécute |
|------------|-------------|-----------------|
| **@Before** | Exécute **avant** l'appel de la méthode | Avant le join point |
| **@After** | Exécute **après** la méthode (succès OU exception) | Toujours après le join point |
| **@AfterReturning** | Exécute **uniquement si** la méthode réussit | Après succès uniquement |
| **@AfterThrowing** | Exécute **uniquement si** la méthode lève une exception | Après exception uniquement |
| **@Around** | Exécute **avant ET après** la méthode (plus puissant) | Entoure complètement le join point |
| **@Pointcut** | Définit une expression pointcut réutilisable | N/A (définition seulement) |

---

### **Configuration de base**

**1. Dépendance Maven :**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-aop</artifactId>
</dependency>
```

**2. Activer AOP (automatique avec Spring Boot) :**
```java
@SpringBootApplication
@EnableAspectJAutoProxy  // Optionnel avec Spring Boot
public class Application {
    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }
}
```

---

### **Exemples d'implémentation**

**1. @Before - Logging avant l'exécution**
```java
@Aspect
@Component
public class LoggingAspect {
    
    private static final Logger log = LoggerFactory.getLogger(LoggingAspect.class);
    
    // Pointcut : toutes les méthodes du package service
    @Before("execution(* com.example.service.*.*(..))")
    public void logBefore(JoinPoint joinPoint) {
        log.info("Before method: {} with arguments: {}", 
                joinPoint.getSignature(), 
                Arrays.toString(joinPoint.getArgs()));
    }
}
```

**2. @After - Toujours exécuté après (succès ou exception)**
```java
@After("execution(* com.example.service.*.*(..))")
public void logAfter(JoinPoint joinPoint) {
    log.info("After method: {}", joinPoint.getSignature());
}
```

**3. @AfterReturning - Uniquement après succès**
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

**4. @AfterThrowing - Uniquement en cas d'exception**
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

**5. @Around - Avant ET après (le plus puissant)**
```java
@Around("execution(* com.example.service.*.*(..))")
public Object logAround(ProceedingJoinPoint joinPoint) throws Throwable {
    // AVANT l'exécution
    log.info("Before method: {}", joinPoint.getSignature());
    long startTime = System.currentTimeMillis();
    
    Object result = null;
    try {
        // Exécuter la méthode cible
        result = joinPoint.proceed();
        
        // APRÈS l'exécution (succès)
        long timeTaken = System.currentTimeMillis() - startTime;
        log.info("Method {} executed in {} ms", 
                joinPoint.getSignature(), 
                timeTaken);
        
    } catch (Exception e) {
        // APRÈS l'exécution (exception)
        log.error("Method {} threw exception", joinPoint.getSignature(), e);
        throw e;
    }
    
    return result;
}
```

---

### **Pointcut Expressions**

**Syntaxe de base :**
```java
execution(modifiers? return-type declaring-type? method-name(params) throws?)
```

**Exemples courants :**

```java
// Toutes les méthodes du package service
execution(* com.example.service.*.*(..))

// Toutes les méthodes qui retournent String
execution(String com.example.service.*.*(..))

// Méthode spécifique
execution(* com.example.service.UserService.findById(..))

// Toutes les méthodes commençant par "save"
execution(* com.example.service.*.save*(..))

// Toutes les méthodes avec un seul paramètre Long
execution(* com.example.service.*.*(Long))

// Toutes les méthodes du package et sous-packages
execution(* com.example.service..*.*(..))

// Toutes les méthodes publiques
execution(public * com.example.service.*.*(..))
```

---

### **Best Practice - Common Pointcut Definitions**

Créer des pointcuts réutilisables avec **@Pointcut** :

```java
@Aspect
@Component
public class CommonPointcuts {
    
    // Pointcut pour toutes les méthodes du layer service
    @Pointcut("execution(* com.example.service.*.*(..))")
    public void serviceMethods() {}
    
    // Pointcut pour toutes les méthodes du layer repository
    @Pointcut("execution(* com.example.repository.*.*(..))")
    public void repositoryMethods() {}
    
    // Pointcut pour toutes les méthodes du layer controller
    @Pointcut("execution(* com.example.controller.*.*(..))")
    public void controllerMethods() {}
    
    // Pointcut combiné pour tous les layers
    @Pointcut("serviceMethods() || repositoryMethods() || controllerMethods()")
    public void allLayers() {}
}

@Aspect
@Component
public class LoggingAspect {
    
    // Réutiliser les pointcuts définis
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

### **Timer avec @Around - Mesurer le temps d'exécution**

```java
@Aspect
@Component
public class PerformanceAspect {
    
    private static final Logger log = LoggerFactory.getLogger(PerformanceAspect.class);
    
    @Around("execution(* com.example.service.*.*(..))")
    public Object measureExecutionTime(ProceedingJoinPoint joinPoint) throws Throwable {
        long startTime = System.currentTimeMillis();
        
        // Exécuter la méthode
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

### **Custom Annotation pour Track Time**

**1. Créer l'annotation :**
```java
@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
public @interface TrackTime {
}
```

**2. Créer l'Aspect :**
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

**3. Utiliser l'annotation :**
```java
@Service
public class UserService {
    
    @TrackTime  // L'aspect va mesurer le temps automatiquement
    public User findById(Long id) {
        // Logique métier
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

**JoinPoint :**
- Utilisé avec @Before, @After, @AfterReturning, @AfterThrowing
- Fournit des infos sur la méthode interceptée
- **Ne peut pas contrôler** l'exécution de la méthode

```java
@Before("execution(* com.example.*.*(..))")
public void logBefore(JoinPoint joinPoint) {
    String methodName = joinPoint.getSignature().getName();
    Object[] args = joinPoint.getArgs();
    // Ne peut pas exécuter la méthode
}
```

**ProceedingJoinPoint :**
- Utilisé **uniquement** avec @Around
- Étend JoinPoint
- Permet de **contrôler l'exécution** avec `proceed()`

```java
@Around("execution(* com.example.*.*(..))")
public Object logAround(ProceedingJoinPoint joinPoint) throws Throwable {
    // Code avant
    Object result = joinPoint.proceed();  // Exécute la méthode
    // Code après
    return result;
}
```

---

### **Order des Aspects**

Si plusieurs aspects s'appliquent au même pointcut, utiliser **@Order** :

```java
@Aspect
@Component
@Order(1)  // Exécuté en premier
public class LoggingAspect { }

@Aspect
@Component
@Order(2)  // Exécuté en second
public class SecurityAspect { }

@Aspect
@Component
@Order(3)  // Exécuté en dernier
public class PerformanceAspect { }
```

**Ordre d'exécution @Before :**
1. LoggingAspect
2. SecurityAspect
3. PerformanceAspect
4. **Méthode cible**

**Ordre d'exécution @After (inverse) :**
1. **Méthode cible**
2. PerformanceAspect
3. SecurityAspect
4. LoggingAspect

---

### **Use Cases courants**

**1. Logging Aspect :**
```java
@Before("execution(* com.example.service.*.*(..))")
public void logMethodCall(JoinPoint joinPoint) {
    log.info("Method called: {}", joinPoint.getSignature());
}
```

**2. Security Aspect :**
```java
@Before("@annotation(com.example.security.RequiresRole)")
public void checkSecurity(JoinPoint joinPoint) {
    // Vérifier l'autorisation
    if (!hasPermission()) {
        throw new AccessDeniedException("Unauthorized");
    }
}
```

**3. Transaction Aspect :**
```java
@Around("@annotation(org.springframework.transaction.annotation.Transactional)")
public Object manageTransaction(ProceedingJoinPoint joinPoint) throws Throwable {
    // Commencer transaction
    Object result = joinPoint.proceed();
    // Commit transaction
    return result;
}
```

**4. Exception Handling Aspect :**
```java
@AfterThrowing(pointcut = "execution(* com.example.service.*.*(..))", 
               throwing = "exception")
public void handleException(JoinPoint joinPoint, Exception exception) {
    log.error("Exception in {}: {}", 
             joinPoint.getSignature(), 
             exception.getMessage());
    // Envoyer notification, sauvegarder en DB, etc.
}
```

**5. Caching Aspect :**
```java
@Around("@annotation(org.springframework.cache.annotation.Cacheable)")
public Object cacheResult(ProceedingJoinPoint joinPoint) throws Throwable {
    // Vérifier cache
    if (cacheExists()) {
        return getCachedResult();
    }
    Object result = joinPoint.proceed();
    // Sauvegarder en cache
    return result;
}
```

---

### **Avantages et Inconvénients**

**Avantages :**
- ✅ **Separation of concerns** : Code métier séparé des cross-cutting concerns
- ✅ **Réutilisabilité** : Un aspect peut s'appliquer à plusieurs méthodes
- ✅ **Maintenabilité** : Changement centralisé
- ✅ **Moins de duplication** : Pas besoin de répéter le logging partout

**Inconvénients :**
- ❌ **Complexité** : Difficile à débugger (code exécuté "magiquement")
- ❌ **Performance overhead** : Légère surcharge
- ❌ **Courbe d'apprentissage** : Syntaxe des pointcuts à maîtriser

---

### **Points clés**

- **AOP** = séparation des cross-cutting concerns (logging, security, performance)
- **Aspect** = Advice + Pointcut
- **Advice** = code à exécuter (what?)
- **Pointcut** = expression pour identifier les méthodes (where?)
- **@Before** : avant la méthode
- **@After** : après la méthode (toujours)
- **@AfterReturning** : après succès uniquement
- **@AfterThrowing** : après exception uniquement
- **@Around** : avant ET après (le plus puissant, peut contrôler l'exécution)
- **Common Pointcuts** : définir des pointcuts réutilisables avec @Pointcut
- **Custom annotations** : créer des annotations comme @TrackTime pour un usage ciblé
- **JoinPoint** : infos sur la méthode interceptée
- **ProceedingJoinPoint** : permet d'exécuter la méthode avec proceed()

---

## Questions/Réponses - AOP

### **1. Qu'est-ce que l'AOP (Aspect Oriented Programming) ?**

**Réponse :**
**AOP** est un paradigme de programmation qui permet de séparer les **cross-cutting concerns** (préoccupations transversales) de la logique métier principale de l'application.

**Cross-Cutting Concerns** sont des fonctionnalités qui s'appliquent à plusieurs layers de l'application :
- **Logging** (enregistrer les appels de méthodes)
- **Security** (authentification, autorisation)
- **Performance monitoring** (mesurer le temps d'exécution)
- **Transaction management**
- **Exception handling**
- **Caching**

**Problème sans AOP :**
Ces concerns sont **dispersés et dupliqués** dans tout le code (web layer, business layer, data layer).

**Solution avec AOP :**
Centraliser ces concerns dans des **Aspects** séparés, qui sont appliqués automatiquement aux méthodes ciblées sans modifier le code métier.

**Exemple :** Au lieu d'ajouter du logging manuellement dans chaque méthode, on crée un aspect qui le fait automatiquement pour toutes les méthodes d'un package.

---

### **2. Expliquez la terminologie de base d'AOP : Aspect, Advice, Pointcut, Join Point**

**Réponse :**

**1. Aspect :**
- Classe qui contient le code des cross-cutting concerns
- Combinaison de **Advice + Pointcut**
- Annotation : `@Aspect`

**2. Advice :**
- Le **code à exécuter** (WHAT?)
- Exemples : logging logic, authentication check, performance timer
- Types : @Before, @After, @AfterReturning, @AfterThrowing, @Around

**3. Pointcut :**
- **Expression qui identifie** les méthodes à intercepter (WHERE? WHEN?)
- Définit à quels join points l'advice doit être appliqué
- Exemple : `execution(* com.example.service.*.*(..))`

**4. Join Point :**
- Point d'exécution spécifique dans l'application
- Moment où l'aspect peut intervenir (appel de méthode, levée d'exception, etc.)
- Dans Spring AOP : principalement les appels de méthodes

**Formule :** **Aspect = Advice + Pointcut**

**Exemple complet :**
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

### **3. Quels sont les différents types d'Advice en AOP ?**

**Réponse :**

| Advice | Quand s'exécute | Use Case |
|--------|----------------|----------|
| **@Before** | **Avant** l'appel de la méthode | Logging, validation, security check |
| **@After** | **Après** la méthode (succès OU exception) | Cleanup, logging final |
| **@AfterReturning** | **Uniquement si** la méthode réussit | Logger le résultat, post-processing |
| **@AfterThrowing** | **Uniquement si** exception levée | Exception handling, logging errors |
| **@Around** | **Avant ET après** (le plus puissant) | Performance timing, transaction management, caching |

**Détails :**

**@Before :**
```java
@Before("execution(* com.example.service.*.*(..))")
public void logBefore(JoinPoint joinPoint) {
    // S'exécute AVANT la méthode
}
```

**@After (finally) :**
```java
@After("execution(* com.example.service.*.*(..))")
public void logAfter(JoinPoint joinPoint) {
    // S'exécute TOUJOURS après (succès ou exception)
    // Comme un bloc finally
}
```

**@AfterReturning :**
```java
@AfterReturning(
    pointcut = "execution(* com.example.service.*.*(..))",
    returning = "result")
public void logAfterReturning(JoinPoint joinPoint, Object result) {
    // S'exécute UNIQUEMENT si succès
    // Peut accéder au résultat
}
```

**@AfterThrowing :**
```java
@AfterThrowing(
    pointcut = "execution(* com.example.service.*.*(..))",
    throwing = "exception")
public void logAfterThrowing(JoinPoint joinPoint, Exception exception) {
    // S'exécute UNIQUEMENT si exception
    // Peut accéder à l'exception
}
```

**@Around (le plus puissant) :**
```java
@Around("execution(* com.example.service.*.*(..))")
public Object logAround(ProceedingJoinPoint joinPoint) throws Throwable {
    // Code AVANT
    Object result = joinPoint.proceed();  // Exécute la méthode
    // Code APRÈS
    return result;
}
```

---

### **4. Quelle est la différence entre @Before et @Around ?**

**Réponse :**

| @Before | @Around |
|---------|---------|
| Exécute **uniquement avant** | Exécute **avant ET après** |
| Ne peut pas empêcher l'exécution de la méthode | Peut empêcher l'exécution (ne pas appeler proceed()) |
| Ne peut pas modifier les arguments | Peut modifier les arguments |
| Ne peut pas modifier le résultat | Peut modifier le résultat |
| Utilise **JoinPoint** | Utilise **ProceedingJoinPoint** |
| Plus simple | Plus puissant mais plus complexe |

**Exemple @Before :**
```java
@Before("execution(* com.example.service.*.*(..))")
public void logBefore(JoinPoint joinPoint) {
    System.out.println("Before: " + joinPoint.getSignature());
    // La méthode s'exécutera automatiquement après
    // Ne peut pas l'empêcher
}
```

**Exemple @Around :**
```java
@Around("execution(* com.example.service.*.*(..))")
public Object logAround(ProceedingJoinPoint joinPoint) throws Throwable {
    System.out.println("Before: " + joinPoint.getSignature());
    
    // CONTRÔLE TOTAL sur l'exécution
    if (conditionNotMet()) {
        return null;  // Empêche l'exécution de la méthode
    }
    
    Object result = joinPoint.proceed();  // Exécute la méthode
    
    System.out.println("After: " + result);
    return result;  // Peut modifier le résultat
}
```

**Quand utiliser :**
- **@Before** : Pour simple logging, validation
- **@Around** : Pour performance timing, transaction management, caching

---

### **5. Qu'est-ce qu'une Pointcut Expression et comment l'écrire ?**

**Réponse :**
Une **Pointcut Expression** est une expression qui identifie les méthodes (join points) où l'advice doit être appliqué.

**Syntaxe de base :**
```
execution(modifiers? return-type declaring-type? method-name(params) throws?)
```

**Exemples courants :**

```java
// 1. Toutes les méthodes du package service
execution(* com.example.service.*.*(..))
// * = n'importe quel return type
// service.* = n'importe quelle classe dans service
// *.* = n'importe quelle méthode
// (..) = n'importe quels paramètres

// 2. Toutes les méthodes d'une classe spécifique
execution(* com.example.service.UserService.*(..))

// 3. Méthodes qui retournent String
execution(String com.example.service.*.*(..))

// 4. Méthodes commençant par "find"
execution(* com.example.service.*.find*(..))

// 5. Méthodes avec un paramètre Long
execution(* com.example.service.*.*(Long))

// 6. Méthodes avec deux paramètres (n'importe quel type)
execution(* com.example.service.*.*(*, *))

// 7. Package et sous-packages (.. après package)
execution(* com.example.service..*.*(..))

// 8. Méthodes publiques uniquement
execution(public * com.example.service.*.*(..))
```

**Opérateurs logiques :**
```java
// ET (&&)
execution(* com.example.service.*.*(..)) && args(Long)

// OU (||)
execution(* com.example.service.*.*(..)) || execution(* com.example.repository.*.*(..))

// NEGATION (!)
!execution(* com.example.service.UserService.*(..))
```

---

### **6. Comment créer un Aspect de logging dans Spring AOP ?**

**Réponse :**

**1. Ajouter la dépendance :**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-aop</artifactId>
</dependency>
```

**2. Créer l'Aspect :**
```java
@Aspect
@Component
public class LoggingAspect {
    
    private static final Logger log = LoggerFactory.getLogger(LoggingAspect.class);
    
    // Avant l'exécution
    @Before("execution(* com.example.service.*.*(..))")
    public void logBefore(JoinPoint joinPoint) {
        log.info("Calling method: {} with arguments: {}", 
                joinPoint.getSignature().getName(),
                Arrays.toString(joinPoint.getArgs()));
    }
    
    // Après le succès
    @AfterReturning(
        pointcut = "execution(* com.example.service.*.*(..))",
        returning = "result")
    public void logAfterReturning(JoinPoint joinPoint, Object result) {
        log.info("Method {} returned: {}", 
                joinPoint.getSignature().getName(),
                result);
    }
    
    // Après une exception
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

**3. Utilisation automatique :**
```java
@Service
public class UserService {
    
    public User findById(Long id) {
        // Le logging aspect s'applique automatiquement
        return userRepository.findById(id).orElse(null);
    }
}
```

---

### **7. Comment mesurer le temps d'exécution d'une méthode avec @Around ?**

**Réponse :**

```java
@Aspect
@Component
public class PerformanceAspect {
    
    private static final Logger log = LoggerFactory.getLogger(PerformanceAspect.class);
    
    @Around("execution(* com.example.service.*.*(..))")
    public Object measureExecutionTime(ProceedingJoinPoint joinPoint) throws Throwable {
        // Temps de début
        long startTime = System.currentTimeMillis();
        
        // Exécuter la méthode cible
        Object result = joinPoint.proceed();
        
        // Temps de fin
        long endTime = System.currentTimeMillis();
        long executionTime = endTime - startTime;
        
        // Logger le temps d'exécution
        log.info("Method {} executed in {} ms", 
                joinPoint.getSignature().getName(),
                executionTime);
        
        return result;
    }
}
```

**Output exemple :**
```
INFO - Method findById executed in 45 ms
INFO - Method saveUser executed in 123 ms
```

**Note :** @Around est **obligatoire** pour mesurer le temps car on doit exécuter du code avant ET après avec accès au temps.

---

### **8. Qu'est-ce qu'un Common Pointcut Definition et pourquoi l'utiliser ?**

**Réponse :**
Un **Common Pointcut Definition** est une pointcut réutilisable définie avec **@Pointcut** dans une classe centrale, pour éviter la duplication des expressions pointcut.

**Sans Common Pointcut (duplication) :**
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

**Avec Common Pointcut (réutilisation) :**
```java
@Aspect
@Component
public class CommonPointcuts {
    
    // Définir les pointcuts réutilisables
    @Pointcut("execution(* com.example.service.*.*(..))")
    public void serviceMethods() {}
    
    @Pointcut("execution(* com.example.repository.*.*(..))")
    public void repositoryMethods() {}
    
    @Pointcut("execution(* com.example.controller.*.*(..))")
    public void controllerMethods() {}
    
    // Pointcut combiné
    @Pointcut("serviceMethods() || repositoryMethods() || controllerMethods()")
    public void allLayers() {}
}

@Aspect
@Component
public class LoggingAspect {
    
    // Réutiliser les pointcuts
    @Before("com.example.aop.CommonPointcuts.serviceMethods()")
    public void logBefore(JoinPoint joinPoint) { }
    
    @After("com.example.aop.CommonPointcuts.serviceMethods()")
    public void logAfter(JoinPoint joinPoint) { }
}

@Aspect
@Component
public class PerformanceAspect {
    
    // Même pointcut, pas de duplication
    @Around("com.example.aop.CommonPointcuts.allLayers()")
    public Object trackTime(ProceedingJoinPoint joinPoint) throws Throwable { }
}
```

**Avantages :**
- ✅ Évite la duplication
- ✅ Centralise les définitions
- ✅ Facilite la maintenance
- ✅ Changement unique si modification nécessaire

---

### **9. Comment créer une custom annotation @TrackTime pour mesurer le temps d'exécution ?**

**Réponse :**

**Étape 1 : Créer l'annotation**
```java
@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
public @interface TrackTime {
}
```

**Étape 2 : Créer l'Aspect**
```java
@Aspect
@Component
public class TrackTimeAspect {
    
    private static final Logger log = LoggerFactory.getLogger(TrackTimeAspect.class);
    
    // Pointcut : toutes les méthodes annotées avec @TrackTime
    @Around("@annotation(com.example.aop.TrackTime)")
    public Object trackTime(ProceedingJoinPoint joinPoint) throws Throwable {
        long startTime = System.currentTimeMillis();
        
        // Exécuter la méthode
        Object result = joinPoint.proceed();
        
        long timeTaken = System.currentTimeMillis() - startTime;
        
        log.info("Method {} took {} ms to execute", 
                joinPoint.getSignature().getName(),
                timeTaken);
        
        return result;
    }
}
```

**Étape 3 : Utiliser l'annotation**
```java
@Service
public class UserService {
    
    @TrackTime  // L'aspect va automatiquement mesurer le temps
    public User findById(Long id) {
        return userRepository.findById(id).orElse(null);
    }
    
    @TrackTime
    public User save(User user) {
        return userRepository.save(user);
    }
    
    // Cette méthode ne sera PAS trackée (pas d'annotation)
    public List<User> findAll() {
        return userRepository.findAll();
    }
}
```

**Avantages :**
- ✅ **Ciblé** : appliqué uniquement aux méthodes annotées
- ✅ **Flexible** : facile d'ajouter/retirer l'annotation
- ✅ **Clair** : on voit directement quelles méthodes sont trackées
- ✅ **Réutilisable** : même annotation pour toute l'application

---

### **10. Quelle est la différence entre JoinPoint et ProceedingJoinPoint ?**

**Réponse :**

| JoinPoint | ProceedingJoinPoint |
|-----------|---------------------|
| Utilisé avec @Before, @After, @AfterReturning, @AfterThrowing | Utilisé **uniquement** avec @Around |
| Fournit des infos sur la méthode | Étend JoinPoint + contrôle d'exécution |
| **Ne peut pas** exécuter la méthode | **Peut** exécuter la méthode avec proceed() |
| Lecture seule | Contrôle complet |

**JoinPoint - Informations disponibles :**
```java
@Before("execution(* com.example.service.*.*(..))")
public void logBefore(JoinPoint joinPoint) {
    // Signature de la méthode
    String methodName = joinPoint.getSignature().getName();
    String className = joinPoint.getSignature().getDeclaringTypeName();
    
    // Arguments
    Object[] args = joinPoint.getArgs();
    
    // Target object
    Object target = joinPoint.getTarget();
    
    // Ne peut PAS exécuter ou contrôler la méthode
}
```

**ProceedingJoinPoint - Contrôle d'exécution :**
```java
@Around("execution(* com.example.service.*.*(..))")
public Object logAround(ProceedingJoinPoint joinPoint) throws Throwable {
    // Toutes les infos de JoinPoint
    String methodName = joinPoint.getSignature().getName();
    Object[] args = joinPoint.getArgs();
    
    // PLUS : Contrôle de l'exécution
    
    // Option 1 : Exécuter normalement
    Object result = joinPoint.proceed();
    
    // Option 2 : Exécuter avec arguments modifiés
    Object[] newArgs = modifyArgs(args);
    Object result = joinPoint.proceed(newArgs);
    
    // Option 3 : Ne pas exécuter du tout
    if (shouldNotExecute()) {
        return null;  // Court-circuite la méthode
    }
    
    // Option 4 : Modifier le résultat
    Object result = joinPoint.proceed();
    return modifyResult(result);
}
```

---

### **11. Comment gérer les exceptions dans un Aspect ?**

**Réponse :**

**Option 1 : @AfterThrowing (logging uniquement)**
```java
@AfterThrowing(
    pointcut = "execution(* com.example.service.*.*(..))",
    throwing = "exception")
public void handleException(JoinPoint joinPoint, Exception exception) {
    log.error("Exception in method {}: {}", 
             joinPoint.getSignature().getName(),
             exception.getMessage());
    
    // Envoyer notification, sauvegarder en DB, etc.
    // L'exception continue à se propager
}
```

**Option 2 : @Around (gestion complète)**
```java
@Around("execution(* com.example.service.*.*(..))")
public Object handleException(ProceedingJoinPoint joinPoint) throws Throwable {
    try {
        return joinPoint.proceed();
        
    } catch (SpecificException e) {
        log.error("Specific exception in {}", joinPoint.getSignature());
        // Gérer l'exception spécifiquement
        throw new CustomException("Wrapped exception", e);
        
    } catch (Exception e) {
        log.error("Unexpected exception in {}", joinPoint.getSignature());
        // Retourner une valeur par défaut au lieu de propager
        return getDefaultValue();
    }
}
```

**Option 3 : Combiné (logging + retry)**
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
                throw e;  // Max retries atteint
            }
            Thread.sleep(1000);  // Attendre avant retry
        }
    }
    
    throw new RuntimeException("Max retries exceeded");
}
```

---

### **12. Comment appliquer un Aspect uniquement aux méthodes annotées avec une annotation spécifique ?**

**Réponse :**

**Créer la pointcut avec @annotation :**
```java
@Aspect
@Component
public class SecurityAspect {
    
    // Applique uniquement aux méthodes avec @RequiresRole
    @Before("@annotation(com.example.security.RequiresRole)")
    public void checkSecurity(JoinPoint joinPoint) {
        log.info("Checking security for: {}", joinPoint.getSignature());
        
        if (!hasPermission()) {
            throw new AccessDeniedException("Unauthorized");
        }
    }
    
    // Avec accès à l'annotation
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
        // Aspect vérifie le rôle avant l'exécution
    }
}
```

---

### **13. Comment définir l'ordre d'exécution de plusieurs Aspects ?**

**Réponse :**

Utiliser **@Order** pour définir la priorité :

```java
@Aspect
@Component
@Order(1)  // S'exécute EN PREMIER
public class LoggingAspect {
    
    @Before("execution(* com.example.service.*.*(..))")
    public void log(JoinPoint joinPoint) {
        log.info("1. Logging aspect");
    }
}

@Aspect
@Component
@Order(2)  // S'exécute EN SECOND
public class SecurityAspect {
    
    @Before("execution(* com.example.service.*.*(..))")
    public void checkSecurity(JoinPoint joinPoint) {
        log.info("2. Security aspect");
    }
}

@Aspect
@Component
@Order(3)  // S'exécute EN DERNIER
public class PerformanceAspect {
    
    @Before("execution(* com.example.service.*.*(..))")
    public void startTimer(JoinPoint joinPoint) {
        log.info("3. Performance aspect");
    }
}
```

**Ordre d'exécution @Before :**
1. LoggingAspect (Order=1)
2. SecurityAspect (Order=2)
3. PerformanceAspect (Order=3)
4. **Méthode cible**

**Ordre d'exécution @After (INVERSE) :**
1. **Méthode cible**
2. PerformanceAspect (Order=3)
3. SecurityAspect (Order=2)
4. LoggingAspect (Order=1)

**Ordre d'exécution @Around (enveloppe) :**
```
LoggingAspect.before
  SecurityAspect.before
    PerformanceAspect.before
      MÉTHODE CIBLE
    PerformanceAspect.after
  SecurityAspect.after
LoggingAspect.after
```

**Note :** Plus le numéro est **bas**, plus l'aspect s'exécute **tôt** (pour @Before).

---

### **14. Comment accéder aux arguments d'une méthode dans un Aspect ?**

**Réponse :**

**Option 1 : Avec JoinPoint.getArgs()**
```java
@Before("execution(* com.example.service.*.*(..))")
public void logArguments(JoinPoint joinPoint) {
    Object[] args = joinPoint.getArgs();
    
    log.info("Method {} called with arguments: {}", 
            joinPoint.getSignature().getName(),
            Arrays.toString(args));
    
    // Accès par index
    if (args.length > 0) {
        Object firstArg = args[0];
    }
}
```

**Option 2 : Avec args() dans la pointcut**
```java
// Méthodes avec un seul argument Long
@Before("execution(* com.example.service.*.*(Long)) && args(id)")
public void logWithId(JoinPoint joinPoint, Long id) {
    log.info("Method called with ID: {}", id);
}

// Méthodes avec plusieurs arguments spécifiques
@Before("execution(* com.example.service.*.save(..)) && args(user)")
public void logWithUser(JoinPoint joinPoint, User user) {
    log.info("Saving user: {}", user.getName());
}

// Capturer plusieurs arguments
@Before("execution(* com.example.service.*.update(..)) && args(id, user)")
public void logUpdate(JoinPoint joinPoint, Long id, User user) {
    log.info("Updating user {} with ID: {}", user.getName(), id);
}
```

**Option 3 : Modifier les arguments avec @Around**
```java
@Around("execution(* com.example.service.*.save(..)) && args(user)")
public Object modifyArguments(ProceedingJoinPoint joinPoint, User user) throws Throwable {
    // Modifier l'argument avant l'exécution
    user.setLastModified(new Date());
    
    // Exécuter avec l'argument modifié
    return joinPoint.proceed(new Object[]{user});
}
```

---

### **15. Comment créer un Aspect pour la gestion des transactions ?**

**Réponse :**

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
            
            // Exécuter la méthode
            Object result = joinPoint.proceed();
            
            // Commit si succès
            transactionManager.commit(status);
            log.info("Transaction committed for: {}", joinPoint.getSignature());
            
            return result;
            
        } catch (Exception e) {
            // Rollback si exception
            transactionManager.rollback(status);
            log.error("Transaction rolled back for: {}", joinPoint.getSignature(), e);
            throw e;
        }
    }
}
```

**Note :** Spring fournit déjà @Transactional, cet exemple est pour comprendre comment ça fonctionne en interne.

---

### **16. Comment créer un Aspect pour le caching ?**

**Réponse :**

```java
@Aspect
@Component
public class CachingAspect {
    
    private static final Logger log = LoggerFactory.getLogger(CachingAspect.class);
    private final Map<String, Object> cache = new ConcurrentHashMap<>();
    
    @Around("@annotation(com.example.cache.Cacheable)")
    public Object cacheResult(ProceedingJoinPoint joinPoint) throws Throwable {
        // Créer une clé de cache basée sur méthode + arguments
        String cacheKey = generateCacheKey(joinPoint);
        
        // Vérifier si le résultat est en cache
        if (cache.containsKey(cacheKey)) {
            log.info("Cache hit for: {}", cacheKey);
            return cache.get(cacheKey);
        }
        
        log.info("Cache miss for: {}", cacheKey);
        
        // Exécuter la méthode
        Object result = joinPoint.proceed();
        
        // Sauvegarder en cache
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
        // Résultat mis en cache automatiquement
        return userRepository.findById(id).orElse(null);
    }
}
```

---

### **17. Quelles sont les limitations de Spring AOP ?**

**Réponse :**

**1. Proxy-based (limitations) :**
- Spring AOP utilise des **proxies dynamiques**
- Ne fonctionne **que sur les méthodes publiques**
- Les appels **internes** (self-invocation) ne déclenchent pas les aspects

```java
@Service
public class UserService {
    
    @Autowired
    private UserService self;  // Injection du proxy
    
    @TrackTime
    public void methodA() {
        self.methodB();  // ✅ L'aspect s'applique (appel via proxy)
    }
    
    @TrackTime
    public void methodB() {
        // Logic
    }
}
```

**2. Seulement method execution :**
- Spring AOP ne supporte que les **method execution join points**
- Pas de support pour field access, constructor calls, etc.
- AspectJ (full AOP) supporte plus de join points

**3. Performance overhead :**
- Légère surcharge due aux proxies
- Impact sur les méthodes appelées fréquemment

**4. Uniquement pour Spring beans :**
- Les aspects ne s'appliquent qu'aux beans gérés par Spring
- Pas d'aspect sur les objets créés avec `new`

```java
// ✅ Fonctionne (Spring bean)
@Service
public class UserService {
    @TrackTime
    public void method() { }
}

// ❌ Ne fonctionne PAS (objet créé manuellement)
UserService service = new UserService();
service.method();  // Pas d'aspect
```

**5. Complexité de debugging :**
- Code exécuté "magiquement"
- Stack traces plus difficiles à lire
- Peut masquer des erreurs

**Alternatives :** Utiliser **AspectJ** (full AOP) si ces limitations sont problématiques.

---

### **18. Quelle est la différence entre Spring AOP et AspectJ ?**

**Réponse :**

| Spring AOP | AspectJ |
|------------|---------|
| **Runtime weaving** (proxies dynamiques) | **Compile-time ou load-time weaving** |
| Proxy-based | Bytecode manipulation |
| Seulement **method execution** | Tous les join points (field, constructor, etc.) |
| **Uniquement Spring beans** | N'importe quel objet Java |
| Méthodes **publiques** uniquement | Toutes les visibilités |
| **Plus simple** à configurer | Plus complexe |
| **Moins performant** (légère surcharge) | Plus performant |
| Suffisant pour la plupart des cas | Pour besoins avancés |

**Spring AOP - Use Cases typiques :**
```java
@Aspect
@Component
public class LoggingAspect {
    @Before("execution(* com.example.service.*.*(..))")
    public void log(JoinPoint joinPoint) { }
}
```

**AspectJ - Capacités avancées :**
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

**Quand utiliser AspectJ :**
- Besoin de field access interception
- Besoin de constructor interception
- Self-invocation doit déclencher l'aspect
- Performance critique

**Quand utiliser Spring AOP :**
- Cas d'usage standards (logging, security, transactions)
- Simplicité de configuration
- Suffisant pour 95% des besoins

---

### **19. Comment tester un Aspect dans un test unitaire ?**

**Réponse :**

**Option 1 : Test d'intégration avec Spring Context**
```java
@SpringBootTest
class LoggingAspectTest {
    
    @Autowired
    private UserService userService;
    
    @Test
    void testLoggingAspectIsApplied() {
        // L'aspect s'applique automatiquement
        User user = userService.findById(1L);
        
        // Vérifier que la méthode a été exécutée
        assertNotNull(user);
        
        // Vérifier les logs (avec un appender de test)
        // ou mock du logger
    }
}
```

**Option 2 : Tester l'Aspect isolément avec Mockito**
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

**Option 3 : Test avec AspectJ bytecode weaving**
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
        // Aspect doit s'appliquer
        userService.findById(1L);
        
        // Vérifier que l'aspect a été exécuté
        verify(loggingAspect).logBefore(any(JoinPoint.class));
    }
}
```

**Option 4 : Test du comportement avec @Around**
```java
@SpringBootTest
class PerformanceAspectTest {
    
    @Autowired
    private SlowService slowService;
    
    @Test
    void testPerformanceAspectMeasuresTime() {
        // Exécuter une méthode lente
        slowService.slowMethod();
        
        // Vérifier que le temps a été loggé
        // (utiliser un test logger ou mock)
    }
}
```

---

### **20. Quelles sont les best practices pour utiliser AOP ?**

**Réponse :**

**1. Utiliser AOP uniquement pour les cross-cutting concerns**
```java
// ✅ Bon usage (cross-cutting)
@Aspect - Logging
@Aspect - Security
@Aspect - Performance monitoring
@Aspect - Transaction management

// ❌ Mauvais usage (business logic)
@Aspect - Calculer le prix d'un produit
@Aspect - Valider les règles métier spécifiques
```

**2. Créer des Common Pointcut Definitions**
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

**3. Utiliser des pointcuts spécifiques (pas trop larges)**
```java
// ❌ Trop large
@Before("execution(* *.*(..))")  // Toutes les méthodes !

// ✅ Spécifique
@Before("execution(* com.example.service.*.*(..))")
```

**4. Préférer @Before pour simple logging, @Around pour contrôle**
```java
// Simple logging
@Before("serviceMethods()")
public void log(JoinPoint jp) { }

// Performance timing
@Around("serviceMethods()")
public Object time(ProceedingJoinPoint pjp) throws Throwable { }
```

**5. Utiliser @Order pour gérer la priorité**
```java
@Aspect
@Order(1)  // Security en premier
public class SecurityAspect { }

@Aspect
@Order(2)  // Logging ensuite
public class LoggingAspect { }
```

**6. Logger de manière appropriée dans les aspects**
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

**7. Documenter les aspects**
```java
/**
 * Aspect pour logger toutes les méthodes du service layer.
 * S'applique à : com.example.service.*.*
 * Ordre : 2 (après SecurityAspect)
 */
@Aspect
@Component
@Order(2)
public class LoggingAspect { }
```

**8. Éviter la logique métier complexe dans les aspects**
```java
// ❌ Mauvais : logique métier dans l'aspect
@Before("serviceMethods()")
public void complexBusinessLogic(JoinPoint jp) {
    // Calculs complexes, appels DB, etc.
}

// ✅ Bon : logique simple (logging, validation basique)
@Before("serviceMethods()")
public void simpleLogging(JoinPoint jp) {
    log.info("Method: {}", jp.getSignature());
}
```

**9. Utiliser des custom annotations pour plus de clarté**
```java
// Plus clair que des pointcuts complexes
@TrackTime
@RequiresRole("ADMIN")
@Cacheable
public void myMethod() { }
```

**10. Tester les aspects**
```java
@SpringBootTest
class AspectTest {
    @Test
    void testAspectBehavior() {
        // Vérifier que l'aspect fonctionne correctement
    }
}
```

**11. Attention aux performances**
```java
// ❌ Aspect sur méthodes appelées très fréquemment
@Around("execution(* com.example.util.StringUtils.*(..))")

// ✅ Aspect sur méthodes business moins fréquentes
@Around("execution(* com.example.service.*.*(..))")
```

**12. Gérer les exceptions proprement dans @Around**
```java
@Around("serviceMethods()")
public Object handle(ProceedingJoinPoint pjp) throws Throwable {
    try {
        return pjp.proceed();
    } catch (Exception e) {
        log.error("Error", e);
        throw e;  // Toujours re-throw
    }
}
```

---

### **21. Comment créer un Aspect pour auditer les modifications en base de données ?**

**Réponse :**

```java
@Aspect
@Component
public class AuditAspect {
    
    private static final Logger log = LoggerFactory.getLogger(AuditAspect.class);
    
    @Autowired
    private AuditLogRepository auditLogRepository;
    
    @Autowired
    private SecurityContext securityContext;
    
    // Auditer toutes les opérations save/update/delete
    @AfterReturning(
        pointcut = "execution(* com.example.repository.*.save(..)) || " +
                   "execution(* com.example.repository.*.delete(..))",
        returning = "result")
    public void auditDataModification(JoinPoint joinPoint, Object result) {
        
        String methodName = joinPoint.getSignature().getName();
        String entityName = extractEntityName(joinPoint);
        Object[] args = joinPoint.getArgs();
        String username = securityContext.getCurrentUsername();
        
        // Créer le log d'audit
        AuditLog auditLog = new AuditLog();
        auditLog.setTimestamp(new Date());
        auditLog.setUsername(username);
        auditLog.setAction(methodName);
        auditLog.setEntityName(entityName);
        auditLog.setEntityId(extractEntityId(args));
        auditLog.setDetails(buildDetails(args));
        
        // Sauvegarder
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

// Entity pour l'audit
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
    
    // Getters et Setters
}
```

---

### **22. Comment combiner plusieurs pointcuts avec des opérateurs logiques ?**

**Réponse :**

**Opérateurs disponibles :**
- **&&** (AND) : Les deux conditions doivent être vraies
- **||** (OR) : Au moins une condition doit être vraie
- **!** (NOT) : Négation de la condition

**Exemples :**

```java
@Aspect
@Component
public class CombinedPointcutsAspect {
    
    // AND - Service methods ET qui retournent User
    @Before("execution(* com.example.service.*.*(..)) && " +
            "execution(com.example.model.User com.example.service.*.*(..))")
    public void logServiceMethodsReturningUser(JoinPoint jp) { }
    
    // OR - Service OU Repository methods
    @Before("execution(* com.example.service.*.*(..)) || " +
            "execution(* com.example.repository.*.*(..))")
    public void logServiceOrRepository(JoinPoint jp) { }
    
    // NOT - Toutes les méthodes SAUF celles qui commencent par "get"
    @Before("execution(* com.example.service.*.*(..)) && " +
            "!execution(* com.example.service.*.get*(..))")
    public void logNonGetterMethods(JoinPoint jp) { }
    
    // Combinaison complexe
    @Before("(execution(* com.example.service.*.*(..)) || " +
            " execution(* com.example.repository.*.*(..))) && " +
            "!execution(* com.example.service.*.find*(..))")
    public void complexPointcut(JoinPoint jp) { }
    
    // Avec Common Pointcuts
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
    
    // Pointcut combiné
    @Pointcut("serviceMethods() && methodsWithLongParameter()")
    public void serviceMethodsWithLong() {}
}
```

---

### **23. Comment créer un Aspect pour valider les paramètres avant l'exécution ?**

**Réponse :**

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
            
            // Valider avec Bean Validation
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
        // Les paramètres sont automatiquement validés avant l'exécution
        return userRepository.save(user);
    }
}
```

---

### **24. Comment créer un Aspect pour limiter le nombre d'appels (Rate Limiting) ?**

**Réponse :**

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
    
    // Map pour stocker les compteurs : clé = méthode + user, valeur = liste des timestamps
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
        
        // Nettoyer les anciens timestamps (hors fenêtre)
        timestamps.removeIf(timestamp -> currentTime - timestamp > timeWindowMs);
        
        // Vérifier la limite
        if (timestamps.size() >= rateLimit.maxCalls()) {
            log.warn("Rate limit exceeded for user {} on method {}", 
                    username, 
                    joinPoint.getSignature().getName());
            
            throw new RateLimitExceededException(
                String.format("Rate limit exceeded: max %d calls per %d seconds", 
                             rateLimit.maxCalls(), 
                             rateLimit.timeWindowSeconds()));
        }
        
        // Ajouter le timestamp actuel
        timestamps.add(currentTime);
        
        log.debug("Rate limit check passed for user {} ({}/{} calls)", 
                 username, 
                 timestamps.size(), 
                 rateLimit.maxCalls());
        
        // Exécuter la méthode
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
    @RateLimit(maxCalls = 5, timeWindowSeconds = 60)  // Max 5 appels par minute
    public ResponseEntity<Data> getData() {
        // Protégé par rate limiting
        return ResponseEntity.ok(dataService.getData());
    }
}
```

---

**Conseil final :** Prépare-toi à expliquer un cas d'usage concret d'AOP que tu as utilisé ou que tu utiliserais dans un projet réel. Les interviewers aiment les exemples pratiques ! 💪