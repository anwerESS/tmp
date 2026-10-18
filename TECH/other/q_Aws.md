# Questions AWS pour Cloud Practitioner (Niveau Fondamental)

Je vais te donner des questions basiques AWS qu'un recruteur pourrait poser en voyant ta certification, sans trop approfondir puisque tu n'as pas pratiqué récemment.

## **Questions Générales AWS**

**1. Qu'est-ce qu'AWS et pourquoi l'utiliser ?**
- Amazon Web Services = plateforme cloud leader mondial
- **Avantages** :
  - Pay-as-you-go : payer uniquement ce qu'on utilise
  - Scalabilité : augmenter/réduire les resources facilement
  - Disponibilité globale : data centers partout dans le monde
  - Pas de gestion matérielle : focus sur l'application
  - Sécurité : certifications, compliance
  - Large choix de services : compute, storage, DB, ML, etc.

**2. Explique les Régions et Availability Zones**
- **Région** : zone géographique (ex: eu-west-1 = Irlande, us-east-1 = Virginie)
  - Chaque région est isolée
  - Choisir selon : proximité clients, compliance, coûts
- **Availability Zone (AZ)** : data center isolé dans une région
  - Chaque région a 2+ AZ
  - AZ connectées par réseau bas-latence
  - Déployer sur plusieurs AZ = haute disponibilité
- **Edge Locations** : pour CloudFront CDN (cache proche utilisateurs)

**3. Qu'est-ce que le modèle de responsabilité partagée ?**
- **AWS responsable** : sécurité DU cloud
  - Infrastructure physique, hardware, networking
  - Hyperviseur, data centers
  - Services managés (RDS, S3)
- **Client responsable** : sécurité DANS le cloud
  - Données, chiffrement
  - IAM (users, permissions)
  - Configuration OS, firewall
  - Applications, code
- Résumé : AWS protège l'infra, toi tu protèges ce que tu y mets

## **Questions Compute (EC2)**

**4. Qu'est-ce qu'EC2 ?**
- Elastic Compute Cloud = serveurs virtuels dans le cloud
- Choisir :
  - **Type d'instance** : t2.micro (gratuit tier), t3.medium, etc.
  - **AMI** : Amazon Machine Image (OS + config de base)
  - **Storage** : EBS (disque persistant) ou Instance Store (éphémère)
  - **Security Group** : firewall virtuel
- Use cases : serveurs web, apps, bases de données

**5. Quels sont les types d'instances EC2 et leurs uses cases ?**
- **General Purpose** (t3, m5) : équilibré CPU/RAM, apps web classiques
- **Compute Optimized** (c5) : CPU puissant, calculs intensifs
- **Memory Optimized** (r5) : beaucoup de RAM, bases de données en mémoire
- **Storage Optimized** (i3) : I/O disque élevé, data warehouses
- **GPU** (p3, g4) : machine learning, rendering 3D

**6. Différence entre Stop et Terminate une instance EC2 ?**
- **Stop** : éteindre, données EBS conservées, pas de facturation compute (mais storage oui), peut redémarrer
- **Terminate** : supprimer définitivement, données perdues (sauf EBS avec DeleteOnTermination=false)

**7. Qu'est-ce qu'un Security Group ?**
- Firewall virtuel au niveau de l'instance EC2
- **Stateful** : si on autorise entrant, sortant autorisé automatiquement
- Règles : protocole (TCP/UDP), port, source (IP ou autre SG)
- Par défaut : tout sortant autorisé, tout entrant refusé
```
Règle inbound exemple :
Type: HTTP, Port: 80, Source: 0.0.0.0/0 (tout le monde)
Type: SSH, Port: 22, Source: Mon IP uniquement
```

**8. Qu'est-ce qu'une AMI ?**
- Amazon Machine Image = template pour lancer une instance
- Contient : OS, logiciels pré-installés, configuration
- Types :
  - **AMI AWS** : Amazon Linux, Ubuntu, Windows
  - **AMI Marketplace** : avec softwares pré-configurés
  - **AMI custom** : créer sa propre image depuis une instance existante
- Use case : déployer des serveurs identiques rapidement

## **Questions Storage**

**9. Qu'est-ce que S3 et ses use cases ?**
- Simple Storage Service = stockage d'objets (fichiers)
- **Caractéristiques** :
  - Illimité en taille
  - Durabilité 99.999999999% (11 neuf)
  - Accessible via URL HTTP
  - Versioning possible
- **Use cases** :
  - Backup et archivage
  - Hosting de sites statiques
  - Stockage de médias (images, vidéos)
  - Data lakes pour analytics
- **Structure** : Buckets (conteneur) → Objects (fichiers)

**10. Quelles sont les classes de stockage S3 ?**
- **S3 Standard** : accès fréquent, le plus cher
- **S3 Intelligent-Tiering** : déplace auto entre Standard et IA selon usage
- **S3 Standard-IA** (Infrequent Access) : accès moins fréquent, moins cher
- **S3 One Zone-IA** : une seule AZ, encore moins cher
- **S3 Glacier** : archivage long terme, récupération en minutes/heures
- **S3 Glacier Deep Archive** : archivage très long terme (années), récupération en 12h
- Choisir selon : fréquence d'accès et temps de récupération acceptable

**11. Qu'est-ce qu'EBS ?**
- Elastic Block Store = disques durs virtuels pour EC2
- **Persistant** : données conservées même si instance stoppée
- Attaché à une seule instance à la fois (sauf io2 multi-attach)
- Dans une seule AZ
- Types :
  - **gp3/gp2** : SSD général (boot, apps)
  - **io2/io1** : SSD haute performance (bases de données)
  - **st1** : HDD throughput optimisé (big data)
  - **sc1** : HDD cold storage (accès rare)
- Snapshots : backups incrémentaux stockés dans S3

**12. Différence entre EBS et S3 ?**
- **EBS** : disque pour EC2, accès bloc, une seule instance, une seule AZ
- **S3** : stockage objet, accès HTTP, disponible globalement, partage facile
- **EBS** pour : OS, bases de données
- **S3** pour : fichiers, backups, médias

**13. Qu'est-ce qu'EFS ?**
- Elastic File System = NFS managé dans le cloud
- **Partageable** entre plusieurs instances EC2 simultanément
- Multi-AZ automatique
- Élastique : croît/décroît automatiquement
- Use case : content management, dev environments partagés

## **Questions Database**

**14. Qu'est-ce que RDS ?**
- Relational Database Service = bases de données SQL managées
- **Moteurs supportés** : MySQL, PostgreSQL, MariaDB, Oracle, SQL Server, Aurora
- **AWS gère** : backups, patches, haute dispo, réplication
- **Tu gères** : schéma, requêtes, tuning
- **Multi-AZ** : failover automatique pour HA
- **Read Replicas** : copies en lecture pour scaler

**15. Différence entre RDS et DynamoDB ?**
- **RDS** :
  - SQL relationnel
  - Schéma fixe, tables, relations
  - ACID transactions
  - Vertical scaling principalement
- **DynamoDB** :
  - NoSQL key-value/document
  - Schéma flexible
  - Eventual consistency (ou strongly consistent reads)
  - Horizontal scaling automatique, serverless
  - Ultra-rapide, millisecondes latency

**16. Qu'est-ce qu'Aurora ?**
- Base de données relationnelle propriétaire AWS
- Compatible MySQL et PostgreSQL
- 5x plus rapide que MySQL, 3x plus rapide que PostgreSQL
- Stockage automatique croît jusqu'à 128 TB
- 6 copies des données sur 3 AZ
- Auto-healing, backups continus vers S3

**17. Qu'est-ce qu'ElastiCache ?**
- Cache en mémoire managé
- **Redis** : structures de données riches, persistence, pub/sub
- **Memcached** : simple, multi-threaded
- Use cases : sessions utilisateurs, cache de requêtes DB, leaderboards
- Réduit latency et charge sur la DB

## **Questions Networking**

**18. Qu'est-ce qu'un VPC ?**
- Virtual Private Cloud = réseau privé virtuel dans AWS
- **Composants** :
  - **Subnets** : découpage du VPC (public/private)
  - **Internet Gateway** : accès Internet pour subnet public
  - **NAT Gateway** : permet aux instances privées d'accéder à Internet (sortant uniquement)
  - **Route Tables** : règles de routage
  - **Security Groups** : firewall instances
  - **NACLs** : firewall subnets (stateless)

**19. Différence entre subnet public et private ?**
- **Public Subnet** :
  - Route vers Internet Gateway
  - Instances ont des IP publiques
  - Accessible depuis Internet (si SG autorise)
  - Use case : serveurs web, load balancers
- **Private Subnet** :
  - Pas de route directe vers Internet
  - Pas d'IP publique
  - Accès Internet via NAT Gateway
  - Use case : bases de données, app servers

**20. Qu'est-ce qu'un Load Balancer AWS ?**
- Distribue le trafic entre plusieurs instances
- **ALB** (Application Load Balancer) :
  - HTTP/HTTPS (Layer 7)
  - Routing avancé (path, hostname)
  - Pour applications web
- **NLB** (Network Load Balancer) :
  - TCP/UDP (Layer 4)
  - Ultra performance, low latency
  - Pour applications non-HTTP
- **CLB** (Classic) : ancien, déprécié
- Health checks : retire automatiquement les instances malades

**21. Qu'est-ce que Route 53 ?**
- Service DNS managé par AWS
- Enregistrer et gérer des noms de domaine
- Routing policies :
  - **Simple** : un seul endpoint
  - **Weighted** : répartir le trafic (A/B testing)
  - **Latency** : route vers la région la plus proche
  - **Failover** : bascule si primary down
  - **Geolocation** : route selon localisation utilisateur

**22. Qu'est-ce que CloudFront ?**
- CDN (Content Delivery Network) global
- Cache le contenu dans les Edge Locations (200+ dans le monde)
- Réduit latency pour les utilisateurs
- Use cases :
  - Servir des sites web statiques (S3)
  - Streaming vidéo
  - API acceleration
- Intégration avec S3, ALB, EC2

## **Questions IAM et Sécurité**

**23. Qu'est-ce qu'IAM ?**
- Identity and Access Management
- Gérer l'accès aux ressources AWS
- **Composants** :
  - **Users** : personnes physiques
  - **Groups** : ensemble d'users
  - **Roles** : pour services AWS ou apps
  - **Policies** : permissions en JSON
- Best practices :
  - Pas utiliser root account
  - MFA activé
  - Principe du moindre privilège
  - Rotation des credentials

**24. Différence entre IAM User et IAM Role ?**
- **User** :
  - Pour les humains (ou apps long-terme)
  - Credentials permanents (password, access keys)
  - Associé à une personne
- **Role** :
  - Pour services AWS ou accès temporaire
  - Credentials temporaires (assume role)
  - Pas de password ni access keys
- Use case Role : EC2 accède à S3, Lambda accède à DynamoDB

**25. Qu'est-ce qu'une Policy IAM ?**
- Document JSON définissant les permissions
```json
{
  "Version": "2012-10-17",
  "Statement": [{
    "Effect": "Allow",
    "Action": "s3:GetObject",
    "Resource": "arn:aws:s3:::my-bucket/*"
  }]
}
```
- **Effect** : Allow ou Deny
- **Action** : opérations autorisées (s3:*, ec2:StartInstance)
- **Resource** : sur quelles ressources (ARN)
- Types : AWS Managed, Customer Managed, Inline

**26. Qu'est-ce que MFA ?**
- Multi-Factor Authentication
- Double authentification : password + code temporaire
- Types :
  - Virtual MFA (Google Authenticator, Authy)
  - Hardware MFA (clé physique)
  - SMS (moins sécurisé)
- Obligatoire pour root account et users admin

**27. Qu'est-ce que AWS Organizations ?**
- Gestion centralisée de plusieurs comptes AWS
- **Organization Units (OU)** : groupes de comptes (par env, par équipe)
- **Service Control Policies (SCP)** : restrictions au niveau OU
- **Consolidated Billing** : facturation centralisée, volume discounts
- Use case : entreprise avec plusieurs projets/équipes

## **Questions Monitoring et Management**

**28. Qu'est-ce que CloudWatch ?**
- Service de monitoring AWS
- **Metrics** : CPU, RAM, disk, network des instances
- **Logs** : centraliser les logs des applications
- **Alarms** : alertes si métrique dépasse un seuil
- **Events/EventBridge** : automatiser actions (démarrer instance, Lambda)
- **Dashboards** : visualisation des métriques
- Use case : surveiller la santé des ressources

**29. Qu'est-ce que CloudTrail ?**
- Audit trail : enregistre toutes les API calls AWS
- **Qui** a fait **quoi**, **quand**, depuis **où**
- Logs stockés dans S3
- Use cases :
  - Compliance et audit
  - Troubleshooting (qui a supprimé cette instance ?)
  - Détection d'activités suspectes
- Toujours activer dans tous les comptes

**30. Qu'est-ce qu'AWS Config ?**
- Évalue la configuration des ressources AWS
- Vérifie la conformité aux règles (compliance)
- Historique des changements de configuration
- Exemples de règles :
  - Les buckets S3 sont-ils chiffrés ?
  - Les Security Groups autorisent-ils SSH depuis 0.0.0.0/0 ?
  - Les instances ont-elles des backups ?

**31. Qu'est-ce que Trusted Advisor ?**
- Recommandations automatiques pour optimiser AWS
- 5 catégories :
  - **Cost Optimization** : réduire les coûts (instances idle)
  - **Performance** : améliorer perfs
  - **Security** : fermer les failles (ports ouverts)
  - **Fault Tolerance** : améliorer disponibilité
  - **Service Limits** : alertes si proche des limites
- Gratuit (basique) ou complet (Business/Enterprise support)

## **Questions Autres Services**

**32. Qu'est-ce que Lambda ?**
- Serverless compute : exécuter du code sans gérer de serveurs
- Pay-per-execution (par milliseconde)
- Supporte : Python, Node.js, Java, Go, .NET, etc.
- Déclencheurs : API Gateway, S3 events, DynamoDB streams, CloudWatch
- Use cases :
  - APIs REST (avec API Gateway)
  - Traitement de fichiers uploadés sur S3
  - Automatisation de tâches
  - Backends légers

**33. Qu'est-ce qu'ECS vs EKS ?**
- **ECS** (Elastic Container Service) :
  - Orchestration de containers AWS-natif
  - Plus simple, intégration AWS native
  - Fargate : serverless containers
- **EKS** (Elastic Kubernetes Service) :
  - Kubernetes managé sur AWS
  - Standard open-source
  - Plus flexible, portable multi-cloud
  - Tu le connais déjà avec ta certif CKAD !

**34. Qu'est-ce qu'API Gateway ?**
- Service pour créer et gérer des APIs REST/WebSocket
- Frontend pour Lambda, EC2, autres services
- Fonctionnalités :
  - Authentification (IAM, Cognito, JWT)
  - Rate limiting, throttling
  - Caching des réponses
  - Versioning des APIs
- Use case : exposer Lambda comme API REST

**35. Qu'est-ce qu'SNS ?**
- Simple Notification Service = pub/sub messaging
- **Publisher** envoie message vers **Topic**
- **Subscribers** reçoivent le message (email, SMS, HTTP, Lambda, SQS)
- Use case : alertes, notifications, fan-out pattern

**36. Qu'est-ce que SQS ?**
- Simple Queue Service = file d'attente de messages
- Découplage entre services
- **Standard** : ordre non garanti, at-least-once delivery
- **FIFO** : ordre garanti, exactly-once delivery
- Use cases :
  - Traitement asynchrone (upload image → queue → Lambda resize)
  - Buffer entre services (pic de charge)

**37. Différence entre SNS et SQS ?**
- **SNS** : push, pub/sub, un message vers plusieurs subscribers, temps réel
- **SQS** : pull, queue, un message vers un consumer, peut attendre
- **Combo** : SNS → plusieurs SQS (fan-out)

**38. Qu'est-ce que CloudFormation ?**
- Infrastructure as Code pour AWS
- Templates YAML/JSON déclaratifs
- Provisionne et gère les ressources AWS
- Stack = ensemble de ressources créées ensemble
- Alternative AWS-native à Terraform
```yaml
Resources:
  MyBucket:
    Type: AWS::S3::Bucket
    Properties:
      BucketName: my-awesome-bucket
```

**39. Qu'est-ce que Elastic Beanstalk ?**
- PaaS (Platform as a Service) pour déployer des apps
- Tu uploades ton code, Beanstalk gère :
  - Provisioning EC2, Load Balancer, Auto Scaling
  - Déploiement, monitoring
  - Updates, patches
- Supporte : Java, .NET, Node.js, Python, Ruby, Go, Docker
- Use case : déployer rapidement sans gérer l'infra

**40. Qu'est-ce que Systems Manager (SSM) ?**
- Gestion centralisée des instances EC2 et on-premise
- **Parameter Store** : stocker configs et secrets
- **Session Manager** : SSH sans ouvrir le port 22
- **Patch Manager** : automatiser les patches OS
- **Run Command** : exécuter des commandes sur plusieurs instances

## **Questions Coûts et Facturation**

**41. Comment optimiser les coûts AWS ?**
- **Right-sizing** : adapter taille des instances à l'usage réel
- **Reserved Instances** : engagement 1-3 ans pour -75% de coût
- **Savings Plans** : flexible, -72% de coût
- **Spot Instances** : instances interruptibles, -90% de coût
- **Auto Scaling** : ajuster capacité selon la demande
- **S3 Intelligent-Tiering** : optimisation auto du storage
- **Supprimer ressources inutilisées** : snapshots anciens, volumes non attachés
- **Budget Alarms** : alertes si dépassement

**42. Qu'est-ce que le Free Tier AWS ?**
- Offre gratuite pour découvrir AWS
- **12 mois** : EC2 t2.micro 750h/mois, RDS, ELB, etc.
- **Toujours gratuit** : Lambda 1M requests/mois, DynamoDB 25 GB, SNS, etc.
- **Trials** : services avec périodes d'essai (SageMaker, etc.)
- Attention aux dépassements !

**43. Qu'est-ce que Cost Explorer ?**
- Outil de visualisation des coûts AWS
- Graphiques interactifs
- Filtrer par : service, région, tag, account
- Forecasting : prédire les coûts futurs
- Identifier les ressources coûteuses

**44. Qu'est-ce que les Tags AWS ?**
- Métadonnées key-value sur les ressources
- Exemples : `Environment: Production`, `Team: Backend`, `CostCenter: Marketing`
- Use cases :
  - Organisation des ressources
  - Allocation des coûts par projet/équipe
  - Automation (backup selon tag)
  - Billing reports détaillés

## **Questions Support et Well-Architected**

**45. Quels sont les plans de support AWS ?**
- **Basic** : gratuit, documentation, forums
- **Developer** : $29/mois, support email, business hours
- **Business** : $100+/mois, support 24/7, Trusted Advisor complet, < 1h pour prod down
- **Enterprise** : $15,000+/mois, Technical Account Manager, < 15min pour critical
- Choisir selon criticité et budget

**46. Qu'est-ce que le AWS Well-Architected Framework ?**
5 piliers pour construire des systèmes cloud robustes :
1. **Operational Excellence** : automatisation, monitoring, amélioration continue
2. **Security** : IAM, chiffrement, defense in depth
3. **Reliability** : haute dispo, disaster recovery, auto-scaling
4. **Performance Efficiency** : right-sizing, serverless, caching
5. **Cost Optimization** : pay-as-you-go, Reserved Instances

**47. Qu'est-ce qu'un DR (Disaster Recovery) plan ?**
Stratégies selon le RTO (Recovery Time Objective) et RPO (Recovery Point Objective) :
- **Backup & Restore** : backups réguliers, restore en cas de disaster (RTO: heures)
- **Pilot Light** : minimal version en standby, scale up si nécessaire (RTO: ~10min)
- **Warm Standby** : version réduite toujours running (RTO: minutes)
- **Multi-Site Active/Active** : full capacity en parallèle (RTO: secondes)

## **Questions Concepts Généraux Cloud**

**48. Qu'est-ce que l'élasticité dans le cloud ?**
- Capacité à scale UP ou DOWN automatiquement selon la demande
- Scale out : ajouter des instances (horizontal)
- Scale up : augmenter taille instance (vertical)
- Payer uniquement ce qu'on utilise
- AWS Auto Scaling gère ça automatiquement

**49. Différence entre CapEx et OpEx ?**
- **CapEx** (Capital Expenditure) :
  - Investissement initial lourd (acheter des serveurs)
  - Amortissement sur plusieurs années
  - Risque de sur/sous-provisioning
- **OpEx** (Operating Expenditure) :
  - Dépenses opérationnelles récurrentes (louer le cloud)
  - Pay-as-you-go
  - Flexibilité, pas de gaspillage
- Cloud = passage de CapEx à OpEx

**50. Qu'est-ce que le principe de "Design for Failure" ?**
- Assumer que tout peut tomber (instance, AZ, région)
- Architecture résiliente :
  - Multi-AZ deployments
  - Load balancing
  - Auto Scaling
  - Backups automatiques
  - Health checks et auto-healing
- Chaos Engineering : tester les pannes volontairement

---

## **Questions Rapides (One-liners)**

**51. Qu'est-ce qu'un ARN ?** Amazon Resource Name, identifiant unique d'une ressource AWS

**52. Qu'est-ce que S3 Bucket Policy ?** Permissions au niveau bucket (qui peut accéder au bucket)

**53. Qu'est-ce que CORS dans S3 ?** Cross-Origin Resource Sharing, permet accès depuis un autre domaine

**54. Qu'est-ce qu'un Snapshot EBS ?** Backup incrémental d'un volume EBS stocké dans S3

**55. Qu'est-ce qu'un Elastic IP ?** IP publique statique qu'on peut associer/désassocier d'instances

**56. Qu'est-ce que STS ?** Security Token Service, génère credentials temporaires

**57. Qu'est-ce que KMS ?** Key Management Service, gestion des clés de chiffrement

**58. Qu'est-ce que Secrets Manager ?** Stockage sécurisé de secrets avec rotation automatique

**59. Qu'est-ce que GuardDuty ?** Détection de menaces et activités malveillantes

**60. Qu'est-ce que AWS Shield ?** Protection DDoS (Standard gratuit, Advanced payant)

**61. Qu'est-ce que WAF ?** Web Application Firewall, protection contre OWASP Top 10

**62. Qu'est-ce qu'AWS Backup ?** Service de backup centralisé pour plusieurs services AWS

**63. Qu'est-ce que Transfer Family ?** Transfert de fichiers SFTP/FTPS vers S3

**64. Qu'est-ce que WorkSpaces ?** VDI (Virtual Desktop Infrastructure) dans le cloud

**65. Qu'est-ce que Lightsail ?** VPS simple et pas cher pour débuter (alternative à EC2)

---

## **Stratégie pour l'Entretien AWS**

**Si le recruteur approfondit et que tu ne sais pas :**

✅ **Réponse honnête** :
> "J'ai obtenu la certification Cloud Practitioner il y a [date], donc j'ai les fondamentaux AWS. Dans mon poste actuel, on utilise [autre cloud/on-premise], donc je n'ai pas pratiqué AWS quotidiennement. Mais je connais bien les concepts cloud grâce à Kubernetes, et je suis capable de monter en compétence rapidement sur AWS."

✅ **Rediriger vers tes forces** :
> "Par contre, je maîtrise très bien Docker et Kubernetes, et j'ai ma certification CKAD. Je suis confiant que mes compétences en orchestration de containers se transfèrent facilement vers EKS."

✅ **Montrer ta motivation** :
> "Si le poste nécessite AWS, je suis prêt à me former rapidement. J'ai déjà les fondamentaux avec ma certification, il me suffit de pratiquer en conditions réelles."

**Questions à poser au recruteur :**
- Utilisez-vous AWS dans votre stack actuelle ?
- Si oui, quels services AWS utilisez-vous principalement ?
- Y a-t-il des formations AWS prévues pour les nouveaux arrivants ?
- Travaillez-vous avec EKS ou un autre service de containers ?

**Astuce** : Mets en avant que Kubernetes (CKAD) est **portable** et fonctionne sur AWS (EKS), Azure (AKS), GCP (GKE). Ta certif K8s a plus de valeur long-terme !

Bon courage ! 💪☁️































<br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br />