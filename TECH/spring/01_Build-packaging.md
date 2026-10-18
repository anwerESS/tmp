## [**..**](./00_index.md)

Build & Packaging Java

# Maven, Build & Packaging

## 🔧 Maven - L'Essentiel au Quotidien

### Commandes Maven Quotidiennes

**`mvn clean`**
- Supprime le répertoire `/target`
- Nettoie les fichiers compilés précédents
- À utiliser avant un build complet pour éviter les problèmes de cache

**`mvn install`**
- Exécute : compile → test → package → install
- Installe l'artifact dans le repository local `.m2`
- Permet aux autres projets locaux d'utiliser cette dépendance

**`mvn clean install`**
- Combinaison la plus courante
- Nettoie puis reconstruit tout le projet
- Garantit un build propre sans résidus

**`mvn clean install -U`**
- `-U` = `--update-snapshots`
- Force la mise à jour des dépendances SNAPSHOT depuis les repositories distants
- **Critique** : Résout les problèmes de dépendances obsolètes en cache

**`mvn clean install -DskipTests`**
- Ignore l'exécution des tests (mais les compile quand même)
- Gain de temps en développement
- ⚠️ Ne jamais utiliser en CI/CD !

**`mvn clean install -Dmaven.test.skip=true`**
- Ignore complètement les tests (ne les compile même pas)
- Plus rapide que `-DskipTests`

**`mvn dependency:tree`**
- Affiche l'arbre complet des dépendances
- Essentiel pour détecter les conflits de versions

**`mvn dependency:resolve`**
- Télécharge toutes les dépendances sans compiler
- Utile pour pré-charger le cache `.m2`

**`mvn spring-boot:run`**
- Lance l'application Spring Boot directement
- Développement rapide sans packaging

**`mvn package`**
- Crée le JAR/WAR dans `/target`
- N'installe pas dans `.m2`

**`mvn versions:display-dependency-updates`**
- Liste les dépendances qui ont des versions plus récentes disponibles

### Lifecycle Maven - Les 3 Cycles

#### 1. **Clean Lifecycle**
- `pre-clean` → `clean` → `post-clean`

#### 2. **Default Lifecycle** (le plus important)
```
validate → compile → test → package → verify → install → deploy
```

**Phases clés :**
- **validate** : Vérifie que le projet est correct
- **compile** : Compile le code source (`/src/main/java` → `/target/classes`)
- **test** : Exécute les tests unitaires (JUnit)
- **package** : Crée le JAR/WAR
- **verify** : Exécute les tests d'intégration
- **install** : Installe dans `.m2/repository`
- **deploy** : Déploie vers un repository distant (Nexus, Artifactory)

#### 3. **Site Lifecycle**
- `pre-site` → `site` → `post-site` → `site-deploy`
- Génère la documentation du projet

**Important** : Chaque phase exécute automatiquement toutes les phases précédentes !

### .m2 Directory - Le Repository Local

**Structure :**
```
~/.m2/
├── repository/          # Cache local des dépendances
│   └── com/example/...
└── settings.xml         # Configuration globale Maven
```

**Emplacement par défaut :**
- Linux/Mac : `~/.m2`
- Windows : `C:\Users\{username}\.m2`

**Pourquoi c'est important ?**
- Évite de télécharger les dépendances à chaque build
- Peut grossir rapidement (plusieurs GB)
- En cas de corruption : supprimer `/repository` et relancer `mvn install`

**Nettoyer le cache :**
```bash
rm -rf ~/.m2/repository
mvn dependency:resolve
```

### settings.xml - Configuration Maven

**Deux emplacements :**
1. **Global** : `$MAVEN_HOME/conf/settings.xml`
2. **User** : `~/.m2/settings.xml` (prioritaire)

**Configuration typique pour Spring Boot :**

```xml
<settings>
  <!-- Serveurs (credentials pour repositories privés) -->
  <servers>
    <server>
      <id>nexus-releases</id>
      <username>deployUser</username>
      <password>{encrypted-password}</password>
    </server>
  </servers>

  <!-- Mirrors (pour accélérer ou utiliser un proxy) -->
  <mirrors>
    <mirror>
      <id>company-nexus</id>
      <mirrorOf>central</mirrorOf>
      <url>https://nexus.company.com/repository/maven-public</url>
    </mirror>
  </mirrors>

  <!-- Profils -->
  <profiles>
    <profile>
      <id>dev</id>
      <repositories>
        <repository>
          <id>spring-snapshots</id>
          <url>https://repo.spring.io/snapshot</url>
          <snapshots><enabled>true</enabled></snapshots>
        </repository>
      </repositories>
    </profile>
  </profiles>

  <activeProfiles>
    <activeProfile>dev</activeProfile>
  </activeProfiles>

  <!-- Proxy (si nécessaire en entreprise) -->
  <proxies>
    <proxy>
      <id>company-proxy</id>
      <active>true</active>
      <protocol>http</protocol>
      <host>proxy.company.com</host>
      <port>8080</port>
    </proxy>
  </proxies>
</settings>
```

### Repositories Maven

**Types de repositories :**

1. **Maven Central** (par défaut)
   - Repository public principal
   - URL : https://repo.maven.apache.org/maven2

2. **Spring Repositories**
   - **Releases** : https://repo.spring.io/release
   - **Milestones** : https://repo.spring.io/milestone
   - **Snapshots** : https://repo.spring.io/snapshot

3. **Repositories d'entreprise** (Nexus, Artifactory)
   - Héberge les artifacts internes
   - Proxy/cache de Maven Central

**Configuration dans pom.xml :**
```xml
<repositories>
  <repository>
    <id>spring-milestones</id>
    <name>Spring Milestones</name>
    <url>https://repo.spring.io/milestone</url>
  </repository>
</repositories>

<pluginRepositories>
  <pluginRepository>
    <id>spring-milestones</id>
    <url>https://repo.spring.io/milestone</url>
  </pluginRepository>
</pluginRepositories>
```

### POM.xml - Structure pour Spring Boot

```xml
<project>
  <modelVersion>4.0.0</modelVersion>
  
  <!-- Héritage Spring Boot -->
  <parent>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-parent</artifactId>
    <version>3.2.0</version>
  </parent>

  <!-- Coordonnées du projet -->
  <groupId>com.example</groupId>
  <artifactId>my-app</artifactId>
  <version>1.0.0-SNAPSHOT</version>
  <packaging>jar</packaging>

  <!-- Properties -->
  <properties>
    <java.version>17</java.version>
    <maven.compiler.source>17</maven.compiler.source>
    <maven.compiler.target>17</maven.compiler.target>
  </properties>

  <!-- Dépendances -->
  <dependencies>
    <dependency>
      <groupId>org.springframework.boot</groupId>
      <artifactId>spring-boot-starter-web</artifactId>
    </dependency>
  </dependencies>

  <!-- Plugins -->
  <build>
    <plugins>
      <plugin>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-maven-plugin</artifactId>
      </plugin>
    </plugins>
  </build>
</project>
```

### Packaging Types

**JAR (par défaut pour Spring Boot)**
```xml
<packaging>jar</packaging>
```
- Application autonome avec Tomcat embarqué
- Exécution : `java -jar app.jar`
- **Fat JAR** : Contient toutes les dépendances

**WAR (pour déploiement sur serveur externe)**
```xml
<packaging>war</packaging>
```
- Pour Tomcat, WildFly, WebLogic externe
- Nécessite une classe `SpringBootServletInitializer`

### Versions - SNAPSHOT vs RELEASE

**SNAPSHOT** (`1.0.0-SNAPSHOT`)
- Version en développement
- Peut changer à chaque build
- Maven vérifie les mises à jour régulièrement
- Utilisé en développement actif

**RELEASE** (`1.0.0`, `1.0.1`)
- Version figée et immuable
- Une fois publiée, ne change jamais
- Utilisé en production

## 🏗️ Gradle (Alternative à Maven)

**build.gradle (Groovy DSL) :**
```groovy
plugins {
    id 'java'
    id 'org.springframework.boot' version '3.2.0'
}

dependencies {
    implementation 'org.springframework.boot:spring-boot-starter-web'
    testImplementation 'org.springframework.boot:spring-boot-starter-test'
}
```

**Avantages vs Maven :**
- Build incrémental (plus rapide)
- Syntaxe plus concise
- Cache de build distribué
- Meilleure gestion du multi-module

**Commandes équivalentes :**
- `gradle clean build` = `mvn clean install`
- `gradle bootRun` = `mvn spring-boot:run`
- `gradle dependencies` = `mvn dependency:tree`

## ⚡ GraalVM - Compilation Native

**Pour Spring Boot :**

```xml
<plugin>
  <groupId>org.graalvm.buildtools</groupId>
  <artifactId>native-maven-plugin</artifactId>
</plugin>
```

**Build native :**
```bash
mvn -Pnative native:compile
```

**Avantages :**
- Démarrage : 50ms vs 3-5s (JVM classique)
- Mémoire : 50MB vs 300MB+
- Parfait pour microservices et serverless

**Limitations :**
- Temps de compilation long (5-10 min)
- Réflexion Java limitée (nécessite hints)
- Certaines bibliothèques incompatibles

## 🐳 Build d'Images Docker - 3 Approches

### 1. Dockerfile (Traditionnel)

**Multi-stage optimal pour Spring Boot :**
```dockerfile
# Stage 1: Build
FROM maven:3.9-eclipse-temurin-17 AS build
WORKDIR /app
COPY pom.xml .
RUN mvn dependency:go-offline
COPY src ./src
RUN mvn package -DskipTests

# Stage 2: Runtime
FROM eclipse-temurin:17-jre-alpine
WORKDIR /app
COPY --from=build /app/target/*.jar app.jar
EXPOSE 8080
ENTRYPOINT ["java", "-jar", "app.jar"]
```

**Build :**
```bash
docker build -t myapp:latest .
```

**Avantages :** Contrôle total, flexible
**Inconvénients :** Verbeux, nécessite Docker

### 2. Buildpacks (Cloud Native)

```bash
mvn spring-boot:build-image
```

**Ou avec configuration :**
```xml
<plugin>
  <groupId>org.springframework.boot</groupId>
  <artifactId>spring-boot-maven-plugin</artifactId>
  <configuration>
    <image>
      <name>myapp:${project.version}</name>
    </image>
  </configuration>
</plugin>
```

**Avantages :** Zéro configuration, standards CNCF, optimisé automatiquement
**Inconvénients :** Moins de contrôle

### 3. Google Jib (Sans Docker !)

**Configuration pom.xml :**
```xml
<plugin>
  <groupId>com.google.cloud.tools</groupId>
  <artifactId>jib-maven-plugin</artifactId>
  <version>3.4.0</version>
  <configuration>
    <from>
      <image>eclipse-temurin:17-jre</image>
    </from>
    <to>
      <image>myapp</image>
      <tags>
        <tag>latest</tag>
        <tag>${project.version}</tag>
      </tags>
    </to>
    <container>
      <ports>
        <port>8080</port>
      </ports>
      <jvmFlags>
        <jvmFlag>-Xms512m</jvmFlag>
        <jvmFlag>-Xmx1024m</jvmFlag>
      </jvmFlags>
    </container>
  </configuration>
</plugin>
```

**Commandes Jib :**

```bash
# Build dans Docker local
mvn compile jib:dockerBuild

# Push direct vers registry (sans Docker)
mvn compile jib:build

# Avec configuration personnalisée
mvn compile jib:build -Dimage=gcr.io/myproject/myapp:v1.0
```

**Layering optimisé par Jib :**
```
Layer 1: Dépendances (rarement changé)
Layer 2: Spring Boot loader
Layer 3: Snapshot dependencies
Layer 4: Resources
Layer 5: Classes (change souvent)
```

**Avantages Jib :**
- ✅ Pas besoin de Docker installé
- ✅ Build reproductible (même image = même layers)
- ✅ Cache intelligent (rebuild ultra-rapide)
- ✅ Idéal pour CI/CD
- ✅ Sécurité : pas de daemon Docker à exposer

## 📊 Comparaison des 3 Approches

| Critère | Dockerfile | Buildpacks | Jib |
|---------|-----------|------------|-----|
| **Docker requis** | ✅ Oui | ✅ Oui | ❌ Non |
| **Vitesse** | Moyenne | Rapide | Très rapide |
| **Simplicité** | Complexe | Simple | Simple |
| **Contrôle** | Total | Limité | Moyen |
| **CI/CD** | Complexe | Simple | Excellent |
| **Layering** | Manuel | Auto | Optimisé |

**Recommandation pour Spring Boot :**
- **Développement** : Jib (`mvn jib:dockerBuild`)
- **Production simple** : Buildpacks (`mvn spring-boot:build-image`)
- **Besoins spécifiques** : Dockerfile (multi-stage)

# Questions & Réponses - Build & Packaging Java

## Maven & Gradle

**Q1 : Quelle est la différence principale entre Maven et Gradle ?**
**R :** Maven utilise XML (pom.xml) avec une structure rigide et conventionnelle, tandis que Gradle utilise un DSL Groovy/Kotlin offrant plus de flexibilité. Gradle est généralement plus rapide grâce au build incrémental et au cache de build.

**Q2 : Qu'est-ce que le cycle de vie Maven ?**
**R :** Maven a 3 cycles : clean, default (compile, test, package, install, deploy) et site. Chaque phase exécute automatiquement les phases précédentes.

**Q3 : Pourquoi choisir Gradle plutôt que Maven ?**
**R :** Gradle offre de meilleures performances (build incrémental), plus de flexibilité pour des builds complexes, et une syntaxe plus concise. Maven est préférable pour des projets simples avec des conventions standards.

## GraalVM

**Q4 : Qu'est-ce que GraalVM et pourquoi l'utiliser ?**
**R :** GraalVM est une JVM permettant la compilation native (AOT). Avantages : démarrage ultra-rapide (millisecondes vs secondes), empreinte mémoire réduite, parfait pour les microservices, serverless et conteneurs.

**Q5 : Quelle est la différence entre compilation JIT et AOT ?**
**R :** JIT (Just-In-Time) compile le bytecode en code natif à l'exécution (JVM classique). AOT (Ahead-Of-Time) compile en natif avant l'exécution, produisant un exécutable autonome (GraalVM native-image).

**Q6 : Quels sont les inconvénients de GraalVM native ?**
**R :** Temps de compilation plus long, limitations sur la réflexion Java (nécessite configuration), taille d'exécutable parfois plus grande, et certaines bibliothèques peuvent ne pas être compatibles.

## Approches de Build d'Images Docker

**Q7 : Expliquez les 3 approches de build d'images Docker**
**R :**
- **Dockerfile** : Approche traditionnelle, contrôle total, nécessite Docker daemon
- **Buildpacks** : Détection automatique, standards CNCF, pas de Dockerfile nécessaire
- **Jib** : Build sans Docker, optimisé pour Java, layering automatique des dépendances

**Q8 : Quel est l'avantage principal de Jib ?**
**R :** Jib ne nécessite pas Docker installé. Il construit l'image directement depuis Maven/Gradle, ce qui simplifie énormément les pipelines CI/CD (pas besoin de Docker-in-Docker).

**Q9 : Comment Jib optimise-t-il les images ?**
**R :** Jib sépare intelligemment les couches : dépendances (changent rarement), ressources, classes, et configuration. Lors d'un rebuild, seules les couches modifiées sont reconstruites, accélérant drastiquement le processus.

**Q10 : Différence entre `jib:build` et `jib:dockerBuild` ?**
**R :**
- `jib:dockerBuild` : Construit l'image dans le daemon Docker local
- `jib:build` : Pousse directement l'image vers un registry distant (Docker Hub, GCR, etc.) sans passer par Docker local

**Q11 : Quand utiliser Dockerfile vs Jib ?**
**R :** Utilisez Dockerfile pour des besoins très spécifiques (installations système, multi-stage complexe). Utilisez Jib pour des applications Java standards - c'est plus simple, plus rapide et ne nécessite pas Docker.

**Q12 : Qu'est-ce que Buildpacks et quand l'utiliser ?**
**R :** Buildpacks détecte automatiquement le type d'application et crée une image optimisée sans configuration. Idéal pour des déploiements standardisés sur des plateformes cloud (Heroku, Cloud Foundry, Kubernetes).

## Questions Pratiques

**Q13 : Commande complète Maven pour compiler et créer une image Docker avec Jib ?**
**R :** `mvn compile jib:dockerBuild` ou directement `mvn package jib:dockerBuild`

**Q14 : Comment configurer Jib dans pom.xml ?**
**R :**
```xml
<plugin>
  <groupId>com.google.cloud.tools</groupId>
  <artifactId>jib-maven-plugin</artifactId>
  <version>3.4.0</version>
  <configuration>
    <to>
      <image>myapp:latest</image>
    </to>
  </configuration>
</plugin>
```

**Q15 : Avantages de GraalVM pour les conteneurs ?**
**R :** Images finales beaucoup plus petites (10-50 MB vs 200+ MB), démarrage instantané (important pour l'auto-scaling), consommation mémoire réduite = plus de conteneurs par nœud = coûts réduits.