Worked for 34s

Ce ticket Jira SQU1-1275 — “[PROD] Implement Alerts on data transformer in elasticsearch” demande de détecter automatiquement les anomalies de traitement en production, pour éviter de parcourir de longs logs chaque jour.

Le problème central est le suivant : si aucun document n’est traité, comment savoir s’il n’y a rien de nouveau dans MyAudit ou si le traitement est bloqué ?

1. Ce que demande le ticket

La description indique que :

- vous n’avez pas accès à la base de données de production ;
- vous ne savez pas facilement si de nouveaux documents sont disponibles dans MyAudit ;
- les logs sont trop longs pour une vérification quotidienne ;
- une alerte, suivie d’une vérification avec le métier, aiderait à détecter ces anomalies.

Il s’agit donc de mettre en place une surveillance du fonctionnement du flux, avec des notifications utiles. Le ticket ne précise toutefois ni l’anomalie initiale exacte, ni les seuils, ni les destinataires.

2. Ce que j’ai retrouvé dans notre contexte précédent

Les éléments retrouvés dans les documents de contexte SQUID décrivent ce flux :

Export CSV MyAudit → dépôt S3 → notification HTTP à l’Ingestion API → événement Redis → Data Transformer → traitement Bronze / Silver / Gold.

Les PDF suivent un autre circuit, avec extraction par le RPA puis parsing, découpage et embeddings. Il faut donc identifier si le ticket concerne les données MyAudit, les PDF, ou les deux.

Cela suggère que Data Transformer est ici un composant applicatif. Ce n’est pas nécessairement la fonctionnalité native Transform d’Elasticsearch.

Autre point utile : des outils de monitoring avaient déjà été identifiés — Absana, Cocoon et Grafana pour Redis. Il faudra vérifier leurs informations disponibles pour éviter de reconstruire une surveillance existante.

3. Quelles alertes mettre en place ?

La règle « aucun document traité depuis X heures » ne suffit pas : cela peut être normal s’il n’y a pas de nouvelles données.

Je proposerais plutôt ces contrôles :

Situation	Signal à surveiller	Interprétation

Un traitement échoue	Erreur explicite, exception ou événement d’échec	Incident technique
Un traitement démarre sans terminer	Début sans fin pour un même identifiant de traitement, au-delà du délai normal	Traitement potentiellement bloqué
Des données attendent sans être consommées	Ancienneté des messages ou éléments en attente dans Redis	Retard du consommateur
Un export attendu n’arrive pas	Absence de réception après l’échéance prévue	Problème possible en amont
Aucun signe de vie n’est reçu	Absence d’un événement périodique prévu	Service arrêté ou collecte de logs interrompue
Le traitement se termine partiellement	Écart inexpliqué entre entrées, succès, rejets et éléments restants	Anomalie de traitement


Les délais doivent être définis à partir du fonctionnement réel et validés avec le métier. Les horaires retrouvés dans le contexte précédent étaient illustratifs, pas des horaires PROD confirmés.

Sans signal sur les données attendues ou reçues, on peut détecter une absence d’activité, mais pas conclure qu’un document a été perdu.

4. Les solutions techniques possibles

Option A — Utiliser les logs existants dans Kibana

C’est le meilleur point de départ si les logs permettent déjà d’identifier les erreurs et les traitements.

Une règle Elasticsearch query peut exécuter régulièrement une recherche, comparer les résultats à un seuil et déclencher une notification via un connecteur. Elle peut aussi notifier le retour à la normale. La disponibilité exacte dépend de votre version, de vos droits et des connecteurs autorisés. 

Exemple de filtre KQL, à adapter aux vrais champs et valeurs :

service.name: "data-transformer"
and service.environment: "production"
and log.level: "ERROR"

Premier cas simple : déclencher une alerte si au moins une erreur de traitement apparaît dans la fenêtre surveillée.

En revanche, détecter « un début sans fin » demande de corréler les événements ; une simple recherche sur les erreurs ne le couvre pas.

Option B — Ajouter des événements de suivi dans le Data Transformer

Si les logs ne suffisent pas, faire produire au service des événements structurés contenant :

- un identifiant de traitement commun, par exemple run_id ;
- l’étape exécutée et son statut : début, succès, échec ;
- les dates de début et de fin ;
- les nombres d’éléments reçus, traités et rejetés ;
- la cause de l’échec.

Cela permet de savoir où le flux s’arrête, sans accès direct à la DB PROD. Pour repérer les traitements bloqués, on peut maintenir un état par traitement, puis surveiller les traitements encore ouverts et trop anciens.

Option C — Réutiliser le monitoring existant

Si Grafana expose déjà l’attente Redis ou si Cocoon suit les exécutions, utiliser ces signaux pour compléter les alertes sur les logs. Vérifier d’abord ce que ces outils mesurent réellement.

Cas particulier — S’il s’agit d’un Transform natif Elasticsearch

Dans ce cas, Elastic propose une règle Transform health pour surveiller les transforms continus, notamment leur arrêt ou leur état dégradé. Cette solution ne surveille pas automatiquement un service applicatif portant le nom « Data Transformer ». 

5. La démarche concrète que je te conseille

1. Faire préciser l’incident à détecter. Demander un exemple réel : date, document ou lot concerné, comportement attendu et résultat observé.
2. Retrouver le bon périmètre dans Kibana. Index ou data stream PROD, nom du service, champs disponibles et exemples de traitement réussi et échoué.
3. Vérifier les signaux d’entrée. Peut-on voir qu’un CSV est arrivé, qu’une notification a été reçue et qu’un message Redis attend ?
4. Définir deux ou trois alertes initiales. Erreur de traitement, traitement trop long et retard de consommation si le signal existe.
5. Compléter les logs uniquement si nécessaire. Ajouter les identifiants, événements de fin ou compteurs manquants.
6. Tester en préproduction. Vérifier un succès, un échec, un blocage et une période normale sans nouvelles données.
7. Documenter la réaction attendue. Qui reçoit l’alerte, quel lien Kibana ouvrir, quoi vérifier et quand contacter le métier.

L’alerte doit donner au minimum l’environnement, le traitement concerné, l’anomalie, l’heure et un lien vers les logs. Prévoir également une limitation des notifications répétées.

Pour ce ticket, je commencerais par les logs Kibana existants et les signaux Redis disponibles. Ensuite, j’ajouterais le suivi par traitement si les logs actuels ne permettent pas de distinguer « rien à traiter » de « traitement bloqué ».