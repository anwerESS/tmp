## [**..**](./00_index.md)

# Chapter 6: Class Design

## Understanding Inheritance

**Inheritance** (héritage): permet à une classe de **réutiliser** le code d'une autre classe.

### Basic Inheritance

**Syntaxe**: `class SousClasse extends SuperClasse`

```java
// Superclass (classe parent)
public class Animal {
    protected String nom;
    
    public void manger() {
        System.out.println(nom + " mange");
    }
}

// Subclass (classe enfant)
public class Chien extends Animal {
    public void aboyer() {
        System.out.println(nom + " aboie");
    }
}

// Utilisation
Chien chien = new Chien();
chien.nom = "Rex";
chien.manger();      // Hérité de Animal
chien.aboyer();      // Défini dans Chien
```

### Single Inheritance

Java supporte **l'héritage simple** uniquement: une classe ne peut étendre qu'**une seule** classe.

```java
public class A { }
public class B { }

// public class C extends A, B { }  // ERREUR: multiple inheritance interdit

public class C extends A { }        // OK
```

**Mais**: une classe peut implémenter **plusieurs interfaces** (voir Chapter 7).

### The Object Class

**Toutes les classes** héritent de `java.lang.Object` (implicitement).

```java
public class MaClasse { }
// Équivalent à:
public class MaClasse extends Object { }
```

**Méthodes héritées d'Object**:
- `toString()`: représentation String de l'objet
- `equals(Object obj)`: comparaison d'égalité
- `hashCode()`: code de hachage
- `getClass()`: information sur la classe
- `clone()`: copie de l'objet
- etc.

```java
Object obj = new Object();
System.out.println(obj.toString());        // java.lang.Object@hashcode
System.out.println(obj.getClass());        // class java.lang.Object
```

### Inheritance Hierarchy

```java
public class Animal {
    public void respirer() {
        System.out.println("Respire");
    }
}

public class Mammifere extends Animal {
    public void allaiter() {
        System.out.println("Allaite");
    }
}

public class Chien extends Mammifere {
    public void aboyer() {
        System.out.println("Aboie");
    }
}

// Chien hérite de:
// - Mammifere: allaiter()
// - Animal: respirer()
// - Object: toString(), equals(), etc.

Chien chien = new Chien();
chien.respirer();    // De Animal
chien.allaiter();    // De Mammifere
chien.aboyer();      // De Chien
```

## Creating Classes

### Class Declaration

```java
[access_modifier] [modifiers] class ClassName [extends SuperClass] [implements Interfaces] {
    // Class body
}
```

**Exemple complet**:
```java
public final class MaClasse extends SuperClasse implements Interface1, Interface2 {
    // Fields
    private int valeur;
    
    // Constructor
    public MaClasse() { }
    
    // Methods
    public void method() { }
}
```

### Class Modifiers

**public**: accessible partout
```java
public class PublicClass { }
```

**default** (package-private): accessible dans le même package uniquement
```java
class DefaultClass { }
```

**final**: ne peut **pas** être étendue (pas de sous-classes)
```java
public final class FinalClass { }

// public class SousClasse extends FinalClass { }  // ERREUR
```

**abstract**: ne peut **pas** être instantiée, peut contenir des méthodes abstraites
```java
public abstract class AbstractClass {
    public abstract void methodeAbstraite();
}

// AbstractClass obj = new AbstractClass();  // ERREUR
```

### this Reference

**this**: référence à l'**instance courante**.

**Usages**:
1. Accéder aux instance variables quand masquées
2. Appeler un autre constructor de la même classe
3. Passer l'instance courante en paramètre
4. Retourner l'instance courante

```java
public class Personne {
    private String nom;
    private int age;
    
    // 1. Accéder aux instance variables
    public Personne(String nom, int age) {
        this.nom = nom;      // this.nom = instance variable
        this.age = age;      // nom = paramètre
    }
    
    // 2. Appeler un autre constructor
    public Personne(String nom) {
        this(nom, 0);        // Appelle Personne(String, int)
    }
    
    // 3. Passer l'instance en paramètre
    public void enregistrer() {
        Database.save(this);
    }
    
    // 4. Retourner l'instance (method chaining)
    public Personne setNom(String nom) {
        this.nom = nom;
        return this;
    }
}
```

**Important**: `this()` doit être la **première instruction** d'un constructor.

```java
public Personne(String nom) {
    System.out.println("Hello");
    // this(nom, 0);         // ERREUR: doit être en premier
}

public Personne(String nom) {
    this(nom, 0);            // OK
    System.out.println("Hello");
}
```

### super Reference

**super**: référence à la **superclasse**.

**Usages**:
1. Accéder aux members de la superclasse
2. Appeler le constructor de la superclasse

```java
public class Animal {
    protected String nom;
    
    public Animal(String nom) {
        this.nom = nom;
    }
    
    public void manger() {
        System.out.println("Animal mange");
    }
}

public class Chien extends Animal {
    private String race;
    
    // 1. Appeler le constructor de la superclasse
    public Chien(String nom, String race) {
        super(nom);          // Appelle Animal(String)
        this.race = race;
    }
    
    // 2. Accéder aux members de la superclasse
    @Override
    public void manger() {
        super.manger();      // Appelle Animal.manger()
        System.out.println("Chien mange");
    }
}
```

**Important**: `super()` doit être la **première instruction** d'un constructor.

## Declaring Constructors

### Constructor Rules

**Règles**:
1. Même nom que la classe
2. Pas de return type (même pas `void`)
3. Peut avoir n'importe quel access modifier
4. Peut lancer des exceptions

```java
public class Exemple {
    // Constructor valide
    public Exemple() { }
    
    // Constructor avec paramètres
    public Exemple(int x) { }
    
    // Constructor private (Singleton pattern)
    private Exemple(String s) { }
    
    // ERREUR: a un return type
    // public void Exemple() { }
    
    // ERREUR: nom différent de la classe
    // public exemple() { }
}
```

### Default Constructor

Si **aucun constructor** n'est défini, Java crée un **default constructor** automatiquement.

```java
public class Animal {
    // Pas de constructor défini
}

// Java crée automatiquement:
// public Animal() {
//     super();
// }

Animal animal = new Animal();  // OK
```

**Important**: Si vous définissez **un** constructor, le default constructor n'est **plus** créé automatiquement.

```java
public class Animal {
    public Animal(String nom) {
        // Constructor avec paramètre
    }
}

// Animal animal = new Animal();  // ERREUR: no default constructor
Animal animal = new Animal("Rex");  // OK
```

### Constructor Chaining

Les constructors peuvent s'appeler entre eux avec `this()` et `super()`.

```java
public class Personne {
    private String nom;
    private int age;
    private String ville;
    
    public Personne(String nom, int age, String ville) {
        this.nom = nom;
        this.age = age;
        this.ville = ville;
    }
    
    public Personne(String nom, int age) {
        this(nom, age, "Paris");     // Appelle le constructor à 3 paramètres
    }
    
    public Personne(String nom) {
        this(nom, 0);                // Appelle le constructor à 2 paramètres
    }
    
    public Personne() {
        this("Inconnu");             // Appelle le constructor à 1 paramètre
    }
}
```

**Avec héritage**:
```java
public class Animal {
    protected String nom;
    
    public Animal(String nom) {
        this.nom = nom;
        System.out.println("Constructor Animal");
    }
}

public class Chien extends Animal {
    private String race;
    
    public Chien(String nom, String race) {
        super(nom);                  // Doit appeler super() en premier
        this.race = race;
        System.out.println("Constructor Chien");
    }
}

Chien chien = new Chien("Rex", "Labrador");
// Output:
// Constructor Animal
// Constructor Chien
```

### Constructor Order

**Ordre d'exécution** lors de la création d'un objet:

1. Constructor de la superclasse (récursif jusqu'à `Object`)
2. Instance variables initialization
3. Instance initializer blocks
4. Constructor body

```java
public class Parent {
    private int x = init("Parent variable");
    
    {
        System.out.println("2. Parent initializer block");
    }
    
    public Parent() {
        System.out.println("3. Parent constructor");
    }
    
    private int init(String msg) {
        System.out.println("1. " + msg);
        return 1;
    }
}

public class Enfant extends Parent {
    private int y = init("Enfant variable");
    
    {
        System.out.println("5. Enfant initializer block");
    }
    
    public Enfant() {
        System.out.println("6. Enfant constructor");
    }
    
    private int init(String msg) {
        System.out.println("4. " + msg);
        return 2;
    }
}

Enfant e = new Enfant();
// Output:
// 1. Parent variable
// 2. Parent initializer block
// 3. Parent constructor
// 4. Enfant variable
// 5. Enfant initializer block
// 6. Enfant constructor
```

## Initializing Objects

### Order of Initialization

**Ordre complet** (avec static):

1. **Static** members de la superclasse
2. **Static** members de la classe
3. **Instance** members de la superclasse
4. **Constructor** de la superclasse
5. **Instance** members de la classe
6. **Constructor** de la classe

```java
public class Parent {
    static {
        System.out.println("1. Parent static block");
    }
    
    {
        System.out.println("3. Parent instance block");
    }
    
    public Parent() {
        System.out.println("4. Parent constructor");
    }
}

public class Enfant extends Parent {
    static {
        System.out.println("2. Enfant static block");
    }
    
    {
        System.out.println("5. Enfant instance block");
    }
    
    public Enfant() {
        System.out.println("6. Enfant constructor");
    }
}

Enfant e = new Enfant();
// Premier accès à la classe
```

### Instance Initializers

**Instance initializer blocks**: blocs de code exécutés avant le constructor.

```java
public class Example {
    private int valeur;
    
    // Instance initializer block
    {
        valeur = 10;
        System.out.println("Instance initializer");
    }
    
    public Example() {
        System.out.println("Constructor, valeur = " + valeur);
    }
}

Example ex = new Example();
// Output:
// Instance initializer
// Constructor, valeur = 10
```

**Usage**: initialisation complexe commune à tous les constructors.

```java
public class Configuration {
    private Map<String, String> settings;
    
    {
        // Initialisation complexe
        settings = new HashMap<>();
        settings.put("url", "localhost");
        settings.put("port", "8080");
        settings.put("timeout", "30");
    }
    
    public Configuration() {
        // settings déjà initialisé
    }
    
    public Configuration(String env) {
        // settings déjà initialisé
        if (env.equals("prod")) {
            settings.put("url", "prod.server.com");
        }
    }
}
```

### Final Instance Variables

**final instance variables**: doivent être initialisées:
- Lors de la déclaration, OU
- Dans un instance initializer, OU
- Dans **tous** les constructors

```java
public class Example {
    // Initialisé lors de la déclaration
    private final int a = 10;
    
    // Initialisé dans instance initializer
    private final int b;
    {
        b = 20;
    }
    
    // Initialisé dans le constructor
    private final int c;
    public Example() {
        c = 30;
    }
    
    // ERREUR: pas initialisé
    // private final int d;
}
```

## Inheriting Members

### Inheriting Methods

Une sous-classe **hérite** de toutes les méthodes **non-private** de sa superclasse.

```java
public class Animal {
    public void manger() {
        System.out.println("Animal mange");
    }
    
    protected void dormir() {
        System.out.println("Animal dort");
    }
    
    private void respirer() {
        System.out.println("Animal respire");
    }
}

public class Chien extends Animal {
    public void test() {
        manger();        // OK (public)
        dormir();        // OK (protected)
        // respirer();   // ERREUR (private)
    }
}
```

### Overriding Methods

**Overriding**: redéfinir une méthode héritée.

**Règles**:
1. **Même signature** (nom + paramètres)
2. Return type doit être le **même** ou un **sous-type** (covariant)
3. Access modifier doit être le **même** ou **plus permissif**
4. Ne peut **pas** lancer de **nouvelles checked exceptions** (ou plus larges)
5. Méthode doit être **non-final** dans la superclasse

```java
public class Animal {
    public void manger() {
        System.out.println("Animal mange");
    }
    
    public Animal reproduire() {
        return new Animal();
    }
}

public class Chien extends Animal {
    // Override correct
    @Override
    public void manger() {
        System.out.println("Chien mange");
    }
    
    // Covariant return type
    @Override
    public Chien reproduire() {      // Chien est un sous-type d'Animal
        return new Chien();
    }
}
```

### @Override Annotation

**@Override**: annotation pour indiquer qu'on override une méthode.

**Avantages**:
- Détecte les erreurs à la compilation
- Documente l'intention
- Prévient les bugs

```java
public class Animal {
    public void manger() { }
}

public class Chien extends Animal {
    @Override
    public void manger() { }         // OK
    
    // @Override
    // public void mangerr() { }     // ERREUR de compilation (typo détectée)
}
```

### Overriding Rules

**Règle 1**: Signature identique

```java
public class Parent {
    public void method(int x) { }
}

public class Enfant extends Parent {
    // @Override
    // public void method(long x) { }   // ERREUR: pas un override (overload)
    
    @Override
    public void method(int x) { }       // OK: override
}
```

**Règle 2**: Access modifier identique ou plus permissif

```java
public class Parent {
    protected void method() { }
}

public class Enfant extends Parent {
    // @Override
    // private void method() { }        // ERREUR: moins permissif
    
    @Override
    protected void method() { }         // OK: identique
    
    @Override
    public void method() { }            // OK: plus permissif
}
```

**Règle 3**: Return type identique ou covariant

```java
public class Parent {
    public Number getNumber() {
        return 1;
    }
}

public class Enfant extends Parent {
    @Override
    public Integer getNumber() {        // OK: Integer extends Number (covariant)
        return 1;
    }
    
    // @Override
    // public String getNumber() { }    // ERREUR: String n'est pas un sous-type de Number
}
```

**Règle 4**: Exceptions

```java
public class Parent {
    public void method() throws IOException { }
}

public class Enfant extends Parent {
    @Override
    public void method() throws FileNotFoundException { }  // OK: plus spécifique
    
    @Override
    public void method() { }                                // OK: pas d'exception
    
    // @Override
    // public void method() throws Exception { }            // ERREUR: plus large
}
```

### Hiding Static Methods

Les méthodes **static** ne sont **pas overridden**, elles sont **cachées** (hidden).

```java
public class Parent {
    public static void staticMethod() {
        System.out.println("Parent static");
    }
}

public class Enfant extends Parent {
    public static void staticMethod() {  // Cache (ne override pas)
        System.out.println("Enfant static");
    }
}

// Utilisation
Parent.staticMethod();                   // "Parent static"
Enfant.staticMethod();                   // "Enfant static"

Parent p = new Enfant();
p.staticMethod();                        // "Parent static" (pas de polymorphisme)
```

### Hiding Variables

Les variables peuvent être **cachées** (mais c'est **déconseillé**).

```java
public class Parent {
    public int x = 10;
}

public class Enfant extends Parent {
    public int x = 20;               // Cache la variable parent
}

Enfant e = new Enfant();
System.out.println(e.x);             // 20 (variable enfant)
System.out.println(((Parent)e).x);   // 10 (variable parent)
```

**Recommandation**: Ne pas cacher les variables, utiliser des noms différents.

### final Methods

Une méthode **final** ne peut **pas** être overridden.

```java
public class Parent {
    public final void methodeFinale() {
        System.out.println("Final");
    }
}

public class Enfant extends Parent {
    // @Override
    // public void methodeFinale() { }  // ERREUR: cannot override final method
}
```

## Creating Abstract Classes

### Abstract Class Basics

**Abstract class**: classe qui ne peut **pas** être instantiée, peut contenir des méthodes abstraites.

```java
public abstract class Animal {
    protected String nom;
    
    // Méthode abstraite (pas d'implémentation)
    public abstract void faireDuBruit();
    
    // Méthode concrète (avec implémentation)
    public void dormir() {
        System.out.println(nom + " dort");
    }
}

// Animal animal = new Animal();  // ERREUR: cannot instantiate abstract class
```

### Abstract Methods

**Méthode abstraite**: déclarée avec `abstract`, **pas d'implémentation**.

**Règles**:
1. Doit être dans une **classe abstraite** ou **interface**
2. Ne peut **pas** être `private`, `static`, ou `final`
3. Les sous-classes **concrètes** doivent l'implémenter

```java
public abstract class Forme {
    // Méthode abstraite
    public abstract double calculerAire();
    
    // Méthode abstraite
    public abstract double calculerPerimetre();
    
    // Méthode concrète
    public void afficher() {
        System.out.println("Aire: " + calculerAire());
        System.out.println("Périmètre: " + calculerPerimetre());
    }
}

public class Cercle extends Forme {
    private double rayon;
    
    public Cercle(double rayon) {
        this.rayon = rayon;
    }
    
    // Doit implémenter toutes les méthodes abstraites
    @Override
    public double calculerAire() {
        return Math.PI * rayon * rayon;
    }
    
    @Override
    public double calculerPerimetre() {
        return 2 * Math.PI * rayon;
    }
}
```

### Abstract Class Rules

**Règle 1**: Une classe abstraite peut avoir **0 ou plusieurs** méthodes abstraites.

```java
public abstract class A {
    // Pas de méthode abstraite (mais classe abstraite quand même)
}

public abstract class B {
    public abstract void method1();
    public abstract void method2();
    public void method3() { }        // Méthode concrète OK
}
```

**Règle 2**: Si une classe a **au moins une** méthode abstraite, elle **doit** être abstraite.

```java
// public class C {                  // ERREUR: doit être abstract
//     public abstract void method();
// }

public abstract class C {            // OK
    public abstract void method();
}
```

**Règle 3**: Une sous-classe concrète **doit** implémenter toutes les méthodes abstraites.

```java
public abstract class Parent {
    public abstract void method1();
    public abstract void method2();
}

// public class Enfant extends Parent {  // ERREUR: doit implémenter method1 et method2
// }

public class Enfant extends Parent {
    @Override
    public void method1() { }
    
    @Override
    public void method2() { }
}
```

**Règle 4**: Une sous-classe abstraite peut ne **pas** implémenter les méthodes abstraites.

```java
public abstract class Parent {
    public abstract void method1();
    public abstract void method2();
}

public abstract class EnfantAbstrait extends Parent {
    @Override
    public void method1() { }
    // method2() reste abstraite
}

public class EnfantConcret extends EnfantAbstrait {
    @Override
    public void method2() { }        // Doit implémenter method2
}
```

### Abstract vs Concrete

| Abstract Class | Concrete Class |
|---------------|----------------|
| `abstract` keyword | Pas de `abstract` |
| Ne peut **pas** être instantiée | Peut être instantiée |
| Peut avoir méthodes abstraites | Toutes les méthodes sont concrètes |
| Doit être étendue | Peut être étendue (sauf si `final`) |

```java
public abstract class Animal {
    public abstract void faireDuBruit();
}

public class Chien extends Animal {
    @Override
    public void faireDuBruit() {
        System.out.println("Woof!");
    }
}

// Animal a = new Animal();      // ERREUR
Animal a = new Chien();          // OK (polymorphisme)
a.faireDuBruit();                // "Woof!"
```

## Creating Immutable Objects

### Immutability

**Objet immuable**: ne peut **pas** être modifié après création.

**Exemples**: `String`, `Integer`, `LocalDate`, etc.

**Avantages**:
- Thread-safe (pas de synchronisation nécessaire)
- Peut être partagé librement
- Hashcode stable (bon pour HashMap, HashSet)
- Plus simple à raisonner

### Creating Immutable Classes

**Règles** pour créer une classe immuable:

1. Déclarer la classe **final** (empêche l'héritage)
2. Tous les fields **private** et **final**
3. Pas de **setters**
4. Constructor initialise tous les fields
5. Defensive copies pour les objets mutables

**Exemple simple**:
```java
public final class Personne {
    private final String nom;
    private final int age;
    
    public Personne(String nom, int age) {
        this.nom = nom;
        this.age = age;
    }
    
    // Getters seulement (pas de setters)
    public String getNom() {
        return nom;
    }
    
    public int getAge() {
        return age;
    }
}

Personne p = new Personne("Alice", 25);
System.out.println(p.getNom());      // "Alice"
// p.setNom("Bob");                  // Impossible: pas de setter
```

### Defensive Copies

Pour les **objets mutables**, faire des **copies défensives**.

**Mauvais** (mutable):
```java
public final class Mauvais {
    private final List<String> items;
    
    public Mauvais(List<String> items) {
        this.items = items;              // Référence directe!
    }
    
    public List<String> getItems() {
        return items;                    // Référence directe!
    }
}

List<String> list = new ArrayList<>();
list.add("A");
Mauvais m = new Mauvais(list);
list.add("B");                           // Modifie l'objet!
System.out.println(m.getItems());        // [A, B]
```

**Bon** (immuable):
```java
public final class Bon {
    private final List<String> items;
    
    public Bon(List<String> items) {
        this.items = new ArrayList<>(items);  // Copie défensive
    }
    
    public List<String> getItems() {
        return new ArrayList<>(items);        // Copie défensive
    }
}

List<String> list = new ArrayList<>();
list.add("A");
Bon b = new Bon(list);
list.add("B");                           // N'affecte PAS l'objet
System.out.println(b.getItems());        // [A]

b.getItems().add("C");                   // N'affecte PAS l'objet
System.out.println(b.getItems());        // [A]
```

### Immutable with Mutable Fields

**Alternative**: retourner une **vue non-modifiable**.

```java
import java.util.*;

public final class Personne {
    private final List<String> emails;
    
    public Personne(List<String> emails) {
        this.emails = new ArrayList<>(emails);
    }
    
    public List<String> getEmails() {
        return Collections.unmodifiableList(emails);  // Vue non-modifiable
    }
}

Personne p = new Personne(Arrays.asList("a@mail.com", "b@mail.com"));
List<String> emails = p.getEmails();
// emails.add("c@mail.com");            // UnsupportedOperationException
```

---

## Exemples Pratiques Complets

### Exemple 1: Hiérarchie d'Animaux
```java
// Classe abstraite de base
public abstract class Animal {
    protected String nom;
    protected int age;
    
    public Animal(String nom, int age) {
        this.nom = nom;
        this.age = age;
    }
    
    // Méthode abstraite
    public abstract void faireDuBruit();
    
    // Méthode concrète
    public void dormir() {
        System.out.println(nom + " dort");
    }
    
    public void afficherInfo() {
        System.out.println("Nom: " + nom + ", Age: " + age);
    }
    
    // Getters
    public String getNom() { return nom; }
    public int getAge() { return age; }
}

// Sous-classe abstraite intermédiaire
public abstract class Mammifere extends Animal {
    protected boolean estDomestique;
    
    public Mammifere(String nom, int age, boolean estDomestique) {
        super(nom, age);
        this.estDomestique = estDomestique;
    }
    
    public void allaiter() {
        System.out.println(nom + " allaite");
    }
}

// Classe concrète
public class Chien extends Mammifere {
    private String race;
    
    public Chien(String nom, int age, String race) {
        super(nom, age, true);
        this.race = race;
    }
    
    @Override
    public void faireDuBruit() {
        System.out.println(nom + " aboie: Woof!");
    }
    
    public void chercher() {
        System.out.println(nom + " cherche la balle");
    }
    
    @Override
    public void afficherInfo() {
        super.afficherInfo();
        System.out.println("Race: " + race);
        System.out.println("Domestique: " + (estDomestique ? "Oui" : "Non"));
    }
}

// Classe concrète
public class Chat extends Mammifere {
    private int nombreVies;
    
    public Chat(String nom, int age) {
        super(nom, age, true);
        this.nombreVies = 9;
    }
    
    @Override
    public void faireDuBruit() {
        System.out.println(nom + " miaule: Meow!");
    }
    
    public void grimper() {
        System.out.println(nom + " grimpe à l'arbre");
    }
}

// Utilisation
public class TestAnimaux {
    public static void main(String[] args) {
        // Animal animal = new Animal("Test", 5);  // ERREUR: abstract
        
        Chien chien = new Chien("Rex", 3, "Labrador");
        chien.afficherInfo();
        chien.faireDuBruit();
        chien.dormir();
        chien.allaiter();
        chien.chercher();
        
        System.out.println();
        
        Chat chat = new Chat("Minou", 2);
        chat.afficherInfo();
        chat.faireDuBruit();
        chat.grimper();
        
        System.out.println();
        
        // Polymorphisme
        Animal[] animaux = {chien, chat};
        for (Animal a : animaux) {
            a.faireDuBruit();
        }
    }
}
```

### Exemple 2: Hiérarchie de Formes Géométriques
```java
public abstract class Forme {
    protected String couleur;
    
    public Forme(String couleur) {
        this.couleur = couleur;
    }
    
    // Méthodes abstraites
    public abstract double calculerAire();
    public abstract double calculerPerimetre();
    
    // Méthode concrète
    public void afficherInfo() {
        System.out.println("Couleur: " + couleur);
        System.out.println("Aire: " + calculerAire());
        System.out.println("Périmètre: " + calculerPerimetre());
    }
    
    public String getCouleur() {
        return couleur;
    }
}

public class Cercle extends Forme {
    private double rayon;
    
    public Cercle(String couleur, double rayon) {
        super(couleur);
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
    public void afficherInfo() {
        System.out.println("=== Cercle ===");
        System.out.println("Rayon: " + rayon);
        super.afficherInfo();
    }
}

public class Rectangle extends Forme {
    private double longueur;
    private double largeur;
    
    public Rectangle(String couleur, double longueur, double largeur) {
        super(couleur);
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
    public void afficherInfo() {
        System.out.println("=== Rectangle ===");
        System.out.println("Longueur: " + longueur);
        System.out.println("Largeur: " + largeur);
        super.afficherInfo();
    }
}

public class Triangle extends Forme {
    private double cote1, cote2, cote3;
    
    public Triangle(String couleur, double cote1, double cote2, double cote3) {
        super(couleur);
        this.cote1 = cote1;
        this.cote2 = cote2;
        this.cote3 = cote3;
    }
    
    @Override
    public double calculerAire() {
        // Formule de Héron
        double s = calculerPerimetre() / 2;
        return Math.sqrt(s * (s - cote1) * (s - cote2) * (s - cote3));
    }
    
    @Override
    public double calculerPerimetre() {
        return cote1 + cote2 + cote3;
    }
    
    @Override
    public void afficherInfo() {
        System.out.println("=== Triangle ===");
        System.out.println("Côtés: " + cote1 + ", " + cote2 + ", " + cote3);
        super.afficherInfo();
    }
}

// Utilisation
public class TestFormes {
    public static void main(String[] args) {
        Forme[] formes = {
            new Cercle("Rouge", 5),
            new Rectangle("Bleu", 10, 5),
            new Triangle("Vert", 3, 4, 5)
        };
        
        double aireTotal = 0;
        for (Forme forme : formes) {
            forme.afficherInfo();
            aireTotal += forme.calculerAire();
            System.out.println();
        }
        
        System.out.println("Aire totale: " + aireTotal);
    }
}
```

### Exemple 3: Classe Immuable - Point
```java
public final class Point {
    private final int x;
    private final int y;
    
    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }
    
    public int getX() {
        return x;
    }
    
    public int getY() {
        return y;
    }
    
    // Méthodes qui retournent de NOUVEAUX objets
    public Point deplacer(int dx, int dy) {
        return new Point(x + dx, y + dy);
    }
    
    public Point doubler() {
        return new Point(x * 2, y * 2);
    }
    
    public double distanceOrigine() {
        return Math.sqrt(x * x + y * y);
    }
    
    public double distanceVers(Point autre) {
        int dx = this.x - autre.x;
        int dy = this.y - autre.y;
        return Math.sqrt(dx * dx + dy * dy);
    }
    
    @Override
    public String toString() {
        return "Point(" + x + ", " + y + ")";
    }
    
    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (!(obj instanceof Point)) return false;
        Point other = (Point) obj;
        return this.x == other.x && this.y == other.y;
    }
    
    @Override
    public int hashCode() {
        return 31 * x + y;
    }
}

// Utilisation
public class TestPoint {
    public static void main(String[] args) {
        Point p1 = new Point(3, 4);
        System.out.println(p1);                     // Point(3, 4)
        
        Point p2 = p1.deplacer(2, 1);
        System.out.println(p2);                     // Point(5, 5)
        System.out.println(p1);                     // Point(3, 4) - inchangé!
        
        Point p3 = p1.doubler();
        System.out.println(p3);                     // Point(6, 8)
        
        System.out.println("Distance origine: " + p1.distanceOrigine());
        System.out.println("Distance p1-p2: " + p1.distanceVers(p2));
    }
}
```

### Exemple 4: Classe Immuable avec Collection
```java
import java.util.*;

public final class Entreprise {
    private final String nom;
    private final List<String> employes;
    
    public Entreprise(String nom, List<String> employes) {
        this.nom = nom;
        // Copie défensive
        this.employes = new ArrayList<>(employes);
    }
    
    public String getNom() {
        return nom;
    }
    
    public List<String> getEmployes() {
        // Retourne une copie non-modifiable
        return Collections.unmodifiableList(employes);
    }
    
    public int getNombreEmployes() {
        return employes.size();
    }
    
    // Retourne une NOUVELLE entreprise avec un employé ajouté
    public Entreprise ajouterEmploye(String employe) {
        List<String> nouveauxEmployes = new ArrayList<>(employes);
        nouveauxEmployes.add(employe);
        return new Entreprise(nom, nouveauxEmployes);
    }
    
    // Retourne une NOUVELLE entreprise avec un employé retiré
    public Entreprise retirerEmploye(String employe) {
        List<String> nouveauxEmployes = new ArrayList<>(employes);
        nouveauxEmployes.remove(employe);
        return new Entreprise(nom, nouveauxEmployes);
    }
    
    @Override
    public String toString() {
        return nom + " (" + employes.size() + " employés): " + employes;
    }
}

// Utilisation
public class TestEntreprise {
    public static void main(String[] args) {
        List<String> liste = new ArrayList<>();
        liste.add("Alice");
        liste.add("Bob");
        
        Entreprise e1 = new Entreprise("TechCorp", liste);
        System.out.println(e1);
        
        // Modifier la liste originale n'affecte pas l'objet
        liste.add("Charlie");
        System.out.println(e1);  // Toujours Alice et Bob
        
        // Ajouter un employé crée une NOUVELLE entreprise
        Entreprise e2 = e1.ajouterEmploye("Diana");
        System.out.println(e1);  // Inchangé
        System.out.println(e2);  // Avec Diana
        
        // Impossible de modifier via le getter
        // e1.getEmployes().add("Eve");  // UnsupportedOperationException
    }
}
```

### Exemple 5: Constructor Chaining Complexe
```java
public class Produit {
    private String nom;
    private double prix;
    private String categorie;
    private int stock;
    private String description;
    
    // Constructor principal (le plus complet)
    public Produit(String nom, double prix, String categorie, int stock, String description) {
        if (nom == null || nom.isEmpty()) {
            throw new IllegalArgumentException("Le nom ne peut pas être vide");
        }
        if (prix < 0) {
            throw new IllegalArgumentException("Le prix doit être positif");
        }
        
        this.nom = nom;
        this.prix = prix;
        this.categorie = categorie;
        this.stock = stock;
        this.description = description;
        
        System.out.println("Constructor principal appelé");
    }
    
    // Constructor sans description
    public Produit(String nom, double prix, String categorie, int stock) {
        this(nom, prix, categorie, stock, "Aucune description");
        System.out.println("Constructor 4 paramètres appelé");
    }
    
    // Constructor sans stock (défaut: 0)
    public Produit(String nom, double prix, String categorie) {
        this(nom, prix, categorie, 0);
        System.out.println("Constructor 3 paramètres appelé");
    }
    
    // Constructor sans catégorie (défaut: "Général")
    public Produit(String nom, double prix) {
        this(nom, prix, "Général");
        System.out.println("Constructor 2 paramètres appelé");
    }
    
    // Constructor minimal
    public Produit(String nom) {
        this(nom, 0.0);
        System.out.println("Constructor 1 paramètre appelé");
    }
    
    @Override
    public String toString() {
        return String.format("Produit[nom=%s, prix=%.2f, categorie=%s, stock=%d, description=%s]",
            nom, prix, categorie, stock, description);
    }
    
    public static void main(String[] args) {
        System.out.println("=== Création avec 1 paramètre ===");
        Produit p1 = new Produit("Laptop");
        System.out.println(p1);
        System.out.println();
        
        System.out.println("=== Création avec 3 paramètres ===");
        Produit p2 = new Produit("Souris", 25.99, "Accessoires");
        System.out.println(p2);
    }
}
```

### Exemple 6: Inheritance avec Initialization Order
```java
public class Parent {
    private static int staticParent = initStatic("Parent");
    private int instanceParent = initInstance("Parent");
    
    static {
        System.out.println("2. Static block Parent");
    }
    
    {
        System.out.println("4. Instance block Parent");
    }
    
    public Parent() {
        System.out.println("5. Constructor Parent");
    }
    
    private static int initStatic(String nom) {
        System.out.println("1. Init static variable " + nom);
        return 1;
    }
    
    private int initInstance(String nom) {
        System.out.println("3. Init instance variable " + nom);
        return 1;
    }
}

public class Enfant extends Parent {
    private static int staticEnfant = initStatic("Enfant");
    private int instanceEnfant = initInstance("Enfant");
    
    static {
        System.out.println("7. Static block Enfant");
    }
    
    {
        System.out.println("9. Instance block Enfant");
    }
    
    public Enfant() {
        super();  // Implicite si pas écrit
        System.out.println("10. Constructor Enfant");
    }
    
    private static int initStatic(String nom) {
        System.out.println("6. Init static variable " + nom);
        return 2;
    }
    
    private int initInstance(String nom) {
        System.out.println("8. Init instance variable " + nom);
        return 2;
    }
    
    public static void main(String[] args) {
        System.out.println("=== Première instance ===");
        Enfant e1 = new Enfant();
        System.out.println();
        
        System.out.println("=== Deuxième instance ===");
        Enfant e2 = new Enfant();
    }
}
```

Output:
```
1. Init static variable Parent
2. Static block Parent
6. Init static variable Enfant
7. Static block Enfant
=== Première instance ===
3. Init instance variable Parent
4. Instance block Parent
5. Constructor Parent
8. Init instance variable Enfant
9. Instance block Enfant
10. Constructor Enfant

=== Deuxième instance ===
3. Init instance variable Parent
4. Instance block Parent
5. Constructor Parent
8. Init instance variable Enfant
9. Instance block Enfant
10. Constructor Enfant
```

### Exemple 7: Override avec Covariant Return Type
```java
public class Animal {
    protected String nom;
    
    public Animal(String nom) {
        this.nom = nom;
    }
    
    public Animal reproduire() {
        System.out.println("Animal se reproduit");
        return new Animal("Bébé " + nom);
    }
    
    public void afficherInfo() {
        System.out.println("Animal: " + nom);
    }
}

public class Chien extends Animal {
    private String race;
    
    public Chien(String nom, String race) {
        super(nom);
        this.race = race;
    }
    
    // Covariant return type: Chien au lieu d'Animal
    @Override
    public Chien reproduire() {
        System.out.println("Chien se reproduit");
        return new Chien("Chiot " + nom, race);
    }
    
    @Override
    public void afficherInfo() {
        System.out.println("Chien: " + nom + " (" + race + ")");
    }
}

public class TestCovariance {
    public static void main(String[] args) {
        Animal animal = new Animal("Générique");
        Animal bebe1 = animal.reproduire();
        bebe1.afficherInfo();
        System.out.println();
        
        Chien chien = new Chien("Rex", "Labrador");
        Chien chiot = chien.reproduire();  // Pas besoin de cast!
        chiot.afficherInfo();
        System.out.println();
        
        // Polymorphisme
        Animal a = new Chien("Max", "Berger");
        Animal bebe2 = a.reproduire();  // Retourne un Chien, stocké comme Animal
        bebe2.afficherInfo();
    }
}
```

---

## Points Clés du Chapter 6

✅ **Inheritance**: `extends` pour hériter d'une classe, héritage simple uniquement

✅ **Object class**: toutes les classes héritent implicitement d'`Object`

✅ **this**: référence à l'instance courante
   - `this.variable` pour accéder aux instance variables
   - `this()` pour appeler un autre constructor (doit être en premier)

✅ **super**: référence à la superclasse
   - `super.method()` pour appeler une méthode de la superclasse
   - `super()` pour appeler le constructor parent (doit être en premier)

✅ **Constructor**:
   - Même nom que la classe, pas de return type
   - Default constructor créé automatiquement si aucun défini
   - Constructor chaining avec `this()` et `super()`

✅ **Initialization order**:
   1. Static members (superclasse puis classe)
   2. Instance members de la superclasse
   3. Constructor de la superclasse
   4. Instance members de la classe
   5. Constructor de la classe

✅ **Overriding**:
   - `@Override` annotation recommandée
   - Même signature
   - Return type identique ou covariant
   - Access modifier identique ou plus permissif
   - Exceptions identiques ou plus spécifiques

✅ **Abstract class**:
   - `abstract` keyword, ne peut pas être instantiée
   - Peut avoir méthodes abstraites (pas d'implémentation)
   - Sous-classes concrètes doivent implémenter toutes les méthodes abstraites

✅ **final**:
   - Classe `final`: ne peut pas être étendue
   - Méthode `final`: ne peut pas être overridden
   - Variable `final`: ne peut pas être réassignée

✅ **Immutability**:
   - Classe `final`
   - Fields `private final`
   - Pas de setters
   - Defensive copies pour objets mutables

---

# Questions/Réponses - Chapter 6: Class Design

## Questions sur l'Héritage

**Q1: Ce code compile-t-il?**
```java
public class Animal { }
public class Mammifere { }
public class Chien extends Animal, Mammifere { }
```

**R:** Non, **erreur de compilation**: "'{' expected".

Java supporte uniquement l'**héritage simple**. Une classe ne peut étendre qu'**une seule** classe.

**Correct**:
```java
public class Animal { }
public class Mammifere extends Animal { }
public class Chien extends Mammifere { }  // OK: hiérarchie
```

---

**Q2: Toutes les classes héritent de quelle classe?**

**R:** **`java.lang.Object`**

Toutes les classes héritent implicitement d'`Object`, même si non spécifié.

```java
public class MaClasse { }
// Équivalent à:
public class MaClasse extends Object { }
```

---

**Q3: Quelles méthodes sont héritées?**
```java
public class Parent {
    public void methodePublique() { }
    protected void methodeProtegee() { }
    void methodePackage() { }
    private void methodePrivee() { }
}

public class Enfant extends Parent { }
```

**R:** `Enfant` hérite de:
- ✅ `methodePublique()` (public)
- ✅ `methodeProtegee()` (protected)
- ✅ `methodePackage()` (si même package)
- ❌ `methodePrivee()` (private - **pas héritée**)

---

**Q4: Que s'affiche-t-il?**
```java
public class Parent {
    public void afficher() {
        System.out.println("Parent");
    }
}

public class Enfant extends Parent {
    public void afficher() {
        System.out.println("Enfant");
    }
}

Parent p = new Enfant();
p.afficher();
```

**R:** `"Enfant"`

**Polymorphisme**: La méthode de la classe **réelle** de l'objet (`Enfant`) est appelée, pas celle du type de référence (`Parent`).

---

**Q5: Comment accéder à la méthode de la superclasse depuis la sous-classe?**
```java
public class Parent {
    public void afficher() {
        System.out.println("Parent");
    }
}

public class Enfant extends Parent {
    public void afficher() {
        // Appeler la méthode Parent ici
        System.out.println("Enfant");
    }
}
```

**R:** Utiliser **`super`**:
```java
public void afficher() {
    super.afficher();        // Appelle Parent.afficher()
    System.out.println("Enfant");
}
// Output:
// Parent
// Enfant
```

---

## Questions sur this et super

**Q6: Ce code compile-t-il?**
```java
public class Example {
    private int x;
    
    public Example(int x) {
        System.out.println("Hello");
        this.x = x;
        this(10);
    }
    
    public Example(int x, int y) {
        this(x);
    }
}
```

**R:** Non, **erreur de compilation**: "call to this must be first statement in constructor".

`this()` (appel à un autre constructor) doit être la **première instruction**.

---

**Q7: Ce code compile-t-il?**
```java
public class Parent {
    public Parent(int x) { }
}

public class Enfant extends Parent {
    public Enfant() {
        super(10);
        super(20);
    }
}
```

**R:** Non, **erreur de compilation**: "call to super must be first statement in constructor".

On ne peut appeler `super()` qu'**une seule fois** et ça doit être la **première instruction**.

---

**Q8: Que se passe-t-il ici?**
```java
public class Parent {
    public Parent(int x) {
        System.out.println("Parent: " + x);
    }
}

public class Enfant extends Parent {
    public Enfant() {
        System.out.println("Enfant");
    }
}
```

**R:** **Erreur de compilation**: "constructor Parent in class Parent cannot be applied to given types".

Si la superclasse n'a **pas de default constructor**, la sous-classe doit **explicitement** appeler un constructor parent avec `super()`.

**Correct**:
```java
public Enfant() {
    super(10);           // Appel explicite
    System.out.println("Enfant");
}
```

---

**Q9: Peut-on utiliser this() et super() dans le même constructor?**
```java
public class Enfant extends Parent {
    public Enfant(int x) {
        super(x);
        this(x, 0);
    }
}
```

**R:** Non, **impossible**.

On ne peut utiliser qu'**un seul** des deux (`this()` ou `super()`), et il doit être en première position.

---

**Q10: Quelle est la différence?**
```java
public class Example {
    private int x = 10;
    
    public void method1() {
        int x = 20;
        System.out.println(x);
    }
    
    public void method2() {
        int x = 20;
        System.out.println(this.x);
    }
}
```

**R:**
```java
method1()  // Affiche 20 (variable locale)
method2()  // Affiche 10 (instance variable avec this)
```

`this.x` fait **explicitement** référence à l'instance variable, même si une variable locale porte le même nom.

---

## Questions sur les Constructors

**Q11: Si aucun constructor n'est défini, que crée Java automatiquement?**

**R:** Un **default constructor** (sans paramètres) qui appelle `super()`.

```java
public class Example {
    // Java crée automatiquement:
    // public Example() {
    //     super();
    // }
}

Example ex = new Example();  // OK
```

---

**Q12: Ce code compile-t-il?**
```java
public class Example {
    public Example(int x) {
        System.out.println(x);
    }
}

Example ex = new Example();
```

**R:** Non, **erreur de compilation**: "constructor Example in class Example cannot be applied to given types".

Dès qu'on définit **un** constructor, le default constructor n'est **plus** créé automatiquement.

**Correct**:
```java
Example ex = new Example(10);  // OK
```

---

**Q13: Quel est l'ordre d'exécution?**
```java
public class Parent {
    public Parent() {
        System.out.println("1");
    }
}

public class Enfant extends Parent {
    public Enfant() {
        System.out.println("2");
    }
}

Enfant e = new Enfant();
```

**R:**
```
1
2
```

Le constructor de la **superclasse** est toujours exécuté **avant** celui de la sous-classe (via `super()` implicite ou explicite).

---

**Q14: Dans quel ordre s'exécutent ces blocs?**
```java
public class Example {
    private int x = init();
    
    {
        System.out.println("2");
    }
    
    public Example() {
        System.out.println("3");
    }
    
    private int init() {
        System.out.println("1");
        return 10;
    }
}

Example ex = new Example();
```

**R:**
```
1
2
3
```

**Ordre**:
1. Instance variables initialization
2. Instance initializer blocks
3. Constructor

---

**Q15: Quel est l'ordre d'exécution complet?**
```java
public class Parent {
    static { System.out.println("1"); }
    { System.out.println("3"); }
    public Parent() { System.out.println("4"); }
}

public class Enfant extends Parent {
    static { System.out.println("2"); }
    { System.out.println("5"); }
    public Enfant() { System.out.println("6"); }
}

Enfant e = new Enfant();
```

**R:**
```
1
2
3
4
5
6
```

**Ordre**:
1. Static block Parent (une fois)
2. Static block Enfant (une fois)
3. Instance block Parent
4. Constructor Parent
5. Instance block Enfant
6. Constructor Enfant

---

## Questions sur l'Override

**Q16: Ce code compile-t-il?**
```java
public class Parent {
    public void method(int x) { }
}

public class Enfant extends Parent {
    @Override
    public void method(long x) { }
}
```

**R:** Non, **erreur de compilation**: "method does not override or implement a method from a supertype".

Les **signatures** sont différentes: `method(int)` vs `method(long)`. C'est un **overload**, pas un override.

**Pour override**, la signature doit être **identique**:
```java
@Override
public void method(int x) { }  // OK
```

---

**Q17: Ce code compile-t-il?**
```java
public class Parent {
    public Number getNumber() {
        return 1;
    }
}

public class Enfant extends Parent {
    @Override
    public Integer getNumber() {
        return 1;
    }
}
```

**R:** Oui, compile.

**Covariant return type**: Le return type peut être un **sous-type** du return type original. `Integer` extends `Number`, donc c'est valide.

---

**Q18: Ce code compile-t-il?**
```java
public class Parent {
    protected void method() { }
}

public class Enfant extends Parent {
    @Override
    private void method() { }
}
```

**R:** Non, **erreur de compilation**: "attempting to assign weaker access privileges".

L'access modifier lors de l'override doit être **identique** ou **plus permissif**, jamais **moins permissif**.

```java
protected → protected  // OK
protected → public     // OK
protected → private    // ERREUR
```

---

**Q19: Ce code compile-t-il?**
```java
import java.io.*;

public class Parent {
    public void method() throws IOException { }
}

public class Enfant extends Parent {
    @Override
    public void method() throws Exception { }
}
```

**R:** Non, **erreur de compilation**: "method() in Enfant cannot override method() in Parent".

On ne peut **pas** lancer une exception **plus large** (ou nouvelle checked exception) lors de l'override. `Exception` est plus large qu'`IOException`.

**Valide**:
```java
@Override
public void method() throws FileNotFoundException { }  // OK: plus spécifique
@Override
public void method() { }                               // OK: pas d'exception
```

---

**Q20: Pourquoi utiliser @Override?**
```java
public class Parent {
    public void afficher() { }
}

public class Enfant extends Parent {
    @Override
    public void afficheer() { }  // Typo
}
```

**R:** Avec `@Override`, on obtient une **erreur de compilation**: "method does not override or implement a method from a supertype".

Sans `@Override`, le code compilerait mais `afficheer()` serait une **nouvelle méthode** (pas un override), causant un bug silencieux.

**Recommandation**: Toujours utiliser `@Override` pour détecter les erreurs.

---

**Q21: Quelle méthode est appelée?**
```java
public class Parent {
    public static void method() {
        System.out.println("Parent");
    }
}

public class Enfant extends Parent {
    public static void method() {
        System.out.println("Enfant");
    }
}

Parent p = new Enfant();
p.method();
```

**R:** `"Parent"`

Les méthodes **static** ne sont **pas overridden**, elles sont **cachées** (hidden). La méthode appelée dépend du **type de référence**, pas du type réel de l'objet.

```java
Parent.method();   // "Parent"
Enfant.method();   // "Enfant"
```

---

## Questions sur final

**Q22: Ce code compile-t-il?**
```java
public final class Parent { }
public class Enfant extends Parent { }
```

**R:** Non, **erreur de compilation**: "cannot inherit from final Parent".

Une classe **final** ne peut **pas** être étendue (pas de sous-classes).

---

**Q23: Ce code compile-t-il?**
```java
public class Parent {
    public final void method() { }
}

public class Enfant extends Parent {
    @Override
    public void method() { }
}
```

**R:** Non, **erreur de compilation**: "method() in Enfant cannot override method() in Parent".

Une méthode **final** ne peut **pas** être overridden.

---

**Q24: Ce code compile-t-il?**
```java
public class Example {
    private final int x;
    
    public Example() {
        System.out.println("Hello");
    }
}
```

**R:** Non, **erreur de compilation**: "variable x might not have been initialized".

Une variable **final** doit être initialisée:
- Lors de la déclaration, OU
- Dans un instance initializer, OU
- Dans **tous** les constructors

**Correct**:
```java
public Example() {
    x = 10;  // OK
    System.out.println("Hello");
}
```

---

**Q25: Ce code compile-t-il?**
```java
public class Example {
    private final int x = 10;
    
    public void modifier() {
        x = 20;
    }
}
```

**R:** Non, **erreur de compilation**: "cannot assign a value to final variable x".

Une variable **final** ne peut être assignée qu'**une seule fois**.

---

## Questions sur abstract

**Q26: Ce code compile-t-il?**
```java
public abstract class Animal {
    public abstract void faireDuBruit();
}

Animal animal = new Animal();
```

**R:** Non, **erreur de compilation**: "Animal is abstract; cannot be instantiated".

Une classe **abstract** ne peut **pas** être instantiée.

**Correct**:
```java
Animal animal = new Chien();  // Où Chien extends Animal
```

---

**Q27: Ce code compile-t-il?**
```java
public class Animal {
    public abstract void faireDuBruit();
}
```

**R:** Non, **erreur de compilation**: "abstract method faireDuBruit in class Animal".

Si une classe contient **au moins une** méthode abstraite, la classe **doit** être déclarée `abstract`.

**Correct**:
```java
public abstract class Animal {
    public abstract void faireDuBruit();
}
```

---

**Q28: Ce code compile-t-il?**
```java
public abstract class Animal {
    public abstract void faireDuBruit();
}

public class Chien extends Animal {
    // Pas d'implémentation
}
```

**R:** Non, **erreur de compilation**: "Chien is not abstract and does not override abstract method faireDuBruit() in Animal".

Une sous-classe **concrète** doit implémenter **toutes** les méthodes abstraites de la superclasse.

**Correct**:
```java
public class Chien extends Animal {
    @Override
    public void faireDuBruit() {
        System.out.println("Woof!");
    }
}
```

---

**Q29: Ce code compile-t-il?**
```java
public abstract class Animal {
    public abstract void faireDuBruit();
}

public abstract class Mammifere extends Animal {
    // Pas d'implémentation de faireDuBruit()
}
```

**R:** Oui, compile.

Une sous-classe **abstraite** n'est **pas obligée** d'implémenter les méthodes abstraites de la superclasse.

---

**Q30: Une classe abstract peut-elle avoir des méthodes concrètes?**
```java
public abstract class Animal {
    public abstract void faireDuBruit();
    
    public void dormir() {
        System.out.println("Dort");
    }
}
```

**R:** Oui, **valide**.

Une classe **abstract** peut avoir:
- 0 ou plusieurs méthodes abstraites
- 0 ou plusieurs méthodes concrètes
- Des constructors
- Des fields

---

**Q31: Ce code compile-t-il?**
```java
public abstract class Animal {
    private abstract void method();
}
```

**R:** Non, **erreur de compilation**: "illegal combination of modifiers: abstract and private".

Une méthode **abstract** ne peut **pas** être:
- `private` (doit être accessible aux sous-classes)
- `static` (doit être overridden)
- `final` (doit être overridden)

---

## Questions sur l'Immutabilité

**Q32: Cette classe est-elle immuable?**
```java
public class Point {
    private final int x;
    private final int y;
    
    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }
    
    public int getX() { return x; }
    public int getY() { return y; }
}
```

**R:** **Presque**, mais pas complètement.

Pour être **totalement immuable**, la classe devrait aussi être **final** pour empêcher l'héritage.

**Correct**:
```java
public final class Point {  // final ajouté
    // ...
}
```

---

**Q33: Cette classe est-elle immuable?**
```java
public final class Personne {
    private final String nom;
    private final List<String> emails;
    
    public Personne(String nom, List<String> emails) {
        this.nom = nom;
        this.emails = emails;
    }
    
    public List<String> getEmails() {
        return emails;
    }
}
```

**R:** Non, **pas immuable**.

**Problèmes**:
1. Le constructor accepte une référence externe (qui peut être modifiée après)
2. Le getter retourne la référence interne (qui peut être modifiée)

**Correct**:
```java
public Personne(String nom, List<String> emails) {
    this.nom = nom;
    this.emails = new ArrayList<>(emails);  // Copie défensive
}

public List<String> getEmails() {
    return new ArrayList<>(emails);  // Copie défensive
    // OU
    return Collections.unmodifiableList(emails);
}
```

---

**Q34: Que se passe-t-il?**
```java
public final class Personne {
    private final List<String> emails;
    
    public Personne(List<String> emails) {
        this.emails = new ArrayList<>(emails);
    }
    
    public List<String> getEmails() {
        return emails;
    }
}

List<String> liste = Arrays.asList("a@mail.com");
Personne p = new Personne(liste);
p.getEmails().add("b@mail.com");  // ?
```

**R:** L'objet est **modifié** via `getEmails().add()`.

Même si le constructor fait une copie, le getter **retourne la référence interne**. Pour être immuable:

```java
public List<String> getEmails() {
    return Collections.unmodifiableList(emails);  // Vue non-modifiable
}
// Maintenant p.getEmails().add() → UnsupportedOperationException
```

---

**Q35: Pourquoi cette classe n'est pas totalement immuable?**
```java
public final class Container {
    private final StringBuilder content;
    
    public Container(StringBuilder content) {
        this.content = new StringBuilder(content);
    }
    
    public StringBuilder getContent() {
        return new StringBuilder(content);
    }
}
```

**R:** Elle **est** immuable!

Les deux méthodes font des **copies défensives**. Modifier le StringBuilder externe ou retourné n'affecte pas l'objet interne.

**Alternative** (si on veut éviter les copies):
```java
public String getContent() {
    return content.toString();  // Retourne un String immuable
}
```

---

## Questions Pièges

**Q36: Combien d'objets sont créés?**
```java
public class Parent {
    public Parent() {
        System.out.println("Parent");
    }
}

public class Enfant extends Parent {
    public Enfant() {
        System.out.println("Enfant");
    }
}

Enfant e = new Enfant();
```

**R:** **1 seul objet** (de type `Enfant`).

Même si deux constructors sont appelés, on ne crée qu'**un seul objet**. Le constructor `Parent` initialise la partie "parent" de cet objet.

---

**Q37: Que s'affiche-t-il?**
```java
public class Parent {
    public void method() {
        System.out.println("Parent");
    }
}

public class Enfant extends Parent {
    public void method() {
        method();
        System.out.println("Enfant");
    }
}

Enfant e = new Enfant();
e.method();
```

**R:** **StackOverflowError** (récursion infinie).

Dans `Enfant.method()`, l'appel `method()` appelle **lui-même** (récursion), pas `Parent.method()`.

**Pour appeler la version parent**:
```java
super.method();  // Appelle Parent.method()
```

---

**Q38: Ce code compile-t-il?**
```java
public class Parent {
    public Parent(int x) { }
}

public class Enfant extends Parent {
    public Enfant() { }
}
```

**R:** Non, **erreur de compilation**: "constructor Parent in class Parent cannot be applied to given types".

`Enfant()` appelle implicitement `super()` (sans paramètres), mais `Parent` n'a **pas** de default constructor.

**Correct**:
```java
public Enfant() {
    super(10);  // Appel explicite avec paramètre
}
```

---

**Q39: Quelle méthode est appelée?**
```java
public class Parent {
    public void method() {
        System.out.println("Parent instance");
    }
    
    public static void staticMethod() {
        System.out.println("Parent static");
    }
}

public class Enfant extends Parent {
    public void method() {
        System.out.println("Enfant instance");
    }
    
    public static void staticMethod() {
        System.out.println("Enfant static");
    }
}

Parent p = new Enfant();
p.method();
p.staticMethod();
```

**R:**
```
Enfant instance
Parent static
```

- **Instance methods**: polymorphisme → méthode de la classe réelle (`Enfant`)
- **Static methods**: pas de polymorphisme → méthode du type de référence (`Parent`)

---

**Q40: Dans quel ordre les variables sont-elles initialisées?**
```java
public class Example {
    private int a = 1;
    private int b = init();
    
    {
        b = 2;
    }
    
    private int init() {
        return a + 10;
    }
    
    public Example() {
        b = 3;
    }
}

Example ex = new Example();
System.out.println(ex.b);
```

**R:** `3`

**Ordre**:
1. `a = 1`
2. `b = init()` → `b = 11` (a vaut 1)
3. Instance initializer → `b = 2`
4. Constructor → `b = 3`

Résultat final: `b = 3`

---

**Q41: Ce code compile-t-il?**
```java
public abstract class Animal {
    public abstract void method();
}

public final abstract class Chien extends Animal {
    @Override
    public void method() { }
}
```

**R:** Non, **erreur de compilation**: "illegal combination of modifiers: abstract and final".

Une classe ne peut **pas** être à la fois `abstract` (conçue pour être étendue) et `final` (ne peut pas être étendue).

---

**Q42: Que retourne getClass()?**
```java
public class Parent { }
public class Enfant extends Parent { }

Parent p = new Enfant();
System.out.println(p.getClass());
System.out.println(p instanceof Parent);
System.out.println(p instanceof Enfant);
```

**R:**
```
class Enfant
true
true
```

`getClass()` retourne la **classe réelle** de l'objet (`Enfant`), pas le type de référence (`Parent`).

---

**Q43: Ce code compile-t-il?**
```java
public class Parent {
    public Object method() {
        return "Parent";
    }
}

public class Enfant extends Parent {
    @Override
    public String method() {
        return "Enfant";
    }
}
```

**R:** Oui, compile.

**Covariant return type**: `String` est un sous-type d'`Object`, donc c'est valide.

---

**Q44: Quelle est la différence?**
```java
// Version 1
public class Enfant extends Parent {
    public Enfant() {
        super();
    }
}

// Version 2
public class Enfant extends Parent {
    public Enfant() {
        // Rien
    }
}
```

**R:** **Aucune différence**.

Si on n'écrit pas `super()`, Java l'ajoute **automatiquement** comme première instruction du constructor.

---

**Q45: Ce code compile-t-il?**
```java
public class Parent {
    private void method() {
        System.out.println("Parent");
    }
}

public class Enfant extends Parent {
    @Override
    public void method() {
        System.out.println("Enfant");
    }
}
```

**R:** Oui, compile (mais **sans warning**).

Ce n'est **pas** un override car la méthode `Parent.method()` est **private** (pas héritée). `Enfant.method()` est une **nouvelle méthode**.

`@Override` causerait une erreur de compilation (ce qui est souhaitable pour détecter l'erreur).

---

**Q46: Combien de fois "Init" s'affiche-t-il?**
```java
public class Parent {
    {
        System.out.println("Init");
    }
}

public class Enfant extends Parent { }

Enfant e1 = new Enfant();
Enfant e2 = new Enfant();
```

**R:** **2 fois**.

Le **instance initializer block** s'exécute à **chaque création** d'objet (même s'il est défini dans la superclasse).

---

**Q47: Que s'affiche-t-il?**
```java
public class Parent {
    public int x = 10;
}

public class Enfant extends Parent {
    public int x = 20;
}

Parent p = new Enfant();
System.out.println(p.x);
System.out.println(((Enfant)p).x);
```

**R:**
```
10
20
```

Les **variables** ne sont **pas** polymorphiques. La variable accédée dépend du **type de référence**, pas du type réel.

**Recommandation**: Ne jamais cacher les variables (utiliser des noms différents).

---

**Q48: Ce code compile-t-il?**
```java
public abstract class Animal {
    public abstract void method();
}

public class Chien extends Animal {
    public void method() {
        super.method();
    }
}
```

**R:** Non, **erreur de compilation**: "abstract method method() in Animal cannot be accessed directly".

On ne peut **pas** appeler une méthode **abstraite** (elle n'a pas d'implémentation).

---

**Q49: Quelle est la valeur de x?**
```java
public final class Example {
    private final int x;
    
    {
        x = 10;
    }
    
    public Example() {
        x = 20;
    }
}
```

**R:** **Erreur de compilation**: "variable x might already have been assigned".

Une variable **final** ne peut être assignée qu'**une seule fois**. Elle est déjà assignée dans l'instance initializer, donc on ne peut pas la réassigner dans le constructor.

**Solutions**:
```java
// Option 1: Initialiser seulement dans le bloc
{
    x = 10;
}
public Example() { }

// Option 2: Initialiser seulement dans le constructor
public Example() {
    x = 20;
}
```

---

**Q50: Cette classe est-elle correctement conçue pour l'immutabilité?**
```java
public final class Config {
    private final Map<String, String> settings;
    
    public Config(Map<String, String> settings) {
        this.settings = new HashMap<>(settings);
    }
    
    public Map<String, String> getSettings() {
        return Collections.unmodifiableMap(settings);
    }
}
```

**R:** Oui, **correctement conçue** pour l'immutabilité.

- Classe **final**
- Field **private final**
- **Copie défensive** dans le constructor
- **Vue non-modifiable** retournée par le getter

Aucune modification externe ne peut affecter l'objet interne.

---