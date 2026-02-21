## [**..**](./00_index.md)

## Spring Web MVC - Complete Guide

### 1. **Project Structure**

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
│   └── web.xml            # Deployment descriptor (optional with Java config)
│
src/main/resources/
├── templates/             # Thymeleaf templates
├── static/                # CSS, JS, images
└── application.properties
```

### 2. **The Layers**

**Controller Layer** :
- Handles incoming HTTP requests
- Annotations : `@Controller`, `@RequestMapping`, `@GetMapping`, `@PostMapping`
- Returns the **view** name or data with `@ResponseBody`

**Service Layer** :
- Contains the business logic
- Annotation : `@Service`
- Called by the controller, calls the repository

**Repository/DAO Layer** :
- Handles data access (database)
- Annotations : `@Repository` or Spring Data JPA
- Interacts with the database

**Model Layer** :
- Domain objects (entities, DTOs, POJOs)
- Represents the application data

### 3. **The MVC Pattern**

**Model** : Data transferred between controller and view (via `Model` or `ModelAndView`)

**View** : Templates (JSP, Thymeleaf) that display the data

**Controller** : Receives requests, processes them via services, returns the appropriate view

### 4. **DispatcherServlet (Front Controller)**

- Single entry point for all HTTP requests
- Routes requests to the appropriate controllers
- Flow : Request → DispatcherServlet → HandlerMapping → Controller → ViewResolver → View

### 5. **View Technologies**

**JSP (JavaServer Pages)** :
- Traditional Java templates with `<% %>` syntax
- Uses JSTL (JSP Standard Tag Library) : `<c:forEach>`, `<c:if>`
- `ViewResolver` configuration to resolve view names

**Thymeleaf** :
- Modern template engine, natural HTML syntax
- Attributes : `th:text`, `th:href`, `th:each`, `th:if`
- Better integration with Spring Boot

### 6. **Key Annotations**

**@Controller** : Marks a class as an MVC controller

**@RequestMapping** : Maps a URL to a method (or class)
```java
@RequestMapping(value = "/users", method = RequestMethod.GET)
```

**@GetMapping / @PostMapping** : Shortcuts for GET/POST requests

**@RequestParam** : Gets query parameters
```java
public String show(@RequestParam("id") int userId)
```

**@PathVariable** : Gets URL variables
```java
@GetMapping("/users/{id}")
public String show(@PathVariable("id") int userId)
```

**@ModelAttribute** : 
- Binds form data to an object
- Adds attributes to the Model available for all methods

**@SessionAttributes** : 
- Stores Model attributes in the HTTP session across multiple requests
- Useful for **multi-step forms**, **wizards**, or keeping user state
```java
@Controller
@SessionAttributes("user")
public class UserController { ... }
```

**@ResponseBody** : 
- The return value is written directly into the HTTP response body
- Not interpreted as a view name
- Used for **REST APIs** (returns JSON/XML)
- Combined with `@RestController` = `@Controller` + `@ResponseBody` on all methods

### 7. **Model and Data Binding**

**Model** : Interface for passing data to the view
```java
@GetMapping("/users")
public String list(Model model) {
    model.addAttribute("users", userService.findAll());
    return "user-list"; // view name
}
```

**ModelAndView** : Combines model data and view name
```java
ModelAndView mav = new ModelAndView("user-list");
mav.addObject("users", userService.findAll());
return mav;
```

### 8. **Form Handling**

- Uses `@ModelAttribute` to bind form data
- Validation with `@Valid` and `BindingResult`
- Spring Form Tags in JSP : `<form:form>`, `<form:input>`, `<form:errors>`

### 9. **Configuration**

**Java Config** (modern) :
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

### 10. **Differences: Spring MVC vs Spring Boot**

**Spring MVC** : 
- Framework for web applications with server-side rendering
- Manual configuration (XML or Java config)
- Requires a servlet container (Tomcat, Jetty)

**Spring Boot** : 
- Framework that simplifies Spring configuration
- Auto-configuration, embedded server
- Can include Spring MVC for the web layer

---

**Key points to remember for interviews :**
- Spring MVC uses the **DispatcherServlet** as front controller
- **Layered** architecture (Controller → Service → Repository)
- **@SessionAttributes** to keep state between requests
- **@ResponseBody** for REST APIs (no view rendering)
- **ViewResolver** to map view names to templates (JSP/Thymeleaf)


---

## Questions/Answers - Spring Web MVC

### **1. What is Spring MVC and how does it work ?**

**Answer :**
Spring MVC is a web framework based on the **Model-View-Controller** pattern. It uses the **DispatcherServlet** as a front controller that receives all HTTP requests.

**Flow :** 
1. Request arrives at the **DispatcherServlet**
2. **HandlerMapping** finds the appropriate controller
3. The **Controller** processes the request (calls services/repositories)
4. Returns a **view name** and the **Model**
5. **ViewResolver** finds the corresponding template (JSP/Thymeleaf)
6. The **View** is rendered and sent to the client

---

### **2. What is the difference between @Controller and @RestController ?**

**Answer :**
- **@Controller** : Used for MVC controllers that return **views** (JSP, Thymeleaf). The return value is interpreted as a view name.

- **@RestController** : Combination of `@Controller` + `@ResponseBody`. All methods return data (JSON/XML) directly in the HTTP response body. Used for **REST APIs**.

```java
@Controller
public String showPage() {
    return "home"; // returns the home.jsp view
}

@RestController
public User getUser() {
    return user; // returns JSON directly
}
```

---

### **3. Explain @RequestMapping and its variants**

**Answer :**
**@RequestMapping** : Maps a URL to a controller method. You can specify the HTTP method, params, headers, etc.

**Variants (shortcuts) :**
- `@GetMapping` = `@RequestMapping(method = RequestMethod.GET)`
- `@PostMapping` = `@RequestMapping(method = RequestMethod.POST)`
- `@PutMapping`, `@DeleteMapping`, `@PatchMapping`

```java
@RequestMapping(value = "/users", method = RequestMethod.GET)
// equivalent to
@GetMapping("/users")
```

---

### **4. Difference between @RequestParam and @PathVariable ?**

**Answer :**
- **@RequestParam** : Gets **query parameters** from the URL
  ```java
  // URL: /users?id=123
  @GetMapping("/users")
  public String show(@RequestParam("id") int userId) { ... }
  ```

- **@PathVariable** : Gets **URL variables** (URI template)
  ```java
  // URL: /users/123
  @GetMapping("/users/{id}")
  public String show(@PathVariable("id") int userId) { ... }
  ```

---

### **5. What is @ModelAttribute and how to use it ?**

**Answer :**
**@ModelAttribute** has two main uses :

**1. Form data binding :**
```java
@PostMapping("/users")
public String create(@ModelAttribute("user") User user) {
    // user object is automatically populated with form data
    userService.save(user);
    return "redirect:/users";
}
```

**2. Adding attributes to the Model (for all methods) :**
```java
@ModelAttribute("countries")
public List<String> getCountries() {
    return Arrays.asList("France", "USA", "UK");
    // Available in all views of this controller
}
```

---

### **6. Explain @SessionAttributes and give a use case**

**Answer :**
**@SessionAttributes** stores Model attributes in the **HTTP session** across multiple requests instead of losing them after each request.

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
        // form contains data from step1 (stored in session)
        return "step2";
    }
    
    @PostMapping("/register/complete")
    public String complete(@ModelAttribute("registrationForm") RegistrationForm form,
                          SessionStatus status) {
        userService.register(form);
        status.setComplete(); // Clears the session
        return "success";
    }
}
```

---

### **7. What is @ResponseBody and when to use it ?**

**Answer :**
**@ResponseBody** indicates that the return value of a method must be written **directly into the HTTP response body**, instead of being interpreted as a view name.

**Usage :** REST APIs that return JSON/XML

```java
@Controller
public class UserController {
    
    @GetMapping("/api/users")
    @ResponseBody
    public List<User> getUsers() {
        return userService.findAll(); // Returns JSON
    }
    
    @GetMapping("/users")
    public String showUsers(Model model) {
        model.addAttribute("users", userService.findAll());
        return "user-list"; // Returns JSP view
    }
}
```

---

### **8. What is the DispatcherServlet ?**

**Answer :**
The **DispatcherServlet** is the **Front Controller** of Spring MVC. It is the single entry point for all HTTP requests.

**Responsibilities :**
- Receives all requests
- Uses **HandlerMapping** to find the appropriate controller
- Delegates to the controller
- Uses **ViewResolver** to resolve view names
- Returns the response to the client

It is configured in `web.xml` (XML config) or via Java config with `WebApplicationInitializer`.

---

### **9. What is a ViewResolver ?**

**Answer :**
The **ViewResolver** maps the **view names** returned by controllers to the **actual templates** (JSP, Thymeleaf).

**Example with InternalResourceViewResolver (for JSP) :**
```java
@Bean
public ViewResolver viewResolver() {
    InternalResourceViewResolver resolver = new InternalResourceViewResolver();
    resolver.setPrefix("/WEB-INF/views/");
    resolver.setSuffix(".jsp");
    return resolver;
}

// Controller returns "home"
// ViewResolver resolves to /WEB-INF/views/home.jsp
```

---

### **10. Difference between Model, ModelMap and ModelAndView ?**

**Answer :**
- **Model** : Interface to add attributes passed to the view
  ```java
  public String show(Model model) {
      model.addAttribute("user", user);
      return "profile";
  }
  ```

- **ModelMap** : Map implementation, similar to Model
  ```java
  public String show(ModelMap model) {
      model.addAttribute("user", user);
      return "profile";
  }
  ```

- **ModelAndView** : Contains both the **model data** AND the **view name**
  ```java
  public ModelAndView show() {
      ModelAndView mav = new ModelAndView("profile");
      mav.addObject("user", user);
      return mav;
  }
  ```

**In practice :** Model/ModelMap are simpler. ModelAndView is useful when you want to return a single object.

---

### **11. How to handle forms with Spring MVC ?**

**Answer :**
Spring MVC provides automatic **data binding** with `@ModelAttribute`.

**Steps :**
1. **GET request** : Display the form
```java
@GetMapping("/users/new")
public String newUser(Model model) {
    model.addAttribute("user", new User());
    return "user-form";
}
```

2. **View (JSP with Spring Form Tags) :**
```jsp
<form:form modelAttribute="user" method="post">
    <form:input path="name" />
    <form:input path="email" />
    <button type="submit">Submit</button>
</form:form>
```

3. **POST request** : Process the form
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

### **12. What is the difference between forward and redirect ?**

**Answer :**
- **Forward** : Server-side, same request, URL does not change, can access request attributes
  ```java
  return "forward:/success";
  ```

- **Redirect** : Client-side, new request, URL changes, loses request attributes
  ```java
  return "redirect:/users";
  ```

**Usage :** 
- Forward → to pass data between controllers
- Redirect → after POST (PRG pattern - Post-Redirect-Get) to avoid double submission

---

### **13. How to handle exceptions in Spring MVC ?**

**Answer :**
Several approaches :

**1. @ExceptionHandler (at controller level) :**
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

### **14. What is an Interceptor in Spring MVC ?**

**Answer :**
A **HandlerInterceptor** allows you to run code **before** and **after** a request is processed by the controller.

**Methods :**
- `preHandle()` : Before the controller (authentication, logging)
- `postHandle()` : After the controller, before the view
- `afterCompletion()` : After view rendering

```java
public class AuthInterceptor implements HandlerInterceptor {
    @Override
    public boolean preHandle(HttpServletRequest request, 
                            HttpServletResponse response, 
                            Object handler) {
        // Check authentication
        if (!isAuthenticated(request)) {
            response.sendRedirect("/login");
            return false; // Stop the request
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

### **15. Difference between JSP and Thymeleaf ?**

**Answer :**
**JSP (JavaServer Pages) :**
- Traditional Java technology
- Syntax with `<% %>`, requires JSTL
- Must be compiled
- Less natural for web designers

**Thymeleaf :**
- Modern template engine
- Natural HTML syntax with `th:*` attributes
- Can be opened in a browser (natural templates)
- Better integration with Spring Boot
- HTML5 support

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

### **16. How to configure Spring MVC without XML (Java Config) ?**

**Answer :**
Use `@Configuration`, `@EnableWebMvc`, and `WebMvcConfigurer` :

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

// Replaces web.xml
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

### **17. What is Content Negotiation ?**

**Answer :**
**Content Negotiation** allows the same endpoint to return different formats (JSON, XML, HTML) depending on the `Accept` header of the request.

```java
@GetMapping(value = "/users/{id}", 
            produces = {"application/json", "application/xml"})
@ResponseBody
public User getUser(@PathVariable int id) {
    return userService.findById(id);
}

// Request with Accept: application/json → returns JSON
// Request with Accept: application/xml → returns XML
```

---

### **18. How to validate form data ?**

**Answer :**
Use **Bean Validation** (`@Valid`) with **BindingResult** :

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
        return "user-form"; // Returns to the form with errors
    }
    userService.save(user);
    return "redirect:/users";
}

// JSP (display errors)
<form:errors path="name" cssClass="error" />
```

---

### **19. What is the typical layered architecture of a Spring MVC application ?**

**Answer :**
**4 main layers :**

1. **Presentation Layer (Controller)** : Handles HTTP requests/responses
2. **Service Layer** : Business logic, transactions
3. **Repository/DAO Layer** : Data access
4. **Domain/Model Layer** : Entities, DTOs

**Flow :** 
Client → Controller → Service → Repository → Database
Database → Repository → Service → Controller → View → Client

**Advantages :** Separation of concerns, testability, maintainability

---

### **20. How to handle static resources (CSS, JS, images) ?**

**Answer :**
Configure the **resource handlers** :

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

// In JSP
<link rel="stylesheet" href="${pageContext.request.contextPath}/css/style.css">
```

---

**Final tip :** Be ready to code a small controller example with form handling during the interview. It's a very common practical question ! 💪

Good luck ! 🚀
