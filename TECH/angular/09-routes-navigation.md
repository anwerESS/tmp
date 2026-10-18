# Chapitre 9 : Les Routes et la Navigation

## Introduction

Le système de routing d'Angular permet de créer des applications single-page (SPA) avec navigation entre différentes vues. Ce chapitre couvre la configuration des routes, la navigation programmatique, les paramètres d'URL, et les routes enfants.

## Concepts Clés

### Qu'est-ce que le Routing ?

Le routing permet de :
- Mapper des URLs à des composants spécifiques
- Naviguer entre différentes vues sans recharger la page
- Gérer l'historique du navigateur (boutons précédent/suivant)
- Passer des paramètres via l'URL
- Protéger certaines routes avec des guards

### Architecture du Router

Le système de routing Angular repose sur :
- **Routes** : configuration des chemins et composants
- **RouterOutlet** : emplacement où afficher les composants routés
- **RouterLink** : directive pour créer des liens de navigation
- **Router** : service pour la navigation programmatique
- **ActivatedRoute** : service pour accéder aux informations de la route active

## Configuration de Base

### 1. Fichier app.routes.ts

Le fichier `app.routes.ts` contient la configuration de toutes les routes de l'application.

```typescript
import { Routes } from '@angular/router';
import { CollectionDetail } from './pages/collection-detail/collection-detail';
import { CollectionItemDetail } from './pages/collection-item-detail/collection-item-detail';
import { NotFound } from './pages/not-found/not-found';

export const routes: Routes = [{
    path: '',
    redirectTo: 'home',
    pathMatch: 'full'
}, {
    path: 'home',
    component: CollectionDetail
}, {
    path: 'item/:id',
    component: CollectionItemDetail
}, {
    path: '**',
    component: NotFound
}];
```

**Explication** :
- `path: ''` : route vide (racine de l'application)
- `redirectTo: 'home'` : redirige vers /home
- `pathMatch: 'full'` : correspondance exacte du chemin complet
- `path: 'item/:id'` : route avec paramètre dynamique
- `path: '**'` : route wildcard pour les 404 (à placer en dernier)

### 2. RouterOutlet dans le Template

Le `RouterOutlet` est le conteneur où s'affichent les composants routés.

```html
<!-- app.html -->
<div id="page-container">
    <nav>
        <!-- Menu de navigation -->
    </nav>
    <main>
        <router-outlet></router-outlet>
    </main>
</div>
```

### 3. Configuration dans app.config.ts

```typescript
import { ApplicationConfig, provideZonelessChangeDetection } from '@angular/core';
import { provideRouter, withComponentInputBinding } from '@angular/router';
import { routes } from './app.routes';

export const appConfig: ApplicationConfig = {
  providers: [
    provideZonelessChangeDetection(),
    provideRouter(routes, withComponentInputBinding())
  ]
};
```

**Note** : `withComponentInputBinding()` permet de recevoir les paramètres de route directement comme inputs du composant.

## Paramètres de Route

### Définir un Paramètre

```typescript
{
    path: 'item/:id',
    component: CollectionItemDetail
}
```

Le `:id` est un paramètre dynamique qui peut contenir n'importe quelle valeur.

### Méthode 1 : Snapshot (lecture unique)

```typescript
import { Component, inject } from '@angular/core';
import { ActivatedRoute } from '@angular/router';

export class CollectionItemDetail {
    private route = inject(ActivatedRoute);
    
    ngOnInit() {
        const id = this.route.snapshot.paramMap.get('id');
        console.log('Item ID:', id);
    }
}
```

**Avantage** : Simple et direct  
**Inconvénient** : Ne se met pas à jour si les paramètres changent pendant que le composant est affiché

### Méthode 2 : Observable (réactif)

```typescript
import { Component, inject, OnDestroy } from '@angular/core';
import { ActivatedRoute } from '@angular/router';
import { Subscription } from 'rxjs';

export class CollectionItemDetail implements OnDestroy {
    private route = inject(ActivatedRoute);
    private paramSubscription: Subscription | null = null;
    
    ngOnInit() {
        this.paramSubscription = this.route.paramMap.subscribe(params => {
            const id = params.get('id');
            console.log('Item ID:', id);
            // Le code ici s'exécute à chaque changement de paramètre
        });
    }
    
    ngOnDestroy() {
        this.paramSubscription?.unsubscribe();
    }
}
```

**Avantage** : Réactif aux changements de paramètres  
**Inconvénient** : Plus de code, nécessite un unsubscribe

### Méthode 3 : Input Binding (recommandé avec Angular 19+)

Avec `withComponentInputBinding()` dans la configuration :

```typescript
import { Component, input } from '@angular/core';

export class CollectionItemDetail {
    // Le nom doit correspondre au nom du paramètre dans la route
    id = input<string>();
    
    ngOnInit() {
        console.log('Item ID:', this.id());
    }
}
```

**Avantage** : Code le plus simple et moderne  
**Inconvénient** : Nécessite Angular 16+ et la configuration withComponentInputBinding()

## Navigation

### Navigation avec RouterLink (HTML)

```html
<!-- Navigation simple -->
<a [routerLink]="['/home']">Accueil</a>

<!-- Navigation avec paramètre -->
<a [routerLink]="['/item', item.id]">Voir l'item</a>

<!-- Navigation relative -->
<a [routerLink]="['../']">Retour</a>

<!-- Avec classe active -->
<a [routerLink]="['/home']" routerLinkActive="active">Accueil</a>
```

### Navigation Programmatique (TypeScript)

```typescript
import { Component, inject } from '@angular/core';
import { Router } from '@angular/router';

export class MyComponent {
    private router = inject(Router);
    
    navigateToHome() {
        this.router.navigate(['/home']);
    }
    
    navigateToItem(id: number) {
        this.router.navigate(['/item', id]);
    }
    
    navigateWithQueryParams() {
        this.router.navigate(['/search'], {
            queryParams: { q: 'angular', page: 1 }
        });
        // URL résultante : /search?q=angular&page=1
    }
    
    navigateRelative() {
        // Navigation relative à la route actuelle
        this.router.navigate(['../'], { relativeTo: this.route });
    }
}
```

## Routes Enfants (Children Routes)

Les routes enfants permettent de créer une hiérarchie de navigation.

```typescript
export const routes: Routes = [{
    path: 'item',
    children: [{
        path: '',
        component: CollectionItemDetail
    }, {
        path: ':id',
        component: CollectionItemDetail
    }]
}, {
    path: 'admin',
    component: AdminLayoutComponent,
    children: [{
        path: 'users',
        component: UsersComponent
    }, {
        path: 'settings',
        component: SettingsComponent
    }]
}];
```

**Utilisation** :
- `/item` → affiche CollectionItemDetail sans ID
- `/item/123` → affiche CollectionItemDetail avec ID 123
- `/admin/users` → affiche AdminLayoutComponent avec UsersComponent à l'intérieur
- `/admin/settings` → affiche AdminLayoutComponent avec SettingsComponent à l'intérieur

Le composant parent (`AdminLayoutComponent`) doit avoir un `<router-outlet>` pour afficher ses enfants.

## Redirections

```typescript
export const routes: Routes = [{
    path: '',
    redirectTo: 'home',
    pathMatch: 'full'
}, {
    path: 'old-path',
    redirectTo: 'new-path',
    pathMatch: 'full'
}];
```

**pathMatch** :
- `'full'` : le chemin doit correspondre exactement
- `'prefix'` : le chemin doit commencer par (utilisé rarement)

## Route 404 (Wildcard)

```typescript
export const routes: Routes = [
    // ... autres routes ...
    {
        path: '**',
        component: NotFoundComponent
    }
];
```

**Important** : La route wildcard `**` doit toujours être la dernière, car elle capture toutes les URLs non matchées.

## Query Parameters et Fragments

### Passer des Query Parameters

```typescript
// Dans le template
<a [routerLink]="['/search']" 
   [queryParams]="{q: 'angular', category: 'tutorial'}">
   Rechercher
</a>

// Programmatiquement
this.router.navigate(['/search'], {
    queryParams: { q: 'angular', category: 'tutorial' }
});
// URL: /search?q=angular&category=tutorial
```

### Lire des Query Parameters

```typescript
import { Component, inject } from '@angular/core';
import { ActivatedRoute } from '@angular/router';

export class SearchComponent {
    private route = inject(ActivatedRoute);
    
    ngOnInit() {
        // Snapshot
        const query = this.route.snapshot.queryParamMap.get('q');
        
        // Observable
        this.route.queryParamMap.subscribe(params => {
            const query = params.get('q');
            const category = params.get('category');
        });
    }
}
```

### Fragments (ancres)

```typescript
// Navigation vers une section spécifique de la page
this.router.navigate(['/page'], { fragment: 'section-2' });
// URL: /page#section-2
```

## Exemple Complet : Application de Collection

### Structure des Routes

```typescript
// app.routes.ts
import { Routes } from '@angular/router';
import { CollectionDetail } from './pages/collection-detail/collection-detail';
import { CollectionItemDetail } from './pages/collection-item-detail/collection-item-detail';
import { LoginComponent } from './pages/login/login';
import { NotFound } from './pages/not-found/not-found';

export const routes: Routes = [{
    path: '',
    redirectTo: 'home',
    pathMatch: 'full'
}, {
    path: 'home',
    component: CollectionDetail
}, {
    path: 'item',
    children: [{
        path: '',
        component: CollectionItemDetail
    }, {
        path: ':id',
        component: CollectionItemDetail
    }]
}, {
    path: 'login',
    component: LoginComponent
}, {
    path: '**',
    component: NotFound
}];
```

### Page de Liste avec Navigation

```typescript
// collection-detail.ts
import { Component, inject } from '@angular/core';
import { Router } from '@angular/router';
import { CollectionItem } from '../../models/collection-item';

export class CollectionDetail {
    private router = inject(Router);
    items: CollectionItem[] = [
        { id: 1, name: 'Timbre rare', year: 1920 },
        { id: 2, name: 'Pièce ancienne', year: 1850 }
    ];
    
    viewItem(item: CollectionItem) {
        this.router.navigate(['/item', item.id]);
    }
    
    createNewItem() {
        this.router.navigate(['/item']);
    }
}
```

```html
<!-- collection-detail.html -->
<div class="collection-list">
    <h2>Ma Collection</h2>
    
    <button (click)="createNewItem()">
        Nouvel Item
    </button>
    
    <div *ngFor="let item of items" class="item-card">
        <h3>{{ item.name }}</h3>
        <p>Année: {{ item.year }}</p>
        <a [routerLink]="['/item', item.id]">Voir détails</a>
        <!-- ou -->
        <button (click)="viewItem(item)">Voir détails</button>
    </div>
</div>
```

### Page de Détail avec Paramètre

```typescript
// collection-item-detail.ts
import { Component, inject, input } from '@angular/core';
import { Router } from '@angular/router';
import { CollectionService } from '../../services/collection/collection-service';

export class CollectionItemDetail {
    private router = inject(Router);
    private collectionService = inject(CollectionService);
    
    // Récupération automatique du paramètre de route
    id = input<string>();
    item: CollectionItem | null = null;
    
    ngOnInit() {
        const itemId = this.id();
        if (itemId) {
            // Mode édition
            this.item = this.collectionService.getItemById(parseInt(itemId));
        } else {
            // Mode création
            this.item = new CollectionItem();
        }
    }
    
    save() {
        if (this.item) {
            this.collectionService.saveItem(this.item);
            this.goBack();
        }
    }
    
    goBack() {
        this.router.navigate(['/home']);
    }
}
```

```html
<!-- collection-item-detail.html -->
<div class="item-detail">
    <h2>{{ id() ? 'Modifier' : 'Nouveau' }} Item</h2>
    
    <form (submit)="save()">
        <input [(ngModel)]="item.name" placeholder="Nom">
        <input type="number" [(ngModel)]="item.year" placeholder="Année">
        
        <button type="submit">Enregistrer</button>
        <button type="button" (click)="goBack()">Annuler</button>
    </form>
</div>
```

## Navigation Guards (Aperçu)

Les guards permettent de contrôler l'accès aux routes (authentification, autorisation, etc.). Ils seront détaillés dans le chapitre 12.

```typescript
{
    path: 'admin',
    component: AdminComponent,
    canActivate: [authGuard]  // Vérifie avant d'activer la route
}
```

## Bonnes Pratiques

### 1. Organisation des Routes

```typescript
// Grouper les routes par fonctionnalité
const adminRoutes: Routes = [
    { path: 'users', component: UsersComponent },
    { path: 'settings', component: SettingsComponent }
];

export const routes: Routes = [
    { path: 'admin', children: adminRoutes },
    // ... autres routes
];
```

### 2. Lazy Loading (Chargement Paresseux)

Pour les grandes applications, charger les modules à la demande :

```typescript
{
    path: 'admin',
    loadChildren: () => import('./admin/admin.routes')
        .then(m => m.ADMIN_ROUTES)
}
```

### 3. Nommage des Routes

```typescript
{
    path: 'item/:id',
    component: ItemComponent,
    title: 'Détails de l\'item'  // Titre de la page
}
```

### 4. Navigation Sûre

Toujours vérifier que la navigation a réussi :

```typescript
this.router.navigate(['/home']).then(success => {
    if (success) {
        console.log('Navigation réussie');
    } else {
        console.log('Navigation bloquée');
    }
});
```

## Debugging du Routing

### Activer le Tracing

```typescript
// app.config.ts
export const appConfig: ApplicationConfig = {
  providers: [
    provideRouter(routes, 
        withComponentInputBinding(),
        withDebugTracing()  // Active les logs de navigation
    )
  ]
};
```

### Logs Courants

Dans la console, vous verrez :
- `NavigationStart` : début de la navigation
- `RoutesRecognized` : routes identifiées
- `NavigationEnd` : navigation terminée
- `NavigationError` : erreur de navigation
- `NavigationCancel` : navigation annulée

## Exercices Pratiques

### Exercice 1 : Navigation de Base
Créez une application avec 3 pages (Accueil, À propos, Contact) et un menu de navigation.

### Exercice 2 : Route avec Paramètres
Créez une page "Produits" qui liste des produits, et une page "Détail Produit" qui affiche les détails d'un produit sélectionné.

### Exercice 3 : Routes Enfants
Créez un dashboard admin avec des sous-sections (utilisateurs, statistiques, paramètres).

### Exercice 4 : Query Parameters
Ajoutez une barre de recherche qui utilise les query parameters pour filtrer des résultats.

## Questions d'Entretien Fréquentes

**Q1 : Quelle est la différence entre `routerLink` et `href` ?**
- `routerLink` : navigation SPA, pas de rechargement de page, gestion par Angular Router
- `href` : navigation classique, rechargement complet de la page

**Q2 : Comment passer des données entre routes ?**
Plusieurs méthodes :
- Paramètres d'URL (`/item/:id`)
- Query parameters (`/search?q=angular`)
- State de navigation : `router.navigate(['/page'], { state: { data: myData } })`
- Service partagé

**Q3 : Que fait `pathMatch: 'full'` ?**
Spécifie que le chemin doit correspondre exactement à l'URL complète pour activer la redirection.

**Q4 : Quelle est la différence entre `snapshot` et `subscribe` pour les paramètres ?**
- `snapshot` : lecture unique au chargement du composant
- `subscribe` : observable qui se met à jour si les paramètres changent

**Q5 : Pourquoi la route wildcard `**` doit être en dernier ?**
Car elle capture toutes les URLs. Si elle est avant les autres routes, elles ne seront jamais atteintes.

**Q6 : Comment faire une navigation relative ?**
Utiliser `relativeTo` dans les options de navigation :
```typescript
this.router.navigate(['../'], { relativeTo: this.route });
```

**Q7 : Qu'est-ce que `withComponentInputBinding()` ?**
Permet de recevoir les paramètres de route directement comme inputs du composant, simplifiant considérablement le code.

**Q8 : Comment gérer les routes enfants avec plusieurs niveaux ?**
Chaque composant parent doit avoir son propre `<router-outlet>` pour afficher ses enfants.

## Résumé

Le routing Angular permet de :
- ✅ Créer des applications multi-pages sans rechargement
- ✅ Passer des données via l'URL (paramètres et query params)
- ✅ Organiser les routes de manière hiérarchique
- ✅ Rediriger automatiquement certaines URLs
- ✅ Gérer les pages 404
- ✅ Naviguer programmatiquement depuis le code TypeScript

**Points clés à retenir** :
- Configurer les routes dans `app.routes.ts`
- Utiliser `<router-outlet>` pour afficher les composants
- Préférer `input()` avec `withComponentInputBinding()` pour les paramètres (Angular 16+)
- Toujours placer la route wildcard `**` en dernier
- Utiliser `routerLink` pour les liens HTML et `Router.navigate()` pour le code TypeScript

Dans le prochain chapitre, nous verrons comment créer des formulaires réactifs pour gérer les saisies utilisateur de manière robuste.
