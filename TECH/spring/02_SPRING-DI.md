## [**..**](./00_index.md)

# Java Spring Framework Core Concepts

## 🔗 Loose Coupling vs Tight Coupling

### Tight Coupling (Couplage Fort) - ❌ À Éviter
```java
// Mauvais exemple - dépendance directe
public class GameRunner {
    private MarioGame game = new MarioGame(); // Couplé à MarioGame
    
    public void run() {
        game.up();
        game.down();
    }
}
```
**Problèmes :**
- Impossible de changer de jeu sans modifier le code
- Difficile à tester (on ne peut pas mocker MarioGame)
- Code rigide et non réutilisable

### Loose Coupling (Couplage Faible) - ✅ Recommandé
```java
// Bon exemple - dépendance via interface
public interface Game {
    void up();
    void down();
}

public class GameRunner {
    private Game game; // Dépend de l'interface, pas de l'implémentation
    
    public GameRunner(Game game) {
        this.game = game;
    }
    
    public void run() {
        game.up();
        game.down();
    }
}
```
**Avantages :**
- Facile de changer d'implémentation (MarioGame, PacmanGame...)
- Testable (injection de mocks)
- Code flexible et maintenable

**Spring permet d'obtenir le Loose Coupling grâce à :**
- **Dependency Injection (DI)**
- **Inversion of Control (IoC)**

## 🏭 Spring IoC Container - Inversion of Control

### Concept Principal
**IoC** signifie que Spring gère la création et le cycle de vie des objets (beans) au lieu que ce soit le développeur qui les instancie manuellement avec `new`.

**Sans Spring :**
```java
MarioGame game = new MarioGame();
GameRunner runner = new GameRunner(game);
```

**Avec Spring IoC :**
```java
ApplicationContext context = new AnnotationConfigApplicationContext(AppConfig.class);
GameRunner runner = context.getBean(GameRunner.class);
// Spring a automatiquement créé et injecté les dépendances
```

### Deux Implémentations du Container IoC

#### 1. **BeanFactory** (Interface de base)
- Container IoC le plus simple
- Lazy initialization (beans créés à la demande)
- Moins de fonctionnalités
- Rarement utilisé directement

#### 2. **ApplicationContext** (Recommandé) ⭐
- Étend BeanFactory
- Eager initialization (beans créés au démarrage)
- Fonctionnalités avancées :
  - Internationalisation (i18n)
  - Event publishing
  - Integration avec Spring AOP
  - Web application context

**Types d'ApplicationContext :**
```java
// Configuration par annotations (moderne)
ApplicationContext context = new AnnotationConfigApplicationContext(AppConfig.class);

// Configuration XML (ancienne méthode)
ApplicationContext context = new ClassPathXmlApplicationContext("beans.xml");

// Pour applications web
ApplicationContext context = new WebApplicationContext();
```

**Récupération des beans :**
```java
// Par type
GameRunner runner = context.getBean(GameRunner.class);

// Par nom
GameRunner runner = (GameRunner) context.getBean("gameRunner");

// Par nom et type
GameRunner runner = context.getBean("gameRunner", GameRunner.class);
```

## 🫘 Java Bean vs POJO vs Spring Bean

### POJO (Plain Old Java Object)
```java
public class Person {
    private String name;
    private int age;
    
    // Constructeurs, getters, setters
}
```
- Simple classe Java sans contraintes
- Pas de restrictions particulières
- N'hérite d'aucune classe spécifique
- N'implémente aucune interface obligatoire

### Java Bean (Conventions strictes)
```java
public class Person implements Serializable {
    private String name;
    private int age;
    
    // Constructeur sans arguments obligatoire
    public Person() {}
    
    // Getters/Setters obligatoires
    public String getName() { return name; }
    public void setName(String name) { this.name = name; }
    public int getAge() { return age; }
    public void setAge(int age) { this.age = age; }
}
```
**Règles strictes :**
- ✅ Doit être `Serializable`
- ✅ Constructeur sans arguments
- ✅ Propriétés privées avec getters/setters
- ✅ Conventions de nommage strictes

### Spring Bean
```java
@Component
public class GameRunner {
    // ...
}
```
- **Définition** : Tout objet géré par le Spring IoC Container
- Créé, configuré et géré par Spring
- Peut être un POJO simple
- N'a pas besoin de suivre les conventions Java Bean

**Cycle de vie géré par Spring :**
1. Instantiation
2. Injection des dépendances
3. Initialisation (`@PostConstruct`)
4. Utilisation
5. Destruction (`@PreDestroy`)

## 📦 Spring Framework Modules

Spring Framework est modulaire et contient plusieurs modules :

- **Spring Core** : IoC Container, Dependency Injection
- **Spring MVC** : Applications web (REST APIs, Controllers)
- **Spring JDBC** : Accès aux bases de données
- **Spring ORM** : Intégration JPA, Hibernate
- **Spring AOP** : Aspect-Oriented Programming
- **Spring Test** : Tests unitaires et d'intégration
- **Spring Security** : Authentification et autorisation
- **Spring Data** : Repositories et accès données simplifié

## 💉 Dependency Injection - Les 3 Approches

### 1. Constructor-based Injection (✅ Recommandée)
```java
@Component
public class GameRunner {
    private final Game game;
    
    @Autowired // Optionnel si un seul constructeur
    public GameRunner(Game game) {
        this.game = game;
    }
}
```
**Avantages :**
- Dépendances obligatoires et immuables (`final`)
- Facile à tester
- Thread-safe
- **Spring recommande cette approche**

### 2. Setter-based Injection
```java
@Component
public class GameRunner {
    private Game game;
    
    @Autowired
    public void setGame(Game game) {
        this.game = game;
    }
}
```
**Usage :**
- Dépendances optionnelles
- Permet de changer la dépendance après création
- Moins sûr que Constructor injection

### 3. Field Injection (⚠️ Non recommandée)
```java
@Component
public class GameRunner {
    @Autowired
    private Game game;
}
```
**Inconvénients :**
- Impossible de créer des champs `final`
- Difficile à tester (nécessite réflexion)
- Masque les dépendances
- **À éviter en production**

## 🎯 Annotations Principales

### @Configuration
```java
@Configuration
public class AppConfig {
    @Bean
    public Game game() {
        return new MarioGame();
    }
    
    @Bean
    public GameRunner gameRunner(Game game) {
        return new GameRunner(game);
    }
}
```
- Indique une classe de configuration Spring
- Contient des méthodes `@Bean` pour définir les beans
- Traité par le Spring Container au démarrage
- Remplace la configuration XML

### @Bean
```java
@Bean
public DataSource dataSource() {
    return new HikariDataSource();
}
```
- Déclare qu'une méthode produit un bean géré par Spring
- Utilisé dans les classes `@Configuration`
- Permet un contrôle fin sur la création du bean
- Nom du bean = nom de la méthode (par défaut)

### @Component
```java
@Component
public class MarioGame implements Game {
    // ...
}
```
- Marque une classe comme composant Spring
- Détectée automatiquement par `@ComponentScan`
- Spring crée automatiquement un bean
- Annotations spécialisées : `@Service`, `@Repository`, `@Controller`

### @ComponentScan
```java
@Configuration
@ComponentScan("com.example.game")
public class AppConfig {
    // Spring scanne le package pour trouver les @Component
}
```
- Active le scan automatique des composants
- Cherche `@Component`, `@Service`, `@Repository`, `@Controller`
- Enregistre automatiquement les beans trouvés
- Scan récursif des sous-packages

### @Autowired
```java
@Component
public class GameRunner {
    @Autowired
    private Game game; // Spring injecte automatiquement
}
```
- Injection automatique des dépendances par Spring
- Peut être utilisé sur : constructeurs, setters, champs
- Recherche un bean compatible par type
- Optionnel sur les constructeurs (depuis Spring 4.3)

### @Primary
```java
@Component
@Primary
public class MarioGame implements Game {
    // Si plusieurs implémentations de Game existent,
    // MarioGame sera injecté par défaut
}
```
- Donne la priorité à un bean quand plusieurs candidats existent
- Résout les ambiguïtés d'autowiring
- Utilisé quand il y a plusieurs beans du même type
- Un seul `@Primary` par type

### @Qualifier
```java
@Component
@Qualifier("mario")
public class MarioGame implements Game { }

@Component
@Qualifier("pacman")
public class PacmanGame implements Game { }

// Injection avec qualification
@Component
public class GameRunner {
    @Autowired
    @Qualifier("mario")
    private Game game; // Force l'injection de MarioGame
}
```
- Spécifie quel bean injecter quand plusieurs candidats existent
- Plus précis que `@Primary`
- Utilisé avec `@Autowired` pour lever l'ambiguïté
- Peut être utilisé sur champs, paramètres, méthodes

**Différence @Primary vs @Qualifier :**
- **@Primary** : Choix par défaut global
- **@Qualifier** : Choix explicite au point d'injection

### @Lazy
```java
@Component
@Lazy
public class HeavyService {
    public HeavyService() {
        System.out.println("Heavy initialization...");
    }
}
```
- Initialisation différée du bean
- Bean créé uniquement lors de la première utilisation
- Par défaut : beans créés au démarrage (eager)
- Utile pour beans lourds rarement utilisés

### @Scope
```java
@Component
@Scope(ConfigurableBeanFactory.SCOPE_PROTOTYPE)
public class PrototypeBean {
    // Nouvelle instance à chaque injection
}

@Component
@Scope(ConfigurableBeanFactory.SCOPE_SINGLETON) // Par défaut
public class SingletonBean {
    // Une seule instance dans le container
}
```

**Scopes disponibles :**
- **SINGLETON** (défaut) : Une instance par IoC Container
- **PROTOTYPE** : Nouvelle instance à chaque demande
- **REQUEST** : Une instance par requête HTTP (Web)
- **SESSION** : Une instance par session HTTP (Web)
- **APPLICATION** : Une instance par ServletContext (Web)

### @PostConstruct
```java
@Component
public class DatabaseConnection {
    @PostConstruct
    public void init() {
        System.out.println("Connecting to database...");
        // Exécuté après injection des dépendances
    }
}
```
- Méthode exécutée après l'injection des dépendances
- Utilisé pour l'initialisation du bean
- Appelé automatiquement par Spring
- Alternative : `InitializingBean` interface

### @PreDestroy
```java
@Component
public class DatabaseConnection {
    @PreDestroy
    public void cleanup() {
        System.out.println("Closing database connection...");
        // Exécuté avant destruction du bean
    }
}
```
- Méthode exécutée avant destruction du bean
- Utilisé pour le nettoyage des ressources
- Appelé lors de l'arrêt du container
- Alternative : `DisposableBean` interface

## 🌐 Jakarta CDI Annotations (Standard Java EE)

### @Named (équivalent à @Component)
```java
@Named
public class MarioGame implements Game {
    // Bean CDI, compatible avec Spring
}
```
- Annotation Jakarta CDI
- Équivalent à `@Component` de Spring
- Standard Java EE/Jakarta EE
- Spring le supporte pour la compatibilité

### @Inject (équivalent à @Autowired)
```java
@Named
public class GameRunner {
    @Inject
    private Game game; // Injection CDI
}
```
- Annotation Jakarta CDI pour injection
- Équivalent à `@Autowired` de Spring
- Standard plus portable
- Pas d'attribut `required`

**Spring vs Jakarta CDI :**
- Spring : `@Component` + `@Autowired`
- Jakarta : `@Named` + `@Inject`
- Spring supporte les deux pour compatibilité

## 🔄 @Component vs @Bean - Quand utiliser lequel ?

### @Component (Scan automatique)
```java
@Component
public class MarioGame implements Game {
    // Spring crée automatiquement le bean
}
```
**Utiliser @Component quand :**
- ✅ Vous contrôlez le code source de la classe
- ✅ Configuration simple sans logique complexe
- ✅ Scan automatique souhaité
- ✅ Classes de votre application

**Exemples :** Services, Repositories, Controllers

### @Bean (Configuration explicite)
```java
@Configuration
public class AppConfig {
    @Bean
    public DataSource dataSource() {
        HikariConfig config = new HikariConfig();
        config.setJdbcUrl("jdbc:mysql://localhost/db");
        // Configuration complexe
        return new HikariDataSource(config);
    }
}
```
**Utiliser @Bean quand :**
- ✅ Classes tierces (librairies externes)
- ✅ Configuration complexe nécessaire
- ✅ Logique de création personnalisée
- ✅ Conditions à évaluer avant création
- ✅ Contrôle fin requis

**Exemples :** DataSource, RestTemplate, ObjectMapper

## 📝 Configuration XML vs Annotations

### Configuration XML (Ancienne méthode)

**beans.xml :**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xsi:schemaLocation="http://www.springframework.org/schema/beans
       http://www.springframework.org/schema/beans/spring-beans.xsd">

    <!-- Définition des beans -->
    <bean id="marioGame" class="com.example.MarioGame"/>
    
    <bean id="gameRunner" class="com.example.GameRunner">
        <!-- Constructor injection -->
        <constructor-arg ref="marioGame"/>
    </bean>
    
    <!-- Setter injection -->
    <bean id="person" class="com.example.Person">
        <property name="name" value="Mario"/>
        <property name="age" value="30"/>
    </bean>
    
    <!-- Component scan en XML -->
    <context:component-scan base-package="com.example"/>
</beans>
```

**Chargement :**
```java
ApplicationContext context = new ClassPathXmlApplicationContext("beans.xml");
GameRunner runner = context.getBean(GameRunner.class);
```

### Configuration par Annotations (Moderne) ⭐

```java
@Configuration
@ComponentScan("com.example")
public class AppConfig {
    
    @Bean
    public Game game() {
        return new MarioGame();
    }
    
    @Bean
    public GameRunner gameRunner(Game game) {
        return new GameRunner(game);
    }
}
```

**Chargement :**
```java
ApplicationContext context = new AnnotationConfigApplicationContext(AppConfig.class);
GameRunner runner = context.getBean(GameRunner.class);
```

### Comparaison XML vs Annotations

| Critère | XML | Annotations |
|---------|-----|-------------|
| **Lisibilité** | Verbeux | Concis |
| **Maintenance** | Difficile | Facile |
| **Type-safety** | ❌ Non | ✅ Oui |
| **Refactoring** | Risqué | Sûr (IDE) |
| **Séparation config/code** | ✅ Oui | ❌ Non |
| **Apprentissage** | Complexe | Simple |
| **Standard moderne** | ❌ Obsolète | ✅ Actuel |

**Recommandation :** Utiliser les annotations pour les nouveaux projets. XML n'est plus utilisé sauf pour maintenir du code legacy.

### Configuration Hybride (Si nécessaire)
```java
@Configuration
@ImportResource("classpath:legacy-beans.xml") // Import XML
@ComponentScan("com.example")
public class AppConfig {
    @Bean
    public Game game() {
        return new MarioGame();
    }
}
```

## 📊 Résumé des Bonnes Pratiques

✅ **À FAIRE :**
- Préférer Constructor Injection
- Utiliser `@Component` pour vos classes
- Utiliser `@Bean` pour classes tierces
- Configuration par annotations
- Un seul `@Primary` par type
- `@Qualifier` pour lever les ambiguïtés
- `@Lazy` pour beans lourds
- `@PostConstruct`/`@PreDestroy` pour lifecycle

❌ **À ÉVITER :**
- Field Injection
- Configuration XML (sauf legacy)
- Multiples `@Primary` pour même type
- Scope PROTOTYPE sans raison
- Beans mutables dans Singleton scope




# Questions & Réponses - Java Spring Framework Core

## 🔗 Loose Coupling vs Tight Coupling

**Q1 : Qu'est-ce que le Tight Coupling et pourquoi est-ce un problème ?**
**R :** Le Tight Coupling (couplage fort) signifie qu'une classe dépend directement d'une implémentation concrète plutôt que d'une abstraction. Exemple : `private MarioGame game = new MarioGame();`. Problèmes : impossible de changer l'implémentation sans modifier le code, difficile à tester, code rigide et non réutilisable.

**Q2 : Comment Spring aide-t-il à obtenir le Loose Coupling ?**
**R :** Spring utilise l'Inversion of Control (IoC) et la Dependency Injection pour obtenir le Loose Coupling. Au lieu d'instancier les dépendances avec `new`, Spring les injecte automatiquement via des interfaces, permettant de changer facilement d'implémentation sans modifier le code.

**Q3 : Donnez un exemple concret de transformation d'un code Tight Coupling vers Loose Coupling**
**R :** 
- **Tight Coupling :** `private MarioGame game = new MarioGame();` - dépend directement de MarioGame
- **Loose Coupling :** `private Game game;` avec injection par constructeur - dépend de l'interface Game, l'implémentation est injectée par Spring

## 🏭 Spring IoC Container

**Q4 : Qu'est-ce que l'Inversion of Control (IoC) ?**
**R :** IoC signifie que le contrôle de la création et gestion des objets est inversé : au lieu que le développeur crée les objets avec `new`, c'est le framework Spring (IoC Container) qui les crée, les configure et gère leur cycle de vie.

**Q5 : Quelle est la différence entre BeanFactory et ApplicationContext ?**
**R :**
- **BeanFactory** : Container IoC basique, lazy initialization (beans créés à la demande), fonctionnalités limitées
- **ApplicationContext** : Étend BeanFactory, eager initialization (beans créés au démarrage), fonctionnalités avancées (i18n, events, AOP). ApplicationContext est recommandé pour tous les cas d'usage.

**Q6 : Comment récupérer un bean depuis l'ApplicationContext ?**
**R :**
```java
// Par type
GameRunner runner = context.getBean(GameRunner.class);

// Par nom
GameRunner runner = (GameRunner) context.getBean("gameRunner");

// Par nom et type (recommandé)
GameRunner runner = context.getBean("gameRunner", GameRunner.class);
```

**Q7 : Quels sont les types d'ApplicationContext disponibles ?**
**R :**
- **AnnotationConfigApplicationContext** : Configuration par annotations (moderne)
- **ClassPathXmlApplicationContext** : Configuration XML (legacy)
- **WebApplicationContext** : Pour applications web

## 🫘 Java Bean vs POJO vs Spring Bean

**Q8 : Quelle est la différence entre POJO, Java Bean et Spring Bean ?**
**R :**
- **POJO** : Simple classe Java sans contraintes ni règles spécifiques
- **Java Bean** : POJO suivant des conventions strictes (Serializable, constructeur vide, getters/setters)
- **Spring Bean** : Tout objet géré par le Spring IoC Container, peut être un simple POJO

**Q9 : Quelles sont les règles strictes d'un Java Bean ?**
**R :**
1. Doit implémenter `Serializable`
2. Constructeur sans arguments obligatoire
3. Propriétés privées avec getters/setters publics
4. Conventions de nommage (getName, setName)

**Q10 : Un Spring Bean doit-il suivre les conventions Java Bean ?**
**R :** Non, un Spring Bean peut être n'importe quelle classe Java (POJO). Il n'a pas besoin de suivre les conventions Java Bean strictes. Spring gère juste son cycle de vie.

## 📦 Spring Modules

**Q11 : Citez 5 modules principaux du Spring Framework**
**R :**
1. **Spring Core** : IoC Container, Dependency Injection
2. **Spring MVC** : Applications web, REST APIs
3. **Spring JDBC** : Accès bases de données simplifié
4. **Spring Security** : Authentification et autorisation
5. **Spring Test** : Support tests unitaires et d'intégration

## 💉 Dependency Injection

**Q12 : Quelles sont les 3 approches de Dependency Injection dans Spring ?**
**R :**
1. **Constructor-based Injection** : Via constructeur (✅ recommandée)
2. **Setter-based Injection** : Via méthodes setters
3. **Field Injection** : Directement sur les champs avec @Autowired (⚠️ déconseillée)

**Q13 : Pourquoi Constructor Injection est-elle recommandée ?**
**R :** 
- Dépendances obligatoires et immuables (champs `final`)
- Thread-safe et facile à tester
- Rend les dépendances explicites
- Impossible d'avoir un objet dans un état invalide
- Recommandation officielle Spring

**Q14 : Quels sont les inconvénients du Field Injection ?**
**R :**
- Impossible d'utiliser `final` (dépendances mutables)
- Difficile à tester (nécessite réflexion)
- Masque les dépendances de la classe
- Impossible d'instancier la classe sans Spring
- À éviter en production

**Q15 : Quand utiliser Setter Injection ?**
**R :** Pour des dépendances optionnelles ou quand on veut permettre la reconfiguration du bean après sa création. Constructor Injection reste préférable pour les dépendances obligatoires.

## 🎯 Annotations Core

**Q16 : Quelle est la différence entre @Configuration et @Component ?**
**R :**
- **@Configuration** : Classe de configuration contenant des méthodes @Bean pour créer des beans programmatiquement
- **@Component** : Marque la classe elle-même comme bean, détectée par component scan

**Q17 : Expliquez le rôle de @Bean**
**R :** @Bean est utilisé dans une classe @Configuration pour déclarer qu'une méthode produit un bean géré par Spring. Permet un contrôle fin sur la création du bean. Utilisé principalement pour des classes tierces ou configuration complexe.

**Q18 : Qu'est-ce que @ComponentScan et comment fonctionne-t-il ?**
**R :** @ComponentScan active le scan automatique des packages pour détecter les classes annotées avec @Component, @Service, @Repository, @Controller. Spring les enregistre automatiquement comme beans. Le scan est récursif sur les sous-packages.

**Q19 : @Autowired est-il obligatoire sur les constructeurs ?**
**R :** Non, depuis Spring 4.3, @Autowired est optionnel si la classe n'a qu'un seul constructeur. Spring l'utilise automatiquement pour l'injection.

## 🎯 @Primary vs @Qualifier

**Q20 : Quelle est la différence entre @Primary et @Qualifier ?**
**R :**
- **@Primary** : Définit le bean par défaut quand plusieurs candidats existent. Choix global.
- **@Qualifier** : Spécifie explicitement quel bean injecter au point d'injection. Choix local et précis.

**Q21 : Quand utiliser @Primary ?**
**R :** Quand vous avez plusieurs implémentations d'une interface et qu'une est clairement le choix par défaut pour la majorité des cas. Exemple : base de données principale vs secondaire.

**Q22 : Comment utiliser @Qualifier avec @Autowired ?**
**R :**
```java
@Component
@Qualifier("mario")
public class MarioGame implements Game { }

@Autowired
@Qualifier("mario")
private Game game; // Force injection de MarioGame
```

**Q23 : Peut-on avoir plusieurs @Primary pour le même type ?**
**R :** Non, un seul @Primary par type est autorisé, sinon Spring lève une exception d'ambiguïté. Utilisez @Qualifier pour des choix multiples.

## 📊 Scopes et Lifecycle

**Q24 : Quels sont les principaux Bean Scopes dans Spring ?**
**R :**
- **SINGLETON** (défaut) : Une instance par IoC Container
- **PROTOTYPE** : Nouvelle instance à chaque demande
- **REQUEST** : Une instance par requête HTTP (Web)
- **SESSION** : Une instance par session HTTP (Web)
- **APPLICATION** : Une instance par ServletContext (Web)

**Q25 : Quelle est la différence entre Singleton et Prototype scope ?**
**R :**
- **Singleton** : Spring crée une seule instance réutilisée partout (défaut)
- **Prototype** : Spring crée une nouvelle instance à chaque injection ou getBean()

**Q26 : Qu'est-ce que @Lazy et quand l'utiliser ?**
**R :** @Lazy indique à Spring de ne créer le bean que lors de sa première utilisation plutôt qu'au démarrage. Utile pour des beans lourds rarement utilisés ou pour éviter des dépendances circulaires.

**Q27 : Expliquez @PostConstruct et @PreDestroy**
**R :**
- **@PostConstruct** : Méthode exécutée après l'injection des dépendances, utilisée pour l'initialisation
- **@PreDestroy** : Méthode exécutée avant destruction du bean, utilisée pour le nettoyage des ressources

**Q28 : Dans quel ordre s'exécute le cycle de vie d'un bean ?**
**R :**
1. Instantiation du bean
2. Injection des dépendances (@Autowired)
3. @PostConstruct
4. Bean prêt à l'utilisation
5. @PreDestroy (lors de l'arrêt)
6. Destruction

## 🌐 Jakarta CDI

**Q29 : Quelle est la différence entre les annotations Spring et Jakarta CDI ?**
**R :**
- **Spring** : @Component + @Autowired (spécifique Spring)
- **Jakarta CDI** : @Named + @Inject (standard Java EE/Jakarta)
Spring supporte les deux pour la portabilité.

**Q30 : Pourquoi utiliser @Named et @Inject au lieu de @Component et @Autowired ?**
**R :** Pour la portabilité. @Named et @Inject sont des standards Jakarta EE fonctionnant avec différents containers (Spring, CDI Weld, etc.). Utilisez-les si votre code doit être portable entre frameworks.

## 🔄 @Component vs @Bean

**Q31 : Quand utiliser @Component vs @Bean ?**
**R :**
- **@Component** : Pour vos propres classes (code source contrôlé), détection automatique
- **@Bean** : Pour classes tierces (librairies), configuration complexe, logique de création personnalisée

**Q32 : Peut-on combiner @Component et @Bean dans la même application ?**
**R :** Oui, c'est même recommandé. Utilisez @Component pour vos services/repositories, et @Bean dans @Configuration pour DataSource, RestTemplate, ObjectMapper, etc.

**Q33 : Donnez un exemple typique d'utilisation de @Bean**
**R :**
```java
@Configuration
public class AppConfig {
    @Bean
    public DataSource dataSource() {
        HikariConfig config = new HikariConfig();
        config.setJdbcUrl("jdbc:mysql://localhost/db");
        config.setUsername("user");
        config.setPassword("pass");
        return new HikariDataSource(config);
    }
}
```
Utilisé car HikariDataSource est une classe tierce nécessitant une configuration complexe.

## 📝 Configuration XML vs Annotations

**Q34 : Quels sont les avantages des annotations par rapport à XML ?**
**R :**
- Code plus concis et lisible
- Type-safe (erreurs détectées à la compilation)
- Refactoring sûr avec l'IDE
- Maintenance plus facile
- Standard moderne (XML est obsolète)

**Q35 : Y a-t-il des cas où XML est préférable ?**
**R :** Presque jamais pour les nouveaux projets. XML peut être utilisé uniquement pour maintenir du code legacy ou si vous voulez une séparation stricte configuration/code (très rare aujourd'hui).

**Q36 : Comment migrer d'une configuration XML vers annotations ?**
**R :**
1. Créer une classe @Configuration
2. Remplacer `<bean>` par @Bean ou @Component
3. Remplacer `<context:component-scan>` par @ComponentScan
4. Remplacer ClassPathXmlApplicationContext par AnnotationConfigApplicationContext

**Q37 : Exemple de bean XML équivalent en annotations ?**
**R :**
**XML :**
```xml
<bean id="gameRunner" class="com.example.GameRunner">
    <constructor-arg ref="marioGame"/>
</bean>
```
**Annotations :**
```java
@Component
public class GameRunner {
    @Autowired
    public GameRunner(Game game) { }
}
```

## 🎓 Questions Avancées

**Q38 : Que se passe-t-il si Spring trouve plusieurs beans du même type sans @Primary ni @Qualifier ?**
**R :** Spring lève une `NoUniqueBeanDefinitionException` au démarrage car il ne peut pas déterminer quel bean injecter. Solution : ajouter @Primary sur un bean ou utiliser @Qualifier au point d'injection.

**Q39 : Peut-on injecter un bean Prototype dans un bean Singleton ?**
**R :** Oui, mais attention : le Prototype sera créé une seule fois lors de l'injection dans le Singleton. Pour obtenir une nouvelle instance à chaque utilisation, utilisez `ApplicationContext.getBean()` ou un Provider.

**Q40 : Quelle est la différence entre @Component, @Service, @Repository et @Controller ?**
**R :**
- **@Component** : Annotation générique pour tout composant Spring
- **@Service** : Spécialisation pour la couche service (logique métier)
- **@Repository** : Spécialisation pour la couche DAO (accès données), ajoute la traduction d'exceptions
- **@Controller** : Spécialisation pour les contrôleurs MVC
Techniquement identiques, mais améliorent la lisibilité et permettent des traitements spécifiques.

**Q41 : Comment tester une classe avec Dependency Injection ?**
**R :** Avec Constructor Injection, c'est simple :
```java
// Test unitaire
Game mockGame = Mockito.mock(Game.class);
GameRunner runner = new GameRunner(mockGame); // Pas besoin de Spring
```
C'est pourquoi Constructor Injection est recommandée pour la testabilité.

**Q42 : Que fait context.getBean() exactement ?**
**R :** `context.getBean()` récupère un bean depuis l'IoC Container Spring. Pour un Singleton, retourne toujours la même instance. Pour un Prototype, crée une nouvelle instance à chaque appel.

**Q43 : Quand le ApplicationContext est-il créé dans une application Spring Boot ?**
**R :** Lors du démarrage de l'application avec `SpringApplication.run()`. Spring scanne les composants, crée les beans, injecte les dépendances, puis l'application est prête.

**Q44 : Peut-on avoir plusieurs classes @Configuration ?**
**R :** Oui, on peut avoir plusieurs classes @Configuration. Spring les traite toutes. Utilisez @Import pour référencer explicitement d'autres configurations si nécessaire.

**Q45 : Quelle est la meilleure pratique : une seule @Configuration ou plusieurs ?**
**R :** Plusieurs @Configuration organisées par domaine fonctionnel (DatabaseConfig, SecurityConfig, WebConfig). Améliore la lisibilité et la maintenance.