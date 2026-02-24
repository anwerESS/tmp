# ✅Java

<details>
<summary> Core APIs</summary>

- ✅ **String**: Immutable, use `equals()` for comparison, String pool for literals

- ✅ **StringBuilder**: Mutable, used for building Strings, more efficient than multiple concatenations

- ✅ **String methods**:
   - `length()`, `charAt()`, `indexOf()`, `substring()`
   - `toLowerCase()`, `toUpperCase()`, `trim()`, `strip()`
   - `replace()`, `startsWith()`, `endsWith()`, `contains()`
- ✅ **StringBuilder methods**:
   - `append()`, `insert()`, `delete()`, `deleteCharAt()`
   - `replace()`, `reverse()`, `toString()`
- ✅ **Arrays**: 
   - Fixed size, indexed from 0 to `length-1`
   - `Arrays.sort()`, `Arrays.binarySearch()` (requires sorted array)
   - `Arrays.equals()`, `Arrays.toString()`, `Arrays.copyOf()`
- ✅ **Math**: 
   - `abs()`, `min()`, `max()`, `pow()`, `sqrt()`
   - `round()`, `ceil()`, `floor()`, `random()`
- ✅ **Date/Time API** (java.time):
   - **LocalDate**: date without time
   - **LocalTime**: time without date
   - **LocalDateTime**: date and time
   - **Immutable**: methods return new objects
- ✅ **Period vs Duration**:
   - **Period**: years, months, days (for LocalDate)
   - **Duration**: hours, minutes, seconds (for LocalTime)
- ✅ **Date manipulation**:
   - `plusDays()`, `plusMonths()`, `plusYears()`
   - `minusDays()`, `minusMonths()`, `minusYears()`
   - `isBefore()`, `isAfter()`, `isEqual()`
- ✅ **Formatting**: `DateTimeFormatter` with custom patterns

</details>



<br>

<details>
<summary> Object</summary>

**All classes** implicitly inherit from `java.lang.Object`.


**Inherited methods from Object**:  
`toString()`: String representation of the object  
`equals(Object obj)`: equality comparison  
`hashCode()`: hash code  
`getClass()`: class information  
`clone()`: object copy  
etc.

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
**Order of Objects Initialization**

  `1)` Static members of the superclass  
  `2)` Static members of the class  
  `3)` Instance members of the superclass  
  `4)` Constructor of the superclass  
  `5)` Instance members of the class  
  `6)` Constructor of the class  
</details>



<details>
<summary> Overriding, Abstract Methods/Classes, Immutable Classes</summary>

**✅Overriding Methods**: redefine an inherited method.

> `Rules`:  
> `1)` **Same signature** (name + parameters)  
> `2)` Return type must be the **same** or a **subtype** (covariant)  
> `3)` Access modifier must be the **same** or **more permissive**  
> `4)` Cannot throw **new checked exceptions** (or broader ones)  
> `5)` Method must be **non-final** in the superclass  


**✅Abstract Methods**: declared with `abstract`, `no implementation`.

> `Rules`:  
> `1)`. Must be in an `abstract class` or `interface`  
> `2)`. Cannot be `private`, `static`, or `final`  


**✅Abstract Class Rules**

> `Rule 1`: An abstract class can have **0 or more** abstract methods.  
> `Rule 2`: If a class has **at least one** abstract method, it **must** be abstract.  
> `Rule 3`: A concrete subclass **must** implement all abstract methods.  
> `Rule 4`: An abstract subclass does **not** have to implement abstract methods.  


**✅Creating Immutable Classes**

> `Rules` to create an immutable class:  
> `1)` Declare the class **final** (prevents inheritance)  
> `2)` All fields **private** and **final**  
> `3)` No **setters**  
> `4)` Constructor initializes all fields  
> `5)` Defensive copies for mutable objects (List, classes...)  

```java
    public List<String> getEmails() {
        return Collections.unmodifiableList(emails);  // Read-only view
    }

```

</details>

<details>
<summary> Interfaces, Enums, Sealed, Records, Nested Classes, Polymorphism</summary>

- ✅ **Interfaces**:
   - Contract of methods to implement
   - Multiple inheritance possible
   - Members are `public` by default
   - Variables are `public static final`
   - Default methods (Java 8+): implementation inside the interface
   - Static methods (Java 8+): utility methods
   - Private methods (Java 9+): internal helpers
- ✅ **Enums**:
   - Type for fixed constants
   - Methods: `values()`, `valueOf()`, `ordinal()`, `name()`
   - Can have fields, constructors (private), methods
   - Can implement interfaces
   - Abstract methods possible
- ✅ **Sealed Classes** (Java 17+):
   - Controls subclasses with `permits`
   - Subclasses must be `final`, `sealed`, or `non-sealed`
   - Exhaustive pattern matching
- ✅ **Records** (Java 16+):
   - Immutable data classes
   - Auto-generates: constructor, getters, `equals()`, `hashCode()`, `toString()`
   - Implicitly `private final` fields
   - Compact constructor for validation
   - Defensive copy needed for mutable objects
- ✅ **Nested Classes**:
   - **Inner class**: access to outer members, requires instance
   - **Static nested class**: no access to outer instance members
   - **Local class**: defined inside a block, accesses final/effectively final variables
   - **Anonymous class**: no name, single use
- ✅ **Polymorphism**:
   - Object takes multiple forms
   - Methods called based on object type (runtime)
   - Variables accessed based on reference type (compile-time)
   - Upcasting implicit, downcasting explicit
   - Check with `instanceof` before casting

  **Interface vs Abstract Class**

  | Interface | Abstract Class |
  |-----------|----------------|
  | `interface` keyword | `abstract class` keyword |
  | All members are `public` by default | Can have any access modifier |
  | Abstract methods by default | Can have concrete and abstract methods |
  | No constructors | Can have constructors |
  | **Multiple** inheritance possible | **Single** inheritance only |
  | No instance variables (except constants) | Can have instance variables |

</details>

<br>

<details>
<summary> Lambda, Functional Interface, Method Reference, Method Chaining</summary>

**`lambda expression`** is a concise anonymous function, introduced in Java 8.

**`functional interface`** has **exactly one** abstract method (Default and static methods don't count).

**`method reference`** is a shortcut for a lambda that **just calls a method**.


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

Java provides functional interfaces in java.util.function.

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

**Primitive Functional Interfaces**

To avoid autoboxing/unboxing, Java provides primitive versions.

**Examples**:
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
<summary> Collection Hierarchy, List </summary>

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
**List** is an `ordered` collection that allows `duplicates` and access by `index`.

**Main implementations**:
`ArrayList`: resizable array, fast for random access
`LinkedList`: doubly linked list, fast for insert/delete



**List-specific methods** (in addition to Collection):

```java
// Access by index
E get(int index)
E set(int index, E element)
// Insert/remove by index
void add(int index, E element)
E remove(int index)
// Search
int indexOf(Object o)
int lastIndexOf(Object o)
// Sublist: VIEW (backed by original)
List<E> subList(int fromIndex, int toIndex)
// Replacement
void replaceAll(UnaryOperator<E> operator)
// Sort
void sort(Comparator<? super E> c)
```


**ArrayList vs LinkedList**

| Feature | ArrayList | LinkedList |  
|---------|-----------|------------|  
| Access by index | O(1) fast | O(n) slow |  
| Insert/delete at start/middle | O(n) slow | O(1) fast |  
| Insert/delete at end | O(1) amortized | O(1) |  

</details>


<details>
<summary> Set, Queue, Deque</summary>

**✅Set**: a collection that **doesn't allow duplicates**.

**Main implementations**:
`HashSet`: unordered, fast (O(1) for add/remove/contains) (allows 1 null)
`LinkedHashSet`: maintains insertion order (allows 1 null)
`TreeSet`: sorted (natural order or Comparator), slower (O(log n)) (no nulls allowed)

**Set Operations**

```java
// Union (all elements)
union.addAll(set2);
// Intersection (common elements)
intersection.retainAll(set2);
// Difference (in set1 but not in set2)
difference.removeAll(set2);
```

**✅Queue**: follows the **FIFO** principle (First In, First Out).

`Main methods`:

  | Operation | Throws Exception | Returns Special Value |
  |-----------|------------------|----------------------|
  | Insert | `add(e)` | `offer(e)` |
  | Remove | `remove()` | `poll()` |
  | Examine | `element()` | `peek()` |


**✅Deque** (Double-Ended Queue): queue with **two ends**, can be used as:
`Queue` (FIFO)
`Stack` (LIFO)

`Main methods`:

  | Operation | First Element | Last Element |
  |-----------|--------------|--------------|
  | Insert | `addFirst(e)` / `offerFirst(e)` | `addLast(e)` / `offerLast(e)` |
  | Remove | `removeFirst()` / `pollFirst()` | `removeLast()` / `pollLast()` |
  | Examine | `getFirst()` / `peekFirst()` | `getLast()` / `peekLast()` |

**Stack operations**:
- `push(e)` = `addFirst(e)`
- `pop()` = `removeFirst()`
- `peek()` = `peekFirst()`

</details>


<details>
<summary> Map </summary>

**Map** stores `key-value` pairs. Keys are `unique`, values can be duplicated.

**Main implementations**:  
`HashMap`: unordered, fast (O(1))  
`LinkedHashMap`: maintains insertion order  
`TreeMap`: sorted keys (O(log n))  

**Common Map Methods**

```java
// Add/update
V put(K key, V value)
void putAll(Map<? extends K, ? extends V> m)
V putIfAbsent(K key, V value)
// Retrieve
V get(Object key)
V getOrDefault(Object key, V defaultValue)
// Remove
V remove(Object key)
boolean remove(Object key, Object value)
void clear()
// Check
boolean containsKey(Object key)
boolean containsValue(Object value)
boolean isEmpty()
int size()
// Views
Set<K> keySet()
Collection<V> values()
Set<Map.Entry<K, V>> entrySet()
// Java 8+ operations
void forEach(BiConsumer<? super K, ? super V> action)
V compute(K key, BiFunction<? super K, ? super V, ? extends V> remappingFunction)
V merge(K key, V value, BiFunction<? super V, ? super V, ? extends V> remappingFunction)
```

</details>


<details>
<summary> Sorting Data (Comparable, Comparator)</summary>


**Comparable**: defines the **natural ordering** of a class.

Method: `int compareTo(T o)`

```java
public class Person implements Comparable<Person> {
    //..
    @Override
    public int compareTo(Person other) {
        return this.age - other.age;  // Sort by age
    }
}

Collections.sort(personList);  // Uses compareTo()
```

**Comparator**: defines a **custom ordering** (without modifying the class).

Method: `int compare(T o1, T o2)`

```java
// Comparator by name
Comparator<Person> byName = (p1, p2) -> p1.name.compareTo(p2.name);

Collections.sort(personList, byName);
personList.sort(byName);  // Or
```

**Comparator Helper Methods (Java 8+)**

```java
// Sort by a field
Comparator<Person> byAge = Comparator.comparing(Person::getAge);
// Reverse order
Comparator<Person> byAgeDesc = Comparator.comparing(Person::getAge).reversed();
// Multi-field sort (age then name)
Comparator<Person> comparator = Comparator
    .comparing(Person::getAge)
    .thenComparing(Person::getName);
// Handle nulls
Comparator<String> comparator = Comparator.nullsFirst(String::compareTo);
// natural/reverse order
numbers.sort(Comparator.naturalOrder());// [1, 2, 5, 8, 9]
numbers.sort(Comparator.reverseOrder());// [9, 8, 5, 2, 1]
```

</details>


<details>
<summary> Generics & Wildcards </summary>

**Generics**: allow creating type-safe classes, interfaces and methods (checked at compile time).


**Unbounded wildcard** (`?`): any type.

```java
public static void printList(List<?> list) {
    for (Object item : list) {System.out.println(item);}
}

List<String> strings = Arrays.asList("A", "B");
List<Integer> ints = Arrays.asList(1, 2);
printList(strings);  // OK
printList(ints);     // OK
```

**Upper bounded wildcard** (`? extends Type`): Type or subtypes.

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

**Lower bounded wildcard** (`? super Type`): Type or supertypes.

```java
// Accepts List<Integer>, List<Number>, List<Object>
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

**PECS Rule**: **Producer Extends, Consumer Super**  
**`Producer`** (reading): use `? extends T`  
**`Consumer`** (writing): use `? super T`  

</details>

<br>

<details>
<summary> Optional </summary>


**Optional** (Java 8+) container that may or may not hold a value, avoiding `NullPointerException`

```java
Optional<String> empty = Optional.empty();
Optional<String> nonEmpty = Optional.of("Hello");
Optional<String> maybeEmpty = Optional.ofNullable(value);  // empty

//------  checking if value present ------ 
if (opt.isPresent()) {System.out.println(opt.get());}
if (opt.isEmpty()) {System.out.println("Empty");}
// if present, call consumer
opt.ifPresent(System.out::println); 

//------ Providing Default Values ------ 
// orElse() - default value
String s1 = empty.orElse("Default");     // "Default"
// orElseGet() - Supplier for default value (lazy)
String s3 = empty.orElseGet(() -> "Computed Default");
// orElseThrow() - throws exception if empty
String s4 = empty.orElseThrow();  // NoSuchElementException
String s5 = empty.orElseThrow(() -> new IllegalStateException("Empty!"))
//------ Transforming Optional ------ 
// map(): transforms value if present
Optional<String> opt = Optional.of("Hello");
Optional<Integer> length = opt.map(String::length);
// flatMap(): transforms into Optional (avoids Optional<Optional>)
public class Person { private Optional<Address> address;}

// map() would give Optional<Optional<Address>>
Optional<Optional<Address>> nested = person.map(Person::getAddress);
// flatMap() gives Optional<Address>
Optional<Address> address = person.flatMap(Person::getAddress);
/*❌ Optional<Address> address = person.map(p -> p.address.get());*/

// filter(): keeps value if it satisfies the predicate
Optional<Integer> opt = Optional.of(42);
Optional<Integer> even = opt.filter(n -> n % 2 == 0);
```
**Primitive Optional**: `OptionalInt`, `OptionalLong`, `OptionalDouble`
```java
IntStream stream = IntStream.of(1, 2, 3, 4, 5);
// Specific terminal methods
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

A **Stream** is a sequence of elements supporting sequential and parallel operations. It doesn't store elements and can only be used once (otherwise IllegalStateException)


**Creating a Stream**:
```java
// From a collection
List<String> list = Arrays.asList("A", "B", "C");
Stream<String> stream1 = list.stream();
// From an array
String[] array = {"A", "B", "C"};
Stream<String> stream2 = Arrays.stream(array);
Stream<String> stream3 = Stream.of("A", "B", "C");
// Empty stream
Stream<String> empty = Stream.empty();
// Infinite stream
Stream<Integer> infinite1 = Stream.iterate(0, n -> n + 2);  // 0, 2, 4, 6...
Stream<Double> infinite2 = Stream.generate(Math::random);
// From a range (primitive streams)
IntStream range = IntStream.range(1, 5);      // 1, 2, 3, 4
IntStream rangeClosed = IntStream.rangeClosed(1, 5);  // 1, 2, 3, 4, 5
```
- ✅ **Intermediate Operations** (return Stream):
   - `filter()`, `map()`, `mapToObj()`, `flatMap()`, 
   - `distinct()`, `sorted()`, `limit()`, `skip()`
   - `peek()`
- ✅ **Terminal Operations** (return result):
   - `forEach()`, `count()`, `collect()`
   - `reduce()`, `min()`, `max()`
   - `anyMatch()`, `allMatch()`, `noneMatch()`
   - `findFirst()`, `findAny()`
- ✅ **Primitive Streams**:
   - `IntStream`, `LongStream`, `DoubleStream`
   - Methods: `sum()`, `average()`, `min()`, `max()`, `count()`
   - `range()`, `rangeClosed()`
   - `mapToInt()`, `mapToLong()`, `mapToDouble()`, `boxed()`
- ✅ **Collectors**:
   - `toList()`, `toSet()`, `toMap()`, `toCollection(TreeSet::new)`
   - `joining()`, `counting()`, `summingInt()`, `averagingInt()`
   - `groupingBy()`, `partitioningBy()`
   - `filtering()`, `flatMapping()` (Java 9+)
   - `teeing()` (Java 12+)

</details>

<br>

<details>
<summary> Exceptions </summary>

An **Exception** is an event that disrupts the normal flow of program execution.

**Exception Hierarchy**

```java
// Throwable (root)
//   ├── Error (system errors, unrecoverable)
//   │     └── OutOfMemoryError, StackOverflowError...
//   └── Exception (application errors, recoverable)
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

A **suppressed exception** is an exception that gets hidden (attached) to another main exception.
This mostly happens with try-with-resources.

```java
// to catch suppressed exception
catch (Exception e) {
    for (Throwable t : e.getSuppressed()) {
        System.out.println(t);
    }
}
```

- ✅ Exceptions
  - **Checked exceptions** must be declared (`throws`) or caught
  - **Unchecked exceptions** (RuntimeException) are optional
  - **Multi-catch** with `|` for multiple exceptions
  - **Finally** always executes, even with return or exception
  - **Try-with-resources** automatically closes AutoCloseable resources

</details>


<details>
<summary> Localization, Formatting </summary>

- ✅ Formatting
  - **NumberFormat**: numbers, currencies, percentages
  - **DateFormat**: short, medium, long dates
  - **SimpleDateFormat**: custom format (not thread-safe)
  - **DateTimeFormatter**: Java 8+ format (thread-safe)
- ✅ Internationalization
  - **Locale**: language + country (e.g., fr_FR)
  - **ResourceBundle**: load localized messages
  - **Fallback mechanism**: looks for specific locale → general → default
  - **MessageFormat**: parameterize messages with {0}, {1}...

</details>

<br>

<details>
<summary> Threads </summary>



🧠 Process vs Thread  
`Thread`: a unit of execution inside a process, shares the same memory, lighter than a process  
`Process`: a running program, has isolated memory, each process has its own resources  


**Thread types**  
`User thread` - normal thread  
`Daemon thread` - background thread (dies with the program)  



✅ Threads  
`Thread`: create with Runnable or extends Thread  
`start()` vs `run()`: start() creates a new thread, run() executes in the current thread  
`Callable`: like Runnable but returns a value  


**Thread Lifecycle**

```java
// Thread states
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
<summary> Concurrency API </summary>

**ExecutorService**

```java
// ExecutorService = thread pool manager
// Benefits: Reusable, Automatic management, More efficient than new Thread()

//--------  Create ExecutorService
// 1. Single thread
ExecutorService single = Executors.newSingleThreadExecutor();
// 2. Fixed thread pool
ExecutorService fixed = Executors.newFixedThreadPool(5);  // 5 threads
// 3. Cached thread pool (unlimited)
ExecutorService cached = Executors.newCachedThreadPool();
// 4. Scheduled thread pool
ScheduledExecutorService scheduled = Executors.newScheduledThreadPool(3);

//--------  Submit Tasks
ExecutorService executor = Executors.newSingleThreadExecutor();
// execute() - fire and forget (void)
executor.execute(() -> {System.out.println("Task executed");});
// submit() - returns Future
Future<?> future = executor.submit(() -> {
    System.out.println("Task submitted");
});
// submit() with Callable (returns result)
Future<Integer> result = executor.submit(() -> 42);

//--------  Get the result
try {
    Integer value = result.get(/*2, TimeUnit.SECONDS*/); // Blocking
    System.out.println("Result: " + value);  // 42
} catch (InterruptedException | ExecutionException e) {
    e.printStackTrace();
}
// IMPORTANT: Shutdown
executor.shutdown();  // Stops after current tasks
// executor.shutdownNow();  // Stops immediately

//-------- invokeAll and invokeAny
ExecutorService executor = Executors.newFixedThreadPool(3);
List<Callable<String>> tasks = Arrays.asList(
    () -> { Thread.sleep(1000); return "Task 1"; },
    () -> { Thread.sleep(500); return "Task 2"; },
    () -> { Thread.sleep(2000); return "Task 3"; }
);
// invokeAll() - runs all, waits for all
List<Future<String>> results = executor.invokeAll(tasks);
for (Future<String> future : results) {
    System.out.println(future.get());  // Task 1, Task 2, Task 3
}
// invokeAny() - returns the first one to finish
String first = executor.invokeAny(tasks);
System.out.println(first);  // "Task 2" (the fastest)

```

</details>

<details>
<summary> Thread Safety & Problems</summary>

**✅ Thread Safety Problems**  
`🔒 Deadlock` is when two threads block each other because each one is waiting for a resource held by the other.  
👉 Result: nobody moves forward.

`🍽️ Starvation` is when a thread never gets the resources it needs, because other threads are always prioritized.  
👉 It's not technically blocked, but it never makes progress.  

`🔄 Livelock`  
Livelock is when threads are active but make no progress.  
👉 In a deadlock, threads are blocked. In a livelock, they keep moving but don't advance.  

`⚠️ Race condition`: when the result depends on the execution order of threads.  
If accesses aren't synchronized, you can get incorrect or unpredictable results.  

**✅ Thread Safety**  
`synchronized`: Lock on (method or block or this)  
`Lock`: More flexible than synchronized (Lock lock = new ReentrantLock();)  
  ``` java
      public void increment() {
          lock.lock();
          try { count++;}
          finally {lock.unlock();}
      }

      // tryLock() - non-blocking
      public boolean tryIncrement() {
          if (lock.tryLock()) {
              try { count++; return true;}
              finally { lock.unlock();}
          }
          return false;  // Lock not available
      }
  ```
`Atomic classes`: Thread-safe without locks (AtomicInteger, etc.)  
`volatile`: Guarantees visibility (not atomicity; reads/writes go directly to memory; no caching)  


**✅ Concurrent Collections**  
`ConcurrentHashMap`: Thread-safe HashMap  
`CopyOnWriteArrayList`: Thread-safe for read-heavy workloads  
`BlockingQueue`: Queue with blocking (put/take)  

**✅ Parallel Streams**  
`parallelStream()`: Parallel processing  
`Use when`: Large collection + CPU-intensive  
`Avoid when`: Small collection, I/O, mutable state  
`forEachOrdered`: Preserves order in parallel  

</details>

<br>

<details>
<summary> I/O</summary>

- ✅File vs Path
  - **File**: Legacy (Java 1.0), less functional
  - **Path**: Modern (Java 7+), more operations, better error handling
- ✅Streams
  - **Byte Streams**: InputStream/OutputStream (binary data)
  - **Character Streams**: Reader/Writer (text, encoding)
  - **Buffered Streams**: BufferedReader/Writer (performance)
  - **Always close**: try-with-resources
- ✅Files Class
  - **Files.readAllLines()**: Reads everything into memory
  - **Files.lines()**: Lazy stream (large files)
  - **Files.walk()**: Recursive traversal
  - **Files.copy/move/delete**: File operations
- ✅Serialization
  - **Serializable**: Marker interface
  - **transient**: Fields not serialized
  - **serialVersionUID**: Version control
  - **ObjectInputStream/ObjectOutputStream**: Serialization
- ✅Console vs Scanner
  - **Console**: Password masking, null in IDE
  - **Scanner**: More flexible, type parsing

</details>

<br><br>

# ✅Spring

<details>


<summary> DI , IOC , Annotations </summary>

**✅Dependency Injection**: It's a design pattern where dependencies are passed into a class rather than created inside it. It reduces tight coupling and makes testing much easier.  
**✅Inversion of Control (IoC)**: IoC means that Spring manages the creation and lifecycle of objects (beans) instead of the developer manually instantiating them with new.  
**✅Bean (Spring Bean)**:  object managed by the Spring container. Spring handles its creation, injection, and lifecycle.  
**✅Tight Coupling vs Loose Coupling**  
`Tight coupling` is when a class directly depends on a concrete implementation. if implementation changes, you have to modify the code.  
`Loose coupling` class depends on `interfaces` rather than implementations. This makes the code `more flexible`, `easier to test`.   
**✅Main Annotations**    
`@Configuration`: Marks a class as Spring configuration, Contains @Bean methods to define beans  
`@Bean`: Declares that a method produces a bean managed by Spring, Used in @Configuration classes  
`@Component`: Marks a class as a Spring component, Automatically detected by @ComponentScan  
`@ComponentScan`: Enables automatic component scanning, Looks for @Component..., registers found beans  
`@Autowired`: Automatic dependency injection by Spring, Can be used on constructors, setters, fields  
`@Primary`: Gives priority to a bean when multiple beans with same type exist to resolve ambiguities  
`@Qualifier`: Specifies which bean to inject when multiple beans with sameType, More precise than @Primary  
`@Lazy`: Bean created only on first use, By default : beans created at startup (eager)  
`@Scope`: SINGLETON( instance per IoC Container), PROTOTYPE, REQUEST, SESSION, APPLICATION  
`@PostConstruct`: Method executed after dependency injection  
`@PreDestroy`: Method executed before bean destruction (Used for resource cleanup·)  
**🎯when to use @Component vs @Bean**  
`@Component`: you control your own class (sevices, Repositories, Controllers)  
`@Bean`: Third-party classes, Complex config needed (DataSource, RestTemplate, ObjectMapper)  

</details>


<details>


<summary> (Spring vs Boot vs MVC) & Spring Boot Starters & Auto-configuration </summary>

**✅Spring vs Spring Boot vs Spring MVC**  
`Spring` is the base framework that provides IoC, DI, and the ecosystem.  
`Spring MVC` is a Spring module dedicated to building web APIs and REST apps.  
`Spring Boot` simplifies Spring: it provides auto-configuration, an embedded server, and allows you to start an application quickly with minimal configuration.  
**✅Spring Boot Starters**     
Starters are “all-in-one” dependencies that automatically include all the libraries needed for a specific use case.
`Advantages`: `Guaranteed compatibility between dependencies`, `No conflicts`, `Fewer configuration errors`  
**spring-boot-starter-xxxxx**  
`web`: automatically includes: Spring MVC (for REST APIs and web), Embedded Tomcat(server), Jackson(JSON serialization), Validation, logging  
`data-jpa`: Spring Data JPA, Hibernate (ORM), JDBC)  
`security`: Spring Security Core, Authentication, Authorization, Session Management  
`test`: JUnit 5, Mockito, AssertJ  
`actuator`: Health checks, Metrics, Application info, Env properties  
**✅Auto-configuration**  
allows Spring Boot to automatically configure the application based on the dependencies present in the classpath.  
`example`: if SpringBoot sees a JPA dependency, it automatically configures the datasource & Hibernate.  

</details>


<details>

<summary> Spring JDBC, &nbsp;JPA, &nbsp;Hibernate & Auditing</summary>

**✅ Spring JDBC**  
simplifies database access by eliminating traditional JDBC boilerplate code (connection management, ResultSet, exceptions).  
With `JdbcTemplate` (Automatic connection/exception management, Less boilerplate code, No manual resource closing)  
**✅ Spring JPA**  
JPA is a Java specification for ORM. It defines how to map Java objects to database tables.  
EntityManager (persist, merge, createQuery, remove)  
**✅ Spring Data JPA**   
Spring Data JPA adds an abstraction layer on top of JPA, eliminating even the need to write queries for basic CRUD operations.  
`public interface CourseRepository extends JpaRepository<Course, Long> {}`
`save(entity)`, `saveAll(entities)`, `findById(id)`, `findAll()`, `findAllById(ids)`, `count()`, `existsById(id)`, `deleteById(id)`, `delete(entity)`, `deleteAll()`, `findAll(Pageable pageable)`, `findAll(Sort sort)`  
✅ Query Methods - Automatic Generation, example ( List<Course> findByName(String name)  
✅ @Query - Custom Queries
```java
@Query("SELECT c FROM Course c WHERE c.author = :author")
List<Course> findCoursesByAuthor(@Param("author") String author);
```
**✅ Auditing**
Auditing allows you to automatically track who created/modified an entity and when, without manual code in each service.  
`@EnableJpaAuditing` in config and `@EntityListeners(AuditingEntityListener.class)`
columns: `@CreatedDate`, ` @CreatedBy`, `@LastModifiedDate`, `@LastModifiedBy`

</details>


<details>


<summary> Controllers(ResponseEntity, httpCodes, preAuth, CORS, ContentNegotiation)</summary>

**✅ResponseEntity**: wrapper that allows you to fully control the HTTP response: status code, headers, and body.  
EX: `ResponseEntity.ok()`, `ResponseEntity.created(location).body(created)`, `ResponseEntity.noContent().build()`  
**✅HTTP Status Codes**  
`2xx Success`: (`200 OK` - Request successful), (`201 CREATED` - Resource created), (`202 ACCEPTED` - Request accepted), (`204 NO CONTENT` - Success with no content to return)  
`3xx Redirection`  
`4xx Client Errors`: (`400 BAD REQUEST` - Invalid request), (`401 UNAUTHORIZED` - Not authenticated), (`403 FORBIDDEN` - Authenticated but not authorized), (`404 NOT FOUND` - Resource not found), (`409 CONFLICT` - Conflict), (`429 TOO MANY REQUESTS` - Rate limiting)  
`5xx Server Error` (`500 INTERNAL SERVER ERROR`), (5`03 SERVICE UNAVAILABLE`)  
**✅preAuth**  
`@PreAuthorize (("hasAuthority('USER_WRITE')"), ("hasRole('ADMIN')"))` is a Spring Security annotation to control access to a method BEFORE its execution   
`@PostAuthorize` (Control after execution)  
`@Secured` @Secured("ROLE_ADMIN") (Simple alternative)  
**✅cors**: `Cross-Origin Resource Sharing` - A browser security mechanism that controls HTTP calls to a different origins(domain, port, protocol).  
`CORS per Controller/Endpoint` (@CrossOrigin(origins = "http://localhost:3000")) or in `global config` (.allowedOrigins("http://localhost:3000", "https://myapp.com")))  
**✅Content Negotiation**:  allows the server to return different formats (JSON, XML, CSV) based on what the client requests via the Accept header

</details>

<details>


<summary> Validation</summary>
allows you to check that data received in an application (via REST API or forms) meets certain constraints before processing it.
`EX`: @NotNull, @NotEmpty, @NotBlank, @Email, @Pattern, @regex, @Past, @Size, @Min

| @Valid | @Validated |
|--------|------------|
| Standard Java (JSR-380) | Spring-specific |
| No support for groups | Supports **validation groups** |
| Used on method parameters | Can be used at the class level |
| Standard validation | Advanced validation with groups |
| MethodArgumentNotValidException | ConstraintViolationException |
`Without a custom handler`: Spring automatically returns `400 Bad Request` with errorDetails  
**✅Group Validation**: Allows applying different validations depending on the context
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

`@ResponseStatus`: Associates HTTP status code with a specific exception
`@ControllerAdvice`: Creates global exceptionHandler for entire application
`@ExceptionHandler`: Handles specific exceptions thrown by controller methods

🔹1. Create custom exceptions:
```java
@ResponseStatus(code = HttpStatus.NOT_FOUND)
public class ResourceNotFoundException extends RuntimeException {/**/}
```

🔹2. Create an ErrorDetails class (DTO for errors):
```java
public class ErrorDetails {
    private Date timestamp;
    private String message, details;
```

🔹3. Create a Global Exception Handler with @ControllerAdvice:
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

SpringBoot Actuator provides production-ready features to monitor and manage a SpringBoot app. It exposes HTTP endpoints that provide information about the app's state and metrics.

`/actuator` : Lists all available endpoints  
`/actuator/health` : Application health status (UP/DOWN)  
`/actuator/info` : Application metadata (version, description)  
`/actuator/metrics` : Performance metrics (JVM, HTTP requests, database)  
`/actuator/env` : Environment properties and configuration  
`/actuator/beans` : Complete list of Spring beans  
`/actuator/mappings` : All Request Mappings (REST/MVC endpoints)  
`/actuator/loggers` : Log level management  
`/actuator/shutdown` : Allows shutting down the application (must be enabled)  
</details>

<details>


<summary> AOP</summary>

It's an approach that allows you to separate cross-cutting concerns from business code, such as logging, security, or transaction management.  
For example, for logging or transactions, you can use an aspect that runs before or after a method, without polluting the business code.  
`Cross-Cutting Concerns`: Features that apply to multiple layers of the app  
**✅1. Aspect**: Class that contains the cross-cutting concerns code, `Aspect = Advice + Pointcut`  
**✅2. Advice**: The code to execute , Ex: logging logic, authentication logic  
`Types`: @Before, @After, @AfterReturning, @AfterThrowing, @Around  
**✅3. Pointcut**: Expression that identifies the methods to intercept(Defines when the advice should be executed)  
`Ex`: ` @Pointcut("execution(* com.example.service.*.*(..))")`  
**✅4. Join Point**: Execution point in the application (method call, exception, etc.) (Where the aspect can be applied)  
**✅5. Weaving**: Process of linking aspects to the application code Can be done at compile-time, load-time, or runtime  

✅**Main Annotations**  
`@Before` Runs before the method call  
`@After` Runs after the method (success OR exception)  
`@AfterReturning` Runs only if the method succeeds  
`@AfterThrowing` Runs only if the method throws an exception  
`@Around` Runs before AND after the method (more powerful)  
`@Pointcut` Defines a reusable pointcut expression  
✅**JoinPoint vs ProceedingJoinPoint**  
`JoinPoint`: Used with @Before, @After, @AfterReturning, @AfterThrowing🔹 Provides info about the intercepted method 🔹Cannot control the method execution  
`ProceedingJoinPoint`: Used only with @Around🔹 Extends JoinPoint🔹Allows controlling execution with proceed()  

</details>


<details>


<summary> Spring Security</summary>

With **`Spring Security`**, endpoints are protected by defining **access rules** based on authentication and roles.  

**Spring Security** works with a chain of Security Filters that intercept every HTTP request. The main components are :  
`SecurityFilterChain`: specify which URLs are public and which require authentication or a specific role.
`AuthenticationManager`: Manages the authentication process
`UserDetailsService`: Loads user information from the data source
`PasswordEncoder`: Encodes/verifies passwords

1️⃣ Maven Dependency `spring-boot-starter-security`  
2️⃣ Spring Security Configuration (basic)
```java
@Configuration @EnableWebSecurity public class SecurityConfig {
  @Bean
  public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
    http
      .csrf(csrf -> csrf.disable()) // often disabled for REST APIs
      .authorizeHttpRequests(auth -> auth
          .requestMatchers("/public/**").permitAll()
          .requestMatchers("/admin/**").hasRole("ADMIN")
          .anyRequest().authenticated()
      )
      .httpBasic(); // or JWT / OAuth2 depending on the case
    return http.build();
    }
}
```
3️⃣ Annotation-based security (fine-grained)
```java
@PreAuthorize("hasRole('ADMIN')")
@GetMapping("/admin/secure")
public String secureAdminEndpoint() {/*...*/}
```

</details>

<br>

<details>

<summary> Spring Cloud</summary>

**Spring Cloud** is a set of frameworks built on top of Spring Boot to simplify the development of distributed systems and microservice architectures.  
It provides solutions for centralized configuration, service discovery, inter-service communication, resilience, and load balancing.  
`Spring Cloud Netflix Eureka`: allows a service to find the address of another service (Eureka Server (Service Registry) :  Eureka Client (Service that registers)  
` Spring Cloud LoadBalancer`: Load Balancing  
`Spring Cloud Gateway`: API Gateway (Single entry point)  
`Spring Cloud Config`: Centralized Configuration  
`Spring Cloud Sleuth`:  Distributed Tracing  
</details>

<details>

<summary> Configuration & Spring Cloud Config</summary>

**✅ Reading configurations**  
🔹1 @Value (for simple properties)  
🔹2 @ConfigurationProperties(prefix = "accounts") (for structured configs)  
🔹3 Environment Interface (environment.getProperty("build.version"))  
  
**✅ Precedence Order**  
🔹1. **Command line arguments**: `java -jar app.jar --server.port=9090`  
🔹2. **JVM system properties**: `java -Dserver.port=9090 -jar app.jar`  
🔹3. **OS environment variables**: `export SERVER_PORT=9090`  
🔹4. **application-{profile}.yml** (active profile)  
🔹5. **application.yml** (main file)  
🔹6. **@PropertySource**  
🔹7. **Default properties**  

**✅ Spring Cloud Config Server** provides a solution to externalize and centralize the configs of multiple microservices.  
`Config Server`: Centralized server that stores configs  
`Config Clients`: Microservices that fetch their configs from the ConfigServer  
**`==> Config Sources`**: Classpath, Filesystem, Git Repo   

**✅ Refresh Configurations at Runtime (3 approachs)**  
🔹1: Refresh Actuator (manual for each ms) `http://{URL}/actuator/refresh`  
🔹2: Spring Cloud Bus + RabbitMQ (automatic) `http://localhost:8080/actuator/busrefresh`
🔹3: Spring Cloud Bus + GitHub Webhook (fully automatic)  

</details>

<details>

<summary> SYNCHRONOUS-Inter-service-Comm</summary>

`🟦 RestTemplate`: is the old Spring HTTP client.  
It is blocking and easy to use, but now deprecated.  
It is mostly found in legacy projects.  
`🟦 OpenFeign`: is a declarative HTTP client used mainly in microservices.  
We define an interface, and Spring generates the implementation.  
It simplifies service-to-service communication.  
`🟦 RestClient`: is the modern replacement for RestTemplate in Spring 6.  
It uses a fluent API and is still blocking.  
It is recommended for simple HTTP calls in new projects.  
`🟦 WebClient`: is a reactive and non-blocking HTTP client.  
It is more scalable and suited for high-concurrency systems.  
It requires knowledge of reactive programming.  
`🟦 HTTP Interfaces`: is a modern declarative approach in Spring 6.  
We define an interface with annotations, and Spring creates the client automatically.  
It is clean and recommended for modern applications.  

</details>

<details>


<summary> ASYNCHRONOUS-Inter-service-Comm (rabbitMq, Kafka)</summary>

**✅RabbitMQ**: traditional message broker based on the AMQP (Advanced Message Queuing Protocol) protocol. It acts as an intermediary that receives, stores, and distributes messages.  
**🟦 Producer**  
**🟦 Exchange**  
`Direct Exchange`: Routing based on an exact key(routing key)  
`Fanout Exchange`: Broadcasts to all bound queues  
`Topic Exchange`: Pattern-based routing  
`Headers Exchange`: Routing based on message headers  
**🟦 Queue**  
Stores messages waiting to be consumed (FIFO)  
Can be durable (survives restarts) or temporary  
**🟦 Binding**  
Connects an Exchange to a Queue with a routing key  
**🟦 Consumer**  
Application that reads and processes messages  
Can acknowledge (ACK) or reject (NACK) a message  

**✅Kafka**: Apache Kafka is a high-performance distributed streaming platform, designed for large-scale event-driven architectures.
| Criteria | RabbitMQ | Kafka |
|---------|----------|-------|
| **Messaging model** | Traditional queue (AMQP) | Distributed log / Event Stream |
| **Throughput** | ~20k msg/sec | ~1M+ msg/sec |
| **Latency** | <1 ms | ~5-10 ms |
| **Persistence** | Optional | By default (all messages) |
| **Retention duration** | Until consumption + ACK | Configurable(7 days, 1 month) |
| **Consumption** | Message deleted after ACK | Message kept, offset advances |
| **Replay** | ❌ Not possible | ✅ Possible |
| **Routing** | Complex(exchange, bindings) | Simple (partition key) |
| **Use cases** | Task queues, RPC, workflows | Event sourcing, logs, analytics, streaming |
</details>


<details>

<summary> Service Discovery </summary>

**✅ service discovery**: is a mechanism that allows microservices to dynamically locate each other in a distributed environment.  
`CSSD (Eureka)`: means the client is responsible for determining the IPs of available service instances and load balancing between them.  
`SSSD (Kubernetes)`: means an infrastructure manages discovery and routing. Clients are unaware of this mechanism.  
**✅ Eureka - Architecture**  
`Eureka Server`: Centralized registry that maintains the list of all services🔹Services register themselves at startup🔹Send regular heartbeats (every 30s)  
`Eureka Client`: Integrated into each microservice🔹 Fetches the registry and caches it locally  
✔️ `Autonomy`: Clients can work even if Eureka goes down(thanks to localCache)  
✔️ `Flexible load balancing`: Customizable algorithms on the client side  
❌ `Limited polyglot`: Difficult to integrate non-JVM services  
❌ `App dependency`: Each service must integrate the Eureka client  
**✅ Kubernetes - Architecture**  
load balancing managed by services that Provides DNS names and fixed ClusterIPs, routing managed by ingress based on hostname/path, it requires an Ingress Controller(Nginx)  
✔️ `Polyglot`: Works with any language/framework  
✔️ `Native health checks`: liveness/readiness probes  
✔️ `Zero downtime deployments`: Native rolling updates  
❌ `Heavy infrastructure`: Requires a K8s cluster (cost, maintenance)  
❌ `Difficult debugging`: More abstraction layers  
</details>


<details>


<summary> API Gateway (Spring Cloud Gateway) </summary>

> is a single entry point that intercepts all requests to apply processing before routing to the appropriate service.
> it can Transform a Request/Response, implement Authentication/Authorization, Rate Limiting, Retry Logic, Circuit Breaker...etc

</details>

<details>

<summary> Fault Tolerance (Resilience4j)</summary>

**✅Resilience4j**: helps protect your application from service failures using patterns like circuit breaker and retry.  
**🟦 Circuit Breaker**  
The Circuit Breaker is a pattern that prevents repeated calls to a failing service. It works like an electrical circuit breaker with 3 states:  
`CLOSED`: Everything works normally, requests go through  
`OPEN`: The service is detected as failing, requests are immediately rejected without calling the service  
`HALF_OPEN`: Testing phase to check if the service has recovered  
**🟦 Retry Pattern**  
The Retry Pattern automatically attempts to re-execute a failed operation.  
EX: Transient errors (network timeout, temporarily unavailable service) 5xx  
**🟦 Rate Limiting**  
Protects services against overloads and abuse/attacks (scraping, DDoS)  
Guarantees fair quality of service between users  
Avoids overloading backend resources  

</details>

<br>

<details>

<summary> Versioning Strategies, Swagger, HATEOAS, HAL, i18n</summary>


**✅Versioning Strategies**  
🔹1. Versioning through URI Path  
🔹2. Versioning through Query Parameters  
🔹3. Versioning through Custom Headers  
🔹4. Versioning through Content Negotiation (Accept Header)  
**✅Swagger** : a tool used to document and test REST APIs. It provides a clear interface where you can see and try API endpoints.  
**✅HATEOAS** : is a REST principle where the API response includes links to related actions. It helps clients navigate the API dynamically.  
**✅HAL Explorer** : a tool used to explore and test HATEOAS-based APIs. It provides a simple UI to navigate API links.  
**✅Internationalization (i18n)?** : allows an app to support multiple languages.  



</details>


<details>

<summary> Unit Test, TDD, Junit5, Mockito</summary>

**✅TDD**: is a practice where you first write a failing test, then implement the code to pass it, and finally refactor.
It helps improve code quality and design.

**✅JUnit**  
Main annotations: `@Test`,`@BeforeEach`,`@AfterEach`,`@BeforeAll`,`@AfterAll`,`@DisplayName`,`@Disabled`,`@RepeatedTest(n)`,`@ParameterizedTest`  
Common Assertions: `assertNotNull`, `assertThrows`, `assertIterableEquals`, `assertTimeout`, `assertAll`  

**✅Mockito**  
| Annotation | Description  | Usage |
| -  |  - |  - |
| `@Mock` | Creates a mock (fake object) | Pure unit tests |
| `@Spy`| Creates a spy (real object with spying capability) | Rare partial mocking |
| `@InjectMocks` | Injects mocks into the class under test | Class being tested |
| `@Captor` | Captures arguments passed to methods | Advanced verification |

```java
////////// Mockito Setup
  @Mock private MyRepository repository;
  @InjectMocks private MyService service;
///////// Stubbing (Simulating behavior)
when(repository.findById(1L)).thenReturn(Optional.of(user));
doNothing().when(repository).delete(any());
doThrow(new RuntimeException()).when(repository).delete(any());
///////// Verification (Verifying calls)
verify(repository, times(1)).save(any());
verify(repository, never()).delete(any());
///////// Capture arguments
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
////// Test repository ysing H2
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
**✅Reflection API**: Reflection allows inspecting and manipulating private classes, methods and fields at runtime.


</details>

<details>

<summary> BDD & Cumcumber</summary>

**✅Behavior-Driven Development**is a software development approach where we define the behavior of the app in plain language, before writing the code. The focus is on business behavior, not technical implementation.

**✅Cucumber**: is a testing framework used for BDD. It allows you to write tests in  Gherkin language using Given / When / Then:

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
│  Scenarios in Gherkin (natural language)        │
└─────────────────┬───────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────┐
│  Step Definitions (Java)                        │
│  Code that executes each step                   │
└─────────────────┬───────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────┐
│  Application Code                               │
│  Tested business code                           │
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

**✅Elasticsearch** is a distributed search and analytics engine. used to search large amounts of data very fast and Analyze logs and metrics  
**✅Logstash** is a data processing pipeline tool. It is used to Collect data from multiple sources, Transform/parse data, Send data to another system (Elasticsearch)  
**✅Kibana** is a data visualization and monitoring tool. It is used to Visualize data stored in Elasticsearch, Create dashboards and Search logs  
**✅Filebeat** is a lightweight agent (log shipper) that Reads log files and Sends data to Logstash or Elasticsearch.  
**✅Zipkin** is a distributed tracing tool. used to Track requests across multiple microservices, Measure latency and Identify performance bottlenecks  


**🔹Zipkin Key Concepts**  
`Trace` → Entire request lifecycle  
`Span` → One unit of work (ex: one service call)  
`Trace ID` → Unique ID for the request  
`Span ID` → Unique ID for each operation  
</details>



---

<br>


# ✅TypeScript


<details>
<summary> Typescript </summary>

> TypeScript is a **superset of JavaScript** with static typing, which allows error detection at compile time

</details>

<details>
<summary> tsconfig </summary>

> It's the configuration file that defines TypeScript compilation options: target JS version, paths, strict checking rules, etc

`Auto generation`
```bash
npx tsc --init
```

</details>


<details>
<summary> migration </summary>

> Progressive migration: enable allowJs, create tsconfig.json, convert file by file, gradually tighten the config with strict: true.

***Progressive migration strategy***

**Phase 1: Preparation (without breaking existing code)**

1. **Install TypeScript**
```bash
npm install --save-dev typescript @types/node
```

2. **Create a permissive tsconfig.json**
```json
{
  "compilerOptions": {
    "target": "ES2020",
    "module": "commonjs",
    "outDir": "./dist",
    "rootDir": "./src",
    /* Permissive mode at the start */
    "allowJs": true,              // ✅ Accepts .js files
    "checkJs": false,             // ❌ Does NOT check .js files yet
    "noEmit": true,               // Doesn't generate files
    "strict": false,              // Non-strict mode
    "skipLibCheck": true
  },
  "include": ["src/**/*"],
  "exclude": ["node_modules"]
}
```
3. **Test the compilation**
```bash
npx tsc --noEmit
```

</details>


<details>
<summary> Third-Party Types </summary>


How to type a JavaScript library that has no types 

If a library has no types, create a `.d.ts` file:

**Project structure** :
```
src/
├── types/
│   └── my-library.d.ts
└── index.ts
```

**my-library.d.ts** :
```typescript
// Module declaration
declare module 'my-library-without-types' {
  // Function exports
  export function connect(url: string): Promise<Connection>;
  export function disconnect(): void;
  
  // Interface exports
  export interface Connection {
    // ...
  }
}
```

</details>


<details>
<summary> Types for global libraries </summary>

```typescript
// types/globals.d.ts

// Global variable
declare var MY_GLOBAL_VAR: string;

// Global function
declare function myGlobalFunction(param: number): string;

// Global namespace
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
<summary> Primitive & Built-in Types </summary>

`Primitive Types` : number, string, boolean, any, unknown, void, never (represents something that never happens)  
`Built-in`: Array, Tuple (fixed-size array with specific types), Object, Function, Date, RegExp, Error

</details>

<details>
<summary> Creating Custom Types </summary>

`Interface` for objects and classes (extensible, declaration merging).  
`Type alias` for unions, intersections, primitive types.

**Interface**
An interface defines the structure of an object. It's a contract that objects must follow.


**Type alias**
A type alias creates a new name for a type. Similar to interfaces but more flexible.

**enums**
An enum (enumeration) defines a set of named constants.

**🆚 Enum vs Union Type**

```typescript
// ❌ Enum : generates JavaScript code
enum Color {
  Red = "RED",
  Green = "GREEN",
  Blue = "BLUE"
}
// Generates extra JS code

// ✅ Union Type : no code generated
type Color = "RED" | "GREEN" | "BLUE";
// No JS code generated, just compile-time checking

// When to use Enum:
// - Need reverse mapping
// - Computed values
// - Complex business logic around values

// When to use Union Type:
// - Simple types
// - Performance-critical code
// - Libraries
```
</details>


<details>
<summary> generics & Built-in Utilities </summary>

Generics allow you to create reusable components that work with multiple types rather than just one.

**🎯 Built-in Utility Generics**

TypeScript provides built-in utility generic types:

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
// ===> { id: string; name: string; }

// Omit<T, K> : exclude certain properties
type UserWithoutId = Omit<User, "id">;
// { name: string; email: string; age: number; }

// Record<K, T> : object with keys K and values T
type UserRoles = Record<string, "admin" | "user" | "guest">;
// ===> { [key: string]: "admin" | "user" | "guest" }

// Exclude<T, U> : exclude from union
type Role = "admin" | "user" | "guest";
type NonAdminRole = Exclude<Role, "admin">;
// ===> "user" | "guest"

// Extract<T, U> : extract from union
type AdminRole = Extract<Role, "admin">;
// ===>"admin"

// NonNullable<T> : exclude null and undefined
type MaybeString = string | null | undefined;
type DefinitelyString = NonNullable<MaybeString>;
// ===> string

// ReturnType<T> : function return type
function getUser() {
  return { id: "1", name: "John" };
}
type UserReturnType = ReturnType<typeof getUser>;
// ===> { id: string; name: string; }

// Parameters<T> : parameter types
function createUser(name: string, age: number) {
  return { name, age };
}
type CreateUserParams = Parameters<typeof createUser>;
// ===> [name: string, age: number]

// Type : {...} (without Promise) => if fetchUserData (promise)
type UserData = Awaited<ReturnType<typeof fetchUserData>>;
```

</details>

<br>



<details>
<summary> keyof operator </summary>

> The keyof operator creates a union type of all the keys of a given type.
</details>



<details>
<summary> typeof operator </summary>

> The typeof operator in TypeScript lets you get the type of a value or variable.

⚠️ Important difference : typeof TypeScript (compile-time) ≠ typeof JavaScript (runtime)

**🔧 typeof with as const**

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
//   readonly secondary: "#6c757d";
//   readonly success: "#28a745";
//   readonly danger: "#dc3545";
// }

// Extract the type of values
type ColorValue = typeof COLORS[keyof typeof COLORS];
// "#007bff" | "#6c757d" | "#28a745" | "#dc3545"
```

</details>




<details>
<summary> Limited dynamic types with Record </summary>

> The **Record<K, V>** type creates an object type with keys of type `K` and values of type `V`.

```typescript
type StringDictionary = Record<string, string>;

const translations: StringDictionary = {
  hello: "bonjour",
  goodbye: "au revoir",
  thanks: "merci"
};

// Dynamic addition
translations["welcome"] = "bienvenue";  // ✅ OK
translations["yes"] = 123;              // ❌ Error
```

</details>

<br>

<details>
<summary> Decorators </summary>

> A function that modifies or annotates a class, method, or property without changing its source code. Lets you add behaviors in a declarative way.

**🎨 Types of decorators**

TypeScript supports **5 types** of decorators:

1. **Class decorator** : `@ClassDecorator`
2. **Method decorator** : `@MethodDecorator`
3. **Property decorator** : `@PropertyDecorator`
4. **Accessor decorator** : `@AccessorDecorator`
5. **Parameter decorator** : `@ParameterDecorator`

**💡 Why use decorators?**

**Advantages :**
- ✅ **Separation of concerns** : business logic separated from cross-cutting concerns
- ✅ **Reusability** : same decorator on multiple methods/classes
- ✅ **Readability** : more declarative code
- ✅ **Non-intrusive** : no modification of original code

**Common use cases :**
- 📊 Logging and monitoring
- ⏱️ Performance measurement
- 🔒 Permission checking
- ✅ Data validation
- 💾 Cache and memoization
- 🔄 Automatic retry
- 📝 Metadata (Angular, NestJS)

</details>


---

<br>

# ✅ RxJs


<details>
<summary> RxJS & observable & Subscription </summary>

**RxJS**
> RxJS (Reactive Extensions for JavaScript) is a library for reactive programming using Observables. It makes it easy to handle asynchronous data streams and events

**Observable vs Promise:**
| Aspect | Promise | Observable |
|--------|---------|------------|
| **Values** | 1 single value | 0, 1 or multiple values |
| **Lazy** | ❌ No (runs immediately) | ✅ Yes (runs on subscription) |
| **Cancellable** | ❌ No | ✅ Yes (unsubscribe) |
| **Operators** | then, catch | map, filter, merge, etc. (100+) |
| **Multicast** | ❌ No | ✅ Yes (with Subjects) |

`Observable`: A data stream that can emit 0, 1 or multiple values over time.  
`Observer`: An object that defines how to react to emitted values.  
`Subscription`: Represents the execution of an Observable. Allows you to **unsubscribe**.  
`Operators`: Functions that transform, filter, or combine Observables.  
`Subject`: It's both an Observable AND an Observer. Allows **multicast**.  
</details>


<details>
<summary> Creation Operators (Observables (of, from, interval..) & Subjects (BehaviorSubject...))</summary>

✅ `of()` : Emit values directly  
✅ `from()` : Convert arrays, Promises, iterables  
✅ `interval()` : Emit at regular intervals  
✅ `timer()` : Initial delay then interval  
✅ `fromEvent()` : DOM events  


**Subjects**
A Subject is both an Observable and an Observer.

✅ `Subject` : Basic multicast  
✅ `BehaviorSubject` : Keeps the last emitted value and sends it immediately to new subscribers.  
✅ `ReplaySubject` : Keeps the last N values and replays them for new subscribers.  
✅ `AsyncSubject` : Emits only the last value when the Subject completes.  


| Type | Initial Value | Replay | On Subscription |
|------|----------------|--------|-------------------|
| **Subject** | ❌ No | ❌ None | Nothing |
| **BehaviorSubject** | ✅ Yes (required) | ✅ Last value | Last value |
| **ReplaySubject** | ❌ No | ✅ Last N values | Last N values |
| **AsyncSubject** | ❌ No | ✅ Last (if complete) | Nothing until complete |
</details>


<details>
<summary> Transformation Operators (Basic, Flattening, Accumulation)</summary>

**Basic Operators**  
✅ `map()` :  transforms each emitted value by applying a function.  
✅ `mapTo()` : Map to a constant  
✅ `pluck()` : Deprecated, use map() instead  

**Flattening Operators**: transform an Observable of Observables into a simple Observable.  
✅ `mergeMap()` : Parallel, all values processed  
✅ `switchMap()` : Cancels the previous one (search, navigation)  
✅ `concatMap()` : Sequential, order guaranteed (waits for the previous Observable to finish before starting the next)  
✅ `exhaustMap()` : Ignores while running (buttons)  

    When to use which operator?
    - switchMap() -> Search/Autocomplete (debounceTime)
    - mergeMap() -> Independent requests
    - concatMap() -> In-order operations
    - exhaustMap() -> Avoid double-submit

    
    🆚 map() vs switchMap() ?
      🔹 map() transforms a value
      🔹 switchMap() transforms a value into a new Observable

**Accumulation**  
✅ `scan()` : applies an accumulation function on each value (like reduce but emits at each step)  
✅ `reduce()` : like scan(), but emits only the final value (after completion)  
✅ `toArray()` : collects all values and emits them as an array on completion  

</details>



<details>
<summary> Filtering Operators (Filtering, Time Management, Deduplication) </summary>

**Value Filtering**<br>
✅ `filter()` : Filtering condition<br>
✅ `take(n)` : Take N values<br>
✅ `takeUntil()` : Stop with an external signal (unsubscribe pattern)<br>
✅ `takeWhile()` : Continue while condition is true<br>
✅ `skip(n)` : Skip the first N values<br>

**Time Management**<br>
✅ `debounceTime()` : Search, validation (wait for silence)<br>
✅ `throttleTime()` : Scroll, resize (limit frequency)<br>
✅ `auditTime()` : Last value of each period<br>
✅ `sampleTime()` : Sampling at fixed intervals<br>

**Deduplication**<br>
✅ `distinct()` : Unique values (entire history)<br>
✅ `distinctUntilChanged()` : Changes only (vs previous)<br>
✅ `distinctUntilKeyChanged()` : Comparison by property<br>

</details>



<details>
<summary> Error Handling & Finalization </summary>


**Error Handling**<br>
✅ `catchError()` : Intercepts and recovers from errors<br>
✅ `retry()` : Automatically retries N times<br>
✅ `retryWhen()` : Retry with custom logic<br>
✅ `throwError()` : Create an Observable error<br>

**Finalization**<br>
✅ `finalize()` : Guaranteed cleanup (always runs)<br>
✅ `tap()` : Side effects without modifying the stream<br>

**Multiple error handling approaches**

**1. In subscribe() :**
```typescript
obs$.subscribe({
  next: v => console.log(v),
  error: e => console.error('Error:', e)
});
```

**2. With catchError() :**
```typescript
obs$.pipe(
  catchError(err => {
    console.error(err);
    return of([]); // Fallback value
  })
).subscribe(/* ... */);
```
**3. With retry() :**
```typescript
obs$.pipe(
  retry(3),  // Retry 3 times
  catchError(err => of([]))
).subscribe(/* ... */);
```

</details>

---

<br>


# ✅ Angular

<details>
<summary> Project Architecture </summary>

> ├── `angular.json` Main Angular CLI config (build, serve, assets, styles…)  
> ├── `node_modules` Installed npm dependencies (auto-generated)  
> ├── `package.json` npm file: dependencies, scripts, project info  
> ├── `package-lock.json` Locks the exact versions of dependencies  
> ├── `public` Static files copied as-is into the build  
> │   └── `favicon.ico` Icon shown in the browser tab  
> ├── `src` Main source code of the application  
> │   ├── `app` Main folder of the Angular application  
> │   │   ├── `app.ts` Root component of the application  
> │   │   ├── `app.config.ts` Global configuration (providers, router, interceptors…)  
> │   │   └── `app.routes.ts` Route definitions (navigation)  
> │   ├── `index.html` Main HTML file loaded by the browser  
> │   ├── `main.ts` Application entry point (Angular bootstrap)  
> │   └── `styles.css` Global application styles  
> ├── `tree.txt` Generated text file to display the folder tree  
> ├── `tsconfig.app.json` TypeScript config for the application  
> ├── `tsconfig.json` Global TypeScript config for the project  
> └── `tsconfig.spec.json` TypeScript config for unit tests  

</details>


<details>
<summary> Component </summary>

A component is a reusable building block of the application made up of:
- The **content** (HTML)
- The **style** (CSS/SCSS)
- The **behavior** (TypeScript) (required)
- The **tests** (Spec)

example `app.ts`
```typescript
import { Component } from '@angular/core';
import { CollectionItemCard } from './components/collection-item-card/collection-item-card';

@Component({
  selector: 'app-root',
  imports: [CollectionItemCard],  // ⭐ Important !
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss'
})
export class AppComponent {
}
```

</details>


<details>
<summary> Inputs </summary>

An input lets you pass values from a parent component to a child component.

**Parent Component (app.component.html)**
```html
<!-- Without brackets = string -->
<app-collection-item-card name="Hero"></app-collection-item-card>
<!-- With brackets = TypeScript expression -->
<app-collection-item-card [price]="20"></app-collection-item-card>
<img [src]="item().image" alt="Item" />
```

```typescript
item = input<CollectionItem>();  // Explicit type
// InputSignal is the type returned by input()
item: InputSignal<CollectionItem> = input<CollectionItem>();
// required
item = input.required<CollectionItem>();
```
**Transform the Value**
```typescript
item = input.required<CollectionItem, CollectionItem>({
  transform: (value: CollectionItem) => {
    value.price = value.price * 1.17;  // Convert € -> $
    return value;
  }
});
```
```typescript
input.required<ReturnType, InputType>({
  transform: (value: InputType) => ReturnType
});
```
</details>


<details>
<summary> Outputs </summary>

`Output` : Emit events from the child to the parent  
`OutputEmitterRef` : Type for outputs  
`output()` : Function to create an output  
`emit()` : Method to send an event  
`model()` : Combines input + output (two-way binding)  
`$event` : Variable containing the emitted value  

```typescript
export class SearchBar {
  searchButtonClicked: OutputEmitterRef<void> = output<void>();
  searchChange = output<string>(); // output with data


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
<summary> FormsModule and ngModel </summary>

```typescript
@Component({
  imports: [FormsModule],  // ⭐ Required for ngModel
})
export class SearchBar {
  search = input("Initial");
}
```

**Binding with ngModel**

```html
<!-- - **[ngModel]** : Binds the displayed value (Input) -->
<!-- - **(ngModelChange)** : Detects changes (Output) -->
<input 
  [ngModel]="search()" 
  (ngModelChange)="updateSearch($event)"
/>
```

**Two-Way Binding** 🔄 Automatic sync between the view (HTML) and the model (TS)

```html
<!-- Input : propertyName -->
<!-- Output : propertyNameChange -->
<app-search-bar 
  [(search)]="search"
></app-search-bar>
```

**model**: automatically combines an `input` and `output` with the name `propertyChange`

```typescript
  // search: InputSignal<string>; searchChange: OutputEmitterRef<string> 
  search = model("Initial");
```

</details>


<details>
<summary> Signals (Angular 16+) & computed & effect</summary>

Reactive value that automatically notifies Angular of changes  
`signal()` : Create a mutable value  
`computed()` : Create a derived value that depends on other signals  
`effect()` : Run code automatically when signals change  

**Modifying a Signal**: `set()` : Full new value, `update()` : Transformation function

`computed()`: creates a derived signal that automatically recalculates when its dependencies change.  
`effect()` runs code automatically when the signals it uses change.  

</details>


<details>
<summary> Zone Detection</summary>

`Zone.js`vs `Zoneless` : Two approaches to change detection  
`OnPush Strategy` : Optimized detection strategy  

**1) Zone.js**: intercepts all events: Clicks, keyboard, HTTP calls, setTimeout/setInterval




**2) OnPush Strategy**
```typescript
@Component({
  changeDetection: ChangeDetectionStrategy.OnPush,  // activation
})
export class MyComponent {}
```
The component is checked ONLY if:

1. ✅ A **signal** (input, signal, computed) changes
2. ✅ An **event** happens in the component
3. ✅ An **AsyncPipe** receives a new value
4. ✅ **markForCheck()** is called manually



**3) Zoneless mode** without Zone.js (since Angular 20.2):

> - No more dependency on Zone.js
> - Detection based on **explicit notifications**
> - Better performance
>
> `==> Notifications in Zoneless Mode:`
> 1. **Signals** : Changes to `signal()`, `computed()`
> 2. **Inputs** : Component input modifications
> 3. **Events** : Clicks, keyboard input in the template
> 4. **AsyncPipe** : New async values
> 5. **markForCheck()** : Manual call


</details>


<details>
<summary> Loops and Conditions</summary>

`@for` : Iterate over lists in templates  
`@empty` : Handle empty lists  
`@if / @else` : Conditions in templates  
`@switch / @case` : Multiple conditions  
`@let` : Declare variables in templates  
`track` : Identify elements for optimization  


| Variable | Type | Description |
|----------|------|-------------|
| `$index` | number | Element index (0, 1, 2...) |
| `$first` | boolean | First element |
| `$last` | boolean | Last element |
| `$even` | boolean | Even index |
| `$odd` | boolean | Odd index |
| `$count` | number | Total number of elements |

</details>


<details>
<summary> Service</summary>

> `Service` : Injectable singleton class for business logic  
> `@Injectable` : Decorator to make a class injectable  
> `inject()` : Function to inject a service  
> `providedIn`: 'root' : Service available throughout the app  
> `CRUD` : Create, Read, Update, Delete operations  
> `localStorage` : Persist data in the browser  

</details>


<details>
<summary> Routing</summary>

Angular's routing system lets you build SPAs with navigation between different views.

**1. app.routes.ts**
The app.routes.ts file contains the configuration of all the application's routes.  
> `path: ''` : empty route (root of the application)  
> `redirectTo: 'home'` : redirects to /home  
> `pathMatch: 'full'` : exact match of the full path  
> `path: 'item/:id'` : route with dynamic parameter  
> `path: '**'` : wildcard route for 404s (place last)  

**2. RouterOutlet in the Template**
> RouterOutlet is the container where routed components are displayed.

**3. Configuration in app.config.ts**
```typescript
export const appConfig: ApplicationConfig = {
  providers: [
    provideZonelessChangeDetection(),
    provideRouter(routes, withComponentInputBinding())
  ]
};
```

**Note** : `withComponentInputBinding()` lets you receive route parameters directly as component inputs.


</details>


<details>
<summary> Navigation</summary>

Navigation with RouterLink (HTML)
```html
<!-- Navigation with parameter -->
<a [routerLink]="['/item', item.id]">View item</a>
<!-- With active class -->
<a [routerLink]="['/home']" routerLinkActive="active">Home</a>
```

Programmatic Navigation (TypeScript)

```typescript
export class MyComponent {
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
        }); // Resulting URL: /search?q=angular&page=1
    }
    
    navigateRelative() { // relative to the current route
        this.router.navigate(['../'], { relativeTo: this.route });
    }
}
```

</details>


<details>
<summary> Reactive Forms</summary>

Angular offers three approaches to handle forms:  
`Template-driven` forms : logic in the HTML template  
`Reactive forms` : logic in TypeScript code (recommended)  
`Signal forms` : new experimental approach with signals  

**Advantages of Reactive Forms**  
✅ `Testability` : logic in TypeScript, easy to test  
✅ `Complex validation` : synchronous and asynchronous validators  
✅ `Strong typing` : error detection at compile time  
✅ `Reactivity` : Observable to track changes  
✅ `Dynamism` : add/remove controls easily  
✅ `Performance` : better control of change detection  


**FormControl** represents an individual form field.
```typescript
const nameControl = new FormControl('');
const ageControl = new FormControl(25);
const emailControl = new FormControl('', [Validators.required, Validators.email]);
const disabledControl = new FormControl({value: 'test', disabled: true});
```
**FormGroup** groups multiple `FormControl`s into an object.
```typescript
const userForm = new FormGroup({
    firstname: new FormControl('', Validators.required),
    email: new FormControl('', [Validators.required, Validators.email]),
    age: new FormControl(null, [Validators.min(18), Validators.max(100)])
});
```
**FormBuilder** simplifies form creation.
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

**Display Errors**
```html
<div>
    <input formControlName="email" type="email">
    <!-- Error display -->
    @if (userForm.controls.email.invalid && userForm.controls.email.touched) {
        <div class="error">
            @if (userForm.controls.email.hasError('required')) {
                <span>Email is required</span>
            }
            @if (userForm.controls.email.hasError('email')) {
                <span>Invalid email format</span>
            }
        </div>
    }
</div>
```

</details>



<details>
<summary> Angular Material </summary>

The official UI component library for Angular. It provides ready-made and customizable components to build professional interfaces quickly

**Advantages**:  
✅ `Pre-built components` : buttons, forms, tables, dialogs, etc.  
✅ `Consistent design` : Google's Material Design  
✅ `Accessibility (a11y)` : ARIA, built-in keyboard navigation  
✅ `Responsive` : adapts to different screen sizes  
✅ `Themes` : easy color customization  
✅ `Animations` : built-in smooth transitions  
✅ `Documentation` : complete examples and clear API  

**Main Components**  
`Forms` : Input, Select, Checkbox, Radio, Slider, Datepicker  
`Buttons` : Button, FAB, Icon Button, Button Toggle  
`Navigation` : Toolbar, Menu, Sidenav, Tabs  
`Layout` : Card, Expansion Panel, Stepper, Divider  
`Data` : Table, Paginator, Sort, Tree  
`Popups` : Dialog, Snackbar, Tooltip, Bottom Sheet  

</details>


<details>
<summary> HTTP Interceptors</summary>

Interceptors let you intercept all HTTP requests to modify them before they're sent.

```typescript
// interceptors/auth-token/auth-token-interceptor.ts
import { HttpInterceptorFn } from '@angular/common/http';

export const authTokenInterceptor: HttpInterceptorFn = (req, next) => {
    // Get the token from localStorage
    const token = localStorage.getItem('token');
    let requestToSend = req;
    if (token) {
        // Add the Authorization header with the token
        const headers = req.headers.set('Authorization', 'Bearer ' + token);
        // Clone the request with the new headers
        requestToSend = req.clone({ headers: headers });
    }
    // Pass the request (modified or not) to the next interceptor/backend
    return next(requestToSend);
};
```

1. An interceptor is a function that receives `(req, next)`
2. `req` : the HTTP request to intercept
3. `next` : function to pass to the next interceptor or make the HTTP call
4. We clone the request to modify it (HTTP requests are immutable)
5. We add the `Authorization: Bearer <token>` header

</details>



<details>
<summary> Route Guards</summary>

Guards protect routes by checking conditions before navigation.  
`CanActivate` : can we activate this route?  
`CanDeactivate` : can we leave this route?  
`CanActivateChild` : can we activate child routes?  
`CanLoad` : can we load a lazy-loaded module?  

```typescript
// guards/is-logged-in/is-logged-in-guard.ts
import { CanActivateFn, Router } from '@angular/router';

export const isLoggedInGuard: CanActivateFn = (route, state) => {
    const loginService = inject(LoginService);
    const router = inject(Router);
    // Case 1: We don't know yet if the user is logged in
    if (loginService.user() === undefined) {
        // Check with the API
        return loginService.getUser().pipe(
            map(user => {
                // If we have a user, allow access
                if (user) { return true;}
                // Otherwise, redirect to login
                return router.createUrlTree(['/login']);
            }),
            catchError(() => {
                // On error, redirect to login
                return of(router.createUrlTree(['/login']));
            })
        );
    }
    // Case 2: User is not logged in (null)
    if (loginService.user() === null) {
        return router.createUrlTree(['/login']);
    }
    // Case 3: User is logged in
    return true;
};
```

- The guard returns `true` to allow access
- It returns `false` or a `UrlTree` to redirect
</details>
