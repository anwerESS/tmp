# Guide pratique SSL/TLS, certificats, PKI, Keystores, OpenSSL et HTTPS

**De la théorie aux laboratoires Linux, Java/Tomcat, Apache et Kubernetes**  
**Langue :** français, avec les termes techniques en anglais  
**Édition :** novembre 2026  
**Public :** développeur Java, ingénieur applicatif, DevOps ou administrateur souhaitant comprendre et diagnostiquer les certificats en situation réelle.

> **Objectif.** Comprendre **pourquoi** un certificat est nécessaire, **où** se trouvent les clés et les certificats, **comment** établir et diagnostiquer HTTPS, puis **comment** automatiser leur déploiement et leur renouvellement. Les exercices sont conçus pour un environnement de **laboratoire**. Ne pas réutiliser les mots de passe de démonstration ni les autorités de certification (CA) du laboratoire en production.

## Sommaire

1. [Parcours recommandé et sources](#1-parcours-recommandé-et-sources)
2. [Confidentialité, intégrité, authenticité et MAC](#2-confidentialité-intégrité-authenticité-et-mac)
3. [Cryptographie symétrique, asymétrique et signatures](#3-cryptographie-symétrique-asymétrique-et-signatures)
4. [SSL, TLS, HTTPS et le TLS Handshake](#4-ssl-tls-https-et-le-tls-handshake)
5. [Certificat X.509 : contenu et vérifications](#5-certificat-x509--contenu-et-vérifications)
6. [PKI, Root CA, Intermediate CA, certificat serveur et chaîne de confiance](#6-pki-root-ca-intermediate-ca-certificat-serveur-et-chaîne-de-confiance)
7. [Identity Store, Keystore et Truststore](#7-identity-store-keystore-et-truststore)
8. [PEM, DER, JKS, PKCS#12 : formats et extensions](#8-pem-der-jks-pkcs12--formats-et-extensions)
9. [OpenSSL : commandes indispensables](#9-openssl--commandes-indispensables)
10. [Keytool : laboratoire JKS et PKCS#12](#10-keytool--laboratoire-jks-et-pkcs12)
11. [Laboratoire de bout en bout : Root CA → Intermediate CA → certificat Tomcat](#11-laboratoire-de-bout-en-bout--root-ca--intermediate-ca--certificat-tomcat)
12. [Apache HTTP Server : HTTPS global et Virtual Hosts](#12-apache-http-server--https-global-et-virtual-hosts)
13. [Java, Tomcat, Spring Boot et reverse proxy](#13-java-tomcat-spring-boot-et-reverse-proxy)
14. [Certificats publics, Let's Encrypt et renouvellement sans interruption](#14-certificats-publics-lets-encrypt-et-renouvellement-sans-interruption)
15. [Docker, CI/CD, Kubernetes et cert-manager](#15-docker-cicd-kubernetes-et-cert-manager)
16. [mTLS : authentification réciproque](#16-mtls--authentification-réciproque)
17. [Troubleshooting : diagnostic méthodique](#17-troubleshooting--diagnostic-méthodique)
18. [Erreurs et limites du cours Udemy : corrections importantes](#18-erreurs-et-limites-du-cours-udemy--corrections-importantes)
19. [Cheat sheet : commandes à garder](#19-cheat-sheet--commandes-à-garder)
20. [Exercices, checklist de production et glossaire](#20-exercices-checklist-de-production-et-glossaire)
21. [Références](#21-références)

---

## 1. Parcours recommandé et sources

### 1.1 Les connaissances réellement utiles

Pour gérer une application Java/Tomcat derrière un reverse proxy, il faut surtout maîtriser :

- **Très important :** clés publique/privée ; certificat X.509 ; CA ; Trust Chain ; TLS Handshake ; SAN/SNI ; PKCS#12 ; `keytool` ; `openssl s_client` ; HTTPS côté Apache/Nginx/Tomcat ; renouvellement.
- **Important :** CSR ; HMAC vs signature numérique ; Cipher Suites ; mTLS ; Secret Kubernetes ; configuration d'Ingress/Gateway ; automation ACME.
- **Secondaire pour cet objectif :** **Oracle Wallet (`orapki`)** et **IBM CMS Keystore (`iKeyman`)**. Ils appliquent des principes similaires, mais avec des outils propres à Oracle/IBM. On peut les ignorer tant qu'aucune application ne les utilise.

### 1.2 Sources du cours fournies

Le guide reprend les thèmes du **programme Udemy, sections 1–14** et se fonde plus précisément sur :

- **`Keystore.txt`** : commandes `keytool` pour JKS/PKCS#12 ; Root/Intermediate/Server ; CSR ; import/export/suppression ; section Oracle Wallet.
- **`OpenSSL.pdf`** (14 pages) : formats PEM/JKS/PKCS#12 ; `openssl version`, `rand`, `enc`, `s_client`, `genpkey`, `pkey`, `req`, `x509`, `pkcs12` ; principes de configuration Apache.
- **`D-ApacheSSLConfiguration.txt`** : laboratoire `httpd`/`mod_ssl` sous distribution RHEL-like ; configuration globale ; Virtual Hosts ; certificats autosignés et signés par CA.
- **Captures partagées dans la discussion** : Identity/Trust, WebLogic, création JKS puis PKCS#12, CSR, chaîne de certificats et choix des sections utiles.
- **Discussions pratiques** : certificat public, Let's Encrypt, rechargement sans downtime, Docker, CI/CD, Kubernetes, cert-manager.

**Méthode éditoriale :** les exemples et principes issus de la formation sont conservés, mais les erreurs identifiées sont explicitement corrigées à la section 18. Les chapitres sur l'automatisation DevOps développent les thèmes de notre discussion, au-delà des laboratoires Udemy.

---

## 2. Confidentialité, intégrité, authenticité et MAC

Lorsque le navigateur échange avec un serveur, trois propriétés sont souhaitées :

| Propriété | Question | Menace | Mécanisme courant |
|---|---|---|---|
| **Confidentialité (Confidentiality)** | Qui peut lire le message ? | Espionnage du trafic | Chiffrement, par ex. AES-GCM |
| **Intégrité (Integrity)** | Le message a-t-il été modifié ? | Modification d'un montant ou d'une requête | HMAC, AEAD, signature |
| **Authenticité (Authenticity)** | Qui a produit ce message ou qui est ce serveur ? | Usurpation | HMAC, signature, authentification TLS |

### 2.1 Exemple : demande de virement

- Alice envoie `montant=100` ; un attaquant tente de le transformer en `montant=9000`.
- Un simple **hash** tel que SHA-256 ne suffit pas à authentifier l'émetteur : l'attaquant pourrait remplacer **à la fois** le message et son hash.
- Un **MAC — Message Authentication Code** est calculé avec une **clé secrète partagée**. Si l'attaquant ne connaît pas cette clé, il ne peut pas produire un MAC valide pour le nouveau message.
- **HMAC-SHA256** est une construction de MAC fondée sur SHA-256 ; elle ne chiffre pas le message.

```text
Alice et Bob partagent secrètement K.

Alice : M = "montant=100"
        tag = HMAC-SHA256(K, M)
        envoie (M, tag)

Bob :   calcule tag2 = HMAC-SHA256(K, M_reçu)
        compare tag et tag2 de manière sûre
        → égal : message authentifié avec la clé partagée
        → différent : rejeter
```

### 2.2 Exemple minimal Python

```python
import hmac
import hashlib

secret = b"cle-de-demonstration-non-securisee"
message = b"montant=100"

tag = hmac.new(secret, message, hashlib.sha256).digest()

# Le destinataire recalcule le MAC.
attendu = hmac.new(secret, message, hashlib.sha256).digest()
if hmac.compare_digest(tag, attendu):
    print("Integrite et authenticite par cle partagee verifiees")
```

**Limites :** un HMAC ne garantit pas la confidentialité, ni l'identité exclusive d'une personne si plusieurs détenteurs connaissent la même clé, ni une protection automatique contre les **replay attacks**. On peut protéger contre les rejeux à l'aide d'un identifiant unique (*nonce*), d'un timestamp et d'une logique anti-duplication.

### 2.3 AEAD : confidentialité + intégrité

Les connexions TLS modernes utilisent des modes de **Authenticated Encryption with Associated Data (AEAD)**, par exemple **AES-GCM** ou **ChaCha20-Poly1305**. Ils chiffrent les données et détectent leur falsification. Ils n'utilisent pas nécessairement un HMAC séparé pour chaque enregistrement TLS.

---

## 3. Cryptographie symétrique, asymétrique et signatures

### 3.1 Symmetric Encryption

Une **même clé secrète** chiffre et déchiffre les données :

```text
Message en clair --[AES + secret K]--> Message chiffré
Message chiffré --[AES + secret K]--> Message en clair
```

- **Avantage :** rapide, approprié aux grands volumes de données.
- **Difficulté :** la clé doit être connue des participants sans être interceptée.
- **Exemples :** AES-128-GCM, AES-256-GCM ; ChaCha20-Poly1305.

### 3.2 Asymmetric Cryptography

Chaque identité possède une paire de clés :

- **Private Key** : secrète, conservée par le propriétaire.
- **Public Key** : distribuable, souvent incluse dans le certificat.

Pour une **signature numérique**, le signataire calcule la signature avec sa clé privée ; le vérificateur la contrôle avec la clé publique. Cela permet de vérifier qu'un détenteur de la clé privée a signé les données.

**Attention :** la clé privée du certificat TLS ne sert généralement **pas** à déchiffrer l'ensemble des pages HTTP dans TLS 1.3. Elle permet surtout de **signer le handshake** pour prouver l'identité du serveur. Des clés de session symétriques sont ensuite négociées.

### 3.3 Comment TLS combine les deux

1. Le serveur présente un **certificat** contenant sa clé publique.
2. Le client vérifie l'identité et la chaîne de confiance.
3. Les pairs négocient des secrets de session, généralement via **(EC)DHE** (*Ephemeral Diffie-Hellman*).
4. Ils chiffrent les données applicatives avec des **clés symétriques**.

Ce mélange s'appelle parfois **hybrid cryptography** : l'asymétrique sert à l'authentification et à l'établissement de secrets ; le symétrique protège efficacement le trafic.

### 3.4 HMAC vs signature numérique

| | HMAC | Signature numérique |
|---|---|---|
| Secret utilisé | Même clé secrète partagée | Clé privée pour signer |
| Vérification | Même clé secrète | Clé publique |
| Usage | Authentifier des messages entre systèmes partageant un secret | Certificats, signatures de CSR, authentification du handshake |
| Confidentialité | Non | Non |

---

## 4. SSL, TLS, HTTPS et le TLS Handshake

### 4.1 SSL vs TLS

**SSL** est le nom historique. Les anciennes versions SSL sont obsolètes et dangereuses. Aujourd'hui, lorsque l'on parle de « certificat SSL », on désigne habituellement un **certificat TLS**.

- **TLS 1.3** : version moderne à privilégier.
- **TLS 1.2** : encore présente pour l'interopérabilité avec certains systèmes.
- **SSLv2 / SSLv3 / TLS 1.0 / TLS 1.1** : à désactiver sur une nouvelle configuration.

**HTTPS** est HTTP transporté dans une connexion TLS, généralement sur le port **443**. HTTP classique utilise généralement le port **80**.

### 4.2 Handshake TLS 1.3 : vue simplifiée

```text
Navigateur (client)                        Serveur (Tomcat/Nginx)
        |                                           |
        |---- ClientHello ------------------------->|
        |     versions, suites, key_share, SNI      |
        |<--- ServerHello --------------------------|
        |     choix crypto, key_share               |
        |<--- EncryptedExtensions ------------------|
        |<--- Certificate --------------------------|
        |<--- CertificateVerify --------------------|
        |<--- Finished -----------------------------|
        |---- Finished ---------------------------->|
        |                                           |
        |<==== HTTP chiffré avec clés de session ===>|
```

**Ce qu'il faut comprendre :**

- `ClientHello` : le client indique notamment les versions et suites prises en charge et le nom de domaine visé via **SNI**.
- `ServerHello` : le serveur choisit les paramètres compatibles et participe à la négociation du secret.
- `Certificate` : le serveur transmet son identité certifiée.
- `CertificateVerify` : le serveur prouve qu'il possède la **clé privée** du certificat.
- `Finished` : les participants confirment l'intégrité de la négociation.
- Après le handshake, le trafic applicatif est protégé avec des **clés symétriques**.

En TLS 1.3, une grande partie du handshake qui suit `ServerHello` est déjà protégée. Le schéma ci-dessus est pédagogique : il masque volontairement plusieurs détails techniques.

### 4.3 SNI, SAN et ALPN : ne pas confondre

| Terme | Où intervient-il ? | Fonction |
|---|---|---|
| **SNI — Server Name Indication** | `ClientHello` | Le client demande le certificat du site voulu quand plusieurs sites partagent un serveur/IP |
| **SAN — Subject Alternative Name** | Certificat X.509 | Liste des noms DNS/IP pour lesquels le certificat est valide |
| **ALPN — Application-Layer Protocol Negotiation** | Handshake TLS | Choix du protocole applicatif (`h2`, `http/1.1`, etc.) |

Exemple : un reverse proxy héberge `app.example.com` et `api.example.com` sur la même IP. Grâce au SNI, il peut présenter le certificat correspondant à chaque domaine. Le client vérifie ensuite que le **SAN** du certificat correspond au nom demandé.

### 4.4 Cipher Suites

Une **Cipher Suite** est un ensemble de paramètres cryptographiques négociés. Exemple TLS 1.3 :

```text
TLS_AES_256_GCM_SHA384
```

- `AES_256_GCM` : chiffrement authentifié des données.
- `SHA384` : fonction de hachage utilisée notamment dans la dérivation de clés de TLS.

En **TLS 1.3**, le choix du groupe d'échange de clés et de l'algorithme de signature est séparé du nom de la Cipher Suite. Il ne faut donc pas lire les suites TLS 1.3 comme les suites TLS 1.2 historiques telles que `ECDHE-RSA-AES128-GCM-SHA256`.

### 4.5 Quelques points de diagnostic

```bash
# Tester spécifiquement TLS 1.3
openssl s_client -connect example.com:443 \
  -servername example.com -tls1_3

# Tester TLS 1.2, si encore nécessaire
openssl s_client -connect example.com:443 \
  -servername example.com -tls1_2
```

---

## 5. Certificat X.509 : contenu et vérifications

Un **certificat numérique X.509** associe une **clé publique** à des informations d'identité ; il est signé par une autorité, ou autosigné dans un laboratoire.

### 5.1 Champs importants

| Champ | Rôle | Exemple |
|---|---|---|
| **Subject** | Identité déclarée | `CN=app.example.com` |
| **Issuer** | CA qui a signé | `CN=Entreprise Issuing CA` |
| **SAN** | Noms DNS/IP autorisés | `DNS:app.example.com`, `IP:10.0.0.5` |
| **Validity** | `Not Before` / `Not After` | Dates de validité |
| **Public Key** | Clé publique associée | RSA 3072 bits ou clé EC |
| **Signature Algorithm** | Algorithme de signature du certificat | `sha256WithRSAEncryption` |
| **Basic Constraints** | Distingue CA et certificat final | `CA:TRUE` ou `CA:FALSE` |
| **Key Usage / Extended Key Usage** | Opérations autorisées | `digitalSignature`, `serverAuth` |
| **Fingerprint** | Empreinte du certificat | SHA-256 |

Le **CN** est encore visible, mais pour l'identification moderne d'un serveur HTTPS, il faut renseigner le **SAN**.

### 5.2 Certificat vs clé privée

```text
server.crt / cert.pem       = certificat PUBLIC, contient la clé publique
server.key / privkey.pem    = clé PRIVÉE, ne jamais distribuer
server.csr                 = demande de certificat, contient la clé publique
```

Une **CSR** n'est pas un certificat utilisable en production. Elle sert à demander une signature à une CA ; la clé privée reste chez son propriétaire.

### 5.3 Vérifications effectuées par le client

Le navigateur ou le client TLS vérifie notamment :

1. Le certificat correspond au nom de domaine demandé (**hostname verification**).
2. Il est dans sa période de validité.
3. Sa signature et les signatures de la chaîne sont correctes.
4. La chaîne mène à une **trust anchor** (CA racine ou autre point d'ancrage explicitement approuvé).
5. Les contraintes d'utilisation, algorithmes et autres politiques de validation sont respectés.
6. Selon le client et la politique, l'état de **révocation** est aussi évalué.

Le certificat **seul** ne prouve pas que le serveur possède la clé privée : cette possession est démontrée lors du handshake TLS.

---

## 6. PKI, Root CA, Intermediate CA, certificat serveur et chaîne de confiance

**PKI — Public Key Infrastructure** : ensemble de rôles, certificats, clés, procédures et règles permettant d'émettre et de vérifier des identités numériques.

### 6.1 Les trois certificats du fichier `Keystore.txt`

```text
Root CA Certificate (autorité racine)
        |
        | signe
        v
Intermediate CA Certificate (autorité intermédiaire)
        |
        | signe
        v
Server / Personal Certificate (certificat du site)
```

| Certificat | Appartient à | Sert à | Où se trouve en général sa **clé privée** ? |
|---|---|---|---|
| **Root CA** | Autorité racine | Ancrer la confiance ; signer une CA subordonnée | Chez la Root CA, fortement protégée |
| **Intermediate CA** | Autorité intermédiaire | Émettre les certificats serveurs ou d'autres CA | Chez l'Intermediate CA |
| **Server / Personal** | Serveur HTTPS | Authentifier le serveur | Sur le serveur, ou dans un gestionnaire sécurisé de clés |

Le certificat Root CA est généralement **autosigné**, mais **autosigné** ne veut pas dire « automatiquement de confiance ». Il doit figurer parmi les certificats explicitement approuvés du client.

Le certificat serveur a généralement `CA:FALSE`. Contrairement à une CA, il n'a pas pour rôle de signer de nouveaux certificats.

### 6.2 Exemple réel de validation

Pour `https://jira.entreprise.example` :

```text
Certificat serveur : jira.entreprise.example
     signé par : Entreprise Issuing CA
     signé par : Entreprise Root CA
     approuvée dans : Truststore du client
```

Le serveur transmet habituellement **son certificat et les intermédiaires**, sans nécessairement transmettre la Root CA. Le client doit déjà disposer d'un point d'ancrage de confiance ; envoyer une Root CA inconnue ne la rend pas fiable.

### 6.3 Self-Signed vs CA-Signed

**Self-Signed Certificate :** signé avec sa propre clé. Bien pour un test local, une CA privée contrôlée, certains environnements internes. Un navigateur ne le reconnaîtra pas spontanément s'il n'a pas été approuvé.

**CA-Signed Certificate :** signé par une autorité de certification. Une CA publique valide le contrôle du domaine (DV) ; une CA interne peut émettre des certificats pour les services d'entreprise.

**Important :** un certificat DV émis par une CA publique ne prouve pas que l'entreprise exploitant un site est digne de confiance ; il prouve principalement le contrôle du nom de domaine lors de l'émission.

### 6.4 Trois certificats ne sont pas toujours nécessaires

- Un seul : certificat autosigné.
- Deux : Root CA → serveur (dans une architecture simple).
- Trois : Root CA → Intermediate CA → serveur (cas fréquent).
- Quatre ou plus : plusieurs niveaux de CA intermédiaires.

**Erreurs fréquentes :** certificat intermédiaire manquant, CA interne non importée côté client, domaine absent du SAN, certificat expiré, CA racine révoquée ou désapprouvée.

---

## 7. Identity Store, Keystore et Truststore

### 7.1 Les concepts

- **Identity Store :** *rôle fonctionnel*. Stockage permettant à **moi / mon application** de présenter une identité cryptographique. Contient généralement une clé privée et le certificat associé.
- **Truststore :** *rôle fonctionnel*. Stockage des certificats d'autorités ou d'identités auxquelles **je fais confiance** pour vérifier l'autre partie.
- **Keystore :** *mécanisme de stockage*. En Java, les API Keystore manipulent notamment des entrées de clé privée, de certificat de confiance et parfois des clés symétriques.

Ainsi, **un Keystore peut servir d'Identity Store ou de Truststore**. Les mots Identity et Trust décrivent **l'usage**, pas un format obligatoirement différent.

### 7.2 Cas classique HTTPS

```text
Client Java / navigateur                         Serveur Tomcat
  Truststore (CA reconnues)                   Identity Keystore
            |                                  clé privée + certificat
            |<-------- certificat serveur -------------|
            |                                         |
            |---- vérifie chaîne et hostname ------->|
```

Dans le cas de **mTLS**, **chaque partie** présente son certificat (Identity Store) et vérifie l'autre (Truststore).

### 7.3 Où sont-ils stockés techniquement ?

Il n'y a **aucun emplacement universel obligatoire**.

```text
/opt/tomcat/conf/identity.p12         # Exemple d'Identity Store
/etc/myapp/tls/truststore.p12         # Exemple de Truststore applicatif
$JAVA_HOME/lib/security/cacerts      # Truststore par défaut courant de Java
/etc/ssl/certs/java/cacerts          # Chemin possible sur Debian/Ubuntu
```

Sous **Windows Server**, la clé et le certificat peuvent être dans un fichier `.p12`/`.jks` ou dans un magasin de certificats Windows, selon l'application et son mode de terminaison TLS.

Pour **Tomcat**, il faut suivre le chemin configuré dans `server.xml`. Pour une application Spring Boot, consulter `server.ssl.key-store`. Pour un reverse proxy Apache/Nginx, chercher les directives de chemins vers les fichiers PEM. La connexion HTTPS peut être terminée **sur un load balancer en amont**, auquel cas Tomcat n'héberge pas nécessairement le certificat public.

### 7.4 Java : types d'entrées

```text
identity.p12
└── Alias : tomcat
    └── PrivateKeyEntry
        ├── Private Key
        └── Certificate Chain

autorites.p12
├── Alias : ca-interne
│   └── trustedCertEntry
└── Alias : autre-ca
    └── trustedCertEntry
```

**Attention :** importer un certificat avec un **alias différent** de celui de la clé privée crée généralement un `trustedCertEntry`. Pour remplacer la réponse de certificat associée à la clé privée à l'issue d'une CSR, il faut réutiliser **le même alias** que celui du `PrivateKeyEntry`.

---

## 8. PEM, DER, JKS, PKCS#12 : formats et extensions

| Format | Extensions courantes | Contenu / fonctionnement | Outil habituel |
|---|---|---|---|
| **PEM** | `.pem`, `.crt`, `.cer`, `.key` | Encodage texte de certificats, clés ou chaînes ; fichiers séparés ou concaténés | OpenSSL, Apache, Nginx |
| **DER** | `.der`, parfois `.cer` | Encodage binaire ASN.1 | OpenSSL, Java |
| **JKS** | `.jks` | Format historique Java KeyStore | `keytool` |
| **PKCS#12** | `.p12`, `.pfx`, `.pkcs12` | Conteneur interopérable de clés et certificats, généralement protégé par mot de passe | OpenSSL, `keytool` |
| **Oracle Wallet** | selon installation | Stockage spécialisé de l'écosystème Oracle | `orapki` |
| **IBM CMS** | `.kdb` | Format spécialisé IBM | iKeyman / GSKit |

### 8.1 La bonne règle sur les extensions

**L'extension n'est pas une garantie de contenu.** Un fichier `.cer` peut être PEM ou DER. Un fichier `.pem` peut contenir un certificat, une chaîne ou une clé privée. Il faut inspecter le **contenu réel** :

```bash
# Texte : -----BEGIN CERTIFICATE----- ou -----BEGIN PRIVATE KEY-----
head -n 2 server.crt

# Identifier un fichier
file server.crt

# Afficher un certificat PEM
openssl x509 -in server.crt -noout -subject -issuer -dates

# Si le certificat est en DER
openssl x509 -inform DER -in server.cer -text -noout
```

### 8.2 PEM n'est pas PKCS#12

```text
Apache/Nginx (souvent) :       Java/Tomcat (souvent) :
  server.key                   identity.p12
  server.crt                   ├── Private Key
  intermediate.crt             └── Certificate Chain
```

Il est possible de convertir ces fichiers dans les deux sens avec les précautions appropriées. **OpenSSL sait parfaitement créer des PKCS#12** : une affirmation contraire dans le tableau de la formation `OpenSSL.pdf` p.5 est erronée (voir section 18).

---

## 9. OpenSSL : commandes indispensables

**OpenSSL** est une boîte à outils indépendante de Java, particulièrement utile pour créer des clés/CSR, inspecter des certificats et diagnostiquer des serveurs HTTPS.

### 9.1 Installation et découverte

```bash
# Vérifier les outils
openssl version
command -v openssl
openssl help

# Ubuntu / Debian
sudo apt update
sudo apt install -y openssl ca-certificates

# RHEL / Rocky / AlmaLinux
sudo dnf install -y openssl ca-certificates

# Algorithmes pris en charge
openssl list -cipher-algorithms
openssl list -digest-algorithms
openssl list -public-key-algorithms
```

### 9.2 Laboratoire OpenSSL : clé privée et clé publique

```bash
mkdir -p ~/tls-openssl-lab
cd ~/tls-openssl-lab
umask 077

# Générer la clé privée RSA (fichier PEM)
openssl genpkey -algorithm RSA \
  -pkeyopt rsa_keygen_bits:3072 \
  -out server.key

# Extraire la clé publique à partir de la clé privée
openssl pkey -in server.key -pubout -out server.pub

# Voir les informations de clé SANS afficher la clé privée
openssl pkey -in server.key -noout -text_pub
```

**Explication :** `genpkey` crée une clé ; `pkey -pubout` extrait uniquement la partie publique. Pour un service critique, employer un mécanisme de protection de clés adapté ; un simple PEM non chiffré doit bénéficier d'une protection forte par permissions et contrôle d'accès.

### 9.3 Générer une CSR avec SAN

```bash
openssl req -new \
  -key server.key \
  -out server.csr \
  -subj "/C=FR/O=Laboratoire/CN=app.example.test" \
  -addext "subjectAltName=DNS:app.example.test,DNS:www.app.example.test"

# Vérifier la demande (elle ne contient PAS la clé privée)
openssl req -in server.csr -text -noout -verify
```

Le certificat émis par la CA doit comporter le **SAN** voulu ; l'ajouter à la CSR ne force pas toutes les CA à le reprendre automatiquement.

### 9.4 Créer un certificat autosigné de laboratoire

```bash
# À partir de la clé privée existante
openssl req -x509 -new \
  -key server.key \
  -sha256 \
  -days 30 \
  -out server-selfsigned.crt \
  -subj "/C=FR/O=Laboratoire/CN=localhost" \
  -addext "subjectAltName=DNS:localhost,IP:127.0.0.1" \
  -addext "basicConstraints=critical,CA:FALSE"
```

Ce certificat peut être utilisé en test local ; il n'est **pas reconnu spontanément** par les navigateurs publics.

### 9.5 Inspecter le certificat et son expiration

```bash
openssl x509 -in server-selfsigned.crt -text -noout
openssl x509 -in server-selfsigned.crt -noout -subject -issuer
openssl x509 -in server-selfsigned.crt -noout -dates
openssl x509 -in server-selfsigned.crt -noout -fingerprint -sha256

# Afficher les SAN
openssl x509 -in server-selfsigned.crt -noout -ext subjectAltName

# Vérifier s'il expire dans les 30 prochains jours (2592000 secondes)
openssl x509 -in server-selfsigned.crt -noout -checkend 2592000
```

**Interprétation de `-checkend` :** code retour `0` si le certificat reste valide au-delà de la période ; code non nul s'il expire avant ou dans la fenêtre considérée.

### 9.6 Tester une connexion HTTPS (commande prioritaire)

```bash
openssl s_client \
  -connect example.com:443 \
  -servername example.com \
  -showcerts \
  -verify_hostname example.com \
  -verify_return_error </dev/null
```

| Option | Utilité |
|---|---|
| `-connect host:port` | Se connecter à l'hôte TCP sur le port donné |
| `-servername host` | Envoyer le SNI ; essentiel en cas de Virtual Hosts |
| `-showcerts` | Voir les certificats réellement **envoyés par le serveur** |
| `-verify_hostname host` | Contrôler l'identité DNS du serveur |
| `-verify_return_error` | Rendre une erreur de vérification bloquante |

**Lire le résultat :** `Certificate chain`, `subject`, `issuer`, `Protocol`, `Cipher`, `Verify return code`. **Attention :** `Verify return code: 0` sans validation de hostname n'est pas une preuve suffisante de bonne correspondance du domaine.

```bash
# Tester un service Tomcat local
openssl s_client \
  -connect localhost:8443 \
  -servername localhost \
  -verify_hostname localhost \
  -showcerts </dev/null

# Tester un serveur qui utilise une CA interne
openssl s_client \
  -connect app.example.test:443 \
  -servername app.example.test \
  -CAfile entreprise-root.crt \
  -verify_hostname app.example.test \
  -verify_return_error </dev/null
```

### 9.7 Vérifier une chaîne de certificats **hors réseau**

```bash
# root.crt = CA approuvée ; intermediate.crt = émetteur intermédiaire
openssl verify \
  -CAfile root.crt \
  -untrusted intermediate.crt \
  server.crt
```

`server.crt: OK` signifie que la vérification a réussi **selon les paramètres fournis**. Pour un serveur réel, vérifier également le nom, les usages autorisés, la durée de validité et les politiques de validation applicables.

### 9.8 Créer un PKCS#12 à partir d'une clé et de certificats PEM

```bash
openssl pkcs12 -export \
  -inkey server.key \
  -in server.crt \
  -certfile intermediate.crt \
  -name tomcat \
  -out identity.p12

# Inspecter sans afficher les secrets
openssl pkcs12 -in identity.p12 -info -noout

# Extraire les certificats publics
openssl pkcs12 -in identity.p12 -nokeys -out certificates.pem
```

Pour **extraire la clé privée** (uniquement en cas de nécessité et sous contrôle strict) :

```bash
# La commande demande un mot de passe pour protéger la clé exportée.
openssl pkcs12 -in identity.p12 -nocerts -out private-export.pem
```

**Avertissement :** ne publie pas ce fichier, n'utilise pas `-nodes`/`-noenc` sans raison, ne l'inclus jamais dans Git ou une image Docker.

### 9.9 Divers utiles

```bash
# 16 octets aléatoires affichés en hexadécimal
openssl rand -hex 16

# Vérifier qu'une clé privée et un certificat ont la même clé publique
openssl pkey -in server.key -pubout -outform DER | openssl dgst -sha256
openssl x509 -in server.crt -pubkey -noout | \
  openssl pkey -pubin -outform DER | openssl dgst -sha256
```

Les deux empreintes SHA-256 doivent être identiques. C'est préférable aux comparaisons d'un « modulus » RSA lorsque l'on veut une méthode compatible avec plusieurs types de clés.

Le document `OpenSSL.pdf` montre aussi `openssl enc -aes-256-cbc` pour le chiffrement de fichiers. C'est utile pédagogiquement pour comprendre le **symmetric encryption**, mais ce n'est pas un système complet de chiffrement authentifié : pour protéger des données sensibles en production, privilégier des outils/formats fournissant authentification et gestion de clés adaptées plutôt que bricoler un protocole avec `openssl enc`.

---
## 10. Keytool : laboratoire JKS et PKCS#12

`keytool` est livré avec le **JDK** et constitue l'outil le plus direct pour inspecter et administrer les Keystores Java.

### 10.1 Identifier l'outil

```bash
java -version
keytool -help
command -v keytool

# Truststore système Java par défaut, généralement peuplé de CA publiques
keytool -list -cacerts
```

**Ne pas confondre** : `cacerts` est principalement un **Truststore Java par défaut** ; il ne s'agit pas d'un Identity Keystore avec la clé privée de ton serveur.

### 10.2 LAB JKS — créer un magasin historique Java

```bash
mkdir -p ~/tls-keytool-lab && cd ~/tls-keytool-lab
umask 077

keytool -genkeypair \
  -alias legacy-tomcat \
  -keyalg RSA \
  -keysize 3072 \
  -sigalg SHA256withRSA \
  -validity 365 \
  -storetype JKS \
  -keystore identity-legacy.jks \
  -dname "CN=localhost,O=Laboratoire,C=FR" \
  -ext "SAN=dns:localhost,ip:127.0.0.1"
```

La commande demande un mot de passe. Elle génère la **paire de clés** et un **certificat autosigné initial** avec l'alias `legacy-tomcat`.

```bash
keytool -list -v -keystore identity-legacy.jks
```

Tu dois voir `Keystore type: JKS`, `Alias name: legacy-tomcat` et `Entry type: PrivateKeyEntry`.

### 10.3 LAB PKCS#12 — format recommandé pour les nouveaux exemples

```bash
keytool -genkeypair \
  -alias tomcat \
  -keyalg RSA \
  -keysize 3072 \
  -sigalg SHA256withRSA \
  -validity 365 \
  -storetype PKCS12 \
  -keystore identity.p12 \
  -dname "CN=localhost,O=Laboratoire,C=FR" \
  -ext "SAN=dns:localhost,ip:127.0.0.1"

keytool -list -v -keystore identity.p12
```

**Pourquoi PKCS#12 ?** Standard interopérable avec OpenSSL et de nombreux autres outils, utilisé par défaut pour les nouveaux magasins Java depuis Java 9. Les fichiers `.p12`, `.pfx` et `.pkcs12` désignent habituellement ce format.

**JKS vs PKCS#12 :** le cours utilise un mot de passe distinct `-keypass`/`-storepass` pour JKS. Avec l'implémentation PKCS#12 habituelle de `keytool`, utilise **le même mot de passe pour le magasin et l'entrée** ; elle ignore généralement les différences avec un avertissement.

### 10.4 Expliquer les options keytool

| Option | Ce qu'elle fait | Exemple |
|---|---|---|
| `-genkeypair` | Génère une paire de clés et un certificat autosigné initial | RSA |
| `-alias` | Nom logique de l'entrée du magasin | `tomcat` |
| `-keyalg` | Algorithme des clés | `RSA` |
| `-keysize` | Taille de clé | `3072` |
| `-sigalg` | Signature du certificat autosigné ou de la demande, selon commande | `SHA256withRSA` |
| `-storetype` | Format du magasin | `PKCS12`, `JKS` |
| `-keystore` | Fichier ou chemin du magasin | `identity.p12` |
| `-storepass` | Mot de passe d'accès au magasin | Saisie sécurisée recommandée |
| `-keypass` | Mot de passe de l'entrée de clé, selon format | souvent même que le magasin pour PKCS#12 |
| `-dname` | Distinguished Name | `CN=localhost,O=Lab,C=FR` |
| `-ext` | Extensions X.509 | `SAN=dns:localhost` |
| `-validity` | Validité du certificat autosigné créé | `365` jours |

**Ne mets pas les mots de passe dans tes scripts de production en clair.** Certains outils/OS peuvent laisser apparaître des arguments dans les processus, logs et historiques du shell.

### 10.5 Exporter un certificat autosigné — pas la clé privée

```bash
keytool -exportcert \
  -rfc \
  -alias tomcat \
  -keystore identity.p12 \
  -file tomcat-public.crt

keytool -printcert -file tomcat-public.crt
```

`-rfc` produit du texte PEM : `-----BEGIN CERTIFICATE-----`. La **clé privée reste dans le magasin**.

### 10.6 Importer ce certificat dans un Truststore séparé

Un client Java doit, pour les besoins du laboratoire, reconnaître le certificat autosigné de Tomcat.

```bash
# Vérifier son fingerprint AVANT de l'approuver.
keytool -printcert -file tomcat-public.crt

keytool -importcert \
  -alias tomcat-local-approuve \
  -file tomcat-public.crt \
  -storetype PKCS12 \
  -keystore client-truststore.p12

keytool -list -v -keystore client-truststore.p12
```

Tu obtiens normalement :

```text
Identity Keystore :
  alias tomcat                  → PrivateKeyEntry

Client Truststore :
  alias tomcat-local-approuve   → trustedCertEntry
```

Ce mécanisme crée explicitement de la confiance envers **ce certificat** ; il ne transforme pas le certificat autosigné en certificat émis par une CA publique.

### 10.7 Générer une CSR depuis Java

```bash
keytool -certreq \
  -alias tomcat \
  -keystore identity.p12 \
  -file tomcat.csr \
  -sigalg SHA256withRSA \
  -ext "SAN=dns:localhost,ip:127.0.0.1"

# Lecture avec OpenSSL
openssl req -in tomcat.csr -text -noout -verify
```

La CSR contient la clé publique, les informations demandées et une signature faite avec la clé privée. **Elle ne contient pas la clé privée**.

### 10.8 Importer une réponse de CA : le piège de l'alias

Après émission d'un certificat serveur par ta CA :

```bash
# Installer la réponse signée sous l'alias de la clé privée EXISTANTE.
keytool -importcert \
  -alias tomcat \
  -keystore identity.p12 \
  -file tomcat-signed.crt
```

- **Même alias `tomcat`** : `keytool` comprend qu'il s'agit de la réponse à la CSR et met à jour la chaîne de la `PrivateKeyEntry`.
- **Autre alias** (par ex. `certificat-final`) : il tente plutôt d'ajouter un `trustedCertEntry`, **sans associer le nouveau certificat à la clé privée**.

La réponse de CA doit être accompagnée d'une chaîne de confiance que Java puisse reconstruire (certificats de CA déjà approuvés, ou réponse PKCS#7/chaîne appropriée).

### 10.9 Convertir JKS vers PKCS#12

```bash
keytool -importkeystore \
  -srckeystore identity-legacy.jks \
  -srcstoretype JKS \
  -destkeystore converted.p12 \
  -deststoretype PKCS12

keytool -list -v -keystore converted.p12
```

Vérifie bien que les alias, les chaînes et les clés sont présents après conversion, puis teste l'application avant de remplacer l'ancien magasin.

### 10.10 Supprimer une entrée : prudence

```bash
# Visualiser les alias
keytool -list -keystore client-truststore.p12

# Supprimer UNE entrée de confiance, dans le labo
keytool -delete \
  -alias tomcat-local-approuve \
  -keystore client-truststore.p12
```

**Ne supprime jamais un `PrivateKeyEntry` de production sans sauvegarde :** la clé privée peut être irrécupérable.

---

## 11. Laboratoire de bout en bout : Root CA → Intermediate CA → certificat Tomcat

Ce **lab complet** reproduit les trois certificats de `Keystore.txt` sans dépendre d'une CA publique. **Tout se passe localement et uniquement pour apprendre.** Les commandes ont été testées avec OpenSSL 3 et `keytool` modernes. Les tailles RSA 3072 illustrent une configuration courante ; d'autres algorithmes peuvent être préférables selon les contraintes.

### 11.1 Préparer le laboratoire

```bash
mkdir -p ~/tls-chain-lab
cd ~/tls-chain-lab
umask 077

# Mot de passe de TEST, jamais pour la production.
# Le passage par l'option -storepass dans les commandes ci-dessous
# est volontairement simplifié pour rendre le LAB reproductible.
LAB_PASS='Changeit-2026-Strong'
```

### 11.2 Créer une Root CA de laboratoire

```bash
openssl genpkey -algorithm RSA \
  -pkeyopt rsa_keygen_bits:3072 \
  -out root.key

openssl req -x509 -new \
  -key root.key \
  -sha256 -days 3650 \
  -out root.crt \
  -subj "/CN=Formation Root CA/O=Lab TLS/C=FR" \
  -addext "basicConstraints=critical,CA:TRUE,pathlen:1" \
  -addext "keyUsage=critical,keyCertSign,cRLSign"
```

**Résultat :** `root.key` (clé **privée** de la CA) et `root.crt` (certificat **public** de la CA). En production, la clé Root CA doit être particulièrement protégée et on n'en crée pas une improvisée sur un serveur applicatif.

### 11.3 Créer une Intermediate CA

```bash
openssl genpkey -algorithm RSA \
  -pkeyopt rsa_keygen_bits:3072 \
  -out intermediate.key

openssl req -new -key intermediate.key \
  -out intermediate.csr \
  -subj "/CN=Formation Intermediate CA/O=Lab TLS/C=FR"
```

Créer `intermediate.ext` :

```ini
basicConstraints=critical,CA:TRUE,pathlen:0
keyUsage=critical,keyCertSign,cRLSign
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid,issuer
```

Signer le certificat intermédiaire avec la Root CA :

```bash
openssl x509 -req \
  -in intermediate.csr \
  -CA root.crt \
  -CAkey root.key \
  -CAcreateserial \
  -out intermediate.crt \
  -days 730 -sha256 \
  -extfile intermediate.ext
```

Tu as maintenant :

```text
root.crt                Root CA autosignée
intermediate.crt        CA intermédiaire signée par root.key
```

### 11.4 Créer la paire de clés serveur dans le Keystore Java

```bash
keytool -genkeypair \
  -alias tomcat \
  -keyalg RSA -keysize 3072 \
  -storetype PKCS12 \
  -keystore tomcat.p12 \
  -storepass "$LAB_PASS" \
  -validity 365 \
  -dname "CN=localhost,O=Lab TLS,C=FR" \
  -ext "SAN=dns:localhost,ip:127.0.0.1"

keytool -certreq \
  -alias tomcat \
  -keystore tomcat.p12 \
  -storepass "$LAB_PASS" \
  -file server.csr \
  -ext "SAN=dns:localhost,ip:127.0.0.1"
```

**Ne perds pas `tomcat.p12` :** la CA ne possède pas la clé privée serveur et ne pourra pas la recréer depuis la CSR.

### 11.5 Signer la CSR serveur avec l'Intermediate CA

Créer `server.ext` :

```ini
basicConstraints=critical,CA:FALSE
keyUsage=critical,digitalSignature,keyEncipherment
extendedKeyUsage=serverAuth
subjectAltName=DNS:localhost,IP:127.0.0.1
authorityKeyIdentifier=keyid,issuer
```

Puis :

```bash
openssl x509 -req \
  -in server.csr \
  -CA intermediate.crt \
  -CAkey intermediate.key \
  -CAcreateserial \
  -out server.crt \
  -days 180 -sha256 \
  -extfile server.ext
```

**Important :** dans ce laboratoire, nous jouons nous-mêmes le rôle de CA. En réalité, les autorités disposent de procédures d'émission et de gestion des identités et clés beaucoup plus strictes.

### 11.6 Vérifier la Trust Chain

```bash
openssl verify \
  -CAfile root.crt \
  -untrusted intermediate.crt \
  server.crt
```

Résultat attendu :

```text
server.crt: OK
```

Tu peux également examiner :

```bash
openssl x509 -in root.crt -noout -subject -issuer
openssl x509 -in intermediate.crt -noout -subject -issuer
openssl x509 -in server.crt -noout -subject -issuer -ext subjectAltName
```

### 11.7 Importer Root → Intermediate → Server dans le Keystore

Dans le scénario de `Keystore.txt`, `keytool` doit être capable de reconstruire la chaîne pour mettre à jour la `PrivateKeyEntry`.

```bash
# 1. Ajouter la Root CA validée
keytool -importcert -noprompt \
  -alias root-ca \
  -file root.crt \
  -keystore tomcat.p12 \
  -storepass "$LAB_PASS"

# 2. Ajouter l'Intermediate CA validée
keytool -importcert -noprompt \
  -alias intermediate-ca \
  -file intermediate.crt \
  -keystore tomcat.p12 \
  -storepass "$LAB_PASS"

# 3. IMPORTANT : importer server.crt sous LE MÊME ALIAS
#    que la clé privée créée à l'étape 11.4.
keytool -importcert \
  -alias tomcat \
  -file server.crt \
  -keystore tomcat.p12 \
  -storepass "$LAB_PASS"
```

**`-noprompt` n'est utilisé ici que pour le lab.** Pour une CA réelle, **vérifier indépendamment son identité et son empreinte** avant de l'ajouter à un magasin de confiance.

### 11.8 Contrôler le résultat

```bash
keytool -list -v \
  -alias tomcat \
  -keystore tomcat.p12 \
  -storepass "$LAB_PASS"
```

Résultat attendu (résumé) :

```text
Alias name: tomcat
Entry type: PrivateKeyEntry
Certificate chain length: 3
  Certificate[1] = Server Certificate
  Certificate[2] = Intermediate CA
  Certificate[3] = Root CA
```

La **clé privée** et la **chaîne** font désormais partie de la même entrée `tomcat`. En production, la Root CA n'a pas nécessairement besoin d'être incluse dans la chaîne envoyée par le serveur.

### 11.9 Créer un vrai Truststore client séparé

```bash
keytool -importcert \
  -alias formation-root \
  -file root.crt \
  -keystore client-truststore.p12 \
  -storetype PKCS12
```

Le client Java fait confiance à la Root CA. Le serveur présente sa chaîne. **C'est la séparation Identity Store / Truststore**.

### 11.10 Qu'a-t-on appris ?

- La **clé privée du serveur** n'est jamais envoyée à la Root ni à l'Intermediate CA.
- `server.csr` contient la demande et la clé publique.
- `root.crt`, `intermediate.crt` et `server.crt` ont des rôles différents.
- Le certificat signé est importé **sous l'alias de la clé privée existante**.
- Le Truststore client ne doit pas contenir la clé privée du serveur.

---

## 12. Apache HTTP Server : HTTPS global et Virtual Hosts

Ce chapitre reprend les laboratoires **RHEL/CentOS/Oracle Linux** du fichier `D-ApacheSSLConfiguration.txt` : installation `mod_ssl`, certificat autosigné global, plusieurs Virtual Hosts et certificat signé par CA. Les exemples de commandes sont actualisés et les permissions dangereuses du support sont corrigées.

### 12.1 Installation et vérifications

```bash
# RHEL / Rocky / AlmaLinux : noms de paquets de cette famille
sudo dnf install -y httpd mod_ssl openssl

# Module SSL actif
sudo httpd -M | grep ssl

# Vérifier le service
sudo systemctl status httpd

# Vérifier la configuration avant de (re)charger
sudo apachectl configtest
```

Sous **Ubuntu/Debian**, le service s'appelle généralement `apache2`, les Virtual Hosts se trouvent sous `/etc/apache2/sites-available/`, et le module peut être activé avec `sudo a2enmod ssl`.

### 12.2 Global SSL Configuration vs Virtual Host SSL

**Global Level :** configuration SSL commune dans un fichier de configuration serveur, par exemple `/etc/httpd/conf.d/ssl.conf` sur certaines distributions. Utile pour les paramètres généraux ou un unique site par défaut.

**Virtual Host Level :** chaque site dispose d'un `ServerName`, d'un `DocumentRoot` et éventuellement d'une paire certificat/clé propres :

```text
Apache httpd :443
  ├── site1.example.test → certificat 1, document root 1
  └── site2.example.test → certificat 2, document root 2
```

Le navigateur envoie un **SNI** afin qu'Apache puisse sélectionner le bon certificat pour un Virtual Host.

#### 12.2.1 LAB complémentaire — Certificat autosigné au « Global Level » (captures Udemy, section 14)

Les nouvelles captures montrent un premier scénario sous **RHEL / Rocky / AlmaLinux** : installer `mod_ssl`, créer `localhost.key` et `localhost.crt`, puis utiliser les chemins dans `/etc/httpd/conf.d/ssl.conf`. Il s'agit généralement du **site HTTPS par défaut** configuré par le paquet, pas d'un certificat automatiquement partagé par tous les Virtual Hosts : chaque Virtual Host peut définir sa propre identité TLS.

**Étape 1 — Vérifier ce qui existe avant de modifier la configuration :**

```bash
sudo dnf install -y httpd mod_ssl openssl
sudo httpd -M | grep ssl

# Selon la distribution, mod_ssl charge son module depuis
# /etc/httpd/conf.modules.d/00-ssl.conf, et peut fournir ssl.conf.
sudo ls -l /etc/httpd/conf.modules.d/00-ssl.conf /etc/httpd/conf.d/ssl.conf

# Repérer les écoutes et certificats DÉJÀ configurés :
sudo grep -RnE '^[[:space:]]*(Listen[[:space:]]+443|SSLCertificateFile|SSLCertificateKeyFile)' \
  /etc/httpd/conf /etc/httpd/conf.d /etc/httpd/conf.modules.d 2>/dev/null
```

Le nom `00-ssl.conf` et le contenu de `ssl.conf` dépendent des paquets installés. Le module SSL apparaît normalement sous la forme `ssl_module (shared)` dans `httpd -M`.

**Étape 2 — Créer une identité TLS de test sans écraser les certificats existants :**

```bash
sudo install -d -m 700 -o root -g root /etc/httpd/tls
sudo openssl req -x509 -newkey rsa:3072 -noenc -sha256 -days 30 \
  -keyout /etc/httpd/tls/localhost.key \
  -out /etc/httpd/tls/localhost.crt \
  -subj '/C=FR/O=Lab/CN=localhost' \
  -addext 'subjectAltName=DNS:localhost,IP:127.0.0.1' \
  -addext 'basicConstraints=critical,CA:FALSE'

sudo chown root:root /etc/httpd/tls/localhost.key /etc/httpd/tls/localhost.crt
sudo chmod 600 /etc/httpd/tls/localhost.key
sudo chmod 644 /etc/httpd/tls/localhost.crt
```

- `-x509` : produire un certificat **autosigné** plutôt qu'une CSR ;
- `-newkey rsa:3072` : générer une **nouvelle clé privée RSA** ;
- `-noenc` : ne pas chiffrer cette clé avec une passphrase (nom ancien : `-nodes` ; réserver ce choix au contexte contrôlé du laboratoire) ;
- `-keyout` / `-out` : chemins de la **Private Key** et du **Server Certificate** ;
- `-subj` : **Distinguished Name (DN)** ; `-addext subjectAltName` : noms DNS/IP réellement vérifiés par les clients modernes.

**Étape 3 — Renseigner le certificat dans la configuration HTTPS existante :**

Dans `/etc/httpd/conf.d/ssl.conf`, modifier **les directives du site HTTPS par défaut déjà présent** (souvent un bloc `<VirtualHost _default_:443>`) ; ne pas coller aveuglément un second bloc ni une deuxième directive `Listen` :

```apache
# À placer dans le contexte HTTPS approprié, par exemple
# à l'intérieur du VirtualHost _default_:443 existant :
SSLEngine on
SSLCertificateFile    /etc/httpd/tls/localhost.crt
SSLCertificateKeyFile /etc/httpd/tls/localhost.key
```

Confirmer qu'Apache **écoute déjà sur le port 443** (`Listen 443` ou `Listen 443 https`). Si ce n'est pas le cas, ajouter cette directive **une seule fois** dans la configuration appropriée ; le mot `https` est facultatif sur le port 443. Adapter au besoin les contextes **SELinux** des fichiers.

**Étape 4 — Contrôler et recharger, puis vérifier le résultat réseau :**

```bash
sudo apachectl configtest   # attendu : Syntax OK
sudo apachectl -S           # identifier les Virtual Hosts effectifs
sudo apachectl graceful     # rechargement progressif, si la config est valide

# Test local : le certificat autosigné n'est pas approuvé par défaut.
openssl s_client -connect localhost:443 -servername localhost \
  -showcerts -verify_hostname localhost </dev/null
```

**À ne pas confondre :** `SSL global/default` sert ici à la configuration du serveur ou du site HTTPS par défaut ; `SSL Virtual Host` permet à `site1.example.test` et `site2.example.test` de présenter des certificats différents sur **la même IP et le même port 443** grâce au **SNI**. Les captures « Virtual Host » sont déjà reprises dans les sous-sections **12.3 à 12.6** ci-dessous.

### 12.3 Générer un certificat autosigné de test

```bash
sudo install -d -m 700 -o root -g root /etc/httpd/tls

# Créer directement dans un répertoire protégé, pas dans /tmp.
sudo openssl req -x509 -newkey rsa:3072 -noenc \
  -keyout /etc/httpd/tls/site1.key \
  -out /etc/httpd/tls/site1.crt \
  -days 30 -sha256 \
  -subj "/C=FR/O=Lab/CN=site1.example.test" \
  -addext "subjectAltName=DNS:site1.example.test" \
  -addext "basicConstraints=critical,CA:FALSE"

sudo chown root:root /etc/httpd/tls/site1.key /etc/httpd/tls/site1.crt
sudo chmod 600 /etc/httpd/tls/site1.key
sudo chmod 644 /etc/httpd/tls/site1.crt
```

**Précision :** le répertoire de clés est protégé. Sur une machine avec **SELinux**, il peut aussi être nécessaire d’appliquer les contextes de sécurité adaptés à Apache. Vérifier l’accès du service aux clés sans rendre le répertoire publiquement lisible.

### 12.4 Créer le contenu du site

```bash
sudo install -d -m 755 /var/www/site1/html
printf 'Bonjour HTTPS depuis Apache\n' | \
  sudo tee /var/www/site1/html/index.html
```

### 12.5 Configurer un Virtual Host HTTPS

Créer par exemple `/etc/httpd/conf.d/site1-https.conf` :

```apache
<VirtualHost *:80>
    ServerName site1.example.test
    Redirect permanent / https://site1.example.test/
</VirtualHost>

<VirtualHost *:443>
    ServerName site1.example.test
    DocumentRoot /var/www/site1/html

    SSLEngine on
    SSLCertificateFile    /etc/httpd/tls/site1.crt
    SSLCertificateKeyFile /etc/httpd/tls/site1.key

    <Directory /var/www/site1/html>
        Options -Indexes +FollowSymLinks
        AllowOverride None
        Require all granted
    </Directory>

    ErrorLog  /var/log/httpd/site1_ssl_error.log
    CustomLog /var/log/httpd/site1_ssl_access.log combined
</VirtualHost>
```

Vérifier et charger avec interruption minimale :

```bash
sudo apachectl configtest
sudo apachectl graceful
```

**`graceful`** redémarre progressivement les processus Apache en conservant autant que possible les connexions en cours. Ce n'est pas la même chose qu'un arrêt brutal suivi d'un démarrage.

Pour tester depuis la machine locale, tu peux déclarer dans le fichier `hosts` :

```text
127.0.0.1 site1.example.test
```

Puis :

```bash
curl -vk https://site1.example.test/
```

Avec un certificat autosigné non approuvé, le client remonte normalement un **problème de confiance**. `-k` n'est toléré ici qu'en **test** ; ne le transforme pas en solution de production.

### 12.6 Deux sites HTTPS sur le même serveur

Le fichier de cours contient deux Virtual Hosts sur le port 443. Pour ajouter un second site :

- Créer une deuxième clé privée et un certificat avec `SAN=DNS:site2.example.test`.
- Créer un deuxième `DocumentRoot`, par exemple `/var/www/site2/html`.
- Ajouter un autre `<VirtualHost *:443>` avec `ServerName site2.example.test`, `SSLCertificateFile` et `SSLCertificateKeyFile` pointant vers les **bons fichiers**.
- Ajouter les résolutions DNS/`hosts` nécessaires, vérifier `apachectl configtest`, puis `apachectl graceful`.

L'utilisation du même port et de la même IP est possible grâce à **SNI**.

### 12.7 Installer un certificat signé par une CA

Dans le cours, l'ordre est :

```text
server.key (clé privée)
   → server.csr (demande)
   → CA
   → server.crt + ca_bundle.crt
   → Apache
```

Si la CA fournit `server.crt` et une chaîne d'intermédiaires `intermediate.crt`, construire :

```bash
cat server.crt intermediate.crt > fullchain.crt
```

Puis configurer Apache moderne :

```apache
SSLEngine on
SSLCertificateFile    /etc/httpd/tls/fullchain.crt
SSLCertificateKeyFile /etc/httpd/tls/site1.key
```

**Correction importante :** `SSLCertificateChainFile`, cité dans le support, est **obsolète depuis Apache 2.4.8**. La documentation Apache recommande de mettre le certificat serveur **suivi des intermédiaires** dans le fichier indiqué par `SSLCertificateFile`. Ne mélange pas le certificat racine dans ce `fullchain` sans nécessité ; la racine de confiance relève du client.

**Permissions :** ne reproduis pas `chmod -R 755 /etc/ssl` ni `chown -R apache:apache /etc/ssl` de la formation. Ce serait trop permissif pour les clés privées et affecterait des fichiers ne dépendant pas forcément de cette application. Les fichiers de clé devraient être détenus par un compte approprié (souvent `root`) avec des droits minimaux, compatibles avec la manière dont Apache ouvre les clés au démarrage et les contrôles SELinux éventuels.

### 12.8 Après configuration : vérifier ce qui est réellement servi

```bash
sudo apachectl configtest
sudo apachectl graceful

openssl s_client \
  -connect site1.example.test:443 \
  -servername site1.example.test \
  -showcerts \
  -verify_hostname site1.example.test </dev/null
```

`openssl s_client` teste le **serveur réellement accessible** ; `openssl x509 -in fichier.crt` inspecte seulement un **fichier local**. Les deux résultats peuvent différer si Apache a chargé un autre certificat ou si un load balancer termine TLS en amont.

---

## 13. Java, Tomcat, Spring Boot et reverse proxy

### 13.1 Tomcat gère directement HTTPS

```text
Navigateur -- HTTPS:8443 --> Tomcat
                             └── identity.p12
                                 ├── Private Key
                                 └── Server Certificate + Chain
```

Exemple de configuration **Tomcat 9**, dans `conf/server.xml` (syntaxe dépendant de la version et du connecteur) :

```xml
<Connector port="8443"
           protocol="org.apache.coyote.http11.Http11NioProtocol"
           SSLEnabled="true"
           scheme="https"
           secure="true">
    <SSLHostConfig protocols="TLSv1.2,TLSv1.3">
        <Certificate
            certificateKeystoreFile="/opt/tomcat/conf/identity.p12"
            certificateKeystoreType="PKCS12"
            certificateKeystorePassword="REMPLACER_PAR_UN_SECRET"
            certificateKeyAlias="tomcat"
            type="RSA" />
    </SSLHostConfig>
</Connector>
```

**Attention :** `REMPLACER_PAR_UN_SECRET` est un **placeholder pédagogique**. En production, externaliser et protéger les secrets selon la plateforme et la version de Tomcat, ne pas les déposer dans un dépôt Git.

Tester ensuite :

```bash
# Fichier utilisé localement par Tomcat
keytool -list -v -keystore /opt/tomcat/conf/identity.p12

# Certificat réellement présenté par Tomcat
openssl s_client -connect localhost:8443 \
  -servername localhost -showcerts </dev/null
```

**Rechargement** : certains Tomcat modernes disposent d'opérations pour recharger la configuration SSL, par exemple `reloadSslHostConfigs` ou des interfaces de gestion adaptées. Cela doit être vérifié pour la **version précise**, le connecteur et le mode de déploiement ; ne pas supposer qu'un simple remplacement de `identity.p12` sera automatiquement pris en compte.

### 13.2 Spring Boot : Identity Keystore et Truststore

Exemple simplifié d'un serveur Spring Boot gérant TLS directement :

```properties
server.port=8443
server.ssl.enabled=true
server.ssl.key-store=file:/etc/myapp/identity.p12
server.ssl.key-store-type=PKCS12
server.ssl.key-alias=server
# Configurer le mot de passe depuis un gestionnaire de secrets
# ou la configuration externalisée appropriée.
```

Pour une **application Java cliente** (sortie HTTPS vers une API interne), on peut définir un Truststore JVM :

```bash
java \
  -Djavax.net.ssl.trustStore=/etc/myapp/client-truststore.p12 \
  -Djavax.net.ssl.trustStoreType=PKCS12 \
  -jar mon-client.jar
```

Si le Truststore est protégé, la JVM doit recevoir le mot de passe par une méthode approuvée. Attention : un Truststore personnalisé peut **remplacer** la sélection par défaut de CA selon les APIs et la configuration. N'importe quelle chaîne publique ne sera pas forcément reconnue si les racines nécessaires n'y figurent pas.

### 13.3 Architecture courante : Apache/Nginx devant Tomcat

```text
Utilisateur navigateur
       |
       | HTTPS 443
       v
Reverse Proxy Apache/Nginx
  ├── server.key
  └── fullchain.crt
       |
       | HTTP 8080 sur réseau interne protégé
       | (ou HTTPS/mTLS interne si exigé)
       v
Tomcat / Application Java
```

Dans ce cas, **le certificat public est détenu par le reverse proxy**, pas nécessairement par Tomcat. Il faut maîtriser la configuration `X-Forwarded-*` / `Forwarded`, les noms d'hôtes, redirections et cookies sécurisés, et **ne faire confiance qu'aux proxys autorisés**.

### 13.4 Pour diagnostiquer une application Jira/Tomcat existante

Sans présumer de son architecture, commencer par identifier :

1. Le point exact de **TLS Termination** : Tomcat, Apache, Nginx, IIS, reverse proxy ou Load Balancer ?
2. Le **hostname** réellement utilisé par les utilisateurs et le **certificat présenté**.
3. Le **fichier Keystore ou PEM** et le compte de service qui y accède.
4. La présence éventuelle de **CA d'entreprise** et des Truststores Java.
5. Les versions Java/Tomcat ; leur compatibilité TLS ; la méthode de rotation et de **rollback**.
6. La chaîne d'appels sortants de Jira vers des applications métiers : leur `PKIX path building failed` peut concerner le Truststore **client** de Jira, et non le certificat HTTPS présenté aux utilisateurs.

**Commandes de diagnostic en lecture seule** :

```bash
# Depuis un poste autorisé à accéder au service
openssl s_client -connect jira.example.test:443 \
  -servername jira.example.test -showcerts </dev/null

# Sur le serveur Java, vérifier les fichiers réellement configurés
# (Chemins à adapter ; aucun changement de configuration ici.)
keytool -list -v -keystore /chemin/vers/identity.p12
```

---
## 14. Certificats publics, Let's Encrypt et renouvellement sans interruption

### 14.1 Comment obtenir un certificat pour un site public ?

Pour un nom de domaine que tu contrôles (`app.mondomaine.fr`), une autorité publique comme **Let's Encrypt** peut émettre gratuitement un **Domain Validation certificate (DV)**. La preuve habituelle est le contrôle du domaine ; elle ne valide pas la moralité d'une organisation.

Les étapes sont :

```text
Nom DNS public → Serveur IP publique → Validation ACME
                                      → Certificat + chaîne
                                      → Configuration HTTPS
                                      → Renouvellement automatique
```

Une autorité ACME valide le contrôle du domaine via un **challenge** :

- **HTTP-01** : réponse vérifiable sur le port HTTP 80 du domaine.
- **DNS-01** : enregistrement TXT dans le DNS ; indispensable notamment pour les **wildcard certificates** (`*.mondomaine.fr`).
- **TLS-ALPN-01** : autre méthode possible selon le client et l'environnement.

Les noms locaux comme `localhost`, `monservice.local` ou `app.example.test` du laboratoire ne sont **pas** des domaines publics pour lesquels obtenir simplement un certificat public ; ils servent aux démonstrations ou à une PKI interne.

### 14.2 Exemple Certbot + Nginx sur un vrai domaine

Pré-requis : DNS correct, Nginx installé et configuré, ports nécessaires accessibles, domaine réellement détenu.

```bash
# Sur une distribution où Certbot est déjà correctement installé :
sudo certbot --nginx -d app.mondomaine.fr

# Liste des certificats gérés
sudo certbot certificates

# Simuler le renouvellement
sudo certbot renew --dry-run
```

L'installation initiale de Certbot dépend de la distribution et du mode de packaging ; suivre la documentation officielle du système cible.

Après émission, les fichiers sont généralement accessibles sous :

```text
/etc/letsencrypt/live/app.mondomaine.fr/
  privkey.pem     # clé privée
  cert.pem        # certificat final
  chain.pem       # intermédiaires
  fullchain.pem   # certificat final + intermédiaires
```

Ces chemins sont généralement des **liens symboliques** vers des versions dans `/etc/letsencrypt/archive/` ; ne remplace pas à la main leur contenu.

### 14.3 Le renouvellement sans downtime : ce qu'il faut distinguer

**Renouveler** le certificat ≠ **recharger** le certificat dans le serveur.

1. Certbot obtient de nouveaux fichiers.
2. Le serveur web doit charger les nouveaux fichiers.
3. Si le serveur supporte un **graceful reload** / rechargement dynamique, il peut le faire sans interruption prévue des requêtes en cours.

Avec Nginx :

```bash
sudo nginx -t && sudo systemctl reload nginx
```

Avec Apache :

```bash
sudo apachectl configtest && sudo apachectl graceful
```

Avec Tomcat directement : vérifier sa méthode de **SSL certificate reload** et sa version ; si elle n'existe pas, prévoir une stratégie de redémarrage contrôlé avec plusieurs instances, ou terminer TLS en amont.

### 14.4 Certbot deploy hook

Pour un déploiement où Nginx a besoin d'être rechargé après chaque renouvellement réussi :

```bash
# Exemple de hook que l'on peut installer de façon durable
# dans /etc/letsencrypt/renewal-hooks/deploy/reload-nginx.sh :
#!/bin/sh
nginx -t && systemctl reload nginx
```

Le fichier doit être exécutable et installé avec les permissions adaptées. Les clients Certbot et plugins peuvent déjà gérer certaines opérations de rechargement : ne duplique pas inutilement les hooks. **Le `deploy-hook` ne s'exécute qu'après le renouvellement réussi** dans le flux de Certbot.

### 14.5 Durée de vie et supervision

**Au 15 novembre 2026**, la durée standard des certificats Let's Encrypt est de **90 jours**. Let's Encrypt a annoncé le passage à **64 jours par défaut le 10 février 2027**, puis une diminution supplémentaire ultérieure. **Ne code jamais « renouveler exactement tous les 90 jours ».** Laisse le client ACME suivre les échéances ; surveille l'expiration et les échecs.

Les contrôles indispensables en production :

- Certificat effectivement présenté sur `https://domaine:443`, pas seulement celui présent sur disque.
- Date `Not After` et SAN.
- Échec de challenge ACME, DNS incorrect, port 80 bloqué, droits sur fichiers, quota/limites de l'autorité.
- Reload de Nginx/Apache/Gateway après renouvellement et métriques d'expiration.
- Sauvegardes et gestion de rotation des **clés privées**.

### 14.6 Est-ce possible sans aucune interruption ?

**Oui, normalement, si l'architecture supporte la rotation à chaud**, mais aucune commande isolée ne garantit le « zéro downtime » de toute l'application : il faut aussi considérer les mises à jour applicatives, les erreurs de configuration, le load balancer, les connexions longues et les dépendances.

---

## 15. Docker, CI/CD, Kubernetes et cert-manager

### 15.1 Idée essentielle : séparer certificat et application

Dans un système moderne, on évite souvent de placer le certificat TLS public dans **chaque image Docker**.

```text
                     Internet (HTTPS)
                            |
                    Reverse Proxy / Gateway
                      (certificat TLS)
                            |
                       Service interne
                          /   |   \
                      Pod1  Pod2  Pod3
                     Java   Java   Java
```

- **Docker** : empaquette et exécute l'application.
- **CI/CD** : construit, teste et déploie des versions applicatives.
- **Reverse Proxy/Gateway** : termine la connexion HTTPS publique.
- **Certbot/Traefik/cert-manager/Cloud ACM** : émet et renouvelle les certificats.
- **Kubernetes Secret TLS** : contient le certificat et sa clé privée lorsque la terminaison est dans Kubernetes.

Ainsi, le renouvellement d'un certificat ne nécessite généralement **pas** de reconstruire l'image de l'application.

### 15.2 Docker Compose avec Traefik (architecture pédagogique)

Traefik peut gérer ACME et stocker son état dans un volume persistant. Exemple indicatif de `compose.yaml` :

```yaml
services:
  reverse-proxy:
    image: traefik:v3   # En production : fixer une version précise / un digest
    command:
      - --providers.docker=true
      - --providers.docker.exposedbydefault=false
      - --entrypoints.web.address=:80
      - --entrypoints.websecure.address=:443
      - --certificatesresolvers.le.acme.email=admin@mondomaine.fr
      - --certificatesresolvers.le.acme.storage=/letsencrypt/acme.json
      - --certificatesresolvers.le.acme.httpchallenge.entrypoint=web
    ports:
      - "80:80"
      - "443:443"
    volumes:
      - /var/run/docker.sock:/var/run/docker.sock:ro
      - ./letsencrypt:/letsencrypt

  application:
    image: monorganisation/monapp:1.0.0
    labels:
      - traefik.enable=true
      - traefik.http.routers.application.rule=Host(`app.mondomaine.fr`)
      - traefik.http.routers.application.entrypoints=websecure
      - traefik.http.routers.application.tls.certresolver=le
      - traefik.http.services.application.loadbalancer.server.port=8080
```

**Pré-requis :** `app.mondomaine.fr` doit être un vrai nom détenu et résolu vers le serveur ; ouvrir les ports 80 et 443. Le stockage ACME doit survivre à la recréation du conteneur. Il faut appliquer des permissions adaptées à `acme.json`.

**Point de sécurité :** même monté en lecture seule, le **socket Docker** peut donner des capacités d'administration très puissantes au processus qui l'utilise. En production, envisager un proxy Docker Socket avec permissions réduites ou une architecture évitant d'exposer le daemon Docker au composant frontal.

**Disponibilité :** Traefik peut renouveler et recharger le certificat sans redémarrer le conteneur Java. Mais un **seul** reverse proxy reste un point unique de défaillance : le « zéro downtime » au niveau du service demande une architecture résiliente.

### 15.3 Kubernetes : cert-manager et TLS Secret

**Pré-requis :** cluster K8s opérationnel, cert-manager installé, contrôleur Ingress/Gateway maintenu, domaine public et DNS, accès réseau pour les challenges. Exemple avec une classe d'Ingress appelée `traefik` : l'adapter au contrôleur réellement installé.

#### Déclarer un ClusterIssuer ACME

```yaml
apiVersion: cert-manager.io/v1
kind: ClusterIssuer
metadata:
  name: letsencrypt-prod
spec:
  acme:
    email: admin@mondomaine.fr
    server: https://acme-v02.api.letsencrypt.org/directory
    privateKeySecretRef:
      name: letsencrypt-account
    solvers:
      - http01:
          ingress:
            ingressClassName: traefik
```

**En pratique :** commencer par l'endpoint **staging** de Let's Encrypt pour tester l'automatisation, puis passer en production. Les certificats staging ne sont généralement pas approuvés par les navigateurs.

#### Déclarer un certificat

```yaml
apiVersion: cert-manager.io/v1
kind: Certificate
metadata:
  name: monapp-cert
  namespace: apps
spec:
  secretName: monapp-tls
  dnsNames:
    - app.mondomaine.fr
  issuerRef:
    name: letsencrypt-prod
    kind: ClusterIssuer
```

#### Déclarer un Ingress utilisant le Secret

```yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: monapp-ingress
  namespace: apps
spec:
  ingressClassName: traefik
  tls:
    - hosts:
        - app.mondomaine.fr
      secretName: monapp-tls
  rules:
    - host: app.mondomaine.fr
      http:
        paths:
          - path: /
            pathType: Prefix
            backend:
              service:
                name: monapp
                port:
                  number: 80
```

Le **Secret** `monapp-tls` est créé/actualisé par cert-manager après émission, dans le namespace `apps` :

```text
Secret Kubernetes (type kubernetes.io/tls)
  tls.crt  = certificat et chaîne
  tls.key  = clé privée
```

Les données d'un Secret ne sont **pas sécurisées simplement parce qu'elles sont encodées en base64**. Appliquer RBAC strict, protection d'etcd, chiffrement au repos et règles d'accès appropriées.

#### Commandes de diagnostic Kubernetes

```bash
kubectl get clusterissuer
kubectl get certificates -n apps
kubectl describe certificate monapp-cert -n apps
kubectl get secret monapp-tls -n apps
kubectl get ingress -n apps
kubectl get challenges,orders -A

# Éviter d'afficher la clé privée en clair dans les logs ou le terminal.
```

### 15.4 Renouvellement Kubernetes

```text
cert-manager détecte l'échéance
          ↓
ACME délivre le nouveau certificat
          ↓
cert-manager met à jour monapp-tls
          ↓
Ingress/Gateway observe le nouveau Secret
          ↓
Le contrôleur compatible recharge TLS
          ↓
Les Pods Java ne redémarrent pas
```

**Nuance :** la mise à jour du Secret est automatique, mais le **rechargement à chaud du Gateway** dépend du contrôleur et doit être testé. Certains systèmes terminent TLS sur un Load Balancer **externe**, qui gère lui-même ses propres certificats.

### 15.5 CI/CD et certificat : deux cycles de vie

```text
CI/CD applicatif :
  Git push → tests → build Docker → registry → rolling update des Pods

Gestion TLS :
  cert-manager → ACME/CA → Kubernetes Secret → recharge du Gateway
```

Pour obtenir des déploiements avec peu ou pas d'interruption : plusieurs réplicas, **readiness probes**, Rolling Update, ressources suffisantes, drain correct et supervision.

### 15.6 Autres architectures courantes

| Architecture | Gestion TLS typique | Redémarrage de l'app lors de la rotation ? |
|---|---|---|
| Docker Compose + Traefik | ACME intégré à Traefik | Non, si TLS termine chez Traefik |
| Docker Compose + Nginx | Certbot + rechargement Nginx | Non |
| Kubernetes + Ingress/Gateway | cert-manager + TLS Secret | Normalement non, si reload dynamique |
| AWS Load Balancer + ACM | Certificat managé AWS | Non pour le backend |
| Tomcat direct | PKCS#12 + processus de rotation/reload | Dépend de la version/configuration |

---

## 16. mTLS : authentification réciproque

**TLS classique** : le **serveur** présente un certificat au client. Le client se fie à une CA et vérifie le hostname.

**mTLS — Mutual TLS** : le **client présente aussi son certificat** ; le serveur le vérifie. Fréquent dans les microservices, applications d'entreprise et API internes fortement contrôlées.

```text
CLIENT                                         SERVEUR
Identity Store (clé + cert client)            Identity Store (clé + cert serveur)
Truststore (CA serveur)                       Truststore (CA clients)
           |                                             |
           |<-------- certificat serveur -----------------|
           |--------- certificat client ----------------->|
           |<========= canal TLS authentifié ============>|
```

| Magasin | Côté client mTLS | Côté serveur mTLS |
|---|---|---|
| **Identity Keystore** | Clé et certificat du client | Clé et certificat du serveur |
| **Truststore** | CA qui a signé le certificat serveur | CA autorisée à signer les certificats clients |

**Point important :** le certificat client peut nécessiter l'usage **`clientAuth`**. Un certificat public DV pour serveur web n'est pas automatiquement adapté à l'authentification client. Dans la pratique, les entreprises utilisent souvent une **CA privée** pour le mTLS interne.

### 16.1 Exemple de test de connexion mTLS avec OpenSSL

```bash
openssl s_client \
  -connect api-interne.example.test:443 \
  -servername api-interne.example.test \
  -cert client.crt \
  -key client.key \
  -CAfile ca-serveur.crt \
  -verify_hostname api-interne.example.test \
  -verify_return_error
```

Cette commande suppose que tu possèdes réellement une paire clé/certificat client appropriée et l'autorisation de tester ce service. Elle illustre aussi la différence : **le certificat client prouve l'identité du client**, `-CAfile` sert à vérifier le serveur.

---

## 17. Troubleshooting : diagnostic méthodique

Avant de modifier un Keystore, poser **cinq questions** :

1. **Qui initie la connexion ?** Navigateur, application Java, reverse proxy ?
2. **Qui termine TLS ?** Tomcat, Apache, Nginx, Load Balancer, Ingress ?
3. **Quel hostname et quel port ?** SNI et certificat doivent correspondre.
4. **Quel certificat est réellement présenté ?** Il peut être différent de celui sur disque.
5. **Quel Truststore le client utilise-t-il ?** Java, navigateur, OS et OpenSSL n'utilisent pas toujours la même sélection de CA.

### 17.1 Procédure de première analyse

```bash
# 1. Est-ce que le service répond et avec quel certificat ?
openssl s_client -connect app.example.com:443 \
  -servername app.example.com \
  -showcerts \
  -verify_hostname app.example.com \
  -verify_return_error </dev/null

# 2. Quel est le certificat enregistré localement ?
openssl x509 -in server.crt -noout -subject -issuer -dates

# 3. Quel alias/quelle chaîne existe dans le Keystore Java ?
keytool -list -v -keystore identity.p12

# 4. En cas de CA interne, contrôler sa chaîne explicitement
openssl verify -CAfile root.crt -untrusted intermediate.crt server.crt
```

### 17.2 Tableau d'erreurs fréquentes

| Symptôme | Cause possible | Quoi vérifier en premier ? |
|---|---|---|
| `PKIX path building failed` | Java ne trouve pas de chaîne menant à une CA approuvée | Truststore client ; intermédiaires ; CA internes |
| `unable to get local issuer certificate` | Intermediate manquant / CA non approuvée | Chaîne envoyée ; CAfile ; Truststore |
| `self-signed certificate` | Certificat autosigné non approuvé | Origine du cert ; nécessité d'une CA approuvée |
| `certificate has expired` | Date de fin dépassée | `openssl x509 -enddate` ; renouvellement et reload |
| `hostname mismatch` | SAN ne correspond pas à l'URL | Nom demandé, SAN, SNI, proxy |
| `handshake_failure` | Protocole, cipher, certificat client ou politique incompatible | Versions TLS, Cipher Suites, mTLS, logs |
| `bad_certificate` | Certificat rejeté par le pair | Émetteur, EKU, validité, révocation, mTLS |
| `Keystore was tampered with, or password incorrect` | Mauvais mot de passe ou magasin incompatible | Mot de passe, format réel, type attendu |
| `Alias <...> does not exist` | Mauvais alias | `keytool -list -keystore ...` |
| Certificat « ancien » après renouvellement | Serveur/proxy n'a pas rechargé ou fichier différent | `s_client` vs fichier local, endpoint TLS |
| HTTP fonctionne, HTTPS échoue | Certificat, port 443, firewall, bind ou TLS | `ss -ltn`, logs du proxy, TLS handshake |

### 17.3 Cas pratique A — Java client vers API interne

**Symptôme** : `javax.net.ssl.SSLHandshakeException: PKIX path building failed`.

**Lecture :** l'application Java cliente ne peut pas construire une chaîne vers une CA de confiance. Il ne faut **pas** commencer par désactiver la vérification des certificats.

**Diagnostic :**

```bash
openssl s_client -connect api.example.test:443 \
  -servername api.example.test -showcerts </dev/null

# Quel truststore utilise l'application ?
# Chercher javax.net.ssl.trustStore dans les paramètres JVM.
keytool -list -v -keystore /chemin/truststore.p12
```

**Correction possible :** réparer la chaîne présentée par l'API **ou** ajouter la bonne CA interne, préalablement authentifiée, au Truststore **du client Java**. Selon la configuration, la CA peut déjà être reconnue par le navigateur mais **pas** par la JVM.

### 17.4 Cas pratique B — Certificat nouveau dans le Keystore, ancien au navigateur

```text
identity.p12 sur le disque : certificat valable jusqu'en 2027
openssl s_client          : certificat expiré en 2026
```

Vérifier successivement :

1. Le `certificateKeystoreFile`/alias réellement configuré par Tomcat.
2. Le processus Tomcat a-t-il **rechargé** le certificat ?
3. Un proxy/LB termine-t-il TLS **avant** Tomcat ?
4. La commande `s_client` utilise-t-elle **le bon SNI** ?
5. Les réplicas ont-ils tous été mis à jour ?

### 17.5 Cas pratique C — Certificat valide mais mauvais nom

Un certificat peut être **non expiré, signé par une CA approuvée**, mais inutilisable pour `api.example.com` si son SAN ne contient que `www.example.com`.

```bash
openssl x509 -in server.crt -noout -ext subjectAltName
openssl s_client -connect api.example.com:443 \
  -servername api.example.com \
  -verify_hostname api.example.com \
  -verify_return_error </dev/null
```

**Correction** : réémettre le certificat avec les SAN corrects ; ne pas contourner la vérification du nom en production.

### 17.6 Activer le debug TLS côté Java

Pour un environnement de développement ou une investigation contrôlée :

```bash
java -Djavax.net.debug=ssl,handshake \
  -jar mon-application.jar
```

Les traces peuvent être volumineuses et contenir des métadonnées sensibles. Ne pas les laisser activées durablement en production ; préférer les procédures de l'équipe d'exploitation.

---

## 18. Erreurs et limites du cours Udemy : corrections importantes

Les exemples de formation sont utiles, mais ne doivent pas être copiés aveuglément en production.

| Dans le support | Correction / justification |
|---|---|
| `OpenSSL` « ne peut pas créer de PKCS#12 » (tableau p.5 d'`OpenSSL.pdf`) | **Faux :** `openssl pkcs12 -export` crée un `.p12`/`.pfx` ; confirmé par la p.12 du même PDF. |
| `SSLCertificateChainFile ...` dans Apache | **Obsolète depuis Apache 2.4.8** ; utiliser `SSLCertificateFile` avec `server + intermediate(s)` en PEM. |
| `chmod -R 755 /etc/ssl` | **Dangereux :** rend la traversée et la lecture trop ouvertes selon les fichiers ; permission stricte sur les clés. |
| `chown -R apache:apache /etc/ssl` ou `/etc/httpd` | **Dangereux :** ne pas transférer la propriété de tout le répertoire SSL/Apache au compte de service. |
| Certificat créé avec seulement `CN=localhost` | Ajouter un **SAN** (`DNS:localhost`, etc.) pour les vérifications modernes. |
| `keytool -genkey` | Ancien alias ; préférer `keytool -genkeypair`. |
| `keytool -export` / `-import` | Préférer les noms explicites `-exportcert` / `-importcert`. |
| Nouveau fichier `.jks` sans `-storetype JKS` | Ne pas déduire le format de l'extension ; préciser `-storetype` (PKCS#12 est le défaut Java moderne). |
| `-keypass` différent de `-storepass` avec PKCS#12 | Avec le provider usuel de `keytool`, utiliser le même mot de passe ; les différences sont ignorées avec avertissement. |
| Capture CSR PKCS#12 : `-storepass test321` | Incohérent avec le Keystore PKCS#12 créé avec `test123` ; la capture suivante corrige cette valeur. |
| Dans `Keystore.txt`, commande PKCS#12 intermédiaire ciblant `.jks` | Incohérence de chemin/format ; viser le **vrai** fichier PKCS#12. |
| `-trustcacerts` pendant l'import | N'accorde pas automatiquement une confiance sûre ; **contrôler l'origine et l'empreinte de la CA**. |
| Import d'un certificat sous un autre alias que la clé privée | Crée généralement une entrée `trustedCertEntry`, **pas** une mise à jour de la `PrivateKeyEntry`. |
| Root et Intermediate « obligatoirement » dans tous les Truststores | **Nuance :** le client a besoin d'un **trust anchor** approuvé et d'une chaîne complète ; le serveur doit généralement envoyer les intermédiaires. |
| CSR envoyée à une CA | La CSR contient la **clé publique**, pas la clé privée ; la CA ne reçoit jamais `server.key`. |
| `-validity 360` lors de `-genkeypair` | Détermine le certificat autosigné initial ; le certificat définitif signé par la CA a **sa propre** validité. |
| `openssl enc -aes-256-cbc` comme solution universelle | Démonstration de chiffrement, **pas** protection complète contre modifications non détectées ni solution moderne de gestion de secrets. |
| Redémarrer Apache à chaque changement de certificat | Préférer un contrôle de configuration suivi d'un **graceful reload** quand approprié. |
| « Le certificat valide prouve que le site est sûr » | Il atteste une liaison identité/clé et passe des contrôles cryptographiques ; il **ne garantit pas** que le contenu du site est honnête ou sans malware. |

### 18.1 Pourquoi la formation propose parfois d'importer Root et Intermediate dans le même Keystore ?

Pour le **laboratoire**, cela permet à `keytool` de construire la chaîne du certificat serveur lorsqu'il reçoit le certificat signé. Ce n'est pas une obligation générale d'avoir un « Truststore unique » côté serveur. Dans une vraie architecture :

- **Identity Keystore** : clé privée + certificat serveur + chaîne utile.
- **Truststore client** : CA qui signent les services appelés.
- Le certificat intermédiaire est généralement **envoyé par le serveur** au client.
- Le certificat racine doit déjà être **approuvé par le client** ou ancré explicitement.

### 18.2 Quelles sections Udemy peut-on passer ?

- **Sections 1–10 :** à comprendre, surtout TLS Handshake, Identity/Trust, JKS/PKCS#12, Root/Intermediate/Server.
- **Section 11 Oracle Wallet :** facultative tant que tu n'administres pas Oracle Wallet ou la TLS d'un middleware Oracle.
- **Section 12 IBM CMS / iKeyman :** facultative tant que tu n'administres pas IBM HTTP Server, MQ, WebSphere ou des produits GSKit.
- **Section 13 OpenSSL :** prioritaire pour le **diagnostic de terrain**.
- **Section 14 Apache SSL :** prioritaire pour comprendre **Virtual Hosts, termination TLS, fullchain et permissions** ; les mêmes principes se retrouvent avec d'autres reverse proxies.

---

## 19. Cheat sheet : commandes à garder

> Les commandes sont des exemples à adapter. En production, demander l'autorisation avant tout import, suppression, renouvellement ou redémarrage.

| Objectif | Commande |
|---|---|
| Version OpenSSL | `openssl version` |
| Lire certificat | `openssl x509 -in server.crt -text -noout` |
| Dates certificat | `openssl x509 -in server.crt -noout -dates` |
| SAN certificat | `openssl x509 -in server.crt -noout -ext subjectAltName` |
| Empreinte SHA-256 | `openssl x509 -in server.crt -noout -fingerprint -sha256` |
| Générer une clé privée RSA | `openssl genpkey -algorithm RSA -pkeyopt rsa_keygen_bits:3072 -out server.key` |
| Extraire clé publique | `openssl pkey -in server.key -pubout -out server.pub` |
| Créer CSR PEM | `openssl req -new -key server.key -out server.csr` |
| Lire CSR | `openssl req -in server.csr -text -noout -verify` |
| Tester HTTPS | `openssl s_client -connect example.com:443 -servername example.com -showcerts` |
| Tester hostname et chaîne | `openssl s_client -connect example.com:443 -servername example.com -verify_hostname example.com -verify_return_error` |
| Vérifier chaîne locale | `openssl verify -CAfile root.crt -untrusted intermediate.crt server.crt` |
| Générer PKCS#12 PEM→P12 | `openssl pkcs12 -export -inkey server.key -in server.crt -out server.p12` |
| Inspecter PKCS#12 | `openssl pkcs12 -in server.p12 -info -noout` |
| Lister Keystore Java | `keytool -list -v -keystore identity.p12` |
| Lister CA Java | `keytool -list -cacerts` |
| Afficher certificat avec Keytool | `keytool -printcert -file server.crt` |
| Exporter certificat sans clé | `keytool -exportcert -rfc -alias tomcat -keystore identity.p12 -file server.crt` |
| Importer CA vérifiée | `keytool -importcert -alias ma-ca -file root.crt -keystore truststore.p12` |
| CSR Java | `keytool -certreq -alias tomcat -keystore identity.p12 -file server.csr` |
| Conversion JKS→P12 | `keytool -importkeystore -srckeystore app.jks -srcstoretype JKS -destkeystore app.p12 -deststoretype PKCS12` |
| Test configuration Apache | `sudo apachectl configtest` |
| Rechargement Apache | `sudo apachectl graceful` |
| Test configuration Nginx | `sudo nginx -t` |
| Rechargement Nginx | `sudo systemctl reload nginx` |
| Certbot test renouvellement | `sudo certbot renew --dry-run` |
| Voir certificats Kubernetes | `kubectl get certificates -A` |
| Voir Secrets TLS | `kubectl get secrets -A` |

### 19.1 Où est le certificat sur le serveur ?

```bash
# Recherche de fichiers courants, sans les afficher en clair :
sudo find /etc /opt /srv -type f \
  \( -iname '*.jks' -o -iname '*.p12' -o -iname '*.pfx' \
     -o -iname '*.crt' -o -iname '*.pem' \) \
  2>/dev/null
```

La recherche ne garantit pas que tu as trouvé le **certificat effectivement actif** : les applications peuvent utiliser un secret de cluster, un magasin Windows, un HSM ou un certificat géré par un load balancer.

---

## 20. Exercices, checklist de production et glossaire

### 20.1 Mini-parcours de travaux pratiques

**Exercice 1 — Lecture de certificat :** télécharger un certificat public *par une méthode approuvée* ou créer celui du lab, puis afficher `Subject`, `Issuer`, dates, SAN et fingerprint avec OpenSSL.

**Exercice 2 — Auto-signé :** créer `server.key` et `server-selfsigned.crt` ; expliquer pourquoi le navigateur ne le reconnaît pas spontanément.

**Exercice 3 — Java :** créer un `identity.p12`, vérifier l'alias et exporter le certificat sans la clé privée.

**Exercice 4 — Chaîne complète :** réaliser le lab de la section 11 ; expliquer pourquoi l'import serveur doit reprendre **le même alias** de clé.

**Exercice 5 — Apache :** héberger un fichier HTML sur un Virtual Host HTTPS ; analyser la différence entre le fichier `.crt` et le certificat présenté avec `s_client`.

**Exercice 6 — Erreur PKIX :** simuler un client Java ne possédant pas la Root CA ; ajouter la CA au **Truststore client** et expliquer ce qui a changé.

**Exercice 7 — DevOps :** représenter le chemin Internet → Load Balancer → Gateway → Service Kubernetes → Pods, puis identifier **où** se trouve la clé privée publique et **qui** renouvelle le certificat.

### 20.2 Checklist avant une mise en production HTTPS

- [ ] L'URL publique et les SAN exacts sont identifiés.
- [ ] Le **TLS termination point** est identifié (Tomcat, reverse proxy, Gateway ou LB).
- [ ] Le certificat correspond à la clé privée effectivement configurée.
- [ ] Le certificat et les intermédiaires constituent une chaîne valide.
- [ ] Les clients possèdent les bonnes CA de confiance ; aucune validation n'est désactivée.
- [ ] Les permissions de la clé privée sont restrictives et le compte de service peut la lire.
- [ ] Les versions TLS et les Cipher Suites sont conformes à la politique de sécurité.
- [ ] Un test `openssl s_client` avec le **bon SNI** et hostname réussit.
- [ ] La procédure de renouvellement est **automatisée et supervisée**.
- [ ] Le mécanisme de **reload** est testé sur un environnement de validation.
- [ ] Un plan de **rollback** et une sauvegarde sécurisée existent.
- [ ] Les équipes savent si le flux interne reverse proxy → application est HTTP, HTTPS ou mTLS.
- [ ] Les secrets ne sont pas stockés dans Git, les images Docker ou les logs.
- [ ] Le certificat réellement présenté aux clients est surveillé pour l'expiration.

### 20.3 Glossaire express

| Terme anglais | Définition simple |
|---|---|
| **TLS** | Protocole de protection des communications réseau |
| **HTTPS** | HTTP transporté dans TLS |
| **Handshake** | Négociation initiale des paramètres et secrets TLS |
| **Private Key** | Clé secrète du propriétaire |
| **Public Key** | Clé publique associée à une clé privée |
| **Digital Certificate** | Document X.509 liant une identité à une clé publique |
| **CA (Certificate Authority)** | Autorité qui délivre et signe des certificats |
| **Root CA** | Point d'ancrage de confiance |
| **Intermediate CA** | Autorité intermédiaire signant des certificats subordonnés |
| **Trust Chain** | Chaîne de signatures jusqu'à une racine approuvée |
| **CSR** | Demande de signature de certificat |
| **Identity Store** | Magasin de ma propre identité cryptographique |
| **Truststore** | Magasin des identités ou CA approuvées |
| **Keystore** | Mécanisme de stockage de clés et certificats |
| **JKS** | Format de Keystore historique Java |
| **PKCS#12** | Format standard de conteneur clés + certificats |
| **PEM** | Encodage textuel courant de données cryptographiques |
| **SAN** | Liste des DNS/IP permis par le certificat |
| **SNI** | Nom de serveur demandé pendant le handshake TLS |
| **Cipher Suite** | Paramètres de chiffrement négociés |
| **HMAC** | MAC calculé à partir d'une fonction de hachage et d'une clé secrète |
| **mTLS** | TLS avec authentification du client et du serveur |
| **ACME** | Protocole d'automatisation d'émission/renouvellement de certificats |
| **Certbot** | Client ACME utilisé notamment avec Let's Encrypt |
| **cert-manager** | Contrôleur Kubernetes automatisant les certificats |
| **Ingress/Gateway** | Point de routage d'entrée dans Kubernetes |
| **TLS Termination** | Composant où la connexion TLS est déchiffrée |

### 20.4 Résumé à mémoriser

```text
CERTIFICAT = clé publique + identité + signature (+ extensions)
PRIVATE KEY = secrète, jamais envoyée à la CA
CSR = demande contenant notamment la clé publique, signée par la clé privée
ROOT CA = confiance ancrée dans le client
INTERMEDIATE CA = délégation de signature
SERVER CERTIFICATE = identité du serveur
IDENTITY KEYSTORE = ma clé privée et mon certificat
TRUSTSTORE = certificats que j'ai explicitement approuvés
KEYTOOL = gestion du stockage Java (JKS, PKCS#12)
OPENSSL = inspection, tests TLS, clés, CSR, PEM et PKCS#12
CERTBOT / CERT-MANAGER = automatisation ACME et renouvellement
HTTPS = identité validée + clés de session + chiffrement authentifié
```

---

## 21. Références

### 21.1 Documents pédagogiques fournis

1. **`Keystore.txt`** — commandes JKS, PKCS#12, Root/Intermediate/Server, CSR et Oracle Wallet, tel que joint à la discussion.
2. **`OpenSSL.pdf`**, **pages 2–12** — présentation OpenSSL, comparaison formats, commandes et gestion PKCS#12 ; **page 13** — exemple Apache.
3. **`D-ApacheSSLConfiguration.txt`** — exercices Apache SSL global, Virtual Hosts et certificats CA sous Linux.
4. **Programme de formation Udemy** fourni par l'utilisateur — sections 1 à 14, du principe SSL aux configurations Apache.
5. **Captures Udemy partagées dans la discussion** — Identity/Trust, Java Keytool, JKS et PKCS#12.

### 21.2 Documentations techniques officielles vérifiées

- **Apache HTTP Server, mod_ssl** : https://httpd.apache.org/docs/2.4/mod/mod_ssl.html
- **Apache, commande `apachectl`** : https://httpd.apache.org/docs/current/programs/apachectl.html
- **OpenSSL, `pkcs12`** : https://docs.openssl.org/master/man1/openssl-pkcs12/
- **OpenSSL, `s_client`** : https://docs.openssl.org/master/man1/openssl-s_client/
- **OpenJDK, `keytool`** : https://docs.oracle.com/en/java/javase/21/docs/specs/man/keytool.html
- **Tomcat 9, SSL/TLS Configuration** : https://tomcat.apache.org/tomcat-9.0-doc/ssl-howto.html
- **cert-manager, Certificate** : https://cert-manager.io/docs/usage/certificate/
- **cert-manager, ACME HTTP-01** : https://cert-manager.io/docs/configuration/acme/http01/
- **Certbot, utilisation et renewal hooks** : https://eff-certbot.readthedocs.io/en/stable/using.html
- **Let's Encrypt, FAQ** : https://letsencrypt.org/docs/faq/
- **Let's Encrypt, évolution de durée des certificats** : https://letsencrypt.org/2026/10/07/64-day-certs.html

**Note de maintenance :** les syntaxes XML Tomcat, options ACME, politiques de certificats, Cipher Suites, méthodes de reload et configurations Kubernetes varient selon les versions. Toujours confronter un runbook de production à la documentation de **la version déployée** et aux procédures d'exploitation de l'organisation.
