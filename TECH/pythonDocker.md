# SQUID AI — Référence projet CA-CIB / IGE IT

Version 1 — Analyse documentaire du 23 octobre 2026.

## Utilisation et niveau de preuve

Référence destinée à Anouar pour comprendre SQUID AI, préparer les échanges avec IGE IT et AIF, puis poser des questions de maintenance, de fonctionnement ou d'évolution. Relire ce fichier dans les sessions ultérieures plutôt que supposer que tous les détails sont en mémoire. Ce fichier est une référence persistante, pas une modification de la mémoire interne du modèle.

Corpus : 21 photographies et 6 documents Markdown OCR fournis dans la conversation. Toutes les images ont été inspectées et les six documents lus. Aucun accès au code source, à MyAudit, à la production, à LeanIX ou aux dépôts internes n'a été effectué. Les documents ne prouvent pas l'état actuellement déployé. Plusieurs photos se recouvrent ; plusieurs schémas portent la mention MVP1.

Convention :
- **Documenté** : visible dans les pièces, sans garantie de conformité au code actuel.
- **Interprétation** : explication ou conséquence logique des pièces.
- **À confirmer** : ambiguïté, contradiction ou information absente.
- Les exemples métier sont fictifs et ne constituent pas des résultats réels de CA-CIB.

Sources principales : DAD = Squid_AI_DAD_OCR(1).md ; ARCH = arch-diagram ; DATA = myAudiDataIngestion ; RPA = RPA_interface. L'inventaire précis et les transcriptions originales figurent en annexe.

## 1. Identité et finalité

Le DAD nomme l'application « My Audit Intelligent Search », code MAIS, business line IGE, IT line AIF, sponsor IGE Audit Team. Le nom courant est SQUID AI. L'utilisateur situe sa mission dans le périmètre IGE IT. Le nom développé institutionnel exact d'IGE est à confirmer : le DAD utilise « Internal General Evaluation », formulation à ne pas reprendre comme traduction officielle.

SQUID AI est une plateforme de recherche intelligente et d'assistance à l'analyse pour les auditeurs. Elle exploite des rapports d'audit, des missions, des constats, des recommandations et des informations de suivi issus de MyAudit. Elle associe recherche sémantique documentaire et exploitation des relations entre données métier.

MyAudit apparaît comme le système source. SQUID AI extrait et transforme une représentation de ses données pour les interroger. Aucun flux de création, clôture ou modification d'une recommandation dans MyAudit depuis le chatbot n'est démontré. Il ne faut donc pas présenter SQUID comme le remplaçant du système de gestion des audits.

Valeur métier attendue : retrouver les précédents pertinents, préparer une mission, comprendre les constats passés, retrouver des recommandations et produire des réponses contextualisées avec références. Les gains de temps sont attendus mais non mesurés dans le corpus.

## 2. Le métier, expliqué simplement

Dans un contexte d'audit interne, une équipe examine une activité, une organisation ou un processus, apprécie les risques et les contrôles, consigne ses constats, formule des recommandations et suit leur mise en œuvre. Cette description est une explication métier générale ; le workflow exact de validation IGE n'est pas fourni.

| Terme | Sens métier dans l'analyse | Preuve dans le corpus |
|---|---|---|
| Assignment | Mission d'audit | assignments, référence, nom, statut, dates, résumé, niveau de risque |
| Entity | Entité organisationnelle concernée | entities, hiérarchie, pays, zone |
| Zone | Périmètre d'audit, avec hiérarchie ; peut être géographique | zones et exemple Americas |
| Finding | Constat de l'auditeur | finding_id, finding_text, finding_facts, finding_consequences |
| Recommendation | Action/amélioration recommandée en réponse aux constats | recommendation_id, texte, notation, responsable, statut |
| Follow-up | Suivi des recommandations | nombre de suivis validés, réponse du responsable, jalons, échéances, date de mise en œuvre |
| Final report | Rapport final d'une mission | PDF, attachments, sélection des rapports finaux |
| Permission | Droit d'un utilisateur sur une mission ou une recommandation | permissions et partial_rights |

Une mission peut concerner plusieurs entités ; une entité peut être concernée par plusieurs missions. Une mission est associée à des pièces jointes et à des recommandations. Les recommandations contiennent des informations sur les constats, risques, causes, responsables et suivis. La présence d'un finding_id ne prouve pas l'existence d'une table findings autonome : les champs visibles sont dans recommendations.

Exemple fictif : une mission examine la gestion des habilitations d'une entité. Le constat est que certains anciens collaborateurs conservent des accès. La recommandation est d'améliorer le retrait des droits et les contrôles périodiques. Le responsable répond, une échéance est fixée, puis la réalisation fait l'objet d'un suivi. Le rapport final donne le contexte et les éléments détaillés.

SQUID peut servir à chercher des cas similaires, retrouver les missions concernées ou synthétiser les recommandations documentées. Le moteur ne décide pas, à lui seul, si une recommandation peut être clôturée.

## 3. Fonctions documentées et parcours utilisateur

Le DAD annonce recherche documentaire intelligente, réponses RAG, Graph RAG, analyse documentaire et traçabilité. Le schéma présente une interface web, une API Gateway et un Chat AI Service. Le README frontend mentionne les adresses du chat et du gateway ainsi que le SSO USign.

Parcours reconstitué :
1. L'auditeur ouvre le frontend et s'authentifie via le SSO.
2. Il formule une question.
3. Les services recherchent dans les représentations disponibles des données de MyAudit.
4. Les droits de l'utilisateur doivent contraindre les informations exploitables.
5. Le service construit une réponse à partir des éléments trouvés.
6. Le DAD prévoit des références aux sources, permettant la vérification.

Ce parcours est reconstitué à partir de l'architecture ; les écrans, filtres, boutons et interactions exactes n'ont pas été montrés. La présence de droits dans le modèle ne prouve pas que toutes les voies de recherche les appliquent correctement.

Exemples fictifs compatibles avec la finalité :
- « Quels rapports parlent de revues d'habilitations insuffisantes ? »
- « Résume les constats de la mission X et indique les sources. »
- « Quelles recommandations sont liées à l'entité Y ? »
- « Quels problèmes reviennent dans les missions de cette zone ? »

Les recherches complexes, les comptages exhaustifs et les filtres de statut doivent être validés sur le produit réel. Une recherche par similarité peut retrouver des exemples pertinents sans retrouver tous les enregistrements.

## 4. Deux types de connaissance complémentaires

**Documents non structurés** : rapports finaux PDF, texte et éventuellement éléments visuels. Le traitement extrait le contenu, le découpe et génère des représentations vectorielles. Il sert à retrouver des passages proches du sens de la question, puis à fournir un contexte au modèle.

**Données structurées** : missions, recommandations, entités, zones, pays, types de missions, utilisateurs et permissions. Elles permettent d'exploiter les identifiants et relations métier. Les schémas montrent un Data Handler et un Graph Transformer, avec PostgreSQL et Neo4j en aval.

Le RAG consiste à rechercher des éléments avant de générer une réponse. Le Graph RAG ajoute des relations entre objets, par exemple une recommandation rattachée à une mission liée à une entité appartenant à une zone. L'utilisation de documents comme contexte ne signifie pas que le modèle est réentraîné sur chaque document.

À confirmer : sélection du mode de recherche, rôle exact de chaque modèle, génération/refinement de Cypher, règles de combinaison entre graphes et vecteurs, gestion de l'absence de réponse et mécanisme de citation.

## 5. Alimentation MyAudit : trois workflows liés

Sources : RPA 0947, 0953, 0958, 0962, 0967, 1000038792 et 1000038794.

### 5.1 Premier passage : fichiers CSV

Le robot RPA récupère huit exports, les dépose dans S3 sous un préfixe daté, puis envoie une notification HTTP à l'Ingestion API. L'API publie un événement Redis ; le Data Transformer récupère les fichiers et les charge/transforme dans les couches Bronze, Silver et Gold.

Exports visibles :
1. Assignment_List
2. Assignment+-+Entity_List
3. Limitations_MyAudit
4. RecoExtraction
5. Entity
6. Audit+Zone
7. Type+Of+Assignment
8. Risk

La ponctuation des noms varie entre les schémas ; se référer au contrat réel avant implémentation.

### 5.2 Rapports finaux : retour de SQUID vers le robot

SQUID applique des règles métier pour identifier les pièces jointes correspondant aux rapports finaux à extraire. Il produit reports_to_extract en JSON et le dépose dans S3. Le RPA lit cette liste, extrait les rapports MyAudit, dépose les PDF sous SquidAI/reports/ et notifie l'API. Redis déclenche alors parsing, chunking et embedding.

Le schéma mentionne « Replace existing » pour les rapports : l'effet sur les anciennes versions et les anciens chunks est à vérifier. Les règles exactes permettant de choisir un rapport final ne sont pas fournies.

### 5.3 Droits partiels : second passage indispensable

SQUID repère les utilisateurs dont confidential_assignments_authorization vaut « Partial ». Il produit partial_rights_to_extract, contenant une liste d'utilisateurs. Le RPA récupère leurs droits détaillés dans MyAudit et restitue un CSV utilisateur/missions. Après notification, SQUID intègre ces données et finalise la table permissions.

Le schéma précise que les premières transformations peuvent alimenter les tables SAUF permissions ; la deuxième étape complète cette dernière. Le workflow partial rights publie ensuite un événement Gold ready vers le Data Handler.

Conséquence métier : la présence d'un PDF ou d'une mission dans les données ne suffit pas à garantir que l'auditeur pourra la consulter. Il faut également que ses droits aient été correctement calculés et propagés.

### 5.4 Déclenchement et délais

La timeline illustre : 01:00 extraction CSV, vers 01:10 traitement SQUID, 01:30 extraction des rapports, 01:50 extraction des droits partiels et vers 02:00 traitement final. Rapports et droits sont indiqués comme parallélisables.

Ces heures ne sont pas des horaires de production vérifiés. Le corpus décrit une extraction planifiée suivie de traitements événementiels ; cela ne démontre pas une synchronisation temps réel de chaque changement MyAudit.

### 5.5 Contrats d'échange visibles

Les notifications comportent type, filename, path, timestamp. Types exemples : « Final report », « Bulk CSV export », « Users partial rights ». reports_to_extract contient message, reports avec assignment_id et attachment_file, puis timestamp. partial_rights_to_extract contient message, users et timestamp.

Préfixes illustrés : SquidAI/YYYY_MM_DD/csv, SquidAI/YYYY_MM_DD/rpa et SquidAI/reports/. Différents formats de timestamp apparaissent : une normalisation contractuelle est à confirmer.

## 6. Bronze, Silver, Gold

| Couche | Rôle documenté | Conséquence |
|---|---|---|
| Landing S3 | Réception des exports et rapports | Stockage de fichiers distinct de leur disponibilité dans les recherches |
| Bronze | Historisation brute, ajout de lignes | Conserve les extractions avant nettoyage |
| Silver | Déduplication, détection ajout/modification/suppression, nettoyage et validation des types | Prépare les changements utiles |
| Gold | Normalisation, relations, chemins, permissions, rapports finaux, enrichissement par identifiants et dates | Fournit des données métier exploitables |

Le diagramme donne trois jours pour Bronze et un mois pour Silver comme EXEMPLES de rétention. Ce ne sont pas des règles validées.

SQL Server/MSSQL est explicitement dessiné pour les couches de transformation. PostgreSQL/pgvector est présent en aval pour la recherche. Ne pas confondre les deux rôles ni conclure à une migration effectuée : les différentes versions des schémas doivent être réconciliées.

## 7. Modèle de données observé

Gold : users, assignments, attachments, recommendations, entities, zones, countries, assignment_types, permissions et partial_rights. Tables d'association visibles : users_entities, users_zones, assignments_entities, assignments_zones, recommendations_entities, recommendations_zones.

Champs significatifs :
- users : directory_email comme clé, nom/prénom, confidential_assignments_authorization, to_extract, dates techniques.
- assignments : assignment_id, référence, nom, risk_theme, statut, expertise, confidential, mission planifiée, mots-clés, résumé exécutif, évaluation du contrôle interne, notation, niveau de risque, dates et type.
- attachments : attachment_id, assignment_id, nom, s3_path, file_name, type, taille, to_extract, dates et number_of_pages.
- recommendations : recommendation_id, assignment_id, notation, domaine, finding_id et texte/faits/conséquences, texte de recommandation, nombre de suivis validés, niveaux de risque et de cause, responsable, fonction, niveau, chemin de l'objet auditable, rédacteur, statut, expertise, réponse, jalons et échéances.
- entities et zones : identifiants, noms, chemins, niveaux hiérarchiques ; entities référence country_id et zone_id.
- permissions : permission_id, user_directory_email, assignment_id, recommendation_id, creation_timestamp et last_update_timestamp.
- partial_rights : paire utilisateur/mission.

Le schéma décrit une vérification d'accès par paire (UserEmail, AssignmentID) ou (UserEmail, RecommendationID) : une ligne trouvée indique un droit. Les règles générant ces lignes ne sont pas détaillées. La vérification des relations hiérarchiques, des exclusions et des missions confidentielles reste à faire.

Les annotations vertes proposent des conversions vers varchar ; les jaunes des renommages, dont color_rating vers assignment_rating ou recommendation_rating, text vers recommendation_text et file vers file_name. Ce sont des annotations de conception, pas la preuve du DDL actuel. risk_referential est barré dans Silver et une table de risque est barrée dans Gold, alors que l'export Risk existe en amont.

## 8. Architecture technique corroborée

| Élément | Rôle dans les pièces |
|---|---|
| RPA | Extraction MyAudit et dépôt des CSV/PDF |
| Ingestion API / DataIngestionAPI | Réception des notifications et déclenchement asynchrone |
| DataTransformer | Transformation métier, préparation rapports et droits |
| DataParsingHandler | Extraction du contenu documentaire |
| ChunkingHandler | Découpage du contenu |
| EmbeddingHandler | Génération des vecteurs |
| DataHandler | Traitement des données structurées ; stockage aval selon schémas |
| GraphTransformer | Transformation vers le graphe Neo4j |
| Squid API Gateway | SSO, accès aux ressources, points d'entrée DB de supervision |
| Chat AI Service | Recherche et génération de réponses |
| S3 Dell ECS | Landing zone et stockage de rapports |
| Redis / Redis Streams | Événements et cache ; Data Redis et Config Redis distingués |
| PostgreSQL / pgvector | Stockage aval et recherche vectorielle |
| Neo4j | Relations métier et requêtes Cypher |
| MongoDB | Historique de chat documenté ; autres usages à confirmer |
| RunAI | Exécution/déploiement des modèles |
| Grafana | Supervision, pas écran métier principal des auditeurs |

Le schéma de responsabilités MVP1 marque le frontend et l'Ingestion API comme développement IGE IT ; Gateway, Chat AI et handlers comme AIF ; Data Transformer est bicolore. Le bleu identifie des services Cocoon GENAI mutualisés. Cette représentation ne remplace pas un RACI actuel.

Le DAD cite Python 3.11+, FastAPI, PostgreSQL 15.12, MongoDB 7.0.28, Neo4j Community 5.27.0, Redis 7.4.3, Grafana Community 12.2.0, MLflow 3.5.1, Agno, jarvis-adk et composants Cocoon. Versions déclaratives à vérifier dans les lockfiles/manifests.

Modèles du tableau : GPT-OSS 120b pour « Retrieval », Qwen 30b pour résumer les chunks visuels, « Bge-3 » pour les embeddings. Conserver les libellés source ; ne pas inventer les IDs exacts de modèles.

## 9. Authentification et droits : deux sujets distincts

Le diagramme authorization code montre une redirection SSO avec scope openid ; retour code/state ; envoi code/nonce au gateway via POST /api/v1/sso/token ; échange backend /token ; validation de la signature et du nonce ; association hash MD5 d'id_token vers email en Redis, TTL indiqué 70 minutes. Le frontend reçoit l'id_token et le renvoie comme Bearer pour les appels suivants ; le gateway récupère l'email en Redis et l'injecte dans la requête aval.

C'est la description du schéma, pas une validation de sécurité. À examiner dans le code : usage d'id_token comme jeton API, validation issuer/audience/expiration, state/nonce/PKCE, révocation, durée de cache, accès direct au Chat Service et propagation des permissions. Le TTL Redis ne prouve pas la durée réelle de validité du jeton.

La connexion SSO prouve une identité ; elle ne suffit pas à prouver le droit de consulter chaque rapport. Les permissions métier doivent s'appliquer aux documents, aux passages remis au modèle, aux données structurées, aux réponses et aux sources.

Le flux DB de Grafana est séparé : /db/pgsql/query, /db/neo4j/query, /db/mongodb/query, HMAC-SHA256 et en-têtes X-Key-Id, X-Timestamp, X-Signature. Signature sur méthode, chemin, timestamp et hash du corps ; comparaison constante ; secrets référencés via configuration/vault. Le schéma illustre une fenêtre 300 secondes et des restrictions SELECT, find/aggregate, MATCH.

Points à confirmer : protection effective contre les rejeux dans la fenêtre, timestamps futurs, contrôle du langage de requête et comptes de bases réellement en lecture seule. Masquer les routes de Swagger n'est pas un contrôle d'accès.

## 10. Dépôts, développement et livraison

Quatre dépôts visibles : squid_ai_services, squid_api_gateway, squid-frontend, squid-grafana-dashboard. L'absence d'un dépôt Chat dédié dans les captures ne signifie pas que ce code n'existe pas.

Le README AI services décrit un orchestrateur GitLab lancé manuellement depuis l'interface, variable SELECTED_MODULE, sept modules ou All ; passage MODULE_NAME aux pipelines enfants. Feature vers dev, develop vers int, main vers preprod/prod. Prod comporte une étape manuelle. Les indications auto/manuelle de préproduction se contredisent.

Version APPLICATION_TAG_VERSION construite depuis un tag et l'heure, propagation dotenv. Le README recommande les tags annotés mais donne aussi un exemple de tag léger : vérifier le vrai YAML avant de suivre ces instructions. La sélection par date de tag ne garantit pas à elle seule l'association avec le commit visé.

Pipelines : dev build/sonar/build_docker/deploy ; int build_docker/promote_staging/deploy ; preprod et prod build/sonar/build_docker/promote_stable/deploy selon README. Registres scratch, staging, stable. Mise à jour des overlays Kustomize dans un dépôt de manifests, chemin ige-mais-deploy/<module>/overlays/<env>. Le DAD mentionne ArgoCD. Le README seul ne prouve pas la configuration GitOps en production ni la promotion du même digest entre preprod et prod.

Sonar est non bloquant selon le README ; couverture <Module>/coverage.xml. Ajouter un module nécessite d'actualiser les options, la matrice commune, les overlays et les chemins de couverture.

Le README gateway est essentiellement le template GitLab par défaut ; ne pas traiter ses suggestions Auto DevOps ou AWS comme la configuration réelle.

Le README frontend apporte la configuration npm interne, npm install/npm start, localhost:4200 et public/assets/env.js avec uSignApiAuthorizeUrl, uSignApiTokenUrl, uSignRedirectUri, squidChatServiceApi, squidGatewayApi. Le framework réel n'est pas prouvé : le DAD dit React, le port 4200 ne prouve pas Angular. Vérifier package.json.

Le bootstrap local via valeurs de session DEV est décrit ; les tokens sont privés et expirent. Les extraits npm/PowerShell tronqués ne sont pas prêts à exécuter.

Grafana : Docker Compose local, plugin Redis et Infinity, port 3000, provisioning, dossiers de dashboards. Ce README local ne définit pas les accès ni les versions de production. Les commandes down -v détruisent les volumes locaux.

## 11. Qualité, supervision et diagnostic

Le DAD décrit supervision de l'ingestion via PostgreSQL et données assainies vers Cocoon monitoring, ainsi que Redis/Grafana. La supervision retrieval via statistiques MongoDB est explicitement annoncée pour une future release.

Évaluation décrite : historique MongoDB, génération d'une réponse de référence par Claude Sonnet 3.5 via Amazon Bedrock, comparaison DeepEval et suivi MLflow. Métriques : Answer Relevancy, Faithfulness, Cypher Similarity, Contextual Relevancy/Recall/Precision pour PostgreSQL, GEval Answer Correctness. Une réponse produite par un autre modèle reste une référence automatique à valider, pas une vérité métier garantie.

Une déclaration d'infrastructure entièrement on-premise coexiste avec Bedrock dans l'évaluation : faire préciser si ce flux est historique, prévu, approuvé ou réellement exécuté, et sur quelles données. Aucune connexion à Bedrock n'a été effectuée dans cette analyse.

Pistes de diagnostic dérivées de l'architecture :
| Symptôme | Vérifications possibles, sans diagnostic présumé |
|---|---|
| Mission absente | Export RPA, dépôt S3, événement, transformation, relations et droits |
| Rapport absent | Sélection final report, reports_to_extract, extraction, notification, parsing/indexation |
| Réponse dépassée | Date d'extraction, delta, réindexation, versions et caches |
| Utilisateur ne voit rien | SSO puis permissions, droits partiels et mise à jour Gold |
| Mauvaise réponse métier | Source récupérée, fraîcheur, filtrage, requête générée, contexte et génération |
| Dashboard vide | Datasource, signature HMAC, droits DB, disponibilité du gateway |

## 12. Contradictions et limites à garder visibles

| Sujet | Éléments contradictoires ou incomplets | Conclusion prudente |
|---|---|---|
| Vecteurs | PostgreSQL/pgvector concret ; Pinecone/Weaviate dans le texte général | Retenir PostgreSQL comme architecture corroborée |
| Messaging | Redis Streams dans les workflows ; Kafka dans le DAD général | Redis est corroboré, Kafka non démontré |
| Frontend | React 18 dans DAD ; README sans package.json | Ne pas affirmer Angular ni valider React déployé |
| Hébergement | On-premise ; Bedrock dans évaluation ; cloud/multi-region dans prose | Clarifier les périmètres |
| Fonctionnalités avancées | Blockchain, GNN, sentiment, service mesh, GraphQL et autres déclarations | Pas de preuve d'implémentation dans le corpus |
| Performance | 50 utilisateurs/6 simultanés puis 100+ ; recherche 6 s puis 3 s | Objectifs divergents, aucune mesure fournie |
| Indexation | Moins de 5 min puis moins de 30 s | Ne pas présenter comme SLA |
| Rétention | 3 jours/1 mois marqués comme exemples | Politique non confirmée |
| Base de transformation | SQL Server visible mais absent du tableau infra principal | Confirmer l'état déployé |
| Chargement documentaire | Upload auditeur générique dans DAD ; RPA concret dans workflows | Parcours démontré : alimentation MyAudit par RPA |
| Exhaustivité | OCR partiels, noms tronqués et champs hors cadre | Ne pas reconstituer de DDL ou règles manquantes |
| Versions | Captures de main/develop et dates relatives | Ne pas les assimiler à une release déployée unique |

## 13. Volumétrie et fréquence déclarées

Le schéma mentionne un premier lot remontant deux ans, des rapports jusqu'à 300 pages estimées, environ 8 000 lignes d'assignments et 15 000 de recommandations, avec 150 nouvelles lignes de missions par an estimées. Les chiffres concernent des exports, pas nécessairement des objets uniques ou le corpus réellement indexé.

Missions, associations, limitations et recommandations : quotidien sur le schéma. Référentiels entités/zones/types/risques : hebdomadaire. Exemples affichés : 553 entités, 295 zones, 9 types et 219 risques. Plusieurs volumes restent « ??? ». Ne pas inventer les valeurs manquantes ni transformer les estimations en mesures actuelles.

## 14. Questions prioritaires à poser à l'équipe

1. Quelle version et quel périmètre sont réellement en production ? Quel document fait autorité ?
2. Qui possède chaque composant et le support entre IGE IT, AIF, RPA et Cocoon ?
3. Quelles sont les règles de sélection des rapports finaux et les statuts de recommandations ?
4. Comment calculer les permissions, exclusions, droits partiels et héritages entité/zone ?
5. À quel moment sont-ils appliqués dans les chemins vecteur, graphe, sources et historique ?
6. Quels horaires réels, délais de fraîcheur, contrôles de complétude et procédures de reprise ?
7. Comment gérer doublons, messages rejoués, erreurs, suppressions et changement de rapport ?
8. Quel moteur contient actuellement Bronze/Silver/Gold ?
9. Quels filtres et types de questions le chat prend-il effectivement en charge ?
10. Quel framework frontend et quel code source pour Chat AI ?
11. Quelles alertes et quelle procédure de retour arrière ? Sonar et validation métier bloquent-ils les releases ?
12. Quel périmètre d'évaluation Bedrock, quelle validation humaine et quels jeux de test métier ?

## 15. Résumé réutilisable

SQUID AI / MAIS est une plateforme de recherche et d'assistance à l'analyse pour les auditeurs IGE de CA-CIB. MyAudit fournit missions, recommandations, constats, suivis, référentiels, droits et rapports finaux PDF. Un robot RPA échange avec SQUID via S3 et notifications HTTP. Ingestion API publie dans Redis ; Data Transformer transforme Bronze/Silver/Gold, demande les rapports finaux et les droits partiels au robot, puis calcule les permissions. Les PDF suivent parsing/chunking/embedding ; les données structurées alimentent notamment PostgreSQL/pgvector et Neo4j. Frontend, API Gateway et Chat AI permettent une recherche avec RAG/Graph RAG ; MongoDB conserve au moins l'historique de chat. RunAI porte les modèles ; Grafana supervise. IGE IT et AIF partagent des responsabilités. Plusieurs paragraphes génériques du DAD sont incohérents avec les workflows détaillés ; ne pas affirmer leur implémentation. Cette référence est documentaire, pas un audit du code ni une attestation de production.

## Annexe A — Inventaire et observations par source

| Source | Observation | SHA-256 (12 premiers caractères) |
|---|---|---|
| RPA_interface_1000038792.jpg | Dépendances, identification rapports finaux et utilisateurs Partial. | bc4296db0fac |
| RPA_interface_1000038794.jpg | Workflow droits partiels, deuxième extraction, finalisation permissions et Gold ready. | 9c41539d76a9 |
| RPA_interface_1792771970947.jpg | Échanges bidirectionnels SQUID/RPA et entrées du RAG. | bcaaae3865af |
| RPA_interface_1792771970953.jpg | Workflow huit exports CSV, S3, notification, Redis, SQL Server. | bf15fa38145e |
| RPA_interface_1792771970958.jpg | Timeline illustrative et parallélisation possible rapports/droits. | cb30e831aa07 |
| RPA_interface_1792771970962.jpg | Extraction des rapports à la demande de SQUID ; traitement documentaire. | 702b55e8ab46 |
| RPA_interface_1792771970967.jpg | Exemples de contrats JSON et CSV des droits partiels. | 07558b582457 |
| RPA_interface_db_query_flow_ocr.md | Transcription Markdown fournie, reproduite en annexe B. | 83da88d1ebac |
| Squid_AI_DAD_OCR(1).md | Transcription Markdown fournie, reproduite en annexe B. | f9908c451adc |
| arch-diagram_1792771971009(1).jpg | SSO authorization code, gateway, association id_token/email Redis, appels suivants. | 752c4c4a1cad |
| arch-diagram_1792771971015(1).jpg | Répartition IGE/AIF, MyAudit, MSSQL, traitements et stockages aval. | fba0dd142e32 |
| arch-diagram_1792771971020(1).jpg | Architecture par couches MVP1, responsabilités colorées, services Cocoon. | ccd894458265 |
| arch-diagram_1792771971025(1).jpg | Architecture générale MVP1, Redis Streams, supervision, sauvegarde et nettoyage. | 22c34224c22f |
| arch-diagram_1792771971030(1).jpg | DB Query Flow, HMAC, fenêtre 300 secondes et restrictions annoncées de requêtes. | 24b81e7a93f0 |
| myAudiDataIngestion_1000038788(1).jpg | Pipeline médaillon, rôles Bronze/Silver/Gold, rétentions illustratives. | 1c7d261685dd |
| myAudiDataIngestion_1000038790(1).jpg | Silver, tables et changements de noms/types, partie basse tronquée. | e84794a573c5 |
| myAudiDataIngestion_1792771970972(1).jpg | Silver, complément sur les champs de recommandations. | d6d4309fbba4 |
| myAudiDataIngestion_1792771970977(1).jpg | Exemples Gold : entités, zones, permissions. | 29830751cbfa |
| myAudiDataIngestion_1792771970981(1).jpg | Fréquences et volumétrie estimée, nombreux volumes inconnus. | f349b1094e83 |
| myAudiDataIngestion_1792771970986(1).jpg | Gold : users, partial_rights, assignments, attachments, recommendations. | cb0a05ea9235 |
| myAudiDataIngestion_1792771970993(1).jpg | Gold : entités/zones/pays, tables associatives et permissions. | f359aed1edba |
| myAudiDataIngestion_1792771970997(1).jpg | Relations métier MyAudit : missions, rapports, recommandations, entités, zones. | bd04a3f8d7d0 |
| myAudiDataIngestion_1792771971003(1).jpg | Bronze et champs d’extraction, certains bas de tables hors cadre. | c010e2d184af |
| squid_ai_services_repo_readme_ocr(1).md | Transcription Markdown fournie, reproduite en annexe B. | 86c1c86ec78c |
| squid_api_gateway_repo_readme_ocr(1).md | Transcription Markdown fournie, reproduite en annexe B. | c7cad8bd8028 |
| squid_frontend_repo_readme_ocr(1).md | Transcription Markdown fournie, reproduite en annexe B. | cf357ee76c1e |
| squid_grafana_dashboard_repo_readme_ocr(1).md | Transcription Markdown fournie, reproduite en annexe B. | 199668cb663c |

## Annexe B — Transcriptions fournies, conservées sans modification

Ces textes restent des OCR partiels. Leurs affirmations ne remplacent pas les réserves de l’analyse ci-dessus. Les suggestions génériques GitLab et extraits incomplets ne sont pas des instructions opérationnelles validées.


---

### Document source : RPA_interface_db_query_flow_ocr.md

# DB Query Flow

> Transcription des deux captures de `DB Query Flow.docx`, regroupées sans duplication. Texte original en anglais.

## Query Execution Flow

### Client Prepares Request

- Client (Grafana) creates a database query request (for PostgreSQL, Neo4j, or MongoDB).
- Client generates an HMAC signature using the HTTP method, path, timestamp, and body hash.
- Client adds the following headers:
  - `X-Key-Id` (identifies the secret key)
  - `X-Timestamp` (current time)
  - `X-Signature` (HMAC signature)
- Client sends the HTTP request to the API gateway's `/db/pgsql/query`, `/db/neo4j/query`, or `/db/mongodb/query` endpoint.

### API Gateway Receives Request

- FastAPI application receives the request at the `/db/*` endpoint.
- The request is routed to the correct handler based on the path.

### HMAC Authentication

- The `verify_hmac` dependency runs before the route handler:
  - Extracts `X-Key-Id`, `X-Timestamp`, and `X-Signature` from headers.
  - Checks that all required headers are present.
  - Validates the timestamp to prevent replay attacks.
  - Retrieves the shared secret using `X-Key-Id`.
  - Reads and hashes the request body.
  - Builds the canonical string from method, path, timestamp, and body hash.
  - Calculates the expected HMAC signature.
  - Compares the expected signature with the received signature using constant-time comparison.
  - If the check fails, the request is rejected.

### Route Handler Execution

- The correct route handler (`execute_pgsql_query`, `execute_neo4j_query`, or `execute_mongodb_query`) is called.
- The handler calls the appropriate service (`PgSQLService`, `Neo4jService`, or `MongoDBService`) with the query and parameters.

### Database Service Execution

- The service connects to the target database.
- Executes the query (SQL, Cypher, or MongoDB operation).
- Collects the results.

### Response Handling

- The service returns the results to the route handler.
- The handler formats the response using the appropriate response model.
- The response is sent back to the client.

### Error Handling

- If any error occurs at any step, it is logged.
- An HTTP error response is returned to the client with a relevant message.

## Security

### Authentication & Authorization

- Every `/db/*` route is protected by HMAC authentication (`verify_hmac`), which requires a valid signature, timestamp, and key ID in the headers.
- Only clients with the correct shared secret can generate a valid HMAC signature, ensuring only authorized internal services can access these endpoints.
- The API routes are hidden from public documentation (`include_in_schema=False`), further reducing exposure.

### Request Validation

- The HMAC process checks that the request is recent (timestamp), untampered (body hash), and from a trusted source (signature).
- Constant-time comparison is used to prevent timing attacks when checking signatures.

---

*Source : deux photographies de l'écran affichant `DB Query Flow.docx`. Les éléments recouvrant les deux captures ont été retranscrits une seule fois.*


---

### Document source : Squid_AI_DAD_OCR(1).md

# SQUID AI — DETAILED ARCHITECTURE DOCUMENT — V4.3

**Squid AI — Intelligent Search Platform for IGE Auditors**  
**Crédit Agricole Corporate & Investment Bank (CA-CIB)**  
**Classification affichée : Usage Interne**

> **Transcription des captures d'écran fournies (15 images, dont une vue en double).**
> Texte conservé dans sa langue d'origine (anglais). Les pages ont été remises dans un ordre logique.
> Certains passages et schémas sont partiellement visibles, et plusieurs pages du document original ne figurent pas dans les captures. Les passages non récupérables sont signalés par `[illisible]` ou `[tronqué]`. Cette transcription ne constitue pas la reproduction intégrale du document original.

## Document control

| Version | Description | Author | Creation Date | Diffusion Date | Status |
|---|---|---|---|---|---|
| V1 | Initial Architecture Document | AI Team | 2025-07-31 | 2025-07-31 | Draft |

| Version | Validator Name | Validation Date |
|---|---|---|
| V1 | | |

> **NB:** the comments in th… [texte masqué par l'image sur la capture]

## Table of contents (visible extract)

- 4.1.3 Framework
- 4.1.4 Major principles
- 4.1.5 Organization
- 4.1.6 Front-end optimization & frugality
- 4.1.7 Detailed description
- 4.2 Service Layer
  - 4.2.1 General description
  - 4.2.2 Layer function
  - 4.2.3 Framework
  - 4.2.4 Major principles
  - 4.2.5 Organization
  - 4.2.6 Service optimization & frugality
  - 4.2.7 Detailed description
- 4.3 Data Access Layer
  - 4.3.1 General description
  - 4.3.2 Layer function
  - 4.3.3 Framework
  - 4.3.4 Major principles
  - 4.3.5 Organization
  - 4.3.6 Data Minimization processes
  - 4.3.7 Physical model of data
    - 4.3.7.1 Physical data model schema
    - 4.3.7.2 Tables list
- 4.4 Transversal layer
  - 4.4.1 Transaction
  - 4.4.2 Error management
  - 4.4.3 Logging
  - 4.4.4 Internationalization
  - 4.4.5 Security
  - 4.4.6 Monitoring
  - 4.4.7 Exception handling
  - 4.4.8 Others
- 4.5 Batch treatment
  - 4.5.1 `<Treatment name>`
    - 4.5.1.1 Objective
    - 4.5.1.2 Sequencing
    - 4.5.1.3 Specifications

---

# 1. Application Card

## 1.1 General Information

| Field | Value |
|---|---|
| Application code | MAIS |
| Application name | My Audit Intelligent Search |
| Business Line | IGE |
| IT Line | AIF |
| Application Manager | TBD |
| Business Sponsor | IGE Audit Team |

## 1.2 Functional Description

Squid AI is an intelligent search platform designed specifically for IGE (Internal General Evaluation) auditors. The platform leverages advanced AI technologies including Retrieval-Augmented Generation (RAG) and Graph RAG to provide sophisticated search capabilities across audit documents, recommendations and follow ups.

| Functions | Description |
|---|---|
| Intelligent Document Search | AI-powered search across audit documents with semantic understanding |
| RAG-based Query Processing | Context-aware responses using retrieval-augmented generation |
| Graph RAG Implementation | Relationship-based search using knowledge graphs |
| Document Analysis & Insights | Automated analysis of audit documents with key insights extraction |
| Audit Trail Generation | Comprehensive audit trails for all search activities and findings |

## 1.3 Check List on Securing Payment

NA

## 1.4 Reference Documents

| Reference | Title | Provider |
|---|---|---|
| GAD-001 | Global Architecture Document | Enterprise Architecture |
| REQ-001 | Business Requirements Document | IGE Audit Team |

---

# 2. Non-Functional Constraints

## 2.1 End-users Typology and location

| Environment | Organizational Entity | Profile Name | Geographical Localization | Number of users | Estimated Simultaneous users | Usage Frequency | Timeframe of usage | Specific Needs Software requirements |
|---|---|---|---|---:|---:|---|---|---|
| Production | CACIB | | Paris | 50 | 6 | Daily | Working Hours | Standard Browser |
| Pre-PROD | CACIB | | Paris | 50 | 6 | Daily | Working Hours | Standard Browser |
| Dev | GIT | | Paris | 5 | 2 | Daily | Working Hours | Standard Browser |

## 2.2 Deployment Constraints

- We make sure that CI/CD credentials are securely configured as Variables on Gitlab.
- All the infrastructure is on-premise.
  - For Applications, we use Gitlab and Argocd to deploy applications.
  - For Models, we use RunAI CI to deploy the models.

## 2.3 Usage and Performance Requirements

| Functions | Time Frames | Response Time | User Role | Frequency |
|---|---|---|---|---|
| Document Search | Business Hours | < 6 seconds | Auditor | High |
| RAG Query Processing | Business Hours | < 8 seconds | Senior Auditor | Medium |
| Graph RAG Analysis | Business Hours | < 10 seconds | Senior Auditor | Medium |
| Document Upload/Indexing | 24/7 | < 5 minutes | System | Low |

---

# 3. Overall Application Architecture

## 3.1 Applicative Design Overview

### 3.1.1 General Description

Squid AI follows a modern microservices architecture designed for AI-powered search capabilities. The system is built around several key architectural patterns:

- Event-driven architecture for real-time document processing
- Microservices pattern for scalable and maintainable components
- CQRS (Command Query Responsibility Segregation) for optimized read/write operations
- API Gateway pattern for centralized access management
- Repository pattern for data access abstraction

The architecture is designed to handle large volumes of audit documents while providing fast, intelligent search capabilities through advanced AI technologies.

## 3.2 General Technical Architecture

### 3.2.1 Architecture Scheme Analysis

Based on the provided architecture diagram, the Squid AI system consists of several interconnected layers and components:

**Architecture diagrams:** Two diagrams are displayed in the source screenshot, but their small labels are not reliably readable. The following pages describe their components in text.

For more detailed view, can you please follow the easymap diagram.

`https://ca-cib.leanix.net/cacib/diagrams/freedraw/d6146a44-971d-40fa-bf5e-9649c39748fc?returnUrl=%2Fdiagrams%2Foverview%2Fall-diagrams`

### 1. Overall Architecture Pattern

The diagram shows a **layered microservices architecture** with clear separation of concerns:

- **External Interface Layer** (top)
- **Presentation/Gateway Layer**
- **Service/Processing Layer** (middle — the pink/red section)
- **Data Storage Layer** (bottom)
- **Infrastructure/Cross-cutting Layer** (green section)

### 2. External Interface Layer

- **Web Clients:** Multiple user interfaces for different user types
- **API Consumers:** External systems that consume the platform's APIs

### 3. API Gateway & Load Balancing

- **API Gateway:** Central entry point for all requests, handling:
  - Authentication and authorization
  - Rate limiting and throttling
  - Request routing and transformation
- **Load Balancer:** Distributes incoming traffic across multiple service instances

### 4. Core Service Layer (Pink/Red Section)

This is the heart of the Squid AI platform with multiple specialized services:

**Search & Query Services:**

- **Search Service:** Handles traditional keyword-based searches
- **Intelligent Query Processor:** Advanced query understanding and processing
- **Query Optimizer:** Optimizes search queries for better performance

**AI/ML Services:**

- **RAG Engine:** Retrieval-Augmented Generation for intelligent responses
- **Graph RAG:** Enhanced RAG with knowledge graph capabilities
- **LLM Service:** Large Language Model integration
- **Embedding Service:** Vector embeddings for semantic search
- **Data processor:** Data processing capabilities

**Document Processing:**

- **Document Ingestion:** Handles document upload and initial processing
- **Content Extraction:** Extracts text and metadata from various document formats
- **Document Analyzer:** Analyzes document structure and content

**Knowledge Management:**

- **Knowledge Graph Builder:** Constructs and maintains knowledge graphs
- **Relationship Extractor:** Identifies relationships between entities
- **Entity Recognition:** Named Entity Recognition (NER) capabilities

### 5. Data Storage Layer (Bottom)

Multiple specialized data stores for different use cases:

**Primary Databases:**

- **Vector Database:** Stores document embeddings for semantic search
- **Graph Database:** Stores entity relationships and knowledge graphs
- **Document Store:** Raw document storage (likely object storage)
- **Relational Database:** Structured data, user management, metadata

**Caching & Indexing:**

- **Redis Cache:** In-memory caching for frequently accessed data
- **Search Index:** Elasticsearch or similar for full-text search
- **Query Cache:** Caches frequently executed queries

### 6. Infrastructure Layer (Green Section)

Cross-cutting concerns and infrastructure services:

**Container Orchestration:**

- **Kubernetes:** Container orchestration platform
- **Container Registry:** Docker image storage and management
- **Service Mesh:** Inter-service communication management

**Message Queue & Streaming:**

- **Message Queue:** Asynchronous communication between services
- **Event Streaming:** Real-time data processing pipeline
- **Workflow Engine:** Orchestrates complex data processing workflows

**Monitoring & Observability:**

- **Ingestion Monitoring:**
  - Assignment ingestions and Recommendation report ingestions are monitored on CAGIP absana dashboard
  - Datasource is from PostgresSQL (Sanitized data is pushed to cocoon monitoring db)
  - Redis messages are also monitored using plugin of Grafana.
- **Retrieval Monitoring:**
  - Using MongoDB statistics: **For future release.**
- **Monitoring Service:** System health and performance monitoring
- **Logging Service:** Centralized log aggregation and analysis
- **Metrics Collection:** Performance metrics and analytics

**Security & Compliance:**

- **Authentication Service:** User authentication and token management
- **Authorization Service:** Role-based access control
- **Audit Service:** Compliance and audit trail management

### 7. Data Flow Architecture

**Document Processing Pipeline:**

1. Ingestion → Content Extraction → Document Processing → Embedding Generation → Storage
2. **Knowledge Graph Construction:** Entities and relationships are extracted and stored in the graph database

**Query Processing Pipeline:**

1. User Query → Query Processor → RAG Engine → Vector Search + Graph Search → Response Generation
2. **Intelligent Routing:** Queries are routed to appropriate services based on intent

**Search Enhancement:**

- **Semantic Search:** Using vector embeddings for meaning-based search
- **Graph-Enhanced Search:** Leveraging knowledge graphs for contextual results
- **Hybrid Approach:** Combining traditional search with AI-powered capabilities

### 8. Key Architectural Patterns

**Microservices Architecture:**

- Each service has a single responsibility
- Services communicate via APIs and message queues
- Independent deployment and scaling

**Event-Driven Architecture:**

- Asynchronous processing using message queues
- Event sourcing for audit trails
- Real-time data processing capabilities

**CQRS (Command Query Responsibility Segregation):**

- Separate read and write operations
- Optimized data models for different use cases
- Better performance and scalability

### 9. Scalability & Performance Considerations

**Horizontal Scaling:**

- Stateless services that can be scaled independently
- Load balancing across multiple instances
- Auto-scaling based on demand

**Caching Strategy:**

- Multi-level caching (application, database)
- Cache invalidation strategies
- Performance optimization

**Data Partitioning:**

- Shared databases for large datasets
- Distributed vector storage
- Optimized indexing strategies

### 10. Integration Points

**External Systems:**

- **Data Sources:** Integration with various document repositories
- **Identity Providers:** SSO and authentication systems
- **Audit Systems:** Compliance and regulatory reporting

**API Design:**

- RESTful APIs for standard operations
- GraphQL for flexible data querying
- WebSocket for real-time updates

This architecture represents a sophisticated, enterprise-grade intelligent search platform designed for IGE auditors, providing advanced AI capabilities including RAG and Graph RAG for intelligent document analysis and search.

### Component view by layer (additional text accompanying diagrams)

**EXTERNAL LAYER (Yellow):**

- **Web Client:** Browser-based interface for auditors to interact with the system
- **External APIs:** Integration points for third-party audit tools and systems

**PRESENTATION LAYER (Blue):**

- **API Gateway:** Central entry point managing authentication, rate limiting, and request routing
- **Load Balancer:** Distributes incoming requests across multiple service instances
- **Web Application:** React-based frontend providing the user interface

**SERVICE LAYER (Red/Pink):**

- **Search Service:** Core search functionality with semantic search capabilities
- **RAG Service:** Retrieval-Augmented Generation service for context-aware responses
- **Graph RAG Service:** Advanced relationship-based search using knowledge graphs
- **Document Processing Service:** Handles document ingestion, parsing, and indexing
- **Analytics Service:** Provides search analytics and usage insights
- **User Management Service:** Handles authentication, authorization, and user preferences

**DATA LAYER (Green):**

- **Vector Database:** Stores document embeddings for semantic search
- **Graph Database:** Manages relationships and entities for Graph RAG
- **Document Store:** Primary storage for audit documents and metadata
- **Cache Layer:** Redis-based caching for improved performance
- **Search Index:** Elasticsearch for fast text-based search capabilities

**AI/ML LAYER (Light Blue):**

- **LLM Engine:** Large Language Model for natural language processing
- **Embedding Service:** Converts documents and queries into vector representations
- **Knowledge Graph Builder:** Constructs and maintains knowledge graphs from documents
- **Model Registry:** Manages AI model versions and deployments

**INFRASTRUCTURE LAYER (Gray):**

- **Container Orchestration:** Kubernetes for container management
- **Message Queue:** Apache Kafka for asynchronous processing
- **Monitoring & Logging:** Comprehensive observability stack
- **Storage Services:** Distributed file storage and backup systems

### 3.2.2 Frameworks & Development Tools

| Software | Version | License Purchase Required (Y/N) | Software Factory Standard (Y/N) |
|---|---|---|---|
| Python | 3.11+ | N | Y |
| FastAPI | 0.100+ | N | Y |
| React.js | 18.x | N | Y |
| TypeScript | 5.x | N | Y |
| Elasticsearch | 8.x | N | Y |
| Neo4j | 5.x | N | Y |
| Redis | 7.x | N | Y |
| Docker | 24.x | N | Y |
| RunAI | 2.4 | Y (mutual) | Y |
| Cocoon-database-manager | 4.0.8 | N | Y |
| Cocoon-shared-components | 3.0.3 | N | Y |
| pydantic | 2.11.5 | N | Y |
| Pymongo[encryption] | 4.5 | N | Y |
| uvicorn | 0.24 | N | Y |
| boto3 | 1.35.81 | N | Y |
| pypdf2 | 3.0.1 | N | Y |
| Agno | 1.8.1 | N | Y |
| jarvis-adk[runai] | 0.1.0.a1 | N | Y |

**Infrastructure** (table continues across two screenshots):

| Infrastructure | Version | Localization | Mutualized or Dedicated |
|---|---|---|---|
| Postgres-SQL (Pgvector) | 15.12 | Diderot D'alembert | Cocoon Mutualized |
| MongoDB | 7.0.28 | Diderot D'alembert | Cocoon Mutualized |
| Neo4j (Community) | 5.27.0 | Diderot D'alembert | Dedicated |
| S3 (ECS) | CAGIP | Diderot D'alembert | Dedicated |
| RunAI | 2.18 | Marcoussis | Cocoon Mutualized |
| Redis | 7.4.3 | Diderot D'alembert | Dedicated |
| Kubernetes (MAIS) | CAGIP | Diderot D'alembert | Dedicated |
| Grafana (Community) | 12.2.0 | Kubernetes (MAIS) | Dedicated |
| MLFlow | 3.5.1 | Diderot D'alembert | Cocoon Mutualized |

**LLM models:**

| LLM model | Version | Purpose |
|---|---|---|
| GPT-OSS – 120b | N/A | Retrieval |
| Qwen – 30b | N/A | Visual Chunk Summarization |
| Bge-3 | N/A | Embedding |

---

# 4. RAG and Graph RAG Implementation

## 4.1 RAG (Retrieval-Augmented Generation) Architecture

The RAG implementation in Squid AI provides context-aware responses by combining information retrieval with language generation:

### DOCUMENT INGESTION PIPELINE

1. **Document Upload:** Auditors upload documents through the web interface
2. **Text Extraction:** Extract text from various formats (PDF, Word, Excel, etc.)
3. **Chunking:** Split documents into semantic chunks for better retrieval
4. **Embedding Generation:** Convert text chunks into vector embeddings using specialized models
5. **Vector Storage:** Store embeddings in a vector database (Pinecone/Weaviate)
6. **Metadata Indexing:** Index document metadata for filtering and faceted search

### QUERY PROCESSING PIPELINE

1. **Query Analysis:** Parse and understand user intent
2. **Query Embedding:** Convert user query into vector representation
3. **Similarity Search:** Find most relevant document chunks using vector similarity
4. **Context Assembly:** Gather relevant chunks and metadata
5. **Prompt Engineering:** Construct prompts with context and query
6. **LLM Processing:** Generate response using Large Language Model
7. **Response Post-processing:** Format and validate the response
8. **Source Attribution:** Provide references to source documents

### KEY COMPONENTS

- **Embedding Models:** Specialized models for audit domain (fine-tuned BERT/RoBERTa)
- **Vector Database:** High-performance similarity search capabilities
- **Chunk Management:** Intelligent document segmentation with overlap
- **Context Window Optimization:** Efficient use of LLM context limits
- **Relevance Scoring:** Advanced scoring mechanisms for result ranking

## 4.2 Graph RAG Architecture

Graph RAG extends traditional RAG by incorporating relationship information through knowledge graphs:

### KNOWLEDGE GRAPH CONSTRUCTION

1. **Entity Extraction:** Identify entities (people, organizations, regulations, processes)
2. **Relationship Mining:** Extract relationships between entities using NLP
3. **Graph Building:** Construct knowledge graph with nodes and edges
4. **Graph Enrichment:** Add external knowledge sources and ontologies
5. **Graph Validation:** Ensure consistency and accuracy of relationships
6. **Incremental Updates:** Continuously update graph with new documents

### GRAPH-ENHANCED QUERY PROCESSING

1. **Query Entity Recognition:** Identify entities mentioned in user queries
2. **Graph Traversal:** Explore related entities and relationships
3. **Subgraph Extraction:** Extract relevant subgraphs for context
4. **Path Analysis:** Analyze relationship paths for deeper insights
5. **Hybrid Retrieval:** Combine vector similarity with graph relationships
6. **Context Augmentation:** Enrich context with relationship information
7. **Graph-Aware Generation:** Generate responses considering entity relationships

### ADVANCED CAPABILITIES

- **Multi-hop Reasoning:** Answer questions requiring multiple relationship steps
- **Temporal Analysis:** Track changes in relationships over time
- **Compliance Mapping:** Map regulatory requirements to audit evidence
- **Risk Propagation:** Understand how risks propagate through relationships
- **Pattern Detection:** Identify recurring patterns in audit findings

### GRAPH DATABASE FEATURES

- **Neo4j Integration:** High-performance graph database for complex queries
- **Cypher Queries:** Sophisticated graph query language for relationship exploration
- **Graph Algorithms:** Built-in algorithms for centrality, community detection
- **Visualization:** Interactive graph visualization for auditors
- **Schema Management:** Flexible schema evolution for audit domains

---

# 5. AI Components and Services

## LARGE LANGUAGE MODEL (LLM) ENGINE

- **Primary Models:** Qwen or on-premises alternatives
- **Domain Adaptation:** Fine-tuned models for audit terminology
- **Multi-language Support:** Support for multiple languages in audit documents
- **Safety Measures:** Content filtering and bias detection
- **Performance Optimization:** Model quantization and caching strategies

## EMBEDDING SERVICES

- **Text Embeddings:** Specialized embeddings for audit documents
- **Multilingual Models:** Support for documents in multiple languages
- **Domain-Specific Models:** Fine-tuned on audit and compliance data
- **Batch Processing:** Efficient processing of large document collections
- **Real-time Inference:** Fast embedding generation for live queries

## NATURAL LANGUAGE PROCESSING

- **Named Entity Recognition:** Extract audit-relevant entities
- **Relation Extraction:** Identify relationships between entities
- **Sentiment Analysis:** Analyze tone and sentiment in audit findings
- **Classification:** Categorize documents by type, risk level, importance
- **Summarization:** Generate concise summaries of long documents

## KNOWLEDGE GRAPH AI

- **Graph Neural Networks:** Advanced reasoning over graph structures
- **Entity Linking:** Connect entities to external knowledge bases
- **Ontology Management:** Maintain audit domain ontologies
- **Graph Completion:** Predict missing relationships in the graph
- **Anomaly Detection:** Identify unusual patterns in relationships

## SEARCH INTELLIGENCE

- **Query Understanding:** Interpret complex audit queries
- **Intent Classification:** Understand user search intent
- **Query Expansion:** Enhance queries with related terms
- **Result Ranking:** Intelligent ranking of search results
- **Personalization:** Adapt results to user preferences and role

---

# 6. Security and Compliance

## AUTHENTICATION & AUTHORIZATION

- **Multi-Factor Authentication (MFA):** Required for all users
- **Single Sign-On (SSO):** Integration with corporate identity systems
- **Role-Based Access Control (RBAC):** Granular permissions based on audit roles
- **Attribute-Based Access Control (ABAC):** Context-aware access decisions
- **API Security:** OAuth 2.0 and JWT tokens for API access

## DATA PROTECTION

- **Encryption at Rest:** AES-256 encryption for all stored data
- **Encryption in Transit:** TLS 1.3 for all network communications
- **Key Management:** Hardware Security Modules (HSM) for key storage
- **Data Masking:** Sensitive data masking in non-production environments
- **Secure Deletion:** Cryptographic erasure for data removal

## AUDIT & COMPLIANCE

- **Comprehensive Audit Logs:** All user actions and system events logged
- **Immutable Audit Trail:** Tamper-proof logging using blockchain techniques
- **Data Lineage:** Track data flow and transformations
- **Privacy Controls:** GDPR and privacy regulation compliance
- **Retention Policies:** Automated data retention and deletion

## NETWORK SECURITY

- **Zero Trust Architecture:** Never trust, always verify principle
- **Network Segmentation:** Isolated network zones for different functions
- **Intrusion Detection:** Advanced threat detection systems
- **DDoS Protection:** Protection against distributed denial of service attacks
- **Vulnerability Management:** Regular security assessments and patching

## AI SECURITY

- **Model Security:** Protection against adversarial attacks
- **Data Poisoning Prevention:** Safeguards against malicious training data
- **Prompt Injection Protection:** Security measures against prompt attacks
- **Model Interpretability:** Explainable AI for audit trail requirements
- **Bias Detection:** Regular testing for algorithmic bias

---

# 7. Performance and Scalability

## CHATBOT EVALUATION ARCHITECTURE

**Architecture flow (reconstructed from the visible diagram):**

```text
MongoDB
   │
   ▼
Fetch Chat History ───────────────┐
   │                              │
   ▼                              ▼
Amazon Bedrock              DeepEval
(Claude Sonnet 3.5)              ▲
   │                              │
   ▼                              │
Generate Ground Truth ────────────┘
                                  │
                                  ▼
                      Compare Results And
                        Upload to MLFlow
                                  │
                                  ▼
                                MLflow
```

> Diagram note: the source shows chat history fetched from MongoDB, ground truth generation via Amazon Bedrock/Claude Sonnet 3.5, evaluation with DeepEval, then results uploaded to MLflow.

## GROUND TRUTH GENERATION

A full record per question including: question and session identifiers, all three Cypher query variants (generated, refined, executed), and the raw graph results, are fed into Claude Sonnet 3.5, which is regarded as a superior model. The Claude-generated ground truth answer, and the original production answer, will be passed as the primary input to downstream evaluation metrics.

DeepEval Framework compares the production system's answers against the ground truth answers according to the following metrics:

| Metric | Description | Focus Area |
|---|---|---|
| Answer Relevancy | Evaluates how directly the generated answer addresses the user query without unnecessary digressions. | Generation quality |
| Faithfulness | Measures if the generated answer is factually consistent with and grounded in the provided context, detecting hallucinations. | Hallucination check |
| Cypher Similarity (Neo4J Only) | Assesses the deviation between generated cypher query vs refined cypher query. | Cypher similarity |
| Contextual Relevancy (Postgres Only) | Assesses whether the retrieved context chunks are relevant to the user query. | Retrieval relevance |
| Contextual Recall (Postgres Only) | Evaluates if the retrieved context contains all key information needed to answer the query. | Retrieval completeness |
| Contextual Precision (Postgres Only) | Measures if the retrieved context is precise and focused. | Retrieval ranking |
| GEval (G-Eval) Answer Correctness | LLM-as-judge framework for custom criteria like factual correctness vs. expected output, task success, safety, tone. | LLM Judge |

Contextual related metrics are only used in Postgres SQL query due to the embedding store allowing semantic search capabilities.

Experiments are tracked in MLFlow.

## PERFORMANCE TARGETS

- **Search Response Time:** < 3 seconds for 95% of queries
- **RAG Processing:** < 5 seconds for complex contextual queries
- **Graph RAG:** < 10 seconds for multi-hop relationship queries
- **Document Indexing:** < 30 seconds per document
- **Concurrent Users:** Support for 100+ simultaneous users
- **Throughput:** 1000+ queries per minute at peak load

## SCALABILITY ARCHITECTURE

- **Horizontal Scaling:** Auto-scaling microservices based on demand
- **Load Balancing:** Intelligent distribution of requests across instances
- **Caching Strategy:** Multi-level caching for improved performance
- **Database Sharding:** Distributed data storage for large document collections
- **CDN Integration:** Content delivery network for static assets

## OPTIMIZATION STRATEGIES

- **Vector Index Optimization:** Efficient similarity search algorithms
- **Query Optimization:** Smart query planning and execution
- **Batch Processing:** Efficient processing of bulk operations
- **Resource Management:** Dynamic resource allocation based on workload
- **Performance Monitoring:** Real-time performance metrics and alerting

## INFRASTRUCTURE SCALING

- **Kubernetes Auto-scaling:** Dynamic pod scaling based on metrics
- **Database Read Replicas:** Scale read operations across replicas
- **Message Queue Partitioning:** Distributed message processing
- **Storage Scaling:** Elastic storage scaling for document growth
- **Multi-Region Deployment:** Geographic distribution for global access

---

# 8. Conclusion

Squid AI represents a cutting-edge intelligent search platform specifically designed for IGE auditors. The architecture leverages advanced AI technologies including RAG and Graph RAG to provide unprecedented search capabilities across audit documents and regulatory frameworks.

## KEY ARCHITECTURAL BENEFITS

- Scalable microservices architecture for high availability and performance
- Advanced AI capabilities with RAG and Graph RAG for intelligent search
- Comprehensive security framework ensuring data protection and compliance
- Flexible deployment options supporting on-premises and cloud environments
- Extensible design allowing for future AI capability enhancements

The system is designed to transform how auditors discover, analyze, and utilize information, significantly improving audit efficiency and effectiveness while maintaining the highest standards of security and compliance.

This architecture document provides the foundation for implementation, ensuring all stakeholders understand the technical approach, capabilities, and requirements for the Squid AI platform.

---

## Notes sur la transcription

- **Sources :** 15 photographies d'un écran, avec une capture doublée des tableaux techniques. L'ordre des images reçues était inverse de celui du document.
- **Périmètre :** uniquement les pages et fragments visibles. Les parties du document source non photographiées ne sont pas reconstituées.
- **Schémas :** le contenu textuel lisible est retranscrit ; les schémas détaillés en faible résolution ne sont pas reproduits intégralement.
- **Écarts présents dans la source :** certaines technologies et objectifs varient d'une section à l'autre (par exemple bases vectorielles génériques versus PostgreSQL/pgvector, et différents objectifs de temps de réponse). Ces écarts ont été conservés tels quels, sans les corriger ni les fusionner.


---

### Document source : squid_ai_services_repo_readme_ocr(1).md

# SQUID AI — `squid_ai_services` GitLab repository

> **Transcription of 9 screenshots.** Original English preserved. The screenshots show a GitLab group, the `develop` branch of `squid_ai_services`, and excerpts from its `README.md`. Where a line extends beyond a screenshot, this is explicitly noted; no unseen repository content has been invented. Repeated passages across overlapping screenshots have been transcribed once.

## 1. GitLab group and projects

**GitLab breadcrumb:** `grc-cacib / AIFactory / Projects / SQUID AI`

**Group:** `SQUID AI` (private)

**Visible projects:**

| Project | Role shown | Creation date shown |
|---|---|---|
| `squid_ai_services` | Maintainer | Apr 22, 2025 |
| `squid_api_gateway` | Maintainer | Jul 25, 2025 |
| `squid-frontend` | Maintainer | Aug 28, 2025 |
| `squid-grafana-dashboard` | Maintainer | 11 months ago (relative label) |

## 2. Repository files (`develop` branch)

**GitLab breadcrumb:** `grc-cacib / AIFactory / Projects / SQUID AI / squid_ai_services`

**Selected branch:** `develop`

**Visible latest commit:** `fix(core): register main event loop to avoid mixing asyncio loops`  
**Author shown:** BELHOCINE  
**Short commit SHA shown:** `9e44d0b0`  
**Last commit time shown:** `2 weeks ago` (relative label)

### 2.1 File tree (visible root only)

```text
squid_ai_services/
├── .github/
├── ChunkingHandler/
├── DataHandler/
├── DataIngestionAPI/
├── DataParsingHandler/
├── DataTransformer/
├── EmbeddingHandler/
├── GraphTransformer/
├── SQL_scripts/
├── certs/
├── ci-templates/
├── ingestion_notebook/
├── whl_files/
├── .gitignore
├── .gitlab-ci.yml
└── README.md
```

> Only root-level entries visible in the screenshot are listed. Subdirectory contents were not shown.

### 2.2 Last commit details shown in the file list

| Name | Last commit message visible in GitLab | Last update shown |
|---|---|---|
| `.github` | Merge branch 'develop' into 'my_docs' | 1 year ago |
| `ChunkingHandler` | fix(constants): update LLM cert path to u... | 3 weeks ago |
| `DataHandler` | stream config for assignment vector stor... | 1 month ago |
| `DataIngestionAPI` | new cicd pipeline | 1 month ago |
| `DataParsingHandler` | new cicd pipeline | 1 month ago |
| `DataTransformer` | fix(core): register main event loop to avoi... | 2 weeks ago |
| `EmbeddingHandler` | new cicd pipeline | 1 month ago |
| `GraphTransformer` | new cicd pipeline | 1 month ago |
| `SQL_scripts` | comment sql script | 2 months ago |
| `certs` | combined ssl certificate | 3 months ago |
| `ci-templates` | ci(tag): add SSL_CERT_FILE and REQUES... | 2 weeks ago |
| `ingestion_notebook` | fix(constants): update LLM cert path to u... | 3 weeks ago |
| `whl_files` | Fixing redis fallback with new package a... | 11 months ago |
| `.gitignore` | Graph transformer code changes | 1 year ago |
| `.gitlab-ci.yml` | prod resolves tag from artifactory with ro... | 4 weeks ago |
| `README.md` | Merge develop with main | 1 year ago |

> Ellipses (`...`) in the table reflect commit messages actually truncated in the GitLab interface. Times are copied as relative labels from the screenshot, not recalculated.

---

# `README.md` — SquidAI Services – CI/CD Documentation

This README reflects the current GitLab CI/CD setup using a top-level orchestrator pipeline (`.gitlab-ci.yml`) that triggers environment-specific child pipelines. Pipelines are started manually from the GitLab UI (Run pipeline) — they are NOT auto-triggered by pushes or tags but by dropdown triggers. A version tag (semantic) is still recommended to anchor releases.

## 1. Modules

Supported deployable modules (`SELECTED_MODULE`):

- ChunkingHandler
- DataHandler
- DataParsingHandler
- EmbeddingHandler
- DataIngestionAPI
- DataTransformer
- GraphTransformer
- All (matrix builds every module)

Pipeline UI variable name: `SELECTED_MODULE` (previously `MODULE_NAME`). The orchestrator passes `MODULE_NAME` to child pipelines (or a matrix when All is selected).

**Important:**

- The expansion logic for the All option is centralized in the shared template (`.gitlab-squid-common.yml`) via `deploy_parallel_template`.
- When you add a new module you MUST:
  1. Add it to the list above (and to the `options` list in `.gitlab-ci.yml`).
  2. Update the `deploy_parallel_template` in the common template repository so the All selection includes it.
  3. Ensure kustomize overlays exist (`<MODULE_NAME>/overlays/<env>` in the manifests repo) and coverage path matches.

## 2. How Versioning Works

1. You are strongly recommended to create and push a semantic Git tag using command line / Git CLI in your code IDE (e.g. `1.4.2`):

   ```bash
   git tag -a 1.4.2 -m "Release version 1.4.2"
   git push origin 1.4.2
   ```

   > **Note:** Lightweight tags (e.g. `git tag 1.4.2`) do not store metadata like the tagger date, which is required for sorting and selecting the latest tag. Annotated tags are recommended for proper versioning.

2. The orchestrator (stage: `get_git_tags`) fetches all tags and picks the latest by tagger date. If a lightweight tag is present (Created using UI), it will have an empty tagger date and will be placed at the end of the list, effectively being ignored.

3. It creates:

   ```text
   APPLICATION_TAG_VERSION = <LATEST_TAG>_<DDMMYYYY>_<HOUR>
   Example: develop_1.4.2_15092025_13
   ```

4. This value is exported via a dotenv artifact and (for jobs with `forward: pipeline_variables: true`) propagated into child pipelines.

If you do not create a new tag, the previous one is reused with a new timestamp. Create a fresh semantic tag when code changes must be distinguished.

## 3. Starting a Pipeline (Orchestrator)

Steps:

1. Go to CI/CD > Pipelines > Run pipeline.
2. Choose the branch you want (feature / develop / main).
3. Set `SELECTED_MODULE` (single module or All).
4. Run pipeline.

(Workflow rules restrict execution to manual UI runs: `workflow.rules.if: $CI_PIPELINE_SOURCE == "web"`.)

## 4. Branch → Environment Path

| Branch Condition | `SELECTED_MODULE != All` | `SELECTED_MODULE == All` | Resulting Child Pipelines |
|---|---|---|---|
| Feature branch (not main, not develop) | `deploy_in_dev` | `deploy_in_dev_all` | Dev only |
| develop | `deploy_in_int` | `deploy_in_int_all` | Int only (no separate dev here) |
| main (automatic preprod) | `deploy_pprd_prd` | `deploy_pprd_prd` (manual) | Preprod (manual approval) |
| main (manual prod gate) | `deploy_pprd_prd` (manual) | `deploy_pprd_prd_all` (manual) | Prod (manual approval) |

Prod always follows a manual job on main (safety gate).

> **Transcription note:** The `main (automatic preprod)` row contains a mix of “automatic” in the condition and “manual”/“manual approval” in other cells, as displayed in the source screenshot. This apparent inconsistency is preserved rather than resolved by assumption.

## 5. Orchestrator Stages

1. `get_git_tags` (derive `APPLICATION_TAG_VERSION`)
2. `deploy_in_dev` / `deploy_in_dev_all` (feature branches) (auto)
3. `deploy_in_int` / `deploy_in_int_all` (develop) (auto)
4. `deploy_pprd_prd` / `deploy_pprd_prd_all` (main) (manual)

Each stage triggers one of:

- `.gitlab-ci-dev.yml`
- `.gitlab-ci-int.yml`
- `.gitlab-ci-pprd.yml`
- `.gitlab-ci-prd.yml`

## 6. Child Pipeline Summaries

### Dev (`.gitlab-ci-dev.yml`)

Stages: `build → sonar_check → build_docker → deploy_dev`

Behavior:

- Builds module with uv.
- Sonar analysis (quality gate wait disabled).
- Builds image into scratch registry.
- Updates kustomize overlay: `overlays/dev`.

### Int (`.gitlab-ci-int.yml`)

Stages: `build_docker → promote_staging → deploy_int`

Behavior:

- Builds & pushes scratch image.
- Promotes scratch → staging.
- Deploys via kustomize overlay: `overlays/int`.

### Preprod (`.gitlab-ci-pprd.yml`)

Stages: `build → sonar_check → build_docker → promote_stable → deploy_pprd`

Behavior:

- Builds new image.
- Promotes scratch → stable.
- Deploys overlay: `overlays/pre`.

### Prod (`.gitlab-ci-prd.yml`)

Stages: `build → sonar_check → build_docker → promote_stable → deploy_prd`

Behavior:

- Same pattern as preprod (build, sonar, build_docker).
- Promotes scratch → stable.
- Deploys overlay: `overlays/prod`.

## 7. Image Lifecycle

scratch registry (feature build) → staging registry (`promote_staging` on develop) → stable registry (`promote_stable` on main) → consumed by preprod / prod deployments.

## 8. Kustomize Deployment Pattern

Each deploy job:

1. Clones `$K8S_MANIFESTS_REPO_URL`.
2. Navigates to `ige-mais-deploy/<MODULE_NAME>/overlays/<env>`.
3. Runs:

   ```bash
   kustomize edit set image <module-lower>-img=<REGISTRY>/<module-lower>-img:<APPLICATION_TAG_VERSION
   ```

   > **Screenshot limit:** The right end of this command is cut off. The line above reproduces the visible prefix only; the expected closing `>` and any other trailing characters cannot be verified from the image.

4. Commits only if diff exists:
   - Message: `Rollout version <APPLICATION_TAG_VERSION>`

## 9. Quality & Coverage

- Sonar: currently only in dev & preprod & prod pipelines (int has no sonar stage).
- Coverage file path: `<MODULE_NAME>/coverage.xml`.
- `SONAR_SCANNER_OPTS` disables quality gate blocking.

## 10. Semantic Tagging (Recommended Practice)

Use MAJOR.MINOR.PATCH:

- MAJOR – The first number in the version (e.g., `1.0.0`) - breaking changes
- MINOR – The second number (e.g., `0.1.0`) - backward-compatible features
- PATCH – The third number (e.g., `0.0.1`) - fixes
- Feature branch tag should be in the format: `branch_name_0.0.0`
- Dev branch tag should be: `develop_0.0.0`
- Pre and prod branches should use: `release_0.0.0`

Examples:

```bash
git tag develop_2.1.0
git push origin develop_2.1.0
```

Alternatively, Gitlab UI can be utilised. Avoid pre-release identifiers for production (e.g. `1.0.0-beta`).

## 11. Summary

1. Run pipelines manually (web UI).
2. Select module (or All).
3. Version = latest semantic tag + timestamp.
4. Feature → dev; develop → int; main → preprod (auto) + prod (manual).
5. Promote images through registry tiers; deploy via kustomize commits.

## 12. Common Template Abstraction

Abstracted (centralized) pieces:

- `.get_git_tags_job`: Discovers latest tag, builds `APPLICATION_TAG_VERSION`, exports dotenv.
- `.deploy_parallel_template`: Expands the All selection into one child trigger per module.
- `.build_job`: Source code build (uv / deps / tests / coverage export).
- `.sonar_job`: Sonar scan setup and execution.
- `.build_docker_job`: Image build & push to proper registry tier (scratch in dev, etc.).
- `.deploy_job`: Kustomize patch + commit + (optionally) kubectl apply (depending on template logic).
- Promotion helpers (staging/stable) and any future shared scripts.

Thin wrappers in child pipelines (example: `ci-templates/.gitlab-ci-dev.yml`):

- `build` -> extends `.build_job`
- `sonar_check` -> extends `.sonar_job`
- `build_docker` -> extends `.build_docker_job`
- `deploy_dev` -> extends `.deploy_job` (adds `DEPLOY_STAGE`, `OVERLAY_ENV`, registry variable)

Why: Local YAML stays declarative (only environment-specific variables & stage ordering) while behavior stays versioned centrally.

Adding a new module (ALWAYS do all steps):

1. Add module name to Section 1 list.
2. Add to `SELECTED_MODULE` options in root `.gitlab-ci.yml`.
3. Update `deploy_parallel_template` in the shared `.gitlab-squid-common.yml` so All includes it.
4. Ensure kustomize overlays exist: `<module>/overlays/dev|int|pre|prod`.
5. Ensure coverage path `<ModuleName>/coverage.xml` or adjust sonar settings.
6. (If image naming differs) update image mapping logic in the common template.

---

## OCR boundaries

- This is a transcription **only of the portions visible in the nine screenshots**, not a direct download of the real repository.
- README headings 1–12 are represented to the extent shown; duplicated overlapping screenshots were consolidated.
- Some GitLab commit messages are visibly truncated.
- The Kustomize command extends beyond the right side of its screenshot, so its exact ending is not available.
- The project list and repo structure show only the visible screen state; they do not assert that other projects, files or folders do not exist.


---

### Document source : squid_api_gateway_repo_readme_ocr(1).md

# Squid AI — `squid_api_gateway` — GitLab Repository & README (OCR)

> Transcription of four photographed GitLab screenshots, organized in document order. Original English preserved. Duplicate portions across screenshots have been merged. Text that is cut off in a screenshot is explicitly marked as truncated. This is a transcription of the **visible GitLab UI and README**, not an inspection of repository source code.

## 1. Repository information

**Breadcrumb:** `grc-cacib / AIFactory / Projects / SQUID AI / squid_api_gateway`

- **Project:** `squid_api_gateway` (private)
- **Branch displayed:** `main`
- **Latest visible commit message:** `Merge branch 'main' of...` *(truncated in screenshot)*
- **Commit author:** `BELHOCINE`
- **Commit age displayed:** `1 week ago`
- **Short commit hash displayed:** `bbc2cd25`

### Files and folders displayed

| Name | Last commit (as displayed) | Last update (as displayed) |
|---|---|---|
| `.github` | `bug fixes for sources` | 10 months ago |
| `SquidAPIGateway` | `Merge branch 'main' of https://scm.saas...` *(truncated)* | 1 week ago |
| `ci-templates` | `cicd2` | 3 weeks ago |
| `docs` | `new cicd pipeline` | 1 month ago |
| `.gitignore` | `Develop` | 1 year ago |
| `.gitlab-ci.yml` | `cicd2` | 3 weeks ago |
| `README.md` | `Initial commit` | 1 year ago |

---

## 2. `README.md`

# squid_api_gateway

## Getting started

To make it easy for you to get started with GitLab, here's a list of recommended next steps.

Already a pro? Just edit this README.md and make it your own. Want to make it easy? Use the template at the bottom!

### Add your files

- [ ] Create or upload files
- [ ] Add files using the command line or push an existing Git repository with the following command:

```bash
cd existing_repo
git remote add origin https://scm.saas.cagip.group.gca/grc-cacib/aifactory/projects/squid-ai/squid_api_ga...  # [URL truncated on screen]
git branch -M main
git push -uf origin main
```

### Integrate with your tools

- [ ] Set up project integrations

### Collaborate with your team

- [ ] Invite team members and collaborators
- [ ] Create a new merge request
- [ ] Automatically close issues from merge requests
- [ ] Enable merge request approvals
- [ ] Set auto-merge

### Test and Deploy

Use the built-in continuous integration in GitLab.

- [ ] Get started with GitLab CI/CD
- [ ] Analyze your code for known vulnerabilities with Static Application Security Testing (SAST)
- [ ] Deploy to Kubernetes, Amazon EC2, or Amazon ECS using Auto DevOps
- [ ] Use pull-based deployments for improved Kubernetes management
- [ ] Set up protected environments

---

## Editing this README

When you're ready to make this README your own, just edit this file and use the handy template below (or feel free to structure it however you want - this is just a starting point!). Thanks to makeareadme.com for this template.

## Suggestions for a good README

Every project is different, so consider which of these sections apply to yours. The sections used in the template are suggestions for most open source projects. Also keep in mind that while a README can be too long and detailed, too long is better than too short. If you think your README is too long, consider utilizing another form of documentation rather than cutting out information.

### Name

Choose a self-explaining name for your project.

### Description

Let people know what your project can do specifically. Provide context and add a link to any reference visitors might be unfamiliar with. A list of Features or a Background subsection can also be added here. If there are alternatives to your project, this is a good place to list differentiating factors.

### Badges

On some READMEs, you may see small images that convey metadata, such as whether or not all the tests are passing for the project. You can use Shields to add some to your README. Many services also have instructions for adding a badge.

### Visuals

Depending on what you are making, it can be a good idea to include screenshots or even a video (you'll frequently see GIFs rather than actual videos). Tools like ttygif can help, but check out Asciinema for a more sophisticated method.

### Installation

Within a particular ecosystem, there may be a common way of installing things, such as using Yarn, NuGet, or Homebrew. However, consider the possibility that whoever is reading your README is a novice and would like more guidance. Listing specific steps helps remove ambiguity and gets people to using your project as quickly as possible. If it only runs in a specific context like a particular programming language version or operating system or has dependencies that have to be installed manually, also add a Requirements subsection.

### Usage

Use examples liberally, and show the expected output if you can. It's helpful to have inline the smallest example of usage that you can demonstrate, while providing links to more sophisticated examples if they are too long to reasonably include in the README.

### Support

Tell people where they can go for help. It can be any combination of an issue tracker, a chat room, an email address, etc.

### Roadmap

If you have ideas for releases in the future, it is a good idea to list them in the README.

### Contributing

State if you are open to contributions and what your requirements are for accepting them.

For people who want to make changes to your project, it's helpful to have some documentation on how to get started. Perhaps there is a script that they should run or some environment variables that they need to set. Make these steps explicit. These instructions could also be useful to your future self.

You can also document commands to lint the code or run tests. These steps help to ensure high code quality and reduce the likelihood that the changes inadvertently break something. Having instructions for running tests is especially helpful if it requires external setup, such as starting a Selenium server for testing in a browser.

### Authors and acknowledgment

Show your appreciation to those who have contributed to the project.

### License

For open source projects, say how it is licensed.

### Project status

If you have run out of energy or time for your project, put a note at the top of the README saying that development has slowed down or stopped completely. Someone may choose to fork your project or volunteer to step in as a maintainer or owner, allowing your project to keep going. You can also make an explicit request for maintainers.

---

## 3. Transcription notes

- The `README.md` shown in the screenshots is the **default GitLab getting-started / README template**, not project-specific API gateway documentation.
- The project repository **does** visibly contain `SquidAPIGateway/`, `ci-templates/`, `docs/`, and `.gitlab-ci.yml`, but the internal content of those files and folders is **not visible** in these screenshots.
- The `git remote add origin` URL is cut off horizontally in the screenshot; the ellipsis and inline note indicate an incomplete transcription, **not** a command to copy and run.
- Commit messages and relative dates were transcribed as displayed in the screenshots, not verified against the live repository.


---

### Document source : squid_frontend_repo_readme_ocr(1).md

# Squid AI — `squid-frontend` — README (OCR des captures)

> **Source :** 5 captures d'écran de GitLab, chemin visible : `AIFactory / Projects / SQUID AI / squid-frontend`.  
> **Méthode :** transcription du texte visible en anglais, sans compléter les parties absentes. Les passages identiques entre captures sont fusionnés.  
> **Marquage :** `[truncated in screenshot]` désigne une ligne ou un passage qui sort du cadre ; `[not visible]` indique un contenu non présent dans les captures.

---

## 1. Project-specific README — NPM setup

Before installing any packages, NPM must be configured to use the internal Artifactory registry and the CA-CIB proxy.

### 1. Create the `NpmTemplate.npmrc` file in `My_IT_Applications`

```ini
registry=https://registry.saas.cagip.group.gca/artifactory/api/npm/npm-remote/
init.author.name={UserFullName}
init.author.email={UserEmail}
email={UserEmail}
always-auth=true
strict-ssl=false
registry.saas.cagip.group.gca/artifactory/api/npm/npm-remote/:_auth={CertificateToken}
proxy=http://proxy-intra.par.emea.cib:8080/
noproxy=registry.saas.cagip.group.gca
https-proxy=http://proxy-intra.par.emea.cib:8080/
@inside-design:registry=https://registry.saas.cagip.group.gca/artifactory/api/npm/cacib-idc-npm-staging... [truncated in screenshot]
@insidedesign:registry=https://registry.saas.cagip.group.gca/artifactory/api/npm/cacib-idc-npm-stable-i... [truncated in screenshot]
cafile=C:\Users\{UserName}\ca-bundle.crt
```

> If the `proxy-intra` proxy does not work, try `http://proxy-fr.par.emea.cib:8080/` instead.

**OCR note:** The two scoped registry URLs are cut off horizontally in the screenshot. The configuration above is a transcription, **not** a copy-paste-ready `.npmrc` file.

### 2. Create and run the `ConfigureNpm.ps1` script

Visible beginning of the PowerShell code:

```powershell
function ReplaceTextInFile
(
    $FilePath,
    $OldText,
    $NewText
) {
    Write-Output ("Replacing text '" + $OldText + "' by '" + $NewText + "' in file '" + $FilePath + "' ... [truncated in screenshot]")
    $fileContent = [IO.File]::ReadAllText($FilePath)
    $fileContent = ($fileContent -Replace $OldText, $NewText)
    Set-Content -Path $FilePath -Value $fileContent -NoNewline
}

$currentScriptDirectory = Split-Path -Path $MyInvocation.MyCommand.Definition -Parent

#cd $env:USERPROFILE
```

> **OCR note:** The displayed `Write-Output` line extends beyond the screenshot. Its ending is not recoverable from the provided image. The remainder of the PowerShell script is not visible.

Create the `ConfigureNpm.ps1` file in the same folder as `NpmTemplate.npmrc` (see full content on the wiki above), then run it using **"Run with Administrative Privileges (Audited)"**.

The script will:

- Prompt for your Windows password to generate a Base64 authentication token
- Download the `ca-bundle.crt` certificate
- Generate the `%USERPROFILE%\.npmrc` file with your credentials

### 3. Verification

After the script completes:

- Check the content of `%USERPROFILE%\.npmrc`
- Verify that `cert.txt` does not exist (`cert.64.txt` is expected)
- Test with `npm install` inside the `SquidAIFrontend` folder

## 2. Project-specific README — Run the frontend

### Install and run

```bash
cd SquidAIFrontend
npm install
npm start
```

The app runs on:

- `http://localhost:4200`

### Local authentication bootstrap (using DEV session)

When developing locally, you can reuse an authenticated DEV session to start working quickly.

DEV environment used for sign-in:

- `https://squid-ai-dev.hors-prod.caas.cib.gca`

#### Steps

1. Open `https://squid-ai-dev.hors-prod.caas.cib.gca` in your browser and sign in.
2. Open browser DevTools on the DEV page.
3. Go to Application (or Storage) -> Local Storage.
4. Copy these keys and values from DEV local storage:
   - `id_token`
   - `id_token_expiration`
   - `user`
5. Open `http://localhost:4200/`.
6. Open DevTools on localhost -> Local Storage.
7. Paste the same 3 keys with the copied values:
   - `id_token`
   - `id_token_expiration`
   - `user`
8. Refresh localhost and start developing your feature.

#### Important notes

- Use only DEV values. Do not use PROD credentials or tokens.
- Tokens expire. If you are logged out locally, repeat the copy process.
- Keep tokens private and never commit them into source code or screenshots.

### Where to change local env values

Update local runtime values in:

- `SquidAIFrontend/public/assets/env.js`

Typical fields:

- `uSignApiAuthorizeUrl`
- `uSignApiTokenUrl`
- `uSignRedirectUri`
- `squidChatServiceApi`
- `squidGatewayApi`

---

## 3. Generic GitLab README template also visible

> The following sections are generic GitLab README guidance present below the project-specific instructions, **not confirmed technical documentation of the Squid AI frontend**.

### Add your files

- [ ] Create or upload files
- [ ] Add files using the command line or push an existing Git repository with the following command:

```bash
cd existing_repo
git remote add origin https://scm.saas.cagip.group.gca/grc-cacib/aifactory/projects/squid-ai/squid-front... [truncated in screenshot]
git branch -M main
git push -uf origin main
```

### Integrate with your tools

- [ ] Set up project integrations

### Collaborate with your team

- [ ] Invite team members and collaborators
- [ ] Create a new merge request
- [ ] Automatically close issues from merge requests
- [ ] Enable merge request approvals
- [ ] Set auto-merge

**[The intervening template sections are not visible in the provided screenshots.]**

### Badges

On some READMEs, you may see small images that convey metadata, such as whether or not all the tests are passing for the project. You can use Shields to add some to your README. Many services also have instructions for adding a badge.

### Visuals

Depending on what you are making, it can be a good idea to include screenshots or even a video (you'll frequently see GIFs rather than actual videos). Tools like ttygif can help, but check out Asciinema for a more sophisticated method.

### Installation

Within a particular ecosystem, there may be a common way of installing things, such as using Yarn, NuGet, or Homebrew. However, consider the possibility that whoever is reading your README is a novice and would like more guidance. Listing specific steps helps remove ambiguity and gets people to using your project as quickly as possible. If it only runs in a specific context like a particular programming language version or operating system or has dependencies that have to be installed manually, also add a Requirements subsection.

### Usage

Use examples liberally, and show the expected output if you can. It's helpful to have inline the smallest example of usage that you can demonstrate, while providing links to more sophisticated examples if they are too long to reasonably include in the README.

### Support

Tell people where they can go to for help. It can be any combination of an issue tracker, a chat room, an email address, etc.

### Roadmap

If you have ideas for releases in the future, it is a good idea to list them in the README.

### Contributing

State if you are open to contributions and what your requirements are for accepting them.

For people who want to make changes to your project, it's helpful to have some documentation on how to get started. Perhaps there is a script that they should run or some environment variables that they need to set. Make these steps explicit. These instructions could also be useful to your future self.

You can also document commands to lint the code or run tests. These steps help to ensure high code quality and reduce the likelihood ... **[continuation cut off in screenshot]**

---

## Notes on completeness

- This is an OCR of **5 screenshots**, not a direct export of the original README.
- Repeated passages (local auth steps) are included only once.
- The npm configuration and PowerShell code are partly clipped; do **not** use those snippets as complete executable setup instructions.
- The provided screenshots do **not** show the frontend's repository file tree, its framework/package versions, or its GitLab CI/CD configuration.
- No authentication tokens, passwords, or secret values appeared in these screenshots; the README discusses *names* of stored authentication fields and a credential template.


---

### Document source : squid_grafana_dashboard_repo_readme_ocr(1).md

# Squid AI — `squid-grafana-dashboard`

> **OCR des 6 captures d'écran GitLab fournies.** Texte anglais conservé, passages communs regroupés et ordre logique rétabli. Les points de suspension suivis de `[truncated in screenshot]` signalent le texte masqué dans les captures. Ce fichier ne constitue pas une extraction du dépôt GitLab lui-même.

## 1. GitLab repository

- **Project:** `squid-grafana-dashboard`
- **Group:** `grc-cacib / AIFactory / Projects / SQUID AI`
- **Branch shown:** `main`
- **Last commit displayed:** `6000d553`
- **Last commit message:** `Merge branch 'develop' into 'main'`
- **Author:** `CHAN`
- **Displayed commit age:** `3 months ago`

### Repository files and folders

```text
squid-grafana-dashboard/
├── SquidGrafanaDashboard/
├── ci-templates/
├── dashboards/
│   └── SquidAI/
├── documentation/
├── .env
├── .gitignore
├── .gitlab-ci.yml
└── README.md
```

| Name | Last commit (as visible) | Last update (as visible) |
|---|---|---|
| `SquidGrafanaDashboard/` | `fix(dashboard): replace hardcoded collec...` [truncated in screenshot] | 3 months ago |
| `ci-templates/` | `fixing ci for prd` | 6 months ago |
| `dashboards/SquidAI/` | `Update file squid-ai-pre.json` | 11 months ago |
| `documentation/` | `feat(readme): add documentation section...` [truncated in screenshot] | 4 months ago |
| `.env` | `first version` | 10 months ago |
| `.gitignore` | `first version` | 10 months ago |
| `.gitlab-ci.yml` | `first version` | 10 months ago |
| `README.md` | `feat(readme): add documentation section...` [truncated in screenshot] | 4 months ago |

---

## 2. README.md

# Grafana Docker Setup with Redis and Infinity Plugins

This setup includes Grafana with Redis datasource plugin and Infinity plugin for API data sources.

## Documentation

| Guide | Description |
|---|---|
| Local Windows installation (CACIB) | How to install Grafana locally on a CACIB Windows workstation and edit dashboards with this repository |

## Prerequisites

- Docker
- Docker Compose

## What's Included

- **Grafana:** Latest version with Redis and Infinity plugins pre-installed
- **Redis:** Alpine version for testing Redis datasource
- **Provisioning:** Pre-configured datasources for Redis and Infinity

## Quick Start

### 1. Start the Services

```bash
docker-compose up -d
```

### 2. Access Grafana

Open your browser and navigate to: http://localhost:3000

Default Credentials:

- **Username:** `admin`
- **Password:** `admin`

### 3. Verify Plugins

Go to Configuration → Plugins to verify:

- Redis Datasource
- Infinity Datasource

### 4. Verify Datasources

Go to Configuration → Data Sources to verify:

- Redis (connected to local Redis container)
- Infinity (for API queries)

## Commands

### Start services

```bash
docker-compose up -d
```

### Stop services

```bash
docker-compose down
```

### View logs

```bash
docker-compose logs -f grafana
```

### Restart services

```bash
docker-compose restart
```

### Stop and remove volumes (clean slate)

```bash
docker-compose down -v
```

## Configuration

### Grafana

- **Port:** 3000
- **Config file:** `grafana.ini`
- **Provisioning:** `provisioning/` directory

### Redis

- **Port:** 6379
- **Host:** `redis` (within Docker network)

## Plugins

The following plugins are automatically installed:

1. **redis-datasource:** For Redis data queries
2. **yesoreyeram-infinity-datasource:** For REST API, GraphQL, CSV, JSON queries

## Testing Redis Connection

Once Grafana is running, you can test the Redis connection by:

1. Adding some test data to Redis:

   ```bash
   docker exec -it redis redis-cli SET test:key "Hello from Redis"
   ```

2. In Grafana, use the Redis datasource to query the data

## Using Infinity Plugin

The Infinity plugin allows you to query:

- REST APIs
- JSON/CSV data
- GraphQL endpoints
- XML data

Example API to test with: https://jsonplaceholder.typicode.com/users

## Volumes

- **`grafana-storage`:** Persists Grafana data (dashboards, users, etc.)
- **`redis-data`:** Persists Redis data

## Network

All services communicate via the `grafana-network` bridge network.

## Customization

### Change Admin Password

Edit the `.env` file or `docker-compose.yml`:

```dotenv
GF_SECURITY_ADMIN_PASSWORD=your_secure_password
```

### Add More Plugins

Edit `docker-compose.yml` and add to `GF_INSTALL_PLUGINS`:

```dotenv
GF_INSTALL_PLUGINS=redis-datasource,yesoreyeram-infinity-datasource,another-plugin
```

## Troubleshooting

### Check if containers are running

```bash
docker-compose ps
```

### View Grafana logs

```bash
docker-compose logs grafana
```

### Check plugin installation

```bash
docker exec -it grafana grafana-cli plugins ls
```

### Restart with clean state

```bash
docker-compose down -v
docker-compose up -d
```

---

## Transcription notes

- All content above is transcribed or reorganized from the **six provided screenshots**; repeated portions between screenshots were merged.
- The source screenshots only show a portion of the GitLab interface and README; the full source repository, configuration files and linked documentation were not available.
- Some GitLab commit messages are clipped by the interface; they are explicitly marked as truncated.
- **Operational caution (not part of the original README):** `docker-compose down -v` deletes Docker Compose volumes. The credentials `admin` / `admin` are documented defaults; change the administrator password before using a real environment.
