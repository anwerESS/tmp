## [**..**](./00_index.md)

# Chapter 12: Modules

## 1. Introducing Modules

**Module** (Java 9+) est une unité de code et de données qui définit explicitement ses dépendances et ce qu'il expose.

### Problèmes avant les Modules

```java
// AVANT Java 9 - Classpath
// Problèmes:
// 1. Pas d'encapsulation forte (tout est public)
// 2. Pas de gestion explicite des dépendances
// 3. "JAR Hell" - conflits de versions
// 4. Classes publiques accessibles partout
// 5. Pas de vérification à la compilation

// Exemple
public class MyClass {
    // public = accessible de PARTOUT (même si non souhaité)
}
```

### Avantages des Modules

```java
// APRÈS Java 9 - Module System
// Avantages:
// 1. Encapsulation forte (exports explicites)
// 2. Dépendances explicites (requires)
// 3. Services clairement définis
// 4. Vérification à la compilation
// 5. Sécurité renforcée
// 6. Performance améliorée

// module-info.java
module com.myapp {
    exports com.myapp.api;      // Seulement ce package est public
    requires java.sql;          // Dépendance explicite
}
```

---

## 2. Creating and Running a Modular Program

### Structure de Base

```
myapp/
├── src/
│   └── com.myapp/
│       ├── module-info.java
│       └── com/
│           └── myapp/
│               └── Main.java
└── mods/
    └── com.myapp.jar
```

### Module Descriptor (module-info.java)

```java
// module-info.java - TOUJOURS à la racine du module
module com.myapp {
    // Déclarations du module
}

// Exemple complet
module com.myapp {
    // Exporte un package (accessible aux autres modules)
    exports com.myapp.api;
    
    // Require une dépendance
    requires java.sql;
    
    // Require transitif (propage la dépendance)
    requires transitive java.logging;
    
    // Ouvre pour reflection
    opens com.myapp.internal;
}
```

### Exemple Simple

```java
// src/com.myapp/module-info.java
module com.myapp {
    // Module simple sans dépendances
}

// src/com.myapp/com/myapp/Main.java
package com.myapp;

public class Main {
    public static void main(String[] args) {
        System.out.println("Hello from module!");
    }
}

// Compiler
javac -d mods/com.myapp \
    src/com.myapp/module-info.java \
    src/com.myapp/com/myapp/Main.java

// Créer JAR
jar --create --file mods/com.myapp.jar \
    -C mods/com.myapp .

// Exécuter
java --module-path mods \
     --module com.myapp/com.myapp.Main
```

---

## 3. Updating Our Example for Multiple Modules

### Structure Multi-Modules

```
project/
├── com.myapp.api/
│   ├── module-info.java
│   └── com/myapp/api/
│       └── Service.java
├── com.myapp.impl/
│   ├── module-info.java
│   └── com/myapp/impl/
│       └── ServiceImpl.java
└── com.myapp.client/
    ├── module-info.java
    └── com/myapp/client/
        └── Client.java
```

### Module API

```java
// com.myapp.api/module-info.java
module com.myapp.api {
    exports com.myapp.api;  // Expose l'API publique
}

// com.myapp.api/com/myapp/api/Service.java
package com.myapp.api;

public interface Service {
    String process(String input);
}
```

### Module Implementation

```java
// com.myapp.impl/module-info.java
module com.myapp.impl {
    requires com.myapp.api;     // Dépend de l'API
    exports com.myapp.impl;     // Expose l'implémentation
}

// com.myapp.impl/com/myapp/impl/ServiceImpl.java
package com.myapp.impl;

import com.myapp.api.Service;

public class ServiceImpl implements Service {
    @Override
    public String process(String input) {
        return input.toUpperCase();
    }
}
```

### Module Client

```java
// com.myapp.client/module-info.java
module com.myapp.client {
    requires com.myapp.api;      // Dépend de l'API
    requires com.myapp.impl;     // Dépend de l'implémentation
}

// com.myapp.client/com/myapp/client/Client.java
package com.myapp.client;

import com.myapp.api.Service;
import com.myapp.impl.ServiceImpl;

public class Client {
    public static void main(String[] args) {
        Service service = new ServiceImpl();
        System.out.println(service.process("hello"));  // "HELLO"
    }
}
```

### Compilation et Exécution

```bash
# Compiler API
javac -d mods/com.myapp.api \
    com.myapp.api/module-info.java \
    com.myapp.api/com/myapp/api/Service.java

# Compiler Implementation (dépend d'API)
javac --module-path mods \
    -d mods/com.myapp.impl \
    com.myapp.impl/module-info.java \
    com.myapp.impl/com/myapp/impl/ServiceImpl.java

# Compiler Client (dépend d'API et Implementation)
javac --module-path mods \
    -d mods/com.myapp.client \
    com.myapp.client/module-info.java \
    com.myapp.client/com/myapp/client/Client.java

# Exécuter
java --module-path mods \
     --module com.myapp.client/com.myapp.client.Client
```

---

## 4. Diving into the Module Declaration

### exports

```java
module com.myapp {
    // Export simple - accessible à TOUS les modules
    exports com.myapp.api;
    
    // Export qualifié - accessible seulement à certains modules
    exports com.myapp.internal to com.myapp.client, com.myapp.test;
}

// Exemple
// com.myapp.api.Service - accessible partout
// com.myapp.internal.Helper - accessible seulement à client et test
```

### requires

```java
module com.myapp {
    // Require simple - dépendance normale
    requires java.sql;
    
    // Require transitive - propage la dépendance
    requires transitive java.logging;
    
    // Require static - optionnel (compile-time seulement)
    requires static lombok;
    
    // Require transitive + static
    requires transitive static annotations;
}

// Différence requires vs requires transitive:
// Module A: requires transitive java.sql
// Module B: requires A
// → B a automatiquement accès à java.sql (grâce à transitive)

// Sans transitive:
// Module A: requires java.sql
// Module B: requires A
// → B doit déclarer "requires java.sql" explicitement
```

### opens

```java
module com.myapp {
    // Open package - accessible via reflection
    opens com.myapp.internal;
    
    // Open à modules spécifiques
    opens com.myapp.model to hibernate, jackson;
}

// Différence exports vs opens:
// exports: accessible pour code normal
// opens: accessible pour reflection (mais pas code normal)

// Exemple
package com.myapp.internal;

public class Config {  // Pas exporté
    private String value;
    
    // Avec "opens", frameworks peuvent accéder via reflection
    // Sans "opens", IllegalAccessException
}

// Open module complet
open module com.myapp {
    // TOUS les packages sont open (reflection)
    // Mais pas exportés (pas accessible normalement)
}
```

### uses et provides

```java
// Service Provider Interface (SPI)

// Module API
module com.myapp.api {
    exports com.myapp.api;
    uses com.myapp.api.Service;  // Déclare utiliser le service
}

// Module Implementation
module com.myapp.impl {
    requires com.myapp.api;
    provides com.myapp.api.Service 
        with com.myapp.impl.ServiceImpl;  // Fournit l'implémentation
}

// Code API
package com.myapp.api;

public interface Service {
    String process(String input);
}

// Code Implementation
package com.myapp.impl;

import com.myapp.api.Service;

public class ServiceImpl implements Service {
    @Override
    public String process(String input) {
        return input.toUpperCase();
    }
}

// Code Client
import java.util.ServiceLoader;
import com.myapp.api.Service;

ServiceLoader<Service> loader = ServiceLoader.load(Service.class);
for (Service service : loader) {
    System.out.println(service.process("hello"));
}
```

---

## 5. Creating a Service

### Exemple Complet de Service

**Module API:**
```java
// com.myapp.spi/module-info.java
module com.myapp.spi {
    exports com.myapp.spi;
    uses com.myapp.spi.Calculator;  // Déclare utiliser
}

// com.myapp.spi/com/myapp/spi/Calculator.java
package com.myapp.spi;

public interface Calculator {
    int calculate(int a, int b);
}
```

**Module Implementation 1:**
```java
// com.myapp.add/module-info.java
module com.myapp.add {
    requires com.myapp.spi;
    provides com.myapp.spi.Calculator 
        with com.myapp.add.AddCalculator;
}

// com.myapp.add/com/myapp/add/AddCalculator.java
package com.myapp.add;

import com.myapp.spi.Calculator;

public class AddCalculator implements Calculator {
    @Override
    public int calculate(int a, int b) {
        return a + b;
    }
}
```

**Module Implementation 2:**
```java
// com.myapp.multiply/module-info.java
module com.myapp.multiply {
    requires com.myapp.spi;
    provides com.myapp.spi.Calculator 
        with com.myapp.multiply.MultiplyCalculator;
}

// com.myapp.multiply/com/myapp/multiply/MultiplyCalculator.java
package com.myapp.multiply;

import com.myapp.spi.Calculator;

public class MultiplyCalculator implements Calculator {
    @Override
    public int calculate(int a, int b) {
        return a * b;
    }
}
```

**Module Client:**
```java
// com.myapp.client/module-info.java
module com.myapp.client {
    requires com.myapp.spi;
    uses com.myapp.spi.Calculator;  // Utilise le service
}

// com.myapp.client/com/myapp/client/Client.java
package com.myapp.client;

import java.util.ServiceLoader;
import com.myapp.spi.Calculator;

public class Client {
    public static void main(String[] args) {
        ServiceLoader<Calculator> loader = ServiceLoader.load(Calculator.class);
        
        // Trouve TOUTES les implémentations disponibles
        for (Calculator calc : loader) {
            System.out.println(calc.getClass().getName());
            System.out.println("5 + 3 = " + calc.calculate(5, 3));
        }
    }
}
// Output:
// com.myapp.add.AddCalculator
// 5 + 3 = 8
// com.myapp.multiply.MultiplyCalculator
// 5 + 3 = 15
```

---

## 6. Discovering Modules

### Commandes de Diagnostic

```bash
# Lister modules disponibles
java --list-modules

# java.base@17.0.1
# java.sql@17.0.1
# com.myapp@1.0
# ...

# Décrire un module
java --describe-module java.sql

# java.sql@17.0.1
# exports java.sql
# exports javax.sql
# requires java.base
# requires java.logging transitive
# requires java.xml
# uses java.sql.Driver

# Décrire un module custom
jar --describe-module --file mods/com.myapp.jar

# Afficher les dépendances (module graph)
jdeps --module-path mods -s mods/com.myapp.jar

# com.myapp -> java.base
# com.myapp -> java.sql

# Analyser les dépendances en détail
jdeps --module-path mods mods/com.myapp.jar

# Vérifier les modules manquants
jdeps --check mods/com.myapp.jar
```

### Module Resolution

```java
// Java résout les modules au démarrage
// 1. Module root (--module)
// 2. Ses dépendances (requires)
// 3. Dépendances transitives (requires transitive)
// 4. Services (uses/provides)

// Exemple
module com.myapp.client {
    requires com.myapp.api;          // Direct
}

module com.myapp.api {
    requires transitive java.logging; // Transitive
}

// → com.myapp.client a accès à java.logging (via transitive)

// Vérifier la résolution
java --show-module-resolution \
     --module-path mods \
     --module com.myapp.client/com.myapp.client.Client
```

---

## 7. Comparing Types of Modules

### Named Module

```java
// Module avec module-info.java
module com.myapp {
    exports com.myapp.api;
    requires java.sql;
}

// Caractéristiques:
// - module-info.java présent
// - Exports/requires explicites
// - Forte encapsulation
// - Sur module path
```

### Automatic Module

```java
// JAR sans module-info.java sur module path
// Devient automatiquement un module

// Nom dérivé du JAR filename
// commons-lang3-3.12.0.jar → commons.lang3

// Caractéristiques:
// - Pas de module-info.java
// - Exports TOUS les packages
// - Requires TOUS les autres modules
// - Pas d'encapsulation forte
// - Transition vers modules

// Utilisation
module com.myapp {
    requires commons.lang3;  // Automatic module
}
```

### Unnamed Module

```java
// JAR sur classpath (ancien système)
// Toutes les classes non modulaires sur classpath

// Caractéristiques:
// - Sur classpath (pas module path)
// - Peut lire tous les modules
// - Pas exporté vers modules named
// - Backward compatibility

// Exemple
java --class-path libs/* \
     --module-path mods \
     --add-modules ALL-MODULE-PATH \
     com.myapp.Main

// Unnamed module peut utiliser:
// - Autres classes unnamed
// - Tous les modules exportés
```

### Tableau Comparatif

| **Type** | **module-info** | **Path** | **Exports** | **Reads** |
|----------|----------------|----------|-------------|-----------|
| Named | Oui | Module path | Explicite | Explicite |
| Automatic | Non | Module path | Tout | Tout |
| Unnamed | Non | Classpath | Rien | Tout |

---

## 8. Migrating an Application

### Stratégie de Migration

**Étape 1: Bottom-Up (recommandé)**
```
Classpath:              Module Path:
┌─────────────┐
│ Application │
├─────────────┤
│  Library A  │  →      ┌─────────────┐
├─────────────┤         │  Library A  │ (module)
│  Library B  │  →      ├─────────────┤
└─────────────┘         │  Library B  │ (module)
                        └─────────────┘
                        ↓
                        ┌─────────────┐
                        │ Application │ (module)
                        └─────────────┘

// Migrer d'abord les dépendances, puis l'application
```

**Étape 2: Top-Down (plus difficile)**
```
// Migrer l'application d'abord
// Les dépendances restent sur classpath (automatic modules)
```

### Migration Pratique

**1. Analyse des dépendances:**
```bash
# Analyser les dépendances
jdeps --class-path libs/* myapp.jar

# Identifier les packages utilisés
jdeps -s myapp.jar

# Générer module-info suggestions
jdeps --generate-module-info out myapp.jar
```

**2. Créer module-info.java:**
```java
// module-info.java généré
module com.myapp {
    requires java.sql;
    requires java.logging;
    requires org.apache.commons.lang3;  // Automatic module
    
    exports com.myapp.api;
}
```

**3. Gérer les problèmes:**

```java
// Problème: Internal API usage (sun.misc, com.sun.*)
// Solution 1: --add-exports
java --add-exports java.base/sun.misc=ALL-UNNAMED \
     --module-path mods \
     --module com.myapp

// Solution 2: --add-opens (reflection)
java --add-opens java.base/java.lang=ALL-UNNAMED \
     --module-path mods \
     --module com.myapp

// Problème: Split package (même package dans plusieurs JARs)
// Solution: Renommer ou merger les packages

// Problème: Reflection sur classes non-exported
module com.myapp {
    opens com.myapp.internal;  // Permet reflection
}
```

**4. Migration progressive:**

```bash
# Phase 1: Tout sur classpath
java -cp "libs/*:myapp.jar" com.myapp.Main

# Phase 2: Libraries sur module path (automatic modules)
java --class-path myapp.jar \
     --module-path libs \
     --add-modules ALL-MODULE-PATH \
     com.myapp.Main

# Phase 3: Tout sur module path
java --module-path "mods:libs" \
     --module com.myapp/com.myapp.Main
```

### Unnamed Module et Module Interactions

```java
// Module peut accéder à unnamed module
module com.myapp {
    // Pas besoin de requires pour unnamed
}

// Code dans module
import org.oldlib.OldClass;  // Sur classpath
public class MyClass {
    OldClass old = new OldClass();  // OK
}

// Unnamed module PEUT accéder aux modules
// Mais modules ne peuvent PAS accéder à unnamed
```

### --add-modules

```bash
# Ajouter des modules au module graph
java --add-modules java.se \
     --module-path mods \
     --module com.myapp

# ALL-MODULE-PATH - tous les modules sur module path
java --add-modules ALL-MODULE-PATH \
     --module-path mods \
     --module com.myapp

# ALL-DEFAULT - modules par défaut
java --add-modules ALL-DEFAULT \
     --module com.myapp

# ALL-SYSTEM - tous les modules système
java --add-modules ALL-SYSTEM \
     --module com.myapp
```

---

## Modules Système (JDK)

### java.base

```java
// Module de base - TOUJOURS présent
// Implicitement requis par tous les modules

module com.myapp {
    // requires java.base;  // Implicite, pas besoin de déclarer
}

// Contient:
// - java.lang.*
// - java.util.*
// - java.io.*
// - java.net.*
// etc.
```

### Autres Modules Courants

```java
// java.sql - JDBC
module com.myapp {
    requires java.sql;
}

// java.xml - XML processing
module com.myapp {
    requires java.xml;
}

// java.desktop - GUI (Swing, AWT)
module com.myapp {
    requires java.desktop;
}

// java.logging - Logging
module com.myapp {
    requires java.logging;
}

// java.net.http - HTTP Client (Java 11+)
module com.myapp {
    requires java.net.http;
}
```

---

## Bonnes Pratiques

### 1. Nommage des Modules

```java
// Convention: reverse domain name
module com.company.product.feature {
    // ...
}

// Exemples
module com.google.guava { }
module org.springframework.core { }
module io.netty.buffer { }

// ÉVITER
module mymodule { }  // Trop générique
module Module1 { }   // Pas descriptif
```

### 2. Packages et Exports

```java
// BONNE PRATIQUE: Packages séparés API/Implementation
module com.myapp {
    exports com.myapp.api;           // API publique
    // com.myapp.internal pas exporté  // Implémentation privée
}

// MAUVAISE PRATIQUE: Tout dans un package
module com.myapp {
    exports com.myapp;  // API et implémentation mélangés
}
```

### 3. Requires Transitive

```java
// Utiliser transitive quand le module expose des types d'une dépendance

// API utilise java.sql types
module com.myapp.api {
    requires transitive java.sql;  // Les clients ont besoin de java.sql
    exports com.myapp.api;
}

package com.myapp.api;
import java.sql.Connection;

public interface DatabaseService {
    Connection getConnection();  // Expose java.sql type
}

// Client n'a PAS besoin de "requires java.sql"
module com.myapp.client {
    requires com.myapp.api;  // java.sql disponible via transitive
}
```

### 4. Services vs Direct Dependencies

```java
// Préférer services (uses/provides) pour découplage

// BON: Loose coupling
module com.myapp.client {
    requires com.myapp.api;
    uses com.myapp.api.Service;  // Découplé de l'implémentation
}

// MOINS BON: Tight coupling
module com.myapp.client {
    requires com.myapp.api;
    requires com.myapp.impl;  // Couplé à l'implémentation
}
```

---

## Points Clés

### Modules
- **Module-info.java**: descripteur du module (racine du module)
- **exports**: rend un package accessible
- **requires**: déclare une dépendance
- **requires transitive**: propage la dépendance
- **opens**: permet la reflection

### Types de Modules
- **Named**: module-info.java présent, forte encapsulation
- **Automatic**: JAR sans module-info sur module path
- **Unnamed**: classes sur classpath (backward compatibility)

### Services
- **uses**: déclare utiliser un service
- **provides...with**: fournit une implémentation
- **ServiceLoader**: charge les implémentations

### Migration
- **Bottom-up**: migrer les dépendances d'abord
- **jdeps**: analyser les dépendances
- **--add-modules**: ajouter des modules au graph
- **--add-exports/opens**: contourner l'encapsulation (temporaire)

---

# Questions/Réponses - Chapter 12: Modules

## Questions sur les Bases des Modules

**Q1: Qu'est-ce qu'un module en Java?**

**R:** Un **module** est une unité de code et de données qui:
- Définit **explicitement** ses dépendances (`requires`)
- Déclare ce qu'il **expose** (`exports`)
- Améliore l'**encapsulation**
- Permet la vérification à la **compilation**

```java
// module-info.java
module com.myapp {
    exports com.myapp.api;      // Ce qu'on expose
    requires java.sql;          // Ce dont on dépend
}
```

---

**Q2: Où doit se trouver module-info.java?**

**R:** À la **racine** du module (même niveau que les packages).

```
mymodule/
├── module-info.java          ← ICI (racine)
└── com/
    └── myapp/
        └── Main.java
```

**ERREUR commune:**
```
mymodule/
└── com/
    └── myapp/
        ├── module-info.java  ← FAUX (pas à la racine)
        └── Main.java
```

---

**Q3: Ce code compile-t-il?**
```java
// module-info.java
package com.myapp;  // Package declaration

module com.myapp {
    exports com.myapp.api;
}
```

**R:** Non, **erreur de compilation**.

`module-info.java` ne doit **PAS** avoir de déclaration de package.

**Correct:**
```java
// module-info.java
module com.myapp {
    exports com.myapp.api;
}
```

---

**Q4: Quelle est la différence entre exports et requires?**

**R:**
- **exports**: Rend un **package** accessible aux **autres modules**
- **requires**: Déclare une **dépendance** vers un autre module

```java
// Module A
module com.myapp.api {
    exports com.myapp.api;  // Expose le package
}

// Module B
module com.myapp.client {
    requires com.myapp.api;  // Dépend du module A
}

// Module B peut maintenant utiliser les classes de com.myapp.api
```

---

**Q5: Ce code compile-t-il?**
```java
// Module A
module com.myapp.api {
    exports com.myapp.api;
}

// Module B
module com.myapp.client {
    // Pas de requires
}

// Code dans Module B
import com.myapp.api.Service;  // ???
```

**R:** Non, **erreur de compilation**: "package com.myapp.api is not visible".

Module B doit déclarer `requires com.myapp.api`.

---

**Q6: Que fait java.base?**

**R:** **java.base** est le module **fondamental** de Java:
- Contient `java.lang.*`, `java.util.*`, `java.io.*`...
- **Implicitement requis** par tous les modules
- Pas besoin de déclarer `requires java.base`

```java
module com.myapp {
    // requires java.base;  ← Implicite, pas nécessaire
    exports com.myapp;
}

// Peut utiliser String, System, List, etc. sans requires
```

---

**Q7: Comment compiler un module?**

**R:**
```bash
# Structure
src/
└── com.myapp/
    ├── module-info.java
    └── com/myapp/Main.java

# Compiler
javac -d mods/com.myapp \
      src/com.myapp/module-info.java \
      src/com.myapp/com/myapp/Main.java

# Créer JAR
jar --create --file mods/com.myapp.jar \
    --main-class com.myapp.Main \
    -C mods/com.myapp .

# Exécuter
java --module-path mods \
     --module com.myapp/com.myapp.Main

# Ou si main-class défini dans JAR
java --module-path mods \
     --module com.myapp
```

---

**Q8: Quelle est la différence entre --module-path et --class-path?**

**R:**

| **--module-path** | **--class-path** |
|-------------------|------------------|
| Pour modules | Pour JAR traditionnels |
| Forte encapsulation | Pas d'encapsulation |
| Vérification dépendances | Pas de vérification |
| Java 9+ | Depuis Java 1.0 |

```bash
# Module path
java --module-path mods \
     --module com.myapp

# Class path (ancien)
java --class-path libs/myapp.jar \
     com.myapp.Main
```

---

## Questions sur exports et requires

**Q9: Quelle est la différence entre ces deux?**
```java
// Version 1
module com.myapp {
    exports com.myapp.api;
}

// Version 2
module com.myapp {
    exports com.myapp.api to com.myapp.client;
}
```

**R:**
- **Version 1**: `com.myapp.api` accessible à **TOUS** les modules
- **Version 2**: `com.myapp.api` accessible **SEULEMENT** à `com.myapp.client` (**qualified export**)

```java
// Version 2: exports qualifié
module com.myapp {
    exports com.myapp.api to com.myapp.client, com.myapp.test;
}

// Seulement client et test peuvent accéder à com.myapp.api
```

---

**Q10: Ce code a-t-il un problème?**
```java
// Module A
module com.myapp.api {
    exports com.myapp.api;
}

// Module B
module com.myapp.client {
    requires com.myapp.api;
}

// Code dans Module B
import com.myapp.api.Service;       // OK
import com.myapp.api.internal.Util; // ???
```

**R:** **Erreur de compilation** sur la 2ème ligne: "package com.myapp.api.internal is not visible".

Seul `com.myapp.api` est exporté, pas `com.myapp.api.internal`.

**Règle**: `exports` expose **UN** package, pas ses sous-packages.

---

**Q11: Quelle est la différence entre requires et requires transitive?**

**R:**

**requires**: Dépendance **directe** seulement
```java
// Module A
module com.myapp.api {
    requires java.sql;  // Seulement A peut utiliser java.sql
}

// Module B
module com.myapp.client {
    requires com.myapp.api;
    requires java.sql;  // Doit déclarer explicitement
}
```

**requires transitive**: Dépendance **propagée**
```java
// Module A
module com.myapp.api {
    requires transitive java.sql;  // Propage java.sql
    exports com.myapp.api;
}

// Module B
module com.myapp.client {
    requires com.myapp.api;
    // Pas besoin de requires java.sql (via transitive)
}
```

---

**Q12: Quand utiliser requires transitive?**

**R:** Quand le module **expose des types** d'une dépendance dans son API.

```java
// Module API expose java.sql types
module com.myapp.api {
    requires transitive java.sql;  // CORRECT
    exports com.myapp.api;
}

package com.myapp.api;
import java.sql.Connection;

public interface DatabaseService {
    Connection getConnection();  // Expose java.sql type
}

// Client utilise Connection sans requires java.sql
module com.myapp.client {
    requires com.myapp.api;  // java.sql disponible via transitive
}

import com.myapp.api.DatabaseService;
import java.sql.Connection;  // OK grâce à transitive

DatabaseService service = ...;
Connection conn = service.getConnection();  // OK
```

**Sans transitive:**
```java
module com.myapp.api {
    requires java.sql;  // PAS transitive
}

// Client doit déclarer java.sql explicitement
module com.myapp.client {
    requires com.myapp.api;
    requires java.sql;  // Obligatoire
}
```

---

**Q13: Que fait requires static?**

**R:** **requires static** = dépendance **optionnelle** (compile-time seulement).

```java
module com.myapp {
    requires static lombok;  // Optionnel au runtime
}

// Utilisation
import lombok.Data;

@Data  // OK à la compilation (lombok présent)
public class User {
    private String name;
}

// Au runtime:
// - Si lombok présent → OK
// - Si lombok absent → OK aussi (pas d'erreur)
```

**Cas d'usage**: Annotations, outils de build, dépendances de développement.

---

**Q14: Ce code compile-t-il?**
```java
module com.myapp {
    exports com.myapp.api;
    exports com.myapp.api;  // Dupliqué
}
```

**R:** Non, **erreur de compilation**: "duplicate exports directive".

Chaque package ne peut être exporté qu'**une seule fois**.

---

**Q15: Que se passe-t-il si un package n'est pas exporté?**

**R:** Il est **privé** au module (encapsulation forte).

```java
// Module A
module com.myapp.api {
    exports com.myapp.api;
    // com.myapp.internal PAS exporté
}

package com.myapp.internal;
public class Helper {  // Public dans le package
    // Mais pas accessible hors du module
}

// Module B
module com.myapp.client {
    requires com.myapp.api;
}

import com.myapp.internal.Helper;  // ERREUR: package not visible

// MÊME si Helper est public, il n'est pas accessible
```

---

## Questions sur opens et Reflection

**Q16: Quelle est la différence entre exports et opens?**

**R:**

| **exports** | **opens** |
|-------------|-----------|
| Accessible par code normal | Accessible par reflection |
| Compilation + Runtime | Seulement runtime |
| Classes publiques accessibles | Toutes classes accessibles (même private) |

```java
module com.myapp {
    exports com.myapp.api;     // Code normal peut accéder
    opens com.myapp.model;     // Reflection peut accéder
}

// Code normal
import com.myapp.api.Service;    // OK
import com.myapp.model.User;     // ERREUR: not exported

// Reflection
Class<?> clazz = Class.forName("com.myapp.model.User");  // OK
Field field = clazz.getDeclaredField("name");
field.setAccessible(true);  // OK grâce à opens
```

---

**Q17: Que fait open module?**

**R:** **open module** = tous les packages sont **open** (reflection accessible).

```java
// Version 1: open spécifique
module com.myapp {
    opens com.myapp.model;
    opens com.myapp.entity;
}

// Version 2: open module (tout ouvert)
open module com.myapp {
    // TOUS les packages open pour reflection
    // MAIS pas exportés (pas accessible en code normal)
}
```

**Cas d'usage**: Frameworks (Hibernate, Spring, Jackson) qui utilisent la reflection.

---

**Q18: Ce code fonctionne-t-il?**
```java
// Module
module com.myapp {
    // Pas de opens
}

package com.myapp.model;
public class User {
    private String name;
}

// Code client
Class<?> clazz = Class.forName("com.myapp.model.User");
Field field = clazz.getDeclaredField("name");
field.setAccessible(true);  // ???
field.set(user, "Alice");
```

**R:** Non, **InaccessibleObjectException** sur `setAccessible(true)`.

Sans `opens`, la reflection profonde (champs private) est **bloquée**.

**Solution:**
```java
module com.myapp {
    opens com.myapp.model;  // Permet reflection
}
```

---

**Q19: Quelle est la différence?**
```java
// Version 1
module com.myapp {
    opens com.myapp.model;
}

// Version 2
module com.myapp {
    opens com.myapp.model to hibernate, jackson;
}
```

**R:**
- **Version 1**: Ouvert à **tous** les modules
- **Version 2**: Ouvert **seulement** à hibernate et jackson (**qualified opens**)

```java
// Version 2
module com.myapp {
    opens com.myapp.model to org.hibernate.orm, com.fasterxml.jackson;
}

// Seulement ces modules peuvent utiliser reflection sur model
```

---

**Q20: Peut-on combiner exports et opens?**

**R:** Oui, **totalement valide**.

```java
module com.myapp {
    exports com.myapp.api;     // Code normal
    opens com.myapp.api;       // + Reflection
}

// OU
module com.myapp {
    exports com.myapp.api;         // API publique
    opens com.myapp.internal;      // Seulement reflection
}
```

---

## Questions sur Services (uses/provides)

**Q21: Que font uses et provides?**

**R:**
- **uses**: Déclare qu'on **utilise** un service (SPI)
- **provides...with**: Fournit une **implémentation** du service

```java
// Module API (définit le service)
module com.myapp.spi {
    exports com.myapp.spi;
    uses com.myapp.spi.Calculator;  // Déclare utiliser
}

// Interface
package com.myapp.spi;
public interface Calculator {
    int calculate(int a, int b);
}

// Module Implementation (fournit le service)
module com.myapp.impl {
    requires com.myapp.spi;
    provides com.myapp.spi.Calculator 
        with com.myapp.impl.AddCalculator;
}

// Implementation
package com.myapp.impl;
import com.myapp.spi.Calculator;

public class AddCalculator implements Calculator {
    @Override
    public int calculate(int a, int b) {
        return a + b;
    }
}
```

---

**Q22: Comment charger les services avec ServiceLoader?**

**R:**
```java
import java.util.ServiceLoader;
import com.myapp.spi.Calculator;

// Charger toutes les implémentations disponibles
ServiceLoader<Calculator> loader = ServiceLoader.load(Calculator.class);

// Itérer sur les services
for (Calculator calc : loader) {
    System.out.println(calc.getClass().getName());
    System.out.println("5 + 3 = " + calc.calculate(5, 3));
}

// Ou trouver le premier
Calculator calculator = loader.findFirst()
    .orElseThrow(() -> new RuntimeException("No implementation found"));

// Stream API
loader.stream()
    .map(ServiceLoader.Provider::get)
    .forEach(calc -> System.out.println(calc.calculate(2, 3)));
```

---

**Q23: Peut-on avoir plusieurs implémentations d'un service?**

**R:** Oui, **ServiceLoader trouve TOUTES** les implémentations disponibles.

```java
// Module 1
module com.myapp.add {
    provides com.myapp.spi.Calculator 
        with com.myapp.add.AddCalculator;
}

// Module 2
module com.myapp.multiply {
    provides com.myapp.spi.Calculator 
        with com.myapp.multiply.MultiplyCalculator;
}

// Client trouve les DEUX
ServiceLoader<Calculator> loader = ServiceLoader.load(Calculator.class);
int count = 0;
for (Calculator calc : loader) {
    count++;
}
System.out.println("Found " + count + " implementations");  // 2
```

---

**Q24: Ce code compile-t-il?**
```java
module com.myapp.impl {
    requires com.myapp.spi;
    provides com.myapp.spi.Calculator 
        with com.myapp.impl.AddCalculator;
}

package com.myapp.impl;
class AddCalculator implements Calculator {  // package-private
    // ...
}
```

**R:** Non, **erreur de compilation**: "service implementation must be public".

La classe d'implémentation doit être **public** et avoir un **constructeur public no-arg**.

**Correct:**
```java
package com.myapp.impl;
public class AddCalculator implements Calculator {  // public
    public AddCalculator() {  // constructeur public
        // ...
    }
    // ...
}
```

---

**Q25: Quelle est la différence entre requires et uses?**

**R:**
- **requires**: Dépendance **statique** (connue à la compilation)
- **uses**: Dépendance **dynamique** (découverte au runtime)

```java
// requires - dépendance fixe
module com.myapp.client {
    requires com.myapp.impl;  // Sait quelle implémentation
}
import com.myapp.impl.ServiceImpl;
ServiceImpl service = new ServiceImpl();  // Direct

// uses - dépendance dynamique
module com.myapp.client {
    requires com.myapp.spi;
    uses com.myapp.spi.Service;  // Ne sait pas quelle implémentation
}
ServiceLoader<Service> loader = ServiceLoader.load(Service.class);
// Trouve les implémentations au runtime
```

---

## Questions sur les Types de Modules

**Q26: Quels sont les 3 types de modules?**

**R:**

**1. Named Module** (module explicite)
```java
// module-info.java présent
module com.myapp {
    exports com.myapp.api;
    requires java.sql;
}
// Sur module path
```

**2. Automatic Module** (JAR sans module-info sur module path)
```java
// commons-lang3-3.12.0.jar sur module path
// Devient module: commons.lang3
// Exports TOUS les packages
// Requires TOUS les autres modules

module com.myapp {
    requires commons.lang3;  // Automatic module
}
```

**3. Unnamed Module** (classpath)
```java
// JARs sur classpath
// Peut lire tous les modules
// Pas accessible depuis named modules
```

---

**Q27: Comment est dérivé le nom d'un automatic module?**

**R:** Depuis le **nom du JAR**:

```bash
# Nom du JAR → Nom du module
commons-lang3-3.12.0.jar     → commons.lang3
jackson-databind-2.13.0.jar  → jackson.databind
spring-core-5.3.9.jar        → spring.core

# Règles:
# 1. Enlever version (après dernier -)
# 2. Remplacer - par .
# 3. Enlever caractères invalides

# Exemple
my-app-utils-1.0.0-SNAPSHOT.jar → my.app.utils
```

**Override avec MANIFEST.MF:**
```
Manifest-Version: 1.0
Automatic-Module-Name: com.mycompany.utils
```

---

**Q28: Quelle est la différence entre automatic et unnamed module?**

**R:**

| **Automatic** | **Unnamed** |
|---------------|-------------|
| Sur **module path** | Sur **classpath** |
| A un nom | Pas de nom |
| Peut être requis | Ne peut pas être requis |
| Exports tout | N'exporte rien |

```bash
# Automatic module
java --module-path libs \
     --module com.myapp

# Unnamed module
java --class-path libs/* \
     com.myapp.Main
```

---

**Q29: Un named module peut-il accéder à un unnamed module?**

**R:** **NON**, directement impossible.

```java
// Module nommé
module com.myapp {
    // Impossible de requires un unnamed module
}

// Code
import com.oldlib.OldClass;  // Sur classpath
// ERREUR: package not visible
```

**Solution**: `--add-reads` et `--add-exports`
```bash
java --add-reads com.myapp=ALL-UNNAMED \
     --add-exports java.base/sun.misc=ALL-UNNAMED \
     --module-path mods \
     --class-path libs/* \
     --module com.myapp
```

---

**Q30: Un unnamed module peut-il accéder à un named module?**

**R:** **OUI**, l'unnamed module peut accéder à **tous** les modules exportés.

```java
// Named module
module com.myapp.api {
    exports com.myapp.api;
}

// Code sur classpath (unnamed)
import com.myapp.api.Service;  // OK
Service service = new Service();  // OK
```

---

## Questions sur Migration

**Q31: Quelles sont les deux stratégies de migration?**

**R:**

**1. Bottom-Up (recommandé)**
```
Migrer d'abord les dépendances (libraries)
Puis l'application

Avantages:
- Plus sûr
- Modules validés avant utilisation
- Encapsulation progressive
```

**2. Top-Down**
```
Migrer d'abord l'application
Les dépendances deviennent automatic modules

Avantages:
- Plus rapide initialement
Inconvénients:
- Pas d'encapsulation des dépendances
- Noms automatic modules imprévisibles
```

---

**Q32: Que fait jdeps?**

**R:** **jdeps** analyse les dépendances d'un JAR/classe.

```bash
# Analyser les dépendances
jdeps myapp.jar

# Résumé (-s)
jdeps -s myapp.jar
# myapp.jar -> java.base
# myapp.jar -> java.sql

# Avec classpath
jdeps --class-path libs/* myapp.jar

# Générer module-info.java
jdeps --generate-module-info out myapp.jar
# Crée: out/myapp/module-info.java

# Vérifier les modules
jdeps --check myapp.jar
# Vérifie que toutes les dépendances sont satisfaites

# Voir les packages utilisés
jdeps --list-deps myapp.jar
# java.base
# java.sql
# java.logging
```

---

**Q33: Qu'est-ce que le "split package" problem?**

**R:** **Split package** = même package dans **plusieurs modules/JARs**.

```java
// JAR 1
package com.myapp.util;
public class Helper1 { }

// JAR 2
package com.myapp.util;  // MÊME package
public class Helper2 { }

// Problème: com.myapp.util existe dans 2 modules
// Solution 1: Renommer un package
// Solution 2: Merger les JARs
// Solution 3: Refactoring
```

**INTERDIT** dans le module system:
```java
module com.myapp.a {
    exports com.myapp.util;
}

module com.myapp.b {
    exports com.myapp.util;  // ERREUR: split package
}
```

---

**Q34: Que fait --add-modules?**

**R:** **--add-modules** ajoute des modules au module graph.

```bash
# Ajouter un module spécifique
java --add-modules java.se \
     --module-path mods \
     --module com.myapp

# ALL-MODULE-PATH: tous les modules sur module path
java --add-modules ALL-MODULE-PATH \
     --module-path mods \
     --module com.myapp

# ALL-DEFAULT: modules par défaut
java --add-modules ALL-DEFAULT \
     --module com.myapp

# ALL-SYSTEM: tous les modules JDK
java --add-modules ALL-SYSTEM \
     --module com.myapp
```

**Cas d'usage**: Charger des modules non référencés directement (services, plugins).

---

**Q35: Que font --add-exports et --add-opens?**

**R:** Contournent l'encapsulation des modules (temporaire, pour migration).

```bash
# --add-exports: rend un package accessible
java --add-exports java.base/sun.misc=ALL-UNNAMED \
     --module com.myapp

# --add-opens: permet reflection
java --add-opens java.base/java.lang=ALL-UNNAMED \
     --module com.myapp

# Plusieurs modules
java --add-opens java.base/java.lang=com.myapp,com.other \
     --module com.myapp
```

**ATTENTION**: **Mauvaise pratique** en production. Utiliser seulement pour migration.

---

**Q36: Comment gérer l'accès à des internal APIs (sun.*, com.sun.*)?**

**R:**

```java
// Code utilisant internal API
import sun.misc.Unsafe;
Unsafe unsafe = Unsafe.getUnsafe();  // Internal API
```

**Solutions:**

**1. --add-exports (temporaire)**
```bash
java --add-exports java.base/sun.misc=ALL-UNNAMED \
     --module com.myapp
```

**2. Utiliser une API publique**
```java
// Au lieu de sun.misc.Unsafe
import java.lang.invoke.VarHandle;
// Utiliser VarHandle (API publique)
```

**3. Migrer vers librairie alternative**
```java
// Au lieu de com.sun.image.codec.jpeg
// Utiliser javax.imageio
```

---

## Questions sur Commandes et Outils

**Q37: Que fait --describe-module?**

**R:** Affiche les informations d'un module.

```bash
# Décrire un module JDK
java --describe-module java.sql

# Output:
# java.sql@17.0.1
# exports java.sql
# exports javax.sql
# requires java.base
# requires java.logging transitive
# requires java.xml
# uses java.sql.Driver

# Décrire un module custom
jar --describe-module --file mods/com.myapp.jar

# Ou
java --module-path mods --describe-module com.myapp
```

---

**Q38: Comment lister tous les modules disponibles?**

**R:**
```bash
# Lister modules JDK
java --list-modules

# Output:
# java.base@17.0.1
# java.compiler@17.0.1
# java.datatransfer@17.0.1
# java.desktop@17.0.1
# ...

# Lister modules custom
java --module-path mods --list-modules

# Filtrer
java --list-modules | grep java.sql
```

---

**Q39: Comment créer un module JAR?**

**R:**
```bash
# Structure compilée
mods/com.myapp/
├── module-info.class
└── com/myapp/
    └── Main.class

# Créer JAR
jar --create \
    --file mods/com.myapp.jar \
    --main-class com.myapp.Main \
    -C mods/com.myapp .

# Ou avec manifest
jar --create \
    --file mods/com.myapp.jar \
    --main-class com.myapp.Main \
    --module-version 1.0.0 \
    -C mods/com.myapp .

# Extraire module-info
jar --describe-module --file mods/com.myapp.jar
```

---

**Q40: Comment voir les dépendances d'un module?**

**R:**
```bash
# jdeps - résumé
jdeps -s mods/com.myapp.jar

# Output:
# com.myapp -> java.base
# com.myapp -> java.sql

# jdeps - détaillé
jdeps mods/com.myapp.jar

# Output:
# com.myapp
#   com.myapp.Main (com.myapp)
#     -> java.lang.Object    java.base
#     -> java.lang.String    java.base
#     -> java.sql.Connection java.sql

# Module graph
jdeps --module-path mods -s --module com.myapp

# Récursif (avec dépendances transitives)
jdeps -R -s mods/com.myapp.jar
```

---

## Questions Pièges et Cas Complexes

**Q41: Ce code compile-t-il?**
```java
// Module A
module com.myapp.a {
    exports com.myapp.api;
}

// Module B
module com.myapp.b {
    exports com.myapp.api;  // Même package
}
```

**R:** **Erreur au runtime**: "package com.myapp.api in both modules com.myapp.a and com.myapp.b".

**Split package** interdit. Chaque package ne peut appartenir qu'à **UN SEUL** module.

---

**Q42: Ce code fonctionne-t-il?**
```java
// Module A
module com.myapp.a {
    exports com.myapp.api;
    requires com.myapp.b;
}

// Module B
module com.myapp.b {
    exports com.myapp.util;
    requires com.myapp.a;  // Circular dependency
}
```

**R:** **Erreur de compilation**: "cyclic dependence involving com.myapp.a".

**Dépendances circulaires interdites**. Solution: refactoring ou module intermédiaire.

---

**Q43: Que se passe-t-il?**
```java
// Module A
module com.myapp.a {
    requires transitive com.myapp.b;
}

// Module B
module com.myapp.b {
    requires transitive com.myapp.c;
}

// Module C
module com.myapp.c {
    exports com.myapp.c.api;
}

// Client
module com.myapp.client {
    requires com.myapp.a;
}

// Client peut-il utiliser com.myapp.c.api?
```

**R:** **OUI**, grâce aux **requires transitive** en chaîne.

```
Client → A (transitive) → B (transitive) → C
```

Client a accès à A, B, ET C.

---

**Q44: Ce code compile-t-il?**
```java
module com.myapp {
    exports com.myapp.api;
    opens com.myapp.api;  // Même package
}
```

**R:** Oui, **totalement valide**.

Un package peut être à la fois **exported** (code normal) ET **open** (reflection).

---

**Q45: Que fait ce code?**
```java
module com.myapp {
    provides com.myapp.spi.Service 
        with com.myapp.impl.ServiceImpl;
}

package com.myapp.impl;
class ServiceImpl implements Service {  // package-private
    // ...
}
```

**R:** **Erreur de compilation**: "service implementation must be public and have a public no-args constructor".

L'implémentation doit être **public** pour ServiceLoader.

---

**Q46: Peut-on avoir un module sans exports?**
**R:** Oui, **valide** si le module est une **application standalone**.

```java
// Module application (pas de bibliothèque)
module com.myapp {
    requires java.sql;
    // Pas d'exports - tout est privé
}

// Cas d'usage: Application finale qui n'expose rien
// Le module peut avoir un main() sans exporter de packages
```

---

**Q47: Ce code compile-t-il?**
```java
module com.myapp {
    requires java.sql;
    requires java.sql;  // Dupliqué
}
```

**R:** Non, **erreur de compilation**: "duplicate requires directive".

Chaque module ne peut être requis qu'**une seule fois**.

---

**Q48: Quelle est la différence?**
```java
// Version 1
module com.myapp {
    exports com.myapp.api;
    opens com.myapp.api;
}

// Version 2
open module com.myapp {
    exports com.myapp.api;
}
```

**R:**
- **Version 1**: Seulement `com.myapp.api` est open
- **Version 2**: **TOUS** les packages sont open (même ceux non exportés)

```java
// Version 2: open module
open module com.myapp {
    exports com.myapp.api;
}

// Équivalent à
module com.myapp {
    exports com.myapp.api;
    opens com.myapp.api;
    opens com.myapp.internal;
    opens com.myapp.model;
    // ... tous les packages sont open
}
```

---

**Q49: Que se passe-t-il?**
```java
// Module A
module com.myapp.a {
    requires transitive com.myapp.b;
    exports com.myapp.a.api;
}

// Module B
module com.myapp.b {
    exports com.myapp.b.api;
}

// Module C
module com.myapp.c {
    requires com.myapp.a;
    // Peut utiliser com.myapp.b.api?
}
```

**R:** **OUI**, grâce à `requires transitive`.

```java
// Module C
import com.myapp.a.api.ServiceA;  // OK
import com.myapp.b.api.ServiceB;  // OK via transitive
```

Sans `transitive`, Module C devrait déclarer `requires com.myapp.b`.

---

**Q50: Ce code a-t-il un problème?**
```java
module com.myapp {
    provides com.myapp.spi.Service 
        with com.myapp.impl.ServiceImpl;
    
    // Service pas exporté
}

package com.myapp.spi;
public interface Service {
    void execute();
}
```

**R:** **Oui**, problème de visibilité!

Pour que `ServiceLoader` trouve le service, l'interface doit être **exportée**.

**Correct:**
```java
module com.myapp {
    exports com.myapp.spi;  // Interface visible
    provides com.myapp.spi.Service 
        with com.myapp.impl.ServiceImpl;
}

// L'implémentation n'a PAS besoin d'être exportée
```

---

**Q51: Quelle est la différence entre ces deux approches?**
```java
// Approche 1: Direct dependency
module com.myapp.client {
    requires com.myapp.impl;
}
import com.myapp.impl.ServiceImpl;
ServiceImpl service = new ServiceImpl();

// Approche 2: Service Loader
module com.myapp.client {
    requires com.myapp.spi;
    uses com.myapp.spi.Service;
}
ServiceLoader<Service> loader = ServiceLoader.load(Service.class);
Service service = loader.findFirst().orElseThrow();
```

**R:**

| **Approche 1 (Direct)** | **Approche 2 (ServiceLoader)** |
|-------------------------|--------------------------------|
| **Tight coupling** | **Loose coupling** |
| Dépend de l'implémentation | Dépend de l'interface |
| Changement = recompilation | Changement = runtime swap |
| Une seule implémentation | Plusieurs implémentations possibles |

---

**Q52: Que fait cette commande?**
```bash
java --show-module-resolution \
     --module-path mods \
     --module com.myapp/com.myapp.Main
```

**R:** Affiche le **processus de résolution** des modules au démarrage.

```
Output:
root com.myapp
com.myapp requires java.sql
  java.sql@17.0.1 binds java.logging@17.0.1
  java.sql@17.0.1 binds java.xml@17.0.1
com.myapp requires java.base
  java.base@17.0.1 is the platform module

# Montre:
# - Modules résolus
# - Ordre de résolution
# - Dépendances transitives
# - Bindings de services
```

**Utile pour**: Debugging de problèmes de modules, comprendre le module graph.

---

**Q53: Ce code compile-t-il?**
```java
// module-info.java
module com.myapp {
    exports com.myapp.api;
}

// Main.java
module com.myapp {  // ???
    public static void main(String[] args) {
        System.out.println("Hello");
    }
}
```

**R:** Non, **erreur de compilation**: "class, interface, or enum expected".

`module` ne peut apparaître que dans **module-info.java**. Pas dans les fichiers de classe normaux.

---

**Q54: Que se passe-t-il avec plusieurs provides?**
```java
module com.myapp {
    provides com.myapp.spi.Service 
        with com.myapp.impl.ServiceImpl1,
             com.myapp.impl.ServiceImpl2;
}
```

**R:** **Valide**. Un module peut fournir **plusieurs implémentations** du même service.

```java
ServiceLoader<Service> loader = ServiceLoader.load(Service.class);
int count = 0;
for (Service service : loader) {
    System.out.println(service.getClass().getName());
    count++;
}
// Output:
// com.myapp.impl.ServiceImpl1
// com.myapp.impl.ServiceImpl2
System.out.println("Total: " + count);  // 2
```

---

**Q55: Comment gérer les versions de modules?**

**R:** Les modules Java **ne gèrent PAS les versions** nativement (contrairement à Maven/Gradle).

```java
// module-info.java - PAS de version
module com.myapp {
    // Pas de version ici
}

// Version dans JAR manifest
jar --create \
    --file com.myapp.jar \
    --module-version 1.0.0 \
    -C mods/com.myapp .

// Vérifier la version
jar --describe-module --file com.myapp.jar
# com.myapp@1.0.0

// Mais requires ne peut pas spécifier de version
module com.myapp.client {
    requires com.myapp;  // Pas de version possible
    // requires com.myapp@1.0.0;  // INVALIDE
}
```

**Gestion des versions**: Utiliser Maven/Gradle/tools de build, pas le module system.

---

## Questions sur Migration et Compatibilité

**Q56: Comment migrer progressivement vers les modules?**

**R:** **Approche en 3 phases:**

**Phase 1: Tout sur classpath (status quo)**
```bash
java -cp "libs/*:app.jar" com.myapp.Main
```

**Phase 2: Mix classpath + module path**
```bash
# Libraries sur module path (automatic modules)
# Application sur classpath
java --class-path app.jar \
     --module-path libs \
     --add-modules ALL-MODULE-PATH \
     com.myapp.Main
```

**Phase 3: Tout sur module path**
```bash
# Tout modulaire
java --module-path "mods:libs" \
     --module com.myapp/com.myapp.Main
```

---

**Q57: Que fait --patch-module?**

**R:** **--patch-module** remplace/ajoute des classes à un module (pour testing/debugging).

```bash
# Remplacer une classe dans un module
javac --patch-module java.base=patches \
      patches/java/lang/String.java

java --patch-module java.base=patches \
     --module-path mods \
     --module com.myapp

# Cas d'usage:
# - Tester un fix dans un module JDK
# - Override une classe pour debugging
# - Hot-swap en développement
```

**ATTENTION**: **Ne jamais utiliser en production!**

---

**Q58: Comment gérer les tests avec modules?**

**R:** Plusieurs approches:

**Approche 1: Black-box testing (tests externes)**
```java
// Module
module com.myapp {
    exports com.myapp.api;
    // com.myapp.internal pas exporté
}

// Tests (autre module)
module com.myapp.test {
    requires com.myapp;
    requires org.junit.jupiter.api;
    
    // Peut tester seulement l'API publique
}
```

**Approche 2: White-box testing (tests internes)**
```java
// Module principal
module com.myapp {
    exports com.myapp.api;
    // Pour tests seulement
    exports com.myapp.internal to com.myapp.test;
    opens com.myapp.internal to com.myapp.test;
}

// Tests
module com.myapp.test {
    requires com.myapp;
    requires org.junit.jupiter.api;
    
    // Peut tester API + internal
}
```

**Approche 3: Pas de module pour tests**
```bash
# Tests sur classpath (pas de module-info)
java --class-path "app.jar:test-classes:junit.jar" \
     --add-opens com.myapp/com.myapp.internal=ALL-UNNAMED \
     org.junit.platform.console.ConsoleLauncher
```

---

**Q59: Comment utiliser des modules avec Maven/Gradle?**

**R:**

**Maven:**
```xml
<project>
    <build>
        <plugins>
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-compiler-plugin</artifactId>
                <version>3.11.0</version>
                <configuration>
                    <release>17</release>
                </configuration>
            </plugin>
        </plugins>
    </build>
    
    <dependencies>
        <dependency>
            <groupId>org.example</groupId>
            <artifactId>my-lib</artifactId>
            <version>1.0.0</version>
        </dependency>
    </dependencies>
</project>
```

**Gradle:**
```groovy
plugins {
    id 'java'
}

java {
    modularity.inferModulePath = true
    sourceCompatibility = JavaVersion.VERSION_17
}

dependencies {
    implementation 'org.example:my-lib:1.0.0'
}
```

**Structure:**
```
src/
├── main/
│   ├── java/
│   │   ├── module-info.java
│   │   └── com/myapp/Main.java
│   └── resources/
└── test/
    ├── java/
    │   └── com/myapp/MainTest.java
    └── resources/
```

---

**Q60: Que sont les platform modules?**

**R:** **Platform modules** = modules du **JDK** (fournis par Java).

```java
// Modules platform principaux
java.base           // Base (toujours présent)
java.sql            // JDBC
java.xml            // XML
java.logging        // Logging
java.desktop        // Swing/AWT
java.net.http       // HTTP Client
java.compiler       // javax.tools
java.management     // JMX
java.naming         // JNDI
java.prefs          // Preferences

// Utilisation
module com.myapp {
    requires java.sql;
    requires java.logging;
}
```

**Module aggregator:**
```java
// java.se - aggregator de modules communs
module com.myapp {
    requires java.se;  // Inclut: sql, xml, logging, etc.
}
```

---

**Q61: Comment créer un runtime image custom avec jlink?**

**R:** **jlink** crée un runtime JRE minimal avec seulement les modules nécessaires.

```bash
# Créer image custom
jlink --module-path $JAVA_HOME/jmods:mods \
      --add-modules com.myapp \
      --output myapp-runtime \
      --launcher myapp=com.myapp/com.myapp.Main

# Structure générée
myapp-runtime/
├── bin/
│   ├── java
│   └── myapp       # Launcher custom
├── conf/
├── lib/
└── legal/

# Exécuter
./myapp-runtime/bin/myapp

# Avantages:
# - Taille réduite (seulement modules nécessaires)
# - Startup plus rapide
# - Distribution simplifiée
# - JRE embarqué

# Exemple: Application avec seulement java.base
jlink --module-path mods \
      --add-modules com.myapp \
      --output minimal-runtime \
      --compress=2 \
      --strip-debug \
      --no-header-files \
      --no-man-pages

# Comparer les tailles
du -sh $JAVA_HOME              # ~300 MB
du -sh minimal-runtime         # ~50 MB
```

---

**Q62: Que fait --illegal-access?**

**R:** **--illegal-access** (Java 9-16, removed in 17) contrôlait l'accès aux internal APIs.

```bash
# Java 9-16
java --illegal-access=permit  # Default, warn first access
java --illegal-access=warn    # Warn each access
java --illegal-access=debug   # Warn + stack trace
java --illegal-access=deny    # Deny all access

# Java 17+: Option removed, toujours "deny"
# Utiliser --add-opens à la place
java --add-opens java.base/java.lang=ALL-UNNAMED \
     --module com.myapp
```

---

## Questions Pratiques et Scénarios Réels

**Q63: Comment débugger "module not found"?**

**R:**

**Erreur:**
```
Error occurred during initialization of boot layer
java.lang.module.FindException: Module com.myapp not found
```

**Solutions:**

```bash
# 1. Vérifier module path
ls mods/
# Le JAR est-il présent?

# 2. Vérifier le nom du module
jar --describe-module --file mods/com.myapp.jar
# Le nom correspond-il?

# 3. Lister modules disponibles
java --module-path mods --list-modules
# Le module apparaît-il?

# 4. Vérifier le module-info
jar -xf mods/com.myapp.jar module-info.class
javap module-info.class
# module com.myapp@version
# requires ...
# exports ...

# 5. Utiliser --show-module-resolution
java --show-module-resolution \
     --module-path mods \
     --module com.myapp
```

---

**Q64: Comment débugger "package not visible"?**

**R:**

**Erreur:**
```
error: package com.myapp.internal is not visible
  (package com.myapp.internal is declared in module com.myapp,
   which does not export it to module com.myapp.client)
```

**Solutions:**

```java
// 1. Exporter le package
module com.myapp {
    exports com.myapp.internal;  // Rend visible
}

// 2. Export qualifié (seulement à certains modules)
module com.myapp {
    exports com.myapp.internal to com.myapp.client, com.myapp.test;
}

// 3. Si c'est pour reflection, utiliser opens
module com.myapp {
    opens com.myapp.internal;  // Reflection seulement
}

// 4. Temporaire (migration): --add-exports
java --add-exports com.myapp/com.myapp.internal=com.myapp.client \
     --module-path mods \
     --module com.myapp.client
```

---

**Q65: Comment gérer les frameworks qui utilisent reflection?**

**R:**

**Problème:** Frameworks (Hibernate, Spring, Jackson) utilisent la reflection.

**Solution 1: opens**
```java
module com.myapp {
    // Ouvrir packages pour frameworks
    opens com.myapp.entity to org.hibernate.orm;
    opens com.myapp.model to com.fasterxml.jackson.databind;
    opens com.myapp.config to spring.core;
}
```

**Solution 2: open module**
```java
// Tout ouvert (plus simple mais moins sécurisé)
open module com.myapp {
    requires spring.boot;
    requires org.hibernate.orm;
}
```

**Solution 3: Configuration au runtime**
```bash
# Ouvrir au runtime
java --add-opens com.myapp/com.myapp.entity=ALL-UNNAMED \
     --module-path mods \
     --module com.myapp
```

---

**Q66: Comment structurer un projet multi-modules?**

**R:**

**Structure Maven:**
```
myproject/
├── pom.xml                    # Parent POM
├── myapp-api/
│   ├── pom.xml
│   └── src/main/java/
│       ├── module-info.java
│       └── com/myapp/api/
├── myapp-impl/
│   ├── pom.xml
│   └── src/main/java/
│       ├── module-info.java
│       └── com/myapp/impl/
└── myapp-client/
    ├── pom.xml
    └── src/main/java/
        ├── module-info.java
        └── com/myapp/client/
```

**Parent POM:**
```xml
<project>
    <groupId>com.myapp</groupId>
    <artifactId>myapp-parent</artifactId>
    <packaging>pom</packaging>
    
    <modules>
        <module>myapp-api</module>
        <module>myapp-impl</module>
        <module>myapp-client</module>
    </modules>
</project>
```

**Module POM (exemple impl):**
```xml
<project>
    <parent>
        <groupId>com.myapp</groupId>
        <artifactId>myapp-parent</artifactId>
        <version>1.0.0</version>
    </parent>
    
    <artifactId>myapp-impl</artifactId>
    
    <dependencies>
        <dependency>
            <groupId>com.myapp</groupId>
            <artifactId>myapp-api</artifactId>
            <version>${project.version}</version>
        </dependency>
    </dependencies>
</project>
```

---

**Q67: Comment gérer les dépendances transitives avec automatic modules?**

**R:**

**Problème:**
```
Module A (named) → Library B (automatic) → Library C (automatic)
Module A peut-il accéder à C?
```

**Réponse:** **OUI**, automatic modules **exportent tout** et **requires tout**.

```java
// Module A
module com.myapp {
    requires library.b;  // Automatic module
    // Peut utiliser library.c (transitive automatique)
}

// Library B et C sont automatic modules
// B a automatiquement: requires transitive library.c
```

**Mais attention:**
```java
// Si B devient named module plus tard
module library.b {
    requires library.c;  // Sans transitive!
    exports library.b.api;
}

// Module A ne peut plus accéder à C
module com.myapp {
    requires library.b;
    requires library.c;  // Maintenant nécessaire
}
```

---

**Q68: Comment tester qu'un package n'est PAS exporté?**

**R:**

```java
// Module
module com.myapp {
    exports com.myapp.api;
    // com.myapp.internal PAS exporté
}

// Test (autre module)
module com.myapp.test {
    requires com.myapp;
}

// Code de test
import com.myapp.api.Service;           // OK
// import com.myapp.internal.Helper;    // Ne compile pas

// Test avec reflection
@Test
public void testInternalNotAccessible() {
    assertThrows(IllegalAccessException.class, () -> {
        Class<?> clazz = Class.forName("com.myapp.internal.Helper");
        clazz.getDeclaredConstructor().newInstance();
    });
}
```

---

**Q69: Peut-on avoir un module vide?**

**R:** Oui, **techniquement valide** mais **inutile**.

```java
// module-info.java
module com.myapp {
    // Rien
}

// Module valide mais:
// - N'exporte rien
// - Ne require rien (sauf java.base implicite)
// - Pas de code accessible de l'extérieur

// Cas d'usage: Module placeholder pour structure future
```

---

**Q70: Comment gérer les ressources dans un module?**

**R:**

**Structure:**
```
com.myapp/
├── module-info.java
├── com/myapp/
│   └── Main.java
└── resources/
    ├── config.properties
    └── messages_fr.properties
```

**Accéder aux ressources:**
```java
// Depuis la classe
InputStream is = Main.class.getResourceAsStream("/config.properties");

// Avec ClassLoader
InputStream is2 = Main.class.getClassLoader()
    .getResourceAsStream("config.properties");

// Module doit permettre l'accès
module com.myapp {
    // opens pour resources accessible par reflection
    opens com.myapp;
}

// ResourceBundle avec modules
ResourceBundle bundle = ResourceBundle.getBundle(
    "messages",
    Locale.FRANCE,
    Main.class.getModule()  // Spécifier le module
);
```

---

**Q71: Comment gérer les JAR multi-release avec modules?**

**R:**

**Multi-Release JAR** = différentes versions de classes pour différentes versions de Java.

```
myapp.jar
├── META-INF/
│   ├── MANIFEST.MF (Multi-Release: true)
│   └── versions/
│       ├── 9/
│       │   └── module-info.class
│       └── 17/
│           └── com/myapp/Helper.class
├── com/myapp/
│   ├── Main.class
│   └── Helper.class  (Java 8 version)
└── module-info.class (Java 9+)
```

**MANIFEST.MF:**
```
Manifest-Version: 1.0
Multi-Release: true
```

**Créer:**
```bash
jar --create \
    --file myapp.jar \
    --main-class com.myapp.Main \
    --release 9 -C classes-java9 module-info.class \
    --release 17 -C classes-java17 com/myapp/Helper.class \
    -C classes-java8 .
```

---

**Q72: Que sont les incubator modules?**

**R:** **Incubator modules** = modules **expérimentaux** du JDK (préfixe `jdk.incubator.`).

```java
// Exemple: Foreign Memory Access API (Java 14-16)
module com.myapp {
    requires jdk.incubator.foreign;
}

// Avertissement à la compilation:
// "module jdk.incubator.foreign is an incubator module"

// Caractéristiques:
// - API non finalisée
// - Peut changer entre versions
// - Peut être retirée
// - Nécessite --add-modules

java --add-modules jdk.incubator.foreign \
     --module-path mods \
     --module com.myapp
```

**Evolution:**
```
Java 14-16: jdk.incubator.foreign (incubator)
Java 17-21: jdk.incubator.foreign → java.foreign (preview)
Java 22+:   java.foreign (stable)
```

---

**Q73: Comment documenter un module avec Javadoc?**

**R:**

```java
/**
 * Module providing core APIs for the application.
 * 
 * <p>This module exports the following packages:
 * <ul>
 *   <li>{@code com.myapp.api} - Public API</li>
 *   <li>{@code com.myapp.util} - Utility classes</li>
 * </ul>
 * 
 * @since 1.0
 * @see com.myapp.api.Service
 */
module com.myapp {
    exports com.myapp.api;
    exports com.myapp.util;
    
    requires transitive java.sql;
    requires java.logging;
}

// Générer Javadoc
javadoc -d docs \
        --module-path mods \
        --module com.myapp \
        --module-source-path src
```

---

**Q74: Comment gérer plusieurs versions du même module sur le module path?**

**R:** **Impossible!** Java charge seulement **une version** d'un module.

```bash
# module-path avec 2 versions
mods/
├── mylib-1.0.jar
└── mylib-2.0.jar

# Java charge seulement une (généralement la première trouvée)
java --module-path mods --list-modules
# mylib@1.0  (ou 2.0, non déterministe)

# Solution: Utiliser des noms de modules différents
module com.myapp.mylib.v1 { }
module com.myapp.mylib.v2 { }
```

**Best practice:** Gérer les versions avec Maven/Gradle, pas le module system.

---

**Q75: Comment créer un module pour Spring Boot?**

**R:**

**module-info.java:**
```java
module com.myapp {
    // Spring
    requires spring.boot;
    requires spring.boot.autoconfigure;
    requires spring.web;
    requires spring.context;
    
    // Ouvrir pour Spring (component scanning, reflection)
    opens com.myapp to spring.core, spring.beans, spring.context;
    opens com.myapp.controller to spring.core, spring.beans, spring.web;
    opens com.myapp.service to spring.core, spring.beans;
    opens com.myapp.model to spring.core, com.fasterxml.jackson.databind;
    
    // Exporter API si nécessaire
    exports com.myapp.api;
}
```

**Application:**
```java
package com.myapp;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class Application {
    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }
}
```

**Alternative: open module (plus simple)**
```java
open module com.myapp {
    requires spring.boot;
    requires spring.boot.autoconfigure;
    requires spring.web;
    
    exports com.myapp.api;
}
```
