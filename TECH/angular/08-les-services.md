# 🛠️ Chapitre 8 : Les Services

## 🎯 Points Clés pour l'Entretien

- **Service** : Classe singleton injectable pour la logique métier
- **@Injectable** : Decorator pour rendre une classe injectable
- **inject()** : Fonction pour injecter un service
- **providedIn: 'root'** : Service disponible dans toute l'application
- **CRUD** : Create, Read, Update, Delete operations
- **localStorage** : Persister des données dans le navigateur

---

## 📖 Qu'est-ce qu'un Service ?

### Définition
Un **service** est une classe qui centralise :
- 📊 **Données** partagées entre composants
- 🔧 **Logique métier** réutilisable
- 🌐 **Appels API** et communication serveur

### Caractéristiques

- **Singleton** : Une seule instance dans l'application
- **Injectable** : Peut être injecté dans les composants
- **Réutilisable** : Partagé entre plusieurs composants

---

## 🏗️ Créer un Service

### Commande CLI

```bash
ng generate service services/collection-service
# OU
ng g s services/collection-service
```

### Fichier Généré

```typescript
import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class CollectionService {
  constructor() { }
}
```

---

## 🔍 Anatomie d'un Service

### @Injectable Decorator

```typescript
@Injectable({
  providedIn: 'root'  // ⭐ Singleton global
})
```

| Paramètre | Description |
|-----------|-------------|
| `'root'` | Service disponible partout (recommandé) |
| Module spécifique | Service limité à un module |

---

## 💉 Injecter un Service

### Avec inject()

**Méthode moderne (recommandée)** :

```typescript
import { Component, inject } from '@angular/core';
import { CollectionService } from './services/collection-service';

@Component({
  selector: 'app-root',
  templateUrl: './app.html'
})
export class App {
  collectionService = inject(CollectionService);  // ⭐
  
  constructor() {
    console.log(this.collectionService);
  }
}
```

### Avec Constructor Injection

**Méthode classique** :

```typescript
export class App {
  constructor(private collectionService: CollectionService) {
    console.log(this.collectionService);
  }
}
```

> 💡 **Préférer inject()** pour la nouvelle syntaxe Angular

---

## 🎯 Exemple Complet : Service CRUD

### Modèles de Données

```typescript
// collection-item.ts
export class CollectionItem {
  id = -1;
  name = "Linx";
  description = "A legendary sword...";
  image = "img/linx.png"
  rarity = "Legendary";
  price = 250;

  copy(): CollectionItem {
    return Object.assign(new CollectionItem(), this);
  }
}

// collection.ts
export class Collection {
  id = -1;
  title: string = "My Collection";
  items: CollectionItem[] = [];

  copy(): Collection {
    const copy = Object.assign(new Collection(), this);
    copy.items = this.items.map(item => item.copy());
    return copy;
  }
}
```

> 💡 **Méthode copy()** : Évite les modifications par référence

---

## 📝 Service CRUD Complet

### Structure de Base

```typescript
import { Injectable } from '@angular/core';
import { Collection } from '../models/collection';
import { CollectionItem } from '../models/collection-item';

@Injectable({
  providedIn: 'root'
})
export class CollectionService {
  private collections: Collection[] = [];
  private currentId = 1;
  private currentItemIndex: {[key: number]: number} = {};

  constructor() {
    this.generateDummyData();
  }
}
```

---

## 📖 Opérations READ

### getAll() - Récupérer toutes les collections

```typescript
getAll(): Collection[] {
  return this.collections.map(collection => collection.copy());
}
```

### get() - Récupérer une collection par ID

```typescript
get(collectionId: number): Collection | null {
  const storedCopy = this.collections.find(
    collection => collection.id === collectionId
  );

  if (!storedCopy) return null;
  return storedCopy.copy();
}
```

---

## ➕ Opérations CREATE

### add() - Ajouter une collection

```typescript
add(collection: Omit<Collection, 'id' | 'items'>): Collection {
  const storedCopy = collection.copy();
  storedCopy.id = this.currentId;
  this.collections.push(storedCopy);

  this.currentItemIndex[storedCopy.id] = 1;
  this.currentId++;

  return storedCopy.copy();
}
```

### addItem() - Ajouter un item à une collection

```typescript
addItem(collection: Collection, item: CollectionItem): Collection | null {
  const storedCollection = this.collections.find(
    c => c.id === collection.id
  );
  
  if (!storedCollection) return null;
  
  const storedItem = item.copy();
  storedItem.id = this.currentItemIndex[collection.id];
  storedCollection.items.push(storedItem);

  this.currentItemIndex[collection.id]++;

  return storedCollection.copy();
}
```

---

## ✏️ Opérations UPDATE

### update() - Mettre à jour une collection

```typescript
update(collection: Omit<Collection, 'items'>): Collection | null {
  const storedCopy = this.collections.find(
    c => c.id === collection.id
  );

  if (!storedCopy) return null;
  
  Object.assign(storedCopy, collection);
  return storedCopy.copy();
}
```

### updateItem() - Mettre à jour un item

```typescript
updateItem(collection: Collection, item: CollectionItem): Collection | null {
  const storedCollection = this.collections.find(
    c => c.id === collection.id
  );
  
  if (!storedCollection) return null;

  const storedItemIndex = storedCollection.items.findIndex(
    storedItem => storedItem.id === item.id
  );

  if (storedItemIndex === -1) return null;

  storedCollection.items[storedItemIndex] = item.copy();
  return storedCollection.copy();
}
```

---

## 🗑️ Opérations DELETE

### delete() - Supprimer une collection

```typescript
delete(collectionId: number): void {
  this.collections = this.collections.filter(
    collection => collection.id !== collectionId
  );
}
```

### deleteItem() - Supprimer un item

```typescript
deleteItem(collectionId: number, itemId: number): Collection | null {
  const storedCollection = this.collections.find(
    c => c.id === collectionId
  );
  
  if (!storedCollection) return null;

  storedCollection.items = storedCollection.items.filter(
    item => item.id !== itemId
  );

  return storedCollection.copy();
}
```

---

## 💾 Persistance avec localStorage

### Méthode save()

```typescript
private save() {
  localStorage.setItem('collections', JSON.stringify(this.collections));
}
```

### Méthode load()

```typescript
private load() {
  const collectionsJson = localStorage.getItem('collections');
  
  if (collectionsJson) {
    this.collections = JSON.parse(collectionsJson).map((collectionJson: any) => {
      const collection = Object.assign(new Collection(), collectionJson);
      const itemsJson = collectionJson['items'] || [];
      collection.items = itemsJson.map((item: any) => 
        Object.assign(new CollectionItem, item)
      );
      return collection;
    });
    
    // Recalculer les IDs
    this.currentId = Math.max(...this.collections.map(c => c.id)) + 1;
    this.currentItemIndex = this.collections.reduce(
      (indexes: {[key: number]: number}, collection) => {
        indexes[collection.id] = Math.max(...collection.items.map(i => i.id)) + 1;
        return indexes;
      }, {}
    );
  } else {
    this.generateDummyData();
    this.save();
  }
}
```

### Initialisation dans le Constructor

```typescript
constructor() {
  this.load();
}
```

---

## 📱 Utilisation dans un Component

### TypeScript

```typescript
import { Component, inject, signal } from '@angular/core';
import { CollectionService } from './services/collection-service';

@Component({
  selector: 'app-root',
  templateUrl: './app.html'
})
export class App {
  collectionService = inject(CollectionService);
  selectedCollection = signal<Collection | null>(null);
  
  constructor() {
    const allCollections = this.collectionService.getAll();
    if (allCollections.length > 0) {
      this.selectedCollection.set(allCollections[0]);
    }
  }

  addGenericItem() {
    const genericItem = new CollectionItem();
    const collection = this.selectedCollection();
    
    if (!collection) return;

    const updatedCollection = this.collectionService.addItem(
      collection, 
      genericItem
    );
    this.selectedCollection.set(updatedCollection);
  }
}
```

### Template

```html
<button (click)="addGenericItem()">Ajouter Objet</button>

@if (selectedCollection()) {
  <h2>{{ selectedCollection()!.title }}</h2>
  @for (item of selectedCollection()!.items; track item.id) {
    <app-item-card [item]="item"></app-item-card>
  }
}
```

---

## 🎓 Concepts Clés pour l'Entretien

### Questions Fréquentes

**Q: Pourquoi utiliser des services ?**
- Centraliser la logique métier
- Partager des données entre composants
- Faciliter les tests unitaires
- Séparer préoccupations (separation of concerns)

**Q: Qu'est-ce qu'un Singleton ?**
- Une seule instance dans toute l'application
- Créée au premier inject()
- Détruite à la fin de l'application

**Q: Différence entre inject() et constructor injection ?**
- `inject()` : Moderne, flexible, en dehors du constructor
- Constructor : Classique, verbose, uniquement dans constructor

**Q: Pourquoi copier les objets dans le service ?**
- Éviter les modifications par référence
- Encapsulation des données
- Contrôle des mutations

**Q: Quand utiliser localStorage ?**
- Données simples à persister
- Pas de données sensibles
- < 5-10 MB de données

---

## 💡 Bonnes Pratiques

1. ✅ **Un service = une responsabilité**
2. ✅ **Toujours copier** les objets en entrée/sortie
3. ✅ **Utiliser inject()** (nouvelle syntaxe)
4. ✅ **providedIn: 'root'** par défaut
5. ✅ **Typer les retours** (null | T)
6. ✅ **Méthodes privées** pour l'implémentation

### Anti-Patterns

1. ❌ Logique UI dans les services
2. ❌ Retourner des références directes
3. ❌ Plusieurs responsabilités
4. ❌ État global non maîtrisé

---

## 🔧 Patterns Avancés

### Service avec Signals

```typescript
@Injectable({
  providedIn: 'root'
})
export class CollectionService {
  private collectionsSignal = signal<Collection[]>([]);
  
  // Exposer en lecture seule
  collections = this.collectionsSignal.asReadonly();
  
  add(collection: Collection) {
    this.collectionsSignal.update(cols => [...cols, collection]);
  }
}
```

### Service Factory

```typescript
export function createCollectionService() {
  return new CollectionService();
}

@Injectable({
  providedIn: 'root',
  useFactory: createCollectionService
})
export class CollectionService {
}
```

---

## 📝 Checklist Services

- [ ] Service créé avec CLI
- [ ] @Injectable avec providedIn: 'root'
- [ ] Méthodes CRUD implémentées
- [ ] Copie des objets en entrée/sortie
- [ ] Persistence (localStorage ou API)
- [ ] Injection avec inject()
- [ ] Tests du service

---

## 🎯 Exercice Pratique

Créer un **TodoService** avec :
- Méthodes CRUD complètes
- Persistence dans localStorage
- Signal pour l'état
- Filtrage par statut

```typescript
@Injectable({
  providedIn: 'root'
})
export class TodoService {
  private todos = signal<Todo[]>([]);
  
  getAll() {
    return this.todos();
  }
  
  add(todo: Omit<Todo, 'id'>) {
    const newTodo = { ...todo, id: Date.now() };
    this.todos.update(t => [...t, newTodo]);
    this.save();
  }
  
  update(id: number, updates: Partial<Todo>) {
    this.todos.update(todos => 
      todos.map(t => t.id === id ? { ...t, ...updates } : t)
    );
    this.save();
  }
  
  delete(id: number) {
    this.todos.update(todos => todos.filter(t => t.id !== id));
    this.save();
  }
  
  private save() {
    localStorage.setItem('todos', JSON.stringify(this.todos()));
  }
  
  private load() {
    const data = localStorage.getItem('todos');
    if (data) {
      this.todos.set(JSON.parse(data));
    }
  }
}
```

---

## ⚠️ Erreurs Courantes

| Erreur | Cause | Solution |
|--------|-------|----------|
| Service not found | Pas de @Injectable | Ajouter @Injectable |
| Multiple instances | providedIn manquant | Ajouter providedIn: 'root' |
| Mutation bugs | Retour par référence | Copier les objets |
| localStorage full | Trop de données | Limiter ou utiliser IndexedDB |

---

## 🚀 localStorage vs Alternatives

| Solution | Taille | Complexité | Persistence |
|----------|--------|------------|-------------|
| **localStorage** | ~5 MB | Simple | Navigateur |
| **sessionStorage** | ~5 MB | Simple | Session |
| **IndexedDB** | ~1 GB+ | Complexe | Navigateur |
| **API Backend** | Illimité | Moyenne | Serveur |

---

## 📚 Résumé

**Services = Logique Métier Centralisée**

- **@Injectable** → Rendre injectable
- **inject()** → Injecter dans composants
- **CRUD** → Create, Read, Update, Delete
- **copy()** → Éviter mutations
- **localStorage** → Persistence simple

**Un service = une responsabilité !**
