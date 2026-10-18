## [**..**](./00_index.md)

# Chapitre 6 : Scripting et Ligne de Commande

## 🎯 Objectifs du chapitre
- Créer des scripts CLI professionnels avec argparse
- Exécuter et contrôler des commandes système avec subprocess
- Gérer les variables d'environnement
- Automatiser des tâches courantes
- Appliquer les bonnes pratiques de scripting

---

## 6.1 Arguments en ligne de commande

### sys.argv - Méthode simple

```python
import sys

# script.py
print(f"Nom du script: {sys.argv[0]}")
print(f"Nombre d'arguments: {len(sys.argv) - 1}")
print(f"Arguments: {sys.argv[1:]}")

# Usage: python script.py arg1 arg2 arg3
# Sortie:
# Nom du script: script.py
# Nombre d'arguments: 3
# Arguments: ['arg1', 'arg2', 'arg3']

# Exemple: script de copie simple
if len(sys.argv) != 3:
    print("Usage: python script.py <source> <destination>")
    sys.exit(1)

source = sys.argv[1]
destination = sys.argv[2]
print(f"Copie de {source} vers {destination}")

# Limitations:
# - Pas de validation
# - Pas d'aide automatique
# - Pas d'options (--verbose, -h, etc.)
# - Arguments positionnels uniquement
```

### argparse - Solution professionnelle

```python
import argparse

# Créer le parser
parser = argparse.ArgumentParser(
    description='Script de démonstration argparse',
    epilog='Exemple: python script.py --verbose input.txt output.txt'
)

# Arguments positionnels (obligatoires)
parser.add_argument('input', help='Fichier d\'entrée')
parser.add_argument('output', help='Fichier de sortie')

# Arguments optionnels
parser.add_argument('-v', '--verbose', action='store_true',
                    help='Mode verbeux')
parser.add_argument('-n', '--number', type=int, default=10,
                    help='Nombre d\'itérations (défaut: 10)')
parser.add_argument('-f', '--format', choices=['json', 'csv', 'xml'],
                    default='json', help='Format de sortie')

# Parser les arguments
args = parser.parse_args()

# Utilisation
print(f"Entrée: {args.input}")
print(f"Sortie: {args.output}")
if args.verbose:
    print(f"Mode verbeux activé")
print(f"Nombre: {args.number}")
print(f"Format: {args.format}")

# Aide automatique: python script.py -h
```

### Types d'arguments argparse

```python
import argparse

parser = argparse.ArgumentParser()

# 1. Arguments positionnels
parser.add_argument('filename', help='Nom du fichier')
parser.add_argument('count', type=int, help='Nombre d\'éléments')

# 2. Arguments optionnels
parser.add_argument('-o', '--output', help='Fichier de sortie')
parser.add_argument('-v', '--verbose', action='store_true')

# 3. Types de données
parser.add_argument('-n', '--number', type=int, default=0)
parser.add_argument('-f', '--float', type=float)
parser.add_argument('-p', '--path', type=argparse.FileType('r'))

# 4. Choix limités
parser.add_argument('--format', choices=['json', 'xml', 'csv'])
parser.add_argument('--level', type=int, choices=range(1, 6))

# 5. Actions
parser.add_argument('-v', '--verbose', action='store_true')    # True si présent
parser.add_argument('-q', '--quiet', action='store_false')     # False si présent
parser.add_argument('--version', action='version', version='1.0.0')
parser.add_argument('--add', action='append')                  # Liste d'éléments
parser.add_argument('--count', action='count')                 # Compte les occurrences

# 6. Valeurs multiples
parser.add_argument('--files', nargs='+', help='Un ou plusieurs fichiers')
parser.add_argument('--coords', nargs=2, type=float, help='x y')
parser.add_argument('--optional', nargs='?', default='default')
parser.add_argument('--all', nargs='*', help='0 ou plus')

# 7. Arguments mutuellement exclusifs
group = parser.add_mutually_exclusive_group()
group.add_argument('--json', action='store_true')
group.add_argument('--xml', action='store_true')

# 8. Arguments requis
parser.add_argument('--config', required=True, help='Fichier config (requis)')

# 9. Métavariable pour l'aide
parser.add_argument('--output', metavar='FILE', help='Fichier de sortie')
```

### Sous-commandes (comme git)

```python
import argparse

# Parser principal
parser = argparse.ArgumentParser(prog='mytool')
subparsers = parser.add_subparsers(dest='command', help='Commandes disponibles')

# Sous-commande "create"
parser_create = subparsers.add_parser('create', help='Créer un élément')
parser_create.add_argument('name', help='Nom de l\'élément')
parser_create.add_argument('--type', choices=['file', 'dir'], default='file')

# Sous-commande "delete"
parser_delete = subparsers.add_parser('delete', help='Supprimer un élément')
parser_delete.add_argument('name', help='Nom de l\'élément')
parser_delete.add_argument('-f', '--force', action='store_true')

# Sous-commande "list"
parser_list = subparsers.add_parser('list', help='Lister les éléments')
parser_list.add_argument('--sort', choices=['name', 'date', 'size'])

# Parser
args = parser.parse_args()

# Dispatcher
if args.command == 'create':
    print(f"Création de {args.name} (type: {args.type})")
elif args.command == 'delete':
    force_msg = " (forcé)" if args.force else ""
    print(f"Suppression de {args.name}{force_msg}")
elif args.command == 'list':
    sort_msg = f" (tri: {args.sort})" if args.sort else ""
    print(f"Liste des éléments{sort_msg}")
else:
    parser.print_help()

# Usage:
# python mytool.py create myfile --type file
# python mytool.py delete myfile -f
# python mytool.py list --sort name
```

### Exemple complet: Script de backup

```python
#!/usr/bin/env python3
"""
Script de backup de fichiers.
"""
import argparse
import shutil
from pathlib import Path
from datetime import datetime

def create_backup(source, destination, compress=False, verbose=False):
    """Crée un backup du répertoire source."""
    source_path = Path(source)
    
    if not source_path.exists():
        raise FileNotFoundError(f"Source {source} n'existe pas")
    
    # Créer le nom du backup avec timestamp
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    backup_name = f"{source_path.name}_backup_{timestamp}"
    
    dest_path = Path(destination) / backup_name
    
    if verbose:
        print(f"Création du backup de {source} vers {dest_path}")
    
    # Copier
    if source_path.is_file():
        shutil.copy2(source, dest_path)
    else:
        shutil.copytree(source, dest_path)
    
    # Compresser si demandé
    if compress:
        if verbose:
            print("Compression du backup...")
        archive_path = shutil.make_archive(dest_path, 'zip', dest_path)
        shutil.rmtree(dest_path)  # Supprimer le répertoire non compressé
        if verbose:
            print(f"Archive créée: {archive_path}")
        return archive_path
    
    return dest_path

def main():
    parser = argparse.ArgumentParser(
        description='Crée un backup de fichiers/répertoires',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Exemples:
  python backup.py /data /backups
  python backup.py /data /backups -c -v
  python backup.py /data /backups --compress --verbose
        """
    )
    
    # Arguments positionnels
    parser.add_argument('source', help='Fichier ou répertoire à sauvegarder')
    parser.add_argument('destination', help='Répertoire de destination')
    
    # Arguments optionnels
    parser.add_argument('-c', '--compress', action='store_true',
                        help='Compresser le backup en ZIP')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Afficher les détails')
    parser.add_argument('--version', action='version', version='1.0.0')
    
    args = parser.parse_args()
    
    try:
        result = create_backup(
            args.source,
            args.destination,
            compress=args.compress,
            verbose=args.verbose
        )
        print(f"✅ Backup créé: {result}")
        return 0
    except Exception as e:
        print(f"❌ Erreur: {e}")
        return 1

if __name__ == '__main__':
    import sys
    sys.exit(main())
```

---

## 6.2 Variables d'environnement

### Lecture des variables d'environnement

```python
import os

# Obtenir une variable
home = os.environ.get('HOME')          # None si n'existe pas
path = os.environ.get('PATH', '/usr/bin')  # Valeur par défaut

# Accès direct (lève KeyError si n'existe pas)
try:
    user = os.environ['USER']
except KeyError:
    user = 'unknown'

# Vérifier l'existence
if 'EDITOR' in os.environ:
    editor = os.environ['EDITOR']

# Obtenir toutes les variables
for key, value in os.environ.items():
    print(f"{key} = {value}")

# Variables courantes
print(os.environ.get('HOME'))      # Répertoire home
print(os.environ.get('USER'))      # Nom d'utilisateur
print(os.environ.get('PATH'))      # Chemins d'exécutables
print(os.environ.get('LANG'))      # Langue système
print(os.environ.get('SHELL'))     # Shell par défaut
```

### Définir des variables d'environnement

```python
import os

# Définir une variable (pour ce processus et ses enfants)
os.environ['MY_VAR'] = 'my_value'

# Modifier PATH
current_path = os.environ.get('PATH', '')
os.environ['PATH'] = f"/custom/bin:{current_path}"

# Supprimer une variable
if 'TEMP_VAR' in os.environ:
    del os.environ['TEMP_VAR']

# Variables temporaires avec subprocess
import subprocess
env = os.environ.copy()
env['MY_VAR'] = 'value'
subprocess.run(['command'], env=env)
```

### Fichiers .env avec python-dotenv

```python
# Installation: pip install python-dotenv

# Fichier .env
"""
DATABASE_URL=postgresql://localhost/mydb
API_KEY=abc123xyz
DEBUG=True
MAX_CONNECTIONS=10
"""

# Script Python
from dotenv import load_dotenv
import os

# Charger le fichier .env
load_dotenv()

# Accéder aux variables
db_url = os.environ.get('DATABASE_URL')
api_key = os.environ.get('API_KEY')
debug = os.environ.get('DEBUG', 'False') == 'True'
max_conn = int(os.environ.get('MAX_CONNECTIONS', 5))

print(f"DB: {db_url}")
print(f"Debug: {debug}")

# Spécifier un fichier .env
load_dotenv('.env.production')

# Surcharger les variables existantes
load_dotenv(override=True)
```

### Configuration avec variables d'environnement

```python
import os
from pathlib import Path

class Config:
    """Configuration de l'application."""
    
    # Valeurs par défaut
    DEBUG = False
    DATABASE_URL = 'sqlite:///app.db'
    LOG_LEVEL = 'INFO'
    MAX_WORKERS = 4
    
    @classmethod
    def from_env(cls):
        """Charge la config depuis les variables d'environnement."""
        config = cls()
        
        # Booléens
        config.DEBUG = os.environ.get('DEBUG', 'False').lower() == 'true'
        
        # Strings
        config.DATABASE_URL = os.environ.get('DATABASE_URL', cls.DATABASE_URL)
        config.LOG_LEVEL = os.environ.get('LOG_LEVEL', cls.LOG_LEVEL)
        
        # Entiers
        config.MAX_WORKERS = int(os.environ.get('MAX_WORKERS', cls.MAX_WORKERS))
        
        # Chemins
        config.DATA_DIR = Path(os.environ.get('DATA_DIR', 'data'))
        
        return config

# Usage
config = Config.from_env()
print(f"Debug: {config.DEBUG}")
print(f"Database: {config.DATABASE_URL}")
```

---

## 6.3 Exécution de commandes système

### subprocess.run() - Recommandé (Python 3.5+)

```python
import subprocess

# Exécution simple
result = subprocess.run(['ls', '-la'])
# Affiche directement la sortie

# Capturer la sortie
result = subprocess.run(['ls', '-la'], capture_output=True, text=True)
print(result.stdout)  # Sortie standard
print(result.stderr)  # Sortie d'erreur
print(result.returncode)  # Code de retour (0 = succès)

# Alternative (Python < 3.7)
result = subprocess.run(
    ['ls', '-la'],
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True  # ou encoding='utf-8'
)

# Vérifier le succès
result = subprocess.run(['false'], check=True)  # Lève CalledProcessError

# Gestion des erreurs
try:
    result = subprocess.run(
        ['command_qui_echoue'],
        check=True,
        capture_output=True,
        text=True
    )
except subprocess.CalledProcessError as e:
    print(f"Erreur: {e}")
    print(f"Code de retour: {e.returncode}")
    print(f"Sortie: {e.stdout}")
    print(f"Erreur: {e.stderr}")

# Timeout
try:
    result = subprocess.run(['sleep', '10'], timeout=5)
except subprocess.TimeoutExpired:
    print("Commande trop longue!")

# Répertoire de travail
result = subprocess.run(['pwd'], cwd='/tmp', capture_output=True, text=True)
print(result.stdout)  # /tmp

# Variables d'environnement
env = os.environ.copy()
env['MY_VAR'] = 'value'
result = subprocess.run(['env'], env=env, capture_output=True, text=True)

# Shell=True (ATTENTION: risque de sécurité)
result = subprocess.run('ls -la | grep .py', shell=True, capture_output=True, text=True)
# PRÉFÉRABLE:
result = subprocess.run(['ls', '-la'], capture_output=True, text=True)
files = [line for line in result.stdout.split('\n') if '.py' in line]
```

### Exemples pratiques

```python
import subprocess
import sys

def run_command(cmd, verbose=False):
    """Execute une commande et retourne stdout, stderr, returncode."""
    if verbose:
        print(f"Exécution: {' '.join(cmd)}")
    
    result = subprocess.run(
        cmd,
        capture_output=True,
        text=True
    )
    
    if verbose:
        if result.stdout:
            print(result.stdout)
        if result.stderr:
            print(result.stderr, file=sys.stderr)
    
    return result.stdout, result.stderr, result.returncode

# Exemple: git status
stdout, stderr, code = run_command(['git', 'status'], verbose=True)
if code == 0:
    print("Git OK")
else:
    print("Git erreur")

# Exemple: vérifier si une commande existe
def command_exists(command):
    """Vérifie si une commande existe."""
    try:
        subprocess.run(
            ['which', command],
            check=True,
            capture_output=True
        )
        return True
    except subprocess.CalledProcessError:
        return False

if command_exists('git'):
    print("Git est installé")
else:
    print("Git n'est pas installé")

# Exemple: pipeline de commandes
def git_changed_files():
    """Liste les fichiers modifiés dans git."""
    result = subprocess.run(
        ['git', 'diff', '--name-only'],
        capture_output=True,
        text=True,
        check=True
    )
    return result.stdout.strip().split('\n') if result.stdout else []

files = git_changed_files()
for file in files:
    print(f"Modifié: {file}")
```

### subprocess.Popen - Contrôle avancé

```python
import subprocess

# Démarrer un processus en arrière-plan
process = subprocess.Popen(
    ['sleep', '10'],
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE
)

# Vérifier si le processus est terminé
if process.poll() is None:
    print("Processus en cours...")
else:
    print(f"Processus terminé avec code {process.returncode}")

# Attendre la fin
process.wait()
stdout, stderr = process.communicate()

# Tuer le processus
process.terminate()  # SIGTERM
# ou
process.kill()       # SIGKILL

# Processus avec entrée/sortie interactive
process = subprocess.Popen(
    ['python', '-i'],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True
)

# Envoyer des commandes
process.stdin.write('print(2+2)\n')
process.stdin.flush()

# Lire la sortie
output = process.stdout.readline()
print(output)

# Fermer
process.stdin.close()
process.terminate()
process.wait()
```

### os.system() - Déprécié mais parfois utile

```python
import os

# Exécution simple (retourne le code de sortie)
exit_code = os.system('ls -la')

# Problèmes:
# - Pas de capture de sortie
# - Risques de sécurité avec shell
# - Moins de contrôle

# PRÉFÉRABLE: utiliser subprocess.run()
```

---

## 6.4 Scripts utilitaires courants

### Script de traitement de logs

```python
#!/usr/bin/env python3
"""
Analyse un fichier de logs et extrait les erreurs.
"""
import argparse
import re
from collections import Counter
from datetime import datetime

def parse_log_line(line):
    """Parse une ligne de log."""
    # Format: 2024-01-29 14:30:45 [ERROR] module: message
    pattern = r'(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}) \[(\w+)\] (\w+): (.+)'
    match = re.match(pattern, line)
    if match:
        timestamp, level, module, message = match.groups()
        return {
            'timestamp': datetime.strptime(timestamp, '%Y-%m-%d %H:%M:%S'),
            'level': level,
            'module': module,
            'message': message
        }
    return None

def analyze_logs(filename, level_filter=None):
    """Analyse un fichier de logs."""
    errors = []
    level_count = Counter()
    module_count = Counter()
    
    with open(filename, 'r') as f:
        for line in f:
            parsed = parse_log_line(line.strip())
            if parsed:
                level_count[parsed['level']] += 1
                module_count[parsed['module']] += 1
                
                if level_filter is None or parsed['level'] == level_filter:
                    errors.append(parsed)
    
    return errors, level_count, module_count

def main():
    parser = argparse.ArgumentParser(description='Analyse les fichiers de logs')
    parser.add_argument('logfile', help='Fichier de log à analyser')
    parser.add_argument('-l', '--level', choices=['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'],
                        help='Filtrer par niveau')
    parser.add_argument('-n', '--number', type=int, default=10,
                        help='Nombre de lignes à afficher')
    parser.add_argument('-s', '--stats', action='store_true',
                        help='Afficher les statistiques')
    
    args = parser.parse_args()
    
    try:
        entries, level_count, module_count = analyze_logs(args.logfile, args.level)
        
        if args.stats:
            print("\n=== Statistiques ===")
            print("\nNiveau:")
            for level, count in level_count.most_common():
                print(f"  {level}: {count}")
            
            print("\nModules:")
            for module, count in module_count.most_common(5):
                print(f"  {module}: {count}")
        
        print(f"\n=== Dernières {args.number} entrées ===")
        for entry in entries[-args.number:]:
            print(f"{entry['timestamp']} [{entry['level']}] {entry['module']}: {entry['message']}")
        
        return 0
    except Exception as e:
        print(f"Erreur: {e}")
        return 1

if __name__ == '__main__':
    import sys
    sys.exit(main())
```

### Script de manipulation de fichiers en masse

```python
#!/usr/bin/env python3
"""
Renomme des fichiers en masse selon un pattern.
"""
import argparse
import re
from pathlib import Path

def rename_files(directory, pattern, replacement, dry_run=True, verbose=False):
    """Renomme les fichiers correspondant au pattern."""
    directory = Path(directory)
    renamed = 0
    
    for file_path in directory.glob('*'):
        if file_path.is_file():
            old_name = file_path.name
            new_name = re.sub(pattern, replacement, old_name)
            
            if old_name != new_name:
                new_path = file_path.parent / new_name
                
                if verbose or dry_run:
                    print(f"{old_name} → {new_name}")
                
                if not dry_run:
                    file_path.rename(new_path)
                
                renamed += 1
    
    return renamed

def main():
    parser = argparse.ArgumentParser(
        description='Renomme des fichiers en masse',
        epilog='Exemple: python rename.py /path "IMG_" "photo_" --execute'
    )
    
    parser.add_argument('directory', help='Répertoire contenant les fichiers')
    parser.add_argument('pattern', help='Pattern regex à rechercher')
    parser.add_argument('replacement', help='Texte de remplacement')
    parser.add_argument('-e', '--execute', action='store_true',
                        help='Exécuter réellement (sinon dry-run)')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Mode verbeux')
    
    args = parser.parse_args()
    
    dry_run = not args.execute
    if dry_run:
        print("=== MODE DRY-RUN (utilisez --execute pour renommer) ===\n")
    
    renamed = rename_files(
        args.directory,
        args.pattern,
        args.replacement,
        dry_run=dry_run,
        verbose=args.verbose
    )
    
    print(f"\n{renamed} fichier(s) {'seraient' if dry_run else 'ont été'} renommé(s)")
    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main())
```

### Script d'automatisation de tâches

```python
#!/usr/bin/env python3
"""
Automatise des tâches de maintenance système.
"""
import argparse
import subprocess
import logging
from pathlib import Path
from datetime import datetime, timedelta

def setup_logging(verbose=False):
    """Configure le logging."""
    level = logging.DEBUG if verbose else logging.INFO
    logging.basicConfig(
        level=level,
        format='%(asctime)s [%(levelname)s] %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S'
    )

def cleanup_old_files(directory, days=30, pattern='*.log', dry_run=True):
    """Supprime les fichiers plus vieux que X jours."""
    directory = Path(directory)
    cutoff = datetime.now() - timedelta(days=days)
    deleted = []
    
    for file_path in directory.rglob(pattern):
        if file_path.is_file():
            mtime = datetime.fromtimestamp(file_path.stat().st_mtime)
            if mtime < cutoff:
                age = (datetime.now() - mtime).days
                logging.info(f"Fichier ancien ({age} jours): {file_path}")
                
                if not dry_run:
                    file_path.unlink()
                    logging.info(f"Supprimé: {file_path}")
                
                deleted.append(file_path)
    
    return deleted

def backup_directory(source, destination):
    """Crée un backup d'un répertoire."""
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    backup_name = f"backup_{timestamp}"
    
    cmd = ['tar', '-czf', f"{destination}/{backup_name}.tar.gz", source]
    
    logging.info(f"Création du backup: {source} → {destination}/{backup_name}.tar.gz")
    
    try:
        subprocess.run(cmd, check=True, capture_output=True, text=True)
        logging.info("Backup créé avec succès")
        return True
    except subprocess.CalledProcessError as e:
        logging.error(f"Erreur lors du backup: {e.stderr}")
        return False

def check_disk_space(path='/', threshold=90):
    """Vérifie l'espace disque disponible."""
    import shutil
    
    usage = shutil.disk_usage(path)
    percent_used = (usage.used / usage.total) * 100
    
    logging.info(f"Utilisation disque {path}: {percent_used:.1f}%")
    
    if percent_used > threshold:
        logging.warning(f"⚠️  Espace disque faible: {percent_used:.1f}% utilisé")
        return False
    
    return True

def main():
    parser = argparse.ArgumentParser(description='Script de maintenance système')
    parser.add_argument('-v', '--verbose', action='store_true', help='Mode verbeux')
    parser.add_argument('--cleanup', metavar='DIR', help='Nettoyer les vieux fichiers')
    parser.add_argument('--days', type=int, default=30, help='Âge des fichiers à supprimer')
    parser.add_argument('--backup', nargs=2, metavar=('SRC', 'DST'), help='Créer un backup')
    parser.add_argument('--check-disk', action='store_true', help='Vérifier l\'espace disque')
    parser.add_argument('--execute', action='store_true', help='Exécuter (sinon dry-run)')
    
    args = parser.parse_args()
    
    setup_logging(args.verbose)
    
    dry_run = not args.execute
    if dry_run:
        logging.info("=== MODE DRY-RUN ===")
    
    # Nettoyage
    if args.cleanup:
        deleted = cleanup_old_files(args.cleanup, args.days, dry_run=dry_run)
        logging.info(f"{len(deleted)} fichier(s) {'seraient' if dry_run else 'ont été'} supprimé(s)")
    
    # Backup
    if args.backup:
        source, dest = args.backup
        if not dry_run:
            backup_directory(source, dest)
        else:
            logging.info(f"Backup: {source} → {dest} (dry-run)")
    
    # Vérification disque
    if args.check_disk:
        check_disk_space()
    
    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main())
```

---

## 6.5 Bonnes pratiques de scripting

### Structure d'un script professionnel

```python
#!/usr/bin/env python3
"""
Description du script.

Usage:
    script.py [options] <arguments>

Examples:
    script.py --verbose input.txt output.txt
"""

import sys
import argparse
import logging
from pathlib import Path

# Constants
VERSION = '1.0.0'
DEFAULT_TIMEOUT = 30

def setup_logging(verbose=False):
    """Configure logging."""
    level = logging.DEBUG if verbose else logging.INFO
    logging.basicConfig(
        level=level,
        format='%(asctime)s [%(levelname)s] %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S'
    )

def validate_args(args):
    """Valide les arguments."""
    input_path = Path(args.input)
    if not input_path.exists():
        raise FileNotFoundError(f"Fichier introuvable: {args.input}")
    
    if not input_path.is_file():
        raise ValueError(f"Pas un fichier: {args.input}")
    
    return True

def process(input_file, output_file, options):
    """Fonction principale de traitement."""
    logging.info(f"Traitement: {input_file} → {output_file}")
    
    # Traitement ici
    try:
        # ...
        logging.info("Traitement terminé avec succès")
        return True
    except Exception as e:
        logging.error(f"Erreur: {e}")
        return False

def main():
    """Point d'entrée principal."""
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Arguments
    parser.add_argument('input', help='Fichier d\'entrée')
    parser.add_argument('output', help='Fichier de sortie')
    parser.add_argument('-v', '--verbose', action='store_true')
    parser.add_argument('--version', action='version', version=VERSION)
    
    args = parser.parse_args()
    
    # Configuration
    setup_logging(args.verbose)
    
    # Validation
    try:
        validate_args(args)
    except (FileNotFoundError, ValueError) as e:
        logging.error(e)
        return 1
    
    # Traitement
    success = process(args.input, args.output, {})
    
    return 0 if success else 1

if __name__ == '__main__':
    sys.exit(main())
```

### Gestion d'erreurs robuste

```python
import sys
import logging
import signal

def signal_handler(sig, frame):
    """Gère l'interruption par Ctrl+C."""
    logging.info("\nInterruption reçue, nettoyage...")
    # Nettoyage ici
    sys.exit(0)

# Enregistrer le handler
signal.signal(signal.SIGINT, signal_handler)

def safe_execute(func, *args, **kwargs):
    """Exécute une fonction avec gestion d'erreur."""
    try:
        return func(*args, **kwargs), None
    except Exception as e:
        logging.exception(f"Erreur dans {func.__name__}")
        return None, e

# Usage
result, error = safe_execute(risky_function, arg1, arg2)
if error:
    logging.error(f"Échec: {error}")
    sys.exit(1)
```

### Configuration et profils

```python
import json
from pathlib import Path

class Config:
    """Gestion de la configuration."""
    
    def __init__(self, config_file=None):
        self.config_file = config_file or self._default_config_path()
        self.data = self._load()
    
    @staticmethod
    def _default_config_path():
        """Chemin par défaut de la config."""
        return Path.home() / '.config' / 'myscript' / 'config.json'
    
    def _load(self):
        """Charge la configuration."""
        if self.config_file.exists():
            with open(self.config_file) as f:
                return json.load(f)
        return self._defaults()
    
    @staticmethod
    def _defaults():
        """Configuration par défaut."""
        return {
            'timeout': 30,
            'retries': 3,
            'log_level': 'INFO'
        }
    
    def save(self):
        """Sauvegarde la configuration."""
        self.config_file.parent.mkdir(parents=True, exist_ok=True)
        with open(self.config_file, 'w') as f:
            json.dump(self.data, f, indent=2)
    
    def get(self, key, default=None):
        """Obtient une valeur."""
        return self.data.get(key, default)
    
    def set(self, key, value):
        """Définit une valeur."""
        self.data[key] = value
        self.save()

# Usage
config = Config()
timeout = config.get('timeout', 30)
```

---

## 🎯 Points clés à retenir

### Arguments CLI
- **sys.argv** : simple mais limité
- **argparse** : solution professionnelle
- Toujours fournir une aide (`-h`)
- Valider les arguments
- Sous-commandes pour outils complexes

### Variables d'environnement
- `os.environ.get()` avec valeur par défaut
- `.env` avec python-dotenv pour config locale
- Ne jamais hardcoder les secrets
- Documenter les variables requises

### Subprocess
- **subprocess.run()** pour exécution simple
- **capture_output=True** pour capturer stdout/stderr
- **check=True** pour lever exception si échec
- **timeout** pour éviter les blocages
- **shell=False** pour sécurité (sauf cas particuliers)

### Bonnes pratiques
- Shebang `#!/usr/bin/env python3`
- Docstring au début
- Fonction `main()` avec `sys.exit()`
- Logging au lieu de print
- Validation des arguments
- Gestion des erreurs
- Mode dry-run pour opérations dangereuses

---

## 💡 Questions d'entretien courantes (Chapitre 6)

1. **Différence entre sys.argv et argparse ?**
   - sys.argv : liste simple, pas de validation
   - argparse : parsing automatique, aide, types, validation
   - argparse recommandé pour scripts professionnels

2. **Pourquoi éviter shell=True dans subprocess ?**
   - Risque d'injection de commandes
   - Comportement dépendant du shell
   - Moins portable
   - Utiliser liste d'arguments à la place

3. **Comment capturer stdout et stderr séparément ?**
   ```python
   result = subprocess.run(
       ['command'],
       stdout=subprocess.PIPE,
       stderr=subprocess.PIPE,
       text=True
   )
   print(result.stdout)
   print(result.stderr)
   ```

4. **Comment rendre un script exécutable ?**
   - Ajouter shebang : `#!/usr/bin/env python3`
   - Rendre exécutable : `chmod +x script.py`
   - Lancer : `./script.py` au lieu de `python script.py`

5. **Quelle est la différence entre os.environ et os.getenv() ?**
   - `os.environ` : dictionnaire, lève KeyError si absent
   - `os.getenv()` : fonction, retourne None si absent
   - `os.environ.get()` : méthode dict, valeur par défaut

6. **Comment gérer Ctrl+C proprement ?**
   - Utiliser signal.signal(signal.SIGINT, handler)
   - Faire le nettoyage dans le handler
   - sys.exit(0) pour sortie propre

---

## 🔧 Exercices pratiques

1. **Script de recherche de fichiers**
   - Arguments : répertoire, pattern, extension
   - Options : --recursive, --size, --modified-since
   - Afficher résultats avec taille et date

2. **Gestionnaire de processus**
   - Lancer une commande avec timeout
   - Logger stdout/stderr
   - Gérer les codes de retour

3. **Script de déploiement**
   - Sous-commandes : deploy, rollback, status
   - Variables d'env pour config
   - Exécution de commandes ssh/git

4. **Analyseur de CSV**
   - Lire CSV, filtrer, transformer
   - Arguments pour colonnes, filtres
   - Export en JSON/CSV

5. **Outil de monitoring**
   - Vérifier services (subprocess)
   - Alerter si problème
   - Logger dans fichier avec rotation

---

**Félicitations !** Vous avez maintenant toutes les bases essentielles pour le scripting Python. Ce chapitre est crucial pour votre entretien !

**Prochaine étape :** Chapitre 7 - Fonctionnalités Avancées
