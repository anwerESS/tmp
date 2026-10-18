# Chapitre 13 : API REST et HttpClient

## Introduction

Ce chapitre couvre l'intégration complète d'une API REST dans une application Angular. Nous verrons comment créer un service CRUD complet (Create, Read, Update, Delete) pour gérer une collection d'items via HttpClient.

## Architecture REST

### Qu'est-ce qu'une API REST ?

REST (Representational State Transfer) est un style d'architecture pour les API web qui utilise :
- Les verbes HTTP (GET, POST, PUT, DELETE)
- Des URLs pour identifier les ressources
- JSON pour échanger des données
- Des codes de statut HTTP pour indiquer le résultat

### Convention REST Standard

| Opération | Méthode HTTP | URL | Body | Retour |
|-----------|-------------|-----|------|--------|
| Lister | GET | `/api/items` | - | `Item[]` |
| Obtenir | GET | `/api/items/:id` | - | `Item` |
| Créer | POST | `/api/items` | `Item` | `Item` |
| Mettre à jour | PUT | `/api/items/:id` | `Item` | `Item` |
| Supprimer | DELETE | `/api/items/:id` | - | - |

### Codes de Statut HTTP

**Success (2xx)** :
- `200 OK` : requête réussie
- `201 Created` : ressource créée
- `204 No Content` : succès sans contenu de réponse

**Client Errors (4xx)** :
- `400 Bad Request` : données invalides
- `401 Unauthorized` : non authentifié
- `403 Forbidden` : non autorisé
- `404 Not Found` : ressource introuvable
- `422 Unprocessable Entity` : validation échouée

**Server Errors (5xx)** :
- `500 Internal Server Error` : erreur serveur
- `503 Service Unavailable` : service indisponible

## Configuration du Backend

### Backend de Test (Docker)

Le backend utilisé dans ce chapitre est disponible sur :
```bash
git clone https://gitlab.com/simpletechprod1/angular-collection-management-backend
cd angular-collection-management-backend
sudo docker-compose up
```

**URL de l'API** : `http://localhost:3000`  
**Documentation** : `http://localhost:3000/api-docs`

### Endpoints Disponibles

```
POST   /login                    # Authentification
POST   /logout                   # Déconnexion
GET    /me                       # Info utilisateur

GET    /collections              # Liste des collections
GET    /collections/:id          # Détail d'une collection
POST   /collections              # Créer une collection
PUT    /collections/:id          # Modifier une collection
DELETE /collections/:id          # Supprimer une collection

GET    /items                    # Liste des items
GET    /items/:id                # Détail d'un item
POST   /items                    # Créer un item
PUT    /items/:id                # Modifier un item
DELETE /items/:id                # Supprimer un item
```

## Service CRUD Complet

### Structure du Service

```typescript
// services/collection/collection-service.ts
import { HttpClient, HttpErrorResponse } from '@angular/common/http';
import { inject, Injectable, signal } from '@angular/core';
import { Observable, throwError, BehaviorSubject } from 'rxjs';
import { catchError, tap, map } from 'rxjs/operators';
import { CollectionItem } from '../../models/collection-item';

@Injectable({
  providedIn: 'root',
})
export class CollectionService {
    private BASE_URL = 'http://localhost:3000';
    private http = inject(HttpClient);
    
    // Signal pour stocker les items en cache
    private itemsSignal = signal<CollectionItem[]>([]);
    
    // Exposer les items en lecture seule
    items = this.itemsSignal.asReadonly();
    
    // BehaviorSubject pour le chargement
    loading$ = new BehaviorSubject<boolean>(false);
    error$ = new BehaviorSubject<string | null>(null);

    /**
     * Récupère tous les items
     */
    getItems(): Observable<CollectionItem[]> {
        this.loading$.next(true);
        this.error$.next(null);
        
        return this.http.get<CollectionItem[]>(this.BASE_URL + '/items').pipe(
            tap(items => {
                // Convertir en instances de CollectionItem
                const itemsInstances = items.map(item => 
                    Object.assign(new CollectionItem(), item)
                );
                this.itemsSignal.set(itemsInstances);
                this.loading$.next(false);
            }),
            catchError(error => this.handleError(error))
        );
    }

    /**
     * Récupère un item par son ID
     */
    getItemById(id: number): Observable<CollectionItem> {
        return this.http.get<CollectionItem>(this.BASE_URL + `/items/${id}`).pipe(
            map(item => Object.assign(new CollectionItem(), item)),
            catchError(error => this.handleError(error))
        );
    }

    /**
     * Crée un nouvel item
     */
    createItem(item: CollectionItem): Observable<CollectionItem> {
        this.loading$.next(true);
        this.error$.next(null);
        
        return this.http.post<CollectionItem>(this.BASE_URL + '/items', item).pipe(
            tap(newItem => {
                // Ajouter le nouvel item à la liste
                const current = this.itemsSignal();
                this.itemsSignal.set([...current, Object.assign(new CollectionItem(), newItem)]);
                this.loading$.next(false);
            }),
            catchError(error => this.handleError(error))
        );
    }

    /**
     * Met à jour un item existant
     */
    updateItem(id: number, item: CollectionItem): Observable<CollectionItem> {
        this.loading$.next(true);
        this.error$.next(null);
        
        return this.http.put<CollectionItem>(this.BASE_URL + `/items/${id}`, item).pipe(
            tap(updatedItem => {
                // Mettre à jour dans la liste
                const current = this.itemsSignal();
                const index = current.findIndex(i => i.id === id);
                if (index !== -1) {
                    const newItems = [...current];
                    newItems[index] = Object.assign(new CollectionItem(), updatedItem);
                    this.itemsSignal.set(newItems);
                }
                this.loading$.next(false);
            }),
            catchError(error => this.handleError(error))
        );
    }

    /**
     * Supprime un item
     */
    deleteItem(id: number): Observable<void> {
        this.loading$.next(true);
        this.error$.next(null);
        
        return this.http.delete<void>(this.BASE_URL + `/items/${id}`).pipe(
            tap(() => {
                // Retirer l'item de la liste
                const current = this.itemsSignal();
                this.itemsSignal.set(current.filter(i => i.id !== id));
                this.loading$.next(false);
            }),
            catchError(error => this.handleError(error))
        );
    }

    /**
     * Gestion centralisée des erreurs
     */
    private handleError(error: HttpErrorResponse): Observable<never> {
        this.loading$.next(false);
        
        let errorMessage = 'Une erreur est survenue';
        
        if (error.error instanceof ErrorEvent) {
            // Erreur côté client
            errorMessage = `Erreur: ${error.error.message}`;
        } else {
            // Erreur côté serveur
            switch (error.status) {
                case 400:
                    errorMessage = 'Requête invalide';
                    break;
                case 401:
                    errorMessage = 'Non authentifié';
                    break;
                case 403:
                    errorMessage = 'Accès interdit';
                    break;
                case 404:
                    errorMessage = 'Ressource non trouvée';
                    break;
                case 422:
                    errorMessage = 'Données de validation incorrectes';
                    break;
                case 500:
                    errorMessage = 'Erreur serveur';
                    break;
                default:
                    errorMessage = `Erreur ${error.status}: ${error.message}`;
            }
        }
        
        this.error$.next(errorMessage);
        console.error('Erreur HTTP:', error);
        return throwError(() => new Error(errorMessage));
    }
    
    /**
     * Réinitialise le cache
     */
    clearCache(): void {
        this.itemsSignal.set([]);
        this.error$.next(null);
    }
}
```

## Composant Liste (READ)

### Template

```html
<!-- pages/collection-list/collection-list.html -->
<div class="collection-list-container">
    <header>
        <h2>Ma Collection</h2>
        <button mat-fab color="primary" (click)="createNewItem()">
            <mat-icon>add</mat-icon>
        </button>
    </header>
    
    <!-- Loading spinner -->
    @if (loading$ | async) {
        <div class="loading-container">
            <mat-spinner></mat-spinner>
        </div>
    }
    
    <!-- Erreur -->
    @if (error$ | async; as error) {
        <mat-card class="error-card">
            <mat-card-content>
                <mat-icon color="warn">error</mat-icon>
                <p>{{ error }}</p>
                <button mat-button (click)="reload()">Réessayer</button>
            </mat-card-content>
        </mat-card>
    }
    
    <!-- Liste des items -->
    @if (!(loading$ | async) && !(error$ | async)) {
        <div class="items-grid">
            @for (item of items(); track item.id) {
                <mat-card class="item-card">
                    @if (item.imageBase64) {
                        <img mat-card-image [src]="item.imageBase64" 
                             [alt]="item.name">
                    }
                    
                    <mat-card-header>
                        <mat-card-title>{{ item.name }}</mat-card-title>
                        <mat-card-subtitle>
                            {{ item.category }} • {{ item.year }}
                        </mat-card-subtitle>
                    </mat-card-header>
                    
                    <mat-card-content>
                        <p class="description">{{ item.description }}</p>
                        <div class="condition">
                            État: <span class="badge">{{ getConditionLabel(item.condition) }}</span>
                        </div>
                    </mat-card-content>
                    
                    <mat-card-actions>
                        <button mat-button (click)="editItem(item)">
                            <mat-icon>edit</mat-icon>
                            Modifier
                        </button>
                        <button mat-button color="warn" (click)="deleteItem(item)">
                            <mat-icon>delete</mat-icon>
                            Supprimer
                        </button>
                    </mat-card-actions>
                </mat-card>
            } @empty {
                <div class="empty-state">
                    <mat-icon>inventory_2</mat-icon>
                    <h3>Aucun item</h3>
                    <p>Commencez par ajouter votre premier item de collection</p>
                    <button mat-raised-button color="primary" (click)="createNewItem()">
                        Ajouter un item
                    </button>
                </div>
            }
        </div>
    }
</div>
```

### TypeScript

```typescript
// pages/collection-list/collection-list.ts
import { Component, inject, OnInit, OnDestroy } from '@angular/core';
import { Router } from '@angular/router';
import { MatCardModule } from '@angular/material/card';
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatDialog } from '@angular/material/dialog';
import { CommonModule } from '@angular/common';
import { Subscription } from 'rxjs';
import { CollectionService } from '../../services/collection/collection-service';
import { CollectionItem } from '../../models/collection-item';
import { ConfirmDialogComponent } from '../../components/confirm-dialog/confirm-dialog';

@Component({
    selector: 'app-collection-list',
    imports: [
        CommonModule,
        MatCardModule,
        MatButtonModule,
        MatIconModule,
        MatProgressSpinnerModule
    ],
    templateUrl: './collection-list.html',
    styleUrl: './collection-list.scss'
})
export class CollectionListComponent implements OnInit, OnDestroy {
    private collectionService = inject(CollectionService);
    private router = inject(Router);
    private dialog = inject(MatDialog);
    
    private subscriptions = new Subscription();
    
    // Accès au signal des items
    items = this.collectionService.items;
    
    // Observables pour le loading et les erreurs
    loading$ = this.collectionService.loading$;
    error$ = this.collectionService.error$;
    
    conditionLabels = {
        'mint': 'Neuf',
        'excellent': 'Excellent',
        'good': 'Bon',
        'fair': 'Correct',
        'poor': 'Mauvais'
    };
    
    ngOnInit() {
        this.loadItems();
    }
    
    loadItems() {
        const sub = this.collectionService.getItems().subscribe({
            error: (error) => console.error('Erreur chargement:', error)
        });
        this.subscriptions.add(sub);
    }
    
    reload() {
        this.loadItems();
    }
    
    createNewItem() {
        this.router.navigate(['/item']);
    }
    
    editItem(item: CollectionItem) {
        this.router.navigate(['/item', item.id]);
    }
    
    deleteItem(item: CollectionItem) {
        const dialogRef = this.dialog.open(ConfirmDialogComponent, {
            width: '400px',
            data: {
                title: 'Confirmer la suppression',
                message: `Voulez-vous vraiment supprimer "${item.name}" ?`,
                confirmText: 'Supprimer',
                cancelText: 'Annuler'
            }
        });
        
        dialogRef.afterClosed().subscribe(confirmed => {
            if (confirmed && item.id) {
                const sub = this.collectionService.deleteItem(item.id).subscribe({
                    next: () => console.log('Item supprimé'),
                    error: (error) => console.error('Erreur suppression:', error)
                });
                this.subscriptions.add(sub);
            }
        });
    }
    
    getConditionLabel(condition: string): string {
        return this.conditionLabels[condition as keyof typeof this.conditionLabels] || condition;
    }
    
    ngOnDestroy() {
        this.subscriptions.unsubscribe();
    }
}
```

### Styles

```scss
// collection-list.scss
.collection-list-container {
    padding: 2rem;
    max-width: 1400px;
    margin: 0 auto;
}

header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 2rem;
    
    h2 {
        margin: 0;
        font-size: 2rem;
    }
}

.loading-container {
    display: flex;
    justify-content: center;
    padding: 4rem;
}

.error-card {
    mat-card-content {
        display: flex;
        flex-direction: column;
        align-items: center;
        gap: 1rem;
        padding: 2rem;
        
        mat-icon {
            font-size: 48px;
            width: 48px;
            height: 48px;
        }
    }
}

.items-grid {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
    gap: 1.5rem;
}

.item-card {
    display: flex;
    flex-direction: column;
    transition: transform 0.2s, box-shadow 0.2s;
    
    &:hover {
        transform: translateY(-4px);
        box-shadow: 0 8px 16px rgba(0,0,0,0.1);
    }
    
    img {
        width: 100%;
        height: 200px;
        object-fit: cover;
    }
    
    .description {
        display: -webkit-box;
        -webkit-line-clamp: 3;
        -webkit-box-orient: vertical;
        overflow: hidden;
        margin: 0.5rem 0;
    }
    
    .condition {
        margin-top: 0.5rem;
        
        .badge {
            display: inline-block;
            padding: 0.25rem 0.75rem;
            background: #e3f2fd;
            color: #1976d2;
            border-radius: 12px;
            font-size: 0.875rem;
            font-weight: 500;
        }
    }
    
    mat-card-actions {
        margin-top: auto;
        padding: 0.5rem;
    }
}

.empty-state {
    grid-column: 1 / -1;
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 1rem;
    padding: 4rem 2rem;
    text-align: center;
    
    mat-icon {
        font-size: 64px;
        width: 64px;
        height: 64px;
        color: #999;
    }
    
    h3 {
        margin: 0;
        color: #666;
    }
    
    p {
        color: #999;
    }
}
```

## Composant Formulaire (CREATE / UPDATE)

### TypeScript

```typescript
// pages/collection-item-form/collection-item-form.ts
import { Component, inject, input, OnInit, OnDestroy } from '@angular/core';
import { FormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';
import { Router } from '@angular/router';
import { MatButtonModule } from '@angular/material/button';
import { MatInputModule } from '@angular/material/input';
import { MatSelectModule } from '@angular/material/select';
import { MatSnackBar } from '@angular/material/snack-bar';
import { Subscription, switchMap, of } from 'rxjs';
import { CollectionService } from '../../services/collection/collection-service';
import { CollectionItem } from '../../models/collection-item';

@Component({
    selector: 'app-collection-item-form',
    imports: [
        ReactiveFormsModule,
        MatButtonModule,
        MatInputModule,
        MatSelectModule
    ],
    templateUrl: './collection-item-form.html',
    styleUrl: './collection-item-form.scss'
})
export class CollectionItemFormComponent implements OnInit, OnDestroy {
    private formBuilder = inject(FormBuilder);
    private collectionService = inject(CollectionService);
    private router = inject(Router);
    private snackBar = inject(MatSnackBar);
    
    id = input<string>();
    item: CollectionItem | null = null;
    imagePreview: string | null = null;
    isEditMode = false;
    
    private subscriptions = new Subscription();
    
    itemFormGroup = this.formBuilder.group({
        name: ['', [Validators.required, Validators.minLength(3)]],
        description: ['', Validators.maxLength(500)],
        year: [null as number | null, [Validators.min(1800), Validators.max(2024)]],
        category: ['', Validators.required],
        condition: ['good', Validators.required]
    });
    
    categories = ['Timbres', 'Pièces', 'Cartes', 'Figurines', 'Autre'];
    conditions = [
        { value: 'mint', label: 'Neuf' },
        { value: 'excellent', label: 'Excellent' },
        { value: 'good', label: 'Bon' },
        { value: 'fair', label: 'Correct' },
        { value: 'poor', label: 'Mauvais' }
    ];
    
    ngOnInit() {
        const itemId = this.id();
        
        if (itemId) {
            // Mode édition
            this.isEditMode = true;
            const sub = this.collectionService.getItemById(parseInt(itemId)).subscribe({
                next: (item) => {
                    this.item = item;
                    this.itemFormGroup.patchValue({
                        name: item.name,
                        description: item.description,
                        year: item.year,
                        category: item.category,
                        condition: item.condition
                    });
                    this.imagePreview = item.imageBase64;
                },
                error: (error) => {
                    this.snackBar.open('Erreur de chargement', 'Fermer', { duration: 3000 });
                    this.router.navigate(['/home']);
                }
            });
            this.subscriptions.add(sub);
        } else {
            // Mode création
            this.item = new CollectionItem();
        }
    }
    
    onFileSelected(event: Event) {
        const input = event.target as HTMLInputElement;
        const file = input.files?.[0];
        
        if (file) {
            const reader = new FileReader();
            reader.onload = () => {
                const base64 = reader.result as string;
                this.imagePreview = base64;
                if (this.item) {
                    this.item.imageBase64 = base64;
                }
            };
            reader.readAsDataURL(file);
        }
    }
    
    removeImage() {
        this.imagePreview = null;
        if (this.item) {
            this.item.imageBase64 = '';
        }
    }
    
    save() {
        if (this.itemFormGroup.invalid) {
            this.itemFormGroup.markAllAsTouched();
            return;
        }
        
        if (!this.item) return;
        
        // Mettre à jour l'item avec les valeurs du formulaire
        Object.assign(this.item, this.itemFormGroup.value);
        
        // Créer ou mettre à jour
        const request$ = this.isEditMode && this.item.id
            ? this.collectionService.updateItem(this.item.id, this.item)
            : this.collectionService.createItem(this.item);
        
        const sub = request$.subscribe({
            next: () => {
                this.snackBar.open(
                    this.isEditMode ? 'Item modifié' : 'Item créé',
                    'Fermer',
                    { duration: 3000 }
                );
                this.router.navigate(['/home']);
            },
            error: (error) => {
                this.snackBar.open(
                    `Erreur: ${error.message}`,
                    'Fermer',
                    { duration: 5000 }
                );
            }
        });
        
        this.subscriptions.add(sub);
    }
    
    cancel() {
        this.router.navigate(['/home']);
    }
    
    ngOnDestroy() {
        this.subscriptions.unsubscribe();
    }
}
```

## Composant de Confirmation

### Dialog Component

```typescript
// components/confirm-dialog/confirm-dialog.ts
import { Component, inject } from '@angular/core';
import { MAT_DIALOG_DATA, MatDialogRef, MatDialogModule } from '@angular/material/dialog';
import { MatButtonModule } from '@angular/material/button';

export interface ConfirmDialogData {
    title: string;
    message: string;
    confirmText?: string;
    cancelText?: string;
}

@Component({
    selector: 'app-confirm-dialog',
    imports: [MatDialogModule, MatButtonModule],
    template: `
        <h2 mat-dialog-title>{{ data.title }}</h2>
        <mat-dialog-content>
            <p>{{ data.message }}</p>
        </mat-dialog-content>
        <mat-dialog-actions align="end">
            <button mat-button (click)="onCancel()">
                {{ data.cancelText || 'Annuler' }}
            </button>
            <button mat-flat-button color="warn" (click)="onConfirm()">
                {{ data.confirmText || 'Confirmer' }}
            </button>
        </mat-dialog-actions>
    `,
    styles: [`
        mat-dialog-content {
            min-width: 300px;
        }
    `]
})
export class ConfirmDialogComponent {
    data = inject<ConfirmDialogData>(MAT_DIALOG_DATA);
    dialogRef = inject(MatDialogRef<ConfirmDialogComponent>);
    
    onCancel() {
        this.dialogRef.close(false);
    }
    
    onConfirm() {
        this.dialogRef.close(true);
    }
}
```

## Gestion Avancée des Erreurs

### Service d'Erreurs Centralisé

```typescript
// services/error/error-handler-service.ts
import { Injectable, inject } from '@angular/core';
import { MatSnackBar } from '@angular/material/snack-bar';
import { HttpErrorResponse } from '@angular/common/http';

@Injectable({
    providedIn: 'root'
})
export class ErrorHandlerService {
    private snackBar = inject(MatSnackBar);
    
    handleError(error: HttpErrorResponse): void {
        let message = 'Une erreur est survenue';
        
        if (error.error instanceof ErrorEvent) {
            // Erreur côté client
            message = error.error.message;
        } else {
            // Erreur côté serveur
            message = this.getServerErrorMessage(error);
        }
        
        this.showError(message);
    }
    
    private getServerErrorMessage(error: HttpErrorResponse): string {
        if (error.error?.message) {
            return error.error.message;
        }
        
        switch (error.status) {
            case 400:
                return 'Requête invalide';
            case 401:
                return 'Vous devez vous reconnecter';
            case 403:
                return 'Accès refusé';
            case 404:
                return 'Ressource non trouvée';
            case 422:
                return 'Données invalides';
            case 500:
                return 'Erreur serveur';
            case 503:
                return 'Service temporairement indisponible';
            default:
                return `Erreur ${error.status}`;
        }
    }
    
    showError(message: string): void {
        this.snackBar.open(message, 'Fermer', {
            duration: 5000,
            panelClass: ['error-snackbar']
        });
    }
    
    showSuccess(message: string): void {
        this.snackBar.open(message, 'Fermer', {
            duration: 3000,
            panelClass: ['success-snackbar']
        });
    }
}
```

## Optimisations et Bonnes Pratiques

### 1. Pagination

```typescript
interface PaginatedResponse<T> {
    data: T[];
    total: number;
    page: number;
    pageSize: number;
}

getItemsPaginated(page: number = 1, pageSize: number = 10): Observable<PaginatedResponse<CollectionItem>> {
    const params = { page: page.toString(), pageSize: pageSize.toString() };
    return this.http.get<PaginatedResponse<CollectionItem>>(
        `${this.BASE_URL}/items`,
        { params }
    );
}
```

### 2. Recherche et Filtres

```typescript
searchItems(searchTerm: string, filters?: any): Observable<CollectionItem[]> {
    let params: any = { search: searchTerm };
    
    if (filters) {
        params = { ...params, ...filters };
    }
    
    return this.http.get<CollectionItem[]>(`${this.BASE_URL}/items/search`, { params });
}
```

### 3. Cache avec Timestamp

```typescript
private cache = new Map<string, { data: any, timestamp: number }>();
private CACHE_DURATION = 5 * 60 * 1000; // 5 minutes

getItemsWithCache(): Observable<CollectionItem[]> {
    const cacheKey = 'items';
    const cached = this.cache.get(cacheKey);
    
    if (cached && Date.now() - cached.timestamp < this.CACHE_DURATION) {
        return of(cached.data);
    }
    
    return this.getItems().pipe(
        tap(items => {
            this.cache.set(cacheKey, { data: items, timestamp: Date.now() });
        })
    );
}
```

### 4. Retry sur Échec

```typescript
import { retry, delay } from 'rxjs/operators';

getItemsWithRetry(): Observable<CollectionItem[]> {
    return this.http.get<CollectionItem[]>(`${this.BASE_URL}/items`).pipe(
        retry({
            count: 3,
            delay: 1000  // Attendre 1s entre chaque retry
        }),
        catchError(error => this.handleError(error))
    );
}
```

### 5. Upload de Fichiers Volumineux

```typescript
uploadImage(file: File): Observable<string> {
    const formData = new FormData();
    formData.append('image', file);
    
    return this.http.post<{ url: string }>(`${this.BASE_URL}/upload`, formData).pipe(
        map(response => response.url)
    );
}

// Avec progression
uploadImageWithProgress(file: File): Observable<number | string> {
    const formData = new FormData();
    formData.append('image', file);
    
    return this.http.post(`${this.BASE_URL}/upload`, formData, {
        reportProgress: true,
        observe: 'events'
    }).pipe(
        map(event => {
            if (event.type === HttpEventType.UploadProgress) {
                return Math.round(100 * event.loaded / (event.total || 1));
            } else if (event.type === HttpEventType.Response) {
                return event.body.url;
            }
            return 0;
        })
    );
}
```

## Environnements

### Configuration par Environnement

```typescript
// environments/environment.development.ts
export const environment = {
    production: false,
    apiUrl: 'http://localhost:3000',
    apiTimeout: 30000
};

// environments/environment.production.ts
export const environment = {
    production: true,
    apiUrl: 'https://api.monapp.com',
    apiTimeout: 10000
};
```

### Utilisation dans le Service

```typescript
import { environment } from '../../../environments/environment';

export class CollectionService {
    private BASE_URL = environment.apiUrl;
}
```

## Tests Unitaires

### Test d'un Service

```typescript
// collection-service.spec.ts
import { TestBed } from '@angular/core/testing';
import { HttpClientTestingModule, HttpTestingController } from '@angular/common/http/testing';
import { CollectionService } from './collection-service';
import { CollectionItem } from '../../models/collection-item';

describe('CollectionService', () => {
    let service: CollectionService;
    let httpMock: HttpTestingController;
    
    beforeEach(() => {
        TestBed.configureTestingModule({
            imports: [HttpClientTestingModule],
            providers: [CollectionService]
        });
        
        service = TestBed.inject(CollectionService);
        httpMock = TestBed.inject(HttpTestingController);
    });
    
    afterEach(() => {
        httpMock.verify();  // Vérifier qu'aucune requête n'est en attente
    });
    
    it('should get items', () => {
        const mockItems: CollectionItem[] = [
            { id: 1, name: 'Test Item 1', category: 'Timbres' },
            { id: 2, name: 'Test Item 2', category: 'Pièces' }
        ];
        
        service.getItems().subscribe(items => {
            expect(items.length).toBe(2);
            expect(items[0].name).toBe('Test Item 1');
        });
        
        const req = httpMock.expectOne('http://localhost:3000/items');
        expect(req.request.method).toBe('GET');
        req.flush(mockItems);
    });
    
    it('should create an item', () => {
        const newItem: CollectionItem = {
            name: 'New Item',
            category: 'Cartes',
            condition: 'good'
        };
        
        service.createItem(newItem).subscribe(item => {
            expect(item.id).toBe(3);
            expect(item.name).toBe('New Item');
        });
        
        const req = httpMock.expectOne('http://localhost:3000/items');
        expect(req.request.method).toBe('POST');
        expect(req.request.body).toEqual(newItem);
        req.flush({ ...newItem, id: 3 });
    });
    
    it('should handle errors', () => {
        service.getItems().subscribe({
            next: () => fail('should have failed'),
            error: (error) => {
                expect(error.message).toContain('Erreur');
            }
        });
        
        const req = httpMock.expectOne('http://localhost:3000/items');
        req.flush('Error', { status: 500, statusText: 'Server Error' });
    });
});
```

## Questions d'Entretien Fréquentes

**Q1 : Quelle est la différence entre Observable et Promise ?**
- Observable : flux de données multiples, annulable, opérateurs RxJS
- Promise : valeur unique, non annulable, .then()/.catch()

**Q2 : Qu'est-ce que le cold vs hot Observable ?**
- Cold : exécute à chaque subscribe (HttpClient)
- Hot : partage entre subscribers (Subject, BehaviorSubject)

**Q3 : Comment éviter les memory leaks avec HttpClient ?**
HttpClient complète automatiquement après la réponse, mais toujours unsubscribe dans les autres cas.

**Q4 : Que fait l'opérateur `switchMap()` ?**
Annule la requête précédente et switch vers la nouvelle (utile pour la recherche).

**Q5 : Comment gérer le CORS ?**
Le backend doit configurer les headers CORS. Angular ne peut pas contourner CORS.

**Q6 : Pourquoi utiliser `tap()` au lieu de `subscribe()` ?**
`tap()` permet de faire des effets de bord dans le pipe sans s'abonner.

**Q7 : Comment typer les réponses HTTP ?**
```typescript
this.http.get<MyType>(url)  // Typage générique
```

**Q8 : Quelle est la différence entre `map()` et `tap()` ?**
- `map()` : transforme la valeur
- `tap()` : effet de bord, ne modifie pas la valeur

## Résumé

L'intégration d'une API REST avec Angular repose sur :
- ✅ **HttpClient** : service pour les requêtes HTTP
- ✅ **Observables** : gestion asynchrone avec RxJS
- ✅ **Services** : centralisation de la logique API
- ✅ **Signals** : cache réactif des données
- ✅ **Error Handling** : gestion centralisée des erreurs
- ✅ **Typage** : interfaces TypeScript pour la sécurité

**Points clés à retenir** :
- Configurer `provideHttpClient()` dans `app.config.ts`
- Utiliser les verbes HTTP appropriés (GET, POST, PUT, DELETE)
- Gérer les erreurs avec `catchError()`
- Implémenter un cache avec signals pour les performances
- Toujours unsubscribe dans `ngOnDestroy()`
- Typer les réponses HTTP avec des interfaces
- Utiliser des environnements pour les URLs d'API

**Félicitations !** Vous avez maintenant toutes les connaissances pour créer une application Angular professionnelle complète avec authentification, routing, formulaires réactifs, Material Design, et intégration API REST !
