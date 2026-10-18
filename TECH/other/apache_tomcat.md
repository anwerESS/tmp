# Apache Tomcat — Cours Complet pour Entretien Java / Spring Boot

---

## Table des matières

1. [Introduction à Apache Tomcat](#1-introduction-à-apache-tomcat)
2. [Architecture interne de Tomcat](#2-architecture-interne-de-tomcat)
3. [Structure des répertoires](#3-structure-des-répertoires)
4. [Fichiers de configuration clés](#4-fichiers-de-configuration-clés)
5. [Configuration du server.xml](#5-configuration-du-serverxml)
6. [Configuration du web.xml](#6-configuration-du-webxml)
7. [Déploiement d'une application](#7-déploiement-dune-application)
8. [Tomcat et Spring Boot](#8-tomcat-et-spring-boot)
9. [Gestion des threads et du pool de connexions](#9-gestion-des-threads-et-du-pool-de-connexions)
10. [HTTPS / TLS — Configuration SSL](#10-https--tls--configuration-ssl)
11. [Logging dans Tomcat](#11-logging-dans-tomcat)
12. [Monitoring et administration](#12-monitoring-et-administration)
13. [Tomcat en production — Bonnes pratiques](#13-tomcat-en-production--bonnes-pratiques)
14. [Questions / Réponses orales](#14-questions--réponses-orales)

---

## 1. Introduction à Apache Tomcat

**Apache Tomcat** est un **serveur d'applications** open source développé par la **Apache Software Foundation**. Il implémente les spécifications Java EE / Jakarta EE suivantes :

| Spécification | Description |
|--------------|-------------|
| **Servlet** | Traitement des requêtes HTTP côté serveur |
| **JSP** (JavaServer Pages) | Génération de pages HTML dynamiques |
| **WebSocket** | Communication bidirectionnelle temps réel |
| **Expression Language (EL)** | Langage d'expression dans les JSP |

> Tomcat **n'est pas** un serveur d'applications Java EE complet (pas de support EJB, JMS natif, etc.) — c'est un **Servlet Container** (aussi appelé **Web Container**). Pour Java EE complet, on utiliserait WildFly, Payara ou WebSphere.

### Versions courantes

| Version Tomcat | Servlet API | Java requis |
|---------------|-------------|-------------|
| Tomcat 9.x | Servlet 4.0 | Java 8+ |
| Tomcat 10.x | Servlet 5.0 (Jakarta EE 9) | Java 11+ |
| Tomcat 11.x | Servlet 6.0 (Jakarta EE 11) | Java 17+ |

> **Point important en entretien** : Tomcat 10+ utilise le namespace `jakarta.*` au lieu de `javax.*` — migration obligatoire depuis Spring Boot 3.x.

### Rôle de Tomcat dans l'écosystème Java

```
Navigateur / Client HTTP
        ↓
[ Apache HTTP Server ] (optionnel, reverse proxy)
        ↓  (mod_jk / mod_proxy)
[  Apache Tomcat  ]
   ├── reçoit les requêtes HTTP
   ├── instancie les Servlets
   ├── gère le cycle de vie des Servlets
   └── retourne les réponses HTTP
        ↓
[ Application Java / Spring Boot ]
```

---

## 2. Architecture interne de Tomcat

Tomcat est organisé selon une hiérarchie de composants emboîtés, définis dans le fichier `server.xml`.

```
Server
└── Service
    ├── Connector (HTTP/1.1 sur port 8080)
    ├── Connector (AJP sur port 8009)
    └── Engine (Catalina)
        └── Host (localhost)
            ├── Context (/myapp)
            │   └── Wrapper (Servlet)
            └── Context (/api)
```

### Description des composants

| Composant | Rôle |
|-----------|------|
| **Server** | Représente l'instance Tomcat globale. Point d'entrée du `server.xml`. |
| **Service** | Regroupe un ou plusieurs **Connectors** avec un **Engine**. |
| **Connector** | Point d'entrée réseau — écoute sur un port (HTTP, AJP). |
| **Engine** | Moteur de traitement des requêtes (**Catalina** par défaut). |
| **Host** | Hôte virtuel (Virtual Host) — correspond à un nom de domaine. |
| **Context** | Représente une **Web Application** déployée (une `.war`). |
| **Wrapper** | Encapsule une **Servlet** individuelle. |

### Les 3 moteurs internes de Tomcat

| Moteur | Rôle |
|--------|------|
| **Catalina** | Moteur Servlet (cœur de Tomcat) |
| **Coyote** | Implémentation du connecteur HTTP/1.1 et HTTP/2 |
| **Jasper** | Moteur JSP (compile les fichiers `.jsp` en Servlets) |

---

## 3. Structure des répertoires

Après installation, Tomcat a la structure suivante :

```
$CATALINA_HOME/
├── bin/                  ← Scripts de démarrage/arrêt
│   ├── startup.sh        ← Démarrage (Linux/Mac)
│   ├── shutdown.sh       ← Arrêt
│   ├── catalina.sh       ← Script principal
│   └── startup.bat       ← Démarrage (Windows)
│
├── conf/                 ← Fichiers de configuration
│   ├── server.xml        ← Configuration principale (ports, connecteurs, hosts)
│   ├── web.xml           ← Configuration globale des Servlets
│   ├── context.xml       ← Configuration des contextes (DataSource, sessions)
│   ├── tomcat-users.xml  ← Utilisateurs et rôles (Manager App)
│   └── catalina.policy   ← Politiques de sécurité Java
│
├── lib/                  ← JARs partagés par toutes les applications
│
├── logs/                 ← Fichiers de logs
│   ├── catalina.out      ← Log principal
│   ├── localhost.log     ← Log par Host
│   └── access_log.*      ← Log des accès HTTP
│
├── webapps/              ← Répertoire de déploiement des applications
│   ├── ROOT/             ← Application déployée sur "/"
│   ├── manager/          ← Interface d'administration Web
│   └── myapp/            ← Votre application déployée
│
├── work/                 ← Fichiers de travail (JSP compilées, sessions)
│
└── temp/                 ← Fichiers temporaires
```

### Variables d'environnement importantes

| Variable | Description |
|----------|-------------|
| `CATALINA_HOME` | Répertoire d'installation de Tomcat |
| `CATALINA_BASE` | Répertoire d'instance (peut être différent de HOME pour plusieurs instances) |
| `JAVA_HOME` | Répertoire d'installation du JDK |
| `JAVA_OPTS` | Options JVM globales (heap, GC, etc.) |
| `CATALINA_OPTS` | Options JVM spécifiques à Tomcat |

---

## 4. Fichiers de configuration clés

### Vue d'ensemble

| Fichier | Portée | Rôle principal |
|---------|--------|----------------|
| `server.xml` | Serveur global | Ports, connecteurs, hôtes virtuels |
| `web.xml` | Serveur global | Configuration par défaut des Servlets |
| `context.xml` | Application | DataSource JNDI, sessions, ressources |
| `tomcat-users.xml` | Sécurité | Comptes utilisateurs et rôles |

---

## 5. Configuration du server.xml

C'est le fichier de configuration **le plus important** de Tomcat. Il définit toute la topologie du serveur.

### Exemple complet annoté

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Server port="8005" shutdown="SHUTDOWN">

    <!-- ========== LISTENERS ========== -->
    <!-- Vérifie que le JVM APR/native est disponible -->
    <Listener className="org.apache.catalina.core.AprLifecycleListener"
              SSLEngine="on" />
    <!-- Empêche les fuites mémoire liées à JDBC -->
    <Listener className="org.apache.catalina.core.JreMemoryLeakPreventionListener"/>

    <!-- ========== SERVICE ========== -->
    <Service name="Catalina">

        <!-- ========== CONNECTOR HTTP ========== -->
        <Connector port="8080"
                   protocol="HTTP/1.1"
                   connectionTimeout="20000"
                   redirectPort="8443"
                   maxThreads="200"
                   minSpareThreads="10"
                   acceptCount="100"
                   URIEncoding="UTF-8"
                   compression="on"
                   compressableMimeType="text/html,text/xml,application/json"/>

        <!-- ========== CONNECTOR AJP ========== -->
        <!-- Utilisé avec Apache HTTP Server (mod_jk) -->
        <!-- En production, désactiver si inutilisé (CVE-2020-1938 Ghostcat) -->
        <Connector protocol="AJP/1.3"
                   address="127.0.0.1"
                   port="8009"
                   redirectPort="8443"
                   secretRequired="false"/>

        <!-- ========== ENGINE ========== -->
        <Engine name="Catalina" defaultHost="localhost">

            <!-- ========== HOST (Virtual Host) ========== -->
            <Host name="localhost"
                  appBase="webapps"
                  unpackWARs="true"
                  autoDeploy="true">

                <!-- Log des accès HTTP -->
                <Valve className="org.apache.catalina.valves.AccessLogValve"
                       directory="logs"
                       prefix="localhost_access_log"
                       suffix=".txt"
                       pattern="%h %l %u %t &quot;%r&quot; %s %b %D"/>

                <!-- Filtre par IP (exemple restriction admin) -->
                <Valve className="org.apache.catalina.valves.RemoteAddrValve"
                       allow="127\.0\.0\.1"/>

                <!-- Context explicite (optionnel si autoDeploy=true) -->
                <Context path="/myapp"
                         docBase="/opt/apps/myapp"
                         reloadable="false"/>
            </Host>

        </Engine>
    </Service>
</Server>
```

### Paramètres Connector importants

| Paramètre | Défaut | Description |
|-----------|--------|-------------|
| `port` | 8080 | Port d'écoute HTTP |
| `maxThreads` | 200 | Nombre maximum de threads de traitement |
| `minSpareThreads` | 10 | Threads minimum en attente |
| `acceptCount` | 100 | File d'attente quand tous les threads sont occupés |
| `connectionTimeout` | 20000 | Timeout de connexion en millisecondes |
| `keepAliveTimeout` | — | Timeout pour les connexions HTTP Keep-Alive |
| `URIEncoding` | UTF-8 | Encodage des URL |
| `compression` | off | Compression GZIP des réponses |
| `redirectPort` | 8443 | Port HTTPS vers lequel rediriger |

---

## 6. Configuration du web.xml

Le `conf/web.xml` est la configuration **globale** appliquée à toutes les applications web déployées. Chaque application peut ensuite surcharger avec son propre `WEB-INF/web.xml`.

### Servlets par défaut importants

```xml
<!-- DefaultServlet : sert les fichiers statiques (HTML, CSS, JS, images) -->
<servlet>
    <servlet-name>default</servlet-name>
    <servlet-class>org.apache.catalina.servlets.DefaultServlet</servlet-class>
    <init-param>
        <param-name>debug</param-name>
        <param-value>0</param-value>
    </init-param>
    <init-param>
        <param-name>listings</param-name>
        <!-- SÉCURITÉ : mettre à false en production ! -->
        <param-value>false</param-value>
    </init-param>
    <load-on-startup>1</load-on-startup>
</servlet>

<!-- JspServlet : compile et sert les fichiers JSP -->
<servlet>
    <servlet-name>jsp</servlet-name>
    <servlet-class>org.apache.jasper.servlet.JspServlet</servlet-class>
    <load-on-startup>3</load-on-startup>
</servlet>
```

### Configuration de la session

```xml
<!-- Durée de vie de la session HTTP (en minutes) -->
<session-config>
    <session-timeout>30</session-timeout>
    <cookie-config>
        <http-only>true</http-only>    <!-- Empêche l'accès JavaScript au cookie -->
        <secure>true</secure>           <!-- Cookie transmis uniquement en HTTPS -->
    </cookie-config>
    <tracking-mode>COOKIE</tracking-mode>
</session-config>
```

### Configuration de la compression MIME

```xml
<mime-mapping>
    <extension>json</extension>
    <mime-type>application/json</mime-type>
</mime-mapping>
```

---

## 7. Déploiement d'une application

### 7.1 Formats de déploiement

| Format | Description |
|--------|-------------|
| **WAR** (Web Application Archive) | Archive `.war` — format standard de déploiement |
| **Exploded WAR** | Répertoire décompressé directement dans `webapps/` |
| **Context XML** | Fichier `.xml` dans `conf/Catalina/localhost/` |

### 7.2 Méthodes de déploiement

#### Méthode 1 — Copie dans webapps/ (la plus simple)

```bash
# Copier le WAR dans le répertoire webapps
cp target/myapp.war $CATALINA_HOME/webapps/

# Tomcat détecte automatiquement et déploie (si autoDeploy=true)
# L'application sera accessible sur : http://localhost:8080/myapp
```

#### Méthode 2 — Tomcat Manager App (interface web)

```
URL : http://localhost:8080/manager/html
```

Activer le Manager en configurant `tomcat-users.xml` :

```xml
<tomcat-users>
    <role rolename="manager-gui"/>
    <role rolename="manager-script"/>
    <user username="admin"
          password="motdepasse_fort"
          roles="manager-gui,manager-script"/>
</tomcat-users>
```

#### Méthode 3 — Manager App via cURL (déploiement CI/CD)

```bash
# Déployer un WAR via l'API REST du Manager
curl -u admin:motdepasse \
     -T target/myapp.war \
     "http://localhost:8080/manager/text/deploy?path=/myapp&update=true"

# Undeploy
curl -u admin:motdepasse \
     "http://localhost:8080/manager/text/undeploy?path=/myapp"

# Redémarrer une application
curl -u admin:motdepasse \
     "http://localhost:8080/manager/text/reload?path=/myapp"
```

#### Méthode 4 — Context XML dans conf/Catalina/localhost/

```xml
<!-- Fichier : conf/Catalina/localhost/myapp.xml -->
<Context docBase="/opt/deployments/myapp-1.0.war"
         path="/myapp"
         reloadable="false"
         antiResourceLocking="false">

    <!-- DataSource JNDI -->
    <Resource name="jdbc/myDB"
              auth="Container"
              type="javax.sql.DataSource"
              driverClassName="org.postgresql.Driver"
              url="jdbc:postgresql://localhost:5432/mydb"
              username="dbuser"
              password="dbpassword"
              maxTotal="20"
              maxIdle="10"
              maxWaitMillis="10000"/>
</Context>
```

### 7.3 Structure d'un WAR

```
myapp.war
├── index.html
├── static/
│   ├── css/
│   └── js/
└── WEB-INF/
    ├── web.xml            ← Descripteur de déploiement de l'application
    ├── classes/           ← Classes Java compilées
    │   └── com/example/
    └── lib/               ← JARs de l'application
        ├── spring-web.jar
        └── ...
```

### 7.4 Démarrage et arrêt de Tomcat

```bash
# Démarrer Tomcat
$CATALINA_HOME/bin/startup.sh       # Linux/Mac
$CATALINA_HOME\bin\startup.bat      # Windows

# Arrêter proprement
$CATALINA_HOME/bin/shutdown.sh

# Démarrer avec logs en temps réel
$CATALINA_HOME/bin/catalina.sh run

# Vérifier que Tomcat tourne
curl http://localhost:8080/

# Voir les logs en temps réel
tail -f $CATALINA_HOME/logs/catalina.out
```

---

## 8. Tomcat et Spring Boot

### 8.1 Tomcat embarqué (Embedded Tomcat)

Depuis Spring Boot, **Tomcat est embarqué** directement dans le JAR exécutable. Il n'y a plus besoin d'installer Tomcat séparément.

```xml
<!-- spring-boot-starter-web inclut Tomcat par défaut -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-web</artifactId>
    <!-- Tomcat est inclus automatiquement -->
</dependency>
```

```bash
# Lancer l'application Spring Boot (Tomcat démarré automatiquement)
java -jar target/myapp.jar
```

### 8.2 Changer de serveur embarqué

```xml
<!-- Exclure Tomcat et utiliser Jetty à la place -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-web</artifactId>
    <exclusions>
        <exclusion>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-tomcat</artifactId>
        </exclusion>
    </exclusions>
</dependency>

<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-jetty</artifactId>
</dependency>
```

### 8.3 Configuration Tomcat via application.properties

```properties
# Port du serveur
server.port=8080

# Contexte de l'application
server.servlet.context-path=/api

# Encoding
server.servlet.encoding.charset=UTF-8
server.servlet.encoding.force=true

# Timeout de connexion
server.connection-timeout=20s

# Compression GZIP
server.compression.enabled=true
server.compression.mime-types=application/json,application/xml,text/html
server.compression.min-response-size=1024

# Session
server.servlet.session.timeout=30m
server.servlet.session.cookie.http-only=true
server.servlet.session.cookie.secure=true

# Tomcat spécifique — Thread pool
server.tomcat.threads.max=200
server.tomcat.threads.min-spare=10
server.tomcat.accept-count=100
server.tomcat.max-connections=8192

# Logs d'accès Tomcat
server.tomcat.accesslog.enabled=true
server.tomcat.accesslog.directory=logs
server.tomcat.accesslog.pattern=%h %l %u %t "%r" %s %b %D

# Max taille des requêtes
server.tomcat.max-http-form-post-size=2MB
server.tomcat.max-swallow-size=2MB
```

### 8.4 Configuration programmatique du Tomcat embarqué

```java
@Configuration
public class TomcatConfig {

    @Bean
    public WebServerFactoryCustomizer<TomcatServletWebServerFactory> tomcatCustomizer() {
        return factory -> {
            // Configurer le thread pool
            factory.addConnectorCustomizers(connector -> {
                ProtocolHandler handler = connector.getProtocolHandler();
                if (handler instanceof AbstractProtocol<?> protocol) {
                    protocol.setMaxThreads(300);
                    protocol.setMinSpareThreads(20);
                    protocol.setConnectionTimeout(30000);
                }
            });

            // Activer la compression
            factory.addConnectorCustomizers(connector -> {
                connector.setProperty("compression", "on");
                connector.setProperty("compressableMimeType",
                    "application/json,text/html,text/plain");
            });
        };
    }
}
```

### 8.5 Déploiement WAR sur Tomcat externe (Spring Boot)

Pour déployer une application Spring Boot comme un **WAR** sur un Tomcat externe :

```xml
<!-- pom.xml : changer le packaging -->
<packaging>war</packaging>

<!-- Marquer Tomcat comme "provided" (fourni par le serveur) -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-tomcat</artifactId>
    <scope>provided</scope>
</dependency>
```

```java
// La classe principale doit étendre SpringBootServletInitializer
@SpringBootApplication
public class MyApplication extends SpringBootServletInitializer {

    public static void main(String[] args) {
        SpringApplication.run(MyApplication.class, args);
    }

    @Override
    protected SpringApplicationBuilder configure(SpringApplicationBuilder builder) {
        return builder.sources(MyApplication.class);
    }
}
```

```bash
# Générer le WAR
mvn clean package

# Copier dans webapps/
cp target/myapp.war $CATALINA_HOME/webapps/
```

---

## 9. Gestion des threads et du pool de connexions

### 9.1 Thread Pool de Tomcat

Tomcat utilise un **Thread Pool** pour traiter les requêtes HTTP entrantes. Chaque requête est assignée à un thread.

```
Requête HTTP entrante
       ↓
  [ Connector ]
  acceptCount = 100 (file d'attente)
       ↓
  [ Thread Pool ]
  minSpareThreads = 10  (threads toujours prêts)
  maxThreads = 200      (maximum concurrent)
       ↓
  [ Servlet / Application ]
```

### 9.2 Executor partagé (optionnel)

Pour un contrôle plus fin du Thread Pool, on peut définir un `Executor` explicite dans `server.xml` :

```xml
<Service name="Catalina">

    <!-- Executor partagé -->
    <Executor name="tomcatThreadPool"
              namePrefix="catalina-exec-"
              maxThreads="300"
              minSpareThreads="20"
              maxIdleTime="60000"
              maxQueueSize="200"/>

    <!-- Connector utilisant l'Executor -->
    <Connector port="8080"
               protocol="HTTP/1.1"
               executor="tomcatThreadPool"
               connectionTimeout="20000"/>
</Service>
```

### 9.3 DataSource JNDI — Pool de connexions BDD

Tomcat peut gérer un **pool de connexions JDBC** via **JNDI**, partageable entre toutes les applications.

```xml
<!-- Dans context.xml ou conf/Catalina/localhost/myapp.xml -->
<Resource name="jdbc/myDB"
          auth="Container"
          type="javax.sql.DataSource"
          factory="org.apache.tomcat.jdbc.pool.DataSourceFactory"
          driverClassName="org.postgresql.Driver"
          url="jdbc:postgresql://localhost:5432/mydb"
          username="dbuser"
          password="dbpassword"
          initialSize="5"
          maxActive="50"
          maxIdle="20"
          minIdle="5"
          maxWait="10000"
          testOnBorrow="true"
          validationQuery="SELECT 1"
          removeAbandoned="true"
          removeAbandonedTimeout="60"/>
```

Utilisation dans l'application :

```java
// Récupération via JNDI
Context ctx = new InitialContext();
DataSource ds = (DataSource) ctx.lookup("java:comp/env/jdbc/myDB");
Connection conn = ds.getConnection();
```

---

## 10. HTTPS / TLS — Configuration SSL

### 10.1 Générer un certificat auto-signé (développement)

```bash
# Générer un KeyStore JKS avec un certificat auto-signé
keytool -genkey \
        -alias tomcat \
        -keyalg RSA \
        -keysize 2048 \
        -keystore /opt/certs/keystore.jks \
        -validity 365 \
        -storepass changeit \
        -keypass changeit \
        -dname "CN=localhost, OU=Dev, O=MyCompany, L=Paris, ST=IDF, C=FR"
```

### 10.2 Configuration HTTPS dans server.xml

```xml
<!-- Connector HTTPS -->
<Connector port="8443"
           protocol="org.apache.coyote.http11.Http11NioProtocol"
           SSLEnabled="true"
           maxThreads="150"
           scheme="https"
           secure="true"
           clientAuth="false"
           sslProtocol="TLS">

    <SSLHostConfig protocols="TLSv1.2,TLSv1.3"
                   ciphers="TLS_AES_256_GCM_SHA384,TLS_CHACHA20_POLY1305_SHA256">
        <Certificate certificateKeystoreFile="/opt/certs/keystore.jks"
                     certificateKeystorePassword="changeit"
                     type="RSA"/>
    </SSLHostConfig>
</Connector>
```

### 10.3 Redirection HTTP → HTTPS

```xml
<!-- Dans server.xml : Connector HTTP normal -->
<Connector port="8080" redirectPort="8443" .../>

<!-- Dans web.xml ou WEB-INF/web.xml de l'app -->
<security-constraint>
    <web-resource-collection>
        <web-resource-name>Entire App</web-resource-name>
        <url-pattern>/*</url-pattern>
    </web-resource-collection>
    <user-data-constraint>
        <transport-guarantee>CONFIDENTIAL</transport-guarantee>
    </user-data-constraint>
</security-constraint>
```

### 10.4 HTTPS avec Spring Boot (Tomcat embarqué)

```properties
# application.properties
server.port=8443
server.ssl.enabled=true
server.ssl.key-store=classpath:keystore.jks
server.ssl.key-store-password=changeit
server.ssl.key-alias=tomcat
server.ssl.protocol=TLS
server.ssl.enabled-protocols=TLSv1.2,TLSv1.3
```

---

## 11. Logging dans Tomcat

### 11.1 Fichiers de logs

| Fichier | Contenu |
|---------|---------|
| `catalina.out` | Log principal — stdout/stderr de la JVM et de Tomcat |
| `catalina.YYYY-MM-DD.log` | Log de Catalina (rotation quotidienne) |
| `localhost.YYYY-MM-DD.log` | Erreurs des applications déployées |
| `localhost_access_log.txt` | Log des accès HTTP (Apache Combined Log Format) |

### 11.2 Configuration du logging (logging.properties)

```properties
# $CATALINA_HOME/conf/logging.properties

# Niveau global
handlers = 1catalina.org.apache.juli.AsyncFileHandler, \
           2localhost.org.apache.juli.AsyncFileHandler, \
           java.util.logging.ConsoleHandler

# Catalina
1catalina.org.apache.juli.AsyncFileHandler.level = FINE
1catalina.org.apache.juli.AsyncFileHandler.directory = ${catalina.base}/logs
1catalina.org.apache.juli.AsyncFileHandler.prefix = catalina.

# Console
java.util.logging.ConsoleHandler.level = FINE
java.util.logging.ConsoleHandler.formatter = org.apache.juli.OneLineFormatter
```

### 11.3 Access Log — Pattern communs

```xml
<Valve className="org.apache.catalina.valves.AccessLogValve"
       pattern="%h %l %u %t &quot;%r&quot; %s %b %D %{User-Agent}i"/>
```

| Token | Description |
|-------|-------------|
| `%h` | Adresse IP du client |
| `%l` | Identité distante (généralement `-`) |
| `%u` | Utilisateur authentifié |
| `%t` | Date et heure de la requête |
| `%r` | Ligne de requête HTTP (`GET /index.html HTTP/1.1`) |
| `%s` | Code de statut HTTP |
| `%b` | Taille de la réponse en octets |
| `%D` | Temps de traitement en millisecondes |
| `%{X}i` | Header HTTP entrant `X` |
| `%{X}o` | Header HTTP sortant `X` |

---

## 12. Monitoring et administration

### 12.1 Tomcat Manager App

L'interface web de management est disponible à `/manager/html` après configuration dans `tomcat-users.xml`.

**Fonctionnalités :**
- Lister les applications déployées et leur statut
- Déployer / undeploy / démarrer / arrêter des applications
- Voir les sessions actives
- Afficher les informations JVM et système

### 12.2 JMX (Java Management Extensions)

Tomcat expose ses métriques via **JMX**, permettant la surveillance avec des outils comme **JConsole**, **VisualVM**, ou **Prometheus + Micrometer**.

```bash
# Activer JMX remote dans CATALINA_OPTS
export CATALINA_OPTS="-Dcom.sun.management.jmxremote \
                      -Dcom.sun.management.jmxremote.port=9090 \
                      -Dcom.sun.management.jmxremote.ssl=false \
                      -Dcom.sun.management.jmxremote.authenticate=false"
```

### 12.3 Métriques clés à surveiller

| Métrique | Description | Seuil d'alerte |
|----------|-------------|----------------|
| **Threads actifs** | Nombre de threads en cours de traitement | > 80% de `maxThreads` |
| **File d'attente** | Requêtes en attente (`acceptCount`) | > 50% |
| **Heap memory** | Mémoire JVM utilisée | > 80% |
| **GC time** | Temps passé en Garbage Collection | > 5% du temps total |
| **Sessions actives** | Nombre de sessions HTTP | Selon capacité |
| **Error rate** | % de réponses 5xx | > 1% |
| **Response time** | Temps de réponse moyen | > SLA défini |

### 12.4 Monitoring avec Spring Boot Actuator

```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-actuator</artifactId>
</dependency>
```

```properties
# Exposer les métriques Tomcat
management.endpoints.web.exposure.include=health,metrics,info
management.endpoint.health.show-details=always
```

```bash
# Voir les métriques Tomcat
curl http://localhost:8080/actuator/metrics/tomcat.threads.busy
curl http://localhost:8080/actuator/metrics/tomcat.connections.current
```

---

## 13. Tomcat en production — Bonnes pratiques

### 13.1 Sécurité

```xml
<!-- 1. Désactiver l'AJP Connector si inutilisé (CVE Ghostcat) -->
<!-- Commenter ou supprimer cette ligne dans server.xml -->
<!-- <Connector protocol="AJP/1.3" port="8009" .../> -->

<!-- 2. Désactiver le listing des répertoires (web.xml) -->
<init-param>
    <param-name>listings</param-name>
    <param-value>false</param-value>
</init-param>

<!-- 3. Masquer la version de Tomcat dans les erreurs -->
```

```xml
<!-- conf/server.xml : masquer la version dans les headers HTTP -->
<Connector ... server="Apache"/>
```

```bash
# 4. Supprimer les applications par défaut inutiles
rm -rf $CATALINA_HOME/webapps/examples
rm -rf $CATALINA_HOME/webapps/docs
# Garder manager/ uniquement si nécessaire
```

### 13.2 Performance JVM

```bash
# CATALINA_OPTS — Configuration JVM recommandée pour la production
export CATALINA_OPTS="\
  -Xms512m \
  -Xmx2g \
  -XX:+UseG1GC \
  -XX:MaxGCPauseMillis=200 \
  -XX:+HeapDumpOnOutOfMemoryError \
  -XX:HeapDumpPath=/opt/dumps/ \
  -Djava.security.egd=file:/dev/./urandom \
  -Dfile.encoding=UTF-8"
```

| Option JVM | Description |
|------------|-------------|
| `-Xms` | Taille initiale de la Heap |
| `-Xmx` | Taille maximale de la Heap |
| `-XX:+UseG1GC` | Garbage Collector G1 (recommandé Java 9+) |
| `-XX:MaxGCPauseMillis` | Objectif de pause GC en ms |
| `-XX:+HeapDumpOnOutOfMemoryError` | Dump automatique en cas d'OOM |
| `-Djava.security.egd` | Source d'entropie pour SSL (évite les lenteurs) |

### 13.3 Tomcat derrière un reverse proxy (Nginx / Apache)

En production, on place généralement Tomcat derrière un **reverse proxy** (Nginx ou Apache HTTP Server) :

```
Internet → [Nginx :443 HTTPS] → [Tomcat :8080 HTTP]
```

**Avantages :**
- Gestion du TLS/SSL par Nginx
- Load balancing entre plusieurs instances Tomcat
- Mise en cache des ressources statiques
- Protection DDoS

```nginx
# Configuration Nginx
server {
    listen 443 ssl;
    server_name monapp.fr;

    ssl_certificate     /etc/ssl/certs/monapp.crt;
    ssl_certificate_key /etc/ssl/private/monapp.key;

    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

```xml
<!-- server.xml : RemoteIpValve pour que Tomcat voit la vraie IP client -->
<Valve className="org.apache.catalina.valves.RemoteIpValve"
       remoteIpHeader="X-Forwarded-For"
       protocolHeader="X-Forwarded-Proto"/>
```

### 13.4 Clustering et Sessions distribuées

Pour la haute disponibilité, Tomcat supporte le **clustering** avec réplication de sessions :

```xml
<Cluster className="org.apache.catalina.ha.tcp.SimpleTcpCluster">
    <Manager className="org.apache.catalina.ha.session.DeltaManager"
             expireSessionsOnShutdown="false"
             notifyListenersOnReplication="true"/>
    <Channel className="org.apache.catalina.tribes.group.GroupChannel">
        <Membership className="org.apache.catalina.tribes.membership.McastService"
                    address="228.0.0.4"
                    port="45564"
                    frequency="500"
                    dropTime="3000"/>
        <Receiver className="org.apache.catalina.tribes.transport.nio.NioReceiver"
                  address="auto"
                  port="4000"/>
        <Sender className="org.apache.catalina.tribes.transport.ReplicationTransmitter"/>
    </Channel>
</Cluster>
```

> En pratique moderne, on préfère des **sessions sans état** (JWT) ou un **store de sessions externalisé** (Redis + Spring Session) plutôt que le clustering Tomcat natif.

---

## 14. Questions / Réponses orales

---

### Q1 — Qu'est-ce qu'Apache Tomcat et quel est son rôle ?

> **Apache Tomcat** est un **Servlet Container** (ou Web Container) open source. Son rôle est d'héberger et d'exécuter des applications Java web en implémentant les spécifications **Servlet**, **JSP** et **WebSocket** du Jakarta EE.
>
> Il n'est pas un serveur d'applications Java EE complet — il ne supporte pas nativement les **EJB** ou **JMS**. Pour ces besoins, on utiliserait WildFly ou Payara. En revanche, Tomcat est très léger, facile à configurer et très utilisé dans l'écosystème Spring Boot, où il est embarqué directement dans le JAR exécutable.

---

### Q2 — Quelle est la différence entre Tomcat embarqué (Embedded) et Tomcat externe ?

> Avec **Tomcat embarqué** (Spring Boot), le serveur est packagé directement dans le **JAR exécutable**. On démarre l'application avec `java -jar myapp.jar` sans installer Tomcat séparément. C'est l'approche recommandée pour les microservices et les applications cloud-native.
>
> Avec **Tomcat externe**, on installe Tomcat sur le serveur, et on déploie l'application sous forme de fichier **WAR** dans le répertoire `webapps/`. C'est l'approche classique dans les environnements d'entreprise legacy où plusieurs applications partagent la même instance Tomcat.
>
> En Spring Boot, pour déployer en WAR sur un Tomcat externe, la classe principale doit étendre `SpringBootServletInitializer` et la dépendance `spring-boot-starter-tomcat` doit être en scope `provided`.

---

### Q3 — À quoi sert le fichier server.xml ?

> `server.xml` est le **fichier de configuration principal** de Tomcat. Il définit toute la topologie du serveur :
>
> - Le **Server** : l'instance Tomcat et le port d'arrêt (`shutdown`),
> - Les **Connectors** : les ports d'écoute (HTTP sur 8080, HTTPS sur 8443, AJP sur 8009),
> - L'**Engine** (Catalina) : le moteur de traitement des requêtes,
> - Les **Hosts** : les hôtes virtuels (Virtual Hosts),
> - Les **Contexts** : les applications web déployées.
>
> C'est dans ce fichier qu'on configure des paramètres critiques comme le nombre de threads (`maxThreads`), les timeouts, la compression GZIP, et le SSL.

---

### Q4 — Quelles sont les méthodes de déploiement d'une application sur Tomcat ?

> Il existe plusieurs méthodes :
>
> La plus simple est de **copier le WAR dans `webapps/`** : Tomcat détecte automatiquement le fichier et le déploie si `autoDeploy=true` dans `server.xml`. L'application est accessible sur `http://host:8080/nomDuWar`.
>
> On peut aussi utiliser le **Tomcat Manager App** — une interface web accessible sur `/manager/html` — pour déployer, undeploy, ou redémarrer des applications, ce qui est pratique pour des environnements de recette.
>
> Pour les pipelines **CI/CD**, on utilise l'**API REST du Manager** avec un simple appel `curl` en envoyant le WAR en HTTP PUT.
>
> Enfin, on peut créer un **fichier Context XML** dans `conf/Catalina/localhost/` pour pointer vers un WAR ou un répertoire en dehors du `webapps/`, utile pour les configurations avec DataSource JNDI.

---

### Q5 — Comment configurer le pool de threads de Tomcat ?

> Le pool de threads se configure via les attributs du **Connector** dans `server.xml` :
>
> - `maxThreads` : nombre maximum de threads simultanés (défaut : 200). Si toutes les requêtes prennent plus de temps, les threads sont tous occupés.
> - `minSpareThreads` : nombre de threads toujours en attente pour répondre immédiatement (défaut : 10).
> - `acceptCount` : taille de la file d'attente quand tous les threads sont occupés (défaut : 100). Au-delà, les nouvelles connexions sont refusées.
>
> Pour un contrôle plus fin, on peut définir un `Executor` partagé dans `server.xml` avec un `namePrefix` pour identifier les threads, un `maxQueueSize` et un `maxIdleTime`.
>
> Dans Spring Boot, ces paramètres se configurent dans `application.properties` avec les clés `server.tomcat.threads.max`, `server.tomcat.threads.min-spare` et `server.tomcat.accept-count`.

---

### Q6 — Comment configurer HTTPS sur Tomcat ?

> Il faut d'abord générer ou obtenir un **certificat SSL**. En développement, on utilise `keytool` pour générer un certificat auto-signé dans un **KeyStore JKS**. En production, on utilise un certificat signé par une **CA** (Let's Encrypt, DigiCert, etc.) ou un certificat d'entreprise.
>
> Ensuite, on configure un **Connector HTTPS** dans `server.xml` sur le port 8443, avec `SSLEnabled="true"` et le chemin vers le KeyStore. On précise les protocoles TLS autorisés (`TLSv1.2`, `TLSv1.3`) et les suites de chiffrement.
>
> Pour rediriger automatiquement HTTP vers HTTPS, on ajoute `CONFIDENTIAL` dans la configuration de sécurité du `web.xml`.
>
> Dans Spring Boot, c'est encore plus simple : on configure `server.ssl.*` dans `application.properties`.

---

### Q7 — Quelle est la différence entre les Connectors HTTP et AJP ?

> Le **Connector HTTP** (port 8080) traite directement les requêtes HTTP/1.1 et HTTP/2. Il est utilisé quand Tomcat est exposé directement au client ou derrière un reverse proxy HTTP comme Nginx.
>
> Le **Connector AJP** (AJP = Apache JServ Protocol, port 8009) est un protocole binaire optimisé conçu pour la communication entre un **Apache HTTP Server** et Tomcat via le module `mod_jk` ou `mod_proxy_ajp`. Il est plus performant que HTTP pour cette communication interne car il est binaire et maintient des connexions persistantes.
>
> Cependant, en 2020, une vulnérabilité critique appelée **Ghostcat** (CVE-2020-1938) a été découverte sur le Connector AJP. Il est donc **recommandé de désactiver AJP** en production si Apache HTTP Server n'est pas utilisé, ou de le restreindre à `127.0.0.1` avec un secret d'authentification.

---

### Q8 — Comment optimiser les performances de Tomcat en production ?

> Plusieurs axes d'optimisation :
>
> Au niveau **JVM** : ajuster la mémoire Heap (`-Xms`, `-Xmx`), utiliser le **G1GC** (`-XX:+UseG1GC`) pour minimiser les pauses GC, et configurer la source d'entropie SSL (`-Djava.security.egd=file:/dev/./urandom`) pour éviter les lenteurs au démarrage.
>
> Au niveau **Thread Pool** : dimensionner correctement `maxThreads` selon la capacité de traitement de l'application. Trop de threads = contention CPU ; trop peu = files d'attente. Un bon point de départ est de monitorer le ratio threads actifs / maxThreads.
>
> Au niveau **réseau** : activer la **compression GZIP** pour les réponses JSON/HTML, activer le **HTTP Keep-Alive**, et utiliser NIO (Non-Blocking I/O) comme protocole de Connector.
>
> Au niveau **sécurité/nettoyage** : supprimer les applications par défaut (`examples`, `docs`), désactiver le **listing des répertoires**, masquer la version de Tomcat dans les headers HTTP.

---

### Q9 — Comment Tomcat gère-t-il le cycle de vie d'une Servlet ?

> Le cycle de vie d'une Servlet suit trois phases gérées par le **Servlet Container** (Tomcat) :
>
> **Initialisation** (`init()`) : Tomcat instancie la Servlet une seule fois au démarrage (ou à la première requête selon `load-on-startup`). La méthode `init()` est appelée pour les initialisations (connexions BDD, cache, etc.).
>
> **Traitement des requêtes** (`service()`) : pour chaque requête, Tomcat appelle `service()` qui dispatche vers `doGet()`, `doPost()`, etc. Cette étape est **multi-threaded** — plusieurs requêtes sont traitées en parallèle par la même instance de Servlet. Il est donc impératif que les Servlets soient **thread-safe** (pas de variables d'instance mutables).
>
> **Destruction** (`destroy()`) : lors du undeploy ou de l'arrêt de Tomcat, `destroy()` est appelé pour libérer les ressources.

---

### Q10 — Que se passe-t-il quand Tomcat reçoit une requête HTTP ?

> Voici le chemin d'une requête dans Tomcat :
>
> Le **Connector** (Coyote) reçoit la connexion TCP sur le port 8080 et lit la requête HTTP. Il crée un objet `Request` et `Response`.
>
> Ensuite, le **Engine** (Catalina) détermine quel **Host** doit traiter la requête en fonction du header `Host`.
>
> Le Host sélectionne le bon **Context** (application web) en fonction du chemin de l'URL (`/myapp`).
>
> Le Context route la requête vers le bon **Wrapper** (Servlet) en consultant le `web.xml` ou les annotations `@WebServlet`.
>
> La **Servlet** traite la requête (appel de la méthode `doGet()`, `doPost()`, etc.), génère la réponse, et la retourne via le même chemin en sens inverse jusqu'au Connector qui renvoie la réponse HTTP au client.

---

### Q11 — Qu'est-ce que le Context dans Tomcat ?

> Un **Context** représente une **application web** déployée dans Tomcat. Il correspond au chemin d'accès de l'application dans l'URL (ex : `/myapp`).
>
> Chaque Context dispose de son propre **ClassLoader** isolé, ce qui permet à plusieurs applications de coexister dans la même JVM avec des versions différentes de bibliothèques.
>
> Le Context peut être configuré via un fichier **Context XML** dans `conf/Catalina/localhost/` ou directement dans `server.xml`. On y définit notamment les **DataSources JNDI**, les paramètres d'initialisation et les propriétés de rechargement (`reloadable="true/false"`).
>
> En développement, `reloadable="true"` permet à Tomcat de détecter les changements de classes et de redémarrer automatiquement le contexte. En production, on met `reloadable="false"` pour éviter la charge CPU du monitoring.

---

### Q12 — Comment gérer la sécurité de Tomcat en production ?

> Plusieurs mesures essentielles :
>
> **Désactiver le Connector AJP** s'il n'est pas utilisé (vulnérabilité Ghostcat CVE-2020-1938).
>
> **Supprimer les applications par défaut** (`examples`, `docs`, `host-manager`) qui exposent des informations sur la version de Tomcat.
>
> **Désactiver le listing des répertoires** dans `web.xml` (paramètre `listings=false` du DefaultServlet) pour éviter de révéler l'arborescence des fichiers.
>
> **Masquer la version de Tomcat** dans les headers HTTP en changeant l'attribut `server` du Connector et en configurant `ServerInfo.properties`.
>
> **Sécuriser le Manager App** avec un mot de passe fort et en le restreignant à l'IP locale avec un `RemoteAddrValve`.
>
> **Activer HTTPS** avec TLS 1.2/1.3 uniquement, en désactivant SSLv3, TLS 1.0 et 1.1.
>
> **Configurer les headers de sécurité HTTP** (X-Frame-Options, X-Content-Type-Options, Content-Security-Policy) via un Filter ou le `HttpHeaderSecurityFilter` de Tomcat.

---

*Bonne chance pour ton entretien ! 🚀*
