## [**..**](./00_index.md)

# Chapitre 9 : Performance et Optimisation

## 🎯 Objectifs du chapitre
- Mesurer et profiler les performances
- Comprendre la complexité algorithmique (Big O)
- Optimiser le code Python
- Utiliser les bonnes structures de données
- Appliquer des techniques de cache et memoization

---

## 9.1 Mesure de performance

### timeit - Mesurer le temps d'exécution

```python
import timeit

# Mesurer une expression simple
time = timeit.timeit('sum(range(100))', number=10000)
print(f"Temps: {time:.4f}s")

# Mesurer une fonction
def my_function():
    return sum(range(100))

time = timeit.timeit(my_function, number=10000)
print(f"Temps: {time:.4f}s")

# Avec setup (code d'initialisation)
code = '''
result = sum(numbers)
'''

setup = '''
numbers = range(1000)
'''

time = timeit.timeit(code, setup=setup, number=1000)
print(f"Temps: {time:.4f}s")

# Comparer plusieurs approches
def compare_methods():
    # Méthode 1 : boucle for
    time1 = timeit.timeit('''
result = 0
for i in range(1000):
    result += i
''', number=10000)
    
    # Méthode 2 : sum
    time2 = timeit.timeit('sum(range(1000))', number=10000)
    
    # Méthode 3 : formule mathématique
    time3 = timeit.timeit('n = 1000; (n * (n - 1)) // 2', number=10000)
    
    print(f"Boucle for: {time1:.4f}s")
    print(f"sum():      {time2:.4f}s")
    print(f"Formule:    {time3:.4f}s")

compare_methods()

# repeat() pour obtenir plusieurs mesures
times = timeit.repeat('sum(range(1000))', number=1000, repeat=5)
print(f"Meilleur temps: {min(times):.4f}s")
print(f"Moyenne: {sum(times)/len(times):.4f}s")

# Timer contextuel
from timeit import default_timer as timer

start = timer()
# Code à mesurer
result = sum(range(1000000))
end = timer()
print(f"Temps: {end - start:.4f}s")
```

### time - Mesure simple

```python
import time

# time.time() - timestamp Unix (secondes depuis 1970)
start = time.time()
# Code à mesurer
time.sleep(1)
end = time.time()
print(f"Durée: {end - start:.2f}s")

# time.perf_counter() - Plus précis pour mesurer des durées
start = time.perf_counter()
result = sum(range(1000000))
end = time.perf_counter()
print(f"Durée: {end - start:.6f}s")

# time.process_time() - Temps CPU (exclut le temps d'attente)
start = time.process_time()
result = sum(range(1000000))
end = time.process_time()
print(f"Temps CPU: {end - start:.6f}s")

# Décorateur pour mesurer le temps
def timer_decorator(func):
    def wrapper(*args, **kwargs):
        start = time.perf_counter()
        result = func(*args, **kwargs)
        end = time.perf_counter()
        print(f"{func.__name__} a pris {end - start:.4f}s")
        return result
    return wrapper

@timer_decorator
def slow_function():
    return sum(range(1000000))

slow_function()
```

### cProfile - Profiler détaillé

```python
import cProfile
import pstats
import io

def fibonacci(n):
    if n < 2:
        return n
    return fibonacci(n-1) + fibonacci(n-2)

def main():
    for i in range(20):
        fibonacci(i)

# Profiler simple
cProfile.run('main()')

# Profiler avec sortie dans un fichier
cProfile.run('main()', 'profile_stats')

# Analyser les stats
stats = pstats.Stats('profile_stats')
stats.strip_dirs()
stats.sort_stats('cumulative')
stats.print_stats(10)  # Top 10 fonctions

# Profiler dans du code
profiler = cProfile.Profile()
profiler.enable()
# Code à profiler
main()
profiler.disable()

# Afficher les résultats
s = io.StringIO()
ps = pstats.Stats(profiler, stream=s).sort_stats('cumulative')
ps.print_stats()
print(s.getvalue())

# Colonnes du profiler :
# ncalls  : nombre d'appels
# tottime : temps total dans la fonction (sans sous-fonctions)
# percall : tottime / ncalls
# cumtime : temps cumulé (avec sous-fonctions)
# percall : cumtime / ncalls
```

### line_profiler - Profiler ligne par ligne

```python
# Installation: pip install line_profiler

# Utilisation avec décorateur
from line_profiler import LineProfiler

def slow_function():
    total = 0
    for i in range(1000):
        total += i * 2
    
    result = []
    for i in range(1000):
        result.append(i ** 2)
    
    return total, result

# Profiler
lp = LineProfiler()
lp.add_function(slow_function)
lp.enable()
slow_function()
lp.disable()
lp.print_stats()

# Ou en ligne de commande avec décorateur @profile
# kernprof -l -v script.py
```

### memory_profiler - Analyser la mémoire

```python
# Installation: pip install memory_profiler

from memory_profiler import profile

@profile
def memory_intensive():
    # Crée une grande liste
    big_list = [0] * (10 ** 6)
    
    # Crée un dictionnaire
    big_dict = {i: i*2 for i in range(10 ** 5)}
    
    # Traitement
    result = sum(big_list)
    
    return result

memory_intensive()

# Ou en ligne de commande:
# python -m memory_profiler script.py

# Mesurer l'utilisation mémoire
import sys

data = [1, 2, 3, 4, 5]
print(f"Taille: {sys.getsizeof(data)} bytes")

# Comparer structures de données
import sys

list_data = list(range(10000))
tuple_data = tuple(range(10000))
set_data = set(range(10000))

print(f"Liste:  {sys.getsizeof(list_data)} bytes")
print(f"Tuple:  {sys.getsizeof(tuple_data)} bytes")
print(f"Set:    {sys.getsizeof(set_data)} bytes")
```

---

## 9.2 Complexité algorithmique (Big O)

### Notation Big O

```python
# O(1) - Constant
def get_first(items):
    """Temps constant, indépendant de la taille."""
    return items[0]

# O(log n) - Logarithmique
def binary_search(sorted_list, target):
    """Recherche dichotomique."""
    left, right = 0, len(sorted_list) - 1
    
    while left <= right:
        mid = (left + right) // 2
        if sorted_list[mid] == target:
            return mid
        elif sorted_list[mid] < target:
            left = mid + 1
        else:
            right = mid - 1
    
    return -1

# O(n) - Linéaire
def find_max(items):
    """Parcourt tous les éléments une fois."""
    max_value = items[0]
    for item in items:
        if item > max_value:
            max_value = item
    return max_value

# O(n log n) - Linéarithmique
def merge_sort(items):
    """Tri fusion (efficace)."""
    if len(items) <= 1:
        return items
    
    mid = len(items) // 2
    left = merge_sort(items[:mid])
    right = merge_sort(items[mid:])
    
    return merge(left, right)

# O(n²) - Quadratique
def bubble_sort(items):
    """Tri à bulles (inefficace)."""
    n = len(items)
    for i in range(n):
        for j in range(n - i - 1):
            if items[j] > items[j + 1]:
                items[j], items[j + 1] = items[j + 1], items[j]
    return items

# O(2ⁿ) - Exponentiel
def fibonacci_recursive(n):
    """Fibonacci récursif (très inefficace)."""
    if n < 2:
        return n
    return fibonacci_recursive(n-1) + fibonacci_recursive(n-2)

# O(n!) - Factoriel
def permutations(items):
    """Génère toutes les permutations."""
    if len(items) <= 1:
        return [items]
    
    result = []
    for i, item in enumerate(items):
        rest = items[:i] + items[i+1:]
        for perm in permutations(rest):
            result.append([item] + perm)
    
    return result
```

### Complexité des structures de données Python

```python
# Liste (list)
# Accès par index:     O(1)
# Recherche:           O(n)
# Insertion début:     O(n)
# Insertion fin:       O(1) amortisé
# Suppression début:   O(n)
# Suppression fin:     O(1)

# Tuple (tuple)
# Même complexité que list, mais immuable

# Dictionnaire (dict)
# Accès:               O(1) moyen
# Insertion:           O(1) moyen
# Suppression:         O(1) moyen
# Recherche clé:       O(1) moyen

# Set (set)
# Recherche:           O(1) moyen
# Insertion:           O(1) moyen
# Suppression:         O(1) moyen
# Union/Inter:         O(len(s) + len(t))

# Deque (collections.deque)
# Append gauche:       O(1)
# Append droite:       O(1)
# Pop gauche:          O(1)
# Pop droite:          O(1)
# Accès milieu:        O(n)

# Exemples pratiques
import time

# Insertion au début
# Liste: O(n)
def insert_beginning_list(n):
    lst = []
    start = time.perf_counter()
    for i in range(n):
        lst.insert(0, i)
    return time.perf_counter() - start

# Deque: O(1)
from collections import deque
def insert_beginning_deque(n):
    d = deque()
    start = time.perf_counter()
    for i in range(n):
        d.appendleft(i)
    return time.perf_counter() - start

print(f"List (10000):  {insert_beginning_list(10000):.4f}s")
print(f"Deque (10000): {insert_beginning_deque(10000):.4f}s")

# Recherche
# Liste: O(n)
# Set: O(1)
data_list = list(range(10000))
data_set = set(range(10000))

start = time.perf_counter()
9999 in data_list
time_list = time.perf_counter() - start

start = time.perf_counter()
9999 in data_set
time_set = time.perf_counter() - start

print(f"Recherche dans list: {time_list:.6f}s")
print(f"Recherche dans set:  {time_set:.6f}s")
```

---

## 9.3 Optimisations courantes

### Compréhensions vs boucles

```python
import timeit

# Boucle for
def with_loop():
    result = []
    for i in range(1000):
        result.append(i ** 2)
    return result

# Compréhension de liste
def with_comprehension():
    return [i ** 2 for i in range(1000)]

# map + lambda
def with_map():
    return list(map(lambda x: x ** 2, range(1000)))

time_loop = timeit.timeit(with_loop, number=10000)
time_comp = timeit.timeit(with_comprehension, number=10000)
time_map = timeit.timeit(with_map, number=10000)

print(f"Loop:          {time_loop:.4f}s")
print(f"Comprehension: {time_comp:.4f}s")  # Plus rapide
print(f"Map:           {time_map:.4f}s")
```

### Générateurs vs listes

```python
import sys

# Liste : tout en mémoire
def numbers_list(n):
    return [i for i in range(n)]

# Générateur : paresseux
def numbers_generator(n):
    return (i for i in range(n))

n = 1000000

lst = numbers_list(n)
gen = numbers_generator(n)

print(f"Liste:      {sys.getsizeof(lst):,} bytes")
print(f"Générateur: {sys.getsizeof(gen):,} bytes")

# Performance
import timeit

# Somme avec liste
time_list = timeit.timeit(
    'sum([i for i in range(10000)])',
    number=1000
)

# Somme avec générateur
time_gen = timeit.timeit(
    'sum(i for i in range(10000))',
    number=1000
)

print(f"Liste:      {time_list:.4f}s")
print(f"Générateur: {time_gen:.4f}s")  # Souvent plus rapide
```

### Variables locales vs globales

```python
import timeit

# Variable globale
GLOBAL_VAR = 42

def use_global():
    result = 0
    for _ in range(1000):
        result += GLOBAL_VAR
    return result

# Variable locale
def use_local():
    local_var = 42
    result = 0
    for _ in range(1000):
        result += local_var
    return result

time_global = timeit.timeit(use_global, number=10000)
time_local = timeit.timeit(use_local, number=10000)

print(f"Global: {time_global:.4f}s")
print(f"Local:  {time_local:.4f}s")  # Plus rapide

# Conseil : mettre les variables globales en local dans les boucles
def optimized():
    local_ref = GLOBAL_VAR  # Copie locale
    result = 0
    for _ in range(1000):
        result += local_ref
    return result
```

### Éviter les appels de méthodes répétés

```python
import timeit

data = list(range(1000))

# ❌ Appel répété de len()
def with_repeated_call():
    for i in range(len(data)):
        x = data[i]

# ✅ Stocker len() dans une variable
def with_stored_len():
    length = len(data)
    for i in range(length):
        x = data[i]

# ✅✅ Meilleure approche : itération directe
def with_direct_iteration():
    for item in data:
        x = item

time1 = timeit.timeit(with_repeated_call, number=10000)
time2 = timeit.timeit(with_stored_len, number=10000)
time3 = timeit.timeit(with_direct_iteration, number=10000)

print(f"Repeated call: {time1:.4f}s")
print(f"Stored len:    {time2:.4f}s")
print(f"Direct iter:   {time3:.4f}s")  # Le plus rapide
```

### String concatenation

```python
import timeit

# ❌ Concaténation avec +
def concat_with_plus():
    result = ""
    for i in range(1000):
        result += str(i)
    return result

# ✅ Join (beaucoup plus rapide)
def concat_with_join():
    return "".join(str(i) for i in range(1000))

# ✅ List + join
def concat_with_list():
    parts = []
    for i in range(1000):
        parts.append(str(i))
    return "".join(parts)

time_plus = timeit.timeit(concat_with_plus, number=1000)
time_join = timeit.timeit(concat_with_join, number=1000)
time_list = timeit.timeit(concat_with_list, number=1000)

print(f"+ operator: {time_plus:.4f}s")
print(f"join:       {time_join:.4f}s")  # Beaucoup plus rapide
print(f"list+join:  {time_list:.4f}s")
```

### Utiliser les bonnes structures de données

```python
import timeit

# Recherche d'appartenance
# Liste vs Set

data_list = list(range(10000))
data_set = set(range(10000))

# Recherche dans liste : O(n)
time_list = timeit.timeit(
    '9999 in data_list',
    globals=globals(),
    number=10000
)

# Recherche dans set : O(1)
time_set = timeit.timeit(
    '9999 in data_set',
    globals=globals(),
    number=10000
)

print(f"List: {time_list:.4f}s")
print(f"Set:  {time_set:.4f}s")  # Beaucoup plus rapide

# Suppression des doublons
numbers = list(range(1000)) * 10

# ❌ Avec liste
def remove_duplicates_list():
    result = []
    for num in numbers:
        if num not in result:
            result.append(num)
    return result

# ✅ Avec set
def remove_duplicates_set():
    return list(set(numbers))

time_list = timeit.timeit(remove_duplicates_list, number=100)
time_set = timeit.timeit(remove_duplicates_set, number=100)

print(f"List method: {time_list:.4f}s")
print(f"Set method:  {time_set:.4f}s")  # Beaucoup plus rapide
```

---

## 9.4 Cache et memoization

### functools.lru_cache

```python
from functools import lru_cache
import time

# Sans cache
def fibonacci(n):
    if n < 2:
        return n
    return fibonacci(n-1) + fibonacci(n-2)

# Avec cache
@lru_cache(maxsize=128)
def fibonacci_cached(n):
    if n < 2:
        return n
    return fibonacci_cached(n-1) + fibonacci_cached(n-2)

# Comparaison
start = time.perf_counter()
result = fibonacci(30)
time_no_cache = time.perf_counter() - start

start = time.perf_counter()
result = fibonacci_cached(30)
time_with_cache = time.perf_counter() - start

print(f"Sans cache: {time_no_cache:.4f}s")
print(f"Avec cache: {time_with_cache:.6f}s")  # Des milliers de fois plus rapide

# Informations sur le cache
print(fibonacci_cached.cache_info())
# CacheInfo(hits=28, misses=31, maxsize=128, currsize=31)

# Vider le cache
fibonacci_cached.cache_clear()

# Cache sans limite de taille (attention à la mémoire)
@lru_cache(maxsize=None)
def expensive_function(n):
    time.sleep(0.1)
    return n * 2

# maxsize doit être une puissance de 2 pour meilleures performances
# Par défaut: maxsize=128
```

### Cache personnalisé avec décorateur

```python
from functools import wraps

def memoize(func):
    """Cache simple sans limite de taille."""
    cache = {}
    
    @wraps(func)
    def wrapper(*args):
        if args not in cache:
            cache[args] = func(*args)
        return cache[args]
    
    wrapper.cache = cache
    wrapper.cache_clear = cache.clear
    
    return wrapper

@memoize
def fibonacci(n):
    if n < 2:
        return n
    return fibonacci(n-1) + fibonacci(n-2)

result = fibonacci(100)
print(f"Cache size: {len(fibonacci.cache)}")

# Cache avec TTL (Time To Live)
import time

def ttl_cache(seconds):
    """Cache avec expiration."""
    def decorator(func):
        cache = {}
        
        @wraps(func)
        def wrapper(*args):
            now = time.time()
            
            # Nettoyer les entrées expirées
            expired = [k for k, (_, timestamp) in cache.items() 
                      if now - timestamp > seconds]
            for k in expired:
                del cache[k]
            
            # Vérifier le cache
            if args in cache:
                value, timestamp = cache[args]
                if now - timestamp <= seconds:
                    return value
            
            # Calculer et mettre en cache
            value = func(*args)
            cache[args] = (value, now)
            return value
        
        return wrapper
    return decorator

@ttl_cache(seconds=5)
def get_data(key):
    print(f"Fetching {key}...")
    time.sleep(1)
    return f"Data for {key}"

print(get_data("test"))  # Fetching test...
print(get_data("test"))  # Cache (pas de print)
time.sleep(6)
print(get_data("test"))  # Fetching test... (cache expiré)
```

### Cache pour méthodes de classe

```python
from functools import lru_cache

class DataProcessor:
    def __init__(self, data):
        self.data = data
    
    # ❌ Problème : self n'est pas hashable
    # @lru_cache
    # def process(self):
    #     pass
    
    # ✅ Solution 1 : méthode statique
    @staticmethod
    @lru_cache(maxsize=128)
    def process_data(data_tuple):
        # Traitement coûteux
        return sum(data_tuple)
    
    def process(self):
        return self.process_data(tuple(self.data))
    
    # ✅ Solution 2 : cached_property (Python 3.8+)
    from functools import cached_property
    
    @cached_property
    def expensive_computation(self):
        """Calculé une fois, puis mis en cache."""
        print("Calcul coûteux...")
        return sum(x ** 2 for x in self.data)

processor = DataProcessor([1, 2, 3, 4, 5])
print(processor.expensive_computation)  # Calcul coûteux...
print(processor.expensive_computation)  # Cache (pas de print)
```

---

## 9.5 Optimisations avancées

### Utiliser les modules C

```python
# Modules optimisés en C pour certaines opérations

# array - tableaux typés (plus efficaces que list pour nombres)
import array

# Liste Python
python_list = [i for i in range(1000)]

# Array typé
int_array = array.array('i', range(1000))  # 'i' = signed int

import sys
print(f"Liste:  {sys.getsizeof(python_list)} bytes")
print(f"Array:  {sys.getsizeof(int_array)} bytes")  # Plus compact

# numpy - pour calculs numériques (si disponible)
try:
    import numpy as np
    
    # Liste Python
    python_list = list(range(1000000))
    
    # NumPy array
    numpy_array = np.arange(1000000)
    
    import timeit
    
    time_list = timeit.timeit(
        'sum(python_list)',
        globals=globals(),
        number=100
    )
    
    time_numpy = timeit.timeit(
        'numpy_array.sum()',
        globals=globals(),
        number=100
    )
    
    print(f"Liste:  {time_list:.4f}s")
    print(f"NumPy:  {time_numpy:.4f}s")  # Beaucoup plus rapide
except ImportError:
    print("NumPy non disponible")
```

### Éviter les copies inutiles

```python
import copy

data = [[1, 2, 3] for _ in range(1000)]

# Shallow copy (rapide, mais partage les objets internes)
shallow = data.copy()
shallow = data[:]
shallow = list(data)

# Deep copy (lent, mais copie tout)
deep = copy.deepcopy(data)

import timeit

time_shallow = timeit.timeit(
    'data.copy()',
    globals=globals(),
    number=10000
)

time_deep = timeit.timeit(
    'copy.deepcopy(data)',
    setup='import copy',
    globals=globals(),
    number=10000
)

print(f"Shallow copy: {time_shallow:.4f}s")
print(f"Deep copy:    {time_deep:.4f}s")

# Souvent, on peut éviter la copie complètement
# En passant des vues ou en modifiant sur place
```

### Lazy evaluation

```python
# Évaluation paresseuse avec générateurs

# ❌ Calcule tout immédiatement
def process_all(data):
    results = []
    for item in data:
        results.append(expensive_operation(item))
    return results

# ✅ Calcule à la demande
def process_lazy(data):
    for item in data:
        yield expensive_operation(item)

# Utilisation
data = range(1000000)

# Avec process_all : calcule tout de suite (lent au démarrage)
# results = process_all(data)

# Avec process_lazy : calcule au fur et à mesure
results = process_lazy(data)
first_10 = list(itertools.islice(results, 10))  # Seulement 10 calculs
```

### Slots pour économiser la mémoire

```python
import sys

# Classe normale
class NormalPerson:
    def __init__(self, name, age):
        self.name = name
        self.age = age

# Classe avec __slots__
class SlotsPerson:
    __slots__ = ['name', 'age']
    
    def __init__(self, name, age):
        self.name = name
        self.age = age

# Comparaison mémoire
normal = NormalPerson("Alice", 30)
slots = SlotsPerson("Alice", 30)

print(f"Normal: {sys.getsizeof(normal)} bytes")
print(f"Slots:  {sys.getsizeof(slots)} bytes")  # Plus compact

# Avec __slots__, pas de __dict__
# print(normal.__dict__)  # Fonctionne
# print(slots.__dict__)   # AttributeError

# Avantages :
# - Économie de mémoire (important pour beaucoup d'instances)
# - Accès légèrement plus rapide
# - Empêche l'ajout dynamique d'attributs

# Inconvénients :
# - Moins flexible
# - Ne peut pas ajouter d'attributs dynamiquement
```

### Multiprocessing pour parallélisme

```python
from multiprocessing import Pool
import time

def expensive_task(n):
    """Tâche CPU-intensive."""
    total = 0
    for i in range(n):
        total += i ** 2
    return total

# Séquentiel
def sequential():
    results = []
    for i in range(8):
        results.append(expensive_task(10000000))
    return results

# Parallèle
def parallel():
    with Pool(processes=4) as pool:
        results = pool.map(expensive_task, [10000000] * 8)
    return results

# Comparaison
start = time.time()
sequential()
time_seq = time.time() - start

start = time.time()
parallel()
time_par = time.time() - start

print(f"Séquentiel: {time_seq:.2f}s")
print(f"Parallèle:  {time_par:.2f}s")

# Note : Multiprocessing a un overhead
# Utile pour tâches CPU-intensive
# Pour I/O-bound, utilisez threading ou asyncio
```

---

## 🎯 Points clés à retenir

### Mesure
- **timeit** pour benchmarks précis
- **cProfile** pour profiling complet
- **line_profiler** pour ligne par ligne
- **memory_profiler** pour la mémoire
- Toujours mesurer avant d'optimiser !

### Big O
- **O(1)** : accès dict/set
- **O(log n)** : recherche binaire
- **O(n)** : parcours simple
- **O(n log n)** : tri efficace
- **O(n²)** : boucles imbriquées
- Choisir la bonne structure de données

### Optimisations
- **Compréhensions** > boucles
- **Générateurs** pour économie mémoire
- **Variables locales** > globales
- **join()** pour strings
- **set** pour recherches
- **lru_cache** pour calculs répétés

### Structures de données
- **list** : accès rapide, insertion lente au début
- **dict/set** : O(1) pour recherche
- **deque** : O(1) aux deux bouts
- **tuple** : léger, immuable

### Cache
- **@lru_cache** pour fonctions pures
- TTL cache pour données volatiles
- Attention à la mémoire
- Vider le cache si nécessaire

---

## 💡 Questions d'entretien courantes (Chapitre 9)

1. **Comment mesurer la performance d'un code ?**
   - timeit pour benchmarks
   - cProfile pour profiling
   - time.perf_counter() pour mesures simples
   - Toujours mesurer avant d'optimiser

2. **Quelle est la complexité de dict[key] ?**
   - O(1) en moyenne
   - O(n) pire cas (rare, collisions)
   - Implémentation par table de hachage

3. **List vs Tuple pour performance ?**
   - Tuple légèrement plus rapide
   - Tuple immuable → optimisations possibles
   - Différence minime en pratique
   - Choisir selon mutabilité nécessaire

4. **Pourquoi les générateurs sont-ils efficaces ?**
   - Paresseux : calcule à la demande
   - Économie mémoire (pas de liste complète)
   - Bon pour pipelines de données
   - Peut être infini

5. **Comment optimiser string concatenation ?**
   - Éviter + en boucle (O(n²))
   - Utiliser join() : O(n)
   - Ou construire liste puis join()

6. **Quand utiliser lru_cache ?**
   - Fonctions pures (même entrée → même sortie)
   - Calculs coûteux répétés
   - Arguments hashables
   - Attention à la mémoire avec maxsize=None

---

## 🔧 Exercices pratiques

1. **Benchmark de tri**
   - Comparez bubble sort, merge sort, Python sorted()
   - Mesurez avec différentes tailles
   - Graphez les résultats

2. **Optimisation de recherche**
   - Code avec recherche linéaire lente
   - Convertissez en recherche dict/set
   - Mesurez l'amélioration

3. **Profiling d'application**
   - Programme avec plusieurs fonctions
   - Profilez avec cProfile
   - Identifiez les bottlenecks

4. **Cache intelligent**
   - Implémentez cache LRU manuel
   - Ajoutez TTL
   - Comparez avec @lru_cache

5. **Pipeline de données**
   - Traitement de gros fichier
   - Comparez liste vs générateur
   - Mesurez mémoire et temps

---

**Prochaine étape :** Chapitre 10 - Questions d'Entretien Courantes (la préparation finale !)
