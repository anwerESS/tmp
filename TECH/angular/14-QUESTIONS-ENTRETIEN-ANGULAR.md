# 🎯 Guide d'Entretien Angular - Questions/Réponses Orales

## 📋 Table des Matières

1. [Questions Générales sur Angular](#questions-générales)
2. [Components et Architecture](#components-et-architecture)
3. [Data Binding et Communication](#data-binding-et-communication)
4. [Services et Injection de Dépendances](#services-et-injection)
5. [Routing et Navigation](#routing-et-navigation)
6. [Formulaires](#formulaires)
7. [HTTP et API](#http-et-api)
8. [RxJS et Observables](#rxjs-et-observables)
9. [Performance et Optimisation](#performance-et-optimisation)
10. [Signals (Nouvelle API)](#signals-nouvelle-api)
11. [Angular Material](#angular-material)
12. [Sécurité](#sécurité)
13. [Testing](#testing)
14. [Questions Avancées](#questions-avancées)

---

## Questions Générales sur Angular

### Q1: Qu'est-ce qu'Angular ?

**Réponse orale** :

"Angular est un framework open-source développé par Google pour créer des applications web. C'est un framework complet qui utilise TypeScript et qui est basé sur une architecture par composants. 

La particularité d'Angular, c'est qu'il permet de créer des Single Page Applications, c'est-à-dire des applications où on ne recharge jamais complètement la page. Tout se passe côté client de manière fluide.

Angular fournit tout ce dont on a besoin : le routing pour la navigation, la gestion des formulaires, les appels HTTP, l'injection de dépendances, etc. C'est pour ça qu'on dit que c'est un framework 'opinionated' - il impose une certaine façon de structurer l'application, ce qui est un avantage pour la maintenabilité dans les grandes équipes."

**Points à mentionner si le recruteur creuse** :
- Développé par Google (2016)
- Basé sur TypeScript (typage fort)
- Architecture MVC/MVVM
- CLI puissant pour la génération de code
- Écosystème riche (Material, CDK, Universal...)

---

### Q2: Quelle est la différence entre Angular et AngularJS ?

**Réponse orale** :

"Ah oui, c'est une question importante ! AngularJS et Angular sont complètement différents.

AngularJS, c'est l'ancienne version, la v1, qui est sortie en 2010. Elle utilisait JavaScript pur et avait une architecture basée sur les scopes et les controllers.

Angular, à partir de la version 2, c'est une réécriture complète sortie en 2016. Les différences principales :

- AngularJS utilisait JavaScript, Angular utilise TypeScript
- AngularJS utilisait des controllers et $scope, Angular utilise des composants
- AngularJS utilisait two-way binding partout, Angular est plus performant avec one-way binding par défaut
- Il n'y a aucune compatibilité entre les deux - on ne peut pas migrer facilement

Aujourd'hui, AngularJS n'est plus maintenu depuis 2022, donc quand on parle d'Angular, on parle toujours d'Angular 2+. Actuellement on est à Angular 20, mais la version n'importe plus vraiment car les mises à jour sont semestrielles et gérées par le CLI."

---

### Q3: Quels sont les avantages d'Angular par rapport à React ou Vue ?

**Réponse orale** :

"Alors, chaque framework a ses forces. Pour Angular :

**Les avantages principaux** :
- C'est un framework complet - on n'a pas besoin de chercher des bibliothèques tierces pour le routing, les formulaires, ou les appels HTTP. Tout est intégré et cohérent.
- Le typage fort avec TypeScript est obligatoire, ce qui évite beaucoup de bugs en production
- L'architecture est plus stricte, ce qui est excellent pour les grosses équipes - tout le monde code de la même façon
- Le CLI est très puissant pour générer du code et gérer les dépendances
- Google le maintient activement avec un cycle de release prévisible

**Par rapport à React** :
- Angular est plus structuré, React laisse plus de liberté (ce qui peut être un avantage ou un inconvénient)
- Angular inclut tout, React nécessite d'ajouter des bibliothèques (React Router, Redux, etc.)

**Par rapport à Vue** :
- Angular est plus adapté aux grosses applications d'entreprise
- La courbe d'apprentissage d'Angular est plus raide

En résumé, j'utiliserais Angular pour une grosse application d'entreprise avec beaucoup de développeurs, React pour plus de flexibilité, et Vue pour démarrer rapidement."

---

### Q4: Qu'est-ce que TypeScript et pourquoi Angular l'utilise ?

**Réponse orale** :

"TypeScript, c'est un sur-ensemble de JavaScript développé par Microsoft. Ça veut dire que tout code JavaScript valide est aussi du TypeScript valide, mais TypeScript ajoute des fonctionnalités supplémentaires.

La principale fonctionnalité, c'est le **typage statique**. En JavaScript classique, je peux écrire :
```javascript
let age = 25;
age = 'vingt-cinq'; // Ça marche, mais c'est dangereux
```

Avec TypeScript :
```typescript
let age: number = 25;
age = 'vingt-cinq'; // Erreur à la compilation
```

**Pourquoi Angular l'utilise** :
1. **Détection d'erreurs précoce** - Les erreurs sont trouvées pendant le développement, pas en production
2. **Autocomplétion** - L'IDE sait exactement quelles propriétés et méthodes sont disponibles
3. **Refactoring sûr** - On peut renommer une variable et être sûr que toutes les références sont mises à jour
4. **Documentation vivante** - Les types servent de documentation
5. **Meilleure maintenabilité** - Surtout dans les grandes équipes

Pour moi, TypeScript est devenu indispensable. Une fois qu'on y goûte, il est difficile de revenir au JavaScript pur !"

---

## Components et Architecture

### Q5: Qu'est-ce qu'un Component Angular ?

**Réponse orale** :

"Un component, c'est la brique de base d'une application Angular. C'est une classe TypeScript qui encapsule :
- La vue (le HTML)
- La logique (le TypeScript)
- Le style (le CSS ou SCSS)

Concrètement, quand je crée un component avec `ng g c mon-composant`, Angular génère 4 fichiers :
- `mon-composant.ts` - la logique
- `mon-composant.html` - le template
- `mon-composant.scss` - les styles
- `mon-composant.spec.ts` - les tests

Le component est marqué par le décorateur `@Component` qui définit :
```typescript
@Component({
  selector: 'app-mon-composant',  // Comment on l'utilise en HTML
  templateUrl: './mon-composant.html',
  styleUrls: ['./mon-composant.scss']
})
```

L'idée, c'est qu'un component doit être **réutilisable** et avoir une **responsabilité unique**. Par exemple, un component 'UserCard' affiche une carte utilisateur, un component 'SearchBar' gère la recherche, etc.

C'est cette approche par composants qui rend Angular modulaire et maintenable."

---

### Q6: Expliquez le cycle de vie d'un component

**Réponse orale** :

"Le cycle de vie d'un component, ce sont les différentes étapes qu'il traverse depuis sa création jusqu'à sa destruction. Angular nous donne des hooks pour exécuter du code à chaque étape.

**Les hooks principaux dans l'ordre** :

1. **ngOnInit** - C'est le plus important, appelé une seule fois après la création. C'est ici qu'on charge les données initiales.

2. **ngOnChanges** - Appelé quand un `@Input()` change. Utile pour réagir aux changements venant du parent.

3. **ngAfterViewInit** - Appelé quand la vue est complètement initialisée. On l'utilise quand on doit accéder au DOM.

4. **ngOnDestroy** - Appelé juste avant la destruction. **Crucial** pour le cleanup : désabonnement des Observables, arrêt des timers, etc.

**Un exemple concret** :
```typescript
export class MyComponent implements OnInit, OnDestroy {
  private subscription: Subscription;
  
  ngOnInit() {
    // Charger les données
    this.subscription = this.service.getData().subscribe(...)
  }
  
  ngOnDestroy() {
    // Nettoyer pour éviter les memory leaks
    this.subscription.unsubscribe();
  }
}
```

Le plus important à retenir : **toujours unsubscribe dans ngOnDestroy** pour éviter les fuites mémoire. C'est une erreur classique des débutants."

---

### Q7: Quelle est la différence entre constructor et ngOnInit ?

**Réponse orale** :

"Bonne question ! C'est une confusion fréquente.

**Le constructor** :
- C'est une fonction TypeScript/JavaScript standard
- Il est appelé quand Angular crée l'instance de la classe
- On l'utilise **uniquement** pour l'injection de dépendances
- Les `@Input()` ne sont PAS encore disponibles
- La vue n'est PAS encore créée

**ngOnInit** :
- C'est un hook Angular spécifique
- Il est appelé après que tous les `@Input()` sont initialisés
- On l'utilise pour toute initialisation qui nécessite des données du component
- C'est ici qu'on fait les appels API

**Exemple concret** :
```typescript
export class UserProfile {
  @Input() userId!: number;
  user: User;
  
  constructor(private userService: UserService) {
    // ❌ ERREUR - userId n'est pas encore disponible
    this.user = this.userService.getUser(this.userId);
  }
  
  ngOnInit() {
    // ✅ BON - userId est maintenant disponible
    this.user = this.userService.getUser(this.userId);
  }
}
```

**Règle simple** : Constructor pour l'injection, ngOnInit pour l'initialisation."

---

### Q8: Qu'est-ce qu'un Standalone Component ?

**Réponse orale** :

"Les Standalone Components, c'est la nouvelle façon de créer des components depuis Angular 14, et c'est devenu la méthode par défaut depuis Angular 17.

**Avant (avec NgModules)** :
On devait créer des modules qui déclaraient les components :
```typescript
@NgModule({
  declarations: [MyComponent],
  imports: [CommonModule, FormsModule],
  exports: [MyComponent]
})
export class MyModule { }
```

**Maintenant (Standalone)** :
Le component déclare directement ce qu'il utilise :
```typescript
@Component({
  selector: 'app-my-component',
  standalone: true,  // C'est ça la clé !
  imports: [CommonModule, FormsModule],
  template: '...'
})
export class MyComponent { }
```

**Les avantages** :
- **Moins de boilerplate** - plus besoin de créer des NgModules partout
- **Plus simple** à comprendre - tout est au même endroit
- **Lazy loading** plus facile
- **Tree shaking** amélioré - le bundle final est plus petit

C'est vraiment un game-changer ! Ça rend Angular beaucoup plus accessible et proche de React en termes de simplicité. Si je commence un nouveau projet aujourd'hui, j'utilise forcément des Standalone Components."

---

## Data Binding et Communication

### Q9: Expliquez les différents types de data binding

**Réponse orale** :

"Le data binding, c'est la communication entre le TypeScript et le HTML. Angular propose 4 types :

**1. Interpolation {{ }}** - One-way : TypeScript → HTML
```html
<h1>Bonjour {{ username }}</h1>
```
C'est le plus simple, on affiche juste une valeur.

**2. Property Binding [property]** - One-way : TypeScript → HTML
```html
<img [src]="imageUrl" [alt]="imageAlt">
<button [disabled]="isLoading">Envoyer</button>
```
On lie une propriété TypeScript à une propriété HTML.

**3. Event Binding (event)** - One-way : HTML → TypeScript
```html
<button (click)="save()">Sauvegarder</button>
<input (keyup)="onKeyUp($event)">
```
On écoute les événements HTML et on appelle une méthode TypeScript.

**4. Two-Way Binding [(ngModel)]** - Bidirectionnel
```html
<input [(ngModel)]="username">
```
Le TypeScript et le HTML sont synchronisés dans les deux sens.

**Un truc pour se souvenir** :
- `[]` = données qui SORTENT du TypeScript vers le HTML
- `()` = événements qui ENTRENT du HTML vers le TypeScript
- `[()]` = les deux en même temps, d'où le surnom 'banana in a box' 🍌

Le plus important : **éviter le two-way binding partout** car ça peut créer des problèmes de performance. Préférer le one-way binding et gérer manuellement les changements."

---

### Q10: Comment faire communiquer deux components ?

**Réponse orale** :

"Il y a plusieurs façons de faire communiquer des components, selon leur relation.

**1. Parent → Enfant : @Input()** ou **input()**
```typescript
// Parent
<app-child [data]="myData"></app-child>

// Enfant
@Input() data: DataType;
// ou avec signals (nouvelle API)
data = input.required<DataType>();
```

**2. Enfant → Parent : @Output()** ou **output()**
```typescript
// Enfant
@Output() itemSelected = new EventEmitter<Item>();
onSelect(item: Item) {
  this.itemSelected.emit(item);
}

// Parent
<app-child (itemSelected)="handleSelection($event)"></app-child>
```

**3. Two-Way Binding : model()**
```typescript
// Enfant
searchTerm = model<string>('');

// Parent
<app-search [(searchTerm)]="mySearchTerm"></app-search>
```

**4. Components non liés : Service partagé**
```typescript
@Injectable({ providedIn: 'root' })
export class DataService {
  private dataSubject = new BehaviorSubject<Data>(null);
  data$ = this.dataSubject.asObservable();
  
  updateData(data: Data) {
    this.dataSubject.next(data);
  }
}

// Les components s'abonnent au service
```

**Règle de choix** :
- Relation Parent-Enfant directe → Input/Output
- Two-way simple → model()
- Components éloignés ou logique métier → Service

Le service est souvent la meilleure solution pour du code maintenable."

---

### Q11: Quelle est la différence entre @Input() et input() ?

**Réponse orale** :

"Excellente question ! C'est la différence entre l'ancienne et la nouvelle API Angular.

**@Input() - Ancienne API** (décorateur) :
```typescript
@Input() data: DataType;
@Input() required!: string;  // ! pour requis
@Input() optional?: number;  // ? pour optionnel
```

**input() - Nouvelle API** (fonction, depuis Angular 17) :
```typescript
data = input<DataType>();              // optionnel
required = input.required<string>();   // requis
withDefault = input<number>(42);       // avec valeur par défaut
```

**Les différences importantes** :

1. **Syntaxe plus claire**
```typescript
// Ancien
@Input() userId!: number;
// Nouveau - on voit immédiatement que c'est requis
userId = input.required<number>();
```

2. **Signal intégré**
```typescript
// Ancien
@Input() name: string;
console.log(this.name);  // Valeur directe

// Nouveau
name = input<string>();
console.log(this.name());  // Fonction - c'est un signal !
```

3. **Computed automatique**
```typescript
fullName = computed(() => 
  `${this.firstName()} ${this.lastName()}`
);
// Se met à jour automatiquement quand firstName ou lastName change
```

4. **Meilleure performance** - Angular sait exactement ce qui a changé

**Mon conseil** : Sur un nouveau projet, utilisez `input()`. C'est l'avenir d'Angular et c'est beaucoup plus élégant. Par contre, sur un projet existant, les deux syntaxes cohabitent sans problème."

---

## Services et Injection de Dépendances

### Q12: Qu'est-ce qu'un Service Angular ?

**Réponse orale** :

"Un service, c'est une classe qui contient de la logique métier réutilisable. L'idée, c'est de **sortir la logique des components** pour la centraliser.

**Pourquoi créer un service** :
- Partager des données entre plusieurs components
- Centraliser les appels API
- Éviter la duplication de code
- Rendre le code testable

**Un service classique** :
```typescript
@Injectable({ providedIn: 'root' })
export class UserService {
  private users = signal<User[]>([]);
  
  getUsers(): Observable<User[]> {
    return this.http.get<User[]>('/api/users');
  }
  
  addUser(user: User): Observable<User> {
    return this.http.post<User>('/api/users', user);
  }
}
```

**L'annotation `@Injectable({ providedIn: 'root' })`** est cruciale :
- Elle rend le service injectable dans toute l'application
- `providedIn: 'root'` signifie qu'Angular crée une **seule instance** (singleton) partagée par tous

**Utilisation dans un component** :
```typescript
export class UserList {
  private userService = inject(UserService);
  
  ngOnInit() {
    this.userService.getUsers().subscribe(users => {
      console.log(users);
    });
  }
}
```

Un service bien fait doit avoir **une seule responsabilité**. Par exemple : UserService pour les utilisateurs, AuthService pour l'authentification, etc."

---

### Q13: Qu'est-ce que l'injection de dépendances ?

**Réponse orale** :

"L'injection de dépendances, ou DI (Dependency Injection), c'est un pattern où Angular crée et fournit automatiquement les instances des classes dont on a besoin.

**Sans DI (ancien style)** :
```typescript
class MyComponent {
  private service: UserService;
  
  constructor() {
    this.service = new UserService(); // ❌ Couplage fort
  }
}
```
Problèmes : difficile à tester, couplage fort, on doit gérer la création.

**Avec DI (Angular)** :
```typescript
class MyComponent {
  private service = inject(UserService); // ✅ Angular fournit l'instance
}
```

**Comment ça marche** :
1. Angular maintient un **injecteur** (Injector)
2. On déclare ce qui est injectable avec `@Injectable()`
3. Quand on demande quelque chose avec `inject()`, Angular :
   - Vérifie si une instance existe déjà
   - Si oui, il la réutilise (singleton)
   - Si non, il la crée

**Les avantages** :
- **Testabilité** - On peut facilement mocker les dépendances
- **Découplage** - Le component ne sait pas comment créer le service
- **Réutilisation** - Une seule instance partagée (par défaut)
- **Flexibilité** - On peut changer l'implémentation sans toucher au component

**Nouvelle syntaxe (recommandée)** :
```typescript
// Ancien (dans constructor)
constructor(private service: UserService) {}

// Nouveau (avec inject)
private service = inject(UserService);
```

La nouvelle syntaxe est plus concise et fonctionne même en dehors du constructor !"

---

### Q14: Quelle est la différence entre providedIn: 'root' et providers: [] ?

**Réponse orale** :

"Ah, c'est une question importante sur le scope des services !

**providedIn: 'root'** - Singleton global
```typescript
@Injectable({ providedIn: 'root' })
export class UserService { }
```
- **Une seule instance** pour toute l'application
- Disponible partout automatiquement
- C'est le choix par défaut et recommandé
- Tree-shakable - si personne n'utilise le service, il n'est pas inclus dans le bundle

**providers: []** - Scope limité
```typescript
@Component({
  selector: 'app-my-component',
  providers: [MyService]  // ⚠️ Nouvelle instance pour ce component
})
export class MyComponent { }
```
- **Nouvelle instance** créée pour ce component (et ses enfants)
- Utile quand on veut isoler l'état
- Pas tree-shakable

**Exemple concret où providers: [] est utile** :
Imaginons un component de formulaire multi-étapes :
```typescript
@Injectable()
export class FormStateService {
  currentStep = signal(1);
  formData = signal({});
}

@Component({
  providers: [FormStateService]  // Nouvelle instance par formulaire
})
export class MultiStepForm { }
```
Chaque instance du formulaire a son propre état, elles ne se mélangent pas.

**Règle simple** :
- Service partagé globalement (API, Auth) → `providedIn: 'root'`
- État isolé par component → `providers: []`
- 90% du temps, c'est `providedIn: 'root'` !"

---

## Routing et Navigation

### Q15: Comment fonctionne le routing dans Angular ?

**Réponse orale** :

"Le routing dans Angular, c'est ce qui permet de naviguer entre les différentes pages sans recharger le navigateur. C'est l'essence d'une SPA.

**La configuration se fait en 3 étapes** :

**1. Définir les routes** (app.routes.ts) :
```typescript
export const routes: Routes = [
  { path: '', redirectTo: 'home', pathMatch: 'full' },
  { path: 'home', component: HomeComponent },
  { path: 'users/:id', component: UserDetailComponent },
  { path: '**', component: NotFoundComponent }  // 404
];
```

**2. Enregistrer dans app.config.ts** :
```typescript
export const appConfig: ApplicationConfig = {
  providers: [
    provideRouter(routes)
  ]
};
```

**3. Ajouter le RouterOutlet** dans le template :
```html
<nav>
  <a routerLink="/home">Accueil</a>
  <a routerLink="/about">À propos</a>
</nav>
<router-outlet></router-outlet>
```

**Navigation programmatique** :
```typescript
export class MyComponent {
  private router = inject(Router);
  
  goToUser(id: number) {
    this.router.navigate(['/users', id]);
  }
}
```

**Récupérer les paramètres** :
```typescript
// Ancienne façon
route = inject(ActivatedRoute);
userId = this.route.snapshot.paramMap.get('id');

// Nouvelle façon (avec signals)
userId = input<string>();  // Grâce à withComponentInputBinding()
```

Le routing Angular est très puissant : lazy loading, guards, resolvers, routes enfants... C'est vraiment un de ses points forts !"

---

### Q16: Qu'est-ce qu'un Guard et à quoi ça sert ?

**Réponse orale** :

"Un guard, c'est une fonction qui **protège** une route. Angular l'exécute avant d'activer la route, et selon le résultat, autorise ou bloque la navigation.

**Les types de guards** :
- `CanActivate` - Peut-on accéder à cette route ?
- `CanDeactivate` - Peut-on quitter cette route ?
- `CanActivateChild` - Peut-on accéder aux routes enfants ?

**Exemple concret : protéger une page admin** :
```typescript
export const authGuard: CanActivateFn = () => {
  const authService = inject(AuthService);
  const router = inject(Router);
  
  if (authService.isLoggedIn()) {
    return true;  // ✅ Accès autorisé
  }
  
  return router.createUrlTree(['/login']);  // ❌ Redirection
};

// Application dans les routes
{
  path: 'admin',
  component: AdminComponent,
  canActivate: [authGuard]
}
```

**Cas d'usage typiques** :
- Vérifier l'authentification
- Vérifier les permissions (admin, user...)
- Confirmer avant de quitter (formulaire non sauvegardé)
- Précharger des données (avec Resolve)

**CanDeactivate - exemple pratique** :
```typescript
export const unsavedChangesGuard: CanDeactivateFn<FormComponent> = 
  (component) => {
    if (component.hasUnsavedChanges()) {
      return confirm('Voulez-vous vraiment quitter sans sauvegarder ?');
    }
    return true;
  };
```

Les guards sont essentiels pour la sécurité et l'UX. Sur une vraie application, presque toutes les routes protégées en ont !"

---

### Q17: Quelle est la différence entre routerLink et href ?

**Réponse orale** :

"Ah oui, c'est une distinction importante !

**href (HTML standard)** :
```html
<a href="/about">À propos</a>
```
- Recharge **complètement** la page
- Perd l'état de l'application
- Requête complète au serveur
- Comme un site web classique

**routerLink (Angular)** :
```html
<a routerLink="/about">À propos</a>
```
- Navigation **client-side** uniquement
- Pas de rechargement
- Garde l'état de l'application
- Beaucoup plus rapide

**Démonstration visuelle** :
Avec `href`, si j'ai un compteur qui est à 10, il repart à 0 après la navigation. Avec `routerLink`, il reste à 10.

**Fonctionnalités de routerLink** :
```html
<!-- Navigation simple -->
<a routerLink="/users">Utilisateurs</a>

<!-- Avec paramètres -->
<a [routerLink]="['/users', userId]">Profil</a>

<!-- Avec query params -->
<a [routerLink]="['/search']" [queryParams]="{q: 'angular'}">Rechercher</a>

<!-- Active class automatique -->
<a routerLink="/home" routerLinkActive="active">Accueil</a>
```

**Règle simple** : 
- Navigation interne à l'app → `routerLink`
- Lien externe → `href`
- **JAMAIS** `href` pour de la navigation interne !

C'est une erreur classique des débutants qui utilisent `href` partout et se demandent pourquoi l'app recharge à chaque clic."

---

## Formulaires

### Q18: Quelle est la différence entre Template-Driven et Reactive Forms ?

**Réponse orale** :

"Angular propose deux approches pour les formulaires, et c'est important de comprendre quand utiliser chacune.

**Template-Driven Forms** :
- La logique est dans le **HTML**
- Utilise `ngModel`
- Simple et rapide
- Bon pour des formulaires basiques

```html
<form #myForm="ngForm" (submit)="onSubmit(myForm)">
  <input name="email" ngModel required email>
  <button>Envoyer</button>
</form>
```

**Reactive Forms** (recommandé) :
- La logique est dans le **TypeScript**
- Utilise `FormGroup` et `FormControl`
- Plus puissant et testable
- Pour les formulaires complexes

```typescript
loginForm = this.formBuilder.group({
  email: ['', [Validators.required, Validators.email]],
  password: ['', [Validators.required, Validators.minLength(8)]]
});
```

**Tableau comparatif** :

| Critère | Template-Driven | Reactive |
|---------|----------------|----------|
| Logique | HTML | TypeScript |
| Testabilité | Difficile | Facile |
| Validation | Attributs HTML | Validators TS |
| Async | Complexe | Simple |
| Scalabilité | Limitée | Excellente |

**Mon conseil** : 
- Formulaire de contact basique → Template-Driven
- Formulaire de login/inscription → Reactive
- Formulaire dynamique/complexe → **Toujours** Reactive

Sur un projet professionnel, j'utilise pratiquement toujours les Reactive Forms. C'est plus verbeux au début, mais beaucoup plus maintenable."

---

### Q19: Comment valider un formulaire ?

**Réponse orale** :

"La validation dans Angular est très complète. Je vais vous montrer avec un Reactive Form.

**1. Validateurs intégrés** :
```typescript
userForm = this.formBuilder.group({
  // Champ requis
  name: ['', Validators.required],
  
  // Email valide
  email: ['', [Validators.required, Validators.email]],
  
  // Longueur min/max
  username: ['', [
    Validators.minLength(3), 
    Validators.maxLength(20)
  ]],
  
  // Valeur numérique min/max
  age: [null, [Validators.min(18), Validators.max(100)]],
  
  // Pattern (regex)
  phone: ['', Validators.pattern(/^[0-9]{10}$/)],
});
```

**2. Afficher les erreurs** :
```html
<input formControlName="email">
@if (userForm.controls.email.invalid && 
     userForm.controls.email.touched) {
  <div class="error">
    @if (userForm.controls.email.hasError('required')) {
      L'email est requis
    }
    @if (userForm.controls.email.hasError('email')) {
      Format email invalide
    }
  </div>
}
```

**3. Validateur personnalisé** :
```typescript
function passwordMatchValidator(): ValidatorFn {
  return (control: AbstractControl): ValidationErrors | null => {
    const password = control.get('password');
    const confirm = control.get('confirmPassword');
    
    return password?.value === confirm?.value 
      ? null 
      : { passwordMismatch: true };
  };
}

// Utilisation
registrationForm = this.formBuilder.group({
  password: ['', Validators.required],
  confirmPassword: ['', Validators.required]
}, { 
  validators: passwordMatchValidator() 
});
```

**4. Validateur asynchrone** (vérification serveur) :
```typescript
function usernameAvailableValidator(userService: UserService): AsyncValidatorFn {
  return (control: AbstractControl): Observable<ValidationErrors | null> => {
    return userService.checkUsername(control.value).pipe(
      map(isAvailable => isAvailable ? null : { usernameTaken: true })
    );
  };
}
```

**États importants** :
- `valid` / `invalid` - État de validation
- `touched` - L'utilisateur a cliqué puis quitté le champ
- `dirty` - L'utilisateur a modifié la valeur
- `pristine` - Jamais modifié

La validation Angular est vraiment complète. On peut gérer des scénarios très complexes !"

---

### Q20: Qu'est-ce que FormBuilder et pourquoi l'utiliser ?

**Réponse orale** :

"FormBuilder, c'est un utilitaire qui **simplifie** la création de formulaires réactifs.

**Sans FormBuilder** (verbeux) :
```typescript
loginForm = new FormGroup({
  email: new FormControl('', [Validators.required, Validators.email]),
  password: new FormControl('', [Validators.required, Validators.minLength(8)])
});
```

**Avec FormBuilder** (concis) :
```typescript
private formBuilder = inject(FormBuilder);

loginForm = this.formBuilder.group({
  email: ['', [Validators.required, Validators.email]],
  password: ['', [Validators.required, Validators.minLength(8)]]
});
```

**Encore plus simple avec** :
```typescript
// Juste la valeur initiale
loginForm = this.formBuilder.group({
  email: [''],
  password: ['']
});

// Ou avec un objet d'options
loginForm = this.formBuilder.group({
  email: ['', {
    validators: [Validators.required, Validators.email],
    updateOn: 'blur'  // Valider au blur, pas à chaque keystroke
  }]
});
```

**FormBuilder supporte aussi** :
- `formBuilder.control()` - Créer un FormControl
- `formBuilder.array()` - Créer un FormArray (liste dynamique)
- `formBuilder.record()` - Pour des objets dynamiques

**Exemple de FormArray** :
```typescript
skillsForm = this.formBuilder.group({
  skills: this.formBuilder.array([
    this.formBuilder.group({ name: '', level: '' })
  ])
});

addSkill() {
  this.skills.push(
    this.formBuilder.group({ name: '', level: '' })
  );
}
```

**Pourquoi l'utiliser** :
- Code plus lisible et concis
- Moins de répétition
- C'est la pratique standard dans l'industrie

Honnêtement, je n'ai jamais vu de projet professionnel qui n'utilise pas FormBuilder. C'est un must-have !"

---

## HTTP et API

### Q21: Comment faire des appels API avec Angular ?

**Réponse orale** :

"Pour les appels API, Angular fournit HttpClient, qui est très puissant et basé sur RxJS.

**1. Configuration** (app.config.ts) :
```typescript
import { provideHttpClient } from '@angular/common/http';

export const appConfig: ApplicationConfig = {
  providers: [
    provideHttpClient()
  ]
};
```

**2. Créer un service API** :
```typescript
@Injectable({ providedIn: 'root' })
export class UserService {
  private apiUrl = 'https://api.exemple.com';
  private http = inject(HttpClient);
  
  // GET - Récupérer la liste
  getUsers(): Observable<User[]> {
    return this.http.get<User[]>(`${this.apiUrl}/users`);
  }
  
  // GET - Récupérer un élément
  getUser(id: number): Observable<User> {
    return this.http.get<User>(`${this.apiUrl}/users/${id}`);
  }
  
  // POST - Créer
  createUser(user: User): Observable<User> {
    return this.http.post<User>(`${this.apiUrl}/users`, user);
  }
  
  // PUT - Modifier
  updateUser(id: number, user: User): Observable<User> {
    return this.http.put<User>(`${this.apiUrl}/users/${id}`, user);
  }
  
  // DELETE - Supprimer
  deleteUser(id: number): Observable<void> {
    return this.http.delete<void>(`${this.apiUrl}/users/${id}`);
  }
}
```

**3. Utiliser dans un component** :
```typescript
export class UserList implements OnDestroy {
  private userService = inject(UserService);
  users = signal<User[]>([]);
  private subscription: Subscription;
  
  ngOnInit() {
    this.subscription = this.userService.getUsers().subscribe({
      next: (users) => this.users.set(users),
      error: (error) => console.error('Erreur:', error)
    });
  }
  
  ngOnDestroy() {
    this.subscription?.unsubscribe();  // Important !
  }
}
```

**Points importants** :
- HttpClient retourne des **Observables**, pas des Promises
- Toujours **typer** les réponses : `<User[]>`
- Toujours **unsubscribe** (sauf si c'est une requête one-shot comme POST/PUT/DELETE)
- HttpClient complete automatiquement après la réponse

**Gestion des erreurs** :
```typescript
getUsers(): Observable<User[]> {
  return this.http.get<User[]>(`${this.apiUrl}/users`).pipe(
    catchError(error => {
      console.error('Erreur API:', error);
      return throwError(() => new Error('Échec du chargement'));
    })
  );
}
```

C'est vraiment simple et élégant ! Le typage TypeScript nous protège des erreurs."

---

### Q22: Qu'est-ce qu'un Interceptor et à quoi ça sert ?

**Réponse orale** :

"Un interceptor, c'est une fonction qui **intercepte** toutes les requêtes HTTP pour les modifier avant qu'elles partent, ou toutes les réponses avant qu'elles arrivent.

**Cas d'usage typiques** :
- Ajouter un token d'authentification à chaque requête
- Logger toutes les requêtes
- Gérer les erreurs globalement
- Ajouter un loading spinner

**Exemple concret : ajouter le token JWT** :
```typescript
export const authInterceptor: HttpInterceptorFn = (req, next) => {
  const token = localStorage.getItem('token');
  
  if (token) {
    // Cloner la requête et ajouter le header
    req = req.clone({
      setHeaders: {
        Authorization: `Bearer ${token}`
      }
    });
  }
  
  return next(req);  // Passer au prochain interceptor ou au serveur
};
```

**Enregistrer l'interceptor** :
```typescript
// app.config.ts
import { provideHttpClient, withInterceptors } from '@angular/common/http';

export const appConfig: ApplicationConfig = {
  providers: [
    provideHttpClient(
      withInterceptors([authInterceptor])
    )
  ]
};
```

**Interceptor de gestion d'erreurs globale** :
```typescript
export const errorInterceptor: HttpInterceptorFn = (req, next) => {
  return next(req).pipe(
    catchError((error: HttpErrorResponse) => {
      if (error.status === 401) {
        // Token expiré, rediriger vers login
        inject(Router).navigate(['/login']);
      } else if (error.status === 500) {
        // Erreur serveur
        console.error('Erreur serveur:', error);
      }
      return throwError(() => error);
    })
  );
};
```

**Interceptor de loading** :
```typescript
export const loadingInterceptor: HttpInterceptorFn = (req, next) => {
  const loadingService = inject(LoadingService);
  loadingService.show();
  
  return next(req).pipe(
    finalize(() => loadingService.hide())
  );
};
```

**Ordre d'exécution** :
Si j'ai plusieurs interceptors :
```typescript
withInterceptors([authInterceptor, loadingInterceptor, errorInterceptor])
```
Ils s'exécutent dans l'ordre pour la requête, et en ordre inverse pour la réponse.

Les interceptors sont super puissants ! C'est un des patterns les plus utiles d'Angular."

---

## RxJS et Observables

### Q23: Qu'est-ce qu'un Observable ?

**Réponse orale** :

"Un Observable, c'est un **flux de données** qui peut émettre plusieurs valeurs au fil du temps. C'est le cœur de la programmation réactive avec RxJS.

**Analogie simple** :
Imaginez une chaîne YouTube (Observable) avec des abonnés (Observers). À chaque nouvelle vidéo (émission), tous les abonnés sont notifiés.

**Différence avec Promise** :

| Promise | Observable |
|---------|-----------|
| Émet **1 seule** valeur | Émet **0, 1 ou plusieurs** valeurs |
| Eager (s'exécute immédiatement) | Lazy (ne s'exécute qu'au subscribe) |
| Non annulable | Annulable (unsubscribe) |
| .then() / .catch() | .subscribe() / .pipe() |

**Exemple concret** :
```typescript
// Une Promise
const promise = fetch('/api/users');
promise.then(users => console.log(users));  // 1 seule valeur

// Un Observable
const users$ = this.http.get<User[]>('/api/users');
users$.subscribe(users => console.log(users));  // 1 valeur AUSSI

// Mais Observable peut faire ça :
const clicks$ = fromEvent(button, 'click');
clicks$.subscribe(() => console.log('Cliqué!'));  // Plusieurs valeurs !
```

**Création d'Observables** :
```typescript
// Depuis un événement
const clicks$ = fromEvent(document, 'click');

// Depuis un tableau
const numbers$ = from([1, 2, 3, 4, 5]);

// Depuis un timer
const timer$ = interval(1000);  // Émet 0, 1, 2, 3... chaque seconde

// Custom
const custom$ = new Observable(observer => {
  observer.next('Valeur 1');
  observer.next('Valeur 2');
  observer.complete();
});
```

**Opérateurs RxJS** (le vrai pouvoir) :
```typescript
users$.pipe(
  map(users => users.filter(u => u.age > 18)),
  tap(users => console.log('Majeurs:', users)),
  catchError(error => of([]))  // Valeur par défaut en cas d'erreur
).subscribe(users => this.users.set(users));
```

**Règle d'or** : Toujours unsubscribe pour éviter les memory leaks !
```typescript
private subscription: Subscription;

ngOnInit() {
  this.subscription = data$.subscribe(...);
}

ngOnDestroy() {
  this.subscription.unsubscribe();
}
```

Les Observables sont essentiels dans Angular. Tout HttpClient, tout événement, tout routing... c'est basé sur RxJS !"

---

### Q24: Quelle est la différence entre map(), tap() et switchMap() ?

**Réponse orale** :

"Ce sont trois opérateurs RxJS très utilisés mais avec des rôles différents. Laissez-moi vous expliquer.

**map()** - Transforme les valeurs
```typescript
// Transforme chaque User en nom complet
users$.pipe(
  map(users => users.map(u => `${u.firstName} ${u.lastName}`))
).subscribe(names => console.log(names));

// Input: [{firstName: 'John', lastName: 'Doe'}]
// Output: ['John Doe']
```
C'est comme le `.map()` des tableaux, mais pour les Observables.

**tap()** - Effet de bord SANS modifier
```typescript
users$.pipe(
  tap(users => console.log('Reçu:', users)),  // Log pour debug
  tap(users => this.cache.set(users)),        // Sauvegarde dans cache
  map(users => users.length)
).subscribe(count => console.log(count));

// tap() ne change PAS la valeur, c'est juste pour des effets de bord
```
Utilisé pour : logging, mise à jour de cache, analytics...

**switchMap()** - Bascule vers un nouvel Observable
```typescript
// Recherche qui annule la requête précédente
searchTerm$.pipe(
  switchMap(term => this.http.get(`/api/search?q=${term}`))
).subscribe(results => console.log(results));

// Si je tape 'ang', puis 'angu', puis 'angular' rapidement :
// - switchMap annule les requêtes pour 'ang' et 'angu'
// - Seule la requête 'angular' aboutit
```

**Comparaison des opérateurs de mapping** :

| Opérateur | Quand utiliser |
|-----------|---------------|
| **switchMap** | Annuler l'Observable précédent (recherche, autocomplete) |
| **mergeMap** | Laisser tous les Observables se terminer (upload de fichiers) |
| **concatMap** | Attendre que chaque Observable se termine avant le suivant (séquentiel) |
| **exhaustMap** | Ignorer les nouvelles émissions si l'Observable précédent n'est pas fini (prévenir double-clic) |

**Exemple complet - Recherche** :
```typescript
searchControl.valueChanges.pipe(
  debounceTime(300),           // Attendre 300ms après la dernière frappe
  distinctUntilChanged(),      // Ignorer si même valeur
  tap(() => this.loading.set(true)),
  switchMap(term => 
    this.searchService.search(term).pipe(
      catchError(() => of([]))
    )
  ),
  tap(() => this.loading.set(false))
).subscribe(results => this.results.set(results));
```

**Règle simple** :
- Transformer une valeur → `map()`
- Faire quelque chose sans changer → `tap()`
- Passer à un autre Observable → `switchMap()` (le plus courant)

C'est vraiment la base de RxJS, il faut bien maîtriser ces opérateurs !"

---

## Performance et Optimisation

### Q25: Comment optimiser les performances d'une application Angular ?

**Réponse orale** :

"Excellente question ! La performance est cruciale pour l'expérience utilisateur. Voici les techniques principales que j'utilise.

**1. Change Detection Strategy** :
```typescript
@Component({
  changeDetection: ChangeDetectionStrategy.OnPush
})
export class MyComponent {
  // Angular ne vérifie que si les inputs changent
}
```
Par défaut, Angular vérifie TOUT à chaque événement. Avec OnPush, il vérifie uniquement quand :
- Un input change
- Un événement du component se déclenche
- Un Observable émet
C'est un gain **énorme** sur les grosses applications.

**2. TrackBy dans les ngFor** :
```html
<!-- ❌ Lent - recrée tout à chaque changement -->
@for (item of items; track item) { }

<!-- ✅ Rapide - ne recrée que ce qui change -->
@for (item of items; track item.id) { }
```

**3. Lazy Loading** :
```typescript
// Charger un module seulement quand on en a besoin
{
  path: 'admin',
  loadChildren: () => import('./admin/admin.routes')
}
```
Ça réduit le bundle initial de façon drastique.

**4. Utiliser les Signals** (nouveau) :
```typescript
// Au lieu de
items: Item[] = [];

// Utiliser
items = signal<Item[]>([]);
```
Les signals sont optimisés par nature et plus performants que les observables pour l'état local.

**5. Virtual Scrolling** pour les longues listes :
```html
<cdk-virtual-scroll-viewport itemSize="50" style="height: 400px;">
  @for (item of items; track item.id) {
    <div class="item">{{ item.name }}</div>
  }
</cdk-virtual-scroll-viewport>
```
Affiche seulement les éléments visibles au lieu de tout rendre.

**6. Unsubscribe proprement** :
```typescript
private subscriptions = new Subscription();

ngOnInit() {
  this.subscriptions.add(
    source1$.subscribe(...)
  );
  this.subscriptions.add(
    source2$.subscribe(...)
  );
}

ngOnDestroy() {
  this.subscriptions.unsubscribe();  // Tout en une fois
}
```

**7. Build en mode Production** :
```bash
ng build --configuration production
```
Active : minification, tree-shaking, AOT compilation, dead code elimination.

**8. Préchargement des modules** :
```typescript
provideRouter(routes, 
  withPreloading(PreloadAllModules)  // Précharge en arrière-plan
)
```

**Tools pour mesurer** :
- Angular DevTools (profiler)
- Chrome DevTools (Lighthouse)
- Webpack Bundle Analyzer

**Le plus important** : Mesurer AVANT d'optimiser. Ne pas optimiser prématurément !"

---

## Signals (Nouvelle API)

### Q26: Qu'est-ce qu'un Signal et pourquoi l'utiliser ?

**Réponse orale** :

"Les Signals, c'est la nouvelle API réactive d'Angular introduite en version 16, et c'est vraiment un game-changer !

**Définition simple** : Un Signal, c'est une valeur qui peut changer et qui notifie automatiquement quand elle change.

**Syntaxe de base** :
```typescript
// Créer un signal
count = signal(0);

// Lire la valeur (comme une fonction)
console.log(this.count());  // 0

// Modifier la valeur
this.count.set(5);           // Remplace par 5
this.count.update(n => n + 1);  // Incrémente de 1
```

**Dans le template** :
```html
<p>Compteur: {{ count() }}</p>
<button (click)="count.update(n => n + 1)">+1</button>
```

**Computed signals** - Valeurs dérivées :
```typescript
firstName = signal('John');
lastName = signal('Doe');

// Se met à jour automatiquement quand firstName ou lastName change
fullName = computed(() => 
  `${this.firstName()} ${this.lastName()}`
);
```

**Effects** - Réagir aux changements :
```typescript
constructor() {
  effect(() => {
    console.log(`Le compteur est à ${this.count()}`);
    // S'exécute à chaque changement de count
  });
}
```

**Pourquoi c'est mieux que les Observables pour l'état** :

| Aspect | Observables | Signals |
|--------|------------|---------|
| Subscribe | Obligatoire | Automatique |
| Unsubscribe | Manuel (risque leak) | Automatique |
| Syntaxe | `value$ \| async` | `value()` |
| Performance | Change detection globale | Granulaire |
| Courbe apprentissage | RxJS = complexe | Simple |

**Exemple complet** - Panier e-commerce :
```typescript
// État
items = signal<CartItem[]>([]);

// Valeurs calculées
itemCount = computed(() => this.items().length);
totalPrice = computed(() => 
  this.items().reduce((sum, item) => sum + item.price, 0)
);

// Actions
addItem(item: CartItem) {
  this.items.update(items => [...items, item]);
}

removeItem(id: number) {
  this.items.update(items => items.filter(i => i.id !== id));
}
```

**Template** :
```html
<div>
  Panier ({{ itemCount() }} articles) - {{ totalPrice() }}€
</div>
```

**Quand utiliser Signals vs Observables** :
- **Signals** : État local du component, valeurs qui changent
- **Observables** : Événements, requêtes HTTP, streams de données

L'avenir d'Angular, c'est les Signals. C'est plus simple, plus performant, et ça réduit le boilerplate. Je recommande fortement de les utiliser sur tout nouveau projet !"

---

## Angular Material

### Q27: Qu'est-ce qu'Angular Material ?

**Réponse orale** :

"Angular Material, c'est la bibliothèque officielle de composants UI pour Angular, basée sur Material Design de Google.

**Pourquoi l'utiliser** :
- Composants **prêts à l'emploi** : boutons, formulaires, tables, dialogs...
- **Design cohérent** avec Material Design
- **Accessibilité** intégrée (ARIA, navigation clavier)
- **Responsive** par défaut
- Bien **documenté** avec des exemples interactifs
- **Personnalisable** avec les thèmes

**Installation** :
```bash
ng add @angular/material
```
Cette commande :
- Installe les packages
- Configure le thème
- Importe les polices et icônes
- Configure les animations

**Utilisation basique** :
```typescript
import { MatButtonModule } from '@angular/material/button';
import { MatInputModule } from '@angular/material/input';

@Component({
  imports: [MatButtonModule, MatInputModule],
  template: `
    <button mat-raised-button color="primary">
      Cliquer
    </button>
    
    <mat-form-field>
      <mat-label>Email</mat-label>
      <input matInput type="email">
    </mat-form-field>
  `
})
```

**Composants les plus utilisés** :
- **MatButton** : boutons avec différentes variantes
- **MatFormField** : wrapper pour les inputs
- **MatDialog** : modales
- **MatTable** : tableaux de données
- **MatCard** : cartes avec header/footer
- **MatMenu** : menus déroulants
- **MatSnackbar** : notifications toast

**Exemple de Dialog** :
```typescript
export class MyComponent {
  dialog = inject(MatDialog);
  
  openDialog() {
    const dialogRef = this.dialog.open(ConfirmDialogComponent, {
      width: '400px',
      data: { message: 'Êtes-vous sûr ?' }
    });
    
    dialogRef.afterClosed().subscribe(result => {
      if (result) {
        // Confirmé
      }
    });
  }
}
```

**Theming** :
On peut personnaliser les couleurs facilement :
```scss
@use '@angular/material' as mat;

$my-theme: mat.define-light-theme((
  color: (
    primary: mat.define-palette(mat.$indigo-palette),
    accent: mat.define-palette(mat.$pink-palette),
  )
));

@include mat.all-component-themes($my-theme);
```

**Avantages pour un projet pro** :
- Développement **rapide** - pas besoin de tout créer
- **Cohérence** visuelle garantie
- Moins de bugs CSS à debugger
- Les clients connaissent déjà l'interface (Material Design omniprésent)

Sur mes projets professionnels, j'utilise quasi-systématiquement Material. Ça fait gagner énormément de temps !"

---

## Sécurité

### Q28: Comment sécuriser une application Angular ?

**Réponse orale** :

"La sécurité, c'est crucial. Voici les pratiques essentielles que j'applique.

**1. Protection XSS (Cross-Site Scripting)** :
Angular protège automatiquement contre le XSS :
```typescript
// ✅ Sûr - Angular échappe automatiquement
<div>{{ userInput }}</div>

// ⚠️ DANGER - bypassSecurityTrustHtml désactive la protection
<div [innerHTML]="userInput"></div>  // Éviter !
```
**Règle** : Ne JAMAIS utiliser `innerHTML` avec des données utilisateur non validées.

**2. Protection CSRF** :
```typescript
// Angular gère automatiquement les tokens CSRF
provideHttpClient(
  withXsrfConfiguration({
    cookieName: 'XSRF-TOKEN',
    headerName: 'X-XSRF-TOKEN'
  })
)
```

**3. Authentification avec JWT** :
```typescript
// Interceptor pour ajouter le token
export const authInterceptor: HttpInterceptorFn = (req, next) => {
  const token = localStorage.getItem('token');
  
  if (token) {
    req = req.clone({
      setHeaders: { Authorization: `Bearer ${token}` }
    });
  }
  
  return next(req);
};
```

**4. Guards pour protéger les routes** :
```typescript
export const authGuard: CanActivateFn = () => {
  const authService = inject(AuthService);
  
  if (authService.isLoggedIn()) {
    return true;
  }
  
  return inject(Router).createUrlTree(['/login']);
};

// Application
{
  path: 'admin',
  component: AdminComponent,
  canActivate: [authGuard]
}
```

**5. Validation côté client ET serveur** :
```typescript
// Ne JAMAIS faire confiance au client seul
userForm = this.formBuilder.group({
  email: ['', [Validators.required, Validators.email]],
  age: ['', [Validators.min(18), Validators.max(120)]]
});

// Mais toujours valider côté serveur aussi !
```

**6. HTTPS en production** :
```typescript
// environment.prod.ts
export const environment = {
  production: true,
  apiUrl: 'https://api.monapp.com'  // HTTPS obligatoire !
};
```

**7. Sanitization du HTML** :
```typescript
import { DomSanitizer } from '@angular/platform-browser';

constructor(private sanitizer: DomSanitizer) {}

getSafeHtml(html: string) {
  return this.sanitizer.sanitize(SecurityContext.HTML, html);
}
```

**8. Content Security Policy (CSP)** :
```html
<!-- index.html -->
<meta http-equiv="Content-Security-Policy" 
      content="default-src 'self'; script-src 'self'">
```

**9. Ne jamais stocker de données sensibles** :
```typescript
// ❌ JAMAIS ça
localStorage.setItem('password', password);
localStorage.setItem('creditCard', cardNumber);

// ✅ Seulement les tokens
localStorage.setItem('token', jwtToken);
```

**10. Limiter les permissions** :
```typescript
// Vérifier les rôles utilisateur
export const adminGuard: CanActivateFn = () => {
  const authService = inject(AuthService);
  
  if (authService.hasRole('admin')) {
    return true;
  }
  
  return inject(Router).createUrlTree(['/forbidden']);
};
```

**Outils de sécurité** :
- npm audit (vérifier les vulnérabilités)
- OWASP ZAP (tests de pénétration)
- Helmet.js (headers HTTP sécurisés, côté serveur)

**Règle d'or** : Ne JAMAIS faire confiance aux données côté client. Toujours valider côté serveur !"

---

## Testing

### Q29: Comment tester un component Angular ?

**Réponse orale** :

"Le testing dans Angular utilise Jasmine pour les tests et Karma comme test runner. Je vais vous montrer les bases.

**Structure d'un test** :
```typescript
import { ComponentFixture, TestBed } from '@angular/core/testing';
import { MyComponent } from './my-component';

describe('MyComponent', () => {
  let component: MyComponent;
  let fixture: ComponentFixture<MyComponent>;
  
  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [MyComponent]  // Standalone component
    }).compileComponents();
    
    fixture = TestBed.createComponent(MyComponent);
    component = fixture.componentInstance;
  });
  
  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
```

**Tester une propriété** :
```typescript
it('should have correct title', () => {
  component.title = 'Test Title';
  expect(component.title).toBe('Test Title');
});
```

**Tester le rendu** :
```typescript
it('should display the title', () => {
  component.title = 'Test Title';
  fixture.detectChanges();  // Déclenche le rendu
  
  const compiled = fixture.nativeElement;
  const h1 = compiled.querySelector('h1');
  
  expect(h1.textContent).toContain('Test Title');
});
```

**Tester un événement** :
```typescript
it('should increment counter on button click', () => {
  const button = fixture.nativeElement.querySelector('button');
  
  button.click();
  fixture.detectChanges();
  
  expect(component.count).toBe(1);
});
```

**Tester un service mocké** :
```typescript
describe('UserComponent', () => {
  let component: UserComponent;
  let userService: jasmine.SpyObj<UserService>;
  
  beforeEach(() => {
    // Créer un mock du service
    const userServiceSpy = jasmine.createSpyObj('UserService', ['getUsers']);
    
    TestBed.configureTestingModule({
      imports: [UserComponent],
      providers: [
        { provide: UserService, useValue: userServiceSpy }
      ]
    });
    
    userService = TestBed.inject(UserService) as jasmine.SpyObj<UserService>;
  });
  
  it('should load users on init', () => {
    const mockUsers = [{ id: 1, name: 'John' }];
    userService.getUsers.and.returnValue(of(mockUsers));
    
    component.ngOnInit();
    
    expect(userService.getUsers).toHaveBeenCalled();
    expect(component.users()).toEqual(mockUsers);
  });
});
```

**Tester un formulaire** :
```typescript
it('should validate email format', () => {
  const emailControl = component.loginForm.get('email');
  
  emailControl?.setValue('invalid-email');
  expect(emailControl?.hasError('email')).toBe(true);
  
  emailControl?.setValue('valid@email.com');
  expect(emailControl?.hasError('email')).toBe(false);
});
```

**Couverture de code** :
```bash
ng test --code-coverage
```
Génère un rapport dans `coverage/index.html`.

**Bonnes pratiques** :
- Un test = une fonctionnalité
- Tests indépendants (pas d'ordre requis)
- Noms explicites : 'should do X when Y'
- Mocker les dépendances
- Viser 80%+ de couverture sur le code critique

Les tests, c'est ce qui permet de refactorer en confiance. Sur un projet pro, c'est indispensable !"

---

## Questions Avancées

### Q30: Expliquez le Change Detection d'Angular

**Réponse orale** :

"Le Change Detection, c'est le mécanisme qui détecte quand les données changent et met à jour la vue. C'est le cœur d'Angular.

**Comment ça marche** :
Angular crée un arbre de composants. À chaque événement (clic, timer, requête HTTP...), Angular parcourt cet arbre pour vérifier si quelque chose a changé.

**Déclencheurs du Change Detection** :
1. Événements DOM (click, input...)
2. Timers (setTimeout, setInterval)
3. Requêtes HTTP
4. Modification manuelle avec ChangeDetectorRef

**Deux stratégies** :

**1. Default (par défaut)** :
```typescript
@Component({
  changeDetection: ChangeDetectionStrategy.Default
})
```
Angular vérifie TOUT le component et tous ses enfants à chaque événement.
- Avantage : Aucun risque de louper un changement
- Inconvénient : Lent sur les grosses apps

**2. OnPush (optimisé)** :
```typescript
@Component({
  changeDetection: ChangeDetectionStrategy.OnPush
})
```
Angular vérifie uniquement si :
- Un @Input() a changé (référence, pas valeur profonde)
- Un événement du component se déclenche
- Un Observable émet (avec async pipe)
- ChangeDetectorRef.markForCheck() est appelé

**Exemple de problème OnPush** :
```typescript
@Component({
  changeDetection: ChangeDetectionStrategy.OnPush
})
export class MyComponent {
  @Input() user: User;
  
  updateName() {
    this.user.name = 'New Name';  // ❌ Ne déclenche PAS le change detection
    // Car la référence de 'user' n'a pas changé
  }
}

// Solution 1: Nouvelle référence
updateName() {
  this.user = { ...this.user, name: 'New Name' };  // ✅
}

// Solution 2: Forcer la détection
private cdr = inject(ChangeDetectorRef);
updateName() {
  this.user.name = 'New Name';
  this.cdr.markForCheck();  // ✅
}
```

**Zone.js** :
Angular utilise Zone.js pour intercepter tous les événements async. C'est magique, mais ça a un coût en performance.

**Zoneless (nouveau)** :
Angular 18+ propose de se passer de Zone.js avec les Signals :
```typescript
// app.config.ts
export const appConfig: ApplicationConfig = {
  providers: [
    provideExperimentalZonelessChangeDetection()
  ]
};
```
Avec les Signals, Angular sait exactement ce qui a changé, pas besoin de tout vérifier.

**Bonnes pratiques** :
- Utiliser OnPush partout où possible
- Privilégier les Signals (zoneless)
- Utiliser le pipe async pour les Observables
- Éviter les mutations directes des objets

Le Change Detection, c'est complexe mais crucial pour la performance. OnPush + Signals, c'est le combo gagnant !"

---

### Q31: Qu'est-ce que le Lazy Loading et pourquoi l'utiliser ?

**Réponse orale** :

"Le Lazy Loading, c'est le chargement à la demande. Au lieu de charger toute l'application au démarrage, on charge les modules uniquement quand on en a besoin.

**Sans Lazy Loading** :
```
Bundle initial: 5 MB
- Module Admin: 2 MB
- Module User: 2 MB  
- Module Public: 1 MB
```
L'utilisateur télécharge 5 MB même s'il n'ira jamais dans l'admin.

**Avec Lazy Loading** :
```
Bundle initial: 1 MB (Module Public)
- Module Admin: 2 MB (chargé seulement si accès /admin)
- Module User: 2 MB (chargé seulement si accès /user)
```

**Configuration** :
```typescript
// app.routes.ts
export const routes: Routes = [
  {
    path: 'admin',
    loadChildren: () => import('./admin/admin.routes')
      .then(m => m.ADMIN_ROUTES)
  },
  {
    path: 'user',
    loadChildren: () => import('./user/user.routes')
      .then(m => m.USER_ROUTES)
  }
];
```

**Que se passe-t-il** :
1. L'utilisateur clique sur "/admin"
2. Angular télécharge le fichier admin.chunk.js
3. Angular affiche le module Admin

**Préchargement** :
On peut précharger en arrière-plan :
```typescript
provideRouter(routes, 
  withPreloading(PreloadAllModules)  // Précharge tout après le chargement initial
)
```

Ou créer une stratégie personnalisée :
```typescript
export class CustomPreloadStrategy implements PreloadingStrategy {
  preload(route: Route, load: () => Observable<any>): Observable<any> {
    // Précharger seulement les routes avec data.preload = true
    return route.data?.['preload'] ? load() : of(null);
  }
}

// Utilisation
{
  path: 'important',
  loadChildren: () => import('./important/routes'),
  data: { preload: true }
}
```

**Avantages** :
- **Démarrage rapide** - Bundle initial très léger
- **Bande passante économisée** - On ne charge que ce qu'on utilise
- **UX améliorée** - Moins d'attente au premier chargement

**Inconvénient** :
- Petit délai lors de la première navigation vers un module lazy

**Quand l'utiliser** :
- Modules admin (peu d'utilisateurs y accèdent)
- Fonctionnalités avancées
- Pages rarement visitées

**Quand NE PAS l'utiliser** :
- Pages critiques visitées immédiatement
- Très petits modules (overhead pas rentable)

Sur une grosse application, le Lazy Loading est **obligatoire**. Ça peut diviser le bundle initial par 3 ou 4 !"

---

### Q32: Qu'est-ce que le Server-Side Rendering (SSR) dans Angular ?

**Réponse orale** :

"Le SSR, c'est le fait de **rendre l'application côté serveur** avant de l'envoyer au client.

**Application classique (CSR - Client-Side Rendering)** :
1. Serveur envoie index.html presque vide
2. Téléchargement du JavaScript
3. Angular démarre
4. Appels API
5. Rendu de la page

**Avec SSR** :
1. Serveur exécute Angular
2. Serveur fait les appels API
3. Serveur génère le HTML complet
4. Client reçoit une page déjà rendue
5. Angular 'hydrate' la page (rend interactive)

**Installation** :
```bash
ng add @angular/ssr
```

**Configuration** (server.ts) :
```typescript
import { APP_BASE_HREF } from '@angular/common';
import { CommonEngine } from '@angular/ssr';

export function app(): express.Express {
  const server = express();
  const distFolder = join(process.cwd(), 'dist/browser');
  
  server.get('*', (req, res) => {
    const engine = new CommonEngine();
    
    engine.render({
      bootstrap: AppServerModule,
      documentFilePath: join(distFolder, 'index.html'),
      url: req.url,
      publicPath: distFolder,
      providers: [
        { provide: APP_BASE_HREF, useValue: req.baseUrl }
      ]
    }).then(html => res.send(html));
  });
  
  return server;
}
```

**Avantages** :
1. **SEO** - Google voit le contenu immédiatement
2. **Performance perçue** - L'utilisateur voit du contenu plus vite
3. **Social sharing** - Les previews Facebook/Twitter fonctionnent
4. **Accessibilité** - Fonctionne même si JS est désactivé

**Inconvénients** :
1. Complexité accrue
2. Besoin d'un serveur Node.js
3. Certaines bibliothèques incompatibles (accès au DOM)
4. Coût serveur plus élevé

**Pièges SSR** :
```typescript
// ❌ ERREUR SSR - window n'existe pas côté serveur
if (window.innerWidth > 768) { }

// ✅ Solution
import { isPlatformBrowser } from '@angular/common';

constructor(@Inject(PLATFORM_ID) private platformId: Object) {}

ngOnInit() {
  if (isPlatformBrowser(this.platformId)) {
    // Code qui accède au DOM
  }
}
```

**Quand utiliser SSR** :
- Sites vitrines / Marketing
- E-commerce
- Blogs / Actualités
- Toute app qui a besoin de SEO

**Quand NE PAS utiliser** :
- Apps derrière authentification (dashboard admin)
- Apps très interactives (jeux)
- Apps où SEO n'importe pas

**Alternative moderne : SSG (Static Site Generation)** :
Génère le HTML au build, pas à chaque requête :
```bash
ng build --configuration production
```

Le SSR est puissant mais ajoute de la complexité. À utiliser seulement si vraiment nécessaire !"

---

### Q33: Comment gérer le state management dans une grosse application ?

**Réponse orale** :

"Dans une petite application, les Services suffisent. Mais dans une grosse app, on a besoin d'une gestion d'état plus structurée.

**Solutions disponibles** :

**1. Services + Signals (recommandé pour débuter)** :
```typescript
@Injectable({ providedIn: 'root' })
export class CartStore {
  // État privé
  private _items = signal<CartItem[]>([]);
  private _loading = signal(false);
  
  // Lecture publique
  items = this._items.asReadonly();
  loading = this._loading.asReadonly();
  
  // Computed
  total = computed(() => 
    this._items().reduce((sum, item) => sum + item.price, 0)
  );
  
  // Actions
  addItem(item: CartItem) {
    this._items.update(items => [...items, item]);
  }
  
  async loadItems() {
    this._loading.set(true);
    const items = await this.api.getItems();
    this._items.set(items);
    this._loading.set(false);
  }
}
```
Simple, performant, intégré à Angular.

**2. NgRx (pour les très grosses apps)** :
```typescript
// État centralisé
export interface AppState {
  cart: CartState;
  user: UserState;
}

// Actions
export const addItem = createAction(
  '[Cart] Add Item',
  props<{ item: CartItem }>()
);

// Reducer
export const cartReducer = createReducer(
  initialState,
  on(addItem, (state, { item }) => ({
    ...state,
    items: [...state.items, item]
  }))
);

// Selector
export const selectCartTotal = createSelector(
  selectCartItems,
  (items) => items.reduce((sum, item) => sum + item.price, 0)
);

// Component
items$ = this.store.select(selectCartItems);

addToCart(item: CartItem) {
  this.store.dispatch(addItem({ item }));
}
```

**Comparaison** :

| Approche | Avantages | Inconvénients | Quand utiliser |
|----------|-----------|---------------|----------------|
| **Services + Signals** | Simple, peu de code | Pas de time-travel debugging | < 10 devs, app moyenne |
| **NgRx** | Debuggable, prévisible | Beaucoup de boilerplate | Grosse équipe, app complexe |
| **Akita** | Moins verbeux que NgRx | Moins populaire | Alternative à NgRx |
| **NGXS** | Plus simple que NgRx | Communauté plus petite | Alternative à NgRx |

**Patterns de state management** :

**Principe d'immutabilité** :
```typescript
// ❌ Mutation directe
items.push(newItem);

// ✅ Immutabilité
items = [...items, newItem];
```

**Single source of truth** :
```typescript
// ❌ État dupliqué
class ComponentA {
  users = [];
}
class ComponentB {
  users = [];  // Duplication !
}

// ✅ Une seule source
@Injectable({ providedIn: 'root' })
class UserStore {
  users = signal([]);
}
```

**Unidirectional data flow** :
```
Component → Action → Store → State → Component
```

**Mon conseil** :
- Projet < 50K lignes → Services + Signals
- Projet > 50K lignes + grosse équipe → NgRx
- Ne pas sur-engineer : commencer simple, complexifier si besoin

La règle d'or : **Keep it simple** ! NgRx apporte de la puissance mais aussi beaucoup de complexité. Il faut que le gain en vaille la peine."

---

### Questions à Poser au Recruteur

**Sur le projet** :
- "Quelle est la taille de la codebase Angular actuelle ?"
- "Quelle version d'Angular utilisez-vous ?"
- "Utilisez-vous TypeScript strict mode ?"
- "Avez-vous une stratégie de tests ? (unitaires, e2e)"
- "Comment gérez-vous le state management ?"

**Sur l'équipe** :
- "Combien de développeurs Angular dans l'équipe ?"
- "Faites-vous des code reviews ?"
- "Quelle est votre stack backend ?"

**Sur les pratiques** :
- "Utilisez-vous CI/CD ?"
- "Quelle est votre fréquence de déploiement ?"
- "Comment gérez-vous les montées de version Angular ?"

---

### Pièges à Éviter

1. **Ne pas dire "je ne sais pas"** → Mieux vaut être honnête
2. **Critiquer les anciennes versions** → Montrer qu'on comprend l'évolution
3. **Parler de technos qu'on ne maîtrise pas** → Rester sur ce qu'on connaît
4. **Être trop théorique** → Donner des exemples concrets
5. **Dénigrer React/Vue** → Chaque framework a ses forces

---

### Phrases Clés à Utiliser

- "Dans mon expérience..."
- "J'ai implémenté cette solution sur..."
- "La bonne pratique dans ce cas est..."
- "Ça dépend du contexte..."
- "Je ne suis pas sûr, mais je pense que..."
- "C'est une excellente question..."

---

## ✅ Checklist Avant l'Entretien

**Technique** :
- [ ] Revoir les concepts de base (Components, Services, Routing)
- [ ] Pratiquer sur un petit projet
- [ ] Relire la doc officielle Angular
- [ ] Préparer des exemples de code propre
- [ ] Réviser RxJS et les Observables
- [ ] Comprendre les Signals (nouvelle API)

**Projet** :
- [ ] Préparer 2-3 projets à présenter
- [ ] Savoir expliquer ses choix techniques
- [ ] Avoir des metrics (performance, tests...)
- [ ] Préparer des diagrammes si possible

**Soft Skills** :
- [ ] Préparer sa présentation (2 minutes)
- [ ] Lister ses points forts
- [ ] Préparer des questions pour le recruteur
- [ ] S'entraîner à expliquer clairement

---
 