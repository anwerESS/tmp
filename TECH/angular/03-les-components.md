# 🧩 Chapitre 3 : Les Components (Composants)

## 🎯 Points Clés pour l'Entretien

- Un **component** est une brique réutilisable de l'application
- Composé de **4 fichiers** : .ts, .html, .scss, .spec.ts
- Le **@Component** decorator définit le comportement
- **Selector** : nom de la balise HTML pour utiliser le composant
- Architecture par **composition** de composants

---

## 📖 Qu'est-ce qu'un Component ?

### Définition
Un composant est une brique de votre application qui gère :
- Le **contenu** (HTML)
- Le **style** (CSS/SCSS)
- Le **comportement** (TypeScript)
- Les **tests** (Spec)

### Exemple d'Architecture

```
Collection App
├── Collection Component
│   ├── Search Bar Component
│   └── Collection Item Component (x N)
└── Header Component
```

---

## 🏗️ Structure d'un Component

### Les 4 Fichiers

```
component-name/
├── component-name.component.ts       # Logique ⭐
├── component-name.component.html     # Template
├── component-name.component.scss     # Styles
└── component-name.component.spec.ts  # Tests
```

> 💡 **Note** : Seul le fichier `.ts` est obligatoire !

---

## 🔨 Créer un Component

### Commande Complète

```bash
ng generate component <nom>
```

### Commande Courte (recommandée)

```bash
ng g c components/collection-item-card
```

### Sans fichier de test

```bash
ng g c components/collection-item-card --skip-tests
```

---

## 📝 Anatomie d'un Component Minimal

### Version la Plus Simple

```typescript
import { Component } from '@angular/core';

@Component({
  selector: 'app-root',
  template: '<h1>Hello World</h1>',
})
export class AppComponent {
}
```

### Avec Styles Inline

```typescript
import { Component } from '@angular/core';

@Component({
  selector: 'app-root',
  template: '<h1>Hello World</h1>',
  styles: `
    h1 {
      background-color: black;
      color: white;
    }
  `
})
export class AppComponent {
}
```

---

## 🎨 Component avec Fichiers Externes

### Structure Recommandée

```typescript
import { Component } from '@angular/core';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss'
})
export class AppComponent {
}
```

### app.component.html

```html
<h1>Hello World</h1>
```

### app.component.scss

```scss
h1 {
  background-color: black;
  color: white;
}
```

---

## 🔗 Utiliser un Component

### Étape 1 : Importer le Component

```typescript
import { Component } from '@angular/core';
import { CollectionItemCard } from './components/collection-item-card/collection-item-card';

@Component({
  selector: 'app-root',
  imports: [CollectionItemCard],  // ⭐ Important !
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss'
})
export class AppComponent {
}
```

### Étape 2 : Utiliser dans le Template

```html
<app-collection-item-card></app-collection-item-card>
```

> ⚠️ **Important** : Le nom du selector définit le nom de la balise HTML

---

## 🎯 Exemple Pratique : Collection Item Card

### TypeScript (collection-item-card.component.ts)

```typescript
import { Component } from '@angular/core';

@Component({
  selector: 'app-collection-item-card',
  imports: [],
  templateUrl: './collection-item-card.component.html',
  styleUrl: './collection-item-card.component.scss'
})
export class CollectionItemCard {
}
```

### HTML (collection-item-card.component.html)

```html
<article class="collection-item-card">
  <span class="rarity-chip">Legendary</span>

  <figure class="item-image">
    <img src="img/linx.png" alt="Excalibur Sword" />
  </figure>

  <header class="item-header">
    <h2 class="item-name">Excalibur</h2>
  </header>

  <p class="item-description">
    A legendary sword of unmatched sharpness and history.
  </p>
  
  <footer>
    <p class="item-price">$199</p>
  </footer>
</article>
```

### SCSS (collection-item-card.component.scss)

```scss
.collection-item-card {
  display: flex;
  flex-direction: column;
  background: #f0f6ff;
  border: 1px solid #8ca3c7;
  border-radius: 12px;
  padding: 1rem;
  width: 16rem;
  height: 21rem;
  cursor: pointer;
  transition: transform 0.2s ease, box-shadow 0.2s ease;
}

.collection-item-card:hover {
  transform: translateY(-4px);
  box-shadow: 0 6px 12px rgba(0, 0, 0, 0.15);
}

.rarity-chip {
  position: absolute;
  top: 0.75rem;
  left: 0.75rem;
  background: #4a6cf7;
  color: #fff;
  font-size: 0.75rem;
  font-weight: 600;
  padding: 0.25rem 0.6rem;
  border-radius: 999px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.15);
}

.item-image {
  text-align: center;
  flex-shrink: 0;
}

.item-image img {
  max-width: 100%;
  max-height: 10rem;
  border-radius: 8px;
}

.item-name {
  font-size: 1.25rem;
  font-weight: 700;
  margin: 0;
  color: #1e2a3a;
}

.item-description {
  margin: 0.75rem 0;
  color: #334155;
  font-size: 0.95rem;
  line-height: 1.4;
  flex-grow: 1;
}

.item-price {
  font-size: 1.1rem;
  font-weight: 600;
  color: #1e8a3a;
  margin: 0;
}
```

---

## 📋 Paramètres du @Component Decorator

| Paramètre | Type | Description |
|-----------|------|-------------|
| `selector` | string | Nom de la balise HTML |
| `template` | string | HTML inline |
| `templateUrl` | string | Chemin vers fichier HTML |
| `styles` | string | CSS inline |
| `styleUrl` | string | Chemin vers fichier CSS |
| `imports` | array | Composants/modules importés |

---

## 🏛️ Structure de Dossiers (Ce Cours)

```
src/app/
├── components/           # Tous les composants
│   ├── collection-item-card/
│   ├── search-bar/
│   └── ...
├── models/               # Classes/interfaces
├── services/             # Services
└── app.component.ts      # Composant racine
```

> ⚠️ **Note** : Cette structure est pédagogique. En production, préférer une organisation par **fonctionnalités** (feature-based structure).

---

## 🎓 Concepts Clés pour l'Entretien

### Questions Fréquentes

**Q: Quels fichiers sont obligatoires dans un component ?**
- Seul le fichier `.ts` est obligatoire
- Les autres peuvent être inline ou externes

**Q: À quoi sert le selector ?**
- Définit le nom de la balise HTML pour utiliser le composant
- Exemple : `selector: 'app-card'` → `<app-card></app-card>`

**Q: Peut-on avoir plusieurs components dans un fichier ?**
- Techniquement oui, mais **fortement déconseillé**
- Un component = un fichier pour la maintenabilité

**Q: Différence entre template et templateUrl ?**
- `template` : HTML inline (pour contenu court)
- `templateUrl` : HTML externe (recommandé)

---

## 🔄 Cycle de Vie d'un Component

### Création

```bash
ng g c mon-composant
```

### Utilisation

```typescript
imports: [MonComposant]  // Dans le parent
```

```html
<app-mon-composant></app-mon-composant>
```

---

## 💡 Bonnes Pratiques

1. ✅ **Un component = une responsabilité**
2. ✅ **Nommer les selectors avec le préfixe app-**
3. ✅ **Utiliser des fichiers externes pour HTML/CSS**
4. ✅ **Garder les components petits et réutilisables**
5. ✅ **Commenter le code complexe**

### Mauvaises Pratiques

1. ❌ Components trop larges (>500 lignes)
2. ❌ Logique métier dans les components
3. ❌ HTML/CSS inline pour contenu long
4. ❌ Selector sans préfixe

---

## 📝 Checklist Component

- [ ] Component créé avec `ng g c`
- [ ] Selector unique et descriptif
- [ ] HTML structuré et sémantique
- [ ] SCSS organisé avec classes
- [ ] Importé dans le component parent
- [ ] Utilisé dans le template parent
- [ ] Testé dans le navigateur

---

## 🎯 Exercice Pratique

Créer un component **UserCard** qui affiche :
- Photo de profil
- Nom et prénom
- Email
- Badge de statut (actif/inactif)

```bash
ng g c components/user-card
```

---

## 🔍 Debugging Components

### Outils DevTools Angular

```bash
# Installer Angular DevTools (Chrome Extension)
```

### Console Inspection

```typescript
console.log('Component initialisé');
```

### Template Debugging

```html
<pre>{{ variable | json }}</pre>
```

---

## ⚠️ Erreurs Courantes

| Erreur | Cause | Solution |
|--------|-------|----------|
| `Can't bind to 'X'` | Import manquant | Vérifier les imports |
| `Unknown element` | Selector incorrect | Vérifier le selector |
| Styles non appliqués | Encapsulation | Vérifier styleUrl |
