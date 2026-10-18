# 📤 Chapitre 5 : Les Outputs et Input Models

## 🎯 Points Clés pour l'Entretien

- **Output** : Émettre des événements de l'enfant vers le parent
- **OutputEmitterRef** : Type pour les outputs
- **output()** : Fonction pour créer un output
- **emit()** : Méthode pour envoyer un événement
- **model()** : Combine input + output (two-way binding)
- **$event** : Variable contenant la valeur émise

---

## 📖 Rappel : Communication Parent ↔ Enfant

```
Parent → [Input] → Enfant    (Inputs: chapitre précédent)
Parent ← (Output) ← Enfant   (Outputs: ce chapitre)
```

---

## 🎯 Objectif : Barre de Recherche Interactive

### Fonctionnalités à Implémenter
1. ✅ Détecter le clic sur le bouton
2. ✅ Compter les clics
3. ✅ Récupérer le texte de recherche
4. ✅ Mettre à jour en temps réel

---

## 🏗️ Étape 1 : Créer le Component Search Bar

### Création

```bash
ng g c components/search-bar
```

### HTML (search-bar.component.html)

```html
<div class="search-box">
  <img src="img/search.png">
  <input 
    id="live-search" 
    type="search" 
    placeholder="Search..." 
    autocomplete="off"
  >
  <button>Search</button>
</div>
```

### SCSS (search-bar.component.scss)

```scss
.search-box {
  display: flex;
  align-items: center;
  background: #fff;
  border: 1px solid #ccc;
  border-radius: 10px;
  padding: 5px 10px;
  width: 300px;
}

.search-box img {
  width: 18px;
  height: 18px;
  margin-right: 8px;
  flex-shrink: 0;
}

.search-box input {
  border: none;
  outline: none;
  flex: 1;
  font-size: 16px;
}
```

---

## 🎪 Réagir aux Événements HTML Natifs

### Syntaxe Générale

```html
<element (eventName)="expression"></element>
```

### Exemple : Clic sur Bouton

```html
<button (click)="searchClick()">Search</button>
```

### Component TypeScript

```typescript
export class SearchBar {
  searchClick() {
    console.log("clicked");
  }
}
```

> 💡 **Parenthèses** : Indiquent qu'on écoute un événement

---

## 📤 Créer un Output Personnalisé

### Import Nécessaire

```typescript
import { Component, output, OutputEmitterRef } from '@angular/core';
```

### Définir l'Output

```typescript
export class SearchBar {
  searchButtonClicked: OutputEmitterRef<void> = output<void>();

  searchClick() {
    this.searchButtonClicked.emit();
  }
}
```

### Composants de Base

1. **output<T>()** : Crée l'output avec le type T
2. **OutputEmitterRef<T>** : Type de l'output
3. **emit()** : Méthode pour émettre l'événement

---

## 🔗 Utiliser l'Output dans le Parent

### Parent Template (app.component.html)

```html
<app-search-bar 
  (searchButtonClicked)="increaseCount()"
></app-search-bar>

<p>Clicked Count: {{ count }}</p>
```

### Parent TypeScript (app.component.ts)

```typescript
export class App {
  count = 0;

  increaseCount() {
    this.count++;
  }
}
```

> 💡 **Convention** : Nom de l'event = nom de la propriété output

---

## 📊 Outputs avec Données

### Émettre une Valeur

```typescript
export class SearchBar {
  searchChange = output<string>();

  updateSearch(searchText: string) {
    this.searchChange.emit(searchText);
  }
}
```

### Recevoir la Valeur ($event)

```html
<app-search-bar 
  (searchChange)="search = $event"
></app-search-bar>

<p>Search: {{ search }}</p>
```

> ⚠️ **$event** : Variable spéciale contenant la valeur émise

---

## 🔄 FormsModule et ngModel

### Import FormsModule

```typescript
import { FormsModule } from '@angular/forms';

@Component({
  selector: 'app-search-bar',
  imports: [FormsModule],  // ⭐ Nécessaire pour ngModel
  templateUrl: './search-bar.component.html',
  styleUrl: './search-bar.component.scss'
})
export class SearchBar {
  search = input("Initial");
}
```

### Binding avec ngModel

```html
<input 
  [ngModel]="search()"
  (ngModelChange)="updateSearch($event)"
/>
```

### Composants de ngModel

- **[ngModel]** : Lie la valeur affichée (Input)
- **(ngModelChange)** : Détecte les changements (Output)

---

## 🎨 Exemple Complet : Search Bar

### TypeScript

```typescript
import { Component, input, output, OutputEmitterRef } from '@angular/core';
import { FormsModule } from '@angular/forms';

@Component({
  selector: 'app-search-bar',
  imports: [FormsModule],
  templateUrl: './search-bar.component.html',
  styleUrl: './search-bar.component.scss'
})
export class SearchBar {
  search = input("Initial");
  searchChange = output<string>();
  searchButtonClicked: OutputEmitterRef<void> = output<void>();

  searchClick() {
    this.searchButtonClicked.emit();
  }

  updateSearch(searchText: string) {
    this.searchChange.emit(searchText);
  }
}
```

### Template

```html
<div class="search-box">
  <img src="img/search.png">
  <input 
    [ngModel]="search()"
    (ngModelChange)="updateSearch($event)"
    type="search" 
    placeholder="Search..."
  />
  <button (click)="searchClick()">Search</button>
</div>
```

### Utilisation Parent

```html
<app-search-bar 
  (searchButtonClicked)="increaseCount()"
  [search]="search"
  (searchChange)="search = $event"
></app-search-bar>

<p>Search: {{ search }}</p>
<p>Clicked Count: {{ count }}</p>
```

---

## 🔄 Two-Way Binding

### Convention de Nommage

Pour qu'Angular reconnaisse le two-way binding :
- **Input** : `propertyName`
- **Output** : `propertyNameChange`

### Syntaxe Raccourcie

**Avant (verbose):**
```html
<app-search-bar 
  [search]="search"
  (searchChange)="search = $event"
></app-search-bar>
```

**Après (raccourci):**
```html
<app-search-bar 
  [(search)]="search"
></app-search-bar>
```

> 💡 **[(property)]** = **Banana in a Box** syntax

---

## 🎭 Alias pour les Outputs

### Définir un Alias

```typescript
searchButtonClicked: OutputEmitterRef<void> = output<void>({
  alias: 'submit'
});
```

### Utilisation avec Alias

```html
<app-search-bar 
  (submit)="increaseCount()"
></app-search-bar>
```

---

## 🚀 La Fonction model()

### Qu'est-ce que model() ?

`model()` combine automatiquement :
- Un **input**
- Un **output** avec le nom `propertyChange`

### Import

```typescript
import { Component, model } from '@angular/core';
```

### Définition

```typescript
export class SearchBar {
  search = model("Initial");  // Crée input + output
}
```

### Ce que ça Crée

```
search: InputSignal<string> ✅
searchChange: OutputEmitterRef<string> ✅
```

---

## 📝 Utilisation du Model

### Dans le Component Enfant

```typescript
import { Component, model, output, OutputEmitterRef } from '@angular/core';
import { FormsModule } from '@angular/forms';

@Component({
  selector: 'app-search-bar',
  imports: [FormsModule],
  templateUrl: './search-bar.component.html',
  styleUrl: './search-bar.component.scss'
})
export class SearchBar {
  search = model("Initial");
  searchButtonClicked: OutputEmitterRef<void> = output<void>({
    alias: 'submit'
  });

  searchClick() {
    this.searchButtonClicked.emit();
  }
}
```

### Template avec Model (simplifiée!)

```html
<div class="search-box">
  <img src="img/search.png">
  <input 
    [(ngModel)]="search"
    type="search" 
    placeholder="Search..."
  />
  <button (click)="searchClick()">Search</button>
</div>
```

> ⚠️ **Note** : On passe `search` **sans parenthèses** à ngModel

### Parent (utilisation identique)

```html
<app-search-bar 
  (submit)="increaseCount()"
  [(search)]="search"
></app-search-bar>
```

---

## 🔧 Modifier un Model

### Avec .set()

```typescript
updateSearch(searchText: string) {
  this.search.set(searchText);
}
```

### Avec .update()

```typescript
clearSearch() {
  this.search.update(() => "");
}
```

---

## 📊 Comparaison des Approches

| Approche | Verbosité | Flexibilité | Recommandé |
|----------|-----------|-------------|------------|
| Input + Output séparés | +++ | +++ | Oui (pour cases complexes) |
| Two-way binding | ++ | ++ | Oui (pour simplicité) |
| model() | + | ++ | ✅ Oui (Angular 17.2+) |

---

## 🎓 Concepts Clés pour l'Entretien

### Questions Fréquentes

**Q: Quelle est la différence entre Input et Output ?**
- Input : Parent → Enfant (données entrantes)
- Output : Enfant → Parent (événements sortants)

**Q: Qu'est-ce que le two-way binding ?**
- Combinaison d'input + output
- Syntaxe : `[(property)]`
- Convention : input + outputChange

**Q: Pourquoi utiliser model() ?**
- Simplifie le code (moins de boilerplate)
- Gère automatiquement l'input et l'output
- Disponible depuis Angular 17.2

**Q: Que contient $event ?**
- La valeur émise par l'output
- Type défini par `output<T>()`

---

## 💡 Bonnes Pratiques

1. ✅ **Nommer les outputs en PascalCase** avec suffixe explicite
   - `buttonClicked`, `valueChanged`, `formSubmitted`
2. ✅ **Utiliser model()** pour simplifier le two-way binding
3. ✅ **Typer les outputs** : `output<Type>()`
4. ✅ **Émettre des objets** pour plusieurs valeurs
5. ✅ **Documenter les outputs** dans les commentaires

### Anti-Patterns

1. ❌ Émettre trop fréquemment (performance)
2. ❌ Outputs non typés : `output<any>()`
3. ❌ Noms d'outputs non descriptifs : `event1`, `output`
4. ❌ Oublier d'appeler emit()

---

## 🔍 Debugging Outputs

### Console dans l'Enfant

```typescript
searchClick() {
  console.log('Emitting click event');
  this.searchButtonClicked.emit();
}
```

### Console dans le Parent

```typescript
handleClick() {
  console.log('Event received in parent');
  this.count++;
}
```

---

## 📝 Checklist Outputs

- [ ] Import `output` et `OutputEmitterRef`
- [ ] Définir l'output avec type
- [ ] Appeler `emit()` au bon moment
- [ ] Écouter l'event dans le parent avec ()
- [ ] Utiliser $event si valeur émise
- [ ] Tester dans le navigateur

---

## 🎯 Exercice Pratique

Créer un component **Counter** avec :
- Boutons +/- pour modifier un compteur
- Output `valueChange` quand la valeur change
- Output `maxReached` quand compteur atteint 10

```typescript
export class Counter {
  value = model(0);
  maxReached = output<void>();

  increment() {
    this.value.update(v => v + 1);
    if (this.value() >= 10) {
      this.maxReached.emit();
    }
  }
}
```

---

## ⚠️ Erreurs Courantes

| Erreur | Cause | Solution |
|--------|-------|----------|
| `emit is not a function` | Mauvais type | Utiliser OutputEmitterRef |
| Event non reçu | Mauvais nom event | Vérifier (eventName) |
| $event undefined | Pas de valeur émise | emit(value) pas emit() |
| FormsModule error | Import manquant | Ajouter FormsModule |

---

## 🚀 Résumé Final

### Input (Chapitre Précédent)
```typescript
data = input.required<T>();
```
```html
<child [data]="parentData"></child>
```

### Output (Ce Chapitre)
```typescript
dataChange = output<T>();
```
```html
<child (dataChange)="handleChange($event)"></child>
```

### Model (Best of Both)
```typescript
data = model<T>();
```
```html
<child [(data)]="parentData"></child>
```
