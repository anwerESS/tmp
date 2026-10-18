********* Questions Fondamentales *********

1. Explique-moi l'architecture de Kubernetes
  - Control Plane : API Server (point d'entrée), etcd (stockage clé-valeur), Scheduler (placement des pods), Controller Manager (gère l'état désiré)
  - Worker Nodes : Kubelet (agent sur chaque nœud), Kube-proxy (gestion réseau), Container Runtime (Docker/containerd)
  ==> Le Control Plane décide QUOI faire, les Workers exécutent (https://pek3b.qingstor.com/kubesphere-docs/png/20200328170549.png)

2. Quelle est la différence entre un Pod et un Container ?
  - Un Pod est la plus petite unité déployable dans K8s
  - Un Pod peut contenir un ou plusieurs containers qui partagent le même network namespace et volumes
  - Les containers d'un même Pod communiquent via localhost
  - Exemple : un container applicatif + un sidecar pour les logs

3. Qu'est-ce qu'un Namespace et pourquoi l'utiliser ?
  - Isolation logique des ressources dans un cluster
  - Permet la séparation par environnement (dev, staging, prod) ou par équipe
  - Chaque namespace a ses propres quotas, RBAC, network policies
  - Les namespaces kube-system, kube-public, default sont créés par défaut

********* Questions sur les Workloads *********

4. Différence entre Deployment, StatefulSet et DaemonSet ?
  - Deployment : pour applications stateless, scaling horizontal, rolling updates
  - StatefulSet : pour applications stateful (bases de données), identité réseau stable, ordre de démarrage garanti, persistent volumes
  - DaemonSet : pour garantir un pod par nœud (monitoring agents, log collectors)
  - Job/CronJob : tâches ponctuelles(migration d'une BDD, traitment d'un fichier) ou planifiées (Backup, clean log)

5. Comment fais-tu un rolling update sans downtime ?
  - Utiliser une stratégie RollingUpdate dans le Deployment
  - Configurer maxSurge (pods supplémentaires temporaires) et maxUnavailable
  - Définir des readinessProbe pour vérifier que le nouveau pod est prêt
  - K8s arrête progressivement les anciens pods une fois les nouveaux ready

6. Qu'est-ce qu'une readinessProbe vs livenessProbe ?
  - livenessProbe : vérifie si le container est vivant. Si échec → restart
  - readinessProbe : vérifie si le pod est prêt à recevoir du trafic. Si échec → retiré du Service
  - startupProbe : pour les apps avec démarrage lent, désactive les autres probes au début
  - Types : HTTP GET, TCP Socket, exec command

********* Questions Réseau *********

7. Comment fonctionne un Service dans Kubernetes ?
  il expose un ensemble de Pods via un DNS stable
  ClusterIP : accessible uniquement dans le cluster (par défaut)
  NodePort : expose sur un port de chaque nœud (30000-32767)
  LoadBalancer : provisionne un LB externe (cloud provider)
  ExternalName : mapping DNS vers un service externe

8. Explique Ingress vs Service
  - Service : L4 (TCP/UDP) expose les pods rn interne (ClusterIP) ou externe (NodePort/LoadBalancer)
  - Ingress : HTTP/HTTPS L7, permet le routage hostname/path vers servcie et SSL termination
  - Nécessite un Ingress Controller (nginx, traefik, etc.)

9. Qu'est-ce que les NetworkPolicies ?
  - Firewall au niveau des Pods pour contrôler le trafic entrant/sortant
  - Par défaut, tout est autorisé
  - Sélection via labels, règles par namespace, CIDR, ports

********* Questions Storage *********

10. Différence entre PersistentVolume et PersistentVolumeClaim ?
  - PV : ressource de stockage provisionnée par l'admin (NFS, AWS EBS, etc.)
  - PVC : demande de stockage par l'utilisateur (taille, accessMode)
  - Le PVC se "bind" à un PV compatible
  - StorageClass : provisionnement dynamique des PV

11. Quels sont les accessModes d'un volume ?
  - ReadWriteOnce (RWO) : monté en lecture-écriture sur un seul nœud
  - ReadOnlyMany (ROX) : monté en lecture seule sur plusieurs nœuds
  - ReadWriteMany (RWX) : monté en lecture-écriture sur plusieurs nœuds (NFS, CephFS)

********* Questions ConfigMap et Secrets *********

12. Comment injecter de la config dans un Pod ?
  - ConfigMap : pour config non sensible (variables d'env, fichiers de config)
  - Secret : pour données sensibles (passwords, tokens, certificats)
  - Injection via : env variables, volumes montés
  - Les Secrets sont encodés base64 (pas chiffrés par défaut !)

13. Comment sécuriser les Secrets ?
  - Activer l'encryption at rest dans etcd
  - Utiliser des solutions externes : Vault, AWS Secrets Manager
  - Ne jamais commiter les secrets en clair dans Git

********* Questions RBAC et Sécurité *********

14. Explique le RBAC  (Role-Based Access Control) dans Kubernetes
  - Role/ClusterRole : définit les permissions (verbs: get, list, create, delete sur resources)
  - RoleBinding/ClusterRoleBinding : lie un Role à un User/Group/ServiceAccount
  - Role est namespace-scopé, ClusterRole est cluster-wide

15. Qu'est-ce qu'un ServiceAccount ?
  - Identité pour les Pods (pas pour les humains)
  - Chaque namespace a un SA `default`
  - Permet aux Pods d'interagir avec l'API K8s
  - Token JWT monté automatiquement dans `/var/run/secrets/kubernetes.io/serviceaccount/token`

16. Qu'est-ce qu'un SecurityContext ?
  - Définit les privilèges et contrôles d'accès pour un Pod/Container
  - runAsUser, runAsGroup, fsGroup (gestion des UID/GID)
  - capabilities (ajouter/supprimer des capabilities Linux)


********* Questions Avancées *********

17. Comment debugger un Pod qui crashloop ?
  $ kubectl describe pod <pod-name>  # Events et status
  $ kubectl logs <pod-name> --previous  # Logs du container précédent
  $ kubectl get events --sort-by='.lastTimestamp'
  $ kubectl exec -it <pod> -- sh  # Si le pod démarre
  # Vérifier les resources limits, probes mal configurées, dépendances manquantes

18. Qu'est-ce qu'un HorizontalPodAutoscaler ?
  - Scale automatiquement le nombre de replicas basé sur des métriques
  - CPU, mémoire (metrics-server requis)
  - minReplicas, maxReplicas, targetCPUUtilizationPercentage

19. Comment gérer les ressources (requests/limits) ?
  `requests` : minimum garanti pour scheduling
  `limits` : maximum autorisé (throttling CPU, OOMKill si dépassement mémoire)
  Définir des LimitRanges (set default min/max par container) et ResourceQuotas (namespace entier) par namespace

20. Qu'est-ce qu'un Init Container ?
  - Containers qui s'exécutent AVANT les containers principaux
  - S'exécutent séquentiellement (un après l'autre)
  - Utile pour : préparer des données, attendre des dépendances
  - Si un init container échoue, le Pod redémarre

21. Différence entre kubectl apply et kubectl create ?
  `create` : impératif, crée la ressource (erreur si existe déjà)
  `apply` : déclaratif, crée ou met à jour

22. Qu'est-ce que Helm ?
  - Package manager pour K8s
  - Charts Helm = templates paramétrables
  - Gestion des versions, rollback facile
  - Values.yaml pour la personnalisation
  - Alternative : Kustomize (intégré dans kubectl)

23. Comment faire un blue-green deployment ?
  - Déployer la nouvelle version (green) à côté de l'ancienne (blue)
  - Tester la version green
  - Changer le Service selector pour pointer vers green
  - Garder blue en standby puis supprimer

24. Stratégies de déploiement Canary ?
  - Déployer une petite portion de trafic sur la nouvelle version
  - Augmenter progressivement ou rollback

********* Questions Architecture et Production *********

ْ25. Comment assurer la haute disponibilité ?
  Worker Nodes : multi-AZ, autoscaling
  Applications : replicas multiples

26. Comment gérer les secrets d'application dans un pipeline CI/CD ?
  - External Secrets Operator (sync depuis Vault, AWS SM, etc.)
  - Sealed Secrets : chiffre les secrets pour Git
  - SOPS : chiffrement de fichiers YAML avec KMS
  - Injection au runtime depuis le secret manager

27. Expérience avec Spring Boot sur K8s : comment optimiser ?
  - Actuator pour les health checks (/actuator/health)
  - Readiness probe sur /actuator/health/readiness
  - Liveness probe sur /actuator/health/liveness
  - Graceful shutdown configuré
  - Resources requests/limits adaptés à la JVM
  - Utiliser un 'private registry image'
  - Multi-stage Docker builds pour optimiser la taille

********* Questions Pièges / Avancées *********

28. Que ce qu'un DaemonSet ?
  Un DaemonSet garantit qu’une copie d’un Pod tourne sur tous les nœuds (Il est principalement utilisé pour des agents systèmes ou des outils de monitoring/logging, qui doivent tourner partout EXEMPLE: Filebeat).


























<br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br />