# Questions d'entretien DevOps pour un Dev Spring Boot

Je vais te donner les questions DevOps essentielles qu'un Tech Lead pourrait te poser, adaptées à ton profil Spring Boot + K8s.

## **Questions CI/CD**

**1. Explique ton pipeline CI/CD pour une application Spring Boot**
**Réponse type :**
- **CI** : 
  - Trigger sur push/PR (GitHub Actions, GitLab CI, Jenkins)
  - Build Maven/Gradle : `mvn clean package`
  - Tests unitaires et d'intégration
  - Analyse qualité : SonarQube (code smells, coverage, vulnérabilités)
  - Build Docker image
  - Scan de sécurité : Trivy, Snyk pour les vulnérabilités
  - Push vers registry (Docker Hub, ECR, Harbor)
- **CD** :
  - Déploiement sur K8s (kubectl apply, Helm, ArgoCD)
  - Tests smoke en environnement de staging
  - Promotion vers prod (manuelle ou automatique selon la stratégie)

**2. Différence entre CI et CD ?**
- **CI (Continuous Integration)** : intégration fréquente du code, build et tests automatiques
- **CD (Continuous Delivery)** : livraison automatique jusqu'à la prod (mais déploiement manuel)
- **CD (Continuous Deployment)** : déploiement automatique en prod sans intervention humaine
- L'objectif : réduire le time-to-market et les risques

**3. Comment gérer les versions de ton application ?**
- **Semantic Versioning** : MAJOR.MINOR.PATCH (1.2.3)
- Tags Git pour chaque release
- Image Docker taguée avec la version + latest
- Éviter `latest` en prod (préférer des versions fixes)
- Maven versions plugin ou Git tags pour versionning automatique

**4. Qu'est-ce que GitOps ?**
- Infrastructure et déploiements gérés via Git comme source de vérité
- Tout est déclaratif et versionné dans Git
- Outils : ArgoCD, FluxCD
- Avantages : traçabilité, rollback facile, review process
- Le cluster "pull" la config depuis Git (vs push traditionnel)

**5. Comment faire un rollback d'un déploiement raté ?**
```bash
# Kubernetes
kubectl rollout undo deployment/my-app
kubectl rollout history deployment/my-app
kubectl rollout undo deployment/my-app --to-revision=2

# Helm
helm rollback my-release 1

# GitOps (ArgoCD)
git revert <commit> && git push
```

## **Questions Docker**

**6. Montre-moi un Dockerfile optimisé pour Spring Boot**
```dockerfile
# Multi-stage build
FROM maven:3.9-eclipse-temurin-17 AS build
WORKDIR /app
COPY pom.xml .
RUN mvn dependency:go-offline
COPY src ./src
RUN mvn clean package -DskipTests

FROM eclipse-temurin:17-jre-alpine
WORKDIR /app
# Créer un user non-root
RUN addgroup -S spring && adduser -S spring -G spring
USER spring:spring
COPY --from=build /app/target/*.jar app.jar
EXPOSE 8080
ENTRYPOINT ["java", "-jar", "app.jar"]
```

**Optimisations :**
- Multi-stage : image finale plus légère (JRE vs JDK)
- Layer caching : copier pom.xml avant src
- User non-root pour la sécurité
- Image alpine pour réduire la taille
- `.dockerignore` pour exclure target/, .git/

**7. Différence entre CMD et ENTRYPOINT ?**
- **ENTRYPOINT** : commande qui sera toujours exécutée (non remplaçable)
- **CMD** : arguments par défaut pour ENTRYPOINT (remplaçables)
```dockerfile
ENTRYPOINT ["java", "-jar"]
CMD ["app.jar"]
# docker run image  → java -jar app.jar
# docker run image other.jar  → java -jar other.jar
```

**8. Comment optimiser la taille d'une image Docker ?**
- Utiliser des images de base légères : alpine, distroless
- Multi-stage builds
- Nettoyer le cache : `RUN apt-get clean && rm -rf /var/lib/apt/lists/*`
- Minimiser les layers (combiner les RUN)
- .dockerignore pour exclure les fichiers inutiles
- Squash layers si nécessaire

**9. Qu'est-ce que Docker Compose et quand l'utiliser ?**
- Outil pour définir des applications multi-conteneurs
- Fichier `docker-compose.yml` déclaratif
- Utile pour le dev local : app + DB + Redis + etc.
- Pas pour la prod (utiliser K8s, Docker Swarm)
```yaml
version: '3.8'
services:
  app:
    build: .
    ports:
      - "8080:8080"
    depends_on:
      - db
  db:
    image: postgres:15
    environment:
      POSTGRES_PASSWORD: secret
```

**10. Comment scanner les vulnérabilités d'une image ?**
- **Trivy** : `trivy image my-image:tag`
- **Snyk** : `snyk container test my-image:tag`
- **Clair** : scanner intégré dans les registries
- Intégrer le scan dans le pipeline CI
- Définir des policies : bloquer si vulnérabilités critiques

## **Questions Infrastructure as Code**

**11. Qu'est-ce que Terraform ?**
- Outil IaC pour provisionner l'infrastructure
- Déclaratif (HCL - HashiCorp Configuration Language)
- Multi-cloud : AWS, Azure, GCP, etc.
- State management : tfstate stocke l'état actuel
- Plan → Apply → Destroy
```hcl
resource "aws_instance" "web" {
  ami           = "ami-0c55b159cbfafe1f0"
  instance_type = "t2.micro"
  tags = {
    Name = "WebServer"
  }
}
```

**12. Terraform vs Ansible ?**
- **Terraform** : provisioning d'infrastructure (créer des VMs, networks, K8s clusters)
- **Ansible** : configuration management (installer/configurer des softwares sur des serveurs existants)
- Terraform = immutable infrastructure
- Ansible = mutable configuration
- Souvent utilisés ensemble

**13. Qu'est-ce qu'Ansible et ses use cases ?**
- Outil de configuration management et automation
- Agentless (SSH)
- Playbooks YAML déclaratifs
- Idempotent : rejouer n'a pas d'effet si déjà appliqué
- Use cases : déploiement d'apps, config de serveurs, orchestration

**14. Comment gérer les secrets dans Terraform ?**
- **Ne JAMAIS** commiter les secrets en clair
- Variables d'environnement : `TF_VAR_db_password`
- Terraform Cloud/Enterprise : encrypted variables
- HashiCorp Vault integration
- AWS Secrets Manager / Parameter Store
```hcl
data "aws_secretsmanager_secret_version" "db_password" {
  secret_id = "prod/db/password"
}
```

## **Questions Monitoring et Observabilité**

**15. Comment monitorer une application Spring Boot en production ?**
**Stack complète :**
- **Métriques** : Prometheus + Grafana
  - Spring Boot Actuator expose `/actuator/prometheus`
  - Micrometer pour les métriques custom
  - Dashboards Grafana pour visualiser
- **Logs** : ELK/EFK Stack
  - Logback/Log4j2 en JSON format
  - Fluentd/Filebeat collecte les logs
  - Elasticsearch pour stockage/recherche
  - Kibana pour visualisation
- **Tracing** : Zipkin/Jaeger
  - Spring Cloud Sleuth pour distributed tracing
  - Voir les requêtes à travers les microservices
- **APM** : Datadog, New Relic, Dynatrace

**16. Qu'est-ce que Prometheus et comment ça marche ?**
- Système de monitoring open-source orienté time-series
- **Pull model** : Prometheus scrape les targets périodiquement
- PromQL pour requêter les métriques
- Alertmanager pour les alertes (Slack, email, PagerDuty)
- Service Discovery : détecte automatiquement les targets K8s
```yaml
# Spring Boot expose les métriques
management:
  endpoints:
    web:
      exposure:
        include: health,info,prometheus
  metrics:
    export:
      prometheus:
        enabled: true
```

**17. Différence entre logging et monitoring ?**
- **Logging** : événements discrets, debugging, audit
  - Quoi : "User X logged in", "Error occurred"
  - Format : texte structuré (JSON)
- **Monitoring** : métriques agrégées, performance, alertes
  - Quoi : CPU 80%, latency 200ms, requests/sec
  - Format : time-series numériques
- Les deux sont complémentaires

**18. Qu'est-ce qu'un SLA, SLO, SLI ?**
- **SLI (Service Level Indicator)** : métrique mesurable (latency, uptime, error rate)
- **SLO (Service Level Objective)** : objectif interne (99.9% uptime)
- **SLA (Service Level Agreement)** : contrat avec le client (99.95% uptime garanti)
- Exemple : SLI = latency P95, SLO = P95 < 200ms, SLA = 99.9% uptime

**19. Comment gérer les logs dans une architecture microservices ?**
- Centralisation obligatoire (pas de logs sur les pods éphémères)
- Corrélation via Trace ID (Spring Cloud Sleuth)
- Format JSON structuré pour faciliter le parsing
- Retention policy selon les besoins (compliance, coûts)
- Niveaux de logs adaptés : DEBUG en dev, WARN/ERROR en prod

## **Questions Cloud (AWS principalement)**

**20. Quels services AWS utilises-tu pour déployer Spring Boot ?**
- **Compute** : EKS (K8s), ECS (containers), EC2, Lambda (serverless)
- **Storage** : S3 (fichiers), EBS (disques), EFS (NFS)
- **Database** : RDS (PostgreSQL, MySQL), DynamoDB, ElastiCache (Redis)
- **Network** : VPC, ALB/NLB, Route53 (DNS), CloudFront (CDN)
- **CI/CD** : CodePipeline, CodeBuild, ECR (registry Docker)
- **Monitoring** : CloudWatch (logs, metrics, alarms)

**21. Qu'est-ce qu'un VPC ?**
- Virtual Private Cloud : réseau isolé dans AWS
- Subnets : public (avec Internet Gateway) et private
- Route Tables : règles de routage
- Security Groups : firewall stateful au niveau instance
- NACLs : firewall stateless au niveau subnet
- NAT Gateway : pour que les instances privées accèdent à Internet

**22. Différence entre ALB et NLB ?**
- **ALB (Application Load Balancer)** :
  - Layer 7 (HTTP/HTTPS)
  - Routing basé sur path, hostname, headers
  - SSL termination
  - WebSockets, HTTP/2
- **NLB (Network Load Balancer)** :
  - Layer 4 (TCP/UDP)
  - Ultra low latency, millions de req/sec
  - Préserve IP source
  - Pas de SSL termination

**23. Comment sécuriser une application sur AWS ?**
- **IAM** : principe du moindre privilège, MFA
- **Security Groups** : règles entrantes/sortantes strictes
- **Secrets Manager** : stockage sécurisé des credentials
- **KMS** : chiffrement des données (at rest, in transit)
- **WAF** : protection contre OWASP Top 10
- **VPC** : isolation réseau, private subnets
- **GuardDuty** : détection de menaces
- **Inspector** : scan de vulnérabilités

**24. Qu'est-ce que S3 et ses use cases ?**
- Object storage hautement disponible et durable
- Use cases :
  - Stockage de fichiers statiques (images, videos, PDFs)
  - Backup et archivage
  - Data lake pour analytics
  - Hébergement de sites statiques
  - Logs centralisés
- Storage classes : Standard, IA, Glacier (selon fréquence d'accès)

## **Questions Scripting et Automatisation**

**25. Quel langage de scripting utilises-tu et pourquoi ?**
- **Bash** : automation système, scripts CI/CD simples
- **Python** : scripts complexes, manipulation de données, API calls
- **Groovy** : Jenkins pipelines
- Exemple : script de déploiement, backup automatique, healthcheck

**26. Donne un exemple de script Bash utile**
```bash
#!/bin/bash
# Healthcheck et restart si nécessaire

APP_URL="http://localhost:8080/actuator/health"
MAX_RETRIES=3

for i in $(seq 1 $MAX_RETRIES); do
  if curl -f -s $APP_URL > /dev/null; then
    echo "App is healthy"
    exit 0
  fi
  echo "Attempt $i failed, retrying..."
  sleep 5
done

echo "App is down, restarting..."
kubectl rollout restart deployment/my-app
```

**27. Comment automatiser les tâches répétitives ?**
- **Cron jobs** : tâches planifiées (backup, cleanup)
- **K8s CronJobs** : pour les tâches dans le cluster
- **Scripts** : Bash, Python pour l'automation
- **Webhooks** : trigger sur événements (GitHub, Slack)
- **Runbooks** : documentation des procédures manuelles → automation progressive

## **Questions Sécurité DevSecOps**

**28. Qu'est-ce que le shift-left security ?**
- Intégrer la sécurité dès le début du cycle de dev
- Scans de vulnérabilités dans le pipeline CI
- SAST (Static Analysis) : SonarQube, Checkmarx
- DAST (Dynamic Analysis) : OWASP ZAP
- Dependency scanning : OWASP Dependency Check, Snyk
- Container scanning : Trivy, Clair

**29. Comment gérer les secrets dans un pipeline CI/CD ?**
- **Ne JAMAIS** hardcoder dans le code ou Dockerfile
- Variables d'environnement chiffrées dans GitLab CI/GitHub Actions
- HashiCorp Vault : stockage centralisé
- AWS Secrets Manager, Azure Key Vault
- Sealed Secrets pour K8s
- Injection au runtime, pas au build time

**30. Qu'est-ce que OWASP Top 10 ?**
Les 10 vulnérabilités les plus critiques :
1. **Broken Access Control** : mauvaise gestion des permissions
2. **Cryptographic Failures** : données sensibles non chiffrées
3. **Injection** : SQL injection, command injection
4. **Insecure Design** : failles de conception
5. **Security Misconfiguration** : configs par défaut non sécurisées
6. **Vulnerable Components** : dépendances obsolètes
7. **Authentication Failures** : mauvaise gestion auth/session
8. **Software/Data Integrity Failures** : CI/CD non sécurisé
9. **Logging/Monitoring Failures** : pas de détection d'intrusion
10. **SSRF** : Server-Side Request Forgery

## **Questions Performance et Optimisation**

**31. Comment optimiser les performances d'une app Spring Boot ?**
- **JVM tuning** : heap size, GC tuning
```bash
java -Xms512m -Xmx2g -XX:+UseG1GC -jar app.jar
```
- **Connection pooling** : HikariCP (par défaut dans Spring Boot)
- **Caching** : Redis, Caffeine, @Cacheable
- **Lazy loading** : JPA fetch strategies
- **Compression** : Gzip pour les responses HTTP
- **Database indexing** : analyser les slow queries
- **Async processing** : @Async, CompletableFuture
- **Profiling** : JProfiler, YourKit, VisualVM

**32. Comment faire du load testing ?**
- **JMeter** : simulation de charge HTTP
- **Gatling** : tests de performance as code (Scala)
- **k6** : moderne, scriptable en JS
- **Locust** : Python-based, distribué
- Métriques à surveiller : throughput, latency (P50, P95, P99), error rate

**33. Qu'est-ce que le circuit breaker pattern ?**
- Empêche les appels vers un service défaillant
- États : Closed (normal), Open (erreurs), Half-Open (test de récupération)
- Spring Cloud Netflix Hystrix (déprécié) → **Resilience4j**
```java
@CircuitBreaker(name = "backendService", fallbackMethod = "fallback")
public String callExternalService() {
    return restTemplate.getForObject(url, String.class);
}

public String fallback(Exception e) {
    return "Service temporairement indisponible";
}
```

## **Questions Git et Versioning**

**34. Quelle stratégie de branching utilises-tu ?**
- **Git Flow** : master, develop, feature/, release/, hotfix/
  - Complexe, adapté aux releases planifiées
- **GitHub Flow** : master + feature branches, PR, CI/CD
  - Simple, adapté au déploiement continu
- **Trunk-Based Development** : tout sur main, feature flags
  - Pour les équipes matures avec CI/CD robuste

**35. Comment gérer les conflits de merge ?**
- Pulls fréquents pour rester à jour
- Branches courtes (< 2-3 jours)
- Rebase vs Merge selon la stratégie d'équipe
```bash
git pull --rebase origin main
# Résoudre les conflits
git add .
git rebase --continue
```

**36. Qu'est-ce qu'un conventional commit ?**
- Format standardisé pour les messages de commit
```
feat: add user authentication
fix: resolve memory leak in cache
docs: update README with deployment steps
refactor: simplify payment processing logic
```
- Facilite le changelog automatique et semantic versioning

## **Questions Architecture et Best Practices**

**37. Les 12 Factor App - peux-tu en citer quelques-uns ?**
1. **Codebase** : un repo Git par app
2. **Dependencies** : déclarer explicitement (Maven/Gradle)
3. **Config** : dans l'environnement, pas dans le code
4. **Backing services** : traiter comme des ressources attachées
5. **Build, release, run** : séparation stricte
6. **Processes** : stateless, share-nothing
7. **Port binding** : exposer via port binding
8. **Concurrency** : scale out via le process model
9. **Disposability** : démarrage rapide, shutdown graceful
10. **Dev/prod parity** : environnements similaires
11. **Logs** : traiter comme des event streams
12. **Admin processes** : one-off admin tasks

**38. Qu'est-ce que le blue-green deployment ?**
- Deux environnements identiques : Blue (actuel) et Green (nouveau)
- Déployer sur Green, tester
- Basculer le trafic de Blue vers Green
- Garder Blue pour rollback rapide si nécessaire
- Zero downtime

**39. Différence entre horizontal et vertical scaling ?**
- **Vertical (scale up)** : ajouter plus de resources (CPU, RAM) à une instance
  - Limité par le hardware
  - Downtime souvent requis
- **Horizontal (scale out)** : ajouter plus d'instances
  - Quasi illimité
  - Pas de downtime
  - Nécessite load balancing et apps stateless

**40. Comment assurer le zero-downtime deployment ?**
- Rolling updates avec readiness probes
- PodDisruptionBudgets dans K8s
- Graceful shutdown (Spring Boot)
```yaml
spring:
  lifecycle:
    timeout-per-shutdown-phase: 30s
server:
  shutdown: graceful
```
- Health checks pendant le déploiement
- Blue-green ou canary si critique

## **Questions Troubleshooting**

**41. Une app Spring Boot est lente en production, comment debugger ?**
1. **Logs** : activer DEBUG temporairement, chercher slow queries
2. **Metrics** : Prometheus pour CPU, mémoire, GC, latency
3. **APM** : Datadog/New Relic pour tracer les requêtes
4. **Database** : analyser les slow queries (EXPLAIN), indexing
5. **JVM** : heap dump, thread dump pour détecter les bottlenecks
6. **Network** : latency vers les services externes
7. **Profiling** : JProfiler en local pour reproduire

**42. Comment investiguer un problème de mémoire (OOM) ?**
- Analyser les logs : `java.lang.OutOfMemoryError: Java heap space`
- Générer un heap dump : `-XX:+HeapDumpOnOutOfMemoryError`
- Analyser avec VisualVM, Eclipse MAT
- Chercher les memory leaks : objets non garbage-collectés
- Vérifier les caches mal configurés
- Ajuster Xmx si nécessaire, mais d'abord trouver le leak

**43. Un pod redémarre en boucle, que fais-tu ?**
```bash
kubectl describe pod <pod-name>  # Voir les events
kubectl logs <pod-name> --previous  # Logs du container crashé
kubectl get events --sort-by='.lastTimestamp'
```
- Vérifier OOMKilled : augmenter memory limits
- Vérifier les probes : peut-être trop agressives
- Vérifier les dépendances : DB accessible ?
- Vérifier les variables d'env : secrets bien injectés ?

## **Questions Collaboration et Culture DevOps**

**44. Qu'est-ce que la culture DevOps ?**
- Collaboration étroite entre Dev et Ops
- Automatisation maximale
- Infrastructure as Code
- Monitoring et feedback continu
- Amélioration continue (PDCA)
- Responsabilité partagée : "You build it, you run it"

**45. Comment gérer les incidents en production ?**
- **Incident Response Plan** :
  1. Détection : alertes automatiques
  2. Triage : évaluer la sévérité
  3. Communication : informer stakeholders
  4. Mitigation : rollback ou hotfix
  5. Post-mortem : blameless, documenter, actions correctives
- On-call rotation avec PagerDuty/Opsgenie
- Runbooks pour les incidents connus

**46. Qu'est-ce qu'un post-mortem blameless ?**
- Analyse après incident sans blâmer les individus
- Focus sur les processus et systèmes, pas les personnes
- Questions : Qu'est-ce qui s'est passé ? Pourquoi ? Comment prévenir ?
- Actions concrètes : améliorer monitoring, automatiser, documenter
- Culture d'apprentissage, pas de punition

---

## **Questions Bonus - Outils**

**47. Quels outils DevOps maîtrises-tu ?**
- **CI/CD** : Jenkins, GitLab CI, GitHub Actions, CircleCI
- **Containers** : Docker, Docker Compose, Kubernetes
- **IaC** : Terraform, Ansible
- **Monitoring** : Prometheus, Grafana, ELK, Datadog
- **Cloud** : AWS (EKS, ECS, RDS, S3), Azure, GCP
- **GitOps** : ArgoCD, FluxCD
- **Registries** : Docker Hub, ECR, Harbor, Nexus
- **Security** : Trivy, Snyk, Vault

**48. Quel est ton workflow quotidien en tant que dev DevOps-aware ?**
1. Pull du code, créer une feature branch
2. Développer avec tests unitaires
3. Tester en local avec Docker Compose
4. Push et ouvrir une PR
5. Pipeline CI se déclenche : build, tests, scan
6. Review du code par un pair
7. Merge → déploiement auto sur staging
8. Tests d'intégration sur staging
9. Validation → promotion vers prod
10. Monitoring des métriques post-déploiement

---

## **Conseils pour l'Entretien**

1. **Contextualise** : donne des exemples concrets de tes projets
2. **Parle de problèmes résolus** : "J'ai eu un problème de X, j'ai fait Y"
3. **Montre ta veille techno** : "Je suis la roadmap de K8s, les nouvelles features AWS..."
4. **Sécurité d'abord** : mentionne toujours l'aspect sécurité
5. **Automatisation** : c'est le cœur du DevOps
6. **Monitoring** : "Si on ne mesure pas, on ne peut pas améliorer"
7. **Collaboration** : DevOps c'est aussi une culture d'équipe

**Questions à poser au Tech Lead :**
- Quelle est votre stack DevOps actuelle ?
- Comment gérez-vous les déploiements en production ?
- Quelle est votre stratégie de monitoring/alerting ?
- Utilisez-vous GitOps ? Si oui, quel outil ?
- Comment est organisée l'équipe (Dev/Ops séparés ou DevOps intégré) ?

Bonne chance ! 💪🚀

























<br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br />