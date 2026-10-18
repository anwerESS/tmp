## [**..**](./00_index.md)


# Chapitre 9 : Helm & Package Management

## 9.1. Helm Basics (Charts, Releases)

### Qu'est-ce que Helm ?

**Helm** est le **package manager** pour Kubernetes, comparable à apt/yum pour Linux ou npm pour Node.js.

**Problème sans Helm :**
- 10+ fichiers YAML à gérer par application
- Duplication de configuration entre environnements
- Difficile de versionner et déployer
- Pas de rollback simple

**Solution avec Helm :**
- Package complet = **Chart**
- Déploiement = **Release**
- Templating pour réutilisation
- Versioning et rollback intégrés

---

### Architecture Helm

```
Helm Chart (package)
├── Chart.yaml          # Métadonnées
├── values.yaml         # Configuration par défaut
├── templates/          # Templates YAML
│   ├── deployment.yaml
│   ├── service.yaml
│   ├── ingress.yaml
│   └── configmap.yaml
└── charts/             # Dependencies
```

---

### Installation Helm

```bash
# macOS
brew install helm

# Linux
curl https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3 | bash

# Vérifier installation
helm version
# version.BuildInfo{Version:"v3.14.0"}

# Ajouter repository officiel
helm repo add stable https://charts.helm.sh/stable
helm repo add bitnami https://charts.bitnami.com/bitnami

# Update repositories
helm repo update
```

---

### Concepts clés

**Chart** : Package Helm (template + config)

**Release** : Instance d'un Chart déployé

**Repository** : Collection de Charts

**Values** : Configuration pour personnaliser un Chart

---

### Utilisation basique

**Rechercher un Chart :**

```bash
# Rechercher dans tous les repos
helm search hub postgres
helm search hub nginx

# Rechercher dans repos locaux
helm search repo postgres
```

---

**Installer un Chart :**

```bash
# Installation PostgreSQL
helm install my-postgres bitnami/postgresql \
  --namespace production \
  --create-namespace

# Output:
# NAME: my-postgres
# NAMESPACE: production
# STATUS: deployed
# REVISION: 1
```

---

**Lister les releases :**

```bash
helm list -A

# Output:
# NAME          NAMESPACE    REVISION  STATUS    CHART
# my-postgres   production   1         deployed  postgresql-12.5.8
```

---

**Mettre à jour une release :**

```bash
helm upgrade my-postgres bitnami/postgresql \
  --set postgresql.password=newpassword
```

---

**Rollback :**

```bash
# Voir historique
helm history my-postgres

# Rollback à la version précédente
helm rollback my-postgres 1
```

---

**Désinstaller :**

```bash
helm uninstall my-postgres -n production
```

---

## 9.2. values.yaml & Templating

### Structure d'un Chart

**Créer un Chart :**

```bash
helm create my-spring-app

# Génère :
# my-spring-app/
# ├── Chart.yaml
# ├── values.yaml
# ├── templates/
# │   ├── deployment.yaml
# │   ├── service.yaml
# │   ├── ingress.yaml
# │   ├── _helpers.tpl
# │   └── NOTES.txt
# └── .helmignore
```

---

### Chart.yaml (Métadonnées)

```yaml
apiVersion: v2
name: my-spring-app
description: A Helm chart for Spring Boot application
type: application
version: 1.0.0         # Version du Chart
appVersion: "2.1.3"    # Version de l'application

keywords:
  - spring-boot
  - java
  - api

maintainers:
  - name: Backend Team
    email: backend@company.com

dependencies:
  - name: postgresql
    version: "12.5.8"
    repository: https://charts.bitnami.com/bitnami
    condition: postgresql.enabled
  
  - name: redis
    version: "17.11.3"
    repository: https://charts.bitnami.com/bitnami
    condition: redis.enabled
```

---

### values.yaml (Configuration par défaut)

```yaml
# values.yaml
replicaCount: 3

image:
  repository: registry.company.com/my-spring-app
  tag: "2.1.3"
  pullPolicy: IfNotPresent

imagePullSecrets:
  - name: regcred

nameOverride: ""
fullnameOverride: ""

serviceAccount:
  create: true
  annotations: {}
  name: ""

podAnnotations:
  prometheus.io/scrape: "true"
  prometheus.io/port: "9090"
  prometheus.io/path: "/actuator/prometheus"

podSecurityContext:
  runAsNonRoot: true
  runAsUser: 1001
  fsGroup: 1001

securityContext:
  allowPrivilegeEscalation: false
  capabilities:
    drop:
    - ALL
  readOnlyRootFilesystem: true

service:
  type: ClusterIP
  port: 80
  targetPort: 8080
  managementPort: 9090

ingress:
  enabled: true
  className: nginx
  annotations:
    cert-manager.io/cluster-issuer: letsencrypt-prod
  hosts:
    - host: my-app.company.com
      paths:
        - path: /
          pathType: Prefix
  tls:
    - secretName: my-app-tls
      hosts:
        - my-app.company.com

resources:
  limits:
    cpu: 1000m
    memory: 1Gi
  requests:
    cpu: 500m
    memory: 512Mi

autoscaling:
  enabled: true
  minReplicas: 3
  maxReplicas: 10
  targetCPUUtilizationPercentage: 70
  targetMemoryUtilizationPercentage: 80

livenessProbe:
  httpGet:
    path: /actuator/health/liveness
    port: management
  initialDelaySeconds: 60
  periodSeconds: 10
  timeoutSeconds: 5
  failureThreshold: 3

readinessProbe:
  httpGet:
    path: /actuator/health/readiness
    port: management
  initialDelaySeconds: 30
  periodSeconds: 5
  timeoutSeconds: 3
  failureThreshold: 3

nodeSelector: {}

tolerations: []

affinity:
  podAntiAffinity:
    preferredDuringSchedulingIgnoredDuringExecution:
    - weight: 100
      podAffinityTerm:
        labelSelector:
          matchLabels:
            app.kubernetes.io/name: my-spring-app
        topologyKey: kubernetes.io/hostname

env:
  - name: SPRING_PROFILES_ACTIVE
    value: "prod"
  - name: JAVA_OPTS
    value: "-XX:+UseContainerSupport -XX:MaxRAMPercentage=75.0 -XX:+UseG1GC"

envFrom:
  - configMapRef:
      name: my-spring-app-config
  - secretRef:
      name: my-spring-app-secret

configMap:
  enabled: true
  data:
    application.yaml: |
      spring:
        datasource:
          url: jdbc:postgresql://{{ .Release.Name }}-postgresql:5432/mydb
          hikari:
            maximum-pool-size: 20
        redis:
          host: {{ .Release.Name }}-redis-master
          port: 6379
      
      logging:
        level:
          root: INFO
          com.company: DEBUG

secret:
  enabled: true
  data:
    database-username: "YXBwX3VzZXI="  # base64 encoded
    database-password: "c2VjdXJlcGFzcw=="

pdb:
  enabled: true
  minAvailable: 2

# Dependencies
postgresql:
  enabled: true
  auth:
    username: app_user
    password: securepass
    database: mydb
  primary:
    persistence:
      enabled: true
      size: 50Gi

redis:
  enabled: true
  auth:
    enabled: true
    password: redispass
  master:
    persistence:
      enabled: true
      size: 10Gi
```

---

### Templating avec Go Templates

**templates/deployment.yaml :**

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: {{ include "my-spring-app.fullname" . }}
  labels:
    {{- include "my-spring-app.labels" . | nindent 4 }}
spec:
  {{- if not .Values.autoscaling.enabled }}
  replicas: {{ .Values.replicaCount }}
  {{- end }}
  selector:
    matchLabels:
      {{- include "my-spring-app.selectorLabels" . | nindent 6 }}
  template:
    metadata:
      annotations:
        {{- toYaml .Values.podAnnotations | nindent 8 }}
      labels:
        {{- include "my-spring-app.selectorLabels" . | nindent 8 }}
    spec:
      {{- with .Values.imagePullSecrets }}
      imagePullSecrets:
        {{- toYaml . | nindent 8 }}
      {{- end }}
      serviceAccountName: {{ include "my-spring-app.serviceAccountName" . }}
      securityContext:
        {{- toYaml .Values.podSecurityContext | nindent 8 }}
      containers:
      - name: {{ .Chart.Name }}
        image: "{{ .Values.image.repository }}:{{ .Values.image.tag | default .Chart.AppVersion }}"
        imagePullPolicy: {{ .Values.image.pullPolicy }}
        ports:
        - name: http
          containerPort: {{ .Values.service.targetPort }}
          protocol: TCP
        - name: management
          containerPort: {{ .Values.service.managementPort }}
          protocol: TCP
        
        {{- if .Values.livenessProbe }}
        livenessProbe:
          {{- toYaml .Values.livenessProbe | nindent 10 }}
        {{- end }}
        
        {{- if .Values.readinessProbe }}
        readinessProbe:
          {{- toYaml .Values.readinessProbe | nindent 10 }}
        {{- end }}
        
        resources:
          {{- toYaml .Values.resources | nindent 10 }}
        
        {{- with .Values.env }}
        env:
          {{- toYaml . | nindent 10 }}
        {{- end }}
        
        {{- with .Values.envFrom }}
        envFrom:
          {{- toYaml . | nindent 10 }}
        {{- end }}
        
        securityContext:
          {{- toYaml .Values.securityContext | nindent 10 }}
      
      {{- with .Values.nodeSelector }}
      nodeSelector:
        {{- toYaml . | nindent 8 }}
      {{- end }}
      
      {{- with .Values.affinity }}
      affinity:
        {{- toYaml . | nindent 8 }}
      {{- end }}
      
      {{- with .Values.tolerations }}
      tolerations:
        {{- toYaml . | nindent 8 }}
      {{- end }}
```

---

### Fonctions de templating utiles

**Conditions :**

```yaml
{{- if .Values.ingress.enabled }}
# Créer Ingress
{{- end }}

{{- if eq .Values.environment "production" }}
replicas: 5
{{- else }}
replicas: 2
{{- end }}
```

---

**Boucles :**

```yaml
{{- range .Values.extraEnvVars }}
- name: {{ .name }}
  value: {{ .value }}
{{- end }}
```

---

**Fonctions string :**

```yaml
# Upper case
{{ .Values.env | upper }}

# Quote
{{ .Values.password | quote }}

# Default value
{{ .Values.tag | default .Chart.AppVersion }}

# Indentation
{{- toYaml .Values.resources | nindent 10 }}
```

---

### templates/_helpers.tpl (Fonctions réutilisables)

```yaml
{{/*
Expand the name of the chart.
*/}}
{{- define "my-spring-app.name" -}}
{{- default .Chart.Name .Values.nameOverride | trunc 63 | trimSuffix "-" }}
{{- end }}

{{/*
Create a default fully qualified app name.
*/}}
{{- define "my-spring-app.fullname" -}}
{{- if .Values.fullnameOverride }}
{{- .Values.fullnameOverride | trunc 63 | trimSuffix "-" }}
{{- else }}
{{- $name := default .Chart.Name .Values.nameOverride }}
{{- if contains $name .Release.Name }}
{{- .Release.Name | trunc 63 | trimSuffix "-" }}
{{- else }}
{{- printf "%s-%s" .Release.Name $name | trunc 63 | trimSuffix "-" }}
{{- end }}
{{- end }}
{{- end }}

{{/*
Common labels
*/}}
{{- define "my-spring-app.labels" -}}
helm.sh/chart: {{ include "my-spring-app.chart" . }}
{{ include "my-spring-app.selectorLabels" . }}
{{- if .Chart.AppVersion }}
app.kubernetes.io/version: {{ .Chart.AppVersion | quote }}
{{- end }}
app.kubernetes.io/managed-by: {{ .Release.Service }}
{{- end }}

{{/*
Selector labels
*/}}
{{- define "my-spring-app.selectorLabels" -}}
app.kubernetes.io/name: {{ include "my-spring-app.name" . }}
app.kubernetes.io/instance: {{ .Release.Name }}
{{- end }}
```

---

## 9.3. Chart Repository Management

### Créer un Chart Repository

**Option 1 : GitHub Pages (simple) :**

```bash
# 1. Créer repo GitHub : helm-charts
# 2. Package le chart
helm package my-spring-app/

# Output: my-spring-app-1.0.0.tgz

# 3. Créer index
helm repo index . --url https://company.github.io/helm-charts/

# Génère: index.yaml

# 4. Commit et push vers GitHub
git add .
git commit -m "Add my-spring-app chart"
git push

# 5. Activer GitHub Pages (Settings → Pages → main branch)

# 6. Utiliser le repo
helm repo add company https://company.github.io/helm-charts/
helm repo update
helm install my-app company/my-spring-app
```

---

**Option 2 : ChartMuseum (self-hosted) :**

```bash
# Installation
helm install chartmuseum chartmuseum/chartmuseum \
  --set env.open.DISABLE_API=false \
  --set persistence.enabled=true \
  --set persistence.size=10Gi

# Upload chart
curl --data-binary "@my-spring-app-1.0.0.tgz" \
  http://chartmuseum.company.com/api/charts

# Utiliser
helm repo add company http://chartmuseum.company.com
helm install my-app company/my-spring-app
```

---

**Option 3 : Artifact Registry (cloud) :**

```bash
# AWS ECR
aws ecr create-repository --repository-name helm-charts
helm registry login <account-id>.dkr.ecr.eu-west-1.amazonaws.com
helm push my-spring-app-1.0.0.tgz oci://<account-id>.dkr.ecr.eu-west-1.amazonaws.com/helm-charts

# GCP Artifact Registry
helm registry login europe-west1-docker.pkg.dev
helm push my-spring-app-1.0.0.tgz oci://europe-west1-docker.pkg.dev/project/helm-charts
```

---

## 9.4. Helm Best Practices

### 1. Values.yaml par environnement

```
my-spring-app/
├── values.yaml              # Defaults
├── values-dev.yaml          # Dev overrides
├── values-staging.yaml      # Staging overrides
└── values-production.yaml   # Production overrides
```

**values-production.yaml :**

```yaml
replicaCount: 5

resources:
  limits:
    cpu: 2000m
    memory: 2Gi
  requests:
    cpu: 1000m
    memory: 1Gi

ingress:
  hosts:
    - host: api.company.com

autoscaling:
  minReplicas: 5
  maxReplicas: 20

postgresql:
  primary:
    persistence:
      size: 100Gi

redis:
  master:
    persistence:
      size: 20Gi
```

**Installation :**

```bash
# Dev
helm install my-app ./my-spring-app \
  -f my-spring-app/values-dev.yaml \
  -n dev

# Production
helm install my-app ./my-spring-app \
  -f my-spring-app/values-production.yaml \
  -n production
```

---

### 2. Secrets gestion

**Ne JAMAIS commiter secrets en clair dans values.yaml !**

**Option 1 : helm-secrets plugin :**

```bash
# Installation
helm plugin install https://github.com/jkroepke/helm-secrets

# Créer secrets.yaml
cat > secrets.yaml <<EOF
database:
  password: SuperSecurePassword123!
redis:
  password: RedisSecurePass456!
EOF

# Encrypt avec SOPS
sops -e secrets.yaml > secrets.enc.yaml

# Install avec secrets
helm secrets install my-app ./my-spring-app \
  -f values.yaml \
  -f secrets.enc.yaml
```

---

**Option 2 : External Secrets Operator :**

```yaml
# templates/externalsecret.yaml
{{- if .Values.externalSecrets.enabled }}
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: {{ include "my-spring-app.fullname" . }}
spec:
  refreshInterval: 1h
  secretStoreRef:
    name: {{ .Values.externalSecrets.secretStore }}
    kind: SecretStore
  target:
    name: {{ include "my-spring-app.fullname" . }}-secret
  data:
  - secretKey: database-password
    remoteRef:
      key: {{ .Values.externalSecrets.databasePasswordKey }}
{{- end }}
```

---

### 3. Hooks pour lifecycle management

**pre-install hook (migrations DB) :**

```yaml
# templates/job-migration.yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: {{ include "my-spring-app.fullname" . }}-migration
  annotations:
    "helm.sh/hook": pre-install,pre-upgrade
    "helm.sh/hook-weight": "-5"
    "helm.sh/hook-delete-policy": before-hook-creation
spec:
  template:
    spec:
      restartPolicy: Never
      containers:
      - name: flyway
        image: flyway/flyway:9.16
        command:
        - flyway
        - migrate
        env:
        - name: FLYWAY_URL
          value: "jdbc:postgresql://{{ .Release.Name }}-postgresql:5432/mydb"
        - name: FLYWAY_USER
          valueFrom:
            secretKeyRef:
              name: {{ include "my-spring-app.fullname" . }}-secret
              key: database-username
        - name: FLYWAY_PASSWORD
          valueFrom:
            secretKeyRef:
              name: {{ include "my-spring-app.fullname" . }}-secret
              key: database-password
```

**post-install hook (smoke tests) :**

```yaml
# templates/job-test.yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: {{ include "my-spring-app.fullname" . }}-test
  annotations:
    "helm.sh/hook": post-install,post-upgrade
    "helm.sh/hook-weight": "0"
    "helm.sh/hook-delete-policy": hook-succeeded
spec:
  template:
    spec:
      restartPolicy: Never
      containers:
      - name: test
        image: curlimages/curl:latest
        command:
        - sh
        - -c
        - |
          sleep 30
          curl -f http://{{ include "my-spring-app.fullname" . }}/actuator/health
```

---

### 4. Testing Charts

```bash
# Dry-run (voir les manifests générés)
helm install my-app ./my-spring-app --dry-run --debug

# Template (générer sans installer)
helm template my-app ./my-spring-app

# Lint (vérifier syntax)
helm lint my-spring-app/

# Test avec valeurs custom
helm template my-app ./my-spring-app \
  --set replicaCount=10 \
  --set image.tag=v2.0.0
```

---

### 5. Documentation

**templates/NOTES.txt (affiché après install) :**

```
🎉 {{ .Chart.Name }} has been installed!

Release name: {{ .Release.Name }}
Namespace: {{ .Release.Namespace }}

To access your application:

{{- if .Values.ingress.enabled }}
  URL: https://{{ (index .Values.ingress.hosts 0).host }}
{{- else }}
  export POD_NAME=$(kubectl get pods -n {{ .Release.Namespace }} -l "app.kubernetes.io/name={{ include "my-spring-app.name" . }}" -o jsonpath="{.items[0].metadata.name}")
  kubectl port-forward $POD_NAME 8080:8080 -n {{ .Release.Namespace }}
  URL: http://localhost:8080
{{- end }}

To check the status:
  helm status {{ .Release.Name }} -n {{ .Release.Namespace }}

To see logs:
  kubectl logs -f -l app.kubernetes.io/name={{ include "my-spring-app.name" . }} -n {{ .Release.Namespace }}

Database connection:
  Host: {{ .Release.Name }}-postgresql
  Port: 5432
  Database: mydb
  Username: {{ .Values.postgresql.auth.username }}
```

---

## Récapitulatif du Chapitre 9

✅ **Points clés :**

**Helm Basics :**
- Package manager pour Kubernetes
- Chart = package, Release = déploiement
- helm install/upgrade/rollback/uninstall

**Templating :**
- values.yaml pour configuration
- Go templates dans templates/
- Fonctions : if, range, default, etc.
- _helpers.tpl pour réutilisation

**Repository :**
- GitHub Pages (simple)
- ChartMuseum (self-hosted)
- OCI registries (AWS ECR, GCP Artifact Registry)

**Best Practices :**
- Values par environnement
- Secrets externalisés (helm-secrets, External Secrets)
- Hooks pour lifecycle
- Tests (dry-run, lint, template)
- Documentation (NOTES.txt)

---

## Questions d'entretien fréquentes

1. **Qu'est-ce que Helm et pourquoi l'utiliser ?**
   → Package manager, templating, versioning, rollback

2. **Différence entre Chart et Release ?**
   → Chart = package, Release = instance déployée

3. **Comment gérez-vous les secrets dans Helm ?**
   → helm-secrets + SOPS, ou External Secrets Operator

4. **Comment déployez-vous sur différents environnements ?**
   → values-{env}.yaml + helm install -f

5. **Qu'est-ce qu'un Helm hook ?**
   → Job exécuté à un moment spécifique (pre-install, post-upgrade, etc.)

6. **Comment testez-vous un Chart avant installation ?**
   → helm lint, helm template, helm install --dry-run

---

## Commandes Helm essentielles

```bash
# Repository
helm repo add <name> <url>
helm repo update
helm search repo <keyword>

# Installation
helm install <release> <chart>
helm install <release> <chart> -f values.yaml
helm install <release> <chart> --set key=value

# Gestion
helm list -A
helm status <release>
helm get values <release>
helm get manifest <release>

# Upgrade & Rollback
helm upgrade <release> <chart>
helm rollback <release> <revision>
helm history <release>

# Désinstallation
helm uninstall <release>

# Développement
helm create <chart>
helm lint <chart>
helm template <release> <chart>
helm install <release> <chart> --dry-run --debug
helm package <chart>
```
---

# 🎯 Chapitre 9 : Questions/Réponses d'Entretien
## Helm & Package Management

---

## 📋 Section 1 : Helm Basics

### Q1 : Qu'est-ce que Helm et quels problèmes résout-il ?

**Réponse :**

**Helm** est le **package manager** pour Kubernetes, souvent appelé "le apt/yum de Kubernetes".

**Problèmes sans Helm :**

**1. Duplication massive de YAML**

```yaml
# Sans Helm : 3 environnements = 3 × 10 fichiers = 30 fichiers YAML !
dev/
├── deployment.yaml
├── service.yaml
├── ingress.yaml
├── configmap.yaml
├── secret.yaml
├── hpa.yaml
├── pdb.yaml
├── serviceaccount.yaml
├── role.yaml
└── rolebinding.yaml

staging/
├── deployment.yaml  # 95% identique à dev
├── service.yaml
├── ... (10 fichiers)

production/
├── deployment.yaml  # 95% identique à dev
├── service.yaml
├── ... (10 fichiers)
```

---

**2. Difficile de gérer les versions**

```bash
# Sans Helm : Comment revenir à la version précédente ?
kubectl apply -f deployment-v1.yaml  # Espérer avoir gardé l'ancien fichier
```

---

**3. Pas de gestion de dépendances**

```yaml
# Application Spring Boot nécessite :
# - PostgreSQL
# - Redis
# - Elasticsearch

# Sans Helm : Installer manuellement chaque dépendance
kubectl apply -f postgres/
kubectl apply -f redis/
kubectl apply -f elasticsearch/
kubectl apply -f my-app/

# Et gérer les relations entre elles manuellement
```

---

**Solutions avec Helm :**

**1. Templating réutilisable**

```
1 Chart = 1 template set
├── values-dev.yaml       # Config dev
├── values-staging.yaml   # Config staging
└── values-production.yaml # Config production

→ 10 templates + 3 fichiers values = 13 fichiers au lieu de 30 !
```

---

**2. Versioning et rollback intégrés**

```bash
# Install version 1.0.0
helm install my-app ./chart --version 1.0.0

# Upgrade vers 1.1.0
helm upgrade my-app ./chart --version 1.1.0

# Rollback en 1 commande !
helm rollback my-app 1
```

---

**3. Gestion de dépendances**

```yaml
# Chart.yaml
dependencies:
  - name: postgresql
    version: "12.5.8"
    repository: https://charts.bitnami.com/bitnami
  - name: redis
    version: "17.11.3"
    repository: https://charts.bitnami.com/bitnami

# 1 commande installe tout !
helm install my-app ./chart
# → Installe app + PostgreSQL + Redis automatiquement
```

---

**4. Réutilisabilité et partage**

```bash
# Publier le Chart
helm package my-app/
helm push my-app-1.0.0.tgz oci://registry.company.com/helm

# N'importe qui peut l'utiliser
helm install my-app oci://registry.company.com/helm/my-app
```

---

**Comparaison concrète :**

| Aspect | Sans Helm | Avec Helm |
|--------|-----------|-----------|
| **Fichiers YAML** | 30+ fichiers | 10 templates + 3 values |
| **Duplication** | 95% code dupliqué | 0% duplication |
| **Versioning** | Manuel (Git tags) | Automatique (Chart version) |
| **Rollback** | Complexe | 1 commande |
| **Dependencies** | Manuel | Déclaratif |
| **Templating** | ❌ Non | ✅ Oui (Go templates) |
| **Testing** | ❌ Difficile | ✅ helm lint, dry-run |

---

**Mon expérience :**

Avant Helm : 45 minutes pour déployer une app en prod (10 fichiers YAML à éditer manuellement)

Avec Helm : 2 minutes pour déployer (`helm install my-app ./chart -f values-prod.yaml`)

→ **Gain de temps : 95%** 🚀

---

### Q2 : Expliquez la différence entre Chart, Release et Repository

**Réponse :**

Ce sont les 3 concepts fondamentaux de Helm.

**Chart** : Le **package** (template)

```
my-spring-app/
├── Chart.yaml          # Métadonnées du Chart
├── values.yaml         # Configuration par défaut
├── templates/          # Templates YAML
│   ├── deployment.yaml
│   ├── service.yaml
│   └── ingress.yaml
└── charts/             # Sub-charts (dependencies)
    ├── postgresql/
    └── redis/
```

**Analogie :**
- **npm** : Chart = package.json + code
- **Docker** : Chart = Dockerfile + context
- **apt** : Chart = .deb package

---

**Release** : Une **instance déployée** d'un Chart

```bash
# 1 Chart peut avoir plusieurs Releases
helm install payment-api ./my-spring-app -f values-prod.yaml
# Release name: payment-api

helm install user-api ./my-spring-app -f values-prod.yaml
# Release name: user-api

# Même Chart, 2 Releases différentes !
```

**Lister les releases :**

```bash
helm list -A

# Output:
# NAME         NAMESPACE    REVISION  STATUS    CHART              APP VERSION
# payment-api  production   3         deployed  my-spring-app-1.0  2.1.3
# user-api     production   1         deployed  my-spring-app-1.0  1.5.2
```

**Chaque Release a :**
- Son propre nom
- Sa propre configuration (values)
- Son propre historique (revisions)
- Ses propres ressources Kubernetes

---

**Repository** : Une **collection de Charts**

```
Helm Repository (HTTP server)
├── index.yaml                    # Index de tous les Charts
├── my-spring-app-1.0.0.tgz      # Chart version 1.0.0
├── my-spring-app-1.1.0.tgz      # Chart version 1.1.0
├── postgresql-12.5.8.tgz
└── redis-17.11.3.tgz
```

**Analogie :**
- **npm** : Repository = npmjs.com
- **Docker** : Repository = Docker Hub
- **apt** : Repository = deb.debian.org

**Ajouter un repository :**

```bash
helm repo add bitnami https://charts.bitnami.com/bitnami
helm repo update

# Rechercher dans le repo
helm search repo postgresql
# NAME                 CHART VERSION  APP VERSION
# bitnami/postgresql   12.5.8         15.3.0
```

---

**Workflow complet :**

```
1. Developer crée un CHART
   ↓
2. Developer package et publie dans REPOSITORY
   helm package my-spring-app/
   helm push my-spring-app-1.0.0.tgz oci://registry.company.com/helm
   ↓
3. Ops installe le Chart → crée une RELEASE
   helm install payment-api company/my-spring-app
   ↓
4. Release déployée dans Kubernetes
   (Deployment, Service, Ingress créés)
```

---

**Exemple concret :**

```bash
# 1. Ajouter le REPOSITORY Bitnami
helm repo add bitnami https://charts.bitnami.com/bitnami

# 2. Chercher un CHART PostgreSQL
helm search repo postgresql
# Trouve le CHART: bitnami/postgresql version 12.5.8

# 3. Installer le CHART → crée une RELEASE
helm install my-database bitnami/postgresql \
  --set auth.password=securepass \
  -n production

# RELEASE créée avec nom "my-database"

# 4. Vérifier la RELEASE
helm list -n production
# NAME          CHART                 STATUS
# my-database   postgresql-12.5.8     deployed

# 5. Installer une 2ème RELEASE du même CHART
helm install backup-database bitnami/postgresql \
  --set auth.password=anotherpass \
  -n production

# 2 RELEASES du même CHART !
```

---

**Résumé :**

| Concept | Définition | Commandes |
|---------|-----------|-----------|
| **Chart** | Package (template + config) | `helm create`, `helm package` |
| **Release** | Instance déployée d'un Chart | `helm install`, `helm upgrade`, `helm rollback` |
| **Repository** | Collection de Charts | `helm repo add`, `helm search repo` |

---

## 📋 Section 2 : Templating & Values

### Q3 : Comment gérez-vous la configuration pour différents environnements (dev, staging, prod) avec Helm ?

**Réponse :**

J'utilise des **fichiers values par environnement** avec une approche **DRY** (Don't Repeat Yourself).

**Structure du Chart :**

```
my-spring-app/
├── Chart.yaml
├── values.yaml              # Valeurs par défaut (communes)
├── values-dev.yaml          # Overrides pour dev
├── values-staging.yaml      # Overrides pour staging
├── values-production.yaml   # Overrides pour production
└── templates/
    ├── deployment.yaml
    ├── service.yaml
    └── ingress.yaml
```

---

**values.yaml (configuration commune) :**

```yaml
# Valeurs par défaut utilisées par tous les environnements
image:
  repository: registry.company.com/my-spring-app
  pullPolicy: IfNotPresent

service:
  type: ClusterIP
  port: 80

ingress:
  enabled: true
  className: nginx
  annotations:
    cert-manager.io/cluster-issuer: letsencrypt-prod

livenessProbe:
  httpGet:
    path: /actuator/health/liveness
    port: 9090
  initialDelaySeconds: 60
  periodSeconds: 10

readinessProbe:
  httpGet:
    path: /actuator/health/readiness
    port: 9090
  initialDelaySeconds: 30
  periodSeconds: 5

env:
  - name: JAVA_OPTS
    value: "-XX:+UseContainerSupport -XX:MaxRAMPercentage=75.0"

# Dependencies
postgresql:
  enabled: true
  auth:
    database: mydb

redis:
  enabled: true
```

---

**values-dev.yaml (overrides minimaux) :**

```yaml
# Seulement les différences pour dev
replicaCount: 1

image:
  tag: dev-latest
  pullPolicy: Always  # Toujours pull latest en dev

resources:
  limits:
    cpu: 500m
    memory: 512Mi
  requests:
    cpu: 100m
    memory: 256Mi

ingress:
  hosts:
    - host: my-app-dev.company.com
      paths:
        - path: /
          pathType: Prefix

env:
  - name: SPRING_PROFILES_ACTIVE
    value: "dev"
  - name: LOG_LEVEL
    value: "DEBUG"

autoscaling:
  enabled: false

# Dependencies avec moins de ressources
postgresql:
  primary:
    persistence:
      size: 10Gi
  resources:
    limits:
      memory: 512Mi
      cpu: 500m

redis:
  master:
    persistence:
      size: 1Gi
```

---

**values-staging.yaml :**

```yaml
replicaCount: 2

image:
  tag: staging-v1.2.3

resources:
  limits:
    cpu: 1000m
    memory: 1Gi
  requests:
    cpu: 500m
    memory: 512Mi

ingress:
  hosts:
    - host: my-app-staging.company.com
      paths:
        - path: /
          pathType: Prefix
  tls:
    - secretName: my-app-staging-tls
      hosts:
        - my-app-staging.company.com

env:
  - name: SPRING_PROFILES_ACTIVE
    value: "staging"
  - name: LOG_LEVEL
    value: "INFO"

autoscaling:
  enabled: true
  minReplicas: 2
  maxReplicas: 5

postgresql:
  primary:
    persistence:
      size: 50Gi

redis:
  master:
    persistence:
      size: 5Gi
```

---

**values-production.yaml (configuration maximale) :**

```yaml
replicaCount: 5

image:
  tag: v1.2.3  # Tag immutable (semantic versioning)
  pullPolicy: IfNotPresent

resources:
  limits:
    cpu: 2000m
    memory: 2Gi
  requests:
    cpu: 1000m
    memory: 1Gi

ingress:
  hosts:
    - host: api.company.com
      paths:
        - path: /
          pathType: Prefix
  tls:
    - secretName: api-company-tls
      hosts:
        - api.company.com

env:
  - name: SPRING_PROFILES_ACTIVE
    value: "prod"
  - name: LOG_LEVEL
    value: "WARN"
  - name: JAVA_OPTS
    value: "-XX:+UseContainerSupport -XX:MaxRAMPercentage=75.0 -XX:+UseG1GC -XX:MaxGCPauseMillis=200"

# Anti-affinity pour HA
affinity:
  podAntiAffinity:
    requiredDuringSchedulingIgnoredDuringExecution:
    - labelSelector:
        matchLabels:
          app.kubernetes.io/name: my-spring-app
      topologyKey: kubernetes.io/hostname

# PDB pour garantir disponibilité
podDisruptionBudget:
  enabled: true
  minAvailable: 3

# HPA avec métriques avancées
autoscaling:
  enabled: true
  minReplicas: 5
  maxReplicas: 20
  targetCPUUtilizationPercentage: 70
  targetMemoryUtilizationPercentage: 80

# Dependencies production-grade
postgresql:
  enabled: true
  primary:
    persistence:
      enabled: true
      size: 100Gi
      storageClass: fast-ssd
  resources:
    limits:
      memory: 4Gi
      cpu: 2000m
    requests:
      memory: 2Gi
      cpu: 1000m
  replication:
    enabled: true
    numSynchronousReplicas: 1
    synchronousCommit: "on"

redis:
  enabled: true
  architecture: replication
  master:
    persistence:
      enabled: true
      size: 20Gi
  replica:
    replicaCount: 2
    persistence:
      enabled: true
      size: 20Gi
```

---

**Installation par environnement :**

```bash
# Dev
helm install my-app ./my-spring-app \
  -f my-spring-app/values.yaml \
  -f my-spring-app/values-dev.yaml \
  -n dev \
  --create-namespace

# Staging
helm install my-app ./my-spring-app \
  -f my-spring-app/values.yaml \
  -f my-spring-app/values-staging.yaml \
  -n staging \
  --create-namespace

# Production
helm install my-app ./my-spring-app \
  -f my-spring-app/values.yaml \
  -f my-spring-app/values-production.yaml \
  -n production \
  --create-namespace

# Ordre important : values.yaml puis values-{env}.yaml
# Les valeurs de values-{env}.yaml override celles de values.yaml
```

---

**Override depuis command line (pour tests) :**

```bash
# Override une valeur spécifique
helm install my-app ./my-spring-app \
  -f values-production.yaml \
  --set replicaCount=10 \
  --set image.tag=v1.2.4-hotfix

# Override plusieurs valeurs
helm upgrade my-app ./my-spring-app \
  --set resources.limits.cpu=4000m \
  --set resources.limits.memory=4Gi \
  --set autoscaling.maxReplicas=30
```

---

**Avantages de cette approche :**

✅ **DRY** : Configuration commune dans values.yaml
✅ **Minimal overrides** : Chaque environnement définit seulement les différences
✅ **Lisible** : Facile de voir ce qui diffère entre environnements
✅ **Maintenable** : Changement commun = edit 1 fichier
✅ **Testable** : `helm template` pour vérifier output
✅ **Versioned** : Tous les values dans Git

---

**Visualiser les différences :**

```bash
# Générer les manifests pour dev
helm template my-app ./my-spring-app -f values-dev.yaml > dev.yaml

# Générer les manifests pour prod
helm template my-app ./my-spring-app -f values-production.yaml > prod.yaml

# Comparer
diff dev.yaml prod.yaml

# Différences :
# - replicas: 1 vs 5
# - resources: 512Mi vs 2Gi
# - ingress host: dev.company.com vs api.company.com
# - postgresql size: 10Gi vs 100Gi
```

---

**Mon retour d'expérience :**

J'ai géré 15 applications avec cette stratégie :
- values.yaml : ~200 lignes (config commune)
- values-dev.yaml : ~30 lignes (overrides minimaux)
- values-staging.yaml : ~50 lignes
- values-production.yaml : ~100 lignes (config complète + HA)

**Maintenance :** Simple ! Changement commun = 1 fichier modifié

---

### Q4 : Comment sécurisez-vous les secrets dans Helm ?

**Réponse :**

**❌ Ce qu'il NE FAUT JAMAIS faire :**

```yaml
# values-production.yaml
database:
  password: "SuperSecretPassword123!"  # ❌ EN CLAIR DANS GIT !

# C'est LA pire erreur de sécurité !
```

---

**✅ Solutions sécurisées :**

**Solution 1 : helm-secrets avec SOPS (ma préférée)**

**Installation :**

```bash
# Plugin Helm
helm plugin install https://github.com/jkroepke/helm-secrets

# SOPS
brew install sops

# Age (pour encryption)
brew install age
age-keygen -o key.txt
```

**Créer fichier de secrets :**

```yaml
# secrets.yaml (avant encryption)
database:
  username: app_user
  password: SuperSecretPassword123!

redis:
  password: RedisSecurePass456!

jwt:
  secret: MyJWTSecretKey789!
```

**Encrypt avec SOPS :**

```bash
# Encrypt
sops -e -age $(cat key.txt.pub) secrets.yaml > secrets.enc.yaml

# secrets.enc.yaml (safe pour Git)
database:
    username: ENC[AES256_GCM,data:Yx...]
    password: ENC[AES256_GCM,data:T9...]
redis:
    password: ENC[AES256_GCM,data:Kp...]
```

**Utiliser dans Helm :**

```bash
# Install avec secrets
export SOPS_AGE_KEY_FILE=key.txt

helm secrets install my-app ./my-spring-app \
  -f values-production.yaml \
  -f secrets.enc.yaml \
  -n production

# Helm decrypt automatiquement pendant l'installation
```

**Template utilisant les secrets :**

```yaml
# templates/secret.yaml
apiVersion: v1
kind: Secret
metadata:
  name: {{ include "my-spring-app.fullname" . }}
type: Opaque
stringData:
  database-username: {{ .Values.database.username }}
  database-password: {{ .Values.database.password }}
  redis-password: {{ .Values.redis.password }}
```

---

**Solution 2 : External Secrets Operator**

**Secrets stockés dans AWS Secrets Manager/HashiCorp Vault :**

```yaml
# values-production.yaml (pas de secrets !)
externalSecrets:
  enabled: true
  secretStore: aws-secrets-manager
  secrets:
    - name: database-password
      key: prod/myapp/db-password
    - name: redis-password
      key: prod/myapp/redis-password
```

**Template External Secret :**

```yaml
# templates/externalsecret.yaml
{{- if .Values.externalSecrets.enabled }}
apiVersion: external-secrets.io/v1beta1
kind: ExternalSecret
metadata:
  name: {{ include "my-spring-app.fullname" . }}
spec:
  refreshInterval: 1h
  secretStoreRef:
    name: {{ .Values.externalSecrets.secretStore }}
    kind: SecretStore
  target:
    name: {{ include "my-spring-app.fullname" . }}-secret
    creationPolicy: Owner
  data:
  {{- range .Values.externalSecrets.secrets }}
  - secretKey: {{ .name }}
    remoteRef:
      key: {{ .key }}
  {{- end }}
{{- end }}
```

**Avantages :**
- ✅ Zéro secret dans Git
- ✅ Rotation automatique
- ✅ Audit trail (qui a accédé quand)
- ✅ Centralisé (1 source de vérité)

---

**Solution 3 : Sealed Secrets (Bitnami)**

```bash
# Installation du controller
kubectl apply -f https://github.com/bitnami-labs/sealed-secrets/releases/download/v0.24.0/controller.yaml

# Créer secret normal
kubectl create secret generic my-secret \
  --from-literal=password=SuperSecret123! \
  --dry-run=client -o yaml > secret.yaml

# Seal (encrypt)
kubeseal -f secret.yaml -w sealed-secret.yaml

# sealed-secret.yaml est safe pour Git
```

**Template utilisant Sealed Secret :**

```yaml
# templates/sealedsecret.yaml
{{- if .Values.sealedSecrets.enabled }}
apiVersion: bitnami.com/v1alpha1
kind: SealedSecret
metadata:
  name: {{ include "my-spring-app.fullname" . }}
spec:
  encryptedData:
    database-password: {{ .Values.sealedSecrets.databasePassword }}
    redis-password: {{ .Values.sealedSecrets.redisPassword }}
{{- end }}
```

---

**Solution 4 : Secrets depuis CI/CD (GitLab CI)**

```yaml
# .gitlab-ci.yml
deploy-production:
  stage: deploy
  script:
    - helm upgrade my-app ./my-spring-app \
        -f values-production.yaml \
        --set database.password=$DB_PASSWORD \
        --set redis.password=$REDIS_PASSWORD \
        -n production
  variables:
    DB_PASSWORD:
      vault: production/myapp/database-password@secret
    REDIS_PASSWORD:
      vault: production/myapp/redis-password@secret
  only:
    - main
  environment:
    name: production
```

---

**Comparaison des solutions :**

| Solution | Complexité | Sécurité | Rotation | Cost |
|----------|-----------|----------|----------|------|
| **helm-secrets + SOPS** | Moyenne | Haute | Manuelle | Gratuit |
| **External Secrets** | Moyenne | Très haute | Auto | Cloud cost |
| **Sealed Secrets** | Faible | Haute | Manuelle | Gratuit |
| **CI/CD variables** | Faible | Moyenne | Manuelle | Gratuit |

---

**Ma recommandation :**

**Pour dev/staging :** helm-secrets + SOPS (simple, gratuit)

**Pour production :** External Secrets Operator + AWS Secrets Manager
- Rotation automatique
- Audit trail complet
- Intégration IAM roles
- Conformité (SOC2, ISO27001)

---

**Best practices secrets :**

1. ✅ **Jamais de secrets en clair dans Git**
2. ✅ **Rotation régulière** (tous les 90 jours)
3. ✅ **Principe du moindre privilège** (limiter accès)
4. ✅ **Audit trail** (qui a accédé quand)
5. ✅ **Secrets par environnement** (dev ≠ prod)
6. ✅ **Backup des secrets** (disaster recovery)

---

## 📋 Section 3 : Advanced Helm

### Q5 : Qu'est-ce qu'un Helm Hook et donnez des exemples d'utilisation

**Réponse :**

Un **Helm Hook** est une ressource Kubernetes (Job, Pod) exécutée à un **moment spécifique** du lifecycle d'une Release.

**Hooks disponibles :**

| Hook | Quand | Use case |
|------|-------|----------|
| `pre-install` | Avant l'installation | Setup, vérifications |
| `post-install` | Après l'installation | Tests de fumée |
| `pre-upgrade` | Avant l'upgrade | Backup DB |
| `post-upgrade` | Après l'upgrade | Migrations DB |
| `pre-delete` | Avant la suppression | Backup final |
| `post-delete` | Après la suppression | Cleanup |
| `pre-rollback` | Avant rollback | Validation |
| `post-rollback` | Après rollback | Notification |
| `test` | helm test | Tests end-to-end |

---

**Exemple 1 : pre-upgrade - Backup DB avant upgrade**

```yaml
# templates/hook-backup.yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: {{ include "my-spring-app.fullname" . }}-backup
  annotations:
    "helm.sh/hook": pre-delete
    "helm.sh/hook-weight": "-5"
    "helm.sh/hook-delete-policy": hook-succeeded
spec:
  template:
    spec:
      restartPolicy: Never
      containers:
      - name: final-backup
        image: postgres:15
        command:
        - sh
        - -c
        - |
          echo "Creating final backup before deletion..."
          pg_dump -h {{ .Release.Name }}-postgresql \
                  -U {{ .Values.postgresql.auth.username }} \
                  -d {{ .Values.postgresql.auth.database }} \
                  | gzip > /backup/final-backup-$(date +%Y%m%d-%H%M%S).sql.gz
          echo "Final backup complete. Safe to delete."
        env:
        - name: PGPASSWORD
          valueFrom:
            secretKeyRef:
              name: {{ include "my-spring-app.fullname" . }}-secret
              key: database-password
        volumeMounts:
        - name: backup
          mountPath: /backup
      volumes:
      - name: backup
        persistentVolumeClaim:
          claimName: db-backups
```

---

**Annotations importantes :**

**1. helm.sh/hook-weight**

Définit l'ordre d'exécution des hooks (plus petit = exécuté en premier)

```yaml
# Hook 1 : Weight -10 (exécuté en premier)
annotations:
  "helm.sh/hook": pre-upgrade
  "helm.sh/hook-weight": "-10"

# Hook 2 : Weight -5 (exécuté en second)
annotations:
  "helm.sh/hook": pre-upgrade
  "helm.sh/hook-weight": "-5"

# Hook 3 : Weight 0 (exécuté en dernier)
annotations:
  "helm.sh/hook": pre-upgrade
  "helm.sh/hook-weight": "0"
```

---

**2. helm.sh/hook-delete-policy**

Définit quand supprimer le hook

| Policy | Comportement |
|--------|-------------|
| `before-hook-creation` | Supprimer avant de créer nouveau hook |
| `hook-succeeded` | Supprimer après succès |
| `hook-failed` | Supprimer après échec |

```yaml
# Garder seulement en cas d'échec (pour debugging)
annotations:
  "helm.sh/hook-delete-policy": hook-succeeded

# Toujours supprimer (cleanup)
annotations:
  "helm.sh/hook-delete-policy": before-hook-creation,hook-succeeded,hook-failed
```

---

**Exemple complet : Workflow avec hooks**

```
helm upgrade my-app ./my-spring-app

1. pre-upgrade hooks (weight -10 → 0)
   ├─ Backup DB (weight -10)
   └─ Vérifications (weight -5)

2. Upgrade des ressources principales
   ├─ Deployment (rolling update)
   ├─ Service
   └─ Ingress

3. post-upgrade hooks (weight 0 → 10)
   ├─ Migrations DB (weight 0)
   ├─ Smoke tests (weight 5)
   └─ Notification Slack (weight 10)
```

---

**Mon expérience :**

J'utilise systématiquement ces hooks en production :

✅ **pre-upgrade** : Backup DB automatique (sauvé mes fesses 3 fois)
✅ **post-upgrade** : Migrations Flyway (zéro intervention manuelle)
✅ **post-install** : Smoke tests (détection précoce de problèmes)
✅ **pre-delete** : Backup final (conformité audit)

**Incident évité grâce aux hooks :**

Upgrade qui casse la DB → Hook pre-upgrade a fait backup → Rollback + restore → Aucune perte de données ✅

---

### Q6 : Comment testez-vous un Helm Chart avant de le déployer en production ?

**Réponse :**

J'utilise une approche **multi-niveaux** pour garantir la qualité.

**Niveau 1 : Validation syntaxique (local)**

```bash
# 1. Lint (vérifier syntax YAML + Chart.yaml)
helm lint my-spring-app/

# Output:
# ==> Linting my-spring-app/
# [INFO] Chart.yaml: icon is recommended
# [WARNING] templates/: directory not found
# 
# 1 chart(s) linted, 0 chart(s) failed

# 2. Vérifier avec différents values
helm lint my-spring-app/ -f values-dev.yaml
helm lint my-spring-app/ -f values-staging.yaml
helm lint my-spring-app/ -f values-production.yaml
```

---

**Niveau 2 : Dry-run (simuler installation)**

```bash
# Dry-run sans installer
helm install my-app ./my-spring-app \
  -f values-production.yaml \
  --dry-run \
  --debug \
  -n production

# Output: Tous les manifests YAML générés
# Permet de vérifier :
# - Templating correct
# - Values substitués correctement
# - Aucune erreur de syntaxe
```

---

**Niveau 3 : Template (générer manifests)**

```bash
# Générer les manifests sans installer
helm template my-app ./my-spring-app \
  -f values-production.yaml \
  -n production > manifests.yaml

# Inspecter manuellement
less manifests.yaml

# Ou vérifier avec kubectl
kubectl apply -f manifests.yaml --dry-run=client

# Comparer entre environnements
helm template my-app ./my-spring-app -f values-dev.yaml > dev.yaml
helm template my-app ./my-spring-app -f values-prod.yaml > prod.yaml
diff dev.yaml prod.yaml
```

---

**Niveau 4 : Tests unitaires (helm-unittest plugin)**

```bash
# Installation
helm plugin install https://github.com/helm-unittest/helm-unittest

# Créer tests
mkdir my-spring-app/tests
```

**Test exemple (tests/deployment_test.yaml) :**

```yaml
suite: test deployment
templates:
  - deployment.yaml
tests:
  - it: should create deployment with correct replicas
    set:
      replicaCount: 3
    asserts:
      - equal:
          path: spec.replicas
          value: 3
  
  - it: should use correct image
    set:
      image.repository: myapp
      image.tag: v1.0.0
    asserts:
      - equal:
          path: spec.template.spec.containers[0].image
          value: myapp:v1.0.0
  
  - it: should set resources correctly
    set:
      resources:
        limits:
          cpu: 1000m
          memory: 1Gi
    asserts:
      - equal:
          path: spec.template.spec.containers[0].resources.limits.cpu
          value: 1000m
      - equal:
          path: spec.template.spec.containers[0].resources.limits.memory
          value: 1Gi
  
  - it: should enable autoscaling when enabled
    set:
      autoscaling.enabled: true
    template: hpa.yaml
    asserts:
      - isKind:
          of: HorizontalPodAutoscaler
  
  - it: should not create PDB when disabled
    set:
      pdb.enabled: false
    template: pdb.yaml
    asserts:
      - hasDocuments:
          count: 0
```

**Exécuter tests :**

```bash
helm unittest my-spring-app/

# Output:
# ### Chart [ my-spring-app ] my-spring-app
#  PASS  test deployment	tests/deployment_test.yaml
# 
# Charts:      1 passed, 1 total
# Test Suites: 1 passed, 1 total
# Tests:       5 passed, 5 total
# Snapshot:    0 passed, 0 total
# Time:        1.234s
```

---

**Niveau 5 : Déploiement en dev (environnement isolé)**

```bash
# 1. Déployer en dev
helm install my-app ./my-spring-app \
  -f values-dev.yaml \
  -n dev \
  --create-namespace

# 2. Vérifier que tout démarre
kubectl get pods -n dev -w
kubectl get all -n dev

# 3. Vérifier health
kubectl port-forward svc/my-app 8080:80 -n dev
curl http://localhost:8080/actuator/health

# 4. Tester API
curl http://localhost:8080/api/users

# 5. Vérifier logs
kubectl logs -f deployment/my-app -n dev
```

---

**Niveau 6 : Tests d'intégration automatisés**

```yaml
# templates/test-pod.yaml
apiVersion: v1
kind: Pod
metadata:
  name: {{ include "my-spring-app.fullname" . }}-test
  annotations:
    "helm.sh/hook": test
    "helm.sh/hook-delete-policy": hook-succeeded
spec:
  restartPolicy: Never
  containers:
  - name: test
    image: curlimages/curl:latest
    command:
    - sh
    - -c
    - |
      # Test 1: Health check
      echo "Testing health endpoint..."
      curl -f http://{{ include "my-spring-app.fullname" . }}/actuator/health || exit 1
      
      # Test 2: API endpoint
      echo "Testing API..."
      curl -f http://{{ include "my-spring-app.fullname" . }}/api/users?page=0&size=1 || exit 1
      
      # Test 3: Database connectivity
      echo "Testing database connectivity..."
      curl -f http://{{ include "my-spring-app.fullname" . }}/actuator/health/db || exit 1
      
      # Test 4: Redis connectivity
      echo "Testing redis connectivity..."
      curl -f http://{{ include "my-spring-app.fullname" . }}/actuator/health/redis || exit 1
      
      echo "All tests passed!"
```

**Exécuter tests :**

```bash
# Après installation
helm test my-app -n dev

# Output:
# NAME: my-app
# LAST DEPLOYED: Mon Nov 18 14:30:00 2024
# NAMESPACE: dev
# STATUS: deployed
# TEST SUITE:     my-app-test
# Last Started:   Mon Nov 18 14:35:00 2024
# Last Completed: Mon Nov 18 14:35:30 2024
# Phase:          Succeeded
```

---

**Niveau 7 : Smoke tests en staging**

```bash
# 1. Déployer en staging
helm upgrade my-app ./my-spring-app \
  -f values-staging.yaml \
  -n staging \
  --install

# 2. Attendre rollout complet
kubectl rollout status deployment/my-app -n staging

# 3. Automated smoke tests (script custom)
./scripts/smoke-tests.sh staging

# 4. Load testing (optionnel)
k6 run load-test.js --env ENV=staging

# 5. Si OK pendant 24h → Promouvoir vers prod
```

---

**Niveau 8 : Blue/Green en production**

```bash
# 1. Déployer version "green" (nouvelle)
helm install my-app-green ./my-spring-app \
  -f values-production.yaml \
  --set nameOverride=my-app-green \
  --set service.selector.version=green \
  -n production

# 2. Tester green (sans trafic prod)
kubectl port-forward svc/my-app-green 8081:80 -n production
curl http://localhost:8081/actuator/health

# 3. Smoke tests sur green
./scripts/smoke-tests.sh production green

# 4. Si OK → Basculer trafic
kubectl patch service my-app -n production \
  -p '{"spec":{"selector":{"version":"green"}}}'

# 5. Monitorer pendant 1h
# Grafana : error rate, latency, throughput

# 6. Si OK → Supprimer blue
helm uninstall my-app-blue -n production

# 7. Renommer green → blue pour prochain déploiement
```

---

**Pipeline CI/CD complet :**

```yaml
# .gitlab-ci.yml
stages:
  - lint
  - test
  - deploy-dev
  - test-dev
  - deploy-staging
  - test-staging
  - deploy-production

helm-lint:
  stage: lint
  script:
    - helm lint my-spring-app/
    - helm lint my-spring-app/ -f values-dev.yaml
    - helm lint my-spring-app/ -f values-production.yaml

helm-unittest:
  stage: test
  script:
    - helm plugin install https://github.com/helm-unittest/helm-unittest
    - helm unittest my-spring-app/

deploy-dev:
  stage: deploy-dev
  script:
    - helm upgrade my-app ./my-spring-app 
        -f values-dev.yaml 
        -n dev 
        --install 
        --wait 
        --timeout 10m
  only:
    - develop

test-dev:
  stage: test-dev
  script:
    - helm test my-app -n dev
    - ./scripts/smoke-tests.sh dev
  only:
    - develop

deploy-staging:
  stage: deploy-staging
  script:
    - helm upgrade my-app ./my-spring-app 
        -f values-staging.yaml 
        -n staging 
        --install 
        --wait 
        --timeout 10m
  only:
    - main

test-staging:
  stage: test-staging
  script:
    - helm test my-app -n staging
    - ./scripts/smoke-tests.sh staging
    - k6 run load-test.js --env ENV=staging
  only:
    - main

deploy-production:
  stage: deploy-production
  script:
    - helm upgrade my-app ./my-spring-app 
        -f values-production.yaml 
        -n production 
        --install 
        --wait 
        --timeout 15m
  only:
    - tags
  when: manual
  environment:
    name: production
    url: https://api.company.com
```

---

**Checklist complète avant production :**

```
☐ helm lint passed
☐ helm unittest passed (100% coverage)
☐ Dry-run successful
☐ Template inspection OK
☐ Deployed in dev successfully
☐ Integration tests passed in dev
☐ Deployed in staging successfully
☐ Smoke tests passed in staging
☐ Load tests passed in staging (if applicable)
☐ Monitored in staging for 24h+ (no issues)
☐ Rollback plan documented
☐ Team notified
☐ Maintenance window scheduled (if needed)
```

---

**Mon retour d'expérience :**

Avec ce process en place :
- **0 incident** en production sur 50+ déploiements
- **Bugs détectés** : 95% en dev, 4% en staging, 1% en prod
- **Confiance** : 100% lors du déploiement prod

Le niveau de tests dépend de la criticité :
- App interne → Niveaux 1-4 suffisent
- App client critique → Tous les niveaux (1-8)

---

## 📋 Section 4 : Helm in Production

### Q7 : Comment gérez-vous les upgrades de Charts en production avec Helm ?

**Réponse :**

J'utilise une stratégie **progressive et sécurisée** avec plusieurs garde-fous.

**Stratégie complète :**

**Phase 1 : Préparation (avant l'upgrade)**

```bash
# 1. Vérifier l'état actuel
helm list -n production
helm status my-app -n production

# Output:
# NAME    NAMESPACE    REVISION  STATUS    CHART
# my-app  production   12        deployed  my-spring-app-1.5.0

# 2. Voir l'historique
helm history my-app -n production

# Output:
# REVISION  UPDATED                   STATUS      CHART                 DESCRIPTION
# 10        Mon Oct 30 14:00:00 2024  superseded  my-spring-app-1.4.0  Upgrade complete
# 11        Wed Nov 01 10:00:00 2024  superseded  my-spring-app-1.4.1  Hotfix
# 12        Mon Nov 13 15:00:00 2024  deployed    my-spring-app-1.5.0  Upgrade complete

# 3. Backup de la configuration actuelle
helm get values my-app -n production > backup-values-rev12.yaml
helm get manifest my-app -n production > backup-manifest-rev12.yaml

# 4. Backup de la base de données (via hook ou manuel)
kubectl exec -it my-app-postgres-0 -n production -- \
  pg_dump -U postgres mydb | gzip > backup-$(date +%Y%m%d).sql.gz
```

---

**Phase 2 : Dry-run (validation)**

```bash
# Test de l'upgrade sans l'exécuter
helm upgrade my-app ./my-spring-app \
  -f values-production.yaml \
  --version 1.6.0 \
  -n production \
  --dry-run \
  --debug > upgrade-plan.yaml

# Inspecter les changements
less upgrade-plan.yaml

# Vérifier avec kubectl
kubectl diff -f upgrade-plan.yaml -n production
```

---

**Phase 3 : Upgrade progressif**

**Option A : Upgrade standard (Rolling Update)**

```bash
# Upgrade avec monitoring
helm upgrade my-app ./my-spring-app \
  -f values-production.yaml \
  --version 1.6.0 \
  -n production \
  --wait \
  --timeout 10m \
  --atomic

# Flags importants:
# --wait : Attendre que les ressources soient Ready
# --timeout 10m : Timeout max 10 minutes
# --atomic : Rollback automatique si échec
```

---

**Option B : Upgrade avec PDB (garantit disponibilité)**

```yaml
# PDB déjà en place
apiVersion: policy/v1
kind: PodDisruptionBudget
metadata:
  name: my-app-pdb
spec:
  minAvailable: 3  # Au moins 3 pods pendant upgrade
```

```bash
# Upgrade (PDB protège automatiquement)
helm upgrade my-app ./my-spring-app \
  -f values-production.yaml \
  --version 1.6.0 \
  -n production \
  --wait \
  --atomic
```

---

**Option C : Blue/Green Deployment**

```bash
# 1. Installer version "green" (nouvelle) en parallèle
helm install my-app-green ./my-spring-app \
  -f values-production.yaml \
  --version 1.6.0 \
  --set nameOverride=my-app-green \
  --set fullnameOverride=my-app-green \
  -n production

# 2. Attendre que green soit Ready
kubectl rollout status deployment/my-app-green -n production

# 3. Tester green (sans trafic utilisateur)
kubectl port-forward svc/my-app-green 8081:80 -n production
curl http://localhost:8081/actuator/health
./scripts/smoke-tests.sh green

# 4. Basculer le Service vers green
kubectl patch service my-app -n production \
  -p '{"spec":{"selector":{"version":"green"}}}'

# 5. Monitorer pendant 1h
# Grafana : error rate, latency, logs

# 6. Si OK → Supprimer blue
helm uninstall my-app-blue -n production

# 7. Renommer green → my-app pour cohérence
helm upgrade my-app-green ./my-spring-app \
  --set nameOverride=my-app \
  -n production
```

---

**Phase 4 : Validation post-upgrade**

```bash
# 1. Vérifier status
helm status my-app -n production

# 2. Vérifier pods
kubectl get pods -n production -l app.kubernetes.io/instance=my-app

# All pods should be Running

# 3. Vérifier rollout
kubectl rollout status deployment/my-app -n production

# 4. Health checks
kubectl exec -it deployment/my-app -n production -- \
  curl http://localhost:8080/actuator/health

# 5. Tester endpoints critiques
curl https://api.company.com/actuator/health
curl https://api.company.com/api/users?page=0&size=1

# 6. Vérifier logs (pas d'erreurs)
kubectl logs -f deployment/my-app -n production --tail=100 | grep ERROR

# 7. Vérifier métriques Prometheus
# - Error rate < 0.1%
# - Latency p95 < 500ms
# - CPU/Memory dans limites normales
```

---

**Phase 5 : Rollback si nécessaire**

```bash
# Si problème détecté :

# Option 1 : Rollback automatique (si --atomic utilisé)
# Helm rollback automatiquement si échec

# Option 2 : Rollback manuel vers révision précédente
helm rollback my-app -n production

# Option 3 : Rollback vers révision spécifique
helm rollback my-app 12 -n production

# Option 4 : Rollback Blue/Green (instant)
kubectl patch service my-app -n production \
  -p '{"spec":{"selector":{"version":"blue"}}}'

# Vérifier le rollback
helm history my-app -n production
kubectl get pods -n production -l app.kubernetes.io/instance=my-app
```

---

**Monitoring pendant l'upgrade :**

```bash
# Terminal 1 : Watch pods
watch -n 2 'kubectl get pods -n production -l app.kubernetes.io/instance=my-app'

# Terminal 2 : Watch events
kubectl get events -n production --watch

# Terminal 3 : Logs en temps réel
kubectl logs -f deployment/my-app -n production --all-containers

# Terminal 4 : Métriques (Prometheus/Grafana)
# - http_requests_total
# - http_request_duration_seconds
# - error_rate
```

---

**Helm upgrade avec hooks (migrations DB) :**

```yaml
# templates/hook-backup.yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: {{ include "my-spring-app.fullname" . }}-pre-upgrade-backup
  annotations:
    "helm.sh/hook": pre-upgrade
    "helm.sh/hook-weight": "-5"
spec:
  template:
    spec:
      restartPolicy: Never
      containers:
      - name: backup
        # ... backup DB ...

---
# templates/hook-migration.yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: {{ include "my-spring-app.fullname" . }}-migration
  annotations:
    "helm.sh/hook": post-upgrade
    "helm.sh/hook-weight": "0"
spec:
  template:
    spec:
      restartPolicy: Never
      containers:
      - name: flyway
        # ... run migrations ...
```

**Séquence avec hooks :**

```
helm upgrade my-app ./my-spring-app

1. Pre-upgrade hooks
   └─ Backup DB

2. Upgrade Deployment (Rolling Update)
   ├─ Scale up new pods
   ├─ Wait Ready
   └─ Scale down old pods

3. Post-upgrade hooks
   └─ Run DB migrations

4. Upgrade complete
```

---

**Best practices upgrade :**

✅ **Toujours** utiliser `--atomic` (rollback auto si échec)
✅ **PDB** configuré (minAvailable)
✅ **Backup** avant upgrade (DB + manifests)
✅ **Dry-run** pour validation
✅ **Monitoring** actif pendant upgrade
✅ **Heures creuses** (faible trafic)
✅ **Rollback plan** documenté
✅ **Communication** : Notifier équipe avant/après

---

**Mon expérience :**

J'ai fait **50+ upgrades** en production avec cette stratégie :

**Incidents : 2**
- **Incident 1** : Migration DB trop lente (30 min au lieu de 2 min)
  - Solution : Hook pre-upgrade avec timeout augmenté
  - Rollback automatique (--atomic)
  - Aucune perte de données

- **Incident 2** : Incompatibilité version PostgreSQL
  - Détecté en dry-run (Phase 2)
  - Upgrade annulé avant prod
  - Bug fixé en staging

**Taux de succès : 96%** (48/50 upgrades sans incident)

---

## 🎓 Conseils Finaux pour l'Entretien

### Points clés à mentionner :

✅ **Helm = Package manager** : Simplifie déploiements Kubernetes
✅ **Templating** : Réutilisation, pas de duplication
✅ **Versioning** : Rollback en 1 commande
✅ **Hooks** : Automation (backups, migrations, tests)
✅ **Secrets** : helm-secrets + SOPS, ou External Secrets
✅ **Testing** : Multi-niveaux (lint → unittest → staging → prod)
✅ **Production** : Blue/Green, PDB, monitoring, rollback plan

---

### Questions à poser au recruteur :

1. Utilisez-vous Helm actuellement ?
2. Comment gérez-vous les secrets dans vos Charts ?
3. Quelle stratégie de déploiement ? (Blue/Green, Canary)
4. Avez-vous des Charts customs ou utilisez-vous des Charts publics ?
5. Comment testez-vous les Charts avant production ?

---

### Erreurs à éviter :

❌ Dire "Helm c'est juste pour packager des YAML"
❌ Ne pas mentionner la sécurité des secrets
❌ Ignorer les hooks et le lifecycle management
❌ Ne pas parler de tests (lint, dry-run, unittest)
❌ Oublier le rollback plan

---

## 📊 Récapitulatif Chapitre 9

✅ **Helm Basics**
- Package manager pour Kubernetes
- Chart, Release, Repository
- install, upgrade, rollback, uninstall

✅ **Templating & Values**
- Go templates pour réutilisation
- values.yaml par environnement
- Fonctions : if, range, default, include

✅ **Secrets Management**
- helm-secrets + SOPS
- External Secrets Operator
- Sealed Secrets
- CI/CD variables

✅ **Advanced**
- Hooks (pre-install, post-upgrade, etc.)
- Testing (lint, unittest, dry-run)
- Blue/Green deployments
- PDB pour zero downtime

✅ **Production**
- Upgrade strategy progressive
- Monitoring et rollback
- Best practices