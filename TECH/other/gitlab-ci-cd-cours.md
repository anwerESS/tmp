# GitLab CI/CD — Cours complet pour entretien

> Objectif : comprendre **toutes** les notions, savoir écrire un `.gitlab-ci.yml` propre, le débugger, et répondre aux questions d'entretien.

---

## Sommaire

1. [CI/CD : rappel des concepts](#1-cicd--rappel-des-concepts)
2. [Architecture GitLab CI](#2-architecture-gitlab-ci)
3. [Anatomie d'un pipeline](#3-anatomie-dun-pipeline-pipeline--stages--jobs--script)
4. [Mots-clés essentiels](#4-mots-clés-essentiels-du-gitlab-ciyml)
5. [Variables](#5-variables)
6. [`rules` : contrôler quand un job tourne](#6-rules--contrôler-quand-un-job-tourne)
7. [Cache vs Artifacts](#7-cache-vs-artifacts)
8. [Réutiliser du code (DRY)](#8-réutiliser-du-code-dry)
9. [Types de pipelines & DAG](#9-types-de-pipelines--dag)
10. [Environments & Deployments](#10-environments--deployments)
11. [Sécurité](#11-sécurité)
12. [Notions avancées complémentaires](#12-notions-avancées-complémentaires)
13. [Pipeline complet — Java (Spring Boot + Maven + Docker)](#13-pipeline-complet--java-spring-boot--maven--docker)
14. [Pipeline complet — Python](#14-pipeline-complet--python)
15. [Pipeline complet — Node.js / Angular](#15-pipeline-complet--nodejs--angular)
16. [Troubleshooting](#16-troubleshooting)
17. [Optimisation des pipelines](#17-optimisation-des-pipelines)
18. [Best practices](#18-best-practices)
19. [Questions d'entretien (avec réponses)](#19-questions-dentretien-avec-réponses)
20. [Cheat sheet](#20-cheat-sheet)

---

## 1. CI/CD : rappel des concepts

| Terme | Signification | Ce que ça fait concrètement |
|---|---|---|
| **CI** (Continuous Integration) | Intégrer le code très souvent dans la branche principale | À chaque `push` : build + tests + analyse qualité automatiques |
| **CD** (Continuous Delivery) | Le code est **toujours déployable** | Artefact/image prêt, déploiement en prod **déclenché manuellement** (bouton) |
| **CD** (Continuous Deployment) | Déploiement en prod **100 % automatique** | Si le pipeline est vert → prod, sans intervention humaine |

**Pourquoi c'est important (à dire en entretien)** :
- Feedback rapide (on casse tôt, pas en prod)
- Builds reproductibles (même environnement à chaque fois, souvent via Docker)
- Moins d'erreurs humaines, déploiements fréquents et petits (donc moins risqués)
- Traçabilité : chaque déploiement est lié à un commit et à un pipeline

---

## 2. Architecture GitLab CI

```
Developer ──push──▶ GitLab Server ──crée──▶ Pipeline ──▶ Jobs en file d'attente
                                                              │
                                              GitLab Runner ◀─┘ (poll via HTTPS, "pull model")
                                                    │
                                          Executor (docker, shell, kubernetes…)
                                                    │
                                              exécute le `script`
```

### Composants

- **GitLab server** (gitlab.com ou self-hosted) : stocke le code, lit `.gitlab-ci.yml`, orchestre les pipelines, stocke artifacts/logs.
- **GitLab Runner** : agent (binaire Go) qui **exécute** les jobs. Il *demande* du travail au serveur (pull) → pas besoin d'ouvrir de port entrant sur le runner.
- **Executor** : *comment* le runner exécute le job.

| Executor | Usage | Avis |
|---|---|---|
| `docker` | Chaque job dans un container éphémère | ✅ Le standard. Environnement propre et reproductible |
| `kubernetes` | Chaque job = un Pod | ✅ Idéal en cloud / autoscaling |
| `shell` | Directement sur la machine du runner | ⚠️ Pollution entre jobs, difficile à reproduire |
| `docker-autoscaler` / `instance` | VMs éphémères (cloud) | Pour gros volumes |

### Types de runners

- **Shared runners** : fournis par l'instance (gitlab.com en fournit), partagés entre projets.
- **Group runners** : dédiés à un groupe.
- **Project runners** : dédiés à un seul projet.
- **Tags** : étiquettes pour router un job vers un runner précis (`tags: [docker, gpu]`).
- **Protected runners** : n'exécutent que les branches/tags protégés (utile pour la prod).

### Enregistrer un runner (self-hosted)

```bash
# Installer gitlab-runner puis :
gitlab-runner register \
  --url https://gitlab.example.com \
  --token <RUNNER_AUTHENTICATION_TOKEN> \
  --executor docker \
  --docker-image alpine:3.20
```

La config se trouve dans `/etc/gitlab-runner/config.toml` (`concurrent`, `[[runners]]`, `volumes`, `privileged`…).

### Cycle de vie d'un job

1. Le runner prend le job → crée un container (executor docker)
2. `git clone`/`fetch` du repo
3. Restaure le **cache**, télécharge les **artifacts** des jobs précédents (`dependencies`/`needs`)
4. Exécute `before_script` puis `script` (dans le **même shell**)
5. Exécute `after_script` (shell **séparé**, s'exécute même en cas d'échec)
6. Upload du cache et des artifacts
7. Statut : `success` / `failed` / `canceled`… et le container est détruit

---

## 3. Anatomie d'un pipeline (pipeline → stages → jobs → script)

```
Pipeline
 ├── Stage: build   ──▶ Job: compile
 ├── Stage: test    ──▶ Job: unit-test  ┐ (exécutés en parallèle)
 │                      Job: lint       ┘
 └── Stage: deploy  ──▶ Job: deploy-prod
```

| Notion | Définition |
|---|---|
| **Pipeline** | L'ensemble du workflow déclenché par un événement (push, MR, tag, schedule, API…). Défini dans `.gitlab-ci.yml` à la racine du repo |
| **Stage** | Une **phase**. Les stages s'exécutent **séquentiellement** (dans l'ordre de `stages:`). Si un stage échoue, les suivants ne démarrent pas (par défaut) |
| **Job** | L'unité de travail de base. Les jobs d'un **même stage** tournent **en parallèle** (si assez de runners) |
| **Script** | Liste de commandes shell exécutées par le job |

### Exemple minimal

```yaml
stages:          # ordre d'exécution des stages
  - build
  - test
  - deploy

build-job:
  stage: build
  image: alpine:3.20       # image Docker utilisée (executor docker)
  script:
    - echo "Compilation..."
    - mkdir -p out && echo "hello" > out/app.txt
  artifacts:
    paths:
      - out/               # transmis aux jobs des stages suivants

test-job:
  stage: test
  script:
    - cat out/app.txt      # récupère l'artifact du build-job
    - echo "Tests OK"

deploy-job:
  stage: deploy
  script:
    - echo "Déploiement..."
  environment: production
  when: manual             # bouton "play" dans l'UI
```

> **Si `stages` n'est pas défini**, GitLab utilise par défaut : `.pre`, `build`, `test`, `deploy`, `.post`. `.pre` et `.post` sont des stages spéciaux (toujours premier / dernier).

> Un job sans `stage:` est placé dans le stage **`test`** par défaut.

---

## 4. Mots-clés essentiels du `.gitlab-ci.yml`

### 4.1 Global

```yaml
default:                  # valeurs par défaut pour TOUS les jobs
  image: node:22-alpine
  tags: [docker]
  retry:
    max: 2
    when: [runner_system_failure, stuck_or_timeout_failure]
  interruptible: true     # le job peut être annulé si un pipeline plus récent démarre

variables:                # variables globales
  GIT_DEPTH: "20"         # shallow clone (plus rapide)

stages: [build, test, deploy]

workflow:                 # contrôle si le PIPELINE est créé (voir §6)
  rules:
    - if: $CI_COMMIT_BRANCH
```

### 4.2 Les mots-clés d'un job

| Mot-clé | Rôle | Exemple |
|---|---|---|
| `stage` | Stage du job | `stage: test` |
| `image` | Image Docker du job | `image: maven:3.9-eclipse-temurin-21` |
| `script` | **Obligatoire.** Commandes | `script: [mvn test]` |
| `before_script` | Exécuté avant `script` (même shell) | installation de dépendances |
| `after_script` | Exécuté après, **même si échec**, shell séparé | cleanup, logs |
| `variables` | Variables du job | `variables: { ENV: staging }` |
| `rules` | Conditions d'exécution (**à préférer**) | voir §6 |
| `only` / `except` | ⚠️ Ancienne syntaxe, **à éviter** | — |
| `when` | `on_success` (défaut), `on_failure`, `always`, `manual`, `delayed`, `never` | `when: manual` |
| `allow_failure` | Le job peut échouer sans casser le pipeline | `allow_failure: true` |
| `needs` | Dépendances explicites → **DAG** (démarre sans attendre le stage) | `needs: [build]` |
| `dependencies` | Quels jobs fournissent leurs artifacts | `dependencies: [build]` |
| `artifacts` | Fichiers produits conservés (voir §7) | — |
| `cache` | Fichiers réutilisés entre pipelines (voir §7) | — |
| `services` | Containers "sidecar" (DB, Redis…) | `services: [mongo:7]` |
| `tags` | Sélection du runner | `tags: [docker]` |
| `timeout` | Timeout du job | `timeout: 30m` |
| `retry` | Relance automatique | `retry: 2` |
| `parallel` | Exécuter N instances / matrix | `parallel: 3` |
| `environment` | Associer un déploiement à un environnement | `environment: staging` |
| `resource_group` | Empêche 2 déploiements simultanés | `resource_group: prod` |
| `coverage` | Regex pour extraire le % de coverage des logs | `coverage: '/TOTAL.*\s(\d+)%/'` |
| `trigger` | Déclenche un pipeline enfant / autre projet | voir §9 |
| `extends` / `include` | Réutilisation | voir §8 |
| `release` | Crée une Release GitLab | — |
| `interruptible` | Annulable automatiquement | `true` |
| `inherit` | Contrôle l'héritage de `default`/`variables` | `inherit: { default: false }` |

### 4.3 `services` (containers annexes)

```yaml
integration-test:
  image: maven:3.9-eclipse-temurin-21
  services:
    - name: mongo:7
      alias: mongo                  # hostname = "mongo"
    - name: postgres:16
      alias: db
  variables:
    POSTGRES_DB: test
    POSTGRES_USER: test
    POSTGRES_PASSWORD: test
  script:
    - mvn verify -Dspring.data.mongodb.uri=mongodb://mongo:27017/test
```

> Avec l'executor `docker`, le service est joignable par son **alias** (ou nom d'image), pas `localhost`.

### 4.4 `when: manual`, `delayed`, `allow_failure`

```yaml
deploy-prod:
  stage: deploy
  script: ./deploy.sh prod
  when: manual              # nécessite un clic
  allow_failure: false      # ⚠️ par défaut manual = allow_failure: true (le pipeline continue)
                            # → mettre false pour BLOQUER le pipeline tant que non cliqué

smoke-test:
  script: ./smoke.sh
  when: delayed
  start_in: 5 minutes
```

### 4.5 `parallel` et `matrix`

```yaml
test:
  parallel:
    matrix:
      - PYTHON_VERSION: ["3.11", "3.12", "3.13"]
  image: python:${PYTHON_VERSION}-slim
  script: pytest
```

→ génère 3 jobs en parallèle (un par version).

### 4.6 `needs` — le DAG (Directed Acyclic Graph)

Par défaut, un job attend la **fin de tout le stage précédent**. Avec `needs`, il démarre dès que ses dépendances sont terminées → pipeline **plus rapide**.

```yaml
stages: [build, test, deploy]

build-front:
  stage: build
  script: ./build-front.sh
build-back:
  stage: build
  script: ./build-back.sh   # lent

test-front:
  stage: test
  needs: [build-front]      # démarre dès que build-front est fini
  script: ./test-front.sh
```

> `needs: []` (liste vide) → le job démarre **immédiatement**, sans attendre aucun job.

---

## 5. Variables

### 5.1 Où les définir

| Niveau | Où | Usage |
|---|---|---|
| Dans le YAML | `variables:` global ou job | Valeurs **non secrètes** |
| Project / Group / Instance | *Settings → CI/CD → Variables* | **Secrets** (tokens, mots de passe) |
| Au lancement | *Run pipeline* manuel / API / schedule | Paramétrage ponctuel |

Options des variables UI :
- **Masked** : la valeur est masquée dans les logs (contraintes de format : 8+ caractères, pas d'espaces…)
- **Protected** : disponible uniquement sur branches/tags **protégés**
- **Type File** : GitLab écrit la valeur dans un fichier temporaire, la variable contient le **chemin** (idéal pour clés SSH, kubeconfig, certificats)
- **Environment scope** : `production`, `staging`, `*` → même nom, valeur différente par environnement

### 5.2 Précédence (du plus fort au plus faible)

1. Variables de trigger / schedule / run manuel
2. Variables du **projet**
3. Variables du **groupe**
4. Variables de l'**instance**
5. Variables héritées (pipelines parent→enfant)
6. Variables définies dans un **job** du YAML
7. Variables **globales** du YAML
8. Variables **prédéfinies**

### 5.3 Variables prédéfinies à connaître

| Variable | Contenu |
|---|---|
| `CI_COMMIT_SHA` / `CI_COMMIT_SHORT_SHA` | Hash du commit (complet / 8 caractères) |
| `CI_COMMIT_BRANCH` | Nom de la branche (vide pour les tags et MR pipelines) |
| `CI_COMMIT_REF_NAME` | Branche **ou** tag |
| `CI_COMMIT_REF_SLUG` | Idem mais utilisable dans une URL/nom Docker |
| `CI_COMMIT_TAG` | Nom du tag (si pipeline de tag) |
| `CI_DEFAULT_BRANCH` | Branche par défaut (`main`) |
| `CI_PIPELINE_SOURCE` | `push`, `merge_request_event`, `schedule`, `web`, `api`, `trigger`, `pipeline`… |
| `CI_MERGE_REQUEST_IID` | ID de la MR |
| `CI_PROJECT_DIR` | Répertoire de checkout |
| `CI_PROJECT_NAME`, `CI_PROJECT_PATH` | Nom / chemin du projet |
| `CI_REGISTRY` / `CI_REGISTRY_IMAGE` | URL du Container Registry / nom d'image du projet |
| `CI_REGISTRY_USER` / `CI_REGISTRY_PASSWORD` | Credentials temporaires du registry |
| `CI_JOB_TOKEN` | Token temporaire (API GitLab, packages, trigger) |
| `CI_JOB_ID`, `CI_PIPELINE_ID` | Identifiants |
| `CI_ENVIRONMENT_NAME` | Nom de l'environment du job |
| `CI_API_V4_URL` | URL de l'API GitLab |

### 5.4 Utilisation

```yaml
variables:
  APP_ENV: "staging"
  IMAGE: "$CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA"

job:
  script:
    - echo "Deploying $IMAGE to $APP_ENV"
    - echo "Token: $MY_SECRET"        # sera masqué si "Masked"
```

> ⚠️ Pour utiliser une variable dans `rules:if`, la syntaxe est `$VAR` sans guillemets autour de la variable : `if: $CI_COMMIT_BRANCH == "main"`.

---

## 6. `rules` : contrôler quand un job tourne

Les `rules` sont évaluées **dans l'ordre** ; **la première qui matche gagne**. Si aucune ne matche → le job n'est **pas** créé.

```yaml
deploy-prod:
  script: ./deploy.sh
  rules:
    - if: $CI_COMMIT_TAG =~ /^v\d+\.\d+\.\d+$/    # tag sémantique
      when: manual
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH
      when: manual
      allow_failure: true
    - when: never                                  # sinon : jamais
```

### Clauses disponibles

| Clause | Exemple | Sens |
|---|---|---|
| `if` | `if: $CI_PIPELINE_SOURCE == "merge_request_event"` | Condition sur variable |
| `changes` | `changes: [src/**/*, pom.xml]` | Si ces fichiers ont changé |
| `exists` | `exists: [Dockerfile]` | Si le fichier existe |
| `when` | `when: manual` | Comportement si la règle matche |
| `allow_failure` | `true` | Override |
| `variables` | `variables: { ENV: prod }` | Variables si la règle matche |

### `workflow:rules` — éviter les pipelines en double

Problème classique : un `push` sur une branche avec une MR ouverte crée **2 pipelines** (branch + MR). Solution :

```yaml
workflow:
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
    - if: $CI_COMMIT_BRANCH && $CI_OPEN_MERGE_REQUESTS
      when: never                    # branche avec MR ouverte → pas de pipeline "branche"
    - if: $CI_COMMIT_BRANCH
    - if: $CI_COMMIT_TAG
```

### Pattern de règles réutilisables

```yaml
.rules-main-only:
  rules:
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH

deploy:
  extends: .rules-main-only
  script: ./deploy.sh
```

> **Avis candide** : utilisez `rules`, jamais `only/except` (legacy, moins expressif, ne doit pas être mélangé avec `rules` dans un même job).

---

## 7. Cache vs Artifacts

C'est **LA** question classique d'entretien.

| | **Cache** | **Artifacts** |
|---|---|---|
| But | Accélérer (dépendances : `.m2`, `node_modules`, `.cache/pip`) | **Transmettre** des résultats entre jobs / les conserver |
| Portée | Entre **pipelines** (même branche/clé) | Entre **jobs du même pipeline** (+ téléchargeables via UI) |
| Garantie | ❌ **Best effort** (peut ne pas exister) | ✅ Garanti |
| Stockage | Sur le runner (ou S3/GCS si *distributed cache*) | Sur le serveur GitLab |
| Exemples | `.m2/repository`, `.npm/` | `target/*.jar`, `dist/`, rapports de tests |

### Cache

```yaml
cache:
  key:
    files:
      - package-lock.json      # nouveau cache si le lockfile change
  paths:
    - .npm/
  policy: pull-push            # pull (lecture seule), push (écriture seule), pull-push (défaut)
```

Astuces :
- `key: $CI_COMMIT_REF_SLUG` → un cache par branche
- `policy: pull` sur les jobs qui ne modifient pas le cache (gain de temps)
- `fallback_keys` : clé de secours (ex. cache de `main`) si la clé exacte n'existe pas
- ⚠️ Le cache **doit** être dans `$CI_PROJECT_DIR` → on redirige les outils : `-Dmaven.repo.local=.m2/repository`, `npm ci --cache .npm`, `PIP_CACHE_DIR`.

### Artifacts

```yaml
build:
  script: mvn package
  artifacts:
    paths:
      - target/*.jar
    expire_in: 1 week          # nettoyage automatique
    when: always               # même si le job échoue (utile pour les rapports)
    reports:
      junit: target/surefire-reports/TEST-*.xml     # tests visibles dans la MR
      coverage_report:
        coverage_format: cobertura
        path: coverage.xml                           # coverage visible dans les diffs de MR
```

- `dependencies: []` → ne télécharge **aucun** artifact (accélère les jobs qui n'en ont pas besoin)
- `needs: [{ job: build, artifacts: false }]` → dépend du job mais sans télécharger ses artifacts
- Les chemins sont **relatifs à `$CI_PROJECT_DIR`**

---

## 8. Réutiliser du code (DRY)

### 8.1 Hidden jobs + `extends`

```yaml
.node-base:                      # le "." = job caché, jamais exécuté seul
  image: node:22-alpine
  cache:
    key: { files: [package-lock.json] }
    paths: [.npm/]
  before_script:
    - npm ci --cache .npm --prefer-offline

lint:
  extends: .node-base
  stage: test
  script: npm run lint

unit-test:
  extends: .node-base
  stage: test
  script: npm test
```

> `extends` **fusionne** les hashes (merge profond) mais **écrase** les tableaux (`script`, etc.).

### 8.2 `!reference` (réutiliser un morceau précis)

```yaml
.setup:
  script:
    - echo "setup"

job:
  script:
    - !reference [.setup, script]
    - echo "mon vrai script"
```

### 8.3 YAML anchors

```yaml
.defaults: &defaults
  image: alpine:3.20
  tags: [docker]

job1:
  <<: *defaults
  script: echo 1
```

> Les anchors ne fonctionnent **que dans un même fichier**. Préférez `extends`.

### 8.4 `include`

```yaml
include:
  - local: ci/build.yml                           # fichier du même repo
  - project: my-group/ci-templates                # autre projet GitLab
    ref: main
    file: /templates/java.yml
  - remote: https://example.com/ci/common.yml     # URL publique
  - template: Security/SAST.gitlab-ci.yml         # template officiel GitLab
  - component: $CI_SERVER_FQDN/my-org/components/sast@1.0.0   # CI/CD Component (catalog)
```

> Les **CI/CD Components** (versionnés, paramétrables avec `spec:inputs`) sont la manière moderne de partager du CI entre projets.

### 8.5 Templates GitLab "Security" (gratuits ou Ultimate selon le scanner)

```yaml
include:
  - template: Security/SAST.gitlab-ci.yml
  - template: Security/Secret-Detection.gitlab-ci.yml
  - template: Security/Dependency-Scanning.gitlab-ci.yml
```

---

## 9. Types de pipelines & DAG

| Type | Déclencheur | Détail |
|---|---|---|
| **Branch pipeline** | push sur une branche | `CI_PIPELINE_SOURCE == "push"` |
| **Merge request pipeline** | MR ouverte/mise à jour | Nécessite `rules: - if: $CI_PIPELINE_SOURCE == "merge_request_event"` |
| **Merged results pipeline** | MR | Teste le résultat **du merge** (source + target) — plus fiable |
| **Tag pipeline** | création d'un tag | Idéal pour les **releases** |
| **Scheduled pipeline** | cron configuré dans l'UI | Nightly build, scans |
| **API / Trigger** | `curl` avec trigger token | Intégrations externes |
| **Parent-child** | `trigger:include` | Découper un gros pipeline / **monorepo** |
| **Multi-project** | `trigger:project` | Un projet en déclenche un autre |

### Parent-child (monorepo)

```yaml
# .gitlab-ci.yml (parent)
stages: [triggers]

backend:
  stage: triggers
  trigger:
    include: backend/.gitlab-ci.yml
    strategy: depend           # le parent attend le statut de l'enfant
  rules:
    - changes: [backend/**/*]

frontend:
  stage: triggers
  trigger:
    include: frontend/.gitlab-ci.yml
    strategy: depend
  rules:
    - changes: [frontend/**/*]
```

### Multi-project

```yaml
deploy-infra:
  trigger:
    project: my-group/infra
    branch: main
    strategy: depend
```

---

## 10. Environments & Deployments

```yaml
deploy-staging:
  stage: deploy
  script: ./deploy.sh staging
  environment:
    name: staging
    url: https://staging.example.com
  resource_group: staging              # 1 seul déploiement à la fois
  rules:
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH

deploy-production:
  stage: deploy
  script: ./deploy.sh production
  environment:
    name: production
    url: https://example.com
  resource_group: production
  when: manual
  rules:
    - if: $CI_COMMIT_TAG
```

### Review Apps (environnement par Merge Request)

```yaml
deploy-review:
  stage: deploy
  script: ./deploy-review.sh $CI_COMMIT_REF_SLUG
  environment:
    name: review/$CI_COMMIT_REF_SLUG
    url: https://$CI_COMMIT_REF_SLUG.review.example.com
    on_stop: stop-review
    auto_stop_in: 2 days
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"

stop-review:
  stage: deploy
  script: ./destroy-review.sh $CI_COMMIT_REF_SLUG
  environment:
    name: review/$CI_COMMIT_REF_SLUG
    action: stop
  when: manual
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
      when: manual
```

**Stratégies de déploiement** à connaître : *rolling*, *blue/green*, *canary*, *rollback* (re-déployer un ancien commit/tag depuis la page *Environments*).

---

## 11. Sécurité

- 🔐 **Jamais** de secrets dans le `.gitlab-ci.yml` ni dans le repo → variables **Masked + Protected**.
- Variables **File** pour clés SSH/kubeconfig.
- **Protected branches / tags** : seuls eux accèdent aux variables protégées (prod).
- `CI_JOB_TOKEN` plutôt qu'un Personal Access Token (portée limitée, durée de vie = le job).
- **Deploy tokens** : accès read-only au registry pour les serveurs de prod.
- Intégration d'un **secret manager** : HashiCorp Vault, AWS/GCP/Azure via **OIDC** (`id_tokens:`) → pas de credentials statiques.
- Épingler les versions d'images (`node:22.11-alpine`) voire par **digest** ; éviter `latest`.
- Runners **privileged** (nécessaire pour Docker-in-Docker) = risque → isoler.
- **Approvals** pour les déploiements prod (*Protected environments*).
- Scanners : SAST, Secret Detection, Dependency Scanning, Container Scanning.

---

## 12. Notions avancées complémentaires

### 12.1 Statuts d'un pipeline / job

| Statut | Signification |
|---|---|
| `created` / `waiting_for_resource` | Job créé, attend une ressource (`resource_group`) |
| `pending` | En file d'attente, attend un runner |
| `running` | En cours |
| `success` | Terminé avec succès |
| `failed` | Échec (exit code ≠ 0, timeout, erreur système) |
| `canceled` | Annulé (manuellement ou par `interruptible` / auto-cancel) |
| `skipped` | Non exécuté (ex. stage précédent en échec) |
| `manual` | Attend un clic (`when: manual`) |
| `scheduled` | `when: delayed`, attend `start_in` |
| `passed with warnings` | Pipeline vert mais un job `allow_failure: true` a échoué |

### 12.2 Passer des **variables** entre jobs : `artifacts:reports:dotenv`

Les artifacts transmettent des **fichiers** ; pour transmettre une **valeur calculée** (ex. version), on utilise un fichier `.env` déclaré comme report `dotenv`.

```yaml
compute-version:
  stage: build
  script:
    - echo "APP_VERSION=$(date +%Y.%m.%d)-$CI_COMMIT_SHORT_SHA" >> build.env
  artifacts:
    reports:
      dotenv: build.env          # les variables deviennent disponibles dans les jobs suivants

deploy:
  stage: deploy
  needs:
    - job: compute-version
      artifacts: true
  script:
    - echo "Déploiement de la version $APP_VERSION"
```

### 12.3 Merge Request pipelines, Merged Results et Merge Trains

| Mécanisme | Ce qu'il teste | Intérêt |
|---|---|---|
| **MR pipeline** | Le code de la branche source | Accès à `CI_MERGE_REQUEST_*`, rapports dans la MR |
| **Merged results pipeline** | Le résultat **du merge** (source + target) | Détecte les conflits sémantiques avant merge |
| **Merge train** *(Premium)* | File d'attente de MR, chacune testée avec celles qui la précèdent | `main` toujours verte, même avec beaucoup de MR concurrentes |

Prérequis : jobs avec `rules: - if: $CI_PIPELINE_SOURCE == "merge_request_event"`, puis activer *Merged results pipelines* dans *Settings → Merge requests*.

### 12.4 Pipelines enfants **dynamiques** (config générée à la volée)

Utile pour monorepo, matrices complexes, génération dynamique de jobs.

```yaml
generate-config:
  stage: build
  script:
    - ./scripts/generate-ci.sh > generated-ci.yml
  artifacts:
    paths: [generated-ci.yml]

run-generated:
  stage: test
  needs: [generate-config]
  trigger:
    include:
      - artifact: generated-ci.yml
        job: generate-config
    strategy: depend
```

### 12.5 `rules:changes` avancé (`compare_to`)

Par défaut, `changes` compare au commit précédent (peu fiable sur les branches de feature). On compare plutôt à la branche cible :

```yaml
lint-backend:
  script: ./lint-backend.sh
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
      changes:
        paths:
          - backend/**/*
        compare_to: refs/heads/main
```

### 12.6 Variables : options avancées

```yaml
variables:
  # Variable "sélectionnable" au lancement manuel d'un pipeline (Run pipeline)
  DEPLOY_ENV:
    value: "staging"
    options: ["staging", "production"]
    description: "Environnement cible du déploiement"

  # Empêcher l'expansion des $ dans la valeur
  PASSWORD_REGEX:
    value: 'ab$cd'
    expand: false
```

- Pour écrire un `$` littéral dans le YAML : `$$` (ex. `echo "Prix: $$5"`).
- Une variable peut en référencer une autre : `IMAGE: $CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA`.
- **Skip CI** : `git commit -m "docs: typo [ci skip]"` ou `git push -o ci.skip` → aucun pipeline.

### 12.7 **CI/CD Components** et `spec:inputs` (templates paramétrables)

Un *Component* est un fichier YAML versionné, publiable dans le **CI/CD Catalog**, avec des paramètres (`inputs`).

```yaml
# templates/maven-build.yml  (dans le projet "my-org/components")
spec:
  inputs:
    java_version:
      default: "21"
    stage:
      default: build
---
maven-build:
  stage: $[[ inputs.stage ]]
  image: maven:3.9-eclipse-temurin-$[[ inputs.java_version ]]
  script:
    - mvn --batch-mode package
```

```yaml
# Utilisation dans un autre projet
include:
  - component: $CI_SERVER_FQDN/my-org/components/maven-build@1.2.0
    inputs:
      java_version: "17"
```

> Toujours **épingler la version** (`@1.2.0`) d'un component/template partagé : un `@main` flottant peut casser tous vos pipelines d'un coup.

### 12.8 Authentification **sans secret statique** : OIDC (`id_tokens`)

GitLab délivre un **JWT** signé et éphémère par job. Le cloud (AWS, GCP, Azure) ou Vault le valide → pas de clés longue durée stockées dans GitLab.

```yaml
deploy-aws:
  stage: deploy
  image:
    name: amazon/aws-cli:latest
    entrypoint: [""]
  id_tokens:
    GITLAB_OIDC_TOKEN:
      aud: https://gitlab.example.com        # audience attendue par le fournisseur
  script:
    - >
      export $(printf "AWS_ACCESS_KEY_ID=%s AWS_SECRET_ACCESS_KEY=%s AWS_SESSION_TOKEN=%s"
      $(aws sts assume-role-with-web-identity
      --role-arn "$AWS_ROLE_ARN"
      --role-session-name "gitlab-$CI_PROJECT_ID-$CI_PIPELINE_ID"
      --web-identity-token "$GITLAB_OIDC_TOKEN"
      --duration-seconds 3600
      --query 'Credentials.[AccessKeyId,SecretAccessKey,SessionToken]'
      --output text))
    - aws s3 sync dist/ s3://my-bucket/
```

**HashiCorp Vault** (keyword `secrets`) :

```yaml
job:
  id_tokens:
    VAULT_ID_TOKEN:
      aud: https://vault.example.com
  secrets:
    DATABASE_PASSWORD:
      vault: production/db/passwords/password@secrets
      token: $VAULT_ID_TOKEN
  script:
    - ./run-migration.sh            # DATABASE_PASSWORD est fournie (variable de type file par défaut)
```

### 12.9 Releases & versioning

```yaml
release:
  stage: deploy
  image: registry.gitlab.com/gitlab-org/release-cli:latest
  rules:
    - if: $CI_COMMIT_TAG =~ /^v\d+\.\d+\.\d+$/
  script:
    - echo "Création de la release $CI_COMMIT_TAG"
  release:
    tag_name: $CI_COMMIT_TAG
    name: "Release $CI_COMMIT_TAG"
    description: "Voir le changelog du projet."
```

- **Semantic Versioning** : `MAJOR.MINOR.PATCH`.
- **Conventional Commits** (`feat:`, `fix:`, `BREAKING CHANGE:`) + un outil comme *semantic-release* → versions, tags et changelogs **automatiques** depuis le pipeline.
- **Stratégies de branches** : *trunk-based* (branches courtes, feature flags, CI intense) vs *GitLab Flow* (`main` → `pre-production` → `production`, ou branches d'environnement/release). En entretien : préciser qu'on choisit selon la fréquence de release et la maturité des tests.

### 12.10 Pipelines planifiés (**Schedules**) & déclenchement externe

- *Build → Pipeline schedules* : cron (nightly build, scan de sécurité hebdomadaire, nettoyage). Détecter dans le YAML avec `$CI_PIPELINE_SOURCE == "schedule"`.
- **Trigger token** (`curl`, voir cheat sheet) ou **API** pour déclencher depuis un système externe ; `CI_PIPELINE_SOURCE` vaut `trigger` / `api`.
- Les **Pipeline schedules** peuvent porter des variables (ex. `SCAN_TYPE=full`).

```yaml
nightly-full-scan:
  script: ./scan.sh --full
  rules:
    - if: $CI_PIPELINE_SOURCE == "schedule" && $SCAN_TYPE == "full"
```

### 12.11 Dépendances entre jobs/pipelines : `needs` avancé

```yaml
test:
  needs:
    - job: build
      artifacts: true          # télécharger les artifacts
    - job: optional-lint
      optional: true           # ne bloque pas si ce job n'existe pas (rules)
    - pipeline: $PARENT_PIPELINE_ID
      job: shared-build        # artifacts d'un job du pipeline parent
```

### 12.12 Autres mots-clés utiles

| Mot-clé | Usage |
|---|---|
| `artifacts:expose_as` | Affiche un lien direct vers l'artifact dans la MR (ex. rapport HTML) |
| `artifacts:exclude` | Exclure des fichiers (`**/*.log`) d'un artifact |
| `cache:when` | Mettre en cache même en cas d'échec (`on_failure`, `always`) |
| `trigger:forward` | Transmettre variables/YAML variables au pipeline enfant |
| `timeout` (global ou job) | Limite de durée ; évite les jobs zombies |
| `retry:when` | Ne retenter **que** pour certaines causes (`runner_system_failure`, `stuck_or_timeout_failure`, `script_failure`) |
| `inherit:default` / `inherit:variables` | Choisir ce qu'un job hérite (utile pour jobs "spéciaux") |
| `pages` (job) | Publication de site statique (GitLab Pages) |
| `.pre` / `.post` (stages) | Jobs toujours premier / dernier, sans les déclarer dans `stages:` |
| `identity`, `hooks:pre_get_sources_script` | Cas avancés (identité cloud, script avant le clone) |

### 12.13 Auto DevOps, Compliance & gouvernance (à connaître en culture)

- **Auto DevOps** : pipeline pré-configuré (build, test, scan, déploiement Kubernetes) sans écrire de YAML. Bien pour démarrer, rarement suffisant en entreprise.
- **Pipeline execution policies / Compliance** *(Ultimate)* : imposer des jobs obligatoires (scan de sécurité, signature) à **tous** les projets d'un groupe.
- **Container Registry / Package Registry** intégrés : images Docker, Maven, npm, PyPI, générique. Penser aux **cleanup policies** (suppression automatique des anciens tags) pour limiter le stockage.

---

## 13. Pipeline complet — Java (Spring Boot + Maven + Docker)

### Structure attendue

```
my-app/
├── .gitlab-ci.yml
├── Dockerfile
├── pom.xml
└── src/
```

### `Dockerfile`

```dockerfile
FROM eclipse-temurin:21-jre-alpine
WORKDIR /app
COPY target/*.jar app.jar
RUN addgroup -S app && adduser -S app -G app
USER app
EXPOSE 8080
ENTRYPOINT ["java", "-jar", "app.jar"]
```

### `.gitlab-ci.yml`

```yaml
# ============================================================
# Spring Boot + Maven + MongoDB + Docker — GitLab CI/CD
# ============================================================

workflow:                                   # évite les pipelines en double (branche + MR)
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
    - if: $CI_COMMIT_BRANCH && $CI_OPEN_MERGE_REQUESTS
      when: never
    - if: $CI_COMMIT_BRANCH
    - if: $CI_COMMIT_TAG

default:
  image: maven:3.9-eclipse-temurin-21
  tags: [docker]
  interruptible: true
  retry:
    max: 1
    when: [runner_system_failure, stuck_or_timeout_failure]

stages:
  - build
  - test
  - package
  - deploy

variables:
  # Le repo Maven local DOIT être dans le projet pour être mis en cache
  MAVEN_OPTS: "-Dmaven.repo.local=$CI_PROJECT_DIR/.m2/repository"
  MAVEN_CLI_OPTS: "--batch-mode --errors --fail-at-end --show-version -Dstyle.color=always"
  IMAGE_TAG: "$CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA"
  GIT_DEPTH: "20"

cache:
  key:
    files:
      - pom.xml                              # invalidé quand les dépendances changent
  paths:
    - .m2/repository

# ---------- Templates de sécurité GitLab ----------
include:
  - template: Security/SAST.gitlab-ci.yml
  - template: Security/Secret-Detection.gitlab-ci.yml

# ---------- BUILD ----------
compile:
  stage: build
  script:
    - mvn $MAVEN_CLI_OPTS compile

# ---------- TEST ----------
unit-tests:
  stage: test
  script:
    - mvn $MAVEN_CLI_OPTS test jacoco:report
    # Affiche le % de coverage dans les logs → capturé par la regex "coverage"
    - grep -o '<tfoot>.*</tfoot>' target/site/jacoco/index.html || true
  coverage: '/Total.*?([0-9]{1,3})%/'
  artifacts:
    when: always
    expire_in: 1 week
    paths:
      - target/site/jacoco/
    reports:
      junit:
        - target/surefire-reports/TEST-*.xml

integration-tests:
  stage: test
  services:
    - name: mongo:7
      alias: mongo
  variables:
    SPRING_DATA_MONGODB_URI: "mongodb://mongo:27017/testdb"
  script:
    - mvn $MAVEN_CLI_OPTS verify -DskipUnitTests
  artifacts:
    when: always
    expire_in: 1 week
    reports:
      junit:
        - target/failsafe-reports/TEST-*.xml
  rules:
    - if: $CI_COMMIT_BRANCH || $CI_PIPELINE_SOURCE == "merge_request_event"

code-style:
  stage: test
  script:
    - mvn $MAVEN_CLI_OPTS checkstyle:check
  allow_failure: true                        # warning, ne bloque pas le pipeline

# ---------- PACKAGE ----------
package-jar:
  stage: package
  script:
    - mvn $MAVEN_CLI_OPTS package -DskipTests
  artifacts:
    paths:
      - target/*.jar
    expire_in: 1 week
  rules:
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH
    - if: $CI_COMMIT_TAG

docker-image:
  stage: package
  image: docker:27
  services:
    - docker:27-dind                         # Docker-in-Docker (runner "privileged" requis)
  variables:
    DOCKER_TLS_CERTDIR: "/certs"
  needs:
    - package-jar                            # récupère le .jar (artifact) et démarre dès qu'il est prêt
  before_script:
    - echo "$CI_REGISTRY_PASSWORD" | docker login -u "$CI_REGISTRY_USER" --password-stdin "$CI_REGISTRY"
  script:
    - docker build -t "$IMAGE_TAG" -t "$CI_REGISTRY_IMAGE:latest" .
    - docker push "$IMAGE_TAG"
    - docker push "$CI_REGISTRY_IMAGE:latest"
  rules:
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH
    - if: $CI_COMMIT_TAG

# ---------- DEPLOY ----------
.deploy-template:
  image: alpine:3.20
  needs: [docker-image]
  before_script:
    - apk add --no-cache openssh-client
    - chmod 400 "$SSH_PRIVATE_KEY"           # variable de type FILE
    - mkdir -p ~/.ssh && chmod 700 ~/.ssh
    - ssh-keyscan -H "$DEPLOY_HOST" >> ~/.ssh/known_hosts
  script:
    - |
      ssh -i "$SSH_PRIVATE_KEY" "$DEPLOY_USER@$DEPLOY_HOST" "
        echo '$REGISTRY_DEPLOY_TOKEN' | docker login $CI_REGISTRY -u '$REGISTRY_DEPLOY_USER' --password-stdin &&
        cd /opt/app &&
        IMAGE_TAG=$IMAGE_TAG docker compose pull &&
        IMAGE_TAG=$IMAGE_TAG docker compose up -d --remove-orphans
      "

deploy-staging:
  extends: .deploy-template
  stage: deploy
  environment:
    name: staging
    url: https://staging.example.com
  resource_group: staging
  rules:
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH

deploy-production:
  extends: .deploy-template
  stage: deploy
  environment:
    name: production
    url: https://example.com
  resource_group: production
  when: manual                               # Continuous DELIVERY : validation humaine
  allow_failure: false                       # bloque le pipeline tant que non validé
  rules:
    - if: $CI_COMMIT_TAG =~ /^v\d+\.\d+\.\d+$/
      when: manual
```

**Variables à créer dans *Settings → CI/CD → Variables*** : `DEPLOY_HOST`, `DEPLOY_USER`, `SSH_PRIVATE_KEY` (type *File*, Protected), `REGISTRY_DEPLOY_USER`, `REGISTRY_DEPLOY_TOKEN` (Masked, Protected). Utiliser le *Environment scope* pour avoir des valeurs différentes en staging/production.

**Points d'attention Java** :
- `-Dmaven.repo.local` sinon le cache `.m2` ne sert à rien (Maven écrit dans `~/.m2` qui n'est pas dans le projet).
- Surefire = tests unitaires (`*Test`), Failsafe = tests d'intégration (`*IT`, lancés par `verify`).
- Un **multi-stage Docker build** est une alternative (compiler dans le `Dockerfile`), mais perd le cache Maven du runner : garder le build dans le CI est généralement plus rapide.

---

## 14. Pipeline complet — Python

### `.gitlab-ci.yml`

```yaml
# ============================================================
# Python (FastAPI / Django / lib) — GitLab CI/CD
# ============================================================

workflow:
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
    - if: $CI_COMMIT_BRANCH && $CI_OPEN_MERGE_REQUESTS
      when: never
    - if: $CI_COMMIT_BRANCH
    - if: $CI_COMMIT_TAG

default:
  image: python:3.12-slim
  tags: [docker]
  interruptible: true

stages:
  - lint
  - test
  - build
  - deploy

variables:
  PIP_CACHE_DIR: "$CI_PROJECT_DIR/.cache/pip"
  PIP_DISABLE_PIP_VERSION_CHECK: "1"
  IMAGE_TAG: "$CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA"

cache:
  key:
    files:
      - requirements.txt
      - requirements-dev.txt
  paths:
    - .cache/pip

# Template caché : crée un venv et installe les dépendances
.python-setup:
  before_script:
    - python -V
    - pip install -r requirements.txt -r requirements-dev.txt

# ---------- LINT ----------
ruff:
  extends: .python-setup
  stage: lint
  script:
    - ruff check .
    - ruff format --check .

mypy:
  extends: .python-setup
  stage: lint
  script:
    - mypy app/
  allow_failure: true

# ---------- TEST ----------
pytest:
  extends: .python-setup
  stage: test
  parallel:
    matrix:
      - PYTHON_VERSION: ["3.11", "3.12", "3.13"]     # teste 3 versions en parallèle
  image: python:${PYTHON_VERSION}-slim
  services:
    - name: postgres:16
      alias: db
  variables:
    POSTGRES_DB: test
    POSTGRES_USER: test
    POSTGRES_PASSWORD: test
    DATABASE_URL: "postgresql://test:test@db:5432/test"
  script:
    - >
      pytest
      --junitxml=report.xml
      --cov=app
      --cov-report=term
      --cov-report=xml:coverage.xml
  coverage: '/(?i)total.*? (100(?:\.0+)?\%|[1-9]?\d(?:\.\d+)?\%)$/'
  artifacts:
    when: always
    expire_in: 1 week
    reports:
      junit: report.xml
      coverage_report:
        coverage_format: cobertura
        path: coverage.xml

security-audit:
  extends: .python-setup
  stage: test
  script:
    - pip-audit -r requirements.txt            # vulnérabilités des dépendances
    - bandit -r app/ -ll                       # analyse statique sécurité
  allow_failure: true

# ---------- BUILD ----------
docker-image:
  stage: build
  image: docker:27
  services:
    - docker:27-dind
  variables:
    DOCKER_TLS_CERTDIR: "/certs"
  before_script:
    - echo "$CI_REGISTRY_PASSWORD" | docker login -u "$CI_REGISTRY_USER" --password-stdin "$CI_REGISTRY"
  script:
    - docker build --pull -t "$IMAGE_TAG" -t "$CI_REGISTRY_IMAGE:latest" .
    - docker push "$IMAGE_TAG"
    - docker push "$CI_REGISTRY_IMAGE:latest"
  rules:
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH
    - if: $CI_COMMIT_TAG

# Publication d'une librairie sur le Package Registry GitLab (sur tag)
publish-package:
  stage: build
  script:
    - pip install build twine
    - python -m build
    - >
      TWINE_USERNAME=gitlab-ci-token
      TWINE_PASSWORD=$CI_JOB_TOKEN
      python -m twine upload
      --repository-url ${CI_API_V4_URL}/projects/${CI_PROJECT_ID}/packages/pypi
      dist/*
  rules:
    - if: $CI_COMMIT_TAG =~ /^v\d+\.\d+\.\d+$/

# ---------- DEPLOY ----------
deploy-staging:
  stage: deploy
  image: alpine:3.20
  needs: [docker-image]
  before_script:
    - apk add --no-cache openssh-client
    - chmod 400 "$SSH_PRIVATE_KEY"
    - mkdir -p ~/.ssh && chmod 700 ~/.ssh
    - ssh-keyscan -H "$DEPLOY_HOST" >> ~/.ssh/known_hosts
  script:
    - ssh -i "$SSH_PRIVATE_KEY" "$DEPLOY_USER@$DEPLOY_HOST" "docker pull $IMAGE_TAG && docker rm -f myapp || true && docker run -d --name myapp -p 8000:8000 --env-file /opt/myapp/.env $IMAGE_TAG"
  environment:
    name: staging
    url: https://staging.example.com
  resource_group: staging
  rules:
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH

deploy-production:
  extends: deploy-staging
  environment:
    name: production
    url: https://example.com
  resource_group: production
  when: manual
  rules:
    - if: $CI_COMMIT_TAG =~ /^v\d+\.\d+\.\d+$/
      when: manual
```

### `Dockerfile` (exemple)

```dockerfile
FROM python:3.12-slim
WORKDIR /app
ENV PYTHONDONTWRITEBYTECODE=1 PYTHONUNBUFFERED=1
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
COPY . .
RUN useradd -m app && chown -R app /app
USER app
CMD ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000"]
```

**Points d'attention Python** :
- `requirements-dev.txt` doit contenir `pytest`, `pytest-cov`, `ruff`, `mypy`, `pip-audit`, `bandit`.
- Avec `matrix`, chaque job a sa propre variable `PYTHON_VERSION` → l'`image:` l'utilise.
- Alternative moderne : `uv` (installation de dépendances beaucoup plus rapide) — cacher `~/.cache/uv`.

---

## 15. Pipeline complet — Node.js / Angular

### `.gitlab-ci.yml`

```yaml
# ============================================================
# Node.js / Angular — GitLab CI/CD
# ============================================================

workflow:
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
    - if: $CI_COMMIT_BRANCH && $CI_OPEN_MERGE_REQUESTS
      when: never
    - if: $CI_COMMIT_BRANCH
    - if: $CI_COMMIT_TAG

default:
  image: node:22-alpine
  tags: [docker]
  interruptible: true

stages:
  - quality
  - test
  - build
  - deploy

variables:
  npm_config_cache: "$CI_PROJECT_DIR/.npm"     # le cache npm DOIT être dans le projet
  CI: "true"

cache:
  key:
    files:
      - package-lock.json
  paths:
    - .npm/
  policy: pull-push

.node-setup:
  before_script:
    - node -v && npm -v
    - npm ci --prefer-offline --no-audit         # ci = installation reproductible depuis le lockfile

# ---------- QUALITY ----------
lint:
  extends: .node-setup
  stage: quality
  script:
    - npm run lint

audit:
  stage: quality
  script:
    - npm audit --audit-level=high
  allow_failure: true

# ---------- TEST ----------
unit-tests:
  extends: .node-setup
  stage: test
  parallel:
    matrix:
      - NODE_VERSION: ["20", "22"]
  image: node:${NODE_VERSION}-alpine
  script:
    - >
      npm test -- --ci --coverage
      --coverageReporters=text --coverageReporters=cobertura
      --reporters=default --reporters=jest-junit
  coverage: '/All files[^|]*\|[^|]*\s+([\d\.]+)/'
  artifacts:
    when: always
    expire_in: 1 week
    reports:
      junit: junit.xml
      coverage_report:
        coverage_format: cobertura
        path: coverage/cobertura-coverage.xml

# ---------- BUILD ----------
build:
  extends: .node-setup
  stage: build
  script:
    - npm run build -- --configuration production   # Angular ; pour Node pur : npm run build
  artifacts:
    paths:
      - dist/
    expire_in: 1 week
  rules:
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH
    - if: $CI_COMMIT_TAG
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"

# ---------- DEPLOY ----------
# Option A : GitLab Pages (SPA statique)
pages:
  stage: deploy
  needs: [build]
  script:
    - mkdir -p public
    - cp -r dist/*/browser/* public/ || cp -r dist/* public/   # selon la structure Angular
  artifacts:
    paths:
      - public
  environment:
    name: staging
  rules:
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH

# Option B : image Docker (nginx) pour la prod
docker-image:
  stage: deploy
  image: docker:27
  services:
    - docker:27-dind
  needs: [build]
  variables:
    DOCKER_TLS_CERTDIR: "/certs"
  before_script:
    - echo "$CI_REGISTRY_PASSWORD" | docker login -u "$CI_REGISTRY_USER" --password-stdin "$CI_REGISTRY"
  script:
    - docker build -t "$CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA" .
    - docker push "$CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA"
  rules:
    - if: $CI_COMMIT_TAG =~ /^v\d+\.\d+\.\d+$/

deploy-production:
  stage: deploy
  image: alpine:3.20
  needs: [docker-image]
  before_script:
    - apk add --no-cache openssh-client
    - chmod 400 "$SSH_PRIVATE_KEY"
    - mkdir -p ~/.ssh && chmod 700 ~/.ssh
    - ssh-keyscan -H "$DEPLOY_HOST" >> ~/.ssh/known_hosts
  script:
    - ssh -i "$SSH_PRIVATE_KEY" "$DEPLOY_USER@$DEPLOY_HOST" "docker pull $CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA && docker rm -f web || true && docker run -d --name web -p 80:80 $CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA"
  environment:
    name: production
    url: https://example.com
  resource_group: production
  when: manual
  rules:
    - if: $CI_COMMIT_TAG =~ /^v\d+\.\d+\.\d+$/
      when: manual
```

### `Dockerfile` (SPA servie par nginx)

```dockerfile
FROM nginx:1.27-alpine
COPY dist/my-app/browser/ /usr/share/nginx/html/
COPY nginx.conf /etc/nginx/conf.d/default.conf
EXPOSE 80
```

**Points d'attention Node/Angular** :
- `npm ci` (pas `npm install`) : exige un `package-lock.json`, supprime `node_modules`, **reproductible**.
- On cache `.npm/` (le cache de téléchargement), **pas** `node_modules` (lent à zipper/dézipper, fragile).
- **Angular + Karma** nécessite un navigateur : utiliser `ChromeHeadless` avec `--no-sandbox` et une image qui contient Chrome (ex. `cypress/browsers`, ou installer `chromium` via `apk add`). Avec Jest, pas de souci.
- Le job de déploiement GitLab Pages doit s'appeler **`pages`** et publier un dossier **`public`**.
- Fichier de redirection SPA (`try_files $uri /index.html;` dans nginx) pour le routing Angular.

---

## 16. Troubleshooting

### 16.1 Méthodologie (à raconter en entretien)

1. **Lire le log du job** — l'erreur est presque toujours **au-dessus** de la dernière ligne (`ERROR: Job failed: exit code 1`).
2. **Identifier la phase qui échoue** : validation YAML ? pull de l'image ? `before_script` ? `script` ? upload d'artifacts ?
3. **Reproduire en local** : `docker run -it --rm <image> sh`, puis exécuter les commandes du `script` à la main.
4. **Isoler** : ajouter `env | sort`, `pwd`, `ls -la`, `which <cmd>` dans le job (⚠️ attention aux secrets).
5. **Comparer** avec un pipeline qui marchait (diff du commit, version d'image, runner différent…).

### 16.2 Outils de debug

| Outil | Usage |
|---|---|
| **CI Lint / Pipeline editor** (*Build → Pipeline editor*) | Valide la syntaxe, affiche le YAML fusionné (`include`/`extends`) |
| **Full configuration** (onglet de l'éditeur) | Voir le YAML **final** après includes/extends |
| `set -x` dans le script | Affiche chaque commande exécutée |
| `env \| sort` | Liste les variables (⚠️ ne pas le faire dans un repo public) |
| `CI_DEBUG_TRACE: "true"` | Trace **très** verbeuse… ⚠️ **expose les secrets** dans les logs. À utiliser en dernier recours puis supprimer le log |
| `CI_DEBUG_SERVICES: "true"` | Debug des containers `services` |
| `after_script` + `artifacts: when: always` | Récupérer logs/rapports même en cas d'échec |
| **Retry job** / *Run pipeline* avec variables | Rejouer / paramétrer |
| `GIT_STRATEGY: none` | Ne pas cloner (job qui n'en a pas besoin) |
| `GIT_DEPTH: 0` | Clone complet (nécessaire pour `git describe`, versioning par tags) |
| `needs`/`dependencies` | Vérifier que les artifacts sont bien transmis |

### 16.3 Problèmes fréquents et solutions

| Symptôme | Cause probable | Solution |
|---|---|---|
| Pipeline **non créé** / "yaml invalid" | Erreur de syntaxe/indentation, stage inconnu, `workflow:rules` qui exclut tout | Pipeline editor → *Validate*. Vérifier que le `stage:` du job existe dans `stages:` |
| Job **"pending / stuck"** (reste en attente) | Aucun runner ne matche les `tags`, runner offline, branche non protégée sur runner *protected* | Vérifier *Settings → CI/CD → Runners* (statut, tags). Enlever/corriger `tags:` |
| `This job is stuck because the project doesn't have any runners online assigned to it` | Pas de runner actif | Activer les shared runners ou enregistrer un runner |
| **Job n'apparaît pas** dans le pipeline | Aucune `rule` ne matche | Revoir les `rules` (ajouter une règle par défaut, vérifier `CI_PIPELINE_SOURCE`) |
| **2 pipelines** pour un même push | Pipeline branche + pipeline MR | Ajouter `workflow:rules` (§6) |
| `command not found` (exit **127**) | Outil absent de l'image | Changer d'image ou l'installer (`apk add` / `apt-get install`) |
| `permission denied` (exit **126**) | Script non exécutable | `chmod +x script.sh` (et commit avec `git update-index --chmod=+x`) |
| Exit **137** | **OOM kill** (mémoire) ou job tué | Augmenter la mémoire du runner, limiter `-Xmx`, `NODE_OPTIONS=--max-old-space-size=…` |
| `Cannot connect to the Docker daemon at tcp://docker:2375` | DinD mal configuré | Runner `privileged = true`, `services: docker:27-dind`, bon `DOCKER_TLS_CERTDIR` / `DOCKER_HOST` |
| `docker: command not found` | Image sans client Docker | `image: docker:27` |
| `toomanyrequests` (Docker Hub rate limit) | Trop de pulls anonymes | Utiliser le **Dependency Proxy** : `image: ${CI_DEPENDENCY_PROXY_GROUP_IMAGE_PREFIX}/node:22-alpine` ou un miroir/registry interne |
| `Failed to pull image` / `unauthorized` | Registry privé sans credentials | Variable `DOCKER_AUTH_CONFIG` ou `docker login` |
| **Cache jamais restauré** | Chemin hors `$CI_PROJECT_DIR`, clé qui change à chaque fois, runners différents sans cache distribué | Rediriger l'outil dans le projet (`.m2`, `.npm`…), vérifier `key`, configurer S3/GCS cache |
| **Artifacts absents** dans le job suivant | `dependencies`/`needs` mal définis, `expire_in` dépassé, chemin faux, `artifacts:when` (échec) | Vérifier le nom du job source, que `paths` pointe un fichier **existant** (`ls` avant) |
| Variable **vide** | Variable *Protected* sur branche non protégée, mauvais *environment scope*, faute de frappe, variable *File* (contient un chemin) | Vérifier les flags de la variable et la branche |
| Variable *Masked* refusée | Valeur avec espaces/caractères spéciaux, < 8 caractères | Encoder en base64 |
| `$VAR` non expansée dans une `rule` | Mauvaise syntaxe | `if: $VAR == "x"` (pas de `"$VAR"`) |
| Le YAML casse sur `:` dans une commande | `echo "a: b"` interprété comme map YAML | Entourer toute la ligne de quotes ou utiliser `\|` (bloc littéral) |
| Une commande échoue mais le job est vert | Commande dans un pipe (`cmd1 \| cmd2`) | Ajouter `set -o pipefail` ; GitLab utilise `set -e` mais pas `pipefail` par défaut |
| Job échoue à cause de `exit code 1` d'un `grep` | `grep` sans match renvoie 1 | `grep ... \|\| true` |
| `fatal: detected dubious ownership` | Problème de permissions Git | `git config --global --add safe.directory $CI_PROJECT_DIR` |
| Job **timeout** | Job trop long / bloqué (prompt interactif) | `timeout:`, commandes non-interactives (`-y`, `--batch-mode`, `CI=true`) |
| Tests OK en local, KO en CI | Différence d'environnement (timezone, locale, versions, DB absente, ports) | Même image en local, vérifier `services`, fixer les versions |
| Pipeline lent | Pas de cache, pas de `needs`, clone complet, image lourde | Cache, DAG, `GIT_DEPTH`, images `alpine`/`slim`, `parallel`, `rules:changes` |
| Pas d'accès à un autre projet/package | `CI_JOB_TOKEN` n'a pas l'autorisation | *Settings → CI/CD → Job token permissions* → autoriser le projet |

### 16.4 Astuces pratiques

```yaml
# 1) Afficher des sections repliables dans les logs
script:
  - echo -e "\e[0Ksection_start:`date +%s`:install[collapsed=true]\r\e[0KInstall deps"
  - npm ci
  - echo -e "\e[0Ksection_end:`date +%s`:install\r\e[0K"

# 2) Debug temporaire : lister l'environnement sans secrets
script:
  - env | grep '^CI_' | sort

# 3) Rendre les erreurs de pipe visibles
script:
  - set -euo pipefail
  - ./build.sh | tee build.log

# 4) Garder les logs même en cas d'échec
artifacts:
  when: always
  paths: [build.log]

# 5) Forcer un job à tourner même quand un précédent a échoué (cleanup)
cleanup:
  stage: .post
  when: always
  script: ./cleanup.sh

# 6) Annuler les anciens pipelines d'une même branche
default:
  interruptible: true
# + activer "Auto-cancel redundant pipelines" dans les settings

# 7) Tester une règle compliquée : créer un job de debug
debug-rules:
  script: echo "source=$CI_PIPELINE_SOURCE branch=$CI_COMMIT_BRANCH tag=$CI_COMMIT_TAG"
```

---

## 17. Optimisation des pipelines

> **Règle d'or** : ne pas optimiser à l'aveugle. **Mesurer → identifier le goulot (critical path) → corriger → re-mesurer.** Réduire un job qui n'est pas sur le chemin critique ne change **pas** la durée totale du pipeline.

### 17.1 Mesurer

| Quoi | Où / comment |
|---|---|
| Durée et taux de succès des pipelines | *CI/CD Analytics* (menu *Analyze*, le chemin exact varie selon la version) |
| Durée par job, jobs les plus lents | Vue du pipeline, onglet **Jobs** (colonne *Duration*) ; **Pipeline > Needs** pour voir le DAG |
| Temps passé **en attente d'un runner** (`pending`) | Durée "queued" du job → si élevé : problème de **capacité des runners**, pas du code |
| Temps par commande | Préfixer avec `time` ou sections repliables chronométrées dans les logs |
| Jobs instables (flaky) | Rapports JUnit : tests qui échouent/réussissent aléatoirement |

**Décomposer la durée d'un job** :
```
durée job = attente runner + pull image + clone + restore cache + install deps
          + script (build/test) + upload cache + upload artifacts
```
Le `script` n'est souvent **pas** la plus grosse partie. Regardez les logs : l'overhead (pull, clone, cache, artifacts) cache souvent des gains faciles.

### 17.2 Tableau des leviers (gain vs effort)

| Levier | Gain typique | Effort |
|---|---|---|
| Cache des dépendances (`key:files`) | ⭐⭐⭐ | Faible |
| `needs` (DAG) pour casser l'attente de stage | ⭐⭐⭐ | Faible |
| `rules:changes` / ne pas lancer ce qui est inutile | ⭐⭐⭐ | Faible |
| `interruptible` + auto-cancel des pipelines obsolètes | ⭐⭐ | Très faible |
| `GIT_DEPTH` (shallow clone) | ⭐⭐ | Très faible |
| Images plus petites / pré-construites / Dependency Proxy | ⭐⭐ | Faible-moyen |
| `parallel` + split des tests | ⭐⭐⭐ | Moyen |
| Cache des layers Docker (BuildKit) | ⭐⭐⭐ | Moyen |
| Artifacts réduits (`dependencies: []`, `exclude`) | ⭐⭐ | Faible |
| Compression rapide (`FF_USE_FASTZIP`) | ⭐⭐ | Très faible |
| Runners mieux dimensionnés / autoscaling / cache distribué | ⭐⭐⭐ | Moyen-élevé |

### 17.3 Faire **moins** de travail

**a) Ne lancer que ce qui est nécessaire**

```yaml
# Backend seulement si le backend (ou le pom) change
backend-tests:
  script: mvn --batch-mode verify
  rules:
    - changes:
        paths:
          - backend/**/*
          - pom.xml
        compare_to: refs/heads/main
```

- Jobs lourds (E2E, scans complets, tests de charge) : uniquement sur `main`, sur tag, en **nightly** (`schedule`) ou manuels (`when: manual`).
- Ne pas lancer les jobs de déploiement dans les pipelines de MR.

**b) Annuler les pipelines obsolètes**

```yaml
default:
  interruptible: true            # lint, build, tests : annulables

workflow:
  auto_cancel:
    on_new_commit: interruptible # un nouveau push annule les anciens jobs interruptibles

deploy-production:
  interruptible: false           # ⚠️ un déploiement ne doit JAMAIS être interrompu
```

**c) Fail fast**

- Jobs peu coûteux (lint, compilation, tests unitaires) **avant** les jobs coûteux (intégration, E2E, build d'image).
- Option côté outils : `pytest -x`, `mvn --fail-fast` (compromis : on voit moins d'erreurs d'un coup, vs `--fail-at-end` qui les liste toutes).

### 17.4 Faire le travail **en parallèle**

**a) DAG avec `needs`** (voir §4.6) : les jobs démarrent dès que leurs dépendances sont prêtes.

**b) `parallel` + split des tests**

GitLab lance N copies du job, avec `CI_NODE_INDEX` (1..N) et `CI_NODE_TOTAL` (N). C'est à l'outil de test de répartir.

```yaml
jest:
  parallel: 4
  script:
    - npx jest --ci --shard=${CI_NODE_INDEX}/${CI_NODE_TOTAL}     # Jest ≥ 28

pytest:
  parallel: 4
  script:
    - pytest --splits $CI_NODE_TOTAL --group $CI_NODE_INDEX       # plugin pytest-split
```

- Pour les tests Java : paralléliser **dans** le job (`mvn -T 1C`, `forkCount`/`parallel` de Surefire) ou scinder par module.
- Pytest : `pytest -n auto` (plugin `pytest-xdist`) pour utiliser tous les CPU du runner.
- Attention : paralléliser consomme **plus de runners** ; si les runners sont saturés, le gain disparaît (le job reste `pending`).

### 17.5 Réduire l'overhead d'un job

**a) Clone**

```yaml
variables:
  GIT_DEPTH: "20"                    # shallow clone ; "0" = historique complet (si git describe, SonarQube, changelog)
  GIT_SUBMODULE_STRATEGY: none       # défaut ; n'activer `recursive` que si nécessaire

lint-only-docs:
  variables:
    GIT_STRATEGY: none               # job qui n'a pas besoin du code (ex. appel d'API)
```

**b) Cache (aller plus loin)**

```yaml
cache:
  key:
    files: [pom.xml]
    prefix: $CI_JOB_NAME             # un cache par job (évite les conflits d'écriture)
  paths: [.m2/repository]
  fallback_keys:                     # si la clé exacte n'existe pas, repartir du cache de main
    - main
  when: always                       # sauvegarder même si le job échoue (très utile pour ne pas re-télécharger)

# Jobs qui ne modifient pas le cache : lecture seule (pas d'upload → plus rapide)
lint:
  cache:
    key: { files: [pom.xml] }
    paths: [.m2/repository]
    policy: pull
```

Autres astuces :
- Ne **pas** cacher ce qui est plus rapide à regénérer qu'à restaurer (petits fichiers par milliers → souvent lent à zipper).
- Garder des **caches séparés** si deux jobs ont des dépendances différentes.
- **Distributed cache** (S3/GCS/MinIO) quand on a plusieurs runners : sinon chaque runner a son propre cache local et le hit-rate s'effondre.

```toml
# /etc/gitlab-runner/config.toml
[[runners]]
  [runners.cache]
    Type = "s3"
    Shared = true                    # cache partagé entre runners
    [runners.cache.s3]
      ServerAddress = "s3.amazonaws.com"
      BucketName = "my-ci-cache"
      BucketLocation = "eu-west-3"
```

**c) Compression plus rapide (cache et artifacts)**

```yaml
variables:
  FF_USE_FASTZIP: "true"                 # implémentation de zip plus rapide
  ARTIFACT_COMPRESSION_LEVEL: "fast"     # fastest | fast | default | slow | slowest
  CACHE_COMPRESSION_LEVEL: "fast"
  TRANSFER_METER_FREQUENCY: "5s"         # affiche la progression des transferts dans les logs
```

**d) Artifacts : ne transférer que le nécessaire**

```yaml
build:
  artifacts:
    paths: [target/*.jar]
    exclude: [target/**/*.log]
    expire_in: 3 days                    # moins de stockage, moins de bruit

lint:
  dependencies: []                       # ne télécharge AUCUN artifact (gain de temps)

deploy:
  needs:
    - job: docker-image
      artifacts: false                   # dépend du job mais n'en télécharge pas les artifacts
```

**e) Images Docker du CI**

- Utiliser des images **`alpine` / `slim`** (pull plus rapide) et **épinglées** (reproductible).
- Construire une **image CI dédiée** avec les outils déjà installés (`jq`, `aws-cli`, `kubectl`…) plutôt que de faire `apt-get install` à chaque job :

```dockerfile
# ci-image/Dockerfile (poussée dans le registry, mise à jour de temps en temps)
FROM maven:3.9-eclipse-temurin-21
RUN apt-get update \
 && apt-get install -y --no-install-recommends jq openssh-client \
 && rm -rf /var/lib/apt/lists/*
```

- **Dependency Proxy** : GitLab met en cache les images Docker Hub → plus rapide, et évite le **rate limit** de Docker Hub.

```yaml
default:
  image: ${CI_DEPENDENCY_PROXY_GROUP_IMAGE_PREFIX}/maven:3.9-eclipse-temurin-21
```

- Côté runner : `pull_policy = ["if-not-present"]` et pré-pull des images fréquentes sur les machines.

### 17.6 Optimiser le **build Docker**

1. **Ordonner le `Dockerfile` du moins au plus changeant** (les dépendances avant le code source) pour exploiter le cache de layers.
2. **`.dockerignore`** (`.git`, `node_modules`, `target`, `*.md`…) : contexte de build plus léger.
3. **Multi-stage build** : image finale minimale (JRE, `nginx`, `slim`), moins de données à pousser/tirer.
4. **Réutiliser le cache de layers** entre pipelines (par défaut, un runner éphémère/DinD repart de zéro !) :

```yaml
docker-image:
  stage: package
  image: docker:27
  services: [docker:27-dind]
  variables:
    DOCKER_TLS_CERTDIR: "/certs"
  before_script:
    - echo "$CI_REGISTRY_PASSWORD" | docker login -u "$CI_REGISTRY_USER" --password-stdin "$CI_REGISTRY"
    - docker buildx create --use --driver docker-container     # driver qui supporte l'export de cache
  script:
    - >
      docker buildx build
      --cache-from type=registry,ref=$CI_REGISTRY_IMAGE:buildcache
      --cache-to type=registry,ref=$CI_REGISTRY_IMAGE:buildcache,mode=max
      -t $IMAGE_TAG --push .
```

5. Construire **une seule fois** l'image et la promouvoir (staging → prod) au lieu de rebuilder.
6. Éviter DinD quand c'est possible (surcoût de démarrage + sécurité) : builders rootless ou runners Kubernetes dédiés.

### 17.7 Astuces par langage

| Langage | Astuces |
|---|---|
| **Java / Maven** | `-Dmaven.repo.local` + cache `.m2` ; `--no-transfer-progress` (logs plus légers) ; `-T 1C` (build parallèle par module) ; ne pas refaire `clean` inutilement ; `-DskipTests` dans le job `package` si les tests ont déjà tourné ; séparer unit (Surefire) et integration tests (Failsafe) en jobs parallèles. **Gradle** : cache `~/.gradle/caches` + `--build-cache` + `--parallel` |
| **Python** | **`uv`** (installation de dépendances ultra-rapide) ; cache `PIP_CACHE_DIR` ; `pip install --prefer-binary` (wheels) ; `pytest -n auto` ; image dédiée avec dépendances lourdes (numpy, pandas) pré-installées |
| **Node / Angular** | `npm ci --prefer-offline --no-audit --no-fund` ; cache `.npm` (pas `node_modules`) ; cache `.angular/cache` pour les builds Angular ; **Nx/Turborepo** (`nx affected`, cache de tâches) en monorepo ; `pnpm` (store partagé, plus rapide) ; `--shard` pour Jest |

### 17.8 Optimiser l'**infrastructure** (runners)

| Problème | Solution |
|---|---|
| Jobs longtemps `pending` | Plus de runners, `concurrent` plus élevé, **autoscaling** (Kubernetes, `docker-autoscaler`/cloud), runners dédiés par type de charge via `tags` |
| Cache peu efficace | **Distributed cache** (S3/GCS) |
| Jobs lents ou OOM (137) | Machines plus puissantes (CPU/RAM/SSD) pour les jobs lourds, requests/limits adaptés (Kubernetes) |
| Pulls d'images lents | Dependency Proxy, registry proche des runners, images pré-chargées |
| Coût élevé | Instances **spot/preemptible**, scale-to-zero, timeouts, nettoyage des artifacts/images |
| Voisins bruyants | Runners séparés (jobs de build vs jobs de déploiement sensibles) |

### 17.9 Fiabilité & hygiène (une pipeline rapide mais instable est inutile)

- **Flaky tests** : les identifier (rapports JUnit), les **corriger ou mettre en quarantaine**. `retry` ne doit pas masquer un test instable.
- `retry` **uniquement** pour les causes d'infrastructure :

```yaml
default:
  retry:
    max: 2
    when:
      - runner_system_failure
      - stuck_or_timeout_failure
      - api_failure
```

- `timeout` sur chaque job pour éviter les jobs bloqués qui monopolisent des runners.
- `expire_in` partout sur les artifacts ; **cleanup policies** pour le Container Registry ; supprimer les vieux pipelines/artifacts.
- Surveiller le **taux de succès** et la **durée p50/p90** du pipeline (pas seulement la moyenne).

### 17.10 Exemple "avant / après" (illustratif)

> Les durées ci-dessous sont des **ordres de grandeur d'illustration**, pas des mesures : l'effet réel dépend de votre projet et de vos runners.

| | Avant | Après |
|---|---|---|
| Clone | Historique complet | `GIT_DEPTH: 20` |
| Dépendances | Téléchargées à chaque job | Cache `key:files` + `policy: pull` sur les jobs en lecture |
| Ordonnancement | 4 stages strictement séquentiels | `needs` : le build d'image démarre dès que le `.jar` est prêt |
| Tests | 1 job séquentiel | `parallel: 4` (+ split) et unit/integration séparés |
| Docker | Build complet à chaque fois | BuildKit + `cache-from/to` registry |
| Pipelines de MR | Tout est lancé, tout le temps | `rules:changes`, jobs E2E en `main`/nightly |
| Pushs successifs | Anciens pipelines continuent | `interruptible` + `auto_cancel` |
| Résultat typique | Plusieurs dizaines de minutes | Souvent **divisé par 2 à 4**, avec un feedback bien plus rapide sur les MR |

Squelette "après" (Java) combinant les leviers :

```yaml
workflow:
  auto_cancel:
    on_new_commit: interruptible
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
    - if: $CI_COMMIT_BRANCH && $CI_OPEN_MERGE_REQUESTS
      when: never
    - if: $CI_COMMIT_BRANCH
    - if: $CI_COMMIT_TAG

default:
  image: ${CI_DEPENDENCY_PROXY_GROUP_IMAGE_PREFIX}/maven:3.9-eclipse-temurin-21
  interruptible: true

variables:
  GIT_DEPTH: "20"
  FF_USE_FASTZIP: "true"
  ARTIFACT_COMPRESSION_LEVEL: "fast"
  CACHE_COMPRESSION_LEVEL: "fast"
  MAVEN_OPTS: "-Dmaven.repo.local=$CI_PROJECT_DIR/.m2/repository"
  MAVEN_CLI_OPTS: "--batch-mode --no-transfer-progress --fail-at-end"

.maven-cache:
  cache:
    key: { files: [pom.xml], prefix: maven }
    paths: [.m2/repository]
    fallback_keys: [main]
    when: always

compile:                                # sert de "fail fast" : échoue en quelques secondes si ça ne compile pas
  extends: .maven-cache
  stage: build
  script: mvn $MAVEN_CLI_OPTS -T 1C compile

unit-tests:
  extends: .maven-cache
  stage: test
  needs: [compile]                      # démarre dès la fin de compile
  script: mvn $MAVEN_CLI_OPTS -T 1C test
  artifacts:
    when: always
    reports:
      junit: target/surefire-reports/TEST-*.xml

integration-tests:
  extends: .maven-cache
  stage: test
  needs: [compile]                      # en parallèle des unit-tests
  services:
    - name: mongo:7
      alias: mongo
  script: mvn $MAVEN_CLI_OPTS verify -DskipUnitTests
  artifacts:
    when: always
    reports:
      junit: target/failsafe-reports/TEST-*.xml

package-jar:
  extends: .maven-cache
  stage: package
  needs: [compile]                      # n'attend pas la fin des tests pour packager
  script: mvn $MAVEN_CLI_OPTS package -DskipTests
  artifacts:
    paths: [target/*.jar]
    expire_in: 3 days
  rules:
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH
    - if: $CI_COMMIT_TAG

docker-image:
  stage: package
  image: docker:27
  services: [docker:27-dind]
  needs: [package-jar, unit-tests, integration-tests]   # ne pousse l'image que si tout est vert
  variables:
    DOCKER_TLS_CERTDIR: "/certs"
  before_script:
    - echo "$CI_REGISTRY_PASSWORD" | docker login -u "$CI_REGISTRY_USER" --password-stdin "$CI_REGISTRY"
    - docker buildx create --use --driver docker-container
  script:
    - >
      docker buildx build
      --cache-from type=registry,ref=$CI_REGISTRY_IMAGE:buildcache
      --cache-to type=registry,ref=$CI_REGISTRY_IMAGE:buildcache,mode=max
      -t $CI_REGISTRY_IMAGE:$CI_COMMIT_SHORT_SHA --push .
  rules:
    - if: $CI_COMMIT_BRANCH == $CI_DEFAULT_BRANCH
    - if: $CI_COMMIT_TAG

deploy-production:
  stage: deploy
  needs: [docker-image]
  interruptible: false                  # jamais interrompu
  resource_group: production
  script: ./deploy.sh production
  environment: { name: production }
  when: manual
  rules:
    - if: $CI_COMMIT_TAG =~ /^v\d+\.\d+\.\d+$/
      when: manual
```

> Avec `needs`, le job `docker-image` ne démarre pas "après le stage test" mais **dès que ses 3 dépendances sont terminées** : on gagne le temps mort entre jobs, tout en gardant la garantie "rien n'est publié si un test échoue".

### 17.11 Checklist d'optimisation

- [ ] J'ai **mesuré** (durée totale, jobs sur le chemin critique, temps `pending`)
- [ ] Cache configuré avec `key:files`, chemins **dans** `$CI_PROJECT_DIR`, `policy: pull` si lecture seule
- [ ] `needs` utilisé pour le DAG
- [ ] `rules:changes` / règles par type de pipeline (MR vs `main` vs tag vs nightly)
- [ ] `interruptible: true` (sauf déploiements) + auto-cancel
- [ ] `GIT_DEPTH` réduit
- [ ] Images petites, épinglées, via Dependency Proxy ; image CI custom pour les outils
- [ ] Artifacts minimaux, `expire_in`, `dependencies: []` quand inutile
- [ ] Tests parallélisés (`parallel`, `-n auto`, `--shard`) et unit/integration séparés
- [ ] Build Docker : layers ordonnés, `.dockerignore`, multi-stage, cache BuildKit
- [ ] Runners : capacité suffisante, autoscaling, cache distribué
- [ ] Flaky tests traités, `retry` limité aux erreurs d'infra, `timeout` définis

---

## 18. Best practices

✅ **À faire**
- **Fail fast** : lint et tests rapides en premier, jobs lents ensuite.
- **Builder une fois, déployer partout** : même artefact/image de staging à prod (jamais re-builder pour la prod).
- Tagger les images avec `CI_COMMIT_SHORT_SHA` (traçabilité) **et** un tag sémantique sur release ; éviter de ne déployer que `latest`.
- Utiliser `rules` + `workflow:rules`, `needs` (DAG), `cache` avec `key:files`, `interruptible`.
- Épingler les versions des images et des outils.
- Garder des jobs **idempotents** et courts (petits jobs = meilleur parallélisme et logs lisibles).
- Pipeline as code : relu en code review comme le reste.
- `resource_group` sur les déploiements, `environment` pour la traçabilité et le rollback.
- Factoriser avec `extends`/`include`/Components pour toute l'organisation.
- Publier les rapports (`junit`, `coverage_report`, SAST) → visibles dans la MR.

❌ **À éviter**
- Secrets en clair dans le YAML / repo, ou `CI_DEBUG_TRACE` laissé actif.
- `only/except` (legacy), images `latest`.
- Docker-in-Docker sur runners partagés non isolés (privilégier `kubernetes` + build rootless, Buildah, ou Kaniko-like alternatives selon votre infra).
- Un seul gigantesque job qui fait tout.
- Cacher `node_modules` / `target/` entiers à la place du cache de téléchargement.
- Déployer en prod depuis n'importe quelle branche.

> **Avis candide** : le meilleur investissement rentable pour 90 % des pipelines lents est (1) un bon `cache`, (2) `needs` pour casser l'attente de stage, (3) des images plus petites et épinglées. Ne cherchez pas à optimiser autre chose avant d'avoir fait ça.

---

## 19. Questions d'entretien (avec réponses)

**1. C'est quoi un pipeline GitLab ?**
L'ensemble des jobs organisés en stages, défini dans `.gitlab-ci.yml`, déclenché par un événement (push, MR, tag, schedule, API). Les stages s'exécutent en séquence, les jobs d'un même stage en parallèle.

**2. Différence entre stage et job ?**
Un *stage* est une phase (build, test, deploy) ; un *job* est une unité de travail qui appartient à un stage et exécute un `script`.

**3. Qu'est-ce qu'un GitLab Runner et un executor ?**
Le runner est l'agent qui exécute les jobs (il *pull* le travail depuis GitLab). L'executor définit *comment* (docker, kubernetes, shell…). Le plus courant : `docker` → container éphémère par job.

**4. Cache vs artifacts ?**
Cache = accélérer les pipelines (dépendances), *best-effort*, partagé entre pipelines. Artifacts = résultats d'un job transmis aux jobs suivants/téléchargeables, garantis, stockés sur le serveur avec expiration.

**5. Comment accélérer un pipeline ?**
Cache avec `key:files`, `needs` (DAG), `parallel`/`matrix`, `rules:changes`, `GIT_DEPTH`, images légères, `interruptible`, runners dimensionnés/autoscalés, éviter de réinstaller des outils (image custom).

**6. Comment gérer les secrets ?**
Variables CI/CD **Masked + Protected** (ou type *File*), Vault/OIDC pour les clouds, jamais dans le repo. Environment scope par environnement. Protected branches/tags + protected environments pour la prod.

**7. `only/except` vs `rules` ?**
`rules` est plus puissant (`if`, `changes`, `exists`, `when`, variables), évalué dans l'ordre, première règle qui matche. `only/except` est legacy et à éviter.

**8. C'est quoi `needs` ?**
Déclare des dépendances explicites entre jobs → DAG. Le job démarre dès que ses dépendances sont finies, sans attendre tout le stage précédent.

**9. Comment éviter les pipelines dupliqués (branche + MR) ?**
`workflow:rules` : n'autoriser le pipeline de branche que s'il n'y a pas de MR ouverte (`$CI_OPEN_MERGE_REQUESTS`).

**10. Comment déployer en prod de manière sûre ?**
Build unique réutilisé de staging à prod, `when: manual` (Continuous Delivery), `resource_group`, protected environments avec approbations, déploiement déclenché sur tag, rollback possible via *Environments*, smoke tests post-déploiement.

**11. Comment réutiliser de la config CI ?**
`extends` + hidden jobs, `!reference`, `include` (local/project/remote/template), CI/CD Components versionnés avec `inputs`.

**12. Comment fonctionne Docker-in-Docker et quelles alternatives ?**
Job avec `docker:dind` en `service`, runner `privileged`. Risque sécurité. Alternatives : montage du socket Docker (encore plus risqué), Buildah/Podman rootless, BuildKit rootless, ou autres builders sans daemon selon l'infra.

**13. `before_script` vs `after_script` ?**
`before_script` est exécuté dans le même shell que `script` (variables partagées). `after_script` s'exécute dans un shell séparé, même après un échec — utile pour cleanup, mais ne peut pas changer le statut du job.

**14. Comment gérer un monorepo ?**
Pipelines parent-child (`trigger:include`) + `rules:changes` pour ne lancer que les parties modifiées ; `include` de CI par sous-dossier.

**15. Pipeline vert mais le déploiement est cassé : que faire ?**
Rollback via l'environnement/le tag précédent, puis investiguer : manque de smoke tests / healthcheck post-déploiement, écart staging/prod, variables par scope. Ajouter un job de vérification (`curl --fail` sur `/health`) et un déploiement progressif (canary).

**16. Comment débugger un job qui échoue ?**
Lire le log depuis le haut, identifier la phase, reproduire en local avec la même image, ajouter `set -x`/`env`, valider le YAML dans le Pipeline editor, vérifier variables/protections/tags du runner, comparer avec le dernier pipeline vert.

**17. Quelle est la différence entre CI/CD Variables et `variables:` du YAML ?**
Le YAML est versionné et visible → valeurs non secrètes. Les variables UI sont chiffrées, masquables/protégeables → secrets. En cas de conflit, celles de l'UI (projet) ont la priorité sur celles du YAML.

**18. Comment lancer des tests avec une base de données ?**
`services:` (mongo, postgres…) avec alias, variables de connexion pointant vers l'alias (pas `localhost`).

**19. Comment publier un artefact versionné ?**
Pipeline sur tag (`rules: if: $CI_COMMIT_TAG`), push d'image Docker taguée, Package Registry (Maven/npm/PyPI), et `release:` pour créer une Release GitLab avec changelog.

**20. Que sont les Protected branches/environments ?**
Protected branches : contrôle qui peut pusher/merger, et accès aux variables *Protected*. Protected environments : restreint qui peut déployer (et peut exiger des approbations) sur un environnement comme `production`.

**21. Votre pipeline met 40 minutes. Par où commencez-vous ?**
Je **mesure d'abord** : durée par job, chemin critique (graphe `needs`), temps `pending` (problème de runners ou de code ?), overhead (pull d'image, clone, cache, artifacts). Ensuite les gains rapides : cache avec `key:files`, `needs` pour casser l'attente de stage, `rules:changes` pour ne pas lancer l'inutile, `GIT_DEPTH`, `interruptible`. Puis le plus structurant : `parallel` + split des tests, cache de layers Docker, images plus légères, runners mieux dimensionnés. Et je re-mesure après chaque changement.

**22. Pourquoi un job `pending` pendant longtemps, et comment réagir ?**
Aucun runner disponible ou compatible : tags qui ne matchent pas, runner offline, runner *protected* sur branche non protégée, ou capacité insuffisante (`concurrent` atteint). Solutions : corriger les `tags`, ajouter/auto-scaler les runners, séparer les charges par type de runner.

**23. Cache distribué : pourquoi et comment ?**
Avec plusieurs runners, chacun a son cache **local** → taux de hit faible. Un cache distribué (S3/GCS/MinIO, `[runners.cache]` + `Shared = true`) permet à tous les runners de partager le même cache.

**24. Comment passer une valeur calculée d'un job à un autre ?**
`artifacts:reports:dotenv` : le job écrit `VAR=valeur` dans un `.env`, déclaré dans `reports: dotenv`. Les jobs qui en dépendent (`needs`/`dependencies`) reçoivent `VAR` comme variable.

**25. Que sont les Merged Results pipelines et les Merge Trains ?**
Merged results : le pipeline teste le résultat **du merge** (source + cible) plutôt que la branche seule. Merge train (Premium) : file d'attente de MR, chacune testée avec celles qui la précèdent, pour que `main` reste toujours verte même avec beaucoup de MR concurrentes.

**26. Comment éviter les secrets statiques pour déployer dans le cloud ?**
OIDC : `id_tokens` fournit un JWT éphémère signé par GitLab, validé par le cloud provider (AWS `assume-role-with-web-identity`, GCP Workload Identity, Azure) ou par Vault (`secrets:`). Aucune clé longue durée dans GitLab ; les droits sont limités par claims (projet, branche, environnement).

**27. Comment optimiser le build d'une image Docker en CI ?**
`Dockerfile` ordonné (dépendances avant code), `.dockerignore`, multi-stage, cache de layers réutilisé entre pipelines (BuildKit `--cache-from/--cache-to type=registry`), une seule construction promue de staging à prod, et éviter DinD quand c'est possible.

**28. Faut-il utiliser `retry` pour fiabiliser les pipelines ?**
Seulement pour les **erreurs d'infrastructure** (`runner_system_failure`, `stuck_or_timeout_failure`, `api_failure`). Utiliser `retry` pour des tests flaky masque le problème : il faut les corriger ou les mettre en quarantaine.

**29. Comment générer dynamiquement des jobs ?**
Un job génère un fichier YAML (artifact), puis un job `trigger:include:artifact` lance un **pipeline enfant dynamique**. Pratique en monorepo ou pour des matrices complexes.

**30. Quelle différence entre CI/CD Template, Component et `include` classique ?**
`include` charge du YAML brut (local, project, remote). Les *templates* officiels GitLab sont des YAML préconstruits. Les *Components* sont des briques **versionnées et paramétrables** (`spec:inputs`), publiables dans le CI/CD Catalog — la méthode recommandée pour partager du CI entre équipes.

---

## 20. Cheat sheet

### Squelette de départ

```yaml
workflow:
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
    - if: $CI_COMMIT_BRANCH && $CI_OPEN_MERGE_REQUESTS
      when: never
    - if: $CI_COMMIT_BRANCH
    - if: $CI_COMMIT_TAG

default:
  image: <image épinglée>
  interruptible: true

stages: [build, test, package, deploy]

variables: {}

cache:
  key: { files: [<lockfile>] }
  paths: [<dossier de cache>]

build:   { stage: build,   script: [...] }
test:    { stage: test,    script: [...] , artifacts: { reports: { junit: ... } } }
package: { stage: package, script: [...] , artifacts: { paths: [...], expire_in: 1 week } }
deploy:
  stage: deploy
  script: [...]
  environment: { name: production }
  resource_group: production
  when: manual
```

### Commandes/outils utiles

```bash
# Valider le CI depuis la CLI (glab)
glab ci lint

# Voir l'état du pipeline courant
glab ci status

# Déclencher un pipeline via API (trigger token)
curl --request POST \
  --form token=<TRIGGER_TOKEN> \
  --form ref=main \
  "https://gitlab.example.com/api/v4/projects/<ID>/trigger/pipeline"

# Logs / état du runner
sudo gitlab-runner status
sudo gitlab-runner verify
sudo journalctl -u gitlab-runner -f
```

### Mémo ultra-court

| Je veux… | J'utilise… |
|---|---|
| Choisir quand un job tourne | `rules` |
| Éviter doublons branche/MR | `workflow:rules` |
| Accélérer les installs | `cache` (+ `key:files`) |
| Passer des fichiers au job suivant | `artifacts` (+ `needs`) |
| Démarrer un job sans attendre le stage | `needs` |
| Base de données pour les tests | `services` |
| Tester plusieurs versions | `parallel:matrix` |
| Bouton de validation | `when: manual` |
| Empêcher 2 déploiements simultanés | `resource_group` |
| Suivre les déploiements/rollback | `environment` |
| Éviter la duplication | `extends`, `include`, `!reference` |
| Découper un monorepo | `trigger:include` + `rules:changes` |
| Secret | Variable **Masked + Protected** (ou *File*) |
| Débugger | Pipeline editor, `set -x`, `env \| sort`, `after_script` |
| Passer une valeur entre jobs | `artifacts:reports:dotenv` |
| Annuler les pipelines obsolètes | `interruptible` + `workflow:auto_cancel` |
| Répartir les tests | `parallel` + `CI_NODE_INDEX` / `CI_NODE_TOTAL` |
| Réduire le temps de clone | `GIT_DEPTH` |
| Cache partagé entre runners | Distributed cache (S3/GCS) |
| Cache de layers Docker | BuildKit `--cache-from/--cache-to` |
| Pas de secret statique dans le cloud | `id_tokens` (OIDC) / `secrets:` Vault |
| Template partagé versionné | CI/CD Component + `spec:inputs` |
| Publier une release | `release:` sur tag |

---

*Fin du cours — bon courage pour l'entretien !* 🚀
