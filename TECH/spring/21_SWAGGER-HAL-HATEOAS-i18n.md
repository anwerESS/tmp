## [**..**](./00_index.md)

# **Documentation et Standards REST API avec Spring Boot**

---

### **1. Swagger & Open API Specification**


**Implémentation technique :**
- Configuration de **SpringFox** ou **SpringDoc OpenAPI** pour générer la spécification OpenAPI
- Personnalisation de Spring Security pour autoriser l'accès public aux ressources Swagger :

```java
.antMatchers("/v2/api-docs/**").permitAll()
.antMatchers("/swagger-ui/**").permitAll()
.antMatchers("/swagger-resources/**").permitAll()
.antMatchers("/swagger-ui.html").permitAll()
.antMatchers("/webjars/**").permitAll()
```

**Bénéfices :**
- Documentation toujours synchronisée avec le code
- Interface UI intuitive accessible via `/swagger-ui.html`
- Permet aux équipes frontend et partenaires externes de comprendre rapidement les endpoints, paramètres et response models

**Exemple concret :**
Pour un endpoint `GET /api/users/{id}`, Swagger génère automatiquement la description, les paramètres requis, les codes HTTP de réponse (200, 404, 500) et un exemple de payload JSON.

---

### **2. HATEOAS (Hypermedia As The Engine Of Application State)**

**Concept :**
HATEOAS est une contrainte RESTful qui enrichit les réponses API avec des **hypermedia links**, permettant aux clients de découvrir dynamiquement les actions disponibles sans hardcoder les URLs.

**Pourquoi utiliser HATEOAS ?**
- **Self-descriptive API** : Le client découvre les relations et actions possibles directement depuis la réponse
- **Réduction de la dépendance** à la documentation externe
- **Évolutivité** : On peut modifier les URLs côté serveur sans casser les clients

**Implémentation avec Spring HATEOAS :**
```java
EntityModel<User> resource = EntityModel.of(user);
resource.add(linkTo(methodOn(UserController.class).getUser(id)).withSelfRel());
resource.add(linkTo(methodOn(UserController.class).getAllUsers()).withRel("all-users"));
```

**Exemple de réponse JSON avec HATEOAS :**
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

Le client peut maintenant naviguer vers les commandes de l'utilisateur via le link `orders` sans connaître à l'avance la structure de l'URL.

---

### **3. HAL Explorer**

**Utilisation :**
J'ai activé **HAL Explorer** (HAL = Hypertext Application Language), un browser intégré qui permet d'explorer visuellement les ressources de l'API et leurs relations.

**Configuration :**
```xml
<dependency>
    <groupId>org.springframework.data</groupId>
    <artifactId>spring-data-rest-hal-explorer</artifactId>
</dependency>
```

**Accès :**
Via `/browser/index.html`, on peut naviguer dans l'API comme dans un explorateur de fichiers, en suivant les links HATEOAS.

**Avantage :**
Très utile en phase de développement pour tester rapidement les relations entre ressources sans utiliser Postman.

---

### **4. Internationalization (i18n)**

**Contexte :**
Pour supporter des utilisateurs multilingues, j'ai implémenté l'internationalisation en utilisant le **HTTP Header `Accept-Language`**.

**Principe :**
- Le client envoie sa préférence linguistique via le header : `Accept-Language: fr-FR` ou `Accept-Language: en-US`
- Le serveur retourne les messages, erreurs et contenus dans la langue appropriée

**Configuration Spring Boot :**
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

**Fichiers de ressources :**
- `messages_en.properties` : `greeting=Good Morning`
- `messages_fr.properties` : `greeting=Bonjour`
- `messages_nl.properties` : `greeting=Goedemorgen`

**Utilisation dans le code :**
```java
@Autowired
private MessageSource messageSource;

@GetMapping("/greeting")
public String getGreeting(@RequestHeader("Accept-Language") Locale locale) {
    return messageSource.getMessage("greeting", null, locale);
}
```

**Exemple de requête :**
```
GET /api/greeting
Accept-Language: nl
```
**Réponse :** `Goedemorgen`

**Cas d'usage :**
- Messages d'erreur multilingues : `"User not found"` → `"Utilisateur introuvable"`
- Validation messages personnalisés par locale
- Contenus métier internationalisés

---

### **Synthèse des technologies maîtrisées**

✅ **Swagger/OpenAPI** : Documentation automatique et interactive  
✅ **HATEOAS** : APIs self-descriptive avec hypermedia links  
✅ **HAL Explorer** : Navigation visuelle des ressources API  
✅ **i18n** : Support multilingue via Accept-Language header  

Ces pratiques garantissent des APIs **maintenables**, **évolutives** et **conformes aux standards REST** de niveau 3 du Richardson Maturity Model.

---

## **Questions/Réponses : Swagger & Open API Specification**

**Q1 : Quelle est la différence entre Swagger et OpenAPI ?**

**R :** OpenAPI Specification est le **standard officiel** pour décrire les APIs REST (anciennement appelé Swagger Specification). Swagger fait référence aux **outils** développés par SmartBear pour implémenter cette spécification : Swagger UI pour la documentation interactive, Swagger Editor pour éditer les specs, et Swagger Codegen pour générer du code client/serveur.

**Q2 : Comment avez-vous configuré Swagger dans Spring Boot ?**

**R :** J'ai utilisé **SpringDoc OpenAPI** (ou SpringFox pour les anciennes versions). Configuration Maven :
```xml
<dependency>
    <groupId>org.springdoc</groupId>
    <artifactId>springdoc-openapi-ui</artifactId>
</dependency>
```
Puis j'ai personnalisé la configuration :
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

**Q3 : Pourquoi avez-vous ajouté les endpoints Swagger dans permitAll() de Spring Security ?**

**R :** Sans cette configuration, les endpoints Swagger seraient protégés par l'authentification et inaccessibles. En ajoutant `.permitAll()` sur `/swagger-ui/**`, `/v2/api-docs/**`, etc., je permets aux développeurs et partenaires d'accéder à la documentation sans credentials, ce qui facilite l'exploration de l'API.

**Q4 : Comment personnalisez-vous la documentation d'un endpoint avec Swagger ?**

**R :** J'utilise les annotations :
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

## **Questions/Réponses : HATEOAS**

**Q5 : Qu'est-ce que HATEOAS et pourquoi l'utiliser ?**

**R :** HATEOAS (Hypermedia As The Engine Of Application State) est une contrainte REST qui consiste à inclure des **hypermedia links** dans les réponses pour guider le client vers les actions disponibles. 

**Avantages :**
- L'API devient **self-descriptive** : le client découvre les relations dynamiquement
- **Moins de couplage** : on peut changer les URLs côté serveur sans impacter les clients
- Conformité au **niveau 3 du Richardson Maturity Model**

**Q6 : Comment implémentez-vous HATEOAS avec Spring ?**

**R :** J'utilise **Spring HATEOAS** et la classe `EntityModel` :
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

**Q7 : Donnez un exemple de réponse JSON avec HATEOAS**

**R :**
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
Le client peut suivre le link `orders` pour récupérer les commandes sans connaître la structure de l'URL.

**Q8 : Quelle est la différence entre withSelfRel() et withRel() ?**

**R :** 
- **withSelfRel()** : crée un link avec la relation `"self"`, qui pointe vers la ressource actuelle
- **withRel("custom-name")** : crée un link avec une relation personnalisée pour naviguer vers une autre ressource liée

---

## **Questions/Réponses : HAL & HAL Explorer**

**Q9 : Qu'est-ce que HAL (Hypertext Application Language) ?**

**R :** HAL est un **standard de représentation JSON/XML** pour les APIs REST avec HATEOAS. Il définit une structure conventionnelle pour les links (`_links`) et les ressources embarquées (`_embedded`). Spring HATEOAS utilise HAL par défaut pour formater les réponses.

**Q10 : Comment utilisez-vous HAL Explorer ?**

**R :** HAL Explorer est un **browser web** intégré pour explorer visuellement les APIs. Je l'ajoute via la dépendance :
```xml
<dependency>
    <groupId>org.springframework.data</groupId>
    <artifactId>spring-data-rest-hal-explorer</artifactId>
</dependency>
```
Accessible via `/browser/index.html`, il permet de naviguer dans l'API en cliquant sur les links HATEOAS, très utile en développement pour tester les relations entre ressources.

**Q11 : Quelle est la différence entre HAL Explorer et Swagger UI ?**

**R :** 
- **Swagger UI** : documentation statique générée depuis les annotations, permet de tester les endpoints individuellement
- **HAL Explorer** : navigation dynamique basée sur les hypermedia links retournés par l'API, suit le principe HATEOAS

Les deux sont complémentaires : Swagger pour la documentation complète, HAL Explorer pour explorer les relations.

---

## **Questions/Réponses : Internationalization (i18n)**

**Q12 : Comment fonctionne l'internationalization avec Accept-Language ?**

**R :** Le client envoie sa préférence linguistique via le **HTTP header `Accept-Language`** :
```
GET /api/greeting
Accept-Language: fr-FR
```
Spring Boot utilise un `LocaleResolver` pour extraire la locale et un `MessageSource` pour charger les messages correspondants depuis les fichiers properties (`messages_fr.properties`, `messages_en.properties`).

**Q13 : Comment configurez-vous i18n dans Spring Boot ?**

**R :** Configuration du `LocaleResolver` et `MessageSource` :
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

**Q14 : Comment organisez-vous vos fichiers de ressources multilingues ?**

**R :** Je crée plusieurs fichiers properties dans `src/main/resources` :
- `messages.properties` (fallback)
- `messages_en.properties` : `user.not.found=User not found`
- `messages_fr.properties` : `user.not.found=Utilisateur introuvable`
- `messages_nl.properties` : `user.not.found=Gebruiker niet gevonden`

**Q15 : Comment récupérez-vous un message internationalisé dans le code ?**

**R :** J'injecte `MessageSource` et j'utilise la locale extraite du header :
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

**Q16 : Comment gérez-vous l'internationalization des messages de validation ?**

**R :** J'utilise les annotations Bean Validation avec des clés de messages :
```java
public class UserDTO {
    @NotBlank(message = "{user.name.required}")
    private String name;
    
    @Email(message = "{user.email.invalid}")
    private String email;
}
```

Fichiers properties :
- `messages_en.properties` : `user.name.required=Name is required`
- `messages_fr.properties` : `user.name.required=Le nom est obligatoire`

**Q17 : Quelle est la différence entre Accept-Language et un paramètre ?lang=fr ?**

**R :** 
- **Accept-Language (header)** : standard HTTP, respecte les principes REST, géré automatiquement par les browsers
- **?lang=fr (query parameter)** : non-standard, pollue l'URL, nécessite une configuration manuelle

Je privilégie toujours `Accept-Language` car c'est la **bonne pratique REST**.

---

## **Questions Transversales**

**Q18 : Comment ces technologies s'intègrent-elles ensemble dans un projet réel ?**

**R :** Dans mes projets :
1. **Swagger** documente tous les endpoints avec leurs paramètres et réponses
2. **HATEOAS** enrichit les réponses avec des links pour la navigation
3. **HAL Explorer** permet d'explorer visuellement ces links en développement
4. **i18n** internationalise les messages d'erreur et contenus métier

Exemple : un endpoint `GET /users/1` retourne un user avec des HATEOAS links, les messages d'erreur sont traduits selon `Accept-Language`, et tout est documenté dans Swagger.

**Q19 : Quels sont les niveaux du Richardson Maturity Model ?**

**R :** 
- **Niveau 0** : HTTP comme transport (SOAP)
- **Niveau 1** : Introduction des ressources (`/users`, `/orders`)
- **Niveau 2** : Utilisation correcte des verbes HTTP (GET, POST, PUT, DELETE) et status codes
- **Niveau 3** : HATEOAS - hypermedia controls

Avec HATEOAS, on atteint le **niveau 3**, le plus mature.

**Q20 : Comment testez-vous une API avec HATEOAS et i18n ?**

**R :** 
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