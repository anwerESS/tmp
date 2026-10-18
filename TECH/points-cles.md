<details>

<summary> ✅Java </summary>

<details>
<summary> APIs de base (Strings, Arrays, Math, Dates)</summary>

✅ **String** : Immuable, utiliser `equals()` pour comparer, pool de chaînes pour les littéraux  
✅ **StringBuilder** : Mutable, utilisé pour construire des chaînes, plus efficace que les concaténations multiples  
✅ **Méthodes String** :
`length()`, `charAt()`, `indexOf()`, `substring()`  
`toLowerCase()`, `toUpperCase()`, `trim()`, `strip()`  
`replace()`, `startsWith()`, `endsWith()`, `contains()`  
✅ **Méthodes StringBuilder** :  
`append()`, `insert()`, `delete()`, `deleteCharAt()`  
`replace()`, `reverse()`, `toString()`  
✅ **Arrays** :  
Taille fixe, indexé de 0 à `length-1`  
`Arrays.sort()`, `Arrays.binarySearch()` (tableau trié requis)  
`Arrays.equals()`, `Arrays.toString()`, `Arrays.copyOf()`  
✅ **Math** :  
`abs()`, `min()`, `max()`, `pow()`, `sqrt()`  
`round()`, `ceil()`, `floor()`, `random()`  
✅ **API Date/Heure** (java.time) :  
**LocalDate** : date sans heure  
**LocalTime** : heure sans date  
**LocalDateTime** : date et heure  
**Immuable** : les méthodes retournent de nouveaux objets  
✅ **Period vs Duration** :  
**Period** : années, mois, jours (pour LocalDate)  
**Duration** : heures, minutes, secondes (pour LocalTime)  
✅ **Manipulation de dates** :  
`plusDays()`, `plusMonths()`, `plusYears()`  
`minusDays()`, `minusMonths()`, `minusYears()`  
`isBefore()`, `isAfter()`, `isEqual()`  
✅ **Formatage** : `DateTimeFormatter` avec des patterns personnalisés

</details>



<br>

<details>
<summary> Object (equals(), hashCode(), ...)</summary>

**Toutes les classes** héritent implicitement de `java.lang.Object`.  
**Méthodes héritées d'Object** :  
`toString()` : représentation textuelle de l'objet  
`equals(Object obj)` : comparaison d'égalité  
`hashCode()` : code de hachage  
`getClass()` : infos sur la classe  
`clone()` : copie de l'objet  
```java
    @Override
    public boolean equals(Object o) {
        if (!(o instanceof Person person)) return false;
        return Objects.equals(name, person.name) &&
                Objects.equals(address, person.address);
    }
    @Override
    public int hashCode() {
        return Objects.hash(name, address);
    }
```
**Ordre d'initialisation des objets**  
  `1)` Membres statiques de la superclasse  
  `2)` Membres statiques de la classe  
  `3)` Membres d'instance de la superclasse  
  `4)` Constructeur de la superclasse  
  `5)` Membres d'instance de la classe  
  `6)` Constructeur de la classe  
</details>

<details>
<summary> Interfaces, Enums, Sealed, Records, Nested Classes, Polymorphisme</summary>


✅ **Interfaces vs Abstract Class** :   
Une interface définit un contrat de methode que les classe doivent implementer.  
Une classe abstraite peut aussi définir un contrat, mais elle peut contenir des attributs, des méthodes déjà implémentées, et une logique commune.  
En général, j’utilise une interface pour découpler et définir un comportement.  
J’utilise une classe abstraite quand plusieurs classes partagent une logique commune.  
✅ **Enums** :  
Type pour des constantes fixes  
Méthodes : `values()`, `valueOf()`, `ordinal()`, `name()`  
Peut avoir des champs, des constructeurs (privés), des méthodes  
Peut implémenter des interfaces  
Méthodes abstraites possibles  
✅ **Classes Sealed** (Java 17+) :  
Contrôle les sous-classes avec `permits`  
Les sous-classes doivent être `final`, `sealed`, ou `non-sealed`  
Pattern matching exhaustif  
✅ **Records** (Java 16+) :  
Classes de données immuables  
Génère automatiquement : constructeur, getters, `equals()`, `hashCode()`, `toString()`  
Champs implicitement `private final`  
Constructeur compact pour la validation  
Copie défensive nécessaire pour les objets mutables  
✅ **Nested Classes** :  
`Classe interne` : accès aux membres externes, nécessite une instance  
`Classe statique imbriquée` : pas d'accès aux membres d'instance externes  
`Classe locale` : définie dans un bloc, accède aux variables final/effectively final  
`Classe anonyme` : sans nom, usage unique  
✅ **Polymorphisme** :  
Un objet prend plusieurs formes  
Les méthodes sont appelées selon le type de l'objet (à l'exécution)  
Les variables sont accédées selon le type de la référence (à la compilation)  
Upcasting implicite, downcasting explicite  
Vérifier avec `instanceof` avant de caster  

</details>

<br>


<details>
<summary> Lambda, Functional Interface, Method Reference, Method Chaining</summary>

**`lambda expression`** est une fonction anonyme concise, introduite en Java 8.  
**`functional interface`** possède **exactement une** méthode abstraite (les méthodes Default et static ne comptent pas).  
**`method reference`** est un raccourci pour une lambda qui **appelle simplement une méthode**.  
  
```java
//<<<<<<<  Static: ClassName::staticMethod   >>>>>>> 
Function<String, Integer> parser = s -> Integer.parseInt(s);
// Method reference
Function<String, Integer> parser = Integer::parseInt;
//<<<<<<< Instance (particular): instance::instanceMethod >>>>>>>>>> 
String prefix = "Hello ";
Function<String, String> greeter = s -> prefix.concat(s);
// Method reference
Function<String, String> greeter = prefix::concat;
//<<<<<<< Instance (parameter): ClassName::instanceMethod >>>>>>>>>> 
Comparator<String> comp = (s1, s2) -> s1.compareToIgnoreCase(s2);
// Method reference
Comparator<String> comp = String::compareToIgnoreCase;
//<<<<<<< Constructor: ClassName::new >>>>>>>>>> 
Supplier<List<String>> listSupplier = () -> new ArrayList<>();
// Constructor reference
Supplier<List<String>> listSupplier = ArrayList::new;

```

✅ **Method Chaining**:  
`andThen()`, `compose()` (Function)  
`and()`, `or()`, `negate()` (Predicate)  
`andThen()` (Consumer)  

</details>


<details>
<summary> Built-in/Primitive Functional Interfaces </summary>

Java fournit des interfaces fonctionnelles dans java.util.function.

| Interface | Paramètres | Type de retour | Méthode | Cas d'usage |
|-----------|------------|----------------|---------|-------------|
| `Supplier<T>` | 0 | T | `get()` | Factory, init lazy |
| `Consumer<T>` | 1 (T) | void | `accept(T)` | Effets de bord |
| `BiConsumer<T,U>` | 2 (T, U) | void | `accept(T,U)` | Effets de bord |
| `Predicate<T>` | 1 (T) | boolean | `test(T)` | Filtrage |
| `BiPredicate<T,U>` | 2 (T, U) | boolean | `test(T,U)` | Filtrage |
| `Function<T,R>` | 1 (T) | R | `apply(T)` | Transformation |
| `BiFunction<T,U,R>` | 2 (T, U) | R | `apply(T,U)` | Transformation |
| `UnaryOperator<T>` | 1 (T) | T | `apply(T)` | Transformation même type |
| `BinaryOperator<T>` | 2 (T, T) | T | `apply(T,T)` | Combinaison même type |

**Primitive Functional Interfaces**: Pour éviter l'autoboxing/unboxing, Java fournit des versions primitives.  
**Exemples**:
- `IntSupplier`, `LongSupplier`, `DoubleSupplier`
- `IntConsumer`, `LongConsumer`, `DoubleConsumer`
- `IntPredicate`, `LongPredicate`, `DoublePredicate`
- `IntFunction<R>`, `LongFunction<R>`, `DoubleFunction<R>`
- `ToIntFunction<T>`, `ToLongFunction<T>`, `ToDoubleFunction<T>`
- `IntUnaryOperator`, `LongUnaryOperator`, `DoubleUnaryOperator`
- `IntBinaryOperator`, `LongBinaryOperator`, `DoubleBinaryOperator`

```java
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
</details>

<br>


<details>
<summary> Collection Hierarchy</summary>

```
Iterable
    └── Collection
            ├── List (ordered, allows duplicates)
            │   ├── ArrayList
            │   ├── LinkedList
            │   └── Vector (legacy)
            │
            ├── Set (no duplicates)
            │   ├── HashSet
            │   ├── LinkedHashSet
            │   └── TreeSet (sorted)
            │
            └── Queue (FIFO, priority)
                ├── LinkedList
                ├── PriorityQueue
                └── ArrayDeque

Map (not part of Collection hierarchy)
    ├── HashMap
    ├── LinkedHashMap
    ├── TreeMap (sorted)
    └── Hashtable (legacy)
```

</details>




<details>
<summary> List </summary>

**List** est une collection `ordonnée` qui autorise les `doublons` et l'accès par `index`.  
**Implémentations principales**:  
`ArrayList`: tableau redimensionnable, rapide pour l'accès aléatoire  
`LinkedList`: liste doublement chaînée, rapide pour l'insertion/suppression  
**Méthodes spécifiques à List** (en plus de Collection):

```java
// Accès par index
E get(int index)
E set(int index, E element)
// Insertion/suppression par index
void add(int index, E element)
E remove(int index)
// Recherche
int indexOf(Object o)
int lastIndexOf(Object o)
// Sublist: VUE (liée à l'original)
List<E> subList(int fromIndex, int toIndex)
// Remplacement
void replaceAll(UnaryOperator<E> operator)
// Tri
void sort(Comparator<? super E> c)
```
**ArrayList vs LinkedList**  
| Fonctionnalité | ArrayList | LinkedList |  
|----------------|-----------|------------|  
| Accès par index | O(1) rapide | O(n) lent |  
| Insertion/suppression au début/milieu | O(n) lent | O(1) rapide |  
| Insertion/suppression en fin | O(1) amorti | O(1) |  

</details>


<details>
<summary> Set, Queue, Deque</summary>

**✅Set**: une collection qui **n'autorise pas les doublons**.  
**Implémentations principales**:  
`HashSet`: non ordonné, rapide (O(1) pour add/remove/contains) (autorise 1 null)  
`LinkedHashSet`: maintient l'ordre d'insertion (autorise 1 null)  
`TreeSet`: trié (ordre naturel ou Comparator), plus lent (O(log n)) (pas de null autorisé)  
**Opérations sur les Set**

```java
// Union (tous les éléments)
union.addAll(set2);
// Intersection (éléments communs)
intersection.retainAll(set2);
// Différence (dans set1 mais pas dans set2)
difference.removeAll(set2);
```
**✅Queue**: suit le principe **FIFO** (First In, First Out).  
**Méthodes principales**:
| Opération | Lève une Exception | Retourne une Valeur Spéciale |
|-----------|-------------------|------------------------------|
| Insertion | `add(e)` | `offer(e)` |
| Suppression | `remove()` | `poll()` |
| Examen | `element()` | `peek()` |

**✅Deque** (Double-Ended Queue): file avec **deux extrémités**, peut être utilisée comme:
`Queue` (FIFO) `Stack` (LIFO)  
**Méthodes principales**:
| Opération | Premier Élément | Dernier Élément |
|-----------|----------------|-----------------|
| Insertion | `addFirst(e)` / `offerFirst(e)` | `addLast(e)` / `offerLast(e)` |
| Suppression | `removeFirst()` / `pollFirst()` | `removeLast()` / `pollLast()` |
| Examen | `getFirst()` / `peekFirst()` | `getLast()` / `peekLast()` |

**Opérations Stack**:  
`push(e)` = `addFirst(e)`  
`pop()` = `removeFirst()`  
`peek()` = `peekFirst()`  

</details>


<details>
<summary> Map </summary>

**Map** stocke des paires `clé-valeur`. Les clés sont `uniques`, les valeurs peuvent être dupliquées.  
**Implémentations principales**:  
`HashMap`: non ordonné, rapide (O(1))  
`LinkedHashMap`: maintient l'ordre d'insertion  
`TreeMap`: clés triées (O(log n))  

**✅HashMap vs ConcurrentHashMap**

Une `HashMap` n’est pas thread-safe. Donc si plusieurs threads modifient la même map en même temps, on peut avoir des comportements incohérents.  

`ConcurrentHashMap`, elle, est conçue pour un contexte multithread. Elle permet plusieurs lectures/écritures concurrentes avec une meilleure sécurité.  

Donc on utilise `HashMap` dans un contexte simple, mono-thread.
on utilise `ConcurrentHashMap` quand la map est partagée entre plusieurs threads.  


**Méthodes Map courantes**

```java
// Ajout/mise à jour
V put(K key, V value)
void putAll(Map<? extends K, ? extends V> m)
V putIfAbsent(K key, V value)
// Récupération
V get(Object key)
V getOrDefault(Object key, V defaultValue)
// Suppression
V remove(Object key)
boolean remove(Object key, Object value)
void clear()
// Vérification
boolean containsKey(Object key)
boolean containsValue(Object value)
boolean isEmpty()
int size()
// Vues
Set<K> keySet()
Collection<V> values()
Set<Map.Entry<K, V>> entrySet()
// Opérations Java 8+
void forEach(BiConsumer<? super K, ? super V> action)
V compute(K key, BiFunction<? super K, ? super V, ? extends V> remappingFunction)
V merge(K key, V value, BiFunction<? super V, ? super V, ? extends V> remappingFunction)
```

</details>


<details>
<summary> Sorting Data (Comparable, Comparator)</summary>


**Comparable**: définit l'**ordre naturel** d'une classe.  
Méthode : `int compareTo(T o)`

```java
public class Person implements Comparable<Person> {
    //..
    @Override
    public int compareTo(Person other) {
        return this.age - other.age;  // Tri par âge
    }
}

Collections.sort(personList);  // Utilise compareTo()
```

**Comparator**: définit un **ordre personnalisé** (sans modifier la classe).  
Méthode : `int compare(T o1, T o2)`

```java
// Comparator par nom
Comparator<Person> byName = (p1, p2) -> p1.name.compareTo(p2.name);
Collections.sort(personList, byName);
personList.sort(byName);  // Ou
```

**Méthodes helper Comparator (Java 8+)**  

```java
// Tri par un champ
Comparator<Person> byAge = Comparator.comparing(Person::getAge);
// Ordre inverse
Comparator<Person> byAgeDesc = Comparator.comparing(Person::getAge).reversed();
// Tri multi-champs (âge puis nom)
Comparator<Person> comparator = Comparator
    .comparing(Person::getAge)
    .thenComparing(Person::getName);
// Gérer les nulls
Comparator<String> comparator = Comparator.nullsFirst(String::compareTo);
// Ordre naturel/inverse
numbers.sort(Comparator.naturalOrder());// [1, 2, 5, 8, 9]
numbers.sort(Comparator.reverseOrder());// [9, 8, 5, 2, 1]
```

</details>


<details>
<summary> Generics & Wildcards </summary>

**Generics**: permettent de créer des classes, interfaces et méthodes type-safe (vérifiées à la compilation).


**Unbounded wildcard** (`?`): n'importe quel type.

```java
public static void printList(List<?> list) {
    for (Object item : list) {System.out.println(item);}
}

List<String> strings = Arrays.asList("A", "B");
List<Integer> ints = Arrays.asList(1, 2);
printList(strings);  // OK
printList(ints);     // OK
```

**Upper bounded wildcard** (`? extends Type`): Type ou sous-types.

```java
public static double sum(List<? extends Number> numbers) {
    double sum = 0;
    for (Number num : numbers) {
        sum += num.doubleValue();
    }
    return sum;
}

List<Integer> ints = Arrays.asList(1, 2, 3);
List<Double> doubles = Arrays.asList(1.5, 2.5);
System.out.println(sum(ints));     // 6.0
System.out.println(sum(doubles));  // 4.0
```

**Lower bounded wildcard** (`? super Type`): Type ou super-types.

```java
// Accepte List<Integer>, List<Number>, List<Object>
public static void addIntegers(List<? super Integer> list) {
    list.add(1); list.add(2); list.add(3);
}

List<Integer> ints = new ArrayList<>();
List<Number> numbers = new ArrayList<>();
List<Object> objects = new ArrayList<>();

addIntegers(ints);     // OK
addIntegers(numbers);  // OK
addIntegers(objects);  // OK
```

**Règle PECS**: **Producer Extends, Consumer Super**  
**`Producer`** (lecture): utiliser `? extends T`  
**`Consumer`** (écriture): utiliser `? super T`  

</details>

<br>

<details>
<summary> Optional, Primitive Optional </summary>


**Optional** (Java 8+) conteneur qui peut ou non contenir une valeur, évitant la `NullPointerException`

```java
Optional<String> empty = Optional.empty();
Optional<String> nonEmpty = Optional.of("Hello");
Optional<String> maybeEmpty = Optional.ofNullable(value);  // empty

//------  vérifier si la valeur est présente ------ 
if (opt.isPresent()) {System.out.println(opt.get());}
if (opt.isEmpty()) {System.out.println("Empty");}
// si présente, appeler le consumer
opt.ifPresent(System.out::println); 

//------ Fournir des valeurs par défaut ------ 
// orElse() - valeur par défaut
String s1 = empty.orElse("Default");     // "Default"
// orElseGet() - Supplier pour la valeur par défaut (lazy)
String s3 = empty.orElseGet(() -> "Computed Default");
// orElseThrow() - lève une exception si vide
String s4 = empty.orElseThrow();  // NoSuchElementException
String s5 = empty.orElseThrow(() -> new IllegalStateException("Empty!"))
//------ Transformer l'Optional ------ 
// map(): transforme la valeur si présente
Optional<String> opt = Optional.of("Hello");
Optional<Integer> length = opt.map(String::length);
// flatMap(): transforme en Optional (évite Optional<Optional>)
public class Person { private Optional<Address> address;}

// map() donnerait Optional<Optional<Address>>
Optional<Optional<Address>> nested = person.map(Person::getAddress);
// flatMap() donne Optional<Address>
Optional<Address> address = person.flatMap(Person::getAddress);
/*❌ Optional<Address> address = person.map(p -> p.address.get());*/

// filter(): conserve la valeur si elle satisfait le prédicat
Optional<Integer> opt = Optional.of(42);
Optional<Integer> even = opt.filter(n -> n % 2 == 0);
```
**Primitive Optional**: `OptionalInt`, `OptionalLong`, `OptionalDouble`
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

</details>

<details>
<summary> Streams </summary>

Un **Stream** est une séquence d'éléments supportant des opérations séquentielles et parallèles. Il ne stocke pas les éléments et ne peut être utilisé qu'une seule fois (sinon IllegalStateException)


**Créer un Stream**:
```java
// Depuis une collection
List<String> list = Arrays.asList("A", "B", "C");
Stream<String> stream1 = list.stream();
// Depuis un tableau
String[] array = {"A", "B", "C"};
Stream<String> stream2 = Arrays.stream(array);
Stream<String> stream3 = Stream.of("A", "B", "C");
// Stream vide
Stream<String> empty = Stream.empty();
// Stream infini
Stream<Integer> infinite1 = Stream.iterate(0, n -> n + 2);  // 0, 2, 4, 6...
Stream<Double> infinite2 = Stream.generate(Math::random);
// Depuis une plage (streams primitifs)
IntStream range = IntStream.range(1, 5);      // 1, 2, 3, 4
IntStream rangeClosed = IntStream.rangeClosed(1, 5);  // 1, 2, 3, 4, 5
```
**✅Intermediate Operations** (retournent un Stream):  
`filter()`, `map()`, `mapToObj()`, `flatMap()`,   
`distinct()`, `sorted()`, `limit()`, `skip()`  
`peek()`  
**✅Terminal Operations** (retournent un résultat):  
`forEach()`, `count()`, `collect()`  
`reduce()`, `min()`, `max()`  
`anyMatch()`, `allMatch()`, `noneMatch()`  
`findFirst()`, `findAny()`  
**✅Primitive Streams**:  
`IntStream`, `LongStream`, `DoubleStream`  
Méthodes : `sum()`, `average()`, `min()`, `max()`, `count()`  
`range()`, `rangeClosed()`  
`mapToInt()`, `mapToLong()`, `mapToDouble()`, `boxed()`  
**✅Collectors**:  
`toList()`, `toSet()`, `toMap()`, `toCollection(TreeSet::new)`  
`joining()`, `counting()`, `summingInt()`, `averagingInt()`  
`groupingBy()`, `partitioningBy()`  
`filtering()`, `flatMapping()` (Java 9+)  
`teeing()` (Java 12+)  

</details>

<br>

<details>
<summary> Exceptions </summary>

Une **Exception** est un événement qui perturbe le déroulement normal de l'exécution du programme.

**Hiérarchie des Exceptions**

```java
// Throwable (root)
//   ├── Error (erreurs système, irrécupérables)
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
//
```

Une **suppressed exception** est une exception qui se retrouve masquée (attachée) à une autre exception principale.  
Cela se produit surtout avec try-with-resources.

```java
// pour capturer une suppressed exception
catch (Exception e) {
    for (Throwable t : e.getSuppressed()) {
        System.out.println(t);
    }
}
```

✅ Exceptions  
Les **Checked exceptions** doivent être déclarées (`throws`) ou attrapées  
Les **Unchecked exceptions** (RuntimeException) sont optionnelles  
**Multi-catch** avec `|` pour plusieurs exceptions  
**Finally** s'exécute toujours, même avec un return ou une exception  
**Try-with-resources** ferme automatiquement les ressources AutoCloseable  

</details>


<details>
<summary> Localization, Formatting </summary>

- ✅ Formatting
  - **NumberFormat**: nombres, devises, pourcentages
  - **DateFormat**: dates courtes, moyennes, longues
  - **SimpleDateFormat**: format personnalisé (pas thread-safe)
  - **DateTimeFormatter**: format Java 8+ (thread-safe)
- ✅ Internationalization
  - **Locale**: langue + pays (ex: fr_FR)
  - **ResourceBundle**: charger des messages localisés
  - **Fallback mechanism**: cherche la locale spécifique → générale → défaut
  - **MessageFormat**: paramétrer les messages avec {0}, {1}...

</details>

<br>

<details>
<summary> Threads (Process vs Thread, Thread Lifecycle/types)</summary>



🧠 Process vs Thread  
`Thread`: unité d'exécution à l'intérieur d'un processus, partage la même mémoire, plus léger qu'un processus  
`Process`: programme en cours d'exécution, mémoire isolée, chaque processus a ses propres ressources  


**Types de Thread**  
`User thread` - thread normal  
`Daemon thread` - thread d'arrière-plan (meurt avec le programme)  



✅ Threads  
`Thread`: créer avec Runnable ou extends Thread  
`start()` vs `run()`: start() crée un nouveau thread, run() s'exécute dans le thread courant  
`Callable`: comme Runnable mais retourne une valeur  


**Cycle de vie d'un Thread**

```java
// États du Thread
// NEW → RUNNABLE → RUNNING → TERMINATED
//          ↓           ↓
//       BLOCKED    WAITING/TIMED_WAITING
Thread thread = new Thread(() -> {
    try { Thread.sleep(1000);}
    catch (InterruptedException e) {e.printStackTrace();}
});

System.out.println(thread.getState());  // NEW
thread.start();
System.out.println(thread.getState());  // RUNNABLE
Thread.sleep(500);
System.out.println(thread.getState());  // TIMED_WAITING
Thread.sleep(600);
System.out.println(thread.getState());  // TERMINATED
```

</details>

<details>
<summary> Concurrency API (ExecutorService) </summary>

**ExecutorService**

```java
// ExecutorService = gestionnaire de pool de threads
// Avantages: Réutilisable, Gestion automatique, Plus efficace que new Thread()

//--------  Créer un ExecutorService
// 1. Thread unique
ExecutorService single = Executors.newSingleThreadExecutor();
// 2. Pool de threads fixe
ExecutorService fixed = Executors.newFixedThreadPool(5);  // 5 threads
// 3. Pool de threads en cache (illimité)
ExecutorService cached = Executors.newCachedThreadPool();
// 4. Pool de threads planifié
ScheduledExecutorService scheduled = Executors.newScheduledThreadPool(3);

//--------  Soumettre des tâches
ExecutorService executor = Executors.newSingleThreadExecutor();
// execute() - fire and forget (void)
executor.execute(() -> {System.out.println("Task executed");});
// submit() - retourne un Future
Future<?> future = executor.submit(() -> {
    System.out.println("Task submitted");
});
// submit() avec Callable (retourne un résultat)
Future<Integer> result = executor.submit(() -> 42);

//--------  Récupérer le résultat
try {
    Integer value = result.get(/*2, TimeUnit.SECONDS*/); // Bloquant
    System.out.println("Result: " + value);  // 42
} catch (InterruptedException | ExecutionException e) {
    e.printStackTrace();
}
// IMPORTANT: Arrêt
executor.shutdown();  // S'arrête après les tâches en cours
// executor.shutdownNow();  // S'arrête immédiatement

//-------- invokeAll et invokeAny
ExecutorService executor = Executors.newFixedThreadPool(3);
List<Callable<String>> tasks = Arrays.asList(
    () -> { Thread.sleep(1000); return "Task 1"; },
    () -> { Thread.sleep(500); return "Task 2"; },
    () -> { Thread.sleep(2000); return "Task 3"; }
);
// invokeAll() - exécute tout, attend tout
List<Future<String>> results = executor.invokeAll(tasks);
for (Future<String> future : results) {
    System.out.println(future.get());  // Task 1, Task 2, Task 3
}
// invokeAny() - retourne le premier terminé
String first = executor.invokeAny(tasks);
System.out.println(first);  // "Task 2" (le plus rapide)

```

</details>

<details>
<summary> Thread Safety (Deadlock, Starvation, Livelock, Race condition)</summary>

**✅ Problèmes de Thread Safety**  
`🔒 Deadlock` se produit quand deux threads se bloquent mutuellement car chacun attend une ressource détenue par l'autre.  
👉 Résultat : personne n'avance.

`🍽️ Starvation` se produit quand un thread n'obtient jamais les ressources dont il a besoin, car d'autres threads sont toujours prioritaires.  
👉 Il n'est pas techniquement bloqué, mais il ne progresse jamais.  

`🔄 Livelock`  
Le Livelock se produit quand les threads sont actifs mais ne progressent pas.  
👉 Dans un deadlock, les threads sont bloqués. Dans un livelock, ils continuent de s'exécuter mais n'avancent pas.  

`⚠️ Data Race`: arrive quand plusieurs threads accèdent à une même donnée en même temps, et au moins un thread la modifie, sans synchronisation.

`⚠️ Race condition`: quand le résultat dépend de l'ordre d'exécution des threads.  
Si les accès ne sont pas synchronisés, on peut obtenir des résultats incorrects ou imprévisibles.  

**✅ Thread Safety**  
`synchronized`: Verrou sur (méthode, bloc ou this)  
`Lock`: Plus flexible que synchronized (Lock lock = new ReentrantLock();)  
  ``` java
      public void increment() {
          lock.lock();
          try { count++;}
          finally {lock.unlock();}
      }

      // tryLock() - non bloquant
      public boolean tryIncrement() {
          if (lock.tryLock()) {
              try { count++; return true;}
              finally { lock.unlock();}
          }
          return false;  // Verrou non disponible
      }
  ```
`Atomic classes`: Thread-safe sans verrous (AtomicInteger, etc.)  
`volatile`: Garantit la visibilité entre threads (pas l'atomicité ; les lectures/écritures vont directement en mémoire ; pas de cache)  


**✅ Collections Concurrentes**  
`ConcurrentHashMap`: HashMap thread-safe  
`CopyOnWriteArrayList`: Thread-safe pour les charges à dominante lecture  
`BlockingQueue`: File avec blocage (put/take)  

</details>

<details>
<summary> CompletableFuture </summary>

> `CompletableFuture`, c’est une classe Java utilisée pour gérer des traitements asynchrones.  
> Par exemple, on peut lancer un appel API, un calcul, ou une requête longue sans bloquer le thread principal.  
> Ensuite, quand le résultat est disponible, on peut enchaîner un traitement avec `thenApply`, `thenAccept`, ou gérer une erreur avec `exceptionally`.



```java
CompletableFuture<String> future = CompletableFuture.supplyAsync(() -> {
    return "Hello";
});

future.thenApply(result -> result + " Anouar")
      .thenAccept(System.out::println);
```

```java
CompletableFuture<User> userFuture =
    CompletableFuture.supplyAsync(() -> userService.getUserById(1));

CompletableFuture<List<Order>> ordersFuture =
    CompletableFuture.supplyAsync(() -> orderService.getOrdersByUserId(1));

CompletableFuture<Void> result =
    userFuture.thenCombine(ordersFuture, (user, orders) -> {
        user.setOrders(orders);
        return user;
    }).thenAccept(System.out::println);
```

</details>

<br>


<details>
<summary> codeQuality, Tests, TDD, Solid</summary>

<details>

<summary> assurer la qualité du code ?</summary>


> Pour assurer la qualité du code, on s’appuie sur plusieurs choses.
> D’abord, il y a les code reviews : chaque pull request est relue, souvent par un tech lead, pour vérifier la logique, les bonnes pratiques et éviter les regression.

> Ensuite, on a des tests automatisés et des outils comme Sonar pour détecter les problèmes de qualité ou la couverture.
> Et enfin, on fait attention à garder un code lisible et simple, pour qu’il soit facile à maintenir.
</details>

<details>

<summary> Tests ?</summary>

> On teste le code à plusieurs niveaux.
> D’abord avec des tests unitaires en JUnit pour vérifier la logique métier.
> Ensuite, avec des tests d’intégration pour s’assurer que les composants fonctionnent bien ensemble.

> On utilise aussi Cucumber pour les tests métier : les BA décrivent les scénarios fonctionnels, et côté développement on les automatise.
> Ces tests permettent de valider les flux métier de bout en bout, jusqu’à l’envoi vers le marché.
</details>


<details>

<summary> TDD ?</summary>

> Honnêtement, on n’appliquait pas le TDD de manière stricte.
> En pratique, on développait d’abord, puis on écrivait les tests, principalement à cause des contraintes de planning.
> En revanche, les tests restaient indispensables, surtout sur les parties critiques.
</details>

<details>

<summary> principes SOLID ?</summary>

> SOLID ce sont des principes de conception pour réduire le couplage et améliorer la maintenabilité.

`S – Single Responsibility Principle (SRP)`<br>
  Une classe ne doit avoir qu’une seule responsabilité<br>
`O – Open / Closed Principle (OCP)`<br>
  Le code doit être ouvert à l’extension mais fermé à la modif, pour ajouter des fonctionnalités sans casser l’existant.<br>
`L – Liskov Substitution Principle (LSP)`<br>
  Une classe fille doit pouvoir remplacer sa classe mère sans modifier le comportement attendu.<br>
`I – Interface Segregation Principle (ISP)`<br>
  Il vaut mieux plusieurs interfaces petites et spécifiques qu’une seule interface trop large.<br>
`D – Dependency Inversion Principle (DIP)`<br>
  Les classes doivent dépendre d’abstractions (interfaces) et non de classes concrètes.<br>

</details>

</details>

<details>

<summary> ❓ Performance ❓</summary>

<details>

<summary> ❔ Comment améliorer la performance d’une API lente ? </summary>

> Je commence par mesurer pour identifier la cause.
> Ça peut venir de la base de données, d’un appel externe, d’un traitement trop lourd, d’un problème de pagination ou d’un mauvais algorithme.
> Ensuite je corrige selon la cause : index, optimisation de requête, cache, pagination, traitement asynchrone ou refactoring.

</details>


<details>

<summary> ❔ Comment détecter un problème de performance ? </summary>

> Je commence par observer les symptômes : temps de réponse lents, consommation CPU élevée, mémoire qui monte, ou erreurs en production.
> Ensuite, je regarde les métriques, les logs et le monitoring pour identifier où le temps est passé : base de données, appel externe, code applicatif ou file de messages.
> Si besoin, je fais du profiling pour aller plus loin et identifier les méthodes lentes, les blocages de threads ou les problèmes mémoire.

</details>

<details>

<summary> ❔ Quand utiliser un cache ? </summary>

> Le cache est utile pour des données souvent lues et rarement modifiées.
> Il permet d’éviter de recalculer ou de rappeler la base à chaque fois.
> Mais il faut gérer l’invalidation, sinon on risque d’avoir des données obsolètes.

</details>


<details>

<summary> ❔ Low Latency?</summary>

> Low latency, ça veut dire minimiser le temps de réponse du système. En Java, ça passe par l’optimisation du code, l’utilisation du multithreading, éviter les accès lents comme la base de données, et privilégier la mémoire ou des structures performantes.

> Techniquement en Java, pour faire du low latency, on fais attention à plusieurs points.
> D’abord, on évite de créer trop d’objets pour limiter le garbage collector.
> Ensuite, on privilégie des structures rapides comme les tableaux ou les HashMap.
> on essaie aussi d’éviter les accès lents comme la base de données en utilisant du cache en mémoire.
> Et si besoin, on utilise le multithreading pour paralléliser le traitement et gagner du temps.”

</details>


<details>

<summary> ❔ C’est quoi le profiling ? </summary>

> Le profiling, c’est le fait de mesurer l’exécution d’une application pour identifier ce qui consomme le plus de ressources.
> En Java, on peut regarder le CPU, la mémoire, les threads, les blocages, les deadlocks et les temps d’exécution des méthodes.
> L’objectif est de ne pas optimiser au hasard, mais de trouver précisément la cause du problème avant de corriger.

</details>



</details>

<details>
<summary>  ❓ QUESTIONS JAVA ❓</summary>

<details>

<summary>moderne java 17/21 pour la finance?</summary>

> Java 17 et 21 apportent surtout des gains en performance et en stabilité, ce qui est très important en finance.
>
> On a aussi des fonctionnalités comme les records ou les switch améliorés qui rendent le code plus clair et plus fiable.
>
> Du coup, ça permet de réduire les bugs et d’avoir des applications plus robustes, ce qui est essentiel quand on traite des données financières.

</details>

<details>

<summary> ❔ les nouveautés de Java 17 ?</summary>

> Java 17 est une version LTS, avec plusieurs nouveautés importantes, surtout autour de la simplicité du code, de la sécurité et de la performance<br>
Les nouveautés importantes sont par exemple :

* les `records`, pour créer des classes immuables plus simplement ;
* les `sealed classes`, pour mieux contrôler l’héritage ;
* les améliorations du `switch` ;
* les text blocks, pratiques pour les chaînes multilignes ;
* et aussi les améliorations de performance et de sécurité de la JVM.

</details>

<details>

<summary> ❔ les nouveautés aprés Java 21 ?</summary>

Java 21 est aussi une version LTS.  
Les nouveautés importantes sont surtout:  
- les virtual threads, qui permettent de gérer beaucoup de traitements concurrents avec des threads beaucoup plus légers
- le pattern matching, les améliorations du switch.   
- aussi des améliorations de performance au niveau de la JVM, du Garbage Collector. 

</details>


<details>

<summary> ❔ les nouveautés Java 25 ?</summary>

Java 25, c’est une version LTS, donc une version stable pour la prod.  
Je ne l’ai pas encore utilisée directement, mais je sais qu’elle continue dans la logique de Java 21, surtout sur la concurrence et la performance.   
Il y a aussi des petites améliorations pour simplifier le code.  

</details>

<details>

<summary>❔ C’est quoi les <em><b>Virtual Threads</b></em> ?</summary>

> Les Virtual Threads sont des threads légers.
>
> Ils permettent de gérer beaucoup de tâches en parallèle avec moins de coût qu’un thread classique.
>
> C’est surtout utile pour les traitements avec beaucoup d’appels I/O, comme HTTP, base de données ou fichiers.

</details>


<details>

<summary>❔ Comment tu gères la concurrence en Java ?</summary>

> Ça dépend du besoin.
>
> Pour des traitements simples, on peut utiliser des Executors ou des CompletableFuture.
>
> Pour des accès partagés, il faut faire attention à la thread-safety, utiliser des structures adaptées comme ConcurrentHashMap, ou synchroniser certaines sections critiques.
>
> Et dans un contexte Spring, j’essaie aussi d’éviter les états partagés dans les services pour limiter les problèmes de concurrence.

</details>


</details>

<br><br>

</details>

<details>

<summary> ✅ Spring </summary>

<details>


<summary> DI , IOC , Annotations, Tight/Loose coupling  </summary>

**✅Dependency Injection**: C'est un patron de conception où les dépendances sont passées à une classe plutôt que créées à l'intérieur. Il réduit le couplage fort et facilite grandement les tests.  
**✅Inversion of Control (IoC)**: L'IoC signifie que Spring gère la création et le cycle de vie des objets (beans) au lieu que le développeur les instancie manuellement avec new.  
**✅Bean (Spring Bean)**: objet géré par le conteneur Spring. Spring gère sa création, son injection et son cycle de vie.  
**✅Tight Coupling vs Loose Coupling**  
`Tight coupling` c'est quand une classe dépend directement d'une implémentation concrète. si l'implémentation change, il faut modifier le code.  
`Loose coupling` c'est quand une classe dépend d'`interfaces` plutôt que d'implémentations. Cela rend le code `plus flexible`, `plus facile à tester`.   
**✅Annotations principales**    
`@Configuration`: Marque une classe comme configuration Spring, Contient des méthodes @Bean pour définir des beans  
`@Bean`: Déclare qu'une méthode produit un bean géré par Spring, Utilisé dans les classes @Configuration  
`@Component`: Marque une classe comme composant Spring, Détecté automatiquement par @ComponentScan  
`@ComponentScan`: Active le scan automatique des composants, Cherche les @Component..., enregistre les beans trouvés  
`@Autowired`: Injection automatique de dépendance par Spring, Utilisable sur les constructeurs, setters, champs  
`@Primary`: Donne la priorité à un bean quand plusieurs beans du même type existent pour résoudre les ambiguïtés (NoUniqueBeanDefinitionException)  
`@Qualifier`: Spécifie quel bean injecter quand plusieurs beans du même type existent, Plus précis que @Primary  
`@Lazy`: Bean créé seulement à la première utilisation, Par défaut : beans créés au démarrage (eager)  
`@Scope`: SINGLETON (instance par conteneur IoC), PROTOTYPE, REQUEST, SESSION, APPLICATION  
`@PostConstruct`: Méthode exécutée après l'injection de dépendance  
`@PreDestroy`: Méthode exécutée avant la destruction du bean (utilisée pour le nettoyage des ressources)  
**🎯quand utiliser @Component vs @Bean**  
`@Component`: vous contrôlez votre propre classe (services, Repositories, Controllers)  
`@Bean`: Classes tierces, Configuration complexe nécessaire (DataSource, RestTemplate, ObjectMapper)  

</details>


<details>


<summary> (Spring vs Boot vs MVC) & Spring Boot Starters & Auto-configuration </summary>

**✅Spring vs Spring Boot vs Spring MVC**  
`Spring` est le framework de base qui fournit IoC, DI et l'écosystème.  
`Spring MVC` est un module Spring dédié à la construction d'APIs web et d'applications REST.  
`Spring Boot` simplifie Spring : il fournit l'auto-configuration, un serveur embarqué et permet de démarrer une application rapidement avec un minimum de configuration.  
**✅Spring Boot Starters**     
Les Starters sont des dépendances "tout-en-un" qui incluent automatiquement toutes les bibliothèques nécessaires pour un cas d'usage spécifique.
`Avantages`: `Compatibilité garantie entre les dépendances`, `Pas de conflits`, `Moins d'erreurs de configuration`  
**spring-boot-starter-xxxxx**  
`web`: inclut automatiquement : Spring MVC (pour les APIs REST et le web), Tomcat embarqué (serveur), Jackson (sérialisation JSON), Validation, logging  
`data-jpa`: Spring Data JPA, Hibernate (ORM), JDBC)  
`security`: Spring Security Core, Authentication, Authorization, Gestion des sessions  
`test`: JUnit 5, Mockito, AssertJ  
`actuator`: Vérifications de santé, Métriques, Infos application, Propriétés d'environnement  
**✅Auto-configuration**  
permet à Spring Boot de configurer automatiquement l'application en fonction des dépendances présentes dans le classpath.  
`exemple`: si SpringBoot voit une dépendance JPA, il configure automatiquement la datasource & Hibernate.  

</details>

<br>

<details>

<summary> Spring JDBC, &nbsp;JPA, &nbsp;Hibernate & Auditing</summary>

<details>

<summary> 🔹Spring JDBC </summary>

simplifie l'accès à la base de données en éliminant le code JDBC traditionnel répétitif (gestion des connexions, ResultSet, exceptions).  
Avec `JdbcTemplate` (Gestion automatique des connexions/exceptions, Moins de code répétitif, Pas de fermeture manuelle des ressources)  

</details>


<details>

<summary > 🔹Spring JPA </summary>

JPA est une spécification Java pour l'ORM. Elle définit comment mapper des objets Java à des tables de base de données.  
`EntityManager` c’est l’interface principale de JPA pour interagir avec BDD (persist, merge, find, createQuery, remove)  

</details>


<details>

<summary> 🔹Spring Data JPA </summary>

Spring Data JPA ajoute une couche d'abstraction sur JPA, éliminant même le besoin d'écrire des requêtes pour les opérations CRUD de base.  
`public interface CourseRepository extends JpaRepository<Course, Long> {}`
`save(entity)`, `saveAll(entities)`, `findById(id)`, `findAll()`, `findAllById(ids)`, `count()`, `existsById(id)`, `deleteById(id)`, `delete(entity)`, `deleteAll()`, `findAll(Pageable pageable)`, `findAll(Sort sort)`  
✅ Query Methods - Spring génère automatiquement la requête depuis le nom de la méthode, exemple ( List<Course> findByName(String name)  
✅ JPQL - permet d’écrire des requêtes en utilisant les noms des entités au lieu des noms des tables.  
✅ @Query - Requêtes personnalisées
```java
@Query("SELECT c FROM Course c WHERE c.author = :author")
List<Course> findCoursesByAuthor(@Param("author") String author);
```

</details>


<details>

<summary> 🔹specification </summary>

Les Specifications(Requêtes Dynamiques) permettent de construire des requêtes dynamiques en combinant des critères en implementant(JpaSpecificationExecutor).  

</details>


<details>

<summary> 🔹Hibernate </summary>

Hibernate est une implémentation de JPA qui permet de faire le mapping objet-relationnel en Java.  
(+) il ajoute des fonctionnalités concrètes par rapport à JPA, comme le cache, la gestion automatique du SQL, et des optimisations de performance.

</details>


<details>

<summary> 🔹Audit </summary>

L'Auditing permet de tracer automatiquement qui a créé/modifié une entité et quand, sans code manuel dans chaque service.  
`@EnableJpaAuditing` dans la config et `@EntityListeners(AuditingEntityListener.class)`
colonnes: `@CreatedDate`, ` @CreatedBy`, `@LastModifiedDate`, `@LastModifiedBy`

</details>


<details>

<summary> 🔹EAGER vs LAZY </summary>

EAGER charge les relations immédiatement avec l'entité, alors que LAZY les charge uniquement quand on y accède. En pratique, on préfère LAZY pour éviter des problèmes de performance.  
`LAZY` (défaut pour @OneToMany, @ManyToMany) : Préféré, charge à la demande  
`EAGER` (défaut pour @ManyToOne, @OneToOne) : Éviter, peut causer N+1 

</details>

</details>


<details>

<summary> Hibernate (Advanced)</summary>

<details>

<summary> 🔹@Transactional </summary>

Avec Spring, on utilise @Transactional pour gérer les transactions.
On le met généralement sur les méthodes de service qui effectuent plusieurs opérations en base.  


Exemple : si une méthode fait plusieurs opérations en base, et qu’une erreur arrive au milieu, Spring fait un rollback.  

Par défaut, Spring fait rollback sur les exceptions non checked, comme `RuntimeException`.  

@Transactional peut ne pas être appliqué si la méthode est appelée depuis la même classe, parce que l’appel ne passe pas par le proxy Spring.  
Qi la méthode est private, final, si la classe n’est pas un bean Spring, ou si l’exception est catchée sans être relancée.


</details>

<details>

<summary> 🔹EntityManager </summary>

✅ `EntityManager` est l’API JPA principale pour interagir avec la base de données. Il gère le cycle de vie des entités (persist, merge, find, remove) et le context de persistance.  

</details>

<details>

<summary> 🔹first-level cache, second level cache </summary>

✅ Le `first-level cache`, c’est le cache lié à la session Hibernate, activé par défaut. Si on récupère la même entité deux fois dans la même transaction, JPA retourne la même instance depuis le cache sans requête SQL  

✅ Le `second level cache` c’est un cache partagé entre toutes les sessions. Il n’est pas activé par défaut, il faut le configurer. On peut utiliser des providers comme EhCache ou Caffeine  

</details>

<details>

<summary> 🔹dirty checking </summary>

✅ Le `dirty checking` dans un @Transactional, c'est quand Hibernate détecte automatiquement les modifications d’une entité et le  met à jour la base  

```Java
@Transactional
public void updateCourse(Long id) {
    Course course = entityManager.find(Course.class, id);
    course.setName("Updated");
    // Pas besoin de save() !
    // JPA détecte le changement et fait UPDATE automatiquement
```
</details>

<details>

<summary> 🔹CascadeType </summary>

✅ `CascadeType` propage les opérations de l’entité parente vers les enfants.(ALL, PERSIST, MERGE, REMOVE, REFRESH, DETACH)

</details>

<details>

<summary> 🔹orphanRemoval </summary>

✅ `orphanRemoval` permet de supprimer automatiquement une entité enfant si elle n’est plus liée à son parent

</details>

<details>

<summary> 🔹projection DTO </summary>

✅ La `projection DTO` permet de récupérer seulement les données nécessaires depuis la base au lieu de charger toute l’entité

</details>


<details>

<summary> 🔹LazyInitializationException </summary>

C’est une exception qui se produit quand on essaie d’accéder à une collection LAZY en dehors de la session Hibernate(après la fermeture de la transaction). Pour la résoudre :  
🔹 Utiliser JOIN FETCH ou @EntityGraph pour charger les données dans la requête  
🔹 Utiliser le pattern Open Session in View (mais c’est controversé car ça étend trop la session)  
🔹 Retourner des DTOs plutôt que des entités vers la couche de présentation — c’est l’approche   recommandée

</details>

<details>

<summary> 🔹Probleme N </summary>

arrive quand on fait une requête principale pour récupérer une liste, puis une requête supplémentaire pour chaque élément à cause du lazy loading. Ça provoque beaucoup de requêtes SQL et dégrade les performances. On le corrige généralement avec des JOIN FETCH ou des projections DTO. solution:  
🔹 Utiliser JOIN FETCH dans une requête JPQL  
🔹 Utiliser @EntityGraph pour préciser les relations à charger  
🔹 Utiliser @BatchSize pour regrouper les chargements  
🔹 Ou passer à une requête native/DTO directement  

</details>

<details>

<summary> 🔹Propagation de transaction </summary>

```Java
    // ── Propagation ─────────────────────────────────────────
    @Transactional(propagation = Propagation.REQUIRES_NEW)
    public void auditLog(String action) {
        // Toujours dans une NOUVELLE transaction
        // Même si appelé depuis une méthode déjà transactionnelle
        // Utile pour les logs : ils ne sont pas rollbackés si la transaction principale échoue
    }
```

***Propagation — Les cas courants***

```
REQUIRED      → Utilise la transaction existante, ou en crée une (défaut)
REQUIRES_NEW  → Crée toujours une nouvelle transaction (suspend l'existante)
SUPPORTS      → Utilise la transaction si elle existe, sinon sans transaction
NOT_SUPPORTED → Suspend la transaction existante, s'exécute sans transaction
MANDATORY     → Doit être dans une transaction, sinon exception
NEVER         → Ne doit PAS être dans une transaction, sinon exception
NESTED        → Transaction imbriquée avec savepoint
```
✅  ***Quelles sont les fonctionnalités uniques d’Hibernate (non JPA) ?***
🔹Second-Level Cache  
🔹Batch Processing optimisé  
🔹Annotations spécifiques (@Formula, @CreationTimestamp, @UpdateTimestamp)  
🔹Custom ID generators  

</details>

<details>

<summary> 🔹Les annotations de base </summary>

🔹`@Entity`: indique que la classe est une entité mappée à une table   
🔹`@Table`: pour préciser le nom de la table si c’est différent du nom de la classe   
🔹`@Id`: pour marquer la clé primaire   
🔹`@GeneratedValue`: pour dire comment l’ID est généré (auto, séquence, identity…)   
🔹`@Column`: pour configurer une colonne (nom, longueur, nullable…)   
🔹`@Transient`: pour dire qu’un champ ne doit PAS être persisté en base   

</details>

<details>

<summary> 🔹Le cycle de vie d’une entité </summary>

🔹 `Transient`: l’objet vient d’être créé avec new, Hibernate n’en sait rien, rien en base  
🔹 `Persistent`(ou Managed) : l’objet est dans la session Hibernate, il est tracké, toute   modification sera synchronisée en base
🔹 `Detached`: l’objet était persistent mais la session a été fermée. Il n’est plus tracké  
🔹 `Removed`: on a appelé remove(), il sera supprimé en base à la prochaine synchro  

</details>

<details>

<summary> 🔹persist vs merge vs save </summary>

`persist` est une méthode JPA utilisée pour rendre une nouvelle entité persistante.  
Elle est plutôt utilisée quand l’objet est nouveau.  

`merge` sert à rattacher une entité détachée au contexte de persistance.  
Il retourne une nouvelle instance managée.  

`save`, c’est une méthode Spring Data JPA. Elle peut faire un insert ou un update selon l’état de l’entité.  

En pratique, avec Spring Data JPA, on utilise souvent `save`.  
Mais il faut comprendre derrière que pour une nouvelle entité on est proche d’un `persist`, et pour une entité détachée on est proche d’un `merge`.

</details>

<details>

<summary>🔒Comment éviter qu'un utilisateur écrase les modifications faites par un autre utilisateur ?</summary>

Il y a principalement deux stratégies : le locking optimiste et le locking pessimiste.

Dans une application classique, je privilégierais généralement le locking optimiste.

Avec Hibernate, on peut utiliser `@Version`.

À chaque modification, la version est incrémentée. Si quelqu'un essaie de sauvegarder une ancienne version de la donnée, Hibernate détecte le conflit et lève une exception.

</details>

<details>

<summary>🔒Qu'est-ce que @Version dans Hibernate ?</summary>

`@Version` permet d'implémenter l'optimistic locking.

Par exemple, mon entité possède un champ `version`.

Quand je lis l'objet, la version vaut 3. Si quelqu'un le modifie avant moi, elle passe à 4.

Quand j'essaie ensuite de sauvegarder ma version 3, Hibernate voit que la version en base est maintenant 4 et refuse la mise à jour.

Ça évite d'écraser silencieusement les modifications d'un autre utilisateur.

</details>


</details>

<details>

<summary> Spring MongoDB</summary>

`MongoDB` est BDD NoSQL orientée documents. Au lieu de stocker les données dans des tables avec des lignes et des colonnes, MongoDB stocke les données sous forme de documents JSON.


| SQL          | MongoDB                        |
|--------------|--------------------------------|
| Table        | Collection                     |
| Row          | Document                       |
| Column       | Field                          |
| Primary Key  | `_id`                          |
| JOIN         | Embedded document / `$lookup`  |
| Fixed schema | Flexible schema                |


`@Document`: Indique que cette classe Java est mappée à une collection MongoDB.  
`@Id`: Marque le champ qui correspond au _id MongoDB.  
`@Field`: Permet de définir le nom du champ dans MongoDB (si différent du nom Java).  
`@Indexed`: Crée un index sur ce champ pour accélérer les recherches.  
`@DBRef`: Crée une référence vers un autre document (comme une foreign key).  

`MongoTemplate` est une classe Spring qui donne un accès plus bas niveau à MongoDB.  
Les `indexes` accélèrent les requêtes en évitant de parcourir toute la collection.  

> `Aggregation` C’est une suite d’étapes qui transforment les documents ($match pour filtrer, $group pour regrouper, $sort pour trier, $project pour sélectionner les champs)  
> C’est l’équivalent des GROUP BY et des fonctions d’agrégation en SQL. Avec Spring, on utilise MongoTemplate.aggregate()  


> **`Embedded Documents vs References`**  
> `Embedded Documents`: On stocke un objet directement à l’intérieur du document parent.  
> ✅ Une seule requête pour tout lire (pas de JOIN)  
> ✅ Performance excellente en lecture  
> ❌ Duplication de données si l’objet est partagé par plusieurs documents  
> ❌ Document peut devenir très lourd  
> `References avec @DBRef`  
> ✅ Pas de duplication & Mise à jour centralisée  
> ❌ Spring fait des requêtes séparées pour charger les références (N+1 problem possible)  
> ❌ Moins performant que l’embedded  


</details>

<br>

<details>

<summary> Controllers(ResponseEntity, httpCodes, preAuth, CORS, ContentNegotiation)</summary>

**✅ResponseEntity**: wrapper qui permet de contrôler entièrement la réponse HTTP : code de statut, headers et corps.  
EX: `ResponseEntity.ok()`, `ResponseEntity.created(location).body(created)`, `ResponseEntity.noContent().build()`  
**✅Codes de statut HTTP**  
`2xx Succès`: (`200 OK` - Requête réussie), (`201 CREATED` - Ressource créée), (`202 ACCEPTED` - Requête acceptée), (`204 NO CONTENT` - Succès sans contenu à retourner)  
`3xx Redirection`  
`4xx Erreurs client`: (`400 BAD REQUEST` - Requête invalide), (`401 UNAUTHORIZED` - Non authentifié), (`403 FORBIDDEN` - Authentifié mais non autorisé), (`404 NOT FOUND` - Ressource non trouvée), (`409 CONFLICT` - Conflit), (`429 TOO MANY REQUESTS` - Rate limiting)  
`5xx Erreur serveur` (`500 INTERNAL SERVER ERROR`), (`503 SERVICE UNAVAILABLE`)  
**✅preAuth**  
`@PreAuthorize (("hasAuthority('USER_WRITE')"), ("hasRole('ADMIN')"))` est une annotation Spring Security pour contrôler l'accès à une méthode AVANT son exécution   
`@PostAuthorize` (Contrôle après exécution)  
`@Secured` @Secured("ROLE_ADMIN") (Alternative simple)  
**✅cors**: `Cross-Origin Resource Sharing` - Un mécanisme de sécurité du navigateur qui contrôle les appels HTTP vers des origines différentes (domaine, port, protocole).  
`CORS par Controller/Endpoint` (@CrossOrigin(origins = "http://localhost:3000")) ou en `configuration globale` (.allowedOrigins("http://localhost:3000", "https://myapp.com")))  
**✅Content Negotiation**: permet au serveur de retourner différents formats (JSON, XML, CSV) selon ce que le client demande via le header Accept

</details>


<details>


<summary> Validation</summary>
permet de vérifier que les données reçues dans une application (via une API REST ou des formulaires) respectent certaines contraintes avant de les traiter.
`EX`: @NotNull, @NotEmpty, @NotBlank, @Email, @Pattern, @regex, @Past, @Size, @Min

| @Valid | @Validated |
|--------|------------|
| Java standard (JSR-380) | Spécifique à Spring |
| Pas de support des groupes | Supporte les **groupes de validation** |
| Utilisé sur les paramètres de méthode | Peut être utilisé au niveau de la classe |
| Validation standard | Validation avancée avec groupes |
| MethodArgumentNotValidException | ConstraintViolationException |


`Sans handler personnalisé`: Spring retourne automatiquement `400 Bad Request` avec les détails de l'erreur  
**✅Group Validation**: Permet d'appliquer différentes validations selon le contexte
```java
public class User {
  @NotBlank(groups= {CreateValidation.class, UpdateValidation.class})
  private String name;
}
// Controller
@PostMapping
public ResponseEntity<User> create(
  @Validated(CreateValidation.class) @RequestBody User user) {}
```

</details>

<details>

<summary>Exception Handling</summary>

`@ResponseStatus`: Associe un code de statut HTTP à une exception spécifique  
`@ControllerAdvice`: Crée un gestionnaire d'exceptions global pour toute l'application  
`@ExceptionHandler`: Gère des exceptions spécifiques levées par les méthodes du controller  

🔹1. Créer des exceptions personnalisées :
```java
@ResponseStatus(code = HttpStatus.NOT_FOUND)
public class ResourceNotFoundException extends RuntimeException {/**/}
```
🔹2. Créer une classe ErrorDetails (DTO pour les erreurs) :
```java
public class ErrorDetails {
    private Date timestamp;
    private String message, details;
```
🔹3. Créer un gestionnaire d'exceptions global avec @ControllerAdvice :
```java
@ControllerAdvice
public class GlobalExceptionHandler {
  @ExceptionHandler(ResourceNotFoundException.class)
  public ResponseEntity<ErrorDetails> handleResourceNotFoundException(
      ResourceNotFoundException exception,
      WebRequest webRequest) {
    ErrorDetails errorDetails = new ErrorDetails(
      new Date(), exception.getMessage(), webRequest.getDescription(false));
    return new ResponseEntity<>(errorDetails, HttpStatus.NOT_FOUND);
  }
```
</details>

<details>


<summary> Actuators</summary>

SpringBoot Actuator fournit des production-ready features pour surveiller et gérer une application SpringBoot. Il expose des endpoints HTTP qui fournissent des informations sur l'état et les métriques de l'application.

`/actuator` : Liste tous les endpoints disponibles  
`/actuator/health` : État de santé de l'application (UP/DOWN)  
`/actuator/info` : Métadonnées de l'application (version, description)  
`/actuator/metrics` : Métriques de performance (JVM, requêtes HTTP, base de données)  
`/actuator/env` : Propriétés d'environnement et configuration  
`/actuator/beans` : Liste complète des beans Spring  
`/actuator/mappings` : Tous les Request Mappings (endpoints REST/MVC)  
`/actuator/loggers` : Gestion des niveaux de log  
`/actuator/shutdown` : Permet d'arrêter l'application (doit être activé)  
</details>

<details>

<br>

<summary> AOP & Cross-Cutting Concerns</summary>

C'est une approche qui permet de séparer les cross-cutting concerns du code métier, comme le logging, la sécurité ou la gestion des transactions.  
Par exemple, pour le logging ou les transactions, on peut utiliser un aspect qui s'exécute avant ou après une méthode, sans polluer le code métier.  
`Cross-Cutting Concerns`: Fonctionnalités qui s'appliquent à plusieurs couches de l'application  
**✅1. Aspect**: Classe qui contient le code des préoccupations transversales, `Aspect = Advice + Pointcut`  
**✅2. Advice**: Le code à exécuter, Ex : logique de logging, logique d'authentification  
`Types`: @Before, @After, @AfterReturning, @AfterThrowing, @Around  
**✅3. Pointcut**: Expression qui identifie les méthodes à intercepter (Définit quand l'advice doit être exécuté)  
`Ex`: ` @Pointcut("execution(* com.example.service.*.*(..))")`  
**✅4. Join Point**: Point d'exécution dans l'application (appel de méthode, exception, etc.) (Là où l'aspect peut être appliqué)  
**✅5. Weaving**: Processus de liaison des aspects au code de l'application. Peut être effectué à la compilation, au chargement ou à l'exécution  

✅**Annotations principales**  
`@Before` S'exécute avant l'appel de méthode  
`@After` S'exécute après la méthode (succès OU exception)  
`@AfterReturning` S'exécute seulement si la méthode réussit  
`@AfterThrowing` S'exécute seulement si la méthode lève une exception  
`@Around` S'exécute avant ET après la méthode (plus puissant)  
`@Pointcut` Définit une expression de pointcut réutilisable  
✅**JoinPoint vs ProceedingJoinPoint**  
`JoinPoint`: Utilisé avec @Before, @After, @AfterReturning, @AfterThrowing 🔹 Fournit des infos sur la méthode interceptée 🔹 Ne peut pas contrôler l'exécution de la méthode  
`ProceedingJoinPoint`: Utilisé uniquement avec @Around 🔹 Étend JoinPoint 🔹 Permet de contrôler l'exécution avec proceed()  

</details>


<details>


<summary> Spring Security</summary>

Avec **`Spring Security`**, les endpoints sont protégés en définissant des **règles d'accès** basées sur l'authentification et les rôles.  

**Spring Security** fonctionne avec une chaîne de Security Filters qui interceptent chaque requête HTTP. Les composants principaux sont :  
`SecurityFilterChain`: spécifier quelles URLs sont publiques et lesquelles nécessitent une authentification ou un rôle spécifique.
`AuthenticationManager`: Gère le processus d'authentification
`UserDetailsService`: Charge les informations utilisateur depuis la source de données
`PasswordEncoder`: Encode/vérifie les mots de passe

1️⃣ Dépendance Maven `spring-boot-starter-security`  
2️⃣ Configuration Spring Security (basique)
```java
@Configuration @EnableWebSecurity public class SecurityConfig {
  @Bean
  public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
      .csrf(csrf -> csrf.disable()) // souvent désactivé pour les APIs REST
      .authorizeHttpRequests(auth -> auth
          .requestMatchers("/public/**").permitAll()
          .requestMatchers("/admin/**").hasRole("ADMIN")
          .anyRequest().authenticated()
      )
      .httpBasic(); // ou JWT / OAuth2 selon le cas
    return http.build();
    }
}
```
3️⃣ Sécurité basée sur les annotations (granularité fine)
```java
@PreAuthorize("hasRole('ADMIN')")
@GetMapping("/admin/secure")
public String secureAdminEndpoint() {/*...*/}
```

</details>

<details>

<summary> Sécuriser API REST </summary>

Pour sécuriser une API REST avec Spring Boot, on utilise souvent Spring Security.

Dans une architecture moderne, on utilise une authentification par token, souvent JWT.

Le principe est simple :

L’utilisateur s’authentifie avec login/password.  
Le backend vérifie les identifiants.  
Si c’est correct, il génère un token.  
Ensuite, le frontend envoie ce token dans chaque requête HTTP, généralement dans le header :  

```http
Authorization: Bearer token
```

Côté backend, un filtre Spring Security intercepte la requête, valide le token, récupère l’utilisateur et ses rôles, puis autorise ou refuse l’accès.

</details>

<br>

<details>

<summary> Spring Batch</summary>

> `SpringBatch` est un framework utilisé pour faire des traitements batch, souvent sur de gros volumes de données.  
> Il permet de lire, traiter et écrire des données de façon structurée, avec des jobs et des steps.  
> Il apporte aussi des mécanismes importants comme la reprise après erreur, la gestion des transactions et le suivi des exécutions.

> `use cases` : importer un fichier CSV, traiter des transactions, générer des rapports, synchroniser des données entre deux systèmes, ou faire des traitements planifiés la nuit.

```
┌──────────────────────────────────────────────────────────────────────┐
│                          Spring Batch                                │
│                                                                      │
│   ┌─────────────┐     ┌─────────────────────────────────────────┐  │
│   │ JobLauncher │────▶│                  Job                    │  │
│   └─────────────┘     │                                         │  │
│                        │  ┌──────────┐  ┌──────────┐  ┌──────┐ │  │
│   ┌─────────────┐     │  │  Step 1  │─▶│  Step 2  │─▶│  ... │ │  │
│   │JobRepository│     │  └──────────┘  └──────────┘  └──────┘ │  │
│   └─────────────┘     └─────────────────────────────────────────┘  │
│          │                      │                                    │
│          │                      ▼                                    │
│          │           ┌─────────────────────┐                       │
│          │           │  Chunk Processing   │                       │
│          │           │  ItemReader         │                       │
│          │           │  ItemProcessor      │                       │
│          │           │  ItemWriter         │                       │
│          │           └─────────────────────┘                       │
│          │                                                          │
│          └──────────────▶ Base de données (metadata tables)        │
└──────────────────────────────────────────────────────────────────────┘
```

> un `Job` représente un traitement complet. Il contient une ou plusieurs étapes,qu’on appelle des Steps.  

> Un `Step`, c’est une étape d’un traitement batch.
> Il fait une partie du Job.  
> Par exemple, dans un job d’import de fichier, un Step peut lire les données, les transformer, puis les écrire en base.  


> Chaque Step peut être de type `chunk` (le plus courant : `ItemReader` → `ItemProcessor` → `ItemWriter`) ou `Tasklet` (tâche ponctuelle).  

> Le `JobLauncher` démarre les jobs avec des `JobParameters`.
> Le `JobRepository` persiste toutes les métadonnées d’exécution en base de données (tables BATCH_*) : instances, exécutions, statistiques, contextes.


</details>

<details>

<summary> Spring Cloud</summary>

**Spring Cloud** est un ensemble de frameworks construits sur Spring Boot pour simplifier le développement de systèmes distribués et d'architectures microservices.  
Il fournit des solutions pour la configuration centralisée, la découverte de services, la communication inter-services, la résilience et load balancer.  
`Spring Cloud Netflix Eureka`: permet à un service de trouver l'adresse d'un autre service (Eureka Server (Service Registry) : Eureka Client (Service qui s'enregistre))  
`Spring Cloud LoadBalancer`: Équilibrage de charge  
`Spring Cloud Gateway`: API Gateway (Point d'entrée unique)  
`Spring Cloud Config`: Configuration centralisée  
`Spring Cloud Sleuth`: Traçage distribué  
</details>

<details>

<summary> Configuration & Spring Cloud Config</summary>

**✅ Lecture des configurations**  
🔹1 @Value (pour les propriétés simples)  
🔹2 @ConfigurationProperties(prefix = "accounts") (pour les configs structurées)  
🔹3 Interface Environment (environment.getProperty("build.version"))  
  
**✅ Ordre de priorité**  
🔹1. **Arguments en ligne de commande**: `java -jar app.jar --server.port=9090`  
🔹2. **Propriétés système JVM**: `java -Dserver.port=9090 -jar app.jar`  
🔹3. **Variables d'environnement OS**: `export SERVER_PORT=9090`  
🔹4. **application-{profile}.yml** (profil actif)  
🔹5. **application.yml** (fichier principal)  
🔹6. **@PropertySource**  
🔹7. **Propriétés par défaut**  

**✅ Spring Cloud Config Server** fournit une solution pour externaliser et centraliser les configs de plusieurs microservices.  
`Config Server`: Serveur centralisé qui stocke les configs  
`Config Clients`: Microservices qui récupèrent leurs configs depuis le ConfigServer  
**`==> Sources de config`**: Classpath, Système de fichiers, Dépôt Git   

**✅ Rafraîchir les configurations à l'exécution (3 approches)**  
🔹1: Refresh Actuator (manuel pour chaque ms) `http://{URL}/actuator/refresh`  
🔹2: Spring Cloud Bus + RabbitMQ (automatique) `http://localhost:8080/actuator/busrefresh`
🔹3: Spring Cloud Bus + GitHub Webhook (entièrement automatique)  

</details>

<details>

<summary> SYNCHRONOUS-Inter-service-Comm</summary>

`🟦 RestTemplate`: est l'ancien client HTTP Spring.  
Il est bloquant et facile à utiliser, mais maintenant déprécié.  
On le trouve surtout dans les projets legacy.  
`🟦 OpenFeign`: est un client HTTP déclaratif utilisé principalement dans les microservices.  
On définit une interface, et Spring génère l'implémentation.  
Il simplifie la communication service-à-service.  
`🟦 RestClient`: est le remplaçant moderne de RestTemplate dans Spring 6.  
Il utilise une API fluente et reste bloquant.  
Il est recommandé pour les appels HTTP simples dans les nouveaux projets.  
`🟦 WebClient`: est un client HTTP réactif et non bloquant.  
Il est plus scalable et adapté aux systèmes à haute concurrence.  
Il nécessite une connaissance de la programmation réactive.  
`🟦 HTTP Interfaces`: est une approche déclarative moderne dans Spring 6.  
On définit une interface avec des annotations, et Spring crée le client automatiquement.  
C'est propre et recommandé pour les applications modernes.  

</details>

<details>


<summary> ASYNCHRONOUS-Inter-service-Comm (rabbitMq, Kafka)</summary>

**✅RabbitMQ**: broker de messages traditionnel basé sur le protocole AMQP (Advanced Message Queuing Protocol). Il agit comme un intermédiaire qui reçoit, stocke et distribue les messages.  
**🟦 Producer**  
**🟦 Exchange**  
`Direct Exchange`: Routage basé sur une clé exacte (routing key)  
`Fanout Exchange`: Diffuse vers toutes les files liées  
`Topic Exchange`: Routage basé sur des patterns  
`Headers Exchange`: Routage basé sur les headers du message  
**🟦 Queue**  
Stocke les messages en attente de consommation (FIFO)  
Peut être durable (survit aux redémarrages) ou temporaire  
**🟦 Binding**  
Connecte un Exchange à une Queue avec une routing key  
**🟦 Consumer**  
Application qui lit et traite les messages  
Peut acquitter (ACK) ou rejeter (NACK) un message  

**✅Kafka**: Apache Kafka est une plateforme de streaming distribué haute performance, conçue pour les architectures event-driven à grande échelle.
| Critère | RabbitMQ | Kafka |
|---------|----------|-------|
| **Modèle de messagerie** | File traditionnelle (AMQP) | Log distribué / Event Stream |
| **Débit** | ~20k msg/sec | ~1M+ msg/sec |
| **Latence** | <1 ms | ~5-10 ms |
| **Persistance** | Optionnelle | Par défaut (tous les messages) |
| **Durée de rétention** | Jusqu'à consommation + ACK | Configurable (7 jours, 1 mois) |
| **Consommation** | Message supprimé après ACK | Message conservé, offset avance |
| **Replay** | ❌ Impossible | ✅ Possible |
| **Routage** | Complexe (exchange, bindings) | Simple (partition key) |
| **Cas d'usage** | Files de tâches, RPC, workflows | Event sourcing, logs, analytics, streaming |
</details>


<details>

<summary> Service Discovery </summary>

**✅ Service discovery**: est un mécanisme qui permet aux microservices de se localiser dynamiquement dans un environnement distribué.  
`CSSD (Eureka)`: signifie que le client est responsable de déterminer les IPs des instances de service disponibles et d'équilibrer la charge entre elles.  
`SSSD (Kubernetes)`: signifie qu'une infrastructure gère la découverte et le routage. Les clients ne connaissent pas ce mécanisme.  
**✅ Eureka - Architecture**  
`Eureka Server`: Registre centralisé qui maintient la liste de tous les services 🔹 Les services s'enregistrent au démarrage 🔹 Envoient des heartbeats réguliers (toutes les 30s)  
`Eureka Client`: Intégré dans chaque microservice 🔹 Récupère le registre et le met en cache localement  
✔️ `Autonomie`: Les clients peuvent fonctionner même si Eureka tombe (grâce au cache local)  
✔️ `Équilibrage de charge flexible`: Algorithmes personnalisables côté client  
❌ `Polyglot limité`: Difficile d'intégrer des services non-JVM  
❌ `Dépendance applicative`: Chaque service doit intégrer le client Eureka  
**✅ Kubernetes - Architecture**  
équilibrage de charge géré par des services qui fournissent des noms DNS et des ClusterIPs fixes, routage géré par l'ingress basé sur le hostname/path, nécessite un Ingress Controller (Nginx)  
✔️ `Polyglot`: Fonctionne avec n'importe quel langage/framework  
✔️ `Vérifications de santé natives`: sondes liveness/readiness  
✔️ `Déploiements sans interruption`: Rolling updates natifs  
❌ `Infrastructure lourde`: Nécessite un cluster K8s (coût, maintenance)  
❌ `Débogage difficile`: Plus de couches d'abstraction  

</details>


<details>


<summary> API Gateway (Spring Cloud Gateway) </summary>

> est un point d'entrée unique qui intercepte toutes les requêtes pour appliquer un traitement avant de les router vers le service approprié.
> il peut transformer une requête/réponse, implémenter l'authentification/autorisation, le Rate Limiting, la logique de Retry, le Circuit Breaker...etc

</details>

<details>

<summary> Fault Tolerance (Resilience4j)</summary>

**✅Resilience4j**: aide à protéger votre application des défaillances de services grâce à des patterns comme le circuit breaker et le retry.  
**🟦 Circuit Breaker**  
Le Circuit Breaker est un pattern qui empêche les appels répétés vers un service défaillant. Il fonctionne comme un disjoncteur électrique avec 3 états :  
`CLOSED`: Tout fonctionne normalement, les requêtes passent  
`OPEN`: Le service est détecté comme défaillant, les requêtes sont immédiatement rejetées sans appeler le service  
`HALF_OPEN`: Phase de test pour vérifier si le service s'est rétabli  
**🟦 Retry Pattern**  
Le Retry Pattern tente automatiquement de ré-exécuter une opération échouée.  
EX: Erreurs transitoires (timeout réseau, service temporairement indisponible) 5xx  
**🟦 Rate Limiting**  
Protège les services contre les surcharges et les abus/attaques (scraping, DDoS)  
Garantit une qualité de service équitable entre les utilisateurs  
Évite de surcharger les ressources backend  

</details>

<br>

<details>

<summary> Versioning Strategies, Swagger, HATEOAS, HAL, i18n</summary>


**✅Versioning Strategies**  
🔹1. Versioning via le chemin URI  
🔹2. Versioning via les paramètres de requête  
🔹3. Versioning via des headers personnalisés  
🔹4. Versioning via la négociation de contenu (Accept Header)  
**✅Swagger** : outil utilisé pour documenter et tester les APIs REST. Il fournit une interface claire où l'on peut voir et tester les endpoints de l'API. 

`@Operation`: Décrit un **endpoint REST** : son rôle, son résumé, sa description.  
`@ApiResponse`: Décrit **une réponse HTTP possible** d’un endpoint : `200`, etc.  
`@ApiResponses`: Regroupe **plusieurs @ApiResponse** pour un même endpoint.  
`@Parameter`: Documente **un paramètre d’entrée** : `PathVariable`, `RequestParam`, header, etc.  
`@Schema`: Décrit **un objet ou un champ** dans la documentation Swagger : DTO, attribut, exemple, format, etc.  
`@Tag`: Regroupe les endpoints par **catégorie** dans Swagger UI, par exemple : `Positions`, `Users`, `Orders`.  


```java
// @Tag permet de regrouper les endpoints dans Swagger UI.
// Ici, tous les endpoints de ce contrôleur seront regroupés sous le nom "Positions".
@Tag(name = "Positions", description = "API de tenue de positions")
// @RestController indique que cette classe est un contrôleur REST Spring.
// Les méthodes retournent directement des objets JSON dans la réponse HTTP.
@RestController
@RequestMapping("/api/positions")
public class PositionController {
    // `@Operation` permet de documenter un endpoint dans Swagger.
    // summary = résumé court de l'opération.
    // description = description plus détaillée de ce que fait l'endpoint.
    @Operation(
        summary = "Récupérer une position",
        description = "Retourne le détail d'une position à partir de son identifiant"
    )
    // @ApiResponses permet de lister les réponses HTTP possibles pour cet endpoint.
    // Cela permet d'afficher dans Swagger les différents cas de retour.
    @ApiResponses({
        // @ApiResponse décrit une réponse possible.
        // Ici, 200 signifie que la position a été trouvée avec succès.
        @ApiResponse(responseCode = "200", description = "Position trouvée"),
        // Ici, 404 signifie que la position demandée n'existe pas.
        @ApiResponse(responseCode = "404", description = "Position non trouvée")
    })
    @GetMapping("/{id}")
    public PositionDto getPosition(

        // @Parameter sert à documenter le paramètre dans Swagger.
        // Ici, on explique que "id" correspond à l'identifiant de la position.
        @Parameter(description = "Identifiant de la position")
        @PathVariable Long id
    ) {
        return positionService.getPosition(id);
    }
}
```



<!-- **✅HATEOAS** : est un principe REST où la réponse de l'API inclut des liens vers des actions connexes. Il aide les clients à naviguer dans l'API dynamiquement.  
**✅HAL Explorer** : outil utilisé pour explorer et tester les APIs basées sur HATEOAS. Il fournit une interface simple pour naviguer dans les liens de l'API.  
**✅Internationalization (i18n)?** : permet à une application de supporter plusieurs langues.   -->



</details>


<details>

<summary> Unit Test, TDD, Junit5, Mockito</summary>

**✅TDD**: est une pratique où l'on écrit d'abord un test qui échoue, puis on implémente le code pour le faire passer, et enfin on refactorise.
Elle aide à améliorer la qualité et la conception du code.

**✅JUnit**  
Annotations principales : `@Test`,`@BeforeEach`,`@AfterEach`,`@BeforeAll`,`@AfterAll`,`@DisplayName`,`@Disabled`,`@RepeatedTest(n)`,`@ParameterizedTest`  
Assertions courantes : `assertNotNull`, `assertThrows`, `assertIterableEquals`, `assertTimeout`, `assertAll`  

**✅Mockito**  
| Annotation | Description  | Usage |
| -  |  - |  - |
| `@Mock` | Crée un mock (faux objet) | Tests unitaires purs |
| `@Spy`| Crée un spy (objet réel avec capacité d'espionnage) | Mocking partiel rare |
| `@InjectMocks` | Injecte les mocks dans la classe testée | Classe en cours de test |
| `@Captor` | Capture les arguments passés aux méthodes | Vérification avancée |

```java
////////// Configuration Mockito
  @Mock private MyRepository repository;
  @InjectMocks private MyService service;
///////// Stubbing (Simulation de comportement)
when(repository.findById(1L)).thenReturn(Optional.of(user));
doNothing().when(repository).delete(any());
doThrow(new RuntimeException()).when(repository).delete(any());
///////// Verification (Vérification des appels)
verify(repository, times(1)).save(any());
verify(repository, never()).delete(any());
///////// Capture d'arguments
ArgumentCaptor<User> captor = ArgumentCaptor.forClass(User.class);
verify(repository).save(captor.capture());
User capturedUser = captor.getValue();
```

</details>

<details>

<summary> Spring Boot Testing</summary>

**✅Spring Boot Testing**  
| Annotation | Charge | Utilisation |
|-|-|-|
| `@SpringBootTest` | Contexte complet | Tests d'intégration E2E |
| `@WebMvcTest` | Couche Web uniquement | Tests de contrôleurs (rapide) |
| `@DataJpaTest` | Couche JPA uniquement | Tests de repositories |
| `@MockBean` | Mock dans le contexte Spring | Remplacer un bean |
| `@Autowired` | Injection de dépendance | Récupérer un bean |

```java
////// Test unitaire pur (FAST)
@ExtendWith(MockitoExtension.class)
class UserServiceTest {
  @Mock private  UserRepository repository;
  @InjectMocks  private UserService service;
}
////// intégration Spring test (SLOW)
@SpringBootTest
class UserServiceIntegrationTest {
  @MockBean  private UserRepository repository;
  @Autowired private UserService service;
}
////// controller test (OPTIMAL)
@WebMvcTest(UserController.class)
class UserControllerTest {
  @Autowired private MockMvc mockMvc;
  @MockBean private UserService userService;
}
////// Test repository using H2
@DataJpaTest
class UserRepositoryTest {
  @Autowired  private UserRepository repository;
  @Autowired private TestEntityManager entityManager;
}
```

**✅MockMvc - Controller Tests**
```java
mockMvc.perform(post("/api/users")
    .contentType(MediaType.APPLICATION_JSON)
    .content(objectMapper.writeValueAsString(user)))
  .andExpect(status().isCreated())
  .andExpect(jsonPath("$.id").exists());
```
**✅Reflection API**: La Reflection permet d'inspecter et de manipuler des classes, méthodes et champs privés à l'exécution.


</details>

<details>

<summary> BDD & Cumcumber</summary>

**✅Behavior-Driven Development** est une approche de développement logiciel où l'on définit le comportement de l'application en langage naturel, avant d'écrire le code. L'accent est mis sur le comportement métier, pas sur l'implémentation technique.

**✅Cucumber**: est un framework de test utilisé pour le BDD. Il permet d'écrire des tests en langage Gherkin avec Given / When / Then :

```gherkin
Feature: User Management
  Scenario Outline: User age validation
    Given I create a user with age <age>
    Then the validation status is "<status>"

    Examples:
      | age | status   |
      | 17  | invalid  |
      | 18  | valid    |
```
```
┌─────────────────────────────────────────────────┐
│  Feature Files (.feature)                       │
│  Scénarios en Gherkin (langage naturel)         │
└─────────────────┬───────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────┐
│  Step Definitions (Java)                        │
│  Code qui exécute chaque étape                  │
└─────────────────┬───────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────┐
│  Application Code                               │
│  Code métier testé                              │
└─────────────────────────────────────────────────┘
```

```java
    @Given("the application is started")
    public void applicationStarted() { /**/ }
    
    @When("I enter the name {string}")
    public void iEnterName(String name) { /**/ } 
```
</details>

<details>

<summary> ELK, Filebeat, Zipkin</summary>

**✅Elasticsearch** est un moteur de recherche et d'analyse distribué. Utilisé pour rechercher de grandes quantités de données très rapidement et analyser les logs et métriques  
**✅Logstash** est un outil de pipeline de traitement de données. Il est utilisé pour collecter des données depuis plusieurs sources, transformer/analyser les données, envoyer les données vers un autre système (Elasticsearch)  
**✅Kibana** est un outil de visualisation et de monitoring de données. Il est utilisé pour visualiser les données stockées dans Elasticsearch, créer des dashboards et rechercher des logs  
**✅Filebeat** est un agent léger (log shipper) qui lit les fichiers de logs et envoie les données vers Logstash ou Elasticsearch.  
**✅Zipkin** est un outil de traçage distribué. Utilisé pour suivre les requêtes à travers plusieurs microservices, mesurer la latence et identifier les goulots d'étranglement de performance  


**🔹Concepts clés Zipkin**  
`Trace` → Cycle de vie complet d'une requête  
`Span` → Une unité de travail (ex : un appel de service)  
`Trace ID` → ID unique pour la requête  
`Span ID` → ID unique pour chaque opération  
</details>



<br>



<br><br>

</details>



<details>

<summary> ✅ Oracle / PostgreSQL / SQL </summary>

<details>

<summary>Donne 2 différences entre <b>Oracle SQL</b> et <b>PostgreSQL</b> </summary>

> Première différence : Oracle est une base propriétaire, alors que PostgreSQL est open source.
>
> Deuxième différence : certaines syntaxes SQL et certains objets sont différents, par exemple les séquences, les types de données, les fonctions, ou encore les procédures stockées.

</details>

<details>

<summary>Quels sont les arguments pour migrer <b>Oracle vers PostgreSQL</b> ?</summary>

> Les principaux arguments sont surtout **le coût, la flexibilité et l’écosystème open source**.
> Oracle nécessite généralement des licences assez coûteuses, alors que PostgreSQL est open source.
> PostgreSQL est aussi une base très mature, performante et fiable, avec beaucoup de fonctionnalités avancées.
> Enfin, elle s’intègre très bien avec les environnements modernes comme le Cloud, Docker et Kubernetes.
> Par contre, une migration doit être étudiée attentivement, notamment pour les procédures stockées, les types de données et les fonctionnalités spécifiques à Oracle.


</details>

<details>

<summary>Comment tester une <b>migration Oracle vers PostgreSQL</b> ?</summary>

> Je commence par vérifier la structure de la base : tables, colonnes, types, contraintes, index et séquences.
>
> Ensuite, je contrôle les données migrées : nombre de lignes, cohérence des valeurs et comparaison avec Oracle.
>
> Puis je lance les tests applicatifs pour vérifier que les traitements donnent le même résultat.
>
> Enfin, je vérifie les performances des requêtes importantes et j’ajuste les index si besoin.

</details>

<details>

<summary>Comment gérer les <b>migrations de schéma dans Spring Boot</b> ?</summary>

> Dans Spring Boot, on utilise souvent Flyway ou Liquibase.
>
> Les scripts SQL sont versionnés dans le projet, par exemple `V1`, `V2`, `V3`.
>
> Au démarrage, l’outil applique automatiquement les scripts non exécutés.
>
> Ça permet de garder la base de données alignée avec la version de l’application.

</details>


<details>

<summary>Comment optimiser une <b>requête SQL lente</b> ?</summary>

> Je commence par regarder le plan d’exécution avec `EXPLAIN ANALYZE`.
>
> Ensuite, je vérifie les index, les jointures, les filtres, la volumétrie, la pagination, et je regarde si la requête ramène trop de colonnes ou trop de lignes.

1. Utiliser EXPLAIN pour analyser le plan d'exécution
2. Ajouter des index appropriés
3. Éviter SELECT *, spécifier les colonnes nécessaires
4. Limiter les sous-requêtes corrélées
5. Utiliser JOIN au lieu de sous-requêtes quand possible
6. Partitionner les grandes tables
7. Utiliser LIMIT pour limiter les résultats
8. Éviter les fonctions dans WHERE sur colonnes indexées

</details>

<br>

<details>

<summary>Quelle est la différence entre <b>DELETE</b> et <b>TRUNCATE</b> ? Quand utiliser l'un ou l'autre ?</summary>

`DELETE` supprime les lignes une par une. On peut utiliser une clause `WHERE` pour choisir les lignes à supprimer.

`TRUNCATE` vide directement toute la table et est généralement beaucoup plus rapide lorsqu'on veut supprimer toutes les données.

En Oracle, `TRUNCATE` fait un commit implicite, donc on ne peut pas facilement faire de rollback derrière (possible avec postgres).

Donc, si je veux supprimer certaines lignes, j'utilise `DELETE`. Si je veux vider complètement une grosse table, je peux utiliser `TRUNCATE`.

</details>


<details>

<summary>Pourquoi PostgreSQL peut-il faire un <b>Seq Scan</b> même si <b>la colonne est 👉indexée</b> ?</summary>

Il y a plusieurs raisons.

Par exemple, si la table est petite, parcourir toute la table peut être plus rapide.

Ça peut aussi arriver si la requête retourne une grosse partie des lignes. Par exemple, si 80 % des lignes ont le même statut, l'index n'apporte pas forcément beaucoup.

Ça peut également venir de statistiques qui ne sont plus à jour ou d'une fonction appliquée sur la colonne.

Donc avoir un index ne garantit pas que PostgreSQL va l'utiliser.

</details>

<details>

<summary>Sur quoi peut-on créer un <b>👉index</b> ?</summary>

On peut créer un index sur une seule colonne, mais aussi sur plusieurs colonnes : on parle alors d'index composite.

On peut également créer des index partiels, par exemple uniquement sur les lignes actives.

Et on peut créer des index sur des expressions ou des fonctions.

En pratique, je regarde surtout les colonnes utilisées régulièrement dans les `WHERE`, les `JOIN`, les tris ou les recherches.

</details>

<details>

<summary>Qu'est-ce qu'un <b>👉index composite</b> et <b>👉index partiel</b> ?</summary>

**`👉index composite`** C'est un index qui contient plusieurs colonnes.

Par exemple : `INDEX(last_name, first_name)`  
Il peut être intéressant si mes requêtes filtrent régulièrement sur `last_name` et `first_name` ensemble.  
Par contre, l'ordre des colonnes dans l'index est important.


**`👉index partiel`** indexe seulement une partie des lignes de la table.

Par exemple, si je recherche très souvent les commandes actives : `WHERE status = 'ACTIVE'`  
je peux créer un index uniquement sur ces lignes.  
L'index est donc plus petit et peut être plus efficace.  

</details>

<details>

<summary>Quel est l'intérêt d'un <b>👉index sur une fonction</b> ?</summary>

C'est utile lorsqu'une requête applique régulièrement une fonction sur une colonne.

Par exemple :

`WHERE LOWER(email) = 'test@gmail.com'`

Un index classique sur `email` ne sera pas forcément utilisé.

Je peux donc créer un index sur `LOWER(email)` pour optimiser directement ce type de recherche.

</details>

<details>

<summary>Quelle est la différence entre une <b>vue</b> et une <b>vue matérialisée</b> ?</summary>

Une vue classique ne stocke pas réellement les données.

Elle stocke essentiellement la requête et celle-ci est exécutée quand on consulte la vue.

Une vue matérialisée, au contraire, stocke physiquement le résultat de la requête.

Elle peut donc être beaucoup plus rapide pour des calculs coûteux, mais les données peuvent devenir obsolètes et il faut rafraîchir la vue matérialisée.

</details>

<details>

<summary>Quels sont les <b>niveaux d'isolation</b> d'une transaction ?</summary>


`Dirty Read`: lire une donnée non commitée  
`Non-repeatable Read`: relire une ligne et obtenir une autre valeur  
`Phantom Read`: refaire une requête et voir de nouvelles lignes  
`Serialization anomaly`: résultat impossible à expliquer par un ordre séquentiel correct  


Les quatre niveaux classiques sont :

`READ UNCOMMITTED`, `READ COMMITTED`, `REPEATABLE READ` et `SERIALIZABLE`.

Plus on augmente le niveau d'isolation, plus on protège les transactions contre les lectures incohérentes, mais ça peut réduire les performances et augmenter les blocages.

En pratique, `READ COMMITTED` est très courant.

 **Définir le niveau**

```sql
BEGIN TRANSACTION ISOLATION LEVEL REPEATABLE READ;
```

Voir le niveau actuel :

```sql
SHOW transaction_isolation;
```


</details>

<details>

<summary>Quelle est la différence entre <b>normalisation</b> et <b>dénormalisation</b> ?</summary>

La normalisation consiste à séparer les données dans plusieurs tables pour éviter la duplication et améliorer la cohérence.

Par exemple, je sépare les clients et les commandes au lieu de répéter toutes les informations du client dans chaque commande.

La dénormalisation fait l'inverse : on accepte une certaine duplication pour améliorer les performances de lecture.

Donc c'est souvent un compromis entre cohérence, simplicité et performance.

</details>


<br>


<details>

<summary> C’est quoi un <b>🔒lock</b> en base de données ?</summary>

Un lock, ou verrou, sert à protéger une donnée quand plusieurs transactions essaient d’y accéder ou de la modifier en même temps.

Le but est surtout d’éviter les incohérences et les mises à jour concurrentes incorrectes.

</details>

<details>

<summary> Peux-tu donner un exemple simple de <b>🔒lock</b> ?</summary>

Par exemple, si deux transactions veulent modifier le même compte bancaire en même temps, la base peut verrouiller la ligne pendant la première modification.

La deuxième transaction doit attendre que la première fasse un `COMMIT` ou un `ROLLBACK`.

</details>

<details>

<summary> Comment <b>🔒verrouiller</b> une ligne avec <b>PostgreSQL</b> ?</summary>

On peut utiliser `SELECT ... FOR UPDATE`.

Ça permet de lire une ligne tout en la verrouillant pour les modifications concurrentes jusqu’à la fin de la transaction.

```sql
BEGIN;

SELECT *
FROM compte
WHERE id = 1
FOR UPDATE;

UPDATE compte
SET solde = solde - 100
WHERE id = 1;

COMMIT;
```

</details>

<details>

<summary> Est-ce qu’il faut toujours faire <b>🔒FOR UPDATE</b> avant un `UPDATE` ?</summary>

Non.

Un `UPDATE` prend déjà automatiquement un verrou sur les lignes qu’il modifie.

`FOR UPDATE` est surtout utile quand je dois d’abord lire la donnée, faire une vérification métier, puis la modifier.

Par exemple :

```text
lire le solde
→ vérifier qu’il est suffisant
→ débiter le compte
```

</details>

<details>

<summary> <b>🔒pessimistic lock</b> VS <b>🔒optimistic lock</b> ?</summary>

`🔒pessimistic lock`  
Le verrouillage pessimiste consiste à verrouiller la donnée dès qu’on la lit, parce qu’on considère qu’un conflit est possible.

Avec PostgreSQL, `SELECT ... FOR UPDATE` est un exemple classique de verrouillage pessimiste.

`🔒pessimistic lock`  
Avec l’optimistic locking, on ne bloque pas directement la ligne.

On utilise généralement un numéro de version et, au moment de la mise à jour, on vérifie que personne n’a modifié la donnée entre-temps.

En JPA, on peut utiliser :

```java
@Version
private Long version;
```

Si la version a changé, on détecte un conflit.

</details>


<details>

<summary> Peut-on <b>🔒verrouiller</b> toute une <b>table</b> ?</summary>

Oui. PostgreSQL permet par exemple :

```sql
LOCK TABLE commande IN ACCESS EXCLUSIVE MODE;
```

Mais en pratique, je préfère éviter les locks de table dans une application classique, parce qu’ils peuvent bloquer beaucoup d’utilisateurs.

Je privilégie des locks plus fins, au niveau des lignes.

</details>

<details>

<summary> C’est quoi <b>ACCESS EXCLUSIVE</b> ?</summary>

C’est le verrou de table le plus fort sous PostgreSQL.

Il peut bloquer les lectures et les écritures concurrentes.

Il est notamment utilisé par certaines opérations DDL ou de maintenance comme `TRUNCATE`.

En production, il faut l’utiliser avec prudence.

```sql
LOCK TABLE commande IN ACCESS EXCLUSIVE MODE;
```

</details>


<details>

<summary> C’est quoi un <b>🔒deadlock</b> ?</summary>

Un deadlock arrive quand deux transactions s’attendent mutuellement.

Par exemple :

```text
Transaction A
lock ligne 1
attend ligne 2

Transaction B
lock ligne 2
attend ligne 1
```

PostgreSQL détecte ce type de situation et annule généralement une des transactions pour débloquer l’autre.

</details>

<details>

<summary> Comment éviter <b>les problèmes de 🔒lock</b> ?</summary>

J’essaie de garder les transactions courtes, de verrouiller uniquement les données nécessaires, d’accéder aux ressources dans le même ordre quand c’est possible, et d’avoir de bons index pour éviter que les opérations prennent trop de temps.

</details>


<br><br>

</details>

<details>
<summary> ✅ Kafka & RabbitMQ</summary>


<details>

<summary>C’est quoi <em><b>Kafka</b></em> ?</summary>

> Kafka est une plateforme de messaging orientée événement.
>
> Des producteurs publient des messages dans des topics, et des consommateurs lisent ces messages.
>
> C’est très utilisé pour les architectures distribuées, les gros volumes et les traitements asynchrones.

</details>

<details>

<summary>C’est quoi un <em><b>topic</b></em> Kafka ?</summary>

> Un topic est un canal dans lequel on publie des messages.
>
> Par exemple, un service peut publier un événement dans un topic, et plusieurs consommateurs peuvent le lire.

</details>

<details>

<summary>C’est quoi un <em><b>consumer group</b></em> ?</summary>

> Un consumer group permet de répartir la consommation des messages entre plusieurs instances.
>
> Par exemple, si j’ai trois consommateurs dans le même groupe, Kafka répartit les partitions entre eux.

</details>

<details>

<summary>C’est quoi <em><b>RabbitMQ</b></em> ?</summary>

**🟦 Producer**  
**🟦 Exchange**  
`Direct Exchange`: Routage basé sur une clé exacte (routing key)  
`Fanout Exchange`: Diffuse vers toutes les files liées  
`Topic Exchange`: Routage basé sur des patterns  
`Headers Exchange`: Routage basé sur les headers du message  
**🟦 Queue**  
Stocke les messages en attente de consommation (FIFO)  
Peut être durable (survit aux redémarrages) ou temporaire  
**🟦 Binding**  
Connecte un Exchange à une Queue avec une routing key  
**🟦 Consumer**  
Application qui lit et traite les messages  


</details>

<details>

<summary><b>Dead Letter Queue (DLQ)</b></summary>

Une Dead Letter Queue est une queue utilisée pour stocker les messages qui n’ont pas pu être traités correctement.

Par exemple, si un consumer échoue plusieurs fois sur le même message, au lieu de le rejouer indéfiniment et de bloquer la queue principale, on l’envoie dans une DLQ.

Ensuite, on peut analyser le message, comprendre l’erreur et éventuellement le retraiter.

</details>


<details>

<summary> <em><b>RabbitMQ</b></em> vs <em><b>Kafka</b></em> ?</summary>

> **RabbitMQ et Kafka sont tous les deux utilisés pour la communication asynchrone, mais ils répondent à des besoins différents.**
>
> **RabbitMQ est plutôt un message broker** : un producteur envoie un message dans une queue, un consommateur le traite, puis généralement le message est supprimé après l’acquittement. C’est très adapté aux traitements asynchrones entre microservices et aux files de tâches.
>
> **Kafka est plutôt une plateforme de streaming d’événements** : les messages sont conservés dans des topics pendant une certaine durée et plusieurs consommateurs peuvent les relire indépendamment. Kafka est donc très adapté aux gros volumes, au streaming temps réel et à l’event-driven architecture.
>
> Donc, simplement : **RabbitMQ = distribution et traitement de messages**, alors que **Kafka = stockage et diffusion de flux d’événements à grande échelle**.

</details>

<details>


<summary>Comment <b>éviter de perdre un message RabbitMQ si un consumer plante</b> ?</summary>

> Le consumer ne doit acquitter le message qu’après avoir terminé correctement le traitement.
>
> S’il plante avant l’acknowledgment, le broker peut remettre le message à disposition pour qu’il soit retraité.


</details>

<details>

<summary>Que faire si un <b>message échoue toujours après plusieurs retries</b> ?</summary>

> Après un nombre limité de tentatives, je préfère envoyer le message dans une Dead Letter Queue.
>
> Ça évite de bloquer la queue principale et permet d’analyser le message séparément.


</details>


<br><br>

</details>

<br>

<details>

<summary> ✅TypeScript </summary>


<details>
<summary> Typescript & tsconfig </summary>

**✅TypeScript**: est un **superset de JavaScript** avec un Typage statique ce qui permet la détection des erreurs à la compilation  

**tsconfig**: C’est le fichier de configuration qui définit les options de compilation TypeScript : version JS cible, chemins, règles de vérification strictes, etc

`Génération automatique`
```bash
npx tsc --init
```

</details>


<details>
<summary> Stratégie de migration </summary>

`Migration progressive`: activer allowJs, créer tsconfig.json, convertir fichier par fichier, durcir la configuration progressivement avec strict: true.

1. **Installer TypeScript**
```bash
npm install --save-dev typescript @types/node
```
2. **Créer tsconfig.json permissif**
```json
{
  "compilerOptions": {
    "target": "ES2020",
    "module": "commonjs",
    "outDir": "./dist",
    "rootDir": "./src",
    /* Mode permissif au début */
    "allowJs": true,              // ✅ Accepte les .js
    "checkJs": false,             // ❌ Ne vérifie PAS les .js encore
    "noEmit": true,               // Ne génère pas de fichiers
    "strict": false,              // Mode non-strict
    "skipLibCheck": true
  },
  "include": ["src/**/*"],
  "exclude": ["node_modules"]
}
```
3. **Tester la compilation**
```bash
npx tsc --noEmit
```

</details>


<details>
<summary> Types Tiers </summary>

Comment typer une bibliothèque JavaScript sans types 
Si une bibliothèque n'a pas de types, créez un fichier `.d.ts` :

**Structure du projet** :
```
src/
├── types/
│   └── ma-bibliotheque.d.ts
└── index.ts
```
**ma-bibliotheque.d.ts** :
```typescript
// Déclaration de module
declare module 'ma-bibliotheque-sans-types' {
  // Export de fonctions
  export function connect(url: string): Promise<Connection>;
  export function disconnect(): void;
  // Export d'interfaces
  export interface Connection {/**/}
}
```

</details>






<details>
<summary> Types pour bibliothèques globales </summary>

```typescript
// types/globals.d.ts

// Variable globale
declare var MY_GLOBAL_VAR: string;

// Fonction globale
declare function myGlobalFunction(param: number): string;

// Namespace global
declare namespace MyLibrary {
  function init(config: Config): void;
  
  interface Config {
    apiKey: string;
    debug: boolean;
  }
}
```

</details>

<br>


<details>
<summary> Types Primitifs & Builtin </summary>

`Types Primitifs` : number, string, boolean, any, unknown, void, never (représente quelque chose qui ne se produit jamais)  
`Built-in`: Array, Tuple(arr à taille fixe avec types spécifiques), Object, Function, Date, RegExp, Error

</details>

<details>
<summary> Création Custom Types(Interface, Type, Enum)  </summary>

`Interface` pour objets et classes (extensibles, declaration merging).  
`Type alias` pour unions, intersections, types primitifs.

**Interface**: Une interface définit la structure d’un objet. C’est un contrat que les objets doivent respecter.  
**Type alias**: Un type alias crée un nouveau nom pour un type. Similaire aux interfaces mais plus flexible.  
**enums**: Un enum (énumération) définit un ensemble de constantes nommées.

**🆚 Enum vs Union Type**

```typescript
// ❌ Enum : code JavaScript généré
enum Color {
  Red = "RED",
  Green = "GREEN",
  Blue = "BLUE"
}
// Génère du code JS supplémentaire

// ✅ Union Type : pas de code généré
type Color = "RED" | "GREEN" | "BLUE";
// Pas de code JS généré, juste vérification à la compilation

//// Quand utiliser Enum :
// - Besoin de reverse mapping
// - Valeurs calculées
// - Logique métier complexe autour des valeurs
//// Quand utiliser Union Type :
// - Types simples
// - Performance critique
// - Bibliothèques
```
</details>


<details>
<summary> generics & Utilitaires Built-in(Partial, Required...) </summary>

Les génériques permettent de créer des composants réutilisables qui fonctionnent avec plusieurs types plutôt qu’un seul.

**🎯 Génériques Utilitaires Built-in**

```typescript
interface User {id: string; name: string; email: string; age: number;}
// Partial<T> : all properties optional
type PartialUser = Partial<User>;
// Required<T> : all properties required
type RequiredUser = Required<Partial<User>>;
// Readonly<T> : all properties readonly
type ReadonlyUser = Readonly<User>;
// Pick<T, K> : select certain properties
type UserBasicInfo = Pick<User, "id" | "name">;
// =====> { id: string; name: string; }
// Omit<T, K> : exclude certain properties
type UserWithoutId = Omit<User, "id">;
// { name: string; email: string; age: number; }
// Record<K, T> : object with keys K and values T
type UserRoles = Record<string, "admin" | "user" | "guest">;
// =====> { [key: string]: "admin" | "user" | "guest" }
// Exclude<T, U> : exclude from union
type Role = "admin" | "user" | "guest";
type NonAdminRole = Exclude<Role, "admin">;
// =====> "user" | "guest"
// Extract<T, U> : extract from union
type AdminRole = Extract<Role, "admin">;
// =====>"admin"
// NonNullable<T> : exclude null and undefined
type MaybeString = string | null | undefined;
type DefinitelyString = NonNullable<MaybeString>;
// =====> string
// ReturnType<T> : function return type
function getUser() {
  return { id: "1", name: "John" };
}
type UserReturnType = ReturnType<typeof getUser>;
// =====> { id: string; name: string; }

// Parameters<T> : parameter types
function createUser(name: string, age: number) {
  return { name, age };
}
type CreateUserParams = Parameters<typeof createUser>;
// =====> [name: string, age: number]
// Type : {...} (without Promise) => if fetchUserData (promise)
type UserData = Awaited<ReturnType<typeof fetchUserData>>;
```

</details>

<br>



<details>
<summary> Opérateurs (keyof, typeof) </summary>

`keyof` crée un type union de toutes les clés d’un type donné.  

`typeof` en TypeScript permet d’obtenir le type d’une valeur ou variable  
⚠️ Différence importante : typeof TypeScript (compile-time) ≠ typeof JavaScript (runtime)

**🔧 typeof avec as const**

```typescript
const COLORS = {
  primary: "#007bff",
  secondary: "#6c757d",
  success: "#28a745",
  danger: "#dc3545"
} as const;

type Colors = typeof COLORS;
// {
//   readonly primary: "#007bff";
///  ...
//   readonly danger: "#dc3545";
// }

// keyof typeof COLORS => "primary" | "...| "danger"

// Extract the type of values
type ColorValue = typeof COLORS[keyof typeof COLORS];
// "#007bff" | "#6c757d" | "#28a745" | "#dc3545"
```

</details>




<details>
<summary> Types dynamiques limités avec Record </summary>

> Le type **Record<K, V>** c’est un type utilitaire qui permet de créer un des objet typéq avec clé-valeur

```typescript
type StringDictionary = Record<string, string>;

const translations: StringDictionary = {
  hello: "bonjour",
  goodbye: "au revoir",
  thanks: "merci"
};

// Ajout dynamique
translations["welcome"] = "bienvenue";  // ✅ OK
translations["yes"] = 123;              // ❌ Erreur
```

</details>

<br>

<details>
<summary> Décorateurs </summary>

> Une fonction qui modifie ou annote une classe, méthode ou propriété sans modifier son code source. Permet d’ajouter des comportements de manière déclarative.

**🎨 Types de décorateurs**

🔹 1. **Décorateur de classe** : `@ClassDecorator`  
🔹 2. **Décorateur de méthode** : `@MethodDecorator`  
🔹 3. **Décorateur de propriété** : `@PropertyDecorator`  
🔹 4. **Décorateur d'accesseur** : `@AccessorDecorator`  
🔹 5. **Décorateur de paramètre** : `@ParameterDecorator`  


**Avantages :**  
✅ **Séparation des préoccupations** : logique métier séparée des aspects transversaux  
✅ **Réutilisabilité** : même décorateur sur plusieurs méthodes/classes  
✅ **Lisibilité** : code plus déclaratif  
✅ **Non-intrusif** : pas de modification du code original  

**Cas d'usage courants :**  
🔹 📊 Logging et monitoring  
🔹 ⏱️ Mesure de performance  
🔹 🔒 Vérification de permissions  
🔹 ✅ Validation de données  
🔹 💾 Cache et mémoization  
🔹 🔄 Retry automatique  
🔹 📝 Métadonnées (Angular, NestJS)  

</details>


<br><br>


</details>

<details>

<summary> ✅ RxJs </summary>





<details>
<summary> RxJS & observable & Subscription </summary>

**RxJS**: (Reactive Extensions for JavaScript) est une bibliothèque pour la programmation réactive utilisant des Observables. Elle permet de gérer facilement les flux de données asynchrones et les événements

**Observable vs Promise:**
| Aspect | Promise | Observable |
|--------|---------|------------|
| **Valeurs** | 1 seule valeur | 0, 1 ou plusieurs valeurs |
| **Lazy** | ❌ Non (s'exécute immédiatement) | ✅ Oui (exécute à la souscription) |
| **Annulable** | ❌ Non | ✅ Oui (unsubscribe) |
| **Opérateurs** | then, catch | map, filter, merge, etc. (100+) |
| **Multicast** | ❌ Non | ✅ Oui (avec Subjects) |

`Observable`: Un flux de données asynchrone qui peut émettre 0, 1 ou plusieurs valeurs dans le temps.  
`Observer`: Un objet qui définit comment réagir aux valeurs émises.  
`Subscription`: représente l'exécution d'un Observable. Elle permet de **désouscrire**.  
`Opérateurs`: Des fonctions qui transforment, filtrent ou combinent des Observables.  
`Subject`: Il est à la fois un Observable ET un Observer. Il permet le **multicast**.  
</details>

<details>
<summary> Subjects (BehaviorSubject, ReplaySubject, AsyncSubject)  </summary>

**Subjects**: est à la fois un Observable et un Observer  
✅ `Subject` : Multicast basique  
✅ `BehaviorSubject` : garde la dernière val émise et l’envoie immédiatement aux nveaux souscripteurs.  
✅ `ReplaySubject` : garde N dernières val et les rejoue pour les nveaux souscripteurs.  
✅ `AsyncSubject` : AsyncSubject émet seulement la dernière val quand le Subject est complété.  


| Type | Valeur Initiale | Replay | À la Souscription |
|------|----------------|--------|-------------------|
| **Subject** | ❌ Non | ❌ Aucune | Rien |
| **BehaviorSubject** | ✅ Oui (obligatoire) | ✅ Dernière valeur | Dernière valeur |
| **ReplaySubject** | ❌ Non | ✅ N dernières | N dernières valeurs |
| **AsyncSubject** | ❌ Non | ✅ Dernière (si complete) | Rien jusqu'à complete |

</details>

<details>
<summary> Opérateurs de Création (Observavles (of, from, interval..))</summary>

✅ `of()` : Émettre des valeurs directement  
✅ `from()` : Convertir arrays, Promises, iterables  
✅ `interval()` : Émissions à intervalle régulier  
✅ `timer()` : Délai initial puis intervalle  
✅ `fromEvent()` : Événements DOM  

</details>


<details>
<summary> Opérateurs de Transformation  (de Base, Flattening, Accumulation)</summary>

**Opérateurs de Base**  
✅ `map()` :  transforme chaque valeur émise en appliquant une fonction.  
✅ `mapTo()` : Mapper vers une constante  
✅ `pluck()` : Déprécié, utiliser map() à la place  

**Opérateurs de Flattening**: transforment un Observable d’Observables en un Observable simple.  
✅ `mergeMap()` : lance les traitements en parallèle. Il est utile quand l’ordre n’est pas important.    
✅ `switchMap()` : annule l’appel précédent quand un nouvel appel arrive. Il est très utilisé pour les recherches dynamiques, par exemple une barre de recherche.  
✅ `concatMap()` :  exécute les appels un par un, dans l’ordre. Il est utile quand l’ordre est important.  
✅ `exhaustMap()` : Ignore pendant l’exécution (boutons)  

    Quand Utiliser Quel Opérateur ?
    - switchMap() -> Recherche/Autocomplétion (debounceTime)
    - mergeMap() -> Plusieurs Requêtes indépendantes
    - concatMap() -> Opérations dans l’ordre
    - exhaustMap() -> Éviter double-submit
    🆚 map() vs switchMap() ?
      🔹 map() transforme une valeur
      🔹 switchMap() transforme une valeur en nouvel Observable

**Accumulation**  
✅ `scan()` : applique une fn d’accumul sur chaque valeur(comme reduce mais émet à chaque étape)  
✅ `reduce()` : reduce() comme scan(), mais émet seulement la valeur finale (après complétion)  
✅ `toArray()` : collecte toutes les valeurs et les émet dans un tableau à la complétion  

</details>



<details>
<summary> Opérateurs de Filtrage (Filtrage, Gestion du Temps, Déduplication) </summary>

**Filtrage de Valeurs**  
✅ `filter()`: Condition de filtrage  
✅ `take(n)`: Prendre N valeurs  
✅ `takeUntil()`: Arrêter avec un signal externe (pattern de unsubscribe)  
✅ `takeWhile()`: Continuer tant que condition vraie  
✅ `skip(n)` : Sauter les N premières valeurs  

**Gestion du Temps**  
✅ `debounceTime()`: attend que N millisecondes se soient écoulées sans nouvelle émission(Recherche, validation (attendre le silence))  
✅ `throttleTime()`:  émet la première valeur, puis ignore les suivantes pendant N millisecondes (Scroll, resize (limiter la fréquence))  
✅ `auditTime()`: ignore les valeurs pendant N millisecondes, puis émet la dernière valeur reçue  
✅ `sampleTime()`: émet la dernière valeur à intervalles réguliers  

**Déduplication**  
✅ `distinct()`:  ne laisse passer que les valeurs qui n’ont jamais été émises auparavant  
✅ `distinctUntilChanged()` :  ne laisse passer que les valeurs différentes de la précédente.  
✅ `distinctUntilKeyChanged()`:  compare une propriété spécifique entre les valeurs consécutives  

</details>



<details>
<summary> Gestion des Erreurs & Finalisation </summary>


**Gestion d’Erreurs**  
✅ `catchError()` : intercepte les erreurs et permet de retourner un Observable de remplacement (fallback)  
✅ `retry()` : réessaye automatiquement un Observable en cas d’erreur  
✅ `retryWhen()` : permet de contrôler précisément quand et comment réessayer  
✅ `throwError()` : Créer une erreur Observable  

**Finalisation**  
✅ `finalize()` : exécute une fonction de nettoyage toujours, que l’Observable se complète, émette une erreur, ou soit annulé  
✅ `tap()` : Effets de bord sans modifier le flux  

**Plusieurs approches gestions d'erreurs**

**1. Dans subscribe() :**
```typescript
obs$.subscribe({
  next: v => console.log(v),
  error: e => console.error('Erreur:', e)
});
```

**2. Avec catchError() :**
```typescript
obs$.pipe(
  catchError(err => {
    console.error(err);
    return of([]); // Valeur de fallback
  })
).subscribe(/* ... */);
```
**3. Avec retry() :**
```typescript
obs$.pipe(
  retry(3),  // Réessayer 3 fois
  catchError(err => of([]))
).subscribe(/* ... */);
```

</details>


<br><br>



</details>

<details>

<summary> ✅ Angular </summary>

<details>
<summary> Architecture Projet </summary>

Une application Angular est généralement organisée en composants, services, routes, modèles et modules ou features.  

Le composant gère l’affichage et les interactions utilisateur.  

Le service contient la logique partagée, les appels HTTP, ou la communication avec le backend.  

Le routing permet de naviguer entre les pages.
  
Aujourd’hui, avec Angular moderne, on utilise beaucoup les standalone components, le lazy loading, les guards, les interceptors, RxJS, et parfois les signals.  

> ├── `angular.json` Configuration principale Angular CLI (build, serve, assets, styles…)  
> ├── `node_modules` Dépendances npm installées (généré automatiquement)  
> ├── `package.json` Fichier npm : dépendances, scripts, infos projet  
> ├── `package-lock.json` Verrouille les versions exactes des dépendances  
> ├── `public` Fichiers statiques copiés tels quels dans le build  
> │   └── `favicon.ico` Icône affichée dans l’onglet du navigateur  
> ├── `src` Code source principal de l’application  
> │   ├── `app` Dossier principal de l’application Angular  
> │   │   ├── `app.ts` Composant racine de l’application  
> │   │   ├── `app.config.ts` Configuration globale (providers, router, interceptors…)  
> │   │   └── `app.routes.ts` Définition des routes (navigation)  
> │   ├── `index.html` Fichier HTML principal chargé par le navigateur  
> │   ├── `main.ts` Point d’entrée de l’application (bootstrap Angular)  
> │   └── `styles.css` Styles globaux de l’application  
> ├── `tsconfig.app.json` Configuration TypeScript pour l’application  
> ├── `tsconfig.json` Configuration TypeScript globale du projet  
> └── `tsconfig.spec.json` Configuration TypeScript pour les tests unitaires  

</details>


<details>

<summary> Module Angular & composant standalone </summary>

En Angular, **un module** est une façon de **regrouper des éléments liés** d’une application.

Par exemple, un module peut contenir :

* des composants  
* des services  
* des pipes
* des directives
* des routes

Avant Angular standalone, on utilisait beaucoup les **NgModules** avec `@NgModule`.

Exemple :

```ts
@NgModule({
  declarations: [UserComponent],
  imports: [CommonModule],
  providers: [UserService]
})
export class UserModule {}
```

Un composant `standalone` en Angular, c’est un composant qui peut fonctionner sans être déclaré dans un NgModule.

</details>

<details>
<summary> Component </summary>

Un composant est une brique reutilisable de l'application composé d'un :
- Le **contenu** (HTML)
- Le **style** (CSS/SCSS)
- Le **comportement** (TypeScript) (obligatoire)
- Les **tests** (Spec)

exemple `app.ts`
```typescript
import { Component } from '@angular/core';
import { CollectionItemCard } from './components/collection-item-card/collection-item-card';

@Component({
  selector: 'app-root',
  imports: [CollectionItemCard],  // ⭐ Important !
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss'
})
export class AppComponent {}
```

</details>


<details>
<summary> Inputs </summary>

Un input permet de passer des valeurs d’un composant parent vers un composant enfant.

**Parent Component (app.component.html)**
```html
<!-- Sans crochets = string -->
<app-collection-item-card name="Héro"></app-collection-item-card>
<!-- Avec crochets = expression TypeScript -->
<app-collection-item-card [price]="20"></app-collection-item-card>
<img [src]="item().image" alt="Item" />
```

```typescript
item = input<CollectionItem>();  // Type explicite
// InputSignal est le type retourné par input()
item: InputSignal<CollectionItem> = input<CollectionItem>();
// required
item = input.required<CollectionItem>();
```
**Transformer la Valeur**
```typescript
item = input.required<CollectionItem, CollectionItem>({
  transform: (value: CollectionItem) => {
    value.price = value.price * 1.17;  // Conversion € -> $
    return value;
  }
});
```
```typescript
input.required<TypeRetour, TypeEntree>({
  transform: (value: TypeEntree) => TypeRetour
});
```
</details>


<details>
<summary> Outputs </summary>

`Output` : Émettre des événements de l’enfant vers le parent  
`OutputEmitterRef` : Type pour les outputs  
`output()` : Fonction pour créer un output  
`emit()` : Méthode pour envoyer un événement  
`model()` : Combine input + output (two-way binding)  
`$event` : Variable contenant la valeur émise  

```typescript
export class SearchBar {
  searchButtonClicked: OutputEmitterRef<void> = output<void>();
  searchChange = output<string>(); // output avec données

  searchClick() {
    this.searchButtonClicked.emit();
  }

  updateSearch(searchText: string) {
    this.searchChange.emit(searchText);
  }
}
```
</details>


<details>
<summary> FormsModule et ngModel </summary>

```typescript
@Component({
  imports: [FormsModule],  // ⭐ Nécessaire pour ngModel
})
export class SearchBar {
  search = input("Initial");
}
```

**Binding avec ngModel**

```html
<!-- - **[ngModel]** : Lie la valeur affichée (Input) -->
<!-- - **(ngModelChange)** : Détecte les changements (Output) -->
<input 
  [ngModel]="search()" 
  (ngModelChange)="updateSearch($event)"
/>
```

**Two-Way Binding** 🔄 Synchronisation automatique entre la vue(HTML)/modèle(TS)

```html
<!-- Input : propertyName -->
<!-- Output : propertyNameChange -->
<app-search-bar 
  [(search)]="search"
></app-search-bar>
```

**model**: combine automatiquement Un `input` et `output` avec le nom `propertyChange`

```typescript
  // search: InputSignal<string>; searchChange: OutputEmitterRef<string> 
  search = model("Initial");
```

</details>


<details>
<summary> Signals (Angular 16+) & computed & effect</summary>

Valeur réactive qui notifie automatiquement Angular des changements  
`signal()` : Créer une valeur modifiable  
`computed()`: crée un signal dérivé qui se recalcule automatiquement quand ses dépendances changent  
`effect()` exécute du code automatiquement quand les signals qu’il utilise changent  

**Modifier un Signal**: `set()` : Nouvelle valeur complète, `update()` : Fonction de transformation

</details>


<details>
<summary> Zone Detection</summary>

`Zone.js`vs `Zoneless` : Deux approches de détection de changement  
`OnPush Strategy` : Stratégie de détection optimisée  

**1) Zone.js**: intercepte tous les events: Clics, clavier, Appels HTTP, setTimeout/setInterval, Angular vérifie ensuite tous les composants potentiellement affectés. (-)  Peut être lent avec beaucoup de composants  


**2) Stratégie OnPush**
```typescript
@Component({
  changeDetection: ChangeDetectionStrategy.OnPush,  // activation
})
export class MyComponent {}
```
Le composant est vérifié UNIQUEMENT si :  
✅ 1. Un **signal** (input, signal, computed) change  
✅ 2. Un **événement** se produit dans le composant  
✅ 3. Un **AsyncPipe** reçoit une nouvelle valeur  
✅ 4. **markForCheck()** est appelé manuellement  


**3) Mode Zoneless** sans Zone.js (depuis Angular 20.2) :  
> 🔹Plus de dépendance à Zone.js  
> 🔹Détection basée sur les **notifications explicites**  
> 🔹Meilleures performances  
>
> `==> Notifications en Mode Zoneless:`  
> 🔹1. **Signals** : Changements de `signal()`, `computed()`  
> 🔹2. **Inputs** : Modifications des inputs de composants  
> 🔹3. **Événements** : Clics, saisies dans le template  
> 🔹4. **AsyncPipe** : Nouvelles valeurs async  
> 🔹5. **markForCheck()** : Appel manuel  


</details>


<details>
<summary> Boucles et Conditions</summary>

les directives structurelles les plus utilisées ?

> `*ngIf` : affiche ou cache un élément selon une condition  
> `*ngFor` : répète un élément pour chaque item d'une liste  
> `*ngSwitch` : affiche un élément parmi plusieurs selon une valeur  
>
> L'étoile `*` est une notation abrégée.  
Angular la transforme en `<ng-template>` en interne.


`@for` : Itérer sur des listes dans les templates  
`@empty` : Gérer les listes vides  
`@if / @else` : Conditions dans les templates  
`@switch / @case` : Multiple conditions  
`@let` : Déclarer des variables dans les templates  
`track` : Identifier les éléments pour l’optimisation  


| Variable | Type | Description |
|----------|------|-------------|
| `$index` | number | Index de l'élément (0, 1, 2...) |
| `$first` | boolean | Premier élément |
| `$last` | boolean | Dernier élément |
| `$even` | boolean | Index pair |
| `$odd` | boolean | Index impair |
| `$count` | number | Nombre total d'éléments |

</details>


<details>
<summary> Service & HttpClientModule </summary>

Un `service Angular`, c’est une classe injectable qui permet de centraliser une logique réutilisable, comme les appels API ou le partage de données.  
Et quand il est déclaré avec providedIn: 'root', il est singleton dans toute l’application.

> `Service` : Classe singleton injectable pour la logique métier  
> `@Injectable` : Decorator pour rendre une classe injectable  
> `inject()` : Fonction pour injecter un service  
> `providedIn`: ‘root’ : Service disponible dans toute l’application  


Comment faire des appels HTTP dans Angular ?

> Angular fournit le `HttpClient` service. On l'injecte dans le service ou le component, puis on utilise les méthodes get(), post(), put(), delete(), etc.  
Ces méthodes retournent des Observables. Pour les utiliser, il faut importer `HttpClientModule`dans le module.

</details>


<details>
<summary> Routing & lazy loading</summary>

Le sys de routing d’Angular permet de créer des apps (SPA) avec navigation entre différentes vues.

**1. app.routes.ts**: Ce fichier contient la configuration de toutes les routes de l’application   
> `path: ''` : route vide (racine de l’application)  
> `redirectTo: 'home'` : redirige vers /home  
> `pathMatch: 'full'` : correspondance exacte du chemin complet  
> `path: 'item/:id'` : route avec paramètre dynamique  
> `path: '**'` : route wildcard pour les 404 (à placer en dernier)  

**2. RouterOutlet dans le Template**: est le conteneur où s’affichent les composants routés

**3. Configuration dans app.config.ts**
```typescript
export const appConfig: ApplicationConfig = {
  providers: [
    provideZonelessChangeDetection(),
    provideRouter(routes, withComponentInputBinding())
  ]
};
```

**Note** : `withComponentInputBinding()` permet de recevoir les paramètres de route directement comme inputs du composant.

---

Le `lazy loading` c'est le fait de charger un module seulement quand l'utilisateur navigue vers sa route, pas au démarrage de l'application.  
Ça améliore les performances car le bundle initial est plus petit.  
Pour le configurer, on utilise loadChildren dans la configuration des routes avec une fonction qui importe le module dynamiquement.


</details>


<details>
<summary> Navigation</summary>

Navigation avec RouterLink (HTML)
```html
<!-- Navigation avec paramètre -->
<a [routerLink]="['/item', item.id]">Voir l'item</a>
<!-- Avec classe active -->
<a [routerLink]="['/home']" routerLinkActive="active">Accueil</a>
```

Navigation Programmatique (TypeScript)
```typescript
export class MyComponent {
  // service pour accéder aux informations de la route active
  private route = inject(ActivatedRoute);
  private router = inject(Router);
  
  navigateToHome() {
    this.router.navigate(['/home']);
  }
  
  navigateToItem(id: number) {
    this.router.navigate(['/item', id]);
  }
  
  navigateWithQueryParams() {
    this.router.navigate(['/search'], {
        queryParams: { q: 'angular', page: 1 }
    }); // URL résultante : /search?q=angular&page=1
  }
  
  navigateRelative() { // relative à la route actuelle
    this.router.navigate(['../'], { relativeTo: this.route });
  }

  ngOnInit() {
    const id = this.route.snapshot.paramMap.get('id');
    console.log('Item ID:', id);
  }
}
```

</details>


<details>
<summary> Reactive Forms</summary>

> Les `Reactive Forms`, c’est une manière de créer et gérer les formulaires Angular directement dans le TypeScript.  
> On utilise FormGroup et FormControl pour représenter les champs du formulaire.  
> C’est très pratique pour les formulaires complexes, avec des validations, des erreurs, ou des comportements dynamiques.  

Angular propose trois approches pour gérer les formulaires :  
`Template-driven` forms : logique dans le template HTML  
`Reactive forms` : logique dans le code TypeScript (recommandé)  
`Signal forms` : nouvelle approche expérimentale avec signals  

**Avantages Formulaires Réactifs**  
✅ `Testabilité` : logique dans le TypeScript, facile à tester  
✅ `Validation` complexe : validateurs synchrones et asynchrones  
✅ `Typage` fort : détection d’erreurs à la compilation  
✅ `Réactivité` : Observable pour suivre les changements  
✅ `Dynamisme` : ajouter/supprimer des contrôles facilement  
✅ `Performances` : meilleur contrôle du change detection  


**FormControl** représente un champ de formulaire individuel.
```typescript
const nameControl = new FormControl('');
const ageControl = new FormControl(25);
const emailControl = new FormControl('', [Validators.required, Validators.email]);
const disabledControl = new FormControl({value: 'test', disabled: true});
```
**FormGroup** regroupe plusieurs `FormControl` en un objet.
```typescript
const userForm = new FormGroup({
    firstname: new FormControl('', Validators.required),
    email: new FormControl('', [Validators.required, Validators.email]),
    age: new FormControl(null, [Validators.min(18), Validators.max(100)])
});
```
**FormBuilder** simplifie la création de formulaires.
```typescript
export class MyComponent {
    private formBuilder = inject(FormBuilder);
    
    userForm = this.formBuilder.group({
        firstname: ['', Validators.required],
        email: ['', [Validators.required, Validators.email]],
        age: [null, [Validators.min(18), Validators.max(100)]]
    });
}
```

</details>


<details>
<summary> Validation</summary>

```typescript
import { Validators } from '@angular/forms';

userForm = this.formBuilder.group({
    name: ['', Validators.required],
    username: ['', [Validators.minLength(3), Validators.maxLength(20)]],
    age: [null, [Validators.min(18), Validators.max(100)]],
    email: ['', [Validators.required, Validators.email]],
    phone: ['', Validators.pattern(/^[0-9]{10}$/)]
});
```

**Afficher les Erreurs**
```html
<div>
    <input formControlName="email" type="email">
    <!-- Affichage des erreurs -->
    @if (userForm.controls.email.invalid && userForm.controls.email.touched) {
        <div class="error">
            @if (userForm.controls.email.hasError('required')) {
                <span>L'email est requis</span>
            }
            @if (userForm.controls.email.hasError('email')) {
                <span>Format d'email invalide</span>
            }
        </div>
    }
</div>
```

</details>



<details>
<summary> Angular Material </summary>

la LIB officielle de composants UI pour Angular. Elle fournit des composants prêts et personnalisables pour créer des interfaces professionnelles rapidement

**Avantages**:  
✅ `Composants pré-construits` : boutons, formulaires, tables, dialogues, etc.  
✅ `Design cohérent` : Material Design de Google  
✅ `Accessibilité (a11y)` : ARIA, navigation clavier intégrée  
✅ `Responsive` : s’adapte aux différentes tailles d’écran  
✅ `Thèmes` : personnalisation facile des couleurs  
✅ `Animations` : transitions fluides intégrées  
✅ `Documentation` : exemples complets et API claire  

**Composants Principaux**  
`Formulaires` : Input, Select, Checkbox, Radio, Slider, Datepicker  
`Boutons` : Button, FAB, Icon Button, Button Toggle  
`Navigation` : Toolbar, Menu, Sidenav, Tabs  
`Layout` : Card, Expansion Panel, Stepper, Divider  
`Data` : Table, Paginator, Sort, Tree  
`Popups` : Dialog, Snackbar, Tooltip, Bottom Sheet  

</details>


<details>
<summary> Interceptors HTTP</summary>

Les interceptors permettent d'intercepter toutes les requêtesHTTP pour les modifier avant envoi.

```typescript
// interceptors/auth-token/auth-token-interceptor.ts
import { HttpInterceptorFn } from '@angular/common/http';

export const authTokenInterceptor: HttpInterceptorFn = (req, next) => {
    // Récupérer le token depuis localStorage
    const token = localStorage.getItem('token');
    let requestToSend = req;
    if (token) {
        // Ajouter le header Authorization avec le token
        const headers = req.headers.set('Authorization', 'Bearer ' + token);
        // Cloner la requête avec les nouveaux headers
        requestToSend = req.clone({ headers: headers });
    }
    // Passer la requête (modifiée ou non) au prochain interceptor/backend
    return next(requestToSend);
};
```

🔹1. Un interceptor est une fonction qui reçoit `(req, next)`  
🔹2. `req` : la requête HTTP à intercepter  
🔹3. `next` : fonction pour passer au prochain interceptor ou faire l'appel HTTP  
🔹4. On clone la requête pour la modifier (les requêtes HTTP sont immutables)  
🔹5. On ajoute le header `Authorization: Bearer <token>`  

</details>

<details>
<summary> Guards de Route</summary>

Les guards protègent les routes en vérifiant des conditions avant la navigation.  
`CanActivate` : peut-on activer cette route ?  
`CanDeactivate` : peut-on quitter cette route ?  
`CanActivateChild` : peut-on activer les routes enfants ?  
`CanLoad` : peut-on charger un module lazy-loaded ?  

```typescript
// guards/is-logged-in/is-logged-in-guard.ts
import { CanActivateFn, Router } from '@angular/router';

export const isLoggedInGuard: CanActivateFn = (route, state) => {
    const loginService = inject(LoginService);
    const router = inject(Router);
    // Cas 1: On ne sait pas encore si l'utilisateur est connecté
    if (loginService.user() === undefined) {
        // Vérifier auprès de l'API
        return loginService.getUser().pipe(
            map(user => {
                // Si on a un utilisateur, autoriser l'accès
                if (user) { return true;}
                // Sinon, rediriger vers login
                return router.createUrlTree(['/login']);
            }),
            catchError(() => {
                // En cas d'erreur, rediriger vers login
                return of(router.createUrlTree(['/login']));
            })
        );
    }
    // Cas 2: L'utilisateur n'est pas connecté (null)
    if (loginService.user() === null) {
        return router.createUrlTree(['/login']);
    }
    // Cas 3: L'utilisateur est connecté
    return true;
};
```

- Le guard retourne `true` pour autoriser l'accès
- Il retourne `false` ou une `UrlTree` pour rediriger
</details>



<details>

<summary> Authentification OAuth2 avec Angular </summary>

Pour une application Angular qui consomme des APIs sécurisées, on peut utiliser OAuth2 avec OpenID Connect.

En général, l’utilisateur est redirigé vers un serveur d’identité comme Keycloak, Auth0 ou Azure AD.

Après connexion, Angular reçoit un token, souvent un access token JWT.

Ensuite Angular envoie ce token dans les appels API avec un interceptor HTTP :

```http
Authorization: Bearer token
```

Côté backend Spring Boot, on configure Spring Security comme Resource Server pour valider le token.

Donc Angular gère la partie login/token, et le backend vérifie le token et les droits.

</details>


<details>
<summary> ngClass, ngStyle, @ViewChild, lifecycle hooks, Perfermance </summary>

<details>

<summary> 🔷 ngClass et ngStyle ?  </summary>

Ce sont des attribute directives.  
`ngClass` permet d'ajouter ou retirer des classes CSS dynamiquement selon des conditions.  
`ngStyle` permet de modifier directement des styles inline selon des valeurs dynamiques.

</details>

<details>

<summary> 🔷 viewChild ? </summary>

Avec `@ViewChild`, on peut accéder à un élément défini dans le HTML depuis le fichier TypeScript.  
Par exemple, si on a un input dans le template, on peux le récupérer pour faire un focus ou appeler une méthode dessus.  

</details>

<details>

<summary> 🔷 Les lifecycle hooks dans Angular ? </summary>

Ce sont des méthodes spéciales qu'on peut implémenter dans un component pour exécuter du code à des moments précis du cycle de vie du component. Les plus importants sont :  

`ngOnInit()` : appelé une fois après l'initialisation du component, c'est là qu'on fait les appels HTTP ou l'initialisation des données
`ngOnChanges()` : appelé quand les inputs changent
`ngOnDestroy()` : appelé juste avant la destruction du component, c'est là qu'on se désabonne des observables
`ngAfterViewInit()` : appelé après que la vue (le template) est initialisée

</details>

<details>

<summary> 🔷 Optimiser les performances d'une application Angular ? </summary>

Plusieurs techniques existent :  
> Utiliser OnPush change detection strategy  
> Utiliser le lazy loading pour les modules  
> Utiliser le pipe async au lieu de s'abonner manuellement  
> Utiliser trackBy dans les *ngFor pour éviter de re-rendre toute la liste  
> Éviter les calculs complexes dans les templates  
> Utiliser les observables à la place des setInterval ou setTimeout  

</details>

</details>

<br>


<details>
<summary> Spies & Mocking (spyOn et createSpyObj)</summary>

`spyOn(objet, 'nomDeLaMethode')` remplace la méthode par une version fictive qui :  
**ne fait rien** par défaut (elle ne déclenche pas l'implémentation réelle).  
**enregistre** si elle a été appelée, combien de fois, et avec quels arguments.  

```typescript
it('should call mySharedFunction func', () => {
  const shared = new SharedService(); // instance réelle
  const calc = new CalcService(shared);
  // Remplace mySharedFunction par une fonction vide.
  spyOn(shared, 'mySharedFunction');
  // La vraie implémentation (console.log, appel HTTP...) ne s'exécute PAS.
  const result = calc.multiply(3, 5);
  // Vérifie que multiply() a bien appelé mySharedFunction()
  expect(shared.mySharedFunction).toHaveBeenCalled();

  //le spy enregistre l’appel ET la vraie méthode s’exécuter :
  spyOn(shared, 'mySharedFunction').and.callThrough();
  //Spy avec retour de valeur :
  spyOn(calculator, 'add').and.returnValue(100);
  spyOn(calculator, 'divide').and.throwError('Div by zero');
});
```
`jasmine.createSpyObj` — Crée un faux objet avec fausse méthodes
```typescript
it('should call mySharedFunction func', () => {
  // Le constructeur de SharedService n'est JAMAIS appelé.
  let shared = jasmine.createSpyObj('SharedService', ['mySharedFunction']);
  const calc = new CalcService(shared);
  const result = calc.multiply(3, 5);
  expect(shared.mySharedFunction).toHaveBeenCalled();
});
```
| | `spyOn` | `jasmine.createSpyObj` |
|---|---|---|
| Nécessite une vraie instance | ✅ Oui | ❌ Non |
| Exécute le constructeur réel | ✅ Oui | ❌ Non |
| Remplace une méthode | Une seule | Plusieurs à la fois |
| Usage typique | Objet déjà instancié | Faux service sans effet de bord |
</details>



<details>
<summary> TestBed et Dependency Injection </summary>

`TestBed` simule un module Angular temporaire pour les tests. On le configure avec `TestBed.configureTestingModule(...)`
```typescript
TestBed.configureTestingModule({
  imports: [],       // modules à importer
  providers: [],     // services à fournir
  declarations: []   // composants/pipes/directives (pour modules non-standalone)
});
```
`TestBed.inject` — récupérer une instance du service

```typescript
const service = TestBed.inject(MonService);
// Angular résout les dépendances automatiquement grâce au DI.
```
`Injecter un faux service` avec  { provide: ..., useValue: ...}  
on dit à Angular : “pour ce token, utilise cet objet-là au lieu de créer une vraie instance”

</details>

<details>
<summary> HTTP Testing </summary>

Angular fournit `HttpClientTestingModule` et `HttpTestingController` pour intercepter et simuler les requêtesHTTP

```typescript
describe('DataService', () => {
  let service: DataService;
  let testingController: HttpTestingController;
  beforeEach(() => {
    // remplace HttpClientModule par une version fictive
    TestBed.configureTestingModule({imports: [HttpClientTestingModule] });
    service = TestBed.inject(DataService);
    testingController = TestBed.inject(HttpTestingController);
  });
  // pas de requêtes HTTP non gérées
  afterEach(() => { testingController.verify(); });
  ////----- Test d'une requête GET (liste) -----
  it('should get all users', () => {
    // 1. Appeler la méthode — elle déclenche un GET mais il est intercepté
    service.getAllUsers().subscribe((users: any) => {
      // 4. Ces assertions s'exécutent APRÈS le flush()
      expect(users).toBeTruthy();
      expect(users.length).toBe(6);
      const secondUser = users.find((user: any) => user.id === 2);
      expect(secondUser.name).toBe('Elena');
    });
    // 2. Capturer la requête interceptée
    const mockReq = testingController.expectOne('api/users');
    // 3. Vérifier la méthode HTTP
    expect(mockReq.request.method).toEqual('GET');
    // 4. Simuler la réponse du serveur → déclenche le subscribe()
    mockReq.flush(Object.values(USERS));
  });
});
```
</details>


<details>
<summary> Opérations Asynchrones (Promise et Observable) </summary>

Certaines observables sont `synchrones` (elles émettent immédiatement leurs valeurs sans délai). Dans ce cas, un test normal suffit  
Quand une observable utilise `delay`, elle devient `asynchrone`. Il faut `fakeAsync` et `tick` :

```typescript
it('should test an asynchronous observable', fakeAsync(() => {
  let result = false;
  const myObs = of(true).pipe(delay(1000)); // émet après 1 seconde
  myObs.subscribe((val) => {result = val;  });
  expect(result).toBeFalse(); // pas encore émis
  tick(1000); // avancer l'horloge de 1s
  expect(result).toBeTrue(); // maintenant émis
}));
```

`Tester une Promise`

```typescript
it('should test a promise', fakeAsync(() => {
  let resolved = false;
  Promise.resolve(true).then((val) => {resolved = val;}});
  expect(resolved).toBeFalse(); // pas encore résolu
  flushMicrotasks(); // résoudre la Promise
  expect(resolved).toBeTrue();
}));
```

`waitForAsync` — alternative à `fakeAsync`est utilisé quand `fakeAsync` ne peut pas être utilisé (ex: code réellement asynchrone avec `XMLHttpRequest`). Il indique à Jasmine d'attendre que toutes les tâches asynchrones soient terminées :

```typescript
it('should wait for async operations', waitForAsync(() => {
  // Code exécuté quand toutes les opérations asynchrones sont terminées
  fixture.whenStable().then(() => {expect(component).toBeTruthy();});
}));
```
</details>


<details>
<summary> Component Testing & DOM Interaction </summary>

`ComponentFixture<T>`: un wrapper autour du composant qui donne accès à son instance, son DOM, et permet de déclencher la détection de changements  
`fixture.componentInstance`: l'instance TypeScript du composant  
`fixture.debugElement`: accès au DOM du composant déclencher des events  
`fixture.detectChanges()`: déclenche manuellement la détection de changements  

``` typescript
describe('AppComponent', () => {
  let component: AppComponent;
  let fixture: ComponentFixture<AppComponent>;
  let el: DebugElement;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [AppComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(AppComponent);
    component = fixture.componentInstance;
    el = fixture.debugElement;
    fixture.detectChanges();
  });

  it('should render a button with text subscribe and disable it after click', () => {
    // État initial
    component.isSubscribed = false;
    fixture.detectChanges(); // re-rendre le template
    let btnElements = el.queryAll(By.css('.subscribe'));
    // Vérifier l'état initial
    expect(btnElements[0].nativeElement.textContent).toBe('Subscribe');
    expect(btnElements[0].nativeElement.disabled).toBeFalse();
    // Simuler un clic
    btnElements[0].nativeElement.click();
    fixture.detectChanges(); // Angular doit redessiner le DOM après le clic
    // Après le clic, *ngIf bascule → nouveau DOM
    btnElements = el.queryAll(By.css('.subscribe'));
    expect(btnElements[0].nativeElement.textContent).toBe('Subscribed');
    expect(btnElements[0].nativeElement.disabled).toBeTrue();
  });
});
```
</details>

<details>
<summary> Pipe & Directive Testing</summary>

🔹Un `pipe` transforme des données dans les templates Angular.
```typescript
@Pipe({ name: 'grade' })
export class GradePipe implements PipeTransform {
  transform(value: number): string { /**/ }
}
```
Tester un `pipe` est très simple: c'est une `classe pure` avec une méthode `transform` (pas besoin de `TestBed`)    
```typescript
describe('GradePipe', () => {
  let pipe: GradePipe;
  beforeEach(() => {pipe = new GradePipe();});
  it('should...', () => {/**/});
});
```
🔹Une `directive` modifie le comportement ou l'apparence d'un élément du DOM. ils interagissent avec le DOM et nécessitent donc un composant hôte pour être testées  
```typescript
@Directive({ selector: '[appGrade]' })
export class GradeDirective {
  @Input() mark = 0;
  constructor(private el: ElementRef) {}
  @HostListener('mouseenter') onMouseEnter() {/**/}
}
```
Pour tester une directive, il faut un `composant hôte` qui l'utilise. 

```typescript
describe('GradeDirective', () => {
  let component: AppComponent;
  let fixture: ComponentFixture<AppComponent>;
  let el: DebugElement;
  beforeEach(waitForAsync(() => {
    // AppComponent importe GradeDirective
    TestBed.configureTestingModule({imports: [AppComponent]});
    fixture = TestBed.createComponent(AppComponent);
    component = fixture.componentInstance;
    el = fixture.debugElement;
    fixture.detectChanges();
  }));

  it('should create an instance', () => {
    const mockElRef = { nativeElement: document.createElement('div') };
    const directive = new GradeDirective(mockElRef);
    expect(directive).toBeTruthy();
  });

  it('should change text color on mouse enter', () => {
    const divs = el.queryAll(By.css('div'));
    divs[0].triggerEventHandler('mouseenter', {});
    fixture.detectChanges();
    expect(divs[0].nativeElement.style.color).toBe('green');
  });
});
```
</details>


<details>
<summary> Routing & Code Coverage</summary>

🔹Routing

`RouterTestingModule.withRoutes(routes)`→ simuler le Router avec nos routes  
`router.initialNavigation()`→ déclencher la navigation initiale (route par défaut)  
`Location`→ vérifier l'URL active avec `location.pathname`  
`fixture.whenStable()`→ attendre que la navigation (async) soit terminée  

```typescript
  beforeEach(waitForAsync(() => {
    TestBed.configureTestingModule({
      // simuler le Router avec nos routes
      imports: [RouterTestingModule.withRoutes(routes)],
    }).compileComponents();
  }));

it('should redirect to /home by default', waitForAsync(() => {
  fixture.detectChanges();
  fixture.whenStable().then(() => {
    expect(location.pathname).toBe('/home');
  });
}));
```

Tout composant utilisant `routerLink` doit importer `RouterTestingModule` dans son spec.

---

🔹Code Coverage

```bash
ng test --no-watch --code-coverage
```
Génère un rapport dans le dossier `coverage/` (HTML interactif)  
4 métriques : **Statements**, **Branches**, **Functions**, **Lines**  

| Métrique | Ce qu'elle mesure |
|----------|------------------|
| Statements | Déclarations exécutées |
| Branches | Branches if/else/switch couvertes |
| Functions | Fonctions appelées |
| Lines | Lignes de code exécutées |


</details>


<br>


<br><br>

</details>

<details>

<summary> ✅ React </summary>

<details>


<summary> React, composants, Virtual DOM, SPA </summary>

`React` est une bibliothèque JavaScript déclarative, basée sur des composants, pilotée par l’état, pour construire des interfaces utilisateur.

Les `composants` sont les briques de construction reutilisable, techniquement un composant et fonctions JavaScript qui retournent du JSX

`Problème des Applications Vanilla JavaScript`:
Le défi principal : Garder l’interface utilisateur (UI) synchronisée avec les données (state), les probleme concrets sont (Manipulation DOM directe, Synchronisation manuelle).
avec react quand le state change → React re-rend automatiquement

Le `Virtual DOM` en React, c’est une copie légère du DOM en mémoire.
Quand il y a un changement, React compare l’ancienne version avec la nouvelle, et met à jour uniquement ce qui a changé dans le vrai DOM.
Du coup, ça améliore les performances.

`SPA`: Une seule page HTML, JavaScript gère tout le routing et le rendu, Navigation instantanée (pas de rechargement), Sensation d’app native

📊 **CSR vs SSR : Comparaison**

| Critère | CSR (SPA) | SSR (Traditionnel) |
|---------|-----------|-------------------|
| **Performance initiale** | ❌ Plus lent (gros bundle JS) | ✅ Rapide (HTML prêt) |
| **Navigation** | ✅ Instantanée | ❌ Rechargement complet |
| **Interactivité** | ✅ Très interactive | ❌ Limitée |
| **SEO** | ❌ Peut être problématique | ✅ Excellent |
| **Complexité** | ⚠️ Plus complexe | ✅ Plus simple |
| **Use cases** | Apps internes, dashboards | Sites e-commerce, blogs |

**cycle de vie d'un composant**

```
1. MOUNT (Naissance) 🐣
   ↓
   - Composant créé
   - State et props initialisés
   - Première apparition dans le DOM
2. RE-RENDER (Vie) 🐓
   ↓
   - Déclenché par:
     • State change
     • Props change
     • Parent re-render
     • Context change
3. UNMOUNT (Mort) 💀
   ↓
   - Composant retiré du DOM
   - State et props détruits
   - Nettoyage nécessaire
```

</details>

<details>


<summary> props, state, prop drilling,  </summary>

🔄 **Props vs State**

| Props | State |
|-------|-------|
| **Propriété EXTERNE** | **Donnée INTERNE** |
| Vient du **parent** | Appartient au **composant** |
| **READ-ONLY** (immutable) | **MUTABLE** (via setState) |
| Comme des **paramètres de fonction** | Comme des **variables locales** |
| Reçu de l'extérieur | Créé dans le composant |
| Pour **configurer** le composant | Pour rendre le composant **interactif** |

le `children` prop special permet de passer du contenu entre les balises d’un composant.

`COMPONENT COMPOSITION`: Assembler des composants simples pour créer des composants complexes

Le `prop drilling` en React, c’est quand on passe des données de parent à enfant sur plusieurs niveaux. donc la solution est la composition (passer le composant directement) ou  Context API 

les props sont-elles immutables pour (Pureté, Prédictibilité(pas de sideEffect), Débogage)

un `side effect`, c’est tout ce qui n’est pas directement lié au rendu du composant, comme appeler une API, manipuler le DOM ou lancer un timer. (On les gère généralement avec le hook useEffect).

</details>


<details>


<summary> state & events </summary>

le `state`, c’est une donnée interne au composant qui peut changer dans le temps.
Quand le state change, React re-render le composant automatiquement pour mettre à jour l’interface

```jsx
//LAZY INITIALIZATION  Utile si le calcul initial est coûteux
const [count, setCount] = useState(() => {
  const savedCount = localStorage.getItem('count');
  return savedCount ? JSON.parse(savedCount) : 0;
}); // La fonction ne s'exécute qu'une seule fois (au mount)
```

**STATE AVEC OBJETS ET TABLEAUX**

***Updating Objects (Immutabilité)***

```jsx
function UserProfile() {
  const [user, setUser] = useState({
    name: "Alice",
    email: "alice@example.com",
    age: 25,
    address: {city: "Paris", country: "France"}
  });
  
  // ❌ MAUVAIS: Mutation directe
  const updateNameBad = () => {
    user.name = "Bob";  // ❌ Mutation!
    setUser(user);      // React ne détectera pas le changement
  };
  
  // ✅ BON: Créer un nouvel objet
  const updateName = () => {
    setUser({
      ...user,           // Copie toutes les propriétés
      name: "Bob"        // Override la propriété name
    });
  };

  // ✅ Multiple properties update
  const updateProfile = () => {
    setUser(prev => ({
      ...prev,
      name: "Charlie",
      age: prev.age + 1
    }));
  };
  //...

 }
```
***Updating Arrays (Immutabilité)***

```jsx
function TodoList() {
  const [todos, setTodos] = useState([
    { id: 1, text: "Learn React", done: false },
    { id: 2, text: "Build project", done: false }
  ]);
  
  // ✅ AJOUTER un élément
  const addTodo = (text) => {
    const newTodo = {id: Date.now(), text: text, done: false};
    // Méthode 1: Spread operator
    setTodos([...todos, newTodo]);
    // Méthode 2: concat
    setTodos(todos.concat(newTodo));
    // ❌ MAUVAIS: push (mutation)
    // todos.push(newTodo);  // ❌
    // setTodos(todos);      // ❌
  };
  // ✅ SUPPRIMER un élément
  const deleteTodo = (id) => {
    setTodos(todos.filter(todo => todo.id !== id));
  };
  // ✅ MODIFIER un élément
  const toggleTodo = (id) => {
    setTodos(todos.map(todo =>
      todo.id === id 
        ? { ...todo, done: !todo.done }
        : todo
    ));
  };
```

le `derived state`, c’est un state calculé à partir des props ou d’un autre state.`
==> ⚡ Performance : useMemo pour Derived State Coûteux

```tsx
function ExpensiveComponent({ data }) {
  // ❌ Sans useMemo: Recalculé à CHAQUE render (même si data ne change pas)
  const expensiveCalculation = computeExpensiveValue(data);
  
  // ✅ Avec useMemo: Recalculé SEULEMENT si data change
  const memoizedValue = useMemo(() => {
    return computeExpensiveValue(data);
  }, [data]);
  
  return <div>{memoizedValue}</div>;
}
```

</details>


<details>


<summary> useReducer</summary>


**UseReducer**

`useReducer` est un hook pour gérer des états complexes avec une logique centralisée.
On dispatch des actions, et un reducer décide comment mettre à jour le state.
C’est utile quand le state devient difficile à gérer avec useState

```tsx
// useState (simple)
const [count, setCount] = useState(0);
// useReducer (complexe)
function reducer(state, action) { // return state
	switch(action.type){/*..*/}
} 
/* useReducer */
const [state, dispatch] = useReducer(reducer, initialState);
/* dispatch */
<button onClick={() => dispatch({ type: 'increment', payload: {}  })}>

```

```tsx
import { useReducer } from 'react';

// 1️⃣ INITIAL STATE
const initialState = {count: 0, step: 1, status: 'idle'};

// 2️⃣ REDUCER FUNCTION
function reducer(state, action) {
  // Pure function: state + action → new state
  switch (action.type) {
    case 'increment':
      return {...state, count: state.count + state.step};
    case 'decrement':
      return {...state, count: state.count - state.step};
    case 'setStep':
      return {...state, step: action.payload};
    case 'reset':
      return initialState;
    case 'start':
      return {...state, status: 'counting'};
    case 'pause':
      return {...state, status: 'paused'};
    default:
      throw new Error(`Unknown action: ${action.type}`);
  }
}

// 3️⃣ COMPONENT
function Counter() {
  const [state, dispatch] = useReducer(reducer, initialState);
  
  return (
    <div>
      <p>Count: {state.count}</p>
      <p>Step: {state.step}</p>
      <p>Status: {state.status}</p>
      
      <button onClick={() => dispatch({ type: 'increment' })}>   +
      </button>
      <button onClick={() => dispatch({ type: 'decrement' })}>
        - </button>
      
      <input
        type="number"
        value={state.step}
        onChange={(e) => dispatch({ 
          type: 'setStep', 
          payload: Number(e.target.value) 
        })}
      />
      
      <button onClick={() => dispatch({ type: 'start' })}>
        Start </button>
      <button onClick={() => dispatch({ type: 'pause' })}>
        Pause </button>
      <button onClick={() => dispatch({ type: 'reset' })}>
        Reset </button>
    </div>
  );

```

***useState vs useReducer : Comparaison***

| Critère | useState | useReducer |
|---------|----------|------------|
| **Use case** | 1-2 state variables simples | État complexe avec plusieurs variables liées |
| **Logique** | Dans event handlers (dispersée) | Centralisée dans reducer |
| **Updates** | Appeler setState | Dispatcher des actions |
| **Complexity** | ✅ Simple à comprendre | ⚠️ Plus complexe (boilerplate) |
| **Testability** | ⚠️ Difficile (couplé au composant) | ✅ Facile (reducer = pure function) |
| **Debug** | ⚠️ State updates partout | ✅ Toutes les transitions dans reducer |
| **Type safety** | ⚠️ Moins évident | ✅ Actions typées (avec TypeScript) |

---

</details>

<details>


<summary> Context API</summary>

**CONTEXT API : GLOBAL STATE MANAGEMEN**

le `Context API` permet de partager des données globales entre plusieurs composants sans passer par les props à chaque niveau.
C’est souvent utilisé pour éviter le prop drilling, par exemple pour le thème ou l’utilisateur connecté.

```jsx
// createContext
const UserContext = createContext();

// create Provider
function UserProvider({ children }) {
  const [user, setUser] = useState(null);
  
  return (
    <UserContext.Provider value={{ user, setUser }}>
      {children}
    </UserContext.Provider>
  );
}

// 3️⃣ CRÉER UN CUSTOM HOOK (optionnel mais recommandé)
function useUser() {
  const context = useContext(UserContext);
  if (context === undefined)
    throw new Error('useUser must be used within UserProvider');
  return context;
}
// WRAPPER L'APP
function App() {
  return (
    <UserProvider>
      <Dashboard />
    </UserProvider>
  );
}
// utlisation dans un composant n-fils 

function UserProfile() {
  // ✅ Accès direct au context!
  const { user, setUser } = useUser();
 // ... 
}
```

***Decision Matrix : Quel Tool Utiliser ?***
| Type de State | Scope | Outils Recommandés | Use Cases |
|---------------|-------|-------------------|-----------|
| **UI Local** | 1 composant | `useState` | Toggle, form input, modal |
| **UI Local Complexe** | 1 composant | `useReducer` | Form complexe, wizard |
| **UI Shared** | Siblings | Lift state + `useState` | Filter + List |
| **UI Global** | Toute l'app | Context API | Theme, auth status, language |
| **UI Global Complexe** | Toute l'app | Redux, Zustand | Cart, complex UI state |
| **Remote Data** | Toute l'app | React Query, SWR | API data, cache |
| **URL State** | Navigation | React Router | Search, filters, pagination |
| **Browser Storage** | Persistence | localStorage + custom hook | User preferences |

</details>


<details>


<summary> Redux & Redux Toolkit </summary>

`Redux`, c’est une lib pour gérer le state global dans une app React.
On a un store central, et on met à jour le state avec des actions

```tsx
//##########  Configuration du Store ########## 
const store = configureStore({
  reducer: {
    account: accountReducer,
    cart: cartReducer,
    user: userReducer,
  },
  // Middleware et DevTools sont auto-configurés !
});
export default store;

//########## Provider dans l’App (main.jsx ou index.js) ########## 
ReactDOM.createRoot(document.getElementById('root')).render(
  <React.StrictMode>
    <Provider store={store}>
      <App />
    </Provider>
  </React.StrictMode>
);

//########## // cartSlice.js ########## 
const cartSlice = createSlice({
  // 1. NOM (préfixe pour action types)
  name: 'cart',
  // 2. STATE INITIAL
  initialState: {/*...*/},
  // 3. REDUCERS (actions synchrones)
  reducers: { // Reducer simple
    addItem(state, action) {...},
    //...
   }
});

// Export actions (auto-générées)
export const { 
  addItem, removeItem, addItemWithId, clearCart, increaseQuantity 
} = cartSlice.actions;
// Export reducer
export default cartSlice.reducer;

//########## UTILISATION DANS LES COMPONENTS ########## 
// cart.js
function Cart() {
  // ✅ Selector simple
  const items = useSelector((state) => state.cart.items);
  // ✅ Selector avec calcul
  const total = useSelector((state) => 
    state.cart.items.reduce((sum, item) => sum + item.price, 0)
  );
  // ...
 }

```

***ASYNC OPERATIONS : createAsyncThunk***
Un `thunk`est une fonction qui retourne une fonction (pour retarder l’exécution).

**Tableau Comparatif**
| Critère | Redux Toolkit | Context API |
|---------|---------------|-------------|
| **Use case** | State global complexe | State simple (theme, auth) |
| **Performance** | ✅ Optimisé (granular subscriptions) | ⚠️ Re-renders fréquents |
| **DevTools** | ✅ Redux DevTools (time-travel) | ❌ Limité |
| **Async logic** | ✅ createAsyncThunk intégré | ⚠️ useEffect + useState |
| **Middleware** | ✅ Support natif | ❌ Difficile |
| **Learning curve** | ⚠️ Plus complexe | ✅ Plus simple |
| **Boilerplate** | ⚠️ Plus de setup initial | ✅ Minimal |
| **Debugging** | ✅ Excellent | ⚠️ Console.log |
| **Testing** | ✅ Pure functions faciles à tester | ⚠️ Plus complexe |

</details>


<details>


<summary> useEffect, useRef, custom hook</summary>

**useEffect**

`useEffect` est un hook qui gére les side effects, comme appeler une API, localStorage ou ajouter un event listener

```tsx
//##### Sans Dependency Array → Exécute à CHAQUE render
  // ❌ DANGEREUX : Exécute à chaque render
  useEffect(() => {
    console.log('Je m\'exécute à chaque render');
  }); // Pas de []

//##### Avec [] vide → Exécute UNE SEULE FOIS (au mount)
  // ✅ Exécute seulement au premier render
  // Use cases: Fetch initial data, Setup subscriptions, Add event listeners, Initialize third-party libs
  useEffect(() => {
    console.log('Component mounted');
    fetch('/api/data').then(res => res.json()).then(setData);
  }, []); // Array vide

//##### Avec [dependencies] → Exécute quand les deps changent
function SearchResults({ query }) {
  const [results, setResults] = useState([]);
  // ✅ Exécute quand query change
  useEffect(() => {
    console.log(`Searching for: ${query}`);
    fetch(`/api/search?q=${query}`).then(res => res.json())
      .then(setResults);
  }, [query]); // Re-exécute si query change
  
  return (
    <ul>
      {results.map(item => <li key={item.id}>{item.name}</li>)}
    </ul>
  );
}
```
***Cleanup = Fonction retournée par useEffect pour “nettoyer***

```tsx
// ✅ Intervals/Timeouts
useEffect(() => {
  const id = setInterval(() => console.log('tick'), 1000);
  return () => clearInterval(id); // OBLIGATOIRE
}, []);


// ✅ Event Listeners
useEffect(() => {
  const handler = () => console.log('clicked');
  document.addEventListener('click', handler);
  return () => document.removeEventListener('click', handler); // OBLIGATOIRE
}, []);
```

```tsx
// ❌ MAUVAIS : useEffect ne peut pas être async
useEffect(async () => {
  const data = await fetch('/api/data');
  setData(data);
}, []);
// Erreur : useEffect doit retourner une fonction ou rien,
// pas une Promise (ce que fait async)

// ✅ SOLUTION : Fonction async à l'intérieur
useEffect(() => {
  async function fetchData() {
    const res = await fetch('/api/data');
    const json = await res.json();
    setData(json);
  }
  fetchData();
}, []);
```

**useRef**

`useRef` en React permet de persister une valeur entre les renders sans provoquer de re-render

***Différences avec useState :***

| | useState | useRef |
|---|---|---|
| **Change trigger re-render ?** | ✅ Oui | ❌ Non |
| **Valeur persiste ?** | ✅ Oui | ✅ Oui |
| **Immuable ?** | ✅ Oui | ❌ Non (mutable) |
| **Usage** | UI data | DOM refs, values |


**Custom Hook**

Un `custom hook` en React, c’est un hook qu’on crée soi-même pour réutiliser de la logique.

Règles :
	✅ Nom commence par use (convention)
	✅ Peut utiliser d’autres Hooks
	✅ Retourne ce qu’on veut (value, array, object)

`Exemple`

```tsx
function useLocalStorage(key, initialValue) {
  // Lazy initialization
  const [storedValue, setStoredValue] = useState(() => {
    try {
      const item = window.localStorage.getItem(key);
      return item ? JSON.parse(item) : initialValue;
    } catch (error) {
      console.error(error);
      return initialValue;
    }
  });
  
  // Wrapper de setValue qui sync avec localStorage
  const setValue = (value) => {
    try {
      const valueToStore = value instanceof Function ? value(storedValue) : value;
      setStoredValue(valueToStore);
      window.localStorage.setItem(key, JSON.stringify(valueToStore));
    } catch (error) {
      console.error(error);
    }
  };
  
  return [storedValue, setValue];
}

// Usage
function App() {
  const [theme, setTheme] = useLocalStorage('theme', 'dark');
  const [user, setUser] = useLocalStorage('user', null);
  
  return (
    <div className={theme}>
      <button onClick={() => setTheme(theme === 'dark' ? 'light' : 'dark')}>
        Toggle Theme
      </button>
      {user && <p>Welcome {user.name}</p>}
    </div>
  );
}
```

**useLayoutEffect**

***useEffect :***
	✅ Exécute après le paint (asynchrone)  
	✅ N’bloque pas le navigateur  
	✅ Utilisé pour 99% des cas  

***useLayoutEffect :***
	✅ Exécute avant le paint (synchrone)   
	⚠️ Bloque le navigateur  
	✅ Utilisé pour mesures DOM ou mutations synchrones  


</details>

<details>


<summary> Hooks de Performance(React.memo, useMemo, useCallback)</summary>

**Optimization**

React re-render un component quand :  
	1. Son state change  
	2. Ses props changent  
	3. Son parent re-render  

***Les 3 Techniques d'Optimisation***

| Technique | Usage | Quand utiliser |
|-----------|-------|----------------|
| **React.memo** | Éviter re-renders inutiles de components | Component pur avec props stables |
| **useMemo** | Mémoïser des **valeurs** calculées | Calculs coûteux |
| **useCallback** | Mémoïser des **fonctions** | Fonctions passées comme props |


**React.memo**

`React.memo` est utilisé pour optimiser les performances en évitant les re-renders inutiles d’un composant si ses props n’ont pas changé.

```tsx
// ❌ SANS memo : Re-render à chaque fois
function Child({ name }) {
  console.log('Child rendered');
  return <div>Hello {name}</div>;
}

function Parent() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(count + 1)}>Count: {count}</button>
      <Child name="John" />
    </div>
  );
}
// Console : Child rendered (à chaque click sur le bouton)

// ✅ AVEC memo : Re-render seulement si name change
const Child = React.memo(function Child({ name }) {
  console.log('Child rendered');
  return <div>Hello {name}</div>;
});
```

**useMemo**

`useMemo` en React permet de mémoriser une valeur pour éviter de la recalculer à chaque render.

```tsx
// ❌ SANS useMemo : Calcul à chaque render
function Component({ items }) {
  const total = items.reduce((sum, item) => sum + item.price, 0);
  // Recalcule même si items n'a pas changé !
  
  return <div>Total: ${total}</div>;
}

// ✅ AVEC useMemo : Calcul seulement si items change
function Component({ items }) {
  const total = useMemo(
    () => items.reduce((sum, item) => sum + item.price, 0),
    [items]
  );
  
  return <div>Total: ${total}</div>;
}
```

**useCallback**

`useCallback` en React permet de mémoriser une fonction pour éviter qu’elle soit recréée à chaque render.

```tsx
// ✅ AVEC useCallback : Fonction stable
function Parent() {
  const [count, setCount] = useState(0);
  
  const handleClick = useCallback(() => {
    console.log('Clicked');
  }, []); // Même fonction à chaque render
  
  return (
    <div>
      <button onClick={() => setCount(count + 1)}>Count: {count}</button>
      <Child onClick={handleClick} />
    </div>
  );
}
```

**📊 COMPARATIF : memo vs useMemo vs useCallback**

| | React.memo | useMemo | useCallback |
|---|---|---|---|
| **Type** | HOC | Hook | Hook |
| **Mémoïse** | Component entier | Valeur calculée | Fonction |
| **Retourne** | Component mémoïsé | Valeur | Fonction |
| **Usage** | `React.memo(Component)` | `useMemo(() => value, [deps])` | `useCallback(() => {}, [deps])` |
| **Quand** | Éviter re-renders | Calculs coûteux | Fonctions comme props |


</details>


<details>


<summary> React Router </summary>

**React Router**

`React Router` est une librairie qui permet de gérer la navigation entre différentes pages dans une application React (SPA)

```tsx
import { BrowserRouter, Routes, Route } from "react-router-dom";

function App() {
  return (
    <BrowserRouter>
      <Routes>
        <Route path="/" element={<Home />} />
        <Route path="/about" element={<About />} />
          <Route path="/products/:id" element={<ProductDetail />} />
      </Routes>
    </BrowserRouter>
  );
}

//---------------------------------------
// Accéder aux params
import { useParams } from 'react-router-dom';

function ProductDetail() {
  const { id } = useParams();
  // URL: /products/123 → id = "123"
  return <div>Product ID: {id}</div>;
}

//---------------------------------------
// link component & NavLink (avec active state)
function Navigation() {
  return (
    <nav>
      <Link to="/">Home</Link>
      <Link to="/about">About</Link>
      <Link to="/products/123">Product 123</Link>
      <NavLink
        to="/"
        className={({ isActive }) => isActive ? 'active' : ''}
      > About </NavLink>
 	</nav>
   )
}

//---------------------------------------
// Programmatic Navigation (useNavigate)
function LoginForm() {
  const navigate = useNavigate();
  
  const handleSubmit = async (e) => {
    e.preventDefault();
    
    const success = await login(credentials);
    
    if (success) {
      // Naviguer vers /dashboard
      navigate('/dashboard');
      
      // Naviguer en arrière
      // navigate(-1);
      
      // Naviguer avec replace (pas d'historique)
      // navigate('/dashboard', { replace: true });
      
      // Naviguer avec state
      // navigate('/dashboard', { state: { from: 'login' } });
    }
  };
  
  return <form onSubmit={handleSubmit}>...</form>;
}

//---------------------------------------
// useSearchParams (Query Strings)
function ProductsList() {
  const [searchParams, setSearchParams] = useSearchParams();
  
  // URL: /products?category=electronics&sort=price
  const category = searchParams.get('category'); // "electronics"
  const sort = searchParams.get('sort'); // "price"
  
  const handleFilter = (newCategory) => {
    setSearchParams({ category: newCategory, sort });
    // URL devient: /products?category=newCategory&sort=price
  };
  
  return (
    <div>
      <p>Category: {category}</p>
      <p>Sort: {sort}</p>
      
      <button onClick={() => handleFilter('books')}>
        Show Books
      </button>
      
      <button onClick={() => setSearchParams({ sort: 'name' })}>
        Sort by Name
      </button>
    </div>
  );
}

//---------------------------------------
// useLocation
function CurrentPage() {
  const location = useLocation();
  
  console.log(location.pathname); // "/products/123"
  console.log(location.search); // "?sort=price"
  console.log(location.hash); // "#section1"
  console.log(location.state); // { from: 'homepage' }
  
  return (
    <div>
      <p>Current path: {location.pathname}</p>
    </div>
  );
}
```

</details>


<details>


<summary> Data Fetching & React Query</summary>

**React Query**
`React Query` permet de gérer le fetching, le caching et la synchronisation des données côté serveur.


`useQuery` → fetch + cache automatique
`useMutation` → actions avec optimistic update
`queryClient` → gestion du cache et synchronisation

`staleTime` vs `cacheTime` :
staleTime définit combien de temps la data est considérée fraîche (pas de refetch),
cacheTime définit combien de temps elle reste en mémoire après usage.


</details>


<details>


<summary> NextJs </summary>

`Next.js` est un framework React full-stack qui permet de faire du SSR, SSG, ISR, du routing basé sur les fichiers et même des API côté serveur.

| Problème (React)                     | Solution (Next.js)                  |
|-------------------------------------|------------------------------------|
| Pas de routing intégré              | Routing basé sur les fichiers       |
| Pas de SSR (SEO limité)             | SSR / SSG / ISR intégrés            |
| Bundle chargé d’un coup             | Code splitting automatique          |
| Pas de backend intégré              | API Routes intégrées                |
| Optimisation manuelle (images/fonts)| Optimisation automatique            |
| Déploiement complexe                | Déploiement simple (Vercel)         |


**Création d’un Projet Next.js**

```shell
# Créer nouveau projet
npx create-next-app@latest my-app

# Options pendant l'installation :
#  ✅ TypeScript? → No (pour ce cours)
#  ✅ ESLint? → Yes
#  ✅ Tailwind CSS? → Yes
#  ✅ src/ directory? → No
#  ✅ App Router? → Yes (IMPORTANT !)
#  ✅ Import alias? → Yes (@/*)

cd my-app
npm run dev
# App disponible sur http://localhost:3000
```

<br>

**🟢 Quand utiliser React**

* Applications **SPA (Single Page Application)**
* **Dashboards internes** (admin, back-office)
* Projets **sans besoin SEO**
* Applications fortement **interactives côté client**
* Quand tu veux une **librairie légère et flexible**
* Architecture déjà gérée (backend séparé, API existantes)

**🔵 Quand utiliser Next.js**

* Applications **SEO-friendly** (site public, blog, e-commerce)
* Besoin de **SSR / SSG / ISR**
* Projets nécessitant de **bonnes performances initiales**
* Applications **full-stack** (API intégrées)
* Besoin de **routing automatique**
* Optimisations intégrées (images, fonts, code splitting)

`SSR (Server-Side Rendering)`: la page est générée à chaque requête côté serveur → toujours à jour  
`SSG (Static Site Generation)`: la page est générée une fois au build → très rapide  
`ISR (Incremental Static Regeneration)`: c’est du SSG avec mise à jour automatique après un certain temps  

</details>


<details>


<summary> Server Component, Client Components, server action </summary>

**Server Components**

Les `Server Components` en React sont des composants qui s’exécutent côté serveur.

✅ Avantages des Server Components (React / Next.js)
* 🚀 Réduction du JavaScript côté client (bundle plus léger)
* ⚡ Meilleures performances (moins de code à exécuter dans le navigateur)
* 🔒 Accès direct aux ressources serveur (DB, filesystem, variables d’environnement)
* 🌐 Pas besoin d’appels API côté client (moins de latence)
* 📦 Meilleure gestion du SEO (rendu côté serveur)

❌ Limites des Server Components
* 🚫 Pas d’interactivité (pas de gestion d’événements comme onClick)
* 🚫 Pas de hooks client (useState, useEffect, etc.)
* 🚫 Pas d’accès aux APIs navigateur (window, localStorage…)
* 🔄 Nécessite des Client Components pour les parties dynamiques
* ⚠️ Debugging parfois plus complexe

==> ❌ IMPOSSIBLE - Import direct SC dans CC

<br>

**Client Components**

Les `Client Components` en React sont des composants qui s’exécutent dans le navigateur

<br>

**Server actions**

Les `Server Actions` sont des fonctions marquées avec ‘use server’ qui s’exécutent côté serveur et permettent de gérer des mutations sans passer par des API routes.

“En React et Next.js, ces hooks sont surtout utilisés avec les Server Actions :

`useFormState`: permet de gérer l’état d’un formulaire côté serveur (résultat, erreurs…)
`useFormStatus`: permet de savoir si un formulaire est en cours de soumission (loading, disabled…)
`useTransition`: permet de gérer des mises à jour non bloquantes pour garder une UI fluide
`useOptimistic`: permet de mettre à jour l’UI immédiatement avant la réponse du serveur (optimistic UI)


</details>


<details>


<summary> Solutions de Styling </summary>

Les 3 Grandes Approches

| Approche | Exemple | Philosophie |
|----------|---------|-------------|
| **CSS-in-CSS** | CSS Modules | CSS traditionnel avec scoping |
| **CSS-in-JS** | Styled Components | CSS dans JavaScript |
| **Utility-first** | Tailwind CSS | Classes utilitaires composables |

</details>

<br><br>

</details>

<br>

<details>

<summary> ✅ Docker </summary>

<details>

<summary> <em><b>Docker</b></em> vs <em><b>VM</b></em> ?</summary>

> Une VM embarque un système d’exploitation complet, donc elle est plus lourde.
>
> Docker partage le noyau du système hôte et embarque seulement l’application avec ses dépendances.
>
> Donc un conteneur Docker est généralement plus léger, plus rapide à démarrer et plus simple à déployer dans différents environnements.

</details>

<details>
<summary> docker, imageDocker, DockerFile </summary>

`Docker` permet de containeriser une application pour qu’elle fonctionne de manière stable dans différents environnements.  
Une `image Docker`, c’est le modèle qui contient l’application et ses dépendances.  
Un `Dockerfile`, c’est le fichier qui permet de construire cette image Docker.  

</details>



<details>
<summary> Instructions essentielles </summary>

**`FROM` — Image de base**

```dockerfile
FROM eclipse-temurin:21-jre
# ou avec alias pour multi-stage
FROM eclipse-temurin:21-jdk AS builder
```

Toujours la première instruction. Définit l'image de base à partir de laquelle on construit.

**`WORKDIR` — Répertoire de travail**

```dockerfile
WORKDIR /app
```

Crée et positionne le répertoire courant pour les instructions suivantes (`RUN`, `COPY`, `CMD`...). Équivalent à un `cd` + `mkdir -p`.

**`COPY` — Copier des fichiers**

```dockerfile
# Copier un fichier spécifique
COPY target/my-app.jar app.jar

# Copier plusieurs fichiers
COPY src/ ./src/
COPY pom.xml .

# Avec .dockerignore, on évite de copier les fichiers inutiles
```

**`ADD` — Comme COPY mais avec plus de fonctionnalités**

```dockerfile
# ADD peut décompresser des archives et télécharger des URLs
ADD https://example.com/file.tar.gz /app/
```

> **Bonne pratique** : préférer `COPY` à `ADD` sauf besoin spécifique (décompression automatique).

**`RUN` — Exécuter des commandes lors du build**

```dockerfile
# Chaque RUN crée une nouvelle couche
RUN apt-get update && apt-get install -y curl \
    && rm -rf /var/lib/apt/lists/*

# Bonne pratique : chaîner les commandes avec && pour minimiser les couches
```

**`ENV` — Variables d'environnement**

```dockerfile
ENV JAVA_OPTS="-Xmx512m -Xms256m"
ENV SPRING_PROFILES_ACTIVE=production
ENV APP_PORT=8080
```

**`ARG` — Arguments de build (build-time only)**

```dockerfile
ARG JAR_FILE=target/*.jar
ARG BUILD_VERSION=latest

COPY ${JAR_FILE} app.jar
LABEL version=${BUILD_VERSION}
```

Différence avec `ENV` : `ARG` n'est disponible que pendant le `build`, pas à l'exécution du container.

**`EXPOSE` — Documenter le port**

```dockerfile
EXPOSE 8080
EXPOSE 8443
```

> Important : `EXPOSE` est **documentaire seulement**. Il ne publie pas réellement le port. C'est le `-p` lors du `docker run` qui fait la liaison.

**`VOLUME` — Déclarer des volumes**

```dockerfile
VOLUME /app/logs
VOLUME /app/data
```

Déclare un point de montage pour la persistance des données.

**`CMD` — Commande par défaut**

```dockerfile
# Forme exec (recommandée) - pas de shell intermédiaire
CMD ["java", "-jar", "app.jar"]

# Forme shell
CMD java -jar app.jar
```

**`ENTRYPOINT` — Point d'entrée fixe**

```dockerfile
ENTRYPOINT ["java", "-jar", "app.jar"]

# Avec CMD pour des arguments par défaut surchargeables
ENTRYPOINT ["java"]
CMD ["-jar", "app.jar"]
```

**Différence `CMD` vs `ENTRYPOINT`** :

| Aspect         | CMD                                      | ENTRYPOINT                              |
|---------------|------------------------------------------|-----------------------------------------|
| Surcharge      | Facilement surchargeable (`docker run`)  | Difficilement surchargeable (`--entrypoint`) |
| Usage          | Commande par défaut                      | Point d'entrée fixe                     |
| Combinaison    | Fournit les arguments à ENTRYPOINT       | Définit la commande principale  

</details>


<details>
<summary> commandes:  Gérer les containers </summary>

```bash
# Lister les containers en cours
docker ps

# Lister tous les containers (incluant stoppés)
docker ps -a

# Arrêter un container (signal SIGTERM puis SIGKILL après timeout)
docker stop my-spring-app

# Démarrer un container stoppé
docker start my-spring-app

# Redémarrer
docker restart my-spring-app

# Supprimer un container stoppé
docker rm my-spring-app

# Forcer la suppression (même en cours d'exécution)
docker rm -f my-spring-app

# Supprimer tous les containers stoppés
docker container prune
```

</details>


<details>
<summary> commandes: Interagir avec un container  </summary>

```bash
# Exécuter une commande dans un container en cours
docker exec -it my-spring-app bash
docker exec -it my-spring-app sh   # Pour Alpine Linux

# Voir les logs
docker logs my-spring-app
docker logs -f my-spring-app       # Follow (en temps réel)
docker logs --tail=100 my-spring-app  # 100 dernières lignes

# Inspecter les détails d'un container
docker inspect my-spring-app

# Statistiques de ressources en temps réel
docker stats my-spring-app
docker stats                        # Tous les containers

# Copier des fichiers depuis/vers un container
docker cp my-spring-app:/app/logs/app.log ./app.log
docker cp ./config.yml my-spring-app:/app/config/
```

</details>

<details>
<summary> commandes: Volumes </summary>

```bash
# Créer un volume
docker volume create pgdata

# Utiliser un volume
docker run -v pgdata:/var/lib/postgresql/data postgres:15

# Lister les volumes
docker volume ls

# Inspecter un volume
docker volume inspect pgdata

# Supprimer un volume
docker volume rm pgdata

# Supprimer les volumes inutilisés
docker volume prune
```

</details>

<br><br>

</details>

<details>

<summary> ✅ Kubernetes </summary>


<details>
<summary> Kubernetes & Ressources </summary>

> `Kubernetes`, c’est une plateforme d’orchestration qui permet de déployer, gérer et superviser des applications conteneurisées.


`Pod` : plus petite unité déployable dans Kubernetes, contient 1 ou plusieurs containers  
`ReplicaSet` : garantit qu'un nombre fixe de répliques de pods tourne en permanence  
`Deployments` : gère les ReplicaSets et fournit les rolling updates, le rollback et le scaling  
`StatefulSets` : utilisé pour les applications avec état nécessitant une identité réseau stable et un stockage persistant  
`DaemonSets` : garantit qu'un pod tourne sur chaque nœud du cluster  
`Jobs` : exécute une tâche jusqu'à la complétion et garantit qu'elle réussit  
`CronJob` : planifie et exécute des Jobs périodiquement selon une expression cron  

</details>

<details>
<summary> ConfigMaps, Secret, Vault  </summary>


**✅ConfigMaps** : utilisé pour stocker des données de configuration non sensibles dans Kubernetes  
Permet de séparer la config du code de l'application et peut être injecté en tant que variable d'env ou fichier  

**✅Secret** : utilisé pour stocker des informations sensibles comme des mots de passe ou des clés API dans Kubernetes.
Fonctionne de manière similaire à ConfigMap mais est destiné aux données confidentielles.

**🔐HashiCorp Vault** : système externe de gestion des secrets.
Fournit : Chiffrement, Secrets dynamiques, Rotation des secrets, Contrôle d'accès

**🔹 Pourquoi Vault est plus sécurisé ?**  
Vault peut :  
Générer des credentials de BDD temporaires  
Faire tourner automatiquement les mots de passe  
Chiffrer les secrets au repos  
Contrôler les accès via des politiques  

<details>

<summary>Comment une application récupère-t-elle un secret depuis Vault ?</summary>

D'abord, l'application doit s'authentifier auprès de Vault.

Ensuite, Vault vérifie les policies pour savoir si elle a le droit de lire le secret.

Puis l'application peut récupérer le secret en utilisant son chemin, via l'API Vault, le CLI, un agent Vault ou par exemple une intégration Kubernetes.

Dans Kubernetes, on peut aussi utiliser un agent ou un sidecar pour récupérer les secrets et les fournir à l'application.

</details>

</details>

<details>
<summary> Réseau (Services, Ingress, NetworkPolicy) </summary>


**✅Service** : expose des Pods à l'intérieur ou à l'extérieur du cluster   
`ClusterIP` : expose un service en interne dans le cluster Kubernetes, c'est le type de service par défaut   
Utilisé pour : communication entre microservices, APIs internes, bases de données  
`NodePort` : expose un service en externe en ouvrant un port sur chaque nœud du cluster    
Utilisé pour : tests, accès externe simple  
`LoadBalancer` : expose un service en externe en utilisant le load balancer d'un fournisseur cloud et assigne une IP publique     
Utilisé dans : AWS, Azure, GCP

**✅Ingress** : gère l'accès HTTP/HTTPS externe aux services et fournit le routage, la terminaison SSL  
Supporte : terminaison SSL 🔹 routage par chemin 🔹 routage par hôte   
Exemple : api.myapp.com → Service A 🔹 admin.myapp.com → Service B  

**✅NetworkPolicy** : définit des règles pour contrôler le trafic entre les pods dans un cluster Kubernetes, améliorant la sécurité et l'isolation  

</details>


<details>
<summary> Stockage (Volume, PV, PVC, StorageClass) </summary>


**✅Volume** : fournit du stockage aux containers dans un pod et permet le partage de données entre containers dans le même pod  
**✅PersistentVolume** : ressource de stockage du cluster qui existe indépendamment des pods, représente un vrai stockage (NFS, AWS EBS, ...)  
**✅PersistentVolumeClaim** : demande de stockage qui se lie à un PersistentVolume adapté   
**✅StorageClass** : définit comment le stockage est provisionné dynamiquement dans Kubernetes.   
Permet la création automatique de PersistentVolumes quand un PVC est demandé.

</details>

<details>
<summary> Sécurité (RBAC, Role, RoleBinding, ServiceAccount) </summary>


**✅RBAC** : système d'autorisation dans Kubernetes. Contrôle : qui peut accéder au cluster, quelles actions ils peuvent effectuer  
**✅Role** : définit les permissions pour les ressources dans un namespace spécifique  
**✅ClusterRole** : définit des permissions qui s'appliquent à l'échelle du cluster ou sur plusieurs namespaces  
**✅RoleBinding** : assigne un Role à un utilisateur, groupe ou compte de service dans un namespace  
**✅ClusterRoleBinding** : assigne un ClusterRole à un sujet au niveau du cluster  
**✅ServiceAccount** : fournit une identité aux pods pour interagir avec l'API Kubernetes en toute sécurité  

</details>

<details>
<summary> Limites (LimitRange, ResourceQuota, HPA, VPA) </summary>

**✅LimitRange** : définit des limites CPU et mémoire par défaut pour les Pods ou Containers dans un namespace  
**✅ResourceQuota** : restreint l'utilisation totale des ressources dans un namespace pour éviter la surconsommation  
**✅HPA** : met à l'échelle automatiquement le nombre de répliques de pods selon des métriques comme l'utilisation CPU ou mémoire  
**✅VPA** : ajuste automatiquement les demandes de ressources CPU et mémoire pour les pods au lieu de mettre à l'échelle le nombre de répliques  

</details>

<details>


<summary> Stratégies de Déploiement </summary>

**✅Rolling Update** : (par défaut) remplace progressivement les anciens pods par des nouveaux, sans downtime  
**✅Recreate** : arrête tous les anciens pods avant de démarrer les nouveaux, ce qui cause un downtime  
**✅Blue-Green Deployment** : fait tourner deux versions en parallèle et bascule le trafic vers la nouvelle version une fois validée  
**✅Canary Deployment** : déploie une nouvelle version sur un petit pourcentage du trafic pour tester la stabilité avant le déploiement complet  

</details>

<details>

<summary>❔ Comment débuguer un Pod qui ne démarre pas ?</summary>

> Je commence par regarder l’état du Pod avec `kubectl get pods`.
>
> Ensuite, j’utilise `kubectl describe pod` pour voir les événements, par exemple un problème d’image, de secret, de config ou de ressources.
>
> Si le conteneur démarre puis plante, je regarde les logs avec `kubectl logs`.
>
> Et si besoin, je vérifie aussi les ConfigMaps, Secrets, variables d’environnement, probes et ressources CPU/mémoire.

</details>

<details>

<summary>❔ Donne une commande pour débuguer un Pod</summary>

> Les commandes que j’utilise souvent sont :
>
> `kubectl describe pod nom-du-pod`
>
> pour voir les événements et la cause du problème.
>
> Et aussi :
>
> `kubectl logs nom-du-pod`
>
> pour analyser les logs applicatifs.

</details>


<br><br>

</details>

<details>
<summary> ✅ CI / CD (Jenkins & GitLab)</summary>

> La `CI/CD` (Continuous Integration / Continuous Delivery ou Deployment) est une approche DevOps qui automatise les étapes entre l’écriture du code et sa mise en production.  

<details>
<summary> Gitlab</summary>

> GitLab est une plateforme DevOps qui permet de gérer le code source avec Git, les merge requests, les issues, mais aussi la CI/CD.
>
> Pour la CI/CD, on définit généralement le pipeline dans un fichier .gitlab-ci.yml, avec plusieurs stages comme build, test, analyse Sonar, création de l’image Docker et déploiement.
Les jobs sont ensuite exécutés par des GitLab Runners.

</details>

<details>
<summary> Jenkins</summary>


`jenkins` c’est un outil que j’utilise pour automatiser la chaîne CI/CD.
Il permet de lancer automatiquement le build, les tests, et parfois le déploiement après un push Git.  


**Pipeline Complet de Référence**


```groovy
// Jenkinsfile
pipeline {

    // ─────────────────────────────────────────────────────────────────────
    // AGENT & TOOLS
    // ─────────────────────────────────────────────────────────────────────
    agent any

    tools {
        maven 'Maven-3.9'
        jdk   'JDK-17'
    }
    // ─────────────────────────────────────────────────────────────────────
    // OPTIONS
    // ─────────────────────────────────────────────────────────────────────
    options {
        buildDiscarder(logRotator(numToKeepStr: '10'))
        timeout(time: 45, unit: 'MINUTES')
        timestamps()
        ansiColor('xterm')
        disableConcurrentBuilds()
    }
    // ─────────────────────────────────────────────────────────────────────
    // TRIGGERS
    // ─────────────────────────────────────────────────────────────────────
    triggers {
        // Polling SCM toutes les 5 minutes (alternative : webhook Git)
        pollSCM('H/5 * * * *')
    }
    // ─────────────────────────────────────────────────────────────────────
    // PARAMÈTRES (pour builds manuels)
    // ─────────────────────────────────────────────────────────────────────
    parameters {
        booleanParam(name: 'SKIP_TESTS',   defaultValue: false, description: 'Passer les tests')
        booleanParam(name: 'FORCE_DEPLOY', defaultValue: false, description: 'Forcer le déploiement')
        choice(name: 'TARGET_ENV',
               choices: ['staging', 'production'],
               description: 'Environnement cible')
    }

    // ─────────────────────────────────────────────────────────────────────
    // VARIABLES D'ENVIRONNEMENT
    // ─────────────────────────────────────────────────────────────────────
    environment {
        APP_NAME       = 'mon-app-spring'
        DOCKER_REGISTRY = 'registry.example.com'
        
        // Credentials
        SONAR_TOKEN    = credentials('sonar-token')
        DOCKER_CREDS   = credentials('docker-hub-credentials')
        
        // Tags dynamiques
        SHORT_COMMIT   = "${env.GIT_COMMIT?.take(7) ?: 'unknown'}"
        IMAGE_TAG      = "${env.BUILD_NUMBER}-${SHORT_COMMIT}"
        FULL_IMAGE     = "${DOCKER_REGISTRY}/${APP_NAME}:${IMAGE_TAG}"
        LATEST_IMAGE   = "${DOCKER_REGISTRY}/${APP_NAME}:latest"
    }
    // ─────────────────────────────────────────────────────────────────────
    // STAGES
    // ─────────────────────────────────────────────────────────────────────
    stages {

        // ── STAGE 1 : CLONE ──────────────────────────────────────────────
        stage('Clone') {
            steps {
                checkout scm
                script {
                    env.GIT_AUTHOR = sh(
                        script: "git log -1 --pretty=format:'%an <%ae>'",
                        returnStdout: true
                    ).trim()
                    env.GIT_MESSAGE = sh(
                        script: "git log -1 --pretty=format:'%s'",
                        returnStdout: true
                    ).trim()
                    echo "📦 Branche: ${env.BRANCH_NAME}"
                    echo "👤 Auteur: ${env.GIT_AUTHOR}"
                    echo "💬 Message: ${env.GIT_MESSAGE}"
                }
            }
        }
        // ── STAGE 2 : BUILD ──────────────────────────────────────────────
        stage('Build') {
            steps {
                sh """
                    mvn clean package \
                        -DskipTests \
                        -B \
                        --no-transfer-progress
                """
                script {
                    env.APP_VERSION = sh(
                        script: "mvn help:evaluate -Dexpression=project.version -q -DforceStdout",
                        returnStdout: true
                    ).trim()
                    echo "📋 Version : ${env.APP_VERSION}"
                }
            }
            post {
                success {
                    archiveArtifacts artifacts: 'target/*.jar', fingerprint: true
                }
            }
        }
        // ── STAGE 3 : TESTS ──────────────────────────────────────────────
        stage('Tests') {
            when {
                not { expression { params.SKIP_TESTS } }
            }
            parallel {
                stage('Unit Tests') {
                    steps {
                        sh 'mvn test -B --no-transfer-progress'
                    }
                    post {
                        always {
                            junit 'target/surefire-reports/*.xml'
                        }
                    }
                }
                stage('Integration Tests') {
                    steps {
                        sh 'mvn failsafe:integration-test failsafe:verify -B --no-transfer-progress'
                    }
                    post {
                        always {
                            junit 'target/failsafe-reports/*.xml'
                        }
                    }
                }
            }
            post {
                always {
                    jacoco(
                        execPattern: 'target/jacoco.exec',
                        classPattern: 'target/classes',
                        sourcePattern: 'src/main/java',
                        minimumLineCoverage: '80'
                    )
                }
            }
        }
        // ── STAGE 4 : SONARQUBE ──────────────────────────────────────────
        stage('SonarQube Analysis') {
            steps {
                withSonarQubeEnv('SonarQube') {
                    sh """
                        mvn sonar:sonar \
                            -Dsonar.projectKey=${APP_NAME} \
                            -Dsonar.projectVersion=${env.APP_VERSION} \
                            -B --no-transfer-progress
                    """
                }
            }
        }

        stage('Quality Gate') {
            steps {
                timeout(time: 5, unit: 'MINUTES') {
                    script {
                        def qg = waitForQualityGate()
                        if (qg.status != 'OK') {
                            error "❌ Quality Gate échoué : ${qg.status}"
                        }
                        echo "✅ Quality Gate : ${qg.status}"
                    }
                }
            }
        }
        // ── STAGE 5 : BUILD IMAGE DOCKER ─────────────────────────────────
        stage('Build Docker Image') {
            steps {
                sh """
                    docker build \
                        --build-arg BUILD_DATE=\$(date -u +'%Y-%m-%dT%H:%M:%SZ') \
                        --build-arg VCS_REF=${env.GIT_COMMIT} \
                        --build-arg VERSION=${env.APP_VERSION} \
                        -t ${FULL_IMAGE} \
                        -t ${LATEST_IMAGE} \
                        .
                """
                echo "🐳 Image : ${FULL_IMAGE}"
            }
        }
        // ── STAGE 6 : PUSH IMAGE ─────────────────────────────────────────
        stage('Push Docker Image') {
            steps {
                script {
                    docker.withRegistry("https://${DOCKER_REGISTRY}", 'docker-hub-credentials') {
                        sh "docker push ${FULL_IMAGE}"
                        sh "docker push ${LATEST_IMAGE}"
                    }
                }
            }
            post {
                cleanup {
                    sh "docker rmi ${FULL_IMAGE} ${LATEST_IMAGE} || true"
                }
            }
        }
        // ── STAGE 7 : DEPLOYMENT ─────────────────────────────────────────
        stage('Deploy to Staging') {
            when {
                anyOf {
                    branch 'develop'
                    expression { params.FORCE_DEPLOY && params.TARGET_ENV == 'staging' }
                }
            }
            steps {
                withCredentials([file(credentialsId: 'k8s-config', variable: 'KUBECONFIG')]) {
                    sh """
                        sed -i 's|{{IMAGE_TAG}}|${IMAGE_TAG}|g' k8s/staging/deployment.yaml
                        kubectl apply -f k8s/staging/
                        kubectl rollout status deployment/${APP_NAME} -n staging --timeout=5m
                    """
                }
                echo "🚀 Déployé sur Staging !"
            }
        }

        stage('Deploy to Production') {
            when {
                anyOf {
                    branch 'main'
                    expression { params.FORCE_DEPLOY && params.TARGET_ENV == 'production' }
                }
            }
            steps {
                // Validation manuelle avant la production
                input message: "Déployer la version ${IMAGE_TAG} en production ?",
                      ok: 'Déployer',
                      submitter: 'admin,lead-dev'
                
                withCredentials([file(credentialsId: 'k8s-config', variable: 'KUBECONFIG')]) {
                    sh """
                        sed -i 's|{{IMAGE_TAG}}|${IMAGE_TAG}|g' k8s/production/deployment.yaml
                        kubectl apply -f k8s/production/
                        kubectl rollout status deployment/${APP_NAME} -n production --timeout=10m
                    """
                }
                echo "🎉 Déployé en Production !"
            }
            post {
                failure {
                    withCredentials([file(credentialsId: 'k8s-config', variable: 'KUBECONFIG')]) {
                        sh "kubectl rollout undo deployment/${APP_NAME} -n production"
                        echo "⏪ Rollback production effectué !"
                    }
                }
            }
        }
    } // fin stages

    // ─────────────────────────────────────────────────────────────────────
    // POST PIPELINE
    // ─────────────────────────────────────────────────────────────────────
    post {
        always {
            cleanWs()  // Nettoyer le workspace
        }
        success {
            slackSend channel: '#ci-cd', color: 'good',
                message: "✅ *SUCCESS* ${env.JOB_NAME} #${env.BUILD_NUMBER} | `${env.BRANCH_NAME}` | <${env.BUILD_URL}|Voir>"
        }
        failure {
            slackSend channel: '#ci-cd', color: 'danger',
                message: "❌ *FAILURE* ${env.JOB_NAME} #${env.BUILD_NUMBER} | `${env.BRANCH_NAME}` | <${env.BUILD_URL}|Voir>"
            emailext subject: "Build Failed: ${env.JOB_NAME} #${env.BUILD_NUMBER}",
                     body: "Build URL: ${env.BUILD_URL}",
                     to: 'team@example.com'
        }
        unstable {
            slackSend channel: '#ci-cd', color: 'warning',
                message: "⚠️ *UNSTABLE* ${env.JOB_NAME} #${env.BUILD_NUMBER}"
        }
    }

} // fin pipeline
```

</details>

<details>
<summary> Jenkins vs Gitlab</summary>

> `Jenkins` est un serveur CI/CD indépendant et très extensible. `GitLab` CI est une CI/CD directement intégrée à GitLab.   > `Jenkins` utilise des agents et un Jenkinsfile, `GitLab` utilise des runners et un .gitlab-ci.yml.

</details>

<br><br>

</details>

<details>
<summary> ✅ Maven & Git</summary>



<details>
<summary> Maven</summary>

> **Maven est un outil de gestion de projet Java. Il permet de gérer les dépendances, compiler le projet, lancer les tests, générer le package et éventuellement le publier.**
>
> Maven fonctionne avec un **cycle de vie composé de plusieurs phases**. Les principales sont :
>
> * **validate** : vérifie que le projet est correctement configuré ;
> * **compile** : compile le code source ;
> * **test** : exécute les tests unitaires ;
> * **package** : génère le JAR ou le WAR ;
> * **verify** : effectue des vérifications supplémentaires ;
> * **install** : installe l’artefact dans le repository Maven local ;
> * **deploy** : publie l’artefact dans un repository distant, par exemple Nexus ou Artifactory.
>
> **Quand on lance une phase, Maven exécute automatiquement toutes les phases précédentes.** Par exemple, `mvn package` va aussi faire `validate`, `compile` et `test`.

</details>

<details>
<summary> Merge vs Rebase</summary>

> **Merge** conserve l’historique et ajoute éventuellement un commit de merge, alors que **rebase** réécrit l’historique pour obtenir une timeline plus linéaire.

</details>

<details>
<summary> Reset vs Revert</summary>

> `reset` et `revert` permettent tous les deux d’annuler des changements, mais pas de la même manière.
> `Reset` modifie l’historique, alors que `revert` ajoute un nouveau commit d’annulation.


</details>

<details>
<summary> git reset soft, mixed et hard
</summary>

> `soft`: Le commit est supprimé, mais les modifications restent dans le staging area.  
> `mixed`: Le commit est supprimé et les fichiers sont retirés du staging, mais les modifications restent dans le working directory.  
> `hard`: Le commit est supprimé, les fichiers sont retirés du staging et les modifications locales sont également supprimées.  

</details>

<details>
<summary> Cherry-pick</summary>

> `cherry-pick` permet de récupérer un commit précis d’une autre branche et de l’appliquer sur ma branche actuelle

</details>

<br><br>

</details>

<br>

<details>

<summary> ✅ Migration technique </summary>

<details>

<summary>Comment <b>migrer</b> une application vers <b>Kubernetes</b> ?</summary>

> Je commence par analyser l’application : configuration, dépendances, flux et accès base de données.
>
> Ensuite, je la conteneurise avec Docker, puis je prépare les fichiers Kubernetes : Deployment, Service, ConfigMap, Secret et Ingress si besoin.
>
> Enfin, je déploie dans un environnement de test pour valider les logs, les flux, la configuration et les performances.

</details>


<details>

<summary>Comment <em><b>migrer une fonctionnalité d'un monolithe vers un microservice</em></b> ?</b></summary>

> Je commence par analyser la fonctionnalité existante et ses dépendances.
>
> Ensuite, je la sépare du monolithe et je la développe dans un microservice.
>
> Je mets en place les communications nécessaires, par exemple REST ou RabbitMQ.
>
> Puis je teste et je migre progressivement pour limiter les risques.

</details>

<details>

<summary> Comment réussir une migration <b>legacy vers microservices</b> ? </summary>

> Il faut éviter de tout réécrire d’un coup.
> Je préfère une approche progressive : identifier un périmètre, créer un service cible, synchroniser ou exposer les données nécessaires, tester, puis basculer progressivement.
> L’observabilité et la non-régression sont très importantes.

</details>

<details>

<summary>Quels sont les principaux risques d'une migration ?</summary>

> Les principaux risques concernent les incompatibilités techniques, les problèmes de performance, les erreurs de configuration et les impacts sur les flux existants.
>
> Dans le cas d'une migration vers Kubernetes, il faut également s'assurer que les communications réseau, les accès aux bases de données, les certificats et les mécanismes de sécurité fonctionnent correctement.

</details>

<br><br>

</details>



<details>

<summary> ✅ Microservices </summary>

<details>

<summary> C’est quoi une <b>architecture</b> microservices</b> ? </summary>

> C’est une architecture où l’application est découpée en plusieurs services indépendants.
> Chaque service porte un périmètre métier clair, peut être développé, déployé et scalé indépendamment.
> L’objectif est d’avoir plus de modularité et de flexibilité.

</details>

<details>

<summary> <b>Avantages</b> des microservices ? </summary>

> Les avantages sont le découpage par domaine, l’indépendance des déploiements, la scalabilité par service, et une meilleure séparation des responsabilités.
> Ça permet aussi à plusieurs équipes de travailler plus facilement en parallèle.

</details>

<details>

<summary> <b>Inconvénients</b> des microservices ? </summary>

> Ça ajoute de la complexité.
> Il faut gérer la communication réseau, l’observabilité, les erreurs entre services, la cohérence des données, le monitoring et le déploiement.
> Donc il ne faut pas faire du microservice juste pour faire du microservice.

</details>

<details>

<summary> Comment un <b>microservice communique</b> avec un autre ? </summary>

> Il peut communiquer de manière synchrone, par exemple avec REST ou GraphQL.
> Il peut aussi communiquer de manière asynchrone avec une file de messages comme RabbitMQ ou Kafka.
> Le choix dépend du besoin : réponse immédiate ou traitement découplé.

</details>

<details>

<summary> <b>Synchrone</b> vs <b>asynchrone</b> ? </summary>

> Synchrone, le service appelant attend une réponse immédiate. C’est simple, mais ça crée du couplage.
> Asynchrone, on envoie un message et le traitement peut se faire plus tard. C’est plus robuste et découplé, mais plus complexe à suivre et à monitorer.

</details>

<details>

<summary> Comment tu <b>gères une erreur</b> entre microservices ? </summary>

> Je commence par bien gérer les timeouts, les retries si c’est pertinent, et les erreurs fonctionnelles.
> Il faut éviter les retries infinis.
> On peut utiliser un circuit breaker pour éviter de surcharger un service en erreur.
> Et surtout, il faut des logs et du tracing pour comprendre le parcours de la requête.

</details>

<details>

<summary> C’est quoi un <b>circuit breaker</b> ? </summary>

> C’est un mécanisme qui coupe temporairement les appels vers un service qui est en erreur.
> Ça évite de continuer à appeler un service indisponible et de propager la panne.
> Après un certain temps, on retente progressivement.

</details>

<details>

<summary> Comment tu <b>découpes un microservice</b> ? </summary>

> Je pars du domaine métier.
> Chaque service doit avoir une responsabilité claire.
> Par exemple, un service de catégorisation ne doit pas forcément gérer les notifications.
> Il faut éviter de découper uniquement par couche technique, sinon on crée trop de dépendances.

</details>

<br><br>

</details>

<details>

<summary>✅ PROD & Observabilite & Fault Tolerence ?</summary>

**PROD**

<details>

<summary>Comment gérez-vous les <b>incidents en production</b> ?</summary>


> En production, je commence d’abord par analyser les logs pour comprendre l’origine du problème.
>
> Ensuite, je vérifie si c’est un problème technique ou fonctionnel.
> Si c’est fonctionnel, j’échange avec les Business Analysts pour bien comprendre le contexte métier.
>
> Après ça, j’essaie de reproduire le problème en environnement de dev, souvent avec un jeu de données fourni par le BA.
>
> Si nécessaire, je fais du debug en local pour identifier précisément la cause.
>
> Une fois le problème corrigé, je valide avec des tests, puis on prépare le déploiement en production.

</details>

<details>

<summary>Parlez-moi d’un <em><b>incident de production</em></b> que tu as géré</summary>

**Situation**: `Un traitement ne consomme plus les messages RabbitMQ`

> Chez SGSS, on a des workers qui consomme des messages RabbitMQ pour traiter les instructions financières.

**Problème**

> Un jour, on a constaté qu'un traitement n'avançait plus et que des messages commençaient à rester dans la queue.

**Action**

> J'ai commencé par vérifier l'état du pod sur Kubernetes.
>
> Ensuite, j'ai regardé les logs dans Kibana et les événements de l'application.
>
> J'ai identifié un problème de configuration qui empêchait le worker de traiter correctement les messages.
>
> On a corrigé la configuration, redéployé l'application et vérifié que les messages étaient de nouveau consommés.

**Résultat**

> Le traitement fonctione normalement.
>
> Ensuite, on a continué à surveiller la queue et les logs pour vérifier qu'il n'y avait plus de problème.

</details>

<details>

<summary>Parlez-moi d’un <em><b>problème de déploiement</em></b> que tu as rencontré?</summary>

**Situation**

> Après le déploiement d'une nouvelle version d'un microservice, le pod ne démarrait pas correctement.

**Problème**

> Le pod passait régulièrement en CrashLoopBackOff.

**Action**

> J'ai commencé par vérifier le statut du pod et les événements Kubernetes.
>
> Ensuite, j'ai regardé les logs de l'application.
>
> J'ai identifié qu'une variable de configuration attendue par l'application n'était pas correctement définie.
>
> on a corrigé la ConfigMap puis redéployé l'application.

**Résultat**

> Le pod a démarré normalement et le service est redevenu disponible.

</details>

<details>

<summary>Avez-vous déjà participé à la <em><b>documentation technique</em></b> d’une application ?</summary>

> Oui, notamment pendant la migration du monolithe vers les microservices chez SGSS.
>
> Pour chaque nouveau service, on documente son rôle, les API exposées, les dépendances et les principaux flux.
>
> On met aussi les informations importantes dans le README : comment lancer le service, les principales configurations, les variables d’environnement et les dépendances externes.
>
> Comme ça, un nouveau développeur peut comprendre plus facilement le service.

</details>

**Observabilité & Monitoring**

<details>

<summary>Quelle est la différence entre <b>monitoring</b> et <b>observabilité</b> ?</summary>

> Le monitoring consiste principalement à surveiller des métriques connues, par exemple le CPU, la mémoire, le nombre d’erreurs ou le temps de réponse.
>
> L’observabilité va plus loin : elle permet de comprendre ce qui se passe à l’intérieur du système à partir des logs, des métriques et des traces.
>
> Donc le monitoring me dit qu’il y a un problème, alors que l’observabilité m’aide à comprendre pourquoi.

**À retenir :**

`Monitoring → il y a un problème.`
`Observability → pourquoi il y a un problème ?`

</details>


<details>

<summary>Quels <b>niveaux de logs</b> connais-tu ?</summary>

> Principalement TRACE, DEBUG, INFO, WARN et ERROR.
>
> En production, on utilise généralement INFO pour les événements normaux, WARN pour les situations anormales mais non bloquantes, et ERROR lorsqu’un traitement échoue.
>
> DEBUG et TRACE sont plutôt utilisés pour avoir davantage de détails lors d’une investigation.

</details>


<details>

<summary>Qu’est-ce qu’un <b>correlation ID</b> ?</summary>

> C’est un identifiant associé à une requête ou à un traitement et propagé entre les différents services.
>
> Il permet de retrouver facilement tous les logs correspondant au même traitement dans une architecture microservices.

</details>


<details>

<summary>Quelle est la différence entre <b>logs</b> et traces <b>distribuées</b> ?</summary>

> Les logs décrivent les événements produits par chaque application.
>
> Une trace distribuée permet plutôt de suivre le parcours complet d’une requête entre plusieurs services.
>
> Elle permet notamment de voir quels services ont été appelés et où le temps a été passé.

</details>


<details>

<summary>Comment surveiller la disponibilité d’un microservice (<b>Liveness</b> & <b>Readiness</b>) ?</summary>

> Avec des health checks.
>
> Typiquement, on distingue un liveness check qui indique si l’application fonctionne encore et un readiness check qui indique si elle est prête à recevoir du trafic.

```text
Liveness  → est-ce que l'application est vivante ?
Readiness → peut-elle recevoir du trafic ?
```

</details>



**Fault Tolerance / Résilience**

<details>

<summary>Qu’est-ce que la <b>fault tolerance</b> ?</summary>

> La fault tolerance, ou tolérance aux pannes, est la capacité d’un système à continuer à fonctionner même lorsqu’un de ses composants rencontre un problème.
>
> L’objectif est d’éviter qu’une panne locale provoque la panne de tout le système.

</details>


<details>

<summary>Comment rendre un <b>système distribué tolérant aux pannes</b> ?</summary>

> Il existe plusieurs mécanismes : timeout, retry, circuit breaker, réplication, load balancing, health checks et mécanismes de fallback.
>
> Pour les traitements asynchrones, on peut aussi utiliser les acknowledgments, les retries et les dead-letter queues.

</details>


<details>

<summary>Qu’est-ce qu’un <b>retry</b> ?</summary>

> Un retry consiste à réessayer automatiquement une opération lorsqu’elle échoue à cause d’un problème temporaire.
>
> Par exemple, si un service externe est momentanément indisponible, on peut réessayer après un délai.

> Par contre, il faut éviter les retries infinis.

</details>

<details>

<summary>Qu’est-ce qu’un <b>circuit breaker</b> ?</summary>

> Un circuit breaker empêche d’appeler continuellement un service qui est déjà en panne.
>
> Après plusieurs erreurs, le circuit s’ouvre et les appels échouent rapidement.
>
> Après un certain délai, il laisse passer quelques appels pour vérifier si le service est revenu.

Retenir :

```text
CLOSED
   ↓ trop d'erreurs
OPEN
   ↓ après un délai
HALF-OPEN
   ↓
succès → CLOSED
échec  → OPEN
```

</details>



<details>

<summary>Qu’est-ce que le <b>Bulkhead Pattern</b> ?</summary>

> Le principe du bulkhead est d’isoler les ressources utilisées par différents traitements.
>
> Comme ça, si une partie du système est saturée, elle ne consomme pas toutes les ressources et n’empêche pas les autres fonctionnalités de fonctionner.

</details>


<details>

<summary>Pourquoi l’<b>idempotence est-elle importante avec les retries</b> ?</summary>

> Parce qu’un retry peut exécuter plusieurs fois la même opération.
>
> Si le traitement n’est pas idempotent, on risque par exemple de créer plusieurs fois la même transaction.
>
> Il faut donc pouvoir identifier qu’une demande a déjà été traitée.

</details>


<details>

<summary>Que se passe-t-il si une <b>instance de ton microservice tombe</b> ?</summary>

> Si plusieurs instances sont déployées, le trafic doit pouvoir être redirigé vers les instances encore disponibles.
>
> Dans Kubernetes, par exemple, le pod défaillant peut être redémarré et les health checks permettent d’éviter de lui envoyer du trafic tant qu’il n’est pas prêt.

</details>


<details>

<summary>Quelle différence entre <b>High Availability et Fault Tolerance </b> ?</summary>

> La High Availability cherche principalement à maximiser la disponibilité du système et réduire les interruptions.
>
> La Fault Tolerance va plus loin : le système est conçu pour continuer à fonctionner même lorsqu’un composant tombe en panne.


</details>

</br></br>

</details>



<details>
<summary> ✅ AI</summary>

<details>
<summary> C’est quoi l’<b>IA générative</b> ? </summary>

> L’IA générative, c’est une IA capable de générer du contenu : texte, code, résumé, image ou réponse à une question.  
> En entreprise, on l’utilise surtout pour aider les utilisateurs, automatiser certaines tâches ou exploiter des documents internes.  

</details>

<details>
<summary> C’est quoi un <b>LLM</b> ? </summary>

> Un LLM est un grand modèle de langage.  
> Il est entraîné sur beaucoup de textes et il génère une réponse en prédisant les mots les plus probables selon le contexte.  

</details>

<details>
<summary> C’est quoi une <b>hallucination</b> ? </summary>

> Une hallucination, c’est quand le modèle donne une réponse fausse mais avec beaucoup de confiance.  
> Pour limiter ça, il faut utiliser des sources fiables, du RAG, des tests et parfois une validation humaine.  

</details>

<details>
<summary> C’est quoi le <b>Prompt Engineering</b> ? </summary>

> C’est la manière de bien formuler une demande à l’IA.  
> Un bon prompt donne le contexte, l’objectif, les contraintes et le format attendu.  

</details>

<details>
<summary> C’est quoi le <b>RAG</b> ? </summary>

> RAG veut dire Retrieval-Augmented Generation.  
> Le principe est de chercher d’abord les documents utiles, puis de les donner au LLM pour générer une réponse basée sur ces sources.  

</details>

<details>
<summary> Différence entre <b>RAG</b> et <b> Fine-tuning </b> ? </summary>

> Le fine-tuning modifie le modèle en le réentraînant.  
> Le RAG ne modifie pas le modèle, il ajoute seulement du contexte au moment de la question.  
> En entreprise, on commence souvent par le RAG.  

</details>

<details>
<summary> C’est quoi un <b>embedding</b> ? </summary>

> Un embedding, c’est une représentation numérique d’un texte.  
> Ça permet de comparer le sens entre une question et des documents, même si les mots ne sont pas exactement les mêmes.  

</details>

<details>
<summary> C’est quoi une <b>base vectorielle</b> ? </summary>

> C’est une base qui stocke des embeddings.  
> Elle permet de faire une recherche sémantique pour retrouver les documents les plus proches du sens de la question.  

</details>

<details>
<summary> C’est quoi le <b>Function Calling</b> ? </summary>

> C’est la capacité d’un LLM à appeler une fonction ou une API externe.  
> Par exemple, il peut appeler une API métier, récupérer une donnée, puis générer une réponse avec le résultat.  

</details>

<details>
<summary>  C’est quoi un <b>agent IA</b> ? </summary>

> Un agent IA, c’est un LLM qui peut utiliser des outils pour atteindre un objectif.  
> Il peut analyser une demande, choisir une action, appeler une API, lire le résultat, puis continuer.  

</details>

<details>
<summary>  Quels sont les <b>risques de sécurité</b> avec l’IA ? </summary>

> Le risque principal est d’envoyer des données sensibles : code propriétaire, clés API, données clients ou informations internes.  
> Il faut respecter les règles de sécurité, anonymiser les données et utiliser les outils validés par l’entreprise.  

</details>

<details>
<summary>  Comment tu utilises l’IA comme développeur ? </summary>

> Je l’utilise comme un assistant, pas comme une source aveugle.  
> Elle peut m’aider à comprendre du code, générer des tests, proposer un refactoring ou expliquer une erreur.  
> Mais je relis, je comprends, je teste et j’adapte toujours le code généré.  

</details>

<br><br>

</details>



<details>

<summary> ✅ AWS </summary>

<details>

<summary>1. Qu’est-ce qu’<b>AWS</b> et quels services connais-tu ?</summary>

> AWS est une plateforme Cloud qui fournit des services d’infrastructure, de stockage, de bases de données, de réseau et beaucoup d’autres services.
>
> Parmi les services principaux que je connais :
>
> * **EC2** pour les machines virtuelles ;
> * **S3** pour le stockage d’objets ;
> * **RDS** pour les bases de données relationnelles managées ;
> * **IAM** pour gérer les utilisateurs, rôles et permissions ;
> * **VPC** pour le réseau ;
> * **CloudWatch** pour le monitoring et les logs ;
> * **Lambda** pour exécuter du code serverless ;
> * **SQS** pour la messagerie asynchrone ;
> * **EKS** pour Kubernetes sur AWS.

</details>

<details>

<summary>2. Quelle est la différence entre <b>EC2</b>, <b>Lambda</b> et <b>EKS</b> ?</summary>

> **EC2** fournit des machines virtuelles sur lesquelles on gère l’application et l’environnement d’exécution.
>
> **Lambda** permet d’exécuter du code sans gérer de serveur. C’est du serverless et on paie principalement en fonction de l’utilisation.
>
> **EKS** est le service Kubernetes managé d’AWS. Il est adapté lorsqu’on veut déployer des applications conteneurisées avec Kubernetes.
>
> Donc, de manière simple :
>
> **EC2 → VM**
>
> **Lambda → serverless**
>
> **EKS → Kubernetes**

</details>

<details>

<summary>3. Quelle est la différence entre <b>S3</b>, <b>EBS</b> et <b>RDS</b> ?</summary>

> **S3** est un stockage d’objets. On l’utilise par exemple pour stocker des fichiers, des backups ou des documents.
>
> **EBS** est un stockage bloc généralement attaché à une instance EC2, un peu comme un disque dur virtuel.
>
> **RDS** est un service de base de données relationnelle managée qui supporte notamment PostgreSQL, MySQL ou Oracle.
>
> Donc :
>
> **S3 → fichiers / objets**
>
> **EBS → disque d’une VM**
>
> **RDS → base de données relationnelle**

</details>

<details>

<summary>4. Qu’est-ce qu’<b>IAM</b> dans AWS ?</summary>

> IAM signifie **Identity and Access Management**.
>
> Il permet de contrôler **qui peut accéder à quelles ressources AWS et avec quelles permissions**.
>
> On peut gérer notamment :
>
> * les utilisateurs ;
> * les groupes ;
> * les rôles ;
> * les policies.
>
> Une bonne pratique est d’appliquer le principe du **least privilege**, donc donner uniquement les permissions nécessaires.
>
> Par exemple, une application déployée sur AWS peut utiliser un **IAM Role** pour accéder à un bucket S3 sans stocker directement des clés AWS dans le code.

</details>

<details>

<summary>5. Qu’est-ce qu’un <b>VPC</b> ?</summary>

> Un VPC, ou **Virtual Private Cloud**, est un réseau privé virtuel dans AWS.
>
> Il permet d’isoler les ressources et d’organiser le réseau avec notamment :
>
> * des subnets publics et privés ;
> * des security groups ;
> * des route tables ;
> * une Internet Gateway ;
> * éventuellement une NAT Gateway.
>
> Par exemple, je peux mettre un load balancer dans un subnet public et les applications ainsi que la base de données dans des subnets privés.

</details>

<details>

<summary>6. Quelle est la différence entre un <b>Security Group</b> et une <b>NACL</b> ?</summary>

> Les deux servent à contrôler le trafic réseau, mais ils ne travaillent pas au même niveau.
>
> Le **Security Group** est associé principalement aux ressources comme les instances EC2 et fonctionne comme un firewall au niveau de l’instance.
>
> La **Network ACL**, ou NACL, s’applique au niveau du subnet.
>
> Une différence importante est que :
>
> * **Security Group → stateful**
> * **NACL → stateless**
>
> En pratique, les Security Groups sont beaucoup plus couramment utilisés pour contrôler les accès aux applications.

</details>

<details>

<summary>7. Comment rendre une application <b>hautement disponible</b> sur AWS ?</summary>

> J’éviterais d’avoir une seule instance de l’application.
>
> Par exemple, je pourrais avoir :
>
> **Load Balancer**
> → plusieurs instances ou pods
> → répartis sur plusieurs **Availability Zones**
> → base de données RDS avec haute disponibilité.
>
> On peut également utiliser :
>
> * Auto Scaling ;
> * health checks ;
> * monitoring avec CloudWatch ;
> * plusieurs Availability Zones.
>
> Le principe est d’éviter les **single points of failure**.

</details>

<details>

<summary>8. Comment monitorer une application sur <b>AWS</b> (CloudWatch)?</summary>

> Le service principal est **CloudWatch**.
>
> Il permet notamment de récupérer :
>
> * des métriques ;
> * des logs ;
> * des alarmes.
>
> Par exemple, on peut surveiller :
>
> * CPU ;
> * mémoire selon la configuration ;
> * nombre de requêtes ;
> * erreurs ;
> * latence.
>
> Et créer une alarme si une métrique dépasse un certain seuil.
>
> Dans une architecture microservices, je compléterais également cela avec du logging centralisé et du tracing distribué.

</details>

<br><br>

</details>




<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>



<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
