## [**..**](./00_index.md)


# Chapitre 1 : Kubernetes Fundamentals

## 1.1. Architecture Overview

Kubernetes est un système d'orchestration de containers organisé en **deux types de nodes** :

### Control Plane (Master Node)
Le cerveau du cluster qui prend toutes les décisions de scheduling et gère l'état du cluster.

### Worker Nodes
Les machines qui exécutent réellement vos applications containerisées.

```
┌─────────────────────────────────────┐
│        CONTROL PLANE                │
│  ┌──────────┐  ┌──────────────┐   │
│  │API Server│  │   Scheduler   │   │
│  └──────────┘  └──────────────┘   │
│  ┌──────────┐  ┌──────────────┐   │
│  │   etcd   │  │Controller Mgr │   │
│  └──────────┘  └──────────────┘   │
└─────────────────────────────────────┘
           │
    ┌──────┴──────┐
    │             │
┌───▼────┐   ┌───▼────┐
│ Worker │   │ Worker │
│  Node  │   │  Node  │
└────────┘   └────────┘
```

---

## 1.2. Core Components

### Control Plane Components

#### **API Server** (`kube-apiserver`)
- Point d'entrée unique pour toutes les opérations REST
- Valide et configure les objets Kubernetes
- Expose l'API Kubernetes (généralement sur le port 6443)

**Exemple d'interaction :**
```bash
# Chaque commande kubectl communique avec l'API Server
kubectl get pods
# → GET https://api-server:6443/api/v1/namespaces/default/pods
```

#### **etcd**
- Base de données clé-valeur distribuée
- Stocke **tout l'état du cluster**
- Source de vérité pour Kubernetes

**Données stockées :**
- Configurations des Pods, Services, ConfigMaps
- État actuel vs état désiré
- Secrets (encodés en base64)

#### **Scheduler** (`kube-scheduler`)
- Assigne les Pods aux Nodes
- Prend en compte : ressources disponibles, contraintes, affinités

**Exemple de décision :**
```yaml
# Pod avec resource requests
apiVersion: v1
kind: Pod
metadata:
  name: my-app
spec:
  containers:
  - name: app
    image: myapp:1.0
    resources:
      requests:
        memory: "512Mi"
        cpu: "500m"
```
→ Le Scheduler cherche un Node avec au moins 512Mi RAM et 0.5 CPU disponibles

#### **Controller Manager** (`kube-controller-manager`)
- Exécute les controllers qui régulent l'état du cluster
- **Boucle de réconciliation** : compare l'état actuel à l'état désiré

**Principaux controllers :**
- **Node Controller** : surveille l'état des nodes
- **Replication Controller** : maintient le bon nombre de replicas
- **Endpoints Controller** : remplit les objets Endpoints (Services ↔ Pods)
- **ServiceAccount Controller** : crée les ServiceAccounts par défaut

---

### Worker Node Components

#### **kubelet**
- Agent qui s'exécute sur chaque Node
- Reçoit les PodSpecs de l'API Server
- Garantit que les containers définis tournent correctement
- Communique l'état des Pods au Control Plane

**Actions du kubelet :**
```bash
# Le kubelet :
1. Pull l'image depuis le registry
2. Démarre le container via le container runtime
3. Monitor la santé (liveness/readiness probes)
4. Reporte le status au API Server
```

#### **kube-proxy**
- Gère les règles réseau sur chaque Node
- Implémente le concept de **Service**
- Modes : iptables, IPVS, userspace

**Exemple :**
```yaml
apiVersion: v1
kind: Service
metadata:
  name: my-service
spec:
  selector:
    app: backend
  ports:
  - port: 80
    targetPort: 8080
```
→ kube-proxy configure les règles pour router le trafic vers les Pods `app=backend`

#### **Container Runtime**
- Logiciel qui exécute les containers
- Exemples : **containerd**, CRI-O, Docker (deprecated)
- Interface : Container Runtime Interface (CRI)

---

## 1.3. kubectl CLI Basics & Configuration

### Installation et Configuration

**Vérifier la configuration :**
```bash
kubectl version --client
kubectl cluster-info
```

**Structure du fichier kubeconfig** (`~/.kube/config`) :
```yaml
apiVersion: v1
kind: Config
clusters:
- cluster:
    server: https://192.168.1.100:6443
    certificate-authority-data: LS0tLS...
  name: prod-cluster
contexts:
- context:
    cluster: prod-cluster
    namespace: default
    user: admin
  name: prod-context
current-context: prod-context
users:
- name: admin
  user:
    client-certificate-data: LS0tLS...
    client-key-data: LS0tLS...
```

### Commandes kubectl Essentielles

#### **Gestion des ressources**
```bash
# Créer des ressources
kubectl apply -f deployment.yaml
kubectl create deployment nginx --image=nginx

# Lister des ressources
kubectl get pods
kubectl get pods -o wide  # Plus de détails
kubectl get pods -o yaml  # Format YAML complet
kubectl get all           # Tous les types de ressources

# Détails d'une ressource
kubectl describe pod my-pod
kubectl explain pod.spec.containers

# Supprimer des ressources
kubectl delete pod my-pod
kubectl delete -f deployment.yaml
```

#### **Debugging**
```bash
# Logs
kubectl logs my-pod
kubectl logs my-pod -c container-name  # Multi-container pod
kubectl logs -f my-pod                 # Follow mode

# Exécution de commandes
kubectl exec -it my-pod -- /bin/bash
kubectl exec my-pod -- env

# Port forwarding
kubectl port-forward pod/my-pod 8080:80
```

#### **Édition en live**
```bash
# Éditer une ressource
kubectl edit deployment my-deployment

# Patch une ressource
kubectl patch deployment my-deployment -p '{"spec":{"replicas":3}}'

# Scale
kubectl scale deployment my-deployment --replicas=5
```

#### **Informations sur le cluster**
```bash
# Nodes
kubectl get nodes
kubectl describe node worker-1

# Événements
kubectl get events --sort-by=.metadata.creationTimestamp

# Top (nécessite metrics-server)
kubectl top nodes
kubectl top pods
```

---

## 1.4. Namespaces et Contextes

### Namespaces

Les **Namespaces** fournissent une isolation logique des ressources dans un cluster.

**Namespaces par défaut :**
```bash
kubectl get namespaces

# Résultat :
# NAME              STATUS   AGE
# default           Active   10d
# kube-system       Active   10d  (composants K8s)
# kube-public       Active   10d  (lisible par tous)
# kube-node-lease   Active   10d  (heartbeats des nodes)
```

#### **Créer et utiliser des Namespaces**

```yaml
# namespace-dev.yaml
apiVersion: v1
kind: Namespace
metadata:
  name: dev
  labels:
    environment: development
```

```bash
# Créer
kubectl create namespace dev
kubectl apply -f namespace-dev.yaml

# Lister les ressources dans un namespace
kubectl get pods -n dev
kubectl get all -n dev

# Lister dans tous les namespaces
kubectl get pods --all-namespaces
kubectl get pods -A  # Raccourci
```

#### **Déployer dans un Namespace**

```yaml
# pod-in-dev.yaml
apiVersion: v1
kind: Pod
metadata:
  name: my-app
  namespace: dev  # ← Spécifier le namespace
spec:
  containers:
  - name: app
    image: nginx:1.21
```

```bash
kubectl apply -f pod-in-dev.yaml
# OU
kubectl run my-app --image=nginx -n dev
```

#### **ResourceQuota par Namespace**

```yaml
apiVersion: v1
kind: ResourceQuota
metadata:
  name: dev-quota
  namespace: dev
spec:
  hard:
    requests.cpu: "10"
    requests.memory: 20Gi
    limits.cpu: "20"
    limits.memory: 40Gi
    pods: "50"
```

---

### Contextes

Les **Contextes** combinent cluster, user, et namespace pour faciliter le switch entre environnements.

#### **Gestion des Contextes**

```bash
# Lister les contextes
kubectl config get-contexts

# Contexte actuel
kubectl config current-context

# Changer de contexte
kubectl config use-context prod-context

# Créer un nouveau contexte
kubectl config set-context dev-context \
  --cluster=my-cluster \
  --user=dev-user \
  --namespace=dev

# Modifier le namespace par défaut d'un contexte
kubectl config set-context --current --namespace=dev
```

#### **Exemple de Workflow Multi-Environnements**

```bash
# Configuration pour 3 environnements
kubectl config set-context dev --namespace=dev --cluster=k8s-cluster --user=dev-user
kubectl config set-context staging --namespace=staging --cluster=k8s-cluster --user=staging-user
kubectl config set-context prod --namespace=prod --cluster=k8s-cluster --user=prod-user

# Switch rapide
kubectl config use-context dev
kubectl get pods  # → Pods dans le namespace dev

kubectl config use-context prod
kubectl get pods  # → Pods dans le namespace prod
```

#### **Alias Utiles**

```bash
# Ajouter dans ~/.bashrc ou ~/.zshrc
alias k=kubectl
alias kgp='kubectl get pods'
alias kgd='kubectl get deployments'
alias kgs='kubectl get services'
alias kd='kubectl describe'
alias kl='kubectl logs'
alias kx='kubectl config use-context'

# Avec completion
source <(kubectl completion bash)
complete -F __start_kubectl k
```

---

## 🎯 Points Clés à Retenir

### Architecture
- **Control Plane** = cerveau (API Server, etcd, Scheduler, Controllers)
- **Worker Nodes** = muscles (kubelet, kube-proxy, container runtime)
- Communication : kubelet → API Server (jamais l'inverse directement)

### kubectl
- Toutes les commandes passent par l'API Server
- Format : `kubectl <verb> <resource> <name> <flags>`
- Modes déclaratif (`apply`) vs impératif (`create`, `run`)

### Namespaces
- Isolation logique, pas physique
- Utilisés pour multi-tenancy, environments, équipes
- ResourceQuotas et RBAC s'appliquent par namespace

### Contextes
- Facilitent le switch entre clusters/namespaces
- Configurés dans `~/.kube/config`
- Essentiels pour gérer plusieurs environnements

---

## 📝 Exercices Pratiques

1. **Explorez votre cluster :**
```bash
kubectl cluster-info
kubectl get nodes -o wide
kubectl describe node <node-name>
```

2. **Créez un namespace et déployez un Pod :**
```bash
kubectl create namespace test
kubectl run nginx --image=nginx --namespace=test
kubectl get pods -n test
kubectl delete namespace test
```

3. **Configurez un contexte dédié :**
```bash
kubectl config set-context test-ctx --namespace=test --cluster=<your-cluster> --user=<your-user>
kubectl config use-context test-ctx
```

4. **Pratiquez le debugging :**
```bash
kubectl run test --image=busybox --restart=Never -- sleep 3600
kubectl exec -it test -- sh
kubectl logs test
kubectl delete pod test
```


---


# Chapitre 1 : Questions/Réponses d'Entretien

## 🎯 Architecture & Composants

### Q1 : Expliquez l'architecture de Kubernetes et les composants principaux.

**Réponse :**
Kubernetes utilise une architecture master-worker :

**Control Plane (Master) :**
- **API Server** : point d'entrée unique, expose l'API REST, valide et persiste les objets
- **etcd** : base de données distribuée qui stocke tout l'état du cluster
- **Scheduler** : assigne les Pods aux Nodes en fonction des ressources et contraintes
- **Controller Manager** : exécute les controllers (replication, endpoints, serviceaccount) qui maintiennent l'état désiré

**Worker Nodes :**
- **kubelet** : agent qui garantit que les containers tournent selon les PodSpecs
- **kube-proxy** : gère les règles réseau pour implémenter les Services
- **Container Runtime** : exécute les containers (containerd, CRI-O)

Le flow : kubectl → API Server → etcd (stockage) → Scheduler (assignation) → kubelet (exécution)

---

### Q2 : Quelle est la différence entre le Control Plane et les Worker Nodes ?

**Réponse :**
- **Control Plane** : prend les décisions (scheduling, détection d'événements, réponses). Il gère l'état global mais n'exécute pas les applications utilisateur.
- **Worker Nodes** : exécutent les workloads applicatifs (Pods). Ils reçoivent les instructions du Control Plane via kubelet.

En production, on sépare physiquement le Control Plane des Workers pour la haute disponibilité et la sécurité. Le Control Plane peut être multi-master (3 ou 5 instances) pour la résilience.

---

### Q3 : Quel est le rôle de etcd dans Kubernetes ?

**Réponse :**
etcd est la source de vérité (single source of truth) du cluster. Il stocke :
- Configuration de tous les objets (Pods, Services, ConfigMaps, Secrets)
- État actuel vs état désiré
- Métadonnées du cluster

**Caractéristiques importantes :**
- Base de données clé-valeur distribuée et consistante
- Utilise le protocole Raft pour le consensus
- Critique : si etcd est down, le cluster ne peut plus être modifié (lecture seule)
- Best practice : backups réguliers de etcd

```bash
# Backup etcd (exemple)
ETCDCTL_API=3 etcdctl snapshot save snapshot.db \
  --endpoints=https://127.0.0.1:2379 \
  --cert=/etc/kubernetes/pki/etcd/server.crt \
  --key=/etc/kubernetes/pki/etcd/server.key \
  --cacert=/etc/kubernetes/pki/etcd/ca.crt
```

---

### Q4 : Comment le Scheduler décide-t-il sur quel Node placer un Pod ?

**Réponse :**
Le Scheduler utilise un processus en deux phases :

**1. Filtering (Predicates) :**
Élimine les Nodes qui ne peuvent pas accueillir le Pod :
- Ressources insuffisantes (CPU, mémoire)
- Taints/Tolerations incompatibles
- Node selector non satisfait
- Affinity/Anti-affinity rules

**2. Scoring (Priorities) :**
Classe les Nodes éligibles selon plusieurs critères :
- Balance des ressources
- Spread des Pods (pour la HA)
- Image locality (image déjà présente)
- Custom priorities

Le Node avec le score le plus élevé est choisi.

**Exemple d'impact :**
```yaml
resources:
  requests:
    memory: "2Gi"  # Le Scheduler cherche un Node avec ≥2Gi disponible
    cpu: "1"
```

---

### Q5 : Qu'est-ce qu'un Controller et donnez des exemples ?

**Réponse :**
Un Controller est une boucle de contrôle qui surveille l'état du cluster et fait des changements pour atteindre l'état désiré.

**Pattern :** 
```
while true:
  état_actuel = get_current_state()
  état_désiré = get_desired_state()
  if état_actuel != état_désiré:
    reconcile()
```

**Exemples :**
- **Deployment Controller** : gère les ReplicaSets, rolling updates
- **ReplicaSet Controller** : maintient le nombre de replicas
- **Node Controller** : surveille la santé des Nodes (timeout après 40s)
- **Job Controller** : exécute les Pods jusqu'à complétion
- **Endpoint Controller** : maintient les objets Endpoints (mapping Service ↔ Pods)

---

## 🔧 kubectl & Configuration

### Q6 : Quelle est la différence entre `kubectl create` et `kubectl apply` ?

**Réponse :**

**`kubectl create` (impératif) :**
- Crée une ressource une seule fois
- Échoue si la ressource existe déjà
- Pas de tracking des modifications
```bash
kubectl create deployment nginx --image=nginx
# Error si déjà existant
```

**`kubectl apply` (déclaratif) :**
- Crée ou met à jour la ressource
- Idempotent : peut être exécuté plusieurs fois
- Utilise la three-way merge (last-applied-configuration)
- **Best practice en production**
```bash
kubectl apply -f deployment.yaml
# Fonctionne même si existant, applique les diffs
```

**Recommandation :** Utilisez toujours `apply` avec des fichiers YAML versionnés dans Git (GitOps).

---

### Q7 : Comment debugger un Pod qui ne démarre pas ?

**Réponse :**
Approche systématique en 5 étapes :

**1. Vérifier le status :**
```bash
kubectl get pods
# STATUS: Pending, CrashLoopBackOff, ImagePullBackOff, Error
```

**2. Describe pour les events :**
```bash
kubectl describe pod my-pod
# Regarder la section Events en bas
```

**3. Logs du container :**
```bash
kubectl logs my-pod
kubectl logs my-pod -c container-name  # Multi-container
kubectl logs my-pod --previous          # Logs du container crashé
```

**4. Vérifier les ressources et contraintes :**
```bash
kubectl get pod my-pod -o yaml
# Vérifier: resources, nodeSelector, tolerations, image
```

**5. Debug interactif si le Pod tourne :**
```bash
kubectl exec -it my-pod -- sh
# Ou créer un Pod de debug
kubectl debug my-pod -it --image=busybox --share-processes
```

**Causes fréquentes :**
- Image inexistante ou registry inaccessible (ImagePullBackOff)
- Ressources insuffisantes (Pending)
- Application crashe au démarrage (CrashLoopBackOff)
- Probes qui échouent

---

### Q8 : Qu'est-ce que le fichier kubeconfig et comment est-il structuré ?

**Réponse :**
Le kubeconfig (`~/.kube/config`) contient les informations pour se connecter aux clusters.

**Structure (3 sections) :**
```yaml
clusters:         # Informations sur les clusters
- cluster:
    server: https://k8s-api:6443
    certificate-authority-data: LS0t...
  name: prod-cluster

users:            # Credentials utilisateur
- user:
    client-certificate-data: LS0t...
    client-key-data: LS0t...
  name: admin-user

contexts:         # Combinaison cluster + user + namespace
- context:
    cluster: prod-cluster
    user: admin-user
    namespace: default
  name: prod-context

current-context: prod-context  # Contexte actif
```

**Gestion :**
```bash
export KUBECONFIG=~/.kube/config:~/.kube/config-dev  # Merge
kubectl config view
kubectl config use-context dev-context
```

---

### Q9 : Comment lister toutes les ressources d'un cluster ?

**Réponse :**
```bash
# Toutes les ressources dans le namespace actuel
kubectl get all

# Toutes les ressources dans tous les namespaces
kubectl get all -A
kubectl get all --all-namespaces

# Par type spécifique
kubectl get pods,svc,deploy -A

# Vraiment TOUT (inclut CRDs)
kubectl api-resources --verbs=list -o name | xargs -n 1 kubectl get -A

# Avec plus de détails
kubectl get all -o wide
```

**Attention :** `kubectl get all` ne liste PAS ConfigMaps, Secrets, PVCs, etc. Il faut les spécifier explicitement.

---

## 📦 Namespaces & Isolation

### Q10 : À quoi servent les Namespaces ? Donnez des cas d'usage.

**Réponse :**
Les Namespaces fournissent une isolation logique (pas physique) des ressources.

**Cas d'usage :**
1. **Environnements multiples :** dev, staging, prod dans le même cluster
2. **Multi-tenancy :** équipe-frontend, équipe-backend
3. **Isolation des ressources :** ResourceQuotas par namespace
4. **Sécurité :** RBAC granulaire par namespace

**Namespaces système :**
- `default` : namespace par défaut
- `kube-system` : composants Kubernetes
- `kube-public` : lisible par tous (non authentifiés)
- `kube-node-lease` : heartbeats des Nodes

**Exemple - Multi-environnement :**
```bash
kubectl create namespace dev
kubectl create namespace prod

# ResourceQuota pour limiter dev
kubectl apply -f - <<EOF
apiVersion: v1
kind: ResourceQuota
metadata:
  name: dev-quota
  namespace: dev
spec:
  hard:
    requests.cpu: "10"
    pods: "50"
EOF
```

---

### Q11 : Quelles ressources sont namespacées vs non-namespacées ?

**Réponse :**

**Namespacées (scope = Namespaced) :**
- Pods, Services, Deployments, ReplicaSets
- ConfigMaps, Secrets
- PersistentVolumeClaims, ServiceAccounts
- Roles, RoleBindings

**Non-namespacées (scope = Cluster) :**
- Nodes, PersistentVolumes
- ClusterRoles, ClusterRoleBindings
- StorageClasses, Namespaces eux-mêmes
- CustomResourceDefinitions (CRDs)

```bash
# Vérifier le scope d'une ressource
kubectl api-resources | grep -i deployment
# NAME          SHORTNAMES   APIVERSION   NAMESPACED   KIND
# deployments   deploy       apps/v1      true         Deployment

kubectl api-resources --namespaced=false
kubectl api-resources --namespaced=true
```

---

### Q12 : Comment référencer un Service dans un autre Namespace ?

**Réponse :**
Utilisez le FQDN (Fully Qualified Domain Name) DNS de Kubernetes :

**Format :** `<service-name>.<namespace>.svc.cluster.local`

**Exemples :**
```yaml
# Service 'database' dans namespace 'prod'
apiVersion: v1
kind: Pod
metadata:
  name: app
  namespace: dev
spec:
  containers:
  - name: app
    image: myapp
    env:
    - name: DB_HOST
      value: "database.prod.svc.cluster.local"  # ← FQDN
```

**Simplifications possibles :**
- Même namespace : `database` (juste le nom)
- Autre namespace : `database.prod` (namespace suffit)
- FQDN complet : `database.prod.svc.cluster.local` (explicite)

**Note :** Par défaut, les NetworkPolicies n'isolent PAS les namespaces. Il faut les configurer explicitement.

---

## 🔐 Sécurité & Accès

### Q13 : Comment fonctionne l'authentification avec l'API Server ?

**Réponse :**
L'API Server supporte plusieurs méthodes d'authentification :

**1. Certificats X.509 (le plus commun) :**
```bash
# Inclus dans kubeconfig
client-certificate-data: LS0t...
client-key-data: LS0t...
```

**2. Bearer Tokens :**
- ServiceAccount tokens (JWT)
- Bootstrap tokens
```bash
curl https://api-server:6443/api/v1/pods \
  -H "Authorization: Bearer <token>"
```

**3. Autres :**
- Basic Auth (déprécié)
- OpenID Connect (OIDC)
- Webhook Token Authentication

**Flow d'une requête kubectl :**
```
kubectl get pods
  ↓
1. Authentication (qui es-tu ?) → Vérifie le certificat/token
2. Authorization (RBAC) → As-tu le droit ?
3. Admission Controllers → Validations finales
4. Exécution → Récupération des données depuis etcd
```

---

### Q14 : Qu'est-ce qu'un ServiceAccount et quand l'utiliser ?

**Réponse :**
Un ServiceAccount fournit une identité pour les Pods afin d'interagir avec l'API Server.

**Utilisation :**
- Par défaut, chaque namespace a un ServiceAccount `default`
- Chaque Pod utilise automatiquement ce ServiceAccount sauf si spécifié autrement
- Un token JWT est monté dans `/var/run/secrets/kubernetes.io/serviceaccount/token`

**Cas d'usage :**
- Application qui liste/crée des ressources K8s (CI/CD, Operators)
- Accès restreint via RBAC

**Exemple :**
```yaml
apiVersion: v1
kind: ServiceAccount
metadata:
  name: my-app-sa
---
apiVersion: v1
kind: Pod
metadata:
  name: my-app
spec:
  serviceAccountName: my-app-sa  # ← Utiliser ce SA
  containers:
  - name: app
    image: myapp
```

```bash
# Créer et lier un Role
kubectl create serviceaccount my-app-sa
kubectl create role pod-reader --verb=get,list --resource=pods
kubectl create rolebinding my-app-binding \
  --role=pod-reader --serviceaccount=default:my-app-sa
```

---

## 🌐 Réseau & Communication

### Q15 : Comment les Pods communiquent-ils entre eux ?

**Réponse :**
Kubernetes utilise un modèle réseau flat :

**Principes :**
1. Chaque Pod a sa propre IP unique dans le cluster
2. Tous les Pods peuvent communiquer sans NAT
3. Les Nodes peuvent communiquer avec tous les Pods

**Implémentation :**
- Plugin CNI (Container Network Interface) : Calico, Flannel, Cilium, Weave
- Overlay network ou routing direct

**Exemples :**
```bash
# Pod A (IP: 10.244.1.5) peut ping Pod B (IP: 10.244.2.8)
kubectl exec pod-a -- ping 10.244.2.8

# Via Service (DNS)
kubectl exec pod-a -- curl http://my-service:8080
# Résolu en: my-service.default.svc.cluster.local
```

**Composants impliqués :**
- CNI Plugin : assigne les IPs et configure le réseau
- kube-proxy : maintient les règles iptables pour les Services
- CoreDNS : résolution DNS interne

---

### Q16 : Expliquez le rôle de kube-proxy.

**Réponse :**
kube-proxy implémente les Services en gérant les règles réseau sur chaque Node.

**Fonctionnement :**
1. Watch l'API Server pour les Services et Endpoints
2. Configure les règles réseau (iptables/IPVS) pour router le trafic
3. Load balance entre les Pods backend

**Modes :**
- **iptables** (défaut) : règles iptables, random load balancing
- **IPVS** : meilleure performance, algorithmes LB avancés (round-robin, least connection)
- **userspace** (déprécié) : proxy en espace utilisateur

**Exemple :**
```yaml
# Service qui expose 3 Pods
apiVersion: v1
kind: Service
metadata:
  name: web
spec:
  selector:
    app: web
  ports:
  - port: 80
    targetPort: 8080
```

kube-proxy configure :
```bash
# Traffic vers web:80 → Load balancé vers les Pods
ClusterIP:80 → Pod1:8080, Pod2:8080, Pod3:8080
```

---

## 💡 Questions Situationnelles

### Q17 : Le Scheduler a assigné un Pod à un Node, mais le Pod reste en Pending. Pourquoi ?

**Réponses possibles :**

**1. Ressources insuffisantes sur le Node :**
```bash
kubectl describe node <node-name>
# Vérifier Allocated resources vs Capacity
```

**2. Taints sur le Node :**
```bash
kubectl describe node <node-name> | grep Taints
# Le Pod n'a pas les Tolerations nécessaires
```

**3. Volume non disponible :**
- PersistentVolume non bound
- StorageClass ne peut pas provisionner

**4. Image non pullable :**
```bash
kubectl describe pod <pod>
# Events: Failed to pull image
```

**5. Node en status NotReady :**
```bash
kubectl get nodes
# kubelet down ou problèmes réseau
```

---

### Q18 : Comment gérez-vous plusieurs clusters Kubernetes (dev, staging, prod) ?

**Réponse :**
**Méthode 1 : kubeconfig avec contexts**
```bash
# Définir les contexts
kubectl config set-context dev --cluster=dev-cluster --user=dev-user --namespace=default
kubectl config set-context prod --cluster=prod-cluster --user=prod-user --namespace=default

# Switch
kubectl config use-context dev
kubectl get pods  # Cluster dev

kubectl config use-context prod
kubectl get pods  # Cluster prod
```

**Méthode 2 : Fichiers kubeconfig séparés**
```bash
export KUBECONFIG=~/.kube/config-dev
kubectl get pods  # Cluster dev

export KUBECONFIG=~/.kube/config-prod
kubectl get pods  # Cluster prod
```

**Méthode 3 : Tools dédiés (kubectx/kubens)**
```bash
kubectx dev    # Switch vers contexte dev
kubens staging # Switch vers namespace staging
```

**Best practices :**
- Nommage clair des contextes (dev, staging, prod)
- Couleurs différentes dans le prompt (kube-ps1)
- Approvals requis pour prod (GitOps, ArgoCD)
- RBAC strict en production

---

### Q19 : Que se passe-t-il si etcd tombe en panne ?

**Réponse :**
**Impact immédiat :**
- ❌ Impossible de créer/modifier/supprimer des ressources
- ✅ Les Pods existants continuent de tourner (kubelet cache les specs)
- ✅ Services continuent de fonctionner (kube-proxy a les règles en cache)
- ❌ Le Scheduler ne peut plus assigner de nouveaux Pods
- ❌ Les Controllers ne peuvent plus réconcilier l'état

**Durée de l'impact :**
- Si etcd revient en < 5 minutes → reprise normale
- Si etcd perd ses données → **catastrophe**, restaurer depuis backup

**Mitigation :**
```bash
# Setup etcd en cluster (3 ou 5 nodes)
# Backup régulier
ETCDCTL_API=3 etcdctl snapshot save backup.db

# Monitoring de etcd
watch etcdctl endpoint health
```

**En production :** Toujours avoir un cluster etcd HA (3 ou 5 membres) et des backups automatisés quotidiens.

---

### Q20 : Un développeur vous dit "Mon application Spring Boot fonctionne localement mais crash dans Kubernetes". Comment investiguer ?

**Réponse :**
**Étape 1 : Vérifier les logs**
```bash
kubectl logs <pod-name>
kubectl logs <pod-name> --previous  # Si crashé
```

**Problèmes fréquents Spring Boot :**

**1. Configuration manquante :**
```yaml
# ConfigMap ou Secrets non montés
env:
- name: SPRING_DATASOURCE_URL
  valueFrom:
    configMapKeyRef:
      name: app-config
      key: db.url
```

**2. Ressources insuffisantes (OOM Kill) :**
```yaml
resources:
  limits:
    memory: "512Mi"  # Trop bas pour Spring Boot
# Solution: augmenter à 1-2Gi
```

**3. Health checks trop agressifs :**
```yaml
livenessProbe:
  httpGet:
    path: /actuator/health
  initialDelaySeconds: 60  # Spring Boot démarre lentement
  periodSeconds: 10
```

**4. Connexion DB/Services externes :**
```bash
# Tester la connectivité
kubectl exec -it <pod> -- curl database.prod.svc.cluster.local:5432
```

**5. JVM heap dans containers :**
```yaml
env:
- name: JAVA_OPTS
  value: "-Xmx1g -Xms512m -XX:MaxRAMPercentage=75.0"
```

**Checklist debug :**
- [ ] Logs (actuels et précédents)
- [ ] Describe pod (events)
- [ ] Variables d'environnement
- [ ] ConfigMaps/Secrets montés correctement
- [ ] Ressources (CPU/Mémoire)
- [ ] Health checks (liveness/readiness)
- [ ] Connectivité réseau (DNS, Services)

---

## 🎓 Conseils pour l'Entretien

### Comment bien répondre :
1. **Structure ta réponse** : Définition → Explication → Exemple
2. **Montre ton expérience** : "Dans mon projet précédent, j'ai utilisé..."
3. **Sois précis** : Utilise les termes techniques exacts (API Server, etcd, etc.)
4. **Admets si tu ne sais pas** : "Je n'ai pas utilisé cette fonctionnalité, mais je sais que..."
5. **Pose des questions** : "Voulez-vous que je détaille un point spécifique ?"

### Red flags à éviter :
- ❌ Confondre Docker et Kubernetes
- ❌ Dire "Je lance kubectl run et ça marche"
- ❌ Ignorer les best practices (resources, health checks)
- ❌ Ne pas connaître RBAC ou Namespaces

### Points bonus :
- ✅ Mentionner GitOps, Infrastructure as Code
- ✅ Parler de monitoring (Prometheus, logs)
- ✅ Évoquer la sécurité (RBAC, NetworkPolicies)
- ✅ Expérience avec des outils (Helm, ArgoCD)