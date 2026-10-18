# 🐳 Cours Docker — Préparation Entretien Développeur Java / Spring Boot

> **Format :** Français · Termes techniques en anglais  
> **Objectif :** Maîtriser Docker de A à Z pour un poste backend Java/Spring Boot  
> **Niveau :** Débutant → Avancé

---

## Table des matières

1. [Introduction à Docker](#1-introduction-à-docker)
2. [Architecture Docker](#2-architecture-docker)
3. [Images Docker](#3-images-docker)
4. [Dockerfile](#4-dockerfile)
5. [Containers](#5-containers)
6. [Volumes](#6-volumes)
7. [Networking](#7-networking)
8. [Docker Compose](#8-docker-compose)
9. [Dockeriser une application Spring Boot](#9-dockeriser-une-application-spring-boot)
10. [Multi-stage Build](#10-multi-stage-build)
11. [Registry & Docker Hub](#11-registry--docker-hub)
12. [Variables d'environnement & Configuration](#12-variables-denvironnement--configuration)
13. [Bonnes pratiques](#13-bonnes-pratiques)
14. [Docker en CI/CD](#14-docker-en-cicd)
15. [Questions/Réponses d'entretien](#15-questionsréponses-dentretien)

---

## 1. Introduction à Docker

### Qu'est-ce que Docker ?

Docker est une plateforme de **containerisation** open-source qui permet d'empaqueter une application et toutes ses dépendances dans une unité isolée appelée **container**. L'objectif principal est de garantir que l'application fonctionne de manière identique dans tous les environnements : développement, test, staging, production.

Slogan clé : **"Build once, run anywhere"**

### Problème résolu par Docker

Avant Docker, on rencontrait fréquemment le problème du *"it works on my machine"* :

| Problème                          | Solution Docker                          |
|----------------------------------|------------------------------------------|
| Dépendances différentes par env  | Tout est empaqueté dans l'image          |
| Conflits de versions JDK/lib     | Chaque container a son propre runtime    |
| Déploiement complexe             | `docker run` suffit                      |
| Isolation des services           | Chaque service dans son propre container |

### Différence entre Container et Machine Virtuelle (VM)

| Critère              | VM                                  | Container Docker                  |
|---------------------|-------------------------------------|-----------------------------------|
| OS                  | OS complet (kernel inclus)          | Partage le kernel de l'hôte       |
| Démarrage           | Plusieurs minutes                   | Quelques secondes                 |
| Taille              | Plusieurs Go                        | Quelques Mo à centaines de Mo     |
| Isolation           | Totale (hyperviseur)                | Processus isolé (namespaces)      |
| Performance         | Overhead élevé                      | Quasi-native                      |
| Portabilité         | Moins portable                      | Très portable                     |

### Concepts fondamentaux

- **Image** : template immuable pour créer des containers (comme une classe en Java)
- **Container** : instance en cours d'exécution d'une image (comme un objet instancié)
- **Dockerfile** : script de construction d'une image
- **Registry** : dépôt d'images (Docker Hub, GitLab Registry, AWS ECR...)
- **Volume** : mécanisme de persistance de données
- **Network** : réseau virtuel pour la communication entre containers

---

## 2. Architecture Docker

### Composants principaux

```
┌──────────────────────────────────────────────────────────┐
│                     Docker Client (CLI)                  │
│         docker build / docker run / docker pull          │
└─────────────────────┬────────────────────────────────────┘
                      │ REST API
                      ▼
┌──────────────────────────────────────────────────────────┐
│                    Docker Daemon (dockerd)                │
│         Gère images, containers, volumes, networks       │
└──────┬──────────────────────────────────┬────────────────┘
       │                                  │
       ▼                                  ▼
┌─────────────┐                  ┌────────────────┐
│  containerd │                  │   Docker Hub   │
│  (runtime)  │                  │   (Registry)   │
└─────────────┘                  └────────────────┘
```

### Docker Engine

Le **Docker Engine** est composé de trois éléments :

1. **Docker Daemon (`dockerd`)** : processus serveur qui tourne en arrière-plan, reçoit les commandes via l'API REST et gère tous les objets Docker.
2. **Docker Client (`docker`)** : interface en ligne de commande (CLI) qui envoie les commandes au daemon.
3. **Docker API** : interface REST entre le client et le daemon.

### Isolation via les namespaces Linux

Docker utilise des fonctionnalités du kernel Linux pour isoler les containers :

| Namespace  | Isolation de...                          |
|-----------|------------------------------------------|
| `pid`     | Processus (chaque container a son propre arbre de processus) |
| `net`     | Interfaces réseau, routing tables        |
| `mnt`     | Système de fichiers (mount points)       |
| `uts`     | Hostname et domaine                      |
| `ipc`     | Inter-process communication              |
| `user`    | UID/GID (user namespace)                 |

### Control Groups (cgroups)

Les **cgroups** permettent de limiter et monitorer les ressources consommées par un container :
- CPU : `--cpus="1.5"` → limite à 1.5 CPU
- Mémoire : `--memory="512m"` → limite à 512 Mo de RAM
- Disk I/O, réseau...

### Union File System (overlay2)

Docker utilise un **layered file system** (système de fichiers en couches). Chaque instruction dans un `Dockerfile` crée une nouvelle couche en lecture seule. Quand un container démarre, une couche supplémentaire en écriture est ajoutée au-dessus.

```
┌─────────────────────────────────┐
│  Container Layer (read/write)   │  ← modifications runtime
├─────────────────────────────────┤
│  Layer 4: COPY app.jar          │  ← instruction COPY
├─────────────────────────────────┤
│  Layer 3: RUN mvn package       │  ← instruction RUN
├─────────────────────────────────┤
│  Layer 2: WORKDIR /app          │  ← instruction WORKDIR
├─────────────────────────────────┤
│  Layer 1: FROM eclipse-temurin  │  ← image de base
└─────────────────────────────────┘
```

**Avantage clé** : les couches sont **partagées et cachées** entre images qui ont la même base, ce qui économise de l'espace disque et du temps de téléchargement.

---

## 3. Images Docker

### Qu'est-ce qu'une image ?

Une **image** Docker est un template immuable en lecture seule, composé de couches empilées. Elle contient tout le nécessaire pour exécuter une application : OS de base, runtime (JDK), code applicatif, dépendances, configuration.

### Commandes de gestion des images

```bash
# Lister les images locales
docker images
docker image ls

# Télécharger une image depuis le registry
docker pull eclipse-temurin:21-jre

# Inspecter les métadonnées d'une image
docker inspect eclipse-temurin:21-jre

# Afficher les couches d'une image
docker history eclipse-temurin:21-jre

# Supprimer une image
docker rmi mon-image:tag
docker image rm mon-image:tag

# Supprimer toutes les images inutilisées
docker image prune -a

# Tagger une image
docker tag mon-app:latest mon-registry/mon-app:v1.0.0

# Chercher une image sur Docker Hub
docker search openjdk
```

### Nommage et tags

Le format complet d'une référence d'image est :

```
[registry/][namespace/]nom[:tag][@digest]
```

Exemples :
- `ubuntu` → `docker.io/library/ubuntu:latest`
- `eclipse-temurin:21-jre-jammy` → image officielle JRE 21 sur Ubuntu Jammy
- `myregistry.company.com/backend/my-app:v2.1.0`

### Images de base recommandées pour Spring Boot

| Image                              | Description                                    |
|-----------------------------------|------------------------------------------------|
| `eclipse-temurin:21-jre`          | JRE 21 (Adoptium), recommandée en production   |
| `eclipse-temurin:21-jdk`          | JDK complet (pour le build)                    |
| `eclipse-temurin:21-jre-alpine`   | Très légère basée Alpine Linux (~50 Mo)        |
| `amazoncorretto:21`               | JDK Amazon Corretto 21                         |
| `gcr.io/distroless/java21`        | Image ultra-minimaliste (pas de shell)         |

---

## 4. Dockerfile

### Structure d'un Dockerfile

Un **Dockerfile** est un fichier texte contenant une série d'instructions pour construire une image Docker.

```dockerfile
# Commentaire
INSTRUCTION arguments
```

### Instructions essentielles

#### `FROM` — Image de base

```dockerfile
FROM eclipse-temurin:21-jre
# ou avec alias pour multi-stage
FROM eclipse-temurin:21-jdk AS builder
```

Toujours la première instruction. Définit l'image de base à partir de laquelle on construit.

#### `WORKDIR` — Répertoire de travail

```dockerfile
WORKDIR /app
```

Crée et positionne le répertoire courant pour les instructions suivantes (`RUN`, `COPY`, `CMD`...). Équivalent à un `cd` + `mkdir -p`.

#### `COPY` — Copier des fichiers

```dockerfile
# Copier un fichier spécifique
COPY target/my-app.jar app.jar

# Copier plusieurs fichiers
COPY src/ ./src/
COPY pom.xml .

# Avec .dockerignore, on évite de copier les fichiers inutiles
```

#### `ADD` — Comme COPY mais avec plus de fonctionnalités

```dockerfile
# ADD peut décompresser des archives et télécharger des URLs
ADD https://example.com/file.tar.gz /app/
```

> **Bonne pratique** : préférer `COPY` à `ADD` sauf besoin spécifique (décompression automatique).

#### `RUN` — Exécuter des commandes lors du build

```dockerfile
# Chaque RUN crée une nouvelle couche
RUN apt-get update && apt-get install -y curl \
    && rm -rf /var/lib/apt/lists/*

# Bonne pratique : chaîner les commandes avec && pour minimiser les couches
```

#### `ENV` — Variables d'environnement

```dockerfile
ENV JAVA_OPTS="-Xmx512m -Xms256m"
ENV SPRING_PROFILES_ACTIVE=production
ENV APP_PORT=8080
```

#### `ARG` — Arguments de build (build-time only)

```dockerfile
ARG JAR_FILE=target/*.jar
ARG BUILD_VERSION=latest

COPY ${JAR_FILE} app.jar
LABEL version=${BUILD_VERSION}
```

Différence avec `ENV` : `ARG` n'est disponible que pendant le `build`, pas à l'exécution du container.

#### `EXPOSE` — Documenter le port

```dockerfile
EXPOSE 8080
EXPOSE 8443
```

> Important : `EXPOSE` est **documentaire seulement**. Il ne publie pas réellement le port. C'est le `-p` lors du `docker run` qui fait la liaison.

#### `VOLUME` — Déclarer des volumes

```dockerfile
VOLUME /app/logs
VOLUME /app/data
```

Déclare un point de montage pour la persistance des données.

#### `CMD` — Commande par défaut

```dockerfile
# Forme exec (recommandée) - pas de shell intermédiaire
CMD ["java", "-jar", "app.jar"]

# Forme shell
CMD java -jar app.jar
```

#### `ENTRYPOINT` — Point d'entrée fixe

```dockerfile
ENTRYPOINT ["java", "-jar", "app.jar"]

# Avec CMD pour des arguments par défaut surchargeables
ENTRYPOINT ["java"]
CMD ["-jar", "app.jar"]
```

**Différence `CMD` vs `ENTRYPOINT`** :

| Aspect         | CMD                                      | ENTRYPOINT                              |
|---------------|------------------------------------------|-----------------------------------------|
| Surcharge      | Facilement surchargeable (`docker run`)  | Difficilement surchargeable (`--entrypoint`) |
| Usage          | Commande par défaut                      | Point d'entrée fixe                     |
| Combinaison    | Fournit les arguments à ENTRYPOINT       | Définit la commande principale          |

#### `HEALTHCHECK` — Vérification de santé

```dockerfile
HEALTHCHECK --interval=30s --timeout=10s --start-period=60s --retries=3 \
  CMD curl -f http://localhost:8080/actuator/health || exit 1
```

#### `LABEL` — Métadonnées

```dockerfile
LABEL maintainer="anouar@company.com"
LABEL version="1.0.0"
LABEL description="Spring Boot REST API"
```

#### `USER` — Utilisateur d'exécution

```dockerfile
# Créer un utilisateur non-root (sécurité)
RUN addgroup --system appgroup && adduser --system --group appgroup appuser
USER appuser
```

### Exemple complet — Dockerfile Spring Boot simple

```dockerfile
FROM eclipse-temurin:21-jre

# Métadonnées
LABEL maintainer="anouar@company.com"
LABEL version="1.0.0"

# Répertoire de travail
WORKDIR /app

# Copier le JAR
COPY target/my-spring-app.jar app.jar

# Port exposé
EXPOSE 8080

# Healthcheck via Spring Actuator
HEALTHCHECK --interval=30s --timeout=10s --start-period=90s --retries=3 \
  CMD curl -f http://localhost:8080/actuator/health || exit 1

# Commande de démarrage
ENTRYPOINT ["java", "-jar", "app.jar"]
```

### .dockerignore — Exclure des fichiers du contexte de build

```
# .dockerignore
target/
*.md
.git/
.idea/
*.iml
.mvn/
mvnw
mvnw.cmd
docker-compose*.yml
```

---

## 5. Containers

### Cycle de vie d'un container

```
created → running → paused → running → stopped → removed
              ↑                              ↓
              └──────────── restart ─────────┘
```

### Commandes essentielles

#### Créer et démarrer un container

```bash
# Démarrage simple
docker run eclipse-temurin:21 java -version

# Options courantes
docker run \
  -d \                          # Detached mode (arrière-plan)
  -p 8080:8080 \                # Port mapping host:container
  --name my-spring-app \        # Nom du container
  -e SPRING_PROFILES_ACTIVE=dev \ # Variable d'environnement
  -v /host/data:/app/data \     # Volume mount
  --memory="512m" \             # Limite mémoire
  --cpus="1.0" \                # Limite CPU
  --network my-network \        # Réseau Docker
  --restart=unless-stopped \    # Politique de redémarrage
  my-spring-app:latest
```

#### Gérer les containers

```bash
# Lister les containers en cours
docker ps

# Lister tous les containers (incluant stoppés)
docker ps -a

# Arrêter un container (signal SIGTERM puis SIGKILL après timeout)
docker stop my-spring-app

# Démarrer un container stoppé
docker start my-spring-app

# Redémarrer
docker restart my-spring-app

# Supprimer un container stoppé
docker rm my-spring-app

# Forcer la suppression (même en cours d'exécution)
docker rm -f my-spring-app

# Supprimer tous les containers stoppés
docker container prune
```

#### Interagir avec un container

```bash
# Exécuter une commande dans un container en cours
docker exec -it my-spring-app bash
docker exec -it my-spring-app sh   # Pour Alpine Linux

# Voir les logs
docker logs my-spring-app
docker logs -f my-spring-app       # Follow (en temps réel)
docker logs --tail=100 my-spring-app  # 100 dernières lignes

# Inspecter les détails d'un container
docker inspect my-spring-app

# Statistiques de ressources en temps réel
docker stats my-spring-app
docker stats                        # Tous les containers

# Copier des fichiers depuis/vers un container
docker cp my-spring-app:/app/logs/app.log ./app.log
docker cp ./config.yml my-spring-app:/app/config/
```

### Politiques de redémarrage (`--restart`)

| Politique              | Comportement                                             |
|-----------------------|----------------------------------------------------------|
| `no` (défaut)         | Ne redémarre jamais                                      |
| `on-failure[:N]`      | Redémarre si exit code ≠ 0, maximum N fois               |
| `always`              | Redémarre toujours, même au démarrage du daemon          |
| `unless-stopped`      | Toujours, sauf si manuellement stoppé                    |

---

## 6. Volumes

### Pourquoi les volumes ?

Les containers sont **éphémères** : toute donnée écrite dans le container est perdue quand il est supprimé. Les **volumes** permettent de **persister** les données indépendamment du cycle de vie du container.

### Les trois types de montage

#### 1. Volumes gérés par Docker (recommandé en production)

```bash
# Créer un volume
docker volume create pgdata

# Utiliser un volume
docker run -v pgdata:/var/lib/postgresql/data postgres:15

# Lister les volumes
docker volume ls

# Inspecter un volume
docker volume inspect pgdata

# Supprimer un volume
docker volume rm pgdata

# Supprimer les volumes inutilisés
docker volume prune
```

Les volumes sont stockés dans `/var/lib/docker/volumes/` sur l'hôte.

#### 2. Bind Mounts — Monter un répertoire de l'hôte

```bash
# Monter le répertoire courant dans le container
docker run -v $(pwd)/config:/app/config my-spring-app

# Ou avec la syntaxe --mount (plus explicite)
docker run --mount type=bind,source=$(pwd)/config,target=/app/config my-spring-app
```

Usage typique : développement local (hot reload), configuration externe.

#### 3. tmpfs Mounts — Stockage en mémoire

```bash
docker run --tmpfs /tmp my-spring-app
# Ou
docker run --mount type=tmpfs,destination=/tmp my-spring-app
```

Pour des données temporaires sensibles (secrets, sessions...).

### Tableau comparatif

| Type         | Géré par     | Persistance  | Performance | Usage                          |
|-------------|-------------|--------------|-------------|--------------------------------|
| Volume      | Docker      | Oui          | Excellente  | Production, bases de données   |
| Bind Mount  | Hôte        | Oui          | Bonne       | Développement, configs         |
| tmpfs       | Mémoire RAM | Non          | Très rapide | Données temporaires sensibles  |

### Volumes avec Spring Boot

```yaml
# docker-compose.yml — persister les logs et données H2
services:
  app:
    image: my-spring-app:latest
    volumes:
      - app-logs:/app/logs        # Logs persistants
      - app-data:/app/data        # Données applicatives
      - ./config:/app/config:ro   # Config externe (read-only)

volumes:
  app-logs:
  app-data:
```

---

## 7. Networking

### Pourquoi le networking Docker ?

Par défaut, les containers sont isolés. Le **networking Docker** permet la communication entre containers, et entre containers et l'extérieur.

### Types de networks

#### 1. `bridge` (défaut)

Réseau privé virtuel sur l'hôte. Les containers peuvent communiquer entre eux via leur nom (avec un network bridge personnalisé) ou leur IP.

```bash
# Créer un network bridge personnalisé
docker network create --driver bridge my-network

# Connecter des containers au même network
docker run --network my-network --name app my-spring-app
docker run --network my-network --name db postgres:15

# Dans app, on peut pinger db par son nom
# JDBC URL : jdbc:postgresql://db:5432/mydb
```

#### 2. `host`

Le container partage directement le network de l'hôte. Pas d'isolation réseau.

```bash
docker run --network host my-spring-app
# Le port 8080 du container = port 8080 de l'hôte directement
```

#### 3. `none`

Aucune interface réseau. Container complètement isolé.

```bash
docker run --network none my-batch-job
```

#### 4. `overlay`

Pour les clusters Docker Swarm ou Kubernetes. Permet la communication entre containers sur différents hôtes physiques.

### Commandes réseau

```bash
# Lister les networks
docker network ls

# Inspecter un network
docker network inspect my-network

# Connecter un container à un network (à chaud)
docker network connect my-network my-container

# Déconnecter
docker network disconnect my-network my-container

# Supprimer un network
docker network rm my-network
```

### Port mapping

```bash
# Mapper le port 8080 du container sur le port 9090 de l'hôte
docker run -p 9090:8080 my-spring-app

# Mapper sur toutes les interfaces (défaut)
docker run -p 0.0.0.0:8080:8080 my-spring-app

# Mapper sur une interface spécifique
docker run -p 127.0.0.1:8080:8080 my-spring-app

# Mapper un port UDP
docker run -p 5005:5005/udp my-spring-app

# Mapper un range de ports
docker run -p 8080-8090:8080-8090 my-spring-app
```

### DNS interne Docker

Dans un network bridge personnalisé, Docker fournit un **DNS interne** automatique : les containers se trouvent par leur nom ou leur alias.

```yaml
# docker-compose.yml
services:
  app:
    image: my-spring-app
    # spring.datasource.url=jdbc:postgresql://postgres:5432/mydb
    # "postgres" est le nom du service, résolu automatiquement
  
  postgres:
    image: postgres:15
```

---

## 8. Docker Compose

### Qu'est-ce que Docker Compose ?

**Docker Compose** est un outil pour définir et gérer des applications multi-containers via un fichier YAML (`docker-compose.yml`). Il permet de décrire toute une stack applicative (app + base de données + cache + broker...) et de la démarrer avec une seule commande.

### Structure d'un `docker-compose.yml`

```yaml
# Version du format (optionnelle depuis Compose v2)
version: '3.9'

services:        # Définition des containers
  service-name:
    image: ...   # OU build: ...
    ports: ...
    environment: ...
    volumes: ...
    networks: ...
    depends_on: ...

volumes:         # Déclaration des volumes nommés
  volume-name:

networks:        # Déclaration des networks
  network-name:
```

### Exemple complet — Stack Spring Boot + PostgreSQL + Redis

```yaml
version: '3.9'

services:

  # ── Application Spring Boot ──────────────────────────────
  app:
    build:
      context: .
      dockerfile: Dockerfile
    container_name: spring-app
    ports:
      - "8080:8080"
    environment:
      - SPRING_PROFILES_ACTIVE=docker
      - SPRING_DATASOURCE_URL=jdbc:postgresql://postgres:5432/mydb
      - SPRING_DATASOURCE_USERNAME=myuser
      - SPRING_DATASOURCE_PASSWORD=mypassword
      - SPRING_REDIS_HOST=redis
      - SPRING_REDIS_PORT=6379
    volumes:
      - app-logs:/app/logs
    depends_on:
      postgres:
        condition: service_healthy
      redis:
        condition: service_started
    networks:
      - app-network
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:8080/actuator/health"]
      interval: 30s
      timeout: 10s
      retries: 3
      start_period: 90s

  # ── PostgreSQL ───────────────────────────────────────────
  postgres:
    image: postgres:15-alpine
    container_name: postgres-db
    ports:
      - "5432:5432"
    environment:
      POSTGRES_DB: mydb
      POSTGRES_USER: myuser
      POSTGRES_PASSWORD: mypassword
    volumes:
      - pgdata:/var/lib/postgresql/data
      - ./init-scripts:/docker-entrypoint-initdb.d:ro
    networks:
      - app-network
    restart: unless-stopped
    healthcheck:
      test: ["CMD-SHELL", "pg_isready -U myuser -d mydb"]
      interval: 10s
      timeout: 5s
      retries: 5

  # ── Redis ────────────────────────────────────────────────
  redis:
    image: redis:7-alpine
    container_name: redis-cache
    ports:
      - "6379:6379"
    command: redis-server --maxmemory 256mb --maxmemory-policy allkeys-lru
    volumes:
      - redisdata:/data
    networks:
      - app-network
    restart: unless-stopped

  # ── PgAdmin (optionnel, dev seulement) ───────────────────
  pgadmin:
    image: dpage/pgadmin4
    container_name: pgadmin
    ports:
      - "5050:80"
    environment:
      PGADMIN_DEFAULT_EMAIL: admin@admin.com
      PGADMIN_DEFAULT_PASSWORD: admin
    networks:
      - app-network
    profiles:
      - dev   # Démarré uniquement avec --profile dev

volumes:
  pgdata:
  redisdata:
  app-logs:

networks:
  app-network:
    driver: bridge
```

### Commandes Docker Compose

```bash
# Démarrer tous les services (en arrière-plan)
docker compose up -d

# Démarrer avec rebuild de l'image
docker compose up -d --build

# Démarrer uniquement certains services
docker compose up -d postgres redis

# Démarrer avec un profil spécifique
docker compose --profile dev up -d

# Arrêter tous les services
docker compose down

# Arrêter ET supprimer volumes (attention !)
docker compose down -v

# Voir les logs
docker compose logs -f
docker compose logs -f app

# Lister les services
docker compose ps

# Exécuter une commande dans un service
docker compose exec app bash

# Scaler un service (plusieurs instances)
docker compose up -d --scale app=3

# Redémarrer un service
docker compose restart app

# Voir les variables d'environnement d'un service
docker compose config
```

### `depends_on` et `condition`

```yaml
depends_on:
  postgres:
    condition: service_healthy   # Attend que le healthcheck passe
  redis:
    condition: service_started   # Attend juste le démarrage
  migrations:
    condition: service_completed_successfully  # Attend la fin d'un job
```

### Fichiers `.env` avec Docker Compose

```bash
# .env (à la racine, automatiquement chargé)
POSTGRES_USER=myuser
POSTGRES_PASSWORD=secret123
SPRING_PROFILE=docker
APP_VERSION=1.0.0
```

```yaml
# docker-compose.yml — utilisation des variables
services:
  postgres:
    image: postgres:${POSTGRES_VERSION:-15}
    environment:
      POSTGRES_USER: ${POSTGRES_USER}
      POSTGRES_PASSWORD: ${POSTGRES_PASSWORD}
```

---

## 9. Dockeriser une application Spring Boot

### Structure du projet

```
my-spring-app/
├── src/
│   └── main/
│       ├── java/
│       └── resources/
│           ├── application.yml
│           └── application-docker.yml   # Config spécifique Docker
├── Dockerfile
├── .dockerignore
├── docker-compose.yml
└── pom.xml
```

### Configuration Spring Boot pour Docker

```yaml
# application-docker.yml
spring:
  datasource:
    url: jdbc:postgresql://${DB_HOST:postgres}:${DB_PORT:5432}/${DB_NAME:mydb}
    username: ${DB_USER:myuser}
    password: ${DB_PASSWORD:mypassword}
    hikari:
      maximum-pool-size: 10
      minimum-idle: 5
  
  redis:
    host: ${REDIS_HOST:redis}
    port: ${REDIS_PORT:6379}

  jpa:
    hibernate:
      ddl-auto: validate
    show-sql: false

server:
  port: ${SERVER_PORT:8080}

management:
  endpoints:
    web:
      exposure:
        include: health,info,metrics
  endpoint:
    health:
      show-details: always
```

### Construction de l'image

```bash
# Build le projet Maven d'abord
./mvnw clean package -DskipTests

# Construire l'image Docker
docker build -t my-spring-app:1.0.0 .
docker build -t my-spring-app:latest .

# Build avec ARG
docker build --build-arg BUILD_VERSION=1.0.0 -t my-spring-app:1.0.0 .

# Builder et nommer en une seule commande
docker build -t my-spring-app:$(git rev-parse --short HEAD) .
```

### Démarrage avec variables d'environnement

```bash
docker run -d \
  -p 8080:8080 \
  -e SPRING_PROFILES_ACTIVE=docker \
  -e DB_HOST=localhost \
  -e DB_NAME=mydb \
  -e DB_USER=myuser \
  -e DB_PASSWORD=secret \
  --name spring-app \
  my-spring-app:latest
```

### Passer la JVM Options

```dockerfile
# Dans le Dockerfile
ENV JAVA_OPTS="-Xmx512m -Xms256m -XX:+UseContainerSupport"

ENTRYPOINT ["sh", "-c", "java $JAVA_OPTS -jar app.jar"]
```

Ou plus proprement :

```dockerfile
ENTRYPOINT ["java", \
  "-XX:+UseContainerSupport", \
  "-XX:MaxRAMPercentage=75.0", \
  "-jar", \
  "app.jar"]
```

> **Important en production** : utiliser `-XX:+UseContainerSupport` (activé par défaut depuis Java 10) et `-XX:MaxRAMPercentage` au lieu de `-Xmx` fixe. Ainsi la JVM respecte la limite mémoire du container.

### Profil de debug distant

```bash
docker run -d \
  -p 8080:8080 \
  -p 5005:5005 \
  -e JAVA_TOOL_OPTIONS="-agentlib:jdwp=transport=dt_socket,server=y,suspend=n,address=*:5005" \
  --name spring-app-debug \
  my-spring-app:latest
```

---

## 10. Multi-stage Build

### Pourquoi le multi-stage build ?

Sans multi-stage, l'image finale contient tout le JDK, Maven, le code source et les artefacts de build → image volumineuse (~500 Mo+). Avec le multi-stage build, on sépare le **build** de l'**exécution** → image finale légère (~150 Mo ou moins).

### Multi-stage Build avec Maven

```dockerfile
# ═══════════════════════════════════════════════════════════
# STAGE 1 : Build de l'application
# ═══════════════════════════════════════════════════════════
FROM eclipse-temurin:21-jdk AS builder

WORKDIR /build

# Copier d'abord le pom.xml pour bénéficier du cache Docker
# Si le pom.xml ne change pas, cette couche est cachée
COPY pom.xml .
COPY .mvn/ .mvn/
COPY mvnw .
RUN chmod +x mvnw

# Télécharger les dépendances (couche cacheable)
RUN ./mvnw dependency:go-offline -q

# Copier le code source et compiler
COPY src/ src/
RUN ./mvnw clean package -DskipTests -q

# ═══════════════════════════════════════════════════════════
# STAGE 2 : Image de production (légère)
# ═══════════════════════════════════════════════════════════
FROM eclipse-temurin:21-jre AS production

WORKDIR /app

# Utilisateur non-root pour la sécurité
RUN addgroup --system appgroup \
    && adduser --system --group appgroup appuser

# Copier uniquement le JAR depuis le stage builder
COPY --from=builder /build/target/*.jar app.jar

# Changer le propriétaire
RUN chown appuser:appgroup app.jar

USER appuser

EXPOSE 8080

HEALTHCHECK --interval=30s --timeout=10s --start-period=90s --retries=3 \
  CMD curl -f http://localhost:8080/actuator/health || exit 1

ENTRYPOINT ["java", \
  "-XX:+UseContainerSupport", \
  "-XX:MaxRAMPercentage=75.0", \
  "-jar", \
  "app.jar"]
```

### Multi-stage Build avec layers Spring Boot

Depuis Spring Boot 2.3+, il est possible d'utiliser les **layered JARs** pour optimiser encore le cache Docker.

```dockerfile
# STAGE 1 : Extraction des couches
FROM eclipse-temurin:21-jdk AS builder
WORKDIR /build
COPY target/*.jar app.jar
# Extraire les couches du JAR Spring Boot
RUN java -Djarmode=layertools -jar app.jar extract --destination extracted

# STAGE 2 : Image finale avec couches séparées
FROM eclipse-temurin:21-jre AS production
WORKDIR /app

RUN addgroup --system appgroup && adduser --system --group appgroup appuser

# Copier chaque couche séparément (ordre du moins au plus changeant)
COPY --from=builder /build/extracted/dependencies/ ./
COPY --from=builder /build/extracted/spring-boot-loader/ ./
COPY --from=builder /build/extracted/snapshot-dependencies/ ./
COPY --from=builder /build/extracted/application/ ./

USER appuser

EXPOSE 8080

ENTRYPOINT ["java", "org.springframework.boot.loader.launch.JarLauncher"]
```

**Avantage** : seule la couche `application` change à chaque build → rebuild et push ultra-rapides.

```bash
# Activer les layered JARs dans pom.xml
# (activé par défaut depuis Spring Boot 2.3)
<plugin>
  <groupId>org.springframework.boot</groupId>
  <artifactId>spring-boot-maven-plugin</artifactId>
  <configuration>
    <layers>
      <enabled>true</enabled>
    </layers>
  </configuration>
</plugin>
```

### Comparaison des tailles d'image

| Approche                        | Taille approximative |
|--------------------------------|---------------------|
| Dockerfile simple (JDK)        | ~400–500 Mo          |
| Multi-stage (JRE)              | ~150–200 Mo          |
| Multi-stage (JRE Alpine)       | ~100–120 Mo          |
| Distroless Java                | ~80–100 Mo           |

---

## 11. Registry & Docker Hub

### Qu'est-ce qu'un Registry ?

Un **Docker Registry** est un service de stockage et distribution d'images Docker. Les images sont organisées en **repositories** avec des **tags**.

### Docker Hub

Registry public officiel : [hub.docker.com](https://hub.docker.com)

```bash
# Se connecter
docker login

# Se connecter à un registry privé
docker login registry.company.com

# Pousser une image
docker push username/my-spring-app:1.0.0

# Tirer une image
docker pull username/my-spring-app:1.0.0
```

### Registry privé (self-hosted)

```bash
# Démarrer un registry local
docker run -d \
  -p 5000:5000 \
  --name local-registry \
  -v registry-data:/var/lib/registry \
  registry:2

# Tagger pour le registry local
docker tag my-spring-app:latest localhost:5000/my-spring-app:latest

# Pousser
docker push localhost:5000/my-spring-app:latest

# Tirer
docker pull localhost:5000/my-spring-app:latest
```

### Registries cloud populaires

| Registry            | Cloud Provider | Commande de login                               |
|--------------------|---------------|--------------------------------------------------|
| Docker Hub         | -             | `docker login`                                   |
| Amazon ECR         | AWS           | `aws ecr get-login-password \| docker login ...` |
| Google Artifact Registry | GCP    | `gcloud auth configure-docker`                   |
| Azure Container Registry | Azure  | `az acr login --name myregistry`                 |
| GitLab Registry    | GitLab        | `docker login registry.gitlab.com`              |
| GitHub Registry    | GitHub        | `docker login ghcr.io`                           |

---

## 12. Variables d'environnement & Configuration

### Stratégies de configuration Spring Boot dans Docker

#### Option 1 : Variables d'environnement directes

```bash
docker run -e SPRING_DATASOURCE_URL=jdbc:postgresql://... my-app
```

#### Option 2 : Fichier `.env`

```bash
# .env
DB_HOST=postgres
DB_PORT=5432
DB_NAME=mydb
DB_USER=myuser
DB_PASSWORD=secret

# docker run avec --env-file
docker run --env-file .env my-spring-app
```

#### Option 3 : Secrets Docker (Swarm)

Pour les données sensibles (mots de passe, clés API) en production.

```bash
# Créer un secret
echo "mysecretpassword" | docker secret create db_password -

# Utiliser dans le service
docker service create \
  --secret db_password \
  my-spring-app
```

Le secret est monté dans `/run/secrets/db_password`.

#### Option 4 : Fichier `application.yml` externe

```yaml
# docker-compose.yml
services:
  app:
    volumes:
      - ./config/application-docker.yml:/app/config/application-docker.yml:ro
    environment:
      - SPRING_CONFIG_ADDITIONAL_LOCATION=file:/app/config/
```

#### Option 5 : Spring Cloud Config Server

Pour les microservices, externaliser toute la configuration dans un Config Server.

```yaml
# bootstrap.yml dans le container
spring:
  cloud:
    config:
      uri: http://config-server:8888
      profile: docker
```

### Convention de nommage Spring Boot + Variables d'env

Spring Boot convertit automatiquement les variables d'environnement :
- `SPRING_DATASOURCE_URL` → `spring.datasource.url`
- `SPRING_JPA_HIBERNATE_DDL_AUTO` → `spring.jpa.hibernate.ddl-auto`

---

## 13. Bonnes pratiques

### 1. Minimiser le nombre de couches

```dockerfile
# ❌ Mauvais : 3 couches séparées
RUN apt-get update
RUN apt-get install -y curl
RUN rm -rf /var/lib/apt/lists/*

# ✅ Bon : une seule couche
RUN apt-get update \
    && apt-get install -y curl \
    && rm -rf /var/lib/apt/lists/*
```

### 2. Optimiser le cache Docker (ordre des instructions)

```dockerfile
# ✅ Mettre les couches les moins changeantes en premier
# Les dépendances changent rarement → couche stable et cachée
COPY pom.xml .
RUN ./mvnw dependency:go-offline

# Le code source change souvent → couche invalidée souvent
COPY src/ src/
RUN ./mvnw package
```

### 3. Utiliser un utilisateur non-root

```dockerfile
RUN addgroup --system appgroup && adduser --system --group appgroup appuser
USER appuser
```

### 4. Image de base minimale

```dockerfile
# ❌ Trop lourd
FROM ubuntu:22.04

# ✅ Mieux : image officielle JRE
FROM eclipse-temurin:21-jre

# ✅ Encore mieux : Alpine (si compatible)
FROM eclipse-temurin:21-jre-alpine
```

### 5. Ne jamais stocker de secrets dans l'image

```dockerfile
# ❌ JAMAIS ça
ENV DB_PASSWORD=mysecretpassword
RUN echo "password" > /app/secret.txt

# ✅ Passer via variable d'environnement au runtime
# docker run -e DB_PASSWORD=$DB_PASSWORD ...
```

### 6. Utiliser `.dockerignore`

```
target/
.git/
.idea/
*.md
*.log
node_modules/
```

### 7. Tags explicites — jamais `latest` en production

```bash
# ❌ Dangereux en production
FROM my-app:latest

# ✅ Version explicite et traçable
FROM my-app:1.2.3
FROM my-app:abc1234  # git commit SHA
```

### 8. Healthcheck toujours défini

```dockerfile
HEALTHCHECK --interval=30s --timeout=10s --start-period=90s --retries=3 \
  CMD curl -f http://localhost:8080/actuator/health || exit 1
```

### 9. Paramètres JVM adaptés aux containers

```dockerfile
ENTRYPOINT ["java", \
  "-XX:+UseContainerSupport", \
  "-XX:MaxRAMPercentage=75.0", \
  "-XX:+ExitOnOutOfMemoryError", \
  "-jar", "app.jar"]
```

### 10. Scanner les images pour les vulnérabilités

```bash
# Avec Docker Scout (intégré à Docker Desktop)
docker scout cves my-spring-app:latest

# Avec Trivy (open-source)
trivy image my-spring-app:latest
```

---

## 14. Docker en CI/CD

### Workflow typique avec GitHub Actions

```yaml
# .github/workflows/docker.yml
name: Build and Push Docker Image

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

env:
  REGISTRY: ghcr.io
  IMAGE_NAME: ${{ github.repository }}

jobs:
  build-and-push:
    runs-on: ubuntu-latest
    permissions:
      contents: read
      packages: write

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Set up JDK 21
        uses: actions/setup-java@v4
        with:
          java-version: '21'
          distribution: 'temurin'

      - name: Cache Maven packages
        uses: actions/cache@v3
        with:
          path: ~/.m2
          key: ${{ runner.os }}-m2-${{ hashFiles('**/pom.xml') }}

      - name: Build with Maven
        run: ./mvnw clean package -DskipTests

      - name: Log in to Container Registry
        uses: docker/login-action@v3
        with:
          registry: ${{ env.REGISTRY }}
          username: ${{ github.actor }}
          password: ${{ secrets.GITHUB_TOKEN }}

      - name: Extract metadata (tags, labels)
        id: meta
        uses: docker/metadata-action@v5
        with:
          images: ${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}
          tags: |
            type=ref,event=branch
            type=ref,event=pr
            type=semver,pattern={{version}}
            type=sha,prefix=,suffix=,format=short

      - name: Build and push Docker image
        uses: docker/build-push-action@v5
        with:
          context: .
          push: ${{ github.event_name != 'pull_request' }}
          tags: ${{ steps.meta.outputs.tags }}
          labels: ${{ steps.meta.outputs.labels }}
          cache-from: type=gha
          cache-to: type=gha,mode=max
```

### Buildkit et cache avancé

```bash
# Activer BuildKit (activé par défaut depuis Docker 23)
export DOCKER_BUILDKIT=1

# Build avec cache de registry
docker buildx build \
  --cache-from=type=registry,ref=myregistry/my-app:cache \
  --cache-to=type=registry,ref=myregistry/my-app:cache,mode=max \
  -t myregistry/my-app:latest \
  --push .
```

### Tests d'intégration avec Docker Compose en CI

```yaml
# .github/workflows/integration-tests.yml
- name: Start services
  run: docker compose -f docker-compose.test.yml up -d

- name: Wait for services to be healthy
  run: |
    docker compose -f docker-compose.test.yml ps
    sleep 30

- name: Run integration tests
  run: ./mvnw verify -P integration-tests

- name: Stop services
  if: always()
  run: docker compose -f docker-compose.test.yml down -v
```

### Testcontainers — Tests d'intégration dans Spring Boot

**Testcontainers** est une bibliothèque Java qui permet de démarrer des containers Docker directement dans les tests JUnit.

```java
// pom.xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-testcontainers</artifactId>
    <scope>test</scope>
</dependency>
<dependency>
    <groupId>org.testcontainers</groupId>
    <artifactId>postgresql</artifactId>
    <scope>test</scope>
</dependency>
```

```java
@SpringBootTest
@Testcontainers
class UserServiceIntegrationTest {

    @Container
    static PostgreSQLContainer<?> postgres = new PostgreSQLContainer<>("postgres:15-alpine")
            .withDatabaseName("testdb")
            .withUsername("testuser")
            .withPassword("testpass");

    @DynamicPropertySource
    static void configureDataSource(DynamicPropertyRegistry registry) {
        registry.add("spring.datasource.url", postgres::getJdbcUrl);
        registry.add("spring.datasource.username", postgres::getUsername);
        registry.add("spring.datasource.password", postgres::getPassword);
    }

    @Autowired
    private UserService userService;

    @Test
    void shouldSaveAndRetrieveUser() {
        User user = new User("anouar", "anouar@example.com");
        User saved = userService.save(user);
        assertThat(saved.getId()).isNotNull();
    }
}
```

Avec Spring Boot 3.1+ (annotation simplifiée) :

```java
@SpringBootTest
@ImportTestcontainers(PostgresContainerConfig.class)
class UserServiceIntegrationTest { ... }

// PostgresContainerConfig.java
@TestConfiguration(proxyBeanMethods = false)
class PostgresContainerConfig {
    @Bean
    @ServiceConnection
    PostgreSQLContainer<?> postgres() {
        return new PostgreSQLContainer<>("postgres:15-alpine");
    }
}
```

---

## 15. Questions/Réponses d'entretien

---

### 🔹 Question 1 : Quelle est la différence entre une image et un container Docker ?

**Réponse :**

Une **image** est un template immuable en lecture seule qui contient tout le nécessaire pour exécuter une application : système de base, runtime, code, dépendances. On peut faire l'analogie avec une **classe** en Java.

Un **container** est une instance en cours d'exécution d'une image. C'est un processus isolé qui dispose de sa propre couche d'écriture. Si l'image est la classe, le container est l'**objet instancié**.

On peut créer plusieurs containers à partir d'une même image, tout comme on peut instancier plusieurs objets depuis une classe.

---

### 🔹 Question 2 : Quelle est la différence entre `CMD` et `ENTRYPOINT` dans un Dockerfile ?

**Réponse :**

`ENTRYPOINT` définit la commande principale du container, qui ne peut pas être facilement surchargée.

`CMD` définit les arguments par défaut fournis à l'ENTRYPOINT (ou la commande par défaut si pas d'ENTRYPOINT). Elle est facilement surchargeable lors du `docker run`.

Exemple :
```dockerfile
ENTRYPOINT ["java"]
CMD ["-jar", "app.jar"]
```
→ `docker run my-app` lance `java -jar app.jar`
→ `docker run my-app -jar other.jar` lance `java -jar other.jar`

En pratique pour Spring Boot, j'utilise `ENTRYPOINT ["java", "-jar", "app.jar"]` pour éviter toute ambiguïté.

---

### 🔹 Question 3 : Qu'est-ce qu'un multi-stage build et pourquoi l'utiliser ?

**Réponse :**

Le **multi-stage build** permet d'utiliser plusieurs stages `FROM` dans un seul Dockerfile. L'idée est de séparer l'environnement de **build** (JDK + Maven) de l'environnement d'**exécution** (JRE seul).

Avantages :
- Image finale **beaucoup plus légère** (150 Mo au lieu de 500 Mo)
- Aucun outil de build (Maven, JDK) dans l'image de production
- **Surface d'attaque réduite** en termes de sécurité
- Build en une seule commande `docker build`

Pour Spring Boot : stage 1 avec `eclipse-temurin:21-jdk` pour compiler, stage 2 avec `eclipse-temurin:21-jre` pour l'exécution, en copiant uniquement le JAR avec `COPY --from=builder`.

---

### 🔹 Question 4 : Comment Docker isole-t-il les containers ?

**Réponse :**

Docker utilise des mécanismes du kernel Linux :

- **Namespaces** : isolent les ressources (PID pour les processus, NET pour le réseau, MNT pour le système de fichiers, UTS pour le hostname, IPC pour la communication inter-processus).
- **Control Groups (cgroups)** : limitent les ressources consommées (CPU, mémoire, I/O).
- **Union File System (overlay2)** : système de fichiers en couches qui permet de partager les couches communes entre containers.

Contrairement aux VMs qui virtualisent le hardware, Docker partage le kernel de l'hôte. C'est pourquoi les containers démarrent en quelques secondes et consomment beaucoup moins de ressources.

---

### 🔹 Question 5 : Quelle est la différence entre un volume et un bind mount ?

**Réponse :**

- **Volume** : géré entièrement par Docker, stocké dans `/var/lib/docker/volumes/`. Il est indépendant de la structure de l'hôte, portable entre machines et recommandé en production (notamment pour les bases de données).

- **Bind Mount** : monte directement un répertoire ou fichier de l'hôte dans le container. Le chemin exact de l'hôte doit exister. Très utile en développement pour le hot reload (montage du répertoire src/) ou pour injecter une configuration externe.

En production, je préfère les volumes car ils sont plus sûrs, plus performants et gérés par Docker. En développement, j'utilise les bind mounts pour la rapidité d'itération.

---

### 🔹 Question 6 : Comment fonctionne le networking entre deux containers ?

**Réponse :**

Par défaut, les containers sur le réseau `bridge` ne peuvent pas se résoudre par nom. Il faut créer un **network bridge personnalisé** pour bénéficier du DNS interne Docker.

Dans un réseau personnalisé, Docker fournit un serveur DNS intégré : chaque container est accessible via son nom ou son alias de service. Ainsi dans Docker Compose, un service `postgres` est accessible depuis l'application via l'URL `jdbc:postgresql://postgres:5432/mydb` sans configuration supplémentaire.

Les containers peuvent également publier leurs ports vers l'hôte avec `-p host:container`, permettant l'accès depuis l'extérieur.

---

### 🔹 Question 7 : Qu'est-ce que Docker Compose et quand l'utiliser ?

**Réponse :**

**Docker Compose** est un outil qui permet de définir et gérer des applications multi-containers via un fichier YAML. Il est idéal pour :

- Les **environnements de développement local** : démarrer toute la stack (app + PostgreSQL + Redis + Kafka) en une commande.
- Les **tests d'intégration en CI/CD**.
- Les **déploiements simples** sur un seul hôte.

Pour un projet Spring Boot type, je définis : le service applicatif (avec build du Dockerfile), PostgreSQL avec son healthcheck, Redis, et les networks/volumes nécessaires. La commande `docker compose up -d --build` suffit à lancer tout l'environnement.

Pour des déploiements complexes en cluster (multi-hôtes, haute disponibilité), on passe à **Kubernetes**.

---

### 🔹 Question 8 : Qu'est-ce que Testcontainers et comment l'utiliser avec Spring Boot ?

**Réponse :**

**Testcontainers** est une bibliothèque Java qui permet de démarrer des containers Docker automatiquement lors des tests JUnit. On a ainsi de vrais services (PostgreSQL, Redis, Kafka...) en test, sans mock ni configuration complexe.

Dans Spring Boot 3.1+, avec l'annotation `@ServiceConnection`, Testcontainers configure automatiquement les propriétés Spring (URL, username, password) sans `@DynamicPropertySource`.

Avantages :
- Tests d'intégration **fiables** sur la vraie base de données
- Pas de base de données H2 en mémoire qui ne reflète pas les spécificités de PostgreSQL
- Isolation : chaque test démarre son propre container (ou réutilise via `@Container static`)
- Fonctionne en local et en CI (tout environnement avec Docker)

---

### 🔹 Question 9 : Comment gérer les secrets et la configuration sensible dans Docker ?

**Réponse :**

Il ne faut **jamais** stocker des secrets dans l'image Docker (dans le Dockerfile ou sous forme de `ENV`). Les bonnes pratiques :

1. **Variables d'environnement au runtime** : `docker run -e DB_PASSWORD=...` ou via `--env-file`.
2. **Docker Secrets** (mode Swarm) : montés dans `/run/secrets/` avec accès restreint.
3. **Gestionnaires de secrets externes** : HashiCorp Vault, AWS Secrets Manager, Azure Key Vault — l'application les consomme au démarrage.
4. **Kubernetes Secrets** : en environnement K8s, les secrets sont injectés comme variables d'env ou volumes.

La règle d'or : les secrets ne doivent apparaître ni dans le code, ni dans les images, ni dans les logs.

---

### 🔹 Question 10 : Quelles options JVM recommandes-tu pour un container Docker ?

**Réponse :**

Les JVM modernes (Java 10+) reconnaissent les limites des containers grâce au flag `-XX:+UseContainerSupport` (activé par défaut).

Je recommande :
- `-XX:+UseContainerSupport` : respecte les limites mémoire du container (cgroups)
- `-XX:MaxRAMPercentage=75.0` : utilise 75% de la RAM du container (préférable à `-Xmx` fixe, car adaptatif)
- `-XX:+ExitOnOutOfMemoryError` : le container se stoppe proprement en cas d'OOM (plutôt que rester dans un état instable)
- `-XX:+HeapDumpOnOutOfMemoryError -XX:HeapDumpPath=/app/dumps` : diagnostics en cas de problème mémoire

En développement ou debug : `-agentlib:jdwp=transport=dt_socket,server=y,suspend=n,address=*:5005` pour le remote debugging.

---

### 🔹 Question 11 : Quelle est la différence entre `COPY` et `ADD` dans un Dockerfile ?

**Réponse :**

`COPY` copie simplement des fichiers locaux dans l'image. C'est la commande à préférer dans la grande majorité des cas.

`ADD` fait la même chose mais offre deux fonctionnalités supplémentaires : la décompression automatique des archives tar locales, et le téléchargement depuis une URL.

Bonne pratique : utiliser `COPY` par défaut pour la lisibilité et la prévisibilité. N'utiliser `ADD` que si on a réellement besoin de la décompression automatique.

---

### 🔹 Question 12 : Comment optimiser le cache lors du build d'une image Spring Boot ?

**Réponse :**

La règle fondamentale : placer les instructions qui changent le moins souvent en **premier**, celles qui changent souvent en **dernier**.

Pour Spring Boot avec Maven :
1. `COPY pom.xml .` → copier d'abord le descripteur de dépendances
2. `RUN ./mvnw dependency:go-offline` → télécharger les dépendances (couche stable)
3. `COPY src/ .` → code source (couche volatile)
4. `RUN ./mvnw package` → compilation

Ainsi, si seul le code source change, les étapes 1 et 2 restent cachées et le build est beaucoup plus rapide.

Avec les **layered JARs** de Spring Boot, on peut aller encore plus loin : les dépendances, le loader et le code applicatif sont dans des couches Docker séparées. Seule la couche `application` change à chaque rebuild.

---

*Fin du cours — Bonne préparation à l'entretien ! 🚀*
