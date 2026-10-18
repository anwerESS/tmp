## [**..**](./00_index.md)

# Chapter 7: Beyond Classes

## Implementing Interfaces

### Interface Basics

Une **interface** définit un **contrat** : un ensemble de méthodes que les classes implémentantes doivent fournir.

**Syntaxe**:
```java
public interface Volant {
    void voler();
    void atterrir();
}
```

**Implémentation**:
```java
public class Oiseau implements Volant {
    @Override
    public void voler() {
        System.out.println("L'oiseau vole");
    }
    
    @Override
    public void atterrir() {
        System.out.println("L'oiseau atterrit");
    }
}
```

### Interface vs Abstract Class

| Interface | Abstract Class |
|-----------|----------------|
| `interface` keyword | `abstract class` keyword |
| Tous les membres sont `public` par défaut | Peut avoir n'importe quel access modifier |
| Méthodes abstraites par défaut | Peut avoir méthodes concrètes et abstraites |
| Pas de constructors | Peut avoir constructors |
| Héritage **multiple** possible | Héritage **simple** uniquement |
| Pas d'instance variables (sauf constants) | Peut avoir instance variables |

```java
// Interface
public interface A { }
public interface B { }
public class C implements A, B { }  // OK: multiple interfaces

// Abstract class
public abstract class X { }
public abstract class Y { }
// public class Z extends X, Y { }  // ERREUR: héritage simple seulement
```

### Interface Members

**Avant Java 8**: Uniquement méthodes abstraites et constantes.

**Java 8+**: 
- Méthodes **default**
- Méthodes **static**

**Java 9+**:
- Méthodes **private**

#### Abstract Methods (implicit)

Dans une interface, les méthodes sont **public abstract** par défaut.

```java
public interface Example {
    void method1();                  // public abstract implicite
    public void method2();           // public abstract explicite
    abstract void method3();         // public abstract explicite
    public abstract void method4();  // Tout explicite
}
```

**Équivalent**:
```java
public interface Example {
    public abstract void method1();
    public abstract void method2();
    public abstract void method3();
    public abstract void method4();
}
```

#### Constants (Variables)

Les variables d'interface sont **public static final** par défaut.

```java
public interface Configuration {
    int MAX_USERS = 100;                        // public static final implicite
    public static final String APP_NAME = "MyApp";  // Tout explicite
}

// Utilisation
System.out.println(Configuration.MAX_USERS);
System.out.println(Configuration.APP_NAME);
```

**Important**: Les constantes doivent être **initialisées** à la déclaration.

```java
public interface Example {
    int x = 10;      // OK
    // int y;        // ERREUR: must be initialized
}
```

#### Default Methods (Java 8+)

**Default methods**: méthodes avec implémentation dans l'interface.

```java
public interface Volant {
    void voler();  // Abstraite
    
    // Default method
    default void seReposer() {
        System.out.println("Se repose au sol");
    }
}

public class Oiseau implements Volant {
    @Override
    public void voler() {
        System.out.println("Vole");
    }
    
    // seReposer() est héritée (pas besoin de l'implémenter)
}

Oiseau o = new Oiseau();
o.voler();        // "Vole"
o.seReposer();    // "Se repose au sol"
```

**Override optionnel**:
```java
public class Avion implements Volant {
    @Override
    public void voler() {
        System.out.println("Avion vole");
    }
    
    // Override de la default method
    @Override
    public void seReposer() {
        System.out.println("Se gare au hangar");
    }
}
```

**Pourquoi?** Permet d'ajouter des méthodes aux interfaces existantes sans casser le code des classes implémentantes.

#### Static Methods (Java 8+)

**Static methods**: méthodes utilitaires liées à l'interface.

```java
public interface Calculateur {
    int calculer(int a, int b);
    
    // Static method
    static int additionner(int a, int b) {
        return a + b;
    }
    
    static int multiplier(int a, int b) {
        return a * b;
    }
}

// Utilisation (via le nom de l'interface)
int somme = Calculateur.additionner(5, 3);      // 8
int produit = Calculateur.multiplier(5, 3);     // 15
```

**Important**: Les static methods ne sont **pas héritées**.

```java
public class MaClasse implements Calculateur {
    @Override
    public int calculer(int a, int b) {
        return a + b;
    }
}

MaClasse obj = new MaClasse();
// obj.additionner(5, 3);  // ERREUR: static method not inherited
Calculateur.additionner(5, 3);  // OK
```

#### Private Methods (Java 9+)

**Private methods**: code partagé entre les default methods.

```java
public interface Logger {
    default void logInfo(String message) {
        log("INFO", message);
    }
    
    default void logError(String message) {
        log("ERROR", message);
    }
    
    // Private method (helper)
    private void log(String level, String message) {
        System.out.println("[" + level + "] " + message);
    }
}
```

**Private static methods** aussi possibles:
```java
public interface Utilitaire {
    default void method1() {
        helper();  // Appelle private static
    }
    
    private static void helper() {
        System.out.println("Helper");
    }
}
```

### Implementing Multiple Interfaces

Une classe peut implémenter **plusieurs interfaces**.

```java
public interface Nageur {
    void nager();
}

public interface Volant {
    void voler();
}

public class Canard implements Nageur, Volant {
    @Override
    public void nager() {
        System.out.println("Canard nage");
    }
    
    @Override
    public void voler() {
        System.out.println("Canard vole");
    }
}

Canard c = new Canard();
c.nager();
c.voler();
```

### Interface Inheritance

Une interface peut **étendre** (extends) une ou plusieurs interfaces.

```java
public interface Animal {
    void manger();
}

public interface Mammifere extends Animal {
    void allaiter();
}

public interface Volant extends Animal {
    void voler();
}

// Interface qui étend plusieurs interfaces
public interface ChauvesSouris extends Mammifere, Volant {
    void echolocaliser();
}

// Implémentation
public class Roussette implements ChauvesSouris {
    @Override
    public void manger() { }
    
    @Override
    public void allaiter() { }
    
    @Override
    public void voler() { }
    
    @Override
    public void echolocaliser() { }
}
```

### Conflicts with Default Methods

Que se passe-t-il si plusieurs interfaces ont des default methods avec la même signature?

```java
public interface A {
    default void method() {
        System.out.println("A");
    }
}

public interface B {
    default void method() {
        System.out.println("B");
    }
}

// ERREUR de compilation: doit résoudre le conflit
// public class C implements A, B { }

// Résolution du conflit
public class C implements A, B {
    @Override
    public void method() {
        A.super.method();  // Appelle la version de A
        // OU
        // B.super.method();  // Appelle la version de B
        // OU
        // System.out.println("C");  // Propre implémentation
    }
}
```

### Functional Interfaces

Une **functional interface** a **exactement une** méthode abstraite.

**@FunctionalInterface** annotation (optionnelle mais recommandée):

```java
@FunctionalInterface
public interface Calculateur {
    int calculer(int a, int b);  // Une seule méthode abstraite
    
    // Default et static methods OK
    default void afficher() {
        System.out.println("Calculateur");
    }
    
    static void log() {
        System.out.println("Log");
    }
}
```

**Utilisées avec les lambdas** (Chapter 8):
```java
Calculateur addition = (a, b) -> a + b;
System.out.println(addition.calculer(5, 3));  // 8
```

**Erreur si plus d'une méthode abstraite**:
```java
@FunctionalInterface
public interface Invalid {
    void method1();
    void method2();  // ERREUR avec @FunctionalInterface
}
```

## Working with Enums

### Enum Basics

Un **enum** est un type spécial pour représenter un ensemble **fixe** de constantes.

**Syntaxe basique**:
```java
public enum Jour {
    LUNDI, MARDI, MERCREDI, JEUDI, VENDREDI, SAMEDI, DIMANCHE
}

// Utilisation
Jour jour = Jour.LUNDI;
System.out.println(jour);  // LUNDI
```

### Enum Methods

Tous les enums héritent de `java.lang.Enum` et ont des méthodes utiles.

#### values()

Retourne un **array** de toutes les valeurs.

```java
for (Jour j : Jour.values()) {
    System.out.println(j);
}
// LUNDI
// MARDI
// ...
```

#### valueOf(String)

Convertit un String en enum (case-sensitive).

```java
Jour j = Jour.valueOf("LUNDI");  // OK
System.out.println(j);           // LUNDI

// Jour j2 = Jour.valueOf("lundi");  // IllegalArgumentException
// Jour j3 = Jour.valueOf("INVALID"); // IllegalArgumentException
```

#### ordinal()

Retourne la **position** (0-based).

```java
System.out.println(Jour.LUNDI.ordinal());     // 0
System.out.println(Jour.MARDI.ordinal());     // 1
System.out.println(Jour.DIMANCHE.ordinal());  // 6
```

#### name()

Retourne le **nom** de la constante.

```java
Jour j = Jour.LUNDI;
System.out.println(j.name());  // "LUNDI"
```

### Enum in switch

Les enums fonctionnent parfaitement avec `switch`.

```java
public static void afficherType(Jour jour) {
    switch (jour) {
        case LUNDI:
        case MARDI:
        case MERCREDI:
        case JEUDI:
        case VENDREDI:
            System.out.println("Jour de semaine");
            break;
        case SAMEDI:
        case DIMANCHE:
            System.out.println("Week-end");
            break;
    }
}

afficherType(Jour.LUNDI);    // "Jour de semaine"
afficherType(Jour.SAMEDI);   // "Week-end"
```

**Avec switch expression** (Java 14+):
```java
String type = switch (jour) {
    case LUNDI, MARDI, MERCREDI, JEUDI, VENDREDI -> "Jour de semaine";
    case SAMEDI, DIMANCHE -> "Week-end";
};
```

### Enum with Fields and Methods

Les enums peuvent avoir des **fields**, **constructors**, et **methods**.

```java
public enum Planete {
    MERCURE(3.303e+23, 2.4397e6),
    VENUS(4.869e+24, 6.0518e6),
    TERRE(5.976e+24, 6.37814e6),
    MARS(6.421e+23, 3.3972e6);
    
    private final double masse;      // en kilogrammes
    private final double rayon;      // en mètres
    
    // Constructor (toujours private ou package-private)
    Planete(double masse, double rayon) {
        this.masse = masse;
        this.rayon = rayon;
    }
    
    // Methods
    public double getMasse() {
        return masse;
    }
    
    public double getRayon() {
        return rayon;
    }
    
    public double gravite() {
        final double G = 6.67300E-11;
        return G * masse / (rayon * rayon);
    }
}

// Utilisation
Planete p = Planete.TERRE;
System.out.println("Masse: " + p.getMasse());
System.out.println("Gravité: " + p.gravite());
```

### Enum with Abstract Methods

Les enums peuvent avoir des **méthodes abstraites** que chaque constante doit implémenter.

```java
public enum Operation {
    ADDITION {
        @Override
        public double calculer(double a, double b) {
            return a + b;
        }
    },
    SOUSTRACTION {
        @Override
        public double calculer(double a, double b) {
            return a - b;
        }
    },
    MULTIPLICATION {
        @Override
        public double calculer(double a, double b) {
            return a * b;
        }
    },
    DIVISION {
        @Override
        public double calculer(double a, double b) {
            return a / b;
        }
    };
    
    // Méthode abstraite
    public abstract double calculer(double a, double b);
}

// Utilisation
double resultat = Operation.ADDITION.calculer(5, 3);  // 8
System.out.println(resultat);
```

### Enum Rules

**Règles importantes**:
1. Ne peut **pas** étendre une classe (étend déjà `Enum`)
2. Peut **implémenter** des interfaces
3. Constructor est **implicitement private**
4. Ne peut **pas** être instantié avec `new`
5. Les constantes doivent être déclarées **en premier**

```java
public enum Exemple {
    // Constantes en premier
    VALEUR1, VALEUR2;
    
    // Puis fields, constructors, methods
    private int x;
    
    // public Exemple() { }  // ERREUR: doit être private
    private Exemple() {
        this.x = 0;
    }
}

// Exemple ex = new Exemple();  // ERREUR: cannot instantiate
```

## Sealing Classes

### Sealed Classes (Java 17+)

Une **sealed class** contrôle **quelles classes** peuvent l'étendre.

**Syntaxe**:
```java
public sealed class Animal permits Chien, Chat, Oiseau {
    // ...
}

// Seules ces classes peuvent étendre Animal
public final class Chien extends Animal { }
public final class Chat extends Animal { }
public final class Oiseau extends Animal { }

// public class Lapin extends Animal { }  // ERREUR: not permitted
```

### Sealed Interface

Les interfaces peuvent aussi être sealed.

```java
public sealed interface Forme permits Cercle, Rectangle, Triangle {
    double calculerAire();
}

public final class Cercle implements Forme {
    private double rayon;
    
    @Override
    public double calculerAire() {
        return Math.PI * rayon * rayon;
    }
}

public final class Rectangle implements Forme {
    private double longueur, largeur;
    
    @Override
    public double calculerAire() {
        return longueur * largeur;
    }
}

public non-sealed class Triangle implements Forme {
    // non-sealed: permet l'extension
    @Override
    public double calculerAire() {
        return 0;
    }
}
```

### Permitted Subclasses Rules

Les sous-classes d'une sealed class doivent être:
1. **final** (ne peut plus être étendue)
2. **sealed** (contrôle ses propres sous-classes)
3. **non-sealed** (ouvre l'héritage)

```java
public sealed class A permits B, C, D { }

public final class B extends A { }        // OK: final
public sealed class C extends A permits E { }  // OK: sealed
public non-sealed class D extends A { }   // OK: non-sealed

public class E extends C { }              // ERREUR: E doit etre (final, sealed ou non sealed)
public class F extends D { }              // OK: D est non-sealed
// public class G extends B { }           // ERREUR: B est final
```

### Benefits of Sealed Classes

**Avantages**:
1. **Pattern matching** exhaustif
2. Contrôle de la hiérarchie
3. Documentation claire des sous-types

```java
public sealed interface Resultat permits Succes, Erreur { }

public record Succes(String data) implements Resultat { }
public record Erreur(String message) implements Resultat { }

// Pattern matching exhaustif (Java 17+)
public static void traiter(Resultat r) {
    switch (r) {
        case Succes s -> System.out.println("Data: " + s.data());
        case Erreur e -> System.out.println("Erreur: " + e.message());
        // Pas besoin de default: tous les cas couverts
    }
}
```

## Encapsulating Data with Records

### Record Basics (Java 16+)

Un **record** est une classe **immuable** pour stocker des données (équivalent à un data class).

**Syntaxe**:
```java
public record Point(int x, int y) { }
```

**Génère automatiquement**:
- Constructor avec tous les paramètres
- Getters (sans préfixe "get")
- `equals()`, `hashCode()`, `toString()`
- Tous les fields sont `private final`

**Utilisation**:
```java
Point p = new Point(3, 4);
System.out.println(p.x());        // 3 (getter)
System.out.println(p.y());        // 4 (getter)
System.out.println(p);            // Point[x=3, y=4]

Point p2 = new Point(3, 4);
System.out.println(p.equals(p2)); // true
```

### Record vs Class

**Sans record**:
```java
public final class Point {
    private final int x;
    private final int y;
    
    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }
    
    public int x() { return x; }
    public int y() { return y; }
    
    @Override
    public boolean equals(Object obj) {
        if (!(obj instanceof Point)) return false;
        Point other = (Point) obj;
        return x == other.x && y == other.y;
    }
    
    @Override
    public int hashCode() {
        return Objects.hash(x, y);
    }
    
    @Override
    public String toString() {
        return "Point[x=" + x + ", y=" + y + "]";
    }
}
```

**Avec record** (équivalent):
```java
public record Point(int x, int y) { }
```

### Customizing Records

**Compact constructor**:
```java
public record Point(int x, int y) {
    // Compact constructor (validation)
    public Point {
        if (x < 0 || y < 0) {
            throw new IllegalArgumentException("Coordonnées négatives");
        }
    }
}
```

**Méthodes additionnelles**:
```java
public record Point(int x, int y) {
    // Méthodes custom
    public double distanceOrigine() {
        return Math.sqrt(x * x + y * y);
    }
    
    public Point deplacer(int dx, int dy) {
        return new Point(x + dx, y + dy);
    }
}

Point p = new Point(3, 4);
System.out.println(p.distanceOrigine());  // 5.0
Point p2 = p.deplacer(1, 1);
System.out.println(p2);                   // Point[x=4, y=5]
```

**Override des méthodes générées**:
```java
public record Personne(String nom, int age) {
    @Override
    public String toString() {
        return nom + " (" + age + " ans)";
    }
}
```

### Record Rules

**Règles**:
1. Tous les fields sont **implicitly final**
2. Ne peut **pas** étendre une classe (mais peut implémenter des interfaces)
3. Ne peut **pas** déclarer d'instance variables en dehors du header
4. Ne peut **pas** être **abstract**
5. Est **implicitly final** (ne peut pas être étendu)

```java
// public record R() extends SomeClass { }  // ERREUR
public record R() implements SomeInterface { }  // OK

public record R(int x) {
    // private int y;  // ERREUR: instance variable
    private static int count;  // OK: static variable
}
```

### Records with Collections

```java
import java.util.*;

public record Equipe(String nom, List<String> joueurs) {
    // Compact constructor avec copie défensive
    public Equipe {
        joueurs = List.copyOf(joueurs);  // Immuable
    }
}

List<String> liste = new ArrayList<>();
liste.add("Alice");
liste.add("Bob");

Equipe e = new Equipe("Team A", liste);
liste.add("Charlie");  // N'affecte pas e

System.out.println(e.joueurs());  // [Alice, Bob]
```

## Creating Nested Classes

### Types of Nested Classes

Java a **4 types** de nested classes:

1. **Inner class** (non-static member class)
2. **Static nested class**
3. **Local class**
4. **Anonymous class**

### Inner Classes (Non-Static Member Classes)

**Inner class**: classe définie dans une autre classe, **non-static**.

```java
public class Exterieure {
    private int x = 10;
    
    // Inner class
    public class Interieure {
        private int y = 20;
        
        public void afficher() {
            System.out.println("x = " + x);  // Accès aux membres de la classe extérieure
            System.out.println("y = " + y);
        }
    }
    
    public void creerInterieure() {
        Interieure inner = new Interieure();
        inner.afficher();
    }
}

// Utilisation
Exterieure ext = new Exterieure();
ext.creerInterieure();

// Créer une inner class depuis l'extérieur
Exterieure.Interieure inner = ext.new Interieure();
inner.afficher();
```

**Caractéristiques**:
- A accès à **tous les membres** de la classe extérieure (même private)
- Ne peut **pas** avoir de membres static (sauf constantes) (⚠️ avant Java 16)
- Nécessite une **instance** de la classe extérieure

### Static Nested Classes

**Static nested class**: classe définie dans une autre classe, **static**.

```java
public class Exterieure {
    private static int x = 10;
    private int y = 20;
    
    // Static nested class
    public static class Nested {
        public void afficher() {
            System.out.println("x = " + x);  // OK: x est static
            // System.out.println("y = " + y);  // ERREUR: y n'est pas static
        }
    }
}

// Utilisation (pas besoin d'instance de Exterieure)
Exterieure.Nested nested = new Exterieure.Nested();
nested.afficher();
```

**Caractéristiques**:
- N'a **pas** accès aux membres d'instance de la classe extérieure
- Peut avoir des membres **static**
- Ne nécessite **pas** d'instance de la classe extérieure

### Local Classes

**Local class**: classe définie dans un **bloc** (méthode, constructor, initializer).

```java
public class Exterieure {
    private int x = 10;
    
    public void method() {
        final int y = 20;  // Effectively final
        
        // Local class
        class Locale {
            public void afficher() {
                System.out.println("x = " + x);  // Accès à la classe extérieure
                System.out.println("y = " + y);  // Accès aux variables locales final/effectively final
            }
        }
        
        Locale locale = new Locale();
        locale.afficher();
    }
}
```

**Caractéristiques**:
- Visible uniquement dans le **bloc** où elle est définie
- A accès aux variables locales **final** ou **effectively final**
- Peut accéder aux membres de la classe extérieure

### Anonymous Classes

**Anonymous class**: classe **sans nom**, définie et instantiée en une seule expression.

**Avec interface**:
```java
public interface Calculateur {
    int calculer(int a, int b);
}

public class Test {
    public static void main(String[] args) {
        // Anonymous class
        Calculateur addition = new Calculateur() {
            @Override
            public int calculer(int a, int b) {
                return a + b;
            }
        };
        
        System.out.println(addition.calculer(5, 3));  // 8
    }
}
```

**Avec classe abstraite**:
```java
public abstract class Animal {
    public abstract void faireDuBruit();
}

Animal chien = new Animal() {
    @Override
    public void faireDuBruit() {
        System.out.println("Woof!");
    }
};

chien.faireDuBruit();  // "Woof!"
```

**Caractéristiques**:
- Pas de nom
- Utilisée pour des implémentations **one-time**
- Peut avoir des variables locales et methods
- A accès aux membres de la classe extérieure et variables locales final/effectively final

### Nested Class Access

| Type | Accès aux membres static externes | Accès aux membres d'instance externes | Peut avoir membres static |
|------|-----------------------------------|---------------------------------------|---------------------------|
| Inner class | ✅ | ✅ | ❌ (sauf constantes (⚠️ avant Java 16)) |
| Static nested | ✅ | ❌ | ✅ |
| Local class | ✅ | ✅ | ❌ |
| Anonymous class | ✅ | ✅ | ❌ |

## Understanding Polymorphism

### Polymorphism Basics

**Polymorphisme**: capacité d'un objet à prendre plusieurs formes.

```java
public class Animal {
    public void faireDuBruit() {
        System.out.println("Animal bruit");
    }
}

public class Chien extends Animal {
    @Override
    public void faireDuBruit() {
        System.out.println("Woof!");
    }
}

public class Chat extends Animal {
    @Override
    public void faireDuBruit() {
        System.out.println("Meow!");
    }
}

// Polymorphisme
Animal a1 = new Chien();
Animal a2 = new Chat();
Animal a3 = new Animal();

a1.faireDuBruit();  // "Woof!"
a2.faireDuBruit();  // "Meow!"
a3.faireDuBruit();  // "Animal bruit"
```

### Object Type vs Reference Type

```java
Animal animal = new Chien();
```

- **Reference type**: `Animal` (type de la variable)
- **Object type**: `Chien` (type réel de l'objet)

**Règle**:
- Les **méthodes** appelées dépendent du **object type** (polymorphisme)
- Les **variables** accédées dépendent du **reference type**
- La **compilation** vérifie le **reference type**
- L'**exécution** utilise le **object type**

```java
public class Animal {
    public void manger() {
        System.out.println("Animal mange");
    }
}

public class Chien extends Animal {
    @Override
    public void manger() {
        System.out.println("Chien mange");
    }
    
    public void aboyer() {
        System.out.println("Woof!");
    }
}

Animal a = new Chien();
a.manger();      // "Chien mange" (polymorphisme)
// a.aboyer();   // ERREUR de compilation: Animal n'a pas aboyer()

// Cast nécessaire
((Chien)a).aboyer();  // OK: "Woof!"
```

### Casting Objects

**Upcasting** (implicite): sous-type → super-type
```java
Chien chien = new Chien();
Animal animal = chien;  // Upcasting implicite
```

**Downcasting** (explicite): super-type → sous-type
```java
Animal animal = new Chien();
Chien chien = (Chien)animal;  // Downcasting explicite
chien.aboyer();  // OK
```

**ClassCastException** si le cast est invalide:
```java
Animal animal = new Chat();
// Chien chien = (Chien)animal;  // ClassCastException à l'exécution!
```

**Vérifier avant de caster** avec `instanceof`:
```java
Animal animal = new Chat();

if (animal instanceof Chien) {
    Chien chien = (Chien)animal;
    chien.aboyer();
} else {
    System.out.println("Pas un chien");
}
```

### Polymorphism with Interfaces

```java
public interface Nageur {
    void nager();
}

public class Poisson implements Nageur {
    @Override
    public void nager() {
        System.out.println("Poisson nage");
    }
}

public class Humain implements Nageur {
    @Override
    public void nager() {
        System.out.println("Humain nage");
    }
}

// Polymorphisme
Nageur n1 = new Poisson();
Nageur n2 = new Humain();

n1.nager();  // "Poisson nage"
n2.nager();  // "Humain nage"

// Array polymorphique
Nageur[] nageurs = {new Poisson(), new Humain(), new Poisson()};
for (Nageur n : nageurs) {
    n.nager();
}
```

---

## Exemples Pratiques Complets

### Exemple 1: Système de Paiement avec Interfaces
```java
public interface Payable {
    double getMontant();
    void payer();
    
    default void afficherRecap() {
        System.out.println("Montant à payer: " + getMontant() + "€");
    }
}

public class Facture implements Payable {
    private String numero;
    private double montant;
    private boolean payee;
    
    public Facture(String numero, double montant) {
        this.numero = numero;
        this.montant = montant;
        this.payee = false;
    }
    
    @Override
    public double getMontant() {
        return montant;
    }
    
    @Override
    public void payer() {
        if (!payee) {
            System.out.println("Facture " + numero + " payée: " + montant + "€");
            payee = true;
        } else {
            System.out.println("Facture déjà payée");
        }
    }
    
    @Override
    public void afficherRecap() {
        System.out.println("Facture #" + numero);
        Payable.super.afficherRecap();
        System.out.println("Statut: " + (payee ? "Payée" : "Impayée"));
    }
}

public class Salaire implements Payable {
    private String employe;
    private double montantBrut;
    private double tauxCotisation = 0.23;
    
    public Salaire(String employe, double montantBrut) {
        this.employe = employe;
        this.montantBrut = montantBrut;
    }
    
    @Override
    public double getMontant() {
        return montantBrut * (1 - tauxCotisation);
    }
    
    @Override
    public void payer() {
        System.out.println("Salaire versé à " + employe + ": " + getMontant() + "€");
    }
    
    @Override
    public void afficherRecap() {
        System.out.println("Employé: " + employe);
        System.out.println("Brut: " + montantBrut + "€");
        System.out.println("Net: " + getMontant() + "€");
    }
}

public class GestionnairePaiements {
    private List<Payable> paiements = new ArrayList<>();
    
    public void ajouterPaiement(Payable paiement) {
        paiements.add(paiement);
    }
    
    public void payerTout() {
        for (Payable p : paiements) {
            p.afficherRecap();
            p.payer();
            System.out.println("---");
        }
    }
    
    public double calculerTotal() {
        double total = 0;
        for (Payable p : paiements) {
            total += p.getMontant();
        }
        return total;
    }
    
    public static void main(String[] args) {
        GestionnairePaiements gp = new GestionnairePaiements();
        
        gp.ajouterPaiement(new Facture("F001", 1500.0));
        gp.ajouterPaiement(new Facture("F002", 2300.0));
        gp.ajouterPaiement(new Salaire("Alice Dupont", 3000.0));
        gp.ajouterPaiement(new Salaire("Bob Martin", 3500.0));
        
        System.out.println("=== PAIEMENTS À EFFECTUER ===");
        gp.payerTout();
        
        System.out.println("\nTotal à payer: " + gp.calculerTotal() + "€");
    }
}
```

### Exemple 2: Système de Notifications avec Enum
```java
public enum NiveauUrgence {
    INFO(1, "Informative") {
        @Override
        public void traiter(String message) {
            System.out.println("[ℹ INFO] " + message);
        }
    },
    WARNING(2, "Avertissement") {
        @Override
        public void traiter(String message) {
            System.out.println("[⚠ WARNING] " + message.toUpperCase());
        }
    },
    ERROR(3, "Erreur") {
        @Override
        public void traiter(String message) {
            System.err.println("[❌ ERROR] " + message.toUpperCase());
        }
    },
    CRITIQUE(4, "Critique") {
        @Override
        public void traiter(String message) {
            System.err.println("[🚨 CRITIQUE] " + message.toUpperCase());
            System.err.println("INTERVENTION IMMEDIATE REQUISE!");
        }
    };
    
    private final int niveau;
    private final String description;
    
    NiveauUrgence(int niveau, String description) {
        this.niveau = niveau;
        this.description = description;
    }
    
    public int getNiveau() {
        return niveau;
    }
    
    public String getDescription() {
        return description;
    }
    
    // Méthode abstraite que chaque constante implémente
    public abstract void traiter(String message);
    
    public boolean estPlusUrgentQue(NiveauUrgence autre) {
        return this.niveau > autre.niveau;
    }
    
    public static NiveauUrgence parNiveau(int niveau) {
        for (NiveauUrgence nu : values()) {
            if (nu.niveau == niveau) {
                return nu;
            }
        }
        throw new IllegalArgumentException("Niveau invalide: " + niveau);
    }
}

public class Notification {
    private String message;
    private NiveauUrgence niveau;
    private LocalDateTime timestamp;
    
    public Notification(String message, NiveauUrgence niveau) {
        this.message = message;
        this.niveau = niveau;
        this.timestamp = LocalDateTime.now();
    }
    
    public void envoyer() {
        System.out.println("\n[" + timestamp.format(DateTimeFormatter.ofPattern("HH:mm:ss")) + "]");
        niveau.traiter(message);
    }
    
    public NiveauUrgence getNiveau() {
        return niveau;
    }
}

public class SystemeNotifications {
    private Queue<Notification> notifications = new LinkedList<>();
    
    public void ajouter(String message, NiveauUrgence niveau) {
        notifications.offer(new Notification(message, niveau));
    }
    
    public void traiterTout() {
        System.out.println("=== TRAITEMENT DES NOTIFICATIONS ===");
        while (!notifications.isEmpty()) {
            notifications.poll().envoyer();
        }
    }
    
    public void traiterUrgencesUniquement() {
        System.out.println("=== URGENCES UNIQUEMENT ===");
        notifications.stream()
            .filter(n -> n.getNiveau().estPlusUrgentQue(NiveauUrgence.WARNING))
            .forEach(Notification::envoyer);
    }
    
    public static void main(String[] args) {
        SystemeNotifications systeme = new SystemeNotifications();
        
        systeme.ajouter("Application démarrée", NiveauUrgence.INFO);
        systeme.ajouter("Mémoire à 80%", NiveauUrgence.WARNING);
        systeme.ajouter("Connexion base de données perdue", NiveauUrgence.ERROR);
        systeme.ajouter("Serveur inaccessible", NiveauUrgence.CRITIQUE);
        systeme.ajouter("Sauvegarde effectuée", NiveauUrgence.INFO);
        
        systeme.traiterTout();
        
        // Test des méthodes enum
        System.out.println("\n=== TEST MÉTHODES ENUM ===");
        for (NiveauUrgence niveau : NiveauUrgence.values()) {
            System.out.println(niveau + " - " + niveau.getDescription() + " (niveau " + niveau.getNiveau() + ")");
        }
    }
}
```

### Exemple 3: Sealed Classes avec Pattern Matching
```java
// Sealed interface pour les résultats
public sealed interface Resultat permits Succes, Erreur, EnAttente { }

public record Succes(String data, LocalDateTime timestamp) implements Resultat { }

public record Erreur(String message, String code, Exception cause) implements Resultat { }

public record EnAttente(String operation, int pourcentage) implements Resultat { }

// Service qui utilise les résultats
public class ServiceAPI {
    
    public static Resultat appelAPI(String endpoint) {
        // Simulation d'appels API
        double random = Math.random();
        
        if (random < 0.6) {
            return new Succes("{\"user\": \"Alice\"}", LocalDateTime.now());
        } else if (random < 0.9) {
            return new Erreur("Timeout", "ERR_TIMEOUT", new Exception("Connection timeout"));
        } else {
            return new EnAttente("Chargement données", 45);
        }
    }
    
    public static void traiterResultat(Resultat resultat) {
        // Pattern matching exhaustif (Java 17+)
        switch (resultat) {
            case Succes s -> {
                System.out.println("✅ Succès!");
                System.out.println("Data: " + s.data());
                System.out.println("Reçu à: " + s.timestamp());
            }
            case Erreur e -> {
                System.out.println("❌ Erreur!");
                System.out.println("Message: " + e.message());
                System.out.println("Code: " + e.code());
                System.out.println("Cause: " + e.cause().getMessage());
            }
            case EnAttente a -> {
                System.out.println("⏳ En attente...");
                System.out.println("Opération: " + a.operation());
                System.out.println("Progression: " + a.pourcentage() + "%");
            }
            // Pas besoin de default: exhaustif grâce à sealed
        }
    }
    
    public static void main(String[] args) {
        System.out.println("=== TEST API ===");
        for (int i = 1; i <= 5; i++) {
            System.out.println("\n--- Appel #" + i + " ---");
            Resultat r = appelAPI("/api/users");
            traiterResultat(r);
        }
    }
}
```

### Exemple 4: Records Immuables pour Données
```java
import java.util.*;

public record Adresse(String rue, String ville, String codePostal) {
    // Compact constructor avec validation
    public Adresse {
        Objects.requireNonNull(rue, "Rue requise");
        Objects.requireNonNull(ville, "Ville requise");
        if (codePostal == null || !codePostal.matches("\\d{5}")) {
            throw new IllegalArgumentException("Code postal invalide");
        }
    }
    
    public String formatee() {
        return rue + ", " + codePostal + " " + ville;
    }
}

public record Client(
    String nom,
    String email,
    Adresse adresse,
    List<String> commandes
) {
    // Compact constructor avec copie défensive
    public Client {
        Objects.requireNonNull(nom);
        Objects.requireNonNull(email);
        Objects.requireNonNull(adresse);
        commandes = commandes != null ? List.copyOf(commandes) : List.of();
    }
    
    // Constructor alternatif
    public Client(String nom, String email, Adresse adresse) {
        this(nom, email, adresse, List.of());
    }
    
    // Méthode pour ajouter une commande (retourne un nouveau Client)
    public Client avecCommande(String commande) {
        List<String> nouvellesCommandes = new ArrayList<>(commandes);
        nouvellesCommandes.add(commande);
        return new Client(nom, email, adresse, nouvellesCommandes);
    }
    
    public int nombreCommandes() {
        return commandes.size();
    }
    
    @Override
    public String toString() {
        return String.format("Client[nom=%s, email=%s, adresse=%s, %d commandes]",
            nom, email, adresse.formatee(), nombreCommandes());
    }
}

public class GestionClients {
    public static void main(String[] args) {
        // Créer une adresse
        Adresse adresse = new Adresse("123 Rue de la Paix", "Paris", "75001");
        
        // Créer un client
        Client client1 = new Client("Alice Dupont", "alice@example.com", adresse);
        System.out.println(client1);
        
        // Ajouter des commandes (crée de nouveaux objets)
        Client client2 = client1.avecCommande("CMD001");
        Client client3 = client2.avecCommande("CMD002");
        
        System.out.println("\n=== IMMUTABILITÉ ===");
        System.out.println("Client1: " + client1.nombreCommandes() + " commandes");
        System.out.println("Client2: " + client2.nombreCommandes() + " commandes");
        System.out.println("Client3: " + client3.nombreCommandes() + " commandes");
        
        // Test d'égalité
        Client client4 = new Client("Alice Dupont", "alice@example.com", adresse);
        System.out.println("\n=== ÉGALITÉ ===");
        System.out.println("client1.equals(client4): " + client1.equals(client4));  // true
        
        // Test de copie défensive
        List<String> liste = new ArrayList<>();
        liste.add("CMD999");
        Client client5 = new Client("Bob", "bob@example.com", adresse, liste);
        liste.add("CMD888");  // N'affecte pas client5
        System.out.println("\n=== COPIE DÉFENSIVE ===");
        System.out.println("Client5: " + client5.commandes());  // [CMD999] seulement
    }
}
```

### Exemple 5: Nested Classes - Builder Pattern
```java
public class Ordinateur {
    // Attributs obligatoires
    private final String processeur;
    private final int ram;
    
    // Attributs optionnels
    private final int stockage;
    private final String carteGraphique;
    private final boolean bluetooth;
    private final boolean wifi;
    
    // Constructor privé
    private Ordinateur(Builder builder) {
        this.processeur = builder.processeur;
        this.ram = builder.ram;
        this.stockage = builder.stockage;
        this.carteGraphique = builder.carteGraphique;
        this.bluetooth = builder.bluetooth;
        this.wifi = builder.wifi;
    }
    
    // Static nested class Builder
    public static class Builder {
        // Obligatoires
        private final String processeur;
        private final int ram;
        
        // Optionnels (valeurs par défaut)
        private int stockage = 256;
        private String carteGraphique = "Intégrée";
        private boolean bluetooth = false;
        private boolean wifi = true;
        
        public Builder(String processeur, int ram) {
            this.processeur = processeur;
            this.ram = ram;
        }
        
        public Builder stockage(int stockage) {
            this.stockage = stockage;
            return this;
        }
        
        public Builder carteGraphique(String carteGraphique) {
            this.carteGraphique = carteGraphique;
            return this;
        }
        
        public Builder bluetooth(boolean bluetooth) {
            this.bluetooth = bluetooth;
            return this;
        }
        
        public Builder wifi(boolean wifi) {
            this.wifi = wifi;
            return this;
        }
        
        public Ordinateur build() {
            return new Ordinateur(this);
        }
    }
    
    @Override
    public String toString() {
        return String.format(
            "Ordinateur[processeur=%s, ram=%dGB, stockage=%dGB, " +
            "carteGraphique=%s, bluetooth=%s, wifi=%s]",
            processeur, ram, stockage, carteGraphique, bluetooth, wifi
        );
    }
    
    public static void main(String[] args) {
        // Construction avec Builder
        Ordinateur pc1 = new Ordinateur.Builder("Intel i7", 16)
            .stockage(512)
            .carteGraphique("NVIDIA RTX 3060")
            .bluetooth(true)
            .build();
        
        Ordinateur pc2 = new Ordinateur.Builder("AMD Ryzen 5", 8)
            .stockage(1000)
            .build();
        
        Ordinateur pc3 = new Ordinateur.Builder("Intel i5", 32)
            .carteGraphique("NVIDIA RTX 4090")
            .bluetooth(true)
            .wifi(true)
            .build();
        
        System.out.println(pc1);
        System.out.println(pc2);
        System.out.println(pc3);
    }
}
```

### Exemple 6: Anonymous Classes et Local Classes
```java
import java.util.*;

public class TrieurCustom {
    
    public static void trierAvecComparator() {
        List<String> noms = Arrays.asList("Alice", "Charlie", "Bob", "David");
        
        // Anonymous class implémentant Comparator
        Collections.sort(noms, new Comparator<String>() {
            @Override
            public int compare(String s1, String s2) {
                return s2.length() - s1.length();  // Ordre décroissant par longueur
            }
        });
        
        System.out.println("Triés par longueur (décroissant): " + noms);
    }
    
    public static void trierAvecLocalClass() {
        List<Integer> nombres = Arrays.asList(5, 2, 8, 1, 9, 3);
        final int seuil = 5;  // Effectively final
        
        // Local class
        class ComparateurSeuil implements Comparator<Integer> {
            @Override
            public int compare(Integer n1, Integer n2) {
                // Priorité aux nombres > seuil
                boolean n1Grand = n1 > seuil;
                boolean n2Grand = n2 > seuil;
                
                if (n1Grand && !n2Grand) return -1;
                if (!n1Grand && n2Grand) return 1;
                return n1.compareTo(n2);
            }
        }
        
        Collections.sort(nombres, new ComparateurSeuil());
        System.out.println("Triés avec seuil " + seuil + ": " + nombres);
    }
    
    public static List<String> filtrerAvecPredicate(List<String> liste, String prefixe) {
        List<String> resultats = new ArrayList<>();
        
        // Anonymous class implémentant un prédicat custom
        interface Predicate {
            boolean test(String s);
        }
        
        Predicate predicate = new Predicate() {
            @Override
            public boolean test(String s) {
                return s.startsWith(prefixe) && s.length() > 3;
            }
        };
        
        for (String item : liste) {
            if (predicate.test(item)) {
                resultats.add(item);
            }
        }
        
        return resultats;
    }
    
    public static void main(String[] args) {
        System.out.println("=== ANONYMOUS CLASS ===");
        trierAvecComparator();
        
        System.out.println("\n=== LOCAL CLASS ===");
        trierAvecLocalClass();
        
        System.out.println("\n=== FILTRAGE ===");
        List<String> mots = Arrays.asList("Java", "JavaScript", "Python", "Ruby", "Go");
        List<String> filtres = filtrerAvecPredicate(mots, "Ja");
        System.out.println("Filtrés (commence par 'Ja' et longueur > 3): " + filtres);
    }
}
```

### Exemple 7: Polymorphisme Complet
```java
public interface Dessinable {
    void dessiner();
    
    default void afficherInfo() {
        System.out.println("Forme dessinable");
    }
}

public abstract class Forme implements Dessinable {
    protected String couleur;
    protected boolean rempli;
    
    public Forme(String couleur, boolean rempli) {
        this.couleur = couleur;
        this.rempli = rempli;
    }
    
    public abstract double calculerAire();
    public abstract double calculerPerimetre();
    
    @Override
    public void afficherInfo() {
        System.out.println("Couleur: " + couleur);
        System.out.println("Rempli: " + (rempli ? "Oui" : "Non"));
        System.out.println("Aire: " + calculerAire());
        System.out.println("Périmètre: " + calculerPerimetre());
    }
}

public class Cercle extends Forme {
    private double rayon;
    
    public Cercle(String couleur, boolean rempli, double rayon) {
        super(couleur, rempli);
        this.rayon = rayon;
    }
    
    @Override
    public double calculerAire() {
        return Math.PI * rayon * rayon;
    }
    
    @Override
    public double calculerPerimetre() {
        return 2 * Math.PI * rayon;
    }
    
    @Override
    public void dessiner() {
        System.out.println("Dessin d'un cercle " + couleur + " de rayon " + rayon);
    }
    
    @Override
    public void afficherInfo() {
        System.out.println("=== CERCLE ===");
        System.out.println("Rayon: " + rayon);
        super.afficherInfo();
    }
}

public class Rectangle extends Forme {
    private double longueur;
    private double largeur;
    
    public Rectangle(String couleur, boolean rempli, double longueur, double largeur) {
        super(couleur, rempli);
        this.longueur = longueur;
        this.largeur = largeur;
    }
    
    @Override
    public double calculerAire() {
        return longueur * largeur;
    }
    
    @Override
    public double calculerPerimetre() {
        return 2 * (longueur + largeur);
    }
    
    @Override
    public void dessiner() {
        System.out.println("Dessin d'un rectangle " + couleur + " " + longueur + "x" + largeur);
    }
    
    @Override
    public void afficherInfo() {
        System.out.println("=== RECTANGLE ===");
        System.out.println("Dimensions: " + longueur + "x" + largeur);
        super.afficherInfo();
    }
}

public class GestionnaireFormes {
    private List<Dessinable> formes = new ArrayList<>();
    
    public void ajouterForme(Dessinable forme) {
        formes.add(forme);
    }
    
    public void dessinerTout() {
        System.out.println("=== DESSIN DE TOUTES LES FORMES ===");
        for (Dessinable d : formes) {
            d.dessiner();
        }
    }
    
    public void afficherInfosDetaillees() {
        System.out.println("\n=== INFORMATIONS DÉTAILLÉES ===");
        for (Dessinable d : formes) {
            if (d instanceof Forme f) {  // Pattern matching (Java 16+)
                f.afficherInfo();
                System.out.println();
            }
        }
    }
    
    public double calculerAireTotale() {
        double total = 0;
        for (Dessinable d : formes) {
            if (d instanceof Forme f) {
                total += f.calculerAire();
            }
        }
        return total;
    }
    
    public static void main(String[] args) {
        GestionnaireFormes gf = new GestionnaireFormes();
        
        // Polymorphisme: tous stockés comme Dessinable
        gf.ajouterForme(new Cercle("Rouge", true, 5));
        gf.ajouterForme(new Rectangle("Bleu", false, 10, 5));
        gf.ajouterForme(new Cercle("Vert", false, 3));
        
        gf.dessinerTout();
        gf.afficherInfosDetaillees();
        
        System.out.println("Aire totale: " + gf.calculerAireTotale());
    }
}
```

---

## Points Clés du Chapter 7

✅ **Interfaces**:
   - Contrat de méthodes à implémenter
   - Héritage multiple possible
   - Membres `public` par défaut
   - Variables sont `public static final`
   - Default methods (Java 8+): implémentation dans l'interface
   - Static methods (Java 8+): méthodes utilitaires
   - Private methods (Java 9+): helpers internes

✅ **Enums**:
   - Type pour constantes fixes
   - Méthodes: `values()`, `valueOf()`, `ordinal()`, `name()`
   - Peut avoir fields, constructors (private), methods
   - Peut implémenter des interfaces
   - Méthodes abstraites possibles

✅ **Sealed Classes** (Java 17+):
   - Contrôle des sous-classes avec `permits`
   - Sous-classes doivent être `final`, `sealed`, ou `non-sealed`
   - Pattern matching exhaustif

✅ **Records** (Java 16+):
   - Classes immuables pour données (dataclass), final implicitement
   - peut etendre des inteface mais pas des classes
   - Génère automatiquement: constructor, getters, `equals()`, `hashCode()`, `toString()`
   - Fields `private final` implicites
   - Compact constructor pour validation
   - Copie défensive nécessaire pour objets mutables

✅ **Nested Classes**:
   - **Inner class**: accès aux membres externes, nécessite instance
   - **Static nested class**: pas d'accès aux membres d'instance externes
   - **Local class**: définie dans un bloc, accès aux variables final/effectively final
   - **Anonymous class**: sans nom, usage unique

✅ **Polymorphisme**:
   - Objet prend plusieurs formes
   - Méthodes appelées selon object type (runtime)
   - Variables accédées selon reference type (compile-time)
   - Upcasting implicite, downcasting explicite
   - Vérifier avec `instanceof` avant cast

---