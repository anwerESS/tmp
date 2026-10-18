## [**..**](./00_index.md)

# Chapitre 7 : Fonctionnalités Avancées

## 🎯 Objectifs du chapitre
- Maîtriser les itérateurs et générateurs
- Créer et utiliser des décorateurs avancés
- Comprendre les context managers
- Appliquer les concepts de programmation fonctionnelle
- Optimiser le code avec des techniques avancées

---

## 7.1 Itérateurs et générateurs

### Protocole d'itération

```python
# Un objet itérable implémente __iter__()
# Un itérateur implémente __iter__() et __next__()

class CountDown:
    """Itérateur qui compte à rebours."""
    
    def __init__(self, start):
        self.current = start
    
    def __iter__(self):
        """Retourne l'itérateur (self)."""
        return self
    
    def __next__(self):
        """Retourne le prochain élément."""
        if self.current <= 0:
            raise StopIteration
        
        self.current -= 1
        return self.current + 1

# Utilisation
for num in CountDown(5):
    print(num)  # 5, 4, 3, 2, 1

# Équivalent manuel
countdown = CountDown(3)
iterator = iter(countdown)  # Appelle __iter__()
print(next(iterator))  # Appelle __next__() : 3
print(next(iterator))  # 2
print(next(iterator))  # 1
# next(iterator)  # StopIteration

# Itérable vs Itérateur
class Range:
    """Itérable (peut être itéré plusieurs fois)."""
    
    def __init__(self, start, end):
        self.start = start
        self.end = end
    
    def __iter__(self):
        """Retourne un nouvel itérateur à chaque fois."""
        return RangeIterator(self.start, self.end)

class RangeIterator:
    """Itérateur (usage unique)."""
    
    def __init__(self, start, end):
        self.current = start
        self.end = end
    
    def __iter__(self):
        return self
    
    def __next__(self):
        if self.current >= self.end:
            raise StopIteration
        value = self.current
        self.current += 1
        return value

# Peut être itéré plusieurs fois
r = Range(1, 4)
print(list(r))  # [1, 2, 3]
print(list(r))  # [1, 2, 3] - fonctionne encore
```

### Générateurs avec yield

```python
# Générateur simple
def count_up_to(n):
    """Compte de 1 à n."""
    i = 1
    while i <= n:
        yield i
        i += 1

# Utilisation
for num in count_up_to(5):
    print(num)  # 1, 2, 3, 4, 5

# Le générateur est paresseux (lazy)
gen = count_up_to(1000000)  # Pas de calcul immédiat
print(next(gen))  # 1 - calcule uniquement le premier
print(next(gen))  # 2 - puis le deuxième

# Générateur vs liste
def numbers_list(n):
    """Retourne une liste (tout en mémoire)."""
    result = []
    for i in range(n):
        result.append(i)
    return result

def numbers_generator(n):
    """Retourne un générateur (un élément à la fois)."""
    for i in range(n):
        yield i

# Comparaison
import sys
lst = numbers_list(10000)
gen = numbers_generator(10000)
print(sys.getsizeof(lst))  # ~90 KB
print(sys.getsizeof(gen))  # ~200 bytes

# Générateur infini
def fibonacci():
    """Génère la suite de Fibonacci indéfiniment."""
    a, b = 0, 1
    while True:
        yield a
        a, b = b, a + b

# Utilisation avec itertools.islice
from itertools import islice
first_10 = list(islice(fibonacci(), 10))
print(first_10)  # [0, 1, 1, 2, 3, 5, 8, 13, 21, 34]

# Générateur avec gestion d'état
def running_average():
    """Calcule la moyenne glissante."""
    total = 0
    count = 0
    
    while True:
        value = yield total / count if count > 0 else 0
        total += value
        count += 1

avg = running_average()
next(avg)  # Initialise le générateur
print(avg.send(10))  # 10.0
print(avg.send(20))  # 15.0
print(avg.send(30))  # 20.0
```

### Expressions génératrices

```python
# Liste comprehension
squares_list = [x**2 for x in range(10)]  # Crée une liste complète

# Expression génératrice (plus efficace en mémoire)
squares_gen = (x**2 for x in range(10))   # Générateur

print(type(squares_list))  # <class 'list'>
print(type(squares_gen))   # <class 'generator'>

# Utilisation
for square in squares_gen:
    print(square)

# Utile pour de grandes données
import sys
big_list = [x for x in range(1000000)]
big_gen = (x for x in range(1000000))
print(sys.getsizeof(big_list))  # ~8 MB
print(sys.getsizeof(big_gen))   # ~200 bytes

# Chaînage de générateurs
def read_large_file(filename):
    """Lit un fichier ligne par ligne."""
    with open(filename) as f:
        for line in f:
            yield line.strip()

def filter_comments(lines):
    """Filtre les commentaires."""
    for line in lines:
        if not line.startswith('#'):
            yield line

def uppercase_lines(lines):
    """Convertit en majuscules."""
    for line in lines:
        yield line.upper()

# Pipeline de traitement
lines = read_large_file('data.txt')
filtered = filter_comments(lines)
processed = uppercase_lines(filtered)

for line in processed:
    print(line)  # Traitement paresseux, ligne par ligne
```

### yield from (Python 3.3+)

```python
# Délégation à un sous-générateur
def generator1():
    yield 1
    yield 2

def generator2():
    yield 3
    yield 4

def combined_old():
    """Ancienne méthode."""
    for value in generator1():
        yield value
    for value in generator2():
        yield value

def combined_new():
    """Avec yield from."""
    yield from generator1()
    yield from generator2()

print(list(combined_new()))  # [1, 2, 3, 4]

# Aplatir une structure imbriquée
def flatten(nested):
    """Aplatit une liste imbriquée."""
    for item in nested:
        if isinstance(item, list):
            yield from flatten(item)
        else:
            yield item

nested_list = [1, [2, 3, [4, 5]], 6, [7, [8, 9]]]
print(list(flatten(nested_list)))  # [1, 2, 3, 4, 5, 6, 7, 8, 9]

# Parcourir un arbre
class TreeNode:
    def __init__(self, value, children=None):
        self.value = value
        self.children = children or []
    
    def traverse(self):
        """Parcours en profondeur."""
        yield self.value
        for child in self.children:
            yield from child.traverse()

# Construction de l'arbre
root = TreeNode(1, [
    TreeNode(2, [TreeNode(4), TreeNode(5)]),
    TreeNode(3, [TreeNode(6)])
])

print(list(root.traverse()))  # [1, 2, 4, 5, 3, 6]
```

### Exemples pratiques

```python
# 1. Lecture de gros fichiers
def read_csv_chunks(filename, chunk_size=1000):
    """Lit un CSV par chunks."""
    chunk = []
    with open(filename) as f:
        for line in f:
            chunk.append(line.strip())
            if len(chunk) == chunk_size:
                yield chunk
                chunk = []
        if chunk:
            yield chunk

# 2. Pipeline de traitement de données
def process_data_pipeline(filename):
    """Pipeline complet de traitement."""
    
    def read_lines(filename):
        with open(filename) as f:
            for line in f:
                yield line.strip()
    
    def parse_csv(lines):
        for line in lines:
            yield line.split(',')
    
    def filter_valid(rows):
        for row in rows:
            if len(row) >= 3 and row[0]:
                yield row
    
    def extract_fields(rows):
        for row in rows:
            yield {'name': row[0], 'age': int(row[1]), 'city': row[2]}
    
    # Chaînage
    lines = read_lines(filename)
    rows = parse_csv(lines)
    valid = filter_valid(rows)
    records = extract_fields(valid)
    
    return records

# 3. Générateur de nombres premiers
def primes():
    """Génère les nombres premiers."""
    yield 2
    primes_list = [2]
    candidate = 3
    
    while True:
        is_prime = True
        for p in primes_list:
            if p * p > candidate:
                break
            if candidate % p == 0:
                is_prime = False
                break
        
        if is_prime:
            primes_list.append(candidate)
            yield candidate
        
        candidate += 2

# Premiers 20 nombres premiers
print(list(islice(primes(), 20)))

# 4. Fenêtre glissante (sliding window)
def sliding_window(iterable, size):
    """Crée une fenêtre glissante."""
    from collections import deque
    it = iter(iterable)
    window = deque(islice(it, size), maxlen=size)
    
    if len(window) == size:
        yield tuple(window)
    
    for item in it:
        window.append(item)
        yield tuple(window)

data = [1, 2, 3, 4, 5, 6]
for window in sliding_window(data, 3):
    print(window)
# (1, 2, 3)
# (2, 3, 4)
# (3, 4, 5)
# (4, 5, 6)
```

---

## 7.2 Décorateurs avancés

### Rappel : décorateurs de base

```python
def my_decorator(func):
    """Décorateur simple."""
    def wrapper(*args, **kwargs):
        print("Avant")
        result = func(*args, **kwargs)
        print("Après")
        return result
    return wrapper

@my_decorator
def say_hello():
    print("Hello!")

say_hello()
# Avant
# Hello!
# Après
```

### Préserver les métadonnées avec functools.wraps

```python
from functools import wraps

def my_decorator(func):
    @wraps(func)  # IMPORTANT : préserve __name__, __doc__, etc.
    def wrapper(*args, **kwargs):
        """Wrapper documentation."""
        return func(*args, **kwargs)
    return wrapper

@my_decorator
def greet(name):
    """Salue une personne."""
    return f"Hello {name}"

print(greet.__name__)  # 'greet' (sans @wraps serait 'wrapper')
print(greet.__doc__)   # 'Salue une personne.'
```

### Décorateurs avec arguments

```python
from functools import wraps

def repeat(times):
    """Décorateur qui répète l'exécution."""
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            results = []
            for _ in range(times):
                result = func(*args, **kwargs)
                results.append(result)
            return results
        return wrapper
    return decorator

@repeat(3)
def greet(name):
    return f"Hello {name}"

print(greet("Alice"))  # ['Hello Alice', 'Hello Alice', 'Hello Alice']

# Équivalent sans @
def greet(name):
    return f"Hello {name}"
greet = repeat(3)(greet)
```

### Décorateurs paramétrables optionnels

```python
from functools import wraps

def optional_decorator(func=None, *, option="default"):
    """Décorateur utilisable avec ou sans arguments."""
    
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            print(f"Option: {option}")
            return func(*args, **kwargs)
        return wrapper
    
    if func is None:
        # Appelé avec arguments: @optional_decorator(option="value")
        return decorator
    else:
        # Appelé sans arguments: @optional_decorator
        return decorator(func)

# Sans arguments
@optional_decorator
def func1():
    print("Func1")

# Avec arguments
@optional_decorator(option="custom")
def func2():
    print("Func2")

func1()  # Option: default, Func1
func2()  # Option: custom, Func2
```

### Décorateurs de classe

```python
from functools import wraps

# 1. Décorateur qui modifie une classe
def add_repr(cls):
    """Ajoute une méthode __repr__ à une classe."""
    def __repr__(self):
        attrs = ', '.join(f"{k}={v!r}" for k, v in self.__dict__.items())
        return f"{cls.__name__}({attrs})"
    
    cls.__repr__ = __repr__
    return cls

@add_repr
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age

person = Person("Alice", 30)
print(person)  # Person(name='Alice', age=30)

# 2. Singleton avec décorateur
def singleton(cls):
    """Garantit qu'une classe n'a qu'une seule instance."""
    instances = {}
    
    @wraps(cls)
    def get_instance(*args, **kwargs):
        if cls not in instances:
            instances[cls] = cls(*args, **kwargs)
        return instances[cls]
    
    return get_instance

@singleton
class Database:
    def __init__(self):
        self.connection = "Connected"

db1 = Database()
db2 = Database()
print(db1 is db2)  # True (même instance)
```

### Décorateurs utilitaires

```python
from functools import wraps
import time
import logging

# 1. Timer - Mesure le temps d'exécution
def timer(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        start = time.perf_counter()
        result = func(*args, **kwargs)
        end = time.perf_counter()
        print(f"{func.__name__} a pris {end - start:.4f}s")
        return result
    return wrapper

@timer
def slow_function():
    time.sleep(1)
    return "Done"

slow_function()  # slow_function a pris 1.0001s

# 2. Retry - Réessaye en cas d'échec
def retry(max_attempts=3, delay=1):
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            for attempt in range(1, max_attempts + 1):
                try:
                    return func(*args, **kwargs)
                except Exception as e:
                    if attempt == max_attempts:
                        raise
                    print(f"Tentative {attempt} échouée, réessai dans {delay}s...")
                    time.sleep(delay)
        return wrapper
    return decorator

@retry(max_attempts=3, delay=0.5)
def unstable_function():
    import random
    if random.random() < 0.7:
        raise ValueError("Échec aléatoire")
    return "Succès"

# 3. Cache/Memoization
def memoize(func):
    cache = {}
    
    @wraps(func)
    def wrapper(*args):
        if args not in cache:
            cache[args] = func(*args)
        return cache[args]
    return wrapper

@memoize
def fibonacci(n):
    if n < 2:
        return n
    return fibonacci(n-1) + fibonacci(n-2)

print(fibonacci(100))  # Très rapide grâce au cache

# Alternative: functools.lru_cache
from functools import lru_cache

@lru_cache(maxsize=128)
def fibonacci_cached(n):
    if n < 2:
        return n
    return fibonacci_cached(n-1) + fibonacci_cached(n-2)

# 4. Logger - Log les appels de fonction
def log_calls(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        args_str = ', '.join(repr(arg) for arg in args)
        kwargs_str = ', '.join(f"{k}={v!r}" for k, v in kwargs.items())
        all_args = ', '.join(filter(None, [args_str, kwargs_str]))
        
        logging.info(f"Appel de {func.__name__}({all_args})")
        
        try:
            result = func(*args, **kwargs)
            logging.info(f"{func.__name__} retourne {result!r}")
            return result
        except Exception as e:
            logging.error(f"{func.__name__} a levé {e!r}")
            raise
    
    return wrapper

# 5. Validate - Validation des arguments
def validate_types(**type_checks):
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            # Obtenir les noms des paramètres
            import inspect
            sig = inspect.signature(func)
            bound = sig.bind(*args, **kwargs)
            bound.apply_defaults()
            
            for param_name, expected_type in type_checks.items():
                if param_name in bound.arguments:
                    value = bound.arguments[param_name]
                    if not isinstance(value, expected_type):
                        raise TypeError(
                            f"{param_name} doit être {expected_type.__name__}, "
                            f"pas {type(value).__name__}"
                        )
            
            return func(*args, **kwargs)
        return wrapper
    return decorator

@validate_types(name=str, age=int)
def create_user(name, age):
    return f"User: {name}, {age}"

create_user("Alice", 30)  # OK
# create_user("Alice", "30")  # TypeError
```

### Décorateurs empilés

```python
@timer
@retry(max_attempts=3)
@log_calls
def critical_function():
    return "Result"

# Équivalent à:
# critical_function = timer(retry(max_attempts=3)(log_calls(critical_function)))

# L'ordre compte!
# 1. log_calls enregistre l'appel
# 2. retry réessaye si échec
# 3. timer mesure le temps total
```

---

## 7.3 Context managers

### Protocole __enter__ et __exit__

```python
class FileManager:
    """Context manager pour gérer un fichier."""
    
    def __init__(self, filename, mode):
        self.filename = filename
        self.mode = mode
        self.file = None
    
    def __enter__(self):
        """Appelé au début du bloc with."""
        print(f"Ouverture de {self.filename}")
        self.file = open(self.filename, self.mode)
        return self.file  # Valeur assignée à 'as variable'
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Appelé à la fin du bloc with."""
        print(f"Fermeture de {self.filename}")
        if self.file:
            self.file.close()
        
        # exc_type: type de l'exception (ou None)
        # exc_val: instance de l'exception (ou None)
        # exc_tb: traceback (ou None)
        
        # Retourner True supprime l'exception
        # Retourner False (ou None) propage l'exception
        return False

# Utilisation
with FileManager('test.txt', 'w') as f:
    f.write('Hello World!')
# Le fichier est automatiquement fermé

# Avec gestion d'exception
try:
    with FileManager('test.txt', 'r') as f:
        content = f.read()
        raise ValueError("Erreur!")
except ValueError:
    print("Exception capturée")
# Le fichier est quand même fermé
```

### contextlib - Créer des context managers facilement

```python
from contextlib import contextmanager

@contextmanager
def file_manager(filename, mode):
    """Context manager avec décorateur."""
    print(f"Ouverture de {filename}")
    f = open(filename, mode)
    
    try:
        yield f  # __enter__ retourne f
    finally:
        print(f"Fermeture de {filename}")
        f.close()  # __exit__ ferme le fichier

# Utilisation identique
with file_manager('test.txt', 'w') as f:
    f.write('Hello!')

# Context manager pour mesurer le temps
@contextmanager
def timer_context(name):
    """Mesure le temps d'exécution d'un bloc."""
    start = time.perf_counter()
    print(f"Début de {name}")
    
    try:
        yield
    finally:
        end = time.perf_counter()
        print(f"{name} a pris {end - start:.4f}s")

with timer_context("opération lourde"):
    time.sleep(1)
    # Code à mesurer

# Context manager pour changer temporairement de répertoire
import os

@contextmanager
def change_dir(path):
    """Change temporairement de répertoire."""
    old_dir = os.getcwd()
    os.chdir(path)
    
    try:
        yield
    finally:
        os.chdir(old_dir)

with change_dir('/tmp'):
    print(os.getcwd())  # /tmp
    # Opérations dans /tmp
print(os.getcwd())  # Répertoire d'origine
```

### Context managers multiples

```python
# Plusieurs context managers
with open('input.txt', 'r') as infile, \
     open('output.txt', 'w') as outfile:
    content = infile.read()
    outfile.write(content.upper())

# Ou avec contextlib.ExitStack (Python 3.3+)
from contextlib import ExitStack

files = ['file1.txt', 'file2.txt', 'file3.txt']

with ExitStack() as stack:
    file_objects = [stack.enter_context(open(f)) for f in files]
    
    for f in file_objects:
        print(f.read())
# Tous les fichiers sont fermés automatiquement
```

### Context managers utiles

```python
from contextlib import contextmanager, suppress, redirect_stdout
import sys

# 1. suppress - Supprime les exceptions spécifiques
from contextlib import suppress

with suppress(FileNotFoundError):
    os.remove('fichier_inexistant.txt')
# Pas d'erreur si le fichier n'existe pas

# 2. redirect_stdout - Redirige stdout
import io

f = io.StringIO()
with redirect_stdout(f):
    print("Ce texte va dans f")
    print("Pas dans la console")

output = f.getvalue()
print(output)  # Affiche le contenu capturé

# 3. Context manager pour base de données
@contextmanager
def database_transaction(connection):
    """Gère une transaction de base de données."""
    cursor = connection.cursor()
    
    try:
        yield cursor
        connection.commit()  # Commit si pas d'erreur
    except Exception:
        connection.rollback()  # Rollback si erreur
        raise
    finally:
        cursor.close()

# Usage (pseudo-code)
# with database_transaction(conn) as cursor:
#     cursor.execute("INSERT INTO ...")
#     cursor.execute("UPDATE ...")
# Auto-commit ou rollback

# 4. Lock thread-safe
from threading import Lock

lock = Lock()

with lock:
    # Section critique
    # Garantie d'exclusion mutuelle
    pass
# Lock automatiquement relâché

# 5. Context manager réutilisable
class ResourceManager:
    """Gère une ressource de manière réutilisable."""
    
    def __init__(self, resource):
        self.resource = resource
        self.in_use = False
    
    def __enter__(self):
        if self.in_use:
            raise RuntimeError("Ressource déjà en cours d'utilisation")
        self.in_use = True
        return self.resource
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.in_use = False
        return False

resource = ResourceManager("ma_ressource")

with resource as r:
    print(f"Utilisation de {r}")

with resource as r:  # Réutilisable
    print(f"Nouvelle utilisation de {r}")
```

---

## 7.4 Programmation fonctionnelle

### Fonctions de première classe

```python
# Les fonctions sont des objets
def greet(name):
    return f"Hello {name}"

# Assigner à une variable
say_hello = greet
print(say_hello("Alice"))  # Hello Alice

# Passer comme argument
def execute_func(func, arg):
    return func(arg)

result = execute_func(greet, "Bob")  # Hello Bob

# Retourner depuis une fonction
def get_greeting_func(language):
    def english(name):
        return f"Hello {name}"
    
    def french(name):
        return f"Bonjour {name}"
    
    if language == "en":
        return english
    else:
        return french

greet_fn = get_greeting_func("fr")
print(greet_fn("Alice"))  # Bonjour Alice

# Stocker dans des structures de données
operations = {
    'add': lambda a, b: a + b,
    'subtract': lambda a, b: a - b,
    'multiply': lambda a, b: a * b,
}

print(operations['add'](5, 3))  # 8
```

### map, filter, reduce

```python
# map - Applique une fonction à chaque élément
numbers = [1, 2, 3, 4, 5]
squared = list(map(lambda x: x**2, numbers))
print(squared)  # [1, 4, 9, 16, 25]

# Équivalent en comprehension (plus pythonique)
squared = [x**2 for x in numbers]

# map avec plusieurs itérables
list1 = [1, 2, 3]
list2 = [10, 20, 30]
sums = list(map(lambda a, b: a + b, list1, list2))
print(sums)  # [11, 22, 33]

# filter - Filtre les éléments
numbers = [1, 2, 3, 4, 5, 6]
evens = list(filter(lambda x: x % 2 == 0, numbers))
print(evens)  # [2, 4, 6]

# Équivalent en comprehension
evens = [x for x in numbers if x % 2 == 0]

# reduce - Réduit à une valeur unique
from functools import reduce

numbers = [1, 2, 3, 4, 5]
sum_all = reduce(lambda a, b: a + b, numbers)
print(sum_all)  # 15

# Avec valeur initiale
product = reduce(lambda a, b: a * b, numbers, 1)
print(product)  # 120

# Exemples pratiques
# Aplatir une liste de listes
nested = [[1, 2], [3, 4], [5, 6]]
flattened = reduce(lambda a, b: a + b, nested)
print(flattened)  # [1, 2, 3, 4, 5, 6]

# Trouver le maximum
numbers = [3, 1, 4, 1, 5, 9, 2, 6]
maximum = reduce(lambda a, b: a if a > b else b, numbers)
print(maximum)  # 9
```

### Closures (Fermetures)

```python
# Une closure capture les variables de sa portée englobante
def make_multiplier(factor):
    """Retourne une fonction qui multiplie par factor."""
    def multiplier(x):
        return x * factor  # Capture 'factor'
    return multiplier

double = make_multiplier(2)
triple = make_multiplier(3)

print(double(5))  # 10
print(triple(5))  # 15

# La variable est capturée, pas copiée
def make_counter():
    """Retourne un compteur."""
    count = 0
    
    def increment():
        nonlocal count  # Modifie la variable de la portée englobante
        count += 1
        return count
    
    return increment

counter = make_counter()
print(counter())  # 1
print(counter())  # 2
print(counter())  # 3

# Factory de fonctions
def make_validator(min_val, max_val):
    """Crée une fonction de validation."""
    def validate(value):
        return min_val <= value <= max_val
    return validate

is_valid_age = make_validator(0, 150)
is_valid_percent = make_validator(0, 100)

print(is_valid_age(25))     # True
print(is_valid_percent(150)) # False
```

### Fonctions partielles

```python
from functools import partial

# Fixer certains arguments d'une fonction
def power(base, exponent):
    return base ** exponent

# Créer une fonction spécialisée
square = partial(power, exponent=2)
cube = partial(power, exponent=3)

print(square(5))  # 25
print(cube(5))    # 125

# Exemple pratique: logging
import logging

def log(level, message):
    logging.log(level, message)

# Créer des fonctions spécialisées
debug = partial(log, logging.DEBUG)
info = partial(log, logging.INFO)
error = partial(log, logging.ERROR)

info("Application démarrée")
error("Erreur critique")

# Avec fonctions built-in
from operator import add

add_10 = partial(add, 10)
print(add_10(5))  # 15

# Conversion de types
int_from_binary = partial(int, base=2)
int_from_hex = partial(int, base=16)

print(int_from_binary('1010'))  # 10
print(int_from_hex('FF'))        # 255
```

### Composition de fonctions

```python
from functools import reduce

def compose(*functions):
    """Compose plusieurs fonctions."""
    def inner(arg):
        return reduce(lambda x, f: f(x), reversed(functions), arg)
    return inner

# Fonctions simples
def add_10(x):
    return x + 10

def multiply_2(x):
    return x * 2

def square(x):
    return x ** 2

# Composition
f = compose(add_10, multiply_2, square)
print(f(3))  # ((3 ** 2) * 2) + 10 = 28

# Pipeline de traitement
def clean_text(text):
    return text.strip().lower()

def remove_punctuation(text):
    import string
    return text.translate(str.maketrans('', '', string.punctuation))

def split_words(text):
    return text.split()

process_text = compose(split_words, remove_punctuation, clean_text)
result = process_text("  Hello, World! ")
print(result)  # ['hello', 'world']
```

### Programmation déclarative

```python
# Impératif vs Déclaratif

# Style impératif (comment faire)
def get_even_squares_imperative(numbers):
    result = []
    for num in numbers:
        if num % 2 == 0:
            square = num ** 2
            result.append(square)
    return result

# Style déclaratif (quoi faire)
def get_even_squares_declarative(numbers):
    return [x**2 for x in numbers if x % 2 == 0]

# Ou avec map/filter
def get_even_squares_functional(numbers):
    return list(map(lambda x: x**2, filter(lambda x: x % 2 == 0, numbers)))

numbers = [1, 2, 3, 4, 5, 6]
print(get_even_squares_declarative(numbers))  # [4, 16, 36]

# Pipeline fonctionnel complexe
from operator import itemgetter

# Données
users = [
    {'name': 'Alice', 'age': 30, 'city': 'Paris'},
    {'name': 'Bob', 'age': 25, 'city': 'Lyon'},
    {'name': 'Charlie', 'age': 35, 'city': 'Paris'},
]

# Obtenir les noms des utilisateurs de Paris, triés par âge
result = list(map(
    itemgetter('name'),
    sorted(
        filter(lambda u: u['city'] == 'Paris', users),
        key=itemgetter('age')
    )
))

print(result)  # ['Alice', 'Charlie']

# Plus pythonique avec comprehension
result = [u['name'] for u in sorted(users, key=itemgetter('age')) 
          if u['city'] == 'Paris']
```

---

## 🎯 Points clés à retenir

### Itérateurs et générateurs
- **Générateur** : fonction avec `yield`, paresseux, économe en mémoire
- **yield from** : déléguer à un sous-générateur
- **Expression génératrice** : `(x for x in ...)` au lieu de `[x for x in ...]`
- Utiliser pour grands ensembles de données

### Décorateurs
- **@wraps** : préserver les métadonnées
- Arguments : `def decorator(arg): def wrapper(func):...`
- Utilités : timer, retry, cache, validation, logging
- Ordre important quand empilés

### Context managers
- **__enter__/__exit__** : protocole complet
- **@contextmanager** : version simple avec yield
- Garantit libération des ressources
- Utile pour : fichiers, verrous, transactions, mesures

### Programmation fonctionnelle
- **Fonctions de première classe** : peuvent être passées, retournées
- **map/filter/reduce** : transformations fonctionnelles
- **Closures** : capture de variables de portée englobante
- **partial** : fixer des arguments
- Style **déclaratif** souvent plus lisible

---

## 💡 Questions d'entretien courantes (Chapitre 7)

1. **Différence entre itérateur et générateur ?**
   - Itérateur : implémente `__iter__` et `__next__`
   - Générateur : fonction avec `yield`, crée automatiquement un itérateur
   - Générateur plus simple à écrire

2. **Pourquoi utiliser yield au lieu de return avec une liste ?**
   - Économie de mémoire (paresseux)
   - Commence immédiatement (pas d'attente)
   - Peut générer infiniment
   - Bon pour pipelines de traitement

3. **Qu'est-ce qu'une closure ?**
   - Fonction qui capture des variables de sa portée englobante
   - Les variables capturées "survivent" après la fonction parente
   - Utile pour factory de fonctions

4. **À quoi sert functools.wraps ?**
   - Préserve `__name__`, `__doc__`, etc. de la fonction décorée
   - Sans ça, la fonction wrapper perd ses métadonnées
   - Toujours l'utiliser dans les décorateurs

5. **Quand utiliser un context manager ?**
   - Gestion de ressources (fichiers, connexions)
   - Setup/teardown garantis
   - Code plus propre et sûr
   - Alternative au try/finally

6. **map/filter vs comprehensions ?**
   - Comprehensions : plus pythoniques, plus lisibles
   - map/filter : style fonctionnel, parfois plus composable
   - Comprehensions préférées en Python

---

## 🔧 Exercices pratiques

1. **Générateur de mots de passe**
   - Génère des mots de passe aléatoires
   - Paramètres : longueur, caractères autorisés
   - Générateur infini

2. **Décorateur de cache avec TTL**
   - Cache avec expiration temporelle
   - Paramètre : durée de validité
   - Nettoie les entrées expirées

3. **Context manager de profiling**
   - Mesure CPU et mémoire
   - Affiche un rapport à la sortie
   - Utilise psutil si disponible

4. **Pipeline de traitement de données**
   - Générateurs chaînés
   - Lecture → Filtrage → Transformation → Agrégation
   - Fichier CSV en entrée

5. **Décorateur de rate limiting**
   - Limite le nombre d'appels par période
   - Paramètres : max_calls, period
   - Utilise time.sleep si nécessaire

---

**Prochaine étape :** Chapitre 8 - Bonnes Pratiques et Style, ou Chapitre 10 - Questions d'Entretien
