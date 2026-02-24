## [**..**](./00_index.md)

## Microservices Security with OAuth2 & Keycloak


---

## 1. Securing Gateway Server as a Resource Server (Keycloak)

### Security Architecture

**Secured architecture components**:
```
Client (Browser/Mobile)
    ↓
    ↓ 1. Request with Access Token
    ↓
API Gateway (Resource Server)
    ↓ 2. Validates Token with Keycloak
    ↓ 3. Verifies JWT Signature
    ↓ 4. Extract Claims & Roles
    ↓
Microservices Backend (accounts, loans, cards)
```

### Keycloak Configuration

**Keycloak Installation and Setup**:
- **Deployment**: Keycloak on Docker/Kubernetes to centralize authentication
- **Realm**: Create a dedicated realm (e.g. "microservices-realm")
- **Clients**: Configure OAuth2 clients for the Gateway and frontend applications
- **Roles & Groups**: Define business roles (ADMIN, USER, MANAGER)
- **Users**: Manage users with role assignment

**Keycloak Realm Configuration**:
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

### Configuring Spring Cloud Gateway as a Resource Server

**Maven Dependencies**:
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
    
    <!-- OAuth2 Client (for Authorization Code flow) -->
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

**Application Configuration (application.yml)**:
```yaml
spring:
  application:
    name: api-gateway
  
  security:
    oauth2:
      # Resource Server configuration
      resourceserver:
        jwt:
          # URL to fetch Keycloak public keys
          jwk-set-uri: http://localhost:8080/realms/microservices-realm/protocol/openid-connect/certs
          # Issuer for token validation
          issuer-uri: http://localhost:8080/realms/microservices-realm
      
      # OAuth2 Client configuration
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
        # Route to Accounts Service
        - id: accounts-service
          uri: lb://ACCOUNTS-SERVICE
          predicates:
            - Path=/api/accounts/**
          filters:
            - TokenRelay=  # Propagates the token to the backend
            - RemoveRequestHeader=Cookie
        
        # Route to Loans Service
        - id: loans-service
          uri: lb://LOANS-SERVICE
          predicates:
            - Path=/api/loans/**
          filters:
            - TokenRelay=
        
        # Route to Cards Service (admin only)
        - id: cards-service
          uri: lb://CARDS-SERVICE
          predicates:
            - Path=/api/cards/**
          filters:
            - TokenRelay=

server:
  port: 8072

# Actuator for monitoring
management:
  endpoints:
    web:
      exposure:
        include: health,info,metrics,gateway
```

**Security Configuration (SecurityConfig.java)**:
```java
@Configuration
@EnableWebFluxSecurity  // For reactive (Spring Cloud Gateway is reactive)
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
     * Role extractor from JWT
     * Keycloak stores roles in jwt.realm_access.roles
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

**Keycloak Role Conversion**:
```java
/**
 * Converter to extract roles from the Keycloak JWT token
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

**Token Validation and Claims**:
```java
@Service
public class TokenValidationService {
    
    @Autowired
    private JwtDecoder jwtDecoder;
    
    /**
     * Validates and decodes the JWT
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
     * Extracts roles from the token
     */
    private List<String> extractRoles(Jwt jwt) {
        Map<String, Object> realmAccess = jwt.getClaim("realm_access");
        if (realmAccess != null && realmAccess.containsKey("roles")) {
            return (List<String>) realmAccess.get("roles");
        }
        return Collections.emptyList();
    }
    
    /**
     * Checks if the user has a specific role
     */
    public boolean hasRole(Jwt jwt, String requiredRole) {
        List<String> roles = extractRoles(jwt);
        return roles.contains(requiredRole);
    }
}
```

---

## 2. Implement Authorization inside Gateway Server using Roles

### Role-Based Authorization (RBAC)

**Authorization rules configuration by route**:
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

### Gateway Filters for Fine-Grained Authorization

**Custom Gateway Filter for role checking**:
```java
@Component
public class RoleBasedAuthorizationGatewayFilter implements GlobalFilter, Ordered {
    
    private static final Logger log = LoggerFactory.getLogger(RoleBasedAuthorizationGatewayFilter.class);
    
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = exchange.getRequest();
        String path = request.getPath().toString();
        
        // Get the authentication depuis le contexte réactif
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
                
                // Add user info in headers to backend
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

### Fine-Grained Authorization with HTTP Method

**Route-based Authorization Filter**:
```java
@Component
public class RouteBasedAuthorizationFilter implements GatewayFilter {
    
    private final Map<String, Set<String>> routeRoleMapping;
    
    public RouteBasedAuthorizationFilter() {
        // Mapping route → allowed roles
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
                
                // Check if the user has at least one required role
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

### Per-Route Filter Configuration

**Application.yml with custom filters**:
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

**Full Authorization Code Flow**:
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

### OAuth2 Client Configuration (Authorization Code)

**Application.yml - OAuth2 Client Configuration**:
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

### Security Configuration for Authorization Code Flow

**SecurityConfig with OAuth2 Login**:
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
     * Handler for successful authentication
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
                
                // Extract user information
                OAuth2User oauth2User = oauth2Token.getPrincipal();
                String username = oauth2User.getAttribute("preferred_username");
                String email = oauth2User.getAttribute("email");
                
                log.info("User {} logged in successfully with email: {}", username, email);
                
                // Redirect to home page
                ServerWebExchange exchange = webFilterExchange.getExchange();
                return new RedirectServerAuthenticationSuccessHandler("/home")
                    .onAuthenticationSuccess(webFilterExchange, authentication);
            }
        };
    }
    
    /**
     * Handler for authentication failure
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
     * Logout handler for OIDC
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

**Service to manage tokens**:
```java
@Service
public class TokenManagementService {
    
    @Autowired
    private ReactiveOAuth2AuthorizedClientService authorizedClientService;
    
    /**
     * Gets the Access Token of the authenticated user
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
     * Gets the Refresh Token
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
     * Refreshes the Access Token using the Refresh Token
     */
    public Mono<OAuth2AccessToken> refreshAccessToken(String refreshToken) {
        // Logique de refresh avec Keycloak
        // Appel POST vers /token avec grant_type=refresh_token
        return Mono.empty(); // Implémentation simplifiée
    }
    
    /**
     * Revokes the token (logout)
     */
    public Mono<Void> revokeToken(String token) {
        // Appel vers Keycloak revocation endpoint
        return Mono.empty(); // Implémentation simplifiée
    }
}
```

### Controller for Authentication Flow

**REST Controller to handle authentication**:
```java
@RestController
@RequestMapping("/auth")
public class AuthenticationController {
    
    @Autowired
    private TokenManagementService tokenService;
    
    /**
     * Endpoint to initiate OAuth2 login
     */
    @GetMapping("/login")
    public Mono<Void> login(ServerWebExchange exchange) {
        // Redirects to Keycloak for authentication
        return Mono.fromRunnable(() -> {
            exchange.getResponse().setStatusCode(HttpStatus.FOUND);
            exchange.getResponse().getHeaders()
                .setLocation(URI.create("/oauth2/authorization/keycloak-client"));
        }).then();
    }
    
    /**
     * Gets information about the logged-in user
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
     * Gets the user's Access Token
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
     * Refreshes the token
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

**Frontend integration**:
```javascript
// AuthService.js
class AuthService {
  
  // Initiate OAuth2 login
  login() {
    window.location.href = 'http://localhost:8072/oauth2/authorization/keycloak-client';
  }
  
  // Get token from session cookie
  async getAccessToken() {
    const response = await fetch('http://localhost:8072/auth/token', {
      credentials: 'include'  // Important for cookies
    });
    const data = await response.json();
    return data.access_token;
  }
  
  // API call with token
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
  
  // Get user information
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
  
  // Check if user is authenticated
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

### Token Relay to Backend Microservices

**Configuration to propagate the token**:
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
            - TokenRelay=  # Automatically propagates the OAuth2 token
            - RemoveRequestHeader=Cookie  # Removes cookies
```

**Custom Gateway Filter for Token Relay**:
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

### Securing Backend Microservices

**Configuring Microservices as Resource Servers**:
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
     * Converter to extract roles from JWT
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

**Method-level authorization**:
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
     * Endpoint with custom check
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

**Custom security service**:
```java
@Service("accountSecurityService")
public class AccountSecurityService {
    
    @Autowired
    private AccountRepository accountRepository;
    
    /**
     * Checks if the user is the owner of the account
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
     * Checks if the user has a specific role
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

### Security Monitoring and Logging

**Audit Logging for access**:
```java
@Aspect
@Component
public class SecurityAuditAspect {
    
    private static final Logger log = LoggerFactory.getLogger(SecurityAuditAspect.class);
    
    @Autowired
    private AuditLogRepository auditLogRepository;
    
    /**
     * Log all access to secured endpoints
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
            
            // Save to DB for audit
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

**Custom Access Denied Handler**:
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

**Prometheus metrics for security**:
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

### Security Tests

**Integration tests for Authorization**:
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
     * Test protected endpoint without token - must return 401
     */
    @Test
    public void testProtectedEndpoint_NoAuth_Returns401() throws Exception {
        mockMvc.perform(get("/api/accounts/myaccount"))
            .andExpect(status().isUnauthorized());
    }
    
    /**
     * Test protected endpoint with valid USER token - must succeed
     */
    @Test
    @WithMockJwt(subject = "john.doe", roles = {"USER"})
    public void testProtectedEndpoint_ValidUserToken_Success() throws Exception {
        mockMvc.perform(get("/api/accounts/myaccount"))
            .andExpect(status().isOk());
    }
    
    /**
     * Test ADMIN endpoint with USER token - must return 403
     */
    @Test
    @WithMockJwt(subject = "john.doe", roles = {"USER"})
    public void testAdminEndpoint_UserToken_Returns403() throws Exception {
        mockMvc.perform(get("/api/accounts/all"))
            .andExpect(status().isForbidden());
    }
    
    /**
     * Test ADMIN endpoint with ADMIN token - must succeed
     */
    @Test
    @WithMockJwt(subject = "admin", roles = {"ADMIN"})
    public void testAdminEndpoint_AdminToken_Success() throws Exception {
        mockMvc.perform(get("/api/accounts/all"))
            .andExpect(status().isOk());
    }
    
    /**
     * Test with expired JWT token - must return 401
     */
    @Test
    public void testProtectedEndpoint_ExpiredToken_Returns401() throws Exception {
        String expiredToken = generateExpiredJWT();
        
        mockMvc.perform(get("/api/accounts/myaccount")
                .header("Authorization", "Bearer " + expiredToken))
            .andExpect(status().isUnauthorized());
    }
    
    /**
     * Test with invalid JWT token - must return 401
     */
    @Test
    public void testProtectedEndpoint_InvalidToken_Returns401() throws Exception {
        mockMvc.perform(get("/api/accounts/myaccount")
                .header("Authorization", "Bearer invalid.jwt.token"))
            .andExpect(status().isUnauthorized());
    }
}
```

**Custom annotation for Mock JWT**:
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

## Results & Impact

### Security & Compliance
✅ **Centralized authentication**: Single Sign-On (SSO) with Keycloak for all microservices  
✅ **Granular authorization**: RBAC at Gateway and microservices level with fine role control  
✅ **Token-based security**: Stateless JWT for scalability and performance  
✅ **OAuth2/OIDC standards**: Compliance with industry standards  
✅ **Complete audit**: Logging of all access and unauthorized access attempts

### Performance & Scalability
✅ **Stateless authentication**: No server-side sessions, unlimited horizontal scalability  
✅ **Local JWT validation**: No call to Keycloak for each request (local signature verification)  
✅ **Public key caching**: JWK Set cached for optimal performance  
✅ **Token Relay**: Transparent propagation of security context between microservices

### Developer Experience
✅ **Declarative configuration**: Security defined in annotations and YAML  
✅ **Automated tests**: Complete security test suite with Mock JWT  
✅ **OpenAPI documentation**: Swagger UI with integrated OAuth2 authentication  
✅ **Developer-friendly**: Simple local configuration with Docker Compose

---

## Technologies & Standards

**Security Frameworks**: Spring Security, Spring OAuth2 Resource Server, Spring OAuth2 Client  
**Identity Provider**: Keycloak (open-source IAM)  
**Standards**: OAuth 2.0, OpenID Connect (OIDC), JWT (RFC 7519)  
**Grant Types**: Authorization Code, Client Credentials, Refresh Token  
**Authorization**: RBAC (Role-Based Access Control), Method-level security  
**API Gateway**: Spring Cloud Gateway (reactive security)  
**Monitoring**: Security audit logs, Prometheus metrics, distributed tracing

---

## Best Practices Applied

```yaml
✅ Principle of Least Privilege (minimum required roles)
✅ Defense in Depth (multi-layer security: Gateway + Microservices)
✅ Fail Secure (deny by default, explicit allow)
✅ Short token expiration (5-15 minutes) with refresh tokens
✅ HTTPS/TLS required in production
✅ CORS configured strictly
✅ Rate limiting to prevent brute-force attacks
✅ Security headers (X-Frame-Options, CSP, etc.)
✅ Complete audit logging for compliance
✅ Externalized secrets (never hardcoded)
✅ Regular security testing (OWASP Top 10)
```

---

# Technical Questions/Answers - Microservices Security with OAuth2 & Keycloak

## 1. Fundamental OAuth2 & Keycloak Concepts

### Q1: Explain the difference between Authentication and Authorization. How does OAuth2 handle both?
**A:**

**Authentication**:
- **Definition**: Verify the user's identity ("Who are you?")
- **Example**: Login with username/password, verifying credentials
- **Result**: Confirmation that the user is who they claim to be

**Authorization**:
- **Definition**: Determine what the user can do ("What can you do?")
- **Example**: Check if the user has the ADMIN role to delete an account
- **Result**: Permission or denial of access to a resource

**OAuth2 and these concepts**:
```
OAuth2 is mainly an AUTHORIZATION framework, not an authentication one!

Authentication → Handled by OpenID Connect (OIDC), a layer on top of OAuth2
Authorization → Handled directly by OAuth2 (scopes, tokens)

Flow complet:
1. Authentication (OIDC): 
   - User connects to Keycloak
   - Keycloak validates credentials
   - Keycloak returns ID Token (contains user info)

2. Authorization (OAuth2):
   - Keycloak returns Access Token (contains scopes/roles)
   - Client uses Access Token to access resources
   - Resource Server validates the token and checks permissions
```

**In our architecture**:
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

### Q2: What is a JWT (JSON Web Token) and how does it work? What are its parts?
**A:**

**JWT Structure**:
```
A JWT is composed of 3 parts separated by dots (.):
HEADER.PAYLOAD.SIGNATURE

Exemple:
eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJqb2huLmRvZSIsInJvbGVzIjpbIlVTRVIiXX0.signature
```

**1. Header (Base64URL encoded)**:
```json
{
  "alg": "RS256",       // Signature algorithm (RSA SHA-256)
  "typ": "JWT",         // Token type
  "kid": "key-id-123"   // Key ID (which public key to use)
}
```

**2. Payload (Base64URL encoded - Claims)**:
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

**3. Signature (cryptographic)**:
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

**JWT Advantages**:
```yaml
✅ Stateless: No need to store sessions server-side
✅ Self-contained: All necessary info is in the token
✅ Scalable: Each instance can validate independently
✅ Interoperable: Open standard (RFC 7519)
✅ Performance: Local validation without calling Keycloak

⚠️ Warning:
❌ Token cannot be revoked before expiration (use refresh tokens)
❌ Large size if many claims
❌ Visible in clear (Base64 ≠ encryption) - don't put secrets
```

**JWT Validation in Spring Boot**:
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

### Q3: What is the difference between Access Token and Refresh Token? Why use both?
**A:**

**Access Token**:
```yaml
Lifetime: Short (5-15 minutes typically)
Usage: Access protected resources (API calls)
Storage: Memory (never localStorage in production)
Revocation: Difficult (wait for expiration)
Content: JWT with user claims and roles
```

**Refresh Token**:
```yaml
Lifetime: Long (days/weeks/months)
Usage: Obtain a new Access Token
Storage: HttpOnly secure cookie
Revocation: Possible (stored server-side in Keycloak)
Content: Opaque token (usually UUID)
```

**Why both?**

**Security**:
```
Short Access Token = Limited exposure window if stolen
Long Refresh Token = User comfort (no frequent re-login)

Trade-off:
- If Access Token is stolen → Valid only 5-15 min
- If Refresh Token is stolen → Can be revoked server-side
```

**Full flow with both tokens**:
```
1. Login initial:
   User → Keycloak (credentials)
   Keycloak → Client (Access Token + Refresh Token)

2. Appel API normal:
   Client → Gateway (Access Token)
   Gateway → Validates token locally
   Gateway → Microservice (valid token)

3. Access Token expiré:
   Client → Gateway (expired Access Token)
   Gateway → 401 Unauthorized
   Client → Keycloak (Refresh Token)
   Keycloak → Validates Refresh Token
   Keycloak → Client (New Access Token)
   Client → Gateway (New Access Token)
   Gateway → Microservice (succès)

4. Refresh Token expiré ou révoqué:
   Client → Keycloak (expired Refresh Token)
   Keycloak → 401 Unauthorized
   Client → Redirect to Login
```

**Client-side implementation**:
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

**Gateway-side implementation**:
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
     * Endpoint to refresh the Access Token
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

**Best Practices**:
```yaml
✅ Access Token: 5-15 minutes max
✅ Refresh Token: 1-7 days depending on application sensitivity
✅ Refresh Token stored in HttpOnly cookie (not accessible by JS)
✅ Refresh Token rotation: new refresh token on each refresh
✅ Token replay detection: invalidate old refresh after use
✅ Limit number of refreshes: max 10 refreshes per session
❌ NEVER store Access/Refresh Token in localStorage (XSS risk)
```

---

## 2. Authorization Code Flow

### Q4: Explain the complete Authorization Code flow with PKCE. Why is it the most secure flow?
**A:**

**Authorization Code Flow with PKCE (Proof Key for Code Exchange)**:

**Step-by-step detail**:
```
1. Client génère code_verifier et code_challenge
   code_verifier = random_string_43_128_chars()
   code_challenge = BASE64URL(SHA256(code_verifier))

2. User clicks "Login"
   Frontend → Redirect vers Keycloak avec:
   GET /auth?
     response_type=code
     &client_id=my-app
     &redirect_uri=http://localhost:3000/callback
     &scope=openid profile email
     &state=random_state_value
     &code_challenge=abc123...
     &code_challenge_method=S256

3. User enters credentials in Keycloak
   Keycloak → Validates username/password
   Keycloak → Generates Authorization Code

4. Keycloak redirect vers application
   Keycloak → Frontend avec:
   http://localhost:3000/callback?
     code=AUTH_CODE_XYZ
     &state=random_state_value

5. Frontend validates state (prevents CSRF)
   if (received_state !== sent_state) reject()

6. Frontend sends code to Backend
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

8. Keycloak validates and returns tokens
   Keycloak checks:
   - code valid and not used
   - code_verifier matches the code_challenge
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

10. User is authenticated
    Frontend can now call APIs with session
```

**Why is it the most secure?**

**1. Authorization Code never exposed to the frontend**:
```
Mauvais (Implicit Flow):
Keycloak → Frontend with Access Token directly in URL
https://app.com/callback#access_token=SECRET_TOKEN
❌ Token visible in browser history
❌ Token accessible par JavaScript
❌ XSS risk

Bon (Authorization Code):
Keycloak → Frontend with temporary Code
https://app.com/callback?code=TEMP_CODE
Backend → Keycloak to exchange Code for Token
✅ Token never exposed to the browser
✅ Code usable only once (10 seconds max)
```

**2. PKCE protects against interception**:
```
Sans PKCE:
Attacker intercepte code → Peut l'utiliser si connaît client_secret

Avec PKCE:
Attacker intercepts code → Cannot use it without code_verifier
code_verifier never transmitted over the network (except in final exchange)
```

**3. Client Secret on the backend**:
```
client_secret never exposed to the frontend
Only the backend knows the secret
Impossible for attacker to steal the secret from the browser
```

**4. State parameter (CSRF protection)**:
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

**Full implementation with PKCE**:
```java
@Service
public class PKCEService {
    
    private static final SecureRandom SECURE_RANDOM = new SecureRandom();
    
    /**
     * Generates a random code_verifier
     */
    public String generateCodeVerifier() {
        byte[] codeVerifier = new byte[32];
        SECURE_RANDOM.nextBytes(codeVerifier);
        return Base64.getUrlEncoder()
            .withoutPadding()
            .encodeToString(codeVerifier);
    }
    
    /**
     * Generates code_challenge from code_verifier
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
     * Initiates the Authorization Code flow with PKCE
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
     * Callback after Keycloak authentication
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

**Flow comparison**:
```
| Flow                    | Sécurité | Use Case                    |
|------------------------|----------|------------------------------|
| Authorization Code+PKCE | ⭐⭐⭐⭐⭐ | SPA, Mobile apps (moderne)   |
| Authorization Code      | ⭐⭐⭐⭐   | Traditional web apps         |
| Client Credentials      | ⭐⭐⭐⭐   | Server-to-server (M2M)       |
| Resource Owner Password | ⭐⭐      | Legacy (deprecated)          |
| Implicit                | ⭐       | Deprecated (insecure)        |
```

---

## 3. Role-Based Access Control (RBAC)

### Q5: How to implement fine-grained authorization with hierarchical roles? For example ADMIN > MANAGER > USER
**A:**

**Approach 1: Keycloak configuration with composite roles**:
```
In Keycloak Admin Console:

1. Create the base roles:
   - USER (base role)
   - MANAGER (contient USER)
   - ADMIN (contient MANAGER et USER)

2. Configure composite roles:
   ADMIN → Composite Roles → Add "MANAGER" and "USER"
   MANAGER → Composite Roles → Add "USER"

Result:
- User with ADMIN role → automatically has MANAGER and USER
- User with MANAGER role → automatically has USER
- User with USER role → has only USER
```

**Approach 2: Role Hierarchy in Spring Security**:
```java
@Configuration
@EnableMethodSecurity
public class SecurityConfig {
    
    @Bean
    public RoleHierarchy roleHierarchy() {
        RoleHierarchyImpl roleHierarchy = new RoleHierarchyImpl();
        
        // Define the hierarchy
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

**Usage in controllers**:
```java
@RestController
@RequestMapping("/api/accounts")
public class AccountController {
    
    /**
     * Accessible by USER, MANAGER and ADMIN (thanks to hierarchy)
     */
    @GetMapping("/my-account")
    @PreAuthorize("hasRole('USER')")
    public ResponseEntity<AccountDto> getMyAccount(@AuthenticationPrincipal Jwt jwt) {
        String username = jwt.getSubject();
        return ResponseEntity.ok(accountService.getByUsername(username));
    }
    
    /**
     * Accessible by MANAGER and ADMIN only
     */
    @GetMapping("/team-accounts")
    @PreAuthorize("hasRole('MANAGER')")
    public ResponseEntity<List<AccountDto>> getTeamAccounts() {
        return ResponseEntity.ok(accountService.getTeamAccounts());
    }
    
    /**
     * Accessible by ADMIN only
     */
    @DeleteMapping("/{id}")
    @PreAuthorize("hasRole('ADMIN')")
    public ResponseEntity<Void> deleteAccount(@PathVariable Long id) {
        accountService.delete(id);
        return ResponseEntity.noContent().build();
    }
    
    /**
     * Complex conditional authorization
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

**Security service with business logic**:
```java
@Service("accountSecurityService")
public class AccountSecurityService {
    
    @Autowired
    private AccountRepository accountRepository;
    
    @Autowired
    private TeamRepository teamRepository;
    
    /**
     * Checks if the user is the owner of the account
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
     * Checks if the user is in the same team
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
     * Checks if the user can view sensitive data
     */
    public boolean canViewSensitiveData(Authentication auth) {
        return hasAnyRole(auth, "ADMIN", "MANAGER");
    }
    
    /**
     * Helper to check multiple roles
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

**Filter data based on role**:
```java
@Service
public class AccountService {
    
    @Autowired
    private AccountSecurityService securityService;
    
    /**
     * Returns data filtered by role
     */
    public AccountDto getAccount(Long id, Authentication auth) {
        Account account = accountRepository.findById(id)
            .orElseThrow(() -> new NotFoundException("Account not found"));
        
        AccountDto dto = new AccountDto();
        dto.setId(account.getId());
        dto.setUsername(account.getUsername());
        dto.setEmail(account.getEmail());
        
        // Sensitive data only for ADMIN/MANAGER
        if (securityService.canViewSensitiveData(auth)) {
            dto.setSsn(account.getSsn());
            dto.setIncome(account.getIncome());
            dto.setCreditScore(account.getCreditScore());
        }
        
        // Very sensitive data only for ADMIN
        if (securityService.hasRole(auth, "ADMIN")) {
            dto.setInternalNotes(account.getInternalNotes());
            dto.setRiskProfile(account.getRiskProfile());
        }
        
        return dto;
    }
}
```

**Authorization at Gateway level with roles**:
```java
@Component
public class RoleBasedAuthorizationFilter implements GlobalFilter, Ordered {
    
    private static final Logger log = LoggerFactory.getLogger(RoleBasedAuthorizationFilter.class);
    
    // Configuration des règles d'autorisation par route
    private final Map<RoutePattern, RoleRequirement> authorizationRules;
    
    public RoleBasedAuthorizationFilter() {
        this.authorizationRules = new HashMap<>();
        
        // Define access rules
        // Pattern: path, method → required roles
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
        
        // Get the authentication
        return exchange.getPrincipal()
            .cast(JwtAuthenticationToken.class)
            .flatMap(authentication -> {
                Jwt jwt = (Jwt) authentication.getPrincipal();
                
                // Extract user roles
                Set<String> userRoles = extractRoles(authentication);
                
                log.info("Authorization check - User: {}, Roles: {}, Path: {}, Method: {}", 
                    jwt.getSubject(), userRoles, path, method);
                
                // Find the matching authorization rule
                Optional<RoleRequirement> requirement = findMatchingRule(path, method);
                
                if (requirement.isEmpty()) {
                    // No specific rule = authentication is sufficient
                    return chain.filter(exchange);
                }
                
                RoleRequirement roleReq = requirement.get();
                
                // Check if the user has at least one of the required roles
                boolean authorized = userRoles.stream()
                    .anyMatch(roleReq.getRequiredRoles()::contains);
                
                if (!authorized) {
                    log.warn("Access denied - User: {} with roles {} tried to access {} {}", 
                        jwt.getSubject(), userRoles, method, path);
                    
                    return forbidden(exchange, 
                        "Insufficient permissions. Required roles: " + roleReq.getRequiredRoles());
                }
                
                // Add headers for downstream microservices
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
     * Checks if the path matches the pattern (wildcard support)
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

**Approach 3: Fine-grained permissions (beyond roles)**:
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
    private String resource;  // e.g. "accounts", "loans", "reports"
    
    @Column(nullable = false)
    private String action;    // e.g. "read", "write", "delete", "approve"
    
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
 * Service to check permissions
 */
@Service
public class PermissionService {
    
    @Autowired
    private RoleRepository roleRepository;
    
    /**
     * Checks if the user has a specific permission
     */
    public boolean hasPermission(Authentication auth, String resource, String action) {
        Set<String> userRoles = extractRoles(auth);
        
        // Get all permissions for the user's roles
        Set<Permission> permissions = roleRepository
            .findByNameIn(userRoles).stream()
            .flatMap(role -> role.getPermissions().stream())
            .collect(Collectors.toSet());
        
        // Check if the permission exists
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

**Permissions configuration in Keycloak**:
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

### Q6: How to handle security in a multi-tenant context? (Multiple organizations/clients using the same application)
**A:**

**Multi-Tenant architecture with security isolation**:

**1. Identify the tenant**:
```java
/**
 * Strategies to identify the tenant
 */
public enum TenantIdentificationStrategy {
    
    // 1. By subdomain: tenant1.myapp.com, tenant2.myapp.com
    SUBDOMAIN,
    
    // 2. By HTTP header: X-Tenant-ID
    HEADER,
    
    // 3. By path: /tenant1/api/..., /tenant2/api/...
    PATH,
    
    // 4. By JWT claim (recommended)
    JWT_CLAIM
}

/**
 * Tenant resolver from JWT
 */
@Component
public class JwtTenantResolver {
    
    public String resolveTenant(Jwt jwt) {
        // The tenant is stored in the JWT during authentication
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

**2. Tenant Context (ThreadLocal)**:
```java
/**
 * Storing the tenant in the request context
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
 * Filter to extract and store the tenant
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
                
                // Extract the tenant ID
                String tenantId = tenantResolver.resolveTenant(jwt);
                
                // Store in context
                TenantContext.setTenant(tenantId);
                
                // Add to header for microservices
                ServerHttpRequest modifiedRequest = exchange.getRequest().mutate()
                    .header("X-Tenant-ID", tenantId)
                    .build();
                
                return chain.filter(exchange.mutate().request(modifiedRequest).build())
                    .doFinally(signalType -> TenantContext.clear());
            });
    }
    
    @Override
    public int getOrder() {
        return -90;  // After authentication, before authorization
    }
}
```

**3. Data isolation by tenant**:
```java
/**
 * Entity with tenant
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
 * Repository with automatic tenant filtering
 */
@Repository
public interface AccountRepository extends JpaRepository<Account, Long> {
    
    // Query automatically filtered by tenant (thanks to @Filter)
    List<Account> findAll();
    
    Optional<Account> findById(Long id);
    
    // Custom query if you need to override the filter
    @Query("SELECT a FROM Account a WHERE a.tenantId = :tenantId")
    List<Account> findAllByTenant(@Param("tenantId") String tenantId);
}

/**
 * Hibernate Interceptor to enable the tenant filter
 */
@Component
public class TenantInterceptor extends EmptyInterceptor {
    
    @Override
    public String onPrepareStatement(String sql) {
        // The filter will be applied automatically
        return super.onPrepareStatement(sql);
    }
}

/**
 * JPA configuration to enable the filter
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

**4. Cross-tenant validation**:
```java
/**
 * Multi-tenant security service
 */
@Service
public class MultiTenantSecurityService {
    
    @Autowired
    private JwtTenantResolver tenantResolver;
    
    /**
     * Checks that the resource belongs to the user's tenant
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
     * Gets the tenant ID of a resource
     */
    private String getResourceTenantId(Long resourceId, String resourceType) {
        // Logic to get tenant based on the resource type
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
 * Usage in the controllers
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

**5. Keycloak Multi-Tenant Configuration**:
```
Strategy 1: One Realm per tenant
- Realm "tenant1"
- Realm "tenant2"
- Advantage: Complete isolation
- Disadvantage: Complex management

Strategy 2: One Realm, tenant attributes (Recommended)
- Realm "microservices"
- User attributes: tenant_id = "tenant1"
- Group per tenant: "tenant1-users", "tenant2-users"
- Mapper to add tenant_id to the JWT
```

**Keycloak Mapper to add tenant_id to JWT**:
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

**6. Multi-tenant tests**:
```java
@SpringBootTest
public class MultiTenantSecurityTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    /**
     * Test: User from tenant1 cannot access tenant2 data
     */
    @Test
    @WithMockJwt(subject = "user1", tenantId = "tenant1", roles = {"USER"})
    public void testCrossTenantAccess_ShouldBeForbidden() throws Exception {
        // Create an account for tenant2
        Long tenant2AccountId = createAccountForTenant("tenant2");
        
        // User from tenant1 tries to access
        mockMvc.perform(get("/api/accounts/" + tenant2AccountId))
            .andExpect(status().isForbidden())
            .andExpect(jsonPath("$.error").value("Cross-tenant access denied"));
    }
    
    /**
     * Test: User can only access their own tenant data
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
     * Test: Complete data isolation in list
     */
    @Test
    @WithMockJwt(subject = "user1", tenantId = "tenant1", roles = {"USER"})
    public void testListAccounts_ShouldReturnOnlyTenantData() throws Exception {
        createAccountForTenant("tenant1");
        createAccountForTenant("tenant1");
        createAccountForTenant("tenant2");  // Should not appear
        
        mockMvc.perform(get("/api/accounts"))
            .andExpect(status().isOk())
            .andExpect(jsonPath("$.length()").value(2))
            .andExpect(jsonPath("$[*].tenantId").value(everyItem(equalTo("tenant1"))));
    }
}
```

**Multi-Tenant Best Practices**:
```yaml
✅ Always validate tenant_id in each request
✅ Use Hibernate filters for automatic isolation
✅ Log all attempted cross-tenant accesses
✅ Exhaustive isolation tests
✅ Encryption of sensitive data per tenant
✅ Separate backup per tenant
✅ Rate limiting per tenant
✅ Monitoring of metrics per tenant
❌ NEVER trust the client's tenant_id
❌ NEVER allow changing tenant without re-authentication
```

This concludes the complete answer on multi-tenancy!