## [**..**](./00_index.md)

# Chapter 5: Methods

## Designing Methods

### Method Structure

Une **méthode** (method) est un bloc de code qui effectue une tâche spécifique.

**Structure complète**:
```java
accessModifier returnType methodName(parameterList) throws exceptionList {
    // method body
    return value;
}
```

**Exemple**:
```java
public static int additionner(int a, int b) {
    return a + b;
}
```

### Components of a Method

```java
public final void calculateScore(int base, int bonus) throws Exception {
    // method body
}
```

**Composants**:
1. **Access modifier**: `public` (qui peut appeler la méthode)
2. **Optional specifiers**: `final`, `static`, `abstract` (modificateurs optionnels)
3. **Return type**: `void` (ce que la méthode retourne)
4. **Method name**: `calculateScore` (identificateur)
5. **Parameter list**: `(int base, int bonus)` (paramètres entre parenthèses)
6. **Optional exception list**: `throws Exception` (exceptions déclarées)
7. **Method body**: `{ }` (code entre accolades)

### Access Modifiers

```java
public class Example {
    public void methodePublique() { }       // Accessible partout
    protected void methodeProtegee() { }    // Accessible dans le package et sous-classes
    void methodePackage() { }               // Accessible dans le package (default)
    private void methodePrivee() { }        // Accessible seulement dans la classe
}
```

### Optional Specifiers

```java
// static: appartient à la classe, pas à l'instance
public static void methodeStatique() { }

// final: ne peut pas être overridden
public final void methodeFinale() { }

// abstract: pas d'implémentation, doit être dans une classe abstraite
public abstract void methodeAbstraite();

// synchronized: pour le multithreading
public synchronized void methodeSynchronisee() { }
```

**Ordre important**: access modifier → optional specifiers → return type

```java
public static final void correct() { }      // OK
static public final void correct2() { }     // OK (mais moins courant)
final public static void correct3() { }     // OK (mais moins courant)
// public void static incorrect() { }       // ERREUR
```

### Return Types

```java
// void: ne retourne rien
public void afficher() {
    System.out.println("Hello");
    // Pas de return nécessaire
}

// primitive type
public int getAge() {
    return 25;
}

// reference type
public String getNom() {
    return "Alice";
}

// array
public int[] getScores() {
    return new int[]{95, 87, 92};
}
```

**Important**: Une méthode non-void **doit** retourner une valeur de ce type.

```java
public int invalide() {
    System.out.println("Hello");
    // ERREUR: missing return statement
}

public int valide() {
    return 42;  // OK
}
```

### Method Names

**Règles** (identiques aux variables):
- Commence par une lettre, `$`, ou `_`
- Peut contenir des lettres, chiffres, `$`, `_`
- Ne peut pas être un mot-clé réservé

**Convention**: camelCase (première lettre minuscule)

```java
public void calculerTotal() { }         // Bon
public void CalculerTotal() { }         // Fonctionne mais non conventionnel
public void calcul_total() { }          // Fonctionne mais non conventionnel
public void calculer2Total() { }        // OK
// public void 2calculer() { }          // ERREUR: commence par un chiffre
```

### Parameter List

```java
// Pas de paramètres
public void sansPrams() { }

// Un paramètre
public void unParam(int x) { }

// Plusieurs paramètres
public void plusieursPrams(int x, String nom, boolean actif) { }

// Même type
public void memeType(int a, int b, int c) { }
```

**Important**: Les parenthèses sont **obligatoires**, même sans paramètres.

```java
public void methode() { }    // OK
// public void methode { }   // ERREUR de compilation
```

### Method Signature

La **signature** d'une méthode = nom + liste des types de paramètres (dans l'ordre).

```java
public void methode(int x) { }              // Signature: methode(int)
public void methode(int x, String s) { }    // Signature: methode(int, String)
public void methode(String s, int x) { }    // Signature: methode(String, int) - DIFFÉRENT!
```

**Ne fait PAS partie de la signature**:
- Return type
- Parameter names
- Access modifiers
- Exceptions

## Declaring Local and Instance Variables

### Local Variables

**Variables locales**: déclarées dans une méthode, existent uniquement pendant l'exécution de la méthode.

```java
public void example() {
    int x = 10;             // Variable locale
    String nom = "Alice";   // Variable locale
    
    System.out.println(x);
}  // x et nom n'existent plus ici
```

**Caractéristiques**:
- **Doivent être initialisées** avant utilisation
- Pas de valeur par défaut
- Ne peuvent pas avoir d'access modifiers

```java
public void incorrect() {
    int x;
    System.out.println(x);  // ERREUR: variable not initialized
}

public void correct() {
    int x = 0;
    System.out.println(x);  // OK
}

public void invalide() {
    // public int x = 10;   // ERREUR: illegal start of expression
    int x = 10;             // OK
}
```

### Instance Variables

**Variables d'instance** (fields): appartiennent à l'objet, existent tant que l'objet existe.

```java
public class Personne {
    private String nom;         // Instance variable
    private int age;            // Instance variable
    
    public void afficher() {
        System.out.println(nom);  // Accessible
        System.out.println(age);  // Accessible
    }
}
```

**Caractéristiques**:
- Ont des **valeurs par défaut** (0, null, false, etc.)
- Peuvent avoir des access modifiers
- Partagées par toutes les méthodes de l'instance

```java
public class Example {
    private int compteur;       // Valeur par défaut: 0
    private String texte;       // Valeur par défaut: null
    private boolean actif;      // Valeur par défaut: false
    
    public void afficher() {
        System.out.println(compteur);  // 0
        System.out.println(texte);     // null
        System.out.println(actif);     // false
    }
}
```

### Variable Scope

```java
public class ScopeExample {
    private int instanceVar = 1;        // Instance variable
    
    public void method(int paramVar) {  // Parameter
        int localVar = 3;               // Local variable
        
        // Tous accessibles ici
        System.out.println(instanceVar);  // 1
        System.out.println(paramVar);     // 2
        System.out.println(localVar);     // 3
    }
    
    public void autreMethode() {
        System.out.println(instanceVar);  // OK
        // System.out.println(paramVar);  // ERREUR
        // System.out.println(localVar);  // ERREUR
    }
}
```

### Masking (Variable Shadowing)

Quand une variable locale a le **même nom** qu'une instance variable:

```java
public class Masking {
    private int compteur = 10;      // Instance variable
    
    public void method() {
        int compteur = 20;          // Local variable (masque l'instance variable)
        System.out.println(compteur);        // 20 (local)
        System.out.println(this.compteur);   // 10 (instance)
    }
}
```

**Utiliser `this`** pour accéder à l'instance variable quand elle est masquée.

## Working with Varargs

**Varargs** (variable arguments): permet de passer un nombre variable d'arguments.

**Syntaxe**: `type... variableName`

```java
public static void afficher(int... nombres) {
    for (int nombre : nombres) {
        System.out.println(nombre);
    }
}

// Utilisation
afficher(1, 2, 3);          // 3 arguments
afficher(1);                // 1 argument
afficher();                 // 0 argument

int[] tableau = {1, 2, 3};
afficher(tableau);          // Passer un array
```

### Varargs Rules

**Règle 1**: Un seul varargs par méthode
```java
// public void method(int... a, String... b) { }  // ERREUR
public void method(int... a) { }                  // OK
```

**Règle 2**: Varargs doit être le **dernier** paramètre
```java
public void correct(String nom, int... scores) { }     // OK
// public void incorrect(int... scores, String nom) { } // ERREUR
```

**Règle 3**: Varargs est traité comme un array
```java
public static void afficher(int... nombres) {
    System.out.println(nombres.length);     // Propriété length
    System.out.println(nombres[0]);         // Accès par index
}
```

### Varargs Examples

```java
public class VarargsExamples {
    
    // Somme de nombres
    public static int somme(int... nombres) {
        int total = 0;
        for (int nombre : nombres) {
            total += nombre;
        }
        return total;
    }
    
    // Avec paramètres normaux
    public static void afficherMessage(String prefixe, String... messages) {
        for (String message : messages) {
            System.out.println(prefixe + ": " + message);
        }
    }
    
    public static void main(String[] args) {
        System.out.println(somme(1, 2, 3));              // 6
        System.out.println(somme(1, 2, 3, 4, 5));        // 15
        System.out.println(somme());                     // 0
        
        afficherMessage("INFO", "Début", "Fin");
        // INFO: Début
        // INFO: Fin
    }
}
```

## Applying Access Modifiers

### The Four Access Levels

| Modifier | Même classe | Même package | Sous-classe (autre package) | Partout |
|----------|-------------|--------------|----------------------------|---------|
| `private` | ✅ | ❌ | ❌ | ❌ |
| default (package-private) | ✅ | ✅ | ❌ | ❌ |
| `protected` | ✅ | ✅ | ✅ | ❌ |
| `public` | ✅ | ✅ | ✅ | ✅ |

### private

Accessible **uniquement** dans la classe où elle est déclarée.

```java
public class Example {
    private int secret = 42;
    
    private void methodePrivee() {
        System.out.println(secret);  // OK (même classe)
    }
    
    public void methodePublique() {
        methodePrivee();  // OK (même classe)
    }
}

class AutreClasse {
    public void test() {
        Example ex = new Example();
        // System.out.println(ex.secret);      // ERREUR
        // ex.methodePrivee();                 // ERREUR
        ex.methodePublique();                  // OK
    }
}
```

### Default (Package-Private)

Accessible dans le **même package** uniquement (pas de mot-clé).

```java
// Fichier: com/example/ClasseA.java
package com.example;

class ClasseA {           // default access
    int nombre = 10;      // default access
    
    void afficher() {     // default access
        System.out.println(nombre);
    }
}

// Fichier: com/example/ClasseB.java
package com.example;

class ClasseB {
    public void test() {
        ClasseA a = new ClasseA();
        System.out.println(a.nombre);  // OK (même package)
        a.afficher();                  // OK (même package)
    }
}

// Fichier: com/autre/ClasseC.java
package com.autre;
import com.example.ClasseA;

class ClasseC {
    public void test() {
        // ClasseA a = new ClasseA();  // ERREUR (package différent)
    }
}
```

### protected

Accessible dans le **même package** ET dans les **sous-classes** (même d'un autre package).

```java
// Fichier: com/example/Parent.java
package com.example;

public class Parent {
    protected int valeur = 100;
    
    protected void afficher() {
        System.out.println(valeur);
    }
}

// Fichier: com/example/Enfant.java (même package)
package com.example;

public class Enfant {
    public void test() {
        Parent p = new Parent();
        System.out.println(p.valeur);  // OK (même package)
        p.afficher();                  // OK (même package)
    }
}

// Fichier: com/autre/SousClasse.java (autre package)
package com.autre;
import com.example.Parent;

public class SousClasse extends Parent {
    public void test() {
        System.out.println(valeur);    // OK (héritage)
        afficher();                    // OK (héritage)
        
        Parent p = new Parent();
        // System.out.println(p.valeur);  // ERREUR (pas via héritage)
    }
}
```

### public

Accessible **partout**.

```java
// Fichier: com/example/Public.java
package com.example;

public class Public {
    public int nombre = 42;
    
    public void afficher() {
        System.out.println(nombre);
    }
}

// Utilisable de n'importe où
package com.autre;
import com.example.Public;

public class Test {
    public void test() {
        Public p = new Public();
        System.out.println(p.nombre);  // OK
        p.afficher();                  // OK
    }
}
```

## Accessing static Data

### static Methods

**static method**: appartient à la **classe**, pas à une instance.

```java
public class Calculator {
    public static int additionner(int a, int b) {
        return a + b;
    }
}

// Appel via le nom de la classe
int resultat = Calculator.additionner(5, 3);  // 8

// Peut aussi appeler via une instance (déconseillé)
Calculator calc = new Calculator();
int resultat2 = calc.additionner(5, 3);      // Fonctionne mais non recommandé
```

### static Variables

**static variable** (class variable): partagée par **toutes les instances**.

```java
public class Compteur {
    private static int count = 0;       // Partagée
    private int instanceCount = 0;      // Par instance
    
    public Compteur() {
        count++;
        instanceCount++;
    }
    
    public static int getCount() {
        return count;
    }
    
    public int getInstanceCount() {
        return instanceCount;
    }
}

// Utilisation
Compteur c1 = new Compteur();
Compteur c2 = new Compteur();
Compteur c3 = new Compteur();

System.out.println(Compteur.getCount());    // 3 (partagé)
System.out.println(c1.getInstanceCount());  // 1
System.out.println(c2.getInstanceCount());  // 1
System.out.println(c3.getInstanceCount());  // 1
```

### static Rules

**Règle 1**: Une méthode static **ne peut pas** accéder directement aux members d'instance.

```java
public class Example {
    private int instanceVar = 10;
    private static int staticVar = 20;
    
    public static void staticMethod() {
        // System.out.println(instanceVar);  // ERREUR!
        System.out.println(staticVar);       // OK
    }
    
    public void instanceMethod() {
        System.out.println(instanceVar);     // OK
        System.out.println(staticVar);       // OK
    }
}
```

**Règle 2**: Une méthode static **ne peut pas** utiliser `this` ou `super`.

```java
public class Example {
    private int valeur = 10;
    
    public static void staticMethod() {
        // System.out.println(this.valeur);  // ERREUR!
    }
    
    public void instanceMethod() {
        System.out.println(this.valeur);     // OK
    }
}
```

**Règle 3**: Pour accéder aux members d'instance depuis une méthode static, il faut créer une instance.

```java
public class Example {
    private int instanceVar = 10;
    
    public static void staticMethod() {
        Example ex = new Example();
        System.out.println(ex.instanceVar);  // OK (via une instance)
    }
}
```

### static Initializers

**static initializer block**: exécuté **une seule fois** quand la classe est chargée.

```java
public class Configuration {
    private static String config;
    
    // static initializer
    static {
        System.out.println("Initialisation de Configuration");
        config = "Production";
    }
    
    public static String getConfig() {
        return config;
    }
}

// Premier accès à la classe
System.out.println(Configuration.getConfig());  // Affiche "Initialisation..." puis "Production"
System.out.println(Configuration.getConfig());  // Affiche seulement "Production"
```

### static Imports

Permet d'importer des **static members** pour les utiliser sans le nom de la classe.

```java
// Sans static import
import java.lang.Math;

public class Example {
    public static void main(String[] args) {
        double sqrt = Math.sqrt(16);
        double pi = Math.PI;
    }
}

// Avec static import
import static java.lang.Math.sqrt;
import static java.lang.Math.PI;

public class Example {
    public static void main(String[] args) {
        double sqrt = sqrt(16);      // Plus besoin de Math.
        double pi = PI;              // Plus besoin de Math.
    }
}

// Import tous les static members
import static java.lang.Math.*;

public class Example {
    public static void main(String[] args) {
        double sqrt = sqrt(16);
        double pi = PI;
        double max = max(10, 20);
    }
}
```

**À utiliser avec modération** pour éviter la confusion.

## Passing Data among Methods

### Pass-by-Value

Java est **pass-by-value**: la **valeur** du paramètre est copiée.

**Pour les primitives**: la valeur elle-même est copiée.

```java
public static void modifier(int x) {
    x = 20;
    System.out.println("Dans la méthode: " + x);  // 20
}

public static void main(String[] args) {
    int nombre = 10;
    modifier(nombre);
    System.out.println("Après l'appel: " + nombre);  // 10 (inchangé)
}
```

**Pour les references**: l'**adresse mémoire** est copiée (mais l'objet reste le même).

```java
public static void modifier(StringBuilder sb) {
    sb.append(" World");
    System.out.println("Dans la méthode: " + sb);  // "Hello World"
}

public static void main(String[] args) {
    StringBuilder texte = new StringBuilder("Hello");
    modifier(texte);
    System.out.println("Après l'appel: " + texte);  // "Hello World" (modifié!)
}
```

**Attention**: Si on réassigne la référence, ça n'affecte pas l'original.

```java
public static void modifier(StringBuilder sb) {
    sb = new StringBuilder("Nouveau");  // Réassignation locale
}

public static void main(String[] args) {
    StringBuilder texte = new StringBuilder("Hello");
    modifier(texte);
    System.out.println(texte);  // "Hello" (inchangé)
}
```

### Autoboxing and Unboxing

**Autoboxing**: conversion automatique de primitive → wrapper.
**Unboxing**: conversion automatique de wrapper → primitive.

```java
// Autoboxing
Integer i = 10;           // int → Integer automatiquement

// Unboxing
int x = i;                // Integer → int automatiquement

// Dans les méthodes
public static void afficher(Integer nombre) {
    System.out.println(nombre);
}

afficher(5);              // Autoboxing: int → Integer
```

**Liste des wrappers**:
- `byte` → `Byte`
- `short` → `Short`
- `int` → `Integer`
- `long` → `Long`
- `float` → `Float`
- `double` → `Double`
- `char` → `Character`
- `boolean` → `Boolean`

**Piège avec null**:
```java
Integer i = null;
// int x = i;            // NullPointerException! (unboxing de null)

// Vérifier null avant unboxing
if (i != null) {
    int x = i;            // OK
}
```

### Returning Values

```java
// Retourner une primitive
public static int additionner(int a, int b) {
    return a + b;
}

// Retourner un objet
public static String creerMessage(String nom) {
    return "Bonjour " + nom;
}

// Retourner un array
public static int[] creerArray(int taille) {
    return new int[taille];
}

// Méthode void (pas de return)
public static void afficher(String message) {
    System.out.println(message);
    // return;  // Optionnel dans void
}
```

## Overloading Methods

**Method overloading**: plusieurs méthodes avec le **même nom** mais des **signatures différentes**.

### Overloading Rules

**Différence requise**: types de paramètres, nombre de paramètres, ou ordre des paramètres.

```java
public class Calculator {
    // Surcharge basée sur le nombre de paramètres
    public int additionner(int a, int b) {
        return a + b;
    }
    
    public int additionner(int a, int b, int c) {
        return a + b + c;
    }
    
    // Surcharge basée sur le type de paramètres
    public double additionner(double a, double b) {
        return a + b;
    }
    
    // Surcharge basée sur l'ordre des paramètres
    public void afficher(String texte, int nombre) {
        System.out.println(texte + ": " + nombre);
    }
    
    public void afficher(int nombre, String texte) {
        System.out.println(nombre + " - " + texte);
    }
}
```

### What Doesn't Count for Overloading

**Ne permettent PAS l'overloading**:
- Return type différent seul
- Parameter names différents
- Access modifiers différents

```java
// ERREUR: ne diffèrent que par le return type
// public int methode(int x) { return x; }
// public double methode(int x) { return x; }

// ERREUR: ne diffèrent que par les noms des paramètres
// public void methode(int a) { }
// public void methode(int b) { }

// OK: même nom mais paramètres différents
public void methode(int x) { }
public void methode(String x) { }
```

### Overloading with Varargs

```java
public class VarargsOverload {
    public void afficher(int... nombres) {
        System.out.println("varargs");
    }
    
    public void afficher(int nombre) {
        System.out.println("int");
    }
    
    public void afficher(int a, int b) {
        System.out.println("deux int");
    }
}

VarargsOverload obj = new VarargsOverload();
obj.afficher(1);          // "int" (version exacte prioritaire)
obj.afficher(1, 2);       // "deux int" (version exacte prioritaire)
obj.afficher(1, 2, 3);    // "varargs"
obj.afficher();           // "varargs"
```

**Règle**: La version la **plus spécifique** est choisie.

### Overloading and Type Promotion

Java peut **promouvoir** les types pour trouver une méthode correspondante.

```java
public class Promotion {
    public void afficher(int x) {
        System.out.println("int");
    }
    
    public void afficher(long x) {
        System.out.println("long");
    }
}

Promotion obj = new Promotion();
byte b = 10;
obj.afficher(b);     // "int" (byte promu en int)

short s = 10;
obj.afficher(s);     // "int" (short promu en int)

obj.afficher(10);    // "int" (match exact)
obj.afficher(10L);   // "long" (match exact)
```

**Ordre de promotion**: `byte` → `short` → `int` → `long` → `float` → `double`

### Overloading Examples

```java
public class StringUtils {
    
    // Répéter un caractère
    public static String repeter(char c, int fois) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < fois; i++) {
            sb.append(c);
        }
        return sb.toString();
    }
    
    // Répéter un String
    public static String repeter(String s, int fois) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < fois; i++) {
            sb.append(s);
        }
        return sb.toString();
    }
    
    // Répéter avec séparateur
    public static String repeter(String s, int fois, String separateur) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < fois; i++) {
            sb.append(s);
            if (i < fois - 1) {
                sb.append(separateur);
            }
        }
        return sb.toString();
    }
    
    public static void main(String[] args) {
        System.out.println(repeter('*', 5));                    // "*****"
        System.out.println(repeter("Ha", 3));                   // "HaHaHa"
        System.out.println(repeter("Java", 3, " - "));          // "Java - Java - Java"
    }
}
```

---

## Exemples Pratiques Complets

### Exemple 1: Classe Compte Bancaire
```java
public class CompteBancaire {
    private String numeroCompte;
    private double solde;
    private static int nombreComptes = 0;
    
    // Constructor
    public CompteBancaire(String numeroCompte) {
        this.numeroCompte = numeroCompte;
        this.solde = 0.0;
        nombreComptes++;
    }
    
    // Constructor avec solde initial
    public CompteBancaire(String numeroCompte, double soldeInitial) {
        this(numeroCompte);  // Appel au premier constructor
        if (soldeInitial > 0) {
            this.solde = soldeInitial;
        }
    }
    
    // Méthode d'instance
    public void deposer(double montant) {
        if (montant > 0) {
            solde += montant;
            System.out.println("Dépôt de " + montant + " effectué");
        } else {
            System.out.println("Montant invalide");
        }
    }
    
    // Overload avec message personnalisé
    public void deposer(double montant, String description) {
        deposer(montant);
        System.out.println("Description: " + description);
    }
    
    // Méthode d'instance
    public boolean retirer(double montant) {
        if (montant > 0 && montant <= solde) {
            solde -= montant;
            System.out.println("Retrait de " + montant + " effectué");
            return true;
        }
        System.out.println("Retrait impossible");
        return false;
    }
    
    // Getter
    public double getSolde() {
        return solde;
    }
    
    // Méthode static
    public static int getNombreComptes() {
        return nombreComptes;
    }
    
    // Méthode d'instance
    public void afficherInfos() {
        System.out.println("Compte: " + numeroCompte);
        System.out.println("Solde: " + solde + "€");
    }
    
    public static void main(String[] args) {
        CompteBancaire compte1 = new CompteBancaire("FR001");
        CompteBancaire compte2 = new CompteBancaire("FR002", 1000);
        
        compte1.deposer(500);
        compte1.deposer(200, "Salaire");
        compte1.retirer(100);
        
        compte1.afficherInfos();
        System.out.println("Nombre total de comptes: " + CompteBancaire.getNombreComptes());
    }
}
```

### Exemple 2: Classe avec Varargs
```java
public class StatistiquesCalculator {
    
    // Calculer la moyenne avec varargs
    public static double moyenne(double... nombres) {
        if (nombres.length == 0) {
            return 0;
        }
        
        double somme = 0;
        for (double nombre : nombres) {
            somme += nombre;
        }
        return somme / nombres.length;
    }
    
    // Trouver le maximum
    public static double max(double... nombres) {
        if (nombres.length == 0) {
            throw new IllegalArgumentException("Au moins un nombre requis");
        }
        
        double maximum = nombres[0];
        for (double nombre : nombres) {
            if (nombre > maximum) {
                maximum = nombre;
            }
        }
        return maximum;
    }
    
    // Trouver le minimum
    public static double min(double... nombres) {
        if (nombres.length == 0) {
            throw new IllegalArgumentException("Au moins un nombre requis");
        }
        
        double minimum = nombres[0];
        for (double nombre : nombres) {
            if (nombre < minimum) {
                minimum = nombre;
            }
        }
        return minimum;
    }
    
    // Afficher des statistiques
    public static void afficherStats(String titre, double... nombres) {
        System.out.println("=== " + titre + " ===");
        System.out.println("Nombre de valeurs: " + nombres.length);
        System.out.println("Moyenne: " + moyenne(nombres));
        System.out.println("Maximum: " + max(nombres));
        System.out.println("Minimum: " + min(nombres));
    }
    
    public static void main(String[] args) {
        afficherStats("Notes des étudiants", 85, 92, 78, 95, 88);
        System.out.println();
        afficherStats("Températures", 20.5, 22.3, 19.8, 21.0);
    }
}
```

### Exemple 3: Overloading Complexe
```java
public class MessageFormatter {
    
    // Formater avec String
    public String formater(String message) {
        return "[INFO] " + message;
    }
    
    // Formater avec niveau de log
    public String formater(String message, String niveau) {
        return "[" + niveau.toUpperCase() + "] " + message;
    }
    
    // Formater avec timestamp
    public String formater(String message, boolean avecTimestamp) {
        if (avecTimestamp) {
            return "[" + java.time.LocalTime.now() + "] " + message;
        }
        return message;
    }
    
    // Formater avec niveau et timestamp
    public String formater(String message, String niveau, boolean avecTimestamp) {
        String base = formater(message, niveau);
        if (avecTimestamp) {
            return "[" + java.time.LocalTime.now() + "] " + base;
        }
        return base;
    }
    
    // Formater plusieurs messages
    public String formater(String... messages) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < messages.length; i++) {
            sb.append((i + 1)).append(". ").append(messages[i]);
            if (i < messages.length - 1) {
                sb.append("\n");
            }
        }
        return sb.toString();
    }
    
    public static void main(String[] args) {
        MessageFormatter formatter = new MessageFormatter();
        
        System.out.println(formatter.formater("Démarrage"));
        System.out.println(formatter.formater("Erreur détectée", "ERROR"));
        System.out.println(formatter.formater("Opération terminée", true));
        System.out.println(formatter.formater("Alerte importante", "WARN", true));
        System.out.println(formatter.formater("Ligne 1", "Ligne 2", "Ligne 3"));
    }
}
```

### Exemple 4: Classe Utilitaire avec static
```java
public class MathUtils {
    
    // Constructor privé pour empêcher l'instanciation
    private MathUtils() {
        throw new UnsupportedOperationException("Classe utilitaire");
    }
    
    // Calculer le factoriel
    public static long factoriel(int n) {
        if (n < 0) {
            throw new IllegalArgumentException("n doit être positif");
        }
        if (n == 0 || n == 1) {
            return 1;
        }
        
        long resultat = 1;
        for (int i = 2; i <= n; i++) {
            resultat *= i;
        }
        return resultat;
    }
    
    // Vérifier si nombre premier
    public static boolean estPremier(int n) {
        if (n < 2) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;
        
        for (int i = 3; i * i <= n; i += 2) {
            if (n % i == 0) {
                return false;
            }
        }
        return true;
    }
    
    // PGCD (Plus Grand Commun Diviseur)
    public static int pgcd(int a, int b) {
        a = Math.abs(a);
        b = Math.abs(b);
        
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }
    
    // PPCM (Plus Petit Commun Multiple)
    public static int ppcm(int a, int b) {
        if (a == 0 || b == 0) {
            return 0;
        }
        return Math.abs(a * b) / pgcd(a, b);
    }
    
    // Fibonacci
    public static long fibonacci(int n) {
        if (n < 0) {
            throw new IllegalArgumentException("n doit être positif");
        }
        if (n == 0) return 0;
        if (n == 1) return 1;
        
        long a = 0, b = 1;
        for (int i = 2; i <= n; i++) {
            long temp = a + b;
            a = b;
            b = temp;
        }
        return b;
    }
    
    public static void main(String[] args) {
        System.out.println("Factoriel de 5: " + factoriel(5));
        System.out.println("17 est premier: " + estPremier(17));
        System.out.println("PGCD(48, 18): " + pgcd(48, 18));
        System.out.println("PPCM(12, 18): " + ppcm(12, 18));
        System.out.println("Fibonacci(10): " + fibonacci(10));
    }
}
```

### Exemple 5: Pass-by-Value Démonstration
```java
public class PassByValueDemo {
    
    // Ne modifie PAS l'original (primitive)
    public static void modifierPrimitive(int x) {
        x = 100;
        System.out.println("Dans la méthode (primitive): " + x);
    }
    
    // Modifie l'objet (mais pas la référence)
    public static void modifierObjet(StringBuilder sb) {
        sb.append(" World");
        System.out.println("Dans la méthode (objet): " + sb);
    }
    
    // Ne modifie PAS l'original (réassignation)
    public static void reassignerObjet(StringBuilder sb) {
        sb = new StringBuilder("Nouveau");
        System.out.println("Dans la méthode (réassigné): " + sb);
    }
    
    // Modifie le contenu de l'array
    public static void modifierArray(int[] tableau) {
        tableau[0] = 999;
        System.out.println("Dans la méthode (array): " + java.util.Arrays.toString(tableau));
    }
    
    // Ne modifie PAS l'array original (réassignation)
    public static void reassignerArray(int[] tableau) {
        tableau = new int[]{100, 200, 300};
        System.out.println("Dans la méthode (array réassigné): " + java.util.Arrays.toString(tableau));
    }
    
    public static void main(String[] args) {
        // Test avec primitive
        int nombre = 10;
        modifierPrimitive(nombre);
        System.out.println("Après l'appel (primitive): " + nombre);
        System.out.println();
        
        // Test avec objet modifié
        StringBuilder texte = new StringBuilder("Hello");
        modifierObjet(texte);
        System.out.println("Après l'appel (objet): " + texte);
        System.out.println();
        
        // Test avec objet réassigné
        StringBuilder texte2 = new StringBuilder("Original");
        reassignerObjet(texte2);
        System.out.println("Après l'appel (réassigné): " + texte2);
        System.out.println();
        
        // Test avec array modifié
        int[] tableau = {1, 2, 3};
        modifierArray(tableau);
        System.out.println("Après l'appel (array): " + java.util.Arrays.toString(tableau));
        System.out.println();
        
        // Test avec array réassigné
        int[] tableau2 = {10, 20, 30};
        reassignerArray(tableau2);
        System.out.println("Après l'appel (array réassigné): " + java.util.Arrays.toString(tableau2));
    }
}
```

### Exemple 6: Classe avec Initialization Order
```java
public class InitializationOrderDemo {
    
    // 1. Static variables
    private static int staticVar = initialiserStatic();
    
    // 2. Static initializer block
    static {
        System.out.println("3. Static initializer block");
        staticVar += 10;
    }
    
    // 4. Instance variables
    private int instanceVar = initialiserInstance();
    
    // 5. Instance initializer block
    {
        System.out.println("6. Instance initializer block");
        instanceVar += 10;
    }
    
    // 7. Constructor
    public InitializationOrderDemo() {
        System.out.println("7. Constructor");
        System.out.println("   staticVar = " + staticVar);
        System.out.println("   instanceVar = " + instanceVar);
    }
    
    private static int initialiserStatic() {
        System.out.println("2. Initialisation static variable");
        return 100;
    }
    
    private int initialiserInstance() {
        System.out.println("5. Initialisation instance variable");
        return 200;
    }
    
    public static void main(String[] args) {
        System.out.println("1. Début du main");
        System.out.println();
        
        System.out.println("=== Première instance ===");
        InitializationOrderDemo obj1 = new InitializationOrderDemo();
        System.out.println();
        
        System.out.println("=== Deuxième instance ===");
        InitializationOrderDemo obj2 = new InitializationOrderDemo();
    }
}
```

Output:
```
1. Début du main
2. Initialisation static variable
3. Static initializer block

=== Première instance ===
5. Initialisation instance variable
6. Instance initializer block
7. Constructor
   staticVar = 110
   instanceVar = 210

=== Deuxième instance ===
5. Initialisation instance variable
6. Instance initializer block
7. Constructor
   staticVar = 110
   instanceVar = 210
```

---

## Points Clés du Chapter 5

✅ **Method structure**: access modifier + optional specifiers + return type + name + parameters

✅ **Access modifiers**: `private` < default < `protected` < `public`

✅ **Local variables**: doivent être initialisées, pas de valeur par défaut, pas d'access modifier

✅ **Instance variables**: valeurs par défaut, peuvent avoir access modifiers

✅ **static**: appartient à la classe, pas aux instances
   - static methods ne peuvent accéder qu'aux static members directement
   - Pas de `this` ou `super` dans static context

✅ **Varargs**: 
   - Syntaxe: `type... name`
   - Doit être le dernier paramètre
   - Un seul varargs par méthode
   - Traité comme un array

✅ **Pass-by-value**:
   - Primitives: valeur copiée
   - References: adresse copiée (l'objet peut être modifié, mais pas la référence elle-même)

✅ **Autoboxing/Unboxing**: conversion automatique primitive ↔ wrapper

✅ **Method overloading**: même nom, signatures différentes
   - Doit différer par: nombre, type, ou ordre des paramètres
   - Return type seul ne suffit pas
   - Version la plus spécifique choisie

✅ **Method signature**: nom + types de paramètres (dans l'ordre)

✅ **Initialization order**:
   1. Static variables et static initializers (une fois)
   2. Instance variables et instance initializers (à chaque création)
   3. Constructor

---

# Questions/Réponses - Chapter 5: Methods

## Questions sur la Structure des Méthodes

**Q1: Ce code compile-t-il?**
```java
public class Example {
    public void static method() {
        System.out.println("Hello");
    }
}
```

**R:** Non, **erreur de compilation**: "illegal start of type".

L'**ordre** est important: access modifier → optional specifiers → return type.

**Correct**:
```java
public static void method() {  // OK
```

---

**Q2: Combien d'erreurs dans ce code?**
```java
public int calculate {
    return 42;
}
```

**R:** **1 erreur**: les **parenthèses** sont obligatoires pour la liste des paramètres, même vide.

**Correct**:
```java
public int calculate() {
    return 42;
}
```

---

**Q3: Ce code compile-t-il?**
```java
public int getValue() {
    System.out.println("Hello");
}
```

**R:** Non, **erreur de compilation**: "missing return statement".

Une méthode **non-void** doit **toujours** retourner une valeur du type déclaré.

**Correct**:
```java
public int getValue() {
    System.out.println("Hello");
    return 42;
}
```

---

**Q4: Quelle est la signature de cette méthode?**
```java
public static int additionner(int a, int b) {
    return a + b;
}
```

**R:** `additionner(int, int)`

La **signature** = nom + types des paramètres (dans l'ordre).

**Ne font PAS partie de la signature**:
- Return type (`int`)
- Access modifier (`public`)
- Specifier (`static`)
- Noms des paramètres (`a`, `b`)

---

**Q5: Ces deux méthodes peuvent-elles coexister dans la même classe?**
```java
public int calculate(int x) {
    return x * 2;
}

public double calculate(int x) {
    return x * 2.0;
}
```

**R:** Non, **erreur de compilation**: "method calculate(int) is already defined".

Elles ont la **même signature** `calculate(int)`. Le **return type** différent ne suffit pas pour l'overloading.

---

## Questions sur les Variables

**Q6: Que s'affiche-t-il?**
```java
public class Test {
    public void method() {
        int x;
        System.out.println(x);
    }
}
```

**R:** **Erreur de compilation**: "variable x might not have been initialized".

Les **variables locales** n'ont **pas de valeur par défaut**. Elles doivent être initialisées avant utilisation.

---

**Q7: Que s'affiche-t-il?**
```java
public class Test {
    private int x;
    
    public void afficher() {
        System.out.println(x);
    }
}

Test t = new Test();
t.afficher();
```

**R:** `0`

Les **instance variables** ont des **valeurs par défaut**: `0` pour les types numériques.

---

**Q8: Ce code compile-t-il?**
```java
public class Test {
    public void method() {
        public int x = 10;
        System.out.println(x);
    }
}
```

**R:** Non, **erreur de compilation**: "illegal start of expression".

Les **variables locales** ne peuvent **pas** avoir d'access modifiers (`public`, `private`, etc.).

---

**Q9: Que s'affiche-t-il?**
```java
public class Test {
    private int compteur = 10;
    
    public void method() {
        int compteur = 20;
        System.out.println(compteur);
        System.out.println(this.compteur);
    }
}

Test t = new Test();
t.method();
```

**R:**
```
20
10
```

**Variable shadowing**: La variable locale `compteur` **masque** l'instance variable.
- `compteur` → variable locale (20)
- `this.compteur` → instance variable (10)

---

**Q10: Quelle est la portée de chaque variable?**
```java
public class Scope {
    private int a = 1;          // ?
    
    public void method(int b) { // ?
        int c = 3;              // ?
        
        if (true) {
            int d = 4;          // ?
        }
        // System.out.println(d); // ?
    }
}
```

**R:**
- `a`: **Instance variable** - accessible dans toute la classe
- `b`: **Parameter** - accessible dans toute la méthode
- `c`: **Local variable** - accessible dans toute la méthode
- `d`: **Local variable** - accessible uniquement dans le bloc `if`
- `System.out.println(d)` → **ERREUR** (d n'existe plus hors du bloc if)

---

## Questions sur static

**Q11: Ce code compile-t-il?**
```java
public class Test {
    private int instanceVar = 10;
    
    public static void staticMethod() {
        System.out.println(instanceVar);
    }
}
```

**R:** Non, **erreur de compilation**: "non-static variable instanceVar cannot be referenced from a static context".

Une méthode **static** ne peut **pas** accéder directement aux **instance members**.

**Correct**:
```java
public static void staticMethod() {
    Test obj = new Test();
    System.out.println(obj.instanceVar);  // Via une instance
}
```

---

**Q12: Que s'affiche-t-il?**
```java
public class Compteur {
    private static int count = 0;
    
    public Compteur() {
        count++;
    }
    
    public static int getCount() {
        return count;
    }
}

Compteur c1 = new Compteur();
Compteur c2 = new Compteur();
Compteur c3 = new Compteur();
System.out.println(Compteur.getCount());
```

**R:** `3`

La variable **static** `count` est **partagée** par toutes les instances. Chaque création incrémente le même `count`.

---

**Q13: Ce code compile-t-il?**
```java
public class Test {
    private int x = 10;
    
    public static void method() {
        System.out.println(this.x);
    }
}
```

**R:** Non, **erreur de compilation**: "non-static variable this cannot be referenced from a static context".

Les méthodes **static** ne peuvent **pas** utiliser `this` ou `super` (ils font référence à une instance).

---

**Q14: Combien de fois "Init" s'affiche-t-il?**
```java
public class Test {
    static {
        System.out.println("Init");
    }
    
    public Test() {
        System.out.println("Constructor");
    }
}

Test t1 = new Test();
Test t2 = new Test();
```

**R:**
```
Init
Constructor
Constructor
```

Le **static initializer block** s'exécute **une seule fois** quand la classe est chargée (premier accès).
Le **constructor** s'exécute à chaque création d'instance.

---

**Q15: Quel est l'ordre d'exécution?**
```java
public class Test {
    private static int a = init1();
    private int b = init2();
    
    static {
        System.out.println("3");
    }
    
    {
        System.out.println("5");
    }
    
    public Test() {
        System.out.println("6");
    }
    
    private static int init1() {
        System.out.println("1");
        return 1;
    }
    
    private int init2() {
        System.out.println("4");
        return 2;
    }
}

Test t = new Test();
```

**R:** `1 3 4 5 6`

**Ordre**:
1. Static variables dans l'ordre (`init1()` → "1")
2. Static initializer blocks ("3")
3. Instance variables dans l'ordre (`init2()` → "4")
4. Instance initializer blocks ("5")
5. Constructor ("6")

---

**Q16: Après import static, ce code compile-t-il?**
```java
import static java.lang.Math.*;

public class Test {
    public static void main(String[] args) {
        double result = sqrt(16);
        System.out.println(PI);
    }
}
```

**R:** Oui, compile et affiche:
```
4.0
3.141592653589793
```

**static import** permet d'utiliser les static members sans le nom de la classe (`Math.`).

---

## Questions sur Varargs

**Q17: Ce code compile-t-il?**
```java
public void method(int... a, String... b) {
    System.out.println("Hello");
}
```

**R:** Non, **erreur de compilation**: "varargs parameter must be last".

On ne peut avoir qu'**un seul varargs** par méthode, et il doit être le **dernier paramètre**.

---

**Q18: Ce code compile-t-il?**
```java
public void method(String nom, int... scores) {
    System.out.println(nom);
}
```

**R:** Oui, c'est **valide**. Le varargs peut être précédé d'autres paramètres, tant qu'il est le **dernier**.

---

**Q19: Quels appels sont valides?**
```java
public static void afficher(int... nombres) {
    System.out.println(java.util.Arrays.toString(nombres));
}

// Appels:
afficher();                  // ?
afficher(1);                 // ?
afficher(1, 2, 3);          // ?
int[] tab = {1, 2, 3};
afficher(tab);               // ?
afficher(null);              // ?
```

**R:** Tous sont **valides**:
```java
afficher();          // [] (array vide)
afficher(1);         // [1]
afficher(1, 2, 3);   // [1, 2, 3]
afficher(tab);       // [1, 2, 3]
afficher(null);      // null
```

---

**Q20: Que s'affiche-t-il?**
```java
public static void afficher(int... nombres) {
    System.out.println(nombres.length);
}

afficher();
afficher(1, 2, 3);
```

**R:**
```
0
3
```

Varargs est traité comme un **array**. On peut utiliser `.length`.

---

## Questions sur Pass-by-Value

**Q21: Que s'affiche-t-il?**
```java
public static void modifier(int x) {
    x = 100;
}

public static void main(String[] args) {
    int nombre = 10;
    modifier(nombre);
    System.out.println(nombre);
}
```

**R:** `10` (inchangé)

Java est **pass-by-value** pour les primitives. La **valeur** est copiée, donc modifier `x` n'affecte pas `nombre`.

---

**Q22: Que s'affiche-t-il?**
```java
public static void modifier(StringBuilder sb) {
    sb.append(" World");
}

public static void main(String[] args) {
    StringBuilder texte = new StringBuilder("Hello");
    modifier(texte);
    System.out.println(texte);
}
```

**R:** `"Hello World"` (modifié!)

Pour les **references**, l'**adresse mémoire** est copiée. `sb` et `texte` pointent vers le **même objet**, donc la modification affecte l'original.

---

**Q23: Que s'affiche-t-il?**
```java
public static void modifier(StringBuilder sb) {
    sb = new StringBuilder("Nouveau");
}

public static void main(String[] args) {
    StringBuilder texte = new StringBuilder("Original");
    modifier(texte);
    System.out.println(texte);
}
```

**R:** `"Original"` (inchangé)

La **réassignation** de `sb` dans la méthode ne change que la **copie locale** de la référence, pas la référence originale.

---

**Q24: Que contient l'array après l'appel?**
```java
public static void modifier(int[] tableau) {
    tableau[0] = 999;
}

public static void main(String[] args) {
    int[] nombres = {1, 2, 3};
    modifier(nombres);
    System.out.println(java.util.Arrays.toString(nombres));
}
```

**R:** `[999, 2, 3]`

Les **arrays sont des objets**. La référence est copiée, donc on peut modifier le **contenu** de l'array.

---

**Q25: Que contient l'array après l'appel?**
```java
public static void modifier(int[] tableau) {
    tableau = new int[]{100, 200, 300};
}

public static void main(String[] args) {
    int[] nombres = {1, 2, 3};
    modifier(nombres);
    System.out.println(java.util.Arrays.toString(nombres));
}
```

**R:** `[1, 2, 3]` (inchangé)

La **réassignation** de `tableau` ne change que la copie locale de la référence.

---

## Questions sur Autoboxing/Unboxing

**Q26: Ce code compile-t-il?**
```java
Integer i = 10;
int x = i;
```

**R:** Oui, compile.
- `Integer i = 10;` → **Autoboxing** (int → Integer)
- `int x = i;` → **Unboxing** (Integer → int)

---

**Q27: Que se passe-t-il ici?**
```java
Integer i = null;
int x = i;
```

**R:** **NullPointerException** à l'exécution.

Lors de l'**unboxing**, Java essaie de convertir `null` en `int` → exception.

**Correct**:
```java
Integer i = null;
if (i != null) {
    int x = i;  // OK
}
```

---

**Q28: Ce code compile-t-il?**
```java
public static void afficher(Integer nombre) {
    System.out.println(nombre);
}

afficher(5);
```

**R:** Oui, compile.

**Autoboxing** convertit automatiquement `5` (int) en `Integer`.

---

## Questions sur Overloading

**Q29: Quelle méthode est appelée?**
```java
public class Test {
    public void afficher(int x) {
        System.out.println("int");
    }
    
    public void afficher(long x) {
        System.out.println("long");
    }
    
    public void afficher(Integer x) {
        System.out.println("Integer");
    }
}

Test t = new Test();
t.afficher(5);
```

**R:** `"int"`

La version avec **match exact** est prioritaire. `5` est un `int`, donc la première méthode est choisie.

---

**Q30: Quelle méthode est appelée?**
```java
public class Test {
    public void afficher(long x) {
        System.out.println("long");
    }
    
    public void afficher(Integer x) {
        System.out.println("Integer");
    }
}

Test t = new Test();
t.afficher(5);
```

**R:** `"long"`

Java préfère la **promotion de primitive** (`int` → `long`) plutôt que l'**autoboxing** (`int` → `Integer`).

**Ordre de priorité**:
1. Match exact
2. Promotion de primitive
3. Autoboxing
4. Varargs

---

**Q31: Ces deux méthodes peuvent-elles coexister?**
```java
public void method(int a, String b) { }
public void method(String a, int b) { }
```

**R:** Oui, c'est **valide**.

Elles ont des signatures **différentes**: `method(int, String)` vs `method(String, int)`.

L'**ordre** des paramètres compte pour l'overloading.

---

**Q32: Ces deux méthodes peuvent-elles coexister?**
```java
public int calculate(int x) {
    return x * 2;
}

public void calculate(int x) {
    System.out.println(x);
}
```

**R:** Non, **erreur de compilation**: "method calculate(int) is already defined".

Même signature `calculate(int)`. Le **return type** différent ne suffit pas.

---

**Q33: Quelle méthode est appelée?**
```java
public class Test {
    public void afficher(int x) {
        System.out.println("int");
    }
    
    public void afficher(int... x) {
        System.out.println("varargs");
    }
}

Test t = new Test();
t.afficher(5);
t.afficher(5, 10);
t.afficher();
```

**R:**
```
int
varargs
varargs
```

La version **exacte** est toujours prioritaire. Varargs est utilisé seulement si aucune autre ne correspond.

---

**Q34: Quelle méthode est appelée?**
```java
public class Test {
    public void afficher(Object obj) {
        System.out.println("Object");
    }
    
    public void afficher(String s) {
        System.out.println("String");
    }
}

Test t = new Test();
t.afficher("Hello");
```

**R:** `"String"`

La version la **plus spécifique** est choisie. `String` est plus spécifique qu'`Object`.

---

**Q35: Ce code compile-t-il?**
```java
public void method(int x) { }
public void method(int y) { }
```

**R:** Non, **erreur de compilation**: "method method(int) is already defined".

Les **noms des paramètres** ne comptent **pas** pour l'overloading. Les deux méthodes ont la même signature `method(int)`.

---

**Q36: Quelle méthode est appelée?**
```java
public class Test {
    public void afficher(byte x) {
        System.out.println("byte");
    }
    
    public void afficher(short x) {
        System.out.println("short");
    }
    
    public void afficher(int x) {
        System.out.println("int");
    }
}

Test t = new Test();
byte b = 10;
t.afficher(b);
```

**R:** `"byte"`

Match **exact** avec `byte`.

---

**Q37: Quelle méthode est appelée?**
```java
public class Test {
    public void afficher(int x) {
        System.out.println("int");
    }
    
    public void afficher(long x) {
        System.out.println("long");
    }
}

Test t = new Test();
byte b = 10;
t.afficher(b);
```

**R:** `"int"`

Pas de match exact pour `byte`. Java **promeut** `byte` → `short` → `int`.

---

**Q38: Ce code compile-t-il?**
```java
public void method(int x, String... messages) { }
public void method(int x, String y) { }
```

**R:** Oui, compile. Ce sont des **signatures différentes**:
- `method(int, String...)`
- `method(int, String)`

Mais attention à l'**ambiguïté** lors de l'appel:
```java
method(5, "Hello");  // Ambiguë! Les deux correspondent
```

---

## Questions Pièges

**Q39: Combien de paramètres cette méthode accepte-t-elle?**
```java
public void method(int x, int... y) { }
```

**R:** **Au minimum 1**, au maximum **illimité**.

- `x` est obligatoire (1 paramètre minimum)
- `y` peut être vide ou contenir plusieurs valeurs

```java
method(5);           // OK: x=5, y=[]
method(5, 10);       // OK: x=5, y=[10]
method(5, 10, 20);   // OK: x=5, y=[10, 20]
// method();         // ERREUR: x manquant
```

---

**Q40: Que s'affiche-t-il?**
```java
public class Test {
    private static int count = 0;
    
    public Test() {
        count++;
    }
    
    public int getCount() {
        return count;
    }
}

Test t1 = new Test();
Test t2 = new Test();
System.out.println(t1.getCount());
System.out.println(t2.getCount());
```

**R:**
```
2
2
```

`count` est **static**, donc partagé. Les deux instances voient la même valeur.

---

**Q41: Ce code compile-t-il?**
```java
public class Test {
    public static void main(String[] args) {
        method();
    }
    
    private static void method() {
        System.out.println("Hello");
    }
}
```

**R:** Oui, compile et affiche `"Hello"`.

Une méthode **private** peut être appelée depuis la **même classe**, même si elle est **static**.

---

**Q42: Que retourne cette méthode?**
```java
public static int mystere(int n) {
    if (n <= 1) return 1;
    return n * mystere(n - 1);
}

System.out.println(mystere(5));
```

**R:** `120` (factoriel de 5)

C'est une méthode **récursive** calculant `5 * 4 * 3 * 2 * 1 = 120`.

---

**Q43: Combien d'objets sont créés?**
```java
public static void method(StringBuilder sb) {
    sb.append(" World");
    sb = new StringBuilder("Nouveau");
    sb.append("!");
}

public static void main(String[] args) {
    StringBuilder texte = new StringBuilder("Hello");
    method(texte);
}
```

**R:** **3 objets**:
1. `new StringBuilder("Hello")` dans main
2. Chaîne interne après `append(" World")` (StringBuilder crée un nouvel objet interne)
3. `new StringBuilder("Nouveau")` dans method

---

**Q44: Ce code compile-t-il?**
```java
public class Test {
    public void method() {
        return;
    }
}
```

**R:** Oui, compile.

Un **return vide** est valide dans une méthode **void** (optionnel, sert juste à sortir prématurément).

---

**Q45: Quelle est la différence entre ces deux?**
```java
public void method1(int[] array) { }
public void method2(int... array) { }
```

**R:** **Syntaxe différente**, mais **fonctionnement identique** à l'intérieur de la méthode.

**Différence à l'appel**:
```java
method1(new int[]{1, 2, 3});  // Doit passer un array explicite
method1(1, 2, 3);             // ERREUR

method2(new int[]{1, 2, 3});  // OK
method2(1, 2, 3);             // OK (plus flexible)
```

---

**Q46: Pourquoi ce code ne compile-t-il pas?**
```java
public class Test {
    public void method() {
        System.out.println("Hello");
        return 42;
    }
}
```

**R:** **Erreur de compilation**: "incompatible types: unexpected return value".

La méthode est **void**, elle ne peut **pas** retourner une valeur.

**Correct**:
```java
public int method() {        // Changer en int
    System.out.println("Hello");
    return 42;
}
```

---

**Q47: Combien de fois "Hello" s'affiche-t-il?**
```java
public class Test {
    {
        System.out.println("Hello");
    }
    
    public Test() {
        System.out.println("Constructor");
    }
}

Test t1 = new Test();
Test t2 = new Test();
```

**R:**
```
Hello
Constructor
Hello
Constructor
```

Le **instance initializer block** s'exécute **à chaque création** d'instance, **avant** le constructor.

---

**Q48: Que retourne cette méthode pour n=5?**
```java
public static int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}
```

**R:** `5`

Calcul de Fibonacci(5):
- fib(5) = fib(4) + fib(3)
- fib(4) = fib(3) + fib(2) = 3 + 1 = 3
- fib(3) = fib(2) + fib(1) = 1 + 1 = 2
- fib(5) = 3 + 2 = **5**

---

**Q49: Cette utilisation de varargs est-elle valide?**
```java
public static int somme(int debut, int... nombres) {
    int total = debut;
    for (int n : nombres) {
        total += n;
    }
    return total;
}

System.out.println(somme(10));
System.out.println(somme(10, 1, 2, 3));
```

**R:** Oui, **valide**. Affiche:
```
10
16
```

Varargs peut être **vide**. `somme(10)` → `debut=10`, `nombres=[]`.

---

**Q50: Quelle méthode est appelée avec null?**
```java
public class Test {
    public void method(String s) {
        System.out.println("String");
    }
    
    public void method(Integer i) {
        System.out.println("Integer");
    }
}

Test t = new Test();
t.method(null);
```

**R:** **Erreur de compilation**: "reference to method is ambiguous".

`null` peut être assigné à **String** ou **Integer**. Le compiler ne peut pas choisir.

**Solution** (cast explicite):
```java
t.method((String)null);   // "String"
t.method((Integer)null);  // "Integer"
```

---

