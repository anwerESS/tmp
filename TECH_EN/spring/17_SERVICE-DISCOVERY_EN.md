## [**..**](./00_index.md)

# Service Discovery: Client-Side vs Server-Side

## 1. Client-Side Service Discovery (Eureka)

### Concept
**Client-side service discovery** means that the client is responsible for determining the locations of available service instances and doing the **load balancing** between them.

### Eureka - Architecture

**Eureka Server** (Netflix OSS):
- Centralized registry that maintains the list of all services
- Services register themselves at startup (**self-registration**)
- Sends regular **heartbeats** (every 30s by default)

**Eureka Client**:
- Integrated into each microservice
- Fetches the **registry** and caches it locally
- Does **client-side load balancing** with Ribbon/Spring Cloud LoadBalancer

### Spring Boot Example with Eureka

**Eureka Server:**
```java
@SpringBootApplication
@EnableEurekaServer
public class EurekaServerApplication {
    public static void main(String[] args) {
        SpringApplication.run(EurekaServerApplication.class, args);
    }
}
```

**application.yml (Eureka Server):**
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

**Eureka Client (Microservice):**
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
    private RestTemplate restTemplate; // with @LoadBalanced
    
    @GetMapping("/orders/{id}")
    public Order getOrder(@PathVariable Long id) {
        // Call via service name instead of IP:PORT
        User user = restTemplate.getForObject(
            "http://USER-SERVICE/users/123", 
            User.class
        );
        return new Order(id, user);
    }
}
```

**application.yml (Client):**
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

### Eureka Advantages
- ✅ **Autonomy**: Clients can work even if Eureka goes down (thanks to local cache)
- ✅ **Flexible load balancing**: Customizable algorithms on the client side (round-robin, weighted, etc.)
- ✅ **Built-in health checks**: Automatic detection of failing instances
- ✅ **No single point of failure**: The registry is replicated across Eureka servers
- ✅ **Self-preservation mode**: Protects against massive network failures

### Eureka Disadvantages
- ❌ **Additional infrastructure**: Requires deploying and maintaining Eureka Server
- ❌ **Application dependency**: Each service must integrate the Eureka client
- ❌ **Propagation latency**: Delay before a new service is visible (~30-60s)
- ❌ **Complexity**: Registry configuration and tuning, heartbeats, eviction
- ❌ **Limited polyglot**: Difficult to integrate non-JVM services

---

## 2. Server-Side Service Discovery (Kubernetes)

### Concept
**Server-side service discovery** means that an infrastructure component (load balancer, proxy) manages discovery and routing. Clients are not aware of this mechanism.

### Kubernetes - Architecture

**Core DNS**:
- Internal DNS that resolves **Service** names to their IPs
- Automatic, no configuration needed

**Service**:
- Stable abstraction in front of a set of **Pods**
- Provides a **DNS name** and a fixed **ClusterIP**
- Does automatic **load balancing** between Pods

**Ingress**:
- HTTP/HTTPS entry point from outside the cluster
- Routing based on hostname/path
- Requires an **Ingress Controller** (Nginx, Traefik, etc.)

### Kubernetes Example

**Deployment:**
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
          value: "http://user-service:8080"  # K8s internal DNS
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

**Service (ClusterIP):**
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
    port: 8080        # Service port
    targetPort: 8080  # Container port
```

**Service for external access (LoadBalancer):**
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

**Ingress:**
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

**Spring Boot Application (simplified):**
```java
@RestController
public class OrderController {
    
    @Value("${user.service.url}")
    private String userServiceUrl; // http://user-service:8080
    
    private final RestTemplate restTemplate;
    
    @GetMapping("/orders/{id}")
    public Order getOrder(@PathVariable Long id) {
        // Direct call via K8s DNS
        User user = restTemplate.getForObject(
            userServiceUrl + "/users/123", 
            User.class
        );
        return new Order(id, user);
    }
}
```

### Essential Kubernetes Resources

| Resource | Role |
|----------|------|
| **Pod** | Deployment unit (1+ containers) |
| **Deployment** | Manages replicas, rolling updates |
| **Service** | Service discovery + internal load balancing |
| **Ingress** | External HTTP(S) routing |
| **ConfigMap** | Externalized configuration (non-sensitive) |
| **Secret** | Sensitive configuration (credentials, certificates) |
| **PersistentVolume** | Persistent storage |
| **Namespace** | Logical isolation of resources |

### Kubernetes Advantages
- ✅ **Standard platform**: Built-in discovery, no specific code needed
- ✅ **Polyglot**: Works with any language/framework
- ✅ **Native health checks**: liveness/readiness probes
- ✅ **Infrastructure as Code**: Everything declarative via YAML
- ✅ **Automatic scaling**: HPA (Horizontal Pod Autoscaler)
- ✅ **Zero downtime deployments**: Native rolling updates
- ✅ **Rich ecosystem**: Service mesh (Istio, Linkerd), observability

### Kubernetes Disadvantages
- ❌ **Operational complexity**: Steep learning curve
- ❌ **Heavy infrastructure**: Requires a K8s cluster (cost, maintenance)
- ❌ **Overkill for small projects**: Disproportionate complexity
- ❌ **DNS latency**: Can be slower than local cache (mitigated by DNS cache)
- ❌ **Difficult debugging**: More abstraction layers

---

## 3. Comparison Summary

| Criteria | Eureka (Client-Side) | Kubernetes (Server-Side) |
|---------|---------------------|-------------------------|
| **Discovery location** | Client does the lookup | Infrastructure DNS/Service |
| **Load balancing** | Client-side (Ribbon/LoadBalancer) | Server-side (kube-proxy) |
| **Code dependency** | Strong (client library) | None (standard DNS) |
| **Polyglot** | Difficult (Netflix stack) | Native |
| **Resilience** | Local cache | Depends on CoreDNS/kube-proxy |
| **Discovery latency** | 30-60s (heartbeat) | Near-instant |
| **Complexity** | Medium (registry) | High (entire cluster) |
| **Ideal use case** | Spring Cloud micro-services | Cloud-native multi-language applications |

### Hybrid Approach
Many organizations use **both**:
- **Kubernetes** for basic service discovery
- **Service Mesh** (Istio, Linkerd) for advanced features: circuit breaking, retry, mTLS, observability

```yaml
# Example with Istio
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

## Interview Tip

**Likely question**: "Why migrate from Eureka to Kubernetes?"

**Strong answer**:
- Eureka was suitable for our 100% Spring Cloud stack
- With the adoption of polyglot microservices (Go, Python), client-side discovery becomes limiting
- Kubernetes offers a unified platform: discovery + orchestration + scaling + deployments
- Reduces application complexity by delegating to the infrastructure
- But we keep Eureka for legacy services during the transition phase

# Q&A - Service Discovery (Oral Interview)

## Fundamental Questions

### Q1: Can you explain what service discovery is and why it matters?

**Answer:**
Service discovery is a mechanism that allows microservices to dynamically locate each other in a distributed environment.

In a monolithic architecture, we have fixed URLs, but with microservices, instances can start/stop at any time, change IP, and scale horizontally. Service discovery solves this problem by maintaining a dynamic registry.

It matters because it enables:
- **Auto-scaling**: new instances automatically discovered
- **Resilience**: automatic removal of failing instances
- **Load balancing**: distribution of traffic between instances
- **Simplicity**: call by logical name instead of hardcoded IP:PORT

---

### Q2: What is the difference between client-side and server-side discovery?

**Answer:**
**Client-side discovery** (like Eureka):
- The client queries the registry to get the list of instances
- The client does the load balancing itself
- Example: My Order service calls Eureka, gets 3 IPs for User-Service, picks one IP

**Server-side discovery** (like Kubernetes):
- The client calls a load balancer or router
- The infrastructure does the routing
- Example: My Order service calls `http://user-service`, the Kubernetes Service routes automatically

**Analogy:**
- Client-side = You look up the phone directory yourself then call
- Server-side = You call a switchboard that automatically transfers you

---

### Q3: How does Eureka work in practice?

**Answer:**
Eureka works in 3 steps:

**1. Registration:**
```
On startup → Service sends POST /eureka/apps/{APP_NAME}
Data: IP, port, metadata, health check URL
```

**2. Heartbeat:**
```
Every 30s → Service sends PUT /eureka/apps/{APP_NAME}/{INSTANCE_ID}
If no heartbeat for 90s → eviction from the registry
```

**3. Fetch Registry:**
```
Client fetches the registry (every 30s by default)
Local cache → If Eureka is down, continues with the cache
```

**Self-Preservation Mode:** If 15% of services miss their heartbeat, Eureka assumes a network problem and stops evicting to avoid a cascade of removals.

---

### Q4: What is a Service in Kubernetes?

**Answer:**
A Kubernetes Service is an abstraction that exposes a set of Pods under a DNS name and a stable IP.

**Main types:**

**ClusterIP** (default):
- Virtual IP internal to the cluster
- Only accessible from inside the cluster
- Use case: inter-service communication

**NodePort**:
- Exposed on a port on each node (30000-32767)
- Accessible from outside via NodeIP:NodePort
- Use case: development, testing

**LoadBalancer**:
- Creates a cloud load balancer (AWS ELB, GCP LB)
- External public IP
- Use case: production exposure

**Concrete example:**
```yaml
# Service automatically creates endpoints to Pods
# with the label app: order-service
# DNS: order-service.default.svc.cluster.local
# Or simply: order-service
```

---

## Advanced Technical Questions

### Q5: How do you handle the "cache staleness" problem with Eureka?

**Answer:**
Cache staleness is the delay between a change (new service, instance down) and its propagation.

**In Eureka, multiple cache layers:**
1. **Server cache**: Eureka caches the registry (30s by default)
2. **Client cache**: Client fetches every 30s
3. **Ribbon cache**: Ribbon caches available instances

**Total worst case:** 90 seconds of latency

**Solutions:**
```yaml
# Reduce intervals (watch out for load)
eureka:
  instance:
    lease-renewal-interval-in-seconds: 10  # heartbeat
    lease-expiration-duration-in-seconds: 30  # eviction
  client:
    registry-fetch-interval-seconds: 10  # fetch
```

**Alternative:** Use Spring Cloud Consul which supports long-polling and reduces latency.

**In production:** We generally accept the delay as it protects against "flapping services".

---

### Q6: How does Kubernetes handle load balancing?

**Answer:**
Kubernetes uses **kube-proxy** running on each node.

**3 modes:**

**1. iptables (default):**
```
Client → iptables rules → random Pod selection
Advantage: Performance
Disadvantage: No retry if Pod is down
```

**2. IPVS:**
```
Client → IPVS load balancer → Pod (round-robin, least connection...)
Advantage: Advanced algorithms, better performance
```

**3. userspace (legacy):**
```
Client → kube-proxy (userspace) → Pod
Slow, deprecated
```

**Concrete example:**
```bash
# kube-proxy creates iptables rules like:
-A KUBE-SERVICES -d 10.96.0.1/32 -p tcp -m tcp --dport 8080 \
  -j KUBE-SVC-XXXX

# Which redirects to one of the backend Pods
-A KUBE-SVC-XXXX -m statistic --mode random --probability 0.33 \
  -j KUBE-SEP-POD1
```

---

### Q7: What is an Ingress and why use it?

**Answer:**
An Ingress is a Kubernetes object that manages external HTTP(S) access to Services.

**Why not just use a LoadBalancer Service?**
- One LoadBalancer = 1 public IP = cost
- With 10 services = 10 LoadBalancers = 10x the cost

**Ingress = 1 entry point for N services**

**Features:**
- **Path-based routing**: /api/orders → order-service, /api/users → user-service
- **Host-based routing**: api.company.com, admin.company.com
- **TLS termination**: Centralized SSL certificate management
- **Rewrites, redirects**

**Real example:**
```yaml
# 1 single public LoadBalancer
# Routes based on path
/orders/* → order-service:8080
/users/* → user-service:8080
/payments/* → payment-service:8080

# Automatic TLS with cert-manager
annotations:
  cert-manager.io/cluster-issuer: "letsencrypt-prod"
```

**Note:** The Ingress is just a definition. You need an **Ingress Controller** (Nginx, Traefik, HAProxy) to implement it.

---

### Q8: How do you ensure high availability with Eureka?

**Answer:**
Several strategies:

**1. Eureka Server Cluster:**
```yaml
# Eureka Server 1
eureka:
  client:
    service-url:
      defaultZone: http://eureka-2:8761/eureka/,http://eureka-3:8761/eureka/

# Servers replicate with each other (peer-to-peer)
```

**2. Client resilience:**
```yaml
eureka:
  client:
    # Multiple fallback URLs
    service-url:
      defaultZone: http://eureka-1:8761/eureka/,http://eureka-2:8761/eureka/
    
    # Client keeps cache if all servers are down
    fetch-registry: true
```

**3. Circuit Breaker Pattern:**
```java
@HystrixCommand(fallbackMethod = "fallbackUser")
public User getUser(Long id) {
    return restTemplate.getForObject(
        "http://USER-SERVICE/users/" + id, 
        User.class
    );
}

public User fallbackUser(Long id) {
    return new User(id, "Unknown"); // Graceful degradation
}
```

**4. Multiple Health Checks:**
- Eureka heartbeat
- Spring Boot Actuator `/health`
- External monitoring (Prometheus, Datadog)

---

### Q9: What are the health checks in Kubernetes?

**Answer:**
Kubernetes has 3 types of probes:

**1. Liveness Probe:**
- "Is the container alive?"
- If it fails → Kubernetes **restarts** the container
- Use case: deadlock, memory leak

```yaml
livenessProbe:
  httpGet:
    path: /actuator/health/liveness
    port: 8080
  initialDelaySeconds: 60  # Wait for startup
  periodSeconds: 10
  failureThreshold: 3  # 3 failures before restart
```

**2. Readiness Probe:**
- "Is the container ready to receive traffic?"
- If it fails → Kubernetes **removes it from the Service** (no restart)
- Use case: warm-up, dependencies not ready

```yaml
readinessProbe:
  httpGet:
    path: /actuator/health/readiness
    port: 8080
  initialDelaySeconds: 30
  periodSeconds: 5
```

**3. Startup Probe:**
- "Has the container finished starting up?"
- For apps with long startup time (> 1 minute)
- Disables liveness during startup

**Classic mistake:**
Don't confuse liveness and readiness! If liveness fails too easily, you can end up in an infinite restart loop.

**Spring Boot best practice:**
```yaml
management:
  endpoint:
    health:
      probes:
        enabled: true  # Enables /health/liveness and /health/readiness
  health:
    readinessState:
      enabled: true
    livenessState:
      enabled: true
```

---

## Design/Architecture Questions

### Q10: When should you choose Eureka vs Kubernetes?

**Answer:**

**Choose Eureka if:**
- ✅ 100% Spring Cloud stack (synergy with Config Server, Gateway, etc.)
- ✅ No Kubernetes infrastructure available
- ✅ Need for advanced client-side load balancing (weighted, zone-aware)
- ✅ Team comfortable with Netflix stack
- ✅ Medium-sized project (< 50 services)

**Choose Kubernetes if:**
- ✅ Polyglot architecture (Java, Go, Python, Node.js...)
- ✅ Need for full orchestration (scaling, rolling updates, resource management)
- ✅ Cloud-native infrastructure
- ✅ Large scale (> 50 services)
- ✅ Need for advanced features (service mesh, CRDs, operators)

**My personal opinion:**
For a new project in 2025, I recommend **Kubernetes** because:
- Industry standard
- Avoids vendor lock-in
- Mature ecosystem (Helm, Operators, CNCF tools)
- Infra/SRE teams already trained

But if you already have legacy Spring Cloud with Eureka, no need to migrate right away. Incremental approach.

---

### Q11: How would you migrate from Eureka to Kubernetes?

**Answer:**

**Phase 1: Dual Stack (3-6 months)**
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

**Phase 2: Bridge Pattern**
```java
// Service supports both modes
@Configuration
public class DiscoveryConfig {
    
    @Bean
    @ConditionalOnProperty(name = "discovery.mode", havingValue = "eureka")
    public RestTemplate eurekaRestTemplate() {
        return new RestTemplate(); // with @LoadBalanced
    }
    
    @Bean
    @ConditionalOnProperty(name = "discovery.mode", havingValue = "kubernetes")
    public RestTemplate k8sRestTemplate() {
        return new RestTemplate(); // standard, K8s DNS
    }
}
```

**Phase 3: Progressive migration**
1. New services → K8s only
2. Low-coupling services → K8s
3. Critical services → last (with rollback plan)

**Phase 4: Decommission Eureka**
- Monitoring to validate zero traffic
- Shut down Eureka servers

**Risks to manage:**
- Cross-cluster latency
- Distributed transaction management
- Team training

**Realistic timeline:** 12-18 months for a large app

---

### Q12: What is a Service Mesh and how does it compare to Eureka?

**Answer:**

A Service Mesh (Istio, Linkerd) is an infrastructure layer dedicated to inter-service communication.

**Architecture:**
```
Service A → Sidecar Proxy (Envoy) → Sidecar Proxy → Service B
              ↓                           ↓
         Control Plane (Istiod)
```

**Comparison:**

| Feature | Eureka | Kubernetes | Service Mesh |
|---------|--------|------------|--------------|
| Discovery | ✅ | ✅ | ✅ |
| Load Balancing | ✅ Client-side | ✅ Server-side | ✅ Proxy-side |
| Circuit Breaking | ❌ (Hystrix separate) | ❌ | ✅ |
| Retry Logic | ❌ (in code) | ❌ | ✅ |
| Mutual TLS | ❌ | ❌ (manual) | ✅ Automatic |
| Traffic Shifting | ❌ | ⚠️ Limited | ✅ Canary, A/B |
| Observability | ⚠️ Basic | ⚠️ Basic | ✅ Metrics, Traces |

**Istio example - Canary Deployment:**
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

**When to use a Service Mesh?**
- 50+ microservices
- Need for automatic mTLS
- Advanced observability required
- Dedicated SRE team

**Cost:** Complexity + latency (proxy overhead ~1-5ms) + resources (1 proxy per pod)

---

### Q13: How do you manage configuration in these two approaches?

**Answer:**

**With Eureka (Spring Cloud Config):**
```yaml
# Centralized Config Server
spring:
  cloud:
    config:
      server:
        git:
          uri: https://github.com/company/config-repo
          search-paths: '{application}'
```

```
# Git structure
config-repo/
├── order-service.yml         # Config per service
├── order-service-dev.yml     # Config per environment
├── order-service-prod.yml
└── application.yml           # Shared config
```

```java
// Client auto-refresh with @RefreshScope
@RestController
@RefreshScope
public class OrderController {
    
    @Value("${tax.rate}")
    private Double taxRate;  // Refresh via POST /actuator/refresh
}
```

**With Kubernetes:**

**ConfigMap (non-sensitive):**
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

**Secret (sensitive):**
```yaml
apiVersion: v1
kind: Secret
metadata:
  name: order-service-secrets
type: Opaque
data:
  db-password: cGFzc3dvcmQxMjM=  # base64
```

**Mounting in the Pod:**
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

**Refresh without restart:**
```
Option 1: External Config (Spring Cloud Kubernetes Config)
Option 2: ConfigMap reload with Reloader operator
Option 3: Automatic rolling restart
```

**Best practice:** Sensitive secrets → **External Secrets Operator** that syncs from Vault, AWS Secrets Manager, etc.

---

### Q14: Tell me about the "thundering herd" problem with Eureka

**Answer:**

The **thundering herd** (stampede) happens when all clients perform the same action simultaneously.

**Scenario with Eureka:**
```
1. Eureka Server restarts
2. All clients (100 services × 10 instances = 1000 clients)
   try to reconnect AT THE SAME TIME
3. Eureka is overwhelmed → crashes → restarts → loop
```

**Another case: Service restarts**
```
1. USER-SERVICE restarts (10 instances)
2. All 10 send a heartbeat simultaneously
3. All 10 instances appear "available" at the same time
4. ORDER-SERVICE sends a burst of requests
5. Instances are not yet "warm" → timeouts
```

**Solutions:**

**1. Jitter (random offset):**
```java
// Instead of heartbeat every exactly 30s
int interval = 30 + random.nextInt(10); // 30-40s
```

**2. Graceful startup:**
```yaml
eureka:
  instance:
    initial-status: STARTING  # Not "UP" immediately
    
management:
  endpoint:
    health:
      readiness:
        enabled: true  # Waits until ready before receiving traffic
```

**3. Circuit Breaker:**
```java
@CircuitBreaker(name = "userService", fallbackMethod = "fallback")
public User getUser(Long id) {
    // If too many errors, circuit opens automatically
}
```

**4. Rate Limiting:**
```yaml
# Limit the number of reconnections
eureka:
  client:
    registry-fetch-interval-seconds: 30
    initial-instance-info-replication-interval-seconds: 40
```

**Kubernetes handles this better:** Readiness probes prevent traffic before the Pod is ready, and the rollout is progressive (maxSurge: 1, maxUnavailable: 0).

---

### Q15: Give me an example of debugging a service discovery problem

**Answer:**

**Real scenario:** Order-Service cannot find User-Service

**Step 1: Check the registry (Eureka)**
```bash
# Call the Eureka API
curl http://eureka-server:8761/eureka/apps

# Check if USER-SERVICE is present
# Check the status: UP or DOWN?
# Check the IP/port
```

**Step 2: Check client logs**
```
# Search in the logs
ERROR - DiscoveryClient - Cannot fetch registry from server

# Possible causes:
- Firewall blocking port 8761
- Wrong URL in eureka.client.service-url.defaultZone
- Eureka server down
```

**Step 3: Test connectivity**
```bash
# From the Order-Service Pod
kubectl exec -it order-service-pod -- sh

# Ping the service (if possible)
ping user-service

# Curl directly
curl http://USER-SERVICE-IP:8080/actuator/health
```

**Step 4: Check the client cache**
```java
// Expose a debug endpoint
@GetMapping("/debug/registry")
public List<ServiceInstance> getRegistry() {
    return discoveryClient.getInstances("USER-SERVICE");
}
```

**Kubernetes:**
```bash
# Check the Service
kubectl get svc user-service
kubectl describe svc user-service

# Check the Endpoints (Pods behind the Service)
kubectl get endpoints user-service

# If empty → selector problem
# Check Pod labels
kubectl get pods -l app=user-service --show-labels

# Test DNS
kubectl exec -it order-service-pod -- nslookup user-service

# Check kube-proxy logs
kubectl logs -n kube-system -l k8s-app=kube-proxy
```

**Classic bug:**
```yaml
# Service selector
selector:
  app: user-service  # ⚠️ Watch out for case sensitivity

# Pod labels
labels:
  app: User-Service  # ❌ No match! (capital U)
```

**Fix:** Standardize the labels

**Pro tool:** Use **ksniff** (Wireshark for K8s) to capture network traffic

---
