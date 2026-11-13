# Jira Core (JRACOR) — Analyse du projet et prochaines étapes

J'ai repris les trois ressources, notamment la nouvelle version du guide Markdown qui contient maintenant les sections sur ton rôle technique, les accès, les risques, les questions à poser et le plan d'action.

## 1. Ce que je retiens du projet CA-CIB

Projet Jira Core — LEGAL IT

JRACOR

Version actuelle documentée

# 8.20.7

Version cible envisagée

# 9.12.4

Infrastructure

Windows Server 2019

Stack principale

Java / Tomcat / SQL / PowerShell

Authentification

USignOn / Kantega SAML

Objectif à terme

Migration Jira Cloud

Tu interviens sur une application Atlassian Jira Core auto-hébergée, pas sur un développement Java classique. Le travail porte principalement sur l'administration technique, la compatibilité des composants, les certificats, les intégrations, la préparation des opérations et les tests.

### Les quatre chantiers identifiés

| Chantier                           | Situation issue des documents                                          |
| ---------------------------------- | ---------------------------------------------------------------------- |
| Migration Jira 8.20.7 → 9.12.4     | À étudier ; pertinence encore à confirmer                              |
| Migration HTTP → HTTPS avec TRUSTY | UAT installée et testée ; certificat PROD encore attendu               |
| SSO USignOn / Kantega              | Fonctionnement UAT démontré ; validations de configuration à compléter |
| Purge des utilisateurs CMI         | Suivi par CMI ; confirmation de fin de purge attendue                  |

Ces statuts correspondent au compte rendu du 10 novembre 2026. Ils ne constituent pas une vérification en temps réel des environnements.

## 2. Ton rôle exact dans le projet

Lors de la réunion, la répartition a été clairement présentée :

- Toi : référent pour la partie technique.
- Laure : partie fonctionnelle et coordination.
- Nicolas Bouilhol : interlocuteur pour la documentation, l'installation existante et les questions techniques.
- CMI / sécurité : analyse des vulnérabilités et besoin de remédiation.

On attend de toi que tu étudies les étapes de migration, identifies les prérequis et reviennes vers Nicolas lorsque certains points ne sont pas clairs. La réunion n'établit pas que tu dois exécuter seul l'upgrade.

### Ce que tu vas réellement manipuler

| Domaine          | Technologies / outils                            |
| ---------------- | ------------------------------------------------ |
| Application      | Jira Core, Java                                  |
| Serveur          | Windows Server, Windows Services, Tomcat         |
| Configuration    | XML, JVM options, `server.xml`                   |
| Sécurité         | TLS, certificats PFX, Java `keytool`, truststore |
| Authentification | SAML, SSO, USignOn, Kantega                      |
| Base de données  | SQL, sauvegardes, restauration                   |
| Tests techniques | REST API, PowerShell, `curl`                     |
| Extensions Jira  | JSU, Jira Workflow Toolbox, Kantega              |

Il n'y a pas de développement Java applicatif spécifique confirmé dans les ressources. En revanche, le guide propose de créer des scripts de pré-check et de smoke tests pour sécuriser la migration.&#x20;

Jira_Core_Migration_Guide_OCR(1).md



## 3. Point important : faut-il vraiment migrer vers 9.12.4 ?

C'est la première question à résoudre avant d'établir le runbook définitif.

J'ai vérifié les références officielles d'Atlassian :

- Le support des produits Jira Server s'est terminé le 15 février 2024.&#x20;

  [image](https://www.google.com/s2/favicons?domain=https://www.atlassian.com\&sz=32)

  Atlassian+1

- Jira Core 9.12 est la dernière branche prenant en charge les licences Server.

- La documentation Jira Core 9.12 indique une fin de vie au 29 novembre 2025 pour cette branche.&#x20;

  [image](https://www.google.com/s2/favicons?domain=https://confluence.atlassian.com\&sz=32)

  Atlassian Documentation+1



Il faut donc distinguer la possibilité technique de faire l'upgrade et le fait d'obtenir une version réellement corrigée et maintenue.

Pour CA-CIB, il reste à confirmer la licence actuelle, les CVE identifiées par la sécurité, la version qui les corrige et le calendrier envisagé pour Jira Cloud.

Le simple fait de passer à 9.12.4 ne suffit pas à démontrer que le risque de sécurité sera traité.

## 4. Tes prochaines étapes concrètes

Voici l'ordre que je te recommande.

Étape 1 — Confirmer la décision de migration

Prioritaire

Demander à Laure et à l'équipe sécurité si la migration 9.12.4 est validée, et obtenir la liste des vulnérabilités à corriger.

Résultat : une version cible justifiée et un objectif sécurité clair.

Étape 2 — Vérifier tes accès techniques

Vérifier l'accès au wiki, à Jira UAT et aux écrans d'administration. Faire préciser si tu auras besoin d'un accès au serveur Windows et au repo Git contenant les scripts.

Point important : GRANT Admin IT, administrateur Jira et administrateur Windows sont trois habilitations distinctes.&#x20;

trasnscript(1).txt



Étape 3 — Organiser une passation technique avec Nicolas

Prévoir une session de partage d'écran sur l'UAT pour comprendre l'installation Jira, le service Windows, Tomcat, les plugins, les logs et les dossiers `JIRA_INSTALL` et `JIRA_HOME`.

Résultat : savoir où se trouvent les éléments techniques et qui peut intervenir dessus.

Étape 4 — Réaliser un inventaire technique

Examiner l'UAT, puis comparer avec la PROD, en lecture seule. Recenser les versions, la base de données, les licences, les plugins et les intégrations.

Résultat : un tableau des composants actuels, avec les compatibilités et les points bloquants.

Étape 5 — Préparer la migration en environnement de test

Une fois les décisions et prérequis validés, préparer les sauvegardes, la compatibilité des plugins, la procédure d'upgrade, les tests de non-régression et le rollback.

Aucun upgrade en PROD avant une répétition concluante, une restauration testée et l'approbation du changement.

Cet ordre adapte le plan d'action du nouveau guide.&#x20;

Jira_Core_Migration_Guide_OCR(1).md



## 5. Ce que tu dois vérifier lors de ta première connexion à Jira UAT

| Écran d'administration | Informations à récupérer                 |
| ---------------------- | ---------------------------------------- |
| System info            | Version Jira, Java, OS, DB               |
| Manage apps            | Liste et versions des plugins            |
| Jira update check      | Compatibilité avec la version cible      |
| Plan your upgrade      | Prérequis et avertissements              |
| General configuration  | Base URL                                 |
| Kantega SSO            | Configuration IdP/SSO, sans modification |

La priorité est de confirmer notamment l'utilisation du plugin Security Fields and Attachments, signalé comme potentiellement critique dans le guide.

S'il est réellement utilisé pour protéger des informations LEGAL, il faut tester la confidentialité avec des utilisateurs aux droits restreints avant de valider toute migration. L'exposition potentielle indiquée par Copilot est un risque à investiguer, pas un incident démontré.&#x20;

Jira_Core_Migration_Guide_OCR(1).md



## 6. Les questions les plus utiles à poser à Nicolas

Je te conseille de commencer par cinq questions techniques :

1. Où se trouve le repo Git `jira` mentionné dans la documentation, et contient-il les scripts d'installation et de migration ?
2. Comment accéder à l'administration de Jira UAT et, si nécessaire, au serveur Windows ?
3. Quelle est la base de données utilisée et qui s'occupe des sauvegardes et restaurations ?
4. L'UAT est-elle représentative de la production, notamment concernant les plugins, les workflows et les intégrations ?
5. Existe-t-il déjà un environnement ou une procédure prévue pour tester la migration avant la PROD ?

Voici un message Teams court pour commencer :

Bonjour Nicolas,

J'ai commencé à parcourir la documentation de migration Jira Core.

J'aimerais faire un point avec toi pour mieux comprendre l'installation actuelle, les accès techniques nécessaires et les prérequis de la migration.

J'ai aussi quelques questions concernant les plugins, la base de données et l'environnement UAT.

Est-ce qu'on pourrait prévoir un petit créneau pour regarder tout ça ensemble ?

Merci !

## 7. Le suivi que tu peux préparer

Voici une première matrice de travail. Les statuts sont fondés sur les documents et restent à actualiser après vérification.

| Sujet                           | Statut documentaire         | Prochaine vérification         |
| ------------------------------- | --------------------------- | ------------------------------ |
| Version cible                   | À valider                   | Laure / sécurité               |
| Licence Jira Server ou DC       | Inconnue                    | Administration Jira / licences |
| Accès Jira UAT                  | À vérifier                  | Connexion SSO et droits        |
| Accès serveur UAT               | À clarifier                 | Nicolas                        |
| Base de données                 | Inconnue                    | System info / DBA              |
| Plugins                         | Compatibilité non confirmée | Manage apps                    |
| Security Fields and Attachments | Utilisation inconnue        | Inventaire / métier            |
| Intégrations Smart'Eq           | Tests nécessaires           | Équipe Smart'Eq                |
| HTTPS UAT                       | Testé                       | Confirmer Base URL / SSO       |
| HTTPS PROD                      | Certificat attendu          | Laure / METIS                  |
| Backups / rollback              | Procédure à vérifier        | Nicolas / DBA                  |

## 8. Ce que je conserve pour nos prochaines discussions

J'ai mis à jour la mémoire du chantier CA-CIB / LEGAL / JRACOR, notamment sur ton rôle, l'architecture technique, les plugins, les certificats TRUSTY, le SSO, les risques, les accès et la feuille de route.

Le nouveau guide est plus complet que le précédent : il inclut les sections 10 à 15. Il reste néanmoins une retranscription de captures, et signale encore des passages manquants, notamment au début du document. Je distinguerai donc les faits confirmés des hypothèses issues de Copilot.

Pour ta prochaine journée de travail, ton objectif concret est simple : vérifier tes accès Jira UAT, parcourir les écrans d'administration disponibles et préparer les questions pour Nicolas. En parallèle, faire confirmer la version cible par Laure et la sécurité.

À ce stade, tu n'as pas besoin de modifier une configuration ni de lancer une migration. Tu dois d'abord comprendre précisément l'existant et identifier ce qui pourrait bloquer l'opération.

---

---

---


Oui, exactement. D'après la réunion, tu dois avoir un compte utilisateur Jira UAT ainsi que les droits d'administration Jira pour pouvoir commencer ton analyse technique.

Attention : les droits GRANT et les droits d'administration directement dans Jira sont distincts.

Voici un mail simple et professionnel à envoyer à Laure.

Bonjour Laure,

Dans le cadre du projet de migration Jira Core, pourrais-tu m'ajouter comme utilisateur sur l'environnement UAT et me donner les droits d'administration Jira nécessaires pour consulter la configuration et les plugins ?

Cela me permettra de commencer l'analyse technique et de préparer les étapes de migration.

Merci d'avance pour ton aide.

Bonne journée,

Tu peux commencer par l'UAT. Pour la PROD, il sera préférable de demander ensuite un accès adapté, éventuellement en lecture seule pour l'inventaire initial.

---

Bonjour Laure,

Pour pouvoir avancer sur l'analyse technique de la migration Jira Core, pourrais-tu me créer un compte sur l'environnement UAT et m'attribuer les droits d'administration Jira nécessaires ?

Merci d'avance pour ton aide.

Bonne journée,
