# 🧪 Angular Unit Testing — Cours Complet pour l'Entretien

> Basé sur la série YouTube *"Angular Unit Testing Made Easy"* et le code source du projet.

---

## Table des matières

1. [Introduction aux Unit Tests](#1-introduction-aux-unit-tests)
2. [Spy, spyOn et createSpyObj](#2-spy-spyon-et-createspyobj)
3. [BeforeEach, TestBed et Dependency Injection](#3-beforeeach-testbed-et-dependency-injection)
4. [HTTP Testing](#4-http-testing)
5. [Component Testing](#5-component-testing)
6. [DOM Interaction](#6-dom-interaction)
7. [Opérations Asynchrones](#7-opérations-asynchrones)
8. [Promise et Observable](#8-promise-et-observable)
9. [Pipes](#9-pipes)
10. [Directives](#10-directives)
11. [Routing](#11-routing)
12. [Code Coverage](#12-code-coverage)

---

## 1. Introduction aux Unit Tests

### Qu'est-ce qu'un Unit Test ?

Un **unit test** (test unitaire) consiste à tester une unité isolée de code — une fonction, une méthode, un composant — sans dépendre des autres parties du système. L'objectif est de s'assurer que chaque pièce fonctionne correctement de manière indépendante.

### Outils utilisés dans Angular

| Outil | Rôle |
|-------|------|
| **Jasmine** | Framework de tests (syntaxe `describe`, `it`, `expect`) |
| **Karma** | Runner de tests — exécute les tests dans un vrai navigateur |
| **TestBed** | Utilitaire Angular pour créer un environnement de test isolé |

### Structure d'un fichier de test

Tout fichier de test Angular porte l'extension `.spec.ts`. Jasmine structure les tests avec trois éléments principaux :

- **`describe`** : définit un groupe de tests (une *suite*).
- **`it`** : définit un cas de test individuel (une *spec*).
- **`expect`** : définit une assertion à vérifier.

### Premier exemple : tester un service simple

**`calc.service.ts`** — le service à tester :

```typescript
import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root',
})
export class CalcService {
  multiply(a: number, b: number): number {
    return a * b;
  }
}
```

**`calc.service.spec.ts`** — le fichier de test :

```typescript
import { CalcService } from './calc.service';

describe('CalcService', () => {

  it('should multiply two numbers', () => {
    const calcService = new CalcService();
    const result = calcService.multiply(3, 5);
    expect(result).toBe(15);
  });

});
```

> 💡 **Anatomie d'un test :**
> - `describe('CalcService', ...)` → le groupe de tests pour ce service.
> - `it('should multiply...', ...)` → un cas de test spécifique.
> - `expect(result).toBe(15)` → on vérifie que le résultat vaut 15.

### Lancer les tests

```bash
ng test
```

Karma ouvre un navigateur, exécute tous les fichiers `*.spec.ts` et affiche les résultats en temps réel.

### Matchers Jasmine courants

```typescript
expect(value).toBe(15);             // égalité stricte (===)
expect(value).toEqual({a: 1});      // égalité profonde (objets)
expect(value).toBeTruthy();         // valeur vraie (non null, non 0, etc.)
expect(value).toBeFalsy();          // valeur fausse
expect(value).toBeTrue();           // strictement true
expect(value).toBeFalse();          // strictement false
expect(value).toBeNull();           // strictement null
expect(value).toContain('texte');   // tableau ou chaîne contient
expect(fn).toHaveBeenCalled();      // une fonction (spy) a été appelée
expect(fn).toHaveBeenCalledWith(x); // appelée avec un argument précis
```

---

## 2. Spy, spyOn et createSpyObj

### Pourquoi utiliser des Spies ?

Un test unitaire doit être **isolé**. Si `CalcService` dépend de `SharedService`, on ne veut pas que `SharedService` effectue de vraies opérations (appels HTTP, logs, etc.) pendant les tests. On utilise donc des **spies** — des fonctions fictives qui remplacent les vraies.

### `spyOn` — espionner une méthode existante

`spyOn(objet, 'nomDeLaMethode')` remplace la méthode par une version fictive qui :
- **ne fait rien** par défaut (elle ne déclenche pas l'implémentation réelle).
- **enregistre** si elle a été appelée, combien de fois, et avec quels arguments.

**Contexte :** `CalcService` appelle `sharedService.mySharedFunction()` dans sa méthode `multiply` :

```typescript
// calc.service.ts
export class CalcService {
  constructor(private sharedService: SharedService) {}

  multiply(a: number, b: number): number {
    this.sharedService.mySharedFunction(); // appel à un service externe
    return a * b;
  }
}
```

**Test avec `spyOn` :**

```typescript
it('should call mySharedFunction func', () => {
  const shared = new SharedService(); // instance réelle
  const calc = new CalcService(shared);

  spyOn(shared, 'mySharedFunction');
  // ↑ Remplace mySharedFunction par une fonction vide.
  // La vraie implémentation (console.log, appel HTTP...) ne s'exécute PAS.

  const result = calc.multiply(3, 5);

  expect(shared.mySharedFunction).toHaveBeenCalled();
  // ↑ Vérifie que multiply() a bien appelé mySharedFunction()
});
```

### `.and.callThrough()` — espionner sans bloquer

Si on veut que le spy **enregistre l'appel ET laisse la vraie méthode s'exécuter** :

```typescript
spyOn(shared, 'mySharedFunction').and.callThrough();
// Le spy surveille la méthode, mais l'implémentation réelle tourne quand même.
```

### `jasmine.createSpyObj` — créer un faux objet entier

`spyOn` nécessite une instance réelle en entrée. Mais créer une vraie instance peut avoir des effets de bord (par exemple, le constructeur de `SharedService` simule un appel HTTP) :

```typescript
// shared.service.ts
export class SharedService {
  constructor() {
    console.log("A HTTP Call"); // effet de bord indésirable dans les tests !
  }
  mySharedFunction() { ... }
}
```

La solution : **`jasmine.createSpyObj`** crée un objet fictif complet, sans jamais instancier la vraie classe.

```typescript
it('should call mySharedFunction func', () => {
  // Crée un faux objet avec une fausse méthode "mySharedFunction"
  // Le constructeur de SharedService n'est JAMAIS appelé.
  let shared = jasmine.createSpyObj('SharedService', ['mySharedFunction']);

  const calc = new CalcService(shared);
  const result = calc.multiply(3, 5);

  expect(shared.mySharedFunction).toHaveBeenCalled();
});
```

### Tableau comparatif

| | `spyOn` | `jasmine.createSpyObj` |
|---|---|---|
| Nécessite une vraie instance | ✅ Oui | ❌ Non |
| Exécute le constructeur réel | ✅ Oui | ❌ Non |
| Remplace une méthode | Une seule | Plusieurs à la fois |
| Usage typique | Objet déjà instancié | Faux service sans effet de bord |

---

## 3. BeforeEach, TestBed et Dependency Injection

### Le problème de l'instanciation manuelle

Dans les exemples précédents, on instanciait les services manuellement avec `new`. Cela ne reflète pas la réalité d'Angular qui utilise l'**Injection de Dépendances (DI)**. Il faut utiliser `TestBed` pour simuler le système DI d'Angular.

### `TestBed` — le module de test Angular

`TestBed` simule un module Angular temporaire pour les tests. On le configure avec `TestBed.configureTestingModule(...)`, qui accepte les mêmes options qu'un vrai `@NgModule` :

```typescript
TestBed.configureTestingModule({
  imports: [],       // modules à importer
  providers: [],     // services à fournir
  declarations: []   // composants / pipes / directives (pour les modules non-standalone)
});
```

### `beforeEach` — initialisation avant chaque test

`beforeEach` exécute une fonction **avant chaque test** dans un `describe`. C'est l'endroit idéal pour configurer `TestBed` et instancier les services.

### `TestBed.inject` — récupérer une instance du service

```typescript
const service = TestBed.inject(MonService);
// Angular résout les dépendances automatiquement grâce au DI.
```

### Exemple complet : instance réelle des services (commit 3.1)

```typescript
import { TestBed } from '@angular/core/testing';
import { CalcService } from './calc.service';
import { SharedService } from '../shared/shared.service';

describe('CalcService', () => {
  let sharedService: SharedService;
  let calcService: CalcService;

  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [CalcService, SharedService], // les deux services sont fournis
    });

    sharedService = TestBed.inject(SharedService); // Angular instancie SharedService
    calcService = TestBed.inject(CalcService);     // Angular instancie CalcService avec SharedService injecté
  });

  it('should multiply two numbers', () => {
    const result = calcService.multiply(3, 5);
    expect(result).toBe(15);
  });

  it('should add two numbers', () => {
    const result = calcService.add(3, 5);
    expect(result).toBe(8);
  });
});
```

> ⚠️ Ici on utilise de vraies instances. Le constructeur de `SharedService` s'exécute, donc le `console.log("A HTTP Call")` apparaîtra dans la console. C'est pour ça qu'on préfère injecter de faux objets.

### Injecter un faux service (commit 3.2)

On peut fournir un **faux objet** à la place d'un service réel grâce au mécanisme `{ provide: ..., useValue: ... }` :

```typescript
beforeEach(() => {
  // Créer le faux service AVANT de configurer TestBed
  sharedService = jasmine.createSpyObj('sharedService', ['mySharedFunction']);

  TestBed.configureTestingModule({
    providers: [
      CalcService,
      { provide: SharedService, useValue: sharedService }
      // ↑ "Quand quelqu'un demande SharedService, donne cet objet fictif"
    ],
  });

  calcService = TestBed.inject(CalcService);
  // CalcService reçoit le faux SharedService, pas le vrai.
});
```

> 💡 **Pourquoi `{ provide, useValue }` ?** Parce qu'Angular utilise un token (le type `SharedService`) pour résoudre les dépendances. En utilisant `useValue`, on dit à Angular : "pour ce token, utilise cet objet-là au lieu de créer une vraie instance".

### `xdescribe`, `fdescribe`, `xit`, `fit` — focus et exclusion (commit 3.3)

Ces prefixes permettent de contrôler quels tests s'exécutent :

| Préfixe | Effet |
|---------|-------|
| `xdescribe` | Ignore tout le groupe de tests |
| `xit` | Ignore ce test individuel |
| `fdescribe` | Exécute **uniquement** ce groupe (focus) |
| `fit` | Exécute **uniquement** ce test (focus) |

```typescript
// xdescribe('CalcService', () => { ... }); // tout le groupe est ignoré
// fdescribe('CalcService', () => { ... }); // seul ce groupe s'exécute

describe('CalcService', () => {
  // xit('should multiply', () => { ... }); // ce test est ignoré
  // fit('should multiply', () => { ... }); // seul ce test s'exécute
  it('should multiply', () => { ... });
});
```

> ⚠️ Ne jamais laisser `fdescribe` ou `fit` en production — d'autres tests ne s'exécuteront plus.

### `afterEach` et `afterAll`

En complément de `beforeEach`, Jasmine propose :

```typescript
afterEach(() => {
  // Exécuté après chaque test — idéal pour nettoyer des ressources
});

beforeAll(() => {
  // Exécuté une seule fois avant tous les tests du describe
});

afterAll(() => {
  // Exécuté une seule fois après tous les tests du describe
});
```

---

## 4. HTTP Testing

### Le problème des vrais appels HTTP

Dans les tests, on ne veut **jamais** faire de vrais appels réseau. Ils seraient lents, non déterministes, et dépendraient d'un serveur externe. Angular fournit `HttpClientTestingModule` et `HttpTestingController` pour intercepter et simuler les requêtes HTTP.

### Le service à tester

```typescript
// data.service.ts
import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';

@Injectable({ providedIn: 'root' })
export class DataService {

  constructor(private http: HttpClient) {}

  getAllUsers() {
    return this.http.get('api/users');
  }

  getUserById(id: number) {
    return this.http.get(`api/users/${id}`);
  }

  updateUser(id: number, updatedUser: any) {
    return this.http.put(`api/users/${id}`, updatedUser);
  }
}
```

### Données mockées

```typescript
// mock-data-users.ts
export const USERS = {
  1: { id: 1, name: 'John', age: 25, gender: 'Male' },
  2: { id: 2, name: 'Elena', age: 31, gender: 'Female' },
  // ...
};
```

### Structure du fichier de test HTTP

```typescript
import { TestBed } from '@angular/core/testing';
import { DataService } from './data.service';
import { HttpClientTestingModule, HttpTestingController } from '@angular/common/http/testing';
import { USERS } from './mock-data-users';

describe('DataService', () => {
  let service: DataService;
  let testingController: HttpTestingController;

  beforeEach(() => {
    TestBed.configureTestingModule({
      imports: [HttpClientTestingModule], // remplace HttpClientModule par une version fictive
    });

    service = TestBed.inject(DataService);
    testingController = TestBed.inject(HttpTestingController);
  });

  afterEach(() => {
    testingController.verify();
    // ↑ Vérifie qu'il n'y a pas de requêtes HTTP non gérées à la fin du test.
    // Lance une erreur si une requête a été faite sans être "flushée".
  });

  // ... les tests ci-dessous
});
```

### Test d'une requête GET (liste)

```typescript
it('should get all users', () => {
  // 1. Appeler la méthode — elle déclenche un GET mais il est intercepté
  service.getAllUsers().subscribe((users: any) => {
    // 4. Ces assertions s'exécutent APRÈS le flush()
    expect(users).toBeTruthy();
    expect(users.length).toBe(6);
    const secondUser = users.find((user: any) => user.id === 2);
    expect(secondUser.name).toBe('Elena');
  });

  // 2. Capturer la requête interceptée
  const mockReq = testingController.expectOne('api/users');

  // 3. Vérifier la méthode HTTP
  expect(mockReq.request.method).toEqual('GET');

  // 4. Simuler la réponse du serveur → déclenche le subscribe()
  mockReq.flush(Object.values(USERS));
});
```

> 💡 **Flux d'exécution complet :**
>
> ```
> service.getAllUsers()
>         ↓
> HttpClient envoie une requête GET (interceptée)
>         ↓
> HttpTestingController la retient
>         ↓
> expectOne('api/users') la capture dans mockReq
>         ↓
> flush(data) simule la réponse du serveur
>         ↓
> subscribe() reçoit les données → les assertions s'exécutent
> ```

### Test d'une requête GET (par ID)

```typescript
it('should get user by id', () => {
  service.getUserById(1).subscribe((user: any) => {
    expect(user).toBeTruthy();
    expect(user.name).toBe('John');
  });

  const mockReq = testingController.expectOne('api/users/1');
  expect(mockReq.request.method).toEqual('GET');
  mockReq.flush(USERS[1]);
});
```

### Test d'une requête PUT (mise à jour)

```typescript
it('should update user by id', () => {
  const changes = { age: 24 };

  service.updateUser(1, changes).subscribe((user: any) => {
    expect(user).toBeTruthy();
    expect(user.id).toBe(1);
  });

  const mockReq = testingController.expectOne('api/users/1');
  expect(mockReq.request.method).toEqual('PUT');

  // Vérifier le body de la requête
  expect(mockReq.request.body.age).toEqual(changes.age);

  // Simuler la réponse
  const modifiedUser = { ...USERS[1], age: 24 };
  mockReq.flush(modifiedUser);
});
```

### Méthodes clés de `HttpTestingController`

| Méthode | Description |
|---------|-------------|
| `expectOne(url)` | Capture exactement une requête vers cette URL |
| `expectNone(url)` | Vérifie qu'aucune requête n'a été faite vers cette URL |
| `match(url)` | Capture toutes les requêtes correspondant à l'URL |
| `verify()` | Vérifie qu'il n'y a pas de requêtes non gérées |
| `flush(data)` | Simule une réponse réussie |
| `error(error)` | Simule une erreur de réponse |

---

## 5. Component Testing

### Les outils du Component Testing

Pour tester un composant Angular, on utilise :

- **`ComponentFixture<T>`** : un wrapper autour du composant qui donne accès à son instance, son DOM, et permet de déclencher la détection de changements.
- **`fixture.componentInstance`** : l'instance TypeScript du composant.
- **`fixture.debugElement`** : accès au DOM du composant pour faire des requêtes et déclencher des événements.
- **`fixture.detectChanges()`** : déclenche manuellement la détection de changements (comme si Angular faisait un cycle de rendu).

### Exemple : `HomeComponent`

**`home.component.html` :**
```html
<p>home works!</p>
<button class="btn" disabled>Submit</button>
<div class="title">{{title}}</div>
```

**`home.component.ts` :**
```typescript
@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
})
export class HomeComponent {
  title = 'Home Page';
}
```

**`home.component.spec.ts` :**

```typescript
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { HomeComponent } from './home.component';
import { DebugElement } from '@angular/core';
import { By } from '@angular/platform-browser';

describe('HomeComponent', () => {
  let component: HomeComponent;
  let fixture: ComponentFixture<HomeComponent>;
  let el: DebugElement;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [HomeComponent], // standalone component → on l'importe
    }).compileComponents(); // compile le template et les styles

    fixture = TestBed.createComponent(HomeComponent);
    component = fixture.componentInstance;
    el = fixture.debugElement;

    fixture.detectChanges(); // déclenche ngOnInit et le premier rendu
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });

  it('should have correct content', () => {
    let pEl = el.queryAll(By.css('p'));
    expect(pEl[0].nativeElement.textContent).toBe('home works!');

    let btnEl = el.queryAll(By.css('.btn'));
    expect(btnEl[0].nativeElement.disabled).toBeTrue();
  });
});
```

### `compileComponents()` — pourquoi l'utiliser ?

`compileComponents()` compile les templates et styles des composants de manière asynchrone. Il est nécessaire quand le composant utilise des fichiers externes (`templateUrl`, `styleUrl`). Sans lui, Angular ne peut pas lire le HTML et le CSS.

Il retourne une `Promise`, d'où l'utilisation de `async/await`.

### `By.css` vs `nativeElement.querySelector`

| Approche | Description |
|----------|-------------|
| `el.query(By.css('.btn'))` | Retourne un `DebugElement` (API Angular) |
| `el.queryAll(By.css('.btn'))` | Retourne un tableau de `DebugElement` |
| `el.nativeElement.querySelector('.btn')` | Accès direct à l'élément DOM natif |

`By.css` est préféré car il est compatible avec tous les environnements (navigateur, server-side rendering).

### Composants standalone vs déclarés dans un module

```typescript
// Composant STANDALONE (Angular 14+)
TestBed.configureTestingModule({
  imports: [MonComposantStandalone]
});

// Composant classique (dans un NgModule)
TestBed.configureTestingModule({
  declarations: [MonComposant],
  imports: [BrowserModule, AutreModule]
});
```

---

## 6. DOM Interaction

### Tester les interactions utilisateur avec le DOM

Le `DebugElement` permet d'interagir avec le DOM dans les tests. On peut simuler des clics, vérifier du texte, des états de boutons, etc.

### Exemple complet : `AppComponent`

**`app.component.html` :**
```html
<button class="subscribe" *ngIf="!isSubscribed" (click)="subscribe()">Subscribe</button>
<button class="subscribe" *ngIf="isSubscribed" disabled>Subscribed</button>
```

**`app.component.ts` :**
```typescript
export class AppComponent {
  isSubscribed = false;

  subscribe() {
    this.isSubscribed = true;
  }
}
```

**`app.component.spec.ts` :**

```typescript
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { AppComponent } from './app.component';
import { DebugElement } from '@angular/core';
import { By } from '@angular/platform-browser';

describe('AppComponent', () => {
  let component: AppComponent;
  let fixture: ComponentFixture<AppComponent>;
  let el: DebugElement;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [AppComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(AppComponent);
    component = fixture.componentInstance;
    el = fixture.debugElement;

    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });

  it('should render a button with text subscribe and disable it after click', () => {
    // État initial
    component.isSubscribed = false;
    fixture.detectChanges(); // re-rendre le template

    let btnElements = el.queryAll(By.css('.subscribe'));

    // Vérifier l'état initial
    expect(btnElements[0].nativeElement.textContent).toBe('Subscribe');
    expect(btnElements[0].nativeElement.disabled).toBeFalse();

    // Simuler un clic
    btnElements[0].nativeElement.click();
    fixture.detectChanges(); // Angular doit redessiner le DOM après le clic

    // Après le clic, *ngIf bascule → nouveau DOM
    btnElements = el.queryAll(By.css('.subscribe'));
    expect(btnElements[0].nativeElement.textContent).toBe('Subscribed');
    expect(btnElements[0].nativeElement.disabled).toBeTrue();
  });
});
```

### `fixture.detectChanges()` — quand l'appeler ?

`detectChanges()` déclenche le cycle de détection de changements d'Angular. Il faut l'appeler :

1. **Après l'initialisation** du composant (`beforeEach`) → déclenche `ngOnInit`.
2. **Après chaque modification** de propriété du composant → met à jour le DOM.
3. **Après un clic** ou tout autre événement → re-rendre le template.

> ⚠️ Sans `detectChanges()`, Angular ne met pas à jour le DOM automatiquement dans les tests. C'est un comportement volontaire pour garder les tests déterministes.

### `triggerEventHandler` — déclencher des événements Angular

```typescript
// Méthode 1 : Clic natif
element.nativeElement.click();

// Méthode 2 : Via DebugElement (recommandé pour les événements Angular)
element.triggerEventHandler('click', {});
element.triggerEventHandler('mouseenter', {});
element.triggerEventHandler('mouseleave', null);
```

`triggerEventHandler` est préféré car il déclenche les event bindings Angular (`(click)`, `(mouseenter)`, etc.) directement, sans passer par le DOM natif.

---

## 7. Opérations Asynchrones

### Le problème du code asynchrone dans les tests

Les tests Jasmine sont par défaut **synchrones**. Si on utilise `setTimeout`, `Promise`, ou des observables asynchrones, le test se termine avant que le code asynchrone s'exécute. Résultat : les assertions ne sont jamais vérifiées, ou le test réussit à tort.

### `fakeAsync` et `tick` — contrôler le temps (commit 7.1)

**`fakeAsync`** crée une zone fictive dans laquelle Angular remplace les vrais timers (`setTimeout`, `setInterval`) par une **horloge virtuelle** que l'on contrôle manuellement.

**`tick(ms)`** fait avancer cette horloge virtuelle du nombre de millisecondes indiqué.

```typescript
import { fakeAsync, tick } from '@angular/core/testing';

it('should test async with tick', fakeAsync(() => {
  component.isSubscribed = false;
  fixture.detectChanges();

  let btnElements = el.queryAll(By.css('.subscribe'));

  // Simuler le clic
  btnElements[0].nativeElement.click();

  // Du code asynchrone qui s'exécutera après 3 secondes
  setTimeout(() => {
    fixture.detectChanges();
    btnElements = el.queryAll(By.css('.subscribe'));
  }, 3000);

  // Faire avancer l'horloge virtuelle de 3000ms
  tick(3000);
  fixture.detectChanges();

  expect(btnElements[0].nativeElement.textContent).toBe('Subscribed');
  expect(btnElements[0].nativeElement.disabled).toBeTrue();

  // Il faut "consommer" tous les timers restants à la fin du fakeAsync
  tick(5000); // consomme le setTimeout(() => {}, 8000) restant
}));
```

### `flush` — consommer tous les timers d'un coup (commit 7.2)

Quand on ne connaît pas exactement la durée des timers, ou qu'il y en a plusieurs, on utilise **`flush()`** qui exécute **tous les timers en attente** jusqu'à ce qu'il n'en reste plus.

```typescript
import { fakeAsync, flush } from '@angular/core/testing';

it('should test async with flush', fakeAsync(() => {
  // ...
  setTimeout(() => {
    fixture.detectChanges();
    btnElements = el.queryAll(By.css('.subscribe'));
  }, 3000);

  flush(); // exécute tous les setTimeout en attente, quelle que soit leur durée
  fixture.detectChanges();

  expect(btnElements[0].nativeElement.textContent).toBe('Subscribed');
}));
```

### `flushMicrotasks` — pour les Promises

Les **microtasks** (Promises, `queueMicrotask`) sont distinctes des macrotasks (setTimeout). `flushMicrotasks()` résout uniquement les Promises en attente, sans avancer l'horloge pour les `setTimeout`.

```typescript
import { fakeAsync, tick, flushMicrotasks } from '@angular/core/testing';

it('should handle mixed async operations', fakeAsync(() => {
  let counter = 0;

  setTimeout(() => { counter += 2; }, 2000); // macrotask
  setTimeout(() => { counter += 3; }, 3000); // macrotask

  Promise.resolve().then(() => { counter += 1; }); // microtask

  flushMicrotasks(); // résout uniquement la Promise
  expect(counter).toBe(1); // seule la Promise s'est exécutée

  tick(2000); // avance l'horloge de 2s
  expect(counter).toBe(3); // 1 + 2

  tick(3000); // avance l'horloge de 3s supplémentaires
  expect(counter).toBe(6); // 3 + 3
}));
```

### Tableau récapitulatif des utilitaires asynchrones

| Utilitaire | Quand l'utiliser |
|------------|-----------------|
| `fakeAsync` | Wrapper pour tout test asynchrone avec timers ou Promises |
| `tick(ms)` | Avancer l'horloge d'une durée précise |
| `flush()` | Consommer tous les macrotasks en attente |
| `flushMicrotasks()` | Résoudre toutes les Promises en attente |
| `waitForAsync` | Pour les tests avec `compileComponents()` ou des opérations réellement asynchrones |

---

## 8. Promise et Observable

### Tester une Observable synchrone

Certaines observables sont **synchrones** (elles émettent immédiatement leurs valeurs sans délai). Dans ce cas, un test normal suffit :

```typescript
import { of } from 'rxjs';

it('should test a synchronous observable', () => {
  let result = false;
  const myObs = of(true); // émet immédiatement true

  myObs.subscribe((value) => {
    result = value;
  });

  // Pas besoin de fakeAsync, le subscribe s'est exécuté de manière synchrone
  expect(result).toBeTrue();
});
```

### Tester une Observable asynchrone avec `delay`

Quand une observable utilise `delay`, elle devient asynchrone. Il faut `fakeAsync` et `tick` :

```typescript
import { of, delay } from 'rxjs';
import { fakeAsync, tick } from '@angular/core/testing';

it('should test an asynchronous observable', fakeAsync(() => {
  let result = false;
  const myObs = of(true).pipe(delay(1000)); // émet après 1 seconde

  myObs.subscribe((value) => {
    result = value;
  });

  expect(result).toBeFalse(); // pas encore émis

  tick(1000); // avancer l'horloge de 1s

  expect(result).toBeTrue(); // maintenant émis
}));
```

### Tester une Promise

```typescript
import { fakeAsync, flushMicrotasks } from '@angular/core/testing';

it('should test a promise', fakeAsync(() => {
  let resolved = false;

  Promise.resolve(true).then((value) => {
    resolved = value;
  });

  expect(resolved).toBeFalse(); // pas encore résolu

  flushMicrotasks(); // résoudre la Promise

  expect(resolved).toBeTrue();
}));
```

### `waitForAsync` — alternative à `fakeAsync`

`waitForAsync` est utilisé quand `fakeAsync` ne peut pas être utilisé (ex. : code réellement asynchrone avec `XMLHttpRequest`). Il indique à Jasmine d'attendre que toutes les tâches asynchrones soient terminées :

```typescript
import { waitForAsync } from '@angular/core/testing';

it('should wait for async operations', waitForAsync(() => {
  fixture.whenStable().then(() => {
    // Code exécuté quand toutes les opérations asynchrones sont terminées
    expect(component).toBeTruthy();
  });
}));
```

---

## 9. Pipes

### Qu'est-ce qu'un Pipe ?

Un **pipe** transforme des données dans les templates Angular. Par exemple `{{ mark | grade }}` applique le pipe `grade` à la valeur `mark`.

### Le Pipe à tester

```typescript
// grade.pipe.ts
import { Pipe, PipeTransform } from '@angular/core';

@Pipe({ name: 'grade' })
export class GradePipe implements PipeTransform {
  transform(value: number): string {
    if (value >= 90) return 'A';
    if (value >= 80) return 'B';
    if (value >= 70) return 'C';
    if (value >= 60) return 'D';
    if (value >= 35) return 'E';
    return 'FAIL';
  }
}
```

### Tester un Pipe

Tester un pipe est très simple : un pipe est une **classe pure** avec une méthode `transform`. Il n'y a pas besoin de `TestBed` !

```typescript
import { GradePipe } from './grade.pipe';

describe('GradePipe', () => {
  let pipe: GradePipe;

  beforeEach(() => {
    pipe = new GradePipe(); // instanciation directe, pas besoin de TestBed
  });

  it('should create an instance', () => {
    expect(pipe).toBeTruthy();
  });

  it('should assign A grade when mark is greater than 89', () => {
    const grade = pipe.transform(93);
    expect(grade).toBe('A');
  });

  it('should assign B grade when mark is between 80 and 89', () => {
    const grade = pipe.transform(84);
    expect(grade).toBe('B');
  });

  it('should assign FAIL when mark is below 35', () => {
    const grade = pipe.transform(20);
    expect(grade).toBe('FAIL');
  });
});
```

> 💡 **Bonne pratique :** couvrir chaque branche de la logique `if/else` pour atteindre une bonne couverture de code.

---

## 10. Directives

### Qu'est-ce qu'une Directive ?

Une **directive** modifie le comportement ou l'apparence d'un élément du DOM. Contrairement aux pipes, les directives interagissent avec le DOM et nécessitent donc un composant hôte pour être testées.

### La Directive à tester

```typescript
// grade.directive.ts
import { Directive, ElementRef, HostListener, Input } from '@angular/core';

@Directive({ selector: '[appGrade]' })
export class GradeDirective {

  @Input() mark = 0;

  constructor(private el: ElementRef) {}

  @HostListener('mouseenter') onMouseEnter() {
    if (this.mark >= 90) {
      this.el.nativeElement.style.color = 'green';
    } else if (this.mark >= 35) {
      this.el.nativeElement.style.color = 'blue';
    } else {
      this.el.nativeElement.style.color = 'red';
    }
  }

  @HostListener('mouseleave') onMouseLeave() {
    this.el.nativeElement.style.color = 'black';
  }
}
```

**Template utilisant la directive :**
```html
<!-- app.component.html -->
<div *ngFor="let mark of marks" [mark]="mark" appGrade>{{ mark | grade }}</div>
```

### Tester une Directive

Pour tester une directive, il faut un **composant hôte** qui l'utilise. On utilise ici `AppComponent` qui contient des éléments avec `appGrade`.

```typescript
import { GradeDirective } from './grade.directive';
import { AppComponent } from '../app.component';
import { ComponentFixture, TestBed, waitForAsync } from '@angular/core/testing';
import { DebugElement } from '@angular/core';
import { By } from '@angular/platform-browser';

describe('GradeDirective', () => {
  let component: AppComponent;
  let fixture: ComponentFixture<AppComponent>;
  let el: DebugElement;

  beforeEach(waitForAsync(() => {
    TestBed.configureTestingModule({
      imports: [AppComponent] // AppComponent importe GradeDirective
    });

    fixture = TestBed.createComponent(AppComponent);
    component = fixture.componentInstance;
    el = fixture.debugElement;
    fixture.detectChanges();
  }));

  it('should create an instance', () => {
    // Test minimal : instancier la directive directement
    const mockElRef = { nativeElement: document.createElement('div') };
    const directive = new GradeDirective(mockElRef);
    expect(directive).toBeTruthy();
  });

  it('should change text color on mouse enter', () => {
    const divs = el.queryAll(By.css('div'));

    // marks = [97, 68, 83, 29, 75]
    // div0 → mark=97 (>= 90) → vert
    divs[0].triggerEventHandler('mouseenter', {});
    fixture.detectChanges();
    expect(divs[0].nativeElement.style.color).toBe('green');

    // div1 → mark=68 (>= 35 et < 90) → bleu
    divs[1].triggerEventHandler('mouseenter', {});
    fixture.detectChanges();
    expect(divs[1].nativeElement.style.color).toBe('blue');

    // div3 → mark=29 (< 35) → rouge
    divs[3].triggerEventHandler('mouseenter', {});
    fixture.detectChanges();
    expect(divs[3].nativeElement.style.color).toBe('red');
  });
});
```

> 💡 **Pourquoi utiliser `waitForAsync` ici ?** Parce que `AppComponent` est un composant standalone qui importe d'autres modules. La compilation peut être asynchrone.

---

## 11. Routing

### Tester le routing Angular

Pour tester le routing, Angular fournit `RouterTestingModule` qui simule le Router sans naviguer réellement dans un vrai navigateur.

### Configuration des routes

```typescript
// app.routes.ts
import { Routes } from '@angular/router';
import { HomeComponent } from './home/home.component';
import { InfoComponent } from './info/info.component';

export const routes: Routes = [
  { path: '', redirectTo: 'home', pathMatch: 'full' },
  { path: 'home', component: HomeComponent },
  { path: 'info', component: InfoComponent },
];
```

### Le fichier de test de routing

```typescript
import { Router } from '@angular/router';
import { ComponentFixture, TestBed, waitForAsync } from '@angular/core/testing';
import { RouterTestingModule } from '@angular/router/testing';
import { routes } from './app.routes';
import { AppComponent } from './app.component';
import { HomeComponent } from './home/home.component';
import { InfoComponent } from './info/info.component';
import { DebugElement } from '@angular/core';
import { By } from '@angular/platform-browser';

describe('App Routing', () => {
  let router: Router;
  let fixture: ComponentFixture<AppComponent>;
  let location: Location;
  let homeFixture: ComponentFixture<HomeComponent>;
  let infoFixture: ComponentFixture<InfoComponent>;
  let el: DebugElement;
  let btnEl: DebugElement;

  beforeEach(waitForAsync(() => {
    TestBed.configureTestingModule({
      imports: [
        RouterTestingModule.withRoutes(routes) // simuler le Router avec nos routes
      ],
      declarations: [AppComponent, HomeComponent, InfoComponent]
    }).compileComponents();
  }));

  beforeEach(() => {
    router = TestBed.inject(Router);
    location = TestBed.inject(Location);

    router.initialNavigation(); // déclenche la navigation initiale

    fixture = TestBed.createComponent(AppComponent);
    homeFixture = TestBed.createComponent(HomeComponent);
    infoFixture = TestBed.createComponent(InfoComponent);

    el = homeFixture.debugElement;
    btnEl = infoFixture.debugElement;
  });

  // Test 1 : la route par défaut redirige vers /home
  it('should navigate to the default path = home', waitForAsync(() => {
    fixture.detectChanges();
    fixture.whenStable().then(() => {
      expect(location.pathname).toBe('/home');
    });
  }));

  // Test 2 : cliquer sur un lien dans HomeComponent navigue vers /info
  it('should navigate to info on clicking the link in home component', waitForAsync(() => {
    homeFixture.detectChanges();
    const links = el.queryAll(By.css('a'));
    links[0].nativeElement.click(); // clic sur <a routerLink="/info">
    homeFixture.whenStable().then(() => {
      expect(location.pathname).toBe('/info');
    });
  }));

  // Test 3 : cliquer sur "Back" dans InfoComponent navigue vers /home
  it('should navigate to home on clicking the back button in info component', waitForAsync(() => {
    infoFixture.detectChanges();
    const btns = el.queryAll(By.css('button'));
    btns[0].nativeElement.click(); // appelle goBack() → router.navigate(['/'])
    infoFixture.whenStable().then(() => {
      expect(location.pathname).toBe('/home');
    });
  }));
});
```

### Concepts clés du test de routing

**`RouterTestingModule.withRoutes(routes)`** — configure un faux Router avec nos routes pour les tests.

**`router.initialNavigation()`** — déclenche la navigation initiale (équivalent au premier chargement de la page). Sans cet appel, la route par défaut ne s'applique pas.

**`fixture.whenStable()`** — retourne une Promise qui se résout quand toutes les opérations asynchrones (dont la navigation) sont terminées. La navigation n'est pas instantanée, c'est pour ça qu'on l'utilise.

**`Location`** — service Angular qui donne accès à l'URL actuelle du navigateur. Dans les tests, `location.pathname` retourne le chemin de la route active.

### `RouterTestingModule` vs `RouterTestingModule.withRoutes`

```typescript
// Sans routes — utile pour les composants qui utilisent routerLink
// mais dont on ne teste pas la navigation
RouterTestingModule

// Avec routes — pour tester la logique de navigation
RouterTestingModule.withRoutes(routes)
```

> ⚠️ Si un composant utilise `routerLink` ou `RouterOutlet`, son spec file doit importer `RouterTestingModule`, sinon Angular lancera une erreur de compilation.

---

## 12. Code Coverage

### Qu'est-ce que la couverture de code ?

La **couverture de code** (code coverage) mesure quelle proportion du code source est exécutée lors des tests. Un rapport de couverture indique les fichiers et les lignes qui ne sont pas couverts par des tests.

### Générer le rapport de couverture

```bash
ng test --no-watch --code-coverage
```

- `--no-watch` : exécute les tests une seule fois (sans rester en mode watch).
- `--code-coverage` : génère le rapport de couverture dans le dossier `coverage/`.

### Exemple de sortie

```
=============================== Coverage summary ===============================
Statements   : 92.72% ( 51/55 )
Branches     : 96%    ( 24/25 )
Functions    : 80%    ( 12/15 )
Lines        : 92.15% ( 47/51 )
================================================================================
```

### Les 4 types de couverture

| Type | Description |
|------|-------------|
| **Statements** | Pourcentage de déclarations exécutées |
| **Branches** | Pourcentage de branches (if/else, switch) exécutées |
| **Functions** | Pourcentage de fonctions appelées |
| **Lines** | Pourcentage de lignes de code exécutées |

### Le dossier `coverage/`

Angular génère un rapport HTML interactif dans `coverage/` :

```
coverage/
  index.html       ← rapport global
  src/
    app/
      calc.service.ts.html  ← rapport par fichier
      ...
```

Ouvrir `coverage/index.html` dans un navigateur pour voir les lignes non couvertes surlignées en rouge.

### Configurer le seuil minimum de couverture

Dans `angular.json`, on peut définir un seuil de couverture minimum. Si ce seuil n'est pas atteint, le build échoue :

```json
"test": {
  "options": {
    "codeCoverage": true,
    "codeCoverageExclude": ["src/app/**/*.module.ts"],
    "karmaConfig": "karma.conf.js"
  }
}
```

Et dans `karma.conf.js` :

```javascript
coverageReporter: {
  thresholds: {
    emitWarning: false,
    global: {
      statements: 80,
      branches: 80,
      functions: 80,
      lines: 80
    }
  }
}
```

---

## Récapitulatif général

### Architecture des fichiers de test

```
src/app/
  service/
    calc/
      calc.service.ts
      calc.service.spec.ts      ← tests du service
    data/
      data.service.ts
      data.service.spec.ts      ← tests HTTP
  pipe/
    grade.pipe.ts
    grade.pipe.spec.ts          ← tests du pipe
  directive/
    grade.directive.ts
    grade.directive.spec.ts     ← tests de la directive
  home/
    home.component.ts
    home.component.spec.ts      ← tests du composant
  app.routes.spec               ← tests du routing
```

### Choisir la bonne approche selon ce qu'on teste

| Ce qu'on teste | Approche recommandée |
|----------------|---------------------|
| Service sans dépendances | `new MonService()` |
| Service avec dépendances | `TestBed` + `jasmine.createSpyObj` |
| Service avec HTTP | `HttpClientTestingModule` + `HttpTestingController` |
| Composant (rendu, DOM) | `TestBed` + `ComponentFixture` |
| Code asynchrone | `fakeAsync` + `tick` ou `flush` |
| Pipe | `new MonPipe()` (pas de TestBed) |
| Directive | `TestBed` avec un composant hôte |
| Routing | `RouterTestingModule.withRoutes(routes)` |

### Commandes utiles

```bash
ng test                           # lancer les tests en mode watch
ng test --no-watch                # lancer les tests une seule fois
ng test --no-watch --code-coverage # avec rapport de couverture
ng test --include="**/calc*"      # filtrer les fichiers de tests
```

### Anti-patterns à éviter

- ❌ Utiliser `new MonService()` quand le service a des dépendances complexes.
- ❌ Effectuer de vrais appels HTTP dans les tests.
- ❌ Laisser `fdescribe` ou `fit` dans le code → les autres tests ne s'exécuteront plus.
- ❌ Oublier `fixture.detectChanges()` après une modification du composant.
- ❌ Oublier `testingController.verify()` dans `afterEach` pour les tests HTTP.
- ❌ Utiliser des vraies instances de services qui ont des effets de bord (appels HTTP, timers, etc.).

---

# 🧪 Angular Unit Testing — Résumé Rapide (Cheat Sheet)

---

## 1. Introduction

- Les tests Angular utilisent **Jasmine** (syntaxe) + **Karma** (runner).
- Structure de base : `describe` → groupe | `it` → cas de test | `expect` → assertion.
- Fichiers de test : `*.spec.ts` | Lancer : `ng test`.

```typescript
describe('CalcService', () => {
  it('should multiply', () => {
    const result = new CalcService().multiply(3, 5);
    expect(result).toBe(15);
  });
});
```

---

## 2. Spy

- **`spyOn(obj, 'method')`** → remplace une méthode par une fonction vide + surveille les appels. Nécessite une vraie instance.
- **`.and.callThrough()`** → surveille ET laisse la vraie implémentation tourner.
- **`jasmine.createSpyObj('Nom', ['method1', 'method2'])`** → crée un faux objet entier sans jamais instancier la vraie classe (évite les effets de bord du constructeur).

```typescript
// spyOn → instance réelle requise
spyOn(shared, 'mySharedFunction');

// createSpyObj → pas d'instance réelle, constructeur jamais appelé
let shared = jasmine.createSpyObj('SharedService', ['mySharedFunction']);
```

---

## 3. BeforeEach, TestBed & DI

- **`TestBed.configureTestingModule({...})`** → simule un module Angular pour les tests.
- **`TestBed.inject(Service)`** → récupère une instance via le système DI d'Angular.
- **`beforeEach`** → s'exécute avant chaque test (initialisation).
- **`{ provide: X, useValue: fakeObj }`** → injecter un faux service à la place du vrai.

```typescript
beforeEach(() => {
  sharedService = jasmine.createSpyObj('SharedService', ['mySharedFunction']);
  TestBed.configureTestingModule({
    providers: [CalcService, { provide: SharedService, useValue: sharedService }]
  });
  calcService = TestBed.inject(CalcService);
});
```

- `xdescribe` / `xit` → ignorer | `fdescribe` / `fit` → focus (ne jamais laisser en prod !)

---

## 4. HTTP Testing

- Importer **`HttpClientTestingModule`** (remplace `HttpClientModule`).
- **`HttpTestingController`** → intercepte et simule les requêtes HTTP.
- **`expectOne(url)`** → capture la requête | **`flush(data)`** → simule la réponse.
- **`verify()`** dans `afterEach` → s'assurer qu'il n'y a pas de requêtes non gérées.

```typescript
it('should get users', () => {
  service.getAllUsers().subscribe(users => {
    expect(users.length).toBe(6); // assertions DANS le subscribe
  });
  const req = testingController.expectOne('api/users');
  expect(req.request.method).toEqual('GET');
  req.flush(mockData); // déclenche le subscribe
});
```

---

## 5. Component Testing

- **`ComponentFixture<T>`** → wrapper du composant (accès au DOM + instance).
- **`fixture.componentInstance`** → l'instance TypeScript du composant.
- **`fixture.debugElement`** → accès au DOM (requêtes CSS, événements).
- **`fixture.detectChanges()`** → déclenche le rendu / la détection de changements (**obligatoire** après chaque modification !).
- **`compileComponents()`** → compile template + styles (nécessaire si `templateUrl` / `styleUrl`).

```typescript
fixture = TestBed.createComponent(HomeComponent);
component = fixture.componentInstance;
el = fixture.debugElement;
fixture.detectChanges(); // déclenche ngOnInit + premier rendu
```

---

## 6. DOM Interaction

- **`el.queryAll(By.css('.btn'))`** → sélectionner des éléments du DOM.
- **`.nativeElement`** → accès à l'élément DOM natif (`.textContent`, `.disabled`, `.style`...).
- **`.nativeElement.click()`** → simuler un clic natif.
- **`el.triggerEventHandler('mouseenter', {})`** → déclencher un event binding Angular.
- Toujours appeler `fixture.detectChanges()` après un événement pour mettre à jour le DOM.

```typescript
const btn = el.queryAll(By.css('.subscribe'))[0];
btn.nativeElement.click();
fixture.detectChanges(); // ← ne pas oublier !
expect(btn.nativeElement.disabled).toBeTrue();
```

---

## 7. Opérations Asynchrones

- **`fakeAsync`** → remplace les vrais timers par une horloge virtuelle.
- **`tick(ms)`** → avance l'horloge virtuelle d'une durée précise.
- **`flush()`** → exécute tous les `setTimeout` en attente (durée inconnue).
- **`flushMicrotasks()`** → résout toutes les Promises en attente.

```typescript
it('async test', fakeAsync(() => {
  let counter = 0;
  setTimeout(() => counter += 2, 2000);
  Promise.resolve().then(() => counter += 1);

  flushMicrotasks(); // → counter = 1 (Promise résolue)
  tick(2000);        // → counter = 3 (setTimeout exécuté)
}));
```

---

## 8. Promise & Observable

- Observable **synchrone** (`of(value)`) → pas besoin de `fakeAsync`, le `subscribe` s'exécute immédiatement.
- Observable **asynchrone** (`of(value).pipe(delay(1000))`) → utiliser `fakeAsync` + `tick`.
- **`whenStable()`** → Promise qui se résout quand toutes les tâches async sont terminées.

```typescript
// Observable asynchrone
it('async obs', fakeAsync(() => {
  let result = false;
  of(true).pipe(delay(1000)).subscribe(v => result = v);
  tick(1000);
  expect(result).toBeTrue();
}));
```

---

## 9. Pipes

- Un pipe = classe pure avec méthode `transform` → **pas besoin de `TestBed`**, instanciation directe.
- Tester chaque branche `if/else` pour une bonne couverture.

```typescript
describe('GradePipe', () => {
  let pipe: GradePipe;
  beforeEach(() => { pipe = new GradePipe(); });

  it('should return A for 93', () => expect(pipe.transform(93)).toBe('A'));
  it('should return FAIL for 20', () => expect(pipe.transform(20)).toBe('FAIL'));
});
```

---

## 10. Directives

- Une directive a besoin d'un **composant hôte** pour être testée (elle vit dans le DOM).
- Utiliser `triggerEventHandler` pour déclencher les `@HostListener`.
- Utiliser `waitForAsync` si le composant hôte a des imports asynchrones.

```typescript
it('should color green on mouseenter (mark >= 90)', () => {
  const divs = el.queryAll(By.css('div'));
  divs[0].triggerEventHandler('mouseenter', {});
  fixture.detectChanges();
  expect(divs[0].nativeElement.style.color).toBe('green');
});
```

---

## 11. Routing

- **`RouterTestingModule.withRoutes(routes)`** → simuler le Router avec nos routes.
- **`router.initialNavigation()`** → déclencher la navigation initiale (route par défaut).
- **`Location`** → vérifier l'URL active avec `location.pathname`.
- **`fixture.whenStable()`** → attendre que la navigation (async) soit terminée.

```typescript
it('should redirect to /home by default', waitForAsync(() => {
  fixture.detectChanges();
  fixture.whenStable().then(() => {
    expect(location.pathname).toBe('/home');
  });
}));
```

> Tout composant utilisant `routerLink` doit importer `RouterTestingModule` dans son spec.

---

## 12. Code Coverage

```bash
ng test --no-watch --code-coverage
```

- Génère un rapport dans le dossier `coverage/` (HTML interactif).
- 4 métriques : **Statements**, **Branches**, **Functions**, **Lines**.

| Métrique | Ce qu'elle mesure |
|----------|------------------|
| Statements | Déclarations exécutées |
| Branches | Branches if/else/switch couvertes |
| Functions | Fonctions appelées |
| Lines | Lignes de code exécutées |

---

## Tableau de décision rapide

| Ce qu'on teste | Approche |
|----------------|----------|
| Service sans dépendances | `new MonService()` |
| Service avec dépendances | `TestBed` + `createSpyObj` + `useValue` |
| Service avec HTTP | `HttpClientTestingModule` + `HttpTestingController` |
| Composant / DOM | `TestBed` + `ComponentFixture` + `detectChanges` |
| Code async | `fakeAsync` + `tick` / `flush` |
| Pipe | `new MonPipe()` — pas de TestBed |
| Directive | `TestBed` avec composant hôte + `triggerEventHandler` |
| Routing | `RouterTestingModule.withRoutes(routes)` + `whenStable` |

---

## ⚠️ Pièges classiques à éviter

| Erreur | Solution |
|--------|----------|
| Oublier `fixture.detectChanges()` | Toujours appeler après une modif du composant |
| Laisser `fdescribe` / `fit` | Les autres tests ne s'exécutent plus |
| Oublier `verify()` en HTTP | `afterEach(() => testingController.verify())` |
| Vrai `new SharedService()` avec effet de bord | Utiliser `createSpyObj` |
| Assertions dans `subscribe` sans `flush` | `flush()` ou `tick()` avant les assertions |


---

# 🎤 Angular Unit Testing — Questions / Réponses d'Entretien

## 🟢 Niveau Débutant — Les bases

---

**Q : C'est quoi un unit test, et pourquoi c'est important ?**

Un unit test, c'est un test qui vérifie qu'une petite unité de code — une fonction, un service, un composant — fonctionne correctement de manière isolée, sans dépendre du reste de l'application. C'est important parce que ça permet de détecter les bugs tôt, avant même de lancer l'application. Ça donne aussi confiance pour refactoriser du code, puisqu'on sait que si les tests passent toujours, on n'a rien cassé.

---

**Q : Quels outils est-ce qu'Angular utilise pour les tests ?**

Angular utilise principalement deux outils. D'abord **Jasmine**, qui est le framework de tests — c'est lui qui fournit la syntaxe avec `describe`, `it`, `expect`. Ensuite **Karma**, qui est le runner — c'est lui qui exécute les tests dans un vrai navigateur et affiche les résultats. En plus de ça, Angular fournit **TestBed**, qui est un utilitaire pour créer un environnement de test Angular isolé.

---

**Q : C'est quoi la différence entre `describe` et `it` en Jasmine ?**

`describe` sert à regrouper des tests qui concernent la même chose — par exemple tous les tests d'un service. C'est comme un dossier. Et `it` définit un cas de test individuel à l'intérieur de ce groupe. En pratique, le nom qu'on donne au `it` doit décrire ce que le test vérifie — genre `it('should multiply two numbers')` — pour que quand un test échoue, on comprenne immédiatement ce qui ne va pas.

---

**Q : Comment tu lances les tests dans un projet Angular ?**

La commande de base c'est `ng test`, et ça lance Karma en mode watch — les tests se relancent automatiquement à chaque modification. Si on veut juste exécuter les tests une seule fois sans rester en mode watch, on utilise `ng test --no-watch`. Et pour générer un rapport de couverture de code en même temps, on ajoute `--code-coverage`.

---

**Q : C'est quoi les matchers Jasmine les plus utilisés ?**

Les plus courants c'est `toBe` pour une égalité stricte, `toEqual` pour une comparaison profonde d'objets, `toBeTruthy` et `toBeFalsy` pour vérifier qu'une valeur est vraie ou fausse au sens JavaScript, et `toHaveBeenCalled` pour vérifier qu'une fonction a été appelée. Il y a aussi `toBeTrue`, `toBeFalse`, `toContain`, et `toHaveBeenCalledWith` pour les spies.

---

## 🟡 Niveau Intermédiaire — TestBed, Spies, DI

---

**Q : C'est quoi `TestBed` et pourquoi on l'utilise ?**

`TestBed` c'est l'utilitaire principal d'Angular pour les tests. Il permet de créer un module Angular temporaire juste pour les tests — un peu comme un `@NgModule` mais uniquement en mémoire. Ça permet de bénéficier du système de Dependency Injection d'Angular dans les tests, et d'éviter d'instancier manuellement tous les services avec `new`. On le configure avec `configureTestingModule`, et ensuite on récupère les instances avec `TestBed.inject`.

---

**Q : C'est quoi la différence entre `spyOn` et `jasmine.createSpyObj` ?**

Les deux créent des espions, mais leur usage est différent. `spyOn` s'utilise sur une instance qui existe déjà — elle remplace une méthode par une version fictive qui enregistre les appels mais ne fait rien par défaut. Le problème c'est qu'il faut quand même instancier la vraie classe. `jasmine.createSpyObj` lui, crée un faux objet entier sans jamais instancier la vraie classe — donc le constructeur ne s'exécute jamais, ce qui évite les effets de bord comme de vrais appels HTTP ou des logs indésirables. En pratique, si mon service a un constructeur qui fait quelque chose de lourd, je préfère `createSpyObj`.

---

**Q : Pourquoi on utilise `{ provide: X, useValue: fakeObj }` dans `configureTestingModule` ?**

Quand un service a des dépendances, Angular les résout via le système DI en utilisant le type comme token. Avec `useValue`, on dit à Angular : "pour ce token-là, donne cet objet fictif au lieu de créer une vraie instance". C'est la façon propre d'injecter un faux service dans les tests. Par exemple, si `CalcService` dépend de `SharedService`, je crée un faux `SharedService` avec `createSpyObj`, et je dis à TestBed de l'utiliser à la place du vrai.

---

**Q : C'est quoi `beforeEach` et quand est-ce qu'on l'utilise ?**

`beforeEach` est une fonction qui s'exécute automatiquement avant chaque test dans un `describe`. C'est l'endroit idéal pour initialiser les variables, configurer TestBed, et instancier les composants ou services. L'idée c'est que chaque test repart avec un état propre et connu — on évite que les tests se contaminent entre eux.

---

**Q : C'est quoi `xdescribe`, `fdescribe`, `xit`, `fit` ?**

Ce sont des préfixes pour contrôler quels tests s'exécutent. `x` signifie "exclure" — le test ou le groupe est ignoré. `f` signifie "focus" — seul ce test ou ce groupe s'exécute. C'est utile pour déboguer un test spécifique sans lancer toute la suite. Mais attention, il ne faut jamais les laisser en production — si on pousse un `fdescribe`, les autres tests de l'application ne s'exécuteront plus et on pourrait passer à côté de régressions.

---

**Q : Quelle est la différence entre `spyOn` avec et sans `.and.callThrough()` ?**

Sans `.callThrough()`, le spy remplace la méthode par une fonction vide — la vraie implémentation ne s'exécute pas. Avec `.callThrough()`, le spy surveille l'appel mais laisse quand même la vraie méthode tourner. On utilise `.callThrough()` quand on veut à la fois vérifier qu'une fonction a été appelée ET s'assurer que son comportement réel se produit. Sans `callThrough`, c'est uniquement pour vérifier qu'elle a été appelée, sans effets de bord.

---

## 🟠 Niveau Avancé — HTTP, Composants, Async

---

**Q : Comment on teste un service qui fait des appels HTTP ?**

On importe `HttpClientTestingModule` dans la configuration de TestBed — ça remplace le vrai `HttpClient` par une version fictive. Ensuite on injecte `HttpTestingController` qui permet d'intercepter les requêtes. Le test fonctionne comme ça : on appelle la méthode du service, ce qui déclenche une requête HTTP qui est interceptée. On capture cette requête avec `expectOne(url)`, on vérifie sa méthode avec `request.method`, et on simule la réponse du serveur avec `flush(data)`. C'est ce `flush` qui déclenche le `subscribe` et où les assertions s'exécutent. Et dans `afterEach`, on appelle `verify()` pour s'assurer qu'il ne reste pas de requêtes non gérées.

---

**Q : Pourquoi les assertions sont dans le `subscribe` et pas après ?**

Parce que le `subscribe` s'exécute de manière asynchrone — il attend la réponse du serveur. Si on met les assertions après le `subscribe` sans attendre, elles s'exécutent avant que le `subscribe` ait reçu ses données, donc elles ne testent rien. C'est pourquoi on appelle `flush` après avoir configuré le `subscribe` — le `flush` simule la réponse, ce qui déclenche immédiatement le callback du `subscribe`, et là les assertions s'exécutent dans le bon ordre.

---

**Q : C'est quoi `ComponentFixture` et à quoi ça sert ?**

`ComponentFixture` c'est un wrapper qu'Angular fournit autour d'un composant de test. Il donne accès à trois choses principales : `componentInstance` pour accéder à l'instance TypeScript du composant et modifier ses propriétés, `debugElement` pour interagir avec le DOM du composant via des requêtes CSS, et `detectChanges()` pour déclencher manuellement la détection de changements et mettre à jour le DOM.

---

**Q : Pourquoi est-ce qu'on doit appeler `fixture.detectChanges()` manuellement ?**

Dans les tests, Angular ne déclenche pas automatiquement la détection de changements pour garder les tests déterministes. Si on modifie une propriété du composant, le DOM ne se met pas à jour tout seul — il faut appeler `detectChanges()` explicitement. C'est important de l'appeler après l'initialisation pour déclencher `ngOnInit`, et après chaque modification ou événement pour voir les effets dans le DOM.

---

**Q : C'est quoi la différence entre `fakeAsync`/`tick` et `waitForAsync`/`whenStable` ?**

`fakeAsync` avec `tick` est la solution préférée pour la plupart des cas asynchrones. Elle remplace les vrais timers par une horloge virtuelle que l'on contrôle avec `tick(ms)`. C'est synchrone dans le test, prévisible, et très lisible. `waitForAsync` avec `whenStable` est une alternative quand on a de vraies opérations asynchrones que `fakeAsync` ne peut pas intercepter — comme certains appels XHR. `whenStable()` retourne une Promise qui se résout quand toutes les tâches async sont terminées. En pratique, `fakeAsync` est beaucoup plus courant.

---

**Q : Quelle est la différence entre `tick`, `flush` et `flushMicrotasks` ?**

`tick(ms)` avance l'horloge virtuelle d'un nombre précis de millisecondes. C'est utile quand on connaît exactement la durée d'un `setTimeout`. `flush()` exécute tous les macrotasks en attente — tous les `setTimeout` et `setInterval` — sans qu'on ait besoin de connaître leur durée. C'est plus souple. Et `flushMicrotasks()` lui, ne gère que les microtasks — c'est-à-dire les Promises. Si j'ai à la fois des Promises et des `setTimeout`, je peux appeler `flushMicrotasks()` d'abord pour résoudre les Promises, puis `tick` ou `flush` pour les timers.

---

**Q : Comment tu testes un pipe Angular ?**

Un pipe c'est juste une classe avec une méthode `transform`. Il n'y a aucune raison d'utiliser TestBed pour ça. On l'instancie directement avec `new`, et on appelle `transform` avec différentes valeurs en vérifiant le résultat. C'est le type de test le plus simple à écrire. La bonne pratique c'est de couvrir chaque branche de la logique — chaque condition `if/else` — pour avoir une bonne couverture de code.

---

**Q : Comment tu testes une directive Angular ?**

C'est plus complexe qu'un pipe parce qu'une directive vit dans le DOM — elle a besoin d'un élément HTML pour s'attacher. Donc on utilise un composant hôte qui utilise déjà cette directive dans son template. On crée le composant avec `TestBed`, et ensuite on utilise `triggerEventHandler` pour déclencher les `@HostListener` de la directive — par exemple `mouseenter` ou `mouseleave`. On vérifie ensuite les effets dans le DOM, comme un changement de couleur ou de style.

---

## 🔴 Niveau Expert — Routing, Coverage, Bonnes pratiques

---

**Q : Comment tu testes le routing Angular ?**

On utilise `RouterTestingModule.withRoutes(routes)` pour simuler le Router sans naviguer dans un vrai navigateur. On injecte `Router` pour déclencher des navigations, et `Location` pour vérifier l'URL active via `location.pathname`. Il faut appeler `router.initialNavigation()` pour que la route par défaut s'applique. Et comme la navigation est asynchrone, on utilise `fixture.whenStable()` pour attendre que la navigation soit terminée avant de vérifier l'URL.

---

**Q : Pourquoi on doit ajouter `RouterTestingModule` même dans des tests de composants qui n'testent pas le routing ?**

Parce que si un composant utilise `routerLink` ou `RouterOutlet` dans son template, Angular va essayer de résoudre ces directives au moment de la compilation du template. Sans `RouterTestingModule`, Angular ne connaît pas ces directives et lance une erreur. Donc même si on ne teste pas la navigation, il faut importer `RouterTestingModule` pour que le composant puisse compiler correctement.

---

**Q : C'est quoi la couverture de code et comment tu la génères dans Angular ?**

La couverture de code mesure quelle proportion du code est exécutée pendant les tests. On la génère avec `ng test --no-watch --code-coverage`. Angular génère un rapport HTML dans le dossier `coverage/` qu'on peut ouvrir dans un navigateur pour voir exactement quelles lignes ne sont pas couvertes — elles sont surlignées en rouge. Il y a quatre métriques : les Statements (déclarations), les Branches (conditions if/else), les Functions (fonctions), et les Lines (lignes). Un objectif courant c'est d'atteindre au moins 80% sur chaque métrique.

---

**Q : C'est quoi la différence entre `imports`, `declarations` et `providers` dans `configureTestingModule` ?**

`imports` c'est pour les modules Angular — comme `HttpClientTestingModule`, `RouterTestingModule`, ou les composants standalone. `declarations` c'est pour les composants, pipes, et directives classiques qui appartiennent à un module — les non-standalone. Et `providers` c'est pour les services — soit les vrais, soit des faux via `useValue`. Dans les projets modernes avec des composants standalone, on utilise presque exclusivement `imports`.

---

**Q : Quelle est la bonne approche pour tester un composant standalone ?**

Pour un composant standalone, on l'importe directement dans `imports` de `configureTestingModule` — pas dans `declarations`. C'est parce qu'un composant standalone est lui-même un module. Ensuite, si ce composant importe d'autres choses comme `RouterLink` ou des pipes, il faut aussi s'assurer qu'ils sont disponibles — soit parce que le composant les importe lui-même, soit en ajoutant les modules correspondants dans le test.

---

**Q : Pourquoi on ne doit pas faire de vrais appels HTTP dans les tests ?**

Plusieurs raisons. D'abord la **vitesse** — un vrai appel réseau peut prendre des centaines de millisecondes, et si on a des centaines de tests, ça devient très lent. Ensuite la **fiabilité** — les tests deviendraient dépendants d'un serveur externe qui peut être indisponible. Et enfin le **déterminisme** — le même test pourrait retourner des résultats différents selon l'état du serveur. Les tests doivent être rapides, fiables, et reproductibles — d'où l'utilisation de `HttpClientTestingModule`.

---

**Q : Que se passe-t-il si on oublie `testingController.verify()` dans `afterEach` ?**

Si on oublie `verify()`, Angular ne vérifie pas qu'il n'y a pas de requêtes HTTP non gérées. Ça peut masquer des bugs — par exemple, si une méthode fait un appel HTTP inattendu, le test passerait quand même. `verify()` agit comme un filet de sécurité qui garantit que toutes les requêtes déclenchées pendant le test ont été explicitement traitées. C'est une bonne pratique systématique à mettre dans `afterEach`.

---

**Q : Comment tu testes une méthode qui retourne à la fois des données ET appelle un autre service ?**

C'est le cas typique de `CalcService` qui fait `multiply` ET appelle `sharedService.mySharedFunction()`. On fait deux assertions dans le même test : d'abord on vérifie le résultat du calcul avec `expect(result).toBe(15)`, et ensuite on vérifie que le service dépendant a bien été appelé avec `expect(sharedService.mySharedFunction).toHaveBeenCalled()`. Pour que cette deuxième assertion fonctionne, `sharedService` doit être un spy — soit créé avec `createSpyObj`, soit espionné avec `spyOn`.

---

**Q : Comment tu gères le cas où un composant utilise `*ngIf` et que tu dois vérifier l'état après un changement ?**

Avec `*ngIf`, quand la condition change, Angular ajoute ou retire l'élément du DOM. Il ne suffit pas de garder une référence à l'ancien élément — il faut re-requêter le DOM après `detectChanges`. Par exemple, si j'ai deux boutons avec `*ngIf="!isSubscribed"` et `*ngIf="isSubscribed"`, après le clic j'appelle `fixture.detectChanges()`, et ensuite je refais `el.queryAll(By.css('.subscribe'))` pour récupérer les nouveaux éléments. L'ancien tableau de références est devenu obsolète.

---

## 💬 Questions comportementales liées au testing

---

**Q : Dans ton expérience, qu'est-ce qui est le plus difficile à tester dans Angular ?**

Pour moi, le plus difficile c'est le code asynchrone complexe — quand on mélange des Promises, des observables avec des délais, et des `setTimeout` dans le même flux. Il faut bien comprendre la différence entre macrotasks et microtasks pour utiliser le bon outil : `flushMicrotasks` pour les Promises, `tick` ou `flush` pour les timers. L'autre chose difficile c'est les directives, parce qu'elles nécessitent un composant hôte et on doit bien configurer l'environnement de test.

---

**Q : Quelle est ta stratégie pour tester un service qui a beaucoup de dépendances ?**

Ma stratégie c'est de toujours créer des faux objets pour toutes les dépendances avec `jasmine.createSpyObj`. Je ne veux tester que le service lui-même, pas ses dépendances. Pour chaque test, je configure le comportement des spies selon ce dont j'ai besoin — par exemple `.and.returnValue(of([]))` pour simuler un Observable, ou `.and.resolveTo(data)` pour une Promise. Ça garde les tests isolés et rapides.

---

**Q : Comment tu sais si tu as assez de tests ?**

La couverture de code est un indicateur utile mais pas suffisant. On peut avoir 100% de couverture et quand même avoir de mauvais tests si les assertions ne testent pas les bons comportements. Ma règle c'est : tester chaque comportement observable — chaque branche `if/else`, chaque cas limite (valeur nulle, tableau vide, erreur HTTP), et chaque interaction utilisateur importante. Le chiffre de 80% de couverture est un minimum raisonnable, mais l'objectif c'est vraiment de tester les cas qui comptent.

---

**Q : Qu'est-ce que tu fais quand un test devient trop complexe ou difficile à écrire ?**

Un test difficile à écrire est souvent le signe que le code lui-même est trop couplé ou a trop de responsabilités — c'est le principe "code testable = code bien conçu". Si un composant est dur à tester, je me demande si je peux extraire de la logique dans un service séparé, ou si le composant fait trop de choses à la fois. En pratique, si je dois mocker 5 dépendances pour tester une seule méthode, c'est un signal que quelque chose ne va pas dans l'architecture.
