# AI Engineering pour Développeurs Java / Spring / Angular

> Cours complet couvrant les fondamentaux du **Generative AI**, du **Prompt Engineering**, du **RAG**, du **Fine-tuning**, du **Function Calling**, du **Model Context Protocol (MCP)** et leur implémentation pratique avec **Spring AI**.

---

## Sommaire

1. [Introduction : qu'est-ce que l'AI Engineering ?](#1-introduction--quest-ce-que-lai-engineering-)
2. [Fondamentaux des LLMs](#2-fondamentaux-des-llms-large-language-models)
3. [Prompt Engineering & Chain of Thought](#3-prompt-engineering--chain-of-thought-cot)
4. [RAG (Retrieval-Augmented Generation)](#4-rag-retrieval-augmented-generation)
5. [Fine-tuning & RLHF](#5-fine-tuning--rlhf)
6. [Function Calling / Tool Use & Agentic AI](#6-function-calling--tool-use--agentic-ai)
7. [Model Context Protocol (MCP)](#7-model-context-protocol-mcp)
8. [Outils IA pour développeurs : Copilot, Cursor, Claude Code](#8-outils-ia-pour-développeurs--copilot-cursor-claude-code)
9. [Sécurité & Gouvernance IA en entreprise](#9-sécurité--gouvernance-ia-en-entreprise)
10. [Spring AI : implémenter l'AI Engineering en Java](#10-spring-ai--implémenter-lai-engineering-en-java)
11. [Q&A Oral – Préparation Entretien (15 questions)](#11-qa-oral--préparation-entretien-15-questions)
12. [Ressources pour aller plus loin](#12-ressources-pour-aller-plus-loin)

---

## 1. Introduction : qu'est-ce que l'AI Engineering ?

L'**AI Engineering** (parfois appelé **GenAI Engineering** ou **Applied AI**) est la discipline qui consiste à **intégrer des modèles d'IA générative (LLMs) dans des applications logicielles**, par opposition au **Machine Learning Engineering** classique qui consiste à entraîner des modèles depuis zéro.

### Différence clé avec le ML classique

| | ML Engineering classique | AI Engineering (GenAI) |
|---|---|---|
| Objectif | Entraîner un modèle sur des données | Intégrer un modèle déjà entraîné (LLM) dans une app |
| Compétence dominante | Data Science, statistiques | Développement logiciel (API, architecture, prompts) |
| Exemple | Entraîner un modèle de scoring crédit | Brancher GPT/Claude dans un backend Spring Boot |

**Pourquoi c'est ton domaine en tant que développeur Java/Spring/Angular :** tu n'as pas besoin de savoir entraîner un réseau de neurones. Tu as besoin de savoir :
- Appeler une API LLM correctement (**prompting**)
- Lui donner du contexte (**RAG**)
- Lui donner des capacités d'action (**Function Calling / Tool Use**)
- Le connecter à des systèmes externes de façon standardisée (**MCP**)
- Sécuriser tout ça (**AI Governance**)

C'est exactement ce que couvrent les chapitres suivants.

---

## 2. Fondamentaux des LLMs (Large Language Models)

### 2.1 Qu'est-ce qu'un LLM ?

Un **LLM (Large Language Model)** est un modèle basé sur l'architecture **Transformer**, entraîné sur d'énormes quantités de texte pour prédire le **token** suivant dans une séquence.

### 2.2 Les Tokens

Un **token** n'est pas un mot, c'est une unité de découpage du texte (souvent un morceau de mot). Par exemple, "développement" peut être découpé en plusieurs tokens.

```
"Je code en Java" → ["Je", " code", " en", " Java"]  (simplifié)
```

C'est important côté dev car :
- Les API LLM **facturent au token** (input + output)
- Chaque modèle a une **context window** limitée (ex. 200K tokens) = la quantité totale de texte (prompt + historique + réponse) qu'il peut traiter en une fois

### 2.3 Embeddings

Un **embedding** est une représentation vectorielle (un tableau de nombres flottants) d'un texte, capturant son **sens sémantique**. Deux textes proches en sens auront des vecteurs proches mathématiquement (distance cosinus, etc.).

C'est la brique de base du **RAG** (voir chapitre 4) et de la recherche sémantique.

### 2.4 Pourquoi les LLMs ont du mal avec les calculs mathématiques

C'est une question classique en entretien : un LLM **ne calcule pas**, il **prédit le prochain token de façon probabiliste**. Il n'a pas d'**Arithmetic Logic Unit (ALU)** interne garantissant l'exactitude d'un calcul, contrairement à une calculatrice.

➡️ **Bonne pratique** : pour des calculs complexes, déléguer à un outil externe via **Function Calling** (ex. exécuter du code Python, appeler une calculatrice) plutôt que de faire confiance au calcul "mental" du LLM.

---

## 3. Prompt Engineering & Chain of Thought (CoT)

### 3.1 Prompt Engineering

Le **Prompt Engineering** consiste à formuler ses instructions (le **prompt**) de façon à obtenir la meilleure réponse possible d'un LLM. Ce n'est pas de la magie, ce sont des techniques reproductibles :

- **Être spécifique et explicite** (rôle, format de sortie attendu, contraintes)
- **Donner des exemples** (**few-shot prompting**) plutôt que de décrire abstraitement (**zero-shot prompting**)
- **Structurer le prompt** avec des tags ou des sections claires (ex. balises XML : `<context>`, `<task>`, `<format>`)
- **Demander un format de sortie précis** (JSON, Markdown, etc.) quand le résultat doit être parsé par du code

### 3.2 Chain of Thought (CoT)

Le **Chain of Thought** consiste à demander au modèle de **raisonner étape par étape avant de donner sa réponse finale**, plutôt que de répondre directement.

**Exemple sans CoT :**
```
Prompt : "Un train part à 14h, roule à 80km/h, arrive à 16h30. Distance ?"
→ Risque d'erreur de calcul direct
```

**Exemple avec CoT :**
```
Prompt : "Réfléchis étape par étape avant de répondre :
Un train part à 14h, roule à 80km/h, arrive à 16h30. Distance ?"
→ Le modèle décompose : durée = 2h30 = 2.5h, distance = 80 × 2.5 = 200km
```

Le CoT améliore significativement la fiabilité sur les tâches de raisonnement, de calcul, et d'analyse complexe. C'est aussi la base de modèles dits "reasoning" qui appliquent du CoT de façon native et étendue avant de répondre.

---

## 4. RAG (Retrieval-Augmented Generation)

### 4.1 Le problème que RAG résout

Un LLM a une **connaissance figée** au moment de son entraînement (**knowledge cutoff**) et ne connaît pas tes données privées (documentation interne, base de données métier, etc.).

Le **RAG (Retrieval-Augmented Generation)** résout ça en **injectant du contexte externe pertinent dans le prompt**, sans modifier le modèle.

### 4.2 Le pipeline RAG

```
1. INDEXATION (offline)
   Documents → Découpage (chunking) → Embedding → Stockage dans un Vector Store

2. REQUÊTE (au runtime)
   Question utilisateur → Embedding de la question
        ↓
   Recherche de similarité dans le Vector Store (Vector Search)
        ↓
   Récupération des chunks les plus pertinents (Retrieval)
        ↓
   Injection de ces chunks dans le prompt (Augmentation)
        ↓
   Le LLM génère sa réponse en s'appuyant sur ce contexte (Generation)
```

### 4.3 Composants techniques clés

| Composant | Rôle |
|---|---|
| **Chunking** | Découper les documents en morceaux exploitables (ex. 500-1000 tokens) |
| **Embedding Model** | Transformer chaque chunk en vecteur |
| **Vector Store / Vector Database** | Stocker et indexer les vecteurs (ex. pgvector, Pinecone, Qdrant, Redis) |
| **Similarity Search** | Trouver les vecteurs les plus proches de la question (cosine similarity) |
| **Reranking** (optionnel) | Affiner les résultats récupérés avant de les envoyer au LLM |

### 4.4 RAG vs Fine-tuning — la question classique en entretien

| | **RAG** | **Fine-tuning** |
|---|---|---|
| Mécanisme | Injecte du contexte externe au moment de la requête | Modifie les poids internes du modèle |
| Analogie | "Mémoire court terme" / un livre ouvert pendant l'examen | "Mémoire long terme" / des révisions intégrées |
| Mise à jour des données | Facile (mettre à jour le Vector Store) | Difficile (réentraîner) |
| Coût | Plus faible | Plus élevé (GPU, données labellisées) |
| Cas d'usage | Connaissances factuelles, données qui changent souvent | Adapter le style, le format, un comportement spécifique |
| Risque | Dépend de la qualité de la recherche (retrieval) | Risque d'overfitting / de "catastrophic forgetting" |

➡️ En pratique, beaucoup de systèmes en production combinent les deux, mais **RAG est la solution par défaut** pour la majorité des cas d'usage en entreprise (documentation, support client, recherche interne).

---

## 5. Fine-tuning & RLHF

### 5.1 Fine-tuning

Le **Fine-tuning** consiste à continuer l'entraînement d'un modèle déjà pré-entraîné sur un jeu de données spécifique, pour qu'il adapte ses poids internes (**weights**) à une tâche ou un style précis.

Types courants :
- **Full fine-tuning** : tous les poids sont réajustés (coûteux)
- **LoRA / QLoRA (Low-Rank Adaptation)** : on entraîne seulement de petites matrices additionnelles, beaucoup moins coûteux

### 5.2 RLHF (Reinforcement Learning from Human Feedback)

Le **RLHF** est la méthode utilisée par les éditeurs de LLMs (OpenAI, Anthropic, etc.) pour **aligner** le comportement du modèle sur les préférences humaines, après le pré-entraînement initial.

**Principe simplifié :**
```
1. Le modèle génère plusieurs réponses à un même prompt
2. Des évaluateurs humains classent ces réponses (la meilleure, la moins bonne...)
3. Un "Reward Model" apprend à prédire ce classement humain
4. Le LLM est ensuite optimisé (Reinforcement Learning) pour maximiser
   le score donné par ce Reward Model
```

**Pourquoi c'est important à connaître** : c'est ce qui explique pourquoi un LLM "moderne" est plus utile, plus poli et mieux aligné qu'un modèle juste pré-entraîné brut — qui aurait tendance à simplement compléter du texte de façon statistique sans notion d'utilité ou de sécurité.

### 5.3 Quand choisir quoi ?

```
Besoin de connaissances à jour / spécifiques à l'entreprise → RAG
Besoin de changer le STYLE / FORMAT / TON de réponse        → Fine-tuning
Besoin d'améliorer le raisonnement sur une tâche ponctuelle  → Prompt Engineering (CoT)
Besoin de faire AGIR le modèle (appeler une API, une DB)     → Function Calling
```

---

## 6. Function Calling / Tool Use & Agentic AI

### 6.1 Définition

Le **Function Calling** (aussi appelé **Tool Use**) permet à un LLM d'**invoquer des fonctions ou API externes** pour accomplir une tâche qu'il ne peut pas faire seul : récupérer une donnée en temps réel, écrire en base de données, exécuter un calcul exact, etc.

### 6.2 Comment ça fonctionne techniquement

```
1. Le développeur déclare les "tools" disponibles au LLM,
   avec leur nom, description et schéma de paramètres (JSON Schema)

2. Le LLM, en analysant la question utilisateur, décide :
   "Pour répondre, j'ai besoin d'appeler tel outil avec tels paramètres"

3. Le LLM ne répond PAS directement : il renvoie une structure JSON
   indiquant quel outil appeler et avec quels arguments

4. Le code applicatif (PAS le LLM) exécute réellement la fonction

5. Le résultat de la fonction est renvoyé au LLM dans le contexte

6. Le LLM formule sa réponse finale en s'appuyant sur ce résultat
```

**Point important pour un entretien** : le LLM **ne exécute jamais le code lui-même**. Il décide *quoi* appeler ; c'est toujours le code applicatif qui exécute *réellement* l'action. Cette distinction est centrale pour la sécurité.

### 6.3 Agentic AI

Un **Agent IA** est un système qui combine LLM + Function Calling **en boucle** : le modèle peut enchaîner plusieurs appels d'outils, observer les résultats, et décider de l'étape suivante de façon autonome jusqu'à accomplir une tâche complexe (ex. "réserve-moi un billet d'avion" → recherche vols → compare prix → réserve → confirme).

```
Boucle Agentic :
  Observation (résultat précédent) → Raisonnement (CoT) → Action (Tool Call) → Observation → ...
```

C'est la base des outils comme **Claude Code**, **Cursor Agent**, ou des frameworks comme **LangGraph**.

---

## 7. Model Context Protocol (MCP)

### 7.1 Définition

Le **Model Context Protocol (MCP)** est un **standard ouvert**, introduit par Anthropic, qui définit une façon **unifiée et standardisée** pour un LLM de se connecter à des outils, des données et des services externes — au lieu que chaque application réinvente son propre système de Function Calling.

**Analogie utile pour un entretien** : MCP est au LLM ce que **USB-C est aux périphériques** — un connecteur universel, au lieu d'avoir un câble propriétaire différent pour chaque outil.

### 7.2 Architecture MCP

```
┌─────────────┐       MCP Protocol        ┌─────────────┐
│  MCP Host   │ ◄───────────────────────► │  MCP Server │
│ (ex: Claude,│        (JSON-RPC)         │ (ex: GitHub,│
│  IDE, app)  │                           │  DB, Slack) │
└─────────────┘                           └─────────────┘
       │
       └── contient un MCP Client qui gère la connexion
```

- **MCP Host** : l'application qui utilise le LLM (ex. Claude Desktop, ton IDE, ton app Spring Boot)
- **MCP Client** : le composant qui gère la connexion à un serveur MCP spécifique
- **MCP Server** : expose des **tools**, des **resources** (données) et des **prompts** réutilisables à travers le protocole standard

### 7.3 MCP vs Function Calling "classique"

| | Function Calling classique | MCP |
|---|---|---|
| Portée | Spécifique à une intégration / un projet | Standard interopérable entre applications |
| Réutilisabilité | Faible (code à refaire à chaque projet) | Élevée (un serveur MCP sert n'importe quel client MCP) |
| Écosystème | Fermé, propre à chaque app | Ouvert, écosystème partagé (serveurs MCP communautaires) |

### 7.4 Pourquoi c'est stratégique pour Onepoint / un ESN

Dans un contexte d'entreprise, MCP permet de construire **un seul serveur MCP** exposant les données internes (CRM, ticketing, documentation) et de le rendre accessible à **plusieurs assistants IA** (Claude, IDE, chatbot interne) sans dupliquer l'intégration à chaque fois.

---

## 8. Outils IA pour développeurs : Copilot, Cursor, Claude Code

### 8.1 Comparaison des approches

| Outil | Avantage clé |
|---|---|
| **ChatGPT (copier-coller)** | Flexible mais sans contexte projet automatique |
| **GitHub Copilot** | Intégré à l'IDE, suggestions inline en temps réel basées sur le fichier ouvert |
| **Cursor** | IDE complet pensé "AI-first", accès à toute la structure du projet, **Agent mode** capable de modifier plusieurs fichiers |
| **Claude Code** | Agent en ligne de commande, capable d'explorer un repo entier, exécuter des commandes, faire des refactors multi-fichiers de façon autonome |

L'avantage commun de Copilot/Cursor/Claude Code par rapport au simple copier-coller dans un chat : ils ont **accès au contexte du projet** (fichiers ouverts, structure, dépendances) directement, sans que le développeur ait à tout transmettre manuellement.

### 8.2 Bonnes pratiques avec le code généré par IA

Règle d'or testée en entretien : **Review → Comprendre → Tester → Adapter**.

```
❌ Mauvaise pratique : copier-coller directement le code généré sans relecture
✅ Bonne pratique :
   1. Relire le code généré
   2. Comprendre sa logique (pas de "boîte noire")
   3. Le tester (unitairement, fonctionnellement)
   4. L'adapter aux conventions et au contexte du projet
```

**Pourquoi c'est critique** : le code généré par IA peut contenir des erreurs subtiles, des failles de sécurité (ex. injection SQL, mauvaise gestion des secrets), ou simplement ne pas respecter les conventions de l'équipe.

---

## 9. Sécurité & Gouvernance IA en entreprise

### 9.1 Le risque principal : la fuite de données

Quand on envoie du code ou des données à un **LLM public** (ChatGPT grand public, par exemple), le risque majeur est que ces données :
- Soient **journalisées** (logs) côté fournisseur
- Soient potentiellement **utilisées pour l'entraînement** de futurs modèles (selon les conditions d'utilisation du service)

### 9.2 Bonnes pratiques en entreprise

```
❌ À éviter :
   - Coller du code propriétaire dans un LLM grand public sans contrôle
   - Envoyer des clés API, secrets, credentials dans un prompt
   - Envoyer des données clients / données personnelles (RGPD) sans anonymisation

✅ À privilégier :
   - Utiliser des offres "Enterprise" avec garanties contractuelles (Zero Data Retention)
   - Utiliser des modèles déployés en interne / on-premise / VPC privé pour les données sensibles
   - Mettre en place des outils de filtrage / DLP (Data Loss Prevention) avant l'envoi vers une API IA
```

### 9.3 RGPD et IA

En entreprise (surtout en contexte européen), il faut aussi penser à :
- La **base légale** du traitement de données personnelles par un LLM
- Le **droit à l'explication** (en cas de décision automatisée impactant un individu)
- Les clauses contractuelles avec le fournisseur de LLM (sous-traitance de données)

---

## 10. Spring AI : implémenter l'AI Engineering en Java

C'est ici que tout ce que tu as appris devient **concret dans ton stack**. **Spring AI** est le framework officiel de l'écosystème Spring pour l'AI Engineering : il traite un modèle IA comme n'importe quel autre Bean Spring (comme une base de données ou un message broker), avec auto-configuration et starters.

### 10.1 Setup de base

```xml
<!-- pom.xml -->
<dependency>
    <groupId>org.springframework.ai</groupId>
    <artifactId>spring-ai-starter-model-openai</artifactId>
</dependency>
<!-- ou spring-ai-starter-model-anthropic pour Claude -->
```

```yaml
# application.yml
spring:
  ai:
    openai:
      api-key: ${OPENAI_API_KEY}
      chat:
        options:
          model: gpt-4o
```

### 10.2 ChatClient — l'API fluide centrale

```java
@Service
public class AssistantService {

    private final ChatClient chatClient;

    public AssistantService(ChatClient.Builder builder) {
        this.chatClient = builder.build();
    }

    public String repondre(String question) {
        return chatClient.prompt()
                .user(question)
                .call()
                .content();
    }
}
```

```java
@RestController
@RequestMapping("/api/ai")
public class AssistantController {

    private final AssistantService assistantService;

    public AssistantController(AssistantService assistantService) {
        this.assistantService = assistantService;
    }

    @PostMapping("/ask")
    public ResponseEntity<String> ask(@RequestBody String question) {
        return ResponseEntity.ok(assistantService.repondre(question));
    }
}
```

### 10.3 Implémenter du RAG avec Spring AI

```java
@Configuration
public class RagConfig {

    @Bean
    public VectorStore vectorStore(EmbeddingModel embeddingModel) {
        // Exemple avec PgVectorStore (PostgreSQL + extension pgvector)
        return PgVectorStore.builder(jdbcTemplate, embeddingModel)
                .build();
    }
}
```

```java
@Service
public class RagService {

    private final ChatClient chatClient;
    private final VectorStore vectorStore;

    public RagService(ChatClient.Builder builder, VectorStore vectorStore) {
        this.chatClient = builder.build();
        this.vectorStore = vectorStore;
    }

    public void indexerDocument(String contenu) {
        Document document = new Document(contenu);
        vectorStore.add(List.of(document));
    }

    public String repondreAvecContexte(String question) {
        // 1. Retrieval : recherche des chunks pertinents
        List<Document> resultats = vectorStore.similaritySearch(
                SearchRequest.query(question).withTopK(3)
        );

        String contexte = resultats.stream()
                .map(Document::getContent)
                .collect(Collectors.joining("\n\n"));

        // 2. Augmentation + Generation
        return chatClient.prompt()
                .system("Réponds uniquement à partir du contexte fourni : " + contexte)
                .user(question)
                .call()
                .content();
    }
}
```

### 10.4 Function Calling avec Spring AI (`@Tool`)

```java
@Component
public class MeteoTools {

    @Tool(description = "Récupère la météo actuelle pour une ville donnée")
    public String getMeteo(String ville) {
        // Appel réel à une API météo
        return "Il fait 18°C et ensoleillé à " + ville;
    }
}
```

```java
public String repondreAvecOutils(String question) {
    return chatClient.prompt()
            .user(question)
            .tools(new MeteoTools())
            .call()
            .content();
}
// Le modèle décide lui-même d'appeler getMeteo() si la question le nécessite
```

### 10.5 MCP avec Spring AI

Spring AI offre un **support natif MCP** via des starters dédiés, dans les deux sens :

**a) Consommer un serveur MCP existant (ton app = MCP Client) :**

```xml
<dependency>
    <groupId>org.springframework.ai</groupId>
    <artifactId>spring-ai-starter-mcp-client</artifactId>
</dependency>
```

```yaml
spring:
  ai:
    mcp:
      client:
        sse:
          connections:
            github-server:
              url: http://localhost:3001
```

**b) Exposer ton application Spring comme serveur MCP (ton app = MCP Server) :**

```xml
<dependency>
    <groupId>org.springframework.ai</groupId>
    <artifactId>spring-ai-starter-mcp-server-webmvc</artifactId>
</dependency>
```

```java
@Component
public class CommandeTools {

    @Tool(description = "Récupère le statut d'une commande à partir de son ID")
    public String getStatutCommande(String commandeId) {
        // logique métier réelle
        return "Commande " + commandeId + " : en cours de livraison";
    }
}
```

➡️ Avec ça, n'importe quel client MCP (Claude Desktop, un autre agent IA) peut découvrir et appeler `getStatutCommande` automatiquement, sans intégration spécifique.

### 10.6 Architecture type d'une application "AI-Native" en Spring

```
Client (Angular) → REST API (Spring Boot Controller)
                        ↓
                  AI Service Layer (ChatClient + Advisors)
                        ↓
        ┌───────────────┼────────────────┐
        ↓               ↓                ↓
   Vector Store    Tools (@Tool)     MCP Servers
   (RAG / pgvector) (function calling) (systèmes externes)
        ↓               ↓                ↓
              LLM Provider (OpenAI / Anthropic / Ollama local)
```

---

## 11. Q&A Oral – Préparation Entretien (15 questions)

**Q1. Quelle est la différence fondamentale entre RAG et Fine-tuning ?**
R : Le Fine-tuning modifie les poids internes du modèle (mémoire long terme) ; le RAG injecte du contexte externe au moment de la requête sans modifier le modèle (mémoire court terme).

**Q2. Qu'est-ce que le Model Context Protocol (MCP) ?**
R : Un standard ouvert qui permet à un LLM de se connecter de façon unifiée à des outils, données et API externes, plutôt que de réinventer une intégration propriétaire à chaque fois.

**Q3. Pourquoi un LLM peut-il se tromper dans un calcul mathématique simple ?**
R : Parce qu'il prédit le prochain token de façon probabiliste, sans Arithmetic Logic Unit garantissant l'exactitude. Pour des calculs fiables, il faut déléguer à un outil externe via Function Calling.

**Q4. Qu'est-ce que le Function Calling / Tool Use ?**
R : La capacité d'un LLM à invoquer des fonctions ou API externes pour effectuer des actions ou récupérer des données qu'il ne peut pas connaître seul. Le LLM décide quoi appeler, mais c'est toujours le code applicatif qui exécute réellement l'action.

**Q5. Qu'est-ce que le Chain of Thought (CoT) et pourquoi l'utiliser ?**
R : Une technique de prompting qui pousse le modèle à raisonner étape par étape avant de répondre, ce qui améliore significativement la fiabilité sur les tâches complexes (calcul, logique, analyse).

**Q6. Que signifie RLHF ?**
R : Reinforcement Learning from Human Feedback. Une méthode d'alignement où des évaluateurs humains classent les réponses du modèle, ce qui permet d'entraîner un Reward Model puis d'optimiser le LLM par renforcement pour produire des réponses plus utiles et alignées.

**Q7. Quel est le principal risque de sécurité à envoyer du code propriétaire à un LLM public ?**
R : Le code peut être journalisé (logs) côté fournisseur ou potentiellement utilisé pour l'entraînement de futurs modèles, selon les conditions d'utilisation. Il faut privilégier des offres Enterprise avec garanties contractuelles ou des modèles déployés en interne.

**Q8. Quelle est la bonne pratique pour utiliser du code généré par IA ?**
R : Toujours le relire, le comprendre, le tester, puis l'adapter au contexte du projet — jamais le copier-coller en confiance aveugle.

**Q9. Quel est l'avantage de Copilot/Cursor par rapport à un copier-coller dans ChatGPT ?**
R : Ces outils sont intégrés à l'IDE et ont accès directement au contexte du projet (fichiers ouverts, structure, dépendances), sans que le développeur ait à tout transmettre manuellement.

**Q10. Qu'est-ce qu'un embedding ?**
R : Une représentation vectorielle d'un texte qui capture son sens sémantique, permettant de comparer la proximité de sens entre deux textes mathématiquement (similarité cosinus). C'est la brique de base du RAG.

**Q11. Décris les étapes d'un pipeline RAG.**
R : Indexation (chunking des documents, embedding, stockage en Vector Store) puis, au runtime : embedding de la question, recherche de similarité (retrieval), injection des chunks pertinents dans le prompt (augmentation), génération de la réponse par le LLM.

**Q12. Qu'est-ce qu'un agent IA (Agentic AI) ?**
R : Un système combinant LLM et Function Calling en boucle, capable d'enchaîner plusieurs appels d'outils, d'observer les résultats et de décider de façon autonome des étapes suivantes pour accomplir une tâche complexe.

**Q13. Comment Spring AI permet-il d'implémenter du Function Calling ?**
R : Via l'annotation `@Tool` sur une méthode Java, exposée au ChatClient via `.tools(...)`. Le modèle décide d'appeler la méthode si nécessaire, et Spring AI orchestre l'exécution réelle et le renvoi du résultat au modèle.

**Q14. Quelle est la différence entre MCP Host, MCP Client et MCP Server ?**
R : Le Host est l'application qui utilise le LLM (ex. un IDE), il contient un Client qui gère la connexion au protocole, et le Server est le système qui expose des tools/resources/prompts via le standard MCP.

**Q15. Pourquoi MCP est-il stratégique en entreprise par rapport à du Function Calling "fait maison" ?**
R : Parce qu'un seul serveur MCP exposant les données/systèmes internes peut être réutilisé par plusieurs clients IA différents (assistant interne, IDE, chatbot), évitant de dupliquer l'intégration à chaque nouveau projet.

---

## 12. Ressources pour aller plus loin

| Ressource | Pour quoi faire |
|---|---|
| **spring.io/projects/spring-ai** | Doc officielle Spring AI, ChatClient, RAG, MCP, exemples de code |
| **github.com/spring-projects/spring-ai** | Code source, exemples concrets, samples MCP |
| **docs.claude.com** | Guide de prompt engineering et documentation MCP (Anthropic, créateur du protocole) |
| **DeepLearning.AI** (deeplearning.ai) | Short courses gratuits : Prompt Engineering, Function Calling, RAG, MCP |
| **Livre "AI Engineering" – Chip Huyen (O'Reilly)** | Référence théorique complète sur RAG, fine-tuning, évaluation, prompting |
| **start.spring.io** | Générer un projet Spring Boot avec les starters Spring AI directement |

---

*Cours généré pour préparation à un entretien technique Java/Spring orienté AI Engineering (contexte Onepoint).*
