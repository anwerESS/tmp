# Linux pour Développeur Java/Angular — Cours Complet

> **Format** : Cours en français, termes techniques en anglais.
> **Public cible** : Développeur backend Java/Spring Boot + frontend Angular, préparant un entretien technique.

---

## Table des matières

1. [Introduction à Linux](#1-introduction-à-linux)
2. [Architecture du système de fichiers (Filesystem)](#2-architecture-du-système-de-fichiers)
3. [Navigation et manipulation de fichiers](#3-navigation-et-manipulation-de-fichiers)
4. [Permissions et propriété des fichiers](#4-permissions-et-propriété-des-fichiers)
5. [Gestion des processus (Process Management)](#5-gestion-des-processus)
6. [Variables d'environnement et configuration Shell](#6-variables-denvironnement-et-configuration-shell)
7. [Gestion des packages (Package Management)](#7-gestion-des-packages)
8. [Réseau (Networking)](#8-réseau-networking)
9. [Outils de recherche et de filtrage (Search & Filter)](#9-outils-de-recherche-et-de-filtrage)
10. [Redirection, Pipes et Flux (I/O Redirection)](#10-redirection-pipes-et-flux)
11. [SSH et transfert de fichiers](#11-ssh-et-transfert-de-fichiers)
12. [Scripting Bash pour développeurs](#12-scripting-bash-pour-développeurs)
13. [Linux et Java — Commandes essentielles](#13-linux-et-java--commandes-essentielles)
14. [Linux et Angular — Commandes essentielles](#14-linux-et-angular--commandes-essentielles)
15. [Docker et Linux](#15-docker-et-linux)
16. [Logs et monitoring](#16-logs-et-monitoring)
17. [Gestion des services avec systemd](#17-gestion-des-services-avec-systemd)
18. [Questions / Réponses Orales](#18-questions--réponses-orales)

---

## 1. Introduction à Linux

### Qu'est-ce que Linux ?

Linux est un **système d'exploitation open-source** de type Unix, créé par Linus Torvalds en 1991. Il est massivement utilisé dans les environnements de serveurs, le cloud, les pipelines CI/CD et les conteneurs Docker.

Pour un développeur Java/Angular, Linux est incontournable car :
- Les serveurs de production tournent quasi-exclusivement sous Linux.
- Les outils de développement modernes (Docker, Kubernetes, Git, Maven, Node.js) s'y exécutent nativement.
- Les pipelines CI/CD (GitHub Actions, Jenkins, GitLab CI) utilisent des runners Linux.

### Les distributions (distros) principales

| Distribution | Usage typique |
|---|---|
| **Ubuntu** | Développement, desktop, serveurs |
| **Debian** | Serveurs stables, base de nombreuses distros |
| **CentOS / RHEL** | Entreprise, production |
| **Alpine** | Images Docker ultra-légères |
| **Arch Linux** | Usage avancé, cutting-edge |

### Le Shell

Le **shell** est l'interface en ligne de commande qui interprète vos commandes. Le plus utilisé est **Bash** (Bourne Again Shell). D'autres shells populaires : `zsh`, `fish`, `sh`.

```bash
# Vérifier le shell actif
echo $SHELL

# Vérifier la version de Bash
bash --version
```

---

## 2. Architecture du système de fichiers

Sous Linux, tout est fichier. Le système de fichiers part d'une racine unique notée `/`.

```
/
├── bin/        # Binaires essentiels (ls, cp, mv...)
├── boot/       # Fichiers de démarrage (kernel)
├── dev/        # Fichiers de périphériques (devices)
├── etc/        # Fichiers de configuration système
├── home/       # Répertoires utilisateurs (/home/anouar)
├── lib/        # Bibliothèques partagées
├── opt/        # Logiciels optionnels installés manuellement
├── proc/       # Système de fichiers virtuel des processus
├── root/       # Répertoire home du superutilisateur (root)
├── sbin/       # Binaires système (pour l'admin)
├── srv/        # Données des services (web, ftp...)
├── tmp/        # Fichiers temporaires (vidés au redémarrage)
├── usr/        # Applications et utilitaires utilisateur
│   ├── bin/    # Binaires utilisateur (/usr/bin/java, /usr/bin/node)
│   ├── lib/    # Bibliothèques
│   └── local/  # Logiciels compilés localement
└── var/        # Données variables (logs, bases de données, caches)
    └── log/    # Fichiers de logs
```

### Points clés pour un développeur

- `/etc/environment` → variables d'environnement globales
- `/var/log/` → journaux des applications (Tomcat, Nginx, etc.)
- `/opt/` → installation manuelle de JDK, serveurs d'applications
- `/tmp/` → fichiers temporaires (utiles pour les tests)
- `/proc/` → informations runtime sur les processus (CPU, mémoire)

---

## 3. Navigation et manipulation de fichiers

### Navigation de base

```bash
# Afficher le répertoire courant (Print Working Directory)
pwd

# Lister les fichiers
ls              # Liste simple
ls -l           # Format long (permissions, taille, date)
ls -la          # Inclut les fichiers cachés (commençant par .)
ls -lh          # Taille lisible (human-readable : KB, MB, GB)
ls -lt          # Trier par date de modification

# Changer de répertoire
cd /home/anouar/projects    # Chemin absolu
cd projects                 # Chemin relatif
cd ..                       # Remonter d'un niveau
cd ~                        # Aller dans le home de l'utilisateur
cd -                        # Revenir au répertoire précédent
```

### Création et suppression

```bash
# Créer des répertoires
mkdir mon-projet
mkdir -p src/main/java/com/example    # Créer les parents si nécessaire (-p)

# Créer des fichiers
touch application.properties
touch src/main/resources/application.yml

# Copier
cp fichier.txt copie.txt
cp -r dossier/ dossier-copie/         # Copie récursive (-r)

# Déplacer / Renommer
mv fichier.txt nouveau-nom.txt
mv fichier.txt /tmp/fichier.txt

# Supprimer
rm fichier.txt
rm -r dossier/                        # Suppression récursive
rm -rf dossier/                       # Force (sans confirmation) — ATTENTION
rmdir dossier/                        # Supprimer un répertoire vide uniquement
```

### Lecture de fichiers

```bash
# Afficher le contenu complet
cat application.properties

# Afficher avec numéros de ligne
cat -n fichier.txt

# Paginer (naviguer avec les flèches, q pour quitter)
less fichier-log-long.log
more fichier.txt

# Premières lignes (défaut : 10)
head application.log
head -n 50 application.log            # 50 premières lignes

# Dernières lignes (très utile pour les logs)
tail application.log
tail -n 100 application.log           # 100 dernières lignes
tail -f application.log               # Suivre en temps réel (follow) — indispensable en prod
tail -f -n 200 application.log        # 200 dernières lignes + suivi temps réel
```

### Liens symboliques (Symlinks)

```bash
# Créer un lien symbolique (raccourci)
ln -s /opt/jdk-21 /usr/local/java

# Vérifier où pointe un lien
ls -la /usr/local/java
readlink -f /usr/local/java
```

---

## 4. Permissions et propriété des fichiers

### Comprendre les permissions

Chaque fichier a des permissions pour 3 entités :
- **u** (user/owner) : le propriétaire
- **g** (group) : le groupe
- **o** (others) : les autres utilisateurs

Chaque entité a 3 droits :
- **r** (read) : lire → valeur **4**
- **w** (write) : écrire → valeur **2**
- **x** (execute) : exécuter → valeur **1**

```bash
# Exemple de sortie de ls -l
-rwxr-xr-- 1 anouar devteam 4096 Jun 14 10:00 start.sh
 │││││││││
 ││││││││└─ autres : r-- (4 = lecture seule)
 │││││││└── groupe : r-x (5 = lecture + exécution)
 ││││││└─── owner  : rwx (7 = lecture + écriture + exécution)
 │││││└──── nombre de liens
 └──── type : - (fichier), d (dossier), l (lien)
```

### Modifier les permissions (chmod)

```bash
# Notation octale (la plus courante)
chmod 755 start.sh      # rwxr-xr-x
chmod 644 config.yml    # rw-r--r--
chmod 600 secret.key    # rw------- (clé privée SSH)
chmod 777 script.sh     # rwxrwxrwx (DÉCONSEILLÉ en prod)

# Notation symbolique
chmod +x deploy.sh      # Ajouter le droit d'exécution à tous
chmod u+x deploy.sh     # Ajouter exécution pour le owner seulement
chmod g-w fichier.txt   # Retirer l'écriture au groupe
chmod o-r fichier.txt   # Retirer la lecture aux autres

# Récursif
chmod -R 755 /opt/myapp/
```

### Modifier le propriétaire (chown)

```bash
# Changer le propriétaire
chown anouar fichier.txt
chown anouar:devteam fichier.txt      # Owner + groupe
chown -R anouar:devteam /opt/myapp/  # Récursif
```

### Cas pratiques pour Java/Spring Boot

```bash
# Rendre le jar exécutable
chmod +x myapp.jar

# Script de démarrage
chmod 755 start.sh

# Fichier de config sensible (mots de passe DB)
chmod 600 application-prod.properties
chown root:root application-prod.properties
```

---

## 5. Gestion des processus (Process Management)

### Afficher les processus

```bash
# Snapshot des processus actifs
ps aux                          # Tous les processus, format détaillé
ps aux | grep java              # Filtrer les processus Java
ps aux | grep spring            # Filtrer les processus Spring Boot
ps -ef                          # Format alternatif

# Monitoring en temps réel
top                             # Interactif (q pour quitter)
htop                            # Version améliorée de top (à installer)

# Arborescence des processus
pstree
pstree -p                       # Avec les PIDs
```

### Gérer les processus

```bash
# Trouver le PID d'un processus
pgrep java
pgrep -a java                   # Avec la commande complète
pidof java

# Tuer un processus
kill PID                        # Signal SIGTERM (arrêt propre)
kill -9 PID                     # Signal SIGKILL (arrêt forcé)
kill -15 PID                    # Signal SIGTERM (explicite)
killall java                    # Tuer tous les processus nommés "java"
pkill -f "myapp.jar"            # Tuer par pattern de commande

# Exécution en arrière-plan (background)
java -jar myapp.jar &           # Lancer en background
nohup java -jar myapp.jar &     # Lancer et ignorer le signal HUP (résiste à la déconnexion)
nohup java -jar myapp.jar > app.log 2>&1 &   # Avec redirection des logs

# Gérer les jobs en background
jobs                            # Lister les jobs du shell courant
bg %1                           # Reprendre le job 1 en background
fg %1                           # Ramener le job 1 en foreground
Ctrl+Z                          # Suspendre le processus courant
Ctrl+C                          # Arrêter le processus courant
```

### Monitoring de la mémoire et du CPU

```bash
# Utilisation de la mémoire
free -h                         # RAM disponible/utilisée (human-readable)
free -m                         # En mégaoctets

# Espace disque
df -h                           # Espace disque par partition
df -h /                         # Espace sur la partition racine
du -sh /opt/myapp/              # Taille d'un répertoire
du -sh *                        # Taille de chaque élément du dossier courant
du -sh * | sort -h              # Trier par taille

# Informations CPU
nproc                           # Nombre de CPUs/cores
lscpu                           # Infos détaillées CPU
cat /proc/cpuinfo               # Infos raw du kernel

# Charge système (load average)
uptime
cat /proc/loadavg
```

---

## 6. Variables d'environnement et configuration Shell

### Variables d'environnement

```bash
# Afficher toutes les variables
env
printenv

# Afficher une variable spécifique
echo $JAVA_HOME
echo $PATH
echo $HOME
echo $USER

# Définir une variable (portée locale au shell)
export JAVA_HOME=/opt/jdk-21
export SPRING_PROFILES_ACTIVE=prod
export SERVER_PORT=8080

# Vérifier
echo $JAVA_HOME

# Supprimer une variable
unset JAVA_HOME
```

### Configuration persistante

Les variables persistantes se configurent dans des fichiers de démarrage du shell :

```bash
# Pour un utilisateur (Bash)
~/.bashrc           # Chargé à chaque shell interactif
~/.bash_profile     # Chargé à la connexion
~/.profile          # Chargé si .bash_profile absent

# Pour tous les utilisateurs (root requis)
/etc/environment    # Variables globales (syntaxe KEY=VALUE, sans export)
/etc/profile        # Script global
/etc/profile.d/     # Scripts globaux modulaires
```

```bash
# Exemple : Ajouter JAVA_HOME dans ~/.bashrc
echo 'export JAVA_HOME=/opt/jdk-21' >> ~/.bashrc
echo 'export PATH=$PATH:$JAVA_HOME/bin' >> ~/.bashrc

# Recharger sans redémarrer le terminal
source ~/.bashrc
# ou
. ~/.bashrc
```

### Le PATH

Le `PATH` est la variable qui indique au shell où chercher les exécutables.

```bash
# Afficher le PATH actuel
echo $PATH
# Ex: /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/opt/jdk-21/bin

# Ajouter un répertoire au PATH
export PATH=$PATH:/opt/maven/bin
export PATH=$PATH:/opt/node/bin

# Trouver où est installé un exécutable
which java
which node
which mvn
whereis java
```

---

## 7. Gestion des packages (Package Management)

### APT (Ubuntu/Debian)

```bash
# Mettre à jour la liste des packages
sudo apt update

# Mettre à jour les packages installés
sudo apt upgrade

# Rechercher un package
apt search openjdk
apt search nodejs

# Installer un package
sudo apt install openjdk-21-jdk
sudo apt install nodejs npm
sudo apt install maven
sudo apt install git
sudo apt install curl wget

# Supprimer un package
sudo apt remove openjdk-21-jdk
sudo apt purge openjdk-21-jdk    # + supprime les fichiers de config

# Nettoyer le cache
sudo apt autoremove              # Supprimer les dépendances inutilisées
sudo apt clean                   # Vider le cache de téléchargement

# Afficher les infos d'un package installé
dpkg -l | grep java
apt show openjdk-21-jdk
```

### YUM / DNF (CentOS/RHEL/Fedora)

```bash
# Installer
sudo dnf install java-21-openjdk-devel
sudo yum install java-11-openjdk-devel

# Rechercher
dnf search java

# Supprimer
sudo dnf remove java-21-openjdk-devel

# Lister les packages installés
rpm -qa | grep java
```

### Gestion de Java avec update-alternatives

```bash
# Lister les versions de Java installées
sudo update-alternatives --list java
sudo update-alternatives --config java

# Choisir la version par défaut
sudo update-alternatives --set java /opt/jdk-21/bin/java

# Vérifier la version active
java -version
javac -version
```

---

## 8. Réseau (Networking)

### Informations réseau

```bash
# Interfaces réseau et adresses IP
ip addr show
ip a                        # Raccourci
ifconfig                    # Ancien outil (peut nécessiter net-tools)

# Table de routage
ip route show
route -n                    # Ancien outil

# Connexions réseau actives
netstat -tuln               # Ports TCP/UDP en écoute
netstat -tulnp              # Avec les PIDs des processus
ss -tuln                    # Alternative moderne à netstat
ss -tulnp                   # Avec PIDs

# Vérifier si un port est utilisé (ex: 8080 pour Spring Boot)
ss -tulnp | grep 8080
netstat -tulnp | grep 8080
lsof -i :8080               # Quel processus écoute sur 8080
```

### Tests de connectivité

```bash
# Ping
ping google.com
ping -c 4 google.com        # 4 paquets seulement

# Tracer la route
traceroute google.com
tracepath google.com

# Résolution DNS
nslookup google.com
dig google.com
host google.com

# Tester une connexion TCP
telnet localhost 8080
nc -zv localhost 8080       # Netcat (plus moderne)
curl -v http://localhost:8080/actuator/health

# Tester une API REST (indispensable pour Spring Boot)
curl http://localhost:8080/api/users
curl -X POST http://localhost:8080/api/users \
  -H "Content-Type: application/json" \
  -d '{"name":"Anouar","email":"anouar@example.com"}'

# Télécharger des fichiers
wget https://example.com/file.zip
curl -O https://example.com/file.zip
curl -L -o output.zip https://example.com/file.zip   # Suivre les redirections
```

### Firewall (UFW sur Ubuntu)

```bash
# Vérifier le statut
sudo ufw status

# Autoriser un port
sudo ufw allow 8080
sudo ufw allow 443
sudo ufw allow 22             # SSH — TOUJOURS autoriser avant d'activer le firewall

# Bloquer un port
sudo ufw deny 3306            # Bloquer MySQL depuis l'extérieur

# Activer / désactiver
sudo ufw enable
sudo ufw disable
```

---

## 9. Outils de recherche et de filtrage (Search & Filter)

### find — Rechercher des fichiers

```bash
# Trouver par nom
find /opt -name "*.jar"
find /opt -name "application.properties"
find . -name "*.java"             # Fichiers .java dans le dossier courant

# Trouver par type
find /opt -type f -name "*.log"   # Fichiers uniquement
find /opt -type d -name "logs"    # Répertoires uniquement

# Trouver par taille
find /var/log -size +100M         # Fichiers > 100 MB
find /tmp -size -1k               # Fichiers < 1 KB

# Trouver par date de modification
find /var/log -mtime -1           # Modifié dans les dernières 24h
find /var/log -mtime +7           # Modifié il y a plus de 7 jours

# Exécuter une commande sur les résultats
find /var/log -name "*.log" -mtime +30 -exec rm {} \;
find . -name "*.class" -exec rm -f {} \;   # Supprimer les .class compilés
```

### grep — Rechercher dans le contenu

```bash
# Recherche de base
grep "ERROR" application.log
grep "NullPointerException" application.log

# Options courantes
grep -i "error" application.log         # Insensible à la casse
grep -r "TODO" src/                     # Récursif dans un dossier
grep -n "Exception" application.log     # Afficher les numéros de ligne
grep -v "DEBUG" application.log         # Exclure les lignes contenant "DEBUG"
grep -c "ERROR" application.log         # Compter les occurrences
grep -l "TODO" src/**/*.java            # Afficher les fichiers contenant le pattern

# Contexte autour de la correspondance
grep -A 5 "ERROR" application.log       # 5 lignes Après (After)
grep -B 5 "ERROR" application.log       # 5 lignes Avant (Before)
grep -C 5 "ERROR" application.log       # 5 lignes de contexte des 2 côtés

# Expressions régulières (Regex)
grep -E "^[0-9]{4}-[0-9]{2}" app.log   # Lignes commençant par une date
grep -E "ERROR|WARN" app.log            # ERROR ou WARN
grep -P "\d{3}-\d{4}" fichier.txt       # Regex Perl
```

### Autres outils de filtrage

```bash
# awk — Traitement de colonnes
awk '{print $1, $3}' fichier.txt         # Afficher la 1ère et 3ème colonne
awk -F: '{print $1}' /etc/passwd         # Avec séparateur : → liste des users
awk '/ERROR/{print NR, $0}' app.log      # Numéro de ligne + ligne si "ERROR"
ps aux | awk '{print $1, $2, $11}'       # User, PID, commande

# sed — Stream Editor (substitution)
sed 's/localhost/prod-server/g' config.properties   # Remplacer dans le flux
sed -i 's/localhost/prod-server/g' config.properties # Modifier le fichier directement
sed '/^#/d' application.properties       # Supprimer les lignes de commentaires
sed -n '10,20p' fichier.txt             # Afficher les lignes 10 à 20

# sort — Trier
sort fichier.txt                         # Tri alphabétique
sort -r fichier.txt                      # Tri inversé
sort -n fichier.txt                      # Tri numérique
sort -k 2 fichier.txt                    # Trier sur la 2ème colonne
du -sh * | sort -h                       # Trier les tailles de fichiers

# uniq — Dédupliquer (doit être trié d'abord)
sort fichier.txt | uniq
sort fichier.txt | uniq -c              # Compter les occurrences
sort fichier.txt | uniq -d              # Afficher uniquement les doublons

# wc — Compter
wc -l fichier.txt                       # Nombre de lignes
wc -w fichier.txt                       # Nombre de mots
grep -c "ERROR" app.log                 # Nombre d'erreurs dans les logs

# cut — Extraire des colonnes
cut -d: -f1 /etc/passwd                 # 1ère colonne, séparateur :
cut -d, -f2,4 data.csv                  # Colonnes 2 et 4 d'un CSV
```

---

## 10. Redirection, Pipes et Flux (I/O Redirection)

### Les flux standard

Linux a 3 flux standard :
- **stdin** (0) : entrée standard (clavier)
- **stdout** (1) : sortie standard (terminal)
- **stderr** (2) : sortie d'erreur (terminal)

### Redirection

```bash
# Rediriger stdout vers un fichier (écrase)
echo "Hello" > fichier.txt
java -jar myapp.jar > app.log

# Rediriger stdout vers un fichier (ajoute)
echo "Hello" >> fichier.txt
java -jar myapp.jar >> app.log

# Rediriger stderr
java -jar myapp.jar 2> errors.log

# Rediriger stdout ET stderr vers le même fichier
java -jar myapp.jar > app.log 2>&1
java -jar myapp.jar &> app.log        # Syntaxe Bash moderne

# Rediriger stderr vers /dev/null (ignorer les erreurs)
command 2>/dev/null

# Ignorer toute la sortie
command > /dev/null 2>&1
```

### Pipes (|)

Le **pipe** connecte le stdout d'une commande au stdin de la suivante.

```bash
# Exemples fondamentaux
ps aux | grep java                    # Filtrer les processus Java
cat app.log | grep "ERROR"            # Filtrer les erreurs
cat app.log | grep "ERROR" | wc -l   # Compter les erreurs

# Pipes plus complexes
ps aux | grep java | awk '{print $2}'  # Obtenir les PIDs Java
ls -la | sort -k5 -n | tail -10        # 10 plus grands fichiers
cat access.log | awk '{print $1}' | sort | uniq -c | sort -rn | head -20
# → Top 20 des IPs qui accèdent à l'application

# Utilisation avec Maven / Gradle
mvn test 2>&1 | tee test-results.log   # Afficher ET sauvegarder
mvn clean package | grep -E "ERROR|BUILD"
```

---

## 11. SSH et transfert de fichiers

### SSH — Connexion sécurisée

```bash
# Connexion basique
ssh user@server.example.com
ssh anouar@192.168.1.100
ssh -p 2222 user@server.com           # Port personnalisé

# Connexion avec clé privée
ssh -i ~/.ssh/id_rsa user@server.com
ssh -i ~/.ssh/deploy_key anouar@prod-server.com

# Exécuter une commande à distance
ssh user@server.com 'java -version'
ssh user@server.com 'systemctl status myapp'
ssh user@server.com 'tail -f /var/log/myapp/application.log'
```

### Génération de clés SSH

```bash
# Générer une paire de clés RSA (4096 bits)
ssh-keygen -t rsa -b 4096 -C "anouar@example.com"

# Générer une clé ED25519 (plus moderne, recommandée)
ssh-keygen -t ed25519 -C "anouar@example.com"

# Copier la clé publique sur un serveur
ssh-copy-id user@server.com
ssh-copy-id -i ~/.ssh/id_ed25519.pub user@server.com

# Clé publique (à partager)
cat ~/.ssh/id_rsa.pub

# Clé privée (NE JAMAIS PARTAGER)
cat ~/.ssh/id_rsa
```

### Transfert de fichiers

```bash
# SCP — Secure Copy
scp fichier.txt user@server.com:/home/user/    # Envoyer
scp user@server.com:/home/user/logs.zip .      # Récupérer
scp -r dossier/ user@server.com:/opt/          # Dossier entier
scp -P 2222 fichier.txt user@server.com:/tmp/  # Port personnalisé

# SFTP — Interactif
sftp user@server.com
sftp> ls
sftp> get fichier.txt
sftp> put app.jar
sftp> bye

# rsync — Synchronisation (plus efficace que scp pour gros volumes)
rsync -avz /local/path/ user@server.com:/remote/path/
rsync -avz --progress myapp.jar user@server.com:/opt/deploy/
rsync -avz --delete src/ user@server.com:/opt/src/  # Synchronisation miroir
```

### Configuration SSH (~/.ssh/config)

```bash
# Créer/éditer le fichier de config SSH
nano ~/.ssh/config
```

```
Host prod
    HostName prod-server.example.com
    User anouar
    IdentityFile ~/.ssh/id_rsa_prod
    Port 22

Host staging
    HostName staging.example.com
    User deploy
    IdentityFile ~/.ssh/id_rsa_staging
```

```bash
# Utilisation simplifiée
ssh prod
ssh staging
scp fichier.txt prod:/opt/app/
```

---

## 12. Scripting Bash pour développeurs

### Bases du scripting

```bash
#!/bin/bash
# Shebang : indique l'interpréteur

# Variables
APP_NAME="myapp"
JAR_PATH="/opt/$APP_NAME/$APP_NAME.jar"
LOG_DIR="/var/log/$APP_NAME"

# Affichage
echo "Démarrage de $APP_NAME"
echo "JAR : $JAR_PATH"

# Commentaires
# Ceci est un commentaire

# Lecture d'entrée utilisateur
read -p "Entrez le profil Spring (prod/dev) : " PROFILE
echo "Profil sélectionné : $PROFILE"
```

### Conditions et boucles

```bash
#!/bin/bash

# Condition if/else
if [ -f "$JAR_PATH" ]; then
    echo "JAR trouvé : $JAR_PATH"
else
    echo "ERREUR : JAR introuvable !"
    exit 1
fi

# Condition avec AND / OR
if [ -f "$JAR_PATH" ] && [ -d "$LOG_DIR" ]; then
    echo "Prêt à démarrer"
fi

# Comparaisons numériques
PORT=8080
if [ $PORT -gt 1024 ]; then
    echo "Port valide"
fi
# -eq (equal), -ne (not equal), -lt (less than), -le, -gt, -ge

# Boucle for
for env in dev staging prod; do
    echo "Déploiement sur : $env"
done

# Boucle while
COUNTER=0
while [ $COUNTER -lt 5 ]; do
    echo "Tentative $COUNTER"
    COUNTER=$((COUNTER + 1))
done

# Boucle sur des fichiers
for file in /var/log/*.log; do
    echo "Traitement : $file"
    gzip "$file"
done
```

### Script de déploiement Spring Boot

```bash
#!/bin/bash

# Script de déploiement pour une application Spring Boot
set -e  # Arrêter en cas d'erreur

APP_NAME="myapp"
JAR_PATH="/opt/$APP_NAME/$APP_NAME.jar"
LOG_FILE="/var/log/$APP_NAME/application.log"
PROFILE="${1:-prod}"              # Utiliser l'argument ou "prod" par défaut
PID_FILE="/tmp/$APP_NAME.pid"

echo "===================================="
echo " Déploiement de $APP_NAME"
echo " Profil : $PROFILE"
echo "===================================="

# 1. Arrêter l'instance en cours si elle tourne
if [ -f "$PID_FILE" ]; then
    PID=$(cat "$PID_FILE")
    if kill -0 "$PID" 2>/dev/null; then
        echo "Arrêt du processus existant (PID: $PID)..."
        kill "$PID"
        sleep 3
    fi
    rm -f "$PID_FILE"
fi

# 2. Vérifier que le JAR existe
if [ ! -f "$JAR_PATH" ]; then
    echo "ERREUR : $JAR_PATH introuvable."
    exit 1
fi

# 3. Créer le répertoire de logs si nécessaire
mkdir -p "$(dirname $LOG_FILE)"

# 4. Démarrer l'application
echo "Démarrage de l'application..."
nohup java \
    -Xms512m \
    -Xmx1024m \
    -Dspring.profiles.active=$PROFILE \
    -Dserver.port=8080 \
    -jar "$JAR_PATH" \
    > "$LOG_FILE" 2>&1 &

echo $! > "$PID_FILE"
echo "Application démarrée (PID: $(cat $PID_FILE))"
echo "Logs : $LOG_FILE"

# 5. Vérifier le démarrage
sleep 10
if kill -0 "$(cat $PID_FILE)" 2>/dev/null; then
    echo "✓ Application démarrée avec succès"
else
    echo "✗ ERREUR : l'application n'a pas démarré"
    cat "$LOG_FILE"
    exit 1
fi
```

### Fonctions Bash

```bash
#!/bin/bash

# Définir une fonction
check_port() {
    local PORT=$1
    if ss -tuln | grep -q ":$PORT "; then
        echo "Port $PORT : EN UTILISATION"
        return 0
    else
        echo "Port $PORT : LIBRE"
        return 1
    fi
}

# Appeler la fonction
check_port 8080
check_port 3000

# Fonctions avec valeur de retour
get_pid() {
    local APP=$1
    pgrep -f "$APP" | head -1
}

PID=$(get_pid "myapp.jar")
echo "PID trouvé : $PID"
```

---

## 13. Linux et Java — Commandes essentielles

### Installation et gestion du JDK

```bash
# Installer OpenJDK
sudo apt install openjdk-21-jdk
sudo apt install openjdk-17-jdk

# Vérifier l'installation
java -version
javac -version
echo $JAVA_HOME

# Choisir la version par défaut
sudo update-alternatives --config java
sudo update-alternatives --config javac

# Trouver JAVA_HOME dynamiquement
dirname $(dirname $(readlink -f $(which java)))
```

### Lancer une application Spring Boot

```bash
# Avec java -jar
java -jar target/myapp.jar

# Avec profil Spring
java -jar -Dspring.profiles.active=prod target/myapp.jar
SPRING_PROFILES_ACTIVE=prod java -jar target/myapp.jar

# Avec port personnalisé
java -jar -Dserver.port=9090 target/myapp.jar

# Configuration mémoire JVM
java -Xms256m -Xmx512m -jar target/myapp.jar     # Min 256MB, Max 512MB
java -XX:+UseG1GC -jar target/myapp.jar           # Garbage Collector G1

# Combinaison complète (production)
java \
  -Xms512m \
  -Xmx1024m \
  -XX:+UseG1GC \
  -XX:MaxGCPauseMillis=200 \
  -Dspring.profiles.active=prod \
  -Dserver.port=8080 \
  -Dlogging.file.path=/var/log/myapp \
  -jar /opt/myapp/myapp.jar

# Lancer avec Maven
mvn spring-boot:run
mvn spring-boot:run -Dspring-boot.run.profiles=dev

# Build avec Maven
mvn clean package
mvn clean package -DskipTests
mvn clean package -Pprod                # Avec profil Maven
```

### Diagnostics Java sous Linux

```bash
# Lister les processus Java
jps                                    # Java Virtual Machines actives
jps -l                                 # Avec le nom complet de la classe main
jps -v                                 # Avec les arguments JVM

# Informations sur le heap
jmap -heap <PID>                       # Statistiques heap
jmap -histo <PID>                      # Histogramme des objets en mémoire
jmap -dump:format=b,file=heap.hprof <PID>  # Dump heap pour analyse

# Thread dump (pour diagnostiquer les deadlocks)
jstack <PID>                           # Thread dump vers stdout
jstack <PID> > thread-dump.txt         # Sauvegarder le thread dump
kill -3 <PID>                          # Signal SIGQUIT → thread dump dans stdout

# Monitoring JVM
jstat -gc <PID> 1000 10               # Stats GC toutes les secondes, 10 fois
jstat -gcutil <PID> 1000              # Utilisation GC en pourcentage

# Connexion à JConsole / VisualVM à distance
java -Dcom.sun.management.jmxremote \
     -Dcom.sun.management.jmxremote.port=9090 \
     -Dcom.sun.management.jmxremote.authenticate=false \
     -Dcom.sun.management.jmxremote.ssl=false \
     -jar myapp.jar
```

### Variables d'environnement Spring Boot

```bash
# Spring Boot lit automatiquement les variables d'environnement
# Convention : propriété spring.datasource.url → variable SPRING_DATASOURCE_URL

export SPRING_DATASOURCE_URL=jdbc:postgresql://prod-db:5432/mydb
export SPRING_DATASOURCE_USERNAME=appuser
export SPRING_DATASOURCE_PASSWORD=secret123
export SPRING_PROFILES_ACTIVE=prod
export SERVER_PORT=8080

java -jar myapp.jar
```

---

## 14. Linux et Angular — Commandes essentielles

### Installation de Node.js et Angular CLI

```bash
# Installer Node.js via NodeSource (version LTS)
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
sudo apt install -y nodejs

# Vérifier
node --version
npm --version

# Installer Angular CLI globalement
npm install -g @angular/cli
ng version

# Mettre à jour Angular CLI
npm update -g @angular/cli
```

### Commandes npm / Angular dans un contexte Linux

```bash
# Installation des dépendances
npm install
npm ci                             # Installation propre (CI/CD) — préféré en production

# Build de production
ng build --configuration=production
npm run build

# Build avec variables d'environnement Linux
export API_URL=https://api.example.com
ng build --configuration=production

# Serveur de développement
ng serve
ng serve --host 0.0.0.0 --port 4200    # Accessible depuis d'autres machines

# Tests
ng test --watch=false                   # Tests unitaires sans mode watch (CI)
ng test --no-watch --code-coverage      # Avec couverture de code
ng e2e                                   # Tests end-to-end

# Linting
ng lint
npm run lint

# Analyse des bundles
npm install -g source-map-explorer
source-map-explorer dist/myapp/*.js
```

### Servir le build Angular avec Nginx sur Linux

```bash
# Installer Nginx
sudo apt install nginx

# Copier le build Angular
sudo cp -r dist/myapp/* /var/www/html/

# Configuration Nginx pour Angular (SPA routing)
sudo nano /etc/nginx/sites-available/myapp
```

```nginx
server {
    listen 80;
    server_name example.com;
    root /var/www/html;
    index index.html;

    # Angular SPA : toujours retourner index.html pour le routing
    location / {
        try_files $uri $uri/ /index.html;
    }

    # Proxy vers l'API Spring Boot
    location /api/ {
        proxy_pass http://localhost:8080/;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

```bash
# Activer le site et redémarrer Nginx
sudo ln -s /etc/nginx/sites-available/myapp /etc/nginx/sites-enabled/
sudo nginx -t                           # Tester la configuration
sudo systemctl restart nginx
sudo systemctl status nginx
```

### Gestion des permissions Node/npm

```bash
# Éviter les erreurs de permission avec npm global
mkdir ~/.npm-global
npm config set prefix '~/.npm-global'
echo 'export PATH=~/.npm-global/bin:$PATH' >> ~/.bashrc
source ~/.bashrc

# Corriger les permissions d'un projet Angular
chown -R $USER:$USER /opt/angular-app/
chmod -R 755 /opt/angular-app/
```

---

## 15. Docker et Linux

### Commandes Docker essentielles

```bash
# Images
docker images                           # Lister les images locales
docker pull openjdk:21-jre-slim         # Télécharger une image
docker build -t myapp:1.0 .             # Builder une image depuis le Dockerfile
docker build -t myapp:1.0 -f Dockerfile.prod .   # Dockerfile spécifique
docker rmi myapp:1.0                    # Supprimer une image
docker image prune                      # Nettoyer les images non utilisées

# Conteneurs
docker run -d --name myapp -p 8080:8080 myapp:1.0    # Lancer en background
docker run -it ubuntu:22.04 /bin/bash                # Interactif
docker run --rm myapp:1.0                            # Supprimer après l'arrêt

# Variables d'environnement dans Docker
docker run -d \
    -e SPRING_PROFILES_ACTIVE=prod \
    -e SPRING_DATASOURCE_URL=jdbc:postgresql://db:5432/mydb \
    -p 8080:8080 \
    --name myapp \
    myapp:1.0

# Gestion des conteneurs
docker ps                               # Conteneurs en cours
docker ps -a                            # Tous les conteneurs (arrêtés inclus)
docker stop myapp                       # Arrêt propre
docker start myapp                      # Redémarrer
docker restart myapp
docker rm myapp                         # Supprimer un conteneur arrêté
docker rm -f myapp                      # Forcer la suppression

# Logs Docker
docker logs myapp
docker logs -f myapp                    # Suivre en temps réel
docker logs --tail 100 myapp            # 100 dernières lignes

# Entrer dans un conteneur en cours
docker exec -it myapp /bin/bash
docker exec -it myapp /bin/sh           # Si bash absent (Alpine)

# Copier des fichiers
docker cp myapp:/opt/logs/app.log .     # Extraire du conteneur
docker cp config.yml myapp:/app/        # Injecter dans le conteneur
```

### Dockerfile pour Spring Boot

```dockerfile
# Dockerfile optimisé pour Spring Boot
FROM openjdk:21-jre-slim AS runtime

WORKDIR /app

# Copier le JAR
COPY target/myapp.jar app.jar

# Port exposé
EXPOSE 8080

# Utilisateur non-root (sécurité)
RUN addgroup --system appgroup && adduser --system appuser --ingroup appgroup
USER appuser

# Point d'entrée
ENTRYPOINT ["java", "-Xms256m", "-Xmx512m", "-jar", "/app/app.jar"]
```

### Docker Compose

```bash
# Lancer tous les services définis dans docker-compose.yml
docker compose up -d

# Arrêter
docker compose down

# Voir les logs
docker compose logs -f

# Rebuild
docker compose up -d --build

# Nettoyer complètement
docker compose down -v --remove-orphans
docker system prune -a
```

---

## 16. Logs et monitoring

### Gestion des logs applicatifs

```bash
# Suivre les logs Spring Boot en temps réel
tail -f /var/log/myapp/application.log

# Filtrer les erreurs en temps réel
tail -f application.log | grep -E "ERROR|WARN"

# Compter les erreurs dans les logs du jour
grep "$(date '+%Y-%m-%d')" application.log | grep "ERROR" | wc -l

# Rechercher une exception spécifique
grep -A 20 "NullPointerException" application.log

# Compresser les vieux logs
gzip /var/log/myapp/application.log.2024-01-01
find /var/log/myapp -name "*.log" -mtime +30 -exec gzip {} \;

# Rotation des logs (logrotate)
cat /etc/logrotate.d/myapp
```

### Journald (systemd)

```bash
# Voir les logs d'un service systemd
journalctl -u myapp.service
journalctl -u myapp.service -f                     # Suivre en temps réel
journalctl -u myapp.service --since "1 hour ago"   # Dernière heure
journalctl -u myapp.service -n 100                 # 100 dernières lignes
journalctl -u myapp.service --since "2024-01-01" --until "2024-01-02"

# Logs de tout le système
journalctl -f                                      # Tous les logs en temps réel
journalctl -p err                                  # Erreurs uniquement
journalctl --since today
```

### Monitoring système

```bash
# CPU et mémoire
vmstat 1                       # Stats VM toutes les secondes
iostat -x 1                    # Stats I/O disque
sar -u 1 10                    # CPU utilization

# Mémoire détaillée
cat /proc/meminfo
free -h

# Disque
df -h
iotop                          # Top des I/O disque (à installer)
lsblk                          # Lister les disques et partitions

# Réseau
nethogs                        # Bande passante par processus (à installer)
iftop                          # Monitoring réseau en temps réel
```

---

## 17. Gestion des services avec systemd

### Créer un service systemd pour Spring Boot

```bash
# Créer le fichier de service
sudo nano /etc/systemd/system/myapp.service
```

```ini
[Unit]
Description=My Spring Boot Application
After=network.target

[Service]
Type=simple
User=appuser
Group=appgroup
WorkingDirectory=/opt/myapp
ExecStart=/usr/bin/java -Xms512m -Xmx1024m -Dspring.profiles.active=prod -jar /opt/myapp/myapp.jar
Restart=on-failure
RestartSec=10
SuccessExitStatus=143
StandardOutput=journal
StandardError=journal
SyslogIdentifier=myapp

# Variables d'environnement
Environment="SPRING_DATASOURCE_URL=jdbc:postgresql://localhost:5432/mydb"
Environment="SPRING_PROFILES_ACTIVE=prod"
EnvironmentFile=/opt/myapp/app.env

[Install]
WantedBy=multi-user.target
```

### Commandes systemd

```bash
# Recharger la configuration systemd après modification
sudo systemctl daemon-reload

# Activer le service au démarrage
sudo systemctl enable myapp

# Désactiver le service au démarrage
sudo systemctl disable myapp

# Démarrer / Arrêter / Redémarrer
sudo systemctl start myapp
sudo systemctl stop myapp
sudo systemctl restart myapp
sudo systemctl reload myapp           # Rechargement doux (si supporté)

# Vérifier le statut
sudo systemctl status myapp

# Voir les logs du service
journalctl -u myapp -f

# Lister tous les services
systemctl list-units --type=service
systemctl list-units --type=service --state=running
```

---

## 18. Questions / Réponses Orales

### Section 1 : Fondamentaux Linux

---

**Q1 : Quelle est la différence entre un chemin absolu et un chemin relatif ?**

Un **chemin absolu** part toujours de la racine du système `/`, par exemple `/home/anouar/projects/myapp`. Il est valable quel que soit le répertoire courant. Un **chemin relatif** est exprimé par rapport au répertoire dans lequel on se trouve, par exemple `projects/myapp` ou `../autre-projet`. En développement, on utilise les chemins relatifs dans les scripts pour les rendre portables, et les chemins absolus pour les fichiers de configuration système.

---

**Q2 : Que fait la commande `chmod 755 deploy.sh` ?**

La commande `chmod 755` attribue au fichier les permissions suivantes : le **propriétaire** peut lire, écrire et exécuter (7 = 4+2+1), le **groupe** peut lire et exécuter (5 = 4+0+1), et les **autres** peuvent lire et exécuter (5 = 4+0+1). C'est la permission typique pour un script de déploiement : le propriétaire peut le modifier, et tout le monde peut l'exécuter. Pour un fichier de configuration avec des mots de passe, j'utiliserais plutôt `chmod 600` pour que seul le propriétaire puisse y accéder.

---

**Q3 : Comment trouver tous les processus Java en cours d'exécution ?**

J'ai plusieurs options. La plus directe est `jps` qui liste les Java Virtual Machines actives avec leurs PIDs. Je peux aussi faire `ps aux | grep java` pour avoir les détails complets de chaque processus Java (arguments JVM, chemin du JAR, etc.). Si je cherche un processus Spring Boot spécifique, j'utilise `pgrep -f "myapp.jar"` ou `ps aux | grep myapp`. Pour tuer proprement un processus Java, j'utilise d'abord `kill PID` (signal SIGTERM pour un arrêt propre), et seulement en dernier recours `kill -9 PID` (SIGKILL).

---

**Q4 : Quelle est la différence entre `>` et `>>` en Bash ?**

Le `>` **écrase** le fichier de destination s'il existe, et le crée sinon. Le `>>` **ajoute** à la fin du fichier existant sans l'effacer. Par exemple, `java -jar app.jar > app.log` écrase le fichier de log à chaque redémarrage, ce qui peut être voulu en développement. En production, j'utiliserais `>>` ou mieux, la gestion des logs intégrée à Spring Boot avec Logback/Log4j2 et un système de rotation des logs.

---

**Q5 : Qu'est-ce qu'un pipe (`|`) et donnez un exemple concret ?**

Un **pipe** connecte la sortie standard (`stdout`) d'une commande à l'entrée standard (`stdin`) de la suivante. Cela permet de chaîner des commandes. Par exemple, pour compter le nombre d'erreurs dans les logs Spring Boot aujourd'hui : `grep "$(date '+%Y-%m-%d')" application.log | grep "ERROR" | wc -l`. Ici, `grep` filtre d'abord par date, puis un second `grep` filtre les erreurs, et `wc -l` compte les lignes résultantes. C'est très utile pour l'analyse rapide de logs en production.

---

### Section 2 : Réseau et sécurité

---

**Q6 : Comment vérifier qu'une application Spring Boot écoute bien sur le port 8080 ?**

J'utilise `ss -tulnp | grep 8080` ou `netstat -tulnp | grep 8080`. Ces commandes affichent les ports TCP/UDP en écoute avec le processus associé. Je peux aussi utiliser `lsof -i :8080` pour voir quel processus utilise ce port. Pour tester que l'application répond, j'utilise `curl http://localhost:8080/actuator/health` si Spring Boot Actuator est configuré, ou simplement `curl http://localhost:8080/`.

---

**Q7 : Expliquez la différence entre SSH avec mot de passe et SSH avec clé publique.**

L'authentification par **mot de passe** est simple mais vulnérable aux attaques par brute-force et aux interceptions. L'authentification par **clé publique/privée** est bien plus sécurisée : on génère une paire de clés avec `ssh-keygen`, on dépose la clé **publique** sur le serveur dans `~/.ssh/authorized_keys`, et on garde la clé **privée** sur sa machine locale (protégée par une passphrase). En production, on désactive l'authentification par mot de passe dans `/etc/ssh/sshd_config` (`PasswordAuthentication no`). Dans un contexte CI/CD, les clés SSH permettent à Jenkins ou GitHub Actions de déployer automatiquement sans mot de passe.

---

**Q8 : Comment utiliser `curl` pour tester une API REST Spring Boot ?**

Pour tester un GET : `curl http://localhost:8080/api/users`. Pour un POST avec JSON : `curl -X POST http://localhost:8080/api/users -H "Content-Type: application/json" -d '{"name":"Anouar"}'`. Pour ajouter un header d'authentification JWT : `curl -H "Authorization: Bearer <token>" http://localhost:8080/api/secure-resource`. L'option `-v` (verbose) affiche les headers HTTP complets, très utile pour déboguer. En développement, j'utilise aussi Postman, mais `curl` est indispensable en SSH sur un serveur.

---

### Section 3 : Java et Linux

---

**Q9 : Comment configurer JAVA_HOME de manière persistante ?**

J'ajoute les lignes suivantes dans `~/.bashrc` (pour l'utilisateur courant) ou dans `/etc/environment` (pour tous les utilisateurs) :
```bash
export JAVA_HOME=/opt/jdk-21
export PATH=$PATH:$JAVA_HOME/bin
```
Ensuite `source ~/.bashrc` pour recharger sans redémarrer le terminal. Je vérifie avec `echo $JAVA_HOME` et `java -version`. En environnement Docker ou systemd, je passe `JAVA_HOME` comme variable d'environnement dans le Dockerfile ou le fichier `.service`.

---

**Q10 : Comment passer les propriétés Spring Boot en ligne de commande Linux ?**

Il y a plusieurs façons. La première est d'utiliser les options JVM avec `-D` : `java -Dspring.profiles.active=prod -Dserver.port=9090 -jar myapp.jar`. La deuxième est d'utiliser les variables d'environnement, car Spring Boot les lit automatiquement en appliquant la convention de nommage (les `.` et `-` sont remplacés par `_` et tout est en majuscule) : `SPRING_PROFILES_ACTIVE=prod SERVER_PORT=9090 java -jar myapp.jar`. En production avec `systemd`, je configure les variables dans le fichier `.service` via la directive `Environment=` ou `EnvironmentFile=`.

---

**Q11 : Comment obtenir un thread dump d'une application Spring Boot en production ?**

J'utilise `jstack <PID>` pour générer un thread dump vers stdout, ou `jstack <PID> > thread-dump.txt` pour le sauvegarder dans un fichier. Je peux aussi envoyer le signal SIGQUIT avec `kill -3 <PID>` qui provoque l'affichage du thread dump dans la sortie standard de l'application. Si Spring Boot Actuator est activé, j'utilise `curl http://localhost:8080/actuator/threaddump` pour l'obtenir via HTTP. Le thread dump est essentiel pour diagnostiquer les **deadlocks** ou les threads bloqués.

---

**Q12 : Comment monitorer la mémoire JVM d'une application Java sous Linux ?**

J'utilise `jstat -gcutil <PID> 1000` pour surveiller l'utilisation du Garbage Collector toutes les secondes. Pour voir l'état du heap, j'utilise `jmap -heap <PID>`. Si Spring Boot Actuator est configuré, `curl http://localhost:8080/actuator/metrics/jvm.memory.used` donne les métriques JVM en JSON. En production, on met en place un système de monitoring comme **Prometheus + Grafana** ou **Micrometer** (intégré à Spring Boot) pour observer les métriques JVM dans le temps et configurer des alertes.

---

### Section 4 : Angular et Linux

---

**Q13 : Quelle est la différence entre `npm install` et `npm ci` en contexte Linux/CI-CD ?**

`npm install` lit le fichier `package.json`, résout les versions des dépendances (en pouvant installer des versions légèrement différentes selon les règles semver), et met à jour le `package-lock.json`. `npm ci` (CI = Clean Install) est fait pour les environnements d'intégration continue : il installe exactement les versions spécifiées dans le `package-lock.json` sans le modifier, supprime `node_modules` avant d'installer, et échoue si `package-lock.json` est absent ou incohérent avec `package.json`. En CI/CD (GitHub Actions, Jenkins), j'utilise toujours `npm ci` pour garantir des builds reproductibles.

---

**Q14 : Comment servir une application Angular en production sous Linux avec Nginx ?**

Je build l'application avec `ng build --configuration=production`, qui génère les fichiers optimisés dans `dist/`. Je les copie dans `/var/www/html/` ou un répertoire dédié. Dans la configuration Nginx, la directive clé pour une **Single Page Application** Angular est `try_files $uri $uri/ /index.html` : elle redirige toutes les routes inconnues vers `index.html` pour laisser le **Router Angular** gérer la navigation côté client. Si l'application Angular consomme une API Spring Boot, je configure Nginx en **reverse proxy** avec `location /api/ { proxy_pass http://localhost:8080/; }` pour éviter les problèmes CORS.

---

**Q15 : Comment déboguer les problèmes de permissions avec `node_modules` sous Linux ?**

Les problèmes de permissions avec npm surviennent souvent quand des packages ont été installés en `sudo`. La solution propre est de configurer un répertoire global npm dans le home de l'utilisateur : `npm config set prefix '~/.npm-global'` et d'ajouter `~/.npm-global/bin` au `PATH`. Pour un projet existant avec des permissions cassées, `chown -R $USER:$USER node_modules/` corrige le propriétaire. En Docker, on évite ces problèmes en utilisant un utilisateur non-root et en configurant les permissions correctement dans le Dockerfile.

---

### Section 5 : Scripts et automatisation

---

**Q16 : Expliquez le rôle du `shebang` (`#!/bin/bash`) dans un script.**

Le **shebang** est la première ligne d'un script, préfixée par `#!`. Elle indique au système quel interpréteur utiliser pour exécuter le fichier, même si on le lance directement (`./deploy.sh`) sans spécifier `bash deploy.sh`. `#!/bin/bash` utilise Bash, `#!/bin/sh` utilise le shell POSIX (plus portable), `#!/usr/bin/env python3` utilise Python. Sans shebang, le système essaiera d'exécuter le script avec le shell par défaut, ce qui peut donner des comportements inattendus. La commande `set -e` en début de script est également importante : elle arrête le script dès qu'une commande retourne un code d'erreur non nul.

---

**Q17 : Comment utiliser les variables d'environnement dans un script Bash de déploiement ?**

Dans un script, je peux utiliser `export VAR=value` pour définir des variables disponibles dans les sous-processus. Pour les valeurs par défaut, j'utilise la syntaxe `${VAR:-valeur_par_defaut}` : par exemple `PROFILE=${1:-prod}` utilise le premier argument ou "prod" si aucun argument n'est fourni. Pour un déploiement Spring Boot, je peux externaliser les secrets dans un fichier `.env` (que je ne commit pas dans Git) et le charger avec `source /opt/myapp/app.env`. Sous systemd, j'utilise `EnvironmentFile=/opt/myapp/app.env` dans le fichier de service, ce qui est plus sécurisé car le fichier est lisible uniquement par root.

---

**Q18 : Quelle est la différence entre `kill`, `kill -9` et `killall` ?**

`kill PID` envoie le signal **SIGTERM** (15) au processus, lui demandant de s'arrêter proprement. Le processus peut intercepter ce signal, finir de traiter les requêtes en cours et libérer ses ressources (clean shutdown). C'est ce qu'utilise Spring Boot pour son arrêt gracieux. `kill -9 PID` envoie **SIGKILL**, un signal que le processus ne peut pas intercepter ni ignorer — le kernel force l'arrêt immédiat. C'est le dernier recours quand un processus ne répond plus, car il peut laisser des données corrompues. `killall java` envoie SIGTERM à tous les processus dont le nom est "java", et `pkill -f "myapp.jar"` permet de cibler un pattern dans la commande complète.

---

### Section 6 : Docker et Linux

---

**Q19 : Expliquez comment Docker s'intègre avec Linux au niveau système.**

Docker repose sur deux fonctionnalités du kernel Linux : les **namespaces** (isolation des processus, réseau, système de fichiers) et les **cgroups** (limitation des ressources CPU, mémoire). Les images Docker sont constituées de **layers** en lecture seule (basés sur Union File Systems), et chaque conteneur ajoute une couche en écriture. Pour un développeur Java/Angular, Docker garantit que l'application s'exécute de la même façon en local et en production. La commande `docker run -d -p 8080:8080 myapp:1.0` démarre un conteneur en arrière-plan (`-d`) et mappe le port 8080 du host vers le port 8080 du conteneur.

---

**Q20 : Comment gérer les logs d'une application Spring Boot dans Docker ?**

Par défaut, Docker capture ce que l'application écrit sur `stdout` et `stderr`. Avec Spring Boot, je configure Logback pour écrire sur `stdout` (configuration `CONSOLE_LOG_PATTERN`) plutôt que dans un fichier, pour respecter les bonnes pratiques des **12-Factor Apps**. Je consulte les logs avec `docker logs myapp` ou `docker logs -f myapp` pour le mode temps réel. En production, on configure un **log driver** Docker (comme `json-file`, `fluentd`, `awslogs`) pour centraliser les logs vers un système comme **ELK Stack** (Elasticsearch, Logstash, Kibana) ou **CloudWatch**.

---

### Section 7 : systemd et déploiement

---

**Q21 : Pourquoi utiliser systemd pour déployer une application Spring Boot ?**

`systemd` est le système d'initialisation standard de Linux (Ubuntu, Debian, CentOS…). Il offre plusieurs avantages pour une application Spring Boot : le **redémarrage automatique** en cas de crash (`Restart=on-failure`), le **démarrage automatique** au boot du serveur (`systemctl enable myapp`), l'**intégration avec journald** pour les logs (accessibles via `journalctl -u myapp`), la gestion des **dépendances de démarrage** (`After=network.target` pour démarrer après le réseau), et la gestion de l'utilisateur sous lequel tourne l'application (`User=appuser`) pour des raisons de sécurité. C'est une solution robuste et native, sans avoir besoin de scripts `nohup` artisanaux.

---

**Q22 : Que fait `journalctl -u myapp -f --since "1 hour ago"` ?**

Cette commande affiche les logs du service systemd `myapp` de manière continue (`-f` pour follow, comme `tail -f`), en commençant depuis une heure avant le moment courant. `journald` centralise les logs de tous les services systemd, avec des métadonnées riches (timestamp, PID, hostname, priorité). Je peux filtrer par priorité avec `-p err` pour n'afficher que les erreurs, ou par plage de temps avec `--since "2024-01-15 10:00" --until "2024-01-15 11:00"`. C'est l'équivalent de `tail -f application.log | grep ...` mais avec beaucoup plus de flexibilité et sans avoir à connaître le chemin du fichier de log.

---

> **Conseil d'entretien** : Pour les questions techniques, illustrez toujours vos réponses par des exemples concrets tirés de votre expérience Spring Boot ou Angular. Les recruteurs apprécient les candidats qui savent pourquoi ils utilisent une commande, pas seulement comment.
