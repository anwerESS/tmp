## [**..**](./00_index.md)

## Sécurité des Microservices avec OAuth2 & Keycloak


---

## 1. Securing Gateway Server as a Resource Server (Keycloak)

### Architecture de Sécurité

**Composants de l'architecture sécurisée** :
```
Client (Browser/Mobile)
    ↓
    ↓ 1. Request avec Access Token
    ↓
API Gateway (Resource Server)
    ↓ 2. Valide Token avec Keycloak
    ↓ 3. Vérifie Signature JWT
    ↓ 4. Extract Claims & Roles
    ↓
Microservices Backend (accounts, loans, cards)
```

### Configuration Keycloak

**Installation et Setup Keycloak** :
- **Déploiement** : Keycloak sur Docker/Kubernetes pour centraliser l'authentification
- **Realm** : Création d'un realm dédié (ex: "microservices-realm")
- **Clients** : Configuration de clients OAuth2 pour Gateway et applications frontend
- **Roles & Groups** : Définition de rôles métier (ADMIN, USER, MANAGER)
- **Users** : Gestion des utilisateurs avec attribution de rôles

**Configuration du Realm Keycloak** :
```json
{
  "realm": "microservices-realm",
  "enabled": true,
  "clients": [
    {
      "clientId": "api-gateway",
      "enabled": true,
      "protocol": "openid-connect",
      "publicClient": false,
      "standardFlowEnabled": true,
      "directAccessGrantsEnabled": false,
      "serviceAccountsEnabled": true,
      "authorizationServicesEnabled": true,
      "redirectUris": ["http://localhost:8080/*"],
      "webOrigins": ["http://localhost:3000"],
      "attributes": {
        "access.token.lifespan": "300",
        "client.session.idle.timeout": "600"
      }
    }
  ],
  "roles": {
    "realm": [
      {"name": "ADMIN"},
      {"name": "USER"},
      {"name": "MANAGER"}
    ]
  }
}
```

### Configuration Spring Cloud Gateway comme Resource Server

**Dépendances Maven** :
```xml
<dependencies>
    <!-- Spring Cloud Gateway -->
    <dependency>
        <groupId>org.springframework.cloud</groupId>
        <artifactId>spring-cloud-starter-gateway</artifactId>
    </dependency>
    
    <!-- OAuth2 Resource Server -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-oauth2-resource-server</artifactId>
    </dependency>
    
    <!-- OAuth2 Client (pour Authorization Code flow) -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-oauth2-client</artifactId>
    </dependency>
    
    <!-- Spring Security -->
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-security</artifactId>
    </dependency>
</dependencies>
```

**Configuration Application (application.yml)** :
```yaml
spring:
  application:
    name: api-gateway
  
  security:
    oauth2:
      # Configuration Resource Server
      resourceserver:
        jwt:
          # URL pour récupérer les clés publiques de Keycloak
          jwk-set-uri: http://localhost:8080/realms/microservices-realm/protocol/openid-connect/certs
          # Issuer pour validation du token
          issuer-uri: http://localhost:8080/realms/microservices-realm
      
      # Configuration Client OAuth2
      client:
        provider:
          keycloak:
            issuer-uri: http://localhost:8080/realms/microservices-realm
            user-name-attribute: preferred_username
        
        registration:
          api-gateway-client:
            provider: keycloak
            client-id: api-gateway
            client-secret: ${KEYCLOAK_CLIENT_SECRET}
            authorization-grant-type: authorization_code
            redirect-uri: "{baseUrl}/login/oauth2/code/{registrationId}"
            scope:
              - openid
              - profile
              - email
              - roles
  
  cloud:
    gateway:
      routes:
        # Route vers Accounts Service
        - id: accounts-service
          uri: lb://ACCOUNTS-SERVICE
          predicates:
            - Path=/api/accounts/**
          filters:
            - TokenRelay=  # Propage le token vers le backend
            - RemoveRequestHeader=Cookie
        
        # Route vers Loans Service
        - id: loans-service
          uri: lb://LOANS-SERVICE
          predicates:
            - Path=/api/loans/**
          filters:
            - TokenRelay=
        
        # Route vers Cards Service (admin seulement)
        - id: cards-service
          uri: lb://CARDS-SERVICE
          predicates:
            - Path=/api/cards/**
          filters:
            - TokenRelay=

server:
  port: 8072

# Actuator pour monitoring
management:
  endpoints:
    web:
      exposure:
        include: health,info,metrics,gateway
```

**Configuration de Sécurité (SecurityConfig.java)** :
```java
@Configuration
@EnableWebFluxSecurity  // Pour reactive (Spring Cloud Gateway est reactive)
public class SecurityConfig {
    
    @Bean
    public SecurityWebFilterChain securityWebFilterChain(ServerHttpSecurity http) {
        http
            // Configuration CSRF
            .csrf(csrf -> csrf.disable())  // Désactivé pour APIs stateless
            
            // Configuration CORS
            .cors(cors -> cors.configurationSource(corsConfigurationSource()))
            
            // Configuration des autorisations
            .authorizeExchange(exchanges -> exchanges
                // Endpoints publics (pas d'authentification requise)
                .pathMatchers("/actuator/health", "/actuator/info").permitAll()
                .pathMatchers("/public/**").permitAll()
                .pathMatchers("/auth/**").permitAll()
                
                // Tous les autres endpoints nécessitent authentification
                .anyExchange().authenticated()
            )
            
            // Configuration OAuth2 Resource Server
            .oauth2ResourceServer(oauth2 -> oauth2
                .jwt(jwt -> jwt
                    .jwtAuthenticationConverter(grantedAuthoritiesExtractor())
                )
            )
            
            // Configuration OAuth2 Login (pour Authorization Code flow)
            .oauth2Login(Customizer.withDefaults());
        
        return http.build();
    }
    
    /**
     * Extracteur de rôles depuis le JWT
     * Keycloak stocke les rôles dans jwt.realm_access.roles
     */
    @Bean
    public Converter<Jwt, Mono<AbstractAuthenticationToken>> grantedAuthoritiesExtractor() {
        JwtAuthenticationConverter jwtAuthConverter = new JwtAuthenticationConverter();
        jwtAuthConverter.setJwtGrantedAuthoritiesConverter(new KeycloakRoleConverter());
        return new ReactiveJwtAuthenticationConverterAdapter(jwtAuthConverter);
    }
    
    /**
     * Configuration CORS
     */
    @Bean
    public CorsConfigurationSource corsConfigurationSource() {
        CorsConfiguration configuration = new CorsConfiguration();
        configuration.setAllowedOrigins(Arrays.asList("http://localhost:3000", "https://myapp.com"));
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

**Conversion des Rôles Keycloak** :
```java
/**
 * Convertisseur pour extraire les rôles depuis le token JWT Keycloak
 */
public class KeycloakRoleConverter implements Converter<Jwt, Collection<GrantedAuthority>> {
    
    @Override
    public Collection<GrantedAuthority> convert(Jwt jwt) {
        // Keycloak stocke les rôles dans realm_access.roles
        Map<String, Object> realmAccess = jwt.getClaim("realm_access");
        
        if (realmAccess == null || realmAccess.isEmpty()) {
            return Collections.emptyList();
        }
        
        @SuppressWarnings("unchecked")
        List<String> roles = (List<String>) realmAccess.get("roles");
        
        if (roles == null || roles.isEmpty()) {
            return Collections.emptyList();
        }
        
        // Convertir les rôles en GrantedAuthority avec préfixe ROLE_
        return roles.stream()
            .map(role -> new SimpleGrantedAuthority("ROLE_" + role.toUpperCase()))
            .collect(Collectors.toList());
    }
}
```

**Validation et Claims du Token** :
```java
@Service
public class TokenValidationService {
    
    @Autowired
    private JwtDecoder jwtDecoder;
    
    /**
     * Valide et décode le JWT
     */
    public Map<String, Object> validateAndExtractClaims(String token) {
        try {
            Jwt jwt = jwtDecoder.decode(token);
            
            // Claims standards
            String subject = jwt.getSubject();  // Username
            String issuer = jwt.getIssuer().toString();
            Instant issuedAt = jwt.getIssuedAt();
            Instant expiresAt = jwt.getExpiresAt();
            
            // Claims personnalisés
            String email = jwt.getClaim("email");
            String preferredUsername = jwt.getClaim("preferred_username");
            List<String> roles = extractRoles(jwt);
            
            // Retourner toutes les informations
            Map<String, Object> claims = new HashMap<>();
            claims.put("subject", subject);
            claims.put("email", email);
            claims.put("username", preferredUsername);
            claims.put("roles", roles);
            claims.put("issuedAt", issuedAt);
            claims.put("expiresAt", expiresAt);
            
            return claims;
            
        } catch (JwtException e) {
            throw new UnauthorizedException("Invalid JWT token: " + e.getMessage());
        }
    }
    
    /**
     * Extrait les rôles du token
     */
    private List<String> extractRoles(Jwt jwt) {
        Map<String, Object> realmAccess = jwt.getClaim("realm_access");
        if (realmAccess != null && realmAccess.containsKey("roles")) {
            return (List<String>) realmAccess.get("roles");
        }
        return Collections.emptyList();
    }
    
    /**
     * Vérifie si l'utilisateur a un rôle spécifique
     */
    public boolean hasRole(Jwt jwt, String requiredRole) {
        List<String> roles = extractRoles(jwt);
        return roles.contains(requiredRole);
    }
}
```

---

## 2. Implement Authorization inside Gateway Server using Roles

### Autorisation basée sur les Rôles (RBAC)

**Configuration des règles d'autorisation par route** :
```java
@Configuration
public class GatewaySecurityConfig {
    
    @Bean
    public SecurityWebFilterChain securityFilterChain(ServerHttpSecurity http) {
        http
            .authorizeExchange(exchanges -> exchanges
                // Endpoints publics
                .pathMatchers("/actuator/health").permitAll()
                .pathMatchers("/public/**").permitAll()
                
                // Accounts - USER role minimum
                .pathMatchers(HttpMethod.GET, "/api/accounts/**")
                    .hasAnyRole("USER", "ADMIN")
                .pathMatchers(HttpMethod.POST, "/api/accounts/**")
                    .hasAnyRole("USER", "ADMIN")
                .pathMatchers(HttpMethod.PUT, "/api/accounts/**")
                    .hasAnyRole("USER", "ADMIN")
                .pathMatchers(HttpMethod.DELETE, "/api/accounts/**")
                    .hasRole("ADMIN")
                
                // Loans - USER role
                .pathMatchers("/api/loans/**")
                    .hasAnyRole("USER", "MANAGER", "ADMIN")
                
                // Cards - ADMIN uniquement
                .pathMatchers("/api/cards/**")
                    .hasRole("ADMIN")
                
                // Opérations administratives - ADMIN uniquement
                .pathMatchers("/api/admin/**")
                    .hasRole("ADMIN")
                
                // Rapports - MANAGER et ADMIN
                .pathMatchers("/api/reports/**")
                    .hasAnyRole("MANAGER", "ADMIN")
                
                // Autres routes nécessitent authentification
                .anyExchange().authenticated()
            )
            .oauth2ResourceServer(oauth2 -> oauth2
                .jwt(jwt -> jwt.jwtAuthenticationConverter(jwtAuthConverter()))
            );
        
        return http.build();
    }
}
```

### Gateway Filters pour Autorisation Fine

**Custom Gateway Filter pour vérification de rôles** :
```java
@Component
public class RoleBasedAuthorizationGatewayFilter implements GlobalFilter, Ordered {
    
    private static final Logger log = LoggerFactory.getLogger(RoleBasedAuthorizationGatewayFilter.class);
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = exchange.getRequest();
        String path = request.getPath().toString();
        
        // Récupérer l'authentification depuis le contexte réactif
        return exchange.getPrincipal()
            .cast(JwtAuthenticationToken.class)
            .flatMap(authentication -> {
                Jwt jwt = (Jwt) authentication.getPrincipal();
                
                // Extraire les rôles
                Collection<String> roles = authentication.getAuthorities().stream()
                    .map(GrantedAuthority::getAuthority)
                    .map(role -> role.replace("ROLE_", ""))
                    .collect(Collectors.toList());
                
                log.info("User: {} with roles: {} accessing path: {}", 
                    jwt.getSubject(), roles, path);
                
                // Vérifications d'autorisation personnalisées
                if (path.startsWith("/api/cards") && !roles.contains("ADMIN")) {
                    return unauthorized(exchange, "Access to Cards API requires ADMIN role");
                }
                
                if (path.startsWith("/api/reports") && 
                    !roles.contains("ADMIN") && !roles.contains("MANAGER")) {
                    return unauthorized(exchange, "Access to Reports requires ADMIN or MANAGER role");
                }
                
                // Ajouter les infos utilisateur dans les headers vers backend
                ServerHttpRequest modifiedRequest = request.mutate()
                    .header("X-User-Id", jwt.getSubject())
                    .header("X-User-Email", jwt.getClaim("email"))
                    .header("X-User-Roles", String.join(",", roles))
                    .build();
                
                return chain.filter(exchange.mutate().request(modifiedRequest).build());
            })
            .onErrorResume(e -> {
                log.error("Authorization error: {}", e.getMessage());
                return unauthorized(exchange, "Authentication required");
            });
    }
    
    private Mono<Void> unauthorized(ServerWebExchange exchange, String message) {
        exchange.getResponse().setStatusCode(HttpStatus.FORBIDDEN);
        exchange.getResponse().getHeaders().setContentType(MediaType.APPLICATION_JSON);
        
        String errorResponse = String.format(
            "{\"error\":\"Forbidden\",\"message\":\"%s\",\"timestamp\":\"%s\"}", 
            message, 
            Instant.now()
        );
        
        DataBuffer buffer = exchange.getResponse()
            .bufferFactory()
            .wrap(errorResponse.getBytes(StandardCharsets.UTF_8));
        
        return exchange.getResponse().writeWith(Mono.just(buffer));
    }
    
    @Override
    public int getOrder() {
        return -100;  // Exécuter tôt dans la chaîne de filtres
    }
}
```

### Autorisation Fine avec Méthode HTTP

**Route-based Authorization Filter** :
```java
@Component
public class RouteBasedAuthorizationFilter implements GatewayFilter {
    
    private final Map<String, Set<String>> routeRoleMapping;
    
    public RouteBasedAuthorizationFilter() {
        // Mapping route → rôles autorisés
        this.routeRoleMapping = Map.of(
            "accounts-service", Set.of("USER", "ADMIN"),
            "loans-service", Set.of("USER", "MANAGER", "ADMIN"),
            "cards-service", Set.of("ADMIN"),
            "reports-service", Set.of("MANAGER", "ADMIN")
        );
    }
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        // Récupérer le route ID
        Route route = exchange.getAttribute(ServerWebExchangeUtils.GATEWAY_ROUTE_ATTR);
        String routeId = route != null ? route.getId() : "unknown";
        
        return exchange.getPrincipal()
            .cast(JwtAuthenticationToken.class)
            .flatMap(auth -> {
                Set<String> userRoles = auth.getAuthorities().stream()
                    .map(GrantedAuthority::getAuthority)
                    .map(role -> role.replace("ROLE_", ""))
                    .collect(Collectors.toSet());
                
                Set<String> requiredRoles = routeRoleMapping.getOrDefault(routeId, Set.of());
                
                // Vérifier si l'utilisateur a au moins un rôle requis
                boolean authorized = userRoles.stream()
                    .anyMatch(requiredRoles::contains);
                
                if (!authorized) {
                    ServerHttpResponse response = exchange.getResponse();
                    response.setStatusCode(HttpStatus.FORBIDDEN);
                    
                    String error = String.format(
                        "{\"error\":\"Insufficient permissions\",\"required_roles\":%s}",
                        new ObjectMapper().writeValueAsString(requiredRoles)
                    );
                    
                    DataBuffer buffer = response.bufferFactory()
                        .wrap(error.getBytes(StandardCharsets.UTF_8));
                    
                    return response.writeWith(Mono.just(buffer));
                }
                
                return chain.filter(exchange);
            });
    }
}
```

### Configuration des Filtres par Route

**Application.yml avec filtres personnalisés** :
```yaml
spring:
  cloud:
    gateway:
      routes:
        - id: accounts-service
          uri: lb://ACCOUNTS-SERVICE
          predicates:
            - Path=/api/accounts/**
          filters:
            - TokenRelay=
            - name: RoleBasedAuthorization
              args:
                requiredRoles: USER,ADMIN
        
        - id: cards-service
          uri: lb://CARDS-SERVICE
          predicates:
            - Path=/api/cards/**
          filters:
            - TokenRelay=
            - name: RoleBasedAuthorization
              args:
                requiredRoles: ADMIN
        
        - id: reports-service
          uri: lb://REPORTS-SERVICE
          predicates:
            - Path=/api/reports/**
          filters:
            - TokenRelay=
            - name: RoleBasedAuthorization
              args:
                requiredRoles: MANAGER,ADMIN
```

---

## 3. Securing Gateway Server using Authorization Code Grant Type Flow

### OAuth2 Authorization Code Flow Implementation

**Flow complet Authorization Code** :
```
1. User → Frontend → Click "Login"
2. Frontend → Redirects to Keycloak Login Page
3. User → Enters credentials in Keycloak
4. Keycloak → Validates credentials
5. Keycloak → Redirects to Gateway with Authorization Code
6. Gateway → Exchanges Code for Access Token (backend call to Keycloak)
7. Keycloak → Returns Access Token + Refresh Token
8. Gateway → Stores tokens in session
9. Gateway → Redirects user to application
10. Frontend → Makes API calls with Access Token
11. Gateway → Validates token and forwards request to microservices
```

### Configuration OAuth2 Client (Authorization Code)

**Application.yml - OAuth2 Client Configuration** :
```yaml
spring:
  security:
    oauth2:
      client:
        provider:
          keycloak:
            issuer-uri: http://localhost:8080/realms/microservices-realm
            authorization-uri: http://localhost:8080/realms/microservices-realm/protocol/openid-connect/auth
            token-uri: http://localhost:8080/realms/microservices-realm/protocol/openid-connect/token
            user-info-uri: http://localhost:8080/realms/microservices-realm/protocol/openid-connect/userinfo
            jwk-set-uri: http://localhost:8080/realms/microservices-realm/protocol/openid-connect/certs
            user-name-attribute: preferred_username
        
        registration:
          keycloak-client:
            provider: keycloak
            client-id: api-gateway
            client-secret: ${KEYCLOAK_CLIENT_SECRET}
            authorization-grant-type: authorization_code
            redirect-uri: "{baseUrl}/login/oauth2/code/{registrationId}"
            scope:
              - openid
              - profile
              - email
              - roles
            client-name: API Gateway Client
```

### Security Configuration pour Authorization Code Flow

**SecurityConfig avec OAuth2 Login** :
```java
@Configuration
@EnableWebFluxSecurity
public class OAuth2LoginSecurityConfig {
    
    @Bean
    public SecurityWebFilterChain springSecurityFilterChain(ServerHttpSecurity http) {
        http
            .csrf(csrf -> csrf.disable())
            
            .authorizeExchange(exchanges -> exchanges
                // Public endpoints
                .pathMatchers("/", "/home", "/public/**").permitAll()
                .pathMatchers("/login/**", "/oauth2/**").permitAll()
                .pathMatchers("/actuator/health").permitAll()
                
                // Protected endpoints
                .pathMatchers("/api/**").authenticated()
                .anyExchange().authenticated()
            )
            
            // OAuth2 Login configuration
            .oauth2Login(oauth2Login -> oauth2Login
                .authenticationSuccessHandler(authenticationSuccessHandler())
                .authenticationFailureHandler(authenticationFailureHandler())
            )
            
            // OAuth2 Resource Server (valide les tokens dans les requêtes API)
            .oauth2ResourceServer(oauth2 -> oauth2
                .jwt(jwt -> jwt.jwtAuthenticationConverter(jwtAuthConverter()))
            )
            
            // Logout configuration
            .logout(logout -> logout
                .logoutUrl("/logout")
                .logoutSuccessHandler(oidcLogoutSuccessHandler())
            );
        
        return http.build();
    }
    
    /**
     * Handler de succès après authentication
     */
    @Bean
    public ServerAuthenticationSuccessHandler authenticationSuccessHandler() {
        return new ServerAuthenticationSuccessHandler() {
            @Override
            public Mono<Void> onAuthenticationSuccess(
                WebFilterExchange webFilterExchange, 
                Authentication authentication) {
                
                OAuth2AuthenticationToken oauth2Token = 
                    (OAuth2AuthenticationToken) authentication;
                
                // Extraire les informations utilisateur
                OAuth2User oauth2User = oauth2Token.getPrincipal();
                String username = oauth2User.getAttribute("preferred_username");
                String email = oauth2User.getAttribute("email");
                
                log.info("User {} logged in successfully with email: {}", username, email);
                
                // Rediriger vers la page d'accueil
                ServerWebExchange exchange = webFilterExchange.getExchange();
                return new RedirectServerAuthenticationSuccessHandler("/home")
                    .onAuthenticationSuccess(webFilterExchange, authentication);
            }
        };
    }
    
    /**
     * Handler d'échec d'authentication
     */
    @Bean
    public ServerAuthenticationFailureHandler authenticationFailureHandler() {
        return (webFilterExchange, exception) -> {
            log.error("Authentication failed: {}", exception.getMessage());
            
            ServerWebExchange exchange = webFilterExchange.getExchange();
            exchange.getResponse().setStatusCode(HttpStatus.UNAUTHORIZED);
            
            String errorJson = String.format(
                "{\"error\":\"authentication_failed\",\"message\":\"%s\"}", 
                exception.getMessage()
            );
            
            DataBuffer buffer = exchange.getResponse().bufferFactory()
                .wrap(errorJson.getBytes(StandardCharsets.UTF_8));
            
            return exchange.getResponse().writeWith(Mono.just(buffer));
        };
    }
    
    /**
     * Logout handler pour OIDC
     */
    @Bean
    public ServerLogoutSuccessHandler oidcLogoutSuccessHandler() {
        OidcClientInitiatedServerLogoutSuccessHandler successHandler =
            new OidcClientInitiatedServerLogoutSuccessHandler(clientRegistrationRepository());
        
        successHandler.setPostLogoutRedirectUri("{baseUrl}/logged-out");
        return successHandler;
    }
}
```

### Token Management Service

**Service pour gérer les tokens** :
```java
@Service
public class TokenManagementService {
    
    @Autowired
    private ReactiveOAuth2AuthorizedClientService authorizedClientService;
    
    /**
     * Récupère l'Access Token de l'utilisateur authentifié
     */
    public Mono<String> getAccessToken(Authentication authentication) {
        if (!(authentication instanceof OAuth2AuthenticationToken)) {
            return Mono.empty();
        }
        
        OAuth2AuthenticationToken oauth2Token = (OAuth2AuthenticationToken) authentication;
        String clientRegistrationId = oauth2Token.getAuthorizedClientRegistrationId();
        String principalName = oauth2Token.getName();
        
        return authorizedClientService
            .loadAuthorizedClient(clientRegistrationId, principalName)
            .map(client -> client.getAccessToken().getTokenValue());
    }
    
    /**
     * Récupère le Refresh Token
     */
    public Mono<String> getRefreshToken(Authentication authentication) {
        OAuth2AuthenticationToken oauth2Token = (OAuth2AuthenticationToken) authentication;
        
        return authorizedClientService
            .loadAuthorizedClient(
                oauth2Token.getAuthorizedClientRegistrationId(),
                oauth2Token.getName()
            )
            .map(client -> client.getRefreshToken())
            .map(OAuth2RefreshToken::getTokenValue);
    }
    
    /**
     * Rafraîchit l'Access Token avec le Refresh Token
     */
    public Mono<OAuth2AccessToken> refreshAccessToken(String refreshToken) {
        // Logique de refresh avec Keycloak
        // Appel POST vers /token avec grant_type=refresh_token
        return Mono.empty(); // Implémentation simplifiée
    }
    
    /**
     * Révoque le token (logout)
     */
    public Mono<Void> revokeToken(String token) {
        // Appel vers Keycloak revocation endpoint
        return Mono.empty(); // Implémentation simplifiée
    }
}
```

### Controller pour Authentication Flow

**REST Controller pour gérer l'authentication** :
```java
@RestController
@RequestMapping("/auth")
public class AuthenticationController {
    
    @Autowired
    private TokenManagementService tokenService;
    
    /**
     * Endpoint pour initier le login OAuth2
     */
    @GetMapping("/login")
    public Mono<Void> login(ServerWebExchange exchange) {
        // Redirige vers Keycloak pour authentication
        return Mono.fromRunnable(() -> {
            exchange.getResponse().setStatusCode(HttpStatus.FOUND);
            exchange.getResponse().getHeaders()
                .setLocation(URI.create("/oauth2/authorization/keycloak-client"));
        }).then();
    }
    
    /**
     * Récupère les informations de l'utilisateur connecté
     */
    @GetMapping("/user-info")
    public Mono<Map<String, Object>> getUserInfo(@AuthenticationPrincipal OAuth2User principal) {
        Map<String, Object> userInfo = new HashMap<>();
        userInfo.put("username", principal.getAttribute("preferred_username"));
        userInfo.put("email", principal.getAttribute("email"));
        userInfo.put("firstName", principal.getAttribute("given_name"));
        userInfo.put("lastName", principal.getAttribute("family_name"));
        userInfo.put("roles", extractRoles(principal));
        
        return Mono.just(userInfo);
    }
    
    /**
     * Récupère l'Access Token de l'utilisateur
     */
    @GetMapping("/token")
    public Mono<Map<String, String>> getToken(Authentication authentication) {
        return tokenService.getAccessToken(authentication)
            .map(token -> Map.of(
                "access_token", token,
                "token_type", "Bearer"
            ));
    }
    
    /**
     * Refresh le token
     */
    @PostMapping("/refresh")
    public Mono<OAuth2AccessToken> refreshToken(@RequestBody String refreshToken) {
        return tokenService.refreshAccessToken(refreshToken);
    }
    
    /**
     * Logout
     */
    @PostMapping("/logout")
    public Mono<Void> logout(ServerWebExchange exchange, Authentication authentication) {
        return tokenService.getAccessToken(authentication)
            .flatMap(token -> tokenService.revokeToken(token))
            .then(exchange.getSession())
            .flatMap(WebSession::invalidate);
    }
    
    private List<String> extractRoles(OAuth2User principal) {
        Map<String, Object> realmAccess = principal.getAttribute("realm_access");
        if (realmAccess != null && realmAccess.containsKey("roles")) {
            return (List<String>) realmAccess.get("roles");
        }
        return Collections.emptyList();
    }
}
```

### Frontend Integration (React Example)

**Integration côté frontend** :
```javascript
// AuthService.js
class AuthService {
  
  // Initier le login OAuth2
  login() {
    window.location.href = 'http://localhost:8072/oauth2/authorization/keycloak-client';
  }
  
  // Récupérer le token depuis le cookie de session
  async getAccessToken() {
    const response = await fetch('http://localhost:8072/auth/token', {
      credentials: 'include'  // Important pour les cookies
    });
    const data = await response.json();
    return data.access_token;
  }
  
  // Appel API avec token
  async callProtectedAPI(endpoint) {
    const token = await this.getAccessToken();
    
    const response = await fetch(`http://localhost:8072${endpoint}`, {
      headers: {
        'Authorization': `Bearer ${token}`
      },
      credentials: 'include'
    });
    
    return response.json();
  }
  
  // Récupérer les informations utilisateur
  async getUserInfo() {
    const response = await fetch('http://localhost:8072/auth/user-info', {
      credentials: 'include'
    });
    return response.json();
  }
  
  // Logout
  async logout() {
    await fetch('http://localhost:8072/auth/logout', {
      method: 'POST',
      credentials: 'include'
    });
    window.location.href = '/';
  }
  
  // Vérifier si l'utilisateur est authentifié
  async isAuthenticated() {
    try {
      await this.getUserInfo();
      return true;
    } catch (error) {
      return false;
    }
  }
}

export default new AuthService();
```

```jsx
// App.jsx - Exemple d'utilisation
import React, { useEffect, useState } from 'react';
import AuthService from './services/AuthService';

function App() {
  const [user, setUser] = useState(null);
  const [loading, setLoading] = useState(true);
  
  useEffect(() => {
    loadUserInfo();
  }, []);
  
  const loadUserInfo = async () => {
    try {
      const userInfo = await AuthService.getUserInfo();
      setUser(userInfo);
    } catch (error) {
      console.log('User not authenticated');
    } finally {
      setLoading(false);
    }
  };
  
  const handleLogin = () => {
    AuthService.login();
  };
  
  const handleLogout = async () => {
    await AuthService.logout();
    setUser(null);
  };
  
  const callAccountsAPI = async () => {
    try {
      const data = await AuthService.callProtectedAPI('/api/accounts/myaccount');
      console.log('Account data:', data);
    } catch (error) {
      console.error('API call failed:', error);
    }
  };
  
  if (loading) {
    return <div>Loading...</div>;
  }
  
  return (
    <div className="App">
      {user ? (
        <div>
          <h1>Welcome, {user.username}!</h1>
          <p>Email: {user.email}</p>
          <p>Roles: {user.roles.join(', ')}</p>
          <button onClick={callAccountsAPI}>Get My Account</button>
          <button onClick={handleLogout}>Logout</button>
        </div>
      ) : (
        <div>
          <h1>Please login</h1>
          <button onClick={handleLogin}>Login with Keycloak</button>
        </div>
      )}
    </div>
  );
}

export default App;
```

---

### Token Relay vers Microservices Backend

**Configuration pour propager le token** :
```yaml
spring:
  cloud:
    gateway:
      routes:
        - id: accounts-service
          uri: lb://ACCOUNTS-SERVICE
          predicates:
            - Path=/api/accounts/**
          filters:
            - TokenRelay=  # Propage automatiquement le token OAuth2
            - RemoveRequestHeader=Cookie  # Enlève les cookies
```

**Gateway Filter personnalisé pour Token Relay** :
```java
@Component
public class CustomTokenRelayGatewayFilter implements GlobalFilter, Ordered {
    
    @Autowired
    private ReactiveOAuth2AuthorizedClientService authorizedClientService;
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        return exchange.getPrincipal()
            .filter(principal -> principal instanceof OAuth2AuthenticationToken)
            .cast(OAuth2AuthenticationToken.class)
            .flatMap(oauth2Token -> 
                authorizedClientService.loadAuthorizedClient(
                    oauth2Token.getAuthorizedClientRegistrationId(),
                    oauth2Token.getName()
                )
            )
            .map(OAuth2AuthorizedClient::getAccessToken)
            .map(OAuth2AccessToken::getTokenValue)
            .flatMap(token -> {
                // Ajouter le token dans le header Authorization
                ServerHttpRequest modifiedRequest = exchange.getRequest().mutate()
                    .header(HttpHeaders.AUTHORIZATION, "Bearer " + token)
                    .build();
                
                return chain.filter(exchange.mutate().request(modifiedRequest).build());
            })
            .switchIfEmpty(chain.filter(exchange));
    }
    
    @Override
    public int getOrder() {
        return -50;
    }
}
```

---

### Sécurisation des Microservices Backend

**Configuration des Microservices comme Resource Servers** :
```java
// Dans chaque microservice (Accounts, Loans, Cards)
@Configuration
@EnableWebSecurity
@EnableMethodSecurity  // Pour @PreAuthorize
public class ResourceServerConfig {
    
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
            .csrf(csrf -> csrf.disable())
            
            .authorizeHttpRequests(auth -> auth
                // Actuator endpoints
                .requestMatchers("/actuator/health", "/actuator/info").permitAll()
                
                // API endpoints - tous nécessitent authentification
                .requestMatchers("/api/**").authenticated()
                
                .anyRequest().authenticated()
            )
            
            // Configuration OAuth2 Resource Server
            .oauth2ResourceServer(oauth2 -> oauth2
                .jwt(jwt -> jwt
                    .jwtAuthenticationConverter(jwtAuthenticationConverter())
                )
            );
        
        return http.build();
    }
    
    /**
     * Converter pour extraire les rôles du JWT
     */
    @Bean
    public JwtAuthenticationConverter jwtAuthenticationConverter() {
        JwtGrantedAuthoritiesConverter grantedAuthoritiesConverter = 
            new JwtGrantedAuthoritiesConverter();
        
        // Ne pas ajouter le préfixe SCOPE_ automatiquement
        grantedAuthoritiesConverter.setAuthorityPrefix("");
        
        JwtAuthenticationConverter jwtAuthConverter = new JwtAuthenticationConverter();
        jwtAuthConverter.setJwtGrantedAuthoritiesConverter(
            new KeycloakRoleConverter()
        );
        
        return jwtAuthConverter;
    }
}
```

**Autorisation au niveau méthode** :
```java
@RestController
@RequestMapping("/api/accounts")
public class AccountController {
    
    @Autowired
    private AccountService accountService;
    
    /**
     * Endpoint accessible par USER et ADMIN
     */
    @GetMapping("/myaccount")
    @PreAuthorize("hasAnyRole('USER', 'ADMIN')")
    public ResponseEntity<AccountDto> getMyAccount(@AuthenticationPrincipal Jwt jwt) {
        String username = jwt.getSubject();
        AccountDto account = accountService.getAccountByUsername(username);
        return ResponseEntity.ok(account);
    }
    
    /**
     * Endpoint accessible uniquement par ADMIN
     */
    @GetMapping("/all")
    @PreAuthorize("hasRole('ADMIN')")
    public ResponseEntity<List<AccountDto>> getAllAccounts() {
        List<AccountDto> accounts = accountService.getAllAccounts();
        return ResponseEntity.ok(accounts);
    }
    
    /**
     * Endpoint avec vérification custom
     */
    @PutMapping("/{accountId}")
    @PreAuthorize("hasRole('ADMIN') or @accountSecurityService.isOwner(#accountId, authentication)")
    public ResponseEntity<AccountDto> updateAccount(
        @PathVariable Long accountId,
        @RequestBody AccountDto accountDto) {
        
        AccountDto updated = accountService.updateAccount(accountId, accountDto);
        return ResponseEntity.ok(updated);
    }
    
    /**
     * Suppression - ADMIN uniquement
     */
    @DeleteMapping("/{accountId}")
    @PreAuthorize("hasRole('ADMIN')")
    public ResponseEntity<Void> deleteAccount(@PathVariable Long accountId) {
        accountService.deleteAccount(accountId);
        return ResponseEntity.noContent().build();
    }
}
```

**Service de sécurité personnalisé** :
```java
@Service("accountSecurityService")
public class AccountSecurityService {
    
    @Autowired
    private AccountRepository accountRepository;
    
    /**
     * Vérifie si l'utilisateur est propriétaire du compte
     */
    public boolean isOwner(Long accountId, Authentication authentication) {
        if (authentication == null || !(authentication.getPrincipal() instanceof Jwt)) {
            return false;
        }
        
        Jwt jwt = (Jwt) authentication.getPrincipal();
        String username = jwt.getSubject();
        
        return accountRepository.findById(accountId)
            .map(account -> account.getUsername().equals(username))
            .orElse(false);
    }
    
    /**
     * Vérifie si l'utilisateur a un rôle spécifique
     */
    public boolean hasRole(Authentication authentication, String role) {
        return authentication.getAuthorities().stream()
            .anyMatch(grantedAuthority -> 
                grantedAuthority.getAuthority().equals("ROLE_" + role)
            );
    }
}
```

---

### Monitoring et Logging de Sécurité

**Audit Logging pour les accès** :
```java
@Aspect
@Component
public class SecurityAuditAspect {
    
    private static final Logger log = LoggerFactory.getLogger(SecurityAuditAspect.class);
    
    @Autowired
    private AuditLogRepository auditLogRepository;
    
    /**
     * Log tous les accès aux endpoints sécurisés
     */
    @Around("@annotation(preAuthorize)")
    public Object logSecuredMethodAccess(
        ProceedingJoinPoint joinPoint, 
        PreAuthorize preAuthorize) throws Throwable {
        
        Authentication auth = SecurityContextHolder.getContext().getAuthentication();
        
        if (auth != null && auth.getPrincipal() instanceof Jwt) {
            Jwt jwt = (Jwt) auth.getPrincipal();
            
            String username = jwt.getSubject();
            String method = joinPoint.getSignature().getName();
            String className = joinPoint.getTarget().getClass().getSimpleName();
            
            List<String> roles = auth.getAuthorities().stream()
                .map(GrantedAuthority::getAuthority)
                .collect(Collectors.toList());
            
            log.info("Security Access - User: {}, Roles: {}, Method: {}.{}", 
                username, roles, className, method);
            
            // Sauvegarder dans DB pour audit
            AuditLog auditLog = AuditLog.builder()
                .username(username)
                .action(method)
                .resource(className)
                .roles(String.join(",", roles))
                .timestamp(Instant.now())
                .build();
            
            auditLogRepository.save(auditLog);
        }
        
        try {
            return joinPoint.proceed();
        } catch (AccessDeniedException e) {
            log.warn("Access Denied - User: {}, Method: {}", 
                auth != null ? auth.getName() : "anonymous", 
                joinPoint.getSignature()
            );
            throw e;
        }
    }
}
```

**Custom Access Denied Handler** :
```java
@Component
public class CustomAccessDeniedHandler implements AccessDeniedHandler {
    
    private static final Logger log = LoggerFactory.getLogger(CustomAccessDeniedHandler.class);
    
    @Override
    public void handle(
        HttpServletRequest request,
        HttpServletResponse response,
        AccessDeniedException accessDeniedException) throws IOException {
        
        Authentication auth = SecurityContextHolder.getContext().getAuthentication();
        
        if (auth != null) {
            log.warn("User: {} attempted to access protected resource: {} - Access Denied",
                auth.getName(), request.getRequestURI());
        }
        
        response.setStatus(HttpServletResponse.SC_FORBIDDEN);
        response.setContentType(MediaType.APPLICATION_JSON_VALUE);
        
        Map<String, Object> errorDetails = Map.of(
            "error", "Forbidden",
            "message", "You don't have permission to access this resource",
            "path", request.getRequestURI(),
            "timestamp", Instant.now().toString()
        );
        
        ObjectMapper mapper = new ObjectMapper();
        response.getWriter().write(mapper.writeValueAsString(errorDetails));
    }
}
```

**Métriques Prometheus pour sécurité** :
```java
@Component
public class SecurityMetrics {
    
    private final Counter authenticationAttempts;
    private final Counter authenticationFailures;
    private final Counter accessDenied;
    private final Counter tokenValidationErrors;
    
    public SecurityMetrics(MeterRegistry registry) {
        this.authenticationAttempts = Counter.builder("security.authentication.attempts")
            .description("Total authentication attempts")
            .register(registry);
        
        this.authenticationFailures = Counter.builder("security.authentication.failures")
            .description("Failed authentication attempts")
            .register(registry);
        
        this.accessDenied = Counter.builder("security.access.denied")
            .description("Access denied count")
            .tag("endpoint", "unknown")
            .register(registry);
        
        this.tokenValidationErrors = Counter.builder("security.token.validation.errors")
            .description("JWT token validation errors")
            .register(registry);
    }
    
    public void recordAuthenticationAttempt() {
        authenticationAttempts.increment();
    }
    
    public void recordAuthenticationFailure() {
        authenticationFailures.increment();
    }
    
    public void recordAccessDenied(String endpoint) {
        Counter.builder("security.access.denied")
            .tag("endpoint", endpoint)
            .register(new SimpleMeterRegistry())
            .increment();
    }
    
    public void recordTokenValidationError() {
        tokenValidationErrors.increment();
    }
}
```

---

### Tests de Sécurité

**Tests d'intégration pour Authorization** :
```java
@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
@AutoConfigureMockMvc
public class SecurityIntegrationTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    /**
     * Test endpoint public accessible sans token
     */
    @Test
    public void testPublicEndpoint_NoAuth_Success() throws Exception {
        mockMvc.perform(get("/actuator/health"))
            .andExpect(status().isOk());
    }
    
    /**
     * Test endpoint protégé sans token - doit retourner 401
     */
    @Test
    public void testProtectedEndpoint_NoAuth_Returns401() throws Exception {
        mockMvc.perform(get("/api/accounts/myaccount"))
            .andExpect(status().isUnauthorized());
    }
    
    /**
     * Test endpoint protégé avec token valide USER - doit réussir
     */
    @Test
    @WithMockJwt(subject = "john.doe", roles = {"USER"})
    public void testProtectedEndpoint_ValidUserToken_Success() throws Exception {
        mockMvc.perform(get("/api/accounts/myaccount"))
            .andExpect(status().isOk());
    }
    
    /**
     * Test endpoint ADMIN avec token USER - doit retourner 403
     */
    @Test
    @WithMockJwt(subject = "john.doe", roles = {"USER"})
    public void testAdminEndpoint_UserToken_Returns403() throws Exception {
        mockMvc.perform(get("/api/accounts/all"))
            .andExpect(status().isForbidden());
    }
    
    /**
     * Test endpoint ADMIN avec token ADMIN - doit réussir
     */
    @Test
    @WithMockJwt(subject = "admin", roles = {"ADMIN"})
    public void testAdminEndpoint_AdminToken_Success() throws Exception {
        mockMvc.perform(get("/api/accounts/all"))
            .andExpect(status().isOk());
    }
    
    /**
     * Test avec token JWT expiré - doit retourner 401
     */
    @Test
    public void testProtectedEndpoint_ExpiredToken_Returns401() throws Exception {
        String expiredToken = generateExpiredJWT();
        
        mockMvc.perform(get("/api/accounts/myaccount")
                .header("Authorization", "Bearer " + expiredToken))
            .andExpect(status().isUnauthorized());
    }
    
    /**
     * Test avec token JWT invalide - doit retourner 401
     */
    @Test
    public void testProtectedEndpoint_InvalidToken_Returns401() throws Exception {
        mockMvc.perform(get("/api/accounts/myaccount")
                .header("Authorization", "Bearer invalid.jwt.token"))
            .andExpect(status().isUnauthorized());
    }
}
```

**Annotation personnalisée pour Mock JWT** :
```java
@Retention(RetentionPolicy.RUNTIME)
@WithSecurityContext(factory = WithMockJwtSecurityContextFactory.class)
public @interface WithMockJwt {
    String subject() default "user";
    String[] roles() default {"USER"};
    String email() default "user@example.com";
}

public class WithMockJwtSecurityContextFactory 
    implements WithSecurityContextFactory<WithMockJwt> {
    
    @Override
    public SecurityContext createSecurityContext(WithMockJwt annotation) {
        SecurityContext context = SecurityContextHolder.createEmptyContext();
        
        Map<String, Object> headers = new HashMap<>();
        headers.put("alg", "RS256");
        headers.put("typ", "JWT");
        
        Map<String, Object> claims = new HashMap<>();
        claims.put("sub", annotation.subject());
        claims.put("email", annotation.email());
        claims.put("preferred_username", annotation.subject());
        
        Map<String, Object> realmAccess = new HashMap<>();
        realmAccess.put("roles", Arrays.asList(annotation.roles()));
        claims.put("realm_access", realmAccess);
        
        Jwt jwt = new Jwt(
            "mock-token",
            Instant.now(),
            Instant.now().plusSeconds(3600),
            headers,
            claims
        );
        
        Collection<GrantedAuthority> authorities = Arrays.stream(annotation.roles())
            .map(role -> new SimpleGrantedAuthority("ROLE_" + role))
            .collect(Collectors.toList());
        
        JwtAuthenticationToken auth = new JwtAuthenticationToken(jwt, authorities);
        context.setAuthentication(auth);
        
        return context;
    }
}
```

---

## Résultats & Impact

### Sécurité & Conformité
✅ **Authentication centralisée** : Single Sign-On (SSO) avec Keycloak pour tous les microservices  
✅ **Authorization granulaire** : RBAC au niveau Gateway et microservices avec contrôle fin par rôle  
✅ **Token-based security** : JWT stateless pour scalabilité et performance  
✅ **Standards OAuth2/OIDC** : Conformité aux standards de l'industrie  
✅ **Audit complet** : Logging de tous les accès et tentatives d'accès non autorisées

### Performance & Scalabilité
✅ **Stateless authentication** : Pas de session serveur, scalabilité horizontale illimitée  
✅ **Validation locale JWT** : Pas d'appel à Keycloak pour chaque requête (vérification signature locale)  
✅ **Caching des clés publiques** : JWK Set caché pour performance optimale  
✅ **Token Relay** : Propagation transparente du contexte de sécurité entre microservices

### Expérience Développeur
✅ **Configuration déclarative** : Sécurité définie en annotations et YAML  
✅ **Tests automatisés** : Suite complète de tests de sécurité avec Mock JWT  
✅ **Documentation OpenAPI** : Swagger UI intégré avec authentication OAuth2  
✅ **Developer-friendly** : Configuration locale simple avec Docker Compose

---

## Technologies & Standards Maîtrisés

**Security Frameworks** : Spring Security, Spring OAuth2 Resource Server, Spring OAuth2 Client  
**Identity Provider** : Keycloak (open-source IAM)  
**Standards** : OAuth 2.0, OpenID Connect (OIDC), JWT (RFC 7519)  
**Grant Types** : Authorization Code, Client Credentials, Refresh Token  
**Authorization** : RBAC (Role-Based Access Control), Method-level security  
**API Gateway** : Spring Cloud Gateway (reactive security)  
**Monitoring** : Security audit logs, Prometheus metrics, distributed tracing

---

## Best Practices Appliquées

```yaml
✅ Principle of Least Privilege (rôles minimaux requis)
✅ Defense in Depth (sécurité multi-couches: Gateway + Microservices)
✅ Fail Secure (deny by default, explicit allow)
✅ Token expiration courts (5-15 minutes) avec refresh tokens
✅ HTTPS/TLS obligatoire en production
✅ CORS configuré strictement
✅ Rate limiting pour prévenir attaques brute-force
✅ Security headers (X-Frame-Options, CSP, etc.)
✅ Audit logging complet pour compliance
✅ Secrets externalisés (jamais en dur dans code)
✅ Regular security testing (OWASP Top 10)
```

---

# Questions/Réponses Techniques - Sécurité Microservices avec OAuth2 & Keycloak

## 1. Concepts Fondamentaux OAuth2 & Keycloak

### Q1: Expliquez la différence entre Authentication et Authorization. Comment OAuth2 gère-t-il ces deux aspects ?
**R:**

**Authentication (Authentification)** :
- **Définition** : Vérifier l'identité de l'utilisateur ("Qui êtes-vous ?")
- **Exemple** : Login avec username/password, vérification des credentials
- **Résultat** : Confirmation que l'utilisateur est bien celui qu'il prétend être

**Authorization (Autorisation)** :
- **Définition** : Déterminer ce que l'utilisateur peut faire ("Que pouvez-vous faire ?")
- **Exemple** : Vérifier si l'utilisateur a le rôle ADMIN pour supprimer un compte
- **Résultat** : Permission ou refus d'accès à une ressource

**OAuth2 et ces concepts** :
```
OAuth2 est principalement un framework d'AUTHORIZATION, pas d'authentication !

Authentication → Géré par OpenID Connect (OIDC) qui est une couche au-dessus d'OAuth2
Authorization → Géré directement par OAuth2 (scopes, tokens)

Flow complet:
1. Authentication (OIDC): 
   - User se connecte à Keycloak
   - Keycloak valide credentials
   - Keycloak retourne ID Token (contient infos user)

2. Authorization (OAuth2):
   - Keycloak retourne Access Token (contient scopes/roles)
   - Client utilise Access Token pour accéder aux ressources
   - Resource Server valide le token et vérifie les permissions
```

**Dans notre architecture** :
```java
// Authentication - Qui est l'utilisateur ?
@GetMapping("/user-info")
public Mono<UserInfo> getUserInfo(@AuthenticationPrincipal Jwt jwt) {
    String username = jwt.getSubject();  // Authentication
    String email = jwt.getClaim("email");
    return Mono.just(new UserInfo(username, email));
}

// Authorization - Que peut-il faire ?
@DeleteMapping("/accounts/{id}")
@PreAuthorize("hasRole('ADMIN')")  // Authorization
public ResponseEntity<Void> deleteAccount(@PathVariable Long id) {
    accountService.delete(id);
    return ResponseEntity.noContent().build();
}
```

---

### Q2: Qu'est-ce qu'un JWT (JSON Web Token) et comment fonctionne-t-il ? Quelles sont ses parties ?
**R:**

**JWT Structure** :
```
Un JWT est composé de 3 parties séparées par des points (.):
HEADER.PAYLOAD.SIGNATURE

Exemple:
eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJqb2huLmRvZSIsInJvbGVzIjpbIlVTRVIiXX0.signature
```

**1. Header (Base64URL encodé)** :
```json
{
  "alg": "RS256",       // Algorithme de signature (RSA SHA-256)
  "typ": "JWT",         // Type de token
  "kid": "key-id-123"   // Key ID (quelle clé publique utiliser)
}
```

**2. Payload (Base64URL encodé - Claims)** :
```json
{
  // Claims standards (registered)
  "iss": "http://localhost:8080/realms/microservices-realm",  // Issuer
  "sub": "john.doe",                                          // Subject (user ID)
  "aud": "api-gateway",                                       // Audience
  "exp": 1735889400,                                          // Expiration timestamp
  "iat": 1735885800,                                          // Issued at
  "nbf": 1735885800,                                          // Not before
  
  // Claims personnalisés Keycloak
  "email": "john.doe@example.com",
  "preferred_username": "john.doe",
  "given_name": "John",
  "family_name": "Doe",
  
  // Rôles Keycloak
  "realm_access": {
    "roles": ["USER", "MANAGER"]
  },
  
  // Scopes OAuth2
  "scope": "openid profile email"
}
```

**3. Signature (cryptographique)** :
```javascript
// Création de la signature (côté Keycloak)
signature = RSA_SHA256(
  base64UrlEncode(header) + "." + base64UrlEncode(payload),
  privateKey  // Clé privée de Keycloak (secrète)
)

// Vérification de la signature (côté Gateway/Microservices)
isValid = RSA_SHA256_VERIFY(
  base64UrlEncode(header) + "." + base64UrlEncode(payload),
  signature,
  publicKey  // Clé publique de Keycloak (disponible via JWK Set)
)
```

**Avantages JWT** :
```yaml
✅ Stateless: Pas besoin de stocker les sessions côté serveur
✅ Self-contained: Toutes les infos nécessaires sont dans le token
✅ Scalable: Chaque instance peut valider indépendamment
✅ Interopérable: Standard ouvert (RFC 7519)
✅ Performance: Validation locale sans appel à Keycloak

⚠️ Attention:
❌ Token non révocable avant expiration (use refresh tokens)
❌ Taille importante si beaucoup de claims
❌ Visible en clair (Base64 ≠ encryption) - ne pas mettre de secrets
```

**Validation JWT dans Spring Boot** :
```java
@Service
public class JwtValidationService {
    
    public boolean validateJwt(String token) {
        try {
            Jwt jwt = jwtDecoder.decode(token);
            
            // 1. Vérifier la signature (automatique par JwtDecoder)
            // 2. Vérifier l'expiration
            if (jwt.getExpiresAt().isBefore(Instant.now())) {
                return false;  // Token expiré
            }
            
            // 3. Vérifier l'issuer
            String issuer = jwt.getIssuer().toString();
            if (!issuer.equals("http://localhost:8080/realms/microservices-realm")) {
                return false;  // Mauvais issuer
            }
            
            // 4. Vérifier l'audience
            List<String> audience = jwt.getAudience();
            if (!audience.contains("api-gateway")) {
                return false;  // Token pas destiné à nous
            }
            
            return true;
            
        } catch (JwtException e) {
            log.error("JWT validation failed: {}", e.getMessage());
            return false;
        }
    }
}
```

---

### Q3: Quelle est la différence entre Access Token et Refresh Token ? Pourquoi utiliser les deux ?
**R:**

**Access Token** :
```yaml
Durée de vie: Court (5-15 minutes généralement)
Usage: Accéder aux ressources protégées (API calls)
Stockage: Mémoire (jamais localStorage en production)
Révocation: Difficile (attendre expiration)
Contenu: JWT avec claims utilisateur et rôles
```

**Refresh Token** :
```yaml
Durée de vie: Long (jours/semaines/mois)
Usage: Obtenir un nouveau Access Token
Stockage: HttpOnly cookie sécurisé
Révocation: Possible (stocké côté serveur dans Keycloak)
Contenu: Opaque token (UUID généralement)
```

**Pourquoi les deux ?**

**Sécurité** :
```
Access Token court = Fenêtre d'exposition limitée si volé
Refresh Token long = Confort utilisateur (pas de re-login fréquent)

Compromis:
- Si Access Token volé → Valide seulement 5-15 min
- Si Refresh Token volé → Peut être révoqué côté serveur
```

**Flow complet avec les deux tokens** :
```
1. Login initial:
   User → Keycloak (credentials)
   Keycloak → Client (Access Token + Refresh Token)

2. Appel API normal:
   Client → Gateway (Access Token)
   Gateway → Valide token localement
   Gateway → Microservice (token valide)

3. Access Token expiré:
   Client → Gateway (Access Token expiré)
   Gateway → 401 Unauthorized
   Client → Keycloak (Refresh Token)
   Keycloak → Valide Refresh Token
   Keycloak → Client (Nouveau Access Token)
   Client → Gateway (Nouveau Access Token)
   Gateway → Microservice (succès)

4. Refresh Token expiré ou révoqué:
   Client → Keycloak (Refresh Token expiré)
   Keycloak → 401 Unauthorized
   Client → Redirect to Login
```

**Implémentation côté client** :
```javascript
class TokenManager {
  constructor() {
    this.accessToken = null;
    this.refreshToken = null;
  }
  
  // Appel API avec refresh automatique
  async callAPI(url, options = {}) {
    // 1. Essayer avec Access Token actuel
    let response = await fetch(url, {
      ...options,
      headers: {
        ...options.headers,
        'Authorization': `Bearer ${this.accessToken}`
      }
    });
    
    // 2. Si 401, refresh et réessayer
    if (response.status === 401) {
      console.log('Access Token expired, refreshing...');
      
      const refreshed = await this.refreshAccessToken();
      
      if (!refreshed) {
        // Refresh failed, redirect to login
        window.location.href = '/login';
        return;
      }
      
      // Réessayer avec nouveau token
      response = await fetch(url, {
        ...options,
        headers: {
          ...options.headers,
          'Authorization': `Bearer ${this.accessToken}`
        }
      });
    }
    
    return response;
  }
  
  // Refresh l'Access Token
  async refreshAccessToken() {
    try {
      const response = await fetch('http://localhost:8072/auth/refresh', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          refresh_token: this.refreshToken
        }),
        credentials: 'include'  // Envoyer cookies
      });
      
      if (response.ok) {
        const data = await response.json();
        this.accessToken = data.access_token;
        return true;
      }
      
      return false;
      
    } catch (error) {
      console.error('Refresh failed:', error);
      return false;
    }
  }
}
```

**Implémentation côté Gateway** :
```java
@RestController
@RequestMapping("/auth")
public class TokenRefreshController {
    
    @Autowired
    private WebClient webClient;
    
    @Value("${keycloak.token-uri}")
    private String keycloakTokenUri;
    
    @Value("${keycloak.client-id}")
    private String clientId;
    
    @Value("${keycloak.client-secret}")
    private String clientSecret;
    
    /**
     * Endpoint pour refresh l'Access Token
     */
    @PostMapping("/refresh")
    public Mono<TokenResponse> refreshToken(@RequestBody RefreshTokenRequest request) {
        
        MultiValueMap<String, String> formData = new LinkedMultiValueMap<>();
        formData.add("grant_type", "refresh_token");
        formData.add("refresh_token", request.getRefreshToken());
        formData.add("client_id", clientId);
        formData.add("client_secret", clientSecret);
        
        return webClient.post()
            .uri(keycloakTokenUri)
            .contentType(MediaType.APPLICATION_FORM_URLENCODED)
            .body(BodyInserters.fromFormData(formData))
            .retrieve()
            .onStatus(HttpStatusCode::is4xxClientError, response -> {
                log.error("Refresh token invalid or expired");
                return Mono.error(new UnauthorizedException("Invalid refresh token"));
            })
            .bodyToMono(KeycloakTokenResponse.class)
            .map(keycloakResponse -> TokenResponse.builder()
                .accessToken(keycloakResponse.getAccessToken())
                .expiresIn(keycloakResponse.getExpiresIn())
                .refreshExpiresIn(keycloakResponse.getRefreshExpiresIn())
                .tokenType("Bearer")
                .build()
            );
    }
}
```

**Best Practices** :
```yaml
✅ Access Token: 5-15 minutes max
✅ Refresh Token: 1-7 jours selon sensibilité application
✅ Refresh Token stocké en HttpOnly cookie (pas accessible par JS)
✅ Refresh Token rotation: nouveau refresh token à chaque refresh
✅ Détection token replay: invalider ancien refresh après usage
✅ Limite nombre de refresh: max 10 refresh par session
❌ JAMAIS stocker Access/Refresh Token dans localStorage (XSS risk)
```

---

## 2. Authorization Code Flow

### Q4: Expliquez le flow complet Authorization Code avec PKCE. Pourquoi est-ce le flow le plus sécurisé ?
**R:**

**Authorization Code Flow avec PKCE (Proof Key for Code Exchange)** :

**Étape par étape détaillée** :
```
1. Client génère code_verifier et code_challenge
   code_verifier = random_string_43_128_chars()
   code_challenge = BASE64URL(SHA256(code_verifier))

2. User clique "Login"
   Frontend → Redirect vers Keycloak avec:
   GET /auth?
     response_type=code
     &client_id=my-app
     &redirect_uri=http://localhost:3000/callback
     &scope=openid profile email
     &state=random_state_value
     &code_challenge=abc123...
     &code_challenge_method=S256

3. User entre credentials dans Keycloak
   Keycloak → Valide username/password
   Keycloak → Génère Authorization Code

4. Keycloak redirect vers application
   Keycloak → Frontend avec:
   http://localhost:3000/callback?
     code=AUTH_CODE_XYZ
     &state=random_state_value

5. Frontend valide state (prévient CSRF)
   if (received_state !== sent_state) reject()

6. Frontend envoie code au Backend
   Frontend → Backend Gateway:
   POST /auth/token
   {
     "code": "AUTH_CODE_XYZ",
     "code_verifier": "original_verifier"
   }

7. Backend échange code contre tokens
   Backend → Keycloak:
   POST /token
   {
     "grant_type": "authorization_code",
     "code": "AUTH_CODE_XYZ",
     "redirect_uri": "http://localhost:3000/callback",
     "client_id": "my-app",
     "client_secret": "secret",
     "code_verifier": "original_verifier"
   }

8. Keycloak valide et retourne tokens
   Keycloak vérifie:
   - code valide et non utilisé
   - code_verifier correspond au code_challenge
   - client_id et client_secret valides
   - redirect_uri correspond
   
   Keycloak → Backend:
   {
     "access_token": "eyJhbGc...",
     "refresh_token": "eyJhbGc...",
     "id_token": "eyJhbGc...",
     "token_type": "Bearer",
     "expires_in": 300
   }

9. Backend stocke tokens et crée session
   Backend → Frontend:
   Set-Cookie: SESSION=secure_session_id; HttpOnly; Secure; SameSite=Strict

10. User est authentifié
    Frontend peut maintenant appeler APIs avec session
```

**Pourquoi c'est le plus sécurisé ?**

**1. Authorization Code jamais exposé au frontend** :
```
Mauvais (Implicit Flow):
Keycloak → Frontend avec Access Token direct dans URL
https://app.com/callback#access_token=SECRET_TOKEN
❌ Token visible dans browser history
❌ Token accessible par JavaScript
❌ Risque XSS

Bon (Authorization Code):
Keycloak → Frontend avec Code temporaire
https://app.com/callback?code=TEMP_CODE
Backend → Keycloak pour échanger Code contre Token
✅ Token jamais exposé au browser
✅ Code utilisable 1 seule fois (10 secondes max)
```

**2. PKCE protège contre interception** :
```
Sans PKCE:
Attacker intercepte code → Peut l'utiliser si connaît client_secret

Avec PKCE:
Attacker intercepte code → Ne peut pas l'utiliser sans code_verifier
code_verifier jamais transmis sur le réseau (sauf dans échange final)
```

**3. Client Secret côté backend** :
```
client_secret jamais exposé au frontend
Seul le backend connaît le secret
Impossible pour attacker de voler le secret depuis le browser
```

**4. State parameter (CSRF protection)** :
```java
// Génération state
String state = UUID.randomUUID().toString();
session.setAttribute("oauth_state", state);

// Redirection vers Keycloak
return "redirect:" + keycloakAuthUrl + 
       "?response_type=code" +
       "&client_id=" + clientId +
       "&redirect_uri=" + redirectUri +
       "&scope=openid profile email" +
       "&state=" + state +
       "&code_challenge=" + codeChallenge +
       "&code_challenge_method=S256";

// Callback validation
@GetMapping("/callback")
public String callback(@RequestParam String code, 
                      @RequestParam String state,
                      HttpSession session) {
    String expectedState = (String) session.getAttribute("oauth_state");
    
    if (!state.equals(expectedState)) {
        throw new SecurityException("Invalid state - CSRF detected!");
    }
    
    // Exchange code for tokens...
}
```

**Implémentation complète avec PKCE** :
```java
@Service
public class PKCEService {
    
    private static final SecureRandom SECURE_RANDOM = new SecureRandom();
    
    /**
     * Génère un code_verifier aléatoire
     */
    public String generateCodeVerifier() {
        byte[] codeVerifier = new byte[32];
        SECURE_RANDOM.nextBytes(codeVerifier);
        return Base64.getUrlEncoder()
            .withoutPadding()
            .encodeToString(codeVerifier);
    }
    
    /**
     * Génère code_challenge depuis code_verifier
     */
    public String generateCodeChallenge(String codeVerifier) {
        try {
            byte[] bytes = codeVerifier.getBytes(StandardCharsets.US_ASCII);
            MessageDigest md = MessageDigest.getInstance("SHA-256");
            md.update(bytes);
            byte[] digest = md.digest();
            return Base64.getUrlEncoder()
                .withoutPadding()
                .encodeToString(digest);
        } catch (NoSuchAlgorithmException e) {
            throw new RuntimeException("SHA-256 not available", e);
        }
    }
}

@RestController
@RequestMapping("/auth")
public class OAuth2Controller {
    
    @Autowired
    private PKCEService pkceService;
    
    @Autowired
    private WebClient webClient;
    
    /**
     * Initie le flow Authorization Code avec PKCE
     */
    @GetMapping("/login")
    public ResponseEntity<Void> login(HttpSession session) {
        // 1. Générer PKCE
        String codeVerifier = pkceService.generateCodeVerifier();
        String codeChallenge = pkceService.generateCodeChallenge(codeVerifier);
        
        // 2. Générer state (CSRF protection)
        String state = UUID.randomUUID().toString();
        
        // 3. Stocker en session
        session.setAttribute("code_verifier", codeVerifier);
        session.setAttribute("oauth_state", state);
        
        // 4. Construire URL Keycloak
        String authUrl = UriComponentsBuilder
            .fromHttpUrl("http://localhost:8080/realms/microservices-realm/protocol/openid-connect/auth")
            .queryParam("response_type", "code")
            .queryParam("client_id", "api-gateway")
            .queryParam("redirect_uri", "http://localhost:8072/auth/callback")
            .queryParam("scope", "openid profile email")
            .queryParam("state", state)
            .queryParam("code_challenge", codeChallenge)
            .queryParam("code_challenge_method", "S256")
            .build()
            .toUriString();
        
        // 5. Redirect
        return ResponseEntity
            .status(HttpStatus.FOUND)
            .location(URI.create(authUrl))
            .build();
    }
    
    /**
     * Callback après authentication Keycloak
     */
    @GetMapping("/callback")
    public ResponseEntity<Void> callback(
        @RequestParam String code,
        @RequestParam String state,
        HttpSession session) {
        
        // 1. Valider state (CSRF)
        String expectedState = (String) session.getAttribute("oauth_state");
        if (!state.equals(expectedState)) {
            throw new SecurityException("Invalid state parameter - CSRF detected");
        }
        
        // 2. Récupérer code_verifier
        String codeVerifier = (String) session.getAttribute("code_verifier");
        
        // 3. Échanger code contre tokens
        MultiValueMap<String, String> formData = new LinkedMultiValueMap<>();
        formData.add("grant_type", "authorization_code");
        formData.add("code", code);
        formData.add("redirect_uri", "http://localhost:8072/auth/callback");
        formData.add("client_id", "api-gateway");
        formData.add("client_secret", clientSecret);
        formData.add("code_verifier", codeVerifier);
        
        TokenResponse tokenResponse = webClient.post()
            .uri("http://localhost:8080/realms/microservices-realm/protocol/openid-connect/token")
            .contentType(MediaType.APPLICATION_FORM_URLENCODED)
            .body(BodyInserters.fromFormData(formData))
            .retrieve()
            .bodyToMono(TokenResponse.class)
            .block();
        
        // 4. Stocker tokens en session
        session.setAttribute("access_token", tokenResponse.getAccessToken());
        session.setAttribute("refresh_token", tokenResponse.getRefreshToken());
        session.setAttribute("id_token", tokenResponse.getIdToken());
        
        // 5. Nettoyer attributs temporaires
        session.removeAttribute("code_verifier");
        session.removeAttribute("oauth_state");
        
        // 6. Redirect vers application
        return ResponseEntity
            .status(HttpStatus.FOUND)
            .location(URI.create("/home"))
            .build();
    }
}
```

**Comparaison des flows** :
```
| Flow                    | Sécurité | Use Case                    |
|------------------------|----------|------------------------------|
| Authorization Code+PKCE | ⭐⭐⭐⭐⭐ | SPA, Mobile apps (moderne)   |
| Authorization Code      | ⭐⭐⭐⭐   | Web apps traditionnelles     |
| Client Credentials      | ⭐⭐⭐⭐   | Server-to-server (M2M)       |
| Resource Owner Password | ⭐⭐      | Legacy (déprécié)            |
| Implicit                | ⭐       | Déprécié (insécure)          |
```

---

## 3. Role-Based Access Control (RBAC)

### Q5: Comment implémenter une autorisation fine avec des rôles hiérarchiques ? Par exemple ADMIN > MANAGER > USER
**R:**

**Approche 1 : Configuration Keycloak avec rôles composites** :
```
Dans Keycloak Admin Console:

1. Créer les rôles de base:
   - USER (rôle de base)
   - MANAGER (contient USER)
   - ADMIN (contient MANAGER et USER)

2. Configurer rôles composites:
   ADMIN → Composite Roles → Add "MANAGER" et "USER"
   MANAGER → Composite Roles → Add "USER"

Résultat:
- User avec rôle ADMIN → a automatiquement MANAGER et USER
- User avec rôle MANAGER → a automatiquement USER
- User avec rôle USER → a seulement USER
```

**Approche 2 : Role Hierarchy dans Spring Security** :
```java
@Configuration
@EnableMethodSecurity
public class SecurityConfig {
    
    @Bean
    public RoleHierarchy roleHierarchy() {
        RoleHierarchyImpl roleHierarchy = new RoleHierarchyImpl();
        
        // Définir la hiérarchie
        // Format: ROLE_PARENT > ROLE_CHILD
        String hierarchy = "ROLE_ADMIN > ROLE_MANAGER \n" +
                          "ROLE_MANAGER > ROLE_USER";
        
        roleHierarchy.setHierarchy(hierarchy);
        return roleHierarchy;
    }
    
    @Bean
    public MethodSecurityExpressionHandler methodSecurityExpressionHandler(
        RoleHierarchy roleHierarchy) {
        
        DefaultMethodSecurityExpressionHandler expressionHandler = 
            new DefaultMethodSecurityExpressionHandler();
        
        expressionHandler.setRoleHierarchy(roleHierarchy);
        return expressionHandler;
    }
}
```

**Utilisation dans les controllers** :
```java
@RestController
@RequestMapping("/api/accounts")
public class AccountController {
    
    /**
     * Accessible par USER, MANAGER et ADMIN (grâce à hiérarchie)
     */
    @GetMapping("/my-account")
    @PreAuthorize("hasRole('USER')")
    public ResponseEntity<AccountDto> getMyAccount(@AuthenticationPrincipal Jwt jwt) {
        String username = jwt.getSubject();
        return ResponseEntity.ok(accountService.getByUsername(username));
    }
    
    /**
     * Accessible par MANAGER et ADMIN uniquement
     */
    @GetMapping("/team-accounts")
    @PreAuthorize("hasRole('MANAGER')")
    public ResponseEntity<List<AccountDto>> getTeamAccounts() {
        return ResponseEntity.ok(accountService.getTeamAccounts());
    }
    
    /**
     * Accessible par ADMIN uniquement
     */
    @DeleteMapping("/{id}")
    @PreAuthorize("hasRole('ADMIN')")
    public ResponseEntity<Void> deleteAccount(@PathVariable Long id) {
        accountService.delete(id);
        return ResponseEntity.noContent().build();
    }
    
    /**
     * Autorisation conditionnelle complexe
     */
    @PutMapping("/{id}")
    @PreAuthorize("hasRole('ADMIN') or " +
                  "(hasRole('MANAGER') and @accountSecurityService.isSameTeam(#id, authentication)) or " +
                  "(hasRole('USER') and @accountSecurityService.isOwner(#id, authentication))")
    public ResponseEntity<AccountDto> updateAccount(
        @PathVariable Long id,
        @RequestBody AccountDto dto) {
        
        AccountDto updated = accountService.update(id, dto);
        return ResponseEntity.ok(updated);
    }
}
```

**Service de sécurité avec logique métier** :
```java
@Service("accountSecurityService")
public class AccountSecurityService {
    
    @Autowired
    private AccountRepository accountRepository;
    
    @Autowired
    private TeamRepository teamRepository;
    
    /**
     * Vérifie si l'utilisateur est propriétaire du compte
     */
    public boolean isOwner(Long accountId, Authentication auth) {
        if (auth == null || !(auth.getPrincipal() instanceof Jwt)) {
            return false;
        }
        
        Jwt jwt = (Jwt) auth.getPrincipal();
        String username = jwt.getSubject();
        
        return accountRepository.findById(accountId)
            .map(account -> account.getUsername().equals(username))
            .orElse(false);
    }
    
    /**
     * Vérifie si l'utilisateur est dans la même team
     */
    public boolean isSameTeam(Long accountId, Authentication auth) {
        if (auth == null || !(auth.getPrincipal() instanceof Jwt)) {
            return false;
        }
        
        Jwt jwt = (Jwt) auth.getPrincipal();
        String username = jwt.getSubject();
        
        Optional<Account> targetAccount = accountRepository.findById(accountId);
        Optional<Account> currentAccount = accountRepository.findByUsername(username);
        
        if (targetAccount.isEmpty() || currentAccount.isEmpty()) {
            return false;
        }
        
        return targetAccount.get().getTeamId()
            .equals(currentAccount.get().getTeamId());
    }
    
    /**
     * Vérifie si l'utilisateur peut voir les données sensibles
     */
    public boolean canViewSensitiveData(Authentication auth) {
        return hasAnyRole(auth, "ADMIN", "MANAGER");
    }
    
    /**
     * Helper pour vérifier multiple rôles
     */
    private boolean hasAnyRole(Authentication auth, String... roles) {
        return Arrays.stream(roles)
            .anyMatch(role -> auth.getAuthorities().stream()
                .anyMatch(grantedAuth -> 
                    grantedAuth.getAuthority().equals("ROLE_" + role)
                )
            );
    }
}
```

**Filter les données selon le rôle** :
```java
@Service
public class AccountService {
    
    @Autowired
    private AccountSecurityService securityService;
    
    /**
     * Retourne les données avec filtrage selon rôle
     */
    public AccountDto getAccount(Long id, Authentication auth) {
        Account account = accountRepository.findById(id)
            .orElseThrow(() -> new NotFoundException("Account not found"));
        
        AccountDto dto = new AccountDto();
        dto.setId(account.getId());
        dto.setUsername(account.getUsername());
        dto.setEmail(account.getEmail());
        
        // Données sensibles uniquement pour ADMIN/MANAGER
        if (securityService.canViewSensitiveData(auth)) {
            dto.setSsn(account.getSsn());
            dto.setIncome(account.getIncome());
            dto.setCreditScore(account.getCreditScore());
        }
        
        // Données très sensibles uniquement pour ADMIN
        if (securityService.hasRole(auth, "ADMIN")) {
            dto.setInternalNotes(account.getInternalNotes());
            dto.setRiskProfile(account.getRiskProfile());
        }
        
        return dto;
    }
}
```

**Autorisation au niveau Gateway avec rôles** :
```java
@Component
public class RoleBasedAuthorizationFilter implements GlobalFilter, Ordered {
    
    private static final Logger log = LoggerFactory.getLogger(RoleBasedAuthorizationFilter.class);
    
    // Configuration des règles d'autorisation par route
    private final Map<RoutePattern, RoleRequirement> authorizationRules;
    
    public RoleBasedAuthorizationFilter() {
        this.authorizationRules = new HashMap<>();
        
        // Définir les règles d'accès
        // Pattern: path, method → rôles requis
        authorizationRules.put(
            new RoutePattern("/api/accounts/**", HttpMethod.GET),
            new RoleRequirement(Set.of("USER", "MANAGER", "ADMIN"))
        );
        
        authorizationRules.put(
            new RoutePattern("/api/accounts/**", HttpMethod.POST),
            new RoleRequirement(Set.of("MANAGER", "ADMIN"))
        );
        
        authorizationRules.put(
            new RoutePattern("/api/accounts/**", HttpMethod.DELETE),
            new RoleRequirement(Set.of("ADMIN"))
        );
        
        authorizationRules.put(
            new RoutePattern("/api/reports/**", HttpMethod.GET),
            new RoleRequirement(Set.of("MANAGER", "ADMIN"))
        );
        
        authorizationRules.put(
            new RoutePattern("/api/admin/**", null),  // null = all methods
            new RoleRequirement(Set.of("ADMIN"))
        );
    }
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = exchange.getRequest();
        String path = request.getPath().toString();
        HttpMethod method = request.getMethod();
        
        // Récupérer l'authentification
        return exchange.getPrincipal()
            .cast(JwtAuthenticationToken.class)
            .flatMap(authentication -> {
                Jwt jwt = (Jwt) authentication.getPrincipal();
                
                // Extraire les rôles de l'utilisateur
                Set<String> userRoles = extractRoles(authentication);
                
                log.info("Authorization check - User: {}, Roles: {}, Path: {}, Method: {}", 
                    jwt.getSubject(), userRoles, path, method);
                
                // Trouver la règle d'autorisation correspondante
                Optional<RoleRequirement> requirement = findMatchingRule(path, method);
                
                if (requirement.isEmpty()) {
                    // Pas de règle spécifique = authentification suffisante
                    return chain.filter(exchange);
                }
                
                RoleRequirement roleReq = requirement.get();
                
                // Vérifier si l'utilisateur a au moins un des rôles requis
                boolean authorized = userRoles.stream()
                    .anyMatch(roleReq.getRequiredRoles()::contains);
                
                if (!authorized) {
                    log.warn("Access denied - User: {} with roles {} tried to access {} {}", 
                        jwt.getSubject(), userRoles, method, path);
                    
                    return forbidden(exchange, 
                        "Insufficient permissions. Required roles: " + roleReq.getRequiredRoles());
                }
                
                // Ajouter les headers pour les microservices downstream
                ServerHttpRequest modifiedRequest = request.mutate()
                    .header("X-User-Id", jwt.getSubject())
                    .header("X-User-Email", jwt.getClaim("email"))
                    .header("X-User-Roles", String.join(",", userRoles))
                    .header("X-User-Name", jwt.getClaim("preferred_username"))
                    .build();
                
                return chain.filter(exchange.mutate().request(modifiedRequest).build());
            })
            .switchIfEmpty(Mono.defer(() -> {
                log.warn("Unauthenticated request to {}", path);
                return unauthorized(exchange, "Authentication required");
            }));
    }
    
    /**
     * Extrait les rôles depuis le JWT
     */
    private Set<String> extractRoles(JwtAuthenticationToken authentication) {
        return authentication.getAuthorities().stream()
            .map(GrantedAuthority::getAuthority)
            .map(role -> role.replace("ROLE_", ""))
            .collect(Collectors.toSet());
    }
    
    /**
     * Trouve la règle d'autorisation correspondant au path et method
     */
    private Optional<RoleRequirement> findMatchingRule(String path, HttpMethod method) {
        return authorizationRules.entrySet().stream()
            .filter(entry -> {
                RoutePattern pattern = entry.getKey();
                boolean pathMatches = matchesPattern(path, pattern.getPathPattern());
                boolean methodMatches = pattern.getMethod() == null || 
                                       pattern.getMethod().equals(method);
                return pathMatches && methodMatches;
            })
            .map(Map.Entry::getValue)
            .findFirst();
    }
    
    /**
     * Vérifie si le path correspond au pattern (support wildcards)
     */
    private boolean matchesPattern(String path, String pattern) {
        if (pattern.endsWith("/**")) {
            String prefix = pattern.substring(0, pattern.length() - 3);
            return path.startsWith(prefix);
        }
        return path.equals(pattern);
    }
    
    private Mono<Void> unauthorized(ServerWebExchange exchange, String message) {
        return sendError(exchange, HttpStatus.UNAUTHORIZED, "Unauthorized", message);
    }
    
    private Mono<Void> forbidden(ServerWebExchange exchange, String message) {
        return sendError(exchange, HttpStatus.FORBIDDEN, "Forbidden", message);
    }
    
    private Mono<Void> sendError(ServerWebExchange exchange, HttpStatus status, 
                                  String error, String message) {
        exchange.getResponse().setStatusCode(status);
        exchange.getResponse().getHeaders().setContentType(MediaType.APPLICATION_JSON);
        
        Map<String, Object> errorResponse = Map.of(
            "error", error,
            "message", message,
            "path", exchange.getRequest().getPath().toString(),
            "timestamp", Instant.now().toString(),
            "status", status.value()
        );
        
        try {
            byte[] bytes = new ObjectMapper().writeValueAsBytes(errorResponse);
            DataBuffer buffer = exchange.getResponse().bufferFactory().wrap(bytes);
            return exchange.getResponse().writeWith(Mono.just(buffer));
        } catch (Exception e) {
            return exchange.getResponse().setComplete();
        }
    }
    
    @Override
    public int getOrder() {
        return -100;  // Exécuter tôt
    }
    
    // Classes internes
    @Data
    @AllArgsConstructor
    private static class RoutePattern {
        private String pathPattern;
        private HttpMethod method;
    }
    
    @Data
    @AllArgsConstructor
    private static class RoleRequirement {
        private Set<String> requiredRoles;
    }
}
```

**Approche 3 : Permissions granulaires (au-delà des rôles)** :
```java
/**
 * Système de permissions fine-grained
 */
@Entity
@Table(name = "permissions")
public class Permission {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    @Column(nullable = false)
    private String resource;  // ex: "accounts", "loans", "reports"
    
    @Column(nullable = false)
    private String action;    // ex: "read", "write", "delete", "approve"
    
    @ManyToMany(mappedBy = "permissions")
    private Set<Role> roles;
}

@Entity
@Table(name = "roles")
public class Role {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    @Column(nullable = false, unique = true)
    private String name;  // ADMIN, MANAGER, USER
    
    @ManyToMany
    @JoinTable(
        name = "role_permissions",
        joinColumns = @JoinColumn(name = "role_id"),
        inverseJoinColumns = @JoinColumn(name = "permission_id")
    )
    private Set<Permission> permissions;
}

/**
 * Service pour vérifier les permissions
 */
@Service
public class PermissionService {
    
    @Autowired
    private RoleRepository roleRepository;
    
    /**
     * Vérifie si l'utilisateur a une permission spécifique
     */
    public boolean hasPermission(Authentication auth, String resource, String action) {
        Set<String> userRoles = extractRoles(auth);
        
        // Récupérer toutes les permissions des rôles utilisateur
        Set<Permission> permissions = roleRepository
            .findByNameIn(userRoles).stream()
            .flatMap(role -> role.getPermissions().stream())
            .collect(Collectors.toSet());
        
        // Vérifier si la permission existe
        return permissions.stream()
            .anyMatch(permission -> 
                permission.getResource().equals(resource) &&
                permission.getAction().equals(action)
            );
    }
    
    private Set<String> extractRoles(Authentication auth) {
        return auth.getAuthorities().stream()
            .map(GrantedAuthority::getAuthority)
            .map(role -> role.replace("ROLE_", ""))
            .collect(Collectors.toSet());
    }
}

/**
 * Utilisation avec @PreAuthorize
 */
@RestController
@RequestMapping("/api/loans")
public class LoanController {
    
    @GetMapping("/{id}")
    @PreAuthorize("@permissionService.hasPermission(authentication, 'loans', 'read')")
    public ResponseEntity<LoanDto> getLoan(@PathVariable Long id) {
        return ResponseEntity.ok(loanService.findById(id));
    }
    
    @PostMapping("/{id}/approve")
    @PreAuthorize("@permissionService.hasPermission(authentication, 'loans', 'approve')")
    public ResponseEntity<LoanDto> approveLoan(@PathVariable Long id) {
        return ResponseEntity.ok(loanService.approve(id));
    }
}
```

**Configuration des permissions dans Keycloak** :
```json
// Ajouter des claims personnalisés au token
{
  "realm_access": {
    "roles": ["MANAGER"]
  },
  "permissions": [
    {
      "resource": "accounts",
      "actions": ["read", "write"]
    },
    {
      "resource": "loans",
      "actions": ["read", "write", "approve"]
    },
    {
      "resource": "reports",
      "actions": ["read"]
    }
  ]
}
```

---

### Q6: Comment gérer la sécurité dans un contexte multi-tenant ? (Plusieurs organisations/clients utilisant la même application)
**R:**

**Architecture Multi-Tenant avec isolation de sécurité** :

**1. Identifier le tenant** :
```java
/**
 * Stratégies pour identifier le tenant
 */
public enum TenantIdentificationStrategy {
    
    // 1. Par sous-domaine: tenant1.myapp.com, tenant2.myapp.com
    SUBDOMAIN,
    
    // 2. Par header HTTP: X-Tenant-ID
    HEADER,
    
    // 3. Par path: /tenant1/api/..., /tenant2/api/...
    PATH,
    
    // 4. Par JWT claim (recommandé)
    JWT_CLAIM
}

/**
 * Resolver de tenant depuis le JWT
 */
@Component
public class JwtTenantResolver {
    
    public String resolveTenant(Jwt jwt) {
        // Le tenant est stocké dans le JWT lors de l'authentification
        String tenantId = jwt.getClaim("tenant_id");
        
        if (tenantId == null) {
            throw new SecurityException("No tenant information in token");
        }
        
        return tenantId;
    }
    
    public String resolveFromHeader(ServerHttpRequest request) {
        List<String> tenantHeader = request.getHeaders().get("X-Tenant-ID");
        
        if (tenantHeader == null || tenantHeader.isEmpty()) {
            throw new SecurityException("Missing X-Tenant-ID header");
        }
        
        return tenantHeader.get(0);
    }
}
```

**2. Tenant Context (ThreadLocal)** :
```java
/**
 * Stockage du tenant dans le contexte de la requête
 */
public class TenantContext {
    
    private static final ThreadLocal<String> CURRENT_TENANT = new ThreadLocal<>();
    
    public static void setTenant(String tenantId) {
        CURRENT_TENANT.set(tenantId);
    }
    
    public static String getTenant() {
        return CURRENT_TENANT.get();
    }
    
    public static void clear() {
        CURRENT_TENANT.remove();
    }
}

/**
 * Filter pour extraire et stocker le tenant
 */
@Component
public class TenantFilter implements GlobalFilter, Ordered {
    
    @Autowired
    private JwtTenantResolver tenantResolver;
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        return exchange.getPrincipal()
            .cast(JwtAuthenticationToken.class)
            .flatMap(authentication -> {
                Jwt jwt = (Jwt) authentication.getPrincipal();
                
                // Extraire le tenant ID
                String tenantId = tenantResolver.resolveTenant(jwt);
                
                // Stocker dans le contexte
                TenantContext.setTenant(tenantId);
                
                // Ajouter dans header pour microservices
                ServerHttpRequest modifiedRequest = exchange.getRequest().mutate()
                    .header("X-Tenant-ID", tenantId)
                    .build();
                
                return chain.filter(exchange.mutate().request(modifiedRequest).build())
                    .doFinally(signalType -> TenantContext.clear());
            });
    }
    
    @Override
    public int getOrder() {
        return -90;  // Après authentication, avant authorization
    }
}
```

**3. Isolation des données par tenant** :
```java
/**
 * Entity avec tenant
 */
@Entity
@Table(name = "accounts")
@FilterDef(name = "tenantFilter", parameters = @ParamDef(name = "tenantId", type = "string"))
@Filter(name = "tenantFilter", condition = "tenant_id = :tenantId")
public class Account {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    @Column(name = "tenant_id", nullable = false)
    private String tenantId;
    
    private String username;
    private String email;
    
    // ... autres champs
}

/**
 * Repository avec filtrage automatique par tenant
 */
@Repository
public interface AccountRepository extends JpaRepository<Account, Long> {
    
    // Requête automatiquement filtrée par tenant (grâce au @Filter)
    List<Account> findAll();
    
    Optional<Account> findById(Long id);
    
    // Requête custom si besoin d'override le filtre
    @Query("SELECT a FROM Account a WHERE a.tenantId = :tenantId")
    List<Account> findAllByTenant(@Param("tenantId") String tenantId);
}

/**
 * Interceptor Hibernate pour activer le filtre tenant
 */
@Component
public class TenantInterceptor extends EmptyInterceptor {
    
    @Override
    public String onPrepareStatement(String sql) {
        // Le filtre sera appliqué automatiquement
        return super.onPrepareStatement(sql);
    }
}

/**
 * Configuration JPA pour activer le filtre
 */
@Configuration
public class HibernateConfig {
    
    @Bean
    public FilterRegistrationBean<TenantFilterRegistration> tenantFilter() {
        FilterRegistrationBean<TenantFilterRegistration> registrationBean = 
            new FilterRegistrationBean<>();
        
        registrationBean.setFilter(new TenantFilterRegistration());
        registrationBean.addUrlPatterns("/*");
        
        return registrationBean;
    }
}

@Component
public class TenantFilterRegistration implements Filter {
    
    @Autowired
    private EntityManager entityManager;
    
    @Override
    public void doFilter(ServletRequest request, ServletResponse response, 
                        FilterChain chain) throws IOException, ServletException {
        
        String tenantId = TenantContext.getTenant();
        
        if (tenantId != null) {
            Session session = entityManager.unwrap(Session.class);
            org.hibernate.Filter filter = session.enableFilter("tenantFilter");
            filter.setParameter("tenantId", tenantId);
        }
        
        try {
            chain.doFilter(request, response);
        } finally {
            TenantContext.clear();
        }
    }
}
```

**4. Validation cross-tenant** :
```java
/**
 * Service de sécurité multi-tenant
 */
@Service
public class MultiTenantSecurityService {
    
    @Autowired
    private JwtTenantResolver tenantResolver;
    
    /**
     * Vérifie que la ressource appartient au tenant de l'utilisateur
     */
    public boolean belongsToUserTenant(Long resourceId, String resourceType, 
                                       Authentication auth) {
        Jwt jwt = (Jwt) auth.getPrincipal();
        String userTenantId = tenantResolver.resolveTenant(jwt);
        
        // Récupérer le tenant de la ressource
        String resourceTenantId = getResourceTenantId(resourceId, resourceType);
        
        if (!userTenantId.equals(resourceTenantId)) {
            log.warn("Cross-tenant access attempt - User tenant: {}, Resource tenant: {}", 
                userTenantId, resourceTenantId);
            return false;
        }
        
        return true;
    }
    
    /**
     * Récupère le tenant ID d'une ressource
     */
    private String getResourceTenantId(Long resourceId, String resourceType) {
        // Logique pour récupérer le tenant selon le type de ressource
        switch (resourceType) {
            case "account":
                return accountRepository.findById(resourceId)
                    .map(Account::getTenantId)
                    .orElseThrow();
            case "loan":
                return loanRepository.findById(resourceId)
                    .map(Loan::getTenantId)
                    .orElseThrow();
            default:
                throw new IllegalArgumentException("Unknown resource type: " + resourceType);
        }
    }
}

/**
 * Utilisation dans les controllers
 */
@RestController
@RequestMapping("/api/accounts")
public class AccountController {
    
    @GetMapping("/{id}")
    @PreAuthorize("@multiTenantSecurityService.belongsToUserTenant(#id, 'account', authentication)")
    public ResponseEntity<AccountDto> getAccount(@PathVariable Long id) {
        return ResponseEntity.ok(accountService.findById(id));
    }
    
    @PutMapping("/{id}")
    @PreAuthorize("hasRole('USER') and " +
                  "@multiTenantSecurityService.belongsToUserTenant(#id, 'account', authentication)")
    public ResponseEntity<AccountDto> updateAccount(
        @PathVariable Long id,
        @RequestBody AccountDto dto) {
        
        // Le filtre Hibernate garantit qu'on ne peut update que son tenant
        return ResponseEntity.ok(accountService.update(id, dto));
    }
}
```

**5. Configuration Keycloak Multi-Tenant** :
```
Stratégie 1: Un Realm par tenant
- Realm "tenant1"
- Realm "tenant2"
- Avantage: Isolation complète
- Inconvénient: Gestion complexe

Stratégie 2: Un Realm, attributs tenant (Recommandé)
- Realm "microservices"
- User attributes: tenant_id = "tenant1"
- Group per tenant: "tenant1-users", "tenant2-users"
- Mapper pour ajouter tenant_id dans le JWT
```

**Mapper Keycloak pour ajouter tenant_id au JWT** :
```json
// Dans Keycloak Client Mappers
{
  "protocol": "openid-connect",
  "protocolMapper": "oidc-usermodel-attribute-mapper",
  "name": "tenant-id-mapper",
  "config": {
    "user.attribute": "tenant_id",
    "claim.name": "tenant_id",
    "jsonType.label": "String",
    "id.token.claim": "true",
    "access.token.claim": "true"
  }
}
```

**6. Tests multi-tenant** :
```java
@SpringBootTest
public class MultiTenantSecurityTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    /**
     * Test: User de tenant1 ne peut pas accéder aux données de tenant2
     */
    @Test
    @WithMockJwt(subject = "user1", tenantId = "tenant1", roles = {"USER"})
    public void testCrossTenantAccess_ShouldBeForbidden() throws Exception {
        // Créer un account pour tenant2
        Long tenant2AccountId = createAccountForTenant("tenant2");
        
        // User de tenant1 essaie d'accéder
        mockMvc.perform(get("/api/accounts/" + tenant2AccountId))
            .andExpect(status().isForbidden())
            .andExpect(jsonPath("$.error").value("Cross-tenant access denied"));
    }
    
    /**
     * Test: User peut accéder seulement à ses propres données tenant
     */
    @Test
    @WithMockJwt(subject = "user1", tenantId = "tenant1", roles = {"USER"})
    public void testSameTenantAccess_ShouldSucceed() throws Exception {
        Long tenant1AccountId = createAccountForTenant("tenant1");
        
        mockMvc.perform(get("/api/accounts/" + tenant1AccountId))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.tenantId").value("tenant1"));
    }
    
    /**
     * Test: Isolation complète des données en liste
     */
    @Test
    @WithMockJwt(subject = "user1", tenantId = "tenant1", roles = {"USER"})
    public void testListAccounts_ShouldReturnOnlyTenantData() throws Exception {
        createAccountForTenant("tenant1");
        createAccountForTenant("tenant1");
        createAccountForTenant("tenant2");  // Ne doit pas apparaître
        
        mockMvc.perform(get("/api/accounts"))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.length()").value(2))
            .andExpect(jsonPath("$[*].tenantId").value(everyItem(equalTo("tenant1"))));
    }
}
```

**Best Practices Multi-Tenant** :
```yaml
✅ Toujours valider tenant_id dans chaque requête
✅ Utiliser filtres Hibernate pour isolation automatique
✅ Logger tous les accès cross-tenant tentés
✅ Tests exhaustifs d'isolation
✅ Encryption des données sensibles par tenant
✅ Backup séparé par tenant
✅ Rate limiting par tenant
✅ Monitoring des métriques par tenant
❌ JAMAIS faire confiance au tenant_id du client
❌ JAMAIS permettre de changer de tenant sans re-authentication
```

Voilà une réponse complète sur le multi-tenant ! Voulez-vous que je continue avec d'autres questions sur la sécurité ?