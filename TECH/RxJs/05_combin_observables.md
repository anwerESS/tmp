## [**..**](./00_index.md)

# Chapitre 5 : Combinaison d'Observables

## 📑 Table des matières
- [5.1 Opérateurs de Combinaison](#51-opérateurs-de-combinaison)
- [5.2 Opérateurs de Jointure](#52-opérateurs-de-jointure)
- [5.3 Cas d'Usage Pratiques](#53-cas-dusage-pratiques)
- [5.4 Exercice Pratique](#54-exercice-pratique)
- [Questions/Réponses d'Entretien](#-questionsréponses-dentretien)

---

## 5.1 Opérateurs de Combinaison

### 🎯 merge() - Fusionner Plusieurs Flux

**`merge()`** combine plusieurs Observables en un seul flux qui émet toutes les valeurs dès qu'elles arrivent.

```typescript
import { merge, interval } from 'rxjs';
import { map, take } from 'rxjs/operators';

// Deux flux qui émettent à des vitesses différentes
const fast$ = interval(500).pipe(
  take(5),
  map(n => `Fast: ${n}`)
);

const slow$ = interval(1000).pipe(
  take(3),
  map(n => `Slow: ${n}`)
);

// Fusionner les deux flux
merge(fast$, slow$).subscribe(console.log);

// Output (ordre d'arrivée):
// Fast: 0
// Slow: 0
// Fast: 1
// Fast: 2
// Slow: 1
// Fast: 3
// Fast: 4
// Slow: 2
```

**Diagramme de marbre :**
```
fast$:  --0--1--2--3--4|
slow$:  ----0----1----2|
merge:  --0-01--2-13--42|
```

#### Cas d'usage pratiques

```typescript
// Combiner plusieurs sources de notifications
@Injectable({ providedIn: 'root' })
export class NotificationService {
  private emailNotif$ = this.getEmailNotifications();
  private pushNotif$ = this.getPushNotifications();
  private smsNotif$ = this.getSMSNotifications();
  
  // Toutes les notifications en un seul flux
  allNotifications$ = merge(
    this.emailNotif$,
    this.pushNotif$,
    this.smsNotif$
  );
}

// Gérer plusieurs événements utilisateur
const clicks$ = fromEvent(button, 'click');
const keypress$ = fromEvent(document, 'keypress');
const touch$ = fromEvent(element, 'touchstart');

merge(clicks$, keypress$, touch$).subscribe(event => {
  console.log('User interaction:', event.type);
});
```

### 🔗 concat() - Concaténer Séquentiellement

**`concat()`** combine des Observables en les exécutant **l'un après l'autre** dans l'ordre.

```typescript
import { concat, of, interval } from 'rxjs';
import { take } from 'rxjs/operators';

const first$ = of(1, 2, 3);
const second$ = of(4, 5, 6);
const third$ = interval(1000).pipe(take(3));

concat(first$, second$, third$).subscribe(console.log);

// Output:
// 1, 2, 3 (immédiat)
// 4, 5, 6 (immédiat)
// 0 (après 1s)
// 1 (après 2s)
// 2 (après 3s)
```

**Diagramme de marbre :**
```
first$:  (123)|
second$:       (456)|
third$:             --0--1--2|
concat:  (123)(456)--0--1--2|
```

#### Cas d'usage pratiques

```typescript
// Séquence d'initialisation
function initializeApp(): Observable<string> {
  return concat(
    this.loadConfig(),      // 1. Charger la config
    this.authenticate(),    // 2. S'authentifier
    this.loadUserData(),    // 3. Charger les données
    this.initializeModules() // 4. Initialiser les modules
  );
}

// Opérations CRUD séquentielles
function updateMultipleRecords(records: Record[]): Observable<any> {
  const updates$ = records.map(record => 
    this.http.put(`/api/records/${record.id}`, record)
  );
  
  // Exécuter toutes les mises à jour dans l'ordre
  return concat(...updates$);
}
```

### 🏁 race() - Premier à Émettre

**`race()`** renvoie le premier Observable qui émet une valeur, et ignore les autres.

```typescript
import { race, timer } from 'rxjs';
import { map } from 'rxjs/operators';

const slow$ = timer(3000).pipe(map(() => 'Slow'));
const fast$ = timer(1000).pipe(map(() => 'Fast'));
const medium$ = timer(2000).pipe(map(() => 'Medium'));

race(slow$, fast$, medium$).subscribe(console.log);

// Output: Fast (car c'est le premier à émettre)
```

**Diagramme de marbre :**
```
slow$:   ------S|
fast$:   --F|
medium$: ----M|
race:    --F|
         └─ Seul fast$ émet
```

#### Cas d'usage pratiques

```typescript
// Timeout pour requêtes HTTP
function getDataWithTimeout<T>(url: string): Observable<T> {
  const data$ = this.http.get<T>(url);
  const timeout$ = timer(5000).pipe(
    map(() => { throw new Error('Request timeout'); })
  );
  
  return race(data$, timeout$);
}

// Choisir la source la plus rapide
const cache$ = this.getFromCache(key);
const api$ = this.http.get(url);

race(cache$, api$).subscribe(data => {
  console.log('Got data from fastest source:', data);
});
```

### 🤝 zip() - Combiner par Paires

**`zip()`** combine les valeurs de plusieurs Observables **par index** (première avec première, deuxième avec deuxième, etc.).

```typescript
import { zip, of, interval } from 'rxjs';
import { take } from 'rxjs/operators';

const letters$ = of('A', 'B', 'C', 'D');
const numbers$ = of(1, 2, 3, 4);
const intervals$ = interval(1000).pipe(take(4));

zip(letters$, numbers$, intervals$).subscribe(([letter, number, time]) => {
  console.log(`${letter}${number} at ${time}s`);
});

// Output (toutes les secondes):
// A1 at 0s
// B2 at 1s
// C3 at 2s
// D4 at 3s
```

**Diagramme de marbre :**
```
letters$:  (ABCD)|
numbers$:  (1234)|
intervals: --0--1--2--3|
zip:       --A1-B2-C3-D4|
```

#### Cas d'usage pratiques

```typescript
// Combiner plusieurs requêtes HTTP par ID
function getUsersWithDetails(userIds: string[]): Observable<UserDetail[]> {
  const users$ = userIds.map(id => this.http.get<User>(`/api/users/${id}`));
  const details$ = userIds.map(id => this.http.get<Detail>(`/api/details/${id}`));
  
  return zip(...users$, ...details$).pipe(
    map(results => {
      const mid = results.length / 2;
      const users = results.slice(0, mid) as User[];
      const details = results.slice(mid) as Detail[];
      
      return users.map((user, i) => ({ ...user, detail: details[i] }));
    })
  );
}

// Synchroniser animations
const anim1$ = this.animateElement1();
const anim2$ = this.animateElement2();
const anim3$ = this.animateElement3();

zip(anim1$, anim2$, anim3$).subscribe(() => {
  console.log('Toutes les animations terminées');
});
```

### 🔄 combineLatest() - Dernières Valeurs

**`combineLatest()`** émet un tableau avec les **dernières valeurs** de chaque Observable à chaque fois que l'un d'eux émet.

```typescript
import { combineLatest, interval } from 'rxjs';
import { map, take } from 'rxjs/operators';

const timer1$ = interval(1000).pipe(take(4));
const timer2$ = interval(1500).pipe(take(3));

combineLatest([timer1$, timer2$]).subscribe(([val1, val2]) => {
  console.log(`Timer1: ${val1}, Timer2: ${val2}`);
});

// Output:
// Timer1: 0, Timer2: 0 (après 1.5s - première émission des deux)
// Timer1: 1, Timer2: 0 (après 2s)
// Timer1: 2, Timer2: 1 (après 3s)
// Timer1: 3, Timer2: 1 (après 4s)
// Timer1: 3, Timer2: 2 (après 4.5s)
```

**Diagramme de marbre :**
```
timer1$: --0--1--2--3|
timer2$: ----0----1----2|
combine: ----[0,0]-[1,0]-[2,1]-[3,1]-[3,2]|
```

#### Cas d'usage pratiques

```typescript
// Formulaire réactif : combiner plusieurs champs
@Component({
  selector: 'app-address-form',
  template: `
    <input [formControl]="street">
    <input [formControl]="city">
    <input [formControl]="zipCode">
    <div>{{ fullAddress$ | async }}</div>
  `
})
export class AddressFormComponent {
  street = new FormControl('');
  city = new FormControl('');
  zipCode = new FormControl('');
  
  fullAddress$ = combineLatest([
    this.street.valueChanges,
    this.city.valueChanges,
    this.zipCode.valueChanges
  ]).pipe(
    map(([street, city, zip]) => `${street}, ${city} ${zip}`)
  );
}

// Filtres multiples
@Injectable()
export class ProductService {
  private category$ = new BehaviorSubject<string>('all');
  private priceRange$ = new BehaviorSubject<[number, number]>([0, 1000]);
  private searchTerm$ = new BehaviorSubject<string>('');
  
  filteredProducts$ = combineLatest([
    this.products$,
    this.category$,
    this.priceRange$,
    this.searchTerm$
  ]).pipe(
    map(([products, category, [min, max], term]) => {
      return products.filter(p => 
        (category === 'all' || p.category === category) &&
        (p.price >= min && p.price <= max) &&
        (p.name.toLowerCase().includes(term.toLowerCase()))
      );
    })
  );
}
```

### 🍴 forkJoin() - Attendre la Complétion

**`forkJoin()`** attend que **tous** les Observables se complètent, puis émet un tableau avec leurs dernières valeurs.

```typescript
import { forkJoin, of, timer } from 'rxjs';
import { map } from 'rxjs/operators';

const obs1$ = of('Résultat 1');
const obs2$ = timer(2000).pipe(map(() => 'Résultat 2'));
const obs3$ = timer(1000).pipe(map(() => 'Résultat 3'));

forkJoin([obs1$, obs2$, obs3$]).subscribe(([res1, res2, res3]) => {
  console.log('Tous terminés:', res1, res2, res3);
});

// Output (après 2 secondes):
// Tous terminés: Résultat 1, Résultat 2, Résultat 3
```

**Diagramme de marbre :**
```
obs1$: (Résultat 1)|
obs2$: -----(Résultat 2)|
obs3$: --(Résultat 3)|
fork:  -----[Résultat 1, Résultat 2, Résultat 3]|
```

#### Cas d'usage pratiques

```typescript
// Charger plusieurs ressources en parallèle
function loadDashboardData(): Observable<DashboardData> {
  return forkJoin({
    users: this.http.get<User[]>('/api/users'),
    stats: this.http.get<Stats>('/api/stats'),
    notifications: this.http.get<Notification[]>('/api/notifications'),
    settings: this.http.get<Settings>('/api/settings')
  });
}

// Utilisation
this.loadDashboardData().subscribe(data => {
  console.log('Users:', data.users);
  console.log('Stats:', data.stats);
  console.log('Notifications:', data.notifications);
  console.log('Settings:', data.settings);
});

// Sauvegarder plusieurs entités
function saveAll(items: Item[]): Observable<Item[]> {
  const saves$ = items.map(item => 
    this.http.post<Item>('/api/items', item)
  );
  
  return forkJoin(saves$);
}

saveAll(items).subscribe(
  savedItems => console.log('Tout sauvegardé:', savedItems),
  error => console.error('Erreur de sauvegarde:', error)
);
```

### 📊 Comparaison des Opérateurs de Combinaison

| Opérateur | Quand émet | Attend la complétion | Ordre | Cas d'usage |
|-----------|------------|---------------------|-------|-------------|
| **merge** | Dès qu'une source émet | ❌ Non | Ordre d'arrivée | Combiner événements |
| **concat** | Après chaque complétion | ✅ Oui (séquentiel) | Ordre strict | Opérations séquentielles |
| **race** | Au premier qui émet | ❌ Non | Premier seulement | Timeout, fastest source |
| **zip** | Quand toutes ont émis N fois | ❌ Non | Par index | Synchroniser flux |
| **combineLatest** | Dès qu'une source émet (après la première de toutes) | ❌ Non | Dernières valeurs | Formulaires, filtres |
| **forkJoin** | Quand toutes complètes | ✅ Oui (parallèle) | Dernières valeurs | Requêtes HTTP parallèles |

---

## 5.2 Opérateurs de Jointure

### 🔗 withLatestFrom() - Combiner avec le Dernier

**`withLatestFrom()`** combine l'Observable source avec les **dernières valeurs** d'autres Observables, mais n'émet que quand la **source** émet.

```typescript
import { interval, fromEvent } from 'rxjs';
import { withLatestFrom, map } from 'rxjs/operators';

const clicks$ = fromEvent(document, 'click');
const timer$ = interval(1000);

clicks$.pipe(
  withLatestFrom(timer$),
  map(([click, time]) => time)
).subscribe(time => {
  console.log(`Cliqué à ${time} secondes`);
});

// N'émet que lors des clics, avec le dernier timer
```

**Diagramme de marbre :**
```
clicks$: ----c------c--c---------c---|
timer$:  --0--1--2--3--4--5--6--7--8-|
result:  ----[c,1]--[c,3][c,4]--[c,7]|
         └─ Émet seulement quand clicks$ émet
```

#### Cas d'usage pratiques

```typescript
// Sauvegarder avec le contexte actuel
@Component({
  selector: 'app-editor',
  template: `<button (click)="save()">Sauvegarder</button>`
})
export class EditorComponent {
  private save$ = new Subject<void>();
  private currentUser$ = this.authService.currentUser$;
  private currentDocument$ = this.documentService.current$;
  
  constructor() {
    this.save$.pipe(
      withLatestFrom(this.currentUser$, this.currentDocument$),
      switchMap(([_, user, doc]) => {
        return this.api.save({
          document: doc,
          userId: user.id,
          timestamp: Date.now()
        });
      })
    ).subscribe();
  }
  
  save() {
    this.save$.next();
  }
}

// Filtrer avec état actuel
const searchClicks$ = fromEvent(searchButton, 'click');
const currentFilter$ = this.filterService.currentFilter$;

searchClicks$.pipe(
  withLatestFrom(currentFilter$),
  switchMap(([_, filter]) => this.api.search(filter))
).subscribe();
```

### ⚡ startWith() - Valeur Initiale

**`startWith()`** émet une valeur **immédiatement** avant que l'Observable source ne commence à émettre.

```typescript
import { interval } from 'rxjs';
import { startWith, map, take } from 'rxjs/operators';

const timer$ = interval(1000).pipe(
  take(3),
  map(n => `Tick ${n}`)
);

timer$.pipe(
  startWith('Début!')
).subscribe(console.log);

// Output:
// Début! (immédiat)
// Tick 0 (après 1s)
// Tick 1 (après 2s)
// Tick 2 (après 3s)
```

**Diagramme de marbre :**
```
source:     --1--2--3--|
startWith:  0--1--2--3--|
            └─ Valeur initiale
```

#### Cas d'usage pratiques

```typescript
// Formulaire avec valeur par défaut
@Component({
  selector: 'app-search',
  template: `
    <input [formControl]="search" placeholder="Rechercher...">
    <div *ngFor="let result of results$ | async">{{ result }}</div>
  `
})
export class SearchComponent {
  search = new FormControl('');
  
  results$ = this.search.valueChanges.pipe(
    startWith(''),  // Valeur initiale vide
    debounceTime(300),
    switchMap(term => this.api.search(term))
  );
}

// combineLatest qui émet immédiatement
const filter1$ = this.filter1Control.valueChanges.pipe(startWith(''));
const filter2$ = this.filter2Control.valueChanges.pipe(startWith(''));

combineLatest([filter1$, filter2$]).pipe(
  // Émet immédiatement avec les valeurs par défaut
  switchMap(([f1, f2]) => this.loadData(f1, f2))
).subscribe();
```

### 🏁 endWith() - Valeur Finale

**`endWith()`** émet une valeur **après** que l'Observable source se complète.

```typescript
import { of } from 'rxjs';
import { endWith } from 'rxjs/operators';

const numbers$ = of(1, 2, 3);

numbers$.pipe(
  endWith('Fin!')
).subscribe(console.log);

// Output:
// 1
// 2
// 3
// Fin!
```

**Diagramme de marbre :**
```
source:  --1--2--3--|
endWith: --1--2--3--FIN|
                    └─ Valeur finale
```

#### Cas d'usage pratiques

```typescript
// Ajouter un message de complétion
const loading$ = this.api.loadData().pipe(
  map(data => `Loaded: ${data.length} items`),
  endWith('Chargement terminé!')
);

// Séquence avec message final
const steps$ = concat(
  of('Étape 1: Initialisation'),
  timer(1000).pipe(mapTo('Étape 2: Connexion')),
  timer(2000).pipe(mapTo('Étape 3: Chargement'))
).pipe(
  endWith('✅ Processus terminé!')
);
```

### 🔄 pairwise() - Paires Consécutives

**`pairwise()`** émet la valeur **actuelle et la précédente** sous forme de paire.

```typescript
import { fromEvent } from 'rxjs';
import { pairwise, map } from 'rxjs/operators';

const clicks$ = fromEvent(document, 'click');

clicks$.pipe(
  map(event => ({ x: event.clientX, y: event.clientY })),
  pairwise(),
  map(([prev, curr]) => {
    const distance = Math.sqrt(
      Math.pow(curr.x - prev.x, 2) + Math.pow(curr.y - prev.y, 2)
    );
    return `Distance: ${distance.toFixed(2)}px`;
  })
).subscribe(console.log);
```

**Diagramme de marbre :**
```
source:   --1--2--3--4--5--|
pairwise: ----[1,2][2,3][3,4][4,5]|
          └─ Première valeur ignorée
```

#### Cas d'usage pratiques

```typescript
// Détecter les changements de prix
const price$ = new BehaviorSubject(100);

price$.pipe(
  pairwise(),
  map(([prev, curr]) => {
    const change = curr - prev;
    const percent = (change / prev) * 100;
    return {
      previous: prev,
      current: curr,
      change: change,
      changePercent: percent.toFixed(2)
    };
  })
).subscribe(data => {
  console.log(`Prix: ${data.current} (${data.changePercent}%)`);
});

// Détecter la direction de scroll
const scroll$ = fromEvent(window, 'scroll');

scroll$.pipe(
  map(() => window.scrollY),
  pairwise(),
  map(([prev, curr]) => curr > prev ? 'down' : 'up')
).subscribe(direction => {
  console.log('Scroll direction:', direction);
});
```

---

## 5.3 Cas d'Usage Pratiques

### 🎨 Formulaires Réactifs

#### Validation Dépendante

```typescript
@Component({
  selector: 'app-registration-form',
  template: `
    <form [formGroup]="form">
      <input formControlName="password" type="password">
      <input formControlName="confirmPassword" type="password">
      <div *ngIf="passwordMismatch$ | async">
        Les mots de passe ne correspondent pas
      </div>
    </form>
  `
})
export class RegistrationFormComponent {
  form = new FormGroup({
    password: new FormControl(''),
    confirmPassword: new FormControl('')
  });
  
  passwordMismatch$ = combineLatest([
    this.form.get('password').valueChanges.pipe(startWith('')),
    this.form.get('confirmPassword').valueChanges.pipe(startWith(''))
  ]).pipe(
    map(([password, confirm]) => {
      return password !== confirm && confirm.length > 0;
    })
  );
}
```

#### Recherche avec Filtres Multiples

```typescript
@Component({
  selector: 'app-product-search',
  template: `
    <input [formControl]="searchTerm" placeholder="Rechercher...">
    <select [formControl]="category">
      <option value="">Toutes catégories</option>
      <option value="books">Livres</option>
      <option value="electronics">Électronique</option>
    </select>
    <input [formControl]="minPrice" type="number" placeholder="Prix min">
    <input [formControl]="maxPrice" type="number" placeholder="Prix max">
    
    <div *ngFor="let product of products$ | async">
      {{ product.name }} - {{ product.price }}€
    </div>
  `
})
export class ProductSearchComponent {
  searchTerm = new FormControl('');
  category = new FormControl('');
  minPrice = new FormControl(0);
  maxPrice = new FormControl(1000);
  
  products$ = combineLatest([
    this.searchTerm.valueChanges.pipe(startWith(''), debounceTime(300)),
    this.category.valueChanges.pipe(startWith('')),
    this.minPrice.valueChanges.pipe(startWith(0)),
    this.maxPrice.valueChanges.pipe(startWith(1000))
  ]).pipe(
    switchMap(([term, cat, min, max]) => {
      return this.productService.search({
        term,
        category: cat,
        minPrice: min,
        maxPrice: max
      });
    }),
    shareReplay(1)
  );
}
```

### 📡 Requêtes Parallèles

#### Dashboard avec Données Multiples

```typescript
@Component({
  selector: 'app-dashboard',
  template: `
    <div *ngIf="dashboardData$ | async as data">
      <h2>Utilisateurs: {{ data.users.length }}</h2>
      <h2>Ventes: {{ data.sales.total }}€</h2>
      <h2>Notifications: {{ data.notifications.length }}</h2>
    </div>
  `
})
export class DashboardComponent implements OnInit {
  dashboardData$: Observable<DashboardData>;
  
  ngOnInit() {
    this.dashboardData$ = forkJoin({
      users: this.http.get<User[]>('/api/users'),
      sales: this.http.get<Sales>('/api/sales'),
      notifications: this.http.get<Notification[]>('/api/notifications'),
      settings: this.http.get<Settings>('/api/settings')
    }).pipe(
      catchError(error => {
        console.error('Erreur chargement dashboard:', error);
        return of({
          users: [],
          sales: { total: 0 },
          notifications: [],
          settings: {}
        });
      })
    );
  }
}
```

### 🔄 Dépendances entre Requêtes

#### Chargement en Cascade

```typescript
@Injectable()
export class OrderService {
  // Charger commande → détails → client → historique
  loadOrderComplete(orderId: string): Observable<CompleteOrder> {
    return this.http.get<Order>(`/api/orders/${orderId}`).pipe(
      switchMap(order => {
        // Charger les détails et le client en parallèle
        return forkJoin({
          order: of(order),
          details: this.http.get(`/api/orders/${orderId}/details`),
          customer: this.http.get(`/api/customers/${order.customerId}`)
        });
      }),
      switchMap(({ order, details, customer }) => {
        // Charger l'historique du client
        return this.http.get(`/api/customers/${customer.id}/history`).pipe(
          map(history => ({
            order,
            details,
            customer,
            history
          }))
        );
      })
    );
  }
}
```

#### Auto-Save avec Debounce

```typescript
@Component({
  selector: 'app-editor',
  template: `
    <textarea [formControl]="content"></textarea>
    <div *ngIf="saveStatus$ | async as status">
      {{ status }}
    </div>
  `
})
export class EditorComponent {
  content = new FormControl('');
  private documentId = 'doc-123';
  
  saveStatus$ = this.content.valueChanges.pipe(
    debounceTime(1000),
    distinctUntilChanged(),
    switchMap(content => {
      return this.api.saveDocument(this.documentId, content).pipe(
        map(() => '✅ Sauvegardé'),
        startWith('💾 Sauvegarde en cours...'),
        catchError(() => of('❌ Erreur de sauvegarde'))
      );
    }),
    startWith('Prêt')
  );
}
```

### 🎮 Gestion d'État Complexe

```typescript
interface AppState {
  user: User | null;
  cart: CartItem[];
  notifications: Notification[];
  isLoading: boolean;
}

@Injectable({ providedIn: 'root' })
export class StateService {
  private userSubject = new BehaviorSubject<User | null>(null);
  private cartSubject = new BehaviorSubject<CartItem[]>([]);
  private notificationsSubject = new BehaviorSubject<Notification[]>([]);
  private loadingSubject = new BehaviorSubject<boolean>(false);
  
  // État global
  state$ = combineLatest([
    this.userSubject.asObservable(),
    this.cartSubject.asObservable(),
    this.notificationsSubject.asObservable(),
    this.loadingSubject.asObservable()
  ]).pipe(
    map(([user, cart, notifications, isLoading]) => ({
      user,
      cart,
      notifications,
      isLoading
    })),
    shareReplay(1)
  );
  
  // Sélecteurs
  user$ = this.state$.pipe(
    map(state => state.user),
    distinctUntilChanged()
  );
  
  cartTotal$ = this.state$.pipe(
    map(state => state.cart.reduce((sum, item) => sum + item.price, 0)),
    distinctUntilChanged()
  );
  
  unreadNotifications$ = this.state$.pipe(
    map(state => state.notifications.filter(n => !n.read).length),
    distinctUntilChanged()
  );
  
  // Actions
  setUser(user: User) {
    this.userSubject.next(user);
  }
  
  addToCart(item: CartItem) {
    const current = this.cartSubject.value;
    this.cartSubject.next([...current, item]);
  }
  
  removeFromCart(itemId: string) {
    const current = this.cartSubject.value;
    this.cartSubject.next(current.filter(item => item.id !== itemId));
  }
}
```

---

## 5.4 Exercice Pratique

### 🎯 Challenge : Dashboard avec Données Multiples

Créez un dashboard qui :
1. Charge plusieurs sources de données en parallèle (utilisateurs, ventes, produits)
2. Affiche un état de chargement pendant la récupération
3. Gère les erreurs individuelles sans bloquer l'affichage
4. Rafraîchit automatiquement toutes les 30 secondes
5. Permet de filtrer les données avec des critères multiples

### 💡 Solution

```typescript
// models/dashboard.model.ts
export interface User {
  id: string;
  name: string;
  email: string;
  role: string;
}

export interface Sale {
  id: string;
  amount: number;
  date: Date;
  userId: string;
}

export interface Product {
  id: string;
  name: string;
  category: string;
  stock: number;
  price: number;
}

export interface DashboardData {
  users: User[];
  sales: Sale[];
  products: Product[];
  stats: {
    totalUsers: number;
    totalSales: number;
    totalRevenue: number;
    lowStock: number;
  };
}

// dashboard.service.ts
import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, forkJoin, interval, combineLatest, BehaviorSubject } from 'rxjs';
import { map, switchMap, catchError, shareReplay, startWith } from 'rxjs/operators';

@Injectable({ providedIn: 'root' })
export class DashboardService {
  private refreshTrigger$ = new BehaviorSubject<void>(undefined);
  private categoryFilter$ = new BehaviorSubject<string>('all');
  private dateRange$ = new BehaviorSubject<{ start: Date; end: Date } | null>(null);
  
  constructor(private http: HttpClient) {}
  
  // Chargement des données avec auto-refresh
  dashboardData$: Observable<DashboardData> = combineLatest([
    this.refreshTrigger$,
    interval(30000).pipe(startWith(0)) // Refresh auto toutes les 30s
  ]).pipe(
    switchMap(() => this.loadAllData()),
    shareReplay(1)
  );
  
  // Données filtrées
  filteredData$ = combineLatest([
    this.dashboardData$,
    this.categoryFilter$,
    this.dateRange$
  ]).pipe(
    map(([data, category, dateRange]) => this.applyFilters(data, category, dateRange)),
    shareReplay(1)
  );
  
  private loadAllData(): Observable<DashboardData> {
    return forkJoin({
      users: this.loadUsers(),
      sales: this.loadSales(),
      products: this.loadProducts()
    }).pipe(
      map(({ users, sales, products }) => {
        const stats = this.calculateStats(users, sales, products);
        return { users, sales, products, stats };
      }),
      catchError(error => {
        console.error('Erreur chargement dashboard:', error);
        return of({
          users: [],
          sales: [],
          products: [],
          stats: {
            totalUsers: 0,
            totalSales: 0,
            totalRevenue: 0,
            lowStock: 0
          }
        });
      })
    );
  }
  
  private loadUsers(): Observable<User[]> {
    return this.http.get<User[]>('/api/users').pipe(
      catchError(error => {
        console.error('Erreur chargement users:', error);
        return of([]);
      })
    );
  }
  
  private loadSales(): Observable<Sale[]> {
    return this.http.get<Sale[]>('/api/sales').pipe(
      catchError(error => {
        console.error('Erreur chargement sales:', error);
        return of([]);
      })
    );
  }
  
  private loadProducts(): Observable<Product[]> {
    return this.http.get<Product[]>('/api/products').pipe(
      catchError(error => {
        console.error('Erreur chargement products:', error);
        return of([]);
      })
    );
  }
  
  private calculateStats(users: User[], sales: Sale[], products: Product[]) {
    return {
      totalUsers: users.length,
      totalSales: sales.length,
      totalRevenue: sales.reduce((sum, sale) => sum + sale.amount, 0),
      lowStock: products.filter(p => p.stock < 10).length
    };
  }
  
  private applyFilters(
    data: DashboardData,
    category: string,
    dateRange: { start: Date; end: Date } | null
  ): DashboardData {
    let filteredProducts = data.products;
    let filteredSales = data.sales;
    
    // Filtre par catégorie
    if (category !== 'all') {
      filteredProducts = filteredProducts.filter(p => p.category === category);
    }
    
    // Filtre par date
    if (dateRange) {
      filteredSales = filteredSales.filter(s => 
        s.date >= dateRange.start && s.date <= dateRange.end
      );
    }
    
    const stats = this.calculateStats(data.users, filteredSales, filteredProducts);
    
    return {
      users: data.users,
      sales: filteredSales,
      products: filteredProducts,
      stats
    };
  }
  
  // Actions
  refresh() {
    this.refreshTrigger$.next();
  }
  
  setCategory(category: string) {
    this.categoryFilter$.next(category);
  }
  
  setDateRange(start: Date, end: Date) {
    this.dateRange$.next({ start, end });
  }
  
  clearFilters() {
    this.categoryFilter$.next('all');
    this.dateRange$.next(null);
  }
}

// dashboard.component.ts
import { Component, OnInit } from '@angular/core';
import { DashboardService } from './dashboard.service';

@Component({
  selector: 'app-dashboard',
  template: `
    <div class="dashboard">
      <div class="header">
        <h1>Dashboard</h1>
        <button (click)="refresh()">🔄 Rafraîchir</button>
      </div>
      
      <div class="filters">
        <select (change)="onCategoryChange($event.target.value)">
          <option value="all">Toutes catégories</option>
          <option value="electronics">Électronique</option>
          <option value="books">Livres</option>
          <option value="clothing">Vêtements</option>
        </select>
        
        <button (click)="clearFilters()">Effacer filtres</button>
      </div>
      
      <div class="stats" *ngIf="data$ | async as data">
        <div class="stat-card">
          <h3>Utilisateurs</h3>
          <p class="stat-value">{{ data.stats.totalUsers }}</p>
        </div>
        
        <div class="stat-card">
          <h3>Ventes</h3>
          <p class="stat-value">{{ data.stats.totalSales }}</p>
        </div>
        
        <div class="stat-card">
          <h3>Revenu Total</h3>
          <p class="stat-value">{{ data.stats.totalRevenue | currency:'EUR' }}</p>
        </div>
        
        <div class="stat-card warning" *ngIf="data.stats.lowStock > 0">
          <h3>⚠️ Stock Faible</h3>
          <p class="stat-value">{{ data.stats.lowStock }}</p>
        </div>
      </div>
      
      <div class="content" *ngIf="data$ | async as data">
        <div class="section">
          <h2>Produits ({{ data.products.length }})</h2>
          <div class="product-grid">
            <div *ngFor="let product of data.products" class="product-card">
              <h4>{{ product.name }}</h4>
              <p>{{ product.category }}</p>
              <p>Stock: {{ product.stock }}</p>
              <p class="price">{{ product.price | currency:'EUR' }}</p>
            </div>
          </div>
        </div>
        
        <div class="section">
          <h2>Ventes Récentes ({{ data.sales.length }})</h2>
          <table>
            <thead>
              <tr>
                <th>Date</th>
                <th>Montant</th>
                <th>Utilisateur</th>
              </tr>
            </thead>
            <tbody>
              <tr *ngFor="let sale of data.sales | slice:0:10">
                <td>{{ sale.date | date:'short' }}</td>
                <td>{{ sale.amount | currency:'EUR' }}</td>
                <td>{{ sale.userId }}</td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>
      
      <div class="loading" *ngIf="!(data$ | async)">
        Chargement des données...
      </div>
    </div>
  `,
  styles: [`
    .dashboard {
      padding: 20px;
      max-width: 1400px;
      margin: 0 auto;
    }
    
    .header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 20px;
    }
    
    .filters {
      margin-bottom: 20px;
      display: flex;
      gap: 10px;
    }
    
    .stats {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
      gap: 20px;
      margin-bottom: 30px;
    }
    
    .stat-card {
      background: white;
      padding: 20px;
      border-radius: 8px;
      box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
    
    .stat-card.warning {
      background: #fff3cd;
      border-left: 4px solid #ffc107;
    }
    
    .stat-card h3 {
      margin: 0 0 10px 0;
      font-size: 14px;
      color: #666;
    }
    
    .stat-value {
      font-size: 32px;
      font-weight: bold;
      margin: 0;
      color: #333;
    }
    
    .section {
      margin-bottom: 30px;
      background: white;
      padding: 20px;
      border-radius: 8px;
      box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
    
    .product-grid {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
      gap: 15px;
      margin-top: 15px;
    }
    
    .product-card {
      border: 1px solid #ddd;
      padding: 15px;
      border-radius: 4px;
    }
    
    .product-card h4 {
      margin: 0 0 10px 0;
    }
    
    .price {
      font-weight: bold;
      color: #2196f3;
    }
    
    table {
      width: 100%;
      border-collapse: collapse;
      margin-top: 15px;
    }
    
    th, td {
      padding: 12px;
      text-align: left;
      border-bottom: 1px solid #ddd;
    }
    
    th {
      background: #f5f5f5;
      font-weight: bold;
    }
    
    .loading {
      text-align: center;
      padding: 50px;
      font-size: 18px;
      color: #666;
    }
    
    button {
      padding: 10px 20px;
      background: #2196f3;
      color: white;
      border: none;
      border-radius: 4px;
      cursor: pointer;
    }
    
    button:hover {
      background: #1976d2;
    }
    
    select {
      padding: 8px 12px;
      border: 1px solid #ddd;
      border-radius: 4px;
    }
  `]
})
export class DashboardComponent implements OnInit {
  data$ = this.dashboardService.filteredData$;
  
  constructor(private dashboardService: DashboardService) {}
  
  ngOnInit() {}
  
  refresh() {
    this.dashboardService.refresh();
  }
  
  onCategoryChange(category: string) {
    this.dashboardService.setCategory(category);
  }
  
  clearFilters() {
    this.dashboardService.clearFilters();
  }
}
```

---

## 📝 Questions/Réponses d'Entretien

### Q1 : Quelle est la différence entre `merge()` et `concat()` ?

**Réponse :**
> **`merge()`** :
> - Combine plusieurs Observables en parallèle
> - Émet les valeurs dès qu'elles arrivent (ordre d'arrivée)
> - Toutes les sources s'exécutent simultanément
> - Cas d'usage : combiner des événements indépendants
> 
> **`concat()`** :
> - Combine des Observables séquentiellement
> - Attend que chaque Observable se complète avant de passer au suivant
> - Préserve l'ordre strict
> - Cas d'usage : opérations qui doivent s'exécuter dans un ordre précis
> 
> ```typescript
> // merge : en parallèle
> merge(timer(1000), timer(500)).subscribe();
> // Émet à 500ms, puis à 1000ms
> 
> // concat : séquentiel
> concat(timer(1000), timer(500)).subscribe();
> // Émet à 1000ms, puis à 1500ms (1000+500)
> ```

### Q2 : Quand utiliser `combineLatest()` vs `zip()` ?

**Réponse :**
> **`combineLatest()`** :
> - Émet à chaque fois qu'une source émet (après la première émission de toutes)
> - Combine les **dernières valeurs** de chaque source
> - Cas d'usage : formulaires réactifs, filtres multiples, état dépendant
> 
> **`zip()`** :
> - Émet quand toutes les sources ont émis leur Nième valeur
> - Combine par **index** (première avec première, etc.)
> - Cas d'usage : synchroniser des flux, paires de valeurs correspondantes
> 
> ```typescript
> const a$ = interval(1000); // 0, 1, 2, 3...
> const b$ = interval(2000); // 0, 1, 2...
> 
> // combineLatest : émet à 1s, 2s, 3s, 4s...
> combineLatest([a$, b$]); // [0,0], [1,0], [2,1], [3,1], [4,2]...
> 
> // zip : émet à 2s, 4s, 6s...
> zip(a$, b$); // [0,0], [1,1], [2,2]...
> ```

### Q3 : Qu'est-ce que `forkJoin()` et quand l'utiliser ?

**Réponse :**
> `forkJoin()` attend que **tous les Observables se complètent**, puis émet un tableau avec leurs **dernières valeurs**.
> 
> **Caractéristiques** :
> - Équivalent de `Promise.all()` pour RxJS
> - N'émet qu'une seule fois, à la fin
> - Si un Observable ne se complète jamais, `forkJoin()` n'émettra jamais
> - Si un Observable émet une erreur, `forkJoin()` émet l'erreur
> 
> **Cas d'usage** :
> - Requêtes HTTP parallèles
> - Chargement de plusieurs ressources
> - Attendre plusieurs opérations avant de continuer
> 
> ```typescript
> // Charger plusieurs ressources en parallèle
> forkJoin({
>   users: this.http.get('/api/users'),
>   products: this.http.get('/api/products'),
>   settings: this.http.get('/api/settings')
> }).subscribe(({ users, products, settings }) => {
>   // Toutes les données sont chargées
> });
> ```

### Q4 : Quelle est la différence entre `withLatestFrom()` et `combineLatest()` ?

**Réponse :**
> **`withLatestFrom()`** :
> - N'émet que quand l'Observable **source** émet
> - Combine avec les dernières valeurs des autres
> - L'Observable source contrôle le timing
> 
> **`combineLatest()`** :
> - Émet quand **n'importe quel** Observable émet
> - Tous les Observables sont égaux
> - Aucun Observable ne contrôle le timing
> 
> ```typescript
> const clicks$ = fromEvent(button, 'click');
> const timer$ = interval(1000);
> 
> // withLatestFrom : émet seulement lors des clics
> clicks$.pipe(
>   withLatestFrom(timer$)
> ).subscribe(([click, time]) => {
>   console.log(`Cliqué à ${time}s`);
> });
> 
> // combineLatest : émet à chaque clic ET chaque seconde
> combineLatest([clicks$, timer$]).subscribe(([click, time]) => {
>   console.log(`${time}s`);
> });
> ```

### Q5 : Pourquoi utiliser `startWith()` ?

**Réponse :**
> `startWith()` émet une valeur **immédiatement** avant que l'Observable source ne commence.
> 
> **Cas d'usage** :
> 
> **1. Valeur initiale pour combineLatest**
> ```typescript
> // Sans startWith : n'émet pas avant que les deux aient émis
> combineLatest([
>   input1$.valueChanges,
>   input2$.valueChanges
> ]); // ❌ Attend les deux premières valeurs
> 
> // Avec startWith : émet immédiatement
> combineLatest([
>   input1$.valueChanges.pipe(startWith('')),
>   input2$.valueChanges.pipe(startWith(''))
> ]); // ✅ Émet tout de suite avec valeurs par défaut
> ```
> 
> **2. État initial dans un flux**
> ```typescript
> const loading$ = this.api.loadData().pipe(
>   map(() => false),
>   startWith(true) // Commence par true (loading)
> );
> ```

### Q6 : Comment gérer les erreurs avec `forkJoin()` ?

**Réponse :**
> Si un Observable dans `forkJoin()` émet une erreur, **tout le forkJoin émet l'erreur** et les autres requêtes sont ignorées.
> 
> **Solutions** :
> 
> **1. catchError sur chaque Observable**
> ```typescript
> forkJoin({
>   users: this.http.get('/api/users').pipe(
>     catchError(() => of([]))
>   ),
>   products: this.http.get('/api/products').pipe(
>     catchError(() => of([]))
>   )
> });
> // ✅ Continue même si une requête échoue
> ```
> 
> **2. catchError sur le forkJoin entier**
> ```typescript
> forkJoin({
>   users: this.http.get('/api/users'),
>   products: this.http.get('/api/products')
> }).pipe(
>   catchError(error => {
>     return of({ users: [], products: [] });
>   })
> });
> ```

### Q7 : Quelle est la différence entre `race()` et `merge()` ?

**Réponse :**
> **`race()`** :
> - Ne souscrit qu'au **premier** Observable qui émet
> - Ignore complètement les autres
> - Cas d'usage : timeout, source la plus rapide
> 
> **`merge()`** :
> - Souscrit à **tous** les Observables
> - Émet toutes les valeurs de tous les flux
> - Cas d'usage : combiner plusieurs sources d'événements
> 
> ```typescript
> const slow$ = timer(3000).pipe(mapTo('slow'));
> const fast$ = timer(1000).pipe(mapTo('fast'));
> 
> // race : émet seulement 'fast'
> race(slow$, fast$).subscribe(console.log);
> // Output: fast
> 
> // merge : émet les deux
> merge(slow$, fast$).subscribe(console.log);
> // Output: fast, slow
> ```

### Q8 : Qu'est-ce que `pairwise()` et à quoi sert-il ?

**Réponse :**
> `pairwise()` émet la **valeur actuelle et la valeur précédente** sous forme de paire `[prev, curr]`.
> 
> **Caractéristiques** :
> - Ne commence à émettre qu'à partir de la **deuxième** valeur
> - Utile pour comparer des valeurs consécutives
> 
> **Cas d'usage** :
> 
> **1. Détecter les changements**
> ```typescript
> price$.pipe(
>   pairwise(),
>   map(([prev, curr]) => curr - prev)
> ).subscribe(change => {
>   console.log('Changement de prix:', change);
> });
> ```
> 
> **2. Détecter la direction de scroll**
> ```typescript
> scroll$.pipe(
>   pairwise(),
>   map(([prev, curr]) => curr > prev ? 'down' : 'up')
> ).subscribe(direction => {
>   console.log('Direction:', direction);
> });
> ```

### Q9 : Comment implémenter un système de filtres multiples ?

**Réponse :**
> Utilisez `combineLatest()` avec plusieurs BehaviorSubjects pour les filtres.
> 
> ```typescript
> @Injectable()
> export class FilterService {
>   private searchTerm$ = new BehaviorSubject<string>('');
>   private category$ = new BehaviorSubject<string>('all');
>   private priceRange$ = new BehaviorSubject<[number, number]>([0, 1000]);
>   
>   filteredProducts$ = combineLatest([
>     this.products$,
>     this.searchTerm$,
>     this.category$,
>     this.priceRange$
>   ]).pipe(
>     map(([products, term, cat, [min, max]]) => {
>       return products.filter(p => 
>         p.name.includes(term) &&
>         (cat === 'all' || p.category === cat) &&
>         p.price >= min && p.price <= max
>       );
>     })
>   );
>   
>   setSearchTerm(term: string) {
>     this.searchTerm$.next(term);
>   }
>   
>   setCategory(category: string) {
>     this.category$.next(category);
>   }
>   
>   setPriceRange(min: number, max: number) {
>     this.priceRange$.next([min, max]);
>   }
> }
> ```

### Q10 : Quand utiliser `shareReplay()` avec les opérateurs de combinaison ?

**Réponse :**
> Utilisez `shareReplay()` pour **éviter les exécutions multiples** quand plusieurs composants souscrivent au même Observable.
> 
> **Sans shareReplay** :
> ```typescript
> dashboardData$ = forkJoin({
>   users: this.http.get('/api/users'),
>   products: this.http.get('/api/products')
> });
> 
> // ❌ Chaque souscription déclenche de nouvelles requêtes HTTP
> this.dashboardData$.subscribe(); // Requêtes 1
> this.dashboardData$.subscribe(); // Requêtes 2
> ```
> 
> **Avec shareReplay** :
> ```typescript
> dashboardData$ = forkJoin({
>   users: this.http.get('/api/users'),
>   products: this.http.get('/api/products')
> }).pipe(shareReplay(1));
> 
> // ✅ Les requêtes ne sont faites qu'une seule fois
> this.dashboardData$.subscribe(); // Requêtes
> this.dashboardData$.subscribe(); // Cache
> ```

---

## 🎯 Points Clés du Chapitre 5

### Opérateurs de Combinaison
✅ **merge()** : Combine en parallèle, ordre d'arrivée
✅ **concat()** : Combine séquentiellement, ordre strict
✅ **race()** : Premier à émettre gagne
✅ **zip()** : Combine par index (paires)
✅ **combineLatest()** : Dernières valeurs, émet à chaque changement
✅ **forkJoin()** : Attend toutes les complétions, comme Promise.all()

### Opérateurs de Jointure
✅ **withLatestFrom()** : Combine avec dernière valeur, contrôlé par la source
✅ **startWith()** : Valeur initiale immédiate
✅ **endWith()** : Valeur finale après complétion
✅ **pairwise()** : Paires de valeurs consécutives

### Différences Clés
✅ **combineLatest vs zip** : Dernières valeurs vs paires par index
✅ **withLatestFrom vs combineLatest** : Source contrôle vs tous égaux
✅ **merge vs concat** : Parallèle vs séquentiel
✅ **race vs merge** : Premier vs tous

### Cas d'Usage
✅ **Formulaires réactifs** : combineLatest + startWith
✅ **Requêtes parallèles** : forkJoin + catchError
✅ **Dépendances** : switchMap + forkJoin
✅ **Filtres multiples** : combineLatest + BehaviorSubjects
✅ **Auto-refresh** : interval + switchMap + shareReplay

---