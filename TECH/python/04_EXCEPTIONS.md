## [**..**](./00_index.md)

# Chapitre 4 : Gestion des Erreurs et Exceptions

## 🎯 Objectifs du chapitre
- Comprendre le mécanisme des exceptions en Python
- Utiliser try/except/finally/else efficacement
- Créer des exceptions personnalisées
- Appliquer les bonnes pratiques de gestion d'erreurs
- Distinguer logging et print pour le debugging

---

## 4.1 Try/Except/Finally

### Syntaxe de base

```python
# Sans gestion d'erreur
number = int("abc")  # ValueError: invalid literal for int()
# Le programme crash

# Avec gestion d'erreur
try:
    number = int("abc")
    print("Conversion réussie")
except ValueError:
    print("Erreur: impossible de convertir en entier")
    number = 0

print(f"Résultat: {number}")  # Le programme continue
```

### Capturer différents types d'exceptions

```python
# Capture spécifique
try:
    file = open('inexistant.txt', 'r')
    content = file.read()
except FileNotFoundError:
    print("Le fichier n'existe pas")

# Capturer plusieurs exceptions (tuple)
try:
    result = 10 / 0
    number = int("abc")
except (ZeroDivisionError, ValueError):
    print("Erreur mathématique ou de conversion")

# Capturer plusieurs exceptions séparément
try:
    result = 10 / 0
except ZeroDivisionError:
    print("Division par zéro")
except ValueError:
    print("Valeur invalide")
except Exception as e:
    print(f"Autre erreur: {e}")

# Obtenir les détails de l'exception
try:
    number = int("abc")
except ValueError as e:
    print(f"Erreur de type: {type(e).__name__}")
    print(f"Message: {e}")
    print(f"Args: {e.args}")
```

### Bloc else - Exécuté si pas d'exception

```python
try:
    number = int("42")
except ValueError:
    print("Conversion impossible")
else:
    print("Conversion réussie!")  # S'exécute seulement si try réussit
    print(f"Nombre: {number}")

# Exemple pratique
def read_config(filename):
    try:
        with open(filename, 'r') as f:
            config = f.read()
    except FileNotFoundError:
        print(f"Fichier {filename} introuvable")
        return None
    else:
        print("Configuration chargée avec succès")
        return config
    finally:
        print("Tentative de lecture terminée")
```

### Bloc finally - Toujours exécuté

```python
# finally s'exécute TOUJOURS
try:
    file = open('data.txt', 'r')
    data = file.read()
    result = int(data)
except FileNotFoundError:
    print("Fichier introuvable")
except ValueError:
    print("Données invalides")
finally:
    print("Nettoyage...")
    # S'exécute même s'il y a une exception
    # Utilisé pour libérer des ressources
    try:
        file.close()
    except:
        pass

# Exemple : connexion base de données
import sqlite3

def fetch_data():
    conn = None
    try:
        conn = sqlite3.connect('database.db')
        cursor = conn.cursor()
        cursor.execute('SELECT * FROM users')
        return cursor.fetchall()
    except sqlite3.Error as e:
        print(f"Erreur BDD: {e}")
        return None
    finally:
        if conn:
            conn.close()  # Toujours fermée
            print("Connexion fermée")
```

### Ordre complet : try/except/else/finally

```python
def divide_numbers(a, b):
    try:
        result = a / b
    except ZeroDivisionError:
        print("Erreur: division par zéro")
        return None
    except TypeError:
        print("Erreur: types invalides")
        return None
    else:
        print("Division réussie")
        return result
    finally:
        print("Opération terminée")

# Tests
print(divide_numbers(10, 2))   # Division réussie, Opération terminée, 5.0
print(divide_numbers(10, 0))   # Erreur: division par zéro, Opération terminée, None
print(divide_numbers(10, "a")) # Erreur: types invalides, Opération terminée, None
```

### Capture de toutes les exceptions (à éviter)

```python
# MAUVAISE PRATIQUE : trop générique
try:
    # code
    pass
except:  # Capture TOUT, même KeyboardInterrupt
    print("Une erreur s'est produite")

# MEILLEURE PRATIQUE : Exception de base
try:
    # code
    pass
except Exception as e:  # Capture les erreurs, pas les interruptions système
    print(f"Erreur: {e}")

# ENCORE MIEUX : Exceptions spécifiques
try:
    # code
    pass
except (ValueError, TypeError, KeyError) as e:
    print(f"Erreur attendue: {e}")
```

### Exceptions imbriquées

```python
try:
    try:
        number = int("abc")
    except ValueError:
        print("Erreur interne capturée")
        raise  # Relance l'exception
except ValueError:
    print("Erreur externe capturée")

# Chaîner les exceptions (Python 3)
try:
    value = int("abc")
except ValueError as e:
    raise RuntimeError("Erreur de traitement") from e
    # Préserve l'exception d'origine
```

---

## 4.2 Lever des exceptions (raise)

### Lever une exception

```python
# Lever une exception existante
def divide(a, b):
    if b == 0:
        raise ValueError("Le diviseur ne peut pas être zéro")
    return a / b

try:
    result = divide(10, 0)
except ValueError as e:
    print(f"Erreur: {e}")

# Lever sans argument (dans un except)
def process_data(data):
    try:
        result = int(data)
    except ValueError:
        print("Log: conversion impossible")
        raise  # Relance l'exception originale

try:
    process_data("abc")
except ValueError as e:
    print(f"Capturé: {e}")
```

### Exceptions standard courantes

```python
# ValueError - valeur inappropriée
def set_age(age):
    if age < 0 or age > 150:
        raise ValueError("L'âge doit être entre 0 et 150")
    return age

# TypeError - type inapproprié
def add_numbers(a, b):
    if not isinstance(a, (int, float)) or not isinstance(b, (int, float)):
        raise TypeError("Les arguments doivent être des nombres")
    return a + b

# KeyError - clé inexistante
data = {'name': 'Alice'}
try:
    age = data['age']
except KeyError:
    print("Clé 'age' introuvable")

# IndexError - index hors limites
lst = [1, 2, 3]
try:
    item = lst[10]
except IndexError:
    print("Index hors limites")

# AttributeError - attribut inexistant
class Person:
    def __init__(self, name):
        self.name = name

person = Person("Alice")
try:
    city = person.city
except AttributeError:
    print("Attribut 'city' inexistant")

# FileNotFoundError - fichier introuvable
try:
    with open('inexistant.txt', 'r') as f:
        content = f.read()
except FileNotFoundError:
    print("Fichier introuvable")

# PermissionError - permissions insuffisantes
try:
    with open('/root/file.txt', 'w') as f:
        f.write("data")
except PermissionError:
    print("Permissions insuffisantes")

# RuntimeError - erreur générique
def check_state(ready):
    if not ready:
        raise RuntimeError("Le système n'est pas prêt")
```

### Hiérarchie des exceptions

```python
"""
BaseException
├── SystemExit
├── KeyboardInterrupt
├── GeneratorExit
└── Exception
    ├── StopIteration
    ├── ArithmeticError
    │   ├── ZeroDivisionError
    │   ├── OverflowError
    │   └── FloatingPointError
    ├── AssertionError
    ├── AttributeError
    ├── EOFError
    ├── ImportError
    ├── LookupError
    │   ├── IndexError
    │   └── KeyError
    ├── MemoryError
    ├── NameError
    ├── OSError
    │   ├── FileNotFoundError
    │   ├── PermissionError
    │   └── TimeoutError
    ├── RuntimeError
    ├── TypeError
    ├── ValueError
    └── ...
"""

# Capturer une classe parente capture aussi les enfants
try:
    result = 10 / 0
except ArithmeticError:  # Capture aussi ZeroDivisionError
    print("Erreur arithmétique")

# Ordre des except : du plus spécifique au plus général
try:
    data = [1, 2, 3]
    print(data[10])
except IndexError:     # Spécifique
    print("Index invalide")
except LookupError:    # Parent
    print("Erreur de lookup")
except Exception:      # Très général
    print("Autre erreur")
```

---

## 4.3 Exceptions personnalisées

### Créer une exception simple

```python
# Exception personnalisée de base
class CustomError(Exception):
    pass

# Utilisation
def check_value(value):
    if value < 0:
        raise CustomError("La valeur ne peut pas être négative")

try:
    check_value(-5)
except CustomError as e:
    print(f"Erreur: {e}")
```

### Exception avec attributs

```python
class ValidationError(Exception):
    def __init__(self, field, message):
        self.field = field
        self.message = message
        super().__init__(f"{field}: {message}")

def validate_user(name, age):
    if not name:
        raise ValidationError("name", "Le nom est requis")
    if age < 0:
        raise ValidationError("age", "L'âge ne peut pas être négatif")

try:
    validate_user("", 25)
except ValidationError as e:
    print(f"Champ invalide: {e.field}")
    print(f"Message: {e.message}")
```

### Hiérarchie d'exceptions personnalisées

```python
# Exception de base pour l'application
class AppError(Exception):
    """Exception de base pour l'application"""
    pass

# Exceptions spécifiques
class DatabaseError(AppError):
    """Erreurs liées à la base de données"""
    pass

class AuthenticationError(AppError):
    """Erreurs d'authentification"""
    pass

class PermissionError(AppError):
    """Erreurs de permissions"""
    pass

# Utilisation
def connect_database():
    raise DatabaseError("Impossible de se connecter à la BDD")

def login(username, password):
    if not username or not password:
        raise AuthenticationError("Identifiants invalides")
    # ...

def access_resource(user, resource):
    if not user.has_permission(resource):
        raise PermissionError("Accès refusé")
    # ...

# Capture par hiérarchie
try:
    connect_database()
except DatabaseError:
    print("Erreur BDD")
except AppError:
    print("Autre erreur application")
```

### Exemple complet : Système de validation

```python
class ValidationError(Exception):
    """Exception de base pour les erreurs de validation"""
    def __init__(self, field, value, message):
        self.field = field
        self.value = value
        self.message = message
        super().__init__(f"Validation failed for '{field}': {message}")

class RequiredFieldError(ValidationError):
    """Champ requis manquant"""
    def __init__(self, field):
        super().__init__(field, None, "This field is required")

class InvalidTypeError(ValidationError):
    """Type invalide"""
    def __init__(self, field, value, expected_type):
        message = f"Expected {expected_type}, got {type(value).__name__}"
        super().__init__(field, value, message)
        self.expected_type = expected_type

class RangeError(ValidationError):
    """Valeur hors limites"""
    def __init__(self, field, value, min_val, max_val):
        message = f"Value must be between {min_val} and {max_val}"
        super().__init__(field, value, message)
        self.min = min_val
        self.max = max_val

# Validateur
class Validator:
    @staticmethod
    def validate_string(field, value, required=True, min_length=None, max_length=None):
        if value is None or value == "":
            if required:
                raise RequiredFieldError(field)
            return
        
        if not isinstance(value, str):
            raise InvalidTypeError(field, value, "str")
        
        if min_length and len(value) < min_length:
            raise RangeError(field, len(value), min_length, max_length or float('inf'))
        
        if max_length and len(value) > max_length:
            raise RangeError(field, len(value), min_length or 0, max_length)
    
    @staticmethod
    def validate_int(field, value, required=True, min_val=None, max_val=None):
        if value is None:
            if required:
                raise RequiredFieldError(field)
            return
        
        if not isinstance(value, int):
            raise InvalidTypeError(field, value, "int")
        
        if min_val is not None and value < min_val:
            raise RangeError(field, value, min_val, max_val or float('inf'))
        
        if max_val is not None and value > max_val:
            raise RangeError(field, value, min_val or float('-inf'), max_val)

# Utilisation
def create_user(username, age):
    try:
        Validator.validate_string("username", username, min_length=3, max_length=20)
        Validator.validate_int("age", age, min_val=0, max_val=150)
        return {"username": username, "age": age}
    except RequiredFieldError as e:
        print(f"❌ Champ requis: {e.field}")
    except InvalidTypeError as e:
        print(f"❌ Type invalide: {e.field} (attendu: {e.expected_type})")
    except RangeError as e:
        print(f"❌ Valeur hors limites: {e.field} (min: {e.min}, max: {e.max})")
    except ValidationError as e:
        print(f"❌ Erreur de validation: {e}")

# Tests
create_user("Al", 25)        # ❌ Champ trop court
create_user("Alice", 200)    # ❌ Âge hors limites
create_user("Alice", "25")   # ❌ Type invalide
create_user("Alice", 25)     # ✅ OK
```

---

## 4.4 Bonnes pratiques

### Être spécifique dans la capture

```python
# ❌ MAUVAIS : trop général
try:
    value = int(user_input)
except:
    print("Erreur")

# ✅ BON : exceptions spécifiques
try:
    value = int(user_input)
except ValueError:
    print("Entrée invalide")
except Exception as e:
    print(f"Erreur inattendue: {e}")

# ❌ MAUVAIS : capture aveugle
try:
    process_data()
except Exception:
    pass  # Ignore silencieusement

# ✅ BON : log et potentiellement relance
import logging
try:
    process_data()
except Exception as e:
    logging.error(f"Erreur lors du traitement: {e}", exc_info=True)
    raise
```

### Ne pas capturer trop large

```python
# ❌ MAUVAIS : capture SystemExit, KeyboardInterrupt, etc.
try:
    main()
except:
    cleanup()

# ✅ BON : laisse passer les interruptions système
try:
    main()
except Exception:
    cleanup()
    raise
finally:
    # Ou utilisez finally pour le nettoyage
    cleanup()
```

### Éviter les exceptions comme contrôle de flux

```python
# ❌ MAUVAIS : utiliser les exceptions pour la logique
def find_index(lst, value):
    try:
        return lst.index(value)
    except ValueError:
        return -1

# ✅ BON : vérifier avant
def find_index(lst, value):
    if value in lst:
        return lst.index(value)
    return -1

# Exception : EAFP (Easier to Ask Forgiveness than Permission)
# Parfois les exceptions sont idiomatiques en Python

# ✅ Style pythonique (EAFP)
try:
    value = my_dict['key']
except KeyError:
    value = default

# vs LBYL (Look Before You Leap)
if 'key' in my_dict:
    value = my_dict['key']
else:
    value = default

# Pour les fichiers, EAFP est souvent meilleur
try:
    with open('file.txt') as f:
        content = f.read()
except FileNotFoundError:
    content = ""
```

### Fournir des messages d'erreur utiles

```python
# ❌ MAUVAIS : message vague
def withdraw(account, amount):
    if amount > account.balance:
        raise ValueError("Erreur")

# ✅ BON : message descriptif
def withdraw(account, amount):
    if amount > account.balance:
        raise ValueError(
            f"Solde insuffisant: tentative de retrait de {amount}€ "
            f"avec un solde de {account.balance}€"
        )

# ✅ BON : avec contexte
def process_file(filename):
    try:
        with open(filename) as f:
            data = f.read()
    except FileNotFoundError:
        raise FileNotFoundError(
            f"Le fichier '{filename}' est introuvable. "
            f"Vérifiez le chemin et réessayez."
        )
```

### Nettoyer les ressources

```python
# ❌ MAUVAIS : ressources non libérées
def read_file(filename):
    file = open(filename)
    data = file.read()
    return data  # file jamais fermé

# ✅ BON : avec context manager
def read_file(filename):
    with open(filename) as file:
        return file.read()  # automatiquement fermé

# ✅ BON : avec try/finally
def read_file(filename):
    file = open(filename)
    try:
        return file.read()
    finally:
        file.close()
```

### Assertions vs Exceptions

```python
# Assertions : pour vérifier des invariants (bugs internes)
def calculate_average(numbers):
    assert len(numbers) > 0, "La liste ne doit pas être vide"
    return sum(numbers) / len(numbers)

# ATTENTION : les assertions peuvent être désactivées avec -O
# N'utilisez PAS les assertions pour la validation utilisateur

# ✅ BON : exceptions pour validation utilisateur
def calculate_average(numbers):
    if not numbers:
        raise ValueError("La liste ne doit pas être vide")
    return sum(numbers) / len(numbers)

# Règle : 
# - Assertions : erreurs de programmation (ne devraient jamais arriver)
# - Exceptions : erreurs d'exécution (peuvent arriver normalement)
```

---

## 4.5 Logging vs Print

### Pourquoi utiliser logging ?

```python
import logging

# Configuration basique
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)

# Niveaux de log (du moins au plus sévère)
logging.debug("Message de debug")      # Informations détaillées
logging.info("Message d'info")         # Informations générales
logging.warning("Message d'avertissement")  # Avertissements
logging.error("Message d'erreur")      # Erreurs
logging.critical("Message critique")   # Erreurs critiques

# vs print
print("Message")  # ❌ Pas de niveau, pas de timestamp, pas de contrôle
```

### Configuration avancée

```python
import logging

# Configuration avec fichier
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(name)s: %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S',
    handlers=[
        logging.FileHandler('app.log'),      # Log dans un fichier
        logging.StreamHandler()               # Log dans la console
    ]
)

logger = logging.getLogger(__name__)

# Utilisation
logger.info("Application démarrée")
logger.warning("Ressource presque saturée")
logger.error("Erreur lors du traitement")
```

### Logging dans les exceptions

```python
import logging

logger = logging.getLogger(__name__)

def process_data(data):
    try:
        result = int(data) / 10
        logger.info(f"Traitement réussi: {result}")
        return result
    except ValueError as e:
        logger.error(f"Erreur de conversion: {e}")
        return None
    except ZeroDivisionError as e:
        logger.error(f"Division par zéro: {e}")
        return None
    except Exception as e:
        logger.critical(f"Erreur inattendue: {e}", exc_info=True)
        # exc_info=True inclut la traceback complète
        raise

# Test
process_data("abc")  # Log l'erreur
```

### Logger par module

```python
# module1.py
import logging
logger = logging.getLogger(__name__)  # __name__ = 'module1'

def function1():
    logger.info("Function1 appelée")

# module2.py
import logging
logger = logging.getLogger(__name__)  # __name__ = 'module2'

def function2():
    logger.info("Function2 appelée")

# main.py
import logging
logging.basicConfig(level=logging.INFO)

import module1
import module2

module1.function1()  # [INFO] module1: Function1 appelée
module2.function2()  # [INFO] module2: Function2 appelée
```

### Rotation des logs

```python
from logging.handlers import RotatingFileHandler
import logging

# Rotation par taille
handler = RotatingFileHandler(
    'app.log',
    maxBytes=10*1024*1024,  # 10 MB
    backupCount=5            # Garde 5 fichiers
)
formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)

logger = logging.getLogger(__name__)
logger.addHandler(handler)
logger.setLevel(logging.INFO)

# Rotation par temps
from logging.handlers import TimedRotatingFileHandler

handler = TimedRotatingFileHandler(
    'app.log',
    when='midnight',  # Rotation à minuit
    interval=1,       # Chaque jour
    backupCount=7     # Garde 7 jours
)
```

### Exemple complet avec logging

```python
import logging
from pathlib import Path

def setup_logging(log_file='app.log', level=logging.INFO):
    """Configure le système de logging"""
    
    # Créer le répertoire logs si nécessaire
    log_path = Path(log_file)
    log_path.parent.mkdir(parents=True, exist_ok=True)
    
    # Format des logs
    log_format = '%(asctime)s [%(levelname)-8s] %(name)s - %(message)s'
    date_format = '%Y-%m-%d %H:%M:%S'
    
    # Configuration
    logging.basicConfig(
        level=level,
        format=log_format,
        datefmt=date_format,
        handlers=[
            logging.FileHandler(log_file, encoding='utf-8'),
            logging.StreamHandler()
        ]
    )

# Module principal
logger = logging.getLogger(__name__)

def main():
    setup_logging(log_file='logs/app.log', level=logging.DEBUG)
    
    logger.info("Application démarrée")
    
    try:
        # Traitement
        logger.debug("Début du traitement")
        result = process_data()
        logger.info(f"Traitement terminé: {result}")
    except Exception as e:
        logger.exception("Erreur fatale")  # Inclut automatiquement la traceback
        return 1
    finally:
        logger.info("Application terminée")
    
    return 0

def process_data():
    logger = logging.getLogger(__name__)
    logger.debug("Chargement des données")
    
    try:
        data = load_data()
        logger.info(f"{len(data)} éléments chargés")
    except FileNotFoundError:
        logger.error("Fichier de données introuvable")
        raise
    
    return data

if __name__ == '__main__':
    exit(main())
```

---

## 🎯 Points clés à retenir

### Gestion des exceptions
- **try/except** : capture les erreurs
- **else** : exécuté si pas d'exception
- **finally** : toujours exécuté (nettoyage)
- **raise** : lever une exception
- Capturer des exceptions **spécifiques**, pas `except:`

### Exceptions personnalisées
- Hériter de `Exception` (ou sous-classe)
- Créer une hiérarchie si nécessaire
- Ajouter des attributs pour le contexte
- Fournir des messages d'erreur clairs

### Bonnes pratiques
- **EAFP** (Easier to Ask Forgiveness than Permission) - style pythonique
- Ne pas ignorer silencieusement les exceptions
- Nettoyer les ressources (context managers)
- Assertions pour bugs internes, exceptions pour validation

### Logging
- Utiliser `logging`, pas `print`
- Niveaux : DEBUG < INFO < WARNING < ERROR < CRITICAL
- Logger par module : `logger = logging.getLogger(__name__)`
- `exc_info=True` ou `.exception()` pour la traceback

---

## 💡 Questions d'entretien courantes (Chapitre 4)

1. **Quelle est la différence entre `except Exception` et `except` ?**
   - `except:` capture TOUT (même SystemExit, KeyboardInterrupt)
   - `except Exception:` capture les erreurs normales, pas les interruptions système
   - Toujours préférer `except Exception:`

2. **À quoi sert le bloc `finally` ?**
   - S'exécute toujours, même en cas d'exception ou de return
   - Utilisé pour libérer des ressources (fermer fichiers, connexions)
   - Équivalent au `__exit__` des context managers

3. **Différence entre EAFP et LBYL ?**
   - **EAFP** (Easier to Ask Forgiveness): essayer puis gérer l'exception
   - **LBYL** (Look Before You Leap): vérifier avant d'agir
   - Python favorise EAFP (plus rapide, évite les race conditions)

4. **Quand créer une exception personnalisée ?**
   - Pour des erreurs spécifiques à votre domaine
   - Pour ajouter du contexte (attributs)
   - Pour créer une hiérarchie d'exceptions
   - Hériter de `Exception` ou d'une sous-classe appropriée

5. **Pourquoi utiliser `logging` plutôt que `print` ?**
   - Niveaux de sévérité (DEBUG, INFO, WARNING, ERROR, CRITICAL)
   - Formatage automatique (timestamp, niveau, module)
   - Redirection flexible (fichier, console, réseau)
   - Possibilité de désactiver certains niveaux

6. **Que fait `raise` sans argument ?**
   - Relance l'exception courante (dans un `except`)
   - Préserve la traceback originale
   - Utile pour logger puis propager l'erreur

---

## 🔧 Exercices pratiques

1. **Validateur de données**
   - Créez des exceptions personnalisées pour différents types d'erreurs
   - Validez un dictionnaire avec des règles
   - Retournez tous les champs invalides

2. **Gestionnaire de fichiers robuste**
   - Lisez un fichier avec gestion complète des erreurs
   - Gérez : FileNotFoundError, PermissionError, UnicodeDecodeError
   - Ajoutez du logging approprié

3. **Calculatrice avec gestion d'erreurs**
   - Opérations +, -, *, /
   - Gérez division par zéro, types invalides
   - Utilisez try/except/else/finally

4. **Retry decorator**
   - Créez un décorateur qui réessaye en cas d'erreur
   - Paramètres : nombre d'essais, délai entre essais
   - Loggez chaque tentative

5. **Config loader avec validation**
   - Chargez un fichier JSON/YAML
   - Validez la structure et les types
   - Levez des exceptions personnalisées si invalide
   - Loggez toutes les opérations

---

**Prochaine étape :** Chapitre 5 - Modules et Packages
