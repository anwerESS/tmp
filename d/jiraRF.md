De mon côté, je suis en train de regarder le problème sur le job Sonar Check.

J’ai analysé les pipelines et les logs du job. La partie Node fonctionne bien, mais le moteur Java n’arrive pas à valider le certificat lors de la connexion à SonarQube.

J’ai relancé un job qui avait réussi la semaine dernière, sur le même commit, sans aucune modification du code, et maintenant il échoue aussi.

Du coup, je pense qu’il y a peut-être eu un changement au niveau du certificat ou de l’environnement.

Je pense qu’il faudrait voir ça avec l’équipe Sonar ou l’équipe infrastructure. Vous savez qui on pourrait contacter ?

---

# 1. Proposer le partage d’écran

« Je peux partager rapidement mon écran pour vous montrer où ça bloque. »
« Vous voyez bien mon écran ? »

# 2. Montrer le démarrage du scanner — lignes 38 à 44

Pointe la commande `npx`, puis `Server URL`.
« Là, on lance le scanner Sonar via Node. Et ici, on voit l’URL du serveur SonarQube auquel il se connecte. »

# 3. Montrer les téléchargements réussis — lignes 45 à 53

Pointe les deux messages `Download complete`.
« Ici, Node arrive bien à télécharger le runtime Java et le moteur du scanner. Donc jusque-là, tout se passe bien, il n’y a pas d’erreur. »

# 4. Montrer le démarrage de Java et le blocage — lignes 54 à 59

Pointe `javax.net.ssl.trustStore`, puis `Load global settings`.
« Là, on voit que Java est configuré pour utiliser notre truststore `certs/cacerts`.
Le moteur du scanner démarre bien, mais ça bloque au moment de récupérer la configuration depuis SonarQube. »

Descends ensuite jusqu’à `SSLHandshakeException` et `PKIX path building failed` :
« Et là, on voit l’erreur exacte. En fait, Java n’arrive pas à valider la chaîne de certificats reçue lors de la connexion. »

Pour faire participer l’équipe :
« Vous savez s’il y a eu récemment un changement de certificat ou de proxy ? »

# 5. Montrer l’exécution réussie de la semaine dernière

Ouvre l’ancien job vert et montre `Load global settings (done)`, puis `ANALYSIS SUCCESSFUL`.
« Maintenant, si on regarde le job de la semaine dernière, on voit que cette étape passait bien et que l’analyse se terminait sans problème. »

Montre ensuite le commit et la nouvelle tentative en échec :
« J’ai relancé ce même job, sur exactement le même commit, sans aucun changement de code. Et cette fois, il échoue avec l’erreur de certificat qu’on vient de voir. »

# 6. Terminer par une proposition et une question

« C’est pour ça que je pense qu’il y a peut-être eu un changement au niveau de l’environnement, même si pour l’instant, on n’a pas encore confirmé la cause.

Je pense qu’il faudrait vérifier la chaîne de certificats reçue par le job et la comparer avec les certificats présents dans le truststore Java. »
