
## [**..**](./00_index.md)

# Chapitre 2 : Création d'Observables

## 📑 Table des matières
- [2.1 Opérateurs de Création](#21-opérateurs-de-création)
- [2.2 Création Personnalisée](#22-création-personnalisée)
- [2.3 Subjects](#23-subjects)
- [2.4 Exercice Pratique](#24-exercice-pratique)
- [Questions/Réponses d'Entretien](#-questionsréponses-dentretien)

---

## 2.1 Opérateurs de Création

### 🎯 of() - Émettre des Valeurs

L'opérateur **`of()`** crée un Observable qui émet les valeurs fournies en arguments, puis se complète.

```typescript
import { of } from 'rxjs';

// Émettre plusieurs valeurs
const numbers$ = of(1, 2, 3, 4, 5);

numbers$.subscribe({
  next: value => console.log('Valeur:', value),
  complete: () => console.log('Terminé!')
});

// Output:
// Valeur: 1
// Valeur: 2
// Valeur: 3
// Valeur: 4
// Valeur: 5
// Terminé!
```

**Diagramme de marbre :**
```
of(1,2,3): (123)|
           └──┘└─ complete
           valeurs
```

#### Cas d'usage pratiques

```typescript
// Retourner une valeur par défaut
function getUserById(id: string): Observable<User> {
  const user = cache.get(id);
  if (user) {
    return of(user);  // ✅ Retourne immédiatement
  }
  return this.http.get<User>(`/api/users/${id}`);
}

// Émettre des objets complexes
const users$ = of(
  { id: 1, name: 'Alice' },
  { id: 2, name: 'Bob' },
  { id: 3, name: 'Charlie' }
);

// Émettre différents types (union type)
const mixed$ = of(
  'string',
  42,
  true,
  { key: 'value' },
  [1, 2, 3]
);
```

### 📦 from() - Convertir des Structures

L'opérateur **`from()`** convertit un array, Promise, itérable ou Observable-like en Observable.

```typescript
import { from } from 'rxjs';

// 1. Depuis un Array
const array$ = from([10, 20, 30, 40, 50]);
array$.subscribe(console.log);
// Output: 10, 20, 30, 40, 50

// 2. Depuis une Promise
const promise = fetch('/api/data').then(r => r.json());
const fromPromise$ = from(promise);
fromPromise$.subscribe(data => console.log(data));

// 3. Depuis un String (itérable)
const string$ = from('Hello');
string$.subscribe(console.log);
// Output: H, e, l, l, o

// 4. Depuis un Set
const set = new Set([1, 2, 3, 3, 4]);
const fromSet$ = from(set);
fromSet$.subscribe(console.log);
// Output: 1, 2, 3, 4

// 5. Depuis un Map
const map = new Map([
  ['key1', 'value1'],
  ['key2', 'value2']
]);
const fromMap$ = from(map);
fromMap$.subscribe(console.log);
// Output: ['key1', 'value1'], ['key2', 'value2']
```

#### Différence of() vs from()

```typescript
// of() avec un array : émet l'array entier
of([1, 2, 3]).subscribe(console.log);
// Output: [1, 2, 3]

// from() avec un array : émet chaque élément
from([1, 2, 3]).subscribe(console.log);
// Output: 1, 2, 3

// of() avec plusieurs valeurs
of(1, 2, 3).subscribe(console.log);
// Output: 1, 2, 3
```

### ⏱️ interval() et timer() - Valeurs Temporelles

#### interval() - Émettre à intervalle régulier

```typescript
import { interval } from 'rxjs';
import { take } from 'rxjs/operators';

// Émet 0, 1, 2, 3... chaque seconde
const counter$ = interval(1000);

counter$.pipe(
  take(5)  // Prendre seulement 5 valeurs
).subscribe(value => console.log('Tick:', value));

// Output (chaque seconde):
// Tick: 0
// Tick: 1
// Tick: 2
// Tick: 3
// Tick: 4
```

**Diagramme de marbre :**
```
interval(1000): --0--1--2--3--4--5-->
                  └1s┘
```

#### timer() - Délai puis intervalle

```typescript
import { timer } from 'rxjs';

// Émet après 3 secondes, puis toutes les secondes
const delayed$ = timer(3000, 1000);

delayed$.pipe(take(5)).subscribe(console.log);
// Output:
// (après 3s) 0
// (après 4s) 1
// (après 5s) 2
// (après 6s) 3
// (après 7s) 4

// Seulement un délai (pas d'intervalle)
const onlyDelay$ = timer(2000);
onlyDelay$.subscribe(() => console.log('Exécuté après 2 secondes'));
```

**Diagramme de marbre :**
```
timer(3000, 1000): ----0--1--2--3-->
                   └3s┘└1s┘
```

#### Cas d'usage pratiques

```typescript
// Polling : rafraîchir des données toutes les 30 secondes
const polling$ = interval(30000).pipe(
  switchMap(() => this.http.get('/api/status'))
);

// Countdown timer
const countdown$ = timer(0, 1000).pipe(
  map(n => 60 - n),
  takeWhile(n => n >= 0)
);

countdown$.subscribe(seconds => {
  console.log(`${seconds} secondes restantes`);
});
```

### 🖱️ fromEvent() - Événements DOM

```typescript
import { fromEvent } from 'rxjs';
import { map, debounceTime } from 'rxjs/operators';

// 1. Événements de clic
const button = document.getElementById('myButton');
const clicks$ = fromEvent(button, 'click');

clicks$.subscribe(() => console.log('Button clicked!'));

// 2. Événements de saisie
const input = document.getElementById('searchInput');
const keyup$ = fromEvent<KeyboardEvent>(input, 'keyup');

keyup$.pipe(
  map(event => (event.target as HTMLInputElement).value),
  debounceTime(300)  // Attendre 300ms après la dernière frappe
).subscribe(value => {
  console.log('Recherche:', value);
});

// 3. Événements de scroll
const scroll$ = fromEvent(window, 'scroll');

scroll$.pipe(
  debounceTime(200)
).subscribe(() => {
  console.log('Scrolled to:', window.scrollY);
});

// 4. Événements de resize
const resize$ = fromEvent(window, 'resize');

resize$.pipe(
  debounceTime(300)
).subscribe(() => {
  console.log('Window size:', window.innerWidth, window.innerHeight);
});
```

#### Cas d'usage Angular

```typescript
import { Component, ElementRef, ViewChild, OnInit } from '@angular/core';
import { fromEvent } from 'rxjs';
import { debounceTime, map } from 'rxjs/operators';

@Component({
  selector: 'app-search',
  template: `
    <input #searchInput type="text" placeholder="Rechercher...">
    <div *ngFor="let result of results">{{ result }}</div>
  `
})
export class SearchComponent implements OnInit {
  @ViewChild('searchInput') searchInput: ElementRef;
  results: string[] = [];
  
  ngOnInit() {
    fromEvent(this.searchInput.nativeElement, 'keyup').pipe(
      map((event: any) => event.target.value),
      debounceTime(300)
    ).subscribe(query => {
      this.search(query);
    });
  }
  
  search(query: string) {
    // Logique de recherche
    console.log('Searching for:', query);
  }
}
```

### 🌐 ajax() - Requêtes HTTP

```typescript
import { ajax } from 'rxjs/ajax';
import { map, catchError } from 'rxjs/operators';
import { of } from 'rxjs';

// GET request
const users$ = ajax.getJSON<User[]>('https://api.example.com/users');

users$.subscribe(
  users => console.log('Users:', users),
  error => console.error('Error:', error)
);

// POST request
const newUser$ = ajax({
  url: 'https://api.example.com/users',
  method: 'POST',
  headers: {
    'Content-Type': 'application/json'
  },
  body: {
    name: 'John Doe',
    email: 'john@example.com'
  }
});

newUser$.pipe(
  map(response => response.response),
  catchError(error => {
    console.error('Error creating user:', error);
    return of(null);
  })
).subscribe(user => {
  console.log('Created user:', user);
});

// GET avec headers personnalisés
const protectedData$ = ajax({
  url: 'https://api.example.com/protected',
  method: 'GET',
  headers: {
    'Authorization': 'Bearer token123'
  }
});
```

**Note Angular** : Préférez `HttpClient` d'Angular qui retourne déjà des Observables.

```typescript
// Angular HttpClient (recommandé)
constructor(private http: HttpClient) {}

getUsers() {
  return this.http.get<User[]>('/api/users');
}
```

---

## 2.2 Création Personnalisée

### 🎯 new Observable() - Création Manuelle

```typescript
import { Observable } from 'rxjs';

// Observable simple
const custom$ = new Observable<number>(subscriber => {
  console.log('Observable started');
  
  // Émettre des valeurs
  subscriber.next(1);
  subscriber.next(2);
  subscriber.next(3);
  
  // Terminer le flux
  subscriber.complete();
  
  // Code de nettoyage (cleanup)
  return () => {
    console.log('Observable cleaned up');
  };
});

// Souscription
const subscription = custom$.subscribe({
  next: value => console.log('Value:', value),
  complete: () => console.log('Complete!')
});

// Output:
// Observable started
// Value: 1
// Value: 2
// Value: 3
// Complete!
// Observable cleaned up
```

### ⚡ Observable Asynchrone

```typescript
const async$ = new Observable<string>(subscriber => {
  console.log('Starting async operation');
  
  // Opération asynchrone
  const timeoutId = setTimeout(() => {
    subscriber.next('Data loaded!');
    subscriber.complete();
  }, 2000);
  
  // Cleanup : annuler le timeout si unsubscribe
  return () => {
    console.log('Cancelling timeout');
    clearTimeout(timeoutId);
  };
});

const sub = async$.subscribe(console.log);

// Annuler avant la complétion
setTimeout(() => {
  sub.unsubscribe();
  console.log('Unsubscribed');
}, 1000);

// Output:
// Starting async operation
// (après 1s) Cancelling timeout
// Unsubscribed
```

### 🔄 Observable avec Gestion d'Erreur

```typescript
const errorProne$ = new Observable<number>(subscriber => {
  subscriber.next(1);
  subscriber.next(2);
  
  // Simuler une erreur
  const random = Math.random();
  if (random < 0.5) {
    subscriber.error(new Error('Something went wrong!'));
    return;  // Important : arrêter après error
  }
  
  subscriber.next(3);
  subscriber.complete();
});

errorProne$.subscribe({
  next: value => console.log('Value:', value),
  error: err => console.error('Error:', err.message),
  complete: () => console.log('Complete!')
});
```

### 📡 Exemple Pratique : WebSocket

```typescript
function createWebSocket(url: string): Observable<any> {
  return new Observable(subscriber => {
    const socket = new WebSocket(url);
    
    socket.onopen = () => {
      console.log('WebSocket connected');
    };
    
    socket.onmessage = (event) => {
      subscriber.next(JSON.parse(event.data));
    };
    
    socket.onerror = (error) => {
      subscriber.error(error);
    };
    
    socket.onclose = () => {
      subscriber.complete();
    };
    
    // Cleanup : fermer la connexion si unsubscribe
    return () => {
      console.log('Closing WebSocket');
      socket.close();
    };
  });
}

// Utilisation
const ws$ = createWebSocket('wss://example.com/socket');

const subscription = ws$.subscribe({
  next: message => console.log('Message:', message),
  error: err => console.error('Error:', err),
  complete: () => console.log('Connection closed')
});

// Fermer la connexion après 10 secondes
setTimeout(() => subscription.unsubscribe(), 10000);
```

### 🌡️ Observables Froids vs Chauds

#### Observable Froid (Cold)

Un Observable **froid** crée un nouveau producteur pour **chaque souscripteur**.

```typescript
// Observable froid
const cold$ = new Observable(subscriber => {
  console.log('Producer created');
  subscriber.next(Math.random());
});

// Chaque souscripteur obtient un nouveau producteur
cold$.subscribe(value => console.log('A:', value));
// Producer created
// A: 0.234567

cold$.subscribe(value => console.log('B:', value));
// Producer created
// B: 0.876543

// Valeurs différentes car producteurs différents
```

**Caractéristiques des Observables froids** :
- `interval()`, `timer()`, `of()`, `from()`
- HttpClient dans Angular
- Chaque subscribe démarre une nouvelle exécution

#### Observable Chaud (Hot)

Un Observable **chaud** partage le **même producteur** entre tous les souscripteurs.

```typescript
import { Subject } from 'rxjs';

// Observable chaud avec Subject
const hot$ = new Subject<number>();

// Les deux souscripteurs partagent le même producteur
hot$.subscribe(value => console.log('A:', value));
hot$.subscribe(value => console.log('B:', value));

// Émettre une valeur
hot$.next(Math.random());
// A: 0.456789
// B: 0.456789

// Même valeur car même producteur
```

**Caractéristiques des Observables chauds** :
- Subjects (Subject, BehaviorSubject, ReplaySubject)
- `fromEvent()` (événements DOM)
- WebSockets
- Données partagées entre souscripteurs

#### Convertir Froid → Chaud avec share()

```typescript
import { interval } from 'rxjs';
import { share, take } from 'rxjs/operators';

// Froid par défaut
const cold$ = interval(1000).pipe(take(5));

// Convertir en chaud
const hot$ = cold$.pipe(share());

// Premier souscripteur
setTimeout(() => {
  hot$.subscribe(value => console.log('A:', value));
}, 0);

// Deuxième souscripteur (2 secondes après)
setTimeout(() => {
  hot$.subscribe(value => console.log('B:', value));
}, 2000);

// Output:
// A: 0
// A: 1
// A: 2  B: 2  (B commence ici)
// A: 3  B: 3
// A: 4  B: 4
```

---

## 2.3 Subjects

### 🎯 Subject - Multicast Basique

Un **Subject** est à la fois un Observable et un Observer.

```typescript
import { Subject } from 'rxjs';

const subject = new Subject<number>();

// Souscripteurs
subject.subscribe(value => console.log('Observer A:', value));
subject.subscribe(value => console.log('Observer B:', value));

// Émettre des valeurs
subject.next(1);
subject.next(2);

// Nouveau souscripteur (ne reçoit que les valeurs futures)
subject.subscribe(value => console.log('Observer C:', value));

subject.next(3);

// Output:
// Observer A: 1
// Observer B: 1
// Observer A: 2
// Observer B: 2
// Observer A: 3
// Observer B: 3
// Observer C: 3
```

**Cas d'usage : Event Bus**

```typescript
// Service Angular
@Injectable({ providedIn: 'root' })
export class EventBusService {
  private eventSubject = new Subject<any>();
  events$ = this.eventSubject.asObservable();
  
  emit(event: any) {
    this.eventSubject.next(event);
  }
}

// Component A
this.eventBus.emit({ type: 'USER_LOGGED_IN', userId: '123' });

// Component B
this.eventBus.events$.subscribe(event => {
  if (event.type === 'USER_LOGGED_IN') {
    console.log('User logged in:', event.userId);
  }
});
```

### 🔄 BehaviorSubject - Avec Valeur Initiale

**BehaviorSubject** garde la **dernière valeur** émise et l'envoie immédiatement aux nouveaux souscripteurs.

```typescript
import { BehaviorSubject } from 'rxjs';

// Valeur initiale obligatoire
const currentValue$ = new BehaviorSubject<number>(0);

// Observer A (reçoit la valeur initiale)
currentValue$.subscribe(value => console.log('A:', value));
// Output: A: 0

// Émettre de nouvelles valeurs
currentValue$.next(1);
currentValue$.next(2);
// Output: A: 1
// Output: A: 2

// Observer B (reçoit la dernière valeur émise : 2)
currentValue$.subscribe(value => console.log('B:', value));
// Output: B: 2

currentValue$.next(3);
// Output: A: 3
// Output: B: 3

// Obtenir la valeur actuelle (synchrone)
console.log('Current:', currentValue$.value);
// Output: Current: 3
```

**Cas d'usage : State Management**

```typescript
interface AppState {
  user: User | null;
  isLoading: boolean;
}

@Injectable({ providedIn: 'root' })
export class StateService {
  private stateSubject = new BehaviorSubject<AppState>({
    user: null,
    isLoading: false
  });
  
  // Observable public (read-only)
  state$ = this.stateSubject.asObservable();
  
  // Getters pour des parties spécifiques
  get user$() {
    return this.state$.pipe(
      map(state => state.user),
      distinctUntilChanged()
    );
  }
  
  get isLoading$() {
    return this.state$.pipe(
      map(state => state.isLoading),
      distinctUntilChanged()
    );
  }
  
  // Setters
  setUser(user: User) {
    this.stateSubject.next({
      ...this.stateSubject.value,
      user
    });
  }
  
  setLoading(isLoading: boolean) {
    this.stateSubject.next({
      ...this.stateSubject.value,
      isLoading
    });
  }
}

// Component
constructor(private state: StateService) {}

ngOnInit() {
  this.state.user$.subscribe(user => {
    console.log('User changed:', user);
  });
}
```

### 📼 ReplaySubject - Replay d'Événements

**ReplaySubject** garde **N dernières valeurs** et les rejoue pour les nouveaux souscripteurs.

```typescript
import { ReplaySubject } from 'rxjs';

// Garder les 3 dernières valeurs
const replay$ = new ReplaySubject<number>(3);

replay$.next(1);
replay$.next(2);
replay$.next(3);
replay$.next(4);

// Nouveau souscripteur reçoit les 3 dernières valeurs
replay$.subscribe(value => console.log('A:', value));
// Output:
// A: 2
// A: 3
// A: 4

replay$.next(5);
// Output: A: 5

// Avec limite de temps (buffer window)
const replayTime$ = new ReplaySubject<number>(100, 500); // 100 valeurs max, 500ms window

replayTime$.next(1);

setTimeout(() => replayTime$.next(2), 200);
setTimeout(() => replayTime$.next(3), 400);
setTimeout(() => replayTime$.next(4), 600);

setTimeout(() => {
  replayTime$.subscribe(console.log);
  // Reçoit seulement 2, 3, 4 (dans la fenêtre de 500ms)
}, 700);
```

**Cas d'usage : Cache de Requêtes HTTP**

```typescript
@Injectable({ providedIn: 'root' })
export class UserService {
  private usersSubject = new ReplaySubject<User[]>(1);
  users$ = this.usersSubject.asObservable();
  
  constructor(private http: HttpClient) {
    this.loadUsers();
  }
  
  private loadUsers() {
    this.http.get<User[]>('/api/users').subscribe(users => {
      this.usersSubject.next(users);
    });
  }
  
  refreshUsers() {
    this.loadUsers();
  }
}

// Component
// Chaque souscripteur reçoit immédiatement la dernière liste
this.userService.users$.subscribe(users => {
  console.log(users);
});
```

### 🎬 AsyncSubject - Dernière Valeur à la Complétion

**AsyncSubject** émet seulement la **dernière valeur** quand le Subject est **complété**.

```typescript
import { AsyncSubject } from 'rxjs';

const async$ = new AsyncSubject<number>();

async$.subscribe(value => console.log('A:', value));

async$.next(1);
async$.next(2);
async$.next(3);

// Nouveau souscripteur
async$.subscribe(value => console.log('B:', value));

async$.next(4);
async$.complete();  // Seulement maintenant les valeurs sont émises

// Output:
// A: 4
// B: 4
```

**Cas d'usage : Opération Longue**

```typescript
function processLargeFile(file: File): AsyncSubject<string> {
  const result$ = new AsyncSubject<string>();
  
  const reader = new FileReader();
  
  reader.onload = () => {
    // Traitement long
    const processed = processData(reader.result);
    result$.next(processed);
    result$.complete();
  };
  
  reader.readAsText(file);
  
  return result$;
}

// Utilisation
const result$ = processLargeFile(myFile);

// Premier souscripteur
result$.subscribe(data => console.log('First:', data));

// Deuxième souscripteur (même résultat, pas de re-processing)
result$.subscribe(data => console.log('Second:', data));
```

### 📊 Comparaison des Subjects

| Type | Valeur Initiale | Replay | À la Souscription |
|------|----------------|--------|-------------------|
| **Subject** | ❌ Non | ❌ Aucune | Rien |
| **BehaviorSubject** | ✅ Oui (obligatoire) | ✅ Dernière valeur | Dernière valeur |
| **ReplaySubject** | ❌ Non | ✅ N dernières | N dernières valeurs |
| **AsyncSubject** | ❌ Non | ✅ Dernière (si complete) | Rien jusqu'à complete |

---

## 2.4 Exercice Pratique

### 🎯 Challenge : Service de Notifications

Créez un service de notifications avec :
1. Un Subject pour émettre des notifications
2. Affichage des notifications dans un composant
3. Auto-dismiss après 3 secondes
4. Limite de 5 notifications à l'écran

### 💡 Solution

```typescript
// notification.model.ts
export interface Notification {
  id: string;
  message: string;
  type: 'success' | 'error' | 'info' | 'warning';
  timestamp: Date;
}

// notification.service.ts
import { Injectable } from '@angular/core';
import { Subject, Observable } from 'rxjs';
import { scan } from 'rxjs/operators';

@Injectable({ providedIn: 'root' })
export class NotificationService {
  private notificationSubject = new Subject<Notification>();
  
  notifications$: Observable<Notification[]> = this.notificationSubject.pipe(
    scan((acc: Notification[], notification: Notification) => {
      // Ajouter la nouvelle notification
      const updated = [notification, ...acc];
      
      // Garder seulement les 5 dernières
      return updated.slice(0, 5);
    }, [])
  );
  
  show(message: string, type: Notification['type'] = 'info') {
    const notification: Notification = {
      id: this.generateId(),
      message,
      type,
      timestamp: new Date()
    };
    
    this.notificationSubject.next(notification);
    
    // Auto-dismiss après 3 secondes
    setTimeout(() => {
      this.dismiss(notification.id);
    }, 3000);
  }
  
  success(message: string) {
    this.show(message, 'success');
  }
  
  error(message: string) {
    this.show(message, 'error');
  }
  
  info(message: string) {
    this.show(message, 'info');
  }
  
  warning(message: string) {
    this.show(message, 'warning');
  }
  
  private dismiss(id: string) {
    // Implémenter la logique de suppression
  }
  
  private generateId(): string {
    return `notif-${Date.now()}-${Math.random()}`;
  }
}

// notification.component.ts
import { Component } from '@angular/core';
import { NotificationService } from './notification.service';

@Component({
  selector: 'app-notifications',
  template: `
    <div class="notifications-container">
      <div *ngFor="let notif of notifications$ | async"
           class="notification"
           [class.success]="notif.type === 'success'"
           [class.error]="notif.type === 'error'"
           [class.info]="notif.type === 'info'"
           [class.warning]="notif.type === 'warning'">
        {{ notif.message }}
      </div>
    </div>
  `,
  styles: [`
    .notifications-container {
      position: fixed;
      top: 20px;
      right: 20px;
      z-index: 1000;
    }
    
    .notification {
      padding: 15px 20px;
      margin-bottom: 10px;
      border-radius: 4px;
      box-shadow: 0 2px 8px rgba(0,0,0,0.15);
      animation: slideIn 0.3s ease-out;
    }
    
    .success { background: #4caf50; color: white; }
    .error { background: #f44336; color: white; }
    .info { background: #2196f3; color: white; }
    .warning { background: #ff9800; color: white; }
    
    @keyframes slideIn {
      from {
        transform: translateX(100%);
        opacity: 0;
      }
      to {
        transform: translateX(0);
        opacity: 1;
      }
    }
  `]
})
export class NotificationsComponent {
  notifications$ = this.notificationService.notifications$;
  
  constructor(private notificationService: NotificationService) {}
}

// app.component.ts (utilisation)
export class AppComponent {
  constructor(private notifications: NotificationService) {}
  
  onSave() {
    this.notifications.success('Données sauvegardées !');
  }
  
  onError() {
    this.notifications.error('Une erreur est survenue');
  }
  
  onInfo() {
    this.notifications.info('Information importante');
  }
}
```

---

## 📝 Questions/Réponses d'Entretien

### Q1 : Quelle est la différence entre `of()` et `from()` ?

**Réponse :**
> **`of()`** :
> - Émet chaque argument comme une valeur séparée
> - `of(1, 2, 3)` émet 1, puis 2, puis 3
> - `of([1, 2, 3])` émet le tableau entier
> 
> **`from()`** :
> - Convertit un itérable en Observable
> - `from([1, 2, 3])` émet 1, puis 2, puis 3
> - Peut convertir Promises, arrays, strings, etc.
> 
> ```typescript
> of([1, 2, 3]).subscribe(console.log);     // [1, 2, 3]
> from([1, 2, 3]).subscribe(console.log);   // 1, 2, 3
> ```

### Q2 : Quand utiliser `interval()` vs `timer()` ?

**Réponse :**
> **`interval(n)`** :
> - Commence immédiatement (émission à 0ms)
> - Émet ensuite toutes les n millisecondes
> - Cas d'usage : polling régulier
> 
> **`timer(delay, interval)`** :
> - Attend un délai avant la première émission
> - Puis émet à intervalle régulier (optionnel)
> - Cas d'usage : action différée, countdown
> 
> ```typescript
> // interval : commence immédiatement
> interval(1000).subscribe(console.log);
> // Output: 0 (immédiat), 1 (après 1s), 2 (après 2s)...
> 
> // timer : délai avant de commencer
> timer(3000, 1000).subscribe(console.log);
> // Output: 0 (après 3s), 1 (après 4s), 2 (après 5s)...
> 
> // timer : une seule émission
> timer(2000).subscribe(() => console.log('Exécuté après 2s'));
> ```

### Q3 : Comment créer un Observable à partir d'une Promise ?

**Réponse :**
> Utilisez `from()` pour convertir une Promise en Observable :
> 
> ```typescript
> import { from } from 'rxjs';
> 
> // Promise existante
> const promise = fetch('/api/users').then(r => r.json());
> 
> // Convertir en Observable
> const users$ = from(promise);
> 
> users$.subscribe({
>   next: users => console.log('Users:', users),
>   error: err => console.error('Error:', err),
>   complete: () => console.log('Complete')
> });
> 
> // Ou directement
> from(fetch('/api/users').then(r => r.json())).subscribe(console.log);
> ```
> 
> **Avantages de l'Observable** :
> - Peut être annulé avec `unsubscribe()`
> - Peut utiliser les opérateurs RxJS
> - Peut être combiné avec d'autres Observables

### Q4 : Qu'est-ce qu'un Observable froid et comment le rendre chaud ?

**Réponse :**
> **Observable froid** :
> - Crée un nouveau producteur pour chaque souscripteur
> - Exemple : `interval()`, `HttpClient`
> 
> **Observable chaud** :
> - Partage le même producteur entre souscripteurs
> - Exemple : `Subject`, `fromEvent()`
> 
> **Convertir froid → chaud** :
> 
> ```typescript
> import { interval, Subject } from 'rxjs';
> import { share, shareReplay } from 'rxjs/operators';
> 
> // Méthode 1 : share()
> const cold$ = interval(1000);
> const hot$ = cold$.pipe(share());
> 
> // Méthode 2 : shareReplay()
> const hotWithCache$ = cold$.pipe(
>   shareReplay(1)  // Cache la dernière valeur
> );
> 
> // Méthode 3 : Subject manuel
> const subject = new Subject();
> cold$.subscribe(subject);
> const hot2$ = subject.asObservable();
> ```

### Q5 : Quelle est la différence entre Subject, BehaviorSubject, ReplaySubject et AsyncSubject ?

**Réponse :**
> | Type | Valeur Initiale | Replay | Émission aux nouveaux souscripteurs |
> |------|----------------|--------|-------------------------------------|
> | **Subject** | Non | Non | Rien |
> | **BehaviorSubject** | Oui (obligatoire) | Dernière valeur | Dernière valeur |
> | **ReplaySubject** | Non | N dernières | N dernières valeurs |
> | **AsyncSubject** | Non | Dernière (si complete) | Rien jusqu'à complete |
> 
> ```typescript
> // Subject : rien aux nouveaux souscripteurs
> const subject = new Subject();
> subject.next(1);
> subject.subscribe(console.log);  // Ne reçoit rien
> subject.next(2);  // Reçoit 2
> 
> // BehaviorSubject : dernière valeur
> const behavior = new BehaviorSubject(0);
> behavior.next(1);
> behavior.subscribe(console.log);  // Reçoit 1 immédiatement
> behavior.next(2);  // Reçoit 2
> 
> // ReplaySubject : N dernières valeurs
> const replay = new ReplaySubject(2);
> replay.next(1);
> replay.next(2);
> replay.next(3);
> replay.subscribe(console.log);  // Reçoit 2, 3
> 
> // AsyncSubject : dernière valeur à la complétion
> const async = new AsyncSubject();
> async.next(1);
> async.next(2);
> async.subscribe(console.log);  // Ne reçoit rien encore
> async.next(3);
> async.complete();  // Maintenant reçoit 3
> ```

### Q6 : Comment créer un Observable qui émet des événements DOM ?

**Réponse :**
> Utilisez `fromEvent()` :
> 
> ```typescript
> import { fromEvent } from 'rxjs';
> import { map, debounceTime } from 'rxjs/operators';
> 
> // Clics de souris
> const clicks$ = fromEvent(document, 'click');
> clicks$.subscribe(event => console.log('Clicked at:', event.clientX, event.clientY));
> 
> // Saisie clavier
> const input = document.getElementById('searchInput');
> const keyup$ = fromEvent<KeyboardEvent>(input, 'keyup');
> 
> keyup$.pipe(
>   map(event => (event.target as HTMLInputElement).value),
>   debounceTime(300)
> ).subscribe(value => console.log('Search:', value));
> 
> // Scroll
> const scroll$ = fromEvent(window, 'scroll');
> scroll$.subscribe(() => console.log('Scrolled to:', window.scrollY));
> 
> // Resize
> const resize$ = fromEvent(window, 'resize');
> resize$.subscribe(() => console.log('Window size:', window.innerWidth));
> ```

### Q7 : Quelle est la différence entre `new Observable()` et `create()` ?

**Réponse :**
> **`create()`** est **déprécié depuis RxJS 6**. Utilisez toujours `new Observable()` :
> 
> ```typescript
> // ❌ Ancienne méthode (dépréciée)
> import { Observable } from 'rxjs';
> const obs$ = Observable.create(subscriber => {
>   subscriber.next(1);
>   subscriber.complete();
> });
> 
> // ✅ Méthode recommandée
> const obs$ = new Observable(subscriber => {
>   subscriber.next(1);
>   subscriber.complete();
> });
> ```
> 
> **Raison de la dépréciation** :
> - `new Observable()` est plus standard JavaScript
> - Meilleur support TypeScript
> - Plus cohérent avec les autres APIs

### Q8 : Comment implémenter un Observable avec cleanup (nettoyage) ?

**Réponse :**
> Retournez une fonction de teardown dans le constructor :
> 
> ```typescript
> const observable$ = new Observable<number>(subscriber => {
>   console.log('Observable started');
>   
>   // Configuration
>   const intervalId = setInterval(() => {
>     subscriber.next(Math.random());
>   }, 1000);
>   
>   // ✅ Fonction de cleanup (teardown)
>   return () => {
>     console.log('Observable cleaned up');
>     clearInterval(intervalId);
>   };
> });
> 
> const subscription = observable$.subscribe(console.log);
> 
> // Après 5 secondes, nettoyer
> setTimeout(() => {
>   subscription.unsubscribe();  // Appelle la fonction de cleanup
> }, 5000);
> ```
> 
> **Cas d'usage** :
> - Nettoyer les timers (`clearInterval`, `clearTimeout`)
> - Fermer les WebSockets
> - Annuler les requêtes HTTP
> - Supprimer les event listeners

### Q9 : Comment créer un Observable depuis un WebSocket ?

**Réponse :**
> Créez un Observable personnalisé avec gestion du cleanup :
> 
> ```typescript
> function createWebSocketObservable(url: string): Observable<any> {
>   return new Observable(subscriber => {
>     const socket = new WebSocket(url);
>     
>     socket.onopen = () => {
>       console.log('WebSocket connected');
>     };
>     
>     socket.onmessage = (event) => {
>       subscriber.next(JSON.parse(event.data));
>     };
>     
>     socket.onerror = (error) => {
>       subscriber.error(error);
>     };
>     
>     socket.onclose = () => {
>       subscriber.complete();
>     };
>     
>     // ✅ Cleanup : fermer le WebSocket
>     return () => {
>       console.log('Closing WebSocket');
>       if (socket.readyState === WebSocket.OPEN) {
>         socket.close();
>       }
>     };
>   });
> }
> 
> // Utilisation
> const ws$ = createWebSocketObservable('wss://example.com/socket');
> 
> const subscription = ws$.subscribe({
>   next: message => console.log('Message:', message),
>   error: err => console.error('Error:', err),
>   complete: () => console.log('Connection closed')
> });
> 
> // Fermer après 30 secondes
> setTimeout(() => subscription.unsubscribe(), 30000);
> ```

### Q10 : Quand utiliser BehaviorSubject plutôt que Subject ?

**Réponse :**
> Utilisez **BehaviorSubject** quand vous avez besoin :
> 
> **1. D'une valeur initiale** :
> ```typescript
> // ✅ BehaviorSubject : valeur initiale obligatoire
> const currentUser$ = new BehaviorSubject<User | null>(null);
> ```
> 
> **2. Que les nouveaux souscripteurs reçoivent la dernière valeur** :
> ```typescript
> const isLoading$ = new BehaviorSubject<boolean>(false);
> 
> isLoading$.next(true);
> 
> // Nouveau composant qui s'abonne plus tard
> isLoading$.subscribe(loading => {
>   console.log('Loading:', loading);  // Reçoit true immédiatement
> });
> ```
> 
> **3. D'accéder à la valeur actuelle de façon synchrone** :
> ```typescript
> const count$ = new BehaviorSubject<number>(0);
> count$.next(5);
> 
> console.log(count$.value);  // 5 (accès synchrone)
> ```
> 
> **Cas d'usage typiques** :
> - State management (état de l'application)
> - Valeurs de configuration
> - État de connexion
> - Données de cache
> 
> Utilisez **Subject** quand :
> - Pas besoin de valeur initiale
> - Les nouveaux souscripteurs attendent les valeurs futures
> - Event bus

### Q11 : Comment implémenter un cache avec ReplaySubject ?

**Réponse :**
> ```typescript
> @Injectable({ providedIn: 'root' })
> export class CachedDataService {
>   private dataSubject = new ReplaySubject<Data[]>(1);
>   public data$ = this.dataSubject.asObservable();
>   private loaded = false;
>   
>   constructor(private http: HttpClient) {}
>   
>   // Charger les données (seulement si pas déjà chargées)
>   loadData(): Observable<Data[]> {
>     if (!this.loaded) {
>       this.http.get<Data[]>('/api/data').subscribe(data => {
>         this.dataSubject.next(data);
>         this.loaded = true;
>       });
>     }
>     
>     return this.data$;
>   }
>   
>   // Rafraîchir les données
>   refreshData(): Observable<Data[]> {
>     return this.http.get<Data[]>('/api/data').pipe(
>       tap(data => this.dataSubject.next(data))
>     );
>   }
> }
> 
> // Utilisation dans un composant
> @Component({...})
> export class MyComponent implements OnInit {
>   data$ = this.dataService.data$;
>   
>   constructor(private dataService: CachedDataService) {}
>   
>   ngOnInit() {
>     // Premier composant : charge les données
>     this.dataService.loadData().subscribe();
>   }
> }
> 
> // Dans un autre composant
> @Component({...})
> export class OtherComponent implements OnInit {
>   data$ = this.dataService.data$;
>   
>   constructor(private dataService: CachedDataService) {}
>   
>   ngOnInit() {
>     // Reçoit immédiatement les données cachées (pas de nouvelle requête)
>     this.dataService.loadData().subscribe();
>   }
> }
> ```

### Q12 : Quelle est la différence entre `ajax()` de RxJS et `HttpClient` d'Angular ?

**Réponse :**
> | Aspect | RxJS ajax() | Angular HttpClient |
> |--------|-------------|-------------------|
> | **Framework** | Indépendant | Angular uniquement |
> | **Intercepteurs** | Non | Oui ✅ |
> | **Testing** | Plus complexe | TestingModule ✅ |
> | **Type safety** | Manuel | Automatique ✅ |
> | **XSRF** | Manuel | Automatique ✅ |
> 
> ```typescript
> // RxJS ajax
> import { ajax } from 'rxjs/ajax';
> 
> ajax.getJSON<User[]>('/api/users').subscribe(users => {
>   console.log(users);
> });
> 
> // Angular HttpClient (recommandé dans Angular)
> constructor(private http: HttpClient) {}
> 
> this.http.get<User[]>('/api/users').subscribe(users => {
>   console.log(users);
> });
> ```
> 
> **Recommandation** : Dans Angular, **toujours utiliser HttpClient**.

### Q13 : Comment créer un Observable qui émet à des intervalles croissants ?

**Réponse :**
> Utilisez `defer()` avec un Observable personnalisé :
> 
> ```typescript
> import { Observable } from 'rxjs';
> 
> function exponentialBackoff(maxRetries: number): Observable<number> {
>   return new Observable<number>(subscriber => {
>     let attempt = 0;
>     
>     function scheduleNext() {
>       if (attempt >= maxRetries) {
>         subscriber.complete();
>         return;
>       }
>       
>       const delay = Math.pow(2, attempt) * 1000;  // 1s, 2s, 4s, 8s...
>       console.log(`Attempt ${attempt + 1} in ${delay}ms`);
>       
>       const timeoutId = setTimeout(() => {
>         subscriber.next(attempt);
>         attempt++;
>         scheduleNext();
>       }, delay);
>       
>       // Cleanup
>       return () => clearTimeout(timeoutId);
>     }
>     
>     scheduleNext();
>   });
> }
> 
> // Utilisation
> exponentialBackoff(5).subscribe({
>   next: attempt => console.log('Attempt:', attempt),
>   complete: () => console.log('Max retries reached')
> });
> 
> // Output:
> // Attempt 0 in 1000ms
> // Attempt 1 in 2000ms
> // Attempt 2 in 4000ms
> // Attempt 3 in 8000ms
> // Attempt 4 in 16000ms
> // Max retries reached
> ```

### Q14 : Comment partager un Observable entre plusieurs composants ?

**Réponse :**
> Utilisez un **Service avec BehaviorSubject** :
> 
> ```typescript
> // shared-data.service.ts
> @Injectable({ providedIn: 'root' })
> export class SharedDataService {
>   private dataSubject = new BehaviorSubject<Data | null>(null);
>   
>   // Observable public (read-only)
>   public data$ = this.dataSubject.asObservable();
>   
>   // Setter
>   setData(data: Data) {
>     this.dataSubject.next(data);
>   }
>   
>   // Getter (valeur actuelle)
>   getData(): Data | null {
>     return this.dataSubject.value;
>   }
> }
> 
> // component-a.component.ts
> export class ComponentA {
>   constructor(private sharedData: SharedDataService) {}
>   
>   updateData() {
>     this.sharedData.setData({ id: 1, name: 'New Data' });
>   }
> }
> 
> // component-b.component.ts
> export class ComponentB implements OnInit {
>   data$ = this.sharedData.data$;
>   
>   constructor(private sharedData: SharedDataService) {}
>   
>   ngOnInit() {
>     this.data$.subscribe(data => {
>       console.log('Data updated:', data);
>     });
>   }
> }
> ```

### Q15 : Quelle est la différence entre `of()` et `from()` avec une Promise ?

**Réponse :**
> ```typescript
> const promise = Promise.resolve('Hello');
> 
> // of() : émet la Promise elle-même (pas sa valeur)
> of(promise).subscribe(console.log);
> // Output: Promise { 'Hello' }
> 
> // from() : convertit la Promise et émet sa valeur
> from(promise).subscribe(console.log);
> // Output: Hello
> ```
> 
> **Règle** : Toujours utiliser `from()` pour les Promises.

---

## 🎯 Points Clés à Retenir

### Opérateurs de Création
✅ **of()** : Émettre des valeurs directement
✅ **from()** : Convertir arrays, Promises, iterables
✅ **interval()** : Émissions à intervalle régulier
✅ **timer()** : Délai initial puis intervalle
✅ **fromEvent()** : Événements DOM

### Subjects
✅ **Subject** : Multicast basique
✅ **BehaviorSubject** : Avec valeur initiale et dernière valeur
✅ **ReplaySubject** : Replay de N valeurs
✅ **AsyncSubject** : Dernière valeur à la complétion

### Observable Froid vs Chaud
✅ **Froid** : Nouveau producteur par souscripteur
✅ **Chaud** : Producteur partagé
✅ **share()** : Convertir froid → chaud

### Best Practices
✅ Toujours retourner une fonction de **cleanup** dans `new Observable()`
✅ Utiliser **BehaviorSubject** pour le state management
✅ Utiliser **from()** pour convertir des Promises
✅ Utiliser **HttpClient** plutôt que `ajax()` dans Angular
✅ Partager des données entre composants via **Services**

---

## 🚀 Prochaines Étapes

Maintenant que vous maîtrisez la création d'Observables, le **Chapitre 3** vous apprendra à les **transformer** avec les opérateurs les plus puissants de RxJS !

**Prêt à continuer ?** 💪