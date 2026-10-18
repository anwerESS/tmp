## [**..**](./00_index.md)

# Chapter 9: Collections and Generics

## Using Common Collection APIs

### Collection Framework Overview

Le **Java Collections Framework** fournit des structures de données pour stocker et manipuler des groupes d'objets.

**Hiérarchie principale**:
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

### Collection Interface

**Méthodes communes** à toutes les collections:

```java
import java.util.*;

Collection<String> collection = new ArrayList<>();

// Ajouter
boolean add(E element)
boolean addAll(Collection<? extends E> c)

// Supprimer
boolean remove(Object o)
boolean removeAll(Collection<?> c)
void clear()

// Vérifier
boolean contains(Object o)
boolean containsAll(Collection<?> c)
boolean isEmpty()
int size()

// Conversion
Object[] toArray()
<T> T[] toArray(T[] array)

// Itération
Iterator<E> iterator()
```

**Exemples**:
```java
Collection<String> collection = new ArrayList<>();

// Ajouter
collection.add("Alice");
collection.add("Bob");
System.out.println(collection);  // [Alice, Bob]

// Taille
System.out.println(collection.size());  // 2

// Contient
System.out.println(collection.contains("Alice"));  // true

// Supprimer
collection.remove("Bob");
System.out.println(collection);  // [Alice]

// Vider
collection.clear();
System.out.println(collection.isEmpty());  // true
```

## Using the List Interface

### List Basics

Une **List** est une collection **ordonnée** qui permet les **doublons** et l'accès par **index**.

**Implémentations principales**:
- **ArrayList**: array redimensionnable, rapide pour l'accès aléatoire
- **LinkedList**: liste doublement chaînée, rapide pour insert/delete

```java
import java.util.*;

List<String> list = new ArrayList<>();
list.add("Alice");
list.add("Bob");
list.add("Alice");  // Doublon autorisé
System.out.println(list);  // [Alice, Bob, Alice]
```

### Creating Lists

**Différentes façons de créer des listes**:

```java
// Liste vide
List<String> list1 = new ArrayList<>();

// Avec capacité initiale
List<String> list2 = new ArrayList<>(100);

// Depuis une autre collection
List<String> list3 = new ArrayList<>(list1);

// Liste immuable (Java 9+)
List<String> list4 = List.of("A", "B", "C");

// Arrays.asList (taille fixe)
List<String> list5 = Arrays.asList("A", "B", "C");

// Collections utility
List<String> list6 = new ArrayList<>();
Collections.addAll(list6, "A", "B", "C");
```

**Important**: `List.of()` et `Arrays.asList()` créent des listes **immuables/fixed-size**.

```java
List<String> list = List.of("A", "B");
// list.add("C");  // UnsupportedOperationException

List<String> list2 = Arrays.asList("A", "B");
// list2.add("C");  // UnsupportedOperationException
list2.set(0, "X");  // OK: modification autorisée
```

### List Methods

**Méthodes spécifiques aux List** (en plus de Collection):

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

// Sous-liste
List<E> subList(int fromIndex, int toIndex)

// Remplacement
void replaceAll(UnaryOperator<E> operator)

// Tri
void sort(Comparator<? super E> c)
```

**Exemples**:

#### get() et set()
```java
List<String> list = new ArrayList<>();
list.add("A");
list.add("B");
list.add("C");

System.out.println(list.get(0));  // "A"
System.out.println(list.get(2));  // "C"

list.set(1, "X");
System.out.println(list);  // [A, X, C]
```

#### add() avec index
```java
List<String> list = new ArrayList<>(Arrays.asList("A", "C"));
list.add(1, "B");  // Insère à l'index 1
System.out.println(list);  // [A, B, C]
```

#### indexOf() et lastIndexOf()
```java
List<String> list = Arrays.asList("A", "B", "A", "C");
System.out.println(list.indexOf("A"));      // 0 (première occurrence)
System.out.println(list.lastIndexOf("A"));  // 2 (dernière occurrence)
System.out.println(list.indexOf("X"));      // -1 (non trouvé)
```

#### subList()
```java
List<String> list = Arrays.asList("A", "B", "C", "D", "E");
List<String> sub = list.subList(1, 4);  // De 1 (inclus) à 4 (exclus)
System.out.println(sub);  // [B, C, D]

// La sous-liste est une VUE (backed by original)
sub.set(0, "X");
System.out.println(list);  // [A, X, C, D, E]
```

#### replaceAll()
```java
List<String> list = new ArrayList<>(Arrays.asList("a", "b", "c"));
list.replaceAll(s -> s.toUpperCase());
System.out.println(list);  // [A, B, C]
```

#### sort()
```java
List<Integer> numbers = new ArrayList<>(Arrays.asList(5, 2, 8, 1, 9));
numbers.sort(Comparator.naturalOrder());
System.out.println(numbers);  // [1, 2, 5, 8, 9]

List<String> words = new ArrayList<>(Arrays.asList("Charlie", "Alice", "Bob"));
words.sort(String::compareToIgnoreCase);
System.out.println(words);  // [Alice, Bob, Charlie]
```

### ArrayList vs LinkedList

| Feature | ArrayList | LinkedList |
|---------|-----------|------------|
| Structure | Array dynamique | Liste doublement chaînée |
| Accès par index | O(1) rapide | O(n) lent |
| Insert/delete début/milieu | O(n) lent | O(1) rapide |
| Insert/delete fin | O(1) amorti | O(1) |
| Mémoire | Compacte | Plus de mémoire (nodes) |

**Exemples**:
```java
// ArrayList: bon pour l'accès aléatoire
List<String> arrayList = new ArrayList<>();
arrayList.add("A");
arrayList.add("B");
arrayList.add("C");
System.out.println(arrayList.get(1));  // Rapide: O(1)

// LinkedList: bon pour insert/delete
List<String> linkedList = new LinkedList<>();
linkedList.add(0, "First");  // Rapide au début
linkedList.add("Last");
```

### Looping Through Lists

**Différentes façons de parcourir**:

```java
List<String> list = Arrays.asList("A", "B", "C");

// 1. For traditionnel (avec index)
for (int i = 0; i < list.size(); i++) {
    System.out.println(list.get(i));
}

// 2. Enhanced for loop
for (String item : list) {
    System.out.println(item);
}

// 3. Iterator
Iterator<String> iterator = list.iterator();
while (iterator.hasNext()) {
    System.out.println(iterator.next());
}

// 4. forEach avec lambda (Java 8+)
list.forEach(item -> System.out.println(item));
list.forEach(System.out::println);  // Method reference

// 5. Stream (Java 8+)
list.stream().forEach(System.out::println);
```

### Removing Elements While Looping

**Piège**: Modification pendant l'itération peut causer des problèmes.

**Mauvais** (ConcurrentModificationException):
```java
List<String> list = new ArrayList<>(Arrays.asList("A", "B", "C"));
for (String item : list) {
    if (item.equals("B")) {
        // list.remove(item);  // ConcurrentModificationException!
    }
}
```

**Solutions**:

**1. Iterator.remove()**:
```java
List<String> list = new ArrayList<>(Arrays.asList("A", "B", "C", "B"));
Iterator<String> iterator = list.iterator();
while (iterator.hasNext()) {
    String item = iterator.next();
    if (item.equals("B")) {
        iterator.remove();  // OK
    }
}
System.out.println(list);  // [A, C]
```

**2. removeIf()** (Java 8+):
```java
List<String> list = new ArrayList<>(Arrays.asList("A", "B", "C", "B"));
list.removeIf(item -> item.equals("B"));
System.out.println(list);  // [A, C]
```

**3. For traditionnel (en arrière)**:
```java
List<String> list = new ArrayList<>(Arrays.asList("A", "B", "C", "B"));
for (int i = list.size() - 1; i >= 0; i--) {
    if (list.get(i).equals("B")) {
        list.remove(i);
    }
}
System.out.println(list);  // [A, C]
```

## Using the Set Interface

### Set Basics

Un **Set** est une collection qui **n'autorise pas les doublons**.

**Implémentations principales**:
- **HashSet**: non ordonné, rapide (O(1) pour add/remove/contains)
- **LinkedHashSet**: maintient l'ordre d'insertion
- **TreeSet**: trié naturellement, plus lent (O(log n))

```java
Set<String> set = new HashSet<>();
set.add("Alice");
set.add("Bob");
set.add("Alice");  // Ignoré (doublon)
System.out.println(set);  // [Alice, Bob] (ou [Bob, Alice], ordre non garanti)
```

### HashSet

**Caractéristiques**:
- Pas d'ordre garanti
- Permet 1 seul `null`
- Performance O(1) pour les opérations de base

```java
Set<String> set = new HashSet<>();
set.add("Charlie");
set.add("Alice");
set.add("Bob");
set.add(null);
System.out.println(set);  // Ordre aléatoire, ex: [null, Bob, Alice, Charlie]

// Vérifier présence
System.out.println(set.contains("Alice"));  // true

// Taille
System.out.println(set.size());  // 4

// Supprimer
set.remove("Bob");
System.out.println(set);  // [null, Alice, Charlie]
```

### LinkedHashSet

**Caractéristiques**:
- Maintient l'**ordre d'insertion**
- Légèrement plus lent que HashSet
- Permet 1 seul `null`

```java
Set<String> set = new LinkedHashSet<>();
set.add("Charlie");
set.add("Alice");
set.add("Bob");
System.out.println(set);  // [Charlie, Alice, Bob] (ordre d'insertion préservé)
```

### TreeSet

**Caractéristiques**:
- Éléments **triés** (ordre naturel ou Comparator)
- Performance O(log n)
- **N'autorise pas `null`** (NullPointerException)

```java
Set<String> set = new TreeSet<>();
set.add("Charlie");
set.add("Alice");
set.add("Bob");
System.out.println(set);  // [Alice, Bob, Charlie] (ordre alphabétique)

Set<Integer> numbers = new TreeSet<>();
numbers.add(5);
numbers.add(2);
numbers.add(8);
System.out.println(numbers);  // [2, 5, 8] (ordre croissant)
```

**Avec Comparator custom**:
```java
// Ordre décroissant
Set<Integer> set = new TreeSet<>(Comparator.reverseOrder());
set.add(5);
set.add(2);
set.add(8);
System.out.println(set);  // [8, 5, 2]

// Tri par longueur
Set<String> words = new TreeSet<>(Comparator.comparing(String::length));
words.add("Java");
words.add("Python");
words.add("C");
System.out.println(words);  // [C, Java, Python]
```

### Set Operations

**Opérations ensemblistes**:

```java
Set<Integer> set1 = new HashSet<>(Arrays.asList(1, 2, 3, 4));
Set<Integer> set2 = new HashSet<>(Arrays.asList(3, 4, 5, 6));

// Union (tous les éléments)
Set<Integer> union = new HashSet<>(set1);
union.addAll(set2);
System.out.println("Union: " + union);  // [1, 2, 3, 4, 5, 6]

// Intersection (éléments communs)
Set<Integer> intersection = new HashSet<>(set1);
intersection.retainAll(set2);
System.out.println("Intersection: " + intersection);  // [3, 4]

// Différence (dans set1 mais pas dans set2)
Set<Integer> difference = new HashSet<>(set1);
difference.removeAll(set2);
System.out.println("Différence: " + difference);  // [1, 2]
```

## Using the Queue and Deque Interfaces

### Queue Basics

Une **Queue** (file) suit le principe **FIFO** (First In, First Out).

**Méthodes principales**:

| Operation | Throws Exception | Returns Special Value |
|-----------|------------------|----------------------|
| Insert | `add(e)` | `offer(e)` |
| Remove | `remove()` | `poll()` |
| Examine | `element()` | `peek()` |

```java
import java.util.*;

Queue<String> queue = new LinkedList<>();

// Ajouter
queue.offer("First");
queue.offer("Second");
queue.offer("Third");

// Examiner (sans retirer)
System.out.println(queue.peek());  // "First"

// Retirer
System.out.println(queue.poll());  // "First"
System.out.println(queue.poll());  // "Second"

System.out.println(queue);  // [Third]
```

### LinkedList as Queue

**LinkedList** implémente Queue et fonctionne bien comme file.

```java
Queue<Integer> queue = new LinkedList<>();
queue.offer(1);
queue.offer(2);
queue.offer(3);

while (!queue.isEmpty()) {
    System.out.println(queue.poll());
}
// Output: 1, 2, 3 (ordre FIFO)
```

### PriorityQueue

**PriorityQueue**: éléments triés selon leur **priorité** (ordre naturel ou Comparator).

```java
Queue<Integer> pq = new PriorityQueue<>();
pq.offer(5);
pq.offer(2);
pq.offer(8);
pq.offer(1);

while (!pq.isEmpty()) {
    System.out.println(pq.poll());
}
// Output: 1, 2, 5, 8 (ordre croissant)
```

**Avec Comparator custom**:
```java
// File de priorité (ordre décroissant)
Queue<Integer> pq = new PriorityQueue<>(Comparator.reverseOrder());
pq.offer(5);
pq.offer(2);
pq.offer(8);

System.out.println(pq.poll());  // 8 (plus grand en premier)
System.out.println(pq.poll());  // 5
System.out.println(pq.poll());  // 2
```

### Deque Interface

**Deque** (Double-Ended Queue): file à **deux extrémités**, peut être utilisée comme:
- Queue (FIFO)
- Stack (LIFO)

**Méthodes principales**:

| Operation | First Element | Last Element |
|-----------|--------------|--------------|
| Insert | `addFirst(e)` / `offerFirst(e)` | `addLast(e)` / `offerLast(e)` |
| Remove | `removeFirst()` / `pollFirst()` | `removeLast()` / `pollLast()` |
| Examine | `getFirst()` / `peekFirst()` | `getLast()` / `peekLast()` |

**Stack operations**:
- `push(e)` = `addFirst(e)`
- `pop()` = `removeFirst()`
- `peek()` = `peekFirst()`

```java
Deque<String> deque = new ArrayDeque<>();

// Utiliser comme Queue (FIFO)
deque.offerLast("First");
deque.offerLast("Second");
System.out.println(deque.pollFirst());  // "First"

// Utiliser comme Stack (LIFO)
deque.push("A");
deque.push("B");
System.out.println(deque.pop());  // "B"
```

### ArrayDeque

**ArrayDeque** est l'implémentation recommandée pour Deque (plus rapide que LinkedList).

```java
Deque<Integer> stack = new ArrayDeque<>();

// Utiliser comme Stack
stack.push(1);
stack.push(2);
stack.push(3);

while (!stack.isEmpty()) {
    System.out.println(stack.pop());
}
// Output: 3, 2, 1 (LIFO)
```

## Using the Map Interface

### Map Basics

Un **Map** stocke des paires **clé-valeur**. Les clés sont **uniques**, les valeurs peuvent être dupliquées.

**Implémentations principales**:
- **HashMap**: non ordonné, rapide (O(1))
- **LinkedHashMap**: maintient l'ordre d'insertion
- **TreeMap**: clés triées (O(log n))

```java
import java.util.*;

Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);
map.put("Bob", 30);
map.put("Charlie", 28);

System.out.println(map.get("Alice"));  // 25
System.out.println(map.size());        // 3
```

### Common Map Methods

```java
// Ajouter/modifier
V put(K key, V value)
void putAll(Map<? extends K, ? extends V> m)
V putIfAbsent(K key, V value)

// Récupérer
V get(Object key)
V getOrDefault(Object key, V defaultValue)

// Supprimer
V remove(Object key)
boolean remove(Object key, Object value)
void clear()

// Vérifier
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

**Exemples**:

#### put() et get()
```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);
map.put("Bob", 30);
map.put("Alice", 26);  // Remplace la valeur existante

System.out.println(map.get("Alice"));  // 26
System.out.println(map.get("Charlie")); // null (clé absente)
```

#### getOrDefault()
```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);

System.out.println(map.getOrDefault("Alice", 0));    // 25
System.out.println(map.getOrDefault("Charlie", 0));  // 0 (défaut)
```

#### putIfAbsent()
```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);

map.putIfAbsent("Alice", 30);    // N'ajoute pas (clé existe)
map.putIfAbsent("Bob", 30);      // Ajoute (clé absente)

System.out.println(map);  // {Alice=25, Bob=30}
```

#### containsKey() et containsValue()
```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);
map.put("Bob", 30);

System.out.println(map.containsKey("Alice"));    // true
System.out.println(map.containsKey("Charlie"));  // false
System.out.println(map.containsValue(25));       // true
System.out.println(map.containsValue(40));       // false
```

#### remove()
```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);
map.put("Bob", 30);

map.remove("Alice");
System.out.println(map);  // {Bob=30}

// Remove with value check
map.put("Charlie", 28);
map.remove("Charlie", 30);  // Ne supprime pas (valeur différente)
map.remove("Charlie", 28);  // Supprime (clé ET valeur correspondent)
```

### Iterating Over Maps

**Différentes façons de parcourir**:

```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);
map.put("Bob", 30);
map.put("Charlie", 28);

// 1. forEach avec lambda (Java 8+)
map.forEach((key, value) -> 
    System.out.println(key + ": " + value)
);

// 2. Parcourir les clés
for (String key : map.keySet()) {
    System.out.println(key + ": " + map.get(key));
}

// 3. Parcourir les valeurs
for (Integer value : map.values()) {
    System.out.println(value);
}

// 4. Parcourir les entries
for (Map.Entry<String, Integer> entry : map.entrySet()) {
    System.out.println(entry.getKey() + ": " + entry.getValue());
}

// 5. Stream (Java 8+)
map.entrySet().stream()
   .forEach(entry -> System.out.println(entry.getKey() + ": " + entry.getValue()));
```

### HashMap vs LinkedHashMap vs TreeMap

**HashMap**:
```java
Map<String, Integer> map = new HashMap<>();
map.put("Charlie", 28);
map.put("Alice", 25);
map.put("Bob", 30);
System.out.println(map);  // Ordre aléatoire
```

**LinkedHashMap** (ordre d'insertion):
```java
Map<String, Integer> map = new LinkedHashMap<>();
map.put("Charlie", 28);
map.put("Alice", 25);
map.put("Bob", 30);
System.out.println(map);  // {Charlie=28, Alice=25, Bob=30}
```

**TreeMap** (clés triées):
```java
Map<String, Integer> map = new TreeMap<>();
map.put("Charlie", 28);
map.put("Alice", 25);
map.put("Bob", 30);
System.out.println(map);  // {Alice=25, Bob=30, Charlie=28}
```

### Advanced Map Operations (Java 8+)

#### compute()
```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);

// Modifier la valeur existante
map.compute("Alice", (key, value) -> value + 1);
System.out.println(map.get("Alice"));  // 26

// Créer une nouvelle entrée
map.compute("Bob", (key, value) -> value == null ? 1 : value + 1);
System.out.println(map.get("Bob"));  // 1
```

#### merge()
```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);

// Si la clé existe, merge les valeurs
map.merge("Alice", 5, (oldValue, newValue) -> oldValue + newValue);
System.out.println(map.get("Alice"));  // 30

// Si la clé n'existe pas, ajoute la nouvelle valeur
map.merge("Bob", 30, (oldValue, newValue) -> oldValue + newValue);
System.out.println(map.get("Bob"));  // 30
```

**Exemple pratique** (compter les occurrences):
```java
List<String> words = Arrays.asList("apple", "banana", "apple", "cherry", "banana", "apple");
Map<String, Integer> wordCount = new HashMap<>();

for (String word : words) {
    wordCount.merge(word, 1, Integer::sum);
}

System.out.println(wordCount);  // {apple=3, banana=2, cherry=1}
```

## Comparing Collection Types

### Performance Comparison

| Collection | Add | Remove | Get/Contains | Iteration |
|------------|-----|--------|--------------|-----------|
| ArrayList | O(1)* | O(n) | O(1) | O(n) |
| LinkedList | O(1) | O(1)** | O(n) | O(n) |
| HashSet | O(1) | O(1) | O(1) | O(n) |
| LinkedHashSet | O(1) | O(1) | O(1) | O(n) |
| TreeSet | O(log n) | O(log n) | O(log n) | O(n) |
| HashMap | O(1) | O(1) | O(1) | O(n) |
| LinkedHashMap | O(1) | O(1) | O(1) | O(n) |
| TreeMap | O(log n) | O(log n) | O(log n) | O(n) |

*Amorti (peut être O(n) lors du redimensionnement)
**Si on a la référence au node

### Choosing the Right Collection

**Critères de choix**:

**List**:
- Besoin d'ordre et d'index → **ArrayList** (accès rapide)
- Insertions/suppressions fréquentes au début/milieu → **LinkedList**

**Set**:
- Pas de doublons, pas d'ordre → **HashSet**
- Pas de doublons, ordre d'insertion → **LinkedHashSet**
- Pas de doublons, ordre trié → **TreeSet**

**Queue/Deque**:
- File FIFO → **LinkedList** ou **ArrayDeque**
- File de priorité → **PriorityQueue**
- Stack LIFO → **ArrayDeque** (mieux que Stack legacy)

**Map**:
- Clé-valeur, pas d'ordre → **HashMap**
- Clé-valeur, ordre d'insertion → **LinkedHashMap**
- Clé-valeur, clés triées → **TreeMap**

## Sorting Data

### Comparable Interface

**Comparable**: définit l'**ordre naturel** d'une classe.

**Méthode**: `int compareTo(T o)`

```java
public class Person implements Comparable<Person> {
    private String name;
    private int age;
    
    public Person(String name, int age) {
        this.name = name;
        this.age = age;
    }
    
    @Override
    public int compareTo(Person other) {
        return this.age - other.age;  // Tri par âge
    }
    
    @Override
    public String toString() {
        return name + " (" + age + ")";
    }
}

// Utilisation
List<Person> people = new ArrayList<>();
people.add(new Person("Alice", 30));
people.add(new Person("Bob", 25));
people.add(new Person("Charlie", 35));

Collections.sort(people);  // Utilise compareTo()
System.out.println(people);  // [Bob (25), Alice (30), Charlie (35)]
```

**Règles compareTo()**:
- Retourne **négatif** si `this < other`
- Retourne **0** si `this == other`
- Retourne **positif** si `this > other`

```java
// Exemples
"apple".compareTo("banana")  // < 0 (apple avant banana)
"banana".compareTo("apple")  // > 0 (banana après apple)
"apple".compareTo("apple")   // 0 (égaux)
```

### Comparator Interface

**Comparator**: définit un **ordre personnalisé** (sans modifier la classe).

**Méthode**: `int compare(T o1, T o2)`

```java
// Comparator par nom
Comparator<Person> byName = (p1, p2) -> p1.name.compareTo(p2.name);

List<Person> people = new ArrayList<>();
people.add(new Person("Charlie", 35));
people.add(new Person("Alice", 30));
people.add(new Person("Bob", 25));

Collections.sort(people, byName);
System.out.println(people);  // [Alice (30), Bob (25), Charlie (35)]
```

### Comparator Helper Methods (Java 8+)

**comparing()**:
```java
// Tri par un champ
Comparator<Person> byAge = Comparator.comparing(Person::getAge);
Comparator<Person> byName = Comparator.comparing(Person::getName);

people.sort(byAge);
```

**reversed()**:
```java
// Ordre inverse
Comparator<Person> byAgeDesc = Comparator.comparing(Person::getAge).reversed();
people.sort(byAgeDesc);
```

**thenComparing()**:
```java
// Tri multiple (âge puis nom)
Comparator<Person> comparator = Comparator
    .comparing(Person::getAge)
    .thenComparing(Person::getName);

people.sort(comparator);
```

**nullsFirst() / nullsLast()**:
```java
// Gérer les nulls
Comparator<String> comparator = Comparator.nullsFirst(String::compareTo);

List<String> list = Arrays.asList("B", null, "A", "C");
list.sort(comparator);
System.out.println(list);  // [null, A, B, C]
```

**naturalOrder() / reverseOrder()**:
```java
List<Integer> numbers = Arrays.asList(5, 2, 8, 1, 9);

numbers.sort(Comparator.naturalOrder());
System.out.println(numbers);  // [1, 2, 5, 8, 9]

numbers.sort(Comparator.reverseOrder());
System.out.println(numbers);  // [9, 8, 5, 2, 1]
```

### Sorting Examples

**Collections.sort()**:
```java
List<String> names = new ArrayList<>(Arrays.asList("Charlie", "Alice", "Bob"));

// Ordre naturel
Collections.sort(names);
System.out.println(names);  // [Alice, Bob, Charlie]

// Avec Comparator
Collections.sort(names, Comparator.reverseOrder());
System.out.println(names);  // [Charlie, Bob, Alice]
```

**List.sort()** (Java 8+):
```java
List<String> names = new ArrayList<>(Arrays.asList("Charlie", "Alice", "Bob"));

// Ordre naturel
names.sort(Comparator.naturalOrder());
System.out.println(names);  // [Alice, Bob, Charlie]

// Lambda
names.sort((s1, s2) -> s1.length() - s2.length());
System.out.println(names);  // [Bob, Alice, Charlie]
```

**Arrays.sort()**:
```java
String[] array = {"Charlie", "Alice", "Bob"};

Arrays.sort(array);
System.out.println(Arrays.toString(array));  // [Alice, Bob, Charlie]

Arrays.sort(array, Comparator.reverseOrder());
System.out.println(Arrays.toString(array));  // [Charlie, Bob, Alice]
```

### Comparable vs Comparator

| Comparable | Comparator |
|------------|------------|
| Ordre **naturel** unique | Ordre **custom** multiple |
| Méthode `compareTo()` | Méthode `compare()` |
| Modifie la classe | Classe externe |
| `Collections.sort(list)` | `Collections.sort(list, comparator)` |

```java
// Classe avec Comparable
public class Product implements Comparable<Product> {
    private String name;
    private double price;
    
    @Override
    public int compareTo(Product other) {
        return Double.compare(this.price, other.price);  // Ordre naturel par prix
    }
}

// Comparators externes
Comparator<Product> byName = Comparator.comparing(Product::getName);
Comparator<Product> byPriceDesc = Comparator.comparing(Product::getPrice).reversed();

List<Product> products = new ArrayList<>();
products.sort(null);       // Utilise compareTo() (ordre naturel)
products.sort(byName);     // Utilise Comparator (ordre par nom)
products.sort(byPriceDesc); // Utilise Comparator (prix décroissant)
```

## Working with Generics

### Generics Basics

Les **generics** permettent de créer des classes, interfaces et méthodes **type-safe** (vérification à la compilation).

**Avant les generics** (Java < 5):
```java
List list = new ArrayList();
list.add("Hello");
list.add(123);
String s = (String) list.get(0);  // Cast nécessaire
String s2 = (String) list.get(1); // ClassCastException à l'exécution!
```

**Avec generics**:
```java
List<String> list = new ArrayList<>();
list.add("Hello");
// list.add(123);  // ERREUR de compilation
String s = list.get(0);  // Pas de cast nécessaire
```

### Generic Classes

**Créer une classe générique**:

```java
public class Box<T> {
    private T content;
    
    public void set(T content) {
        this.content = content;
    }
    
    public T get() {
        return content;
    }
}

// Utilisation
Box<String> stringBox = new Box<>();
stringBox.set("Hello");
String s = stringBox.get();  // Pas de cast

Box<Integer> intBox = new Box<>();
intBox.set(42);
Integer i = intBox.get();
```

**Plusieurs paramètres de type**:
```java
public class Pair<K, V> {
    private K key;
    private V value;
    
    public Pair(K key, V value) {
        this.key = key;
        this.value = value;
    }
    
    public K getKey() { return key; }
    public V getValue() { return value; }
}

// Utilisation
Pair<String, Integer> pair = new Pair<>("Age", 25);
System.out.println(pair.getKey() + ": " + pair.getValue());  // "Age: 25"
```

### Generic Interfaces

```java
public interface Processor<T> {
    T process(T input);
}

// Implémentation
public class StringProcessor implements Processor<String> {
    @Override
    public String process(String input) {
        return input.toUpperCase();
    }
}

// Utilisation
Processor<String> processor = new StringProcessor();
String result = processor.process("hello");
System.out.println(result);  // "HELLO"
```

### Generic Methods

**Méthode générique** (indépendante de la classe):

```java
public class Util {
    // Méthode générique static
    public static <T> void printArray(T[] array) {
        for (T element : array) {
            System.out.print(element + " ");
        }
        System.out.println();
    }
    
    // Méthode générique avec return
    public static <T> T getFirst(List<T> list) {
        return list.isEmpty() ? null : list.get(0);
    }
}

// Utilisation
String[] strings = {"A", "B", "C"};
Util.printArray(strings);  // A B C

Integer[] numbers = {1, 2, 3};
Util.printArray(numbers);  // 1 2 3

List<String> list = Arrays.asList("Alice", "Bob");
String first = Util.getFirst(list);  // "Alice"
```

**Syntaxe**: `<T>` avant le return type indique que c'est une méthode générique.

### Type Erasure

**Type erasure**: Les generics sont supprimés à la compilation (pour compatibilité avec ancien code).

```java
// Code source
List<String> list = new ArrayList<>();

// Après compilation (bytecode)
List list = new ArrayList();  // Type effacé
```

**Conséquences**:
1. Pas d'instance de type paramétré à l'exécution
2. Pas de tableau générique
3. Cast automatiques insérés par le compilateur

```java
// Impossible
// T obj = new T();              // ERREUR
// T[] array = new T[10];        // ERREUR
// if (obj instanceof T) { }     // ERREUR

// Possible (workaround)
public <T> T[] createArray(Class<T> clazz, int size) {
    return (T[]) Array.newInstance(clazz, size);
}
```

### Bounded Type Parameters

**Upper bound** (`extends`): limite les types acceptés.

```java
// T doit être Number ou une sous-classe
public class NumberBox<T extends Number> {
    private T value;
    
    public NumberBox(T value) {
        this.value = value;
    }
    
    public double doubleValue() {
        return value.doubleValue();  // Méthode de Number disponible
    }
}

// Utilisation
NumberBox<Integer> intBox = new NumberBox<>(42);
NumberBox<Double> doubleBox = new NumberBox<>(3.14);
// NumberBox<String> stringBox = new NumberBox<>("Hello");  // ERREUR
```

**Exemple pratique**:
```java
// Méthode qui accepte uniquement des listes de nombres
public static double sum(List<? extends Number> numbers) {
    double sum = 0;
    for (Number num : numbers) {
        sum += num.doubleValue();
    }
    return sum;
}

List<Integer> ints = Arrays.asList(1, 2, 3);
List<Double> doubles = Arrays.asList(1.5, 2.5, 3.5);

System.out.println(sum(ints));     // 6.0
System.out.println(sum(doubles));  // 7.5
```

**Multiple bounds**:
```java
// T doit implémenter Comparable ET Serializable
public class SortableBox<T extends Comparable<T> & Serializable> {
    private T value;
    
    public int compareTo(T other) {
        return value.compareTo(other);
    }
}
```

### Wildcards

**Unbounded wildcard** (`?`): n'importe quel type.

```java
public static void printList(List<?> list) {
    for (Object item : list) {
        System.out.println(item);
    }
}

List<String> strings = Arrays.asList("A", "B");
List<Integer> ints = Arrays.asList(1, 2);

printList(strings);  // OK
printList(ints);     // OK
```

**Upper bounded wildcard** (`? extends Type`): Type ou sous-types.

```java
// Accepte List<Integer>, List<Double>, etc.
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
    list.add(1);
    list.add(2);
    list.add(3);
}

List<Integer> ints = new ArrayList<>();
List<Number> numbers = new ArrayList<>();
List<Object> objects = new ArrayList<>();

addIntegers(ints);     // OK
addIntegers(numbers);  // OK
addIntegers(objects);  // OK
```

### PECS Rule

**PECS**: **Producer Extends, Consumer Super**

- **Producer** (lecture): utiliser `? extends T`
- **Consumer** (écriture): utiliser `? super T`

```java
// Producer (lit des éléments)
public static void copy(List<? extends Number> source, List<? super Number> dest) {
    for (Number num : source) {  // Lit depuis source
        dest.add(num);           // Écrit dans dest
    }
}

List<Integer> source = Arrays.asList(1, 2, 3);
List<Number> dest = new ArrayList<>();

copy(source, dest);
System.out.println(dest);  // [1, 2, 3]
```

**Exemples**:
```java
// Producer (extends)
List<? extends Number> producer = new ArrayList<Integer>();
Number n = producer.get(0);        // OK: lecture
// producer.add(5);                // ERREUR: écriture interdite

// Consumer (super)
List<? super Integer> consumer = new ArrayList<Number>();
consumer.add(5);                   // OK: écriture
// Integer i = consumer.get(0);    // ERREUR: lecture (retourne Object)
```

---

## Exemples Pratiques Complets

### Exemple 1: Gestion d'Inventaire avec Map
```java
import java.util.*;

public class Inventory {
    private Map<String, Integer> stock = new HashMap<>();
    
    public void addProduct(String product, int quantity) {
        stock.merge(product, quantity, Integer::sum);
    }
    
    public boolean removeProduct(String product, int quantity) {
        Integer current = stock.get(product);
        if (current == null || current < quantity) {
            return false;
        }
        
        if (current == quantity) {
            stock.remove(product);
        } else {
            stock.put(product, current - quantity);
        }
        return true;
    }
    
    public int getStock(String product) {
        return stock.getOrDefault(product, 0);
    }
    
    public List<String> getLowStockProducts(int threshold) {
        List<String> lowStock = new ArrayList<>();
        stock.forEach((product, quantity) -> {
            if (quantity < threshold) {
                lowStock.add(product);
            }
        });
        return lowStock;
    }
    
    public void displayInventory() {
        System.out.println("=== INVENTAIRE ===");
        stock.entrySet().stream()
             .sorted(Map.Entry.comparingByKey())
             .forEach(entry -> 
                 System.out.printf("%s: %d unités%n", entry.getKey(), entry.getValue())
             );
    }
    
    public static void main(String[] args) {
        Inventory inv = new Inventory();
        
        inv.addProduct("Laptop", 10);
        inv.addProduct("Mouse", 50);
        inv.addProduct("Keyboard", 30);
        inv.addProduct("Mouse", 20);  // Ajoute 20 de plus
        
        inv.displayInventory();
        
        System.out.println("\nStock Mouse: " + inv.getStock("Mouse"));
        
        inv.removeProduct("Laptop", 5);
        System.out.println("\nAprès retrait:");
        inv.displayInventory();
        
        System.out.println("\nProduits en rupture (< 15):");
        List<String> lowStock = inv.getLowStockProducts(15);
        lowStock.forEach(System.out::println);
    }
}
```

### Exemple 2: Cache avec LinkedHashMap (LRU)
```java
import java.util.*;

public class LRUCache<K, V> extends LinkedHashMap<K, V> {
    private final int capacity;
    
    public LRUCache(int capacity) {
        super(capacity, 0.75f, true);  // accessOrder = true (LRU)
        this.capacity = capacity;
    }
    
    @Override
    protected boolean removeEldestEntry(Map.Entry<K, V> eldest) {
        return size() > capacity;  // Supprimer le plus ancien si dépassement
    }
    
    public static void main(String[] args) {
        LRUCache<String, Integer> cache = new LRUCache<>(3);
        
        cache.put("A", 1);
        cache.put("B", 2);
        cache.put("C", 3);
        System.out.println("Cache: " + cache);  // {A=1, B=2, C=3}
        
        cache.get("A");  // Accès à A (devient le plus récent)
        System.out.println("Après accès à A: " + cache);  // {B=2, C=3, A=1}
        
        cache.put("D", 4);  // B est supprimé (plus ancien)
        System.out.println("Après ajout D: " + cache);  // {C=3, A=1, D=4}
        
        cache.put("E", 5);  // C est supprimé
        System.out.println("Après ajout E: " + cache);  // {A=1, D=4, E=5}
    }
}
```

### Exemple 3: Generic Repository Pattern
```java
import java.util.*;

// Interface générique
public interface Repository<T, ID> {
    void save(T entity);
    Optional<T> findById(ID id);
    List<T> findAll();
    void delete(ID id);
    boolean exists(ID id);
}

// Implémentation en mémoire
public class InMemoryRepository<T extends Identifiable<ID>, ID> implements Repository<T, ID> {
    private Map<ID, T> storage = new HashMap<>();
    
    @Override
    public void save(T entity) {
        storage.put(entity.getId(), entity);
    }
    
    @Override
    public Optional<T> findById(ID id) {
        return Optional.ofNullable(storage.get(id));
    }
    
    @Override
    public List<T> findAll() {
        return new ArrayList<>(storage.values());
    }
    
    @Override
    public void delete(ID id) {
        storage.remove(id);
    }
    
    @Override
    public boolean exists(ID id) {
        return storage.containsKey(id);
    }
}

// Interface pour les entités avec ID
public interface Identifiable<ID> {
    ID getId();
}

// Entité User
public class User implements Identifiable<Long> {
    private Long id;
    private String name;
    private String email;
    
    public User(Long id, String name, String email) {
        this.id = id;
        this.name = name;
        this.email = email;
    }
    
    @Override
    public Long getId() { return id; }
    public String getName() { return name; }
    public String getEmail() { return email; }
    
    @Override
    public String toString() {
        return String.format("User[id=%d, name=%s, email=%s]", id, name, email);
    }
}

// Utilisation
public class RepositoryDemo {
    public static void main(String[] args) {
        Repository<User, Long> userRepo = new InMemoryRepository<>();
        
        // Sauvegarder des utilisateurs
        userRepo.save(new User(1L, "Alice", "alice@example.com"));
        userRepo.save(new User(2L, "Bob", "bob@example.com"));
        userRepo.save(new User(3L, "Charlie", "charlie@example.com"));
        
        // Rechercher
        userRepo.findById(2L).ifPresent(System.out::println);
        
        // Lister tous
        System.out.println("\nTous les utilisateurs:");
        userRepo.findAll().forEach(System.out::println);
        
        // Supprimer
        userRepo.delete(2L);
        
        System.out.println("\nAprès suppression:");
        userRepo.findAll().forEach(System.out::println);
        
        // Vérifier existence
        System.out.println("\nUser 2 existe: " + userRepo.exists(2L));
        System.out.println("User 1 existe: " + userRepo.exists(1L));
    }
}
```

### Exemple 4: Tri Multi-Critères
```java
import java.util.*;

public class Employee {
    private String name;
    private int age;
    private double salary;
    private String department;
    
    public Employee(String name, int age, double salary, String department) {
        this.name = name;
        this.age = age;
        this.salary = salary;
        this.department = department;
    }
    
    public String getName() { return name; }
    public int getAge() { return age; }
    public double getSalary() { return salary; }
    public String getDepartment() { return department; }
    
    @Override
    public String toString() {
        return String.format("%s (age:%d, salary:%.0f, dept:%s)", 
            name, age, salary, department);
    }
}

public class EmployeeSorting {
    public static void main(String[] args) {
        List<Employee> employees = Arrays.asList(
            new Employee("Alice", 30, 60000, "IT"),
            new Employee("Bob", 25, 50000, "HR"),
            new Employee("Charlie", 35, 70000, "IT"),
            new Employee("David", 25, 55000, "IT"),
            new Employee("Eve", 30, 65000, "HR")
        );
        
        // Tri par département puis salaire (décroissant)
        Comparator<Employee> byDeptThenSalary = Comparator
            .comparing(Employee::getDepartment)
            .thenComparing(Employee::getSalary, Comparator.reverseOrder());
        
        System.out.println("=== Par département puis salaire ===");
        employees.stream()
                 .sorted(byDeptThenSalary)
                 .forEach(System.out::println);
        
        // Tri par âge puis nom
        Comparator<Employee> byAgeThenName = Comparator
            .comparing(Employee::getAge)
            .thenComparing(Employee::getName);
        
        System.out.println("\n=== Par âge puis nom ===");
        employees.stream()
                 .sorted(byAgeThenName)
                 .forEach(System.out::println);
        
        // Top 3 salaires
        System.out.println("\n=== Top 3 salaires ===");
        employees.stream()
                 .sorted(Comparator.comparing(Employee::getSalary).reversed())
                 .limit(3)
                 .forEach(System.out::println);
        
        // Grouper par département
        System.out.println("\n=== Groupés par département ===");
        Map<String, List<Employee>> byDept = new TreeMap<>();
        for (Employee emp : employees) {
            byDept.computeIfAbsent(emp.getDepartment(), k -> new ArrayList<>()).add(emp);
        }
        byDept.forEach((dept, emps) -> {
            System.out.println(dept + ":");
            emps.forEach(e -> System.out.println("  " + e));
        });
    }
}
```

### Exemple 5: Generic Utility Methods
```java
import java.util.*;

public class CollectionUtils {
    
    // Filtrer une collection
    public static <T> List<T> filter(Collection<T> collection, Predicate<T> predicate) {
        List<T> result = new ArrayList<>();
        for (T item : collection) {
            if (predicate.test(item)) {
                result.add(item);
            }
        }
        return result;
    }
    
    // Transformer une collection
    public static <T, R> List<R> map(Collection<T> collection, Function<T, R> mapper) {
        List<R> result = new ArrayList<>();
        for (T item : collection) {
            result.add(mapper.apply(item));
        }
        return result;
    }
    
    // Trouver le premier élément
    public static <T> Optional<T> findFirst(Collection<T> collection, Predicate<T> predicate) {
        for (T item : collection) {
            if (predicate.test(item)) {
                return Optional.of(item);
            }
        }
        return Optional.empty();
    }
    
    // Partitionner une liste
    public static <T> List<List<T>> partition(List<T> list, int size) {
        List<List<T>> partitions = new ArrayList<>();
        for (int i = 0; i < list.size(); i += size) {
            partitions.add(list.subList(i, Math.min(i + size, list.size())));
        }
        return partitions;
    }
    
    // Fréquence des éléments
    public static <T> Map<T, Integer> frequency(Collection<T> collection) {
        Map<T, Integer> freq = new HashMap<>();
        for (T item : collection) {
            freq.merge(item, 1, Integer::sum);
        }
        return freq;
    }
    
    // Intersection de deux sets
    public static <T> Set<T> intersection(Set<T> set1, Set<T> set2) {
        Set<T> result = new HashSet<>(set1);
        result.retainAll(set2);
        return result;
    }
    
    // Union de deux sets
    public static <T> Set<T> union(Set<T> set1, Set<T> set2) {
        Set<T> result = new HashSet<>(set1);
        result.addAll(set2);
        return result;
    }
    
    public static void main(String[] args) {
        List<Integer> numbers = Arrays.asList(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        
        // Filtrer les pairs
        List<Integer> evens = filter(numbers, n -> n % 2 == 0);
        System.out.println("Pairs: " + evens);
        
        // Doubler les nombres
        List<Integer> doubled = map(numbers, n -> n * 2);
        System.out.println("Doublés: " + doubled);
        
        // Trouver le premier > 5
        Optional<Integer> first = findFirst(numbers, n -> n > 5);
        first.ifPresent(n -> System.out.println("Premier > 5: " + n));
        
        // Partitionner en groupes de 3
        List<List<Integer>> partitions = partition(numbers, 3);
        System.out.println("Partitions: " + partitions);
        
        // Fréquence
        List<String> words = Arrays.asList("a", "b", "a", "c", "b", "a");
        Map<String, Integer> freq = frequency(words);
        System.out.println("Fréquences: " + freq);
        
        // Opérations sur sets
        Set<Integer> set1 = new HashSet<>(Arrays.asList(1, 2, 3, 4));
        Set<Integer> set2 = new HashSet<>(Arrays.asList(3, 4, 5, 6));
        
        System.out.println("Intersection: " + intersection(set1, set2));
        System.out.println("Union: " + union(set1, set2));
    }
}
```

---

## Points Clés du Chapter 9

✅ **Collection Hierarchy**:
   - **List**: ordered, duplicates OK, index access
   - **Set**: no duplicates, no guaranteed order
   - **Queue/Deque**: FIFO/LIFO operations
   - **Map**: key-value pairs (not part of Collection)

✅ **List Implementations**:
   - **ArrayList**: fast random access, slow insert/delete
   - **LinkedList**: fast insert/delete, slow random access

✅ **Set Implementations**:
   - **HashSet**: no order, O(1) operations
   - **LinkedHashSet**: insertion order
   - **TreeSet**: sorted, O(log n) operations

✅ **Map Implementations**:
   - **HashMap**: no order, O(1) operations
   - **LinkedHashMap**: insertion order
   - **TreeMap**: sorted keys, O(log n) operations

✅ **Sorting**:
   - **Comparable**: natural order (`compareTo()`)
   - **Comparator**: custom order (`compare()`)
   - Helper methods: `comparing()`, `thenComparing()`, `reversed()`

✅ **Generics**:
   - Type safety at compile time
   - Syntax: `Class<T>`, `Method<T>()`
   - Bounded: `<T extends Type>`
   - Wildcards: `?`, `? extends`, `? super`
   - PECS: Producer Extends, Consumer Super

✅ **Common Operations**:
   - `add()`, `remove()`, `contains()`, `size()`, `isEmpty()`
   - `forEach()`, `removeIf()`, `replaceAll()` (Java 8+)
   - `sort()`, `stream()` (Java 8+)

✅ **Iteration**:
   - Enhanced for loop
   - Iterator (safe removal)
   - forEach with lambda
   - Stream API

---

# Questions/Réponses - Chapter 9: Collections and Generics

## Questions sur les Collections (Général)

**Q1: Quelle est la hiérarchie correcte?**
```java
// A. Iterable → Collection → List → ArrayList
// B. Collection → Iterable → List → ArrayList
// C. List → Collection → Iterable → ArrayList
// D. ArrayList → List → Collection → Iterable
```

**R:** **A** est correct.

```
Iterable (interface)
    └── Collection (interface)
            └── List (interface)
                    └── ArrayList (classe)
```

---

**Q2: Map fait-elle partie de la hiérarchie Collection?**

**R:** Non, **Map n'étend pas Collection**.

Map est une **interface séparée** car elle stocke des paires clé-valeur, pas des éléments simples.

```java
// Collection<E> collection
// Map<K, V> map
```

---

**Q3: Quelle est la différence entre Collection et Collections?**

**R:**
- **Collection**: **interface** de base pour les collections (List, Set, etc.)
- **Collections**: **classe utilitaire** avec des méthodes static (`sort()`, `reverse()`, etc.)

```java
Collection<String> col = new ArrayList<>();  // Interface
Collections.sort(list);                      // Classe utilitaire
```

---

## Questions sur les List

**Q4: Quelle est la différence fondamentale entre ArrayList et LinkedList?**

**R:**
- **ArrayList**: array dynamique, accès O(1), insert/delete O(n)
- **LinkedList**: liste chaînée, accès O(n), insert/delete O(1) (si on a la référence)

```java
// ArrayList: meilleur pour l'accès par index
List<String> arrayList = new ArrayList<>();
String item = arrayList.get(1000);  // Rapide: O(1)

// LinkedList: meilleur pour insert/delete fréquents
List<String> linkedList = new LinkedList<>();
linkedList.add(0, "First");  // Rapide au début
```

---

**Q5: Ce code compile-t-il?**
```java
List<String> list = List.of("A", "B", "C");
list.add("D");
```

**R:** Compile mais lance **UnsupportedOperationException** à l'exécution.

`List.of()` crée une liste **immuable**.

---

**Q6: Quelle est la différence?**
```java
// Version 1
List<String> list1 = List.of("A", "B", "C");

// Version 2
List<String> list2 = Arrays.asList("A", "B", "C");
```

**R:**
- **List.of()**: **Totalement immuable** (pas d'add, remove, ni set)
- **Arrays.asList()**: **Taille fixe** (pas d'add/remove, mais set autorisé)

```java
list1.set(0, "X");  // UnsupportedOperationException
list2.set(0, "X");  // OK
```

---

**Q7: Que retourne indexOf() si l'élément n'existe pas?**
```java
List<String> list = Arrays.asList("A", "B", "C");
int index = list.indexOf("X");
```

**R:** `-1`

`indexOf()` retourne **-1** si l'élément n'est pas trouvé (pas d'exception).

---

**Q8: Que fait subList()?**
```java
List<String> list = new ArrayList<>(Arrays.asList("A", "B", "C", "D", "E"));
List<String> sub = list.subList(1, 4);
sub.set(0, "X");
System.out.println(list);
```

**R:** `[A, X, C, D, E]`

`subList()` retourne une **vue** (backed by original). Les modifications dans la sous-liste affectent la liste originale.

---

**Q9: Pourquoi ce code lance-t-il une ConcurrentModificationException?**
```java
List<String> list = new ArrayList<>(Arrays.asList("A", "B", "C"));
for (String item : list) {
    if (item.equals("B")) {
        list.remove(item);
    }
}
```

**R:** On ne peut pas **modifier une collection** pendant qu'on l'itère avec un enhanced for loop.

**Solutions**:
```java
// 1. Iterator
Iterator<String> it = list.iterator();
while (it.hasNext()) {
    if (it.next().equals("B")) {
        it.remove();
    }
}

// 2. removeIf
list.removeIf(item -> item.equals("B"));

// 3. For traditionnel (en arrière)
for (int i = list.size() - 1; i >= 0; i--) {
    if (list.get(i).equals("B")) {
        list.remove(i);
    }
}
```

---

**Q10: Que fait replaceAll()?**
```java
List<String> list = new ArrayList<>(Arrays.asList("a", "b", "c"));
list.replaceAll(s -> s.toUpperCase());
System.out.println(list);
```

**R:** `[A, B, C]`

`replaceAll()` applique une fonction à **chaque élément** et remplace in-place.

---

## Questions sur les Set

**Q11: Que s'affiche-t-il?**
```java
Set<String> set = new HashSet<>();
set.add("Alice");
set.add("Bob");
set.add("Alice");
System.out.println(set.size());
```

**R:** `2`

Les **Set** n'autorisent **pas les doublons**. Le second "Alice" est ignoré.

---

**Q12: Quelle est la différence entre HashSet, LinkedHashSet et TreeSet?**

**R:**
- **HashSet**: **pas d'ordre**, O(1), permet `null`
- **LinkedHashSet**: **ordre d'insertion**, O(1), permet `null`
- **TreeSet**: **ordre trié**, O(log n), **pas de `null`**

```java
Set<String> hash = new HashSet<>();
hash.add("C"); hash.add("A"); hash.add("B");
System.out.println(hash);  // Ordre aléatoire: [A, B, C] ou [B, C, A]...

Set<String> linked = new LinkedHashSet<>();
linked.add("C"); linked.add("A"); linked.add("B");
System.out.println(linked);  // [C, A, B] (ordre d'insertion)

Set<String> tree = new TreeSet<>();
tree.add("C"); tree.add("A"); tree.add("B");
System.out.println(tree);  // [A, B, C] (ordre trié)
```

---

**Q13: Ce code compile-t-il?**
```java
Set<Integer> set = new TreeSet<>();
set.add(null);
```

**R:** Compile mais lance **NullPointerException** à l'exécution.

**TreeSet n'accepte pas `null`** (car il doit comparer les éléments).

---

**Q14: Comment faire une union de deux sets?**
```java
Set<Integer> set1 = new HashSet<>(Arrays.asList(1, 2, 3));
Set<Integer> set2 = new HashSet<>(Arrays.asList(3, 4, 5));
// Union ?
```

**R:**
```java
Set<Integer> union = new HashSet<>(set1);
union.addAll(set2);
System.out.println(union);  // [1, 2, 3, 4, 5]
```

---

**Q15: Comment faire une intersection de deux sets?**
```java
Set<Integer> set1 = new HashSet<>(Arrays.asList(1, 2, 3, 4));
Set<Integer> set2 = new HashSet<>(Arrays.asList(3, 4, 5, 6));
// Intersection ?
```

**R:**
```java
Set<Integer> intersection = new HashSet<>(set1);
intersection.retainAll(set2);
System.out.println(intersection);  // [3, 4]
```

---

## Questions sur les Queue et Deque

**Q16: Quelle est la différence entre add() et offer() pour Queue?**
```java
Queue<String> queue = new LinkedList<>();
queue.add("A");      // ?
queue.offer("B");    // ?
```

**R:**
- **add()**: lance une **exception** si l'opération échoue
- **offer()**: retourne **false** si l'opération échoue

Pour les queues sans limite de capacité (comme LinkedList), pas de différence pratique.

---

**Q17: Quelle est la différence entre remove() et poll()?**
```java
Queue<String> queue = new LinkedList<>();
String s1 = queue.remove();  // ?
String s2 = queue.poll();    // ?
```

**R:**
- **remove()**: lance **NoSuchElementException** si vide
- **poll()**: retourne **null** si vide

```java
Queue<String> queue = new LinkedList<>();
// String s = queue.remove();  // NoSuchElementException
String s = queue.poll();       // null
```

---

**Q18: Quelle est la différence entre element() et peek()?**
```java
Queue<String> queue = new LinkedList<>();
String s1 = queue.element();  // ?
String s2 = queue.peek();     // ?
```

**R:**
- **element()**: lance **NoSuchElementException** si vide
- **peek()**: retourne **null** si vide

Les deux **examinent** sans retirer.

---

**Q19: Comment utiliser ArrayDeque comme Stack?**
```java
Deque<Integer> stack = new ArrayDeque<>();
// LIFO (Last In, First Out)
```

**R:**
```java
stack.push(1);
stack.push(2);
stack.push(3);

System.out.println(stack.pop());  // 3
System.out.println(stack.pop());  // 2
System.out.println(stack.pop());  // 1
```

`push()` et `pop()` pour comportement LIFO (Stack).

---

**Q20: Dans quel ordre PriorityQueue retire-t-elle les éléments?**
```java
Queue<Integer> pq = new PriorityQueue<>();
pq.offer(5);
pq.offer(2);
pq.offer(8);

while (!pq.isEmpty()) {
    System.out.println(pq.poll());
}
```

**R:**
```
2
5
8
```

**PriorityQueue** retire les éléments dans l'**ordre naturel** (croissant pour les nombres).

---

## Questions sur les Map

**Q21: Ce code compile-t-il?**
```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);
map.put("Alice", 30);
System.out.println(map.get("Alice"));
```

**R:** Oui, compile et affiche `30`.

Si on `put()` avec une clé **existante**, la valeur est **remplacée**.

---

**Q22: Quelle est la différence?**
```java
// Version 1
Integer age = map.get("Bob");

// Version 2
Integer age = map.getOrDefault("Bob", 0);
```

**R:**
- **get()**: retourne **null** si la clé n'existe pas
- **getOrDefault()**: retourne la **valeur par défaut** si la clé n'existe pas

```java
Map<String, Integer> map = new HashMap<>();
System.out.println(map.get("Bob"));              // null
System.out.println(map.getOrDefault("Bob", 0));  // 0
```

---

**Q23: Que fait putIfAbsent()?**
```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);

map.putIfAbsent("Alice", 30);
map.putIfAbsent("Bob", 30);

System.out.println(map);
```

**R:** `{Alice=25, Bob=30}`

`putIfAbsent()` ajoute **uniquement si la clé n'existe pas**.

---

**Q24: Comment compter les occurrences de mots avec Map?**
```java
List<String> words = Arrays.asList("apple", "banana", "apple", "cherry", "banana", "apple");
// Compter ?
```

**R:**
```java
Map<String, Integer> count = new HashMap<>();
for (String word : words) {
    count.merge(word, 1, Integer::sum);
}
System.out.println(count);  // {apple=3, banana=2, cherry=1}
```

`merge()` est parfait pour compter.

---

**Q25: Comment parcourir une Map?**
```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);
map.put("Bob", 30);
// Parcourir ?
```

**R:** **4 façons principales**:

```java
// 1. forEach (Java 8+)
map.forEach((key, value) -> System.out.println(key + ": " + value));

// 2. entrySet
for (Map.Entry<String, Integer> entry : map.entrySet()) {
    System.out.println(entry.getKey() + ": " + entry.getValue());
}

// 3. keySet
for (String key : map.keySet()) {
    System.out.println(key + ": " + map.get(key));
}

// 4. values (seulement les valeurs)
for (Integer value : map.values()) {
    System.out.println(value);
}
```

---

**Q26: Quelle est la différence entre HashMap, LinkedHashMap et TreeMap?**

**R:**
- **HashMap**: **pas d'ordre**, O(1)
- **LinkedHashMap**: **ordre d'insertion**, O(1)
- **TreeMap**: **clés triées**, O(log n)

```java
Map<String, Integer> hash = new HashMap<>();
hash.put("C", 3); hash.put("A", 1); hash.put("B", 2);
System.out.println(hash);  // Ordre aléatoire

Map<String, Integer> linked = new LinkedHashMap<>();
linked.put("C", 3); linked.put("A", 1); linked.put("B", 2);
System.out.println(linked);  // {C=3, A=1, B=2}

Map<String, Integer> tree = new TreeMap<>();
tree.put("C", 3); tree.put("A", 1); tree.put("B", 2);
System.out.println(tree);  // {A=1, B=2, C=3}
```

---

**Q27: Que fait compute()?**
```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);

map.compute("Alice", (key, value) -> value + 1);
map.compute("Bob", (key, value) -> value == null ? 1 : value + 1);

System.out.println(map);
```

**R:** `{Alice=26, Bob=1}`

`compute()` calcule une nouvelle valeur basée sur la clé et la valeur actuelle.

---

**Q28: Que retourne keySet()?**
```java
Map<String, Integer> map = new HashMap<>();
map.put("Alice", 25);
map.put("Bob", 30);

Set<String> keys = map.keySet();
keys.remove("Alice");
System.out.println(map);
```

**R:** `{Bob=30}`

`keySet()` retourne une **vue** (backed by map). Supprimer de `keys` supprime aussi de la `map`.

---

## Questions sur le Tri

**Q29: Quelle est la différence entre Comparable et Comparator?**

**R:**
- **Comparable**: définit l'**ordre naturel** (méthode `compareTo()` dans la classe)
- **Comparator**: définit un **ordre custom** (classe externe, méthode `compare()`)

```java
// Comparable (ordre naturel)
public class Person implements Comparable<Person> {
    private int age;
    
    @Override
    public int compareTo(Person other) {
        return this.age - other.age;
    }
}

// Comparator (ordre custom)
Comparator<Person> byName = (p1, p2) -> p1.getName().compareTo(p2.getName());
```

---

**Q30: Que retourne compareTo()?**
```java
String s1 = "Apple";
String s2 = "Banana";
int result = s1.compareTo(s2);
```

**R:** **Négatif** (< 0)

Règles:
- **< 0**: `this` avant `other`
- **= 0**: égaux
- **> 0**: `this` après `other`

"Apple" vient avant "Banana" alphabétiquement.

---

**Q31: Comment trier en ordre décroissant?**
```java
List<Integer> numbers = new ArrayList<>(Arrays.asList(5, 2, 8, 1, 9));
// Trier en ordre décroissant ?
```

**R:**
```java
numbers.sort(Comparator.reverseOrder());
System.out.println(numbers);  // [9, 8, 5, 2, 1]

// OU
Collections.sort(numbers, Collections.reverseOrder());
```

---

**Q32: Comment trier par plusieurs critères?**
```java
List<Person> people = ...;
// Trier par âge puis par nom ?
```

**R:**
```java
Comparator<Person> comparator = Comparator
    .comparing(Person::getAge)
    .thenComparing(Person::getName);

people.sort(comparator);
```

`thenComparing()` ajoute un critère secondaire.

---

**Q33: Comment gérer les null dans un tri?**
```java
List<String> list = Arrays.asList("B", null, "A", "C");
// Trier avec null ?
```

**R:**
```java
list.sort(Comparator.nullsFirst(String::compareTo));
System.out.println(list);  // [null, A, B, C]

// OU
list.sort(Comparator.nullsLast(String::compareTo));
System.out.println(list);  // [A, B, C, null]
```

---

## Questions sur les Generics

**Q34: Ce code compile-t-il?**
```java
List<String> list = new ArrayList<String>();
list.add("Hello");
list.add(123);
```

**R:** Non, **erreur de compilation** sur `list.add(123)`.

Les **generics** assurent la **type safety** à la compilation. On ne peut ajouter que des `String`.

---

**Q35: Quelle est la différence?**
```java
// Version 1
List list = new ArrayList();

// Version 2
List<String> list = new ArrayList<>();
```

**R:**
- **Version 1**: **Raw type** (pas de type safety, comme avant Java 5)
- **Version 2**: **Generic** (type safety)

```java
List list = new ArrayList();
list.add("Hello");
list.add(123);        // OK mais dangereux
String s = (String) list.get(1);  // ClassCastException!

List<String> list2 = new ArrayList<>();
list2.add("Hello");
// list2.add(123);    // ERREUR de compilation
```

---

**Q36: Qu'est-ce que le diamond operator (<>)?**
```java
List<String> list = new ArrayList<>();
```

**R:** Le **diamond operator** `<>` (Java 7+) permet au compilateur d'**inférer** le type.

Équivalent à:
```java
List<String> list = new ArrayList<String>();
```

---

**Q37: Ce code compile-t-il?**
```java
public class Box<T> {
    private T content;
    
    public Box() {
        content = new T();
    }
}
```

**R:** Non, **erreur de compilation**: "cannot instantiate the type T".

On ne peut **pas** créer une instance d'un type paramétré (à cause du type erasure).

---

**Q38: Quelle est la différence?**
```java
// Version 1
public static void print(List<Object> list) {
    list.forEach(System.out::println);
}

// Version 2
public static void print(List<?> list) {
    list.forEach(System.out::println);
}

List<String> strings = Arrays.asList("A", "B");
```

**R:**
- **Version 1**: n'accepte que `List<Object>` exactement

```java
// print(strings);  // ERREUR de compilation
```

- **Version 2**: accepte `List` de **n'importe quel type**

```java
print(strings);  // OK
```

---

**Q39: Quelle est la différence entre `<?>`, `<? extends T>` et `<? super T>`?**

**R:**
- **`<?>`**: unbounded wildcard (n'importe quel type)
- **`<? extends T>`**: upper bound (T ou sous-types) - **LECTURE**
- **`<? super T>`**: lower bound (T ou super-types) - **ÉCRITURE**

```java
// Lecture (extends)
public static double sum(List<? extends Number> numbers) {
    double sum = 0;
    for (Number n : numbers) {  // Lecture OK
        sum += n.doubleValue();
    }
    // numbers.add(5);  // ERREUR: écriture interdite
    return sum;
}

// Écriture (super)
public static void addIntegers(List<? super Integer> list) {
    list.add(1);  // Écriture OK
    list.add(2);
    // Integer i = list.get(0);  // ERREUR: lecture retourne Object
}
```

---

**Q40: Qu'est-ce que la règle PECS?**

**R:** **PECS**: **Producer Extends, Consumer Super**

- **Producer** (fournit/lit des données): utiliser `<? extends T>`
- **Consumer** (consomme/écrit des données): utiliser `<? super T>`

```java
// Producer (lit)
public void readFrom(List<? extends Number> source) {
    for (Number n : source) {  // Lecture
        System.out.println(n);
    }
}

// Consumer (écrit)
public void writeTo(List<? super Integer> dest) {
    dest.add(1);  // Écriture
    dest.add(2);
}
```

---

**Q41: Ce code compile-t-il?**
```java
List<? extends Number> list = new ArrayList<Integer>();
list.add(5);
```

**R:** Non, **erreur de compilation** sur `list.add(5)`.

Avec `<? extends Number>`, on ne peut **pas écrire** dans la liste (sauf `null`).

---

**Q42: Ce code compile-t-il?**
```java
List<? super Integer> list = new ArrayList<Number>();
list.add(5);
Integer i = list.get(0);
```

**R:** 
- `list.add(5)` : ✅ OK
- `Integer i = list.get(0)` : ❌ **Erreur de compilation**

Avec `<? super Integer>`, on peut **écrire** mais la **lecture** retourne `Object`.

**Correct**:
```java
Object obj = list.get(0);
```

---

**Q43: Que fait le type erasure?**
```java
List<String> list1 = new ArrayList<>();
List<Integer> list2 = new ArrayList<>();

System.out.println(list1.getClass() == list2.getClass());
```

**R:** `true`

À l'exécution, les types génériques sont **effacés**. Les deux listes ont le même type: `ArrayList`.

---

**Q44: Ce code compile-t-il?**
```java
public class Box<T extends Number> {
    private T value;
    
    public double doubleValue() {
        return value.doubleValue();  // ?
    }
}
```

**R:** Oui, compile.

Avec `<T extends Number>`, on sait que `T` est un `Number`, donc on peut appeler les méthodes de `Number`.

---

**Q45: Comment créer une méthode générique?**
```java
// Méthode qui inverse un array de n'importe quel type
```

**R:**
```java
public static <T> void reverse(T[] array) {
    for (int i = 0; i < array.length / 2; i++) {
        T temp = array[i];
        array[i] = array[array.length - 1 - i];
        array[array.length - 1 - i] = temp;
    }
}

// Utilisation
String[] strings = {"A", "B", "C"};
reverse(strings);
System.out.println(Arrays.toString(strings));  // [C, B, A]
```

Le `<T>` avant le return type indique une méthode générique.

---

## Questions Pièges

**Q46: Que s'affiche-t-il?**
```java
List<String> list = new ArrayList<>();
list.add("A");
list.add("B");
list.add("C");

list.remove("B");
list.remove(0);

System.out.println(list);
```

**R:** `[C]`

1. `remove("B")` supprime l'élément "B" → `[A, C]`
2. `remove(0)` supprime l'élément à l'index 0 (A) → `[C]`

---

**Q47: Que se passe-t-il?**
```java
List<Integer> list = new ArrayList<>();
list.add(1);
list.add(2);
list.add(3);

list.remove(1);
System.out.println(list);
```

**R:** `[1, 3]`

**Piège**: `remove(1)` supprime l'élément à l'**index 1** (pas la valeur 1).

Pour supprimer la valeur:
```java
list.remove(Integer.valueOf(1));  // Supprime la valeur 1
```

---

**Q48: Ce code compile-t-il?**
```java
List<String> list = Arrays.asList("A", "B", "C");
list.sort(null);
```

**R:** Oui, compile et trie en ordre naturel.

Passer `null` à `sort()` utilise l'**ordre naturel** (Comparable).

---

**Q49: Que retourne ce code?**
```java
TreeSet<String> set = new TreeSet<>();
set.add("B");
set.add("A");
set.add("C");

System.out.println(set.first());
System.out.println(set.last());
```

**R:**
```
A
C
```

`TreeSet` est trié. `first()` retourne le plus petit, `last()` le plus grand.

---

**Q50: Que s'affiche-t-il?**
```java
Map<String, Integer> map = new HashMap<>();
map.put("A", 1);
map.put("B", 2);

map.merge("A", 10, (old, newVal) -> old + newVal);
map.merge("C", 10, (old, newVal) -> old + newVal);

System.out.println(map);
```

**R:** `{A=11, B=2, C=10}`

`merge()`:
- Si clé existe: applique la fonction `old + newVal` → A: 1 + 10 = 11
- Si clé n'existe pas: ajoute la nouvelle valeur → C: 10

---

**Q51: Ce code produit-il une erreur?**
```java
List<String> list = new ArrayList<>();
list.add("A");
list.add("B");

for (String s : list) {
    System.out.println(s);
    list.add("C");
}
```

**R:** Oui, **ConcurrentModificationException**.

On ne peut pas **modifier** une collection pendant qu'on l'itère avec enhanced for loop.

---

**Q52: Quelle collection utiliser pour un cache LRU?**

**R:** **LinkedHashMap** avec `accessOrder = true`.

```java
Map<String, Integer> cache = new LinkedHashMap<>(16, 0.75f, true) {
    protected boolean removeEldestEntry(Map.Entry eldest) {
        return size() > 3;  // Max 3 éléments
    }
};
```

---

**Q53: Comment créer une List immuable et non-vide?**
```java
// Option 1
List<String> list1 = List.of("A", "B", "C");

// Option 2
List<String> list2 = Collections.unmodifiableList(
    new ArrayList<>(Arrays.asList("A", "B", "C"))
);
```

**R:** Les **deux** créent des listes immuables.

`List.of()` est plus concis (Java 9+).

---

**Q54: Que retourne ce comparateur?**
```java
Comparator<String> comp = Comparator.comparing(String::length)
                                     .thenComparing(String::compareTo);

List<String> words = Arrays.asList("Apple", "Pie", "Banana", "Cat");
words.sort(comp);
System.out.println(words);
```

**R:** `[Cat, Pie, Apple, Banana]`

Tri par:
1. **Longueur** (3, 3, 5, 6)
2. **Ordre alphabétique** pour longueurs égales (Cat avant Pie)

---

**Q55: Ce code compile-t-il?**
```java
List<String> list = new ArrayList<>();
List<Object> objectList = list;
```

**R:** Non, **erreur de compilation**: "incompatible types".

`List<String>` n'est **pas** un sous-type de `List<Object>` (invariance des generics).

**Correct** (avec wildcard):
```java
List<? extends Object> objectList = list;  // OK
```

---