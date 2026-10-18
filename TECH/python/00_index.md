# Programme de Préparation Python - Entretien Technique

## 📋 Vue d'ensemble
Ce programme couvre les compétences Python essentielles pour un entretien technique, avec un focus sur le scripting et Python général (non-applicatif).

---

## 📚 Chapitres

### [**Chapitre 1 : Fondamentaux Python**](./01_FONDAMENTAUX_PYTHON.md)
- 1.1 Types de données et structures
  - Types primitifs (int, float, str, bool)
  - Collections (list, tuple, dict, set)
  - Compréhensions de listes/dictionnaires
- 1.2 Opérateurs et expressions
  - Opérateurs arithmétiques, logiques, de comparaison
  - Opérateur walrus (:=)
  - Opérateurs d'appartenance et d'identité
- 1.3 Structures de contrôle
  - if/elif/else
  - Boucles for et while
  - break, continue, pass
- 1.4 Fonctions
  - Définition et appel
  - Arguments (*args, **kwargs)
  - Fonctions lambda
  - Décorateurs de base

### [**Chapitre 2 : Programmation Orientée Objet (POO)**](./02_POO.md)
- 2.1 Classes et objets
  - Définition de classes
  - Constructeur __init__
  - Attributs d'instance vs de classe
- 2.2 Méthodes spéciales (dunder methods)
  - __str__, __repr__
  - __len__, __getitem__
  - Opérateurs surchargés
- 2.3 Héritage et polymorphisme
  - Héritage simple et multiple
  - super()
  - Méthodes abstraites
- 2.4 Encapsulation
  - Attributs privés (_private, __private)
  - Properties et getters/setters

### [**Chapitre 3 : Manipulation de Fichiers et I/O**](./03_FICHIERS_IO.md)
- 3.1 Lecture/Écriture de fichiers
  - Context manager (with)
  - Modes d'ouverture (r, w, a, b)
  - Gestion des encodages
- 3.2 Formats de données
  - JSON (json module)
  - CSV (csv module)
  - Pickle
- 3.3 Gestion des chemins
  - pathlib vs os.path
  - Manipulation de répertoires

### [**Chapitre 4 : Gestion des Erreurs et Exceptions**](./04_EXCEPTIONS.md)
- 4.1 Try/Except/Finally
  - Capture d'exceptions spécifiques
  - Bloc else
  - Finally pour le nettoyage
- 4.2 Création d'exceptions personnalisées
- 4.3 Bonnes pratiques
  - Quand lever une exception
  - Logging vs print

### [**Chapitre 5 : Modules et Packages**](./05_MODULES_PACKAGES.md)
- 5.1 Import de modules
  - import, from...import
  - Alias (as)
  - Import relatifs vs absolus
- 5.2 Création de modules
  - __name__ == "__main__"
  - Structure d'un package
- 5.3 Modules standard essentiels
  - os, sys
  - datetime, time
  - re (expressions régulières)
  - collections (defaultdict, Counter, namedtuple)
  - itertools

### [**Chapitre 6 : Scripting et Ligne de Commande**](./06__SCRIPTING_CLI.md)
- 6.1 Arguments en ligne de commande
  - sys.argv
  - argparse
  - Click (optionnel)
- 6.2 Variables d'environnement
  - os.environ
  - dotenv
- 6.3 Exécution de commandes système
  - subprocess module
  - os.system vs subprocess
- 6.4 Scripts utilitaires courants
  - Traitement de logs
  - Manipulation de fichiers en masse
  - Automatisation de tâches

### [**Chapitre 7 : Fonctionnalités Avancées**](./07_FONCTIONNALITES_AVANCEES.md)
- 7.1 Itérateurs et générateurs
  - yield
  - Expressions génératrices
  - itertools
- 7.2 Décorateurs avancés
  - Décorateurs avec arguments
  - functools.wraps
  - Cas d'usage pratiques
- 7.3 Context managers
  - Protocole __enter__/__exit__
  - contextlib
- 7.4 Programmation fonctionnelle
  - map, filter, reduce
  - Fonctions de première classe
  - Closures

### [**Chapitre 8 : Bonnes Pratiques et Style**](./08_BONNES_PRATIQUES.md)
- 8.1 PEP 8 - Style de code
  - Conventions de nommage
  - Indentation et espacement
  - Longueur des lignes
- 8.2 Documentation
  - Docstrings
  - Type hints
  - Comments utiles
- 8.3 Testing
  - unittest
  - pytest (basique)
  - Assertions
- 8.4 Debugging
  - pdb
  - print debugging
  - Logging

### [**Chapitre 9 : Performance et Optimisation**](./09_PERFORMANCE.md)
- 9.1 Mesure de performance
  - timeit
  - Profiling
- 9.2 Optimisations courantes
  - Compréhensions vs boucles
  - Générateurs vs listes
  - Cache et memoization
- 9.3 Complexité algorithmique (Big O)
  - Notions de base
  - Structures de données et complexité

### [**Chapitre 10 : Questions d'Entretien Courantes**](./10_QUESTIONS_ENTRETIEN.md)
- 10.1 Questions théoriques
  - Différence list/tuple
  - GIL (Global Interpreter Lock)
  - Mutabilité vs Immutabilité
- 10.2 Exercices de code pratiques
  - Manipulation de strings
  - Problèmes sur les listes
  - Dictionnaires et hashmaps
- 10.3 Questions pièges
  - Default mutable arguments
  - Late binding closures
  - Shallow vs deep copy

---

## 🎯 Comment utiliser ce programme

1. Lisez d'abord ce sommaire pour avoir une vue d'ensemble
2. Demandez-moi de générer le chapitre qui vous intéresse
3. Étudiez chaque chapitre dans l'ordre ou selon vos besoins
4. Pratiquez avec les exemples fournis
5. Revenez sur les chapitres difficiles

---

## ⏱️ Estimation de temps par chapitre

- Chapitres 1-2 : 2-3h chacun (révisions fondamentales)
- Chapitres 3-6 : 1-2h chacun (pratique et scripting)
- Chapitres 7-9 : 2h chacun (concepts avancés)
- Chapitre 10 : 3-4h (préparation intensive)

**Total estimé : 20-25h de préparation**

---