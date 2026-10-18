# Guide Complet Angular & TypeScript pour Entretien Backend

## Table des Matières
1. [TypeScript - Fondamentaux](#typescript-fondamentaux)
2. [Angular - Architecture Complète](#angular-architecture)
3. [Components en Détail](#components)
4. [Services & Dependency Injection](#services)
5. [Routing & Navigation](#routing)
6. [HTTP & Communication Backend](#http-communication)
7. [Forms & Validation](#forms)
8. [RxJS & Observables](#rxjs)
9. [Directives & Pipes](#directives-pipes)
10. [State Management](#state-management)
11. [Testing](#testing)
12. [Best Practices](#best-practices)

---

## TypeScript - Fondamentaux

### Qu'est-ce que TypeScript ?
TypeScript est un sur-ensemble typé de JavaScript développé par Microsoft. Il compile vers du JavaScript standard et ajoute :
- **Typage statique** : détection d'erreurs à la compilation
- **Interfaces & Types** : contrats pour la structure des données
- **Classes avancées** : héritage, modificateurs d'accès, abstractions
- **Génériques** : code réutilisable et type-safe
- **Decorators** : métaprogrammation (utilisé massivement par Angular)

### Types de Base

```typescript
// Types primitifs
let nom: string = "John";
let age: number = 30;
let estActif: boolean = true;
let data: any = "peut être n'importe quoi"; // à éviter
let inconnu: unknown = "plus sûr que any";
let rien: void = undefined; // pour fonctions sans retour
let nul: null = null;
let indefini: undefined = undefined;

// Arrays
let nombres: number[] = [1, 2, 3];
let textes: Array<string> = ["a", "b", "c"];

// Tuples
let personne: [string, number] = ["Alice", 25];

// Enums
enum Role {
  Admin = "ADMIN",
  User = "USER",
  Guest = "GUEST"
}
let userRole: Role = Role.Admin;

// Union types
let id: string | number = 123;
id = "ABC123"; // valide aussi

// Literal types
let direction: "left" | "right" | "up" | "down" = "left";
```

### Interfaces vs Types

```typescript
// Interface - extensible, pour objets
interface User {
  id: number;
  name: string;
  email: string;
  role?: Role; // propriété optionnelle
  readonly createdAt: Date; // lecture seule
}

// Extension d'interface
interface Admin extends User {
  permissions: string[];
}

// Type alias - plus flexible
type UserOrAdmin = User | Admin;
type Callback = (data: string) => void;
type Nullable<T> = T | null;

// Différence principale : 
// - Interface peut être étendue par d'autres (declaration merging)
// - Type peut représenter unions, intersections, primitifs
```

### Classes

```typescript
class Animal {
  // Propriétés
  private nom: string; // accessible uniquement dans la classe
  protected age: number; // accessible dans la classe et sous-classes
  public espece: string; // accessible partout (par défaut)
  readonly id: number; // lecture seule après initialisation
  
  // Constructeur avec shorthand
  constructor(nom: string, age: number, espece: string) {
    this.nom = nom;
    this.age = age;
    this.espece = espece;
    this.id = Math.random();
  }
  
  // Shorthand constructor (équivalent au dessus)
  // constructor(
  //   private nom: string,
  //   protected age: number,
  //   public espece: string
  // ) {}
  
  // Méthodes
  public parler(): string {
    return `${this.nom} fait du bruit`;
  }
  
  // Getter
  get Age(): number {
    return this.age;
  }
  
  // Setter
  set Age(value: number) {
    if (value > 0) this.age = value;
  }
  
  // Méthode statique
  static creerChien(nom: string, age: number): Animal {
    return new Animal(nom, age, "Chien");
  }
}

// Héritage
class Chien extends Animal {
  constructor(nom: string, age: number) {
    super(nom, age, "Chien"); // appel constructeur parent
  }
  
  // Override
  parler(): string {
    return `${this.nom} aboie`;
  }
}

// Classes abstraites
abstract class Forme {
  abstract calculerAire(): number; // doit être implémentée
  
  afficher(): void {
    console.log(`Aire: ${this.calculerAire()}`);
  }
}
```

### Génériques

```typescript
// Fonction générique
function identity<T>(arg: T): T {
  return arg;
}

let output1 = identity<string>("Hello");
let output2 = identity<number>(42);

// Interface générique
interface ApiResponse<T> {
  data: T;
  status: number;
  message: string;
}

const userResponse: ApiResponse<User> = {
  data: { id: 1, name: "John", email: "john@test.com" },
  status: 200,
  message: "Success"
};

// Classe générique
class DataStore<T> {
  private items: T[] = [];
  
  add(item: T): void {
    this.items.push(item);
  }
  
  getAll(): T[] {
    return this.items;
  }
}

const userStore = new DataStore<User>();
userStore.add({ id: 1, name: "Alice", email: "alice@test.com" });

// Contraintes génériques
interface HasLength {
  length: number;
}

function logLength<T extends HasLength>(arg: T): void {
  console.log(arg.length);
}

logLength("Hello"); // OK
logLength([1, 2, 3]); // OK
// logLength(123); // Erreur : number n'a pas de propriété length
```

### Decorators (utilisés par Angular)

```typescript
// Decorator de classe
function Component(config: any) {
  return function(target: any) {
    // Logique du decorator
    target.prototype.config = config;
  }
}

// Decorator de méthode
function Log(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
  const originalMethod = descriptor.value;
  
  descriptor.value = function(...args: any[]) {
    console.log(`Appel de ${propertyKey} avec`, args);
    return originalMethod.apply(this, args);
  }
}

// Utilisation
@Component({ selector: 'app-test' })
class TestComponent {
  @Log
  sayHello(name: string) {
    console.log(`Hello ${name}`);
  }
}
```

### Utility Types

```typescript
interface Todo {
  title: string;
  description: string;
  completed: boolean;
}

// Partial - toutes propriétés optionnelles
type PartialTodo = Partial<Todo>;
// { title?: string; description?: string; completed?: boolean; }

// Required - toutes propriétés obligatoires
type RequiredTodo = Required<PartialTodo>;

// Readonly - toutes propriétés en lecture seule
type ReadonlyTodo = Readonly<Todo>;

// Pick - sélectionner certaines propriétés
type TodoPreview = Pick<Todo, 'title' | 'completed'>;
// { title: string; completed: boolean; }

// Omit - exclure certaines propriétés
type TodoInfo = Omit<Todo, 'completed'>;
// { title: string; description: string; }

// Record - créer un type objet
type TodoMap = Record<string, Todo>;
// { [key: string]: Todo }

// ReturnType - extraire le type de retour
function getTodo(): Todo {
  return { title: "", description: "", completed: false };
}
type TodoReturn = ReturnType<typeof getTodo>; // Todo
```

---

## Angular - Architecture

### Vue d'ensemble

Angular est un framework frontend complet développé par Google pour créer des Single Page Applications (SPA). Il suit l'architecture **MVC/MVVM** et utilise le pattern **Component-Based**.

**Philosophie Angular :**
- **Opiniated** : impose une structure et des patterns
- **Batteries included** : routing, forms, HTTP, testing intégrés
- **TypeScript first** : pensé pour TypeScript
- **Reactive** : utilise RxJS pour la programmation réactive

### Structure d'un Projet Angular

```
mon-app/
├── src/
│   ├── app/
│   │   ├── core/                 # Services singleton, guards
│   │   ├── shared/               # Components, directives, pipes partagés
│   │   ├── features/             # Modules fonctionnels
│   │   │   ├── users/
│   │   │   │   ├── components/
│   │   │   │   ├── services/
│   │   │   │   ├── models/
│   │   │   │   └── users.module.ts
│   │   ├── app.component.ts      # Component racine
│   │   ├── app.component.html
│   │   ├── app.component.css
│   │   ├── app.module.ts         # Module racine
│   │   └── app-routing.module.ts # Configuration routing
│   ├── assets/                   # Images, fonts, etc.
│   ├── environments/             # Config par environnement
│   ├── index.html
│   ├── main.ts                   # Point d'entrée
│   └── styles.css                # Styles globaux
├── angular.json                  # Configuration Angular CLI
├── package.json
└── tsconfig.json                 # Configuration TypeScript
```

### Modules Angular

Les modules organisent l'application en blocs cohérents.

```typescript
// app.module.ts
import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { HttpClientModule } from '@angular/common/http';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { UsersModule } from './features/users/users.module';

@NgModule({
  declarations: [
    // Components, Directives, Pipes déclarés dans ce module
    AppComponent
  ],
  imports: [
    // Modules importés
    BrowserModule,        // Requis pour browser
    HttpClientModule,     // Pour les appels HTTP
    FormsModule,          // Forms template-driven
    ReactiveFormsModule,  // Forms reactive
    AppRoutingModule,     // Routing
    UsersModule          // Module fonctionnel
  ],
  providers: [
    // Services disponibles dans toute l'app (singleton)
    // Préférer providedIn: 'root' dans les services
  ],
  bootstrap: [AppComponent] // Component à bootstrapper
})
export class AppModule { }
```

**Module Feature (fonctionnel) :**

```typescript
// users.module.ts
import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { UsersRoutingModule } from './users-routing.module';

import { UserListComponent } from './components/user-list/user-list.component';
import { UserDetailComponent } from './components/user-detail/user-detail.component';
import { UserService } from './services/user.service';

@NgModule({
  declarations: [
    UserListComponent,
    UserDetailComponent
  ],
  imports: [
    CommonModule,           // Directives communes (ngIf, ngFor, etc.)
    UsersRoutingModule
  ],
  providers: [UserService], // ou providedIn dans le service
  exports: [UserListComponent] // Si utilisé dans d'autres modules
})
export class UsersModule { }
```

---

## Components

### Anatomie d'un Component

Un component = Template (HTML) + Classe (TS) + Styles (CSS) + Metadata (Decorator)

```typescript
// user-card.component.ts
import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';

@Component({
  selector: 'app-user-card',           // Comment utiliser: <app-user-card>
  templateUrl: './user-card.component.html',  // Template externe
  // ou template: `<div>...</div>`,    // Template inline
  styleUrls: ['./user-card.component.css'],   // Styles externes
  // ou styles: [`div { color: red; }`] // Styles inline
})
export class UserCardComponent implements OnInit {
  // @Input - Données reçues du parent
  @Input() user!: User;              // ! = definitely assigned
  @Input() showDetails: boolean = false;
  
  // @Output - Événements émis vers le parent
  @Output() userSelected = new EventEmitter<User>();
  @Output() deleteRequested = new EventEmitter<number>();
  
  // Propriétés du component
  isExpanded: boolean = false;
  
  // Lifecycle hook
  ngOnInit(): void {
    // Initialisation après création du component
    console.log('Component initialisé avec user:', this.user);
  }
  
  // Méthodes
  toggleExpand(): void {
    this.isExpanded = !this.isExpanded;
  }
  
  selectUser(): void {
    this.userSelected.emit(this.user);
  }
  
  deleteUser(): void {
    this.deleteRequested.emit(this.user.id);
  }
  
  // Getter (accessible dans le template)
  get fullName(): string {
    return `${this.user.firstName} ${this.user.lastName}`;
  }
}
```

**Template correspondant :**

```html
<!-- user-card.component.html -->
<div class="user-card" [class.expanded]="isExpanded">
  <!-- Property binding -->
  <h3 [textContent]="fullName"></h3>
  
  <!-- Interpolation -->
  <p>Email: {{ user.email }}</p>
  
  <!-- Event binding -->
  <button (click)="toggleExpand()">
    {{ isExpanded ? 'Réduire' : 'Voir plus' }}
  </button>
  
  <!-- Two-way binding -->
  <input [(ngModel)]="user.name" />
  
  <!-- Conditional display -->
  <div *ngIf="isExpanded && showDetails">
    <p>Détails supplémentaires...</p>
    <p>ID: {{ user.id }}</p>
    <p>Créé le: {{ user.createdAt | date:'dd/MM/yyyy' }}</p>
  </div>
  
  <!-- Event avec paramètre -->
  <button (click)="selectUser()">Sélectionner</button>
  <button (click)="deleteUser()">Supprimer</button>
</div>
```

**Utilisation dans le component parent :**

```html
<!-- app.component.html -->
<app-user-card 
  [user]="selectedUser"
  [showDetails]="true"
  (userSelected)="onUserSelected($event)"
  (deleteRequested)="onDeleteUser($event)">
</app-user-card>
```

```typescript
// app.component.ts
export class AppComponent {
  selectedUser: User = {
    id: 1,
    firstName: "John",
    lastName: "Doe",
    email: "john@test.com",
    createdAt: new Date()
  };
  
  onUserSelected(user: User): void {
    console.log('Utilisateur sélectionné:', user);
  }
  
  onDeleteUser(userId: number): void {
    console.log('Suppression utilisateur:', userId);
  }
}
```

### Lifecycle Hooks

Angular appelle des hooks à différents moments du cycle de vie d'un component.

```typescript
import { Component, OnInit, OnChanges, OnDestroy, 
         DoCheck, AfterContentInit, AfterContentChecked,
         AfterViewInit, AfterViewChecked, SimpleChanges } from '@angular/core';

export class MyComponent implements OnInit, OnChanges, OnDestroy {
  
  // 1. Constructor - appelé en premier (avant Angular)
  constructor() {
    console.log('1. Constructor');
  }
  
  // 2. ngOnChanges - à chaque changement d'@Input
  ngOnChanges(changes: SimpleChanges): void {
    console.log('2. ngOnChanges', changes);
  }
  
  // 3. ngOnInit - une fois, après le premier ngOnChanges
  // => Lieu idéal pour initialisation, appels HTTP
  ngOnInit(): void {
    console.log('3. ngOnInit');
  }
  
  // 4. ngDoCheck - à chaque détection de changements
  ngDoCheck(): void {
    console.log('4. ngDoCheck');
  }
  
  // 5. ngAfterContentInit - après projection du contenu (ng-content)
  ngAfterContentInit(): void {
    console.log('5. ngAfterContentInit');
  }
  
  // 6. ngAfterContentChecked - après vérification du contenu projeté
  ngAfterContentChecked(): void {
    console.log('6. ngAfterContentChecked');
  }
  
  // 7. ngAfterViewInit - après initialisation de la vue
  ngAfterViewInit(): void {
    console.log('7. ngAfterViewInit');
  }
  
  // 8. ngAfterViewChecked - après vérification de la vue
  ngAfterViewChecked(): void {
    console.log('8. ngAfterViewChecked');
  }
  
  // 9. ngOnDestroy - avant destruction du component
  // => Nettoyage : unsubscribe, clear timers
  ngOnDestroy(): void {
    console.log('9. ngOnDestroy');
  }
}
```

**Ordre d'exécution typique :**
1. Constructor
2. ngOnChanges (si @Input présents)
3. ngOnInit (une seule fois)
4. ngDoCheck
5. ngAfterContentInit (une seule fois)
6. ngAfterContentChecked
7. ngAfterViewInit (une seule fois)
8. ngAfterViewChecked
9. ... (4, 6, 8 répétés à chaque cycle de détection)
10. ngOnDestroy (à la destruction)

### Communication entre Components

**1. Parent → Enfant : @Input**

```typescript
// Enfant
@Component({ selector: 'app-child' })
export class ChildComponent {
  @Input() message: string = '';
}

// Template parent
<app-child [message]="parentMessage"></app-child>
```

**2. Enfant → Parent : @Output + EventEmitter**

```typescript
// Enfant
@Component({ selector: 'app-child' })
export class ChildComponent {
  @Output() notify = new EventEmitter<string>();
  
  sendNotification() {
    this.notify.emit('Message depuis enfant');
  }
}

// Template parent
<app-child (notify)="handleNotification($event)"></app-child>
```

**3. Via Service (pour components non liés)**

```typescript
// Service partagé
@Injectable({ providedIn: 'root' })
export class DataService {
  private dataSubject = new BehaviorSubject<string>('');
  data$ = this.dataSubject.asObservable();
  
  updateData(value: string) {
    this.dataSubject.next(value);
  }
}

// Component A
constructor(private dataService: DataService) {}
sendData() {
  this.dataService.updateData('Nouvelle donnée');
}

// Component B
constructor(private dataService: DataService) {}
ngOnInit() {
  this.dataService.data$.subscribe(data => {
    console.log('Données reçues:', data);
  });
}
```

**4. ViewChild / ContentChild - Accès direct**

```typescript
// Parent accède à l'enfant
@Component({
  template: '<app-child #childRef></app-child>'
})
export class ParentComponent {
  @ViewChild('childRef') child!: ChildComponent;
  
  ngAfterViewInit() {
    console.log(this.child.someProperty);
    this.child.someMethod();
  }
}
```

---

## Services & Dependency Injection

### Qu'est-ce qu'un Service ?

Les services encapsulent la logique métier, les appels HTTP, le partage de données. Ils sont **injectables** et généralement **singleton** (une seule instance partagée).

```typescript
// user.service.ts
import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, BehaviorSubject } from 'rxjs';
import { map, catchError } from 'rxjs/operators';

@Injectable({
  providedIn: 'root' // Service singleton disponible partout
  // Alternative: providedIn: UsersModule (limité au module)
})
export class UserService {
  private apiUrl = 'https://api.example.com/users';
  
  // State management simple avec BehaviorSubject
  private usersSubject = new BehaviorSubject<User[]>([]);
  public users$ = this.usersSubject.asObservable();
  
  // Injection de dépendances dans le constructor
  constructor(
    private http: HttpClient,
    private logger: LoggerService
  ) {
    this.loadUsers();
  }
  
  // Méthode privée
  private loadUsers(): void {
    this.getUsers().subscribe(users => {
      this.usersSubject.next(users);
    });
  }
  
  // Méthodes publiques
  getUsers(): Observable<User[]> {
    this.logger.log('Récupération des utilisateurs');
    return this.http.get<User[]>(this.apiUrl).pipe(
      map(users => users.filter(u => u.active)),
      catchError(error => {
        this.logger.error('Erreur lors de la récupération', error);
        throw error;
      })
    );
  }
  
  getUserById(id: number): Observable<User> {
    return this.http.get<User>(`${this.apiUrl}/${id}`);
  }
  
  createUser(user: Partial<User>): Observable<User> {
    return this.http.post<User>(this.apiUrl, user).pipe(
      map(newUser => {
        const current = this.usersSubject.value;
        this.usersSubject.next([...current, newUser]);
        return newUser;
      })
    );
  }
  
  updateUser(id: number, user: Partial<User>): Observable<User> {
    return this.http.put<User>(`${this.apiUrl}/${id}`, user).pipe(
      map(updated => {
        const current = this.usersSubject.value;
        const index = current.findIndex(u => u.id === id);
        if (index > -1) {
          current[index] = updated;
          this.usersSubject.next([...current]);
        }
        return updated;
      })
    );
  }
  
  deleteUser(id: number): Observable<void> {
    return this.http.delete<void>(`${this.apiUrl}/${id}`).pipe(
      map(() => {
        const current = this.usersSubject.value;
        this.usersSubject.next(current.filter(u => u.id !== id));
      })
    );
  }
  
  // Méthodes synchrones
  getCurrentUsers(): User[] {
    return this.usersSubject.value;
  }
  
  searchUsers(term: string): User[] {
    return this.getCurrentUsers().filter(u => 
      u.name.toLowerCase().includes(term.toLowerCase())
    );
  }
}
```

### Dependency Injection (DI)

Angular utilise un système de DI puissant pour gérer les dépendances.

```typescript
// Component avec injection de services
@Component({
  selector: 'app-user-list',
  templateUrl: './user-list.component.html'
})
export class UserListComponent implements OnInit {
  users: User[] = [];
  
  // Injection dans le constructor
  constructor(
    private userService: UserService,     // Service
    private router: Router,               // Router Angular
    private route: ActivatedRoute,        // Route actuelle
    private fb: FormBuilder               // Form builder
  ) {}
  
  ngOnInit(): void {
    this.loadUsers();
  }
  
  loadUsers(): void {
    this.userService.getUsers().subscribe(users => {
      this.users = users;
    });
  }
}
```

**Niveaux de providers (scope) :**

```typescript
// 1. Root level - singleton pour toute l'app
@Injectable({
  providedIn: 'root'
})
export class GlobalService { }

// 2. Module level - une instance par module
@NgModule({
  providers: [ModuleScopedService]
})
export class UsersModule { }

// 3. Component level - nouvelle instance par component
@Component({
  selector: 'app-user',
  providers: [ComponentScopedService] // Nouvelle instance à chaque component
})
export class UserComponent { }
```

**Injection de tokens personnalisés :**

```typescript
// Définir un token
import { InjectionToken } from '@angular/core';

export const API_URL = new InjectionToken<string>('api.url');

// Fournir la valeur
@NgModule({
  providers: [
    { provide: API_URL, useValue: 'https://api.example.com' }
  ]
})
export class AppModule { }

// Injecter
constructor(@Inject(API_URL) private apiUrl: string) {
  console.log(this.apiUrl); // https://api.example.com
}
```

**Alternatives de providers :**

```typescript
// useClass - fournir une classe
{ provide: LoggerService, useClass: ConsoleLoggerService }

// useValue - fournir une valeur
{ provide: API_URL, useValue: 'https://api.example.com' }

// useFactory - créer avec une fonction
{
  provide: UserService,
  useFactory: (http: HttpClient, config: AppConfig) => {
    return new UserService(http, config.apiUrl);
  },
  deps: [HttpClient, AppConfig]
}

// useExisting - alias vers un provider existant
{ provide: NewLogger, useExisting: LoggerService }
```

---

## Routing & Navigation

### Configuration de Base

```typescript
// app-routing.module.ts
import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';

import { HomeComponent } from './components/home/home.component';
import { UserListComponent } from './components/user-list/user-list.component';
import { UserDetailComponent } from './components/user-detail/user-detail.component';
import { NotFoundComponent } from './components/not-found/not-found.component';
import { AuthGuard } from './guards/auth.guard';

const routes: Routes = [
  // Route simple
  { path: '', component: HomeComponent },
  
  // Route avec paramètre
  { path: 'users/:id', component: UserDetailComponent },
  
  // Route avec guard (protection)
  { 
    path: 'admin', 
    component: AdminComponent,
    canActivate: [AuthGuard]
  },
  
  // Routes enfants
  {
    path: 'users',
    component: UserListComponent,
    children: [
      { path: ':id', component: UserDetailComponent },
      { path: ':id/edit', component: UserEditComponent }
    ]
  },
  
  // Lazy loading (chargement différé)
  {
    path: 'products',
    loadChildren: () => import('./features/products/products.module')
      .then(m => m.ProductsModule)
  },
  
  // Redirect
  { path: 'home', redirectTo: '', pathMatch: 'full' },
  
  // Wildcard - 404
  { path: '**', component: NotFoundComponent }
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
```

### Navigation dans le Template

```html
<!-- app.component.html -->
<nav>
  <!-- routerLink - navigation déclarative -->
  <a routerLink="/">Accueil</a>
  <a routerLink="/users">Utilisateurs</a>
  <a [routerLink]="['/users', userId]">Détail User</a>
  
  <!-- routerLinkActive - classe CSS si route active -->
  <a routerLink="/users" 
     routerLinkActive="active"
     [routerLinkActiveOptions]="{exact: true}">
    Utilisateurs
  </a>
</nav>

<!-- router-outlet - où afficher les components -->
<router-outlet></router-outlet>
```

### Navigation Programmatique

```typescript
import { Router, ActivatedRoute } from '@angular/router';

export class UserListComponent {
  constructor(
    private router: Router,
    private route: ActivatedRoute
  ) {}
  
  navigateToUser(userId: number): void {
    // Navigation simple
    this.router.navigate(['/users', userId]);
    
    // Navigation relative à la route actuelle
    this.router.navigate(['../detail', userId], { relativeTo: this.route });
    
    // Avec query params
    this.router.navigate(['/users'], {
      queryParams: { page: 1, sort: 'name' }
      // URL: /users?page=1&sort=name
    });
    
    // Avec fragment (anchor)
    this.router.navigate(['/users'], {
      fragment: 'section-2'
      // URL: /users#section-2
    });
    
    // navigateByUrl - URL complète
    this.router.navigateByUrl('/users/123');
  }
}
```

### Récupération des Paramètres

```typescript
import { ActivatedRoute, ParamMap } from '@angular/router';
import { switchMap } from 'rxjs/operators';

export class UserDetailComponent implements OnInit {
  user$!: Observable<User>;
  
  constructor(
    private route: ActivatedRoute,
    private userService: UserService
  ) {}
  
  ngOnInit(): void {
    // Méthode 1: Snapshot (valeur instantanée)
    const id = this.route.snapshot.paramMap.get('id');
    if (id) {
      this.userService.getUserById(+id).subscribe(user => {
        this.user = user;
      });
    }
    
    // Méthode 2: Observable (réactif, recommandé)
    this.user$ = this.route.paramMap.pipe(
      switchMap((params: ParamMap) => {
        const id = params.get('id');
        return this.userService.getUserById(+id!);
      })
    );
    
    // Query params
    this.route.queryParamMap.subscribe(params => {
      const page = params.get('page');
      const sort = params.get('sort');
      console.log('Page:', page, 'Sort:', sort);
    });
    
    // Fragment
    this.route.fragment.subscribe(fragment => {
      console.log('Fragment:', fragment);
    });
    
    // Tous les data
    this.route.data.subscribe(data => {
      console.log('Route data:', data);
    });
  }
}
```

### Route Guards

Les guards protègent les routes (authentification, autorisation, etc.)

```typescript
// auth.guard.ts
import { Injectable } from '@angular/core';
import { 
  CanActivate, 
  ActivatedRouteSnapshot, 
  RouterStateSnapshot, 
  Router,
  UrlTree 
} from '@angular/router';
import { Observable } from 'rxjs';
import { AuthService } from '../services/auth.service';

@Injectable({
  providedIn: 'root'
})
export class AuthGuard implements CanActivate {
  
  constructor(
    private authService: AuthService,
    private router: Router
  ) {}
  
  canActivate(
    route: ActivatedRouteSnapshot,
    state: RouterStateSnapshot
  ): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
    
    if (this.authService.isLoggedIn()) {
      return true;
    }
    
    // Redirection vers login avec returnUrl
    return this.router.createUrlTree(['/login'], {
      queryParams: { returnUrl: state.url }
    });
  }
}

// role.guard.ts - Guard avec paramètres
@Injectable({
  providedIn: 'root'
})
export class RoleGuard implements CanActivate {
  
  constructor(
    private authService: AuthService,
    private router: Router
  ) {}
  
  canActivate(
    route: ActivatedRouteSnapshot,
    state: RouterStateSnapshot
  ): boolean {
    const requiredRole = route.data['role'];
    const userRole = this.authService.getUserRole();
    
    if (userRole === requiredRole) {
      return true;
    }
    
    this.router.navigate(['/unauthorized']);
    return false;
  }
}

// Utilisation dans routes
const routes: Routes = [
  {
    path: 'admin',
    component: AdminComponent,
    canActivate: [AuthGuard, RoleGuard],
    data: { role: 'admin' }
  }
];
```

**Autres types de guards :**

```typescript
// CanDeactivate - confirmation avant quitter
export interface CanComponentDeactivate {
  canDeactivate: () => Observable<boolean> | Promise<boolean> | boolean;
}

@Injectable({ providedIn: 'root' })
export class UnsavedChangesGuard implements CanDeactivate<CanComponentDeactivate> {
  canDeactivate(
    component: CanComponentDeactivate
  ): Observable<boolean> | Promise<boolean> | boolean {
    return component.canDeactivate ? component.canDeactivate() : true;
  }
}

// Dans le component
export class EditUserComponent implements CanComponentDeactivate {
  hasUnsavedChanges = false;
  
  canDeactivate(): boolean {
    if (this.hasUnsavedChanges) {
      return confirm('Vous avez des modifications non sauvegardées. Voulez-vous quitter ?');
    }
    return true;
  }
}

// CanLoad - empêcher chargement lazy module
@Injectable({ providedIn: 'root' })
export class AuthGuard implements CanLoad {
  canLoad(): boolean {
    return this.authService.isLoggedIn();
  }
}
```

### Resolvers

Les resolvers chargent les données avant d'afficher la route.

```typescript
// user.resolver.ts
import { Injectable } from '@angular/core';
import { 
  Resolve, 
  ActivatedRouteSnapshot, 
  RouterStateSnapshot 
} from '@angular/router';
import { Observable } from 'rxjs';
import { UserService } from '../services/user.service';

@Injectable({
  providedIn: 'root'
})
export class UserResolver implements Resolve<User> {
  
  constructor(private userService: UserService) {}
  
  resolve(
    route: ActivatedRouteSnapshot,
    state: RouterStateSnapshot
  ): Observable<User> {
    const id = route.paramMap.get('id');
    return this.userService.getUserById(+id!);
  }
}

// Configuration route
const routes: Routes = [
  {
    path: 'users/:id',
    component: UserDetailComponent,
    resolve: {
      user: UserResolver // data sera disponible sous 'user'
    }
  }
];

// Dans le component
export class UserDetailComponent implements OnInit {
  user!: User;
  
  constructor(private route: ActivatedRoute) {}
  
  ngOnInit(): void {
    // Données déjà chargées par le resolver
    this.route.data.subscribe(data => {
      this.user = data['user'];
    });
  }
}
```

---

## HTTP & Communication Backend

### HttpClient Service

```typescript
import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders, HttpParams, HttpErrorResponse } from '@angular/common/http';
import { Observable, throwError } from 'rxjs';
import { catchError, retry, map, tap } from 'rxjs/operators';

@Injectable({
  providedIn: 'root'
})
export class ApiService {
  private baseUrl = 'https://api.example.com';
  
  constructor(private http: HttpClient) {}
  
  // GET simple
  getUsers(): Observable<User[]> {
    return this.http.get<User[]>(`${this.baseUrl}/users`);
  }
  
  // GET avec paramètres
  getUsersFiltered(role: string, page: number): Observable<User[]> {
    const params = new HttpParams()
      .set('role', role)
      .set('page', page.toString())
      .set('limit', '10');
    
    return this.http.get<User[]>(`${this.baseUrl}/users`, { params });
    // URL: /users?role=admin&page=1&limit=10
  }
  
  // GET avec headers personnalisés
  getProtectedData(): Observable<any> {
    const headers = new HttpHeaders({
      'Authorization': 'Bearer ' + this.getToken(),
      'Content-Type': 'application/json'
    });
    
    return this.http.get(`${this.baseUrl}/protected`, { headers });
  }
  
  // GET avec response complète
  getUserWithHeaders(id: number): Observable<any> {
    return this.http.get(`${this.baseUrl}/users/${id}`, {
      observe: 'response' // Obtenir HttpResponse complète
    }).pipe(
      map(response => {
        console.log('Headers:', response.headers);
        console.log('Status:', response.status);
        return response.body;
      })
    );
  }
  
  // POST
  createUser(user: Partial<User>): Observable<User> {
    return this.http.post<User>(`${this.baseUrl}/users`, user).pipe(
      tap(newUser => console.log('Utilisateur créé:', newUser)),
      catchError(this.handleError)
    );
  }
  
  // PUT
  updateUser(id: number, user: Partial<User>): Observable<User> {
    return this.http.put<User>(`${this.baseUrl}/users/${id}`, user).pipe(
      retry(2), // Réessayer 2 fois en cas d'erreur
      catchError(this.handleError)
    );
  }
  
  // PATCH
  patchUser(id: number, changes: Partial<User>): Observable<User> {
    return this.http.patch<User>(`${this.baseUrl}/users/${id}`, changes);
  }
  
  // DELETE
  deleteUser(id: number): Observable<void> {
    return this.http.delete<void>(`${this.baseUrl}/users/${id}`);
  }
  
  // Upload de fichier
  uploadFile(file: File): Observable<any> {
    const formData = new FormData();
    formData.append('file', file, file.name);
    
    return this.http.post(`${this.baseUrl}/upload`, formData, {
      reportProgress: true, // Pour track la progression
      observe: 'events'     // Observer tous les événements
    });
  }
  
  // Gestion des erreurs
  private handleError(error: HttpErrorResponse) {
    let errorMessage = 'Une erreur est survenue';
    
    if (error.error instanceof ErrorEvent) {
      // Erreur côté client
      errorMessage = `Erreur: ${error.error.message}`;
    } else {
      // Erreur côté serveur
      errorMessage = `Code: ${error.status}, Message: ${error.message}`;
      
      switch (error.status) {
        case 400:
          errorMessage = 'Requête invalide';
          break;
        case 401:
          errorMessage = 'Non autorisé';
          break;
        case 404:
          errorMessage = 'Ressource non trouvée';
          break;
        case 500:
          errorMessage = 'Erreur serveur';
          break;
      }
    }
    
    console.error(errorMessage);
    return throwError(() => new Error(errorMessage));
  }
  
  private getToken(): string {
    return localStorage.getItem('token') || '';
  }
}
```

### Interceptors

Les interceptors interceptent les requêtes/réponses HTTP pour ajouter des fonctionnalités globales.

```typescript
// auth.interceptor.ts - Ajouter token automatiquement
import { Injectable } from '@angular/core';
import {
  HttpInterceptor,
  HttpRequest,
  HttpHandler,
  HttpEvent
} from '@angular/common/http';
import { Observable } from 'rxjs';
import { AuthService } from '../services/auth.service';

@Injectable()
export class AuthInterceptor implements HttpInterceptor {
  
  constructor(private authService: AuthService) {}
  
  intercept(
    req: HttpRequest<any>,
    next: HttpHandler
  ): Observable<HttpEvent<any>> {
    
    // Cloner la requête et ajouter le header
    const token = this.authService.getToken();
    
    if (token) {
      const clonedReq = req.clone({
        headers: req.headers.set('Authorization', `Bearer ${token}`)
      });
      return next.handle(clonedReq);
    }
    
    return next.handle(req);
  }
}

// error.interceptor.ts - Gérer les erreurs globalement
@Injectable()
export class ErrorInterceptor implements HttpInterceptor {
  
  constructor(
    private router: Router,
    private notificationService: NotificationService
  ) {}
  
  intercept(
    req: HttpRequest<any>,
    next: HttpHandler
  ): Observable<HttpEvent<any>> {
    
    return next.handle(req).pipe(
      catchError((error: HttpErrorResponse) => {
        
        if (error.status === 401) {
          // Rediriger vers login si non autorisé
          this.router.navigate(['/login']);
        }
        
        if (error.status === 403) {
          this.notificationService.error('Accès refusé');
        }
        
        if (error.status === 500) {
          this.notificationService.error('Erreur serveur');
        }
        
        return throwError(() => error);
      })
    );
  }
}

// loading.interceptor.ts - Afficher loader global
@Injectable()
export class LoadingInterceptor implements HttpInterceptor {
  
  constructor(private loadingService: LoadingService) {}
  
  intercept(
    req: HttpRequest<any>,
    next: HttpHandler
  ): Observable<HttpEvent<any>> {
    
    this.loadingService.show();
    
    return next.handle(req).pipe(
      finalize(() => {
        this.loadingService.hide();
      })
    );
  }
}

// Enregistrement dans app.module.ts
import { HTTP_INTERCEPTORS } from '@angular/common/http';

@NgModule({
  providers: [
    {
      provide: HTTP_INTERCEPTORS,
      useClass: AuthInterceptor,
      multi: true // Important: permet plusieurs interceptors
    },
    {
      provide: HTTP_INTERCEPTORS,
      useClass: ErrorInterceptor,
      multi: true
    },
    {
      provide: HTTP_INTERCEPTORS,
      useClass: LoadingInterceptor,
      multi: true
    }
  ]
})
export class AppModule { }
```

### Utilisation dans Components

```typescript
export class UserListComponent implements OnInit, OnDestroy {
  users: User[] = [];
  loading = false;
  error: string | null = null;
  
  private destroy$ = new Subject<void>();
  
  constructor(private userService: UserService) {}
  
  ngOnInit(): void {
    this.loadUsers();
  }
  
  loadUsers(): void {
    this.loading = true;
    this.error = null;
    
    this.userService.getUsers()
      .pipe(
        takeUntil(this.destroy$) // Unsubscribe automatique
      )
      .subscribe({
        next: (users) => {
          this.users = users;
          this.loading = false;
        },
        error: (error) => {
          this.error = error.message;
          this.loading = false;
        }
      });
  }
  
  createUser(userData: Partial<User>): void {
    this.userService.createUser(userData)
      .pipe(takeUntil(this.destroy$))
      .subscribe({
        next: (newUser) => {
          this.users.push(newUser);
          console.log('Utilisateur créé avec succès');
        },
        error: (error) => {
          console.error('Erreur lors de la création', error);
        }
      });
  }
  
  deleteUser(id: number): void {
    if (!confirm('Confirmer la suppression ?')) return;
    
    this.userService.deleteUser(id)
      .pipe(takeUntil(this.destroy$))
      .subscribe({
        next: () => {
          this.users = this.users.filter(u => u.id !== id);
        },
        error: (error) => {
          console.error('Erreur lors de la suppression', error);
        }
      });
  }
  
  ngOnDestroy(): void {
    this.destroy$.next();
    this.destroy$.complete();
  }
}
```

---

## Forms & Validation

Angular propose deux approches pour les formulaires : **Template-driven** et **Reactive**.

### Template-Driven Forms

Approche déclarative, logique dans le template (similaire à Angular.js).

```typescript
// user-form.component.ts
import { Component } from '@angular/core';

@Component({
  selector: 'app-user-form',
  templateUrl: './user-form.component.html'
})
export class UserFormComponent {
  model = {
    name: '',
    email: '',
    age: null,
    role: 'user'
  };
  
  roles = ['user', 'admin', 'moderator'];
  
  onSubmit(): void {
    console.log('Form submitted:', this.model);
  }
}
```

```html
<!-- user-form.component.html -->
<form #userForm="ngForm" (ngSubmit)="onSubmit()">
  
  <!-- Input text avec validation -->
  <div>
    <label for="name">Nom:</label>
    <input 
      type="text" 
      id="name"
      name="name"
      [(ngModel)]="model.name"
      required
      minlength="3"
      #name="ngModel">
    
    <!-- Messages d'erreur -->
    <div *ngIf="name.invalid && (name.dirty || name.touched)">
      <small *ngIf="name.errors?.['required']">Le nom est requis</small>
      <small *ngIf="name.errors?.['minlength']">
        Minimum 3 caractères (actuel: {{ name.value?.length }})
      </small>
    </div>
  </div>
  
  <!-- Email avec validation -->
  <div>
    <label for="email">Email:</label>
    <input 
      type="email" 
      id="email"
      name="email"
      [(ngModel)]="model.email"
      required
      email
      #email="ngModel">
    
    <div *ngIf="email.invalid && email.touched">
      <small *ngIf="email.errors?.['required']">Email requis</small>
      <small *ngIf="email.errors?.['email']">Email invalide</small>
    </div>
  </div>
  
  <!-- Number -->
  <div>
    <label for="age">Âge:</label>
    <input 
      type="number" 
      id="age"
      name="age"
      [(ngModel)]="model.age"
      min="18"
      max="100"
      #age="ngModel">
    
    <div *ngIf="age.invalid && age.touched">
      <small *ngIf="age.errors?.['min']">Âge minimum: 18</small>
      <small *ngIf="age.errors?.['max']">Âge maximum: 100</small>
    </div>
  </div>
  
  <!-- Select -->
  <div>
    <label for="role">Rôle:</label>
    <select 
      id="role" 
      name="role"
      [(ngModel)]="model.role"
      required>
      <option *ngFor="let role of roles" [value]="role">
        {{ role }}
      </option>
    </select>
  </div>
  
  <!-- Submit button -->
  <button 
    type="submit" 
    [disabled]="userForm.invalid">
    Soumettre
  </button>
  
  <!-- Debug -->
  <pre>Form valid: {{ userForm.valid }}</pre>
  <pre>Model: {{ model | json }}</pre>
</form>
```

### Reactive Forms (Recommandé)

Approche programmatique, logique dans le component (plus puissant, testable).

```typescript
// user-form.component.ts
import { Component, OnInit } from '@angular/core';
import { 
  FormBuilder, 
  FormGroup, 
  FormControl, 
  Validators,
  AbstractControl,
  ValidationErrors
} from '@angular/forms';

@Component({
  selector: 'app-user-form',
  templateUrl: './user-form.component.html'
})
export class UserFormComponent implements OnInit {
  userForm!: FormGroup;
  
  roles = ['user', 'admin', 'moderator'];
  
  constructor(private fb: FormBuilder) {}
  
  ngOnInit(): void {
    this.buildForm();
  }
  
  // Méthode 1: avec FormBuilder (recommandé)
  buildForm(): void {
    this.userForm = this.fb.group({
      name: ['', [
        Validators.required,
        Validators.minLength(3),
        Validators.maxLength(50)
      ]],
      email: ['', [
        Validators.required,
        Validators.email
      ]],
      age: [null, [
        Validators.required,
        Validators.min(18),
        Validators.max(100)
      ]],
      role: ['user', Validators.required],
      password: ['', [
        Validators.required,
        Validators.minLength(8),
        this.passwordValidator // Validator personnalisé
      ]],
      confirmPassword: ['', Validators.required],
      address: this.fb.group({ // FormGroup imbriqué
        street: [''],
        city: ['', Validators.required],
        zipCode: ['', [Validators.required, Validators.pattern(/^\d{5}$/)]]
      }),
      hobbies: this.fb.array([]) // FormArray
    }, {
      validators: this.passwordMatchValidator // Validator au niveau form
    });
    
    // Écouter les changements
    this.userForm.get('role')?.valueChanges.subscribe(role => {
      console.log('Role changed to:', role);
    });
  }
  
  // Méthode 2: avec FormControl directement
  buildFormManual(): void {
    this.userForm = new FormGroup({
      name: new FormControl('', [Validators.required]),
      email: new FormControl('', [Validators.required, Validators.email])
    });
  }
  
  // Validator personnalisé - niveau control
  passwordValidator(control: AbstractControl): ValidationErrors | null {
    const value = control.value;
    
    if (!value) return null;
    
    const hasNumber = /[0-9]/.test(value);
    const hasUpper = /[A-Z]/.test(value);
    const hasLower = /[a-z]/.test(value);
    const hasSpecial = /[!@#$%^&*]/.test(value);
    
    const valid = hasNumber && hasUpper && hasLower && hasSpecial;
    
    return valid ? null : {
      passwordStrength: {
        hasNumber,
        hasUpper,
        hasLower,
        hasSpecial
      }
    };
  }
  
  // Validator personnalisé - niveau form
  passwordMatchValidator(form: AbstractControl): ValidationErrors | null {
    const password = form.get('password')?.value;
    const confirmPassword = form.get('confirmPassword')?.value;
    
    return password === confirmPassword ? null : { passwordMismatch: true };
  }
  
  // Getters pour accès facile dans template
  get name() { return this.userForm.get('name'); }
  get email() { return this.userForm.get('email'); }
  get password() { return this.userForm.get('password'); }
  get city() { return this.userForm.get('address.city'); }
  
  // Gestion FormArray
  get hobbies() {
    return this.userForm.get('hobbies') as FormArray;
  }
  
  addHobby(): void {
    const hobby = this.fb.control('', Validators.required);
    this.hobbies.push(hobby);
  }
  
  removeHobby(index: number): void {
    this.hobbies.removeAt(index);
  }
  
  // Submit
  onSubmit(): void {
    if (this.userForm.invalid) {
      this.userForm.markAllAsTouched(); // Afficher toutes les erreurs
      return;
    }
    
    console.log('Form values:', this.userForm.value);
    console.log('Form raw values:', this.userForm.getRawValue()); // Inclut disabled
    
    // Réinitialiser
    this.userForm.reset();
  }
  
  // Méthodes utilitaires
  patchValues(): void {
    this.userForm.patchValue({
      name: 'John Doe',
      email: 'john@test.com'
    });
  }
  
  setValues(): void {
    this.userForm.setValue({
      name: 'John',
      email: 'john@test.com',
      age: 30,
      role: 'user',
      password: '',
      confirmPassword: '',
      address: { street: '', city: '', zipCode: '' },
      hobbies: []
    });
  }
  
  disableField(): void {
    this.userForm.get('email')?.disable();
  }
  
  enableField(): void {
    this.userForm.get('email')?.enable();
  }
}
```

```html
<!-- user-form.component.html -->
<form [formGroup]="userForm" (ngSubmit)="onSubmit()">
  
  <!-- Input simple -->
  <div>
    <label>Nom:</label>
    <input type="text" formControlName="name">
    
    <div *ngIf="name?.invalid && name?.touched">
      <small *ngIf="name?.errors?.['required']">Requis</small>
      <small *ngIf="name?.errors?.['minlength']">
        Min {{ name?.errors?.['minlength'].requiredLength }} caractères
      </small>
    </div>
  </div>
  
  <!-- Email -->
  <div>
    <label>Email:</label>
    <input type="email" formControlName="email">
    
    <div *ngIf="email?.invalid && email?.touched">
      <small *ngIf="email?.errors?.['required']">Requis</small>
      <small *ngIf="email?.errors?.['email']">Email invalide</small>
    </div>
  </div>
  
  <!-- Password avec validator personnalisé -->
  <div>
    <label>Mot de passe:</label>
    <input type="password" formControlName="password">
    
    <div *ngIf="password?.errors?.['passwordStrength'] && password?.touched">
      <small>Le mot de passe doit contenir:</small>
      <ul>
        <li [class.valid]="password?.errors?.['passwordStrength'].hasNumber">
          Un chiffre
        </li>
        <li [class.valid]="password?.errors?.['passwordStrength'].hasUpper">
          Une majuscule
        </li>
        <li [class.valid]="password?.errors?.['passwordStrength'].hasLower">
          Une minuscule
        </li>
        <li [class.valid]="password?.errors?.['passwordStrength'].hasSpecial">
          Un caractère spécial
        </li>
      </ul>
    </div>
  </div>
  
  <!-- Confirm password -->
  <div>
    <label>Confirmer:</label>
    <input type="password" formControlName="confirmPassword">
    
    <div *ngIf="userForm.errors?.['passwordMismatch'] && userForm.touched">
      <small>Les mots de passe ne correspondent pas</small>
    </div>
  </div>
  
  <!-- FormGroup imbriqué -->
  <div formGroupName="address">
    <h3>Adresse</h3>
    <input type="text" formControlName="street" placeholder="Rue">
    <input type="text" formControlName="city" placeholder="Ville">
    <input type="text" formControlName="zipCode" placeholder="Code postal">
    
    <div *ngIf="city?.invalid && city?.touched">
      <small>Ville requise</small>
    </div>
  </div>
  
  <!-- FormArray -->
  <div formArrayName="hobbies">
    <h3>Hobbies</h3>
    <div *ngFor="let hobby of hobbies.controls; let i = index">
      <input type="text" [formControlName]="i">
      <button type="button" (click)="removeHobby(i)">Supprimer</button>
    </div>
    <button type="button" (click)="addHobby()">Ajouter hobby</button>
  </div>
  
  <!-- Select -->
  <div>
    <label>Rôle:</label>
    <select formControlName="role">
      <option *ngFor="let role of roles" [value]="role">
        {{ role }}
      </option>
    </select>
  </div>
  
  <!-- Submit -->
  <button type="submit" [disabled]="userForm.invalid">
    Soumettre
  </button>
  
  <!-- Debug -->
  <pre>Valid: {{ userForm.valid }}</pre>
  <pre>Values: {{ userForm.value | json }}</pre>
  <pre>Errors: {{ userForm.errors | json }}</pre>
</form>
```

### Validators Async

Pour valider côté serveur (ex: vérifier si email existe).

```typescript
// email-validator.service.ts
@Injectable({ providedIn: 'root' })
export class EmailValidatorService {
  
  constructor(private http: HttpClient) {}
  
  validateEmail(control: AbstractControl): Observable<ValidationErrors | null> {
    if (!control.value) {
      return of(null);
    }
    
    return this.http.get(`/api/check-email/${control.value}`).pipe(
      map((response: any) => {
        return response.exists ? { emailTaken: true } : null;
      }),
      debounceTime(500), // Attendre 500ms après dernière saisie
      distinctUntilChanged(),
      first()
    );
  }
}

// Utilisation
constructor(
  private fb: FormBuilder,
  private emailValidator: EmailValidatorService
) {}

buildForm(): void {
  this.userForm = this.fb.group({
    email: ['', 
      [Validators.required, Validators.email], // Validators sync
      [this.emailValidator.validateEmail.bind(this.emailValidator)] // Async
    ]
  });
}
```

---

## RxJS & Observables

### Qu'est-ce que RxJS ?

**Reactive Extensions for JavaScript** - Bibliothèque pour la programmation réactive avec des Observables.

**Concepts clés :**
- **Observable** : flux de données asynchrone (comme une Promise multiple)
- **Observer** : consomme les données (via `subscribe()`)
- **Operators** : transforment les flux (map, filter, etc.)
- **Subject** : Observable + Observer (peut émettre ET recevoir)

### Créer des Observables

```typescript
import { Observable, of, from, interval, fromEvent } from 'rxjs';

// of - créer depuis valeurs
const obs1$ = of(1, 2, 3, 4, 5);
obs1$.subscribe(x => console.log(x)); // 1, 2, 3, 4, 5

// from - créer depuis array, promise, iterable
const obs2$ = from([10, 20, 30]);
const obs3$ = from(fetch('/api/data'));

// interval - émet toutes les X ms
const obs4$ = interval(1000); // 0, 1, 2, 3... chaque seconde

// fromEvent - depuis événement DOM
const clicks$ = fromEvent(document, 'click');

// create - personnalisé
const custom$ = new Observable(observer => {
  observer.next('Hello');
  observer.next('World');
  observer.complete();
  
  // Cleanup
  return () => {
    console.log('Unsubscribed');
  };
});
```

### Subscribe aux Observables

```typescript
import { Subscription } from 'rxjs';

const subscription: Subscription = obs1$.subscribe({
  next: (value) => console.log('Valeur:', value),
  error: (error) => console.error('Erreur:', error),
  complete: () => console.log('Terminé')
});

// Ou avec fonctions séparées
obs1$.subscribe(
  value => console.log(value),      // next
  error => console.error(error),    // error
  () => console.log('Terminé')      // complete
);

// Unsubscribe pour éviter memory leaks
subscription.unsubscribe();

// Pattern Angular pour auto-unsubscribe
export class MyComponent implements OnInit, OnDestroy {
  private destroy$ = new Subject<void>();
  
  ngOnInit(): void {
    this.dataService.getData()
      .pipe(takeUntil(this.destroy$))
      .subscribe(data => {
        console.log(data);
      });
  }
  
  ngOnDestroy(): void {
    this.destroy$.next();
    this.destroy$.complete();
  }
}
```

### Operators Principaux

```typescript
import { 
  map, filter, tap, take, first, debounceTime, distinctUntilChanged,
  switchMap, mergeMap, concatMap, exhaustMap,
  catchError, retry, finalize,
  combineLatest, forkJoin, merge, concat
} from 'rxjs/operators';

// MAP - transformer les valeurs
of(1, 2, 3).pipe(
  map(x => x * 10)
).subscribe(console.log); // 10, 20, 30

// FILTER - filtrer les valeurs
of(1, 2, 3, 4, 5).pipe(
  filter(x => x % 2 === 0)
).subscribe(console.log); // 2, 4

// TAP - effet de bord sans modifier
of(1, 2, 3).pipe(
  tap(x => console.log('Avant:', x)),
  map(x => x * 2),
  tap(x => console.log('Après:', x))
).subscribe();

// TAKE - prendre N valeurs puis complete
interval(1000).pipe(
  take(5)
).subscribe(console.log); // 0, 1, 2, 3, 4 puis complete

// FIRST - prendre la première valeur qui match
of(1, 2, 3, 4, 5).pipe(
  first(x => x > 3)
).subscribe(console.log); // 4

// DEBOUNCE TIME - attendre X ms après dernière émission
fromEvent(input, 'input').pipe(
  map((e: any) => e.target.value),
  debounceTime(500) // Attendre 500ms d'inactivité
).subscribe(console.log);

// DISTINCT UNTIL CHANGED - ignorer valeurs identiques consécutives
of(1, 1, 2, 2, 3, 3, 2).pipe(
  distinctUntilChanged()
).subscribe(console.log); // 1, 2, 3, 2

// SWITCH MAP - annuler observable précédent (pour recherche)
searchInput$.pipe(
  debounceTime(300),
  distinctUntilChanged(),
  switchMap(term => this.apiService.search(term))
).subscribe(results => {
  this.searchResults = results;
});

// MERGE MAP - exécuter en parallèle sans annuler
userIds$.pipe(
  mergeMap(id => this.userService.getUser(id))
).subscribe(user => console.log(user));

// CONCAT MAP - exécuter séquentiellement
userIds$.pipe(
  concatMap(id => this.userService.getUser(id))
).subscribe(user => console.log(user));

// EXHAUST MAP - ignorer nouvelles émissions tant que précédente active
saveButton$.pipe(
  exhaustMap(() => this.apiService.save(this.data))
).subscribe(result => console.log('Saved'));

// CATCH ERROR - gérer les erreurs
this.http.get('/api/data').pipe(
  catchError(error => {
    console.error('Erreur:', error);
    return of([]); // Retourner valeur par défaut
  })
).subscribe(data => console.log(data));

// RETRY - réessayer en cas d'erreur
this.http.get('/api/data').pipe(
  retry(3) // Réessayer 3 fois
).subscribe();

// FINALIZE - exécuter à la fin (succès ou erreur)
this.http.get('/api/data').pipe(
  finalize(() => {
    this.loading = false;
    console.log('Requête terminée');
  })
).subscribe();
```

### Combination Operators

```typescript
import { combineLatest, forkJoin, merge, concat, zip } from 'rxjs';

// COMBINE LATEST - émet quand n'importe quelle source émet
const age$ = of(25);
const name$ = of('John');
const city$ = of('Paris');

combineLatest([age$, name$, city$]).subscribe(
  ([age, name, city]) => {
    console.log(`${name}, ${age} ans, habite à ${city}`);
  }
);

// FORK JOIN - attendre que toutes sources completent (comme Promise.all)
forkJoin({
  users: this.userService.getUsers(),
  products: this.productService.getProducts(),
  orders: this.orderService.getOrders()
}).subscribe(({ users, products, orders }) => {
  console.log('Tout est chargé', users, products, orders);
});

// MERGE - fusionner plusieurs observables
merge(
  clicks$.pipe(map(() => 'click')),
  hovers$.pipe(map(() => 'hover'))
).subscribe(event => console.log(event));

// CONCAT - émettre séquentiellement
concat(
  of(1, 2, 3),
  of(4, 5, 6)
).subscribe(console.log); // 1, 2, 3, 4, 5, 6

// ZIP - combiner par index
zip(
  of('A', 'B', 'C'),
  of(1, 2, 3)
).subscribe(console.log); // ['A', 1], ['B', 2], ['C', 3]
```

### Subjects

Les Subjects sont à la fois Observable et Observer.

```typescript
import { Subject, BehaviorSubject, ReplaySubject, AsyncSubject } from 'rxjs';

// SUBJECT - simple multicast
const subject = new Subject<number>();

subject.subscribe(x => console.log('Observer 1:', x));
subject.subscribe(x => console.log('Observer 2:', x));

subject.next(1); // Les deux observent
subject.next(2);

// BEHAVIOR SUBJECT - garde dernière valeur + valeur initiale
const behaviorSubject = new BehaviorSubject<number>(0); // Valeur initiale

behaviorSubject.subscribe(x => console.log('A:', x)); // Reçoit 0 immédiatement

behaviorSubject.next(1);
behaviorSubject.next(2);

behaviorSubject.subscribe(x => console.log('B:', x)); // Reçoit 2 immédiatement

console.log('Valeur actuelle:', behaviorSubject.value); // 2

// REPLAY SUBJECT - rejoue N dernières valeurs
const replaySubject = new ReplaySubject<number>(2); // Garde 2 dernières

replaySubject.next(1);
replaySubject.next(2);
replaySubject.next(3);

replaySubject.subscribe(x => console.log('Observer:', x)); // Reçoit 2, 3

// ASYNC SUBJECT - émet uniquement la dernière valeur à complete
const asyncSubject = new AsyncSubject<number>();

asyncSubject.subscribe(x => console.log('Observer:', x));

asyncSubject.next(1);
asyncSubject.next(2);
asyncSubject.next(3);
asyncSubject.complete(); // Émet 3 maintenant
```

### Exemple Pratique : Service avec State Management

```typescript
@Injectable({ providedIn: 'root' })
export class CartService {
  private cartItemsSubject = new BehaviorSubject<CartItem[]>([]);
  public cartItems$ = this.cartItemsSubject.asObservable();
  
  private loadingSubject = new BehaviorSubject<boolean>(false);
  public loading$ = this.loadingSubject.asObservable();
  
  // Computed observables
  public totalItems$ = this.cartItems$.pipe(
    map(items => items.reduce((sum, item) => sum + item.quantity, 0))
  );
  
  public totalPrice$ = this.cartItems$.pipe(
    map(items => items.reduce((sum, item) => sum + (item.price * item.quantity), 0))
  );
  
  constructor(private http: HttpClient) {
    this.loadCart();
  }
  
  private loadCart(): void {
    this.loadingSubject.next(true);
    
    this.http.get<CartItem[]>('/api/cart').pipe(
      finalize(() => this.loadingSubject.next(false))
    ).subscribe(items => {
      this.cartItemsSubject.next(items);
    });
  }
  
  addItem(product: Product): void {
    const currentItems = this.cartItemsSubject.value;
    const existingItem = currentItems.find(i => i.productId === product.id);
    
    if (existingItem) {
      existingItem.quantity++;
      this.cartItemsSubject.next([...currentItems]);
    } else {
      const newItem: CartItem = {
        id: Date.now(),
        productId: product.id,
        name: product.name,
        price: product.price,
        quantity: 1
      };
      this.cartItemsSubject.next([...currentItems, newItem]);
    }
  }
  
  removeItem(itemId: number): void {
    const currentItems = this.cartItemsSubject.value;
    this.cartItemsSubject.next(currentItems.filter(i => i.id !== itemId));
  }
  
  clearCart(): void {
    this.cartItemsSubject.next([]);
  }
  
  getCurrentItems(): CartItem[] {
    return this.cartItemsSubject.value;
  }
}

// Utilisation dans component
export class CartComponent implements OnInit {
  cartItems$ = this.cartService.cartItems$;
  totalItems$ = this.cartService.totalItems$;
  totalPrice$ = this.cartService.totalPrice$;
  loading$ = this.cartService.loading$;
  
  constructor(private cartService: CartService) {}
  
  ngOnInit(): void {
    // Template utilise async pipe, pas besoin de subscribe
  }
  
  removeItem(id: number): void {
    this.cartService.removeItem(id);
  }
}
```

```html
<!-- cart.component.html -->
<div *ngIf="loading$ | async">Chargement...</div>

<div *ngIf="!(loading$ | async)">
  <h2>Panier ({{ totalItems$ | async }} articles)</h2>
  
  <div *ngFor="let item of cartItems$ | async">
    {{ item.name }} - {{ item.price }}€ x {{ item.quantity }}
    <button (click)="removeItem(item.id)">Supprimer</button>
  </div>
  
  <div class="total">
    Total: {{ totalPrice$ | async | currency:'EUR' }}
  </div>
</div>
```

---

## Directives & Pipes

### Directives Structurelles

Les directives structurelles modifient la structure du DOM (ajoutent/retirent éléments).

```html
<!-- *ngIf - affichage conditionnel -->
<div *ngIf="isLoggedIn">Bienvenue!</div>

<div *ngIf="user; else loading">
  {{ user.name }}
</div>
<ng-template #loading>
  <div>Chargement...</div>
</ng-template>

<!-- *ngIf avec as pour stocker résultat -->
<div *ngIf="users$ | async as users">
  Il y a {{ users.length }} utilisateurs
</div>

<!-- *ngFor - boucle -->
<div *ngFor="let user of users">
  {{ user.name }}
</div>

<!-- *ngFor avec index et autres variables -->
<div *ngFor="let user of users; let i = index; let first = first; let last = last; let even = even">
  {{ i + 1 }}. {{ user.name }}
  <span *ngIf="first">(Premier)</span>
  <span *ngIf="last">(Dernier)</span>
  <span [class.even]="even"></span>
</div>

<!-- *ngFor avec trackBy pour performance -->
<div *ngFor="let user of users; trackBy: trackByUserId">
  {{ user.name }}
</div>

<!-- trackBy function -->
trackByUserId(index: number, user: User): number {
  return user.id;
}

<!-- *ngSwitch - switch case -->
<div [ngSwitch]="userRole">
  <div *ngSwitchCase="'admin'">Panel Admin</div>
  <div *ngSwitchCase="'user'">Panel User</div>
  <div *ngSwitchCase="'guest'">Panel Guest</div>
  <div *ngSwitchDefault>Rôle inconnu</div>
</div>
```

### Directives d'Attribut

Les directives d'attribut modifient l'apparence ou le comportement d'un élément.

```html
<!-- ngClass - classes conditionnelles -->
<div [ngClass]="'active'">Simple string</div>
<div [ngClass]="['class1', 'class2']">Array</div>
<div [ngClass]="{ 'active': isActive, 'disabled': isDisabled }">Object</div>
<div [ngClass]="getClasses()">Function</div>

<!-- ngStyle - styles conditionnels -->
<div [ngStyle]="{ 'color': textColor, 'font-size': fontSize + 'px' }">
  Texte stylé
</div>

<!-- ngModel - two-way binding -->
<input [(ngModel)]="username">

<!-- Class binding -->
<div [class.active]="isActive"></div>
<div [class]="'my-class'"></div>

<!-- Style binding -->
<div [style.color]="textColor"></div>
<div [style.font-size.px]="fontSize"></div>
```

### Créer une Directive Personnalisée

```typescript
// highlight.directive.ts
import { Directive, ElementRef, HostListener, Input } from '@angular/core';

@Directive({
  selector: '[appHighlight]'
})
export class HighlightDirective {
  @Input() appHighlight: string = 'yellow'; // Couleur par défaut
  @Input() defaultColor: string = 'transparent';
  
  constructor(private el: ElementRef) {
    this.el.nativeElement.style.backgroundColor = this.defaultColor;
  }
  
  @HostListener('mouseenter') onMouseEnter() {
    this.highlight(this.appHighlight);
  }
  
  @HostListener('mouseleave') onMouseLeave() {
    this.highlight(this.defaultColor);
  }
  
  private highlight(color: string) {
    this.el.nativeElement.style.backgroundColor = color;
  }
}

// Utilisation
<p appHighlight="lightblue">Survolez-moi</p>
<p [appHighlight]="'pink'" defaultColor="lightgray">Survolez-moi aussi</p>
```

**Directive structurelle personnalisée :**

```typescript
// unless.directive.ts - inverse de ngIf
import { Directive, Input, TemplateRef, ViewContainerRef } from '@angular/core';

@Directive({
  selector: '[appUnless]'
})
export class UnlessDirective {
  private hasView = false;
  
  constructor(
    private templateRef: TemplateRef<any>,
    private viewContainer: ViewContainerRef
  ) {}
  
  @Input() set appUnless(condition: boolean) {
    if (!condition && !this.hasView) {
      this.viewContainer.createEmbeddedView(this.templateRef);
      this.hasView = true;
    } else if (condition && this.hasView) {
      this.viewContainer.clear();
      this.hasView = false;
    }
  }
}

// Utilisation
<div *appUnless="isLoggedIn">
  Veuillez vous connecter
</div>
```

### Pipes

Les pipes transforment les données dans le template.

```html
<!-- Pipes built-in -->

<!-- DatePipe -->
<p>{{ today | date }}</p>
<p>{{ today | date:'dd/MM/yyyy' }}</p>
<p>{{ today | date:'short' }}</p>
<p>{{ today | date:'fullDate':'':'fr' }}</p>

<!-- UpperCase / LowerCase / TitleCase -->
<p>{{ 'hello' | uppercase }}</p>  <!-- HELLO -->
<p>{{ 'HELLO' | lowercase }}</p>  <!-- hello -->
<p>{{ 'hello world' | titlecase }}</p>  <!-- Hello World -->

<!-- CurrencyPipe -->
<p>{{ 123.45 | currency }}</p>  <!-- $123.45 -->
<p>{{ 123.45 | currency:'EUR' }}</p>  <!-- €123.45 -->
<p>{{ 123.45 | currency:'EUR':'symbol':'1.2-2':'fr' }}</p>  <!-- 123,45 € -->

<!-- DecimalPipe -->
<p>{{ 3.14159 | number }}</p>  <!-- 3.142 -->
<p>{{ 3.14159 | number:'1.0-0' }}</p>  <!-- 3 -->
<p>{{ 3.14159 | number:'3.1-5' }}</p>  <!-- 003.14159 -->

<!-- PercentPipe -->
<p>{{ 0.25 | percent }}</p>  <!-- 25% -->
<p>{{ 0.259 | percent:'1.2-2' }}</p>  <!-- 25.90% -->

<!-- SlicePipe -->
<p>{{ 'Hello World' | slice:0:5 }}</p>  <!-- Hello -->
<p>{{ [1,2,3,4,5] | slice:1:4 }}</p>  <!-- [2,3,4] -->

<!-- JsonPipe - debug -->
<pre>{{ user | json }}</pre>

<!-- AsyncPipe - subscribe automatiquement -->
<div *ngIf="users$ | async as users">
  <p *ngFor="let user of users">{{ user.name }}</p>
</div>

<!-- Chaîner les pipes -->
<p>{{ today | date:'fullDate' | uppercase }}</p>
```

### Créer un Pipe Personnalisé

```typescript
// truncate.pipe.ts
import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
  name: 'truncate'
})
export class TruncatePipe implements PipeTransform {
  transform(value: string, limit: number = 50, ellipsis: string = '...'): string {
    if (!value) return '';
    if (value.length <= limit) return value;
    return value.substring(0, limit) + ellipsis;
  }
}

// Utilisation
<p>{{ longText | truncate:100 }}</p>
<p>{{ longText | truncate:50:'---' }}</p>
```

**Pipe avec paramètres multiples :**

```typescript
// highlight.pipe.ts
@Pipe({
  name: 'highlight'
})
export class HighlightPipe implements PipeTransform {
  transform(value: string, search: string, className: string = 'highlight'): string {
    if (!search) return value;
    
    const regex = new RegExp(search, 'gi');
    return value.replace(regex, match => 
      `<span class="${className}">${match}</span>`
    );
  }
}

// Utilisation (avec [innerHTML] pour HTML)
<p [innerHTML]="text | highlight:'Angular':'highlight-yellow'"></p>
```

**Pipe impure (se déclenche à chaque change detection) :**

```typescript
@Pipe({
  name: 'filter',
  pure: false  // Impure pipe
})
export class FilterPipe implements PipeTransform {
  transform(items: any[], searchText: string, property: string): any[] {
    if (!items) return [];
    if (!searchText) return items;
    
    searchText = searchText.toLowerCase();
    return items.filter(item => {
      return item[property].toLowerCase().includes(searchText);
    });
  }
}

// Utilisation
<div *ngFor="let user of users | filter:searchTerm:'name'">
  {{ user.name }}
</div>
```

**Async pipe personnalisé :**

```typescript
@Pipe({
  name: 'loading'
})
export class LoadingPipe implements PipeTransform {
  transform(observable: Observable<any>, loadingText: string = 'Chargement...'): Observable<any> {
    return observable.pipe(
      startWith({ loading: true, data: null }),
      map(data => ({ loading: false, data })),
      catchError(error => of({ loading: false, error }))
    );
  }
}
```

---

## State Management

### Approche Simple avec Services

Pour petites/moyennes applications.

```typescript
// state.service.ts
@Injectable({ providedIn: 'root' })
export class StateService {
  // State
  private stateSubject = new BehaviorSubject<AppState>({
    users: [],
    selectedUser: null,
    loading: false,
    error: null
  });
  
  public state$ = this.stateSubject.asObservable();
  
  // Selectors
  public users$ = this.state$.pipe(
    map(state => state.users),
    distinctUntilChanged()
  );
  
  public selectedUser$ = this.state$.pipe(
    map(state => state.selectedUser),
    distinctUntilChanged()
  );
  
  public loading$ = this.state$.pipe(
    map(state => state.loading),
    distinctUntilChanged()
  );
  
  // Actions
  setLoading(loading: boolean): void {
    this.updateState({ loading });
  }
  
  setUsers(users: User[]): void {
    this.updateState({ users, loading: false });
  }
  
  selectUser(user: User): void {
    this.updateState({ selectedUser: user });
  }
  
  setError(error: string): void {
    this.updateState({ error, loading: false });
  }
  
  // Helper privé
  private updateState(partial: Partial<AppState>): void {
    const currentState = this.stateSubject.value;
    this.stateSubject.next({ ...currentState, ...partial });
  }
  
  // Getter synchrone
  getCurrentState(): AppState {
    return this.stateSubject.value;
  }
}

interface AppState {
  users: User[];
  selectedUser: User | null;
  loading: boolean;
  error: string | null;
}
```

### NgRx (Redux pattern)

Pour grandes applications complexes.

```bash
npm install @ngrx/store @ngrx/effects @ngrx/entity @ngrx/store-devtools
```

**1. Actions :**

```typescript
// user.actions.ts
import { createAction, props } from '@ngrx/store';

export const loadUsers = createAction('[User List] Load Users');

export const loadUsersSuccess = createAction(
  '[User API] Load Users Success',
  props<{ users: User[] }>()
);

export const loadUsersFailure = createAction(
  '[User API] Load Users Failure',
  props<{ error: string }>()
);

export const selectUser = createAction(
  '[User Detail] Select User',
  props<{ userId: number }>()
);

export const createUser = createAction(
  '[User Form] Create User',
  props<{ user: Partial<User> }>()
);
```

**2. Reducer :**

```typescript
// user.reducer.ts
import { createReducer, on } from '@ngrx/store';
import * as UserActions from './user.actions';

export interface UserState {
  users: User[];
  selectedUserId: number | null;
  loading: boolean;
  error: string | null;
}

export const initialState: UserState = {
  users: [],
  selectedUserId: null,
  loading: false,
  error: null
};

export const userReducer = createReducer(
  initialState,
  
  on(UserActions.loadUsers, state => ({
    ...state,
    loading: true,
    error: null
  })),
  
  on(UserActions.loadUsersSuccess, (state, { users }) => ({
    ...state,
    users,
    loading: false
  })),
  
  on(UserActions.loadUsersFailure, (state, { error }) => ({
    ...state,
    loading: false,
    error
  })),
  
  on(UserActions.selectUser, (state, { userId }) => ({
    ...state,
    selectedUserId: userId
  }))
);
```

**3. Selectors :**

```typescript
// user.selectors.ts
import { createFeatureSelector, createSelector } from '@ngrx/store';

export const selectUserState = createFeatureSelector<UserState>('users');

export const selectAllUsers = createSelector(
  selectUserState,
  state => state.users
);

export const selectSelectedUserId = createSelector(
  selectUserState,
  state => state.selectedUserId
);

export const selectSelectedUser = createSelector(
  selectAllUsers,
  selectSelectedUserId,
  (users, selectedId) => users.find(u => u.id === selectedId) || null
);

export const selectLoading = createSelector(
  selectUserState,
  state => state.loading
);

export const selectError = createSelector(
  selectUserState,
  state => state.error
);

export const selectUserCount = createSelector(
  selectAllUsers,
  users => users.length
);
```

**4. Effects :**

```typescript
// user.effects.ts
import { Injectable } from '@angular/core';
import { Actions, createEffect, ofType } from '@ngrx/effects';
import { of } from 'rxjs';
import { map, mergeMap, catchError } from 'rxjs/operators';
import * as UserActions from './user.actions';

@Injectable()
export class UserEffects {
  
  loadUsers$ = createEffect(() =>
    this.actions$.pipe(
      ofType(UserActions.loadUsers),
      mergeMap(() =>
        this.userService.getUsers().pipe(
          map(users => UserActions.loadUsersSuccess({ users })),
          catchError(error => of(UserActions.loadUsersFailure({ 
            error: error.message 
          })))
        )
      )
    )
  );
  
  createUser$ = createEffect(() =>
    this.actions$.pipe(
      ofType(UserActions.createUser),
      mergeMap(({ user }) =>
        this.userService.createUser(user).pipe(
          map(() => UserActions.loadUsers()), // Recharger la liste
          catchError(error => of(UserActions.loadUsersFailure({ 
            error: error.message 
          })))
        )
      )
    )
  );
  
  constructor(
    private actions$: Actions,
    private userService: UserService
  ) {}
}
```

**5. Configuration :**

```typescript
// app.module.ts
import { StoreModule } from '@ngrx/store';
import { EffectsModule } from '@ngrx/effects';
import { StoreDevtoolsModule } from '@ngrx/store-devtools';
import { userReducer } from './store/user.reducer';
import { UserEffects } from './store/user.effects';

@NgModule({
  imports: [
    StoreModule.forRoot({ users: userReducer }),
    EffectsModule.forRoot([UserEffects]),
    StoreDevtoolsModule.instrument({
      maxAge: 25,
      logOnly: environment.production
    })
  ]
})
export class AppModule { }
```

**6. Utilisation dans Component :**

```typescript
import { Store } from '@ngrx/store';
import * as UserActions from './store/user.actions';
import * as UserSelectors from './store/user.selectors';

export class UserListComponent implements OnInit {
  users$ = this.store.select(UserSelectors.selectAllUsers);
  loading$ = this.store.select(UserSelectors.selectLoading);
  error$ = this.store.select(UserSelectors.selectError);
  userCount$ = this.store.select(UserSelectors.selectUserCount);
  
  constructor(private store: Store) {}
  
  ngOnInit(): void {
    this.store.dispatch(UserActions.loadUsers());
  }
  
  selectUser(userId: number): void {
    this.store.dispatch(UserActions.selectUser({ userId }));
  }
  
  createUser(user: Partial<User>): void {
    this.store.dispatch(UserActions.createUser({ user }));
  }
}
```

```html
<div *ngIf="loading$ | async">Chargement...</div>

<div *ngIf="error$ | async as error" class="error">
  {{ error }}
</div>

<p>Total: {{ userCount$ | async }} utilisateurs</p>

<div *ngFor="let user of users$ | async">
  <span (click)="selectUser(user.id)">{{ user.name }}</span>
</div>
```

---

## Testing

### Tests Unitaires (Jasmine + Karma)

**Test d'un Service :**

```typescript
// user.service.spec.ts
import { TestBed } from '@angular/core/testing';
import { HttpClientTestingModule, HttpTestingController } from '@angular/common/http/testing';
import { UserService } from './user.service';

describe('UserService', () => {
  let service: UserService;
  let httpMock: HttpTestingController;
  
  beforeEach(() => {
    TestBed.configureTestingModule({
      imports: [HttpClientTestingModule],
      providers: [UserService]
    });
    
    service = TestBed.inject(UserService);
    httpMock = TestBed.inject(HttpTestingController);
  });
  
  afterEach(() => {
    httpMock.verify(); // Vérifier qu'il n'y a pas de requêtes en attente
  });
  
  it('should be created', () => {
    expect(service).toBeTruthy();
  });
  
  it('should get users', () => {
    const mockUsers: User[] = [
      { id: 1, name: 'John', email: 'john@test.com' },
      { id: 2, name: 'Jane', email: 'jane@test.com' }
    ];
    
    service.getUsers().subscribe(users => {
      expect(users.length).toBe(2);
      expect(users).toEqual(mockUsers);
    });
    
    const req = httpMock.expectOne('/api/users');
    expect(req.request.method).toBe('GET');
    req.flush(mockUsers);
  });
  
  it('should handle error', () => {
    service.getUsers().subscribe({
      next: () => fail('should have failed'),
      error: (error) => {
        expect(error.message).toContain('Error');
      }
    });
    
    const req = httpMock.expectOne('/api/users');
    req.flush('Error', { status: 500, statusText: 'Server Error' });
  });
  
  it('should create user', () => {
    const newUser = { name: 'Bob', email: 'bob@test.com' };
    const createdUser = { id: 3, ...newUser };
    
    service.createUser(newUser).subscribe(user => {
      expect(user).toEqual(createdUser);
    });
    
    const req = httpMock.expectOne('/api/users');
    expect(req.request.method).toBe('POST');
    expect(req.request.body).toEqual(newUser);
    req.flush(createdUser);
  });
});
```

**Test d'un Component :**

```typescript
// user-list.component.spec.ts
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { DebugElement } from '@angular/core';
import { By } from '@angular/platform-browser';
import { of, throwError } from 'rxjs';
import { UserListComponent } from './user-list.component';
import { UserService } from './user.service';

describe('UserListComponent', () => {
  let component: UserListComponent;
  let fixture: ComponentFixture<UserListComponent>;
  let userService: jasmine.SpyObj<UserService>;
  let compiled: HTMLElement;
  
  const mockUsers: User[] = [
    { id: 1, name: 'John', email: 'john@test.com' },
    { id: 2, name: 'Jane', email: 'jane@test.com' }
  ];
  
  beforeEach(async () => {
    // Créer un spy du service
    const userServiceSpy = jasmine.createSpyObj('UserService', ['getUsers', 'deleteUser']);
    
    await TestBed.configureTestingModule({
      declarations: [ UserListComponent ],
      providers: [
        { provide: UserService, useValue: userServiceSpy }
      ]
    }).compileComponents();
    
    userService = TestBed.inject(UserService) as jasmine.SpyObj<UserService>;
  });
  
  beforeEach(() => {
    fixture = TestBed.createComponent(UserListComponent);
    component = fixture.componentInstance;
    compiled = fixture.nativeElement;
  });
  
  it('should create', () => {
    expect(component).toBeTruthy();
  });
  
  it('should load users on init', () => {
    userService.getUsers.and.returnValue(of(mockUsers));
    
    fixture.detectChanges(); // Trigger ngOnInit
    
    expect(userService.getUsers).toHaveBeenCalled();
    expect(component.users).toEqual(mockUsers);
    expect(component.loading).toBe(false);
  });
  
  it('should display users in template', () => {
    userService.getUsers.and.returnValue(of(mockUsers));
    
    fixture.detectChanges();
    
    const userElements = compiled.querySelectorAll('.user-item');
    expect(userElements.length).toBe(2);
    expect(userElements[0].textContent).toContain('John');
    expect(userElements[1].textContent).toContain('Jane');
  });
  
  it('should display loading message', () => {
    component.loading = true;
    fixture.detectChanges();
    
    const loadingElement = compiled.querySelector('.loading');
    expect(loadingElement).toBeTruthy();
    expect(loadingElement?.textContent).toContain('Chargement');
  });
  
  it('should display error message', () => {
    userService.getUsers.and.returnValue(throwError(() => new Error('Network error')));
    
    fixture.detectChanges();
    
    expect(component.error).toBeTruthy();
    
    const errorElement = compiled.querySelector('.error');
    expect(errorElement).toBeTruthy();
  });
  
  it('should delete user when button clicked', () => {
    userService.getUsers.and.returnValue(of(mockUsers));
    userService.deleteUser.and.returnValue(of(void 0));
    
    fixture.detectChanges();
    
    spyOn(window, 'confirm').and.returnValue(true);
    
    component.deleteUser(1);
    
    expect(userService.deleteUser).toHaveBeenCalledWith(1);
    expect(component.users.length).toBe(1);
  });
  
  it('should emit event on user select', () => {
    spyOn(component.userSelected, 'emit');
    
    component.selectUser(mockUsers[0]);
    
    expect(component.userSelected.emit).toHaveBeenCalledWith(mockUsers[0]);
  });
  
  // Test avec DebugElement
  it('should call method on button click', () => {
    userService.getUsers.and.returnValue(of(mockUsers));
    fixture.detectChanges();
    
    spyOn(component, 'deleteUser');
    
    const button: DebugElement = fixture.debugElement.query(By.css('.delete-btn'));
    button.nativeElement.click();
    
    expect(component.deleteUser).toHaveBeenCalled();
  });
});
```

**Test d'un Component avec Reactive Form :**

```typescript
// user-form.component.spec.ts
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { ReactiveFormsModule } from '@angular/forms';
import { UserFormComponent } from './user-form.component';

describe('UserFormComponent', () => {
  let component: UserFormComponent;
  let fixture: ComponentFixture<UserFormComponent>;
  
  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ UserFormComponent ],
      imports: [ ReactiveFormsModule ]
    }).compileComponents();
    
    fixture = TestBed.createComponent(UserFormComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });
  
  it('should create form with validators', () => {
    expect(component.userForm).toBeTruthy();
    expect(component.userForm.get('name')?.hasError('required')).toBe(true);
    expect(component.userForm.get('email')?.hasError('required')).toBe(true);
  });
  
  it('should validate email format', () => {
    const emailControl = component.userForm.get('email');
    
    emailControl?.setValue('invalid');
    expect(emailControl?.hasError('email')).toBe(true);
    
    emailControl?.setValue('valid@test.com');
    expect(emailControl?.hasError('email')).toBe(false);
  });
  
  it('should validate password match', () => {
    component.userForm.patchValue({
      password: 'Test123!',
      confirmPassword: 'Different123!'
    });
    
    expect(component.userForm.hasError('passwordMismatch')).toBe(true);
    
    component.userForm.patchValue({
      confirmPassword: 'Test123!'
    });
    
    expect(component.userForm.hasError('passwordMismatch')).toBe(false);
  });
  
  it('should disable submit button when form invalid', () => {
    const submitBtn = fixture.nativeElement.querySelector('button[type="submit"]');
    
    expect(submitBtn.disabled).toBe(true);
    
    component.userForm.patchValue({
      name: 'John Doe',
      email: 'john@test.com',
      password: 'Test123!',
      confirmPassword: 'Test123!'
    });
    fixture.detectChanges();
    
    expect(submitBtn.disabled).toBe(false);
  });
  
  it('should call onSubmit when form submitted', () => {
    spyOn(component, 'onSubmit');
    
    component.userForm.patchValue({
      name: 'John Doe',
      email: 'john@test.com',
      password: 'Test123!',
      confirmPassword: 'Test123!'
    });
    
    const form = fixture.nativeElement.querySelector('form');
    form.dispatchEvent(new Event('submit'));
    
    expect(component.onSubmit).toHaveBeenCalled();
  });
});
```

**Test avec Routing :**

```typescript
// user-detail.component.spec.ts
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { ActivatedRoute } from '@angular/router';
import { of } from 'rxjs';
import { UserDetailComponent } from './user-detail.component';
import { UserService } from './user.service';

describe('UserDetailComponent', () => {
  let component: UserDetailComponent;
  let fixture: ComponentFixture<UserDetailComponent>;
  let userService: jasmine.SpyObj<UserService>;
  
  const mockUser: User = { id: 1, name: 'John', email: 'john@test.com' };
  
  beforeEach(async () => {
    const userServiceSpy = jasmine.createSpyObj('UserService', ['getUserById']);
    
    await TestBed.configureTestingModule({
      declarations: [ UserDetailComponent ],
      providers: [
        { provide: UserService, useValue: userServiceSpy },
        {
          provide: ActivatedRoute,
          useValue: {
            paramMap: of(new Map([['id', '1']]))
          }
        }
      ]
    }).compileComponents();
    
    userService = TestBed.inject(UserService) as jasmine.SpyObj<UserService>;
  });
  
  beforeEach(() => {
    userService.getUserById.and.returnValue(of(mockUser));
    fixture = TestBed.createComponent(UserDetailComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });
  
  it('should load user based on route param', () => {
    expect(userService.getUserById).toHaveBeenCalledWith(1);
    expect(component.user).toEqual(mockUser);
  });
});
```

**Test d'un Pipe :**

```typescript
// truncate.pipe.spec.ts
import { TruncatePipe } from './truncate.pipe';

describe('TruncatePipe', () => {
  let pipe: TruncatePipe;
  
  beforeEach(() => {
    pipe = new TruncatePipe();
  });
  
  it('should create', () => {
    expect(pipe).toBeTruthy();
  });
  
  it('should truncate long text', () => {
    const text = 'This is a very long text that should be truncated';
    const result = pipe.transform(text, 20);
    
    expect(result).toBe('This is a very long ...');
    expect(result.length).toBe(23); // 20 + '...'
  });
  
  it('should not truncate short text', () => {
    const text = 'Short text';
    const result = pipe.transform(text, 20);
    
    expect(result).toBe('Short text');
  });
  
  it('should use custom ellipsis', () => {
    const text = 'This is a very long text';
    const result = pipe.transform(text, 10, '---');
    
    expect(result).toBe('This is a ---');
  });
  
  it('should handle empty string', () => {
    expect(pipe.transform('', 10)).toBe('');
  });
  
  it('should handle null', () => {
    expect(pipe.transform(null as any, 10)).toBe('');
  });
});
```

**Test d'une Directive :**

```typescript
// highlight.directive.spec.ts
import { Component, DebugElement } from '@angular/core';
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { By } from '@angular/platform-browser';
import { HighlightDirective } from './highlight.directive';

@Component({
  template: `
    <p appHighlight="yellow">Test paragraph</p>
    <p appHighlight>Default highlight</p>
  `
})
class TestComponent {}

describe('HighlightDirective', () => {
  let fixture: ComponentFixture<TestComponent>;
  let des: DebugElement[];
  
  beforeEach(() => {
    TestBed.configureTestingModule({
      declarations: [ HighlightDirective, TestComponent ]
    });
    
    fixture = TestBed.createComponent(TestComponent);
    fixture.detectChanges();
    des = fixture.debugElement.queryAll(By.directive(HighlightDirective));
  });
  
  it('should have two highlighted elements', () => {
    expect(des.length).toBe(2);
  });
  
  it('should apply custom color', () => {
    const bgColor = des[0].nativeElement.style.backgroundColor;
    // Après hover, tester avec triggerEventHandler
  });
  
  it('should change color on mouseenter', () => {
    const p = des[0];
    
    p.triggerEventHandler('mouseenter', null);
    fixture.detectChanges();
    
    const bgColor = p.nativeElement.style.backgroundColor;
    expect(bgColor).toBe('yellow');
  });
  
  it('should reset color on mouseleave', () => {
    const p = des[0];
    
    p.triggerEventHandler('mouseenter', null);
    p.triggerEventHandler('mouseleave', null);
    fixture.detectChanges();
    
    const bgColor = p.nativeElement.style.backgroundColor;
    expect(bgColor).toBe('transparent');
  });
});
```

### Tests E2E (Protractor/Cypress)

**Exemple avec Cypress :**

```typescript
// users.spec.ts
describe('User Management', () => {
  beforeEach(() => {
    cy.visit('/users');
  });
  
  it('should display user list', () => {
    cy.get('.user-item').should('have.length.greaterThan', 0);
  });
  
  it('should search users', () => {
    cy.get('input[name="search"]').type('John');
    cy.get('.user-item').should('contain', 'John');
  });
  
  it('should navigate to user detail', () => {
    cy.get('.user-item').first().click();
    cy.url().should('include', '/users/');
    cy.get('.user-detail').should('be.visible');
  });
  
  it('should create new user', () => {
    cy.get('.create-user-btn').click();
    
    cy.get('input[name="name"]').type('New User');
    cy.get('input[name="email"]').type('newuser@test.com');
    cy.get('button[type="submit"]').click();
    
    cy.get('.success-message').should('be.visible');
    cy.get('.user-item').should('contain', 'New User');
  });
  
  it('should validate form', () => {
    cy.get('.create-user-btn').click();
    cy.get('button[type="submit"]').click();
    
    cy.get('.error-message').should('be.visible');
  });
  
  it('should delete user', () => {
    cy.get('.user-item').first().within(() => {
      cy.get('.delete-btn').click();
    });
    
    cy.get('.confirm-dialog').should('be.visible');
    cy.get('.confirm-btn').click();
    
    cy.get('.success-message').should('contain', 'Utilisateur supprimé');
  });
});
```

---

## Best Practices

### Architecture & Organisation

**1. Structure de projet recommandée :**

```
src/app/
├── core/                        # Services singleton, guards, interceptors
│   ├── guards/
│   ├── interceptors/
│   ├── services/
│   └── core.module.ts
├── shared/                      # Components, directives, pipes réutilisables
│   ├── components/
│   ├── directives/
│   ├── pipes/
│   └── shared.module.ts
├── features/                    # Modules fonctionnels
│   ├── users/
│   │   ├── components/
│   │   ├── services/
│   │   ├── models/
│   │   ├── store/              # Si NgRx
│   │   └── users.module.ts
│   └── products/
└── app.component.ts
```

**2. Modules :**

- **CoreModule** : Importé UNE SEULE FOIS dans AppModule
- **SharedModule** : Importé dans chaque feature module qui en a besoin
- **Feature Modules** : Lazy loaded quand possible

```typescript
// core.module.ts
@NgModule({
  providers: [
    AuthService,
    LoggerService,
    // Guards, Interceptors...
  ]
})
export class CoreModule {
  // Empêcher import multiple
  constructor(@Optional() @SkipSelf() parentModule: CoreModule) {
    if (parentModule) {
      throw new Error('CoreModule est déjà chargé. Importez-le uniquement dans AppModule');
    }
  }
}
```

### Performance

**1. OnPush Change Detection :**

```typescript
@Component({
  selector: 'app-user-list',
  changeDetection: ChangeDetectionStrategy.OnPush, // Performance++
  template: `...`
})
export class UserListComponent {
  @Input() users: User[] = []; // Immutable inputs
  
  constructor(private cdr: ChangeDetectorRef) {}
  
  // Forcer detection manuelle si besoin
  forceUpdate() {
    this.cdr.markForCheck();
  }
}
```

**2. TrackBy dans ngFor :**

```html
<div *ngFor="let user of users; trackBy: trackByUserId">
  {{ user.name }}
</div>
```

```typescript
trackByUserId(index: number, user: User): number {
  return user.id; // Angular ne recrée que si l'id change
}
```

**3. Lazy Loading :**

```typescript
const routes: Routes = [
  {
    path: 'admin',
    loadChildren: () => import('./admin/admin.module').then(m => m.AdminModule)
  }
];
```

**4. Unsubscribe automatique :**

```typescript
// Méthode 1: takeUntil
private destroy$ = new Subject<void>();

ngOnInit() {
  this.service.getData()
    .pipe(takeUntil(this.destroy$))
    .subscribe();
}

ngOnDestroy() {
  this.destroy$.next();
  this.destroy$.complete();
}

// Méthode 2: async pipe (recommandé)
data$ = this.service.getData(); // Pas de subscribe
```

```html
<div *ngFor="let item of data$ | async">{{ item }}</div>
```

**5. Pure Pipes :**

```typescript
@Pipe({
  name: 'expensiveCalc',
  pure: true // Par défaut, ne se déclenche que si input change
})
```

### Code Quality

**1. Typage fort :**

```typescript
// ✅ BON
interface User {
  id: number;
  name: string;
}

getUser(): Observable<User> {
  return this.http.get<User>('/api/user');
}

// ❌ MAUVAIS
getUser(): Observable<any> {
  return this.http.get('/api/user');
}
```

**2. Éviter any :**

```typescript
// ✅ BON
function process(data: unknown): void {
  if (typeof data === 'string') {
    console.log(data.toUpperCase());
  }
}

// ❌ MAUVAIS
function process(data: any): void {
  console.log(data.toUpperCase()); // Pas de type safety
}
```

**3. Immutabilité :**

```typescript
// ✅ BON
const users = [...this.users, newUser];
const updatedUser = { ...user, name: 'New Name' };

// ❌ MAUVAIS
this.users.push(newUser); // Mutation directe
user.name = 'New Name';
```

**4. Services providedIn root :**

```typescript
// ✅ BON - Tree-shakeable
@Injectable({
  providedIn: 'root'
})
export class UserService { }

// ❌ MOINS BON - Doit être déclaré dans providers
@Injectable()
export class UserService { }
```

**5. Reactive programming :**

```typescript
// ✅ BON
searchTerm$ = new Subject<string>();
results$ = this.searchTerm$.pipe(
  debounceTime(300),
  distinctUntilChanged(),
  switchMap(term => this.api.search(term))
);

// ❌ MAUVAIS
searchTerm = '';
onSearch() {
  setTimeout(() => {
    if (this.lastSearch !== this.searchTerm) {
      this.api.search(this.searchTerm).subscribe(results => {
        this.results = results;
      });
    }
  }, 300);
}
```

### Sécurité

**1. Sanitize HTML :**

```typescript
import { DomSanitizer, SafeHtml } from '@angular/platform-browser';

constructor(private sanitizer: DomSanitizer) {}

safeHtml: SafeHtml = this.sanitizer.sanitize(SecurityContext.HTML, userInput);
```

**2. Éviter eval et innerHTML avec données non fiables :**

```html
<!-- ❌ DANGEREUX -->
<div [innerHTML]="userInput"></div>

<!-- ✅ BON -->
<div>{{ userInput }}</div>
```

**3. HttpOnly cookies pour tokens :**

```typescript
// Stockage token côté serveur dans cookie HttpOnly
// Pas dans localStorage (vulnérable XSS)
```

### Conventions de Nommage

**1. Fichiers :**
- Components: `user-list.component.ts`
- Services: `user.service.ts`
- Pipes: `truncate.pipe.ts`
- Directives: `highlight.directive.ts`
- Guards: `auth.guard.ts`
- Models: `user.model.ts` ou `user.interface.ts`

**2. Classes :**
- Components: `UserListComponent`
- Services: `UserService`
- Pipes: `TruncatePipe`

**3. Observables :**
```typescript
users$ = this.userService.getUsers(); // $ suffix pour observables
```

**4. Subjects privés :**
```typescript
private usersSubject = new BehaviorSubject<User[]>([]);
public users$ = this.usersSubject.asObservable(); // Exposer comme Observable
```

### Debugging

**1. Angular DevTools :**
- Extension Chrome/Firefox
- Inspect component tree, change detection, profiling

**2. Console logs avec tap :**
```typescript
this.service.getData().pipe(
  tap(data => console.log('Data received:', data)),
  map(data => transform(data)),
  tap(data => console.log('Data transformed:', data))
).subscribe();
```

**3. Redux DevTools (avec NgRx) :**
```typescript
StoreDevtoolsModule.instrument({
  maxAge: 25,
  logOnly: environment.production
})
```

**4. RxJS spy :**
```typescript
import { create } from 'rxjs-spy';
const spy = create();

this.service.getData().pipe(
  tag('user-data') // Tag pour debugging
).subscribe();
```

---
