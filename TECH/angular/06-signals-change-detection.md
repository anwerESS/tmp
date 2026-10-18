# ⚡ Chapitre 6 : Les Signaux et la Détection de Changement

## 🎯 Points Clés pour l'Entretien

- **Signal** : Valeur réactive qui notifie automatiquement Angular des changements
- **signal()** : Créer une valeur modifiable
- **computed()** : Créer une valeur dérivée qui dépend d'autres signals
- **effect()** : Exécuter du code automatiquement quand les signals changent
- **Zone.js** vs **Zoneless** : Deux approches de détection de changement
- **OnPush Strategy** : Stratégie de détection optimisée

---

## 📖 Qu'est-ce qu'un Signal ?

### Définition
Un **signal** est une valeur réactive qui notifie automatiquement Angular de ses changements. Contrairement à une variable classique, Angular sait immédiatement quand un signal est modifié et peut mettre à jour l'interface en conséquence.

### Les 3 Primitives de Base

Introduites dans Angular 16 :

1. **signal()** - Valeur réactive modifiable
2. **computed()** - Valeur dérivée automatique
3. **effect()** - Exécution de code sur changement

---

## 🔨 La Primitive signal()

### Créer un Signal

```typescript
import { Component, signal } from '@angular/core';

export class App {
  selectedItemIndex = signal(0);  // Valeur initiale: 0
}
```

### Type de Retour

```typescript
selectedItemIndex: WritableSignal<number> = signal(0);
```

---

## 📖 Accéder à un Signal

### Dans TypeScript

```typescript
const currentIndex = this.selectedItemIndex();  // Appel comme une fonction
```

### Dans le Template

```html
<p>Index actuel: {{ selectedItemIndex() }}</p>
<app-item [item]="collectionItems[selectedItemIndex()]"></app-item>
```

> ⚠️ **Important** : Toujours utiliser les parenthèses `()` pour lire un signal

---

## ✏️ Modifier un Signal

### Méthode set()

Assigne une nouvelle valeur directement :

```typescript
// Assigner une valeur fixe
this.selectedItemIndex.set(1);

// Calculer puis assigner
const currentIndex = this.selectedItemIndex();
this.selectedItemIndex.set((currentIndex + 1) % 2);
```

### Méthode update()

Modifie basé sur la valeur actuelle :

```typescript
this.selectedItemIndex.update(currentIndex => (currentIndex + 1) % 2);
```

**Différence clé** :
- `set()` : Nouvelle valeur complète
- `update()` : Fonction de transformation

---

## 🔄 La Primitive computed()

### Définition
**computed()** crée un signal dérivé qui se recalcule automatiquement quand ses dépendances changent.

### Exemple Basique

```typescript
import { computed, signal } from '@angular/core';

export class App {
  collectionItems: CollectionItem[] = [...];
  selectedItemIndex = signal(0);
  
  // Se recalcule automatiquement quand selectedItemIndex change
  selectedItem = computed(() => 
    this.collectionItems[this.selectedItemIndex()]
  );
}
```

### Utilisation dans le Template

```html
<app-collection-item-card [item]="selectedItem()"></app-collection-item-card>
```

### Caractéristiques

- ✅ **Lecture seule** : Pas de `.set()` ou `.update()`
- ✅ **Lazy** : Recalculé uniquement quand nécessaire
- ✅ **Mémorisé** : Cache la valeur si aucune dépendance n'a changé
- ✅ **Composable** : Peut dépendre d'autres computed()

---

## ⚙️ La Primitive effect()

### Définition
**effect()** exécute du code automatiquement quand les signals qu'il utilise changent.

### Cas d'Usage Principaux

1. **Logging** - Déboguer les changements
2. **Synchronisation** - Sauvegarder dans localStorage

### Exemple : Logging

```typescript
import { effect } from '@angular/core';

export class App {
  selectedItemIndex = signal(0);
  selectedItem = computed(() => 
    this.collectionItems[this.selectedItemIndex()]
  );

  loggingEffect = effect(() => {
    console.log('Index:', this.selectedItemIndex());
    console.log('Item:', this.selectedItem());
  });
}
```

### Exemple : LocalStorage Sync

```typescript
loggingEffect = effect(() => {
  const data = this.myData();
  localStorage.setItem('myData', JSON.stringify(data));
});
```

> ⚠️ **ATTENTION** : Ne jamais modifier un signal dans un effect() !

---

## 🔍 Détection de Changement : Zone.js

### Fonctionnement avec Zone.js

Zone.js intercepte tous les événements :
- Clics utilisateur
- Saisies clavier
- Appels HTTP
- setTimeout/setInterval

Angular vérifie ensuite tous les composants potentiellement affectés.

### Stratégie par Défaut (Default)

```
Événement détecté
    ↓
Vérification du Component Root
    ↓
Vérification de TOUS les enfants
```

**Avantage** : Fonctionnement garanti  
**Inconvénient** : Peut être lent avec beaucoup de composants

---

## ⚡ Stratégie OnPush

### Activation

```typescript
import { ChangeDetectionStrategy } from '@angular/core';

@Component({
  selector: 'app-my-component',
  changeDetection: ChangeDetectionStrategy.OnPush,  // ⭐
  templateUrl: './my-component.html'
})
export class MyComponent {
}
```

### Conditions de Vérification

Le composant est vérifié UNIQUEMENT si :

1. ✅ Un **signal** (input, signal, computed) change
2. ✅ Un **événement** se produit dans le composant
3. ✅ Un **AsyncPipe** reçoit une nouvelle valeur
4. ✅ **markForCheck()** est appelé manuellement

> 💡 **Comparaison par référence** : Angular compare les objets par référence, pas par contenu

---

## 🚀 Mode Zoneless

### Qu'est-ce que Zoneless ?

Angular **sans Zone.js** (disponible depuis Angular 20.2) :
- Plus de dépendance à Zone.js
- Détection basée sur les **notifications explicites**
- Meilleures performances

### Notifications en Mode Zoneless

1. **Signals** : Changements de `signal()`, `computed()`
2. **Inputs** : Modifications des inputs de composants
3. **Événements** : Clics, saisies dans le template
4. **AsyncPipe** : Nouvelles valeurs async
5. **markForCheck()** : Appel manuel

### Activation Zoneless

Dans `app.config.ts` :

```typescript
import { ApplicationConfig } from '@angular/platform-browser';
import { provideExperimentalZonelessChangeDetection } from '@angular/core';

export const appConfig: ApplicationConfig = {
  providers: [
    provideExperimentalZonelessChangeDetection()  // ⭐
  ]
};
```

> 💡 **Note** : Si créé avec les options du cours, déjà en zoneless !

---

## 📊 Comparaison des Stratégies

| Stratégie | Quand Vérifier | Performance | Complexité |
|-----------|----------------|-------------|------------|
| **Default** | Toujours tous les composants | ⚠️ Moyenne | ✅ Simple |
| **OnPush** | Seulement si marqué | ✅ Bonne | ⚠️ Attention requise |
| **Zoneless + OnPush** | Notifications explicites | ⭐ Excellente | ⚠️ Discipline requise |

---

## 🎯 Exemple Complet avec Tous les Concepts

```typescript
import { 
  Component, 
  signal, 
  computed, 
  effect,
  ChangeDetectionStrategy 
} from '@angular/core';

@Component({
  selector: 'app-root',
  templateUrl: './app.html',
  changeDetection: ChangeDetectionStrategy.OnPush
})
export class App {
  // Signal de base
  collectionItems: CollectionItem[] = [/* ... */];
  selectedItemIndex = signal(0);
  
  // Computed signal (dérivé)
  selectedItem = computed(() => 
    this.collectionItems[this.selectedItemIndex()]
  );
  
  // Effect (logging)
  loggingEffect = effect(() => {
    console.log('Selected:', this.selectedItem().name);
  });
  
  // Méthode de modification
  toggleItem() {
    this.selectedItemIndex.update(index => (index + 1) % 2);
  }
}
```

```html
<!-- app.html -->
<app-collection-item-card [item]="selectedItem()"></app-collection-item-card>
<button (click)="toggleItem()">Toggle</button>
```

---

## 🎓 Concepts Clés pour l'Entretien

### Questions Fréquentes

**Q: Quelle est la différence entre signal() et computed() ?**
- `signal()` : Valeur modifiable avec `.set()` et `.update()`
- `computed()` : Lecture seule, recalculé automatiquement

**Q: Quand utiliser effect() ?**
- Logging et debugging
- Synchronisation avec APIs externes (localStorage, etc.)
- **Jamais** pour modifier d'autres signals

**Q: Qu'est-ce que Zone.js ?**
- Bibliothèque qui intercepte les événements asynchrones
- Permet à Angular de détecter automatiquement les changements
- Peut être remplacée par le mode Zoneless

**Q: Pourquoi utiliser OnPush ?**
- Améliore les performances
- Force une architecture plus propre
- Réduit les vérifications inutiles

**Q: Comment fonctionne le mode Zoneless ?**
- Pas d'interception automatique
- Détection basée sur les signals et événements explicites
- Nécessite l'utilisation de signals et d'OnPush

---

## 💡 Bonnes Pratiques

1. ✅ **Préférer signals** aux variables classiques pour l'état
2. ✅ **Utiliser computed()** pour les valeurs dérivées
3. ✅ **OnPush partout** en mode Zoneless
4. ✅ **effect() uniquement** pour les effets de bord
5. ✅ **Pas de mutation** des objets dans les signals

### Anti-Patterns

1. ❌ Modifier des signals dans effect()
2. ❌ Oublier les () pour lire un signal
3. ❌ Utiliser computed() pour des effets de bord
4. ❌ Mélanger Zone.js et Zoneless

---

## 📝 Checklist Signals & Change Detection

- [ ] Comprendre signal(), computed(), effect()
- [ ] Savoir lire un signal avec ()
- [ ] Utiliser set() et update() correctement
- [ ] Appliquer ChangeDetectionStrategy.OnPush
- [ ] Comprendre Zone.js vs Zoneless
- [ ] Ne jamais muter des signals dans effect()

---

## 🔧 Debugging

### Afficher la Valeur d'un Signal

```html
<pre>{{ mySignal() | json }}</pre>
```

### Utiliser Effect pour Logging

```typescript
effect(() => {
  console.log('Signal changed:', this.mySignal());
});
```

### Angular DevTools

Installer l'extension Chrome "Angular DevTools" pour :
- Inspecter les composants
- Voir les signals
- Profiler la détection de changement

---

## ⚠️ Erreurs Courantes

| Erreur | Cause | Solution |
|--------|-------|----------|
| `X is not a function` | Oubli des () | `signal()` pas `signal` |
| Pas de mise à jour UI | OnPush sans signal | Utiliser signals |
| Boucle infinie | Mutation dans effect | Lire seulement |
| Performance dégradée | Trop d'effects | Limiter les effects |

---

## 🚀 Exercice Pratique

Créer un compteur avec :
- Signal pour le count
- Computed pour doubleCount
- Effect pour logger les changements
- Boutons +/- avec OnPush

```typescript
export class Counter {
  count = signal(0);
  doubleCount = computed(() => this.count() * 2);
  
  logEffect = effect(() => {
    console.log('Count:', this.count());
  });
  
  increment() {
    this.count.update(c => c + 1);
  }
  
  decrement() {
    this.count.update(c => c - 1);
  }
}
```

---

## 📚 Résumé

**Signals = Réactivité Simple et Performante**

- `signal()` → Valeur modifiable
- `computed()` → Valeur dérivée
- `effect()` → Effets de bord
- **OnPush** → Optimisation
- **Zoneless** → Performance maximale
