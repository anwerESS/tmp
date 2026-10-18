## [**..**](./00_index.md)

# Chapitre 3 : Manipulation de Fichiers et I/O

## 🎯 Objectifs du chapitre
- Maîtriser la lecture et l'écriture de fichiers
- Utiliser les context managers pour gérer les ressources
- Travailler avec différents formats de données (JSON, CSV, etc.)
- Manipuler les chemins de fichiers de manière portable
- Gérer les encodages correctement

---

## 3.1 Lecture et écriture de fichiers

### Ouverture de fichiers avec open()

```python
# Syntaxe de base
file = open('fichier.txt', 'r')  # Ouvre en lecture
content = file.read()
file.close()  # NE PAS OUBLIER !

# MEILLEURE PRATIQUE : Context Manager (with)
with open('fichier.txt', 'r') as file:
    content = file.read()
# Le fichier est automatiquement fermé, même en cas d'erreur
```

### Modes d'ouverture

```python
# Modes principaux
# 'r'  : lecture (défaut) - erreur si fichier inexistant
# 'w'  : écriture - crée ou écrase le fichier
# 'a'  : ajout (append) - crée ou ajoute à la fin
# 'x'  : création exclusive - erreur si fichier existe
# 'b'  : mode binaire (à combiner : 'rb', 'wb')
# 't'  : mode texte (défaut)
# '+'  : lecture + écriture

# Lecture
with open('data.txt', 'r') as f:
    content = f.read()

# Écriture (écrase le contenu)
with open('output.txt', 'w') as f:
    f.write('Hello World!\n')
    f.write('Deuxième ligne\n')

# Ajout à la fin
with open('log.txt', 'a') as f:
    f.write('Nouvelle entrée de log\n')

# Création exclusive (erreur si existe)
try:
    with open('new_file.txt', 'x') as f:
        f.write('Nouveau fichier')
except FileExistsError:
    print("Le fichier existe déjà!")

# Lecture et écriture
with open('file.txt', 'r+') as f:
    content = f.read()
    f.write('\nNouvelle ligne')
```

### Méthodes de lecture

```python
# read() - lit tout le fichier
with open('data.txt', 'r') as f:
    content = f.read()  # String avec tout le contenu
    print(content)

# read(size) - lit un nombre spécifique d'octets/caractères
with open('data.txt', 'r') as f:
    chunk = f.read(100)  # Lit les 100 premiers caractères
    print(chunk)

# readline() - lit une ligne
with open('data.txt', 'r') as f:
    first_line = f.readline()
    second_line = f.readline()
    print(first_line)
    print(second_line)

# readlines() - lit toutes les lignes dans une liste
with open('data.txt', 'r') as f:
    lines = f.readlines()  # Liste de strings
    for line in lines:
        print(line.strip())  # strip() enlève le \n

# Itération (MEILLEURE PRATIQUE pour gros fichiers)
with open('data.txt', 'r') as f:
    for line in f:  # Lit ligne par ligne, économe en mémoire
        print(line.strip())
```

### Méthodes d'écriture

```python
# write() - écrit une string
with open('output.txt', 'w') as f:
    f.write('Ligne 1\n')
    f.write('Ligne 2\n')

# writelines() - écrit une liste de strings
lines = ['Ligne 1\n', 'Ligne 2\n', 'Ligne 3\n']
with open('output.txt', 'w') as f:
    f.writelines(lines)

# Attention : writelines() n'ajoute PAS de \n automatiquement
lines = ['Ligne 1', 'Ligne 2', 'Ligne 3']
with open('output.txt', 'w') as f:
    f.writelines(line + '\n' for line in lines)

# Ou utiliser write() avec join()
with open('output.txt', 'w') as f:
    f.write('\n'.join(lines))
```

### Position du curseur

```python
with open('data.txt', 'r') as f:
    # tell() - obtenir la position actuelle
    print(f.tell())  # 0 (début du fichier)
    
    # Lire 10 caractères
    f.read(10)
    print(f.tell())  # 10
    
    # seek(offset, whence) - déplacer le curseur
    # whence: 0=début, 1=position actuelle, 2=fin
    f.seek(0)  # Retour au début
    print(f.tell())  # 0
    
    # Aller à la fin
    f.seek(0, 2)
    print(f.tell())  # Taille du fichier
```

### Encodage des fichiers

```python
# Par défaut, Python utilise l'encodage système
# Sur Windows: souvent 'cp1252'
# Sur Linux/Mac: souvent 'utf-8'

# TOUJOURS spécifier l'encodage explicitement
with open('fichier.txt', 'r', encoding='utf-8') as f:
    content = f.read()

# Écriture avec encodage
with open('output.txt', 'w', encoding='utf-8') as f:
    f.write('Caractères spéciaux: é, è, à, ç\n')
    f.write('Emoji: 😀\n')

# Gestion des erreurs d'encodage
# 'strict' : erreur si caractère invalide (défaut)
# 'ignore' : ignore les caractères invalides
# 'replace' : remplace par un caractère de remplacement
# 'backslashreplace' : remplace par une séquence \x...

with open('fichier.txt', 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

# Détection automatique d'encodage (avec chardet)
# pip install chardet
import chardet

with open('fichier.txt', 'rb') as f:
    raw_data = f.read()
    result = chardet.detect(raw_data)
    encoding = result['encoding']
    print(f"Encodage détecté: {encoding}")

# Relire avec le bon encodage
with open('fichier.txt', 'r', encoding=encoding) as f:
    content = f.read()
```

### Fichiers binaires

```python
# Lecture binaire
with open('image.png', 'rb') as f:
    binary_data = f.read()
    print(type(binary_data))  # <class 'bytes'>
    print(len(binary_data))   # Taille en octets

# Écriture binaire
data = b'\x89PNG\r\n\x1a\n'  # Début d'un fichier PNG
with open('test.png', 'wb') as f:
    f.write(data)

# Copie de fichier binaire
def copy_file(source, destination):
    with open(source, 'rb') as src:
        with open(destination, 'wb') as dst:
            dst.write(src.read())

# Ou avec shutil (plus efficace)
import shutil
shutil.copy2('source.png', 'destination.png')

# Lecture par blocs (pour gros fichiers)
def copy_large_file(source, destination, chunk_size=1024*1024):  # 1MB
    with open(source, 'rb') as src:
        with open(destination, 'wb') as dst:
            while True:
                chunk = src.read(chunk_size)
                if not chunk:
                    break
                dst.write(chunk)
```

### Context managers personnalisés

```python
# Exemple simple
class FileHandler:
    def __init__(self, filename, mode):
        self.filename = filename
        self.mode = mode
        self.file = None
    
    def __enter__(self):
        self.file = open(self.filename, self.mode)
        return self.file
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.file:
            self.file.close()
        return False

# Utilisation
with FileHandler('test.txt', 'w') as f:
    f.write('Hello!')

# Avec contextlib
from contextlib import contextmanager

@contextmanager
def open_file(filename, mode):
    f = open(filename, mode)
    try:
        yield f
    finally:
        f.close()

with open_file('test.txt', 'w') as f:
    f.write('Hello!')
```

---

## 3.2 Formats de données

### JSON (JavaScript Object Notation)

```python
import json

# Python dict/list vers JSON
data = {
    'name': 'Alice',
    'age': 30,
    'email': 'alice@example.com',
    'hobbies': ['reading', 'coding', 'gaming'],
    'is_active': True,
    'balance': 1234.56
}

# Écrire dans un fichier JSON
with open('data.json', 'w', encoding='utf-8') as f:
    json.dump(data, f, indent=4, ensure_ascii=False)

# indent=4 : formate joliment
# ensure_ascii=False : garde les caractères spéciaux

# Convertir en string JSON
json_string = json.dumps(data, indent=2)
print(json_string)

# Lire depuis un fichier JSON
with open('data.json', 'r', encoding='utf-8') as f:
    loaded_data = json.load(f)
    print(loaded_data['name'])  # Alice

# Parser une string JSON
json_string = '{"name": "Bob", "age": 25}'
data = json.loads(json_string)
print(data['name'])  # Bob

# Gestion des erreurs
try:
    with open('data.json', 'r') as f:
        data = json.load(f)
except FileNotFoundError:
    print("Fichier non trouvé")
except json.JSONDecodeError as e:
    print(f"Erreur de parsing JSON: {e}")

# Types de données JSON ↔ Python
# JSON        Python
# object   ↔  dict
# array    ↔  list
# string   ↔  str
# number   ↔  int, float
# true     ↔  True
# false    ↔  False
# null     ↔  None

# Sérialisation d'objets personnalisés
from datetime import datetime

class Person:
    def __init__(self, name, birthdate):
        self.name = name
        self.birthdate = birthdate
    
    def to_dict(self):
        return {
            'name': self.name,
            'birthdate': self.birthdate.isoformat()
        }
    
    @classmethod
    def from_dict(cls, data):
        return cls(
            data['name'],
            datetime.fromisoformat(data['birthdate'])
        )

# Encoder personnalisé
class PersonEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, Person):
            return obj.to_dict()
        return super().default(obj)

person = Person('Alice', datetime(1990, 5, 15))
json_str = json.dumps(person, cls=PersonEncoder)
print(json_str)

# Ou utiliser to_dict()
with open('person.json', 'w') as f:
    json.dump(person.to_dict(), f)
```

### CSV (Comma-Separated Values)

```python
import csv

# Écrire un fichier CSV
data = [
    ['Nom', 'Âge', 'Ville'],
    ['Alice', '30', 'Paris'],
    ['Bob', '25', 'Lyon'],
    ['Charlie', '35', 'Marseille']
]

with open('data.csv', 'w', newline='', encoding='utf-8') as f:
    writer = csv.writer(f)
    writer.writerows(data)

# newline='' évite les lignes vides sous Windows

# Écrire ligne par ligne
with open('data.csv', 'w', newline='', encoding='utf-8') as f:
    writer = csv.writer(f)
    writer.writerow(['Nom', 'Âge', 'Ville'])
    writer.writerow(['Alice', 30, 'Paris'])

# Lire un fichier CSV
with open('data.csv', 'r', encoding='utf-8') as f:
    reader = csv.reader(f)
    for row in reader:
        print(row)  # Liste de strings

# Lire en sautant l'en-tête
with open('data.csv', 'r', encoding='utf-8') as f:
    reader = csv.reader(f)
    next(reader)  # Saute la première ligne
    for row in reader:
        print(row)

# CSV avec dictionnaires (DictReader/DictWriter)
# Lecture
with open('data.csv', 'r', encoding='utf-8') as f:
    reader = csv.DictReader(f)
    for row in reader:
        print(row['Nom'], row['Âge'])  # Accès par clé

# Écriture
data = [
    {'Nom': 'Alice', 'Âge': 30, 'Ville': 'Paris'},
    {'Nom': 'Bob', 'Âge': 25, 'Ville': 'Lyon'}
]

with open('data.csv', 'w', newline='', encoding='utf-8') as f:
    fieldnames = ['Nom', 'Âge', 'Ville']
    writer = csv.DictWriter(f, fieldnames=fieldnames)
    writer.writeheader()
    writer.writerows(data)

# Personnaliser le délimiteur
with open('data.tsv', 'w', newline='', encoding='utf-8') as f:
    writer = csv.writer(f, delimiter='\t')  # TSV (Tab-Separated)
    writer.writerow(['Nom', 'Âge', 'Ville'])

# Gérer les guillemets
with open('data.csv', 'w', newline='', encoding='utf-8') as f:
    writer = csv.writer(f, quoting=csv.QUOTE_ALL)  # Quote tout
    writer.writerow(['Alice', 'Paris, France', '30'])

# Options de quoting:
# csv.QUOTE_ALL - quote tous les champs
# csv.QUOTE_MINIMAL - quote seulement si nécessaire (défaut)
# csv.QUOTE_NONNUMERIC - quote les non-numériques
# csv.QUOTE_NONE - pas de quotes

# Dialectes CSV
with open('data.csv', 'w', newline='', encoding='utf-8') as f:
    writer = csv.writer(f, dialect='excel')  # Dialecte Excel
    writer.writerow(['Data'])

# Créer un dialecte personnalisé
csv.register_dialect('custom', delimiter=';', quotechar='"', 
                     quoting=csv.QUOTE_MINIMAL)
```

### Pickle - Sérialisation Python

```python
import pickle

# Sérialiser (dump) un objet Python
data = {
    'name': 'Alice',
    'scores': [95, 87, 92],
    'info': {'age': 30, 'city': 'Paris'}
}

with open('data.pkl', 'wb') as f:  # Mode binaire
    pickle.dump(data, f)

# Désérialiser (load)
with open('data.pkl', 'rb') as f:
    loaded_data = pickle.load(f)
    print(loaded_data)

# Sérialiser plusieurs objets
with open('multiple.pkl', 'wb') as f:
    pickle.dump(obj1, f)
    pickle.dump(obj2, f)
    pickle.dump(obj3, f)

# Désérialiser plusieurs objets
with open('multiple.pkl', 'rb') as f:
    obj1 = pickle.load(f)
    obj2 = pickle.load(f)
    obj3 = pickle.load(f)

# Sérialiser en bytes (pickle.dumps)
pickled_bytes = pickle.dumps(data)
unpickled_data = pickle.loads(pickled_bytes)

# Pickle peut sérialiser presque tout
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age

person = Person('Alice', 30)
with open('person.pkl', 'wb') as f:
    pickle.dump(person, f)

with open('person.pkl', 'rb') as f:
    loaded_person = pickle.load(f)
    print(loaded_person.name)  # Alice

# ATTENTION : Pickle n'est PAS sécurisé !
# Ne jamais unpickler des données non fiables
# Pickle peut exécuter du code arbitraire

# Versions de protocole
# 0 : ASCII (lisible)
# 1, 2 : anciennes versions binaires
# 3 : Python 3 (défaut)
# 4 : Python 3.4+ (gros objets)
# 5 : Python 3.8+ (out-of-band data)

with open('data.pkl', 'wb') as f:
    pickle.dump(data, f, protocol=pickle.HIGHEST_PROTOCOL)
```

### YAML (avec PyYAML)

```python
# pip install pyyaml
import yaml

# Écrire YAML
data = {
    'name': 'Alice',
    'age': 30,
    'hobbies': ['reading', 'coding'],
    'address': {
        'city': 'Paris',
        'country': 'France'
    }
}

with open('config.yaml', 'w', encoding='utf-8') as f:
    yaml.dump(data, f, default_flow_style=False, allow_unicode=True)

# Lire YAML
with open('config.yaml', 'r', encoding='utf-8') as f:
    loaded_data = yaml.safe_load(f)  # safe_load pour sécurité
    print(loaded_data['name'])

# YAML depuis/vers string
yaml_string = yaml.dump(data)
parsed_data = yaml.safe_load(yaml_string)
```

### XML (avec ElementTree)

```python
import xml.etree.ElementTree as ET

# Créer un XML
root = ET.Element('catalog')
book1 = ET.SubElement(root, 'book', id='1')
title1 = ET.SubElement(book1, 'title')
title1.text = '1984'
author1 = ET.SubElement(book1, 'author')
author1.text = 'George Orwell'

# Écrire dans un fichier
tree = ET.ElementTree(root)
tree.write('catalog.xml', encoding='utf-8', xml_declaration=True)

# Lire un XML
tree = ET.parse('catalog.xml')
root = tree.getroot()

# Parcourir l'arbre
for book in root.findall('book'):
    book_id = book.get('id')
    title = book.find('title').text
    author = book.find('author').text
    print(f"Livre {book_id}: {title} par {author}")

# XPath-like queries
for title in root.findall('.//title'):
    print(title.text)
```

### INI/Config files

```python
import configparser

# Créer un fichier de configuration
config = configparser.ConfigParser()

config['DATABASE'] = {
    'host': 'localhost',
    'port': '5432',
    'database': 'mydb'
}

config['API'] = {
    'key': 'abc123',
    'timeout': '30'
}

with open('config.ini', 'w') as f:
    config.write(f)

# Lire un fichier de configuration
config = configparser.ConfigParser()
config.read('config.ini')

# Accès aux valeurs
db_host = config['DATABASE']['host']
api_key = config['API']['key']

# Avec valeurs par défaut
timeout = config.get('API', 'timeout', fallback='60')

# Types
port = config.getint('DATABASE', 'port')  # Convertit en int
debug = config.getboolean('APP', 'debug', fallback=False)
```

---

## 3.3 Gestion des chemins

### pathlib - Approche moderne (Python 3.4+)

```python
from pathlib import Path

# Créer un objet Path
path = Path('data/files/example.txt')
print(path)

# Chemin absolu
abs_path = path.resolve()
print(abs_path)

# Répertoire courant
current_dir = Path.cwd()
print(current_dir)

# Répertoire home
home_dir = Path.home()
print(home_dir)

# Construction de chemins (portable)
data_dir = Path('data')
file_path = data_dir / 'files' / 'example.txt'
# Équivalent à: data/files/example.txt sur Unix
#               data\files\example.txt sur Windows

# Composants du chemin
path = Path('/home/user/documents/file.txt')
print(path.name)       # file.txt
print(path.stem)       # file
print(path.suffix)     # .txt
print(path.parent)     # /home/user/documents
print(path.parents[0]) # /home/user/documents
print(path.parents[1]) # /home/user
print(path.parts)      # ('/', 'home', 'user', 'documents', 'file.txt')

# Tests
path = Path('data/file.txt')
print(path.exists())      # Le chemin existe ?
print(path.is_file())     # C'est un fichier ?
print(path.is_dir())      # C'est un répertoire ?
print(path.is_absolute()) # Chemin absolu ?

# Opérations sur les fichiers
path = Path('example.txt')

# Lire le contenu
if path.exists():
    content = path.read_text(encoding='utf-8')
    binary_content = path.read_bytes()

# Écrire
path.write_text('Hello World!', encoding='utf-8')
path.write_bytes(b'Binary data')

# Créer des répertoires
new_dir = Path('data/logs')
new_dir.mkdir(parents=True, exist_ok=True)
# parents=True : crée les parents si nécessaire
# exist_ok=True : pas d'erreur si existe déjà

# Lister les fichiers d'un répertoire
data_dir = Path('data')

# Tous les fichiers
for item in data_dir.iterdir():
    print(item)

# Avec glob pattern
for txt_file in data_dir.glob('*.txt'):
    print(txt_file)

# Récursif
for txt_file in data_dir.rglob('*.txt'):
    print(txt_file)

# Renommer/Déplacer
old_path = Path('old_name.txt')
new_path = Path('new_name.txt')
old_path.rename(new_path)

# Supprimer
file_path = Path('temp.txt')
file_path.unlink()  # Supprime le fichier
# file_path.unlink(missing_ok=True)  # Python 3.8+

# Supprimer un répertoire
dir_path = Path('temp_dir')
dir_path.rmdir()  # Doit être vide

# Informations sur le fichier
path = Path('example.txt')
stat = path.stat()
print(f"Taille: {stat.st_size} octets")
print(f"Dernière modification: {stat.st_mtime}")

from datetime import datetime
mod_time = datetime.fromtimestamp(stat.st_mtime)
print(f"Modifié le: {mod_time}")

# Changer l'extension
path = Path('document.txt')
new_path = path.with_suffix('.pdf')  # document.pdf
```

### os.path - Approche classique

```python
import os

# Joindre des chemins
path = os.path.join('data', 'files', 'example.txt')
print(path)  # data/files/example.txt (ou data\files\example.txt)

# Chemin absolu
abs_path = os.path.abspath('file.txt')
print(abs_path)

# Normaliser un chemin
path = os.path.normpath('data/../files/./example.txt')
print(path)  # files/example.txt

# Séparer le chemin
path = '/home/user/file.txt'
dir_name = os.path.dirname(path)   # /home/user
base_name = os.path.basename(path) # file.txt
root, ext = os.path.splitext(path) # ('/home/user/file', '.txt')

# Tests
print(os.path.exists('file.txt'))
print(os.path.isfile('file.txt'))
print(os.path.isdir('data'))
print(os.path.isabs('/home/user'))

# Taille du fichier
size = os.path.getsize('file.txt')

# Date de modification
mod_time = os.path.getmtime('file.txt')

# Répertoire courant
current = os.getcwd()
```

### os - Opérations sur le système de fichiers

```python
import os

# Changer de répertoire
os.chdir('/path/to/directory')

# Créer un répertoire
os.mkdir('new_dir')
os.makedirs('path/to/new/dir', exist_ok=True)  # Crée les parents

# Lister les fichiers
files = os.listdir('.')
for file in files:
    print(file)

# Filtrer par extension
txt_files = [f for f in os.listdir('.') if f.endswith('.txt')]

# Parcourir récursivement
for root, dirs, files in os.walk('.'):
    print(f"Répertoire: {root}")
    for file in files:
        file_path = os.path.join(root, file)
        print(f"  Fichier: {file_path}")

# Renommer
os.rename('old_name.txt', 'new_name.txt')

# Supprimer
os.remove('file.txt')  # Fichier
os.rmdir('empty_dir')  # Répertoire vide
os.removedirs('path/to/empty/dirs')  # Répertoires vides récursivement

# Variables d'environnement
home = os.environ.get('HOME')
path = os.environ.get('PATH')

# Informations système
print(os.name)  # 'posix' sur Unix, 'nt' sur Windows
print(os.sep)   # '/' sur Unix, '\\' sur Windows
```

### shutil - Opérations de haut niveau

```python
import shutil

# Copier un fichier
shutil.copy('source.txt', 'destination.txt')      # Copie le fichier
shutil.copy2('source.txt', 'destination.txt')     # + métadonnées
shutil.copyfile('source.txt', 'destination.txt')  # Contenu seulement

# Copier un répertoire
shutil.copytree('source_dir', 'destination_dir')

# Déplacer
shutil.move('source.txt', 'destination.txt')
shutil.move('file.txt', 'directory/')

# Supprimer un répertoire (même non vide)
shutil.rmtree('directory')

# Créer une archive
shutil.make_archive('backup', 'zip', 'source_dir')
# Formats: 'zip', 'tar', 'gztar', 'bztar', 'xztar'

# Extraire une archive
shutil.unpack_archive('archive.zip', 'destination_dir')

# Espace disque
usage = shutil.disk_usage('/')
print(f"Total: {usage.total / (1024**3):.2f} GB")
print(f"Utilisé: {usage.used / (1024**3):.2f} GB")
print(f"Libre: {usage.free / (1024**3):.2f} GB")

# Trouver un exécutable
python_path = shutil.which('python')
print(python_path)
```

### glob - Pattern matching

```python
import glob

# Trouver tous les .txt
txt_files = glob.glob('*.txt')
print(txt_files)

# Récursif (Python 3.5+)
all_txt = glob.glob('**/*.txt', recursive=True)

# Avec pathlib (plus moderne)
from pathlib import Path
txt_files = list(Path('.').glob('*.txt'))
all_txt = list(Path('.').rglob('*.txt'))

# Patterns
# * : n'importe quels caractères
# ? : un caractère
# [seq] : n'importe quel caractère dans seq
# [!seq] : n'importe quel caractère pas dans seq

files = glob.glob('data_[0-9].txt')  # data_0.txt, data_1.txt, ...
```

### Exemple complet : Script de gestion de fichiers

```python
from pathlib import Path
import shutil
from datetime import datetime

def organize_files(source_dir, dest_dir):
    """Organise les fichiers par extension"""
    source = Path(source_dir)
    dest = Path(dest_dir)
    
    # Créer le répertoire de destination
    dest.mkdir(parents=True, exist_ok=True)
    
    # Parcourir les fichiers
    for file_path in source.rglob('*'):
        if file_path.is_file():
            # Obtenir l'extension
            ext = file_path.suffix[1:] or 'no_extension'
            
            # Créer le sous-répertoire
            ext_dir = dest / ext
            ext_dir.mkdir(exist_ok=True)
            
            # Copier le fichier
            dest_file = ext_dir / file_path.name
            
            # Éviter d'écraser
            if dest_file.exists():
                timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
                stem = dest_file.stem
                dest_file = ext_dir / f"{stem}_{timestamp}{dest_file.suffix}"
            
            shutil.copy2(file_path, dest_file)
            print(f"Copié: {file_path} → {dest_file}")

def cleanup_old_files(directory, days=30):
    """Supprime les fichiers plus vieux que X jours"""
    import time
    
    directory = Path(directory)
    current_time = time.time()
    cutoff = current_time - (days * 86400)  # 86400 secondes = 1 jour
    
    for file_path in directory.rglob('*'):
        if file_path.is_file():
            file_time = file_path.stat().st_mtime
            if file_time < cutoff:
                print(f"Suppression: {file_path}")
                file_path.unlink()

def get_directory_size(directory):
    """Calcule la taille totale d'un répertoire"""
    directory = Path(directory)
    total_size = 0
    
    for file_path in directory.rglob('*'):
        if file_path.is_file():
            total_size += file_path.stat().st_size
    
    # Convertir en unités lisibles
    for unit in ['B', 'KB', 'MB', 'GB', 'TB']:
        if total_size < 1024:
            return f"{total_size:.2f} {unit}"
        total_size /= 1024

# Utilisation
if __name__ == '__main__':
    organize_files('downloads', 'organized')
    cleanup_old_files('temp', days=7)
    size = get_directory_size('data')
    print(f"Taille du répertoire: {size}")
```

---

## 🎯 Points clés à retenir

### Lecture/Écriture de fichiers
- **TOUJOURS** utiliser `with open()` (context manager)
- Spécifier l'**encodage** explicitement (`encoding='utf-8'`)
- Modes: `r` (lecture), `w` (écriture), `a` (ajout), `b` (binaire)
- Itérer ligne par ligne pour les gros fichiers

### Formats de données
- **JSON** : `json.dump()`, `json.load()` - interchange universel
- **CSV** : `csv.writer()`, `csv.DictReader()` - données tabulaires
- **Pickle** : sérialisation Python (⚠️ pas sécurisé)
- Toujours gérer les exceptions (`try/except`)

### Chemins de fichiers
- **pathlib** (moderne) : `Path()`, méthodes orientées objet
- **os.path** (classique) : `os.path.join()`, fonctions
- Utiliser `/` avec pathlib pour portabilité
- `mkdir(parents=True, exist_ok=True)` pour créer des répertoires

### Bonnes pratiques
- Context managers pour fermer automatiquement
- Encodage UTF-8 par défaut
- Vérifier l'existence avant suppression
- Utiliser `pathlib` pour nouveau code

---

## 💡 Questions d'entretien courantes (Chapitre 3)

1. **Quelle est la différence entre `read()`, `readline()` et `readlines()` ?**
   - `read()` : lit tout le fichier en une string
   - `readline()` : lit une ligne
   - `readlines()` : lit toutes les lignes dans une liste

2. **Pourquoi utiliser un context manager (`with`) ?**
   - Ferme automatiquement le fichier
   - Même en cas d'erreur/exception
   - Gère proprement les ressources

3. **Différence entre pickle et JSON ?**
   - JSON : format texte, lisible, universel, types limités
   - Pickle : binaire, Python seulement, tous types Python, pas sécurisé

4. **Comment lire un très gros fichier sans saturer la mémoire ?**
   - Itérer ligne par ligne : `for line in f:`
   - Lire par blocs : `f.read(chunk_size)`
   - Utiliser des générateurs

5. **pathlib vs os.path ?**
   - pathlib : moderne, orienté objet, chaînable
   - os.path : classique, fonctions, compatible anciennes versions

6. **Comment gérer les différents encodages ?**
   - Toujours spécifier `encoding='utf-8'`
   - Utiliser `errors='ignore'` ou `'replace'` si nécessaire
   - Détecter avec `chardet` si inconnu

---

## 🔧 Exercices pratiques

1. **Script de recherche dans les fichiers**
   - Cherche un mot dans tous les .txt d'un répertoire
   - Affiche le fichier et le numéro de ligne
   - Compte le nombre d'occurrences

2. **Convertisseur CSV → JSON**
   - Lit un CSV avec DictReader
   - Convertit en liste de dictionnaires
   - Écrit en JSON formaté

3. **Organiseur de fichiers**
   - Trie les fichiers par extension
   - Crée des sous-répertoires
   - Gère les doublons avec timestamp

4. **Analyseur de logs**
   - Lit un fichier log ligne par ligne
   - Extrait les erreurs
   - Génère un rapport JSON

5. **Backup automatique**
   - Copie un répertoire avec date
   - Crée une archive zip
   - Nettoie les vieux backups

---

**Prochaine étape :** Chapitre 4 - Gestion des Erreurs et Exceptions
