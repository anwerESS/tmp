## [**..**](./00_index.md)


# Chapitre 4 : Networking

## 4.1. Services (ClusterIP, NodePort, LoadBalancer)

### Qu'est-ce qu'un Service ?

Un **Service** est une abstraction qui définit un ensemble logique de Pods et une politique d'accès. Il fournit :
- Une IP stable (les Pods peuvent mourir/redémarrer)
- Un DNS stable
- Load balancing entre les Pods

```
Service (IP stable : 10.96.100.50)
    │
    ├─→ Pod-1 (IP: 10.244.1.10)
    ├─→ Pod-2 (IP: 10.244.1.11)
    └─→ Pod-3 (IP: 10.244.1.12)
```

---

### Type 1 : ClusterIP (défaut)

Expose le Service sur une **IP interne** au cluster uniquement.

```yaml
apiVersion: v1
kind: Service
metadata:
  name: spring-app
  labels:
    app: spring-app
spec:
  type: ClusterIP  # Type par défaut
  selector:
    app: spring-app  # Sélectionne les Pods avec ce label
  ports:
  - name: http
    port: 80        # Port du Service
    targetPort: 8080  # Port du container
    protocol: TCP
  
  # Optional
  sessionAffinity: ClientIP  # Sticky sessions
```

**Caractéristiques :**
- IP accessible uniquement DANS le cluster
- DNS : `spring-app.default.svc.cluster.local`
- Use case : Communication inter-services

**Exemple d'utilisation :**
```yaml
# Frontend Pod accède au backend
apiVersion: v1
kind: Pod
metadata:
  name: frontend
spec:
  containers:
  - name: app
    image: frontend:1.0
    env:
    - name: BACKEND_URL
      value: "http://backend-service:80"  # DNS du Service
```

```bash
# Créer le Service
kubectl apply -f service.yaml

# Vérifier
kubectl get service spring-app
# NAME         TYPE        CLUSTER-IP      EXTERNAL-IP   PORT(S)   AGE
# spring-app   ClusterIP   10.96.100.50    <none>        80/TCP    5m

# Tester depuis un Pod
kubectl run test --rm -it --image=curlimages/curl -- sh
curl http://spring-app:80
```

**Load balancing :**
```bash
# Le Service load balance automatiquement
curl http://spring-app:80
# → Pod-1

curl http://spring-app:80
# → Pod-2

curl http://spring-app:80
# → Pod-3
```

---

### Type 2 : NodePort

Expose le Service sur un **port statique** sur chaque Node.

```yaml
apiVersion: v1
kind: Service
metadata:
  name: spring-app-nodeport
spec:
  type: NodePort
  selector:
    app: spring-app
  ports:
  - port: 80          # Port du Service (ClusterIP)
    targetPort: 8080  # Port du container
    nodePort: 30080   # Port sur chaque Node (30000-32767)
    protocol: TCP
```

**Caractéristiques :**
- Accessible via `<NodeIP>:30080` depuis l'extérieur
- Crée aussi un ClusterIP automatiquement
- Range de ports : 30000-32767

**Accès :**
```bash
# Depuis l'extérieur du cluster
curl http://<node-ip>:30080

# Depuis l'intérieur (ClusterIP créé automatiquement)
curl http://spring-app-nodeport:80
```

```bash
kubectl get service spring-app-nodeport
# NAME                   TYPE       CLUSTER-IP      EXTERNAL-IP   PORT(S)        AGE
# spring-app-nodeport    NodePort   10.96.100.51    <none>        80:30080/TCP   5m

# Obtenir l'IP d'un Node
kubectl get nodes -o wide
# NAME       STATUS   ROLES    EXTERNAL-IP
# worker-1   Ready    <none>   192.168.1.100

# Accès externe
curl http://192.168.1.100:30080
```

**Use case :**
- Accès externe simple (dev/test)
- Pas de LoadBalancer disponible
- Éviter en production (ports non standard)

---

### Type 3 : LoadBalancer

Crée un **load balancer externe** (cloud provider).

```yaml
apiVersion: v1
kind: Service
metadata:
  name: spring-app-lb
  annotations:
    service.beta.kubernetes.io/aws-load-balancer-type: "nlb"  # AWS NLB
spec:
  type: LoadBalancer
  selector:
    app: spring-app
  ports:
  - port: 80
    targetPort: 8080
    protocol: TCP
  
  # Optional
  loadBalancerSourceRanges:  # Whitelist IPs
  - 203.0.113.0/24
```

**Comportement :**
1. Crée un ClusterIP
2. Crée un NodePort
3. Provisionne un load balancer externe (AWS ELB, GCP LB, Azure LB)
4. Configure le LB pour pointer vers les NodePorts

```bash
kubectl get service spring-app-lb
# NAME            TYPE           CLUSTER-IP      EXTERNAL-IP                                PORT(S)        AGE
# spring-app-lb   LoadBalancer   10.96.100.52    a1b2c3.us-east-1.elb.amazonaws.com         80:31234/TCP   5m

# Accès via le LoadBalancer
curl http://a1b2c3.us-east-1.elb.amazonaws.com
```

**Use case :**
- Production avec cloud provider
- Haute disponibilité
- Gestion automatique du trafic externe

**Limitations :**
- Coût : 1 LB = 1 Service = $$$
- Solution : Utiliser Ingress (1 LB pour N services)

---

### Type 4 : ExternalName

Crée un **alias DNS** vers un service externe.

```yaml
apiVersion: v1
kind: Service
metadata:
  name: external-db
spec:
  type: ExternalName
  externalName: database.external.com  # FQDN externe
```

**Use case :**
```yaml
# Application dans K8s accède à une DB externe
env:
- name: DATABASE_URL
  value: "jdbc:postgresql://external-db:5432/mydb"
# Résolu en: database.external.com
```

**Avantages :**
- Abstraction : change `externalName` sans modifier les apps
- Migration progressive vers K8s

---

### Headless Service (ClusterIP: None)

Service sans IP, retourne directement les IPs des Pods.

```yaml
apiVersion: v1
kind: Service
metadata:
  name: postgres-headless
spec:
  clusterIP: None  # ← Headless
  selector:
    app: postgres
  ports:
  - port: 5432
```

**DNS résolution :**
```bash
# Service normal
nslookup spring-app.default.svc.cluster.local
# → 10.96.100.50 (ClusterIP)

# Headless Service
nslookup postgres-headless.default.svc.cluster.local
# → 10.244.1.10 (Pod-1)
# → 10.244.1.11 (Pod-2)
# → 10.244.1.12 (Pod-3)
```

**Use case :**
- StatefulSets (chaque Pod a son DNS stable)
- Client-side load balancing
- Peer discovery (Elasticsearch, Kafka)

```bash
# Accès à un Pod spécifique d'un StatefulSet
postgres-0.postgres-headless.default.svc.cluster.local
postgres-1.postgres-headless.default.svc.cluster.local
```

---

### Endpoints

Les **Endpoints** sont créés automatiquement par le Service pour mapper les Pods.

```bash
kubectl get endpoints spring-app
# NAME         ENDPOINTS                                      AGE
# spring-app   10.244.1.10:8080,10.244.1.11:8080,10.244.1.12:8080   5m

kubectl describe endpoints spring-app
# Subsets:
#   Addresses:  10.244.1.10,10.244.1.11,10.244.1.12
#   Ports:      8080/TCP
```

**Endpoints vides (pas de Pods) :**
```bash
kubectl get endpoints spring-app
# NAME         ENDPOINTS   AGE
# spring-app   <none>      5m

# Causes possibles :
# - Selector ne match aucun Pod
# - Aucun Pod Ready
# - Labels incorrects
```

---

### Session Affinity (Sticky Sessions)

Par défaut, le Service load balance **round-robin**. Pour sticky sessions :

```yaml
spec:
  sessionAffinity: ClientIP
  sessionAffinityConfig:
    clientIP:
      timeoutSeconds: 10800  # 3 heures
```

**Comportement :**
```bash
# Même client → toujours le même Pod
curl http://spring-app:80  # → Pod-1
curl http://spring-app:80  # → Pod-1
curl http://spring-app:80  # → Pod-1
```

---

### Exemple Complet : Application Multi-tiers

```yaml
# Backend Service (ClusterIP - interne)
apiVersion: v1
kind: Service
metadata:
  name: backend
spec:
  type: ClusterIP
  selector:
    app: backend
    tier: api
  ports:
  - port: 8080
    targetPort: 8080
---
# Backend Deployment
apiVersion: apps/v1
kind: Deployment
metadata:
  name: backend
spec:
  replicas: 3
  selector:
    matchLabels:
      app: backend
      tier: api
  template:
    metadata:
      labels:
        app: backend
        tier: api
    spec:
      containers:
      - name: api
        image: myapp/backend:1.0
        ports:
        - containerPort: 8080
---
# Frontend Service (LoadBalancer - externe)
apiVersion: v1
kind: Service
metadata:
  name: frontend
spec:
  type: LoadBalancer
  selector:
    app: frontend
    tier: web
  ports:
  - port: 80
    targetPort: 3000
---
# Frontend Deployment
apiVersion: apps/v1
kind: Deployment
metadata:
  name: frontend
spec:
  replicas: 2
  selector:
    matchLabels:
      app: frontend
      tier: web
  template:
    metadata:
      labels:
        app: frontend
        tier: web
    spec:
      containers:
      - name: web
        image: myapp/frontend:1.0
        ports:
        - containerPort: 3000
        env:
        - name: BACKEND_URL
          value: "http://backend:8080"  # DNS interne
---
# Database Headless Service
apiVersion: v1
kind: Service
metadata:
  name: postgres
spec:
  clusterIP: None  # Headless
  selector:
    app: postgres
  ports:
  - port: 5432
---
# Database StatefulSet
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: postgres
spec:
  serviceName: postgres
  replicas: 3
  selector:
    matchLabels:
      app: postgres
  template:
    metadata:
      labels:
        app: postgres
    spec:
      containers:
      - name: postgres
        image: postgres:14
        ports:
        - containerPort: 5432
```

**Flow de trafic :**
```
Internet
  ↓
LoadBalancer (frontend)
  ↓
Frontend Pods :3000
  ↓ (HTTP request to backend:8080)
Backend Service (ClusterIP)
  ↓ (Load balance)
Backend Pods :8080
  ↓ (SQL query to postgres-0.postgres)
Postgres Headless Service
  ↓
Postgres-0 Pod :5432
```

---

## 4.2. Ingress & Ingress Controllers

### Qu'est-ce qu'un Ingress ?

Un **Ingress** expose des routes HTTP/HTTPS vers des Services, avec :
- **Routing basé sur l'host** (virtual hosting)
- **Routing basé sur le path**
- **TLS termination**
- **Load balancing L7** (application layer)

**Avantages vs LoadBalancer Services :**
- 1 Ingress = 1 LoadBalancer pour N services
- Routing intelligent (host, path, headers)
- TLS centralisé

```
                    Ingress
                      │
        ┌─────────────┼─────────────┐
        │             │             │
    Service A     Service B     Service C
```

---

### Ingress Controller

Un **Ingress Controller** est requis pour que l'Ingress fonctionne. Options :

1. **Nginx Ingress Controller** (le plus populaire)
2. **Traefik**
3. **HAProxy**
4. **Kong**
5. **AWS ALB Ingress Controller**
6. **GCE Ingress Controller**

**Installation Nginx Ingress :**
```bash
kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/controller-v1.8.1/deploy/static/provider/cloud/deploy.yaml

# Vérifier
kubectl get pods -n ingress-nginx
kubectl get service -n ingress-nginx
```

---

### Ingress Simple

```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: simple-ingress
  annotations:
    nginx.ingress.kubernetes.io/rewrite-target: /
spec:
  ingressClassName: nginx  # Controller à utiliser
  rules:
  - host: myapp.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: spring-app
            port:
              number: 80
```

```bash
kubectl apply -f ingress.yaml

kubectl get ingress
# NAME              CLASS   HOSTS                ADDRESS         PORTS   AGE
# simple-ingress    nginx   myapp.example.com    34.123.45.67    80      5m

# Tester (ajouter l'entrée DNS ou /etc/hosts)
curl http://myapp.example.com
```

---

### Ingress avec TLS (HTTPS)

```yaml
apiVersion: v1
kind: Secret
metadata:
  name: tls-secret
type: kubernetes.io/tls
data:
  tls.crt: <base64-cert>
  tls.key: <base64-key>
---
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: tls-ingress
spec:
  ingressClassName: nginx
  tls:
  - hosts:
    - myapp.example.com
    secretName: tls-secret  # Certificat TLS
  rules:
  - host: myapp.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: spring-app
            port:
              number: 80
```

**Comportement :**
- HTTP (port 80) → Redirect vers HTTPS
- HTTPS (port 443) → TLS termination → Service

```bash
curl https://myapp.example.com
# TLS terminé par l'Ingress Controller
```

---

### Ingress avec Multiple Hosts

```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: multi-host-ingress
spec:
  ingressClassName: nginx
  rules:
  # Host 1 : API
  - host: api.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: api-service
            port:
              number: 8080
  
  # Host 2 : Admin
  - host: admin.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: admin-service
            port:
              number: 80
  
  # Host 3 : Web
  - host: www.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: web-service
            port:
              number: 3000
```

**Routing :**
```
http://api.example.com   → api-service:8080
http://admin.example.com → admin-service:80
http://www.example.com   → web-service:3000
```

---

### Ingress avec Path-based Routing

```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: path-based-ingress
  annotations:
    nginx.ingress.kubernetes.io/rewrite-target: /$2
spec:
  ingressClassName: nginx
  rules:
  - host: myapp.example.com
    http:
      paths:
      # /api/* → api-service
      - path: /api(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: api-service
            port:
              number: 8080
      
      # /admin/* → admin-service
      - path: /admin(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: admin-service
            port:
              number: 80
      
      # /* → web-service
      - path: /
        pathType: Prefix
        backend:
          service:
            name: web-service
            port:
              number: 3000
```

**Routing :**
```
http://myapp.example.com/           → web-service:3000/
http://myapp.example.com/api/users  → api-service:8080/users
http://myapp.example.com/admin/dash → admin-service:80/dash
```

---

### PathType Options

**1. Prefix** (le plus courant)
```yaml
path: /api
pathType: Prefix
# Matche : /api, /api/, /api/users, /api/v1/users
```

**2. Exact**
```yaml
path: /api
pathType: Exact
# Matche SEULEMENT : /api
# Ne matche PAS : /api/, /api/users
```

**3. ImplementationSpecific**
```yaml
path: /api(/|$)(.*)
pathType: ImplementationSpecific
# Comportement dépend de l'Ingress Controller (regex nginx)
```

---

### Annotations Nginx Ingress

```yaml
metadata:
  annotations:
    # Rewrite
    nginx.ingress.kubernetes.io/rewrite-target: /
    
    # CORS
    nginx.ingress.kubernetes.io/enable-cors: "true"
    nginx.ingress.kubernetes.io/cors-allow-origin: "https://frontend.example.com"
    
    # Rate limiting
    nginx.ingress.kubernetes.io/limit-rps: "10"
    
    # Timeouts
    nginx.ingress.kubernetes.io/proxy-connect-timeout: "60"
    nginx.ingress.kubernetes.io/proxy-read-timeout: "60"
    
    # Client body size (uploads)
    nginx.ingress.kubernetes.io/proxy-body-size: "100m"
    
    # Basic Auth
    nginx.ingress.kubernetes.io/auth-type: basic
    nginx.ingress.kubernetes.io/auth-secret: basic-auth
    nginx.ingress.kubernetes.io/auth-realm: "Authentication Required"
    
    # Redirect HTTP → HTTPS
    nginx.ingress.kubernetes.io/force-ssl-redirect: "true"
    
    # Whitelist IPs
    nginx.ingress.kubernetes.io/whitelist-source-range: "10.0.0.0/8,172.16.0.0/12"
    
    # Custom headers
    nginx.ingress.kubernetes.io/configuration-snippet: |
      more_set_headers "X-Custom-Header: value";
```

---

### Ingress avec Cert-Manager (Let's Encrypt)

**1. Installer Cert-Manager :**
```bash
kubectl apply -f https://github.com/cert-manager/cert-manager/releases/download/v1.13.0/cert-manager.yaml
```

**2. Créer un ClusterIssuer :**
```yaml
apiVersion: cert-manager.io/v1
kind: ClusterIssuer
metadata:
  name: letsencrypt-prod
spec:
  acme:
    server: https://acme-v02.api.letsencrypt.org/directory
    email: admin@example.com
    privateKeySecretRef:
      name: letsencrypt-prod
    solvers:
    - http01:
        ingress:
          class: nginx
```

**3. Ingress avec auto-TLS :**
```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: auto-tls-ingress
  annotations:
    cert-manager.io/cluster-issuer: "letsencrypt-prod"
spec:
  ingressClassName: nginx
  tls:
  - hosts:
    - myapp.example.com
    secretName: myapp-tls  # Créé automatiquement par cert-manager
  rules:
  - host: myapp.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: spring-app
            port:
              number: 80
```

**Cert-Manager :**
1. Détecte l'annotation
2. Crée un Certificate resource
3. Fait la validation ACME (HTTP-01 challenge)
4. Obtient le certificat de Let's Encrypt
5. Crée le Secret `myapp-tls`
6. Renouvelle automatiquement avant expiration

---

### Ingress Canary Deployment

```yaml
# Stable version (90% traffic)
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: spring-app-stable
spec:
  ingressClassName: nginx
  rules:
  - host: myapp.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: spring-app-stable
            port:
              number: 80
---
# Canary version (10% traffic)
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: spring-app-canary
  annotations:
    nginx.ingress.kubernetes.io/canary: "true"
    nginx.ingress.kubernetes.io/canary-weight: "10"  # 10% du trafic
spec:
  ingressClassName: nginx
  rules:
  - host: myapp.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: spring-app-canary
            port:
              number: 80
```

**Canary basé sur header :**
```yaml
annotations:
  nginx.ingress.kubernetes.io/canary: "true"
  nginx.ingress.kubernetes.io/canary-by-header: "X-Canary"
  nginx.ingress.kubernetes.io/canary-by-header-value: "true"

# Request avec header X-Canary: true → canary
# Request sans header → stable
```

---

## 4.3. Network Policies

### Qu'est-ce qu'une NetworkPolicy ?

Une **NetworkPolicy** définit comment les Pods peuvent communiquer entre eux et avec d'autres endpoints réseau.

**Par défaut** : Tous les Pods peuvent communiquer avec tous les Pods (no isolation).

**Avec NetworkPolicies** : Deny by default + allow explicites.

---

### Network Policy : Deny All Ingress

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: deny-all-ingress
  namespace: default
spec:
  podSelector: {}  # Tous les Pods du namespace
  policyTypes:
  - Ingress
  # Pas de règles ingress → deny all
```

**Résultat :** Aucun Pod ne peut recevoir de trafic entrant.

---

### Network Policy : Allow Specific Ingress

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-frontend-to-backend
  namespace: default
spec:
  podSelector:
    matchLabels:
      app: backend  # Appliqué aux Pods backend
  policyTypes:
  - Ingress
  ingress:
  - from:
    - podSelector:
        matchLabels:
          app: frontend  # Autorise depuis frontend
    ports:
    - protocol: TCP
      port: 8080
```

**Résultat :**
- ✅ Frontend → Backend :8080 : Autorisé
- ❌ Autre Pod → Backend : Bloqué

---

### Network Policy : Allow from Namespace

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-from-dev-namespace
  namespace: prod
spec:
  podSelector:
    matchLabels:
      app: api
  policyTypes:
  - Ingress
  ingress:
  - from:
    - namespaceSelector:
        matchLabels:
          environment: dev  # Namespace avec ce label
    ports:
    - protocol: TCP
      port: 8080
```

---

### Network Policy : Egress (sortant)

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-egress-to-db
  namespace: default
spec:
  podSelector:
    matchLabels:
      app: backend
  policyTypes:
  - Egress
  egress:
  - to:
    - podSelector:
        matchLabels:
          app: postgres
    ports:
    - protocol: TCP
      port: 5432
  
  # Autoriser DNS
  - to:
    - namespaceSelector:
        matchLabels:
          name: kube-system
    ports:
    - protocol: UDP
      port: 53
```

**Résultat :**
- ✅ Backend → Postgres :5432 : Autorisé
- ✅ Backend → DNS :53 : Autorisé
- ❌ Backend → Internet : Bloqué

---

### Network Policy : Exemple Complet Multi-tiers

```yaml
# 1. Deny all ingress par défaut
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: default-deny-all
  namespace: prod
spec:
  podSelector: {}
  policyTypes:
  - Ingress
  - Egress
---
# 2. Frontend : Ingress depuis Internet (via Ingress Controller)
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-frontend-ingress
  namespace: prod
spec:
  podSelector:
    matchLabels:
      app: frontend
      tier: web
  policyTypes:
  - Ingress
  ingress:
  - from:
    - namespaceSelector:
        matchLabels:
          name: ingress-nginx  # Ingress Controller namespace
    ports:
    - protocol: TCP
      port: 3000
---
# 3. Frontend : Egress vers Backend
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: frontend-to-backend
  namespace: prod
spec:
  podSelector:
    matchLabels:
      app: frontend
  policyTypes:
  - Egress
  egress:
  - to:
    - podSelector:
        matchLabels:
          app: backend
    ports:
    - protocol: TCP
      port: 8080
  - to:  # DNS
    - namespaceSelector:
        matchLabels:
          name: kube-system
    ports:
    - protocol: UDP
      port: 53
---
# 4. Backend : Ingress depuis Frontend
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-backend-from-frontend
  namespace: prod
spec:
  podSelector:
    matchLabels:
      app: backend
      tier: api
  policyTypes:
  - Ingress
  ingress:
  - from:
    - podSelector:
        matchLabels:
          app: frontend
    ports:
    - protocol: TCP
      port: 8080
---
# 5. Backend : Egress vers Database
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: backend-to-db
  namespace: prod
spec:
  podSelector:
    matchLabels:
      app: backend
  policyTypes:
  - Egress
  egress:
  - to:
    - podSelector:
        matchLabels:
          app: postgres
    ports:
    - protocol: TCP
      port: 5432
  - to:  # DNS
    - namespaceSelector:
        matchLabels:
          name: kube-system
    ports:
    - protocol: UDP
      port: 53
---
# 6. Database : Ingress depuis Backend uniquement
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-db-from-backend
  namespace: prod
spec:
  podSelector:
    matchLabels:
      app: postgres
      tier: database
  policyTypes:
  - Ingress
  ingress:
  - from:
    - podSelector:
        matchLabels:
          app: backend
    ports:
    - protocol: TCP
      port: 5432
```

**Flow sécurisé :**
```
Internet
  ↓ (HTTPS)
Ingress Controller
  ↓ (autorisé par allow-frontend-ingress)
Frontend Pods :3000
  ↓ (autorisé par frontend-to-backend)
Backend Pods :8080
  ↓ (autorisé par backend-to-db)
Postgres Pods :5432
```

---

### Network Policy : CIDR Blocks

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-external-api
spec:
  podSelector:
    matchLabels:
      app: backend
  policyTypes:
  - Egress
  egress:
  - to:
    - ipBlock:
        cidr: 203.0.113.0/24  # API externe
        except:
        - 203.0.113.50/32  # Exclure une IP
    ports:
    - protocol: TCP
      port: 443
```

---

### Debugging Network Policies

```bash
# Tester la connectivité
kubectl run test-frontend --image=busybox --labels=app=frontend -it --rm -- sh
wget -O- http://backend:8080  # Test si autorisé

kubectl run test-other --image=busybox --labels=app=other -it --rm -- sh
wget -O- http://backend:8080  # Devrait être bloqué

# Vérifier les NetworkPolicies appliquées
kubectl get networkpolicies
kubectl describe networkpolicy allow-backend-from-frontend

# Logs (dépend du CNI plugin)
# Calico
kubectl logs -n kube-system -l k8s-app=calico-node

# Cilium
kubectl -n kube-system exec -it cilium-xxx -- cilium monitor
```

---

### CNI Requirements

⚠️ **Les NetworkPolicies nécessitent un CNI plugin compatible** :

✅ Compatible :
- Calico (le plus utilisé)
- Cilium
- Weave Net
- Antrea

❌ Non compatible :
- Flannel (seul)
- Canal = Flannel + Calico (compatible)

```bash
# Vérifier le CNI installé
kubectl get pods -n kube-system | grep -E "calico|cilium|weave"
```

---

## 4.4. DNS in Kubernetes

### CoreDNS

**CoreDNS** est le serveur DNS par défaut dans Kubernetes.

```bash
# Vérifier CoreDNS
kubectl get pods -n kube-system -l k8s-app=kube-dns
# NAME                       READY   STATUS    RESTARTS   AGE
# coredns-5d78c9869d-abc12   1/1     Running   0          10d

kubectl get service -n kube-system kube-dns
# NAME       TYPE        CLUSTER-IP   EXTERNAL-IP   PORT(S)         AGE
# kube-dns   ClusterIP   10.96.0.10   <none>        53/UDP,53/TCP   10d
```

---

### DNS pour Services

**Format DNS :**
```
<service-name>.<namespace>.svc.cluster.local
```

**Exemples :**
```bash
# Service "backend" dans namespace "default"
backend.default.svc.cluster.local

# Service "postgres" dans namespace "prod"
postgres.prod.svc.cluster.local
```

**Résolution simplifiée :**
```bash
# Même namespace
curl http://backend:8080  # Résolu automatiquement

# Autre namespace
curl http://backend.prod:8080  # Ajout du namespace suffit

# FQDN complet (toujours)
curl http://backend.prod.svc.cluster.local:8080
```

---

### DNS pour Pods

**Format DNS :**
```
<pod-ip-dashed>.<namespace>.pod.cluster.local
```

**Exemple :**
```bash
# Pod avec IP 10.244.1.5 dans namespace default
10-244-1-5.default.pod.cluster.local
```

**⚠️ Rarement utilisé** : Préférer les Services pour la découverte.

---

### DNS pour Headless Services

```yaml
apiVersion: v1
kind: Service
metadata:
  name: postgres
spec:
  clusterIP: None  # Headless
  selector:
    app: postgres
  ports:
  - port: 5432
```

**DNS par Pod (StatefulSet) :**
```
<pod-name>.<service-name>.<namespace>.svc.cluster.local
```

**Exemple :**
```bash
# StatefulSet "postgres" avec 3 replicas
postgres-0.postgres.default.svc.cluster.local
postgres-1.postgres.default.svc.cluster.local
postgres-2.postgres.default.svc.cluster.local
```

**Application :**
```java
// Connexion au master PostgreSQL
String masterHost = "postgres-0.postgres.prod.svc.cluster.local";

// Connexion aux replicas (read-only)
String replica1 = "postgres-1.postgres.prod.svc.cluster.local";
String replica2 = "postgres-2.postgres.prod.svc.cluster.local";
```

---

### Résolution DNS depuis un Pod

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: test-dns
spec:
  containers:
  - name: test
    image: busybox
    command: ["sleep", "3600"]
  
  # DNS configuration (optionnel)
  dnsPolicy: ClusterFirst  # Défaut
  dnsConfig:
    nameservers:
    - 8.8.8.8  # DNS externe additionnel
    searches:
    - default.svc.cluster.local
    - svc.cluster.local
    - cluster.local
    options:
    - name: ndots
      value: "5"
```

**dnsPolicy options :**

**1. ClusterFirst (défaut)**
```yaml
dnsPolicy: ClusterFirst
# DNS queries → CoreDNS → External DNS (si non trouvé)
```

**2. Default**
```yaml
dnsPolicy: Default
# Utilise le DNS du Node (/etc/resolv.conf)
```

**3. None**
```yaml
dnsPolicy: None
# Aucun DNS configuré automatiquement
# doit spécifier dnsConfig manuellement
```

**4. ClusterFirstWithHostNet**
```yaml
hostNetwork: true
dnsPolicy: ClusterFirstWithHostNet
# Pour Pods avec hostNetwork: true
```

---

### Tester la résolution DNS

```bash
# Créer un Pod de test
kubectl run test-dns --image=busybox:1.28 --rm -it -- sh

# Résoudre un Service
nslookup backend
# Server:    10.96.0.10  (CoreDNS)
# Address 1: 10.96.0.10 kube-dns.kube-system.svc.cluster.local
#
# Name:      backend
# Address 1: 10.96.100.50 backend.default.svc.cluster.local

# FQDN
nslookup backend.default.svc.cluster.local

# Service dans autre namespace
nslookup postgres.prod.svc.cluster.local

# Headless Service (retourne IPs des Pods)
nslookup postgres-headless
# Address 1: 10.244.1.10 postgres-0.postgres.default.svc.cluster.local
# Address 2: 10.244.1.11 postgres-1.postgres.default.svc.cluster.local
# Address 3: 10.244.1.12 postgres-2.postgres.default.svc.cluster.local
```

---

### DNS pour External Services

```yaml
apiVersion: v1
kind: Service
metadata:
  name: external-api
spec:
  type: ExternalName
  externalName: api.external.com
```

```bash
# Depuis un Pod
curl http://external-api
# Résolu en: api.external.com
```

---

### Personnaliser CoreDNS

```bash
# Configurer CoreDNS
kubectl edit configmap coredns -n kube-system
```

**Exemple : Ajouter un DNS externe pour un domaine spécifique**
```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: coredns
  namespace: kube-system
data:
  Corefile: |
    .:53 {
        errors
        health {
           lameduck 5s
        }
        ready
        kubernetes cluster.local in-addr.arpa ip6.arpa {
           pods insecure
           fallthrough in-addr.arpa ip6.arpa
           ttl 30
        }
        prometheus :9153
        forward . /etc/resolv.conf {
           max_concurrent 1000
        }
        cache 30
        loop
        reload
        loadbalance
    }
    
    # Custom : Forward *.company.com vers DNS interne
    company.com:53 {
        errors
        cache 30
        forward . 10.0.0.1 10.0.0.2
    }
```

---

### DNS Performance Tuning

**ndots configuration :**
```yaml
dnsConfig:
  options:
  - name: ndots
    value: "2"  # Défaut: 5
```

**Problème avec ndots=5 :**
```bash
# Query: backend
# CoreDNS essaie (avec ndots=5):
1. backend.default.svc.cluster.local.  ← 5 dots
2. backend.svc.cluster.local.          ← 4 dots
3. backend.cluster.local.              ← 3 dots
4. backend.                            ← Finalement résolu

# Total: 4 queries DNS !
```

**Solution : FQDN avec trailing dot**
```java
// ❌ Lent (4 queries)
String url = "http://backend:8080";

// ✅ Rapide (1 query)
String url = "http://backend.default.svc.cluster.local.:8080";
//                                                      ↑ trailing dot
```

---

## 4.5. Service Discovery

### Service Discovery via DNS

**Automatique pour tous les Services :**

```java
@Configuration
public class ServiceDiscoveryConfig {
    
    // DNS resolution automatique
    @Bean
    public RestTemplate restTemplate() {
        return new RestTemplate();
    }
    
    @Service
    public class BackendClient {
        
        @Autowired
        private RestTemplate restTemplate;
        
        public String callBackend() {
            // DNS: backend.default.svc.cluster.local
            return restTemplate.getForObject(
                "http://backend:8080/api/data", 
                String.class
            );
        }
    }
}
```

---

### Service Discovery via Environment Variables

Kubernetes injecte automatiquement des variables d'env pour chaque Service.

```bash
# Service "backend" dans namespace "default"
BACKEND_SERVICE_HOST=10.96.100.50
BACKEND_SERVICE_PORT=8080
BACKEND_PORT=tcp://10.96.100.50:8080
BACKEND_PORT_8080_TCP=tcp://10.96.100.50:8080
BACKEND_PORT_8080_TCP_PROTO=tcp
BACKEND_PORT_8080_TCP_PORT=8080
BACKEND_PORT_8080_TCP_ADDR=10.96.100.50
```

**⚠️ Limitation :** Les Services doivent exister AVANT le Pod pour que les variables soient injectées.

**Recommandation :** Utiliser DNS (plus flexible).

---

### Service Discovery via Kubernetes API

```java
@Service
public class K8sServiceDiscovery {
    
    private final KubernetesClient client = new DefaultKubernetesClient();
    
    public List<String> discoverBackendPods() {
        return client.pods()
            .inNamespace("default")
            .withLabel("app", "backend")
            .list()
            .getItems()
            .stream()
            .map(pod -> pod.getStatus().getPodIP())
            .collect(Collectors.toList());
    }
    
    public String getServiceEndpoint(String serviceName) {
        Service service = client.services()
            .inNamespace("default")
            .withName(serviceName)
            .get();
        
        String clusterIP = service.getSpec().getClusterIP();
        int port = service.getSpec().getPorts().get(0).getPort();
        
        return String.format("http://%s:%d", clusterIP, port);
    }
}
```

**Dépendance Maven :**
```xml
<dependency>
    <groupId>io.fabric8</groupId>
    <artifactId>kubernetes-client</artifactId>
    <version>6.9.0</version>
</dependency>
```

**⚠️ Nécessite RBAC approprié :**
```yaml
apiVersion: v1
kind: ServiceAccount
metadata:
  name: service-discovery-sa
---
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: service-discovery-role
rules:
- apiGroups: [""]
  resources: ["services", "pods", "endpoints"]
  verbs: ["get", "list", "watch"]
---
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: service-discovery-binding
subjects:
- kind: ServiceAccount
  name: service-discovery-sa
roleRef:
  kind: Role
  name: service-discovery-role
  apiGroup: rbac.authorization.k8s.io
```

---

### Service Mesh pour Service Discovery avancé

**Istio / Linkerd :**
- Service discovery automatique
- Load balancing intelligent
- Circuit breaking
- Retry policies
- Traffic splitting

**Exemple avec Istio :**
```yaml
apiVersion: networking.istio.io/v1beta1
kind: VirtualService
metadata:
  name: backend
spec:
  hosts:
  - backend
  http:
  - route:
    - destination:
        host: backend
        subset: v1
      weight: 90
    - destination:
        host: backend
        subset: v2
      weight: 10  # Canary 10%
```

---

## 📝 Best Practices - Networking

### Services

✅ **Utiliser ClusterIP par défaut**
```yaml
type: ClusterIP  # Communication interne
```

✅ **Labels cohérents**
```yaml
selector:
  app: backend
  tier: api
  version: v1
```

✅ **Named ports**
```yaml
ports:
- name: http
  port: 80
  targetPort: 8080
- name: metrics
  port: 9090
  targetPort: 9090
```

✅ **Health checks sur les Pods**
```yaml
readinessProbe:
  httpGet:
    path: /health
    port: 8080
# Seuls les Pods Ready reçoivent du trafic
```

❌ **Éviter NodePort en production**
❌ **Éviter LoadBalancer pour chaque service** (utiliser Ingress)

---

### Ingress

✅ **TLS pour tous les endpoints publics**
```yaml
tls:
- hosts:
  - myapp.example.com
  secretName: tls-secret
```

✅ **Rate limiting**
```yaml
annotations:
  nginx.ingress.kubernetes.io/limit-rps: "100"
```

✅ **Redirect HTTP → HTTPS**
```yaml
annotations:
  nginx.ingress.kubernetes.io/force-ssl-redirect: "true"
```

✅ **Cert-Manager pour auto-renewal**

✅ **Un Ingress par environnement/namespace**

❌ **Éviter les regex complexes** (performance)

---

### Network Policies

✅ **Deny by default**
```yaml
# Appliquer en premier
spec:
  podSelector: {}
  policyTypes:
  - Ingress
  - Egress
```

✅ **Principe du moindre privilège**
```yaml
# Autoriser uniquement le strict nécessaire
ingress:
- from:
  - podSelector:
      matchLabels:
        app: frontend
  ports:
  - protocol: TCP
    port: 8080
```

✅ **Autoriser DNS explicitement**
```yaml
egress:
- to:
  - namespaceSelector:
      matchLabels:
        name: kube-system
  ports:
  - protocol: UDP
    port: 53
```

✅ **Labels explicites**
```yaml
metadata:
  labels:
    app: backend
    tier: api
    security-zone: dmz
```

✅ **Tester avant de déployer en prod**

❌ **Ne pas oublier le DNS dans egress**
❌ **Ne pas bloquer les health checks**

---

### DNS

✅ **FQDN avec trailing dot pour performance**
```java
String url = "http://backend.default.svc.cluster.local.:8080";
```

✅ **Headless Services pour StatefulSets**

✅ **Configurer ndots selon vos besoins**
```yaml
dnsConfig:
  options:
  - name: ndots
    value: "2"
```

✅ **Monitoring DNS latency**

❌ **Éviter DNS queries dans hot paths** (caching)

---

## 🎯 Checklist Networking

```yaml
production-networking-checklist:
  services:
    ✓ ClusterIP pour services internes
    ✓ Named ports définis
    ✓ Selectors corrects et testés
    ✓ SessionAffinity si nécessaire
    ✓ Health checks configurés
  
  ingress:
    ✓ TLS activé (cert-manager)
    ✓ Force HTTPS redirect
    ✓ Rate limiting configuré
    ✓ CORS si nécessaire
    ✓ Timeout appropriés
    ✓ Body size limits
    ✓ Monitoring et logs
  
  network-policies:
    ✓ Deny by default appliqué
    ✓ Ingress rules définies
    ✓ Egress rules définies
    ✓ DNS autorisé explicitement
    ✓ Labels cohérents
    ✓ Testé dans non-prod
  
  dns:
    ✓ CoreDNS opérationnel
    ✓ FQDN utilisés où nécessaire
    ✓ ndots optimisé
    ✓ Résolution testée
  
  monitoring:
    ✓ Métriques Ingress (latency, errors)
    ✓ Métriques Services (connections, throughput)
    ✓ DNS latency
    ✓ NetworkPolicy logs (si supporté)
```

---

# Chapitre 4 : Questions/Réponses d'Entretien - Networking

## 🌐 Services

### Q1 : Expliquez les différents types de Services et leurs cas d'usage.

**Réponse :**

**1. ClusterIP (défaut)**
```yaml
type: ClusterIP
```
- IP interne au cluster uniquement
- Accessible via DNS : `service.namespace.svc.cluster.local`
- **Use case** : Communication inter-services (backend ↔ database)

**2. NodePort**
```yaml
type: NodePort
spec:
  ports:
  - port: 80
    nodePort: 30080  # 30000-32767
```
- Expose sur `<NodeIP>:30080`
- Crée automatiquement un ClusterIP
- **Use case** : Accès externe simple (dev/test), pas de LoadBalancer

**3. LoadBalancer**
```yaml
type: LoadBalancer
```
- Provisionne un LB externe (AWS ELB, GCP LB, Azure LB)
- Crée ClusterIP + NodePort + LB
- **Use case** : Production avec cloud provider
- **Limitation** : 1 LB = 1 Service = coût élevé

**4. ExternalName**
```yaml
type: ExternalName
externalName: database.external.com
```
- Alias DNS vers service externe
- **Use case** : Migration progressive, services legacy

**Comparaison :**

| Type | Accessible | Coût | Use case |
|------|------------|------|----------|
| ClusterIP | Interne | Gratuit | Inter-services |
| NodePort | Externe | Gratuit | Dev/Test |
| LoadBalancer | Externe | $$$ | Prod (1 service) |
| ExternalName | Externe | Gratuit | Services externes |

**Recommandation Production :**
- Services internes → **ClusterIP**
- Services externes → **Ingress** (1 LB pour N services)

---

### Q2 : Qu'est-ce qu'un Headless Service et quand l'utiliser ?

**Réponse :**

Un **Headless Service** (`clusterIP: None`) ne crée pas de ClusterIP et retourne directement les IPs des Pods.

```yaml
apiVersion: v1
kind: Service
metadata:
  name: postgres-headless
spec:
  clusterIP: None  # ← Headless
  selector:
    app: postgres
  ports:
  - port: 5432
```

**DNS résolution :**
```bash
# Service normal
nslookup backend
# → 10.96.100.50 (ClusterIP unique)

# Headless Service
nslookup postgres-headless
# → 10.244.1.10 (Pod-0)
# → 10.244.1.11 (Pod-1)
# → 10.244.1.12 (Pod-2)
```

**Cas d'usage :**

**1. StatefulSets (principal)**
```yaml
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: postgres
spec:
  serviceName: postgres-headless  # ← Headless Service
```

**DNS par Pod :**
```
postgres-0.postgres-headless.default.svc.cluster.local
postgres-1.postgres-headless.default.svc.cluster.local
postgres-2.postgres-headless.default.svc.cluster.local
```

**Application :**
```java
// Connexion au master
String master = "postgres-0.postgres-headless.default.svc.cluster.local";

// Connexion aux replicas (read-only)
String replica1 = "postgres-1.postgres-headless.default.svc.cluster.local";
String replica2 = "postgres-2.postgres-headless.default.svc.cluster.local";
```

**2. Client-side load balancing**
```java
// Récupérer toutes les IPs des Pods
InetAddress[] addresses = InetAddress.getAllByName("postgres-headless");
// Client choisit le Pod (round-robin, least connections, etc.)
```

**3. Peer discovery (Elasticsearch, Kafka)**
```java
// Découvrir tous les peers du cluster
List<String> peers = discoverPeers("elasticsearch-headless");
```

**Différences clés :**

| Aspect | Service normal | Headless Service |
|--------|---------------|------------------|
| ClusterIP | Oui (unique) | Non (None) |
| DNS | 1 IP (ClusterIP) | N IPs (Pods) |
| Load balancing | kube-proxy | Client-side |
| Use case | Stateless apps | StatefulSets, peer discovery |

---

### Q3 : Comment fonctionne le load balancing dans un Service ?

**Réponse :**

**Composant : kube-proxy**

kube-proxy tourne sur chaque Node et maintient les règles réseau pour router le trafic vers les Pods.

**Modes de kube-proxy :**

**1. iptables (défaut)**
```bash
# kube-proxy crée des règles iptables
iptables -t nat -L KUBE-SERVICES
# KUBE-SVC-XXX → random Pod selection
```

**Caractéristiques :**
- Load balancing : **Random** (pas round-robin !)
- Performance : Bonne (kernel space)
- Scale : Limité (trop de rules si beaucoup de services)

**2. IPVS**
```bash
# Mode IPVS (meilleure performance)
ipvsadm -Ln
```

**Caractéristiques :**
- Algorithmes : round-robin, least connection, source hashing
- Performance : Excellente (optimisé pour LB)
- Scale : Très bon

**3. userspace (déprécié)**
- Proxy en espace utilisateur
- Performance : Faible

---

**Flow détaillé :**

```yaml
apiVersion: v1
kind: Service
metadata:
  name: backend
spec:
  selector:
    app: backend
  ports:
  - port: 80
    targetPort: 8080
```

**1. Service créé → ClusterIP assigné (ex: 10.96.100.50)**

**2. kube-proxy détecte et crée les règles iptables :**
```bash
# Traffic vers 10.96.100.50:80
# → Random entre :
#   - 10.244.1.10:8080 (Pod-1)
#   - 10.244.1.11:8080 (Pod-2)
#   - 10.244.1.12:8080 (Pod-3)
```

**3. Application fait une requête :**
```java
restTemplate.getForObject("http://backend:80/api/data", String.class);
```

**4. DNS résout `backend` → `10.96.100.50`**

**5. Packet envoyé vers `10.96.100.50:80`**

**6. iptables intercepte et redirige vers un Pod (random)**

**7. Pod répond, packet retrace le chemin**

---

**Session Affinity (Sticky Sessions) :**
```yaml
spec:
  sessionAffinity: ClientIP
  sessionAffinityConfig:
    clientIP:
      timeoutSeconds: 10800  # 3h
```

**Comportement :**
- Même IP client → toujours le même Pod
- Basé sur hash de l'IP source
- **Use case** : Sessions stateful, WebSocket

---

**Health checks impact :**
```yaml
readinessProbe:
  httpGet:
    path: /health
    port: 8080
  failureThreshold: 3
```

**Si un Pod n'est pas Ready :**
- Retiré des Endpoints automatiquement
- kube-proxy met à jour les règles
- Traffic non routé vers ce Pod

```bash
kubectl get endpoints backend
# NAME      ENDPOINTS                          AGE
# backend   10.244.1.10:8080,10.244.1.11:8080  5m
# Pod-2 (10.244.1.12) est NotReady → absent des Endpoints
```

---

## 🌍 Ingress

### Q4 : Quelle est la différence entre un Service LoadBalancer et un Ingress ?

**Réponse :**

**Service LoadBalancer :**
```yaml
apiVersion: v1
kind: Service
metadata:
  name: app1
spec:
  type: LoadBalancer
  ports:
  - port: 80
```

**Caractéristiques :**
- 1 LoadBalancer = 1 Service
- Load balancing L4 (TCP/UDP)
- Pas de routing intelligent
- **Coût** : 1 LB par service ($$$)

**Problème avec 10 services :**
```
Service-1 → LoadBalancer-1 ($20/mois)
Service-2 → LoadBalancer-2 ($20/mois)
...
Service-10 → LoadBalancer-10 ($20/mois)
Total: $200/mois
```

---

**Ingress :**
```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: multi-service-ingress
spec:
  rules:
  - host: api.example.com
    http:
      paths:
      - path: /
        backend:
          service:
            name: api-service
  - host: admin.example.com
    http:
      paths:
      - path: /
        backend:
          service:
            name: admin-service
```

**Caractéristiques :**
- 1 LoadBalancer pour N services
- Load balancing L7 (HTTP/HTTPS)
- Routing intelligent (host, path, headers)
- TLS termination
- **Coût** : 1 LB total ($20/mois)

**Architecture :**
```
                    Internet
                       ↓
            LoadBalancer ($20/mois)
                       ↓
              Ingress Controller
                       ↓
         ┌─────────────┼─────────────┐
         ↓             ↓             ↓
    Service A     Service B     Service C
```

**Comparaison :**

| Aspect | Service LB | Ingress |
|--------|-----------|---------|
| **Layer** | L4 (TCP/UDP) | L7 (HTTP/HTTPS) |
| **Routing** | IP:Port | Host, Path, Headers |
| **TLS** | Passthrough | Termination |
| **Coût** | $$ par service | $ partagé |
| **Use case** | Services non-HTTP | Applications web |

**Quand utiliser quoi :**

✅ **Service LoadBalancer :**
- Protocoles non-HTTP (gRPC, WebSocket, TCP custom)
- 1 seul service public
- Besoin de l'IP source préservée

✅ **Ingress :**
- Applications HTTP/HTTPS
- Multiples services publics
- TLS termination
- Routing complexe

**Recommandation :** Ingress pour 99% des cas web.

---

### Q5 : Comment implémentez-vous le routing basé sur le path dans un Ingress ?

**Réponse :**

**Objectif :** Router différentes URLs vers différents services.

```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: path-based-routing
  annotations:
    nginx.ingress.kubernetes.io/rewrite-target: /$2
spec:
  ingressClassName: nginx
  rules:
  - host: myapp.example.com
    http:
      paths:
      # /api/* → api-service
      - path: /api(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: api-service
            port:
              number: 8080
      
      # /admin/* → admin-service
      - path: /admin(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: admin-service
            port:
              number: 80
      
      # /* → web-service (default)
      - path: /
        pathType: Prefix
        backend:
          service:
            name: web-service
            port:
              number: 3000
```

**Routing :**
```bash
# Request: http://myapp.example.com/api/users
# → api-service:8080/users (rewrite-target supprime /api)

# Request: http://myapp.example.com/admin/dashboard
# → admin-service:80/dashboard

# Request: http://myapp.example.com/
# → web-service:3000/
```

---

**PathType options :**

**1. Prefix (le plus courant)**
```yaml
path: /api
pathType: Prefix
# Matche: /api, /api/, /api/users, /api/v1/users
```

**2. Exact**
```yaml
path: /api
pathType: Exact
# Matche: /api uniquement
# Ne matche PAS: /api/, /api/users
```

**3. ImplementationSpecific**
```yaml
path: /api(/|$)(.*)
pathType: ImplementationSpecific
# Regex Nginx : capture groups pour rewrite
```

---

**Annotation rewrite-target :**

**Sans rewrite :**
```yaml
path: /api
pathType: Prefix
# Request: /api/users
# → Backend reçoit: /api/users
```

**Avec rewrite :**
```yaml
annotations:
  nginx.ingress.kubernetes.io/rewrite-target: /$2
path: /api(/|$)(.*)
# Request: /api/users
# → Backend reçoit: /users (sans /api)
```

**Explications :**
- `(/|$)` : Matche `/` ou fin de string
- `(.*)` : Capture tout après → `$2`
- `rewrite-target: /$2` : Remplace le path par la capture

---

**Exemple complexe : Microservices**

```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: microservices-ingress
  annotations:
    nginx.ingress.kubernetes.io/rewrite-target: /$2
spec:
  rules:
  - host: api.example.com
    http:
      paths:
      - path: /users(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: user-service
            port:
              number: 8080
      
      - path: /orders(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: order-service
            port:
              number: 8080
      
      - path: /products(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: product-service
            port:
              number: 8080
```

**Routing :**
```
GET /users/123        → user-service:8080/123
GET /orders/456       → order-service:8080/456
GET /products?cat=tech → product-service:8080/?cat=tech
```

---

**Application Spring Boot :**

```java
@RestController
@RequestMapping  // Pas de préfixe /api ici
public class UserController {
    
    @GetMapping("/{id}")  // Reçoit: /{id} (pas /api/{id})
    public User getUser(@PathVariable Long id) {
        return userService.findById(id);
    }
}
```

**Configuration :**
```yaml
server:
  port: 8080
  # Pas de context-path, l'Ingress gère le routing
```

---

### Q6 : Comment configurez-vous HTTPS/TLS avec un Ingress ?

**Réponse :**

**Méthode 1 : TLS manuel (certificat existant)**

```bash
# Créer le Secret TLS
kubectl create secret tls myapp-tls \
  --cert=path/to/tls.crt \
  --key=path/to/tls.key
```

```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: tls-ingress
  annotations:
    nginx.ingress.kubernetes.io/force-ssl-redirect: "true"
spec:
  ingressClassName: nginx
  tls:
  - hosts:
    - myapp.example.com
    secretName: myapp-tls  # Secret créé ci-dessus
  rules:
  - host: myapp.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: spring-app
            port:
              number: 80
```

**Comportement :**
- HTTP (port 80) → Redirect vers HTTPS (force-ssl-redirect)
- HTTPS (port 443) → TLS termination par Ingress Controller
- Backend reçoit HTTP (déchiffré)

---

**Méthode 2 : Cert-Manager + Let's Encrypt (recommandé)**

**1. Installer Cert-Manager :**
```bash
kubectl apply -f https://github.com/cert-manager/cert-manager/releases/download/v1.13.0/cert-manager.yaml
```

**2. Créer un ClusterIssuer :**
```yaml
apiVersion: cert-manager.io/v1
kind: ClusterIssuer
metadata:
  name: letsencrypt-prod
spec:
  acme:
    server: https://acme-v02.api.letsencrypt.org/directory
    email: admin@example.com
    privateKeySecretRef:
      name: letsencrypt-prod-key
    solvers:
    - http01:
        ingress:
          class: nginx
```

**3. Ingress avec annotation Cert-Manager :**
```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: auto-tls-ingress
  annotations:
    cert-manager.io/cluster-issuer: "letsencrypt-prod"
    nginx.ingress.kubernetes.io/force-ssl-redirect: "true"
spec:
  ingressClassName: nginx
  tls:
  - hosts:
    - myapp.example.com
    secretName: myapp-auto-tls  # Créé automatiquement
  rules:
  - host: myapp.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: spring-app
            port:
              number: 80
```

**Process automatique :**
1. Cert-Manager détecte l'annotation
2. Crée une ressource `Certificate`
3. Lance le challenge ACME (HTTP-01)
4. Let's Encrypt valide le domaine
5. Certificat obtenu → Secret `myapp-auto-tls` créé
6. **Renouvellement automatique** avant expiration (90 jours)

```bash
# Vérifier le certificat
kubectl get certificate
# NAME              READY   SECRET            AGE
# myapp-auto-tls    True    myapp-auto-tls    5m

kubectl describe certificate myapp-auto-tls
# Events:
#   Type    Reason     Age   Message
#   Normal  Issued     5m    Certificate issued successfully
```

---

**Méthode 3 : Wildcard certificate**

```yaml
apiVersion: cert-manager.io/v1
kind: Certificate
metadata:
  name: wildcard-cert
spec:
  secretName: wildcard-tls
  issuerRef:
    name: letsencrypt-prod
    kind: ClusterIssuer
  dnsNames:
  - "*.example.com"  # Wildcard
  - "example.com"
```

```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: multi-subdomain-ingress
spec:
  tls:
  - hosts:
    - "*.example.com"
    secretName: wildcard-tls
  rules:
  - host: api.example.com
    http:
      paths:
      - path: /
        backend:
          service:
            name: api-service
  - host: admin.example.com
    http:
      paths:
      - path: /
        backend:
          service:
            name: admin-service
```

**⚠️ Wildcard nécessite DNS-01 challenge** (pas HTTP-01).

---

**Best Practices TLS :**

✅ **Force HTTPS redirect**
```yaml
nginx.ingress.kubernetes.io/force-ssl-redirect: "true"
```

✅ **HSTS (HTTP Strict Transport Security)**
```yaml
nginx.ingress.kubernetes.io/configuration-snippet: |
  more_set_headers "Strict-Transport-Security: max-age=31536000; includeSubDomains; preload";
```

✅ **TLS 1.2+ uniquement**
```yaml
nginx.ingress.kubernetes.io/ssl-protocols: "TLSv1.2 TLSv1.3"
```

✅ **Strong ciphers**
```yaml
nginx.ingress.kubernetes.io/ssl-ciphers: "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256"
```

✅ **Monitoring expiration**
```bash
# Prometheus alert
kubectl get certificate -o json | jq '.items[] | select(.status.notAfter < now + 30days)'
```

---

## 🔒 Network Policies

### Q7 : Qu'est-ce qu'une NetworkPolicy et pourquoi l'utiliser ?

**Réponse :**

Une **NetworkPolicy** définit comment les Pods peuvent communiquer entre eux et avec d'autres endpoints réseau.

**Par défaut : Aucune isolation**
```
Tous les Pods peuvent communiquer avec tous les Pods
```

**Avec NetworkPolicies : Zero Trust**
```
Deny by default + Allow explicites
```

---

**Exemple problème sans NetworkPolicy :**

```
Internet → Frontend → Backend → Database
              ↓
          Compromis !
              ↓
    Accès direct : Frontend → Database ❌
```

**Avec NetworkPolicy :**
```
Internet → Frontend ✓
Frontend → Backend ✓
Backend → Database ✓
Frontend → Database ❌ (bloqué)
```

---

**NetworkPolicy de base :**

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-frontend-to-backend
  namespace: prod
spec:
  # Appliqué aux Pods backend
  podSelector:
    matchLabels:
      app: backend
      tier: api
  
  policyTypes:
  - Ingress  # Règles entrantes
  
  ingress:
  - from:
    # Autorise uniquement depuis frontend
    - podSelector:
        matchLabels:
          app: frontend
          tier: web
    ports:
    - protocol: TCP
      port: 8080
```

**Résultat :**
- ✅ Frontend → Backend:8080 : Autorisé
- ❌ Autre Pod → Backend : Bloqué
- ❌ Frontend → Backend:5432 : Bloqué (port non autorisé)

---

**Pourquoi l'utiliser :**

**1. Sécurité (Defense in Depth)**
```
Application compromise → Lateral movement limité
```

**2. Compliance (PCI-DSS, HIPAA)**
```
Segmentation réseau requise
```

**3. Multi-tenancy**
```
Isolation entre équipes/projets
```

**4. Limiter blast radius**
```
Attaque contenue dans un segment
```

---

### Q8 : Comment implémentez-vous une stratégie "deny by default" avec NetworkPolicies ?

**Réponse :**

**Stratégie : Deny all → Allow explicites**

**Étape 1 : Deny all ingress et egress**

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: default-deny-all
  namespace: prod
spec:
  podSelector: {}  # Tous les Pods du namespace
  policyTypes:
  - Ingress
  - Egress
  # Pas de règles → deny all
```

**Résultat :**
- ❌ Aucun Pod ne peut recevoir de trafic
- ❌ Aucun Pod ne peut envoyer de trafic
- ❌ **DNS ne fonctionne plus** (important !)

---

**Étape 2 : Allow DNS (essentiel)**

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-dns
  namespace: prod
spec:
  podSelector: {}  # Tous les Pods
  policyTypes:
  - Egress
  egress:
  - to:
    - namespaceSelector:
        matchLabels:
          name: kube-system
    ports:
    - protocol: UDP
      port: 53
```

---

**Étape 3 : Allow frontend ingress (depuis Ingress Controller)**

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-frontend-ingress
  namespace: prod
spec:
  podSelector:
    matchLabels:
      app: frontend
      tier: web
  policyTypes:
  - Ingress
  ingress:
  - from:
    - namespaceSelector:
        matchLabels:
          name: ingress-nginx
    ports:
    - protocol: TCP
      port: 3000
```

---

**Étape 4 : Allow frontend → backend**

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: frontend-to-backend-egress
  namespace: prod
spec:
  podSelector:
    matchLabels:
      app: frontend
  policyTypes:
  - Egress
  egress:
  - to:
    - podSelector:
        matchLabels:
          app: backend
    ports:
    - protocol: TCP
      port: 8080
```

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: backend-from-frontend-ingress
  namespace: prod
spec:
  podSelector:
    matchLabels:
      app: backend
  policyTypes:
  - Ingress
  ingress:
  - from:
    - podSelector:
        matchLabels:
          app: frontend
    ports:
    - protocol: TCP
      port: 8080
```

---

**Étape 5 : Allow backend → database**

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: backend-to-db-egress
  namespace: prod
spec:
  podSelector:
    matchLabels:
      app: backend
  policyTypes:
  - Egress
  egress:
  - to:
    - podSelector:
        matchLabels:
          app: postgres
    ports:
    - protocol: TCP
      port: 5432
```

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: db-from-backend-ingress
  namespace: prod
spec:
  podSelector:
    matchLabels:
      app: postgres
  policyTypes:
  - Ingress
  ingress:
  - from:
    - podSelector:
        matchLabels:
          app: backend
    ports:
    - protocol: TCP
      port: 5432
```

---

**Résultat final :**
```
Internet → Ingress Controller → Frontend → Backend → Database
                                   ↓
                             Accès direct bloqué
                                   ↓
                                Database ❌
```

**Ordre d'application recommandé :**
1. Deny all
2. Allow DNS (critique !)
3. Allow ingress pour services publics
4. Allow communication inter-services (paire egress/ingress)

---

**Script de déploiement :**
```bash
#!/bin/bash
# deploy-network-policies.sh

# 1. Deny all (destructive!)
kubectl apply -f 01-deny-all.yaml

# 2. DNS (urgent, sinon rien ne fonctionne)
kubectl apply -f 02-allow-dns.yaml

# 3. Services publics
kubectl apply -f 03-allow-frontend-ingress.yaml

# 4. Inter-services
kubectl apply -f 04-frontend-to-backend.yaml
kubectl apply -f 05-backend-to-database.yaml

# Tester
kubectl run test --image=curlimages/curl --rm -it -- sh
```

---

**Best Practices :**

✅ **Tester dans dev/staging d'abord**
```bash
# Appliquer en mode audit (si supporté par CNI)
```

✅ **Toujours allow DNS**
```yaml
egress:
- to:
  - namespaceSelector:
      matchLabels:
        name: kube-system
  ports:
  - protocol: UDP
    port: 53
```

✅ **Labels explicites**
```yaml
metadata:
  labels:
    app: backend
    tier: api
    security-zone: dmz
```

✅ **Documenter les policies**
```yaml
metadata:
  annotations:
    description: "Allows frontend to access backend API"
    owner: "platform-team"
```

✅ **Monitoring**
```bash
# Logs (Calico)
kubectl logs -n kube-system -l k8s-app=calico-node | grep DENY
```

❌ **Ne pas oublier les health checks**
```yaml
# Kubelet doit pouvoir accéder aux probes
ingress:
- from:
  - ipBlock:
      cidr: <node-cidr>
  ports:
  - protocol: TCP
    port: 8080  # Health check port
```

---

## 🌐 DNS & Service Discovery

### Q9 : Comment fonctionne la résolution DNS dans Kubernetes ?

**Réponse :**

**Composant : CoreDNS**

CoreDNS est le serveur DNS par défaut dans Kubernetes.

```bash
kubectl get pods -n kube-system -l k8s-app=kube-dns
# NAME                       READY   STATUS    RESTARTS   AGE
# coredns-5d78c9869d-abc12   1/1     Running   0          10d

kubectl get service -n kube-system kube-dns
# NAME       TYPE        CLUSTER-IP   EXTERNAL-IP   PORT(S)        AGE
# kube-dns   ClusterIP   10.96.0.10   <none>        53/UDP,53/TCP  10d
```

---

**Format DNS pour Services :**
```
<service-name>.<namespace>.svc.cluster.local
```

**Exemples :**
```bash
# Service "backend" dans namespace "default"
backend.default.svc.cluster.local

# Service "postgres" dans namespace "prod"
postgres.prod.svc.cluster.local

# Service "redis" dans namespace "cache"
redis.cache.svc.cluster.local
```

**Résolution simplifiée (search domains) :**
```bash
# Depuis un Pod dans namespace "default"
curl http://backend:8080
# Résolu en: backend.default.svc.cluster.local

# Accès à un service dans autre namespace
curl http://postgres.prod:5432
# Résolu en: postgres.prod.svc.cluster.local

# FQDN complet (toujours fonctionne)
curl http://backend.default.svc.cluster.local:8080
```

---

**Configuration DNS dans un Pod :**

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: app
spec:
  containers:
  - name: app
    image: myapp:1.0
  
  # DNS policy
  dnsPolicy: ClusterFirst  # Défaut
  
  # DNS config (optionnel)
  dnsConfig:
    nameservers:
    - 8.8.8.8  # DNS externe additionnel
    searches:
    - default.svc.cluster.local
    - svc.cluster.local
    - cluster.local
    options:
    - name: ndots
      value: "5"
```

**Fichier /etc/resolv.conf dans le Pod :**
```bash
kubectl exec app -- cat /etc/resolv.conf
# nameserver 10.96.0.10          (CoreDNS)
# search default.svc.cluster.local svc.cluster.local cluster.local
# options ndots:5
```

---

**Process de résolution :**

**Query : `backend`**

```
1. Pod → CoreDNS (10.96.0.10:53)
2. CoreDNS essaie (avec ndots:5):
   a. backend.default.svc.cluster.local.  ← 5 dots → FOUND!
   b. (s'arrête ici si trouvé)
3. CoreDNS répond : 10.96.100.50 (ClusterIP)
4. Pod envoie traffic vers 10.96.100.50
5. kube-proxy route vers un Pod backend
```

**Query : `google.com`**

```
1. Pod → CoreDNS
2. CoreDNS essaie (avec ndots:5):
   a. google.com.default.svc.cluster.local.  ← NOT FOUND
   b. google.com.svc.cluster.local.          ← NOT FOUND
   c. google.com.cluster.local.              ← NOT FOUND
   d. google.com.                            ← FOUND (forwarded to upstream DNS)
3. CoreDNS forward vers upstream (8.8.8.8)
4. Upstream répond avec l'IP publique
```

---

**DNS pour Headless Services :**

```yaml
apiVersion: v1
kind: Service
metadata:
  name: postgres
spec:
  clusterIP: None  # Headless
  selector:
    app: postgres
```

```bash
nslookup postgres.default.svc.cluster.local
# Server:    10.96.0.10
# Address 1: 10.96.0.10 kube-dns.kube-system.svc.cluster.local
#
# Name:      postgres.default.svc.cluster.local
# Address 1: 10.244.1.10 postgres-0.postgres.default.svc.cluster.local
# Address 2: 10.244.1.11 postgres-1.postgres.default.svc.cluster.local
# Address 3: 10.244.1.12 postgres-2.postgres.default.svc.cluster.local
```

**DNS par Pod (StatefulSet) :**
```bash
nslookup postgres-0.postgres.default.svc.cluster.local
# Address 1: 10.244.1.10 postgres-0.postgres.default.svc.cluster.local
```

---

**Application Java/Spring Boot :**

```java
@Configuration
public class DatabaseConfig {
    
    // DNS résolu automatiquement
    @Bean
    public DataSource dataSource() {
        HikariConfig config = new HikariConfig();
        
        // Résolution DNS simple (même namespace)
        config.setJdbcUrl("jdbc:postgresql://postgres:5432/mydb");
        
        // Autre namespace
        // config.setJdbcUrl("jdbc:postgresql://postgres.prod:5432/mydb");
        
        // FQDN (toujours fonctionne)
        // config.setJdbcUrl("jdbc:postgresql://postgres.prod.svc.cluster.local:5432/mydb");
        
        config.setUsername("user");
        config.setPassword("pass");
        
        return new HikariDataSource(config);
    }
}
```

---

**Performance : ndots problème**

**Problème avec ndots=5 (défaut) :**

```bash
# Query: backend
# CoreDNS reçoit 4 queries:
1. backend.default.svc.cluster.local.  ← 5 dots → FOUND
2. (inutiles car déjà trouvé, mais search list complète)
```

**Pour un domaine externe :**
```bash
# Query: api.external.com (2 dots)
# CoreDNS reçoit 4 queries:
1. api.external.com.default.svc.cluster.local.  ← NOT FOUND
2. api.external.com.svc.cluster.local.          ← NOT FOUND
3. api.external.com.cluster.local.              ← NOT FOUND
4. api.external.com.                            ← FOUND

# 4 queries DNS pour 1 résolution !
```

**Solution 1 : FQDN avec trailing dot**
```java
// ✅ 1 seule query DNS
String url = "http://backend.default.svc.cluster.local.:8080";
//                                                      ↑ trailing dot
```

**Solution 2 : Réduire ndots**
```yaml
dnsConfig:
  options:
  - name: ndots
    value: "2"  # Au lieu de 5
```

**Solution 3 : Cache application-level**
```java
@Component
public class DNSCache {
    private final Map<String, InetAddress> cache = new ConcurrentHashMap<>();
    
    public InetAddress resolve(String hostname) {
        return cache.computeIfAbsent(hostname, h -> {
            try {
                return InetAddress.getByName(h);
            } catch (UnknownHostException e) {
                throw new RuntimeException(e);
            }
        });
    }
}
```

---

### Q10 : Comment debuggez-vous des problèmes réseau dans Kubernetes ?

**Réponse :**

**Checklist systématique :**

---

**1. Vérifier les Pods**

```bash
# Status des Pods
kubectl get pods
# NAME              READY   STATUS    RESTARTS   AGE
# backend-abc123    1/1     Running   0          5m

# Si NotReady ou CrashLoopBackOff
kubectl describe pod backend-abc123
kubectl logs backend-abc123

# IP du Pod
kubectl get pod backend-abc123 -o wide
# NAME              IP            NODE
# backend-abc123    10.244.1.10   worker-1
```

---

**2. Vérifier le Service**

```bash
# Service existe ?
kubectl get service backend
# NAME      TYPE        CLUSTER-IP      EXTERNAL-IP   PORT(S)    AGE
# backend   ClusterIP   10.96.100.50    <none>        80/TCP     5m

# Describe
kubectl describe service backend

# Endpoints (IPs des Pods)
kubectl get endpoints backend
# NAME      ENDPOINTS                                      AGE
# backend   10.244.1.10:8080,10.244.1.11:8080             5m

# Si ENDPOINTS vide → problème !
# Causes possibles:
# - Selector ne match pas les Pods
# - Pods pas Ready
# - Labels incorrects
```

**Debug selector :**
```bash
# Labels du Service
kubectl get service backend -o yaml | grep -A 5 selector
# selector:
#   app: backend
#   tier: api

# Labels des Pods
kubectl get pods -l app=backend,tier=api
# Si vide → selector ne match pas !

# Vérifier les labels réels
kubectl get pods --show-labels
```

---

**3. Tester DNS**

```bash
# Créer un Pod de test
kubectl run test-dns --image=busybox:1.28 --rm -it -- sh

# Inside Pod:
# Résoudre le Service
nslookup backend
# Server:    10.96.0.10
# Address 1: 10.96.0.10 kube-dns.kube-system.svc.cluster.local
# Name:      backend
# Address 1: 10.96.100.50 backend.default.svc.cluster.local

# Si échec:
nslookup backend.default.svc.cluster.local

# Tester CoreDNS
nslookup kube-dns.kube-system.svc.cluster.local

# Si DNS ne fonctionne pas:
kubectl get pods -n kube-system -l k8s-app=kube-dns
# CoreDNS Pods doivent être Running
```

---

**4. Tester connectivité Pod-to-Pod**

```bash
# IP d'un Pod backend
BACKEND_IP=$(kubectl get pod backend-abc123 -o jsonpath='{.status.podIP}')

# Depuis un autre Pod
kubectl run test --image=busybox --rm -it -- sh
wget -O- http://$BACKEND_IP:8080
# ou
ping $BACKEND_IP

# Si échec → problème réseau (CNI, NetworkPolicy)
```

---

**5. Tester connectivité via Service**

```bash
# Depuis un Pod
kubectl run test --image=curlimages/curl --rm -it -- sh
curl http://backend:80

# Si échec mais Pod-to-Pod fonctionne → problème Service/kube-proxy
```

---

**6. Vérifier NetworkPolicies**

```bash
# Lister les NetworkPolicies
kubectl get networkpolicies

# Describe
kubectl describe networkpolicy allow-backend

# Tester avec labels différents
kubectl run test-allowed --image=curlimages/curl --labels=app=frontend --rm -it -- sh
curl http://backend:80  # Devrait fonctionner

kubectl run test-blocked --image=curlimages/curl --labels=app=other --rm -it -- sh
curl http://backend:80  # Devrait être bloqué
```

**Debug NetworkPolicy :**
```bash
# Calico
kubectl logs -n kube-system -l k8s-app=calico-node | grep DENY

# Cilium
kubectl -n kube-system exec -it cilium-xxx -- cilium monitor --type drop
```

---

**7. Vérifier kube-proxy**

```bash
# kube-proxy Pods
kubectl get pods -n kube-system -l k8s-app=kube-proxy

# Logs
kubectl logs -n kube-system kube-proxy-xxx

# iptables rules (sur un Node)
sudo iptables-save | grep backend

# IPVS (si mode IPVS)
sudo ipvsadm -Ln
```

---

**8. Vérifier Ingress**

```bash
# Ingress resource
kubectl get ingress
kubectl describe ingress myapp-ingress

# Ingress Controller Pods
kubectl get pods -n ingress-nginx

# Logs Ingress Controller
kubectl logs -n ingress-nginx deployment/ingress-nginx-controller

# Tester depuis l'extérieur
curl -H "Host: myapp.example.com" http://<ingress-ip>
```

---

**9. tcpdump sur un Pod**

```bash
# Installer tcpdump dans le Pod (si possible)
kubectl exec -it backend-abc123 -- sh
apk add tcpdump  # Alpine
tcpdump -i any port 8080

# Ou depuis un Node (hostNetwork)
kubectl run tcpdump --image=nicolaka/netshoot --rm -it --hostnetwork -- tcpdump -i any port 8080
```

---

**10. Outils de debug avancés**

**Netshoot (toolkit complet) :**
```bash
kubectl run netshoot --image=nicolaka/netshoot --rm -it -- bash

# Tools disponibles: curl, wget, nslookup, dig, tcpdump, iperf, etc.
```

**Debug containers (Ephemeral Containers) :**
```bash
kubectl debug backend-abc123 -it --image=busybox --share-processes
```

---

**Checklist complète :**

```yaml
debugging-checklist:
  pods:
    ✓ kubectl get pods (status Running ?)
    ✓ kubectl describe pod (events ?)
    ✓ kubectl logs (erreurs applicatives ?)
    ✓ Health checks configurés ?
  
  service:
    ✓ kubectl get service (existe ?)
    ✓ kubectl get endpoints (non vide ?)
    ✓ Selector match les Pods ?
    ✓ Port mapping correct ?
  
  dns:
    ✓ CoreDNS Pods Running ?
    ✓ nslookup service (résolu ?)
    ✓ /etc/resolv.conf correct ?
  
  network:
    ✓ Ping Pod-to-Pod (connectivité ?)
    ✓ curl Service (kube-proxy OK ?)
    ✓ NetworkPolicies appliquées ?
    ✓ CNI plugin opérationnel ?
  
  ingress:
    ✓ Ingress resource créé ?
    ✓ Ingress Controller Running ?
    ✓ DNS externe pointé ?
    ✓ TLS configuré ?
```

---

## 🎯 Questions Situationnelles

### Q11 : Vous avez 10 microservices qui doivent être exposés publiquement. Comment architectez-vous le networking ?

**Réponse :**

**❌ Mauvaise approche : LoadBalancer par service**

```yaml
# Service-1
type: LoadBalancer  # $20/mois
# Service-2
type: LoadBalancer  # $20/mois
# ...
# Service-10
type: LoadBalancer  # $20/mois

# Total: $200/mois + complexité gestion
```

**Problèmes :**
- Coût élevé (10 LoadBalancers)
- 10 IPs publiques à gérer
- Pas de TLS centralisé
- Pas de routing intelligent

---

**✅ Bonne approche : Ingress unique**

```yaml
# 1. Services internes (ClusterIP)
apiVersion: v1
kind: Service
metadata:
  name: user-service
spec:
  type: ClusterIP
  selector:
    app: user-service
  ports:
  - port: 8080
---
# (Répéter pour les 10 services...)
---
# 2. Ingress unique avec routing
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: api-gateway
  annotations:
    cert-manager.io/cluster-issuer: "letsencrypt-prod"
    nginx.ingress.kubernetes.io/rate-limit: "100"
spec:
  ingressClassName: nginx
  tls:
  - hosts:
    - api.example.com
    secretName: api-tls
  rules:
  - host: api.example.com
    http:
      paths:
      # /users/* → user-service
      - path: /users(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: user-service
            port:
              number: 8080
      
      # /orders/* → order-service
      - path: /orders(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: order-service
            port:
              number: 8080
      
      # /products/* → product-service
      - path: /products(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: product-service
            port:
              number: 8080
      
      # ... (7 autres services)
```

**Avantages :**
- ✅ 1 LoadBalancer = $20/mois total
- ✅ 1 IP publique
- ✅ TLS centralisé (auto-renew avec cert-manager)
- ✅ Rate limiting global
- ✅ CORS, auth centralisés
- ✅ Monitoring centralisé

---

**Architecture complète :**

```
                        Internet
                           ↓
                   DNS: api.example.com
                           ↓
                    LoadBalancer ($20)
                           ↓
                  Ingress Controller
                    (Nginx Pods x3)
                           ↓
            ┌──────────────┼──────────────┐
            ↓              ↓              ↓
      user-service   order-service   product-service
        (ClusterIP)    (ClusterIP)     (ClusterIP)
            ↓              ↓              ↓
       User Pods      Order Pods     Product Pods
```

**Configuration Ingress Controller :**
```yaml
apiVersion: v1
kind: Service
metadata:
  name: ingress-nginx-controller
  namespace: ingress-nginx
spec:
  type: LoadBalancer  # 1 seul LoadBalancer
  ports:
  - port: 80
    targetPort: 80
  - port: 443
    targetPort: 443
```

---

**Alternative : Subdomains**

```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: api-gateway
spec:
  rules:
  # users.api.example.com → user-service
  - host: users.api.example.com
    http:
      paths:
      - path: /
        backend:
          service:
            name: user-service
  
  # orders.api.example.com → order-service
  - host: orders.api.example.com
    http:
      paths:
      - path: /
        backend:
          service:
            name: order-service
  
  # ... (8 autres sous-domaines)
```

**Choix : Paths vs Subdomains**

| Approche | URL | Avantages | Inconvénients |
|----------|-----|-----------|---------------|
| **Path-based** | api.example.com/users | 1 domaine, simple | Rewrite complexe |
| **Subdomain** | users.api.example.com | Isolation claire | N certificats |

---

**NetworkPolicies pour sécuriser :**

```yaml
# 1. Deny all
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: default-deny-all
spec:
  podSelector: {}
  policyTypes:
  - Ingress
  - Egress
---
# 2. Allow Ingress Controller → Services
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-from-ingress
spec:
  podSelector:
    matchLabels:
      tier: api  # Tous les services API
  policyTypes:
  - Ingress
  ingress:
  - from:
    - namespaceSelector:
        matchLabels:
          name: ingress-nginx
    ports:
    - protocol: TCP
      port: 8080
---
# 3. Services peuvent communiquer entre eux (si nécessaire)
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-inter-service
spec:
  podSelector:
    matchLabels:
      tier: api
  policyTypes:
  - Egress
  egress:
  - to:
    - podSelector:
        matchLabels:
          tier: api
```

---

**Monitoring :**

```yaml
# Prometheus ServiceMonitor
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: ingress-nginx
spec:
  selector:
    matchLabels:
      app: ingress-nginx
  endpoints:
  - port: metrics
    interval: 30s
```

**Grafana dashboards :**
- Requests per second par service
- Latency p50, p95, p99
- Error rate (4xx, 5xx)
- Top endpoints

---

**Cost comparison :**

| Architecture | Monthly Cost | Complexity |
|--------------|--------------|------------|
| 10 LoadBalancers | $200 | High |
| 1 Ingress | $20 | Low |
| **Savings** | **$180** | **90% moins complex** |

---

### Q12 : Comment sécurisez-vous la communication entre microservices ?

**Réponse :**

**Niveaux de sécurité :**

---

**Niveau 1 : NetworkPolicies (minimum)**

```yaml
# Deny by default + Allow explicites
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: frontend-to-backend-only
spec:
  podSelector:
    matchLabels:
      app: backend
  policyTypes:
  - Ingress
  ingress:
  - from:
    - podSelector:
        matchLabels:
          app: frontend
    ports:
    - protocol: TCP
      port: 8080
```

**✅ Avantages :**
- Gratuit
- Simple à implémenter
- Limite lateral movement

**❌ Limitations :**
- Pas de chiffrement
- Pas d'authentification forte
- Vulnérable si Pod compromis

---

**Niveau 2 : mTLS avec Service Mesh (recommandé)**

**Avec Istio :**

```yaml
# 1. Enable mTLS strict
apiVersion: security.istio.io/v1beta1
kind: PeerAuthentication
metadata:
  name: default
  namespace: prod
spec:
  mtls:
    mode: STRICT  # mTLS obligatoire
```

**Comportement :**
- Chiffrement automatique de tout le trafic inter-services
- Certificats auto-générés et rotatés (Istio CA)
- Authentification mutuelle (client + serveur)

```
Frontend Pod → [Envoy Proxy] --mTLS--> [Envoy Proxy] → Backend Pod
                     ↓                        ↓
              Client cert                Server cert
              (auto-generated)          (auto-generated)
```

**Authorization policies :**
```yaml
apiVersion: security.istio.io/v1beta1
kind: AuthorizationPolicy
metadata:
  name: backend-authz
  namespace: prod
spec:
  selector:
    matchLabels:
      app: backend
  rules:
  - from:
    - source:
        principals: ["cluster.local/ns/prod/sa/frontend"]
    to:
    - operation:
        methods: ["GET", "POST"]
        paths: ["/api/*"]
```

**✅ Avantages :**
- Chiffrement end-to-end automatique
- Zero-trust architecture
- Policies granulaires (HTTP-level)
- Observability (traces, metrics)

**❌ Limitations :**
- Complexité (Service Mesh)
- Overhead (latency +1-2ms)
- Ressources (Envoy sidecars)

---

**Niveau 3 : API Gateway + JWT**

```yaml
# API Gateway (Kong, Ambassador)
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: api-gateway
  annotations:
    konghq.com/plugins: jwt-auth
spec:
  rules:
  - host: api.example.com
    http:
      paths:
      - path: /
        backend:
          service:
            name: backend
```

**JWT validation :**
```java
@Configuration
@EnableWebSecurity
public class SecurityConfig {
    
    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
        http
            .oauth2ResourceServer()
                .jwt()
                .jwtAuthenticationConverter(jwtAuthenticationConverter());
        
        http
            .authorizeHttpRequests()
                .requestMatchers("/api/public/**").permitAll()
                .requestMatchers("/api/admin/**").hasRole("ADMIN")
                .anyRequest().authenticated();
        
        return http.build();
    }
}
```

**Service-to-Service avec JWT :**
```java
@Service
public class OrderServiceClient {
    
    @Autowired
    private RestTemplate restTemplate;
    
    @Autowired
    private JwtService jwtService;
    
    public Order getOrder(Long orderId) {
        // Générer JWT pour service-to-service
        String token = jwtService.generateServiceToken("user-service");
        
        HttpHeaders headers = new HttpHeaders();
        headers.setBearerAuth(token);
        
        HttpEntity<Void> entity = new HttpEntity<>(headers);
        
        return restTemplate.exchange(
            "http://order-service:8080/api/orders/" + orderId,
            HttpMethod.GET,
            entity,
            Order.class
        ).getBody();
    }
}
```

---

**Niveau 4 : Combinaison (Defense in Depth)**

```yaml
architecture:
  layer-1-network:
    - NetworkPolicies (deny by default)
    - Firewall rules (cloud provider)
  
  layer-2-transport:
    - mTLS (Service Mesh)
    - Certificate rotation
  
  layer-3-application:
    - JWT authentication
    - OAuth2/OIDC
    - API keys
  
  layer-4-authorization:
    - RBAC (Kubernetes)
    - OPA policies
    - Fine-grained permissions
  
  layer-5-monitoring:
    - Audit logs
    - Anomaly detection
    - Alerts sur accès non autorisés
```

---

**Best Practices :**

✅ **Always encrypt inter-service communication (mTLS)**

✅ **Implement least privilege (NetworkPolicies)**

✅ **Use service accounts per service**
```yaml
spec:
  serviceAccountName: backend-sa  # Dedicated SA
```

✅ **Rotate credentials regularly**

✅ **Audit all service-to-service calls**

✅ **Implement circuit breakers**
```yaml
# Istio DestinationRule
apiVersion: networking.istio.io/v1beta1
kind: DestinationRule
metadata:
  name: backend
spec:
  host: backend
  trafficPolicy:
    outlierDetection:
      consecutiveErrors: 5
      interval: 30s
      baseEjectionTime: 30s
```

✅ **Monitor for anomalies**
```yaml
# Alert: unusual service-to-service traffic
- alert: UnusualServiceCommunication
  expr: rate(istio_requests_total{source_app="frontend",destination_app!~"backend|api"}[5m]) > 0
  annotations:
    summary: "Frontend communicating with unexpected service"
```

---

## 🎓 Conseils pour l'Entretien

### Points clés à maîtriser :
1. **Services** : 4 types, use cases, load balancing
2. **Ingress** : Routing, TLS, annotations
3. **NetworkPolicies** : Deny by default, ingress/egress
4. **DNS** : CoreDNS, résolution, performance
5. **Debugging** : Checklist systématique

### Red flags à éviter :
- ❌ LoadBalancer pour chaque service
- ❌ Pas de NetworkPolicies en production
- ❌ Pas de TLS sur Ingress
- ❌ Ignorer les Endpoints vides
- ❌ Ne pas comprendre DNS resolution

### Points bonus :
- ✅ Cert-Manager pour auto-TLS
- ✅ Service Mesh (Istio) pour mTLS
- ✅ Canary deployments avec Ingress
- ✅ Performance tuning (ndots, FQDN)
- ✅ Monitoring réseau (Prometheus, Grafana)

---