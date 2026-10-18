# Jenkins CI/CD — Guide Complet pour Développeurs Java/Spring avec Maven

> **Niveau** : Intermédiaire à Avancé  
> **Stack** : Java 17+, Spring Boot, Maven, Docker, SonarQube, Kubernetes  
> **Durée estimée** : 4–6h de lecture et mise en pratique

---

## Table des Matières

1. [Introduction à Jenkins et CI/CD](#1-introduction-à-jenkins-et-cicd)
2. [Installation et Configuration de Jenkins](#2-installation-et-configuration-de-jenkins)
3. [Concepts Fondamentaux des Pipelines](#3-concepts-fondamentaux-des-pipelines)
4. [Structure d'un Projet Spring Boot avec Maven](#4-structure-dun-projet-spring-boot-avec-maven)
5. [Stage : Clone (SCM Checkout)](#5-stage--clone-scm-checkout)
6. [Stage : Build avec Maven](#6-stage--build-avec-maven)
7. [Stage : Tests (Unit & Integration)](#7-stage--tests-unit--integration)
8. [Stage : Analyse SonarQube](#8-stage--analyse-sonarqube)
9. [Stage : Build de l'Image Docker](#9-stage--build-de-limage-docker)
10. [Stage : Push vers un Registry](#10-stage--push-vers-un-registry)
11. [Stage : Deployment (Kubernetes / Docker Compose)](#11-stage--deployment-kubernetes--docker-compose)
12. [Notifications et Rapports](#12-notifications-et-rapports)
13. [Pipeline Complet de Référence](#13-pipeline-complet-de-référence)
14. [Bonnes Pratiques et Patterns Avancés](#14-bonnes-pratiques-et-patterns-avancés)
15. [Troubleshooting](#15-troubleshooting)

---

## 1. Introduction à Jenkins et CI/CD

### 1.1 Qu'est-ce que la CI/CD ?

La **CI/CD** (Continuous Integration / Continuous Delivery ou Deployment) est une approche DevOps qui automatise les étapes entre l'écriture du code et sa mise en production.

| Concept | Définition |
|---|---|
| **CI** (Continuous Integration) | Intégrer et vérifier automatiquement chaque modification de code |
| **CD** (Continuous Delivery) | Préparer automatiquement une release déployable en production |
| **CD** (Continuous Deployment) | Déployer automatiquement en production sans intervention manuelle |

```
Developer → git push → Jenkins Pipeline → Tests → SonarQube → Docker Build → Deploy
```

### 1.2 Pourquoi Jenkins ?

**Jenkins** est un serveur d'automatisation open-source écrit en Java. C'est l'outil CI/CD le plus utilisé dans l'écosystème Java/Spring pour plusieurs raisons :

- **Écosystème riche** : plus de 1800 plugins disponibles
- **Pipeline as Code** : les pipelines sont versionnés dans le code source (Jenkinsfile)
- **Intégration native** avec Maven, Git, SonarQube, Docker, Kubernetes
- **Gratuit et open-source**, hébergeable on-premise ou dans le cloud
- **Déclaratif ou Scripted** : deux syntaxes de pipeline disponibles

### 1.3 Architecture Jenkins

```
┌─────────────────────────────────────────────┐
│              Jenkins Controller              │
│  - Gestion des Pipelines                    │
│  - Interface Web (port 8080)                │
│  - Scheduler                                │
└──────────────┬───────────────────────────────┘
               │ Distribue les Jobs
    ┌──────────┼───────────┐
    ▼          ▼           ▼
┌────────┐ ┌────────┐ ┌────────┐
│Agent 1 │ │Agent 2 │ │Agent 3 │
│(Maven) │ │(Docker)│ │(K8s)   │
└────────┘ └────────┘ └────────┘
```

- **Controller** : cerveau de Jenkins, orchestre les jobs
- **Agent** (ou Node) : machine qui exécute réellement les stages du pipeline
- **Executor** : slot d'exécution sur un agent (un agent peut avoir plusieurs executors)

---

## 2. Installation et Configuration de Jenkins

### 2.1 Installation via Docker (recommandée pour le développement)

```bash
# Créer un volume persistant
docker volume create jenkins_home

# Lancer Jenkins avec Docker
docker run -d \
  --name jenkins \
  -p 8080:8080 \
  -p 50000:50000 \
  -v jenkins_home:/var/jenkins_home \
  -v /var/run/docker.sock:/var/run/docker.sock \
  --restart=unless-stopped \
  jenkins/jenkins:lts-jdk17
```

> **Note** : Le montage de `/var/run/docker.sock` permet à Jenkins de piloter Docker depuis le container, nécessaire pour le build d'images.

### 2.2 Installation via Docker Compose

```yaml
# docker-compose.yml
version: '3.8'

services:
  jenkins:
    image: jenkins/jenkins:lts-jdk17
    container_name: jenkins
    ports:
      - "8080:8080"
      - "50000:50000"
    volumes:
      - jenkins_home:/var/jenkins_home
      - /var/run/docker.sock:/var/run/docker.sock
    environment:
      - JAVA_OPTS=-Djenkins.install.runSetupWizard=false
    networks:
      - cicd-network

  sonarqube:
    image: sonarqube:community
    container_name: sonarqube
    ports:
      - "9000:9000"
    environment:
      - SONAR_JDBC_URL=jdbc:postgresql://sonar-db:5432/sonar
      - SONAR_JDBC_USERNAME=sonar
      - SONAR_JDBC_PASSWORD=sonar
    depends_on:
      - sonar-db
    networks:
      - cicd-network

  sonar-db:
    image: postgres:15
    container_name: sonar-db
    environment:
      - POSTGRES_USER=sonar
      - POSTGRES_PASSWORD=sonar
      - POSTGRES_DB=sonar
    volumes:
      - sonar_db:/var/lib/postgresql/data
    networks:
      - cicd-network

volumes:
  jenkins_home:
  sonar_db:

networks:
  cicd-network:
    driver: bridge
```

```bash
docker-compose up -d
```

### 2.3 Configuration initiale

Après le démarrage, accéder à `http://localhost:8080` :

```bash
# Récupérer le mot de passe initial
docker exec jenkins cat /var/jenkins_home/secrets/initialAdminPassword
```

**Plugins indispensables à installer :**

```
# Pipeline
- Pipeline
- Pipeline: Declarative
- Pipeline: Stage View

# Source Control
- Git
- GitHub / GitLab (selon votre SCM)
- Credentials Binding

# Build
- Maven Integration
- JDK Tool

# Qualité
- SonarQube Scanner

# Docker
- Docker Pipeline
- Docker Commons

# Notifications
- Email Extension
- Slack Notification (optionnel)

# Utilitaires
- Blue Ocean (interface moderne)
- Workspace Cleanup
- Timestamper
- AnsiColor
```

### 2.4 Configuration des Tools dans Jenkins

Aller dans **Manage Jenkins → Tools** :

**JDK :**
```
Name: JDK-17
Install automatically: ✓
Version: OpenJDK 17
```

**Maven :**
```
Name: Maven-3.9
Install automatically: ✓
Version: 3.9.6
```

**SonarQube Scanner :**
```
Name: SonarScanner
Install automatically: ✓
Version: latest
```

### 2.5 Configuration des Credentials

Aller dans **Manage Jenkins → Credentials → System → Global credentials** :

| ID | Type | Usage |
|---|---|---|
| `git-credentials` | Username/Password ou SSH Key | Accès au repository |
| `sonar-token` | Secret text | Token SonarQube |
| `docker-hub-credentials` | Username/Password | Push vers Docker Hub |
| `k8s-config` | Secret file | kubeconfig Kubernetes |

---

## 3. Concepts Fondamentaux des Pipelines

### 3.1 Jenkinsfile : Pipeline as Code

Un **Jenkinsfile** est un fichier texte versionné dans votre repository Git qui décrit votre pipeline CI/CD. Il doit toujours être à la **racine du projet**.

```
mon-projet/
├── src/
├── pom.xml
├── Dockerfile
└── Jenkinsfile          ← Pipeline as Code
```

### 3.2 Syntaxe Déclarative vs Scriptée

**Syntaxe Déclarative** (recommandée) :
```groovy
pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                sh 'mvn clean package'
            }
        }
    }
}
```

**Syntaxe Scriptée** (plus flexible, plus complexe) :
```groovy
node {
    stage('Build') {
        sh 'mvn clean package'
    }
}
```

> **Recommandation** : Utiliser la syntaxe **Déclarative** pour sa lisibilité, sa structure claire et son meilleur support des erreurs.

### 3.3 Structure d'un Pipeline Déclaratif

```groovy
pipeline {
    // ① Où s'exécute le pipeline
    agent { ... }

    // ② Variables d'environnement globales
    environment { ... }

    // ③ Configuration des outils
    tools { ... }

    // ④ Options du pipeline
    options { ... }

    // ⑤ Déclencheurs
    triggers { ... }

    // ⑥ Paramètres (si pipeline paramétré)
    parameters { ... }

    // ⑦ Étapes du pipeline
    stages {
        stage('Nom du Stage') {
            steps {
                // actions
            }
            post {
                // actions post-stage
            }
        }
    }

    // ⑧ Actions finales (succès, échec, always)
    post { ... }
}
```

### 3.4 La Directive `agent`

```groovy
// Sur n'importe quel agent disponible
agent any

// Aucun agent global (défini stage par stage)
agent none

// Sur un agent avec un label spécifique
agent { label 'maven-agent' }

// Dans un container Docker
agent {
    docker {
        image 'maven:3.9-eclipse-temurin-17'
        args '-v $HOME/.m2:/root/.m2'
    }
}

// Avec Kubernetes
agent {
    kubernetes {
        yaml '''
        apiVersion: v1
        kind: Pod
        spec:
          containers:
          - name: maven
            image: maven:3.9-eclipse-temurin-17
            command: ["sleep", "infinity"]
        '''
    }
}
```

### 3.5 Variables d'environnement

```groovy
environment {
    // Variables statiques
    APP_NAME = 'mon-app-spring'
    DOCKER_REGISTRY = 'registry.example.com'
    
    // Depuis les credentials Jenkins
    SONAR_TOKEN = credentials('sonar-token')
    DOCKER_CREDS = credentials('docker-hub-credentials')
    
    // Dynamiques (calculées)
    IMAGE_TAG = "${env.BUILD_NUMBER}-${env.GIT_COMMIT.take(7)}"
    FULL_IMAGE = "${DOCKER_REGISTRY}/${APP_NAME}:${IMAGE_TAG}"
}
```

**Variables built-in importantes :**

| Variable | Description |
|---|---|
| `env.BUILD_NUMBER` | Numéro de build Jenkins |
| `env.BUILD_ID` | Identifiant du build |
| `env.JOB_NAME` | Nom du job Jenkins |
| `env.BRANCH_NAME` | Nom de la branche Git (Multibranch) |
| `env.GIT_COMMIT` | SHA du commit Git |
| `env.GIT_BRANCH` | Branche Git |
| `env.WORKSPACE` | Chemin du workspace sur l'agent |

---

## 4. Structure d'un Projet Spring Boot avec Maven

### 4.1 Structure type du projet

```
mon-app-spring/
├── src/
│   ├── main/
│   │   ├── java/com/example/app/
│   │   │   ├── MonApplication.java
│   │   │   ├── controller/
│   │   │   ├── service/
│   │   │   └── repository/
│   │   └── resources/
│   │       ├── application.yml
│   │       └── application-prod.yml
│   └── test/
│       ├── java/com/example/app/
│       │   ├── unit/
│       │   └── integration/
│       └── resources/
│           └── application-test.yml
├── Dockerfile
├── docker-compose.yml
├── k8s/
│   ├── deployment.yaml
│   ├── service.yaml
│   └── ingress.yaml
├── pom.xml
├── sonar-project.properties
└── Jenkinsfile
```

### 4.2 pom.xml configuré pour CI/CD

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0
         https://maven.apache.org/xsd/maven-4.0.0.xsd">

    <modelVersion>4.0.0</modelVersion>

    <parent>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-parent</artifactId>
        <version>3.2.0</version>
    </parent>

    <groupId>com.example</groupId>
    <artifactId>mon-app-spring</artifactId>
    <version>1.0.0-SNAPSHOT</version>
    <packaging>jar</packaging>

    <properties>
        <java.version>17</java.version>
        <sonar.projectKey>mon-app-spring</sonar.projectKey>
        <sonar.organization>mon-org</sonar.organization>
        <sonar.coverage.jacoco.xmlReportPaths>
            target/site/jacoco/jacoco.xml
        </sonar.coverage.jacoco.xmlReportPaths>
    </properties>

    <dependencies>
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-web</artifactId>
        </dependency>
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-data-jpa</artifactId>
        </dependency>

        <!-- Tests -->
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-test</artifactId>
            <scope>test</scope>
        </dependency>
    </dependencies>

    <build>
        <plugins>
            <!-- Spring Boot Maven Plugin -->
            <plugin>
                <groupId>org.springframework.boot</groupId>
                <artifactId>spring-boot-maven-plugin</artifactId>
            </plugin>

            <!-- JaCoCo pour la couverture de code -->
            <plugin>
                <groupId>org.jacoco</groupId>
                <artifactId>jacoco-maven-plugin</artifactId>
                <version>0.8.11</version>
                <executions>
                    <execution>
                        <goals>
                            <goal>prepare-agent</goal>
                        </goals>
                    </execution>
                    <execution>
                        <id>report</id>
                        <phase>prepare-package</phase>
                        <goals>
                            <goal>report</goal>
                        </goals>
                    </execution>
                    <!-- Quality Gate sur la couverture -->
                    <execution>
                        <id>check</id>
                        <goals>
                            <goal>check</goal>
                        </goals>
                        <configuration>
                            <rules>
                                <rule>
                                    <element>BUNDLE</element>
                                    <limits>
                                        <limit>
                                            <counter>LINE</counter>
                                            <value>COVEREDRATIO</value>
                                            <minimum>0.80</minimum> <!-- 80% couverture minimum -->
                                        </limit>
                                    </limits>
                                </rule>
                            </rules>
                        </configuration>
                    </execution>
                </executions>
            </plugin>

            <!-- Surefire pour les tests unitaires -->
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-surefire-plugin</artifactId>
                <version>3.2.2</version>
                <configuration>
                    <includes>
                        <include>**/*Test.java</include>
                        <include>**/*Tests.java</include>
                    </includes>
                    <excludes>
                        <exclude>**/*IT.java</exclude>
                    </excludes>
                </configuration>
            </plugin>

            <!-- Failsafe pour les tests d'intégration -->
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-failsafe-plugin</artifactId>
                <version>3.2.2</version>
                <executions>
                    <execution>
                        <goals>
                            <goal>integration-test</goal>
                            <goal>verify</goal>
                        </goals>
                    </execution>
                </executions>
                <configuration>
                    <includes>
                        <include>**/*IT.java</include>
                    </includes>
                </configuration>
            </plugin>
        </plugins>
    </build>
</project>
```

---

## 5. Stage : Clone (SCM Checkout)

### 5.1 Checkout basique

```groovy
stage('Clone') {
    steps {
        // Checkout simple (Jenkins récupère l'URL depuis la config du job)
        checkout scm
    }
}
```

### 5.2 Checkout avec configuration avancée

```groovy
stage('Clone') {
    steps {
        checkout([
            $class: 'GitSCM',
            branches: [[name: "*/${env.BRANCH_NAME}"]],
            userRemoteConfigs: [[
                url: 'https://github.com/mon-org/mon-app.git',
                credentialsId: 'git-credentials'
            ]],
            extensions: [
                // Nettoyer le workspace avant le checkout
                [$class: 'CleanBeforeCheckout'],
                // Cloner seulement les N derniers commits (shallow clone)
                [$class: 'CloneOption', shallow: true, depth: 10],
                // Inclure les submodules
                [$class: 'SubmoduleOption', recursiveSubmodules: true]
            ]
        ])
    }
    post {
        success {
            echo "✅ Code cloné avec succès : commit ${env.GIT_COMMIT.take(7)}"
        }
    }
}
```

### 5.3 Récupérer des informations Git après le checkout

```groovy
stage('Clone') {
    steps {
        checkout scm
        script {
            // Récupérer l'auteur du dernier commit
            env.GIT_AUTHOR = sh(
                script: "git log -1 --pretty=format:'%an'",
                returnStdout: true
            ).trim()
            
            // Récupérer le message du commit
            env.GIT_MESSAGE = sh(
                script: "git log -1 --pretty=format:'%s'",
                returnStdout: true
            ).trim()
            
            // Tag Git (si présent)
            env.GIT_TAG = sh(
                script: "git describe --tags --exact-match 2>/dev/null || echo ''",
                returnStdout: true
            ).trim()
            
            echo "📦 Commit: ${env.GIT_COMMIT.take(7)}"
            echo "👤 Auteur: ${env.GIT_AUTHOR}"
            echo "💬 Message: ${env.GIT_MESSAGE}"
        }
    }
}
```

---

## 6. Stage : Build avec Maven

### 6.1 Build basique

```groovy
stage('Build') {
    steps {
        sh 'mvn clean package -DskipTests'
    }
    post {
        success {
            // Archiver le JAR produit
            archiveArtifacts artifacts: 'target/*.jar', fingerprint: true
        }
    }
}
```

### 6.2 Build optimisé avec cache Maven

```groovy
stage('Build') {
    steps {
        sh """
            mvn clean package \
                -DskipTests \
                -Dmaven.repo.local=${WORKSPACE}/.m2/repository \
                -B \
                --no-transfer-progress
        """
        // -B = batch mode (pas d'interactivité)
        // --no-transfer-progress = moins de logs de téléchargement
    }
}
```

### 6.3 Build avec profil Maven spécifique

```groovy
stage('Build') {
    steps {
        script {
            def mavenProfile = env.BRANCH_NAME == 'main' ? 'production' : 'staging'
            sh "mvn clean package -P${mavenProfile} -DskipTests -B"
        }
    }
}
```

### 6.4 Utilisation de l'outil Maven configuré dans Jenkins

```groovy
pipeline {
    tools {
        maven 'Maven-3.9'
        jdk   'JDK-17'
    }

    stages {
        stage('Build') {
            steps {
                // Maven est automatiquement dans le PATH grâce à 'tools'
                sh 'mvn clean package -DskipTests -B'
            }
        }
    }
}
```

### 6.5 Afficher la version et les informations du build

```groovy
stage('Build Info') {
    steps {
        sh '''
            echo "=== Versions ==="
            java -version
            mvn -version
            echo "=== Projet ==="
            mvn help:evaluate -Dexpression=project.version -q -DforceStdout
        '''
        
        script {
            // Lire la version depuis le pom.xml
            env.APP_VERSION = sh(
                script: "mvn help:evaluate -Dexpression=project.version -q -DforceStdout",
                returnStdout: true
            ).trim()
            echo "Version de l'application : ${env.APP_VERSION}"
        }
    }
}
```

---

## 7. Stage : Tests (Unit & Integration)

### 7.1 Tests unitaires

```groovy
stage('Unit Tests') {
    steps {
        sh 'mvn test -B --no-transfer-progress'
    }
    post {
        always {
            // Publier les résultats JUnit dans Jenkins
            junit testResults: 'target/surefire-reports/*.xml',
                  allowEmptyResults: false

            // Publier le rapport JaCoCo
            jacoco(
                execPattern: 'target/jacoco.exec',
                classPattern: 'target/classes',
                sourcePattern: 'src/main/java',
                exclusionPattern: '**/config/**,**/dto/**,**/entity/**',
                minimumLineCoverage: '80',
                changeBuildStatus: true
            )
        }
        failure {
            echo '❌ Des tests unitaires ont échoué !'
        }
    }
}
```

### 7.2 Tests d'intégration avec Testcontainers

```groovy
stage('Integration Tests') {
    steps {
        // Testcontainers nécessite accès à Docker
        sh '''
            mvn verify \
                -P integration-test \
                -Dfailsafe.useFile=false \
                -B
        '''
    }
    post {
        always {
            junit testResults: 'target/failsafe-reports/*.xml',
                  allowEmptyResults: true
        }
    }
}
```

**Exemple de test d'intégration avec Testcontainers :**

```java
// src/test/java/.../UserServiceIT.java
@SpringBootTest
@Testcontainers
class UserServiceIT {

    @Container
    static PostgreSQLContainer<?> postgres = new PostgreSQLContainer<>("postgres:15")
            .withDatabaseName("testdb")
            .withUsername("test")
            .withPassword("test");

    @DynamicPropertySource
    static void configureProperties(DynamicPropertyRegistry registry) {
        registry.add("spring.datasource.url", postgres::getJdbcUrl);
        registry.add("spring.datasource.username", postgres::getUsername);
        registry.add("spring.datasource.password", postgres::getPassword);
    }

    @Autowired
    private UserService userService;

    @Test
    void shouldCreateUser() {
        User user = userService.create(new CreateUserRequest("john@example.com"));
        assertThat(user.getId()).isNotNull();
    }
}
```

### 7.3 Tests en parallèle (accélération du pipeline)

```groovy
stage('Tests') {
    parallel {
        stage('Unit Tests') {
            steps {
                sh 'mvn test -B -Dsurefire.failIfNoSpecifiedTests=false'
            }
            post {
                always {
                    junit 'target/surefire-reports/*.xml'
                }
            }
        }
        stage('Integration Tests') {
            steps {
                sh 'mvn failsafe:integration-test failsafe:verify -B'
            }
            post {
                always {
                    junit 'target/failsafe-reports/*.xml'
                }
            }
        }
    }
}
```

### 7.4 Quality Gate sur les tests

```groovy
stage('Tests') {
    steps {
        sh 'mvn test -B'
    }
    post {
        always {
            script {
                def testResults = junit testResults: 'target/surefire-reports/*.xml'
                
                if (testResults.failCount > 0) {
                    error("❌ ${testResults.failCount} test(s) ont échoué sur ${testResults.totalCount}")
                }
                
                echo "✅ ${testResults.passCount}/${testResults.totalCount} tests passés"
            }
        }
    }
}
```

---

## 8. Stage : Analyse SonarQube

### 8.1 Configuration SonarQube dans Jenkins

Aller dans **Manage Jenkins → Configure System → SonarQube servers** :
```
Name: SonarQube
Server URL: http://sonarqube:9000
Server authentication token: sonar-token (credential)
```

### 8.2 Analyse basique

```groovy
stage('SonarQube Analysis') {
    steps {
        withSonarQubeEnv('SonarQube') {
            sh '''
                mvn sonar:sonar \
                    -Dsonar.projectKey=${APP_NAME} \
                    -Dsonar.projectName="${APP_NAME}" \
                    -Dsonar.projectVersion=${BUILD_NUMBER} \
                    -B
            '''
        }
    }
}
```

### 8.3 Attendre le Quality Gate

```groovy
stage('SonarQube Analysis') {
    steps {
        withSonarQubeEnv('SonarQube') {
            sh 'mvn sonar:sonar -B'
        }
    }
}

stage('Quality Gate') {
    steps {
        script {
            // Attendre le résultat de l'analyse (timeout 5 minutes)
            timeout(time: 5, unit: 'MINUTES') {
                def qualityGate = waitForQualityGate()
                
                if (qualityGate.status != 'OK') {
                    error """
                        ❌ Quality Gate échoué !
                        Statut : ${qualityGate.status}
                        
                        Vérifiez les problèmes sur SonarQube :
                        ${env.SONAR_HOST_URL}/dashboard?id=${APP_NAME}
                    """
                }
                echo "✅ Quality Gate passé avec le statut : ${qualityGate.status}"
            }
        }
    }
}
```

### 8.4 Analyse SonarQube avec paramètres avancés

```groovy
stage('SonarQube Analysis') {
    steps {
        withSonarQubeEnv('SonarQube') {
            sh """
                mvn sonar:sonar \
                    -Dsonar.projectKey=${APP_NAME} \
                    -Dsonar.projectVersion=${APP_VERSION} \
                    -Dsonar.branch.name=${env.BRANCH_NAME} \
                    -Dsonar.coverage.jacoco.xmlReportPaths=target/site/jacoco/jacoco.xml \
                    -Dsonar.java.binaries=target/classes \
                    -Dsonar.java.test.binaries=target/test-classes \
                    -Dsonar.exclusions=**/config/**,**/dto/**,**/entity/**,**/generated/** \
                    -Dsonar.coverage.exclusions=**/MonApplication.java,**/config/** \
                    -B
            """
        }
    }
}
```

### 8.5 fichier `sonar-project.properties` (alternative au pom.xml)

```properties
# sonar-project.properties
sonar.projectKey=mon-app-spring
sonar.projectName=Mon Application Spring
sonar.projectVersion=1.0

sonar.sources=src/main/java
sonar.tests=src/test/java
sonar.java.binaries=target/classes
sonar.java.test.binaries=target/test-classes

sonar.coverage.jacoco.xmlReportPaths=target/site/jacoco/jacoco.xml

# Exclusions
sonar.exclusions=**/generated/**,**/dto/**
sonar.coverage.exclusions=**/config/**,**/*Application.java

# Quality Gate
sonar.qualitygate.wait=true
```

---

## 9. Stage : Build de l'Image Docker

### 9.1 Dockerfile optimisé pour Spring Boot

```dockerfile
# Dockerfile
# ── Stage 1 : Build (optionnel si Maven dans le pipeline) ─────────────────
FROM eclipse-temurin:17-jdk AS builder
WORKDIR /app
COPY pom.xml .
# Télécharger les dépendances séparément pour profiter du cache Docker
COPY pom.xml .
RUN mvn dependency:go-offline -B
COPY src ./src
RUN mvn package -DskipTests -B

# ── Stage 2 : Runtime ──────────────────────────────────────────────────────
FROM eclipse-temurin:17-jre AS runtime

# Utilisateur non-root pour la sécurité
RUN addgroup --system appgroup && adduser --system --ingroup appgroup appuser
USER appuser

WORKDIR /app

# Copier le JAR depuis le stage builder OU depuis le pipeline
COPY --from=builder /app/target/*.jar app.jar
# Alternative si le JAR est déjà buildé par le pipeline Maven :
# COPY target/*.jar app.jar

# Healthcheck
HEALTHCHECK --interval=30s --timeout=10s --start-period=60s --retries=3 \
    CMD curl -f http://localhost:8080/actuator/health || exit 1

EXPOSE 8080

# Optimisations JVM pour les containers
ENTRYPOINT ["java", \
    "-XX:+UseContainerSupport", \
    "-XX:MaxRAMPercentage=75.0", \
    "-XX:+UseG1GC", \
    "-Djava.security.egd=file:/dev/./urandom", \
    "-jar", "app.jar"]
```

### 9.2 Build de l'image dans Jenkins

```groovy
stage('Build Docker Image') {
    steps {
        script {
            // Définir le tag de l'image
            def commitSha = env.GIT_COMMIT.take(7)
            env.IMAGE_TAG = "${env.BUILD_NUMBER}-${commitSha}"
            env.FULL_IMAGE_NAME = "${DOCKER_REGISTRY}/${APP_NAME}:${IMAGE_TAG}"
            env.LATEST_IMAGE = "${DOCKER_REGISTRY}/${APP_NAME}:latest"
            
            // Build de l'image
            sh """
                docker build \
                    --build-arg BUILD_DATE=\$(date -u +'%Y-%m-%dT%H:%M:%SZ') \
                    --build-arg VCS_REF=${env.GIT_COMMIT} \
                    --build-arg VERSION=${env.APP_VERSION} \
                    -t ${FULL_IMAGE_NAME} \
                    -t ${LATEST_IMAGE} \
                    --no-cache \
                    .
            """
            
            echo "🐳 Image construite : ${FULL_IMAGE_NAME}"
        }
    }
}
```

### 9.3 Build avec le plugin Docker Pipeline

```groovy
stage('Build Docker Image') {
    steps {
        script {
            // Utilisation du plugin Docker Pipeline
            def image = docker.build("${DOCKER_REGISTRY}/${APP_NAME}:${IMAGE_TAG}", "--no-cache .")
            
            // Stocker la référence pour utilisation dans le stage Push
            env.DOCKER_IMAGE_ID = image.id
        }
    }
}
```

### 9.4 Scan de sécurité de l'image (Trivy)

```groovy
stage('Security Scan') {
    steps {
        sh """
            # Scanner l'image avec Trivy
            docker run --rm \
                -v /var/run/docker.sock:/var/run/docker.sock \
                -v \$HOME/Library/Caches:/root/.cache/ \
                aquasec/trivy:latest image \
                --exit-code 1 \
                --severity HIGH,CRITICAL \
                --ignore-unfixed \
                ${FULL_IMAGE_NAME}
        """
    }
    post {
        failure {
            echo '⚠️ Des vulnérabilités HIGH ou CRITICAL ont été détectées !'
        }
    }
}
```

---

## 10. Stage : Push vers un Registry

### 10.1 Push vers Docker Hub

```groovy
stage('Push Docker Image') {
    steps {
        script {
            withCredentials([usernamePassword(
                credentialsId: 'docker-hub-credentials',
                usernameVariable: 'DOCKER_USER',
                passwordVariable: 'DOCKER_PASS'
            )]) {
                sh """
                    echo \$DOCKER_PASS | docker login -u \$DOCKER_USER --password-stdin
                    docker push ${FULL_IMAGE_NAME}
                    docker push ${LATEST_IMAGE}
                    docker logout
                """
            }
        }
    }
    post {
        success {
            echo "✅ Image pushée : ${FULL_IMAGE_NAME}"
        }
        cleanup {
            // Supprimer l'image locale après le push
            sh "docker rmi ${FULL_IMAGE_NAME} ${LATEST_IMAGE} || true"
        }
    }
}
```

### 10.2 Push vers un registry privé (Harbor, ECR, etc.)

```groovy
stage('Push to Registry') {
    steps {
        script {
            // AWS ECR
            sh """
                aws ecr get-login-password --region eu-west-1 | \
                    docker login --username AWS --password-stdin ${ECR_REGISTRY}
                docker push ${FULL_IMAGE_NAME}
            """
            
            // Harbor (registry privé)
            // withCredentials([usernamePassword(credentialsId: 'harbor-creds', ...)]) {
            //     sh "docker login harbor.example.com -u \$USER -p \$PASS"
            //     sh "docker push ${FULL_IMAGE_NAME}"
            // }
        }
    }
}
```

### 10.3 Utilisation du plugin Docker Pipeline pour le push

```groovy
stage('Push Docker Image') {
    steps {
        script {
            docker.withRegistry("https://${DOCKER_REGISTRY}", 'docker-hub-credentials') {
                def image = docker.image("${APP_NAME}:${IMAGE_TAG}")
                image.push()
                image.push('latest')
                
                // Push avec le tag de la branche
                image.push(env.BRANCH_NAME.replaceAll('/', '-'))
            }
        }
    }
}
```

---

## 11. Stage : Deployment (Kubernetes / Docker Compose)

### 11.1 Deployment sur Kubernetes

**Fichiers Kubernetes du projet :**

```yaml
# k8s/deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: mon-app-spring
  namespace: production
  labels:
    app: mon-app-spring
    version: "{{IMAGE_TAG}}"  # Placeholder remplacé par Jenkins
spec:
  replicas: 3
  selector:
    matchLabels:
      app: mon-app-spring
  template:
    metadata:
      labels:
        app: mon-app-spring
    spec:
      containers:
      - name: mon-app-spring
        image: registry.example.com/mon-app-spring:{{IMAGE_TAG}}
        ports:
        - containerPort: 8080
        env:
        - name: SPRING_PROFILES_ACTIVE
          value: "production"
        resources:
          requests:
            memory: "256Mi"
            cpu: "250m"
          limits:
            memory: "512Mi"
            cpu: "500m"
        readinessProbe:
          httpGet:
            path: /actuator/health/readiness
            port: 8080
          initialDelaySeconds: 30
          periodSeconds: 10
        livenessProbe:
          httpGet:
            path: /actuator/health/liveness
            port: 8080
          initialDelaySeconds: 60
          periodSeconds: 30
```

```yaml
# k8s/service.yaml
apiVersion: v1
kind: Service
metadata:
  name: mon-app-spring-svc
  namespace: production
spec:
  selector:
    app: mon-app-spring
  ports:
  - port: 80
    targetPort: 8080
  type: ClusterIP
```

**Stage Jenkins pour le déploiement K8s :**

```groovy
stage('Deploy to Kubernetes') {
    when {
        branch 'main'  // Déployer uniquement depuis main
    }
    steps {
        script {
            withCredentials([file(credentialsId: 'k8s-config', variable: 'KUBECONFIG')]) {
                // Remplacer le placeholder de l'image tag
                sh """
                    sed -i 's|{{IMAGE_TAG}}|${IMAGE_TAG}|g' k8s/deployment.yaml
                    
                    kubectl apply -f k8s/deployment.yaml
                    kubectl apply -f k8s/service.yaml
                    kubectl apply -f k8s/ingress.yaml
                    
                    # Attendre que le rollout se termine
                    kubectl rollout status deployment/mon-app-spring \
                        -n production \
                        --timeout=5m
                """
            }
        }
    }
    post {
        success {
            echo "🚀 Déploiement réussi sur Kubernetes !"
        }
        failure {
            script {
                // Rollback automatique en cas d'échec
                withCredentials([file(credentialsId: 'k8s-config', variable: 'KUBECONFIG')]) {
                    sh "kubectl rollout undo deployment/mon-app-spring -n production"
                    echo "⏪ Rollback effectué !"
                }
            }
        }
    }
}
```

### 11.2 Deployment avec Helm

```groovy
stage('Deploy with Helm') {
    when {
        anyOf {
            branch 'main'
            branch 'staging'
        }
    }
    steps {
        script {
            def namespace = env.BRANCH_NAME == 'main' ? 'production' : 'staging'
            def helmRelease = "${APP_NAME}-${namespace}"
            
            withCredentials([file(credentialsId: 'k8s-config', variable: 'KUBECONFIG')]) {
                sh """
                    helm upgrade --install ${helmRelease} ./helm/mon-app \
                        --namespace ${namespace} \
                        --create-namespace \
                        --set image.tag=${IMAGE_TAG} \
                        --set image.repository=${DOCKER_REGISTRY}/${APP_NAME} \
                        --set replicaCount=3 \
                        --set environment=${namespace} \
                        --wait \
                        --timeout 5m
                """
            }
        }
    }
}
```

### 11.3 Deployment avec Docker Compose (environnement staging)

```groovy
stage('Deploy to Staging') {
    when {
        branch 'develop'
    }
    steps {
        script {
            // Se connecter au serveur staging via SSH
            sshagent(['staging-ssh-key']) {
                sh """
                    ssh -o StrictHostKeyChecking=no deploy@staging.example.com << 'EOF'
                        cd /opt/mon-app
                        
                        # Mettre à jour la variable d'environnement de l'image
                        export IMAGE_TAG=${IMAGE_TAG}
                        
                        # Puller la nouvelle image
                        docker-compose pull app
                        
                        # Redémarrer sans interruption (zero-downtime)
                        docker-compose up -d --no-deps app
                        
                        # Vérifier que le container est sain
                        sleep 30
                        docker-compose ps
                    EOF
                """
            }
        }
    }
}
```

### 11.4 Blue-Green Deployment

```groovy
stage('Blue-Green Deploy') {
    steps {
        script {
            withCredentials([file(credentialsId: 'k8s-config', variable: 'KUBECONFIG')]) {
                // Déterminer l'environnement actif
                def currentColor = sh(
                    script: "kubectl get svc mon-app-svc -o jsonpath='{.spec.selector.color}' -n production 2>/dev/null || echo 'blue'",
                    returnStdout: true
                ).trim()
                
                def newColor = currentColor == 'blue' ? 'green' : 'blue'
                echo "Déploiement sur l'environnement : ${newColor}"
                
                // Déployer sur le nouvel environnement
                sh """
                    sed -i 's/{{COLOR}}/${newColor}/g' k8s/deployment-${newColor}.yaml
                    sed -i 's/{{IMAGE_TAG}}/${IMAGE_TAG}/g' k8s/deployment-${newColor}.yaml
                    kubectl apply -f k8s/deployment-${newColor}.yaml -n production
                    kubectl rollout status deployment/mon-app-${newColor} -n production --timeout=5m
                """
                
                // Basculer le traffic
                sh "kubectl patch svc mon-app-svc -n production -p '{\"spec\":{\"selector\":{\"color\":\"${newColor}\"}}}'"
                echo "✅ Traffic basculé vers ${newColor}"
                
                // Supprimer l'ancienne version après validation
                sh "kubectl delete deployment mon-app-${currentColor} -n production --ignore-not-found"
            }
        }
    }
}
```

---

## 12. Notifications et Rapports

### 12.1 Notifications par Email

```groovy
post {
    failure {
        emailext (
            subject: "❌ BUILD FAILED: ${env.JOB_NAME} #${env.BUILD_NUMBER}",
            body: """
                <h2>Build échoué</h2>
                <p><b>Job:</b> ${env.JOB_NAME}</p>
                <p><b>Build:</b> #${env.BUILD_NUMBER}</p>
                <p><b>Branche:</b> ${env.BRANCH_NAME}</p>
                <p><b>Commit:</b> ${env.GIT_COMMIT?.take(7)}</p>
                <p><b>Auteur:</b> ${env.GIT_AUTHOR}</p>
                <p><b>Logs:</b> <a href="${env.BUILD_URL}console">Voir les logs</a></p>
            """,
            mimeType: 'text/html',
            to: "${env.GIT_AUTHOR_EMAIL}",
            recipientProviders: [[$class: 'DevelopersRecipientProvider']]
        )
    }
    success {
        emailext (
            subject: "✅ BUILD SUCCESS: ${env.JOB_NAME} #${env.BUILD_NUMBER}",
            body: "Le build ${env.BUILD_NUMBER} sur la branche ${env.BRANCH_NAME} a réussi !",
            to: "team@example.com"
        )
    }
}
```

### 12.2 Notifications Slack

```groovy
post {
    failure {
        slackSend(
            channel: '#ci-cd',
            color: 'danger',
            message: """
                ❌ *Build échoué* : ${env.JOB_NAME} #${env.BUILD_NUMBER}
                Branche: `${env.BRANCH_NAME}` | Commit: `${env.GIT_COMMIT?.take(7)}`
                <${env.BUILD_URL}|Voir les logs>
            """
        )
    }
    success {
        slackSend(
            channel: '#ci-cd',
            color: 'good',
            message: "✅ *Build réussi* : ${env.JOB_NAME} #${env.BUILD_NUMBER} sur `${env.BRANCH_NAME}`"
        )
    }
}
```

### 12.3 Rapport de couverture HTML

```groovy
stage('Publish Reports') {
    steps {
        // Rapport JaCoCo HTML
        publishHTML([
            allowMissing: false,
            alwaysLinkToLastBuild: true,
            keepAll: true,
            reportDir: 'target/site/jacoco',
            reportFiles: 'index.html',
            reportName: 'JaCoCo Coverage Report'
        ])
        
        // Rapport Surefire
        publishHTML([
            reportDir: 'target/surefire-reports',
            reportFiles: 'index.html',
            reportName: 'Test Report'
        ])
    }
}
```

---

## 13. Pipeline Complet de Référence

Voici un **Jenkinsfile complet** pour un projet Spring Boot Java :

```groovy
// Jenkinsfile
pipeline {

    // ─────────────────────────────────────────────────────────────────────
    // AGENT & TOOLS
    // ─────────────────────────────────────────────────────────────────────
    agent any

    tools {
        maven 'Maven-3.9'
        jdk   'JDK-17'
    }

    // ─────────────────────────────────────────────────────────────────────
    // OPTIONS
    // ─────────────────────────────────────────────────────────────────────
    options {
        buildDiscarder(logRotator(numToKeepStr: '10'))
        timeout(time: 45, unit: 'MINUTES')
        timestamps()
        ansiColor('xterm')
        disableConcurrentBuilds()
    }

    // ─────────────────────────────────────────────────────────────────────
    // TRIGGERS
    // ─────────────────────────────────────────────────────────────────────
    triggers {
        // Polling SCM toutes les 5 minutes (alternative : webhook Git)
        pollSCM('H/5 * * * *')
    }

    // ─────────────────────────────────────────────────────────────────────
    // PARAMÈTRES (pour builds manuels)
    // ─────────────────────────────────────────────────────────────────────
    parameters {
        booleanParam(name: 'SKIP_TESTS',   defaultValue: false, description: 'Passer les tests')
        booleanParam(name: 'FORCE_DEPLOY', defaultValue: false, description: 'Forcer le déploiement')
        choice(name: 'TARGET_ENV',
               choices: ['staging', 'production'],
               description: 'Environnement cible')
    }

    // ─────────────────────────────────────────────────────────────────────
    // VARIABLES D'ENVIRONNEMENT
    // ─────────────────────────────────────────────────────────────────────
    environment {
        APP_NAME       = 'mon-app-spring'
        DOCKER_REGISTRY = 'registry.example.com'
        
        // Credentials
        SONAR_TOKEN    = credentials('sonar-token')
        DOCKER_CREDS   = credentials('docker-hub-credentials')
        
        // Tags dynamiques
        SHORT_COMMIT   = "${env.GIT_COMMIT?.take(7) ?: 'unknown'}"
        IMAGE_TAG      = "${env.BUILD_NUMBER}-${SHORT_COMMIT}"
        FULL_IMAGE     = "${DOCKER_REGISTRY}/${APP_NAME}:${IMAGE_TAG}"
        LATEST_IMAGE   = "${DOCKER_REGISTRY}/${APP_NAME}:latest"
    }

    // ─────────────────────────────────────────────────────────────────────
    // STAGES
    // ─────────────────────────────────────────────────────────────────────
    stages {

        // ── STAGE 1 : CLONE ──────────────────────────────────────────────
        stage('Clone') {
            steps {
                checkout scm
                script {
                    env.GIT_AUTHOR = sh(
                        script: "git log -1 --pretty=format:'%an <%ae>'",
                        returnStdout: true
                    ).trim()
                    env.GIT_MESSAGE = sh(
                        script: "git log -1 --pretty=format:'%s'",
                        returnStdout: true
                    ).trim()
                    echo "📦 Branche: ${env.BRANCH_NAME}"
                    echo "👤 Auteur: ${env.GIT_AUTHOR}"
                    echo "💬 Message: ${env.GIT_MESSAGE}"
                }
            }
        }

        // ── STAGE 2 : BUILD ──────────────────────────────────────────────
        stage('Build') {
            steps {
                sh """
                    mvn clean package \
                        -DskipTests \
                        -B \
                        --no-transfer-progress
                """
                script {
                    env.APP_VERSION = sh(
                        script: "mvn help:evaluate -Dexpression=project.version -q -DforceStdout",
                        returnStdout: true
                    ).trim()
                    echo "📋 Version : ${env.APP_VERSION}"
                }
            }
            post {
                success {
                    archiveArtifacts artifacts: 'target/*.jar', fingerprint: true
                }
            }
        }

        // ── STAGE 3 : TESTS ──────────────────────────────────────────────
        stage('Tests') {
            when {
                not { expression { params.SKIP_TESTS } }
            }
            parallel {
                stage('Unit Tests') {
                    steps {
                        sh 'mvn test -B --no-transfer-progress'
                    }
                    post {
                        always {
                            junit 'target/surefire-reports/*.xml'
                        }
                    }
                }
                stage('Integration Tests') {
                    steps {
                        sh 'mvn failsafe:integration-test failsafe:verify -B --no-transfer-progress'
                    }
                    post {
                        always {
                            junit 'target/failsafe-reports/*.xml'
                        }
                    }
                }
            }
            post {
                always {
                    jacoco(
                        execPattern: 'target/jacoco.exec',
                        classPattern: 'target/classes',
                        sourcePattern: 'src/main/java',
                        minimumLineCoverage: '80'
                    )
                }
            }
        }

        // ── STAGE 4 : SONARQUBE ──────────────────────────────────────────
        stage('SonarQube Analysis') {
            steps {
                withSonarQubeEnv('SonarQube') {
                    sh """
                        mvn sonar:sonar \
                            -Dsonar.projectKey=${APP_NAME} \
                            -Dsonar.projectVersion=${env.APP_VERSION} \
                            -B --no-transfer-progress
                    """
                }
            }
        }

        stage('Quality Gate') {
            steps {
                timeout(time: 5, unit: 'MINUTES') {
                    script {
                        def qg = waitForQualityGate()
                        if (qg.status != 'OK') {
                            error "❌ Quality Gate échoué : ${qg.status}"
                        }
                        echo "✅ Quality Gate : ${qg.status}"
                    }
                }
            }
        }

        // ── STAGE 5 : BUILD IMAGE DOCKER ─────────────────────────────────
        stage('Build Docker Image') {
            steps {
                sh """
                    docker build \
                        --build-arg BUILD_DATE=\$(date -u +'%Y-%m-%dT%H:%M:%SZ') \
                        --build-arg VCS_REF=${env.GIT_COMMIT} \
                        --build-arg VERSION=${env.APP_VERSION} \
                        -t ${FULL_IMAGE} \
                        -t ${LATEST_IMAGE} \
                        .
                """
                echo "🐳 Image : ${FULL_IMAGE}"
            }
        }

        // ── STAGE 6 : PUSH IMAGE ─────────────────────────────────────────
        stage('Push Docker Image') {
            steps {
                script {
                    docker.withRegistry("https://${DOCKER_REGISTRY}", 'docker-hub-credentials') {
                        sh "docker push ${FULL_IMAGE}"
                        sh "docker push ${LATEST_IMAGE}"
                    }
                }
            }
            post {
                cleanup {
                    sh "docker rmi ${FULL_IMAGE} ${LATEST_IMAGE} || true"
                }
            }
        }

        // ── STAGE 7 : DEPLOYMENT ─────────────────────────────────────────
        stage('Deploy to Staging') {
            when {
                anyOf {
                    branch 'develop'
                    expression { params.FORCE_DEPLOY && params.TARGET_ENV == 'staging' }
                }
            }
            steps {
                withCredentials([file(credentialsId: 'k8s-config', variable: 'KUBECONFIG')]) {
                    sh """
                        sed -i 's|{{IMAGE_TAG}}|${IMAGE_TAG}|g' k8s/staging/deployment.yaml
                        kubectl apply -f k8s/staging/
                        kubectl rollout status deployment/${APP_NAME} -n staging --timeout=5m
                    """
                }
                echo "🚀 Déployé sur Staging !"
            }
        }

        stage('Deploy to Production') {
            when {
                anyOf {
                    branch 'main'
                    expression { params.FORCE_DEPLOY && params.TARGET_ENV == 'production' }
                }
            }
            steps {
                // Validation manuelle avant la production
                input message: "Déployer la version ${IMAGE_TAG} en production ?",
                      ok: 'Déployer',
                      submitter: 'admin,lead-dev'
                
                withCredentials([file(credentialsId: 'k8s-config', variable: 'KUBECONFIG')]) {
                    sh """
                        sed -i 's|{{IMAGE_TAG}}|${IMAGE_TAG}|g' k8s/production/deployment.yaml
                        kubectl apply -f k8s/production/
                        kubectl rollout status deployment/${APP_NAME} -n production --timeout=10m
                    """
                }
                echo "🎉 Déployé en Production !"
            }
            post {
                failure {
                    withCredentials([file(credentialsId: 'k8s-config', variable: 'KUBECONFIG')]) {
                        sh "kubectl rollout undo deployment/${APP_NAME} -n production"
                        echo "⏪ Rollback production effectué !"
                    }
                }
            }
        }

    } // fin stages

    // ─────────────────────────────────────────────────────────────────────
    // POST PIPELINE
    // ─────────────────────────────────────────────────────────────────────
    post {
        always {
            cleanWs()  // Nettoyer le workspace
        }
        success {
            slackSend channel: '#ci-cd', color: 'good',
                message: "✅ *SUCCESS* ${env.JOB_NAME} #${env.BUILD_NUMBER} | `${env.BRANCH_NAME}` | <${env.BUILD_URL}|Voir>"
        }
        failure {
            slackSend channel: '#ci-cd', color: 'danger',
                message: "❌ *FAILURE* ${env.JOB_NAME} #${env.BUILD_NUMBER} | `${env.BRANCH_NAME}` | <${env.BUILD_URL}|Voir>"
            emailext subject: "Build Failed: ${env.JOB_NAME} #${env.BUILD_NUMBER}",
                     body: "Build URL: ${env.BUILD_URL}",
                     to: 'team@example.com'
        }
        unstable {
            slackSend channel: '#ci-cd', color: 'warning',
                message: "⚠️ *UNSTABLE* ${env.JOB_NAME} #${env.BUILD_NUMBER}"
        }
    }

} // fin pipeline
```

---

## 14. Bonnes Pratiques et Patterns Avancés

### 14.1 Shared Libraries Jenkins

Pour mutualiser du code entre plusieurs Jenkinsfiles, utiliser les **Shared Libraries** :

**Structure d'une Shared Library :**
```
jenkins-shared-library/
├── vars/
│   ├── mavenBuild.groovy      ← fonctions globales
│   ├── dockerBuild.groovy
│   └── k8sDeploy.groovy
├── src/
│   └── org/example/
│       └── PipelineHelper.groovy
└── resources/
    └── templates/
        └── deployment.yaml
```

**Exemple de `vars/mavenBuild.groovy` :**
```groovy
def call(Map config = [:]) {
    def skipTests = config.get('skipTests', false)
    def profile   = config.get('profile', '')
    def extraArgs = config.get('extraArgs', '')
    
    sh """
        mvn clean package \
            ${skipTests ? '-DskipTests' : ''} \
            ${profile ? "-P${profile}" : ''} \
            ${extraArgs} \
            -B --no-transfer-progress
    """
}
```

**Utilisation dans un Jenkinsfile :**
```groovy
@Library('jenkins-shared-library@main') _

pipeline {
    stages {
        stage('Build') {
            steps {
                mavenBuild(skipTests: false, profile: 'production')
            }
        }
    }
}
```

### 14.2 Multibranch Pipeline

Le **Multibranch Pipeline** détecte automatiquement toutes les branches et PR d'un repository Git et crée un pipeline pour chacune.

**Stratégie par branche (dans le Jenkinsfile) :**
```groovy
stage('Deploy') {
    when {
        anyOf {
            branch 'main'       // Branche principale → Production
            branch 'develop'    // Branche de développement → Staging
            branch pattern: 'release/*', comparator: 'GLOB'  // Releases
        }
    }
    steps {
        script {
            def targetEnv = env.BRANCH_NAME == 'main' ? 'production' : 'staging'
            echo "Déploiement sur : ${targetEnv}"
        }
    }
}
```

### 14.3 Conditions et Expressions

```groovy
// Condition sur la branche
when { branch 'main' }

// Condition sur une expression Groovy
when { expression { return env.BRANCH_NAME.startsWith('release/') } }

// Condition sur un paramètre
when { equals expected: true, actual: params.FORCE_DEPLOY }

// Conditions combinées
when {
    allOf {
        branch 'main'
        not { expression { params.SKIP_DEPLOY } }
    }
}

// Condition sur un changeset (fichiers modifiés)
when {
    changeset 'src/**'
}
```

### 14.4 Gestion des secrets avec Vault

```groovy
stage('Deploy') {
    steps {
        withVault(
            configuration: [
                vaultUrl: 'https://vault.example.com',
                vaultCredentialId: 'vault-approle'
            ],
            vaultSecrets: [
                [path: 'secret/mon-app/db',
                 secretValues: [
                     [envVar: 'DB_PASSWORD', vaultKey: 'password'],
                     [envVar: 'DB_URL',      vaultKey: 'url']
                 ]]
            ]
        ) {
            sh 'kubectl create secret generic db-secret --from-literal=password=$DB_PASSWORD'
        }
    }
}
```

### 14.5 Gestion des versions sémantiques

```groovy
stage('Versioning') {
    steps {
        script {
            // Utiliser la version du pom.xml + numéro de build
            def pomVersion = sh(
                script: "mvn help:evaluate -Dexpression=project.version -q -DforceStdout",
                returnStdout: true
            ).trim()
            
            // Supprimer le SNAPSHOT pour les builds de main
            if (env.BRANCH_NAME == 'main') {
                env.RELEASE_VERSION = pomVersion.replace('-SNAPSHOT', '')
                sh "mvn versions:set -DnewVersion=${RELEASE_VERSION} -DgenerateBackupPoms=false"
            }
            
            env.IMAGE_TAG = env.BRANCH_NAME == 'main' ? 
                RELEASE_VERSION : 
                "${pomVersion}-${env.BUILD_NUMBER}"
        }
    }
}
```

### 14.6 Pipeline avec Docker comme agent dynamique

```groovy
pipeline {
    agent none  // Pas d'agent global

    stages {
        stage('Build & Test') {
            agent {
                docker {
                    image 'maven:3.9-eclipse-temurin-17-alpine'
                    // Partager le cache Maven local
                    args '-v $HOME/.m2:/root/.m2:rw'
                }
            }
            steps {
                sh 'mvn clean verify -B'
            }
        }

        stage('Docker Build') {
            agent any  // Agent hôte pour accès à Docker
            steps {
                sh "docker build -t ${APP_NAME}:${IMAGE_TAG} ."
            }
        }
    }
}
```

---

## 15. Troubleshooting

### 15.1 Problèmes courants et solutions

**❌ Erreur : `Permission denied` sur Docker**
```bash
# Solution : Ajouter l'utilisateur jenkins au groupe docker
sudo usermod -aG docker jenkins
sudo systemctl restart jenkins

# Ou dans Docker-in-Docker
docker run -v /var/run/docker.sock:/var/run/docker.sock \
           --group-add $(stat -c '%g' /var/run/docker.sock) \
           jenkins/jenkins:lts
```

**❌ Erreur : `OutOfMemoryError` lors du build Maven**
```groovy
// Augmenter la mémoire Maven dans le Jenkinsfile
environment {
    MAVEN_OPTS = '-Xmx1g -Xms512m -XX:+UseG1GC'
}
```

**❌ Erreur : SonarQube `Quality Gate timeout`**
```groovy
// Augmenter le timeout
timeout(time: 10, unit: 'MINUTES') {
    waitForQualityGate()
}
```

**❌ Erreur : `mvn: command not found`**
```groovy
// S'assurer que l'outil Maven est configuré
tools {
    maven 'Maven-3.9'  // Doit correspondre au nom configuré dans Jenkins Tools
}
```

**❌ Build lent à cause du téléchargement des dépendances Maven**
```groovy
// Utiliser un cache Maven persistant
agent {
    docker {
        image 'maven:3.9-eclipse-temurin-17'
        args '-v /opt/jenkins/maven-cache:/root/.m2/repository'
    }
}
// Ou monter un volume persistant sur l'agent
```

### 15.2 Commandes de debug utiles

```groovy
// Lister les variables d'environnement disponibles
stage('Debug') {
    steps {
        sh 'env | sort'
        sh 'pwd && ls -la'
        sh 'java -version && mvn -version'
        sh 'docker version'
    }
}
```

### 15.3 Rejouer un stage spécifique

Depuis l'interface Jenkins :
1. Aller sur le build en question
2. Cliquer sur **"Replay"** (pour relancer le pipeline entier avec possibilité de modifier le Jenkinsfile)
3. Ou utiliser l'option **"Restart from Stage"** (Blue Ocean)

### 15.4 Archiver les logs pour le debug

```groovy
post {
    failure {
        // Archiver les logs Maven en cas d'échec
        archiveArtifacts artifacts: '**/surefire-reports/**', allowEmptyArchive: true
        archiveArtifacts artifacts: '**/failsafe-reports/**', allowEmptyArchive: true
        archiveArtifacts artifacts: 'target/*.log', allowEmptyArchive: true
    }
}
```

---

## Récapitulatif du Flow CI/CD

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         PIPELINE CI/CD                                  │
│                                                                         │
│  git push                                                               │
│     │                                                                   │
│     ▼                                                                   │
│  [CLONE] ──► git checkout + infos commit                               │
│     │                                                                   │
│     ▼                                                                   │
│  [BUILD] ──► mvn clean package -DskipTests                             │
│     │                                                                   │
│     ▼                                                                   │
│  [TESTS] ──► Unit Tests ──┐                                            │
│             Integration   ├──► JUnit Reports + JaCoCo Coverage         │
│             Tests ────────┘                                            │
│     │                                                                   │
│     ▼                                                                   │
│  [SONAR] ──► mvn sonar:sonar ──► Quality Gate (OK/FAILED)             │
│     │                                                                   │
│     ▼                                                                   │
│  [DOCKER] ──► docker build ──► Scan Trivy ──► docker push             │
│     │                                                                   │
│     ▼                                                                   │
│  [DEPLOY STAGING] (branche develop)                                    │
│     │                                                                   │
│     ▼                                                                   │
│  [APPROVAL] ──► Validation manuelle (branche main)                    │
│     │                                                                   │
│     ▼                                                                   │
│  [DEPLOY PRODUCTION] (branche main) ──► Rollback si échec             │
│     │                                                                   │
│     ▼                                                                   │
│  [NOTIFY] ──► Slack / Email                                            │
└─────────────────────────────────────────────────────────────────────────┘
```

---

*Tutoriel rédigé pour Jenkins LTS, Java 17, Spring Boot 3.x, Maven 3.9+*  
*Dernière mise à jour : 2024*
