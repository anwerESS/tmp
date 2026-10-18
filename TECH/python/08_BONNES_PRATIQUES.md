## [**..**](./00_index.md)

# Chapitre 8 : Bonnes Pratiques et Style

## 🎯 Objectifs du chapitre
- Maîtriser les conventions PEP 8
- Écrire du code Python idiomatique (pythonique)
- Documenter efficacement avec docstrings et type hints
- Tester son code avec unittest et pytest
- Débugger efficacement avec pdb et logging

---

## 8.1 PEP 8 - Style de code

### Conventions de nommage

```python
# Modules et packages : lowercase avec underscores
# my_module.py, my_package/

# Classes : PascalCase (CapWords)
class MyClass:
    pass

class HTTPServer:
    pass

# Fonctions et variables : snake_case
def calculate_total():
    pass

user_name = "Alice"
total_count = 42

# Constantes : UPPER_CASE
MAX_SIZE = 100
DEFAULT_TIMEOUT = 30

# Variables privées : préfixe underscore
class Person:
    def __init__(self):
        self.public_var = "visible"
        self._protected_var = "convention"
        self.__private_var = "name mangling"
    
    def public_method(self):
        pass
    
    def _protected_method(self):
        pass
    
    def __private_method(self):
        pass

# Éviter les noms d'une lettre (sauf dans boucles courtes)
# ❌ MAUVAIS
l = [1, 2, 3]
O = 0

# ✅ BON
items = [1, 2, 3]
count = 0

# OK dans les boucles courtes
for i in range(10):
    print(i)

for x, y in coordinates:
    print(x, y)
```

### Indentation et espacement

```python
# Indentation : 4 espaces (PAS de tabs)
def my_function():
    if True:
        print("4 espaces")

# Longueur de ligne : max 79 caractères (code), 72 (commentaires/docstrings)

# ❌ Ligne trop longue
result = some_function(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10)

# ✅ Bonnes coupures
result = some_function(
    arg1, arg2, arg3,
    arg4, arg5, arg6,
    arg7, arg8, arg9,
    arg10
)

# Ou utiliser le backslash (moins préférable)
result = some_function(arg1, arg2, arg3, arg4, \
                      arg5, arg6, arg7, arg8)

# Listes/dicts sur plusieurs lignes
my_list = [
    1, 2, 3,
    4, 5, 6,
]

my_dict = {
    'key1': 'value1',
    'key2': 'value2',
    'key3': 'value3',
}

# Espaces autour des opérateurs
# ✅ BON
x = 1
y = 2
result = x + y

# ❌ MAUVAIS
x=1
y=2
result=x+y

# Pas d'espace avant les deux-points (sauf slicing)
# ✅ BON
if x == 1:
    pass

my_dict = {'key': 'value'}
my_list[1:3]

# ❌ MAUVAIS
if x == 1 :
    pass

my_dict = {'key' : 'value'}

# Espaces dans les appels de fonctions
# ✅ BON
func(arg1, arg2, kwarg1=value1)

# ❌ MAUVAIS
func( arg1, arg2, kwarg1 = value1 )
```

### Imports

```python
# Ordre des imports :
# 1. Standard library
# 2. Third-party
# 3. Local application

# ✅ BON
import os
import sys
from pathlib import Path

import numpy as np
import pandas as pd

from myproject.module1 import function1
from myproject.module2 import Class2

# ❌ MAUVAIS : tout mélangé
from myproject.module1 import function1
import numpy as np
import os

# Imports sur des lignes séparées
# ✅ BON
import os
import sys

# ❌ MAUVAIS
import os, sys

# Exception pour les types (Python 3.10+)
from typing import Union, Optional, List

# Éviter les imports wildcard
# ❌ MAUVAIS
from module import *

# ✅ BON
from module import func1, func2, Class1
# Ou
import module

# Imports absolus préférés
# ✅ BON
from myproject.subpackage import module

# OK si package réutilisable
from . import sibling
from .. import parent
```

### Lignes vides

```python
# 2 lignes vides avant les classes et fonctions de niveau module
import os


def function1():
    pass


def function2():
    pass


class MyClass:
    pass


class AnotherClass:
    pass


# 1 ligne vide entre les méthodes de classe
class MyClass:
    
    def method1(self):
        pass
    
    def method2(self):
        pass


# Lignes vides pour séparer les sections logiques
def complex_function():
    # Initialisation
    x = 0
    y = 0
    
    # Traitement
    for i in range(10):
        x += i
        y += i * 2
    
    # Retour
    return x, y
```

### Commentaires

```python
# Commentaires en ligne : 2 espaces avant le #
x = x + 1  # Incrémenter x

# ❌ MAUVAIS
x = x + 1# Pas d'espace
x = x + 1 #Un seul espace

# Commentaires de blocs : même indentation que le code
# Chaque ligne commence par # et un espace
def function():
    # Ceci est un commentaire de bloc
    # qui s'étend sur plusieurs lignes
    # et explique le code ci-dessous
    x = 1
    y = 2

# Éviter les commentaires évidents
# ❌ MAUVAIS
x = x + 1  # Ajoute 1 à x

# ✅ BON
x = x + 1  # Compense l'indexation à partir de 0

# TODO, FIXME, etc.
# TODO: Implémenter la gestion des erreurs
# FIXME: Ce code ne fonctionne pas avec des valeurs négatives
# NOTE: Cette optimisation a été ajoutée pour Python 3.9+
```

---

## 8.2 Code pythonique (idiomatique)

### Swap de variables

```python
# ❌ Non pythonique
temp = a
a = b
b = temp

# ✅ Pythonique
a, b = b, a
```

### Vérifier si une liste est vide

```python
my_list = []

# ❌ Non pythonique
if len(my_list) == 0:
    print("Vide")

# ✅ Pythonique (utilise la "truthiness")
if not my_list:
    print("Vide")

if my_list:
    print("Non vide")
```

### Compréhensions au lieu de boucles

```python
numbers = [1, 2, 3, 4, 5]

# ❌ Non pythonique
squares = []
for n in numbers:
    squares.append(n ** 2)

# ✅ Pythonique
squares = [n ** 2 for n in numbers]

# Avec condition
# ❌
evens = []
for n in numbers:
    if n % 2 == 0:
        evens.append(n)

# ✅
evens = [n for n in numbers if n % 2 == 0]
```

### enumerate au lieu de range(len(...))

```python
items = ['a', 'b', 'c']

# ❌ Non pythonique
for i in range(len(items)):
    print(i, items[i])

# ✅ Pythonique
for i, item in enumerate(items):
    print(i, item)

# Avec index de départ personnalisé
for i, item in enumerate(items, start=1):
    print(i, item)  # 1 a, 2 b, 3 c
```

### zip pour itérer sur plusieurs séquences

```python
names = ['Alice', 'Bob', 'Charlie']
ages = [25, 30, 35]

# ❌ Non pythonique
for i in range(len(names)):
    print(names[i], ages[i])

# ✅ Pythonique
for name, age in zip(names, ages):
    print(name, age)

# Créer un dictionnaire
# ❌
user_dict = {}
for i in range(len(names)):
    user_dict[names[i]] = ages[i]

# ✅
user_dict = dict(zip(names, ages))
```

### Unpacking

```python
# Unpacking basique
first, second, third = [1, 2, 3]

# Extended unpacking (Python 3)
first, *rest = [1, 2, 3, 4, 5]
print(first)  # 1
print(rest)   # [2, 3, 4, 5]

*beginning, last = [1, 2, 3, 4, 5]
print(beginning)  # [1, 2, 3, 4]
print(last)       # 5

first, *middle, last = [1, 2, 3, 4, 5]
print(middle)  # [2, 3, 4]

# Ignorer des valeurs avec _
first, _, third = [1, 2, 3]
print(first, third)  # 1 3
```

### with pour les ressources

```python
# ❌ Non pythonique
file = open('data.txt')
try:
    data = file.read()
finally:
    file.close()

# ✅ Pythonique
with open('data.txt') as file:
    data = file.read()
```

### get() pour les dictionnaires

```python
my_dict = {'a': 1, 'b': 2}

# ❌ Non pythonique
if 'c' in my_dict:
    value = my_dict['c']
else:
    value = 0

# ✅ Pythonique
value = my_dict.get('c', 0)

# setdefault pour obtenir ou créer
count = my_dict.setdefault('c', 0)
```

### String formatting

```python
name = "Alice"
age = 30

# ❌ Ancien style
message = "Je m'appelle %s et j'ai %d ans" % (name, age)

# ✅ Format moderne
message = "Je m'appelle {} et j'ai {} ans".format(name, age)

# ✅✅ F-strings (Python 3.6+) - MEILLEUR
message = f"Je m'appelle {name} et j'ai {age} ans"

# F-strings avec expressions
print(f"L'année prochaine j'aurai {age + 1} ans")
print(f"Total: {sum([1, 2, 3])}")
```

### any() et all()

```python
numbers = [1, 2, 3, 4, 5]

# ❌ Non pythonique
has_even = False
for n in numbers:
    if n % 2 == 0:
        has_even = True
        break

# ✅ Pythonique
has_even = any(n % 2 == 0 for n in numbers)

# all()
all_positive = all(n > 0 for n in numbers)
```

### Context managers multiples

```python
# ❌ Imbriqué
with open('input.txt') as infile:
    with open('output.txt', 'w') as outfile:
        outfile.write(infile.read())

# ✅ Pythonique
with open('input.txt') as infile, open('output.txt', 'w') as outfile:
    outfile.write(infile.read())
```

### Comparaisons chaînées

```python
x = 5

# ❌ Non pythonique
if x > 0 and x < 10:
    print("Entre 0 et 10")

# ✅ Pythonique
if 0 < x < 10:
    print("Entre 0 et 10")
```

### is vs ==

```python
# == compare les valeurs
# is compare l'identité (même objet)

# ✅ Pour None, True, False
if value is None:
    pass

if flag is True:
    pass

# ✅ Pour les valeurs
if x == 5:
    pass
```

---

## 8.3 Documentation

### Docstrings

```python
# Docstring de module (début du fichier)
"""
Module de gestion des utilisateurs.

Ce module fournit des fonctions pour créer, modifier et supprimer
des utilisateurs dans le système.
"""

# Docstring de fonction - Une ligne
def greet(name):
    """Retourne un message de salutation."""
    return f"Hello {name}"

# Docstring de fonction - Multi-lignes (style Google)
def calculate_statistics(numbers):
    """
    Calcule les statistiques d'une liste de nombres.
    
    Args:
        numbers (list[float]): Liste de nombres à analyser.
    
    Returns:
        dict: Dictionnaire contenant:
            - mean (float): Moyenne
            - median (float): Médiane
            - std (float): Écart-type
    
    Raises:
        ValueError: Si la liste est vide.
        TypeError: Si numbers n'est pas une liste.
    
    Examples:
        >>> calculate_statistics([1, 2, 3, 4, 5])
        {'mean': 3.0, 'median': 3.0, 'std': 1.414}
    """
    if not numbers:
        raise ValueError("La liste ne peut pas être vide")
    
    mean = sum(numbers) / len(numbers)
    # ... calculs
    return {'mean': mean, 'median': median, 'std': std}

# Docstring de fonction - Style NumPy
def complex_function(param1, param2, param3=None):
    """
    Description courte de la fonction.
    
    Description longue qui peut s'étendre sur plusieurs lignes
    et fournir plus de détails sur le fonctionnement de la fonction.
    
    Parameters
    ----------
    param1 : int
        Description du premier paramètre.
    param2 : str
        Description du deuxième paramètre.
    param3 : list, optional
        Description du paramètre optionnel (default is None).
    
    Returns
    -------
    bool
        Description de ce qui est retourné.
    
    Notes
    -----
    Notes additionnelles sur la fonction.
    
    Examples
    --------
    >>> complex_function(10, "test")
    True
    """
    pass

# Docstring de classe
class Person:
    """
    Représente une personne.
    
    Attributes:
        name (str): Le nom de la personne.
        age (int): L'âge de la personne.
        email (str, optional): L'email de la personne.
    
    Examples:
        >>> person = Person("Alice", 30)
        >>> person.greet()
        'Hello, my name is Alice'
    """
    
    def __init__(self, name, age, email=None):
        """
        Initialise une personne.
        
        Args:
            name (str): Le nom.
            age (int): L'âge.
            email (str, optional): L'email.
        """
        self.name = name
        self.age = age
        self.email = email
    
    def greet(self):
        """Retourne un message de salutation."""
        return f"Hello, my name is {self.name}"
```

### Type hints (Python 3.5+)

```python
from typing import List, Dict, Tuple, Optional, Union, Callable, Any

# Types de base
def greet(name: str) -> str:
    return f"Hello {name}"

def add(a: int, b: int) -> int:
    return a + b

# Types composés
def process_items(items: List[str]) -> Dict[str, int]:
    return {item: len(item) for item in items}

def get_coordinates() -> Tuple[float, float]:
    return (10.5, 20.3)

# Optional (peut être None)
def find_user(user_id: int) -> Optional[str]:
    if user_id == 1:
        return "Alice"
    return None

# Union (plusieurs types possibles)
def process_value(value: Union[int, str]) -> str:
    return str(value)

# Callable (fonction)
def execute(func: Callable[[int, int], int], a: int, b: int) -> int:
    return func(a, b)

# Any (n'importe quel type)
def flexible_function(param: Any) -> Any:
    return param

# Variables
count: int = 0
name: str = "Alice"
items: List[str] = []

# Annotations de classe
class Person:
    name: str
    age: int
    
    def __init__(self, name: str, age: int) -> None:
        self.name = name
        self.age = age

# Génériques (Python 3.9+)
def first_element(items: list[str]) -> str:
    return items[0]

# Avant Python 3.9
from typing import List
def first_element(items: List[str]) -> str:
    return items[0]

# Type aliases
from typing import List, Dict

UserId = int
UserData = Dict[str, Union[str, int]]
UserList = List[UserData]

def get_users() -> UserList:
    return [
        {'name': 'Alice', 'age': 30},
        {'name': 'Bob', 'age': 25}
    ]

# Vérification des types avec mypy
# Installation: pip install mypy
# Usage: mypy script.py
```

---

## 8.4 Testing

### unittest - Framework standard

```python
import unittest

# Code à tester
def add(a, b):
    return a + b

def divide(a, b):
    if b == 0:
        raise ValueError("Division par zéro")
    return a / b

# Tests
class TestMathFunctions(unittest.TestCase):
    """Tests pour les fonctions mathématiques."""
    
    def setUp(self):
        """Appelé avant chaque test."""
        self.numbers = [1, 2, 3, 4, 5]
    
    def tearDown(self):
        """Appelé après chaque test."""
        pass
    
    def test_add_positive_numbers(self):
        """Test addition de nombres positifs."""
        result = add(2, 3)
        self.assertEqual(result, 5)
    
    def test_add_negative_numbers(self):
        """Test addition de nombres négatifs."""
        result = add(-2, -3)
        self.assertEqual(result, -5)
    
    def test_divide_normal(self):
        """Test division normale."""
        result = divide(10, 2)
        self.assertEqual(result, 5)
    
    def test_divide_by_zero(self):
        """Test division par zéro."""
        with self.assertRaises(ValueError):
            divide(10, 0)
    
    def test_divide_result_type(self):
        """Test type du résultat."""
        result = divide(10, 3)
        self.assertIsInstance(result, float)

# Assertions courantes
class TestAssertions(unittest.TestCase):
    
    def test_equality(self):
        self.assertEqual(1 + 1, 2)
        self.assertNotEqual(1 + 1, 3)
    
    def test_boolean(self):
        self.assertTrue(True)
        self.assertFalse(False)
    
    def test_none(self):
        value = None
        self.assertIsNone(value)
        self.assertIsNotNone(42)
    
    def test_membership(self):
        self.assertIn(3, [1, 2, 3])
        self.assertNotIn(4, [1, 2, 3])
    
    def test_instance(self):
        self.assertIsInstance("hello", str)
        self.assertNotIsInstance("hello", int)
    
    def test_comparison(self):
        self.assertGreater(5, 3)
        self.assertLess(3, 5)
        self.assertGreaterEqual(5, 5)
        self.assertLessEqual(3, 5)
    
    def test_almost_equal(self):
        # Utile pour les floats
        self.assertAlmostEqual(0.1 + 0.2, 0.3, places=7)

# Lancer les tests
if __name__ == '__main__':
    unittest.main()
```

### pytest - Framework moderne

```python
# Installation: pip install pytest
# Usage: pytest test_module.py

# Code à tester
def add(a, b):
    return a + b

def divide(a, b):
    if b == 0:
        raise ValueError("Division par zéro")
    return a / b

# Tests (pas besoin de classe)
def test_add_positive():
    assert add(2, 3) == 5

def test_add_negative():
    assert add(-2, -3) == -5

def test_divide_normal():
    assert divide(10, 2) == 5

def test_divide_by_zero():
    import pytest
    with pytest.raises(ValueError):
        divide(10, 0)

# Fixtures
import pytest

@pytest.fixture
def sample_data():
    """Fixture qui fournit des données de test."""
    return [1, 2, 3, 4, 5]

def test_with_fixture(sample_data):
    assert len(sample_data) == 5
    assert sum(sample_data) == 15

# Paramétrage de tests
@pytest.mark.parametrize("a,b,expected", [
    (2, 3, 5),
    (0, 5, 5),
    (-2, 2, 0),
    (10, -5, 5),
])
def test_add_parametrized(a, b, expected):
    assert add(a, b) == expected

# Marqueurs
@pytest.mark.slow
def test_slow_operation():
    # Test long
    pass

@pytest.mark.skip(reason="Pas encore implémenté")
def test_future_feature():
    pass

@pytest.mark.skipif(sys.platform == 'win32', reason="Ne fonctionne pas sur Windows")
def test_unix_only():
    pass

# Vérifier les exceptions avec le message
def test_divide_by_zero_message():
    with pytest.raises(ValueError, match="Division par zéro"):
        divide(10, 0)

# Setup et teardown
@pytest.fixture
def resource():
    # Setup
    r = setup_resource()
    yield r
    # Teardown
    cleanup_resource(r)
```

### Doctest - Tests dans la documentation

```python
def add(a, b):
    """
    Additionne deux nombres.
    
    Examples:
        >>> add(2, 3)
        5
        >>> add(-1, 1)
        0
        >>> add(0, 0)
        0
    """
    return a + b

def factorial(n):
    """
    Calcule la factorielle de n.
    
    Examples:
        >>> factorial(0)
        1
        >>> factorial(1)
        1
        >>> factorial(5)
        120
        >>> factorial(-1)
        Traceback (most recent call last):
            ...
        ValueError: n doit être positif
    """
    if n < 0:
        raise ValueError("n doit être positif")
    if n == 0:
        return 1
    return n * factorial(n - 1)

# Lancer les doctests
if __name__ == '__main__':
    import doctest
    doctest.testmod()

# Ou en ligne de commande:
# python -m doctest module.py
# python -m doctest module.py -v  (verbeux)
```

### Coverage - Couverture de code

```bash
# Installation
pip install coverage

# Lancer les tests avec coverage
coverage run -m pytest

# Rapport dans le terminal
coverage report

# Rapport HTML
coverage html
# Ouvre htmlcov/index.html
```

---

## 8.5 Debugging

### print debugging (basique)

```python
def complex_function(data):
    print(f"DEBUG: data = {data}")  # Point de debug
    
    result = process(data)
    print(f"DEBUG: result after process = {result}")
    
    final = transform(result)
    print(f"DEBUG: final result = {final}")
    
    return final

# Mieux: utiliser logging
import logging
logging.basicConfig(level=logging.DEBUG)

def complex_function(data):
    logging.debug(f"data = {data}")
    
    result = process(data)
    logging.debug(f"result after process = {result}")
    
    final = transform(result)
    logging.debug(f"final result = {final}")
    
    return final
```

### pdb - Python Debugger

```python
import pdb

def buggy_function(x, y):
    result = x + y
    pdb.set_trace()  # Point d'arrêt (breakpoint)
    # Le programme s'arrête ici et lance le debugger interactif
    return result * 2

# Python 3.7+ : utiliser breakpoint()
def buggy_function(x, y):
    result = x + y
    breakpoint()  # Plus moderne
    return result * 2

# Commandes pdb courantes:
# n (next)      : ligne suivante
# s (step)      : entre dans la fonction
# c (continue)  : continue jusqu'au prochain breakpoint
# l (list)      : affiche le code autour
# p variable    : affiche la valeur d'une variable
# pp variable   : pretty-print
# w (where)     : affiche la stack trace
# u (up)        : remonte dans la stack
# d (down)      : descend dans la stack
# q (quit)      : quitte le debugger
# h (help)      : aide

# Post-mortem debugging
import pdb
import sys

def main():
    try:
        buggy_code()
    except Exception:
        pdb.post_mortem(sys.exc_info()[2])

# Ou automatiquement sur toute exception
import sys
import pdb

def excepthook(type, value, tb):
    pdb.post_mortem(tb)

sys.excepthook = excepthook
```

### Assertions

```python
def divide(a, b):
    # Assertion pour vérifier les préconditions
    assert b != 0, "Le diviseur ne peut pas être zéro"
    return a / b

# ATTENTION: Les assertions peuvent être désactivées avec -O
# python -O script.py
# N'utilisez PAS les assertions pour la validation utilisateur

# ✅ Pour validation utilisateur
def divide(a, b):
    if b == 0:
        raise ValueError("Le diviseur ne peut pas être zéro")
    return a / b

# ✅ Assertions pour les invariants (bugs internes)
def process_list(items):
    assert isinstance(items, list), "items doit être une liste"
    assert len(items) > 0, "items ne peut pas être vide"
    # ... traitement
```

### Logging pour debugging

```python
import logging

# Configuration
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s [%(levelname)s] %(name)s: %(message)s',
    handlers=[
        logging.FileHandler('debug.log'),
        logging.StreamHandler()
    ]
)

logger = logging.getLogger(__name__)

def complex_operation(data):
    logger.debug(f"Début de complex_operation avec {data}")
    
    try:
        result = step1(data)
        logger.debug(f"step1 terminé: {result}")
        
        result = step2(result)
        logger.debug(f"step2 terminé: {result}")
        
        return result
    except Exception as e:
        logger.exception("Erreur dans complex_operation")
        raise

# Différents niveaux selon le contexte
logger.debug("Info détaillée pour debugging")
logger.info("Information générale")
logger.warning("Avertissement")
logger.error("Erreur")
logger.critical("Erreur critique")
```

### traceback - Analyser les exceptions

```python
import traceback
import sys

try:
    buggy_function()
except Exception:
    # Afficher la traceback complète
    traceback.print_exc()
    
    # Obtenir la traceback comme string
    tb_str = traceback.format_exc()
    print(tb_str)
    
    # Obtenir les détails
    exc_type, exc_value, exc_tb = sys.exc_info()
    
    # Extraire les informations
    for line in traceback.format_tb(exc_tb):
        print(line)
```

### Outils de profiling

```python
# timeit - Mesurer le temps d'exécution
import timeit

# Mesurer une expression
time = timeit.timeit('sum(range(100))', number=10000)
print(f"Temps: {time}")

# Mesurer une fonction
def my_function():
    return sum(range(100))

time = timeit.timeit(my_function, number=10000)

# cProfile - Profiler un programme
import cProfile
import pstats

cProfile.run('my_function()', 'output.stats')

# Analyser les stats
stats = pstats.Stats('output.stats')
stats.strip_dirs()
stats.sort_stats('cumulative')
stats.print_stats(10)  # Top 10

# memory_profiler - Analyser l'utilisation mémoire
# pip install memory_profiler

from memory_profiler import profile

@profile
def memory_intensive():
    big_list = [0] * (10**6)
    return sum(big_list)

memory_intensive()
```

---

## 🎯 Points clés à retenir

### PEP 8
- **snake_case** pour fonctions/variables
- **PascalCase** pour classes
- **UPPER_CASE** pour constantes
- 4 espaces d'indentation
- Max 79 caractères par ligne
- 2 lignes vides entre fonctions/classes

### Code pythonique
- Compréhensions au lieu de boucles
- `enumerate()` au lieu de `range(len())`
- `zip()` pour itérer en parallèle
- F-strings pour formatage
- Context managers (`with`)
- `any()`/`all()` pour conditions

### Documentation
- **Docstrings** pour modules, classes, fonctions
- **Type hints** pour clarté
- Exemples dans les docstrings
- Style Google ou NumPy

### Testing
- **unittest** : framework standard
- **pytest** : moderne, plus simple
- **doctest** : tests dans la doc
- Coverage pour mesurer les tests

### Debugging
- **logging** au lieu de print
- **pdb/breakpoint()** pour debugging interactif
- **Assertions** pour invariants internes
- **traceback** pour analyser les erreurs

---

## 💡 Questions d'entretien courantes (Chapitre 8)

1. **Qu'est-ce que PEP 8 ?**
   - Guide de style officiel pour Python
   - Conventions de nommage, formatage, organisation
   - Améliore lisibilité et cohérence

2. **Différence entre assertion et exception ?**
   - Assertion : vérifier invariants internes (bugs)
   - Exception : gérer erreurs d'exécution
   - Assertions peuvent être désactivées (-O)

3. **Pourquoi utiliser type hints ?**
   - Documentation du code
   - Détection d'erreurs avec mypy
   - Meilleur support IDE
   - Pas d'impact runtime

4. **unittest vs pytest ?**
   - unittest : standard library, classes, verbose
   - pytest : plus simple, fixtures, parametrize
   - pytest recommandé pour nouveaux projets

5. **Comment debugger efficacement ?**
   - logging pour traces
   - pdb/breakpoint() pour inspection
   - Tests unitaires pour isoler
   - traceback pour comprendre erreurs

6. **Qu'est-ce qu'un code pythonique ?**
   - Utilise les idiomes Python
   - Clair et expressif
   - Suit les conventions
   - Utilise les fonctionnalités du langage

---

## 🔧 Exercices pratiques

1. **Refactoring PEP 8**
   - Prenez du code non conforme
   - Appliquez toutes les règles PEP 8
   - Utilisez flake8 ou pylint pour vérifier

2. **Pythonification**
   - Réécrivez du code non pythonique
   - Utilisez compréhensions, enumerate, zip
   - Comparez performance et lisibilité

3. **Documentation complète**
   - Documentez un module avec docstrings
   - Ajoutez type hints partout
   - Incluez des exemples doctests

4. **Suite de tests**
   - Écrivez tests pour un module
   - Atteignez 100% de coverage
   - Utilisez pytest avec fixtures

5. **Debugging challenge**
   - Code avec bugs intentionnels
   - Utilisez pdb pour les trouver
   - Ajoutez logging approprié

---

**Prochaine étape :** Chapitre 10 - Questions d'Entretien Courantes (parfait pour réviser avant l'entretien !)
