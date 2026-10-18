## [**..**](./00_index.md)

# Chapitre 1 : Fondamentaux Python

## 🎯 Objectifs du chapitre
- Maîtriser les types de données et structures Python
- Comprendre les opérateurs et expressions
- Utiliser efficacement les structures de contrôle
- Écrire des fonctions robustes et pythoniques

---

## 1.1 Types de données et structures

### Types primitifs

#### Entiers (int)
```python
# Python 3 : les entiers ont une précision illimitée
x = 42
y = -17
big_number = 123456789012345678901234567890

# Différentes bases
binary = 0b1010      # Binaire (10 en décimal)
octal = 0o12         # Octal (10 en décimal)
hexa = 0xFF          # Hexadécimal (255 en décimal)

# Opérations
print(10 // 3)       # Division entière : 3
print(10 % 3)        # Modulo : 1
print(2 ** 10)       # Puissance : 1024
```

#### Flottants (float)
```python
pi = 3.14159
e = 2.71828
scientific = 1.5e-3  # 0.0015

# Attention aux approximations !
print(0.1 + 0.2 == 0.3)  # False !
print(0.1 + 0.2)          # 0.30000000000000004

# Solution : utiliser math.isclose() ou round()
import math
print(math.isclose(0.1 + 0.2, 0.3))  # True
```

#### Chaînes de caractères (str)
```python
# Différentes façons de créer des strings
simple = 'Hello'
double = "World"
triple = '''Multiligne
avec plusieurs
lignes'''

# Formatage
name = "Alice"
age = 30

# f-strings (Python 3.6+) - RECOMMANDÉ
message = f"Je m'appelle {name} et j'ai {age} ans"

# format()
message = "Je m'appelle {} et j'ai {} ans".format(name, age)

# % (ancien style)
message = "Je m'appelle %s et j'ai %d ans" % (name, age)

# Méthodes courantes
text = "  Python Programming  "
print(text.strip())           # "Python Programming"
print(text.lower())           # "  python programming  "
print(text.upper())           # "  PYTHON PROGRAMMING  "
print(text.replace("P", "J")) # "  Jython Jrogramming  "
print(text.split())           # ['Python', 'Programming']

# Slicing
word = "Python"
print(word[0])      # 'P'
print(word[-1])     # 'n'
print(word[0:3])    # 'Pyt'
print(word[::-1])   # 'nohtyP' (inversé)
```

#### Booléens (bool)
```python
# Valeurs booléennes
is_valid = True
is_empty = False

# Valeurs "truthy" et "falsy"
# Falsy: False, None, 0, 0.0, '', [], {}, set()
# Truthy: tout le reste

if []:  # Liste vide est False
    print("Ne s'affiche pas")

if [1, 2, 3]:  # Liste non-vide est True
    print("S'affiche !")

# Opérateurs logiques
print(True and False)  # False
print(True or False)   # True
print(not True)        # False
```

### Collections

#### Listes (list) - Mutables et ordonnées
```python
# Création
fruits = ['pomme', 'banane', 'orange']
numbers = [1, 2, 3, 4, 5]
mixed = [1, 'hello', 3.14, True, [1, 2]]

# Accès
print(fruits[0])      # 'pomme'
print(fruits[-1])     # 'orange' (dernier élément)

# Modification
fruits[1] = 'poire'
fruits.append('kiwi')           # Ajoute à la fin
fruits.insert(0, 'fraise')      # Insère à l'index 0
fruits.extend(['mangue', 'papaye'])  # Ajoute plusieurs éléments

# Suppression
del fruits[0]                   # Supprime par index
fruits.remove('kiwi')           # Supprime par valeur
popped = fruits.pop()           # Retire et retourne le dernier
fruits.clear()                  # Vide la liste

# Méthodes utiles
numbers = [3, 1, 4, 1, 5, 9, 2, 6]
numbers.sort()                  # Trie sur place
sorted_nums = sorted(numbers)   # Retourne une nouvelle liste triée
numbers.reverse()               # Inverse sur place
print(numbers.count(1))         # Compte les occurrences de 1
print(numbers.index(4))         # Index de la première occurrence de 4

# Slicing
nums = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
print(nums[2:5])      # [2, 3, 4]
print(nums[:3])       # [0, 1, 2]
print(nums[7:])       # [7, 8, 9]
print(nums[::2])      # [0, 2, 4, 6, 8] (tous les 2)
print(nums[::-1])     # Inverse la liste
```

#### Tuples (tuple) - Immutables et ordonnés
```python
# Création
coordinates = (10, 20)
singleton = (42,)  # Attention à la virgule !
empty = ()

# Unpacking
x, y = coordinates
print(x, y)  # 10 20

# Tuples imbriqués
person = ('Alice', 30, ('Paris', 'France'))
name, age, (city, country) = person

# Les tuples sont immutables
# coordinates[0] = 15  # TypeError !

# Mais peuvent contenir des objets mutables
mixed_tuple = ([1, 2, 3], 'hello')
mixed_tuple[0].append(4)  # OK ! On modifie la liste, pas le tuple
print(mixed_tuple)  # ([1, 2, 3, 4], 'hello')

# Usage typique : retours multiples de fonctions
def get_stats(numbers):
    return min(numbers), max(numbers), sum(numbers)

minimum, maximum, total = get_stats([1, 2, 3, 4, 5])
```

#### Dictionnaires (dict) - Mutables, non ordonnés (ordonnés depuis Python 3.7+)
```python
# Création
person = {
    'name': 'Alice',
    'age': 30,
    'city': 'Paris'
}

# Autre syntaxe
person = dict(name='Alice', age=30, city='Paris')

# Accès
print(person['name'])           # 'Alice'
print(person.get('age'))        # 30
print(person.get('country', 'Unknown'))  # 'Unknown' (valeur par défaut)

# Modification
person['age'] = 31
person['country'] = 'France'    # Ajoute une nouvelle clé

# Suppression
del person['city']
age = person.pop('age')         # Retire et retourne la valeur
person.clear()                  # Vide le dictionnaire

# Méthodes utiles
person = {'name': 'Alice', 'age': 30, 'city': 'Paris'}

print(person.keys())            # dict_keys(['name', 'age', 'city'])
print(person.values())          # dict_values(['Alice', 30, 'Paris'])
print(person.items())           # dict_items([('name', 'Alice'), ...])

# Itération
for key in person:
    print(f"{key}: {person[key]}")

for key, value in person.items():
    print(f"{key}: {value}")

# Fusion de dictionnaires (Python 3.9+)
dict1 = {'a': 1, 'b': 2}
dict2 = {'c': 3, 'd': 4}
merged = dict1 | dict2  # {'a': 1, 'b': 2, 'c': 3, 'd': 4}

# Ancienne méthode
merged = {**dict1, **dict2}
```

#### Sets (set) - Mutables, non ordonnés, uniques
```python
# Création
fruits = {'pomme', 'banane', 'orange'}
numbers = set([1, 2, 3, 2, 1])  # {1, 2, 3} - les doublons sont supprimés
empty_set = set()  # ATTENTION : {} crée un dict vide, pas un set !

# Ajout/Suppression
fruits.add('kiwi')
fruits.remove('pomme')       # KeyError si absent
fruits.discard('mangue')     # Pas d'erreur si absent
popped = fruits.pop()        # Retire un élément arbitraire

# Opérations d'ensembles
a = {1, 2, 3, 4}
b = {3, 4, 5, 6}

print(a | b)    # Union : {1, 2, 3, 4, 5, 6}
print(a & b)    # Intersection : {3, 4}
print(a - b)    # Différence : {1, 2}
print(a ^ b)    # Différence symétrique : {1, 2, 5, 6}

# Méthodes équivalentes
print(a.union(b))
print(a.intersection(b))
print(a.difference(b))
print(a.symmetric_difference(b))

# Tests d'appartenance (très rapide O(1))
print(3 in a)           # True
print(a.issubset(b))    # False
print(a.issuperset(b))  # False
```

### Compréhensions (List/Dict/Set comprehensions)

#### List comprehension
```python
# Syntaxe de base : [expression for item in iterable if condition]

# Carrés des nombres de 0 à 9
squares = [x**2 for x in range(10)]
# [0, 1, 4, 9, 16, 25, 36, 49, 64, 81]

# Nombres pairs seulement
evens = [x for x in range(20) if x % 2 == 0]
# [0, 2, 4, 6, 8, 10, 12, 14, 16, 18]

# Avec transformation
words = ['hello', 'world', 'python']
upper_words = [w.upper() for w in words]
# ['HELLO', 'WORLD', 'PYTHON']

# Compréhension imbriquée
matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
flat = [num for row in matrix for num in row]
# [1, 2, 3, 4, 5, 6, 7, 8, 9]

# Équivalent avec boucles classiques
flat = []
for row in matrix:
    for num in row:
        flat.append(num)
```

#### Dict comprehension
```python
# Créer un dictionnaire de carrés
squares_dict = {x: x**2 for x in range(5)}
# {0: 0, 1: 1, 2: 4, 3: 9, 4: 16}

# Inverser un dictionnaire
original = {'a': 1, 'b': 2, 'c': 3}
inverted = {v: k for k, v in original.items()}
# {1: 'a', 2: 'b', 3: 'c'}

# Avec condition
even_squares = {x: x**2 for x in range(10) if x % 2 == 0}
# {0: 0, 2: 4, 4: 16, 6: 36, 8: 64}
```

#### Set comprehension
```python
# Créer un set de carrés
squares_set = {x**2 for x in range(10)}
# {0, 1, 4, 9, 16, 25, 36, 49, 64, 81}

# Avec condition
even_squares_set = {x**2 for x in range(10) if x % 2 == 0}
# {0, 4, 16, 36, 64}
```

---

## 1.2 Opérateurs et expressions

### Opérateurs arithmétiques
```python
a, b = 10, 3

print(a + b)    # Addition : 13
print(a - b)    # Soustraction : 7
print(a * b)    # Multiplication : 30
print(a / b)    # Division : 3.3333...
print(a // b)   # Division entière : 3
print(a % b)    # Modulo : 1
print(a ** b)   # Puissance : 1000

# Opérateurs d'assignation composés
x = 5
x += 3   # x = x + 3  → 8
x -= 2   # x = x - 2  → 6
x *= 4   # x = x * 4  → 24
x /= 3   # x = x / 3  → 8.0
x //= 2  # x = x // 2 → 4.0
x **= 2  # x = x ** 2 → 16.0
```

### Opérateurs de comparaison
```python
a, b = 5, 10

print(a == b)   # Égal : False
print(a != b)   # Différent : True
print(a < b)    # Inférieur : True
print(a > b)    # Supérieur : False
print(a <= b)   # Inférieur ou égal : True
print(a >= b)   # Supérieur ou égal : False

# Comparaisons chaînées
x = 5
print(1 < x < 10)  # True (équivalent à : 1 < x and x < 10)
```

### Opérateurs logiques
```python
# and, or, not
print(True and False)   # False
print(True or False)    # True
print(not True)         # False

# Évaluation court-circuit
def func1():
    print("func1 called")
    return True

def func2():
    print("func2 called")
    return False

# func2 n'est pas appelée car func1() est déjà False
result = func2() and func1()  

# Usage pratique
name = input("Nom: ")
if name and len(name) > 0:  # Vérifie que name n'est pas vide
    print(f"Bonjour {name}")
```

### Opérateur walrus (:=) - Python 3.8+
```python
# Permet d'assigner et utiliser une variable dans la même expression

# Sans walrus
data = input("Entrez quelque chose: ")
if len(data) > 5:
    print(f"Longueur: {len(data)}")

# Avec walrus
if (n := len(input("Entrez quelque chose: "))) > 5:
    print(f"Longueur: {n}")

# Autre exemple : éviter de calculer deux fois
numbers = [1, 2, 3, 4, 5]
# Sans walrus
if len(numbers) > 3:
    print(f"Liste de {len(numbers)} éléments")

# Avec walrus
if (length := len(numbers)) > 3:
    print(f"Liste de {length} éléments")

# Dans les compréhensions
data = [1, 2, 3, 4, 5]
results = [(x, x**2) for x in data if (square := x**2) > 10]
```

### Opérateurs d'appartenance
```python
# in, not in
fruits = ['pomme', 'banane', 'orange']
print('pomme' in fruits)      # True
print('kiwi' not in fruits)   # True

# Avec strings
text = "Python Programming"
print('Python' in text)       # True
print('Java' not in text)     # True

# Avec dictionnaires (teste les clés)
person = {'name': 'Alice', 'age': 30}
print('name' in person)       # True
print('Alice' in person)      # False (c'est une valeur, pas une clé)
print('Alice' in person.values())  # True
```

### Opérateurs d'identité
```python
# is, is not (teste si deux variables pointent vers le même objet)

a = [1, 2, 3]
b = [1, 2, 3]
c = a

print(a == b)    # True (même contenu)
print(a is b)    # False (objets différents)
print(a is c)    # True (même objet)

# Cas particulier : petits entiers et strings sont "internés"
x = 256
y = 256
print(x is y)    # True (Python optimise)

x = 257
y = 257
print(x is y)    # False (au-delà de la plage d'optimisation)

# Usage recommandé : tester None
value = None
if value is None:      # RECOMMANDÉ
    print("Pas de valeur")

if value == None:      # DÉCONSEILLÉ
    print("Pas de valeur")
```

---

## 1.3 Structures de contrôle

### if/elif/else
```python
# Syntaxe de base
age = 18

if age < 18:
    print("Mineur")
elif age == 18:
    print("Tout juste majeur")
else:
    print("Majeur")

# Expression ternaire (opérateur conditionnel)
status = "majeur" if age >= 18 else "mineur"

# Conditions multiples
score = 85
if score >= 90:
    grade = 'A'
elif score >= 80:
    grade = 'B'
elif score >= 70:
    grade = 'C'
elif score >= 60:
    grade = 'D'
else:
    grade = 'F'

# match/case (Python 3.10+) - Pattern matching
def http_status(status):
    match status:
        case 200:
            return "OK"
        case 404:
            return "Not Found"
        case 500:
            return "Internal Server Error"
        case _:  # Default case
            return "Unknown"

# Pattern matching avancé
def process_command(command):
    match command.split():
        case ["quit"]:
            return "Exiting..."
        case ["load", filename]:
            return f"Loading {filename}"
        case ["save", filename]:
            return f"Saving {filename}"
        case _:
            return "Unknown command"
```

### Boucles for
```python
# Itération sur une séquence
fruits = ['pomme', 'banane', 'orange']
for fruit in fruits:
    print(fruit)

# range()
for i in range(5):           # 0, 1, 2, 3, 4
    print(i)

for i in range(2, 10):       # 2, 3, 4, 5, 6, 7, 8, 9
    print(i)

for i in range(0, 10, 2):    # 0, 2, 4, 6, 8 (pas de 2)
    print(i)

# enumerate() - obtenir index et valeur
fruits = ['pomme', 'banane', 'orange']
for index, fruit in enumerate(fruits):
    print(f"{index}: {fruit}")

for index, fruit in enumerate(fruits, start=1):
    print(f"{index}: {fruit}")  # Commence à 1

# zip() - itérer sur plusieurs séquences
names = ['Alice', 'Bob', 'Charlie']
ages = [25, 30, 35]
for name, age in zip(names, ages):
    print(f"{name} a {age} ans")

# Itération sur un dictionnaire
person = {'name': 'Alice', 'age': 30, 'city': 'Paris'}

# Clés uniquement
for key in person:
    print(key)

# Clés et valeurs
for key, value in person.items():
    print(f"{key}: {value}")

# Valeurs uniquement
for value in person.values():
    print(value)
```

### Boucles while
```python
# Syntaxe de base
count = 0
while count < 5:
    print(count)
    count += 1

# Boucle infinie avec condition de sortie
while True:
    user_input = input("Entrez 'quit' pour sortir: ")
    if user_input == 'quit':
        break
    print(f"Vous avez entré: {user_input}")

# Avec else (s'exécute si la boucle se termine normalement)
n = 5
while n > 0:
    print(n)
    n -= 1
else:
    print("Décollage!")  # S'affiche après la boucle
```

### break, continue, pass
```python
# break - sort de la boucle
for i in range(10):
    if i == 5:
        break  # Sort quand i vaut 5
    print(i)  # Affiche 0, 1, 2, 3, 4

# continue - passe à l'itération suivante
for i in range(10):
    if i % 2 == 0:
        continue  # Saute les nombres pairs
    print(i)  # Affiche 1, 3, 5, 7, 9

# pass - ne fait rien (placeholder)
for i in range(5):
    if i == 3:
        pass  # À implémenter plus tard
    else:
        print(i)

# Usage de pass pour définir une fonction vide
def future_function():
    pass  # TODO: implémenter cette fonction

# else avec for (s'exécute si pas de break)
for i in range(5):
    if i == 10:
        break
else:
    print("Boucle terminée normalement")  # S'affiche

# Recherche d'un élément
numbers = [1, 2, 3, 4, 5]
search = 10

for num in numbers:
    if num == search:
        print("Trouvé!")
        break
else:
    print("Pas trouvé")  # S'affiche car search n'est pas dans la liste
```

---

## 1.4 Fonctions

### Définition et appel
```python
# Fonction simple
def greet():
    print("Bonjour!")

greet()  # Appel

# Fonction avec paramètres
def greet_person(name):
    print(f"Bonjour {name}!")

greet_person("Alice")

# Fonction avec valeur de retour
def add(a, b):
    return a + b

result = add(3, 5)  # 8

# Retours multiples (utilise un tuple)
def get_stats(numbers):
    return min(numbers), max(numbers), sum(numbers) / len(numbers)

minimum, maximum, average = get_stats([1, 2, 3, 4, 5])

# Fonction sans return explicite retourne None
def print_message(msg):
    print(msg)
    # return None (implicite)

result = print_message("Hello")  # result vaut None
```

### Arguments avec valeurs par défaut
```python
def greet(name, greeting="Bonjour"):
    print(f"{greeting} {name}!")

greet("Alice")                    # Bonjour Alice!
greet("Bob", "Salut")             # Salut Bob!

# ATTENTION : piège des arguments mutables par défaut
def add_item(item, list=[]):  # MAUVAIS !
    list.append(item)
    return list

print(add_item(1))  # [1]
print(add_item(2))  # [1, 2] - Surprise !

# SOLUTION : utiliser None comme valeur par défaut
def add_item(item, list=None):  # BON !
    if list is None:
        list = []
    list.append(item)
    return list

print(add_item(1))  # [1]
print(add_item(2))  # [2] - Correct !
```

### Arguments positionnels et nommés
```python
def create_user(name, age, city):
    return f"{name}, {age} ans, {city}"

# Arguments positionnels
result = create_user("Alice", 30, "Paris")

# Arguments nommés (keywords)
result = create_user(name="Alice", age=30, city="Paris")

# Mélange (positionnels d'abord)
result = create_user("Alice", age=30, city="Paris")

# Forcer les arguments nommés avec * (Python 3+)
def create_user(name, *, age, city):  # age et city DOIVENT être nommés
    return f"{name}, {age} ans, {city}"

create_user("Alice", age=30, city="Paris")  # OK
# create_user("Alice", 30, "Paris")  # TypeError !
```

### *args et **kwargs
```python
# *args - nombre variable d'arguments positionnels (tuple)
def sum_all(*args):
    total = 0
    for num in args:
        total += num
    return total

print(sum_all(1, 2, 3))        # 6
print(sum_all(1, 2, 3, 4, 5))  # 15

# **kwargs - nombre variable d'arguments nommés (dict)
def print_info(**kwargs):
    for key, value in kwargs.items():
        print(f"{key}: {value}")

print_info(name="Alice", age=30, city="Paris")
# name: Alice
# age: 30
# city: Paris

# Combinaison complète
def full_function(pos1, pos2, *args, kw1="default", **kwargs):
    print(f"pos1: {pos1}")
    print(f"pos2: {pos2}")
    print(f"args: {args}")
    print(f"kw1: {kw1}")
    print(f"kwargs: {kwargs}")

full_function(1, 2, 3, 4, 5, kw1="custom", extra1="a", extra2="b")

# Unpacking avec * et **
def add(a, b, c):
    return a + b + c

numbers = [1, 2, 3]
print(add(*numbers))  # Équivalent à add(1, 2, 3)

params = {'a': 1, 'b': 2, 'c': 3}
print(add(**params))  # Équivalent à add(a=1, b=2, c=3)
```

### Fonctions lambda
```python
# Syntaxe : lambda arguments: expression

# Fonction lambda simple
square = lambda x: x ** 2
print(square(5))  # 25

# Équivalent avec def
def square(x):
    return x ** 2

# Lambda avec plusieurs arguments
add = lambda a, b: a + b
print(add(3, 5))  # 8

# Usage courant : avec map, filter, sorted
numbers = [1, 2, 3, 4, 5]

# map() - applique une fonction à chaque élément
squared = list(map(lambda x: x**2, numbers))
print(squared)  # [1, 4, 9, 16, 25]

# filter() - filtre les éléments
evens = list(filter(lambda x: x % 2 == 0, numbers))
print(evens)  # [2, 4]

# sorted() - tri avec clé personnalisée
words = ['python', 'is', 'awesome', 'and', 'powerful']
sorted_words = sorted(words, key=lambda x: len(x))
print(sorted_words)  # ['is', 'and', 'python', 'awesome', 'powerful']

# Limitation : une seule expression, pas de statements
# OK : lambda x: x + 1
# ERREUR : lambda x: print(x); return x + 1
```

### Décorateurs de base
```python
# Un décorateur modifie le comportement d'une fonction

# Décorateur simple
def my_decorator(func):
    def wrapper():
        print("Avant l'appel")
        func()
        print("Après l'appel")
    return wrapper

@my_decorator
def say_hello():
    print("Hello!")

say_hello()
# Affiche:
# Avant l'appel
# Hello!
# Après l'appel

# Équivalent sans @
def say_hello():
    print("Hello!")
say_hello = my_decorator(say_hello)

# Décorateur avec arguments
def repeat(times):
    def decorator(func):
        def wrapper(*args, **kwargs):
            for _ in range(times):
                result = func(*args, **kwargs)
            return result
        return wrapper
    return decorator

@repeat(3)
def greet(name):
    print(f"Bonjour {name}!")

greet("Alice")
# Affiche:
# Bonjour Alice!
# Bonjour Alice!
# Bonjour Alice!

# Décorateur pour mesurer le temps d'exécution
import time

def timer(func):
    def wrapper(*args, **kwargs):
        start = time.time()
        result = func(*args, **kwargs)
        end = time.time()
        print(f"{func.__name__} a pris {end - start:.4f} secondes")
        return result
    return wrapper

@timer
def slow_function():
    time.sleep(1)
    return "Terminé"

result = slow_function()
# slow_function a pris 1.0001 secondes
```

---

## 🎯 Points clés à retenir

### Types de données
- **Immutables** : int, float, str, tuple, frozenset
- **Mutables** : list, dict, set
- Attention aux pièges des mutables (arguments par défaut, copies)

### Collections
- **list** : ordonnée, mutable, doublons autorisés
- **tuple** : ordonnée, immutable, doublons autorisés
- **dict** : paires clé-valeur, clés uniques
- **set** : non ordonné, valeurs uniques

### Compréhensions
- Plus pythoniques et souvent plus rapides que les boucles
- Syntaxe : `[expression for item in iterable if condition]`
- Disponibles pour list, dict, set

### Fonctions
- `*args` pour arguments positionnels variables
- `**kwargs` pour arguments nommés variables
- Lambda pour fonctions simples et anonymes
- Décorateurs pour modifier le comportement

---

## 💡 Questions d'entretien courantes (Chapitre 1)

1. **Quelle est la différence entre une liste et un tuple ?**
   - Liste : mutable, tuple : immutable
   - Liste : [], tuple : ()
   - Performance : tuples légèrement plus rapides

2. **Qu'est-ce qu'un argument mutable par défaut et pourquoi est-ce un problème ?**
   - Les objets mutables (list, dict) gardent leur état entre les appels
   - Solution : utiliser None comme défaut

3. **Différence entre `is` et `==` ?**
   - `==` compare les valeurs
   - `is` compare l'identité (même objet en mémoire)

4. **Comment copier une liste ?**
   ```python
   # Shallow copy
   new_list = old_list.copy()
   new_list = old_list[:]
   new_list = list(old_list)
   
   # Deep copy (pour listes imbriquées)
   import copy
   new_list = copy.deepcopy(old_list)
   ```

5. **Qu'est-ce qu'une compréhension de liste ?**
   - Syntaxe concise pour créer des listes
   - Plus rapide et pythonique que les boucles for

---

## 🔧 Exercices pratiques

1. Créez une fonction qui inverse un dictionnaire (clés ↔ valeurs)
2. Écrivez une compréhension de liste pour obtenir tous les nombres pairs de 1 à 100
3. Créez un décorateur qui compte le nombre d'appels d'une fonction
4. Écrivez une fonction qui fusionne deux dictionnaires sans écraser les valeurs existantes
5. Utilisez `*args` et `**kwargs` pour créer une fonction flexible de logging

---

**Prochaine étape :** Chapitre 2 - Programmation Orientée Objet
