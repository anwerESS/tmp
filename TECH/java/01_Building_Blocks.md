## [**..**](./00_index.md)

# Chapter 1: Building Blocks

## Learning about the Environment

Java nécessite deux composants essentiels:
- **JDK (Java Development Kit)**: pour compiler et développer
- **JRE (Java Runtime Environment)**: pour exécuter les applications

Le processus de compilation et d'exécution:
```java
// Compilation
javac MonProgramme.java  // Génère MonProgramme.class (bytecode)

// Exécution
java MonProgramme  // Lance la JVM qui exécute le bytecode
```

## Understanding the Class Structure

Une classe Java contient des **fields** (variables) et des **methods** (fonctions).

```java
// Structure basique d'une classe
public class Animal {
    // Field (variable d'instance)
    private String name;
    
    // Method
    public String getName() {
        return name;
    }
    
    public void setName(String newName) {
        name = newName;
    }
}
```

**Important**: 
- Le nom du fichier doit correspondre au nom de la classe publique
- Une seule classe `public` par fichier

## Writing a main() Method

Le **main method** est le point d'entrée d'un programme Java.

```java
public class Application {
    // Signature obligatoire du main method
    public static void main(String[] args) {
        System.out.println("Hello World!");
    }
}
```

**Variations acceptées**:
```java
public static void main(String args[])     // OK
public static void main(String... args)    // OK (varargs)
static public void main(String[] args)     // OK (ordre des modifiers)
```

**Arguments de ligne de commande**:
```java
public static void main(String[] args) {
    System.out.println(args[0]);  // Premier argument
    System.out.println(args[1]);  // Deuxième argument
}
// Exécution: java Application bonjour monde
// Output: bonjour
//         monde
```

## Understanding Package Declarations and Imports

Les **packages** organisent les classes. Les **imports** permettent d'utiliser des classes d'autres packages.

```java
// Déclaration du package (doit être en première ligne)
package com.monapp.modele;

// Imports
import java.util.ArrayList;        // Import d'une classe spécifique
import java.util.*;                // Import de toutes les classes du package

public class Client {
    private ArrayList<String> emails;  // Pas besoin de java.util.ArrayList
}
```

**Wildcards**:
```java
import java.util.*;      // Importe toutes les classes de java.util
import java.nio.file.*;  // Importe toutes les classes de java.nio.file
// MAIS ne pas importer les sous-packages
```

**Imports automatiques**:
- `java.lang.*` est toujours importé automatiquement
- Pas besoin d'importer les classes du même package

**Conflits de noms**:
```java
import java.util.Date;
import java.sql.Date;  // ERREUR de compilation

// Solution: utiliser le fully qualified name
import java.util.Date;
public class Test {
    Date utilDate = new Date();
    java.sql.Date sqlDate = new java.sql.Date(0);
}
```

## Creating Objects

Création d'objets avec l'**operator** `new`:

```java
// Syntaxe de base
ClassName objectName = new ClassName();

// Exemples
String texte = new String("Hello");
ArrayList<Integer> nombres = new ArrayList<>();
Scanner scanner = new Scanner(System.in);
```

**Constructors**: méthodes spéciales pour initialiser les objets:
```java
public class Personne {
    private String nom;
    private int age;
    
    // Constructor par défaut
    public Personne() {
        nom = "Inconnu";
        age = 0;
    }
    
    // Constructor avec paramètres
    public Personne(String nom, int age) {
        this.nom = nom;
        this.age = age;
    }
}

// Utilisation
Personne p1 = new Personne();                    // Utilise le constructor par défaut
Personne p2 = new Personne("Alice", 25);         // Utilise le constructor paramétré
```

**Ordre d'exécution**:
1. Les **fields** sont initialisés
2. Le **constructor** est exécuté

## Understanding Data Types

Java a deux catégories de types:

### Primitive Types (8 types)

```java
// Types entiers
byte b = 127;           // 8 bits  (-128 à 127)
short s = 32767;        // 16 bits (-32,768 à 32,767)
int i = 2147483647;     // 32 bits (-2^31 à 2^31-1)
long l = 9223372036L;   // 64 bits (suffixe L requis)

// Types décimaux
float f = 3.14f;        // 32 bits (suffixe f requis)
double d = 3.14159;     // 64 bits (type par défaut)

// Autres
char c = 'A';           // 16 bits (caractère Unicode)
boolean bool = true;    // true ou false
```

**Underscores dans les nombres** (pour la lisibilité):
```java
int million = 1_000_000;
long carte = 1234_5678_9012_3456L;
double pi = 3.14_15_92;
```

### Reference Types

Toutes les classes, interfaces, arrays, enums:
```java
String texte = "Hello";           // Classe String
Integer nombre = 100;             // Wrapper class
int[] tableau = {1, 2, 3};       // Array
ArrayList<String> liste = new ArrayList<>();  // Collection
```

**Différence clé**:
- **Primitives**: stockent la valeur directement
- **References**: stockent l'adresse mémoire de l'objet

```java
int a = 10;
int b = a;     // Copie la valeur
b = 20;        // a vaut toujours 10

StringBuilder sb1 = new StringBuilder("Hello");
StringBuilder sb2 = sb1;  // Copie la référence
sb2.append(" World");     // sb1 vaut aussi "Hello World"
```

## Declaring Variables

**Règles de nommage**:
- Commence par une lettre, `$`, ou `_`
- Peut contenir des chiffres après le premier caractère
- **Case-sensitive**: `variable` ≠ `Variable`
- Ne peut pas être un **reserved word** (mot-clé Java)

```java
// Valides
int nombre;
String _texte;
double $prix;
boolean estValide;
int nombre1, nombre2;  // Déclaration multiple

// Invalides
int 2nombres;      // Commence par un chiffre
String class;      // Reserved word
double mon-age;    // Contient un tiret
```

**Conventions de nommage**:
- **Variables/methods**: camelCase (`monNomDeVariable`)
- **Classes**: PascalCase (`MaClasseExemple`)
- **Constants**: SNAKE_CASE_MAJUSCULE (`MAX_VALUE`)

```java
public class CompteBancaire {
    private static final double TAUX_INTERET = 0.05;  // Constante
    private double soldeActuel;                        // Variable d'instance
    
    public void deposer(double montant) {              // Method
        soldeActuel += montant;
    }
}
```

## Initializing Variables

### Variables locales

**Doivent être initialisées** avant utilisation:
```java
public void calculer() {
    int x;
    // System.out.println(x);  // ERREUR: variable not initialized
    
    x = 10;
    System.out.println(x);     // OK
}
```

### Instance variables

Ont des **valeurs par défaut**:
```java
public class Defaults {
    boolean bool;      // false
    byte b;            // 0
    short s;           // 0
    int i;             // 0
    long l;            // 0L
    float f;           // 0.0f
    double d;          // 0.0
    char c;            // '\u0000' (null character)
    String str;        // null (toute référence)
    
    public void afficher() {
        System.out.println(bool);  // false
        System.out.println(str);   // null
    }
}
```

### Initializers

```java
public class Initialisation {
    // Initialisation inline
    private int compteur = 0;
    
    // Instance initializer block
    {
        compteur++;
        System.out.println("Instance initializer");
    }
    
    // Static initializer block
    static {
        System.out.println("Static initializer");
    }
    
    // Constructor
    public Initialisation() {
        compteur++;
        System.out.println("Constructor");
    }
}
```

**Ordre d'exécution**:
1. Static initializers (une seule fois)
2. Instance initializers
3. Constructor

## Managing Variable Scope

### Types de variables

```java
public class Scope {
    private static int classVariable = 1;     // Class/static variable
    private int instanceVariable = 2;         // Instance variable
    
    public void method(int parametre) {       // Method parameter
        int localVariable = 3;                // Local variable
        
        System.out.println(classVariable);    // Accessible
        System.out.println(instanceVariable); // Accessible
        System.out.println(parametre);        // Accessible
        System.out.println(localVariable);    // Accessible
    }
}
```

**Durée de vie**:
- **Class variables**: existent pendant toute l'exécution du programme
- **Instance variables**: existent tant que l'objet existe
- **Local variables**: existent uniquement dans le bloc où elles sont déclarées
- **Method parameters**: existent pendant l'exécution de la méthode

```java
public void exemple() {
    int x = 10;
    
    if (x > 5) {
        int y = 20;
        System.out.println(x);  // OK
        System.out.println(y);  // OK
    }
    
    // System.out.println(y);  // ERREUR: y n'existe plus
}
```

## Destroying Objects

Java utilise le **Garbage Collector** (GC) pour libérer la mémoire automatiquement.

### Eligibilité au Garbage Collection

Un objet est éligible quand il n'y a plus de **référence** vers lui:

```java
// Cas 1: Réassignation
String s1 = new String("Hello");
String s2 = new String("World");
s1 = s2;  // L'objet "Hello" est éligible au GC

// Cas 2: Référence null
StringBuilder sb = new StringBuilder("Test");
sb = null;  // L'objet StringBuilder est éligible au GC

// Cas 3: Sortie de scope
public void method() {
    String local = new String("Local");
}  // local est éligible au GC ici
```

**Method finalize()** (deprecated):
```java
public class Resource {
    @Override
    protected void finalize() throws Throwable {
        System.out.println("Cleaning up");
        super.finalize();
    }
}
```

**Forcer le GC** (non garanti):
```java
System.gc();          // Suggère au GC de s'exécuter
Runtime.getRuntime().gc();  // Équivalent
```

**Important**: 
- Ne pas compter sur le GC pour libérer des ressources critiques
- Utiliser **try-with-resources** pour les ressources (voir Chapter 11)
- Le GC gère uniquement la mémoire heap, pas les ressources externes

---

**Points clés du Chapter 1**:
- ✅ Java compile en bytecode, exécuté par la JVM
- ✅ `main()` method = point d'entrée obligatoire
- ✅ 8 primitive types + reference types
- ✅ Variables locales doivent être initialisées
- ✅ Instance variables ont des valeurs par défaut
- ✅ Garbage Collector gère automatiquement la mémoire

---

# Questions/Réponses - Chapter 1: Building Blocks

## Questions Fondamentales

**Q1: Quelle est la différence entre le JDK et le JRE?**

**R:** Le JRE (Java Runtime Environment) contient uniquement la JVM et les libraries nécessaires pour exécuter des applications Java. Le JDK (Java Development Kit) inclut le JRE plus les outils de développement comme le compiler `javac`, le debugger, etc. Pour développer, on a besoin du JDK. Pour simplement exécuter une application, le JRE suffit.

---

**Q2: Quelle est la signature correcte d'un main method? Pourquoi doit-il être static?**

**R:** 
```java
public static void main(String[] args)
```
Il doit être:
- `public`: accessible de l'extérieur
- `static`: pour que la JVM puisse l'appeler sans créer une instance de la classe
- `void`: ne retourne rien
- Accepter un array de String comme paramètre

Le `static` est crucial car quand la JVM démarre, aucun objet n'existe encore. Elle doit pouvoir invoquer la méthode directement sur la classe.

---

**Q3: Combien de classes public peut-on avoir dans un fichier .java?**

**R:** Une seule classe `public` par fichier, et le nom du fichier doit correspondre exactement au nom de cette classe. On peut avoir plusieurs classes non-public dans le même fichier, mais c'est rarement une bonne pratique.

```java
// Fichier: Animal.java
public class Animal { }  // OK - nom correspond au fichier
class Helper { }         // OK - pas public
// public class Autre { }   // ERREUR - deuxième classe public
```

---

## Questions sur les Packages et Imports

**Q4: Quelle est la différence entre ces deux imports?**
```java
import java.util.ArrayList;
import java.util.*;
```

**R:** Le premier importe uniquement la classe `ArrayList`. Le second utilise un wildcard `*` et importe toutes les classes du package `java.util`, mais pas les sous-packages. Le wildcard n'affecte pas les performances car l'import se fait à la compilation, pas à l'exécution. Cependant, l'import spécifique est préféré pour la lisibilité et éviter les conflits de noms.

---

**Q5: Pourquoi ce code compile-t-il sans import?**
```java
public class Test {
    public static void main(String[] args) {
        String texte = "Hello";
        System.out.println(texte);
    }
}
```

**R:** Parce que le package `java.lang` est automatiquement importé dans tous les fichiers Java. `String` et `System` font partie de `java.lang`, donc pas besoin d'import explicite.

---

**Q6: Comment résoudre ce conflit d'imports?**
```java
import java.util.Date;
import java.sql.Date;
```

**R:** On ne peut pas importer deux classes avec le même nom. La solution est d'importer celle qu'on utilise le plus et utiliser le fully qualified name pour l'autre:

```java
import java.util.Date;

public class Test {
    Date utilDate = new Date();                    // java.util.Date
    java.sql.Date sqlDate = new java.sql.Date(0);  // fully qualified name
}
```

---

## Questions sur les Types de Données

**Q7: Quelle est la différence entre un primitive type et un reference type?**

**R:** 
- **Primitive types**: stockent directement la valeur en mémoire (stack). Il y en a 8: byte, short, int, long, float, double, char, boolean.
- **Reference types**: stockent une adresse mémoire pointant vers l'objet dans le heap. Incluent toutes les classes, interfaces, arrays.

```java
int a = 10;           // Primitive - valeur 10 stockée directement
Integer b = 10;       // Reference - pointe vers un objet Integer dans le heap

int x = 5;
int y = x;            // Copie la valeur
y = 10;               // x vaut toujours 5

StringBuilder sb1 = new StringBuilder("Hello");
StringBuilder sb2 = sb1;  // Copie la référence
sb2.append(" World");     // sb1 vaut aussi "Hello World" !
```

---

**Q8: Pourquoi ce code ne compile-t-il pas?**
```java
long nombre = 3000000000;
```

**R:** Par défaut, les literals numériques entiers sont de type `int`. 3000000000 dépasse la capacité d'un `int` (max 2,147,483,647). Il faut ajouter le suffixe `L`:

```java
long nombre = 3000000000L;  // OK
```

---

**Q9: Quels sont les noms de variables valides?**
```java
int _valeur;      // ?
int $prix;        // ?
int 2nombres;     // ?
int mon-age;      // ?
int class;        // ?
```

**R:** 
```java
int _valeur;      // ✅ OK (underscore autorisé)
int $prix;        // ✅ OK ($ autorisé)
int 2nombres;     // ❌ ERREUR (ne peut pas commencer par un chiffre)
int mon-age;      // ❌ ERREUR (tiret non autorisé)
int class;        // ❌ ERREUR (reserved word)
```

---

## Questions sur l'Initialisation

**Q10: Pourquoi ce code ne compile-t-il pas?**
```java
public void calculer() {
    int resultat;
    System.out.println(resultat);
}
```

**R:** Les variables locales n'ont pas de valeur par défaut. Elles doivent être explicitement initialisées avant utilisation. Ce code génère une erreur "variable resultat might not have been initialized".

```java
public void calculer() {
    int resultat = 0;  // OK
    System.out.println(resultat);
}
```

---

**Q11: Quelles sont les valeurs par défaut des instance variables?**

**R:**
```java
public class Defaults {
    boolean bool;      // false
    byte b;            // 0
    short s;           // 0
    int i;             // 0
    long l;            // 0L
    float f;           // 0.0f
    double d;          // 0.0
    char c;            // '\u0000' (null character)
    String str;        // null (toute référence = null)
}
```

Les primitives ont des valeurs numériques/false, les references valent `null`.

---

**Q12: Dans quel ordre s'exécutent ces blocs?**
```java
public class Init {
    static { System.out.println("1"); }
    { System.out.println("2"); }
    public Init() { System.out.println("3"); }
    { System.out.println("4"); }
    static { System.out.println("5"); }
}
```

**R:** 
1. Static initializers (une seule fois, dans l'ordre): "1" puis "5"
2. Instance initializers (à chaque création d'objet, dans l'ordre): "2" puis "4"
3. Constructor: "3"

Output lors de `new Init()`: 1, 5, 2, 4, 3

---

## Questions sur le Scope

**Q13: Pourquoi ce code ne compile-t-il pas?**
```java
public void method() {
    if (true) {
        int x = 10;
    }
    System.out.println(x);
}
```

**R:** La variable `x` est déclarée dans le scope du bloc `if`. Elle n'existe plus en dehors de ce bloc. Pour corriger:

```java
public void method() {
    int x;
    if (true) {
        x = 10;
    }
    System.out.println(x);  // OK
}
```

---

**Q14: Quelle est la différence entre ces variables?**
```java
public class Example {
    private static int a;    // ?
    private int b;           // ?
    
    public void method(int c) {  // ?
        int d;               // ?
    }
}
```

**R:**
- `a`: **class variable** (static) - partagée par toutes les instances, existe pendant toute l'exécution
- `b`: **instance variable** - chaque objet a sa propre copie, existe tant que l'objet existe
- `c`: **method parameter** - existe pendant l'exécution de la méthode
- `d`: **local variable** - existe uniquement dans le scope de la méthode

---

## Questions sur le Garbage Collection

**Q15: Combien d'objets sont éligibles au Garbage Collection après cette ligne?**
```java
String s1 = new String("Hello");
String s2 = new String("World");
String s3 = s1;
s1 = s2;
s2 = null;
// Ici ?
```

**R:** 1 objet est éligible (l'objet "Hello").
- `s1` pointe maintenant vers "World"
- `s2` vaut `null`
- `s3` pointe toujours vers "Hello"
- "World" a encore une référence via `s1`

Donc seul l'objet "Hello" n'a qu'une référence (via `s3`). 

Attendez, je me corrige: **0 objet** est éligible car "Hello" est toujours référencé par `s3` et "World" est toujours référencé par `s1`.

---

**Q16: Que fait System.gc()?**

**R:** `System.gc()` suggère à la JVM d'exécuter le Garbage Collector, mais ce n'est qu'une suggestion. La JVM peut choisir de l'ignorer. On ne peut jamais forcer le GC avec certitude. C'est généralement une mauvaise pratique d'appeler `System.gc()` car la JVM gère mieux la mémoire elle-même.

---

## Questions Pièges

**Q17: Ce code compile-t-il? Si oui, que fait-il?**
```java
public class Test {
    public static void main(String[] args) {
        int i = 10;
        int i = 20;
        System.out.println(i);
    }
}
```

**R:** Non, ce code ne compile pas. On ne peut pas déclarer deux variables avec le même nom dans le même scope. Erreur: "variable i is already defined".

---

**Q18: Quelle est la valeur de x?**
```java
int x = 10;
{
    int x = 20;
}
System.out.println(x);
```

**R:** Ce code ne compile pas. On ne peut pas redéclarer une variable avec le même nom dans un scope imbriqué si elle existe déjà dans le scope parent. C'est différent des langages comme JavaScript.

---

**Q19: Que s'affiche-t-il?**
```java
public class Test {
    static int count;
    
    public static void main(String[] args) {
        System.out.println(count);
    }
}
```

**R:** `0`. Même si `count` est une static variable, elle a quand même une valeur par défaut (0 pour les int). Les variables locales n'ont pas de valeur par défaut, mais les class et instance variables en ont.

---

**Q20: Pourquoi utilise-t-on des underscores dans les nombres?**
```java
int million = 1_000_000;
```

**R:** C'est purement pour la lisibilité du code. Le compiler ignore les underscores. C'est introduit en Java 7. On peut les placer n'importe où sauf au début, à la fin, ou autour du point décimal:

```java
int valide = 1_000_000;        // ✅ OK
double valide2 = 3.14_15_92;   // ✅ OK
int invalide = _1000;          // ❌ ERREUR
double invalide2 = 3._14;      // ❌ ERREUR
```

---

Ces questions couvrent les concepts essentiels du Chapter 1. Tu veux que je continue avec les questions pour le **Chapter 2: Operators**?