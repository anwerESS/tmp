## [**..**](./00_index.md)

## Spring Web MVC - Guide Complet

### 1. **Architecture du Projet (Project Structure)**

```
src/main/java/
├── com.example.app/
│   ├── controller/        # @Controller classes
│   ├── service/           # Business logic layer
│   ├── repository/        # Data access layer (DAO)
│   ├── model/             # Domain entities/POJOs
│   └── config/            # Configuration classes
│
src/main/webapp/
├── WEB-INF/
│   ├── views/             # JSP files
│   │   ├── home.jsp
│   │   └── user-form.jsp
│   └── web.xml            # Deployment descriptor (optionnel avec Java config)
│
src/main/resources/
├── templates/             # Thymeleaf templates
├── static/                # CSS, JS, images
└── application.properties
```

### 2. **Les Layers (Couches)**

**Controller Layer** :
- Gère les requêtes HTTP entrantes
- Annotations : `@Controller`, `@RequestMapping`, `@GetMapping`, `@PostMapping`
- Retourne le nom de la **view** ou des données avec `@ResponseBody`

**Service Layer** :
- Contient la logique métier (business logic)
- Annotation : `@Service`
- Appelé par le controller, appelle le repository

**Repository/DAO Layer** :
- Gère l'accès aux données (database)
- Annotations : `@Repository` ou utilisation de Spring Data JPA
- Interagit avec la base de données

**Model Layer** :
- Objets domain (entities, DTOs, POJOs)
- Représente les données de l'application

### 3. **Le Pattern MVC**

**Model** : Données transférées entre controller et view (via `Model` ou `ModelAndView`)

**View** : Templates (JSP, Thymeleaf) qui affichent les données

**Controller** : Reçoit les requests, traite via services, retourne la view appropriée

### 4. **DispatcherServlet (Front Controller)**

- Point d'entrée unique pour toutes les requêtes HTTP
- Route les requests vers les controllers appropriés
- Flow : Request → DispatcherServlet → HandlerMapping → Controller → ViewResolver → View

### 5. **View Technologies**

**JSP (JavaServer Pages)** :
- Templates Java traditionnels avec syntaxe `<% %>`
- Utilise JSTL (JSP Standard Tag Library) : `<c:forEach>`, `<c:if>`
- Configuration du `ViewResolver` pour résoudre les noms de vues

**Thymeleaf** :
- Moteur de templates moderne, syntaxe HTML naturelle
- Attributs : `th:text`, `th:href`, `th:each`, `th:if`
- Meilleure intégration avec Spring Boot

### 6. **Annotations Clés**

**@Controller** : Marque une classe comme controller MVC

**@RequestMapping** : Map une URL à une méthode (ou classe)
```java
@RequestMapping(value = "/users", method = RequestMethod.GET)
```

**@GetMapping / @PostMapping** : Raccourcis pour GET/POST requests

**@RequestParam** : Récupère les query parameters
```java
public String show(@RequestParam("id") int userId)
```

**@PathVariable** : Récupère les variables d'URL
```java
@GetMapping("/users/{id}")
public String show(@PathVariable("id") int userId)
```

**@ModelAttribute** : 
- Bind les form data à un objet
- Ajoute des attributs au Model disponibles pour toutes les méthodes

**@SessionAttributes** : 
- Stocke des attributs du Model dans la HTTP session entre plusieurs requests
- Utile pour les **multi-step forms**, **wizards**, ou maintenir l'état utilisateur
```java
@Controller
@SessionAttributes("user")
public class UserController { ... }
```

**@ResponseBody** : 
- La return value est écrite directement dans le HTTP response body
- Pas d'interprétation comme un view name
- Utilisé pour les **REST APIs** (retourne JSON/XML)
- Combiné avec `@RestController` = `@Controller` + `@ResponseBody` sur toutes les méthodes

### 7. **Model et Data Binding**

**Model** : Interface pour passer des données au view
```java
@GetMapping("/users")
public String list(Model model) {
    model.addAttribute("users", userService.findAll());
    return "user-list"; // view name
}
```

**ModelAndView** : Combine model data et view name
```java
ModelAndView mav = new ModelAndView("user-list");
mav.addObject("users", userService.findAll());
return mav;
```

### 8. **Form Handling**

- Utilise `@ModelAttribute` pour bind form data
- Validation avec `@Valid` et `BindingResult`
- Spring Form Tags dans JSP : `<form:form>`, `<form:input>`, `<form:errors>`

### 9. **Configuration**

**Java Config** (moderne) :
```java
@Configuration
@EnableWebMvc
@ComponentScan("com.example.app")
public class WebConfig implements WebMvcConfigurer {
    @Bean
    public ViewResolver viewResolver() {
        InternalResourceViewResolver resolver = new InternalResourceViewResolver();
        resolver.setPrefix("/WEB-INF/views/");
        resolver.setSuffix(".jsp");
        return resolver;
    }
}
```

**XML Config** (legacy) : `servlet-context.xml`, `applicationContext.xml`

### 10. **Différences Spring MVC vs Spring Boot**

**Spring MVC** : 
- Framework pour web applications avec server-side rendering
- Configuration manuelle (XML ou Java config)
- Nécessite un servlet container (Tomcat, Jetty)

**Spring Boot** : 
- Framework qui simplifie la configuration de Spring
- Auto-configuration, embedded server
- Peut inclure Spring MVC pour le web layer

---

**Points à retenir pour l'entretien :**
- Spring MVC utilise le **DispatcherServlet** comme front controller
- Architecture en **layers** (Controller → Service → Repository)
- **@SessionAttributes** pour maintenir state entre requests
- **@ResponseBody** pour REST APIs (pas de view rendering)
- **ViewResolver** pour mapper view names aux templates (JSP/Thymeleaf)


---

## Questions/Réponses - Spring Web MVC

### **1. Qu'est-ce que Spring MVC et comment fonctionne-t-il ?**

**Réponse :**
Spring MVC est un framework web basé sur le pattern **Model-View-Controller**. Il utilise le **DispatcherServlet** comme front controller qui reçoit toutes les requêtes HTTP. 

**Flow :** 
1. Request arrive au **DispatcherServlet**
2. **HandlerMapping** trouve le controller approprié
3. Le **Controller** traite la requête (appelle services/repositories)
4. Retourne un **view name** et le **Model**
5. **ViewResolver** trouve le template correspondant (JSP/Thymeleaf)
6. La **View** est rendue et envoyée au client

---

### **2. Quelle est la différence entre @Controller et @RestController ?**

**Réponse :**
- **@Controller** : Utilisé pour les controllers MVC qui retournent des **views** (JSP, Thymeleaf). La return value est interprétée comme un view name.

- **@RestController** : Combinaison de `@Controller` + `@ResponseBody`. Toutes les méthodes retournent directement des données (JSON/XML) dans le HTTP response body. Utilisé pour les **REST APIs**.

```java
@Controller
public String showPage() {
    return "home"; // retourne la vue home.jsp
}

@RestController
public User getUser() {
    return user; // retourne JSON directement
}
```

---

### **3. Expliquez @RequestMapping et ses variantes**

**Réponse :**
**@RequestMapping** : Map une URL à une méthode controller. On peut spécifier la HTTP method, les params, headers, etc.

**Variantes (shortcuts) :**
- `@GetMapping` = `@RequestMapping(method = RequestMethod.GET)`
- `@PostMapping` = `@RequestMapping(method = RequestMethod.POST)`
- `@PutMapping`, `@DeleteMapping`, `@PatchMapping`

```java
@RequestMapping(value = "/users", method = RequestMethod.GET)
// équivalent à
@GetMapping("/users")
```

---

### **4. Différence entre @RequestParam et @PathVariable ?**

**Réponse :**
- **@RequestParam** : Récupère les **query parameters** de l'URL
  ```java
  // URL: /users?id=123
  @GetMapping("/users")
  public String show(@RequestParam("id") int userId) { ... }
  ```

- **@PathVariable** : Récupère les **variables d'URL** (URI template)
  ```java
  // URL: /users/123
  @GetMapping("/users/{id}")
  public String show(@PathVariable("id") int userId) { ... }
  ```

---

### **5. Qu'est-ce que @ModelAttribute et comment l'utiliser ?**

**Réponse :**
**@ModelAttribute** a deux usages principaux :

**1. Binding de form data :**
```java
@PostMapping("/users")
public String create(@ModelAttribute("user") User user) {
    // user object est automatiquement peuplé avec form data
    userService.save(user);
    return "redirect:/users";
}
```

**2. Ajouter des attributs au Model (pour toutes les méthodes) :**
```java
@ModelAttribute("countries")
public List<String> getCountries() {
    return Arrays.asList("France", "USA", "UK");
    // Disponible dans toutes les views de ce controller
}
```

---

### **6. Expliquez @SessionAttributes et donnez un use case**

**Réponse :**
**@SessionAttributes** stocke des attributs du Model dans la **HTTP session** entre plusieurs requests au lieu de les perdre après chaque request.

**Use case :** Multi-step form (wizard)
```java
@Controller
@SessionAttributes("registrationForm")
public class RegistrationController {
    
    @GetMapping("/register/step1")
    public String step1(Model model) {
        model.addAttribute("registrationForm", new RegistrationForm());
        return "step1";
    }
    
    @PostMapping("/register/step2")
    public String step2(@ModelAttribute("registrationForm") RegistrationForm form) {
        // form contient les données de step1 (stockées en session)
        return "step2";
    }
    
    @PostMapping("/register/complete")
    public String complete(@ModelAttribute("registrationForm") RegistrationForm form,
                          SessionStatus status) {
        userService.register(form);
        status.setComplete(); // Nettoie la session
        return "success";
    }
}
```

---

### **7. Qu'est-ce que @ResponseBody et quand l'utiliser ?**

**Réponse :**
**@ResponseBody** indique que la return value d'une méthode doit être écrite **directement dans le HTTP response body**, au lieu d'être interprétée comme un view name.

**Utilisation :** REST APIs qui retournent JSON/XML

```java
@Controller
public class UserController {
    
    @GetMapping("/api/users")
    @ResponseBody
    public List<User> getUsers() {
        return userService.findAll(); // Retourne JSON
    }
    
    @GetMapping("/users")
    public String showUsers(Model model) {
        model.addAttribute("users", userService.findAll());
        return "user-list"; // Retourne JSP view
    }
}
```

---

### **8. Qu'est-ce que le DispatcherServlet ?**

**Réponse :**
Le **DispatcherServlet** est le **Front Controller** de Spring MVC. C'est le point d'entrée unique pour toutes les requêtes HTTP.

**Responsabilités :**
- Reçoit toutes les requests
- Utilise **HandlerMapping** pour trouver le controller approprié
- Délègue au controller
- Utilise **ViewResolver** pour résoudre les view names
- Retourne la response au client

Il est configuré dans `web.xml` (XML config) ou via Java config avec `WebApplicationInitializer`.

---

### **9. Qu'est-ce qu'un ViewResolver ?**

**Réponse :**
Le **ViewResolver** mappe les **view names** retournés par les controllers aux **templates réels** (JSP, Thymeleaf).

**Exemple avec InternalResourceViewResolver (pour JSP) :**
```java
@Bean
public ViewResolver viewResolver() {
    InternalResourceViewResolver resolver = new InternalResourceViewResolver();
    resolver.setPrefix("/WEB-INF/views/");
    resolver.setSuffix(".jsp");
    return resolver;
}

// Controller retourne "home"
// ViewResolver résout vers /WEB-INF/views/home.jsp
```

---

### **10. Différence entre Model, ModelMap et ModelAndView ?**

**Réponse :**
- **Model** : Interface pour ajouter des attributs passés à la view
  ```java
  public String show(Model model) {
      model.addAttribute("user", user);
      return "profile";
  }
  ```

- **ModelMap** : Implémentation de Map, similaire à Model
  ```java
  public String show(ModelMap model) {
      model.addAttribute("user", user);
      return "profile";
  }
  ```

- **ModelAndView** : Contient à la fois le **model data** ET le **view name**
  ```java
  public ModelAndView show() {
      ModelAndView mav = new ModelAndView("profile");
      mav.addObject("user", user);
      return mav;
  }
  ```

**En pratique :** Model/ModelMap sont plus simples. ModelAndView utile quand on veut retourner un objet unique.

---

### **11. Comment gérer les forms avec Spring MVC ?**

**Réponse :**
Spring MVC offre le **data binding** automatique avec `@ModelAttribute`.

**Étapes :**
1. **GET request** : Afficher le form
```java
@GetMapping("/users/new")
public String newUser(Model model) {
    model.addAttribute("user", new User());
    return "user-form";
}
```

2. **View (JSP avec Spring Form Tags) :**
```jsp
<form:form modelAttribute="user" method="post">
    <form:input path="name" />
    <form:input path="email" />
    <button type="submit">Submit</button>
</form:form>
```

3. **POST request** : Traiter le form
```java
@PostMapping("/users")
public String create(@Valid @ModelAttribute("user") User user, 
                     BindingResult result) {
    if (result.hasErrors()) {
        return "user-form";
    }
    userService.save(user);
    return "redirect:/users";
}
```

---

### **12. Quelle est la différence entre forward et redirect ?**

**Réponse :**
- **Forward** : Server-side, même request, URL ne change pas, peut accéder aux request attributes
  ```java
  return "forward:/success";
  ```

- **Redirect** : Client-side, nouvelle request, URL change, perd les request attributes
  ```java
  return "redirect:/users";
  ```

**Usage :** 
- Forward → pour passer des données entre controllers
- Redirect → après POST (pattern PRG - Post-Redirect-Get) pour éviter double submission

---

### **13. Comment gérer les exceptions dans Spring MVC ?**

**Réponse :**
Plusieurs approches :

**1. @ExceptionHandler (au niveau du controller) :**
```java
@Controller
public class UserController {
    
    @ExceptionHandler(UserNotFoundException.class)
    public String handleNotFound() {
        return "error/404";
    }
}
```

**2. @ControllerAdvice (global) :**
```java
@ControllerAdvice
public class GlobalExceptionHandler {
    
    @ExceptionHandler(Exception.class)
    public String handleException(Exception ex, Model model) {
        model.addAttribute("error", ex.getMessage());
        return "error/general";
    }
}
```

**3. @ResponseStatus :**
```java
@ResponseStatus(HttpStatus.NOT_FOUND)
public class UserNotFoundException extends RuntimeException { }
```

---

### **14. Qu'est-ce que l'Interceptor dans Spring MVC ?**

**Réponse :**
Un **HandlerInterceptor** permet d'exécuter du code **avant** et **après** le traitement d'une request par le controller.

**Méthodes :**
- `preHandle()` : Avant le controller (authentification, logging)
- `postHandle()` : Après le controller, avant la view
- `afterCompletion()` : Après la view rendering

```java
public class AuthInterceptor implements HandlerInterceptor {
    @Override
    public boolean preHandle(HttpServletRequest request, 
                            HttpServletResponse response, 
                            Object handler) {
        // Check authentication
        if (!isAuthenticated(request)) {
            response.sendRedirect("/login");
            return false; // Stop la request
        }
        return true; // Continue
    }
}

// Configuration
@Configuration
public class WebConfig implements WebMvcConfigurer {
    @Override
    public void addInterceptors(InterceptorRegistry registry) {
        registry.addInterceptor(new AuthInterceptor())
                .addPathPatterns("/admin/**");
    }
}
```

---

### **15. Différence entre JSP et Thymeleaf ?**

**Réponse :**
**JSP (JavaServer Pages) :**
- Technologie Java traditionnelle
- Syntaxe avec `<% %>`, nécessite JSTL
- Doit être compilé
- Moins naturel pour les designers web

**Thymeleaf :**
- Moteur de templates moderne
- Syntaxe HTML naturelle avec attributs `th:*`
- Peut être ouvert dans un browser (natural templates)
- Meilleure intégration avec Spring Boot
- Support pour HTML5

```jsp
<!-- JSP -->
<c:forEach items="${users}" var="user">
    <p>${user.name}</p>
</c:forEach>
```

```html
<!-- Thymeleaf -->
<p th:each="user : ${users}" th:text="${user.name}"></p>
```

---

### **16. Comment configurer Spring MVC sans XML (Java Config) ?**

**Réponse :**
Utiliser `@Configuration`, `@EnableWebMvc`, et `WebMvcConfigurer` :

```java
@Configuration
@EnableWebMvc
@ComponentScan("com.example.app")
public class WebConfig implements WebMvcConfigurer {
    
    @Bean
    public ViewResolver viewResolver() {
        InternalResourceViewResolver resolver = new InternalResourceViewResolver();
        resolver.setPrefix("/WEB-INF/views/");
        resolver.setSuffix(".jsp");
        return resolver;
    }
    
    @Override
    public void addResourceHandlers(ResourceHandlerRegistry registry) {
        registry.addResourceHandler("/static/**")
                .addResourceLocations("/resources/");
    }
}

// Remplace web.xml
public class WebInitializer extends AbstractAnnotationConfigDispatcherServletInitializer {
    @Override
    protected Class<?>[] getRootConfigClasses() {
        return null;
    }
    
    @Override
    protected Class<?>[] getServletConfigClasses() {
        return new Class[] { WebConfig.class };
    }
    
    @Override
    protected String[] getServletMappings() {
        return new String[] { "/" };
    }
}
```

---

### **17. Qu'est-ce que le Content Negotiation ?**

**Réponse :**
Le **Content Negotiation** permet au même endpoint de retourner différents formats (JSON, XML, HTML) selon le header `Accept` de la request.

```java
@GetMapping(value = "/users/{id}", 
            produces = {"application/json", "application/xml"})
@ResponseBody
public User getUser(@PathVariable int id) {
    return userService.findById(id);
}

// Request avec Accept: application/json → retourne JSON
// Request avec Accept: application/xml → retourne XML
```

---

### **18. Comment valider les données de form ?**

**Réponse :**
Utiliser **Bean Validation** (`@Valid`) avec **BindingResult** :

```java
// Entity
public class User {
    @NotBlank(message = "Name is required")
    private String name;
    
    @Email(message = "Invalid email")
    private String email;
    
    @Min(value = 18, message = "Must be 18+")
    private int age;
}

// Controller
@PostMapping("/users")
public String create(@Valid @ModelAttribute("user") User user,
                     BindingResult result) {
    if (result.hasErrors()) {
        return "user-form"; // Retourne au form avec erreurs
    }
    userService.save(user);
    return "redirect:/users";
}

// JSP (afficher erreurs)
<form:errors path="name" cssClass="error" />
```

---

### **19. Quelle est l'architecture en layers typique d'une application Spring MVC ?**

**Réponse :**
**4 layers principaux :**

1. **Presentation Layer (Controller)** : Gère les HTTP requests/responses
2. **Service Layer** : Business logic, transactions
3. **Repository/DAO Layer** : Data access
4. **Domain/Model Layer** : Entities, DTOs

**Flow :** 
Client → Controller → Service → Repository → Database
Database → Repository → Service → Controller → View → Client

**Avantages :** Separation of concerns, testabilité, maintenabilité

---

### **20. Comment gérer les static resources (CSS, JS, images) ?**

**Réponse :**
Configurer les **resource handlers** :

```java
@Configuration
@EnableWebMvc
public class WebConfig implements WebMvcConfigurer {
    
    @Override
    public void addResourceHandlers(ResourceHandlerRegistry registry) {
        registry.addResourceHandler("/css/**")
                .addResourceLocations("/resources/css/");
        
        registry.addResourceHandler("/js/**")
                .addResourceLocations("/resources/js/");
        
        registry.addResourceHandler("/images/**")
                .addResourceLocations("/resources/images/");
    }
}

// Dans JSP
<link rel="stylesheet" href="${pageContext.request.contextPath}/css/style.css">
```

---

**Conseil final :** Prépare-toi à coder un petit exemple de controller avec form handling lors de l'entretien. C'est une question pratique très courante ! 💪

Bonne chance ! 🚀