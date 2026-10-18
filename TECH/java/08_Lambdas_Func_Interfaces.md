## [**..**](./00_index.md)

# Chapter 8: Lambdas and Functional Interfaces

## Writing Simple Lambdas

### Lambda Basics

Une **lambda expression** est une fonction anonyme concise, introduite en Java 8.

**Syntaxe de base**:
```
(parameters) -> expression
(parameters) -> { statements; }
```

**Avant les lambdas** (anonymous class):
```java
Comparator<String> comparator = new Comparator<String>() {
    @Override
    public int compare(String s1, String s2) {
        return s1.length() - s2.length();
    }
};
```

**Avec lambda**:
```java
Comparator<String> comparator = (s1, s2) -> s1.length() - s2.length();
```

### Lambda Syntax

**Une seule expression** (return implicite):
```java
// Sans paramètres
() -> System.out.println("Hello")
() -> 42

// Un paramètre (parenthèses optionnelles)
x -> x * 2
(x) -> x * 2  // Équivalent

// Plusieurs paramètres
(x, y) -> x + y
(a, b, c) -> a + b + c
```

**Bloc de statements** (return explicite si nécessaire):
```java
// Avec return
(x, y) -> {
    int sum = x + y;
    return sum;
}

// Sans return (void)
() -> {
    System.out.println("Line 1");
    System.out.println("Line 2");
}

// Un paramètre avec bloc
x -> {
    System.out.println(x);
    return x * 2;
}
```

### Lambda Syntax Rules

**Règle 1**: Parenthèses **optionnelles** pour un seul paramètre (sans type)
```java
x -> x * 2          // OK
(x) -> x * 2        // OK
x, y -> x + y       // ERREUR: parenthèses requises pour plusieurs paramètres
(x, y) -> x + y     // OK
```

**Règle 2**: Types de paramètres **optionnels** (inférence de type)
```java
(x, y) -> x + y                    // OK: types inférés
(int x, int y) -> x + y            // OK: types explicites
(x, int y) -> x + y                // ERREUR: tous ou aucun
```

**Règle 3**: Accolades `{}` requises pour plusieurs statements
```java
x -> x * 2                         // OK: une expression
x -> { return x * 2; }             // OK: bloc avec return
x -> System.out.println(x)         // OK: une expression void
x -> {
    System.out.println(x);
    return x * 2;
}  // OK: bloc avec plusieurs statements
```

**Règle 4**: `return` requis dans un bloc (si la méthode retourne quelque chose)
```java
(x, y) -> { x + y; }               // ERREUR: missing return
(x, y) -> { return x + y; }        // OK
```

**Règle 5**: Point-virgule `;` requis à la fin des statements dans un bloc
```java
() -> { System.out.println("Hello") }     // ERREUR: missing ;
() -> { System.out.println("Hello"); }    // OK
```

### Lambda Examples

```java
// Pas de paramètres
Runnable r = () -> System.out.println("Running");

// Un paramètre
Consumer<String> printer = s -> System.out.println(s);
Function<Integer, Integer> doubler = x -> x * 2;

// Plusieurs paramètres
BiFunction<Integer, Integer, Integer> adder = (a, b) -> a + b;
Comparator<String> byLength = (s1, s2) -> s1.length() - s2.length();

// Bloc de code
Consumer<List<String>> processor = list -> {
    list.sort(String::compareTo);
    System.out.println("Sorted: " + list);
};
```

## Coding Functional Interfaces

### Functional Interface Definition

Une **functional interface** a **exactement une** méthode abstraite.

**@FunctionalInterface** annotation (optionnelle mais recommandée):
```java
@FunctionalInterface
public interface Calculateur {
    int calculer(int a, int b);  // Méthode abstraite
    
    // Default et static methods OK (ne comptent pas)
    default void log() {
        System.out.println("Calculation");
    }
    
    static void info() {
        System.out.println("Calculateur interface");
    }
}
```

**Utilisation avec lambda**:
```java
Calculateur addition = (a, b) -> a + b;
Calculateur multiplication = (a, b) -> a * b;

System.out.println(addition.calculer(5, 3));        // 8
System.out.println(multiplication.calculer(5, 3));  // 15
```

### Why Functional Interfaces?

Les lambdas fonctionnent **uniquement** avec des functional interfaces.

```java
// Interface avec 1 méthode abstraite → OK
@FunctionalInterface
interface Printer {
    void print(String s);
}

Printer p = s -> System.out.println(s);  // Lambda OK

// Interface avec 2 méthodes abstraites → ERREUR
interface Invalid {
    void method1();
    void method2();
}

// Invalid obj = () -> {...};  // ERREUR: pas une functional interface
```

### Common Functional Interfaces Pattern

**Patterns courants**:

**1. No parameters, returns nothing**:
```java
@FunctionalInterface
interface Action {
    void execute();
}

Action action = () -> System.out.println("Executing");
```

**2. One parameter, returns nothing**:
```java
@FunctionalInterface
interface Consumer<T> {
    void accept(T t);
}

Consumer<String> printer = s -> System.out.println(s);
```

**3. One parameter, returns value**:
```java
@FunctionalInterface
interface Transformer<T, R> {
    R transform(T t);
}

Transformer<String, Integer> length = s -> s.length();
```

**4. Two parameters, returns value**:
```java
@FunctionalInterface
interface Combiner<T> {
    T combine(T t1, T t2);
}

Combiner<Integer> adder = (a, b) -> a + b;
```

## Using Method References

### Method Reference Basics

Une **method reference** est un raccourci pour une lambda qui **appelle simplement une méthode**.

**Syntaxe**: `ClassName::methodName` ou `instance::methodName`

**Équivalences**:
```java
// Lambda
x -> System.out.println(x)
// Method reference
System.out::println

// Lambda
(s1, s2) -> s1.compareToIgnoreCase(s2)
// Method reference
String::compareToIgnoreCase

// Lambda
() -> new ArrayList<>()
// Method reference
ArrayList::new
```

### Types of Method References

Java a **4 types** de method references:

#### 1. Static Method Reference

**Syntaxe**: `ClassName::staticMethod`

```java
// Lambda
Function<String, Integer> parser = s -> Integer.parseInt(s);
// Method reference
Function<String, Integer> parser = Integer::parseInt;

// Utilisation
int number = parser.apply("123");  // 123
```

**Exemples**:
```java
// Math.abs
Function<Integer, Integer> absoluteValue = Math::abs;
System.out.println(absoluteValue.apply(-5));  // 5

// Math.pow (deux paramètres)
BiFunction<Double, Double, Double> power = Math::pow;
System.out.println(power.apply(2.0, 3.0));    // 8.0
```

#### 2. Instance Method Reference on Particular Object

**Syntaxe**: `instance::instanceMethod`

```java
String prefix = "Hello ";

// Lambda
Function<String, String> greeter = s -> prefix.concat(s);
// Method reference
Function<String, String> greeter = prefix::concat;

System.out.println(greeter.apply("World"));  // "Hello World"
```

**Exemples**:
```java
List<String> list = Arrays.asList("a", "b", "c");

// Lambda
list.forEach(s -> System.out.println(s));
// Method reference
list.forEach(System.out::println);

// StringBuilder
StringBuilder sb = new StringBuilder("Start");
Consumer<String> appender = sb::append;
appender.accept(" Middle");
appender.accept(" End");
System.out.println(sb);  // "Start Middle End"
```

#### 3. Instance Method Reference on Parameter

**Syntaxe**: `ClassName::instanceMethod`

Le premier paramètre de la lambda devient l'objet sur lequel la méthode est appelée.

```java
// Lambda
Comparator<String> comp = (s1, s2) -> s1.compareToIgnoreCase(s2);
// Method reference
Comparator<String> comp = String::compareToIgnoreCase;

// s1 devient l'instance, s2 devient le paramètre
```

**Exemples**:
```java
// Lambda: s -> s.length()
Function<String, Integer> lengthFn = String::length;
System.out.println(lengthFn.apply("Hello"));  // 5

// Lambda: s -> s.toUpperCase()
Function<String, String> upperFn = String::toUpperCase;
System.out.println(upperFn.apply("hello"));   // "HELLO"

// Lambda: (list, item) -> list.add(item)
BiFunction<List<String>, String, Boolean> addFn = List::add;
List<String> names = new ArrayList<>();
addFn.apply(names, "Alice");
System.out.println(names);  // [Alice]
```

#### 4. Constructor Reference

**Syntaxe**: `ClassName::new`

```java
// Lambda
Supplier<List<String>> listSupplier = () -> new ArrayList<>();
// Constructor reference
Supplier<List<String>> listSupplier = ArrayList::new;

List<String> list = listSupplier.get();
```

**Avec paramètres**:
```java
// Lambda: s -> new StringBuilder(s)
Function<String, StringBuilder> builderFn = StringBuilder::new;
StringBuilder sb = builderFn.apply("Hello");

// Lambda: (capacity) -> new ArrayList<>(capacity)
Function<Integer, List<String>> listFn = ArrayList::new;
List<String> list = listFn.apply(100);
```

**Array constructor reference**:
```java
// Lambda: size -> new String[size]
Function<Integer, String[]> arrayCreator = String[]::new;
String[] array = arrayCreator.apply(5);  // String[5]
```

### Method Reference Rules

**Règle 1**: La signature doit correspondre
```java
// OK: String::length correspond à Function<String, Integer>
Function<String, Integer> fn = String::length;

// ERREUR: String::charAt nécessite un paramètre int
// Function<String, Character> fn = String::charAt;
BiFunction<String, Integer, Character> fn = String::charAt;  // OK
```

**Règle 2**: Le nombre de paramètres doit correspondre
```java
// Consumer<T> prend 1 paramètre
Consumer<String> c = System.out::println;  // OK

// Supplier<T> ne prend aucun paramètre
// Supplier<String> s = String::length;    // ERREUR
Supplier<String> s = String::new;          // OK (constructor sans paramètre)
```

## Working with Built-in Functional Interfaces

Java fournit des functional interfaces dans `java.util.function`.

### Supplier<T>

**Signature**: `T get()`

Ne prend **aucun paramètre**, retourne une valeur.

```java
import java.util.function.Supplier;

Supplier<String> supplier1 = () -> "Hello";
Supplier<Integer> supplier2 = () -> 42;
Supplier<Double> supplier3 = Math::random;

System.out.println(supplier1.get());  // "Hello"
System.out.println(supplier2.get());  // 42
System.out.println(supplier3.get());  // nombre aléatoire
```

**Cas d'usage**: Lazy initialization, factory methods
```java
public static <T> T getOrDefault(Supplier<T> supplier, T defaultValue) {
    T value = supplier.get();
    return value != null ? value : defaultValue;
}
```

### Consumer<T>

**Signature**: `void accept(T t)`

Prend **un paramètre**, ne retourne **rien**.

```java
import java.util.function.Consumer;

Consumer<String> printer = s -> System.out.println(s);
Consumer<String> upperPrinter = s -> System.out.println(s.toUpperCase());
Consumer<List<String>> listClearer = list -> list.clear();

printer.accept("Hello");           // "Hello"
upperPrinter.accept("Hello");      // "HELLO"

List<String> list = new ArrayList<>(Arrays.asList("a", "b"));
listClearer.accept(list);
System.out.println(list.isEmpty()); // true
```

**Method chaining avec andThen()**:
```java
Consumer<String> c1 = s -> System.out.print(s);
Consumer<String> c2 = s -> System.out.print(s.toUpperCase());

Consumer<String> combined = c1.andThen(c2);
combined.accept("Hello");  // "HelloHELLO"
```

### BiConsumer<T, U>

**Signature**: `void accept(T t, U u)`

Prend **deux paramètres**, ne retourne **rien**.

```java
import java.util.function.BiConsumer;

BiConsumer<String, Integer> printer = (s, i) -> 
    System.out.println(s + ": " + i);

BiConsumer<Map<String, Integer>, String> remover = (map, key) -> 
    map.remove(key);

printer.accept("Age", 25);  // "Age: 25"

Map<String, Integer> map = new HashMap<>();
map.put("a", 1);
remover.accept(map, "a");
System.out.println(map.isEmpty());  // true
```

### Predicate<T>

**Signature**: `boolean test(T t)`

Prend **un paramètre**, retourne **boolean**.

```java
import java.util.function.Predicate;

Predicate<String> isEmpty = s -> s.isEmpty();
Predicate<Integer> isPositive = n -> n > 0;
Predicate<String> startsWithA = s -> s.startsWith("A");

System.out.println(isEmpty.test(""));       // true
System.out.println(isEmpty.test("Hello")); // false
System.out.println(isPositive.test(5));    // true
System.out.println(isPositive.test(-5));   // false
```

**Logical operations**:
```java
Predicate<String> startsWithA = s -> s.startsWith("A");
Predicate<String> endsWithZ = s -> s.endsWith("z");

// AND
Predicate<String> startsWithAAndEndsWithZ = startsWithA.and(endsWithZ);
System.out.println(startsWithAAndEndsWithZ.test("Az"));    // true
System.out.println(startsWithAAndEndsWithZ.test("Alice")); // false

// OR
Predicate<String> startsWithAOrEndsWithZ = startsWithA.or(endsWithZ);
System.out.println(startsWithAOrEndsWithZ.test("Alice"));  // true
System.out.println(startsWithAOrEndsWithZ.test("Buzz"));   // true

// NEGATE
Predicate<String> notStartsWithA = startsWithA.negate();
System.out.println(notStartsWithA.test("Alice"));  // false
System.out.println(notStartsWithA.test("Bob"));    // true
```

### BiPredicate<T, U>

**Signature**: `boolean test(T t, U u)`

Prend **deux paramètres**, retourne **boolean**.

```java
import java.util.function.BiPredicate;

BiPredicate<String, String> equals = (s1, s2) -> s1.equals(s2);
BiPredicate<String, Integer> hasLength = (s, len) -> s.length() == len;

System.out.println(equals.test("Hello", "Hello"));  // true
System.out.println(equals.test("Hello", "World"));  // false
System.out.println(hasLength.test("Hello", 5));     // true
```

### Function<T, R>

**Signature**: `R apply(T t)`

Prend **un paramètre** de type T, retourne une valeur de type R.

```java
import java.util.function.Function;

Function<String, Integer> length = s -> s.length();
Function<Integer, Integer> square = n -> n * n;
Function<String, String> upper = s -> s.toUpperCase();

System.out.println(length.apply("Hello"));    // 5
System.out.println(square.apply(5));          // 25
System.out.println(upper.apply("hello"));     // "HELLO"
```

**Method chaining avec andThen() et compose()**:
```java
Function<Integer, Integer> multiplyBy2 = n -> n * 2;
Function<Integer, Integer> add10 = n -> n + 10;

// andThen: f.andThen(g) = g(f(x))
Function<Integer, Integer> combined1 = multiplyBy2.andThen(add10);
System.out.println(combined1.apply(5));  // (5 * 2) + 10 = 20

// compose: f.compose(g) = f(g(x))
Function<Integer, Integer> combined2 = multiplyBy2.compose(add10);
System.out.println(combined2.apply(5));  // (5 + 10) * 2 = 30
```

### BiFunction<T, U, R>

**Signature**: `R apply(T t, U u)`

Prend **deux paramètres**, retourne une valeur.

```java
import java.util.function.BiFunction;

BiFunction<Integer, Integer, Integer> add = (a, b) -> a + b;
BiFunction<String, String, String> concat = (s1, s2) -> s1 + s2;
BiFunction<String, Integer, String> repeat = (s, n) -> s.repeat(n);

System.out.println(add.apply(5, 3));            // 8
System.out.println(concat.apply("Hello", "World"));  // "HelloWorld"
System.out.println(repeat.apply("Ha", 3));      // "HaHaHa"
```

### UnaryOperator<T>

**Signature**: `T apply(T t)` (extends Function<T, T>)

Cas spécial de Function où l'entrée et la sortie sont du **même type**.

```java
import java.util.function.UnaryOperator;

UnaryOperator<Integer> square = n -> n * n;
UnaryOperator<String> upper = s -> s.toUpperCase();
UnaryOperator<String> reverse = s -> new StringBuilder(s).reverse().toString();

System.out.println(square.apply(5));        // 25
System.out.println(upper.apply("hello"));   // "HELLO"
System.out.println(reverse.apply("Hello")); // "olleH"
```

### BinaryOperator<T>

**Signature**: `T apply(T t1, T t2)` (extends BiFunction<T, T, T>)

Cas spécial de BiFunction où les **deux entrées et la sortie** sont du même type.

```java
import java.util.function.BinaryOperator;

BinaryOperator<Integer> max = (a, b) -> Math.max(a, b);
BinaryOperator<String> concat = (s1, s2) -> s1 + s2;
BinaryOperator<Integer> multiply = (a, b) -> a * b;

System.out.println(max.apply(5, 10));           // 10
System.out.println(concat.apply("Hello", "World"));  // "HelloWorld"
System.out.println(multiply.apply(5, 3));       // 15
```

### Summary of Built-in Functional Interfaces

| Interface | Parameters | Return Type | Method | Use Case |
|-----------|------------|-------------|--------|----------|
| `Supplier<T>` | 0 | T | `get()` | Factory, lazy init |
| `Consumer<T>` | 1 (T) | void | `accept(T)` | Side effects |
| `BiConsumer<T,U>` | 2 (T, U) | void | `accept(T,U)` | Side effects |
| `Predicate<T>` | 1 (T) | boolean | `test(T)` | Filtering |
| `BiPredicate<T,U>` | 2 (T, U) | boolean | `test(T,U)` | Filtering |
| `Function<T,R>` | 1 (T) | R | `apply(T)` | Transformation |
| `BiFunction<T,U,R>` | 2 (T, U) | R | `apply(T,U)` | Transformation |
| `UnaryOperator<T>` | 1 (T) | T | `apply(T)` | Same type transform |
| `BinaryOperator<T>` | 2 (T, T) | T | `apply(T,T)` | Same type combine |

### Primitive Functional Interfaces

Pour éviter l'autoboxing/unboxing, Java fournit des versions primitives.

**Exemples**:
- `IntSupplier`, `LongSupplier`, `DoubleSupplier`
- `IntConsumer`, `LongConsumer`, `DoubleConsumer`
- `IntPredicate`, `LongPredicate`, `DoublePredicate`
- `IntFunction<R>`, `LongFunction<R>`, `DoubleFunction<R>`
- `ToIntFunction<T>`, `ToLongFunction<T>`, `ToDoubleFunction<T>`
- `IntUnaryOperator`, `LongUnaryOperator`, `DoubleUnaryOperator`
- `IntBinaryOperator`, `LongBinaryOperator`, `DoubleBinaryOperator`

```java
import java.util.function.*;

// Évite l'autoboxing
IntSupplier randomInt = () -> (int)(Math.random() * 100);
IntConsumer printer = i -> System.out.println(i);
IntPredicate isEven = i -> i % 2 == 0;
IntUnaryOperator doubler = i -> i * 2;
IntBinaryOperator adder = (a, b) -> a + b;

System.out.println(randomInt.getAsInt());
printer.accept(42);
System.out.println(isEven.test(4));     // true
System.out.println(doubler.applyAsInt(5));  // 10
System.out.println(adder.applyAsInt(5, 3)); // 8
```

## Working with Variables in Lambdas

### Accessing Variables

Les lambdas peuvent accéder à différents types de variables:

#### 1. Instance Variables

```java
public class Example {
    private int x = 10;
    
    public void method() {
        Consumer<Integer> consumer = n -> System.out.println(x + n);
        consumer.accept(5);  // 15
    }
}
```

#### 2. Static Variables

```java
public class Example {
    private static int count = 0;
    
    public static void method() {
        Runnable r = () -> count++;
        r.run();
        System.out.println(count);  // 1
    }
}
```

#### 3. Method Parameters

```java
public void method(int param) {
    Consumer<Integer> consumer = n -> System.out.println(param + n);
    consumer.accept(5);  // param + 5
}
```

#### 4. Local Variables (Effectively Final)

Les lambdas peuvent accéder aux variables locales **final** ou **effectively final**.

**Effectively final**: variable qui n'est jamais modifiée après son initialisation.

```java
public void method() {
    int x = 10;  // Effectively final
    
    Consumer<Integer> consumer = n -> System.out.println(x + n);
    consumer.accept(5);  // 15
    
    // x = 20;  // ERREUR: rend x non-effectively final
}
```

**Erreur si la variable est modifiée**:
```java
public void method() {
    int x = 10;
    x = 20;  // Modifié
    
    // Consumer<Integer> c = n -> System.out.println(x + n);  // ERREUR
}
```

### Why Effectively Final?

**Raison**: Les lambdas peuvent être exécutées **après** la fin de la méthode (ex: dans un thread). Si la variable locale pouvait changer, cela créerait des problèmes de cohérence.

```java
public Runnable createRunnable() {
    int x = 10;  // Effectively final
    return () -> System.out.println(x);  // Lambda capture x
}  // x n'existe plus, mais la lambda garde sa valeur

Runnable r = createRunnable();
r.run();  // Affiche 10
```

### Variable Scope

```java
public class VariableScope {
    private int instanceVar = 10;
    private static int staticVar = 20;
    
    public void method(int parameter) {
        int localVar = 30;
        
        Consumer<Integer> consumer = n -> {
            // Accès à toutes les variables
            System.out.println(instanceVar);  // OK
            System.out.println(staticVar);    // OK
            System.out.println(parameter);    // OK (effectively final)
            System.out.println(localVar);     // OK (effectively final)
            System.out.println(n);            // OK (paramètre lambda)
            
            // Modification
            // instanceVar++;     // OK (instance variable)
            // staticVar++;       // OK (static variable)
            // parameter++;       // ERREUR (parameter effectively final)
            // localVar++;        // ERREUR (local variable effectively final)
            // n++;               // OK (paramètre lambda, variable locale à la lambda)
        };
    }
}
```

### Parameter Name Conflicts

Les paramètres de la lambda **ne peuvent pas** avoir le même nom que les variables locales.

```java
public void method() {
    int x = 10;
    
    // Consumer<Integer> c = x -> System.out.println(x);  // ERREUR: x déjà défini
    Consumer<Integer> c = y -> System.out.println(y + x);  // OK
}
```

---

## Exemples Pratiques Complets

### Exemple 1: Traitement de Liste avec Lambdas
```java
import java.util.*;
import java.util.function.*;

public class ListProcessor {
    
    public static List<String> filter(List<String> list, Predicate<String> predicate) {
        List<String> result = new ArrayList<>();
        for (String item : list) {
            if (predicate.test(item)) {
                result.add(item);
            }
        }
        return result;
    }
    
    public static List<String> transform(List<String> list, Function<String, String> function) {
        List<String> result = new ArrayList<>();
        for (String item : list) {
            result.add(function.apply(item));
        }
        return result;
    }
    
    public static void process(List<String> list, Consumer<String> consumer) {
        for (String item : list) {
            consumer.accept(item);
        }
    }
    
    public static void main(String[] args) {
        List<String> names = Arrays.asList("Alice", "Bob", "Charlie", "David", "Eve");
        
        // Filtrer les noms qui commencent par 'A' ou 'C'
        Predicate<String> startsWithAOrC = s -> s.startsWith("A") || s.startsWith("C");
        List<String> filtered = filter(names, startsWithAOrC);
        System.out.println("Filtrés: " + filtered);  // [Alice, Charlie]
        
        // Transformer en majuscules
        Function<String, String> toUpper = String::toUpperCase;
        List<String> transformed = transform(filtered, toUpper);
        System.out.println("Transformés: " + transformed);  // [ALICE, CHARLIE]
        
        // Afficher avec préfixe
        Consumer<String> printer = s -> System.out.println(">> " + s);
        process(transformed, printer);
        
        // Chaînage de prédicats
        Predicate<String> longerThan3 = s -> s.length() > 3;
        Predicate<String> combined = startsWithAOrC.and(longerThan3);
        System.out.println("\nFiltre combiné: " + filter(names, combined));  // [Alice, Charlie]
    }
}
```

### Exemple 2: Calculateur avec Functional Interfaces
```java
import java.util.function.*;

public class Calculator {
    
    // Map d'opérations
    private Map<String, BinaryOperator<Double>> operations = new HashMap<>();
    
    public Calculator() {
        // Initialiser les opérations
        operations.put("+", (a, b) -> a + b);
        operations.put("-", (a, b) -> a - b);
        operations.put("*", (a, b) -> a * b);
        operations.put("/", (a, b) -> b != 0 ? a / b : Double.NaN);
        operations.put("pow", Math::pow);
        operations.put("max", Math::max);
        operations.put("min", Math::min);
    }
    
    public double calculate(String operation, double a, double b) {
        BinaryOperator<Double> op = operations.get(operation);
        if (op == null) {
            throw new IllegalArgumentException("Opération inconnue: " + operation);
        }
        return op.apply(a, b);
    }
    
    // Ajouter une opération custom
    public void addOperation(String name, BinaryOperator<Double> operation) {
        operations.put(name, operation);
    }
    
    // Fonctions unaires
    public double applyUnary(UnaryOperator<Double> operator, double value) {
        return operator.apply(value);
    }
    
    public static void main(String[] args) {
        Calculator calc = new Calculator();
        
        System.out.println("5 + 3 = " + calc.calculate("+", 5, 3));
        System.out.println("5 * 3 = " + calc.calculate("*", 5, 3));
        System.out.println("2^3 = " + calc.calculate("pow", 2, 3));
        System.out.println("max(5,3) = " + calc.calculate("max", 5, 3));
        
        // Opération custom
        calc.addOperation("avg", (a, b) -> (a + b) / 2);
        System.out.println("avg(5,3) = " + calc.calculate("avg", 5, 3));
        
        // Opérations unaires
        UnaryOperator<Double> square = x -> x * x;
        UnaryOperator<Double> sqrt = Math::sqrt;
        UnaryOperator<Double> negate = x -> -x;
        
        System.out.println("\n=== OPÉRATIONS UNAIRES ===");
        System.out.println("square(5) = " + calc.applyUnary(square, 5.0));
        System.out.println("sqrt(16) = " + calc.applyUnary(sqrt, 16.0));
        System.out.println("negate(5) = " + calc.applyUnary(negate, 5.0));
        
        // Composition de fonctions
        UnaryOperator<Double> squareThenSqrt = square.andThen(sqrt);
        System.out.println("sqrt(square(5)) = " + calc.applyUnary(squareThenSqrt, 5.0));
    }
}
```

### Exemple 3: Validation avec Predicates
```java
import java.util.function.*;
import java.util.*;

public class ValidationSystem {
    
    // Validateurs prédéfinis
    public static class Validators {
        public static Predicate<String> notNull = s -> s != null;
        public static Predicate<String> notEmpty = s -> !s.isEmpty();
        public static Predicate<String> minLength(int min) {
            return s -> s.length() >= min;
        }
        public static Predicate<String> maxLength(int max) {
            return s -> s.length() <= max;
        }
        public static Predicate<String> matches(String regex) {
            return s -> s.matches(regex);
        }
        public static Predicate<Integer> inRange(int min, int max) {
            return n -> n >= min && n <= max;
        }
    }
    
    public static class ValidationResult {
        private boolean valid;
        private List<String> errors;
        
        public ValidationResult() {
            this.valid = true;
            this.errors = new ArrayList<>();
        }
        
        public void addError(String error) {
            this.valid = false;
            this.errors.add(error);
        }
        
        public boolean isValid() { return valid; }
        public List<String> getErrors() { return errors; }
    }
    
    public static ValidationResult validate(String value, String fieldName, 
                                           List<Predicate<String>> validators,
                                           List<String> errorMessages) {
        ValidationResult result = new ValidationResult();
        
        for (int i = 0; i < validators.size(); i++) {
            if (!validators.get(i).test(value)) {
                result.addError(fieldName + ": " + errorMessages.get(i));
            }
        }
        
        return result;
    }
    
    public static void main(String[] args) {
        // Validation d'un email
        String email = "test@example";
        
        List<Predicate<String>> emailValidators = Arrays.asList(
            Validators.notNull,
            Validators.notEmpty,
            Validators.minLength(5),
            s -> s.contains("@"),
            s -> s.contains(".")
        );
        
        List<String> emailErrors = Arrays.asList(
            "ne peut pas être null",
            "ne peut pas être vide",
            "doit avoir au moins 5 caractères",
            "doit contenir @",
            "doit contenir un point"
        );
        
        ValidationResult emailResult = validate(email, "Email", emailValidators, emailErrors);
        
        System.out.println("=== VALIDATION EMAIL ===");
        System.out.println("Email: " + email);
        System.out.println("Valide: " + emailResult.isValid());
        if (!emailResult.isValid()) {
            System.out.println("Erreurs:");
            emailResult.getErrors().forEach(e -> System.out.println("  - " + e));
        }
        
        // Validation d'un mot de passe
        String password = "Pass1";
        
        Predicate<String> passwordValidator = Validators.notNull
            .and(Validators.minLength(8))
            .and(s -> s.matches(".*[A-Z].*"))      // Au moins une majuscule
            .and(s -> s.matches(".*[a-z].*"))      // Au moins une minuscule
            .and(s -> s.matches(".*\\d.*"))         // Au moins un chiffre
            .and(s -> s.matches(".*[!@#$%^&*].*")); // Au moins un caractère spécial
        
        System.out.println("\n=== VALIDATION MOT DE PASSE ===");
        System.out.println("Mot de passe: " + password);
        System.out.println("Valide: " + passwordValidator.test(password));
        
        // Validation d'un âge
        Integer age = 150;
        Predicate<Integer> ageValidator = Validators.inRange(0, 120);
        
        System.out.println("\n=== VALIDATION ÂGE ===");
        System.out.println("Âge: " + age);
        System.out.println("Valide: " + ageValidator.test(age));
    }
}
```

### Exemple 4: Pipeline de Transformation avec Functions
```java
import java.util.function.*;
import java.util.*;

public class TransformationPipeline {
    
    public static class Pipeline<T> {
        private T value;
        
        private Pipeline(T value) {
            this.value = value;
        }
        
        public static <T> Pipeline<T> of(T value) {
            return new Pipeline<>(value);
        }
        
        public <R> Pipeline<R> map(Function<T, R> mapper) {
            return new Pipeline<>(mapper.apply(value));
        }
        
        public Pipeline<T> filter(Predicate<T> predicate) {
            if (!predicate.test(value)) {
                throw new IllegalStateException("Filter failed");
            }
            return this;
        }
        
        public Pipeline<T> peek(Consumer<T> action) {
            action.accept(value);
            return this;
        }
        
        public T get() {
            return value;
        }
    }
    
    public static void main(String[] args) {
        // Pipeline de transformation de String
        String result = Pipeline.of("  hello world  ")
            .peek(s -> System.out.println("Input: '" + s + "'"))
            .map(String::trim)
            .peek(s -> System.out.println("After trim: '" + s + "'"))
            .map(String::toUpperCase)
            .peek(s -> System.out.println("After upper: '" + s + "'"))
            .map(s -> s.replace(" ", "_"))
            .peek(s -> System.out.println("After replace: '" + s + "'"))
            .get();
        
        System.out.println("Final result: " + result);
        
        // Pipeline de transformation numérique
        System.out.println("\n=== PIPELINE NUMÉRIQUE ===");
        
        Integer number = Pipeline.of(5)
            .peek(n -> System.out.println("Start: " + n))
            .map(n -> n * 2)
            .peek(n -> System.out.println("After *2: " + n))
            .map(n -> n + 10)
            .peek(n -> System.out.println("After +10: " + n))
            .map(n -> n * n)
            .peek(n -> System.out.println("After square: " + n))
            .get();
        
        System.out.println("Final: " + number);
        
        // Pipeline avec filtrage
        System.out.println("\n=== PIPELINE AVEC FILTRE ===");
        
        try {
            String validated = Pipeline.of("password123")
                .filter(s -> s.length() >= 8)
                .peek(s -> System.out.println("✓ Length OK"))
                .filter(s -> s.matches(".*\\d.*"))
                .peek(s -> System.out.println("✓ Contains digit"))
                .filter(s -> s.matches(".*[A-Z].*"))  // Va échouer
                .get();
        } catch (IllegalStateException e) {
            System.out.println("✗ Validation failed");
        }
    }
}
```

### Exemple 5: Factory Pattern avec Suppliers
```java
import java.util.function.*;
import java.util.*;

public class ObjectFactory {
    
    // Cache des factories
    private Map<String, Supplier<?>> factories = new HashMap<>();
    
    // Enregistrer une factory
    public <T> void register(String name, Supplier<T> factory) {
        factories.put(name, factory);
    }
    
    // Créer un objet
    @SuppressWarnings("unchecked")
    public <T> T create(String name) {
        Supplier<?> factory = factories.get(name);
        if (factory == null) {
            throw new IllegalArgumentException("Unknown factory: " + name);
        }
        return (T) factory.get();
    }
    
    // Créer plusieurs instances
    public <T> List<T> createMultiple(String name, int count) {
        List<T> list = new ArrayList<>();
        for (int i = 0; i < count; i++) {
            list.add(create(name));
        }
        return list;
    }
    
    public static void main(String[] args) {
        ObjectFactory factory = new ObjectFactory();
        
        // Enregistrer des factories
        factory.register("list", ArrayList::new);
        factory.register("set", HashSet::new);
        factory.register("map", HashMap::new);
        factory.register("random", Math::random);
        factory.register("uuid", () -> UUID.randomUUID().toString());
        
        // Factory avec configuration
        factory.register("list-10", () -> {
            List<Integer> list = new ArrayList<>();
            for (int i = 0; i < 10; i++) {
                list.add(i);
            }
            return list;
        });
        
        // Créer des objets
        List<String> list = factory.create("list");
        System.out.println("List créée: " + list.getClass().getSimpleName());
        
        Set<String> set = factory.create("set");
        System.out.println("Set créé: " + set.getClass().getSimpleName());
        
        Double random = factory.create("random");
        System.out.println("Random: " + random);
        
        String uuid = factory.create("uuid");
        System.out.println("UUID: " + uuid);
        
        List<Integer> initializedList = factory.create("list-10");
        System.out.println("List initialisée: " + initializedList);
        
        // Créer plusieurs UUIDs
        List<String> uuids = factory.createMultiple("uuid", 3);
        System.out.println("\n=== 3 UUIDs ===");
        uuids.forEach(System.out::println);
        
        // Lazy initialization avec Supplier
        System.out.println("\n=== LAZY INITIALIZATION ===");
        Supplier<List<String>> lazyList = () -> {
            System.out.println("Création de la liste...");
            return new ArrayList<>();
        };
        
        System.out.println("Supplier créé (pas encore exécuté)");
        List<String> actualList = lazyList.get();  // Création ici
        System.out.println("Liste créée: " + actualList);
    }
}
```

### Exemple 6: Method References Comparisons
```java
import java.util.*;
import java.util.function.*;

public class MethodReferencesDemo {
    
    public static void main(String[] args) {
        List<String> names = new ArrayList<>(Arrays.asList("Charlie", "Alice", "Bob", "David"));
        
        System.out.println("=== COMPARAISONS ===");
        System.out.println("Liste originale: " + names);
        
        // 1. Static method reference
        names.sort(String::compareToIgnoreCase);
        System.out.println("Triée (case insensitive): " + names);
        
        // 2. Instance method reference on particular object
        String prefix = ">>> ";
        Consumer<String> prefixPrinter = prefix::concat;
        System.out.println("\n=== AVEC PRÉFIXE ===");
        names.forEach(name -> System.out.println(prefixPrinter.apply(name)));
        
        // 3. Instance method reference on parameter
        List<Integer> lengths = new ArrayList<>();
        Function<String, Integer> lengthFn = String::length;
        names.forEach(name -> lengths.add(lengthFn.apply(name)));
        System.out.println("\n=== LONGUEURS ===");
        System.out.println("Noms: " + names);
        System.out.println("Longueurs: " + lengths);
        
        // 4. Constructor reference
        Supplier<List<String>> listSupplier = ArrayList::new;
        Function<Integer, List<String>> listWithCapacity = ArrayList::new;
        Function<String, StringBuilder> sbCreator = StringBuilder::new;
        
        List<String> newList1 = listSupplier.get();
        List<String> newList2 = listWithCapacity.apply(100);
        StringBuilder sb = sbCreator.apply("Initial");
        
        System.out.println("\n=== CONSTRUCTORS ===");
        System.out.println("newList1: " + newList1.getClass().getSimpleName());
        System.out.println("newList2: " + newList2.getClass().getSimpleName());
        System.out.println("StringBuilder: " + sb);
        
        // Exemples pratiques
        System.out.println("\n=== EXEMPLES PRATIQUES ===");
        
        List<String> words = Arrays.asList("java", "lambda", "stream", "api");
        
        // forEach avec method reference
        words.forEach(System.out::println);
        
        // map avec method reference
        List<Integer> wordLengths = new ArrayList<>();
        words.stream()
             .map(String::length)
             .forEach(wordLengths::add);
        System.out.println("\nLongueurs: " + wordLengths);
        
        // removeIf avec method reference
        List<String> filterable = new ArrayList<>(words);
        Predicate<String> isEmpty = String::isEmpty;
        filterable.removeIf(s -> s.length() < 4);
        System.out.println("Filtrée (length >= 4): " + filterable);
        
        // replaceAll avec method reference
        List<String> upperWords = new ArrayList<>(words);
        upperWords.replaceAll(String::toUpperCase);
        System.out.println("En majuscules: " + upperWords);
    }
}
```

### Exemple 7: Effectively Final Variables
```java
import java.util.function.*;

public class EffectivelyFinalDemo {
    
    private int instanceVar = 100;
    private static int staticVar = 200;
    
    public void demonstrateScopes() {
        final int finalVar = 10;
        int effectivelyFinalVar = 20;
        int modifiedVar = 30;
        
        System.out.println("=== ACCÈS AUX VARIABLES ===");
        
        // Lambda avec accès à différentes variables
        Consumer<Integer> consumer1 = n -> {
            System.out.println("Instance var: " + instanceVar);
            System.out.println("Static var: " + staticVar);
            System.out.println("Final var: " + finalVar);
            System.out.println("Effectively final var: " + effectivelyFinalVar);
            System.out.println("Parameter: " + n);
            
            // Modifications possibles
            instanceVar++;  // OK
            staticVar++;    // OK
            // finalVar++;  // ERREUR: final
            // effectivelyFinalVar++;  // ERREUR: effectively final
            // n++;  // OK (paramètre de la lambda, pas effectively final)
        };
        
        consumer1.accept(50);
        
        // Cette ligne rend modifiedVar non-effectively final
        modifiedVar = 40;
        
        // Consumer<Integer> consumer2 = n -> {
        //     System.out.println(modifiedVar);  // ERREUR: pas effectively final
        // };
        
        // Contournement: utiliser un tableau ou wrapper
        int[] wrapper = {modifiedVar};
        Consumer<Integer> consumer3 = n -> {
            System.out.println("\nWrapper value: " + wrapper[0]);
            wrapper[0]++;  // Modification possible
            System.out.println("Modified wrapper: " + wrapper[0]);
        };
        
        consumer3.accept(0);
        System.out.println("Final wrapper value: " + wrapper[0]);
    }
    
    public Runnable createRunnableWithCapture() {
        int localVar = 42;
        String localString = "Captured";
        
        // Ces variables sont capturées par la lambda
        return () -> {
            System.out.println("\n=== VARIABLES CAPTURÉES ===");
            System.out.println("Local var: " + localVar);
            System.out.println("Local string: " + localString);
        };
    }  // localVar et localString n'existent plus ici
    
    public static void demonstrateParameterConflict() {
        int x = 10;
        
        // Consumer<Integer> c1 = x -> System.out.println(x);  // ERREUR: x déjà défini
        Consumer<Integer> c1 = y -> System.out.println(y + x);  // OK
        
        c1.accept(5);
    }
    
    public static void main(String[] args) {
        EffectivelyFinalDemo demo = new EffectivelyFinalDemo();
        
        demo.demonstrateScopes();
        
        // Les variables capturées persistent
        Runnable r = demo.createRunnableWithCapture();
        r.run();  // Fonctionne même si la méthode est terminée
        
        demonstrateParameterConflict();
    }
}
```

---

## Points Clés du Chapter 8

✅ **Lambda Syntax**:
   - `(params) -> expression` ou `(params) -> { statements; }`
   - Parenthèses optionnelles pour 1 paramètre
   - Types optionnels (inférence)
   - Return implicite pour expression simple

✅ **Functional Interface**:
   - **Exactement 1** méthode abstraite
   - `@FunctionalInterface` annotation recommandée
   - Default et static methods ne comptent pas
   - Nécessaire pour utiliser les lambdas

✅ **Method References** (4 types):
   - **Static**: `ClassName::staticMethod`
   - **Instance (particular)**: `instance::instanceMethod`
   - **Instance (parameter)**: `ClassName::instanceMethod`
   - **Constructor**: `ClassName::new`

✅ **Built-in Functional Interfaces**:
   - **Supplier<T>**: `() -> T`
   - **Consumer<T>**: `T -> void`
   - **Predicate<T>**: `T -> boolean`
   - **Function<T,R>**: `T -> R`
   - **UnaryOperator<T>**: `T -> T`
   - **BinaryOperator<T>**: `(T,T) -> T`
   - Versions Bi- pour 2 paramètres

✅ **Variables in Lambdas**:
   - Accès aux instance/static variables: ✅
   - Accès aux local variables: ✅ (si **effectively final**)
   - Modification local variables: ❌
   - Modification instance/static: ✅

✅ **Effectively Final**:
   - Variable jamais modifiée après initialisation
   - Nécessaire pour capture par lambda
   - Vérifié à la compilation

✅ **Method Chaining**:
   - `andThen()`, `compose()` (Function)
   - `and()`, `or()`, `negate()` (Predicate)
   - `andThen()` (Consumer)

✅ **Primitive Versions**:
   - `IntSupplier`, `IntConsumer`, `IntPredicate`, etc.
   - Évitent l'autoboxing/unboxing
   - Meilleure performance

---

# Questions/Réponses - Chapter 8: Lambdas and Functional Interfaces

## Questions sur la Syntaxe des Lambdas

**Q1: Lesquelles de ces lambdas sont valides?**
```java
// A
x -> x * 2

// B
(x) -> x * 2

// C
(int x) -> x * 2

// D
x, y -> x + y

// E
(x, y) -> x + y
```

**R:**
- A: ✅ Valide (1 paramètre sans parenthèses)
- B: ✅ Valide (1 paramètre avec parenthèses)
- C: ✅ Valide (type explicite)
- D: ❌ **Invalide** (parenthèses requises pour plusieurs paramètres)
- E: ✅ Valide

---

**Q2: Ce code compile-t-il?**
```java
Function<Integer, Integer> fn = x -> { x * 2 };
```

**R:** Non, **erreur de compilation**: "missing return statement".

Dans un **bloc** `{}`, le `return` est **explicite** si la méthode retourne une valeur.

**Correct**:
```java
Function<Integer, Integer> fn = x -> { return x * 2; };
// OU
Function<Integer, Integer> fn = x -> x * 2;  // Sans bloc
```

---

**Q3: Ce code compile-t-il?**
```java
Consumer<String> c = s -> System.out.println(s);
```

**R:** Oui, compile.

Pour les méthodes **void**, pas besoin de `return`. L'expression est un statement complet.

---

**Q4: Quelle est l'erreur?**
```java
BiFunction<Integer, Integer, Integer> fn = (x, int y) -> x + y;
```

**R:** **Erreur de compilation**: "illegal start of type".

Si on spécifie le type d'un paramètre, on doit spécifier le type de **tous** les paramètres.

**Correct**:
```java
BiFunction<Integer, Integer, Integer> fn = (int x, int y) -> x + y;
// OU
BiFunction<Integer, Integer, Integer> fn = (x, y) -> x + y;  // Types inférés
```

---

**Q5: Ce code compile-t-il?**
```java
Runnable r = () -> { System.out.println("Hello") };
```

**R:** Non, **erreur de compilation**: "';' expected".

Les statements dans un bloc doivent se terminer par un **point-virgule**.

**Correct**:
```java
Runnable r = () -> { System.out.println("Hello"); };
// OU
Runnable r = () -> System.out.println("Hello");  // Sans bloc
```

---

**Q6: Quelle est la différence?**
```java
// Version 1
Function<Integer, Integer> fn1 = x -> x * 2;

// Version 2
Function<Integer, Integer> fn2 = (Integer x) -> x * 2;
```

**R:** **Aucune différence fonctionnelle**.

- Version 1: types **inférés** par le compilateur
- Version 2: types **explicites**

Les deux sont équivalentes. La version 1 est plus concise et généralement préférée.

---

**Q7: Ce code compile-t-il?**
```java
Supplier<Integer> s = () -> return 42;
```

**R:** Non, **erreur de compilation**.

Sans bloc `{}`, le `return` est **implicite**. On ne doit pas l'écrire.

**Correct**:
```java
Supplier<Integer> s = () -> 42;
// OU
Supplier<Integer> s = () -> { return 42; };  // Avec bloc
```

---

## Questions sur les Functional Interfaces

**Q8: Ce code compile-t-il?**
```java
@FunctionalInterface
public interface Invalid {
    void method1();
    void method2();
}
```

**R:** Non, **erreur de compilation**: "Invalid is not a functional interface".

Une functional interface doit avoir **exactement 1** méthode abstraite.

---

**Q9: Ce code compile-t-il?**
```java
@FunctionalInterface
public interface Valid {
    void abstractMethod();
    
    default void defaultMethod() {
        System.out.println("Default");
    }
    
    static void staticMethod() {
        System.out.println("Static");
    }
}
```

**R:** Oui, compile.

Les **default** et **static** methods ne comptent **pas** pour la règle "1 méthode abstraite".

---

**Q10: Peut-on utiliser une lambda avec cette interface?**
```java
public interface Calculator {
    int calculate(int a, int b);
    int calculate(int a, int b, int c);
}
```

**R:** Non, **erreur de compilation** si on essaie.

Cette interface a **2 méthodes abstraites**, donc ce n'est **pas** une functional interface.

```java
// Calculator c = (a, b) -> a + b;  // ERREUR
```

---

**Q11: Quelle functional interface correspond à cette lambda?**
```java
x -> x.length()
```

**R:** `Function<String, Integer>` (ou `ToIntFunction<String>`).

- **Entrée**: String (x)
- **Sortie**: Integer (x.length())

```java
Function<String, Integer> fn = x -> x.length();
// OU
Function<String, Integer> fn = String::length;
```

---

**Q12: Quelle functional interface correspond?**
```java
() -> System.out.println("Hello")
```

**R:** `Runnable` (ou `Consumer<Void>` avec adaptation).

- **Pas de paramètre**
- **Pas de retour** (void)

```java
Runnable r = () -> System.out.println("Hello");
```

---

## Questions sur les Method References

**Q13: Quelle est la différence entre ces deux?**
```java
// Version 1
Function<String, Integer> fn1 = s -> s.length();

// Version 2
Function<String, Integer> fn2 = String::length;
```

**R:** **Aucune différence fonctionnelle**.

Les deux font exactement la même chose. La version 2 (method reference) est plus concise.

---

**Q14: Ce code compile-t-il?**
```java
Consumer<String> c = System.out::println;
c.accept("Hello");
```

**R:** Oui, compile et affiche `"Hello"`.

`System.out::println` est une **method reference** (instance method sur objet particulier).

---

**Q15: Quel type de method reference est-ce?**
```java
Function<String, Integer> fn = Integer::parseInt;
```

**R:** **Static method reference** (`ClassName::staticMethod`).

`parseInt` est une méthode **static** de la classe `Integer`.

---

**Q16: Quel type de method reference est-ce?**
```java
String prefix = "Hello ";
Function<String, String> fn = prefix::concat;
```

**R:** **Instance method reference on particular object** (`instance::instanceMethod`).

On appelle la méthode `concat` sur l'objet **spécifique** `prefix`.

---

**Q17: Quel type de method reference est-ce?**
```java
Comparator<String> comp = String::compareToIgnoreCase;
```

**R:** **Instance method reference on parameter** (`ClassName::instanceMethod`).

Équivalent à:
```java
(s1, s2) -> s1.compareToIgnoreCase(s2)
```

Le premier paramètre (`s1`) devient l'**instance**, le second (`s2`) devient le **paramètre**.

---

**Q18: Quel type de method reference est-ce?**
```java
Supplier<List<String>> supplier = ArrayList::new;
```

**R:** **Constructor reference** (`ClassName::new`).

Crée une nouvelle `ArrayList` via son constructor sans paramètres.

---

**Q19: Comment convertir cette lambda en method reference?**
```java
Function<String, String> fn = s -> s.toUpperCase();
```

**R:**
```java
Function<String, String> fn = String::toUpperCase;
```

C'est une instance method reference on parameter.

---

**Q20: Ce code compile-t-il?**
```java
Function<String, Integer> fn = String::length;
int length = fn.apply("Hello");
```

**R:** Oui, compile.

`String::length` correspond à `s -> s.length()`, donc compatible avec `Function<String, Integer>`.

---

## Questions sur les Built-in Functional Interfaces

**Q21: Que fait ce code?**
```java
Supplier<Double> supplier = Math::random;
System.out.println(supplier.get());
System.out.println(supplier.get());
```

**R:** Affiche **deux nombres aléatoires différents**.

Chaque appel à `get()` exécute `Math.random()` et génère un nouveau nombre.

---

**Q22: Quelle est la différence?**
```java
// Version 1
Consumer<String> c1 = s -> System.out.println(s);

// Version 2
Consumer<String> c2 = System.out::println;
```

**R:** **Aucune différence**.

Les deux affichent le paramètre. Version 2 utilise une method reference.

---

**Q23: Que retourne cette expression?**
```java
Predicate<String> isEmpty = s -> s.isEmpty();
Predicate<String> notEmpty = isEmpty.negate();

System.out.println(notEmpty.test("Hello"));
System.out.println(notEmpty.test(""));
```

**R:**
```
true   // "Hello" n'est pas vide
false  // "" est vide
```

`negate()` **inverse** le résultat du Predicate.

---

**Q24: Que s'affiche-t-il?**
```java
Predicate<String> startsA = s -> s.startsWith("A");
Predicate<String> endsZ = s -> s.endsWith("z");
Predicate<String> combined = startsA.and(endsZ);

System.out.println(combined.test("Az"));
System.out.println(combined.test("Alice"));
```

**R:**
```
true   // Commence par A ET finit par z
false  // Commence par A mais ne finit pas par z
```

`and()` combine deux Predicates avec un **ET logique**.

---

**Q25: Quelle est la différence entre Function et UnaryOperator?**
```java
Function<Integer, Integer> fn = x -> x * 2;
UnaryOperator<Integer> op = x -> x * 2;
```

**R:** **Sémantiquement**, pas de différence pour ce cas.

**Différence**:
- `Function<T, R>`: entrée type T, sortie type R (peuvent être différents)
- `UnaryOperator<T>`: entrée ET sortie du **même type** T

`UnaryOperator<T>` extends `Function<T, T>` (cas spécial).

---

**Q26: Que fait andThen()?**
```java
Function<Integer, Integer> multiply2 = x -> x * 2;
Function<Integer, Integer> add10 = x -> x + 10;

Function<Integer, Integer> combined = multiply2.andThen(add10);
System.out.println(combined.apply(5));
```

**R:** `20`

**Ordre d'exécution**: `andThen` exécute la première fonction, **puis** la seconde.

1. `multiply2.apply(5)` → 10
2. `add10.apply(10)` → 20

Formule: `andThen` = `g(f(x))`

---

**Q27: Quelle est la différence entre andThen() et compose()?**
```java
Function<Integer, Integer> multiply2 = x -> x * 2;
Function<Integer, Integer> add10 = x -> x + 10;

Function<Integer, Integer> f1 = multiply2.andThen(add10);
Function<Integer, Integer> f2 = multiply2.compose(add10);

System.out.println(f1.apply(5));
System.out.println(f2.apply(5));
```

**R:**
```
20  // (5 * 2) + 10
30  // (5 + 10) * 2
```

- **andThen**: exécute **d'abord** la fonction, **puis** l'argument → `g(f(x))`
- **compose**: exécute **d'abord** l'argument, **puis** la fonction → `f(g(x))`

---

**Q28: Quelle functional interface utiliser?**
```java
// Prend un String, ne retourne rien
? printer = s -> System.out.println(s);

// Prend deux Integers, retourne leur somme
? adder = (a, b) -> a + b;

// Ne prend rien, retourne un nombre aléatoire
? random = () -> Math.random();

// Prend un String, retourne sa longueur
? length = s -> s.length();
```

**R:**
```java
Consumer<String> printer = s -> System.out.println(s);
BinaryOperator<Integer> adder = (a, b) -> a + b;
// OU BiFunction<Integer, Integer, Integer>
Supplier<Double> random = () -> Math.random();
Function<String, Integer> length = s -> s.length();
```

---

## Questions sur les Variables

**Q29: Ce code compile-t-il?**
```java
public void method() {
    int x = 10;
    Consumer<Integer> c = n -> System.out.println(x + n);
    x = 20;
}
```

**R:** Non, **erreur de compilation**: "local variables referenced from a lambda expression must be final or effectively final".

La variable `x` est **modifiée**, donc elle n'est **pas effectively final**.

---

**Q30: Ce code compile-t-il?**
```java
public void method() {
    int x = 10;
    Consumer<Integer> c = n -> System.out.println(x + n);
    c.accept(5);
}
```

**R:** Oui, compile et affiche `15`.

La variable `x` n'est **jamais modifiée**, donc elle est **effectively final**.

---

**Q31: Ce code compile-t-il?**
```java
public void method() {
    int x = 10;
    Consumer<Integer> c = x -> System.out.println(x);
}
```

**R:** Non, **erreur de compilation**: "variable x is already defined in method method()".

Le **paramètre** de la lambda ne peut pas avoir le **même nom** qu'une variable locale.

**Correct**:
```java
Consumer<Integer> c = y -> System.out.println(x + y);
```

---

**Q32: Ce code compile-t-il?**
```java
public class Example {
    private int x = 10;
    
    public void method() {
        Consumer<Integer> c = n -> x++;
        c.accept(5);
    }
}
```

**R:** Oui, compile.

Les **instance variables** peuvent être **modifiées** dans une lambda (pas de restriction effectively final).

---

**Q33: Que se passe-t-il?**
```java
public Runnable createRunnable() {
    int x = 42;
    return () -> System.out.println(x);
}  // x n'existe plus ici

Runnable r = createRunnable();
r.run();
```

**R:** Affiche `42`.

La lambda **capture** la valeur de `x` (effectively final). Même si `x` n'existe plus après la fin de la méthode, la lambda conserve sa valeur.

---

**Q34: Ce code compile-t-il?**
```java
public void method(int parameter) {
    Consumer<Integer> c = n -> {
        parameter++;
        System.out.println(parameter);
    };
}
```

**R:** Non, **erreur de compilation**: "final parameter may not be assigned".

Les **paramètres de méthode** doivent aussi être effectively final pour être utilisés dans une lambda.

---

**Q35: Comment contourner la restriction effectively final?**
```java
public void method() {
    int counter = 0;
    Runnable r = () -> counter++;  // Erreur
}
```

**R:** Utiliser un **array** ou un **wrapper**:

```java
// Avec array
int[] counter = {0};
Runnable r = () -> counter[0]++;

// Avec classe wrapper
class Counter { int value = 0; }
Counter counter = new Counter();
Runnable r = () -> counter.value++;

// Avec AtomicInteger
AtomicInteger counter = new AtomicInteger(0);
Runnable r = () -> counter.incrementAndGet();
```

---

## Questions Pièges

**Q36: Combien de fois "Hello" s'affiche-t-il?**
```java
Supplier<String> supplier = () -> "Hello";
System.out.println(supplier);
```

**R:** **0 fois**.

Le code affiche la **référence** du Supplier, pas le résultat de `get()`.

**Pour afficher "Hello"**:
```java
System.out.println(supplier.get());
```

---

**Q37: Ce code compile-t-il?**
```java
Predicate<String> p = s -> s.length() > 5;
boolean result = p.test(null);
```

**R:** Oui, compile mais lance **NullPointerException** à l'exécution.

`null.length()` cause l'exception. Il faut vérifier `null` d'abord:

```java
Predicate<String> p = s -> s != null && s.length() > 5;
```

---

**Q38: Que s'affiche-t-il?**
```java
List<String> list = Arrays.asList("a", "b", "c");
list.forEach(s -> System.out.print(s));
```

**R:** `abc` (sans espaces ni retours à la ligne).

`forEach` exécute l'action pour chaque élément dans l'ordre.

---

**Q39: Ce code compile-t-il?**
```java
Function<String, Integer> fn = s -> {
    if (s == null) return 0;
    return s.length();
};
```

**R:** Oui, compile.

Un bloc peut contenir plusieurs statements, y compris plusieurs `return`.

---

**Q40: Quelle est l'erreur?**
```java
Consumer<String> c1 = s -> System.out.println(s);
Consumer<String> c2 = s -> System.out.println(s.toUpperCase());
Consumer<String> combined = c1.compose(c2);
```

**R:** **Erreur de compilation**: "cannot find symbol: method compose".

`Consumer` n'a **pas** de méthode `compose()`, seulement `andThen()`.

**Correct**:
```java
Consumer<String> combined = c1.andThen(c2);
```

---

**Q41: Que retourne cette expression?**
```java
BinaryOperator<Integer> max = (a, b) -> a > b ? a : b;
int result = max.apply(5, 10);
```

**R:** `10`

La lambda retourne le **maximum** des deux nombres.

---

**Q42: Ce code compile-t-il?**
```java
@FunctionalInterface
public interface MyInterface {
    void method();
    
    default void method2() { }
    default void method3() { }
    
    static void method4() { }
    static void method5() { }
}
```

**R:** Oui, compile.

Il n'y a **qu'une seule** méthode abstraite (`method`). Les default et static ne comptent pas.

---

**Q43: Que fait cette lambda?**
```java
BiFunction<String, String, Boolean> fn = String::equals;
System.out.println(fn.apply("Hello", "Hello"));
```

**R:** Affiche `true`.

Équivalent à: `(s1, s2) -> s1.equals(s2)`

Le premier paramètre devient l'instance, le second devient le paramètre de `equals()`.

---

**Q44: Ce code compile-t-il?**
```java
Supplier<String> s1 = () -> "Hello";
Supplier<String> s2 = () -> "World";
Supplier<String> combined = s1.andThen(s2);
```

**R:** Non, **erreur de compilation**: "cannot find symbol: method andThen".

`Supplier` n'a **pas** de méthode `andThen()` (ni `compose()`).

---

**Q45: Quelle functional interface correspond?**
```java
(String s, Integer i) -> s.repeat(i)
```

**R:** `BiFunction<String, Integer, String>`

- **Entrée 1**: String
- **Entrée 2**: Integer
- **Sortie**: String

```java
BiFunction<String, Integer, String> fn = (s, i) -> s.repeat(i);
System.out.println(fn.apply("Ha", 3));  // "HaHaHa"
```

---

**Q46: Que s'affiche-t-il?**
```java
Function<Integer, Integer> f1 = x -> x + 1;
Function<Integer, Integer> f2 = x -> x * 2;

int result = f1.andThen(f2).apply(3);
System.out.println(result);
```

**R:** `8`

**Ordre d'exécution**:
1. `f1.apply(3)` → 4
2. `f2.apply(4)` → 8

---

**Q47: Ce code compile-t-il?**
```java
Predicate<String> isEmpty = String::isEmpty;
System.out.println(isEmpty.test(""));
System.out.println(isEmpty.test("Hello"));
```

**R:** Oui, compile.

```
true   // "" est vide
false  // "Hello" n'est pas vide
```

`String::isEmpty` est équivalent à `s -> s.isEmpty()`.

---

**Q48: Que fait ce code?**
```java
List<Integer> list = new ArrayList<>(Arrays.asList(1, 2, 3));
Consumer<Integer> doubler = n -> n * 2;
list.forEach(doubler);
System.out.println(list);
```

**R:** Affiche `[1, 2, 3]` (inchangé).

**Piège**: `Consumer` ne **retourne rien** (`void`). L'expression `n * 2` est calculée mais **pas utilisée**.

Pour modifier la liste:
```java
list.replaceAll(n -> n * 2);  // UnaryOperator
System.out.println(list);  // [2, 4, 6]
```

---

**Q49: Ce code compile-t-il?**
```java
Function<Integer, String> fn = x -> return x.toString();
```

**R:** Non, **erreur de compilation**.

Sans bloc `{}`, le `return` est **implicite**.

**Correct**:
```java
Function<Integer, String> fn = x -> x.toString();
// OU
Function<Integer, String> fn = x -> { return x.toString(); };
```

---

**Q50: Que retourne cette expression?**
```java
Predicate<String> startsA = s -> s.startsWith("A");
Predicate<String> endsZ = s -> s.endsWith("z");
Predicate<String> combined = startsA.or(endsZ);

System.out.println(combined.test("Alice"));
System.out.println(combined.test("Buzz"));
System.out.println(combined.test("Bob"));
```

**R:**
```
true   // Commence par A (OU satisfait)
true   // Finit par z (OU satisfait)
false  // Ni l'un ni l'autre
```

`or()` combine avec un **OU logique** (au moins une condition vraie).

---

**Q51: Ce code compile-t-il?**
```java
Function<String, Integer> fn1 = s -> s.length();
Function<String, Integer> fn2 = String::length;

System.out.println(fn1 == fn2);
```

**R:** Oui, compile mais affiche `false`.

Ce sont deux **objets différents**, même s'ils font la même chose.

Pour comparer fonctionnellement:
```java
System.out.println(fn1.apply("test") == fn2.apply("test"));  // true
```

---

**Q52: Quel type de method reference?**
```java
// A
List<String> list = new ArrayList<>();
Consumer<String> c = list::add;

// B
Function<Integer, String[]> fn = String[]::new;

// C
Comparator<String> comp = String::compareTo;

// D
Supplier<List<String>> s = ArrayList::new;
```

**R:**
- **A**: Instance method on particular object (`list::add`)
- **B**: Array constructor reference (`String[]::new`)
- **C**: Instance method on parameter (`String::compareTo`)
- **D**: Constructor reference (`ArrayList::new`)

---

**Q53: Ce code compile-t-il?**
```java
BiConsumer<List<String>, String> adder = List::add;
List<String> list = new ArrayList<>();
adder.accept(list, "Hello");
System.out.println(list);
```

**R:** Oui, compile et affiche `[Hello]`.

`List::add` est une instance method reference on parameter.

Équivalent à: `(list, item) -> list.add(item)`

---

**Q54: Que se passe-t-il?**
```java
Function<String, String> identity = Function.identity();
System.out.println(identity.apply("Hello"));
```

**R:** Affiche `"Hello"`.

`Function.identity()` retourne une fonction qui **retourne son entrée inchangée**: `x -> x`.

---

**Q55: Ce code compile-t-il?**
```java
Predicate<Integer> isEven = n -> n % 2 == 0;
Predicate<Integer> isPositive = n -> n > 0;

Predicate<Integer> combined = isEven
    .and(isPositive)
    .or(n -> n == 0);

System.out.println(combined.test(4));
System.out.println(combined.test(-2));
System.out.println(combined.test(0));
```

**R:** Oui, compile.

```
true   // (pair ET positif) OU 0 → true
false  // (pair ET positif=false) OU 0=false → false
true   // (pair ET positif=false) OU 0=true → true
```

Les opérations de Predicate peuvent être **chaînées**.

---