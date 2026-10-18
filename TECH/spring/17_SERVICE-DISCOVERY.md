## [**..**](./00_index.md)

# Service Discovery : Client-Side vs Server-Side

## 1. Service Discovery Client-Side (Eureka)

### Concept
Le **client-side service discovery** signifie que le client est responsable de déterminer les locations des instances de services disponibles et de faire le **load balancing** entre elles.

### Eureka - Architecture

**Eureka Server** (Netflix OSS) :
- Registry centralisé qui maintient la liste de tous les services
- Les services s'enregistrent au démarrage (**self-registration**)
- Envoie des **heartbeats** réguliers (par défaut toutes les 30s)

**Eureka Client** :
- Intégré dans chaque microservice
- Récupère le **registry** et le met en cache localement
- Fait le **client-side load balancing** avec Ribbon/Spring Cloud LoadBalancer

### Exemple Spring Boot avec Eureka

**Eureka Server :**
```java
@SpringBootApplication
@EnableEurekaServer
public class EurekaServerApplication {
    public static void main(String[] args) {
        SpringApplication.run(EurekaServerApplication.class, args);
    }
}
```

**application.yml (Eureka Server) :**
```yaml
server:
  port: 8761

eureka:
  client:
    register-with-eureka: false
    fetch-registry: false
  server:
    enable-self-preservation: true
```

**Eureka Client (Microservice) :**
```java
@SpringBootApplication
@EnableDiscoveryClient
public class OrderServiceApplication {
    public static void main(String[] args) {
        SpringApplication.run(OrderServiceApplication.class, args);
    }
}

@RestController
public class OrderController {
    
    @Autowired
    private RestTemplate restTemplate; // avec @LoadBalanced
    
    @GetMapping("/orders/{id}")
    public Order getOrder(@PathVariable Long id) {
        // Appel via service name au lieu d'IP:PORT
        User user = restTemplate.getForObject(
            "http://USER-SERVICE/users/123", 
            User.class
        );
        return new Order(id, user);
    }
}
```

**application.yml (Client) :**
```yaml
spring:
  application:
    name: order-service

eureka:
  client:
    service-url:
      defaultZone: http://localhost:8761/eureka/
  instance:
    prefer-ip-address: true
    lease-renewal-interval-in-seconds: 30
```

### Avantages Eureka
- ✅ **Autonomie** : Les clients peuvent fonctionner même si Eureka tombe (grace au cache local)
- ✅ **Load balancing flexible** : Algorithmes personnalisables côté client (round-robin, weighted, etc.)
- ✅ **Health checks intégrés** : Détection automatique des instances défaillantes
- ✅ **Pas de single point of failure** : Le registry est repliqué entre serveurs Eureka
- ✅ **Self-preservation mode** : Protège contre les pannes réseau massives

### Inconvénients Eureka
- ❌ **Infrastructure supplémentaire** : Nécessite de déployer et maintenir Eureka Server
- ❌ **Dépendance applicative** : Chaque service doit intégrer le client Eureka
- ❌ **Latence de propagation** : Délai avant qu'un nouveau service soit visible (~30-60s)
- ❌ **Complexité** : Configuration et tuning du registry, heartbeats, eviction
- ❌ **Polyglotte limité** : Difficile d'intégrer des services non-JVM

---

## 2. Service Discovery Server-Side (Kubernetes)

### Concept
Le **server-side service discovery** signifie qu'un composant d'infrastructure (load balancer, proxy) gère la découverte et le routing. Les clients n'ont pas conscience de ce mécanisme.

### Kubernetes - Architecture

**Core DNS** :
- DNS interne qui résout les noms de **Services** vers leurs IPs
- Automatique, pas de configuration nécessaire

**Service** :
- Abstraction stable devant un ensemble de **Pods**
- Fournit un **DNS name**, une **ClusterIP** fixe
- Fait le **load balancing** automatique entre les Pods

**Ingress** :
- Point d'entrée HTTP/HTTPS depuis l'extérieur
- Routing basé sur hostname/path
- Nécessite un **Ingress Controller** (Nginx, Traefik, etc.)

### Exemple Kubernetes

**Deployment :**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
spec:
  replicas: 3
  selector:
    matchLabels:
      app: order-service
  template:
    metadata:
      labels:
        app: order-service
    spec:
      containers:
      - name: order-service
        image: mycompany/order-service:1.0
        ports:
        - containerPort: 8080
        env:
        - name: USER_SERVICE_URL
          value: "http://user-service:8080"  # DNS interne K8s
        livenessProbe:
          httpGet:
            path: /actuator/health
            port: 8080
          initialDelaySeconds: 30
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
```

**Service (ClusterIP) :**
```yaml
apiVersion: v1
kind: Service
metadata:
  name: order-service
spec:
  type: ClusterIP
  selector:
    app: order-service
  ports:
  - protocol: TCP
    port: 8080        # Port du Service
    targetPort: 8080  # Port du container
```

**Service pour accès externe (LoadBalancer) :**
```yaml
apiVersion: v1
kind: Service
metadata:
  name: order-service-external
spec:
  type: LoadBalancer
  selector:
    app: order-service
  ports:
  - protocol: TCP
    port: 80
    targetPort: 8080
```

**Ingress :**
```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: api-ingress
  annotations:
    nginx.ingress.kubernetes.io/rewrite-target: /
spec:
  ingressClassName: nginx
  rules:
  - host: api.mycompany.com
    http:
      paths:
      - path: /orders
        pathType: Prefix
        backend:
          service:
            name: order-service
            port:
              number: 8080
      - path: /users
        pathType: Prefix
        backend:
          service:
            name: user-service
            port:
              number: 8080
  tls:
  - hosts:
    - api.mycompany.com
    secretName: tls-secret
```

**Application Spring Boot (simplifié) :**
```java
@RestController
public class OrderController {
    
    @Value("${user.service.url}")
    private String userServiceUrl; // http://user-service:8080
    
    private final RestTemplate restTemplate;
    
    @GetMapping("/orders/{id}")
    public Order getOrder(@PathVariable Long id) {
        // Appel direct via DNS K8s
        User user = restTemplate.getForObject(
            userServiceUrl + "/users/123", 
            User.class
        );
        return new Order(id, user);
    }
}
```

### Resources Kubernetes essentielles

| Resource | Rôle |
|----------|------|
| **Pod** | Unité de déploiement (1+ containers) |
| **Deployment** | Gère les replicas, rolling updates |
| **Service** | Service discovery + load balancing interne |
| **Ingress** | Routage HTTP(S) depuis l'extérieur |
| **ConfigMap** | Configuration externalisée (non-sensible) |
| **Secret** | Configuration sensible (credentials, certificates) |
| **PersistentVolume** | Stockage persistant |
| **Namespace** | Isolation logique des resources |

### Avantages Kubernetes
- ✅ **Plateforme standard** : Discovery intégré, pas de code spécifique
- ✅ **Polyglotte** : Fonctionne avec n'importe quel langage/framework
- ✅ **Health checks natifs** : liveness/readiness probes
- ✅ **Infrastructure as Code** : Tout déclaratif via YAML
- ✅ **Scaling automatique** : HPA (Horizontal Pod Autoscaler)
- ✅ **Zero downtime deployments** : Rolling updates natifs
- ✅ **Écosystème riche** : Service mesh (Istio, Linkerd), observability

### Inconvénients Kubernetes
- ❌ **Complexité opérationnelle** : Courbe d'apprentissage steep
- ❌ **Infrastructure lourde** : Nécessite un cluster K8s (coût, maintenance)
- ❌ **Overkill pour petits projets** : Complexité disproportionnée
- ❌ **Latence DNS** : Peut être plus lente que cache local (mitigé par cache DNS)
- ❌ **Debugging difficile** : Plus de layers d'abstraction

---

## 3. Comparaison Synthétique

| Critère | Eureka (Client-Side) | Kubernetes (Server-Side) |
|---------|---------------------|-------------------------|
| **Discovery location** | Client fait le lookup | Infrastructure DNS/Service |
| **Load balancing** | Client-side (Ribbon/LoadBalancer) | Server-side (kube-proxy) |
| **Dépendance code** | Forte (client library) | Aucune (DNS standard) |
| **Polyglotte** | Difficile (Netflix stack) | Natif |
| **Résilience** | Cache local | Dépend de CoreDNS/kube-proxy |
| **Latence discovery** | 30-60s (heartbeat) | Quasi-instantané |
| **Complexité** | Moyenne (registry) | Élevée (cluster entier) |
| **Use case idéal** | Micro-services Spring Cloud | Applications cloud-native multi-langage |

### Approche Hybride
Beaucoup d'organisations utilisent **les deux** :
- **Kubernetes** pour le service discovery de base
- **Service Mesh** (Istio, Linkerd) pour des features avancées : circuit breaking, retry, mTLS, observability

```yaml
# Exemple avec Istio
apiVersion: networking.istio.io/v1beta1
kind: VirtualService
metadata:
  name: order-service
spec:
  hosts:
  - order-service
  http:
  - route:
    - destination:
        host: order-service
        subset: v1
      weight: 90
    - destination:
        host: order-service
        subset: v2
      weight: 10  # Canary deployment
    retries:
      attempts: 3
      perTryTimeout: 2s
```

---

## Conseil pour ton entretien

**Question probable** : "Pourquoi migrer d'Eureka vers Kubernetes ?"

**Réponse solide** :
- Eureka était adapté pour notre stack 100% Spring Cloud
- Avec l'adoption de microservices polyglotte (Go, Python), le client-side discovery devient limitant
- Kubernetes offre une plateforme unifiée : discovery + orchestration + scaling + deployments
- Réduction de la complexité applicative en déléguant à l'infrastructure
- Mais on garde Eureka pour le legacy en phase de transition

# Questions/Réponses - Service Discovery (Entretien Oral)

## Questions Fondamentales

### Q1: Peux-tu m'expliquer ce qu'est le service discovery et pourquoi c'est important ?

**Réponse :**
Le service discovery est un mécanisme qui permet aux microservices de se localiser dynamiquement dans un environnement distribué. 

Dans une architecture monolithique, on a des URLs fixes, mais avec les microservices, les instances peuvent démarrer/s'arrêter à tout moment, changer d'IP, scaler horizontalement. Le service discovery résout ce problème en maintenant un registry dynamique.

C'est important car ça permet :
- L'**auto-scaling** : nouvelles instances automatiquement découvertes
- La **résilience** : retrait automatique des instances défaillantes
- Le **load balancing** : distribution du traffic entre instances
- La **simplicité** : appeler par nom logique au lieu d'IP:PORT hardcodées

---

### Q2: Quelle est la différence entre client-side et server-side discovery ?

**Réponse :**
**Client-side discovery** (comme Eureka) :
- Le client interroge le registry pour obtenir la liste des instances
- Le client fait lui-même le load balancing
- Exemple : Mon service Order appelle Eureka, récupère 3 IPs de User-Service, choisit une IP

**Server-side discovery** (comme Kubernetes) :
- Le client appelle un load balancer ou router
- C'est l'infrastructure qui fait le routing
- Exemple : Mon service Order appelle `http://user-service`, le Service Kubernetes route automatiquement

**Analogie :** 
- Client-side = Tu cherches toi-même dans l'annuaire puis tu appelles
- Server-side = Tu appelles un standard qui te transfère automatiquement

---

### Q3: Comment fonctionne Eureka concrètement ?

**Réponse :**
Eureka fonctionne en 3 étapes :

**1. Registration :**
```
Au démarrage → Service envoie POST /eureka/apps/{APP_NAME}
Données : IP, port, metadata, health check URL
```

**2. Heartbeat :**
```
Toutes les 30s → Service envoie PUT /eureka/apps/{APP_NAME}/{INSTANCE_ID}
Si pas de heartbeat pendant 90s → éviction du registry
```

**3. Fetch Registry :**
```
Client récupère le registry (toutes les 30s par défaut)
Cache local → Si Eureka down, continue avec le cache
```

**Self-Preservation Mode :** Si 15% des services manquent leur heartbeat, Eureka suppose un problème réseau et arrête d'évincer pour éviter une cascade de suppressions.

---

### Q4: Qu'est-ce qu'un Service dans Kubernetes ?

**Réponse :**
Un Service Kubernetes est une abstraction qui expose un ensemble de Pods sous un nom DNS et une IP stable.

**Les types principaux :**

**ClusterIP** (par défaut) :
- IP virtuelle interne au cluster
- Accessible uniquement depuis l'intérieur
- Use case : communication inter-services

**NodePort** :
- Expose sur un port sur chaque node (30000-32767)
- Accessible depuis l'extérieur via NodeIP:NodePort
- Use case : développement, tests

**LoadBalancer** :
- Crée un load balancer cloud (AWS ELB, GCP LB)
- IP publique externe
- Use case : exposition production

**Exemple concret :**
```yaml
# Service crée automatiquement des endpoints vers les Pods
# avec le label app: order-service
# DNS : order-service.default.svc.cluster.local
# Ou simplement : order-service
```

---

## Questions Techniques Avancées

### Q5: Comment gères-tu le problème du "cache staleness" avec Eureka ?

**Réponse :**
Le cache staleness est le délai entre un changement (nouveau service, instance down) et sa propagation.

**Dans Eureka, plusieurs niveaux de cache :**
1. **Server cache** : Eureka met en cache le registry (30s par défaut)
2. **Client cache** : Client fetch toutes les 30s
3. **Ribbon cache** : Ribbon cache les instances disponibles

**Total worst case :** 90 secondes de latence

**Solutions :**
```yaml
# Réduire les intervalles (attention à la charge)
eureka:
  instance:
    lease-renewal-interval-in-seconds: 10  # heartbeat
    lease-expiration-duration-in-seconds: 30  # eviction
  client:
    registry-fetch-interval-seconds: 10  # fetch
```

**Alternative :** Utiliser Spring Cloud Consul qui supporte le long-polling et réduit la latence.

**En production :** On accepte généralement le délai car il protège contre les "flapping services".

---

### Q6: Comment Kubernetes gère-t-il le load balancing ?

**Réponse :**
Kubernetes utilise **kube-proxy** qui tourne sur chaque node.

**3 modes :**

**1. iptables (défaut) :**
```
Client → iptables rules → random Pod selection
Avantage : Performance
Inconvénient : Pas de retry si Pod down
```

**2. IPVS :**
```
Client → IPVS load balancer → Pod (round-robin, least connection...)
Avantage : Algorithmes avancés, meilleure performance
```

**3. userspace (legacy) :**
```
Client → kube-proxy (userspace) → Pod
Lent, deprecated
```

**Exemple concret :**
```bash
# kube-proxy crée des iptables rules comme :
-A KUBE-SERVICES -d 10.96.0.1/32 -p tcp -m tcp --dport 8080 \
  -j KUBE-SVC-XXXX

# Qui redirige vers un des Pods backends
-A KUBE-SVC-XXXX -m statistic --mode random --probability 0.33 \
  -j KUBE-SEP-POD1
```

---

### Q7: Qu'est-ce qu'un Ingress et pourquoi l'utiliser ?

**Réponse :**
Un Ingress est un objet Kubernetes qui gère l'accès HTTP(S) externe vers les Services.

**Pourquoi pas juste un Service LoadBalancer ?**
- Un LoadBalancer = 1 IP publique = coût
- Avec 10 services = 10 LoadBalancers = 10x le coût

**Ingress = 1 point d'entrée pour N services**

**Features :**
- **Path-based routing** : /api/orders → order-service, /api/users → user-service
- **Host-based routing** : api.company.com, admin.company.com
- **TLS termination** : Gestion centralisée des certificats SSL
- **Rewrites, redirects**

**Exemple réel :**
```yaml
# 1 seul LoadBalancer public
# Route selon le path
/orders/* → order-service:8080
/users/* → user-service:8080
/payments/* → payment-service:8080

# TLS automatique avec cert-manager
annotations:
  cert-manager.io/cluster-issuer: "letsencrypt-prod"
```

**Attention :** L'Ingress n'est qu'une définition. Il faut un **Ingress Controller** (Nginx, Traefik, HAProxy) pour l'implémenter.

---

### Q8: Comment assures-tu la haute disponibilité avec Eureka ?

**Réponse :**
Plusieurs stratégies :

**1. Eureka Server Cluster :**
```yaml
# Eureka Server 1
eureka:
  client:
    service-url:
      defaultZone: http://eureka-2:8761/eureka/,http://eureka-3:8761/eureka/

# Les serveurs se répliquent entre eux (peer-to-peer)
```

**2. Client resilience :**
```yaml
eureka:
  client:
    # Plusieurs URLs de fallback
    service-url:
      defaultZone: http://eureka-1:8761/eureka/,http://eureka-2:8761/eureka/
    
    # Le client conserve le cache si tous les serveurs sont down
    fetch-registry: true
```

**3. Circuit Breaker Pattern :**
```java
@HystrixCommand(fallbackMethod = "fallbackUser")
public User getUser(Long id) {
    return restTemplate.getForObject(
        "http://USER-SERVICE/users/" + id, 
        User.class
    );
}

public User fallbackUser(Long id) {
    return new User(id, "Unknown"); // Dégradation gracieuse
}
```

**4. Health Checks multiples :**
- Eureka heartbeat
- Spring Boot Actuator `/health`
- Monitoring externe (Prometheus, Datadog)

---

### Q9: Quels sont les health checks dans Kubernetes ?

**Réponse :**
Kubernetes a 3 types de probes :

**1. Liveness Probe :**
- "Le container est-il vivant ?"
- Si fail → Kubernetes **restart** le container
- Use case : deadlock, memory leak

```yaml
livenessProbe:
  httpGet:
    path: /actuator/health/liveness
    port: 8080
  initialDelaySeconds: 60  # Attendre le démarrage
  periodSeconds: 10
  failureThreshold: 3  # 3 échecs avant restart
```

**2. Readiness Probe :**
- "Le container est-il prêt à recevoir du traffic ?"
- Si fail → Kubernetes **retire du Service** (pas de restart)
- Use case : warm-up, dépendances non prêtes

```yaml
readinessProbe:
  httpGet:
    path: /actuator/health/readiness
    port: 8080
  initialDelaySeconds: 30
  periodSeconds: 5
```

**3. Startup Probe :**
- "Le container a-t-il fini de démarrer ?"
- Pour les apps avec long startup (> 1 minute)
- Désactive liveness pendant le startup

**Piège classique :** 
Ne pas confondre liveness et readiness ! Si liveness fail trop facilement, on peut avoir un restart loop infini.

**Bonne pratique Spring Boot :**
```yaml
management:
  endpoint:
    health:
      probes:
        enabled: true  # Active /health/liveness et /health/readiness
  health:
    readinessState:
      enabled: true
    livenessState:
      enabled: true
```

---

## Questions de Design/Architecture

### Q10: Dans quel cas choisir Eureka vs Kubernetes ?

**Réponse :**

**Choisis Eureka si :**
- ✅ Stack 100% Spring Cloud (synergy avec Config Server, Gateway, etc.)
- ✅ Pas d'infrastructure Kubernetes disponible
- ✅ Besoin de client-side load balancing avancé (weighted, zone-aware)
- ✅ Équipe confortable avec Netflix stack
- ✅ Projet de taille moyenne (< 50 services)

**Choisis Kubernetes si :**
- ✅ Architecture polyglotte (Java, Go, Python, Node.js...)
- ✅ Besoin d'orchestration complète (scaling, rolling updates, resources management)
- ✅ Infrastructure cloud-native
- ✅ Grande échelle (> 50 services)
- ✅ Besoin de features avancées (service mesh, CRDs, operators)

**Mon avis personnel :**
Pour un nouveau projet en 2025, je recommande **Kubernetes** car :
- Standard de l'industrie
- Évite la vendor lock-in
- Écosystème mature (Helm, Operators, CNCF tools)
- Équipes infra/SRE déjà formées

Mais si tu as déjà du legacy Spring Cloud avec Eureka, pas besoin de migrer tout de suite. Approche incrémentale.

---

### Q11: Comment migrerais-tu d'Eureka vers Kubernetes ?

**Réponse :**

**Phase 1 : Dual Stack (3-6 mois)**
```
┌─────────────────────────────────────┐
│  Eureka Server (legacy)             │
│  ├─ Old Services (Eureka clients)   │
│  └─ New Services (+ Eureka clients) │
└─────────────────────────────────────┘

┌─────────────────────────────────────┐
│  Kubernetes Cluster                 │
│  └─ New Services (K8s native)       │
└─────────────────────────────────────┘
```

**Phase 2 : Bridge Pattern**
```java
// Service supporte les 2 modes
@Configuration
public class DiscoveryConfig {
    
    @Bean
    @ConditionalOnProperty(name = "discovery.mode", havingValue = "eureka")
    public RestTemplate eurekaRestTemplate() {
        return new RestTemplate(); // avec @LoadBalanced
    }
    
    @Bean
    @ConditionalOnProperty(name = "discovery.mode", havingValue = "kubernetes")
    public RestTemplate k8sRestTemplate() {
        return new RestTemplate(); // standard, DNS K8s
    }
}
```

**Phase 3 : Migration progressive**
1. Nouveaux services → K8s only
2. Services à faible couplage → K8s
3. Services critiques → derniers (avec rollback plan)

**Phase 4 : Decommission Eureka**
- Monitoring pour valider zéro traffic
- Shutdown Eureka servers

**Risques à gérer :**
- Latence cross-cluster
- Gestion des transactions distribuées
- Formation équipes

**Durée réaliste :** 12-18 mois pour grosse app

---

### Q12: Qu'est-ce qu'un Service Mesh et comment ça se compare à Eureka ?

**Réponse :**

Un Service Mesh (Istio, Linkerd) est une couche d'infrastructure dédiée à la communication inter-services.

**Architecture :**
```
Service A → Sidecar Proxy (Envoy) → Sidecar Proxy → Service B
              ↓                           ↓
         Control Plane (Istiod)
```

**Comparaison :**

| Feature | Eureka | Kubernetes | Service Mesh |
|---------|--------|------------|--------------|
| Discovery | ✅ | ✅ | ✅ |
| Load Balancing | ✅ Client-side | ✅ Server-side | ✅ Proxy-side |
| Circuit Breaking | ❌ (Hystrix séparé) | ❌ | ✅ |
| Retry Logic | ❌ (code) | ❌ | ✅ |
| Mutual TLS | ❌ | ❌ (manuel) | ✅ Automatique |
| Traffic Shifting | ❌ | ⚠️ Limité | ✅ Canary, A/B |
| Observability | ⚠️ Basique | ⚠️ Basique | ✅ Metrics, Traces |

**Exemple Istio - Canary Deployment :**
```yaml
apiVersion: networking.istio.io/v1beta1
kind: VirtualService
metadata:
  name: user-service
spec:
  hosts:
  - user-service
  http:
  - match:
    - headers:
        x-beta-user:
          exact: "true"
    route:
    - destination:
        host: user-service
        subset: v2  # Beta users → v2
  - route:
    - destination:
        host: user-service
        subset: v1
      weight: 95  # 95% traffic
    - destination:
        host: user-service
        subset: v2
      weight: 5   # 5% traffic (canary)
```

**Quand utiliser un Service Mesh ?**
- 50+ microservices
- Besoin de mTLS automatique
- Observability avancée requise
- Team SRE dédiée

**Coût :** Complexité + latency (proxy overhead ~1-5ms) + resources (1 proxy par pod)

---

### Q13: Comment gères-tu la configuration dans ces deux approches ?

**Réponse :**

**Avec Eureka (Spring Cloud Config) :**
```yaml
# Config Server centralisé
spring:
  cloud:
    config:
      server:
        git:
          uri: https://github.com/company/config-repo
          search-paths: '{application}'
```

```
# Structure Git
config-repo/
├── order-service.yml         # Config par service
├── order-service-dev.yml     # Config par environnement
├── order-service-prod.yml
└── application.yml           # Config commune
```

```java
// Client auto-refresh avec @RefreshScope
@RestController
@RefreshScope
public class OrderController {
    
    @Value("${tax.rate}")
    private Double taxRate;  // Refresh via POST /actuator/refresh
}
```

**Avec Kubernetes :**

**ConfigMap (non-sensible) :**
```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: order-service-config
data:
  application.yml: |
    server:
      port: 8080
    tax:
      rate: 0.20
```

**Secret (sensible) :**
```yaml
apiVersion: v1
kind: Secret
metadata:
  name: order-service-secrets
type: Opaque
data:
  db-password: cGFzc3dvcmQxMjM=  # base64
```

**Montage dans le Pod :**
```yaml
spec:
  containers:
  - name: order-service
    env:
    - name: TAX_RATE
      valueFrom:
        configMapKeyRef:
          name: order-service-config
          key: tax.rate
    - name: DB_PASSWORD
      valueFrom:
        secretKeyRef:
          name: order-service-secrets
          key: db-password
    volumeMounts:
    - name: config
      mountPath: /config
  volumes:
  - name: config
    configMap:
      name: order-service-config
```

**Refresh sans restart :**
```
Option 1 : External Config (Spring Cloud Kubernetes Config)
Option 2 : ConfigMap reload avec Reloader operator
Option 3 : Rolling restart automatique
```

**Best practice :** Secrets sensibles → **External Secrets Operator** qui sync depuis Vault, AWS Secrets Manager, etc.

---

### Q14: Parle-moi du problème du "thundering herd" avec Eureka

**Réponse :**

Le **thundering herd** (stampede) se produit quand tous les clients font la même action simultanément.

**Scénario avec Eureka :**
```
1. Eureka Server redémarre
2. Tous les clients (100 services × 10 instances = 1000 clients)
   tentent de se reconnecter EN MÊME TEMPS
3. Eureka est submergé → crash → redémarre → boucle
```

**Autre cas : Service redémarre**
```
1. USER-SERVICE redémarre (10 instances)
2. Toutes envoient un heartbeat simultanément
3. Toutes les 10 instances apparaissent "disponibles" en même temps
4. ORDER-SERVICE envoie un burst de requêtes
5. Les instances ne sont pas encore "warm" → timeouts
```

**Solutions :**

**1. Jitter (décalage aléatoire) :**
```java
// Au lieu de heartbeat toutes les 30s exactement
int interval = 30 + random.nextInt(10); // 30-40s
```

**2. Graceful startup :**
```yaml
eureka:
  instance:
    initial-status: STARTING  # Pas "UP" immédiatement
    
management:
  endpoint:
    health:
      readiness:
        enabled: true  # Attend que ready avant de recevoir traffic
```

**3. Circuit Breaker :**
```java
@CircuitBreaker(name = "userService", fallbackMethod = "fallback")
public User getUser(Long id) {
    // Si trop d'erreurs, circuit s'ouvre automatiquement
}
```

**4. Rate Limiting :**
```yaml
# Limiter le nombre de reconnexions
eureka:
  client:
    registry-fetch-interval-seconds: 30
    initial-instance-info-replication-interval-seconds: 40
```

**Kubernetes gère mieux :** Les readiness probes empêchent le traffic avant que le Pod soit ready, et le rollout est progressif (maxSurge: 1, maxUnavailable: 0).

---

### Q15: Donne-moi un exemple de debugging d'un problème de service discovery

**Réponse :**

**Scénario réel :** Order-Service ne trouve pas User-Service

**Étape 1 : Vérifier le registry (Eureka)**
```bash
# Appeler l'API Eureka
curl http://eureka-server:8761/eureka/apps

# Vérifier si USER-SERVICE est présent
# Vérifier le status : UP ou DOWN ?
# Vérifier l'IP/port
```

**Étape 2 : Vérifier les logs du client**
```
# Rechercher dans les logs
ERROR - DiscoveryClient - Cannot fetch registry from server

# Causes possibles :
- Firewall bloque le port 8761
- Mauvaise URL dans eureka.client.service-url.defaultZone
- Eureka server down
```

**Étape 3 : Tester la connectivité**
```bash
# Depuis le Pod Order-Service
kubectl exec -it order-service-pod -- sh

# Ping le service (si possible)
ping user-service

# Curl directement
curl http://USER-SERVICE-IP:8080/actuator/health
```

**Étape 4 : Vérifier le cache client**
```java
// Exposer un endpoint de debug
@GetMapping("/debug/registry")
public List<ServiceInstance> getRegistry() {
    return discoveryClient.getInstances("USER-SERVICE");
}
```

**Kubernetes :**
```bash
# Vérifier le Service
kubectl get svc user-service
kubectl describe svc user-service

# Vérifier les Endpoints (Pods derrière le Service)
kubectl get endpoints user-service

# Si vide → problème de selector
# Vérifier les labels des Pods
kubectl get pods -l app=user-service --show-labels

# Test DNS
kubectl exec -it order-service-pod -- nslookup user-service

# Vérifier kube-proxy logs
kubectl logs -n kube-system -l k8s-app=kube-proxy
```

**Bug classique :**
```yaml
# Service selector
selector:
  app: user-service  # ⚠️ Attention à la casse

# Pod labels
labels:
  app: User-Service  # ❌ Pas de match ! (U majuscule)
```

**Fix :** Uniformiser les labels

**Outil pro :** Utiliser **ksniff** (Wireshark pour K8s) pour capturer le traffic réseau

---
