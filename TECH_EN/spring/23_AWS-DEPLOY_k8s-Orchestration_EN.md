## [**..**](./00_index.md)


## Cloud Deployment and Orchestration (AWS & Kubernetes)

## 1. Deploying Spring Boot on AWS Cloud

### Cloud Native Architecture on AWS
- **Design** : Deploying Spring Boot applications on the AWS ecosystem with a multi-tier architecture
- **Mastered AWS Services** :
  - **EC2** : Traditional deployment with Auto Scaling Groups for high availability
  - **ECS (Elastic Container Service)** : Docker container orchestration with Fargate (serverless)
  - **EKS (Elastic Kubernetes Service)** : Managed Kubernetes cluster for advanced orchestration
  - **RDS** : Relational databases (PostgreSQL, MySQL) with Multi-AZ for resilience
  - **ElastiCache** : Redis/Memcached for distributed caching
  - **S3** : Object storage for static files and backups
  - **CloudWatch** : Monitoring, logs, and application metrics
  - **ALB/NLB** : Application/Network Load Balancers for traffic distribution
  - **Route 53** : DNS and intelligent routing
  - **Secrets Manager** : Secure management of credentials and sensitive configurations
  - **IAM** : Access and permission management with least privilege principle

### CI/CD Pipeline on AWS
- **Strategy** : Full deployment automation with a CI/CD pipeline
- **Tools used** :
  - **CodePipeline** : End-to-end pipeline orchestration
  - **CodeBuild** : Compilation, tests, and Docker image creation
  - **ECR (Elastic Container Registry)** : Private registry to store Docker images
  - **CodeDeploy** : Blue/Green or Rolling deployment for zero downtime
- **Example workflow** :
  1. Push code to GitHub/GitLab
  2. CodePipeline triggers automatically
  3. CodeBuild runs tests and builds the Docker image
  4. Push image to ECR
  5. CodeDeploy deploys to ECS/EKS with Blue/Green strategy
  6. Automatic health checks and rollback on failure

### Spring Boot Configuration for AWS
```yaml
# Application optimized for AWS
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
        instance-profile: true  # Using IAM Role
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

### Security and AWS Best Practices
- **Secrets Management** : No hardcoded secrets in code, using AWS Secrets Manager
- **Network Security** : VPC with public/private subnets, restrictive Security Groups
- **IAM Roles** : Assigning IAM roles to EC2/ECS task instances (no static credentials)
- **Encryption** : Data encrypted at-rest (RDS, S3) and in-transit (TLS/SSL)
- **Backup & Disaster Recovery** : Automatic RDS snapshots, cross-region replication if critical

---

## 2. Container Orchestration with Kubernetes

### Deploying Spring Boot Applications on K8s

#### Kubernetes Architecture
- **Clusters** : Configuration and management of Kubernetes clusters (EKS on AWS, GKE, or on-premise)
- **Namespaces** : Logical isolation per environment (dev, staging, production)
- **Deployment strategy** : Rolling updates, Blue/Green, Canary deployments

#### Deployment & Pods
```yaml
# Example Deployment for Spring Boot
apiVersion: apps/v1
kind: Deployment
metadata:
  name: accounts-service
  namespace: production
  labels:
    app: accounts
    version: v1.2.0
spec:
  replicas: 3  # High availability
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
        
        # Resource configuration
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
        
        # Environment variables from ConfigMaps/Secrets
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

### Managing ConfigMaps & Secrets

#### ConfigMaps - Externalized Configuration
- **Purpose** : Separate application configuration from code
- **Usage** : Non-sensitive properties (URLs, timeouts, feature flags)

```yaml
# ConfigMap for application configuration
apiVersion: v1
kind: ConfigMap
metadata:
  name: accounts-config
  namespace: production
data:
  # Spring Boot configuration
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
  
  # Simple environment variables
  LOG_LEVEL: "INFO"
  FEATURE_NEW_API: "true"
  MAX_RETRY_ATTEMPTS: "3"

---
# Mount as a file
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

# Spring Boot automatically reads from /config/application.yaml
```

#### Secrets - Secure Management of Sensitive Data
- **Purpose** : Store credentials, tokens, certificates securely
- **Encryption at rest** : Secrets encrypted in etcd with KMS (AWS, GCP, Azure)

```yaml
# Secret for database credentials
apiVersion: v1
kind: Secret
metadata:
  name: accounts-secrets
  namespace: production
type: Opaque
data:
  # Base64-encoded values
  DB_USERNAME: YWRtaW4=  # admin
  DB_PASSWORD: c3VwZXJTZWNyZXQxMjM=  # superSecret123
  JWT_SECRET: bXlTdXBlclNlY3JldEtleUZvckpXVA==
  REDIS_PASSWORD: cmVkaXNQYXNzd29yZA==

---
# Secret from file (for TLS certificates)
apiVersion: v1
kind: Secret
metadata:
  name: tls-secret
type: kubernetes.io/tls
data:
  tls.crt: LS0tLS1CRUdJTi...  # Certificate
  tls.key: LS0tLS1CRUdJTi...  # Private key

---
# Usage in the Pod
apiVersion: v1
kind: Pod
metadata:
  name: accounts-pod
spec:
  containers:
  - name: app
    image: accounts-service:latest
    env:
    # Secret as environment variable
    - name: DB_PASSWORD
      valueFrom:
        secretKeyRef:
          name: accounts-secrets
          key: DB_PASSWORD
    
    # Or mount as a file
    volumeMounts:
    - name: secret-volume
      mountPath: /secrets
      readOnly: true
  volumes:
  - name: secret-volume
    secret:
      secretName: accounts-secrets
```

**Secrets best practices** :
- ✅ Use **External Secrets Operator** to sync with AWS Secrets Manager/Vault
- ✅ Automatic secret rotation
- ✅ Strict RBAC (least privilege principle)
- ✅ Audit logging of secret access
- ❌ Never put secrets in Docker images or ConfigMaps

---

### Kubernetes Services

#### Service Types
- **ClusterIP** : Internal communication between microservices
- **NodePort** : Exposure on a node port (dev/test)
- **LoadBalancer** : External exposure via cloud Load Balancer (AWS ELB)
- **Headless Service** : Individual pod discovery (StatefulSets)

```yaml
# ClusterIP Service for internal communication
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
    app: accounts  # Routes to pods with this label
  ports:
  - name: http
    port: 80
    targetPort: 8080
    protocol: TCP
  - name: management
    port: 8081
    targetPort: 8081
    protocol: TCP
  
  # Session affinity if needed
  sessionAffinity: ClientIP

---
# LoadBalancer Service for external exposure
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
# Headless Service for StatefulSet (database)
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
- Services automatically create DNS entries : `accounts-service.production.svc.cluster.local`
- Spring Boot can call : `http://accounts-service/api/accounts`

---

### Ingress - Intelligent HTTP/HTTPS Routing

#### Ingress Controller
- **Installation** : NGINX Ingress Controller, Traefik, or AWS ALB Ingress Controller
- **Features** : TLS termination, path-based routing, virtual hosting

```yaml
# Install NGINX Ingress Controller (Helm)
helm repo add ingress-nginx https://kubernetes.github.io/ingress-nginx
helm install nginx-ingress ingress-nginx/ingress-nginx \
  --namespace ingress-nginx --create-namespace

---
# Ingress with advanced routing
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
    secretName: tls-secret  # SSL Certificate
  
  rules:
  # Main virtual host
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
# Ingress with Canary Deployment
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: accounts-canary
  namespace: production
  annotations:
    nginx.ingress.kubernetes.io/canary: "true"
    nginx.ingress.kubernetes.io/canary-weight: "10"  # 10% of traffic
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
            name: accounts-service-v2  # New version
            port:
              number: 80
```

---

### Advanced Deployment Strategies

#### Rolling Update (Default)
```yaml
spec:
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxSurge: 1        # 1 extra pod during update
      maxUnavailable: 0  # No pod down = zero downtime
```

#### Blue/Green Deployment
```yaml
# Blue Deployment (current version)
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
# Green Deployment (new version)
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
# Service that switches between Blue and Green
apiVersion: v1
kind: Service
metadata:
  name: accounts-service
spec:
  selector:
    app: accounts
    version: blue  # Change to 'green' to switch
  ports:
  - port: 80
    targetPort: 8080
```

**Process** :
1. Deploy Green in parallel with Blue
2. Test Green internally
3. Switch the Service from `version: blue` to `version: green`
4. Instant rollback if issue (switch back to blue)
5. Delete Blue after validation

---

### Monitoring & Observability on Kubernetes

#### Prometheus & Grafana
```yaml
# ServiceMonitor for Prometheus
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
# Spring Boot configuration for metrics
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

#### Logging with ELK/EFK Stack
```yaml
# Fluentd DaemonSet to collect logs
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

### Auto-Scaling on Kubernetes

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
        averageUtilization: 70  # Scale if CPU > 70%
  - type: Resource
    resource:
      name: memory
      target:
        type: Utilization
        averageUtilization: 80  # Scale if RAM > 80%
  
  # Scale faster during spikes
  behavior:
    scaleUp:
      stabilizationWindowSeconds: 60
      policies:
      - type: Percent
        value: 100  # Double the number of pods
        periodSeconds: 60
    scaleDown:
      stabilizationWindowSeconds: 300  # Wait 5min before scale down
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
    updateMode: "Auto"  # Automatically adjusts resources
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

## Results & Impact

### Performance & Scalability
✅ **Zero Downtime** : Uninterrupted deployments thanks to rolling updates and health checks  
✅ **Auto-scaling** : Automatic management from 3 to 50 pods based on load  
✅ **High Availability** : Multi-AZ deployment on AWS with automatic failover  
✅ **Time to Market** : 70% reduction in deployment time via automated CI/CD

### Costs & Efficiency
✅ **Cost Optimization** : 40% infrastructure cost reduction via intelligent auto-scaling  
✅ **Resource Efficiency** : Optimal resource usage with well-calibrated requests/limits  
✅ **Spot Instances** : Integration of EC2 Spot Instances for non-critical workloads

### Security & Compliance
✅ **Secrets Management** : No plaintext secrets, automatic rotation  
✅ **Network Policies** : Network isolation between namespaces and pods  
✅ **RBAC** : Least privilege principle strictly applied  
✅ **Audit Logging** : Full traceability of deployments and access

---

## Mastered Technologies & Tools

**Cloud Providers** : AWS (EKS, EC2, ECS, RDS, ElastiCache, S3, CloudWatch)  
**Container Orchestration** : Kubernetes, Docker, Helm  
**CI/CD** : AWS CodePipeline, Jenkins, GitLab CI, ArgoCD  
**Infrastructure as Code** : Terraform, AWS CloudFormation, Helm Charts  
**Monitoring** : Prometheus, Grafana, CloudWatch, ELK/EFK Stack  
**Service Mesh** : Istio (bonus if applicable)  
**Security** : AWS Secrets Manager, HashiCorp Vault, Network Policies


---

# Technical Questions/Answers - AWS & Kubernetes

## 1. Deploying Spring Boot on AWS

### Q1: What are the different options for deploying a Spring Boot application on AWS and how to choose?
**A:** There are several options, each with its advantages:

**1. EC2 (Elastic Compute Cloud)**
- ✅ Full control over infrastructure
- ✅ Compatible with legacy applications
- ❌ Manual server management, patching, scaling
- **When to use** : Monolithic applications, system customization needs

**2. ECS (Elastic Container Service) + Fargate**
- ✅ No server management (serverless containers)
- ✅ Native AWS integration (IAM, CloudWatch)
- ✅ Simple automatic scaling
- ❌ AWS lock-in
- **When to use** : Containerized microservices, teams without K8s expertise

**3. EKS (Elastic Kubernetes Service)**
- ✅ Portable (multi-cloud, on-premise)
- ✅ Rich K8s ecosystem (Helm, Operators)
- ✅ Advanced orchestration
- ❌ Higher complexity
- ❌ Control plane management costs
- **When to use** : Complex microservices architecture, portability requirements

**4. Elastic Beanstalk**
- ✅ Simple PaaS, fast deployment
- ✅ Automatic infrastructure management
- ❌ Less control, limited customization
- **When to use** : Prototypes, small applications, beginner teams

**5. Lambda (serverless)**
- ✅ Pay-per-use, no server management
- ✅ Unlimited auto-scaling
- ❌ Cold start, limitations (15min timeout, package size)
- **When to use** : Event-driven APIs, sporadic workloads

**My recommendation by context** :
```
Startup / MVP           → Elastic Beanstalk or ECS Fargate
Simple microservices    → ECS Fargate
Complex microservices   → EKS
Event-driven/Async      → Lambda
Legacy migration        → EC2 with progressive migration to containers
```

---

### Q2: How to configure a Spring Boot application to read secrets from AWS Secrets Manager?
**A:** Several approaches are possible:

**Approach 1 : Direct AWS SDK**
```xml
<!-- Maven dependency -->
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
        // Retrieve the secret
        GetSecretValueRequest request = GetSecretValueRequest.builder()
            .secretId("prod/database/credentials")
            .build();
        
        GetSecretValueResponse response = client.getSecretValue(request);
        String secretString = response.secretString();
        
        // Parse the JSON
        JsonNode secrets = new ObjectMapper().readTree(secretString);
        String username = secrets.get("username").asText();
        String password = secrets.get("password").asText();
        
        // Configure DataSource
        HikariDataSource dataSource = new HikariDataSource();
        dataSource.setJdbcUrl(secrets.get("url").asText());
        dataSource.setUsername(username);
        dataSource.setPassword(password);
        
        return dataSource;
    }
}
```

**Approach 2 : Spring Cloud AWS (recommended)**
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
        instance-profile: true  # Uses IAM role from EC2/ECS
      
  config:
    import: aws-secretsmanager:prod/myapp/  # Secrets prefix

# Secrets are automatically injected as properties
# Secret "prod/myapp/database" with key "password" becomes:
# ${database.password}
```

```java
@Configuration
public class DatabaseConfig {
    
    @Value("${database.url}")
    private String dbUrl;
    
    @Value("${database.username}")
    private String dbUsername;
    
    @Value("${database.password}")  // From Secrets Manager
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

**Approach 3 : Automatic rotation with cache**
```java
@Service
public class SecretsCacheService {
    
    private final SecretsManagerClient client;
    private final Cache<String, String> secretsCache;
    
    public SecretsCacheService(SecretsManagerClient client) {
        this.client = client;
        this.secretsCache = CacheBuilder.newBuilder()
            .expireAfterWrite(5, TimeUnit.MINUTES)  // Refresh every 5min
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

**IAM Role configuration (important)** :
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

### Q3: How to set up a complete CI/CD pipeline to deploy a Spring Boot app on AWS EKS?
**A:** Here is a complete pipeline with AWS CodePipeline and GitHub Actions:

**Pipeline Architecture** :
```
GitHub Push → CodePipeline → CodeBuild → ECR → EKS Deployment → Health Check
```

**Step 1 : buildspec.yml for CodeBuild**
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
      # Build Spring Boot with Maven
      - echo Build started on `date`
      - mvn clean package -DskipTests
      
      # Build Docker image
      - docker build -t $REPOSITORY_URI:latest .
      - docker tag $REPOSITORY_URI:latest $REPOSITORY_URI:$IMAGE_TAG
      
      # Tests (optional)
      - mvn test
  
  post_build:
    commands:
      # Push to ECR
      - echo Pushing Docker image...
      - docker push $REPOSITORY_URI:latest
      - docker push $REPOSITORY_URI:$IMAGE_TAG
      
      # Create imagedefinitions.json for ECS (if ECS)
      - printf '[{"name":"accounts-service","imageUri":"%s"}]' $REPOSITORY_URI:$IMAGE_TAG > imagedefinitions.json
      
      # Update K8s deployment
      - echo Deploying to EKS...
      - aws eks update-kubeconfig --region $AWS_REGION --name $EKS_CLUSTER_NAME
      - kubectl set image deployment/accounts-service accounts-app=$REPOSITORY_URI:$IMAGE_TAG -n production
      - kubectl rollout status deployment/accounts-service -n production

artifacts:
  files:
    - imagedefinitions.json
    - k8s/*.yaml
```

**Step 2 : GitHub Actions alternative (more modern)**
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

**Step 3 : Blue/Green Deployment with validation**
```yaml
# deploy-blue-green.yml
name: Blue/Green Deployment

jobs:
  deploy:
    steps:
    # ... (identical build steps)
    
    - name: Deploy Green version
      run: |
        # Deploy new version (green)
        kubectl apply -f k8s/deployment-green.yaml
        kubectl wait --for=condition=available --timeout=300s deployment/accounts-green -n production
    
    - name: Run smoke tests on Green
      run: |
        GREEN_POD=$(kubectl get pod -n production -l version=green -o jsonpath='{.items[0].metadata.name}')
        kubectl port-forward -n production $GREEN_POD 8080:8080 &
        sleep 5
        
        # Health tests
        curl -f http://localhost:8080/actuator/health || exit 1
        curl -f http://localhost:8080/actuator/health/readiness || exit 1
        
        # Functional tests
        ./run-integration-tests.sh http://localhost:8080
    
    - name: Switch traffic to Green
      if: success()
      run: |
        # Switch service to green
        kubectl patch service accounts-service -n production -p '{"spec":{"selector":{"version":"green"}}}'
        
        echo "Traffic switched to Green version"
    
    - name: Monitor for 5 minutes
      run: |
        sleep 300
        
        # Check for errors
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

**Step 4 : Slack Notifications**
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

### Q4: What is the difference between ConfigMap and Secret? When to use one or the other?
**A:**

| Aspect | ConfigMap | Secret |
|--------|-----------|--------|
| **Usage** | Non-sensitive configuration | Sensitive data (passwords, tokens, certs) |
| **Storage** | Plain text in etcd | Base64 encoded (⚠️ not encryption!) |
| **Encryption at rest** | No by default | Yes if KMS enabled |
| **Max size** | 1MB | 1MB |
| **Visibility** | Visible in logs, describe | Hidden in describe (shown as ***) |

**ConfigMap - Usage examples** :
```yaml
# ✅ GOOD - Application configuration
- Internal service URLs
- Feature flags (FEATURE_NEW_API: "true")
- Spring profile configuration
- Timeout, retry parameters
- Logging level configuration

# ❌ BAD - Sensitive data
- Database passwords
- External API keys
- JWT secrets
- TLS certificates
```

**Secret - Usage examples** :
```yaml
# ✅ GOOD - Sensitive data
- Database passwords
- API keys (AWS, Stripe, etc.)
- OAuth tokens
- Private keys, certificates
- Docker registry credentials

# K8s secret types
- Opaque: generic secret (default)
- kubernetes.io/tls: TLS certificates
- kubernetes.io/dockerconfigjson: Docker registry auth
- kubernetes.io/basic-auth: user/password
- kubernetes.io/ssh-auth: SSH keys
```

**⚠️ IMPORTANT : K8s Secrets security**
```yaml
# By default, K8s secrets = base64 (NOT ENCRYPTION!)
# Anyone with kubectl access can decode:
kubectl get secret my-secret -o jsonpath='{.data.password}' | base64 -d

# Solutions for real security:

# 1. Encryption at rest with KMS
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

# 2. External Secrets Operator (recommended)
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: database-credentials
spec:
  secretStoreRef:
    name: aws-secrets-manager
    kind: SecretStore
  target:
    name: db-secret  # K8s Secret created automatically
  data:
  - secretKey: password
    remoteRef:
      key: prod/database/credentials
      property: password
```

---

### Q5: How to manage ConfigMaps and Secrets across different environments (dev, staging, prod)?
**A:** Several strategies depending on complexity:

**Strategy 1 : Separate Namespaces**
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

# Same name, different namespace
# Deployment: kubectl apply -f config.yaml -n dev|production
```

**Strategy 2 : Kustomize (recommended for native K8s)**
```bash
# File structure
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
# base/configmap.yaml (default values)
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

# Deployment
kubectl apply -k overlays/dev
kubectl apply -k overlays/production
```

**Strategy 3 : Helm with values files**
```yaml
# Chart structure
charts/accounts-service/
├── Chart.yaml
├── templates/
│   ├── deployment.yaml
│   ├── configmap.yaml
│   └── secret.yaml
├── values.yaml          # Default
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
# values.yaml (default)
database:
  url: "jdbc:postgresql://localhost:5432/db"
logging:
  level: "INFO"
replicas: 2

---
# values-prod.yaml (prod override)
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

# Deployment
helm install accounts ./charts/accounts-service -f values-dev.yaml -n dev
helm install accounts ./charts/accounts-service -f values-prod.yaml -n production
```

**Strategy 4 : External Secrets per environment**
```yaml
# Secret Store per environment
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
# External Secret pointing to different AWS secrets
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
      key: dev/database/credentials  # AWS secret for dev

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
      key: prod/database/credentials  # AWS secret for prod
```

**Best Practices** :
```yaml
✅ Use Kustomize or Helm to manage multi-env
✅ Sensitive secrets in AWS Secrets Manager/Vault
✅ GitOps with ArgoCD for automatic sync
✅ Separate namespaces per environment
✅ Strict RBAC (dev cannot access prod)
✅ Validate configs with dry-run before apply
❌ NEVER commit secrets in Git (even base64 encoded)
❌ Do not share the same secrets between envs
```

---

## 3. Kubernetes Services & Ingress

### Q6: Explain the different K8s Service types and give a use case for each
**A:**

**1. ClusterIP (default)** - Internal service only
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

# Use cases:
# - Communication between microservices (accounts → loans)
# - Database (postgres service)
# - Internal Redis cache
# - Backend services not exposed publicly

# Access: http://accounts-internal.production.svc.cluster.local
```

**2. NodePort** - Exposed on a node port
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
    nodePort: 30080  # Port on each node (30000-32767)

# Use cases:
# - Dev/test environment (direct access)
# - Temporary debugging
# - CI/CD for integration tests
# ⚠️ Rarely used in production

# Access: http://<node-ip>:30080
```

**3. LoadBalancer** - Cloud load balancer (AWS ELB/NLB)
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

# Use cases:
# - Public API exposed to the Internet
# - Frontend web application
# - Services that need to be accessible externally

# AWS automatically creates an ELB/NLB
# Access: http://<elb-dns-name>
```

**4. Headless Service** - No ClusterIP, DNS resolves directly to pods
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

# Use cases:
# - StatefulSets (databases, Kafka, Elasticsearch)
# - Individual pod service discovery
# - Direct connection to a specific pod

# DNS resolves to all pod IPs:
# postgres-0.postgres-headless.default.svc.cluster.local → 10.0.1.5
# postgres-1.postgres-headless.default.svc.cluster.local → 10.0.1.6
# postgres-2.postgres-headless.default.svc.cluster.local → 10.0.1.7
```

**5. ExternalName** - Alias to external service
```yaml
apiVersion: v1
kind: Service
metadata:
  name: external-payment-api
spec:
  type: ExternalName
  externalName: api.stripe.com

# Use cases:
# - Reference external API with internal name
# - Progressive migration (point to old system)
# - External service abstraction

# In Spring Boot app:
# http://external-payment-api/v1/charges
# → Resolves to api.stripe.com/v1/charges
```

**Visual comparison** :
```
ClusterIP:    Pod ←→ Service (ClusterIP) ←→ Pod
              (internal network only)

NodePort:     External → Node:30080 → Service → Pod
              (each node exposes the port)

LoadBalancer: External → Cloud LB → Service → Pod
              (AWS/GCP/Azure creates LB automatically)

Headless:     Client → DNS → Pod IP directly
              (no load balancing, DNS round-robin)
```

---

### Q7: How to configure an Ingress with SSL/TLS and different backends by path?
**A:**

**Prerequisite : Install Ingress Controller**
```bash
# 1. Install NGINX Ingress Controller
helm repo add ingress-nginx https://kubernetes.github.io/ingress-nginx
helm repo update

helm install nginx-ingress ingress-nginx/ingress-nginx \
  --namespace ingress-nginx \
  --create-namespace \
  --set controller.service.type=LoadBalancer \
  --set controller.metrics.enabled=true

# 2. Verify installation
kubectl get pods -n ingress-nginx
kubectl get svc -n ingress-nginx  # Note the External IP/DNS of the LB
```

**Complete multi-path Ingress configuration with TLS** :
```yaml
# 1. Create TLS certificate (Let's Encrypt with cert-manager)
apiVersion: cert-manager.io/v1
kind: Certificate
metadata:
  name: api-tls-cert
  namespace: production
spec:
  secretName: api-tls-secret  # K8s Secret created automatically
  issuerRef:
    name: letsencrypt-prod
    kind: ClusterIssuer
  dnsNames:
    - api.myapp.com
    - www.api.myapp.com

---
# 2. Ingress with advanced routing
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
    
    # Whitelist IPs (optional)
    nginx.ingress.kubernetes.io/whitelist-source-range: "10.0.0.0/8,172.16.0.0/12"
    
    # Circuit Breaker
    nginx.ingress.kubernetes.io/upstream-fail-timeout: "10"
    nginx.ingress.kubernetes.io/upstream-max-fails: "3"

spec:
  ingressClassName: nginx
  
  # TLS/SSL Configuration
  tls:
  - hosts:
    - api.myapp.com
    secretName: api-tls-secret  # Certificate created by cert-manager
  
  rules:
  # Main rule for api.myapp.com
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
      
      # Payments Service (with specific rate limiting)
      - path: /api/payments(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: payments-service
            port:
              number: 80
      
      # API Gateway (catch-all for other routes)
      - path: /api(/|$)(.*)
        pathType: ImplementationSpecific
        backend:
          service:
            name: api-gateway
            port:
              number: 80
      
      # Health check endpoint (no auth)
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

**Advanced configuration : Header-based routing**
```yaml
---
# Canary Ingress (10% of traffic)
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: accounts-canary
  namespace: production
  annotations:
    nginx.ingress.kubernetes.io/canary: "true"
    nginx.ingress.kubernetes.io/canary-weight: "10"  # 10% of traffic
    # OR canary by header
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
            name: accounts-service-v2  # New version
            port:
              number: 80
```

**OAuth2/Authentication Configuration**
```yaml
---
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: protected-api
  namespace: production
  annotations:
    # OAuth2 Proxy for authentication
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

**Testing and validation**
```bash
# 1. Verify Ingress configuration
kubectl describe ingress microservices-ingress -n production

# 2. Test DNS resolution
nslookup api.myapp.com

# 3. Test endpoints
curl -I https://api.myapp.com/api/accounts
curl -I https://api.myapp.com/api/loans

# 4. Test HTTP → HTTPS redirect
curl -I http://api.myapp.com/api/accounts
# Should return 301/308 redirect to HTTPS

# 5. Verify SSL certificate
openssl s_client -connect api.myapp.com:443 -servername api.myapp.com

# 6. Test rate limiting
for i in {1..150}; do curl -I https://api.myapp.com/api/accounts; done
# After 100 requests, should return 429 Too Many Requests

# 7. NGINX Ingress logs
kubectl logs -n ingress-nginx deployment/nginx-ingress-controller --tail=100 -f
```

---

### Q8: How to manage Ingress Controller monitoring and high availability?
**A:**

**High Availability Ingress**
```yaml
# HA Deployment of NGINX Ingress
apiVersion: apps/v1
kind: Deployment
metadata:
  name: nginx-ingress-controller
  namespace: ingress-nginx
spec:
  replicas: 3  # Minimum 3 for HA
  
  # Anti-affinity for distribution across different nodes
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
        
        # Guaranteed resources
        resources:
          requests:
            cpu: 500m
            memory: 512Mi
          limits:
            cpu: 2000m
            memory: 2Gi
        
        # Critical probes
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
# LoadBalancer Service with health checks
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
  externalTrafficPolicy: Local  # Preserves source IP
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

**Monitoring with Prometheus & Grafana**
```yaml
# ServiceMonitor for Prometheus scraping
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
# PrometheusRule for alerts
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
    # Alert if 5xx error rate > 5%
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
    
    # Alert if p99 latency > 5s
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
    
    # Alert if Ingress Controller is down
    - alert: IngressControllerDown
      expr: up{job="nginx-ingress-controller"} == 0
      for: 1m
      labels:
        severity: critical
      annotations:
        summary: "Ingress Controller is down"
        description: "{{ $labels.instance }} has been down for more than 1 minute"
    
    # Alert on excessive rate limiting
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

**Example Grafana Dashboard**
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

## 4. Advanced Kubernetes Deployments

### Q9: How to implement a Canary deployment with automatic validation?
**A:**

**Approach 1 : Ingress Canary with progressive validation**
```yaml
# 1. Stable version deployment (blue)
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
# 2. Canary version deployment
apiVersion: apps/v1
kind: Deployment
metadata:
  name: accounts-canary
  namespace: production
spec:
  replicas: 2  # Start with few replicas
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
        image: accounts-service:v1.6.0  # New version
        ports:
        - containerPort: 8080

---
# 3. Main service (stable)
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
# 4. Main Ingress (stable)
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
# 5. Canary Ingress (5% of traffic)
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: accounts-canary-ingress
  namespace: production
  annotations:
    nginx.ingress.kubernetes.io/canary: "true"
    nginx.ingress.kubernetes.io/canary-weight: "5"  # 5% only
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
# 6. Canary service
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

**Automated validation and progression script**
```bash
#!/bin/bash
# canary-promotion.sh

NAMESPACE="production"
CANARY_INGRESS="accounts-canary-ingress"
STABLE_DEPLOYMENT="accounts-stable"
CANARY_DEPLOYMENT="accounts-canary"

# Function to get metrics
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

# Phase 1: 5% of traffic
echo "Phase 1: Routing 5% traffic to canary..."
kubectl patch ingress $CANARY_INGRESS -n $NAMESPACE --type=json \
    -p='[{"op": "replace", "path": "/metadata/annotations/nginx.ingress.kubernetes.io~1canary-weight", "value": "5"}]'

sleep 300  # Wait 5 minutes

# Phase 1 Validation
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

# Phase 2: 25% of traffic
echo "Phase 2: Routing 25% traffic to canary..."
kubectl patch ingress $CANARY_INGRESS -n $NAMESPACE --type=json \
    -p='[{"op": "replace", "path": "/metadata/annotations/nginx.ingress.kubernetes.io~1canary-weight", "value": "25"}]'

# Scale up canary
kubectl scale deployment $CANARY_DEPLOYMENT -n $NAMESPACE --replicas=5

sleep 600  # Wait 10 minutes

# Phase 2 Validation (same logic)
echo "Validating Phase 2 metrics..."
# ... (same validation)

echo "✅ Phase 2 validation passed"

# Phase 3: 50% of traffic
echo "Phase 3: Routing 50% traffic to canary..."
kubectl patch ingress $CANARY_INGRESS -n $NAMESPACE --type=json \
    -p='[{"op": "replace", "path": "/metadata/annotations/nginx.ingress.kubernetes.io~1canary-weight", "value": "50"}]'

kubectl scale deployment $CANARY_DEPLOYMENT -n $NAMESPACE --replicas=10

sleep 600

# Phase 3 Validation
echo "Validating Phase 3 metrics..."
# ... (same validation)

echo "✅ Phase 3 validation passed"

# Phase 4: 100% promotion
echo "Phase 4: Full promotion to canary..."

# Switch main service to canary
kubectl patch service accounts-service -n $NAMESPACE --type=json \
    -p='[{"op": "replace", "path": "/spec/selector/version", "value": "canary"}]'

# Delete canary Ingress
kubectl delete ingress $CANARY_INGRESS -n $NAMESPACE

# Delete old stable deployment after validation
sleep 300
kubectl delete deployment $STABLE_DEPLOYMENT -n $NAMESPACE

# Rename canary to stable
kubectl patch deployment $CANARY_DEPLOYMENT -n $NAMESPACE --type=json \
    -p='[{"op": "replace", "path": "/metadata/name", "value": "accounts-stable"}]'

echo "✅ Canary promotion completed successfully!"
```

**Approach 2 : Flagger (canary automation)**
```yaml
# Install Flagger
kubectl apply -k github.com/fluxcd/flagger//kustomize/linkerd

---
# Canary resource (Flagger)
apiVersion: flagger.app/v1beta1
kind: Canary
metadata:
  name: accounts-canary
  namespace: production
spec:
  # Deployment to deploy as canary
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: accounts-service
  
  # Service
  service:
    port: 80
    targetPort: 8080
  
  # Automatic analysis
  analysis:
    # Interval between each step
    interval: 1m
    
    # Success threshold before progression
    threshold: 5
    
    # Max failures before rollback
    maxWeight: 50
    
    # Traffic increment
    stepWeight: 10  # 10%, 20%, 30%...
    
    # Metrics to validate
    metrics:
    - name: request-success-rate
      thresholdRange:
        min: 99  # Min 99% success
      interval: 1m
    
    - name: request-duration
      thresholdRange:
        max: 500  # Max 500ms latency
      interval: 1m
    
    # Webhooks for custom tests
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

  # Alerts
  alerts:
  - name: slack
    severity: info
    providerRef:
      name: slack-notifier
```

---

### Q10: How to manage StatefulSets for databases on Kubernetes?
**A:**

**Example : PostgreSQL StatefulSet**
```yaml
# 1. StorageClass for persistent volumes
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
reclaimPolicy: Retain  # ⚠️ Do not delete the data

---
# 2. Headless Service for service discovery
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
# 3. LoadBalancer Service for external access (optional)
apiVersion: v1
kind: Service
metadata:
  name: postgres-lb
  namespace: database
spec:
  type: LoadBalancer
  selector:
    app: postgres
    statefulset.kubernetes.io/pod-name: postgres-0  # Master only
  ports:
  - port: 5432
    targetPort: 5432

---
# 4. PostgreSQL StatefulSet
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
      # Anti-affinity for distribution
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
        
        # Environment variables from Secret
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
  
  # Volume Claim Template (created automatically for each pod)
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
# 5. Secret for credentials
apiVersion: v1
kind: Secret
metadata:
  name: postgres-secret
  namespace: database
type: Opaque
data:
  password: c3VwZXJTZWNyZXRQYXNzd29yZA==  # base64 encoded
```

**Connection from Spring Boot**
```yaml
# ConfigMap for Spring Boot
apiVersion: v1
kind: ConfigMap
metadata:
  name: app-config
data:
  application.yaml: |
    spring:
      datasource:
        # Master for writes
        url: jdbc:postgresql://postgres-0.postgres-headless.database.svc.cluster.local:5432/myappdb
        username: admin
        password: ${DB_PASSWORD}
        
        hikari:
          maximum-pool-size: 20
          connection-timeout: 30000
      
      # Read from replicas (load balancing)
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

**Automatic backup**
```yaml
# CronJob for PostgreSQL backup
apiVersion: batch/v1
kind: CronJob
metadata:
  name: postgres-backup
  namespace: database
spec:
  schedule: "0 2 * * *"  # Every day at 2am
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
              
              # Upload to S3
              aws s3 cp $BACKUP_FILE s3://my-backups/postgres/
              
              # Keep only the last 7 days locally
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

**Scaling and maintenance**
```bash
# Scale up (add a replica)
kubectl scale statefulset postgres -n database --replicas=4

# Rolling update (version upgrade)
kubectl set image statefulset/postgres postgres=postgres:16-alpine -n database
# StatefulSet updates one pod at a time: postgres-2, then postgres-1, then postgres-0

# Check status
kubectl rollout status statefulset/postgres -n database

# Access a specific pod
kubectl exec -it postgres-0 -n database -- psql -U admin -d myappdb

# Restore a backup
kubectl exec -it postgres-0 -n database -- /bin/bash
gunzip < /backup/postgres-20250101-020000.sql.gz | psql -U admin -d myappdb

# Check persistent volumes
kubectl get pvc -n database
kubectl describe pvc postgres-storage-postgres-0 -n database

# Delete a pod (will be automatically recreated with the same volume)
kubectl delete pod postgres-1 -n database
# Pod restarts with the same data (PVC retained)

# Disaster recovery - recreate from backup
kubectl delete statefulset postgres -n database
# PVCs are retained (reclaimPolicy: Retain)
kubectl apply -f postgres-statefulset.yaml
# Pods come back up with the existing data
```

---

### Q11: How to manage sensitive secrets in a truly secure way on K8s (not just base64)?
**A:**

**Problem with native K8s Secrets** :
```bash
# Anyone with kubectl access can decode
kubectl get secret db-password -o jsonpath='{.data.password}' | base64 -d
# → "superSecretPassword123"

# Secrets visible in etcd without encryption by default
```

**Solution 1 : External Secrets Operator + AWS Secrets Manager (Recommended)**
```yaml
# 1. Install External Secrets Operator
helm repo add external-secrets https://charts.external-secrets.io
helm install external-secrets external-secrets/external-secrets \
  -n external-secrets-system --create-namespace

---
# 2. IAM Role for ServiceAccount (IRSA)
apiVersion: v1
kind: ServiceAccount
metadata:
  name: external-secrets-sa
  namespace: production
  annotations:
    eks.amazonaws.com/role-arn: arn:aws:iam::123456789:role/ExternalSecretsRole

---
# 3. SecretStore (connection to AWS Secrets Manager)
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
# 4. ExternalSecret (automatic sync from AWS)
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: database-credentials
  namespace: production
spec:
  # Refresh every 5 minutes
  refreshInterval: 5m
  
  secretStoreRef:
    name: aws-secrets-store
    kind: SecretStore
  
  target:
    name: db-credentials  # Name of K8s Secret created
    creationPolicy: Owner
    template:
      engineVersion: v2
      data:
        # Template to format the secret
        DATABASE_URL: "postgresql://{{ .username }}:{{ .password }}@postgres-service:5432/mydb"
  
  # Data to retrieve from AWS Secrets Manager
  data:
  - secretKey: username
    remoteRef:
      key: prod/database/credentials  # Name in AWS
      property: username
  
  - secretKey: password
    remoteRef:
      key: prod/database/credentials
      property: password
  
  - secretKey: ssl-cert
    remoteRef:
      key: prod/database/ssl-cert

---
# 5. Usage in Pod
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
          name: db-credentials  # Secret created by ExternalSecret
          key: DATABASE_URL
```

**Creating the secret in AWS**
```bash
# Create secret in AWS Secrets Manager
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

# Automatic secret rotation (Lambda)
aws secretsmanager rotate-secret \
  --secret-id prod/database/credentials \
  --rotation-lambda-arn arn:aws:lambda:eu-west-1:123456789:function:SecretsManagerRotation \
  --rotation-rules AutomaticallyAfterDays=30
```

**Solution 2 : HashiCorp Vault (for multi-cloud)**
```yaml
# 1. Install Vault
helm repo add hashicorp https://helm.releases.hashicorp.com
helm install vault hashicorp/vault \
  --namespace vault --create-namespace \
  --set "server.ha.enabled=true" \
  --set "server.ha.replicas=3"

---
# 2. Configure Vault K8s Auth
# In Vault CLI:
vault auth enable kubernetes

vault write auth/kubernetes/config \
  kubernetes_host="https://$KUBERNETES_PORT_443_TCP_ADDR:443" \
  token_reviewer_jwt="$(cat /var/run/secrets/kubernetes.io/serviceaccount/token)" \
  kubernetes_ca_cert=@/var/run/secrets/kubernetes.io/serviceaccount/ca.crt

# Create policy
vault policy write accounts-policy - <<EOF
path "secret/data/accounts/*" {
  capabilities = ["read"]
}
EOF

# Create K8s role
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
  
  # Automatic rollout restart if secret changes
  rolloutRestartTargets:
  - kind: Deployment
    name: accounts-service

---
# 4. ServiceAccount with Vault access
apiVersion: v1
kind: ServiceAccount
metadata:
  name: accounts-sa
  namespace: production

---
# 5. Deployment using Vault
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
        
        # Option 1: Via K8s Secret (created by Vault Operator)
        env:
        - name: DB_PASSWORD
          valueFrom:
            secretKeyRef:
              name: db-vault-secret
              key: password
        
        # Option 2: Vault Agent Sidecar Injector
        # automatic annotations:
        # vault.hashicorp.com/agent-inject: "true"
        # vault.hashicorp.com/role: "accounts"
        # vault.hashicorp.com/agent-inject-secret-db: "secret/accounts/database"
```

**Vault Agent Sidecar (automatic injection)**
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: accounts-service
spec:
  template:
    metadata:
      annotations:
        # Enable Vault Agent injection
        vault.hashicorp.com/agent-inject: "true"
        vault.hashicorp.com/role: "accounts"
        
        # Inject the secret into a file
        vault.hashicorp.com/agent-inject-secret-database.txt: "secret/accounts/database"
        
        # Template to format the secret
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
        
        # The file is mounted at /vault/secrets/database.txt
        command:
        - java
        - -jar
        - app.jar
        - --spring.config.import=file:/vault/secrets/database.txt
```

**Solution 3 : Sealed Secrets (encrypted secrets in Git)**
```bash
# 1. Install Sealed Secrets Controller
kubectl apply -f https://github.com/bitnami-labs/sealed-secrets/releases/download/v0.24.0/controller.yaml

# 2. Install kubeseal CLI
wget https://github.com/bitnami-labs/sealed-secrets/releases/download/v0.24.0/kubeseal-linux-amd64
chmod +x kubeseal-linux-amd64
sudo mv kubeseal-linux-amd64 /usr/local/bin/kubeseal

# 3. Create a normal secret
kubectl create secret generic db-credentials \
  --from-literal=username=admin \
  --from-literal=password=superSecret123 \
  --dry-run=client -o yaml > secret.yaml

# 4. Seal the secret (encryption with cluster public key)
kubeseal --format yaml < secret.yaml > sealed-secret.yaml

# 5. The SealedSecret can be committed to Git
cat sealed-secret.yaml
```

```yaml
# sealed-secret.yaml (safe to commit to Git)
apiVersion: bitnami.com/v1alpha1
kind: SealedSecret
metadata:
  name: db-credentials
  namespace: production
spec:
  encryptedData:
    username: AgBghQ7Xmv8... # Encrypted with public key
    password: AgC8H3nK9s2... # Cannot be decrypted without cluster private key
  template:
    metadata:
      name: db-credentials
      namespace: production

# Apply in the cluster
kubectl apply -f sealed-secret.yaml

# The controller automatically decrypts and creates the K8s Secret
# Only the cluster can decrypt (private key in the controller)
```

**Solutions comparison**

| Solution | Advantages | Disadvantages | Use Case |
|----------|-----------|---------------|----------|
| **External Secrets + AWS** | ✅ Native AWS<br>✅ Auto rotation<br>✅ CloudTrail audit | ❌ AWS lock-in<br>❌ AWS cost | Production on AWS |
| **Vault** | ✅ Multi-cloud<br>✅ Dynamic secrets<br>✅ Very flexible | ❌ Setup complexity<br>❌ Vault maintenance | Enterprise, multi-cloud |
| **Sealed Secrets** | ✅ GitOps friendly<br>✅ Simple<br>✅ No external infra | ❌ No auto rotation<br>❌ Critical key backup | Startups, small projects |
| **KMS Encryption** | ✅ Native K8s<br>✅ Transparent | ❌ Secrets visible with kubectl<br>❌ No rotation | Minimum baseline |

**Best Practice : Combination**
```yaml
# Recommended strategy:
1. External Secrets Operator + AWS Secrets Manager (sensitive secrets)
2. ConfigMaps for non-sensitive configuration
3. Sealed Secrets for less critical secrets (non-prod API keys)
4. KMS encryption at rest enabled (baseline)
5. Strict RBAC (who can read secrets?)
6. Audit logging (who accessed secrets?)
7. Automatic secret rotation (30-90 days)
```

---

### Q12: How to debug a pod that is crashlooping or not starting on Kubernetes?
**A:**

**Complete debugging methodology**

**1. Check Pod Status**
```bash
# General status
kubectl get pods -n production
# NAME                    READY   STATUS             RESTARTS   AGE
# accounts-5d7f8-xyz      0/1     CrashLoopBackOff   5          3m

# Full details
kubectl describe pod accounts-5d7f8-xyz -n production

# Things to look at:
# - Events (at the bottom): image pull errors, OOMKilled, etc.
# - Containers.State: waiting/terminated with reason
# - Conditions: see if Initialized, Ready, etc.
```

**2. Container Logs**
```bash
# Current logs
kubectl logs accounts-5d7f8-xyz -n production

# Previous container logs (if crashed)
kubectl logs accounts-5d7f8-xyz -n production --previous

# Follow logs in real time
kubectl logs -f accounts-5d7f8-xyz -n production

# Logs from all containers in a pod (if multi-container)
kubectl logs accounts-5d7f8-xyz -n production --all-containers=true

# Logs with timestamps
kubectl logs accounts-5d7f8-xyz -n production --timestamps=true

# Last X lines
kubectl logs accounts-5d7f8-xyz -n production --tail=100
```

**3. Common problems and solutions**

**A. ImagePullBackOff / ErrImagePull**
```bash
# Problem: Cannot pull image

# Verify image name
kubectl describe pod accounts-5d7f8-xyz -n production | grep Image

# Possible solutions:
# 1. Image does not exist
docker pull myregistry.io/accounts-service:v1.2.0  # Test manually

# 2. Registry authentication issue
kubectl get secret regcred -n production -o yaml
# Create/update the secret
kubectl create secret docker-registry regcred \
  --docker-server=myregistry.io \
  --docker-username=myuser \
  --docker-password=mypass \
  --docker-email=email@example.com \
  -n production

# Add to Deployment
spec:
  template:
    spec:
      imagePullSecrets:
      - name: regcred

# 3. Private AWS ECR registry
aws ecr get-login-password --region eu-west-1 | \
  docker login --username AWS --password-stdin 123456789.dkr.ecr.eu-west-1.amazonaws.com
```

**B. CrashLoopBackOff (application crashes at startup)**
```bash
# Logs show the error
kubectl logs accounts-5d7f8-xyz -n production --previous

# Common Spring Boot errors:

# 1. Cannot connect to database
# Solution: Check ConfigMap/Secret
kubectl get configmap app-config -n production -o yaml
kubectl get secret db-credentials -n production -o jsonpath='{.data.password}' | base64 -d

# Test DB connectivity from a temporary pod
kubectl run -it --rm debug --image=postgres:15 --restart=Never -- \
  psql -h postgres-service -U admin -d mydb

# 2. Port already in use (bad config)
# Check in Deployment
kubectl get deployment accounts-service -n production -o yaml | grep containerPort

# 3. OutOfMemory (OOMKilled)
# Increase memory limits
kubectl set resources deployment accounts-service \
  --limits=memory=2Gi \
  --requests=memory=1Gi \
  -n production

# 4. Missing environment variable
kubectl exec -it accounts-5d7f8-xyz -n production -- env | grep DATABASE
```

**C. Pending (pod never starts)**
```bash
kubectl describe pod accounts-5d7f8-xyz -n production

# Common reasons:

# 1. Insufficient CPU/Memory (no node with enough resources)
# Events: 0/3 nodes available: insufficient memory
kubectl top nodes  # See usage
kubectl describe nodes  # See capacity

# Solution: Scale down other apps or add nodes
kubectl scale deployment other-app --replicas=1

# 2. PVC Pending (volume not created)
kubectl get pvc -n production
# Solution: Verify StorageClass exists
kubectl get storageclass

# 3. Incompatible node selector/affinity
# Check if the deployment has constraints
kubectl get deployment accounts-service -n production -o yaml | grep -A 10 nodeSelector

# List node labels
kubectl get nodes --show-labels
```

**D. Init Container Failed**
```bash
# Check init containers status
kubectl describe pod accounts-5d7f8-xyz -n production | grep -A 20 "Init Containers"

# Init container logs
kubectl logs accounts-5d7f8-xyz -n production -c init-db-migration

# Example: DB migration that fails
# Solution: Check migration script, DB permissions
```

**4. Interactive debugging**

**Exec into a running container**
```bash
# Interactive shell
kubectl exec -it accounts-5d7f8-xyz -n production -- /bin/bash

# Useful commands inside the container:
# Check Java process
ps aux | grep java

# Test network connectivity
curl http://postgres-service:5432
nc -zv postgres-service 5432

# Check DNS
nslookup postgres-service
cat /etc/resolv.conf

# Check env variables
env | grep DATABASE

# Check mounted files
ls -la /config
cat /config/application.yaml

# Java thread dump (if app is stuck)
kill -3 $(pgrep java)
# Logs appear in kubectl logs
```

**Debug container for pod that crashes immediately**
```yaml
# Technique: Override entrypoint to keep container alive
apiVersion: v1
kind: Pod
metadata:
  name: accounts-debug
spec:
  containers:
  - name: accounts
    image: accounts-service:v1.2.0
    command: ["/bin/sh"]
    args: ["-c", "sleep 3600"]  # Keep container alive 1h
    
    env:
    - name: DATABASE_URL
      value: "jdbc:postgresql://postgres:5432/mydb"

# Apply and exec into it
kubectl apply -f accounts-debug.yaml
kubectl exec -it accounts-debug -- /bin/bash

# Now we can debug manually:
java -jar /app/app.jar
# See the exact error
```

**Ephemeral Debug Container (K8s 1.23+)**
```bash
# Add a debug container to an existing pod
kubectl debug accounts-5d7f8-xyz -it --image=busybox:1.28 -n production

# Or with a more complete image
kubectl debug accounts-5d7f8-xyz -it \
  --image=nicolaka/netshoot \
  --target=accounts \
  -n production

# Allows debugging without modifying the Deployment
```

**5. Network debugging**
```bash
# Debug pod with network tools
kubectl run netshoot --rm -it --image=nicolaka/netshoot -- /bin/bash

# Inside the pod:
# Test connectivity
ping postgres-service
curl -v http://accounts-service/actuator/health

# DNS lookup
nslookup accounts-service.production.svc.cluster.local

# Trace route
traceroute accounts-service

# Port scan
nmap -p 8080 accounts-service

# Test from outside the cluster
kubectl port-forward pod/accounts-5d7f8-xyz 8080:8080 -n production
# In another terminal:
curl http://localhost:8080/actuator/health
```

**6. Events and monitoring**
```bash
# See all recent events
kubectl get events -n production --sort-by='.lastTimestamp'

# Events for a specific pod
kubectl get events -n production --field-selector involvedObject.name=accounts-5d7f8-xyz

# Pod metrics
kubectl top pods -n production
kubectl top pod accounts-5d7f8-xyz -n production --containers

# See failing probes
kubectl describe pod accounts-5d7f8-xyz -n production | grep -A 5 "Liveness\|Readiness"
```

**7. Spring Boot configuration issues**
```bash
# Check active profile
kubectl exec accounts-5d7f8-xyz -n production -- \
  curl http://localhost:8081/actuator/env | jq '.propertySources[] | select(.name | contains("application"))'

# Check loaded properties
kubectl exec accounts-5d7f8-xyz -n production -- \
  curl http://localhost:8081/actuator/configprops

# Detailed health check
kubectl exec accounts-5d7f8-xyz -n production -- \
  curl http://localhost:8081/actuator/health/readiness

# Loaded beans
kubectl exec accounts-5d7f8-xyz -n production -- \
  curl http://localhost:8081/actuator/beans
```

**Complete debugging checklist**
```yaml
✅ Pod status (get, describe)
✅ Current and previous logs
✅ Namespace events
✅ Sufficient resources (CPU/RAM)?
✅ Image exists and pullable?
✅ ConfigMaps/Secrets mounted correctly?
✅ Correct environment variables?
✅ Network connectivity (DB, other services)?
✅ DNS working?
✅ Appropriate probes (liveness/readiness)?
✅ Permissions (RBAC, SecurityContext)?
✅ PVC bound correctly?
✅ Init containers successful?
```

**Automated debugging script**
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

This methodical approach solves 95% of pods that won't start!
