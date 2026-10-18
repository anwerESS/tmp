# Chapitre 14 : Tests Unitaires avec Jasmine et Karma

## Introduction

Les tests sont **essentiels** pour garantir la qualité et la maintenabilité d'une application Angular. Ce chapitre couvre en profondeur les tests unitaires avec Jasmine (framework de test) et Karma (test runner).

## Pourquoi Tester ?

### Avantages des Tests

- ✅ **Confiance** : Refactorer sans craindre de casser l'existant
- ✅ **Documentation** : Les tests documentent le comportement attendu
- ✅ **Qualité** : Détection précoce des bugs
- ✅ **Maintenance** : Facilite les modifications futures
- ✅ **Régression** : Évite la réintroduction de bugs corrigés
- ✅ **Design** : Force une meilleure architecture (code testable = code bien conçu)

### Types de Tests

```
Pyramide des Tests
        /\
       /E2E\      10% - Tests End-to-End (Protractor/Cypress)
      /______\
     /        \
    /Integration\ 20% - Tests d'Intégration
   /____________\
  /              \
 /   Unit Tests   \ 70% - Tests Unitaires (Jasmine/Karma)
/__________________\
```

Ce chapitre se concentre sur les **tests unitaires** qui forment la base solide.

## Configuration et Outils

### Jasmine

**Jasmine** est un framework de test behavior-driven (BDD) pour JavaScript.

**Caractéristiques** :
- Syntaxe claire et expressive
- Pas de dépendances externes
- Assertions intégrées
- Mocking et spying natifs

**Installation** : Déjà inclus avec Angular CLI

### Karma

**Karma** est un test runner qui exécute les tests dans de vrais navigateurs.

**Caractéristiques** :
- Exécute les tests dans Chrome, Firefox, Safari, etc.
- Watch mode pour re-exécuter automatiquement
- Rapport de couverture de code
- Intégration CI/CD

### Configuration

**karma.conf.js** (généré automatiquement) :
```javascript
module.exports = function(config) {
  config.set({
    basePath: '',
    frameworks: ['jasmine', '@angular-devkit/build-angular'],
    plugins: [
      require('karma-jasmine'),
      require('karma-chrome-launcher'),
      require('karma-coverage')
    ],
    browsers: ['Chrome'],
    singleRun: false,
    restartOnFileChange: true
  });
};
```

## Anatomie d'un Test

### Structure de Base

```typescript
// user.service.spec.ts
import { TestBed } from '@angular/core/testing';
import { UserService } from './user.service';

describe('UserService', () => {
  let service: UserService;
  
  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(UserService);
  });
  
  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
```

### Blocs Principaux

#### describe() - Suite de Tests

```typescript
describe('UserService', () => {
  // Tests regroupés pour UserService
  
  describe('addUser', () => {
    // Tests spécifiques à la méthode addUser
  });
  
  describe('deleteUser', () => {
    // Tests spécifiques à la méthode deleteUser
  });
});
```

Les `describe()` peuvent être **imbriqués** pour organiser logiquement.

#### it() - Test Individuel

```typescript
it('should add a user to the list', () => {
  // Arrange (Préparer)
  const user = { id: 1, name: 'John' };
  
  // Act (Agir)
  service.addUser(user);
  
  // Assert (Vérifier)
  expect(service.users()).toContain(user);
});
```

**Pattern AAA** (Arrange, Act, Assert) :
- **Arrange** : Préparer les données
- **Act** : Exécuter l'action testée
- **Assert** : Vérifier le résultat

#### beforeEach() - Configuration

```typescript
beforeEach(() => {
  // Exécuté AVANT chaque test it()
  service = new UserService();
});
```

#### afterEach() - Nettoyage

```typescript
afterEach(() => {
  // Exécuté APRÈS chaque test it()
  service.cleanup();
});
```

#### beforeAll() et afterAll()

```typescript
beforeAll(() => {
  // Exécuté UNE SEULE FOIS avant tous les tests
});

afterAll(() => {
  // Exécuté UNE SEULE FOIS après tous les tests
});
```

### Matchers (Assertions)

#### Égalité

```typescript
// Égalité stricte (===)
expect(value).toBe(4);
expect(value).toBe('hello');
expect(value).toBe(true);

// Égalité profonde (objets/tableaux)
expect(user).toEqual({ id: 1, name: 'John' });
expect(array).toEqual([1, 2, 3]);

// Identité (même référence)
const obj = { a: 1 };
expect(obj).toBe(obj);  // ✅
expect(obj).toBe({ a: 1 });  // ❌ Référence différente
expect(obj).toEqual({ a: 1 });  // ✅
```

#### Véracité

```typescript
expect(value).toBeTruthy();   // != null, != undefined, != false, != 0, != ''
expect(value).toBeFalsy();    // null, undefined, false, 0, ''
expect(value).toBeNull();     // === null
expect(value).toBeUndefined(); // === undefined
expect(value).toBeDefined();  // !== undefined
```

#### Comparaisons Numériques

```typescript
expect(age).toBeGreaterThan(17);
expect(age).toBeGreaterThanOrEqual(18);
expect(age).toBeLessThan(100);
expect(age).toBeLessThanOrEqual(99);

// Nombres décimaux (avec précision)
expect(0.1 + 0.2).toBeCloseTo(0.3, 2);  // Précision: 2 décimales
```

#### Chaînes de Caractères

```typescript
expect(message).toContain('Angular');
expect(message).toMatch(/^Hello/);  // Regex
expect(message).toMatch('Hello');   // String
```

#### Tableaux et Collections

```typescript
expect(array).toContain('item');
expect(array.length).toBe(5);
expect(set).toContain('value');
```

#### Exceptions

```typescript
expect(() => service.throwError()).toThrow();
expect(() => service.throwError()).toThrowError('Error message');
expect(() => service.throwError()).toThrowError(TypeError);
```

#### Négation

```typescript
expect(value).not.toBe(null);
expect(value).not.toContain('test');
```

## Tester un Service

### Service Simple

**user.service.ts** :
```typescript
import { Injectable, signal } from '@angular/core';

export interface User {
  id: number;
  name: string;
  email: string;
}

@Injectable({
  providedIn: 'root'
})
export class UserService {
  private usersSignal = signal<User[]>([]);
  
  users = this.usersSignal.asReadonly();
  
  addUser(user: User): void {
    this.usersSignal.update(users => [...users, user]);
  }
  
  getUserById(id: number): User | undefined {
    return this.usersSignal().find(u => u.id === id);
  }
  
  deleteUser(id: number): void {
    this.usersSignal.update(users => users.filter(u => u.id !== id));
  }
  
  updateUser(id: number, updates: Partial<User>): void {
    this.usersSignal.update(users =>
      users.map(u => u.id === id ? { ...u, ...updates } : u)
    );
  }
  
  clearUsers(): void {
    this.usersSignal.set([]);
  }
}
```

**user.service.spec.ts** :
```typescript
import { TestBed } from '@angular/core/testing';
import { UserService, User } from './user.service';

describe('UserService', () => {
  let service: UserService;
  
  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(UserService);
  });
  
  it('should be created', () => {
    expect(service).toBeTruthy();
  });
  
  describe('addUser', () => {
    it('should add a user to the list', () => {
      // Arrange
      const user: User = { id: 1, name: 'John Doe', email: 'john@example.com' };
      
      // Act
      service.addUser(user);
      
      // Assert
      expect(service.users()).toContain(user);
      expect(service.users().length).toBe(1);
    });
    
    it('should add multiple users', () => {
      const user1: User = { id: 1, name: 'John', email: 'john@example.com' };
      const user2: User = { id: 2, name: 'Jane', email: 'jane@example.com' };
      
      service.addUser(user1);
      service.addUser(user2);
      
      expect(service.users().length).toBe(2);
      expect(service.users()).toEqual([user1, user2]);
    });
  });
  
  describe('getUserById', () => {
    beforeEach(() => {
      service.addUser({ id: 1, name: 'John', email: 'john@example.com' });
      service.addUser({ id: 2, name: 'Jane', email: 'jane@example.com' });
    });
    
    it('should return user when exists', () => {
      const user = service.getUserById(1);
      
      expect(user).toBeDefined();
      expect(user?.name).toBe('John');
    });
    
    it('should return undefined when user does not exist', () => {
      const user = service.getUserById(999);
      
      expect(user).toBeUndefined();
    });
  });
  
  describe('deleteUser', () => {
    beforeEach(() => {
      service.addUser({ id: 1, name: 'John', email: 'john@example.com' });
      service.addUser({ id: 2, name: 'Jane', email: 'jane@example.com' });
    });
    
    it('should remove user from list', () => {
      service.deleteUser(1);
      
      expect(service.users().length).toBe(1);
      expect(service.getUserById(1)).toBeUndefined();
      expect(service.getUserById(2)).toBeDefined();
    });
    
    it('should do nothing if user does not exist', () => {
      service.deleteUser(999);
      
      expect(service.users().length).toBe(2);
    });
  });
  
  describe('updateUser', () => {
    beforeEach(() => {
      service.addUser({ id: 1, name: 'John', email: 'john@example.com' });
    });
    
    it('should update user properties', () => {
      service.updateUser(1, { name: 'John Updated' });
      
      const user = service.getUserById(1);
      expect(user?.name).toBe('John Updated');
      expect(user?.email).toBe('john@example.com'); // Inchangé
    });
    
    it('should update multiple properties', () => {
      service.updateUser(1, { 
        name: 'John Updated', 
        email: 'new@example.com' 
      });
      
      const user = service.getUserById(1);
      expect(user).toEqual({ 
        id: 1, 
        name: 'John Updated', 
        email: 'new@example.com' 
      });
    });
  });
  
  describe('clearUsers', () => {
    it('should remove all users', () => {
      service.addUser({ id: 1, name: 'John', email: 'john@example.com' });
      service.addUser({ id: 2, name: 'Jane', email: 'jane@example.com' });
      
      service.clearUsers();
      
      expect(service.users().length).toBe(0);
    });
  });
});
```

### Service avec Dépendances

**auth.service.ts** :
```typescript
import { Injectable, inject } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';

export interface LoginCredentials {
  username: string;
  password: string;
}

@Injectable({
  providedIn: 'root'
})
export class AuthService {
  private http = inject(HttpClient);
  private apiUrl = 'http://localhost:3000';
  
  login(credentials: LoginCredentials): Observable<{ token: string }> {
    return this.http.post<{ token: string }>(
      `${this.apiUrl}/login`, 
      credentials
    );
  }
  
  logout(): Observable<void> {
    return this.http.post<void>(`${this.apiUrl}/logout`, {});
  }
}
```

**auth.service.spec.ts** :
```typescript
import { TestBed } from '@angular/core/testing';
import { HttpClientTestingModule, HttpTestingController } from '@angular/common/http/testing';
import { AuthService, LoginCredentials } from './auth.service';

describe('AuthService', () => {
  let service: AuthService;
  let httpMock: HttpTestingController;
  
  beforeEach(() => {
    TestBed.configureTestingModule({
      imports: [HttpClientTestingModule]
    });
    
    service = TestBed.inject(AuthService);
    httpMock = TestBed.inject(HttpTestingController);
  });
  
  afterEach(() => {
    // Vérifier qu'aucune requête HTTP n'est en attente
    httpMock.verify();
  });
  
  it('should be created', () => {
    expect(service).toBeTruthy();
  });
  
  describe('login', () => {
    it('should send POST request with credentials', () => {
      const credentials: LoginCredentials = {
        username: 'admin',
        password: 'admin1234'
      };
      const mockResponse = { token: 'fake-jwt-token' };
      
      // S'abonner à la réponse
      service.login(credentials).subscribe(response => {
        expect(response).toEqual(mockResponse);
      });
      
      // Vérifier la requête
      const req = httpMock.expectOne('http://localhost:3000/login');
      expect(req.request.method).toBe('POST');
      expect(req.request.body).toEqual(credentials);
      
      // Simuler la réponse
      req.flush(mockResponse);
    });
    
    it('should handle login error', () => {
      const credentials: LoginCredentials = {
        username: 'wrong',
        password: 'wrong'
      };
      
      service.login(credentials).subscribe({
        next: () => fail('Should have failed'),
        error: (error) => {
          expect(error.status).toBe(401);
        }
      });
      
      const req = httpMock.expectOne('http://localhost:3000/login');
      req.flush('Invalid credentials', { 
        status: 401, 
        statusText: 'Unauthorized' 
      });
    });
  });
  
  describe('logout', () => {
    it('should send POST request to logout endpoint', () => {
      service.logout().subscribe();
      
      const req = httpMock.expectOne('http://localhost:3000/logout');
      expect(req.request.method).toBe('POST');
      
      req.flush(null);
    });
  });
});
```

## Tester un Component

### Component Simple

**counter.component.ts** :
```typescript
import { Component, signal } from '@angular/core';

@Component({
  selector: 'app-counter',
  standalone: true,
  template: `
    <div class="counter">
      <h2>Counter: {{ count() }}</h2>
      <button (click)="increment()" class="btn-increment">+</button>
      <button (click)="decrement()" class="btn-decrement">-</button>
      <button (click)="reset()" class="btn-reset">Reset</button>
    </div>
  `,
  styles: [`
    .counter {
      padding: 20px;
      text-align: center;
    }
  `]
})
export class CounterComponent {
  count = signal(0);
  
  increment(): void {
    this.count.update(n => n + 1);
  }
  
  decrement(): void {
    this.count.update(n => n - 1);
  }
  
  reset(): void {
    this.count.set(0);
  }
}
```

**counter.component.spec.ts** :
```typescript
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { CounterComponent } from './counter.component';

describe('CounterComponent', () => {
  let component: CounterComponent;
  let fixture: ComponentFixture<CounterComponent>;
  let compiled: HTMLElement;
  
  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [CounterComponent]
    }).compileComponents();
    
    fixture = TestBed.createComponent(CounterComponent);
    component = fixture.componentInstance;
    compiled = fixture.nativeElement;
    fixture.detectChanges();
  });
  
  it('should create', () => {
    expect(component).toBeTruthy();
  });
  
  describe('Initial state', () => {
    it('should initialize with count at 0', () => {
      expect(component.count()).toBe(0);
    });
    
    it('should display initial count in template', () => {
      const h2 = compiled.querySelector('h2');
      expect(h2?.textContent).toContain('Counter: 0');
    });
  });
  
  describe('increment', () => {
    it('should increment count when increment is called', () => {
      component.increment();
      
      expect(component.count()).toBe(1);
    });
    
    it('should update template when incremented', () => {
      component.increment();
      fixture.detectChanges();
      
      const h2 = compiled.querySelector('h2');
      expect(h2?.textContent).toContain('Counter: 1');
    });
    
    it('should increment when increment button is clicked', () => {
      const button = compiled.querySelector('.btn-increment') as HTMLButtonElement;
      
      button.click();
      fixture.detectChanges();
      
      expect(component.count()).toBe(1);
    });
    
    it('should increment multiple times', () => {
      component.increment();
      component.increment();
      component.increment();
      
      expect(component.count()).toBe(3);
    });
  });
  
  describe('decrement', () => {
    it('should decrement count', () => {
      component.count.set(5);
      component.decrement();
      
      expect(component.count()).toBe(4);
    });
    
    it('should allow negative values', () => {
      component.decrement();
      
      expect(component.count()).toBe(-1);
    });
    
    it('should decrement when button is clicked', () => {
      const button = compiled.querySelector('.btn-decrement') as HTMLButtonElement;
      
      button.click();
      fixture.detectChanges();
      
      expect(component.count()).toBe(-1);
    });
  });
  
  describe('reset', () => {
    it('should reset count to 0', () => {
      component.count.set(10);
      component.reset();
      
      expect(component.count()).toBe(0);
    });
    
    it('should reset from negative value', () => {
      component.count.set(-5);
      component.reset();
      
      expect(component.count()).toBe(0);
    });
    
    it('should reset when reset button is clicked', () => {
      component.count.set(7);
      
      const button = compiled.querySelector('.btn-reset') as HTMLButtonElement;
      button.click();
      fixture.detectChanges();
      
      expect(component.count()).toBe(0);
    });
  });
});
```

### Component avec Inputs/Outputs

**user-card.component.ts** :
```typescript
import { Component, input, output } from '@angular/core';

export interface User {
  id: number;
  name: string;
  email: string;
}

@Component({
  selector: 'app-user-card',
  standalone: true,
  template: `
    <div class="user-card">
      <h3>{{ user().name }}</h3>
      <p>{{ user().email }}</p>
      <button (click)="handleEdit()" class="btn-edit">Edit</button>
      <button (click)="handleDelete()" class="btn-delete">Delete</button>
    </div>
  `
})
export class UserCardComponent {
  user = input.required<User>();
  
  userEdit = output<User>();
  userDelete = output<number>();
  
  handleEdit(): void {
    this.userEdit.emit(this.user());
  }
  
  handleDelete(): void {
    this.userDelete.emit(this.user().id);
  }
}
```

**user-card.component.spec.ts** :
```typescript
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { UserCardComponent, User } from './user-card.component';

describe('UserCardComponent', () => {
  let component: UserCardComponent;
  let fixture: ComponentFixture<UserCardComponent>;
  let compiled: HTMLElement;
  
  const mockUser: User = {
    id: 1,
    name: 'John Doe',
    email: 'john@example.com'
  };
  
  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [UserCardComponent]
    }).compileComponents();
    
    fixture = TestBed.createComponent(UserCardComponent);
    component = fixture.componentInstance;
    compiled = fixture.nativeElement;
    
    // Définir l'input requis
    fixture.componentRef.setInput('user', mockUser);
    fixture.detectChanges();
  });
  
  it('should create', () => {
    expect(component).toBeTruthy();
  });
  
  describe('Display', () => {
    it('should display user name', () => {
      const h3 = compiled.querySelector('h3');
      expect(h3?.textContent).toBe('John Doe');
    });
    
    it('should display user email', () => {
      const p = compiled.querySelector('p');
      expect(p?.textContent).toBe('john@example.com');
    });
    
    it('should update display when user input changes', () => {
      const newUser: User = {
        id: 2,
        name: 'Jane Smith',
        email: 'jane@example.com'
      };
      
      fixture.componentRef.setInput('user', newUser);
      fixture.detectChanges();
      
      const h3 = compiled.querySelector('h3');
      expect(h3?.textContent).toBe('Jane Smith');
    });
  });
  
  describe('Edit event', () => {
    it('should emit user when edit button is clicked', () => {
      let emittedUser: User | undefined;
      
      component.userEdit.subscribe((user: User) => {
        emittedUser = user;
      });
      
      const button = compiled.querySelector('.btn-edit') as HTMLButtonElement;
      button.click();
      
      expect(emittedUser).toEqual(mockUser);
    });
    
    it('should emit correct user after user change', () => {
      const newUser: User = {
        id: 2,
        name: 'Jane',
        email: 'jane@example.com'
      };
      
      fixture.componentRef.setInput('user', newUser);
      fixture.detectChanges();
      
      let emittedUser: User | undefined;
      component.userEdit.subscribe(user => emittedUser = user);
      
      component.handleEdit();
      
      expect(emittedUser).toEqual(newUser);
    });
  });
  
  describe('Delete event', () => {
    it('should emit user id when delete button is clicked', () => {
      let emittedId: number | undefined;
      
      component.userDelete.subscribe((id: number) => {
        emittedId = id;
      });
      
      const button = compiled.querySelector('.btn-delete') as HTMLButtonElement;
      button.click();
      
      expect(emittedId).toBe(1);
    });
  });
});
```

### Component avec Service

**user-list.component.ts** :
```typescript
import { Component, inject, OnInit, signal } from '@angular/core';
import { UserService, User } from '../services/user.service';

@Component({
  selector: 'app-user-list',
  standalone: true,
  template: `
    <div class="user-list">
      <h2>Users ({{ users().length }})</h2>
      <ul>
        @for (user of users(); track user.id) {
          <li>{{ user.name }} - {{ user.email }}</li>
        }
      </ul>
      <button (click)="loadUsers()" class="btn-load">Load Users</button>
    </div>
  `
})
export class UserListComponent implements OnInit {
  private userService = inject(UserService);
  
  users = signal<User[]>([]);
  
  ngOnInit(): void {
    this.loadUsers();
  }
  
  loadUsers(): void {
    this.users.set(this.userService.users());
  }
}
```

**user-list.component.spec.ts** :
```typescript
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { UserListComponent } from './user-list.component';
import { UserService, User } from '../services/user.service';

describe('UserListComponent', () => {
  let component: UserListComponent;
  let fixture: ComponentFixture<UserListComponent>;
  let userService: UserService;
  let compiled: HTMLElement;
  
  const mockUsers: User[] = [
    { id: 1, name: 'John Doe', email: 'john@example.com' },
    { id: 2, name: 'Jane Smith', email: 'jane@example.com' }
  ];
  
  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [UserListComponent],
      providers: [UserService]
    }).compileComponents();
    
    fixture = TestBed.createComponent(UserListComponent);
    component = fixture.componentInstance;
    compiled = fixture.nativeElement;
    userService = TestBed.inject(UserService);
    
    // Pré-remplir le service avec des données de test
    mockUsers.forEach(user => userService.addUser(user));
  });
  
  it('should create', () => {
    expect(component).toBeTruthy();
  });
  
  describe('ngOnInit', () => {
    it('should load users on init', () => {
      fixture.detectChanges(); // Déclenche ngOnInit
      
      expect(component.users().length).toBe(2);
      expect(component.users()).toEqual(mockUsers);
    });
  });
  
  describe('Display', () => {
    beforeEach(() => {
      fixture.detectChanges();
    });
    
    it('should display user count', () => {
      const h2 = compiled.querySelector('h2');
      expect(h2?.textContent).toContain('Users (2)');
    });
    
    it('should display all users', () => {
      const items = compiled.querySelectorAll('li');
      expect(items.length).toBe(2);
    });
    
    it('should display user details', () => {
      const items = compiled.querySelectorAll('li');
      expect(items[0].textContent).toContain('John Doe');
      expect(items[0].textContent).toContain('john@example.com');
    });
  });
  
  describe('loadUsers', () => {
    it('should refresh users list', () => {
      fixture.detectChanges();
      
      // Ajouter un utilisateur au service
      const newUser: User = { 
        id: 3, 
        name: 'Bob', 
        email: 'bob@example.com' 
      };
      userService.addUser(newUser);
      
      // Charger à nouveau
      component.loadUsers();
      fixture.detectChanges();
      
      expect(component.users().length).toBe(3);
      
      const h2 = compiled.querySelector('h2');
      expect(h2?.textContent).toContain('Users (3)');
    });
  });
});
```

## Tester les Formulaires

### Reactive Form

**login-form.component.ts** :
```typescript
import { Component, inject } from '@angular/core';
import { FormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';

@Component({
  selector: 'app-login-form',
  standalone: true,
  imports: [ReactiveFormsModule],
  template: `
    <form [formGroup]="loginForm" (submit)="onSubmit()">
      <input formControlName="email" placeholder="Email">
      <input formControlName="password" type="password" placeholder="Password">
      <button type="submit" [disabled]="loginForm.invalid">Login</button>
    </form>
  `
})
export class LoginFormComponent {
  private formBuilder = inject(FormBuilder);
  
  loginForm = this.formBuilder.group({
    email: ['', [Validators.required, Validators.email]],
    password: ['', [Validators.required, Validators.minLength(8)]]
  });
  
  onSubmit(): void {
    if (this.loginForm.valid) {
      console.log('Login:', this.loginForm.value);
    }
  }
}
```

**login-form.component.spec.ts** :
```typescript
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { LoginFormComponent } from './login-form.component';

describe('LoginFormComponent', () => {
  let component: LoginFormComponent;
  let fixture: ComponentFixture<LoginFormComponent>;
  
  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [LoginFormComponent]
    }).compileComponents();
    
    fixture = TestBed.createComponent(LoginFormComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });
  
  it('should create', () => {
    expect(component).toBeTruthy();
  });
  
  describe('Form initialization', () => {
    it('should create form with email and password controls', () => {
      expect(component.loginForm.contains('email')).toBe(true);
      expect(component.loginForm.contains('password')).toBe(true);
    });
    
    it('should initialize with empty values', () => {
      expect(component.loginForm.get('email')?.value).toBe('');
      expect(component.loginForm.get('password')?.value).toBe('');
    });
    
    it('should be invalid when empty', () => {
      expect(component.loginForm.valid).toBe(false);
    });
  });
  
  describe('Email validation', () => {
    it('should require email', () => {
      const emailControl = component.loginForm.get('email');
      
      emailControl?.setValue('');
      expect(emailControl?.hasError('required')).toBe(true);
    });
    
    it('should validate email format', () => {
      const emailControl = component.loginForm.get('email');
      
      emailControl?.setValue('invalid-email');
      expect(emailControl?.hasError('email')).toBe(true);
      
      emailControl?.setValue('valid@email.com');
      expect(emailControl?.hasError('email')).toBe(false);
    });
    
    it('should be valid with correct email', () => {
      const emailControl = component.loginForm.get('email');
      
      emailControl?.setValue('test@example.com');
      expect(emailControl?.valid).toBe(true);
    });
  });
  
  describe('Password validation', () => {
    it('should require password', () => {
      const passwordControl = component.loginForm.get('password');
      
      passwordControl?.setValue('');
      expect(passwordControl?.hasError('required')).toBe(true);
    });
    
    it('should require minimum 8 characters', () => {
      const passwordControl = component.loginForm.get('password');
      
      passwordControl?.setValue('short');
      expect(passwordControl?.hasError('minlength')).toBe(true);
      
      passwordControl?.setValue('longenough');
      expect(passwordControl?.hasError('minlength')).toBe(false);
    });
  });
  
  describe('Form validation', () => {
    it('should be valid with correct data', () => {
      component.loginForm.patchValue({
        email: 'test@example.com',
        password: 'password123'
      });
      
      expect(component.loginForm.valid).toBe(true);
    });
    
    it('should be invalid with incorrect email', () => {
      component.loginForm.patchValue({
        email: 'invalid',
        password: 'password123'
      });
      
      expect(component.loginForm.invalid).toBe(true);
    });
    
    it('should be invalid with short password', () => {
      component.loginForm.patchValue({
        email: 'test@example.com',
        password: 'short'
      });
      
      expect(component.loginForm.invalid).toBe(true);
    });
  });
  
  describe('Form submission', () => {
    it('should call onSubmit when form is submitted', () => {
      spyOn(component, 'onSubmit');
      
      component.loginForm.patchValue({
        email: 'test@example.com',
        password: 'password123'
      });
      
      const form = fixture.nativeElement.querySelector('form');
      form.dispatchEvent(new Event('submit'));
      
      expect(component.onSubmit).toHaveBeenCalled();
    });
    
    it('should disable submit button when form is invalid', () => {
      const button = fixture.nativeElement.querySelector('button');
      
      expect(button.disabled).toBe(true);
    });
    
    it('should enable submit button when form is valid', () => {
      component.loginForm.patchValue({
        email: 'test@example.com',
        password: 'password123'
      });
      fixture.detectChanges();
      
      const button = fixture.nativeElement.querySelector('button');
      expect(button.disabled).toBe(false);
    });
  });
});
```

## Spies et Mocking

### Jasmine Spies

**Spy basique** :
```typescript
describe('Calculator', () => {
  let calculator: Calculator;
  
  beforeEach(() => {
    calculator = new Calculator();
  });
  
  it('should call add method', () => {
    // Créer un spy sur la méthode add
    spyOn(calculator, 'add');
    
    calculator.add(1, 2);
    
    expect(calculator.add).toHaveBeenCalled();
    expect(calculator.add).toHaveBeenCalledWith(1, 2);
    expect(calculator.add).toHaveBeenCalledTimes(1);
  });
});
```

**Spy avec retour de valeur** :
```typescript
it('should return mocked value', () => {
  spyOn(calculator, 'add').and.returnValue(100);
  
  const result = calculator.add(1, 2);
  
  expect(result).toBe(100);
});
```

**Spy avec throw** :
```typescript
it('should throw error', () => {
  spyOn(calculator, 'divide').and.throwError('Division by zero');
  
  expect(() => calculator.divide(10, 0)).toThrowError('Division by zero');
});
```

**Spy avec callThrough** :
```typescript
it('should call original method', () => {
  spyOn(calculator, 'add').and.callThrough();
  
  const result = calculator.add(2, 3);
  
  expect(result).toBe(5); // Appelle la vraie méthode
  expect(calculator.add).toHaveBeenCalled();
});
```

**Spy avec callFake** :
```typescript
it('should call fake implementation', () => {
  spyOn(calculator, 'add').and.callFake((a: number, b: number) => {
    return a * b; // Comportement différent
  });
  
  const result = calculator.add(2, 3);
  
  expect(result).toBe(6); // 2 * 3 au lieu de 2 + 3
});
```

### Mock d'un Service

```typescript
describe('UserListComponent with Mock', () => {
  let component: UserListComponent;
  let fixture: ComponentFixture<UserListComponent>;
  let mockUserService: jasmine.SpyObj<UserService>;
  
  beforeEach(async () => {
    // Créer un mock du service
    mockUserService = jasmine.createSpyObj('UserService', [
      'getUsers',
      'addUser',
      'deleteUser'
    ]);
    
    await TestBed.configureTestingModule({
      imports: [UserListComponent],
      providers: [
        { provide: UserService, useValue: mockUserService }
      ]
    }).compileComponents();
    
    fixture = TestBed.createComponent(UserListComponent);
    component = fixture.componentInstance;
  });
  
  it('should call getUsers on init', () => {
    const mockUsers = [
      { id: 1, name: 'John', email: 'john@example.com' }
    ];
    mockUserService.getUsers.and.returnValue(of(mockUsers));
    
    fixture.detectChanges(); // Déclenche ngOnInit
    
    expect(mockUserService.getUsers).toHaveBeenCalled();
  });
});
```

## Tests Asynchrones

### Observable Testing

```typescript
import { fakeAsync, tick } from '@angular/core/testing';

describe('Async tests', () => {
  it('should handle observable', (done) => {
    const observable = of('test').pipe(delay(100));
    
    observable.subscribe(value => {
      expect(value).toBe('test');
      done(); // Signale que le test async est terminé
    });
  });
  
  it('should handle observable with fakeAsync', fakeAsync(() => {
    let value: string = '';
    const observable = of('test').pipe(delay(100));
    
    observable.subscribe(v => value = v);
    
    tick(100); // Avance le temps de 100ms
    
    expect(value).toBe('test');
  }));
});
```

### Promise Testing

```typescript
import { waitForAsync } from '@angular/core/testing';

describe('Promise tests', () => {
  it('should handle promise', waitForAsync(() => {
    const promise = Promise.resolve('test');
    
    promise.then(value => {
      expect(value).toBe('test');
    });
  }));
  
  it('should handle async/await', async () => {
    const promise = Promise.resolve('test');
    
    const value = await promise;
    
    expect(value).toBe('test');
  });
});
```

### HTTP Testing

```typescript
import { HttpClientTestingModule, HttpTestingController } from '@angular/common/http/testing';

describe('HTTP Testing', () => {
  let httpMock: HttpTestingController;
  let service: DataService;
  
  beforeEach(() => {
    TestBed.configureTestingModule({
      imports: [HttpClientTestingModule],
      providers: [DataService]
    });
    
    httpMock = TestBed.inject(HttpTestingController);
    service = TestBed.inject(DataService);
  });
  
  afterEach(() => {
    httpMock.verify(); // Vérifie qu'aucune requête n'est en attente
  });
  
  it('should fetch data', () => {
    const mockData = [{ id: 1, name: 'Test' }];
    
    service.getData().subscribe(data => {
      expect(data).toEqual(mockData);
    });
    
    const req = httpMock.expectOne('http://api.example.com/data');
    expect(req.request.method).toBe('GET');
    
    req.flush(mockData);
  });
  
  it('should handle error', () => {
    service.getData().subscribe({
      next: () => fail('Should have failed'),
      error: (error) => {
        expect(error.status).toBe(404);
      }
    });
    
    const req = httpMock.expectOne('http://api.example.com/data');
    req.flush('Not found', { status: 404, statusText: 'Not Found' });
  });
});
```

## Couverture de Code

### Générer le Rapport

```bash
# Exécuter les tests avec couverture
ng test --code-coverage

# Ou dans karma.conf.js
module.exports = function(config) {
  config.set({
    // ...
    coverageReporter: {
      dir: require('path').join(__dirname, './coverage'),
      subdir: '.',
      reporters: [
        { type: 'html' },
        { type: 'text-summary' },
        { type: 'lcovonly' }
      ]
    }
  });
};
```

### Interpréter les Résultats

```
=============================== Coverage summary ===============================
Statements   : 87.5% ( 35/40 )
Branches     : 75% ( 6/8 )
Functions    : 80% ( 8/10 )
Lines        : 85.7% ( 30/35 )
================================================================================
```

**Métriques** :
- **Statements** : Pourcentage d'instructions exécutées
- **Branches** : Pourcentage de branches (if/else) testées
- **Functions** : Pourcentage de fonctions appelées
- **Lines** : Pourcentage de lignes exécutées

**Objectifs recommandés** :
- Minimum : 70%
- Bon : 80%
- Excellent : 90%+
- **Important** : 100% sur le code critique (auth, paiement, etc.)

### Visualiser le Rapport

Ouvrir `coverage/index.html` dans le navigateur pour voir :
- Vue d'ensemble par fichier
- Code non couvert en rouge
- Branches non testées

## Bonnes Pratiques

### 1. Nommage des Tests

```typescript
// ❌ Mauvais
it('test 1', () => { });
it('works', () => { });

// ✅ Bon
it('should add user to the list', () => { });
it('should throw error when email is invalid', () => { });
it('should disable submit button when form is invalid', () => { });
```

**Pattern** : `should [action] when [condition]`

### 2. Tests Indépendants

```typescript
// ❌ Mauvais - Tests dépendants
describe('UserService', () => {
  it('should add user', () => {
    service.addUser(user);
    expect(service.users.length).toBe(1);
  });
  
  it('should delete user', () => {
    // Dépend du test précédent ❌
    service.deleteUser(1);
    expect(service.users.length).toBe(0);
  });
});

// ✅ Bon - Tests indépendants
describe('UserService', () => {
  beforeEach(() => {
    service = new UserService();
  });
  
  it('should add user', () => {
    service.addUser(user);
    expect(service.users.length).toBe(1);
  });
  
  it('should delete user', () => {
    service.addUser(user); // Setup propre à ce test
    service.deleteUser(1);
    expect(service.users.length).toBe(0);
  });
});
```

### 3. Un Test = Une Assertion Logique

```typescript
// ❌ Mauvais - Trop d'assertions non liées
it('should work', () => {
  service.addUser(user);
  expect(service.users.length).toBe(1);
  
  service.deleteUser(1);
  expect(service.users.length).toBe(0);
  
  service.updateUser(2, { name: 'New' });
  expect(service.getUser(2).name).toBe('New');
});

// ✅ Bon - Tests séparés
it('should add user', () => {
  service.addUser(user);
  expect(service.users.length).toBe(1);
});

it('should delete user', () => {
  service.addUser(user);
  service.deleteUser(1);
  expect(service.users.length).toBe(0);
});

it('should update user', () => {
  service.addUser({ id: 2, name: 'Old' });
  service.updateUser(2, { name: 'New' });
  expect(service.getUser(2).name).toBe('New');
});
```

### 4. Tester le Comportement, Pas l'Implémentation

```typescript
// ❌ Mauvais - Test l'implémentation interne
it('should call private method', () => {
  spyOn(service as any, '_privateMethod');
  service.publicMethod();
  expect((service as any)._privateMethod).toHaveBeenCalled();
});

// ✅ Bon - Test le comportement public
it('should return correct result', () => {
  const result = service.publicMethod();
  expect(result).toBe(expectedValue);
});
```

### 5. Utiliser des Données de Test Réalistes

```typescript
// ❌ Mauvais
const user = { id: 1, name: 'a', email: 'b' };

// ✅ Bon
const user = { 
  id: 1, 
  name: 'John Doe', 
  email: 'john.doe@example.com' 
};
```

### 6. Grouper Logiquement avec describe()

```typescript
describe('UserService', () => {
  describe('addUser', () => {
    it('should add user to empty list', () => { });
    it('should add user to existing list', () => { });
    it('should throw error when user already exists', () => { });
  });
  
  describe('deleteUser', () => {
    it('should delete existing user', () => { });
    it('should do nothing when user does not exist', () => { });
  });
});
```

### 7. Mock les Dépendances

```typescript
// ✅ Bon - Mock HttpClient
TestBed.configureTestingModule({
  imports: [HttpClientTestingModule]
});

// ✅ Bon - Mock Service
const mockService = jasmine.createSpyObj('MyService', ['method1', 'method2']);
TestBed.configureTestingModule({
  providers: [
    { provide: MyService, useValue: mockService }
  ]
});
```

### 8. Nettoyer Après les Tests

```typescript
describe('MyComponent', () => {
  let subscription: Subscription;
  
  afterEach(() => {
    subscription?.unsubscribe();
  });
  
  it('should subscribe to data', () => {
    subscription = service.getData().subscribe(...);
  });
});
```

## Commandes Utiles

```bash
# Exécuter tous les tests
ng test

# Exécuter une seule fois (sans watch)
ng test --watch=false

# Exécuter avec couverture
ng test --code-coverage

# Exécuter dans un navigateur spécifique
ng test --browsers=Chrome
ng test --browsers=ChromeHeadless  # Sans UI (CI/CD)

# Exécuter les tests d'un fichier spécifique
ng test --include='**/user.service.spec.ts'

# Mode debug
ng test --browsers=Chrome --watch=true
# Puis ouvrir Chrome DevTools sur l'URL affichée
```

## Configuration Avancée

### karma.conf.js Personnalisé

```javascript
module.exports = function(config) {
  config.set({
    basePath: '',
    frameworks: ['jasmine', '@angular-devkit/build-angular'],
    
    plugins: [
      require('karma-jasmine'),
      require('karma-chrome-launcher'),
      require('karma-jasmine-html-reporter'),
      require('karma-coverage')
    ],
    
    client: {
      jasmine: {
        random: false, // Désactiver l'ordre aléatoire
        seed: 42,
        stopSpecOnExpectationFailure: false
      },
      clearContext: false
    },
    
    jasmineHtmlReporter: {
      suppressAll: true
    },
    
    coverageReporter: {
      dir: require('path').join(__dirname, './coverage'),
      subdir: '.',
      reporters: [
        { type: 'html' },
        { type: 'text-summary' },
        { type: 'lcovonly' }
      ],
      check: {
        global: {
          statements: 80,
          branches: 80,
          functions: 80,
          lines: 80
        }
      }
    },
    
    reporters: ['progress', 'kjhtml', 'coverage'],
    
    port: 9876,
    colors: true,
    logLevel: config.LOG_INFO,
    autoWatch: true,
    browsers: ['Chrome'],
    singleRun: false,
    restartOnFileChange: true,
    
    // Timeout pour les tests lents
    browserNoActivityTimeout: 30000,
    
    // Configuration CI/CD
    customLaunchers: {
      ChromeHeadlessCI: {
        base: 'ChromeHeadless',
        flags: ['--no-sandbox', '--disable-gpu']
      }
    }
  });
};
```

### Test dans CI/CD

```yaml
# .github/workflows/test.yml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
      - uses: actions/checkout@v2
      
      - name: Setup Node.js
        uses: actions/setup-node@v2
        with:
          node-version: '20'
      
      - name: Install dependencies
        run: npm ci
      
      - name: Run tests
        run: npm run test -- --watch=false --browsers=ChromeHeadlessCI --code-coverage
      
      - name: Upload coverage
        uses: codecov/codecov-action@v2
        with:
          files: ./coverage/lcov.info
```

## Debugging des Tests

### 1. Focus sur un Test Spécifique

```typescript
// Exécuter seulement ce test
fit('should work', () => {
  expect(true).toBe(true);
});

// Exécuter seulement cette suite
fdescribe('MyComponent', () => {
  it('test 1', () => { });
  it('test 2', () => { });
});
```

### 2. Ignorer un Test

```typescript
// Ignorer ce test
xit('should be skipped', () => {
  expect(true).toBe(true);
});

// Ignorer cette suite
xdescribe('MyComponent', () => {
  it('test 1', () => { });
});
```

### 3. Debug dans Chrome DevTools

```bash
ng test --browsers=Chrome
```

Puis :
1. Cliquer sur "DEBUG" dans la page Karma
2. Ouvrir DevTools (F12)
3. Mettre des breakpoints dans les fichiers source
4. Rafraîchir la page

### 4. Console Logs

```typescript
it('should debug', () => {
  console.log('Value:', component.value);
  console.log('Form:', component.form.value);
  
  expect(component.value).toBe(10);
});
```

## Exercices Pratiques

### Exercice 1 : Tester un Service de Calcul

Créez et testez un `CalculatorService` avec :
- `add(a, b)` : addition
- `subtract(a, b)` : soustraction
- `multiply(a, b)` : multiplication
- `divide(a, b)` : division (avec gestion de division par zéro)

### Exercice 2 : Tester un Component de Todo

Créez et testez un `TodoComponent` avec :
- Liste de tâches
- Ajout de tâche
- Suppression de tâche
- Toggle complété/non complété
- Compteur de tâches restantes

### Exercice 3 : Tester un Formulaire de Validation

Créez et testez un `RegistrationFormComponent` avec :
- Email (requis, format email)
- Password (requis, min 8 caractères)
- Confirm Password (doit correspondre au password)
- Age (optionnel, min 18)

### Exercice 4 : Tester un Service HTTP

Créez et testez un `ProductService` qui :
- Récupère la liste des produits (GET)
- Crée un produit (POST)
- Met à jour un produit (PUT)
- Supprime un produit (DELETE)
- Gère les erreurs HTTP

## Questions d'Entretien

### Q1 : Quelle est la différence entre Jasmine et Karma ?

**Réponse** :
- **Jasmine** est un framework de test (les assertions, spies, etc.)
- **Karma** est un test runner (exécute les tests dans les navigateurs)
- Jasmine écrit les tests, Karma les exécute

### Q2 : Qu'est-ce qu'un spy Jasmine ?

**Réponse** :
Un spy permet de :
- Vérifier qu'une fonction a été appelée
- Vérifier avec quels arguments
- Mocker le retour d'une fonction
- Compter le nombre d'appels

### Q3 : Pourquoi utiliser TestBed ?

**Réponse** :
TestBed crée un module de test Angular qui :
- Configure l'environnement de test
- Fournit l'injection de dépendances
- Permet de tester les components avec leur template
- Mock les dépendances nécessaires

### Q4 : Quelle est la différence entre toBe() et toEqual() ?

**Réponse** :
- `toBe()` : égalité stricte (===), même référence
- `toEqual()` : égalité profonde, même valeur

```typescript
const obj1 = { a: 1 };
const obj2 = { a: 1 };

expect(obj1).toBe(obj1);  // ✅
expect(obj1).toBe(obj2);  // ❌ Références différentes
expect(obj1).toEqual(obj2);  // ✅ Mêmes valeurs
```

### Q5 : Comment tester du code asynchrone ?

**Réponse** :
Trois méthodes :
- `done()` callback
- `fakeAsync()` avec `tick()`
- `waitForAsync()` (anciennement `async()`)

### Q6 : Qu'est-ce que HttpClientTestingModule ?

**Réponse** :
Module de test qui mock HttpClient :
- Intercepte les requêtes HTTP
- Permet de vérifier les requêtes
- Simule des réponses
- Évite de vrais appels réseau

### Q7 : Comment mocker un service dans un test de component ?

**Réponse** :
```typescript
const mockService = jasmine.createSpyObj('MyService', ['method1']);

TestBed.configureTestingModule({
  providers: [
    { provide: MyService, useValue: mockService }
  ]
});
```

### Q8 : Qu'est-ce que fixture.detectChanges() ?

**Réponse** :
Déclenche manuellement la détection de changement Angular :
- Met à jour le DOM
- Exécute ngOnInit (la première fois)
- Nécessaire après modification des propriétés

### Q9 : Comment tester un @Input() ?

**Réponse** :
```typescript
fixture.componentRef.setInput('inputName', value);
fixture.detectChanges();
```

### Q10 : Comment tester un @Output() ?

**Réponse** :
```typescript
let emittedValue: any;
component.myOutput.subscribe(value => emittedValue = value);

component.triggerOutput();

expect(emittedValue).toBe(expectedValue);
```

### Q11 : Quelle couverture de code viser ?

**Réponse** :
- Minimum acceptable : 70%
- Bon niveau : 80%
- Excellent : 90%+
- Code critique (auth, paiement) : 100%

### Q12 : Pourquoi les tests sont-ils importants ?

**Réponse** :
- Confiance lors du refactoring
- Documentation du comportement
- Détection précoce des bugs
- Facilite la maintenance
- Réduit les coûts long terme

### Q13 : Qu'est-ce qu'un test unitaire ?

**Réponse** :
Un test qui vérifie **une seule unité** de code (fonction, méthode, component) de manière **isolée**, en mockant toutes les dépendances.

### Q14 : Comment tester un formulaire réactif ?

**Réponse** :
```typescript
it('should validate form', () => {
  const control = component.form.get('email');
  
  control?.setValue('invalid');
  expect(control?.hasError('email')).toBe(true);
  
  control?.setValue('valid@email.com');
  expect(control?.valid).toBe(true);
});
```

### Q15 : Qu'est-ce que le TDD (Test-Driven Development) ?

**Réponse** :
Approche où on écrit le test **avant** le code :
1. Écrire un test qui échoue (Red)
2. Écrire le code minimum pour passer le test (Green)
3. Refactorer (Refactor)
4. Répéter

## Résumé

Les tests Angular avec Jasmine et Karma permettent de :
- ✅ Garantir la qualité du code
- ✅ Détecter les bugs tôt
- ✅ Refactorer en confiance
- ✅ Documenter le comportement
- ✅ Faciliter la maintenance

**Points clés à retenir** :
- Jasmine = framework de test, Karma = test runner
- Pattern AAA : Arrange, Act, Assert
- Utiliser TestBed pour les components et services Angular
- Mock les dépendances avec `jasmine.createSpyObj()`
- Viser 80%+ de couverture de code
- Tests indépendants et déterministes
- Nommer clairement : `should [action] when [condition]`
- Tester le comportement, pas l'implémentation

**Commandes essentielles** :
- `ng test` : exécuter les tests
- `ng test --code-coverage` : avec couverture
- `ng test --watch=false` : exécution unique
- `ng test --browsers=ChromeHeadless` : pour CI/CD

Les tests ne sont pas optionnels sur un projet professionnel - ils sont la **fondation** d'une application maintenable et fiable !

---

*Chapitre 14 terminé - Vous maîtrisez maintenant les tests Angular avec Jasmine !*
