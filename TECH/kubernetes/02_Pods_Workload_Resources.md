## [**..**](./00_index.md)

# Chapitre 2 : Pods & Workload Resources

## 2.1. Pods (Lifecycle, Multi-container Patterns)

### Qu'est-ce qu'un Pod ?

Un **Pod** est la plus petite unité déployable dans Kubernetes. C'est un groupe d'un ou plusieurs containers qui partagent :
- Le même namespace réseau (localhost)
- Le même namespace IPC
- Les volumes de stockage

```
┌─────────────────────────────────┐
│           POD                   │
│  ┌──────────┐    ┌──────────┐  │
│  │Container │    │Container │  │
│  │   App    │◄──►│  Sidecar │  │
│  └──────────┘    └──────────┘  │
│         │              │         │
│    ┌────▼──────────────▼────┐   │
│    │  Shared Volumes        │   │
│    └───────────────────────┘   │
│    Network: localhost          │
└─────────────────────────────────┘
```

### Pod Simple

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: nginx-pod
  labels:
    app: web
    environment: dev
spec:
  containers:
  - name: nginx
    image: nginx:1.21
    ports:
    - containerPort: 80
    resources:
      requests:
        memory: "128Mi"
        cpu: "100m"
      limits:
        memory: "256Mi"
        cpu: "200m"
```

```bash
# Créer le Pod
kubectl apply -f nginx-pod.yaml

# Vérifier le status
kubectl get pods
kubectl get pod nginx-pod -o wide

# Détails complets
kubectl describe pod nginx-pod

# Accéder au Pod
kubectl exec -it nginx-pod -- bash
```

---

### Lifecycle d'un Pod

```
Pending → Running → Succeeded/Failed
   │         │
   │         └─→ CrashLoopBackOff (si restart)
   │
   └─→ ImagePullBackOff (si erreur image)
```

**Phases :**

1. **Pending** : Accepté par K8s, en attente de scheduling
2. **Running** : Assigné à un Node, au moins un container actif
3. **Succeeded** : Tous les containers terminés avec succès
4. **Failed** : Au moins un container a échoué
5. **Unknown** : État indéterminé (problème kubelet)

**Status conditions :**
```bash
kubectl get pod nginx-pod -o jsonpath='{.status.conditions[*].type}'
# PodScheduled, Initialized, ContainersReady, Ready
```

---

### Multi-container Patterns

#### **1. Sidecar Pattern**
Container auxiliaire qui étend/améliore le container principal.

**Exemple : Log collector**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: app-with-logging
spec:
  containers:
  # Container principal
  - name: app
    image: myapp:1.0
    volumeMounts:
    - name: logs
      mountPath: /var/log/app
  
  # Sidecar pour les logs
  - name: log-collector
    image: fluentd:latest
    volumeMounts:
    - name: logs
      mountPath: /var/log/app
      readOnly: true
  
  volumes:
  - name: logs
    emptyDir: {}
```

**Cas d'usage :**
- Collecte de logs (Fluentd, Filebeat)
- Service mesh (Envoy, Istio)
- Monitoring agents

---

#### **2. Init Containers**
Containers qui s'exécutent **avant** les containers applicatifs.

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: myapp-pod
spec:
  # Init containers (ordre séquentiel)
  initContainers:
  - name: wait-for-db
    image: busybox:1.28
    command: ['sh', '-c', 'until nslookup database.prod.svc.cluster.local; do echo waiting for db; sleep 2; done']
  
  - name: setup-config
    image: busybox:1.28
    command: ['sh', '-c', 'echo "Config ready" > /config/app.conf']
    volumeMounts:
    - name: config
      mountPath: /config
  
  # Container principal
  containers:
  - name: myapp
    image: myapp:1.0
    volumeMounts:
    - name: config
      mountPath: /config
  
  volumes:
  - name: config
    emptyDir: {}
```

**Caractéristiques :**
- S'exécutent en séquence (un après l'autre)
- Doivent se terminer avec succès avant le démarrage des containers principaux
- Utilisent les mêmes volumes que le Pod

**Cas d'usage :**
- Attendre des dépendances (DB, services)
- Cloner un repo Git
- Initialiser des fichiers de configuration
- Setup de sécurité (permissions)

---

#### **3. Ambassador Pattern**
Proxy qui simplifie l'accès à des services externes.

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: app-with-proxy
spec:
  containers:
  - name: app
    image: myapp:1.0
    env:
    - name: DATABASE_HOST
      value: "localhost"  # ← Pointe vers l'ambassador
    - name: DATABASE_PORT
      value: "5432"
  
  - name: db-ambassador
    image: postgres-proxy:1.0
    ports:
    - containerPort: 5432
    env:
    - name: REAL_DB_HOST
      value: "prod-db.external.com"
```

---

#### **4. Adapter Pattern**
Transforme les outputs d'un container pour les standardiser.

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: app-with-adapter
spec:
  containers:
  - name: app
    image: legacy-app:1.0  # Logs en format propriétaire
    volumeMounts:
    - name: logs
      mountPath: /var/log
  
  - name: log-adapter
    image: log-formatter:1.0  # Convertit vers JSON
    volumeMounts:
    - name: logs
      mountPath: /var/log
      readOnly: true
  
  volumes:
  - name: logs
    emptyDir: {}
```

---

### Pod avec ressources et contraintes

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: constrained-pod
spec:
  # Node selection
  nodeSelector:
    disktype: ssd
  
  # Affinity (préférence forte)
  affinity:
    nodeAffinity:
      requiredDuringSchedulingIgnoredDuringExecution:
        nodeSelectorTerms:
        - matchExpressions:
          - key: kubernetes.io/zone
            operator: In
            values:
            - eu-west-1a
            - eu-west-1b
  
  # Tolerations (pour les taints)
  tolerations:
  - key: "app"
    operator: "Equal"
    value: "backend"
    effect: "NoSchedule"
  
  containers:
  - name: app
    image: myapp:1.0
    resources:
      requests:
        memory: "512Mi"
        cpu: "500m"
      limits:
        memory: "1Gi"
        cpu: "1000m"
```

---

## 2.2. ReplicaSets

### Qu'est-ce qu'un ReplicaSet ?

Un ReplicaSet maintient un nombre stable de Pods répliqués en cours d'exécution.

**⚠️ Note :** En pratique, on utilise des **Deployments** qui gèrent les ReplicaSets automatiquement.

```yaml
apiVersion: apps/v1
kind: ReplicaSet
metadata:
  name: nginx-rs
  labels:
    app: nginx
spec:
  # Nombre de replicas désiré
  replicas: 3
  
  # Selector : identifie les Pods gérés
  selector:
    matchLabels:
      app: nginx
      tier: frontend
  
  # Template : spec des Pods
  template:
    metadata:
      labels:
        app: nginx
        tier: frontend
    spec:
      containers:
      - name: nginx
        image: nginx:1.21
        ports:
        - containerPort: 80
```

```bash
# Créer le ReplicaSet
kubectl apply -f replicaset.yaml

# Vérifier
kubectl get rs
kubectl get pods -l app=nginx

# Scaler
kubectl scale rs nginx-rs --replicas=5

# Supprimer (supprime aussi les Pods)
kubectl delete rs nginx-rs
```

**Comment ça fonctionne :**
```
ReplicaSet Controller observe:
- État actuel : 2 Pods running
- État désiré : 3 Pods

Action: Crée 1 Pod supplémentaire
```

---

## 2.3. Deployments (Rolling Updates, Rollbacks, Strategies)

### Qu'est-ce qu'un Deployment ?

Un **Deployment** gère les ReplicaSets et permet des mises à jour déclaratives des Pods.

```
Deployment
    │
    ├─→ ReplicaSet v1 (0 Pods) ← Ancienne version
    │
    └─→ ReplicaSet v2 (3 Pods) ← Version actuelle
```

### Deployment Complet (Spring Boot)

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: spring-app
  labels:
    app: spring-app
spec:
  replicas: 3
  
  # Stratégie de mise à jour
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxSurge: 1        # Pods supplémentaires pendant l'update
      maxUnavailable: 1  # Pods indisponibles max pendant l'update
  
  selector:
    matchLabels:
      app: spring-app
  
  template:
    metadata:
      labels:
        app: spring-app
        version: v1.2.0
    spec:
      containers:
      - name: app
        image: myregistry/spring-app:1.2.0
        ports:
        - containerPort: 8080
          name: http
        
        # Variables d'environnement
        env:
        - name: SPRING_PROFILES_ACTIVE
          value: "prod"
        - name: JAVA_OPTS
          value: "-Xmx1g -Xms512m -XX:MaxRAMPercentage=75.0"
        
        # Ressources
        resources:
          requests:
            memory: "1Gi"
            cpu: "500m"
          limits:
            memory: "2Gi"
            cpu: "1000m"
        
        # Health checks
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8080
          initialDelaySeconds: 60
          periodSeconds: 10
          failureThreshold: 3
        
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          initialDelaySeconds: 30
          periodSeconds: 5
          failureThreshold: 3
        
        # Graceful shutdown
        lifecycle:
          preStop:
            exec:
              command: ["/bin/sh", "-c", "sleep 10"]
```

```bash
# Créer le Deployment
kubectl apply -f deployment.yaml

# Vérifier
kubectl get deployments
kubectl get rs
kubectl get pods

# Status détaillé
kubectl rollout status deployment/spring-app
```

---

### Rolling Update

**Mise à jour progressive sans downtime.**

```bash
# Méthode 1 : Modifier le YAML et apply
kubectl apply -f deployment.yaml

# Méthode 2 : set image
kubectl set image deployment/spring-app \
  app=myregistry/spring-app:1.3.0

# Méthode 3 : edit en direct
kubectl edit deployment spring-app
```

**Ce qui se passe :**
```
1. Deployment crée un nouveau ReplicaSet (v2)
2. Scale up ReplicaSet v2 : 0 → 1 Pod
3. Attend que le Pod soit Ready
4. Scale down ReplicaSet v1 : 3 → 2 Pods
5. Répète jusqu'à ce que v1 = 0 et v2 = 3
```

**Monitoring :**
```bash
# Suivre le rollout
kubectl rollout status deployment/spring-app

# Historique
kubectl rollout history deployment/spring-app

# Pause/Resume
kubectl rollout pause deployment/spring-app
kubectl rollout resume deployment/spring-app
```

---

### Rollback

**Revenir à une version précédente en cas de problème.**

```bash
# Rollback vers la version précédente
kubectl rollout undo deployment/spring-app

# Rollback vers une révision spécifique
kubectl rollout history deployment/spring-app
# REVISION  CHANGE-CAUSE
# 1         <none>
# 2         Updated to v1.2.0
# 3         Updated to v1.3.0 (broken)

kubectl rollout undo deployment/spring-app --to-revision=2

# Vérifier
kubectl rollout status deployment/spring-app
```

**Automatisation du rollback :**
```yaml
spec:
  # Limite le nombre de révisions gardées
  revisionHistoryLimit: 10
  
  # Temps max pour un rollout
  progressDeadlineSeconds: 600
```

---

### Stratégies de Déploiement

#### **1. RollingUpdate (Défaut)**
```yaml
strategy:
  type: RollingUpdate
  rollingUpdate:
    maxSurge: 25%        # 25% de Pods supplémentaires
    maxUnavailable: 25%  # 25% de Pods indisponibles
```

**Avantages :** Zéro downtime, rollback facile
**Inconvénients :** Période avec v1 et v2 en même temps

---

#### **2. Recreate**
```yaml
strategy:
  type: Recreate
```

**Comportement :**
1. Supprime TOUS les Pods v1
2. Crée les Pods v2

**Avantages :** Simple, pas de cohabitation v1/v2
**Inconvénients :** Downtime pendant le switch

**Cas d'usage :** Applications incompatibles entre versions

---

#### **3. Blue/Green (Manuel avec labels)**
```yaml
# Blue (version actuelle)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: app-blue
spec:
  replicas: 3
  selector:
    matchLabels:
      app: myapp
      version: blue
  template:
    metadata:
      labels:
        app: myapp
        version: blue
    spec:
      containers:
      - name: app
        image: myapp:1.0
---
# Green (nouvelle version)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: app-green
spec:
  replicas: 3
  selector:
    matchLabels:
      app: myapp
      version: green
  template:
    metadata:
      labels:
        app: myapp
        version: green
    spec:
      containers:
      - name: app
        image: myapp:2.0
---
# Service qui pointe vers blue
apiVersion: v1
kind: Service
metadata:
  name: myapp-service
spec:
  selector:
    app: myapp
    version: blue  # ← Switch vers green pour basculer
  ports:
  - port: 80
    targetPort: 8080
```

**Switch instantané :**
```bash
# Tester green
kubectl port-forward svc/myapp-service 8080:80

# Basculer vers green
kubectl patch service myapp-service -p '{"spec":{"selector":{"version":"green"}}}'

# Rollback si problème
kubectl patch service myapp-service -p '{"spec":{"selector":{"version":"blue"}}}'
```

---

#### **4. Canary (Progressif)**
```yaml
# Stable (90% du trafic)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: app-stable
spec:
  replicas: 9
  selector:
    matchLabels:
      app: myapp
      track: stable
  template:
    metadata:
      labels:
        app: myapp
        track: stable
    spec:
      containers:
      - name: app
        image: myapp:1.0
---
# Canary (10% du trafic)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: app-canary
spec:
  replicas: 1  # ← 10% du trafic
  selector:
    matchLabels:
      app: myapp
      track: canary
  template:
    metadata:
      labels:
        app: myapp
        track: canary
    spec:
      containers:
      - name: app
        image: myapp:2.0
---
# Service qui load balance les deux
apiVersion: v1
kind: Service
metadata:
  name: myapp-service
spec:
  selector:
    app: myapp  # ← Pas de track, donc les deux versions
  ports:
  - port: 80
    targetPort: 8080
```

**Progression :**
```bash
# Phase 1 : 10% canary
kubectl scale deployment app-canary --replicas=1

# Monitorer les métriques
# Si OK → Phase 2 : 50% canary
kubectl scale deployment app-canary --replicas=5
kubectl scale deployment app-stable --replicas=5

# Si OK → 100% canary
kubectl scale deployment app-canary --replicas=10
kubectl scale deployment app-stable --replicas=0
```

---

## 2.4. StatefulSets

### Qu'est-ce qu'un StatefulSet ?

Pour les applications **stateful** qui nécessitent :
- Identités stables et persistantes
- Stockage persistant
- Ordre de déploiement/suppression

```
Deployment:           StatefulSet:
├─ pod-abc123        ├─ app-0 (identity stable)
├─ pod-def456        ├─ app-1
└─ pod-ghi789        └─ app-2
   (noms aléatoires)    (noms ordonnés)
```

### Exemple : Base de données PostgreSQL

```yaml
apiVersion: v1
kind: Service
metadata:
  name: postgres-headless
spec:
  clusterIP: None  # Headless Service
  selector:
    app: postgres
  ports:
  - port: 5432
---
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: postgres
spec:
  serviceName: postgres-headless  # Lié au Service Headless
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
          name: postgres
        env:
        - name: POSTGRES_PASSWORD
          value: "mysecretpassword"
        - name: PGDATA
          value: /var/lib/postgresql/data/pgdata
        volumeMounts:
        - name: postgres-storage
          mountPath: /var/lib/postgresql/data
  
  # Volume Claim Templates (un PVC par Pod)
  volumeClaimTemplates:
  - metadata:
      name: postgres-storage
    spec:
      accessModes: ["ReadWriteOnce"]
      storageClassName: fast-ssd
      resources:
        requests:
          storage: 10Gi
```

**Caractéristiques :**
```bash
# Pods créés dans l'ordre
postgres-0  # Créé en premier
postgres-1  # Créé après que postgres-0 soit Ready
postgres-2  # Créé après que postgres-1 soit Ready

# Chaque Pod a son DNS stable
postgres-0.postgres-headless.default.svc.cluster.local
postgres-1.postgres-headless.default.svc.cluster.local
postgres-2.postgres-headless.default.svc.cluster.local

# Chaque Pod a son PVC dédié
postgres-storage-postgres-0
postgres-storage-postgres-1
postgres-storage-postgres-2
```

**Gestion :**
```bash
# Créer
kubectl apply -f statefulset.yaml

# Vérifier
kubectl get statefulset
kubectl get pods -l app=postgres
kubectl get pvc

# Scale (ajout/suppression ordonnée)
kubectl scale statefulset postgres --replicas=5

# Supprimer (ordre inverse)
kubectl delete statefulset postgres
# Supprime : postgres-2, puis postgres-1, puis postgres-0
# ⚠️ Les PVCs ne sont PAS supprimés automatiquement
```

**Cas d'usage :**
- Bases de données (PostgreSQL, MySQL, MongoDB)
- Message queues (Kafka, RabbitMQ)
- Systèmes distribués (Elasticsearch, Cassandra)

---

## 2.5. DaemonSets

### Qu'est-ce qu'un DaemonSet ?

Garantit qu'un Pod tourne sur **chaque Node** (ou un subset de Nodes).

```
Cluster:
Node-1: [daemon-pod-1]
Node-2: [daemon-pod-2]
Node-3: [daemon-pod-3]

Nouveau Node-4 ajouté → [daemon-pod-4] créé automatiquement
```

### Exemple : Node Monitoring avec Prometheus

```yaml
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: node-exporter
  namespace: monitoring
spec:
  selector:
    matchLabels:
      app: node-exporter
  
  template:
    metadata:
      labels:
        app: node-exporter
    spec:
      # Tolère tous les taints pour tourner sur tous les Nodes
      tolerations:
      - effect: NoSchedule
        operator: Exists
      
      # HostNetwork pour accéder aux métriques du Node
      hostNetwork: true
      hostPID: true
      
      containers:
      - name: node-exporter
        image: prom/node-exporter:v1.3.1
        ports:
        - containerPort: 9100
          name: metrics
        volumeMounts:
        - name: proc
          mountPath: /host/proc
          readOnly: true
        - name: sys
          mountPath: /host/sys
          readOnly: true
      
      volumes:
      - name: proc
        hostPath:
          path: /proc
      - name: sys
        hostPath:
          path: /sys
```

```bash
# Créer
kubectl apply -f daemonset.yaml

# Vérifier (1 Pod par Node)
kubectl get daemonset -n monitoring
kubectl get pods -n monitoring -o wide

# Update (rolling update comme Deployment)
kubectl set image daemonset/node-exporter node-exporter=prom/node-exporter:v1.4.0 -n monitoring
```

**Cas d'usage :**
- Log collection (Fluentd, Filebeat)
- Monitoring (Prometheus node-exporter, Datadog agent)
- Storage daemons (Ceph, GlusterFS)
- Network plugins (kube-proxy, Calico)

**Node Selector pour DaemonSet :**
```yaml
spec:
  template:
    spec:
      nodeSelector:
        disktype: ssd  # Seulement sur les Nodes avec SSD
```

---

## 2.6. Jobs & CronJobs

### Jobs

Un **Job** crée un ou plusieurs Pods et s'assure qu'un nombre spécifié se termine avec succès.

```yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: data-migration
spec:
  # Nombre de completions souhaitées
  completions: 1
  
  # Parallélisme (Pods en simultané)
  parallelism: 1
  
  # Nombre de retry en cas d'échec
  backoffLimit: 3
  
  # Timeout
  activeDeadlineSeconds: 600  # 10 minutes max
  
  template:
    spec:
      restartPolicy: OnFailure  # ou Never
      containers:
      - name: migration
        image: myapp/migration:1.0
        command: ["python", "migrate.py"]
        env:
        - name: DB_HOST
          value: "postgres.prod.svc.cluster.local"
```

```bash
# Créer et suivre
kubectl apply -f job.yaml
kubectl get jobs
kubectl get pods

# Logs
kubectl logs job/data-migration

# Supprimer (nettoie aussi les Pods)
kubectl delete job data-migration
```

**Job avec parallélisme :**
```yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: batch-processing
spec:
  completions: 10      # 10 tâches au total
  parallelism: 3       # 3 Pods en parallèle
  template:
    spec:
      restartPolicy: OnFailure
      containers:
      - name: processor
        image: batch-processor:1.0
```

**Comportement :**
```
1. Démarre 3 Pods en parallèle
2. Quand un Pod termine → démarre un nouveau
3. Continue jusqu'à 10 completions
```

---

### CronJobs

Un **CronJob** crée des Jobs selon un planning (cron syntax).

```yaml
apiVersion: batch/v1
kind: CronJob
metadata:
  name: backup-database
spec:
  # Planning cron : minute heure jour mois jour_semaine
  schedule: "0 2 * * *"  # Tous les jours à 2h du matin
  
  # Timezone
  timeZone: "Europe/Paris"
  
  # Politique de concurrence
  concurrencyPolicy: Forbid  # Forbid, Allow, Replace
  
  # Nombre de Jobs à conserver
  successfulJobsHistoryLimit: 3
  failedJobsHistoryLimit: 1
  
  jobTemplate:
    spec:
      template:
        spec:
          restartPolicy: OnFailure
          containers:
          - name: backup
            image: postgres:14
            command:
            - /bin/sh
            - -c
            - |
              pg_dump -h postgres.prod.svc.cluster.local \
                      -U admin mydb > /backup/dump-$(date +%Y%m%d).sql
            volumeMounts:
            - name: backup-storage
              mountPath: /backup
          volumes:
          - name: backup-storage
            persistentVolumeClaim:
              claimName: backup-pvc
```

**Exemples de schedules :**
```yaml
"*/5 * * * *"      # Toutes les 5 minutes
"0 */2 * * *"      # Toutes les 2 heures
"0 0 * * 0"        # Tous les dimanches à minuit
"30 3 * * 1-5"     # Lundi à vendredi à 3h30
"0 0 1 * *"        # Le 1er de chaque mois à minuit
```

```bash
# Créer
kubectl apply -f cronjob.yaml

# Lister
kubectl get cronjobs

# Déclencher manuellement
kubectl create job backup-manual --from=cronjob/backup-database

# Suspendre
kubectl patch cronjob backup-database -p '{"spec":{"suspend":true}}'

# Historique des Jobs
kubectl get jobs
```

**Concurrency Policies :**
- **Allow** : Plusieurs Jobs peuvent tourner en parallèle
- **Forbid** : Si un Job précédent tourne encore, skip
- **Replace** : Annule le Job précédent et démarre le nouveau

---

## 🎯 Résumé des Workload Resources

| Resource      | Use Case                          | Pods Identity | Ordre    | Storage     |
|---------------|-----------------------------------|---------------|----------|-------------|
| **Pod**       | Test, debug                       | Aléatoire     | N/A      | Éphémère    |
| **ReplicaSet**| Géré par Deployment               | Aléatoire     | Parallèle| Éphémère    |
| **Deployment**| Applications stateless            | Aléatoire     | Parallèle| Éphémère    |
| **StatefulSet**| Applications stateful (DB)       | Stable        | Séquentiel| Persistant |
| **DaemonSet** | Agents système (logs, monitoring) | Par Node      | Parallèle| Selon besoin|
| **Job**       | Tâches one-shot                   | Aléatoire     | Parallèle| Éphémère    |
| **CronJob**   | Tâches planifiées                 | Aléatoire     | Selon schedule| Éphémère |

---

## 📝 Best Practices

### Deployment
✅ Toujours spécifier `resources` (requests/limits)
✅ Configurer `livenessProbe` et `readinessProbe`
✅ Utiliser `RollingUpdate` avec `maxSurge` et `maxUnavailable`
✅ Limiter `revisionHistoryLimit` (défaut: 10)
✅ Ajouter des labels significatifs

### StatefulSet
✅ Utiliser un Headless Service
✅ Définir `volumeClaimTemplates` pour le storage
✅ Tester le scale up/down avant prod
✅ Documenter l'ordre de démarrage si critique

### DaemonSet
✅ Ajouter les `tolerations` nécessaires
✅ Utiliser `updateStrategy: RollingUpdate`
✅ Limiter les ressources (peut affecter tous les Nodes)

### Jobs/CronJobs
✅ Définir `activeDeadlineSeconds` (timeout)
✅ Configurer `backoffLimit` (retries)
✅ Utiliser `restartPolicy: OnFailure` ou `Never`
✅ Pour CronJobs: définir `concurrencyPolicy`

---

# Chapitre 2 : Questions/Réponses d'Entretien

## 🎯 Pods & Lifecycle

### Q1 : Qu'est-ce qu'un Pod et pourquoi Kubernetes n'utilise-t-il pas directement des containers ?

**Réponse :**
Un Pod est la plus petite unité déployable dans Kubernetes. C'est un wrapper autour d'un ou plusieurs containers qui partagent :
- **Le même namespace réseau** : ils communiquent via localhost
- **Les mêmes volumes** : partage de fichiers possible
- **Le même namespace IPC** : communication inter-processus

**Pourquoi pas juste des containers ?**
1. **Patterns multi-containers** : sidecar, adapter, ambassador
2. **Co-location** : garantit que les containers sont sur le même Node
3. **Gestion unifiée** : cycle de vie commun (démarrage, arrêt)
4. **Abstraction** : indépendant du runtime (Docker, containerd, CRI-O)

**Exemple :** Application web + sidecar pour logs
```yaml
spec:
  containers:
  - name: web
    image: nginx
  - name: log-collector
    image: fluentd
```
Les deux containers peuvent accéder aux mêmes logs via un volume partagé.

---

### Q2 : Expliquez les différentes phases du lifecycle d'un Pod.

**Réponse :**

**Phases principales :**

1. **Pending**
   - Pod accepté par K8s
   - En attente de scheduling ou de pull d'image
   - Causes : pas de Node disponible, image inexistante

2. **Running**
   - Pod assigné à un Node
   - Au moins un container est en cours d'exécution
   - Peut avoir des containers en restart

3. **Succeeded**
   - Tous les containers terminés avec succès (exit 0)
   - Typique pour les Jobs
   - Les Pods ne redémarrent pas

4. **Failed**
   - Au moins un container a échoué (exit non-zero)
   - Aucun container ne tourne

5. **Unknown**
   - État indéterminé (problème de communication avec kubelet)

**Status spéciaux :**
- **CrashLoopBackOff** : Container crashe répétitivement, K8s augmente le délai de restart
- **ImagePullBackOff** : Impossible de télécharger l'image, backoff exponentiel

**Vérification :**
```bash
kubectl get pods
kubectl describe pod <pod-name>  # Voir les Events
kubectl get pod <pod-name> -o jsonpath='{.status.phase}'
```

---

### Q3 : Quelle est la différence entre un Init Container et un Sidecar Container ?

**Réponse :**

| Aspect | Init Container | Sidecar Container |
|--------|----------------|-------------------|
| **Exécution** | Avant les containers principaux | En parallèle avec les containers principaux |
| **Ordre** | Séquentiel (un après l'autre) | Simultané |
| **Lifecycle** | Doit se terminer avec succès | Tourne pendant toute la vie du Pod |
| **Restart** | Ne redémarre pas | Redémarre si crash |

**Init Container - Exemple :**
```yaml
initContainers:
- name: wait-db
  image: busybox
  command: ['sh', '-c', 'until nslookup db; do sleep 2; done']
# ↓ Démarre APRÈS succès de l'init container
containers:
- name: app
  image: myapp
```

**Sidecar - Exemple :**
```yaml
containers:
- name: app
  image: myapp
- name: log-shipper  # ← Tourne en parallèle
  image: fluentd
```

**Cas d'usage Init :**
- Attendre des dépendances (DB, services)
- Cloner un repo Git
- Générer des fichiers de config
- Setup permissions

**Cas d'usage Sidecar :**
- Collecte de logs
- Service mesh proxy (Envoy, Istio)
- Monitoring agents
- Cache local

---

### Q4 : Comment debugger un Pod en CrashLoopBackOff ?

**Réponse :**

**Approche systématique :**

**1. Vérifier les logs :**
```bash
# Logs actuels
kubectl logs <pod-name>

# Logs du container précédent (avant crash)
kubectl logs <pod-name> --previous

# Multi-container pod
kubectl logs <pod-name> -c <container-name>
```

**2. Examiner les events :**
```bash
kubectl describe pod <pod-name>
# Section Events en bas : erreurs, warnings
```

**3. Vérifier la configuration :**
```bash
kubectl get pod <pod-name> -o yaml
```
- Image correcte ?
- Variables d'environnement ?
- Resources suffisantes ?
- Probes trop agressives ?

**4. Causes fréquentes et solutions :**

**a) Application crashe au démarrage :**
```yaml
# Problème : liveness trop tôt
livenessProbe:
  initialDelaySeconds: 10  # ← Trop court pour Spring Boot
# Solution :
livenessProbe:
  initialDelaySeconds: 60  # Laisser le temps de démarrer
```

**b) Manque de mémoire (OOMKilled) :**
```bash
kubectl describe pod <pod> | grep -i oom
# Last State: Terminated, Reason: OOMKilled

# Solution : augmenter les limits
resources:
  limits:
    memory: "2Gi"  # Au lieu de 512Mi
```

**c) Erreur de configuration :**
```yaml
# ConfigMap ou Secret manquant
env:
- name: DB_PASSWORD
  valueFrom:
    secretKeyRef:
      name: db-secret  # ← Ce secret existe-t-il ?
      key: password
```

**5. Debug interactif (si possible) :**
```bash
# Modifier le Pod pour qu'il ne crashe pas immédiatement
kubectl edit pod <pod-name>
# Changer command: ["/bin/sh", "-c", "sleep 3600"]

# Ou créer un Pod de debug
kubectl debug <pod-name> -it --image=busybox --share-processes
```

**6. Pour Spring Boot spécifiquement :**
```yaml
env:
- name: JAVA_OPTS
  value: "-XX:+HeapDumpOnOutOfMemoryError -XX:HeapDumpPath=/tmp"
# Permet d'analyser les heap dumps en cas d'OOM
```

---

## 🔄 Deployments & Updates

### Q5 : Quelle est la différence entre un Deployment et un ReplicaSet ?

**Réponse :**

**ReplicaSet :**
- Maintient un nombre stable de Pods répliqués
- Fonction principale : s'assurer que X Pods tournent toujours
- **Limitation** : pas de gestion des updates

**Deployment :**
- Niveau supérieur qui **gère les ReplicaSets**
- Fournit des updates déclaratifs (rolling updates, rollbacks)
- Gère l'historique des versions

**Hiérarchie :**
```
Deployment "myapp"
  ├─ ReplicaSet "myapp-abc123" (v1, 0 replicas) ← Ancienne version
  └─ ReplicaSet "myapp-xyz789" (v2, 3 replicas) ← Version actuelle
```

**En pratique :**
- ✅ Utilisez toujours des Deployments (sauf cas spécifiques)
- ❌ Ne créez jamais de ReplicaSets manuellement
- Les ReplicaSets sont créés et gérés automatiquement par les Deployments

```bash
# Créer un Deployment crée automatiquement un ReplicaSet
kubectl create deployment nginx --image=nginx --replicas=3

kubectl get deployments
kubectl get replicasets  # Un RS créé automatiquement
kubectl get pods         # 3 Pods gérés par le RS
```

---

### Q6 : Expliquez comment fonctionne un Rolling Update dans un Deployment.

**Réponse :**

Un Rolling Update remplace progressivement les Pods de l'ancienne version par la nouvelle, sans downtime.

**Configuration :**
```yaml
strategy:
  type: RollingUpdate
  rollingUpdate:
    maxSurge: 1        # Pods supplémentaires autorisés
    maxUnavailable: 1  # Pods indisponibles max
```

**Processus détaillé (replicas: 3) :**

```
État initial : 3 Pods v1
ReplicaSet-v1: [Pod-A, Pod-B, Pod-C]

1. Deployment crée ReplicaSet-v2
   RS-v1: [Pod-A, Pod-B, Pod-C]
   RS-v2: []

2. Scale up RS-v2 (maxSurge=1 → total=4 Pods OK)
   RS-v1: [Pod-A, Pod-B, Pod-C]
   RS-v2: [Pod-D] ← Creating

3. Attend que Pod-D soit Ready
   RS-v2: [Pod-D] ← Running & Ready

4. Scale down RS-v1 (maxUnavailable=1)
   RS-v1: [Pod-A, Pod-B] ← Pod-C Terminating
   RS-v2: [Pod-D]

5. Répète : scale up v2, attend Ready, scale down v1
   RS-v1: [Pod-A]
   RS-v2: [Pod-D, Pod-E]

6. Final
   RS-v1: [] ← 0 replicas (mais conservé pour rollback)
   RS-v2: [Pod-D, Pod-E, Pod-F]
```

**Paramètres importants :**
- **maxSurge** : nombre de Pods supplémentaires (ex: 25% = 0.75 arrondi à 1)
- **maxUnavailable** : nombre de Pods indisponibles max
- **progressDeadlineSeconds** : timeout (défaut: 600s)

**Commandes :**
```bash
# Déclencher un update
kubectl set image deployment/myapp app=myapp:2.0

# Suivre la progression
kubectl rollout status deployment/myapp

# Pause/Resume
kubectl rollout pause deployment/myapp
kubectl rollout resume deployment/myapp
```

**Avantages :**
- ✅ Zéro downtime
- ✅ Rollback facile
- ✅ Progressif et contrôlé

**Inconvénients :**
- ⚠️ Période avec 2 versions en production
- ⚠️ Nécessite compatibilité v1 ↔ v2 (DB schema, APIs)

---

### Q7 : Comment faire un rollback d'un Deployment ? Dans quels cas l'utiliser ?

**Réponse :**

**Méthode 1 : Rollback vers la version précédente**
```bash
kubectl rollout undo deployment/myapp
```

**Méthode 2 : Rollback vers une révision spécifique**
```bash
# Voir l'historique
kubectl rollout history deployment/myapp
# REVISION  CHANGE-CAUSE
# 1         Initial deployment
# 2         Updated to v1.2
# 3         Updated to v1.3 (broken)

# Rollback vers la revision 2
kubectl rollout undo deployment/myapp --to-revision=2
```

**Méthode 3 : Via YAML (GitOps)**
```bash
# Modifier le YAML pour revenir à l'ancienne image
kubectl apply -f deployment.yaml
```

**Comment ça fonctionne :**
```
État actuel :
RS-v3 (broken): 3 replicas
RS-v2 (stable): 0 replicas ← Toujours existant !

Après rollback :
RS-v3: 0 replicas (scale down progressif)
RS-v2: 3 replicas (scale up progressif)
```

**Cas d'usage du rollback :**

1. **Bug critique en production**
   ```bash
   # Rollback immédiat
   kubectl rollout undo deployment/api-backend
   ```

2. **Performance dégradée**
   - Latence augmentée
   - Erreurs 500
   - Consommation mémoire excessive

3. **Incompatibilité découverte**
   - Breaking change dans l'API
   - Migration DB échouée

4. **Échec des health checks**
   ```yaml
   readinessProbe:
     httpGet:
       path: /health
     failureThreshold: 3
   # Si les nouveaux Pods ne passent jamais Ready → rollback manuel
   ```

**Best practices :**
```yaml
spec:
  # Conserver 10 révisions pour rollback
  revisionHistoryLimit: 10
  
  # Timeout pour détecter les problèmes
  progressDeadlineSeconds: 600
```

**Monitoring post-rollback :**
```bash
# Vérifier que le rollback a réussi
kubectl rollout status deployment/myapp

# Vérifier les Pods
kubectl get pods -l app=myapp

# Logs pour identifier la cause du problème initial
kubectl logs deployment/myapp --previous --tail=100
```

---

### Q8 : Expliquez la différence entre les stratégies RollingUpdate et Recreate.

**Réponse :**

### **RollingUpdate (Défaut)**

**Comportement :**
- Remplace progressivement les Pods
- Maintient toujours des Pods actifs
- Période de transition avec v1 et v2

```yaml
strategy:
  type: RollingUpdate
  rollingUpdate:
    maxSurge: 1        # +1 Pod temporaire
    maxUnavailable: 0  # Toujours 3 Pods disponibles
```

**Timeline :**
```
t0: [v1, v1, v1]           ← 3 Pods v1
t1: [v1, v1, v1, v2]       ← +1 Pod v2 (maxSurge)
t2: [v1, v1, v2]           ← -1 Pod v1
t3: [v1, v1, v2, v2]       ← +1 Pod v2
t4: [v1, v2, v2]           ← -1 Pod v1
t5: [v1, v2, v2, v2]       ← +1 Pod v2
t6: [v2, v2, v2]           ← -1 Pod v1, terminé
```

**Avantages :**
- ✅ Zéro downtime
- ✅ Rollback facile (ReplicaSets gardés)
- ✅ Détection progressive des problèmes

**Inconvénients :**
- ⚠️ v1 et v2 cohabitent temporairement
- ⚠️ Requiert compatibilité entre versions
- ⚠️ Plus lent qu'un Recreate

---

### **Recreate**

**Comportement :**
- Supprime TOUS les Pods v1
- Puis crée TOUS les Pods v2
- Downtime garanti

```yaml
strategy:
  type: Recreate
```

**Timeline :**
```
t0: [v1, v1, v1]     ← 3 Pods v1
t1: []               ← Tous les Pods supprimés (DOWNTIME)
t2: [v2, v2, v2]     ← 3 Pods v2 créés
```

**Avantages :**
- ✅ Pas de cohabitation v1/v2
- ✅ Plus simple conceptuellement
- ✅ Plus rapide (pas de phase progressive)

**Inconvénients :**
- ❌ Downtime (secondes à minutes)
- ❌ Pas de rollback automatique
- ❌ Impact utilisateur direct

---

**Quand utiliser quoi ?**

| Critère | RollingUpdate | Recreate |
|---------|---------------|----------|
| **Applications stateless** | ✅ Recommandé | ❌ |
| **APIs REST** | ✅ Oui | ❌ |
| **Microservices** | ✅ Oui | ❌ |
| **Applications avec état** | ⚠️ Selon cas | ✅ Parfois |
| **Incompatibilité v1/v2** | ❌ | ✅ Oui |
| **Dev/Test environments** | ⚠️ | ✅ OK |
| **Production critique** | ✅ Toujours | ❌ |

**Exemple Recreate - Application avec state incompatible :**
```yaml
# v1 utilise SQLite, v2 utilise PostgreSQL
# Impossible de faire cohabiter les deux
strategy:
  type: Recreate
```

---

### Q9 : Comment implémenteriez-vous une stratégie de déploiement Canary ?

**Réponse :**

Une stratégie Canary expose une nouvelle version à un petit pourcentage d'utilisateurs avant un rollout complet.

**Implémentation manuelle (2 Deployments) :**

```yaml
# Deployment stable (90% du trafic)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: myapp-stable
spec:
  replicas: 9  # 90%
  selector:
    matchLabels:
      app: myapp
      version: stable
  template:
    metadata:
      labels:
        app: myapp
        version: stable
    spec:
      containers:
      - name: app
        image: myapp:1.0
---
# Deployment canary (10% du trafic)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: myapp-canary
spec:
  replicas: 1  # 10%
  selector:
    matchLabels:
      app: myapp
      version: canary
  template:
    metadata:
      labels:
        app: myapp
        version: canary
    spec:
      containers:
      - name: app
        image: myapp:2.0
---
# Service qui load balance les deux versions
apiVersion: v1
kind: Service
metadata:
  name: myapp
spec:
  selector:
    app: myapp  # ← Match les 2 Deployments
  ports:
  - port: 80
    targetPort: 8080
```

**Progression Canary :**

```bash
# Phase 1 : 10% canary
kubectl apply -f deployments.yaml
# stable: 9 replicas, canary: 1 replica

# Monitoring pendant 30 minutes
# - Métriques : latence, erreurs, CPU, mémoire
# - Logs : erreurs applicatives
# - APM : traces distribuées

# Si OK → Phase 2 : 50% canary
kubectl scale deployment myapp-canary --replicas=5
kubectl scale deployment myapp-stable --replicas=5

# Monitoring 1 heure

# Si OK → Phase 3 : 100% canary
kubectl scale deployment myapp-canary --replicas=10
kubectl scale deployment myapp-stable --replicas=0

# Nettoyage final
kubectl delete deployment myapp-stable
kubectl patch deployment myapp-canary --type=json \
  -p='[{"op": "remove", "path": "/spec/template/metadata/labels/version"}]'
```

**Métriques à surveiller :**
```yaml
# Prometheus queries
# Taux d'erreur par version
sum(rate(http_requests_total{status=~"5..",version="canary"}[5m]))
/ sum(rate(http_requests_total{version="canary"}[5m]))

# Latence p99 par version
histogram_quantile(0.99, 
  rate(http_request_duration_seconds_bucket{version="canary"}[5m]))
```

**Rollback automatique (avec Flagger) :**
```yaml
apiVersion: flagger.app/v1beta1
kind: Canary
metadata:
  name: myapp
spec:
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: myapp
  service:
    port: 80
  analysis:
    interval: 1m
    threshold: 5
    maxWeight: 50
    stepWeight: 10
    metrics:
    - name: request-success-rate
      thresholdRange:
        min: 99
    - name: request-duration
      thresholdRange:
        max: 500
```

**Avantages Canary :**
- ✅ Risque limité (impact < 100% users)
- ✅ Feedback réel en production
- ✅ Rollback rapide si problème détecté

**Inconvénients :**
- ⚠️ Complexe à mettre en place
- ⚠️ Nécessite monitoring avancé
- ⚠️ Gestion manuelle sans outils dédiés (Flagger, Argo Rollouts)

---

## 📊 StatefulSets

### Q10 : Quelle est la différence entre un Deployment et un StatefulSet ?

**Réponse :**

| Aspect | Deployment | StatefulSet |
|--------|-----------|-------------|
| **Identité Pod** | Aléatoire (pod-abc123) | Stable (app-0, app-1, app-2) |
| **Ordre création** | Parallèle | Séquentiel (0→1→2) |
| **Ordre suppression** | Parallèle | Inverse (2→1→0) |
| **DNS** | Via Service (load balanced) | Headless Service + DNS par Pod |
| **Storage** | Partagé ou éphémère | PVC dédié par Pod |
| **Use case** | Applications stateless | Applications stateful |

**Deployment - Identité éphémère :**
```bash
kubectl get pods
# web-5d4c8f9b7-abc12
# web-5d4c8f9b7-def34  ← Noms aléatoires
# web-5d4c8f9b7-ghi56

# Si un Pod crash → nouveau nom
# web-5d4c8f9b7-jkl78
```

**StatefulSet - Identité stable :**
```bash
kubectl get pods
# postgres-0  ← Nom stable
# postgres-1
# postgres-2

# Si postgres-1 crash → recréé avec le MÊME nom
# postgres-1 (nouveau Pod, même identité)
```

**DNS Headless Service :**
```yaml
# Deployment : un seul DNS pour tous les Pods
web.default.svc.cluster.local → Load balancé

# StatefulSet : DNS individuel par Pod
postgres-0.postgres.default.svc.cluster.local
postgres-1.postgres.default.svc.cluster.local
postgres-2.postgres.default.svc.cluster.local
```

**Storage :**
```yaml
# Deployment : tous les Pods partagent le même PVC (ou aucun)
volumes:
- name: shared-data
  persistentVolumeClaim:
    claimName: shared-pvc  # Tous les Pods utilisent ce PVC

# StatefulSet : chaque Pod a son PVC dédié
volumeClaimTemplates:
- metadata:
    name: data
  spec:
    accessModes: ["ReadWriteOnce"]
    resources:
      requests:
        storage: 10Gi
# Crée : data-postgres-0, data-postgres-1, data-postgres-2
```

**Quand utiliser StatefulSet :**
- ✅ Bases de données (PostgreSQL, MySQL, MongoDB)
- ✅ Applications clustered (Kafka, Elasticsearch, Cassandra)
- ✅ Besoin d'identités stables et prévisibles
- ✅ Ordre de démarrage/arrêt important

**Quand utiliser Deployment :**
- ✅ APIs REST, microservices
- ✅ Applications stateless
- ✅ Pas besoin de storage persistant dédié
- ✅ Ordre de création non important

---

### Q11 : Comment scaler un StatefulSet et quelles sont les précautions à prendre ?

**Réponse :**

**Scale Up (augmenter les replicas) :**
```bash
# De 3 à 5 Pods
kubectl scale statefulset postgres --replicas=5

# Ordre de création : SÉQUENTIEL
# 1. postgres-3 créé et attend Ready
# 2. postgres-4 créé et attend Ready
```

**Process détaillé :**
```
t0: [postgres-0, postgres-1, postgres-2]

t1: postgres-3 → Pending
    - PVC créé : data-postgres-3
    - Attend volume binding

t2: postgres-3 → Running
    - Attend readinessProbe OK

t3: postgres-3 → Ready
    - postgres-4 peut maintenant être créé

t4: postgres-4 → Pending → Running → Ready

Final: [postgres-0, postgres-1, postgres-2, postgres-3, postgres-4]
```

---

**Scale Down (réduire les replicas) :**
```bash
# De 5 à 3 Pods
kubectl scale statefulset postgres --replicas=3

# Ordre de suppression : INVERSE (dernier créé = premier supprimé)
# 1. postgres-4 supprimé
# 2. postgres-3 supprimé
```

**⚠️ Important : Les PVCs ne sont PAS supprimés !**
```bash
kubectl get pvc
# data-postgres-0  Bound
# data-postgres-1  Bound
# data-postgres-2  Bound
# data-postgres-3  Bound  ← PVC conservé même si Pod supprimé
# data-postgres-4  Bound  ← PVC conservé même si Pod supprimé

# Nettoyage manuel si nécessaire
kubectl delete pvc data-postgres-3 data-postgres-4
```

---

**Précautions à prendre :**

**1. Vérifier la santé avant scale :**
```bash
# Tous les Pods doivent être Ready
kubectl get statefulset postgres
# NAME       READY   AGE
# postgres   3/3     10d

# Si 2/3 Ready → NE PAS scaler, debugger d'abord
```

**2. Pour les bases de données - Vérifier la réplication :**
```bash
# PostgreSQL : vérifier que les replicas sont synchro
kubectl exec postgres-0 -- psql -c "SELECT * FROM pg_stat_replication;"

# Si lag important → attendre la synchro avant scale
```

**3. Capacité du cluster :**
```bash
# Vérifier qu'il y a assez de ressources
kubectl describe nodes | grep -A 5 "Allocated resources"

# Vérifier les StorageClasses
kubectl get storageclass
# Peut provisionner des nouveaux PVs ?
```

**4. Considérations applicatives :**

**PostgreSQL avec streaming replication :**
```bash
# Scale up : OK, nouveau replica se synchronise automatiquement
kubectl scale statefulset postgres --replicas=4

# Scale down : Attention !
# Si postgres-3 était un replica actif → risque de perte de connexions
kubectl scale statefulset postgres --replicas=3
```

**Kafka :**
```bash
# Scale up : Kafka détecte automatiquement le nouveau broker
kubectl scale statefulset kafka --replicas=4

# Scale down : DANGER !
# Rebalancer les partitions AVANT de scale down
# Sinon : perte de données potentielle
```

**Elasticsearch :**
```bash
# Scale up : Le cluster s'équilibre automatiquement
kubectl scale statefulset elasticsearch --replicas=4

# Scale down : Vérifier qu'aucun shard n'est sur le node à supprimer
# Désactiver l'allocation sur le node, attendre migration
```

---

**5. PodDisruptionBudget (PDB) :**
```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: postgres-pdb
spec:
  minAvailable: 2  # Au moins 2 Pods doivent rester disponibles
  selector:
    matchLabels:
      app: postgres
```

**6. Monitoring post-scale :**
```bash
# Vérifier que tous les Pods sont Ready
watch kubectl get pods -l app=postgres

# Vérifier les events
kubectl get events --sort-by='.lastTimestamp' | grep postgres

# Vérifier les PVCs
kubectl get pvc -l app=postgres

# Tester la connectivité
kubectl exec postgres-0 -- pg_isready
kubectl exec postgres-3 -- pg_isready  # Nouveau Pod
```

---

**Best practices :**
```yaml
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: postgres
spec:
  # Politique de gestion des Pods
  podManagementPolicy: OrderedReady  # Défaut : séquentiel
  # Ou Parallel : plus rapide mais moins safe
  
  # Update strategy
  updateStrategy:
    type: RollingUpdate
    rollingUpdate:
      partition: 0  # Permet de tester sur un subset
  
  # Révisions conservées
  revisionHistoryLimit: 10
```

**Scale progressif pour production :**
```bash
# Au lieu de scaler de 3 à 10 directement
# Scaler progressivement et monitorer

kubectl scale statefulset postgres --replicas=4
# Attendre stabilisation (30 min)

kubectl scale statefulset postgres --replicas=6
# Monitorer performance, réplication

kubectl scale statefulset postgres --replicas=10
```

---

## 🔧 DaemonSets & Jobs

### Q12 : Qu'est-ce qu'un DaemonSet et donnez des cas d'usage concrets.

**Réponse :**

Un **DaemonSet** garantit qu'une copie d'un Pod tourne sur tous (ou certains) Nodes du cluster.

**Comportements clés :**
- 1 Pod par Node automatiquement
- Si un nouveau Node est ajouté → Pod créé automatiquement
- Si un Node est retiré → Pod supprimé automatiquement
- Bypass du Scheduler (placement direct sur les Nodes)

**Schéma :**
```
Cluster avec 4 Nodes :

Node-1: [daemon-xyz]
Node-2: [daemon-abc]
Node-3: [daemon-def]
Node-4: [daemon-ghi]

Nouveau Node-5 ajouté :
Node-5: [daemon-jkl]  ← Créé automatiquement
```

---

**Cas d'usage 1 : Collecte de logs**
```yaml
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: fluentd
  namespace: kube-system
spec:
  selector:
    matchLabels:
      name: fluentd
  template:
    metadata:
      labels:
        name: fluentd
    spec:
      containers:
      - name: fluentd
        image: fluent/fluentd-kubernetes-daemonset:v1.14
        volumeMounts:
        - name: varlog
          mountPath: /var/log
        - name: containers
          mountPath: /var/lib/docker/containers
          readOnly: true
      volumes:
      - name: varlog
        hostPath:
          path: /var/log
      - name: containers
        hostPath:
          path: /var/lib/docker/containers
```
Chaque Node a son agent Fluentd qui collecte les logs localement.

---

**Cas d'usage 2 : Monitoring des Nodes**
```yaml
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: node-exporter
  namespace: monitoring
spec:
  selector:
    matchLabels:
      app: node-exporter
  template:
    metadata:
      labels:
        app: node-exporter
    spec:
      hostNetwork: true  # Accès aux métriques du host
      hostPID: true
      containers:
      - name: node-exporter
        image: prom/node-exporter:v1.3.1
        ports:
        - containerPort: 9100
          hostPort: 9100
```
Prometheus scrape chaque Node individuellement pour les métriques système.

---

**Cas d'usage 3 : CNI Network Plugin**
```yaml
# Exemple : Calico DaemonSet
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: calico-node
  namespace: kube-system
spec:
  selector:
    matchLabels:
      k8s-app: calico-node
  template:
    spec:
      hostNetwork: true
      tolerations:
      - effect: NoSchedule
        operator: Exists  # Tourne même sur master nodes
      containers:
      - name: calico-node
        image: calico/node:v3.24.0
```
Gère le réseau sur chaque Node.

---

**Cas d'usage 4 : Storage Daemons**
```yaml
# Exemple : Rook Ceph
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: rook-ceph-agent
spec:
  template:
    spec:
      containers:
      - name: ceph-agent
        image: rook/ceph:v1.9.0
```
Agent de stockage sur chaque Node pour Ceph.

---

**Cas d'usage 5 : Security & Compliance**
```yaml
# Exemple : Falco (Runtime Security)
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: falco
spec:
  template:
    spec:
      hostNetwork: true
      hostPID: true
      containers:
      - name: falco
        image: falcosecurity/falco:latest
        securityContext:
          privileged: true
```
Détecte les comportements suspects sur chaque Node.

---

**DaemonSet avec NodeSelector :**
```yaml
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: gpu-monitoring
spec:
  selector:
    matchLabels:
      app: gpu-monitor
  template:
    metadata:
      labels:
        app: gpu-monitor
    spec:
      nodeSelector:
        accelerator: nvidia-gpu  # Seulement sur Nodes avec GPU
      containers:
      - name: nvidia-dcgm
        image: nvidia/dcgm-exporter:latest
```

**Commandes utiles :**
```bash
# Lister les DaemonSets
kubectl get daemonsets -A

# Vérifier qu'il y a bien 1 Pod par Node
kubectl get daemonset <name> -o wide
# DESIRED=4, CURRENT=4, READY=4 (si 4 Nodes)

# Update (rolling update comme Deployment)
kubectl set image daemonset/<name> <container>=<new-image>

# Rollout status
kubectl rollout status daemonset/<name>
```

**Différence avec Deployment répliqué sur tous les Nodes :**
- DaemonSet : 1 Pod par Node garanti
- Deployment avec replicas=N : N Pods distribués selon le Scheduler

---

### Q13 : Quelle est la différence entre un Job et un CronJob ?

**Réponse :**

**Job :**
- Exécute une tâche **une seule fois**
- S'assure qu'elle se termine avec succès
- Pod(s) créé(s) jusqu'à complétion

**CronJob :**
- Exécute des Jobs **selon un planning** (syntaxe cron)
- Crée des Jobs à intervalles réguliers
- Gère l'historique des Jobs créés

**Hiérarchie :**
```
CronJob "backup-db"
  ├─ Job "backup-db-1234567890" (créé à 02:00)
  │   └─ Pod "backup-db-1234567890-xyz"
  ├─ Job "backup-db-1234567891" (créé à 02:00 le lendemain)
  │   └─ Pod "backup-db-1234567891-abc"
  └─ ...
```

---

**Job - Exemple : Migration de base de données**
```yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: db-migration
spec:
  completions: 1       # 1 exécution réussie
  parallelism: 1       # 1 Pod à la fois
  backoffLimit: 3      # 3 tentatives max
  activeDeadlineSeconds: 600  # Timeout 10 min
  
  template:
    spec:
      restartPolicy: OnFailure  # Redémarre si échec
      containers:
      - name: migrate
        image: myapp/migrations:v1.2.0
        command: ["python", "manage.py", "migrate"]
        env:
        - name: DATABASE_URL
          valueFrom:
            secretKeyRef:
              name: db-secret
              key: url
```

```bash
# Créer et suivre
kubectl apply -f job.yaml
kubectl get jobs
kubectl logs job/db-migration

# Vérifier complétion
kubectl get job db-migration
# COMPLETIONS   DURATION   AGE
# 1/1           45s        2m

# Nettoyer
kubectl delete job db-migration
```

**Job avec parallélisme :**
```yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: batch-processing
spec:
  completions: 100     # 100 tâches total
  parallelism: 10      # 10 Pods en parallèle
  template:
    spec:
      restartPolicy: OnFailure
      containers:
      - name: processor
        image: batch-processor:1.0
        env:
        - name: TASK_ID
          valueFrom:
            fieldRef:
              fieldPath: metadata.name
```

**Comportement :**
```
1. Démarre 10 Pods (0-9)
2. Pod-0 termine → Pod-10 démarre
3. Continue jusqu'à 100 completions
```

---

**CronJob - Exemple : Backup quotidien**
```yaml
apiVersion: batch/v1
kind: CronJob
metadata:
  name: backup-database
spec:
  # Tous les jours à 2h du matin
  schedule: "0 2 * * *"
  
  # Timezone
  timeZone: "Europe/Paris"
  
  # Politique de concurrence
  concurrencyPolicy: Forbid  # N'exécute pas si le précédent tourne encore
  
  # Historique
  successfulJobsHistoryLimit: 3  # Garde 3 Jobs réussis
  failedJobsHistoryLimit: 1      # Garde 1 Job échoué
  
  # Deadline pour démarrer le Job
  startingDeadlineSeconds: 300   # Si en retard de 5 min, skip
  
  jobTemplate:
    spec:
      template:
        spec:
          restartPolicy: OnFailure
          containers:
          - name: backup
            image: postgres:14
            command:
            - /bin/sh
            - -c
            - |
              TIMESTAMP=$(date +%Y%m%d-%H%M%S)
              pg_dump -h postgres.prod.svc.cluster.local \
                      -U backup_user \
                      -d mydb \
                      > /backup/dump-${TIMESTAMP}.sql
              # Upload vers S3
              aws s3 cp /backup/dump-${TIMESTAMP}.sql \
                        s3://backups/postgres/
            env:
            - name: PGPASSWORD
              valueFrom:
                secretKeyRef:
                  name: postgres-secret
                  key: password
            - name: AWS_ACCESS_KEY_ID
              valueFrom:
                secretKeyRef:
                  name: aws-credentials
                  key: access-key
            volumeMounts:
            - name: backup-storage
              mountPath: /backup
          volumes:
          - name: backup-storage
            emptyDir: {}
```

**Syntaxe Schedule (Cron) :**
```
 ┌───────────── minute (0 - 59)
 │ ┌───────────── hour (0 - 23)
 │ │ ┌───────────── day of month (1 - 31)
 │ │ │ ┌───────────── month (1 - 12)
 │ │ │ │ ┌───────────── day of week (0 - 6) (Sunday=0)
 │ │ │ │ │
 * * * * *

Exemples :
"*/5 * * * *"      → Toutes les 5 minutes
"0 */2 * * *"      → Toutes les 2 heures
"0 0 * * *"        → Tous les jours à minuit
"0 2 * * *"        → Tous les jours à 2h
"0 0 * * 0"        → Tous les dimanches à minuit
"0 0 1 * *"        → Le 1er de chaque mois à minuit
"30 3 * * 1-5"     → Lundi-vendredi à 3h30
"0 9,17 * * *"     → Tous les jours à 9h et 17h
"0 0 1,15 * *"     → Le 1er et 15 de chaque mois
```

**Commandes CronJob :**
```bash
# Créer
kubectl apply -f cronjob.yaml

# Lister
kubectl get cronjobs
# NAME              SCHEDULE      SUSPEND   ACTIVE   LAST SCHEDULE   AGE
# backup-database   0 2 * * *     False     0        12h             5d

# Voir les Jobs créés
kubectl get jobs
# backup-database-1234567890   1/1    45s    13h
# backup-database-1234567891   1/1    42s    37h

# Déclencher manuellement
kubectl create job backup-manual --from=cronjob/backup-database

# Suspendre
kubectl patch cronjob backup-database -p '{"spec":{"suspend":true}}'

# Réactiver
kubectl patch cronjob backup-database -p '{"spec":{"suspend":false}}'
```

---

**Concurrency Policies :**

**1. Allow (default) :**
```yaml
concurrencyPolicy: Allow
# Plusieurs Jobs peuvent tourner en parallèle
# Si Job-1 prend 2h et schedule = 1h
# → Job-1 et Job-2 tournent ensemble
```

**2. Forbid :**
```yaml
concurrencyPolicy: Forbid
# Si Job-1 tourne encore quand Job-2 devrait démarrer
# → Job-2 est SKIPPED
```

**3. Replace :**
```yaml
concurrencyPolicy: Replace
# Si Job-1 tourne encore quand Job-2 devrait démarrer
# → Job-1 est KILLED, Job-2 démarre
```

---

**Différences clés :**

| Aspect | Job | CronJob |
|--------|-----|---------|
| **Fréquence** | Une fois | Récurrent |
| **Déclenchement** | Manuel ou automatique | Selon schedule |
| **Utilisation** | Migrations, batch processing | Backups, cleanups, reports |
| **Gestion** | Supprimé manuellement | Historique automatique |

**Cas d'usage Job :**
- Migration de DB
- Processing d'un batch de données
- Génération de rapports one-shot
- Init tasks (seeding DB)

**Cas d'usage CronJob :**
- Backups quotidiens
- Nettoyage de logs/data anciens
- Rapports périodiques (daily/weekly)
- Synchronisation de données
- Health checks planifiés
- Envoi d'emails récurrents

---

### Q14 : Comment gérez-vous un Job qui échoue systématiquement ?

**Réponse :**

**Étape 1 : Identifier la cause de l'échec**

```bash
# Vérifier le status du Job
kubectl get job failing-job
# COMPLETIONS   DURATION   AGE
# 0/1           5m         5m

# Voir les Pods créés
kubectl get pods -l job-name=failing-job
# NAME                 READY   STATUS             RESTARTS
# failing-job-abc      0/1     Error              0
# failing-job-def      0/1     CrashLoopBackOff   3
# failing-job-ghi      0/1     Error              0

# Logs du dernier Pod
kubectl logs failing-job-ghi

# Describe pour les events
kubectl describe job failing-job
```

---

**Étape 2 : Analyser la configuration du Job**

```yaml
spec:
  backoffLimit: 3  # ← Nombre de tentatives (défaut: 6)
  activeDeadlineSeconds: 600  # ← Timeout global
  
  template:
    spec:
      restartPolicy: OnFailure  # ou Never
```

**backoffLimit atteint :**
```bash
# Le Job a échoué 3 fois
kubectl describe job failing-job
# Events:
#   Type     Reason                Age   Message
#   Warning  BackoffLimitExceeded  2m    Job has reached the specified backoff limit
```

**Causes fréquentes :**

**1. Erreur applicative (exit code non-zero) :**
```bash
kubectl logs failing-job-abc
# ERROR: Database connection failed
# exit code 1

# Solution : Corriger le code/config
```

**2. Ressources insuffisantes (OOMKilled) :**
```bash
kubectl describe pod failing-job-abc
# Last State: Terminated
#   Reason: OOMKilled
#   Exit Code: 137

# Solution : Augmenter les limits
resources:
  limits:
    memory: "2Gi"  # Au lieu de 512Mi
```

**3. Timeout (activeDeadlineSeconds) :**
```bash
# Job killed après 600s
kubectl describe job failing-job
# Events:
#   Type     Reason            Age   Message
#   Warning  DeadlineExceeded  1m    Job was active longer than specified deadline

# Solution : Augmenter le timeout ou optimiser le traitement
activeDeadlineSeconds: 1800  # 30 minutes
```

**4. Dépendances manquantes :**
```bash
kubectl logs failing-job-abc
# ERROR: ConfigMap 'app-config' not found

# Solution : Créer le ConfigMap avant le Job
kubectl create configmap app-config --from-file=config.yaml
```

---

**Étape 3 : Stratégies de correction**

**A. Augmenter les tentatives :**
```yaml
spec:
  backoffLimit: 10  # Plus de tentatives
  template:
    spec:
      restartPolicy: OnFailure  # Redémarre le Pod en cas d'échec
```

**B. Ajouter un retry logic dans l'application :**
```yaml
containers:
- name: job
  image: myapp:1.0
  command:
  - /bin/sh
  - -c
  - |
    #!/bin/bash
    MAX_RETRIES=5
    RETRY=0
    
    until python process.py || [ $RETRY -eq $MAX_RETRIES ]; do
      RETRY=$((RETRY+1))
      echo "Retry $RETRY/$MAX_RETRIES in 10s..."
      sleep 10
    done
    
    if [ $RETRY -eq $MAX_RETRIES ]; then
      echo "Failed after $MAX_RETRIES attempts"
      exit 1
    fi
```

**C. Ajouter des init containers pour les dépendances :**
```yaml
spec:
  template:
    spec:
      initContainers:
      - name: wait-for-db
        image: busybox
        command:
        - sh
        - -c
        - |
          until nslookup postgres.default.svc.cluster.local; do
            echo "Waiting for DB..."
            sleep 5
          done
      containers:
      - name: job
        image: myapp:1.0
```

**D. Utiliser un CronJob avec concurrencyPolicy :**
```yaml
# Si le Job échoue temporairement (ex: service externe down)
# Un CronJob peut réessayer automatiquement
apiVersion: batch/v1
kind: CronJob
metadata:
  name: retry-job
spec:
  schedule: "*/10 * * * *"  # Toutes les 10 minutes
  concurrencyPolicy: Forbid
  successfulJobsHistoryLimit: 1
  jobTemplate:
    spec:
      template:
        spec:
          containers:
          - name: job
            image: myapp:1.0
```

---

**Étape 4 : Monitoring et Alerting**

```yaml
# Prometheus alert pour Jobs échoués
- alert: JobFailed
  expr: kube_job_status_failed{job="failing-job"} > 0
  for: 5m
  annotations:
    summary: "Job {{ $labels.job_name }} has failed"
    
- alert: JobRunningTooLong
  expr: time() - kube_job_status_start_time{job="backup-job"} > 3600
  annotations:
    summary: "Job {{ $labels.job_name }} running for > 1h"
```

**Script de monitoring :**
```bash
#!/bin/bash
# check-jobs.sh

FAILED_JOBS=$(kubectl get jobs -o json | \
  jq -r '.items[] | select(.status.failed > 0) | .metadata.name')

if [ -n "$FAILED_JOBS" ]; then
  echo "❌ Failed Jobs detected:"
  echo "$FAILED_JOBS"
  
  for job in $FAILED_JOBS; do
    echo "Logs for $job:"
    kubectl logs job/$job --tail=50
  done
  
  exit 1
fi

echo "✅ All Jobs successful"
```

---

**Étape 5 : Nettoyage et recréation**

```bash
# Supprimer le Job échoué
kubectl delete job failing-job

# Corriger le YAML
# Recréer
kubectl apply -f job-fixed.yaml

# Ou utiliser TTL pour auto-cleanup
apiVersion: batch/v1
kind: Job
metadata:
  name: auto-cleanup-job
spec:
  ttlSecondsAfterFinished: 100  # Supprimé 100s après complétion
  template:
    # ...
```

---

**Best practices pour Jobs robustes :**

```yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: robust-job
spec:
  # Stratégie de retry
  backoffLimit: 5
  activeDeadlineSeconds: 1800
  
  # TTL pour cleanup automatique
  ttlSecondsAfterFinished: 3600
  
  template:
    spec:
      restartPolicy: OnFailure
      
      # Init containers pour dépendances
      initContainers:
      - name: check-deps
        image: busybox
        command: ['sh', '-c', 'echo Checking dependencies...']
      
      containers:
      - name: job
        image: myapp:1.0
        
        # Ressources appropriées
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
        
        # Variables d'environnement
        env:
        - name: LOG_LEVEL
          value: "DEBUG"  # Plus de détails en cas d'échec
        
        # Liveness (si long running)
        livenessProbe:
          exec:
            command: ['sh', '-c', 'ps aux | grep python']
          initialDelaySeconds: 30
          periodSeconds: 60
```

---

## 🎯 Questions Situationnelles

### Q15 : Votre application Spring Boot fonctionne localement mais crash dans Kubernetes avec OOMKilled. Comment résolvez-vous ce problème ?

**Réponse :**

**Diagnostic :**

```bash
# 1. Vérifier le status
kubectl get pods
# NAME              READY   STATUS      RESTARTS
# spring-app-abc    0/1     OOMKilled   5

# 2. Describe pour confirmer
kubectl describe pod spring-app-abc
# Last State: Terminated
#   Reason: OOMKilled
#   Exit Code: 137

# 3. Voir les limites actuelles
kubectl get pod spring-app-abc -o yaml | grep -A 5 resources
# resources:
#   limits:
#     memory: "512Mi"  ← Trop bas pour Spring Boot
```

**Cause :**
Spring Boot a besoin de plus de mémoire que prévu. OOMKilled = le container a dépassé sa limite mémoire.

---

**Solutions :**

**1. Augmenter les memory limits :**
```yaml
resources:
  requests:
    memory: "1Gi"   # Réservation minimale
    cpu: "500m"
  limits:
    memory: "2Gi"   # Maximum autorisé (augmenté)
    cpu: "1000m"
```

**2. Configurer la JVM correctement :**

**Problème :** Par défaut, la JVM ne respecte pas les limites de container.

```yaml
env:
- name: JAVA_OPTS
  value: >-
    -XX:+UseContainerSupport
    -XX:MaxRAMPercentage=75.0
    -XX:InitialRAMPercentage=50.0
    -Xms512m
    -Xmx1536m
```

**Explication :**
- `-XX:+UseContainerSupport` : JVM détecte les limites du container
- `-XX:MaxRAMPercentage=75.0` : Utilise max 75% de la mémoire du container
- `-Xms512m` : Heap initial
- `-Xmx1536m` : Heap maximum (75% de 2Gi)

**3. Activer les heap dumps pour analyse :**
```yaml
env:
- name: JAVA_OPTS
  value: >-
    -XX:+HeapDumpOnOutOfMemoryError
    -XX:HeapDumpPath=/tmp/heapdump.hprof
    -XX:+ExitOnOutOfMemoryError

volumeMounts:
- name: heap-dumps
  mountPath: /tmp

volumes:
- name: heap-dumps
  emptyDir:
    sizeLimit: 4Gi  # Espace pour les dumps
```

**Récupérer le heap dump :**
```bash
# Copier avant que le Pod soit supprimé
kubectl cp spring-app-abc:/tmp/heapdump.hprof ./heapdump.hprof

# Analyser avec MAT (Memory Analyzer Tool) ou VisualVM
```

**4. Optimiser l'application :**

**a) Profiler la mémoire en local :**
```bash
java -XX:+PrintFlagsFinal -version | grep -i heap
# MaxHeapSize, InitialHeapSize

# Run avec profiler
java -agentlib:jdwp=transport=dt_socket,server=y,suspend=n,address=5005 \
     -jar app.jar
```

**b) Identifier les memory leaks :**
- Connexions DB non fermées
- Caches sans limite
- Collections qui grandissent indéfiniment

**c) Ajuster les Spring Boot properties :**
```yaml
# application.yml
spring:
  jpa:
    hibernate:
      ddl-auto: none  # Pas de génération de schema au démarrage
    show-sql: false   # Pas de logs SQL verbeux
  
server:
  tomcat:
    threads:
      max: 50        # Limiter les threads
    max-connections: 100
```

**5. Utiliser un startup probe si le démarrage est lent :**
```yaml
startupProbe:
  httpGet:
    path: /actuator/health
    port: 8080
  failureThreshold: 30  # 30 * 10 = 300s max pour démarrer
  periodSeconds: 10

livenessProbe:
  httpGet:
    path: /actuator/health/liveness
    port: 8080
  initialDelaySeconds: 0  # startup probe prend le relais
  periodSeconds: 10

readinessProbe:
  httpGet:
    path: /actuator/health/readiness
    port: 8080
  initialDelaySeconds: 0
  periodSeconds: 5
```

---

**Configuration complète optimisée :**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: spring-app
spec:
  replicas: 3
  template:
    spec:
      containers:
      - name: app
        image: myapp/spring-boot:1.0
        
        # Ressources appropriées pour Spring Boot
        resources:
          requests:
            memory: "1Gi"
            cpu: "500m"
          limits:
            memory: "2Gi"
            cpu: "1000m"
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          periodSeconds: 5
---
# Service qui pointe initialement vers Blue
apiVersion: v1
kind: Service
metadata:
  name: spring-app
  labels:
    app: spring-app
spec:
  selector:
    app: spring-app
    version: blue  # ← Pointe vers Blue
  ports:
  - port: 80
    targetPort: 8080
    protocol: TCP
  type: ClusterIP
```

**2. Déployer Green et tester :**

```bash
# Déployer Green (nouvelle version)
kubectl apply -f deployment-green.yaml

# Attendre que tous les Pods Green soient Ready
kubectl rollout status deployment/spring-app-green
# deployment "spring-app-green" successfully rolled out

# Vérifier les Pods
kubectl get pods -l version=green
# NAME                               READY   STATUS    RESTARTS   AGE
# spring-app-green-abc123-xyz        1/1     Running   0          2m
# spring-app-green-abc123-def        1/1     Running   0          2m
# spring-app-green-abc123-ghi        1/1     Running   0          2m

# Tester Green directement (port-forward)
kubectl port-forward deployment/spring-app-green 8081:8080

# Tester dans un autre terminal
curl http://localhost:8081/actuator/health
curl http://localhost:8081/api/version
# {"version": "2.0-green"}

# Tests fonctionnels complets sur Green
# - Health checks
# - Endpoints critiques
# - Performance tests
# - Integration tests
```

**3. Switch instantané vers Green :**

```bash
# Méthode 1 : Patch du Service
kubectl patch service spring-app -p '{"spec":{"selector":{"version":"green"}}}'

# Méthode 2 : Edit manuel
kubectl edit service spring-app
# Changer version: blue → version: green

# Méthode 3 : Apply d'un nouveau YAML
kubectl apply -f service-green.yaml
```

**Vérification immédiate :**
```bash
# Le Service pointe maintenant vers Green
kubectl describe service spring-app
# Selector: app=spring-app,version=green
# Endpoints: 10.244.1.10:8080,10.244.1.11:8080,10.244.1.12:8080

# Tester depuis un Pod dans le cluster
kubectl run test --rm -it --image=curlimages/curl -- sh
# curl http://spring-app/api/version
# {"version": "2.0-green"}
```

**4. Monitoring post-switch :**

```bash
# Surveiller les métriques
kubectl top pods -l version=green

# Logs en temps réel
kubectl logs -f deployment/spring-app-green

# Prometheus queries
rate(http_requests_total{version="green",status="500"}[5m])
histogram_quantile(0.99, rate(http_request_duration_seconds_bucket{version="green"}[5m]))

# Si tout va bien pendant 30 minutes → succès
```

**5. Rollback instantané si problème :**

```bash
# Revenir vers Blue immédiatement
kubectl patch service spring-app -p '{"spec":{"selector":{"version":"blue"}}}'

# Vérifier
kubectl describe service spring-app
# Selector: app=spring-app,version=blue

# Blue était toujours running → switch instantané, zéro downtime
```

**6. Cleanup après validation complète :**

```bash
# Après 24h-48h sans incident, supprimer Blue
kubectl delete deployment spring-app-blue

# Optionnel : Renommer Green en Blue pour le prochain cycle
kubectl patch deployment spring-app-green --type=json \
  -p='[{"op": "replace", "path": "/metadata/name", "value": "spring-app-blue"}]'
```

---

**Améliorations avancées :**

**A. Services séparés pour tester Green :**
```yaml
# Service public (production)
apiVersion: v1
kind: Service
metadata:
  name: spring-app
spec:
  selector:
    app: spring-app
    version: blue
  ports:
  - port: 80
    targetPort: 8080
---
# Service interne pour tester Green
apiVersion: v1
kind: Service
metadata:
  name: spring-app-green-test
spec:
  selector:
    app: spring-app
    version: green
  ports:
  - port: 80
    targetPort: 8080
```

**B. Ingress avec poids de trafic (progressive) :**
```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: spring-app
  annotations:
    nginx.ingress.kubernetes.io/canary: "true"
    nginx.ingress.kubernetes.io/canary-weight: "10"  # 10% vers Green
spec:
  rules:
  - host: myapp.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: spring-app-green
            port:
              number: 80
```

**C. Script automatisé :**
```bash
#!/bin/bash
# blue-green-deploy.sh

set -e

CURRENT_VERSION=$(kubectl get service spring-app -o jsonpath='{.spec.selector.version}')
NEW_VERSION="green"

if [ "$CURRENT_VERSION" = "green" ]; then
  NEW_VERSION="blue"
fi

echo "Current version: $CURRENT_VERSION"
echo "New version: $NEW_VERSION"

# Déployer la nouvelle version
kubectl apply -f deployment-$NEW_VERSION.yaml

# Attendre que tous les Pods soient Ready
kubectl rollout status deployment/spring-app-$NEW_VERSION

# Tests automatisés
echo "Running smoke tests..."
kubectl run test-$NEW_VERSION --rm -it --image=curlimages/curl -- \
  curl -f http://spring-app-$NEW_VERSION-test/actuator/health

# Switch
read -p "Switch traffic to $NEW_VERSION? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
  kubectl patch service spring-app -p "{\"spec\":{\"selector\":{\"version\":\"$NEW_VERSION\"}}}"
  echo "✅ Traffic switched to $NEW_VERSION"
else
  echo "❌ Deployment cancelled"
  exit 1
fi

# Monitoring
echo "Monitoring for 5 minutes..."
for i in {1..5}; do
  ERROR_RATE=$(kubectl logs deployment/spring-app-$NEW_VERSION --tail=100 | grep -c ERROR || true)
  echo "Minute $i: $ERROR_RATE errors"
  sleep 60
done

echo "✅ Blue/Green deployment completed"
```

---

**Comparaison avec d'autres stratégies :**

| Aspect | Blue/Green | Rolling Update | Canary |
|--------|------------|----------------|--------|
| **Downtime** | Zéro | Zéro | Zéro |
| **Rollback** | Instantané | Progressive | Instantané |
| **Coût ressources** | 2x (blue+green) | ~1.25x | ~1.1x |
| **Complexité** | Moyenne | Faible | Élevée |
| **Testing** | Complet avant switch | Progressif | Progressif |
| **Use case** | Critical apps | Standard apps | Risk averse |

---

### Q17 : Un Pod StatefulSet est en Pending. Comment debugger ?

**Réponse :**

**Symptômes :**
```bash
kubectl get statefulset postgres
# NAME       READY   AGE
# postgres   2/3     10m

kubectl get pods -l app=postgres
# NAME         READY   STATUS    RESTARTS   AGE
# postgres-0   1/1     Running   0          10m
# postgres-1   1/1     Running   0          9m
# postgres-2   0/1     Pending   0          5m  ← Bloqué
```

---

**Causes possibles et solutions :**

**1. PVC en attente (volume non disponible) :**

```bash
# Vérifier les PVCs
kubectl get pvc
# NAME                    STATUS    VOLUME   CAPACITY   STORAGECLASS
# data-postgres-0         Bound     pv-1     10Gi       fast-ssd
# data-postgres-1         Bound     pv-2     10Gi       fast-ssd
# data-postgres-2         Pending   -        -          fast-ssd  ← Problème

# Describe le PVC
kubectl describe pvc data-postgres-2
# Events:
#   Warning  ProvisioningFailed  2m  persistentvolume-controller
#   Failed to provision volume: StorageClass "fast-ssd" not found
```

**Solutions :**

**a) StorageClass manquante :**
```bash
# Vérifier les StorageClasses disponibles
kubectl get storageclass

# Corriger le StatefulSet
kubectl edit statefulset postgres
# Changer storageClassName vers une SC existante
```

**b) Pas de PV disponibles (provisionning manuel) :**
```bash
# Créer un PV manuellement
kubectl apply -f - <<EOF
apiVersion: v1
kind: PersistentVolume
metadata:
  name: pv-postgres-2
spec:
  capacity:
    storage: 10Gi
  accessModes:
  - ReadWriteOnce
  persistentVolumeReclaimPolicy: Retain
  storageClassName: fast-ssd
  hostPath:
    path: /data/postgres-2
EOF

# Le PVC se bind automatiquement
```

**c) Quota de volume atteint :**
```bash
kubectl describe namespace default | grep -A 5 "Resource Quotas"
# Used: persistentvolumeclaims: 10/10  ← Quota atteint

# Augmenter le quota ou nettoyer des PVCs inutilisés
```

---

**2. Ressources Node insuffisantes :**

```bash
kubectl describe pod postgres-2
# Events:
#   Warning  FailedScheduling  1m  0/3 nodes are available:
#   3 Insufficient memory

# Vérifier les ressources des Nodes
kubectl describe nodes | grep -A 5 "Allocated resources"
# CPU Requests: 7500m (95% of 8000m)
# Memory Requests: 28Gi (90% of 32Gi)
```

**Solutions :**

**a) Réduire les requests du Pod :**
```yaml
spec:
  template:
    spec:
      containers:
      - name: postgres
        resources:
          requests:
            memory: "2Gi"  # Au lieu de 4Gi
            cpu: "500m"
```

**b) Ajouter un Node au cluster :**
```bash
# Selon votre provider (GKE, EKS, AKS)
gcloud container clusters resize my-cluster --num-nodes=4
```

**c) Scaler down d'autres workloads :**
```bash
kubectl scale deployment low-priority-app --replicas=0
```

---

**3. Node avec Taint incompatible :**

```bash
kubectl describe pod postgres-2
# Events:
#   Warning  FailedScheduling  1m  0/3 nodes are available:
#   3 node(s) had taints that the pod didn't tolerate

# Vérifier les taints
kubectl get nodes -o json | jq '.items[].spec.taints'
# [
#   {
#     "effect": "NoSchedule",
#     "key": "dedicated",
#     "value": "gpu-workload"
#   }
# ]
```

**Solution - Ajouter des tolerations :**
```yaml
spec:
  template:
    spec:
      tolerations:
      - key: "dedicated"
        operator: "Equal"
        value: "gpu-workload"
        effect: "NoSchedule"
```

---

**4. Affinity/Anti-affinity non satisfiable :**

```bash
kubectl get pod postgres-2 -o yaml | grep -A 20 affinity
# podAntiAffinity:
#   requiredDuringSchedulingIgnoredDuringExecution:
#   - labelSelector:
#       matchLabels:
#         app: postgres
#     topologyKey: kubernetes.io/hostname

# Vérifier la topologie
kubectl get nodes --show-labels | grep postgres
# Tous les Nodes ont déjà un Pod postgres → anti-affinity empêche le placement
```

**Solution :**
```yaml
# Changer en preferredDuringScheduling (soft affinity)
podAntiAffinity:
  preferredDuringSchedulingIgnoredDuringExecution:
  - weight: 100
    podAffinityTerm:
      labelSelector:
        matchLabels:
          app: postgres
      topologyKey: kubernetes.io/hostname
```

---

**5. StatefulSet bloqué par le Pod précédent :**

StatefulSets créent les Pods de manière **séquentielle**.

```bash
kubectl get pods
# NAME         READY   STATUS             RESTARTS   AGE
# postgres-0   1/1     Running            0          10m
# postgres-1   0/1     CrashLoopBackOff   5          9m  ← Pas Ready
# postgres-2   0/1     Pending            0          5m  ← Attend postgres-1

# postgres-2 ne démarrera JAMAIS tant que postgres-1 n'est pas Ready
```

**Solution - Fixer postgres-1 d'abord :**
```bash
kubectl logs postgres-1
kubectl describe pod postgres-1

# Corriger le problème, puis postgres-2 démarrera automatiquement
```

---

**6. Headless Service manquant :**

```bash
kubectl describe pod postgres-2
# Events:
#   Warning  FailedScheduling  1m  service "postgres-headless" not found

# Vérifier
kubectl get service postgres-headless
# Error: services "postgres-headless" not found
```

**Solution - Créer le Headless Service :**
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

---

**Checklist de debugging complète :**

```bash
# 1. Pod status et events
kubectl describe pod postgres-2

# 2. PVC status
kubectl get pvc data-postgres-2
kubectl describe pvc data-postgres-2

# 3. StorageClass
kubectl get storageclass
kubectl describe storageclass fast-ssd

# 4. Node resources
kubectl describe nodes

# 5. Node taints
kubectl get nodes -o custom-columns=NAME:.metadata.name,TAINTS:.spec.taints

# 6. StatefulSet status
kubectl describe statefulset postgres

# 7. Pods précédents (séquence)
kubectl get pods -l app=postgres --sort-by=.metadata.name

# 8. Events cluster
kubectl get events --sort-by='.lastTimestamp' | grep postgres

# 9. Headless Service
kubectl get service postgres-headless
```

---

**Script de diagnostic automatique :**
```bash
#!/bin/bash
# debug-statefulset.sh

STATEFULSET=$1
POD=$2

echo "=== Pod Status ==="
kubectl get pod $POD -o wide

echo "=== Pod Events ==="
kubectl describe pod $POD | grep -A 10 Events

echo "=== PVC Status ==="
kubectl get pvc | grep $(echo $POD | sed 's/-.$//')

echo "=== Node Resources ==="
NODE=$(kubectl get pod $POD -o jsonpath='{.spec.nodeName}')
if [ -n "$NODE" ]; then
  kubectl describe node $NODE | grep -A 5 "Allocated resources"
else
  echo "Pod not scheduled yet"
fi

echo "=== StatefulSet Status ==="
kubectl get statefulset $STATEFULSET

echo "=== Previous Pods (Sequential) ==="
kubectl get pods -l app=$(kubectl get statefulset $STATEFULSET -o jsonpath='{.spec.selector.matchLabels.app}') \
  --sort-by=.metadata.name

echo "=== Headless Service ==="
SERVICE=$(kubectl get statefulset $STATEFULSET -o jsonpath='{.spec.serviceName}')
kubectl get service $SERVICE 2>/dev/null || echo "Service $SERVICE not found!"
```

**Utilisation :**
```bash
./debug-statefulset.sh postgres postgres-2
```

---

### Q18 : Comment testeriez-vous un Deployment avant de le pousser en production ?

**Réponse :**

**Approche multi-étapes :**

---

**1. Validation locale (avant commit) :**

```bash
# A. Lint YAML
yamllint deployment.yaml

# B. Validation Kubernetes
kubectl apply --dry-run=client -f deployment.yaml
# Vérifie la syntaxe sans créer les ressources

# C. Validation avec server-side dry-run
kubectl apply --dry-run=server -f deployment.yaml
# Vérifie aussi les RBAC, admission controllers

# D. kubeval (validation de schema)
kubeval deployment.yaml

# E. kube-score (best practices)
kube-score score deployment.yaml
# Détecte: ressources manquantes, probes manquantes, etc.
```

---

**2. Tests dans un namespace de dev :**

```bash
# Créer un namespace isolé
kubectl create namespace dev-test

# Déployer
kubectl apply -f deployment.yaml -n dev-test

# Vérifier le rollout
kubectl rollout status deployment/spring-app -n dev-test

# Vérifier les Pods
kubectl get pods -n dev-test
# Tous doivent être Running et Ready

# Health checks
kubectl get pods -n dev-test -o jsonpath='{range .items[*]}{.metadata.name}{"\t"}{.status.conditions[?(@.type=="Ready")].status}{"\n"}{end}'
```

---

**3. Tests fonctionnels automatisés :**

```yaml
# test-job.yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: integration-tests
  namespace: dev-test
spec:
  template:
    spec:
      restartPolicy: Never
      containers:
      - name: tests
        image: myapp/tests:latest
        command:
        - pytest
        - -v
        - --junit-xml=/results/junit.xml
        env:
        - name: APP_URL
          value: "http://spring-app.dev-test.svc.cluster.local"
        volumeMounts:
        - name: results
          mountPath: /results
      volumes:
      - name: results
        emptyDir: {}
```

```bash
# Lancer les tests
kubectl apply -f test-job.yaml

# Attendre la complétion
kubectl wait --for=condition=complete --timeout=600s job/integration-tests -n dev-test

# Récupérer les résultats
kubectl logs job/integration-tests -n dev-test
```

---

**4. Tests de performance (load testing) :**

```yaml
# load-test.yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: load-test
  namespace: dev-test
spec:
  parallelism: 10  # 10 Pods en parallèle
  template:
    spec:
      restartPolicy: Never
      containers:
      - name: k6
        image: grafana/k6:latest
        args:
        - run
        - --vus=100        # 100 virtual users
        - --duration=5m
        - /scripts/load-test.js
        volumeMounts:
        - name: scripts
          mountPath: /scripts
      volumes:
      - name: scripts
        configMap:
          name: k6-scripts
```

```javascript
// k6-script (ConfigMap)
import http from 'k6/http';
import { check, sleep } from 'k6';

export default function () {
  const res = http.get('http://spring-app.dev-test.svc.cluster.local/api/health');
  check(res, {
    'status is 200': (r) => r.status === 200,
    'response time < 200ms': (r) => r.timings.duration < 200,
  });
  sleep(1);
}
```

---

**5. Smoke tests en production (avec Canary) :**

```bash
# Déployer en Canary (10% du trafic)
kubectl apply -f deployment-canary.yaml

# Créer un Service Canary pour tests directs
kubectl apply -f service-canary.yaml

# Smoke tests automatiques
kubectl run smoke-test --rm -it --image=curlimages/curl -- sh -c '
  for i in {1..100}; do
    curl -f http://spring-app-canary/actuator/health || exit 1
  done
  echo "✅ Smoke tests passed"
'

# Monitoring des métriques pendant 15 minutes
watch -n 30 'kubectl top pods -l version=canary'
```

---

**6. Validation avec Policy Engines :**

**OPA (Open Policy Agent) :**
```rego
# policy.rego
package kubernetes.admission

deny[msg] {
  input.request.kind.kind == "Deployment"
  not input.request.object.spec.template.spec.containers[_].resources.limits.memory
  msg = "Deployment must have memory limits"
}

deny[msg] {
  input.request.kind.kind == "Deployment"
  not input.request.object.spec.template.spec.containers[_].livenessProbe
  msg = "Deployment must have liveness probe"
}
```

```bash
# Tester la policy
conftest test deployment.yaml -p policy.rego
```

**Kyverno :**
```yaml
apiVersion: kyverno.io/v1
kind: ClusterPolicy
metadata:
  name: require-probes
spec:
  validationFailureAction: enforce
  rules:
  - name: check-liveness-probe
    match:
      resources:
        kinds:
        - Deployment
    validate:
      message: "Liveness probe is required"
      pattern:
        spec:
          template:
            spec:
              containers:
              - livenessProbe:
                  httpGet:
                    path: "*"
```

---

**7. Checklist finale avant production :**

```yaml
# checklist.yaml
pre-production-checks:
  deployment:
    ✓ replicas: >= 3 (HA)
    ✓ resources.requests: defined
    ✓ resources.limits: defined
    ✓ livenessProbe: configured
    ✓ readinessProbe: configured
    ✓ startupProbe: configured (if slow startup)
    ✓ strategy: RollingUpdate with maxUnavailable < 50%
    ✓ revisionHistoryLimit: defined
    ✓ image: with specific tag (not :latest)
  
  security:
    ✓ securityContext.runAsNonRoot: true
    ✓ securityContext.readOnlyRootFilesystem: true (if possible)
    ✓ No secrets in env (use secretKeyRef)
    ✓ NetworkPolicy: defined
    ✓ ServiceAccount: dedicated (not default)
  
  observability:
    ✓ labels: app, version, environment
    ✓ annotations: prometheus scraping configured
    ✓ logs: JSON structured
    ✓ metrics: exposed (actuator)
    ✓ tracing: enabled (if applicable)
  
  resilience:
    ✓ PodDisruptionBudget: defined
    ✓ Affinity/Anti-affinity: configured for HA
    ✓ terminationGracePeriodSeconds: >= 30
    ✓ preStop hook: graceful shutdown
  
  testing:
    ✓ Integration tests: passed
    ✓ Load tests: passed (p99 < SLO)
    ✓ Chaos tests: passed (if critical)
    ✓ Rollback plan: documented
```

---

**8. Pipeline CI/CD complet :**

```yaml
# .gitlab-ci.yml (exemple)
stages:
  - validate
  - test
  - deploy-dev
  - test-dev
  - deploy-staging
  - deploy-prod

validate:
  stage: validate
  script:
    - yamllint k8s/
    - kubeval k8s/*.yaml
    - kube-score score k8s/deployment.yaml

unit-tests:
  stage: test
  script:
    - mvn test

build-image:
  stage: test
  script:
    - docker build -t myapp:$CI_COMMIT_SHA .
    - docker push myapp:$CI_COMMIT_SHA

deploy-dev:
  stage: deploy-dev
  script:
    - kubectl apply -f k8s/ -n dev
    - kubectl set image deployment/spring-app app=myapp:$CI_COMMIT_SHA -n dev
    - kubectl rollout status deployment/spring-app -n dev

integration-tests:
  stage: test-dev
  script:
    - kubectl apply -f k8s/test-job.yaml -n dev
    - kubectl wait --for=condition=complete job/integration-tests -n dev
    - kubectl logs job/integration-tests -n dev

load-tests:
  stage: test-dev
  script:
    - kubectl apply -f k8s/load-test.yaml -n dev
    - kubectl wait --for=condition=complete job/load-test -n dev

deploy-staging:
  stage: deploy-staging
  when: manual
  script:
    - kubectl apply -f k8s/ -n staging
    - kubectl set image deployment/spring-app app=myapp:$CI_COMMIT_SHA -n staging

deploy-prod:
  stage: deploy-prod
  when: manual
  only:
    - main
  script:
    - kubectl apply -f k8s/ -n prod
    - kubectl set image deployment/spring-app app=myapp:$CI_COMMIT_SHA -n prod
    - ./scripts/canary-deploy.sh
```

---

**9. Monitoring post-déploiement :**

```bash
# Métriques Prometheus
# Taux d'erreur
sum(rate(http_requests_total{status=~"5..",job="spring-app"}[5m]))
/ sum(rate(http_requests_total{job="spring-app"}[5m]))

# Latence p99
histogram_quantile(0.99, 
  rate(http_request_duration_seconds_bucket{job="spring-app"}[5m]))

# Taux de redémarrage des Pods
rate(kube_pod_container_status_restarts_total{pod=~"spring-app.*"}[5m])

# Alerts
kubectl apply -f - <<EOF
apiVersion: monitoring.coreos.com/v1
kind: PrometheusRule
metadata:
  name: spring-app-alerts
spec:
  groups:
  - name: spring-app
    rules:
    - alert: HighErrorRate
      expr: sum(rate(http_requests_total{status=~"5..",job="spring-app"}[5m])) > 0.05
      for: 5m
      annotations:
        summary: "High error rate detected"
    - alert: HighLatency
      expr: histogram_quantile(0.99, rate(http_request_duration_seconds_bucket[5m])) > 1
      for: 10m
      annotations:
        summary: "High latency detected (p99 > 1s)"
EOF
```

---

## 🎓 Conseils pour l'Entretien - Chapitre 2

### Points clés à maîtriser :
1. **Pods** : Lifecycle, multi-container patterns, debugging
2. **Deployments** : Rolling updates, rollbacks, stratégies (Blue/Green, Canary)
3. **StatefulSets** : Différences vs Deployments, cas d'usage DB
4. **DaemonSets** : 1 Pod par Node, use cases monitoring/logs
5. **Jobs/CronJobs** : One-shot vs récurrent, syntaxe cron

### Questions bonus à anticiper :
- "Comment gérez-vous les secrets dans vos Deployments ?"
- "Expliquez votre stratégie de CI/CD avec Kubernetes"
- "Comment assurez-vous la haute disponibilité de vos applications ?"
- "Quelle est votre approche pour le monitoring et les alertes ?"

### Red flags à éviter :
- ❌ Utiliser `latest` comme tag d'image
- ❌ Pas de resources requests/limits
- ❌ Pas de probes (liveness/readiness)
- ❌ Déployer directement en prod sans tests

### Points bonus :
- ✅ Mentionner GitOps (ArgoCD, Flux)
- ✅ Parler de Helm pour la gestion des déploiements
- ✅ Évoquer les PodDisruptionBudgets pour la HA
- ✅ Connaître les patterns de déploiement avancés

---

**Prêt pour le Chapitre 3 : Configuration & Secrets Management ? 🚀**"
            cpu: "500m"
          limits:
            memory: "2Gi"
            cpu: "1000m"
        
        # JVM configurée pour containers
        env:
        - name: JAVA_OPTS
          value: >-
            -XX:+UseContainerSupport
            -XX:MaxRAMPercentage=75.0
            -XX:InitialRAMPercentage=50.0
            -XX:+HeapDumpOnOutOfMemoryError
            -XX:HeapDumpPath=/tmp/heapdump.hprof
            -XX:+UseG1GC
            -XX:MaxGCPauseMillis=100
            -Djava.security.egd=file:/dev/./urandom
        
        - name: SPRING_PROFILES_ACTIVE
          value: "prod"
        
        # Probes adaptées
        startupProbe:
          httpGet:
            path: /actuator/health
            port: 8080
          failureThreshold: 30
          periodSeconds: 10
        
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8080
          periodSeconds: 10
          failureThreshold: 3
        
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          periodSeconds: 5
          failureThreshold: 3
        
        # Graceful shutdown
        lifecycle:
          preStop:
            exec:
              command: ["/bin/sh", "-c", "sleep 10"]
        
        volumeMounts:
        - name: heap-dumps
          mountPath: /tmp
      
      volumes:
      - name: heap-dumps
        emptyDir:
          sizeLimit: 4Gi
      
      # Termination grace period
      terminationGracePeriodSeconds: 30
```

---

**Monitoring post-fix :**
```bash
# Vérifier que les Pods ne crash plus
kubectl get pods -w

# Surveiller la consommation mémoire
kubectl top pods

# Activer Prometheus pour tracking
# Métriques JVM via spring-boot-actuator
jvm_memory_used_bytes
jvm_memory_max_bytes
jvm_gc_pause_seconds
```

---

### Q16 : Comment feriez-vous un déploiement Blue/Green d'une application Spring Boot ?

**Réponse :**

**Stratégie Blue/Green :**
- **Blue** : version actuellement en production
- **Green** : nouvelle version déployée en parallèle
- **Switch instantané** : redirection du trafic de Blue vers Green

**Avantages :**
- Zéro downtime
- Rollback instantané
- Test complet de Green avant switch

---

**Implémentation :**

**1. Créer les deux Deployments :**

```yaml
# Blue (version actuelle v1.0)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: spring-app-blue
  labels:
    app: spring-app
    version: blue
spec:
  replicas: 3
  selector:
    matchLabels:
      app: spring-app
      version: blue
  template:
    metadata:
      labels:
        app: spring-app
        version: blue
    spec:
      containers:
      - name: app
        image: myapp/spring-boot:1.0
        ports:
        - containerPort: 8080
        env:
        - name: VERSION
          value: "1.0-blue"
        resources:
          requests:
            memory: "1Gi"
            cpu: "500m"
          limits:
            memory: "2Gi"
            cpu: "1000m"
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          periodSeconds: 5
---
# Green (nouvelle version v2.0)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: spring-app-green
  labels:
    app: spring-app
    version: green
spec:
  replicas: 3
  selector:
    matchLabels:
      app: spring-app
      version: green
  template:
    metadata:
      labels:
        app: spring-app
        version: green
    spec:
      containers:
      - name: app
        image: myapp/spring-boot:2.0  # ← Nouvelle version
        ports:
        - containerPort: 8080
        env:
        - name: VERSION
          value: "2.0-green"
        resources:
          requests:
            memory: "1Gi