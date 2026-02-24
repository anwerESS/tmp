## [**..**](./00_index.md)

# **REST API Documentation & Standards with Spring Boot**

---

### **1. Swagger & Open API Specification**


**Technical implementation :**
- Configuration of **SpringFox** or **SpringDoc OpenAPI** to generate the OpenAPI specification
- Customizing Spring Security to allow public access to Swagger resources :

```java
.antMatchers("/v2/api-docs/**").permitAll()
.antMatchers("/swagger-ui/**").permitAll()
.antMatchers("/swagger-resources/**").permitAll()
.antMatchers("/swagger-ui.html").permitAll()
.antMatchers("/webjars/**").permitAll()
```

**Benefits :**
- Documentation always in sync with the code
- Intuitive UI accessible at `/swagger-ui.html`
- Allows frontend teams and external partners to quickly understand endpoints, parameters, and response models

**Concrete example :**
For a `GET /api/users/{id}` endpoint, Swagger automatically generates the description, required parameters, HTTP response codes (200, 404, 500), and a sample JSON payload.

---

### **2. HATEOAS (Hypermedia As The Engine Of Application State)**

**Concept :**
HATEOAS is a RESTful constraint that enriches API responses with **hypermedia links**, allowing clients to dynamically discover available actions without hardcoding URLs.

**Why use HATEOAS ?**
- **Self-descriptive API** : The client discovers relationships and possible actions directly from the response
- **Reduced dependency** on external documentation
- **Evolvability** : URLs can be changed on the server side without breaking clients

**Implementation with Spring HATEOAS :**
```java
EntityModel<User> resource = EntityModel.of(user);
resource.add(linkTo(methodOn(UserController.class).getUser(id)).withSelfRel());
resource.add(linkTo(methodOn(UserController.class).getAllUsers()).withRel("all-users"));
```

**Example JSON response with HATEOAS :**
```json
{
  "id": 1,
  "name": "John Doe",
  "email": "john@example.com",
  "_links": {
    "self": {
      "href": "http://localhost:8080/api/users/1"
    },
    "all-users": {
      "href": "http://localhost:8080/api/users"
    },
    "orders": {
      "href": "http://localhost:8080/api/users/1/orders"
    }
  }
}
```

The client can now navigate to the user's orders via the `orders` link without knowing the URL structure in advance.

---

### **3. HAL Explorer**

**Usage :**
I enabled **HAL Explorer** (HAL = Hypertext Application Language), a built-in browser that allows visually exploring the API resources and their relationships.

**Configuration :**
```xml
<dependency>
    <groupId>org.springframework.data</groupId>
    <artifactId>spring-data-rest-hal-explorer</artifactId>
</dependency>
```

**Access :**
Via `/browser/index.html`, you can navigate the API like a file explorer by following HATEOAS links.

**Advantage :**
Very useful during development to quickly test relationships between resources without using Postman.

---

### **4. Internationalization (i18n)**

**Context :**
To support multilingual users, I implemented internationalization using the **HTTP `Accept-Language` header**.

**Principle :**
- The client sends its language preference via the header : `Accept-Language: fr-FR` or `Accept-Language: en-US`
- The server returns messages, errors, and content in the appropriate language

**Spring Boot configuration :**
```java
@Bean
public LocaleResolver localeResolver() {
    AcceptHeaderLocaleResolver resolver = new AcceptHeaderLocaleResolver();
    resolver.setDefaultLocale(Locale.US);
    return resolver;
}

@Bean
public ResourceBundleMessageSource messageSource() {
    ResourceBundleMessageSource source = new ResourceBundleMessageSource();
    source.setBasename("messages");
    source.setDefaultEncoding("UTF-8");
    return source;
}
```

**Resource files :**
- `messages_en.properties` : `greeting=Good Morning`
- `messages_fr.properties` : `greeting=Bonjour`
- `messages_nl.properties` : `greeting=Goedemorgen`

**Usage in code :**
```java
@Autowired
private MessageSource messageSource;

@GetMapping("/greeting")
public String getGreeting(@RequestHeader("Accept-Language") Locale locale) {
    return messageSource.getMessage("greeting", null, locale);
}
```

**Example request :**
```
GET /api/greeting
Accept-Language: nl
```
**Response :** `Goedemorgen`

**Use cases :**
- Multilingual error messages : `"User not found"` → `"Utilisateur introuvable"`
- Custom validation messages per locale
- Internationalized business content

---

### **Summary of mastered technologies**

✅ **Swagger/OpenAPI** : Automatic and interactive documentation  
✅ **HATEOAS** : Self-descriptive APIs with hypermedia links  
✅ **HAL Explorer** : Visual navigation of API resources  
✅ **i18n** : Multilingual support via Accept-Language header  

These practices ensure APIs that are **maintainable**, **evolvable**, and **compliant with REST standards** at level 3 of the Richardson Maturity Model.

---

## **Questions/Answers : Swagger & Open API Specification**

**Q1 : What is the difference between Swagger and OpenAPI ?**

**A :** OpenAPI Specification is the **official standard** for describing REST APIs (formerly called Swagger Specification). Swagger refers to the **tools** developed by SmartBear to implement this specification : Swagger UI for interactive documentation, Swagger Editor to edit specs, and Swagger Codegen to generate client/server code.

**Q2 : How did you configure Swagger in Spring Boot ?**

**A :** I used **SpringDoc OpenAPI** (or SpringFox for older versions). Maven configuration :
```xml
<dependency>
    <groupId>org.springdoc</groupId>
    <artifactId>springdoc-openapi-ui</artifactId>
</dependency>
```
Then I customized the configuration :
```java
@Configuration
public class OpenApiConfig {
    @Bean
    public OpenAPI customOpenAPI() {
        return new OpenAPI()
            .info(new Info()
                .title("User Management API")
                .version("1.0")
                .description("API for managing users"));
    }
}
```

**Q3 : Why did you add Swagger endpoints to permitAll() in Spring Security ?**

**A :** Without this configuration, the Swagger endpoints would be protected by authentication and inaccessible. By adding `.permitAll()` on `/swagger-ui/**`, `/v2/api-docs/**`, etc., I allow developers and partners to access the documentation without credentials, making it easier to explore the API.

**Q4 : How do you customize the documentation of an endpoint with Swagger ?**

**A :** I use the annotations :
```java
@Operation(summary = "Get user by ID", description = "Returns a single user")
@ApiResponses(value = {
    @ApiResponse(responseCode = "200", description = "User found"),
    @ApiResponse(responseCode = "404", description = "User not found")
})
@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(
    @Parameter(description = "ID of user to retrieve") 
    @PathVariable Long id
) {
    // ...
}
```

---

## **Questions/Answers : HATEOAS**

**Q5 : What is HATEOAS and why use it ?**

**A :** HATEOAS (Hypermedia As The Engine Of Application State) is a REST constraint that consists of including **hypermedia links** in responses to guide the client toward available actions.

**Advantages :**
- The API becomes **self-descriptive** : the client discovers relationships dynamically
- **Less coupling** : URLs can be changed on the server without impacting clients
- Compliance with **level 3 of the Richardson Maturity Model**

**Q6 : How do you implement HATEOAS with Spring ?**

**A :** I use **Spring HATEOAS** and the `EntityModel` class :
```java
@GetMapping("/users/{id}")
public EntityModel<User> getUser(@PathVariable Long id) {
    User user = userService.findById(id);
    
    EntityModel<User> resource = EntityModel.of(user);
    resource.add(linkTo(methodOn(UserController.class).getUser(id)).withSelfRel());
    resource.add(linkTo(methodOn(UserController.class).getAllUsers()).withRel("all-users"));
    resource.add(linkTo(methodOn(OrderController.class).getUserOrders(id)).withRel("orders"));
    
    return resource;
}
```

**Q7 : Give an example of a JSON response with HATEOAS**

**A :**
```json
{
  "id": 1,
  "name": "John Doe",
  "email": "john@example.com",
  "_links": {
    "self": {
      "href": "http://localhost:8080/api/users/1"
    },
    "all-users": {
      "href": "http://localhost:8080/api/users"
    },
    "orders": {
      "href": "http://localhost:8080/api/users/1/orders"
    }
  }
}
```
The client can follow the `orders` link to retrieve orders without knowing the URL structure.

**Q8 : What is the difference between withSelfRel() and withRel() ?**

**A :** 
- **withSelfRel()** : creates a link with the `"self"` relation, pointing to the current resource
- **withRel("custom-name")** : creates a link with a custom relation to navigate to another related resource

---

## **Questions/Answers : HAL & HAL Explorer**

**Q9 : What is HAL (Hypertext Application Language) ?**

**A :** HAL is a **JSON/XML representation standard** for REST APIs with HATEOAS. It defines a conventional structure for links (`_links`) and embedded resources (`_embedded`). Spring HATEOAS uses HAL by default to format responses.

**Q10 : How do you use HAL Explorer ?**

**A :** HAL Explorer is a built-in **web browser** for visually exploring APIs. I add it via the dependency :
```xml
<dependency>
    <groupId>org.springframework.data</groupId>
    <artifactId>spring-data-rest-hal-explorer</artifactId>
</dependency>
```
Accessible via `/browser/index.html`, it allows navigating the API by clicking on HATEOAS links — very useful during development to test relationships between resources.

**Q11 : What is the difference between HAL Explorer and Swagger UI ?**

**A :** 
- **Swagger UI** : static documentation generated from annotations, allows testing endpoints individually
- **HAL Explorer** : dynamic navigation based on the hypermedia links returned by the API, follows the HATEOAS principle

Both are complementary : Swagger for complete documentation, HAL Explorer for exploring relationships.

---

## **Questions/Answers : Internationalization (i18n)**

**Q12 : How does internationalization work with Accept-Language ?**

**A :** The client sends its language preference via the **HTTP `Accept-Language` header** :
```
GET /api/greeting
Accept-Language: fr-FR
```
Spring Boot uses a `LocaleResolver` to extract the locale and a `MessageSource` to load the corresponding messages from properties files (`messages_fr.properties`, `messages_en.properties`).

**Q13 : How do you configure i18n in Spring Boot ?**

**A :** Configuration of `LocaleResolver` and `MessageSource` :
```java
@Bean
public LocaleResolver localeResolver() {
    AcceptHeaderLocaleResolver resolver = new AcceptHeaderLocaleResolver();
    resolver.setDefaultLocale(Locale.US);
    return resolver;
}

@Bean
public ResourceBundleMessageSource messageSource() {
    ResourceBundleMessageSource source = new ResourceBundleMessageSource();
    source.setBasename("messages");
    source.setDefaultEncoding("UTF-8");
    return source;
}
```

**Q14 : How do you organize your multilingual resource files ?**

**A :** I create several properties files in `src/main/resources` :
- `messages.properties` (fallback)
- `messages_en.properties` : `user.not.found=User not found`
- `messages_fr.properties` : `user.not.found=Utilisateur introuvable`
- `messages_nl.properties` : `user.not.found=Gebruiker niet gevonden`

**Q15 : How do you retrieve an internationalized message in code ?**

**A :** I inject `MessageSource` and use the locale extracted from the header :
```java
@Autowired
private MessageSource messageSource;

@GetMapping("/users/{id}")
public ResponseEntity<User> getUser(@PathVariable Long id, Locale locale) {
    User user = userService.findById(id);
    if (user == null) {
        String message = messageSource.getMessage("user.not.found", null, locale);
        throw new UserNotFoundException(message);
    }
    return ResponseEntity.ok(user);
}
```

**Q16 : How do you handle internationalization of validation messages ?**

**A :** I use Bean Validation annotations with message keys :
```java
public class UserDTO {
    @NotBlank(message = "{user.name.required}")
    private String name;
    
    @Email(message = "{user.email.invalid}")
    private String email;
}
```

Properties files :
- `messages_en.properties` : `user.name.required=Name is required`
- `messages_fr.properties` : `user.name.required=Le nom est obligatoire`

**Q17 : What is the difference between Accept-Language and a ?lang=fr parameter ?**

**A :** 
- **Accept-Language (header)** : HTTP standard, respects REST principles, handled automatically by browsers
- **?lang=fr (query parameter)** : non-standard, pollutes the URL, requires manual configuration

I always prefer `Accept-Language` because it is the **REST best practice**.

---

## **Cross-cutting Questions**

**Q18 : How do these technologies integrate together in a real project ?**

**A :** In my projects :
1. **Swagger** documents all endpoints with their parameters and responses
2. **HATEOAS** enriches responses with navigation links
3. **HAL Explorer** allows visually exploring these links during development
4. **i18n** internationalizes error messages and business content

Example : a `GET /users/1` endpoint returns a user with HATEOAS links, error messages are translated according to `Accept-Language`, and everything is documented in Swagger.

**Q19 : What are the levels of the Richardson Maturity Model ?**

**A :** 
- **Level 0** : HTTP as transport (SOAP)
- **Level 1** : Introduction of resources (`/users`, `/orders`)
- **Level 2** : Correct use of HTTP verbs (GET, POST, PUT, DELETE) and status codes
- **Level 3** : HATEOAS - hypermedia controls

With HATEOAS, we reach **level 3**, the most mature.

**Q20 : How do you test an API with HATEOAS and i18n ?**

**A :** 
```java
@Test
public void testGetUserWithLinks() {
    mockMvc.perform(get("/api/users/1"))
        .andExpect(status().isOk())
        .andExpect(jsonPath("$._links.self.href").exists())
        .andExpect(jsonPath("$._links.orders.href").exists());
}

@Test
public void testI18n() {
    mockMvc.perform(get("/api/users/999")
            .header("Accept-Language", "fr"))
        .andExpect(status().isNotFound())
        .andExpect(content().string(containsString("Utilisateur introuvable")));
}
```

---
