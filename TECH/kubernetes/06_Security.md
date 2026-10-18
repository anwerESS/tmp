## [**..**](./00_index.md)


# Chapitre 6 : Security

## 6.1. RBAC (Roles, RoleBindings, ClusterRoles)

### Qu'est-ce que RBAC ?

**RBAC (Role-Based Access Control)** contrôle qui peut faire quoi dans le cluster Kubernetes.

**Composants principaux :**
- **User/Group** : Qui (humain ou ServiceAccount)
- **Role/ClusterRole** : Permissions (verbs + resources)
- **RoleBinding/ClusterRoleBinding** : Lien User → Role

```
User/ServiceAccount → RoleBinding → Role → Permissions
```

---

### Role vs ClusterRole

**Role (namespace-scoped) :**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: pod-reader
  namespace: dev
rules:
- apiGroups: [""]  # "" = core API group
  resources: ["pods"]
  verbs: ["get", "list", "watch"]
```

**ClusterRole (cluster-wide) :**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: ClusterRole
metadata:
  name: secret-reader
rules:
- apiGroups: [""]
  resources: ["secrets"]
  verbs: ["get", "list"]
  # Accès à tous les namespaces
```

**Différences :**

| Aspect | Role | ClusterRole |
|--------|------|-------------|
| Scope | 1 namespace | Cluster entier |
| Resources | Namespacées | Namespacées + Non-namespacées |
| Use case | Équipe/App spécifique | Admin, monitoring global |

---

### RoleBinding vs ClusterRoleBinding

**RoleBinding (namespace-scoped) :**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: read-pods
  namespace: dev
subjects:
- kind: User
  name: john@example.com
  apiGroup: rbac.authorization.k8s.io
roleRef:
  kind: Role
  name: pod-reader
  apiGroup: rbac.authorization.k8s.io
```

**ClusterRoleBinding (cluster-wide) :**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: ClusterRoleBinding
metadata:
  name: read-secrets-global
subjects:
- kind: User
  name: admin@example.com
  apiGroup: rbac.authorization.k8s.io
roleRef:
  kind: ClusterRole
  name: secret-reader
  apiGroup: rbac.authorization.k8s.io
```

---

### Verbs (Actions)

```yaml
rules:
- apiGroups: [""]
  resources: ["pods"]
  verbs:
  - get      # Lire un pod spécifique
  - list     # Lister tous les pods
  - watch    # Watch changes
  - create   # Créer un pod
  - update   # Modifier un pod
  - patch    # Patch un pod
  - delete   # Supprimer un pod
  - deletecollection  # Supprimer plusieurs
```

---

### Exemples pratiques

**1. Developer Role (lecture seule) :**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: developer
  namespace: prod
rules:
# Peut voir Pods, Services, ConfigMaps
- apiGroups: [""]
  resources: ["pods", "services", "configmaps"]
  verbs: ["get", "list", "watch"]

# Peut voir logs
- apiGroups: [""]
  resources: ["pods/log"]
  verbs: ["get"]

# Peut voir Deployments
- apiGroups: ["apps"]
  resources: ["deployments", "replicasets"]
  verbs: ["get", "list", "watch"]

# ❌ Pas de Secrets
# ❌ Pas de delete/update
```

**2. DevOps Role (gestion complète) :**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: devops
  namespace: prod
rules:
# Full access sur workloads
- apiGroups: ["", "apps", "batch"]
  resources: 
  - pods
  - services
  - deployments
  - statefulsets
  - jobs
  - cronjobs
  verbs: ["*"]  # Toutes les actions

# Full access sur config
- apiGroups: [""]
  resources: ["configmaps", "secrets"]
  verbs: ["*"]

# Peut gérer Ingress
- apiGroups: ["networking.k8s.io"]
  resources: ["ingresses"]
  verbs: ["*"]
```

**3. CI/CD ServiceAccount (deploiement) :**
```yaml
apiVersion: v1
kind: ServiceAccount
metadata:
  name: gitlab-ci
  namespace: prod
---
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: deployer
  namespace: prod
rules:
# Peut créer/updater Deployments
- apiGroups: ["apps"]
  resources: ["deployments"]
  verbs: ["get", "list", "create", "update", "patch"]

# Peut créer ConfigMaps
- apiGroups: [""]
  resources: ["configmaps"]
  verbs: ["get", "create", "update"]

# Peut lire Secrets (pour les monter dans Pods)
- apiGroups: [""]
  resources: ["secrets"]
  verbs: ["get"]

# ❌ Pas de delete
# ❌ Pas d'accès aux autres namespaces
---
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: gitlab-ci-deployer
  namespace: prod
subjects:
- kind: ServiceAccount
  name: gitlab-ci
  namespace: prod
roleRef:
  kind: Role
  name: deployer
  apiGroup: rbac.authorization.k8s.io
```

---

### ClusterRole pour ressources cluster-wide

```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: ClusterRole
metadata:
  name: node-reader
rules:
# Peut lire les Nodes
- apiGroups: [""]
  resources: ["nodes"]
  verbs: ["get", "list"]

# Peut lire les PersistentVolumes
- apiGroups: [""]
  resources: ["persistentvolumes"]
  verbs: ["get", "list"]

# Peut lire les StorageClasses
- apiGroups: ["storage.k8s.io"]
  resources: ["storageclasses"]
  verbs: ["get", "list"]
```

---

### Aggregated ClusterRoles

```yaml
# ClusterRole de base
apiVersion: rbac.authorization.k8s.io/v1
kind: ClusterRole
metadata:
  name: monitoring-reader
  labels:
    rbac.example.com/aggregate-to-monitoring: "true"
rules:
- apiGroups: [""]
  resources: ["pods", "nodes"]
  verbs: ["get", "list"]
---
# ClusterRole agrégé
apiVersion: rbac.authorization.k8s.io/v1
kind: ClusterRole
metadata:
  name: monitoring
aggregationRule:
  clusterRoleSelectors:
  - matchLabels:
      rbac.example.com/aggregate-to-monitoring: "true"
rules: []  # Automatiquement rempli par agrégation
```

---

### Tester les permissions

```bash
# Vérifier si l'utilisateur actuel peut faire une action
kubectl auth can-i create deployments --namespace=prod
# yes

kubectl auth can-i delete secrets --namespace=prod
# no

# Vérifier pour un autre utilisateur
kubectl auth can-i list pods --as=john@example.com --namespace=dev
# yes

# Vérifier pour un ServiceAccount
kubectl auth can-i create deployments \
  --as=system:serviceaccount:prod:gitlab-ci \
  --namespace=prod
# yes

# Lister toutes les permissions d'un utilisateur
kubectl auth can-i --list --as=john@example.com --namespace=dev
```

---

## 6.2. ServiceAccounts

### Qu'est-ce qu'un ServiceAccount ?

Un **ServiceAccount** fournit une identité pour les Pods afin qu'ils puissent interagir avec l'API Kubernetes.

**Différence User vs ServiceAccount :**
- **User** : Humain (kubectl)
- **ServiceAccount** : Pod/Application

---

### ServiceAccount par défaut

Chaque namespace a un ServiceAccount `default` :

```bash
kubectl get serviceaccount -n default
# NAME      SECRETS   AGE
# default   0         10d
```

Chaque Pod utilise automatiquement le SA `default` sauf si spécifié autrement.

---

### Créer et utiliser un ServiceAccount

```yaml
# 1. Créer ServiceAccount
apiVersion: v1
kind: ServiceAccount
metadata:
  name: my-app-sa
  namespace: prod
---
# 2. Créer Role
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: pod-reader
  namespace: prod
rules:
- apiGroups: [""]
  resources: ["pods"]
  verbs: ["get", "list"]
---
# 3. Lier SA au Role
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: my-app-sa-binding
  namespace: prod
subjects:
- kind: ServiceAccount
  name: my-app-sa
  namespace: prod
roleRef:
  kind: Role
  name: pod-reader
  apiGroup: rbac.authorization.k8s.io
---
# 4. Utiliser dans un Pod
apiVersion: v1
kind: Pod
metadata:
  name: my-app
  namespace: prod
spec:
  serviceAccountName: my-app-sa  # ← Utiliser ce SA
  containers:
  - name: app
    image: myapp:1.0
```

---

### Token ServiceAccount

Le token JWT est automatiquement monté dans le Pod :

```bash
kubectl exec -it my-app -- sh

# Token monté automatiquement
cat /var/run/secrets/kubernetes.io/serviceaccount/token

# CA certificate
cat /var/run/secrets/kubernetes.io/serviceaccount/ca.crt

# Namespace
cat /var/run/secrets/kubernetes.io/serviceaccount/namespace
```

**Application Java utilisant le token :**
```java
import io.fabric8.kubernetes.client.DefaultKubernetesClient;
import io.fabric8.kubernetes.client.KubernetesClient;

public class K8sClient {
    
    public void listPods() {
        // Client automatiquement configuré avec le token du SA
        try (KubernetesClient client = new DefaultKubernetesClient()) {
            client.pods()
                .inNamespace("prod")
                .list()
                .getItems()
                .forEach(pod -> 
                    System.out.println(pod.getMetadata().getName())
                );
        }
    }
}
```

---

### Disable automount du token

```yaml
apiVersion: v1
kind: ServiceAccount
metadata:
  name: my-app-sa
automountServiceAccountToken: false  # ← Pas de token monté
---
# Ou au niveau Pod
apiVersion: v1
kind: Pod
metadata:
  name: my-app
spec:
  serviceAccountName: my-app-sa
  automountServiceAccountToken: false  # ← Override SA setting
```

**Use case :** Applications qui n'ont pas besoin d'accéder à l'API Kubernetes.

---

## 6.3. Security Contexts (runAsUser, capabilities, etc.)

### Qu'est-ce qu'un Security Context ?

Le **Security Context** définit les privilèges et contrôles d'accès pour un Pod ou Container.

---

### Security Context au niveau Pod

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: secure-pod
spec:
  securityContext:
    runAsUser: 1000      # UID non-root
    runAsGroup: 3000     # GID
    fsGroup: 2000        # Group pour volumes
    runAsNonRoot: true   # Force non-root
    seccompProfile:
      type: RuntimeDefault
  
  containers:
  - name: app
    image: myapp:1.0
```

---

### Security Context au niveau Container

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: secure-container
spec:
  containers:
  - name: app
    image: myapp:1.0
    securityContext:
      runAsUser: 1000
      runAsNonRoot: true
      readOnlyRootFilesystem: true  # Filesystem read-only
      allowPrivilegeEscalation: false
      capabilities:
        drop:
        - ALL  # Drop toutes les capabilities
        add:
        - NET_BIND_SERVICE  # Ajouter une capability spécifique
```

---

### Capabilities Linux

**Capabilities courantes :**

```yaml
capabilities:
  drop:
  - ALL  # Drop toutes
  add:
  - NET_BIND_SERVICE  # Bind ports < 1024
  - CHOWN             # Changer ownership fichiers
  - DAC_OVERRIDE      # Bypass file permissions
  - SETUID            # Changer UID
  - SETGID            # Changer GID
```

**⚠️ Capabilities dangereuses (éviter) :**
- `SYS_ADMIN` : Admin système (très dangereux)
- `SYS_PTRACE` : Tracer autres processus
- `NET_ADMIN` : Configuration réseau
- `SYS_MODULE` : Charger kernel modules

---

### Read-only Root Filesystem

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: readonly-fs
spec:
  containers:
  - name: app
    image: myapp:1.0
    securityContext:
      readOnlyRootFilesystem: true
    
    # Monter volumes pour écriture
    volumeMounts:
    - name: tmp
      mountPath: /tmp
    - name: cache
      mountPath: /app/cache
  
  volumes:
  - name: tmp
    emptyDir: {}
  - name: cache
    emptyDir: {}
```

**Avantages :**
- ✅ Empêche modification du filesystem
- ✅ Protection contre malware
- ✅ Compliance requirements

---

### Privileged Containers

```yaml
# ❌ À ÉVITER : Container privilégié
apiVersion: v1
kind: Pod
metadata:
  name: privileged-pod
spec:
  containers:
  - name: app
    image: myapp:1.0
    securityContext:
      privileged: true  # Accès complet au Node
```

**Conséquences :**
- Accès à tous les devices du Node
- Peut charger kernel modules
- Peut modifier la configuration réseau
- = Root sur le Node

**Use case légitime :** DaemonSets système (monitoring, CNI)

---

### Exemple : Pod sécurisé complet

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: hardened-app
spec:
  # Security Context Pod-level
  securityContext:
    runAsUser: 1000
    runAsGroup: 3000
    fsGroup: 2000
    runAsNonRoot: true
    seccompProfile:
      type: RuntimeDefault
  
  serviceAccountName: restricted-sa
  automountServiceAccountToken: false
  
  containers:
  - name: app
    image: myapp:1.0
    
    # Security Context Container-level
    securityContext:
      readOnlyRootFilesystem: true
      allowPrivilegeEscalation: false
      capabilities:
        drop:
        - ALL
    
    # Resources
    resources:
      requests:
        memory: "256Mi"
        cpu: "250m"
      limits:
        memory: "512Mi"
        cpu: "500m"
    
    # Volumes pour écriture
    volumeMounts:
    - name: tmp
      mountPath: /tmp
    - name: cache
      mountPath: /app/cache
  
  volumes:
  - name: tmp
    emptyDir: {}
  - name: cache
    emptyDir: {}
```

---

## 6.4. Pod Security Standards

### Qu'est-ce que Pod Security Standards ?

**Pod Security Standards** (PSS) définissent 3 niveaux de sécurité pour les Pods :

1. **Privileged** : Unrestricted (tout autorisé)
2. **Baseline** : Minimalement restrictif (empêche les escalations connues)
3. **Restricted** : Hautement restrictif (hardened)

---

### Niveaux de sécurité

**1. Privileged (à éviter en production) :**
```yaml
# Tout est autorisé
# - Containers privilégiés
# - hostNetwork, hostPID, hostIPC
# - Capabilities dangereuses
```

**2. Baseline (minimum) :**
```yaml
# Interdit :
# - privileged: true
# - hostNetwork: true
# - hostPID: true
# - hostIPC: true
# - hostPath volumes
# - Capabilities dangereuses (SYS_ADMIN, NET_ADMIN, etc.)

# Autorisé :
# - runAsUser: 0 (root) ⚠️
# - capabilities: NET_BIND_SERVICE
```

**3. Restricted (recommandé) :**
```yaml
# Tout de Baseline +
# Requis :
# - runAsNonRoot: true
# - Capabilities: ALL dropped
# - readOnlyRootFilesystem: true (recommandé)
# - seccompProfile: RuntimeDefault ou Localhost
```

---

### Appliquer Pod Security Standards

**Au niveau Namespace (Kubernetes 1.25+) :**
```yaml
apiVersion: v1
kind: Namespace
metadata:
  name: prod
  labels:
    pod-security.kubernetes.io/enforce: restricted
    pod-security.kubernetes.io/audit: restricted
    pod-security.kubernetes.io/warn: restricted
```

**Modes :**
- **enforce** : Rejette les Pods non conformes
- **audit** : Log les violations (audit logs)
- **warn** : Affiche warnings (mais crée le Pod)

---

### Pod Security Admission

```yaml
# AdmissionConfiguration
apiVersion: apiserver.config.k8s.io/v1
kind: AdmissionConfiguration
plugins:
- name: PodSecurity
  configuration:
    apiVersion: pod-security.admission.config.k8s.io/v1
    kind: PodSecurityConfiguration
    defaults:
      enforce: "baseline"
      audit: "restricted"
      warn: "restricted"
    exemptions:
      usernames: []
      runtimeClasses: []
      namespaces: ["kube-system"]
```

---

### Migration vers Restricted

**Étape 1 : Audit**
```yaml
metadata:
  labels:
    pod-security.kubernetes.io/audit: restricted
    pod-security.kubernetes.io/warn: restricted
```

```bash
# Identifier les Pods non conformes
kubectl label namespace prod \
  pod-security.kubernetes.io/warn=restricted

# Déployer et voir les warnings
kubectl apply -f deployment.yaml
# Warning: would violate PodSecurity "restricted:latest"
```

**Étape 2 : Corriger les Pods**
```yaml
# Avant (Baseline)
spec:
  containers:
  - name: app
    image: myapp:1.0

# Après (Restricted)
spec:
  securityContext:
    runAsNonRoot: true
    runAsUser: 1000
    seccompProfile:
      type: RuntimeDefault
  
  containers:
  - name: app
    image: myapp:1.0
    securityContext:
      allowPrivilegeEscalation: false
      capabilities:
        drop:
        - ALL
      readOnlyRootFilesystem: true
    volumeMounts:
    - name: tmp
      mountPath: /tmp
  
  volumes:
  - name: tmp
    emptyDir: {}
```

**Étape 3 : Enforce**
```yaml
metadata:
  labels:
    pod-security.kubernetes.io/enforce: restricted
```

---

## 6.5. Network Policies (Security Aspect)

### Network Segmentation

**Zero Trust Architecture :**
```yaml
# 1. Deny all ingress & egress
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
# 2. Allow DNS
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-dns
  namespace: prod
spec:
  podSelector: {}
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
---
# 3. Frontend can call Backend only
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: frontend-to-backend
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

## 📝 Best Practices - Security

### RBAC

✅ **Principle of Least Privilege**
```yaml
# ❌ Éviter
verbs: ["*"]

# ✅ Spécifier exactement
verbs: ["get", "list", "create"]
```

✅ **Separate ServiceAccounts par application**
```yaml
# Pas de SA partagé
serviceAccountName: app-specific-sa
```

✅ **Audit régulier des permissions**
```bash
kubectl auth can-i --list --as=user@example.com
```

---

### Security Contexts

✅ **Always run as non-root**
```yaml
runAsNonRoot: true
runAsUser: 1000
```

✅ **Drop all capabilities**
```yaml
capabilities:
  drop: [ALL]
```

✅ **Read-only filesystem**
```yaml
readOnlyRootFilesystem: true
```

✅ **Disable privilege escalation**
```yaml
allowPrivilegeEscalation: false
```

---

### Pod Security

✅ **Enforce Restricted standard en production**
```yaml
pod-security.kubernetes.io/enforce: restricted
```

✅ **Seccomp profile**
```yaml
seccompProfile:
  type: RuntimeDefault
```

---

### Network Policies

✅ **Default deny all**
```yaml
podSelector: {}
policyTypes: [Ingress, Egress]
```

✅ **Allow DNS explicitly**

✅ **Segment par tiers**
```
Internet → Frontend → Backend → Database
         ❌ Direct access blocked
```

---

## 🎯 Checklist Security Production

```yaml
security-checklist:
  rbac:
    ✓ ServiceAccount dédié par app
    ✓ Least privilege principle
    ✓ Pas de verbs: ["*"]
    ✓ Regular audit
  
  pod-security:
    ✓ runAsNonRoot: true
    ✓ readOnlyRootFilesystem: true
    ✓ capabilities: drop: [ALL]
    ✓ allowPrivilegeEscalation: false
    ✓ seccompProfile: RuntimeDefault
  
  pod-security-standards:
    ✓ Namespace enforce: restricted
    ✓ Migration plan documented
    ✓ Exemptions justified
  
  network:
    ✓ NetworkPolicies enabled
    ✓ Default deny all
    ✓ Egress restricted
    ✓ DNS allowed explicitly
  
  secrets:
    ✓ External Secrets Operator
    ✓ Encryption at rest
    ✓ Rotation policy
    ✓ RBAC strict on Secrets
  
  images:
    ✓ Image scanning (Trivy, Snyk)
    ✓ Private registry
    ✓ Image pull secrets
    ✓ No :latest tags
```

---

# Chapitre 6 : Questions/Réponses d'Entretien - Security

## 🔐 RBAC

### Q1 : Expliquez la différence entre Role et ClusterRole, et quand utiliser chacun.

**Réponse :**

**Role (namespace-scoped) :**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: pod-manager
  namespace: dev  # ← Limité à ce namespace
rules:
- apiGroups: [""]
  resources: ["pods"]
  verbs: ["get", "list", "create", "delete"]
```

**Caractéristiques :**
- Scope : **1 namespace uniquement**
- Resources : Uniquement namespacées (Pods, Services, Deployments, etc.)
- Use case : Permissions pour une équipe/app dans un namespace spécifique

**ClusterRole (cluster-wide) :**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: ClusterRole
metadata:
  name: node-reader  # Pas de namespace
rules:
- apiGroups: [""]
  resources: ["nodes"]  # Ressource non-namespacée
  verbs: ["get", "list"]
```

**Caractéristiques :**
- Scope : **Cluster entier**
- Resources : Namespacées ET non-namespacées
- Use case : Admin, monitoring global, ressources cluster-wide

---

**Comparaison détaillée :**

| Aspect | Role | ClusterRole |
|--------|------|-------------|
| **Scope** | 1 namespace | Cluster entier |
| **Metadata** | Avec namespace | Sans namespace |
| **Resources** | Pods, Services, ConfigMaps | Nodes, PVs, Namespaces |
| **Binding** | RoleBinding | ClusterRoleBinding |
| **Use case** | Équipe dev | Cluster admin |

---

**Quand utiliser quoi ?**

**✅ Role - Exemples :**
```yaml
# 1. Équipe dev qui gère ses apps dans namespace "prod"
kind: Role
metadata:
  namespace: prod
rules:
- apiGroups: ["apps"]
  resources: ["deployments"]
  verbs: ["get", "list", "create", "update"]

# 2. CI/CD qui déploie dans namespace "staging"
kind: Role
metadata:
  namespace: staging
rules:
- apiGroups: [""]
  resources: ["pods", "services"]
  verbs: ["*"]
```

**✅ ClusterRole - Exemples :**
```yaml
# 1. Admin cluster qui gère les Nodes
kind: ClusterRole
rules:
- apiGroups: [""]
  resources: ["nodes"]
  verbs: ["get", "list", "update"]

# 2. Monitoring qui lit dans tous les namespaces
kind: ClusterRole
rules:
- apiGroups: [""]
  resources: ["pods", "services"]
  verbs: ["get", "list", "watch"]
# Utilisé dans TOUS les namespaces

# 3. Gestion des PersistentVolumes (non-namespacé)
kind: ClusterRole
rules:
- apiGroups: [""]
  resources: ["persistentvolumes"]
  verbs: ["get", "list", "create", "delete"]
```

---

**Pattern avancé : ClusterRole + RoleBinding**

```yaml
# ClusterRole (réutilisable)
apiVersion: rbac.authorization.k8s.io/v1
kind: ClusterRole
metadata:
  name: pod-reader
rules:
- apiGroups: [""]
  resources: ["pods"]
  verbs: ["get", "list"]
---
# RoleBinding dans namespace "dev"
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: dev-pod-reader
  namespace: dev  # ← Limite au namespace dev
subjects:
- kind: User
  name: john@example.com
roleRef:
  kind: ClusterRole  # ← Référence un ClusterRole
  name: pod-reader
---
# RoleBinding dans namespace "prod"
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: prod-pod-reader
  namespace: prod  # ← Limite au namespace prod
subjects:
- kind: User
  name: john@example.com
roleRef:
  kind: ClusterRole  # Même ClusterRole, namespace différent
  name: pod-reader
```

**Avantage :** 1 ClusterRole réutilisé avec différents RoleBindings = DRY

---

**Résumé - Arbre de décision :**
```
Permissions nécessaires sur quoi ?
│
├─ Ressources non-namespacées (Nodes, PVs, Namespaces)
│   → ClusterRole + ClusterRoleBinding
│
├─ Plusieurs namespaces
│   → ClusterRole + RoleBinding (par namespace)
│
└─ 1 namespace uniquement
    → Role + RoleBinding
```

---

### Q2 : Comment donnez-vous à une application Java/Spring Boot les permissions pour lire les ConfigMaps dans son namespace ?

**Réponse :**

**Étapes complètes :**

**1. Créer un ServiceAccount**
```yaml
apiVersion: v1
kind: ServiceAccount
metadata:
  name: spring-app-sa
  namespace: prod
```

**2. Créer un Role avec permissions ConfigMap**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: configmap-reader
  namespace: prod
rules:
- apiGroups: [""]  # core API group
  resources: ["configmaps"]
  verbs: ["get", "list", "watch"]
```

**3. Créer un RoleBinding**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: spring-app-configmap-reader
  namespace: prod
subjects:
- kind: ServiceAccount
  name: spring-app-sa
  namespace: prod
roleRef:
  kind: Role
  name: configmap-reader
  apiGroup: rbac.authorization.k8s.io
```

**4. Utiliser le ServiceAccount dans le Deployment**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: spring-app
  namespace: prod
spec:
  replicas: 3
  selector:
    matchLabels:
      app: spring-app
  template:
    metadata:
      labels:
        app: spring-app
    spec:
      serviceAccountName: spring-app-sa  # ← Important !
      containers:
      - name: app
        image: myapp/spring-boot:1.0
        ports:
        - containerPort: 8080
```

**5. Code Java pour lire les ConfigMaps**
```java
import io.fabric8.kubernetes.client.DefaultKubernetesClient;
import io.fabric8.kubernetes.client.KubernetesClient;
import io.fabric8.kubernetes.api.model.ConfigMap;

@Service
public class ConfigMapService {
    
    private final KubernetesClient client;
    
    public ConfigMapService() {
        // Client automatiquement configuré avec le token du ServiceAccount
        this.client = new DefaultKubernetesClient();
    }
    
    public String readConfigMap(String configMapName, String key) {
        try {
            ConfigMap configMap = client.configMaps()
                .inNamespace("prod")  // Namespace actuel
                .withName(configMapName)
                .get();
            
            if (configMap != null && configMap.getData() != null) {
                return configMap.getData().get(key);
            }
            return null;
        } catch (Exception e) {
            logger.error("Failed to read ConfigMap: {}", configMapName, e);
            throw e;
        }
    }
    
    @PostConstruct
    public void loadConfiguration() {
        String dbHost = readConfigMap("app-config", "database.host");
        String dbPort = readConfigMap("app-config", "database.port");
        
        logger.info("Database config loaded: {}:{}", dbHost, dbPort);
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

---

**Vérification :**
```bash
# Appliquer toutes les ressources
kubectl apply -f serviceaccount.yaml
kubectl apply -f role.yaml
kubectl apply -f rolebinding.yaml
kubectl apply -f deployment.yaml

# Vérifier que le SA peut lire les ConfigMaps
kubectl auth can-i get configmaps \
  --as=system:serviceaccount:prod:spring-app-sa \
  --namespace=prod
# yes

# Vérifier que le SA ne peut PAS créer de ConfigMaps
kubectl auth can-i create configmaps \
  --as=system:serviceaccount:prod:spring-app-sa \
  --namespace=prod
# no

# Tester depuis le Pod
kubectl exec -it spring-app-xxx -- sh
curl -k -H "Authorization: Bearer $(cat /var/run/secrets/kubernetes.io/serviceaccount/token)" \
  https://kubernetes.default.svc/api/v1/namespaces/prod/configmaps
```

---

**Best Practices :**

✅ **Principle of Least Privilege**
```yaml
# ✅ Bon : Seulement "get", "list"
verbs: ["get", "list"]

# ❌ Mauvais : Trop de permissions
verbs: ["*"]
```

✅ **ServiceAccount dédié par application**
```yaml
# Pas de réutilisation du SA "default"
serviceAccountName: spring-app-sa
```

✅ **automountServiceAccountToken: false si non nécessaire**
```yaml
spec:
  serviceAccountName: spring-app-sa
  automountServiceAccountToken: false  # Si app n'utilise pas l'API K8s
```

---

### Q3 : Comment auditez-vous et testez-vous les permissions RBAC dans votre cluster ?

**Réponse :**

**1. Tester les permissions avec kubectl auth can-i**

```bash
# Vérifier si l'utilisateur actuel peut faire une action
kubectl auth can-i create deployments --namespace=prod
# yes

kubectl auth can-i delete secrets --namespace=prod
# no

# Tester pour un autre utilisateur
kubectl auth can-i list pods \
  --as=john@example.com \
  --namespace=dev
# yes

# Tester pour un ServiceAccount
kubectl auth can-i create deployments \
  --as=system:serviceaccount:prod:gitlab-ci \
  --namespace=prod
# yes

# Lister toutes les permissions d'un utilisateur
kubectl auth can-i --list \
  --as=john@example.com \
  --namespace=dev
```

---

**2. Audit des RoleBindings et ClusterRoleBindings**

```bash
# Lister tous les RoleBindings dans un namespace
kubectl get rolebindings -n prod

# Voir les détails d'un RoleBinding
kubectl describe rolebinding developer-binding -n prod

# Lister tous les ClusterRoleBindings
kubectl get clusterrolebindings

# Qui a accès au Role "cluster-admin" ?
kubectl get clusterrolebindings -o json | \
  jq '.items[] | select(.roleRef.name=="cluster-admin") | {name: .metadata.name, subjects: .subjects}'
```

---

**3. Script d'audit complet**

```bash
#!/bin/bash
# rbac-audit.sh

echo "=== Users with cluster-admin ==="
kubectl get clusterrolebindings -o json | \
  jq -r '.items[] | select(.roleRef.name=="cluster-admin") | 
    .subjects[]? | "\(.kind): \(.name)"' | sort -u

echo "\n=== ServiceAccounts with high privileges ==="
kubectl get clusterrolebindings -o json | \
  jq -r '.items[] | select(.roleRef.name | test("admin|edit")) | 
    .subjects[]? | select(.kind=="ServiceAccount") | 
    "\(.namespace)/\(.name) -> \(.roleRef.name)"'

echo "\n=== Bindings in production namespace ==="
kubectl get rolebindings -n prod -o wide

echo "\n=== Unused ServiceAccounts ==="
# ServiceAccounts sans Pods
for sa in $(kubectl get sa -n prod -o jsonpath='{.items[*].metadata.name}'); do
  pods=$(kubectl get pods -n prod --field-selector spec.serviceAccountName=$sa --no-headers 2>/dev/null | wc -l)
  if [ $pods -eq 0 ]; then
    echo "Unused: $sa"
  fi
done
```

---

**4. Audit Logging (API Server)**

```yaml
# /etc/kubernetes/audit-policy.yaml
apiVersion: audit.k8s.io/v1
kind: Policy
rules:
# Log accès aux Secrets
- level: RequestResponse
  resources:
  - group: ""
    resources: ["secrets"]
  verbs: ["get", "list", "watch"]

# Log modifications RBAC
- level: RequestResponse
  resources:
  - group: "rbac.authorization.k8s.io"
    resources: ["roles", "rolebindings", "clusterroles", "clusterrolebindings"]
  verbs: ["create", "update", "delete"]

# Log échecs d'authentification
- level: Request
  omitStages:
  - RequestReceived
  userGroups:
  - system:unauthenticated
```

**Activer dans kube-apiserver :**
```yaml
# /etc/kubernetes/manifests/kube-apiserver.yaml
spec:
  containers:
  - command:
    - kube-apiserver
    - --audit-policy-file=/etc/kubernetes/audit-policy.yaml
    - --audit-log-path=/var/log/kubernetes/audit.log
    - --audit-log-maxage=30
    - --audit-log-maxbackup=10
```

**Analyser les logs :**
```bash
# Qui a accédé aux Secrets récemment ?
jq 'select(.objectRef.resource=="secrets" and .verb=="get")' \
  /var/log/kubernetes/audit.log

# Modifications RBAC récentes
jq 'select(.objectRef.resource | test("role|binding"))' \
  /var/log/kubernetes/audit.log
```

---

**5. Tools d'audit tiers**

**RBAC Lookup :**
```bash
# Installer
kubectl krew install rbac-lookup

# Qui peut créer des Pods ?
kubectl rbac-lookup pods --verb=create

# Quelles permissions a cet utilisateur ?
kubectl rbac-lookup --kind=user --name=john@example.com
```

**kubectl-who-can :**
```bash
# Installer
kubectl krew install who-can

# Qui peut supprimer des Pods dans prod ?
kubectl who-can delete pods -n prod

# Qui peut lire les Secrets ?
kubectl who-can get secrets --all-namespaces
```

**Kubeaudit :**
```bash
# Audit de sécurité complet
kubeaudit all -f deployment.yaml

# Vérifier RBAC
kubeaudit rbac
```

---

**6. Monitoring continu avec Prometheus**

```yaml
# Alert : Accès aux Secrets par un SA non autorisé
- alert: UnauthorizedSecretAccess
  expr: |
    rate(apiserver_audit_event_total{
      objectRef_resource="secrets",
      verb="get",
      user_username!~"system:serviceaccount:prod:allowed-sa"
    }[5m]) > 0
  annotations:
    summary: "Unauthorized Secret access detected"
```

---

**Checklist d'audit régulier :**

```yaml
rbac-audit-checklist:
  monthly:
    ✓ Lister tous les cluster-admin
    ✓ Identifier ServiceAccounts inutilisés
    ✓ Vérifier permissions trop larges (verbs: ["*"])
    ✓ Audit logs review (accès Secrets)
  
  quarterly:
    ✓ Review de tous les ClusterRoleBindings
    ✓ Nettoyage des Roles/RoleBindings obsolètes
    ✓ Test d'intrusion (penetration testing)
  
  continuous:
    ✓ Prometheus alerts sur accès suspects
    ✓ Audit logging activé
    ✓ Tools automatisés (kubeaudit)
```

---

## 🔑 ServiceAccounts & Security Contexts

### Q4 : Quelle est la différence entre un User et un ServiceAccount ?

**Réponse :**

**User (Humain) :**
- Utilisé par les **personnes** (kubectl)
- Géré **en dehors** de Kubernetes (LDAP, OIDC, certificats)
- Pas d'objet User dans Kubernetes
- Authentification : Certificats X.509, OIDC tokens, webhooks

**ServiceAccount (Pod/Application) :**
- Utilisé par les **Pods/Applications**
- Géré **par Kubernetes** (objet API)
- Token JWT automatiquement créé
- Authentification : Token monté dans `/var/run/secrets/`

---

**Comparaison détaillée :**

| Aspect | User | ServiceAccount |
|--------|------|----------------|
| **Pour** | Humains | Pods/Apps |
| **Géré par** | External (OIDC, LDAP) | Kubernetes |
| **Objet K8s** | Non | Oui (`kubectl get sa`) |
| **Token** | External (kubeconfig) | JWT automatique |
| **Scope** | Multi-namespaces | 1 namespace |
| **Use case** | kubectl, admin | App calling K8s API |

---

**User - Exemple (OIDC) :**
```yaml
# kubeconfig
users:
- name: john@example.com
  user:
    auth-provider:
      name: oidc
      config:
        idp-issuer-url: https://accounts.google.com
        client-id: kubernetes
```

```bash
# kubectl utilise ce User
kubectl get pods --user=john@example.com
```

---

**ServiceAccount - Exemple :**
```yaml
# Créer ServiceAccount
apiVersion: v1
kind: ServiceAccount
metadata:
  name: app-sa
  namespace: prod
---
# Utiliser dans Pod
apiVersion: v1
kind: Pod
metadata:
  name: my-app
spec:
  serviceAccountName: app-sa
  containers:
  - name: app
    image: myapp:1.0
```

**Token automatiquement monté :**
```bash
kubectl exec my-app -- ls /var/run/secrets/kubernetes.io/serviceaccount/
# token
# ca.crt
# namespace

kubectl exec my-app -- cat /var/run/secrets/kubernetes.io/serviceaccount/token
# eyJhbGciOiJSUzI1NiIsImtpZCI6IjEyMyJ9...
```

---

**RoleBinding avec User vs ServiceAccount :**

**User :**
```yaml
subjects:
- kind: User
  name: john@example.com
  apiGroup: rbac.authorization.k8s.io
```

**ServiceAccount :**
```yaml
subjects:
- kind: ServiceAccount
  name: app-sa
  namespace: prod
```

---

**Pourquoi cette séparation ?**

**1. Lifecycle différent**
- User : Géré par RH (embauche/départ)
- ServiceAccount : Géré par K8s (app deploy/undeploy)

**2. Authentification différente**
- User : External IdP (Google, Okta, AD)
- ServiceAccount : Token K8s natif

**3. Permissions différentes**
- User : Large (multi-namespaces, admin)
- ServiceAccount : Étroites (1 namespace, app-specific)

**4. Audit différent**
- User : Qui a fait quoi (compliance)
- ServiceAccount : Quelle app a fait quoi (troubleshooting)

---

**Use cases :**

**User :**
```bash
# Admin qui gère le cluster
kubectl delete deployment old-app --user=admin@example.com

# Developer qui debug
kubectl logs my-pod --user=john@example.com
```

**ServiceAccount :**
```java
// Application Spring Boot qui scale elle-même
@Service
public class AutoScaler {
    private final KubernetesClient client = new DefaultKubernetesClient();
    
    public void scaleUp(String deploymentName) {
        client.apps().deployments()
            .inNamespace("prod")
            .withName(deploymentName)
            .scale(5);  // Utilise le token du ServiceAccount
    }
}
```

---

### Q5 : Comment sécurisez-vous un Pod pour qu'il tourne avec le minimum de privilèges ?

**Réponse :**

**Pod sécurisé complet (Best practices) :**

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: hardened-app
  namespace: prod
spec:
  # 1. ServiceAccount dédié
  serviceAccountName: app-sa
  automountServiceAccountToken: false  # Si pas besoin de K8s API
  
  # 2. Security Context Pod-level
  securityContext:
    runAsNonRoot: true      # Force non-root
    runAsUser: 1000         # UID non-root
    runAsGroup: 3000        # GID
    fsGroup: 2000           # Group pour volumes
    seccompProfile:
      type: RuntimeDefault  # Seccomp profile
  
  containers:
  - name: app
    image: myapp:1.0
    
    # 3. Security Context Container-level
    securityContext:
      readOnlyRootFilesystem: true          # Filesystem read-only
      allowPrivilegeEscalation: false       # Pas d'escalation
      capabilities:
        drop:
        - ALL  # Drop toutes les capabilities
      # Pas de capabilities ajoutées (sauf si nécessaire)
    
    # 4. Resources limits
    resources:
      requests:
        memory: "256Mi"
        cpu: "250m"
      limits:
        memory: "512Mi"
        cpu: "500m"
    
    # 5. Volumes pour écriture (filesystem read-only)
    volumeMounts:
    - name: tmp
      mountPath: /tmp
    - name: cache
      mountPath: /app/cache
    - name: logs
      mountPath: /app/logs
  
  volumes:
  - name: tmp
    emptyDir: {}
  - name: cache
    emptyDir: {}
  - name: logs
    emptyDir: {}
```

---

**Explications détaillées :**

**1. runAsNonRoot: true**
```yaml
runAsNonRoot: true
runAsUser: 1000
```

**Pourquoi :**
- Root (UID 0) a trop de privilèges
- Compromission = accès complet au container
- Best practice : toujours non-root

**Dockerfile correspondant :**
```dockerfile
FROM openjdk:17-slim

# Créer user non-root
RUN groupadd -r appuser && useradd -r -g appuser -u 1000 appuser

# Changer ownership
COPY --chown=appuser:appuser target/app.jar /app/app.jar

# Switch vers non-root
USER 1000

ENTRYPOINT ["java", "-jar", "/app/app.jar"]
```

---

**2. readOnlyRootFilesystem: true**
```yaml
readOnlyRootFilesystem: true
```

**Pourquoi :**
- Empêche modification du filesystem
- Protection contre malware qui écrit sur disque
- Compliance (immutable infrastructure)

**Conséquence :**
- Application ne peut PAS écrire dans `/var`, `/tmp`, etc.
- Solution : Monter des volumes emptyDir

```yaml
volumeMounts:
- name: tmp
  mountPath: /tmp  # Java temp files
- name: cache
  mountPath: /app/cache  # Application cache
```

---

**3. allowPrivilegeEscalation: false**
```yaml
allowPrivilegeEscalation: false
```

**Pourquoi :**
- Empêche un processus d'obtenir plus de privilèges que son parent
- Protection contre exploits qui tentent d'escalader

---

**4. capabilities: drop ALL**
```yaml
capabilities:
  drop:
  - ALL
```

**Pourquoi :**
- Linux capabilities = découpage des privilèges root
- Par défaut, containers ont plusieurs capabilities
- Best practice : drop ALL sauf si nécessaire

**Si besoin d'une capability spécifique :**
```yaml
capabilities:
  drop:
  - ALL
  add:
  - NET_BIND_SERVICE  # Pour bind ports < 1024
```

---

**5. seccompProfile: RuntimeDefault**
```yaml
seccompProfile:
  type: RuntimeDefault
```

**Pourquoi :**
- Seccomp = filtre des syscalls Linux
- RuntimeDefault = profil sécurisé par défaut
- Limite les syscalls disponibles (réduction surface d'attaque)

---

**6. No automount si pas nécessaire**
```yaml
automountServiceAccountToken: false
```

**Pourquoi :**
- Token K8s monté par défaut = risque si compromission
- Si app n'utilise pas l'API K8s → désactiver

---

**Test de sécurité :**

```bash
# Vérifier que le Pod tourne en non-root
kubectl exec hardened-app -- id
# uid=1000(appuser) gid=3000 groups=2000,3000

# Vérifier read-only filesystem
kubectl exec hardened-app -- touch /test
# touch: cannot touch '/test': Read-only file system ✅

# Mais peut écrire dans /tmp
kubectl exec hardened-app -- touch /tmp/test
# ✅ Success

# Vérifier les capabilities
kubectl exec hardened-app -- grep Cap /proc/1/status
# CapPrm: 0000000000000000  ← Aucune capability ✅
```

---

**Comparaison Avant/Après :**

**❌ Avant (Non sécurisé) :**
```yaml
spec:
  containers:
  - name: app
    image: myapp:1.0
    # Tourne en root par défaut
    # Filesystem writable
    # Toutes les capabilities
    # Peut escalader privilèges
```

**✅ Après (Sécurisé) :**
```yaml
spec:
  securityContext:
    runAsNonRoot: true
    runAsUser: 1000
  containers:
  - name: app
    image: myapp:1.0
    securityContext:
      readOnlyRootFilesystem: true
      allowPrivilegeEscalation: false
      capabilities:
        drop: [ALL]
```

---

**Migration progressive :**

**Étape 1 : runAsNonRoot**
```yaml
securityContext:
  runAsNonRoot: true
  runAsUser: 1000
```

**Étape 2 : Drop capabilities**
```yaml
capabilities:
  drop: [ALL]
```

**Étape 3 : readOnlyRootFilesystem**
```yaml
readOnlyRootFilesystem: true
# + Ajouter volumes emptyDir
```

**Étape 4 : Seccomp & reste**
```yaml
seccompProfile:
  type: RuntimeDefault
allowPrivilegeEscalation: false
```

---

## 🛡️ Pod Security Standards

### Q6 : Expliquez les 3 niveaux de Pod Security Standards et leurs différences.

**Réponse :**

**Pod Security Standards (PSS)** définissent 3 profils de sécurité :

---

**1. Privileged (unrestricted)**

**Autorisations :**
- ✅ Tout est autorisé
- ✅ privileged: true
- ✅ hostNetwork, hostPID, hostIPC
- ✅ hostPath volumes
- ✅ Toutes les capabilities
- ✅ Root user (runAsUser: 0)

**Use case :**
- System daemons (CNI, monitoring)
- Infrastructure critique

**⚠️ Danger :**
- Container = Root sur le Node
- Compromission = Compromission du Node

**Exemple :**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: privileged-pod
spec:
  hostNetwork: true  # Réseau du Node
  hostPID: true      # PID namespace du Node
  containers:
  - name: app
    image: myapp:1.0
    securityContext:
      privileged: true  # Tous les privilèges
```

---

**2. Baseline (minimalement restrictif)**

**Interdictions :**
- ❌ privileged: true
- ❌ hostNetwork: true
- ❌ hostPID: true
- ❌ hostIPC: true
- ❌ hostPath volumes
- ❌ Capabilities dangereuses (SYS_ADMIN, NET_ADMIN, etc.)
- ❌ allowPrivilegeEscalation: true (sauf explicite)

**Autorisations :**
- ✅ runAsUser: 0 (root) ⚠️
- ✅ Capabilities "safe" (NET_BIND_SERVICE, CHOWN, etc.)
- ✅ seccompProfile non requis

**Use case :**
- Applications qui ont besoin de root
- Migration depuis environnements non sécurisés
- Applications legacy

**Exemple :**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: baseline-pod
spec:
  containers:
  - name: app
    image: myapp:1.0
    # Peut tourner en root (mais pas privileged)
    securityContext:
      capabilities:
        add:
        - NET_BIND_SERVICE  # Pour bind port 80
```

---

**3. Restricted (hautement restrictif)**

**Tout de Baseline +**

**Requis :**
- ✅ runAsNonRoot: true (obligatoire)
- ✅ capabilities: drop [ALL] (obligatoire)
- ✅ seccompProfile: RuntimeDefault ou Localhost (obligatoire)
- ✅ allowPrivilegeEscalation: false

**Recommandé :**
- ✅ readOnlyRootFilesystem: true

**Use case :**
- **Production** (recommandé)
- Applications stateless
- Microservices

**Exemple :**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: restricted-pod
spec:
  securityContext:
    runAsNonRoot: true
    runAsUser: 1000
    seccompProfile:
      type: RuntimeDefault
  containers:
  - name: app
    image: myapp:1.0
    securityContext:
      allowPrivilegeEscalation: false
      readOnlyRootFilesystem: true
      capabilities:
        drop:
        - ALL
    volumeMounts:
    - name: tmp
      mountPath: /tmp
  volumes:
  - name: tmp
    emptyDir: {}
```

---

**Comparaison tableau :**

| Feature | Privileged | Baseline | Restricted |
|---------|-----------|----------|------------|
| **privileged: true** | ✅ | ❌ | ❌ |
| **hostNetwork** | ✅ | ❌ | ❌ |
| **hostPath** | ✅ | ❌ | ❌ |
| **runAsUser: 0** | ✅ | ✅ | ❌ |
| **capabilities: ALL** | ✅ | ⚠️ Limitées | ❌ |
| **runAsNonRoot** | ❌ | ❌ | ✅ Requis |
| **seccompProfile** | ❌ | ❌ | ✅ Requis |
| **Production** | ❌ | ⚠️ | ✅ |

---

**Application au niveau Namespace :**

```yaml
apiVersion: v1
kind: Namespace
metadata:
  name: prod
  labels:
    # Mode enforce : Rejette les Pods non conformes
    pod-security.kubernetes.io/enforce: restricted
    
    # Mode audit : Log les violations (audit logs)
    pod-security.kubernetes.io/audit: restricted
    
    # Mode warn : Affiche warnings
    pod-security.kubernetes.io/warn: restricted
```

**Test :**
```bash
# Tenter de créer un Pod Baseline dans namespace Restricted
kubectl apply -f baseline-pod.yaml -n prod
# Error: pods "baseline-pod" is forbidden: violates PodSecurity "restricted:latest"
```

---

**Migration Progressive :**

**Phase 1 : Audit & Warn**
```yaml
metadata:
  labels:
    pod-security.kubernetes.io/audit: restricted
    pod-security.kubernetes.io/warn: restricted
# Pas encore enforce
```

**Phase 2 : Fix Pods**
```bash
# Identifier Pods non conformes
kubectl label namespace dev pod-security.kubernetes.io/warn=restricted
kubectl apply -f deployment.yaml
# Warning: would violate PodSecurity "restricted"

# Corriger les Pods
```

**Phase 3 : Enforce**
```yaml
metadata:
  labels:
    pod-security.kubernetes.io/enforce: restricted
```

---

**Exemptions (si nécessaire) :**
```yaml
# AdmissionConfiguration
apiVersion: apiserver.config.k8s.io/v1
kind: AdmissionConfiguration
plugins:
- name: PodSecurity
  configuration:
    apiVersion: pod-security.admission.config.k8s.io/v1
    kind: PodSecurityConfiguration
    defaults:
      enforce: "restricted"
    exemptions:
      namespaces: ["kube-system"]  # System daemons
      runtimeClasses: []
      usernames: []
```

---

**Recommandation :**
- **Production** : Restricted
- **Staging** : Baseline → Restricted (migration)
- **Dev** : Baseline (flexibilité)
- **Kube-system** : Privileged (system components)

---

## 🎯 Questions Situationnelles

### Q7 : Une application a besoin d'accéder à l'API Kubernetes pour lister les Pods. Comment configurez-vous cela de manière sécurisée ?

**Réponse :**

**Approche complète et sécurisée :**

**1. Créer un ServiceAccount dédié**
```yaml
apiVersion: v1
kind: ServiceAccount
metadata:
  name: pod-lister-sa
  namespace: prod
automountServiceAccountToken: true  # Nécessaire pour cette app
```

**2. Créer un Role avec permissions minimales**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: pod-reader
  namespace: prod
rules:
# Uniquement "list" et "get" sur Pods
- apiGroups: [""]
  resources: ["pods"]
  verbs: ["list", "get"]  # ❌ Pas de "create", "delete", "update"

# Optionnel : Lire les logs
- apiGroups: [""]
  resources: ["pods/log"]
  verbs: ["get"]
```

**3. Créer un RoleBinding**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: pod-lister-binding
  namespace: prod
subjects:
- kind: ServiceAccount
  name: pod-lister-sa
  namespace: prod
roleRef:
  kind: Role
  name: pod-reader
  apiGroup: rbac.authorization.k8s.io
```

**4. Deployment sécurisé**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: pod-lister-app
  namespace: prod
spec:
  replicas: 1
  selector:
    matchLabels:
      app: pod-lister
  template:
    metadata:
      labels:
        app: pod-lister
    spec:
      # ServiceAccount dédié
      serviceAccountName: pod-lister-sa
      
      # Security Context
      securityContext:
        runAsNonRoot: true
        runAsUser: 1000
        fsGroup: 2000
        seccompProfile:
          type: RuntimeDefault
      
      containers:
      - name: app
        image: myapp/pod-lister:1.0
        
        securityContext:
          readOnlyRootFilesystem: true
          allowPrivilegeEscalation: false
          capabilities:
            drop:
            - ALL
        
        resources:
          requests:
            memory: "128Mi"
            cpu: "100m"
          limits:
            memory: "256Mi"
            cpu: "200m"
        
        volumeMounts:
        - name: tmp
          mountPath: /tmp
      
      volumes:
      - name: tmp
        emptyDir: {}
```

**5. Code Java Spring Boot**
```java
import io.fabric8.kubernetes.client.DefaultKubernetesClient;
import io.fabric8.kubernetes.client.KubernetesClient;
import io.fabric8.kubernetes.api.model.Pod;

@Service
public class PodMonitorService {
    
    private final KubernetesClient client;
    
    public PodMonitorService() {
        // Client automatiquement configuré avec le token du ServiceAccount
        this.client = new DefaultKubernetesClient();
    }
    
    @Scheduled(fixedRate = 60000)  // Toutes les minutes
    public void listPods() {
        try {
            List<Pod> pods = client.pods()
                .inNamespace("prod")
                .list()
                .getItems();
            
            logger.info("Found {} pods in namespace prod", pods.size());
            
            pods.forEach(pod -> {
                String name = pod.getMetadata().getName();
                String phase = pod.getStatus().getPhase();
                logger.debug("Pod: {} - Status: {}", name, phase);
            });
            
        } catch (Exception e) {
            logger.error("Failed to list pods", e);
        }
    }
    
    @PreDestroy
    public void cleanup() {
        if (client != null) {
            client.close();
        }
    }
}
```

**Dépendances Maven :**
```xml
<dependency>
    <groupId>io.fabric8</groupId>
    <artifactId>kubernetes-client</artifactId>
    <version>6.9.0</version>
</dependency>
```

---

**Vérification de sécurité :**

```bash
# 1. Vérifier que le SA peut lister les Pods
kubectl auth can-i list pods \
  --as=system:serviceaccount:prod:pod-lister-sa \
  --namespace=prod
# yes

# 2. Vérifier que le SA ne peut PAS supprimer de Pods
kubectl auth can-i delete pods \
  --as=system:serviceaccount:prod:pod-lister-sa \
  --namespace=prod
# no

# 3. Vérifier que le SA ne peut PAS lister les Secrets
kubectl auth can-i list secrets \
  --as=system:serviceaccount:prod:pod-lister-sa \
  --namespace=prod
# no

# 4. Vérifier dans le Pod
kubectl exec -it pod-lister-app-xxx -- sh

# Token présent
ls /var/run/secrets/kubernetes.io/serviceaccount/
# token ca.crt namespace

# Tester l'API
curl -k -H "Authorization: Bearer $(cat /var/run/secrets/kubernetes.io/serviceaccount/token)" \
  https://kubernetes.default.svc/api/v1/namespaces/prod/pods
# {"kind":"PodList", ...} ✅

# Tester accès non autorisé
curl -k -H "Authorization: Bearer $(cat /var/run/secrets/kubernetes.io/serviceaccount/token)" \
  https://kubernetes.default.svc/api/v1/namespaces/prod/secrets
# {"kind":"Status","status":"Failure","reason":"Forbidden"} ✅
```

---

**Best Practices appliquées :**

✅ **ServiceAccount dédié** (pas de réutilisation de "default")
✅ **Least Privilege** (uniquement list/get sur Pods)
✅ **Namespace-scoped** (Role, pas ClusterRole)
✅ **Security Context complet** (runAsNonRoot, capabilities drop, etc.)
✅ **Resources limits** (protection contre resource exhaustion)
✅ **readOnlyRootFilesystem** (immutable infrastructure)

---

**Monitoring & Alerting :**
```yaml
# Prometheus alert : Accès API suspects
- alert: UnexpectedAPIAccess
  expr: |
    rate(apiserver_audit_event_total{
      user_username="system:serviceaccount:prod:pod-lister-sa",
      verb!~"list|get",
      objectRef_resource="pods"
    }[5m]) > 0
  annotations:
    summary: "ServiceAccount pod-lister-sa attempted unauthorized action"
```

---

### Q8 : Comment implémentez-vous le principe de "least privilege" dans un cluster Kubernetes ?

**Réponse :**

**Principe : Accorder uniquement les permissions strictement nécessaires, rien de plus.**

---

**1. RBAC : Permissions granulaires**

**❌ Mauvais - Trop de permissions :**
```yaml
rules:
- apiGroups: ["*"]
  resources: ["*"]
  verbs: ["*"]  # Tout autorisé !
```

**✅ Bon - Granulaire :**
```yaml
rules:
# Seulement ce qui est nécessaire
- apiGroups: [""]
  resources: ["pods"]
  verbs: ["get", "list"]  # Pas de create, delete, update

# Pas d'accès aux Secrets
# Pas d'accès aux autres namespaces
```

---

**2. ServiceAccount par application**

**❌ Mauvais - ServiceAccount partagé :**
```yaml
# Toutes les apps utilisent "default"
spec:
  serviceAccountName: default
```

**✅ Bon - ServiceAccount dédié :**
```yaml
# App 1
serviceAccountName: app1-sa

# App 2
serviceAccountName: app2-sa

# App 3
serviceAccountName: app3-sa
```

**Avantage :** Isolation, audit granulaire, revoke facile

---

**3. Namespace segmentation**

```yaml
# Équipes séparées
namespaces:
- name: team-frontend
- name: team-backend
- name: team-data

# Chaque équipe a son namespace avec ses Roles
---
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: frontend-team
  namespace: team-frontend
subjects:
- kind: Group
  name: frontend-developers@example.com
roleRef:
  kind: Role
  name: developer  # Peut gérer ses apps
---
# Pas d'accès cross-namespace par défaut
```

---

**4. Security Contexts restrictifs**

**❌ Mauvais - Privilèges root :**
```yaml
spec:
  containers:
  - name: app
    image: myapp:1.0
    # Tourne en root par défaut
    # Toutes les capabilities
```

**✅ Bon - Minimal privileges :**
```yaml
spec:
  securityContext:
    runAsNonRoot: true
    runAsUser: 1000
  containers:
  - name: app
    image: myapp:1.0
    securityContext:
      readOnlyRootFilesystem: true
      allowPrivilegeEscalation: false
      capabilities:
        drop: [ALL]
```

---

**5. Network Policies - Micro-segmentation**

**❌ Mauvais - Pas de NetworkPolicies :**
```
Tous les Pods peuvent communiquer avec tous les Pods
```

**✅ Bon - Segmentation stricte :**
```yaml
# 1. Deny all par défaut
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
# 2. Allow seulement frontend → backend
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: frontend-to-backend
spec:
  podSelector:
    matchLabels:
      app: backend
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

**6. Secrets access control**

**❌ Mauvais - Tous peuvent lire les Secrets :**
```yaml
rules:
- apiGroups: [""]
  resources: ["secrets"]
  verbs: ["get", "list"]  # Tous les Secrets !
```

**✅ Bon - Accès limité :**
```yaml
rules:
- apiGroups: [""]
  resources: ["secrets"]
  resourceNames: ["app-specific-secret"]  # Seulement ce Secret
  verbs: ["get"]
```

---

**7. Disable automount si non nécessaire**

**❌ Mauvais - Token monté inutilement :**
```yaml
spec:
  # Token K8s monté par défaut
  containers:
  - name: app
    image: static-website:1.0  # N'utilise pas l'API K8s
```

**✅ Bon - Désactiver si inutile :**
```yaml
spec:
  serviceAccountName: static-app-sa
  automountServiceAccountToken: false  # Pas de token
```

---

**8. Pod Security Standards - Restricted**

```yaml
apiVersion: v1
kind: Namespace
metadata:
  name: prod
  labels:
    pod-security.kubernetes.io/enforce: restricted
```

**Force tous les Pods du namespace à avoir des security contexts stricts.**

---

**9. Resource Quotas & Limits**

```yaml
apiVersion: v1
kind: ResourceQuota
metadata:
  name: team-quota
  namespace: team-backend
spec:
  hard:
    requests.cpu: "10"
    requests.memory: 20Gi
    limits.cpu: "20"
    limits.memory: 40Gi
    persistentvolumeclaims: "10"
```

**Empêche une équipe de consommer toutes les ressources du cluster.**

---

**10. Audit & Review régulier**

```bash
# Script d'audit mensuel
#!/bin/bash

echo "=== Users with cluster-admin ==="
kubectl get clusterrolebindings -o json | \
  jq -r '.items[] | select(.roleRef.name=="cluster-admin") | .subjects[]?'

echo "\n=== ServiceAccounts with excessive permissions ==="
kubectl get clusterrolebindings -o json | \
  jq -r '.items[] | select(.subjects[]?.kind=="ServiceAccount") | 
    select(.roleRef.name | test("admin|edit")) | 
    {binding: .metadata.name, sa: .subjects[].name, role: .roleRef.name}'

echo "\n=== Pods running as root ==="
kubectl get pods --all-namespaces -o json | \
  jq -r '.items[] | select(.spec.securityContext.runAsUser==0 or 
    .spec.containers[].securityContext.runAsUser==0) | 
    "\(.metadata.namespace)/\(.metadata.name)"'
```

---

**Checklist Least Privilege :**

```yaml
least-privilege-checklist:
  rbac:
    ✓ Verbs spécifiques (pas ["*"])
    ✓ Resources spécifiques (pas ["*"])
    ✓ Namespace-scoped (Role > ClusterRole)
    ✓ ServiceAccount dédié par app
    ✓ resourceNames si possible
  
  security-context:
    ✓ runAsNonRoot: true
    ✓ capabilities: drop [ALL]
    ✓ readOnlyRootFilesystem: true
    ✓ allowPrivilegeEscalation: false
  
  network:
    ✓ NetworkPolicies activées
    ✓ Default deny all
    ✓ Allow explicite uniquement
  
  secrets:
    ✓ RBAC strict sur Secrets
    ✓ External Secrets Operator
    ✓ Rotation régulière
  
  monitoring:
    ✓ Audit logs activés
    ✓ Alerts sur accès suspects
    ✓ Review mensuel des permissions
```

---

### Q9 : Un développeur se plaint qu'il ne peut pas créer de Deployments dans le namespace prod. Comment investiguer et résoudre ?

**Réponse :**

**Approche de debugging systématique :**

---

**1. Vérifier les permissions actuelles**

```bash
# Test avec kubectl auth can-i
kubectl auth can-i create deployments \
  --as=john@example.com \
  --namespace=prod
# no

# Lister toutes les permissions
kubectl auth can-i --list \
  --as=john@example.com \
  --namespace=prod
```

**Résultat attendu :**
```
Resources          Verbs
pods               [get list]
services           [get list]
configmaps         [get list create update]
# deployments      [PAS LISTÉ]
```

---

**2. Vérifier les RoleBindings**

```bash
# Lister les RoleBindings dans prod
kubectl get rolebindings -n prod

# Voir les détails
kubectl describe rolebinding developer-binding -n prod

# Output:
# Name:         developer-binding
# Subjects:
#   Kind  Name                 Namespace
#   ----  ----                 ---------
#   User  john@example.com
# Role:
#   Kind:  Role
#   Name:  pod-reader  # ← Le Role n'a pas de permissions deployments
```

---

**3. Vérifier le Role**

```bash
kubectl get role pod-reader -n prod -o yaml

# Output:
rules:
- apiGroups: [""]
  resources: ["pods", "services"]
  verbs: ["get", "list"]
# ❌ Pas de "deployments"
```

---

**4. Solution : Ajouter permissions**

**Option A : Modifier le Role existant**
```bash
kubectl edit role pod-reader -n prod

# Ajouter:
rules:
- apiGroups: ["apps"]
  resources: ["deployments"]
  verbs: ["get", "list", "create", "update", "patch"]
```

**Option B : Créer un nouveau Role + RoleBinding**
```yaml
# deployer-role.yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: deployer
  namespace: prod
rules:
# Permissions sur Deployments
- apiGroups: ["apps"]
  resources: ["deployments", "replicasets"]
  verbs: ["get", "list", "create", "update", "patch", "delete"]

# Permissions sur Pods (pour debugging)
- apiGroups: [""]
  resources: ["pods", "pods/log"]
  verbs: ["get", "list", "watch"]

# Permissions sur Services
- apiGroups: [""]
  resources: ["services"]
  verbs: ["get", "list", "create", "update"]

# Permissions sur ConfigMaps
- apiGroups: [""]
  resources: ["configmaps"]
  verbs: ["get", "list", "create", "update"]
---
# deployer-rolebinding.yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: john-deployer
  namespace: prod
subjects:
- kind: User
  name: john@example.com
  apiGroup: rbac.authorization.k8s.io
roleRef:
  kind: Role
  name: deployer
  apiGroup: rbac.authorization.k8s.io
```

```bash
kubectl apply -f deployer-role.yaml
kubectl apply -f deployer-rolebinding.yaml
```

---

**5. Vérifier que ça fonctionne**

```bash
# Re-test
kubectl auth can-i create deployments \
  --as=john@example.com \
  --namespace=prod
# yes ✅

# Test réel
kubectl create deployment nginx --image=nginx \
  --as=john@example.com \
  --namespace=prod
# deployment.apps/nginx created ✅
```

---

**6. Cas particuliers**

**Cas 1 : ClusterRoleBinding mais pas de namespace**
```bash
# L'utilisateur a un ClusterRoleBinding
kubectl get clusterrolebindings | grep john

# Mais le ClusterRole n'a pas de permissions deployments
kubectl get clusterrole developer -o yaml
# rules ne contient pas "deployments"
```

**Solution :** Ajouter permissions au ClusterRole ou créer RoleBinding spécifique.

---

**Cas 2 : Permissions mais Pod Security bloque**
```bash
# L'utilisateur peut créer, mais le Pod est rejeté
kubectl create deployment nginx --image=nginx -n prod
# Error: pods "nginx-xxx" is forbidden: violates PodSecurity "restricted"

# Vérifier Pod Security du namespace
kubectl get namespace prod -o yaml | grep pod-security
# pod-security.kubernetes.io/enforce: restricted
```

**Solution :** Adapter le Deployment pour respecter Restricted :
```yaml
spec:
  template:
    spec:
      securityContext:
        runAsNonRoot: true
        runAsUser: 1000
      containers:
      - name: nginx
        image: nginx
        securityContext:
          allowPrivilegeEscalation: false
          capabilities:
            drop: [ALL]
```

---

**Cas 3 : ResourceQuota limite atteinte**
```bash
kubectl create deployment nginx --image=nginx -n prod
# Error: exceeded quota: team-quota, requested: limits.cpu=1, used: limits.cpu=10, limited: limits.cpu=10

# Vérifier les quotas
kubectl describe resourcequota team-quota -n prod
```

**Solution :** Augmenter quota ou nettoyer ressources inutilisées.

---

**Debugging avancé :**

**Audit logs (si activé) :**
```bash
# Voir les tentatives échouées
jq 'select(.verb=="create" and .objectRef.resource=="deployments" and .responseStatus.code==403)' \
  /var/log/kubernetes/audit.log

# Résultat:
# {
#   "user": "john@example.com",
#   "verb": "create",
#   "objectRef": {"resource": "deployments", "namespace": "prod"},
#   "responseStatus": {"code": 403, "reason": "Forbidden"}
# }
```

---

**Documentation pour le développeur :**

```markdown
# Permissions dans namespace prod

Vous avez maintenant accès au Role "deployer" qui vous permet de :

✅ **Créer/Modifier** :
- Deployments, ReplicaSets
- Services
- ConfigMaps

✅ **Lire** :
- Pods, logs
- Tous les objets ci-dessus

❌ **Interdit** :
- Secrets (contactez l'admin)
- Supprimer des ressources critiques
- Accéder aux autres namespaces

## Tester vos permissions
```bash
kubectl auth can-i create deployments -n prod
kubectl auth can-i --list -n prod
```

## Support
Pour des permissions additionnelles, ouvrez un ticket avec justification.
```

---

**Best Practices :**

✅ **Audit trail** : Logger tous les changements RBAC
✅ **Least privilege** : Donner seulement ce qui est nécessaire
✅ **Documentation** : Expliquer les permissions aux users
✅ **Review régulier** : Vérifier que les permissions sont toujours pertinentes
✅ **IaC** : Gérer RBAC avec GitOps (ArgoCD, Flux)

---

## 🎓 Conseils pour l'Entretien

### Points clés à maîtriser :
1. **RBAC** : Role vs ClusterRole, verbs, testing
2. **ServiceAccounts** : Différence avec Users, token JWT
3. **Security Contexts** : runAsNonRoot, capabilities, readOnlyRootFilesystem
4. **Pod Security Standards** : 3 niveaux, migration
5. **Least Privilege** : Principe et implémentation

### Red flags à éviter :
- ❌ verbs: ["*"] dans RBAC
- ❌ privileged: true en production
- ❌ Tous les Pods en root
- ❌ Pas de NetworkPolicies
- ❌ ServiceAccount "default" partagé

### Points bonus :
- ✅ Audit logging configuré
- ✅ Pod Security Standards enforced
- ✅ External Secrets Operator
- ✅ OPA/Kyverno pour policies
- ✅ Regular security audits

---