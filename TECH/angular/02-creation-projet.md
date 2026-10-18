# 🏗️ Chapitre 2 : Création d'un Projet Angular et Structure des Fichiers

## 🎯 Points Clés pour l'Entretien

- **`ng new`** : Commande pour créer un nouveau projet
- Structure de projet Angular standard
- Fichiers de configuration essentiels
- Différence entre fichiers `.json`, `.ts`, et configuration

---

## 🚀 Création d'un Nouveau Projet

### Commande de Création

```bash
ng new collection-manager
```

### Questions Interactives

1. **Autocomplétion** : Activer pour compléter les commandes Angular avec Tab
2. **Partage de données** : Choix personnel
3. **Format de stylesheet** : CSS, SCSS, Sass, ou Less
4. **SSR (Server-Side Rendering)** : Non pour ce cours d'introduction
5. **Zoneless** : ✅ **Activer** (important pour Angular moderne)
6. **Configuration IA** : Claude, Cursor, Junie, etc.

> 💡 **Tip** : SCSS est recommandé pour plus de flexibilité CSS

---

## 📂 Structure du Projet

### Dossiers Principaux

```
collection-manager/
├── .angular/           # Cache de build
├── .vscode/            # Configuration VS Code
├── node_modules/       # Dépendances installées
├── public/             # Ressources statiques (images, etc.)
├── src/                # Code source de l'application ⭐
│   ├── app/
│   ├── index.html
│   ├── main.ts
│   └── styles.scss
├── angular.json        # Configuration Angular
├── package.json        # Dépendances NPM
├── tsconfig.json       # Configuration TypeScript
└── README.md
```

---

## 📄 Fichiers de Configuration Importants

### angular.json
Configuration du projet Angular (compilation, chemins, options de build)

```json
{
  "projects": {
    "collection-manager": {
      "architect": {
        "build": { /* options de compilation */ }
      }
    }
  }
}
```

### package.json
Liste des dépendances du projet

```json
{
  "name": "collection-manager",
  "version": "0.0.0",
  "dependencies": {
    "@angular/core": "^20.3.0",
    "@angular/common": "^20.3.0"
  }
}
```

### package-lock.json
Versions exactes des dépendances (pour la reproductibilité)

### tsconfig.json
Configuration TypeScript générale

```json
{
  "compilerOptions": {
    "target": "ES2022",
    "module": "ES2022",
    "strict": true
  }
}
```

### tsconfig.app.json
Configuration TypeScript spécifique à l'application (hérite de tsconfig.json)

### tsconfig.spec.json
Configuration TypeScript pour les tests

---

## 📁 Le Dossier src/

### Fichiers Principaux

#### index.html
Page HTML principale où l'application est injectée

```html
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title>CollectionManager</title>
  <base href="/">
</head>
<body>
  <app-root></app-root>
</body>
</html>
```

> ⚠️ **Point Clé** : `<app-root>` est le point d'entrée de l'application

#### main.ts
Point d'entrée de l'application Angular

```typescript
import { bootstrapApplication } from '@angular/platform-browser';
import { appConfig } from './app/app.config';
import { AppComponent } from './app/app.component';

bootstrapApplication(AppComponent, appConfig)
  .catch((err) => console.error(err));
```

#### styles.scss
Styles CSS globaux de l'application

```scss
body {
  margin: 0;
  font-family: Arial, sans-serif;
}
```

---

## 📦 Le Dossier src/app/

### Structure d'un Composant (4 fichiers)

```
app.component.ts        # Logique du composant
app.component.html      # Template HTML
app.component.scss      # Styles du composant
app.component.spec.ts   # Tests unitaires
```

### app.component.ts - Structure de Base

```typescript
import { Component } from '@angular/core';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss'
})
export class AppComponent {
  title = 'collection-manager';
}
```

### app.config.ts
Configuration des dépendances et providers

```typescript
import { ApplicationConfig } from '@angular/core';
import { provideRouter } from '@angular/router';
import { routes } from './app.routes';

export const appConfig: ApplicationConfig = {
  providers: [provideRouter(routes)]
};
```

### app.routes.ts
Définition des routes de l'application

```typescript
import { Routes } from '@angular/router';

export const routes: Routes = [];
```

---

## 🏃 Lancer le Projet

### Commande de Développement

```bash
ng serve
```

**Par défaut** : http://localhost:4200/

### Options Utiles

```bash
ng serve --open          # Ouvre automatiquement le navigateur
ng serve --port 3000     # Change le port
ng serve --host 0.0.0.0  # Accessible sur le réseau local
```

---

## 🔍 Points d'Attention pour l'Entretien

### Questions Fréquentes

**Q: Quelle est la différence entre package.json et package-lock.json ?**
- `package.json` : Versions approximatives (^20.3.0)
- `package-lock.json` : Versions exactes pour reproductibilité

**Q: À quoi sert le dossier node_modules ?**
- Contient toutes les dépendances installées localement

**Q: Que fait ng serve ?**
- Compile l'application
- Lance un serveur de développement
- Active le hot-reload (rechargement automatique)

**Q: Qu'est-ce que le SSR ?**
- Server-Side Rendering : génère le HTML côté serveur
- Améliore le SEO et le temps de chargement initial
- Non activé dans ce cours de base

---

## 📝 Checklist de Création de Projet

- [ ] Projet créé avec `ng new`
- [ ] Options configurées (SCSS, Zoneless, etc.)
- [ ] Structure des dossiers comprise
- [ ] Fichiers de configuration identifiés
- [ ] Application lancée avec `ng serve`
- [ ] Page accessible sur http://localhost:4200

---

## 🎓 Concepts Clés à Maîtriser

1. **Structure de projet** : Dossiers src/, app/, public/
2. **Fichiers de configuration** : angular.json, tsconfig.json, package.json
3. **Point d'entrée** : main.ts → AppComponent → index.html
4. **Hot Reload** : Modifications automatiquement reflétées
5. **Build vs Dev** : `ng serve` pour dev, `ng build` pour prod

---

## ⚠️ Erreurs Courantes

| Erreur | Cause | Solution |
|--------|-------|----------|
| Port déjà utilisé | 4200 occupé | `ng serve --port 3000` |
| Module introuvable | Dépendance manquante | `npm install` |
| Erreur de compilation | Syntaxe TypeScript | Vérifier les types |
