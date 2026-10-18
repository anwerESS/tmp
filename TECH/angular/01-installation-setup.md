# 📦 Chapitre 1 : Installation de Node.js, NPM, Angular et VS Code

## 🎯 Points Clés pour l'Entretien

- **Node.js** : Environnement d'exécution JavaScript côté serveur
- **NPM** : Gestionnaire de paquets pour installer et gérer les bibliothèques JavaScript
- **Angular** : Framework TypeScript pour créer des applications web modernes
- **TypeScript** : Sur-ensemble de JavaScript qui doit être trans-compilé

---

## 📚 Vocabulaire Essentiel

| Terme | Définition |
|-------|------------|
| **Node.js** | Environnement qui permet d'exécuter du JavaScript |
| **NPM** | Node Package Manager - Gestionnaire de dépendances |
| **Angular** | Framework pour créer des applications web en TypeScript |
| **TypeScript** | Langage de programmation, sur-ensemble de JavaScript |

> ⚠️ **Important** : TypeScript doit être trans-compilé en JavaScript pour être interprété par les navigateurs.

---

## 🔧 Installation sur Linux/Mac

### Étape 1 : Installer Curl

```bash
sudo apt update
sudo apt install curl
```

### Étape 2 : Installer NVM (Node Version Manager)

```bash
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash
source ~/.bashrc
nvm install --lts
```

### Étape 3 : Vérifier les versions

```bash
node --version
npm --version
```

---

## 🪟 Installation sur Windows

1. Télécharger l'installateur depuis : https://nodejs.org/en/download
2. Exécuter l'installateur et suivre les étapes
3. Vérifier l'installation :

```bash
node --version
npm --version
```

---

## ✅ Test de l'installation Node.js

### Créer un fichier hello.js

```javascript
console.log("Hello World !");
```

### Exécuter le fichier

```bash
node hello.js
```

### Créer un serveur HTTP simple

```javascript
const http = require('http');

http.createServer((req, res) => {
    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end('Hello World!');
}).listen(3000, 'localhost');
```

**Résultat** : Accessible sur http://localhost:3000

---

## 🚀 Installation d'Angular CLI

```bash
npm install -g @angular/cli
```

### Vérifier la version

```bash
ng version
```

> 💡 **Version minimale requise** : Angular 20.3 ou supérieur

---

## 💻 Installation de VS Code

1. Télécharger depuis : https://code.visualstudio.com/Download
2. Installer l'extension **"Angular Language Service"**
   - Ouvrir l'onglet Extensions (Ctrl+Shift+X)
   - Rechercher "Angular"
   - Installer "Angular Language Service"

### Extensions Recommandées

- Angular Language Service ✅
- Angular Snippets
- Prettier - Code formatter
- ESLint

---

## 📝 Checklist de Vérification

- [ ] Node.js installé et fonctionnel
- [ ] NPM installé et fonctionnel
- [ ] Angular CLI installé globalement
- [ ] VS Code installé avec l'extension Angular
- [ ] Test du serveur HTTP réussi

---

## 🎓 Points à Retenir pour l'Entretien

1. **NVM** permet de gérer plusieurs versions de Node.js
2. **npm install -g** installe un package globalement
3. **ng** est la commande CLI d'Angular
4. TypeScript améliore JavaScript avec le typage statique
5. VS Code avec Angular Language Service offre l'auto-complétion et la vérification de types

---

## ⚠️ Erreurs Courantes

| Problème | Solution |
|----------|----------|
| `npm: command not found` | Redémarrer le terminal après installation |
| `ng: command not found` | Vérifier l'installation globale avec `-g` |
| Permission denied | Utiliser `sudo` sur Linux/Mac |
