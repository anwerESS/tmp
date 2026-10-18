## [**..**](./00_index.md)

# Chapitre 5 : Modules et Packages

## 🎯 Objectifs du chapitre
- Comprendre le système d'import de Python
- Créer et organiser ses propres modules
- Maîtriser les modules standard essentiels
- Utiliser efficacement os, sys, re, collections, itertools
- Structurer un projet Python professionnel

---

## 5.1 Import de modules

### Syntaxes d'import

```python
# Import simple
import math
print(math.sqrt(16))  # 4.0
print(math.pi)        # 3.141592653589793

# Import avec alias
import numpy as np
import pandas as pd

# Import spécifique
from math import sqrt, pi
print(sqrt(16))  # 4.0
print(pi)        # 3.141592653589793

# Import tout (À ÉVITER)
from math import *
print(sqrt(16))  # Quel module? Pas clair!

# Import multiple sur une ligne (À ÉVITER pour lisibilité)
import os, sys, re

# PRÉFÉRABLE
import os
import sys
import re
```

### Import relatifs vs absolus

```python
# Structure du projet:
# myproject/
# ├── __init__.py
# ├── module1.py
# ├── module2.py
# └── package1/
#     ├── __init__.py
#     ├── submodule1.py
#     └── submodule2.py

# Dans myproject/module1.py
# Import absolu (RECOMMANDÉ)
from myproject.package1 import submodule1
from myproject.module2 import some_function

# Dans myproject/package1/submodule1.py
# Import relatif
from . import submodule2        # Même package
from .. import module2          # Package parent
from ..module1 import something # Module dans le parent

# Règle: utilisez les imports absolus pour la clarté
# Imports relatifs uniquement pour les packages réutilisables
```

### Éviter les imports circulaires

```python
# ❌ PROBLÈME: Import circulaire
# file1.py
from file2 import function2
def function1():
    return function2()

# file2.py
from file1 import function1  # Erreur!
def function2():
    return function1()

# ✅ SOLUTION 1: Import dans la fonction
# file1.py
def function1():
    from file2 import function2
    return function2()

# file2.py
def function2():
    from file1 import function1
    return function1()

# ✅ SOLUTION 2: Restructurer le code
# utils.py
def shared_function():
    pass

# file1.py
from utils import shared_function
def function1():
    return shared_function()

# file2.py
from utils import shared_function
def function2():
    return shared_function()
```

### Import conditionnel

```python
# Import selon la disponibilité
try:
    import numpy as np
    HAS_NUMPY = True
except ImportError:
    HAS_NUMPY = False
    print("NumPy n'est pas installé")

if HAS_NUMPY:
    def process_with_numpy(data):
        return np.array(data).mean()
else:
    def process_with_numpy(data):
        return sum(data) / len(data)

# Import selon la version Python
import sys
if sys.version_info >= (3, 10):
    from typing import TypeAlias
else:
    from typing_extensions import TypeAlias

# Import lazy (Python 3.7+)
from importlib import import_module
def get_module():
    return import_module('heavy_module')
```

---

## 5.2 Création de modules

### Module simple

```python
# mymodule.py
"""
Module de démonstration.
Fournit des fonctions mathématiques de base.
"""

__version__ = '1.0.0'
__author__ = 'Votre Nom'

# Constante
PI = 3.14159

# Variable privée (convention)
_internal_value = 42

# Fonction publique
def add(a, b):
    """Additionne deux nombres."""
    return a + b

def multiply(a, b):
    """Multiplie deux nombres."""
    return a * b

# Fonction privée (convention)
def _helper_function():
    """Fonction interne, non destinée à l'export."""
    return _internal_value

# Code exécuté à l'import
print(f"Module {__name__} importé")

# Code exécuté uniquement si lancé directement
if __name__ == '__main__':
    print("Module exécuté directement")
    print(f"2 + 3 = {add(2, 3)}")
    print(f"2 * 3 = {multiply(2, 3)}")
```

### Utilisation du module

```python
# main.py
import mymodule

print(mymodule.add(5, 3))      # 8
print(mymodule.PI)             # 3.14159
print(mymodule.__version__)    # 1.0.0

# Import spécifique
from mymodule import add, multiply
print(add(5, 3))               # 8

# _internal_value est accessible mais déconseillé
print(mymodule._internal_value)  # 42 (mais ne le faites pas!)
```

### __name__ == "__main__"

```python
# script.py
def main():
    """Fonction principale du script."""
    print("Exécution du script")
    # Code principal ici

def helper():
    """Fonction utilitaire."""
    return "Helper"

# Point d'entrée
if __name__ == '__main__':
    # Exécuté seulement si lancé directement
    # Pas exécuté si importé
    main()
else:
    # Exécuté seulement si importé
    print("Module importé")

# Usage:
# python script.py  → Exécute main()
# import script     → N'exécute pas main()
```

### Packages (répertoires avec __init__.py)

```python
# Structure:
# mypackage/
# ├── __init__.py
# ├── module1.py
# ├── module2.py
# └── subpackage/
#     ├── __init__.py
#     └── module3.py

# mypackage/__init__.py
"""Package de démonstration."""

__version__ = '1.0.0'
__all__ = ['module1', 'module2']  # Contrôle 'from mypackage import *'

# Import automatique de certains modules
from .module1 import function1
from .module2 import Class2

# Exposer au niveau du package
public_function = function1

# mypackage/module1.py
def function1():
    return "Function 1"

def function2():
    return "Function 2"

# mypackage/module2.py
class Class2:
    pass

# Utilisation
import mypackage
mypackage.function1()          # Disponible directement
mypackage.public_function()    # Alias

from mypackage import module1
module1.function1()

from mypackage.subpackage import module3
```

### __all__ - Contrôler les exports

```python
# utils.py
"""Module utilitaire."""

__all__ = ['public_func', 'PublicClass']  # API publique

def public_func():
    """Fonction publique."""
    pass

def _private_func():
    """Fonction privée."""
    pass

class PublicClass:
    """Classe publique."""
    pass

class _PrivateClass:
    """Classe privée."""
    pass

# Utilisation
from utils import *
# Importe seulement: public_func, PublicClass
# Pas: _private_func, _PrivateClass
```

---

## 5.3 Modules standard essentiels

### os - Interaction avec le système d'exploitation

```python
import os

# Informations système
print(os.name)          # 'posix' (Unix) ou 'nt' (Windows)
print(os.getcwd())      # Répertoire courant
print(os.getlogin())    # Nom d'utilisateur

# Navigation
os.chdir('/tmp')        # Changer de répertoire
print(os.getcwd())

# Chemins
path = os.path.join('dir', 'subdir', 'file.txt')  # Portable
print(os.path.exists(path))
print(os.path.isfile(path))
print(os.path.isdir(path))
print(os.path.abspath('file.txt'))
print(os.path.dirname('/path/to/file.txt'))   # /path/to
print(os.path.basename('/path/to/file.txt'))  # file.txt
print(os.path.splitext('file.txt'))           # ('file', '.txt')

# Répertoires
os.mkdir('new_dir')                    # Créer
os.makedirs('path/to/dir', exist_ok=True)  # Créer avec parents
os.rmdir('empty_dir')                  # Supprimer (doit être vide)
os.listdir('.')                        # Liste les fichiers

# Fichiers
os.remove('file.txt')                  # Supprimer fichier
os.rename('old.txt', 'new.txt')        # Renommer

# Variables d'environnement
print(os.environ.get('HOME'))          # Variable d'environnement
os.environ['MY_VAR'] = 'value'         # Définir

# Processus
print(os.getpid())                     # PID du processus
os.system('ls -la')                    # Exécuter commande (déprécié, utilisez subprocess)

# Parcourir récursivement
for root, dirs, files in os.walk('.'):
    print(f"Répertoire: {root}")
    for file in files:
        print(f"  Fichier: {os.path.join(root, file)}")
```

### sys - Paramètres système

```python
import sys

# Informations Python
print(sys.version)          # Version Python complète
print(sys.version_info)     # VersionInfo(major=3, minor=10, ...)
print(sys.platform)         # 'linux', 'win32', 'darwin'
print(sys.executable)       # Chemin de l'exécutable Python

# Arguments en ligne de commande
# python script.py arg1 arg2
print(sys.argv)             # ['script.py', 'arg1', 'arg2']
print(sys.argv[0])          # Nom du script
print(sys.argv[1:])         # Arguments

# Chemins de recherche des modules
print(sys.path)             # Liste des répertoires
sys.path.append('/custom/path')  # Ajouter un chemin

# Sortie du programme
sys.exit(0)                 # Code 0 = succès
sys.exit(1)                 # Code != 0 = erreur

# Entrée/Sortie standard
sys.stdout.write('Hello\n') # Écrire sur stdout
sys.stderr.write('Error\n') # Écrire sur stderr
line = sys.stdin.readline() # Lire depuis stdin

# Redirection
original_stdout = sys.stdout
with open('output.txt', 'w') as f:
    sys.stdout = f
    print("Va dans le fichier")
sys.stdout = original_stdout

# Taille des objets
import sys
data = [1, 2, 3, 4, 5]
print(sys.getsizeof(data))  # Taille en octets

# Limite de récursion
print(sys.getrecursionlimit())  # 1000 par défaut
sys.setrecursionlimit(2000)     # Augmenter
```

### re - Expressions régulières

```python
import re

# Recherche simple
text = "Mon email est user@example.com"
match = re.search(r'[\w.-]+@[\w.-]+\.\w+', text)
if match:
    print(match.group())  # user@example.com

# Patterns courants
email_pattern = r'[\w.-]+@[\w.-]+\.\w+'
phone_pattern = r'\d{3}-\d{3}-\d{4}'
url_pattern = r'https?://[\w.-]+\.\w+'

# Méthodes principales

# search() - trouve la première occurrence
text = "Prix: 100€, Poids: 50kg"
match = re.search(r'\d+', text)
print(match.group())  # '100'

# findall() - trouve toutes les occurrences
numbers = re.findall(r'\d+', text)
print(numbers)  # ['100', '50']

# match() - vérifie le début de la string
if re.match(r'\d+', '123abc'):
    print("Commence par des chiffres")

# fullmatch() - vérifie toute la string
if re.fullmatch(r'\d+', '12345'):
    print("Que des chiffres")

# split() - découpe selon un pattern
parts = re.split(r'[,;]', "apple,banana;orange")
print(parts)  # ['apple', 'banana', 'orange']

# sub() - remplace
text = "Prix: 100€"
result = re.sub(r'\d+', '200', text)
print(result)  # "Prix: 200€"

# Groupes de capture
text = "Date: 2024-01-29"
match = re.search(r'(\d{4})-(\d{2})-(\d{2})', text)
if match:
    year, month, day = match.groups()
    print(f"Année: {year}, Mois: {month}, Jour: {day}")

# Groupes nommés
pattern = r'(?P<year>\d{4})-(?P<month>\d{2})-(?P<day>\d{2})'
match = re.search(pattern, text)
if match:
    print(match.group('year'))   # 2024
    print(match.groupdict())     # {'year': '2024', 'month': '01', 'day': '29'}

# Flags
text = "Hello WORLD"
re.search(r'hello', text, re.IGNORECASE)  # Insensible à la casse
re.search(r'^line1.*line2$', text, re.MULTILINE)  # Mode multiligne
re.search(r'a.b', text, re.DOTALL)  # . matche aussi \n

# Compiler pour réutilisation
pattern = re.compile(r'\d+')
pattern.findall("123 abc 456")  # ['123', '456']

# Exemples pratiques
def validate_email(email):
    pattern = r'^[\w.-]+@[\w.-]+\.\w+$'
    return re.fullmatch(pattern, email) is not None

def extract_urls(text):
    pattern = r'https?://[\w.-]+(?:/[\w.-]*)*'
    return re.findall(pattern, text)

def clean_whitespace(text):
    # Remplace espaces multiples par un seul
    return re.sub(r'\s+', ' ', text).strip()

# Métacaractères
# .   : n'importe quel caractère
# ^   : début de string
# $   : fin de string
# *   : 0 ou plus
# +   : 1 ou plus
# ?   : 0 ou 1
# {n} : exactement n
# {n,}: n ou plus
# {n,m}: entre n et m
# []  : ensemble de caractères
# ()  : groupe
# |   : ou
# \d  : chiffre [0-9]
# \w  : mot [a-zA-Z0-9_]
# \s  : whitespace
# \D, \W, \S : inverse
```

### datetime - Manipulation de dates

```python
from datetime import datetime, date, time, timedelta

# Date et heure actuelles
now = datetime.now()
print(now)  # 2024-01-29 14:30:45.123456

today = date.today()
print(today)  # 2024-01-29

# Créer des dates
d = date(2024, 1, 29)
dt = datetime(2024, 1, 29, 14, 30, 45)
t = time(14, 30, 45)

# Formater (strftime)
now = datetime.now()
print(now.strftime('%Y-%m-%d'))           # 2024-01-29
print(now.strftime('%d/%m/%Y %H:%M:%S'))  # 29/01/2024 14:30:45
print(now.strftime('%A, %B %d, %Y'))      # Monday, January 29, 2024

# Parser (strptime)
date_str = "2024-01-29 14:30:45"
dt = datetime.strptime(date_str, '%Y-%m-%d %H:%M:%S')

# Composants
dt = datetime(2024, 1, 29, 14, 30, 45)
print(dt.year, dt.month, dt.day)
print(dt.hour, dt.minute, dt.second)

# Timedelta - Différences de temps
delta = timedelta(days=7, hours=3, minutes=30)
future = datetime.now() + delta
past = datetime.now() - timedelta(weeks=2)

# Différence entre deux dates
dt1 = datetime(2024, 1, 29)
dt2 = datetime(2024, 2, 15)
diff = dt2 - dt1
print(diff.days)           # 17
print(diff.total_seconds()) # 1468800.0

# Comparer des dates
if dt1 < dt2:
    print("dt1 est avant dt2")

# ISO format
now = datetime.now()
iso_str = now.isoformat()     # 2024-01-29T14:30:45.123456
parsed = datetime.fromisoformat(iso_str)

# Timestamp Unix
timestamp = datetime.now().timestamp()
dt = datetime.fromtimestamp(timestamp)

# Codes de format courants:
# %Y : année (4 chiffres)
# %m : mois (01-12)
# %d : jour (01-31)
# %H : heure (00-23)
# %M : minute (00-59)
# %S : seconde (00-59)
# %A : jour de la semaine (Monday)
# %B : mois (January)
```

### time - Mesure du temps

```python
import time

# Timestamp actuel
print(time.time())  # 1706537445.123456

# Pause
time.sleep(2)  # Pause de 2 secondes

# Mesurer le temps d'exécution
start = time.time()
# Code à mesurer
time.sleep(1)
end = time.time()
print(f"Durée: {end - start:.2f}s")

# Perf counter (plus précis)
start = time.perf_counter()
# Code
end = time.perf_counter()
print(f"Durée: {end - start:.6f}s")

# Formater le temps
now = time.time()
print(time.ctime(now))  # Mon Jan 29 14:30:45 2024
print(time.strftime('%Y-%m-%d', time.localtime(now)))
```

---

## 5.4 Collections - Structures de données avancées

### defaultdict - Dictionnaire avec valeur par défaut

```python
from collections import defaultdict

# Problème classique
word_count = {}
words = ['apple', 'banana', 'apple', 'cherry']
for word in words:
    if word not in word_count:
        word_count[word] = 0
    word_count[word] += 1

# Avec defaultdict
word_count = defaultdict(int)  # Valeur par défaut: 0
for word in words:
    word_count[word] += 1  # Pas besoin de vérifier

print(dict(word_count))  # {'apple': 2, 'banana': 1, 'cherry': 1}

# Autres usages
# Listes de valeurs par clé
groups = defaultdict(list)
data = [('a', 1), ('b', 2), ('a', 3), ('b', 4)]
for key, value in data:
    groups[key].append(value)
print(dict(groups))  # {'a': [1, 3], 'b': [2, 4]}

# Sets par clé
tags = defaultdict(set)
tags['python'].add('programming')
tags['python'].add('scripting')

# Dictionnaires imbriqués
nested = defaultdict(lambda: defaultdict(int))
nested['user1']['score'] = 100
nested['user1']['level'] = 5
```

### Counter - Compteur d'éléments

```python
from collections import Counter

# Compter des éléments
words = ['apple', 'banana', 'apple', 'cherry', 'banana', 'apple']
counter = Counter(words)
print(counter)  # Counter({'apple': 3, 'banana': 2, 'cherry': 1})

# Accès
print(counter['apple'])      # 3
print(counter['grape'])      # 0 (pas d'erreur)

# Éléments les plus communs
print(counter.most_common(2))  # [('apple', 3), ('banana', 2)]

# Opérations
c1 = Counter(['a', 'b', 'c', 'a'])
c2 = Counter(['a', 'b', 'd', 'b'])

print(c1 + c2)  # Addition
print(c1 - c2)  # Soustraction (garde positifs)
print(c1 & c2)  # Intersection (min)
print(c1 | c2)  # Union (max)

# Compter des caractères
text = "hello world"
char_count = Counter(text)
print(char_count.most_common(3))

# Compter depuis un itérable
from collections import Counter
numbers = [1, 2, 1, 3, 2, 1, 4]
counter = Counter(numbers)
```

### namedtuple - Tuple nommé

```python
from collections import namedtuple

# Créer un type
Point = namedtuple('Point', ['x', 'y'])

# Créer des instances
p1 = Point(10, 20)
p2 = Point(x=30, y=40)

# Accès
print(p1.x, p1.y)  # 10 20
print(p1[0], p1[1])  # 10 20 (comme un tuple)

# Immuable
# p1.x = 15  # AttributeError

# Utile pour des structures de données simples
Person = namedtuple('Person', ['name', 'age', 'city'])
person = Person('Alice', 30, 'Paris')
print(f"{person.name} a {person.age} ans")

# Convertir en dict
print(person._asdict())  # {'name': 'Alice', 'age': 30, 'city': 'Paris'}

# Remplacer des valeurs (crée un nouveau tuple)
person2 = person._replace(age=31)

# Valeurs par défaut (Python 3.7+)
Task = namedtuple('Task', ['name', 'priority', 'done'], defaults=[False])
task = Task('Faire les courses', 1)
print(task.done)  # False
```

### deque - Double-ended queue

```python
from collections import deque

# Créer une deque
d = deque([1, 2, 3])

# Ajouter aux extrémités
d.append(4)      # Droite: [1, 2, 3, 4]
d.appendleft(0)  # Gauche: [0, 1, 2, 3, 4]

# Retirer des extrémités
d.pop()          # Droite: [0, 1, 2, 3]
d.popleft()      # Gauche: [1, 2, 3]

# Rotation
d = deque([1, 2, 3, 4, 5])
d.rotate(2)   # [4, 5, 1, 2, 3]
d.rotate(-1)  # [5, 1, 2, 3, 4]

# Taille maximale (buffer circulaire)
d = deque(maxlen=3)
d.extend([1, 2, 3, 4, 5])  # Garde seulement les 3 derniers
print(d)  # deque([3, 4, 5], maxlen=3)

# Cas d'usage: historique limité
history = deque(maxlen=10)
for i in range(15):
    history.append(f"Event {i}")
print(history)  # Garde seulement les 10 derniers

# Performance: O(1) pour append/pop aux deux extrémités
# vs list: O(1) à droite, O(n) à gauche
```

### OrderedDict - Dictionnaire ordonné

```python
from collections import OrderedDict

# Note: depuis Python 3.7, dict garde l'ordre d'insertion
# OrderedDict a encore quelques différences

# Création
od = OrderedDict()
od['a'] = 1
od['b'] = 2
od['c'] = 3

# Ordre préservé
for key in od:
    print(key)  # a, b, c

# Déplacer à la fin
od.move_to_end('a')  # {'b': 2, 'c': 3, 'a': 1}

# Déplacer au début
od.move_to_end('c', last=False)  # {'c': 3, 'b': 2, 'a': 1}

# popitem - LIFO par défaut
od.popitem()  # Retire 'a': 1
od.popitem(last=False)  # Retire 'c': 3

# Égalité prend en compte l'ordre
od1 = OrderedDict([('a', 1), ('b', 2)])
od2 = OrderedDict([('b', 2), ('a', 1)])
print(od1 == od2)  # False (ordre différent)

d1 = {'a': 1, 'b': 2}
d2 = {'b': 2, 'a': 1}
print(d1 == d2)  # True (dict normal ignore l'ordre)
```

### ChainMap - Combiner plusieurs dicts

```python
from collections import ChainMap

# Combiner des dictionnaires
dict1 = {'a': 1, 'b': 2}
dict2 = {'b': 3, 'c': 4}
dict3 = {'c': 5, 'd': 6}

chain = ChainMap(dict1, dict2, dict3)
print(chain['a'])  # 1 (depuis dict1)
print(chain['b'])  # 2 (depuis dict1, priorité)
print(chain['c'])  # 4 (depuis dict2)
print(chain['d'])  # 6 (depuis dict3)

# Cas d'usage: configuration avec fallbacks
import os
defaults = {'debug': False, 'timeout': 30}
env_vars = {k.lower(): v for k, v in os.environ.items()}
config = ChainMap(env_vars, defaults)

# Modification affecte le premier dict
chain['a'] = 10  # Modifie dict1
```

---

## 5.5 itertools - Itérateurs efficaces

### Itérateurs infinis

```python
from itertools import count, cycle, repeat

# count() - compteur infini
for i in count(10, 2):  # Commence à 10, pas de 2
    if i > 20:
        break
    print(i)  # 10, 12, 14, 16, 18, 20

# cycle() - répète une séquence indéfiniment
counter = 0
for item in cycle(['A', 'B', 'C']):
    if counter >= 7:
        break
    print(item, end=' ')  # A B C A B C A
    counter += 1

# repeat() - répète une valeur
for x in repeat(10, 3):  # Répète 10, 3 fois
    print(x)  # 10, 10, 10
```

### Combinaisons et permutations

```python
from itertools import combinations, permutations, product

# permutations - arrangements
items = ['A', 'B', 'C']
for perm in permutations(items, 2):
    print(perm)
# ('A', 'B'), ('A', 'C'), ('B', 'A'), ('B', 'C'), ('C', 'A'), ('C', 'B')

# combinations - combinaisons
for comb in combinations(items, 2):
    print(comb)
# ('A', 'B'), ('A', 'C'), ('B', 'C')

# combinations_with_replacement
from itertools import combinations_with_replacement
for comb in combinations_with_replacement(['A', 'B'], 2):
    print(comb)
# ('A', 'A'), ('A', 'B'), ('B', 'B')

# product - produit cartésien
for item in product(['A', 'B'], [1, 2]):
    print(item)
# ('A', 1), ('A', 2), ('B', 1), ('B', 2)

# Équivalent à des boucles imbriquées
for item in product(range(2), repeat=3):
    print(item)
# (0,0,0), (0,0,1), (0,1,0), (0,1,1), (1,0,0), (1,0,1), (1,1,0), (1,1,1)
```

### Groupement et filtrage

```python
from itertools import groupby, filterfalse, takewhile, dropwhile

# groupby - groupe les éléments consécutifs
data = ['A', 'A', 'B', 'B', 'B', 'C', 'C']
for key, group in groupby(data):
    print(key, list(group))
# A ['A', 'A']
# B ['B', 'B', 'B']
# C ['C', 'C']

# ATTENTION: nécessite un tri préalable pour grouper tous les éléments
data = [('A', 1), ('B', 2), ('A', 3), ('B', 4)]
data.sort(key=lambda x: x[0])
for key, group in groupby(data, key=lambda x: x[0]):
    print(key, list(group))

# filterfalse - inverse de filter
numbers = [1, 2, 3, 4, 5, 6]
evens = list(filterfalse(lambda x: x % 2, numbers))
print(evens)  # [2, 4, 6]

# takewhile - prend tant que la condition est vraie
numbers = [1, 2, 3, 4, 1, 2]
result = list(takewhile(lambda x: x < 4, numbers))
print(result)  # [1, 2, 3]

# dropwhile - ignore tant que la condition est vraie
result = list(dropwhile(lambda x: x < 4, numbers))
print(result)  # [4, 1, 2]
```

### Chaînes et agrégations

```python
from itertools import chain, accumulate, compress

# chain - concatène des itérables
list1 = [1, 2, 3]
list2 = [4, 5, 6]
list3 = [7, 8, 9]
result = list(chain(list1, list2, list3))
print(result)  # [1, 2, 3, 4, 5, 6, 7, 8, 9]

# chain.from_iterable - pour un itérable d'itérables
nested = [[1, 2], [3, 4], [5, 6]]
result = list(chain.from_iterable(nested))
print(result)  # [1, 2, 3, 4, 5, 6]

# accumulate - somme cumulée (ou autre opération)
numbers = [1, 2, 3, 4, 5]
cumsum = list(accumulate(numbers))
print(cumsum)  # [1, 3, 6, 10, 15]

# Avec opération personnalisée
import operator
cumproduct = list(accumulate(numbers, operator.mul))
print(cumproduct)  # [1, 2, 6, 24, 120]

# compress - filtre selon un masque booléen
data = ['A', 'B', 'C', 'D', 'E']
selectors = [1, 0, 1, 0, 1]
result = list(compress(data, selectors))
print(result)  # ['A', 'C', 'E']
```

### Découpage

```python
from itertools import islice

# islice - découpe un itérateur
numbers = range(10)
result = list(islice(numbers, 5))  # 5 premiers
print(result)  # [0, 1, 2, 3, 4]

result = list(islice(numbers, 2, 7))  # De 2 à 7
print(result)  # [2, 3, 4, 5, 6]

result = list(islice(numbers, 0, 10, 2))  # De 0 à 10, pas de 2
print(result)  # [0, 2, 4, 6, 8]

# Utile pour les gros fichiers
def read_first_n_lines(filename, n):
    with open(filename) as f:
        return list(islice(f, n))
```

### Exemples pratiques

```python
from itertools import *

# Générer toutes les paires
def all_pairs(items):
    return combinations(items, 2)

# Générer des batches
def batches(iterable, n):
    it = iter(iterable)
    while True:
        batch = list(islice(it, n))
        if not batch:
            return
        yield batch

for batch in batches(range(10), 3):
    print(batch)
# [0, 1, 2], [3, 4, 5], [6, 7, 8], [9]

# Aplatir une liste
def flatten(nested):
    return chain.from_iterable(nested)

nested = [[1, 2], [3, 4], [5, 6]]
print(list(flatten(nested)))  # [1, 2, 3, 4, 5, 6]

# Fenêtre glissante
def window(iterable, n):
    from collections import deque
    it = iter(iterable)
    win = deque(islice(it, n), maxlen=n)
    if len(win) == n:
        yield tuple(win)
    for item in it:
        win.append(item)
        yield tuple(win)

for w in window([1, 2, 3, 4, 5], 3):
    print(w)
# (1, 2, 3), (2, 3, 4), (3, 4, 5)
```

---

## 🎯 Points clés à retenir

### Imports
- **Import absolu** préféré pour la clarté
- **Import relatif** pour packages réutilisables
- Éviter `from module import *`
- `if __name__ == '__main__':` pour code exécutable

### Modules standard essentiels
- **os** : système de fichiers, variables d'env
- **sys** : arguments, version Python, exit
- **re** : expressions régulières
- **datetime** : manipulation de dates/heures

### Collections
- **defaultdict** : dict avec valeur par défaut
- **Counter** : compteur d'éléments
- **namedtuple** : tuple avec champs nommés
- **deque** : file double-ended, O(1) aux extrémités

### itertools
- **Combinaisons** : permutations, combinations, product
- **Groupement** : groupby, chain, compress
- **Filtrage** : takewhile, dropwhile, filterfalse
- **Découpage** : islice, accumulate

---

## 💡 Questions d'entretien courantes (Chapitre 5)

1. **Différence entre `import module` et `from module import function` ?**
   - `import module` : importe le module, accès via `module.function`
   - `from module import function` : importe la fonction directement
   - Premier évite les conflits de noms

2. **Que fait `if __name__ == '__main__':` ?**
   - `__name__` vaut `'__main__'` si le script est exécuté directement
   - Permet de séparer code exécutable et code réutilisable
   - Code dans ce bloc ne s'exécute pas lors d'un import

3. **Quand utiliser defaultdict vs dict normal ?**
   - defaultdict quand on veut éviter les KeyError
   - Utile pour grouper (listes), compter (int), etc.
   - dict normal quand absence de clé = information

4. **Différence entre Counter et defaultdict(int) ?**
   - Counter : conçu pour compter, méthodes `most_common()`, opérations
   - defaultdict(int) : plus générique
   - Counter hérite de dict

5. **Pourquoi utiliser itertools ?**
   - Mémoire efficace (itérateurs, pas de listes)
   - Combinatoires (combinations, permutations)
   - Code plus lisible et pythonique

6. **Regex : différence entre search() et match() ?**
   - `search()` : trouve n'importe où dans la string
   - `match()` : vérifie uniquement le début
   - `fullmatch()` : vérifie toute la string

---

## 🔧 Exercices pratiques

1. **Analyseur de logs**
   - Utilisez `re` pour extraire timestamps, niveaux, messages
   - Groupez avec `defaultdict` et `Counter`
   - Affichez les erreurs les plus fréquentes

2. **Gestionnaire de configuration**
   - Créez un module avec `__init__.py`
   - Chargez config depuis fichier ou variables d'env (os.environ)
   - Utilisez ChainMap pour les fallbacks

3. **Générateur de combinaisons**
   - Utilisez `itertools.product` pour générer toutes les configs possibles
   - Exemple: couleurs × tailles × matières
   - Limitez avec `islice` pour grandes combinaisons

4. **Script de renommage de fichiers**
   - Utilisez `os` et `re` pour trouver/renommer des fichiers
   - Pattern: remplacer dates, numéroter séquentiellement
   - Affichez avant/après avec logging

5. **Analyseur de texte**
   - Counter pour fréquence des mots
   - re pour nettoyer (ponctuation, stopwords)
   - namedtuple pour les résultats

---

**Prochaine étape :** Chapitre 6 - Scripting et Ligne de Commande (le plus important pour votre entretien !)
