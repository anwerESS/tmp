# 🔁 Chapitre 7 : Boucles et Conditions

## 🎯 Points Clés pour l'Entretien

- **@for** : Itérer sur des listes dans les templates
- **@empty** : Gérer les listes vides
- **@if / @else** : Conditions dans les templates
- **@switch / @case** : Multiple conditions
- **@let** : Déclarer des variables dans les templates
- **track** : Identifier les éléments pour l'optimisation

---

## 📖 Vue d'Ensemble

Angular propose une syntaxe de contrôle de flux moderne (depuis Angular 17) directement dans les templates HTML :

- **@for** - Boucles
- **@if / @else** - Conditions
- **@switch / @case** - Conditions multiples
- **@let** - Variables locales
- **@empty** - Gestion liste vide

---

## 🔄 Le Block @for

### Syntaxe de Base

```html
@for (item of items; track item.id) {
  <div>{{ item.name }}</div>
}
```

### Composants

| Partie | Description |
|--------|-------------|
| `item` | Variable de l'élément courant |
| `items` | Collection à parcourir |
| `track item.id` | Expression d'identification unique |

---

## 🎯 Exemple Complet : Liste d'Objets

### TypeScript

```typescript
export class App {
  displayedItems = computed(() => {
    const allItems = this.selectedCollection()?.items || [];
    return allItems.filter(item => 
      item.name.toLowerCase().includes(this.search().toLowerCase())
    );
  });
}
```

### Template

```html
<section class="collection-grid">
  @for (item of displayedItems(); track item.name) {
    <app-collection-item-card [item]="item"></app-collection-item-card>
  }
</section>
```

---

## 🔑 Le Paramètre track

### Pourquoi track est Important ?

**track** indique à Angular comment identifier chaque élément :
- ✅ **Réutilise** les composants existants
- ✅ **Évite** les re-créations inutiles
- ✅ **Améliore** les performances

### Exemples de track

```html
<!-- Avec ID unique (RECOMMANDÉ) -->
@for (item of items; track item.id) { }

<!-- Avec propriété unique -->
@for (item of items; track item.email) { }

<!-- Avec index (À ÉVITER sauf cas simple) -->
@for (item of items; track $index) { }
```

> ⚠️ **Important** : Toujours utiliser une propriété **vraiment unique**

---

## 📭 Le Block @empty

### Syntaxe

```html
@for (item of items; track item.id) {
  <div>{{ item.name }}</div>
} @empty {
  <div>Aucun résultat trouvé.</div>
}
```

### Quand s'exécute @empty ?

- Collection vide : `[]`
- Collection nulle : `null`
- Collection undefined : `undefined`

---

## 🔀 Les Blocks @if et @else

### Syntaxe de Base

```html
@if (condition) {
  <div>Condition vraie</div>
} @else {
  <div>Condition fausse</div>
}
```

### Exemple : Affichage Conditionnel

```html
@if (displayedItems().length > 0) {
  <div>{{ displayedItems().length }} objet(s) affiché(s).</div>
} @else {
  <div>Aucun résultat.</div>
}
```

---

## 🔢 Conditions Multiples avec @else if

```html
@if (score >= 90) {
  <div class="grade-a">Excellent!</div>
} @else if (score >= 70) {
  <div class="grade-b">Bien</div>
} @else if (score >= 50) {
  <div class="grade-c">Passable</div>
} @else {
  <div class="grade-f">Insuffisant</div>
}
```

---

## 📦 Le Block @let

### Définition
**@let** permet de déclarer des variables locales dans le template.

### Syntaxe

```html
@let variableName = expression;
```

### Exemple Basique

```html
@let displayedItemsCount = displayedItems().length;

@if (displayedItemsCount > 0) {
  <div>{{ displayedItemsCount }} objet(s) affiché(s).</div>
} @else {
  <div>Aucun résultat.</div>
}
```

### Avantages

1. ✅ **Évite les répétitions** dans le template
2. ✅ **Améliore la lisibilité**
3. ✅ **Optimise** (évaluation unique)

---

## 🎛️ Le Block @switch

### Syntaxe

```html
@switch (expression) {
  @case (valeur1) {
    <!-- Contenu pour valeur1 -->
  }
  @case (valeur2) {
    <!-- Contenu pour valeur2 -->
  }
  @default {
    <!-- Contenu par défaut -->
  }
}
```

### Exemple : Affichage par Rareté

```html
@for (item of displayedItems(); track item.id) {
  @switch (item.rarity) {
    @case ("Legendary") {
      <div>
        <app-collection-item-card [item]="item"></app-collection-item-card>
        <hr class="gold">
      </div>
    }
    @case ("Rare") {
      <div>
        <app-collection-item-card [item]="item"></app-collection-item-card>
        <hr class="dashed">
      </div>
    }
    @default {
      <app-collection-item-card [item]="item"></app-collection-item-card>
    }
  }
}
```

---

## 🎨 Exemple Complet : Application de Collection

### TypeScript

```typescript
import { Component, computed, model, signal } from '@angular/core';

@Component({
  selector: 'app-root',
  templateUrl: './app.html',
  styleUrl: './app.scss'
})
export class App {
  search = model('');
  
  selectedCollection = signal<Collection | null>(null);
  
  displayedItems = computed(() => {
    const allItems = this.selectedCollection()?.items || [];
    return allItems.filter(item => 
      item.name.toLowerCase().includes(this.search().toLowerCase())
    );
  });
}
```

### Template Complet

```html
<header id="collection-header">
  <h1>{{ selectedCollection()?.title }}</h1>
  <div>
    <app-search-bar [(search)]="search"></app-search-bar>
  </div>
</header>

<section class="collection-grid">
  @for (item of displayedItems(); track item.id) {
    @switch (item.rarity) {
      @case ("Legendary") {
        <div>
          <app-collection-item-card [item]="item"></app-collection-item-card>
          <hr class="gold">
        </div>
      }
      @case ("Rare") {
        <div>
          <app-collection-item-card [item]="item"></app-collection-item-card>
          <hr class="dashed">
        </div>
      }
      @default {
        <app-collection-item-card [item]="item"></app-collection-item-card>
      }
    }
  }
</section>

@let displayedItemsCount = displayedItems().length;
@if (displayedItemsCount > 0) {
  <div class="centered">{{ displayedItemsCount }} objet(s) affiché(s).</div>
} @else {
  <div class="centered">Aucun résultat.</div>
}
```

---

## 📋 Variables Contextuelles dans @for

Angular fournit des variables contextuelles automatiques :

```html
@for (item of items; track item.id) {
  <div>
    Index: {{ $index }}
    Premier: {{ $first }}
    Dernier: {{ $last }}
    Pair: {{ $even }}
    Impair: {{ $odd }}
    Nombre total: {{ $count }}
  </div>
}
```

| Variable | Type | Description |
|----------|------|-------------|
| `$index` | number | Index de l'élément (0, 1, 2...) |
| `$first` | boolean | Premier élément |
| `$last` | boolean | Dernier élément |
| `$even` | boolean | Index pair |
| `$odd` | boolean | Index impair |
| `$count` | number | Nombre total d'éléments |

---

## 🎓 Concepts Clés pour l'Entretien

### Questions Fréquentes

**Q: Quelle est la différence entre @for et *ngFor ?**
- `@for` : Nouvelle syntaxe (Angular 17+), plus performante
- `*ngFor` : Ancienne syntaxe (toujours supportée)

**Q: Pourquoi le paramètre track est-il obligatoire ?**
- Identifie les éléments de manière unique
- Optimise le rendu en réutilisant les composants
- Évite les bugs visuels lors des mises à jour

**Q: Peut-on imbriquer des @for ?**
- Oui, sans limite de profondeur
- Chaque @for doit avoir son propre track

**Q: Différence entre @if et *ngIf ?**
- `@if` : Nouvelle syntaxe, plus claire
- `*ngIf` : Ancienne syntaxe, toujours valide

---

## 💡 Bonnes Pratiques

1. ✅ **Toujours utiliser track** avec une propriété unique
2. ✅ **@let pour variables réutilisées** dans le template
3. ✅ **@switch pour 3+ conditions** sur même valeur
4. ✅ **@empty pour UX** sur listes potentiellement vides
5. ✅ **Computed() pour listes filtrées**

### Anti-Patterns

1. ❌ track avec $index sur listes dynamiques
2. ❌ Logique complexe dans les conditions
3. ❌ @for sans track
4. ❌ Répéter les mêmes expressions

---

## 🔧 Patterns Avancés

### @for avec @let

```html
@for (item of items; track item.id) {
  @let itemPrice = item.price * 1.2;
  @let isExpensive = itemPrice > 100;
  
  <div [class.expensive]="isExpensive">
    {{ item.name }}: {{ itemPrice }}€
  </div>
}
```

### @if avec @let

```html
@let user = currentUser();
@if (user) {
  <div>Bonjour, {{ user.name }}!</div>
  <div>Email: {{ user.email }}</div>
} @else {
  <div>Veuillez vous connecter</div>
}
```

### @switch avec Variables

```html
@let status = order.status;
@switch (status) {
  @case ('pending') {
    <badge color="yellow">En attente</badge>
  }
  @case ('processing') {
    <badge color="blue">En cours</badge>
  }
  @case ('completed') {
    <badge color="green">Terminé</badge>
  }
  @default {
    <badge color="gray">Inconnu</badge>
  }
}
```

---

## 📝 Checklist Boucles et Conditions

- [ ] Utiliser @for au lieu de *ngFor
- [ ] Toujours définir track avec propriété unique
- [ ] Ajouter @empty pour listes potentiellement vides
- [ ] Utiliser @if/@else au lieu de *ngIf
- [ ] Déclarer variables répétées avec @let
- [ ] @switch pour conditions multiples sur même valeur

---

## 🎯 Exercice Pratique

Créer une liste de tâches avec :
- @for pour afficher les tâches
- @empty si liste vide
- @switch pour le statut (todo, doing, done)
- @let pour le nombre de tâches
- @if pour afficher un message

```html
<h2>Mes Tâches</h2>

@let totalTasks = tasks().length;
@let doneTasks = tasks().filter(t => t.status === 'done').length;

<div>{{ doneTasks }} / {{ totalTasks }} terminées</div>

@for (task of tasks(); track task.id) {
  @switch (task.status) {
    @case ('done') {
      <div class="task done">✓ {{ task.title }}</div>
    }
    @case ('doing') {
      <div class="task doing">⟳ {{ task.title }}</div>
    }
    @default {
      <div class="task todo">○ {{ task.title }}</div>
    }
  }
} @empty {
  <div>Aucune tâche pour le moment.</div>
}

@if (doneTasks === totalTasks && totalTasks > 0) {
  <div class="success">🎉 Toutes les tâches terminées!</div>
}
```

---

## ⚠️ Erreurs Courantes

| Erreur | Cause | Solution |
|--------|-------|----------|
| `track is required` | track manquant | Ajouter track avec ID unique |
| Rendu incorrect | track avec $index | Utiliser propriété unique |
| Variable not found | @let mal placée | Déclarer @let avant utilisation |
| Duplication de clé | track non unique | Vérifier l'unicité |

---

## 🚀 Migration depuis l'Ancienne Syntaxe

### Avant (Angular <17)

```html
<div *ngFor="let item of items; trackBy: trackById">
  {{ item.name }}
</div>

<div *ngIf="condition; else elseBlock">
  Condition vraie
</div>
<ng-template #elseBlock>
  Condition fausse
</ng-template>

<div [ngSwitch]="value">
  <div *ngSwitchCase="'a'">A</div>
  <div *ngSwitchCase="'b'">B</div>
  <div *ngSwitchDefault>Default</div>
</div>
```

### Après (Angular 17+)

```html
@for (item of items; track item.id) {
  {{ item.name }}
}

@if (condition) {
  Condition vraie
} @else {
  Condition fausse
}

@switch (value) {
  @case ('a') { A }
  @case ('b') { B }
  @default { Default }
}
```

---

## 📚 Résumé

**Nouvelle Syntaxe de Contrôle = Plus Simple et Performante**

- `@for` → Boucles avec track obligatoire
- `@if/@else` → Conditions claires
- `@switch/@case` → Multi-conditions
- `@let` → Variables locales
- `@empty` → Gestion liste vide

**Toujours utiliser track avec une propriété unique !**
