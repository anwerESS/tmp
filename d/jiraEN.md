“On my side, I’m looking into the Sonar check issue.
I’ve checked the pipelines and the job logs. The Node part runs fine, but the Java engine fails to validate the certificate when connecting to SonarQube.

I reran the job that passed last week, on the same commit, without any code changes, and it now fails too. So there may have been a certificate or environment change.

I think we should check with the Sonar or infrastructure team. Do you know who we can contact?”



------------------------------------------

# 1. Proposer le partage d’écran

“I can quickly share my screen and show you where it fails”
“Can you see my screen?”

# 2. Montrer le démarrage du scanner — lignes 38 à 44

Pointe la commande npx puis Server URL.  
“Here, we start the Sonar scanner through Node. And here you can see the SonarQube URL it connects to.”

# 3. Montrer les téléchargements réussis — lignes 45 à 53

Pointe les deux messages Download complete.  
“The Node bootstrap downloads the Java runtime and the scanner engine successfully. So this part completes without an error.”

# 4. Montrer le démarrage de Java et le blocage — lignes 54 à 59

Pointe javax.net.ssl.trustStore, puis Load global settings.  
“Here, Java is configured to use our certs/cacerts truststore. The scanner engine starts, but it fails when trying to load the settings from SonarQube.”

Descends ensuite jusqu’à SSLHandshakeException et PKIX path building failed :  
“This is the underlying error: Java cannot validate the certificate chain presented during the connection.”

Pour faire participer l’équipe :  
“Do you know if there has been any recent certificate or proxy change?”

# 5. Montrer l’exécution réussie de la semaine dernière

Ouvre l’ancien job vert et montre Load global settings (done), puis ANALYSIS SUCCESSFUL.  
“Now, if we look at last week’s run, this same step completed successfully, and the analysis finished.”

Montre ensuite le commit et la nouvelle tentative en échec :  
“I retried that job on exactly the same commit, with no code changes. The retry now fails with the certificate error we just saw.”

 # 6. Terminer par une proposition et une question

“That’s why I suspect something has changed in the environment, although we haven’t confirmed what yet. I think we should check the certificate chain received by the job and compare it with the Java truststore.”



