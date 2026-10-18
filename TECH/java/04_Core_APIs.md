## [**..**](./00_index.md)

# Chapter 4: Core APIs

## Creating and Manipulating Strings

### String Basics

Les **Strings** sont des séquences de caractères **immuables** (non modifiables).

```java
// Création de Strings
String s1 = "Hello";                    // String literal
String s2 = new String("Hello");        // Constructor
String s3 = new String();               // String vide ""
```

**String Pool**: Les String literals sont stockés dans un pool pour économiser la mémoire:

```java
String a = "Hello";
String b = "Hello";
System.out.println(a == b);             // true (même référence dans le pool)

String c = new String("Hello");
String d = new String("Hello");
System.out.println(c == d);             // false (objets différents)
System.out.println(a == c);             // false
System.out.println(a.equals(c));        // true (même contenu)
```

### String Immutability

**Immuable** = ne peut pas être modifié après création:

```java
String s = "Hello";
s.concat(" World");         // Ne modifie PAS s
System.out.println(s);      // "Hello" (inchangé)

s = s.concat(" World");     // Crée un NOUVEAU String et réassigne
System.out.println(s);      // "Hello World"
```

**Pourquoi?** Sécurité, thread-safety, optimisation (String pool).

### Important String Methods

#### length()
```java
String s = "Hello";
System.out.println(s.length());         // 5

String vide = "";
System.out.println(vide.length());      // 0
```

#### charAt()
```java
String s = "Hello";
System.out.println(s.charAt(0));        // 'H'
System.out.println(s.charAt(4));        // 'o'
// System.out.println(s.charAt(5));     // StringIndexOutOfBoundsException
```

#### indexOf() et lastIndexOf()
```java
String s = "Hello World";
System.out.println(s.indexOf('o'));          // 4 (première occurrence)
System.out.println(s.lastIndexOf('o'));      // 7 (dernière occurrence)
System.out.println(s.indexOf("World"));      // 6
System.out.println(s.indexOf('x'));          // -1 (non trouvé)

// Avec position de départ
System.out.println(s.indexOf('o', 5));       // 7 (cherche à partir de l'index 5)
```

#### substring()
```java
String s = "Hello World";
System.out.println(s.substring(6));          // "World" (de l'index 6 à la fin)
System.out.println(s.substring(0, 5));       // "Hello" (de 0 inclus à 5 exclus)
System.out.println(s.substring(6, 11));      // "World"

// Attention
System.out.println(s.substring(6, 6));       // "" (vide, début == fin)
// System.out.println(s.substring(6, 5));    // StringIndexOutOfBoundsException
```

**Important**: Le second paramètre est **exclusif** (comme les arrays).

#### toLowerCase() et toUpperCase()
```java
String s = "Hello World";
System.out.println(s.toLowerCase());         // "hello world"
System.out.println(s.toUpperCase());         // "HELLO WORLD"
System.out.println(s);                       // "Hello World" (inchangé)
```

#### equals() et equalsIgnoreCase()
```java
String s1 = "Hello";
String s2 = "hello";
System.out.println(s1.equals(s2));           // false
System.out.println(s1.equalsIgnoreCase(s2)); // true
```

#### startsWith() et endsWith()
```java
String s = "Hello World";
System.out.println(s.startsWith("Hello"));   // true
System.out.println(s.startsWith("World"));   // false
System.out.println(s.endsWith("World"));     // true
System.out.println(s.endsWith("world"));     // false (case-sensitive)
```

#### contains()
```java
String s = "Hello World";
System.out.println(s.contains("lo Wo"));     // true
System.out.println(s.contains("xyz"));       // false
```

#### replace()
```java
String s = "Hello World";
System.out.println(s.replace('o', 'a'));     // "Hella Warld"
System.out.println(s.replace("World", "Java")); // "Hello Java"
System.out.println(s);                       // "Hello World" (inchangé)
```

#### trim() et strip()
```java
String s = "  Hello World  ";
System.out.println(s.trim());                // "Hello World" (supprime espaces début/fin)
System.out.println(s.strip());               // "Hello World" (Java 11+, meilleur avec Unicode)
System.out.println(s.stripLeading());        // "Hello World  " (supprime début)
System.out.println(s.stripTrailing());       // "  Hello World" (supprime fin)
```

#### String Chaining

Les méthodes String peuvent être **chaînées**:

```java
String result = "  Hello World  "
    .strip()
    .toLowerCase()
    .replace("world", "java");
System.out.println(result);                  // "hello java"
```

### String Concatenation

```java
// Avec +
String s1 = "Hello";
String s2 = "World";
String s3 = s1 + " " + s2;                   // "Hello World"

// Avec concat()
String s4 = s1.concat(" ").concat(s2);       // "Hello World"

// Avec nombres
System.out.println(1 + 2 + "3");             // "33"
System.out.println("1" + 2 + 3);             // "123"
System.out.println("1" + (2 + 3));           // "15"
```

## Using the StringBuilder Class

### StringBuilder Basics

**StringBuilder** est **mutable** (modifiable), utilisé pour construire des Strings efficacement.

```java
StringBuilder sb = new StringBuilder();              // Capacité par défaut: 16
StringBuilder sb2 = new StringBuilder(50);           // Capacité: 50
StringBuilder sb3 = new StringBuilder("Hello");      // Contenu initial + capacité 16
```

**Différence clé**: `StringBuilder` modifie l'objet existant, pas de création de nouveaux objets.

```java
StringBuilder sb = new StringBuilder("Hello");
sb.append(" World");                    // Modifie sb directement
System.out.println(sb);                 // "Hello World"
```

### Common StringBuilder Methods

#### append()
```java
StringBuilder sb = new StringBuilder("Hello");
sb.append(" World");
sb.append('!');
sb.append(123);
System.out.println(sb);                 // "Hello World!123"
```

#### insert()
```java
StringBuilder sb = new StringBuilder("Hello World");
sb.insert(6, "Beautiful ");
System.out.println(sb);                 // "Hello Beautiful World"

sb.insert(0, ">> ");
System.out.println(sb);                 // ">> Hello Beautiful World"
```

#### delete() et deleteCharAt()
```java
StringBuilder sb = new StringBuilder("Hello World");
sb.delete(5, 11);                       // Supprime de l'index 5 (inclus) à 11 (exclus)
System.out.println(sb);                 // "Hello"

sb = new StringBuilder("Hello");
sb.deleteCharAt(0);                     // Supprime le caractère à l'index 0
System.out.println(sb);                 // "ello"
```

#### replace()
```java
StringBuilder sb = new StringBuilder("Hello World");
sb.replace(6, 11, "Java");              // Remplace de l'index 6 à 11 par "Java"
System.out.println(sb);                 // "Hello Java"
```

#### reverse()
```java
StringBuilder sb = new StringBuilder("Hello");
sb.reverse();
System.out.println(sb);                 // "olleH"
```

#### toString()
```java
StringBuilder sb = new StringBuilder("Hello");
String s = sb.toString();               // Convertit en String
System.out.println(s);                  // "Hello"
```

### Method Chaining

```java
StringBuilder sb = new StringBuilder("Hello");
sb.append(" World")
  .append("!")
  .insert(5, " Beautiful")
  .reverse();
System.out.println(sb);                 // "!dlroW lufituaeB olleH"
```

### StringBuilder vs String Performance

```java
// Mauvaise pratique avec String (crée beaucoup d'objets)
String result = "";
for (int i = 0; i < 10000; i++) {
    result += i;                        // Crée 10000 nouveaux Strings!
}

// Bonne pratique avec StringBuilder
StringBuilder sb = new StringBuilder();
for (int i = 0; i < 10000; i++) {
    sb.append(i);                       // Modifie le même objet
}
String result2 = sb.toString();
```

### StringBuilder vs StringBuffer

- **StringBuilder**: **Non thread-safe**, plus rapide
- **StringBuffer**: **Thread-safe**, plus lent (synchronized)

Pour le code mono-thread, utilisez `StringBuilder`.

## Understanding Equality

### Comparing Primitives

```java
int a = 5;
int b = 5;
System.out.println(a == b);             // true (compare les valeurs)
```

### Comparing References

```java
String s1 = new String("Hello");
String s2 = new String("Hello");
String s3 = s1;

System.out.println(s1 == s2);           // false (références différentes)
System.out.println(s1 == s3);           // true (même référence)
System.out.println(s1.equals(s2));      // true (même contenu)
```

### The equals() Method

```java
String s1 = "Hello";
String s2 = "Hello";
String s3 = "HELLO";

System.out.println(s1.equals(s2));              // true
System.out.println(s1.equals(s3));              // false
System.out.println(s1.equalsIgnoreCase(s3));    // true
```

**Important**: Toujours utiliser `equals()` pour comparer le **contenu** des objets, pas `==`.

### String Pool Behavior

```java
String s1 = "Hello";
String s2 = "Hello";
String s3 = new String("Hello");
String s4 = new String("Hello").intern();

System.out.println(s1 == s2);           // true (même référence dans le pool)
System.out.println(s1 == s3);           // false (s3 dans le heap)
System.out.println(s1 == s4);           // true (intern() retourne la référence du pool)
```

**intern()**: Force le String à utiliser le pool.

## Understanding Arrays

### Creating Arrays

```java
// Déclaration et initialisation
int[] nombres = new int[5];                     // [0, 0, 0, 0, 0]
String[] noms = new String[3];                  // [null, null, null]

// Initialisation avec valeurs
int[] scores = {95, 87, 92};                    // Taille: 3
int[] scores2 = new int[]{95, 87, 92};          // Équivalent

// Syntaxe alternative (déconseillée)
int nombres2[] = new int[5];                    // Fonctionne mais moins lisible
```

### Accessing Array Elements

```java
int[] nombres = {10, 20, 30, 40, 50};

System.out.println(nombres[0]);                 // 10 (premier élément)
System.out.println(nombres[4]);                 // 50 (dernier élément)
// System.out.println(nombres[5]);              // ArrayIndexOutOfBoundsException

nombres[2] = 100;                               // Modification
System.out.println(nombres[2]);                 // 100
```

### Array Length

```java
int[] nombres = {10, 20, 30};
System.out.println(nombres.length);             // 3 (propriété, pas method!)

// Parcourir avec for
for (int i = 0; i < nombres.length; i++) {
    System.out.println(nombres[i]);
}
```

**Important**: `length` est une **propriété** pour les arrays, `length()` est une **méthode** pour les Strings.

### Arrays with Reference Types

```java
String[] noms = new String[3];
System.out.println(noms[0]);                    // null (valeur par défaut)

noms[0] = "Alice";
noms[1] = "Bob";
System.out.println(noms[0]);                    // "Alice"
```

### Multidimensional Arrays

```java
// Array 2D
int[][] matrice = new int[3][4];                // 3 lignes, 4 colonnes

// Initialisation
int[][] scores = {
    {95, 87, 92},
    {88, 91, 85},
    {90, 93, 88}
};

// Accès
System.out.println(scores[0][0]);               // 95
System.out.println(scores[1][2]);               // 85

// Parcourir
for (int i = 0; i < scores.length; i++) {
    for (int j = 0; j < scores[i].length; j++) {
        System.out.print(scores[i][j] + " ");
    }
    System.out.println();
}
```

**Arrays asymétriques**:
```java
int[][] asymetrique = {
    {1, 2, 3},
    {4, 5},
    {6, 7, 8, 9}
};
```

### Sorting Arrays

```java
import java.util.Arrays;

int[] nombres = {5, 2, 8, 1, 9};
Arrays.sort(nombres);                           // Tri en place
System.out.println(Arrays.toString(nombres));   // [1, 2, 5, 8, 9]

String[] noms = {"Charlie", "Alice", "Bob"};
Arrays.sort(noms);                              // Tri alphabétique
System.out.println(Arrays.toString(noms));      // [Alice, Bob, Charlie]
```

### Searching Arrays

```java
import java.util.Arrays;

int[] nombres = {1, 2, 5, 8, 9};                // DOIT être trié!
int index = Arrays.binarySearch(nombres, 5);
System.out.println(index);                      // 2 (index de 5)

int index2 = Arrays.binarySearch(nombres, 3);
System.out.println(index2);                     // -3 (valeur négative si non trouvé)
```

**Important**: L'array **doit être trié** avant `binarySearch()`.

### Comparing Arrays

```java
import java.util.Arrays;

int[] a1 = {1, 2, 3};
int[] a2 = {1, 2, 3};
int[] a3 = {1, 2, 4};

System.out.println(a1 == a2);                   // false (références différentes)
System.out.println(Arrays.equals(a1, a2));      // true (même contenu)
System.out.println(Arrays.equals(a1, a3));      // false (contenu différent)
```

### Using Arrays as Methods

```java
import java.util.Arrays;

int[] nombres = {5, 2, 8, 1, 9};

// toString()
System.out.println(Arrays.toString(nombres));   // [5, 2, 8, 1, 9]

// copyOf()
int[] copie = Arrays.copyOf(nombres, nombres.length);
System.out.println(Arrays.toString(copie));     // [5, 2, 8, 1, 9]

// copyOf() avec taille différente
int[] copie2 = Arrays.copyOf(nombres, 3);       // [5, 2, 8]
int[] copie3 = Arrays.copyOf(nombres, 7);       // [5, 2, 8, 1, 9, 0, 0]

// fill()
int[] rempli = new int[5];
Arrays.fill(rempli, 42);
System.out.println(Arrays.toString(rempli));    // [42, 42, 42, 42, 42]
```

### Varargs

```java
public static void afficher(int... nombres) {    // Varargs = array
    System.out.println(Arrays.toString(nombres));
}

afficher(1, 2, 3);                              // [1, 2, 3]
afficher(1);                                    // [1]
afficher();                                     // []

int[] tab = {1, 2, 3};
afficher(tab);                                  // [1, 2, 3]
```

## Calculating with Math APIs

### Common Math Methods

```java
// Valeur absolue
System.out.println(Math.abs(-10));              // 10
System.out.println(Math.abs(10));               // 10

// Min et Max
System.out.println(Math.min(5, 10));            // 5
System.out.println(Math.max(5, 10));            // 10

// Puissance
System.out.println(Math.pow(2, 3));             // 8.0 (2^3)
System.out.println(Math.pow(5, 2));             // 25.0 (5^2)

// Racine carrée
System.out.println(Math.sqrt(16));              // 4.0
System.out.println(Math.sqrt(25));              // 5.0

// Arrondi
System.out.println(Math.round(3.5));            // 4 (arrondi au plus proche)
System.out.println(Math.round(3.4));            // 3
System.out.println(Math.round(-3.5));           // -3

// Ceil et Floor
System.out.println(Math.ceil(3.1));             // 4.0 (arrondi supérieur)
System.out.println(Math.floor(3.9));            // 3.0 (arrondi inférieur)

// Nombre aléatoire
double random = Math.random();                   // [0.0, 1.0)
int randomInt = (int)(Math.random() * 100);      // [0, 100)
```

### Math Constants

```java
System.out.println(Math.PI);                    // 3.141592653589793
System.out.println(Math.E);                     // 2.718281828459045
```

### Generating Random Numbers

```java
// Nombre entre 0 et 9
int dice = (int)(Math.random() * 10);

// Nombre entre 1 et 6 (dé)
int dice2 = (int)(Math.random() * 6) + 1;

// Nombre entre min et max
int min = 10, max = 20;
int random = (int)(Math.random() * (max - min + 1)) + min;
```

## Working with Dates and Times

### LocalDate

```java
import java.time.LocalDate;

// Date actuelle
LocalDate aujourdhui = LocalDate.now();
System.out.println(aujourdhui);                 // 2025-11-03

// Date spécifique
LocalDate date1 = LocalDate.of(2025, 12, 25);   // 2025-12-25
LocalDate date2 = LocalDate.of(2025, 1, 1);

// Extraction
System.out.println(aujourdhui.getYear());       // 2025
System.out.println(aujourdhui.getMonthValue()); // 11
System.out.println(aujourdhui.getDayOfMonth()); // 3
System.out.println(aujourdhui.getDayOfWeek());  // MONDAY
```

### LocalTime

```java
import java.time.LocalTime;

// Heure actuelle
LocalTime maintenant = LocalTime.now();
System.out.println(maintenant);                 // 14:30:45.123456789

// Heure spécifique
LocalTime temps1 = LocalTime.of(14, 30);        // 14:30
LocalTime temps2 = LocalTime.of(14, 30, 45);    // 14:30:45
LocalTime temps3 = LocalTime.of(14, 30, 45, 123); // 14:30:45.000000123

// Extraction
System.out.println(maintenant.getHour());       // 14
System.out.println(maintenant.getMinute());     // 30
System.out.println(maintenant.getSecond());     // 45
```

### LocalDateTime

```java
import java.time.LocalDateTime;

// Date et heure actuelles
LocalDateTime maintenant = LocalDateTime.now();
System.out.println(maintenant);                 // 2025-11-03T14:30:45.123456789

// Date et heure spécifiques
LocalDateTime dt1 = LocalDateTime.of(2025, 12, 25, 14, 30);
LocalDateTime dt2 = LocalDateTime.of(
    LocalDate.of(2025, 12, 25),
    LocalTime.of(14, 30)
);
```

### Manipulating Dates and Times

**Immutabilité**: Les objets date/time sont **immuables**. Les méthodes retournent de nouveaux objets.

```java
LocalDate date = LocalDate.of(2025, 11, 3);

// Ajouter
LocalDate demain = date.plusDays(1);            // 2025-11-04
LocalDate prochainMois = date.plusMonths(1);    // 2025-12-03
LocalDate prochainAnnee = date.plusYears(1);    // 2026-11-03
LocalDate semaines = date.plusWeeks(2);         // 2025-11-17

// Soustraire
LocalDate hier = date.minusDays(1);             // 2025-11-02
LocalDate moisDernier = date.minusMonths(1);    // 2025-10-03

// Original inchangé
System.out.println(date);                       // 2025-11-03
```

**Avec LocalTime**:
```java
LocalTime temps = LocalTime.of(14, 30);

LocalTime plusTard = temps.plusHours(2);        // 16:30
LocalTime plusTot = temps.minusMinutes(15);     // 14:15
```

### Comparing Dates and Times

```java
LocalDate date1 = LocalDate.of(2025, 11, 3);
LocalDate date2 = LocalDate.of(2025, 12, 25);

// isBefore et isAfter
System.out.println(date1.isBefore(date2));      // true
System.out.println(date1.isAfter(date2));       // false

// isEqual
System.out.println(date1.isEqual(date1));       // true

// equals
System.out.println(date1.equals(date2));        // false
```

### Periods

**Period**: Représente une durée en années, mois, jours.

```java
import java.time.Period;

Period periode = Period.ofDays(10);
Period periode2 = Period.ofWeeks(2);            // 14 jours
Period periode3 = Period.ofMonths(3);
Period periode4 = Period.ofYears(1);

// Period complet
Period periode5 = Period.of(1, 2, 3);           // 1 an, 2 mois, 3 jours

// Utilisation
LocalDate date = LocalDate.of(2025, 1, 1);
LocalDate futur = date.plus(Period.ofMonths(6)); // 2025-07-01
```

### Durations

**Duration**: Représente une durée en heures, minutes, secondes.

```java
import java.time.Duration;

Duration duree = Duration.ofHours(2);
Duration duree2 = Duration.ofMinutes(30);
Duration duree3 = Duration.ofSeconds(45);

// Utilisation
LocalTime temps = LocalTime.of(14, 0);
LocalTime apres = temps.plus(Duration.ofMinutes(90)); // 15:30
```

**Différence Period vs Duration**:
- **Period**: Pour les dates (jours, mois, années)
- **Duration**: Pour les heures (heures, minutes, secondes)

```java
LocalDate date = LocalDate.of(2025, 1, 1);
// date.plus(Duration.ofHours(2));              // ERREUR! Period requis pour LocalDate

LocalTime temps = LocalTime.of(14, 0);
// temps.plus(Period.ofDays(1));                // ERREUR! Duration requis pour LocalTime
```

### Formatting Dates and Times

```java
import java.time.format.DateTimeFormatter;

LocalDate date = LocalDate.of(2025, 11, 3);
LocalTime temps = LocalTime.of(14, 30, 45);
LocalDateTime dateTime = LocalDateTime.of(date, temps);

// Format prédéfini
System.out.println(date.format(DateTimeFormatter.ISO_DATE)); // 2025-11-03

// Format personnalisé
DateTimeFormatter formatter = DateTimeFormatter.ofPattern("dd/MM/yyyy");
System.out.println(date.format(formatter));     // 03/11/2025

DateTimeFormatter formatter2 = DateTimeFormatter.ofPattern("HH:mm:ss");
System.out.println(temps.format(formatter2));   // 14:30:45

DateTimeFormatter formatter3 = DateTimeFormatter.ofPattern("dd MMM yyyy HH:mm");
System.out.println(dateTime.format(formatter3)); // 03 nov. 2025 14:30
```

### Parsing Dates and Times

```java
import java.time.format.DateTimeFormatter;

String dateStr = "03/11/2025";
DateTimeFormatter formatter = DateTimeFormatter.ofPattern("dd/MM/yyyy");
LocalDate date = LocalDate.parse(dateStr, formatter);
System.out.println(date);                       // 2025-11-03

String tempsStr = "14:30";
LocalTime temps = LocalTime.parse(tempsStr);    // Format ISO par défaut
System.out.println(temps);                      // 14:30
```

---

## Exemples Pratiques Complets

### Exemple 1: Validation d'email
```java
public class EmailValidator {
    public static boolean estValide(String email) {
        if (email == null || email.isEmpty()) {
            return false;
        }
        
        int atIndex = email.indexOf('@');
        if (atIndex <= 0) {                     // @ doit exister et pas au début
            return false;
        }
        
        int dotIndex = email.lastIndexOf('.');
        if (dotIndex <= atIndex + 1) {          // . doit être après @ avec au moins 1 caractère
            return false;
        }
        
        if (dotIndex == email.length() - 1) {   // . ne doit pas être à la fin
            return false;
        }
        
        return true;
    }
    
    public static void main(String[] args) {
        String[] emails = {
            "test@example.com",     // true
            "@example.com",         // false
            "test@.com",            // false
            "test@example.",        // false
            "test.example.com",     // false
            "test@ex.co"           // true
        };
        
        for (String email : emails) {
            System.out.println(email + ": " + estValide(email));
        }
    }
}
```

### Exemple 2: Constructeur de requêtes SQL
```java
public class QueryBuilder {
    private StringBuilder query;
    
    public QueryBuilder() {
        query = new StringBuilder();
    }
    
    public QueryBuilder select(String... colonnes) {
        query.append("SELECT ");
        if (colonnes.length == 0) {
            query.append("* ");
        } else {
            for (int i = 0; i < colonnes.length; i++) {
                query.append(colonnes[i]);
                if (i < colonnes.length - 1) {
                    query.append(", ");
                }
            }
            query.append(" ");
        }
        return this;
    }
    
    public QueryBuilder from(String table) {
        query.append("FROM ").append(table).append(" ");
        return this;
    }
    
    public QueryBuilder where(String condition) {
        query.append("WHERE ").append(condition).append(" ");
        return this;
    }
    
    public String build() {
        return query.toString().trim();
    }
    
    public static void main(String[] args) {
        String sql = new QueryBuilder()
            .select("nom", "age", "email")
            .from("utilisateurs")
            .where("age > 18")
            .build();
        
        System.out.println(sql);
        // SELECT nom, age, email FROM utilisateurs WHERE age > 18
    }
}
```

### Exemple 3: Calculateur d'âge
```java
import java.time.LocalDate;
import java.time.Period;

public class AgeCalculator {
    public static int calculerAge(LocalDate dateNaissance) {
        return Period.between(dateNaissance, LocalDate.now()).getYears();
    }
    
    public static String calculerAgeDetaille(LocalDate dateNaissance) {
        Period period = Period.between(dateNaissance, LocalDate.now());
        return period.getYears() + " ans, " 
             + period.getMonths() + " mois, " 
             + period.getDays() + " jours";
    }
    
    public static boolean estMajeur(LocalDate dateNaissance) {
        return calculerAge(dateNaissance) >= 18;
    }
    
    public static void main(String[] args) {
        LocalDate dateNaissance = LocalDate.of(2000, 5, 15);
        
        System.out.println("Âge: " + calculerAge(dateNaissance));
        System.out.println("Détaillé: " + calculerAgeDetaille(dateNaissance));
        System.out.println("Majeur: " + estMajeur(dateNaissance));
    }
}
```

### Exemple 4: Analyseur de texte
```java
public class TextAnalyzer {
    public static void analyser(String texte) {
        if (texte == null || texte.isEmpty()) {
            System.out.println("Texte vide");
            return;
        }
        
        // Statistiques de base
        System.out.println("Longueur: " + texte.length());
        System.out.println("Sans espaces: " + texte.replace(" ", "").length());
        
        // Compter les mots
        String[] mots = texte.trim().split("\\s+");
        System.out.println("Nombre de mots: " + mots.length);
        
        // Compter voyelles et consonnes
        int voyelles = 0, consonnes = 0;
        String texteLower = texte.toLowerCase();
        
        for (int i = 0; i < texteLower.length(); i++) {
            char c = texteLower.charAt(i);
            if (Character.isLetter(c)) {
                if ("aeiou".indexOf(c) != -1) {
                    voyelles++;
                } else {
                    consonnes++;
                }
            }
        }
        
        System.out.println("Voyelles: " + voyelles);
        System.out.println("Consonnes: " + consonnes);
        
        // Mot le plus long
        String motLePlusLong = "";
        for (String mot : mots) {
            if (mot.length() > motLePlusLong.length()) {
                motLePlusLong = mot;
            }
        }
        System.out.println("Mot le plus long: " + motLePlusLong);
    }
    
    public static void main(String[] args) {
        String texte = "Java est un langage de programmation orienté objet";
        analyser(texte);
    }
}
```

### Exemple 5: Gestionnaire de tableau de scores
```java
import java.util.Arrays;

public class ScoreManager {
    private int[] scores;
    
    public ScoreManager(int[] scores) {
        this.scores = Arrays.copyOf(scores, scores.length);
    }
    
    public double calculerMoyenne() {
        if (scores.length == 0) return 0;
        
        int somme = 0;
        for (int score : scores) {
            somme += score;
        }
        return (double) somme / scores.length;
    }
    
    public int trouverMax() {
        if (scores.length == 0) {
            throw new IllegalStateException("Tableau vide");
        }
        
        int max = scores[0];
        for (int score : scores) {
            if (score > max) {
                max = score;
            }
        }
        return max;
    }
    
    public int trouverMin() {
        if (scores.length == 0) {
            throw new IllegalStateException("Tableau vide");
        }
        
        int min = scores[0];
        for (int score : scores) {
            if (score < min) {
                min = score;
            }
        }
        return min;
    }
    
    public int[] trouverAuDessusMoyenne() {
        double moyenne = calculerMoyenne();
        int count = 0;
        
        // Compter combien au-dessus
        for (int score : scores) {
            if (score > moyenne) {
                count++;
            }
        }
        
        // Créer array avec ces scores
        int[] resultat = new int[count];
        int index = 0;
        for (int score : scores) {
            if (score > moyenne) {
                resultat[index++] = score;
            }
        }
        
        return resultat;
    }
    
    public void trierScores() {
        Arrays.sort(scores);
    }
    
    public void afficherStatistiques() {
        System.out.println("Scores: " + Arrays.toString(scores));
        System.out.println("Moyenne: " + calculerMoyenne());
        System.out.println("Maximum: " + trouverMax());
        System.out.println("Minimum: " + trouverMin());
        System.out.println("Au-dessus moyenne: " + 
                          Arrays.toString(trouverAuDessusMoyenne()));
    }
    
    public static void main(String[] args) {
        int[] scores = {85, 92, 78, 95, 88, 73, 90};
        ScoreManager manager = new ScoreManager(scores);
        
        manager.afficherStatistiques();
        
        System.out.println("\nAprès tri:");
        manager.trierScores();
        manager.afficherStatistiques();
    }
}
```

### Exemple 6: Calculateur de jours ouvrables
```java
import java.time.LocalDate;
import java.time.DayOfWeek;
import java.time.temporal.ChronoUnit;

public class BusinessDayCalculator {
    
    public static boolean estJourOuvrable(LocalDate date) {
        DayOfWeek jour = date.getDayOfWeek();
        return jour != DayOfWeek.SATURDAY && jour != DayOfWeek.SUNDAY;
    }
    
    public static int compterJoursOuvrables(LocalDate debut, LocalDate fin) {
        if (debut.isAfter(fin)) {
            throw new IllegalArgumentException("Date début après date fin");
        }
        
        int count = 0;
        LocalDate current = debut;
        
        while (!current.isAfter(fin)) {
            if (estJourOuvrable(current)) {
                count++;
            }
            current = current.plusDays(1);
        }
        
        return count;
    }
    
    public static LocalDate ajouterJoursOuvrables(LocalDate date, int joursOuvrables) {
        LocalDate resultat = date;
        int count = 0;
        
        while (count < joursOuvrables) {
            resultat = resultat.plusDays(1);
            if (estJourOuvrable(resultat)) {
                count++;
            }
        }
        
        return resultat;
    }
    
    public static long joursJusquaWeekend(LocalDate date) {
        DayOfWeek jour = date.getDayOfWeek();
        return ChronoUnit.DAYS.between(date, 
            date.plusDays(DayOfWeek.SATURDAY.getValue() - jour.getValue()));
    }
    
    public static void main(String[] args) {
        LocalDate aujourdhui = LocalDate.now();
        LocalDate dansTroisMois = aujourdhui.plusMonths(3);
        
        System.out.println("Aujourd'hui: " + aujourdhui);
        System.out.println("Est jour ouvrable: " + estJourOuvrable(aujourdhui));
        System.out.println("Jours jusqu'au weekend: " + joursJusquaWeekend(aujourdhui));
        
        System.out.println("\nJours ouvrables dans 3 mois: " + 
                          compterJoursOuvrables(aujourdhui, dansTroisMois));
        
        System.out.println("\n10 jours ouvrables après aujourd'hui: " + 
                          ajouterJoursOuvrables(aujourdhui, 10));
    }
}
```

---

## Points Clés du Chapter 4

✅ **String**: Immuable, utiliser `equals()` pour comparaison, String pool pour les literals

✅ **StringBuilder**: Mutable, utilisé pour constructions de Strings, plus performant que concaténation multiple

✅ **String methods**: 
   - `length()`, `charAt()`, `indexOf()`, `substring()`
   - `toLowerCase()`, `toUpperCase()`, `trim()`, `strip()`
   - `replace()`, `startsWith()`, `endsWith()`, `contains()`

✅ **StringBuilder methods**: 
   - `append()`, `insert()`, `delete()`, `deleteCharAt()`
   - `replace()`, `reverse()`, `toString()`

✅ **Arrays**: 
   - Fixed size, indexés de 0 à `length-1`
   - `Arrays.sort()`, `Arrays.binarySearch()` (nécessite tri)
   - `Arrays.equals()`, `Arrays.toString()`, `Arrays.copyOf()`

✅ **Math**: 
   - `abs()`, `min()`, `max()`, `pow()`, `sqrt()`
   - `round()`, `ceil()`, `floor()`, `random()`

✅ **Date/Time API** (java.time):
   - **LocalDate**: dates sans heure
   - **LocalTime**: heures sans date
   - **LocalDateTime**: date et heure
   - **Immutable**: méthodes retournent de nouveaux objets

✅ **Period vs Duration**:
   - **Period**: années, mois, jours (pour LocalDate)
   - **Duration**: heures, minutes, secondes (pour LocalTime)

✅ **Manipulation dates**:
   - `plusDays()`, `plusMonths()`, `plusYears()`
   - `minusDays()`, `minusMonths()`, `minusYears()`
   - `isBefore()`, `isAfter()`, `isEqual()`

✅ **Formatting**: `DateTimeFormatter` avec patterns personnalisés

---

# Questions/Réponses - Chapter 4: Core APIs

## Questions sur les Strings

**Q1: Quelle est la différence entre ces deux créations de String?**
```java
String s1 = "Hello";
String s2 = new String("Hello");
System.out.println(s1 == s2);
System.out.println(s1.equals(s2));
```

**R:** 
```java
s1 == s2         // false
s1.equals(s2)    // true
```

**Explication**:
- `s1`: String literal stocké dans le **String pool**
- `s2`: Nouvel objet créé dans le **heap** avec `new`
- `==` compare les **références** (différentes)
- `equals()` compare le **contenu** (identique)

---

**Q2: Pourquoi ce code affiche-t-il "Hello" et non "Hello World"?**
```java
String s = "Hello";
s.concat(" World");
System.out.println(s);
```

**R:** Affiche `"Hello"` car les Strings sont **immuables**.

`concat()` crée un **nouveau String** mais ne modifie pas `s`. Pour voir "Hello World", il faut:
```java
s = s.concat(" World");  // Réassigner
System.out.println(s);    // "Hello World"
```

---

**Q3: Que s'affiche-t-il?**
```java
String s1 = "Hello";
String s2 = "Hello";
String s3 = "Hel" + "lo";
System.out.println(s1 == s2);
System.out.println(s1 == s3);
```

**R:**
```java
s1 == s2    // true
s1 == s3    // true
```

Le **compiler** optimise `"Hel" + "lo"` en `"Hello"` à la compilation. Tous pointent vers la **même référence** dans le String pool.

---

**Q4: Quelle est la valeur de result?**
```java
String s = "Hello World";
int result = s.indexOf('o');
int result2 = s.lastIndexOf('o');
int result3 = s.indexOf("World");
int result4 = s.indexOf('x');
```

**R:**
```java
result = 4     // Première occurrence de 'o'
result2 = 7    // Dernière occurrence de 'o'
result3 = 6    // Index où commence "World"
result4 = -1   // Non trouvé
```

**Important**: `indexOf()` retourne `-1` si non trouvé, pas une exception.

---

**Q5: Que retourne substring() dans ces cas?**
```java
String s = "Hello World";
String r1 = s.substring(6);
String r2 = s.substring(0, 5);
String r3 = s.substring(6, 11);
String r4 = s.substring(6, 6);
```

**R:**
```java
r1 = "World"     // De l'index 6 à la fin
r2 = "Hello"     // De 0 (inclus) à 5 (exclus)
r3 = "World"     // De 6 (inclus) à 11 (exclus)
r4 = ""          // Vide (début == fin)
```

**Règle**: Le second paramètre est **exclusif**.

---

**Q6: Ce code compile-t-il?**
```java
String s = "Hello";
int length = s.length();

int[] array = {1, 2, 3};
int size = array.length();
```

**R:** Non, **erreur de compilation** sur `array.length()`.

- **String**: `length()` est une **méthode** → parenthèses requises
- **Array**: `length` est une **propriété** → pas de parenthèses

**Correct**:
```java
int length = s.length();      // Méthode
int size = array.length;      // Propriété
```

---

**Q7: Que s'affiche-t-il?**
```java
String s = "  Hello World  ";
System.out.println(s.trim());
System.out.println(s);
```

**R:**
```
Hello World
  Hello World  
```

`trim()` crée un **nouveau String** sans les espaces, mais `s` reste **inchangé** (immutabilité).

---

**Q8: Quelle est la différence entre equals() et equalsIgnoreCase()?**
```java
String s1 = "Hello";
String s2 = "HELLO";
String s3 = "hello";
System.out.println(s1.equals(s2));
System.out.println(s1.equalsIgnoreCase(s2));
```

**R:**
```java
s1.equals(s2)              // false (case-sensitive)
s1.equalsIgnoreCase(s2)    // true (ignore la casse)
```

---

**Q9: Que retourne cette chaîne de méthodes?**
```java
String result = "  HELLO WORLD  "
    .trim()
    .toLowerCase()
    .replace("world", "java");
```

**R:** `"hello java"`

**Ordre d'exécution**:
1. `trim()` → `"HELLO WORLD"`
2. `toLowerCase()` → `"hello world"`
3. `replace()` → `"hello java"`

---

**Q10: Pourquoi ce code cause-t-il une StringIndexOutOfBoundsException?**
```java
String s = "Hello";
char c = s.charAt(5);
```

**R:** `"Hello"` a une longueur de 5, donc les indices valides sont **0 à 4**.

`s.charAt(5)` tente d'accéder à un index **hors limites** → exception.

**Correct**:
```java
char c = s.charAt(4);  // 'o' (dernier caractère)
```

---

## Questions sur StringBuilder

**Q11: Quelle est la différence entre String et StringBuilder?**
```java
// Avec String
String s = "";
for (int i = 0; i < 1000; i++) {
    s += i;
}

// Avec StringBuilder
StringBuilder sb = new StringBuilder();
for (int i = 0; i < 1000; i++) {
    sb.append(i);
}
```

**R:** 
**String**: 
- **Immuable** → chaque `+=` crée un **nouveau objet** (1000 objets créés!)
- **Très inefficace** pour concaténations multiples

**StringBuilder**: 
- **Mutable** → modifie le **même objet**
- **Beaucoup plus performant** pour constructions de Strings

---

**Q12: Que s'affiche-t-il?**
```java
StringBuilder sb = new StringBuilder("Hello");
sb.append(" World");
System.out.println(sb);
```

**R:** `"Hello World"`

Contrairement à String, `append()` **modifie** l'objet existant.

---

**Q13: Que fait ce code?**
```java
StringBuilder sb = new StringBuilder("Hello World");
sb.insert(6, "Beautiful ");
System.out.println(sb);
```

**R:** `"Hello Beautiful World"`

`insert(6, "Beautiful ")` insère à l'**index 6** (avant 'W').

---

**Q14: Que retourne cette séquence?**
```java
StringBuilder sb = new StringBuilder("Hello World");
sb.delete(5, 11);
System.out.println(sb);
```

**R:** `"Hello"`

`delete(5, 11)` supprime de l'index **5 (inclus)** à **11 (exclus)**, soit " World".

---

**Q15: Que s'affiche-t-il?**
```java
StringBuilder sb = new StringBuilder("Java");
sb.reverse();
System.out.println(sb);
System.out.println(sb.reverse());
```

**R:**
```
avaJ
Java
```

`reverse()` **modifie** et **retourne** le StringBuilder. Le second `reverse()` remet dans l'ordre original.

---

**Q16: Ce code compile-t-il?**
```java
StringBuilder sb = new StringBuilder("Hello");
String s = sb;
```

**R:** Non, **erreur de compilation**: "incompatible types: StringBuilder cannot be converted to String".

Pour convertir:
```java
String s = sb.toString();  // OK
```

---

**Q17: Que retourne cette chaîne de méthodes?**
```java
StringBuilder sb = new StringBuilder("Hello");
sb.append(" World")
  .insert(5, " Beautiful")
  .reverse();
String result = sb.toString();
```

**R:** `"dlroW lufituaeB olleH"`

**Ordre d'exécution**:
1. `append(" World")` → `"Hello World"`
2. `insert(5, " Beautiful")` → `"Hello Beautiful World"`
3. `reverse()` → `"dlroW lufituaeB olleH"`

---

**Q18: Quelle est la différence entre StringBuilder et StringBuffer?**

**R:** 
- **StringBuilder**: **Non thread-safe**, plus rapide, pour usage mono-thread
- **StringBuffer**: **Thread-safe** (synchronized), plus lent, pour usage multi-thread

Pour 99% des cas, utiliser `StringBuilder`.

---

## Questions sur les Arrays

**Q19: Que s'affiche-t-il?**
```java
int[] nombres = new int[5];
System.out.println(nombres[0]);

String[] noms = new String[3];
System.out.println(noms[0]);
```

**R:**
```
0
null
```

**Valeurs par défaut**:
- **Primitives numériques**: `0`
- **boolean**: `false`
- **References**: `null`

---

**Q20: Ce code compile-t-il?**
```java
int[] nombres = {1, 2, 3};
nombres = {4, 5, 6};
```

**R:** Non, **erreur de compilation** sur la ligne 2.

La syntaxe `{...}` n'est valide que lors de la **déclaration**.

**Correct**:
```java
nombres = new int[]{4, 5, 6};  // OK
```

---

**Q21: Que s'affiche-t-il?**
```java
int[] a = {1, 2, 3};
int[] b = {1, 2, 3};
System.out.println(a == b);
System.out.println(Arrays.equals(a, b));
```

**R:**
```java
a == b              // false (références différentes)
Arrays.equals(a, b) // true (même contenu)
```

**Toujours** utiliser `Arrays.equals()` pour comparer le contenu des arrays.

---

**Q22: Pourquoi ce code cause-t-il une ArrayIndexOutOfBoundsException?**
```java
int[] nombres = {10, 20, 30};
System.out.println(nombres[3]);
```

**R:** L'array a une **longueur de 3**, donc les indices valides sont **0, 1, 2**.

`nombres[3]` est **hors limites** → exception.

---

**Q23: Que retourne binarySearch() dans ces cas?**
```java
int[] nombres = {1, 3, 5, 7, 9};
int r1 = Arrays.binarySearch(nombres, 5);
int r2 = Arrays.binarySearch(nombres, 4);
int r3 = Arrays.binarySearch(nombres, 0);
```

**R:**
```java
r1 = 2      // Index de 5
r2 = -3     // Non trouvé (négatif)
r3 = -1     // Non trouvé (négatif)
```

**Si non trouvé**: retourne `-(insertion_point) - 1`

---

**Q24: Que se passe-t-il ici?**
```java
int[] nombres = {5, 2, 8, 1, 9};
int index = Arrays.binarySearch(nombres, 5);
System.out.println(index);
```

**R:** Résultat **imprévisible** (souvent négatif).

`binarySearch()` nécessite un array **trié**. Ici, l'array n'est pas trié.

**Correct**:
```java
Arrays.sort(nombres);                    // Trier d'abord
int index = Arrays.binarySearch(nombres, 5);  // Maintenant OK
```

---

**Q25: Que contient le nouveau array?**
```java
int[] original = {1, 2, 3, 4, 5};
int[] copie1 = Arrays.copyOf(original, 3);
int[] copie2 = Arrays.copyOf(original, 7);
```

**R:**
```java
copie1 = [1, 2, 3]           // Tronqué
copie2 = [1, 2, 3, 4, 5, 0, 0]  // Étendu avec 0
```

---

**Q26: Ce code modifie-t-il l'array original?**
```java
int[] nombres = {1, 2, 3, 4, 5};
for (int nombre : nombres) {
    nombre = nombre * 2;
}
System.out.println(Arrays.toString(nombres));
```

**R:** Non, affiche `[1, 2, 3, 4, 5]` (inchangé).

Dans un **enhanced for loop**, la variable est une **copie**. Pour modifier:
```java
for (int i = 0; i < nombres.length; i++) {
    nombres[i] = nombres[i] * 2;
}
```

---

**Q27: Que s'affiche-t-il?**
```java
int[][] matrice = {
    {1, 2, 3},
    {4, 5},
    {6, 7, 8, 9}
};
System.out.println(matrice.length);
System.out.println(matrice[0].length);
System.out.println(matrice[1].length);
```

**R:**
```
3    // Nombre de lignes
3    // Longueur de la première ligne
2    // Longueur de la deuxième ligne
```

Les arrays **asymétriques** sont permis.

---

**Q28: Que fait Arrays.fill()?**
```java
int[] nombres = new int[5];
Arrays.fill(nombres, 42);
System.out.println(Arrays.toString(nombres));
```

**R:** `[42, 42, 42, 42, 42]`

`fill()` remplit **tous les éléments** avec la valeur spécifiée.

---

**Q29: Comment passer un nombre variable d'arguments?**
```java
public static void afficher(int... nombres) {
    System.out.println(Arrays.toString(nombres));
}

afficher(1, 2, 3);
afficher(1);
afficher();
```

**R:**
```
[1, 2, 3]
[1]
[]
```

**Varargs** (`int...`) = array. Peut recevoir 0 ou plus d'arguments.

---

**Q30: Ce code compile-t-il?**
```java
public static void method(String... args, int x) {
    // ...
}
```

**R:** Non, **erreur de compilation**: "varargs parameter must be last".

Le paramètre **varargs** doit **toujours** être le **dernier**.

**Correct**:
```java
public static void method(int x, String... args) {  // OK
    // ...
}
```

---

## Questions sur Math

**Q31: Que retournent ces méthodes?**
```java
System.out.println(Math.abs(-10));
System.out.println(Math.abs(10));
System.out.println(Math.min(5, 10));
System.out.println(Math.max(5, 10));
```

**R:**
```
10
10
5
10
```

---

**Q32: Quelle est la différence entre ces méthodes d'arrondi?**
```java
System.out.println(Math.round(3.5));
System.out.println(Math.ceil(3.1));
System.out.println(Math.floor(3.9));
```

**R:**
```
4      // round: arrondi au plus proche
4.0    // ceil: arrondi supérieur (plafond)
3.0    // floor: arrondi inférieur (plancher)
```

---

**Q33: Que retourne Math.round() pour les nombres négatifs?**
```java
System.out.println(Math.round(3.5));
System.out.println(Math.round(-3.5));
```

**R:**
```
4
-3
```

`round()` arrondit vers le **plus proche entier**. Pour `-3.5`, le plus proche est `-3` (pas `-4`).

---

**Q34: Comment générer un nombre aléatoire entre 1 et 100 (inclus)?**

**R:**
```java
int random = (int)(Math.random() * 100) + 1;
```

**Explication**:
- `Math.random()` retourne `[0.0, 1.0)` (1.0 exclus)
- `* 100` → `[0.0, 100.0)`
- `(int)` → `[0, 99]`
- `+ 1` → `[1, 100]`

---

**Q35: Que retourne Math.pow()?**
```java
System.out.println(Math.pow(2, 3));
System.out.println(Math.pow(5, 2));
System.out.println(Math.pow(4, 0.5));
```

**R:**
```
8.0      // 2^3
25.0     // 5^2
2.0      // racine carrée de 4
```

**Type de retour**: toujours `double`.

---

## Questions sur Date/Time

**Q36: Quelle est la différence entre ces trois classes?**
```java
LocalDate date = LocalDate.now();
LocalTime temps = LocalTime.now();
LocalDateTime dateTime = LocalDateTime.now();
```

**R:**
- **LocalDate**: Seulement la **date** (année, mois, jour), sans heure
- **LocalTime**: Seulement l'**heure** (heure, minute, seconde), sans date
- **LocalDateTime**: Date **ET** heure combinées

---

**Q37: Ce code modifie-t-il la date originale?**
```java
LocalDate date = LocalDate.of(2025, 1, 1);
date.plusDays(10);
System.out.println(date);
```

**R:** Non, affiche `2025-01-01` (inchangé).

Les objets date/time sont **immuables**. `plusDays()` retourne un **nouveau objet**.

**Correct**:
```java
date = date.plusDays(10);  // Réassigner
System.out.println(date);   // 2025-01-11
```

---

**Q38: Comment créer une date spécifique?**
```java
LocalDate date = LocalDate.of(2025, 12, 25);
// ou
LocalDate date2 = LocalDate.of(2025, Month.DECEMBER, 25);
```

**R:** Les deux sont valides. Le mois peut être spécifié par:
- **int** (1-12)
- **Month enum** (JANUARY, FEBRUARY, etc.)

---

**Q39: Que retourne cette comparaison?**
```java
LocalDate date1 = LocalDate.of(2025, 1, 1);
LocalDate date2 = LocalDate.of(2025, 12, 31);
System.out.println(date1.isBefore(date2));
System.out.println(date1.isAfter(date2));
```

**R:**
```
true
false
```

---

**Q40: Quelle est la différence entre Period et Duration?**
```java
Period period = Period.ofDays(10);
Duration duration = Duration.ofDays(10);

LocalDate date = LocalDate.of(2025, 1, 1);
LocalTime temps = LocalTime.of(14, 0);

date.plus(period);      // ?
date.plus(duration);    // ?
temps.plus(period);     // ?
temps.plus(duration);   // ?
```

**R:**
```java
date.plus(period);      // OK
date.plus(duration);    // ERREUR! UnsupportedTemporalTypeException
temps.plus(period);     // ERREUR! UnsupportedTemporalTypeException
temps.plus(duration);   // OK
```

**Règle**:
- **Period**: Pour **LocalDate** (jours, mois, années)
- **Duration**: Pour **LocalTime** et **LocalDateTime** (heures, minutes, secondes)

---

**Q41: Que retourne Period.between()?**
```java
LocalDate debut = LocalDate.of(2025, 1, 1);
LocalDate fin = LocalDate.of(2025, 6, 15);
Period period = Period.between(debut, fin);
System.out.println(period.getMonths());
System.out.println(period.getDays());
```

**R:**
```
5     // 5 mois
14    // 14 jours (du 1er au 15)
```

`Period.between()` calcule la différence en **années, mois, et jours séparément**.

---

**Q42: Comment formater une date?**
```java
LocalDate date = LocalDate.of(2025, 11, 3);
DateTimeFormatter formatter = DateTimeFormatter.ofPattern("dd/MM/yyyy");
String formatted = date.format(formatter);
```

**R:** `"03/11/2025"`

**Patterns courants**:
- `dd`: jour (01-31)
- `MM`: mois (01-12)
- `yyyy`: année (4 chiffres)
- `HH`: heure 24h
- `mm`: minute
- `ss`: seconde

---

**Q43: Comment parser un String en LocalDate?**
```java
String dateStr = "25/12/2025";
DateTimeFormatter formatter = DateTimeFormatter.ofPattern("dd/MM/yyyy");
LocalDate date = LocalDate.parse(dateStr, formatter);
```

**R:** Crée un `LocalDate` représentant le 25 décembre 2025.

Si le format ne correspond pas, lance **DateTimeParseException**.

---

**Q44: Que s'affiche-t-il?**
```java
LocalDate date = LocalDate.of(2025, 1, 1);
System.out.println(date.getDayOfWeek());
System.out.println(date.getMonth());
System.out.println(date.getMonthValue());
```

**R:**
```
WEDNESDAY    // DayOfWeek enum
JANUARY      // Month enum
1            // int (1-12)
```

---

**Q45: Comment calculer l'âge d'une personne?**
```java
LocalDate dateNaissance = LocalDate.of(2000, 5, 15);
LocalDate aujourdhui = LocalDate.now();
Period age = Period.between(dateNaissance, aujourdhui);
System.out.println(age.getYears());
```

**R:** Retourne le nombre d'années entre la date de naissance et aujourd'hui.

---

**Q46: Ce code compile-t-il?**
```java
LocalDate date = LocalDate.of(2025, 2, 30);
```

**R:** Non, lance **DateTimeException** à l'exécution.

Février n'a jamais 30 jours. Java valide les dates.

---

**Q47: Quelle est la différence entre ces deux méthodes?**
```java
LocalDate date = LocalDate.of(2025, 1, 31);
LocalDate m1 = date.plusMonths(1);
LocalDate m2 = date.plusDays(31);
```

**R:**
```java
m1 = 2025-02-28   // Février n'a que 28 jours, ajusté automatiquement
m2 = 2025-03-03   // 31 jours après le 31 janvier
```

Java ajuste automatiquement les dates invalides.

---

**Q48: Comment obtenir le premier et dernier jour du mois?**
```java
LocalDate date = LocalDate.of(2025, 11, 15);
LocalDate debut = date.withDayOfMonth(1);
LocalDate fin = date.withDayOfMonth(date.lengthOfMonth());
```

**R:**
```java
debut = 2025-11-01
fin = 2025-11-30
```

---

**Q49: Que retourne ChronoUnit.DAYS.between()?**
```java
import java.time.temporal.ChronoUnit;

LocalDate date1 = LocalDate.of(2025, 1, 1);
LocalDate date2 = LocalDate.of(2025, 1, 10);
long jours = ChronoUnit.DAYS.between(date1, date2);
```

**R:** `9` (nombre de jours entre les deux dates)

---

**Q50: Ce code est-il correct?**
```java
LocalTime temps = LocalTime.of(14, 30);
temps = temps.plusHours(25);
System.out.println(temps);
```

**R:** Oui, affiche `15:30`.

LocalTime **wraparound** automatiquement: 14:30 + 25h = 15:30 (le lendemain, mais LocalTime ne stocke pas la date).

---