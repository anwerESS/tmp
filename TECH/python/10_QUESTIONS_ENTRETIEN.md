## [**..**](./00_index.md)

# Chapitre 10 : Questions d'Entretien Courantes

## 🎯 Objectifs du chapitre
- Répondre aux questions théoriques fréquentes
- Résoudre des exercices de code typiques
- Éviter les pièges classiques
- Se préparer efficacement à l'entretien technique

---

## 10.1 Questions théoriques

### Fondamentaux Python

**Q1: Quelle est la différence entre une liste et un tuple ?**

**Réponse :**
- **Mutabilité** : Liste est mutable, tuple est immuable
- **Syntaxe** : `[1, 2, 3]` vs `(1, 2, 3)`
- **Performance** : Tuple légèrement plus rapide et moins de mémoire
- **Usage** : Liste pour collections modifiables, tuple pour données fixes
- **Hashable** : Tuple peut être clé de dict, pas la liste

```python
# Liste (mutable)
my_list = [1, 2, 3]
my_list[0] = 10  # OK
my_list.append(4)  # OK

# Tuple (immutable)
my_tuple = (1, 2, 3)
# my_tuple[0] = 10  # TypeError
# my_tuple.append(4)  # AttributeError

# Tuple comme clé de dict
positions = {(0, 0): 'origine', (1, 1): 'diagonale'}  # OK
# positions = {[0, 0]: 'origine'}  # TypeError
```

---

**Q2: Qu'est-ce que le GIL (Global Interpreter Lock) ?**

**Réponse :**
- **Définition** : Mutex qui protège l'accès aux objets Python
- **Impact** : Un seul thread Python s'exécute à la fois (dans CPython)
- **Conséquence** : Multithreading Python inefficace pour calculs CPU-intensifs
- **Solutions** :
  - Multiprocessing pour parallélisme réel
  - Threading OK pour I/O-bound (réseau, fichiers)
  - asyncio pour I/O asynchrone
  - Utiliser des extensions C (NumPy, etc.)

```python
# Threading (un thread à la fois à cause du GIL)
import threading

def cpu_task():
    total = sum(i**2 for i in range(10000000))

# Pas plus rapide avec threads
threads = [threading.Thread(target=cpu_task) for _ in range(4)]

# Multiprocessing (vrai parallélisme)
from multiprocessing import Pool

def cpu_task(n):
    return sum(i**2 for i in range(n))

with Pool(4) as pool:
    results = pool.map(cpu_task, [10000000] * 4)  # Vraiment parallèle
```

---

**Q3: Différence entre `==` et `is` ?**

**Réponse :**
- **`==`** : Compare les **valeurs** (appelle `__eq__`)
- **`is`** : Compare l'**identité** (même objet en mémoire)

```python
a = [1, 2, 3]
b = [1, 2, 3]
c = a

print(a == b)  # True (même contenu)
print(a is b)  # False (objets différents)
print(a is c)  # True (même objet)

# Cas particulier : petits entiers et strings (interning)
x = 256
y = 256
print(x is y)  # True (CPython optimise -5 à 256)

x = 257
y = 257
print(x is y)  # False (hors de la plage optimisée)

# Règle : toujours utiliser `is` pour None, True, False
value = None
if value is None:  # ✅ Correct
    pass

if value == None:  # ❌ Déconseillé
    pass
```

---

**Q4: Qu'est-ce qu'un décorateur ?**

**Réponse :**
- Fonction qui prend une fonction et retourne une fonction modifiée
- Utilise la syntaxe `@decorator`
- Permet d'ajouter des fonctionnalités sans modifier le code original

```python
def timer(func):
    """Décorateur qui mesure le temps d'exécution."""
    import time
    from functools import wraps
    
    @wraps(func)
    def wrapper(*args, **kwargs):
        start = time.time()
        result = func(*args, **kwargs)
        end = time.time()
        print(f"{func.__name__} a pris {end - start:.4f}s")
        return result
    return wrapper

@timer
def slow_function():
    time.sleep(1)
    return "Done"

# Équivalent à:
# slow_function = timer(slow_function)

slow_function()  # Affiche le temps d'exécution
```

---

**Q5: Différence entre shallow copy et deep copy ?**

**Réponse :**
- **Shallow copy** : Copie l'objet mais pas les objets imbriqués
- **Deep copy** : Copie récursive complète

```python
import copy

# Liste avec objets imbriqués
original = [[1, 2, 3], [4, 5, 6]]

# Shallow copy
shallow = original.copy()
shallow = original[:]
shallow = list(original)

# Deep copy
deep = copy.deepcopy(original)

# Test
original[0][0] = 999

print(original)  # [[999, 2, 3], [4, 5, 6]]
print(shallow)   # [[999, 2, 3], [4, 5, 6]] - Affecté !
print(deep)      # [[1, 2, 3], [4, 5, 6]] - Non affecté
```

---

**Q6: Qu'est-ce qu'un générateur et pourquoi l'utiliser ?**

**Réponse :**
- Fonction qui utilise `yield` au lieu de `return`
- Retourne un itérateur paresseux (lazy)
- **Avantages** :
  - Économie mémoire (un élément à la fois)
  - Peut être infini
  - Bon pour pipelines de traitement

```python
# Fonction normale (tout en mémoire)
def numbers_list(n):
    result = []
    for i in range(n):
        result.append(i)
    return result

# Générateur (paresseux)
def numbers_generator(n):
    for i in range(n):
        yield i

# Comparaison mémoire
import sys
lst = numbers_list(10000)
gen = numbers_generator(10000)

print(sys.getsizeof(lst))  # ~90KB
print(sys.getsizeof(gen))  # ~200 bytes

# Usage
for num in numbers_generator(5):
    print(num)  # Calcule au fur et à mesure
```

---

**Q7: Expliquer `*args` et `**kwargs`**

**Réponse :**
- **`*args`** : Nombre variable d'arguments positionnels (tuple)
- **`**kwargs`** : Nombre variable d'arguments nommés (dict)

```python
def function(*args, **kwargs):
    print(f"args: {args}")      # Tuple
    print(f"kwargs: {kwargs}")  # Dict

function(1, 2, 3, a=4, b=5)
# args: (1, 2, 3)
# kwargs: {'a': 4, 'b': 5}

# Unpacking
def add(a, b, c):
    return a + b + c

numbers = [1, 2, 3]
print(add(*numbers))  # Équivalent à add(1, 2, 3)

params = {'a': 1, 'b': 2, 'c': 3}
print(add(**params))  # Équivalent à add(a=1, b=2, c=3)
```

---

**Q8: Différence entre une méthode de classe et une méthode statique ?**

**Réponse :**
- **Méthode d'instance** : Reçoit `self`, accède à l'instance
- **Méthode de classe** : Reçoit `cls`, accède à la classe
- **Méthode statique** : Ne reçoit ni `self` ni `cls`, fonction utilitaire

```python
class MyClass:
    class_var = "classe"
    
    def instance_method(self):
        """Accède à self et à la classe."""
        return f"{self} - {self.class_var}"
    
    @classmethod
    def class_method(cls):
        """Factory method, accède à la classe."""
        return cls()
    
    @staticmethod
    def static_method(x, y):
        """Fonction utilitaire, pas d'accès."""
        return x + y

# Usage
obj = MyClass()
obj.instance_method()           # Nécessite une instance
MyClass.class_method()          # Peut créer une instance
MyClass.static_method(1, 2)     # Fonction simple
```

---

**Q9: Qu'est-ce que `__init__` et `__new__` ?**

**Réponse :**
- **`__new__`** : Crée l'instance (constructeur)
- **`__init__`** : Initialise l'instance déjà créée

```python
class MyClass:
    def __new__(cls, *args, **kwargs):
        """Appelé en premier, crée l'instance."""
        print("__new__")
        instance = super().__new__(cls)
        return instance
    
    def __init__(self):
        """Appelé après __new__, initialise."""
        print("__init__")

obj = MyClass()
# Affiche:
# __new__
# __init__

# Cas d'usage de __new__ : Singleton
class Singleton:
    _instance = None
    
    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
        return cls._instance

s1 = Singleton()
s2 = Singleton()
print(s1 is s2)  # True (même instance)
```

---

**Q10: Qu'est-ce que la "duck typing" ?**

**Réponse :**
- "If it walks like a duck and quacks like a duck, it's a duck"
- Type basé sur le comportement, pas sur l'héritage
- Python utilise duck typing (pas de vérification de type stricte)

```python
# Pas besoin d'héritage commun
class Duck:
    def quack(self):
        return "Quack!"
    
    def walk(self):
        return "Walk like a duck"

class Person:
    def quack(self):
        return "I'm imitating a duck!"
    
    def walk(self):
        return "Walk like a person"

def make_it_quack(thing):
    """Accepte n'importe quoi avec une méthode quack()."""
    print(thing.quack())

make_it_quack(Duck())    # Fonctionne
make_it_quack(Person())  # Fonctionne aussi !

# EAFP (Easier to Ask Forgiveness than Permission)
try:
    thing.quack()
except AttributeError:
    print("Can't quack")
```

---

## 10.2 Exercices de code pratiques

### Exercice 1 : FizzBuzz

**Énoncé :** Écrivez un programme qui affiche les nombres de 1 à 100. Pour les multiples de 3, affichez "Fizz", pour les multiples de 5, "Buzz", et pour les multiples des deux, "FizzBuzz".

```python
# Solution classique
def fizzbuzz(n):
    for i in range(1, n + 1):
        if i % 15 == 0:
            print("FizzBuzz")
        elif i % 3 == 0:
            print("Fizz")
        elif i % 5 == 0:
            print("Buzz")
        else:
            print(i)

# Solution pythonique
def fizzbuzz_pythonic(n):
    for i in range(1, n + 1):
        output = ""
        if i % 3 == 0:
            output += "Fizz"
        if i % 5 == 0:
            output += "Buzz"
        print(output or i)

# Solution avec compréhension
def fizzbuzz_oneliner(n):
    return ["FizzBuzz" if i % 15 == 0 else "Fizz" if i % 3 == 0 
            else "Buzz" if i % 5 == 0 else i for i in range(1, n + 1)]

fizzbuzz(100)
```

---

### Exercice 2 : Vérifier si un mot est un palindrome

**Énoncé :** Écrivez une fonction qui vérifie si un mot est un palindrome.

```python
def is_palindrome(word):
    """Version simple."""
    return word == word[::-1]

def is_palindrome_ignore_case(word):
    """Ignore la casse et les espaces."""
    word = word.lower().replace(" ", "")
    return word == word[::-1]

def is_palindrome_optimized(word):
    """Plus efficace (arrête dès la première différence)."""
    left, right = 0, len(word) - 1
    while left < right:
        if word[left] != word[right]:
            return False
        left += 1
        right -= 1
    return True

# Tests
print(is_palindrome("radar"))      # True
print(is_palindrome("python"))     # False
print(is_palindrome_ignore_case("A man a plan a canal Panama"))  # True
```

---

### Exercice 3 : Trouver les anagrammes

**Énoncé :** Écrivez une fonction qui vérifie si deux mots sont des anagrammes.

```python
def are_anagrams(word1, word2):
    """Solution simple avec sorted."""
    return sorted(word1.lower()) == sorted(word2.lower())

def are_anagrams_counter(word1, word2):
    """Solution avec Counter."""
    from collections import Counter
    return Counter(word1.lower()) == Counter(word2.lower())

def group_anagrams(words):
    """Grouper les anagrammes ensemble."""
    from collections import defaultdict
    
    anagrams = defaultdict(list)
    for word in words:
        key = "".join(sorted(word.lower()))
        anagrams[key].append(word)
    
    return list(anagrams.values())

# Tests
print(are_anagrams("listen", "silent"))  # True
print(are_anagrams("hello", "world"))    # False

words = ["eat", "tea", "tan", "ate", "nat", "bat"]
print(group_anagrams(words))
# [['eat', 'tea', 'ate'], ['tan', 'nat'], ['bat']]
```

---

### Exercice 4 : Suite de Fibonacci

**Énoncé :** Implémentez la suite de Fibonacci de plusieurs façons.

```python
# Récursif (inefficace)
def fibonacci_recursive(n):
    if n < 2:
        return n
    return fibonacci_recursive(n-1) + fibonacci_recursive(n-2)

# Récursif avec cache
from functools import lru_cache

@lru_cache(maxsize=None)
def fibonacci_cached(n):
    if n < 2:
        return n
    return fibonacci_cached(n-1) + fibonacci_cached(n-2)

# Itératif (efficace)
def fibonacci_iterative(n):
    if n < 2:
        return n
    a, b = 0, 1
    for _ in range(n - 1):
        a, b = b, a + b
    return b

# Générateur (pour séquence)
def fibonacci_generator(limit):
    a, b = 0, 1
    count = 0
    while count < limit:
        yield a
        a, b = b, a + b
        count += 1

# Tests
print(fibonacci_iterative(10))  # 55
print(list(fibonacci_generator(10)))  # [0, 1, 1, 2, 3, 5, 8, 13, 21, 34]
```

---

### Exercice 5 : Compter les occurrences de caractères

**Énoncé :** Comptez les occurrences de chaque caractère dans une string.

```python
def count_chars(text):
    """Solution avec dict."""
    counts = {}
    for char in text:
        counts[char] = counts.get(char, 0) + 1
    return counts

def count_chars_counter(text):
    """Solution avec Counter."""
    from collections import Counter
    return Counter(text)

def count_chars_defaultdict(text):
    """Solution avec defaultdict."""
    from collections import defaultdict
    counts = defaultdict(int)
    for char in text:
        counts[char] += 1
    return dict(counts)

# Tests
text = "hello world"
print(count_chars(text))
# {'h': 1, 'e': 1, 'l': 3, 'o': 2, ' ': 1, 'w': 1, 'r': 1, 'd': 1}

# Caractères les plus fréquents
from collections import Counter
counts = Counter(text)
print(counts.most_common(3))  # [('l', 3), ('o', 2), ('h', 1)]
```

---

### Exercice 6 : Inverser une string/liste

**Énoncé :** Inversez une string ou une liste.

```python
# String
text = "hello"

# Méthode 1 : slicing (le plus pythonique)
reversed_text = text[::-1]

# Méthode 2 : reversed() + join
reversed_text = "".join(reversed(text))

# Méthode 3 : boucle
reversed_text = ""
for char in text:
    reversed_text = char + reversed_text

# Liste
numbers = [1, 2, 3, 4, 5]

# Méthode 1 : slicing
reversed_numbers = numbers[::-1]

# Méthode 2 : reversed()
reversed_numbers = list(reversed(numbers))

# Méthode 3 : reverse() in-place
numbers.reverse()  # Modifie la liste

# Méthode 4 : boucle avec indices
for i in range(len(numbers) // 2):
    numbers[i], numbers[-(i+1)] = numbers[-(i+1)], numbers[i]
```

---

### Exercice 7 : Trouver les doublons

**Énoncé :** Trouvez les éléments en double dans une liste.

```python
def find_duplicates(items):
    """Solution avec set."""
    seen = set()
    duplicates = set()
    
    for item in items:
        if item in seen:
            duplicates.add(item)
        else:
            seen.add(item)
    
    return list(duplicates)

def find_duplicates_counter(items):
    """Solution avec Counter."""
    from collections import Counter
    counts = Counter(items)
    return [item for item, count in counts.items() if count > 1]

def remove_duplicates(items):
    """Supprimer les doublons en gardant l'ordre."""
    seen = set()
    result = []
    for item in items:
        if item not in seen:
            seen.add(item)
            result.append(item)
    return result

# Tests
numbers = [1, 2, 3, 2, 4, 5, 3, 6]
print(find_duplicates(numbers))        # [2, 3]
print(remove_duplicates(numbers))      # [1, 2, 3, 4, 5, 6]
```

---

### Exercice 8 : Deux sommes (Two Sum)

**Énoncé :** Trouvez deux nombres dans une liste qui somment à une cible.

```python
def two_sum(numbers, target):
    """Solution avec dict (O(n))."""
    seen = {}
    
    for i, num in enumerate(numbers):
        complement = target - num
        if complement in seen:
            return [seen[complement], i]
        seen[num] = i
    
    return None

def two_sum_brute_force(numbers, target):
    """Solution brute force (O(n²))."""
    for i in range(len(numbers)):
        for j in range(i + 1, len(numbers)):
            if numbers[i] + numbers[j] == target:
                return [i, j]
    return None

# Tests
numbers = [2, 7, 11, 15]
target = 9
print(two_sum(numbers, target))  # [0, 1] (2 + 7 = 9)
```

---

### Exercice 9 : Aplatir une liste imbriquée

**Énoncé :** Aplatissez une liste de listes.

```python
def flatten(nested_list):
    """Solution récursive."""
    result = []
    for item in nested_list:
        if isinstance(item, list):
            result.extend(flatten(item))
        else:
            result.append(item)
    return result

def flatten_itertools(nested_list):
    """Solution avec itertools."""
    from itertools import chain
    return list(chain.from_iterable(nested_list))

def flatten_generator(nested_list):
    """Solution avec générateur."""
    for item in nested_list:
        if isinstance(item, list):
            yield from flatten_generator(item)
        else:
            yield item

# Tests
nested = [1, [2, 3, [4, 5]], 6, [7, [8, 9]]]
print(flatten(nested))
# [1, 2, 3, 4, 5, 6, 7, 8, 9]

# Pour listes simples (1 niveau)
nested_simple = [[1, 2], [3, 4], [5, 6]]
print(flatten_itertools(nested_simple))
# [1, 2, 3, 4, 5, 6]
```

---

### Exercice 10 : Validation d'email

**Énoncé :** Validez un email avec regex.

```python
import re

def is_valid_email(email):
    """Validation simple d'email."""
    pattern = r'^[\w\.-]+@[\w\.-]+\.\w+$'
    return bool(re.match(pattern, email))

def is_valid_email_strict(email):
    """Validation plus stricte."""
    pattern = r'^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$'
    return bool(re.match(pattern, email))

# Tests
print(is_valid_email("user@example.com"))      # True
print(is_valid_email("user.name@sub.domain.com"))  # True
print(is_valid_email("invalid.email"))         # False
print(is_valid_email("@example.com"))          # False
```

---

## 10.3 Pièges classiques

### Piège 1 : Arguments mutables par défaut

```python
# ❌ PROBLÈME
def add_item(item, list=[]):
    """Liste par défaut est partagée entre tous les appels !"""
    list.append(item)
    return list

print(add_item(1))  # [1]
print(add_item(2))  # [1, 2] - Surprise !
print(add_item(3))  # [1, 2, 3]

# ✅ SOLUTION
def add_item(item, list=None):
    """Créer une nouvelle liste à chaque appel."""
    if list is None:
        list = []
    list.append(item)
    return list

print(add_item(1))  # [1]
print(add_item(2))  # [2] - Correct !

# Pourquoi ? La valeur par défaut est évaluée UNE SEULE FOIS
# lors de la définition de la fonction, pas à chaque appel
```

---

### Piège 2 : Late binding closures

```python
# ❌ PROBLÈME
def create_multipliers():
    """Toutes les fonctions utilisent la dernière valeur de i."""
    multipliers = []
    for i in range(5):
        multipliers.append(lambda x: x * i)
    return multipliers

funcs = create_multipliers()
print([func(2) for func in funcs])  # [8, 8, 8, 8, 8] au lieu de [0, 2, 4, 6, 8]

# ✅ SOLUTION 1 : Argument par défaut
def create_multipliers():
    multipliers = []
    for i in range(5):
        multipliers.append(lambda x, i=i: x * i)  # Capture i immédiatement
    return multipliers

funcs = create_multipliers()
print([func(2) for func in funcs])  # [0, 2, 4, 6, 8] - Correct !

# ✅ SOLUTION 2 : Compréhension de liste
def create_multipliers():
    return [lambda x, i=i: x * i for i in range(5)]
```

---

### Piège 3 : Modification pendant l'itération

```python
# ❌ PROBLÈME
numbers = [1, 2, 3, 4, 5]
for num in numbers:
    if num % 2 == 0:
        numbers.remove(num)  # Modifie la liste pendant l'itération !

print(numbers)  # Résultat imprévisible

# ✅ SOLUTION 1 : Itérer sur une copie
numbers = [1, 2, 3, 4, 5]
for num in numbers[:]:  # Copie de la liste
    if num % 2 == 0:
        numbers.remove(num)

# ✅ SOLUTION 2 : List comprehension
numbers = [1, 2, 3, 4, 5]
numbers = [num for num in numbers if num % 2 != 0]

# ✅ SOLUTION 3 : filter
numbers = [1, 2, 3, 4, 5]
numbers = list(filter(lambda x: x % 2 != 0, numbers))
```

---

### Piège 4 : Scope des variables

```python
# ❌ PROBLÈME
x = 10

def func():
    print(x)  # Essaie d'accéder à x avant de l'assigner
    x = 5     # Crée une variable locale

# func()  # UnboundLocalError

# ✅ SOLUTION 1 : global
x = 10

def func():
    global x
    print(x)
    x = 5

# ✅ SOLUTION 2 : nonlocal (pour closures)
def outer():
    x = 10
    
    def inner():
        nonlocal x
        x = 5
    
    inner()
    print(x)  # 5

# ✅ SOLUTION 3 : Retourner la nouvelle valeur
def func(x):
    print(x)
    return 5

x = func(x)
```

---

### Piège 5 : Compréhension de liste et variable de boucle

```python
# Python 3 : OK (variable de boucle est locale)
x = [i for i in range(5)]
# print(i)  # NameError: 'i' is not defined

# Mais attention avec les walrus operator
y = [j := i for i in range(5)]
print(j)  # 4 (j existe dans la scope externe)

# Compréhension imbriquée
matrix = [[1, 2, 3], [4, 5, 6]]
flat = [num for row in matrix for num in row]
# Équivalent à:
# for row in matrix:
#     for num in row:
#         flat.append(num)
```

---

### Piège 6 : Types mutables comme clés de dict

```python
# ❌ ERREUR
# my_dict = {[1, 2]: "value"}  # TypeError: unhashable type: 'list'

# ✅ SOLUTION : Utiliser tuple
my_dict = {(1, 2): "value"}

# Types hashables : int, float, str, tuple (si contient que des hashables)
# Types non-hashables : list, dict, set
```

---

### Piège 7 : Égalité vs identité pour les singletons

```python
# ✅ Pour None, True, False : utiliser `is`
value = None
if value is None:  # Correct
    pass

if value == None:  # Fonctionne mais déconseillé
    pass

# ✅ Pour les valeurs : utiliser `==`
x = 1000
y = 1000
if x == y:  # Correct
    pass

if x is y:  # Peut être False (objets différents)
    pass
```

---

### Piège 8 : Import circulaire

```python
# file1.py
from file2 import func2

def func1():
    return func2()

# file2.py
from file1 import func1  # Import circulaire !

def func2():
    return func1()

# ✅ SOLUTION 1 : Import dans la fonction
# file1.py
def func1():
    from file2 import func2
    return func2()

# ✅ SOLUTION 2 : Restructurer le code
# utils.py
def shared_logic():
    pass

# file1.py
from utils import shared_logic
def func1():
    return shared_logic()

# file2.py
from utils import shared_logic
def func2():
    return shared_logic()
```

---

## 10.4 Conseils pour l'entretien

### Avant l'entretien

✅ **Réviser les bases**
- Types de données, structures de contrôle
- Compréhensions de listes
- Fonctions, classes de base
- Exceptions

✅ **Pratiquer le code**
- FizzBuzz, palindromes, anagrammes
- Manipulation de listes/dictionnaires
- Recursion simple (Fibonacci, factorielle)
- Recherche et tri

✅ **Connaître les modules standard**
- collections (defaultdict, Counter, deque)
- itertools (combinations, groupby)
- functools (lru_cache, partial)
- os, sys, re

✅ **Comprendre Big O**
- Complexité des structures Python
- O(1), O(n), O(n log n), O(n²)

### Pendant l'entretien

✅ **Clarifier le problème**
- Posez des questions
- Vérifiez les cas limites
- Confirmez les contraintes

✅ **Réfléchir à voix haute**
- Expliquez votre approche
- Discutez des trade-offs
- Mentionnez les alternatives

✅ **Commencer simple**
- Solution brute force d'abord
- Puis optimiser
- Testez avec des exemples

✅ **Écrire du code propre**
- Noms de variables clairs
- Commentaires si nécessaire
- PEP 8

✅ **Tester votre code**
- Cas nominal
- Cas limites (vide, un élément, etc.)
- Cas d'erreur

### Structure de réponse recommandée

```python
"""
1. Comprendre le problème
   - Reformuler
   - Identifier entrées/sorties
   - Cas limites

2. Proposer une approche
   - Solution simple d'abord
   - Complexité estimée
   - Optimisations possibles

3. Implémenter
   - Code clair et testé
   - Gestion d'erreurs si nécessaire

4. Tester
   - Cas normaux
   - Cas limites
   - Vérifier complexité

5. Discuter
   - Améliorations possibles
   - Trade-offs
   - Extensions
"""

def solve_problem(input_data):
    """
    Description claire du problème et de l'approche.
    
    Args:
        input_data: Description
    
    Returns:
        Description du résultat
    
    Complexity:
        Time: O(n)
        Space: O(1)
    """
    # Validation des entrées
    if not input_data:
        return []
    
    # Algorithme principal
    result = []
    # ... code ...
    
    return result

# Tests
assert solve_problem([]) == []
assert solve_problem([1]) == [1]
assert solve_problem([1, 2, 3]) == [1, 2, 3]
```

### Questions à poser à l'intervieweur

- Contraintes sur la complexité ?
- Taille typique des données ?
- Peut-on supposer que l'entrée est valide ?
- Y a-t-il des cas particuliers à gérer ?
- Peut-on utiliser des bibliothèques externes ?
- Performance vs lisibilité ?

---

## 🎯 Checklist finale

### Concepts à maîtriser absolument

- [ ] Types de données (list, dict, set, tuple)
- [ ] Compréhensions
- [ ] Générateurs et yield
- [ ] Décorateurs basiques
- [ ] Context managers (with)
- [ ] Exceptions (try/except/finally)
- [ ] Classes et héritage basique
- [ ] Modules et imports
- [ ] argparse pour CLI
- [ ] subprocess pour commandes
- [ ] Fichiers (lecture/écriture)
- [ ] JSON et CSV
- [ ] logging vs print
- [ ] Big O de base
- [ ] @lru_cache

### Pièges à éviter

- [ ] Arguments mutables par défaut
- [ ] Late binding closures
- [ ] Modification pendant itération
- [ ] Shallow vs deep copy
- [ ] `==` vs `is`
- [ ] Variables scope (global/nonlocal)
- [ ] Import circulaire

### Exercices à savoir faire

- [ ] FizzBuzz
- [ ] Palindrome
- [ ] Anagrammes
- [ ] Fibonacci
- [ ] Inverser string/liste
- [ ] Compter caractères
- [ ] Trouver doublons
- [ ] Two sum
- [ ] Aplatir liste
- [ ] Validation (email, etc.)

---

## 💡 Derniers conseils

**Restez calme** : C'est normal de ne pas tout savoir

**Communiquez** : Expliquez votre raisonnement

**Testez** : Vérifiez votre code avec des exemples

**Optimisez** : Discutez des améliorations possibles

**Soyez honnête** : Si vous ne savez pas, dites-le

**Posez des questions** : Montrez votre curiosité

---

**Bonne chance pour votre entretien ! 🚀**

Vous avez maintenant toutes les connaissances nécessaires. Faites confiance à votre préparation et montrez ce que vous savez faire !
