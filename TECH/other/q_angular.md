# 🅰️ Questions / Réponses Angular — Préparation Entretien

---

## 🔷 1. Les Bases d'Angular

---

**Q1 — C'est quoi Angular et en quoi il est différent d'AngularJS ?**

Angular c'est un framework front-end développé par Google, basé sur TypeScript. Il est utilisé pour construire des Single Page Applications (SPA). AngularJS c'était la version 1, elle était en JavaScript pur et avait une architecture différente avec le two-way data binding basé sur les scopes. Angular (version 2 et plus) a été complètement réécrit, il utilise TypeScript, les components, les modules, et une injection de dépendances beaucoup plus propre. Les deux sont très différents, ce n'est pas juste une mise à jour.

---

**Q2 — C'est quoi un Component dans Angular ?**

Un component c'est la brique de base d'une application Angular. Il regroupe trois choses : un template HTML, du style CSS, et une classe TypeScript avec la logique. On le déclare avec le décorateur `@Component`. Chaque component est responsable d'une portion de l'interface, par exemple un formulaire, une liste, ou un bouton. L'application est une arborescence de components.

---

**Q3 — C'est quoi un Module dans Angular ?**

Un module c'est un conteneur qui regroupe des components, des services, des directives, et des pipes. On le déclare avec le décorateur `@NgModule`. Il y a toujours un module racine qui s'appelle `AppModule`. On peut créer des feature modules pour organiser son code par fonctionnalité, ce qui améliore la lisibilité et permet le lazy loading.

---

**Q4 — Qu'est-ce que le décorateur `@NgModule` contient ?**

Il contient plusieurs propriétés clés :
- `declarations` : les components, directives et pipes qui appartiennent à ce module
- `imports` : les autres modules dont on a besoin
- `exports` : ce qu'on veut rendre disponible aux autres modules
- `providers` : les services qu'on fournit au niveau du module
- `bootstrap` : le component racine, uniquement dans l'AppModule

---

**Q5 — C'est quoi le fichier `main.ts` ?**

C'est le point d'entrée de l'application. Il bootstrap l'AppModule, c'est-à-dire qu'il dit à Angular de démarrer l'application à partir de ce module. Sans ce fichier, l'application ne démarre pas.

---

**Q6 — C'est quoi le selector dans un component ?**

Le selector c'est le nom HTML qu'on utilise pour insérer un component dans un template. Par exemple si le selector est `app-header`, on l'utilise comme `<app-header></app-header>` dans le HTML. C'est Angular qui fait le lien entre ce tag HTML et le component correspondant.

---

## 🔷 2. Data Binding

---

**Q7 — Quels sont les types de data binding dans Angular ?**

Il y en a quatre :
1. **Interpolation** `{{ valeur }}` : affiche une donnée du component dans le HTML
2. **Property binding** `[propriete]="valeur"` : lie une propriété HTML à une variable du component
3. **Event binding** `(evenement)="methode()"` : écoute un événement HTML et appelle une méthode
4. **Two-way binding** `[(ngModel)]="valeur"` : c'est la combinaison des deux précédents, la vue et le model se synchronisent dans les deux sens

---

**Q8 — Comment fonctionne le two-way data binding ?**

C'est la syntaxe "banana in a box" `[()]`. Elle combine property binding et event binding. Quand l'utilisateur modifie une valeur dans l'input, ça met à jour la variable dans le component. Et si la variable change dans le code, ça met à jour l'affichage automatiquement. Pour l'utiliser avec les formulaires, on a besoin du `FormsModule`.

---

**Q9 — C'est quoi la différence entre `[src]` et `src` dans un template ?**

Sans les crochets, `src="valeur"` c'est une valeur statique, Angular ne va pas l'évaluer. Avec les crochets, `[src]="maVariable"` c'est du property binding, Angular va évaluer l'expression et lier la valeur dynamiquement.

---

## 🔷 3. Directives

---

**Q10 — C'est quoi une directive dans Angular ?**

Une directive c'est une instruction qu'on ajoute dans le HTML pour modifier le comportement ou l'apparence d'un élément. Il y a trois types :
- Les **component directives** : les components eux-mêmes sont des directives avec un template
- Les **structural directives** : elles modifient la structure du DOM (ajout/suppression d'éléments)
- Les **attribute directives** : elles modifient l'apparence ou le comportement d'un élément existant

---

**Q11 — Quelles sont les directives structurelles les plus utilisées ?**

- `*ngIf` : affiche ou cache un élément selon une condition
- `*ngFor` : répète un élément pour chaque item d'une liste
- `*ngSwitch` : affiche un élément parmi plusieurs selon une valeur

L'étoile `*` est une notation abrégée. Angular la transforme en `<ng-template>` en interne.

---

**Q12 — C'est quoi la différence entre `*ngIf` et `[hidden]` ?**

`*ngIf` supprime complètement l'élément du DOM quand la condition est fausse. `[hidden]` lui ajoute juste un `display: none`, l'élément reste dans le DOM. Si l'élément contient des ressources ou des traitements lourds, `*ngIf` est plus performant car il ne l'initialise même pas.

---

**Q13 — C'est quoi `ngClass` et `ngStyle` ?**

Ce sont des attribute directives. `ngClass` permet d'ajouter ou retirer des classes CSS dynamiquement selon des conditions. `ngStyle` permet de modifier directement des styles inline selon des valeurs dynamiques.

---

## 🔷 4. Services et Injection de Dépendances

---

**Q14 — C'est quoi un service dans Angular ?**

Un service c'est une classe TypeScript qui contient de la logique réutilisable, comme des appels HTTP, de la manipulation de données, ou du partage d'état entre components. On le décore avec `@Injectable`. L'idée c'est de sortir la logique métier des components pour les garder légers, et de pouvoir réutiliser cette logique partout dans l'app.

---

**Q15 — C'est quoi l'injection de dépendances (DI) ?**

L'injection de dépendances c'est un design pattern où les dépendances (services, etc.) sont fournies à une classe de l'extérieur plutôt qu'instanciées à l'intérieur. Dans Angular, quand on déclare un service dans le constructeur d'un component, Angular s'occupe de créer et fournir l'instance automatiquement. Ça rend le code plus testable et plus maintenable.

---

**Q16 — C'est quoi `providedIn: 'root'` ?**

C'est une façon de déclarer un service au niveau de toute l'application, directement dans le décorateur `@Injectable`. Ça veut dire qu'Angular crée une seule instance du service partagée par toute l'app (un singleton). C'est la façon recommandée depuis Angular 6, on n'a plus besoin de l'ajouter dans les `providers` du module.

---

**Q17 — Quelle est la différence entre fournir un service dans `root` vs dans un module vs dans un component ?**

- `root` : une seule instance pour toute l'application
- Dans un module : une instance partagée pour tous les components de ce module
- Dans un component : une instance créée pour ce component spécifiquement et ses enfants. Chaque fois que le component est créé, une nouvelle instance du service est créée.

---

## 🔷 5. Communication entre Components

---

**Q18 — Comment passer des données d'un parent à un enfant ?**

On utilise `@Input()`. Dans le component enfant, on déclare une propriété avec le décorateur `@Input()`. Dans le template du parent, on utilise le property binding pour passer la valeur : `[maProprietee]="valeur"`.

---

**Q19 — Comment envoyer un événement d'un enfant vers un parent ?**

On utilise `@Output()` avec un `EventEmitter`. Dans l'enfant, on crée un `EventEmitter` décoré avec `@Output()`, puis on appelle `.emit(valeur)` pour envoyer un événement. Dans le template du parent, on écoute cet événement avec l'event binding : `(monEvenement)="maMethode($event)"`.

---

**Q20 — Comment partager des données entre des components qui ne sont pas parent/enfant ?**

On utilise un service partagé. Le service maintient l'état commun, et les deux components l'injectent. On peut utiliser un `BehaviorSubject` de RxJS dans le service pour que les components puissent s'abonner aux changements de données en temps réel.

---

**Q21 — C'est quoi `ViewChild` ?**

`@ViewChild` permet d'accéder directement à un component enfant ou un élément DOM depuis le component parent dans le code TypeScript. C'est utile quand on veut appeler une méthode d'un component enfant ou manipuler directement un élément HTML.

---

## 🔷 6. Lifecycle Hooks

---

**Q22 — C'est quoi les lifecycle hooks dans Angular ?**

Ce sont des méthodes spéciales qu'on peut implémenter dans un component pour exécuter du code à des moments précis du cycle de vie du component. Les plus importants sont :

- `ngOnInit()` : appelé une fois après l'initialisation du component, c'est là qu'on fait les appels HTTP ou l'initialisation des données
- `ngOnChanges()` : appelé quand les inputs changent
- `ngOnDestroy()` : appelé juste avant la destruction du component, c'est là qu'on se désabonne des observables
- `ngAfterViewInit()` : appelé après que la vue (le template) est initialisée

---

**Q23 — Quelle est la différence entre le constructeur et `ngOnInit` ?**

Le constructeur c'est une méthode TypeScript pure, utilisée pour l'injection de dépendances. `ngOnInit` c'est un lifecycle hook Angular, appelé après que les inputs ont été initialisés. On ne doit pas faire des appels HTTP ou accéder aux inputs dans le constructeur. Tout ça doit aller dans `ngOnInit`.

---

## 🔷 7. Routing

---

**Q24 — Comment fonctionne le routing dans Angular ?**

Le routing permet de naviguer entre différentes vues sans recharger la page, c'est ce qui fait d'Angular une SPA. On configure les routes dans un tableau de type `Routes`, en associant un path à un component. On utilise `RouterModule.forRoot()` dans l'AppModule. Dans le template, on place `<router-outlet>` à l'endroit où les components doivent s'afficher.

---

**Q25 — C'est quoi le lazy loading et pourquoi c'est important ?**

Le lazy loading c'est le fait de charger un module seulement quand l'utilisateur navigue vers sa route, pas au démarrage de l'application. Ça améliore les performances car le bundle initial est plus petit. Pour le configurer, on utilise `loadChildren` dans la configuration des routes avec une fonction qui importe le module dynamiquement.

---

**Q26 — C'est quoi un Route Guard ?**

Un route guard c'est un service qui contrôle l'accès à une route. Par exemple, `CanActivate` permet de bloquer l'accès à une route si l'utilisateur n'est pas connecté. `CanDeactivate` permet d'empêcher de quitter une route si l'utilisateur n'a pas sauvegardé ses modifications. C'est très utilisé pour la gestion des droits d'accès.

---

**Q27 — Comment passer des paramètres dans une route ?**

On peut passer des paramètres de deux façons :
- **Route params** : `/produits/:id` → on récupère avec `ActivatedRoute.snapshot.params['id']`
- **Query params** : `/produits?page=2` → on récupère avec `ActivatedRoute.snapshot.queryParams['page']`

On peut aussi naviguer programmatiquement avec le service `Router` : `this.router.navigate(['/produits', id])`.

---

## 🔷 8. Formulaires

---

**Q28 — Quelle est la différence entre Template-driven forms et Reactive forms ?**

**Template-driven forms** : la logique est surtout dans le template HTML avec `ngModel`. C'est simple à mettre en place pour des formulaires basiques, mais difficile à tester et à gérer quand ça devient complexe.

**Reactive forms** : la logique est dans le code TypeScript avec `FormGroup` et `FormControl`. C'est plus verbeux mais plus puissant, plus testable, et recommandé pour les formulaires complexes. On a un contrôle total sur la validation et les changements de valeur.

---

**Q29 — Comment faire de la validation dans un Reactive form ?**

On ajoute des `Validators` directement dans la définition des `FormControl`. Par exemple `Validators.required`, `Validators.minLength(3)`, `Validators.email`. On peut aussi créer des validators personnalisés. Dans le template, on affiche les messages d'erreur en vérifiant l'état du control : `invalid`, `touched`, `dirty`.

---

**Q30 — C'est quoi `FormBuilder` ?**

C'est un service qui simplifie la création des formulaires réactifs. Au lieu d'écrire `new FormGroup({ nom: new FormControl('') })`, on peut utiliser `this.fb.group({ nom: [''] })`. C'est plus court et plus lisible.

---

## 🔷 9. RxJS et Observables

---

**Q31 — C'est quoi un Observable ?**

Un Observable c'est un flux de données asynchrone auquel on peut s'abonner avec `.subscribe()`. C'est la base de RxJS, la bibliothèque de programmation réactive utilisée par Angular. Contrairement aux Promises, un Observable peut émettre plusieurs valeurs au cours du temps, et on peut appliquer des opérateurs dessus pour transformer ou filtrer les données.

---

**Q32 — Quelle est la différence entre un Observable et une Promise ?**

- Une **Promise** ne peut retourner qu'une seule valeur, elle s'exécute immédiatement
- Un **Observable** peut retourner plusieurs valeurs dans le temps, il est lazy (ne s'exécute que quand on subscribe), et on peut l'annuler (unsubscribe)

---

**Q33 — C'est quoi les opérateurs RxJS les plus utilisés ?**

- `map` : transforme chaque valeur émise
- `filter` : filtre les valeurs selon une condition
- `switchMap` : annule l'observable précédent et switch vers un nouveau (utile pour les appels HTTP liés)
- `mergeMap` : exécute plusieurs observables en parallèle
- `catchError` : gère les erreurs dans le flux
- `tap` : effectue un side effect sans modifier les données (utile pour le debug)
- `debounceTime` : attend X millisecondes avant d'émettre (utile pour la recherche en temps réel)
- `distinctUntilChanged` : n'émet que si la valeur a changé

---

**Q34 — C'est quoi un Subject et un BehaviorSubject ?**

Un **Subject** c'est à la fois un Observable et un Observer. On peut émettre des valeurs dedans avec `.next()` et s'y abonner. Un **BehaviorSubject** c'est un Subject qui garde en mémoire la dernière valeur émise. Quand un nouveau subscriber s'abonne, il reçoit immédiatement la dernière valeur. C'est très utile pour partager l'état entre components via un service.

---

**Q35 — Pourquoi est-ce important de se désabonner (unsubscribe) des observables ?**

Si on ne se désabonne pas, les abonnements continuent à vivre même après la destruction du component. Ça cause des memory leaks et peut entraîner des comportements inattendus. On se désabonne dans `ngOnDestroy`. Une autre façon propre de faire ça c'est d'utiliser le pipe `async` dans le template, qui gère automatiquement le subscribe et l'unsubscribe.

---

## 🔷 10. HttpClient

---

**Q36 — Comment faire des appels HTTP dans Angular ?**

Angular fournit le `HttpClient` service. On l'injecte dans le service ou le component, puis on utilise les méthodes `get()`, `post()`, `put()`, `delete()`, etc. Ces méthodes retournent des Observables. Pour les utiliser, il faut importer `HttpClientModule` dans le module.

---

**Q37 — C'est quoi les HTTP Interceptors ?**

Un interceptor c'est un service qui intercepte toutes les requêtes HTTP sortantes ou les réponses entrantes. C'est très utile pour ajouter automatiquement un token d'authentification dans les headers de chaque requête, ou pour gérer les erreurs HTTP de façon centralisée. On l'implémente avec l'interface `HttpInterceptor`.

---

**Q38 — Comment gérer les erreurs HTTP ?**

On utilise l'opérateur `catchError` de RxJS dans le service. Quand une erreur survient, on peut retourner un Observable vide, ou lancer une nouvelle erreur avec un message personnalisé. On peut aussi afficher un message à l'utilisateur. C'est une bonne pratique de centraliser cette gestion dans les services.

---

## 🔷 11. Pipes

---

**Q39 — C'est quoi un pipe dans Angular ?**

Un pipe c'est une fonction de transformation qu'on utilise directement dans le template pour formater des données à l'affichage. Par exemple `{{ date | date:'dd/MM/yyyy' }}` ou `{{ prix | currency:'EUR' }}`. Angular fournit des pipes natifs, et on peut créer ses propres pipes personnalisés.

---

**Q40 — Quels sont les pipes natifs d'Angular les plus utilisés ?**

- `date` : formate une date
- `currency` : formate un nombre en devise
- `decimal` : formate un nombre avec des décimales
- `uppercase` / `lowercase` : change la casse
- `json` : affiche un objet JSON (utile pour le debug)
- `async` : souscrit automatiquement à un Observable ou une Promise dans le template
- `slice` : retourne une portion d'une liste ou d'une chaîne

---

**Q41 — C'est quoi un pure vs impure pipe ?**

Un **pure pipe** (par défaut) n'est recalculé que si l'input change réellement (nouvelle référence). C'est performant. Un **impure pipe** est recalculé à chaque cycle de détection de changement, ce qui peut nuire aux performances mais est parfois nécessaire quand l'input est un objet ou tableau muté.

---

## 🔷 12. Change Detection

---

**Q42 — C'est quoi la détection de changements (change detection) ?**

C'est le mécanisme par lequel Angular détecte les changements dans les données et met à jour la vue. Par défaut, Angular vérifie tous les components à chaque événement, appel HTTP, ou timer. C'est la stratégie `Default`.

---

**Q43 — C'est quoi `ChangeDetectionStrategy.OnPush` ?**

C'est une stratégie de change detection optimisée. Avec `OnPush`, Angular ne vérifie le component que si une de ses inputs a changé (nouvelle référence), si un événement dans le component s'est produit, ou si on déclenche manuellement la détection. C'est une bonne pratique pour les applications avec beaucoup de components car ça améliore significativement les performances.

---

## 🔷 13. Performance et Bonnes Pratiques

---

**Q44 — Comment optimiser les performances d'une application Angular ?**

Plusieurs techniques existent :
- Utiliser `OnPush` change detection strategy
- Utiliser le lazy loading pour les modules
- Utiliser le pipe `async` au lieu de s'abonner manuellement
- Utiliser `trackBy` dans les `*ngFor` pour éviter de re-rendre toute la liste
- Éviter les calculs complexes dans les templates
- Utiliser les observables à la place des setInterval ou setTimeout
- Activer le mode production (AOT compilation, tree shaking)

---

**Q45 — C'est quoi `trackBy` dans `*ngFor` ?**

Par défaut, quand la liste change, Angular re-render tous les éléments. Avec `trackBy`, on lui indique comment identifier chaque élément (souvent par son id). Ainsi, Angular ne re-render que les éléments qui ont vraiment changé, ce qui est bien plus performant pour les grandes listes.

---

**Q46 — C'est quoi AOT (Ahead-of-Time) compilation ?**

AOT c'est la compilation des templates Angular au moment du build, pas dans le navigateur. Ça donne une application plus rapide au démarrage, plus petite en taille, et avec des erreurs détectées plus tôt (pendant le build). Le mode production utilise AOT par défaut.

---

## 🔷 14. State Management

---

**Q47 — Comment gérer l'état dans une application Angular ?**

Pour les petites applications, on peut utiliser des services avec des BehaviorSubjects. Pour les applications plus complexes, on utilise une bibliothèque de state management comme **NgRx** (inspiré de Redux). NgRx organise l'état en un store centralisé, avec des actions, des reducers et des selectors.

---

**Q48 — C'est quoi NgRx en quelques mots ?**

NgRx c'est une bibliothèque de state management pour Angular basée sur le pattern Redux et RxJS. On a :
- **Store** : l'état global de l'application en un seul endroit
- **Actions** : des événements qui décrivent ce qui se passe
- **Reducers** : des fonctions pures qui modifient l'état selon les actions
- **Selectors** : pour lire des données spécifiques du store
- **Effects** : pour les side effects comme les appels HTTP

---

## 🔷 15. Tests

---

**Q49 — Comment tester un component Angular ?**

On utilise `TestBed` pour créer un environnement de test. On peut tester la logique du component (méthodes, propriétés) et l'affichage dans le template. Angular génère automatiquement un fichier `.spec.ts` pour chaque component. On utilise Jasmine comme framework de test et Karma comme test runner.

---

**Q50 — C'est quoi la différence entre un test unitaire et un test d'intégration dans Angular ?**

Un **test unitaire** teste une classe isolément, en mockant toutes ses dépendances. Un **test d'intégration** (ou component test) teste le component avec ses interactions réelles avec le template, les services, et parfois les components enfants. Dans Angular, `TestBed` permet de faire les deux.

---

## 🔷 16. Questions Avancées

---

**Q51 — C'est quoi le Content Projection ?**

C'est un mécanisme qui permet d'injecter du contenu HTML dans un component depuis l'extérieur. On utilise `<ng-content>` à l'intérieur du template du component pour indiquer où le contenu injecté doit apparaître. C'est très utile pour créer des components réutilisables comme des cards ou des modals.

---

**Q52 — C'est quoi `ng-template` et `ng-container` ?**

- `ng-template` : un élément Angular invisible, utilisé pour définir un template qu'on peut afficher conditionnellement avec `*ngIf` ou rendre avec `ngTemplateOutlet`. Il n'est pas rendu dans le DOM par défaut.
- `ng-container` : un élément de groupement qui n'est pas rendu dans le DOM. Utile pour appliquer des directives sur plusieurs éléments sans ajouter un div inutile.

---

**Q53 — C'est quoi les Standalone Components (Angular 14+) ?**

Ce sont des components qui n'ont pas besoin d'être déclarés dans un NgModule. Ils gèrent eux-mêmes leurs imports avec la propriété `imports` dans le décorateur `@Component`. C'est une nouvelle approche qui simplifie l'architecture et réduit la complexité des modules. Angular évolue vers cette approche.

---

**Q54 — C'est quoi les Signals (Angular 16+) ?**

Les Signals sont un nouveau mécanisme de réactivité introduit dans Angular 16. Un signal est une valeur qui notifie automatiquement les consumers quand elle change. C'est une alternative à RxJS pour la gestion de l'état local, plus simple à utiliser et plus performant car Angular sait exactement quels components re-rendre.

---

**Q55 — Quels sont les outils qu'on utilise avec Angular en pratique ?**

- **Angular CLI** : pour générer le projet, les components, les services, etc.
- **Angular DevTools** : extension Chrome pour debugger l'arbre des components
- **RxJS** : programmation réactive, inclus par défaut
- **NgRx** ou **Akita** : state management
- **Angular Material** ou **PrimeNG** : bibliothèques de composants UI
- **Jest** ou **Karma/Jasmine** : pour les tests
- **ESLint** : linting du code TypeScript

---

*Bonne chance pour ton entretien demain ! 💪*
