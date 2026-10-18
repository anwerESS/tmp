# 🔌 Chapitre 4 : Les Signal Inputs

## 🎯 Points Clés pour l'Entretien

- **Input** : Passer des données du parent vers l'enfant
- **Signal Input** : Nouvelle API réactive d'Angular
- **input()** : Fonction qui retourne un InputSignal
- **input.required()** : Input obligatoire sans valeur par défaut
- Accès via **appel de fonction** : `nom()`

---

## 📖 Qu'est-ce qu'un Input ?

### Définition
Un **input** permet de passer des valeurs d'un composant parent vers un composant enfant.

### Analogie
```
Parent Component → [data] → Child Component
```

---

## 🏗️ Créer des Propriétés dans un Component

### Sans Input (données hard-codées)

```typescript
export class CollectionItemCard {
  name = "Excalibur";
  description = "A legendary sword...";
  rarity = "Legendary";
  price = 250;
}
```

### Utilisation dans le Template

```html
<h2>{{name}}</h2>
<p>{{description}}</p>
<span>{{rarity}}</span>
<p>{{price}} €</p>
```

> 💡 **Double Accolades** : `{{ }}` pour afficher une propriété TypeScript

---

## 🔄 Transformer en Signal Inputs

### Import Nécessaire

```typescript
import { Component, input } from '@angular/core';
```

### Définition des Inputs

```typescript
export class CollectionItemCard {
  name = input("Linx");                    // Valeur par défaut
  description = input("A legendary sword...");
  rarity = input("Legendary");
  price = input(250);
}
```

### Utilisation dans le Template (avec parenthèses!)

```html
<h2>{{name()}}</h2>
<p>{{description()}}</p>
<span>{{rarity()}}</span>
<p>{{price()}} €</p>
```

> ⚠️ **Crucial** : Les signal inputs s'appellent comme des fonctions : `name()`

---

## 📝 Passer des Valeurs aux Inputs

### Parent Component (app.component.html)

```html
<!-- Sans crochets = string -->
<app-collection-item-card name="Héro"></app-collection-item-card>

<!-- Avec crochets = expression TypeScript -->
<app-collection-item-card [price]="20"></app-collection-item-card>

<!-- Les deux combinés -->
<app-collection-item-card 
  name="Héro" 
  [price]="20"
></app-collection-item-card>
```

### Règle Importante

| Syntaxe | Type Passé | Exemple |
|---------|------------|---------|
| `attr="value"` | String uniquement | `name="Hero"` |
| `[attr]="value"` | Expression TS | `[price]="20"` |

---

## 🎯 Exemple avec Objet Complet

### Créer un Modèle

**collection-item.ts**
```typescript
export class CollectionItem {
  name = "Linx";
  description = "A legendary sword...";
  image = "img/linx.png"
  rarity = "Legendary";
  price = 250;
}
```

### Component avec Input d'Objet

```typescript
import { Component, input } from '@angular/core';
import { CollectionItem } from '../../models/collection-item';

@Component({
  selector: 'app-collection-item-card',
  imports: [],
  templateUrl: './collection-item-card.component.html',
  styleUrl: './collection-item-card.component.scss'
})
export class CollectionItemCard {
  item = input(new CollectionItem());
}
```

### Template Utilisant l'Objet

```html
<article class="collection-item-card">
  <span class="rarity-chip">{{item().rarity}}</span>
  <img [src]="item().image" alt="Item" />
  <h2>{{item().name}}</h2>
  <p>{{item().description}}</p>
  <p>{{item().price}} €</p>
</article>
```

> 💡 **Property Binding** : `[src]="item().image"` pour lier une propriété HTML

---

## 📤 Utilisation dans le Parent

### Parent TypeScript (app.component.ts)

```typescript
export class App {
  coin!: CollectionItem;
  linx!: CollectionItem;
  
  constructor() {
    this.coin = new CollectionItem();
    this.coin.name = 'Pièce de 1972';
    this.coin.price = 170;
    
    this.linx = new CollectionItem();
    // Garde les valeurs par défaut
  }
}
```

> ⚠️ **Le !** signale à TypeScript qu'on initialisera la propriété (ici dans le constructor)

### Parent Template (app.component.html)

```html
<section class="collection-grid">
  <app-collection-item-card [item]="linx"></app-collection-item-card>
  <app-collection-item-card [item]="coin"></app-collection-item-card>
</section>
```

---

## 🔧 Fonctionnalités Avancées

### 1. Typer un Input sans Valeur Par Défaut

```typescript
item = input<CollectionItem>();  // Type explicite
```

### 2. Type de Retour

```typescript
// InputSignal est le type retourné par input()
item: InputSignal<CollectionItem> = input<CollectionItem>();
```

### 3. Input Obligatoire (Required)

```typescript
item = input.required<CollectionItem>();
```

**Différences avec input() :**
- ✅ Aucune valeur par défaut possible
- ✅ Erreur de compilation si non fourni
- ✅ Type doit être explicite

```html
<!-- ❌ Erreur : input obligatoire manquant -->
<app-collection-item-card></app-collection-item-card>

<!-- ✅ OK -->
<app-collection-item-card [item]="coin"></app-collection-item-card>
```

---

## 🎨 Utilisation Avancée : Alias

### Définir un Alias

```typescript
item = input.required<CollectionItem>({
  alias: 'collection-item'
});
```

### Utilisation avec Alias

```html
<app-collection-item-card 
  [collection-item]="coin"
></app-collection-item-card>
```

> 💡 **Quand utiliser** : Exposer un nom différent de la propriété interne

---

## 🔄 Utilisation Avancée : Transform

### Transformer la Valeur

```typescript
item = input.required<CollectionItem, CollectionItem>({
  transform: (value: CollectionItem) => {
    value.price = value.price * 1.17;  // Conversion € -> $
    return value;
  }
});
```

### Types dans Transform

```typescript
input.required<TypeRetour, TypeEntree>({
  transform: (value: TypeEntree) => TypeRetour
});
```

### Exemple : Extraire une Seule Propriété

```typescript
price = input.required<number, CollectionItem>({
  transform: (value: CollectionItem) => {
    return value.price * 1.17;
  }
});
```

---

## 📊 Comparaison des Syntaxes

| Syntaxe | Valeur Par Défaut | Obligatoire | Type Inféré |
|---------|-------------------|-------------|-------------|
| `input("default")` | ✅ | ❌ | ✅ |
| `input<T>()` | ❌ | ❌ | ❌ (explicite) |
| `input.required<T>()` | ❌ | ✅ | ❌ (explicite) |

---

## 🎓 Concepts Clés pour l'Entretien

### Questions Fréquentes

**Q: Quelle est la différence entre input() et input.required() ?**
- `input()` : optionnel, avec valeur par défaut
- `input.required()` : obligatoire, sans valeur par défaut

**Q: Pourquoi utiliser des parenthèses pour accéder à un signal input ?**
- Les signals sont des fonctions réactives
- `name()` retourne la valeur actuelle du signal

**Q: Quand utiliser les crochets [] ?**
- Pour passer des expressions TypeScript (nombres, objets, variables)
- Sans crochets = toujours une string

**Q: Peut-on modifier un input depuis l'enfant ?**
- ❌ Non ! Les inputs sont **read-only**
- Utiliser des **outputs** pour communiquer vers le parent

---

## 💡 Bonnes Pratiques

1. ✅ **Utiliser input.required()** pour les données essentielles
2. ✅ **Typer explicitement** les inputs complexes
3. ✅ **Valeurs par défaut sensées** pour input()
4. ✅ **Noms descriptifs** pour les propriétés
5. ✅ **Transform pour conversions** simples uniquement

### Anti-Patterns

1. ❌ Modifier un input depuis l'enfant
2. ❌ Transform pour logique métier complexe
3. ❌ Oublier les parenthèses : `name()` et non `name`
4. ❌ Utiliser string sans crochets pour nombres

---

## 🔍 Debugging Inputs

### Afficher la Valeur dans le Template

```html
<pre>{{ item() | json }}</pre>
```

### Console dans TypeScript

```typescript
ngOnInit() {
  console.log('Item reçu:', this.item());
}
```

---

## 📝 Checklist Signal Inputs

- [ ] Import de `input` depuis @angular/core
- [ ] Définition avec `input()` ou `input.required()`
- [ ] Type explicite si pas de valeur par défaut
- [ ] Utilisation avec parenthèses dans le template
- [ ] Passage avec `[attr]` depuis le parent
- [ ] Vérification dans le navigateur

---

## 🎯 Exercice Pratique

Créer un component **ProductCard** avec les inputs suivants :
- `name` : string (obligatoire)
- `price` : number (obligatoire)
- `inStock` : boolean (défaut: true)
- `discount` : number (optionnel)

```typescript
export class ProductCard {
  name = input.required<string>();
  price = input.required<number>();
  inStock = input(true);
  discount = input<number>();
}
```

---

## ⚠️ Erreurs Courantes

| Erreur | Cause | Solution |
|--------|-------|----------|
| `TypeError: X is not a function` | Oubli des () | `name()` pas `name` |
| `Required input missing` | input.required non fourni | Passer [attr] |
| Type mismatch | Mauvais type passé | Vérifier le type |
| `Can't bind to 'X'` | Pas d'input défini | Créer l'input |

---

## 🚀 Prochaine Étape

Dans le prochain chapitre, nous verrons les **Outputs** pour communiquer de l'enfant vers le parent !
