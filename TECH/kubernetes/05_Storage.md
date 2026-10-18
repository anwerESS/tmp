## [**..**](./00_index.md)


# Chapitre 5 : Storage

## 5.1. Volumes (emptyDir, hostPath, etc.)

### Qu'est-ce qu'un Volume ?

Un **Volume** est un répertoire accessible aux containers d'un Pod. Les volumes permettent de :
- Partager des données entre containers d'un même Pod
- Persister des données au-delà du lifecycle d'un container
- Monter des configurations, secrets, ou stockage externe

**Différence avec Docker volumes :**
- Docker : Volume lié au container
- Kubernetes : Volume lié au **Pod** (partagé entre containers)

---

### Type 1 : emptyDir

Volume temporaire créé quand le Pod démarre, supprimé quand le Pod est supprimé.

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: shared-volume-pod
spec:
  containers:
  # Container 1 : Write
  - name: writer
    image: busybox
    command: ['sh', '-c', 'while true; do date >> /data/log.txt; sleep 5; done']
    volumeMounts:
    - name: shared-data
      mountPath: /data
  
  # Container 2 : Read
  - name: reader
    image: busybox
    command: ['sh', '-c', 'tail -f /data/log.txt']
    volumeMounts:
    - name: shared-data
      mountPath: /data
      readOnly: true
  
  volumes:
  - name: shared-data
    emptyDir: {}  # Volume temporaire
```

**Caractéristiques :**
- Créé vide au démarrage du Pod
- Supprimé quand le Pod est supprimé
- Stocké sur le disque du Node
- Partagé entre tous les containers du Pod

**Use cases :**
- Cache temporaire
- Logs temporaires
- Communication inter-containers
- Scratch space pour processing

**emptyDir avec size limit :**
```yaml
volumes:
- name: cache
  emptyDir:
    sizeLimit: 1Gi  # Limite à 1GB
```

**emptyDir en RAM (tmpfs) :**
```yaml
volumes:
- name: memory-cache
  emptyDir:
    medium: Memory  # Stocké en RAM
    sizeLimit: 500Mi
```

**Use case RAM :**
- Performance critique (10x plus rapide que disque)
- Données très volatiles
- Attention : consomme la mémoire du Node

---

### Type 2 : hostPath

Monte un fichier ou répertoire depuis le filesystem du **Node**.

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: hostpath-pod
spec:
  containers:
  - name: app
    image: nginx
    volumeMounts:
    - name: host-logs
      mountPath: /var/log/nginx
  
  volumes:
  - name: host-logs
    hostPath:
      path: /var/log/pods  # Chemin sur le Node
      type: DirectoryOrCreate  # Crée si n'existe pas
```

**Type options :**
- `Directory` : Répertoire doit exister
- `DirectoryOrCreate` : Crée si n'existe pas
- `File` : Fichier doit exister
- `FileOrCreate` : Crée si n'existe pas
- `Socket` : Unix socket

**⚠️ Problèmes de sécurité :**
```yaml
# ❌ DANGER : Accès au Docker socket du Node
volumes:
- name: docker-sock
  hostPath:
    path: /var/run/docker.sock
    type: Socket

# Pod peut maintenant contrôler Docker sur le Node !
# = Root access au Node
```

**Use cases légitimes :**
- DaemonSets (monitoring, logging)
- Accès à des fichiers spécifiques du Node
- Development/testing

**⚠️ Éviter en production :**
- Pas portable (dépend du Node)
- Risques de sécurité
- Pas compatible multi-Node (Pod peut être schedulé ailleurs)

---

### Type 3 : configMap

Monte un ConfigMap comme volume.

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: app-config
data:
  application.properties: |
    server.port=8080
    database.host=postgres
  
  nginx.conf: |
    server {
      listen 80;
      location / {
        proxy_pass http://backend:8080;
      }
    }
---
apiVersion: v1
kind: Pod
metadata:
  name: config-pod
spec:
  containers:
  - name: app
    image: nginx
    volumeMounts:
    - name: config-volume
      mountPath: /etc/nginx
      readOnly: true
  
  volumes:
  - name: config-volume
    configMap:
      name: app-config
      items:  # Sélection optionnelle
      - key: nginx.conf
        path: nginx.conf
        mode: 0644
```

**Résultat dans le container :**
```bash
ls /etc/nginx/
# nginx.conf

cat /etc/nginx/nginx.conf
# server {
#   listen 80;
#   ...
# }
```

---

### Type 4 : secret

Monte un Secret comme volume.

```yaml
apiVersion: v1
kind: Secret
metadata:
  name: db-credentials
type: Opaque
stringData:
  username: "admin"
  password: "SuperSecret123!"
---
apiVersion: v1
kind: Pod
metadata:
  name: secret-pod
spec:
  containers:
  - name: app
    image: myapp:1.0
    volumeMounts:
    - name: db-secrets
      mountPath: /secrets
      readOnly: true
  
  volumes:
  - name: db-secrets
    secret:
      secretName: db-credentials
      defaultMode: 0400  # Permissions restrictives
```

**Résultat :**
```bash
ls -la /secrets/
# -r-------- 1 root root username
# -r-------- 1 root root password

cat /secrets/password
# SuperSecret123!
```

---

### Type 5 : projected

Combine plusieurs sources (ConfigMap, Secret, downwardAPI) dans un seul volume.

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: projected-pod
spec:
  containers:
  - name: app
    image: myapp:1.0
    volumeMounts:
    - name: combined-volume
      mountPath: /combined
  
  volumes:
  - name: combined-volume
    projected:
      sources:
      # ConfigMap
      - configMap:
          name: app-config
          items:
          - key: application.properties
            path: config/app.properties
      
      # Secret
      - secret:
          name: db-credentials
          items:
          - key: password
            path: secrets/db-password
      
      # Downward API
      - downwardAPI:
          items:
          - path: "metadata/labels"
            fieldRef:
              fieldPath: metadata.labels
```

**Résultat :**
```bash
/combined/
├── config/
│   └── app.properties
├── secrets/
│   └── db-password
└── metadata/
    └── labels
```

---

### Comparaison des types de volumes

| Type | Lifetime | Partageable | Persistence | Use case |
|------|----------|-------------|-------------|----------|
| **emptyDir** | Pod | Intra-Pod | Non | Cache, temp files |
| **hostPath** | Node | Inter-Pods | Oui (Node) | DaemonSets, dev |
| **configMap** | External | Multi-Pods | Oui | Configuration |
| **secret** | External | Multi-Pods | Oui | Credentials |
| **PVC** | External | Selon mode | Oui | Données applicatives |

---

## 5.2. PersistentVolumes (PV) & PersistentVolumeClaims (PVC)

### Architecture PV/PVC

```
┌─────────────────────────────────────┐
│      Admin (Infrastructure)         │
│  Provisionne PersistentVolume (PV)  │
└────────────┬────────────────────────┘
             │
             │ Bind
             ↓
┌────────────┴────────────────────────┐
│  Developer (Application)            │
│  Crée PersistentVolumeClaim (PVC)   │
└────────────┬────────────────────────┘
             │
             │ Mount
             ↓
┌────────────┴────────────────────────┐
│            Pod                      │
│   volumeMounts: /data               │
└─────────────────────────────────────┘
```

**Séparation des responsabilités :**
- **Admin** : Gère le stockage physique (PV)
- **Developer** : Demande du stockage (PVC)
- **Kubernetes** : Fait le matching (binding)

---

### PersistentVolume (PV)

Représente une ressource de stockage dans le cluster.

```yaml
apiVersion: v1
kind: PersistentVolume
metadata:
  name: pv-local
spec:
  capacity:
    storage: 10Gi
  
  accessModes:
  - ReadWriteOnce  # RWO
  
  persistentVolumeReclaimPolicy: Retain
  
  storageClassName: local-storage
  
  # Backend storage
  hostPath:
    path: /mnt/data
    type: DirectoryOrCreate
```

**Propriétés importantes :**

**1. capacity :**
```yaml
capacity:
  storage: 10Gi  # Taille
```

**2. accessModes :**
- **ReadWriteOnce (RWO)** : 1 Node en lecture/écriture
- **ReadOnlyMany (ROX)** : N Nodes en lecture seule
- **ReadWriteMany (RWX)** : N Nodes en lecture/écriture

**3. persistentVolumeReclaimPolicy :**
- **Retain** : PV conservé après suppression du PVC (data intact)
- **Delete** : PV supprimé avec le PVC (data perdue)
- **Recycle** : (déprécié) Data effacée, PV disponible pour reuse

**4. storageClassName :**
Associe le PV à une classe de stockage (provisioning dynamique).

---

### PersistentVolumeClaim (PVC)

Demande de stockage par une application.

```yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: my-pvc
spec:
  accessModes:
  - ReadWriteOnce
  
  resources:
    requests:
      storage: 5Gi  # Demande 5Gi (PV doit avoir >= 5Gi)
  
  storageClassName: local-storage
```

**Binding automatique :**
```bash
kubectl get pvc
# NAME     STATUS   VOLUME      CAPACITY   ACCESS MODES   STORAGECLASS
# my-pvc   Bound    pv-local    10Gi       RWO            local-storage

# Le PVC (5Gi) est bind au PV (10Gi)
# Le PVC obtient 10Gi (capacity du PV)
```

---

### Utiliser un PVC dans un Pod

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: pvc-pod
spec:
  containers:
  - name: app
    image: nginx
    volumeMounts:
    - name: data
      mountPath: /usr/share/nginx/html
  
  volumes:
  - name: data
    persistentVolumeClaim:
      claimName: my-pvc  # Référence au PVC
```

**Lifecycle :**
```bash
# 1. Créer PV (Admin)
kubectl apply -f pv.yaml

# 2. Créer PVC (Developer)
kubectl apply -f pvc.yaml

# 3. Binding automatique
kubectl get pvc
# STATUS: Bound

# 4. Utiliser dans Pod
kubectl apply -f pod.yaml

# 5. Données persistent même si Pod est supprimé
kubectl delete pod pvc-pod
kubectl apply -f pod.yaml  # Nouveau Pod, mêmes données

# 6. Supprimer PVC
kubectl delete pvc my-pvc
# PV status: Released (si policy=Retain)
# Data toujours présente sur le disque
```

---

### Exemple : PostgreSQL avec PVC

```yaml
# PVC pour PostgreSQL
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: postgres-pvc
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 20Gi
  storageClassName: fast-ssd
---
# Deployment PostgreSQL
apiVersion: apps/v1
kind: Deployment
metadata:
  name: postgres
spec:
  replicas: 1  # RWO = 1 replica max
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
        env:
        - name: POSTGRES_PASSWORD
          valueFrom:
            secretKeyRef:
              name: postgres-secret
              key: password
        - name: PGDATA
          value: /var/lib/postgresql/data/pgdata
        volumeMounts:
        - name: postgres-storage
          mountPath: /var/lib/postgresql/data
      
      volumes:
      - name: postgres-storage
        persistentVolumeClaim:
          claimName: postgres-pvc
```

**Avantages :**
- Données persistent si Pod crash
- Données persistent si Deployment scale down/up
- Backup possible via snapshot du PV

---

## 5.3. StorageClasses

### Qu'est-ce qu'une StorageClass ?

Une **StorageClass** définit les "classes" de stockage disponibles (SSD, HDD, cloud storage) et permet le **provisioning dynamique** de PersistentVolumes.

**Sans StorageClass (Static Provisioning) :**
```
Admin crée PV manuellement → Developer crée PVC → Binding
```

**Avec StorageClass (Dynamic Provisioning) :**
```
Developer crée PVC → StorageClass provisionne PV automatiquement → Binding
```

---

### StorageClass Examples

**1. Local Storage :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: local-storage
provisioner: kubernetes.io/no-provisioner  # Pas de provisioning dynamique
volumeBindingMode: WaitForFirstConsumer    # Bind quand Pod est schedulé
```

**2. AWS EBS :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: aws-ebs-gp3
provisioner: ebs.csi.aws.com
parameters:
  type: gp3
  iops: "3000"
  throughput: "125"
  encrypted: "true"
allowVolumeExpansion: true
volumeBindingMode: WaitForFirstConsumer
```

**3. GCP Persistent Disk :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: gcp-ssd
provisioner: pd.csi.storage.gke.io
parameters:
  type: pd-ssd
  replication-type: regional-pd
allowVolumeExpansion: true
```

**4. Azure Disk :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: azure-disk-premium
provisioner: disk.csi.azure.com
parameters:
  storageaccounttype: Premium_LRS
  kind: Managed
allowVolumeExpansion: true
```

**5. NFS :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: nfs-storage
provisioner: nfs.csi.k8s.io
parameters:
  server: nfs-server.example.com
  share: /export/data
mountOptions:
  - hard
  - nfsvers=4.1
```

---

### Dynamic Provisioning

**1. Créer StorageClass :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: fast-ssd
provisioner: kubernetes.io/aws-ebs
parameters:
  type: gp3
allowVolumeExpansion: true
```

**2. Créer PVC (pas besoin de PV manuel) :**
```yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: dynamic-pvc
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 50Gi
  storageClassName: fast-ssd  # Utilise la StorageClass
```

**3. PV créé automatiquement :**
```bash
kubectl get pvc
# NAME          STATUS   VOLUME                                     CAPACITY
# dynamic-pvc   Bound    pvc-abc123-def456-ghi789                  50Gi

kubectl get pv
# NAME                        CAPACITY   ACCESS MODES   RECLAIM POLICY   STATUS   CLAIM
# pvc-abc123-def456-ghi789    50Gi       RWO            Delete           Bound    default/dynamic-pvc
```

**4. Utiliser dans un Pod :**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: app
spec:
  containers:
  - name: app
    image: myapp:1.0
    volumeMounts:
    - name: data
      mountPath: /data
  volumes:
  - name: data
    persistentVolumeClaim:
      claimName: dynamic-pvc
```

---

### StorageClass par défaut

```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: standard
  annotations:
    storageclass.kubernetes.io/is-default-class: "true"  # Défaut
provisioner: kubernetes.io/aws-ebs
```

**PVC sans storageClassName :**
```yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: default-pvc
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 10Gi
  # storageClassName non spécifié → utilise la classe par défaut
```

---

### Volume Expansion

**Activer l'expansion :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: expandable-storage
provisioner: ebs.csi.aws.com
allowVolumeExpansion: true  # ← Permet l'expansion
```

**Agrandir un PVC :**
```bash
# PVC initial : 10Gi
kubectl get pvc my-pvc
# CAPACITY: 10Gi

# Éditer le PVC
kubectl edit pvc my-pvc
# Changer: storage: 10Gi → storage: 20Gi

# Expansion automatique
kubectl get pvc my-pvc
# CAPACITY: 20Gi (après quelques secondes)
```

**⚠️ Notes :**
- Expansion possible : 10Gi → 20Gi ✅
- Réduction impossible : 20Gi → 10Gi ❌
- Certains types nécessitent restart du Pod

---

## 5.4. Dynamic Provisioning

### CSI (Container Storage Interface)

**CSI** est l'interface standard pour les plugins de stockage dans Kubernetes.

**Provisioners CSI populaires :**
- **AWS EBS CSI** : `ebs.csi.aws.com`
- **GCP PD CSI** : `pd.csi.storage.gke.io`
- **Azure Disk CSI** : `disk.csi.azure.com`
- **Ceph RBD CSI** : `rbd.csi.ceph.com`
- **NFS CSI** : `nfs.csi.k8s.io`

---

### Exemple complet : Application Stateful

```yaml
# 1. StorageClass
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: fast-ssd
provisioner: ebs.csi.aws.com
parameters:
  type: gp3
  iops: "3000"
  encrypted: "true"
allowVolumeExpansion: true
volumeBindingMode: WaitForFirstConsumer
---
# 2. StatefulSet avec volumeClaimTemplates
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: mongodb
spec:
  serviceName: mongodb
  replicas: 3
  selector:
    matchLabels:
      app: mongodb
  template:
    metadata:
      labels:
        app: mongodb
    spec:
      containers:
      - name: mongodb
        image: mongo:6.0
        ports:
        - containerPort: 27017
        volumeMounts:
        - name: mongo-data
          mountPath: /data/db
        env:
        - name: MONGO_INITDB_ROOT_USERNAME
          value: admin
        - name: MONGO_INITDB_ROOT_PASSWORD
          valueFrom:
            secretKeyRef:
              name: mongo-secret
              key: password
  
  # volumeClaimTemplates : 1 PVC par Pod
  volumeClaimTemplates:
  - metadata:
      name: mongo-data
    spec:
      accessModes: ["ReadWriteOnce"]
      storageClassName: fast-ssd
      resources:
        requests:
          storage: 100Gi
```

**Résultat :**
```bash
kubectl get pvc
# NAME                  STATUS   VOLUME                    CAPACITY
# mongo-data-mongodb-0  Bound    pvc-abc123...             100Gi
# mongo-data-mongodb-1  Bound    pvc-def456...             100Gi
# mongo-data-mongodb-2  Bound    pvc-ghi789...             100Gi

# 3 PVs créés automatiquement (1 par Pod)
```

---

### Snapshot & Restore

**VolumeSnapshot (si supporté par CSI) :**

```yaml
apiVersion: snapshot.storage.k8s.io/v1
kind: VolumeSnapshot
metadata:
  name: postgres-snapshot
spec:
  volumeSnapshotClassName: csi-snapclass
  source:
    persistentVolumeClaimName: postgres-pvc
```

```bash
# Créer snapshot
kubectl apply -f snapshot.yaml

# Lister snapshots
kubectl get volumesnapshot
# NAME                READYTOUSE   SOURCEPVC      AGE
# postgres-snapshot   true         postgres-pvc   5m
```

**Restore depuis snapshot :**
```yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: postgres-pvc-restored
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 20Gi
  storageClassName: fast-ssd
  dataSource:
    name: postgres-snapshot
    kind: VolumeSnapshot
    apiGroup: snapshot.storage.k8s.io
```

---

### Backup Strategies

**1. Volume Snapshots (cloud provider)**
```bash
# AWS EBS snapshot
kubectl create -f ebs-snapshot.yaml
```

**2. Velero (Backup complet)**
```bash
# Backup namespace complet
velero backup create prod-backup --include-namespaces prod

# Restore
velero restore create --from-backup prod-backup
```

**3. Database-native backup**
```yaml
apiVersion: batch/v1
kind: CronJob
metadata:
  name: postgres-backup
spec:
  schedule: "0 2 * * *"  # 2h chaque jour
  jobTemplate:
    spec:
      template:
        spec:
          containers:
          - name: backup
            image: postgres:14
            command:
            - /bin/sh
            - -c
            - |
              pg_dump -h postgres -U admin mydb | \
              gzip > /backup/dump-$(date +%Y%m%d).sql.gz
              aws s3 cp /backup/dump-*.sql.gz s3://backups/postgres/
            volumeMounts:
            - name: backup
              mountPath: /backup
          volumes:
          - name: backup
            emptyDir: {}
```

---

## 📝 Best Practices - Storage

### PersistentVolumes

✅ **Utiliser Dynamic Provisioning**
```yaml
# Préférer StorageClass + PVC
# Plutôt que créer des PV manuellement
```

✅ **ReclaimPolicy appropriée**
```yaml
# Production : Retain (data safe)
persistentVolumeReclaimPolicy: Retain

# Dev/Test : Delete (cleanup auto)
persistentVolumeReclaimPolicy: Delete
```

✅ **AccessMode correct**
```yaml
# Base de données : ReadWriteOnce
# Shared files : ReadWriteMany (si supporté)
```

✅ **Backup régulier**
```yaml
# Snapshots automatiques
# Velero backups
# Database dumps
```

✅ **Monitoring**
```yaml
# Prometheus metrics:
# - kubelet_volume_stats_used_bytes
# - kubelet_volume_stats_capacity_bytes
```

---

### StorageClasses

✅ **Classes par use case**
```yaml
# fast-ssd : Databases
# standard-hdd : Logs, archives
# regional : High availability
```

✅ **allowVolumeExpansion activé**
```yaml
allowVolumeExpansion: true
```

✅ **volumeBindingMode intelligent**
```yaml
# WaitForFirstConsumer : Pod et PV sur même zone
volumeBindingMode: WaitForFirstConsumer
```

✅ **Encryption activée**
```yaml
parameters:
  encrypted: "true"
```

---

### StatefulSets

✅ **volumeClaimTemplates**
```yaml
# 1 PVC dédié par Pod
volumeClaimTemplates:
- metadata:
    name: data
  spec:
    accessModes: ["ReadWriteOnce"]
    resources:
      requests:
        storage: 100Gi
```

✅ **PodDisruptionBudget**
```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: postgres-pdb
spec:
  minAvailable: 2
  selector:
    matchLabels:
      app: postgres
```

---

## 🎯 Checklist Storage Production

```yaml
storage-checklist:
  storage-classes:
    ✓ StorageClass définie par use case
    ✓ allowVolumeExpansion: true
    ✓ volumeBindingMode: WaitForFirstConsumer
    ✓ Encryption activée
    ✓ Classe par défaut configurée
  
  persistent-volumes:
    ✓ Dynamic provisioning utilisé
    ✓ ReclaimPolicy: Retain (prod)
    ✓ AccessModes appropriés
    ✓ Capacity sizing correct
  
  persistent-volume-claims:
    ✓ StorageClassName spécifié
    ✓ Resources.requests défini
    ✓ AccessModes match le use case
  
  stateful-applications:
    ✓ StatefulSet avec volumeClaimTemplates
    ✓ Headless Service configuré
    ✓ PodDisruptionBudget défini
    ✓ Anti-affinity pour HA
  
  backup:
    ✓ Snapshots automatiques (CronJob)
    ✓ Velero installé
    ✓ Restore testé régulièrement
    ✓ RPO/RTO documentés
  
  monitoring:
    ✓ Volume usage tracked
    ✓ Alerts sur 80% capacity
    ✓ PVC Pending alerté
    ✓ IO performance monitored
```

---

# Chapitre 5 : Questions/Réponses d'Entretien - Storage

## 💾 Volumes

### Q1 : Quelle est la différence entre un Volume Kubernetes et un Volume Docker ?

**Réponse :**

**Docker Volume :**
- Lié au **container**
- Lifecycle : Container créé → Volume créé, Container supprimé → Volume peut persister
- Scope : 1 container

**Kubernetes Volume :**
- Lié au **Pod**
- Lifecycle : Pod créé → Volume créé, Pod supprimé → Volume supprimé (sauf PVC)
- Scope : Tous les containers du Pod (partagé)

**Exemple Kubernetes :**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: multi-container-pod
spec:
  containers:
  - name: writer
    image: busybox
    command: ['sh', '-c', 'echo "Hello" > /data/message.txt']
    volumeMounts:
    - name: shared-data
      mountPath: /data
  
  - name: reader
    image: busybox
    command: ['sh', '-c', 'cat /data/message.txt']
    volumeMounts:
    - name: shared-data
      mountPath: /data
  
  volumes:
  - name: shared-data
    emptyDir: {}  # Partagé entre les 2 containers
```

**Avantages Kubernetes :**
- Communication inter-containers via filesystem
- Sidecar patterns (logs, proxy, cache)
- Gestion déclarative

**Comparaison :**

| Aspect | Docker Volume | Kubernetes Volume |
|--------|---------------|-------------------|
| Scope | Container | Pod (multi-containers) |
| Lifecycle | Indépendant container | Lié au Pod |
| Types | Local, Named | emptyDir, PVC, ConfigMap, etc. |
| Partage | Non | Oui (intra-Pod) |

---

### Q2 : Quand utiliser emptyDir vs hostPath vs PersistentVolumeClaim ?

**Réponse :**

**emptyDir :**
```yaml
volumes:
- name: cache
  emptyDir: {}
```

**Caractéristiques :**
- Créé vide au démarrage du Pod
- Supprimé quand Pod est supprimé
- Stocké sur le disque du Node

**Use cases :**
✅ Cache temporaire
✅ Scratch space pour processing
✅ Logs temporaires
✅ Communication inter-containers

**Exemple - Cache applicatif :**
```java
@Configuration
public class CacheConfig {
    @Bean
    public CacheManager cacheManager() {
        // Cache écrit dans /cache (emptyDir)
        return new CaffeineCacheManager();
    }
}
```

---

**hostPath :**
```yaml
volumes:
- name: host-logs
  hostPath:
    path: /var/log/pods
    type: DirectoryOrCreate
```

**Caractéristiques :**
- Monte un chemin depuis le **Node**
- Données persistent même si Pod supprimé
- ⚠️ Dépend du Node spécifique

**Use cases :**
✅ DaemonSets (monitoring, logging)
✅ Accès à Docker socket (CI/CD)
✅ Development/testing

**⚠️ Problèmes :**
- Pas portable (Pod peut être schedulé sur autre Node)
- Risques de sécurité (accès au filesystem du Node)
- ❌ À éviter en production pour apps stateful

**Exemple - Node exporter (monitoring) :**
```yaml
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: node-exporter
spec:
  template:
    spec:
      containers:
      - name: exporter
        image: prom/node-exporter
        volumeMounts:
        - name: proc
          mountPath: /host/proc
          readOnly: true
      volumes:
      - name: proc
        hostPath:
          path: /proc  # Accès aux métriques système du Node
```

---

**PersistentVolumeClaim (PVC) :**
```yaml
volumes:
- name: data
  persistentVolumeClaim:
    claimName: postgres-pvc
```

**Caractéristiques :**
- Stockage persistent indépendant du Pod
- Survit aux restarts/redéploiements
- Portable (pas lié à un Node spécifique)
- Backups possibles

**Use cases :**
✅ Bases de données
✅ Uploads utilisateurs
✅ Données applicatives critiques
✅ Logs à long terme

**Exemple - PostgreSQL :**
```yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: postgres-pvc
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 50Gi
  storageClassName: fast-ssd
---
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: postgres
spec:
  template:
    spec:
      containers:
      - name: postgres
        image: postgres:14
        volumeMounts:
        - name: data
          mountPath: /var/lib/postgresql/data
      volumes:
      - name: data
        persistentVolumeClaim:
          claimName: postgres-pvc
```

---

**Résumé - Arbre de décision :**

```
Besoin de persistence ?
│
├─ NON → emptyDir
│   ├─ Besoin de RAM ? → emptyDir: {medium: Memory}
│   └─ Disque OK → emptyDir: {}
│
└─ OUI → Données doivent survivre au Pod ?
    │
    ├─ NON (Node-local OK) → hostPath
    │   └─ ⚠️ Seulement pour DaemonSets/dev
    │
    └─ OUI → PersistentVolumeClaim
        └─ ✅ Production-ready
```

---

## 📦 PersistentVolumes & PersistentVolumeClaims

### Q3 : Expliquez l'architecture PV/PVC et pourquoi cette séparation existe.

**Réponse :**

**Architecture :**

```
┌─────────────────────────────────────────────┐
│  ADMIN (Infrastructure/Ops)                 │
│                                             │
│  Provisionne PersistentVolume (PV)          │
│  - Gère le stockage physique                │
│  - Configure capacity, accessModes          │
│  - Définit reclaimPolicy                    │
└──────────────┬──────────────────────────────┘
               │
               │ Binding automatique
               ↓
┌──────────────┴──────────────────────────────┐
│  DEVELOPER (Application)                    │
│                                             │
│  Crée PersistentVolumeClaim (PVC)           │
│  - Demande du stockage (taille, accessMode) │
│  - Spécifie storageClassName                │
└──────────────┬──────────────────────────────┘
               │
               │ Mount
               ↓
┌──────────────┴──────────────────────────────┐
│  POD                                        │
│  volumes:                                   │
│    - persistentVolumeClaim:                 │
│        claimName: my-pvc                    │
└─────────────────────────────────────────────┘
```

**Pourquoi cette séparation ?**

**1. Séparation des responsabilités**
- **Admin** : Expertise infrastructure, gère le stockage physique
- **Developer** : Focus application, demande juste "50Gi de stockage"

**2. Abstraction**
```yaml
# Developer ne spécifie pas:
# - Quel disque physique
# - Quelle zone de disponibilité
# - Quel provider (AWS EBS, GCP PD, Azure Disk)
# Juste : "Je veux 50Gi ReadWriteOnce"
```

**3. Portabilité**
```yaml
# Même PVC fonctionne sur :
# - AWS (EBS)
# - GCP (Persistent Disk)
# - Azure (Managed Disk)
# - On-premise (NFS, Ceph)
```

**4. Sécurité**
```yaml
# Developer n'a pas besoin de permissions pour :
# - Créer des disques cloud
# - Accéder au stockage physique
# - Gérer les credentials du provider
```

---

**Exemple complet :**

**Admin (Ops team) :**
```yaml
# Crée PV manuellement OU configure StorageClass
apiVersion: v1
kind: PersistentVolume
metadata:
  name: pv-database
spec:
  capacity:
    storage: 100Gi
  accessModes:
  - ReadWriteOnce
  persistentVolumeReclaimPolicy: Retain
  storageClassName: fast-ssd
  awsElasticBlockStore:
    volumeID: vol-0abc123def456  # Disque AWS créé par admin
    fsType: ext4
```

**Developer (App team) :**
```yaml
# Demande du stockage via PVC
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: postgres-pvc
  namespace: prod
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 50Gi  # Demande 50Gi (PV a 100Gi)
  storageClassName: fast-ssd
```

**Kubernetes (automatique) :**
```bash
# Binding automatique
kubectl get pvc
# NAME           STATUS   VOLUME         CAPACITY   STORAGECLASS
# postgres-pvc   Bound    pv-database    100Gi      fast-ssd

# Le PVC obtient 100Gi (capacity du PV)
# même si demande était 50Gi
```

**Application :**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: postgres
spec:
  containers:
  - name: postgres
    image: postgres:14
    volumeMounts:
    - name: data
      mountPath: /var/lib/postgresql/data
  volumes:
  - name: data
    persistentVolumeClaim:
      claimName: postgres-pvc  # Référence simple
```

---

**Avantages :**

✅ **Developer velocity** : Pas besoin de connaître l'infrastructure
✅ **Sécurité** : Isolation des permissions
✅ **Portabilité** : Même code sur différents clouds
✅ **Scalabilité** : Dynamic provisioning avec StorageClasses

---

### Q4 : Que sont les AccessModes et comment les choisir ?

**Réponse :**

Les **AccessModes** définissent comment un volume peut être monté sur les Nodes.

**3 AccessModes :**

**1. ReadWriteOnce (RWO)**
```yaml
accessModes:
- ReadWriteOnce
```

**Signification :**
- Volume peut être monté en lecture/écriture par **1 seul Node**
- Multiple Pods OK si sur le **même Node**
- ❌ Pods sur différents Nodes ne peuvent pas monter le volume

**Use cases :**
- Bases de données (PostgreSQL, MySQL, MongoDB)
- Applications avec 1 replica
- Block storage (AWS EBS, GCP PD, Azure Disk)

**Exemple :**
```yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: postgres-pvc
spec:
  accessModes:
  - ReadWriteOnce  # 1 Node
  resources:
    requests:
      storage: 20Gi

# Deployment avec 1 replica OK
# StatefulSet avec N replicas OK (1 PVC par Pod)
# Deployment avec N replicas sur différents Nodes ❌
```

---

**2. ReadOnlyMany (ROX)**
```yaml
accessModes:
- ReadOnlyMany
```

**Signification :**
- Volume monté en **lecture seule** par **N Nodes**
- Aucun Pod ne peut écrire

**Use cases :**
- Configuration partagée
- Ressources statiques (images, videos)
- Data lakes en lecture seule

**Exemple :**
```yaml
# PVC ReadOnlyMany
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: shared-config-pvc
spec:
  accessModes:
  - ReadOnlyMany
  resources:
    requests:
      storage: 10Gi
---
# Multiple Pods (différents Nodes) peuvent lire
apiVersion: apps/v1
kind: Deployment
metadata:
  name: web
spec:
  replicas: 10  # Sur N Nodes
  template:
    spec:
      containers:
      - name: web
        image: nginx
        volumeMounts:
        - name: config
          mountPath: /etc/nginx/conf.d
          readOnly: true  # ← Important
      volumes:
      - name: config
        persistentVolumeClaim:
          claimName: shared-config-pvc
```

---

**3. ReadWriteMany (RWX)**
```yaml
accessModes:
- ReadWriteMany
```

**Signification :**
- Volume monté en **lecture/écriture** par **N Nodes**
- Multiple Pods sur différents Nodes peuvent lire/écrire simultanément

**Use cases :**
- Shared file storage (uploads utilisateurs)
- Logs agrégés
- Collaboration (documents partagés)

**⚠️ Limitations :**
- Nécessite un backend compatible (NFS, CephFS, GlusterFS)
- ❌ Pas supporté par AWS EBS, GCP PD, Azure Disk (block storage)
- Performance peut être moindre

**Exemple :**
```yaml
# StorageClass avec NFS (supporte RWX)
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: nfs-storage
provisioner: nfs.csi.k8s.io
---
# PVC RWX
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: shared-uploads-pvc
spec:
  accessModes:
  - ReadWriteMany  # N Nodes
  resources:
    requests:
      storage: 100Gi
  storageClassName: nfs-storage
---
# Deployment avec N replicas sur différents Nodes
apiVersion: apps/v1
kind: Deployment
metadata:
  name: file-processor
spec:
  replicas: 5
  template:
    spec:
      containers:
      - name: processor
        image: file-processor:1.0
        volumeMounts:
        - name: uploads
          mountPath: /uploads
      volumes:
      - name: uploads
        persistentVolumeClaim:
          claimName: shared-uploads-pvc
```

---

**Compatibilité par backend :**

| Storage Type | RWO | ROX | RWX |
|--------------|-----|-----|-----|
| AWS EBS | ✅ | ❌ | ❌ |
| GCP Persistent Disk | ✅ | ❌ | ❌ |
| Azure Disk | ✅ | ❌ | ❌ |
| AWS EFS | ✅ | ✅ | ✅ |
| GCP Filestore | ✅ | ✅ | ✅ |
| Azure Files | ✅ | ✅ | ✅ |
| NFS | ✅ | ✅ | ✅ |
| CephFS | ✅ | ✅ | ✅ |

---

**Comment choisir ?**

```
Application type ?
│
├─ Database (PostgreSQL, MySQL, MongoDB)
│   → ReadWriteOnce (RWO)
│   → 1 replica OU StatefulSet (1 PVC par Pod)
│
├─ Shared config (read-only)
│   → ReadOnlyMany (ROX)
│   → Multiple Pods lecteurs
│
├─ File sharing (uploads, logs)
│   → ReadWriteMany (RWX)
│   → Backend : NFS, CephFS, ou cloud file storage
│
└─ Unsure ?
    → Start with ReadWriteOnce (RWO)
    → Le plus compatible
```

---

### Q5 : Qu'est-ce que le ReclaimPolicy et quel impact a-t-il ?

**Réponse :**

Le **persistentVolumeReclaimPolicy** définit ce qui arrive au PersistentVolume quand le PVC est supprimé.

**3 Policies :**

---

**1. Retain (recommandé en production)**
```yaml
apiVersion: v1
kind: PersistentVolume
metadata:
  name: pv-prod
spec:
  persistentVolumeReclaimPolicy: Retain
  # ...
```

**Comportement :**
```bash
# 1. PVC utilisé
kubectl get pvc
# NAME     STATUS   VOLUME    CAPACITY
# my-pvc   Bound    pv-prod   50Gi

# 2. Supprimer PVC
kubectl delete pvc my-pvc

# 3. PV passe en "Released"
kubectl get pv
# NAME      CAPACITY   STATUS     CLAIM
# pv-prod   50Gi       Released   default/my-pvc

# 4. Données TOUJOURS présentes sur le disque
# Administrateur doit manuellement:
# - Vérifier les données
# - Nettoyer si nécessaire
# - Supprimer le PV
# - Ou le rendre disponible pour reuse
```

**Avantages :**
✅ **Data safety** : Impossible de perdre des données par accident
✅ **Audit** : Admin peut inspecter les données avant suppression
✅ **Recovery** : Possibilité de recréer PVC vers même PV

**Use case :**
- Production (databases critiques)
- Données sensibles
- Compliance requirements

**Exemple - Recovery :**
```bash
# PV en Released, data intact
kubectl get pv pv-prod -o yaml > pv-prod.yaml

# Éditer : retirer claimRef
kubectl edit pv pv-prod
# Supprimer la section claimRef complète

# PV redevient Available
kubectl get pv
# STATUS: Available

# Créer nouveau PVC
kubectl apply -f pvc-new.yaml
# Se bind au même PV → données récupérées
```

---

**2. Delete (défaut pour provisioning dynamique)**
```yaml
persistentVolumeReclaimPolicy: Delete
```

**Comportement :**
```bash
# 1. PVC supprimé
kubectl delete pvc my-pvc

# 2. PV supprimé automatiquement
kubectl get pv
# (PV n'existe plus)

# 3. Disque cloud supprimé (AWS EBS, GCP PD, etc.)
# ⚠️ DONNÉES PERDUES DÉFINITIVEMENT
```

**Avantages :**
✅ **Cleanup automatique** : Pas de PV orphelins
✅ **Cost savings** : Disques cloud supprimés → pas de coût
✅ **Dev/Test** : Pas besoin de cleanup manuel

**Inconvénients :**
❌ **Data loss risk** : Suppression accidentelle = perte définitive
❌ **No recovery** : Impossible de récupérer après suppression

**Use case :**
- Dev/Test environments
- Données non critiques
- Caches temporaires

---

**3. Recycle (déprécié)**
```yaml
persistentVolumeReclaimPolicy: Recycle
```

**Comportement :**
- Exécute `rm -rf /volume/*` (efface les données)
- PV redevient Available pour reuse

**⚠️ Déprécié** : Ne pas utiliser, sera retiré.

---

**Comparaison :**

| Policy | Data après delete PVC | PV status | Use case |
|--------|----------------------|-----------|----------|
| **Retain** | ✅ Intact | Released | Production |
| **Delete** | ❌ Supprimée | Deleted | Dev/Test |
| **Recycle** | ❌ Effacée | Available | Déprécié |

---

**Configuration par StorageClass :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: prod-storage
provisioner: ebs.csi.aws.com
reclaimPolicy: Retain  # ← Policy par défaut pour PVs créés dynamiquement
parameters:
  type: gp3
```

**Changer la policy d'un PV existant :**
```bash
# Voir la policy actuelle
kubectl get pv pv-name -o jsonpath='{.spec.persistentVolumeReclaimPolicy}'
# Delete

# Changer en Retain
kubectl patch pv pv-name -p '{"spec":{"persistentVolumeReclaimPolicy":"Retain"}}'

# Vérifier
kubectl get pv pv-name
# RECLAIM POLICY: Retain
```

---

**Best practice : Toujours Retain en production**
```yaml
# Dans votre StorageClass production
reclaimPolicy: Retain

# Si besoin de supprimer :
# 1. Backup des données
# 2. kubectl delete pvc (PV → Released)
# 3. Vérifier les données
# 4. kubectl delete pv (manuel)
# 5. Supprimer le disque cloud (manuel si nécessaire)
```

---

## 🗄️ StorageClasses & Dynamic Provisioning

### Q6 : Qu'est-ce qu'une StorageClass et comment fonctionne le provisioning dynamique ?

**Réponse :**

Une **StorageClass** définit les "classes" de stockage disponibles et permet le **provisioning dynamique** de PersistentVolumes.

**Sans StorageClass (Static Provisioning) :**
```
1. Admin crée PV manuellement
2. Developer crée PVC
3. Kubernetes fait le binding
4. Limitations : Admin doit pré-créer tous les PVs
```

**Avec StorageClass (Dynamic Provisioning) :**
```
1. Developer crée PVC (spécifie storageClassName)
2. StorageClass provisionne PV automatiquement
3. Kubernetes fait le binding
4. Avantages : On-demand, scalable, automatisé
```

---

**Exemple complet :**

**1. Admin crée StorageClass :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: fast-ssd
provisioner: ebs.csi.aws.com  # AWS EBS CSI driver
parameters:
  type: gp3            # GP3 SSD
  iops: "3000"         # 3000 IOPS
  throughput: "125"    # 125 MB/s
  encrypted: "true"    # Encryption activée
allowVolumeExpansion: true
reclaimPolicy: Retain
volumeBindingMode: WaitForFirstConsumer
```

**2. Developer crée PVC (pas de PV manuel) :**
```yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: postgres-pvc
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 50Gi
  storageClassName: fast-ssd  # ← Référence à la StorageClass
```

**3. Provisioning automatique :**
```bash
# PVC créé
kubectl apply -f pvc.yaml

# StorageClass provisionne automatiquement :
# - Crée un disque AWS EBS (gp3, 50Gi, encrypted)
# - Crée un PV Kubernetes
# - Bind le PVC au PV

kubectl get pvc
# NAME           STATUS   VOLUME                                     CAPACITY
# postgres-pvc   Bound    pvc-abc123-def456-ghi789-jkl012           50Gi

kubectl get pv
# NAME                              CAPACITY   STORAGECLASS   STATUS   CLAIM
# pvc-abc123-def456-ghi789-jkl012   50Gi       fast-ssd       Bound    default/postgres-pvc
```

**4. Utilisation dans le Pod :**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: postgres
spec:
  containers:
  - name: postgres
    image: postgres:14
    volumeMounts:
    - name: data
      mountPath: /var/lib/postgresql/data
  volumes:
  - name: data
    persistentVolumeClaim:
      claimName: postgres-pvc
```

---

**Provisioners populaires :**

| Cloud Provider | Provisioner | Storage Type |
|---------------|-------------|--------------|
| AWS | `ebs.csi.aws.com` | EBS (block) |
| AWS | `efs.csi.aws.com` | EFS (file) |
| GCP | `pd.csi.storage.gke.io` | Persistent Disk |
| Azure | `disk.csi.azure.com` | Managed Disk |
| On-premise | `nfs.csi.k8s.io` | NFS |
| On-premise | `rbd.csi.ceph.com` | Ceph RBD |

---

**volumeBindingMode :**

**1. Immediate (défaut) :**
```yaml
volumeBindingMode: Immediate
```
- PV créé dès que PVC est créé
- Problème : PV peut être dans zone A, Pod schedulé en zone B

**2. WaitForFirstConsumer (recommandé) :**
```yaml
volumeBindingMode: WaitForFirstConsumer
```
- PV créé quand Pod est schedulé
- PV créé dans la même zone que le Pod
- ✅ Meilleure option pour multi-AZ

**Exemple :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: regional-storage
provisioner: ebs.csi.aws.com
volumeBindingMode: WaitForFirstConsumer  # ← Attend le Pod
parameters:
  type: gp3
---
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: app-pvc
spec:
  storageClassName: regional-storage
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 10Gi
---
apiVersion: v1
kind: Pod
metadata:
  name: app
spec:
  # Pod schedulé en zone us-east-1a
  nodeSelector:
    topology.kubernetes.io/zone: us-east-1a
  volumes:
  - name: data
    persistentVolumeClaim:
      claimName: app-pvc
  # PV sera créé en us-east-1a (même zone)
```

---

**allowVolumeExpansion :**
```yaml
allowVolumeExpansion: true
```

Permet d'agrandir un PVC existant :
```bash
# PVC initial : 10Gi
kubectl get pvc my-pvc
# CAPACITY: 10Gi

# Éditer le PVC
kubectl edit pvc my-pvc
# storage: 10Gi → storage: 20Gi

# Expansion automatique
kubectl get pvc my-pvc
# CAPACITY: 20Gi

# ⚠️ Note : Réduction impossible (20Gi → 10Gi ❌)
```

---

**StorageClass par défaut :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: standard
  annotations:
    storageclass.kubernetes.io/is-default-class: "true"
provisioner: ebs.csi.aws.com
```

```yaml
# PVC sans storageClassName
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: auto-pvc
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 5Gi
  # storageClassName non spécifié → utilise "standard" (défaut)
```

---

**Avantages Dynamic Provisioning :**

✅ **On-demand** : Disques créés quand nécessaires
✅ **Scalable** : Pas de limite pré-définie
✅ **Automatisé** : Pas d'intervention manuelle
✅ **Self-service** : Developers autonomes
✅ **Cloud-agnostic** : Même code sur AWS/GCP/Azure

---

### Q7 : Comment déployez-vous une base de données stateful (PostgreSQL) avec storage persistent ?

**Réponse :**

**Architecture recommandée : StatefulSet + volumeClaimTemplates**

```yaml
# 1. StorageClass
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: fast-ssd
provisioner: ebs.csi.aws.com
parameters:
  type: gp3
  iops: "3000"
  encrypted: "true"
allowVolumeExpansion: true
volumeBindingMode: WaitForFirstConsumer
reclaimPolicy: Retain  # ← Important : data safety
---
# 2. Secret pour credentials
apiVersion: v1
kind: Secret
metadata:
  name: postgres-secret
type: Opaque
stringData:
  password: "SuperSecretPassword123!"
---
# 3. Headless Service
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
    targetPort: 5432
---
# 4. StatefulSet
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: postgres
spec:
  serviceName: postgres  # Lié au Headless Service
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
          valueFrom:
            secretKeyRef:
              name: postgres-secret
              key: password
        - name: PGDATA
          value: /var/lib/postgresql/data/pgdata
        volumeMounts:
        - name: postgres-data
          mountPath: /var/lib/postgresql/data
        resources:
          requests:
            memory: "2Gi"
            cpu: "1000m"
          limits:
            memory: "4Gi"
            cpu: "2000m"
        livenessProbe:
          exec:
            command:
            - pg_isready
            - -U
            - postgres
          initialDelaySeconds: 30
          periodSeconds: 10
        readinessProbe:
          exec:
            command:
            - pg_isready
            - -U
            - postgres
          initialDelaySeconds: 5
          periodSeconds: 5
  
  # volumeClaimTemplates : 1 PVC par Pod
  volumeClaimTemplates:
  - metadata:
      name: postgres-data
    spec:
      accessModes: ["ReadWriteOnce"]
      storageClassName: fast-ssd
      resources:
        requests:
          storage: 100Gi
```

**Résultat :**
```bash
# StatefulSet crée 3 Pods séquentiellement
kubectl get pods
# NAME         READY   STATUS    RESTARTS   AGE
# postgres-0   1/1     Running   0          5m
# postgres-1   1/1     Running   0          4m
# postgres-2   1/1     Running   0          3m

# 3 PVCs créés automatiquement (1 par Pod)
kubectl get pvc
# NAME                        STATUS   VOLUME                   CAPACITY
# postgres-data-postgres-0    Bound    pvc-abc123...            100Gi
# postgres-data-postgres-1    Bound    pvc-def456...            100Gi
# postgres-data-postgres-2    Bound    pvc-ghi789...            100Gi

# 3 PVs créés dynamiquement
kubectl get pv
# NAME           CAPACITY   STORAGECLASS   STATUS   CLAIM
# pvc-abc123...  100Gi      fast-ssd       Bound    default/postgres-data-postgres-0
# pvc-def456...  100Gi      fast-ssd       Bound    default/postgres-data-postgres-1
# pvc-ghi789...  100Gi      fast-ssd       Bound    default/postgres-data-postgres-2
```

---

**DNS stable pour chaque Pod :**
```bash
# Accès au master (postgres-0)
psql -h postgres-0.postgres.default.svc.cluster.local -U postgres

# Accès aux replicas
psql -h postgres-1.postgres.default.svc.cluster.local -U postgres
psql -h postgres-2.postgres.default.svc.cluster.local -U postgres
```

---

**Application Spring Boot :**
```java
@Configuration
public class DataSourceConfig {
    
    @Bean
    public DataSource dataSource() {
        HikariConfig config = new HikariConfig();
        
        // Master pour écriture
        config.setJdbcUrl("jdbc:postgresql://postgres-0.postgres:5432/mydb");
        config.setUsername("postgres");
        config.setPassword(System.getenv("DB_PASSWORD"));
        
        // Pool settings
        config.setMaximumPoolSize(20);
        config.setMinimumIdle(5);
        
        return new HikariDataSource(config);
    }
    
    // Replica pour lecture (optionnel)
    @Bean
    public DataSource readReplicaDataSource() {
        HikariConfig config = new HikariConfig();
        config.setJdbcUrl("jdbc:postgresql://postgres-1.postgres:5432/mydb");
        config.setUsername("postgres");
        config.setPassword(System.getenv("DB_PASSWORD"));
        config.setReadOnly(true);
        return new HikariDataSource(config);
    }
}
```

---

**Backup Strategy :**

**1. Volume Snapshots (si supporté) :**
```yaml
apiVersion: snapshot.storage.k8s.io/v1
kind: VolumeSnapshot
metadata:
  name: postgres-backup-daily
spec:
  volumeSnapshotClassName: csi-snapclass
  source:
    persistentVolumeClaimName: postgres-data-postgres-0
```

**2. CronJob pour pg_dump :**
```yaml
apiVersion: batch/v1
kind: CronJob
metadata:
  name: postgres-backup
spec:
  schedule: "0 2 * * *"  # 2h chaque jour
  jobTemplate:
    spec:
      template:
        spec:
          containers:
          - name: backup
            image: postgres:14
            command:
            - /bin/sh
            - -c
            - |
              TIMESTAMP=$(date +%Y%m%d-%H%M%S)
              pg_dump -h postgres-0.postgres -U postgres mydb | \
              gzip > /backup/dump-${TIMESTAMP}.sql.gz
              aws s3 cp /backup/dump-${TIMESTAMP}.sql.gz s3://backups/postgres/
            env:
            - name: PGPASSWORD
              valueFrom:
                secretKeyRef:
                  name: postgres-secret
                  key: password
            volumeMounts:
            - name: backup
              mountPath: /backup
          volumes:
          - name: backup
            emptyDir: {}
          restartPolicy: OnFailure
```

---

**Scale et maintenance :**
```bash
# Scale up (ajout d'un replica)
kubectl scale statefulset postgres --replicas=4
# postgres-3 créé avec son PVC dédié

# Scale down (attention !)
kubectl scale statefulset postgres --replicas=2
# postgres-2 supprimé mais PVC conservé (reclaimPolicy: Retain)

# Rolling update
kubectl set image statefulset/postgres postgres=postgres:15
# Update séquentiel : postgres-2 → postgres-1 → postgres-0
```

---

**Best Practices :**

✅ **StatefulSet** (pas Deployment)
- Identités stables (postgres-0, postgres-1, ...)
- volumeClaimTemplates (1 PVC par Pod)
- Ordre de démarrage séquentiel

✅ **Headless Service**
- DNS par Pod pour connexions directes

✅ **ReclaimPolicy: Retain**
- Data safety en production

✅ **Resources requests/limits**
- Performance prévisible

✅ **Health checks**
- livenessProbe et readinessProbe

✅ **Backups réguliers**
- Snapshots + pg_dump

✅ **PodDisruptionBudget**
```yaml
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: postgres-pdb
spec:
  minAvailable: 2
  selector:
    matchLabels:
      app: postgres
```

---

## 🎯 Questions Situationnelles

### Q8 : Un PVC reste en Pending. Comment debugger ?

**Réponse :**

**Symptoms :**
```bash
kubectl get pvc
# NAME      STATUS    VOLUME   CAPACITY   STORAGECLASS
# my-pvc    Pending   -        -          fast-ssd
```

**Checklist de debugging :**

---

**1. Describe le PVC pour voir les events**
```bash
kubectl describe pvc my-pvc

# Events à chercher:
# - "waiting for first consumer to be created"
# - "no persistent volumes available"
# - "storageclass not found"
# - "failed to provision volume"
```

---

**2. Vérifier la StorageClass**
```bash
# StorageClass existe ?
kubectl get storageclass fast-ssd
# Error: storageclass "fast-ssd" not found ❌

# Solution : Créer ou corriger le nom
kubectl get storageclass
# Lister les StorageClasses disponibles
```

**Fix :**
```yaml
# Corriger le PVC
spec:
  storageClassName: standard  # Nom correct
```

---

**3. Vérifier le provisioner**
```bash
kubectl get storageclass fast-ssd -o yaml

# Vérifier:
provisioner: ebs.csi.aws.com  # Doit être installé

# Vérifier que le CSI driver tourne
kubectl get pods -n kube-system | grep ebs-csi

# Si absent :
helm install aws-ebs-csi-driver aws-ebs-csi-driver/aws-ebs-csi-driver \
  -n kube-system
```

---

**4. volumeBindingMode: WaitForFirstConsumer**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: fast-ssd
volumeBindingMode: WaitForFirstConsumer  # ← Attend le Pod
```

**Comportement :**
```bash
# PVC reste Pending jusqu'à ce qu'un Pod l'utilise
kubectl get pvc
# STATUS: Pending

# Créer le Pod
kubectl apply -f pod.yaml

# PVC devient Bound
kubectl get pvc
# STATUS: Bound
```

**C'est normal !** Le PVC attend le Pod pour créer le PV dans la bonne zone.

---

**5. Pas de PV disponible (static provisioning)**
```bash
kubectl get pv
# No resources found

# Si pas de dynamic provisioning, admin doit créer PV
```

**Fix :**
```yaml
# Admin crée PV
apiVersion: v1
kind: PersistentVolume
metadata:
  name: manual-pv
spec:
  capacity:
    storage: 10Gi
  accessModes:
  - ReadWriteOnce
  storageClassName: fast-ssd  # Match le PVC
  hostPath:
    path: /mnt/data
```

---

**6. Quota de ressources atteint**
```bash
kubectl describe namespace default | grep -A 5 "Resource Quotas"
# persistentvolumeclaims: 10/10  ← Quota atteint

# Solution : Augmenter quota ou nettoyer PVCs inutilisés
kubectl delete pvc old-pvc-1 old-pvc-2
```

---

**7. Permissions cloud provider**
```bash
# AWS : Le cluster a-t-il les permissions pour créer des EBS ?
# Vérifier IAM role du cluster
# Nécessite: ec2:CreateVolume, ec2:AttachVolume, ec2:DeleteVolume

# Logs du CSI driver
kubectl logs -n kube-system ebs-csi-controller-xxx

# Erreurs typiques:
# - "Access Denied"
# - "Insufficient permissions"
```

---

**8. Zone de disponibilité**
```bash
# Erreur: "Volume in zone us-east-1a, pod in us-east-1b"

# Solution : volumeBindingMode: WaitForFirstConsumer
# OU spécifier la zone dans le Pod
nodeSelector:
  topology.kubernetes.io/zone: us-east-1a
```

---

**Script de debug complet :**
```bash
#!/bin/bash
# debug-pvc.sh

PVC_NAME=$1
NAMESPACE=${2:-default}

echo "=== PVC Status ==="
kubectl get pvc $PVC_NAME -n $NAMESPACE

echo "\n=== PVC Events ==="
kubectl describe pvc $PVC_NAME -n $NAMESPACE | grep -A 10 Events

echo "\n=== StorageClass ==="
SC=$(kubectl get pvc $PVC_NAME -n $NAMESPACE -o jsonpath='{.spec.storageClassName}')
kubectl get storageclass $SC 2>&1

echo "\n=== Available PVs ==="
kubectl get pv | grep Available

echo "\n=== CSI Driver Pods ==="
kubectl get pods -n kube-system | grep -E "csi|provisioner"

echo "\n=== Pods using this PVC ==="
kubectl get pods -n $NAMESPACE -o json | \
  jq -r ".items[] | select(.spec.volumes[]?.persistentVolumeClaim.claimName==\"$PVC_NAME\") | .metadata.name"
```

**Usage :**
```bash
./debug-pvc.sh my-pvc default
```

---

### Q9 : Comment migrez-vous des données d'un PVC vers un autre ?

**Réponse :**

**Scénario : Migrer de 50Gi à 100Gi avec changement de StorageClass**

**Méthode 1 : Pod temporaire (simple)**

```bash
# 1. Créer nouveau PVC
cat <<EOF | kubectl apply -f -
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: new-pvc
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 100Gi
  storageClassName: fast-ssd
EOF

# 2. Créer Pod de migration
cat <<EOF | kubectl apply -f -
apiVersion: v1
kind: Pod
metadata:
  name: data-migration
spec:
  containers:
  - name: migrator
    image: busybox
    command: ['sh', '-c', 'cp -av /old/* /new/ && echo "Migration completed"']
    volumeMounts:
    - name: old-data
      mountPath: /old
      readOnly: true
    - name: new-data
      mountPath: /new
  volumes:
  - name: old-data
    persistentVolumeClaim:
      claimName: old-pvc
  - name: new-data
    persistentVolumeClaim:
      claimName: new-pvc
  restartPolicy: Never
EOF

# 3. Attendre la migration
kubectl wait --for=condition=complete pod/data-migration --timeout=600s

# 4. Vérifier
kubectl logs data-migration

# 5. Mettre à jour l'application pour utiliser new-pvc
kubectl set volume deployment/app \
  --add --name=data --type=pvc --claim-name=new-pvc --overwrite

# 6. Supprimer ancien PVC (après validation)
kubectl delete pvc old-pvc
```

---

**Méthode 2 : rsync (pour gros volumes)**

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: data-migration-rsync
spec:
  containers:
  - name: rsync
    image: instrumentisto/rsync-ssh
    command:
    - /bin/sh
    - -c
    - |
      rsync -avh --progress /old/ /new/
      echo "Migration completed"
      sleep infinity
    volumeMounts:
    - name: old-data
      mountPath: /old
      readOnly: true
    - name: new-data
      mountPath: /new
  volumes:
  - name: old-data
    persistentVolumeClaim:
      claimName: old-pvc
  - name: new-data
    persistentVolumeClaim:
      claimName: new-pvc
```

**Monitoring :**
```bash
# Suivre la progression
kubectl exec -it data-migration-rsync -- sh
du -sh /old /new
# Compare sizes
```

---

**Méthode 3 : Snapshot & Clone (si supporté)**

```yaml
# 1. Créer snapshot du PVC source
apiVersion: snapshot.storage.k8s.io/v1
kind: VolumeSnapshot
metadata:
  name: old-pvc-snapshot
spec:
  volumeSnapshotClassName: csi-snapclass
  source:
    persistentVolumeClaimName: old-pvc
---
# 2. Créer nouveau PVC depuis snapshot
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: new-pvc
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 100Gi  # Peut être plus grand que source
  storageClassName: fast-ssd
  dataSource:
    name: old-pvc-snapshot
    kind: VolumeSnapshot
    apiGroup: snapshot.storage.k8s.io
```

**Avantages :**
- ✅ Très rapide (snapshot instantané)
- ✅ Pas de downtime si bien orchestré
- ✅ Pas besoin de Pod temporaire

---

**Méthode 4 : Blue/Green avec données**

```yaml
# 1. Nouveau Deployment avec new-pvc
apiVersion: apps/v1
kind: Deployment
metadata:
  name: app-green
spec:
  replicas: 1
  template:
    spec:
      containers:
      - name: app
        image: myapp:1.0
        volumeMounts:
        - name: data
          mountPath: /data
      volumes:
      - name: data
        persistentVolumeClaim:
          claimName: new-pvc

# 2. Migration de données (voir méthodes ci-dessus)

# 3. Switch traffic (Service selector)
kubectl patch service myapp -p \
  '{"spec":{"selector":{"version":"green"}}}'

# 4. Cleanup ancien Deployment
kubectl delete deployment app-blue
kubectl delete pvc old-pvc
```

---

**Best Practices Migration :**

✅ **Backup avant migration**
```bash
# Snapshot ou backup
velero backup create pre-migration-backup
```

✅ **Tester sur un subset de données**
```bash
# Créer petit PVC test
# Migrer 1GB pour valider le process
```

✅ **Planifier downtime si nécessaire**
```bash
# Scale down pendant migration
kubectl scale deployment app --replicas=0
# Migrer
# Scale up
kubectl scale deployment app --replicas=3
```

✅ **Validation post-migration**
```bash
# Comparer checksums
kubectl exec migration-pod -- md5sum /old/file
kubectl exec migration-pod -- md5sum /new/file

# Vérifier size
kubectl exec migration-pod -- du -sh /old /new
```

✅ **Garder ancien PVC jusqu'à validation complète**
```bash
# Ne supprimer old-pvc qu'après plusieurs jours en prod
```

---

### Q10 : Comment optimisez-vous les performances de stockage dans Kubernetes ?

**Réponse :**

**1. Choisir le bon type de stockage**

**Use case Database :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: db-storage
provisioner: ebs.csi.aws.com
parameters:
  type: io2  # Provisioned IOPS (meilleur perf)
  iops: "10000"  # 10k IOPS
  throughput: "1000"  # 1 GB/s
```

**Use case Logs/Archives :**
```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: archive-storage
provisioner: ebs.csi.aws.com
parameters:
  type: st1  # Throughput Optimized (moins cher)
```

**Comparaison AWS EBS :**

| Type | IOPS | Throughput | Cost | Use case |
|------|------|------------|------|----------|
| gp3 | 3k-16k | 125-1000 MB/s | $ | General |
| io2 | Up to 64k | Up to 1000 MB/s | $$ | Databases |
| st1 | 500 | 500 MB/s | $ | Logs |

---

**2. I/O Tuning au niveau Pod**

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: postgres
spec:
  containers:
  - name: postgres
    image: postgres:14
    volumeMounts:
    - name: data
      mountPath: /var/lib/postgresql/data
      # Mount options pour performance
      mountOptions:
      - noatime  # Pas de update access time
      - nodiratime
    
    resources:
      requests:
        memory: "4Gi"
        cpu: "2000m"
      limits:
        memory: "8Gi"
        cpu: "4000m"
    
    # Postgres tuning
    env:
    - name: POSTGRES_INITDB_ARGS
      value: "--data-checksums"
    command:
    - postgres
    - -c
    - shared_buffers=2GB  # 25% de la RAM
    - -c
    - effective_cache_size=6GB  # 75% de la RAM
    - -c
    - maintenance_work_mem=512MB
    - -c
    - wal_buffers=16MB
    - -c
    - checkpoint_completion_target=0.9
    - -c
    - random_page_cost=1.1  # SSD
```

---

**3. Local Storage pour performance maximale**

```yaml
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: local-nvme
provisioner: kubernetes.io/no-provisioner
volumeBindingMode: WaitForFirstConsumer
---
apiVersion: v1
kind: PersistentVolume
metadata:
  name: local-pv-nvme
spec:
  capacity:
    storage: 1Ti
  accessModes:
  - ReadWriteOnce
  persistentVolumeReclaimPolicy: Retain
  storageClassName: local-nvme
  local:
    path: /mnt/nvme0n1  # NVMe SSD local
  nodeAffinity:
    required:
      nodeSelectorTerms:
      - matchExpressions:
        - key: kubernetes.io/hostname
          operator: In
          values:
          - worker-1  # Node avec NVMe
```

**Performance :**
- Latency : < 1ms (vs 3-5ms pour EBS)
- IOPS : > 100k (vs 16k max pour EBS)
- **Inconvénient** : Pas de réplication, lié au Node

**Use case :** Caches, databases temporaires, très haute performance

---

**4. Monitoring et alerting**

**Prometheus metrics :**
```yaml
# Volume usage
kubelet_volume_stats_used_bytes / kubelet_volume_stats_capacity_bytes > 0.8

# I/O wait
rate(container_cpu_usage_seconds_total{mode="wait"}[5m]) > 0.1

# Disk latency (si exporté)
disk_io_time_seconds > 0.05
```

**Grafana dashboard :**
```yaml
# Panel : Volume Usage
100 * (kubelet_volume_stats_used_bytes / kubelet_volume_stats_capacity_bytes)

# Panel : I/O Operations
rate(container_fs_reads_total[5m])
rate(container_fs_writes_total[5m])

# Panel : Disk Throughput
rate(container_fs_read_bytes_total[5m])
rate(container_fs_write_bytes_total[5m])
```

---

**5. Caching strategies**

**Application-level cache (Redis) :**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: redis-cache
spec:
  template:
    spec:
      containers:
      - name: redis
        image: redis:7
        volumeMounts:
        - name: redis-data
          mountPath: /data
      volumes:
      - name: redis-data
        emptyDir:
          medium: Memory  # RAM disk (très rapide)
          sizeLimit: 4Gi
```

**Read-through cache :**
```java
@Service
public class DataService {
    
    @Cacheable(value = "users", key = "#id")
    public User getUser(Long id) {
        // Hit cache d'abord (Redis/Caffeine)
        // Si miss, query database
        return userRepository.findById(id).orElse(null);
    }
}
```

---

**6. Batch operations**

```java
// ❌ Mauvais : 1000 I/O operations
for (User user : users) {
    userRepository.save(user);
}

// ✅ Bon : 1 I/O operation
userRepository.saveAll(users);
```

---

**7. Volume type approprié**

```yaml
# Pour workload read-heavy
storageClassName: gp3  # Balanced

# Pour workload write-heavy (logs)
storageClassName: throughput-optimized

# Pour database OLTP
storageClassName: provisioned-iops
```

---

**Checklist Performance :**

```yaml
storage-performance-checklist:
  storage-class:
    ✓ Type approprié au workload
    ✓ IOPS/throughput configurés
    ✓ volumeBindingMode: WaitForFirstConsumer
  
  application:
    ✓ Batch operations
    ✓ Caching strategy
    ✓ Connection pooling
    ✓ Async I/O si applicable
  
  pod-configuration:
    ✓ Resources requests/limits
    ✓ Mount options (noatime)
    ✓ Application tuning (DB params)
  
  monitoring:
    ✓ Volume usage alerts (80%)
    ✓ I/O latency tracking
    ✓ IOPS monitoring
    ✓ Throughput metrics
  
  architecture:
    ✓ Read replicas (databases)
    ✓ Sharding si applicable
    ✓ Local storage pour hot data
```

---

## 🎓 Conseils pour l'Entretien

### Points clés à maîtriser :
1. **Volumes** : emptyDir, hostPath, PVC - use cases
2. **PV/PVC** : Architecture, binding, accessModes
3. **StorageClasses** : Dynamic provisioning, provisioners
4. **StatefulSets** : volumeClaimTemplates, databases
5. **Debugging** : PVC Pending, migration, performance

### Red flags à éviter :
- ❌ hostPath en production pour apps stateful
- ❌ Ne pas comprendre ReclaimPolicy
- ❌ Ignorer volumeBindingMode pour multi-AZ
- ❌ Pas de backup strategy
- ❌ ReclaimPolicy: Delete en production

### Points bonus :
- ✅ Volume Snapshots pour backups
- ✅ Velero pour disaster recovery
- ✅ CSI drivers (AWS EBS, GCP PD)
- ✅ Performance tuning (IOPS, throughput)
- ✅ Cost optimization (storage types)

---