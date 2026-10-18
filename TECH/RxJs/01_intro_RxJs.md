## [**..**](./00_index.md)

# Chapitre 1 : Introduction à RxJS

## 📑 Table des matières
- [1.1 Qu'est-ce que RxJS ?](#11-quest-ce-que-rxjs)
- [1.2 Concepts Fondamentaux](#12-concepts-fondamentaux)
- [1.3 Premier Observable](#13-premier-observable)
- [1.4 Exercice Pratique](#14-exercice-pratique)
- [Questions/Réponses d'Entretien](#-questionsréponses-dentretien)

---

## 1.1 Qu'est-ce que RxJS ?

### 🎯 Définition

**RxJS** (Reactive Extensions for JavaScript) est une bibliothèque pour la **programmation réactive** utilisant des Observables. Elle facilite la composition de code asynchrone et basé sur les événements.

### 🌊 Programmation Réactive

La programmation réactive traite les **flux de données asynchrones** comme des streams (flux) que vous pouvez observer et transformer.

**Analogie simple :**
```
Programmation traditionnelle = Tirer de l'eau d'un puits (on demande)
Programmation réactive = Rivière qui coule (on observe)
```

#### Exemple Concret

```typescript
// ❌ Programmation impérative traditionnelle
let count = 0;
button.addEventListener('click', () => {
  count++;
  console.log(`Cliqué ${count} fois`);
});

// ✅ Programmation réactive avec RxJS
const clicks$ = fromEvent(button, 'click');
clicks$.pipe(
  scan((count) => count + 1, 0)
).subscribe(count => {
  console.log(`Cliqué ${count} fois`);
});
```

### 📊 Observable vs Promise

| Aspect | Promise | Observable |
|--------|---------|------------|
| **Valeurs** | 1 seule valeur | 0, 1 ou plusieurs valeurs |
| **Lazy** | ❌ Non (s'exécute immédiatement) | ✅ Oui (exécute à la souscription) |
| **Annulable** | ❌ Non | ✅ Oui (unsubscribe) |
| **Opérateurs** | then, catch | map, filter, merge, etc. (100+) |
| **Multicast** | ❌ Non | ✅ Oui (avec Subjects) |

#### Exemple de Comparaison

```typescript
// Promise : 1 valeur, s'exécute immédiatement
const promise = fetch('/api/users');
promise.then(data => console.log(data));

// Observable : plusieurs valeurs, lazy, annulable
const users$ = new Observable(subscriber => {
  fetch('/api/users')
    .then(response => response.json())
    .then(data => {
      subscriber.next(data);  // Émet la valeur
      subscriber.complete();   // Termine
    })
    .catch(err => subscriber.error(err));
});

// Ne s'exécute que lors de la souscription
const subscription = users$.subscribe({
  next: data => console.log(data),
  error: err => console.error(err),
  complete: () => console.log('Terminé')
});

// Peut être annulé
subscription.unsubscribe();
```

### 🎨 Cas d'Usage dans Angular

RxJS est **intégré** dans Angular pour :

1. **HttpClient** : Toutes les requêtes HTTP
```typescript
this.http.get<User[]>('/api/users').subscribe(users => {
  console.log(users);
});
```

2. **Formulaires Réactifs** : Changements de valeurs
```typescript
this.form.valueChanges.subscribe(values => {
  console.log('Formulaire modifié:', values);
});
```

3. **Router** : Navigation et paramètres
```typescript
this.route.params.subscribe(params => {
  const id = params['id'];
});
```

4. **EventEmitter** : Communication entre composants
```typescript
@Output() userSelected = new EventEmitter<User>();
```

5. **Gestion d'état** : Services avec Subjects
```typescript
private userSubject = new BehaviorSubject<User>(null);
user$ = this.userSubject.asObservable();
```

### 📦 Installation et Configuration

#### Installation

```bash
# RxJS est déjà inclus dans Angular
# Pour un projet non-Angular :
npm install rxjs

# Version utilisée (pour référence)
# Angular 17+ : RxJS 7.8+
```

#### Import

```typescript
// Importer les opérateurs nécessaires
import { Observable, of, from, interval } from 'rxjs';
import { map, filter, take } from 'rxjs/operators';

// Angular - déjà configuré
import { Component } from '@angular/core';
import { HttpClient } from '@angular/common/http';
```

#### Configuration TypeScript

```json
// tsconfig.json (déjà configuré dans Angular)
{
  "compilerOptions": {
    "target": "ES2020",
    "lib": ["ES2020", "DOM"],
    "module": "ES2020",
    "moduleResolution": "node"
  }
}
```

---

## 1.2 Concepts Fondamentaux

### 🎯 Les 5 Concepts Clés

#### 1. Observable

Un **Observable** est un flux de données qui peut émettre 0, 1 ou plusieurs valeurs dans le temps.

```typescript
// Observable simple qui émet 3 valeurs
const numbers$ = new Observable<number>(subscriber => {
  subscriber.next(1);      // Émet 1
  subscriber.next(2);      // Émet 2
  subscriber.next(3);      // Émet 3
  subscriber.complete();   // Termine
});

// Convention : nom$ pour les Observables
```

**Diagramme de marbre (Marble Diagram) :**
```
numbers$: --1--2--3--|
          └─┴─┴─┴──┘
          valeurs  complete
```

#### 2. Observer

Un **Observer** est un objet qui définit comment réagir aux valeurs émises.

```typescript
const observer = {
  next: (value: number) => {
    console.log('Valeur reçue:', value);
  },
  error: (err: Error) => {
    console.error('Erreur:', err);
  },
  complete: () => {
    console.log('Flux terminé');
  }
};

// Les 3 méthodes sont optionnelles
const simpleObserver = {
  next: (value: number) => console.log(value)
};
```

#### 3. Subscription

Une **Subscription** représente l'exécution d'un Observable. Elle permet de **désouscrire**.

```typescript
const subscription = numbers$.subscribe(observer);

// Désouscrire (important pour éviter les memory leaks !)
subscription.unsubscribe();
```

#### 4. Opérateurs

Les **opérateurs** sont des fonctions qui transforment, filtrent ou combinent des Observables.

```typescript
import { map, filter } from 'rxjs/operators';

// Transformer les valeurs
numbers$.pipe(
  map(x => x * 2),      // Multiplier par 2
  filter(x => x > 3)    // Garder seulement > 3
).subscribe(console.log);

// Output: 4, 6
```

**Diagramme de marbre :**
```
numbers$:     --1--2--3--|
  map(x*2):   --2--4--6--|
  filter(>3): -----4--6--|
```

#### 5. Subject

Un **Subject** est à la fois un Observable ET un Observer. Il permet le **multicast**.

```typescript
import { Subject } from 'rxjs';

const subject = new Subject<number>();

// Souscription 1
subject.subscribe(x => console.log('Observer A:', x));

// Souscription 2
subject.subscribe(x => console.log('Observer B:', x));

// Émettre des valeurs
subject.next(1);  // A et B reçoivent 1
subject.next(2);  // A et B reçoivent 2

// Output:
// Observer A: 1
// Observer B: 1
// Observer A: 2
// Observer B: 2
```

### 🔄 Flux de Données

```
┌─────────────┐
│  Observable │  Producteur de données
└──────┬──────┘
       │ emit (next, error, complete)
       ↓
┌─────────────┐
│ Opérateurs  │  Transformation du flux
│ (pipe)      │  map, filter, etc.
└──────┬──────┘
       │
       ↓
┌─────────────┐
│  Observer   │  Consommateur de données
│ (subscribe) │  next, error, complete
└─────────────┘
```

### 📝 Exemple Complet

```typescript
import { Observable } from 'rxjs';
import { map, filter } from 'rxjs/operators';

// 1. Créer un Observable
const source$ = new Observable<number>(subscriber => {
  console.log('Observable exécuté');
  
  subscriber.next(1);
  subscriber.next(2);
  subscriber.next(3);
  subscriber.next(4);
  subscriber.next(5);
  
  subscriber.complete();
});

// 2. Transformer avec des opérateurs
const transformed$ = source$.pipe(
  map(x => x * 10),        // Multiplier par 10
  filter(x => x > 20)      // Garder seulement > 20
);

// 3. Souscrire
console.log('Avant souscription');

const subscription = transformed$.subscribe({
  next: value => console.log('Valeur:', value),
  error: err => console.error('Erreur:', err),
  complete: () => console.log('Terminé!')
});

console.log('Après souscription');

// Output:
// Avant souscription
// Observable exécuté
// Valeur: 30
// Valeur: 40
// Valeur: 50
// Terminé!
// Après souscription
```

---

## 1.3 Premier Observable

### 🎯 Création de Base

#### Méthode 1 : new Observable()

```typescript
import { Observable } from 'rxjs';

const custom$ = new Observable<string>(subscriber => {
  // Code synchrone
  subscriber.next('Hello');
  subscriber.next('World');
  
  // Code asynchrone
  setTimeout(() => {
    subscriber.next('Delayed');
    subscriber.complete();
  }, 1000);
});

custom$.subscribe({
  next: value => console.log(value),
  complete: () => console.log('Done!')
});

// Output:
// Hello
// World
// (attendre 1 seconde)
// Delayed
// Done!
```

#### Méthode 2 : Opérateurs de Création

```typescript
import { of, from, interval } from 'rxjs';

// of() : Émettre des valeurs séquentiellement
const numbers$ = of(1, 2, 3, 4, 5);
numbers$.subscribe(console.log);
// Output: 1, 2, 3, 4, 5

// from() : Convertir un array, Promise, etc.
const array$ = from([10, 20, 30]);
array$.subscribe(console.log);
// Output: 10, 20, 30

// interval() : Émettre à intervalle régulier
const timer$ = interval(1000);  // Chaque seconde
timer$.subscribe(console.log);
// Output: 0, 1, 2, 3, 4... (à l'infini)
```

### 🔌 Souscription (subscribe)

```typescript
// Forme complète
observable$.subscribe({
  next: value => console.log('Valeur:', value),
  error: err => console.error('Erreur:', err),
  complete: () => console.log('Terminé')
});

// Forme courte (seulement next)
observable$.subscribe(value => console.log(value));

// Forme avec error
observable$.subscribe(
  value => console.log(value),
  error => console.error(error)
);

// Forme avec les 3
observable$.subscribe(
  value => console.log(value),
  error => console.error(error),
  () => console.log('Terminé')
);
```

### 🛑 Unsubscribe (Désouscrire)

**TRÈS IMPORTANT** pour éviter les **memory leaks** !

```typescript
import { interval } from 'rxjs';

// Observable qui émet chaque seconde
const timer$ = interval(1000);

// Souscrire
const subscription = timer$.subscribe(console.log);

// Désouscrire après 5 secondes
setTimeout(() => {
  subscription.unsubscribe();
  console.log('Désouscrit!');
}, 5000);

// Output: 0, 1, 2, 3, 4, Désouscrit!
```

#### Patterns de Unsubscribe

**Pattern 1 : Manuel**
```typescript
export class MyComponent {
  subscription: Subscription;
  
  ngOnInit() {
    this.subscription = this.dataService.getData()
      .subscribe(data => console.log(data));
  }
  
  ngOnDestroy() {
    // ✅ Toujours désouscrire dans ngOnDestroy
    this.subscription.unsubscribe();
  }
}
```

**Pattern 2 : Subscription multiple**
```typescript
export class MyComponent {
  subscriptions = new Subscription();
  
  ngOnInit() {
    this.subscriptions.add(
      this.service1.getData().subscribe(/* ... */)
    );
    
    this.subscriptions.add(
      this.service2.getData().subscribe(/* ... */)
    );
  }
  
  ngOnDestroy() {
    // ✅ Désouscrire tout d'un coup
    this.subscriptions.unsubscribe();
  }
}
```

**Pattern 3 : AsyncPipe (Recommandé dans Angular)**
```typescript
export class MyComponent {
  // ✅ Pas besoin de unsubscribe, Angular le fait automatiquement
  data$ = this.dataService.getData();
}
```

```html
<!-- Template -->
<div *ngIf="data$ | async as data">
  {{ data.name }}
</div>
```

**Pattern 4 : takeUntil**
```typescript
import { Subject } from 'rxjs';
import { takeUntil } from 'rxjs/operators';

export class MyComponent implements OnDestroy {
  private destroy$ = new Subject<void>();
  
  ngOnInit() {
    this.service.getData()
      .pipe(takeUntil(this.destroy$))
      .subscribe(data => console.log(data));
    
    this.service.getOtherData()
      .pipe(takeUntil(this.destroy$))
      .subscribe(data => console.log(data));
  }
  
  ngOnDestroy() {
    // ✅ Déclenche la complétion de tous les Observables
    this.destroy$.next();
    this.destroy$.complete();
  }
}
```

### ⚠️ Memory Leaks : Ce qu'il faut éviter

```typescript
// ❌ MAUVAIS : Pas de unsubscribe
export class BadComponent implements OnInit {
  ngOnInit() {
    interval(1000).subscribe(console.log);
    // Le timer continue même après destruction du composant !
  }
}

// ✅ BON : Avec unsubscribe
export class GoodComponent implements OnInit, OnDestroy {
  subscription: Subscription;
  
  ngOnInit() {
    this.subscription = interval(1000).subscribe(console.log);
  }
  
  ngOnDestroy() {
    this.subscription.unsubscribe();
  }
}
```

### 🎨 Exemple Pratique : Compteur de Clics

```typescript
import { fromEvent } from 'rxjs';
import { map, scan } from 'rxjs/operators';

// HTML: <button id="myButton">Cliquez-moi</button>

const button = document.getElementById('myButton');

// Créer un Observable des événements de clic
const clicks$ = fromEvent(button, 'click');

// Compter les clics
const counter$ = clicks$.pipe(
  scan((count) => count + 1, 0),
  map(count => `Cliqué ${count} fois`)
);

// Souscrire
const subscription = counter$.subscribe(
  message => {
    console.log(message);
    button.textContent = message;
  }
);

// Dans un composant Angular, désouscrire dans ngOnDestroy
// subscription.unsubscribe();
```

---

## 1.4 Exercice Pratique

### 🎯 Challenge : Timer avec Contrôles

Créez un timer qui :
1. Émet un nombre chaque seconde (0, 1, 2, 3...)
2. Peut être démarré
3. Peut être mis en pause
4. Peut être réinitialisé
5. S'arrête automatiquement à 10

### 💡 Solution

```typescript
import { interval, Subject, merge, NEVER } from 'rxjs';
import { scan, startWith, switchMap, mapTo } from 'rxjs/operators';

class Timer {
  private start$ = new Subject<void>();
  private pause$ = new Subject<void>();
  private reset$ = new Subject<void>();
  
  // Observable principal du timer
  timer$ = merge(
    this.start$.pipe(mapTo('START')),
    this.pause$.pipe(mapTo('PAUSE')),
    this.reset$.pipe(mapTo('RESET'))
  ).pipe(
    startWith('PAUSE'),
    switchMap(action => {
      if (action === 'START') {
        return interval(1000);
      }
      return NEVER;  // Ne rien émettre
    }),
    scan((count, _) => {
      return count + 1;
    }, 0),
    startWith(0)
  );
  
  start() {
    this.start$.next();
  }
  
  pause() {
    this.pause$.next();
  }
  
  reset() {
    this.reset$.next();
  }
}

// Utilisation
const timer = new Timer();

timer.timer$.subscribe(count => {
  console.log(`Timer: ${count}`);
  
  // Arrêter à 10
  if (count >= 10) {
    timer.pause();
    console.log('Timer terminé!');
  }
});

// Contrôles
timer.start();

setTimeout(() => timer.pause(), 3000);   // Pause après 3 sec
setTimeout(() => timer.start(), 5000);   // Reprendre après 5 sec
setTimeout(() => timer.reset(), 8000);   // Reset après 8 sec
```

### 🎨 Version Angular Component

```typescript
import { Component, OnDestroy } from '@angular/core';
import { interval, Subject, merge, NEVER, Subscription } from 'rxjs';
import { scan, startWith, switchMap, mapTo, takeUntil } from 'rxjs/operators';

@Component({
  selector: 'app-timer',
  template: `
    <div class="timer">
      <h2>Timer: {{ count }}</h2>
      <button (click)="start()" [disabled]="isRunning">Démarrer</button>
      <button (click)="pause()" [disabled]="!isRunning">Pause</button>
      <button (click)="reset()">Reset</button>
    </div>
  `,
  styles: [`
    .timer {
      text-align: center;
      padding: 20px;
    }
    h2 {
      font-size: 48px;
      margin: 20px 0;
    }
    button {
      margin: 0 5px;
      padding: 10px 20px;
      font-size: 16px;
    }
  `]
})
export class TimerComponent implements OnDestroy {
  count = 0;
  isRunning = false;
  
  private start$ = new Subject<void>();
  private pause$ = new Subject<void>();
  private reset$ = new Subject<void>();
  private destroy$ = new Subject<void>();
  
  constructor() {
    merge(
      this.start$.pipe(mapTo('START')),
      this.pause$.pipe(mapTo('PAUSE')),
      this.reset$.pipe(mapTo('RESET'))
    ).pipe(
      startWith('PAUSE'),
      switchMap(action => {
        if (action === 'START') {
          this.isRunning = true;
          return interval(1000);
        } else if (action === 'RESET') {
          this.count = 0;
          this.isRunning = false;
        } else {
          this.isRunning = false;
        }
        return NEVER;
      }),
      takeUntil(this.destroy$)
    ).subscribe(() => {
      this.count++;
      
      if (this.count >= 60) {
        this.pause();
      }
    });
  }
  
  start() {
    this.start$.next();
  }
  
  pause() {
    this.pause$.next();
  }
  
  reset() {
    this.reset$.next();
  }
  
  ngOnDestroy() {
    this.destroy$.next();
    this.destroy$.complete();
  }
}
```

---

## 📝 Questions/Réponses d'Entretien

### Q1 : Qu'est-ce que RxJS et pourquoi l'utiliser ?

**Réponse :**
> RxJS (Reactive Extensions for JavaScript) est une bibliothèque pour la programmation réactive utilisant des Observables. Elle permet de gérer facilement les flux de données asynchrones et les événements. 
> 
> **Avantages** :
> - Gestion élégante de l'asynchrone
> - Composition d'opérations complexes
> - Annulation facile (unsubscribe)
> - 100+ opérateurs pour transformer les données
> - Intégration native dans Angular

### Q2 : Quelle est la différence entre Observable et Promise ?

**Réponse :**
> **Observable** :
> - Peut émettre 0, 1 ou plusieurs valeurs
> - Lazy (ne s'exécute qu'à la souscription)
> - Annulable avec unsubscribe()
> - Nombreux opérateurs (map, filter, merge, etc.)
> 
> **Promise** :
> - Émet 1 seule valeur
> - Eager (s'exécute immédiatement)
> - Non annulable
> - Seulement then/catch
> 
> **Exemple** : Un appel HTTP est une Promise (1 réponse), mais un WebSocket est mieux géré avec un Observable (flux continu de messages).

### Q3 : Qu'est-ce qu'un Observer ?

**Réponse :**
> Un Observer est un objet avec 3 callbacks optionnels qui définit comment réagir aux émissions d'un Observable :
> - `next(value)` : appelé pour chaque valeur émise
> - `error(err)` : appelé en cas d'erreur
> - `complete()` : appelé quand le flux se termine
> 
> ```typescript
> observable$.subscribe({
>   next: v => console.log(v),
>   error: e => console.error(e),
>   complete: () => console.log('Done')
> });
> ```

### Q4 : Pourquoi est-il important de unsubscribe ?

**Réponse :**
> Ne pas unsubscribe cause des **memory leaks** car :
> - L'Observable continue d'émettre même si le composant est détruit
> - Les callbacks restent en mémoire
> - Les ressources ne sont pas libérées
> 
> **Solutions** :
> 1. Manuel avec `subscription.unsubscribe()` dans `ngOnDestroy`
> 2. AsyncPipe (Angular le fait automatiquement)
> 3. `takeUntil(destroy$)` pattern
> 
> **Exception** : Certains Observables se complètent automatiquement (HttpClient, take(n), first())

### Q5 : Qu'est-ce qu'un Subject ?

**Réponse :**
> Un Subject est à la fois un Observable ET un Observer. Il permet le **multicast** (plusieurs souscripteurs reçoivent les mêmes valeurs).
> 
> ```typescript
> const subject = new Subject<number>();
> 
> subject.subscribe(x => console.log('A:', x));
> subject.subscribe(x => console.log('B:', x));
> 
> subject.next(1);  // A et B reçoivent 1
> ```
> 
> **Types** :
> - `Subject` : basique
> - `BehaviorSubject` : garde la dernière valeur
> - `ReplaySubject` : garde N valeurs
> - `AsyncSubject` : émet seulement la dernière valeur à la complétion

### Q6 : Observable chaud vs Observable froid ?

**Réponse :**
> **Observable froid** (Cold) :
> - Crée un nouveau producteur pour chaque souscripteur
> - Chaque souscription démarre le flux depuis le début
> - Exemple : HttpClient, interval()
> 
> **Observable chaud** (Hot) :
> - Partage le même producteur entre tous les souscripteurs
> - Les nouveaux souscripteurs reçoivent seulement les valeurs futures
> - Exemple : Subjects, événements DOM
> 
> ```typescript
> // Froid
> const cold$ = new Observable(s => {
>   console.log('Producteur créé');
>   s.next(Math.random());
> });
> cold$.subscribe(console.log);  // Producteur créé, 0.234
> cold$.subscribe(console.log);  // Producteur créé, 0.876
> 
> // Chaud
> const hot$ = new Subject();
> hot$.subscribe(console.log);
> hot$.subscribe(console.log);
> hot$.next(Math.random());  // Les 2 reçoivent 0.543
> ```

### Q7 : Comment gérer les erreurs dans un Observable ?

**Réponse :**
> Plusieurs approches :
> 
> **1. Dans subscribe() :**
> ```typescript
> obs$.subscribe({
>   next: v => console.log(v),
>   error: e => console.error('Erreur:', e)
> });
> ```
> 
> **2. Avec catchError() :**
> ```typescript
> obs$.pipe(
>   catchError(err => {
>     console.error(err);
>     return of([]); // Valeur de fallback
>   })
> ).subscribe(/* ... */);
> ```
> 
> **3. Avec retry() :**
> ```typescript
> obs$.pipe(
>   retry(3),  // Réessayer 3 fois
>   catchError(err => of([]))
> ).subscribe(/* ... */);
> ```

### Q8 : Quelle est la convention de nommage pour les Observables ?

**Réponse :**
> La convention est d'ajouter un **`$`** à la fin du nom de la variable pour indiquer que c'est un Observable :
> 
> ```typescript
> const users$ = this.http.get<User[]>('/api/users');
> const clicks$ = fromEvent(button, 'click');
> const count$ = interval(1000);
> ```
> 
> **Avantages** :
> - Identifier facilement les Observables dans le code
> - Éviter la confusion avec des valeurs simples
> - Convention standard dans la communauté Angular/RxJS

### Q9 : Quand utiliser AsyncPipe vs subscribe() ?

**Réponse :**
> **AsyncPipe (Recommandé dans Angular)** :
> ```html
> <div *ngIf="users$ | async as users">
>   <div *ngFor="let user of users">{{ user.name }}</div>
> </div>
> ```
> **Avantages** :
> - Unsubscribe automatique
> - ChangeDetection optimisée
> - Code plus propre
> 
> **subscribe() (Quand nécessaire)** :
> ```typescript
> users$.subscribe(users => {
>   // Logique complexe
>   this.processUsers(users);
> });
> ```
> **Quand l'utiliser** :
> - Logique métier complexe
> - Effets de bord
> - Navigation programmatique

### Q10 : Comment déboguer un Observable ?

**Réponse :**
> **1. Opérateur tap() :**
> ```typescript
> observable$.pipe(
>   tap(value => console.log('Valeur:', value)),
>   map(x => x * 2),
>   tap(value => console.log('Après map:', value))
> ).subscribe();
> ```
> 
> **2. Console operators :**
> ```typescript
> import { tap } from 'rxjs/operators';
> 
> const debug = (tag: string) => tap(v => console.log(tag, v));
> 
> obs$.pipe(
>   debug('Avant'),
>   map(x => x * 2),
>   debug('Après')
> ).subscribe();
> ```
> 
> **3. RxJS DevTools** :
> - Extension Chrome pour visualiser les flux
> 
> **4. Marble diagrams** :
> - Dessiner le flux pour comprendre le comportement

---

## 🎯 Points Clés du Chapitre 1

### Concepts Essentiels
✅ **RxJS** : Bibliothèque pour programmation réactive
✅ **Observable** : Flux de données asynchrones
✅ **Observer** : Objet qui réagit aux émissions (next, error, complete)
✅ **Subscription** : Exécution d'un Observable, permet unsubscribe
✅ **Opérateurs** : Transforment et combinent les Observables
✅ **Subject** : Observable + Observer, permet multicast

### Différences Clés
✅ **Observable vs Promise** : Multiple vs 1 valeur, Lazy vs Eager, Annulable vs Non
✅ **Cold vs Hot** : Nouveau producteur vs Producteur partagé
✅ **AsyncPipe vs subscribe** : Auto-unsubscribe vs Manuel

### Best Practices
✅ **Toujours unsubscribe** (sauf AsyncPipe, HttpClient, take, first)
✅ **Utiliser AsyncPipe** dans Angular quand possible
✅ **Convention $** pour nommer les Observables
✅ **takeUntil pattern** pour gérer multiple subscriptions

---