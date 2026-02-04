# 🎯 PRÉPARATION ENTRETIEN ANGULAR - QUESTIONS/RÉPONSES

---

## 📚 PARTIE 1 : CONCEPTS FONDAMENTAUX ANGULAR

### ❓ Question 1 : Pouvez-vous m'expliquer ce qu'est Angular et en quoi il diffère d'AngularJS ?

**💡 Réponse :**
Angular est un framework TypeScript développé par Google pour créer des applications web single-page. Contrairement à AngularJS qui était basé sur JavaScript et utilisait le two-way data binding avec $scope, Angular utilise TypeScript, une architecture basée sur les composants, et un système de détection de changement plus performant avec Zone.js. Angular est aussi plus modulaire et offre de meilleures performances.

---

### ❓ Question 2 : Qu'est-ce qu'un composant dans Angular ?

**💡 Réponse :**
Un composant est la brique de base d'une application Angular. Il contrôle une portion de l'écran appelée vue. Il est composé de trois parties : une classe TypeScript décorée avec @Component qui contient la logique, un template HTML pour la vue, et des styles CSS. Le décorateur @Component définit le sélecteur, le template et les styles du composant.

---

### ❓ Question 3 : Pouvez-vous m'expliquer le cycle de vie d'un composant Angular ?

**💡 Réponse :**
Un composant Angular passe par plusieurs phases dans cet ordre :
- ngOnChanges : quand les inputs changent
- ngOnInit : initialisation, appelé une fois après le premier ngOnChanges
- ngDoCheck : détection personnalisée des changements
- ngAfterContentInit / ngAfterContentChecked : après projection du contenu
- ngAfterViewInit / ngAfterViewChecked : après initialisation de la vue
- ngOnDestroy : nettoyage avant destruction

Les plus utilisés sont ngOnInit pour l'initialisation et ngOnDestroy pour se désabonner des observables.

---

### ❓ Question 4 : Quelle est la différence entre un composant et une directive ?

**💡 Réponse :**
Les composants sont des directives avec un template. Les directives sont des classes qui ajoutent un comportement aux éléments du DOM. Il existe trois types :
- Les composants : directives avec template
- Les directives structurelles : modifient le DOM comme *ngIf et *ngFor
- Les directives d'attribut : modifient l'apparence ou le comportement comme ngClass ou ngStyle

---

### ❓ Question 5 : Qu'est-ce que le Change Detection dans Angular ?

**💡 Réponse :**
Le Change Detection est le mécanisme par lequel Angular détecte les changements dans les données et met à jour la vue. Angular utilise Zone.js pour intercepter les opérations asynchrones. Par défaut, il vérifie tout l'arbre de composants, mais on peut optimiser avec OnPush strategy qui ne vérifie que si les inputs changent ou si un événement se déclenche dans le composant.

---

### ❓ Question 6 : Qu'est-ce que le Shadow DOM et ViewEncapsulation ?

**💡 Réponse :**
ViewEncapsulation définit comment les styles d'un composant sont appliqués. Il y a trois modes : Emulated (par défaut) émule le Shadow DOM en ajoutant des attributs uniques, None applique les styles globalement, et ShadowDom utilise le vrai Shadow DOM du navigateur. Cela permet d'isoler les styles d'un composant pour éviter les conflits CSS.

---

## 🧩 PARTIE 2 : MODULES ET ARCHITECTURE

### ❓ Question 7 : Qu'est-ce qu'un NgModule ?

**💡 Réponse :**
Un NgModule est une classe décorée avec @NgModule qui organise l'application en blocs cohérents. Il déclare quels composants, directives et pipes lui appartiennent, importe d'autres modules dont il a besoin, exporte ce qui peut être utilisé par d'autres modules, et fournit des services. Chaque application a au minimum un root module, généralement AppModule.

---

### ❓ Question 8 : Quelle est la différence entre declarations, imports, providers et exports dans un module ?

**💡 Réponse :**
- **declarations** : contient les composants, directives et pipes qui appartiennent au module
- **imports** : liste les autres modules dont on a besoin
- **providers** : enregistre les services au niveau du module
- **exports** : rend disponibles certains éléments pour d'autres modules qui importent celui-ci

C'est important de bien comprendre cette distinction pour une architecture modulaire propre.

---

### ❓ Question 9 : Qu'est-ce que le lazy loading et pourquoi l'utiliser ?

**💡 Réponse :**
Le lazy loading permet de charger les modules à la demande plutôt qu'au démarrage de l'application. On l'utilise pour améliorer les performances initiales en réduisant la taille du bundle principal. On le configure dans le routing avec loadChildren. Par exemple, un module admin ne sera chargé que quand l'utilisateur accède à cette section, ce qui accélère le temps de chargement initial.

---

### ❓ Question 10 : Qu'est-ce qu'un Standalone Component (Angular 14+) ?

**💡 Réponse :**
Les Standalone Components sont une nouvelle approche qui permet de créer des composants sans NgModule. On ajoute standalone: true dans le décorateur @Component et on peut directement importer d'autres composants, directives ou pipes. Cela simplifie l'architecture et réduit le boilerplate, tout en gardant la compatibilité avec les modules classiques.

---

## 🔄 PARTIE 3 : DATA BINDING ET COMMUNICATION

### ❓ Question 11 : Quels sont les différents types de data binding dans Angular ?

**💡 Réponse :**
Il y a quatre types principaux :
- **Interpolation {{ }}** : afficher des données du composant dans le template
- **Property binding [property]** : lier une propriété du composant à une propriété du DOM
- **Event binding (event)** : écouter les événements du DOM
- **Two-way binding [(ngModel)]** : combine property et event binding pour une synchronisation bidirectionnelle

---

### ❓ Question 12 : Comment faire communiquer un composant parent avec un composant enfant ?

**💡 Réponse :**
Il y a plusieurs méthodes :
- **Parent → Enfant** : utiliser @Input() pour passer des données via le property binding
- **Enfant → Parent** : utiliser @Output() avec un EventEmitter pour émettre des événements
- **Communication complexe** : utiliser un service partagé avec des Observables pour une communication entre composants non liés

---

### ❓ Question 13 : Qu'est-ce que ViewChild et ContentChild ?

**💡 Réponse :**
- **@ViewChild** : permet d'accéder à un élément de la vue du composant (défini dans son propre template). Disponible après ngAfterViewInit.
- **@ContentChild** : permet d'accéder au contenu projeté dans le composant via ng-content. Disponible après ngAfterContentInit.

Ils sont utiles pour manipuler directement des éléments enfants ou du contenu projeté.

---

### ❓ Question 14 : Qu'est-ce que ng-content et la projection de contenu ?

**💡 Réponse :**
ng-content permet d'insérer du contenu externe dans un composant. C'est comme un slot où on peut placer du HTML depuis le composant parent. On peut utiliser des sélecteurs avec ng-content pour avoir plusieurs zones de projection. C'est très utile pour créer des composants réutilisables comme des cartes, modales ou layouts.

---

## 💉 PARTIE 4 : SERVICES ET DEPENDENCY INJECTION

### ❓ Question 15 : Qu'est-ce qu'un service dans Angular et pourquoi l'utiliser ?

**💡 Réponse :**
Un service est une classe avec un objectif précis, généralement marquée par @Injectable(). On l'utilise pour partager de la logique métier, des données ou des fonctionnalités entre plusieurs composants. Cela respecte le principe de séparation des responsabilités : les composants gèrent la vue, les services gèrent la logique métier et les données.

---

### ❓ Question 16 : Comment fonctionne l'injection de dépendances dans Angular ?

**💡 Réponse :**
Angular utilise un système d'injection de dépendances hiérarchique. Quand on décore un service avec @Injectable({providedIn: 'root'}), il est disponible partout en singleton. On peut aussi le fournir au niveau d'un module ou d'un composant. Angular crée et injecte automatiquement l'instance via le constructeur. C'est un pattern très puissant pour la testabilité et la modularité.

---

### ❓ Question 17 : Quelle est la différence entre providedIn: 'root' et déclarer un service dans providers ?

**💡 Réponse :**
- **providedIn: 'root'** : rend le service disponible globalement en singleton et permet le tree-shaking (suppression du code non utilisé)
- **Dans providers d'un module/composant** : crée une instance au niveau de ce module/composant
- **Dans un lazy-loaded module** : chaque instance du module aura sa propre instance du service

---

### ❓ Question 18 : Qu'est-ce que le tree-shaking et comment Angular l'utilise ?

**💡 Réponse :**
Le tree-shaking est un processus d'optimisation qui supprime le code mort (non utilisé) du bundle final. Angular l'utilise notamment avec providedIn: 'root' pour les services : si un service n'est jamais injecté, il ne sera pas inclus dans le bundle. Cela réduit significativement la taille de l'application en production.

---

## 🌊 PARTIE 5 : OBSERVABLES ET RxJS

### ❓ Question 19 : Qu'est-ce qu'un Observable et pourquoi Angular l'utilise ?

**💡 Réponse :**
Un Observable est un stream de données asynchrones qui peut émettre plusieurs valeurs dans le temps. Angular l'utilise massivement avec RxJS pour gérer les événements, les requêtes HTTP, les formulaires réactifs. Contrairement aux Promises qui gèrent une seule valeur, les Observables peuvent émettre plusieurs valeurs, être annulés, et offrent des opérateurs puissants pour transformer les données.

---

### ❓ Question 20 : Quelle est la différence entre subscribe et async pipe ?

**💡 Réponse :**
- **subscribe()** : permet de s'abonner manuellement à un Observable dans le composant, mais nécessite de se désabonner dans ngOnDestroy pour éviter les fuites mémoire
- **async pipe** : s'abonne automatiquement dans le template et se désabonne automatiquement à la destruction du composant

L'async pipe est préférable car il gère automatiquement le cycle de vie et rend le code plus propre.

---

### ❓ Question 21 : Pouvez-vous citer quelques opérateurs RxJS courants et leur utilité ?

**💡 Réponse :**
- **map** : transforme les valeurs émises
- **filter** : filtre selon une condition
- **switchMap** : annule la requête précédente et passe à la nouvelle (utile pour l'autocomplete)
- **mergeMap** : traite toutes les requêtes en parallèle
- **catchError** : gère les erreurs
- **debounceTime** : attend un délai avant d'émettre
- **distinctUntilChanged** : n'émet que si la valeur change
- **tap** : effectue des effets de bord sans modifier le stream

---

### ❓ Question 22 : Quelle est la différence entre Subject, BehaviorSubject et ReplaySubject ?

**💡 Réponse :**
- **Subject** : un Observable et Observer, émet des valeurs uniquement aux abonnés actuels
- **BehaviorSubject** : nécessite une valeur initiale, émet toujours la dernière valeur aux nouveaux abonnés
- **ReplaySubject** : stocke un certain nombre de valeurs et les rejoue aux nouveaux abonnés

BehaviorSubject est très utilisé pour gérer l'état dans les services.

---

### ❓ Question 23 : Comment gérer les fuites mémoire avec les Observables ?

**💡 Réponse :**
Plusieurs stratégies :
- Utiliser l'**async pipe** dans les templates
- Se **désabonner manuellement** dans ngOnDestroy
- Utiliser **takeUntil** avec un Subject qui émet lors de ngOnDestroy
- Utiliser **take(1)** pour les Observables qui ne doivent émettre qu'une fois
- Les Observables HTTP se complètent automatiquement après l'émission

---

## 🌐 PARTIE 6 : HTTP ET APIs

### ❓ Question 24 : Comment effectuer des requêtes HTTP dans Angular ?

**💡 Réponse :**
On utilise HttpClient importé depuis @angular/common/http. Il faut d'abord importer HttpClientModule dans le module. HttpClient retourne des Observables et offre des méthodes get(), post(), put(), delete(), patch(). On peut typer les réponses, ajouter des headers, gérer les erreurs avec catchError, et utiliser des intercepteurs pour manipuler les requêtes globalement.

---

### ❓ Question 25 : Qu'est-ce qu'un intercepteur HTTP et à quoi sert-il ?

**💡 Réponse :**
Un intercepteur HTTP implémente l'interface HttpInterceptor et permet d'intercepter toutes les requêtes et réponses HTTP. Cas d'usage :
- Ajouter automatiquement des tokens d'authentification aux headers
- Gérer les erreurs globalement
- Afficher un loader pendant les requêtes
- Logger les requêtes pour le debug
- Modifier les requêtes ou réponses

Il faut le déclarer dans les providers avec HTTP_INTERCEPTORS et multi: true.

---

### ❓ Question 26 : Comment gérez-vous les erreurs HTTP ?

**💡 Réponse :**
Je gère les erreurs à plusieurs niveaux :
- **Dans le service** : avec catchError de RxJS pour transformer l'erreur ou retourner une valeur par défaut
- **Dans un intercepteur HTTP** : pour gérer les erreurs globales comme les 401 (non authentifié) ou 500 (erreur serveur)
- **Dans le composant** : pour afficher des messages spécifiques à l'utilisateur

Je peux aussi créer un service d'erreur centralisé pour logger et notifier.

---

### ❓ Question 27 : Comment typer les réponses HTTP dans Angular ?

**💡 Réponse :**
On peut passer un type générique à HttpClient :
```typescript
interface User {
  id: number;
  name: string;
}

this.http.get<User[]>('/api/users')
```
Cela permet d'avoir l'autocomplétion TypeScript et de détecter les erreurs à la compilation. On peut aussi créer des interfaces pour les réponses API complexes avec des objets imbriqués.

---

## 🛣️ PARTIE 7 : ROUTING ET NAVIGATION

### ❓ Question 28 : Comment fonctionne le routing dans Angular ?

**💡 Réponse :**
Le routing dans Angular utilise RouterModule pour définir des routes qui mappent des URLs à des composants. On définit les routes dans un tableau avec path et component. Le router-outlet dans le template est remplacé par le composant de la route active. On peut naviguer par code avec Router.navigate() ou par template avec routerLink. Le routing supporte aussi les paramètres, les guards, et le lazy loading.

---

### ❓ Question 29 : Qu'est-ce qu'un guard et quels types existe-t-il ?

**💡 Réponse :**
Les guards protègent les routes et contrôlent la navigation :
- **CanActivate** : vérifie si on peut accéder à une route
- **CanActivateChild** : pour les routes enfants
- **CanDeactivate** : avant de quitter une route (utile pour les formulaires non sauvegardés)
- **Resolve** : pré-charge des données avant d'afficher le composant
- **CanLoad** : pour le lazy loading

Ils retournent un boolean, une Promise ou un Observable.

---

### ❓ Question 30 : Comment récupérer les paramètres d'une route ?

**💡 Réponse :**
On injecte ActivatedRoute et on utilise :
- **paramMap** : pour les paramètres de route (/user/:id)
- **queryParamMap** : pour les query params (/user?role=admin)
- **snapshot** : pour une lecture unique des paramètres
- **Observable** : pour réagir aux changements de paramètres sans recharger le composant

Exemple : `this.route.paramMap.subscribe(params => params.get('id'))`

---

### ❓ Question 31 : Qu'est-ce que le Route Resolver et quand l'utiliser ?

**💡 Réponse :**
Un Resolver implémente l'interface Resolve et permet de pré-charger des données avant qu'une route ne s'active. C'est utile pour éviter d'afficher un composant vide pendant le chargement des données. Le Resolver bloque la navigation jusqu'à ce que les données soient disponibles. On le configure dans la propriété resolve de la route.

---

## 📝 PARTIE 8 : FORMULAIRES

### ❓ Question 32 : Quelle est la différence entre Template-driven forms et Reactive forms ?

**💡 Réponse :**
- **Template-driven** : la logique est dans le template avec ngModel, simples et rapides pour des formulaires basiques, moins de contrôle
- **Reactive forms** : la logique est dans le composant avec FormGroup et FormControl, plus de contrôle, testables, meilleures pour les formulaires complexes avec validations dynamiques

Les Reactive forms sont généralement préférés pour les applications professionnelles.

---

### ❓ Question 33 : Comment créer un formulaire réactif dans Angular ?

**💡 Réponse :**
On importe ReactiveFormsModule, puis on crée un FormGroup dans le composant :
```typescript
this.form = this.fb.group({
  name: ['', Validators.required],
  email: ['', [Validators.required, Validators.email]]
});
```
On lie le formulaire avec [formGroup] dans le template et les champs avec formControlName. On peut accéder aux valeurs avec form.value et vérifier la validité avec form.valid.

---

### ❓ Question 34 : Comment créer un validateur personnalisé ?

**💡 Réponse :**
Un validateur est une fonction qui prend un FormControl et retourne null si valide ou un objet d'erreur sinon :
```typescript
function emailDomainValidator(control: AbstractControl) {
  const email = control.value;
  if (email && !email.endsWith('@company.com')) {
    return { emailDomain: true };
  }
  return null;
}
```
On peut créer des validateurs synchrones ou asynchrones (qui retournent un Observable).

---

### ❓ Question 35 : Qu'est-ce que FormArray et quand l'utiliser ?

**💡 Réponse :**
FormArray est une structure pour gérer un tableau de FormControl, FormGroup ou autres FormArray. C'est utile pour les formulaires dynamiques où on peut ajouter/supprimer des champs, comme une liste d'adresses ou de compétences. On peut accéder aux contrôles avec .at(index) et ajouter avec .push().

---

## 🎨 PARTIE 9 : PIPES ET DIRECTIVES PERSONNALISÉES

### ❓ Question 36 : Qu'est-ce qu'un pipe et comment en créer un personnalisé ?

**💡 Réponse :**
Un pipe transforme les données pour l'affichage dans le template. Angular fournit des pipes built-in (date, uppercase, currency). Pour créer un pipe personnalisé, on implémente l'interface PipeTransform avec la méthode transform(). On décore la classe avec @Pipe et on peut ajouter pure: false pour les pipes impurs qui se réévaluent à chaque cycle de détection.

---

### ❓ Question 37 : Quelle est la différence entre un pure pipe et un impure pipe ?

**💡 Réponse :**
- **Pure pipe (défaut)** : se réévalue uniquement si la référence de l'input change, très performant
- **Impure pipe** : se réévalue à chaque cycle de détection, peut impacter les performances

Les pipes impurs sont nécessaires pour les tableaux/objets mutables, mais à utiliser avec précaution.

---

### ❓ Question 38 : Comment créer une directive personnalisée ?

**💡 Réponse :**
On décore une classe avec @Directive et on peut :
- Injecter ElementRef pour accéder au DOM
- Utiliser @HostListener pour écouter les événements
- Utiliser @HostBinding pour lier des propriétés
- Utiliser @Input pour recevoir des paramètres

Exemple : une directive pour changer la couleur au survol.

---

## 🧪 PARTIE 10 : TESTS

### ❓ Question 39 : Comment tester un composant Angular ?

**💡 Réponse :**
On utilise Jasmine et Karma (ou Jest). On crée un TestBed pour configurer le module de test, compile le composant, crée une fixture, et récupère l'instance du composant. On peut tester :
- Les propriétés du composant
- Les méthodes
- Le rendu du template avec debugElement
- Les événements et interactions

detectChanges() déclenche la détection de changement pour mettre à jour la vue.

---

### ❓ Question 40 : Comment mocker un service dans les tests ?

**💡 Réponse :**
Plusieurs approches :
- Créer un mock object avec les méthodes nécessaires
- Utiliser Jasmine spies avec createSpyObj
- Utiliser le vrai service avec spyOn pour mocker certaines méthodes
- Fournir le mock dans TestBed.configureTestingModule avec providers

L'important est d'isoler le composant pour tester uniquement sa logique.

---

### ❓ Question 41 : Comment tester une requête HTTP ?

**💡 Réponse :**
On utilise HttpClientTestingModule et HttpTestingController :
- Importer HttpClientTestingModule dans TestBed
- Injecter HttpTestingController
- Effectuer la requête
- Utiliser expectOne() pour intercepter la requête
- Simuler la réponse avec flush()
- Vérifier qu'il n'y a pas de requêtes en attente avec verify()

---

## ⚡ PARTIE 11 : PERFORMANCE ET OPTIMISATION

### ❓ Question 42 : Comment optimiser les performances d'une application Angular ?

**💡 Réponse :**
Plusieurs techniques :
- **Lazy loading** pour charger les modules à la demande
- **OnPush change detection** pour limiter les vérifications
- **TrackBy** dans ngFor pour optimiser le rendu des listes
- **Async pipe** pour la gestion automatique des subscriptions
- **Preloading strategies** pour charger les modules en arrière-plan
- **Tree shaking** et build en mode production
- **Compression** et minification des assets
- **Service Workers** pour le cache

---

### ❓ Question 43 : Qu'est-ce que trackBy dans ngFor et pourquoi l'utiliser ?

**💡 Réponse :**
trackBy est une fonction qui aide Angular à identifier les éléments d'une liste lors des mises à jour. Sans trackBy, Angular recrée tous les éléments du DOM à chaque changement. Avec trackBy basé sur un ID unique, Angular ne recrée que les éléments modifiés. C'est crucial pour les performances avec de grandes listes.

```typescript
trackByFn(index, item) {
  return item.id;
}
```

---

### ❓ Question 44 : Qu'est-ce que la strategy OnPush et quand l'utiliser ?

**💡 Réponse :**
OnPush est une stratégie de détection de changement qui ne vérifie le composant que si :
- Un @Input change (nouvelle référence)
- Un événement se déclenche dans le composant ou ses enfants
- On utilise async pipe
- On appelle manuellement markForCheck()

C'est très performant pour les composants avec beaucoup d'enfants ou qui se mettent à jour rarement. À utiliser avec des données immutables.

---

### ❓ Question 45 : Comment implémenter le Virtual Scrolling ?

**💡 Réponse :**
Le virtual scrolling (CDK) affiche uniquement les éléments visibles d'une grande liste, ce qui améliore drastiquement les performances. On utilise ScrollingModule du @angular/cdk et cdk-virtual-scroll-viewport dans le template avec la directive *cdkVirtualFor. Angular calcule dynamiquement quels éléments afficher selon la position de scroll.

---

## 🔐 PARTIE 12 : SÉCURITÉ ET AUTHENTIFICATION

### ❓ Question 46 : Comment gérer l'authentification dans Angular ?

**💡 Réponse :**
Pattern commun :
- Créer un AuthService pour gérer login/logout et stocker le token
- Utiliser un intercepteur HTTP pour ajouter le token aux requêtes
- Créer un AuthGuard pour protéger les routes
- Stocker le token dans localStorage ou sessionStorage (ou mieux : en mémoire)
- Gérer le refresh token pour la persistance
- Implémenter la déconnexion automatique à l'expiration

---

### ❓ Question 47 : Qu'est-ce que la sanitization dans Angular ?

**💡 Réponse :**
Angular sanitize automatiquement les valeurs pour prévenir les attaques XSS. Il nettoie le HTML, les URLs, les styles pour retirer le code malveillant. Si on veut afficher du HTML non sanitized, on doit utiliser DomSanitizer avec bypassSecurityTrustHtml(), mais avec précaution car cela peut créer des vulnérabilités.

---

### ❓ Question 48 : Comment protéger une application Angular contre les attaques courantes ?

**💡 Réponse :**
- **XSS** : utiliser la sanitization d'Angular, éviter innerHTML
- **CSRF** : utiliser des tokens CSRF, Angular le gère avec HttpClient
- **Injection** : valider et sanitizer les entrées utilisateur
- **Sécuriser les tokens** : HTTPS obligatoire, ne pas stocker d'infos sensibles en clair
- **Headers de sécurité** : CSP, X-Frame-Options (configurés côté serveur)
- **Validation côté serveur** : ne jamais faire confiance au client

---

## 🏗️ PARTIE 13 : ARCHITECTURE ET BONNES PRATIQUES

### ❓ Question 49 : Quelles sont les bonnes pratiques d'architecture Angular ?

**💡 Réponse :**
- **Modularité** : découper en feature modules
- **Lazy loading** : pour les modules de fonctionnalités
- **Smart/Dumb components** : séparer composants conteneurs et présentation
- **Services** : centraliser la logique métier
- **State management** : pour les apps complexes (NgRx, Akita)
- **Convention de nommage** : cohérente et claire
- **Shared module** : pour les composants réutilisables
- **Core module** : pour les singletons (services globaux)

---

### ❓ Question 50 : Qu'est-ce que le pattern Smart/Dumb components ?

**💡 Réponse :**
- **Smart components (containers)** : gèrent la logique, les services, les subscriptions, passent les données aux dumb components
- **Dumb components (presentational)** : reçoivent des données via @Input, émettent des événements via @Output, pas de logique métier, purement présentationnels, réutilisables

Cette séparation améliore la réutilisabilité et la testabilité.

---

### ❓ Question 51 : Qu'est-ce que NgRx et quand l'utiliser ?

**💡 Réponse :**
NgRx est une bibliothèque de state management basée sur Redux. Elle utilise le pattern Flux avec Store, Actions, Reducers, Effects et Selectors. À utiliser quand :
- L'application est complexe avec beaucoup d'état partagé
- Plusieurs composants accèdent aux mêmes données
- On veut un historique des changements (time-travel debugging)
- L'état doit être prévisible et testable

Pour les petites apps, un simple service avec BehaviorSubject suffit.

---

### ❓ Question 52 : Comment gérer l'état global sans NgRx ?

**💡 Réponse :**
On peut créer un service avec BehaviorSubject :
```typescript
@Injectable({ providedIn: 'root' })
export class StateService {
  private state = new BehaviorSubject<State>(initialState);
  state$ = this.state.asObservable();
  
  updateState(newState: Partial<State>) {
    this.state.next({...this.state.value, ...newState});
  }
}
```
Les composants s'abonnent au state$ et appellent updateState pour modifier l'état.

---

## 🔧 PARTIE 14 : OUTILS ET DÉVELOPPEMENT

### ❓ Question 53 : Qu'est-ce qu'Angular CLI et quelles sont ses commandes principales ?

**💡 Réponse :**
Angular CLI est un outil en ligne de commande pour Angular. Commandes principales :
- **ng new** : créer un nouveau projet
- **ng serve** : lancer le serveur de développement
- **ng generate** (g) : générer des composants, services, modules, etc.
- **ng build** : compiler l'application
- **ng test** : lancer les tests unitaires
- **ng e2e** : tests end-to-end
- **ng lint** : vérifier le code

---

### ❓ Question 54 : Qu'est-ce que le fichier angular.json ?

**💡 Réponse :**
angular.json est le fichier de configuration du projet Angular. Il contient :
- La structure du workspace
- Les configurations de build (dev, prod)
- Les chemins des assets et styles
- Les options du serveur de développement
- Les configurations des tests
- Les budgets de taille du bundle

On peut y définir plusieurs configurations (environments) et les utiliser avec --configuration.

---

### ❓ Question 55 : Comment gérer les environnements (dev, prod, staging) ?

**💡 Réponse :**
Angular utilise des fichiers environment.ts et environment.prod.ts. On y stocke les configurations spécifiques (URLs d'API, clés, flags). Dans angular.json, on configure les fileReplacements pour remplacer le bon fichier selon l'environnement. On build avec --configuration=production. On peut créer autant d'environnements que nécessaire.

---

### ❓ Question 56 : Qu'est-ce que les Source Maps et pourquoi les utiliser ?

**💡 Réponse :**
Les source maps permettent de mapper le code compilé/minifié vers le code source original. En développement, elles permettent de debugger le code TypeScript dans le navigateur plutôt que le JavaScript compilé. En production, on peut les désactiver pour la sécurité ou les héberger séparément pour le debugging.

---

## 🌍 PARTIE 15 : INTERNATIONALISATION ET ACCESSIBILITÉ

### ❓ Question 57 : Comment implémenter l'internationalisation (i18n) dans Angular ?

**💡 Réponse :**
Angular offre un système i18n natif :
- Marquer les textes avec l'attribut i18n
- Extraire les traductions avec ng extract-i18n
- Créer des fichiers de traduction (XLIFF, JSON)
- Builder l'app pour chaque langue
- Ou utiliser ngx-translate pour un changement dynamique de langue sans rebuild

---

### ❓ Question 58 : Quelles sont les bonnes pratiques d'accessibilité dans Angular ?

**💡 Réponse :**
- Utiliser les **attributs ARIA** appropriés
- Assurer la **navigation au clavier** (tabindex, focus)
- Utiliser des **labels** pour les formulaires
- Avoir un bon **contraste** de couleurs
- Tester avec des **lecteurs d'écran**
- Utiliser **Angular CDK a11y** pour les helpers d'accessibilité
- Respecter la **structure sémantique** HTML
- Fournir des **alternatives textuelles** pour les médias

---

## 🔄 PARTIE 16 : PROGRESSIVE WEB APP (PWA)

### ❓ Question 59 : Comment transformer une application Angular en PWA ?

**💡 Réponse :**
On utilise le package @angular/pwa :
```
ng add @angular/pwa
```
Cela ajoute :
- Un manifest.json pour les métadonnées de l'app
- Des icônes
- Un service worker pour le cache et le mode offline
- La configuration ngsw-config.json

On peut configurer les stratégies de cache et les ressources à mettre en cache.

---

### ❓ Question 60 : Qu'est-ce qu'un Service Worker et comment Angular l'utilise ?

**💡 Réponse :**
Un Service Worker est un script qui s'exécute en arrière-plan, séparé de la page web. Angular l'utilise via @angular/service-worker pour :
- Mettre en cache les assets pour le mode offline
- Précharger les ressources
- Gérer les notifications push
- Synchroniser en arrière-plan

Le SwUpdate service permet de gérer les mises à jour de l'application.

---

## 📱 PARTIE 17 : ANIMATIONS

### ❓ Question 61 : Comment créer des animations dans Angular ?

**💡 Réponse :**
On utilise @angular/animations avec des trigger, state, style, transition et animate :
```typescript
trigger('fadeIn', [
  transition(':enter', [
    style({ opacity: 0 }),
    animate('300ms', style({ opacity: 1 }))
  ])
])
```
On attache le trigger au template avec [@fadeIn]. Angular gère automatiquement les transitions.

---

## 🔌 PARTIE 18 : INTÉGRATION BACKEND (Questions spécifiques)

### ❓ Question 62 : Comment structurez-vous la communication entre Angular et le backend ?

**💡 Réponse :**
Je crée une architecture en couches :
- **Services API** : un service par resource (UserService, ProductService) pour encapsuler les appels HTTP
- **Models/Interfaces** : pour typer les données échangées
- **Intercepteurs** : pour la gestion globale (auth, errors, loading)
- **Environment** : pour les URLs du backend selon l'environnement
- **DTOs** : parfois différents des models frontend pour mapper les données

---

### ❓ Question 63 : Comment gérer le CORS entre Angular et votre API backend ?

**💡 Réponse :**
CORS est géré côté backend, mais du côté Angular :
- En développement : utiliser le proxy de Angular CLI (proxy.conf.json) pour éviter les problèmes CORS
- En production : le backend doit configurer les headers CORS appropriés
- S'assurer que les credentials sont envoyés si nécessaire avec withCredentials: true

---

### ❓ Question 64 : Comment gérez-vous la pagination côté frontend ?

**💡 Réponse :**
Plusieurs approches :
- **Pagination classique** : boutons précédent/suivant, l'API retourne page et totalPages
- **Infinite scroll** : charger plus de données au scroll avec Intersection Observer
- **Virtual scrolling** : pour de grandes listes avec CDK

Je stocke les paramètres de pagination dans le service et les passe aux requêtes HTTP.

---

### ❓ Question 65 : Comment gérer le cache des données venant du backend ?

**💡 Réponse :**
Plusieurs stratégies :
- **Cache en mémoire** : avec un BehaviorSubject dans le service, vérifier si données déjà chargées
- **HTTP Cache** : utiliser un intercepteur pour mettre en cache les réponses GET
- **State management** : NgRx avec @ngrx/entity pour gérer le cache et les entités
- **Service Worker** : pour le cache au niveau réseau (PWA)

Le choix dépend des besoins de fraîcheur des données.

---

## 🎓 PARTIE 19 : QUESTIONS AVANCÉES

### ❓ Question 66 : Qu'est-ce que le Ahead-of-Time (AOT) compilation ?

**💡 Réponse :**
AOT compile les templates Angular en code JavaScript lors du build, avant le déploiement. Avantages :
- Chargement plus rapide (pas de compilation dans le navigateur)
- Détection des erreurs de template à la compilation
- Meilleure sécurité (templates déjà compilés)
- Bundle plus petit

C'est le mode par défaut en production. Le contraire est JIT (Just-in-Time) utilisé en dev.

---

### ❓ Question 67 : Qu'est-ce que l'Ivy renderer ?

**💡 Réponse :**
Ivy est le moteur de rendu et de compilation d'Angular (depuis Angular 9). Avantages :
- Bundles plus petits grâce au tree-shaking amélioré
- Compilation plus rapide
- Meilleur debugging (ng.probe, ng.applyChanges)
- Meilleure compatibilité
- Prépare le terrain pour des fonctionnalités futures

Il a remplacé View Engine.

---

### ❓ Question 68 : Comment fonctionne Zone.js et peut-on s'en passer ?

**💡 Réponse :**
Zone.js intercepte les opérations asynchrones (setTimeout, events, HTTP) pour déclencher la détection de changement. On peut s'en passer en mode "zoneless" :
- Déclencher manuellement la détection avec ApplicationRef.tick()
- Utiliser OnPush partout
- Utiliser ChangeDetectorRef.markForCheck()

C'est plus performant mais nécessite plus de gestion manuelle.

---

### ❓ Question 69 : Qu'est-ce que le Differential Loading ?

**💡 Réponse :**
Le differential loading génère deux bundles : un moderne (ES2015+) pour les navigateurs récents et un bundle ES5 pour les anciens navigateurs. Les navigateurs modernes téléchargent le bundle plus petit et plus performant. Angular CLI gère automatiquement cela avec le fichier browserslist.

---

### ❓ Question 70 : Comment débugger une application Angular ?

**💡 Réponse :**
Plusieurs outils :
- **Chrome DevTools** : points d'arrêt, console, network
- **Angular DevTools** : extension pour inspecter les composants et leur état
- **Augury** : extension pour visualiser l'arbre des composants (moins maintenue)
- **Source maps** : pour debugger le TypeScript
- **console.log** et breakpoints dans le code
- **RxJS debugging** : avec tap() operator
- **ng.probe()** en développement pour inspecter les composants

---

## 💼 PARTIE 20 : QUESTIONS PRATIQUES ET EXPÉRIENCE

### ❓ Question 71 : Parlez-moi d'un problème complexe que vous avez résolu avec Angular

**💡 Réponse :**
Préparez un exemple concret de votre expérience montrant :
- Le contexte et le problème
- Les défis techniques rencontrés
- La solution mise en place
- Les technologies/patterns Angular utilisés
- Le résultat et l'impact

Exemple : optimisation d'une grosse liste avec virtual scrolling et OnPush, gestion d'un formulaire multi-étapes complexe, mise en place d'une architecture scalable, etc.

---

### ❓ Question 72 : Comment travaillez-vous avec une équipe backend sur un projet Angular ?

**💡 Réponse :**
- **Communication** : sync régulières pour aligner les besoins
- **Documentation API** : Swagger/OpenAPI pour la documentation des endpoints
- **Contrat d'interface** : définir ensemble les formats de données (DTOs)
- **Mocks** : utiliser des intercepteurs ou services mockés pendant le développement backend
- **Versionning API** : gérer les versions pour éviter les breaking changes
- **Code review** croisée si pertinent
- **Tests d'intégration** : tester l'intégration complète

---

### ❓ Question 73 : Comment gérez-vous les montées de version d'Angular ?

**💡 Réponse :**
- Suivre le **guide de migration** officiel sur update.angular.io
- Lire le **changelog** et les breaking changes
- Tester dans une **branche dédiée** avant
- Utiliser **ng update** pour automatiser certaines migrations
- Mettre à jour les **dépendances** compatibles
- Lancer tous les **tests** pour détecter les régressions
- Faire des montées de version **régulières** pour éviter les gros gaps

---

### ❓ Question 74 : Quelles sont les métriques de performance que vous surveillez ?

**💡 Réponse :**
- **First Contentful Paint (FCP)** : temps avant le premier contenu
- **Largest Contentful Paint (LCP)** : temps avant le plus gros élément
- **Time to Interactive (TTI)** : temps avant interaction
- **Bundle size** : taille des fichiers JS/CSS
- **Lighthouse score** : score global de performance
- **Memory leaks** : avec Chrome DevTools
- **API response time** : temps des requêtes HTTP

---

### ❓ Question 75 : Comment assurez-vous la qualité du code dans vos projets Angular ?

**💡 Réponse :**
- **Linting** : ESLint avec des règles Angular
- **Prettier** : formatage automatique du code
- **Tests unitaires** : avec Jasmine/Karma ou Jest
- **Tests e2e** : avec Cypress ou Playwright
- **Code review** : revue par les pairs
- **CI/CD** : pipeline automatisé pour tests et build
- **SonarQube** : analyse de code et détection de bugs
- **Documentation** : JSDoc et README à jour
- **Conventions** : guide de style d'équipe

---

## 🚀 BONUS : QUESTIONS PIÈGES

### ❓ Question 76 : Pourquoi ne devrait-on pas manipuler directement le DOM dans Angular ?

**💡 Réponse :**
Angular gère le DOM via son système de détection de changement. Manipuler directement le DOM :
- Casse la synchronisation entre modèle et vue
- Peut causer des problèmes avec le change detection
- Rend le code non testable
- Ne fonctionne pas en server-side rendering

Si vraiment nécessaire, utiliser ElementRef, Renderer2 ou ViewChild, mais c'est rarement la bonne approche.

---

### ❓ Question 77 : Peut-on utiliser jQuery avec Angular ?

**💡 Réponse :**
Techniquement oui, mais c'est **fortement déconseillé** car :
- Angular et jQuery ont des philosophies opposées
- Conflit avec la détection de changement
- Manipulation du DOM qui contourne Angular
- Il existe toujours une alternative Angular native

Si un plugin legacy est absolument nécessaire, l'encapsuler dans une directive.

---

### ❓ Question 78 : Quelle est la différence entre constructor et ngOnInit ?

**💡 Réponse :**
- **constructor** : méthode TypeScript standard, appelée lors de l'instanciation de la classe, utilisé pour l'injection de dépendances, les @Input ne sont pas encore disponibles
- **ngOnInit** : hook Angular appelé après l'initialisation des inputs, c'est là qu'on met la logique d'initialisation du composant

Bonne pratique : constructor léger (juste DI), logique dans ngOnInit.

---

### ❓ Question 79 : Que se passe-t-il si on oublie de se désabonner d'un Observable ?

**💡 Réponse :**
Cela crée une **fuite mémoire** (memory leak). Le composant est détruit mais la subscription continue d'exister et écouter les événements. Sur une SPA avec beaucoup de navigation, cela peut dégrader significativement les performances et consommer de plus en plus de mémoire. D'où l'importance de toujours se désabonner ou utiliser l'async pipe.

---

### ❓ Question 80 : Angular est-il un framework ou une bibliothèque ?

**💡 Réponse :**
Angular est un **framework** complet. La différence :
- **Framework** : impose une structure et une architecture (inversion de contrôle)
- **Bibliothèque** : offre des outils qu'on appelle quand on veut (comme React)

Angular fournit tout : routing, forms, HTTP, animations, CLI, etc. C'est une solution "batteries included" complète pour construire des applications.

---

## ✅ CONSEILS FINAUX POUR L'ENTRETIEN

**🎯 Stratégie de réponse :**
1. Commencez par une définition claire et concise
2. Donnez des exemples concrets si possible
3. Mentionnez les cas d'usage appropriés
4. N'hésitez pas à faire des liens avec votre expérience
5. Si vous ne savez pas, soyez honnête mais montrez votre capacité à raisonner

**💪 Points à mettre en avant :**
- Votre expérience pratique avec Angular
- Votre compréhension de l'architecture
- Votre souci des performances et de la qualité
- Votre capacité à travailler avec une équipe backend
- Votre veille technologique

**⚠️ À éviter :**
- Réponses trop théoriques sans exemples
- Dire "je ne sais pas" sans proposer de réflexion
- Critiquer Angular sans arguments
- Parler de technologies que vous ne maîtrisez pas

---

**Bonne chance pour votre entretien ! 🍀**
