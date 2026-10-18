# 🎯 Cours Angular 2026 - Guide Complet pour Entretien

## 📚 Table des Matières

### Partie 1 : Fondamentaux (Chapitres 1-5)
1. [Installation et Setup](./01-installation-setup.md)
2. [Création de Projet](./02-creation-projet.md)
3. [Les Components](./03-les-components.md)
4. [Les Signal Inputs](./04-signal-inputs.md)
5. [Les Outputs et Models](./05-outputs-models.md)

### Partie 2 : Concepts Avancés (Chapitres 6-8)
6. [Les Signaux et la Détection de Changement](./06-signals-change-detection.md)
7. [Boucles et Conditions](./07-boucles-conditions.md)
8. [Les Services](./08-les-services.md)

### Partie 3 : Applications Professionnelles (Chapitres 9-13)
9. [Les Routes et la Navigation](./09-routes-navigation.md)
10. [Formulaires Réactifs (Reactive Forms)](./10-formulaires-reactifs.md)
11. [Angular Material](./11-angular-material.md)
12. [Authentication - Login, Interceptor, Guard](./12-authentication.md)
13. [API REST et HttpClient](./13-api-http.md)

### Partie 4 : Tests et Qualité (Chapitre 14)
14. [Tests Unitaires avec Jasmine et Karma](./14-tests-jasmine-karma.md)

---

## 🚀 Vue d'Ensemble du Cours

Ce cours complet Angular couvre les fondamentaux d'Angular 20+ avec une approche pratique centrée sur la création d'une **application de gestion de collections**.

### 🎓 Objectifs Pédagogiques

- ✅ Maîtriser l'installation et la configuration d'Angular
- ✅ Comprendre l'architecture par composants
- ✅ Utiliser les Signal Inputs (nouvelle API reactive)
- ✅ Gérer la communication parent-enfant
- ✅ Implémenter le two-way binding avec model()

---

## 📖 Résumé par Chapitre

### Chapitre 1 : Installation
**Durée estimée** : 30 minutes

**Concepts clés** :
- Node.js et NPM
- Angular CLI
- VS Code et extensions
- Vérification de l'environnement

**Commandes essentielles** :
```bash
npm install -g @angular/cli
ng version
```

---

### Chapitre 2 : Création de Projet
**Durée estimée** : 45 minutes

**Concepts clés** :
- Structure d'un projet Angular
- Fichiers de configuration (angular.json, tsconfig.json)
- Dossiers src/ et app/
- Lancement du serveur de développement

**Commandes essentielles** :
```bash
ng new nom-projet
ng serve
```

---

### Chapitre 3 : Les Components
**Durée estimée** : 1 heure

**Concepts clés** :
- Anatomie d'un component (4 fichiers)
- @Component decorator
- Selector et Template
- Composition de components

**Commandes essentielles** :
```bash
ng g c components/nom-composant
```

---

### Chapitre 4 : Signal Inputs
**Durée estimée** : 1h30

**Concepts clés** :
- Communication Parent → Enfant
- input() et input.required()
- InputSignal<T>
- Property binding [attr]
- Transform et Alias

**Code essentiel** :
```typescript
data = input.required<Type>();
```
```html
<child [data]="parentData"></child>
```

---

### Chapitre 6 : Signaux et Détection de Changement
**Durée estimée** : 1h30

**Concepts clés** :
- signal(), computed(), effect()
- Zone.js vs Zoneless
- ChangeDetectionStrategy.OnPush
- Réactivité et optimisation

**Code essentiel** :
```typescript
selectedIndex = signal(0);
selectedItem = computed(() => items[this.selectedIndex()]);
effect(() => console.log(this.selectedItem()));
```

---

### Chapitre 7 : Boucles et Conditions
**Durée estimée** : 1 heure

**Concepts clés** :
- @for avec track obligatoire
- @empty pour listes vides
- @if/@else pour conditions
- @switch/@case pour multi-conditions
- @let pour variables locales

**Code essentiel** :
```html
@for (item of items; track item.id) {
  <app-card [item]="item"></app-card>
} @empty {
  <p>Aucun résultat</p>
}
```

---

### Chapitre 8 : Les Services
**Durée estimée** : 1h30

**Concepts clés** :
- @Injectable et Singleton
- inject() pour injection
- CRUD operations
- localStorage pour persistence

**Code essentiel** :
```typescript
@Injectable({ providedIn: 'root' })
export class CollectionService {
  private collections = signal<Collection[]>([]);
}
```

---

### Chapitres 9-13 : Concepts Professionnels
**Durée estimée** : 8-12 heures

**Chapitre 9 : Routes et Navigation** :
- Configuration des routes (app.routes.ts)
- RouterOutlet et RouterLink
- Paramètres de route (:id)
- Routes enfants (children)
- Redirection et wildcards (**)

**Chapitre 10 : Formulaires Réactifs** :
- FormControl, FormGroup, FormBuilder
- Validators (required, email, min, max, pattern)
- Gestion des erreurs (hasError, touched, dirty)
- valueChanges Observable
- Validateurs personnalisés

**Chapitre 11 : Angular Material** :
- Installation (ng add @angular/material)
- Composants principaux (Button, Input, Select, Card)
- mat-form-field structure
- Upload de fichiers
- Personnalisation et theming

**Chapitre 12 : Authentification** :
- Page de login avec formulaires
- HttpClient configuration
- Interceptors HTTP (ajout de token)
- Guards de route (CanActivate)
- Gestion du token JWT

**Chapitre 13 : API REST et HttpClient** :
- CRUD complet (Create, Read, Update, Delete)
- Gestion des erreurs HTTP
- Cache avec signals
- Pagination et recherche
- Tests unitaires

**Ces 5 chapitres détaillés** couvrent tout le nécessaire pour créer une application Angular professionnelle complète.

---

### Chapitre 14 : Tests Unitaires avec Jasmine et Karma
**Durée estimée** : 6-8 heures

**Concepts fondamentaux** :
- Jasmine (framework de test) vs Karma (test runner)
- Structure des tests : describe(), it(), beforeEach(), afterEach()
- Matchers : toBe(), toEqual(), toBeTruthy(), etc.
- Pattern AAA : Arrange, Act, Assert

**Tester les Services** :
- Services simples avec signals
- Services avec dépendances
- Mock avec jasmine.createSpyObj()
- HttpClientTestingModule pour les appels API

**Tester les Components** :
- TestBed et ComponentFixture
- fixture.detectChanges() pour mettre à jour le DOM
- Tester les @Input() avec setInput()
- Tester les @Output() avec subscribe()
- Tester l'affichage (template)

**Tester les Formulaires** :
- Reactive Forms validation
- Vérifier les erreurs avec hasError()
- Tester les états : valid, invalid, touched, dirty

**Spies et Mocking** :
- spyOn() pour surveiller les appels
- and.returnValue() pour mocker les retours
- and.callThrough() pour appeler la vraie méthode
- and.callFake() pour implémenter un comportement personnalisé

**Tests Asynchrones** :
- done() callback
- fakeAsync() et tick()
- waitForAsync() (anciennement async())
- Tester les Observables et Promises

**Couverture de Code** :
- Générer avec --code-coverage
- Interpréter les métriques (statements, branches, functions, lines)
- Viser 80%+ de couverture

**Bonnes Pratiques** :
- Tests indépendants et déterministes
- Un test = une assertion logique
- Nommage clair : "should [action] when [condition]"
- Mock les dépendances
- Tester le comportement, pas l'implémentation

**15 Questions d'entretien** avec réponses détaillées sur les tests Angular.

Ce chapitre est **essentiel** pour tout développeur Angular professionnel - les tests sont la fondation d'une application maintenable !

---

## 🎯 Points Critiques pour l'Entretien

### Architecture Angular

1. **Composants** : Briques réutilisables de l'application
2. **Modules** : Organisation du code (ancien système)
3. **Services** : Logique métier partagée
4. **Routing** : Navigation entre pages

### Concepts Modernes (Angular 17+)

1. **Standalone Components** : Plus besoin de NgModules
2. **Signal Inputs** : API réactive moderne
3. **model()** : Two-way binding simplifié
4. **Zoneless** : Détection de changement optimisée

### Communication entre Components

```
Parent Component
    ↓ [input]
Child Component
    ↓ (output)
Parent Component
```

---

## 📝 Checklist de Préparation Entretien

### Connaissances de Base
- [ ] Expliquer ce qu'est Angular
- [ ] Différence TypeScript vs JavaScript
- [ ] Rôle de Node.js et NPM
- [ ] Structure d'un projet Angular

### Components
- [ ] Créer un component de A à Z
- [ ] Expliquer le @Component decorator
- [ ] Différence entre template et templateUrl
- [ ] Cycle de vie d'un component

### Data Binding
- [ ] {{ }} : Interpolation
- [ ] [attr] : Property binding
- [ ] (event) : Event binding
- [ ] [(ngModel)] : Two-way binding

### Communication
- [ ] Passer des données avec input()
- [ ] Émettre des événements avec output()
- [ ] Utiliser model() pour le two-way binding
- [ ] Différence input() vs input.required()

---

## 🔍 Questions Fréquentes en Entretien

### Q1: Qu'est-ce qu'Angular ?
**Réponse** : Framework TypeScript pour créer des applications web SPA (Single Page Application). Développé par Google, il propose une architecture par composants, le data binding, et l'injection de dépendances.

### Q2: Différence entre Angular et AngularJS ?
**Réponse** : 
- AngularJS (v1) : JavaScript, ancien framework (2010)
- Angular (v2+) : TypeScript, réécriture complète (2016+)
- Incompatibilité totale entre les deux

### Q3: Qu'est-ce qu'un Component ?
**Réponse** : Classe TypeScript décorée avec @Component qui encapsule :
- Vue (template HTML)
- Style (CSS/SCSS)
- Logique (TypeScript)
- Tests (spec.ts)

### Q4: Comment passer des données entre components ?
**Réponse** :
- Parent → Enfant : `@Input()` ou `input()`
- Enfant → Parent : `@Output()` ou `output()` avec EventEmitter
- Two-way : `[(ngModel)]` ou `model()`

### Q5: Qu'est-ce qu'un Signal ?
**Réponse** : Nouvelle API réactive d'Angular 16+ qui :
- Suit les changements automatiquement
- Optimise la détection de changement
- S'appelle comme une fonction : `signal()`

---

## 💼 Projet Pratique : Application de Gestion de Collections

### Fonctionnalités Implémentées

1. **Affichage de cartes** d'objets de collection
2. **Barre de recherche** interactive
3. **Communication** entre composants
4. **Gestion d'état** avec signals

### Architecture

```
App Component
├── Header
│   └── Search Bar Component
└── Collection Grid
    └── Collection Item Card Component (x N)
```

---

## 🛠️ Commandes Angular Essentielles

```bash
# Création
ng new nom-projet
ng g c components/nom
ng g s services/nom
ng g m modules/nom

# Développement
ng serve
ng serve --open
ng serve --port 3000

# Build
ng build
ng build --configuration production

# Tests
ng test
ng e2e

# Utilitaires
ng version
ng update
ng add @angular/material
```

---

## 📚 Ressources Complémentaires

### Documentation Officielle
- 📖 [Angular.io](https://angular.io)
- 📖 [Angular CLI](https://angular.io/cli)
- 📖 [TypeScript](https://www.typescriptlang.org)

### Outils de Développement
- 🔧 [Angular DevTools](https://angular.io/guide/devtools)
- 🔧 [VS Code Angular Extension](https://marketplace.visualstudio.com/items?itemName=Angular.ng-template)

### Communauté
- 💬 [Stack Overflow - Angular](https://stackoverflow.com/questions/tagged/angular)
- 💬 [Reddit - r/Angular2](https://reddit.com/r/Angular2)
- 💬 [Discord Angular](https://discord.gg/angular)

---

## 🎓 Niveaux de Compétence

### Débutant (1-3 mois)
- ✅ Installation et setup
- ✅ Création de components basiques
- ✅ Data binding simple
- ✅ Inputs/Outputs basiques

### Intermédiaire (3-12 mois)
- ✅ Services et injection de dépendances
- ✅ Routing et navigation
- ✅ Formulaires réactifs
- ✅ HTTP et API REST
- ✅ RxJS de base

### Avancé (1-2 ans)
- ✅ State management (NgRx, Signals)
- ✅ Architecture avancée
- ✅ Performance optimization
- ✅ Testing avancé
- ✅ SSR et PWA

---

## 🚀 Ce Cours Couvre

Après avoir maîtrisé ces 14 chapitres, vous saurez :

✅ **Installer** et configurer Angular  
✅ **Créer** des composants réutilisables  
✅ **Communiquer** entre composants (inputs/outputs)  
✅ **Gérer l'état** avec les signals  
✅ **Optimiser** avec OnPush et Zoneless  
✅ **Boucler et conditionner** dans les templates  
✅ **Centraliser** la logique avec les services  
✅ **Naviguer** entre les pages  
✅ **Valider** les formulaires  
✅ **Styliser** avec Angular Material  
✅ **Sécuriser** avec authentification  
✅ **Communiquer** avec des APIs REST  
✅ **Tester** avec Jasmine et Karma  

### Architecture d'Application Complète

```
Application Angular Professionnelle
├── Components (UI)
├── Services (Logique)
├── Routing (Navigation)
├── Forms (Saisie)
├── Guards (Sécurité)
├── Interceptors (HTTP)
├── API (Backend)
└── Tests (Qualité)
```

---

## 📞 Support

Pour toute question :
- 📧 SimpleTech par Sergio Sousa
- 🌐 [simpletechprod.com](https://simpletechprod.com)
- 📺 Cours vidéo YouTube disponibles

---

## ⭐ Bon Courage pour Votre Entretien !

Ce guide vous donne toutes les bases nécessaires pour réussir un entretien Angular junior à intermédiaire. Pratiquez régulièrement et n'hésitez pas à créer vos propres projets !

**Temps de préparation recommandé** : 10-15 heures de pratique
**Révision avant entretien** : 2-3 heures

---

*Dernière mise à jour : Février 2026*
*Basé sur Angular 20.3+*
