## [**..**](./00_index.md)

# API Versioning avec Spring Boot


---

### **Pourquoi le Versioning ?**

Le versioning est essentiel pour :
- **Éviter les breaking changes** : permettre aux anciens clients de continuer à fonctionner
- **Faciliter l'évolution** : introduire de nouvelles features sans impacter les versions existantes
- **Maintenir plusieurs versions** simultanément pendant la période de transition
- **Améliorer la maintenance** : isoler le code legacy des nouvelles implémentations

---

### **Les 4 Stratégies de Versioning**

J'ai travaillé avec quatre approches principales, chacune ayant ses avantages et inconvénients :

---

#### **1. Versioning through URI Path**

**Principe :** Inclure la version directement dans l'URL

**Implémentation :**
```java
@RestController
@RequestMapping("/api/v1/users")
public class UserControllerV1 {
    @GetMapping("/{id}")
    public UserV1 getUser(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
}

@RestController
@RequestMapping("/api/v2/users")
public class UserControllerV2 {
    @GetMapping("/{id}")
    public UserV2 getUser(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Exemples de requêtes :**
```
GET /api/v1/users/1
GET /api/v2/users/1
```

**Avantages :**
- ✅ Très simple et explicite
- ✅ Facile à tester et débugger
- ✅ Compatible avec les caches et proxies
- ✅ Visible dans les logs

**Inconvénients :**
- ❌ URLs différentes pour la même ressource
- ❌ Duplication de code entre contrôleurs

**Utilisation :** C'est l'approche la plus populaire (utilisée par Twitter, Stripe, GitHub).

---

#### **2. Versioning through Query Parameters**

**Principe :** Passer la version comme paramètre de requête

**Implémentation :**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping(value = "/{id}", params = "version=1")
    public UserV1 getUserV1(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
    
    @GetMapping(value = "/{id}", params = "version=2")
    public UserV2 getUserV2(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Exemples de requêtes :**
```
GET /api/users/1?version=1
GET /api/users/1?version=2
```

**Avantages :**
- ✅ URL de base identique
- ✅ Facile à implémenter
- ✅ Version optionnelle (default possible)

**Inconvénients :**
- ❌ Moins visible et intuitif
- ❌ Peut poser des problèmes avec certains caches
- ❌ Non-standard REST

**Utilisation :** Pratique pour des variations mineures ou features flags.

---

#### **3. Versioning through Custom Headers**

**Principe :** Utiliser un header HTTP personnalisé pour spécifier la version

**Implémentation :**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping(value = "/{id}", headers = "X-API-VERSION=1")
    public UserV1 getUserV1(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
    
    @GetMapping(value = "/{id}", headers = "X-API-VERSION=2")
    public UserV2 getUserV2(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Exemples de requêtes :**
```
GET /api/users/1
Headers: X-API-VERSION: 1

GET /api/users/1
Headers: X-API-VERSION: 2
```

**Avantages :**
- ✅ URL propre et stable
- ✅ Séparation des préoccupations (version dans les métadonnées)
- ✅ Flexible pour des métadonnées additionnelles

**Inconvénients :**
- ❌ Moins visible (headers cachés)
- ❌ Plus difficile à tester manuellement
- ❌ Peut être ignoré par certains proxies/caches
- ❌ Non-standard

**Utilisation :** Pour des APIs internes ou quand on veut garder les URLs propres.

---

#### **4. Versioning through Content Negotiation (Accept Header)**

**Principe :** Utiliser le standard HTTP `Accept` header pour négocier la version via le media type

**Implémentation :**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping(value = "/{id}", produces = "application/vnd.company.app-v1+json")
    public UserV1 getUserV1(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
    
    @GetMapping(value = "/{id}", produces = "application/vnd.company.app-v2+json")
    public UserV2 getUserV2(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Exemples de requêtes :**
```
GET /api/users/1
Accept: application/vnd.company.app-v1+json

GET /api/users/1
Accept: application/vnd.company.app-v2+json
```

**Avantages :**
- ✅ Respecte les principes REST et HTTP standards
- ✅ URL stable et propre
- ✅ Permet de négocier format ET version
- ✅ Approach la plus "RESTful"

**Inconvénients :**
- ❌ Plus complexe à implémenter et comprendre
- ❌ Difficile à tester avec un simple browser
- ❌ Moins intuitif pour les développeurs

**Utilisation :** Pour des APIs publiques respectant strictement REST (utilisée par GitHub API).

---

### **Versioning Cleaning & Best Practices**

**Stratégie de nettoyage :**

1. **Deprecation warnings** : Avertir les clients avant de supprimer une version
```java
@GetMapping("/api/v1/users/{id}")
@Deprecated
public ResponseEntity<UserV1> getUserV1(@PathVariable Long id) {
    return ResponseEntity.ok()
        .header("Warning", "299 - \"API v1 is deprecated. Please migrate to v2\"")
        .body(userService.getUserV1(id));
}
```

2. **Sunset Header** : Indiquer la date de fin de vie
```java
.header("Sunset", "Sat, 31 Dec 2025 23:59:59 GMT")
```

3. **Migration progressive** :
   - Maintenir v1 et v2 pendant 6-12 mois
   - Monitorer l'utilisation de v1 via logs/metrics
   - Communiquer avec les clients
   - Supprimer v1 après migration complète

4. **Documentation** : Documenter dans Swagger toutes les versions disponibles
```java
@Operation(summary = "Get user (v1 - DEPRECATED)", 
           deprecated = true)
```

---

### **Comparaison et Recommandations**

| Stratégie | Simplicité | RESTful | Visibilité | Cache-friendly | Recommandation |
|-----------|------------|---------|------------|----------------|----------------|
| **URI Path** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | **Meilleur choix général** |
| **Query Param** | ⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | Pour variations mineures |
| **Custom Header** | ⭐⭐⭐ | ⭐⭐ | ⭐⭐ | ⭐⭐⭐ | APIs internes |
| **Content Negotiation** | ⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ | APIs publiques strictement RESTful |

**Ma recommandation :**
- **URI Path** pour la majorité des projets (simplicité + clarté)
- **Content Negotiation** pour les APIs publiques nécessitant un strict respect REST
- Éviter le mélange de plusieurs stratégies dans le même projet

---

### **Exemple Complet : Migration v1 → v2**

**Scénario :** Ajout d'un champ `phoneNumber` dans UserV2

**Version 1 :**
```java
@Data
public class UserV1 {
    private Long id;
    private String name;
    private String email;
}
```

**Version 2 :**
```java
@Data
public class UserV2 {
    private Long id;
    private String name;
    private String email;
    private String phoneNumber; // Nouveau champ
}
```

**Controllers :**
```java
@RestController
public class UserController {
    
    @GetMapping("/api/v1/users/{id}")
    @Deprecated
    public ResponseEntity<UserV1> getUserV1(@PathVariable Long id) {
        User user = userService.findById(id);
        UserV1 userV1 = mapper.toV1(user);
        return ResponseEntity.ok()
            .header("Warning", "299 - \"Please migrate to v2\"")
            .body(userV1);
    }
    
    @GetMapping("/api/v2/users/{id}")
    public UserV2 getUserV2(@PathVariable Long id) {
        User user = userService.findById(id);
        return mapper.toV2(user);
    }
}
```

---

**Synthèse :** Le versioning est crucial pour maintenir des APIs évolutives sans breaking changes. La stratégie **URI Path** est généralement la plus pragmatique, mais le choix dépend du contexte (API publique vs interne, contraintes REST, complexité acceptable).

Voici des questions/réponses complètes pour préparer votre entretien sur le versioning :

---

## **Questions/Réponses : API Versioning**

### **Questions Générales**

**Q1 : Qu'est-ce que l'API Versioning et pourquoi est-ce important ?**

**R :** L'API Versioning consiste à gérer différentes versions d'une API simultanément. C'est crucial pour :
- **Éviter les breaking changes** : les anciens clients continuent de fonctionner pendant qu'on introduit de nouvelles features
- **Permettre l'évolution progressive** : on peut améliorer l'API sans forcer tous les clients à migrer immédiatement
- **Maintenir la backward compatibility** : respecter les contrats existants avec les consommateurs
- **Faciliter la maintenance** : séparer le code legacy des nouvelles implémentations

**Exemple concret :** Si je modifie le format de la réponse d'un endpoint (ex: ajouter un champ obligatoire ou changer un type), sans versioning, tous les clients existants casseraient.

---

**Q2 : Quand devez-vous créer une nouvelle version de votre API ?**

**R :** Je crée une nouvelle version lors de **breaking changes** :
- Suppression d'un champ dans la réponse
- Changement du type d'un champ (String → Integer)
- Modification de la structure JSON (objet → tableau)
- Changement de la logique métier qui affecte les contrats
- Renommage d'endpoints ou paramètres

**Je ne crée PAS de nouvelle version pour :**
- Ajout de nouveaux champs optionnels (backward compatible)
- Ajout de nouveaux endpoints
- Corrections de bugs
- Améliorations de performance

---

**Q3 : Quelles sont les 4 principales stratégies de versioning ?**

**R :** 
1. **URI Path versioning** : `/api/v1/users`, `/api/v2/users`
2. **Query Parameter versioning** : `/api/users?version=1`
3. **Custom Header versioning** : `X-API-VERSION: 1`
4. **Content Negotiation versioning** : `Accept: application/vnd.company.app-v1+json`

Chacune a ses avantages selon le contexte du projet.

---

### **Questions : URI Path Versioning**

**Q4 : Comment implémentez-vous le versioning via URI Path dans Spring Boot ?**

**R :** Je crée des contrôleurs séparés pour chaque version :
```java
@RestController
@RequestMapping("/api/v1/users")
public class UserControllerV1 {
    @GetMapping("/{id}")
    public UserV1DTO getUser(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
}

@RestController
@RequestMapping("/api/v2/users")
public class UserControllerV2 {
    @GetMapping("/{id}")
    public UserV2DTO getUser(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Q5 : Quels sont les avantages et inconvénients du URI Path versioning ?**

**R :** 

**Avantages :**
- ✅ **Très simple** : visible immédiatement dans l'URL
- ✅ **Facile à tester** : je peux tester dans le browser directement
- ✅ **Cache-friendly** : chaque version a son URL unique, donc pas de confusion pour les proxies/CDN
- ✅ **Clair dans les logs** : on voit exactement quelle version est appelée
- ✅ **Standard de l'industrie** : utilisé par Twitter, Stripe, GitHub

**Inconvénients :**
- ❌ **Duplication de code** : je dois maintenir plusieurs contrôleurs
- ❌ **URLs différentes** : la même ressource a plusieurs URLs

**Recommandation :** C'est ma stratégie préférée pour 90% des projets car la simplicité l'emporte sur les inconvénients.

---

**Q6 : Comment gérez-vous la duplication de code entre v1 et v2 ?**

**R :** J'utilise plusieurs techniques :

**1. Service Layer commun :**
```java
@Service
public class UserService {
    public User findById(Long id) {
        // Logique métier commune
    }
}

// Mappers séparés pour chaque version
@Component
public class UserMapperV1 {
    public UserV1DTO toDTO(User user) { ... }
}

@Component
public class UserMapperV2 {
    public UserV2DTO toDTO(User user) { ... }
}
```

**2. Héritage de contrôleurs :**
```java
public abstract class BaseUserController {
    @Autowired
    protected UserService userService;
    
    protected abstract Object mapToDTO(User user);
}

@RestController
@RequestMapping("/api/v1/users")
public class UserControllerV1 extends BaseUserController {
    protected UserV1DTO mapToDTO(User user) { ... }
}
```

**3. Composition via services versionnés :**
```java
public interface UserVersionService {
    Object getUser(Long id);
}

@Service("userServiceV1")
public class UserServiceV1 implements UserVersionService { ... }

@Service("userServiceV2")
public class UserServiceV2 implements UserVersionService { ... }
```

---

### **Questions : Query Parameter Versioning**

**Q7 : Comment implémentez-vous le versioning via Query Parameters ?**

**R :** J'utilise l'attribut `params` de l'annotation `@GetMapping` :
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping(value = "/{id}", params = "version=1")
    public UserV1DTO getUserV1(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
    
    @GetMapping(value = "/{id}", params = "version=2")
    public UserV2DTO getUserV2(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
    
    // Version par défaut si pas de paramètre
    @GetMapping("/{id}")
    public UserV2DTO getUserDefault(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Requêtes :**
```
GET /api/users/1?version=1
GET /api/users/1?version=2
GET /api/users/1  (utilise v2 par défaut)
```

---

**Q8 : Quand utiliseriez-vous le Query Parameter versioning ?**

**R :** Je l'utilise principalement pour :
- **Variations mineures** : différentes représentations de la même ressource (ex: format court vs complet)
- **Feature flags** : activer/désactiver certaines features
- **A/B testing** : tester différentes versions d'un endpoint

**Exemple :**
```
GET /api/users/1?format=full
GET /api/users/1?format=summary
```

Je **ne l'utilise pas** pour des versions majeures d'API car c'est moins visible et non-standard REST.

---

### **Questions : Custom Header Versioning**

**Q9 : Comment implémentez-vous le versioning via Custom Headers ?**

**R :** J'utilise l'attribut `headers` de Spring :
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping(value = "/{id}", headers = "X-API-VERSION=1")
    public UserV1DTO getUserV1(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
    
    @GetMapping(value = "/{id}", headers = "X-API-VERSION=2")
    public UserV2DTO getUserV2(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Requête avec curl :**
```bash
curl -H "X-API-VERSION: 1" http://localhost:8080/api/users/1
curl -H "X-API-VERSION: 2" http://localhost:8080/api/users/1
```

---

**Q10 : Quels sont les avantages du Custom Header versioning ?**

**R :** 

**Avantages :**
- ✅ **URL propre** : `/api/users/1` reste stable
- ✅ **Séparation des préoccupations** : la version est dans les métadonnées, pas dans la ressource
- ✅ **Flexible** : on peut ajouter d'autres headers pour des métadonnées (X-Client-ID, X-Feature-Flag)

**Inconvénients :**
- ❌ **Moins visible** : difficile de voir quelle version est utilisée
- ❌ **Difficile à tester** : nécessite un outil comme Postman ou curl
- ❌ **Peut être ignoré** par certains proxies ou caches
- ❌ **Non-standard** : pas de convention HTTP officielle

**Usage :** Je l'utilise pour des **APIs internes** où je contrôle les clients, rarement pour des APIs publiques.

---

### **Questions : Content Negotiation Versioning**

**Q11 : Qu'est-ce que le Content Negotiation versioning ?**

**R :** C'est l'utilisation du **header HTTP standard `Accept`** pour négocier non seulement le format (JSON, XML) mais aussi la version de l'API via un **custom media type**.

**Format standard :** `application/vnd.{vendor}.{resource}-v{version}+{format}`

**Exemples :**
```
Accept: application/vnd.company.user-v1+json
Accept: application/vnd.company.user-v2+json
Accept: application/vnd.github.v3+json  (GitHub)
```

---

**Q12 : Comment implémentez-vous le Content Negotiation versioning ?**

**R :** J'utilise l'attribut `produces` :
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping(value = "/{id}", 
                produces = "application/vnd.company.app-v1+json")
    public UserV1DTO getUserV1(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
    
    @GetMapping(value = "/{id}", 
                produces = "application/vnd.company.app-v2+json")
    public UserV2DTO getUserV2(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
    
    // Fallback avec application/json générique
    @GetMapping(value = "/{id}", 
                produces = MediaType.APPLICATION_JSON_VALUE)
    public UserV2DTO getUserDefault(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

**Requête :**
```bash
curl -H "Accept: application/vnd.company.app-v1+json" \
     http://localhost:8080/api/users/1
```

---

**Q13 : Pourquoi dit-on que Content Negotiation est la méthode la plus "RESTful" ?**

**R :** Parce qu'elle respecte les **principes REST et HTTP** :

1. **Uniform Interface** : l'URL représente la ressource (`/users/1`), pas la version
2. **Utilise les standards HTTP** : le header `Accept` est conçu pour négocier les représentations
3. **Séparation ressource/représentation** : la même ressource peut avoir plusieurs représentations (v1, v2, JSON, XML)
4. **Principe de Roy Fielding** : "Versioning should be a media type concern, not a URI concern"

**Cependant**, en pratique, même des APIs RESTful populaires (Twitter, Stripe) utilisent URI Path car c'est plus pragmatique.

---

**Q14 : Quand utilisez-vous Content Negotiation versioning ?**

**R :** Je l'utilise quand :
- Je construis une **API publique** devant respecter strictement REST
- Le client a besoin de **négocier plusieurs aspects** (version + format)
- L'équipe est expérimentée avec les concepts REST avancés

**Exemple :** GitHub API utilise ce pattern :
```
Accept: application/vnd.github.v3+json
Accept: application/vnd.github.v3.raw
Accept: application/vnd.github.v3.html
```

---

### **Questions : Versioning Cleaning & Best Practices**

**Q15 : Comment gérez-vous la dépréciation (deprecation) d'une ancienne version ?**

**R :** Je suis un processus en 4 étapes :

**1. Warning Header :**
```java
@GetMapping("/api/v1/users/{id}")
@Deprecated
public ResponseEntity<UserV1DTO> getUserV1(@PathVariable Long id) {
    UserV1DTO user = userService.getUserV1(id);
    return ResponseEntity.ok()
        .header("Warning", "299 - \"API v1 is deprecated. Migrate to v2 by 2025-12-31\"")
        .header("Sunset", "Sun, 31 Dec 2025 23:59:59 GMT")
        .body(user);
}
```

**2. Documentation Swagger :**
```java
@Operation(
    summary = "Get user by ID (DEPRECATED)", 
    deprecated = true,
    description = "This endpoint is deprecated. Please use /api/v2/users/{id}"
)
```

**3. Monitoring & Metrics :**
```java
@GetMapping("/api/v1/users/{id}")
public ResponseEntity<UserV1DTO> getUserV1(@PathVariable Long id) {
    metricsService.incrementCounter("api.v1.usage");
    logger.warn("API v1 called by client: {}", request.getHeader("User-Agent"));
    // ...
}
```

**4. Communication :**
- Email aux clients avec timeline de migration
- Blog post annonçant la dépréciation
- 6-12 mois de période de transition

---

**Q16 : Qu'est-ce que le Sunset Header et comment l'utilisez-vous ?**

**R :** Le **Sunset Header** (RFC 8594) indique la date à laquelle un endpoint sera retiré :

```java
@GetMapping("/api/v1/users/{id}")
public ResponseEntity<UserV1DTO> getUserV1(@PathVariable Long id) {
    return ResponseEntity.ok()
        .header("Sunset", "Sat, 31 Dec 2025 23:59:59 GMT")
        .header("Link", "</api/v2/users>; rel=\"successor-version\"")
        .body(user);
}
```

Les clients peuvent parser ce header pour alerter automatiquement les développeurs de la fin de vie imminente.

---

**Q17 : Comment supprimez-vous une ancienne version de façon sécurisée ?**

**R :** Je suis ce processus :

**Phase 1 - Annonce (T-12 mois) :**
- Ajouter Warning et Sunset headers
- Documenter dans Swagger
- Communiquer aux clients

**Phase 2 - Monitoring (T-6 mois) :**
- Tracker l'usage de v1 via metrics
- Contacter les clients utilisant encore v1
- Fournir des guides de migration

**Phase 3 - Read-Only (T-3 mois) :**
```java
@PostMapping("/api/v1/users")
public ResponseEntity<?> createUserV1(@RequestBody UserV1DTO user) {
    return ResponseEntity.status(HttpStatus.GONE)
        .body(new ErrorResponse("POST on v1 is disabled. Use v2"));
}

@GetMapping("/api/v1/users/{id}")  // GET encore disponible
public UserV1DTO getUserV1(@PathVariable Long id) {
    return userService.getUserV1(id);
}
```

**Phase 4 - Suppression (T-0) :**
```java
@GetMapping("/api/v1/users/{id}")
public ResponseEntity<?> getUserV1(@PathVariable Long id) {
    return ResponseEntity.status(HttpStatus.GONE)
        .header("Link", "</api/v2/users/{id}>; rel=\"successor-version\"")
        .body(new ErrorResponse("v1 is retired. Use v2"));
}
```

Ou simplement supprimer le contrôleur (HTTP 404).

---

**Q18 : Quelle est la différence entre HTTP 410 Gone et 404 Not Found pour une API dépréciée ?**

**R :** 
- **404 Not Found** : "Cette ressource n'existe pas" (erreur temporaire possible)
- **410 Gone** : "Cette ressource existait mais a été définitivement supprimée" (plus approprié pour dépréciation)

```java
@GetMapping("/api/v1/users/{id}")
public ResponseEntity<?> getUserV1Retired(@PathVariable Long id) {
    ApiError error = new ApiError(
        HttpStatus.GONE,
        "API v1 has been retired on 2025-12-31",
        "/api/v2/users/" + id
    );
    return new ResponseEntity<>(error, HttpStatus.GONE);
}
```

Le **410** indique clairement aux clients qu'ils doivent arrêter de retry et migrer.

---

### **Questions : Comparaison & Choix de Stratégie**

**Q19 : Quelle stratégie de versioning recommandez-vous et pourquoi ?**

**R :** Ma recommandation dépend du contexte :

**Pour 90% des projets : URI Path** (`/api/v1/users`)
- Simple, clair, standard de l'industrie
- Facile à tester et débugger
- Cache-friendly
- Utilisé par les grandes entreprises (Stripe, Twitter, AWS)

**Pour APIs publiques strictement RESTful : Content Negotiation**
- Respecte les principes REST
- URL stable
- Utilisé par GitHub

**Pour APIs internes avec beaucoup de métadonnées : Custom Headers**
- Flexible pour feature flags
- URL propre

**Je n'utilise JAMAIS Query Parameters** pour du versioning majeur (seulement pour variations mineures).

---

**Q20 : Comment testez-vous différentes versions d'API avec Spring Boot ?**

**R :** 

**Tests unitaires avec MockMvc :**
```java
@WebMvcTest(UserController.class)
public class UserControllerTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    @Test
    public void testGetUserV1() throws Exception {
        mockMvc.perform(get("/api/v1/users/1"))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.name").value("John"))
            .andExpect(jsonPath("$.phoneNumber").doesNotExist()); // v1 n'a pas ce champ
    }
    
    @Test
    public void testGetUserV2() throws Exception {
        mockMvc.perform(get("/api/v2/users/1"))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.name").value("John"))
            .andExpect(jsonPath("$.phoneNumber").exists()); // v2 a ce champ
    }
    
    @Test
    public void testContentNegotiation() throws Exception {
        mockMvc.perform(get("/api/users/1")
                .accept("application/vnd.company.app-v1+json"))
            .andExpect(status().isOk())
            .andExpect(content().contentType("application/vnd.company.app-v1+json"));
    }
    
    @Test
    public void testCustomHeader() throws Exception {
        mockMvc.perform(get("/api/users/1")
                .header("X-API-VERSION", "2"))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.phoneNumber").exists());
    }
}
```

**Tests d'intégration :**
```java
@SpringBootTest(webEnvironment = WebEnvironment.RANDOM_PORT)
public class VersioningIntegrationTest {
    
    @Autowired
    private TestRestTemplate restTemplate;
    
    @Test
    public void testV1AndV2ReturnDifferentStructures() {
        // Test v1
        UserV1DTO v1 = restTemplate.getForObject("/api/v1/users/1", UserV1DTO.class);
        assertNotNull(v1.getName());
        assertNull(v1.getPhoneNumber()); // v1 n'a pas ce champ
        
        // Test v2
        UserV2DTO v2 = restTemplate.getForObject("/api/v2/users/1", UserV2DTO.class);
        assertNotNull(v2.getName());
        assertNotNull(v2.getPhoneNumber()); // v2 a ce champ
    }
}
```

---

**Q21 : Comment documentez-vous les différentes versions dans Swagger ?**

**R :** 

**Configuration OpenAPI avec groupes :**
```java
@Configuration
public class SwaggerConfig {
    
    @Bean
    public GroupedOpenApi publicApiV1() {
        return GroupedOpenApi.builder()
            .group("v1-public-api")
            .pathsToMatch("/api/v1/**")
            .build();
    }
    
    @Bean
    public GroupedOpenApi publicApiV2() {
        return GroupedOpenApi.builder()
            .group("v2-public-api")
            .pathsToMatch("/api/v2/**")
            .build();
    }
    
    @Bean
    public OpenAPI customOpenAPI() {
        return new OpenAPI()
            .info(new Info()
                .title("User Management API")
                .version("2.0")
                .description("Multi-version API with v1 (deprecated) and v2"));
    }
}
```

**Documentation des endpoints :**
```java
@Tag(name = "Users V1 (DEPRECATED)", description = "User management endpoints - Version 1")
@RestController
@RequestMapping("/api/v1/users")
public class UserControllerV1 {
    
    @Operation(
        summary = "Get user by ID",
        deprecated = true,
        description = "⚠️ DEPRECATED: This version will be retired on 2025-12-31. Please use /api/v2/users/{id}"
    )
    @ApiResponse(responseCode = "200", description = "User found")
    @ApiResponse(responseCode = "410", description = "API version retired")
    @GetMapping("/{id}")
    public UserV1DTO getUser(@PathVariable Long id) {
        return userService.getUserV1(id);
    }
}

@Tag(name = "Users V2", description = "User management endpoints - Version 2 (Current)")
@RestController
@RequestMapping("/api/v2/users")
public class UserControllerV2 {
    
    @Operation(summary = "Get user by ID", description = "Returns detailed user information including phone number")
    @GetMapping("/{id}")
    public UserV2DTO getUser(@PathVariable Long id) {
        return userService.getUserV2(id);
    }
}
```

Swagger UI affichera deux groupes séparés, avec v1 clairement marquée comme deprecated.

---

**Q22 : Donnez un exemple complet de migration d'API v1 vers v2**

**R :** 

**Scénario :** Ajouter un champ `address` (objet complexe) dans User

**Modèles :**
```java
// V1 - Structure simple
@Data
public class UserV1DTO {
    private Long id;
    private String name;
    private String email;
}

// V2 - Avec adresse structurée
@Data
public class UserV2DTO {
    private Long id;
    private String name;
    private String email;
    private AddressDTO address; // Nouveau champ
}

@Data
public class AddressDTO {
    private String street;
    private String city;
    private String zipCode;
    private String country;
}
```

**Entity (unique) :**
```java
@Entity
public class User {
    @Id
    private Long id;
    private String name;
    private String email;
    
    // Champs ajoutés pour v2
    private String street;
    private String city;
    private String zipCode;
    private String country;
}
```

**Mappers :**
```java
@Component
public class UserMapper {
    
    public UserV1DTO toV1DTO(User user) {
        return UserV1DTO.builder()
            .id(user.getId())
            .name(user.getName())
            .email(user.getEmail())
            .build();
        // On ignore les champs d'adresse
    }
    
    public UserV2DTO toV2DTO(User user) {
        AddressDTO address = null;
        if (user.getStreet() != null) {
            address = AddressDTO.builder()
                .street(user.getStreet())
                .city(user.getCity())
                .zipCode(user.getZipCode())
                .country(user.getCountry())
                .build();
        }
        
        return UserV2DTO.builder()
            .id(user.getId())
            .name(user.getName())
            .email(user.getEmail())
            .address(address)
            .build();
    }
}
```

**Controllers :**
```java
@RestController
public class UserControllerV1 {
    
    @Autowired
    private UserService userService;
    
    @Autowired
    private UserMapper mapper;
    
    @GetMapping("/api/v1/users/{id}")
    @Deprecated
    public ResponseEntity<UserV1DTO> getUser(@PathVariable Long id) {
        User user = userService.findById(id);
        UserV1DTO dto = mapper.toV1DTO(user);
        
        return ResponseEntity.ok()
            .header("Warning", "299 - \"v1 deprecated. Use v2\"")
            .header("Sunset", "Sat, 31 Dec 2025 23:59:59 GMT")
            .body(dto);
    }
}

@RestController
public class UserControllerV2 {
    
    @Autowired
    private UserService userService;
    
    @Autowired
    private UserMapper mapper;
    
    @GetMapping("/api/v2/users/{id}")
    public UserV2DTO getUser(@PathVariable Long id) {
        User user = userService.findById(id);
        return mapper.toV2DTO(user);
    }
}
```

**Réponses :**

v1 :
```json
{
  "id": 1,
  "name": "John Doe",
  "email": "john@example.com"
}
```

v2 :
```json
{
  "id": 1,
  "name": "John Doe",
  "email": "john@example.com",
  "address": {
    "street": "123 Main St",
    "city": "Paris",
    "zipCode": "75001",
    "country": "France"
  }
}
```

---

**Q23 : Comment gérez-vous le versioning avec Spring Data REST ?**

**R :** Spring Data REST génère automatiquement des endpoints REST. Pour le versioning :

**Option 1 : Projections (recommandé) :**
```java
// Projection v1
@Projection(name = "v1", types = User.class)
public interface UserV1Projection {
    Long getId();
    String getName();
    String getEmail();
}

// Projection v2
@Projection(name = "v2", types = User.class)
public interface UserV2Projection {
    Long getId();
    String getName();
    String getEmail();
    Address getAddress();
}

// Utilisation
GET /api/users/1?projection=v1
GET /api/users/1?projection=v2
```

**Option 2 : RepositoryRestConfigurer :**
```java
@Configuration
public class RestConfiguration implements RepositoryRestConfigurer {
    
    @Override
    public void configureRepositoryRestConfiguration(RepositoryRestConfiguration config) {
        config.setBasePath("/api/v2");
    }
}
```

Mais en général, pour du versioning complexe, je préfère des **contrôleurs manuels** plutôt que Spring Data REST.

---

Ces questions couvrent tous les aspects du versioning d'API. Bonne préparation pour votre entretien ! 🚀