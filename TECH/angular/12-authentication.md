# Chapitre 12 : Authentication - Login, Interceptor, Guard

## Introduction

Ce chapitre couvre l'implémentation d'un système d'authentification complet dans une application Angular. Nous verrons :
- La création d'une page de login
- L'utilisation de HttpClient pour communiquer avec une API
- Les interceptors pour modifier automatiquement les requêtes HTTP
- Les guards pour protéger les routes

## Architecture de l'Authentification

### Flux Typique

1. **Login** : l'utilisateur entre ses identifiants
2. **API** : le backend valide et retourne un token JWT
3. **Stockage** : le token est stocké (localStorage)
4. **Interceptor** : ajoute automatiquement le token à chaque requête
5. **Guard** : vérifie l'authentification avant d'accéder aux routes protégées
6. **Logout** : supprime le token et redirige vers le login

### Structure des Fichiers

```
src/app/
├── models/
│   └── user.ts
├── services/
│   └── login/
│       └── login-service.ts
├── interceptors/
│   └── auth-token/
│       └── auth-token-interceptor.ts
├── guards/
│   └── is-logged-in/
│       └── is-logged-in-guard.ts
└── pages/
    └── login/
        ├── login.ts
        ├── login.html
        └── login.scss
```

## Modèle Utilisateur

### Création du Modèle

```typescript
// models/user.ts
export class User {
    username: string = '';
    firstname: string = '';
    lastname: string = '';
    
    constructor(data?: Partial<User>) {
        Object.assign(this, data);
    }
    
    get fullName(): string {
        return `${this.firstname} ${this.lastname}`.trim();
    }
    
    get initials(): string {
        return `${this.firstname.charAt(0)}${this.lastname.charAt(0)}`.toUpperCase();
    }
}
```

## Backend de Test (Docker)

### Installation

```bash
# Cloner le repository
git clone https://gitlab.com/simpletechprod1/angular-collection-management-backend

# Lancer avec Docker
cd angular-collection-management-backend
sudo docker-compose up
```

### Endpoints Disponibles

L'API sera accessible sur `http://localhost:3000`

Documentation : `http://localhost:3000/api-docs`

**Endpoints principaux** :
- `POST /login` : authentification (retourne un token)
- `POST /logout` : déconnexion
- `GET /me` : informations de l'utilisateur connecté

**Identifiants par défaut** :
- Username: `admin`
- Password: `admin1234`

### Structure de l'API

#### POST /login

**Request** :
```json
{
    "username": "admin",
    "password": "admin1234"
}
```

**Response (200 OK)** :
```json
{
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
}
```

#### GET /me

**Headers** :
```
Authorization: Bearer <token>
```

**Response (200 OK)** :
```json
{
    "username": "admin",
    "firstname": "Admin",
    "lastname": "User"
}
```

#### POST /logout

**Headers** :
```
Authorization: Bearer <token>
```

**Response (200 OK)** : pas de body

## Configuration de HttpClient

### 1. Ajouter le Provider

```typescript
// app.config.ts
import { ApplicationConfig, provideZonelessChangeDetection } from '@angular/core';
import { provideRouter, withComponentInputBinding } from '@angular/router';
import { provideHttpClient } from '@angular/common/http';
import { routes } from './app.routes';

export const appConfig: ApplicationConfig = {
  providers: [
    provideZonelessChangeDetection(),
    provideRouter(routes, withComponentInputBinding()),
    provideHttpClient()  // Important !
  ]
};
```

**Note** : Depuis Angular 21, `provideHttpClient()` est optionnel si vous n'avez pas besoin de configuration spéciale. Mais nous l'ajoutons car nous configurerons des interceptors plus tard.

### 2. Utilisation de HttpClient

```typescript
import { HttpClient } from '@angular/common/http';
import { inject } from '@angular/core';
import { Observable } from 'rxjs';

export class MyService {
    private http = inject(HttpClient);
    
    // GET request
    getData(): Observable<any> {
        return this.http.get('http://localhost:3000/api/data');
    }
    
    // POST request
    postData(data: any): Observable<any> {
        return this.http.post('http://localhost:3000/api/data', data);
    }
    
    // PUT request
    updateData(id: number, data: any): Observable<any> {
        return this.http.put(`http://localhost:3000/api/data/${id}`, data);
    }
    
    // DELETE request
    deleteData(id: number): Observable<any> {
        return this.http.delete(`http://localhost:3000/api/data/${id}`);
    }
}
```

## Service de Login

### Création du Service

```bash
ng g s services/login/login-service
```

### Implémentation Complète

```typescript
// services/login/login-service.ts
import { HttpClient } from '@angular/common/http';
import { inject, Injectable, signal } from '@angular/core';
import { User } from '../../models/user';
import { catchError, map, Observable, of, tap } from 'rxjs';

export interface LoginCredentialsDTO {
    username: string;
    password: string;
}

@Injectable({
  providedIn: 'root',
})
export class LoginService {
    private LK_TOKEN = 'token';
    private BASE_URL = 'http://localhost:3000';
    private http = inject(HttpClient);

    // Signal pour stocker l'utilisateur connecté
    // undefined = non vérifié, null = non connecté, User = connecté
    user = signal<User | undefined | null>(undefined);

    /**
     * Authentifie l'utilisateur et stocke le token
     */
    login(credentials: LoginCredentialsDTO): Observable<void> {
        return this.http.post(this.BASE_URL + '/login/', credentials).pipe(
            tap((result: any) => {
                // Stocker le token dans localStorage
                localStorage.setItem(this.LK_TOKEN, result['token']);
            }),
            map(() => undefined)  // Retourner void
        );
    }

    /**
     * Récupère les informations de l'utilisateur connecté
     */
    getUser(): Observable<User | null | undefined> {
        return this.http.get(this.BASE_URL + '/me/').pipe(
            tap((result: any) => {
                const user = Object.assign(new User(), result);
                this.user.set(user);
            }),
            map(() => this.user()),
            catchError((error) => {
                // En cas d'erreur, l'utilisateur n'est pas connecté
                this.user.set(null);
                return of(null);
            })
        );
    }

    /**
     * Déconnecte l'utilisateur
     */
    logout(): Observable<any> {
        return this.http.post(this.BASE_URL + '/logout/', {}).pipe(
            tap(() => {
                // Supprimer le token et réinitialiser l'utilisateur
                localStorage.removeItem(this.LK_TOKEN);
                this.user.set(null);
            })
        );
    }
    
    /**
     * Vérifie si un utilisateur est connecté
     */
    isLoggedIn(): boolean {
        return this.user() !== null && this.user() !== undefined;
    }
    
    /**
     * Récupère le token stocké
     */
    getToken(): string | null {
        return localStorage.getItem(this.LK_TOKEN);
    }
}
```

**Explications importantes** :

- **Observable vs Promise** : HttpClient retourne des Observables (RxJS)
- **pipe()** : permet d'enchaîner des opérateurs RxJS
- **tap()** : exécute du code sans modifier le résultat
- **map()** : transforme le résultat de l'Observable
- **catchError()** : gère les erreurs

## Page de Login

### Génération du Composant

```bash
ng g c pages/login
```

### Template HTML

```html
<!-- pages/login/login.html -->
<div id="container">
    <form (submit)="login()" [formGroup]="loginFormGroup">
        <h3>Connexion</h3>
        
        <mat-form-field>
            <mat-label>Nom d'utilisateur</mat-label>
            <input matInput formControlName="username" required>
            @if (loginFormGroup.controls.username.hasError('required') && 
                  loginFormGroup.controls.username.touched) {
                <mat-error>Nom d'utilisateur requis</mat-error>
            }
        </mat-form-field>
        
        <mat-form-field>
            <mat-label>Mot de passe</mat-label>
            <input matInput type="password" formControlName="password" required>
            @if (loginFormGroup.controls.password.hasError('required') && 
                  loginFormGroup.controls.password.touched) {
                <mat-error>Mot de passe requis</mat-error>
            }
        </mat-form-field>
        
        <button mat-flat-button color="primary" 
                [disabled]="loginFormGroup.invalid">
            Se connecter
        </button>
        
        @if (invalidCredentials()) {
            <mat-error class="global-error">
                Identifiants invalides
            </mat-error>
        }
    </form>
</div>
```

### Styles SCSS

```scss
// pages/login/login.scss
#container {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 100%;
    height: 100%;
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
}

form {
    background-color: white;
    padding: 2rem;
    border-radius: 12px;
    box-shadow: 0 10px 40px rgba(0, 0, 0, 0.2);
    display: flex;
    flex-direction: column;
    width: 100%;
    max-width: 400px;
    gap: 1rem;
    
    h3 {
        text-align: center;
        margin: 0 0 1rem 0;
        color: #333;
    }
}

mat-form-field {
    width: 100%;
}

.global-error {
    text-align: center;
    padding: 0.5rem;
    background: #ffebee;
    border-radius: 4px;
}
```

### Logique TypeScript

```typescript
// pages/login/login.ts
import { Component, inject, signal } from '@angular/core';
import { FormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';
import { MatButtonModule } from '@angular/material/button';
import { MatInputModule } from '@angular/material/input';
import { LoginCredentialsDTO, LoginService } from '../../services/login/login-service';
import { Router } from '@angular/router';
import { Subscription } from 'rxjs';

@Component({
    selector: 'app-login',
    imports: [ReactiveFormsModule, MatInputModule, MatButtonModule],
    templateUrl: './login.html',
    styleUrl: './login.scss'
})
export class LoginComponent {
    private formBuilder = inject(FormBuilder);
    private loginService = inject(LoginService);
    private router = inject(Router);

    private subscriptions = new Subscription();

    loginFormGroup = this.formBuilder.group({
        'username': ['', [Validators.required]],
        'password': ['', [Validators.required]]
    });
    
    invalidCredentials = signal(false);

    login() {
        // Réinitialiser le message d'erreur
        this.invalidCredentials.set(false);
        
        // Effectuer le login
        const loginSubscription = this.loginService.login(
            this.loginFormGroup.value as LoginCredentialsDTO
        ).subscribe({
            next: () => this.getUserAndRedirect(),
            error: () => this.invalidCredentials.set(true)
        });
        
        this.subscriptions.add(loginSubscription);
    }

    getUserAndRedirect() {
        // Récupérer les infos utilisateur après login réussi
        const getUserSubscription = this.loginService.getUser().subscribe({
            next: () => this.navigateHome(),
            error: () => this.invalidCredentials.set(true)
        });
        
        this.subscriptions.add(getUserSubscription);
    }

    navigateHome() {
        this.router.navigate(['home']);
    }

    ngOnDestroy(): void {
        this.subscriptions.unsubscribe();
    }
}
```

### Route de Login

```typescript
// app.routes.ts
import { LoginComponent } from './pages/login/login';

export const routes: Routes = [
    // ... autres routes
    {
        path: 'login',
        component: LoginComponent
    }
];
```

## Interceptors HTTP

Les interceptors permettent d'intercepter toutes les requêtes HTTP pour les modifier avant envoi.

### Création d'un Interceptor

```bash
ng g interceptor interceptors/auth-token/auth-token
```

### Implémentation

```typescript
// interceptors/auth-token/auth-token-interceptor.ts
import { HttpInterceptorFn } from '@angular/common/http';

export const authTokenInterceptor: HttpInterceptorFn = (req, next) => {
    // Récupérer le token depuis localStorage
    const token = localStorage.getItem('token');
    let requestToSend = req;

    if (token) {
        // Ajouter le header Authorization avec le token
        const headers = req.headers.set('Authorization', 'Bearer ' + token);
        
        // Cloner la requête avec les nouveaux headers
        requestToSend = req.clone({ headers: headers });
    }

    // Passer la requête (modifiée ou non) au prochain interceptor/backend
    return next(requestToSend);
};
```

**Explication** :
1. Un interceptor est une fonction qui reçoit `(req, next)`
2. `req` : la requête HTTP à intercepter
3. `next` : fonction pour passer au prochain interceptor ou faire l'appel HTTP
4. On clone la requête pour la modifier (les requêtes HTTP sont immutables)
5. On ajoute le header `Authorization: Bearer <token>`

### Configuration de l'Interceptor

```typescript
// app.config.ts
import { provideHttpClient, withInterceptors } from '@angular/common/http';
import { authTokenInterceptor } from './interceptors/auth-token/auth-token-interceptor';

export const appConfig: ApplicationConfig = {
  providers: [
    provideZonelessChangeDetection(),
    provideRouter(routes, withComponentInputBinding()),
    provideHttpClient(
        withInterceptors([authTokenInterceptor])  // Enregistrer l'interceptor
    )
  ]
};
```

**Important** : Tous les appels HTTP passeront automatiquement par cet interceptor !

### Interceptor Avancé avec Gestion d'Erreurs

```typescript
import { HttpInterceptorFn, HttpErrorResponse } from '@angular/common/http';
import { inject } from '@angular/core';
import { Router } from '@angular/router';
import { catchError, throwError } from 'rxjs';

export const authTokenInterceptor: HttpInterceptorFn = (req, next) => {
    const router = inject(Router);
    const token = localStorage.getItem('token');
    
    // Ajouter le token si disponible
    if (token) {
        req = req.clone({
            setHeaders: {
                Authorization: `Bearer ${token}`
            }
        });
    }
    
    // Gérer les erreurs d'authentification
    return next(req).pipe(
        catchError((error: HttpErrorResponse) => {
            if (error.status === 401) {
                // Token expiré ou invalide
                localStorage.removeItem('token');
                router.navigate(['/login']);
            }
            return throwError(() => error);
        })
    );
};
```

## Guards de Route

Les guards protègent les routes en vérifiant des conditions avant la navigation.

### Types de Guards

- `CanActivate` : peut-on activer cette route ?
- `CanDeactivate` : peut-on quitter cette route ?
- `CanActivateChild` : peut-on activer les routes enfants ?
- `CanLoad` : peut-on charger un module lazy-loaded ?

### Création d'un Guard

```bash
ng g guard guards/is-logged-in/is-logged-in
? Which type of guard would you like to create? CanActivate
```

### Implémentation

```typescript
// guards/is-logged-in/is-logged-in-guard.ts
import { inject } from '@angular/core';
import { CanActivateFn, Router } from '@angular/router';
import { catchError, map, of } from 'rxjs';
import { LoginService } from '../../services/login/login-service';

export const isLoggedInGuard: CanActivateFn = (route, state) => {
    const loginService = inject(LoginService);
    const router = inject(Router);

    // Cas 1: On ne sait pas encore si l'utilisateur est connecté
    if (loginService.user() === undefined) {
        // Vérifier auprès de l'API
        return loginService.getUser().pipe(
            map(user => {
                // Si on a un utilisateur, autoriser l'accès
                if (user) {
                    return true;
                }
                // Sinon, rediriger vers login
                return router.createUrlTree(['/login']);
            }),
            catchError(() => {
                // En cas d'erreur, rediriger vers login
                return of(router.createUrlTree(['/login']));
            })
        );
    }

    // Cas 2: L'utilisateur n'est pas connecté (null)
    if (loginService.user() === null) {
        return router.createUrlTree(['/login']);
    }

    // Cas 3: L'utilisateur est connecté
    return true;
};
```

**Explication** :
- Le guard retourne `true` pour autoriser l'accès
- Il retourne `false` ou une `UrlTree` pour rediriger
- Il peut retourner un Observable si besoin de vérifier avec l'API

### Application du Guard aux Routes

```typescript
// app.routes.ts
import { isLoggedInGuard } from './guards/is-logged-in/is-logged-in-guard';

export const routes: Routes = [{
    path: '',
    redirectTo: 'home',
    pathMatch: 'full'
}, {
    path: 'home',
    component: CollectionDetail,
    canActivate: [isLoggedInGuard]  // Route protégée
}, {
    path: 'item',
    children: [{
        path: '',
        component: CollectionItemDetail,
        canActivate: [isLoggedInGuard]  // Route protégée
    }, {
        path: ':id',
        component: CollectionItemDetail,
        canActivate: [isLoggedInGuard]  // Route protégée
    }]
}, {
    path: 'login',
    component: LoginComponent  // Pas de guard ici !
}, {
    path: '**',
    component: NotFound
}];
```

## Menu avec Logout

### Template du Menu

```html
<!-- app.html -->
<div id="page-container">
    @let currentUser = user();
    @if (currentUser) {
        <nav>
            <header>
                <span class="avatar">{{ currentUser.initials }}</span>
                {{ currentUser.fullName }}
            </header>
            
            <ul>
                <li class="selected">
                    <div class="dot"></div> 
                    <div class="collection">
                        <div class="name">Ma Collection</div>
                        <div class="items-count">5 items</div>
                    </div>
                </li>
            </ul>
            
            <footer>
                <button mat-flat-button color="warn" (click)="logout()">
                    <mat-icon>logout</mat-icon>
                    Déconnexion
                </button>
            </footer>
        </nav>
    }
    
    <main>
        <router-outlet></router-outlet>
    </main>
</div>
```

### Logique du Composant App

```typescript
// app.ts
import { Component, inject, OnDestroy } from '@angular/core';
import { Router, RouterOutlet } from "@angular/router";
import { MatButton } from "@angular/material/button";
import { MatIcon } from "@angular/material/icon";
import { LoginService } from './services/login/login-service';
import { Subscription } from 'rxjs';

@Component({
  selector: 'app-root',
  templateUrl: './app.html',
  styleUrl: './app.scss',
  imports: [RouterOutlet, MatButton, MatIcon]
})
export class App implements OnDestroy {
    private loginService = inject(LoginService);
    private router = inject(Router);
    
    // Signal de l'utilisateur connecté
    protected user = this.loginService.user;

    private subscriptions = new Subscription();

    logout() {
        const logoutSubscription = this.loginService.logout().subscribe({
            next: () => this.router.navigate(['login']),
            error: () => this.router.navigate(['login']),
        });
        
        this.subscriptions.add(logoutSubscription);
    }

    ngOnDestroy(): void {
        this.subscriptions.unsubscribe();
    }
}
```

### Styles du Menu

```scss
// app.scss
#page-container {
    display: flex;
    width: 100vw;
    height: 100vh;
}

nav {
    display: flex;
    flex-direction: column;
    padding: 1rem;
    background: #f5f5f5;
    min-width: 250px;
    border-right: 1px solid #ddd;
}

nav header {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    font-size: 1.1rem;
    font-weight: bold;
    padding: 1rem;
    border-bottom: 1px solid #ddd;
    margin-bottom: 1rem;
}

.avatar {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    width: 2.5rem;
    height: 2.5rem;
    border-radius: 50%;
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
    font-weight: bold;
}

nav ul {
    flex-grow: 1;
    list-style: none;
    padding: 0;
    margin: 0;
}

nav ul li {
    padding: 1rem;
    border: 2px solid #e0e0e0;
    border-radius: 8px;
    display: flex;
    align-items: center;
    cursor: pointer;
    transition: all 0.2s;
    
    &:hover {
        background: #fafafa;
    }
    
    &.selected {
        border-color: #667eea;
        background: #f0f4ff;
    }
}

.dot {
    width: 1rem;
    height: 1rem;
    background-color: #667eea;
    border-radius: 50%;
    margin-right: 1rem;
}

.collection {
    .name {
        font-weight: bold;
        margin-bottom: 0.25rem;
    }
    
    .items-count {
        font-size: 0.875rem;
        color: #666;
    }
}

nav footer {
    padding-top: 1rem;
    border-top: 1px solid #ddd;
    
    button {
        width: 100%;
    }
}

main {
    flex-grow: 1;
    overflow: auto;
}
```

## Guard Avancé : Redirection après Login

### Sauvegarder l'URL d'origine

```typescript
export const isLoggedInGuard: CanActivateFn = (route, state) => {
    const loginService = inject(LoginService);
    const router = inject(Router);

    if (loginService.user() === undefined) {
        return loginService.getUser().pipe(
            map(user => {
                if (user) {
                    return true;
                }
                // Sauvegarder l'URL demandée
                return router.createUrlTree(['/login'], {
                    queryParams: { returnUrl: state.url }
                });
            }),
            catchError(() => {
                return of(router.createUrlTree(['/login'], {
                    queryParams: { returnUrl: state.url }
                }));
            })
        );
    }

    if (loginService.user() === null) {
        return router.createUrlTree(['/login'], {
            queryParams: { returnUrl: state.url }
        });
    }

    return true;
};
```

### Redirection vers l'URL sauvegardée

```typescript
// login.ts
import { ActivatedRoute } from '@angular/router';

export class LoginComponent {
    private route = inject(ActivatedRoute);
    
    getUserAndRedirect() {
        const getUserSubscription = this.loginService.getUser().subscribe({
            next: () => {
                // Récupérer l'URL de retour ou aller à home par défaut
                const returnUrl = this.route.snapshot.queryParamMap.get('returnUrl') || '/home';
                this.router.navigateByUrl(returnUrl);
            },
            error: () => this.invalidCredentials.set(true)
        });
        
        this.subscriptions.add(getUserSubscription);
    }
}
```

## Sécurité et Bonnes Pratiques

### 1. Ne JAMAIS Stocker de Données Sensibles

```typescript
// ❌ MAUVAIS
localStorage.setItem('password', password);  // JAMAIS !
localStorage.setItem('creditCard', cardNumber);  // JAMAIS !

// ✅ BON
localStorage.setItem('token', jwtToken);  // Token uniquement
```

### 2. Expiration du Token

```typescript
interface TokenPayload {
    exp: number;  // Timestamp d'expiration
    userId: string;
}

function isTokenExpired(token: string): boolean {
    try {
        const payload = JSON.parse(atob(token.split('.')[1])) as TokenPayload;
        return Date.now() >= payload.exp * 1000;
    } catch {
        return true;
    }
}
```

### 3. HTTPS en Production

```typescript
// environment.production.ts
export const environment = {
    production: true,
    apiUrl: 'https://api.monapp.com'  // HTTPS obligatoire !
};
```

### 4. CORS Configuration

Le backend doit autoriser les requêtes cross-origin :

```javascript
// Backend Node.js exemple
app.use(cors({
    origin: 'http://localhost:4200',
    credentials: true
}));
```

### 5. Protection CSRF

Pour les cookies, utiliser des tokens CSRF :

```typescript
import { provideHttpClient, withXsrfConfiguration } from '@angular/common/http';

provideHttpClient(
    withXsrfConfiguration({
        cookieName: 'XSRF-TOKEN',
        headerName: 'X-XSRF-TOKEN'
    })
)
```

## Questions d'Entretien Fréquentes

**Q1 : Quelle est la différence entre localStorage et sessionStorage ?**
- `localStorage` : persiste même après fermeture du navigateur
- `sessionStorage` : effacé à la fermeture de l'onglet
Pour l'authentification, `localStorage` est généralement préféré.

**Q2 : Comment fonctionne un interceptor HTTP ?**
C'est une fonction qui intercepte les requêtes/réponses HTTP pour les modifier. Utilise le pattern chain of responsibility.

**Q3 : Que retourne un Guard pour bloquer la navigation ?**
- `false` : bloque la navigation
- `UrlTree` : redirige vers une autre route
- `Observable<boolean | UrlTree>` : décision asynchrone

**Q4 : Pourquoi cloner la requête HTTP dans un interceptor ?**
Les objets HttpRequest sont immutables. On doit les cloner pour les modifier.

**Q5 : Comment gérer un token expiré ?**
L'interceptor détecte une erreur 401, supprime le token, et redirige vers /login.

**Q6 : Peut-on avoir plusieurs interceptors ?**
Oui, ils s'exécutent dans l'ordre de déclaration dans `withInterceptors([...])`.

**Q7 : Quelle est la différence entre `tap()` et `map()` en RxJS ?**
- `tap()` : exécute du code sans modifier le résultat
- `map()` : transforme le résultat de l'Observable

**Q8 : Comment tester un service avec HttpClient ?**
Utiliser `HttpClientTestingModule` et `HttpTestingController` pour mocker les requêtes.

## Résumé

L'authentification dans Angular repose sur :
- ✅ **Service** : gère login, logout, et état utilisateur
- ✅ **Interceptor** : ajoute automatiquement le token aux requêtes
- ✅ **Guard** : protège les routes nécessitant authentification
- ✅ **Signals** : état réactif de l'utilisateur connecté
- ✅ **localStorage** : stockage du token JWT

**Points clés à retenir** :
- Configurer `provideHttpClient()` avec les interceptors
- Utiliser `signal()` pour l'état de l'utilisateur
- Implémenter un guard `CanActivate` pour les routes protégées
- Toujours faire `unsubscribe()` dans `ngOnDestroy()`
- Ne jamais stocker de mots de passe en clair
- Utiliser HTTPS en production

Dans le prochain chapitre, nous verrons comment utiliser HttpClient pour créer un CRUD complet avec une API REST, en intégrant tout ce que nous avons appris sur l'authentification.
