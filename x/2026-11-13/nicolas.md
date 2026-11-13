Bonjour Nicolas,

J'ai commencé à parcourir la documentation concernant la migration Jira Core de la version 8 vers la version 9.

J'aimerais faire un point avec toi pour mieux comprendre l'environnement technique actuel, les accès nécessaires et les étapes à prévoir pour la migration.

J'ai également quelques questions sur les plugins, la base de données et l'environnement UAT.

Est-ce qu'on pourrait prévoir un petit créneau pour regarder tout ça ensemble ?

Merci !

---

Pour ton point avec Nicolas sur la migration Jira Core, je te propose des questions simples, naturelles et professionnelles, faciles à poser à l'oral.

L'objectif est de comprendre l'environnement technique actuel, les accès nécessaires et les étapes de migration, sans rentrer trop vite dans des détails compliqués.

## 1. Message Teams pour organiser le point

Bonjour Nicolas,

J'ai commencé à parcourir la documentation concernant la migration Jira Core de la version 8 vers la version 9.

J'aimerais faire un point avec toi pour mieux comprendre l'environnement technique actuel, les accès nécessaires et les étapes à prévoir pour la migration.

J'ai également quelques questions sur les plugins, la base de données et l'environnement UAT.

Est-ce qu'on pourrait prévoir un petit créneau pour regarder tout ça ensemble ?

Merci !

## 2. Comment commencer la discussion

Tu peux démarrer naturellement :

> « Salut Nicolas, merci d'avoir pris un peu de temps.
>
> J'ai parcouru la documentation que tu nous as partagée. J'ai compris les grandes étapes de la migration, mais j'ai encore quelques questions sur l'environnement technique et sur la manière dont on va s'organiser. »

## 3. Questions à poser à Nicolas

### A. Architecture et environnement technique

1\. Comprendre l'installation actuelle

« Pour commencer, tu peux m'expliquer rapidement comment Jira Core est installé aujourd'hui ? »

2\. Comprendre les serveurs

« Si j'ai bien compris, Jira tourne sur un serveur Windows, avec Tomcat et Java, c'est bien ça ? »

3\. Identifier les environnements

« On a uniquement deux environnements, UAT et PROD, ou il y en a d'autres ? »

4\. Identifier les accès serveur

« Est-ce que j'aurai besoin d'accéder directement aux serveurs Windows, ou les accès admin Jira suffisent pour le moment ? »

### B. Accès et documentation

5\. Droits d'administration Jira

« J'ai demandé les accès Jira à Laure. Pour la partie technique, est-ce qu'il me faut des droits supplémentaires ? »

6\. Repo Git et scripts

« Dans la documentation, j'ai vu qu'il y avait des scripts PowerShell et un repo Git. Tu sais où je peux les retrouver ? »

7\. Voir l'environnement ensemble

« Est-ce qu'on peut regarder rapidement l'environnement UAT ensemble, pour que je voie comment tout est configuré ? »

### C. Base de données et plugins

8\. Base de données

« Concernant la base de données, tu sais si on utilise SQL Server, PostgreSQL ou autre chose ? »

9\. Sauvegardes

« Et pour les sauvegardes, c'est une équipe dédiée qui s'en occupe, ou c'est géré directement par l'équipe Jira ? »

10\. Compatibilité des plugins

« J'ai vu qu'on utilise notamment JSU, Jira Workflow Toolbox et Kantega. Est-ce que leur compatibilité avec la version 9 a déjà été vérifiée ? »

11\. Autres plugins

« Est-ce qu'il y a d'autres plugins importants à prendre en compte pour la migration ? »

Tu peux notamment mentionner Security Fields and Attachments, signalé dans le guide, et demander s'il est réellement utilisé.

### D. Migration Jira Core

12\. Version cible

« Est-ce que la migration vers la version 9.12.4 est toujours envisagée, ou est-ce qu'on attend encore la validation côté sécurité ? »

13\. Migration UAT

« Pour la migration, on va commencer directement sur l'UAT, ou on prévoit un environnement de test séparé ? »

14\. Retour d'expérience

« Tu avais déjà participé à une migration Jira il y a quelques années, c'est bien ça ? Est-ce qu'il y avait des difficultés particulières ? »

15\. Répartition des tâches

« Concrètement, pour la partie technique, quelles sont les actions que tu penses que je peux prendre en charge ? »

C'est une question importante pour clarifier ton rôle et tes responsabilités.

## 4. Expressions naturelles pendant la discussion

Pour éviter d'enchaîner uniquement les questions, voici des expressions utiles.

| Situation                             | Expression orale                                                  |
| ------------------------------------- | ----------------------------------------------------------------- |
| Demander une démonstration            | « Tu peux me montrer rapidement comment ça fonctionne ? »         |
| Demander une précision                | « Juste pour être sûr d'avoir bien compris… »                     |
| Vérifier une information              | « Donc, si je comprends bien, on doit d'abord… »                  |
| Tu n'as pas compris                   | « Attends, je n'ai pas bien compris ce point. »                   |
| Demander où trouver une configuration | « Et ça, on peut le retrouver où exactement ? »                   |
| Demander si c'est déjà fait           | « Ça a déjà été fait, ou c'est encore à prévoir ? »               |
| Identifier un responsable             | « Et cette partie-là, c'est quelle équipe qui la gère ? »         |
| Reporter une question                 | « D'accord, je vais regarder ça de mon côté. »                    |
| Prévoir un suivi                      | « Je vais creuser ce point et je reviendrai vers toi si besoin. » |

## 5. Comment terminer le point

Tu peux conclure de cette manière :

> « OK, merci Nicolas, c'est beaucoup plus clair.
>
> De mon côté, je vais continuer à regarder la documentation et commencer par vérifier les accès et les prérequis techniques.
>
> Je vais aussi noter les points qui restent à clarifier et je reviendrai vers toi si j'ai d'autres questions.
>
> Merci pour ton aide ! »

## 6. Les 6 questions que je te recommande en priorité

Si vous n'avez que 15 à 20 minutes, concentre-toi sur celles-ci :

Checklist pour ton échange

0/6

Tu peux me montrer rapidement comment Jira Core est installé aujourd'hui ?

Est-ce que j'aurai besoin d'accéder aux serveurs Windows, ou les droits admin Jira suffisent ?

Où est-ce que je peux retrouver les scripts PowerShell et le repo Git ?

Est-ce qu'on connaît déjà la base de données utilisée et la compatibilité des plugins ?

Est-ce que la version cible 9.12.4 est toujours envisagée ?

Concrètement, quelles sont les prochaines actions techniques que je peux prendre en charge ?

&#x20;Copier les questions

Mon conseil : pendant ce premier échange, privilégie la compréhension de l'existant et la répartition des responsabilités. Les questions détaillées sur les certificats, la reindexation Lucene et le rollback pourront venir ensuite, quand tu auras accès à l'environnement et aux scripts.
