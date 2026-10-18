## [**..**](./00_index.md)


# Chapitre 3 : Configuration & Secrets Management

## 3.1. ConfigMaps

### Qu'est-ce qu'un ConfigMap ?

Un **ConfigMap** permet de stocker des données de configuration sous forme de paires clé-valeur, découplant ainsi la configuration du code applicatif.

**Avantages :**
- Configuration externalisée
- Pas besoin de rebuild l'image pour changer la config
- Versioning et rollback possibles
- Partage entre plusieurs Pods

---

### Créer des ConfigMaps

#### **Méthode 1 : Depuis des literals**

```bash
kubectl create configmap app-config \
  --from-literal=database.host=postgres.prod.svc.cluster.local \
  --from-literal=database.port=5432 \
  --from-literal=log.level=INFO \
  --from-literal=app.name="My Spring App"
```

#### **Méthode 2 : Depuis un fichier**

```bash
# application.properties
database.host=postgres.prod.svc.cluster.local
database.port=5432
log.level=INFO
feature.flag.new-ui=true

# Créer le ConfigMap
kubectl create configmap app-config --from-file=application.properties
```

#### **Méthode 3 : Depuis un répertoire**

```bash
# Structure
config/
  ├── application.properties
  ├── logback.xml
  └── features.json

# Créer un ConfigMap avec tous les fichiers
kubectl create configmap app-config --from-file=config/
```

#### **Méthode 4 : Déclaratif (YAML)**

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: app-config
  namespace: default
  labels:
    app: spring-app
data:
  # Données simples (clé-valeur)
  database.host: "postgres.prod.svc.cluster.local"
  database.port: "5432"
  log.level: "INFO"
  
  # Fichier complet
  application.properties: |
    server.port=8080
    spring.datasource.url=jdbc:postgresql://postgres:5432/mydb
    spring.datasource.username=appuser
    spring.jpa.hibernate.ddl-auto=validate
    logging.level.root=INFO
    logging.level.com.myapp=DEBUG
  
  # Configuration JSON
  features.json: |
    {
      "newUI": true,
      "betaFeatures": false,
      "maxUploadSize": 10485760
    }
  
  # Configuration XML
  logback.xml: |
    <?xml version="1.0" encoding="UTF-8"?>
    <configuration>
      <appender name="STDOUT" class="ch.qos.logback.core.ConsoleAppender">
        <encoder>
          <pattern>%d{HH:mm:ss.SSS} [%thread] %-5level %logger{36} - %msg%n</pattern>
        </encoder>
      </appender>
      <root level="INFO">
        <appender-ref ref="STDOUT" />
      </root>
    </configuration>
```

```bash
kubectl apply -f configmap.yaml

# Vérifier
kubectl get configmap app-config
kubectl describe configmap app-config
kubectl get configmap app-config -o yaml
```

---

### Utiliser des ConfigMaps dans les Pods

#### **1. Variables d'environnement (clé individuelle)**

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: spring-app
spec:
  containers:
  - name: app
    image: myapp/spring-boot:1.0
    env:
    # Référencer une clé spécifique
    - name: DATABASE_HOST
      valueFrom:
        configMapKeyRef:
          name: app-config
          key: database.host
    
    - name: DATABASE_PORT
      valueFrom:
        configMapKeyRef:
          name: app-config
          key: database.port
    
    - name: LOG_LEVEL
      valueFrom:
        configMapKeyRef:
          name: app-config
          key: log.level
```

#### **2. Toutes les clés comme variables d'environnement**

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: spring-app
spec:
  containers:
  - name: app
    image: myapp/spring-boot:1.0
    envFrom:
    # Toutes les clés du ConfigMap deviennent des variables d'env
    - configMapRef:
        name: app-config
    # Résultat :
    # DATABASE_HOST=postgres.prod.svc.cluster.local
    # DATABASE_PORT=5432
    # LOG_LEVEL=INFO
```

**Avec préfixe :**
```yaml
envFrom:
- configMapRef:
    name: app-config
  prefix: CONFIG_  # Toutes les vars auront le préfixe CONFIG_
# Résultat :
# CONFIG_DATABASE_HOST=...
# CONFIG_DATABASE_PORT=...
```

#### **3. Monter comme volume (fichier)**

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: spring-app
spec:
  containers:
  - name: app
    image: myapp/spring-boot:1.0
    volumeMounts:
    - name: config-volume
      mountPath: /config  # Répertoire où les fichiers seront montés
      readOnly: true
    
    # L'application peut lire /config/application.properties
    env:
    - name: SPRING_CONFIG_LOCATION
      value: "file:/config/application.properties"
  
  volumes:
  - name: config-volume
    configMap:
      name: app-config
      items:  # Optionnel : sélectionner des clés spécifiques
      - key: application.properties
        path: application.properties  # Nom du fichier dans le volume
      - key: logback.xml
        path: logback.xml
```

**Résultat dans le container :**
```bash
ls -la /config/
# application.properties
# logback.xml

cat /config/application.properties
# server.port=8080
# spring.datasource.url=...
```

#### **4. Monter une clé spécifique avec un nom de fichier custom**

```yaml
volumes:
- name: config-volume
  configMap:
    name: app-config
    items:
    - key: application.properties
      path: app.properties  # Renommé
      mode: 0644  # Permissions
```

---

### Exemple Complet : Spring Boot avec ConfigMap

```yaml
# configmap.yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: spring-app-config
data:
  application.yaml: |
    server:
      port: 8080
    spring:
      application:
        name: my-spring-app
      datasource:
        url: jdbc:postgresql://postgres:5432/mydb
        username: appuser
      jpa:
        hibernate:
          ddl-auto: validate
        show-sql: false
    logging:
      level:
        root: INFO
        com.myapp: DEBUG
    
    # Custom properties
    app:
      feature-flags:
        new-ui: true
        beta-mode: false
      max-upload-size: 10MB
---
# deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: spring-app
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
      containers:
      - name: app
        image: myapp/spring-boot:1.0
        ports:
        - containerPort: 8080
        
        # Pointer Spring Boot vers le ConfigMap
        env:
        - name: SPRING_CONFIG_LOCATION
          value: "file:/config/application.yaml"
        
        volumeMounts:
        - name: config
          mountPath: /config
          readOnly: true
        
        resources:
          requests:
            memory: "1Gi"
            cpu: "500m"
          limits:
            memory: "2Gi"
            cpu: "1000m"
      
      volumes:
      - name: config
        configMap:
          name: spring-app-config
```

---

### Updates de ConfigMaps

#### **Comportement par défaut**

```bash
# Modifier le ConfigMap
kubectl edit configmap app-config

# Ou via YAML
kubectl apply -f configmap-updated.yaml
```

**⚠️ Important :**
- **Variables d'environnement** : Ne sont PAS mises à jour automatiquement (nécessite restart du Pod)
- **Volumes** : Sont mis à jour automatiquement après ~60 secondes (kubelet sync)

```bash
# Pour variables d'env : restart nécessaire
kubectl rollout restart deployment/spring-app

# Pour volumes : automatique mais peut prendre jusqu'à 60s
```

#### **ConfigMap Immutable (recommandé en production)**

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: app-config-v1
immutable: true  # ← Ne peut plus être modifié
data:
  database.host: "postgres.prod.svc.cluster.local"
```

**Avantages :**
- Protection contre les modifications accidentelles
- Meilleure performance (kubelet ne watch plus les changements)
- Versioning explicite (app-config-v1, app-config-v2)

**Workflow de mise à jour :**
```bash
# Créer un nouveau ConfigMap
kubectl apply -f configmap-v2.yaml

# Mettre à jour le Deployment pour utiliser v2
kubectl set env deployment/spring-app --from=configmap/app-config-v2

# Ou via patch
kubectl patch deployment spring-app -p \
  '{"spec":{"template":{"spec":{"volumes":[{"name":"config","configMap":{"name":"app-config-v2"}}]}}}}'
```

---

### Limites et Best Practices

**Limites :**
- Taille maximale : **1 MiB** par ConfigMap
- Ne pas stocker de données sensibles (utiliser Secrets)

**Best Practices :**

✅ **Nommage avec version**
```yaml
metadata:
  name: app-config-v1  # Versioning explicite
```

✅ **Labels pour tracking**
```yaml
metadata:
  labels:
    app: spring-app
    version: v1
    environment: prod
```

✅ **Utiliser immutable en production**
```yaml
immutable: true
```

✅ **Un ConfigMap par environnement**
```
app-config-dev
app-config-staging
app-config-prod
```

✅ **Validation avant apply**
```bash
kubectl apply --dry-run=client -f configmap.yaml
```

❌ **Éviter les secrets dans ConfigMaps**
```yaml
# ❌ MAUVAIS
data:
  database.password: "mysecretpassword"

# ✅ BON - Utiliser un Secret
data:
  database.password: <référence à un Secret>
```

---

## 3.2. Secrets (Types, Best Practices)

### Qu'est-ce qu'un Secret ?

Un **Secret** est un objet qui contient des données sensibles (mots de passe, tokens, clés SSH, etc.). Les données sont encodées en **base64** (pas chiffrées par défaut).

**⚠️ Important :** Base64 n'est PAS du chiffrement, c'est juste de l'encodage !

---

### Types de Secrets

#### **1. Opaque (generic) - Type par défaut**

```bash
# Créer depuis literals
kubectl create secret generic db-credentials \
  --from-literal=username=admin \
  --from-literal=password=SuperSecret123!

# Depuis un fichier
echo -n 'SuperSecret123!' > password.txt
kubectl create secret generic db-credentials \
  --from-file=password=password.txt
```

```yaml
apiVersion: v1
kind: Secret
metadata:
  name: db-credentials
type: Opaque
data:
  # Encodé en base64
  username: YWRtaW4=  # "admin"
  password: U3VwZXJTZWNyZXQxMjMh  # "SuperSecret123!"

# Alternative : utiliser stringData (pas besoin d'encoder)
stringData:
  username: admin
  password: SuperSecret123!
```

```bash
# Encoder/Décoder base64
echo -n 'admin' | base64
# YWRtaW4=

echo 'YWRtaW4=' | base64 -d
# admin
```

#### **2. docker-registry - Pour pull d'images privées**

```bash
kubectl create secret docker-registry regcred \
  --docker-server=myregistry.azurecr.io \
  --docker-username=myuser \
  --docker-password=mypassword \
  --docker-email=user@example.com
```

```yaml
apiVersion: v1
kind: Secret
metadata:
  name: regcred
type: kubernetes.io/dockerconfigjson
data:
  .dockerconfigjson: <base64-encoded-docker-config>
```

**Utilisation dans un Pod :**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: private-app
spec:
  imagePullSecrets:
  - name: regcred  # ← Référence au Secret
  containers:
  - name: app
    image: myregistry.azurecr.io/myapp:1.0
```

#### **3. tls - Pour certificats TLS**

```bash
kubectl create secret tls tls-secret \
  --cert=path/to/tls.crt \
  --key=path/to/tls.key
```

```yaml
apiVersion: v1
kind: Secret
metadata:
  name: tls-secret
type: kubernetes.io/tls
data:
  tls.crt: <base64-cert>
  tls.key: <base64-key>
```

**Utilisation avec Ingress :**
```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: my-ingress
spec:
  tls:
  - hosts:
    - myapp.example.com
    secretName: tls-secret  # ← Certificat TLS
  rules:
  - host: myapp.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: myapp
            port:
              number: 80
```

#### **4. basic-auth - Authentification basique**

```bash
kubectl create secret generic basic-auth \
  --from-literal=username=admin \
  --from-literal=password=secret
```

```yaml
apiVersion: v1
kind: Secret
metadata:
  name: basic-auth
type: kubernetes.io/basic-auth
stringData:
  username: admin
  password: secret
```

#### **5. ssh-auth - Clés SSH**

```bash
kubectl create secret generic ssh-key \
  --from-file=ssh-privatekey=~/.ssh/id_rsa \
  --from-file=ssh-publickey=~/.ssh/id_rsa.pub
```

```yaml
apiVersion: v1
kind: Secret
metadata:
  name: ssh-key
type: kubernetes.io/ssh-auth
data:
  ssh-privatekey: <base64-private-key>
  ssh-publickey: <base64-public-key>
```

---

### Utiliser des Secrets dans les Pods

#### **1. Variables d'environnement**

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: spring-app
spec:
  containers:
  - name: app
    image: myapp/spring-boot:1.0
    env:
    # Référencer une clé spécifique
    - name: DB_USERNAME
      valueFrom:
        secretKeyRef:
          name: db-credentials
          key: username
    
    - name: DB_PASSWORD
      valueFrom:
        secretKeyRef:
          name: db-credentials
          key: password
    
    # Utiliser dans l'application
    - name: SPRING_DATASOURCE_URL
      value: "jdbc:postgresql://postgres:5432/mydb"
    - name: SPRING_DATASOURCE_USERNAME
      valueFrom:
        secretKeyRef:
          name: db-credentials
          key: username
    - name: SPRING_DATASOURCE_PASSWORD
      valueFrom:
        secretKeyRef:
          name: db-credentials
          key: password
```

#### **2. Toutes les clés comme variables d'environnement**

```yaml
envFrom:
- secretRef:
    name: db-credentials
# Toutes les clés deviennent des variables :
# username=admin
# password=SuperSecret123!
```

#### **3. Monter comme volume (fichier)**

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: spring-app
spec:
  containers:
  - name: app
    image: myapp/spring-boot:1.0
    volumeMounts:
    - name: secret-volume
      mountPath: /secrets
      readOnly: true  # ← Toujours en read-only
    
    # L'application lit les fichiers
    env:
    - name: DB_PASSWORD_FILE
      value: "/secrets/password"
  
  volumes:
  - name: secret-volume
    secret:
      secretName: db-credentials
      items:
      - key: password
        path: password
        mode: 0400  # Permissions restrictives
```

**Dans le container :**
```bash
ls -la /secrets/
# -r-------- 1 root root password

cat /secrets/password
# SuperSecret123!
```

---

### Exemple Complet : Spring Boot avec Secrets

```yaml
# secrets.yaml
apiVersion: v1
kind: Secret
metadata:
  name: spring-app-secrets
type: Opaque
stringData:
  db-username: "appuser"
  db-password: "SuperSecret123!"
  jwt-secret: "my-jwt-signing-key-very-long-and-secure"
  api-key: "external-api-key-abc123xyz"
---
# deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: spring-app
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
      containers:
      - name: app
        image: myapp/spring-boot:1.0
        ports:
        - containerPort: 8080
        
        env:
        # Configuration depuis ConfigMap
        - name: SPRING_CONFIG_LOCATION
          value: "file:/config/application.yaml"
        
        # Secrets depuis Secret
        - name: SPRING_DATASOURCE_USERNAME
          valueFrom:
            secretKeyRef:
              name: spring-app-secrets
              key: db-username
        
        - name: SPRING_DATASOURCE_PASSWORD
          valueFrom:
            secretKeyRef:
              name: spring-app-secrets
              key: db-password
        
        - name: JWT_SECRET
          valueFrom:
            secretKeyRef:
              name: spring-app-secrets
              key: jwt-secret
        
        - name: EXTERNAL_API_KEY
          valueFrom:
            secretKeyRef:
              name: spring-app-secrets
              key: api-key
        
        volumeMounts:
        - name: config
          mountPath: /config
          readOnly: true
        
        resources:
          requests:
            memory: "1Gi"
            cpu: "500m"
          limits:
            memory: "2Gi"
            cpu: "1000m"
      
      volumes:
      - name: config
        configMap:
          name: spring-app-config
```

---

### Sécurité des Secrets

#### **Problèmes avec les Secrets par défaut**

❌ **Encodés en base64, pas chiffrés**
```bash
kubectl get secret db-credentials -o yaml
# data:
#   password: U3VwZXJTZWNyZXQxMjMh

echo 'U3VwZXJTZWNyZXQxMjMh' | base64 -d
# SuperSecret123!  ← Facilement décodable !
```

❌ **Stockés en clair dans etcd** (par défaut)
❌ **Visibles par toute personne avec accès kubectl**
❌ **Commit dans Git si non précautionneux**

---

#### **Solutions de sécurisation**

**1. Encryption at Rest (etcd)**

```yaml
# encryption-config.yaml
apiVersion: apiserver.config.k8s.io/v1
kind: EncryptionConfiguration
resources:
  - resources:
    - secrets
    providers:
    - aescbc:
        keys:
        - name: key1
          secret: <32-byte-base64-encoded-key>
    - identity: {}  # Fallback
```

**Configuration API Server :**
```bash
# Dans le manifest de kube-apiserver
--encryption-provider-config=/etc/kubernetes/encryption-config.yaml
```

**2. RBAC Strict**

```yaml
# Rôle qui NE PEUT PAS voir les Secrets
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: pod-reader
rules:
- apiGroups: [""]
  resources: ["pods"]
  verbs: ["get", "list"]
# Pas de "secrets" dans resources !
```

**3. External Secrets Operators**

**a) External Secrets Operator (ESO)**
```yaml
apiVersion: external-secrets.io/v1beta1
kind: SecretStore
metadata:
  name: vault-backend
spec:
  provider:
    vault:
      server: "https://vault.example.com"
      path: "secret"
      version: "v2"
      auth:
        kubernetes:
          mountPath: "kubernetes"
          role: "myapp"
---
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: db-credentials
spec:
  refreshInterval: 1h
  secretStoreRef:
    name: vault-backend
    kind: SecretStore
  target:
    name: db-credentials  # Secret K8s créé automatiquement
  data:
  - secretKey: username
    remoteRef:
      key: database
      property: username
  - secretKey: password
    remoteRef:
      key: database
      property: password
```

**b) Sealed Secrets (Bitnami)**
```bash
# Installer le controller
kubectl apply -f https://github.com/bitnami-labs/sealed-secrets/releases/download/v0.18.0/controller.yaml

# Créer un Secret normal
kubectl create secret generic mysecret --from-literal=password=secret123 --dry-run=client -o yaml > secret.yaml

# Sceller le Secret (chiffré avec la clé publique du cluster)
kubeseal --format yaml < secret.yaml > sealed-secret.yaml

# Commit sealed-secret.yaml dans Git (safe !)
# Le controller déchiffre automatiquement dans le cluster
kubectl apply -f sealed-secret.yaml
```

```yaml
# sealed-secret.yaml (peut être commité dans Git)
apiVersion: bitnami.com/v1alpha1
kind: SealedSecret
metadata:
  name: mysecret
spec:
  encryptedData:
    password: AgB5k7... # ← Chiffré, ne peut être déchiffré que par le cluster
```

**c) HashiCorp Vault**
```yaml
# Pod avec Vault Agent Injector
apiVersion: v1
kind: Pod
metadata:
  name: spring-app
  annotations:
    vault.hashicorp.com/agent-inject: "true"
    vault.hashicorp.com/role: "myapp"
    vault.hashicorp.com/agent-inject-secret-database: "secret/data/database"
spec:
  serviceAccountName: myapp
  containers:
  - name: app
    image: myapp/spring-boot:1.0
    # Vault injecte les secrets dans /vault/secrets/database
```

**d) AWS Secrets Manager / Azure Key Vault / GCP Secret Manager**
```yaml
# AWS Secrets Manager avec ESO
apiVersion: external-secrets.io/v1beta1
kind: SecretStore
metadata:
  name: aws-secrets
spec:
  provider:
    aws:
      service: SecretsManager
      region: eu-west-1
      auth:
        jwt:
          serviceAccountRef:
            name: myapp
```

---

### Best Practices pour les Secrets

✅ **Ne JAMAIS commit de Secrets en clair dans Git**
```bash
# .gitignore
secrets*.yaml
*-secret.yaml
```

✅ **Utiliser stringData pour création**
```yaml
# Plus lisible, pas besoin d'encoder
stringData:
  password: "SuperSecret123!"
```

✅ **RBAC strict**
```bash
# Limiter qui peut lire les Secrets
kubectl create role secret-reader --verb=get,list --resource=secrets
```

✅ **Rotation régulière des Secrets**
```bash
# Automatiser avec External Secrets ou scripts
./rotate-secrets.sh --secret=db-credentials
```

✅ **Audit logs pour accès aux Secrets**
```yaml
# API Server audit policy
- level: RequestResponse
  resources:
  - group: ""
    resources: ["secrets"]
  verbs: ["get", "list", "watch"]
```

✅ **Ne pas logguer les Secrets**
```java
// ❌ MAUVAIS
logger.info("Password: " + password);

// ✅ BON
logger.info("Password loaded successfully");
```

✅ **Immutable Secrets (K8s 1.21+)**
```yaml
apiVersion: v1
kind: Secret
metadata:
  name: db-credentials
immutable: true  # Ne peut plus être modifié
stringData:
  password: "SuperSecret123!"
```

✅ **Separate Secrets par environnement**
```
db-credentials-dev
db-credentials-staging
db-credentials-prod
```

❌ **Éviter les Secrets dans variables d'environnement**
```yaml
# ❌ Variables d'env visibles dans /proc/
env:
- name: DB_PASSWORD
  valueFrom:
    secretKeyRef:
      name: db-credentials
      key: password

# ✅ Préférer les volumes
volumeMounts:
- name: secrets
  mountPath: /secrets
  readOnly: true
```

---

## 3.3. Environment Variables & Volume Mounts

### Variables d'Environnement

#### **Sources de variables d'environnement**

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: env-example
spec:
  containers:
  - name: app
    image: myapp:1.0
    env:
    # 1. Valeur statique
    - name: APP_ENV
      value: "production"
    
    # 2. Depuis ConfigMap
    - name: LOG_LEVEL
      valueFrom:
        configMapKeyRef:
          name: app-config
          key: log.level
    
    # 3. Depuis Secret
    - name: DB_PASSWORD
      valueFrom:
        secretKeyRef:
          name: db-credentials
          key: password
    
    # 4. Downward API (metadata du Pod)
    - name: POD_NAME
      valueFrom:
        fieldRef:
          fieldPath: metadata.name
    
    - name: POD_NAMESPACE
      valueFrom:
        fieldRef:
          fieldPath: metadata.namespace
    
    - name: POD_IP
      valueFrom:
        fieldRef:
          fieldPath: status.podIP
    
    # 5. Resource limits
    - name: MEMORY_LIMIT
      valueFrom:
        resourceFieldRef:
          containerName: app
          resource: limits.memory
    
    # 6. Combinaison de variables
    - name: DB_URL
      value: "postgresql://$(DB_HOST):$(DB_PORT)/$(DB_NAME)"
    
    - name: DB_HOST
      value: "postgres.prod.svc.cluster.local"
    
    - name: DB_PORT
      value: "5432"
    
    - name: DB_NAME
      value: "mydb"
```

#### **envFrom - Charger tout un ConfigMap/Secret**

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: env-from-example
spec:
  containers:
  - name: app
    image: myapp:1.0
    envFrom:
    # Toutes les clés du ConfigMap
    - configMapRef:
        name: app-config
    
    # Toutes les clés du Secret
    - secretRef:
        name: db-credentials
    
    # Avec préfixe
    - configMapRef:
        name: feature-flags
      prefix: FEATURE_
    # Résultat : FEATURE_NEW_UI=true, FEATURE_BETA_MODE=false
```

---

### Volume Mounts

#### **Types de volumes couramment utilisés**

**1. emptyDir - Volume éphémère**
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: shared-volume
spec:
  containers:
  - name: app
    image: myapp:1.0
    volumeMounts:
    - name: cache
      mountPath: /cache
  
  - name: sidecar
    image: cache-cleaner:1.0
    volumeMounts:
    - name: cache
      mountPath: /cache
  
  volumes:
  - name: cache
    emptyDir: {}  # Créé quand le Pod démarre, supprimé quand le Pod est supprimé
```

**emptyDir avec size limit :**
```yaml
volumes:
- name: cache
  emptyDir:
    sizeLimit: 1Gi  # Limite à 1GB
```

**emptyDir en mémoire (RAM) :**
```yaml
volumes:
- name: tmp
  emptyDir:
    medium: Memory  # Stocké en RAM (plus rapide mais limité)
    sizeLimit: 500Mi
```

**2. hostPath - Accès au filesystem du Node**
```yaml
volumes:
- name: host-logs
  hostPath:
    path: /var/log  # Chemin sur le Node
    type: Directory  # Directory, File, DirectoryOrCreate, FileOrCreate
```

⚠️ **Utiliser avec précaution** : pose des problèmes de sécurité et de portabilité

**3. ConfigMap comme volume**
```yaml
volumes:
- name: config
  configMap:
    name: app-config
    items:  # Optionnel : sélectionner des clés
    - key: application.properties
      path: app.properties
      mode: 0644
    defaultMode: 0644  # Permissions par défaut
```

**4. Secret comme volume**
```yaml
volumes:
- name: secrets
  secret:
    secretName: db-credentials
    items:
    - key: password
      path: db-password
      mode: 0400  # Read-only pour owner
    defaultMode: 0400
```

**5. PersistentVolumeClaim**
```yaml
volumes:
- name: data
  persistentVolumeClaim:
    claimName: my-pvc
```

**6. projected - Combiner plusieurs sources**
```yaml
volumes:
- name: combined
  projected:
    sources:
    - configMap:
        name: app-config
        items:
        - key: application.properties
          path: config/app.properties
    
    - secret:
        name: db-credentials
        items:
        - key: password
          path: secrets/db-password
    
    - downwardAPI:
        items:
        - path: "labels"
          fieldRef:
            fieldPath: metadata.labels
```

---

#### **Exemple Complet : Spring Boot avec Configuration Multi-source**

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: spring-app-config
data:
  application.yaml: |
    server:
      port: 8080
    spring:
      application:
        name: my-app
      datasource:
        url: jdbc:postgresql://postgres:5432/mydb
    logging:
      level:
        root: INFO
  
  logback.xml: |
    <?xml version="1.0" encoding="UTF-8"?>
    <configuration>
      <appender name="STDOUT" class="ch.qos.logback.core.ConsoleAppender">
        <encoder>
          <pattern>%d{ISO8601} [%thread] %-5level %logger{36} - %msg%n</pattern>
        </encoder>
      </appender>
      <root level="INFO">
        <appender-ref ref="STDOUT" />
      </root>
    </configuration>
---
apiVersion: v1
kind: Secret
metadata:
  name: spring-app-secrets
type: Opaque
stringData:
  db-username: "appuser"
  db-password: "SuperSecret123!"
---
apiVersion: apps/v1
kind: Deployment
metadata:
  name: spring-app
spec:
  replicas: 3
  selector:
    matchLabels:
      app: spring-app
  template:
    metadata:
      labels:
        app: spring-app
        version: v1
    spec:
      containers:
      - name: app
        image: myapp/spring-boot:1.0
        ports:
        - containerPort: 8080
        
        env:
        # Variables statiques
        - name: SPRING_PROFILES_ACTIVE
          value: "prod"
        
        - name: JAVA_OPTS
          value: "-Xmx1g -XX:MaxRAMPercentage=75.0"
        
        # Depuis ConfigMap
        - name: SPRING_CONFIG_LOCATION
          value: "file:/config/application.yaml"
        
        # Depuis Secret
        - name: SPRING_DATASOURCE_USERNAME
          valueFrom:
            secretKeyRef:
              name: spring-app-secrets
              key: db-username
        
        - name: SPRING_DATASOURCE_PASSWORD
          valueFrom:
            secretKeyRef:
              name: spring-app-secrets
              key: db-password
        
        # Downward API
        - name: POD_NAME
          valueFrom:
            fieldRef:
              fieldPath: metadata.name
        
        - name: POD_NAMESPACE
          valueFrom:
            fieldRef:
              fieldPath: metadata.namespace
        
        - name: POD_IP
          valueFrom:
            fieldRef:
              fieldPath: status.podIP
        
        - name: NODE_NAME
          valueFrom:
            fieldRef:
              fieldPath: spec.nodeName
        
        volumeMounts:
        # ConfigMap monté
        - name: config
          mountPath: /config
          readOnly: true
        
        # Secret monté (alternative aux env vars)
        - name: secrets
          mountPath: /secrets
          readOnly: true
        
        # emptyDir pour cache
        - name: cache
          mountPath: /app/cache
        
        # emptyDir pour logs temporaires
        - name: tmp
          mountPath: /tmp
        
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
          initialDelaySeconds: 30
          periodSeconds: 10
        
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8080
          initialDelaySeconds: 60
          periodSeconds: 10
      
      volumes:
      - name: config
        configMap:
          name: spring-app-config
          items:
          - key: application.yaml
            path: application.yaml
          - key: logback.xml
            path: logback.xml
      
      - name: secrets
        secret:
          secretName: spring-app-secrets
          defaultMode: 0400
      
      - name: cache
        emptyDir:
          sizeLimit: 1Gi
      
      - name: tmp
        emptyDir:
          medium: Memory
          sizeLimit: 500Mi
```

**Vérification dans le container :**
```bash
kubectl exec -it spring-app-xxx -- sh

# Variables d'environnement
env | grep SPRING
# SPRING_PROFILES_ACTIVE=prod
# SPRING_CONFIG_LOCATION=file:/config/application.yaml
# SPRING_DATASOURCE_USERNAME=appuser
# SPRING_DATASOURCE_PASSWORD=SuperSecret123!

env | grep POD
# POD_NAME=spring-app-abc123-xyz
# POD_NAMESPACE=default
# POD_IP=10.244.1.5

# Fichiers montés
ls -la /config/
# application.yaml
# logback.xml

cat /config/application.yaml

ls -la /secrets/
# -r-------- db-username
# -r-------- db-password

cat /secrets/db-password
# SuperSecret123!

df -h /app/cache
# 1.0G limit

df -h /tmp
# 500M limit (memory-backed)
```

---

### SubPath - Monter un fichier spécifique sans écraser le répertoire

```yaml
volumeMounts:
- name: config
  mountPath: /app/application.yaml  # Fichier spécifique
  subPath: application.yaml  # ← Important pour ne pas écraser /app/
  readOnly: true

# Sans subPath : tout /app/ serait remplacé par le contenu du ConfigMap
# Avec subPath : seul /app/application.yaml est monté
```

**Cas d'usage :**
```yaml
# Monter nginx.conf dans /etc/nginx/ sans écraser les autres fichiers
volumeMounts:
- name: nginx-config
  mountPath: /etc/nginx/nginx.conf
  subPath: nginx.conf
  readOnly: true
```

---

## 3.4. Downward API

### Qu'est-ce que la Downward API ?

La **Downward API** permet d'exposer les **métadonnées du Pod** aux containers sous forme de :
- Variables d'environnement
- Fichiers dans un volume

**Informations exposables :**
- Nom du Pod, namespace, labels, annotations
- IP du Pod, nom du Node
- Resources requests et limits
- ServiceAccount name

---

### Downward API via Variables d'Environnement

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: downward-api-env
  labels:
    app: my-app
    tier: backend
    version: v1
  annotations:
    description: "Example Pod for Downward API"
spec:
  containers:
  - name: app
    image: busybox
    command: ["sh", "-c", "while true; do env | grep MY_; sleep 3600; done"]
    env:
    # Metadata du Pod
    - name: MY_POD_NAME
      valueFrom:
        fieldRef:
          fieldPath: metadata.name
    
    - name: MY_POD_NAMESPACE
      valueFrom:
        fieldRef:
          fieldPath: metadata.namespace
    
    - name: MY_POD_IP
      valueFrom:
        fieldRef:
          fieldPath: status.podIP
    
    - name: MY_POD_SERVICE_ACCOUNT
      valueFrom:
        fieldRef:
          fieldPath: spec.serviceAccountName
    
    # Node information
    - name: MY_NODE_NAME
      valueFrom:
        fieldRef:
          fieldPath: spec.nodeName
    
    - name: MY_HOST_IP
      valueFrom:
        fieldRef:
          fieldPath: status.hostIP
    
    # Labels (format: label_key="label_value",...)
    - name: MY_POD_LABELS
      valueFrom:
        fieldRef:
          fieldPath: metadata.labels
    
    # Annotations
    - name: MY_POD_ANNOTATIONS
      valueFrom:
        fieldRef:
          fieldPath: metadata.annotations
    
    # Resources
    - name: MY_CPU_REQUEST
      valueFrom:
        resourceFieldRef:
          containerName: app
          resource: requests.cpu
    
    - name: MY_CPU_LIMIT
      valueFrom:
        resourceFieldRef:
          containerName: app
          resource: limits.cpu
    
    - name: MY_MEMORY_REQUEST
      valueFrom:
        resourceFieldRef:
          containerName: app
          resource: requests.memory
          divisor: 1Mi  # Convertir en MiB
    
    - name: MY_MEMORY_LIMIT
      valueFrom:
        resourceFieldRef:
          containerName: app
          resource: limits.memory
          divisor: 1Mi
    
    resources:
      requests:
        memory: "512Mi"
        cpu: "500m"
      limits:
        memory: "1Gi"
        cpu: "1000m"
```

**Résultat :**
```bash
kubectl logs downward-api-env
# MY_POD_NAME=downward-api-env
# MY_POD_NAMESPACE=default
# MY_POD_IP=10.244.1.10
# MY_POD_SERVICE_ACCOUNT=default
# MY_NODE_NAME=worker-1
# MY_HOST_IP=192.168.1.100
# MY_POD_LABELS=app="my-app",tier="backend",version="v1"
# MY_POD_ANNOTATIONS=description="Example Pod for Downward API"
# MY_CPU_REQUEST=500
# MY_CPU_LIMIT=1000
# MY_MEMORY_REQUEST=512
# MY_MEMORY_LIMIT=1024
```

---

### Downward API via Volume

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: downward-api-volume
  labels:
    app: my-app
    environment: prod
  annotations:
    build.number: "12345"
    git.commit: "abc123def"
spec:
  containers:
  - name: app
    image: busybox
    command: ["sh", "-c", "while true; do cat /etc/podinfo/*; sleep 3600; done"]
    volumeMounts:
    - name: podinfo
      mountPath: /etc/podinfo
      readOnly: true
  
  volumes:
  - name: podinfo
    downwardAPI:
      items:
      # Metadata
      - path: "pod-name"
        fieldRef:
          fieldPath: metadata.name
      
      - path: "pod-namespace"
        fieldRef:
          fieldPath: metadata.namespace
      
      - path: "pod-ip"
        fieldRef:
          fieldPath: status.podIP
      
      # Labels (tous dans un fichier)
      - path: "labels"
        fieldRef:
          fieldPath: metadata.labels
      
      # Annotations (toutes dans un fichier)
      - path: "annotations"
        fieldRef:
          fieldPath: metadata.annotations
      
      # Resources
      - path: "cpu-limit"
        resourceFieldRef:
          containerName: app
          resource: limits.cpu
      
      - path: "memory-limit"
        resourceFieldRef:
          containerName: app
          resource: limits.memory
          divisor: 1Mi
    
    resources:
      requests:
        memory: "256Mi"
        cpu: "250m"
      limits:
        memory: "512Mi"
        cpu: "500m"
```

**Résultat dans le container :**
```bash
kubectl exec downward-api-volume -- ls -la /etc/podinfo/
# pod-name
# pod-namespace
# pod-ip
# labels
# annotations
# cpu-limit
# memory-limit

kubectl exec downward-api-volume -- cat /etc/podinfo/pod-name
# downward-api-volume

kubectl exec downward-api-volume -- cat /etc/podinfo/labels
# app="my-app"
# environment="prod"

kubectl exec downward-api-volume -- cat /etc/podinfo/annotations
# build.number="12345"
# git.commit="abc123def"
# kubernetes.io/config.seen="..."
# kubernetes.io/config.source="..."

kubectl exec downward-api-volume -- cat /etc/podinfo/cpu-limit
# 500m

kubectl exec downward-api-volume -- cat /etc/podinfo/memory-limit
# 512
```

---

### Cas d'Usage Pratiques de la Downward API

#### **1. Logging avec contexte Pod**

```yaml
env:
- name: POD_NAME
  valueFrom:
    fieldRef:
      fieldPath: metadata.name
- name: POD_NAMESPACE
  valueFrom:
    fieldRef:
      fieldPath: metadata.namespace
- name: POD_IP
  valueFrom:
    fieldRef:
      fieldPath: status.podIP
```

**Application Java/Spring Boot :**
```java
@Component
public class LoggingContextConfig {
    @Value("${POD_NAME:unknown}")
    private String podName;
    
    @Value("${POD_NAMESPACE:unknown}")
    private String namespace;
    
    @Value("${POD_IP:unknown}")
    private String podIp;
    
    @PostConstruct
    public void init() {
        MDC.put("pod.name", podName);
        MDC.put("pod.namespace", namespace);
        MDC.put("pod.ip", podIp);
    }
}

// Logs incluront automatiquement : [pod.name=spring-app-abc123]
```

#### **2. Distributed Tracing avec Pod Identity**

```yaml
env:
- name: POD_NAME
  valueFrom:
    fieldRef:
      fieldPath: metadata.name
- name: NODE_NAME
  valueFrom:
    fieldRef:
      fieldPath: spec.nodeName
```

**Jaeger/OpenTelemetry :**
```java
Span span = tracer.buildSpan("processRequest")
    .withTag("pod.name", System.getenv("POD_NAME"))
    .withTag("node.name", System.getenv("NODE_NAME"))
    .start();
```

#### **3. Auto-configuration basée sur les resources**

```yaml
env:
- name: MEMORY_LIMIT
  valueFrom:
    resourceFieldRef:
      resource: limits.memory
      divisor: 1Mi
```

**Application :**
```java
@Configuration
public class CacheConfig {
    @Value("${MEMORY_LIMIT:1024}")
    private int memoryLimitMi;
    
    @Bean
    public CacheManager cacheManager() {
        // Allouer 25% de la mémoire pour le cache
        int cacheSize = (int) (memoryLimitMi * 0.25);
        return new CaffeineCacheManager();
    }
}
```

#### **4. Service Discovery dynamique**

```yaml
env:
- name: POD_NAMESPACE
  valueFrom:
    fieldRef:
      fieldPath: metadata.namespace
```

**Application :**
```java
String namespace = System.getenv("POD_NAMESPACE");
String dbHost = String.format("postgres.%s.svc.cluster.local", namespace);
// Connexion automatique à la DB du bon namespace
```

#### **5. Feature Flags basés sur labels**

```yaml
volumes:
- name: pod-labels
  downwardAPI:
    items:
    - path: "labels"
      fieldRef:
        fieldPath: metadata.labels
```

**Application lit les labels :**
```java
public class FeatureFlagService {
    public Map<String, String> loadLabels() throws IOException {
        Path labelsPath = Paths.get("/etc/podinfo/labels");
        String content = Files.readString(labelsPath);
        // Parse: feature.beta="true"
        return parseLabels(content);
    }
    
    public boolean isBetaEnabled() {
        Map<String, String> labels = loadLabels();
        return "true".equals(labels.get("feature.beta"));
    }
}
```

---

## 🎯 Patterns de Configuration Avancés

### Pattern 1 : Configuration par Environnement

```yaml
# Structure Git
k8s/
  base/
    deployment.yaml
    service.yaml
  overlays/
    dev/
      configmap.yaml
      kustomization.yaml
    staging/
      configmap.yaml
      kustomization.yaml
    prod/
      configmap.yaml
      kustomization.yaml
```

**Avec Kustomize :**
```yaml
# overlays/prod/kustomization.yaml
apiVersion: kustomize.config.k8s.io/v1beta1
kind: Kustomization
namespace: prod
bases:
- ../../base
configMapGenerator:
- name: app-config
  files:
  - application-prod.yaml
secretGenerator:
- name: app-secrets
  envs:
  - secrets-prod.env
```

```bash
kubectl apply -k overlays/prod/
```

---

### Pattern 2 : ConfigMap Reloader (Auto-restart on change)

**Stakater Reloader :**
```bash
kubectl apply -f https://raw.githubusercontent.com/stakater/Reloader/master/deployments/kubernetes/reloader.yaml
```

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: spring-app
  annotations:
    reloader.stakater.com/auto: "true"  # Auto-reload si ConfigMap/Secret change
    # Ou spécifique :
    # configmap.reloader.stakater.com/reload: "app-config"
    # secret.reloader.stakater.com/reload: "app-secrets"
spec:
  template:
    spec:
      containers:
      - name: app
        image: myapp:1.0
        # ...
```

**Quand app-config change → Deployment est automatiquement restart**

---

### Pattern 3 : Configuration avec Validation

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: app-config
data:
  config.json: |
    {
      "database": {
        "host": "postgres.prod.svc.cluster.local",
        "port": 5432,
        "maxConnections": 100
      },
      "cache": {
        "ttl": 3600,
        "maxSize": 1000
      }
    }
```

**Init Container pour validation :**
```yaml
initContainers:
- name: validate-config
  image: myapp/config-validator:1.0
  command:
  - /bin/sh
  - -c
  - |
    python /validate.py /config/config.json
    if [ $? -ne 0 ]; then
      echo "Configuration validation failed!"
      exit 1
    fi
  volumeMounts:
  - name: config
    mountPath: /config
    readOnly: true
```

---

### Pattern 4 : Hot-reload Configuration (sans restart)

**Spring Boot avec @RefreshScope :**
```java
@Configuration
@RefreshScope  // ← Permet le reload
public class DynamicConfig {
    @Value("${feature.flag.new-ui}")
    private boolean newUiEnabled;
    
    public boolean isNewUiEnabled() {
        return newUiEnabled;
    }
}
```

**ConfigMap monté comme volume :**
```yaml
volumeMounts:
- name: config
  mountPath: /config
  readOnly: true

# Spring watch le fichier
env:
- name: SPRING_CONFIG_LOCATION
  value: "file:/config/application.yaml"
- name: SPRING_CLOUD_KUBERNETES_RELOAD_ENABLED
  value: "true"
- name: SPRING_CLOUD_KUBERNETES_RELOAD_MODE
  value: "polling"
- name: SPRING_CLOUD_KUBERNETES_RELOAD_PERIOD
  value: "15000"  # 15 secondes
```

**Dépendance Maven :**
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-kubernetes-config</artifactId>
</dependency>
```

---

### Pattern 5 : Secrets Rotation

```bash
#!/bin/bash
# rotate-secret.sh

SECRET_NAME="db-credentials"
NEW_PASSWORD=$(openssl rand -base64 32)

# Mettre à jour le Secret
kubectl create secret generic $SECRET_NAME \
  --from-literal=password=$NEW_PASSWORD \
  --dry-run=client -o yaml | kubectl apply -f -

# Mettre à jour la DB avec le nouveau password
kubectl exec -it postgres-0 -- psql -c \
  "ALTER USER appuser WITH PASSWORD '$NEW_PASSWORD';"

# Restart les Pods pour charger le nouveau Secret
kubectl rollout restart deployment/spring-app

echo "Secret rotated successfully!"
```

**Automatiser avec CronJob :**
```yaml
apiVersion: batch/v1
kind: CronJob
metadata:
  name: rotate-secrets
spec:
  schedule: "0 2 1 * *"  # Premier jour du mois à 2h
  jobTemplate:
    spec:
      template:
        spec:
          serviceAccountName: secret-rotator  # RBAC nécessaire
          containers:
          - name: rotator
            image: secret-rotator:1.0
            command: ["/rotate-secret.sh"]
          restartPolicy: OnFailure
```

---

## 📝 Best Practices Résumées

### ConfigMaps
✅ Versioning (app-config-v1, v2, v3)
✅ Immutable en production
✅ Séparer par environnement
✅ Validation avant déploiement
✅ Limiter la taille (< 1 MiB)
❌ Pas de données sensibles

### Secrets
✅ Encryption at rest (etcd)
✅ RBAC strict
✅ External Secrets Operator (Vault, AWS Secrets Manager)
✅ Rotation régulière
✅ Audit logging
✅ Immutable en production
✅ Monter en volumes plutôt qu'en env vars
❌ Jamais dans Git (sauf sealed-secrets)
❌ Jamais en logs

### Variables d'Environnement
✅ Nommage cohérent (UPPERCASE_WITH_UNDERSCORES)
✅ Documenter les variables requises
✅ Valeurs par défaut dans l'application
✅ Validation au démarrage
❌ Éviter les secrets en env vars (préférer volumes)

### Volumes
✅ readOnly pour ConfigMaps/Secrets
✅ Permissions restrictives (0400, 0600)
✅ subPath quand nécessaire
✅ emptyDir avec sizeLimit
✅ Cleanup des volumes temporaires

---

## 🎓 Checklist de Configuration

```yaml
pre-deployment-config-check:
  configmaps:
    ✓ Version explicite (app-config-v1)
    ✓ immutable: true
    ✓ Pas de secrets
    ✓ Validé (schema, format)
    ✓ Labels appropriés
  
  secrets:
    ✓ Type approprié (Opaque, TLS, etc.)
    ✓ immutable: true (production)
    ✓ Pas commité dans Git
    ✓ Encryption at rest activée
    ✓ RBAC configuré
    ✓ Rotation planifiée
  
  environment:
    ✓ Variables documentées
    ✓ Secrets via secretKeyRef (pas literals)
    ✓ Downward API pour metadata
    ✓ Validation au démarrage
  
  volumes:
    ✓ readOnly pour config/secrets
    ✓ Permissions restrictives
    ✓ sizeLimit pour emptyDir
    ✓ Pas de hostPath (sauf nécessité absolue)
  
  monitoring:
    ✓ Config reload détecté (metrics)
    ✓ Alertes sur échecs de validation
    ✓ Audit logs pour secrets
```

---

# Chapitre 3 : Questions/Réponses d'Entretien - Configuration & Secrets

## 🎯 ConfigMaps

### Q1 : Qu'est-ce qu'un ConfigMap et pourquoi l'utiliser plutôt que coder la configuration en dur ?

**Réponse :**

Un **ConfigMap** est un objet Kubernetes qui stocke des données de configuration sous forme de paires clé-valeur, permettant de découpler la configuration du code applicatif.

**Avantages :**

1. **Séparation des préoccupations** : Code applicatif indépendant de la configuration
2. **Réutilisabilité** : Même image Docker pour tous les environnements
3. **Versioning et rollback** : Configuration versionnée avec Git
4. **Gestion centralisée** : Un ConfigMap partagé par plusieurs Pods

**Exemple :**
```yaml
# Même image pour tous les environnements
image: myapp/spring-boot:1.0

# ConfigMaps différents par environnement
# dev: app-config-dev → database: postgres-dev
# prod: app-config-prod → database: postgres-prod
```

---

### Q2 : Quelles sont les différentes façons d'utiliser un ConfigMap dans un Pod ?

**Réponse :**

**1. Variables d'environnement (clé individuelle)**
```yaml
env:
- name: DATABASE_HOST
  valueFrom:
    configMapKeyRef:
      name: app-config
      key: database.host
```

**2. envFrom (toutes les clés)**
```yaml
envFrom:
- configMapRef:
    name: app-config
```

**3. Volume mount (fichiers)**
```yaml
volumeMounts:
- name: config-volume
  mountPath: /config
  readOnly: true

volumes:
- name: config-volume
  configMap:
    name: app-config
```

**Comparaison :**

| Méthode | Update auto | Use case |
|---------|-------------|----------|
| env (clé unique) | ❌ | Variables simples |
| envFrom | ❌ | Charger toutes les clés |
| Volume | ✅ | Fichiers config (hot-reload) |

---

### Q3 : Comment gérer des ConfigMaps différents pour dev, staging et prod ?

**Réponse :**

**Stratégie recommandée : Kustomize**

```
k8s/
├── base/
│   ├── deployment.yaml
│   └── kustomization.yaml
└── overlays/
    ├── dev/
    │   ├── configmap.yaml
    │   └── kustomization.yaml
    └── prod/
        ├── configmap.yaml
        └── kustomization.yaml
```

```yaml
# overlays/prod/kustomization.yaml
apiVersion: kustomize.config.k8s.io/v1beta1
kind: Kustomization
namespace: prod
bases:
- ../../base
configMapGenerator:
- name: app-config
  literals:
  - LOG_LEVEL=ERROR
  - DATABASE_HOST=postgres-prod
```

```bash
kubectl apply -k overlays/prod/
```

---

### Q4 : Que se passe-t-il quand vous modifiez un ConfigMap utilisé par un Pod running ?

**Réponse :**

**Variables d'environnement : ❌ PAS de mise à jour**
```bash
# Nécessite restart du Pod
kubectl rollout restart deployment/my-app
```

**Volume mount : ✅ Mise à jour automatique (~60s)**
```bash
# Kubelet synchronise automatiquement
# L'application doit reload la config
```

**ConfigMap Immutable : ❌ Cannot modify**
```yaml
immutable: true  # Protection
```

Pour mettre à jour : créer nouvelle version (v2, v3...)

---

## 🔐 Secrets

### Q5 : Quelle est la différence entre un ConfigMap et un Secret ?

**Réponse :**

| Aspect | ConfigMap | Secret |
|--------|-----------|--------|
| Usage | Config non-sensible | Données sensibles |
| Encodage | Texte brut | Base64 |
| Chiffrement | Non | Optionnel (encryption at rest) |
| RBAC | Moins strict | Doit être strict |

**⚠️ Important : Base64 ≠ Chiffrement !**

```bash
echo 'U3VwZXJTZWNyZXQxMjMh' | base64 -d
# SuperSecret123!  # Facilement décodable
```

---

### Q6 : Comment sécurisez-vous les Secrets dans Kubernetes ?

**Réponse :**

**1. Encryption at Rest**
```yaml
# /etc/kubernetes/encryption-config.yaml
apiVersion: apiserver.config.k8s.io/v1
kind: EncryptionConfiguration
resources:
  - resources:
    - secrets
    providers:
    - aescbc:
        keys:
        - name: key1
          secret: <32-byte-base64-key>
```

**2. RBAC Strict**
```yaml
# Limiter qui peut lire les Secrets
rules:
- apiGroups: [""]
  resources: ["pods", "configmaps"]
  verbs: ["get", "list"]
# Pas de "secrets" !
```

**3. External Secrets Operator (recommandé)**
```yaml
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: db-credentials
spec:
  secretStoreRef:
    name: aws-secrets
  data:
  - secretKey: password
    remoteRef:
      key: prod/database/password
```

**4. Sealed Secrets**
```bash
kubeseal < secret.yaml > sealed-secret.yaml
# Peut être commité dans Git en toute sécurité
```

**5. HashiCorp Vault**
```yaml
annotations:
  vault.hashicorp.com/agent-inject: "true"
  vault.hashicorp.com/role: "myapp"
```

**6. Best Practices**
- ✅ Volumes > Variables d'env
- ✅ Immutable Secrets
- ✅ Rotation régulière
- ✅ Audit logging
- ❌ Jamais dans Git

---

### Q7 : Expliquez les différents types de Secrets et leurs cas d'usage.

**Réponse :**

**1. Opaque (default)**
```yaml
type: Opaque
stringData:
  password: "SuperSecret123!"
```
Use case : Passwords, API keys

**2. kubernetes.io/dockerconfigjson**
```bash
kubectl create secret docker-registry regcred \
  --docker-server=myregistry.azurecr.io
```
Use case : Pull images privées

**3. kubernetes.io/tls**
```bash
kubectl create secret tls tls-secret \
  --cert=tls.crt --key=tls.key
```
Use case : Certificats HTTPS, Ingress

**4. kubernetes.io/basic-auth**
```yaml
type: kubernetes.io/basic-auth
stringData:
  username: admin
  password: secret
```

**5. kubernetes.io/ssh-auth**
```yaml
type: kubernetes.io/ssh-auth
data:
  ssh-privatekey: <base64>
```

---

## 🌐 Variables & Volumes

### Q8 : Quelle est la meilleure pratique pour injecter des secrets : variables d'environnement ou volumes ?

**Réponse :**

**TL;DR : Préférer les VOLUMES**

**Problèmes avec env vars :**
```bash
# ❌ Visible dans /proc/
kubectl exec pod -- cat /proc/1/environ

# ❌ Visible dans ps
kubectl exec pod -- ps e

# ❌ Pas de mise à jour sans restart
```

**Avantages des volumes :**
```yaml
volumes:
- name: secrets
  secret:
    secretName: db-credentials
    defaultMode: 0400  # Permissions restrictives

volumeMounts:
- name: secrets
  mountPath: /secrets
  readOnly: true
```

```bash
# ✅ Permissions restrictives
ls -la /secrets/
# -r-------- password

# ✅ Mise à jour auto (~60s)
# ✅ Pas dans /proc/ ou ps
```

**Code Java pour lire depuis fichiers :**
```java
@Bean
public DataSource dataSource() throws IOException {
    String username = Files.readString(Paths.get("/secrets/db/username")).trim();
    String password = Files.readString(Paths.get("/secrets/db/password")).trim();
    
    HikariConfig config = new HikariConfig();
    config.setJdbcUrl("jdbc:postgresql://postgres:5432/mydb");
    config.setUsername(username);
    config.setPassword(password);
    
    return new HikariDataSource(config);
}
```

---

### Q9 : Qu'est-ce que la Downward API et quels sont ses cas d'usage ?

**Réponse :**

La **Downward API** expose les métadonnées du Pod aux containers.

**Via variables d'env :**
```yaml
env:
- name: POD_NAME
  valueFrom:
    fieldRef:
      fieldPath: metadata.name

- name: MEMORY_LIMIT_MB
  valueFrom:
    resourceFieldRef:
      resource: limits.memory
      divisor: 1Mi
```

**Via volume :**
```yaml
volumes:
- name: podinfo
  downwardAPI:
    items:
    - path: "labels"
      fieldRef:
        fieldPath: metadata.labels
```

**Cas d'usage :**

**1. Logging structuré**
```java
MDC.put("pod.name", System.getenv("POD_NAME"));
MDC.put("pod.ip", System.getenv("POD_IP"));
// Logs: [pod.name=spring-app-abc123] Processing request
```

**2. Auto-configuration**
```java
@Value("${MEMORY_LIMIT_MB:1024}")
private int memoryLimitMb;

@Bean
public CacheManager cacheManager() {
    int cacheSizeMb = (int) (memoryLimitMb * 0.25);
    return CacheManagerBuilder.newCacheManagerBuilder()
        .withCache("myCache", ResourcePoolsBuilder.heap(cacheSizeMb, MemoryUnit.MB))
        .build(true);
}
```

**3. Service Discovery**
```java
String namespace = System.getenv("POD_NAMESPACE");
String dbHost = String.format("postgres.%s.svc.cluster.local", namespace);
```

---

## 🎯 Questions Situationnelles

### Q10 : Votre application Spring Boot a besoin de credentials DB, d'un certificat TLS et d'un fichier de configuration. Comment structurez-vous cela ?

**Réponse :**

```yaml
# 1. ConfigMap pour configuration
apiVersion: v1
kind: ConfigMap
metadata:
  name: spring-app-config-v1
immutable: true
data:
  application.yaml: |
    server:
      port: 8080
    spring:
      datasource:
        url: jdbc:postgresql://postgres:5432/mydb
---
# 2. Secret pour credentials DB
apiVersion: v1
kind: Secret
metadata:
  name: db-credentials-v1
type: Opaque
immutable: true
stringData:
  username: "appuser"
  password: "SuperSecret123!"
---
# 3. Secret TLS
apiVersion: v1
kind: Secret
metadata:
  name: app-tls-v1
type: kubernetes.io/tls
data:
  tls.crt: <base64-cert>
  tls.key: <base64-key>
---
# 4. Deployment
apiVersion: apps/v1
kind: Deployment
metadata:
  name: spring-app
spec:
  template:
    spec:
      containers:
      - name: app
        image: myapp:1.0
        
        env:
        - name: SPRING_CONFIG_LOCATION
          value: "file:/config/application.yaml"
        
        - name: SPRING_DATASOURCE_USERNAME_FILE
          value: "/secrets/db/username"
        - name: SPRING_DATASOURCE_PASSWORD_FILE
          value: "/secrets/db/password"
        
        volumeMounts:
        - name: config
          mountPath: /config
          readOnly: true
        - name: db-secrets
          mountPath: /secrets/db
          readOnly: true
        - name: tls-certs
          mountPath: /secrets/tls
          readOnly: true
      
      volumes:
      - name: config
        configMap:
          name: spring-app-config-v1
      - name: db-secrets
        secret:
          secretName: db-credentials-v1
          defaultMode: 0400
      - name: tls-certs
        secret:
          secretName: app-tls-v1
          defaultMode: 0400
```

**✅ Séparation claire, immutable, sécurisé**

---

### Q11 : Comment gérer la rotation des Secrets sans downtime ?

**Réponse :**

**Stratégie 1 : Dual-credential pattern**

```java
private DataSource createDataSourceWithFallback(String username, String primary, String secondary) {
    try {
        return createDataSource(username, primary);
    } catch (SQLException e) {
        logger.warn("Primary password failed, trying secondary");
        return createDataSource(username, secondary);
    }
}
```

**Étapes :**
1. Application supporte 2 passwords
2. Ajouter nouveau password au Secret
3. Rolling update (nouveaux Pods utilisent nouveau)
4. Mettre à jour la DB
5. Retirer ancien password

**Stratégie 2 : External Secrets avec hot-reload**

```yaml
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: db-credentials
spec:
  refreshInterval: 1h
  secretStoreRef:
    name: aws-secrets
```

```java
@Component
public class DynamicCredentialsProvider {
    @Scheduled(fixedRate = 15000)
    public void checkPasswordRotation() throws IOException {
        FileTime currentModified = Files.getLastModifiedTime(passwordPath);
        
        if (hasChanged(currentModified)) {
            String newPassword = Files.readString(passwordPath).trim();
            rotateDataSourcePassword(newPassword);
        }
    }
    
    private void rotateDataSourcePassword(String newPassword) {
        HikariDataSource ds = (HikariDataSource) dataSource;
        ds.getHikariConfigMXBean().setPassword(newPassword);
        ds.getHikariPoolMXBean().softEvictConnections();
    }
}
```

**✅ Rotation automatique sans restart !**

---

### Q12 : Un développeur a accidentellement commité un Secret dans Git. Que faites-vous ?

**Réponse :**

**Actions immédiates :**

**1. Révoquer le Secret (PRIORITÉ)**
```bash
# Nouveau password
NEW_PASSWORD=$(openssl rand -base64 32)

# Mettre à jour K8s
kubectl create secret generic db-credentials \
  --from-literal=password=$NEW_PASSWORD \
  --dry-run=client -o yaml | kubectl apply -f -

# Mettre à jour la DB
kubectl exec postgres-0 -- psql -c \
  "ALTER USER appuser WITH PASSWORD '$NEW_PASSWORD';"

# Restart apps
kubectl rollout restart deployment/spring-app
```

**2. Nettoyer Git history**
```bash
# BFG Repo-Cleaner
java -jar bfg.jar --delete-files secret.yaml repo.git

cd repo.git
git reflog expire --expire=now --all
git gc --prune=now --aggressive
git push --force --all
```

**⚠️ Tous les devs doivent re-clone !**

**3. Mesures préventives**

**git-secrets :**
```bash
git secrets --install
git secrets --add 'password\s*=\s*.+'
```

**Sealed Secrets :**
```bash
kubeseal < secret.yaml > sealed-secret.yaml
git add sealed-secret.yaml  # SAFE
```

**CI/CD scanning :**
```yaml
# .github/workflows/secret-scan.yml
- name: Gitleaks
  uses: zricethezav/gitleaks-action@v1.6.0
```

**.gitignore :**
```
*secret*.yaml
secrets/
.env
*.pem
*.key
```

**4. Post-mortem**
```markdown
# Root Cause : Pre-commit hooks non configurés
# Actions : 
- ✅ git-secrets installé partout
- ✅ CI/CD scan activé
- ✅ Formation équipe
```

---

## 🎓 Conseils pour l'Entretien

### Points clés à maîtriser :
1. ConfigMaps : Création, utilisation, updates, immutabilité
2. Secrets : Types, sécurisation (encryption, RBAC, ESO)
3. Variables vs Volumes pour secrets
4. Downward API cas d'usage
5. Rotation sans downtime

### Red flags à éviter :
- ❌ Secrets dans Git
- ❌ Pas d'encryption at rest
- ❌ RBAC trop permissif
- ❌ Pas de stratégie de rotation
- ❌ Secrets en env vars

### Points bonus :
- ✅ External Secrets Operator
- ✅ Sealed Secrets pour GitOps
- ✅ git-secrets / pre-commit hooks
- ✅ Hot-reload avec Spring Cloud
- ✅ Compliance (SOC2, GDPR)

---