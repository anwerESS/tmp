## [**..**](./00_index.md)

# Chapitre 7 : RxJS dans Angular

## 📑 Table des matières
- [7.1 HttpClient et Observables](#71-httpclient-et-observables)
- [7.2 Formulaires Réactifs](#72-formulaires-réactifs)
- [7.3 Router et Observables](#73-router-et-observables)
- [7.4 State Management](#74-state-management)
- [7.5 Best Practices Angular](#75-best-practices-angular)
- [7.6 Exercice Pratique](#76-exercice-pratique)
- [Questions/Réponses d'Entretien](#-questionsréponses-dentretien)

---

## 7.1 HttpClient et Observables

### 🎯 Requêtes HTTP de Base

Angular `HttpClient` retourne automatiquement des Observables qui **se complètent après une seule réponse**.

```typescript
import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';

interface User {
  id: number;
  name: string;
  email: string;
}

@Injectable({ providedIn: 'root' })
export class UserService {
  private apiUrl = 'https://api.example.com/users';
  
  constructor(private http: HttpClient) {}
  
  // GET - Récupérer tous les utilisateurs
  getUsers(): Observable<User[]> {
    return this.http.get<User[]>(this.apiUrl);
  }
  
  // GET - Récupérer un utilisateur par ID
  getUserById(id: number): Observable<User> {
    return this.http.get<User>(`${this.apiUrl}/${id}`);
  }
  
  // POST - Créer un utilisateur
  createUser(user: Partial<User>): Observable<User> {
    return this.http.post<User>(this.apiUrl, user);
  }
  
  // PUT - Mettre à jour un utilisateur
  updateUser(id: number, user: Partial<User>): Observable<User> {
    return this.http.put<User>(`${this.apiUrl}/${id}`, user);
  }
  
  // DELETE - Supprimer un utilisateur
  deleteUser(id: number): Observable<void> {
    return this.http.delete<void>(`${this.apiUrl}/${id}`);
  }
}
```

### 📝 Utilisation dans un Composant

```typescript
import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-users',
  template: `
    <div *ngIf="loading">Chargement...</div>
    <div *ngIf="error" class="error">{{ error }}</div>
    
    <div *ngFor="let user of users">
      <h3>{{ user.name }}</h3>
      <p>{{ user.email }}</p>
    </div>
  `
})
export class UsersComponent implements OnInit {
  users: User[] = [];
  loading = false;
  error: string | null = null;
  
  constructor(private userService: UserService) {}
  
  ngOnInit() {
    this.loadUsers();
  }
  
  loadUsers() {
    this.loading = true;
    this.error = null;
    
    this.userService.getUsers().subscribe({
      next: (users) => {
        this.users = users;
        this.loading = false;
      },
      error: (err) => {
        this.error = 'Erreur lors du chargement';
        this.loading = false;
        console.error(err);
      }
    });
  }
}
```

### 🔄 Version avec AsyncPipe (Recommandé)

```typescript
import { Observable, of } from 'rxjs';
import { catchError } from 'rxjs/operators';

@Component({
  selector: 'app-users',
  template: `
    <div *ngIf="users$ | async as users; else loading">
      <div *ngFor="let user of users">
        <h3>{{ user.name }}</h3>
        <p>{{ user.email }}</p>
      </div>
    </div>
    
    <ng-template #loading>
      <div>Chargement...</div>
    </ng-template>
  `
})
export class UsersComponent implements OnInit {
  users$: Observable<User[]>;
  
  constructor(private userService: UserService) {}
  
  ngOnInit() {
    this.users$ = this.userService.getUsers().pipe(
      catchError(err => {
        console.error('Erreur:', err);
        return of([]); // Retourner tableau vide
      })
    );
  }
}
```

### 🎨 Headers et Options HTTP

```typescript
import { HttpHeaders, HttpParams } from '@angular/common/http';

@Injectable({ providedIn: 'root' })
export class UserService {
  constructor(private http: HttpClient) {}
  
  // Avec headers personnalisés
  getUsersWithAuth(): Observable<User[]> {
    const headers = new HttpHeaders({
      'Authorization': 'Bearer token123',
      'Content-Type': 'application/json'
    });
    
    return this.http.get<User[]>(this.apiUrl, { headers });
  }
  
  // Avec paramètres de requête
  searchUsers(query: string, page: number = 1): Observable<User[]> {
    const params = new HttpParams()
      .set('q', query)
      .set('page', page.toString())
      .set('limit', '10');
    
    return this.http.get<User[]>(`${this.apiUrl}/search`, { params });
  }
  
  // Obtenir la réponse complète
  getUserWithResponse(id: number): Observable<HttpResponse<User>> {
    return this.http.get<User>(`${this.apiUrl}/${id}`, {
      observe: 'response'
    });
  }
}
```

### 🔒 Intercepteurs HTTP

```typescript
// auth.interceptor.ts
import { Injectable } from '@angular/core';
import { HttpInterceptor, HttpRequest, HttpHandler } from '@angular/common/http';

@Injectable()
export class AuthInterceptor implements HttpInterceptor {
  intercept(req: HttpRequest<any>, next: HttpHandler) {
    const token = localStorage.getItem('authToken');
    
    if (token) {
      const clonedReq = req.clone({
        headers: req.headers.set('Authorization', `Bearer ${token}`)
      });
      return next.handle(clonedReq);
    }
    
    return next.handle(req);
  }
}
```

### 💾 Cache avec ShareReplay

```typescript
import { shareReplay } from 'rxjs/operators';

@Injectable({ providedIn: 'root' })
export class UserService {
  private usersCache$: Observable<User[]> | null = null;
  
  getUsersCached(): Observable<User[]> {
    if (!this.usersCache$) {
      this.usersCache$ = this.http.get<User[]>(this.apiUrl).pipe(
        shareReplay(1)
      );
    }
    return this.usersCache$;
  }
  
  refreshCache() {
    this.usersCache$ = null;
  }
}
```

---

## 7.2 Formulaires Réactifs

### 🎯 FormControl et valueChanges

```typescript
import { FormControl } from '@angular/forms';
import { debounceTime, distinctUntilChanged } from 'rxjs/operators';

@Component({
  selector: 'app-search',
  template: `
    <input [formControl]="searchControl" placeholder="Rechercher...">
  `
})
export class SearchComponent implements OnInit {
  searchControl = new FormControl('');
  
  ngOnInit() {
    this.searchControl.valueChanges.pipe(
      debounceTime(300),
      distinctUntilChanged()
    ).subscribe(value => {
      console.log('Recherche:', value);
    });
  }
}
```

### 📋 FormGroup Complexe

```typescript
import { FormBuilder, Validators } from '@angular/forms';

@Component({
  selector: 'app-user-form',
  template: `
    <form [formGroup]="userForm" (ngSubmit)="onSubmit()">
      <input formControlName="name" placeholder="Nom">
      <input formControlName="email" placeholder="Email">
      <button [disabled]="userForm.invalid">Envoyer</button>
    </form>
  `
})
export class UserFormComponent implements OnInit {
  userForm: FormGroup;
  
  constructor(private fb: FormBuilder) {}
  
  ngOnInit() {
    this.userForm = this.fb.group({
      name: ['', [Validators.required, Validators.minLength(3)]],
      email: ['', [Validators.required, Validators.email]]
    });
    
    // Écouter les changements
    this.userForm.valueChanges.subscribe(value => {
      console.log('Formulaire modifié:', value);
    });
  }
  
  onSubmit() {
    if (this.userForm.valid) {
      console.log(this.userForm.value);
    }
  }
}
```

### ✅ Validation Asynchrone

```typescript
import { AbstractControl, ValidationErrors } from '@angular/forms';

@Injectable({ providedIn: 'root' })
export class UserValidators {
  constructor(private http: HttpClient) {}
  
  emailExists(control: AbstractControl): Observable<ValidationErrors | null> {
    if (!control.value) return of(null);
    
    return this.http.get<boolean>(`/api/check-email/${control.value}`).pipe(
      map(exists => exists ? { emailExists: true } : null),
      catchError(() => of(null))
    );
  }
}

// Utilisation
this.registerForm = this.fb.group({
  email: ['', 
    [Validators.required, Validators.email],
    [this.validators.emailExists.bind(this.validators)]
  ]
});
```

---

## 7.3 Router et Observables

### 🎯 ActivatedRoute params

```typescript
import { ActivatedRoute } from '@angular/router';
import { switchMap } from 'rxjs/operators';

@Component({
  selector: 'app-user-detail',
  template: `
    <div *ngIf="user$ | async as user">
      <h2>{{ user.name }}</h2>
    </div>
  `
})
export class UserDetailComponent implements OnInit {
  user$: Observable<User>;
  
  constructor(
    private route: ActivatedRoute,
    private userService: UserService
  ) {}
  
  ngOnInit() {
    this.user$ = this.route.params.pipe(
      switchMap(params => {
        const id = +params['id'];
        return this.userService.getUserById(id);
      })
    );
  }
}
```

### 🔍 Query Params

```typescript
@Component({
  selector: 'app-search-results'
})
export class SearchResultsComponent implements OnInit {
  results$: Observable<any[]>;
  
  ngOnInit() {
    this.results$ = this.route.queryParams.pipe(
      switchMap(params => {
        const query = params['q'] || '';
        return this.searchService.search(query);
      })
    );
  }
}
```

### 🔐 Guards avec Observables

```typescript
import { CanActivate, Router } from '@angular/router';

@Injectable({ providedIn: 'root' })
export class AuthGuard implements CanActivate {
  constructor(
    private authService: AuthService,
    private router: Router
  ) {}
  
  canActivate(): Observable<boolean> {
    return this.authService.isAuthenticated().pipe(
      map(isAuth => {
        if (isAuth) return true;
        this.router.navigate(['/login']);
        return false;
      })
    );
  }
}
```

---

## 7.4 State Management

### 🎯 Service avec BehaviorSubject

```typescript
interface AppState {
  user: User | null;
  isLoading: boolean;
}

@Injectable({ providedIn: 'root' })
export class StateService {
  private state = new BehaviorSubject<AppState>({
    user: null,
    isLoading: false
  });
  
  state$ = this.state.asObservable();
  
  user$ = this.state$.pipe(map(s => s.user));
  isLoading$ = this.state$.pipe(map(s => s.isLoading));
  
  setUser(user: User) {
    this.state.next({ ...this.state.value, user });
  }
  
  setLoading(isLoading: boolean) {
    this.state.next({ ...this.state.value, isLoading });
  }
}
```

### 🏪 Pattern Store

```typescript
interface TodoState {
  todos: Todo[];
  filter: 'all' | 'active' | 'completed';
}

@Injectable({ providedIn: 'root' })
export class TodoStore {
  private state = new BehaviorSubject<TodoState>({
    todos: [],
    filter: 'all'
  });
  
  todos$ = this.state.pipe(map(s => s.todos));
  
  filteredTodos$ = this.state.pipe(
    map(state => {
      const { todos, filter } = state;
      switch (filter) {
        case 'active': return todos.filter(t => !t.completed);
        case 'completed': return todos.filter(t => t.completed);
        default: return todos;
      }
    })
  );
  
  addTodo(title: string) {
    const todos = [...this.state.value.todos, {
      id: Date.now(),
      title,
      completed: false
    }];
    this.updateState({ todos });
  }
  
  toggleTodo(id: number) {
    const todos = this.state.value.todos.map(todo =>
      todo.id === id ? { ...todo, completed: !todo.completed } : todo
    );
    this.updateState({ todos });
  }
  
  private updateState(partial: Partial<TodoState>) {
    this.state.next({ ...this.state.value, ...partial });
  }
}
```

---

## 7.5 Best Practices Angular

### 🎯 Unsubscribe Automatique

#### Pattern 1: takeUntil

```typescript
import { Subject } from 'rxjs';
import { takeUntil } from 'rxjs/operators';

@Component({ selector: 'app-example' })
export class ExampleComponent implements OnInit, OnDestroy {
  private destroy$ = new Subject<void>();
  
  ngOnInit() {
    this.dataService.getData()
      .pipe(takeUntil(this.destroy$))
      .subscribe(data => console.log(data));
  }
  
  ngOnDestroy() {
    this.destroy$.next();
    this.destroy$.complete();
  }
}
```

#### Pattern 2: AsyncPipe (Recommandé)

```typescript
@Component({
  selector: 'app-example',
  template: `<div *ngIf="data$ | async as data">{{ data.name }}</div>`
})
export class ExampleComponent {
  data$ = this.dataService.getData();
}
```

### ⚡ Performance

```typescript
// ShareReplay pour cache
@Injectable({ providedIn: 'root' })
export class DataService {
  data$ = this.http.get<Data[]>('/api/data').pipe(
    shareReplay({ bufferSize: 1, refCount: true })
  );
}

// OnPush pour optimisation
@Component({
  changeDetection: ChangeDetectionStrategy.OnPush,
  template: `<div *ngFor="let item of items$ | async">{{ item }}</div>`
})
export class OptimizedComponent {
  items$ = this.itemService.getItems();
}
```

### 🚫 Memory Leaks à Éviter

```typescript
// ❌ MAUVAIS
export class BadComponent implements OnInit {
  ngOnInit() {
    interval(1000).subscribe(console.log);
    // Continue après destruction !
  }
}

// ✅ BON
export class GoodComponent implements OnInit, OnDestroy {
  private destroy$ = new Subject<void>();
  
  ngOnInit() {
    interval(1000)
      .pipe(takeUntil(this.destroy$))
      .subscribe(console.log);
  }
  
  ngOnDestroy() {
    this.destroy$.next();
    this.destroy$.complete();
  }
}
```

---

## 7.6 Exercice Pratique

### 🎯 Challenge : Application CRUD Complète

Créez une application de gestion d'utilisateurs avec :
1. Liste des utilisateurs (GET)
2. Détail d'un utilisateur (GET par ID)
3. Création d'utilisateur (POST)
4. Modification d'utilisateur (PUT)
5. Suppression d'utilisateur (DELETE)
6. Recherche avec debounce
7. State management centralisé
8. Gestion des erreurs
9. Loading states
10. Cache avec ShareReplay

### 💡 Solution

```typescript
// user.model.ts
export interface User {
  id: number;
  name: string;
  email: string;
  role: 'admin' | 'user';
}

// user.service.ts
import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, throwError } from 'rxjs';
import { catchError, shareReplay, retry } from 'rxjs/operators';

@Injectable({ providedIn: 'root' })
export class UserService {
  private apiUrl = 'https://api.example.com/users';
  private usersCache$: Observable<User[]> | null = null;
  
  constructor(private http: HttpClient) {}
  
  getUsers(): Observable<User[]> {
    if (!this.usersCache$) {
      this.usersCache$ = this.http.get<User[]>(this.apiUrl).pipe(
        retry(2),
        shareReplay(1),
        catchError(this.handleError)
      );
    }
    return this.usersCache$;
  }
  
  getUserById(id: number): Observable<User> {
    return this.http.get<User>(`${this.apiUrl}/${id}`).pipe(
      catchError(this.handleError)
    );
  }
  
  createUser(user: Partial<User>): Observable<User> {
    return this.http.post<User>(this.apiUrl, user).pipe(
      catchError(this.handleError)
    );
  }
  
  updateUser(id: number, user: Partial<User>): Observable<User> {
    return this.http.put<User>(`${this.apiUrl}/${id}`, user).pipe(
      catchError(this.handleError)
    );
  }
  
  deleteUser(id: number): Observable<void> {
    return this.http.delete<void>(`${this.apiUrl}/${id}`).pipe(
      catchError(this.handleError)
    );
  }
  
  searchUsers(query: string): Observable<User[]> {
    return this.http.get<User[]>(`${this.apiUrl}/search?q=${query}`).pipe(
      catchError(this.handleError)
    );
  }
  
  refreshCache() {
    this.usersCache$ = null;
  }
  
  private handleError(error: any): Observable<never> {
    console.error('Erreur API:', error);
    return throwError(() => new Error('Erreur lors de la requête'));
  }
}

// user-state.service.ts
import { Injectable } from '@angular/core';
import { BehaviorSubject } from 'rxjs';
import { map } from 'rxjs/operators';

interface UserState {
  users: User[];
  selectedUser: User | null;
  isLoading: boolean;
  error: string | null;
}

@Injectable({ providedIn: 'root' })
export class UserStateService {
  private state = new BehaviorSubject<UserState>({
    users: [],
    selectedUser: null,
    isLoading: false,
    error: null
  });
  
  // Selectors
  users$ = this.state.pipe(map(s => s.users));
  selectedUser$ = this.state.pipe(map(s => s.selectedUser));
  isLoading$ = this.state.pipe(map(s => s.isLoading));
  error$ = this.state.pipe(map(s => s.error));
  
  constructor(private userService: UserService) {}
  
  loadUsers() {
    this.setLoading(true);
    this.userService.getUsers().subscribe({
      next: (users) => {
        this.updateState({ users, isLoading: false, error: null });
      },
      error: (error) => {
        this.updateState({ 
          isLoading: false, 
          error: 'Erreur lors du chargement' 
        });
      }
    });
  }
  
  selectUser(id: number) {
    this.setLoading(true);
    this.userService.getUserById(id).subscribe({
      next: (user) => {
        this.updateState({ selectedUser: user, isLoading: false });
      },
      error: () => {
        this.setError('Utilisateur non trouvé');
      }
    });
  }
  
  createUser(user: Partial<User>) {
    this.setLoading(true);
    this.userService.createUser(user).subscribe({
      next: (newUser) => {
        const users = [...this.state.value.users, newUser];
        this.updateState({ users, isLoading: false });
        this.userService.refreshCache();
      },
      error: () => this.setError('Erreur lors de la création')
    });
  }
  
  updateUser(id: number, user: Partial<User>) {
    this.setLoading(true);
    this.userService.updateUser(id, user).subscribe({
      next: (updated) => {
        const users = this.state.value.users.map(u => 
          u.id === id ? updated : u
        );
        this.updateState({ users, isLoading: false });
        this.userService.refreshCache();
      },
      error: () => this.setError('Erreur lors de la modification')
    });
  }
  
  deleteUser(id: number) {
    this.setLoading(true);
    this.userService.deleteUser(id).subscribe({
      next: () => {
        const users = this.state.value.users.filter(u => u.id !== id);
        this.updateState({ users, isLoading: false });
        this.userService.refreshCache();
      },
      error: () => this.setError('Erreur lors de la suppression')
    });
  }
  
  private setLoading(isLoading: boolean) {
    this.updateState({ isLoading });
  }
  
  private setError(error: string) {
    this.updateState({ error, isLoading: false });
  }
  
  clearError() {
    this.updateState({ error: null });
  }
  
  private updateState(partial: Partial<UserState>) {
    this.state.next({ ...this.state.value, ...partial });
  }
}

// user-list.component.ts
@Component({
  selector: 'app-user-list',
  template: `
    <div class="search-box">
      <input [formControl]="searchControl" 
             placeholder="Rechercher un utilisateur...">
    </div>
    
    <div *ngIf="isLoading$ | async" class="loading">
      Chargement...
    </div>
    
    <div *ngIf="error$ | async as error" class="error">
      {{ error }}
      <button (click)="clearError()">X</button>
    </div>
    
    <div class="user-list">
      <div *ngFor="let user of users$ | async" class="user-card">
        <h3>{{ user.name }}</h3>
        <p>{{ user.email }}</p>
        <span class="role">{{ user.role }}</span>
        
        <div class="actions">
          <button (click)="viewUser(user.id)">Voir</button>
          <button (click)="editUser(user)">Modifier</button>
          <button (click)="deleteUser(user.id)">Supprimer</button>
        </div>
      </div>
    </div>
    
    <button (click)="openCreateDialog()" class="fab">+</button>
  `,
  styles: [`
    .search-box {
      margin: 20px;
    }
    .search-box input {
      width: 100%;
      padding: 10px;
      font-size: 16px;
    }
    .loading {
      text-align: center;
      padding: 20px;
    }
    .error {
      background: #f44336;
      color: white;
      padding: 15px;
      margin: 10px;
      border-radius: 4px;
    }
    .user-list {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
      gap: 20px;
      padding: 20px;
    }
    .user-card {
      border: 1px solid #ddd;
      padding: 20px;
      border-radius: 8px;
    }
    .role {
      background: #2196f3;
      color: white;
      padding: 4px 8px;
      border-radius: 4px;
      font-size: 12px;
    }
    .actions {
      margin-top: 15px;
    }
    .actions button {
      margin-right: 10px;
      padding: 8px 16px;
    }
    .fab {
      position: fixed;
      bottom: 30px;
      right: 30px;
      width: 60px;
      height: 60px;
      border-radius: 50%;
      background: #4caf50;
      color: white;
      font-size: 24px;
      border: none;
      cursor: pointer;
    }
  `]
})
export class UserListComponent implements OnInit, OnDestroy {
  searchControl = new FormControl('');
  private destroy$ = new Subject<void>();
  
  users$ = this.userState.users$;
  isLoading$ = this.userState.isLoading$;
  error$ = this.userState.error$;
  
  constructor(
    private userState: UserStateService,
    private userService: UserService,
    private router: Router
  ) {}
  
  ngOnInit() {
    this.userState.loadUsers();
    
    // Recherche avec debounce
    this.searchControl.valueChanges.pipe(
      debounceTime(300),
      distinctUntilChanged(),
      switchMap(query => {
        if (query && query.length > 2) {
          return this.userService.searchUsers(query);
        }
        return this.userState.users$;
      }),
      takeUntil(this.destroy$)
    ).subscribe();
  }
  
  viewUser(id: number) {
    this.router.navigate(['/users', id]);
  }
  
  editUser(user: User) {
    // Ouvrir dialog d'édition
  }
  
  deleteUser(id: number) {
    if (confirm('Supprimer cet utilisateur ?')) {
      this.userState.deleteUser(id);
    }
  }
  
  openCreateDialog() {
    // Ouvrir dialog de création
  }
  
  clearError() {
    this.userState.clearError();
  }
  
  ngOnDestroy() {
    this.destroy$.next();
    this.destroy$.complete();
  }
}

// user-detail.component.ts
@Component({
  selector: 'app-user-detail',
  template: `
    <div *ngIf="user$ | async as user; else loading">
      <h1>{{ user.name }}</h1>
      <p>Email: {{ user.email }}</p>
      <p>Rôle: {{ user.role }}</p>
      
      <button (click)="goBack()">Retour</button>
      <button (click)="edit(user)">Modifier</button>
    </div>
    
    <ng-template #loading>
      <div>Chargement...</div>
    </ng-template>
  `
})
export class UserDetailComponent implements OnInit {
  user$: Observable<User>;
  
  constructor(
    private route: ActivatedRoute,
    private userService: UserService,
    private location: Location
  ) {}
  
  ngOnInit() {
    this.user$ = this.route.params.pipe(
      switchMap(params => {
        const id = +params['id'];
        return this.userService.getUserById(id);
      })
    );
  }
  
  goBack() {
    this.location.back();
  }
  
  edit(user: User) {
    // Navigation vers édition
  }
}
```

---

## 📝 Questions/Réponses d'Entretien

### Q1 : Pourquoi HttpClient retourne-t-il des Observables et pas des Promises ?

**Réponse :**
> **HttpClient utilise des Observables car** :
> 
> 1. **Annulable** : On peut annuler une requête HTTP avec `unsubscribe()`
> 2. **Opérateurs RxJS** : Permet d'utiliser `retry()`, `catchError()`, `map()`, etc.
> 3. **Composition** : Facile de combiner plusieurs requêtes avec `forkJoin()`, `combineLatest()`
> 4. **Cohérence** : Tout Angular utilise RxJS (Forms, Router, etc.)
> 
> ```typescript
> // Annulation possible
> const sub = this.http.get('/api/data').subscribe();
> sub.unsubscribe(); // Annule la requête
> 
> // Retry automatique
> this.http.get('/api/data').pipe(
>   retry(3),
>   catchError(err => of([]))
> ).subscribe();
> ```

### Q2 : Quelle est la différence entre `subscribe()` et `async pipe` ?

**Réponse :**
> **subscribe()** :
> - Nécessite un `unsubscribe()` manuel
> - Plus de contrôle sur les données
> - Utile pour la logique métier complexe
> 
> **async pipe** :
> - Unsubscribe automatique
> - Change Detection optimisée
> - Code plus propre et déclaratif
> - **Recommandé dans Angular**
> 
> ```typescript
> // ❌ Subscribe manuel
> ngOnInit() {
>   this.userService.getUsers().subscribe(users => {
>     this.users = users;
>   });
> } // Risque de memory leak !
> 
> // ✅ AsyncPipe
> users$ = this.userService.getUsers();
> // Template: <div *ngFor="let u of users$ | async">
> ```

### Q3 : Comment gérer le cache des requêtes HTTP dans Angular ?

**Réponse :**
> Utiliser **`shareReplay()`** pour partager le résultat entre souscripteurs :
> 
> ```typescript
> @Injectable({ providedIn: 'root' })
> export class DataService {
>   private cache$: Observable<Data[]> | null = null;
>   
>   getData(): Observable<Data[]> {
>     if (!this.cache$) {
>       this.cache$ = this.http.get<Data[]>('/api/data').pipe(
>         shareReplay({ bufferSize: 1, refCount: true })
>       );
>     }
>     return this.cache$;
>   }
>   
>   refreshCache() {
>     this.cache$ = null;
>   }
> }
> ```
> 
> **Avantages** :
> - Une seule requête HTTP pour plusieurs composants
> - Refresh manuel possible
> - Performance améliorée

### Q4 : Qu'est-ce qu'un Intercepteur HTTP et à quoi sert-il ?

**Réponse :**
> Un **Intercepteur** permet d'intercepter et modifier toutes les requêtes/réponses HTTP :
> 
> **Cas d'usage** :
> - Ajouter un token d'authentification
> - Logger les requêtes
> - Gérer les erreurs globalement
> - Ajouter des headers personnalisés
> - Afficher un loader global
> 
> ```typescript
> @Injectable()
> export class AuthInterceptor implements HttpInterceptor {
>   intercept(req: HttpRequest<any>, next: HttpHandler) {
>     const token = localStorage.getItem('token');
>     
>     if (token) {
>       req = req.clone({
>         setHeaders: { Authorization: `Bearer ${token}` }
>       });
>     }
>     
>     return next.handle(req);
>   }
> }
> ```

### Q5 : Comment implémenter une validation asynchrone dans un formulaire réactif ?

**Réponse :**
> Créer un **ValidatorFn** qui retourne un `Observable<ValidationErrors | null>` :
> 
> ```typescript
> emailExists(control: AbstractControl): Observable<ValidationErrors | null> {
>   return this.http.get<boolean>(`/api/check-email/${control.value}`).pipe(
>     map(exists => exists ? { emailExists: true } : null),
>     catchError(() => of(null))
>   );
> }
> 
> // Utilisation
> this.form = this.fb.group({
>   email: ['', 
>     [Validators.required], // Validateurs synchrones
>     [this.emailExists.bind(this)] // Validateur asynchrone
>   ]
> });
> ```
> 
> **Important** : Le validateur asynchrone s'exécute **après** les validateurs synchrones.

### Q6 : Comment récupérer les paramètres de route avec RxJS ?

**Réponse :**
> Utiliser `ActivatedRoute.params` qui est un Observable :
> 
> ```typescript
> // Route: /users/:id
> 
> ngOnInit() {
>   // ❌ MAUVAIS : Snapshot (ne réagit pas aux changements)
>   const id = this.route.snapshot.params['id'];
>   
>   // ✅ BON : Observable (réagit aux changements)
>   this.user$ = this.route.params.pipe(
>     switchMap(params => {
>       const id = +params['id'];
>       return this.userService.getUserById(id);
>     })
>   );
> }
> ```
> 
> **Pourquoi Observable ?**
> - Permet de réagir aux changements de route sans recharger le composant
> - Utile pour la navigation entre éléments similaires

### Q7 : Qu'est-ce que le pattern takeUntil et pourquoi l'utiliser ?

**Réponse :**
> **`takeUntil()`** permet de désouscrire automatiquement tous les Observables :
> 
> ```typescript
> export class MyComponent implements OnInit, OnDestroy {
>   private destroy$ = new Subject<void>();
>   
>   ngOnInit() {
>     this.service1.getData()
>       .pipe(takeUntil(this.destroy$))
>       .subscribe(data => console.log(data));
>     
>     this.service2.getOtherData()
>       .pipe(takeUntil(this.destroy$))
>       .subscribe(data => console.log(data));
>   }
>   
>   ngOnDestroy() {
>     this.destroy$.next();
>     this.destroy$.complete();
>   }
> }
> ```
> 
> **Avantages** :
> - Un seul point de désabonnement
> - Code plus propre
> - Évite les memory leaks
> - Pattern recommandé par Angular

### Q8 : Comment implémenter un State Management simple dans Angular ?

**Réponse :**
> Utiliser un **Service avec BehaviorSubject** :
> 
> ```typescript
> interface AppState {
>   user: User | null;
>   isLoading: boolean;
> }
> 
> @Injectable({ providedIn: 'root' })
> export class StateService {
>   private state = new BehaviorSubject<AppState>({
>     user: null,
>     isLoading: false
>   });
>   
>   // Observable public (read-only)
>   state$ = this.state.asObservable();
>   
>   // Selectors
>   user$ = this.state$.pipe(
>     map(state => state.user),
>     distinctUntilChanged()
>   );
>   
>   // Actions
>   setUser(user: User) {
>     this.state.next({
>       ...this.state.value,
>       user
>     });
>   }
> }
> ```
> 
> **Pattern** :
> - BehaviorSubject privé (write)
> - Observable public (read)
> - Immutabilité des données
> - Selectors avec `map()`

### Q9 : Quelle est la différence entre FormControl.valueChanges et FormControl.statusChanges ?

**Réponse :**
> **`valueChanges`** :
> - Émet quand la **valeur** du champ change
> - Retourne la nouvelle valeur
> 
> **`statusChanges`** :
> - Émet quand le **statut de validation** change
> - Retourne : 'VALID', 'INVALID', 'PENDING', 'DISABLED'
> 
> ```typescript
> const emailControl = new FormControl('', [
>   Validators.required,
>   Validators.email
> ]);
> 
> // Écouter les changements de valeur
> emailControl.valueChanges.subscribe(value => {
>   console.log('Valeur:', value);
> });
> // Output: "test", "test@", "test@gmail.com"
> 
> // Écouter les changements de statut
> emailControl.statusChanges.subscribe(status => {
>   console.log('Statut:', status);
> });
> // Output: "INVALID", "INVALID", "VALID"
> ```

### Q10 : Comment combiner plusieurs requêtes HTTP en Angular ?

**Réponse :**
> Utiliser les **opérateurs de combinaison** :
> 
> **1. forkJoin() - Attendre toutes les requêtes (Parallèle)**
> ```typescript
> forkJoin({
>   users: this.http.get<User[]>('/api/users'),
>   posts: this.http.get<Post[]>('/api/posts'),
>   comments: this.http.get<Comment[]>('/api/comments')
> }).subscribe(({ users, posts, comments }) => {
>   console.log('Tout chargé !');
> });
> ```
> 
> **2. combineLatest() - Réagir aux changements**
> ```typescript
> combineLatest([
>   this.route.params,
>   this.userService.getCurrentUser()
> ]).pipe(
>   switchMap(([params, user]) => {
>     return this.getData(params['id'], user);
>   })
> ).subscribe();
> ```
> 
> **3. concat() - Séquentiel**
> ```typescript
> concat(
>   this.http.post('/api/users', user),
>   this.http.post('/api/profile', profile)
> ).subscribe();
> // profile créé APRÈS user
> ```

### Q11 : Qu'est-ce que shareReplay et quand l'utiliser ?

**Réponse :**
> **`shareReplay()`** permet de :
> - **Partager** le résultat entre plusieurs souscripteurs
> - **Mettre en cache** les dernières émissions
> - Éviter les requêtes HTTP multiples
> 
> ```typescript
> // ❌ SANS shareReplay : 3 requêtes HTTP
> getUsers(): Observable<User[]> {
>   return this.http.get<User[]>('/api/users');
> }
> 
> // Component A
> this.service.getUsers().subscribe(); // Requête 1
> // Component B  
> this.service.getUsers().subscribe(); // Requête 2
> // Component C
> this.service.getUsers().subscribe(); // Requête 3
> 
> // ✅ AVEC shareReplay : 1 seule requête
> getUsers(): Observable<User[]> {
>   return this.http.get<User[]>('/api/users').pipe(
>     shareReplay(1)
>   );
> }
> 
> // Component A, B, C : partagent le même résultat
> ```
> 
> **Configuration** :
> - `bufferSize`: Nombre d'émissions à garder
> - `refCount`: true = libère le cache quand plus de souscripteurs

### Q12 : Comment gérer les erreurs HTTP globalement ?

**Réponse :**
> Utiliser un **Intercepteur d'erreurs** :
> 
> ```typescript
> @Injectable()
> export class ErrorInterceptor implements HttpInterceptor {
>   constructor(
>     private notificationService: NotificationService,
>     private router: Router
>   ) {}
>   
>   intercept(req: HttpRequest<any>, next: HttpHandler) {
>     return next.handle(req).pipe(
>       catchError((error: HttpErrorResponse) => {
>         let errorMessage = '';
>         
>         if (error.error instanceof ErrorEvent) {
>           // Erreur client
>           errorMessage = `Erreur: ${error.error.message}`;
>         } else {
>           // Erreur serveur
>           switch (error.status) {
>             case 400:
>               errorMessage = 'Requête invalide';
>               break;
>             case 401:
>               this.router.navigate(['/login']);
>               errorMessage = 'Non authentifié';
>               break;
>             case 403:
>               errorMessage = 'Accès refusé';
>               break;
>             case 404:
>               errorMessage = 'Ressource non trouvée';
>               break;
>             case 500:
>               errorMessage = 'Erreur serveur';
>               break;
>             default:
>               errorMessage = `Erreur ${error.status}`;
>           }
>         }
>         
>         this.notificationService.error(errorMessage);
>         return throwError(() => error);
>       })
>     );
>   }
> }
> ```

### Q13 : Comment créer un formulaire dynamique avec FormArray ?

**Réponse :**
> **FormArray** permet de gérer des listes dynamiques :
> 
> ```typescript
> export class DynamicFormComponent {
>   form: FormGroup;
>   
>   constructor(private fb: FormBuilder) {
>     this.form = this.fb.group({
>       name: ['', Validators.required],
>       items: this.fb.array([]) // FormArray vide
>     });
>   }
>   
>   get items(): FormArray {
>     return this.form.get('items') as FormArray;
>   }
>   
>   addItem() {
>     const itemGroup = this.fb.group({
>       title: ['', Validators.required],
>       quantity: [1, Validators.min(1)]
>     });
>     
>     this.items.push(itemGroup);
>   }
>   
>   removeItem(index: number) {
>     this.items.removeAt(index);
>   }
>   
>   onSubmit() {
>     console.log(this.form.value);
>     // { name: "...", items: [{title: "...", quantity: 1}, ...] }
>   }
> }
> ```
> 
> **Template** :
> ```html
> <form [formGroup]="form">
>   <input formControlName="name">
>   
>   <div formArrayName="items">
>     <div *ngFor="let item of items.controls; let i = index" 
>          [formGroupName]="i">
>       <input formControlName="title">
>       <input formControlName="quantity" type="number">
>       <button (click)="removeItem(i)">X</button>
>     </div>
>   </div>
>   
>   <button (click)="addItem()">Ajouter</button>
> </form>
> ```

### Q14 : Quelle est la différence entre switchMap, mergeMap et concatMap ?

**Réponse :**
> **`switchMap()`** :
> - **Annule** la requête précédente
> - Garde seulement la dernière
> - **Cas d'usage** : Recherche, autocomplete
> 
> ```typescript
> searchControl.valueChanges.pipe(
>   switchMap(query => this.search(query))
> );
> // Si l'utilisateur tape vite, seule la dernière recherche compte
> ```
> 
> **`mergeMap()`** :
> - Exécute **toutes** les requêtes en parallèle
> - N'annule rien
> - **Cas d'usage** : Sauvegarder plusieurs éléments
> 
> ```typescript
> users$.pipe(
>   mergeMap(user => this.http.post('/api/users', user))
> );
> // Toutes les sauvegardes s'exécutent
> ```
> 
> **`concatMap()`** :
> - Exécute en **séquence** (une après l'autre)
> - Préserve l'ordre
> - **Cas d'usage** : Opérations ordonnées
> 
> ```typescript
> operations$.pipe(
>   concatMap(op => this.execute(op))
> );
> // op2 attend que op1 soit terminée
> ```

### Q15 : Comment implémenter un système de retry avec backoff exponentiel ?

**Réponse :**
> Utiliser **`retryWhen()`** avec un délai croissant :
> 
> ```typescript
> import { retryWhen, delay, take, scan } from 'rxjs/operators';
> 
> this.http.get('/api/data').pipe(
>   retryWhen(errors => 
>     errors.pipe(
>       scan((retryCount, err) => {
>         if (retryCount >= 3) {
>           throw err; // Abandonner après 3 essais
>         }
>         console.log(`Retry ${retryCount + 1}/3`);
>         return retryCount + 1;
>       }, 0),
>       delay(1000) // Délai d'1 seconde entre chaque retry
>     )
>   )
> ).subscribe();
> 
> // Version avec backoff exponentiel
> retryWithBackoff(maxRetries: number = 3) {
>   return (errors: Observable<any>) => errors.pipe(
>     scan((retryCount, err) => {
>       if (retryCount >= maxRetries) throw err;
>       return retryCount + 1;
>     }, 0),
>     delayWhen(retryCount => 
>       timer(Math.pow(2, retryCount) * 1000)
>     )
>     // Délai : 2s, 4s, 8s, 16s...
>   );
> }
> 
> // Utilisation
> this.http.get('/api/data').pipe(
>   retryWhen(this.retryWithBackoff(4))
> ).subscribe();
> ```

---

## 🎯 Points Clés du Chapitre 7

### Concepts Essentiels
✅ **HttpClient** : Retourne des Observables auto-complétés
✅ **Intercepteurs** : Modifier toutes les requêtes/réponses
✅ **ShareReplay** : Cache et partage entre souscripteurs
✅ **FormControl.valueChanges** : Écouter les changements de formulaire
✅ **ActivatedRoute.params** : Paramètres de route en Observable
✅ **State Management** : Service avec BehaviorSubject

### Patterns Importants
✅ **AsyncPipe** : Préférer à subscribe() manuel
✅ **takeUntil** : Désabonnement automatique propre
✅ **forkJoin** : Combiner plusieurs requêtes parallèles
✅ **switchMap** : Annuler requêtes précédentes (recherche)
✅ **Validation asynchrone** : ValidatorFn retournant Observable

### Best Practices
✅ **Toujours utiliser AsyncPipe** quand possible
✅ **Centraliser le state** dans des services
✅ **Gérer les erreurs** avec intercepteurs
✅ **Cacher les données** avec shareReplay
✅ **Unsubscribe** avec takeUntil pattern
✅ **OnPush ChangeDetection** pour performance

### Erreurs à Éviter
❌ Oublier de unsubscribe (memory leaks)
❌ Ne pas gérer les erreurs HTTP
❌ Requêtes multiples sans cache
❌ Utiliser snapshot au lieu d'Observable pour les routes
❌ Subscribe manuel au lieu d'AsyncPipe
❌ Mutations directes du state

---

## 📚 Ressources Complémentaires

### Documentation Officielle
- [Angular HttpClient](https://angular.io/guide/http)
- [Angular Reactive Forms](https://angular.io/guide/reactive-forms)
- [Angular Router](https://angular.io/guide/router)
- [RxJS Operators](https://rxjs.dev/guide/operators)

### Patterns Avancés
- [NgRx](https://ngrx.io/) - State management avancé
- [Akita](https://datorama.github.io/akita/) - Alternative à NgRx
- [RxAngular](https://www.rx-angular.io/) - Outils RxJS pour Angular

### Articles Recommandés
- "The Ultimate Answer To The Very Common Angular Question: subscribe() vs | async Pipe"
- "RxJS Best Practices in Angular"
- "Managing State in Angular with RxJS"
- "Advanced Caching with RxJS"

---

## 🎓 Résumé pour l'Entretien

### Questions Essentielles à Maîtriser

1. **HttpClient** : Pourquoi Observable et pas Promise ?
2. **AsyncPipe** : Avantages vs subscribe manuel
3. **Intercepteurs** : Cas d'usage et implémentation
4. **State Management** : Pattern avec BehaviorSubject
5. **Unsubscribe** : Différentes techniques (takeUntil, AsyncPipe)
6. **Formulaires** : valueChanges, validation asynchrone
7. **Router** : params vs queryParams, Guards
8. **Combinaison** : forkJoin, combineLatest, concat
9. **Cache** : shareReplay configuration
10. **Erreurs** : Gestion globale et retry strategies

### Démonstration Pratique

Être capable de :
- Créer un service HTTP complet avec cache
- Implémenter un formulaire réactif avec validation
- Gérer le state d'une application
- Utiliser les intercepteurs correctement
- Combiner plusieurs Observables
- Éviter les memory leaks

### Code à Connaître par Cœur

```typescript
// Service HTTP avec cache
@Injectable({ providedIn: 'root' })
export class DataService {
  private cache$ = this.http.get('/api/data').pipe(
    shareReplay(1)
  );
}

// takeUntil pattern
private destroy$ = new Subject<void>();
ngOnInit() {
  this.data$.pipe(takeUntil(this.destroy$)).subscribe();
}
ngOnDestroy() {
  this.destroy$.next();
  this.destroy$.complete();
}

// State management
private state = new BehaviorSubject<State>(initialState);
state$ = this.state.asObservable();
updateState(partial: Partial<State>) {
  this.state.next({ ...this.state.value, ...partial });
}
```

---