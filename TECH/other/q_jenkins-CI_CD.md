# Questions / Réponses Orales — Jenkins CI/CD (Java / Spring / Maven)

> **Format** : Questions d'entretien ou d'examen oral, classées par thème et niveau de difficulté.  
> 🟢 Débutant · 🟡 Intermédiaire · 🔴 Avancé

---

## Table des Matières

1. [Concepts Généraux CI/CD](#1-concepts-généraux-cicd)
2. [Jenkins — Architecture & Configuration](#2-jenkins--architecture--configuration)
3. [Pipelines & Jenkinsfile](#3-pipelines--jenkinsfile)
4. [Build Maven](#4-build-maven)
5. [Tests & Couverture de Code](#5-tests--couverture-de-code)
6. [SonarQube & Qualité du Code](#6-sonarqube--qualité-du-code)
7. [Docker — Build & Push d'Images](#7-docker--build--push-dimages)
8. [Deployment — Kubernetes & Stratégies](#8-deployment--kubernetes--stratégies)
9. [Sécurité & Secrets](#9-sécurité--secrets)
10. [Bonnes Pratiques & Patterns Avancés](#10-bonnes-pratiques--patterns-avancés)
11. [Troubleshooting & Scénarios Pratiques](#11-troubleshooting--scénarios-pratiques)

---

## 1. Concepts Généraux CI/CD

---

### 🟢 Q1 — Quelle est la différence entre CI, CD (Delivery) et CD (Deployment) ?

**Réponse :**

- **CI (Continuous Integration)** : chaque développeur intègre son code fréquemment (plusieurs fois par jour) dans une branche partagée. À chaque push, un pipeline automatisé déclenche le build et les tests pour détecter les régressions au plus tôt.

- **Continuous Delivery** : le code est toujours dans un état potentiellement déployable en production. Le pipeline automatise tout jusqu'à la mise en préproduction, mais le déploiement final en production reste **déclenché manuellement** (une validation humaine est requise).

- **Continuous Deployment** : va plus loin — chaque commit validé par les tests est **automatiquement déployé en production**, sans intervention humaine.

> En résumé : CI = intégrer souvent, CD Delivery = être prêt à déployer, CD Deployment = déployer automatiquement.

---

### 🟢 Q2 — Pourquoi la CI/CD est-elle importante dans un projet Spring Boot ?

**Réponse :**

Dans un projet Spring Boot, la CI/CD apporte plusieurs bénéfices concrets :

- **Détection rapide des bugs** : les tests unitaires et d'intégration sont exécutés automatiquement à chaque push, évitant que des régressions passent inaperçues pendant des jours.
- **Feedback immédiat** au développeur : il sait en quelques minutes si son code casse quelque chose.
- **Qualité continue** : SonarQube analyse le code à chaque build, surveillant la dette technique, les code smells et la couverture de tests.
- **Réduction du risque** des déploiements : des petites releases fréquentes et automatisées sont moins risquées qu'un gros déploiement manuel trimestriel.
- **Traçabilité** : chaque artefact (JAR, image Docker) est lié à un commit Git précis.

---

### 🟡 Q3 — Qu'est-ce que le concept de "Pipeline as Code" et pourquoi est-ce important ?

**Réponse :**

Le **Pipeline as Code** signifie que le pipeline CI/CD est décrit dans un fichier texte (le `Jenkinsfile`) qui est **versionné dans le repository Git** au même titre que le code source.

**Avantages :**

- **Versioning** : on peut voir l'historique des modifications du pipeline, revenir à une version précédente, etc.
- **Revue de code** : le pipeline peut être relu via une Pull Request comme n'importe quel code.
- **Reproductibilité** : le même pipeline est utilisé dans tous les environnements, pas de configuration "magique" cachée dans l'interface Jenkins.
- **Autonomie des équipes** : chaque équipe gère son propre pipeline sans dépendre d'un administrateur Jenkins central.
- **Documentation vivante** : le Jenkinsfile documente le processus de build et de déploiement du projet.

---

### 🟡 Q4 — Quelle est la différence entre un pipeline déclaratif et un pipeline scripté dans Jenkins ?

**Réponse :**

Jenkins propose deux syntaxes pour écrire les Jenkinsfiles :

| Critère | Pipeline Déclaratif | Pipeline Scripté |
|---|---|---|
| **Syntaxe** | Structure prédéfinie (`pipeline { }`) | Groovy libre (`node { }`) |
| **Lisibilité** | Plus claire, plus structurée | Plus complexe |
| **Validation** | Validée à la lecture, erreurs tôt | Erreurs à l'exécution |
| **Flexibilité** | Limitée à la syntaxe déclarative | Totale (code Groovy pur) |
| **Recommandé** | ✅ Oui, pour la majorité des cas | Pour les cas très complexes |

**Exemple déclaratif :**
```groovy
pipeline {
    agent any
    stages {
        stage('Build') {
            steps { sh 'mvn package' }
        }
    }
}
```

**Exemple scripté :**
```groovy
node {
    stage('Build') {
        sh 'mvn package'
    }
}
```

> **Recommandation** : toujours préférer le pipeline déclaratif. On peut insérer du code Groovy dans un bloc `script { }` quand on a besoin de flexibilité.

---

## 2. Jenkins — Architecture & Configuration

---

### 🟢 Q5 — Qu'est-ce qu'un Agent (ou Node) Jenkins ? Pourquoi en utilise-t-on plusieurs ?

**Réponse :**

Un **Agent** (aussi appelé Node ou Worker) est une machine qui exécute réellement les étapes du pipeline. Le **Controller** Jenkins orchestre et distribue le travail, mais c'est l'agent qui fait le build, les tests, etc.

On utilise plusieurs agents pour :

- **Scalabilité** : exécuter plusieurs pipelines en parallèle sans que l'un bloque l'autre.
- **Spécialisation** : un agent peut avoir un label `maven`, un autre `docker`, un autre `windows`, permettant d'orienter certains stages vers les agents qui ont les bons outils.
- **Isolation** : éviter qu'un build "pollue" l'environnement d'un autre.
- **Sécurité** : les agents de production n'ont pas accès aux mêmes ressources que les agents de test.

---

### 🟡 Q6 — Comment configure-t-on les credentials dans Jenkins et pourquoi ne faut-il jamais mettre un mot de passe en clair dans un Jenkinsfile ?

**Réponse :**

Les credentials se configurent dans **Manage Jenkins → Credentials**. Jenkins propose plusieurs types : Username/Password, Secret text, SSH Private Key, Secret file, etc.

Dans le Jenkinsfile, on les utilise via `credentials()` ou `withCredentials()` :

```groovy
environment {
    DOCKER_CREDS = credentials('docker-hub-credentials')
    // Crée automatiquement DOCKER_CREDS_USR et DOCKER_CREDS_PSW
}
```

**Pourquoi ne jamais mettre un mot de passe en clair ?**

- Le Jenkinsfile est **versionné dans Git** : un mot de passe en clair serait visible par quiconque a accès au repository, et resterait dans l'historique Git même après suppression.
- Les **logs Jenkins** sont accessibles à plusieurs personnes : un secret en clair apparaîtrait dans les logs.
- C'est une faille de sécurité majeure (violation des principes OWASP).

Avec les credentials Jenkins, les valeurs sont **masquées dans les logs** (remplacées par `****`).

---

### 🟡 Q7 — Qu'est-ce qu'un Multibranch Pipeline et quel est son avantage ?

**Réponse :**

Un **Multibranch Pipeline** est un type de job Jenkins qui **scanne automatiquement toutes les branches et Pull Requests** d'un repository Git et crée un pipeline distinct pour chacune.

**Avantages :**

- **Zéro configuration manuelle** : quand un développeur crée une nouvelle branche `feature/ma-feature`, Jenkins détecte automatiquement le Jenkinsfile et crée le job.
- **Isolation** : chaque branche a son propre historique de builds.
- **Stratégies différentes par branche** : avec des conditions `when { branch 'main' }`, on peut déployer en production uniquement depuis `main`, faire du staging depuis `develop`, etc.
- **Nettoyage automatique** : quand une branche est supprimée, Jenkins peut supprimer le job correspondant.

---

## 3. Pipelines & Jenkinsfile

---

### 🟢 Q8 — À quoi sert la directive `post` dans un Jenkinsfile ? Quelles sont les conditions disponibles ?

**Réponse :**

La directive `post` définit des actions à exécuter **après** la fin d'un stage ou du pipeline entier, selon le résultat du build.

Les conditions disponibles sont :

| Condition | Déclenchement |
|---|---|
| `always` | Toujours, quelle que soit l'issue |
| `success` | Uniquement en cas de succès |
| `failure` | Uniquement en cas d'échec |
| `unstable` | Si le build est instable (ex: tests flaky) |
| `changed` | Si le statut a changé par rapport au build précédent |
| `fixed` | Si le build était en échec et redevient vert |
| `regression` | Si un build précédemment vert devient rouge |
| `cleanup` | Toujours exécuté en dernier (même après `always`) |

**Exemple typique :**
```groovy
post {
    always   { junit 'target/surefire-reports/*.xml' }
    success  { slackSend color: 'good', message: '✅ Build OK' }
    failure  { emailext to: 'team@example.com', subject: 'Build Failed' }
    cleanup  { cleanWs() }
}
```

---

### 🟡 Q9 — Comment exécuter des stages en parallèle dans Jenkins et quel est l'impact sur le pipeline ?

**Réponse :**

On utilise le bloc `parallel` à l'intérieur d'un `stage` :

```groovy
stage('Tests') {
    parallel {
        stage('Unit Tests') {
            steps { sh 'mvn test' }
        }
        stage('Integration Tests') {
            steps { sh 'mvn failsafe:integration-test' }
        }
    }
}
```

**Impact sur le pipeline :**

- **Accélération** : les deux stages s'exécutent simultanément, réduisant le temps total. Si chaque stage dure 5 minutes, en parallèle le total est ~5 min au lieu de ~10 min.
- **Nécessite plusieurs executors** ou agents disponibles, sinon Jenkins met en file d'attente.
- **Attention à l'isolation** : les stages parallèles partagent le même workspace par défaut, il faut éviter les conflits de fichiers.
- Si un stage parallèle échoue, Jenkins peut être configuré pour arrêter les autres immédiatement (`failFast: true`) ou les laisser se terminer.

```groovy
stage('Tests') {
    failFast true  // Arrêter tout si un stage échoue
    parallel { ... }
}
```

---

### 🟡 Q10 — Comment gérer les déploiements conditionnels selon la branche Git dans un Jenkinsfile ?

**Réponse :**

On utilise la directive `when` pour conditionner l'exécution d'un stage :

```groovy
// Déployer en staging uniquement depuis develop
stage('Deploy Staging') {
    when { branch 'develop' }
    steps { /* deploy staging */ }
}

// Déployer en production uniquement depuis main
stage('Deploy Production') {
    when {
        allOf {
            branch 'main'
            not { expression { params.SKIP_DEPLOY } }
        }
    }
    steps { /* deploy production */ }
}

// Déployer depuis toutes les branches de release
stage('Deploy Release') {
    when {
        branch pattern: 'release/*', comparator: 'GLOB'
    }
    steps { /* deploy release */ }
}
```

Cette approche implémente le **GitFlow** directement dans le pipeline.

---

### 🔴 Q11 — Qu'est-ce qu'une Shared Library Jenkins et dans quel cas l'utilise-t-on ?

**Réponse :**

Une **Shared Library** est un repository Git séparé contenant du code Groovy réutilisable que plusieurs Jenkinsfiles peuvent importer. Elle se configure dans **Manage Jenkins → Configure System → Global Pipeline Libraries**.

**Structure :**
```
jenkins-shared-library/
├── vars/
│   └── mavenBuild.groovy   ← fonctions globales appelables directement
└── src/
    └── org/example/
        └── Helper.groovy   ← classes Groovy
```

**Cas d'usage :**

- On a **10 microservices** Spring Boot qui ont tous le même pipeline. Sans Shared Library, on duplique le Jenkinsfile 10 fois. Avec une Shared Library, chaque Jenkinsfile appelle simplement `mavenBuild()` ou `dockerBuildAndPush()`.
- Toute modification (ex: ajouter un scan de sécurité) se fait en **un seul endroit** et s'applique à tous les projets.
- Facilite la **gouvernance** : les bonnes pratiques CI/CD sont encodées dans la librairie et imposées à tous.

```groovy
// Jenkinsfile d'un microservice
@Library('jenkins-shared-library@main') _

pipeline {
    stages {
        stage('Build')  { steps { mavenBuild() } }
        stage('Deploy') { steps { k8sDeploy(env: 'staging') } }
    }
}
```

---

## 4. Build Maven

---

### 🟢 Q12 — Que fait la commande `mvn clean package -DskipTests` et pourquoi skippe-t-on les tests dans le stage Build ?

**Réponse :**

- `clean` : supprime le répertoire `target/` pour repartir d'un état propre.
- `package` : compile le code, exécute les phases Maven jusqu'au packaging, et produit un JAR dans `target/`.
- `-DskipTests` : ignore la compilation ET l'exécution des tests.

**Pourquoi skipper les tests dans le stage Build ?**

Dans un pipeline CI/CD bien structuré, le Build et les Tests sont **deux stages séparés**. On skippe les tests dans le Build pour :

- **Séparer les responsabilités** : le stage Build ne fait que produire l'artefact, le stage Test vérifie la qualité.
- **Meilleure lisibilité** dans l'interface Jenkins : on voit clairement quel stage a échoué (Build ? Tests ?).
- **Parallélisation** possible : on peut builder puis lancer plusieurs suites de tests en parallèle.
- **Gain de temps** si on veut juste vérifier que le code compile.

> Note : `-DskipTests` skips l'exécution mais compile les tests. Pour aussi ne pas compiler : `-Dmaven.test.skip=true`.

---

### 🟡 Q13 — Comment optimiser les temps de build Maven dans un pipeline Jenkins ?

**Réponse :**

Plusieurs stratégies pour accélérer les builds Maven :

**1. Cache du repository Maven local :**
```groovy
agent {
    docker {
        image 'maven:3.9-eclipse-temurin-17'
        args '-v /opt/jenkins/m2-cache:/root/.m2/repository'
    }
}
```
Sans cache, Maven re-télécharge toutes les dépendances à chaque build.

**2. Flags Maven pour réduire les logs :**
```bash
mvn package -B --no-transfer-progress
# -B = batch mode (pas d'interactivité)
# --no-transfer-progress = supprime les barres de progression
```

**3. Build incrémental avec `-pl` et `-am` :**
```bash
# Builder seulement le module modifié et ses dépendances
mvn package -pl mon-module -am
```

**4. Parallélisation Maven :**
```bash
mvn package -T 4  # 4 threads
mvn package -T 1C # 1 thread par CPU core
```

**5. Utiliser un Nexus/Artifactory proxy** pour éviter les téléchargements depuis Maven Central.

---

### 🟡 Q14 — Quelle est la différence entre `mvn package` et `mvn verify` ?

**Réponse :**

Ces deux commandes correspondent à des phases du cycle de vie Maven :

| Phase | Commande | Ce qu'elle fait |
|---|---|---|
| `package` | `mvn package` | Compile + Tests unitaires (Surefire) + Crée le JAR |
| `verify` | `mvn verify` | Tout ce que fait `package` + **Tests d'intégration** (Failsafe) + Vérifications JaCoCo |

En pratique dans un pipeline CI/CD :

- On utilise `mvn package -DskipTests` dans le **stage Build** pour produire rapidement l'artefact.
- On utilise `mvn verify` dans le **stage Tests** pour tout vérifier (tests unitaires + intégration + couverture).

> `verify` s'assure que le package est valide selon tous les critères définis (couverture minimale, tests d'intégration OK, etc.).

---

## 5. Tests & Couverture de Code

---

### 🟢 Q15 — Quelle est la différence entre les tests unitaires et les tests d'intégration dans un projet Spring Boot ?

**Réponse :**

| Critère | Tests Unitaires | Tests d'Intégration |
|---|---|---|
| **Scope** | Une seule classe/méthode isolée | Plusieurs composants ensemble |
| **Dépendances** | Mockées (Mockito) | Réelles (base de données, API) |
| **Vitesse** | Très rapides (ms) | Plus lents (secondes) |
| **Plugin Maven** | Surefire | Failsafe |
| **Convention de nommage** | `*Test.java` | `*IT.java` |
| **Annotation Spring** | `@ExtendWith(MockitoExtension.class)` | `@SpringBootTest` |

**Exemple test unitaire :**
```java
@ExtendWith(MockitoExtension.class)
class UserServiceTest {
    @Mock UserRepository repo;
    @InjectMocks UserService service;

    @Test
    void shouldReturnUser() {
        when(repo.findById(1L)).thenReturn(Optional.of(new User("Alice")));
        assertThat(service.getById(1L).getName()).isEqualTo("Alice");
    }
}
```

**Exemple test d'intégration avec Testcontainers :**
```java
@SpringBootTest
@Testcontainers
class UserServiceIT {
    @Container
    static PostgreSQLContainer<?> postgres = new PostgreSQLContainer<>("postgres:15");
    // Utilise une vraie base de données PostgreSQL dans un container Docker
}
```

---

### 🟡 Q16 — Qu'est-ce que JaCoCo et comment l'utilise-t-on pour imposer une couverture de code minimale ?

**Réponse :**

**JaCoCo** (Java Code Coverage) est un plugin Maven qui mesure le **taux de couverture du code** par les tests : combien de lignes, branches, méthodes ont été exécutées pendant les tests.

**Configuration dans `pom.xml` :**
```xml
<plugin>
    <groupId>org.jacoco</groupId>
    <artifactId>jacoco-maven-plugin</artifactId>
    <executions>
        <execution>
            <goals><goal>prepare-agent</goal></goals>
        </execution>
        <execution>
            <id>check</id>
            <goals><goal>check</goal></goals>
            <configuration>
                <rules>
                    <rule>
                        <limits>
                            <limit>
                                <counter>LINE</counter>
                                <value>COVEREDRATIO</value>
                                <minimum>0.80</minimum> <!-- 80% minimum -->
                            </limit>
                        </limits>
                    </rule>
                </rules>
            </configuration>
        </execution>
    </executions>
</plugin>
```

Si la couverture est inférieure à 80%, `mvn verify` **échoue**, ce qui fait échouer le pipeline Jenkins. C'est un **Quality Gate au niveau Maven**.

Dans Jenkins, le plugin JaCoCo permet d'afficher un graphique de couverture dans l'interface sur chaque build.

---

### 🟡 Q17 — Qu'est-ce que Testcontainers et quel problème résout-il pour les tests d'intégration ?

**Réponse :**

**Testcontainers** est une librairie Java qui permet de **démarrer de vrais containers Docker** (PostgreSQL, Redis, Kafka, etc.) pendant l'exécution des tests, puis de les arrêter automatiquement.

**Problème résolu :**

Avant Testcontainers, les tests d'intégration utilisaient souvent une base de données embarquée comme H2 pour simuler PostgreSQL. Mais H2 ne se comporte pas exactement comme PostgreSQL : certaines requêtes SQL spécifiques à PostgreSQL échouent sur H2, et les bugs n'apparaissent qu'en production.

**Avec Testcontainers :**
```java
@Container
static PostgreSQLContainer<?> postgres = new PostgreSQLContainer<>("postgres:15")
    .withDatabaseName("testdb");
```

- On teste avec la **vraie** base de données PostgreSQL, dans la même version qu'en production.
- Le container est **éphémère** : démarré avant le test, supprimé après.
- Fonctionne parfaitement dans Jenkins car l'agent Jenkins a accès à Docker.

---

## 6. SonarQube & Qualité du Code

---

### 🟢 Q18 — Qu'est-ce que SonarQube et quels types de problèmes détecte-t-il ?

**Réponse :**

**SonarQube** est une plateforme d'analyse statique du code source. Il analyse le code sans l'exécuter et détecte plusieurs catégories de problèmes :

| Catégorie | Exemples |
|---|---|
| **Bugs** | NullPointerException potentiel, ressources non fermées |
| **Vulnerabilities** | Failles de sécurité (injection SQL, XSS, mots de passe en dur) |
| **Code Smells** | Code dupliqué, méthodes trop longues, complexité cyclomatique élevée |
| **Coverage** | Taux de couverture de tests (via JaCoCo) |
| **Duplications** | Blocs de code dupliqués |
| **Security Hotspots** | Code à risque nécessitant une revue manuelle |

Dans Jenkins, on l'intègre avec `mvn sonar:sonar` après les tests.

---

### 🟡 Q19 — Qu'est-ce qu'un Quality Gate SonarQube et comment le faire échouer le pipeline Jenkins ?

**Réponse :**

Un **Quality Gate** est un ensemble de conditions définies dans SonarQube qui doivent toutes être respectées pour qu'une analyse soit considérée comme "OK". C'est le seuil de qualité minimal acceptable.

**Exemple de Quality Gate par défaut (Sonar Way) :**
- Couverture de nouveaux codes ≥ 80%
- Duplications de nouveau code ≤ 3%
- Maintainability Rating du nouveau code = A
- Reliability Rating du nouveau code = A
- Security Rating du nouveau code = A

**Intégration dans Jenkins :**
```groovy
stage('Quality Gate') {
    steps {
        timeout(time: 5, unit: 'MINUTES') {
            script {
                def qg = waitForQualityGate()
                if (qg.status != 'OK') {
                    error "Quality Gate échoué : ${qg.status}"
                }
            }
        }
    }
}
```

`waitForQualityGate()` attend le résultat de l'analyse asynchrone. Si le statut est `ERROR` ou `WARN`, on appelle `error()` qui fait **échouer le pipeline**, bloquant ainsi le déploiement d'un code de mauvaise qualité.

---

### 🔴 Q20 — Quelle est la différence entre une analyse SonarQube sur la branche entière et une analyse sur le nouveau code seulement (New Code) ?

**Réponse :**

SonarQube propose deux modes d'analyse :

**Analyse complète :**
- Analyse **tout le code** du projet.
- Utile pour avoir une vue globale de la qualité.
- Problème : sur un projet existant avec beaucoup de dette technique, le Quality Gate échoue immédiatement à cause du code legacy, même si le nouveau code est parfait.

**New Code / Differential Analysis :**
- Analyse **uniquement les modifications** depuis une date de référence ou un commit précis.
- Le Quality Gate s'applique **seulement au nouveau code**.
- C'est l'approche recommandée car elle ne pénalise pas les nouveaux développements pour la dette technique héritée.
- Dans le pipeline, on utilise `-Dsonar.branch.name=${BRANCH_NAME}` pour que SonarQube compare avec la branche principale.

> **Philosophie** : "Ne pas empirer la dette, corriger progressivement l'existant."

---

## 7. Docker — Build & Push d'Images

---

### 🟢 Q21 — Qu'est-ce qu'un Dockerfile multi-stage et pourquoi est-ce une bonne pratique pour les applications Spring Boot ?

**Réponse :**

Un **Dockerfile multi-stage** utilise plusieurs instructions `FROM` pour séparer l'environnement de build de l'environnement d'exécution.

```dockerfile
# Stage 1 : Build (JDK complet)
FROM eclipse-temurin:17-jdk AS builder
WORKDIR /app
COPY . .
RUN mvn package -DskipTests

# Stage 2 : Runtime (JRE léger)
FROM eclipse-temurin:17-jre AS runtime
COPY --from=builder /app/target/*.jar app.jar
ENTRYPOINT ["java", "-jar", "app.jar"]
```

**Avantages :**

- **Image finale légère** : le JDK complet (>400 MB) est uniquement dans le stage builder, l'image finale ne contient que le JRE (~200 MB) et le JAR.
- **Sécurité** : moins d'outils dans l'image de production = moins de surface d'attaque. Le `mvn`, le code source, etc. ne sont pas dans l'image finale.
- **Build reproductible** : le build Maven se fait dans un environnement contrôlé.

---

### 🟡 Q22 — Comment gérer le tagging des images Docker dans un pipeline CI/CD pour assurer la traçabilité ?

**Réponse :**

Une bonne stratégie de tagging combine plusieurs informations pour garantir la traçabilité :

```groovy
environment {
    // Numéro de build Jenkins + SHA court du commit Git
    IMAGE_TAG = "${env.BUILD_NUMBER}-${env.GIT_COMMIT.take(7)}"
    // Exemple : "42-a3f8c1d"
}
```

**Stratégie par branche :**

| Branche | Tag de l'image |
|---|---|
| `main` | `1.2.3` (version sémantique depuis le pom.xml) + `latest` |
| `develop` | `develop-42-a3f8c1d` |
| `feature/*` | `feature-login-42-a3f8c1d` |
| Tag Git `v1.2.3` | `1.2.3` |

**Pourquoi éviter le tag `latest` seul en production ?**
- `latest` est un alias qui change à chaque push : on ne sait pas quelle version tourne en production.
- Pour les déploiements Kubernetes, toujours utiliser un tag immuable et précis.

---

### 🟡 Q23 — Comment scanner une image Docker pour des vulnérabilités dans un pipeline Jenkins ?

**Réponse :**

On utilise **Trivy** (de Aqua Security), l'outil de scan de vulnérabilités le plus répandu dans les pipelines CI/CD :

```groovy
stage('Security Scan') {
    steps {
        sh """
            docker run --rm \
                -v /var/run/docker.sock:/var/run/docker.sock \
                aquasec/trivy:latest image \
                --exit-code 1 \
                --severity HIGH,CRITICAL \
                --ignore-unfixed \
                ${FULL_IMAGE_NAME}
        """
    }
    post {
        failure {
            echo '⚠️ Vulnérabilités HIGH ou CRITICAL détectées, build bloqué !'
        }
    }
}
```

- `--exit-code 1` : Trivy retourne un code d'erreur si des vulnérabilités sont trouvées → le stage échoue → le pipeline est bloqué avant le déploiement.
- `--severity HIGH,CRITICAL` : on ne bloque que pour les failles les plus graves.
- `--ignore-unfixed` : ignorer les CVE qui n'ont pas encore de correctif disponible.

> C'est un **Security Gate** équivalent au Quality Gate SonarQube, mais pour la sécurité des dépendances.

---

## 8. Deployment — Kubernetes & Stratégies

---

### 🟡 Q24 — Comment Jenkins déploie-t-il sur Kubernetes ? Comment sécurise-t-on l'accès au cluster ?

**Réponse :**

Jenkins interagit avec Kubernetes via `kubectl` en utilisant un fichier `kubeconfig` qui contient les credentials d'accès au cluster. Ce fichier est stocké dans Jenkins comme un **Secret file credential**.

```groovy
stage('Deploy') {
    steps {
        withCredentials([file(credentialsId: 'k8s-config', variable: 'KUBECONFIG')]) {
            sh """
                kubectl apply -f k8s/deployment.yaml -n production
                kubectl rollout status deployment/mon-app -n production --timeout=5m
            """
        }
    }
}
```

**Bonnes pratiques de sécurité :**

- Utiliser un **ServiceAccount Kubernetes dédié** à Jenkins avec des droits RBAC minimaux (uniquement `deploy` sur le namespace cible).
- Ne jamais utiliser le kubeconfig admin en CI/CD.
- Stocker le kubeconfig chiffré dans Jenkins Credentials, jamais en clair dans le Jenkinsfile.
- Utiliser **Vault** ou des solutions comme **External Secrets Operator** pour les secrets applicatifs.

---

### 🔴 Q25 — Quelles sont les différences entre les stratégies de déploiement Rolling Update, Blue-Green et Canary ? Quand utiliser chacune ?

**Réponse :**

**Rolling Update (défaut Kubernetes) :**
- Remplace les instances de l'ancienne version **progressivement**, une par une.
- À mi-chemin, les deux versions coexistent (peut poser des problèmes si l'API a changé).
- **Avantage** : simple, pas besoin d'infrastructure supplémentaire.
- **Inconvénient** : rollback plus lent.
- **Quand l'utiliser** : la majorité des déploiements classiques, applications stateless.

**Blue-Green :**
- Deux environnements identiques : `blue` (version actuelle) et `green` (nouvelle version).
- On déploie sur `green`, on valide, puis on bascule le load balancer d'un coup.
- **Avantage** : rollback instantané (rebascule vers `blue`), zéro downtime.
- **Inconvénient** : double l'infrastructure (coûteux).
- **Quand l'utiliser** : applications critiques où le rollback doit être immédiat.

**Canary :**
- La nouvelle version reçoit un **faible pourcentage du traffic** (ex: 5%), le reste va sur l'ancienne.
- On surveille les métriques (erreurs, latence). Si tout va bien, on augmente progressivement.
- **Avantage** : limite l'impact d'une régression à un petit sous-ensemble d'utilisateurs.
- **Inconvénient** : plus complexe à mettre en place (Istio, NGINX Ingress avec weights).
- **Quand l'utiliser** : nouvelles fonctionnalités à risque, A/B testing.

---

### 🔴 Q26 — Comment gérer le rollback automatique en cas d'échec de déploiement dans Jenkins ?

**Réponse :**

Deux niveaux de rollback :

**1. Rollback Kubernetes automatique :**
Kubernetes surveille les `readinessProbe` et `livenessProbe`. Si les nouveaux pods ne deviennent pas `Ready`, le déploiement reste en échec sans remplacer les anciens pods.

```yaml
# Dans deployment.yaml
readinessProbe:
  httpGet:
    path: /actuator/health/readiness
    port: 8080
  initialDelaySeconds: 30
```

**2. Rollback dans le pipeline Jenkins :**
```groovy
stage('Deploy') {
    steps {
        sh 'kubectl apply -f k8s/deployment.yaml'
        sh 'kubectl rollout status deployment/mon-app --timeout=5m'
    }
    post {
        failure {
            sh 'kubectl rollout undo deployment/mon-app -n production'
            echo '⏪ Rollback effectué automatiquement !'
            slackSend color: 'danger',
                message: "⚠️ Déploiement échoué, rollback vers la version précédente !"
        }
    }
}
```

`kubectl rollout status` **attend** que le déploiement soit complet. Si les pods ne deviennent pas `Ready` dans le timeout, la commande retourne une erreur → le stage `post failure` déclenche le rollback.

---

## 9. Sécurité & Secrets

---

### 🟡 Q27 — Comment gérer les secrets applicatifs (mots de passe de bases de données, API keys) dans un pipeline CI/CD ?

**Réponse :**

Il existe plusieurs approches, du moins sécurisé au plus sécurisé :

**❌ À ne jamais faire :**
```groovy
// En clair dans le Jenkinsfile (versionné dans Git !)
env.DB_PASSWORD = 'mon-super-password-123'
```

**✅ Niveau 1 : Jenkins Credentials :**
```groovy
withCredentials([string(credentialsId: 'db-password', variable: 'DB_PASSWORD')]) {
    sh 'kubectl create secret generic db-secret --from-literal=password=$DB_PASSWORD'
}
```
Les credentials sont chiffrés dans Jenkins et masqués dans les logs.

**✅ Niveau 2 : HashiCorp Vault :**
Les secrets sont stockés dans Vault (solution dédiée à la gestion de secrets), Jenkins récupère dynamiquement un secret à durée de vie limitée au moment du build.

**✅ Niveau 3 : Kubernetes Secrets + External Secrets Operator :**
Les secrets ne passent jamais dans le pipeline. Vault ou AWS Secrets Manager synchronise directement les secrets dans les Kubernetes Secrets. L'application les lit via des variables d'environnement injectées par Kubernetes.

> **Règle d'or** : un secret ne doit jamais apparaître en clair dans un fichier versionné, un log ou une variable d'environnement affichée.

---

## 10. Bonnes Pratiques & Patterns Avancés

---

### 🟡 Q28 — Qu'est-ce que le concept "Fail Fast" dans un pipeline CI/CD et comment l'implémenter ?

**Réponse :**

Le principe **Fail Fast** signifie : **arrêter le pipeline le plus tôt possible** si quelque chose ne va pas, pour ne pas perdre de temps (et d'argent) à exécuter des stages inutiles sur un code déjà cassé.

**Application concrète dans l'ordre des stages :**

```
Clone → Build (compile) → Unit Tests → Integration Tests → SonarQube → Docker Build → Deploy
```

Le **Build vient en premier** : si le code ne compile pas, inutile de lancer les tests.  
Les **Unit Tests** viennent avant les Integration Tests : ils sont plus rapides et détectent les problèmes basiques.  
**SonarQube** vient avant le Build Docker : inutile de builder une image si le code est de mauvaise qualité.

**Dans les stages parallèles :**
```groovy
stage('Tests') {
    failFast true  // Si un stage parallèle échoue, annule les autres
    parallel {
        stage('Unit Tests')       { ... }
        stage('Integration Tests') { ... }
    }
}
```

---

### 🟡 Q29 — Comment versionner les artefacts Maven dans un pipeline CI/CD ? Quelle est la différence entre un SNAPSHOT et une release ?

**Réponse :**

**SNAPSHOT :**
- Version suffixée par `-SNAPSHOT` : `1.2.3-SNAPSHOT`
- Indique une version **en cours de développement**, instable.
- Chaque déploiement sur Nexus/Artifactory **écrase** la version précédente.
- Utilisée sur les branches `develop` et `feature/*`.

**Release :**
- Version numérique fixe : `1.2.3`
- Indique une version **stable et immuable**.
- Une fois déployée sur Nexus, elle ne peut plus être modifiée (par convention).
- Utilisée sur la branche `main` ou les tags Git.

**Dans le pipeline Jenkins :**
```groovy
script {
    if (env.BRANCH_NAME == 'main') {
        // Supprimer le SNAPSHOT pour créer une release
        sh "mvn versions:set -DnewVersion=${APP_VERSION.replace('-SNAPSHOT', '')} -DgenerateBackupPoms=false"
        sh "mvn deploy -DskipTests"  // Pousser vers Nexus
    }
}
```

---

### 🔴 Q30 — Comment implémenter un pipeline de promotion d'artefacts entre environnements (staging → production) sans rebuilder ?

**Réponse :**

Le principe est : **"Build once, deploy many"**. On construit le JAR et l'image Docker une seule fois et on les **promeut** d'un environnement à l'autre, sans rebuilder.

**Pourquoi ne pas rebuilder ?**
- Un rebuild pourrait produire un artefact différent (dépendances mises à jour, aléatoire dans les tests).
- Ce qu'on a testé en staging doit être **exactement** ce qu'on déploie en production.

**Implémentation :**

```groovy
// Stage 1 : Build sur develop → image taggée "staging"
stage('Build & Push') {
    steps {
        sh "docker build -t ${REGISTRY}/${APP}:${IMAGE_TAG} ."
        sh "docker push ${REGISTRY}/${APP}:${IMAGE_TAG}"
    }
}

// Stage 2 : Promote vers production → re-tagger sans rebuilder
stage('Promote to Production') {
    when { branch 'main' }
    steps {
        input message: 'Promouvoir en production ?', ok: 'Oui'
        sh """
            # Tirer l'image de staging et la re-tagger en production
            docker pull ${REGISTRY}/${APP}:${IMAGE_TAG}
            docker tag  ${REGISTRY}/${APP}:${IMAGE_TAG} ${REGISTRY}/${APP}:${PROD_TAG}
            docker push ${REGISTRY}/${APP}:${PROD_TAG}
        """
    }
}
```

Sur **Nexus/Artifactory**, la promotion se fait via l'API REST pour déplacer un artefact du repository `snapshots` vers `releases` sans reconstruction.

---

## 11. Troubleshooting & Scénarios Pratiques

---

### 🟡 Q31 — Scénario : le pipeline échoue à cause du Quality Gate SonarQube mais les tests passent tous. Comment diagnostiquer et résoudre ?

**Réponse :**

**Étapes de diagnostic :**

1. **Aller sur l'interface SonarQube** : `http://sonarqube:9000/dashboard?id=mon-app`
2. **Identifier quelle condition du Quality Gate a échoué** : couverture ? Duplications ? Security Rating ?
3. **Analyser les métriques en détail** :
   - Si c'est la couverture : quels packages/classes ne sont pas couverts ? Les nouvelles classes ont-elles des tests ?
   - Si c'est un Code Smell ou Bug critique : corriger dans le code.
   - Si c'est une Security Hotspot : revoir le code signalé.

**Solutions courantes :**

- **Couverture insuffisante** : ajouter des tests unitaires pour les nouvelles classes.
- **Faux positifs** : annoter le code avec `@SuppressWarnings("java:S...")` ou configurer des exclusions dans `sonar-project.properties`.
- **Quality Gate trop strict** : si c'est un projet legacy, assouplir temporairement le Quality Gate et créer un plan de réduction de la dette technique.

```properties
# sonar-project.properties - Exclure du coverage
sonar.coverage.exclusions=**/config/**,**/*Application.java,**/dto/**
```

---

### 🔴 Q32 — Scénario : vous avez 15 microservices Spring Boot, tous avec quasiment le même pipeline. Comment organisez-vous votre infrastructure Jenkins pour éviter la duplication ?

**Réponse :**

La solution est une combinaison de **Shared Libraries** + **Multibranch Pipelines** + **templates de Jenkinsfile minimalistes**.

**1. Créer une Shared Library `ci-cd-commons` :**
```groovy
// vars/springBootPipeline.groovy
def call(Map config) {
    pipeline {
        agent any
        tools { maven 'Maven-3.9'; jdk 'JDK-17' }
        stages {
            stage('Clone')   { steps { checkout scm } }
            stage('Build')   { steps { sh 'mvn package -DskipTests -B' } }
            stage('Test')    { steps { sh 'mvn verify -B' } }
            stage('Sonar')   { steps { withSonarQubeEnv('SonarQube') { sh 'mvn sonar:sonar' } } }
            stage('Docker')  { steps { sh "docker build -t ${config.imageName}:${BUILD_NUMBER} ." } }
            stage('Deploy')  {
                when { branch 'main' }
                steps { sh "helm upgrade --install ${config.releaseName} ./helm" }
            }
        }
    }
}
```

**2. Jenkinsfile de chaque microservice (5 lignes !) :**
```groovy
@Library('ci-cd-commons@main') _

springBootPipeline(
    imageName: 'registry.example.com/user-service',
    releaseName: 'user-service'
)
```

**3. Résultat :**
- Un seul endroit pour maintenir le pipeline.
- Chaque microservice a un Jenkinsfile de quelques lignes.
- Ajouter un scan de sécurité à tous les pipelines = modifier la Shared Library une fois.

---

### 🔴 Q33 — Scénario : en production, le déploiement Kubernetes est bloqué depuis Jenkins car `kubectl rollout status` timeout. Quelles sont vos étapes de diagnostic ?

**Réponse :**

**Étape 1 : Vérifier l'état des pods**
```bash
kubectl get pods -n production -l app=mon-app
# STATUS: CrashLoopBackOff ? ImagePullBackOff ? Pending ?
```

**Étape 2 : Lire les événements Kubernetes**
```bash
kubectl describe deployment mon-app -n production
kubectl describe pod <pod-name> -n production
# Events: OOMKilled ? Failed to pull image ? Readiness probe failed ?
```

**Étape 3 : Lire les logs du pod en échec**
```bash
kubectl logs <pod-name> -n production --previous
# Spring Boot a-t-il démarré ? Exception au démarrage ?
```

**Causes fréquentes et solutions :**

| Symptôme | Cause probable | Solution |
|---|---|---|
| `ImagePullBackOff` | Mauvais tag ou credentials registry invalides | Vérifier le tag de l'image et les imagePullSecrets |
| `CrashLoopBackOff` | Exception au démarrage Spring Boot | Lire les logs, vérifier les variables d'environnement |
| `OOMKilled` | Mémoire insuffisante | Augmenter les `limits.memory` dans le deployment |
| `Readiness probe failed` | L'app démarre trop lentement | Augmenter `initialDelaySeconds` de la readinessProbe |
| `Pending` | Pas assez de ressources sur le cluster | Vérifier les ressources disponibles (`kubectl describe nodes`) |

**Dans le pipeline Jenkins :** ajouter un rollback automatique dans le `post { failure { ... } }` pour minimiser le temps de downtime.

---

*Questions d'entretien pour un profil DevOps / Développeur Java/Spring avec expérience CI/CD.*  
*Niveau : Junior à Senior selon les questions choisies.*
