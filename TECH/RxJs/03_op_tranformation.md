## [**..**](./00_index.md)

# Chapitre 3 : Opérateurs de Transformation

## 📑 Table des matières
- [3.1 Opérateurs de Base](#31-opérateurs-de-base)
- [3.2 Opérateurs de Flattening](#32-opérateurs-de-flattening)
- [3.3 Différences et Cas d'Usage](#33-différences-et-cas-dusage)
- [3.4 Autres Transformations](#34-autres-transformations)
- [3.5 Exercice Pratique](#35-exercice-pratique)
- [Questions/Réponses d'Entretien](#-questionsréponses-dentretien)

---

## 3.1 Opérateurs de Base

### 🎯 map() - Transformer les Valeurs

L'opérateur **`map()`** transforme chaque valeur émise en appliquant une fonction.

```typescript
import { of } from 'rxjs';
import { map } from 'rxjs/operators';

// Transformation simple
const numbers$ = of(1, 2, 3, 4, 5);

numbers$.pipe(
  map(x => x * 2)
).subscribe(console.log);
// Output: 2, 4, 6, 8, 10

// Transformation d'objets
interface User {
  id: number;
  firstName: string;
  lastName: string;
}

const users$ = of(
  { id: 1, firstName: 'John', lastName: 'Doe' },
  { id: 2, firstName: 'Jane', lastName: 'Smith' }
);

users$.pipe(
  map(user => ({
    id: user.id,
    fullName: `${user.firstName} ${user.lastName}`
  }))
).subscribe(console.log);
// Output:
// { id: 1, fullName: 'John Doe' }
// { id: 2, fullName: 'Jane Smith' }
```

**Diagramme de marbre :**
```
source:     --1--2--3--4--5--|
  map(x*2): --2--4--6--8--10-|
```

#### Cas d'usage Angular : Transformation de Réponse HTTP

```typescript
interface UserDTO {
  id: number;
  first_name: string;
  last_name: string;
  email_address: string;
}

interface User {
  id: number;
  fullName: string;
  email: string;
}

@Injectable({ providedIn: 'root' })
export class UserService {
  constructor(private http: HttpClient) {}
  
  getUsers(): Observable<User[]> {
    return this.http.get<UserDTO[]>('/api/users').pipe(
      map(dtos => dtos.map(dto => ({
        id: dto.id,
        fullName: `${dto.first_name} ${dto.last_name}`,
        email: dto.email_address
      })))
    );
  }
}
```

### 🔍 pluck() - Extraire une Propriété

**Note** : `pluck()` est déprécié depuis RxJS 7. Utilisez `map()` à la place.

```typescript
import { of } from 'rxjs';
import { pluck, map } from 'rxjs/operators';

const users$ = of(
  { id: 1, name: 'Alice', age: 25 },
  { id: 2, name: 'Bob', age: 30 },
  { id: 3, name: 'Charlie', age: 35 }
);

// ❌ Ancienne méthode (dépréciée)
users$.pipe(
  pluck('name')
).subscribe(console.log);

// ✅ Nouvelle méthode recommandée
users$.pipe(
  map(user => user.name)
).subscribe(console.log);
// Output: Alice, Bob, Charlie

// Propriétés imbriquées
const data$ = of(
  { user: { profile: { name: 'Alice' } } },
  { user: { profile: { name: 'Bob' } } }
);

// ❌ Ancienne méthode
data$.pipe(
  pluck('user', 'profile', 'name')
).subscribe(console.log);

// ✅ Nouvelle méthode
data$.pipe(
  map(data => data.user.profile.name)
).subscribe(console.log);
// Output: Alice, Bob
```

### 📌 mapTo() - Mapper vers une Constante

```typescript
import { fromEvent } from 'rxjs';
import { mapTo, scan } from 'rxjs/operators';

const button = document.getElementById('incrementBtn');
const clicks$ = fromEvent(button, 'click');

// Chaque clic est transformé en 1
const increments$ = clicks$.pipe(
  mapTo(1),
  scan((total, increment) => total + increment, 0)
);

increments$.subscribe(total => {
  console.log('Total clicks:', total);
});

// Exemple : Réinitialiser un formulaire
const resetBtn = document.getElementById('resetBtn');
const reset$ = fromEvent(resetBtn, 'click');

reset$.pipe(
  mapTo(null)
).subscribe(value => {
  this.form.reset();
  console.log('Form reset');
});
```

**Diagramme de marbre :**
```
clicks$:    --c--c----c--c--|
  mapTo(1): --1--1----1--1--|
  scan():   --1--2----3--4--|
```

---

## 3.2 Opérateurs de Flattening

Les opérateurs de **flattening** (aplatissement) transforment un Observable d'Observables en un Observable simple.

### 🔄 mergeMap() (flatMap) - Fusionner les Observables

**`mergeMap()`** projette chaque valeur vers un Observable, puis fusionne tous les Observables résultants.

```typescript
import { of } from 'rxjs';
import { mergeMap, delay } from 'rxjs/operators';

const source$ = of(1, 2, 3);

source$.pipe(
  mergeMap(value => 
    of(`Résultat: ${value}`).pipe(delay(1000))
  )
).subscribe(console.log);

// Output (tous après 1 seconde, en parallèle):
// Résultat: 1
// Résultat: 2
// Résultat: 3
```

**Diagramme de marbre :**
```
source:       --1--2--3--|
  mergeMap:   
    1:        ---a|
    2:        ------b|
    3:        ---------c|
  résultat:   ---a--b--c--|
```

#### Cas d'usage : Requêtes HTTP Parallèles

```typescript
@Injectable({ providedIn: 'root' })
export class OrderService {
  constructor(private http: HttpClient) {}
  
  // Charger les détails de plusieurs commandes en parallèle
  loadOrdersDetails(orderIds: number[]): Observable<OrderDetail> {
    return from(orderIds).pipe(
      mergeMap(id => this.http.get<OrderDetail>(`/api/orders/${id}`))
    );
  }
}

// Utilisation
const orderIds = [1, 2, 3, 4, 5];
this.orderService.loadOrdersDetails(orderIds).subscribe(
  detail => console.log('Order loaded:', detail),
  error => console.error('Error:', error),
  () => console.log('All orders loaded')
);
```

#### Limiter la Concurrence

```typescript
import { mergeMap } from 'rxjs/operators';

// Limite de 3 requêtes simultanées
from(orderIds).pipe(
  mergeMap(
    id => this.http.get(`/api/orders/${id}`),
    3  // ✅ Concurrent parameter
  )
).subscribe(console.log);
```

### 🔀 switchMap() - Annuler et Basculer

**`switchMap()`** annule l'Observable précédent et bascule vers le nouveau.

```typescript
import { fromEvent, interval } from 'rxjs';
import { switchMap } from 'rxjs/operators';

const button = document.getElementById('startBtn');
const clicks$ = fromEvent(button, 'click');

// Chaque clic annule le timer précédent
clicks$.pipe(
  switchMap(() => interval(1000))
).subscribe(value => {
  console.log('Timer:', value);
});

// Si vous cliquez plusieurs fois :
// Premier clic : 0, 1, 2...
// Deuxième clic : annule le premier, redémarre à 0, 1, 2...
```

**Diagramme de marbre :**
```
clicks:       --c------c--------c----|
  switchMap:
    1:        ---0-1-2-|
    2:        ----------0-1-2-3-|
    3:        ------------------0-1-|
  résultat:   ---0-1-2--0-1-2-3-0-1-|
              └──┘     └───────┘ (annulations)
```

#### Cas d'usage : Recherche avec Autocomplete

```typescript
@Component({
  selector: 'app-search',
  template: `
    <input #searchInput 
           type="text" 
           placeholder="Rechercher...">
    <div *ngFor="let result of results">
      {{ result.name }}
    </div>
  `
})
export class SearchComponent implements OnInit {
  @ViewChild('searchInput') searchInput: ElementRef;
  results: any[] = [];
  
  constructor(private http: HttpClient) {}
  
  ngOnInit() {
    fromEvent(this.searchInput.nativeElement, 'keyup').pipe(
      map((event: any) => event.target.value),
      debounceTime(300),  // Attendre 300ms après la dernière frappe
      distinctUntilChanged(),  // Ignorer si la valeur n'a pas changé
      switchMap(query => {
        if (!query) {
          return of([]);
        }
        // ✅ Annule la requête précédente si une nouvelle recherche arrive
        return this.http.get<any[]>(`/api/search?q=${query}`);
      })
    ).subscribe(results => {
      this.results = results;
    });
  }
}
```

**Pourquoi switchMap() est parfait ici ?**
- Si l'utilisateur tape rapidement, seule la dernière requête est exécutée
- Les requêtes précédentes sont annulées (économie de bande passante)
- Évite les résultats désordonnés (race conditions)

### 🔗 concatMap() - Préserver l'Ordre

**`concatMap()`** attend que l'Observable précédent se termine avant de commencer le suivant.

```typescript
import { of } from 'rxjs';
import { concatMap, delay } from 'rxjs/operators';

const source$ = of(1, 2, 3);

source$.pipe(
  concatMap(value => 
    of(`Traitement: ${value}`).pipe(delay(1000))
  )
).subscribe(console.log);

// Output (un par un, avec 1 seconde entre chaque):
// (après 1s) Traitement: 1
// (après 2s) Traitement: 2
// (après 3s) Traitement: 3
```

**Diagramme de marbre :**
```
source:       --1--2--3--|
  concatMap:
    1:        ---a|
    2:            ---b|
    3:                ---c|
  résultat:   ---a---b---c|
              (attend chaque complétion)
```

#### Cas d'usage : Sauvegarder dans l'Ordre

```typescript
@Injectable({ providedIn: 'root' })
export class FormService {
  constructor(private http: HttpClient) {}
  
  // Sauvegarder plusieurs formulaires dans l'ordre
  saveFormsInOrder(forms: FormData[]): Observable<any> {
    return from(forms).pipe(
      concatMap(form => 
        this.http.post('/api/forms', form).pipe(
          tap(response => console.log('Form saved:', response))
        )
      )
    );
  }
}

// Utilisation
const forms = [form1, form2, form3];
this.formService.saveFormsInOrder(forms).subscribe({
  next: response => console.log('Response:', response),
  complete: () => console.log('All forms saved in order')
});
```

### 🚫 exhaustMap() - Ignorer Pendant l'Exécution

**`exhaustMap()`** ignore les nouvelles valeurs tant que l'Observable interne n'est pas terminé.

```typescript
import { fromEvent, interval } from 'rxjs';
import { exhaustMap, take } from 'rxjs/operators';

const button = document.getElementById('saveBtn');
const clicks$ = fromEvent(button, 'click');

clicks$.pipe(
  exhaustMap(() => 
    // Simulation de sauvegarde (3 secondes)
    interval(1000).pipe(take(3))
  )
).subscribe(value => {
  console.log('Saving...', value);
});

// Si vous cliquez 3 fois rapidement :
// Premier clic : démarre la sauvegarde (0, 1, 2)
// Deuxième clic : IGNORÉ (sauvegarde en cours)
// Troisième clic : IGNORÉ (sauvegarde en cours)
// Après 3 secondes, un nouveau clic démarrera une nouvelle sauvegarde
```

**Diagramme de marbre :**
```
clicks:       --c--c--c------c----|
  exhaustMap:
    1:        ---0-1-2|
    2:        (ignoré)
    3:        (ignoré)
    4:        ---------------0-1-2|
  résultat:   ---0-1-2-------0-1-2|
```

#### Cas d'usage : Bouton de Sauvegarde (Éviter Double-Submit)

```typescript
@Component({
  selector: 'app-form',
  template: `
    <form>
      <input [(ngModel)]="data" name="data">
      <button #saveBtn>Sauvegarder</button>
      <div *ngIf="saving">Sauvegarde en cours...</div>
    </form>
  `
})
export class FormComponent implements OnInit {
  @ViewChild('saveBtn') saveBtn: ElementRef;
  data: string = '';
  saving: boolean = false;
  
  constructor(private http: HttpClient) {}
  
  ngOnInit() {
    fromEvent(this.saveBtn.nativeElement, 'click').pipe(
      exhaustMap(() => {
        this.saving = true;
        return this.http.post('/api/save', { data: this.data }).pipe(
          finalize(() => this.saving = false)
        );
      })
    ).subscribe({
      next: () => console.log('Saved successfully'),
      error: err => console.error('Save failed', err)
    });
  }
}
```

---

## 3.3 Différences et Cas d'Usage

### 📊 Tableau Comparatif

| Opérateur | Comportement | Quand l'utiliser |
|-----------|-------------|------------------|
| **mergeMap** | Exécute en parallèle, fusionne tous | Requêtes indépendantes, ordre non important |
| **switchMap** | Annule le précédent | Recherche, autocomplétion, navigation |
| **concatMap** | Attend la complétion du précédent | Opérations séquentielles, ordre important |
| **exhaustMap** | Ignore les nouvelles demandes | Boutons de sauvegarde, prévenir double-submit |

### 🎯 Quand Utiliser mergeMap vs switchMap ?

#### ✅ mergeMap - Requêtes Indépendantes

```typescript
// ✅ BON : Charger les données de plusieurs utilisateurs
from(userIds).pipe(
  mergeMap(id => this.http.get(`/api/users/${id}`))
).subscribe(user => console.log(user));

// Toutes les requêtes s'exécutent en parallèle
// L'ordre des réponses n'est pas garanti
```

#### ✅ switchMap - Annuler les Requêtes Précédentes

```typescript
// ✅ BON : Recherche avec autocomplétion
searchInput$.pipe(
  debounceTime(300),
  switchMap(query => this.http.get(`/api/search?q=${query}`))
).subscribe(results => console.log(results));

// Chaque nouvelle recherche annule la précédente
// Évite les race conditions
```

### 🎯 Quand Utiliser concatMap vs exhaustMap ?

#### ✅ concatMap - Ordre Garanti

```typescript
// ✅ BON : Envoyer des logs dans l'ordre
from(logEntries).pipe(
  concatMap(entry => this.http.post('/api/logs', entry))
).subscribe();

// Les logs sont envoyés un par un, dans l'ordre
// Attend la complétion de chaque requête
```

#### ✅ exhaustMap - Prévenir les Requêtes Multiples

```typescript
// ✅ BON : Bouton de login
loginButton$.pipe(
  exhaustMap(credentials => this.http.post('/api/login', credentials))
).subscribe();

// Les clics supplémentaires sont ignorés pendant la connexion
// Évite les requêtes duplicatas
```

### ⚠️ Erreurs Courantes

#### ❌ MAUVAIS : mergeMap pour la Recherche

```typescript
// ❌ PROBLÈME : Race condition
searchInput$.pipe(
  debounceTime(300),
  mergeMap(query => this.http.get(`/api/search?q=${query}`))
).subscribe(results => this.results = results);

// Si l'utilisateur tape "chat" puis "chien" :
// La requête "chat" peut arriver APRÈS "chien"
// Les résultats seront incorrects
```

#### ✅ BON : switchMap pour la Recherche

```typescript
// ✅ SOLUTION : switchMap annule les requêtes précédentes
searchInput$.pipe(
  debounceTime(300),
  switchMap(query => this.http.get(`/api/search?q=${query}`))
).subscribe(results => this.results = results);
```

#### ❌ MAUVAIS : concatMap pour des Requêtes Lentes

```typescript
// ❌ PROBLÈME : Très lent
from(largeArray).pipe(
  concatMap(item => this.http.get(`/api/items/${item.id}`))
).subscribe();

// Attend chaque requête = très lent pour 1000 items
```

#### ✅ BON : mergeMap avec Limite de Concurrence

```typescript
// ✅ SOLUTION : Parallèle avec limite
from(largeArray).pipe(
  mergeMap(
    item => this.http.get(`/api/items/${item.id}`),
    5  // Max 5 requêtes simultanées
  )
).subscribe();
```

### 🎨 Diagrammes de Marbre Comparatifs

```
Source:    --1--2--3--4--|

mergeMap:
  1:       ---a|
  2:       ------b|
  3:       ---------c|
  4:       ------------d|
Résultat:  ---a--b--c--d-|
  (toutes en parallèle)

switchMap:
  1:       ---a|
  2:       ------b|
  3:       ---------c|
  4:       ------------d|
Résultat:  ----------c--d|
  (1 et 2 annulés, seules 3 et 4 complètes)

concatMap:
  1:       ---a|
  2:          ---b|
  3:             ---c|
  4:                ---d|
Résultat:  ---a---b---c---d|
  (une par une, dans l'ordre)

exhaustMap:
  1:       ---a|
  2:       (ignoré)
  3:       (ignoré)
  4:       ------------d|
Résultat:  ---a--------d-|
  (2 et 3 ignorés pendant que 1 s'exécute)
```

---

## 3.4 Autres Transformations

### 📊 scan() - Accumulation

**`scan()`** applique une fonction d'accumulation sur chaque valeur (comme `reduce` mais émet à chaque étape).

```typescript
import { of } from 'rxjs';
import { scan } from 'rxjs/operators';

const numbers$ = of(1, 2, 3, 4, 5);

numbers$.pipe(
  scan((acc, value) => acc + value, 0)
).subscribe(console.log);

// Output:
// 1   (0 + 1)
// 3   (1 + 2)
// 6   (3 + 3)
// 10  (6 + 4)
// 15  (10 + 5)
```

**Diagramme de marbre :**
```
source:    --1--2--3--4--5-|
  scan:    --1--3--6--10-15|
           (somme cumulative)
```

#### Cas d'usage : Panier d'Achat

```typescript
interface CartAction {
  type: 'ADD' | 'REMOVE';
  product: Product;
}

interface CartState {
  items: Product[];
  total: number;
}

@Injectable({ providedIn: 'root' })
export class CartService {
  private actionsSubject = new Subject<CartAction>();
  
  cart$ = this.actionsSubject.pipe(
    scan((state: CartState, action: CartAction) => {
      switch (action.type) {
        case 'ADD':
          return {
            items: [...state.items, action.product],
            total: state.total + action.product.price
          };
        case 'REMOVE':
          const items = state.items.filter(p => p.id !== action.product.id);
          return {
            items,
            total: items.reduce((sum, p) => sum + p.price, 0)
          };
        default:
          return state;
      }
    }, { items: [], total: 0 })
  );
  
  addToCart(product: Product) {
    this.actionsSubject.next({ type: 'ADD', product });
  }
  
  removeFromCart(product: Product) {
    this.actionsSubject.next({ type: 'REMOVE', product });
  }
}

// Component
constructor(private cart: CartService) {}

ngOnInit() {
  this.cart.cart$.subscribe(state => {
    console.log('Items:', state.items);
    console.log('Total:', state.total);
  });
}
```

### 🎯 reduce() - Réduction Finale

**`reduce()`** comme `scan()`, mais émet seulement la valeur finale (après complétion).

```typescript
import { of } from 'rxjs';
import { reduce } from 'rxjs/operators';

const numbers$ = of(1, 2, 3, 4, 5);

numbers$.pipe(
  reduce((acc, value) => acc + value, 0)
).subscribe(console.log);

// Output (seulement à la fin):
// 15
```

**Diagramme de marbre :**
```
source:    --1--2--3--4--5-|
  reduce:  ----------------15|
           (seulement la somme finale)
```

#### Comparaison scan() vs reduce()

```typescript
// scan() : émet à chaque étape
of(1, 2, 3, 4, 5).pipe(
  scan((acc, val) => acc + val, 0)
).subscribe(console.log);
// Output: 1, 3, 6, 10, 15

// reduce() : émet seulement à la fin
of(1, 2, 3, 4, 5).pipe(
  reduce((acc, val) => acc + val, 0)
).subscribe(console.log);
// Output: 15
```

### 📦 toArray() - Convertir en Tableau

**`toArray()`** collecte toutes les valeurs et les émet dans un tableau à la complétion.

```typescript
import { interval } from 'rxjs';
import { take, toArray } from 'rxjs/operators';

interval(1000).pipe(
  take(5),
  toArray()
).subscribe(console.log);

// Output (après 5 secondes):
// [0, 1, 2, 3, 4]
```

#### Cas d'usage : Collecter des Résultats

```typescript
@Injectable({ providedIn: 'root' })
export class DataService {
  constructor(private http: HttpClient) {}
  
  getAllUsers(): Observable<User[]> {
    const userIds = [1, 2, 3, 4, 5];
    
    return from(userIds).pipe(
      mergeMap(id => this.http.get<User>(`/api/users/${id}`)),
      toArray()  // ✅ Collecte tous les utilisateurs dans un tableau
    );
  }
}

// Utilisation
this.dataService.getAllUsers().subscribe(users => {
  console.log('All users:', users);  // [user1, user2, user3, user4, user5]
});
```

---

## 3.5 Exercice Pratique

### 🎯 Challenge : Recherche avec Autocomplete

Créez un composant de recherche avec :
1. Champ de saisie
2. Debounce de 300ms
3. Annulation des requêtes précédentes
4. Affichage des résultats
5. Gestion du loading
6. Gestion des erreurs

### 💡 Solution

```typescript
// search.component.ts
import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import { fromEvent, Observable, of } from 'rxjs';
import {
  map,
  debounceTime,
  distinctUntilChanged,
  switchMap,
  catchError,
  tap
} from 'rxjs/operators';
import { HttpClient } from '@angular/common/http';

interface SearchResult {
  id: number;
  title: string;
  description: string;
}

@Component({
  selector: 'app-search',
  template: `
    <div class="search-container">
      <input 
        #searchInput
        type="text" 
        placeholder="Rechercher..."
        class="search-input">
      
      <div *ngIf="loading" class="loading">
        Recherche en cours...
      </div>
      
      <div *ngIf="error" class="error">
        {{ error }}
      </div>
      
      <div class="results">
        <div *ngFor="let result of results" class="result-item">
          <h3>{{ result.title }}</h3>
          <p>{{ result.description }}</p>
        </div>
        
        <div *ngIf="results.length === 0 && !loading && searchQuery" 
             class="no-results">
          Aucun résultat trouvé
        </div>
      </div>
    </div>
  `,
  styles: [`
    .search-container {
      max-width: 600px;
      margin: 20px auto;
      padding: 20px;
    }
    
    .search-input {
      width: 100%;
      padding: 12px;
      font-size: 16px;
      border: 2px solid #ddd;
      border-radius: 4px;
      margin-bottom: 20px;
    }
    
    .search-input:focus {
      outline: none;
      border-color: #4CAF50;
    }
    
    .loading {
      text-align: center;
      color: #666;
      padding: 20px;
    }
    
    .error {
      background: #f44336;
      color: white;
      padding: 12px;
      border-radius: 4px;
      margin-bottom: 20px;
    }
    
    .result-item {
      border: 1px solid #ddd;
      padding: 15px;
      margin-bottom: 10px;
      border-radius: 4px;
      transition: box-shadow 0.3s;
    }
    
    .result-item:hover {
      box-shadow: 0 2px 8px rgba(0,0,0,0.1);
    }
    
    .result-item h3 {
      margin: 0 0 8px 0;
      color: #333;
    }
    
    .result-item p {
      margin: 0;
      color: #666;
    }
    
    .no-results {
      text-align: center;
      color: #999;
      padding: 40px 20px;
    }
  `]
})
export class SearchComponent implements OnInit {
  @ViewChild('searchInput', { static: true }) searchInput: ElementRef;
  
  results: SearchResult[] = [];
  loading: boolean = false;
  error: string | null = null;
  searchQuery: string = '';
  
  constructor(private http: HttpClient) {}
  
  ngOnInit() {
    const searchInput$ = fromEvent<KeyboardEvent>(
      this.searchInput.nativeElement,
      'keyup'
    );
    
    searchInput$.pipe(
      // Extraire la valeur
      map((event: KeyboardEvent) => 
        (event.target as HTMLInputElement).value
      ),
      
      // Sauvegarder la query
      tap(query => this.searchQuery = query),
      
      // Attendre 300ms après la dernière frappe
      debounceTime(300),
      
      // Ignorer si la valeur n'a pas changé
      distinctUntilChanged(),
      
      // Afficher le loading
      tap(() => {
        this.loading = true;
        this.error = null;
      }),
      
      // Effectuer la recherche (annule les précédentes)
      switchMap(query => this.search(query).pipe(
        catchError(error => {
          this.error = 'Erreur lors de la recherche';
          this.loading = false;
          return of([]);
        })
      ))
    ).subscribe(results => {
      this.results = results;
      this.loading = false;
    });
  }
  
  private search(query: string): Observable<SearchResult[]> {
    if (!query || query.trim().length === 0) {
      return of([]);
    }
    
    return this.http.get<SearchResult[]>(
      `/api/search?q=${encodeURIComponent(query)}`
    );
  }
}
```

### 🎨 Version Améliorée avec Historique

```typescript
@Component({
  selector: 'app-advanced-search',
  template: `
    <div class="search-container">
      <input #searchInput type="text" placeholder="Rechercher...">
      
      <!-- Historique des recherches -->
      <div class="search-history" *ngIf="searchHistory.length > 0">
        <h4>Recherches récentes</h4>
        <div class="history-item" 
             *ngFor="let item of searchHistory"
             (click)="searchInput.value = item; searchFromHistory(item)">
          {{ item }}
        </div>
      </div>
      
      <!-- Résultats -->
      <div class="results">
        <div *ngIf="(results$ | async) as results">
          <div *ngFor="let result of results" class="result-item">
            <h3>{{ result.title }}</h3>
            <p>{{ result.description }}</p>
          </div>
        </div>
      </div>
    </div>
  `,
  styles: [`
    /* Styles similaires + historique */
    .search-history {
      background: #f5f5f5;
      padding: 15px;
      border-radius: 4px;
      margin-bottom: 20px;
    }
    
    .history-item {
      padding: 8px;
      cursor: pointer;
      border-radius: 4px;
      margin-bottom: 5px;
    }
    
    .history-item:hover {
      background: #e0e0e0;
    }
  `]
})
export class AdvancedSearchComponent implements OnInit {
  @ViewChild('searchInput', { static: true }) searchInput: ElementRef;
  
  results$: Observable<SearchResult[]>;
  searchHistory: string[] = [];
  private searchSubject = new Subject<string>();
  
  constructor(private http: HttpClient) {}
  
  ngOnInit() {
    // Observable des résultats
    this.results$ = this.searchSubject.pipe(
      debounceTime(300),
      distinctUntilChanged(),
      tap(query => {
        // Ajouter à l'historique
        if (query && !this.searchHistory.includes(query)) {
          this.searchHistory = [query, ...this.searchHistory].slice(0, 5);
        }
      }),
      switchMap(query => {
        if (!query) return of([]);
        return this.http.get<SearchResult[]>(`/api/search?q=${query}`).pipe(
          catchError(() => of([]))
        );
      })
    );
    
    // Écouter les frappes clavier
    fromEvent<KeyboardEvent>(this.searchInput.nativeElement, 'keyup').pipe(
      map(event => (event.target as HTMLInputElement).value)
    ).subscribe(query => {
      this.searchSubject.next(query);
    });
  }
  
  searchFromHistory(query: string) {
    this.searchSubject.next(query);
  }
}
```

---

## 📝 Questions/Réponses d'Entretien

### Q1 : Quelle est la différence entre map() et mergeMap() ?

**Réponse :**
> **`map()`** :
> - Transforme chaque valeur en une nouvelle valeur
> - Retourne une valeur simple (pas un Observable)
> - Opération synchrone
> 
> ```typescript
> of(1, 2, 3).pipe(
>   map(x => x * 2)
> ).subscribe(console.log);
> // Output: 2, 4, 6
> ```
> 
> **`mergeMap()`** :
> - Transforme chaque valeur en un Observable
> - "Aplatit" (flatten) les Observables résultants
> - Gère l'asynchrone
> 
> ```typescript
> of(1, 2, 3).pipe(
>   mergeMap(x => of(x * 2).pipe(delay(1000)))
> ).subscribe(console.log);
> // Output (après 1s): 2, 4, 6
> ```

### Q2 : Quand utiliser switchMap() plutôt que mergeMap() ?

**Réponse :**
> **`switchMap()`** quand vous voulez **annuler les requêtes précédentes** :
> - Recherche avec autocomplétion
> - Navigation (route params)
> - Formulaires réactifs
> 
> **`mergeMap()`** quand toutes les requêtes doivent **s'exécuter en parallèle** :
> - Charger plusieurs ressources indépendantes
> - Traiter une liste d'éléments
> - Requêtes qui ne doivent pas être annulées
> 
> **Exemple - Recherche** :
> ```typescript
> // ✅ BON avec switchMap
> searchInput$.pipe(
>   switchMap(query => this.http.get(`/api/search?q=${query}`))
> )
> // Si l'utilisateur tape rapidement, seule la dernière requête compte
> 
> // ❌ MAUVAIS avec mergeMap
> searchInput$.pipe(
>   mergeMap(query => this.http.get(`/api/search?q=${query}`))
> )
> // Toutes les requêtes s'exécutent = race condition possible
> ```

### Q3 : Expliquez la différence entre concatMap() et exhaustMap()

**Réponse :**
> **`concatMap()`** :
> - **Attend** que l'Observable précédent se termine
> - **File** les nouvelles demandes
> - Toutes les valeurs sont traitées, dans l'ordre
> 
> **`exhaustMap()`** :
> - **Ignore** les nouvelles demandes pendant l'exécution
> - Ne file pas
> - Certaines valeurs sont ignorées
> 
> **Exemple - Bouton de sauvegarde** :
> ```typescript
> // concatMap : toutes les sauvegardes sont effectuées
> saveButton$.pipe(
>   concatMap(() => this.http.post('/api/save', data))
> )
> // 3 clics rapides = 3 sauvegardes (une après l'autre)
> 
> // exhaustMap : ignore les clics pendant la sauvegarde
> saveButton$.pipe(
>   exhaustMap(() => this.http.post('/api/save', data))
> )
> // 3 clics rapides = 1 seule sauvegarde
> ```

### Q4 : Qu'est-ce que le "flattening" d'Observables ?

**Réponse :**
> Le **flattening** (aplatissement) consiste à transformer un **Observable d'Observables** en un **Observable simple**.
> 
> **Sans flattening** :
> ```typescript
> of(1, 2, 3).pipe(
>   map(x => of(x * 2))
> ).subscribe(console.log);
> // Output: Observable, Observable, Observable ❌
> ```
> 
> **Avec flattening (mergeMap)** :
> ```typescript
> of(1, 2, 3).pipe(
>   mergeMap(x => of(x * 2))
> ).subscribe(console.log);
> // Output: 2, 4, 6 ✅
> ```
> 
> Les opérateurs de flattening sont : `mergeMap`, `switchMap`, `concatMap`, `exhaustMap`.

### Q5 : Quelle est la différence entre scan() et reduce() ?

**Réponse :**
> **`scan()`** :
> - Émet à **chaque étape** de l'accumulation
> - Continue tant que l'Observable est actif
> - Utile pour afficher des états intermédiaires
> 
> **`reduce()`** :
> - Émet **seulement à la fin** (après complete)
> - Retourne la valeur finale uniquement
> - Équivalent à `Array.reduce()`
> 
> ```typescript
> of(1, 2, 3, 4, 5).pipe(
>   scan((acc, val) => acc + val, 0)
> ).subscribe(console.log);
> // Output: 1, 3, 6, 10, 15 (5 émissions)
> 
> of(1, 2, 3, 4, 5).pipe(
>   reduce((acc, val) => acc + val, 0)
> ).subscribe(console.log);
> // Output: 15 (1 seule émission)
> ```

### Q6 : Comment éviter les race conditions avec les requêtes HTTP ?

**Réponse :**
> Utilisez **`switchMap()`** qui annule automatiquement les requêtes précédentes :
> 
> ```typescript
> // ❌ PROBLÈME avec mergeMap
> searchInput$.pipe(
>   debounceTime(300),
>   mergeMap(query => this.http.get(`/api/search?q=${query}`))
> )
> // Si "chat" prend 500ms et "chien" prend 100ms,
> // les résultats de "chat" arriveront en dernier ❌
> 
> // ✅ SOLUTION avec switchMap
> searchInput$.pipe(
>   debounceTime(300),
>   switchMap(query => this.http.get(`/api/search?q=${query}`))
> )
> // "chat" est annulé dès que "chien" est tapé ✅
> ```

### Q7 : Pourquoi utiliser toArray() plutôt que reduce() ?

**Réponse :**
> **`toArray()`** est plus lisible et spécialisé pour créer un tableau :
> 
> ```typescript
> // ✅ Avec toArray() - clair et simple
> interval(1000).pipe(
>   take(5),
>   toArray()
> ).subscribe(console.log);
> // Output: [0, 1, 2, 3, 4]
> 
> // ❌ Avec reduce() - plus verbeux
> interval(1000).pipe(
>   take(5),
>   reduce((acc, val) => [...acc, val], [])
> ).subscribe(console.log);
> // Output: [0, 1, 2, 3, 4]
> ```
> 
> Utilisez `toArray()` quand vous voulez collecter toutes les valeurs dans un tableau.

### Q8 : Comment gérer les erreurs dans mergeMap() ?

**Réponse :**
> Utilisez `catchError()` **à l'intérieur** du mergeMap pour éviter de casser le flux principal :
> 
> ```typescript
> // ❌ MAUVAIS : catchError à l'extérieur
> from(ids).pipe(
>   mergeMap(id => this.http.get(`/api/items/${id}`)),
>   catchError(err => of([]))
> )
> // Si une requête échoue, tout le flux s'arrête ❌
> 
> // ✅ BON : catchError à l'intérieur
> from(ids).pipe(
>   mergeMap(id => 
>     this.http.get(`/api/items/${id}`).pipe(
>       catchError(err => {
>         console.error(`Error for ${id}:`, err);
>         return of(null);  // Continuer avec null
>       })
>     )
>   )
> )
> // Si une requête échoue, les autres continuent ✅
> ```

### Q9 : Comment limiter le nombre de requêtes parallèles avec mergeMap() ?

**Réponse :**
> Utilisez le paramètre de **concurrence** de mergeMap :
> 
> ```typescript
> from(largeArray).pipe(
>   mergeMap(
>     item => this.http.get(`/api/items/${item.id}`),
>     3  // ✅ Maximum 3 requêtes simultanées
>   )
> ).subscribe(result => console.log(result));
> ```
> 
> **Avantages** :
> - Évite de surcharger le serveur
> - Meilleure gestion des ressources
> - Plus rapide que concatMap (qui fait 1 par 1)
> - Plus contrôlé que mergeMap sans limite

### Q10 : Quelle est la différence entre pluck() et map() ?

**Réponse :**
> **`pluck()`** est **déprécié depuis RxJS 7**. Utilisez `map()` à la place :
> 
> ```typescript
> const users$ = of(
>   { id: 1, name: 'Alice', age: 25 },
>   { id: 2, name: 'Bob', age: 30 }
> );
> 
> // ❌ Ancienne méthode (dépréciée)
> users$.pipe(
>   pluck('name')
> ).subscribe(console.log);
> 
> // ✅ Nouvelle méthode recommandée
> users$.pipe(
>   map(user => user.name)
> ).subscribe(console.log);
> // Output: Alice, Bob
> ```
> 
> **Raisons de la dépréciation** :
> - `map()` est plus flexible
> - Meilleur support TypeScript
> - Moins d'opérateurs à maintenir

### Q11 : Comment transformer un Observable d'Observables sans flattening operators ?

**Réponse :**
> Vous ne pouvez pas facilement ! C'est exactement pourquoi les opérateurs de flattening existent :
> 
> ```typescript
> // ❌ Sans flattening operator
> of(1, 2, 3).pipe(
>   map(x => this.http.get(`/api/items/${x}`))
> ).subscribe(innerObs$ => {
>   // innerObs$ est un Observable ❌
>   innerObs$.subscribe(data => console.log(data));
> });
> // Crée des nested subscriptions = mauvaise pratique
> 
> // ✅ Avec flattening operator
> of(1, 2, 3).pipe(
>   mergeMap(x => this.http.get(`/api/items/${x}`))
> ).subscribe(data => {
>   // data est la valeur directement ✅
>   console.log(data);
> });
> ```

### Q12 : Dans quel cas utiliser mapTo() ?

**Réponse :**
> Utilisez `mapTo()` quand vous voulez **toujours émettre la même valeur** peu importe l'entrée :
> 
> ```typescript
> // Bouton de reset - toujours émettre null
> resetButton$.pipe(
>   mapTo(null)
> ).subscribe(value => {
>   this.form.reset();
> });
> 
> // Compteur de clics - convertir chaque clic en 1
> clicks$.pipe(
>   mapTo(1),
>   scan((total, one) => total + one, 0)
> ).subscribe(total => console.log('Total:', total));
> 
> // Toggle boolean
> toggleButton$.pipe(
>   scan((state) => !state, false)
> ).subscribe(isOpen => console.log('Is open:', isOpen));
> ```

---

## 🎯 Points Clés du Chapitre 3

### Opérateurs de Base
✅ **map()** : Transformer chaque valeur
✅ **mapTo()** : Mapper vers une constante
✅ **pluck()** : Déprécié, utiliser map() à la place

### Opérateurs de Flattening
✅ **mergeMap()** : Parallèle, toutes les valeurs traitées
✅ **switchMap()** : Annule le précédent (recherche, navigation)
✅ **concatMap()** : Séquentiel, ordre garanti
✅ **exhaustMap()** : Ignore pendant l'exécution (boutons)

### Quand Utiliser Quel Opérateur ?
✅ **Recherche/Autocomplétion** → switchMap()
✅ **Requêtes indépendantes** → mergeMap()
✅ **Opérations dans l'ordre** → concatMap()
✅ **Éviter double-submit** → exhaustMap()

### Accumulation
✅ **scan()** : Émet à chaque étape
✅ **reduce()** : Émet seulement à la fin
✅ **toArray()** : Collecte dans un tableau

### Best Practices
✅ **switchMap** pour annuler les anciennes requêtes
✅ **catchError à l'intérieur** de mergeMap pour gérer les erreurs
✅ **Limiter la concurrence** avec mergeMap(fn, n)
✅ **debounceTime** avant switchMap pour la recherche

---