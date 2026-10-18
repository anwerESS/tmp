## [**..**](./00_index.md)


# Chapitre 12 : Advanced Topics & Best Practices

## 12.1. Custom Resources (CRDs) & Operators

### Qu'est-ce qu'une Custom Resource Definition (CRD) ?

Une **CRD** permet d'étendre l'API Kubernetes en créant vos propres types de ressources.

**Cas d'usage :**
- Gérer des applications complexes (bases de données, message brokers)
- Abstraire la complexité pour les développeurs
- Automatiser des opérations (backup, scaling custom)

---

### Exemple : CRD pour une Application Java

**Définir la CRD :**

```yaml
apiVersion: apiextensions.k8s.io/v1
kind: CustomResourceDefinition
metadata:
  name: springapps.company.com
spec:
  group: company.com
  versions:
    - name: v1
      served: true
      storage: true
      schema:
        openAPIV3Schema:
          type: object
          properties:
            spec:
              type: object
              properties:
                image:
                  type: string
                replicas:
                  type: integer
                  minimum: 1
                  maximum: 10
                javaOpts:
                  type: string
                resources:
                  type: object
                  properties:
                    memory:
                      type: string
                    cpu:
                      type: string
                database:
                  type: object
                  properties:
                    host:
                      type: string
                    name:
                      type: string
              required:
                - image
                - replicas
  scope: Namespaced
  names:
    plural: springapps
    singular: springapp
    kind: SpringApp
    shortNames:
      - sa
```

**Appliquer la CRD :**

```bash
kubectl apply -f springapp-crd.yaml

# Vérifier
kubectl get crds | grep springapp
# springapps.company.com
```

---

### Utiliser la Custom Resource

**Créer une instance :**

```yaml
apiVersion: company.com/v1
kind: SpringApp
metadata:
  name: my-spring-app
  namespace: production
spec:
  image: registry.company.com/my-app:v1.2.3
  replicas: 3
  javaOpts: "-XX:MaxRAMPercentage=75.0 -XX:+UseG1GC"
  resources:
    memory: "1Gi"
    cpu: "1000m"
  database:
    host: postgres-service
    name: mydb
```

```bash
kubectl apply -f my-spring-app.yaml

# Lister les SpringApps
kubectl get springapps
# NAME            AGE
# my-spring-app   30s
```

---

### Operator Pattern

Un **Operator** est un contrôleur qui surveille les Custom Resources et crée/met à jour les ressources Kubernetes standard.

**Architecture :**

```
SpringApp CR → Operator (controller) → 
  Crée : Deployment, Service, ConfigMap, Secret, Ingress
```

**Exemple d'Operator en Java (Fabric8 Kubernetes Client) :**

```java
import io.fabric8.kubernetes.client.KubernetesClient;
import io.fabric8.kubernetes.client.KubernetesClientBuilder;
import io.fabric8.kubernetes.client.informers.ResourceEventHandler;

public class SpringAppOperator {
    
    public static void main(String[] args) {
        KubernetesClient client = new KubernetesClientBuilder().build();
        
        // Watch SpringApp resources
        client.resources(SpringApp.class)
            .inAnyNamespace()
            .inform(new ResourceEventHandler<SpringApp>() {
                
                @Override
                public void onAdd(SpringApp springApp) {
                    System.out.println("SpringApp added: " + springApp.getMetadata().getName());
                    reconcile(client, springApp);
                }
                
                @Override
                public void onUpdate(SpringApp oldApp, SpringApp newApp) {
                    System.out.println("SpringApp updated: " + newApp.getMetadata().getName());
                    reconcile(client, newApp);
                }
                
                @Override
                public void onDelete(SpringApp springApp, boolean deletedFinalStateUnknown) {
                    System.out.println("SpringApp deleted: " + springApp.getMetadata().getName());
                    cleanup(client, springApp);
                }
            });
        
        // Keep running
        Thread.currentThread().join();
    }
    
    private static void reconcile(KubernetesClient client, SpringApp app) {
        String name = app.getMetadata().getName();
        String namespace = app.getMetadata().getNamespace();
        
        // 1. Créer/mettre à jour Deployment
        createOrUpdateDeployment(client, app);
        
        // 2. Créer/mettre à jour Service
        createOrUpdateService(client, app);
        
        // 3. Créer/mettre à jour ConfigMap
        createOrUpdateConfigMap(client, app);
    }
    
    private static void createOrUpdateDeployment(KubernetesClient client, SpringApp app) {
        String name = app.getMetadata().getName();
        String namespace = app.getMetadata().getNamespace();
        
        Deployment deployment = new DeploymentBuilder()
            .withNewMetadata()
                .withName(name)
                .withNamespace(namespace)
                .addToLabels("app", name)
                .addToLabels("managed-by", "springapp-operator")
            .endMetadata()
            .withNewSpec()
                .withReplicas(app.getSpec().getReplicas())
                .withNewSelector()
                    .addToMatchLabels("app", name)
                .endSelector()
                .withNewTemplate()
                    .withNewMetadata()
                        .addToLabels("app", name)
                    .endMetadata()
                    .withNewSpec()
                        .addNewContainer()
                            .withName("app")
                            .withImage(app.getSpec().getImage())
                            .addNewPort()
                                .withContainerPort(8080)
                            .endPort()
                            .addNewEnv()
                                .withName("JAVA_OPTS")
                                .withValue(app.getSpec().getJavaOpts())
                            .endEnv()
                            .withNewResources()
                                .addToRequests("memory", new Quantity(app.getSpec().getResources().getMemory()))
                                .addToRequests("cpu", new Quantity(app.getSpec().getResources().getCpu()))
                                .addToLimits("memory", new Quantity(app.getSpec().getResources().getMemory()))
                                .addToLimits("cpu", new Quantity(app.getSpec().getResources().getCpu()))
                            .endResources()
                        .endContainer()
                    .endSpec()
                .endTemplate()
            .endSpec()
            .build();
        
        client.apps().deployments()
            .inNamespace(namespace)
            .createOrReplace(deployment);
    }
}
```

---

### Operators populaires pour Java developers

**1. Strimzi (Apache Kafka) :**

```yaml
apiVersion: kafka.strimzi.io/v1beta2
kind: Kafka
metadata:
  name: my-cluster
spec:
  kafka:
    version: 3.6.0
    replicas: 3
    storage:
      type: persistent-claim
      size: 100Gi
  zookeeper:
    replicas: 3
    storage:
      type: persistent-claim
      size: 10Gi
```

**2. Zalando Postgres Operator :**

```yaml
apiVersion: acid.zalan.do/v1
kind: postgresql
metadata:
  name: my-postgres
spec:
  teamId: "myteam"
  volume:
    size: 50Gi
  numberOfInstances: 3
  users:
    myapp:
      - superuser
      - createdb
  databases:
    mydb: myapp
  postgresql:
    version: "15"
```

**3. MongoDB Community Operator :**

```yaml
apiVersion: mongodbcommunity.mongodb.com/v1
kind: MongoDBCommunity
metadata:
  name: my-mongodb
spec:
  members: 3
  type: ReplicaSet
  version: "6.0.5"
  security:
    authentication:
      modes: ["SCRAM"]
  users:
    - name: admin
      db: admin
      passwordSecretRef:
        name: mongodb-admin-password
      roles:
        - name: root
          db: admin
  statefulSet:
    spec:
      volumeClaimTemplates:
        - metadata:
            name: data-volume
          spec:
            accessModes: ["ReadWriteOnce"]
            resources:
              requests:
                storage: 50Gi
```

---

## 12.2. Init Containers & Sidecar Patterns

### Init Containers

Les **Init Containers** s'exécutent **avant** les containers principaux et doivent se terminer avec succès.

**Cas d'usage :**
- Attendre qu'une dépendance soit disponible
- Télécharger configuration/données
- Exécuter migrations DB
- Setup/préparation

**Exemple : Attendre PostgreSQL**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  template:
    spec:
      initContainers:
      # Init 1 : Attendre que PostgreSQL soit prêt
      - name: wait-for-postgres
        image: busybox:1.35
        command:
        - sh
        - -c
        - |
          echo "Waiting for PostgreSQL..."
          until nc -z postgres-service 5432; do
            echo "PostgreSQL not ready, waiting..."
            sleep 2
          done
          echo "PostgreSQL is ready!"
      
      # Init 2 : Exécuter migrations Flyway
      - name: flyway-migration
        image: flyway/flyway:9.16
        command:
        - flyway
        - migrate
        env:
        - name: FLYWAY_URL
          value: "jdbc:postgresql://postgres-service:5432/mydb"
        - name: FLYWAY_USER
          valueFrom:
            secretKeyRef:
              name: db-credentials
              key: username
        - name: FLYWAY_PASSWORD
          valueFrom:
            secretKeyRef:
              name: db-credentials
              key: password
        volumeMounts:
        - name: migrations
          mountPath: /flyway/sql
      
      containers:
      - name: app
        image: my-spring-app:v1.0.0
        # App démarre seulement APRÈS succès des init containers
      
      volumes:
      - name: migrations
        configMap:
          name: db-migrations
```

**Séquence d'exécution :**

```
1. Init Container 1 (wait-for-postgres) démarre
2. Vérifie connexion PostgreSQL
3. Si OK → termine avec exit 0
4. Init Container 2 (flyway-migration) démarre
5. Exécute migrations SQL
6. Si OK → termine avec exit 0
7. Container principal (app) démarre

Si un init container échoue → Pod en Init:Error, retry
```

---

### Sidecar Pattern

Un **Sidecar** est un container qui s'exécute **en parallèle** du container principal.

**Cas d'usage :**
- Logging (Fluentd, Filebeat)
- Monitoring (Prometheus exporter)
- Service mesh (Envoy proxy)
- Synchronisation de fichiers

**Exemple 1 : Sidecar de logging**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app
spec:
  template:
    spec:
      containers:
      # Container principal
      - name: app
        image: my-spring-app:v1.0.0
        volumeMounts:
        - name: logs
          mountPath: /app/logs
      
      # Sidecar : collecteur de logs
      - name: log-collector
        image: fluent/fluentd:v1.16
        volumeMounts:
        - name: logs
          mountPath: /app/logs
          readOnly: true
        - name: fluentd-config
          mountPath: /fluentd/etc
        env:
        - name: FLUENT_ELASTICSEARCH_HOST
          value: "elasticsearch-service"
        - name: FLUENT_ELASTICSEARCH_PORT
          value: "9200"
      
      volumes:
      - name: logs
        emptyDir: {}
      - name: fluentd-config
        configMap:
          name: fluentd-config
```

**Configuration Fluentd :**

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: fluentd-config
data:
  fluent.conf: |
    <source>
      @type tail
      path /app/logs/*.log
      pos_file /tmp/app.log.pos
      tag app.logs
      <parse>
        @type json
        time_key time
        time_format %Y-%m-%dT%H:%M:%S.%NZ
      </parse>
    </source>
    
    <match app.logs>
      @type elasticsearch
      host "#{ENV['FLUENT_ELASTICSEARCH_HOST']}"
      port "#{ENV['FLUENT_ELASTICSEARCH_PORT']}"
      index_name app-logs
      type_name _doc
    </match>
```

---

**Exemple 2 : Sidecar Prometheus exporter**

Pour exposer des métriques JMX vers Prometheus :

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  template:
    metadata:
      annotations:
        prometheus.io/scrape: "true"
        prometheus.io/port: "9404"
        prometheus.io/path: "/metrics"
    spec:
      containers:
      # Container principal
      - name: app
        image: my-spring-app:v1.0.0
        ports:
        - containerPort: 8080
          name: http
        env:
        - name: JAVA_OPTS
          value: >-
            -Dcom.sun.management.jmxremote
            -Dcom.sun.management.jmxremote.port=9010
            -Dcom.sun.management.jmxremote.rmi.port=9010
            -Dcom.sun.management.jmxremote.authenticate=false
            -Dcom.sun.management.jmxremote.ssl=false
      
      # Sidecar : JMX Exporter
      - name: jmx-exporter
        image: bitnami/jmx-exporter:0.19.0
        ports:
        - containerPort: 9404
          name: metrics
        env:
        - name: SERVICE_PORT
          value: "9404"
        volumeMounts:
        - name: jmx-config
          mountPath: /opt/bitnami/jmx-exporter/config
      
      volumes:
      - name: jmx-config
        configMap:
          name: jmx-exporter-config
```

---

**Exemple 3 : Istio Service Mesh (sidecar automatique)**

Avec Istio, un sidecar proxy (Envoy) est injecté automatiquement :

```yaml
apiVersion: v1
kind: Namespace
metadata:
  name: production
  labels:
    istio-injection: enabled  # Active injection automatique
---
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app
  namespace: production
spec:
  template:
    spec:
      containers:
      - name: app
        image: my-spring-app:v1.0.0

# Istio injecte automatiquement un sidecar :
# - name: istio-proxy
#   image: docker.io/istio/proxyv2:1.20.0
# Ce sidecar gère :
# - Routing intelligent
# - Load balancing
# - Circuit breaking
# - Mutual TLS
# - Metrics, tracing
```

---

## 12.3. Taints & Tolerations

### Concept

**Taints** : Marques sur les **Nodes** qui repoussent les Pods.

**Tolerations** : Marques sur les **Pods** qui tolèrent certaines taints.

**Use cases :**
- Dédier des nodes à des workloads spécifiques
- Séparer prod/staging
- Nodes avec GPU
- Empêcher scheduling sur master nodes

---

### Syntaxe des Taints

```bash
# Format
kubectl taint nodes <node-name> <key>=<value>:<effect>

# Effects:
# - NoSchedule : Aucun nouveau pod si pas de toleration
# - PreferNoSchedule : Essayer d'éviter, mais possible
# - NoExecute : Éviction des pods existants sans toleration
```

**Exemple :**

```bash
# Marquer un node pour workloads "production"
kubectl taint nodes node1 environment=production:NoSchedule

# Marquer un node avec GPU
kubectl taint nodes gpu-node1 gpu=nvidia:NoSchedule

# Retirer une taint
kubectl taint nodes node1 environment=production:NoSchedule-
```

---

### Tolerations dans les Pods

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-prod-app
spec:
  template:
    spec:
      tolerations:
      # Tolère la taint "environment=production"
      - key: "environment"
        operator: "Equal"
        value: "production"
        effect: "NoSchedule"
      
      containers:
      - name: app
        image: my-app:v1.0.0

# Ce pod PEUT être schedulé sur node1
```

**Sans toleration :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-dev-app
spec:
  template:
    spec:
      # Pas de toleration
      containers:
      - name: app
        image: my-app:dev

# Ce pod NE PEUT PAS être schedulé sur node1
```

---

### Exemple : Séparer Production et Staging

**Setup nodes :**

```bash
# Nodes production
kubectl taint nodes prod-node1 tier=production:NoSchedule
kubectl taint nodes prod-node2 tier=production:NoSchedule

# Nodes staging (pas de taint)
```

**Deployment Production :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app-prod
  namespace: production
spec:
  template:
    spec:
      tolerations:
      - key: "tier"
        operator: "Equal"
        value: "production"
        effect: "NoSchedule"
      
      nodeSelector:
        tier: production  # Force sur nodes production
      
      containers:
      - name: app
        image: my-app:v1.0.0
```

**Deployment Staging :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app-staging
  namespace: staging
spec:
  template:
    spec:
      # Pas de toleration = va sur nodes sans taint (staging)
      nodeSelector:
        tier: staging
      
      containers:
      - name: app
        image: my-app:staging
```

---

### NoExecute Effect (éviction)

```bash
# Taint avec NoExecute
kubectl taint nodes node1 maintenance=true:NoExecute

# Tous les pods SANS toleration sont évictés immédiatement
```

**Toleration avec période de grâce :**

```yaml
tolerations:
- key: "maintenance"
  operator: "Equal"
  value: "true"
  effect: "NoExecute"
  tolerationSeconds: 300  # Tolérer pendant 5 minutes, puis éviction
```

---

## 12.4. Node Affinity & Pod Affinity

### Node Affinity

**Node Affinity** : Placer des Pods sur des Nodes spécifiques selon labels.

**Types :**
- `requiredDuringSchedulingIgnoredDuringExecution` : **Obligatoire**
- `preferredDuringSchedulingIgnoredDuringExecution` : **Préféré** (soft)

**Exemple : Forcer sur nodes avec SSD**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: database-app
spec:
  template:
    spec:
      affinity:
        nodeAffinity:
          # Requis : Node DOIT avoir label disk=ssd
          requiredDuringSchedulingIgnoredDuringExecution:
            nodeSelectorTerms:
            - matchExpressions:
              - key: disk
                operator: In
                values:
                - ssd
          
          # Préféré : Essayer d'avoir zone=europe-west1
          preferredDuringSchedulingIgnoredDuringExecution:
          - weight: 100
            preference:
              matchExpressions:
              - key: zone
                operator: In
                values:
                - europe-west1
      
      containers:
      - name: db
        image: postgres:15
```

**Labels sur nodes :**

```bash
kubectl label nodes node1 disk=ssd
kubectl label nodes node1 zone=europe-west1
```

---

### Pod Affinity

**Pod Affinity** : Placer des Pods **proche** d'autres Pods.

**Use case :** Application et cache Redis sur même node (latence minimale).

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app
spec:
  template:
    metadata:
      labels:
        app: my-app
    spec:
      affinity:
        podAffinity:
          # Placer ce pod sur même node que Redis
          requiredDuringSchedulingIgnoredDuringExecution:
          - labelSelector:
              matchExpressions:
              - key: app
                operator: In
                values:
                - redis
            topologyKey: kubernetes.io/hostname
      
      containers:
      - name: app
        image: my-app:v1.0.0
```

---

### Pod Anti-Affinity

**Pod Anti-Affinity** : Placer des Pods **éloignés** d'autres Pods.

**Use case :** Répartir les replicas sur différents nodes (haute disponibilité).

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app
spec:
  replicas: 3
  template:
    metadata:
      labels:
        app: my-app
    spec:
      affinity:
        podAntiAffinity:
          # Préférer des nodes différents pour chaque replica
          preferredDuringSchedulingIgnoredDuringExecution:
          - weight: 100
            podAffinityTerm:
              labelSelector:
                matchExpressions:
                - key: app
                  operator: In
                  values:
                  - my-app
              topologyKey: kubernetes.io/hostname
      
      containers:
      - name: app
        image: my-app:v1.0.0

# Résultat : Les 3 replicas seront sur 3 nodes différents
```

**Anti-affinity strict (required) :**

```yaml
podAntiAffinity:
  requiredDuringSchedulingIgnoredDuringExecution:
  - labelSelector:
      matchExpressions:
      - key: app
        operator: In
        values:
        - my-app
    topologyKey: kubernetes.io/hostname

# Si pas assez de nodes : Pods restent Pending
```

---

### Exemple complet : Application critique

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: critical-app
spec:
  replicas: 3
  template:
    spec:
      affinity:
        # Node Affinity : Seulement sur nodes haute performance
        nodeAffinity:
          requiredDuringSchedulingIgnoredDuringExecution:
            nodeSelectorTerms:
            - matchExpressions:
              - key: performance
                operator: In
                values:
                - high
        
        # Pod Anti-Affinity : Répartir sur différents nodes
        podAntiAffinity:
          requiredDuringSchedulingIgnoredDuringExecution:
          - labelSelector:
              matchLabels:
                app: critical-app
            topologyKey: kubernetes.io/hostname
        
        # Pod Affinity : Proche de Redis
        podAffinity:
          preferredDuringSchedulingIgnoredDuringExecution:
          - weight: 50
            podAffinityTerm:
              labelSelector:
                matchLabels:
                  app: redis
              topologyKey: kubernetes.io/hostname
      
      containers:
      - name: app
        image: critical-app:v1.0.0
```

---

## 12.5. Pod Disruption Budgets (PDB)

### Concept

Un **PDB** garantit qu'un nombre minimum de Pods reste disponible pendant les **disruptions volontaires** :
- Drain d'un node
- Cluster upgrade
- Node maintenance

**Ne protège PAS contre :**
- Node crash
- Pod crash
- OOMKilled

---

### Syntaxe

```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: my-app-pdb
spec:
  # Option 1 : Minimum disponible
  minAvailable: 2
  
  # OU Option 2 : Maximum indisponible
  # maxUnavailable: 1
  
  selector:
    matchLabels:
      app: my-app
```

**minAvailable: 2** → Au moins 2 pods doivent rester running

**maxUnavailable: 1** → Maximum 1 pod peut être down en même temps

---

### Exemple : Application avec 3 replicas

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app
spec:
  replicas: 3
  selector:
    matchLabels:
      app: my-app
  template:
    metadata:
      labels:
        app: my-app
    spec:
      containers:
      - name: app
        image: my-app:v1.0.0
---
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: my-app-pdb
spec:
  minAvailable: 2  # Au moins 2 sur 3 doivent rester up
  selector:
    matchLabels:
      app: my-app
```

**Scénario : Drain d'un node**

```bash
# Sans PDB:
kubectl drain node1 --ignore-daemonsets
# → Tous les pods sur node1 évictés immédiatement
# → Possibilité de downtime si tous sur node1

# Avec PDB:
kubectl drain node1 --ignore-daemonsets
# → Kubernetes évicte 1 pod à la fois
# → Attend que nouveaux pods soient Ready
# → Garantit toujours 2 pods disponibles
# → Aucun downtime !
```

---

### Percentage-based PDB

```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: my-app-pdb
spec:
  maxUnavailable: 25%  # Max 25% des pods peuvent être down
  selector:
    matchLabels:
      app: my-app

# Avec 4 replicas : max 1 pod down
# Avec 10 replicas : max 2 pods down (25% de 10 = 2.5, arrondi à 2)
```

---

### Best Practices

**1. Toujours utiliser PDB pour apps critiques**

```yaml
# ✅ Bon
minAvailable: 2

# ❌ Mauvais : Pas de PDB
# → Drain peut tuer tous les pods !
```

---

**2. PDB cohérent avec replicas**

```yaml
# ✅ Bon
replicas: 3
minAvailable: 2  # 66% uptime garanti

# ❌ Mauvais
replicas: 2
minAvailable: 2  # Impossible de drain un node !
```

---

**3. Combiner avec Pod Anti-Affinity**

```yaml
# Garantir haute disponibilité
apiVersion: apps/v1
kind: Deployment
spec:
  replicas: 3
  template:
    spec:
      affinity:
        podAntiAffinity:
          preferredDuringSchedulingIgnoredDuringExecution:
          - weight: 100
            podAffinityTerm:
              labelSelector:
                matchLabels:
                  app: my-app
              topologyKey: kubernetes.io/hostname
---
apiVersion: policy/v1
kind: PodDisruptionBudget
spec:
  minAvailable: 2
  selector:
    matchLabels:
      app: my-app

# Résultat :
# - Pods répartis sur différents nodes (anti-affinity)
# - Toujours 2 pods minimum (PDB)
# - Haute disponibilité garantie !
```

---

## 12.6. Cluster Maintenance & Upgrades

### Drain d'un Node

**Drain** : Éviction propre de tous les Pods d'un node.

```bash
# Drain un node (éviction + cordon)
kubectl drain node1 --ignore-daemonsets --delete-emptydir-data

# Options:
# --ignore-daemonsets : Ignorer DaemonSets (ne peuvent pas être évictés)
# --delete-emptydir-data : Supprimer data dans emptyDir volumes
# --force : Forcer éviction (dangereux)
# --grace-period=300 : Grace period pour shutdown (secondes)
```

**Séquence du drain :**

```
1. Node marqué comme "SchedulingDisabled" (cordon)
2. Kubernetes évicte pods un par un (respecte PDB)
3. Pods terminés proprement (graceful shutdown)
4. Nouveaux pods schedulés sur autres nodes
5. Node vide, prêt pour maintenance
```

---

**Cordon** : Empêcher nouveaux pods, sans évincer existants.

```bash
# Cordon (pas d'éviction)
kubectl cordon node1

# Uncordon (réactiver scheduling)
kubectl uncordon node1
```

---

### Upgrade de Cluster Kubernetes

**Stratégie : Rolling upgrade**

```bash
# 1. Upgrade control plane nodes (un par un)
kubectl drain master1 --ignore-daemonsets
# Upgrade Kubernetes components sur master1
kubeadm upgrade apply v1.29.0
kubectl uncordon master1

# Répéter pour worker2, worker3...
```

**Vérification de la version :**

```bash
# Version du cluster
kubectl version --short

# Version de chaque node
kubectl get nodes -o wide

# Output:
# NAME      STATUS   ROLES    VERSION
# master1   Ready    master   v1.29.0
# worker1   Ready    <none>   v1.29.0
# worker2   Ready    <none>   v1.28.3  # Pas encore upgradé
```

---

### Best Practices pour Maintenance

**1. Planifier en heures creuses**

```bash
# Vérifier charge actuelle
kubectl top nodes
kubectl top pods --all-namespaces

# Attendre période de faible trafic
```

---

**2. Backup avant upgrade**

```bash
# Backup etcd
ETCDCTL_API=3 etcdctl snapshot save /backup/etcd-snapshot.db \
  --endpoints=https://127.0.0.1:2379 \
  --cacert=/etc/kubernetes/pki/etcd/ca.crt \
  --cert=/etc/kubernetes/pki/etcd/server.crt \
  --key=/etc/kubernetes/pki/etcd/server.key

# Backup manifests
tar -czf /backup/k8s-manifests.tar.gz /etc/kubernetes/manifests
```

---

**3. Tester sur staging d'abord**

```bash
# Upgrade staging cluster
kubectl config use-context staging
# ... upgrade process ...

# Vérifier apps fonctionnent
kubectl get pods --all-namespaces
curl https://staging.myapp.com/health

# Si OK → Upgrade production
```

---

**4. Upgrade progressif (1 node à la fois)**

```yaml
# Script automatisé
#!/bin/bash
NODES=$(kubectl get nodes -o jsonpath='{.items[*].metadata.name}')

for NODE in $NODES; do
  echo "Upgrading $NODE..."
  
  kubectl drain $NODE --ignore-daemonsets --delete-emptydir-data
  
  # SSH vers le node et upgrade
  ssh $NODE "sudo kubeadm upgrade node && sudo systemctl restart kubelet"
  
  kubectl uncordon $NODE
  
  # Attendre que le node soit Ready
  kubectl wait --for=condition=Ready node/$NODE --timeout=300s
  
  echo "$NODE upgraded successfully"
  
  # Pause entre chaque node
  sleep 60
done
```

---

**5. Monitoring pendant l'upgrade**

```bash
# Terminal 1 : Watch nodes
watch kubectl get nodes

# Terminal 2 : Watch pods
watch kubectl get pods --all-namespaces

# Terminal 3 : Watch events
kubectl get events --all-namespaces --watch

# Terminal 4 : Monitoring applicatif
# Grafana dashboards : Error rate, latency, throughput
```

---

### Rollback en cas de problème

```bash
# Si upgrade échoue sur un node

# 1. Uncordon le node
kubectl uncordon node1

# 2. Vérifier les logs
journalctl -u kubelet -f

# 3. Downgrade si nécessaire
kubeadm upgrade node --certificate-renewal=false --etcd-upgrade=false

# 4. Restart kubelet
systemctl restart kubelet

# 5. Restore etcd si critique
ETCDCTL_API=3 etcdctl snapshot restore /backup/etcd-snapshot.db
```

---

## Récapitulatif du Chapitre 12

✅ **Points clés :**

**Custom Resources & Operators :**
- CRD pour étendre l'API Kubernetes
- Operators pour automatiser gestion d'applications complexes
- Pattern : Watch CRD → Reconcile → Créer/Update ressources

**Init Containers & Sidecars :**
- Init Containers : Exécution **avant** le container principal (setup)
- Sidecars : Exécution **parallèle** (logging, monitoring, proxy)
- Patterns courants : Wait-for, migrations DB, log collectors

**Taints & Tolerations :**
- Taints sur Nodes pour repousser Pods
- Tolerations sur Pods pour tolérer certaines taints
- Use case : Dédier nodes à workloads spécifiques

**Node & Pod Affinity :**
- Node Affinity : Placer Pods sur Nodes spécifiques
- Pod Affinity : Placer Pods proches d'autres Pods
- Pod Anti-Affinity : Répartir Pods sur différents Nodes (HA)

**Pod Disruption Budgets :**
- Garantit disponibilité minimum pendant maintenance
- minAvailable ou maxUnavailable
- Critique pour apps production

**Cluster Maintenance :**
- Drain : Éviction propre des Pods
- Cordon : Bloquer scheduling sans éviction
- Upgrade progressif : 1 node à la fois
- Backup etcd avant toute opération

---

## Questions d'entretien fréquentes

### Q1 : Qu'est-ce qu'un Operator et pourquoi l'utiliser ?

**Réponse :**

Un **Operator** est un pattern qui combine Custom Resources (CRD) et un controller pour automatiser la gestion d'applications complexes dans Kubernetes.

**Avantages :**
- Encapsule l'expertise opérationnelle dans du code
- Automatise les tâches répétitives (backup, upgrade, scaling)
- Self-healing (détecte et corrige les dérives)
- Abstractions simples pour les développeurs

**Exemple : Postgres Operator**

Au lieu de gérer manuellement :
- StatefulSet
- PersistentVolumeClaims
- Services
- Backup CronJobs
- Monitoring
- Replication

On définit simplement :

```yaml
apiVersion: acid.zalan.do/v1
kind: postgresql
metadata:
  name: my-db
spec:
  numberOfInstances: 3
  volume:
    size: 50Gi
```

L'Operator gère tout automatiquement !

---

### Q2 : Quelle est la différence entre Init Containers et Sidecars ?

**Réponse :**

| Aspect | Init Container | Sidecar |
|--------|---------------|---------|
| **Exécution** | Avant le container principal | Parallèle au container principal |
| **Lifecycle** | Se termine (exit 0) | Tourne en continu |
| **Ordre** | Séquentiel (1, 2, 3...) | Tous démarrent ensemble |
| **Use case** | Setup, wait-for, migrations | Logging, monitoring, proxy |
| **Échec** | Pod ne démarre pas | Pod peut continuer |

**Exemple Init Container :**

```yaml
initContainers:
- name: wait-for-db
  image: busybox
  command: ['sh', '-c', 'until nc -z db 5432; do sleep 1; done']
```

**Exemple Sidecar :**

```yaml
containers:
- name: app
  image: my-app
- name: log-collector
  image: fluentd
```

---

### Q3 : Expliquez Taints, Tolerations et leur use case

**Réponse :**

**Taints** : Marquent des Nodes pour repousser des Pods

**Tolerations** : Permettent à des Pods de "tolérer" certaines taints

**Use case principal : Séparer workloads**

**Exemple : Nodes dédiés GPU**

```bash
# Marquer nodes avec GPU
kubectl taint nodes gpu-node1 gpu=nvidia:NoSchedule
kubectl taint nodes gpu-node2 gpu=nvidia:NoSchedule
```

```yaml
# Deployment ML/AI avec toleration
apiVersion: apps/v1
kind: Deployment
metadata:
  name: ml-training
spec:
  template:
    spec:
      tolerations:
      - key: "gpu"
        operator: "Equal"
        value: "nvidia"
        effect: "NoSchedule"
      
      nodeSelector:
        gpu: nvidia
      
      containers:
      - name: training
        image: ml-training:v1.0.0
        resources:
          limits:
            nvidia.com/gpu: 1
```

**Résultat :**
- Seuls les Pods avec toleration peuvent être schedulés sur gpu-nodes
- Apps normales n'utilisent pas les nodes GPU (coûteux)
- Utilisation optimisée des ressources

---

### Q4 : Comment garantissez-vous la haute disponibilité d'une application dans Kubernetes ?

**Réponse :**

Je combine plusieurs techniques :

**1. Replicas multiples**

```yaml
spec:
  replicas: 3  # Au minimum 3 pour tolérer 1 failure
```

---

**2. Pod Anti-Affinity (répartir sur différents nodes)**

```yaml
affinity:
  podAntiAffinity:
    preferredDuringSchedulingIgnoredDuringExecution:
    - weight: 100
      podAffinityTerm:
        labelSelector:
          matchLabels:
            app: my-app
        topologyKey: kubernetes.io/hostname
```

---

**3. Pod Disruption Budget**

```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
spec:
  minAvailable: 2  # Toujours 2 pods minimum
  selector:
    matchLabels:
      app: my-app
```

---

**4. Readiness & Liveness Probes**

```yaml
readinessProbe:
  httpGet:
    path: /health/readiness
    port: 8080
  periodSeconds: 5

livenessProbe:
  httpGet:
    path: /health/liveness
    port: 8080
  periodSeconds: 10
```

---

**5. Resource Requests & Limits**

```yaml
resources:
  requests:
    memory: "512Mi"
    cpu: "500m"
  limits:
    memory: "1Gi"
    cpu: "1000m"
```

---

**6. Multi-AZ deployment (si cloud)**

```yaml
affinity:
  podAntiAffinity:
    preferredDuringSchedulingIgnoredDuringExecution:
    - weight: 100
      podAffinityTerm:
        labelSelector:
          matchLabels:
            app: my-app
        topologyKey: topology.kubernetes.io/zone
```

---

**7. HPA (auto-scaling)**

```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
spec:
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

---

**Résultat :**
- Pods répartis sur plusieurs nodes et AZ
- Redémarrage automatique si crash
- Auto-scaling selon charge
- Pas d'interruption pendant maintenance
- **SLA > 99.9% garanti**

---

### Q5 : Comment procédez-vous pour drainer un node en production sans impacter les utilisateurs ?

**Réponse :**

**Préparation (avant le drain) :**

**1. Vérifier que les apps ont des PDB**

```bash
kubectl get pdb --all-namespaces

# Si manquant, créer :
kubectl apply -f pdb.yaml
```

---

**2. Vérifier replicas suffisants**

```bash
kubectl get deployments --all-namespaces

# Si replicas < 3, scaler :
kubectl scale deployment my-app --replicas=3
```

---

**3. Vérifier anti-affinity**

```bash
# Vérifier que pods sont répartis
kubectl get pods -o wide | grep my-app

# Si tous sur même node → problème !
```

---

**4. Monitoring actif**

```bash
# Ouvrir Grafana dashboards :
# - Error rate
# - Latency p95/p99
# - Active requests
```

---

**Exécution du drain :**

```bash
# Drain avec grace period
kubectl drain node1 \
  --ignore-daemonsets \
  --delete-emptydir-data \
  --grace-period=300 \
  --timeout=600s

# Flags importants :
# --grace-period=300 : 5 min pour graceful shutdown
# --timeout=600s : Timeout total 10 min
# --ignore-daemonsets : Ne pas évincer DaemonSets
```

---

**Monitoring pendant le drain :**

```bash
# Terminal 1 : Watch pods
watch kubectl get pods --all-namespaces -o wide

# Terminal 2 : Watch node
kubectl get node node1 -w

# Terminal 3 : Events
kubectl get events --all-namespaces --watch | grep node1

# Terminal 4 : Métriques applicatives
# Grafana : Vérifier que error rate reste stable
```

---

**Après le drain :**

```bash
# 1. Vérifier que tous les pods sont reschedulés
kubectl get pods --all-namespaces -o wide | grep -v Running

# 2. Vérifier health des apps
kubectl get pods -l app=my-app

# 3. Tester endpoint
curl https://my-app.com/health

# 4. Effectuer maintenance sur node1
# (upgrade, reboot, etc.)

# 5. Uncordon après maintenance
kubectl uncordon node1

# 6. Vérifier que le node est Ready
kubectl get node node1
```

---

**En cas de problème :**

```bash
# Abort du drain (Ctrl+C)

# Uncordon immédiatement
kubectl uncordon node1

# Investiguer
kubectl describe node node1
kubectl get events --all-namespaces | grep node1
```

---

**Checklist finale :**

✅ PDB configuré (minAvailable)
✅ Replicas ≥ 3
✅ Anti-affinity configurée
✅ Monitoring actif
✅ Grace period suffisant (300s)
✅ Drain en heures creuses
✅ Rollback plan prêt

**Mon expérience :**

J'ai drainé ~50 nodes en production sans aucun incident grâce à :
- PDB strict (minAvailable: 2)
- Anti-affinity sur tous les Deployments
- Monitoring temps réel (Prometheus alertes)
- Drain progressif (1 node toutes les 30 minutes)

---

## 🎯 Best Practices Kubernetes (Résumé)

### Sécurité

✅ **Toujours** user non-root dans containers
✅ **Distroless** images en production
✅ **RBAC** strict (principle of least privilege)
✅ **Network Policies** pour isoler namespaces
✅ **Pod Security Standards** (Restricted)
✅ **Secrets** chiffrés at rest (encryption)
✅ **Image scanning** dans CI/CD (Trivy, Snyk)

---

### Haute Disponibilité

✅ **Replicas** ≥ 3 pour apps critiques
✅ **Pod Anti-Affinity** pour répartir
✅ **PDB** (Pod Disruption Budget) obligatoire
✅ **Multi-AZ** deployment si cloud
✅ **Health probes** (liveness + readiness)
✅ **Resource limits** pour éviter OOM

---

### Performance

✅ **Resource requests** = réaliste
✅ **Resource limits** = 2x requests
✅ **HPA** pour auto-scaling
✅ **JVM tuning** (UseContainerSupport, MaxRAMPercentage)
✅ **Layered JARs** pour build rapides
✅ **Node affinity** sur nodes haute performance

---

### Observabilité

✅ **Prometheus** métriques (business + infra)
✅ **Grafana** dashboards
✅ **Logging centralisé** (ELK, Loki)
✅ **Tracing** (Jaeger, Zipkin)
✅ **Alerting** (AlertManager, PagerDuty)
✅ **Custom metrics** pour business KPIs

---

### Déploiement

✅ **GitOps** (ArgoCD, Flux)
✅ **Blue/Green** ou **Canary** pour prod
✅ **Rolling update** avec maxUnavailable: 0
✅ **Graceful shutdown** configuré
✅ **Smoke tests** automatisés
✅ **Rollback plan** documenté

---

### Maintenance

✅ **Backup etcd** régulier
✅ **Drain progressif** (1 node à la fois)
✅ **Upgrade staging** avant prod
✅ **Monitoring** pendant opérations
✅ **Documentation** des procédures
✅ **Post-mortem** après incidents

---

## 📝 Commandes Kubernetes Essentielles

### Debugging

```bash
# Pods
kubectl get pods -A -o wide
kubectl describe pod <pod>
kubectl logs <pod> -f
kubectl logs <pod> --previous
kubectl exec -it <pod> -- sh

# Resources
kubectl top nodes
kubectl top pods -A
kubectl get events -A --sort-by='.lastTimestamp'

# Networking
kubectl get svc,endpoints -A
kubectl port-forward <pod> 8080:8080

# Storage
kubectl get pv,pvc -A
```

---

### Maintenance

```bash
# Nodes
kubectl cordon <node>
kubectl uncordon <node>
kubectl drain <node> --ignore-daemonsets

# Rollout
kubectl rollout status deployment/<name>
kubectl rollout history deployment/<name>
kubectl rollout undo deployment/<name>

# Scaling
kubectl scale deployment/<name> --replicas=5
kubectl autoscale deployment/<name> --min=3 --max=10 --cpu-percent=70
```

---

### Configuration

```bash
# ConfigMaps & Secrets
kubectl create configmap <name> --from-file=app.yaml
kubectl create secret generic <name> --from-literal=password=xxx
kubectl get cm,secret -A

# Apply
kubectl apply -f manifest.yaml
kubectl apply -k ./kustomize/
kubectl diff -f manifest.yaml

# Contexts
kubectl config get-contexts
kubectl config use-context <context>
kubectl config set-context --current --namespace=<ns>
```

---

## 🎓 Pour aller plus loin

**Certifications recommandées :**
- ✅ CKAD (Certified Kubernetes Application Developer) - **Déjà obtenu !**
- 🎯 CKA (Certified Kubernetes Administrator) - Pour ops/SRE
- 🎯 CKS (Certified Kubernetes Security) - Pour sécurité

**Outils à maîtriser :**
- **GitOps** : ArgoCD, Flux
- **Service Mesh** : Istio, Linkerd
- **Monitoring** : Prometheus, Grafana, Datadog
- **Logging** : ELK Stack, Loki
- **Security** : Falco, OPA Gatekeeper, Trivy

**Concepts avancés :**
- Kubernetes Federation (multi-cluster)
- Custom Schedulers
- Admission Webhooks
- Service Mesh (Istio)
- eBPF & Cilium

---

### Upgrade kubelet et kubectl sur worker1

```bash
# Sur le worker node
sudo apt-mark unhold kubelet kubectl
sudo apt-get update
sudo apt-get install -y kubelet=1.29.0-00 kubectl=1.29.0-00
sudo apt-mark hold kubelet kubectl

# Restart services
sudo systemctl daemon-reload
sudo systemctl restart kubelet

# Vérifier version
kubelet --version
```

---

**Vérifier que le node est revenu :**

```bash
# Depuis master
kubectl get node worker1

# Output attendu:
# NAME      STATUS   ROLES    VERSION
# worker1   Ready    <none>   v1.29.0
```

---

### Version Skew Policy

Kubernetes supporte un décalage de version limité entre composants :

| Composant | Version supportée |
|-----------|------------------|
| kube-apiserver | N (ex: 1.29) |
| kubelet | N, N-1, N-2 (1.29, 1.28, 1.27) |
| kubectl | N+1, N, N-1 (1.30, 1.29, 1.28) |
| kube-controller-manager | N, N-1 (1.29, 1.28) |
| kube-scheduler | N, N-1 (1.29, 1.28) |

**Stratégie d'upgrade :**
1. Upgrade control plane en premier
2. Puis upgrade worker nodes progressivement
3. Respecter la version skew policy

---

### Automatisation avec Terraform

```hcl
# terraform/upgrade-cluster.tf
resource "null_resource" "upgrade_node" {
  for_each = toset(var.worker_nodes)
  
  provisioner "local-exec" {
    command = <<-EOT
      kubectl drain ${each.value} --ignore-daemonsets --delete-emptydir-data
      
      ssh ${each.value} '
        sudo kubeadm upgrade node
        sudo systemctl daemon-reload
        sudo systemctl restart kubelet
      '
      
      kubectl uncordon ${each.value}
      kubectl wait --for=condition=Ready node/${each.value} --timeout=300s
    EOT
  }
  
  triggers = {
    node_name = each.value
    version = var.target_version
  }
}
```

---

## Best Practices Production

### 1. Namespaces Organisation

```yaml
# Séparer par environnement et équipe
apiVersion: v1
kind: Namespace
metadata:
  name: production
  labels:
    environment: production
    team: backend
---
apiVersion: v1
kind: Namespace
metadata:
  name: staging
  labels:
    environment: staging
    team: backend
---
apiVersion: v1
kind: Namespace
metadata:
  name: dev
  labels:
    environment: development
    team: backend
```

**ResourceQuota par namespace :**

```yaml
apiVersion: v1
kind: ResourceQuota
metadata:
  name: production-quota
  namespace: production
spec:
  hard:
    requests.cpu: "100"
    requests.memory: "200Gi"
    limits.cpu: "200"
    limits.memory: "400Gi"
    persistentvolumeclaims: "50"
    pods: "200"
```

**LimitRange par namespace :**

```yaml
apiVersion: v1
kind: LimitRange
metadata:
  name: production-limits
  namespace: production
spec:
  limits:
  # Limits par container
  - type: Container
    default:
      cpu: "1000m"
      memory: "1Gi"
    defaultRequest:
      cpu: "100m"
      memory: "128Mi"
    max:
      cpu: "4000m"
      memory: "8Gi"
    min:
      cpu: "50m"
      memory: "64Mi"
  
  # Limits par Pod
  - type: Pod
    max:
      cpu: "8000m"
      memory: "16Gi"
```

---

### 2. Network Policies (Sécurité)

**Isoler les namespaces :**

```yaml
# Deny all traffic par défaut
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: deny-all
  namespace: production
spec:
  podSelector: {}
  policyTypes:
  - Ingress
  - Egress
```

**Autoriser seulement trafic nécessaire :**

```yaml
# Allow app → database
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-app-to-db
  namespace: production
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
          app: my-spring-app
    ports:
    - protocol: TCP
      port: 5432
```

**Allow vers Internet (DNS, HTTPS) :**

```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: allow-egress-internet
  namespace: production
spec:
  podSelector:
    matchLabels:
      app: my-spring-app
  policyTypes:
  - Egress
  egress:
  # Allow DNS
  - to:
    - namespaceSelector:
        matchLabels:
          name: kube-system
    ports:
    - protocol: UDP
      port: 53
  
  # Allow HTTPS
  - to:
    - podSelector: {}
    ports:
    - protocol: TCP
      port: 443
```

---

### 3. Pod Security Standards

**Enforce Restricted policy (le plus strict) :**

```yaml
apiVersion: v1
kind: Namespace
metadata:
  name: production
  labels:
    pod-security.kubernetes.io/enforce: restricted
    pod-security.kubernetes.io/audit: restricted
    pod-security.kubernetes.io/warn: restricted
```

**Ce que ça impose :**
- ❌ Pas de containers privileged
- ❌ Pas de hostPath volumes
- ❌ Pas de hostNetwork/hostPID
- ✅ runAsNonRoot obligatoire
- ✅ Capabilities limitées
- ✅ seccompProfile obligatoire

**Deployment conforme :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: secure-app
  namespace: production
spec:
  template:
    spec:
      securityContext:
        runAsNonRoot: true
        runAsUser: 1001
        fsGroup: 1001
        seccompProfile:
          type: RuntimeDefault
      
      containers:
      - name: app
        image: my-app:v1.0.0
        securityContext:
          allowPrivilegeEscalation: false
          capabilities:
            drop:
            - ALL
          readOnlyRootFilesystem: true
        
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

### 4. Secrets Management

**External Secrets Operator :**

```yaml
# Installation
kubectl apply -f https://raw.githubusercontent.com/external-secrets/external-secrets/main/deploy/crds/bundle.yaml

# SecretStore (AWS Secrets Manager)
apiVersion: external-secrets.io/v1beta1
kind: SecretStore
metadata:
  name: aws-secrets
  namespace: production
spec:
  provider:
    aws:
      service: SecretsManager
      region: eu-west-1
      auth:
        jwt:
          serviceAccountRef:
            name: external-secrets-sa
---
# ExternalSecret
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: app-secrets
  namespace: production
spec:
  refreshInterval: 1h
  secretStoreRef:
    name: aws-secrets
    kind: SecretStore
  target:
    name: my-app-secret
    creationPolicy: Owner
  data:
  - secretKey: database-password
    remoteRef:
      key: prod/myapp/db-password
  - secretKey: api-key
    remoteRef:
      key: prod/myapp/api-key
```

---

### 5. Monitoring & Alerting

**Prometheus Operator :**

```yaml
# ServiceMonitor pour application
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: my-spring-app
  namespace: production
  labels:
    app: my-spring-app
spec:
  selector:
    matchLabels:
      app: my-spring-app
  endpoints:
  - port: management
    path: /actuator/prometheus
    interval: 30s
    scrapeTimeout: 10s
```

**Alertes critiques :**

```yaml
apiVersion: monitoring.coreos.com/v1
kind: PrometheusRule
metadata:
  name: my-spring-app-alerts
  namespace: production
spec:
  groups:
  - name: my-spring-app
    interval: 30s
    rules:
    # High error rate
    - alert: HighErrorRate
      expr: |
        rate(http_server_requests_seconds_count{status=~"5..",app="my-spring-app"}[5m]) 
        / 
        rate(http_server_requests_seconds_count{app="my-spring-app"}[5m]) 
        > 0.05
      for: 5m
      labels:
        severity: critical
      annotations:
        summary: "High error rate on {{ $labels.pod }}"
        description: "Error rate is {{ $value | humanizePercentage }}"
    
    # High latency
    - alert: HighLatency
      expr: |
        histogram_quantile(0.95, 
          rate(http_server_requests_seconds_bucket{app="my-spring-app"}[5m])
        ) > 1
      for: 5m
      labels:
        severity: warning
      annotations:
        summary: "High latency on {{ $labels.pod }}"
        description: "P95 latency is {{ $value }}s"
    
    # Pod down
    - alert: PodDown
      expr: kube_deployment_status_replicas_available{deployment="my-spring-app"} < 2
      for: 2m
      labels:
        severity: critical
      annotations:
        summary: "Less than 2 pods available"
        description: "Only {{ $value }} pods available"
    
    # High memory usage
    - alert: HighMemoryUsage
      expr: |
        container_memory_usage_bytes{pod=~"my-spring-app.*"} 
        / 
        container_spec_memory_limit_bytes{pod=~"my-spring-app.*"} 
        > 0.9
      for: 5m
      labels:
        severity: warning
      annotations:
        summary: "High memory usage on {{ $labels.pod }}"
        description: "Memory usage is {{ $value | humanizePercentage }}"
```

---

### 6. Backup Strategy

**Velero (backup cluster complet) :**

```bash
# Installation Velero
velero install \
  --provider aws \
  --bucket my-k8s-backups \
  --secret-file ./credentials-velero \
  --backup-location-config region=eu-west-1 \
  --snapshot-location-config region=eu-west-1 \
  --use-volume-snapshots=true
```

**Backup automatique quotidien :**

```yaml
apiVersion: velero.io/v1
kind: Schedule
metadata:
  name: daily-backup
  namespace: velero
spec:
  schedule: "0 2 * * *"  # 2h du matin
  template:
    includedNamespaces:
    - production
    - staging
    excludedResources:
    - events
    - events.events.k8s.io
    ttl: 720h  # Garder 30 jours
    storageLocation: default
    volumeSnapshotLocations:
    - default
```

**Restore en cas de disaster :**

```bash
# Lister backups
velero backup get

# Restore
velero restore create --from-backup daily-backup-20241118
```

---

### 7. Cost Optimization

**Vertical Pod Autoscaler (VPA) :**

```yaml
apiVersion: autoscaling.k8s.io/v1
kind: VerticalPodAutoscaler
metadata:
  name: my-spring-app-vpa
spec:
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: my-spring-app
  updatePolicy:
    updateMode: "Auto"  # Auto-adjust resources
  resourcePolicy:
    containerPolicies:
    - containerName: app
      minAllowed:
        cpu: 100m
        memory: 128Mi
      maxAllowed:
        cpu: 2000m
        memory: 4Gi
```

**Cluster Autoscaler (cloud) :**

```yaml
# Pour AWS
apiVersion: apps/v1
kind: Deployment
metadata:
  name: cluster-autoscaler
  namespace: kube-system
spec:
  template:
    spec:
      containers:
      - name: cluster-autoscaler
        image: k8s.gcr.io/autoscaling/cluster-autoscaler:v1.29.0
        command:
        - ./cluster-autoscaler
        - --cloud-provider=aws
        - --namespace=kube-system
        - --node-group-auto-discovery=asg:tag=k8s.io/cluster-autoscaler/enabled,k8s.io/cluster-autoscaler/my-cluster
        - --balance-similar-node-groups
        - --skip-nodes-with-system-pods=false
```

---

## Récapitulatif Final

✅ **Vous maîtrisez maintenant :**

**Fondamentaux :**
- Architecture Kubernetes (Control Plane, Workers)
- Pods, Deployments, Services, Ingress
- ConfigMaps, Secrets
- Volumes, PV, PVC

**Spring Boot sur Kubernetes :**
- Containerization optimisée (Layered JARs)
- Configuration externalisée
- Graceful shutdown
- JVM tuning
- Health checks (Liveness/Readiness)

**CI/CD :**
- GitOps (ArgoCD)
- Deployment strategies (Blue/Green, Canary)
- Pipelines Jenkins/GitLab CI
- Image best practices

**Avancé :**
- Custom Resources & Operators
- Init Containers & Sidecars
- Taints, Tolerations, Affinity
- Pod Disruption Budgets
- Cluster maintenance

**Production :**
- Haute disponibilité
- Sécurité (RBAC, Network Policies, Pod Security)
- Monitoring & Alerting
- Backup & Disaster Recovery
- Cost optimization

---

# 🎯 Chapitre 12 : Questions/Réponses d'Entretien
## Advanced Topics & Best Practices

---

## 📋 Section 1 : Custom Resources & Operators

### Q1 : Qu'est-ce qu'un Operator Kubernetes et pourquoi l'utiliser ?

**Réponse :**

Un **Operator** est un pattern qui combine une **Custom Resource Definition (CRD)** et un **controller** pour automatiser la gestion d'applications complexes dans Kubernetes.

**Composants :**
- **CRD** : Définit un nouveau type de ressource (ex: PostgreSQL, Kafka)
- **Controller** : Surveille les CRD et crée/met à jour les ressources Kubernetes

**Architecture :**

```
Custom Resource → Operator (controller) → 
  Observe → Compare état actuel vs désiré → 
  Reconcile → Crée/Met à jour ressources natives
```

---

**Pourquoi utiliser des Operators ?**

**1. Automatisation de l'expertise opérationnelle**

Au lieu de gérer manuellement :

```yaml
# Sans Operator : ~10 fichiers YAML à gérer
- StatefulSet (réplication, ordonnancement)
- PersistentVolumeClaims (stockage)
- Services (headless + public)
- ConfigMaps (configuration PostgreSQL)
- Secrets (mots de passe)
- CronJobs (backups)
- ServiceMonitor (monitoring)
- NetworkPolicies (sécurité)
```

Avec un Operator :

```yaml
# Avec Operator : 1 simple manifest
apiVersion: acid.zalan.do/v1
kind: postgresql
metadata:
  name: my-postgres
spec:
  numberOfInstances: 3
  volume:
    size: 50Gi
  users:
    myapp: superuser
  databases:
    mydb: myapp
```

L'Operator gère **tout automatiquement** !

---

**2. Self-healing automatique**

```
Problème détecté → Operator reconcile → Correction automatique

Exemples :
- Pod crashé → Operator recrée
- Config drift → Operator corrige
- Backup manqué → Operator relance
- Version outdated → Operator upgrade
```

---

**3. Opérations complexes simplifiées**

```yaml
# Upgrade PostgreSQL 14 → 15
kubectl patch postgresql my-postgres --type=merge -p '{"spec":{"postgresql":{"version":"15"}}}'

# L'Operator gère :
# - Backup avant upgrade
# - Rolling upgrade des replicas
# - Validation de chaque étape
# - Rollback si échec
```

---

**Operators populaires pour Java developers :**

| Operator | Use case | Complexité |
|----------|----------|------------|
| **Strimzi** | Apache Kafka | Haute |
| **Zalando Postgres** | PostgreSQL HA | Moyenne |
| **MongoDB Community** | MongoDB ReplicaSet | Moyenne |
| **Redis Operator** | Redis Cluster | Faible |
| **Elasticsearch Operator** | ELK Stack | Haute |

---

**Mon expérience :**

J'ai utilisé **Strimzi Operator** pour gérer un cluster Kafka :
- 3 brokers Kafka
- 3 nodes ZooKeeper
- Monitoring Prometheus
- Topics auto-créés
- ACLs gérés automatiquement

**Avant Operator :** 2 jours de setup + maintenance hebdomadaire

**Avec Strimzi :** 1h de setup + zéro maintenance (self-healing)

---

### Q2 : Comment créeriez-vous un Custom Resource pour gérer des applications Spring Boot ?

**Réponse :**

Je créerais une **CRD SpringApp** qui abstrait la complexité Kubernetes pour les développeurs.

**Objectif :** Développeur déclare juste l'essentiel, l'Operator crée tout.

**Définition de la CRD :**

```yaml
apiVersion: apiextensions.k8s.io/v1
kind: CustomResourceDefinition
metadata:
  name: springapps.company.com
spec:
  group: company.com
  versions:
    - name: v1
      served: true
      storage: true
      schema:
        openAPIV3Schema:
          type: object
          properties:
            spec:
              type: object
              properties:
                # Image de l'application
                image:
                  type: string
                  pattern: '^[a-z0-9]+([\-\.][a-z0-9]+)*\/[a-z0-9]+([\-\.][a-z0-9]+)*:[a-z0-9\-\.]+$'
                
                # Nombre de replicas
                replicas:
                  type: integer
                  minimum: 1
                  maximum: 20
                  default: 3
                
                # Profil Spring Boot
                profile:
                  type: string
                  enum: ["dev", "staging", "prod"]
                  default: "prod"
                
                # JVM options
                javaOpts:
                  type: string
                  default: "-XX:+UseContainerSupport -XX:MaxRAMPercentage=75.0"
                
                # Resources
                resources:
                  type: object
                  properties:
                    memory:
                      type: string
                      pattern: '^[0-9]+[GM]i$'
                    cpu:
                      type: string
                      pattern: '^[0-9]+m?$'
                
                # Database config
                database:
                  type: object
                  properties:
                    enabled:
                      type: boolean
                      default: true
                    host:
                      type: string
                    port:
                      type: integer
                      default: 5432
                    name:
                      type: string
                
                # Ingress config
                ingress:
                  type: object
                  properties:
                    enabled:
                      type: boolean
                      default: true
                    host:
                      type: string
                    tls:
                      type: boolean
                      default: true
                
                # Monitoring
                monitoring:
                  type: object
                  properties:
                    enabled:
                      type: boolean
                      default: true
                    port:
                      type: integer
                      default: 9090
              
              required:
                - image
  
  scope: Namespaced
  names:
    plural: springapps
    singular: springapp
    kind: SpringApp
    shortNames:
      - sa
```

---

**Utilisation par les développeurs :**

```yaml
apiVersion: company.com/v1
kind: SpringApp
metadata:
  name: my-payment-api
  namespace: production
spec:
  image: registry.company.com/payment-api:v1.2.3
  replicas: 5
  profile: prod
  javaOpts: "-XX:MaxRAMPercentage=75.0 -XX:+UseG1GC"
  resources:
    memory: 2Gi
    cpu: 1000m
  database:
    enabled: true
    host: postgres-service
    port: 5432
    name: payment_db
  ingress:
    enabled: true
    host: payment-api.company.com
    tls: true
  monitoring:
    enabled: true
    port: 9090
```

---

**L'Operator crée automatiquement :**

**1. Deployment**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-payment-api
  namespace: production
spec:
  replicas: 5
  template:
    spec:
      containers:
      - name: app
        image: registry.company.com/payment-api:v1.2.3
        env:
        - name: SPRING_PROFILES_ACTIVE
          value: "prod"
        - name: JAVA_OPTS
          value: "-XX:MaxRAMPercentage=75.0 -XX:+UseG1GC"
        - name: SPRING_DATASOURCE_URL
          value: "jdbc:postgresql://postgres-service:5432/payment_db"
        # ... autres configs auto-générées
```

---

**2. Service**

```yaml
apiVersion: v1
kind: Service
metadata:
  name: my-payment-api
spec:
  ports:
  - name: http
    port: 80
    targetPort: 8080
  - name: management
    port: 9090
    targetPort: 9090
```

---

**3. Ingress**

```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: my-payment-api
  annotations:
    cert-manager.io/cluster-issuer: letsencrypt-prod
spec:
  tls:
  - hosts:
    - payment-api.company.com
    secretName: my-payment-api-tls
  rules:
  - host: payment-api.company.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: my-payment-api
            port:
              number: 80
```

---

**4. ServiceMonitor (Prometheus)**

```yaml
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: my-payment-api
spec:
  endpoints:
  - port: management
    path: /actuator/prometheus
    interval: 30s
```

---

**5. HorizontalPodAutoscaler**

```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: my-payment-api
spec:
  minReplicas: 5
  maxReplicas: 20
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
```

---

**6. PodDisruptionBudget**

```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: my-payment-api
spec:
  minAvailable: 3
```

---

**Avantages pour les développeurs :**

✅ **Simplicité** : 1 manifest au lieu de 6+
✅ **Standards** : Best practices appliquées automatiquement
✅ **Moins d'erreurs** : Validation par CRD
✅ **Self-service** : Développeurs autonomes
✅ **Gouvernance** : Équipe platform contrôle les defaults

---

**Controller en Java (pseudo-code) :**

```java
@Controller
public class SpringAppController {
    
    @EventHandler
    public void onSpringAppCreated(SpringApp app) {
        // 1. Créer Deployment
        Deployment deployment = buildDeployment(app);
        kubernetesClient.apps().deployments()
            .inNamespace(app.getMetadata().getNamespace())
            .createOrReplace(deployment);
        
        // 2. Créer Service
        Service service = buildService(app);
        kubernetesClient.services()
            .inNamespace(app.getMetadata().getNamespace())
            .createOrReplace(service);
        
        // 3. Créer Ingress si enabled
        if (app.getSpec().getIngress().isEnabled()) {
            Ingress ingress = buildIngress(app);
            kubernetesClient.network().v1().ingresses()
                .inNamespace(app.getMetadata().getNamespace())
                .createOrReplace(ingress);
        }
        
        // 4. Créer ServiceMonitor si monitoring enabled
        if (app.getSpec().getMonitoring().isEnabled()) {
            ServiceMonitor monitor = buildServiceMonitor(app);
            // Create via custom resource client
        }
        
        // 5. Update status
        app.getStatus().setPhase("Ready");
        app.getStatus().setReplicas(app.getSpec().getReplicas());
    }
}
```

---

## 📋 Section 2 : Init Containers & Sidecars

### Q3 : Quelle est la différence entre Init Containers et Sidecars ? Donnez des exemples concrets.

**Réponse :**

**Init Containers** : S'exécutent **avant** les containers principaux, **séquentiellement**, et doivent **se terminer avec succès**.

**Sidecars** : S'exécutent **en parallèle** avec le container principal pendant **toute la durée de vie du Pod**.

---

**Comparaison :**

| Aspect | Init Container | Sidecar |
|--------|---------------|---------|
| **Timing** | Avant le main container | Parallèle au main container |
| **Lifecycle** | Se termine (exit 0) | Tourne jusqu'à la fin du Pod |
| **Ordre** | Séquentiel (init1 → init2 → init3) | Tous démarrent ensemble |
| **Échec** | Pod ne démarre pas | Pod continue (selon restart policy) |
| **Use case** | Setup, prerequisites | Logging, monitoring, proxy |
| **Ressources** | Partagent limites du Pod | Ont leurs propres limites |

---

**Exemple 1 : Init Container - Wait for database**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  template:
    spec:
      initContainers:
      # Init 1 : Attendre PostgreSQL
      - name: wait-for-postgres
        image: busybox:1.35
        command:
        - sh
        - -c
        - |
          echo "Waiting for PostgreSQL..."
          until nc -z postgres-service 5432; do
            echo "PostgreSQL not ready, waiting..."
            sleep 2
          done
          echo "PostgreSQL is ready!"
      
      # Init 2 : Attendre Redis
      - name: wait-for-redis
        image: busybox:1.35
        command:
        - sh
        - -c
        - |
          echo "Waiting for Redis..."
          until nc -z redis-service 6379; do
            echo "Redis not ready, waiting..."
            sleep 2
          done
          echo "Redis is ready!"
      
      # Init 3 : Exécuter migrations Flyway
      - name: flyway-migration
        image: flyway/flyway:9.16
        command: ["flyway", "migrate"]
        env:
        - name: FLYWAY_URL
          value: "jdbc:postgresql://postgres-service:5432/mydb"
        - name: FLYWAY_USER
          valueFrom:
            secretKeyRef:
              name: db-credentials
              key: username
        - name: FLYWAY_PASSWORD
          valueFrom:
            secretKeyRef:
              name: db-credentials
              key: password
        volumeMounts:
        - name: migrations
          mountPath: /flyway/sql
      
      containers:
      - name: app
        image: my-spring-app:v1.0.0
        # Démarre SEULEMENT après succès des 3 init containers
      
      volumes:
      - name: migrations
        configMap:
          name: db-migrations
```

**Séquence d'exécution :**

```
1. wait-for-postgres démarre → Vérifie connexion → Exit 0
2. wait-for-redis démarre → Vérifie connexion → Exit 0
3. flyway-migration démarre → Exécute SQL → Exit 0
4. Container "app" démarre

Si un init container échoue → Pod reste en Init:Error
```

---

**Exemple 2 : Sidecar - Log collector**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  template:
    spec:
      containers:
      # Container principal
      - name: app
        image: my-spring-app:v1.0.0
        ports:
        - containerPort: 8080
        volumeMounts:
        - name: logs
          mountPath: /app/logs
        resources:
          limits:
            memory: "1Gi"
            cpu: "1000m"
      
      # Sidecar : Fluentd pour collecter logs
      - name: log-collector
        image: fluent/fluentd:v1.16
        volumeMounts:
        - name: logs
          mountPath: /app/logs
          readOnly: true
        - name: fluentd-config
          mountPath: /fluentd/etc
        env:
        - name: FLUENT_ELASTICSEARCH_HOST
          value: "elasticsearch-service"
        resources:
          limits:
            memory: "256Mi"
            cpu: "200m"
      
      volumes:
      - name: logs
        emptyDir: {}
      - name: fluentd-config
        configMap:
          name: fluentd-config
```

**Fonctionnement :**

```
Container "app" écrit logs → /app/logs/app.log
                             ↓
Sidecar "log-collector" lit → /app/logs/app.log
                             ↓
Parse & envoie → Elasticsearch

Les 2 containers tournent en parallèle !
```

---

**Exemple 3 : Sidecar - Cloud SQL Proxy (GCP)**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  template:
    spec:
      containers:
      # Application
      - name: app
        image: my-spring-app:v1.0.0
        env:
        - name: SPRING_DATASOURCE_URL
          value: "jdbc:postgresql://localhost:5432/mydb"
          # localhost car proxy sidecar écoute sur 5432
      
      # Sidecar : Cloud SQL Proxy
      - name: cloud-sql-proxy
        image: gcr.io/cloudsql-docker/gce-proxy:latest
        command:
        - "/cloud_sql_proxy"
        - "-instances=my-project:europe-west1:my-db=tcp:5432"
        securityContext:
          runAsNonRoot: true
```

**Avantage :** Application se connecte à `localhost:5432`, le proxy gère l'authentification Cloud SQL.

---

**Exemple 4 : Sidecar - Istio Service Mesh**

```yaml
apiVersion: v1
kind: Namespace
metadata:
  name: production
  labels:
    istio-injection: enabled  # Injection automatique !
---
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app
  namespace: production
spec:
  template:
    spec:
      containers:
      - name: app
        image: my-app:v1.0.0

# Istio injecte automatiquement :
# containers:
# - name: istio-proxy
#   image: docker.io/istio/proxyv2:1.20.0
#   (Envoy proxy comme sidecar)
```

**Le sidecar Istio gère :**
- Mutual TLS automatique
- Load balancing intelligent
- Circuit breaking
- Retry logic
- Telemetry (metrics, traces)

---

**Quand utiliser quoi ?**

| Besoin | Solution |
|--------|----------|
| Attendre une dépendance | Init Container |
| Migrations DB | Init Container |
| Download config/data | Init Container |
| Setup permissions | Init Container |
| **Logs en temps réel** | **Sidecar** |
| **Monitoring/Metrics** | **Sidecar** |
| **Service Mesh proxy** | **Sidecar** |
| **File sync continu** | **Sidecar** |

---

## 📋 Section 3 : Taints & Tolerations

### Q4 : Expliquez Taints et Tolerations avec un cas d'usage production

**Réponse :**

**Taints** : Marques sur des **Nodes** qui "repoussent" les Pods.

**Tolerations** : Marques sur des **Pods** qui "tolèrent" certaines taints.

**Analogie :** 
- Taint = "Interdiction d'entrer sauf autorisation"
- Toleration = "J'ai l'autorisation"

---

**Syntaxe d'une Taint :**

```bash
kubectl taint nodes <node> <key>=<value>:<effect>

# Effects:
# - NoSchedule : Nouveaux pods bloqués
# - PreferNoSchedule : Éviter si possible
# - NoExecute : Éviction des pods existants
```

---

**Cas d'usage production : Séparer environnements sur un même cluster**

**Contexte :**
- 1 cluster Kubernetes
- 6 nodes au total
- Besoin : Séparer prod et staging sur des nodes dédiés

**Setup :**

```bash
# Nodes 1-4 : Production
kubectl label nodes node1 environment=production
kubectl label nodes node2 environment=production
kubectl label nodes node3 environment=production
kubectl label nodes node4 environment=production

kubectl taint nodes node1 environment=production:NoSchedule
kubectl taint nodes node2 environment=production:NoSchedule
kubectl taint nodes node3 environment=production:NoSchedule
kubectl taint nodes node4 environment=production:NoSchedule

# Nodes 5-6 : Staging (pas de taint)
kubectl label nodes node5 environment=staging
kubectl label nodes node6 environment=staging
```

---

**Deployment Production :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: payment-api-prod
  namespace: production
spec:
  replicas: 5
  template:
    spec:
      # Toleration pour accéder aux nodes production
      tolerations:
      - key: "environment"
        operator: "Equal"
        value: "production"
        effect: "NoSchedule"
      
      # NodeSelector pour forcer sur nodes production
      nodeSelector:
        environment: production
      
      # Anti-affinity pour répartir
      affinity:
        podAntiAffinity:
          preferredDuringSchedulingIgnoredDuringExecution:
          - weight: 100
            podAffinityTerm:
              labelSelector:
                matchLabels:
                  app: payment-api-prod
              topologyKey: kubernetes.io/hostname
      
      containers:
      - name: app
        image: payment-api:v2.1.0
        resources:
          requests:
            memory: "1Gi"
            cpu: "1000m"
          limits:
            memory: "2Gi"
            cpu: "2000m"
```

**Résultat :** Les 5 replicas seront répartis sur node1, node2, node3, node4 uniquement.

---

**Deployment Staging :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: payment-api-staging
  namespace: staging
spec:
  replicas: 2
  template:
    spec:
      # Pas de toleration = ne peut PAS aller sur nodes prod
      
      nodeSelector:
        environment: staging
      
      containers:
      - name: app
        image: payment-api:v2.2.0-rc1
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
```

**Résultat :** Les 2 replicas seront sur node5 ou node6 uniquement.

---

**Avantages de cette approche :**

✅ **Isolation** : Apps staging ne peuvent jamais "voler" ressources prod
✅ **Predictability** : Prod toujours sur nodes dédiés
✅ **Performance** : Nodes prod peuvent avoir hardware meilleur
✅ **Sécurité** : Séparation physique prod/staging
✅ **Cost optimization** : Nodes staging peuvent être spot instances

---

**Autre cas d'usage : Nodes avec GPU**

```bash
# Marquer nodes avec GPU
kubectl label nodes gpu-node1 gpu=nvidia-tesla-v100
kubectl taint nodes gpu-node1 gpu=nvidia:NoSchedule

kubectl label nodes gpu-node2 gpu=nvidia-tesla-v100
kubectl taint nodes gpu-node2 gpu=nvidia:NoSchedule
```

```yaml
# Deployment ML Training
apiVersion: apps/v1
kind: Deployment
metadata:
  name: ml-training
spec:
  template:
    spec:
      tolerations:
      - key: "gpu"
        operator: "Equal"
        value: "nvidia"
        effect: "NoSchedule"
      
      nodeSelector:
        gpu: nvidia-tesla-v100
      
      containers:
      - name: trainer
        image: ml-training:v1.0.0
        resources:
          limits:
            nvidia.com/gpu: 1  # Request 1 GPU
```

**Résultat :** Seuls les workloads ML avec toleration peuvent utiliser les nodes GPU coûteux.

---

**Effect NoExecute (éviction) :**

```bash
# Marquer un node en maintenance
kubectl taint nodes node1 maintenance=true:NoExecute

# TOUS les pods SANS toleration sont évictés immédiatement !
```

**Toleration avec grace period :**

```yaml
tolerations:
- key: "maintenance"
  operator: "Equal"
  value: "true"
  effect: "NoExecute"
  tolerationSeconds: 300  # Tolérer 5 minutes, puis éviction
```

**Use case :** Drain progressif pour donner temps aux apps de migrer.

---

## 📋 Section 4 : Pod Disruption Budgets

### Q5 : Qu'est-ce qu'un Pod Disruption Budget et pourquoi est-il critique en production ?

**Réponse :**

Un **Pod Disruption Budget (PDB)** garantit qu'un nombre minimum de Pods reste disponible pendant les **disruptions volontaires**.

**Disruptions volontaires :**
- `kubectl drain` (maintenance node)
- Cluster upgrade
- Node auto-scaling down
- Éviction manuelle

**Disruptions INvolontaires (PDB ne protège PAS) :**
- Node crash
- OOMKilled
- Application crash

---

**Syntaxe :**

```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: my-app-pdb
spec:
  # Option 1 : Minimum disponible
  minAvailable: 2
  
  # OU Option 2 : Maximum indisponible
  # maxUnavailable: 1
  
  selector:
    matchLabels:
      app: my-app
```

---

**Exemple concret : API critique avec 5 replicas**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: payment-api
  namespace: production
spec:
  replicas: 5
  selector:
    matchLabels:
      app: payment-api
  template:
    metadata:
      labels:
        app: payment-api
    spec:
      containers:
      - name: app
        image: payment-api:v1.0.0
---
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: payment-api-pdb
  namespace: production
spec:
  minAvailable: 3  # Au moins 3 sur 5 doivent rester up
  selector:
    matchLabels:
      app: payment-api
```

---

**Scénario : Drain d'un node SANS PDB**

```bash
kubectl drain node1 --ignore-daemonsets

# Séquence :
# T+0s : Tous les pods sur node1 évictés en même temps
# T+0s : 3 pods payment-api sur node1 terminés
# T+0s : Reste seulement 2 pods disponibles (sur node2 et node3)
# T+5s : Nouveaux pods démarrent sur node4 et node5
# T+30s : Nouveaux pods Ready

# Problème : Entre T+0s et T+30s, seulement 2 pods !
# → Capacity réduite de 60%
# → Risque de saturation si trafic élevé
# → Possibilité de downtime ❌
```

---

**Scénario : Drain d'un node AVEC PDB**

```bash
kubectl drain node1 --ignore-daemonsets

# Séquence :
# T+0s : Kubernetes tente d'évincer les 3 pods sur node1
# T+0s : PDB bloque ! "Cannot evict: would violate PDB"
# T+0s : Kubernetes évicte seulement 1 pod (reste 4/5 disponibles = OK)
# T+10s : Nouveau pod démarre sur node4 et devient Ready (5/5)
# T+10s : Kubernetes évicte un 2ème pod (reste 4/5 = OK)
# T+20s : Nouveau pod Ready (5/5)
# T+20s : Kubernetes évicte le 3ème pod (reste 4/5 = OK)
# T+30s : Tous les pods migrés, drain complet

# Résultat : Toujours AU MOINS 3 pods disponibles ! ✅
# → Aucune dégradation de service
# → Zero downtime garanti
```

---

**Percentage-based PDB :**

```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: payment-api-pdb
spec:
  maxUnavailable: 25%  # Max 25% des pods peuvent être down
  selector:
    matchLabels:
      app: payment-api

# Avec 4 replicas : max 1 pod down (25% × 4 = 1)
# Avec 8 replicas : max 2 pods down (25% × 8 = 2)
# Avec 10 replicas : max 2 pods down (25% × 10 = 2.5 → arrondi à 2)
```

**Avantage :** S'adapte automatiquement si on scale (HPA).

---

**Pourquoi est-ce CRITIQUE en production ?**

**1. Garantit SLA pendant maintenance**

```
SLA 99.9% = Max 43 minutes downtime/mois

Sans PDB : Drain peut causer 30 min downtime → SLA violé
Avec PDB : Zero downtime pendant drain → SLA respecté ✅
```

---

**2. Protège contre erreurs humaines**

```bash
# Opérateur fait erreur et drain plusieurs nodes en même temps
kubectl drain node1 node2 node3 --ignore-daemonsets

# Sans PDB : Tous les pods évictés → OUTAGE TOTAL ❌
# Avec PDB : Kubernetes refuse si viole minAvailable → Safe ✅
```

---

**3. Permet auto-scaling sécurisé**

```
Cluster Autoscaler veut supprimer un node (scale down)

Sans PDB : Peut supprimer node avec tous les pods d'une app
Avec PDB : Autoscaler respecte PDB, ne scale down que si safe
```

---

**4. Facilite upgrades de cluster**

```bash
# Upgrade Kubernetes 1.28 → 1.29
# Besoin de drainer tous les nodes un par un

Sans PDB : Risque de downtime à chaque drain
Avec PDB : Upgrade transparent, zero downtime garanti ✅
```

---

**Best Practices PDB :**

**1. PDB cohérent avec replicas**

```yaml
# ✅ BON
replicas: 5
minAvailable: 3  # 60% uptime garanti

# ❌ MAUVAIS
replicas: 2
minAvailable: 2  # Impossible de drain un node !

# ✅ BON (alternative)
replicas: 2
maxUnavailable: 1  # Au moins 1 doit rester
```

---

**2. Combiner avec Pod Anti-Affinity**

```yaml
# Garantir HA complète
apiVersion: apps/v1
kind: Deployment
spec:
  replicas: 5
  template:
    spec:
      affinity:
        podAntiAffinity:
          preferredDuringSchedulingIgnoredDuringExecution:
          - weight: 100
            podAffinityTerm:
              labelSelector:
                matchLabels:
                  app: payment-api
              topologyKey: kubernetes.io/hostname
---
apiVersion: policy/v1
kind: PodDisruptionBudget
spec:
  minAvailable: 3
  selector:
    matchLabels:
      app: payment-api

# Résultat :
# - Pods répartis sur différents nodes (anti-affinity)
# - Toujours 3 pods minimum (PDB)
# - Haute disponibilité garantie !
```

---

**3. Multi-AZ avec topologyKey**

```yaml
affinity:
  podAntiAffinity:
    preferredDuringSchedulingIgnoredDuringExecution:
    - weight: 100
      podAffinityTerm:
        labelSelector:
          matchLabels:
            app: payment-api
        topologyKey: topology.kubernetes.io/zone

# Pods répartis sur différentes Availability Zones
# → Protection contre panne d'une AZ complète
```

---

**Mon expérience :**

J'ai eu un incident où on a dû drainer 3 nodes simultanément (problème hardware).

**Sans PDB :** Aurait causé 100% downtime sur 2 applications critiques

**Avec PDB configuré :**
- Kubernetes a refusé de drainer le 3ème node
- Message : "Cannot evict pod: would violate PDB payment-api-pdb"
- On a attendu que les pods migrent
- **Zero downtime** pendant l'opération

→ PDB nous a sauvés d'un incident majeur !

---

## 📋 Section 5 : Node Affinity & Pod Anti-Affinity

### Q6 : Comment garantissez-vous la haute disponibilité d'une application critique avec Node Affinity et Pod Anti-Affinity ?

**Réponse :**

Je combine plusieurs techniques pour garantir HA maximale.

**Architecture cible :**
- Pods répartis sur **différents nodes**
- Pods répartis sur **différentes Availability Zones**
- Uniquement sur nodes **haute performance**
- **PDB** pour protéger pendant maintenance

---

**Configuration complète :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: payment-api
  namespace: production
  labels:
    app: payment-api
    tier: critical
spec:
  replicas: 6  # 2 par AZ
  
  selector:
    matchLabels:
      app: payment-api
  
  template:
    metadata:
      labels:
        app: payment-api
        tier: critical
    
    spec:
      # =================================
      # 1. Node Affinity (nodes haute perf)
      # =================================
      affinity:
        nodeAffinity:
          # REQUIS : Seulement sur nodes haute performance
          requiredDuringSchedulingIgnoredDuringExecution:
            nodeSelectorTerms:
            - matchExpressions:
              - key: performance
                operator: In
                values:
                - high
              - key: node-type
                operator: In
                values:
                - compute-optimized
          
          # PRÉFÉRÉ : Éviter nodes spot instances
          preferredDuringSchedulingIgnoredDuringExecution:
          - weight: 100
            preference:
              matchExpressions:
              - key: instance-lifecycle
                operator: NotIn
                values:
                - spot
        
        # =================================
        # 2. Pod Anti-Affinity (répartition)
        # =================================
        podAntiAffinity:
          # REQUIS : Différents nodes
          requiredDuringSchedulingIgnoredDuringExecution:
          - labelSelector:
              matchLabels:
                app: payment-api
            topologyKey: kubernetes.io/hostname
          
          # PRÉFÉRÉ : Différentes AZ
          preferredDuringSchedulingIgnoredDuringExecution:
          - weight: 100
            podAffinityTerm:
              labelSelector:
                matchLabels:
                  app: payment-api
              topologyKey: topology.kubernetes.io/zone
      
      # =================================
      # 3. Containers configuration
      # =================================
      containers:
      - name: app
        image: payment-api:v2.1.0
        
        ports:
        - name: http
          containerPort: 8080
        - name: management
          containerPort: 9090
        
        # Resources (guaranteed QoS)
        resources:
          requests:
            memory: "1Gi"
            cpu: "1000m"
          limits:
            memory: "1Gi"  # Égal à requests = Guaranteed QoS
            cpu: "1000m"
        
        # Health checks
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 9090
          initialDelaySeconds: 60
          periodSeconds: 10
          timeoutSeconds: 5
          failureThreshold: 3
        
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 9090
          initialDelaySeconds: 30
          periodSeconds: 5
          timeoutSeconds: 3
          failureThreshold: 3
        
        # Graceful shutdown
        lifecycle:
          preStop:
            exec:
              command: ["sh", "-c", "sleep 10"]
      
      # Security
      securityContext:
        runAsNonRoot: true
        runAsUser: 1001
        fsGroup: 1001
      
      terminationGracePeriodSeconds: 60

---
# =================================
# 4. Pod Disruption Budget
# =================================
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: payment-api-pdb
  namespace: production
spec:
  minAvailable: 4  # Au moins 4/6 doivent rester
  selector:
    matchLabels:
      app: payment-api

---
# =================================
# 5. Horizontal Pod Autoscaler
# =================================
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: payment-api-hpa
  namespace: production
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: payment-api
  
  minReplicas: 6
  maxReplicas: 20
  
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
  
  - type: Resource
    resource:
      name: memory
      target:
        type: Utilization
        averageUtilization: 80
  
  behavior:
    scaleDown:
      stabilizationWindowSeconds: 300
      policies:
      - type: Percent
        value: 25
        periodSeconds: 60
    scaleUp:
      stabilizationWindowSeconds: 0
      policies:
      - type: Percent
        value: 50
        periodSeconds: 30
      - type: Pods
        value: 3
        periodSeconds: 30
      selectPolicy: Max
```

---

**Résultat de cette configuration :**

**Vérifier le placement des Pods :**

```bash
kubectl get pods -n production -l app=payment-api -o wide

# Output attendu :
# NAME                  NODE      ZONE
# payment-api-abc123    node1     eu-west-1a
# payment-api-def456    node2     eu-west-1b
# payment-api-ghi789    node3     eu-west-1c
# payment-api-jkl012    node4     eu-west-1a
# payment-api-mno345    node5     eu-west-1b
# payment-api-pqr678    node6     eu-west-1c

# ✅ 6 pods sur 6 nodes différents
# ✅ 2 pods par AZ (eu-west-1a, 1b, 1c)
# ✅ Tous sur nodes "performance=high"
```

---

**Garanties de haute disponibilité :**

| Scénario | Sans HA config | Avec HA config |
|----------|---------------|----------------|
| **1 node crash** | Peut perdre tous les pods | Max 1 pod perdu (anti-affinity) |
| **1 AZ down** | Peut perdre tous les pods | Max 2 pods perdus, 4 restent |
| **Drain 1 node** | Downtime possible | PDB garantit 4 pods min |
| **Drain 2 nodes** | Downtime probable | PDB garantit 4 pods min |
| **Spike de trafic** | Saturation | HPA scale automatiquement |

**Uptime garanti : > 99.95%**

---

**Labels sur les nodes (prérequis) :**

```bash
# Marquer nodes par performance
kubectl label nodes node1 node2 node3 performance=high node-type=compute-optimized
kubectl label nodes node4 node5 node6 performance=high node-type=compute-optimized

# Marquer nodes par AZ (automatique sur cloud providers)
# AWS : topology.kubernetes.io/zone=eu-west-1a
# GCP : topology.kubernetes.io/zone=europe-west1-b
# Azure : topology.kubernetes.io/zone=westeurope-1
```

---

**Test de résilience :**

```bash
# Test 1 : Simuler crash d'un node
kubectl drain node1 --ignore-daemonsets --delete-emptydir-data

# Vérifier que 5 pods restent Running
kubectl get pods -l app=payment-api
# 5/6 Running, 1/6 Terminating

# Attendre nouveau pod
kubectl wait --for=condition=Ready pod -l app=payment-api --timeout=120s
# 6/6 Running sur nodes différents

# Test 2 : Simuler panne d'une AZ (cordon tous les nodes de cette AZ)
kubectl cordon node1 node4  # eu-west-1a
kubectl delete pods -l app=payment-api -n production --field-selector spec.nodeName=node1
kubectl delete pods -l app=payment-api -n production --field-selector spec.nodeName=node4

# Résultat : 4 pods sur eu-west-1b et eu-west-1c continuent
# Nouveaux pods schedulés sur eu-west-1b et eu-west-1c
```

---

**Monitoring de la répartition :**

```yaml
# Prometheus alert si mauvaise répartition
- alert: PodAffinityViolation
  expr: |
    count by (node) (kube_pod_info{pod=~"payment-api.*"}) > 1
  for: 5m
  labels:
    severity: warning
  annotations:
    summary: "Multiple payment-api pods on same node"
    description: "Node {{ $labels.node }} has multiple pods"
```

---

**Mon expérience :**

J'ai configuré une app critique avec cette stratégie complète.

**Incident réel :** Panne complète d'une AZ AWS (eu-west-1a) pendant 2 heures.

**Résultat :**
- 2 pods perdus (ceux sur eu-west-1a)
- 4 pods restants ont absorbé le trafic
- HPA a scalé à 8 pods (sur eu-west-1b et 1c)
- **Zero downtime** côté utilisateur
- Latency p95 : +15ms seulement

→ Haute disponibilité fonctionnelle ! ✅

---

## 📋 Section 6 : Cluster Maintenance

### Q7 : Décrivez votre process complet pour drainer un node en production sans impacter les utilisateurs

**Réponse :**

Voici mon process étape par étape pour un drain **zero downtime**.

**Prérequis (à vérifier AVANT le drain) :**

**1. Vérifier que les apps ont des PDB**

```bash
kubectl get pdb --all-namespaces

# Output attendu :
# NAMESPACE    NAME              MIN AVAILABLE   ALLOWED DISRUPTIONS
# production   payment-api-pdb   3               2
# production   user-api-pdb      2               1
```

Si manquant :

```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: my-app-pdb
spec:
  minAvailable: 2
  selector:
    matchLabels:
      app: my-app
```

---

**2. Vérifier replicas suffisants**

```bash
kubectl get deployments --all-namespaces -o wide

# Si replicas < 3 pour apps critiques :
kubectl scale deployment my-app --replicas=3 -n production
```

---

**3. Vérifier répartition des pods**

```bash
# Lister tous les pods du node à drainer
kubectl get pods --all-namespaces -o wide --field-selector spec.nodeName=node1

# Vérifier qu'aucune app n'a TOUS ses pods sur ce node
kubectl get pods -l app=payment-api -o wide
# Si tous sur node1 → PROBLÈME ! Attendre qu'ils se répartissent
```

---

**4. Vérifier anti-affinity configurée**

```bash
kubectl get deployment payment-api -o yaml | grep -A 10 podAntiAffinity

# Si absent, ajouter avant le drain
```

---

**PHASE 1 : Préparation**

```bash
# 1. Notifier l'équipe
# Slack : "🚧 Maintenance node1 à 14h - Durée estimée: 30 min"

# 2. Activer monitoring intensif
# Ouvrir Grafana dashboards :
# - Error rate
# - Latency p95/p99
# - Request throughput
# - Pod count par deployment

# 3. Backup si opération risquée
velero backup create pre-drain-node1 --include-namespaces production

# 4. Vérifier health global
kubectl get nodes
kubectl get pods --all-namespaces | grep -v Running | grep -v Completed
```

---

**PHASE 2 : Execution du drain**

```bash
# Terminal 1 : Exécuter le drain
kubectl drain node1 \
  --ignore-daemonsets \
  --delete-emptydir-data \
  --grace-period=300 \
  --timeout=600s \
  --pod-selector='!node-role.kubernetes.io/master'

# Flags expliqués :
# --ignore-daemonsets : Ne pas bloquer sur DaemonSets (monitoring, etc.)
# --delete-emptydir-data : Accepter perte data dans emptyDir
# --grace-period=300 : 5 min pour graceful shutdown
# --timeout=600s : Timeout total 10 min
# --pod-selector : Filtre optionnel
```

---

**PHASE 3 : Monitoring pendant le drain**

```bash
# Terminal 2 : Watch pods en temps réel
watch -n 2 'kubectl get pods --all-namespaces -o wide | grep node1'

# Terminal 3 : Watch node status
kubectl get node node1 -w

# Terminal 4 : Events en temps réel
kubectl get events --all-namespaces --watch | grep node1

# Terminal 5 : Métriques applicatives
# Grafana : Surveiller error rate, latency
```

**Ce qu'on surveille :**

```
✅ Pods terminent proprement (Terminating → pas d'erreurs)
✅ Nouveaux pods démarrent sur autres nodes (Pending → Running → Ready)
✅ PDB respectés (pas de violation)
✅ Error rate stable (< 0.1%)
✅ Latency stable (p95 < 500ms)
```

---

**PHASE 4 : Validation post-drain**

```bash
# 1. Vérifier que le node est drained
kubectl get node node1
# NAME    STATUS                     ROLES    AGE
# node1   Ready,SchedulingDisabled   <none>   100d

# 2. Vérifier aucun pod restant (sauf DaemonSets)
kubectl get pods --all-namespaces -o wide --field-selector spec.nodeName=node1

# Output attendu : Seulement DaemonSets (kube-proxy, monitoring-agent, etc.)

# 3. Vérifier que tous les pods sont reschedulés
kubectl get pods --all-namespaces | grep -v Running | grep -v Completed
# Aucun pod en Pending ou Error

# 4. Vérifier health des applications
kubectl get pods -l app=payment-api
# 5/5 Running (tous sur d'autres nodes)

# 5. Test endpoint applicatif
curl https://payment-api.company.com/actuator/health
# {"status":"UP"}

# 6. Vérifier métriques
# Grafana : Error rate, latency, throughput → Tous normaux
```

---

**PHASE 5 : Maintenance du node**

```bash
# SSH vers le node
ssh node1

# Effectuer la maintenance :
# - Upgrade kernel : apt-get upgrade
# - Upgrade kubelet : apt-get install kubelet=1.29.0-00
# - Reboot si nécessaire : reboot

# Attendre que le node redémarre
```

---

**PHASE 6 : Réintégration du node**

```bash
# 1. Vérifier que le node est Ready
kubectl get node node1
# NAME    STATUS   ROLES    AGE
# node1   Ready    <none>   100d

# 2. Uncordon (réactiver scheduling)
kubectl uncordon node1

# 3. Vérifier que des pods commencent à être schedulés
sleep 30
kubectl get pods --all-namespaces -o wide --field-selector spec.nodeName=node1

# 4. Forcer re-balance si nécessaire (optionnel)
kubectl rollout restart deployment payment-api -n production

# 5. Vérifier équilibre global
kubectl get pods -o wide | awk '{print $7}' | sort | uniq -c
# Output attendu : Distribution équilibrée sur tous les nodes
```

---

**PHASE 7 : Post-mortem**

```bash
# 1. Vérifier qu'aucune erreur dans les logs
kubectl logs -l app=payment-api --since=30m | grep ERROR

# 2. Documenter l'opération
# Confluence/Wiki :
# - Durée totale : 25 minutes
# - Pods évictés : 12
# - Incidents : Aucun
# - Downtime : 0 seconde

# 3. Notifier fin de maintenance
# Slack : "✅ Maintenance node1 terminée - Aucun incident"
```

---

**Checklist complète (à imprimer) :**

```
AVANT LE DRAIN :
☐ PDB configurés (minAvailable)
☐ Replicas ≥ 3 pour apps critiques
☐ Anti-affinity configurée
☐ Backup effectué (Velero)
☐ Monitoring actif (Grafana)
☐ Équipe notifiée

PENDANT LE DRAIN :
☐ Grace period suffisant (300s)
☐ Timeout approprié (600s)
☐ Surveiller pods (watch)
☐ Surveiller métriques (Grafana)
☐ Vérifier PDB respectés

APRÈS LE DRAIN :
☐ Node drained (SchedulingDisabled)
☐ Aucun pod restant (sauf DaemonSets)
☐ Tous pods reschedulés (Running)
☐ Health checks OK
☐ Métriques normales

RÉINTÉGRATION :
☐ Node Ready
☐ Uncordon effectué
☐ Pods commencent à être schedulés
☐ Distribution équilibrée
☐ Documentation mise à jour
```

---

**En cas de problème :**

```bash
# Abort du drain (Ctrl+C)
# Le drain s'arrête

# Uncordon immédiatement
kubectl uncordon node1

# Investiguer
kubectl describe node node1
kubectl get events --all-namespaces --sort-by='.lastTimestamp' | head -50

# Rollback si nécessaire
kubectl rollout undo deployment payment-api -n production

# Restore backup si critique
velero restore create --from-backup pre-drain-node1
```

---

**Mon expérience :**

J'ai drainé **~100 nodes** en production sur 2 ans, **zero incident** grâce à :

✅ **Checklist stricte** : Jamais dévier du process
✅ **PDB obligatoire** : Toutes les apps critiques ont minAvailable
✅ **Monitoring temps réel** : Grafana + Slack alerts
✅ **Heures creuses** : Drain entre 2h-6h du matin
✅ **Progressive** : 1 node toutes les 30 minutes max
✅ **Rollback plan** : Toujours prêt à abort

**Durée moyenne d'un drain :** 20-25 minutes (dont 15 min d'attente)

---

## 🎓 Conseils Finaux pour l'Entretien

### Questions que VOUS devriez poser au recruteur :

**Sur l'infrastructure :**
1. Quelle est la taille de vos clusters Kubernetes ? (nodes, pods)
2. Utilisez-vous des Operators ? Lesquels ?
3. Comment gérez-vous les secrets ? (Vault, External Secrets, Sealed Secrets)
4. Quelle est votre stratégie de backup ? (Velero, autres)
5. Multi-cluster ou mono-cluster ?

**Sur les pratiques :**
6. Utilisez-vous GitOps ? (ArgoCD, Flux)
7. Quelle est votre stratégie de déploiement ? (Blue/Green, Canary)
8. Comment gérez-vous les PDB et la haute disponibilité ?
9. Fréquence des upgrades de cluster ?
10. Quels sont vos SLA ?

**Sur le monitoring :**
11. Stack de monitoring ? (Prometheus, Datadog, autres)
12. Comment gérez-vous les alertes ?
13. Avez-vous du tracing distribué ? (Jaeger, Zipkin)

---

### Points clés à mentionner dans vos réponses :

✅ **Expérience CKAD** : "J'ai passé la CKAD, donc je maîtrise les fondamentaux..."

✅ **Production mindset** : Toujours parler de HA, PDB, monitoring, rollback

✅ **Security-first** : RBAC, Network Policies, non-root containers, scanning

✅ **GitOps** : "Je préfère ArgoCD pour la traçabilité et le self-healing..."

✅ **Best practices** : Anti-affinity, graceful shutdown, resource limits

✅ **Troubleshooting** : "J'utilise kubectl logs, describe, events, top..."

---

### Erreurs à éviter :

❌ Dire "Je ne sais pas" sans proposer d'approche

❌ Réponses trop théoriques sans exemples concrets

❌ Ignorer les aspects sécurité

❌ Ne pas mentionner les tests (staging avant prod)

❌ Oublier le monitoring et l'observabilité

---

### Structure de réponse STAR :

**Situation** : Contexte du problème

**Task** : Votre responsabilité

**Action** : Ce que vous avez fait (détails techniques)

**Result** : Résultat mesurable

**Exemple :**

> **S:** Notre cluster avait des problèmes de disponibilité pendant les maintenances
>
> **T:** J'ai été chargé d'améliorer la résilience
>
> **A:** J'ai implémenté :
> - PDB avec minAvailable sur toutes les apps critiques
> - Pod Anti-Affinity pour répartir les pods
> - HPA pour auto-scaling
> - Monitoring Prometheus avec alertes
>
> **R:** Après 6 mois : Zero downtime pendant 15 drains de nodes, SLA 99.95% atteint

---

## 📊 Récapitulatif Complet des 12 Chapitres

Vous maîtrisez maintenant :

**Fondamentaux (Ch 1-6) :**
- Architecture Kubernetes
- Workloads (Pods, Deployments, StatefulSets)
- Configuration (ConfigMaps, Secrets)
- Networking (Services, Ingress, Network Policies)
- Storage (PV, PVC, StorageClasses)
- Security (RBAC, Pod Security Standards)

**Observabilité (Ch 7-8) :**
- Logging, Monitoring, Debugging
- Resource Management (Requests, Limits, HPA)

**Package Management (Ch 9) :**
- Helm (Charts, Templates, Values)

**CI/CD (Ch 10) :**
- GitOps, ArgoCD
- Deployment Strategies (Blue/Green, Canary)
- Pipelines (Jenkins, GitLab CI)
- Image Best Practices

**Spring Boot (Ch 11) :**
- Containerization optimisée
- Configuration externalisée
- Graceful Shutdown
- JVM Tuning
- Actuator & Health Checks

**Advanced (Ch 12) :**
- CRDs & Operators
- Init Containers & Sidecars
- Taints & Tolerations
- Affinity & Anti-Affinity
- Pod Disruption Budgets
- Cluster Maintenance

---
