## [**..**](./00_index.md)

# Chapter 11: Exceptions and Localization

## 1. Understanding Exceptions

**Exception** est un événement qui perturbe le flux normal d'exécution du programme.

### Hiérarchie des Exceptions

```java
// Throwable (racine)
//   ├── Error (erreurs système, non récupérables)
//   │     └── OutOfMemoryError, StackOverflowError...
//   └── Exception (erreurs applicatives, récupérables)
//         ├── RuntimeException (unchecked)
//         │     ├── NullPointerException
//         │     ├── ArrayIndexOutOfBoundsException
//         │     ├── IllegalArgumentException
//         │     └── ArithmeticException
//         └── Checked Exceptions
//               ├── IOException
//               ├── SQLException
//               └── FileNotFoundException
```

### Types d'Exceptions

```java
// CHECKED EXCEPTION - doit être déclarée ou catchée
public void readFile() throws IOException {
    FileReader reader = new FileReader("file.txt");
}

// UNCHECKED EXCEPTION (RuntimeException) - optionnel
public void divide(int a, int b) {
    int result = a / b; // Peut lancer ArithmeticException
}

// ERROR - ne pas catcher
public void infiniteRecursion() {
    infiniteRecursion(); // StackOverflowError
}
```

---

## 2. Recognizing Exception Classes

### RuntimeException (Unchecked)

```java
// ArithmeticException - division par zéro
int result = 10 / 0; // ArithmeticException

// NullPointerException - accès sur null
String s = null;
s.length(); // NullPointerException

// ArrayIndexOutOfBoundsException - index invalide
int[] array = {1, 2, 3};
int value = array[5]; // ArrayIndexOutOfBoundsException

// ClassCastException - cast invalide
Object obj = "Hello";
Integer num = (Integer) obj; // ClassCastException

// IllegalArgumentException - argument invalide
public void setAge(int age) {
    if (age < 0) {
        throw new IllegalArgumentException("Age cannot be negative");
    }
}

// NumberFormatException - parsing invalide
int num = Integer.parseInt("abc"); // NumberFormatException

// IllegalStateException - état invalide
List<String> list = List.of("a", "b");
list.add("c"); // UnsupportedOperationException (subclass)
```

### Checked Exceptions

```java
// IOException - erreur I/O
try {
    FileReader reader = new FileReader("file.txt");
} catch (IOException e) {
    e.printStackTrace();
}

// FileNotFoundException - fichier non trouvé
try {
    FileInputStream fis = new FileInputStream("missing.txt");
} catch (FileNotFoundException e) {
    System.out.println("File not found");
}

// SQLException - erreur base de données
try {
    Connection conn = DriverManager.getConnection(url);
} catch (SQLException e) {
    e.printStackTrace();
}

// ParseException - erreur de parsing
try {
    SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
    Date date = sdf.parse("invalid-date");
} catch (ParseException e) {
    e.printStackTrace();
}
```

### Errors (Ne pas catcher)

```java
// OutOfMemoryError - mémoire insuffisante
List<byte[]> list = new ArrayList<>();
while (true) {
    list.add(new byte[1024 * 1024]); // OutOfMemoryError
}

// StackOverflowError - récursion infinie
public void recurse() {
    recurse(); // StackOverflowError
}

// ExceptionInInitializerError - erreur dans static block
class MyClass {
    static {
        throw new RuntimeException(); // ExceptionInInitializerError
    }
}
```

---

## 3. Handling Exceptions

### Try-Catch

```java
// Try-catch basique
try {
    int result = 10 / 0;
} catch (ArithmeticException e) {
    System.out.println("Cannot divide by zero");
}

// Multi-catch (même handler pour plusieurs exceptions)
try {
    String s = null;
    s.length();
} catch (NullPointerException | IllegalArgumentException e) {
    System.out.println("Error: " + e.getMessage());
}

// Multi-catch différents
try {
    // Code risqué
} catch (FileNotFoundException e) {
    System.out.println("File not found");
} catch (IOException e) {
    System.out.println("IO error");
} catch (Exception e) {
    System.out.println("General error");
}

// IMPORTANT: L'ordre compte!
// Plus spécifique en premier, plus général en dernier
try {
    // Code
} catch (FileNotFoundException e) {  // Spécifique
    // ...
} catch (IOException e) {            // Plus général
    // ...
} catch (Exception e) {              // Le plus général
    // ...
}

// ERREUR de compilation
try {
    // Code
} catch (Exception e) {              // Trop général
    // ...
} catch (IOException e) {            // ERREUR: jamais atteint (unreachable)
    // ...
}
```

### Finally Block

```java
// Finally - toujours exécuté
try {
    // Code risqué
} catch (Exception e) {
    // Gestion erreur
} finally {
    // Toujours exécuté (cleanup)
    System.out.println("Cleanup");
}

// Finally exécuté même avec return
public int getValue() {
    try {
        return 1;
    } finally {
        System.out.println("Finally");  // Exécuté avant le return
    }
}

// Finally exécuté même si exception non catchée
try {
    throw new RuntimeException();
} finally {
    System.out.println("Finally");  // Exécuté avant propagation
}

// Exemple pratique: fermeture de ressources
FileReader reader = null;
try {
    reader = new FileReader("file.txt");
    // Lecture du fichier
} catch (IOException e) {
    e.printStackTrace();
} finally {
    if (reader != null) {
        try {
            reader.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
```

### Throwing Exceptions

```java
// Throw - lancer une exception
public void setAge(int age) {
    if (age < 0) {
        throw new IllegalArgumentException("Age cannot be negative");
    }
    this.age = age;
}

// Throws - déclarer une exception
public void readFile(String path) throws IOException {
    FileReader reader = new FileReader(path);
    // Lecture
}

// Créer une exception custom
class InvalidAgeException extends Exception {
    public InvalidAgeException(String message) {
        super(message);
    }
}

public void setAge(int age) throws InvalidAgeException {
    if (age < 0 || age > 150) {
        throw new InvalidAgeException("Age must be between 0 and 150");
    }
    this.age = age;
}

// Rethrow - relancer une exception
try {
    readFile("file.txt");
} catch (IOException e) {
    System.out.println("Error reading file");
    throw e;  // Relance l'exception
}

// Wrap exception - encapsuler dans une autre
try {
    readFile("file.txt");
} catch (IOException e) {
    throw new RuntimeException("Failed to process file", e);
}
```

### printStackTrace et getMessage

```java
try {
    int result = 10 / 0;
} catch (ArithmeticException e) {
    // getMessage - message court
    System.out.println(e.getMessage());  // "/ by zero"
    
    // toString - nom de classe + message
    System.out.println(e.toString());    // "java.lang.ArithmeticException: / by zero"
    
    // printStackTrace - stack trace complet
    e.printStackTrace();
    // java.lang.ArithmeticException: / by zero
    //     at Main.main(Main.java:5)
    
    // getCause - exception cause
    Throwable cause = e.getCause();
}
```

---

## 4. Automating Resource Management

### Try-with-Resources

```java
// AVANT Java 7 - gestion manuelle
FileReader reader = null;
try {
    reader = new FileReader("file.txt");
    // Utilisation
} catch (IOException e) {
    e.printStackTrace();
} finally {
    if (reader != null) {
        try {
            reader.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

// APRÈS Java 7 - try-with-resources
try (FileReader reader = new FileReader("file.txt")) {
    // Utilisation - reader.close() appelé automatiquement
} catch (IOException e) {
    e.printStackTrace();
}

// Plusieurs ressources
try (FileReader reader = new FileReader("input.txt");
     FileWriter writer = new FileWriter("output.txt")) {
    // Utilisation - fermeture automatique dans l'ordre inverse
} catch (IOException e) {
    e.printStackTrace();
}

// IMPORTANT: Ressources fermées dans l'ordre INVERSE de déclaration
try (Resource1 r1 = new Resource1();   // Fermé en 3ème
     Resource2 r2 = new Resource2();   // Fermé en 2ème
     Resource3 r3 = new Resource3()) { // Fermé en 1er
    // Utilisation
}
```

### AutoCloseable Interface

```java
// Pour utiliser try-with-resources, la classe doit implémenter AutoCloseable
public class MyResource implements AutoCloseable {
    public MyResource() {
        System.out.println("Resource opened");
    }
    
    public void doSomething() {
        System.out.println("Doing something");
    }
    
    @Override
    public void close() {
        System.out.println("Resource closed");
    }
}

// Utilisation
try (MyResource resource = new MyResource()) {
    resource.doSomething();
} // close() appelé automatiquement
// Output:
// Resource opened
// Doing something
// Resource closed

// Closeable vs AutoCloseable
// Closeable extends AutoCloseable mais throw IOException
// AutoCloseable throw Exception (plus général)

public class MyCloseable implements Closeable {
    @Override
    public void close() throws IOException {
        // Implémentation
    }
}

public class MyAutoCloseable implements AutoCloseable {
    @Override
    public void close() throws Exception {
        // Implémentation
    }
}
```

### Suppressed Exceptions

```java
// Si exception dans try ET dans close(), l'exception du try est lancée
// L'exception du close() est "suppressed"
class Resource implements AutoCloseable {
    @Override
    public void close() throws Exception {
        throw new Exception("Close exception");
    }
}

try (Resource r = new Resource()) {
    throw new Exception("Try exception");
} catch (Exception e) {
    System.out.println(e.getMessage());              // "Try exception"
    Throwable[] suppressed = e.getSuppressed();
    System.out.println(suppressed[0].getMessage());  // "Close exception"
}

// Exemple pratique
try (FileReader reader = new FileReader("file.txt")) {
    throw new IOException("Read error");
} catch (IOException e) {
    System.out.println("Main: " + e.getMessage());
    for (Throwable suppressed : e.getSuppressed()) {
        System.out.println("Suppressed: " + suppressed.getMessage());
    }
}
```

---

## 5. Formatting Values

### NumberFormat

```java
import java.text.NumberFormat;
import java.util.Locale;

// Format de nombres généraux
NumberFormat nf = NumberFormat.getNumberInstance();
System.out.println(nf.format(1234567.89));  // "1,234,567.89" (US)

// Format de devise
NumberFormat currency = NumberFormat.getCurrencyInstance();
System.out.println(currency.format(123.45));  // "$123.45" (US)

// Format de pourcentage
NumberFormat percent = NumberFormat.getPercentInstance();
System.out.println(percent.format(0.75));  // "75%"

// Avec Locale spécifique
NumberFormat frenchCurrency = NumberFormat.getCurrencyInstance(Locale.FRANCE);
System.out.println(frenchCurrency.format(123.45));  // "123,45 €"

NumberFormat germanNumber = NumberFormat.getNumberInstance(Locale.GERMANY);
System.out.println(germanNumber.format(1234567.89));  // "1.234.567,89"

// Parsing (String → Number)
try {
    NumberFormat nf = NumberFormat.getInstance();
    Number number = nf.parse("1,234.56");
    System.out.println(number.doubleValue());  // 1234.56
} catch (ParseException e) {
    e.printStackTrace();
}

// Contrôle des décimales
NumberFormat nf = NumberFormat.getNumberInstance();
nf.setMinimumFractionDigits(2);  // Minimum 2 décimales
nf.setMaximumFractionDigits(2);  // Maximum 2 décimales
System.out.println(nf.format(123.4));     // "123.40"
System.out.println(nf.format(123.456));   // "123.46" (arrondi)
```

### DateFormat

```java
import java.text.DateFormat;
import java.util.Date;
import java.util.Locale;

Date now = new Date();

// Format court
DateFormat shortDate = DateFormat.getDateInstance(DateFormat.SHORT);
System.out.println(shortDate.format(now));  // "11/6/25" (US)

// Format moyen
DateFormat mediumDate = DateFormat.getDateInstance(DateFormat.MEDIUM);
System.out.println(mediumDate.format(now));  // "Nov 6, 2025" (US)

// Format long
DateFormat longDate = DateFormat.getDateInstance(DateFormat.LONG);
System.out.println(longDate.format(now));  // "November 6, 2025" (US)

// Format complet
DateFormat fullDate = DateFormat.getDateInstance(DateFormat.FULL);
System.out.println(fullDate.format(now));  // "Thursday, November 6, 2025" (US)

// Format date + heure
DateFormat dateTime = DateFormat.getDateTimeInstance(
    DateFormat.SHORT, 
    DateFormat.SHORT
);
System.out.println(dateTime.format(now));  // "11/6/25, 10:30 AM"

// Avec Locale
DateFormat frenchDate = DateFormat.getDateInstance(
    DateFormat.LONG, 
    Locale.FRANCE
);
System.out.println(frenchDate.format(now));  // "6 novembre 2025"

// Parsing
try {
    DateFormat df = DateFormat.getDateInstance(DateFormat.SHORT);
    Date date = df.parse("11/6/25");
    System.out.println(date);
} catch (ParseException e) {
    e.printStackTrace();
}
```

### SimpleDateFormat

```java
import java.text.SimpleDateFormat;
import java.util.Date;

Date now = new Date();

// Format personnalisé
SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
System.out.println(sdf.format(now));  // "2025-11-06 10:30:45"

// Patterns courants
// y = année, M = mois, d = jour
// H = heure (0-23), h = heure (1-12), m = minute, s = seconde
// E = jour de la semaine, a = AM/PM

SimpleDateFormat format1 = new SimpleDateFormat("dd/MM/yyyy");
System.out.println(format1.format(now));  // "06/11/2025"

SimpleDateFormat format2 = new SimpleDateFormat("EEEE, MMMM d, yyyy");
System.out.println(format2.format(now));  // "Thursday, November 6, 2025"

SimpleDateFormat format3 = new SimpleDateFormat("hh:mm a");
System.out.println(format3.format(now));  // "10:30 AM"

// Parsing
try {
    SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
    Date date = sdf.parse("2025-11-06");
    System.out.println(date);
} catch (ParseException e) {
    e.printStackTrace();
}

// ATTENTION: SimpleDateFormat n'est PAS thread-safe!
// Préférer DateTimeFormatter (Java 8+) pour multi-threading
```

### DateTimeFormatter (Java 8+)

```java
import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

LocalDateTime now = LocalDateTime.now();

// Formats prédéfinis
DateTimeFormatter iso = DateTimeFormatter.ISO_DATE_TIME;
System.out.println(now.format(iso));  // "2025-11-06T10:30:45"

DateTimeFormatter basic = DateTimeFormatter.BASIC_ISO_DATE;
System.out.println(LocalDate.now().format(basic));  // "20251106"

// Format personnalisé
DateTimeFormatter custom = DateTimeFormatter.ofPattern("dd/MM/yyyy HH:mm");
System.out.println(now.format(custom));  // "06/11/2025 10:30"

// Avec Locale
DateTimeFormatter french = DateTimeFormatter.ofPattern(
    "EEEE d MMMM yyyy", 
    Locale.FRANCE
);
System.out.println(LocalDate.now().format(french));  // "jeudi 6 novembre 2025"

// Parsing
LocalDate date = LocalDate.parse("2025-11-06");
LocalDate customDate = LocalDate.parse("06/11/2025", 
    DateTimeFormatter.ofPattern("dd/MM/yyyy"));

// Thread-safe!
DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd");
// Peut être utilisé par plusieurs threads
```

---

## 6. Supporting Internationalization and Localization

### Locale

```java
import java.util.Locale;

// Créer des Locale
Locale us = Locale.US;
Locale france = Locale.FRANCE;
Locale canada = Locale.CANADA;

// Constructeurs
Locale locale1 = new Locale("fr");              // Langue seulement
Locale locale2 = new Locale("fr", "FR");        // Langue + pays
Locale locale3 = new Locale("fr", "FR", "MAC"); // Langue + pays + variante

// Builder (Java 7+)
Locale locale4 = new Locale.Builder()
    .setLanguage("fr")
    .setRegion("FR")
    .build();

// Propriétés
System.out.println(us.getLanguage());      // "en"
System.out.println(us.getCountry());       // "US"
System.out.println(us.getDisplayName());   // "English (United States)"

System.out.println(france.getLanguage());      // "fr"
System.out.println(france.getCountry());       // "FR"
System.out.println(france.getDisplayName());   // "French (France)"

// Display name dans une autre locale
System.out.println(france.getDisplayName(Locale.US));  // "French (France)"
System.out.println(france.getDisplayName(france));     // "français (France)"

// Locale par défaut
Locale defaultLocale = Locale.getDefault();
Locale.setDefault(Locale.FRANCE);

// Locales disponibles
Locale[] locales = Locale.getAvailableLocales();
```

### Format avec Locale

```java
import java.text.NumberFormat;
import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.util.Locale;

double amount = 123456.78;
LocalDate date = LocalDate.of(2025, 11, 6);

// US Locale
NumberFormat usFormat = NumberFormat.getCurrencyInstance(Locale.US);
System.out.println(usFormat.format(amount));  // "$123,456.78"

DateTimeFormatter usDate = DateTimeFormatter.ofPattern("MMMM d, yyyy", Locale.US);
System.out.println(date.format(usDate));  // "November 6, 2025"

// French Locale
NumberFormat frFormat = NumberFormat.getCurrencyInstance(Locale.FRANCE);
System.out.println(frFormat.format(amount));  // "123 456,78 €"

DateTimeFormatter frDate = DateTimeFormatter.ofPattern("d MMMM yyyy", Locale.FRANCE);
System.out.println(date.format(frDate));  // "6 novembre 2025"

// German Locale
NumberFormat deFormat = NumberFormat.getCurrencyInstance(Locale.GERMANY);
System.out.println(deFormat.format(amount));  // "123.456,78 €"

DateTimeFormatter deDate = DateTimeFormatter.ofPattern("d. MMMM yyyy", Locale.GERMANY);
System.out.println(date.format(deDate));  // "6. November 2025"
```

---

## 7. Loading Properties with Resource Bundles

### Properties File

```properties
# messages.properties (default)
greeting=Hello
farewell=Goodbye

# messages_fr.properties (French)
greeting=Bonjour
farewell=Au revoir

# messages_fr_CA.properties (French Canadian)
greeting=Bonjour
farewell=Salut

# messages_en_US.properties (US English)
greeting=Hi
farewell=See you later
```

### ResourceBundle

```java
import java.util.Locale;
import java.util.ResourceBundle;

// Charger le bundle par défaut
ResourceBundle bundle = ResourceBundle.getBundle("messages");
System.out.println(bundle.getString("greeting"));  // "Hello"

// Charger avec Locale spécifique
ResourceBundle frBundle = ResourceBundle.getBundle("messages", Locale.FRANCE);
System.out.println(frBundle.getString("greeting"));  // "Bonjour"

ResourceBundle caBundle = ResourceBundle.getBundle("messages", Locale.CANADA_FRENCH);
System.out.println(caBundle.getString("greeting"));  // "Bonjour"

// Fallback mechanism
// Si messages_fr_CA.properties n'existe pas, cherche:
// 1. messages_fr_CA.properties
// 2. messages_fr.properties
// 3. messages.properties (default)

// Exemple complet
public class I18nExample {
    public static void main(String[] args) {
        Locale[] locales = {Locale.US, Locale.FRANCE, Locale.CANADA_FRENCH};
        
        for (Locale locale : locales) {
            ResourceBundle bundle = ResourceBundle.getBundle("messages", locale);
            System.out.println(locale + ": " + bundle.getString("greeting"));
        }
    }
}
// Output:
// en_US: Hi
// fr_FR: Bonjour
// fr_CA: Bonjour
```

### Properties avec paramètres

```properties
# messages.properties
welcome=Welcome {0}!
order=Your order of {0} items costs {1}

# messages_fr.properties
welcome=Bienvenue {0}!
order=Votre commande de {0} articles coûte {1}
```

```java
import java.text.MessageFormat;
import java.util.Locale;
import java.util.ResourceBundle;

ResourceBundle bundle = ResourceBundle.getBundle("messages", Locale.US);

// Avec MessageFormat
String pattern = bundle.getString("welcome");
String message = MessageFormat.format(pattern, "Alice");
System.out.println(message);  // "Welcome Alice!"

// Exemple avec plusieurs paramètres
String orderPattern = bundle.getString("order");
String orderMessage = MessageFormat.format(orderPattern, 3, "$45.99");
System.out.println(orderMessage);  // "Your order of 3 items costs $45.99"

// Version française
ResourceBundle frBundle = ResourceBundle.getBundle("messages", Locale.FRANCE);
String frPattern = frBundle.getString("welcome");
String frMessage = MessageFormat.format(frPattern, "Alice");
System.out.println(frMessage);  // "Bienvenue Alice!"
```

### ListResourceBundle (Programmatique)

```java
import java.util.ListResourceBundle;

// Alternative aux fichiers .properties
public class MyResources extends ListResourceBundle {
    @Override
    protected Object[][] getContents() {
        return new Object[][] {
            {"greeting", "Hello"},
            {"farewell", "Goodbye"},
            {"count", 42}  // Peut stocker des objets, pas juste des Strings
        };
    }
}

public class MyResources_fr extends ListResourceBundle {
    @Override
    protected Object[][] getContents() {
        return new Object[][] {
            {"greeting", "Bonjour"},
            {"farewell", "Au revoir"},
            {"count", 42}
        };
    }
}

// Utilisation
ResourceBundle bundle = ResourceBundle.getBundle("MyResources", Locale.FRANCE);
System.out.println(bundle.getString("greeting"));  // "Bonjour"
System.out.println(bundle.getObject("count"));     // 42
```

### Ordre de recherche de ResourceBundle

```java
// Recherche pour Locale("fr", "CA")
// 1. MyResources_fr_CA.class
// 2. MyResources_fr_CA.properties
// 3. MyResources_fr.class
// 4. MyResources_fr.properties
// 5. MyResources.class
// 6. MyResources.properties
// Si rien trouvé: MissingResourceException

// Exemple
Locale locale = new Locale("fr", "CA");
try {
    ResourceBundle bundle = ResourceBundle.getBundle("MyResources", locale);
    System.out.println(bundle.getString("greeting"));
} catch (MissingResourceException e) {
    System.out.println("Bundle not found");
}
```

### Handling Missing Keys

```java
ResourceBundle bundle = ResourceBundle.getBundle("messages");

// Vérifier si clé existe
if (bundle.containsKey("greeting")) {
    System.out.println(bundle.getString("greeting"));
}

// Gestion d'erreur
try {
    String value = bundle.getString("nonexistent");
} catch (MissingResourceException e) {
    System.out.println("Key not found: " + e.getKey());
}

// Avec valeur par défaut
public String getMessage(ResourceBundle bundle, String key, String defaultValue) {
    try {
        return bundle.getString(key);
    } catch (MissingResourceException e) {
        return defaultValue;
    }
}
```

---

## Points Clés

### Exceptions
- **Checked exceptions** doivent être déclarées (`throws`) ou catchées
- **Unchecked exceptions** (RuntimeException) sont optionnelles
- **Multi-catch** avec `|` pour plusieurs exceptions
- **Finally** toujours exécuté, même avec return ou exception
- **Try-with-resources** ferme automatiquement les ressources AutoCloseable

### Formatting
- **NumberFormat**: nombres, devises, pourcentages
- **DateFormat**: dates courtes, moyennes, longues
- **SimpleDateFormat**: format personnalisé (pas thread-safe)
- **DateTimeFormatter**: format Java 8+ (thread-safe)

### Internationalization
- **Locale**: langue + pays (ex: fr_FR)
- **ResourceBundle**: charger des messages localisés
- **Fallback mechanism**: cherche locale spécifique → générale → défaut
- **MessageFormat**: paramétrer les messages avec {0}, {1}...

---

# Questions/Réponses - Chapter 11: Exceptions and Localization

## Questions sur Understanding Exceptions

**Q1: Quelle est la différence entre Error et Exception?**

**R:**

| **Error** | **Exception** |
|-----------|---------------|
| Problèmes système graves | Problèmes applicatifs |
| Non récupérable | Récupérable |
| Ne doit PAS être catché | Doit/peut être catché |
| Ex: OutOfMemoryError | Ex: IOException |

```java
// Error - NE PAS CATCHER
try {
    byte[] array = new byte[Integer.MAX_VALUE];
} catch (OutOfMemoryError e) {  // MAUVAISE PRATIQUE
    // Ne peut rien faire
}

// Exception - CATCHER
try {
    FileReader reader = new FileReader("file.txt");
} catch (IOException e) {  // BONNE PRATIQUE
    System.out.println("File not found");
}
```

---

**Q2: Quelle est la différence entre checked et unchecked exceptions?**

**R:**

**Checked Exception (compilateur vérifie)**:
- Doit être déclarée avec `throws` OU catchée
- Hérite de `Exception` (mais pas de `RuntimeException`)
- Ex: IOException, SQLException, ParseException

**Unchecked Exception (compilateur ne vérifie pas)**:
- Pas obligé de déclarer ou catcher
- Hérite de `RuntimeException`
- Ex: NullPointerException, IllegalArgumentException

```java
// CHECKED - doit être déclarée ou catchée
public void readFile() throws IOException {  // throws obligatoire
    FileReader reader = new FileReader("file.txt");
}

// OU
public void readFile() {
    try {
        FileReader reader = new FileReader("file.txt");
    } catch (IOException e) {  // catch obligatoire
        e.printStackTrace();
    }
}

// UNCHECKED - optionnel
public void divide(int a, int b) {  // Pas de throws nécessaire
    int result = a / b;  // Peut lancer ArithmeticException
}
```

---

**Q3: Ce code compile-t-il?**
```java
public void method1() {
    method2();
}

public void method2() throws IOException {
    throw new IOException();
}
```

**R:** Non, **erreur de compilation**: "unhandled exception IOException".

`method1()` appelle `method2()` qui throw une **checked exception**, donc `method1()` doit:
- Soit la catcher
- Soit la déclarer avec `throws`

**Correct:**
```java
// Option 1: déclarer
public void method1() throws IOException {
    method2();
}

// Option 2: catcher
public void method1() {
    try {
        method2();
    } catch (IOException e) {
        e.printStackTrace();
    }
}
```

---

**Q4: Que se passe-t-il?**
```java
String s = null;
try {
    s.length();
} catch (NullPointerException e) {
    System.out.println("Null");
} catch (Exception e) {
    System.out.println("Exception");
}
```

**R:** Affiche `"Null"`.

`NullPointerException` est catché par le **premier catch** qui correspond.

---

**Q5: Ce code compile-t-il?**
```java
try {
    // Code
} catch (Exception e) {
    System.out.println("General");
} catch (IOException e) {
    System.out.println("IO");
}
```

**R:** Non, **erreur de compilation**: "exception IOException has already been caught".

`IOException` extends `Exception`, donc le 2ème catch est **unreachable** (jamais atteint).

**Règle**: Ordre du **plus spécifique** au **plus général**.

**Correct:**
```java
try {
    // Code
} catch (IOException e) {        // Plus spécifique
    System.out.println("IO");
} catch (Exception e) {          // Plus général
    System.out.println("General");
}
```

---

**Q6: Que fait multi-catch avec |?**
```java
try {
    // Code risqué
} catch (IOException | SQLException e) {
    System.out.println(e.getMessage());
}
```

**R:** Catch **plusieurs types d'exceptions** avec le **même handler**.

**IMPORTANT**: La variable `e` est **implicitement final** (ne peut pas être réassignée).

```java
try {
    // Code
} catch (IOException | SQLException e) {
    // e = new IOException();  // ERREUR: e est final
    System.out.println(e.getMessage());  // OK
}
```

**ATTENTION**: Les exceptions ne doivent pas avoir de relation d'héritage.

```java
// ERREUR de compilation
try {
    // Code
} catch (Exception | IOException e) {  // IOException extends Exception
    // Erreur: "The exception IOException is already caught"
}
```

---

**Q7: Quelle est la différence entre throw et throws?**

**R:**
- **throw**: **lance** une exception (dans le corps de la méthode)
- **throws**: **déclare** qu'une méthode peut lancer une exception (dans la signature)

```java
// throws - déclaration
public void readFile(String path) throws IOException {
    if (path == null) {
        throw new IllegalArgumentException("Path is null");  // throw
    }
    FileReader reader = new FileReader(path);  // Peut throw IOException
}

// Exemple complet
public void setAge(int age) throws InvalidAgeException {  // throws
    if (age < 0) {
        throw new InvalidAgeException("Age cannot be negative");  // throw
    }
    this.age = age;
}
```

---

## Questions sur Handling Exceptions

**Q8: Que s'affiche-t-il?**
```java
try {
    System.out.println("1");
    throw new RuntimeException();
} catch (RuntimeException e) {
    System.out.println("2");
} finally {
    System.out.println("3");
}
System.out.println("4");
```

**R:**
```
1
2
3
4
```

**Finally** est toujours exécuté après try/catch, puis le code continue normalement.

---

**Q9: Que s'affiche-t-il?**
```java
try {
    System.out.println("1");
    throw new RuntimeException();
} finally {
    System.out.println("2");
}
System.out.println("3");
```

**R:**
```
1
2
Exception in thread "main" java.lang.RuntimeException
```

`"3"` n'est **jamais affiché** car l'exception se propage après le finally.

---

**Q10: Que retourne cette méthode?**
```java
public int getValue() {
    try {
        return 1;
    } finally {
        return 2;
    }
}
```

**R:** `2`

Le **return dans finally** écrase le return du try. **MAUVAISE PRATIQUE** (ne jamais return dans finally).

---

**Q11: Que s'affiche-t-il?**
```java
try {
    System.out.println("1");
    System.exit(0);
} finally {
    System.out.println("2");
}
```

**R:** `"1"` seulement.

`System.exit()` termine immédiatement la JVM. **C'est le SEUL cas** où finally n'est pas exécuté.

---

**Q12: Ce code compile-t-il?**
```java
try {
    // Code
} finally {
    System.out.println("Finally");
}
```

**R:** Oui, **try-finally sans catch** est valide.

Utile quand on veut du cleanup mais pas de gestion d'erreur.

```java
// Exemple pratique
Lock lock = new ReentrantLock();
lock.lock();
try {
    // Code critique
} finally {
    lock.unlock();  // Toujours libérer le lock
}
```

---

**Q13: Que se passe-t-il avec plusieurs catch?**
```java
try {
    String s = null;
    s.length();
} catch (NullPointerException e) {
    System.out.println("NPE");
    throw e;
} catch (Exception e) {
    System.out.println("Exception");
}
```

**R:** Affiche `"NPE"` puis lance l'exception.

Le premier catch match, affiche, puis **relance** l'exception avec `throw e`. Le 2ème catch n'est **jamais exécuté**.

---

**Q14: Quelle est la différence entre getMessage(), toString() et printStackTrace()?**
```java
try {
    int result = 10 / 0;
} catch (ArithmeticException e) {
    System.out.println(e.getMessage());
    System.out.println(e.toString());
    e.printStackTrace();
}
```

**R:**
```java
e.getMessage():
"/ by zero"

e.toString():
"java.lang.ArithmeticException: / by zero"

e.printStackTrace():
java.lang.ArithmeticException: / by zero
    at Main.main(Main.java:5)
    at ...
```

- **getMessage()**: message court
- **toString()**: nom de classe + message
- **printStackTrace()**: stack trace complet (debug)

---

**Q15: Comment créer une exception custom?**
```java
// Exception custom
public class InvalidAgeException extends Exception {
    public InvalidAgeException() {
        super();
    }
    
    public InvalidAgeException(String message) {
        super(message);
    }
    
    public InvalidAgeException(String message, Throwable cause) {
        super(message, cause);
    }
}

// Utilisation
public void setAge(int age) throws InvalidAgeException {
    if (age < 0 || age > 150) {
        throw new InvalidAgeException("Age must be between 0 and 150: " + age);
    }
    this.age = age;
}

// Appel
try {
    person.setAge(-5);
} catch (InvalidAgeException e) {
    System.out.println(e.getMessage());
}
```

---

**Q16: Quelle est la différence entre Exception et RuntimeException pour une custom exception?**

**R:**

```java
// Checked exception - extends Exception
public class CheckedException extends Exception {
    // Doit être déclarée ou catchée
}

public void method() throws CheckedException {  // throws obligatoire
    throw new CheckedException();
}

// Unchecked exception - extends RuntimeException
public class UncheckedException extends RuntimeException {
    // Optionnel
}

public void method() {  // Pas de throws nécessaire
    throw new UncheckedException();
}
```

**Règle**: 
- Utiliser **Exception** pour erreurs récupérables (fichier manquant, connexion échouée)
- Utiliser **RuntimeException** pour erreurs de programmation (argument null, index invalide)

---

## Questions sur Try-with-Resources

**Q17: Ce code compile-t-il?**
```java
try (FileReader reader = new FileReader("file.txt")) {
    // Lecture
}
```

**R:** Oui, **pas besoin de catch** avec try-with-resources (mais pas besoin de finally non plus).

L'exception peut se propager, et `reader.close()` est quand même appelé.

---

**Q18: Dans quel ordre sont fermées les ressources?**
```java
try (Resource1 r1 = new Resource1();
     Resource2 r2 = new Resource2();
     Resource3 r3 = new Resource3()) {
    // Utilisation
}
```

**R:** Ordre **INVERSE** de déclaration:
1. `r3.close()`
2. `r2.close()`
3. `r1.close()`

Comme une pile (LIFO - Last In First Out).

---

**Q19: Que se passe-t-il si close() lance une exception?**
```java
class MyResource implements AutoCloseable {
    @Override
    public void close() throws Exception {
        throw new Exception("Close failed");
    }
}

try (MyResource r = new MyResource()) {
    System.out.println("Using resource");
}
```

**R:** L'exception de `close()` est **propagée**.

```
Using resource
Exception in thread "main" java.lang.Exception: Close failed
```

---

**Q20: Que se passe-t-il si le try ET close() lancent des exceptions?**
```java
class MyResource implements AutoCloseable {
    @Override
    public void close() throws Exception {
        throw new Exception("Close exception");
    }
}

try (MyResource r = new MyResource()) {
    throw new Exception("Try exception");
}
```

**R:** L'exception du **try** est lancée, celle du **close()** est **suppressed**.

```java
try (MyResource r = new MyResource()) {
    throw new Exception("Try exception");
} catch (Exception e) {
    System.out.println("Main: " + e.getMessage());
    // "Main: Try exception"
    
    Throwable[] suppressed = e.getSuppressed();
    System.out.println("Suppressed: " + suppressed[0].getMessage());
    // "Suppressed: Close exception"
}
```

---

**Q21: Quelle interface doit implémenter une classe pour try-with-resources?**

**R:** **AutoCloseable** (ou **Closeable** qui extends AutoCloseable).

```java
// AutoCloseable - plus général
public class MyResource implements AutoCloseable {
    @Override
    public void close() throws Exception {  // Peut throw Exception
        // Cleanup
    }
}

// Closeable - pour I/O
public class MyCloseable implements Closeable {
    @Override
    public void close() throws IOException {  // Throw seulement IOException
        // Cleanup
    }
}

// Utilisation
try (MyResource r = new MyResource()) {
    // Utilisation
}  // close() appelé automatiquement
```

---

**Q22: Ce code compile-t-il?**
```java
FileReader reader = new FileReader("file.txt");
try (reader) {
    // Lecture
}
```

**R:** Oui, **depuis Java 9**, on peut utiliser des variables **effectively final** déclarées avant le try.

```java
// Java 9+
FileReader reader = new FileReader("file.txt");
try (reader) {
    // Lecture
} // reader.close() appelé

// Équivalent Java 7+
try (FileReader reader = new FileReader("file.txt")) {
    // Lecture
}
```

---

**Q23: Peut-on ajouter un catch/finally avec try-with-resources?**
```java
try (FileReader reader = new FileReader("file.txt")) {
    // Lecture
} catch (IOException e) {
    System.out.println("Error");
} finally {
    System.out.println("Done");
}
```

**R:** Oui, **totalement valide**.

Ordre d'exécution:
1. Try block
2. `reader.close()` automatique
3. Catch (si exception)
4. Finally

---

**Q24: Ce code a-t-il un problème?**
```java
try (FileReader reader = new FileReader("file.txt")) {
    reader = new FileReader("other.txt");  // Réassigner
}
```

**R:** Non, **erreur de compilation**: "cannot assign a value to final variable reader".

La variable dans try-with-resources est **implicitement final**.

---

## Questions sur Formatting

**Q25: Que fait NumberFormat.getInstance()?**
```java
NumberFormat nf = NumberFormat.getInstance();
System.out.println(nf.format(1234567.89));
```

**R:** Formate selon la **Locale par défaut**.

```java
// US Locale
Locale.setDefault(Locale.US);
NumberFormat nf = NumberFormat.getInstance();
System.out.println(nf.format(1234567.89));  // "1,234,567.89"

// French Locale
Locale.setDefault(Locale.FRANCE);
NumberFormat nf2 = NumberFormat.getInstance();
System.out.println(nf2.format(1234567.89));  // "1 234 567,89"
```

---

**Q26: Quelle est la différence entre getNumberInstance(), getCurrencyInstance() et getPercentInstance()?**

**R:**

```java
double value = 123.45;

// getNumberInstance - nombre général
NumberFormat number = NumberFormat.getNumberInstance();
System.out.println(number.format(value));  // "123.45"

// getCurrencyInstance - devise
NumberFormat currency = NumberFormat.getCurrencyInstance();
System.out.println(currency.format(value));  // "$123.45" (US)

// getPercentInstance - pourcentage
NumberFormat percent = NumberFormat.getPercentInstance();
System.out.println(percent.format(0.75));  // "75%"

// Avec Locale
NumberFormat frCurrency = NumberFormat.getCurrencyInstance(Locale.FRANCE);
System.out.println(frCurrency.format(value));  // "123,45 €"
```

---

**Q27: Comment parser une String en Number avec NumberFormat?**
```java
String input = "1,234.56";
// Parser en Number
```

**R:**
```java
try {
    NumberFormat nf = NumberFormat.getInstance();
    Number number = nf.parse(input);
    
    double value = number.doubleValue();
    System.out.println(value);  // 1234.56
    
} catch (ParseException e) {
    System.out.println("Invalid number format");
}

// ATTENTION: parse() peut s'arrêter avant la fin
NumberFormat nf = NumberFormat.getInstance();
Number n = nf.parse("123abc");  // Ne lance PAS d'exception!
System.out.println(n);  // 123 (ignore "abc")
```

---

**Q28: Comment contrôler le nombre de décimales?**
```java
NumberFormat nf = NumberFormat.getInstance();
System.out.println(nf.format(123.456));
// Comment forcer 2 décimales?
```

**R:**
```java
NumberFormat nf = NumberFormat.getInstance();
nf.setMinimumFractionDigits(2);  // Minimum 2 décimales
nf.setMaximumFractionDigits(2);  // Maximum 2 décimales

System.out.println(nf.format(123.4));     // "123.40"
System.out.println(nf.format(123.456));   // "123.46" (arrondi)
System.out.println(nf.format(123));       // "123.00"

// Pour nombres entiers
nf.setMinimumIntegerDigits(3);
System.out.println(nf.format(5));  // "005.00"
```

---

**Q29: Quelle est la différence entre DateFormat styles?**
```java
Date now = new Date();
```

**R:**
```java
// SHORT - court
DateFormat shortDate = DateFormat.getDateInstance(DateFormat.SHORT);
System.out.println(shortDate.format(now));  // "11/6/25"

// MEDIUM - moyen (défaut)
DateFormat mediumDate = DateFormat.getDateInstance(DateFormat.MEDIUM);
System.out.println(mediumDate.format(now));  // "Nov 6, 2025"

// LONG - long
DateFormat longDate = DateFormat.getDateInstance(DateFormat.LONG);
System.out.println(longDate.format(now));  // "November 6, 2025"

// FULL - complet
DateFormat fullDate = DateFormat.getDateInstance(DateFormat.FULL);
System.out.println(fullDate.format(now));  // "Thursday, November 6, 2025"
```

---

**Q30: Comment formater date ET heure?**
```java
Date now = new Date();
```

**R:**
```java
DateFormat dateTime = DateFormat.getDateTimeInstance(
    DateFormat.SHORT,  // Style date
    DateFormat.SHORT   // Style heure
);
System.out.println(dateTime.format(now));  // "11/6/25, 10:30 AM"

DateFormat longDateTime = DateFormat.getDateTimeInstance(
    DateFormat.LONG,
    DateFormat.LONG
);
System.out.println(longDateTime.format(now));
// "November 6, 2025 at 10:30:45 AM EST"
```

---

**Q31: Que font les patterns de SimpleDateFormat?**
```java
SimpleDateFormat sdf = new SimpleDateFormat("???");
```

**R:**

| **Pattern** | **Description** | **Exemple** |
|-------------|-----------------|-------------|
| `y` | Année | 2025 |
| `M` | Mois (nombre) | 11 |
| `MM` | Mois (2 chiffres) | 11 |
| `MMM` | Mois (court) | Nov |
| `MMMM` | Mois (complet) | November |
| `d` | Jour | 6 |
| `dd` | Jour (2 chiffres) | 06 |
| `E` | Jour semaine (court) | Thu |
| `EEEE` | Jour semaine (complet) | Thursday |
| `H` | Heure 24h | 14 |
| `h` | Heure 12h | 2 |
| `m` | Minute | 30 |
| `s` | Seconde | 45 |
| `a` | AM/PM | PM |

```java
Date now = new Date();

SimpleDateFormat sdf1 = new SimpleDateFormat("yyyy-MM-dd");
System.out.println(sdf1.format(now));  // "2025-11-06"

SimpleDateFormat sdf2 = new SimpleDateFormat("dd/MM/yyyy HH:mm:ss");
System.out.println(sdf2.format(now));  // "06/11/2025 14:30:45"

SimpleDateFormat sdf3 = new SimpleDateFormat("EEEE, MMMM d, yyyy");
System.out.println(sdf3.format(now));  // "Thursday, November 6, 2025"

SimpleDateFormat sdf4 = new SimpleDateFormat("hh:mm a");
System.out.println(sdf4.format(now));  // "02:30 PM"
```

---

**Q32: SimpleDateFormat est-il thread-safe?**

**R:** **NON**, SimpleDateFormat n'est **PAS thread-safe**.

```java
// MAUVAISE PRATIQUE - variable statique
private static final SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");

public void method() {
    String date = sdf.format(new Date());  // Problème en multi-threading!
}

// BONNE PRATIQUE - variable locale
public void method() {
    SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
    String date = sdf.format(new Date());  // OK
}

// MEILLEURE PRATIQUE - DateTimeFormatter (Java 8+, thread-safe)
private static final DateTimeFormatter formatter = 
    DateTimeFormatter.ofPattern("yyyy-MM-dd");

public void method() {
    String date = LocalDate.now().format(formatter);  // Thread-safe!
}
```

---

**Q33: Quelle est la différence entre SimpleDateFormat et DateTimeFormatter?**

**R:**

| **SimpleDateFormat** | **DateTimeFormatter** |
|----------------------|-----------------------|
| Java 1.1 | Java 8+ |
| PAS thread-safe | Thread-safe |
| Utilise Date | Utilise LocalDate/LocalDateTime |
| Mutable | Immutable |

```java
// SimpleDateFormat (ancien)
SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
String date1 = sdf.format(new Date());

// DateTimeFormatter (moderne)
DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd");
String date2 = LocalDate.now().format(formatter);
```

---

## Questions sur Locale

**Q34: Comment créer une Locale?**

**R:**
```java
// Constantes prédéfinies
Locale us = Locale.US;
Locale france = Locale.FRANCE;
Locale canada = Locale.CANADA;

// Constructeur - langue seulement
Locale french = new Locale("fr");

// Constructeur - langue + pays
Locale frenchFrance = new Locale("fr", "FR");
Locale frenchCanada = new Locale("fr", "CA");

// Constructeur - langue + pays + variante
Locale custom = new Locale("fr", "FR", "MAC");

// Builder (Java 7+)
Locale locale = new Locale.Builder()
    .setLanguage("fr")
    .setRegion("FR")
    .build();
```

---

**Q35: Que retournent getLanguage() et getCountry()?**
```java
Locale locale = new Locale("fr", "CA");
```

**R:**
```java
System.out.println(locale.getLanguage());  // "fr"
System.out.println(locale.getCountry());   // "CA"
System.out.println(locale.getDisplayLanguage());  // "French" (en anglais)
System.out.println(locale.getDisplayCountry());   // "Canada" (en anglais)

// Display name dans une autre locale
System.out.println(locale.getDisplayLanguage(Locale.FRANCE));  // "français"
System.out.println(locale.getDisplayCountry(Locale.FRANCE));   // "Canada"

// Display name complet
System.out.println(locale.getDisplayName());  // "French (Canada)"
System.out.println(locale.getDisplayName(Locale.FRANCE));  // "français (Canada)"
```

---

**Q36: Comment changer la Locale par défaut?**

**R:**
```java
// Obtenir la Locale par défaut
Locale defaultLocale = Locale.getDefault();
System.out.println(defaultLocale);  // Dépend du système

// Changer la Locale par défaut
Locale.setDefault(Locale.FRANCE);

// Maintenant tous les formats utilisent la Locale française
NumberFormat nf = NumberFormat.getInstance();  // Format français
DateFormat df = DateFormat.getDateInstance();  // Format français

// Impact global
System.out.println(nf.format(1234.56));  // "1 234,56"
```

**ATTENTION**: `setDefault()` affecte **toute** l'application!

---

## Questions sur ResourceBundle

**Q37: Comment charger un ResourceBundle?**

**R:**
```java
// Fichiers properties:
// messages.properties
// messages_fr.properties
// messages_fr_CA.properties

// Charger le bundle par défaut
ResourceBundle bundle = ResourceBundle.getBundle("messages");

// Charger avec Locale spécifique
ResourceBundle frBundle = ResourceBundle.getBundle("messages", Locale.FRANCE);

// Charger avec Locale + ClassLoader
ResourceBundle bundle3 = ResourceBundle.getBundle(
    "messages",
    Locale.FRANCE,
    MyClass.class.getClassLoader()
);

// Obtenir une valeur
String greeting = bundle.getString("greeting");
```

---

**Q38: Quel est l'ordre de recherche pour ResourceBundle?**

**R:** Pour `ResourceBundle.getBundle("MyResource", new Locale("fr", "CA"))`:

**Ordre de recherche**:
1. `MyResource_fr_CA.class`
2. `MyResource_fr_CA.properties`
3. `MyResource_fr.class`
4. `MyResource_fr.properties`
5. `MyResource.class` (default)
6. `MyResource.properties` (default)

Si aucun trouvé → **MissingResourceException**

```java
// Exemple
Locale locale = new Locale("fr", "CA");

try {
    ResourceBundle bundle = ResourceBundle.getBundle("messages", locale);
    System.out.println(bundle.getString("greeting"));
} catch (MissingResourceException e) {
    System.out.println("Bundle not found");
}
```

---

**Q39: Que se passe-t-il si une clé n'existe pas?**
```java
ResourceBundle bundle = ResourceBundle.getBundle("messages");
String value = bundle.getString("nonexistent");
```

**R:** Lance **MissingResourceException**.

```java
// Gestion d'erreur
try {
    String value = bundle.getString("nonexistent");
} catch (MissingResourceException e) {
    System.out.println("Key not found: " + e.getKey());
}

// Vérifier si clé existe
if (bundle.containsKey("greeting")) {
    System.out.println(bundle.getString("greeting"));
}

// Méthode avec valeur par défaut
public String getMessage(ResourceBundle bundle, String key, String defaultValue) {
    try {
        return bundle.getString(key);
    } catch (MissingResourceException e) {
        return defaultValue;
    }
}
```

---

**Q40: Comment utiliser MessageFormat avec ResourceBundle?**

**R:**

```properties
# messages.properties
welcome=Welcome {0}!
order=Your order of {0} items costs {1}
```

```java
import java.text.MessageFormat;

ResourceBundle bundle = ResourceBundle.getBundle("messages");

// Avec un paramètre
String pattern = bundle.getString("welcome");
String message = MessageFormat.format(pattern, "Alice");
System.out.println(message);  // "Welcome Alice!"

// Avec plusieurs paramètres
String orderPattern = bundle.getString("order");
String orderMessage = MessageFormat.format(orderPattern, 3, "$45.99");
System.out.println(orderMessage);  // "Your order of 3 items costs $45.99"

// Équivalent
Object[] params = {"Alice"};
String message2 = MessageFormat.format(pattern, params);
```

---

**Q41: Quelle est la différence entre .properties et ListResourceBundle?**

**R:**

**Properties File (.properties)**:
- Fichier texte
- Seulement **Strings**
- Facile à éditer

```properties
# messages.properties
greeting=Hello
count=42
```

**ListResourceBundle (classe Java)**:
- Classe Java
- Peut stocker **n'importe quel Object**
- Plus flexible

```java
public class MyResources extends ListResourceBundle {
    @Override
    protected Object[][] getContents() {
        return new Object[][] {
            {"greeting", "Hello"},
            {"count", Integer.valueOf(42)},  // Object, pas String!
            {"list", Arrays.asList("a", "b", "c")}
        };
    }
}

// Utilisation
ResourceBundle bundle = ResourceBundle.getBundle("MyResources");
String greeting = bundle.getString("greeting");
Integer count = (Integer) bundle.getObject("count");  // Objet!
```

**IMPORTANT**: `.class` a **priorité** sur `.properties` dans la recherche.

---

**Q42: Comment gérer le fallback de ResourceBundle?**

**R:** ResourceBundle utilise un **fallback automatique**:

```
Recherche pour Locale("fr", "CA"):
1. messages_fr_CA.properties  (plus spécifique)
2. messages_fr.properties
3. messages.properties         (default, toujours présent)
```

```properties
# messages.properties (default - TOUJOURS présent)
greeting=Hello
farewell=Goodbye
question=What?

# messages_fr.properties
greeting=Bonjour
farewell=Au revoir
# question pas déf


# Questions/Réponses - Chapter 11: Exceptions and Localization

## Questions sur ResourceBundle (suite)

**Q43: Que retourne ce code?**
```java
// messages.properties
greeting=Hello
farewell=Goodbye

// messages_fr.properties
greeting=Bonjour
// farewell pas défini

ResourceBundle frBundle = ResourceBundle.getBundle("messages", Locale.FRANCE);
System.out.println(frBundle.getString("greeting"));
System.out.println(frBundle.getString("farewell"));
```

**R:**
```
Bonjour
Goodbye
```

**Fallback**: Si une clé n'existe pas dans `messages_fr.properties`, ResourceBundle cherche dans `messages.properties` (default).

---

**Q44: Comment lister toutes les clés d'un ResourceBundle?**

**R:**
```java
ResourceBundle bundle = ResourceBundle.getBundle("messages");

// Méthode 1: getKeys()
Enumeration<String> keys = bundle.getKeys();
while (keys.hasMoreElements()) {
    String key = keys.nextElement();
    System.out.println(key + " = " + bundle.getString(key));
}

// Méthode 2: keySet() (Java 9+)
Set<String> keySet = bundle.keySet();
for (String key : keySet) {
    System.out.println(key + " = " + bundle.getString(key));
}

// IMPORTANT: getKeys() retourne les clés du bundle ACTUEL + parents
// Si messages_fr.properties a "greeting" et messages.properties a "farewell",
// bundle français aura les DEUX clés
```

---

**Q45: Quelle est la différence entre ces deux?**
```java
// Version 1
ResourceBundle bundle1 = ResourceBundle.getBundle("messages", Locale.FRANCE);

// Version 2
Locale.setDefault(Locale.FRANCE);
ResourceBundle bundle2 = ResourceBundle.getBundle("messages");
```

**R:** **Aucune différence fonctionnelle** pour ce cas.

Les deux chargent le bundle français.

**MAIS attention**:
```java
// Si la Locale change après
Locale.setDefault(Locale.US);
ResourceBundle bundle3 = ResourceBundle.getBundle("messages");
// bundle3 est US, mais bundle2 reste français (immutable)
```

---

**Q46: Comment gérer des caractères spéciaux dans .properties?**

**R:** Les fichiers `.properties` sont en **ISO-8859-1** par défaut. Pour les caractères Unicode:

```properties
# MÉTHODE 1: Utiliser \uXXXX
greeting=Bonjour \u00E0 tous

# MÉTHODE 2: Utiliser native2ascii (outil Java)
# Écrire normalement: greeting=Bonjour à tous
# Puis convertir avec: native2ascii input.properties output.properties

# MÉTHODE 3: Java 9+ supporte UTF-8 directement
# Spécifier encoding UTF-8 dans le fichier
```

**Meilleure pratique**: Utiliser **ListResourceBundle** pour éviter les problèmes d'encoding.

```java
public class Messages_fr extends ListResourceBundle {
    @Override
    protected Object[][] getContents() {
        return new Object[][] {
            {"greeting", "Bonjour à tous"}  // Caractères spéciaux OK
        };
    }
}
```

---

## Questions Pièges et Cas Complexes

**Q47: Ce code compile-t-il?**
```java
try {
    System.out.println("Try");
}
System.out.println("After");
```

**R:** Non, **erreur de compilation**: "try without catch, finally or resource declarations".

Un `try` doit avoir **AU MOINS UN** des suivants:
- `catch` block
- `finally` block
- ressources (try-with-resources)

**Correct:**
```java
// Avec catch
try {
    System.out.println("Try");
} catch (Exception e) {
    e.printStackTrace();
}

// Avec finally
try {
    System.out.println("Try");
} finally {
    System.out.println("Finally");
}

// Avec ressources
try (FileReader reader = new FileReader("file.txt")) {
    // Code
}
```

---

**Q48: Que se passe-t-il?**
```java
try {
    throw new IOException();
} catch (FileNotFoundException e) {
    System.out.println("FileNotFound");
} catch (IOException e) {
    System.out.println("IO");
}
```

**R:** Affiche `"IO"`.

`FileNotFoundException` extends `IOException`, mais l'exception lancée est **IOException** (pas FileNotFoundException), donc le 2ème catch est exécuté.

---

**Q49: Ce code compile-t-il?**
```java
public void method() {
    try {
        throw new IOException();
    } catch (FileNotFoundException e) {
        System.out.println("FileNotFound");
    }
}
```

**R:** Non, **erreur de compilation**: "unreported exception IOException; must be caught or declared to be thrown".

On catch **FileNotFoundException** mais on throw **IOException**. IOException n'est pas catchée.

**Correct:**
```java
public void method() {
    try {
        throw new IOException();
    } catch (IOException e) {  // Catch IOException (parent)
        System.out.println("IO");
    }
}
```

---

**Q50: Que s'affiche-t-il?**
```java
try {
    try {
        throw new IOException();
    } finally {
        throw new RuntimeException();
    }
} catch (Exception e) {
    System.out.println(e.getClass().getName());
}
```

**R:** `"java.lang.RuntimeException"`

L'exception du **finally écrase** l'exception du try. L'IOException est perdue.

**MAUVAISE PRATIQUE**: Ne jamais throw d'exception dans finally!

---

**Q51: Ce code compile-t-il?**
```java
public void method() throws IOException {
    try {
        throw new IOException();
    } catch (IOException e) {
        // Empty catch
    }
}
```

**R:** Oui, compile.

L'exception est **catchée**, donc pas besoin de la déclarer avec `throws`. Le `throws` dans la signature est **inutile** mais pas une erreur.

---

**Q52: Que se passe-t-il?**
```java
public static void main(String[] args) {
    try {
        method();
    } catch (Exception e) {
        System.out.println("Caught");
    }
}

public static void method() {
    throw new RuntimeException();
}
```

**R:** Affiche `"Caught"`.

**RuntimeException** peut être lancée **sans** `throws` dans la signature, et peut être catchée comme n'importe quelle exception.

---

**Q53: Ce code compile-t-il?**
```java
public void method() throws IOException {
    // Méthode vide - ne lance jamais IOException
}
```

**R:** Oui, **compile**.

On peut déclarer `throws` même si la méthode ne lance jamais l'exception. C'est un **contrat** (API).

**Mais attention avec RuntimeException**:
```java
public void method() throws NullPointerException {
    // MAUVAISE PRATIQUE - ne jamais déclarer RuntimeException
}
```

---

**Q54: Que retourne ce code?**
```java
public int getValue() {
    try {
        return 10;
    } finally {
        System.out.println("Finally");
    }
}

int result = getValue();
System.out.println(result);
```

**R:**
```
Finally
10
```

Le `finally` est exécuté **avant le return**, mais le return du try est quand même effectué.

---

**Q55: Quelle est la différence?**
```java
// Version 1
try {
    method();
} catch (Exception e) {
    throw e;
}

// Version 2
try {
    method();
} catch (Exception e) {
    throw new RuntimeException(e);
}
```

**R:**
- **Version 1**: **Relance** l'exception originale (même type)
- **Version 2**: **Wrap** dans RuntimeException (change le type)

```java
// Version 1
public void caller() throws Exception {  // Doit déclarer Exception
    try {
        method();
    } catch (Exception e) {
        throw e;  // Relance Exception
    }
}

// Version 2
public void caller() {  // Pas de throws nécessaire
    try {
        method();
    } catch (Exception e) {
        throw new RuntimeException(e);  // RuntimeException = unchecked
    }
}

// Version 2 permet de récupérer l'exception originale
try {
    caller();
} catch (RuntimeException e) {
    Throwable cause = e.getCause();  // Exception originale
}
```

---

**Q56: Comment créer un multi-catch avec relation d'héritage?**
```java
try {
    // Code
} catch (IOException | FileNotFoundException e) {
    // ???
}
```

**R:** **Erreur de compilation**: "The exception FileNotFoundException is already caught by the alternative IOException".

**Règle**: Les exceptions dans multi-catch ne doivent **PAS** avoir de relation d'héritage.

**Correct:**
```java
// Soit catch séparé
try {
    // Code
} catch (FileNotFoundException e) {
    // Spécifique
} catch (IOException e) {
    // Général
}

// Soit catch seulement le parent
try {
    // Code
} catch (IOException e) {
    // Catch IOException ET FileNotFoundException
}
```

---

**Q57: Que fait ce code?**
```java
NumberFormat nf = NumberFormat.getInstance(Locale.US);
try {
    Number n = nf.parse("123abc");
    System.out.println(n);
} catch (ParseException e) {
    System.out.println("Error");
}
```

**R:** Affiche `123` (pas d'exception).

`parse()` s'arrête au premier caractère **non-numérique** et **ne lance pas d'exception**. "abc" est ignoré.

**Pour une parsing stricte**:
```java
NumberFormat nf = NumberFormat.getInstance();
ParsePosition pos = new ParsePosition(0);
Number n = nf.parse("123abc", pos);
if (pos.getIndex() != "123abc".length()) {
    System.out.println("Invalid format");
}
```

---

**Q58: Quelle est la différence entre format() et parse()?**

**R:**
- **format()**: Objet → String (pas d'exception)
- **parse()**: String → Objet (peut lancer ParseException)

```java
// format - jamais d'exception
NumberFormat nf = NumberFormat.getInstance();
String formatted = nf.format(123.45);  // "123.45"

DateFormat df = DateFormat.getDateInstance();
String dateStr = df.format(new Date());  // "Nov 6, 2025"

// parse - peut lancer ParseException
try {
    Number num = nf.parse("123.45");
    Date date = df.parse("Nov 6, 2025");
} catch (ParseException e) {
    System.out.println("Invalid format");
}
```

---

**Q59: Ce code a-t-il un problème?**
```java
SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
String date1 = sdf.format(new Date());
String date2 = sdf.format(new Date());
```

**R:** En **single-thread**, aucun problème.

En **multi-thread**, **race condition** possible car SimpleDateFormat n'est **pas thread-safe**.

**Solution 1**: Variable locale
```java
public String formatDate() {
    SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
    return sdf.format(new Date());
}
```

**Solution 2**: Synchronisation
```java
private static final SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");

public synchronized String formatDate() {
    return sdf.format(new Date());
}
```

**Solution 3 (MEILLEURE)**: DateTimeFormatter (thread-safe)
```java
private static final DateTimeFormatter formatter = 
    DateTimeFormatter.ofPattern("yyyy-MM-dd");

public String formatDate() {
    return LocalDate.now().format(formatter);  // Thread-safe!
}
```

---

**Q60: Que se passe-t-il?**
```java
try {
    int[] array = {1, 2, 3};
    System.out.println(array[5]);
} catch (RuntimeException e) {
    System.out.println("Runtime");
} catch (ArrayIndexOutOfBoundsException e) {
    System.out.println("Array");
}
```

**R:** **Erreur de compilation**: "exception ArrayIndexOutOfBoundsException has already been caught".

`ArrayIndexOutOfBoundsException` extends `RuntimeException`, donc le 2ème catch est unreachable.

---

**Q61: Comment gérer plusieurs exceptions avec le même code?**
```java
try {
    // Code qui peut lancer IOException ou SQLException
} catch (IOException e) {
    logger.error("Error", e);
} catch (SQLException e) {
    logger.error("Error", e);  // Code dupliqué!
}
```

**R:** Utiliser **multi-catch**:

```java
try {
    // Code
} catch (IOException | SQLException e) {
    logger.error("Error", e);  // Code unique
}

// ATTENTION: e est final
try {
    // Code
} catch (IOException | SQLException e) {
    // e = new IOException();  // ERREUR: e est final
    logger.error("Error", e);
}
```

---

**Q62: Que retourne ce code?**
```java
Locale locale = new Locale("fr", "CA");
NumberFormat nf = NumberFormat.getCurrencyInstance(locale);
System.out.println(nf.format(123.45));
```

**R:** Dépend de la JVM, mais typiquement: `"123,45 $"` ou `"123,45 $ CA"`

**ATTENTION**: Le symbole de devise dépend de la Locale ET des paramètres système.

```java
// US
NumberFormat us = NumberFormat.getCurrencyInstance(Locale.US);
System.out.println(us.format(123.45));  // "$123.45"

// France
NumberFormat fr = NumberFormat.getCurrencyInstance(Locale.FRANCE);
System.out.println(fr.format(123.45));  // "123,45 €"

// UK
NumberFormat uk = NumberFormat.getCurrencyInstance(Locale.UK);
System.out.println(uk.format(123.45));  // "£123.45"

// Japan
NumberFormat jp = NumberFormat.getCurrencyInstance(Locale.JAPAN);
System.out.println(jp.format(123.45));  // "¥123"
```

---

**Q63: Comment formater un pourcentage avec décimales?**
```java
double value = 0.7547;
// Format: "75.47%"
```

**R:**
```java
NumberFormat percent = NumberFormat.getPercentInstance();
percent.setMinimumFractionDigits(2);
percent.setMaximumFractionDigits(2);

System.out.println(percent.format(0.7547));  // "75.47%"

// Par défaut (sans setters)
NumberFormat defaultPercent = NumberFormat.getPercentInstance();
System.out.println(defaultPercent.format(0.7547));  // "75%" (arrondi)

// ATTENTION: multiply par 100 automatiquement
System.out.println(percent.format(75.47));  // "7,547%" (ERREUR!)
```

---

**Q64: Quelle est la différence entre Locale.FRANCE et new Locale("fr", "FR")?**

**R:** **Aucune différence fonctionnelle**.

```java
Locale france1 = Locale.FRANCE;
Locale france2 = new Locale("fr", "FR");

System.out.println(france1.equals(france2));  // true

// Mais FRANCE est une constante (meilleure pratique)
if (locale == Locale.FRANCE) {  // Comparaison référence OK
    // ...
}

if (locale.equals(new Locale("fr", "FR"))) {  // Doit utiliser equals
    // ...
}
```

**Constantes disponibles**:
```java
Locale.US           // en_US
Locale.UK           // en_GB
Locale.FRANCE       // fr_FR
Locale.GERMANY      // de_DE
Locale.ITALY        // it_IT
Locale.JAPAN        // ja_JP
Locale.CHINA        // zh_CN
Locale.CANADA       // en_CA
Locale.CANADA_FRENCH // fr_CA
```

---

**Q65: Ce code a-t-il un problème?**
```java
try (BufferedReader br = new BufferedReader(new FileReader("file.txt"))) {
    String line = br.readLine();
}
```

**R:** Potentiellement, si `new FileReader()` lance une exception, le `BufferedReader` n'est **pas encore créé**, donc rien à fermer.

**Meilleure pratique**:
```java
try (FileReader fr = new FileReader("file.txt");
     BufferedReader br = new BufferedReader(fr)) {
    String line = br.readLine();
}
// Les deux ressources sont fermées dans l'ordre inverse
```

Ou encore mieux:
```java
try (BufferedReader br = Files.newBufferedReader(Paths.get("file.txt"))) {
    String line = br.readLine();
}
```

---

**Q66: Que fait ce code?**
```java
ResourceBundle bundle = ResourceBundle.getBundle("messages", Locale.FRANCE);
Enumeration<String> keys = bundle.getKeys();

while (keys.hasMoreElements()) {
    String key = keys.nextElement();
    bundle.getString(key);
    keys = bundle.getKeys();  // Recharge les clés
}
```

**R:** **Boucle infinie**!

`getKeys()` retourne un **nouvel** Enumeration à chaque appel. L'Enumeration est réinitialisé dans la boucle.

**Correct**:
```java
ResourceBundle bundle = ResourceBundle.getBundle("messages", Locale.FRANCE);
Enumeration<String> keys = bundle.getKeys();  // Une seule fois

while (keys.hasMoreElements()) {
    String key = keys.nextElement();
    String value = bundle.getString(key);
    System.out.println(key + " = " + value);
}
```

---

**Q67: Comment gérer des exceptions dans une lambda?**
```java
List<String> files = Arrays.asList("file1.txt", "file2.txt");
files.forEach(file -> {
    FileReader reader = new FileReader(file);  // Checked exception!
});
```

**R:** **Erreur de compilation**: "unhandled exception IOException".

Les lambdas **ne peuvent pas throw checked exceptions** directement.

**Solutions**:

```java
// Solution 1: Try-catch dans la lambda
files.forEach(file -> {
    try {
        FileReader reader = new FileReader(file);
    } catch (IOException e) {
        e.printStackTrace();
    }
});

// Solution 2: Wrapper méthode
files.forEach(file -> readFile(file));

private static void readFile(String file) {
    try {
        FileReader reader = new FileReader(file);
    } catch (IOException e) {
        e.printStackTrace();
    }
}

// Solution 3: Wrap dans RuntimeException
files.forEach(file -> {
    try {
        FileReader reader = new FileReader(file);
    } catch (IOException e) {
        throw new RuntimeException(e);
    }
});

// Solution 4: Utiliser une functional interface custom
@FunctionalInterface
interface CheckedConsumer<T> {
    void accept(T t) throws Exception;
}

static <T> Consumer<T> wrap(CheckedConsumer<T> consumer) {
    return t -> {
        try {
            consumer.accept(t);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    };
}

files.forEach(wrap(file -> {
    FileReader reader = new FileReader(file);
}));
```

---

**Q68: Que se passe-t-il avec des resources null?**
```java
FileReader reader = null;
try (reader) {
    // Code
}
```

**R:** **NullPointerException** au moment de fermer!

**Règle**: Les ressources dans try-with-resources ne doivent **JAMAIS** être null.

**Correct**:
```java
FileReader reader = getReader();  // Peut retourner null
if (reader != null) {
    try (reader) {
        // Code
    }
}

// Ou
try (FileReader reader = getReaderOrThrow()) {  // Lance exception si null
    // Code
}
```

---

**Q69: Comment formater un nombre avec grouping personnalisé?**
```java
double value = 1234567.89;
// Format: "1'234'567.89" (apostrophe comme séparateur)
```

**R:**
```java
// Utiliser DecimalFormat
DecimalFormat df = new DecimalFormat("#,##0.00");
DecimalFormatSymbols symbols = new DecimalFormatSymbols();
symbols.setGroupingSeparator('\'');
df.setDecimalFormatSymbols(symbols);

System.out.println(df.format(1234567.89));  // "1'234'567.89"

// Autre exemple: espace comme séparateur
symbols.setGroupingSeparator(' ');
df.setDecimalFormatSymbols(symbols);
System.out.println(df.format(1234567.89));  // "1 234 567.89"
```

---

**Q70: Quelle est la différence entre ces deux patterns?**
```java
SimpleDateFormat sdf1 = new SimpleDateFormat("yyyy-MM-dd");
SimpleDateFormat sdf2 = new SimpleDateFormat("YYYY-MM-dd");
```

**R:**
- `yyyy`: Année calendaire (2025)
- `YYYY`: Année de la **week** (week year)

```java
Date date = new Date(2024, 11, 31);  // 31 décembre 2024

SimpleDateFormat sdf1 = new SimpleDateFormat("yyyy-MM-dd");
System.out.println(sdf1.format(date));  // "2024-12-31"

SimpleDateFormat sdf2 = new SimpleDateFormat("YYYY-MM-dd");
System.out.println(sdf2.format(date));  // "2025-12-31" (!) si dans semaine 1 de 2025

// ATTENTION: TOUJOURS utiliser yyyy (lowercase)
```

---

**Q71: Comment obtenir toutes les Locales disponibles?**

**R:**
```java
Locale[] locales = Locale.getAvailableLocales();

System.out.println("Nombre de locales: " + locales.length);  // ~700+

for (Locale locale : locales) {
    System.out.println(locale.getDisplayName());
}

// Filtrer par langue
List<Locale> frenchLocales = Arrays.stream(Locale.getAvailableLocales())
    .filter(l -> l.getLanguage().equals("fr"))
    .collect(Collectors.toList());

for (Locale locale : frenchLocales) {
    System.out.println(locale.getDisplayName());
}
// French (France)
// French (Canada)
// French (Switzerland)
// etc.
```

---

**Q72: Ce code est-il valide?**
```java
try {
    throw new IOException();
} catch (Exception e) {
    if (e instanceof IOException) {
        throw (IOException) e;
    }
}
```

**R:** **Erreur de compilation**: "unreported exception IOException".

Même si on **sait** que c'est une IOException, le compilateur voit qu'on throw une **checked exception** qui n'est pas déclarée.

**Correct**:
```java
public void method() throws IOException {
    try {
        throw new IOException();
    } catch (Exception e) {
        if (e instanceof IOException) {
            throw (IOException) e;
        }
    }
}
```

---

**Q73: Comment gérer les propriétés avec valeurs par défaut?**

**R:**
```java
Properties props = new Properties();

// Charger fichier
try (InputStream is = new FileInputStream("config.properties")) {
    props.load(is);
} catch (IOException e) {
    e.printStackTrace();
}

// Obtenir avec valeur par défaut
String host = props.getProperty("host", "localhost");
String port = props.getProperty("port", "8080");
String timeout = props.getProperty("timeout", "30");

// Convertir en int
int portNum = Integer.parseInt(port);
int timeoutSec = Integer.parseInt(timeout);

// Properties par défaut
Properties defaults = new Properties();
defaults.setProperty("host", "localhost");
defaults.setProperty("port", "8080");

Properties props2 = new Properties(defaults);
// Si une clé n'existe pas, cherche dans defaults
```

---

**Q74: Quelle est la différence entre ces deux?**
```java
// Version 1
DateFormat df1 = DateFormat.getDateInstance(DateFormat.SHORT, Locale.US);

// Version 2
Locale.setDefault(Locale.US);
DateFormat df2 = DateFormat.getDateInstance(DateFormat.SHORT);
```

**R:** **Aucune différence fonctionnelle** pour ce cas.

**MAIS**:
- Version 1: Locale **explicite** (meilleure pratique)
- Version 2: Dépend de la Locale **globale** (peut changer)

```java
// Version 1 - stable
DateFormat df1 = DateFormat.getDateInstance(DateFormat.SHORT, Locale.US);
Locale.setDefault(Locale.FRANCE);  // df1 reste US

// Version 2 - instable
Locale.setDefault(Locale.US);
DateFormat df2 = DateFormat.getDateInstance(DateFormat.SHORT);
Locale.setDefault(Locale.FRANCE);  // df2 reste US (créé avant le changement)

// Mais les NOUVEAUX DateFormat utilisent la nouvelle Locale
DateFormat df3 = DateFormat.getDateInstance(DateFormat.SHORT);  // France
```

---

**Q75: Comment chainer les exceptions?**
```java
try {
    // Code
} catch (IOException e) {
    // Comment inclure l'exception originale?
}
```

**R:**
```java
try {
    readFile("file.txt");
} catch (IOException e) {
    // Wrap et chain
    throw new RuntimeException("Failed to read file", e);
}

// Récupérer la cause
try {
    method();
} catch (RuntimeException e) {
    Throwable cause = e.getCause();  // IOException originale
    System.out.println("Original: " + cause.getMessage());
    
    // Stack trace complet
    e.printStackTrace();
    // RuntimeException: Failed to read file
    //   Caused by: IOException: file not found
}

// initCause (alternative)
RuntimeException wrapper = new RuntimeException("Failed");
wrapper.initCause(new IOException("IO error"));
throw wrapper;
```

---
