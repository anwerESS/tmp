## [**..**](./00_index.md)


# Chapitre 8 : Resource Management

## 8.1. Resource Requests & Limits

### Qu'est-ce que les Resources ?

Les **resources** définissent la quantité de CPU et mémoire qu'un Pod nécessite et peut consommer.

**2 concepts clés :**
- **Requests** : Ressources garanties (minimum)
- **Limits** : Ressources maximum autorisées

```yaml
resources:
  requests:
    memory: "256Mi"    # Minimum garanti
    cpu: "250m"        # 0.25 CPU garanti
  limits:
    memory: "512Mi"    # Maximum autorisé
    cpu: "500m"        # 0.5 CPU maximum
```

---

### CPU : Unités

**Formats :**
```yaml
cpu: "1"      # 1 CPU core
cpu: "1000m"  # 1000 millicores = 1 CPU
cpu: "500m"   # 0.5 CPU
cpu: "100m"   # 0.1 CPU
```

**1 CPU = 1000 millicores (m)**

---

### Memory : Unités

**Formats :**
```yaml
memory: "128Mi"   # 128 Mebibytes (1024-based)
memory: "1Gi"     # 1 Gibibyte
memory: "256M"    # 256 Megabytes (1000-based)
```

**Recommandé : Utiliser Mi/Gi (1024-based)**

---

### Requests vs Limits

**Requests (Minimum garanti) :**
- Kubernetes **garantit** ces ressources
- Utilisé pour le **scheduling** (placement du Pod)

**Limits (Maximum autorisé) :**
- Le Pod ne peut **jamais dépasser** ces limites
- **CPU** : Throttling (ralentissement)
- **Memory** : OOMKilled (Pod tué)

---

### Exemple complet

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
spec:
  replicas: 3
  template:
    spec:
      containers:
      - name: app
        image: order-service:1.0
        
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
```

**Comportement :**
- K8s place le Pod sur un Node ayant au moins **512Mi + 500m** disponibles
- Le Pod peut utiliser **jusqu'à** 1Gi et 1000m
- Si dépassement memory → **OOMKilled**
- Si dépassement CPU → **Throttled**

---

### CPU Throttling

**Si Pod dépasse sa limit CPU :**

```yaml
resources:
  limits:
    cpu: "500m"
```

- Pod essaie d'utiliser 1 CPU
- K8s limite à 0.5 CPU
- Application plus lente, mais **Pod pas tué**

**Vérifier throttling :**
```bash
kubectl exec my-pod -- cat /sys/fs/cgroup/cpu/cpu.stat
# nr_throttled 50000  # Throttled 50% du temps !
```

---

### OOMKilled (Out Of Memory)

**Si Pod dépasse sa limit memory :**

```yaml
resources:
  limits:
    memory: "512Mi"
```

- Pod consomme 513Mi
- K8s tue le Pod immédiatement
- Exit code : 137

**Détecter :**
```bash
kubectl describe pod my-pod
# Last State: Terminated
#   Reason: OOMKilled
#   Exit Code: 137
```

---

### Best Practices

✅ **Toujours définir requests ET limits**
```yaml
resources:
  requests:
    memory: "256Mi"
    cpu: "250m"
  limits:
    memory: "512Mi"
    cpu: "500m"
```

✅ **Ratio limits/requests raisonnable (1.5x-2x)**
```yaml
requests:
  memory: "512Mi"
limits:
  memory: "1Gi"  # 2x
```

✅ **JVM : Xmx = 75% de memory limit**
```yaml
resources:
  limits:
    memory: "1Gi"
env:
- name: JAVA_OPTS
  value: "-Xmx768m"
```

---

## 8.2. Quality of Service (QoS) Classes

### Qu'est-ce que QoS ?

Kubernetes assigne une **QoS class** à chaque Pod basée sur ses resources.

**3 classes :**
1. **Guaranteed** (priorité haute)
2. **Burstable** (priorité moyenne)
3. **BestEffort** (priorité basse)

**Lors de resource pressure, K8s évicte dans l'ordre :**
```
BestEffort → Burstable → Guaranteed
```

---

### Guaranteed

**Conditions :**
- **Tous** les containers ont requests ET limits
- requests = limits (CPU ET Memory)

```yaml
resources:
  requests:
    memory: "512Mi"
    cpu: "500m"
  limits:
    memory: "512Mi"  # = requests
    cpu: "500m"      # = requests
```

**Vérifier :**
```bash
kubectl get pod my-pod -o jsonpath='{.status.qosClass}'
# Guaranteed
```

**Caractéristiques :**
- ✅ **Jamais évicté** (sauf OOMKilled)
- ✅ Ressources garanties
- ✅ Idéal pour workloads critiques

**Use case :**
- Databases (PostgreSQL, Redis)
- Applications critiques

---

### Burstable

**Conditions :**
- Au moins 1 container a requests OU limits
- requests ≠ limits

```yaml
# Exemple 1
resources:
  requests:
    memory: "256Mi"
    cpu: "250m"
  limits:
    memory: "512Mi"  # ≠ requests
    cpu: "500m"

# Exemple 2
resources:
  requests:
    memory: "256Mi"
  # Pas de limits
```

**Caractéristiques :**
- ⚠️ Évicté si Node sous pression
- ✅ Peut utiliser plus que requests
- ✅ Flexible

**Use case :**
- Microservices
- Applications web

---

### BestEffort

**Conditions :**
- **Aucun** container n'a requests ni limits

```yaml
resources: {}
```

**Caractéristiques :**
- ❌ **Premier évicté**
- ❌ Aucune garantie
- ❌ À éviter en production

**Use case :**
- Dev/test uniquement

---

## 8.3. LimitRanges

### Qu'est-ce qu'un LimitRange ?

Un **LimitRange** définit des contraintes de ressources **par défaut** et **min/max** dans un **namespace**.

---

### Exemple complet

```yaml
apiVersion: v1
kind: LimitRange
metadata:
  name: resource-limits
  namespace: prod
spec:
  limits:
  - type: Container
    default:  # limits par défaut
      memory: "512Mi"
      cpu: "500m"
    defaultRequest:  # requests par défaut
      memory: "256Mi"
      cpu: "250m"
    max:  # Maximum autorisé
      memory: "2Gi"
      cpu: "2000m"
    min:  # Minimum requis
      memory: "128Mi"
      cpu: "100m"
    maxLimitRequestRatio:
      memory: 2
      cpu: 2
```

**Comportement :**
- Pods sans resources → Reçoivent les valeurs par défaut
- Pods avec resources < min → Rejetés
- Pods avec resources > max → Rejetés
- Ratio limits/requests > 2 → Rejetés

---

### Use Cases

**Namespace dev - Valeurs par défaut :**
```yaml
apiVersion: v1
kind: LimitRange
metadata:
  name: dev-defaults
  namespace: dev
spec:
  limits:
  - default:
      memory: "256Mi"
      cpu: "250m"
    defaultRequest:
      memory: "128Mi"
      cpu: "100m"
    type: Container
```

**Namespace prod - Contraintes strictes :**
```yaml
apiVersion: v1
kind: LimitRange
metadata:
  name: prod-constraints
  namespace: prod
spec:
  limits:
  - min:
      memory: "256Mi"
      cpu: "250m"
    max:
      memory: "2Gi"
      cpu: "2000m"
    type: Container
```

---

## 8.4. ResourceQuotas

### Qu'est-ce qu'un ResourceQuota ?

Un **ResourceQuota** limite la **consommation totale** dans un **namespace**.

**Différence avec LimitRange :**
- **LimitRange** : Limites par Pod
- **ResourceQuota** : Limites totales namespace

---

### Exemple complet

```yaml
apiVersion: v1
kind: ResourceQuota
metadata:
  name: team-quota
  namespace: team-backend
spec:
  hard:
    # Compute resources
    requests.cpu: "10"
    requests.memory: 20Gi
    limits.cpu: "20"
    limits.memory: 40Gi
    
    # Object count
    pods: "20"
    services: "10"
    configmaps: "20"
    secrets: "20"
    persistentvolumeclaims: "10"
    
    # Storage
    requests.storage: "100Gi"
```

**Comportement :**
- Si total requests.cpu atteint 10 → Nouveau Pod rejeté
- Si 20 Pods existent → Nouveau Pod rejeté

---

### Vérifier ResourceQuota

```bash
# Voir l'utilisation
kubectl describe resourcequota team-quota -n team-backend

# Output:
# Resource         Used   Hard
# --------         ----   ----
# pods             8      20
# requests.cpu     6      10
# requests.memory  12Gi   20Gi
```

---

### Use Cases

**Multi-tenancy - Isolation par équipe :**
```yaml
# Team A
apiVersion: v1
kind: ResourceQuota
metadata:
  name: team-a-quota
  namespace: team-a
spec:
  hard:
    requests.cpu: "20"
    requests.memory: 40Gi
---
# Team B
apiVersion: v1
kind: ResourceQuota
metadata:
  name: team-b-quota
  namespace: team-b
spec:
  hard:
    requests.cpu: "10"
    requests.memory: 20Gi
```

---

## 8.5. Horizontal Pod Autoscaler (HPA)

### Qu'est-ce qu'HPA ?

**HPA** ajuste automatiquement le nombre de replicas basé sur les métriques.

**Horizontal scaling** : Ajouter/retirer des Pods (→)

---

### Prérequis

**1. Metrics Server**
```bash
kubectl apply -f https://github.com/kubernetes-sigs/metrics-server/releases/latest/download/components.yaml
```

**2. Resources requests définis**
```yaml
resources:
  requests:
    cpu: "500m"  # Requis !
```

---

### HPA basé sur CPU

```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: order-service-hpa
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  minReplicas: 3
  maxReplicas: 10
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70  # Target 70%
```

**Comportement :**
- CPU > 70% → Scale up
- CPU < 70% → Scale down
- Min : 3, Max : 10

**Commande :**
```bash
kubectl autoscale deployment order-service \
  --cpu-percent=70 \
  --min=3 \
  --max=10
```

---

### HPA multi-métriques

```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: order-service-hpa
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  minReplicas: 3
  maxReplicas: 10
  metrics:
  # CPU
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
  # Memory
  - type: Resource
    resource:
      name: memory
      target:
        type: Utilization
        averageUtilization: 80
```

**Scale up si CPU > 70% OU Memory > 80%**

---

### Behavior Configuration

```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: order-service-hpa
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  minReplicas: 3
  maxReplicas: 10
  
  behavior:
    scaleUp:
      stabilizationWindowSeconds: 0
      policies:
      - type: Percent
        value: 100  # Double
        periodSeconds: 15
    
    scaleDown:
      stabilizationWindowSeconds: 300  # 5 min
      policies:
      - type: Pods
        value: 1
        periodSeconds: 60
```

**Évite flapping (scale up/down répétitif)**

---

### Vérifier HPA

```bash
# Status
kubectl get hpa

# Output:
# NAME                TARGETS    MINPODS   MAXPODS   REPLICAS
# order-service-hpa   45%/70%    3         10        3

# Détails
kubectl describe hpa order-service-hpa
```

---

### Best Practices HPA

✅ **minReplicas ≥ 2**
```yaml
minReplicas: 3
```

✅ **Target CPU 60-80%**
```yaml
averageUtilization: 70
```

✅ **Stabilization window**
```yaml
stabilizationWindowSeconds: 300
```

✅ **PodDisruptionBudget**
```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: order-service-pdb
spec:
  minAvailable: 2
  selector:
    matchLabels:
      app: order-service
```

---

## 8.6. Vertical Pod Autoscaler (VPA)

### Qu'est-ce que VPA ?

**VPA** ajuste automatiquement les **requests et limits** des Pods.

**Vertical scaling** : Augmenter resources (↕)

**Différence avec HPA :**
- **HPA** : Plus de Pods
- **VPA** : Pods plus gros

---

### VPA Modes

**1. Off (recommandations uniquement)**
```yaml
apiVersion: autoscaling.k8s.io/v1
kind: VerticalPodAutoscaler
metadata:
  name: order-service-vpa
spec:
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  updatePolicy:
    updateMode: "Off"
```

**2. Auto (apply automatiquement)**
```yaml
updatePolicy:
  updateMode: "Auto"
```

**⚠️ VPA redémarre les Pods !**

---

### Exemple complet

```yaml
apiVersion: autoscaling.k8s.io/v1
kind: VerticalPodAutoscaler
metadata:
  name: order-service-vpa
spec:
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  
  updatePolicy:
    updateMode: "Auto"
  
  resourcePolicy:
    containerPolicies:
    - containerName: app
      minAllowed:
        cpu: "250m"
        memory: "256Mi"
      maxAllowed:
        cpu: "2000m"
        memory: "4Gi"
```

---

### Voir recommandations

```bash
kubectl describe vpa order-service-vpa

# Output:
# Recommendation:
#   Target:
#     Cpu:     500m
#     Memory:  800Mi
```

---

### VPA vs HPA

⚠️ **Attention : Incompatibilité !**

```yaml
# ❌ Conflit si les deux sur CPU
HPA: scale basé sur CPU
VPA: ajuste CPU requests

# ✅ Possible si métriques différentes
HPA: scale sur custom metrics
VPA: ajuste CPU/Memory
```

---

### Best Practices VPA

✅ **Commencer en mode Off**
```yaml
updateMode: "Off"
```

✅ **Définir min/max**
```yaml
minAllowed:
  cpu: "250m"
maxAllowed:
  cpu: "2000m"
```

❌ **Éviter VPA + HPA sur CPU**

---

# Questions/Réponses d'Entretien - Chapitre 8

## Q1 : Expliquez la différence entre requests et limits. Que se passe-t-il si un Pod dépasse sa limit ?

**Réponse :**

**Requests (Minimum garanti) :**
- Ressources **garanties** par Kubernetes
- Utilisé pour le **scheduling**

**Limits (Maximum autorisé) :**
- Le Pod ne peut **jamais dépasser**

---

**Si dépassement CPU limit :**

**→ CPU Throttling (ralentissement)**
- Application plus lente
- **Pod pas tué**

```bash
# Vérifier
kubectl exec my-pod -- cat /sys/fs/cgroup/cpu/cpu.stat
# nr_throttled 50000
```

**Impact :**
- ⚠️ Latence augmentée
- ⚠️ Timeouts possibles

---

**Si dépassement Memory limit :**

**→ OOMKilled (Pod tué)**
- Exit code : 137
- Pod redémarre

```bash
kubectl describe pod my-pod
# Reason: OOMKilled
# Exit Code: 137
```

---

**Tableau récapitulatif :**

| Ressource | Dépassement | Action |
|-----------|-------------|--------|
| **CPU** | > limit | **Throttling** |
| **Memory** | > limit | **OOMKilled** |

---

**Best Practices :**

✅ **Toujours requests ET limits**
```yaml
resources:
  requests:
    memory: "256Mi"
    cpu: "250m"
  limits:
    memory: "512Mi"
    cpu: "500m"
```

✅ **Ratio 1.5x-2x**
```yaml
requests:
  memory: "512Mi"
limits:
  memory: "1Gi"  # 2x
```

✅ **JVM : Xmx = 75% limit**
```yaml
limits:
  memory: "1Gi"
env:
- name: JAVA_OPTS
  value: "-Xmx768m"
```

---

## Q2 : Comment dimensionnez-vous les resources pour une application Spring Boot ?

**Réponse :**

**Approche en 4 étapes :**

---

**1. Profiling de l'application**

```bash
# Lancer avec monitoring
java -Xms256m -Xmx1g -jar app.jar

# Load test
ab -n 10000 -c 100 http://localhost:8080/api/orders

# Observer avec VisualVM
```

**Métriques observées :**
```
Idle:
- Heap: 200Mi
- CPU: 50m

Normal load (100 req/s):
- Heap: 600Mi
- CPU: 800m

Peak (500 req/s):
- Heap: 900Mi
- CPU: 1500m
```

---

**2. Calculer resources**

**Formula :**
```yaml
requests:
  memory: normal_load * 1.2
  cpu: normal_load * 1.2

limits:
  memory: peak * 1.3
  cpu: peak * 1.2
```

**Exemple calcul :**
```
Normal: 600Mi, 800m
Peak: 900Mi, 1500m

→ requests: 768Mi, 1000m
→ limits: 1280Mi, 2000m
```

---

**3. Sizing par type d'app**

**A. REST API légère**
```yaml
resources:
  requests:
    memory: "256Mi"
    cpu: "250m"
  limits:
    memory: "512Mi"
    cpu: "500m"
env:
- name: JAVA_OPTS
  value: "-Xms128m -Xmx384m"
```

**B. Microservice standard**
```yaml
resources:
  requests:
    memory: "512Mi"
    cpu: "500m"
  limits:
    memory: "1Gi"
    cpu: "1000m"
env:
- name: JAVA_OPTS
  value: "-Xms256m -Xmx768m"
```

**C. Application intensive**
```yaml
resources:
  requests:
    memory: "1Gi"
    cpu: "1000m"
  limits:
    memory: "2Gi"
    cpu: "2000m"
env:
- name: JAVA_OPTS
  value: "-Xms512m -Xmx1536m"
```

---

**4. Monitoring et ajustement**

```bash
# Observer en staging
kubectl top pods -n staging

# Prometheus
container_memory_working_set_bytes / container_spec_memory_limit_bytes * 100
```

**Ajuster si :**
- Memory > 90% → Augmenter
- CPU throttling → Augmenter
- Resources < 30% utilisées → Réduire

---

## Q3 : Qu'est-ce que la QoS class et comment K8s l'utilise ?

**Réponse :**

**3 classes (priorité) :**

---

**1. Guaranteed (HAUTE)**

**Conditions :**
- requests = limits

```yaml
resources:
  requests:
    memory: "512Mi"
    cpu: "500m"
  limits:
    memory: "512Mi"
    cpu: "500m"
```

**Caractéristiques :**
- ✅ **Jamais évicté**
- ✅ Priorité maximale

**Use case :**
- Databases
- Applications critiques

---

**2. Burstable (MOYENNE)**

**Conditions :**
- requests ≠ limits

```yaml
resources:
  requests:
    memory: "256Mi"
  limits:
    memory: "512Mi"
```

**Caractéristiques :**
- ⚠️ Évicté si Node sous pression
- ✅ Flexible

**Use case :**
- Microservices
- Applications web

---

**3. BestEffort (BASSE)**

**Conditions :**
- Pas de requests/limits

```yaml
resources: {}
```

**Caractéristiques :**
- ❌ **Premier évicté**
- ❌ À éviter en prod

---

**Éviction lors de resource pressure :**

```
Node surchargé:

Ordre d'éviction:
1. BestEffort      ← Premier
2. Burstable       ← Si pas assez
3. Guaranteed      ← Jamais (sauf OOMKilled)
```

**Exemple :**
```
Pods:
- postgres (Guaranteed)      2Gi
- api-service (Burstable)    1.5Gi
- debug-pod (BestEffort)     500Mi

Memory pressure → debug-pod évicté EN PREMIER
```

---

## Q4 : Comment configurez-vous l'autoscaling (HPA) pour Spring Boot ?

**Réponse :**

**Configuration en 5 étapes :**

---

**1. Prérequis**

```bash
# Metrics Server
kubectl apply -f https://github.com/kubernetes-sigs/metrics-server/releases/latest/download/components.yaml
```

```yaml
# Resources requests requis
resources:
  requests:
    cpu: "500m"
```

---

**2. Deployment**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
spec:
  replicas: 3
  template:
    spec:
      containers:
      - name: app
        image: order-service:1.0
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
```

---

**3. HPA basique**

```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: order-service-hpa
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  minReplicas: 3
  maxReplicas: 10
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
```

**Commande :**
```bash
kubectl autoscale deployment order-service \
  --cpu-percent=70 \
  --min=3 \
  --max=10
```

---

**4. HPA avec behavior**

```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: order-service-hpa
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  minReplicas: 3
  maxReplicas: 10
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
  
  behavior:
    scaleUp:
      stabilizationWindowSeconds: 0
      policies:
      - type: Percent
        value: 100
        periodSeconds: 15
    scaleDown:
      stabilizationWindowSeconds: 300
      policies:
      - type: Pods
        value: 1
        periodSeconds: 60
```

**Avantages :**
- Scale up rapide
- Scale down lent (évite flapping)

---

**5. PodDisruptionBudget**

```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: order-service-pdb
spec:
  minAvailable: 2
  selector:
    matchLabels:
      app: order-service
```

---

**Vérification :**

```bash
# Status
kubectl get hpa
# NAME                TARGETS    MINPODS   MAXPODS   REPLICAS
# order-service-hpa   45%/70%    3         10        3

# Détails
kubectl describe hpa order-service-hpa
```

---

**Best Practices :**

✅ **minReplicas ≥ 2**
```yaml
minReplicas: 3
```

✅ **Target 60-80%**
```yaml
averageUtilization: 70
```

✅ **Stabilization window**
```yaml
stabilizationWindowSeconds: 300
```

✅ **PodDisruptionBudget**
```yaml
minAvailable: 2
```

---

## 🎯 Récapitulatif - Chapitre 8

### Points clés maîtrisés :

**Requests & Limits :**
- ✅ Différence requests vs limits
- ✅ CPU throttling vs OOMKilled
- ✅ Sizing Spring Boot
- ✅ JVM tuning (Xmx = 75%)

**QoS Classes :**
- ✅ Guaranteed / Burstable / BestEffort
- ✅ Ordre d'éviction
- ✅ Use cases

**Namespace Policies :**
- ✅ LimitRanges (défauts + min/max)
- ✅ ResourceQuotas (totaux)
- ✅ Isolation multi-tenancy

**Autoscaling :**
- ✅ HPA configuration
- ✅ Behavior policies
- ✅ PodDisruptionBudget
- ✅ VPA recommandations

**Prêt pour l'entretien ! 🎯**

---

# Chapitre 8 : Questions/Réponses d'Entretien - Resource Management

## 📊 REQUESTS & LIMITS

### Q1 : Expliquez la différence entre requests et limits. Que se passe-t-il si un Pod dépasse sa limit ?

**Réponse :**

**Requests (Minimum garanti) :**
- Ressources **garanties** par Kubernetes
- Utilisé pour le **scheduling** (placement du Pod)
- Le Pod ne démarre que sur un Node ayant assez de ressources disponibles

**Limits (Maximum autorisé) :**
- Le Pod ne peut **jamais dépasser** ces limites
- Comportement différent selon la ressource

---

**Exemple :**
```yaml
resources:
  requests:
    memory: "512Mi"    # Minimum garanti
    cpu: "500m"        # 0.5 CPU garanti
  limits:
    memory: "1Gi"      # Maximum 1Gi
    cpu: "1000m"       # Maximum 1 CPU
```

**Comportement :**
- Kubernetes place le Pod sur un Node ayant au moins **512Mi + 500m** disponibles
- Le Pod reçoit **au minimum** 512Mi et 500m
- Le Pod peut utiliser **jusqu'à** 1Gi et 1000m

---

**Si dépassement CPU limit :**

```yaml
resources:
  limits:
    cpu: "500m"  # 0.5 CPU max
```

**→ CPU Throttling (ralentissement)**
- Pod essaie d'utiliser 1 CPU
- Kubernetes limite à 0.5 CPU
- Application plus lente, mais **Pod pas tué**

**Vérifier throttling :**
```bash
kubectl exec my-pod -- cat /sys/fs/cgroup/cpu/cpu.stat
# nr_throttled 50000  # Throttled 50% du temps !
```

**Impact :**
- ⚠️ Latence augmentée
- ⚠️ Timeouts possibles
- ⚠️ Throughput réduit

**Solution :**
```yaml
resources:
  limits:
    cpu: "1000m"  # Augmenter
```

---

**Si dépassement Memory limit :**

```yaml
resources:
  limits:
    memory: "512Mi"
```

**→ OOMKilled (Pod tué)**
- Pod consomme 513Mi
- Kubernetes tue le Pod immédiatement
- Exit code : 137
- Pod redémarre automatiquement

**Détecter :**
```bash
kubectl describe pod my-pod
# Last State: Terminated
#   Reason: OOMKilled
#   Exit Code: 137

kubectl get events | grep OOMKilled
```

**Solutions :**
```yaml
# 1. Augmenter limit
resources:
  limits:
    memory: "1Gi"

# 2. Optimiser JVM
env:
- name: JAVA_OPTS
  value: "-Xms256m -Xmx768m"

# 3. Fixer memory leaks dans le code
```

---

**Tableau récapitulatif :**

| Ressource | Dépassement Limit | Action Kubernetes | Impact |
|-----------|-------------------|-------------------|---------|
| **CPU** | > limit | **Throttling** (ralentissement) | App lente, pas de restart |
| **Memory** | > limit | **OOMKilled** (Pod tué) | Exit 137, restart automatique |

---

**Best Practices :**

✅ **Toujours définir requests ET limits**
```yaml
resources:
  requests:
    memory: "256Mi"
    cpu: "250m"
  limits:
    memory: "512Mi"
    cpu: "500m"
```

✅ **Ratio limits/requests raisonnable (1.5x - 2x)**
```yaml
requests:
  memory: "512Mi"
limits:
  memory: "1Gi"  # 2x requests
```

✅ **JVM : Xmx = 75% de memory limit**
```yaml
resources:
  limits:
    memory: "1Gi"
env:
- name: JAVA_OPTS
  value: "-Xmx768m"  # 75% de 1Gi
```

❌ **Éviter limits trop larges**
```yaml
# ❌ Mauvais
requests:
  memory: "256Mi"
limits:
  memory: "10Gi"  # 40x requests !
```

---

### Q2 : Comment dimensionnez-vous les resources (requests/limits) pour une application Spring Boot ?

**Réponse :**

**Approche méthodique en 5 étapes :**

---

**1. Profiling de l'application**

**A. Environnement local/dev**

```bash
# Lancer l'app avec monitoring
java -Xms256m -Xmx1g \
  -XX:+PrintGCDetails \
  -XX:+PrintGCDateStamps \
  -Xloggc:gc.log \
  -jar app.jar

# Générer du load
ab -n 10000 -c 100 http://localhost:8080/api/orders

# Ou JMeter, Gatling
```

**Observer avec VisualVM ou JConsole :**
- Heap usage
- CPU usage
- Thread count
- GC pauses

**B. Métriques observées**

```
Baseline (idle):
- Heap: 200Mi
- CPU: 50m (5%)
- Threads: 20

Under normal load (100 req/s):
- Heap: 600Mi
- CPU: 800m (80%)
- Threads: 50

Peak load (500 req/s):
- Heap: 900Mi
- CPU: 1500m (150%)
- Threads: 100
```

---

**2. Calculer les resources**

**Formula générale :**

```yaml
requests:
  # Memory: normal load + buffer
  memory: normal_load * 1.2
  
  # CPU: average under normal load
  cpu: normal_cpu * 1.2

limits:
  # Memory: peak + buffer
  memory: peak * 1.3
  
  # CPU: peak + buffer
  cpu: peak_cpu * 1.2
```

**Exemple de calcul :**

```
Observé:
- Normal load heap: 600Mi
- Peak heap: 900Mi
- Normal CPU: 800m
- Peak CPU: 1500m

Calcul:
requests.memory = 600Mi * 1.2 = 720Mi → arrondir 768Mi
requests.cpu = 800m * 1.2 = 960m → arrondir 1000m

limits.memory = 900Mi * 1.3 = 1170Mi → arrondir 1280Mi (1.25Gi)
limits.cpu = 1500m * 1.2 = 1800m → arrondir 2000m
```

**Configuration finale :**
```yaml
resources:
  requests:
    memory: "768Mi"
    cpu: "1000m"
  limits:
    memory: "1280Mi"
    cpu: "2000m"

env:
- name: JAVA_OPTS
  value: "-Xms512m -Xmx960m -XX:+UseG1GC"  # 75% de 1280Mi
```

---

**3. Sizing par type d'application**

**A. REST API légère (CRUD simple)**
```yaml
resources:
  requests:
    memory: "256Mi"
    cpu: "250m"
  limits:
    memory: "512Mi"
    cpu: "500m"

env:
- name: JAVA_OPTS
  value: "-Xms128m -Xmx384m"
```

**Exemples :**
- API Gateway
- Service de configuration
- Health check service

---

**B. Microservice standard (avec DB, cache)**
```yaml
resources:
  requests:
    memory: "512Mi"
    cpu: "500m"
  limits:
    memory: "1Gi"
    cpu: "1000m"

env:
- name: JAVA_OPTS
  value: "-Xms256m -Xmx768m -XX:+UseG1GC"
```

**Exemples :**
- Order service
- User service
- Product catalog
- Payment service

---

**C. Application intensive (traitement lourd)**
```yaml
resources:
  requests:
    memory: "1Gi"
    cpu: "1000m"
  limits:
    memory: "2Gi"
    cpu: "2000m"

env:
- name: JAVA_OPTS
  value: "-Xms512m -Xmx1536m -XX:+UseG1GC -XX:MaxGCPauseMillis=200"
```

**Exemples :**
- Batch processing
- Report generation
- Data analytics
- ETL jobs

---

**D. Application avec cache volumineux**
```yaml
resources:
  requests:
    memory: "2Gi"
    cpu: "1000m"
  limits:
    memory: "4Gi"
    cpu: "2000m"

env:
- name: JAVA_OPTS
  value: "-Xms1g -Xmx3g -XX:+UseG1GC"
```

**Exemples :**
- Service avec Ehcache/Caffeine
- Session management
- Reference data service
- In-memory database

---

**4. Déployer et monitorer**

```bash
# Déployer en staging
kubectl apply -f deployment.yaml

# Observer pendant 1-2 jours
kubectl top pods -n staging --sort-by=memory
kubectl top pods -n staging --sort-by=cpu

# Vérifier métriques Prometheus
```

**Queries Prometheus :**
```promql
# Memory usage vs limit (%)
container_memory_working_set_bytes{pod=~"order-service.*"} /
container_spec_memory_limit_bytes{pod=~"order-service.*"} * 100

# CPU usage vs limit (%)
rate(container_cpu_usage_seconds_total{pod=~"order-service.*"}[5m]) /
container_spec_cpu_quota{pod=~"order-service.*"} * 100

# Memory requests vs actual usage
container_memory_working_set_bytes{pod=~"order-service.*"} /
kube_pod_container_resource_requests{resource="memory"} * 100

# CPU throttling
rate(container_cpu_cfs_throttled_seconds_total[5m])
```

---

**5. Ajuster selon observations**

**Scénarios d'ajustement :**

**A. Memory toujours à 90% de limit**
```yaml
# ❌ Avant
limits:
  memory: "512Mi"  # Utilisé: 460Mi (90%)

# ✅ Après
limits:
  memory: "1Gi"  # Buffer suffisant
```

**B. CPU throttling constant**
```bash
# Vérifier throttling
kubectl exec pod -- cat /sys/fs/cgroup/cpu/cpu.stat
# nr_throttled: 50000  # Élevé !

# Solution
limits:
  cpu: "1000m"  # Augmenter de 500m à 1000m
```

**C. Resources gaspillées**
```yaml
# ❌ Avant
requests:
  memory: "2Gi"  # Utilisé: 400Mi (20%)
  cpu: "1000m"   # Utilisé: 200m (20%)

# ✅ Après
requests:
  memory: "512Mi"  # Plus réaliste
  cpu: "250m"
```

**D. OOMKilled fréquents**
```bash
# Vérifier events
kubectl get events | grep OOMKilled

# Analyser heap dumps
kubectl exec pod -- jmap -heap 1

# Solutions:
# 1. Augmenter memory limit
# 2. Fixer memory leaks
# 3. Optimiser code
```

---

**Outils de sizing :**

**1. Vertical Pod Autoscaler (VPA)**
```yaml
apiVersion: autoscaling.k8s.io/v1
kind: VerticalPodAutoscaler
metadata:
  name: order-service-vpa
spec:
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  updatePolicy:
    updateMode: "Off"  # Recommandations uniquement
```

```bash
# Voir recommandations
kubectl describe vpa order-service-vpa
# Target:
#   Cpu:     500m
#   Memory:  800Mi
```

**2. Goldilocks (UI pour VPA)**
```bash
helm install goldilocks fairwinds-stable/goldilocks
# Dashboard avec recommandations
```

---

**Best Practices :**

✅ **Commencer conservateur**
```yaml
# Mieux surestimer au début
requests:
  memory: "1Gi"  # Réduire après observation
```

✅ **Monitoring continu**
```yaml
# Alertes Prometheus
- Memory usage > 85% limit
- CPU throttling > 10%
- OOMKilled events
```

✅ **Documentation**
```yaml
metadata:
  annotations:
    "sizing-rationale": "Based on load test 100 req/s, observed peak 900Mi"
    "last-reviewed": "2024-11-18"
    "environment": "prod"
```

✅ **Tests de charge avant prod**
```bash
# Staging
ab -n 100000 -c 200 http://staging/api/orders

# Ou Gatling
gatling:test -Dusers=500 -Dduration=300
```

✅ **Review régulier (monthly)**
```bash
# Script d'analyse
#!/bin/bash
kubectl top pods --all-namespaces | awk '{print $1,$2,$3,$4}' | sort -k4 -hr | head -20
```

---

## 🎯 QOS & ÉVICTION

### Q3 : Qu'est-ce que la QoS (Quality of Service) class et comment Kubernetes l'utilise-t-il ?

**Réponse :**

**QoS Class** : Priorité assignée par Kubernetes à un Pod basée sur ses resources.

**3 classes (par ordre de priorité) :**

---

**1. Guaranteed (priorité HAUTE)**

**Conditions :**
- **Tous** les containers ont requests ET limits
- requests = limits (CPU ET Memory)

```yaml
resources:
  requests:
    memory: "512Mi"
    cpu: "500m"
  limits:
    memory: "512Mi"  # = requests
    cpu: "500m"      # = requests
```

**Vérifier :**
```bash
kubectl get pod my-pod -o jsonpath='{.status.qosClass}'
# Guaranteed

# Ou via describe
kubectl describe pod my-pod | grep "QoS Class"
# QoS Class: Guaranteed
```

**Caractéristiques :**
- ✅ **Jamais évicté** (sauf dépassement de limit)
- ✅ Ressources garanties à 100%
- ✅ Priorité maximale lors de scheduling
- ✅ Performance prévisible

**Use case :**
- Databases (PostgreSQL, MySQL, MongoDB)
- Caches (Redis, Memcached)
- Message queues (RabbitMQ, Kafka)
- Applications critiques temps-réel

**Exemple complet :**
```yaml
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: postgres
spec:
  serviceName: postgres
  replicas: 3
  template:
    spec:
      containers:
      - name: postgres
        image: postgres:15
        resources:
          requests:
            memory: "2Gi"
            cpu: "1000m"
          limits:
            memory: "2Gi"
            cpu: "1000m"
```

---

**2. Burstable (priorité MOYENNE)**

**Conditions :**
- Au moins 1 container a requests OU limits
- requests ≠ limits

```yaml
# Exemple 1: requests < limits
resources:
  requests:
    memory: "256Mi"
    cpu: "250m"
  limits:
    memory: "512Mi"  # ≠ requests
    cpu: "500m"

# Exemple 2: seulement requests
resources:
  requests:
    memory: "256Mi"
    cpu: "250m"
  # Pas de limits

# Exemple 3: seulement limits
resources:
  limits:
    memory: "512Mi"
    cpu: "500m"
  # Pas de requests (requests = limits par défaut)
```

**Vérifier :**
```bash
kubectl get pod my-pod -o jsonpath='{.status.qosClass}'
# Burstable
```

**Caractéristiques :**
- ⚠️ Évicté si Node sous pression ET Pod dépasse requests
- ✅ Peut utiliser plus que requests (jusqu'à limits)
- ✅ Flexible pour bursts de trafic
- ⚠️ Performance moins prévisible

**Use case :**
- Microservices REST
- Applications web
- Workers/batch jobs
- Services non critiques

**Exemple complet :**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
spec:
  replicas: 3
  template:
    spec:
      containers:
      - name: app
        image: order-service:1.0
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
```

---

**3. BestEffort (priorité BASSE)**

**Conditions :**
- **Aucun** container n'a requests ni limits

```yaml
resources: {}  # Rien défini
```

**Vérifier :**
```bash
kubectl get pod my-pod -o jsonpath='{.status.qosClass}'
# BestEffort
```

**Caractéristiques :**
- ❌ **Premier évicté** en cas de pressure
- ❌ Aucune garantie de ressources
- ❌ Performance imprévisible
- ❌ À éviter en production

**Use case :**
- Dev/test environments uniquement
- Jobs non critiques temporaires
- Debug pods

**Exemple :**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: debug-pod
spec:
  containers:
  - name: debug
    image: busybox
    command: ["sleep", "3600"]
    # Pas de resources
```

---

**Éviction lors de resource pressure**

**Scénario : Node en memory pressure**

```
Node: 8Gi memory total

Pods running:
1. postgres (Guaranteed)      2Gi     (requests=limits)
2. api-service (Burstable)    1.5Gi   (requests: 1Gi, limits: 2Gi)
3. worker (Burstable)         1Gi     (requests: 512Mi, limits: 1.5Gi)
4. debug-pod (BestEffort)     500Mi   (no limits)

Total utilisé: 5Gi / 8Gi
```

**Node atteint 7.5Gi → Memory pressure detected !**

**Ordre d'éviction par Kubernetes :**

```
1. debug-pod (BestEffort)     ← Évicté EN PREMIER
   Reason: Lowest priority QoS class

2. worker (Burstable)         ← Évicté si pas assez de mémoire libérée
   Reason: Dépasse le plus ses requests (1Gi utilisé, 512Mi requests)

3. api-service (Burstable)    ← Évicté en dernier recours
   Reason: Dépasse moins ses requests (1.5Gi utilisé, 1Gi requests)

4. postgres (Guaranteed)      ← JAMAIS évicté
   Exception: Seulement si OOMKilled (dépasse sa limit)
```

**Events Kubernetes :**
```bash
kubectl get events --sort-by='.lastTimestamp'

# Output:
# LAST SEEN   TYPE      REASON    OBJECT           MESSAGE
# 1m          Warning   Evicted   pod/debug-pod    Pod was evicted due to memory pressure
# 30s         Warning   Evicted   pod/worker-xxx   Pod was evicted due to memory pressure
```

---

**Comparaison détaillée :**

| Aspect | Guaranteed | Burstable | BestEffort |
|--------|-----------|-----------|------------|
| **Priorité éviction** | Jamais | Moyenne | Première |
| **Ressources** | 100% garanties | Partielles | Aucune |
| **Performance** | Prévisible | Variable | Imprévisible |
| **Use case** | Critical | Standard | Dev/test |
| **Production** | ✅ Recommandé | ✅ Acceptable | ❌ Non |

---

**Recommandations par environnement :**

**Production :**
```yaml
# Workloads critiques → Guaranteed
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: postgres
spec:
  template:
    spec:
      containers:
      - name: postgres
        resources:
          requests:
            memory: "2Gi"
            cpu: "1000m"
          limits:
            memory: "2Gi"
            cpu: "1000m"

# Microservices → Burstable
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
spec:
  template:
    spec:
      containers:
      - name: app
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
```

**Dev/Staging :**
```yaml
# Burstable acceptable
resources:
  requests:
    memory: "256Mi"
    cpu: "250m"
  limits:
    memory: "512Mi"
    cpu: "500m"

# BestEffort pour debug uniquement
resources: {}
```

---

**Monitoring QoS :**

```bash
# Lister Pods par QoS class
kubectl get pods --all-namespaces -o custom-columns=\
NAME:.metadata.name,\
NAMESPACE:.metadata.namespace,\
QOS:.status.qosClass

# Compter par QoS class
kubectl get pods --all-namespaces -o json | \
  jq -r '.items[] | .status.qosClass' | sort | uniq -c

# Output:
#   3 BestEffort
#  45 Burstable
#  12 Guaranteed
```

**Prometheus metrics :**
```promql
# Pods par QoS class
count(kube_pod_status_qos_class) by (qos_class)

# Alertes
- alert: TooManyBestEffortPods
  expr: |
    count(kube_pod_status_qos_class{qos_class="BestEffort", namespace="prod"}) > 5
  annotations:
    summary: "Too many BestEffort pods in production"
```

---

### Q4 : Comment gérez-vous les LimitRanges et ResourceQuotas dans un cluster multi-tenant ?

**Réponse :**

**Stratégie complète pour isolation des équipes :**

---

**1. Architecture multi-tenant**

```
Cluster
├── Namespace: team-a
│   ├── LimitRange (defaults + constraints)
│   ├── ResourceQuota (total limits)
│   └── NetworkPolicy (isolation réseau)
├── Namespace: team-b
│   ├── LimitRange
│   ├── ResourceQuota
│   └── NetworkPolicy
└── Namespace: shared-services
    ├── LimitRange
    └── ResourceQuota
```

---

**2. LimitRange par namespace**

**A. Team A (équipe backend - généreuse)**

```yaml
apiVersion: v1
kind: LimitRange
metadata:
  name: team-a-limits
  namespace: team-a
spec:
  limits:
  # Container limits
  - type: Container
    default:  # Si Pod ne définit pas limits
      memory: "1Gi"
      cpu: "1000m"
    defaultRequest:  # Si Pod ne définit pas requests
      memory: "512Mi"
      cpu: "500m"
    max:  # Maximum autorisé par container
      memory: "4Gi"
      cpu: "4000m"
    min:  # Minimum requis
      memory: "128Mi"
      cpu: "100m"
    maxLimitRequestRatio:  # Ratio max
      memory: 2
      cpu: 2
  
  # Pod limits (somme de tous containers)
  - type: Pod
    max:
      memory: "8Gi"
      cpu: "8000m"
  
  # PVC limits
  - type: PersistentVolumeClaim
    max:
      storage: "50Gi"
    min:
      storage: "1Gi"
```

**B. Team B (équipe frontend - limitée)**

```yaml
apiVersion: v1
kind: LimitRange
metadata:
  name: team-b-limits
  namespace: team-b
spec:
  limits:
  - type: Container
    default:
      memory: "512Mi"
      cpu: "500m"
    defaultRequest:
      memory: "256Mi"
      cpu: "250m"
    max:
      memory: "2Gi"
      cpu: "2000m"
    min:
      memory: "64Mi"
      cpu: "50m"
    maxLimitRequestRatio:
      memory: 1.5
      cpu: 1.5
```

---

**3. ResourceQuota par namespace**

**A. Team A Quota**

```yaml
apiVersion: v1
kind: ResourceQuota
metadata:
  name: team-a-quota
  namespace: team-a
spec:
  hard:
    # Compute resources
    requests.cpu: "20"           # 20 CPUs total requests
    requests.memory: 40Gi        # 40Gi total requests
    limits.cpu: "40"             # 40 CPUs total limits
    limits.memory: 80Gi          # 80Gi total limits
    
    # Object count
    pods: "50"                   # Max 50 Pods
    services: "20"
    configmaps: "30"
    secrets: "30"
    persistentvolumeclaims: "20"
    
    # Storage
    requests.storage: "500Gi"    # 500Gi storage total
    
    # Specific QoS
    count/pods: "50"
```

**B. Team B Quota (plus restrictif)**

```yaml
apiVersion: v1
kind: ResourceQuota
metadata:
  name: team-b-quota
  namespace: team-b
spec:
  hard:
    requests.cpu: "10"
    requests.memory: 20Gi
    limits.cpu: "20"
    limits.memory: 40Gi
    
    pods: "30"
    services: "10"
    configmaps: "20"
    secrets: "20"
    persistentvolumeclaims: "10"
    
    requests.storage: "200Gi"
```

---

**4. Quota par QoS class**

```yaml
# Team A - BestEffort quota (limité)
apiVersion: v1
kind: ResourceQuota
metadata:
  name: team-a-besteffort-quota
  namespace: team-a
spec:
  hard:
    pods: "5"  # Max 5 Pods BestEffort
  scopes:
  - BestEffort
---
# Team A - Non-BestEffort quota
apiVersion: v1
kind: ResourceQuota
metadata:
  name: team-a-guaranteed-burstable-quota
  namespace: team-a
spec:
  hard:
    requests.cpu: "20"
    requests.memory: 40Gi
  scopes:
  - NotBestEffort  # Guaranteed + Burstable uniquement
```

---

**5. Vérification et monitoring**

```bash
# Voir LimitRanges
kubectl get limitranges -n team-a
kubectl describe limitrange team-a-limits -n team-a

# Voir ResourceQuotas
kubectl get resourcequotas -n team-a
kubectl describe resourcequota team-a-quota -n team-a

# Output:
# Resource                    Used    Hard
# --------                    ----    ----
# limits.cpu                  25      40
# limits.memory               50Gi    80Gi
# pods                        35      50
# requests.cpu                15      20     ← 75% utilisé
# requests.memory             30Gi    40Gi   ← 75% utilisé
# requests.storage            300Gi   500Gi
```

---

**6. Gestion des dépassements**

**Scénario : Team A tente de créer un Pod qui dépasse quota**

```bash
kubectl apply -f deployment.yaml -n team-a

# Error:
# Error from server (Forbidden): error when creating "deployment.yaml":
# pods "order-service-xxx" is forbidden: exceeded quota: team-a-quota,
# requested: requests.cpu=2, used: requests.cpu=19, limited: requests.cpu=20
```

**Solutions :**

**A. Supprimer des Pods inutilisés**
```bash
# Lister Pods par utilisation CPU
kubectl top pods -n team-a --sort-by=cpu

# Supprimer Pods peu utilisés
kubectl delete deployment old-service -n team-a
```

**B. Optimiser resources des Pods existants**
```yaml
# Réduire requests
resources:
  requests:
    cpu: "250m"  # Au lieu de 500m
```

**C. Demander augmentation quota**
```yaml
# Après justification
spec:
  hard:
    requests.cpu: "30"  # Augmenté de 20 à 30
```

---

**7. Alerting sur quotas**

```yaml
# Prometheus alerts
groups:
- name: resource-quotas
  rules:
  
  # Alert: Quota proche limite
  - alert: QuotaNearLimit
    expr: |
      kube_resourcequota{resource="requests.cpu", type="used"} /
      kube_resourcequota{resource="requests.cpu", type="hard"} > 0.9
    for: 15m
    labels:
      severity: warning
      team: "{{ $labels.namespace }}"
    annotations:
      summary: "Quota CPU at 90% in {{ $labels.namespace }}"
      description: "Team {{ $labels.namespace }} using {{ $value | humanizePercentage }} of CPU quota"
  
  # Alert: Quota atteint
  - alert: QuotaExceeded
    expr: |
      kube_resourcequota{type="used"} >=
      kube_resourcequota{type="hard"}
    labels:
      severity: critical
    annotations:
      summary: "Quota exceeded in {{ $labels.namespace }}"
```

---

**8. Dashboard Grafana**

```json
{
  "dashboard": {
    "title": "Resource Quotas - Multi-tenant",
    "panels": [
      {
        "title": "CPU Quota Usage by Namespace",
        "targets": [
          {
            "expr": "kube_resourcequota{resource='requests.cpu', type='used'} / kube_resourcequota{resource='requests.cpu', type='hard'} * 100"
          }
        ]
      },
      {
        "title": "Memory Quota Usage by Namespace",
        "targets": [
          {
            "expr": "kube_resourcequota{resource='requests.memory', type='used'} / kube_resourcequota{resource='requests.memory', type='hard'} * 100"
          }
        ]
      },
      {
        "title": "Pod Count vs Quota",
        "targets": [
          {
            "expr": "kube_resourcequota{resource='pods', type='used'}",
            "legendFormat": "Used"
          },
          {
            "expr": "kube_resourcequota{resource='pods', type='hard'}",
            "legendFormat": "Limit"
          }
        ]
      }
    ]
  }
}
```

---

**9. Best Practices**

✅ **LimitRange dans chaque namespace**
```yaml
# Valeurs par défaut + contraintes
```

✅ **ResourceQuota pour isolation**
```yaml
# Évite qu'une équipe consomme tout
```

✅ **Monitoring et alertes**
```yaml
# Alert à 90% quota
```

✅ **Documentation pour équipes**
```markdown
# Team A - Resource Limits

## Current Quotas
- CPU: 20 cores (requests), 40 cores (limits)
- Memory: 40Gi (requests), 80Gi (limits)
- Pods: 50 max

## How to check usage
kubectl describe resourcequota team-a-quota -n team-a

## Request increase
Open ticket with justification
```

✅ **Review régulier (quarterly)**
```bash
# Analyser utilisation réelle
kubectl top pods -n team-a
kubectl describe quota team-a-quota -n team-a
```

---

## 🚀 AUTOSCALING

### Q5 : Comment configurez-vous l'autoscaling (HPA) pour une application Spring Boot ?

**Réponse :**

**Configuration complète en 6 étapes :**

---

**1. Prérequis**

**A. Metrics Server**
```bash
# Installation
kubectl apply -f https://github.com/kubernetes-sigs/metrics-server/releases/latest/download/components.yaml

# Vérifier
kubectl get deployment metrics-server -n kube-system
kubectl top nodes  # Doit fonctionner
```

**B. Resources requests définis**
```yaml
# HPA ne fonctionne pas sans requests !
resources:
  requests:
    cpu: "500m"    # Requis pour HPA CPU
    memory: "512Mi"
```

---

**2. Deployment avec resources**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: order-service
  namespace: prod
spec:
  replicas: 3  # Sera override par HPA
  selector:
    matchLabels:
      app: order-service
  template:
    metadata:
      labels:
        app: order-service
    spec:
      containers:
      - name: app
        image: order-service:1.0
        
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"      # ← Requis pour HPA
          limits:
            memory: "1Gi"
            cpu: "1000m"
        
        ports:
        - containerPort: 8080
        
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8080
          periodSeconds: 10
        
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          periodSeconds: 5
```

---

**3. HPA basique (CPU)**

**YAML :**
```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: order-service-hpa
  namespace: prod
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  
  minReplicas: 3
  maxReplicas: 10
  
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70  # Target 70% CPU
```

**Commande kubectl :**
```bash
kubectl autoscale deployment order-service \
  --cpu-percent=70 \
  --min=3 \
  --max=10 \
  -n prod
```

**Comportement :**
- Si CPU moyen > 70% → Scale up
- Si CPU moyen < 70% → Scale down
- Min : 3 replicas
- Max : 10 replicas

---

**4. HPA avec métriques multiples**

```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: order-service-hpa
  namespace: prod
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  
  minReplicas: 3
  maxReplicas: 20
  
  metrics:
  # CPU
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
  
  # Memory
  - type: Resource
    resource:
      name: memory
      target:
        type: Utilization
        averageUtilization: 80
```

**Comportement :**
- Scale up si **CPU > 70% OU Memory > 80%**
- Scale down si **CPU < 70% ET Memory < 80%**

---

**5. HPA avec métriques custom (Prometheus)**

**A. Exposer métrique dans Spring Boot**

```java
@Service
@Slf4j
public class OrderService {
    
    private final MeterRegistry meterRegistry;
    private final Counter requestCounter;
    
    public OrderService(MeterRegistry meterRegistry) {
        this.meterRegistry = meterRegistry;
        
        this.requestCounter = Counter.builder("http_requests_total")
            .description("Total HTTP requests")
            .tag("endpoint", "/api/orders")
            .register(meterRegistry);
    }
    
    @PostMapping("/orders")
    public Order createOrder(@RequestBody OrderRequest request) {
        requestCounter.increment();
        return processOrder(request);
    }
}
```

**B. HPA sur métrique custom**

```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: order-service-hpa
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  
  minReplicas: 3
  maxReplicas: 20
  
  metrics:
  # CPU standard
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
  
  # Métrique custom: requests per second
  - type: Pods
    pods:
      metric:
        name: http_requests_per_second
      target:
        type: AverageValue
        averageValue: "100"  # 100 req/s par Pod
  
  # Métrique external: Queue depth
  - type: External
    external:
      metric:
        name: rabbitmq_queue_messages_ready
        selector:
          matchLabels:
            queue: orders
      target:
        type: AverageValue
        averageValue: "30"  # 30 messages par Pod
```

---

**6. Behavior Configuration (éviter flapping)**

```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: order-service-hpa
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  
  minReplicas: 3
  maxReplicas: 10
  
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
  
  behavior:
    scaleUp:
      stabilizationWindowSeconds: 0  # Scale up immédiatement
      policies:
      - type: Percent
        value: 100      # Double le nombre de Pods
        periodSeconds: 15
      - type: Pods
        value: 4        # Ou +4 Pods maximum
        periodSeconds: 15
      selectPolicy: Max  # Prendre le plus agressif
    
    scaleDown:
      stabilizationWindowSeconds: 300  # Attendre 5 min avant scale down
      policies:
      - type: Percent
        value: 50       # Retire max 50% des Pods
        periodSeconds: 60
      - type: Pods
        value: 2        # Ou retire 2 Pods maximum
        periodSeconds: 60
      selectPolicy: Min  # Prendre le plus conservateur
```

**Explications :**
- **Scale up** : Rapide (double ou +4 Pods en 15s)
- **Scale down** : Lent (retire max 50% ou 2 Pods en 60s)
- **Stabilization** : Attend 5 min avant scale down
- **Évite flapping** : Scale up/down répétitif

---

**7. PodDisruptionBudget (haute dispo)**

```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: order-service-pdb
  namespace: prod
spec:
  minAvailable: 2  # Toujours min 2 Pods disponibles
  selector:
    matchLabels:
      app: order-service
```

**Pourquoi :**
- Évite downtime lors de drain/eviction
- Compatible avec HPA
- Garantit disponibilité minimale

---

**8. Vérification et monitoring**

```bash
# Status HPA
kubectl get hpa -n prod

# Output:
# NAME                REFERENCE              TARGETS    MINPODS   MAXPODS   REPLICAS   AGE
# order-service-hpa   Deployment/order-service   45%/70%    3         10        3          5d

# Détails
kubectl describe hpa order-service-hpa -n prod

# Output:
# Metrics:
#   resource cpu on pods (as a percentage of request):  45% (225m) / 70%
# Min replicas:  3
# Max replicas:  10
# Deployment pods:  3 current / 3 desired
# Conditions:
#   Type            Status  Reason              Message
#   ----            ------  ------              -------
#   AbleToScale     True    ReadyForNewScale    recommended size matches current size
#   ScalingActive   True    ValidMetricFound    the HPA was able to successfully calculate
# Events:
#   Type    Reason             Age   From                       Message
#   ----    ------             ----  ----                       -------
#   Normal  SuccessfulRescale  5m    horizontal-pod-autoscaler  New size: 5; reason: cpu resource utilization above target

# Voir events
kubectl get events -n prod --field-selector involvedObject.name=order-service-hpa

# Watch en temps réel
watch -n 2 'kubectl get hpa -n prod && kubectl top pods -n prod -l app=order-service'
```

---

**9. Timeline exemple (test de charge)**

```
T=0     : 3 Pods, CPU 30% → OK
T=10    : Load test démarre (500 req/s)
T=15    : CPU monte à 85% → HPA déclenche scale up
T=17    : HPA calcule: need 5 Pods (behavior: +100% = double)
T=20    : 5 Pods Running, CPU 55% → Stable
T=30    : Load augmente (1000 req/s), CPU 90%
T=32    : HPA scale to 8 Pods
T=35    : 8 Pods Running, CPU 60% → Stable
T=60    : Load test terminé, CPU chute à 25%
T=360   : Après 5 min stabilité (stabilizationWindow)
T=362   : Scale down to 6 Pods (retire 2 Pods max)
T=480   : Scale down to 4 Pods
T=600   : Scale down to 3 Pods (minReplicas atteint)
```

---

**10. Prometheus metrics et alertes**

```promql
# Current replicas
kube_horizontalpodautoscaler_status_current_replicas{hpa="order-service-hpa"}

# Desired replicas
kube_horizontalpodautoscaler_status_desired_replicas{hpa="order-service-hpa"}

# Min/Max
kube_horizontalpodautoscaler_spec_min_replicas
kube_horizontalpodautoscaler_spec_max_replicas

# CPU target vs actual
kube_horizontalpodautoscaler_status_current_metrics_average_utilization{metric_name="cpu"}
```

**Alertes :**
```yaml
groups:
- name: hpa-alerts
  rules:
  
  # Alert: Max replicas atteint
  - alert: HPAMaxReplicasReached
    expr: |
      kube_horizontalpodautoscaler_status_current_replicas ==
      kube_horizontalpodautoscaler_spec_max_replicas
    for: 10m
    labels:
      severity: warning
    annotations:
      summary: "HPA {{ $labels.hpa }} at max replicas"
      description: "Consider increasing maxReplicas"
  
  # Alert: HPA ne peut pas scale
  - alert: HPAUnableToScale
    expr: |
      kube_horizontalpodautoscaler_status_condition{condition="AbleToScale", status="false"} == 1
    for: 5m
    labels:
      severity: critical
    annotations:
      summary: "HPA {{ $labels.hpa }} unable to scale"
  
  # Alert: Métriques indisponibles
  - alert: HPAMetricsUnavailable
    expr: |
      kube_horizontalpodautoscaler_status_condition{condition="ScalingActive", status="false"} == 1
    for: 5m
    labels:
      severity: warning
    annotations:
      summary: "HPA {{ $labels.hpa }} metrics unavailable"
```

---

**11. Best Practices**

✅ **minReplicas ≥ 2 (haute dispo)**
```yaml
minReplicas: 3  # Jamais 1 !
```

✅ **Target CPU 60-80%**
```yaml
averageUtilization: 70  # Bon compromis
```

✅ **Stabilization window pour scale down**
```yaml
behavior:
  scaleDown:
    stabilizationWindowSeconds: 300  # 5 min
```

✅ **PodDisruptionBudget**
```yaml
minAvailable: 2
```

✅ **Monitoring et alertes**
```yaml
# Alert max replicas
# Alert unable to scale
```

✅ **Tests de charge réguliers**
```bash
# Valider comportement HPA
ab -n 100000 -c 200 http://order-service/api/orders
```

❌ **Éviter minReplicas = 1**
```yaml
# Single point of failure
minReplicas: 1  # ❌
```

❌ **Éviter scale down trop rapide**
```yaml
stabilizationWindowSeconds: 0  # ❌ Flapping
```

❌ **Éviter target trop bas**
```yaml
averageUtilization: 30  # ❌ Gaspillage ressources
```

---

### Q6 : Expliquez la différence entre HPA (Horizontal Pod Autoscaler) et VPA (Vertical Pod Autoscaler). Quand utiliser l'un ou l'autre ?

**Réponse :**

**Différences fondamentales :**

---

**HPA (Horizontal Pod Autoscaler)**

**Principe :** Ajouter/retirer des **Pods** (→ scaling horizontal)

```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: order-service-hpa
spec:
  scaleTargetRef:
    kind: Deployment
    name: order-service
  minReplicas: 3
  maxReplicas: 10
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
```

**Comportement :**
```
CPU > 70%:
3 Pods → 5 Pods → 8 Pods

CPU < 70%:
8 Pods → 5 Pods → 3 Pods
```

**Caractéristiques :**
- ✅ Pas de restart des Pods
- ✅ Scale rapide
- ✅ Compatible avec stateless apps
- ✅ Améliore throughput
- ❌ Nécessite plus de ressources cluster

---

**VPA (Vertical Pod Autoscaler)**

**Principe :** Augmenter/diminuer **resources** des Pods (↕ scaling vertical)

```yaml
apiVersion: autoscaling.k8s.io/v1
kind: VerticalPodAutoscaler
metadata:
  name: order-service-vpa
spec:
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: order-service
  updatePolicy:
    updateMode: "Auto"
  resourcePolicy:
    containerPolicies:
    - containerName: app
      minAllowed:
        cpu: "250m"
        memory: "256Mi"
      maxAllowed:
        cpu: "2000m"
        memory: "4Gi"
```

**Comportement :**
```
Observé: Pod utilise 800Mi (requests: 512Mi)

VPA recommande:
requests:
  memory: "1Gi"    # Augmenté
  cpu: "750m"

VPA applique → Pod RESTART avec nouvelles resources
```

**Caractéristiques :**
- ⚠️ **Restart des Pods** pour appliquer
- ⚠️ Plus lent que HPA
- ✅ Bon pour stateful apps
- ✅ Optimise utilisation ressources
- ✅ Sizing automatique

---

**Tableau comparatif :**

| Aspect | HPA | VPA |
|--------|-----|-----|
| **Scaling** | Horizontal (+ Pods) | Vertical (+ resources) |
| **Restart** | Non | Oui ⚠️ |
| **Vitesse** | Rapide | Lent |
| **Use case** | Stateless | Stateful |
| **Métrique** | CPU, Memory, Custom | CPU, Memory |
| **Impact** | + Throughput | + Performance par Pod |

---

**Quand utiliser HPA ?**

✅ **Applications stateless**
```yaml
# Microservices REST
# API Gateway
# Workers asynchrones
```

**Exemple :**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: api-gateway
spec:
  replicas: 3  # Géré par HPA
  template:
    spec:
      containers:
      - name: gateway
        image: gateway:1.0
        resources:
          requests:
            cpu: "500m"
            memory: "512Mi"
---
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: api-gateway-hpa
spec:
  scaleTargetRef:
    kind: Deployment
    name: api-gateway
  minReplicas: 3
  maxReplicas: 20
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
```

**Avantages :**
- Pas de downtime (rolling scale)
- Réagit vite aux pics de trafic
- Haute disponibilité native

---

**Quand utiliser VPA ?**

✅ **Applications stateful**
```yaml
# Databases (StatefulSet)
# Caches
# Message queues
```

**Exemple :**
```yaml
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: postgres
spec:
  serviceName: postgres
  replicas: 3  # Fixe
  template:
    spec:
      containers:
      - name: postgres
        image: postgres:15
        resources:
          requests:
            cpu: "500m"      # VPA va ajuster
            memory: "1Gi"
---
apiVersion: autoscaling.k8s.io/v1
kind: VerticalPodAutoscaler
metadata:
  name: postgres-vpa
spec:
  targetRef:
    apiVersion: apps/v1
    kind: StatefulSet
    name: postgres
  updatePolicy:
    updateMode: "Auto"
  resourcePolicy:
    containerPolicies:
    - containerName: postgres
      minAllowed:
        cpu: "500m"
        memory: "1Gi"
      maxAllowed:
        cpu: "4000m"
        memory: "8Gi"
```

**Avantages :**
- Sizing automatique
- Optimise coûts
- Bon pour workloads prévisibles

---

**Peut-on combiner HPA + VPA ?**

⚠️ **Attention : Incompatibilité !**

**Conflit si les deux sur CPU/Memory :**
```yaml
# ❌ Problème
HPA: scale basé sur CPU utilization
VPA: ajuste CPU requests
→ Les deux se battent !
```

**Solutions :**

**1. HPA seul (recommandé pour stateless)**
```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: order-service-hpa
spec:
  scaleTargetRef:
    kind: Deployment
    name: order-service
  minReplicas: 3
  maxReplicas: 10
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
```

**2. VPA en mode "Off" (recommandations uniquement)**
```yaml
apiVersion: autoscaling.k8s.io/v1
kind: VerticalPodAutoscaler
metadata:
  name: order-service-vpa
spec:
  targetRef:
    kind: Deployment
    name: order-service
  updatePolicy:
    updateMode: "Off"  # Pas d'application automatique
```

```bash
# Voir recommandations
kubectl describe vpa order-service-vpa
# Target:
#   Cpu:     750m
#   Memory:  1Gi

# Appliquer manuellement après review
```

**3. HPA + VPA sur métriques différentes (avancé)**
```yaml
# HPA sur métrique custom
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: order-service-hpa
spec:
  metrics:
  - type: Pods
    pods:
      metric:
        name: http_requests_per_second
      target:
        type: AverageValue
        averageValue: "100"
---
# VPA ajuste CPU/Memory
apiVersion: autoscaling.k8s.io/v1
kind: VerticalPodAutoscaler
metadata:
  name: order-service-vpa
spec:
  updatePolicy:
    updateMode: "Auto"
```

**⚠️ Complexe, tester en staging d'abord !**

---

**Arbre de décision :**

```
Application stateless ?
├─ Oui → HPA
│  └─ Traffic variable ? → HPA + custom metrics
│
└─ Non (stateful) → VPA
   └─ Peut scale horizontalement ? → HPA + VPA mode Off
```

**Exemples concrets :**

| Application | Solution | Justification |
|-------------|----------|---------------|
| **API REST** | HPA | Stateless, traffic variable |
| **Worker batch** | HPA | Stateless, queue-based |
| **PostgreSQL** | VPA | Stateful, pas de scale horizontal |
| **Redis (single)** | VPA | Stateful, pas de sharding |
| **Redis (cluster)** | HPA | Peut scale horizontalement |
| **Kafka** | VPA | Stateful, rebalancing complexe |
| **Frontend React** | HPA | Stateless, CDN-friendly |

---

## 🎯 Récapitulatif - Chapitre 8

### Questions couvertes :

1. ✅ **Q1** : Différence requests vs limits, CPU throttling vs OOMKilled
2. ✅ **Q2** : Dimensionner resources pour Spring Boot (méthodologie 5 étapes)
3. ✅ **Q3** : QoS classes et éviction lors de resource pressure
4. ✅ **Q4** : LimitRanges et ResourceQuotas multi-tenant
5. ✅ **Q5** : Configuration HPA complète avec behavior policies
6. ✅ **Q6** : Différence HPA vs VPA, quand utiliser chacun

### Points clés maîtrisés :

**Requests & Limits :**
- ✅ Différence fondamentale
- ✅ CPU throttling (ralentissement)
- ✅ Memory OOMKilled (Exit 137)
- ✅ Sizing méthodique (profiling → calcul → monitoring)
- ✅ JVM tuning (Xmx = 75% limit)

**QoS Classes :**
- ✅ Guaranteed / Burstable / BestEffort
- ✅ Ordre d'éviction lors de pressure
- ✅ Use cases par type

**Namespace Policies :**
- ✅ LimitRanges (defaults + min/max)
- ✅ ResourceQuotas (limites totales)
- ✅ Isolation multi-tenancy
- ✅ Monitoring quotas

**Autoscaling :**
- ✅ HPA configuration complète
- ✅ Behavior policies (éviter flapping)
- ✅ Métriques custom (Prometheus)
- ✅ PodDisruptionBudget
- ✅ VPA vs HPA
- ✅ Quand utiliser l'un ou l'autre


---