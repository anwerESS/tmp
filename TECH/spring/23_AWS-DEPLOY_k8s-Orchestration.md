## [**..**](./00_index.md)


## Déploiement et Orchestration Cloud (AWS & Kubernetes)

## 1. Déploiement Spring Boot sur AWS Cloud

### Architecture Cloud Native sur AWS
- **Conception** : Déploiement d'applications Spring Boot sur l'écosystème AWS avec architecture multi-tiers
- **Services AWS maîtrisés** :
  - **EC2** : Déploiement traditionnel avec Auto Scaling Groups pour la haute disponibilité
  - **ECS (Elastic Container Service)** : Orchestration de containers Docker avec Fargate (serverless)
  - **EKS (Elastic Kubernetes Service)** : Cluster Kubernetes managé pour orchestration avancée
  - **RDS** : Bases de données relationnelles (PostgreSQL, MySQL) avec Multi-AZ pour la résilience
  - **ElastiCache** : Redis/Memcached pour le caching distribué
  - **S3** : Stockage d'objets pour fichiers statiques et backups
  - **CloudWatch** : Monitoring, logs et métriques applicatives
  - **ALB/NLB** : Application/Network Load Balancers pour la distribution du trafic
  - **Route 53** : DNS et routing intelligent
  - **Secrets Manager** : Gestion sécurisée des credentials et configurations sensibles
  - **IAM** : Gestion des accès et permissions avec principe du moindre privilège

### Pipeline CI/CD sur AWS
- **Stratégie** : Automatisation complète du déploiement avec pipeline CI/CD
- **Outils utilisés** :
  - **CodePipeline** : Orchestration du pipeline de bout en bout
  - **CodeBuild** : Compilation, tests et création d'images Docker
  - **ECR (Elastic Container Registry)** : Registry privé pour stocker les images Docker
  - **CodeDeploy** : Déploiement Blue/Green ou Rolling pour zéro downtime
- **Exemple de workflow** :
  1. Push code vers GitHub/GitLab
  2. CodePipeline déclenche automatiquement
  3. CodeBuild exécute les tests et build l'image Docker
  4. Push de l'image vers ECR
  5. CodeDeploy déploie sur ECS/EKS avec stratégie Blue/Green
  6. Health checks automatiques et rollback si échec

### Configuration Spring Boot pour AWS
```yaml
# Application optimisée pour AWS
spring:
  datasource:
    url: jdbc:postgresql://${RDS_HOSTNAME}:5432/${RDS_DB_NAME}
    username: ${RDS_USERNAME}
    password: ${RDS_PASSWORD}
  
  cache:
    type: redis
    redis:
      host: ${ELASTICACHE_ENDPOINT}
      port: 6379
  
  cloud:
    aws:
      credentials:
        instance-profile: true  # Utilisation IAM Role
      region:
        static: eu-west-1
      s3:
        bucket: my-app-storage

management:
  endpoints:
    web:
      exposure:
        include: health,metrics,prometheus
  metrics:
    export:
      cloudwatch:
        enabled: true
        namespace: MySpringApp
```

### Sécurité et Bonnes Pratiques AWS
- **Secrets Management** : Aucun secret en dur dans le code, utilisation de AWS Secrets Manager
- **Network Security** : VPC avec subnets publics/privés, Security Groups restrictifs
- **IAM Roles** : Attribution de rôles IAM aux instances EC2/ECS tasks (pas de credentials statiques)
- **Encryption** : Données chiffrées at-rest (RDS, S3) et in-transit (TLS/SSL)
- **Backup & Disaster Recovery** : Snapshots RDS automatiques, réplication cross-region si critique

---

## 2. Container Orchestration avec Kubernetes

### Déploiement d'Applications Spring Boot sur K8s

#### Architecture Kubernetes
- **Clusters** : Configuration et gestion de clusters Kubernetes (EKS sur AWS, GKE, ou on-premise)
- **Namespaces** : Isolation logique par environnement (dev, staging, production)
- **Stratégie de déploiement** : Rolling updates, Blue/Green, Canary deployments

#### Deployment & Pods
```yaml
# Exemple de Deployment pour Spring Boot
apiVersion: apps/v1
kind: Deployment
metadata:
  name: accounts-service
  namespace: production
  labels:
    app: accounts
    version: v1.2.0
spec:
  replicas: 3  # Haute disponibilité
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxSurge: 1
      maxUnavailable: 0  # Zero downtime
  selector:
    matchLabels:
      app: accounts
  template:
    metadata:
      labels:
        app: accounts
        version: v1.2.0
    spec:
      containers:
      - name: accounts-app
        image: myregistry.io/accounts-service:1.2.0
        ports:
        - containerPort: 8080
          name: http
        - containerPort: 8081
          name: management
        
        # Configuration des ressources
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
        
        # Health checks
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8081
          initialDelaySeconds: 60
          periodSeconds: 10
        
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8081
          initialDelaySeconds: 30
          periodSeconds: 5
        
        # Variables d'environnement depuis ConfigMaps/Secrets
        envFrom:
        - configMapRef:
            name: accounts-config
        - secretRef:
            name: accounts-secrets
        
        env:
        - name: SPRING_PROFILES_ACTIVE
          value: "production"
        - name: JAVA_OPTS
          value: "-Xmx768m -Xms512m -XX:+UseG1GC"
```

---

### Gestion des ConfigMaps & Secrets

#### ConfigMaps - Configuration Externalisée
- **Objectif** : Séparer la configuration de l'application du code
- **Usage** : Propriétés non-sensibles (URLs, timeouts, feature flags)

```yaml
# ConfigMap pour configuration applicative
apiVersion: v1
kind: ConfigMap
metadata:
  name: accounts-config
  namespace: production
data:
  # Configuration Spring Boot
  application.yaml: |
    server:
      port: 8080
    
    spring:
      application:
        name: accounts-service
      
      datasource:
        url: jdbc:postgresql://postgres-service:5432/accountsdb
        hikari:
          maximum-pool-size: 20
          minimum-idle: 5
      
      kafka:
        bootstrap-servers: kafka-cluster:9092
        consumer:
          group-id: accounts-consumer-group
    
    resilience4j:
      circuitbreaker:
        instances:
          default:
            failureRateThreshold: 50
            waitDurationInOpenState: 10s
  
  # Variables d'environnement simples
  LOG_LEVEL: "INFO"
  FEATURE_NEW_API: "true"
  MAX_RETRY_ATTEMPTS: "3"

---
# Montage en tant que fichier
apiVersion: v1
kind: Pod
metadata:
  name: accounts-pod
spec:
  containers:
  - name: app
    image: accounts-service:latest
    volumeMounts:
    - name: config-volume
      mountPath: /config
      readOnly: true
  volumes:
  - name: config-volume
    configMap:
      name: accounts-config
      items:
      - key: application.yaml
        path: application.yaml

# Spring Boot lit automatiquement depuis /config/application.yaml
```

#### Secrets - Gestion Sécurisée des Données Sensibles
- **Objectif** : Stocker credentials, tokens, certificats de manière sécurisée
- **Encryption at rest** : Secrets chiffrés dans etcd avec KMS (AWS, GCP, Azure)

```yaml
# Secret pour credentials de base de données
apiVersion: v1
kind: Secret
metadata:
  name: accounts-secrets
  namespace: production
type: Opaque
data:
  # Valeurs encodées en base64
  DB_USERNAME: YWRtaW4=  # admin
  DB_PASSWORD: c3VwZXJTZWNyZXQxMjM=  # superSecret123
  JWT_SECRET: bXlTdXBlclNlY3JldEtleUZvckpXVA==
  REDIS_PASSWORD: cmVkaXNQYXNzd29yZA==

---
# Secret depuis fichier (pour certificats TLS)
apiVersion: v1
kind: Secret
metadata:
  name: tls-secret
type: kubernetes.io/tls
data:
  tls.crt: LS0tLS1CRUdJTi...  # Certificat
  tls.key: LS0tLS1CRUdJTi...  # Clé privée

---
# Utilisation dans le Pod
apiVersion: v1
kind: Pod
metadata:
  name: accounts-pod
spec:
  containers:
  - name: app
    image: accounts-service:latest
    env:
    # Secret comme variable d'environnement
    - name: DB_PASSWORD
      valueFrom:
        secretKeyRef:
          name: accounts-secrets
          key: DB_PASSWORD
    
    # Ou montage en tant que fichier
    volumeMounts:
    - name: secret-volume
      mountPath: /secrets
      readOnly: true
  volumes:
  - name: secret-volume
    secret:
      secretName: accounts-secrets
```

**Bonnes pratiques Secrets** :
- ✅ Utiliser **External Secrets Operator** pour synchro avec AWS Secrets Manager/Vault
- ✅ Rotation automatique des secrets
- ✅ RBAC strict (principe du moindre privilège)
- ✅ Audit logging de l'accès aux secrets
- ❌ Jamais de secrets dans les images Docker ou ConfigMaps

---

### Services Kubernetes

#### Types de Services
- **ClusterIP** : Communication interne entre microservices
- **NodePort** : Exposition sur un port du node (dev/test)
- **LoadBalancer** : Exposition externe via Load Balancer cloud (AWS ELB)
- **Headless Service** : Discovery de pods individuels (StatefulSets)

```yaml
# Service ClusterIP pour communication interne
apiVersion: v1
kind: Service
metadata:
  name: accounts-service
  namespace: production
  labels:
    app: accounts
spec:
  type: ClusterIP
  selector:
    app: accounts  # Route vers les pods avec ce label
  ports:
  - name: http
    port: 80
    targetPort: 8080
    protocol: TCP
  - name: management
    port: 8081
    targetPort: 8081
    protocol: TCP
  
  # Session affinity si nécessaire
  sessionAffinity: ClientIP

---
# Service LoadBalancer pour exposition externe
apiVersion: v1
kind: Service
metadata:
  name: api-gateway-external
  namespace: production
  annotations:
    service.beta.kubernetes.io/aws-load-balancer-type: "nlb"
    service.beta.kubernetes.io/aws-load-balancer-cross-zone-load-balancing-enabled: "true"
spec:
  type: LoadBalancer
  selector:
    app: api-gateway
  ports:
  - name: https
    port: 443
    targetPort: 8443
    protocol: TCP

---
# Headless Service pour StatefulSet (base de données)
apiVersion: v1
kind: Service
metadata:
  name: postgres-headless
spec:
  clusterIP: None  # Headless
  selector:
    app: postgres
  ports:
  - port: 5432
    name: postgres
```

**Service Discovery** :
- Les services créent automatiquement des entrées DNS : `accounts-service.production.svc.cluster.local`
- Spring Boot peut appeler : `http://accounts-service/api/accounts`

---

### Ingress - Routage HTTP/HTTPS Intelligent

#### Ingress Controller
- **Installation** : NGINX Ingress Controller, Traefik, ou AWS ALB Ingress Controller
- **Fonctionnalités** : TLS termination, path-based routing, virtual hosting

```yaml
# Installation NGINX Ingress Controller (Helm)
helm repo add ingress-nginx https://kubernetes.github.io/ingress-nginx
helm install nginx-ingress ingress-nginx/ingress-nginx \
  --namespace ingress-nginx --create-namespace

---
# Ingress avec routing avancé
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: microservices-ingress
  namespace: production
  annotations:
    # NGINX Ingress annotations
    nginx.ingress.kubernetes.io/rewrite-target: /$2
    nginx.ingress.kubernetes.io/ssl-redirect: "true"
    nginx.ingress.kubernetes.io/force-ssl-redirect: "true"
    nginx.ingress.kubernetes.io/rate-limit: "100"
    
    # CORS
    nginx.ingress.kubernetes.io/enable-cors: "true"
    nginx.ingress.kubernetes.io/cors-allow-origin: "https://myapp.com"
    
    # Timeouts
    nginx.ingress.kubernetes.io/proxy-connect-timeout: "30"
    nginx.ingress.kubernetes.io/proxy-send-timeout: "30"
    nginx.ingress.kubernetes.io/proxy-read-timeout: "30"
    
    # Circuit Breaker
    nginx.ingress.kubernetes.io/upstream-fail-timeout: "10"
    nginx.ingress.kubernetes.io/upstream-max-fails: "3"

spec:
  ingressClassName: nginx
  
  # TLS/SSL Configuration
  tls:
  - hosts:
    - api.myapp.com
    secretName: tls-secret  # Certificat SSL
  
  rules:
  # Virtual host principal
  - host: api.myapp.com
    http:
      paths:
      # Accounts Service
      - path: /api/accounts(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: accounts-service
            port:
              number: 80
      
      # Loans Service
      - path: /api/loans(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: loans-service
            port:
              number: 80
      
      # Cards Service
      - path: /api/cards(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: cards-service
            port:
              number: 80
      
      # API Gateway (catch-all)
      - path: /
        pathType: Prefix
        backend:
          service:
            name: api-gateway
            port:
              number: 80

---
# Ingress avec Canary Deployment
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: accounts-canary
  namespace: production
  annotations:
    nginx.ingress.kubernetes.io/canary: "true"
    nginx.ingress.kubernetes.io/canary-weight: "10"  # 10% du trafic
spec:
  ingressClassName: nginx
  rules:
  - host: api.myapp.com
    http:
      paths:
      - path: /api/accounts
        pathType: Prefix
        backend:
          service:
            name: accounts-service-v2  # Nouvelle version
            port:
              number: 80
```

---

### Stratégies Avancées de Déploiement

#### Rolling Update (Par défaut)
```yaml
spec:
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxSurge: 1        # 1 pod supplémentaire pendant update
      maxUnavailable: 0  # Aucun pod down = zero downtime
```

#### Blue/Green Deployment
```yaml
# Déploiement Blue (version actuelle)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: accounts-blue
spec:
  replicas: 3
  selector:
    matchLabels:
      app: accounts
      version: blue
  template:
    metadata:
      labels:
        app: accounts
        version: blue
    spec:
      containers:
      - name: accounts
        image: accounts:1.0.0

---
# Déploiement Green (nouvelle version)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: accounts-green
spec:
  replicas: 3
  selector:
    matchLabels:
      app: accounts
      version: green
  template:
    metadata:
      labels:
        app: accounts
        version: green
    spec:
      containers:
      - name: accounts
        image: accounts:2.0.0

---
# Service qui bascule entre Blue et Green
apiVersion: v1
kind: Service
metadata:
  name: accounts-service
spec:
  selector:
    app: accounts
    version: blue  # Changer à 'green' pour basculer
  ports:
  - port: 80
    targetPort: 8080
```

**Process** :
1. Déployer Green en parallèle de Blue
2. Tester Green en interne
3. Basculer le Service de `version: blue` vers `version: green`
4. Rollback instantané si problème (rebasculer vers blue)
5. Supprimer Blue après validation

---

### Monitoring & Observability sur Kubernetes

#### Prometheus & Grafana
```yaml
# ServiceMonitor pour Prometheus
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: accounts-metrics
  namespace: production
spec:
  selector:
    matchLabels:
      app: accounts
  endpoints:
  - port: management
    path: /actuator/prometheus
    interval: 30s

---
# Spring Boot configuration pour métriques
management:
  endpoints:
    web:
      exposure:
        include: health,metrics,prometheus
  metrics:
    tags:
      application: ${spring.application.name}
      environment: production
    export:
      prometheus:
        enabled: true
```

#### Logging avec ELK/EFK Stack
```yaml
# Fluentd DaemonSet pour collecter les logs
apiVersion: v1
kind: ConfigMap
metadata:
  name: fluentd-config
data:
  fluent.conf: |
    <source>
      @type tail
      path /var/log/containers/*.log
      pos_file /var/log/fluentd-containers.log.pos
      tag kubernetes.*
      format json
    </source>
    
    <filter kubernetes.**>
      @type kubernetes_metadata
    </filter>
    
    <match **>
      @type elasticsearch
      host elasticsearch-service
      port 9200
      index_name fluentd-${tag}
    </match>
```

---

### Auto-Scaling sur Kubernetes

#### Horizontal Pod Autoscaler (HPA)
```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: accounts-hpa
  namespace: production
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: accounts-service
  minReplicas: 3
  maxReplicas: 20
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70  # Scale si CPU > 70%
  - type: Resource
    resource:
      name: memory
      target:
        type: Utilization
        averageUtilization: 80  # Scale si RAM > 80%
  
  # Scale plus rapidement lors des pics
  behavior:
    scaleUp:
      stabilizationWindowSeconds: 60
      policies:
      - type: Percent
        value: 100  # Double le nombre de pods
        periodSeconds: 60
    scaleDown:
      stabilizationWindowSeconds: 300  # Attendre 5min avant scale down
      policies:
      - type: Pods
        value: 1
        periodSeconds: 60
```

#### Vertical Pod Autoscaler (VPA)
```yaml
apiVersion: autoscaling.k8s.io/v1
kind: VerticalPodAutoscaler
metadata:
  name: accounts-vpa
spec:
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: accounts-service
  updatePolicy:
    updateMode: "Auto"  # Ajuste automatiquement les ressources
  resourcePolicy:
    containerPolicies:
    - containerName: accounts-app
      minAllowed:
        cpu: 200m
        memory: 256Mi
      maxAllowed:
        cpu: 2000m
        memory: 2Gi
```

---

## Résultats & Impact

### Performance & Scalabilité
✅ **Zero Downtime** : Déploiements sans interruption grâce aux rolling updates et health checks  
✅ **Auto-scaling** : Gestion automatique de 3 à 50 pods selon la charge  
✅ **Haute Disponibilité** : Multi-AZ deployment sur AWS avec failover automatique  
✅ **Time to Market** : Réduction de 70% du temps de déploiement via CI/CD automatisé

### Coûts & Efficacité
✅ **Optimisation des coûts** : Réduction de 40% des coûts infrastructure via auto-scaling intelligent  
✅ **Resource Efficiency** : Utilisation optimale des ressources avec requests/limits bien calibrés  
✅ **Spot Instances** : Intégration de EC2 Spot Instances pour les workloads non-critiques

### Sécurité & Compliance
✅ **Secrets Management** : Aucun secret en clair, rotation automatique  
✅ **Network Policies** : Isolation réseau entre namespaces et pods  
✅ **RBAC** : Principe du moindre privilège appliqué strictement  
✅ **Audit Logging** : Traçabilité complète des déploiements et accès

---

## Technologies & Outils Maîtrisés

**Cloud Providers** : AWS (EKS, EC2, ECS, RDS, ElastiCache, S3, CloudWatch)  
**Container Orchestration** : Kubernetes, Docker, Helm  
**CI/CD** : AWS CodePipeline, Jenkins, GitLab CI, ArgoCD  
**Infrastructure as Code** : Terraform, AWS CloudFormation, Helm Charts  
**Monitoring** : Prometheus, Grafana, CloudWatch, ELK/EFK Stack  
**Service Mesh** : Istio (bonus si applicable)  
**Security** : AWS Secrets Manager, HashiCorp Vault, Network Policies


---

# Questions/Réponses Techniques - AWS & Kubernetes

## 1. Déploiement Spring Boot sur AWS

### Q1: Quelles sont les différentes options pour déployer une application Spring Boot sur AWS et comment choisir ?
**R:** Il existe plusieurs options, chacune avec ses avantages :

**1. EC2 (Elastic Compute Cloud)**
- ✅ Contrôle total sur l'infrastructure
- ✅ Compatible avec applications legacy
- ❌ Gestion manuelle des serveurs, patches, scaling
- **Quand utiliser** : Applications monolithiques, besoins de personnalisation système

**2. ECS (Elastic Container Service) + Fargate**
- ✅ Pas de gestion de serveurs (serverless containers)
- ✅ Intégration native AWS (IAM, CloudWatch)
- ✅ Scaling automatique simple
- ❌ Lock-in AWS
- **Quand utiliser** : Microservices containerisés, équipe sans expertise K8s

**3. EKS (Elastic Kubernetes Service)**
- ✅ Portable (multi-cloud, on-premise)
- ✅ Écosystème K8s riche (Helm, Operators)
- ✅ Orchestration avancée
- ❌ Complexité plus élevée
- ❌ Coûts de gestion du control plane
- **Quand utiliser** : Architecture microservices complexe, besoin de portabilité

**4. Elastic Beanstalk**
- ✅ PaaS simple, déploiement rapide
- ✅ Gestion automatique infrastructure
- ❌ Moins de contrôle, personnalisation limitée
- **Quand utiliser** : Prototypes, petites applications, équipes débutantes

**5. Lambda (serverless)**
- ✅ Pay-per-use, pas de gestion serveur
- ✅ Auto-scaling illimité
- ❌ Cold start, limitations (timeout 15min, taille package)
- **Quand utiliser** : APIs event-driven, workloads sporadiques

**Ma recommandation selon contexte** :
```
Startup / MVP          → Elastic Beanstalk ou ECS Fargate
Microservices simples  → ECS Fargate
Microservices complexes→ EKS
Événementiel/Async     → Lambda
Legacy migration       → EC2 avec migration progressive vers containers
```

---

### Q2: Comment configurer une application Spring Boot pour lire les secrets depuis AWS Secrets Manager ?
**R:** Plusieurs approches possibles :

**Approche 1 : AWS SDK direct**
```xml
<!-- Dépendance Maven -->
<dependency>
    <groupId>software.amazon.awssdk</groupId>
    <artifactId>secretsmanager</artifactId>
</dependency>
```

```java
@Configuration
public class SecretsManagerConfig {
    
    @Bean
    public SecretsManagerClient secretsManagerClient() {
        return SecretsManagerClient.builder()
            .region(Region.EU_WEST_1)
            .build();
    }
    
    @Bean
    public DataSource dataSource(SecretsManagerClient client) {
        // Récupérer le secret
        GetSecretValueRequest request = GetSecretValueRequest.builder()
            .secretId("prod/database/credentials")
            .build();
        
        GetSecretValueResponse response = client.getSecretValue(request);
        String secretString = response.secretString();
        
        // Parser le JSON
        JsonNode secrets = new ObjectMapper().readTree(secretString);
        String username = secrets.get("username").asText();
        String password = secrets.get("password").asText();
        
        // Configurer DataSource
        HikariDataSource dataSource = new HikariDataSource();
        dataSource.setJdbcUrl(secrets.get("url").asText());
        dataSource.setUsername(username);
        dataSource.setPassword(password);
        
        return dataSource;
    }
}
```

**Approche 2 : Spring Cloud AWS (recommandée)**
```xml
<dependency>
    <groupId>io.awspring.cloud</groupId>
    <artifactId>spring-cloud-aws-starter-secrets-manager</artifactId>
</dependency>
```

```yaml
# application.yml
spring:
  cloud:
    aws:
      region:
        static: eu-west-1
      credentials:
        instance-profile: true  # Utilise IAM role du EC2/ECS
      
  config:
    import: aws-secretsmanager:prod/myapp/  # Préfixe des secrets

# Les secrets sont automatiquement injectés comme properties
# Secret "prod/myapp/database" avec clé "password" devient:
# ${database.password}
```

```java
@Configuration
public class DatabaseConfig {
    
    @Value("${database.url}")
    private String dbUrl;
    
    @Value("${database.username}")
    private String dbUsername;
    
    @Value("${database.password}")  // Depuis Secrets Manager
    private String dbPassword;
    
    @Bean
    public DataSource dataSource() {
        return DataSourceBuilder.create()
            .url(dbUrl)
            .username(dbUsername)
            .password(dbPassword)
            .build();
    }
}
```

**Approche 3 : Rotation automatique avec cache**
```java
@Service
public class SecretsCacheService {
    
    private final SecretsManagerClient client;
    private final Cache<String, String> secretsCache;
    
    public SecretsCacheService(SecretsManagerClient client) {
        this.client = client;
        this.secretsCache = CacheBuilder.newBuilder()
            .expireAfterWrite(5, TimeUnit.MINUTES)  // Refresh toutes les 5min
            .build();
    }
    
    public String getSecret(String secretId) {
        return secretsCache.get(secretId, () -> {
            GetSecretValueResponse response = client.getSecretValue(
                GetSecretValueRequest.builder()
                    .secretId(secretId)
                    .build()
            );
            return response.secretString();
        });
    }
}
```

**Configuration IAM Role (important)** :
```json
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": [
        "secretsmanager:GetSecretValue",
        "secretsmanager:DescribeSecret"
      ],
      "Resource": "arn:aws:secretsmanager:eu-west-1:123456789:secret:prod/myapp/*"
    }
  ]
}
```

---

### Q3: Comment mettre en place un pipeline CI/CD complet pour déployer une Spring Boot app sur AWS EKS ?
**R:** Voici un pipeline complet avec AWS CodePipeline et GitHub Actions :

**Architecture du Pipeline** :
```
GitHub Push → CodePipeline → CodeBuild → ECR → EKS Deployment → Health Check
```

**Étape 1 : buildspec.yml pour CodeBuild**
```yaml
version: 0.2

phases:
  pre_build:
    commands:
      # Login ECR
      - echo Logging in to Amazon ECR...
      - aws ecr get-login-password --region $AWS_REGION | docker login --username AWS --password-stdin $ECR_REGISTRY
      
      # Variables
      - REPOSITORY_URI=$ECR_REGISTRY/$IMAGE_REPO_NAME
      - COMMIT_HASH=$(echo $CODEBUILD_RESOLVED_SOURCE_VERSION | cut -c 1-7)
      - IMAGE_TAG=${COMMIT_HASH:=latest}
  
  build:
    commands:
      # Build Spring Boot avec Maven
      - echo Build started on `date`
      - mvn clean package -DskipTests
      
      # Build Docker image
      - docker build -t $REPOSITORY_URI:latest .
      - docker tag $REPOSITORY_URI:latest $REPOSITORY_URI:$IMAGE_TAG
      
      # Tests (optionnel)
      - mvn test
  
  post_build:
    commands:
      # Push vers ECR
      - echo Pushing Docker image...
      - docker push $REPOSITORY_URI:latest
      - docker push $REPOSITORY_URI:$IMAGE_TAG
      
      # Créer imagedefinitions.json pour ECS (si ECS)
      - printf '[{"name":"accounts-service","imageUri":"%s"}]' $REPOSITORY_URI:$IMAGE_TAG > imagedefinitions.json
      
      # Mettre à jour K8s deployment
      - echo Deploying to EKS...
      - aws eks update-kubeconfig --region $AWS_REGION --name $EKS_CLUSTER_NAME
      - kubectl set image deployment/accounts-service accounts-app=$REPOSITORY_URI:$IMAGE_TAG -n production
      - kubectl rollout status deployment/accounts-service -n production

artifacts:
  files:
    - imagedefinitions.json
    - k8s/*.yaml
```

**Étape 2 : GitHub Actions alternative (plus moderne)**
```yaml
# .github/workflows/deploy-eks.yml
name: Deploy to EKS

on:
  push:
    branches: [main]

env:
  AWS_REGION: eu-west-1
  EKS_CLUSTER_NAME: production-cluster
  ECR_REPOSITORY: accounts-service

jobs:
  build-and-deploy:
    runs-on: ubuntu-latest
    
    steps:
    - name: Checkout code
      uses: actions/checkout@v3
    
    - name: Setup JDK 17
      uses: actions/setup-java@v3
      with:
        java-version: '17'
        distribution: 'temurin'
        cache: maven
    
    - name: Build with Maven
      run: mvn clean package -DskipTests
    
    - name: Run tests
      run: mvn test
    
    - name: Configure AWS credentials
      uses: aws-actions/configure-aws-credentials@v2
      with:
        aws-access-key-id: ${{ secrets.AWS_ACCESS_KEY_ID }}
        aws-secret-access-key: ${{ secrets.AWS_SECRET_ACCESS_KEY }}
        aws-region: ${{ env.AWS_REGION }}
    
    - name: Login to Amazon ECR
      id: login-ecr
      uses: aws-actions/amazon-ecr-login@v1
    
    - name: Build and push Docker image
      env:
        ECR_REGISTRY: ${{ steps.login-ecr.outputs.registry }}
        IMAGE_TAG: ${{ github.sha }}
      run: |
        docker build -t $ECR_REGISTRY/$ECR_REPOSITORY:$IMAGE_TAG .
        docker tag $ECR_REGISTRY/$ECR_REPOSITORY:$IMAGE_TAG $ECR_REGISTRY/$ECR_REPOSITORY:latest
        docker push $ECR_REGISTRY/$ECR_REPOSITORY:$IMAGE_TAG
        docker push $ECR_REGISTRY/$ECR_REPOSITORY:latest
    
    - name: Install kubectl
      uses: azure/setup-kubectl@v3
    
    - name: Update kubeconfig
      run: |
        aws eks update-kubeconfig --region $AWS_REGION --name $EKS_CLUSTER_NAME
    
    - name: Deploy to EKS
      env:
        ECR_REGISTRY: ${{ steps.login-ecr.outputs.registry }}
        IMAGE_TAG: ${{ github.sha }}
      run: |
        kubectl set image deployment/accounts-service \
          accounts-app=$ECR_REGISTRY/$ECR_REPOSITORY:$IMAGE_TAG \
          -n production
        
        kubectl rollout status deployment/accounts-service -n production --timeout=5m
    
    - name: Verify deployment
      run: |
        kubectl get pods -n production -l app=accounts
        kubectl get svc -n production accounts-service
```

**Étape 3 : Blue/Green Deployment avec validation**
```yaml
# deploy-blue-green.yml
name: Blue/Green Deployment

jobs:
  deploy:
    steps:
    # ... (build steps identiques)
    
    - name: Deploy Green version
      run: |
        # Déployer nouvelle version (green)
        kubectl apply -f k8s/deployment-green.yaml
        kubectl wait --for=condition=available --timeout=300s deployment/accounts-green -n production
    
    - name: Run smoke tests on Green
      run: |
        GREEN_POD=$(kubectl get pod -n production -l version=green -o jsonpath='{.items[0].metadata.name}')
        kubectl port-forward -n production $GREEN_POD 8080:8080 &
        sleep 5
        
        # Tests de santé
        curl -f http://localhost:8080/actuator/health || exit 1
        curl -f http://localhost:8080/actuator/health/readiness || exit 1
        
        # Tests fonctionnels
        ./run-integration-tests.sh http://localhost:8080
    
    - name: Switch traffic to Green
      if: success()
      run: |
        # Basculer le service vers green
        kubectl patch service accounts-service -n production -p '{"spec":{"selector":{"version":"green"}}}'
        
        echo "Traffic switched to Green version"
    
    - name: Monitor for 5 minutes
      run: |
        sleep 300
        
        # Vérifier les erreurs
        ERROR_RATE=$(kubectl logs -n production -l version=green --since=5m | grep ERROR | wc -l)
        if [ $ERROR_RATE -gt 10 ]; then
          echo "High error rate detected!"
          exit 1
        fi
    
    - name: Cleanup Blue version
      if: success()
      run: |
        kubectl delete deployment accounts-blue -n production
        echo "Blue version removed successfully"
    
    - name: Rollback to Blue
      if: failure()
      run: |
        echo "Deployment failed, rolling back to Blue"
        kubectl patch service accounts-service -n production -p '{"spec":{"selector":{"version":"blue"}}}'
        kubectl delete deployment accounts-green -n production
```

**Étape 4 : Notifications Slack**
```yaml
    - name: Notify Slack on success
      if: success()
      uses: slackapi/slack-github-action@v1
      with:
        payload: |
          {
            "text": "✅ Deployment successful to EKS",
            "blocks": [
              {
                "type": "section",
                "text": {
                  "type": "mrkdwn",
                  "text": "*Deployment to Production EKS*\n✅ Status: Success\n🔖 Version: ${{ github.sha }}\n👤 Author: ${{ github.actor }}"
                }
              }
            ]
          }
      env:
        SLACK_WEBHOOK_URL: ${{ secrets.SLACK_WEBHOOK }}
```

---

## 2. Kubernetes - ConfigMaps & Secrets

### Q4: Quelle est la différence entre ConfigMap et Secret ? Quand utiliser l'un ou l'autre ?
**R:**

| Aspect | ConfigMap | Secret |
|--------|-----------|--------|
| **Usage** | Configuration non-sensible | Données sensibles (passwords, tokens, certs) |
| **Stockage** | Texte clair dans etcd | Base64 encodé (⚠️ pas du chiffrement!) |
| **Encryption at rest** | Non par défaut | Oui si KMS activé |
| **Taille max** | 1MB | 1MB |
| **Visibilité** | Visible dans logs, describe | Caché dans describe (montré en ***) |

**ConfigMap - Exemples d'usage** :
```yaml
# ✅ BON - Configuration applicative
- URLs de services internes
- Feature flags (FEATURE_NEW_API: "true")
- Configuration Spring profiles
- Paramètres de timeouts, retry
- Configuration logging levels

# ❌ MAUVAIS - Données sensibles
- Passwords database
- API keys externes
- JWT secrets
- Certificats TLS
```

**Secret - Exemples d'usage** :
```yaml
# ✅ BON - Données sensibles
- Database passwords
- API keys (AWS, Stripe, etc.)
- OAuth tokens
- Private keys, certificates
- Docker registry credentials

# Types de secrets K8s
- Opaque: secret générique (default)
- kubernetes.io/tls: certificats TLS
- kubernetes.io/dockerconfigjson: Docker registry auth
- kubernetes.io/basic-auth: user/password
- kubernetes.io/ssh-auth: clés SSH
```

**⚠️ IMPORTANT : Sécurité des Secrets K8s**
```yaml
# Par défaut, secrets K8s = base64 (PAS DU CHIFFREMENT!)
# N'importe qui avec accès kubectl peut décoder:
kubectl get secret my-secret -o jsonpath='{.data.password}' | base64 -d

# Solutions pour vraie sécurité:

# 1. Encryption at rest avec KMS
apiVersion: apiserver.config.k8s.io/v1
kind: EncryptionConfiguration
resources:
  - resources:
      - secrets
    providers:
      - kms:
          name: aws-encryption-provider
          endpoint: unix:///var/run/kmsplugin/socket.sock
      - identity: {}  # Fallback

# 2. External Secrets Operator (recommandé)
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: database-credentials
spec:
  secretStoreRef:
    name: aws-secrets-manager
    kind: SecretStore
  target:
    name: db-secret  # Secret K8s créé automatiquement
  data:
  - secretKey: password
    remoteRef:
      key: prod/database/credentials
      property: password
```

---

### Q5: Comment gérer les ConfigMaps et Secrets dans différents environnements (dev, staging, prod) ?
**R:** Plusieurs stratégies selon la complexité :

**Stratégie 1 : Namespaces séparés**
```yaml
# ConfigMap dev
apiVersion: v1
kind: ConfigMap
metadata:
  name: app-config
  namespace: dev
data:
  DATABASE_URL: "jdbc:postgresql://dev-db:5432/myapp"
  LOG_LEVEL: "DEBUG"
  FEATURE_NEW_API: "true"

---
# ConfigMap prod
apiVersion: v1
kind: ConfigMap
metadata:
  name: app-config
  namespace: production
data:
  DATABASE_URL: "jdbc:postgresql://prod-db:5432/myapp"
  LOG_LEVEL: "WARN"
  FEATURE_NEW_API: "false"

# Même nom, namespace différent
# Déploiement: kubectl apply -f config.yaml -n dev|production
```

**Stratégie 2 : Kustomize (recommandé pour K8s natif)**
```bash
# Structure de fichiers
.
├── base/
│   ├── kustomization.yaml
│   ├── deployment.yaml
│   └── configmap.yaml
├── overlays/
│   ├── dev/
│   │   ├── kustomization.yaml
│   │   └── configmap.yaml
│   ├── staging/
│   │   ├── kustomization.yaml
│   │   └── configmap.yaml
│   └── production/
│       ├── kustomization.yaml
│       ├── configmap.yaml
│       └── replicas.yaml
```

```yaml
# base/kustomization.yaml
apiVersion: kustomize.config.k8s.io/v1beta1
kind: Kustomization
resources:
  - deployment.yaml
  - configmap.yaml
  - service.yaml

---
# base/configmap.yaml (valeurs par défaut)
apiVersion: v1
kind: ConfigMap
metadata:
  name: app-config
data:
  LOG_LEVEL: "INFO"

---
# overlays/dev/kustomization.yaml
apiVersion: kustomize.config.k8s.io/v1beta1
kind: Kustomization
namespace: dev
bases:
  - ../../base
configMapGenerator:
  - name: app-config
    behavior: merge
    literals:
      - LOG_LEVEL=DEBUG
      - ENVIRONMENT=development
replicas:
  - name: accounts-service
    count: 1

---
# overlays/production/kustomization.yaml
apiVersion: kustomize.config.k8s.io/v1beta1
kind: Kustomization
namespace: production
bases:
  - ../../base
configMapGenerator:
  - name: app-config
    behavior: merge
    literals:
      - LOG_LEVEL=WARN
      - ENVIRONMENT=production
replicas:
  - name: accounts-service
    count: 5

# Déploiement
kubectl apply -k overlays/dev
kubectl apply -k overlays/production
```

**Stratégie 3 : Helm avec values files**
```yaml
# Chart structure
charts/accounts-service/
├── Chart.yaml
├── templates/
│   ├── deployment.yaml
│   ├── configmap.yaml
│   └── secret.yaml
├── values.yaml          # Défaut
├── values-dev.yaml
├── values-staging.yaml
└── values-prod.yaml

---
# templates/configmap.yaml (template)
apiVersion: v1
kind: ConfigMap
metadata:
  name: {{ .Release.Name }}-config
data:
  DATABASE_URL: {{ .Values.database.url }}
  LOG_LEVEL: {{ .Values.logging.level }}
  REPLICAS: {{ .Values.replicas | quote }}

---
# values.yaml (défaut)
database:
  url: "jdbc:postgresql://localhost:5432/db"
logging:
  level: "INFO"
replicas: 2

---
# values-prod.yaml (override prod)
database:
  url: "jdbc:postgresql://prod-rds.eu-west-1.rds.amazonaws.com:5432/prod_db"
logging:
  level: "WARN"
replicas: 10
resources:
  requests:
    memory: "1Gi"
    cpu: "500m"
  limits:
    memory: "2Gi"
    cpu: "2000m"

# Déploiement
helm install accounts ./charts/accounts-service -f values-dev.yaml -n dev
helm install accounts ./charts/accounts-service -f values-prod.yaml -n production
```

**Stratégie 4 : External Secrets par environnement**
```yaml
# Secret Store par environnement
apiVersion: external-secrets.io/v1beta1
kind: SecretStore
metadata:
  name: aws-secrets-dev
  namespace: dev
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
apiVersion: external-secrets.io/v1beta1
kind: SecretStore
metadata:
  name: aws-secrets-prod
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
# External Secret pointant vers différents secrets AWS
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: db-credentials
  namespace: dev
spec:
  secretStoreRef:
    name: aws-secrets-dev
  data:
  - secretKey: password
    remoteRef:
      key: dev/database/credentials  # Secret AWS pour dev

---
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: db-credentials
  namespace: production
spec:
  secretStoreRef:
    name: aws-secrets-prod
  data:
  - secretKey: password
    remoteRef:
      key: prod/database/credentials  # Secret AWS pour prod
```

**Best Practices** :
```yaml
✅ Utiliser Kustomize ou Helm pour gérer multi-env
✅ Secrets sensibles dans AWS Secrets Manager/Vault
✅ GitOps avec ArgoCD pour sync automatique
✅ Namespaces séparés par environnement
✅ RBAC strict (dev ne peut pas accéder prod)
✅ Valider configs avec dry-run avant apply
❌ Ne JAMAIS commit secrets dans Git (même encodés base64)
❌ Ne pas partager les mêmes secrets entre envs
```

---

## 3. Services & Ingress Kubernetes

### Q6: Expliquez les différents types de Services K8s et donnez un cas d'usage pour chacun
**R:**

**1. ClusterIP (défaut)** - Service interne uniquement
```yaml
apiVersion: v1
kind: Service
metadata:
  name: accounts-internal
spec:
  type: ClusterIP
  selector:
    app: accounts
  ports:
  - port: 80
    targetPort: 8080

# Cas d'usage:
# - Communication entre microservices (accounts → loans)
# - Base de données (postgres service)
# - Cache Redis interne
# - Services backend non exposés publiquement

# Accès: http://accounts-internal.production.svc.cluster.local
```

**2. NodePort** - Expose sur un port du node
```yaml
apiVersion: v1
kind: Service
metadata:
  name: accounts-nodeport
spec:
  type: NodePort
  selector:
    app: accounts
  ports:
  - port: 80
    targetPort: 8080
    nodePort: 30080  # Port sur chaque node (30000-32767)

# Cas d'usage:
# - Environnement dev/test (accès direct)
# - Debugging temporaire
# - CI/CD pour tests d'intégration
# ⚠️ Rarement utilisé en production

# Accès: http://<node-ip>:30080
```

**3. LoadBalancer** - Load balancer cloud (AWS ELB/NLB)
```yaml
apiVersion: v1
kind: Service
metadata:
  name: api-gateway-public
  annotations:
    # AWS NLB (Network Load Balancer)
    service.beta.kubernetes.io/aws-load-balancer-type: "nlb"
    service.beta.kubernetes.io/aws-load-balancer-cross-zone-load-balancing-enabled: "true"
    service.beta.kubernetes.io/aws-load-balancer-backend-protocol: "http"
    # Healthcheck
    service.beta.kubernetes.io/aws-load-balancer-healthcheck-path: "/actuator/health"
    service.beta.kubernetes.io/aws-load-balancer-healthcheck-interval: "10"
spec:
  type: LoadBalancer
  selector:
    app: api-gateway
  ports:
  - name: https
    port: 443
    targetPort: 8443
    protocol: TCP
  - name: http
    port: 80
    targetPort: 8080
    protocol: TCP

# Cas d'usage:
# - API publique exposée à Internet
# - Frontend web application
# - Services devant être accessibles de l'extérieur

# AWS crée automatiquement un ELB/NLB
# Accès: http://<elb-dns-name>
```

**4. Headless Service** - Pas de ClusterIP, DNS vers pods individuels
```yaml
apiVersion: v1
kind: Service
metadata:
  name: postgres-headless
spec:
  clusterIP: None  # Headless
  selector:
    app: postgres
  ports:
  - port: 5432
    name: postgres

# Cas d'usage:
# - StatefulSets (bases de données, Kafka, Elasticsearch)
# - Service discovery de pods individuels
# - Connexion directe à un pod spécifique

# DNS résout vers IPs de tous les pods:
# postgres-0.postgres-headless.default.svc.cluster.local → 10.0.1.5
# postgres-1.postgres-headless.default.svc.cluster.local → 10.0.1.6
# postgres-2.postgres-headless.default.svc.cluster.local → 10.0.1.7
```

**5. ExternalName** - Alias vers service externe
```yaml
apiVersion: v1
kind: Service
metadata:
  name: external-payment-api
spec:
  type: ExternalName
  externalName: api.stripe.com

# Cas d'usage:
# - Référencer API externe avec nom interne
# - Migration progressive (pointer vers ancien système)
# - Abstraction du service externe

# Dans l'app Spring Boot:
# http://external-payment-api/v1/charges
# → Résolu vers api.stripe.com/v1/charges
```

**Comparaison visuelle** :
```
ClusterIP:    Pod ←→ Service (ClusterIP) ←→ Pod
              (réseau interne seulement)

NodePort:     External → Node:30080 → Service → Pod
              (chaque node expose le port)

LoadBalancer: External → Cloud LB → Service → Pod
              (AWS/GCP/Azure crée LB automatiquement)

Headless:     Client → DNS → Pod IP directement
              (pas de load balancing, DNS round-robin)
```

---

### Q7: Comment configurer un Ingress avec SSL/TLS et différents backends selon le path ?
**R:**

**Prérequis : Installation Ingress Controller**
```bash
# 1. Installer NGINX Ingress Controller
helm repo add ingress-nginx https://kubernetes.github.io/ingress-nginx
helm repo update

helm install nginx-ingress ingress-nginx/ingress-nginx \
  --namespace ingress-nginx \
  --create-namespace \
  --set controller.service.type=LoadBalancer \
  --set controller.metrics.enabled=true

# 2. Vérifier installation
kubectl get pods -n ingress-nginx
kubectl get svc -n ingress-nginx  # Noter l'External IP/DNS du LB
```

**Configuration complète Ingress multi-path avec TLS** :
```yaml
# 1. Créer certificat TLS (Let's Encrypt avec cert-manager)
apiVersion: cert-manager.io/v1
kind: Certificate
metadata:
  name: api-tls-cert
  namespace: production
spec:
  secretName: api-tls-secret  # Secret K8s créé automatiquement
  issuerRef:
    name: letsencrypt-prod
    kind: ClusterIssuer
  dnsNames:
    - api.myapp.com
    - www.api.myapp.com

---
# 2. Ingress avec routing avancé
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: microservices-ingress
  namespace: production
  annotations:
    # Ingress class
    kubernetes.io/ingress.class: nginx
    
    # Force HTTPS redirect
    nginx.ingress.kubernetes.io/force-ssl-redirect: "true"
    nginx.ingress.kubernetes.io/ssl-redirect: "true"
    
    # Rate limiting
    nginx.ingress.kubernetes.io/limit-rps: "100"
    nginx.ingress.kubernetes.io/limit-connections: "10"
    
    # CORS
    nginx.ingress.kubernetes.io/enable-cors: "true"
    nginx.ingress.kubernetes.io/cors-allow-origin: "https://myapp.com, https://www.myapp.com"
    nginx.ingress.kubernetes.io/cors-allow-methods: "GET, POST, PUT, DELETE, OPTIONS"
    nginx.ingress.kubernetes.io/cors-allow-credentials: "true"
    
    # Timeouts
    nginx.ingress.kubernetes.io/proxy-connect-timeout: "30"
    nginx.ingress.kubernetes.io/proxy-send-timeout: "30"
    nginx.ingress.kubernetes.io/proxy-read-timeout: "30"
    
    # Request size
    nginx.ingress.kubernetes.io/proxy-body-size: "10m"
    
    # URL Rewrite
    nginx.ingress.kubernetes.io/rewrite-target: /$2
    
    # Custom headers
    nginx.ingress.kubernetes.io/configuration-snippet: |
      more_set_headers "X-Frame-Options: DENY";
      more_set_headers "X-Content-Type-Options: nosniff";
      more_set_headers "X-XSS-Protection: 1; mode=block";
    
    # Whitelist IPs (optionnel)
    nginx.ingress.kubernetes.io/whitelist-source-range: "10.0.0.0/8,172.16.0.0/12"
    
    # Circuit Breaker
    nginx.ingress.kubernetes.io/upstream-fail-timeout: "10"
    nginx.ingress.kubernetes.io/upstream-max-fails: "3"

spec:
  ingressClassName: nginx
  
  # Configuration TLS/SSL
  tls:
  - hosts:
    - api.myapp.com
    secretName: api-tls-secret  # Certificat créé par cert-manager
  
  rules:
  # Règle principale pour api.myapp.com
  - host: api.myapp.com
    http:
      paths:
      
      # Accounts Service - /api/accounts/*
      - path: /api/accounts(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: accounts-service
            port:
              number: 80
      
      # Loans Service - /api/loans/*
      - path: /api/loans(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: loans-service
            port:
              number: 80
      
      # Cards Service - /api/cards/*
      - path: /api/cards(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: cards-service
            port:
              number: 80
      
      # Payments Service (avec rate limiting spécifique)
      - path: /api/payments(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: payments-service
            port:
              number: 80
      
      # API Gateway (catch-all pour autres routes)
      - path: /api(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: api-gateway
            port:
              number: 80
      
      # Health check endpoint (sans auth)
      - path: /health
        pathType: Exact
        backend:
          service:
            name: api-gateway
            port:
              number: 8081
      
      # Frontend static (SPA React/Angular)
      - path: /
        pathType: Prefix
        backend:
          service:
            name: frontend-service
            port:
              number: 80
```

**Configuration avancée : Routing basé sur headers**
```yaml
---
# Ingress pour version canary (10% du trafic)
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: accounts-canary
  namespace: production
  annotations:
    nginx.ingress.kubernetes.io/canary: "true"
    nginx.ingress.kubernetes.io/canary-weight: "10"  # 10% du trafic
    # OU canary par header
    # nginx.ingress.kubernetes.io/canary-by-header: "X-Canary"
    # nginx.ingress.kubernetes.io/canary-by-header-value: "always"
spec:
  ingressClassName: nginx
  tls:
  - hosts:
    - api.myapp.com
    secretName: api-tls-secret
  rules:
  - host: api.myapp.com
    http:
      paths:
      - path: /api/accounts
        pathType: Prefix
        backend:
          service:
            name: accounts-service-v2  # Nouvelle version
            port:
              number: 80
```

**Configuration OAuth2/Authentication**
```yaml
---
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: protected-api
  namespace: production
  annotations:
    # OAuth2 Proxy pour authentication
    nginx.ingress.kubernetes.io/auth-url: "https://oauth2-proxy.myapp.com/oauth2/auth"
    nginx.ingress.kubernetes.io/auth-signin: "https://oauth2-proxy.myapp.com/oauth2/start?rd=$escaped_request_uri"
    nginx.ingress.kubernetes.io/auth-response-headers: "X-Auth-Request-User,X-Auth-Request-Email"
spec:
  tls:
  - hosts:
    - admin.myapp.com
    secretName: admin-tls-secret
  rules:
  - host: admin.myapp.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: admin-dashboard
            port:
              number: 80
```

**Test et validation**
```bash
# 1. Vérifier configuration Ingress
kubectl describe ingress microservices-ingress -n production

# 2. Tester résolution DNS
nslookup api.myapp.com

# 3. Tester endpoints
curl -I https://api.myapp.com/api/accounts
curl -I https://api.myapp.com/api/loans

# 4. Tester redirect HTTP → HTTPS
curl -I http://api.myapp.com/api/accounts
# Devrait retourner 301/308 redirect vers HTTPS

# 5. Vérifier certificat SSL
openssl s_client -connect api.myapp.com:443 -servername api.myapp.com

# 6. Tester rate limiting
for i in {1..150}; do curl -I https://api.myapp.com/api/accounts; done
# Après 100 requêtes, devrait retourner 429 Too Many Requests

# 7. Logs NGINX Ingress
kubectl logs -n ingress-nginx deployment/nginx-ingress-controller --tail=100 -f
```

---

### Q8: Comment gérer le monitoring et la haute disponibilité de l'Ingress Controller ?
**R:**

**Haute Disponibilité Ingress**
```yaml
# Déploiement HA de NGINX Ingress
apiVersion: apps/v1
kind: Deployment
metadata:
  name: nginx-ingress-controller
  namespace: ingress-nginx
spec:
  replicas: 3  # Minimum 3 pour HA
  
  # Anti-affinity pour distribution sur différents nodes
  affinity:
    podAntiAffinity:
      requiredDuringSchedulingIgnoredDuringExecution:
      - labelSelector:
          matchExpressions:
          - key: app.kubernetes.io/name
            operator: In
            values:
            - ingress-nginx
        topologyKey: kubernetes.io/hostname
  
  template:
    spec:
      containers:
      - name: controller
        image: registry.k8s.io/ingress-nginx/controller:v1.9.0
        
        # Resources garanties
        resources:
          requests:
            cpu: 500m
            memory: 512Mi
          limits:
            cpu: 2000m
            memory: 2Gi
        
        # Probes critiques
        livenessProbe:
          httpGet:
            path: /healthz
            port: 10254
          initialDelaySeconds: 10
          periodSeconds: 10
          timeoutSeconds: 1
          successThreshold: 1
          failureThreshold: 3
        
        readinessProbe:
          httpGet:
            path: /healthz
            port: 10254
          initialDelaySeconds: 10
          periodSeconds: 10
          timeoutSeconds: 1
          successThreshold: 1
          failureThreshold: 3

---
# Service LoadBalancer avec health checks
apiVersion: v1
kind: Service
metadata:
  name: nginx-ingress-controller
  namespace: ingress-nginx
  annotations:
    service.beta.kubernetes.io/aws-load-balancer-type: "nlb"
    service.beta.kubernetes.io/aws-load-balancer-cross-zone-load-balancing-enabled: "true"
    service.beta.kubernetes.io/aws-load-balancer-healthcheck-healthy-threshold: "2"
    service.beta.kubernetes.io/aws-load-balancer-healthcheck-unhealthy-threshold: "2"
    service.beta.kubernetes.io/aws-load-balancer-healthcheck-interval: "10"
spec:
  type: LoadBalancer
  externalTrafficPolicy: Local  # Préserve source IP
  ports:
  - name: http
    port: 80
    targetPort: http
  - name: https
    port: 443
    targetPort: https
  selector:
    app.kubernetes.io/name: ingress-nginx
```

**Monitoring avec Prometheus & Grafana**
```yaml
# ServiceMonitor pour scraping Prometheus
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: nginx-ingress-metrics
  namespace: ingress-nginx
spec:
  selector:
    matchLabels:
      app.kubernetes.io/name: ingress-nginx
  endpoints:
  - port: metrics
    interval: 30s
    path: /metrics

---
# PrometheusRule pour alertes
apiVersion: monitoring.coreos.com/v1
kind: PrometheusRule
metadata:
  name: nginx-ingress-alerts
  namespace: ingress-nginx
spec:
  groups:
  - name: nginx-ingress
    interval: 30s
    rules:
    # Alerte si taux d'erreur 5xx > 5%
    - alert: HighErrorRate5xx
      expr: |
        sum(rate(nginx_ingress_controller_requests{status=~"5.."}[5m])) 
        / 
        sum(rate(nginx_ingress_controller_requests[5m])) 
        > 0.05
      for: 5m
      labels:
        severity: critical
      annotations:
        summary: "High 5xx error rate on Ingress"
        description: "Error rate is {{ $value | humanizePercentage }}"
    
    # Alerte si latence p99 > 5s
    - alert: HighLatencyP99
      expr: |
        histogram_quantile(0.99, 
          sum(rate(nginx_ingress_controller_request_duration_seconds_bucket[5m])) 
          by (le, host, path)
        ) > 5
      for: 5m
      labels:
        severity: warning
      annotations:
        summary: "High request latency (p99)"
        description: "P99 latency is {{ $value }}s for {{ $labels.host }}{{ $labels.path }}"
    
    # Alerte si Ingress Controller down
    - alert: IngressControllerDown
      expr: up{job="nginx-ingress-controller"} == 0
      for: 1m
      labels:
        severity: critical
      annotations:
        summary: "Ingress Controller is down"
        description: "{{ $labels.instance }} has been down for more than 1 minute"
    
    # Alerte sur rate limiting excessif
    - alert: TooManyRateLimited
      expr: |
        sum(rate(nginx_ingress_controller_requests{status="429"}[5m])) 
        > 100
      for: 5m
      labels:
        severity: warning
      annotations:
        summary: "High rate of 429 responses"
        description: "{{ $value }} requests/sec are being rate limited"
```

**Dashboard Grafana exemple**
```json
{
  "dashboard": {
    "title": "NGINX Ingress Controller",
    "panels": [
      {
        "title": "Request Rate",
        "targets": [
          {
            "expr": "sum(rate(nginx_ingress_controller_requests[5m])) by (host)"
          }
        ]
      },
      {
        "title": "Error Rate by Status Code",
        "targets": [
          {
            "expr": "sum(rate(nginx_ingress_controller_requests[5m])) by (status)"
          }
        ]
      },
      {
        "title": "Request Duration (p50, p95, p99)",
        "targets": [
          {
            "expr": "histogram_quantile(0.50, sum(rate(nginx_ingress_controller_request_duration_seconds_bucket[5m])) by (le))",
            "legendFormat": "p50"
          },
          {
            "expr": "histogram_quantile(0.95, sum(rate(nginx_ingress_controller_request_duration_seconds_bucket[5m])) by (le))",
            "legendFormat": "p95"
          },
          {
            "expr": "histogram_quantile(0.99, sum(rate(nginx_ingress_controller_request_duration_seconds_bucket[5m])) by (le))",
            "legendFormat": "p99"
          }
        ]
      },
      {
        "title": "Ingress Controller Memory Usage",
        "targets": [
          {
            "expr": "container_memory_usage_bytes{pod=~\"nginx-ingress-controller.*\"}"
          }
        ]
      },
      {
        "title": "SSL Certificate Expiry",
        "targets": [
          {
            "expr": "(nginx_ingress_controller_ssl_expire_time_seconds - time()) / 86400"
          }
        ]
      }
    ]
  }
}
```

---

## 4. Déploiements Avancés sur Kubernetes

### Q9: Comment implémenter un déploiement Canary avec validation automatique ?
**R:**

**Approche 1 : Ingress Canary avec validation progressive**
```yaml
# 1. Deployment version stable (blue)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: accounts-stable
  namespace: production
spec:
  replicas: 10
  selector:
    matchLabels:
      app: accounts
      version: stable
  template:
    metadata:
      labels:
        app: accounts
        version: stable
    spec:
      containers:
      - name: accounts
        image: accounts-service:v1.5.0
        ports:
        - containerPort: 8080

---
# 2. Deployment version canary
apiVersion: apps/v1
kind: Deployment
metadata:
  name: accounts-canary
  namespace: production
spec:
  replicas: 2  # Commence avec peu de replicas
  selector:
    matchLabels:
      app: accounts
      version: canary
  template:
    metadata:
      labels:
        app: accounts
        version: canary
      annotations:
        prometheus.io/scrape: "true"
        prometheus.io/path: "/actuator/prometheus"
        prometheus.io/port: "8081"
    spec:
      containers:
      - name: accounts
        image: accounts-service:v1.6.0  # Nouvelle version
        ports:
        - containerPort: 8080

---
# 3. Service principal (stable)
apiVersion: v1
kind: Service
metadata:
  name: accounts-service
spec:
  selector:
    app: accounts
    version: stable
  ports:
  - port: 80
    targetPort: 8080

---
# 4. Ingress principal (stable)
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: accounts-ingress
  namespace: production
spec:
  rules:
  - host: api.myapp.com
    http:
      paths:
      - path: /api/accounts
        pathType: Prefix
        backend:
          service:
            name: accounts-service
            port:
              number: 80

---
# 5. Ingress Canary (5% du trafic)
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: accounts-canary-ingress
  namespace: production
  annotations:
    nginx.ingress.kubernetes.io/canary: "true"
    nginx.ingress.kubernetes.io/canary-weight: "5"  # 5% seulement
spec:
  rules:
  - host: api.myapp.com
    http:
      paths:
      - path: /api/accounts
        pathType: Prefix
        backend:
          service:
            name: accounts-service-canary
            port:
              number: 80

---
# 6. Service canary
apiVersion: v1
kind: Service
metadata:
  name: accounts-service-canary
spec:
  selector:
    app: accounts
    version: canary
  ports:
  - port: 80
    targetPort: 8080
```

**Script de validation et progression automatique**
```bash
#!/bin/bash
# canary-promotion.sh

NAMESPACE="production"
CANARY_INGRESS="accounts-canary-ingress"
STABLE_DEPLOYMENT="accounts-stable"
CANARY_DEPLOYMENT="accounts-canary"

# Fonction pour obtenir les métriques
get_error_rate() {
    local version=$1
    kubectl exec -n monitoring prometheus-0 -- \
        promtool query instant \
        'sum(rate(http_requests_total{version="'$version'",status=~"5.."}[5m])) / sum(rate(http_requests_total{version="'$version'"}[5m]))'
}

get_latency_p99() {
    local version=$1
    kubectl exec -n monitoring prometheus-0 -- \
        promtool query instant \
        'histogram_quantile(0.99, sum(rate(http_request_duration_seconds_bucket{version="'$version'"}[5m])) by (le))'
}

# Phase 1: 5% du trafic
echo "Phase 1: Routing 5% traffic to canary..."
kubectl patch ingress $CANARY_INGRESS -n $NAMESPACE --type=json \
    -p='[{"op": "replace", "path": "/metadata/annotations/nginx.ingress.kubernetes.io~1canary-weight", "value": "5"}]'

sleep 300  # Attendre 5 minutes

# Validation Phase 1
echo "Validating Phase 1 metrics..."
CANARY_ERROR_RATE=$(get_error_rate "canary")
CANARY_LATENCY=$(get_latency_p99 "canary")

if (( $(echo "$CANARY_ERROR_RATE > 0.01" | bc -l) )); then
    echo "❌ Error rate too high: $CANARY_ERROR_RATE"
    echo "Rolling back..."
    kubectl delete ingress $CANARY_INGRESS -n $NAMESPACE
    exit 1
fi

if (( $(echo "$CANARY_LATENCY > 2.0" | bc -l) )); then
    echo "❌ Latency too high: $CANARY_LATENCY"
    echo "Rolling back..."
    kubectl delete ingress $CANARY_INGRESS -n $NAMESPACE
    exit 1
fi

echo "✅ Phase 1 validation passed"

# Phase 2: 25% du trafic
echo "Phase 2: Routing 25% traffic to canary..."
kubectl patch ingress $CANARY_INGRESS -n $NAMESPACE --type=json \
    -p='[{"op": "replace", "path": "/metadata/annotations/nginx.ingress.kubernetes.io~1canary-weight", "value": "25"}]'

# Scale up canary
kubectl scale deployment $CANARY_DEPLOYMENT -n $NAMESPACE --replicas=5

sleep 600  # Attendre 10 minutes

# Validation Phase 2 (même logique)
echo "Validating Phase 2 metrics..."
# ... (même validation)

echo "✅ Phase 2 validation passed"

# Phase 3: 50% du trafic
echo "Phase 3: Routing 50% traffic to canary..."
kubectl patch ingress $CANARY_INGRESS -n $NAMESPACE --type=json \
    -p='[{"op": "replace", "path": "/metadata/annotations/nginx.ingress.kubernetes.io~1canary-weight", "value": "50"}]'

kubectl scale deployment $CANARY_DEPLOYMENT -n $NAMESPACE --replicas=10

sleep 600

# Validation Phase 3
echo "Validating Phase 3 metrics..."
# ... (même validation)

echo "✅ Phase 3 validation passed"

# Phase 4: 100% promotion
echo "Phase 4: Full promotion to canary..."

# Basculer le service principal vers canary
kubectl patch service accounts-service -n $NAMESPACE --type=json \
    -p='[{"op": "replace", "path": "/spec/selector/version", "value": "canary"}]'

# Supprimer Ingress canary
kubectl delete ingress $CANARY_INGRESS -n $NAMESPACE

# Supprimer ancien deployment stable après validation
sleep 300
kubectl delete deployment $STABLE_DEPLOYMENT -n $NAMESPACE

# Renommer canary en stable
kubectl patch deployment $CANARY_DEPLOYMENT -n $NAMESPACE --type=json \
    -p='[{"op": "replace", "path": "/metadata/name", "value": "accounts-stable"}]'

echo "✅ Canary promotion completed successfully!"
```

**Approche 2 : Flagger (automatisation canary)**
```yaml
# Installation Flagger
kubectl apply -k github.com/fluxcd/flagger//kustomize/linkerd

---
# Canary resource (Flagger)
apiVersion: flagger.app/v1beta1
kind: Canary
metadata:
  name: accounts-canary
  namespace: production
spec:
  # Deployment à déployer en canary
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: accounts-service
  
  # Service
  service:
    port: 80
    targetPort: 8080
  
  # Analyse automatique
  analysis:
    # Intervalle entre chaque step
    interval: 1m
    
    # Seuil de succès avant progression
    threshold: 5
    
    # Nombre max d'échecs avant rollback
    maxWeight: 50
    
    # Incrémentation du trafic
    stepWeight: 10  # 10%, 20%, 30%...
    
    # Métriques à valider
    metrics:
    - name: request-success-rate
      thresholdRange:
        min: 99  # Min 99% de succès
      interval: 1m
    
    - name: request-duration
      thresholdRange:
        max: 500  # Max 500ms latency
      interval: 1m
    
    # Webhooks pour tests custom
    webhooks:
    - name: load-test
      url: http://flagger-loadtester.test/
      timeout: 5s
      metadata:
        type: cmd
        cmd: "hey -z 1m -q 10 -c 2 http://accounts-service-canary/api/accounts"
    
    - name: integration-test
      url: http://integration-tests.test/run
      timeout: 30s

  # Alertes
  alerts:
  - name: slack
    severity: info
    providerRef:
      name: slack-notifier
```

---

### Q10: Comment gérer les StatefulSets pour des bases de données sur Kubernetes ?
**R:**

**Exemple : PostgreSQL StatefulSet**
```yaml
# 1. StorageClass pour volumes persistants
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: fast-ssd
provisioner: kubernetes.io/aws-ebs
parameters:
  type: gp3
  iops: "3000"
  throughput: "125"
allowVolumeExpansion: true
reclaimPolicy: Retain  # ⚠️ Ne pas supprimer les données

---
# 2. Headless Service pour service discovery
apiVersion: v1
kind: Service
metadata:
  name: postgres-headless
  namespace: database
spec:
  clusterIP: None  # Headless
  selector:
    app: postgres
  ports:
  - name: postgres
    port: 5432
    targetPort: 5432

---
# 3. Service LoadBalancer pour accès externe (optionnel)
apiVersion: v1
kind: Service
metadata:
  name: postgres-lb
  namespace: database
spec:
  type: LoadBalancer
  selector:
    app: postgres
    statefulset.kubernetes.io/pod-name: postgres-0  # Master uniquement
  ports:
  - port: 5432
    targetPort: 5432

---
# 4. StatefulSet PostgreSQL
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: postgres
  namespace: database
spec:
  serviceName: postgres-headless
  replicas: 3  # 1 master + 2 replicas
  
  selector:
    matchLabels:
      app: postgres
  
  template:
    metadata:
      labels:
        app: postgres
    spec:
      # Anti-affinity pour distribution
      affinity:
        podAntiAffinity:
          requiredDuringSchedulingIgnoredDuringExecution:
          - labelSelector:
              matchExpressions:
              - key: app
                operator: In
                values:
                - postgres
            topologyKey: kubernetes.io/hostname
      
      containers:
      - name: postgres
        image: postgres:15-alpine
        ports:
        - containerPort: 5432
          name: postgres
        
        # Variables d'environnement depuis Secret
        env:
        - name: POSTGRES_PASSWORD
          valueFrom:
            secretKeyRef:
              name: postgres-secret
              key: password
        - name: POSTGRES_USER
          value: "admin"
        - name: POSTGRES_DB
          value: "myappdb"
        - name: PGDATA
          value: /var/lib/postgresql/data/pgdata
        
        # Resources
        resources:
          requests:
            memory: "2Gi"
            cpu: "1000m"
          limits:
            memory: "4Gi"
            cpu: "2000m"
        
        # Volume mount
        volumeMounts:
        - name: postgres-storage
          mountPath: /var/lib/postgresql/data
        
        # Probes
        livenessProbe:
          exec:
            command:
            - /bin/sh
            - -c
            - pg_isready -U admin
          initialDelaySeconds: 30
          periodSeconds: 10
        
        readinessProbe:
          exec:
            command:
            - /bin/sh
            - -c
            - pg_isready -U admin
          initialDelaySeconds: 5
          periodSeconds: 5
  
  # Volume Claim Template (créé automatiquement pour chaque pod)
  volumeClaimTemplates:
  - metadata:
      name: postgres-storage
    spec:
      accessModes: [ "ReadWriteOnce" ]
      storageClassName: fast-ssd
      resources:
        requests:
          storage: 100Gi

---
# 5. Secret pour credentials
apiVersion: v1
kind: Secret
metadata:
  name: postgres-secret
  namespace: database
type: Opaque
data:
  password: c3VwZXJTZWNyZXRQYXNzd29yZA==  # base64 encoded
```

**Connexion depuis Spring Boot**
```yaml
# ConfigMap pour Spring Boot
apiVersion: v1
kind: ConfigMap
metadata:
  name: app-config
data:
  application.yaml: |
    spring:
      datasource:
        # Master pour writes
        url: jdbc:postgresql://postgres-0.postgres-headless.database.svc.cluster.local:5432/myappdb
        username: admin
        password: ${DB_PASSWORD}
        
        hikari:
          maximum-pool-size: 20
          connection-timeout: 30000
      
      # Lecture depuis replicas (load balancing)
      datasource-read:
        url: jdbc:postgresql://postgres-headless.database.svc.cluster.local:5432/myappdb
        username: admin
        password: ${DB_PASSWORD}
```

```java
@Configuration
public class DatabaseConfig {
    
    @Bean
    @Primary
    @ConfigurationProperties("spring.datasource")
    public DataSource writeDataSource() {
        return DataSourceBuilder.create().build();
    }
    
    @Bean
    @ConfigurationProperties("spring.datasource-read")
    public DataSource readDataSource() {
        return DataSourceBuilder.create().build();
    }
    
    @Bean
    public DataSource routingDataSource(
        @Qualifier("writeDataSource") DataSource writeDS,
        @Qualifier("readDataSource") DataSource readDS) {
        
        Map<Object, Object> targetDataSources = new HashMap<>();
        targetDataSources.put(DataSourceType.WRITE, writeDS);
        targetDataSources.put(DataSourceType.READ, readDS);
        
        RoutingDataSource routingDataSource = new RoutingDataSource();
        routingDataSource.setTargetDataSources(targetDataSources);
        routingDataSource.setDefaultTargetDataSource(writeDS);
        
        return routingDataSource;
    }
}
```

**Backup automatique**
```yaml
# CronJob pour backup PostgreSQL
apiVersion: batch/v1
kind: CronJob
metadata:
  name: postgres-backup
  namespace: database
spec:
  schedule: "0 2 * * *"  # Tous les jours à 2h du matin
  jobTemplate:
    spec:
      template:
        spec:
          containers:
          - name: backup
            image: postgres:15-alpine
            env:
            - name: PGPASSWORD
              valueFrom:
                secretKeyRef:
                  name: postgres-secret
                  key: password
            command:
            - /bin/sh
            - -c
            - |
              BACKUP_FILE="/backup/postgres-$(date +%Y%m%d-%H%M%S).sql.gz"
              pg_dump -h postgres-0.postgres-headless.database.svc.cluster.local \
                      -U admin myappdb | gzip > $BACKUP_FILE
              
              # Upload vers S3
              aws s3 cp $BACKUP_FILE s3://my-backups/postgres/
              
              # Garder seulement les 7 derniers jours localement
              find /backup -name "postgres-*.sql.gz" -mtime +7 -delete
            
            volumeMounts:
            - name: backup-storage
              mountPath: /backup
          
          restartPolicy: OnFailure
          
          volumes:
          - name: backup-storage
            persistentVolumeClaim:
              claimName: backup-pvc
```

**Scaling et maintenance**
```bash
# Scale up (ajouter un replica)
kubectl scale statefulset postgres -n database --replicas=4

# Rolling update (mise à jour version)
kubectl set image statefulset/postgres postgres=postgres:16-alpine -n database
# StatefulSet update un pod à la fois: postgres-2, puis postgres-1, puis postgres-0

# Vérifier status
kubectl rollout status statefulset/postgres -n database

# Accéder à un pod spécifique
kubectl exec -it postgres-0 -n database -- psql -U admin -d myappdb

# Restaurer un backup
kubectl exec -it postgres-0 -n database -- /bin/bash
gunzip < /backup/postgres-20250101-020000.sql.gz | psql -U admin -d myappdb

# Vérifier les volumes persistants
kubectl get pvc -n database
kubectl describe pvc postgres-storage-postgres-0 -n database

# Supprimer un pod (sera recréé automatiquement avec même volume)
kubectl delete pod postgres-1 -n database
# Le pod redémarre avec les mêmes données (PVC conservé)

# Disaster recovery - recréer depuis backup
kubectl delete statefulset postgres -n database
# Les PVCs sont conservés (reclaimPolicy: Retain)
kubectl apply -f postgres-statefulset.yaml
# Les pods remontent avec les données existantes
```

---

### Q11: Comment gérer les secrets sensibles de manière vraiment sécurisée sur K8s (pas juste base64) ?
**R:**

**Problème avec Secrets K8s natifs** :
```bash
# N'importe qui avec accès kubectl peut décoder
kubectl get secret db-password -o jsonpath='{.data.password}' | base64 -d
# → "superSecretPassword123"

# Secrets visibles dans etcd sans encryption by default
```

**Solution 1 : External Secrets Operator + AWS Secrets Manager (Recommandé)**
```yaml
# 1. Installation External Secrets Operator
helm repo add external-secrets https://charts.external-secrets.io
helm install external-secrets external-secrets/external-secrets \
  -n external-secrets-system --create-namespace

---
# 2. IAM Role pour ServiceAccount (IRSA)
apiVersion: v1
kind: ServiceAccount
metadata:
  name: external-secrets-sa
  namespace: production
  annotations:
    eks.amazonaws.com/role-arn: arn:aws:iam::123456789:role/ExternalSecretsRole

---
# 3. SecretStore (connexion vers AWS Secrets Manager)
apiVersion: external-secrets.io/v1beta1
kind: SecretStore
metadata:
  name: aws-secrets-store
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
# 4. ExternalSecret (sync automatique depuis AWS)
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: database-credentials
  namespace: production
spec:
  # Refresh toutes les 5 minutes
  refreshInterval: 5m
  
  secretStoreRef:
    name: aws-secrets-store
    kind: SecretStore
  
  target:
    name: db-credentials  # Nom du Secret K8s créé
    creationPolicy: Owner
    template:
      engineVersion: v2
      data:
        # Template pour formater le secret
        DATABASE_URL: "postgresql://{{ .username }}:{{ .password }}@postgres-service:5432/mydb"
  
  # Données à récupérer depuis AWS Secrets Manager
  data:
  - secretKey: username
    remoteRef:
      key: prod/database/credentials  # Nom dans AWS
      property: username
  
  - secretKey: password
    remoteRef:
      key: prod/database/credentials
      property: password
  
  - secretKey: ssl-cert
    remoteRef:
      key: prod/database/ssl-cert

---
# 5. Utilisation dans le Pod
apiVersion: v1
kind: Pod
metadata:
  name: accounts-pod
spec:
  containers:
  - name: app
    image: accounts-service:latest
    env:
    - name: DATABASE_URL
      valueFrom:
        secretKeyRef:
          name: db-credentials  # Secret créé par ExternalSecret
          key: DATABASE_URL
```

**Création du secret dans AWS**
```bash
# Créer secret dans AWS Secrets Manager
aws secretsmanager create-secret \
  --name prod/database/credentials \
  --description "Production database credentials" \
  --secret-string '{
    "username": "admin",
    "password": "VerySecurePassword123!",
    "host": "prod-db.cluster-abc.eu-west-1.rds.amazonaws.com",
    "port": "5432"
  }' \
  --region eu-west-1

# Rotation automatique du secret (Lambda)
aws secretsmanager rotate-secret \
  --secret-id prod/database/credentials \
  --rotation-lambda-arn arn:aws:lambda:eu-west-1:123456789:function:SecretsManagerRotation \
  --rotation-rules AutomaticallyAfterDays=30
```

**Solution 2 : HashiCorp Vault (pour multi-cloud)**
```yaml
# 1. Installation Vault
helm repo add hashicorp https://helm.releases.hashicorp.com
helm install vault hashicorp/vault \
  --namespace vault --create-namespace \
  --set "server.ha.enabled=true" \
  --set "server.ha.replicas=3"

---
# 2. Configuration Vault K8s Auth
# Dans Vault CLI:
vault auth enable kubernetes

vault write auth/kubernetes/config \
  kubernetes_host="https://$KUBERNETES_PORT_443_TCP_ADDR:443" \
  token_reviewer_jwt="$(cat /var/run/secrets/kubernetes.io/serviceaccount/token)" \
  kubernetes_ca_cert=@/var/run/secrets/kubernetes.io/serviceaccount/ca.crt

# Créer policy
vault policy write accounts-policy - <<EOF
path "secret/data/accounts/*" {
  capabilities = ["read"]
}
EOF

# Créer role K8s
vault write auth/kubernetes/role/accounts \
  bound_service_account_names=accounts-sa \
  bound_service_account_namespaces=production \
  policies=accounts-policy \
  ttl=24h

---
# 3. Vault Secrets Operator
apiVersion: secrets.hashicorp.com/v1beta1
kind: VaultStaticSecret
metadata:
  name: database-credentials
  namespace: production
spec:
  vaultAuthRef: vault-auth
  mount: secret
  path: accounts/database
  
  # Refresh interval
  refreshAfter: 5m
  
  destination:
    create: true
    name: db-vault-secret
  
  # Rollout restart automatique si secret change
  rolloutRestartTargets:
  - kind: Deployment
    name: accounts-service

---
# 4. ServiceAccount avec accès Vault
apiVersion: v1
kind: ServiceAccount
metadata:
  name: accounts-sa
  namespace: production

---
# 5. Deployment utilisant Vault
apiVersion: apps/v1
kind: Deployment
metadata:
  name: accounts-service
spec:
  template:
    spec:
      serviceAccountName: accounts-sa
      
      containers:
      - name: accounts
        image: accounts-service:latest
        
        # Option 1: Via Secret K8s (créé par Vault Operator)
        env:
        - name: DB_PASSWORD
          valueFrom:
            secretKeyRef:
              name: db-vault-secret
              key: password
        
        # Option 2: Vault Agent Sidecar Injector
        # annotations automatiques:
        # vault.hashicorp.com/agent-inject: "true"
        # vault.hashicorp.com/role: "accounts"
        # vault.hashicorp.com/agent-inject-secret-db: "secret/accounts/database"
```

**Vault Agent Sidecar (injection automatique)**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: accounts-service
spec:
  template:
    metadata:
      annotations:
        # Active l'injection Vault Agent
        vault.hashicorp.com/agent-inject: "true"
        vault.hashicorp.com/role: "accounts"
        
        # Injecter le secret dans un fichier
        vault.hashicorp.com/agent-inject-secret-database.txt: "secret/accounts/database"
        
        # Template pour formater le secret
        vault.hashicorp.com/agent-inject-template-database.txt: |
          {{- with secret "secret/accounts/database" -}}
          spring.datasource.url=jdbc:postgresql://{{ .Data.data.host }}:5432/mydb
          spring.datasource.username={{ .Data.data.username }}
          spring.datasource.password={{ .Data.data.password }}
          {{- end }}
    
    spec:
      containers:
      - name: accounts
        image: accounts-service:latest
        
        # Le fichier est monté dans /vault/secrets/database.txt
        command:
        - java
        - -jar
        - app.jar
        - --spring.config.import=file:/vault/secrets/database.txt
```

**Solution 3 : Sealed Secrets (secrets chiffrés dans Git)**
```bash
# 1. Installation Sealed Secrets Controller
kubectl apply -f https://github.com/bitnami-labs/sealed-secrets/releases/download/v0.24.0/controller.yaml

# 2. Installation kubeseal CLI
wget https://github.com/bitnami-labs/sealed-secrets/releases/download/v0.24.0/kubeseal-linux-amd64
chmod +x kubeseal-linux-amd64
sudo mv kubeseal-linux-amd64 /usr/local/bin/kubeseal

# 3. Créer un secret normal
kubectl create secret generic db-credentials \
  --from-literal=username=admin \
  --from-literal=password=superSecret123 \
  --dry-run=client -o yaml > secret.yaml

# 4. Sceller le secret (chiffrement avec clé publique du cluster)
kubeseal --format yaml < secret.yaml > sealed-secret.yaml

# 5. Le SealedSecret peut être commité dans Git
cat sealed-secret.yaml
```

```yaml
# sealed-secret.yaml (sûr de commiter dans Git)
apiVersion: bitnami.com/v1alpha1
kind: SealedSecret
metadata:
  name: db-credentials
  namespace: production
spec:
  encryptedData:
    username: AgBghQ7Xmv8... # Chiffré avec clé publique
    password: AgC8H3nK9s2... # Impossible à déchiffrer sans clé privée du cluster
  template:
    metadata:
      name: db-credentials
      namespace: production

# Apply dans le cluster
kubectl apply -f sealed-secret.yaml

# Le controller déchiffre automatiquement et crée le Secret K8s
# Seul le cluster peut déchiffrer (clé privée dans le controller)
```

**Comparaison des solutions**

| Solution | Avantages | Inconvénients | Use Case |
|----------|-----------|---------------|----------|
| **External Secrets + AWS** | ✅ Natif AWS<br>✅ Rotation auto<br>✅ Audit CloudTrail | ❌ Lock-in AWS<br>❌ Coût AWS | Production AWS |
| **Vault** | ✅ Multi-cloud<br>✅ Dynamic secrets<br>✅ Très flexible | ❌ Complexité setup<br>❌ Maintenance Vault | Enterprise, multi-cloud |
| **Sealed Secrets** | ✅ GitOps friendly<br>✅ Simple<br>✅ Pas d'infra externe | ❌ Pas de rotation auto<br>❌ Backup clé critique | Startups, petits projets |
| **KMS Encryption** | ✅ Natif K8s<br>✅ Transparent | ❌ Secrets visibles avec kubectl<br>❌ Pas de rotation | Baseline minimum |

**Best Practice : Combinaison**
```yaml
# Stratégie recommandée:
1. External Secrets Operator + AWS Secrets Manager (secrets sensibles)
2. ConfigMaps pour config non-sensible
3. Sealed Secrets pour secrets moins critiques (API keys non-prod)
4. KMS encryption at rest activé (baseline)
5. RBAC strict (qui peut lire les secrets?)
6. Audit logging (qui a accédé aux secrets?)
7. Rotation automatique des secrets (30-90 jours)
```

---

### Q12: Comment débugger un pod qui crashloop ou ne démarre pas sur Kubernetes ?
**R:**

**Méthodologie complète de debugging**

**1. Vérifier l'état du Pod**
```bash
# Status général
kubectl get pods -n production
# NAME                    READY   STATUS             RESTARTS   AGE
# accounts-5d7f8-xyz      0/1     CrashLoopBackOff   5          3m

# Détails complets
kubectl describe pod accounts-5d7f8-xyz -n production

# Points à regarder:
# - Events (en bas): erreurs de pull image, OOMKilled, etc.
# - Containers.State: waiting/terminated avec raison
# - Conditions: voir si Initialized, Ready, etc.
```

**2. Logs du container**
```bash
# Logs actuels
kubectl logs accounts-5d7f8-xyz -n production

# Logs du container précédent (si crashé)
kubectl logs accounts-5d7f8-xyz -n production --previous

# Suivre les logs en temps réel
kubectl logs -f accounts-5d7f8-xyz -n production

# Logs de tous les containers d'un pod (si multi-container)
kubectl logs accounts-5d7f8-xyz -n production --all-containers=true

# Logs avec timestamp
kubectl logs accounts-5d7f8-xyz -n production --timestamps=true

# Dernières X lignes
kubectl logs accounts-5d7f8-xyz -n production --tail=100
```

**3. Problèmes courants et solutions**

**A. ImagePullBackOff / ErrImagePull**
```bash
# Problème: Cannot pull image

# Vérifier le nom de l'image
kubectl describe pod accounts-5d7f8-xyz -n production | grep Image

# Solutions possibles:
# 1. Image n'existe pas
docker pull myregistry.io/accounts-service:v1.2.0  # Tester manuellement

# 2. Problème d'authentification registry
kubectl get secret regcred -n production -o yaml
# Créer/mettre à jour le secret
kubectl create secret docker-registry regcred \
  --docker-server=myregistry.io \
  --docker-username=myuser \
  --docker-password=mypass \
  --docker-email=email@example.com \
  -n production

# Ajouter dans le Deployment
spec:
  template:
    spec:
      imagePullSecrets:
      - name: regcred

# 3. Registry privé AWS ECR
aws ecr get-login-password --region eu-west-1 | \
  docker login --username AWS --password-stdin 123456789.dkr.ecr.eu-west-1.amazonaws.com
```

**B. CrashLoopBackOff (application crash au démarrage)**
```bash
# Logs montrent l'erreur
kubectl logs accounts-5d7f8-xyz -n production --previous

# Erreurs Spring Boot communes:

# 1. Cannot connect to database
# Solution: Vérifier ConfigMap/Secret
kubectl get configmap app-config -n production -o yaml
kubectl get secret db-credentials -n production -o jsonpath='{.data.password}' | base64 -d

# Tester connectivité DB depuis un pod temporaire
kubectl run -it --rm debug --image=postgres:15 --restart=Never -- \
  psql -h postgres-service -U admin -d mydb

# 2. Port already in use (mauvaise config)
# Vérifier dans Deployment
kubectl get deployment accounts-service -n production -o yaml | grep containerPort

# 3. OutOfMemory (OOMKilled)
# Augmenter les limites mémoire
kubectl set resources deployment accounts-service \
  --limits=memory=2Gi \
  --requests=memory=1Gi \
  -n production

# 4. Missing environment variable
kubectl exec -it accounts-5d7f8-xyz -n production -- env | grep DATABASE
```

**C. Pending (pod ne démarre jamais)**
```bash
kubectl describe pod accounts-5d7f8-xyz -n production

# Raisons courantes:

# 1. Insufficient CPU/Memory (pas de node avec ressources suffisantes)
# Events: 0/3 nodes available: insufficient memory
kubectl top nodes  # Voir utilisation
kubectl describe nodes  # Voir capacité

# Solution: Scale down autres apps ou add nodes
kubectl scale deployment other-app --replicas=1

# 2. PVC Pending (volume pas créé)
kubectl get pvc -n production
# Solution: Vérifier StorageClass existe
kubectl get storageclass

# 3. Node selector/affinity incompatible
# Vérifier si le deployment a des contraintes
kubectl get deployment accounts-service -n production -o yaml | grep -A 10 nodeSelector

# Lister les labels des nodes
kubectl get nodes --show-labels
```

**D. Init Container Failed**
```bash
# Vérifier état des init containers
kubectl describe pod accounts-5d7f8-xyz -n production | grep -A 20 "Init Containers"

# Logs de l'init container
kubectl logs accounts-5d7f8-xyz -n production -c init-db-migration

# Exemple: migration DB qui échoue
# Solution: Vérifier script migration, permissions DB
```

**4. Debugging interactif**

**Exec dans un container running**
```bash
# Shell interactif
kubectl exec -it accounts-5d7f8-xyz -n production -- /bin/bash

# Commandes utiles dans le container:
# Vérifier processus Java
ps aux | grep java

# Tester connectivité réseau
curl http://postgres-service:5432
nc -zv postgres-service 5432

# Vérifier DNS
nslookup postgres-service
cat /etc/resolv.conf

# Vérifier variables d'env
env | grep DATABASE

# Vérifier fichiers montés
ls -la /config
cat /config/application.yaml

# Thread dump Java (si app bloquée)
kill -3 $(pgrep java)
# Logs apparaissent dans kubectl logs
```

**Debug container pour pod qui crash immédiatement**
```yaml
# Technique: Override entrypoint pour garder container vivant
apiVersion: v1
kind: Pod
metadata:
  name: accounts-debug
spec:
  containers:
  - name: accounts
    image: accounts-service:v1.2.0
    command: ["/bin/sh"]
    args: ["-c", "sleep 3600"]  # Garde le container alive 1h
    
    env:
    - name: DATABASE_URL
      value: "jdbc:postgresql://postgres:5432/mydb"

# Apply et exec dedans
kubectl apply -f accounts-debug.yaml
kubectl exec -it accounts-debug -- /bin/bash

# Maintenant on peut debugger manuellement:
java -jar /app/app.jar
# Voir l'erreur exacte
```

**Ephemeral Debug Container (K8s 1.23+)**
```bash
# Ajouter un container debug à un pod existant
kubectl debug accounts-5d7f8-xyz -it --image=busybox:1.28 -n production

# Ou avec une image plus complète
kubectl debug accounts-5d7f8-xyz -it \
  --image=nicolaka/netshoot \
  --target=accounts \
  -n production

# Permet de debugger sans modifier le Deployment
```

**5. Network debugging**
```bash
# Pod de debug avec outils réseau
kubectl run netshoot --rm -it --image=nicolaka/netshoot -- /bin/bash

# Dans le pod:
# Tester connectivité
ping postgres-service
curl -v http://accounts-service/actuator/health

# DNS lookup
nslookup accounts-service.production.svc.cluster.local

# Tracer route
traceroute accounts-service

# Port scan
nmap -p 8080 accounts-service

# Tester depuis l'extérieur du cluster
kubectl port-forward pod/accounts-5d7f8-xyz 8080:8080 -n production
# Dans un autre terminal:
curl http://localhost:8080/actuator/health
```

**6. Events et monitoring**
```bash
# Voir tous les events récents
kubectl get events -n production --sort-by='.lastTimestamp'

# Events pour un pod spécifique
kubectl get events -n production --field-selector involvedObject.name=accounts-5d7f8-xyz

# Métriques des pods
kubectl top pods -n production
kubectl top pod accounts-5d7f8-xyz -n production --containers

# Voir les probes qui échouent
kubectl describe pod accounts-5d7f8-xyz -n production | grep -A 5 "Liveness\|Readiness"
```

**7. Problèmes de configuration Spring Boot**
```bash
# Vérifier profile actif
kubectl exec accounts-5d7f8-xyz -n production -- \
  curl http://localhost:8081/actuator/env | jq '.propertySources[] | select(.name | contains("application"))'

# Vérifier properties chargées
kubectl exec accounts-5d7f8-xyz -n production -- \
  curl http://localhost:8081/actuator/configprops

# Health check détaillé
kubectl exec accounts-5d7f8-xyz -n production -- \
  curl http://localhost:8081/actuator/health/readiness

# Beans chargés
kubectl exec accounts-5d7f8-xyz -n production -- \
  curl http://localhost:8081/actuator/beans
```

**Checklist complète de debugging**
```yaml
✅ Status du pod (get, describe)
✅ Logs actuels et précédents
✅ Events du namespace
✅ Resources (CPU/RAM) suffisantes?
✅ Image existe et pullable?
✅ ConfigMaps/Secrets montés correctement?
✅ Variables d'environnement correctes?
✅ Connectivité réseau (DB, autres services)?
✅ DNS fonctionne?
✅ Probes (liveness/readiness) appropriées?
✅ Permissions (RBAC, SecurityContext)?
✅ PVC bindé correctement?
✅ Init containers réussis?
```

**Script automatisé de debugging**
```bash
#!/bin/bash
# k8s-debug.sh

POD_NAME=$1
NAMESPACE=${2:-default}

echo "=== Pod Status ==="
kubectl get pod $POD_NAME -n $NAMESPACE

echo -e "\n=== Pod Description ==="
kubectl describe pod $POD_NAME -n $NAMESPACE

echo -e "\n=== Current Logs ==="
kubectl logs $POD_NAME -n $NAMESPACE --tail=50

echo -e "\n=== Previous Logs (if crashed) ==="
kubectl logs $POD_NAME -n $NAMESPACE --previous --tail=50 2>/dev/null || echo "No previous logs"

echo -e "\n=== Events ==="
kubectl get events -n $NAMESPACE --field-selector involvedObject.name=$POD_NAME

echo -e "\n=== Resource Usage ==="
kubectl top pod $POD_NAME -n $NAMESPACE --containers 2>/dev/null || echo "Metrics unavailable"

echo -e "\n=== ConfigMaps ==="
kubectl get configmaps -n $NAMESPACE

echo -e "\n=== Secrets ==="
kubectl get secrets -n $NAMESPACE

# Usage: ./k8s-debug.sh accounts-5d7f8-xyz production
```

Cette approche méthodique permet de résoudre 95% des problèmes de pods qui ne démarrent pas !