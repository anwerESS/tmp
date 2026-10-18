## [**..**](./00_index.md)


# Chapitre 10 : CI/CD Integration

## 10.1. GitOps Principles

### Qu'est-ce que GitOps ?

GitOps est une approche où Git est la source unique de vérité pour l'infrastructure et les applications. Toute modification passe par Git.

**Principes fondamentaux :**

- **Declarative** : L'état désiré du système est décrit de manière déclarative
- **Versioned & Immutable** : L'état désiré est versionné dans Git et immuable
- **Pulled Automatically** : Des agents approuvent et appliquent automatiquement les changements
- **Continuously Reconciled** : L'état réel converge continuellement vers l'état désiré

**Avantages :**

- Traçabilité complète (Git history)
- Rollback facilité (git revert)
- Disaster recovery simplifié
- Audit trail automatique
- Collaboration via Pull Requests

**Outils populaires :**

- ArgoCD
- Flux CD
- Jenkins X
- Tekton

### Workflow GitOps typique

```
Developer → Git Push → CI Pipeline → Build Image → Update Manifest → 
GitOps Operator → Detect Change → Apply to Cluster
```

---

## 10.2. Deployment Strategies

### Blue/Green Deployment

**Concept :**
Deux environnements identiques (Blue = production actuelle, Green = nouvelle version). On bascule le trafic instantanément.

**Mise en œuvre dans Kubernetes :**

```yaml
# Service pointe vers Blue
apiVersion: v1
kind: Service
metadata:
  name: my-app
spec:
  selector:
    app: my-app
    version: blue  # Changer vers "green" pour basculer
  ports:
    - port: 80
---
# Deployment Blue
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app-blue
spec:
  replicas: 3
  selector:
    matchLabels:
      app: my-app
      version: blue
  template:
    metadata:
      labels:
        app: my-app
        version: blue
    spec:
      containers:
      - name: app
        image: myapp:v1.0
---
# Deployment Green
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app-green
spec:
  replicas: 3
  selector:
    matchLabels:
      app: my-app
      version: green
  template:
    metadata:
      labels:
        app: my-app
        version: green
    spec:
      containers:
      - name: app
        image: myapp:v2.0
```

**Basculement :**

```bash
# Modifier le Service pour pointer vers green
kubectl patch service my-app -p '{"spec":{"selector":{"version":"green"}}}'

# Rollback si problème
kubectl patch service my-app -p '{"spec":{"selector":{"version":"blue"}}}'
```

**Avantages :**
- Zero downtime
- Rollback instantané
- Testing en production (Green) avant basculement

**Inconvénients :**
- Double des ressources nécessaires
- Gestion des bases de données complexe

### Canary Deployment

**Concept :**
Déploiement progressif de la nouvelle version à un petit pourcentage d'utilisateurs, puis augmentation graduelle.

**Mise en œuvre :**

```yaml
# Deployment stable (v1) - 90% du trafic
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app-stable
spec:
  replicas: 9
  selector:
    matchLabels:
      app: my-app
      track: stable
  template:
    metadata:
      labels:
        app: my-app
        track: stable
    spec:
      containers:
      - name: app
        image: myapp:v1.0
---
# Deployment canary (v2) - 10% du trafic
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app-canary
spec:
  replicas: 1
  selector:
    matchLabels:
      app: my-app
      track: canary
  template:
    metadata:
      labels:
        app: my-app
        track: canary
    spec:
      containers:
      - name: app
        image: myapp:v2.0
---
# Service commun
apiVersion: v1
kind: Service
metadata:
  name: my-app
spec:
  selector:
    app: my-app  # Sélectionne stable ET canary
  ports:
    - port: 80
```

**Progression du Canary :**

```bash
# 10% canary (1 replica canary, 9 stable)
kubectl scale deployment my-app-canary --replicas=1
kubectl scale deployment my-app-stable --replicas=9

# 50% canary
kubectl scale deployment my-app-canary --replicas=5
kubectl scale deployment my-app-stable --replicas=5

# 100% canary (promotion complète)
kubectl scale deployment my-app-canary --replicas=10
kubectl scale deployment my-app-stable --replicas=0
```

**Avec Flagger (automatisation) :**

```yaml
apiVersion: flagger.app/v1beta1
kind: Canary
metadata:
  name: my-app
spec:
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: my-app
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

**Avantages :**
- Risque réduit
- Détection précoce de problèmes
- Rollback affecte peu d'utilisateurs

**Inconvénients :**
- Plus complexe à mettre en œuvre
- Nécessite monitoring avancé

### Rolling Update (par défaut Kubernetes)

**Configuration fine :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app
spec:
  replicas: 10
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxSurge: 2        # 2 pods supplémentaires max pendant l'update
      maxUnavailable: 1  # 1 pod peut être indisponible
  template:
    spec:
      containers:
      - name: app
        image: myapp:v2.0
        readinessProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 5
          periodSeconds: 5
```

**Contrôle du rollout :**

```bash
# Lancer le rollout
kubectl set image deployment/my-app app=myapp:v2.0

# Suivre la progression
kubectl rollout status deployment/my-app

# Pause du rollout
kubectl rollout pause deployment/my-app

# Reprise
kubectl rollout resume deployment/my-app

# Rollback
kubectl rollout undo deployment/my-app
```

---

## 10.3. Integration avec Jenkins/GitLab CI/ArgoCD

### Pipeline Jenkins pour Kubernetes

**Jenkinsfile exemple :**

```groovy
pipeline {
    agent {
        kubernetes {
            yaml '''
apiVersion: v1
kind: Pod
spec:
  containers:
  - name: maven
    image: maven:3.8-openjdk-17
    command: ['cat']
    tty: true
  - name: docker
    image: docker:20.10
    command: ['cat']
    tty: true
    volumeMounts:
    - name: docker-sock
      mountPath: /var/run/docker.sock
  - name: kubectl
    image: bitnami/kubectl:latest
    command: ['cat']
    tty: true
  volumes:
  - name: docker-sock
    hostPath:
      path: /var/run/docker.sock
'''
        }
    }
    
    environment {
        DOCKER_REGISTRY = 'docker.io'
        IMAGE_NAME = 'mycompany/myapp'
        IMAGE_TAG = "${env.BUILD_NUMBER}"
        KUBECONFIG = credentials('kubeconfig-prod')
    }
    
    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }
        
        stage('Build & Test') {
            steps {
                container('maven') {
                    sh '''
                        mvn clean package
                        mvn test
                    '''
                }
            }
        }
        
        stage('Build Docker Image') {
            steps {
                container('docker') {
                    script {
                        docker.withRegistry("https://${DOCKER_REGISTRY}", 'docker-credentials') {
                            def image = docker.build("${IMAGE_NAME}:${IMAGE_TAG}")
                            image.push()
                            image.push('latest')
                        }
                    }
                }
            }
        }
        
        stage('Deploy to Kubernetes') {
            steps {
                container('kubectl') {
                    sh '''
                        kubectl set image deployment/my-app \
                          app=${IMAGE_NAME}:${IMAGE_TAG} \
                          --record
                        
                        kubectl rollout status deployment/my-app
                    '''
                }
            }
        }
    }
    
    post {
        success {
            echo 'Deployment successful!'
        }
        failure {
            echo 'Deployment failed!'
            container('kubectl') {
                sh 'kubectl rollout undo deployment/my-app'
            }
        }
    }
}
```

### GitLab CI pour Kubernetes

**.gitlab-ci.yml :**

```yaml
stages:
  - build
  - test
  - package
  - deploy

variables:
  MAVEN_OPTS: "-Dmaven.repo.local=$CI_PROJECT_DIR/.m2/repository"
  DOCKER_IMAGE: $CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA
  KUBECONFIG: /etc/deploy/config

build:
  stage: build
  image: maven:3.8-openjdk-17
  script:
    - mvn clean compile
  cache:
    paths:
      - .m2/repository
      - target/

test:
  stage: test
  image: maven:3.8-openjdk-17
  script:
    - mvn test
  cache:
    paths:
      - .m2/repository
  artifacts:
    reports:
      junit: target/surefire-reports/TEST-*.xml

package:
  stage: package
  image: maven:3.8-openjdk-17
  script:
    - mvn package -DskipTests
  artifacts:
    paths:
      - target/*.jar

docker-build:
  stage: package
  image: docker:20.10
  services:
    - docker:20.10-dind
  before_script:
    - docker login -u $CI_REGISTRY_USER -p $CI_REGISTRY_PASSWORD $CI_REGISTRY
  script:
    - docker build -t $DOCKER_IMAGE .
    - docker push $DOCKER_IMAGE
    - docker tag $DOCKER_IMAGE $CI_REGISTRY_IMAGE:latest
    - docker push $CI_REGISTRY_IMAGE:latest

deploy-staging:
  stage: deploy
  image: bitnami/kubectl:latest
  environment:
    name: staging
  before_script:
    - mkdir -p /etc/deploy
    - echo $KUBE_CONFIG | base64 -d > $KUBECONFIG
  script:
    - kubectl config use-context staging
    - kubectl set image deployment/my-app app=$DOCKER_IMAGE -n staging
    - kubectl rollout status deployment/my-app -n staging
  only:
    - develop

deploy-production:
  stage: deploy
  image: bitnami/kubectl:latest
  environment:
    name: production
  before_script:
    - mkdir -p /etc/deploy
    - echo $KUBE_CONFIG | base64 -d > $KUBECONFIG
  script:
    - kubectl config use-context production
    - kubectl set image deployment/my-app app=$DOCKER_IMAGE -n production
    - kubectl rollout status deployment/my-app -n production
  when: manual
  only:
    - main
```

### ArgoCD - GitOps Approach

**Installation ArgoCD :**

```bash
# Création namespace
kubectl create namespace argocd

# Installation
kubectl apply -n argocd -f https://raw.githubusercontent.com/argoproj/argo-cd/stable/manifests/install.yaml

# Accès UI
kubectl port-forward svc/argocd-server -n argocd 8080:443

# Récupération password admin
kubectl -n argocd get secret argocd-initial-admin-secret -o jsonpath="{.data.password}" | base64 -d
```

**Application ArgoCD :**

```yaml
apiVersion: argoproj.io/v1alpha1
kind: Application
metadata:
  name: my-spring-app
  namespace: argocd
spec:
  project: default
  
  source:
    repoURL: https://github.com/mycompany/k8s-manifests
    targetRevision: HEAD
    path: apps/my-spring-app
    
    # Support Helm
    helm:
      valueFiles:
        - values-prod.yaml
      parameters:
        - name: image.tag
          value: "v2.1.0"
  
  destination:
    server: https://kubernetes.default.svc
    namespace: production
  
  syncPolicy:
    automated:
      prune: true      # Supprime ressources non définies
      selfHeal: true   # Corrige les drifts
      allowEmpty: false
    syncOptions:
      - CreateNamespace=true
    retry:
      limit: 5
      backoff:
        duration: 5s
        factor: 2
        maxDuration: 3m
  
  # Health checks personnalisés
  ignoreDifferences:
    - group: apps
      kind: Deployment
      jsonPointers:
        - /spec/replicas  # Ignore HPA changes
```

**Structure repository GitOps :**

```
k8s-manifests/
├── apps/
│   ├── my-spring-app/
│   │   ├── base/
│   │   │   ├── deployment.yaml
│   │   │   ├── service.yaml
│   │   │   ├── configmap.yaml
│   │   │   └── kustomization.yaml
│   │   ├── overlays/
│   │   │   ├── staging/
│   │   │   │   ├── kustomization.yaml
│   │   │   │   └── patch-replicas.yaml
│   │   │   └── production/
│   │   │       ├── kustomization.yaml
│   │   │       └── patch-replicas.yaml
│   │   └── values-prod.yaml
└── argocd/
    └── applications/
        └── my-spring-app.yaml
```

**Workflow avec ArgoCD :**

1. Developer push code → Git
2. CI pipeline build & test → Create Docker image
3. CI pipeline update manifest → Push to k8s-manifests repo
4. ArgoCD detect change → Sync automatically
5. Application deployed

**CLI ArgoCD :**

```bash
# Login
argocd login localhost:8080

# Créer application
argocd app create my-app --repo https://github.com/mycompany/k8s-manifests \
  --path apps/my-spring-app --dest-namespace production \
  --dest-server https://kubernetes.default.svc

# Sync manual
argocd app sync my-app

# Status
argocd app get my-app

# Rollback
argocd app rollback my-app

# Logs
argocd app logs my-app
```

---

## 10.4. Container Image Best Practices

### Optimisation Images Spring Boot

**Multi-stage Dockerfile :**

```dockerfile
# Stage 1: Build
FROM maven:3.8-openjdk-17-slim AS builder
WORKDIR /app

# Copy dependency files first (cache layer)
COPY pom.xml .
COPY src ./src

# Build application
RUN mvn clean package -DskipTests

# Stage 2: Runtime
FROM eclipse-temurin:17-jre-alpine

# Non-root user
RUN addgroup -g 1001 appuser && \
    adduser -D -u 1001 -G appuser appuser

WORKDIR /app

# Copy only JAR from builder
COPY --from=builder /app/target/*.jar app.jar

# Security & optimization
RUN chown appuser:appuser app.jar

USER appuser

# JVM optimization pour containers
ENV JAVA_OPTS="-XX:+UseContainerSupport \
               -XX:MaxRAMPercentage=75.0 \
               -XX:+UseG1GC \
               -XX:+UseStringDeduplication"

EXPOSE 8080

ENTRYPOINT ["sh", "-c", "java $JAVA_OPTS -jar app.jar"]
```

**Layered JARs (Spring Boot 2.3+) :**

```dockerfile
FROM eclipse-temurin:17-jre-alpine AS builder
WORKDIR /app
COPY target/*.jar app.jar
RUN java -Djarmode=layertools -jar app.jar extract

FROM eclipse-temurin:17-jre-alpine
WORKDIR /app

RUN addgroup -g 1001 appuser && adduser -D -u 1001 -G appuser appuser

# Copy layers (optimisation cache Docker)
COPY --from=builder /app/dependencies/ ./
COPY --from=builder /app/spring-boot-loader/ ./
COPY --from=builder /app/snapshot-dependencies/ ./
COPY --from=builder /app/application/ ./

RUN chown -R appuser:appuser /app
USER appuser

ENV JAVA_OPTS="-XX:+UseContainerSupport -XX:MaxRAMPercentage=75.0"

ENTRYPOINT ["sh", "-c", "java $JAVA_OPTS org.springframework.boot.loader.JarLauncher"]
```

### Image Security Scanning

**Avec Trivy :**

```bash
# Scan locale
trivy image myapp:latest

# Dans CI/CD
trivy image --severity HIGH,CRITICAL --exit-code 1 myapp:latest
```

**GitLab CI avec scan :**

```yaml
container-scan:
  stage: package
  image: aquasec/trivy:latest
  script:
    - trivy image --severity HIGH,CRITICAL --no-progress $DOCKER_IMAGE
  dependencies:
    - docker-build
  allow_failure: false
```

### Image Tagging Strategy

**Bonnes pratiques :**

```bash
# ❌ Éviter
docker tag myapp:latest

# ✅ Préférer
docker tag myapp:v1.2.3              # Semantic versioning
docker tag myapp:v1.2.3-abc123       # Version + Git commit
docker tag myapp:20240115-142530     # Timestamp
docker tag myapp:${CI_COMMIT_SHA}    # Git SHA

# Multi-tags
docker tag myapp:v1.2.3
docker tag myapp:v1.2
docker tag myapp:v1
docker tag myapp:stable
```

### Image Registry Best Practices

**Configuration imagePullSecrets :**

```bash
# Créer secret
kubectl create secret docker-registry regcred \
  --docker-server=docker.io \
  --docker-username=myuser \
  --docker-password=mypassword \
  --docker-email=myemail@example.com

# Utiliser dans Deployment
```

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app
spec:
  template:
    spec:
      imagePullSecrets:
      - name: regcred
      containers:
      - name: app
        image: docker.io/mycompany/myapp:v1.2.3
        imagePullPolicy: IfNotPresent  # Always / IfNotPresent / Never
```

### Health Checks dans Images

**Application Spring Boot :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  template:
    spec:
      containers:
      - name: app
        image: myapp:v1.0
        ports:
        - containerPort: 8080
        
        # Startup probe (démarrage lent)
        startupProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8080
          initialDelaySeconds: 0
          periodSeconds: 5
          failureThreshold: 30  # 150s max pour démarrer
        
        # Liveness probe (redémarrage si KO)
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8080
          initialDelaySeconds: 10
          periodSeconds: 10
          timeoutSeconds: 5
          failureThreshold: 3
        
        # Readiness probe (trafic seulement si OK)
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          initialDelaySeconds: 5
          periodSeconds: 5
          timeoutSeconds: 3
          failureThreshold: 3
        
        # Resources
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
```

**Configuration Spring Boot Actuator :**

```yaml
# application.yml
management:
  endpoints:
    web:
      exposure:
        include: health,info,prometheus
  endpoint:
    health:
      probes:
        enabled: true
      show-details: when-authorized
  health:
    livenessState:
      enabled: true
    readinessState:
      enabled: true
```

---

## Récapitulatif du Chapitre 10

✅ **Points clés :**

- **GitOps** : Git comme source de vérité, déploiement automatisé
- **Deployment Strategies** : Blue/Green, Canary, Rolling Update
- **CI/CD Tools** : Jenkins, GitLab CI, ArgoCD
- **Image Best Practices** : Multi-stage builds, security scanning, tagging strategy

**Questions d'entretien fréquentes :**

1. Expliquez la différence entre Blue/Green et Canary deployment
2. Comment géreriez-vous un rollback en production ?
3. Qu'est-ce que GitOps et quels sont ses avantages ?
4. Comment optimiser une image Docker Spring Boot ?
5. Quelle est votre expérience avec ArgoCD/FluxCD ?
6. Comment intégrer le security scanning dans votre pipeline ?

**Commandes à maîtriser :**

```bash
# Rollout management
kubectl rollout status deployment/my-app
kubectl rollout pause deployment/my-app
kubectl rollout resume deployment/my-app
kubectl rollout undo deployment/my-app
kubectl rollout history deployment/my-app

# Image update
kubectl set image deployment/my-app app=myapp:v2.0

# ArgoCD
argocd app sync my-app
argocd app get my-app
argocd app rollback my-app
```
---

# 🎯 Chapitre 10 : Questions/Réponses d'Entretien
## CI/CD Integration avec Kubernetes

---

## 📋 Section 1 : GitOps Principles

### Q1 : Qu'est-ce que GitOps et quels sont ses principaux avantages ?

**Réponse :**

GitOps est une approche où Git devient la source unique de vérité (single source of truth) pour l'infrastructure et les applications. Tous les changements passent par Git via des Pull Requests.

**Les 4 principes fondamentaux :**
1. **Declarative** : L'état désiré est décrit de manière déclarative (YAML)
2. **Versioned & Immutable** : Tout est versionné dans Git
3. **Pulled Automatically** : Des agents (ArgoCD, Flux) appliquent automatiquement les changements
4. **Continuously Reconciled** : L'état réel converge continuellement vers l'état désiré

**Avantages principaux :**
- **Traçabilité complète** : Chaque changement est tracé dans Git history
- **Rollback facilité** : Simple `git revert` pour revenir en arrière
- **Disaster recovery** : Reconstruction cluster à partir de Git
- **Collaboration** : Revue de code via Pull Requests
- **Audit trail** : Qui a fait quoi et quand
- **Sécurité** : Pas de `kubectl apply` manuel, credentials Kubernetes non partagés

**Exemple concret :**
Au lieu de faire `kubectl apply -f deployment.yaml`, je commit le fichier dans Git, et ArgoCD détecte le changement et déploie automatiquement.

---

### Q2 : Quelle est la différence entre Push-based et Pull-based CI/CD ?

**Réponse :**

**Push-based (approche traditionnelle) :**
- Le pipeline CI/CD **push** les changements vers le cluster
- Jenkins/GitLab CI exécute `kubectl apply` directement
- Nécessite credentials Kubernetes dans CI/CD
- Le cluster est "passif"

```
Jenkins → kubectl apply → Kubernetes Cluster
```

**Pull-based (GitOps) :**
- Un agent **dans le cluster** (ArgoCD, Flux) pull les changements depuis Git
- Le cluster est "actif" et se synchronise lui-même
- Pas besoin de credentials Kubernetes hors du cluster
- Plus sécurisé

```
ArgoCD (in cluster) → Git Pull → Compare → Apply changes
```

**Avantages Pull-based :**
- Meilleure sécurité (credentials restent dans le cluster)
- Self-healing automatique (drift detection)
- Multi-cluster plus facile
- Audit natif

**Mon expérience :**
J'ai travaillé avec les deux approches. Pour les petits projets, push-based avec GitLab CI suffit. Pour des environnements production complexes, je préfère ArgoCD pull-based pour sa sécurité et son auto-healing.

---

### Q3 : Comment gérez-vous les secrets dans une approche GitOps ?

**Réponse :**

C'est un défi car on ne peut pas stocker les secrets en clair dans Git !

**Solutions :**

**1. Sealed Secrets (Bitnami) :**
```bash
# Encrypt secret
kubeseal --format=yaml < secret.yaml > sealed-secret.yaml

# Commit sealed-secret.yaml dans Git
# Le controller dans le cluster le déchiffre automatiquement
```

```yaml
apiVersion: bitnami.com/v1alpha1
kind: SealedSecret
metadata:
  name: my-secret
spec:
  encryptedData:
    password: AgBg7Yx9f... # Encrypted, safe for Git
```

**2. External Secrets Operator :**
```yaml
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: my-app-secret
spec:
  refreshInterval: 1h
  secretStoreRef:
    name: aws-secrets-manager
    kind: SecretStore
  target:
    name: my-app-secret
  data:
    - secretKey: database-password
      remoteRef:
        key: prod/db/password
```

**3. SOPS (Mozilla) :**
Chiffrement des fichiers YAML avec clés PGP/KMS :
```bash
sops --encrypt secret.yaml > secret.enc.yaml
```

**4. HashiCorp Vault :**
Integration directe avec Kubernetes via Vault Agent Injector.

**Ma préférence :**
Pour AWS, j'utilise External Secrets Operator avec AWS Secrets Manager. Pour on-premise, Sealed Secrets est simple et efficace.

---

### Q4 : Comment ArgoCD détecte-t-il les changements et à quelle fréquence ?

**Réponse :**

ArgoCD utilise deux mécanismes :

**1. Polling (par défaut) :**
- Vérifie Git toutes les **3 minutes** par défaut
- Configurable via `timeout.reconciliation`

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: argocd-cm
data:
  timeout.reconciliation: 60s  # Check every 60s
```

**2. Webhook (temps réel) :**
Configuration d'un webhook Git → ArgoCD :

```bash
# Dans GitHub/GitLab
Webhook URL: https://argocd.example.com/api/webhook
Payload: Push events
```

**Drift Detection (auto-healing) :**
Si quelqu'un fait `kubectl edit` directement dans le cluster, ArgoCD détecte la différence (OutOfSync) et peut :
- **Manual** : Alerter uniquement
- **Automated** : Corriger automatiquement

```yaml
spec:
  syncPolicy:
    automated:
      selfHeal: true  # Corrige automatiquement les drifts
      prune: true     # Supprime ressources non définies dans Git
```

**Exemple pratique :**
Un développeur modifie manuellement le nombre de replicas :
```bash
kubectl scale deployment my-app --replicas=10
```
Avec `selfHeal: true`, ArgoCD restaure automatiquement la valeur définie dans Git (ex: 3 replicas) en ~3 minutes.

---

## 📋 Section 2 : Deployment Strategies

### Q5 : Expliquez la différence entre Blue/Green et Canary deployment

**Réponse :**

**Blue/Green Deployment :**
- **Deux environnements complets** (Blue = actuel, Green = nouveau)
- **Basculement instantané** à 100% du trafic
- **Rollback instantané** (juste changer le Service selector)

```yaml
# Basculement
kubectl patch service my-app -p '{"spec":{"selector":{"version":"green"}}}'
```

**Avantages :**
- Zero downtime
- Rollback en secondes
- Tests complets sur Green avant basculement

**Inconvénients :**
- **Coûteux** : double des ressources
- Difficile pour bases de données (schema migrations)

---

**Canary Deployment :**
- **Déploiement progressif** (5% → 25% → 50% → 100%)
- **Cohabitation** ancienne et nouvelle version
- **Monitoring continu** entre chaque étape

```bash
# 10% canary, 90% stable
kubectl scale deployment my-app-canary --replicas=1
kubectl scale deployment my-app-stable --replicas=9

# Si OK, augmenter progressivement
kubectl scale deployment my-app-canary --replicas=5
kubectl scale deployment my-app-stable --replicas=5
```

**Avantages :**
- **Risque réduit** : impact limité si problème
- Détection précoce d'anomalies
- Ressources optimisées

**Inconvénients :**
- Plus complexe (nécessite métriques)
- Durée de déploiement plus longue
- Besoin de feature flags parfois

---

**Quand utiliser quoi ?**

| Critère | Blue/Green | Canary |
|---------|-----------|--------|
| Budget | ✅ Élevé | ✅ Modéré |
| Criticité | ✅ Apps critiques | ✅ Très critique |
| Complexité | Simple | Complexe |
| Testing | Complet avant switch | Progressif en prod |
| Rollback | Instantané | Par étapes |

**Mon expérience :**
Pour une API interne peu critique, Blue/Green suffit. Pour une API client avec SLA strict, je préfère Canary avec Flagger pour automatiser la progression basée sur des métriques (error rate, latency).

---

### Q6 : Comment implémenteriez-vous un Canary deployment avec monitoring automatique ?

**Réponse :**

J'utiliserais **Flagger** avec **Prometheus** pour automatiser complètement le processus.

**Architecture :**

```
Developer → Git Push → CI Build Image → Update Manifest → 
ArgoCD Deploy Flagger Canary → Flagger contrôle progression → 
Prometheus collecte métriques → Flagger décide (promote/rollback)
```

**Configuration Flagger :**

```yaml
apiVersion: flagger.app/v1beta1
kind: Canary
metadata:
  name: my-spring-app
  namespace: production
spec:
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: my-spring-app
  
  service:
    port: 8080
    targetPort: 8080
  
  analysis:
    # Durée entre chaque étape
    interval: 1m
    
    # Nombre d'échecs tolérés avant rollback
    threshold: 5
    
    # Trafic maximum vers canary
    maxWeight: 50
    
    # Incrément de trafic à chaque étape
    stepWeight: 10
    
    # Métriques à surveiller
    metrics:
    
    # 1. Success rate doit être > 99%
    - name: request-success-rate
      thresholdRange:
        min: 99
      interval: 1m
    
    # 2. Latency p99 doit être < 500ms
    - name: request-duration
      thresholdRange:
        max: 500
      interval: 1m
    
    # 3. Error rate doit être < 1%
    - name: error-rate
      templateRef:
        name: error-rate
        namespace: flagger-system
      thresholdRange:
        max: 1
      interval: 1m
    
    # Webhooks (optionnel)
    webhooks:
    # Test de fumée avant de commencer
    - name: smoke-test
      type: pre-rollout
      url: http://flagger-loadtester.test/
      timeout: 5s
      metadata:
        type: bash
        cmd: "curl -sd 'test' http://my-spring-app-canary:8080/health | grep OK"
    
    # Tests de charge pendant le canary
    - name: load-test
      type: rollout
      url: http://flagger-loadtester.test/
      timeout: 5s
      metadata:
        type: cmd
        cmd: "hey -z 1m -q 10 -c 2 http://my-spring-app-canary:8080/api/users"
    
    # Notification Slack
    - name: slack-notification
      type: post-rollout
      url: https://hooks.slack.com/services/YOUR/WEBHOOK
      metadata:
        message: "Canary deployment terminé pour my-spring-app"
```

**Progression automatique :**

```
1. Deploy new version (canary)
2. Run smoke tests (pre-rollout webhook)
3. Route 10% traffic → Wait 1m → Check metrics
4. If OK: 20% → Wait 1m → Check metrics
5. If OK: 30% → Wait 1m → Check metrics
6. If OK: 40% → Wait 1m → Check metrics
7. If OK: 50% → Wait 1m → Check metrics
8. If OK: Promote (100% canary becomes primary)
9. If KO at any step: Automatic rollback
```

**PromQL pour métriques custom :**

```yaml
apiVersion: flagger.app/v1beta1
kind: MetricTemplate
metadata:
  name: error-rate
  namespace: flagger-system
spec:
  provider:
    type: prometheus
    address: http://prometheus:9090
  query: |
    100 - (
      sum(rate(http_requests_total{status!~"5.."}[1m]))
      /
      sum(rate(http_requests_total[1m]))
    ) * 100
```

**Monitoring avec Spring Boot Actuator :**

```yaml
# application.yml
management:
  endpoints:
    web:
      exposure:
        include: health,info,prometheus,metrics
  metrics:
    export:
      prometheus:
        enabled: true
    tags:
      application: ${spring.application.name}
```

**Avantages de cette approche :**
- ✅ Aucune intervention manuelle
- ✅ Rollback automatique si problème
- ✅ Tests de charge automatiques
- ✅ Notifications Slack
- ✅ Décisions basées sur data réelles

---

### Q7 : Comment gérez-vous un rollback en production ? Décrivez différentes situations.

**Réponse :**

Je distingue plusieurs scénarios :

**1. Rollback immédiat (problème critique détecté) :**

**Situation :** Erreur 500, crash loop, bug majeur détecté en production.

```bash
# Rollback instantané vers version précédente
kubectl rollout undo deployment/my-app

# Rollback vers version spécifique
kubectl rollout history deployment/my-app
kubectl rollout undo deployment/my-app --to-revision=5

# Vérification
kubectl rollout status deployment/my-app
```

**Avec GitOps (ArgoCD) :**
```bash
# Via Git (méthode propre)
git revert HEAD
git push

# Ou via ArgoCD CLI (urgence)
argocd app rollback my-app

# Ou via UI ArgoCD: History → Select previous revision → Rollback
```

---

**2. Rollback progressif (problème détecté en Canary) :**

**Situation :** Métrique dégradée (latence, error rate) détectée pendant un Canary deployment.

**Avec Flagger :**
Automatique ! Flagger rollback automatiquement si métriques hors seuil.

**Manuel :**
```bash
# Réduire trafic canary progressivement
kubectl scale deployment my-app-canary --replicas=1
kubectl scale deployment my-app-stable --replicas=9

# Puis supprimer canary
kubectl scale deployment my-app-canary --replicas=0
```

---

**3. Rollback Blue/Green :**

**Situation :** Bug découvert après switch Blue → Green.

```bash
# Basculer le Service vers Blue (ancienne version)
kubectl patch service my-app -p '{"spec":{"selector":{"version":"blue"}}}'

# Instantané, zero downtime
```

---

**4. Rollback avec données corrompues :**

**Situation :** La nouvelle version a corrompu des données en BDD.

**Process :**
1. **Rollback applicatif** immédiat (voir ci-dessus)
2. **Restauration BDD** depuis backup
3. **Post-mortem** pour comprendre
4. **Tests améliorés** pour éviter récurrence

```bash
# Exemple restoration PostgreSQL
kubectl exec -it postgres-0 -- pg_restore -d mydb /backups/mydb_before_deploy.dump
```

---

**5. Rollback d'un StatefulSet :**

**Situation :** Plus délicat car données persistantes.

```bash
# Rollback contrôlé partition par partition
kubectl rollout undo statefulset/kafka --to-revision=3

# Vérifier chaque pod avant de continuer
kubectl get pods -l app=kafka -w
```

---

**Best practices pour faciliter rollback :**

1. **Feature Flags** : Désactiver feature sans redéployer
```java
@Configuration
public class FeatureConfig {
    @Value("${feature.new-algorithm.enabled:false}")
    private boolean newAlgorithmEnabled;
    
    @Bean
    public FeatureToggle featureToggle() {
        return new FeatureToggle(newAlgorithmEnabled);
    }
}
```

2. **Database migrations backward-compatible**
```sql
-- ✅ Good: Add column with default
ALTER TABLE users ADD COLUMN phone VARCHAR(20) DEFAULT '';

-- ❌ Bad: Remove column immediately
ALTER TABLE users DROP COLUMN phone;
```

3. **Immutable deployments** : Nouvelles versions = nouveaux Deployments
4. **Canary avec auto-rollback** (Flagger)
5. **Monitoring robuste** : Alertes précoces

---

**Monitoring post-rollback :**
```bash
# Vérifier logs
kubectl logs -l app=my-app --tail=100 -f

# Vérifier events
kubectl get events --sort-by='.lastTimestamp'

# Vérifier métriques
# → Grafana dashboards (error rate, latency, throughput)
```

**Mon expérience :**
J'ai eu à faire un rollback en production sur un bug de performance (requêtes SQL non optimisées). J'ai utilisé `kubectl rollout undo`, vérifié les métriques Prometheus, puis analysé les logs pour identifier la requête problématique. Le tout en moins de 5 minutes grâce à GitOps et monitoring.

---

## 📋 Section 3 : CI/CD Integration

### Q8 : Décrivez votre pipeline CI/CD idéal pour une application Spring Boot déployée sur Kubernetes

**Réponse :**

Voici mon pipeline complet, que j'ai implémenté avec GitLab CI + ArgoCD :

**Architecture globale :**

```
Git Push → GitLab CI (Build & Test) → Docker Registry → 
Update Manifest Repo → ArgoCD (Deploy) → Kubernetes → 
Prometheus (Monitor) → AlertManager (Alert)
```

---

**Pipeline détaillé (.gitlab-ci.yml) :**

**Stage 1 : Build & Test (5-7 min)**

```yaml
build:
  stage: build
  image: maven:3.8-openjdk-17
  script:
    - mvn clean compile
  cache:
    key: ${CI_COMMIT_REF_SLUG}
    paths:
      - .m2/repository
      - target/
  artifacts:
    paths:
      - target/
    expire_in: 1 hour

test:
  stage: test
  image: maven:3.8-openjdk-17
  script:
    - mvn test
    - mvn verify
  coverage: '/Total.*?([0-9]{1,3})%/'
  artifacts:
    reports:
      junit: target/surefire-reports/TEST-*.xml
      coverage_report:
        coverage_format: cobertura
        path: target/site/cobertura/coverage.xml
```

**Stage 2 : Code Quality & Security (3-5 min)**

```yaml
sonarqube-check:
  stage: test
  image: maven:3.8-openjdk-17
  script:
    - mvn sonar:sonar
      -Dsonar.host.url=$SONAR_HOST_URL
      -Dsonar.login=$SONAR_TOKEN
  only:
    - develop
    - main

dependency-check:
  stage: test
  image: owasp/dependency-check:latest
  script:
    - /usr/share/dependency-check/bin/dependency-check.sh
      --scan . --format HTML --out reports/
  artifacts:
    paths:
      - reports/
```

**Stage 3 : Package & Push Image (2-3 min)**

```yaml
docker-build:
  stage: package
  image: docker:20.10
  services:
    - docker:20.10-dind
  before_script:
    - docker login -u $CI_REGISTRY_USER -p $CI_REGISTRY_PASSWORD $CI_REGISTRY
  script:
    # Build multi-stage Dockerfile
    - docker build -t $CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA .
    - docker tag $CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA $CI_REGISTRY_IMAGE:latest
    
    # Security scan
    - docker run --rm -v /var/run/docker.sock:/var/run/docker.sock
      aquasec/trivy image --severity HIGH,CRITICAL
      $CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA
    
    # Push
    - docker push $CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA
    - docker push $CI_REGISTRY_IMAGE:latest
```

**Stage 4 : Update Manifest (GitOps) (30s)**

```yaml
update-manifest:
  stage: deploy
  image: alpine/git
  before_script:
    - apk add --no-cache yq
  script:
    # Clone manifest repository
    - git clone https://$DEPLOY_TOKEN@gitlab.com/company/k8s-manifests.git
    - cd k8s-manifests
    
    # Update image tag with yq
    - yq eval ".spec.template.spec.containers[0].image = 
      \"$CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA\"" 
      -i apps/my-spring-app/base/deployment.yaml
    
    # Commit & Push
    - git config user.email "ci@company.com"
    - git config user.name "GitLab CI"
    - git add .
    - git commit -m "Update image to $CI_COMMIT_SHORT_SHA"
    - git push origin main
  only:
    - main
```

**Stage 5 : Deploy (ArgoCD auto-sync) (1-2 min)**

ArgoCD détecte le changement dans le manifest repo et déploie automatiquement.

```yaml
verify-deployment:
  stage: verify
  image: argoproj/argocd:latest
  script:
    # Wait for ArgoCD sync
    - argocd app wait my-spring-app --timeout 300
    
    # Check health
    - argocd app get my-spring-app
  only:
    - main
```

**Stage 6 : Smoke Tests (1 min)**

```yaml
smoke-tests:
  stage: verify
  image: curlimages/curl:latest
  script:
    # Health check
    - curl -f https://my-app.company.com/actuator/health || exit 1
    
    # Basic API test
    - curl -f https://my-app.company.com/api/users?limit=1 || exit 1
  only:
    - main
```

---

**Notifications :**

```yaml
slack-notification:
  stage: .post
  image: alpine
  script:
    - apk add --no-cache curl
    - |
      curl -X POST $SLACK_WEBHOOK \
      -H 'Content-Type: application/json' \
      -d "{
        \"text\": \"✅ Deployment successful: ${CI_PROJECT_NAME} ${CI_COMMIT_SHORT_SHA}\",
        \"channel\": \"#deployments\"
      }"
  when: on_success
  only:
    - main
```

---

**Résumé temps :**
- Build & Test: 5-7 min
- Quality & Security: 3-5 min
- Docker Build: 2-3 min
- Update Manifest: 30s
- Deploy (ArgoCD): 1-2 min
- Smoke Tests: 1 min

**Total : ~15 minutes** de commit à production.

---

**Ce que j'aime dans ce pipeline :**

1. ✅ **Sécurité** : Scan Trivy, OWASP dependency-check, SonarQube
2. ✅ **GitOps** : Séparation code app / code infra
3. ✅ **Traçabilité** : Git history + ArgoCD UI
4. ✅ **Automatisation** : Zero touch après merge
5. ✅ **Rollback** : Simple git revert
6. ✅ **Multi-env** : Branches develop → staging, main → production

---

### Q9 : Comment gérez-vous les différents environnements (dev, staging, prod) dans votre CI/CD ?

**Réponse :**

J'utilise une approche **GitOps avec Kustomize overlays** pour gérer les environnements.

**Structure repository :**

```
k8s-manifests/
├── apps/
│   └── my-spring-app/
│       ├── base/                    # Configuration commune
│       │   ├── deployment.yaml
│       │   ├── service.yaml
│       │   ├── configmap.yaml
│       │   ├── hpa.yaml
│       │   └── kustomization.yaml
│       │
│       └── overlays/                # Configurations spécifiques
│           ├── dev/
│           │   ├── kustomization.yaml
│           │   ├── patch-replicas.yaml
│           │   ├── patch-resources.yaml
│           │   └── configmap-dev.yaml
│           │
│           ├── staging/
│           │   ├── kustomization.yaml
│           │   ├── patch-replicas.yaml
│           │   ├── patch-resources.yaml
│           │   └── configmap-staging.yaml
│           │
│           └── production/
│               ├── kustomization.yaml
│               ├── patch-replicas.yaml
│               ├── patch-resources.yaml
│               ├── configmap-prod.yaml
│               └── ingress-tls.yaml
```

---

**Base configuration (commune) :**

```yaml
# base/deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  replicas: 2  # Sera overridé par overlay
  selector:
    matchLabels:
      app: my-spring-app
  template:
    metadata:
      labels:
        app: my-spring-app
    spec:
      containers:
      - name: app
        image: registry.company.com/my-spring-app:latest  # Tag overridé par CI
        ports:
        - containerPort: 8080
        envFrom:
        - configMapRef:
            name: my-spring-app-config
        resources:
          requests:
            memory: "256Mi"
            cpu: "100m"
          limits:
            memory: "512Mi"
            cpu: "500m"
```

```yaml
# base/kustomization.yaml
apiVersion: kustomize.config.k8s.io/v1beta1
kind: Kustomization

resources:
  - deployment.yaml
  - service.yaml
  - configmap.yaml
  - hpa.yaml

commonLabels:
  app: my-spring-app
  managed-by: argocd
```

---

**Overlay DEV (ressources minimales) :**

```yaml
# overlays/dev/kustomization.yaml
apiVersion: kustomize.config.k8s.io/v1beta1
kind: Kustomization

namespace: dev

bases:
  - ../../base

patchesStrategicMerge:
  - patch-replicas.yaml
  - patch-resources.yaml

configMapGenerator:
  - name: my-spring-app-config
    behavior: merge
    literals:
      - SPRING_PROFILES_ACTIVE=dev
      - LOG_LEVEL=DEBUG
      - DATABASE_URL=jdbc:postgresql://postgres-dev:5432/myapp

images:
  - name: registry.company.com/my-spring-app
    newTag: develop-latest  # Auto-updated par CI
```

```yaml
# overlays/dev/patch-replicas.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  replicas: 1  # Une seule replica en dev
```

```yaml
# overlays/dev/patch-resources.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  template:
    spec:
      containers:
      - name: app
        resources:
          requests:
            memory: "128Mi"
            cpu: "50m"
          limits:
            memory: "256Mi"
            cpu: "200m"
```

---

**Overlay STAGING (ressources intermédiaires) :**

```yaml
# overlays/staging/kustomization.yaml
apiVersion: kustomize.config.k8s.io/v1beta1
kind: Kustomization

namespace: staging

bases:
  - ../../base

patchesStrategicMerge:
  - patch-replicas.yaml
  - patch-resources.yaml

configMapGenerator:
  - name: my-spring-app-config
    behavior: merge
    literals:
      - SPRING_PROFILES_ACTIVE=staging
      - LOG_LEVEL=INFO
      - DATABASE_URL=jdbc:postgresql://postgres-staging:5432/myapp

images:
  - name: registry.company.com/my-spring-app
    newTag: staging-v1.2.3  # Tag spécifique
```

```yaml
# overlays/staging/patch-replicas.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  replicas: 2  # 2 replicas en staging
```

---

**Overlay PRODUCTION (ressources complètes + HA) :**

```yaml
# overlays/production/kustomization.yaml
apiVersion: kustomize.config.k8s.io/v1beta1
kind: Kustomization

namespace: production

bases:
  - ../../base

patchesStrategicMerge:
  - patch-replicas.yaml
  - patch-resources.yaml
  - ingress-tls.yaml

configMapGenerator:
  - name: my-spring-app-config
    behavior: merge
    literals:
      - SPRING_PROFILES_ACTIVE=prod
      - LOG_LEVEL=WARN
      - DATABASE_URL=jdbc:postgresql://postgres-prod:5432/myapp
      - REDIS_ENABLED=true

images:
  - name: registry.company.com/my-spring-app
    newTag: v1.2.3  # Semantic versioning strict

# PodDisruptionBudget pour HA
patchesJson6902:
  - target:
      group: apps
      version: v1
      kind: Deployment
      name: my-spring-app
    patch: |-
      - op: add
        path: /spec/template/spec/affinity
        value:
          podAntiAffinity:
            preferredDuringSchedulingIgnoredDuringExecution:
            - weight: 100
              podAffinityTerm:
                labelSelector:
                  matchExpressions:
                  - key: app
                    operator: In
                    values:
                    - my-spring-app
                topologyKey: kubernetes.io/hostname
```

```yaml
# overlays/production/patch-replicas.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
spec:
  replicas: 5  # 5 replicas en production pour HA
```

```yaml
# overlays/production/patch-resources.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-spring-app
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

```yaml
# overlays/production/ingress-tls.yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: my-spring-app
  annotations:
    cert-manager.io/cluster-issuer: letsencrypt-prod
    nginx.ingress.kubernetes.io/ssl-redirect: "true"
spec:
  ingressClassName: nginx
  tls:
  - hosts:
    - api.company.com
    secretName: my-spring-app-tls
  rules:
  - host: api.company.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: my-spring-app
            port:
              number: 80
```

---

**Applications ArgoCD par environnement :**

```yaml
# argocd/dev-app.yaml
apiVersion: argoproj.io/v1alpha1
kind: Application
metadata:
  name: my-spring-app-dev
  namespace: argocd
spec:
  project: default
  source:
    repoURL: https://gitlab.com/company/k8s-manifests
    targetRevision: main
    path: apps/my-spring-app/overlays/dev
  destination:
    server: https://kubernetes.default.svc
    namespace: dev
  syncPolicy:
    automated:
      prune: true
      selfHeal: true
    syncOptions:
      - CreateNamespace=true
```

```yaml
# argocd/staging-app.yaml
apiVersion: argoproj.io/v1alpha1
kind: Application
metadata:
  name: my-spring-app-staging
  namespace: argocd
spec:
  project: default
  source:
    repoURL: https://gitlab.com/company/k8s-manifests
    targetRevision: main
    path: apps/my-spring-app/overlays/staging
  destination:
    server: https://kubernetes.default.svc
    namespace: staging
  syncPolicy:
    automated:
      prune: true
      selfHeal: true
```

```yaml
# argocd/production-app.yaml
apiVersion: argoproj.io/v1alpha1
kind: Application
metadata:
  name: my-spring-app-prod
  namespace: argocd
spec:
  project: default
  source:
    repoURL: https://gitlab.com/company/k8s-manifests
    targetRevision: main
    path: apps/my-spring-app/overlays/production
  destination:
    server: https://kubernetes.default.svc
    namespace: production
  syncPolicy:
    automated:
      prune: false      # Pas de suppression auto en prod
      selfHeal: true
    syncOptions:
      - CreateNamespace=true
  # Sync manual required pour prod
  ignoreDifferences:
  - group: apps
    kind: Deployment
    jsonPointers:
    - /spec/replicas  # Ignore HPA changes
```

---

**Pipeline CI/CD avec promotion entre environnements :**

```yaml
# .gitlab-ci.yml
stages:
  - build
  - test
  - package
  - deploy-dev
  - deploy-staging
  - deploy-production

# ... (stages build/test/package) ...

deploy-dev:
  stage: deploy-dev
  script:
    - cd k8s-manifests
    - cd apps/my-spring-app/overlays/dev
    - kustomize edit set image registry.company.com/my-spring-app:$CI_COMMIT_SHORT_SHA
    - git commit -am "Deploy $CI_COMMIT_SHORT_SHA to dev"
    - git push
  only:
    - develop

deploy-staging:
  stage: deploy-staging
  script:
    - cd k8s-manifests
    - cd apps/my-spring-app/overlays/staging
    - kustomize edit set image registry.company.com/my-spring-app:$CI_COMMIT_SHORT_SHA
    - git commit -am "Deploy $CI_COMMIT_SHORT_SHA to staging"
    - git push
  only:
    - develop
  when: manual  # Déploiement manuel vers staging

deploy-production:
  stage: deploy-production
  script:
    - cd k8s-manifests
    - cd apps/my-spring-app/overlays/production
    - kustomize edit set image registry.company.com/my-spring-app:$CI_COMMIT_TAG
    - git commit -am "Deploy $CI_COMMIT_TAG to production"
    - git push
  only:
    - tags  # Seulement sur tags Git (v1.2.3)
  when: manual  # Approbation manuelle requise
  environment:
    name: production
    url: https://api.company.com
```

---

**Comparaison des environnements :**

| Aspect | DEV | STAGING | PRODUCTION |
|--------|-----|---------|------------|
| **Replicas** | 1 | 2 | 5 |
| **CPU request** | 50m | 100m | 500m |
| **Memory request** | 128Mi | 256Mi | 512Mi |
| **Log Level** | DEBUG | INFO | WARN |
| **Auto-deploy** | ✅ Oui | ⚠️ Manuel | ⚠️ Manuel + Tag |
| **Self-heal** | ✅ Oui | ✅ Oui | ✅ Oui |
| **Auto-prune** | ✅ Oui | ✅ Oui | ❌ Non |
| **TLS/HTTPS** | ❌ Non | ✅ Oui | ✅ Oui |
| **HPA** | ❌ Non | ⚠️ Optionnel | ✅ Oui |
| **PDB** | ❌ Non | ❌ Non | ✅ Oui |
| **Anti-affinity** | ❌ Non | ❌ Non | ✅ Oui |

---

**Avantages de cette approche :**

1. ✅ **DRY** : Configuration commune dans base/
2. ✅ **Overrides clairs** : Différences explicites par environnement
3. ✅ **GitOps** : Tout versionné dans Git
4. ✅ **Promotion contrôlée** : dev → staging → prod
5. ✅ **Rollback facile** : git revert
6. ✅ **Audit trail** : Git history complet

---

### Q10 : Quelle est votre expérience avec ArgoCD ? Avez-vous rencontré des problèmes ?

**Réponse :**

J'ai utilisé ArgoCD pendant [X mois/années] pour déployer [nombre] applications en production.

**Mon expérience positive :**

1. **UI intuitive** : Visualisation claire de l'état des applications
2. **GitOps natif** : Sync automatique depuis Git
3. **Multi-cluster** : Gestion de plusieurs clusters depuis une seule instance
4. **RBAC intégré** : Contrôle d'accès granulaire
5. **Sync waves** : Contrôle de l'ordre de déploiement

**Problèmes rencontrés et solutions :**

---

**Problème 1 : OutOfSync à cause de Helm hooks**

**Situation :**
ArgoCD affiche continuellement "OutOfSync" sur une application Helm qui utilise des hooks (pre-install, post-install).

**Solution :**
```yaml
apiVersion: argoproj.io/v1alpha1
kind: Application
spec:
  ignoreDifferences:
  - group: batch
    kind: Job
    jsonPointers:
    - /status
  # Ou ignorer complètement les hooks
  source:
    helm:
      skipCrds: false
      # Ignorer les resources créées par hooks
```

Ou ajouter annotation sur les Jobs Helm :
```yaml
metadata:
  annotations:
    argocd.argoproj.io/hook: Skip
```

---

**Problème 2 : Sync lent sur gros repository**

**Situation :**
Repository avec 100+ applications, sync prend 5-10 minutes.

**Solution :**
```yaml
# Application of Applications pattern
apiVersion: argoproj.io/v1alpha1
kind: Application
metadata:
  name: app-of-apps
spec:
  source:
    path: argocd/applications
    directory:
      recurse: true
      jsonnet: {}
  # Activer parallel sync
  syncPolicy:
    syncOptions:
    - CreateNamespace=true
    - PruneLast=true
```

Configuration ArgoCD pour parallélisme :
```yaml
# argocd-cm ConfigMap
data:
  application.sync.parallelism: "10"  # Sync 10 apps en parallèle
```

---

**Problème 3 : OutOfSync à cause de HPA changeant replicas**

**Situation :**
HPA modifie `spec.replicas`, ArgoCD détecte un drift et veut resync.

**Solution :**
```yaml
apiVersion: argoproj.io/v1alpha1
kind: Application
spec:
  ignoreDifferences:
  - group: apps
    kind: Deployment
    jsonPointers:
    - /spec/replicas
```

Ou utiliser l'annotation :
```yaml
# Dans le Deployment
metadata:
  annotations:
    argocd.argoproj.io/compare-options: IgnoreExtraneous
```

---

**Problème 4 : Secrets non synchronisés**

**Situation :**
ArgoCD ne détecte pas les changements dans les Secrets (base64 change mais contenu identique).

**Solution :**
Utiliser External Secrets Operator ou Sealed Secrets :
```yaml
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: my-secret
  annotations:
    argocd.argoproj.io/sync-wave: "-1"  # Sync avant les apps
spec:
  refreshInterval: 1h
  secretStoreRef:
    name: aws-secrets-manager
  target:
    name: my-secret
  data:
  - secretKey: password
    remoteRef:
      key: prod/db/password
```

---

**Problème 5 : Deployment stuck en "Progressing"**

**Situation :**
ArgoCD affiche "Progressing" indéfiniment, mais le Deployment est OK.

**Cause :**
Readiness probe échoue ou timeout trop court.

**Solution :**
```yaml
# Ajuster health check timeout dans Application
apiVersion: argoproj.io/v1alpha1
kind: Application
spec:
  syncPolicy:
    syncOptions:
    - RespectIgnoreDifferences=true
  # Custom health check
  info:
  - name: 'Health Check Timeout'
    value: '5m'
```

Ou configurer custom health check :
```yaml
# argocd-cm ConfigMap
data:
  resource.customizations.health.apps_Deployment: |
    hs = {}
    if obj.status ~= nil then
      if obj.status.updatedReplicas == obj.spec.replicas then
        hs.status = "Healthy"
        hs.message = "All replicas updated"
        return hs
      end
    end
    hs.status = "Progressing"
    hs.message = "Waiting for rollout"
    return hs
```

---

**Problème 6 : Trop de sync (flapping)**

**Situation :**
ArgoCD sync toutes les 3 minutes même sans changement.

**Solution :**
Augmenter interval de réconciliation :
```yaml
# argocd-cm ConfigMap
data:
  timeout.reconciliation: 300s  # 5 minutes au lieu de 3
```

Et utiliser webhooks Git pour sync immédiat sur push :
```bash
# Dans GitHub/GitLab
Webhook URL: https://argocd.company.com/api/webhook
Secret: <argocd-webhook-secret>
Events: Push events
```

---

**Best practices ArgoCD que j'applique :**

1. **App of Apps pattern** : Centraliser la déclaration des applications
2. **Sync waves** : Ordonner les déploiements (CRDs → Operators → Apps)
3. **Project isolation** : Séparer dev/staging/prod par Projects
4. **RBAC strict** : Limiter les accès par équipe
5. **Notifications** : Alertes Slack sur échecs de sync
6. **Health checks custom** : Pour StatefulSets, Jobs, etc.

---

## 📋 Section 4 : Container Images Best Practices

### Q11 : Comment optimisez-vous vos images Docker pour une application Spring Boot ?

**Réponse :**

J'utilise plusieurs techniques pour réduire la taille et améliorer la sécurité :

**1. Multi-stage build avec layered JARs**

```dockerfile
# Stage 1: Build avec Maven
FROM maven:3.8-openjdk-17-slim AS builder
WORKDIR /app

# Copier pom.xml séparément (cache layer)
COPY pom.xml .
RUN mvn dependency:go-offline

# Copier source et builder
COPY src ./src
RUN mvn clean package -DskipTests

# Extraire layers Spring Boot
RUN java -Djarmode=layertools -jar target/*.jar extract

# Stage 2: Runtime optimisé
FROM eclipse-temurin:17-jre-alpine

# Créer user non-root
RUN addgroup -g 1001 appuser && \
    adduser -D -u 1001 -G appuser appuser

WORKDIR /app

# Copier layers dans l'ordre (optimisation cache)
COPY --from=builder /app/dependencies/ ./
COPY --from=builder /app/spring-boot-loader/ ./
COPY --from=builder /app/snapshot-dependencies/ ./
COPY --from=builder /app/application/ ./

RUN chown -R appuser:appuser /app
USER appuser

# JVM tuning pour containers
ENV JAVA_OPTS="-XX:+UseContainerSupport \
               -XX:MaxRAMPercentage=75.0 \
               -XX:InitialRAMPercentage=50.0 \
               -XX:+UseG1GC \
               -XX:+UseStringDeduplication \
               -XX:+OptimizeStringConcat \
               -Djava.security.egd=file:/dev/./urandom"

EXPOSE 8080

ENTRYPOINT ["sh", "-c", "java $JAVA_OPTS org.springframework.boot.loader.JarLauncher"]
```

**Résultat :**
- **Avant** : 450 MB (avec full JDK)
- **Après** : 180 MB (avec JRE Alpine + layers)

---

**2. Configuration Spring Boot Layered JAR**

```xml
<!-- pom.xml -->
<build>
    <plugins>
        <plugin>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-maven-plugin</artifactId>
            <configuration>
                <layers>
                    <enabled>true</enabled>
                </layers>
            </configuration>
        </plugin>
    </plugins>
</build>
```

```yaml
# custom-layers.xml (optionnel)
<layers xmlns="http://www.springframework.org/schema/boot/layers"
        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xsi:schemaLocation="http://www.springframework.org/schema/boot/layers
                            https://www.springframework.org/schema/boot/layers/layers-2.7.xsd">
    <application>
        <into layer="spring-boot-loader">
            <include>org/springframework/boot/loader/**</include>
        </into>
        <into layer="application"/>
    </application>
    <dependencies>
        <into layer="snapshot-dependencies">
            <include>*:*:*SNAPSHOT</include>
        </into>
        <into layer="company-dependencies">
            <include>com.company:*:*</include>
        </into>
        <into layer="dependencies"/>
    </dependencies>
    <layerOrder>
        <layer>dependencies</layer>
        <layer>spring-boot-loader</layer>
        <layer>snapshot-dependencies</layer>
        <layer>company-dependencies</layer>
        <layer>application</layer>
    </layerOrder>
</layers>
```

**Avantages du layering :**
- Dependencies changent rarement → Layer mis en cache
- Application code change souvent → Seul cette layer rebuild
- **Build time** réduit de 80% après le 1er build

---

**3. Utilisation de Alpine ou Distroless**

**Comparaison :**

```dockerfile
# Option 1: Alpine (le plus petit)
FROM eclipse-temurin:17-jre-alpine
# Taille: ~180 MB

# Option 2: Distroless (plus sécurisé)
FROM gcr.io/distroless/java17-debian11
# Taille: ~200 MB
# Pas de shell, pas de package manager → surface d'attaque minimale

# Option 3: Standard Debian (éviter)
FROM eclipse-temurin:17-jre
# Taille: ~420 MB
```

**Mon choix :**
- **Alpine** pour dev/staging (facile à débugger avec sh)
- **Distroless** pour production (sécurité maximale)

**Dockerfile Distroless :**

```dockerfile
FROM gcr.io/distroless/java17-debian11

COPY --from=builder /app/dependencies/ /app/
COPY --from=builder /app/spring-boot-loader/ /app/
COPY --from=builder /app/application/ /app/

USER nonroot:nonroot

EXPOSE 8080

ENTRYPOINT ["java", \
    "-XX:+UseContainerSupport", \
    "-XX:MaxRAMPercentage=75.0", \
    "org.springframework.boot.loader.JarLauncher"]
```

---

**4. .dockerignore pour build rapide**

```dockerignore
# .dockerignore
target/
!target/*.jar
.git
.gitignore
*.md
.mvn
.idea
.vscode
*.iml
node_modules/
.DS_Store
```

---

**5. Build cache optimization**

```dockerfile
# ✅ Bon : Copier pom.xml d'abord
COPY pom.xml .
RUN mvn dependency:go-offline
COPY src ./src
RUN mvn package

# ❌ Mauvais : Tout copier d'un coup
COPY . .
RUN mvn package
# → Refetch dependencies à chaque changement de code !
```

---

**6. Healthcheck dans Dockerfile**

```dockerfile
HEALTHCHECK --interval=30s --timeout=3s --start-period=40s --retries=3 \
  CMD curl -f http://localhost:8080/actuator/health/liveness || exit 1
```

Mais je préfère utiliser les probes Kubernetes plutôt que HEALTHCHECK Docker.

---

**Comparaison finale :**

| Technique | Avant | Après | Gain |
|-----------|-------|-------|------|
| JDK → JRE | 650 MB | 420 MB | -35% |
| JRE → Alpine | 420 MB | 180 MB | -57% |
| Layers | Build 8min | Build 2min | -75% |
| Non-root user | - | - | ✅ Sécurité |
| Multi-stage | - | - | ✅ Propre |

---

### Q12 : Comment intégrez-vous le security scanning dans votre pipeline ?

**Réponse :**

J'intègre le scanning à plusieurs niveaux du pipeline :

**1. Scan des dépendances Maven (OWASP Dependency Check)**

```yaml
# .gitlab-ci.yml
dependency-check:
  stage: test
  image: maven:3.8-openjdk-17
  script:
    - mvn org.owasp:dependency-check-maven:check
      -DfailBuildOnCVSS=7
      -DsuppressionFile=dependency-check-suppressions.xml
  artifacts:
    reports:
      dependency_scanning: target/dependency-check-report.json
    paths:
      - target/dependency-check-report.html
  allow_failure: false  # Bloquer le build si CVSS >= 7
```

**Suppression de faux positifs :**

```xml
<!-- dependency-check-suppressions.xml -->
<?xml version="1.0" encoding="UTF-8"?>
<suppressions xmlns="https://jeremylong.github.io/DependencyCheck/dependency-suppression.1.3.xsd">
    <suppress>
        <notes>False positive sur Spring Boot 2.7.x</notes>
        <cve>CVE-2023-12345</cve>
    </suppress>
</suppressions>
```

---

**2. Scan de l'image Docker (Trivy)**

```yaml
trivy-scan:
  stage: package
  image: aquasec/trivy:latest
  services:
    - docker:20.10-dind
  script:
    # Scan local de l'image
    - trivy image --severity HIGH,CRITICAL 
      --exit-code 1 
      --no-progress 
      --format json 
      --output trivy-report.json
      $CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA
    
    # Scan filesystem aussi
    - trivy fs --severity HIGH,CRITICAL .
  artifacts:
    reports:
      container_scanning: trivy-report.json
    paths:
      - trivy-report.json
  allow_failure: false
```

**Configuration Trivy :**

```yaml
# .trivyignore
# Ignorer CVEs spécifiques (avec justification)
CVE-2023-12345  # Fixed in next release, low risk
```

---

**3. Scan avec Snyk**

```yaml
snyk-scan:
  stage: test
  image: snyk/snyk:docker
  script:
    # Scan dependencies
    - snyk test --severity-threshold=high --file=pom.xml
    
    # Scan Docker image
    - snyk container test $CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA
      --severity-threshold=high
      --file=Dockerfile
  only:
    - main
    - develop
```

---

**4. Static Code Analysis (SonarQube)**

```yaml
sonarqube:
  stage: test
  image: maven:3.8-openjdk-17
  script:
    - mvn sonar:sonar
      -Dsonar.host.url=$SONAR_HOST_URL
      -Dsonar.login=$SONAR_TOKEN
      -Dsonar.qualitygate.wait=true
  only:
    - main
    - merge_requests
```

**Quality Gate configuration :**
- Code Coverage > 80%
- Bugs = 0
- Vulnerabilities = 0 (blocker/critical)
- Code Smells < 10
- Security Hotspots reviewed

---

**5. Runtime scanning (Falco dans Kubernetes)**

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: falco-rules
data:
  custom-rules.yaml: |
    - rule: Unauthorized Process
      desc: Detect unauthorized process in container
      condition: >
        spawned_process and 
        container and 
        not proc.name in (java, sh, bash)
      output: >
        Unauthorized process in container
        (user=%user.name command=%proc.cmdline container=%container.name)
      priority: WARNING
```

---

**6. Admission Controller (OPA Gatekeeper)**

```yaml
apiVersion: constraints.gatekeeper.sh/v1beta1
kind: K8sBlockPrivileged
metadata:
  name: block-privileged-containers
spec:
  match:
    kinds:
    - apiGroups: [""]
      kinds: ["Pod"]
  parameters:
    exemptImages:
    - "gcr.io/distroless/*"
```

---

**Pipeline complet avec gates :**

```yaml
stages:
  - build
  - test
  - security-scan      # ← GATE 1
  - package
  - image-scan         # ← GATE 2
  - deploy-dev
  - integration-test
  - deploy-staging
  - penetration-test   # ← GATE 3
  - deploy-production

security-scan:
  stage: security-scan
  script:
    - mvn dependency-check:check
    - mvn sonar:sonar
  allow_failure: false  # Bloquer si vuln critical

image-scan:
  stage: image-scan
  script:
    - trivy image --severity CRITICAL --exit-code 1 $IMAGE
    - snyk container test $IMAGE
  allow_failure: false  # Bloquer si vuln critical dans image

penetration-test:
  stage: penetration-test
  script:
    - zap-baseline.py -t https://staging.company.com
  only:
    - main
  when: manual  # Manuel avant prod
```

---

**Notifications sur vulnerabilités :**

```yaml
slack-security-alert:
  stage: .post
  script:
    - |
      curl -X POST $SLACK_WEBHOOK -d "{
        \"text\": \"⚠️ Security scan found HIGH/CRITICAL vulnerabilities in ${CI_PROJECT_NAME}\",
        \"attachments\": [{
          \"color\": \"danger\",
          \"fields\": [{
            \"title\": \"Pipeline\",
            \"value\": \"${CI_PIPELINE_URL}\",
            \"short\": false
          }]
        }]
      }"
  when: on_failure
  only:
    - main
```

---

**Dashboard de sécurité (exemple) :**

```
┌─────────────────────────────────────────┐
│ Security Scan Results - Sprint 24       │
├─────────────────────────────────────────┤
│ OWASP Dependency Check:                 │
│   ✅ 0 Critical                          │
│   ⚠️  2 High (reviewed, accepted)        │
│   📝 12 Medium                           │
│                                         │
│ Trivy Image Scan:                       │
│   ✅ 0 Critical                          │
│   ✅ 0 High                              │
│   📝 5 Medium                            │
│                                         │
│ SonarQube:                              │
│   ✅ Quality Gate: PASSED                │
│   📊 Coverage: 87%                       │
│   🐛 Bugs: 0                             │
│   🔒 Vulnerabilities: 0                  │
│                                         │
│ Snyk:                                   │
│   ✅ No issues found                     │
└─────────────────────────────────────────┘
```

---

**Best practices que j'applique :**

1. **Scan tôt** : Dès le build, pas seulement avant prod
2. **Fail fast** : Bloquer le pipeline sur Critical/High
3. **Suppressions documentées** : Justifier chaque exception
4. **Scan régulier** : Même sans changement de code (nouvelles CVE)
5. **Multi-outils** : Trivy + Snyk + OWASP (chacun trouve différentes choses)
6. **Monitoring runtime** : Falco pour détecter comportements suspects

---

### Q13 : Expliquez votre stratégie de tagging des images Docker

**Réponse :**

J'utilise une stratégie de **multi-tagging** pour traçabilité et flexibilité :

**1. Tag principal : Git commit SHA**

```bash
# CI/CD
IMAGE_TAG=${CI_COMMIT_SHORT_SHA}  # Exemple: a3f7b21
docker build -t registry.company.com/my-app:${IMAGE_TAG} .
docker push registry.company.com/my-app:${IMAGE_TAG}
```

**Avantages :**
- ✅ Immuable (SHA unique)
- ✅ Traçabilité parfaite (lien vers commit Git)
- ✅ Rollback précis

---

**2. Tags sémantiques (semantic versioning)**

Pour production, j'utilise aussi des tags sémantiques :

```bash
# Pour un tag Git v1.2.3
docker tag my-app:a3f7b21 my-app:v1.2.3
docker tag my-app:a3f7b21 my-app:v1.2
docker tag my-app:a3f7b21 my-app:v1
docker tag my-app:a3f7b21 my-app:latest
docker tag my-app:a3f7b21 my-app:stable
```

**Convention :**
- `v1.2.3` : Version exacte (MAJOR.MINOR.PATCH)
- `v1.2` : Latest patch de cette minor version
- `v1` : Latest minor de cette major version
- `latest` : Latest stable release
- `stable` : Alias pour production

---

**3. Tags par environnement**

```bash
# DEV (branch develop)
docker tag my-app:a3f7b21 my-app:dev-latest
docker tag my-app:a3f7b21 my-app:dev-${DATE}  # dev-20241118

# STAGING (branch staging ou tags RC)
docker tag my-app:a3f7b21 my-app:staging-v1.2.3-rc1

# PRODUCTION (tags uniquement)
docker tag my-app:a3f7b21 my-app:prod-v1.2.3
```

---

**4. Pipeline complet de tagging**

```yaml
# .gitlab-ci.yml
variables:
  IMAGE_NAME: registry.company.com/my-app
  COMMIT_TAG: ${CI_COMMIT_SHORT_SHA}

docker-build:
  stage: package
  script:
    # Build avec commit SHA
    - docker build -t ${IMAGE_NAME}:${COMMIT_TAG} .
    
    # Tags additionnels selon branche/tag
    - |
      if [ "$CI_COMMIT_BRANCH" == "develop" ]; then
        docker tag ${IMAGE_NAME}:${COMMIT_TAG} ${IMAGE_NAME}:dev-latest
        docker tag ${IMAGE_NAME}:${COMMIT_TAG} ${IMAGE_NAME}:dev-$(date +%Y%m%d)
      fi
    
    - |
      if [ "$CI_COMMIT_BRANCH" == "main" ]; then
        docker tag ${IMAGE_NAME}:${COMMIT_TAG} ${IMAGE_NAME}:latest
      fi
    
    - |
      if [[ "$CI_COMMIT_TAG" =~ ^v[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
        # Tag sémantique (v1.2.3)
        VERSION=${CI_COMMIT_TAG#v}  # Enlever 'v' prefix
        MAJOR=$(echo $VERSION | cut -d. -f1)
        MINOR=$(echo $VERSION | cut -d. -f1-2)
        
        docker tag ${IMAGE_NAME}:${COMMIT_TAG} ${IMAGE_NAME}:v${VERSION}
        docker tag ${IMAGE_NAME}:${COMMIT_TAG} ${IMAGE_NAME}:v${MINOR}
        docker tag ${IMAGE_NAME}:${COMMIT_TAG} ${IMAGE_NAME}:v${MAJOR}
        docker tag ${IMAGE_NAME}:${COMMIT_TAG} ${IMAGE_NAME}:stable
      fi
    
    # Push tous les tags
    - docker push ${IMAGE_NAME} --all-tags
```

---

**5. Conventions de nommage**

```bash
# ✅ BON
registry.company.com/my-app:a3f7b21           # Git SHA
registry.company.com/my-app:v1.2.3            # Semantic version
registry.company.com/my-app:v1.2.3-hotfix-1   # Hotfix
registry.company.com/my-app:dev-20241118      # Dev avec date

# ❌ MAUVAIS (à éviter)
my-app:latest              # Trop vague, quel environment?
my-app:prod                # Quelle version?
my-app:fix                 # Quel fix?
my-app:john                # Nom personnel (!)
my-app:v1                  # Trop large pour prod
```

---

**6. Kubernetes deployment avec tags**

```yaml
# DEV : Tag mutable (auto-update)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app-dev
  namespace: dev
spec:
  template:
    spec:
      containers:
      - name: app
        image: registry.company.com/my-app:dev-latest
        imagePullPolicy: Always  # Toujours pull latest

---

# PRODUCTION : Tag immutable (pas d'auto-update)
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app-prod
  namespace: production
spec:
  template:
    spec:
      containers:
      - name: app
        image: registry.company.com/my-app:a3f7b21  # SHA immutable
        imagePullPolicy: IfNotPresent  # Utiliser cache si disponible
```

---

**7. Image promotion pipeline**

```bash
#!/bin/bash
# promote-image.sh

SOURCE_TAG=$1  # Ex: a3f7b21
TARGET_ENV=$2  # Ex: production

IMAGE_NAME="registry.company.com/my-app"

# Promote image vers nouvel environnement
docker pull ${IMAGE_NAME}:${SOURCE_TAG}
docker tag ${IMAGE_NAME}:${SOURCE_TAG} ${IMAGE_NAME}:${TARGET_ENV}-latest
docker push ${IMAGE_NAME}:${TARGET_ENV}-latest

echo "Image ${SOURCE_TAG} promoted to ${TARGET_ENV}"
```

---

**8. Nettoyage des anciennes images**

```yaml
cleanup-old-images:
  stage: cleanup
  script:
    # Garder seulement dernières 10 images par environnement
    - |
      gcloud container images list-tags \
        registry.company.com/my-app \
        --filter='-tags:*' \
        --format='get(digest)' \
        --limit=unlimited | tail -n +11 | \
        xargs -I {} gcloud container images delete \
        registry.company.com/my-app@{} --quiet
  only:
    - schedules  # Cron quotidien
```

---

**Résumé de ma stratégie :**

| Environnement | Tag principal | Tags secondaires | ImagePullPolicy |
|--------------|---------------|------------------|-----------------|
| **DEV** | `dev-latest` | `dev-YYYYMMDD`, SHA | `Always` |
| **STAGING** | `staging-v1.2.3-rc1` | SHA | `IfNotPresent` |
| **PRODUCTION** | `a3f7b21` (SHA) | `v1.2.3`, `stable` | `IfNotPresent` |

**Avantages :**
- ✅ Traçabilité complète
- ✅ Rollback facile (revenir au SHA précédent)
- ✅ Immutabilité en prod (SHA)
- ✅ Flexibilité en dev (latest)

---

## 📋 Section 5 : Questions Pratiques & Debugging

### Q14 : Comment debuggez-vous un déploiement qui échoue dans Kubernetes ?

**Réponse :**

Je suis un processus systématique en 5 étapes :

**1. Vérifier le status du Deployment**

```bash
# Status général
kubectl get deployment my-app -n production

# Output exemple:
# NAME     READY   UP-TO-DATE   AVAILABLE   AGE
# my-app   2/3     3            2           5m

# Problème : 2/3 ready → un pod en échec
```

---

**2. Examiner les Pods**

```bash
# Lister les pods
kubectl get pods -n production -l app=my-app

# Output exemple:
# NAME                      READY   STATUS             RESTARTS   AGE
# my-app-7d9c8b-abc12       1/1     Running            0          5m
# my-app-7d9c8b-def34       1/1     Running            0          5m
# my-app-7d9c8b-ghi56       0/1     CrashLoopBackOff   5          5m
#                                   ^^^^^^^^^^^^^^^^
#                                   Problème ici !

# Détails du pod en échec
kubectl describe pod my-app-7d9c8b-ghi56 -n production
```

---

**3. Analyser les Events**

```bash
# Dans kubectl describe, section Events:
Events:
  Type     Reason     Age                  From               Message
  ----     ------     ----                 ----               -------
  Warning  Failed     2m (x5 over 3m)      kubelet            Error: ImagePullBackOff
  Normal   Pulling    1m (x4 over 3m)      kubelet            Pulling image "my-app:v1.2.3"
  Warning  Failed     1m (x4 over 3m)      kubelet            Failed to pull image: authentication required

# Problème identifié : Authentification registry !
```

**Solutions selon le problème :**

---

**Problème A : ImagePullBackOff (authentification)**

```bash
# Créer/vérifier secret
kubectl get secret regcred -n production

# Si absent, créer:
kubectl create secret docker-registry regcred \
  --docker-server=registry.company.com \
  --docker-username=${REGISTRY_USER} \
  --docker-password=${REGISTRY_PASSWORD} \
  -n production

# Vérifier que le Deployment l'utilise
kubectl get deployment my-app -o yaml | grep imagePullSecrets
```

---

**Problème B : CrashLoopBackOff (application crash)**

```bash
# Voir logs du container qui crash
kubectl logs my-app-7d9c8b-ghi56 -n production

# Logs précédents (si restarted)
kubectl logs my-app-7d9c8b-ghi56 -n production --previous

# Output exemple:
# Error: Could not connect to database at postgres:5432
# Caused by: java.net.UnknownHostException: postgres

# Problème: Service postgres n'existe pas ou mauvais nom
```

**Solution :**

```bash
# Vérifier le Service
kubectl get svc -n production | grep postgres

# Vérifier ConfigMap
kubectl get configmap my-app-config -n production -o yaml | grep DATABASE_URL

# Corriger:
kubectl edit configmap my-app-config -n production
# DATABASE_URL: jdbc:postgresql://postgres-service:5432/mydb
#                                    ^^^^^^^^ Ajouter -service

# Restart pods
kubectl rollout restart deployment my-app -n production
```

---

**Problème C : Pending (ressources insuffisantes)**

```bash
kubectl describe pod my-app-7d9c8b-ghi56 -n production

# Events:
# Warning  FailedScheduling  2m  default-scheduler  
# 0/3 nodes are available: 3 Insufficient memory.

# Problème: Pas assez de mémoire sur les nodes
```

**Solution :**

```bash
# Vérifier ressources du pod
kubectl get pod my-app-7d9c8b-ghi56 -o yaml | grep -A 10 resources:

# Si requests trop élevés, ajuster:
kubectl edit deployment my-app
# resources:
#   requests:
#     memory: "256Mi"  # Au lieu de 4Gi
#     cpu: "100m"

# Ou scaler le cluster (ajouter nodes)
```

---

**Problème D : Readiness probe failing**

```bash
kubectl describe pod my-app-7d9c8b-ghi56 -n production

# Events:
# Warning  Unhealthy  1m (x10 over 3m)  kubelet  
# Readiness probe failed: HTTP probe failed with statuscode: 503

# Application démarre mais health check échoue
```

**Debug :**

```bash
# Accéder au pod directement
kubectl exec -it my-app-7d9c8b-ghi56 -n production -- sh

# Tester health endpoint manuellement
curl http://localhost:8080/actuator/health
# {"status":"DOWN","components":{"db":{"status":"DOWN"}}}

# Problème: Base de données inaccessible !
```

**Solution :**

```bash
# Vérifier connectivité DB depuis pod
kubectl exec -it my-app-7d9c8b-ghi56 -n production -- sh
$ nc -zv postgres-service 5432
# Connection refused

# Vérifier que le Service DB existe
kubectl get svc postgres-service -n production

# Vérifier endpoints
kubectl get endpoints postgres-service -n production
```

---

**4. Logs détaillés**

```bash
# Logs en temps réel
kubectl logs -f my-app-7d9c8b-ghi56 -n production

# Logs avec timestamp
kubectl logs my-app-7d9c8b-ghi56 -n production --timestamps

# Logs de tous les pods du deployment
kubectl logs -l app=my-app -n production --all-containers=true

# Logs depuis les 1h dernières
kubectl logs my-app-7d9c8b-ghi56 -n production --since=1h

# Export logs pour analyse
kubectl logs my-app-7d9c8b-ghi56 -n production > /tmp/pod-logs.txt
```

---

**5. Debugging avancé**

**A. Exec dans le container (si Alpine/Debian) :**

```bash
kubectl exec -it my-app-7d9c8b-ghi56 -n production -- sh

# Dans le pod:
ps aux                    # Vérifier process
env | grep DATABASE       # Vérifier env vars
cat /app/config.yaml      # Vérifier config montée
wget -O- localhost:8080   # Tester app directement
```

**B. Debug container (si Distroless sans shell) :**

```bash
# Kubernetes 1.23+ : Ephemeral debug container
kubectl debug my-app-7d9c8b-ghi56 -n production \
  --image=busybox \
  --target=app \
  -it -- sh

# Depuis debug container, accès au filesystem du target
ls /proc/1/root/app/
```

**C. Port-forward pour tester localement :**

```bash
kubectl port-forward my-app-7d9c8b-ghi56 8080:8080 -n production

# Dans autre terminal:
curl http://localhost:8080/actuator/health
```

**D. Copier fichiers pour analyse :**

```bash
# Copier logs application depuis pod
kubectl cp production/my-app-7d9c8b-ghi56:/app/logs/app.log ./app.log

# Copier config
kubectl cp production/my-app-7d9c8b-ghi56:/app/config.yaml ./config.yaml
```

---

**Checklist complète de debugging :**

```bash
# 1. Status
kubectl get deployment,rs,pod -n production -l app=my-app

# 2. Events
kubectl get events -n production --sort-by='.lastTimestamp' | grep my-app

# 3. Describe
kubectl describe pod <pod-name> -n production

# 4. Logs
kubectl logs <pod-name> -n production --previous

# 5. Config
kubectl get configmap my-app-config -o yaml
kubectl get secret my-app-secret -o yaml

# 6. Network
kubectl get svc,endpoints -n production | grep my-app

# 7. Resources
kubectl top pod -n production -l app=my-app
kubectl describe node <node-name>

# 8. RBAC (si permission denied)
kubectl auth can-i get pods --as=system:serviceaccount:production:my-app-sa
```

---

**Erreurs fréquentes et solutions rapides :**

| Erreur | Cause | Solution rapide |
|--------|-------|----------------|
| `ImagePullBackOff` | Image inexistante ou auth | Vérifier tag + imagePullSecrets |
| `CrashLoopBackOff` | App crash au démarrage | Logs: `kubectl logs --previous` |
| `Pending` | Pas de ressources | Réduire requests ou scaler cluster |
| `CreateContainerConfigError` | ConfigMap/Secret manquant | Vérifier `kubectl get cm,secret` |
| `Readiness probe failed` | App démarre lentement | Augmenter `initialDelaySeconds` |
| `OOMKilled` | Out of memory | Augmenter `memory limits` |

---

### Q15 : Comment géreriez-vous un hotfix urgent en production ?

**Réponse :**

Voici mon process pour un hotfix urgent :

**Scénario : Bug critique en production v1.2.3, nécessite hotfix immédiat**

---

**1. Créer branche hotfix (5 min)**

```bash
# Depuis tag production
git checkout v1.2.3
git checkout -b hotfix/critical-bug-fix

# Fix le bug
vim src/main/java/com/company/BuggyService.java
git add .
git commit -m "fix: Critical bug in payment processing"

# Tests locaux
mvn test

# Push
git push origin hotfix/critical-bug-fix
```

---

**2. Pipeline CI/CD accéléré (10 min)**

```yaml
# .gitlab-ci.yml - Mode hotfix
hotfix-build:
  stage: build
  script:
    - mvn clean package -DskipTests  # Skip tests pour vitesse
  only:
    - /^hotfix\/.*/
  when: manual  # Contrôle manuel

hotfix-deploy:
  stage: deploy
  script:
    - IMAGE_TAG="hotfix-${CI_COMMIT_SHORT_SHA}"
    - docker build -t registry.company.com/my-app:${IMAGE_TAG} .
    - docker push registry.company.com/my-app:${IMAGE_TAG}
    
    # Update manifest direct (pas de PR)