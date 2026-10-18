## [**..**](./00_index.md)

# Chapitre 6 : Gestion des Erreurs

## 📑 Table des matières
- [6.1 Gestion d'Erreurs](#61-gestion-derreurs)
- [6.2 Opérateurs de Finalisation](#62-opérateurs-de-finalisation)
- [6.3 Patterns de Gestion d'Erreurs](#63-patterns-de-gestion-derreurs)
- [6.4 Exercice Pratique](#64-exercice-pratique)
- [Questions/Réponses d'Entretien](#-questionsréponses-dentretien)

---

## 6.1 Gestion d'Erreurs

### 🎯 catchError() - Intercepter les Erreurs

**`catchError()`** intercepte les erreurs et permet de retourner un Observable de remplacement (fallback).

```typescript
import { of, throwError } from 'rxjs';
import { catchError, map } from 'rxjs/operators';

const source$ = throwError(() => new Error('Oops!'));

source$.pipe(
  catchError(error => {
    console.error('Erreur capturée:', error.message);
    return of('Valeur par défaut');
  })
).subscribe({
  next: value => console.log('Reçu:', value),
  error: err => console.error('Erreur non gérée:', err)
});

// Output:
// Erreur capturée: Oops!
// Reçu: Valeur par défaut
```

**Diagramme de marbre :**
```
source:     --1--2--X (erreur)
catchError: --1--2--fallback|
                    └─ Erreur capturée, Observable de remplacement
```

#### Cas d'usage pratiques

```typescript
// Requête HTTP avec fallback
@Injectable()
export class UserService {
  constructor(private http: HttpClient) {}
  
  getUsers(): Observable<User[]> {
    return this.http.get<User[]>('/api/users').pipe(
      catchError(error => {
        console.error('Erreur chargement users:', error);
        return of([]); // Retourne un tableau vide en cas d'erreur
      })
    );
  }
  
  // Avec valeur par défaut depuis le cache
  getUserById(id: string): Observable<User> {
    return this.http.get<User>(`/api/users/${id}`).pipe(
      catchError(error => {
        console.error('Erreur:', error);
        // Essayer de récupérer depuis le cache
        const cachedUser = this.cache.get(id);
        if (cachedUser) {
          return of(cachedUser);
        }
        // Si pas de cache, retourner un utilisateur par défaut
        return of({ id, name: 'Utilisateur inconnu', email: '' });
      })
    );
  }
}
```

#### CatchError avec throwError

```typescript
// Transformer l'erreur avant de la relancer
this.http.get('/api/data').pipe(
  catchError(error => {
    // Logger l'erreur
    console.error('Erreur API:', error);
    
    // Transformer et relancer
    const customError = new Error(
      `Impossible de charger les données: ${error.message}`
    );
    return throwError(() => customError);
  })
).subscribe({
  error: err => console.error('Erreur finale:', err.message)
});
```

### 🔄 retry() - Réessayer Automatiquement

**`retry()`** réessaye automatiquement un Observable en cas d'erreur.

```typescript
import { throwError, timer } from 'rxjs';
import { retry, mergeMap } from 'rxjs/operators';

let attempt = 0;

const unreliable$ = timer(1000).pipe(
  mergeMap(() => {
    attempt++;
    console.log(`Tentative ${attempt}`);
    
    if (attempt < 3) {
      return throwError(() => new Error('Échec'));
    }
    return of('Succès!');
  })
);

unreliable$.pipe(
  retry(2) // Réessayer 2 fois (3 tentatives au total)
).subscribe({
  next: value => console.log('Résultat:', value),
  error: err => console.error('Échec final:', err.message)
});

// Output:
// Tentative 1
// Tentative 2
// Tentative 3
// Résultat: Succès!
```

**Diagramme de marbre :**
```
source:  --X (erreur)
retry(2): --X--X--value|
          1ère 2ème succès
```

#### Cas d'usage pratiques

```typescript
// Requête HTTP avec retry
@Injectable()
export class DataService {
  constructor(private http: HttpClient) {}
  
  loadData(): Observable<any> {
    return this.http.get('/api/data').pipe(
      retry(3), // Réessayer 3 fois
      catchError(error => {
        console.error('Échec après 3 tentatives:', error);
        return of(null);
      })
    );
  }
  
  // Retry seulement pour certaines erreurs
  loadCriticalData(): Observable<any> {
    return this.http.get('/api/critical').pipe(
      retry({
        count: 3,
        delay: 1000 // Attendre 1s entre chaque retry
      }),
      catchError(error => {
        // Envoyer une notification d'erreur critique
        this.notificationService.error('Erreur critique');
        return throwError(() => error);
      })
    );
  }
}
```

### ⏱️ retryWhen() - Réessayer avec Logique Personnalisée

**`retryWhen()`** permet de contrôler précisément quand et comment réessayer.

```typescript
import { throwError, timer } from 'rxjs';
import { retryWhen, mergeMap, tap } from 'rxjs/operators';

const source$ = throwError(() => new Error('Erreur'));

source$.pipe(
  retryWhen(errors => 
    errors.pipe(
      tap(err => console.log('Erreur détectée:', err.message)),
      mergeMap((err, index) => {
        // Réessayer 3 fois maximum
        if (index >= 3) {
          return throwError(() => new Error('Trop de tentatives'));
        }
        
        // Attendre 1s, 2s, 3s (backoff linéaire)
        const delay = (index + 1) * 1000;
        console.log(`Réessai dans ${delay}ms...`);
        return timer(delay);
      })
    )
  )
).subscribe({
  next: value => console.log('Succès:', value),
  error: err => console.error('Échec final:', err.message)
});
```

#### Backoff Exponentiel

```typescript
import { retryWhen, mergeMap, tap, throwError } from 'rxjs/operators';

function retryWithBackoff(maxRetries = 3, initialDelay = 1000) {
  return retryWhen(errors =>
    errors.pipe(
      mergeMap((error, index) => {
        const retryAttempt = index + 1;
        
        // Arrêter après maxRetries
        if (retryAttempt > maxRetries) {
          return throwError(() => error);
        }
        
        // Backoff exponentiel: 1s, 2s, 4s, 8s...
        const delay = initialDelay * Math.pow(2, index);
        
        console.log(
          `Tentative ${retryAttempt}/${maxRetries}. ` +
          `Réessai dans ${delay}ms`
        );
        
        return timer(delay);
      })
    )
  );
}

// Utilisation
this.http.get('/api/data').pipe(
  retryWithBackoff(3, 1000),
  catchError(error => {
    console.error('Toutes les tentatives ont échoué');
    return of(null);
  })
).subscribe();
```

### 🛡️ Stratégies de Récupération

#### 1. Fallback Value (Valeur par Défaut)

```typescript
function getUserWithFallback(id: string): Observable<User> {
  return this.http.get<User>(`/api/users/${id}`).pipe(
    catchError(() => of({
      id,
      name: 'Utilisateur par défaut',
      email: 'default@example.com'
    }))
  );
}
```

#### 2. Fallback to Cache (Repli sur le Cache)

```typescript
function getDataWithCache(key: string): Observable<any> {
  return this.http.get(`/api/data/${key}`).pipe(
    tap(data => this.cache.set(key, data)),
    catchError(() => {
      const cached = this.cache.get(key);
      if (cached) {
        console.log('Utilisation du cache');
        return of(cached);
      }
      return throwError(() => new Error('Pas de données disponibles'));
    })
  );
}
```

#### 3. Alternative Source (Source Alternative)

```typescript
function getDataWithAlternative(): Observable<any> {
  return this.http.get('/api/primary').pipe(
    catchError(error => {
      console.warn('Source primaire échouée, tentative source secondaire');
      return this.http.get('/api/secondary');
    }),
    catchError(error => {
      console.warn('Source secondaire échouée, tentative source tertiaire');
      return this.http.get('/api/tertiary');
    }),
    catchError(error => {
      console.error('Toutes les sources ont échoué');
      return of([]);
    })
  );
}
```

#### 4. Retry avec Condition

```typescript
function retryOnSpecificError() {
  return retryWhen(errors =>
    errors.pipe(
      mergeMap((error, index) => {
        // Réessayer seulement pour les erreurs réseau
        if (error.status === 0 || error.status >= 500) {
          if (index < 3) {
            const delay = 1000 * (index + 1);
            console.log(`Erreur réseau, réessai dans ${delay}ms`);
            return timer(delay);
          }
        }
        
        // Ne pas réessayer pour les autres erreurs
        return throwError(() => error);
      })
    )
  );
}

// Utilisation
this.http.get('/api/data').pipe(
  retryOnSpecificError(),
  catchError(error => {
    if (error.status === 404) {
      return of(null);
    }
    return throwError(() => error);
  })
).subscribe();
```

---

## 6.2 Opérateurs de Finalisation

### 🏁 finalize() - Cleanup Garanti

**`finalize()`** exécute une fonction de nettoyage **toujours**, que l'Observable se complète, émette une erreur, ou soit annulé.

```typescript
import { of, throwError } from 'rxjs';
import { finalize, delay } from 'rxjs/operators';

// Cas 1: Complétion normale
of(1, 2, 3).pipe(
  finalize(() => console.log('Nettoyage après complétion'))
).subscribe({
  next: v => console.log(v),
  complete: () => console.log('Complété')
});

// Output:
// 1
// 2
// 3
// Complété
// Nettoyage après complétion

// Cas 2: Erreur
throwError(() => new Error('Oops')).pipe(
  finalize(() => console.log('Nettoyage après erreur'))
).subscribe({
  error: err => console.error('Erreur:', err.message)
});

// Output:
// Erreur: Oops
// Nettoyage après erreur

// Cas 3: Unsubscribe
const subscription = interval(1000).pipe(
  finalize(() => console.log('Nettoyage après unsubscribe'))
).subscribe(console.log);

setTimeout(() => subscription.unsubscribe(), 3500);

// Output:
// 0
// 1
// 2
// Nettoyage après unsubscribe
```

#### Cas d'usage pratiques

```typescript
// Loading indicator
@Component({
  selector: 'app-data-loader',
  template: `
    <div *ngIf="isLoading">Chargement...</div>
    <div *ngIf="data">{{ data | json }}</div>
  `
})
export class DataLoaderComponent {
  isLoading = false;
  data: any;
  
  loadData() {
    this.isLoading = true;
    
    this.http.get('/api/data').pipe(
      finalize(() => {
        // Toujours exécuté, même en cas d'erreur
        this.isLoading = false;
        console.log('Requête terminée');
      })
    ).subscribe({
      next: data => this.data = data,
      error: err => console.error('Erreur:', err)
    });
  }
}

// Cleanup de ressources
function processFile(file: File): Observable<string> {
  const reader = new FileReader();
  
  return new Observable(subscriber => {
    reader.onload = () => {
      subscriber.next(reader.result as string);
      subscriber.complete();
    };
    
    reader.onerror = () => {
      subscriber.error(new Error('Erreur de lecture'));
    };
    
    reader.readAsText(file);
  }).pipe(
    finalize(() => {
      // Cleanup garanti
      console.log('Nettoyage du FileReader');
      reader.abort();
    })
  );
}
```

### 👁️ tap() - Effets de Bord

**`tap()`** (anciennement `do`) permet d'exécuter des effets de bord sans modifier le flux.

```typescript
import { of } from 'rxjs';
import { tap, map } from 'rxjs/operators';

of(1, 2, 3).pipe(
  tap(value => console.log('Avant map:', value)),
  map(x => x * 10),
  tap(value => console.log('Après map:', value))
).subscribe(value => console.log('Résultat:', value));

// Output:
// Avant map: 1
// Après map: 10
// Résultat: 10
// Avant map: 2
// Après map: 20
// Résultat: 20
// Avant map: 3
// Après map: 30
// Résultat: 30
```

#### Tap avec Observer Complet

```typescript
this.http.get('/api/users').pipe(
  tap({
    next: value => console.log('Données reçues:', value),
    error: err => console.error('Erreur:', err),
    complete: () => console.log('Requête terminée')
  })
).subscribe();
```

#### Cas d'usage pratiques

```typescript
// Logging
@Injectable()
export class UserService {
  constructor(private http: HttpClient) {}
  
  getUsers(): Observable<User[]> {
    return this.http.get<User[]>('/api/users').pipe(
      tap(users => console.log(`${users.length} utilisateurs chargés`)),
      tap(users => this.analytics.track('users_loaded', { count: users.length }))
    );
  }
}

// Mise à jour du cache
getDataWithCache(id: string): Observable<Data> {
  return this.http.get<Data>(`/api/data/${id}`).pipe(
    tap(data => this.cache.set(id, data)),
    tap(data => console.log('Cache mis à jour:', id))
  );
}

// Debugging
source$.pipe(
  tap(value => console.log('🔵 Avant transformation:', value)),
  map(x => x * 2),
  tap(value => console.log('🟢 Après transformation:', value)),
  filter(x => x > 10),
  tap(value => console.log('🟡 Après filtre:', value))
).subscribe();
```

### 📝 Logging et Debugging

#### Opérateur de Debug Personnalisé

```typescript
function debug(tag: string) {
  return tap({
    next: value => console.log(`[${tag}] Next:`, value),
    error: err => console.error(`[${tag}] Error:`, err),
    complete: () => console.log(`[${tag}] Complete`)
  });
}

// Utilisation
source$.pipe(
  debug('Source'),
  map(x => x * 2),
  debug('Après map'),
  filter(x => x > 10),
  debug('Après filter')
).subscribe();
```

#### Log avec Timestamp

```typescript
function logWithTime(tag: string) {
  return tap(value => {
    const time = new Date().toISOString();
    console.log(`[${time}] ${tag}:`, value);
  });
}

this.http.get('/api/data').pipe(
  logWithTime('Requête envoyée'),
  delay(1000),
  logWithTime('Réponse reçue')
).subscribe();
```

#### Performance Monitoring

```typescript
function measureTime(label: string) {
  let startTime: number;
  
  return tap({
    subscribe: () => {
      startTime = Date.now();
      console.log(`⏱️ [${label}] Début`);
    },
    finalize: () => {
      const duration = Date.now() - startTime;
      console.log(`⏱️ [${label}] Durée: ${duration}ms`);
    }
  });
}

this.http.get('/api/large-data').pipe(
  measureTime('Chargement données')
).subscribe();
```

---

## 6.3 Patterns de Gestion d'Erreurs

### 🎯 Pattern 1: Fallback Values

```typescript
@Injectable()
export class ConfigService {
  private defaultConfig: Config = {
    theme: 'light',
    language: 'fr',
    notifications: true
  };
  
  constructor(private http: HttpClient) {}
  
  loadConfig(): Observable<Config> {
    return this.http.get<Config>('/api/config').pipe(
      catchError(error => {
        console.warn('Config serveur indisponible, utilisation config par défaut');
        return of(this.defaultConfig);
      })
    );
  }
}
```

### 🔄 Pattern 2: Retry avec Backoff Exponentiel

```typescript
interface RetryConfig {
  maxRetries: number;
  initialDelay: number;
  maxDelay: number;
  backoffFactor: number;
}

function retryWithExponentialBackoff(config: RetryConfig) {
  const { maxRetries, initialDelay, maxDelay, backoffFactor } = config;
  
  return retryWhen(errors =>
    errors.pipe(
      mergeMap((error, index) => {
        const retryAttempt = index + 1;
        
        if (retryAttempt > maxRetries) {
          console.error(`Échec après ${maxRetries} tentatives`);
          return throwError(() => error);
        }
        
        // Calcul du délai avec backoff exponentiel
        const delay = Math.min(
          initialDelay * Math.pow(backoffFactor, index),
          maxDelay
        );
        
        console.log(
          `⚠️ Tentative ${retryAttempt}/${maxRetries} ` +
          `dans ${delay}ms`
        );
        
        return timer(delay);
      }),
      tap({
        error: err => console.error('Erreur finale:', err)
      })
    )
  );
}

// Utilisation
@Injectable()
export class ApiService {
  constructor(private http: HttpClient) {}
  
  getCriticalData(): Observable<any> {
    return this.http.get('/api/critical').pipe(
      retryWithExponentialBackoff({
        maxRetries: 5,
        initialDelay: 1000,
        maxDelay: 30000,
        backoffFactor: 2
      }),
      catchError(error => {
        this.notificationService.error('Impossible de charger les données');
        return of(null);
      })
    );
  }
}
```

### 🛡️ Pattern 3: Error Boundaries

```typescript
// Service de gestion d'erreurs centralisé
@Injectable({ providedIn: 'root' })
export class ErrorHandlerService {
  constructor(
    private notification: NotificationService,
    private logger: LoggerService
  ) {}
  
  handleError(error: any, context?: string): Observable<never> {
    // Logger l'erreur
    this.logger.error(error, context);
    
    // Déterminer le type d'erreur
    let userMessage = 'Une erreur est survenue';
    
    if (error.status === 0) {
      userMessage = 'Pas de connexion internet';
    } else if (error.status === 401) {
      userMessage = 'Session expirée';
      // Rediriger vers login
    } else if (error.status === 403) {
      userMessage = 'Accès refusé';
    } else if (error.status === 404) {
      userMessage = 'Ressource non trouvée';
    } else if (error.status >= 500) {
      userMessage = 'Erreur serveur';
    }
    
    // Notifier l'utilisateur
    this.notification.error(userMessage);
    
    // Relancer l'erreur
    return throwError(() => error);
  }
}

// Utilisation dans les services
@Injectable()
export class UserService {
  constructor(
    private http: HttpClient,
    private errorHandler: ErrorHandlerService
  ) {}
  
  getUser(id: string): Observable<User> {
    return this.http.get<User>(`/api/users/${id}`).pipe(
      catchError(error => 
        this.errorHandler.handleError(error, 'UserService.getUser')
      )
    );
  }
}
```

### 🔔 Pattern 4: User Notification + Error Recovery

```typescript
@Injectable()
export class DataService {
  private retryAttempts = 0;
  private maxRetries = 3;
  
  constructor(
    private http: HttpClient,
    private notification: NotificationService
  ) {}
  
  loadData(): Observable<Data[]> {
    return this.http.get<Data[]>('/api/data').pipe(
      tap(() => {
        this.retryAttempts = 0; // Reset on success
      }),
      retryWhen(errors =>
        errors.pipe(
          mergeMap((error, index) => {
            this.retryAttempts = index + 1;
            
            if (this.retryAttempts >= this.maxRetries) {
              this.notification.error(
                'Impossible de charger les données après plusieurs tentatives'
              );
              return throwError(() => error);
            }
            
            this.notification.warning(
              `Tentative ${this.retryAttempts}/${this.maxRetries}...`
            );
            
            return timer(1000 * this.retryAttempts);
          })
        )
      ),
      catchError(error => {
        // Fallback vers données en cache ou vides
        const cachedData = this.getCachedData();
        if (cachedData) {
          this.notification.info('Affichage des données en cache');
          return of(cachedData);
        }
        return of([]);
      }),
      finalize(() => {
        console.log('Chargement terminé');
      })
    );
  }
  
  private getCachedData(): Data[] | null {
    // Récupération depuis le cache
    return null;
  }
}
```

### 📊 Pattern 5: Erreurs Spécifiques par Code HTTP

```typescript
interface ApiError {
  code: string;
  message: string;
  details?: any;
}

@Injectable()
export class HttpErrorHandler {
  constructor(private router: Router) {}
  
  handleHttpError<T>(operation = 'operation', fallback?: T) {
    return (error: HttpErrorResponse): Observable<T> => {
      console.error(`${operation} a échoué:`, error);
      
      // Gestion par code HTTP
      switch (error.status) {
        case 400:
          return this.handleBadRequest(error, fallback);
        case 401:
          return this.handleUnauthorized(error);
        case 403:
          return this.handleForbidden(error);
        case 404:
          return this.handleNotFound(error, fallback);
        case 500:
        case 502:
        case 503:
          return this.handleServerError(error, fallback);
        default:
          return this.handleUnknownError(error, fallback);
      }
    };
  }
  
  private handleBadRequest<T>(error: HttpErrorResponse, fallback?: T): Observable<T> {
    const apiError = error.error as ApiError;
    console.error('Requête invalide:', apiError.message);
    
    if (fallback !== undefined) {
      return of(fallback);
    }
    return throwError(() => error);
  }
  
  private handleUnauthorized<T>(error: HttpErrorResponse): Observable<T> {
    console.warn('Non authentifié, redirection vers login');
    this.router.navigate(['/login']);
    return throwError(() => error);
  }
  
  private handleForbidden<T>(error: HttpErrorResponse): Observable<T> {
    console.error('Accès refusé');
    this.router.navigate(['/forbidden']);
    return throwError(() => error);
  }
  
  private handleNotFound<T>(error: HttpErrorResponse, fallback?: T): Observable<T> {
    console.warn('Ressource non trouvée');
    
    if (fallback !== undefined) {
      return of(fallback);
    }
    return throwError(() => error);
  }
  
  private handleServerError<T>(error: HttpErrorResponse, fallback?: T): Observable<T> {
    console.error('Erreur serveur');
    
    if (fallback !== undefined) {
      return of(fallback);
    }
    return throwError(() => error);
  }
  
  private handleUnknownError<T>(error: HttpErrorResponse, fallback?: T): Observable<T> {
    console.error('Erreur inconnue:', error);
    
    if (fallback !== undefined) {
      return of(fallback);
    }
    return throwError(() => error);
  }
}

// Utilisation
@Injectable()
export class ProductService {
  constructor(
    private http: HttpClient,
    private errorHandler: HttpErrorHandler
  ) {}
  
  getProduct(id: string): Observable<Product> {
    return this.http.get<Product>(`/api/products/${id}`).pipe(
      catchError(
        this.errorHandler.handleHttpError<Product>('getProduct', null)
      )
    );
  }
}
```

---

## 6.4 Exercice Pratique

### 🎯 Challenge : API Résiliente avec Gestion d'Erreurs Complète

Créez un service API qui :
1. Réessaye automatiquement en cas d'erreur réseau (backoff exponentiel)
2. Utilise le cache en cas d'échec
3. Notifie l'utilisateur des erreurs
4. Gère spécifiquement chaque code d'erreur HTTP
5. Log toutes les erreurs pour le monitoring
6. Affiche un indicateur de chargement

### 💡 Solution

```typescript
// models/api.model.ts
export interface CacheEntry<T> {
  data: T;
  timestamp: number;
  expiresIn: number;
}

export interface ApiResponse<T> {
  data: T;
  fromCache: boolean;
}

// services/cache.service.ts
@Injectable({ providedIn: 'root' })
export class CacheService {
  private cache = new Map<string, CacheEntry<any>>();
  private defaultTTL = 5 * 60 * 1000; // 5 minutes
  
  set<T>(key: string, data: T, ttl: number = this.defaultTTL): void {
    this.cache.set(key, {
      data,
      timestamp: Date.now(),
      expiresIn: ttl
    });
  }
  
  get<T>(key: string): T | null {
    const entry = this.cache.get(key);
    
    if (!entry) {
      return null;
    }
    
    // Vérifier si le cache est expiré
    const now = Date.now();
    if (now - entry.timestamp > entry.expiresIn) {
      this.cache.delete(key);
      return null;
    }
    
    return entry.data as T;
  }
  
  has(key: string): boolean {
    return this.get(key) !== null;
  }
  
  clear(): void {
    this.cache.clear();
  }
  
  delete(key: string): void {
    this.cache.delete(key);
  }
}

// services/logger.service.ts
@Injectable({ providedIn: 'root' })
export class LoggerService {
  error(error: any, context?: string): void {
    const timestamp = new Date().toISOString();
    console.error(`[${timestamp}] ERROR ${context || ''}:`, error);
    
    // Envoyer au service de monitoring (Sentry, LogRocket, etc.)
    // this.monitoring.captureException(error);
  }
  
  warn(message: string, context?: string): void {
    const timestamp = new Date().toISOString();
    console.warn(`[${timestamp}] WARN ${context || ''}:`, message);
  }
  
  info(message: string, context?: string): void {
    const timestamp = new Date().toISOString();
    console.log(`[${timestamp}] INFO ${context || ''}:`, message);
  }
}

// services/resilient-api.service.ts
@Injectable({ providedIn: 'root' })
export class ResilientApiService {
  private loadingSubject = new BehaviorSubject<boolean>(false);
  loading$ = this.loadingSubject.asObservable();
  
  constructor(
    private http: HttpClient,
    private cache: CacheService,
    private logger: LoggerService,
    private notification: NotificationService
  ) {}
  
  get<T>(url: string, options?: {
    useCache?: boolean;
    cacheTTL?: number;
    maxRetries?: number;
    showNotifications?: boolean;
  }): Observable<ApiResponse<T>> {
    const {
      useCache = true,
      cacheTTL = 5 * 60 * 1000,
      maxRetries = 3,
      showNotifications = true
    } = options || {};
    
    const cacheKey = `GET:${url}`;
    
    // Vérifier le cache d'abord
    if (useCache && this.cache.has(cacheKey)) {
      this.logger.info(`Cache hit pour ${url}`);
      return of({
        data: this.cache.get<T>(cacheKey)!,
        fromCache: true
      });
    }
    
    // Requête HTTP avec gestion d'erreurs complète
    this.loadingSubject.next(true);
    
    return this.http.get<T>(url).pipe(
      // Sauvegarder en cache
      tap(data => {
        if (useCache) {
          this.cache.set(cacheKey, data, cacheTTL);
          this.logger.info(`Données mises en cache: ${url}`);
        }
      }),
      
      // Retry avec backoff exponentiel
      retryWhen(errors =>
        errors.pipe(
          mergeMap((error, index) => {
            const retryAttempt = index + 1;
            
            // Ne réessayer que pour les erreurs réseau
            if (!this.shouldRetry(error) || retryAttempt > maxRetries) {
              return throwError(() => error);
            }
            
            const delay = this.calculateBackoff(retryAttempt);
            
            if (showNotifications) {
              this.notification.warning(
                `Connexion instable. Tentative ${retryAttempt}/${maxRetries}...`
              );
            }
            
            this.logger.warn(
              `Retry ${retryAttempt}/${maxRetries} dans ${delay}ms`,
              url
            );
            
            return timer(delay);
          })
        )
      ),
      
      // Gestion des erreurs
      catchError(error => {
        this.logger.error(error, `GET ${url}`);
        
        // Essayer le cache en dernier recours
        if (useCache) {
          const cachedData = this.cache.get<T>(cacheKey);
          if (cachedData) {
            if (showNotifications) {
              this.notification.info('Affichage des données en cache');
            }
            return of({
              data: cachedData,
              fromCache: true
            });
          }
        }
        
        // Gérer l'erreur selon le code HTTP
        return this.handleHttpError<T>(error, url, showNotifications);
      }),
      
      // Transformer en ApiResponse
      map(data => {
        if ('fromCache' in data) {
          return data as ApiResponse<T>;
        }
        return {
          data: data as T,
          fromCache: false
        };
      }),
      
      // Cleanup garanti
      finalize(() => {
        this.loadingSubject.next(false);
        this.logger.info(`Requête terminée: ${url}`);
      })
    );
  }
  
  private shouldRetry(error: HttpErrorResponse): boolean {
    // Réessayer pour les erreurs réseau et serveur
    return error.status === 0 || error.status >= 500;
  }
  
  private calculateBackoff(attempt: number): number {
    // Backoff exponentiel: 1s, 2s, 4s, 8s, max 30s
    return Math.min(1000 * Math.pow(2, attempt - 1), 30000);
  }
  
  private handleHttpError<T>(
    error: HttpErrorResponse,
    url: string,
    showNotifications: boolean
  ): Observable<ApiResponse<T>> {
    let message = 'Une erreur est survenue';
    
    switch (error.status) {
      case 0:
        message = 'Pas de connexion internet';
        break;
      case 400:
        message = 'Requête invalide';
        break;
      case 401:
        message = 'Session expirée';
        // Rediriger vers login
        break;
      case 403:
        message = 'Accès refusé';
        break;
      case 404:
        message = 'Ressource non trouvée';
        break;
      case 500:
      case 502:
      case 503:
        message = 'Erreur serveur';
        break;
    }
    
    if (showNotifications) {
      this.notification.error(message);
    }
    
    return throwError(() => error);
  }
}

// Component d'utilisation
@Component({
  selector: 'app-product-list',
  template: `
    <div class="container">
      <h1>Produits</h1>
      
      <div class="loading-bar" *ngIf="loading$ | async">
        Chargement en cours...
      </div>
      
      <button (click)="refresh()">🔄 Rafraîchir</button>
      
      <div *ngIf="products$ | async as response" class="products">
        <div *ngIf="response.fromCache" class="cache-notice">
          📦 Données du cache
        </div>
        
        <div class="product-card" *ngFor="let product of response.data">
          <h3>{{ product.name }}</h3>
          <p>{{ product.price | currency:'EUR' }}</p>
        </div>
      </div>
      
      <div *ngIf="error$ | async" class="error">
        ❌ Impossible de charger les produits
      </div>
    </div>
  `,
  styles: [`
    .container {
      padding: 20px;
      max-width: 1200px;
      margin: 0 auto;
    }
    
    .loading-bar {
      background: #2196f3;
      color: white;
      padding: 10px;
      border-radius: 4px;
      margin: 10px 0;
      text-align: center;
    }
    
    .cache-notice {
      background: #fff3cd;
      padding: 10px;
      border-radius: 4px;
      margin: 10px 0;
      border-left: 4px solid #ffc107;
    }
    
    .products {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(250px, 1fr));
      gap: 20px;
      margin-top: 20px;
    }
    
    .product-card {
      border: 1px solid #ddd;
      padding: 20px;
      border-radius: 8px;
      background: white;
    }
    
    .error {
      background: #f44336;
      color: white;
      padding: 15px;
      border-radius: 4px;
      margin: 20px 0;
    }
    
    button {
      padding: 10px 20px;
      background: #2196f3;
      color: white;
      border: none;
      border-radius: 4px;
      cursor: pointer;
      margin: 10px 0;
    }
    
    button:hover {
      background: #1976d2;
    }
  `]
})
export class ProductListComponent implements OnInit {
  private refreshTrigger$ = new Subject<void>();
  
  loading$ = this.api.loading$;
  
  products$ = this.refreshTrigger$.pipe(
    startWith(undefined),
    switchMap(() =>
      this.api.get<Product[]>('/api/products', {
        useCache: true,
        cacheTTL: 5 * 60 * 1000,
        maxRetries: 3,
        showNotifications: true
      })
    ),
    shareReplay(1)
  );
  
  error$ = this.products$.pipe(
    map(() => false),
    catchError(() => of(true))
  );
  
  constructor(private api: ResilientApiService) {}
  
  ngOnInit() {}
  
  refresh() {
    this.refreshTrigger$.next();
  }
}
```

---

## 📝 Questions/Réponses d'Entretien

### Q1 : Quelle est la différence entre `catchError()` dans pipe vs dans subscribe ?

**Réponse :**
> **`catchError()` dans pipe** :
> - Intercepte l'erreur et **retourne un nouvel Observable**
> - Permet la récupération et la continuation du flux
> - L'erreur ne remonte pas au subscribe
> 
> **Gestion dans subscribe** :
> - Dernière ligne de défense
> - L'Observable est déjà en erreur
> - Ne peut pas retourner de valeur de remplacement
> 
> ```typescript
> // catchError dans pipe : récupération possible
> source$.pipe(
>   catchError(err => of('Valeur par défaut'))
> ).subscribe({
>   next: v => console.log(v), // Reçoit 'Valeur par défaut'
>   error: e => console.error(e) // Jamais appelé
> });
> 
> // Gestion dans subscribe : fin du flux
> source$.subscribe({
>   next: v => console.log(v),
>   error: e => console.error(e) // Appelé, flux terminé
> });
> ```

### Q2 : Comment fonctionne `retry()` ?

**Réponse :**
> `retry(n)` **resouscrit** à l'Observable source jusqu'à `n` fois en cas d'erreur.
> 
> **Comportement** :
> - À chaque erreur, l'Observable source est relancé depuis le début
> - Compte le nombre de **nouvelles tentatives** (pas le nombre total)
> - Si toutes les tentatives échouent, l'erreur est propagée
> 
> ```typescript
> // retry(2) = 3 tentatives au total (1 initial + 2 retry)
> source$.pipe(
>   retry(2)
> ).subscribe({
>   next: v => console.log(v),
>   error: e => console.error('Échec après 3 tentatives')
> });
> 
> // Avec configuration
> source$.pipe(
>   retry({
>     count: 3,
>     delay: 1000 // Attendre 1s entre chaque retry
>   })
> );
> ```

### Q3 : Quand utiliser `finalize()` vs `complete` callback ?

**Réponse :**
> **`finalize()`** :
> - S'exécute **toujours** : complétion, erreur, ou unsubscribe
> - Garanti le nettoyage des ressources
> - Équivalent du `finally` en JavaScript
> 
> **`complete` callback** :
> - S'exécute **seulement** si l'Observable se complète normalement
> - **Ne s'exécute pas** en cas d'erreur ou unsubscribe
> 
> ```typescript
> // finalize : toujours exécuté
> source$.pipe(
>   finalize(() => console.log('Toujours exécuté'))
> ).subscribe({
>   complete: () => console.log('Seulement si complétion normale')
> });
> 
> // Cas d'usage : loading indicator
> this.loading = true;
> this.api.getData().pipe(
>   finalize(() => this.loading = false) // ✅ Toujours reset
> ).subscribe();
> ```

### Q4 : Qu'est-ce que le backoff exponentiel et pourquoi l'utiliser ?

**Réponse :**
> Le **backoff exponentiel** augmente progressivement le délai entre les tentatives : 1s, 2s, 4s, 8s, etc.
> 
> **Avantages** :
> - Évite de surcharger un serveur en difficulté
> - Laisse le temps au service de récupérer
> - Réduit la charge réseau
> - Meilleure expérience utilisateur (moins de tentatives inutiles)
> 
> **Implémentation** :
> ```typescript
> retryWhen(errors =>
>   errors.pipe(
>     mergeMap((error, index) => {
>       const attempt = index + 1;
>       
>       if (attempt > 5) {
>         return throwError(() => error);
>       }
>       
>       // Formule : initialDelay * 2^(attempt-1)
>       // 1s, 2s, 4s, 8s, 16s
>       const delay = 1000 * Math.pow(2, index);
>       
>       // Limiter à 30s max
>       const cappedDelay = Math.min(delay, 30000);
>       
>       return timer(cappedDelay);
>     })
>   )
> );
> ```

### Q5 : Comment gérer les erreurs spécifiques par code HTTP ?

**Réponse :**
> Utilisez `catchError()` avec un switch sur `error.status` pour gérer chaque cas.
> 
> ```typescript
> this.http.get('/api/data').pipe(
>   catchError((error: HttpErrorResponse) => {
>     switch (error.status) {
>       case 401:
>         // Session expirée
>         this.router.navigate(['/login']);
>         return throwError(() => error);
>       
>       case 403:
>         // Accès refusé
>         this.notification.error('Accès refusé');
>         return of(null);
>       
>       case 404:
>         // Resource non trouvée
>         return of(defaultValue);
>       
>       case 500:
>       case 502:
>       case 503:
>         // Erreur serveur : retry
>         this.notification.warning('Erreur serveur, nouvelle tentative...');
>         return throwError(() => error);
>       
>       default:
>         // Erreur inconnue
>         this.notification.error('Une erreur est survenue');
>         return throwError(() => error);
>     }
>   }),
>   retry(3) // Retry pour erreurs serveur
> );
> ```

### Q6 : Quelle est la différence entre `tap()` et `map()` ?

**Réponse :**
> **`tap()`** (effets de bord) :
> - **Ne modifie pas** le flux
> - Utilisé pour les effets de bord (logging, cache, analytics)
> - Retourne la valeur originale
> 
> **`map()`** (transformation) :
> - **Transforme** les valeurs du flux
> - Retourne une nouvelle valeur
> - Change le type de l'Observable
> 
> ```typescript
> of(1, 2, 3).pipe(
>   tap(x => console.log('Avant:', x)),     // Logging
>   map(x => x * 10),                        // Transform: 10, 20, 30
>   tap(x => console.log('Après:', x))      // Logging
> ).subscribe();
> 
> // ❌ ERREUR: tap ne doit pas transformer
> tap(x => x * 10) // La valeur reste inchangée !
> 
> // ✅ BON: tap pour effets de bord
> tap(x => this.cache.set('key', x))
> ```

### Q7 : Comment implémenter un fallback vers le cache ?

**Réponse :**
> Utilisez `catchError()` pour retourner les données du cache en cas d'erreur.
> 
> ```typescript
> @Injectable()
> export class DataService {
>   constructor(
>     private http: HttpClient,
>     private cache: CacheService
>   ) {}
>   
>   getData(id: string): Observable<Data> {
>     const cacheKey = `data_${id}`;
>     
>     return this.http.get<Data>(`/api/data/${id}`).pipe(
>       // Sauvegarder en cache en cas de succès
>       tap(data => this.cache.set(cacheKey, data)),
>       
>       // Fallback vers cache en cas d'erreur
>       catchError(error => {
>         console.warn('API échec, tentative cache');
>         
>         const cached = this.cache.get<Data>(cacheKey);
>         
>         if (cached) {
>           console.log('Données du cache utilisées');
>           return of(cached);
>         }
>         
>         // Pas de cache, propager l'erreur
>         return throwError(() => error);
>       })
>     );
>   }
> }
> ```

### Q8 : Pourquoi éviter les try/catch dans les Observables ?

**Réponse :**
> Les **try/catch** ne capturent **pas les erreurs asynchrones** dans les Observables.
> 
> **❌ Mauvais** :
> ```typescript
> try {
>   this.http.get('/api/data').subscribe(data => {
>     console.log(data);
>   });
> } catch (error) {
>   // ❌ Ne capturera JAMAIS les erreurs HTTP
>   console.error(error);
> }
> ```
> 
> **✅ Bon** :
> ```typescript
> // Méthode 1: error callback
> this.http.get('/api/data').subscribe({
>   next: data => console.log(data),
>   error: err => console.error(err)
> });
> 
> // Méthode 2: catchError
> this.http.get('/api/data').pipe(
>   catchError(err => {
>     console.error(err);
>     return of(null);
>   })
> ).subscribe();
> ```

### Q9 : Comment logger les erreurs pour le monitoring ?

**Réponse :**
> Créez un service de logging centralisé et utilisez `tap()` + `catchError()`.
> 
> ```typescript
> @Injectable({ providedIn: 'root' })
> export class LoggerService {
>   error(error: any, context?: string): void {
>     const errorInfo = {
>       message: error.message,
>       stack: error.stack,
>       context,
>       timestamp: new Date().toISOString(),
>       url: window.location.href,
>       userAgent: navigator.userAgent
>     };
>     
>     console.error('ERROR:', errorInfo);
>     
>     // Envoyer au service de monitoring
>     // Sentry.captureException(error);
>     // LogRocket.captureException(error);
>   }
> }
> 
> // Utilisation dans un service
> @Injectable()
> export class ApiService {
>   constructor(
>     private http: HttpClient,
>     private logger: LoggerService
>   ) {}
>   
>   getData(): Observable<Data> {
>     return this.http.get<Data>('/api/data').pipe(
>       tap(data => this.logger.info('Données chargées', 'ApiService')),
>       catchError(error => {
>         this.logger.error(error, 'ApiService.getData');
>         return throwError(() => error);
>       })
>     );
>   }
> }
> ```

### Q10 : Comment tester la gestion d'erreurs avec RxJS ?

**Réponse :**
> Utilisez `throwError()` et les matchers Jasmine/Jest.
> 
> ```typescript
> describe('DataService', () => {
>   let service: DataService;
>   let httpMock: HttpTestingController;
>   
>   beforeEach(() => {
>     TestBed.configureTestingModule({
>       imports: [HttpClientTestingModule]
>     });
>     service = TestBed.inject(DataService);
>     httpMock = TestBed.inject(HttpTestingController);
>   });
>   
>   it('devrait gérer les erreurs HTTP', () => {
>     const errorMessage = 'Erreur 404';
>     
>     service.getData().subscribe({
>       next: () => fail('Devrait avoir échoué'),
>       error: (error) => {
>         expect(error.status).toBe(404);
>         expect(error.statusText).toBe('Not Found');
>       }
>     });
>     
>     const req = httpMock.expectOne('/api/data');
>     req.flush(errorMessage, { status: 404, statusText: 'Not Found' });
>   });
>   
>   it('devrait retourner une valeur par défaut en cas d\'erreur', () => {
>     service.getDataWithFallback().subscribe(data => {
>       expect(data).toEqual([]);
>     });
>     
>     const req = httpMock.expectOne('/api/data');
>     req.error(new ErrorEvent('Network error'));
>   });
> });
> ```

---

## 🎯 Points Clés du Chapitre 6

### Gestion d'Erreurs
✅ **catchError()** : Intercepte et récupère des erreurs
✅ **retry()** : Réessaye automatiquement N fois
✅ **retryWhen()** : Retry avec logique personnalisée
✅ **throwError()** : Créer une erreur Observable

### Finalisation
✅ **finalize()** : Cleanup garanti (toujours exécuté)
✅ **tap()** : Effets de bord sans modifier le flux
✅ **Différence tap vs map** : Effets vs transformation

### Patterns Essentiels
✅ **Fallback values** : Valeur par défaut en cas d'erreur
✅ **Cache fallback** : Repli sur cache si API échoue
✅ **Backoff exponentiel** : Délai croissant entre retries
✅ **Error boundaries** : Gestion centralisée des erreurs
✅ **HTTP error codes** : Traitement spécifique par code

### Best Practices
✅ **Toujours gérer les erreurs** dans catchError ou subscribe
✅ **Logger les erreurs** pour le monitoring
✅ **Notifier l'utilisateur** des erreurs importantes
✅ **Utiliser finalize()** pour le cleanup
✅ **Éviter try/catch** pour les Observables
✅ **Tester la gestion d'erreurs** systématiquement

---