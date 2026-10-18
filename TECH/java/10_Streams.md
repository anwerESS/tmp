## [**..**](./00_index.md)

# Chapter 10: Streams

## Returning an Optional

### Optional Basics

**Optional** (Java 8+) est un conteneur qui peut contenir ou non une valeur, évitant les `NullPointerException`.

**Créer un Optional**:
```java
import java.util.Optional;

// Optional vide
Optional<String> empty = Optional.empty();

// Optional avec valeur
Optional<String> nonEmpty = Optional.of("Hello");

// Optional avec valeur potentiellement null
String value = null;
Optional<String> maybeEmpty = Optional.ofNullable(value);  // empty

value = "World";
Optional<String> maybeNonEmpty = Optional.ofNullable(value);  // non-empty
```

**Important**: `Optional.of(null)` lance **NullPointerException**. Utiliser `ofNullable()` si la valeur peut être null.

### Checking if Value is Present

```java
Optional<String> opt = Optional.of("Hello");

// isPresent()
if (opt.isPresent()) {
    System.out.println(opt.get());  // "Hello"
}

// isEmpty() (Java 11+)
if (opt.isEmpty()) {
    System.out.println("Empty");
}

// ifPresent() avec lambda
opt.ifPresent(value -> System.out.println(value));
opt.ifPresent(System.out::println);  // Method reference
```

### Getting the Value

```java
Optional<String> opt = Optional.of("Hello");

// get() - lance NoSuchElementException si vide
String value = opt.get();  // "Hello"

Optional<String> empty = Optional.empty();
// String value2 = empty.get();  // NoSuchElementException!
```

### Providing Default Values

```java
Optional<String> empty = Optional.empty();
Optional<String> nonEmpty = Optional.of("Hello");

// orElse() - valeur par défaut
String s1 = empty.orElse("Default");     // "Default"
String s2 = nonEmpty.orElse("Default");  // "Hello"

// orElseGet() - Supplier pour valeur par défaut (lazy)
String s3 = empty.orElseGet(() -> "Computed Default");

// orElseThrow() - lance exception si vide
String s4 = nonEmpty.orElseThrow();  // "Hello" (Java 10+)
String s5 = empty.orElseThrow(() -> new IllegalStateException("Empty!"));
```

**Différence orElse() vs orElseGet()**:
```java
// orElse() - TOUJOURS évalué
String result1 = opt.orElse(expensiveMethod());  // expensiveMethod() appelé même si opt non vide

// orElseGet() - évalué SEULEMENT si vide (lazy)
String result2 = opt.orElseGet(() -> expensiveMethod());  // appelé seulement si opt vide
```

### Transforming Optional

**map()**: transforme la valeur si présente
```java
Optional<String> opt = Optional.of("Hello");

Optional<Integer> length = opt.map(String::length);
System.out.println(length.get());  // 5

Optional<String> upper = opt.map(String::toUpperCase);
System.out.println(upper.get());  // "HELLO"

// Si empty, reste empty
Optional<String> empty = Optional.empty();
Optional<Integer> emptyLength = empty.map(String::length);
System.out.println(emptyLength.isPresent());  // false
```

**flatMap()**: transforme en Optional (évite Optional<Optional<T>>)
```java
public class Person {
    private Optional<Address> address;
    
    public Optional<Address> getAddress() {
        return address;
    }
}

public class Address {
    private String city;
    
    public String getCity() {
        return city;
    }
}

Optional<Person> person = Optional.of(new Person());

// map() donnerait Optional<Optional<Address>>
Optional<Optional<Address>> nested = person.map(Person::getAddress);

// flatMap() donne Optional<Address>
Optional<Address> address = person.flatMap(Person::getAddress);

// Chaînage
Optional<String> city = person
    .flatMap(Person::getAddress)
    .map(Address::getCity);
```

**filter()**: garde la valeur si elle satisfait le prédicat
```java
Optional<Integer> opt = Optional.of(42);

Optional<Integer> even = opt.filter(n -> n % 2 == 0);
System.out.println(even.isPresent());  // true

Optional<Integer> odd = opt.filter(n -> n % 2 != 0);
System.out.println(odd.isPresent());  // false

// Chaînage
Optional<String> result = Optional.of("Hello")
    .filter(s -> s.length() > 3)
    .map(String::toUpperCase);
System.out.println(result.get());  // "HELLO"
```

### Optional Examples

**Exemple 1: Méthode qui peut ne pas trouver de résultat**
```java
public class UserRepository {
    private Map<Long, User> users = new HashMap<>();
    
    public Optional<User> findById(Long id) {
        return Optional.ofNullable(users.get(id));
    }
}

// Utilisation
UserRepository repo = new UserRepository();
Optional<User> user = repo.findById(123L);

// Ancienne approche (risque NPE)
// User u = repo.findById(123L);
// if (u != null) { ... }

// Avec Optional
user.ifPresent(u -> System.out.println(u.getName()));

String userName = user
    .map(User::getName)
    .orElse("Unknown");
```

**Exemple 2: Chaînage de transformations**
```java
Optional<String> result = Optional.of("  hello world  ")
    .filter(s -> !s.isEmpty())
    .map(String::trim)
    .map(String::toUpperCase)
    .filter(s -> s.length() > 5);

System.out.println(result.orElse("Too short"));  // "HELLO WORLD"
```

## Using Streams

### Stream Basics

Un **Stream** est une séquence d'éléments supportant des opérations séquentielles et parallèles.

**Caractéristiques**:
- **Lazy**: les opérations intermédiaires ne s'exécutent que quand une opération terminale est appelée
- **Consommable**: un stream ne peut être utilisé qu'une seule fois
- **Pas une structure de données**: ne stocke pas d'éléments

**Créer un Stream**:
```java
import java.util.stream.*;

// Depuis une collection
List<String> list = Arrays.asList("A", "B", "C");
Stream<String> stream1 = list.stream();

// Depuis un array
String[] array = {"A", "B", "C"};
Stream<String> stream2 = Arrays.stream(array);
Stream<String> stream3 = Stream.of("A", "B", "C");

// Stream vide
Stream<String> empty = Stream.empty();

// Stream infini
Stream<Integer> infinite1 = Stream.iterate(0, n -> n + 2);  // 0, 2, 4, 6...
Stream<Double> infinite2 = Stream.generate(Math::random);

// Depuis un range (primitive streams)
IntStream range = IntStream.range(1, 5);      // 1, 2, 3, 4
IntStream rangeClosed = IntStream.rangeClosed(1, 5);  // 1, 2, 3, 4, 5
```

### Stream Pipeline

Un pipeline de stream = **source** + **opérations intermédiaires** + **opération terminale**

```java
List<String> names = Arrays.asList("Alice", "Bob", "Charlie", "David");

long count = names.stream()              // Source
    .filter(s -> s.length() > 3)         // Opération intermédiaire
    .map(String::toUpperCase)            // Opération intermédiaire
    .count();                            // Opération terminale

System.out.println(count);  // 3
```

**Opérations intermédiaires** (retournent un Stream):
- `filter()`, `map()`, `flatMap()`
- `distinct()`, `sorted()`, `limit()`, `skip()`
- `peek()`

**Opérations terminales** (retournent un résultat):
- `forEach()`, `count()`, `collect()`
- `reduce()`, `min()`, `max()`
- `anyMatch()`, `allMatch()`, `noneMatch()`
- `findFirst()`, `findAny()`

### Common Intermediate Operations

#### filter()
Garde les éléments qui satisfont le prédicat.

```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

List<Integer> evens = numbers.stream()
    .filter(n -> n % 2 == 0)
    .collect(Collectors.toList());

System.out.println(evens);  // [2, 4, 6, 8, 10]
```

#### map()
Transforme chaque élément.

```java
List<String> words = Arrays.asList("apple", "banana", "cherry");

List<Integer> lengths = words.stream()
    .map(String::length)
    .collect(Collectors.toList());

System.out.println(lengths);  // [5, 6, 6]

List<String> upper = words.stream()
    .map(String::toUpperCase)
    .collect(Collectors.toList());

System.out.println(upper);  // [APPLE, BANANA, CHERRY]
```

#### flatMap()
Transforme chaque élément en Stream et aplatit le résultat.

```java
List<List<Integer>> nested = Arrays.asList(
    Arrays.asList(1, 2, 3),
    Arrays.asList(4, 5),
    Arrays.asList(6, 7, 8, 9)
);

List<Integer> flat = nested.stream()
    .flatMap(List::stream)
    .collect(Collectors.toList());

System.out.println(flat);  // [1, 2, 3, 4, 5, 6, 7, 8, 9]

// Exemple: séparer les mots
List<String> phrases = Arrays.asList("Hello World", "Java Streams");

List<String> allWords = phrases.stream()
    .flatMap(phrase -> Arrays.stream(phrase.split(" ")))
    .collect(Collectors.toList());

System.out.println(allWords);  // [Hello, World, Java, Streams]
```

#### distinct()
Supprime les doublons.

```java
List<Integer> numbers = Arrays.asList(1, 2, 2, 3, 3, 3, 4, 5, 5);

List<Integer> unique = numbers.stream()
    .distinct()
    .collect(Collectors.toList());

System.out.println(unique);  // [1, 2, 3, 4, 5]
```

#### sorted()
Trie les éléments.

```java
List<Integer> numbers = Arrays.asList(5, 2, 8, 1, 9);

// Ordre naturel
List<Integer> sorted = numbers.stream()
    .sorted()
    .collect(Collectors.toList());
System.out.println(sorted);  // [1, 2, 5, 8, 9]

// Ordre custom
List<String> words = Arrays.asList("Charlie", "Alice", "Bob");
List<String> sortedWords = words.stream()
    .sorted(Comparator.reverseOrder())
    .collect(Collectors.toList());
System.out.println(sortedWords);  // [Charlie, Bob, Alice]
```

#### limit() et skip()
Limite ou saute des éléments.

```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

// limit() - prend les n premiers
List<Integer> first3 = numbers.stream()
    .limit(3)
    .collect(Collectors.toList());
System.out.println(first3);  // [1, 2, 3]

// skip() - saute les n premiers
List<Integer> skip3 = numbers.stream()
    .skip(3)
    .collect(Collectors.toList());
System.out.println(skip3);  // [4, 5, 6, 7, 8, 9, 10]

// Combinaison (pagination)
List<Integer> page2 = numbers.stream()
    .skip(3)
    .limit(3)
    .collect(Collectors.toList());
System.out.println(page2);  // [4, 5, 6]
```

#### peek()
Exécute une action sur chaque élément (debugging).

```java
List<Integer> result = Arrays.asList(1, 2, 3, 4, 5).stream()
    .peek(n -> System.out.println("Original: " + n))
    .map(n -> n * 2)
    .peek(n -> System.out.println("Doubled: " + n))
    .collect(Collectors.toList());

// Output:
// Original: 1
// Doubled: 2
// Original: 2
// Doubled: 4
// ...
```

### Common Terminal Operations

#### forEach()
Exécute une action sur chaque élément.

```java
List<String> names = Arrays.asList("Alice", "Bob", "Charlie");

names.stream()
    .forEach(System.out::println);

// Avec index (via IntStream)
IntStream.range(0, names.size())
    .forEach(i -> System.out.println(i + ": " + names.get(i)));
```

#### count()
Compte le nombre d'éléments.

```java
long count = Arrays.asList(1, 2, 3, 4, 5).stream()
    .filter(n -> n % 2 == 0)
    .count();

System.out.println(count);  // 2
```

#### collect()
Accumule les éléments dans une collection.

```java
List<String> words = Arrays.asList("apple", "banana", "cherry");

// Vers List
List<String> list = words.stream()
    .map(String::toUpperCase)
    .collect(Collectors.toList());

// Vers Set
Set<String> set = words.stream()
    .collect(Collectors.toSet());

// Vers String
String joined = words.stream()
    .collect(Collectors.joining(", "));
System.out.println(joined);  // "apple, banana, cherry"

// Vers Map
Map<String, Integer> map = words.stream()
    .collect(Collectors.toMap(
        w -> w,              // clé
        String::length       // valeur
    ));
System.out.println(map);  // {apple=5, banana=6, cherry=6}
```

#### reduce()
Réduit le stream à une seule valeur.

```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5);

// Somme
Optional<Integer> sum = numbers.stream()
    .reduce((a, b) -> a + b);
System.out.println(sum.get());  // 15

// Avec valeur initiale
int sum2 = numbers.stream()
    .reduce(0, (a, b) -> a + b);
System.out.println(sum2);  // 15

// Produit
int product = numbers.stream()
    .reduce(1, (a, b) -> a * b);
System.out.println(product);  // 120

// Maximum
Optional<Integer> max = numbers.stream()
    .reduce(Integer::max);
System.out.println(max.get());  // 5

// Concaténation de strings
String concatenated = Arrays.asList("a", "b", "c").stream()
    .reduce("", (a, b) -> a + b);
System.out.println(concatenated);  // "abc"
```

#### min() et max()
Trouvent le minimum/maximum.

```java
List<Integer> numbers = Arrays.asList(5, 2, 8, 1, 9);

Optional<Integer> min = numbers.stream()
    .min(Integer::compareTo);
System.out.println(min.get());  // 1

Optional<Integer> max = numbers.stream()
    .max(Integer::compareTo);
System.out.println(max.get());  // 9

// Avec comparateur custom
Optional<String> longest = Arrays.asList("a", "bbb", "cc").stream()
    .max(Comparator.comparing(String::length));
System.out.println(longest.get());  // "bbb"
```

#### anyMatch(), allMatch(), noneMatch()
Vérifient si des éléments correspondent au prédicat.

```java
List<Integer> numbers = Arrays.asList(2, 4, 6, 8, 10);

// anyMatch - au moins un
boolean hasOdd = numbers.stream()
    .anyMatch(n -> n % 2 != 0);
System.out.println(hasOdd);  // false

// allMatch - tous
boolean allEven = numbers.stream()
    .allMatch(n -> n % 2 == 0);
System.out.println(allEven);  // true

// noneMatch - aucun
boolean noneNegative = numbers.stream()
    .noneMatch(n -> n < 0);
System.out.println(noneNegative);  // true
```

#### findFirst() et findAny()
Trouvent un élément.

```java
List<String> names = Arrays.asList("Alice", "Bob", "Charlie", "David");

// findFirst - premier élément
Optional<String> first = names.stream()
    .filter(s -> s.startsWith("C"))
    .findFirst();
System.out.println(first.get());  // "Charlie"

// findAny - n'importe quel élément (utile pour parallel streams)
Optional<String> any = names.stream()
    .filter(s -> s.length() > 3)
    .findAny();
System.out.println(any.get());  // "Alice" (ou autre)
```

### Lazy Evaluation

Les streams sont **lazy**: les opérations intermédiaires ne s'exécutent que quand une opération terminale est appelée.

```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5);

Stream<Integer> stream = numbers.stream()
    .filter(n -> {
        System.out.println("Filter: " + n);
        return n % 2 == 0;
    })
    .map(n -> {
        System.out.println("Map: " + n);
        return n * 2;
    });

System.out.println("Stream créé, mais pas encore exécuté");

// Rien ne s'affiche jusqu'ici

stream.forEach(System.out::println);
// Maintenant tout s'exécute:
// Filter: 1
// Filter: 2
// Map: 2
// 4
// Filter: 3
// Filter: 4
// Map: 4
// 8
// Filter: 5
```

**Court-circuit**: certaines opérations terminales arrêtent le traitement dès que possible.

```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

Optional<Integer> first = numbers.stream()
    .filter(n -> {
        System.out.println("Testing: " + n);
        return n > 5;
    })
    .findFirst();

// Output:
// Testing: 1
// Testing: 2
// Testing: 3
// Testing: 4
// Testing: 5
// Testing: 6

System.out.println(first.get());  // 6
// S'arrête à 6, ne teste pas 7, 8, 9, 10
```

### Stream Cannot Be Reused

Un stream ne peut être utilisé qu'**une seule fois**.

```java
Stream<String> stream = Stream.of("A", "B", "C");
stream.forEach(System.out::println);  // OK

// stream.forEach(System.out::println);  // IllegalStateException!

// Pour réutiliser, créer un nouveau stream
List<String> list = Arrays.asList("A", "B", "C");
list.stream().forEach(System.out::println);  // OK
list.stream().count();  // OK - nouveau stream
```

## Working with Primitive Streams

### Why Primitive Streams?

Les streams génériques utilisent des wrappers (Integer, Double), causant de l'**autoboxing/unboxing** (perte de performance).

Les **primitive streams** évitent ce problème.

**Types de primitive streams**:
- **IntStream**: pour `int`
- **LongStream**: pour `long`
- **DoubleStream**: pour `double`

### Creating Primitive Streams

```java
// IntStream
IntStream intStream1 = IntStream.of(1, 2, 3, 4, 5);
IntStream intStream2 = IntStream.range(1, 5);        // 1, 2, 3, 4
IntStream intStream3 = IntStream.rangeClosed(1, 5);  // 1, 2, 3, 4, 5

// LongStream
LongStream longStream = LongStream.rangeClosed(1L, 1000L);

// DoubleStream
DoubleStream doubleStream = DoubleStream.of(1.5, 2.5, 3.5);

// Depuis un array
int[] array = {1, 2, 3, 4, 5};
IntStream intStream4 = Arrays.stream(array);

// Infinite streams
IntStream infinite1 = IntStream.iterate(0, n -> n + 2);  // 0, 2, 4, 6...
IntStream infinite2 = IntStream.generate(() -> (int)(Math.random() * 100));
```

### Primitive Stream Methods

Les primitive streams ont des méthodes spécifiques pour les statistiques:

```java
IntStream stream = IntStream.of(1, 2, 3, 4, 5);

// Méthodes terminales spécifiques
int sum = stream.sum();                    // 15
OptionalDouble avg = stream.average();     // 3.0
OptionalInt max = stream.max();            // 5
OptionalInt min = stream.min();            // 1
long count = stream.count();               // 5

// IntSummaryStatistics
IntSummaryStatistics stats = IntStream.of(1, 2, 3, 4, 5)
    .summaryStatistics();

System.out.println("Count: " + stats.getCount());      // 5
System.out.println("Sum: " + stats.getSum());          // 15
System.out.println("Min: " + stats.getMin());          // 1
System.out.println("Max: " + stats.getMax());          // 5
System.out.println("Average: " + stats.getAverage());  // 3.0
```

### Mapping Between Stream Types

**Depuis Stream<T> vers primitive stream**:
```java
Stream<String> words = Stream.of("apple", "banana", "cherry");

// mapToInt
IntStream lengths = words.mapToInt(String::length);
System.out.println(lengths.sum());  // 17

// mapToLong
Stream<String> numbers = Stream.of("100", "200", "300");
LongStream longStream = numbers.mapToLong(Long::parseLong);

// mapToDouble
Stream<Integer> ints = Stream.of(1, 2, 3);
DoubleStream doubles = ints.mapToDouble(Integer::doubleValue);
```

**Depuis primitive stream vers Stream<T>**:
```java
IntStream intStream = IntStream.of(1, 2, 3, 4, 5);

// boxed() - convertit en Stream<Integer>
Stream<Integer> boxed = intStream.boxed();

// mapToObj() - transforme en objets
IntStream intStream2 = IntStream.of(1, 2, 3);
Stream<String> strings = intStream2.mapToObj(n -> "Number: " + n);
```

**Entre primitive streams**:
```java
IntStream intStream = IntStream.of(1, 2, 3);

// asLongStream()
LongStream longStream = intStream.asLongStream();

// asDoubleStream()
IntStream intStream2 = IntStream.of(1, 2, 3);
DoubleStream doubleStream = intStream2.asDoubleStream();
```

### OptionalInt, OptionalLong, OptionalDouble

Les primitive streams retournent des Optional spécialisés.

```java
OptionalInt max = IntStream.of(1, 2, 3, 4, 5).max();

if (max.isPresent()) {
    System.out.println(max.getAsInt());  // 5
}

// orElse
int value = max.orElse(0);

// Conversion vers Optional<Integer>
Optional<Integer> boxedMax = max.stream()
    .boxed()
    .findFirst();
```

## Working with Advanced Stream Pipeline Concepts

### Collecting Results

**Collectors** fournit des collecteurs prédéfinis pour accumuler les résultats.

#### Basic Collectors

```java
List<String> words = Arrays.asList("apple", "banana", "cherry", "date");

// toList()
List<String> list = words.stream()
    .filter(s -> s.length() > 5)
    .collect(Collectors.toList());

// toSet()
Set<String> set = words.stream()
    .collect(Collectors.toSet());

// toCollection() - type spécifique
TreeSet<String> treeSet = words.stream()
    .collect(Collectors.toCollection(TreeSet::new));

// toArray()
String[] array = words.stream()
    .toArray(String[]::new);
```

#### Joining Strings

```java
List<String> words = Arrays.asList("Java", "is", "awesome");

// joining() - simple
String joined1 = words.stream()
    .collect(Collectors.joining());
System.out.println(joined1);  // "Javaisawesome"

// joining() - avec séparateur
String joined2 = words.stream()
    .collect(Collectors.joining(" "));
System.out.println(joined2);  // "Java is awesome"

// joining() - avec préfixe et suffixe
String joined3 = words.stream()
    .collect(Collectors.joining(", ", "[", "]"));
System.out.println(joined3);  // "[Java, is, awesome]"
```

#### Averaging, Summing, Counting

```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5);

// averagingInt/Long/Double
Double avg = numbers.stream()
    .collect(Collectors.averagingInt(Integer::intValue));
System.out.println(avg);  // 3.0

// summingInt/Long/Double
Integer sum = numbers.stream()
    .collect(Collectors.summingInt(Integer::intValue));
System.out.println(sum);  // 15

// counting
Long count = numbers.stream()
    .collect(Collectors.counting());
System.out.println(count);  // 5

// summarizingInt/Long/Double
IntSummaryStatistics stats = numbers.stream()
    .collect(Collectors.summarizingInt(Integer::intValue));
System.out.println(stats);  // IntSummaryStatistics{count=5, sum=15, min=1, average=3.0, max=5}
```

#### Collecting to Map

```java
List<String> words = Arrays.asList("apple", "banana", "cherry");

// toMap() - simple
Map<String, Integer> map1 = words.stream()
    .collect(Collectors.toMap(
        w -> w,              // clé
        String::length       // valeur
    ));
System.out.println(map1);  // {apple=5, banana=6, cherry=6}

// toMap() - avec merge function (doublons)
List<String> duplicates = Arrays.asList("a", "bb", "ccc", "dd");
Map<Integer, String> map2 = duplicates.stream()
    .collect(Collectors.toMap(
        String::length,                  // clé
        w -> w,                          // valeur
        (existing, replacement) -> existing + "," + replacement  // merge
    ));
System.out.println(map2);  // {1=a, 2=bb,dd, 3=ccc}

// toMap() - avec type de Map spécifique
TreeMap<String, Integer> treeMap = words.stream()
    .collect(Collectors.toMap(
        w -> w,
        String::length,
        (v1, v2) -> v1,
        TreeMap::new
    ));
```

#### Grouping

```java
List<String> words = Arrays.asList("apple", "banana", "cherry", "date", "apricot", "blueberry");

// groupingBy() - simple
Map<Integer, List<String>> byLength = words.stream()
    .collect(Collectors.groupingBy(String::length));
System.out.println(byLength);
// {4=[date], 5=[apple], 6=[banana, cherry], 7=[apricot], 9=[blueberry]}

// groupingBy() - avec downstream collector
Map<Integer, Long> countByLength = words.stream()
    .collect(Collectors.groupingBy(
        String::length,
        Collectors.counting()
    ));
System.out.println(countByLength);  // {4=1, 5=1, 6=2, 7=1, 9=1}

// groupingBy() - avec Set
Map<Integer, Set<String>> setByLength = words.stream()
    .collect(Collectors.groupingBy(
        String::length,
        Collectors.toSet()
    ));
```

#### Partitioning

```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

// partitioningBy() - sépare en deux groupes (true/false)
Map<Boolean, List<Integer>> partitioned = numbers.stream()
    .collect(Collectors.partitioningBy(n -> n % 2 == 0));

System.out.println(partitioned.get(true));   // [2, 4, 6, 8, 10]
System.out.println(partitioned.get(false));  // [1, 3, 5, 7, 9]

// partitioningBy() - avec downstream collector
Map<Boolean, Long> countPartitioned = numbers.stream()
    .collect(Collectors.partitioningBy(
        n -> n % 2 == 0,
        Collectors.counting()
    ));
System.out.println(countPartitioned);  // {false=5, true=5}
```

### Teeing Collector (Java 12+)

Combine deux collectors pour produire un seul résultat.

```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5);

// Calculer min et max en une seule passe
record MinMax(int min, int max) { }

MinMax result = numbers.stream()
    .collect(Collectors.teeing(
        Collectors.minBy(Integer::compareTo),
        Collectors.maxBy(Integer::compareTo),
        (min, max) -> new MinMax(
            min.orElse(0),
            max.orElse(0)
        )
    ));

System.out.println(result);  // MinMax[min=1, max=5]
```

### Filtering Collector (Java 9+)

Filtre les éléments avant de les collecter.

```java
List<String> words = Arrays.asList("apple", "banana", "cherry", "date");

// Grouper seulement les mots de longueur > 5
Map<Integer, List<String>> grouped = words.stream()
    .collect(Collectors.groupingBy(
        String::length,
        Collectors.filtering(
            s -> s.length() > 5,
            Collectors.toList()
        )
    ));

System.out.println(grouped);  // {4=[], 5=[], 6=[banana, cherry]}
```

### FlatMapping Collector (Java 9+)

Aplatit les résultats pendant la collecte.

```java
List<List<String>> nested = Arrays.asList(
    Arrays.asList("a", "b"),
    Arrays.asList("c", "d", "e"),
    Arrays.asList("f")
);

// Grouper par longueur de la liste, puis aplatir
Map<Integer, List<String>> flattened = nested.stream()
    .collect(Collectors.groupingBy(
        List::size,
        Collectors.flatMapping(
            List::stream,
            Collectors.toList()
        )
    ));

System.out.println(flattened);  // {1=[f], 2=[a, b], 3=[c, d, e]}
```

---

## Exemples Pratiques Complets

### Exemple 1: Traitement de Données Employés
```java
import java.util.*;
import java.util.stream.*;

public class Employee {
    private String name;
    private String department;
    private double salary;
    private int age;
    
    public Employee(String name, String department, double salary, int age) {
        this.name = name;
        this.department = department;
        this.salary = salary;
        this.age = age;
    }
    
    // Getters
    public String getName() { return name; }
    public String getDepartment() { return department; }
    public double getSalary() { return salary; }
    public int getAge() { return age; }
    
    @Override
    public String toString() {
        return String.format("%s (%s, $%.0f, %d ans)", name, department, salary, age);
    }
}

public class EmployeeAnalysis {
    public static void main(String[] args) {
        List<Employee> employees = Arrays.asList(
            new Employee("Alice", "IT", 75000, 30),
            new Employee("Bob", "HR", 50000, 25),
            new Employee("Charlie", "IT", 85000, 35),
            new Employee("David", "Finance", 70000, 28),
            new Employee("Eve", "IT", 90000, 32),
            new Employee("Frank", "HR", 55000, 27),
            new Employee("Grace", "Finance", 80000, 40)
        );
        
        // 1. Salaire moyen par département
        System.out.println("=== SALAIRE MOYEN PAR DÉPARTEMENT ===");
        Map<String, Double> avgSalaryByDept = employees.stream()
            .collect(Collectors.groupingBy(
                Employee::getDepartment,
                Collectors.averagingDouble(Employee::getSalary)
            ));
        avgSalaryByDept.forEach((dept, avg) -> 
            System.out.printf("%s: $%.2f%n", dept, avg)
        );
        
        // 2. Top 3 salaires
        System.out.println("\n=== TOP 3 SALAIRES ===");
        employees.stream()
            .sorted(Comparator.comparing(Employee::getSalary).reversed())
            .limit(3)
            .forEach(System.out::println);
        
        // 3. Employés IT avec salaire > 80000
        System.out.println("\n=== IT SALAIRE > 80K ===");
        List<String> highPaidIT = employees.stream()
            .filter(e -> e.getDepartment().equals("IT"))
            .filter(e -> e.getSalary() > 80000)
            .map(Employee::getName)
            .collect(Collectors.toList());
        System.out.println(highPaidIT);
        
        // 4. Total des salaires par département
        System.out.println("\n=== TOTAL SALAIRES PAR DÉPARTEMENT ===");
        Map<String, Double> totalByDept = employees.stream()
            .collect(Collectors.groupingBy(
                Employee::getDepartment,
                Collectors.summingDouble(Employee::getSalary)
            ));
        totalByDept.forEach((dept, total) -> 
            System.out.printf("%s: $%.0f%n", dept, total)
        );
        
        // 5. Statistiques d'âge
        System.out.println("\n=== STATISTIQUES D'ÂGE ===");
        IntSummaryStatistics ageStats = employees.stream()
            .mapToInt(Employee::getAge)
            .summaryStatistics();
        System.out.println("Âge moyen: " + ageStats.getAverage());
        System.out.println("Âge min: " + ageStats.getMin());
        System.out.println("Âge max: " + ageStats.getMax());
        
        // 6. Grouper par tranche d'âge
        System.out.println("\n=== PAR TRANCHE D'ÂGE ===");
        Map<String, List<Employee>> byAgeGroup = employees.stream()
            .collect(Collectors.groupingBy(e -> {
                if (e.getAge() < 30) return "< 30";
                else if (e.getAge() < 35) return "30-34";
                else return "35+";
            }));
        byAgeGroup.forEach((group, emps) -> {
            System.out.println(group + ":");
            emps.forEach(e -> System.out.println("  " + e));
        });
        
        // 7. Département avec le salaire moyen le plus élevé
        System.out.println("\n=== DÉPARTEMENT SALAIRE MOYEN MAX ===");
        Optional<Map.Entry<String, Double>> topDept = avgSalaryByDept.entrySet().stream()
            .max(Map.Entry.comparingByValue());
        topDept.ifPresent(entry -> 
            System.out.printf("%s: $%.2f%n", entry.getKey(), entry.getValue())
        );
        
        // 8. Partitionner par salaire (> ou <= 70000)
        System.out.println("\n=== PARTITION PAR SALAIRE (70K) ===");
        Map<Boolean, List<Employee>> partitioned = employees.stream()
            .collect(Collectors.partitioningBy(e -> e.getSalary() > 70000));
        System.out.println("> 70K: " + partitioned.get(true).size());
        System.out.println("<= 70K: " + partitioned.get(false).size());
    }
}
```

### Exemple 2: Analyse de Texte
```java
import java.util.*;
import java.util.stream.*;

public class TextAnalysis {
    
    public static Map<String, Long> wordFrequency(String text) {
        return Arrays.stream(text.toLowerCase().split("\\W+"))
            .filter(word -> !word.isEmpty())
            .collect(Collectors.groupingBy(
                word -> word,
                Collectors.counting()
            ));
    }
    
    public static List<String> topWords(String text, int n) {
        return wordFrequency(text).entrySet().stream()
            .sorted(Map.Entry.<String, Long>comparingByValue().reversed())
            .limit(n)
            .map(Map.Entry::getKey)
            .collect(Collectors.toList());
    }
    
    public static Map<Integer, Long> wordLengthDistribution(String text) {
        return Arrays.stream(text.split("\\W+"))
            .filter(word -> !word.isEmpty())
            .collect(Collectors.groupingBy(
                String::length,
                TreeMap::new,
                Collectors.counting()
            ));
    }
    
    public static Set<String> uniqueWords(String text) {
        return Arrays.stream(text.toLowerCase().split("\\W+"))
            .filter(word -> !word.isEmpty())
            .collect(Collectors.toSet());
    }
    
    public static String longestWord(String text) {
        return Arrays.stream(text.split("\\W+"))
            .max(Comparator.comparing(String::length))
            .orElse("");
    }
    
    public static void main(String[] args) {
        String text = """
            Java streams are a powerful feature introduced in Java 8.
            Streams provide a high-level abstraction for processing sequences
            of elements. Using streams makes code more readable and concise.
            Java streams support functional-style operations on streams of elements.
            """;
        
        // Fréquence des mots
        System.out.println("=== FRÉQUENCE DES MOTS ===");
        Map<String, Long> freq = wordFrequency(text);
        freq.entrySet().stream()
            .sorted(Map.Entry.<String, Long>comparingByValue().reversed())
            .limit(10)
            .forEach(entry -> 
                System.out.printf("%s: %d%n", entry.getKey(), entry.getValue())
            );
        
        // Top 5 mots
        System.out.println("\n=== TOP 5 MOTS ===");
        List<String> top5 = topWords(text, 5);
        System.out.println(top5);
        
        // Distribution par longueur
        System.out.println("\n=== DISTRIBUTION LONGUEUR ===");
        Map<Integer, Long> lengthDist = wordLengthDistribution(text);
        lengthDist.forEach((length, count) -> 
            System.out.printf("Longueur %d: %d mots%n", length, count)
        );
        
        // Statistiques
        System.out.println("\n=== STATISTIQUES ===");
        long totalWords = Arrays.stream(text.split("\\W+"))
            .filter(w -> !w.isEmpty())
            .count();
        long uniqueCount = uniqueWords(text).size();
        System.out.println("Total mots: " + totalWords);
        System.out.println("Mots uniques: " + uniqueCount);
        System.out.println("Mot le plus long: " + longestWord(text));
        
        // Mots commençant par 's'
        System.out.println("\n=== MOTS COMMENÇANT PAR 'S' ===");
        List<String> startsWithS = Arrays.stream(text.toLowerCase().split("\\W+"))
            .filter(w -> w.startsWith("s"))
            .distinct()
            .sorted()
            .collect(Collectors.toList());
        System.out.println(startsWithS);
    }
}
```

### Exemple 3: Traitement de Transactions
```java
import java.time.*;
import java.util.*;
import java.util.stream.*;

public class Transaction {
    private String id;
    private String category;
    private double amount;
    private LocalDate date;
    
    public Transaction(String id, String category, double amount, LocalDate date) {
        this.id = id;
        this.category = category;
        this.amount = amount;
        this.date = date;
    }
    
    public String getId() { return id; }
    public String getCategory() { return category; }
    public double getAmount() { return amount; }
    public LocalDate getDate() { return date; }
    
    @Override
    public String toString() {
        return String.format("%s: %s $%.2f (%s)", id, category, amount, date);
    }
}

public class TransactionAnalysis {
    public static void main(String[] args) {
        List<Transaction> transactions = Arrays.asList(
            new Transaction("T001", "Food", 45.50, LocalDate.of(2024, 1, 15)),
            new Transaction("T002", "Transport", 120.00, LocalDate.of(2024, 1, 16)),
            new Transaction("T003", "Food", 32.75, LocalDate.of(2024, 1, 17)),
            new Transaction("T004", "Entertainment", 85.00, LocalDate.of(2024, 1, 18)),
            new Transaction("T005", "Food", 67.25, LocalDate.of(2024, 1, 20)),
            new Transaction("T006", "Transport", 95.50, LocalDate.of(2024, 1, 22)),
            new Transaction("T007", "Shopping", 250.00, LocalDate.of(2024, 1, 25)),
            new Transaction("T008", "Food", 41.80, LocalDate.of(2024, 1, 28))
        );
        
        // 1. Total par catégorie
        System.out.println("=== TOTAL PAR CATÉGORIE ===");
        Map<String, Double> totalByCategory = transactions.stream()
            .collect(Collectors.groupingBy(
                Transaction::getCategory,
                Collectors.summingDouble(Transaction::getAmount)
            ));
        totalByCategory.entrySet().stream()
            .sorted(Map.Entry.<String, Double>comparingByValue().reversed())
            .forEach(entry -> 
                System.out.printf("%s: $%.2f%n", entry.getKey(), entry.getValue())
            );
        
        // 2. Moyenne par catégorie
        System.out.println("\n=== MOYENNE PAR CATÉGORIE ===");
        Map<String, Double> avgByCategory = transactions.stream()
            .collect(Collectors.groupingBy(
                Transaction::getCategory,
                Collectors.averagingDouble(Transaction::getAmount)
            ));
        avgByCategory.forEach((cat, avg) -> 
            System.out.printf("%s: $%.2f%n", cat, avg)
        );
        
        // 3. Transactions > 100
        System.out.println("\n=== TRANSACTIONS > 100 ===");
        List<Transaction> highValue = transactions.stream()
            .filter(t -> t.getAmount() > 100)
            .sorted(Comparator.comparing(Transaction::getAmount).reversed())
            .collect(Collectors.toList());
        highValue.forEach(System.out::println);
        
        // 4. Total dépenses
        System.out.println("\n=== STATISTIQUES GLOBALES ===");
        double total = transactions.stream()
            .mapToDouble(Transaction::getAmount)
            .sum();
        System.out.printf("Total dépenses: $%.2f%n", total);
        
        DoubleSummaryStatistics stats = transactions.stream()
            .mapToDouble(Transaction::getAmount)
            .summaryStatistics();
        System.out.printf("Transaction min: $%.2f%n", stats.getMin());
        System.out.printf("Transaction max: $%.2f%n", stats.getMax());
        System.out.printf("Moyenne: $%.2f%n", stats.getAverage());
        
        // 5. Catégorie la plus dépensière
        System.out.println("\n=== CATÉGORIE LA PLUS DÉPENSIÈRE ===");
        Optional<Map.Entry<String, Double>> topCategory = totalByCategory.entrySet().stream()
            .max(Map.Entry.comparingByValue());
        topCategory.ifPresent(entry -> 
            System.out.printf("%s: $%.2f%n", entry.getKey(), entry.getValue())
        );
        
        // 6. Nombre de transactions par catégorie
        System.out.println("\n=== NOMBRE PAR CATÉGORIE ===");
        Map<String, Long> countByCategory = transactions.stream()
            .collect(Collectors.groupingBy(
                Transaction::getCategory,
                Collectors.counting()
            ));
        countByCategory.forEach((cat, count) -> 
            System.out.printf("%s: %d transactions%n", cat, count)
        );
        
        // 7. Transactions par semaine
        System.out.println("\n=== PAR SEMAINE ===");
        Map<Integer, List<Transaction>> byWeek = transactions.stream()
            .collect(Collectors.groupingBy(
                t -> t.getDate().get(java.time.temporal.ChronoField.ALIGNED_WEEK_OF_YEAR)
            ));
        byWeek.forEach((week, trans) -> {
            double weekTotal = trans.stream().mapToDouble(Transaction::getAmount).sum();
            System.out.printf("Semaine %d: %d transactions, $%.2f%n", 
                week, trans.size(), weekTotal);
        });
    }
}
```

### Exemple 4: Pipeline Complexe avec Optional
```java
import java.util.*;
import java.util.stream.*;

public class OptionalStreamExample {
    
    public static class User {
        private String username;
        private Optional<String> email;
        private Optional<Address> address;
        
        public User(String username, String email, Address address) {
            this.username = username;
            this.email = Optional.ofNullable(email);
            this.address = Optional.ofNullable(address);
        }
        
        public String getUsername() { return username; }
        public Optional<String> getEmail() { return email; }
        public Optional<Address> getAddress() { return address; }
    }
    
    public static class Address {
        private String city;
        private String country;
        
        public Address(String city, String country) {
            this.city = city;
            this.country = country;
        }
        
        public String getCity() { return city; }
        public String getCountry() { return country; }
    }
    
    public static void main(String[] args) {
        List<User> users = Arrays.asList(
            new User("alice", "alice@example.com", new Address("Paris", "France")),
            new User("bob", null, new Address("London", "UK")),
            new User("charlie", "charlie@example.com", null),
            new User("david", "david@example.com", new Address("Paris", "France")),
            new User("eve", null, null)
        );
        
        // 1. Emails présents
        System.out.println("=== EMAILS PRÉSENTS ===");
        List<String> emails = users.stream()
            .map(User::getEmail)
            .filter(Optional::isPresent)
            .map(Optional::get)
            .collect(Collectors.toList());
        System.out.println(emails);
        
        // 2. Utilisateurs à Paris
        System.out.println("\n=== UTILISATEURS À PARIS ===");
        List<String> parisUsers = users.stream()
            .filter(u -> u.getAddress()
                .map(Address::getCity)
                .filter(city -> city.equals("Paris"))
                .isPresent())
            .map(User::getUsername)
            .collect(Collectors.toList());
        System.out.println(parisUsers);
        
        // 3. Villes uniques
        System.out.println("\n=== VILLES UNIQUES ===");
        Set<String> cities = users.stream()
            .map(User::getAddress)
            .flatMap(Optional::stream)  // Java 9+
            .map(Address::getCity)
            .collect(Collectors.toSet());
        System.out.println(cities);
        
        // 4. Compter par pays
        System.out.println("\n=== UTILISATEURS PAR PAYS ===");
        Map<String, Long> byCountry = users.stream()
            .map(User::getAddress)
            .flatMap(Optional::stream)
            .collect(Collectors.groupingBy(
                Address::getCountry,
                Collectors.counting()
            ));
        byCountry.forEach((country, count) -> 
            System.out.printf("%s: %d utilisateurs%n", country, count)
        );
        
        // 5. Utilisateurs avec email ET adresse
        System.out.println("\n=== EMAIL ET ADRESSE ===");
        long completeProfiles = users.stream()
            .filter(u -> u.getEmail().isPresent() && u.getAddress().isPresent())
            .count();
        System.out.println("Profils complets: " + completeProfiles);
        
        // 6. Premier utilisateur à Paris avec email
        System.out.println("\n=== PREMIER À PARIS AVEC EMAIL ===");
        Optional<User> first = users.stream()
            .filter(u -> u.getEmail().isPresent())
            .filter(u -> u.getAddress()
                .map(Address::getCity)
                .filter(city -> city.equals("Paris"))
                .isPresent())
            .findFirst();
        
        first.ifPresent(u -> System.out.println(
            u.getUsername() + " - " + u.getEmail().orElse("N/A")
        ));
    }
}
```

---

## Points Clés du Chapter 10

✅ **Optional**:
   - Conteneur pour valeur nullable
   - `of()`, `ofNullable()`, `empty()`
   - `isPresent()`, `isEmpty()`, `ifPresent()`
   - `orElse()`, `orElseGet()`, `orElseThrow()`
   - `map()`, `flatMap()`, `filter()`

✅ **Stream Basics**:
   - Séquence d'éléments
   - **Lazy evaluation**: exécution différée
   - **Consommable**: usage unique
   - Pipeline: source → intermédiaires → terminale

✅ **Intermediate Operations** (retournent Stream):
   - `filter()`, `map()`, `flatMap()`
   - `distinct()`, `sorted()`, `limit()`, `skip()`
   - `peek()`

✅ **Terminal Operations** (retournent résultat):
   - `forEach()`, `count()`, `collect()`
   - `reduce()`, `min()`, `max()`
   - `anyMatch()`, `allMatch()`, `noneMatch()`
   - `findFirst()`, `findAny()`

✅ **Primitive Streams**:
   - `IntStream`, `LongStream`, `DoubleStream`
   - Méthodes: `sum()`, `average()`, `min()`, `max()`
   - `range()`, `rangeClosed()`
   - `mapToInt()`, `mapToLong()`, `mapToDouble()`, `boxed()`

✅ **Collectors**:
   - `toList()`, `toSet()`, `toMap()`
   - `joining()`, `counting()`, `summingInt()`, `averagingInt()`
   - `groupingBy()`, `partitioningBy()`
   - `filtering()`, `flatMapping()` (Java 9+)
   - `teeing()` (Java 12+)

✅ **Best Practices**:
   - Préférer streams pour code déclaratif
   - Éviter effets de bord dans lambdas
   - Utiliser primitive streams pour performance
   - Short-circuit operations quand possible

---

# Questions/Réponses Complètes - Chapter 10: Streams

## Questions sur Optional

**Q1: Quelle est la différence entre ces deux?**
```java
// Version 1
Optional<String> opt1 = Optional.of(null);

// Version 2
Optional<String> opt2 = Optional.ofNullable(null);
```

**R:**
- **Version 1**: Lance **NullPointerException**
- **Version 2**: Crée un **Optional vide** (safe)

```java
Optional.of(null);           // NullPointerException!
Optional.ofNullable(null);   // Optional.empty()
```

---

**Q2: Que se passe-t-il?**
```java
Optional<String> empty = Optional.empty();
String value = empty.get();
```

**R:** Lance **NoSuchElementException**.

`get()` sur un Optional vide lance une exception. Toujours vérifier avec `isPresent()` ou utiliser `orElse()`.

---

**Q3: Quelle est la différence entre orElse() et orElseGet()?**
```java
Optional<String> opt = Optional.of("Hello");

String s1 = opt.orElse(expensiveMethod());
String s2 = opt.orElseGet(() -> expensiveMethod());
```

**R:**
- **orElse()**: `expensiveMethod()` est **toujours appelée** (même si opt non vide)
- **orElseGet()**: `expensiveMethod()` est appelée **seulement si opt vide** (lazy)

```java
// orElse - toujours évalué
opt.orElse(System.out.println("Called"));  // Affiche "Called" même si opt plein

// orElseGet - lazy
opt.orElseGet(() -> { System.out.println("Called"); return ""; });  // N'affiche rien si opt plein
```

---

**Q4: Que retourne ce code?**
```java
Optional<String> opt = Optional.of("Hello");
Optional<Integer> length = opt.map(String::length);
System.out.println(length.get());
```

**R:** `5`

`map()` transforme la valeur dans l'Optional. Ici, transforme "Hello" en sa longueur (5).

---

**Q5: Quelle est la différence entre map() et flatMap()?**
```java
Optional<String> opt = Optional.of("Hello");

// Version 1
Optional<Optional<String>> nested = opt.map(s -> Optional.of(s.toUpperCase()));

// Version 2
Optional<String> flat = opt.flatMap(s -> Optional.of(s.toUpperCase()));
```

**R:**
- **map()**: retourne `Optional<Optional<String>>` (imbriqué)
- **flatMap()**: retourne `Optional<String>` (aplati)

`flatMap()` évite les Optional imbriqués quand la fonction retourne déjà un Optional.

---

**Q6: Que fait filter() sur Optional?**
```java
Optional<Integer> opt = Optional.of(42);

Optional<Integer> even = opt.filter(n -> n % 2 == 0);
Optional<Integer> odd = opt.filter(n -> n % 2 != 0);

System.out.println(even.isPresent());
System.out.println(odd.isPresent());
```

**R:**
```
true   // 42 est pair, garde la valeur
false  // 42 n'est pas impair, devient empty
```

`filter()` garde la valeur si elle satisfait le prédicat, sinon retourne empty.

---

**Q7: Comment chaîner plusieurs opérations sur Optional?**
```java
Optional<String> opt = Optional.of("  hello  ");
// Trim, uppercase, et garder si longueur > 3
```

**R:**
```java
Optional<String> result = opt
    .map(String::trim)
    .map(String::toUpperCase)
    .filter(s -> s.length() > 3);

System.out.println(result.get());  // "HELLO"
```

---

**Q8: Pourquoi utiliser Optional plutôt que null?**

**R:** Avantages d'Optional:
1. **Explicite**: indique clairement qu'une valeur peut être absente
2. **Évite NullPointerException**: force le développeur à gérer l'absence
3. **API fluent**: chaînage avec map, filter, flatMap
4. **Code plus sûr**: moins d'oublis de vérification null

```java
// AVANT (avec null)
public String getUserEmail(Long id) {
    User user = findUser(id);
    if (user != null) {
        Email email = user.getEmail();
        if (email != null) {
            return email.getValue();
        }
    }
    return "no-email@default.com";
}

// APRÈS (avec Optional)
public String getUserEmail(Long id) {
    return findUser(id)
        .flatMap(User::getEmail)
        .map(Email::getValue)
        .orElse("no-email@default.com");
}
```

---

**Q9: Que fait ifPresent()?**
```java
Optional<String> opt = Optional.of("Hello");
opt.ifPresent(System.out::println);
```

**R:** Exécute le **Consumer** seulement si l'Optional contient une valeur.

```java
Optional<String> empty = Optional.empty();
empty.ifPresent(System.out::println); // Ne fait rien

Optional<String> full = Optional.of("Hello");
full.ifPresent(System.out::println);  // Affiche "Hello"

// ifPresentOrElse (Java 9+)
full.ifPresentOrElse(
    System.out::println,                    // Si présent
    () -> System.out.println("Empty!")      // Si vide
);
```

---

**Q10: Comment gérer les exceptions avec Optional?**
```java
String input = "abc";
// Parser en Integer, retourner Optional
```

**R:**
```java
public Optional<Integer> parseInteger(String input) {
    try {
        return Optional.of(Integer.parseInt(input));
    } catch (NumberFormatException e) {
        return Optional.empty();
    }
}

// Utilisation
Optional<Integer> number = parseInteger("123");
int value = number.orElse(0);  // 123

Optional<Integer> invalid = parseInteger("abc");
int value2 = invalid.orElse(0);  // 0
```

---

## Questions sur les Streams (Basics)

**Q11: Ce code compile-t-il?**
```java
Stream<String> stream = Stream.of("A", "B", "C");
stream.forEach(System.out::println);
stream.count();
```

**R:** Non, lance **IllegalStateException** sur `stream.count()`.

Un stream ne peut être utilisé qu'**une seule fois**. Après `forEach()`, le stream est consommé.

---

**Q12: Que s'affiche-t-il?**
```java
Stream<Integer> stream = Stream.of(1, 2, 3, 4, 5)
    .filter(n -> {
        System.out.println("Filter: " + n);
        return n % 2 == 0;
    });

System.out.println("Stream créé");
```

**R:** `"Stream créé"` **seulement**.

Les streams sont **lazy**: les opérations intermédiaires ne s'exécutent pas tant qu'il n'y a pas d'opération terminale.

---

**Q13: Quelle est la différence?**
```java
// Version 1
IntStream.range(1, 5);

// Version 2
IntStream.rangeClosed(1, 5);
```

**R:**
- **range()**: `[1, 5)` → `1, 2, 3, 4` (borne supérieure **exclusive**)
- **rangeClosed()**: `[1, 5]` → `1, 2, 3, 4, 5` (borne supérieure **inclusive**)

---

**Q14: Comment créer un stream infini?**

**R:** Deux façons:

```java
// iterate() - avec fonction
Stream<Integer> infinite1 = Stream.iterate(0, n -> n + 2);  // 0, 2, 4, 6...

// generate() - avec Supplier
Stream<Double> infinite2 = Stream.generate(Math::random);

// ATTENTION: limiter avec limit()
infinite1.limit(5).forEach(System.out::println);  // 0, 2, 4, 6, 8
```

---

**Q15: Quelle est la différence entre iterate() avec 2 et 3 paramètres?**
```java
// Version 1 (Java 8)
Stream<Integer> stream1 = Stream.iterate(0, n -> n + 1);

// Version 2 (Java 9+)
Stream<Integer> stream2 = Stream.iterate(0, n -> n < 10, n -> n + 1);
```

**R:**
- **Version 1**: Stream **infini** (nécessite `limit()`)
- **Version 2**: Stream avec **condition d'arrêt** (comme un for loop)

```java
// Version 1 - infini
Stream.iterate(0, n -> n + 1)
    .limit(5)
    .forEach(System.out::println);  // 0, 1, 2, 3, 4

// Version 2 - avec condition
Stream.iterate(0, n -> n < 5, n -> n + 1)
    .forEach(System.out::println);  // 0, 1, 2, 3, 4
```

---

**Q16: Comment créer un Stream depuis différentes sources?**

**R:**
```java
// Depuis une Collection
List<String> list = Arrays.asList("a", "b", "c");
Stream<String> stream1 = list.stream();

// Depuis un Array
String[] array = {"a", "b", "c"};
Stream<String> stream2 = Arrays.stream(array);
Stream<String> stream3 = Stream.of(array);

// Depuis des valeurs
Stream<String> stream4 = Stream.of("a", "b", "c");

// Stream vide
Stream<String> empty = Stream.empty();

// Depuis un fichier
Stream<String> lines = Files.lines(Paths.get("file.txt"));

// Depuis une String
IntStream chars = "Hello".chars();

// Builder pattern
Stream<String> stream5 = Stream.<String>builder()
    .add("a")
    .add("b")
    .build();
```

---

**Q17: Quelle est la différence entre Stream et Collection?**

**R:**

| **Stream** | **Collection** |
|------------|----------------|
| Pas de stockage | Stocke les éléments |
| Lazy (évaluation différée) | Eager (évaluation immédiate) |
| Usage unique | Réutilisable |
| Peut être infini | Toujours fini |
| Opérations fonctionnelles | Opérations mutables |

```java
// Collection - stockage
List<String> list = new ArrayList<>();
list.add("a");  // Modifie la structure

// Stream - pipeline
list.stream()
    .filter(s -> s.length() > 1)
    .map(String::toUpperCase);  // Ne modifie PAS la list
```

---

## Questions sur les Opérations Intermédiaires

**Q18: Que fait flatMap()?**
```java
List<List<Integer>> nested = Arrays.asList(
    Arrays.asList(1, 2),
    Arrays.asList(3, 4, 5),
    Arrays.asList(6)
);

List<Integer> flat = nested.stream()
    .flatMap(List::stream)
    .collect(Collectors.toList());

System.out.println(flat);
```

**R:** `[1, 2, 3, 4, 5, 6]`

`flatMap()` **aplatit** une structure imbriquée. Transforme `Stream<List<Integer>>` en `Stream<Integer>`.

---

**Q19: Quelle est la différence entre map() et flatMap()?**

**R:**
- **map()**: transforme chaque élément (1-à-1)
- **flatMap()**: transforme chaque élément en Stream et aplatit (1-à-plusieurs)

```java
// map() - 1 élément → 1 élément
List<Integer> lengths = Arrays.asList("a", "bb", "ccc").stream()
    .map(String::length)
    .collect(Collectors.toList());  // [1, 2, 3]

// flatMap() - 1 élément → Stream d'éléments
List<Character> chars = Arrays.asList("ab", "cd").stream()
    .flatMap(s -> s.chars().mapToObj(c -> (char)c))
    .collect(Collectors.toList());  // [a, b, c, d]
```

---

**Q20: Que fait distinct()?**
```java
List<Integer> result = Arrays.asList(1, 2, 2, 3, 3, 3, 4).stream()
    .distinct()
    .collect(Collectors.toList());

System.out.println(result);
```

**R:** `[1, 2, 3, 4]`

`distinct()` supprime les **doublons** (utilise `equals()` pour comparer).

---

**Q21: Comment fonctionne distinct() avec des objets custom?**
```java
class Person {
    String name;
    int age;
    
    Person(String name, int age) {
        this.name = name;
        this.age = age;
    }
}

List<Person> people = Arrays.asList(
    new Person("Alice", 25),
    new Person("Alice", 25),
    new Person("Bob", 30)
);

List<Person> unique = people.stream()
    .distinct()
    .collect(Collectors.toList());
```

**R:** Sans `equals()` et `hashCode()`, tous les objets sont considérés **différents** (3 éléments).

**Pour que distinct() fonctionne**, override `equals()` et `hashCode()`:

```java
@Override
public boolean equals(Object o) {
    if (this == o) return true;
    if (!(o instanceof Person)) return false;
    Person person = (Person) o;
    return age == person.age && Objects.equals(name, person.name);
}

@Override
public int hashCode() {
    return Objects.hash(name, age);
}
```

---

**Q22: Que fait skip()?**
```java
List<Integer> result = IntStream.range(1, 10)
    .skip(3)
    .boxed()
    .collect(Collectors.toList());

System.out.println(result);
```

**R:** `[4, 5, 6, 7, 8, 9]`

`skip(3)` **saute** les 3 premiers éléments.

---

**Q23: Comment obtenir les éléments 4 à 6 d'un stream?**
```java
IntStream.range(1, 11)  // 1 à 10
// Éléments 4, 5, 6 ?
```

**R:**
```java
List<Integer> result = IntStream.range(1, 11)
    .skip(3)   // Saute 1, 2, 3
    .limit(3)  // Prend 3 éléments
    .boxed()
    .collect(Collectors.toList());

System.out.println(result);  // [4, 5, 6]
```

---

**Q24: À quoi sert peek()?**
```java
List<Integer> result = Arrays.asList(1, 2, 3).stream()
    .peek(n -> System.out.println("Original: " + n))
    .map(n -> n * 2)
    .peek(n -> System.out.println("Doubled: " + n))
    .collect(Collectors.toList());
```

**R:** `peek()` est utilisé pour le **debugging**. Il exécute une action sur chaque élément sans les modifier.

Output:
```
Original: 1
Doubled: 2
Original: 2
Doubled: 4
Original: 3
Doubled: 6
```

---

**Q25: Quelle est la différence entre sorted() et sorted(Comparator)?**
```java
List<String> names = Arrays.asList("Charlie", "Alice", "Bob");

// Version 1
List<String> sorted1 = names.stream()
    .sorted()
    .collect(Collectors.toList());

// Version 2
List<String> sorted2 = names.stream()
    .sorted(Comparator.reverseOrder())
    .collect(Collectors.toList());
```

**R:**
- **sorted()**: tri **naturel** (ordre alphabétique pour String)
- **sorted(Comparator)**: tri **personnalisé**

```java
sorted1;  // [Alice, Bob, Charlie]
sorted2;  // [Charlie, Bob, Alice]

// Trier par longueur
names.stream()
    .sorted(Comparator.comparing(String::length))
    .collect(Collectors.toList());  // [Bob, Alice, Charlie]

// Trier par longueur puis alphabétique
names.stream()
    .sorted(Comparator.comparing(String::length)
                      .thenComparing(String::compareTo))
    .collect(Collectors.toList());
```

---

**Q26: Comment trier des objets custom?**
```java
class Employee {
    String name;
    int salary;
    
    Employee(String name, int salary) {
        this.name = name;
        this.salary = salary;
    }
}

List<Employee> employees = Arrays.asList(
    new Employee("Alice", 50000),
    new Employee("Bob", 60000),
    new Employee("Charlie", 45000)
);
// Trier par salaire décroissant
```

**R:**
```java
List<Employee> sorted = employees.stream()
    .sorted(Comparator.comparing(Employee::getSalary).reversed())
    .collect(Collectors.toList());

// Ou avec comparingInt (évite boxing)
List<Employee> sorted2 = employees.stream()
    .sorted(Comparator.comparingInt(Employee::getSalary).reversed())
    .collect(Collectors.toList());

// Trier par salary puis name
List<Employee> sorted3 = employees.stream()
    .sorted(Comparator.comparingInt(Employee::getSalary)
                      .thenComparing(Employee::getName))
    .collect(Collectors.toList());
```

---

**Q27: Que se passe-t-il avec sorted() sur un stream infini?**
```java
Stream.iterate(1, n -> n + 1)
    .sorted()
    .limit(5)
    .forEach(System.out::println);
```

**R:** Le programme **ne termine jamais** (ou OutOfMemoryError).

`sorted()` doit **charger tous les éléments** en mémoire pour trier. Sur un stream infini, impossible.

**Solution**: limiter **avant** de trier:
```java
Stream.iterate(1, n -> n + 1)
    .limit(5)      // D'abord limiter
    .sorted()      // Puis trier
    .forEach(System.out::println);  // 1, 2, 3, 4, 5
```

---

## Questions sur les Opérations Terminales

**Q28: Quelles sont les opérations terminales?**

**R:** Opérations qui **déclenchent** le pipeline et **consomment** le stream:

```java
// Réduction
long count = stream.count();
Optional<T> min = stream.min(comparator);
Optional<T> max = stream.max(comparator);
Optional<T> reduce = stream.reduce(accumulator);

// Recherche
Optional<T> findFirst = stream.findFirst();
Optional<T> findAny = stream.findAny();

// Matching
boolean anyMatch = stream.anyMatch(predicate);
boolean allMatch = stream.allMatch(predicate);
boolean noneMatch = stream.noneMatch(predicate);

// Collection
List<T> list = stream.collect(Collectors.toList());
void forEach = stream.forEach(consumer);

// Conversion en array
Object[] array = stream.toArray();
T[] array = stream.toArray(T[]::new);
```

---

**Q29: Quelle est la différence entre count() et Collectors.counting()?**

**R:**
- **count()**: opération **terminale** sur un stream
- **Collectors.counting()**: **collector** utilisé avec `collect()` ou `groupingBy()`

```java
// count() - opération terminale
long count1 = list.stream().count();

// counting() - collector
long count2 = list.stream().collect(Collectors.counting());

// Utile avec groupingBy
Map<String, Long> countByCategory = items.stream()
    .collect(Collectors.groupingBy(Item::getCategory, Collectors.counting()));
```

---

**Q30: Que retourne reduce() dans ces cas?**
```java
// Cas 1
Optional<Integer> sum1 = Arrays.asList(1, 2, 3).stream()
    .reduce((a, b) -> a + b);

// Cas 2
int sum2 = Arrays.asList(1, 2, 3).stream()
    .reduce(0, (a, b) -> a + b);

// Cas 3
int sum3 = Stream.<Integer>empty()
    .reduce(10, (a, b) -> a + b);
```

**R:**
- **Cas 1**: `Optional<Integer>` contenant `6` (peut être empty si stream vide)
- **Cas 2**: `int` valant `6` (retourne la valeur initiale si stream vide)
- **Cas 3**: `int` valant `10` (valeur initiale car stream vide)

---

**Q31: Comment fonctionne reduce() en détail?**
```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4);
int sum = numbers.stream()
    .reduce(0, (a, b) -> a + b);
```

**R:** Étapes de réduction:
```
Début: accumulator = 0 (identity)
Étape 1: accumulator = 0 + 1 = 1
Étape 2: accumulator = 1 + 2 = 3
Étape 3: accumulator = 3 + 3 = 6
Étape 4: accumulator = 6 + 4 = 10
Résultat: 10
```

**Les 3 formes de reduce:**
```java
// Forme 1: Pas d'identity
Optional<T> reduce(BinaryOperator<T> accumulator)

// Forme 2: Avec identity
T reduce(T identity, BinaryOperator<T> accumulator)

// Forme 3: Avec combiner (pour parallel streams)
<U> U reduce(U identity,
             BiFunction<U, T, U> accumulator,
             BinaryOperator<U> combiner)
```

---

**Q32: Quelle est la différence entre min()/max() et reduce()?**
```java
// Version 1
Optional<Integer> max1 = list.stream().max(Integer::compareTo);

// Version 2
Optional<Integer> max2 = list.stream().reduce(Integer::max);
```

**R:** **Aucune différence fonctionnelle** pour cet usage.

`max()` est juste une version spécialisée de `reduce()` pour trouver le maximum.

```java
// Équivalents
Optional<Integer> max = list.stream().max(Integer::compareTo);
Optional<Integer> max = list.stream().reduce((a, b) -> a > b ? a : b);
Optional<Integer> max = list.stream().reduce(Integer::max);
```

---

**Q33: Quelle est la différence entre anyMatch(), allMatch() et noneMatch()?**
```java
List<Integer> numbers = Arrays.asList(2, 4, 6, 8);

boolean result1 = numbers.stream().anyMatch(n -> n > 5);
boolean result2 = numbers.stream().allMatch(n -> n % 2 == 0);
boolean result3 = numbers.stream().noneMatch(n -> n < 0);
```

**R:**
```java
result1 = true   // anyMatch: AU MOINS UN > 5 (6, 8)
result2 = true   // allMatch: TOUS pairs
result3 = true   // noneMatch: AUCUN négatif
```

- **anyMatch()**: vrai si **au moins un** correspond
- **allMatch()**: vrai si **tous** correspondent
- **noneMatch()**: vrai si **aucun** ne correspond

**ATTENTION**: Sur stream vide:
```java
Stream.empty().anyMatch(x -> true);   // false
Stream.empty().allMatch(x -> false);  // true (vacuous truth)
Stream.empty().noneMatch(x -> true);  // true
```

---

**Q34: Quelle est la différence entre findFirst() et findAny()?**

**R:**
- **findFirst()**: retourne le **premier** élément (déterministe)
- **findAny()**: retourne **n'importe quel** élément (utile pour parallel streams)

```java
Optional<Integer> first = list.stream().findFirst();  // Toujours le même
Optional<Integer> any = list.parallelStream().findAny();  // Peut varier
```

En stream séquentiel, pas de différence pratique. En parallel stream, `findAny()` est plus rapide.

---

**Q35: Comment forEach() diffère-t-il de forEachOrdered()?**
```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5);

// Version 1
numbers.parallelStream().forEach(System.out::println);

// Version 2
numbers.parallelStream().forEachOrdered(System.out::println);
```

**R:**
- **forEach()**: ordre **non garanti** en parallel
- **forEachOrdered()**: **maintient l'ordre** même en parallel (plus lent)

```java
// forEach - ordre aléatoire en parallel
numbers.parallelStream().forEach(System.out::println);
// Peut afficher: 3, 1, 5, 2, 4

// forEachOrdered - ordre préservé
numbers.parallelStream().forEachOrdered(System.out::println);
// Affiche toujours: 1, 2, 3, 4, 5
```

---

**Q36: Comment convertir un Stream en Array?**
```java
Stream<String> stream = Stream.of("A", "B", "C");
```

**R:**
```java
// Object[]
Object[] array1 = stream.toArray();

// String[] - avec method reference
String[] array2 = stream.toArray(String[]::new);

// Équivalent à
String[] array3 = stream.toArray(size -> new String[size]);

// Pour primitive arrays
int[] intArray = IntStream.of(1, 2, 3).toArray();
```

---

## Questions sur les Primitive Streams

**Q37: Pourquoi utiliser IntStream plutôt que Stream<Integer>?**

**R:** **Performance**: évite l'**autoboxing/unboxing**.

```java
// Stream<Integer> - autoboxing (lent)
int sum1 = Stream.of(1, 2, 3, 4, 5)
    .mapToInt(Integer::intValue)
    .sum();

// IntStream - pas d'autoboxing (rapide)
int sum2 = IntStream.of(1, 2, 3, 4, 5)
    .sum();
```

**Économie mémoire**: Integer (16 bytes) vs int (4 bytes).

---

**Q38: Quelles sont les méthodes spécifiques aux primitive streams?**

**R:** Méthodes statistiques:

```java
IntStream stream = IntStream.of(1, 2, 3, 4, 5);

int sum = stream.sum();                // Somme
OptionalDouble avg = stream.average(); // Moyenne
OptionalInt max = stream.max();        // Maximum
OptionalInt min = stream.min();        // Minimum

// SummaryStatistics - toutes les stats en une passe
IntSummaryStatistics stats = stream.summaryStatistics();
stats.getCount();   // Nombre d'éléments
stats.getSum();     // Somme
stats.getMin();     // Minimum
stats.getMax();     // Maximum
stats.getAverage(); // Moyenne
```

---

**Q39: Comment convertir entre Stream<Integer> et IntStream?**

**R:**

```java
// Stream<Integer> → IntStream
Stream<Integer> boxed = Stream.of(1, 2, 3);
IntStream intStream = boxed.mapToInt(Integer::intValue);

// IntStream → Stream<Integer>
IntStream ints = IntStream.of(1, 2, 3);
Stream<Integer> stream = ints.boxed();

// LongStream → IntStream (avec cast)
LongStream longs = LongStream.of(1L, 2L, 3L);
IntStream ints2 = longs.mapToInt(l -> (int) l);

// IntStream → DoubleStream
IntStream ints3 = IntStream.of(1, 2, 3);
DoubleStream doubles = ints3.asDoubleStream();
```

---

**Q40: Que retourne average() sur un IntStream vide?**
```java
OptionalDouble avg = IntStream.empty().average();
System.out.println(avg.isPresent());
```

**R:** `false`

`average()` retourne un **OptionalDouble vide** si le stream est vide (pas d'exception).

```java
OptionalDouble avg = IntStream.empty().average();
double value = avg.orElse(0.0);  // 0.0
```

---

**Q41: Quelle est la différence entre mapToInt(), mapToLong(), mapToDouble()?**
```java
List<String> numbers = Arrays.asList("1", "2", "3");
```

**R:** Convertissent un Stream vers un primitive stream:

```java
// mapToInt - String → IntStream
IntStream intStream = numbers.stream()
    .mapToInt(Integer::parseInt);

// mapToLong - String → LongStream
LongStream longStream = numbers.stream()
    .mapToLong(Long::parseLong);

// mapToDouble - String → DoubleStream
DoubleStream doubleStream = numbers.stream()
    .mapToDouble(Double::parseDouble);

// Utilisation
int sum = numbers.stream()
    .mapToInt(Integer::parseInt)
    .sum();  // 6
```

---

**Q42: Comment utiliser summaryStatistics()?**
```java
List<Integer> numbers = Arrays.asList(10, 20, 30, 40, 50);
```

**R:**
```java
IntSummaryStatistics stats = numbers.stream()
    .mapToInt(Integer::intValue)
    .summaryStatistics();

System.out.println("Count: " + stats.getCount());     // 5
System.out.println("Sum: " + stats.getSum());         // 150
System.out.println("Min: " + stats.getMin());         // 10
System.out.println("Max: " + stats.getMax());         // 50
System.out.println("Average: " + stats.getAverage()); // 30.0
```

## Questions sur les Collectors

**Q43: Quelle est la différence?**
```java
// Version 1
List<String> list1 = stream.collect(Collectors.toList());

// Version 2
List<String> list2 = stream.toList();  // Java 16+
```

**R:**
- **toList()**: retourne une liste **immuable** (Java 16+)
- **Collectors.toList()**: retourne une liste **mutable** (modifiable)

```java
list1.add("New");  // OK
// list2.add("New");  // UnsupportedOperationException
```

---

**Q44: Comment joindre des strings avec Collectors?**
```java
List<String> words = Arrays.asList("Java", "is", "awesome");
// Joindre en "Java is awesome"
```

**R:**
```java
String joined = words.stream()
    .collect(Collectors.joining(" "));

// Avec préfixe et suffixe
String bracketed = words.stream()
    .collect(Collectors.joining(", ", "[", "]"));
// "[Java, is, awesome]"

// Avec filter
String filtered = words.stream()
    .filter(s -> s.length() > 2)
    .collect(Collectors.joining(", "));
// "Java, awesome"
```

---

**Q45: Comment grouper des éléments?**
```java
List<String> words = Arrays.asList("apple", "banana", "cherry", "date");
// Grouper par longueur
```

**R:**
```java
Map<Integer, List<String>> byLength = words.stream()
    .collect(Collectors.groupingBy(String::length));

System.out.println(byLength);
// {4=[date], 5=[apple], 6=[banana, cherry]}
```

---

**Q46: Comment compter les occurrences avec groupingBy()?**
```java
List<String> words = Arrays.asList("a", "b", "a", "c", "b", "a");
// Compter chaque mot
```

**R:**
```java
Map<String, Long> count = words.stream()
    .collect(Collectors.groupingBy(
        word -> word,
        Collectors.counting()
    ));

System.out.println(count);  // {a=3, b=2, c=1}

// Ou avec Function.identity()
Map<String, Long> count2 = words.stream()
    .collect(Collectors.groupingBy(
        Function.identity(),
        Collectors.counting()
    ));
```

---

**Q47: Quelle est la différence entre groupingBy() et partitioningBy()?**

**R:**
- **groupingBy()**: groupe par n'importe quelle clé (plusieurs groupes)
- **partitioningBy()**: groupe par boolean (exactement 2 groupes: true/false)

```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5, 6);

// groupingBy - plusieurs groupes
Map<String, List<Integer>> grouped = numbers.stream()
    .collect(Collectors.groupingBy(n -> {
        if (n < 3) return "small";
        else if (n < 5) return "medium";
        else return "large";
    }));
// {small=[1,2], medium=[3,4], large=[5,6]}

// partitioningBy - 2 groupes
Map<Boolean, List<Integer>> partitioned = numbers.stream()
    .collect(Collectors.partitioningBy(n -> n % 2 == 0));
// {false=[1,3,5], true=[2,4,6]}
```

**Avantage de partitioningBy**: Garantit **toujours 2 clés** (true et false), même si un groupe est vide.

---

**Q48: Comment créer une Map avec toMap()?**
```java
List<String> words = Arrays.asList("apple", "banana", "cherry");
// Map: mot → longueur
```

**R:**
```java
Map<String, Integer> map = words.stream()
    .collect(Collectors.toMap(
        word -> word,        // clé
        String::length       // valeur
    ));

System.out.println(map);  // {apple=5, banana=6, cherry=6}

// Avec Function.identity()
Map<String, Integer> map2 = words.stream()
    .collect(Collectors.toMap(
        Function.identity(),
        String::length
    ));
```

---

**Q49: Comment gérer les doublons avec toMap()?**
```java
List<String> words = Arrays.asList("a", "bb", "ccc", "dd");
// Map: longueur → mot (collision sur longueur 2)
```

**R:** Utiliser la **merge function**:

```java
Map<Integer, String> map = words.stream()
    .collect(Collectors.toMap(
        String::length,              // clé
        word -> word,                // valeur
        (existing, replacement) -> existing + "," + replacement  // merge
    ));

System.out.println(map);  // {1=a, 2=bb,dd, 3=ccc}

// Garder le premier
Map<Integer, String> map2 = words.stream()
    .collect(Collectors.toMap(
        String::length,
        Function.identity(),
        (first, second) -> first  // Garde le premier
    ));
// {1=a, 2=bb, 3=ccc}

// Garder le dernier
Map<Integer, String> map3 = words.stream()
    .collect(Collectors.toMap(
        String::length,
        Function.identity(),
        (first, second) -> second  // Garde le dernier
    ));
// {1=a, 2=dd, 3=ccc}
```

---

**Q50: Comment spécifier le type de Map avec toMap()?**
```java
List<String> words = Arrays.asList("c", "a", "b");
// TreeMap (triée) au lieu de HashMap
```

**R:**
```java
TreeMap<String, Integer> treeMap = words.stream()
    .collect(Collectors.toMap(
        Function.identity(),
        String::length,
        (v1, v2) -> v1,
        TreeMap::new  // Spécifie le type de Map
    ));

System.out.println(treeMap);  // {a=1, b=1, c=1} (ordre alphabétique)

// LinkedHashMap (préserve l'ordre d'insertion)
LinkedHashMap<String, Integer> linkedMap = words.stream()
    .collect(Collectors.toMap(
        Function.identity(),
        String::length,
        (v1, v2) -> v1,
        LinkedHashMap::new
    ));
// {c=1, a=1, b=1} (ordre d'insertion)
```

---

**Q51: Comment grouper et transformer simultanément?**
```java
class User {
    String name;
    String city;
    int age;
}

List<User> users = Arrays.asList(
    new User("Alice", "Paris", 25),
    new User("Bob", "Paris", 30),
    new User("Charlie", "Lyon", 35)
);
// Grouper par ville et obtenir les noms en UPPERCASE
```

**R:**
```java
Map<String, List<String>> result = users.stream()
    .collect(Collectors.groupingBy(
        User::getCity,
        Collectors.mapping(
            user -> user.getName().toUpperCase(),
            Collectors.toList()
        )
    ));

System.out.println(result);
// {Paris=[ALICE, BOB], Lyon=[CHARLIE]}
```

---

**Q52: Comment obtenir la moyenne par groupe?**
```java
class Student {
    String subject;
    int score;
    
    Student(String subject, int score) {
        this.subject = subject;
        this.score = score;
    }
}

List<Student> students = Arrays.asList(
    new Student("Math", 15),
    new Student("Math", 18),
    new Student("Physics", 12),
    new Student("Physics", 16)
);
// Moyenne par matière
```

**R:**
```java
Map<String, Double> averages = students.stream()
    .collect(Collectors.groupingBy(
        Student::getSubject,
        Collectors.averagingInt(Student::getScore)
    ));

System.out.println(averages);
// {Math=16.5, Physics=14.0}

// Ou avec summaryStatistics pour plus d'infos
Map<String, IntSummaryStatistics> stats = students.stream()
    .collect(Collectors.groupingBy(
        Student::getSubject,
        Collectors.summarizingInt(Student::getScore)
    ));
```

---

**Q53: Comment collecter en Set au lieu de List avec groupingBy()?**
```java
List<String> words = Arrays.asList("apple", "banana", "apricot", "blueberry");
// Grouper par première lettre, sans doublons
```

**R:**
```java
Map<Character, Set<String>> grouped = words.stream()
    .collect(Collectors.groupingBy(
        word -> word.charAt(0),
        Collectors.toSet()  // Set au lieu de List
    ));

System.out.println(grouped);
// {a=[apple, apricot], b=[banana, blueberry]}

// Avec TreeSet (trié)
Map<Character, TreeSet<String>> sorted = words.stream()
    .collect(Collectors.groupingBy(
        word -> word.charAt(0),
        Collectors.toCollection(TreeSet::new)
    ));
```

---

**Q54: Comment filtrer après groupingBy()?**
```java
List<String> words = Arrays.asList("a", "bb", "ccc", "dd", "eee", "f");
// Grouper par longueur, garder seulement les groupes avec > 1 élément
```

**R:**
```java
// Méthode 1: Filtrer après collection
Map<Integer, List<String>> grouped = words.stream()
    .collect(Collectors.groupingBy(String::length))
    .entrySet().stream()
    .filter(entry -> entry.getValue().size() > 1)
    .collect(Collectors.toMap(
        Map.Entry::getKey,
        Map.Entry::getValue
    ));

System.out.println(grouped);  // {2=[bb, dd], 3=[ccc, eee]}

// Méthode 2: filtering collector (Java 9+)
Map<Integer, List<String>> grouped2 = words.stream()
    .collect(Collectors.groupingBy(
        String::length,
        Collectors.filtering(
            word -> words.stream()
                .filter(w -> w.length() == word.length())
                .count() > 1,
            Collectors.toList()
        )
    ));
```

---

**Q55: Comment collecter en plusieurs structures simultanément?**
```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5);
// Obtenir la somme ET le count en une seule passe
```

**R:** Utiliser **teeing** (Java 12+):

```java
record Result(int sum, long count) {}

Result result = numbers.stream()
    .collect(Collectors.teeing(
        Collectors.summingInt(Integer::intValue),
        Collectors.counting(),
        (sum, count) -> new Result(sum, count)
    ));

System.out.println(result); // Result[sum=15, count=5]

// Sans record (Java 8+)
class Result {
    int sum;
    long count;
    Result(int sum, long count) {
        this.sum = sum;
        this.count = count;
    }
}

// Ou avec IntSummaryStatistics (plus simple)
IntSummaryStatistics stats = numbers.stream()
    .mapToInt(Integer::intValue)
    .summaryStatistics();
```

---

**Q56: Comment utiliser partitioningBy avec downstream collector?**
```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5, 6, 7, 8);
// Partitionner pairs/impairs et compter chaque groupe
```

**R:**
```java
Map<Boolean, Long> counts = numbers.stream()
    .collect(Collectors.partitioningBy(
        n -> n % 2 == 0,
        Collectors.counting()
    ));

System.out.println(counts);  // {false=4, true=4}

// Avec somme
Map<Boolean, Integer> sums = numbers.stream()
    .collect(Collectors.partitioningBy(
        n -> n % 2 == 0,
        Collectors.summingInt(Integer::intValue)
    ));
// {false=16, true=20}

// Avec liste des valeurs
Map<Boolean, String> joined = numbers.stream()
    .collect(Collectors.partitioningBy(
        n -> n % 2 == 0,
        Collectors.mapping(
            Object::toString,
            Collectors.joining(", ")
        )
    ));
// {false=1, 3, 5, 7, true=2, 4, 6, 8}
```

---

**Q57: Comment utiliser collectingAndThen()?**
```java
List<String> words = Arrays.asList("apple", "banana", "cherry");
// Collecter en List puis la rendre immuable
```

**R:**
```java
List<String> immutableList = words.stream()
    .collect(Collectors.collectingAndThen(
        Collectors.toList(),
        Collections::unmodifiableList
    ));

// immutableList.add("new"); // UnsupportedOperationException

// Autre exemple: collecter puis trier
List<String> sortedList = words.stream()
    .collect(Collectors.collectingAndThen(
        Collectors.toList(),
        list -> {
            Collections.sort(list);
            return list;
        }
    ));

// Compter puis transformer
String countMessage = words.stream()
    .collect(Collectors.collectingAndThen(
        Collectors.counting(),
        count -> "Total: " + count
    ));
// "Total: 3"
```

---

## Questions Pièges et Cas Complexes

**Q58: Que s'affiche-t-il?**
```java
Stream.of("A", "B", "C")
    .peek(System.out::println);
```

**R:** **Rien**.

`peek()` est une opération **intermédiaire**. Sans opération **terminale**, le stream n'est jamais exécuté (lazy).

**Correct**:
```java
Stream.of("A", "B", "C")
    .peek(System.out::println)
    .count();  // Opération terminale
// Affiche: A, B, C
```

---

**Q59: Que se passe-t-il?**
```java
List<Integer> list = Arrays.asList(1, 2, 3, 4, 5);

list.stream()
    .filter(n -> n % 2 == 0)
    .map(n -> n * 2);

System.out.println(list);
```

**R:** Affiche `[1, 2, 3, 4, 5]` (inchangé).

Les streams **ne modifient pas** la source. Il manque une opération terminale pour collecter le résultat.

**Correct**:
```java
List<Integer> result = list.stream()
    .filter(n -> n % 2 == 0)
    .map(n -> n * 2)
    .collect(Collectors.toList());

System.out.println(result);  // [4, 8]
```

---

**Q60: Ce code compile-t-il?**
```java
IntStream.of(1, 2, 3)
    .map(n -> n * 2.0)
    .sum();
```

**R:** Non, **erreur de compilation**: "incompatible types: possible lossy conversion from double to int".

`IntStream.map()` attend une **IntUnaryOperator** (int → int), pas (int → double).

**Correct**:
```java
DoubleStream.of(1, 2, 3)
    .map(n -> n * 2.0)
    .sum();

// OU
IntStream.of(1, 2, 3)
    .mapToDouble(n -> n * 2.0)
    .sum();
```

---

**Q61: Que retourne ce code?**
```java
List<String> list = Arrays.asList("a", "b", "c");

String result = list.stream()
    .reduce((s1, s2) -> s1 + s2)
    .get();

System.out.println(result);
```

**R:** `"abc"`

`reduce()` concatène toutes les strings: "a" + "b" = "ab", "ab" + "c" = "abc".

---

**Q62: Pourquoi ce code lance-t-il une exception?**
```java
Optional<String> result = Arrays.asList("a", "b", "c").stream()
    .filter(s -> s.length() > 5)
    .findFirst();

System.out.println(result.get());
```

**R:** **NoSuchElementException**.

Aucun élément ne satisfait le filtre (tous ont longueur ≤ 1), donc `findFirst()` retourne un **Optional vide**.

**Correct**:
```java
String value = result.orElse("Not found");
// Ou
if (result.isPresent()) {
    System.out.println(result.get());
}
```

---

**Q63: Que fait ce code?**
```java
Stream.iterate(1, n -> n + 1)
    .limit(5)
    .forEach(System.out::println);
```

**R:** Affiche `1 2 3 4 5`.

`iterate()` crée un stream **infini**. `limit(5)` prend les 5 premiers éléments.

---

**Q64: Ce code compile-t-il?**
```java
List<String> result = Stream.of("A", "B", "C")
    .sorted()
    .collect(Collectors.toList())
    .stream()
    .map(String::toLowerCase)
    .collect(Collectors.toList());
```

**R:** Oui, compile.

On peut créer un **nouveau stream** à partir du résultat d'un stream précédent. Chaque `.stream()` crée un nouveau stream.

---

**Q65: Que s'affiche-t-il?**
```java
long count = Stream.of("A", "B", "C")
    .filter(s -> s.equals("D"))
    .count();

System.out.println(count);
```

**R:** `0`

Aucun élément ne satisfait le filtre, donc `count()` retourne 0.

---

**Q66: Comment éviter NullPointerException avec Optional?**
```java
String value = null;
String result = Optional.ofNullable(value)
    .map(String::toUpperCase)
    .orElse("DEFAULT");

System.out.println(result);
```

**R:** `"DEFAULT"`

Si `value` est null, `map()` n'est **pas exécuté**, et `orElse()` retourne la valeur par défaut.

---

**Q67: Que fait cette pipeline?**
```java
List<Integer> result = Arrays.asList("1", "2", "3", "a", "4").stream()
    .map(s -> {
        try {
            return Optional.of(Integer.parseInt(s));
        } catch (NumberFormatException e) {
            return Optional.<Integer>empty();
        }
    })
    .flatMap(Optional::stream)
    .collect(Collectors.toList());

System.out.println(result);
```

**R:** `[1, 2, 3, 4]`

Parse les strings en Integer, ignore les erreurs ("a" est ignoré), et collecte les résultats valides.

`Optional::stream` (Java 9+) convertit:
- `Optional.of(1)` → Stream avec 1 élément
- `Optional.empty()` → Stream vide

---

**Q68: Quelle est la différence?**
```java
// Version 1
list.stream().forEach(System.out::println);

// Version 2
list.forEach(System.out::println);
```

**R:** **Aucune différence fonctionnelle** pour cet usage.

`Collection.forEach()` (Java 8) est plus direct et légèrement plus efficace (pas de création de stream).

**Mais attention en parallel**:
```java
list.parallelStream().forEach(System.out::println);  // Ordre non garanti
list.forEach(System.out::println);                    // Ordre préservé
```

---

**Q69: Comment obtenir un Set trié depuis un Stream?**
```java
Stream<String> stream = Stream.of("C", "A", "B");
// Set trié ?
```

**R:**
```java
Set<String> sorted = stream
    .collect(Collectors.toCollection(TreeSet::new));

System.out.println(sorted);  // [A, B, C]

// Avec Comparator personnalisé
TreeSet<String> custom = stream
    .collect(Collectors.toCollection(() -> 
        new TreeSet<>(Comparator.reverseOrder())
    ));
// [C, B, A]
```

---

**Q70: Ce code compile-t-il?**
```java
Optional<String> opt = Optional.of("Hello");
String result = opt
    .filter(s -> s.length() > 10)
    .get();
```

**R:** Compile mais lance **NoSuchElementException**.

Le filtre échoue (longueur = 5), donc `opt` devient vide, et `get()` lance une exception.

**Correct**:
```java
String result = opt
    .filter(s -> s.length() > 10)
    .orElse("Too short");
```

---

**Q71: Pourquoi ce code est-il inefficace?**
```java
boolean hasEven = IntStream.range(1, 1000000)
    .filter(n -> n % 2 == 0)
    .count() > 0;
```

**R:** Il parcourt **tous** les éléments pour compter.

**Mieux** (court-circuit):
```java
boolean hasEven = IntStream.range(1, 1000000)
    .anyMatch(n -> n % 2 == 0);  // S'arrête dès qu'un pair est trouvé
```

Les opérations de **court-circuit**: `anyMatch()`, `allMatch()`, `noneMatch()`, `findFirst()`, `findAny()`, `limit()`.

---

**Q72: Comment compter les éléments uniques?**
```java
List<String> words = Arrays.asList("a", "b", "a", "c", "b");
// Compter les uniques
```

**R:**
```java
long unique = words.stream()
    .distinct()
    .count();

System.out.println(unique);  // 3

// Ou avec Set
long unique2 = new HashSet<>(words).size();  // Plus direct
```

---

**Q73: Que retourne ce code?**
```java
List<Integer> result = IntStream.rangeClosed(1, 10)
    .boxed()
    .filter(n -> n % 2 == 0)
    .map(n -> n * n)
    .collect(Collectors.toList());

System.out.println(result);
```

**R:** `[4, 16, 36, 64, 100]`

1. Prend 1 à 10
2. Filtre les pairs: 2, 4, 6, 8, 10
3. Carré de chaque: 4, 16, 36, 64, 100

---

**Q74: Comment transformer une Map en Stream?**
```java
Map<String, Integer> map = Map.of("A", 1, "B", 2, "C", 3);
// Stream des valeurs > 1
```

**R:**
```java
// Stream des valeurs
List<Integer> values = map.values().stream()
    .filter(v -> v > 1)
    .collect(Collectors.toList());
// [2, 3]

// Stream des clés
List<String> keys = map.keySet().stream()
    .filter(k -> map.get(k) > 1)
    .collect(Collectors.toList());
// [B, C]

// Stream des entries
List<String> entries = map.entrySet().stream()
    .filter(entry -> entry.getValue() > 1)
    .map(entry -> entry.getKey() + "=" + entry.getValue())
    .collect(Collectors.toList());
// [B=2, C=3]
```

---

**Q75: Quelle est la différence entre ces deux?**
```java
// Version 1
OptionalDouble avg1 = IntStream.of(1, 2, 3).average();

// Version 2
Double avg2 = Stream.of(1, 2, 3)
    .collect(Collectors.averagingInt(Integer::intValue));
```

**R:**
- **Version 1**: retourne `OptionalDouble` (peut être vide)
- **Version 2**: retourne `Double` (jamais null, retourne NaN si vide)

```java
OptionalDouble avg1 = IntStream.empty().average();
System.out.println(avg1.isPresent());  // false

Double avg2 = Stream.<Integer>empty()
    .collect(Collectors.averagingInt(Integer::intValue));
System.out.println(avg2);  // NaN
```

---

**Q76: Comment obtenir les 3 premiers éléments distincts?**
```java
List<Integer> numbers = Arrays.asList(1, 2, 2, 3, 3, 4, 5, 5);
```

**R:**
```java
List<Integer> result = numbers.stream()
    .distinct()
    .limit(3)
    .collect(Collectors.toList());

System.out.println(result);  // [1, 2, 3]
```

---

**Q77: Que fait flatMap() dans ce contexte?**
```java
List<Optional<String>> optionals = Arrays.asList(
    Optional.of("A"),
    Optional.empty(),
    Optional.of("B")
);

List<String> result = optionals.stream()
    .flatMap(Optional::stream)
    .collect(Collectors.toList());

System.out.println(result);
```

**R:** `[A, B]`

`Optional::stream` (Java 9+) convertit:
- `Optional.of("A")` → Stream avec 1 élément
- `Optional.empty()` → Stream vide

`flatMap()` aplatit tout en un seul stream, filtrant automatiquement les empty.

---

**Q78: Ce code a-t-il un problème?**
```java
List<String> list = Arrays.asList("a", "b", "c");
list.stream()
    .map(String::toUpperCase)
    .forEach(s -> list.add(s + "!"));
```

**R:** Oui! **ConcurrentModificationException**.

On **modifie la collection source** pendant l'itération (même via stream).

**Correct**:
```java
List<String> result = list.stream()
    .map(String::toUpperCase)
    .map(s -> s + "!")
    .collect(Collectors.toList());

list.addAll(result); // Ajoute après
```

---

**Q79: Comment gérer les nulls dans un Stream?**
```java
List<String> words = Arrays.asList("apple", null, "banana", null, "cherry");
// Filtrer les nulls
```

**R:**
```java
// Méthode 1: filter
List<String> filtered = words.stream()
    .filter(Objects::nonNull)
    .collect(Collectors.toList());
// [apple, banana, cherry]

// Méthode 2: flatMap avec Optional (Java 9+)
List<String> filtered2 = words.stream()
    .flatMap(Optional::ofNullable)
    .flatMap(Optional::stream)
    .collect(Collectors.toList());

// Méthode 3: filter explicite
List<String> filtered3 = words.stream()
    .filter(w -> w != null)
    .collect(Collectors.toList());
```

---

**Q80: Que fait takeWhile() et dropWhile()? (Java 9+)**
```java
List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 3, 2, 1);

// takeWhile
List<Integer> take = numbers.stream()
    .takeWhile(n -> n < 4)
    .collect(Collectors.toList());

// dropWhile
List<Integer> drop = numbers.stream()
    .dropWhile(n -> n < 4)
    .collect(Collectors.toList());
```

**R:**
```java
take = [1, 2, 3]        // Prend JUSQU'À ce que la condition soit fausse
drop = [4, 3, 2, 1]     // Saute JUSQU'À ce que la condition soit fausse
```

**Différence avec filter**:
```java
// takeWhile - s'arrête à la première condition fausse
numbers.stream()
    .takeWhile(n -> n < 4)
    .collect(Collectors.toList());
// [1, 2, 3]

// filter - teste tous les éléments
numbers.stream()
    .filter(n -> n < 4)
    .collect(Collectors.toList());
// [1, 2, 3, 3, 2, 1]
```

---

Veux-tu que je continue avec des questions sur la **performance**, le **parallel processing**, ou des **cas pratiques d'entretien** ?