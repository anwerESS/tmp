## [**..**](./00_index.md)

# Chapitre 4 : Opérateurs de Filtrage

## 📑 Table des matières
- [4.1 Filtrage de Valeurs](#41-filtrage-de-valeurs)
- [4.2 Gestion du Temps](#42-gestion-du-temps)
- [4.3 Déduplication](#43-déduplication)
- [4.4 Exercice Pratique](#44-exercice-pratique)
- [Questions/Réponses d'Entretien](#-questionsréponses-dentretien)

---

## 4.1 Filtrage de Valeurs

### 🎯 filter() - Condition de Filtrage

L'opérateur **`filter()`** ne laisse passer que les valeurs qui satisfont une condition.

```typescript
import { of } from 'rxjs';
import { filter } from 'rxjs/operators';

// Filtrage simple
const numbers$ = of(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

numbers$.pipe(
  filter(x => x % 2 === 0)  // Garder seulement les nombres pairs
).subscribe(console.log);
// Output: 2, 4, 6, 8, 10

// Filtrage d'objets
interface User {
  id: number;
  name: string;
  age: number;
  active: boolean;
}

const users$ = of(
  { id: 1, name: 'Alice', age: 25, active: true },
  { id: 2, name: 'Bob', age: 17, active: false },
  { id: 3, name: 'Charlie', age: 30, active: true },
  { id: 4, name: 'David', age: 16, active: true }
);

users$.pipe(
  filter(user => user.age >= 18),
  filter(user => user.active)
).subscribe(console.log);
// Output: Alice, Charlie
```

**Diagramme de marbre :**
```
source:        --1--2--3--4--5--6--|
  filter(>3):  --------4--5--6--|
```

#### Cas d'usage Angular : Filtrer les Événements

```typescript
@Component({
  selector: 'app-form',
  template: `
    <input #searchInput type="text" placeholder="Rechercher...">
    <p>Caractères minimum: 3</p>
  `
})
export class FormComponent implements OnInit {
  @ViewChild('searchInput') searchInput: ElementRef;
  
  ngOnInit() {
    fromEvent(this.searchInput.nativeElement, 'keyup').pipe(
      map((event: any) => event.target.value),
      filter(query => query.length >= 3),  // ✅ Minimum 3 caractères
      debounceTime(300)
    ).subscribe(query => {
      this.search(query);
    });
  }
  
  search(query: string) {
    console.log('Searching for:', query);
  }
}
```

### 📊 take() et takeLast() - Nombre d'Émissions

#### take() - Prendre les N Premières Valeurs

```typescript
import { interval } from 'rxjs';
import { take } from 'rxjs/operators';

// Prendre les 5 premières valeurs
interval(1000).pipe(
  take(5)
).subscribe({
  next: value => console.log('Value:', value),
  complete: () => console.log('Complete!')
});

// Output:
// Value: 0
// Value: 1
// Value: 2
// Value: 3
// Value: 4
// Complete!
```

**Diagramme de marbre :**
```
interval: --0--1--2--3--4--5--6-->
take(3):  --0--1--2|
```

#### takeLast() - Prendre les N Dernières Valeurs

```typescript
import { of } from 'rxjs';
import { takeLast } from 'rxjs/operators';

of(1, 2, 3, 4, 5, 6, 7, 8, 9, 10).pipe(
  takeLast(3)
).subscribe(console.log);

// Output (à la complétion):
// 8
// 9
// 10
```

**Note** : `takeLast()` attend la complétion de l'Observable source avant d'émettre.

#### Cas d'usage : Requête HTTP Unique

```typescript
@Injectable({ providedIn: 'root' })
export class AuthService {
  constructor(private http: HttpClient) {}
  
  login(credentials: any): Observable<User> {
    return this.http.post<User>('/api/login', credentials).pipe(
      take(1)  // ✅ Se désabonne automatiquement après 1 émission
    );
  }
}

// Utilisation - pas besoin de unsubscribe !
this.authService.login(credentials).subscribe(user => {
  console.log('Logged in:', user);
});
```

### ⏹️ takeUntil() et takeWhile() - Conditions d'Arrêt

#### takeUntil() - Arrêter Quand un Autre Observable Émet

```typescript
import { interval, Subject } from 'rxjs';
import { takeUntil } from 'rxjs/operators';

const stop$ = new Subject<void>();

interval(1000).pipe(
  takeUntil(stop$)
).subscribe({
  next: value => console.log('Value:', value),
  complete: () => console.log('Stopped!')
});

// Arrêter après 5 secondes
setTimeout(() => {
  stop$.next();
  stop$.complete();
}, 5000);

// Output:
// Value: 0
// Value: 1
// Value: 2
// Value: 3
// Value: 4
// Stopped!
```

**Diagramme de marbre :**
```
source:       --0--1--2--3--4--5--6-->
stop$:        ----------S|
takeUntil:    --0--1--2-|
```

#### Pattern de Unsubscribe avec takeUntil

```typescript
@Component({
  selector: 'app-user-list'
})
export class UserListComponent implements OnInit, OnDestroy {
  private destroy$ = new Subject<void>();
  
  constructor(
    private userService: UserService,
    private notificationService: NotificationService
  ) {}
  
  ngOnInit() {
    // ✅ Toutes les souscriptions s'arrêtent automatiquement
    this.userService.getUsers().pipe(
      takeUntil(this.destroy$)
    ).subscribe(users => {
      console.log('Users:', users);
    });
    
    this.notificationService.notifications$.pipe(
      takeUntil(this.destroy$)
    ).subscribe(notification => {
      console.log('Notification:', notification);
    });
    
    interval(5000).pipe(
      takeUntil(this.destroy$)
    ).subscribe(() => {
      console.log('Refresh');
    });
  }
  
  ngOnDestroy() {
    this.destroy$.next();
    this.destroy$.complete();
  }
}
```

#### takeWhile() - Continuer Tant Que la Condition est Vraie

```typescript
import { interval } from 'rxjs';
import { takeWhile } from 'rxjs/operators';

// Continuer tant que la valeur est < 5
interval(1000).pipe(
  takeWhile(value => value < 5)
).subscribe({
  next: value => console.log('Value:', value),
  complete: () => console.log('Condition not met!')
});

// Output:
// Value: 0
// Value: 1
// Value: 2
// Value: 3
// Value: 4
// Condition not met!
```

**Note** : Par défaut, `takeWhile()` ne **n'émet pas** la valeur qui ne satisfait pas la condition.

```typescript
// Inclure la valeur qui ne satisfait pas la condition
interval(1000).pipe(
  takeWhile(value => value < 5, true)  // ✅ inclusive: true
).subscribe(console.log);

// Output:
// 0, 1, 2, 3, 4, 5
```

#### Cas d'usage : Countdown Timer

```typescript
@Component({
  selector: 'app-countdown',
  template: `
    <div class="countdown">
      <h2>{{ timeLeft }}</h2>
      <button (click)="start()" [disabled]="isRunning">Démarrer</button>
    </div>
  `
})
export class CountdownComponent {
  timeLeft: number = 60;
  isRunning: boolean = false;
  
  start() {
    this.isRunning = true;
    this.timeLeft = 60;
    
    interval(1000).pipe(
      map(() => --this.timeLeft),
      takeWhile(time => time >= 0, true)  // Inclure 0
    ).subscribe({
      next: time => console.log('Time left:', time),
      complete: () => {
        this.isRunning = false;
        alert('Temps écoulé !');
      }
    });
  }
}
```

### ⏭️ skip() et skipUntil() - Sauter des Valeurs

#### skip() - Sauter les N Premières Valeurs

```typescript
import { interval } from 'rxjs';
import { skip, take } from 'rxjs/operators';

interval(1000).pipe(
  skip(3),    // Sauter les 3 premières
  take(5)     // Prendre ensuite 5 valeurs
).subscribe(console.log);

// Output:
// 3, 4, 5, 6, 7
```

**Diagramme de marbre :**
```
source:   --0--1--2--3--4--5--6--7-->
skip(3):  -----------3--4--5--6--7-->
```

#### skipUntil() - Sauter Jusqu'à ce qu'un Observable Émette

```typescript
import { interval, timer } from 'rxjs';
import { skipUntil } from 'rxjs/operators';

const start$ = timer(3000);  // Émet après 3 secondes

interval(1000).pipe(
  skipUntil(start$)
).subscribe(console.log);

// Output (commence après 3 secondes):
// 3, 4, 5, 6, 7...
```

**Diagramme de marbre :**
```
source:      --0--1--2--3--4--5--6-->
start$:      --------S|
skipUntil:   -----------3--4--5--6-->
```

#### Cas d'usage : Attendre l'Authentification

```typescript
@Injectable({ providedIn: 'root' })
export class DataService {
  private authComplete$ = new Subject<void>();
  
  constructor(
    private http: HttpClient,
    private authService: AuthService
  ) {
    // Signaler quand l'auth est complète
    this.authService.isAuthenticated$.pipe(
      filter(isAuth => isAuth === true),
      take(1)
    ).subscribe(() => {
      this.authComplete$.next();
    });
  }
  
  getData(): Observable<any[]> {
    // Ne charger les données qu'après l'authentification
    return interval(5000).pipe(
      skipUntil(this.authComplete$),
      switchMap(() => this.http.get<any[]>('/api/data'))
    );
  }
}
```

### 🔄 skipWhile() - Sauter Tant Que la Condition est Vraie

```typescript
import { interval } from 'rxjs';
import { skipWhile, take } from 'rxjs/operators';

interval(1000).pipe(
  skipWhile(value => value < 5),  // Sauter tant que < 5
  take(5)
).subscribe(console.log);

// Output:
// 5, 6, 7, 8, 9
```

---

## 4.2 Gestion du Temps

### ⏱️ debounceTime() - Attendre un Délai

**`debounceTime()`** attend que N millisecondes se soient écoulées sans nouvelle émission.

```typescript
import { fromEvent } from 'rxjs';
import { debounceTime, map } from 'rxjs/operators';

const input = document.getElementById('searchInput');

fromEvent(input, 'keyup').pipe(
  map((event: any) => event.target.value),
  debounceTime(300)  // ✅ Attendre 300ms après la dernière frappe
).subscribe(value => {
  console.log('Search:', value);
});
```

**Diagramme de marbre :**
```
source:          --a-b-c----d---e-f-g-----h--|
debounceTime(3): -------c-------g--------h--|
                 └─3─┘   └─3─┘  └─3─┘
```

#### Cas d'usage : Recherche en Temps Réel

```typescript
@Component({
  selector: 'app-search',
  template: `
    <input #searchInput 
           type="text" 
           placeholder="Rechercher..."
           [formControl]="searchControl">
    <div *ngIf="loading">Recherche en cours...</div>
    <div *ngFor="let result of results">{{ result.name }}</div>
  `
})
export class SearchComponent implements OnInit {
  searchControl = new FormControl('');
  results: any[] = [];
  loading = false;
  
  constructor(private http: HttpClient) {}
  
  ngOnInit() {
    this.searchControl.valueChanges.pipe(
      debounceTime(300),  // ✅ Attendre 300ms
      distinctUntilChanged(),
      tap(() => this.loading = true),
      switchMap(query => {
        if (!query) return of([]);
        return this.http.get<any[]>(`/api/search?q=${query}`);
      }),
      tap(() => this.loading = false)
    ).subscribe(results => {
      this.results = results;
    });
  }
}
```

### 🚦 throttleTime() - Limiter la Fréquence

**`throttleTime()`** émet la première valeur, puis ignore les suivantes pendant N millisecondes.

```typescript
import { fromEvent } from 'rxjs';
import { throttleTime } from 'rxjs/operators';

const button = document.getElementById('clickBtn');

fromEvent(button, 'click').pipe(
  throttleTime(2000)  // ✅ Maximum 1 clic toutes les 2 secondes
).subscribe(() => {
  console.log('Button clicked!');
});
```

**Diagramme de marbre :**
```
source:          --a-b-c-d---e-f-g-h---i--|
throttleTime(3): --a-----d-------g-----i--|
                 └─3─┘   └─3─┘   └─3─┘
```

#### Différence debounceTime() vs throttleTime()

```typescript
// debounceTime : Attend le silence
// Utiliser pour : recherche, validation de formulaire
fromEvent(input, 'keyup').pipe(
  debounceTime(300)  // Émet 300ms après la DERNIÈRE frappe
).subscribe(console.log);

// throttleTime : Limite la fréquence
// Utiliser pour : scroll, resize, boutons
fromEvent(window, 'scroll').pipe(
  throttleTime(200)  // Émet au maximum toutes les 200ms
).subscribe(console.log);
```

#### Cas d'usage : Scroll Infini

```typescript
@Component({
  selector: 'app-infinite-scroll',
  template: `
    <div class="content" (scroll)="onScroll($event)">
      <div *ngFor="let item of items">{{ item }}</div>
      <div *ngIf="loading">Chargement...</div>
    </div>
  `
})
export class InfiniteScrollComponent implements OnInit {
  items: any[] = [];
  loading = false;
  private scrollSubject = new Subject<Event>();
  
  constructor(private http: HttpClient) {}
  
  ngOnInit() {
    this.loadInitialData();
    
    this.scrollSubject.pipe(
      throttleTime(200),  // ✅ Limiter les vérifications
      map((event: any) => {
        const element = event.target;
        const atBottom = element.scrollHeight - element.scrollTop === element.clientHeight;
        return atBottom;
      }),
      filter(atBottom => atBottom),
      filter(() => !this.loading)
    ).subscribe(() => {
      this.loadMore();
    });
  }
  
  onScroll(event: Event) {
    this.scrollSubject.next(event);
  }
  
  loadMore() {
    this.loading = true;
    this.http.get<any[]>('/api/items').pipe(
      delay(500)
    ).subscribe(newItems => {
      this.items = [...this.items, ...newItems];
      this.loading = false;
    });
  }
  
  loadInitialData() {
    // Chargement initial
  }
}
```

### 🎧 auditTime() - Dernière Valeur dans l'Intervalle

**`auditTime()`** ignore les valeurs pendant N millisecondes, puis émet la **dernière** valeur reçue.

```typescript
import { fromEvent } from 'rxjs';
import { auditTime } from 'rxjs/operators';

fromEvent(button, 'click').pipe(
  auditTime(2000)
).subscribe(() => {
  console.log('Clicked!');
});
```

**Diagramme de marbre :**
```
source:       --a-b-c---d-e---f-g-h---|
auditTime(3): ------c-------e-------h-|
              └─3─┘     └─3─┘   └─3─┘
              (émet la dernière valeur de chaque période)
```

#### Différences throttleTime vs auditTime

```typescript
// throttleTime : Émet la PREMIÈRE valeur de chaque période
clicks$.pipe(throttleTime(1000))
// Si 5 clics rapides : émet le 1er, ignore les 4 suivants

// auditTime : Émet la DERNIÈRE valeur de chaque période
clicks$.pipe(auditTime(1000))
// Si 5 clics rapides : ignore les 4 premiers, émet le dernier
```

### 📊 sampleTime() - Échantillonner

**`sampleTime()`** émet la dernière valeur à intervalles réguliers.

```typescript
import { interval } from 'rxjs';
import { sampleTime } from 'rxjs/operators';

// Émet 0, 1, 2, 3... toutes les 100ms
interval(100).pipe(
  sampleTime(1000)  // Échantillonner toutes les secondes
).subscribe(console.log);

// Output (toutes les secondes):
// 9, 19, 29, 39...
```

**Diagramme de marbre :**
```
source:       --0-1-2-3-4-5-6-7-8-9-10-11-12-->
sampleTime:   --------4---------9----------14-->
              └─────┘ └─────┘  └─────┘
              (échantillonne à intervalles fixes)
```

#### Cas d'usage : Monitoring en Temps Réel

```typescript
@Component({
  selector: 'app-monitoring',
  template: `
    <div class="stats">
      <div>CPU: {{ cpuUsage }}%</div>
      <div>Memory: {{ memoryUsage }}%</div>
      <div>Last update: {{ lastUpdate | date:'HH:mm:ss' }}</div>
    </div>
  `
})
export class MonitoringComponent implements OnInit {
  cpuUsage: number = 0;
  memoryUsage: number = 0;
  lastUpdate: Date;
  
  constructor(private monitoringService: MonitoringService) {}
  
  ngOnInit() {
    // Les données arrivent toutes les 100ms
    // Mais on met à jour l'UI seulement toutes les 2 secondes
    this.monitoringService.stats$.pipe(
      sampleTime(2000)  // ✅ Échantillonner toutes les 2 secondes
    ).subscribe(stats => {
      this.cpuUsage = stats.cpu;
      this.memoryUsage = stats.memory;
      this.lastUpdate = new Date();
    });
  }
}
```

### 📊 Tableau Comparatif des Opérateurs Temporels

| Opérateur | Comportement | Cas d'usage |
|-----------|-------------|-------------|
| **debounceTime** | Attend le silence | Recherche, validation |
| **throttleTime** | Première valeur de chaque période | Scroll, resize |
| **auditTime** | Dernière valeur de chaque période | Clics répétés |
| **sampleTime** | Échantillonne à intervalle fixe | Monitoring, analytics |

---

## 4.3 Déduplication

### 🔍 distinct() - Valeurs Uniques

**`distinct()`** ne laisse passer que les valeurs qui n'ont jamais été émises auparavant.

```typescript
import { of } from 'rxjs';
import { distinct } from 'rxjs/operators';

of(1, 2, 2, 3, 1, 4, 3, 5, 2).pipe(
  distinct()
).subscribe(console.log);

// Output: 1, 2, 3, 4, 5
```

**Diagramme de marbre :**
```
source:    --1--2--2--3--1--4--3--5--|
distinct:  --1--2-----3-----4-----5--|
```

#### Distinct sur Propriété d'Objet

```typescript
interface User {
  id: number;
  name: string;
}

const users$ = of(
  { id: 1, name: 'Alice' },
  { id: 2, name: 'Bob' },
  { id: 1, name: 'Alice' },  // Doublon
  { id: 3, name: 'Charlie' },
  { id: 2, name: 'Bob' }      // Doublon
);

users$.pipe(
  distinct(user => user.id)  // ✅ Comparer par ID
).subscribe(console.log);

// Output:
// { id: 1, name: 'Alice' }
// { id: 2, name: 'Bob' }
// { id: 3, name: 'Charlie' }
```

#### Cas d'usage : Éviter les Doublons dans une Liste

```typescript
@Injectable({ providedIn: 'root' })
export class NotificationService {
  private notificationSubject = new Subject<Notification>();
  
  notifications$ = this.notificationSubject.pipe(
    distinct(notif => notif.id)  // ✅ Éviter les notifications en double
  );
  
  notify(notification: Notification) {
    this.notificationSubject.next(notification);
  }
}
```

### 🔄 distinctUntilChanged() - Changements Uniquement

**`distinctUntilChanged()`** ne laisse passer que les valeurs **différentes de la précédente**.

```typescript
import { of } from 'rxjs';
import { distinctUntilChanged } from 'rxjs/operators';

of(1, 1, 2, 2, 2, 3, 3, 1, 1, 4).pipe(
  distinctUntilChanged()
).subscribe(console.log);

// Output: 1, 2, 3, 1, 4
```

**Diagramme de marbre :**
```
source:              --1--1--2--2--3--3--1--4--|
distinctUntilChanged:--1-----2-----3-----1--4--|
```

#### Différence distinct() vs distinctUntilChanged()

```typescript
const values$ = of(1, 2, 2, 1, 3, 1);

// distinct : Ignore TOUTES les valeurs déjà vues
values$.pipe(distinct()).subscribe(console.log);
// Output: 1, 2, 3

// distinctUntilChanged : Ignore seulement si identique à la PRÉCÉDENTE
values$.pipe(distinctUntilChanged()).subscribe(console.log);
// Output: 1, 2, 1, 3, 1
```

#### Cas d'usage : Formulaires Réactifs

```typescript
@Component({
  selector: 'app-user-form'
})
export class UserFormComponent implements OnInit {
  userForm = new FormGroup({
    name: new FormControl(''),
    email: new FormControl('')
  });
  
  ngOnInit() {
    // Sauvegarder automatiquement quand les valeurs changent
    this.userForm.valueChanges.pipe(
      distinctUntilChanged((prev, curr) => 
        JSON.stringify(prev) === JSON.stringify(curr)
      ),
      debounceTime(500),
      tap(() => console.log('Saving...'))
    ).subscribe(values => {
      this.save(values);
    });
  }
  
  save(values: any) {
    // Logique de sauvegarde
  }
}
```

### 🔑 distinctUntilKeyChanged() - Par Propriété

**`distinctUntilKeyChanged()`** compare une propriété spécifique entre les valeurs consécutives.

```typescript
import { of } from 'rxjs';
import { distinctUntilKeyChanged } from 'rxjs/operators';

interface User {
  id: number;
  name: string;
  status: string;
}

const users$ = of(
  { id: 1, name: 'Alice', status: 'online' },
  { id: 1, name: 'Alice', status: 'online' },   // Même status
  { id: 1, name: 'Alice', status: 'offline' },  // Status change
  { id: 1, name: 'Alice', status: 'offline' },  // Même status
  { id: 1, name: 'Alice', status: 'online' }    // Status change
);

users$.pipe(
  distinctUntilKeyChanged('status')  // ✅ Comparer seulement le status
).subscribe(user => {
  console.log(`User is now ${user.status}`);
});

// Output:
// User is now online
// User is now offline
// User is now online
```

#### Cas d'usage : Suivre les Changements d'État

```typescript
@Injectable({ providedIn: 'root' })
export class ConnectionService {
  private stateSubject = new BehaviorSubject<ConnectionState>({
    status: 'disconnected',
    timestamp: new Date()
  });
  
  // Émettre seulement quand le status change
  statusChanges$ = this.stateSubject.pipe(
    distinctUntilKeyChanged('status')
  );
  
  updateStatus(status: string) {
    this.stateSubject.next({
      status,
      timestamp: new Date()
    });
  }
}

// Component
constructor(private connection: ConnectionService) {}

ngOnInit() {
  this.connection.statusChanges$.subscribe(state => {
    console.log('Connection status changed to:', state.status);
    this.showNotification(`You are now ${state.status}`);
  });
}
```

---

## 4.4 Exercice Pratique

### 🎯 Challenge : Système de Recherche Optimisé

Créez un système de recherche avec :
1. Input avec debounce de 300ms
2. Minimum 3 caractères
3. Ignorer les valeurs identiques consécutives
4. Annuler les requêtes précédentes
5. Afficher un indicateur de chargement
6. Gérer les erreurs
7. Limiter à 10 résultats
8. Historique des 5 dernières recherches

### 💡 Solution

```typescript
// search.model.ts
export interface SearchResult {
  id: number;
  title: string;
  description: string;
  url: string;
}

export interface SearchState {
  query: string;
  results: SearchResult[];
  loading: boolean;
  error: string | null;
}

// search.service.ts
import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, of } from 'rxjs';
import { delay, map } from 'rxjs/operators';

@Injectable({ providedIn: 'root' })
export class SearchService {
  private searchHistory: string[] = [];
  
  constructor(private http: HttpClient) {
    // Charger l'historique depuis localStorage
    const saved = localStorage.getItem('searchHistory');
    if (saved) {
      this.searchHistory = JSON.parse(saved);
    }
  }
  
  search(query: string): Observable<SearchResult[]> {
    return this.http.get<SearchResult[]>(
      `/api/search?q=${encodeURIComponent(query)}&limit=10`
    ).pipe(
      map(results => {
        // Ajouter à l'historique
        this.addToHistory(query);
        return results;
      })
    );
  }
  
  getHistory(): string[] {
    return [...this.searchHistory];
  }
  
  clearHistory() {
    this.searchHistory = [];
    localStorage.removeItem('searchHistory');
  }
  
  private addToHistory(query: string) {
    // Retirer si existe déjà
    this.searchHistory = this.searchHistory.filter(q => q !== query);
    
    // Ajouter au début
    this.searchHistory.unshift(query);
    
    // Garder seulement les 5 dernières
    this.searchHistory = this.searchHistory.slice(0, 5);
    
    // Sauvegarder
    localStorage.setItem('searchHistory', JSON.stringify(this.searchHistory));
  }
}

// search.component.ts
import { Component, OnInit, ViewChild, ElementRef, OnDestroy } from '@angular/core';
import { FormControl } from '@angular/forms';
import { Subject } from 'rxjs';
import {
  debounceTime,
  distinctUntilChanged,
  filter,
  switchMap,
  catchError,
  tap,
  takeUntil
} from 'rxjs/operators';
import { SearchService } from './search.service';
import { SearchResult, SearchState } from './search.model';

@Component({
  selector: 'app-optimized-search',
  template: `
    <div class="search-container">
      <!-- Barre de recherche -->
      <div class="search-box">
        <input 
          #searchInput
          type="text"
          [formControl]="searchControl"
          placeholder="Rechercher... (min. 3 caractères)"
          class="search-input">
        
        <button 
          *ngIf="searchControl.value" 
          (click)="clearSearch()"
          class="clear-btn">
          ✕
        </button>
      </div>
      
      <!-- Info de recherche -->
      <div class="search-info" *ngIf="state.query">
        <span>Recherche: "{{ state.query }}"</span>
        <span *ngIf="!state.loading && state.results.length > 0">
          {{ state.results.length }} résultat(s)
        </span>
      </div>
      
      <!-- Loading -->
      <div *ngIf="state.loading" class="loading">
        <div class="spinner"></div>
        Recherche en cours...
      </div>
      
      <!-- Erreur -->
      <div *ngIf="state.error" class="error">
        {{ state.error }}
      </div>
      
      <!-- Historique -->
      <div class="history" *ngIf="history.length > 0 && !state.query && !state.loading">
        <h4>Recherches récentes</h4>
        <div class="history-items">
          <button 
            *ngFor="let item of history"
            (click)="searchFromHistory(item)"
            class="history-item">
            🔍 {{ item }}
          </button>
        </div>
        <button (click)="clearHistory()" class="clear-history">
          Effacer l'historique
        </button>
      </div>
      
      <!-- Résultats -->
      <div class="results" *ngIf="state.results.length > 0 && !state.loading">
        <div *ngFor="let result of state.results" class="result-item">
          <h3>{{ result.title }}</h3>
          <p class="description">{{ result.description }}</p>
          <a [href]="result.url" target="_blank" class="link">
            Voir plus →
          </a>
        </div>
      </div>
      
      <!-- Aucun résultat -->
      <div 
        *ngIf="state.results.length === 0 && !state.loading && state.query && !state.error" 
        class="no-results">
        <p>Aucun résultat pour "{{ state.query }}"</p>
        <p class="hint">Essayez avec des mots-clés différents</p>
      </div>
    </div>
  `,
  styles: [`
    .search-container {
      max-width: 800px;
      margin: 40px auto;
      padding: 20px;
    }
    
    .search-box {
      position: relative;
      margin-bottom: 20px;
    }
    
    .search-input {
      width: 100%;
      padding: 15px 45px 15px 15px;
      font-size: 16px;
      border: 2px solid #ddd;
      border-radius: 8px;
      transition: border-color 0.3s;
    }
    
    .search-input:focus {
      outline: none;
      border-color: #4CAF50;
    }
    
    .clear-btn {
      position: absolute;
      right: 10px;
      top: 50%;
      transform: translateY(-50%);
      background: none;
      border: none;
      font-size: 20px;
      cursor: pointer;
      color: #999;
      padding: 5px 10px;
    }
    
    .clear-btn:hover {
      color: #333;
    }
    
    .search-info {
      display: flex;
      justify-content: space-between;
      padding: 10px;
      background: #f5f5f5;
      border-radius: 4px;
      margin-bottom: 20px;
      font-size: 14px;
      color: #666;
    }
    
    .loading {
      text-align: center;
      padding: 40px;
      color: #666;
    }
    
    .spinner {
      border: 3px solid #f3f3f3;
      border-top: 3px solid #4CAF50;
      border-radius: 50%;
      width: 40px;
      height: 40px;
      animation: spin 1s linear infinite;
      margin: 0 auto 15px;
    }
    
    @keyframes spin {
      0% { transform: rotate(0deg); }
      100% { transform: rotate(360deg); }
    }
    
    .error {
      background: #f44336;
      color: white;
      padding: 15px;
      border-radius: 8px;
      margin-bottom: 20px;
    }
    
    .history {
      background: #f9f9f9;
      padding: 20px;
      border-radius: 8px;
      margin-bottom: 20px;
    }
    
    .history h4 {
      margin: 0 0 15px 0;
      color: #333;
    }
    
    .history-items {
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
      margin-bottom: 15px;
    }
    
    .history-item {
      background: white;
      border: 1px solid #ddd;
      padding: 8px 15px;
      border-radius: 20px;
      cursor: pointer;
      transition: all 0.3s;
    }
    
    .history-item:hover {
      background: #4CAF50;
      color: white;
      border-color: #4CAF50;
    }
    
    .clear-history {
      background: none;
      border: none;
      color: #f44336;
      cursor: pointer;
      font-size: 14px;
      text-decoration: underline;
    }
    
    .results {
      display: grid;
      gap: 15px;
    }
    
    .result-item {
      border: 1px solid #ddd;
      padding: 20px;
      border-radius: 8px;
      transition: all 0.3s;
    }
    
    .result-item:hover {
      box-shadow: 0 4px 12px rgba(0,0,0,0.1);
      border-color: #4CAF50;
    }
    
    .result-item h3 {
      margin: 0 0 10px 0;
      color: #333;
    }
    
    .description {
      margin: 0 0 15px 0;
      color: #666;
      line-height: 1.5;
    }
    
    .link {
      color: #4CAF50;
      text-decoration: none;
      font-weight: 500;
    }
    
    .link:hover {
      text-decoration: underline;
    }
    
    .no-results {
      text-align: center;
      padding: 60px 20px;
      color: #999;
    }
    
    .no-results p {
      margin: 10px 0;
    }
    
    .hint {
      font-size: 14px;
      color: #bbb;
    }
  `]
})
export class OptimizedSearchComponent implements OnInit, OnDestroy {
  @ViewChild('searchInput') searchInput: ElementRef;
  
  searchControl = new FormControl('');
  
  state: SearchState = {
    query: '',
    results: [],
    loading: false,
    error: null
  };
  
  history: string[] = [];
  
  private destroy$ = new Subject<void>();
  
  constructor(private searchService: SearchService) {}
  
  ngOnInit() {
    // Charger l'historique
    this.history = this.searchService.getHistory();
    
    // Pipeline de recherche optimisé
    this.searchControl.valueChanges.pipe(
      // 1. Attendre 300ms après la dernière frappe
      debounceTime(300),
      
      // 2. Ignorer si la valeur n'a pas changé
      distinctUntilChanged(),
      
      // 3. Filtrer : minimum 3 caractères
      tap(query => {
        if (!query || query.length < 3) {
          this.resetResults();
        }
      }),
      filter(query => query && query.length >= 3),
      
      // 4. Mettre à jour l'état : loading = true
      tap(query => {
        this.state = {
          query,
          results: [],
          loading: true,
          error: null
        };
      }),
      
      // 5. Effectuer la recherche (annule les précédentes)
      switchMap(query => 
        this.searchService.search(query).pipe(
          catchError(error => {
            console.error('Search error:', error);
            this.state.error = 'Une erreur est survenue lors de la recherche';
            this.state.loading = false;
            return of([]);
          })
        )
      ),
      
      // 6. Prendre seulement 10 résultats
      map(results => results.slice(0, 10)),
      
      // 7. Se désabonner automatiquement à la destruction
      takeUntil(this.destroy$)
      
    ).subscribe(results => {
      this.state = {
        ...this.state,
        results,
        loading: false
      };
      
      // Mettre à jour l'historique
      this.history = this.searchService.getHistory();
    });
  }
  
  searchFromHistory(query: string) {
    this.searchControl.setValue(query);
  }
  
  clearSearch() {
    this.searchControl.setValue('');
    this.resetResults();
    this.searchInput.nativeElement.focus();
  }
  
  clearHistory() {
    this.searchService.clearHistory();
    this.history = [];
  }
  
  private resetResults() {
    this.state = {
      query: '',
      results: [],
      loading: false,
      error: null
    };
  }
  
  ngOnDestroy() {
    this.destroy$.next();
    this.destroy$.complete();
  }
}
```

### 🎨 Version avec Analytics

```typescript
@Component({
  selector: 'app-search-with-analytics'
})
export class SearchWithAnalyticsComponent implements OnInit {
  searchControl = new FormControl('');
  
  constructor(
    private searchService: SearchService,
    private analytics: AnalyticsService
  ) {}
  
  ngOnInit() {
    this.searchControl.valueChanges.pipe(
      debounceTime(300),
      distinctUntilChanged(),
      filter(query => query && query.length >= 3),
      
      // ✅ Suivre les recherches dans Analytics
      tap(query => {
        this.analytics.trackEvent('search_started', { query });
      }),
      
      switchMap(query => 
        this.searchService.search(query).pipe(
          // ✅ Suivre les résultats
          tap(results => {
            this.analytics.trackEvent('search_completed', {
              query,
              resultsCount: results.length
            });
          }),
          catchError(error => {
            // ✅ Suivre les erreurs
            this.analytics.trackEvent('search_error', {
              query,
              error: error.message
            });
            return of([]);
          })
        )
      )
    ).subscribe(results => {
      // Afficher les résultats
    });
  }
}
```

---

## 📝 Questions/Réponses d'Entretien

### Q1 : Quelle est la différence entre filter() et take() ?

**Réponse :**
> **`filter()`** :
> - Filtre selon une **condition**
> - Continue tant que l'Observable émet
> - Peut laisser passer 0, 1 ou plusieurs valeurs
> 
> **`take()`** :
> - Prend un **nombre fixe** de valeurs
> - Se complète automatiquement après N émissions
> - Toujours prévisible
> 
> ```typescript
> // filter : nombre variable de valeurs
> of(1, 2, 3, 4, 5).pipe(
>   filter(x => x % 2 === 0)
> ).subscribe(console.log);
> // Output: 2, 4
> 
> // take : nombre fixe de valeurs
> of(1, 2, 3, 4, 5).pipe(
>   take(3)
> ).subscribe(console.log);
> // Output: 1, 2, 3
> ```

### Q2 : Expliquez la différence entre debounceTime() et throttleTime()

**Réponse :**
> **`debounceTime(n)`** :
> - **Attend le silence** : émet après N ms sans nouvelle valeur
> - Retarde l'émission
> - Utiliser pour : recherche, validation de formulaire
> 
> **`throttleTime(n)`** :
> - **Limite la fréquence** : émet la première valeur, puis ignore pendant N ms
> - Émet immédiatement
> - Utiliser pour : scroll, resize, clics répétés
> 
> **Exemple pratique** :
> ```typescript
> // debounceTime : Recherche
> searchInput$.pipe(
>   debounceTime(300)  // Attend 300ms après la dernière frappe
> )
> 
> // throttleTime : Scroll
> scroll$.pipe(
>   throttleTime(200)  // Maximum 1 événement toutes les 200ms
> )
> ```

### Q3 : Quand utiliser takeUntil() plutôt que unsubscribe() ?

**Réponse :**
> **`takeUntil()`** est préférable quand vous avez **plusieurs souscriptions** car il permet de toutes les gérer avec un seul Subject :
> 
> ```typescript
> // ✅ BON : takeUntil pour plusieurs souscriptions
> export class MyComponent implements OnDestroy {
>   private destroy$ = new Subject<void>();
>   
>   ngOnInit() {
>     observable1$.pipe(takeUntil(this.destroy$)).subscribe();
>     observable2$.pipe(takeUntil(this.destroy$)).subscribe();
>     observable3$.pipe(takeUntil(this.destroy$)).subscribe();
>   }
>   
>   ngOnDestroy() {
>     this.destroy$.next();
>     this.destroy$.complete();
>   }
> }
> 
> // ❌ Moins pratique : unsubscribe manuel
> export class MyComponent implements OnDestroy {
>   sub1: Subscription;
>   sub2: Subscription;
>   sub3: Subscription;
>   
>   ngOnInit() {
>     this.sub1 = observable1$.subscribe();
>     this.sub2 = observable2$.subscribe();
>     this.sub3 = observable3$.subscribe();
>   }
>   
>   ngOnDestroy() {
>     this.sub1.unsubscribe();
>     this.sub2.unsubscribe();
>     this.sub3.unsubscribe();
>   }
> }
> ```

### Q4 : Quelle est la différence entre distinct() et distinctUntilChanged() ?

**Réponse :**
> **`distinct()`** :
> - Compare avec **toutes les valeurs précédentes**
> - Garde en mémoire toutes les valeurs vues
> - Peut consommer beaucoup de mémoire sur des flux longs
> 
> **`distinctUntilChanged()`** :
> - Compare seulement avec la **valeur précédente**
> - Mémoire constante (O(1))
> - Plus performant
> 
> ```typescript
> const values$ = of(1, 2, 2, 1, 3, 1);
> 
> values$.pipe(distinct()).subscribe(console.log);
> // Output: 1, 2, 3 (1 n'est émis qu'une fois)
> 
> values$.pipe(distinctUntilChanged()).subscribe(console.log);
> // Output: 1, 2, 1, 3, 1 (1 peut revenir)
> ```

### Q5 : Comment optimiser une recherche en temps réel ?

**Réponse :**
> Combinez plusieurs opérateurs de filtrage :
> 
> ```typescript
> searchInput$.pipe(
>   // 1. Attendre le silence
>   debounceTime(300),
>   
>   // 2. Ignorer les doublons consécutifs
>   distinctUntilChanged(),
>   
>   // 3. Minimum de caractères
>   filter(query => query.length >= 3),
>   
>   // 4. Annuler les requêtes précédentes
>   switchMap(query => this.http.get(`/api/search?q=${query}`))
> )
> ```
> 
> Ces 4 opérateurs ensemble permettent de :
> - Réduire le nombre de requêtes HTTP
> - Éviter les race conditions
> - Améliorer l'expérience utilisateur

### Q6 : Pourquoi utiliser take(1) avec HttpClient ?

**Réponse :**
> **HttpClient se complète automatiquement** après 1 émission, donc `take(1)` n'est techniquement pas nécessaire. Cependant, certains développeurs l'utilisent pour :
> 
> **1. Clarté du code** :
> ```typescript
> this.http.get('/api/data').pipe(
>   take(1)  // Rend explicite qu'on attend 1 seule valeur
> ).subscribe();
> ```
> 
> **2. Cohérence** :
> ```typescript
> // Tous les Observables utilisent take(1)
> merge(
>   this.http.get('/api/users').pipe(take(1)),
>   this.dataService.getData().pipe(take(1))
> )
> ```
> 
> **Conclusion** : Pas obligatoire avec HttpClient, mais peut améliorer la lisibilité.

### Q7 : Comment limiter le nombre d'événements scroll ?

**Réponse :**
> Utilisez `throttleTime()` ou `auditTime()` :
> 
> ```typescript
> // ✅ throttleTime : Première valeur de chaque période
> fromEvent(window, 'scroll').pipe(
>   throttleTime(200)  // Max 1 événement toutes les 200ms
> ).subscribe(() => {
>   console.log('Scroll position:', window.scrollY);
> });
> 
> // ✅ auditTime : Dernière valeur de chaque période
> fromEvent(window, 'scroll').pipe(
>   auditTime(200)  // Dernière position toutes les 200ms
> ).subscribe(() => {
>   console.log('Final position:', window.scrollY);
> });
> ```
> 
> **Différence** :
> - `throttleTime` → Position au **début** du scroll
> - `auditTime` → Position à la **fin** du scroll

### Q8 : Comment implémenter un countdown timer avec RxJS ?

**Réponse :**
> Utilisez `takeWhile()` pour arrêter quand le temps atteint 0 :
> 
> ```typescript
> const countdown$ = interval(1000).pipe(
>   map(i => 60 - i),          // Décompter de 60
>   takeWhile(time => time >= 0, true),  // Inclure 0
>   tap(time => console.log(`${time} secondes`))
> );
> 
> countdown$.subscribe({
>   complete: () => console.log('Temps écoulé !')
> });
> ```

### Q9 : Comment éviter les memory leaks avec les Observables qui n'émettent jamais ?

**Réponse :**
> Utilisez toujours `takeUntil()` dans les composants Angular :
> 
> ```typescript
> export class MyComponent implements OnDestroy {
>   private destroy$ = new Subject<void>();
>   
>   ngOnInit() {
>     // ✅ Même si cet Observable n'émet jamais,
>     // il sera nettoyé à la destruction
>     this.infiniteObservable$.pipe(
>       takeUntil(this.destroy$)
>     ).subscribe();
>   }
>   
>   ngOnDestroy() {
>     this.destroy$.next();
>     this.destroy$.complete();
>   }
> }
> ```

### Q10 : Quelle est la différence entre skipUntil() et skipWhile() ?

**Réponse :**
> **`skipUntil(notifier$)`** :
> - Attend qu'un **autre Observable** émette
> - S'active avec un événement externe
> 
> **`skipWhile(predicate)`** :
> - Évalue une **condition** sur chaque valeur
> - S'active quand la condition devient fausse
> 
> ```typescript
> // skipUntil : Attendre un signal externe
> const start$ = timer(3000);
> interval(1000).pipe(
>   skipUntil(start$)  // Commence après 3 secondes
> ).subscribe(console.log);
> 
> // skipWhile : Condition sur les valeurs
> interval(1000).pipe(
>   skipWhile(value => value < 5)  // Saute < 5
> ).subscribe(console.log);
> // Output: 5, 6, 7, 8...
> ```

### Q11 : Comment implémenter un infinite scroll avec RxJS ?

**Réponse :**
> Combinez `throttleTime()`, `filter()`, et `switchMap()` :
> 
> ```typescript
> fromEvent(scrollElement, 'scroll').pipe(
>   throttleTime(200),  // Limiter les vérifications
>   map(() => {
>     const atBottom = scrollElement.scrollHeight - 
>                     scrollElement.scrollTop === 
>                     scrollElement.clientHeight;
>     return atBottom;
>   }),
>   filter(atBottom => atBottom),  // Seulement en bas
>   filter(() => !this.loading),   // Pas déjà en train de charger
>   switchMap(() => this.loadMore())
> ).subscribe();
> ```

### Q12 : Pourquoi utiliser distinctUntilKeyChanged() plutôt que distinctUntilChanged() ?

**Réponse :**
> **`distinctUntilKeyChanged()`** est plus performant et lisible quand vous comparez une **seule propriété** :
> 
> ```typescript
> // ❌ Moins lisible
> users$.pipe(
>   distinctUntilChanged((prev, curr) => prev.status === curr.status)
> )
> 
> // ✅ Plus clair
> users$.pipe(
>   distinctUntilKeyChanged('status')
> )
> ```
> 
> **Avantages** :
> - Code plus concis
> - Intention claire
> - Meilleure performance (pas de fonction custom)

---

## 🎯 Points Clés du Chapitre 4

### Filtrage de Valeurs
✅ **filter()** : Condition de filtrage
✅ **take(n)** : Prendre N valeurs
✅ **takeUntil()** : Arrêter avec un signal externe (pattern de unsubscribe)
✅ **takeWhile()** : Continuer tant que condition vraie
✅ **skip(n)** : Sauter les N premières valeurs

### Gestion du Temps
✅ **debounceTime()** : Recherche, validation (attendre le silence)
✅ **throttleTime()** : Scroll, resize (limiter la fréquence)
✅ **auditTime()** : Dernière valeur de chaque période
✅ **sampleTime()** : Échantillonnage à intervalle fixe

### Déduplication
✅ **distinct()** : Valeurs uniques (toute l'histoire)
✅ **distinctUntilChanged()** : Changements uniquement (vs précédent)
✅ **distinctUntilKeyChanged()** : Comparaison par propriété

### Best Practices
✅ **debounceTime + distinctUntilChanged** pour la recherche
✅ **throttleTime** pour scroll/resize
✅ **takeUntil(destroy$)** pour éviter memory leaks
✅ **filter + switchMap** pour requêtes conditionnelles

---