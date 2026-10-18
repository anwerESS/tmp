# 📘 Plan de Formation RxJS pour Entretien

## 🎯 Objectif
Préparer un entretien pour un poste Java avec interventions Angular (RxJS est essentiel pour Angular)

---

## 📚 Structure du Cours

### [**📖 Chapitre 1 : Introduction à RxJS**](./01_intro_RxJs.md)
**Durée estimée : 2-3 heures**

#### 1.1 Qu'est-ce que RxJS ?
- Programmation réactive
- Observables vs Promises
- Cas d'usage dans Angular
- Installation et configuration

#### 1.2 Concepts Fondamentaux
- Observable
- Observer
- Subscription
- Opérateurs
- Subjects

#### 1.3 Premier Observable
- Création basique
- Souscription (subscribe)
- Unsubscribe
- Gestion de la mémoire

#### 1.4 Exercice Pratique
- Challenge : Créer votre premier Observable
- Solution détaillée

#### 📝 Questions/Réponses d'Entretien

---

### [**📖 Chapitre 2 : Création d'Observables**](./02_creation_observble.md)
**Durée estimée : 3-4 heures**

#### 2.1 Opérateurs de Création
- `of()` - Émettre des valeurs
- `from()` - Convertir des structures
- `interval()` et `timer()` - Valeurs temporelles
- `fromEvent()` - Événements DOM
- `ajax()` - Requêtes HTTP

#### 2.2 Création Personnalisée
- `new Observable()`
- `create()` (déprécié)
- Création avec générateurs
- Observables froids vs chauds

#### 2.3 Subjects
- `Subject` - Multicast basique
- `BehaviorSubject` - Avec valeur initiale
- `ReplaySubject` - Replay d'événements
- `AsyncSubject` - Dernière valeur

#### 2.4 Exercice Pratique
- Challenge : Service de notification
- Solution détaillée

#### 📝 Questions/Réponses d'Entretien

---

### [**📖 Chapitre 3 : Opérateurs de Transformation**](./03_op_tranformation.md)
**Durée estimée : 4-5 heures**

#### 3.1 Opérateurs de Base
- `map()` - Transformer les valeurs
- `pluck()` - Extraire une propriété
- `mapTo()` - Mapper vers une constante

#### 3.2 Opérateurs de Flattenning
- `mergeMap()` (flatMap) - Fusionner les Observables
- `switchMap()` - Annuler et basculer
- `concatMap()` - Préserver l'ordre
- `exhaustMap()` - Ignorer pendant l'exécution

#### 3.3 Différences et Cas d'Usage
- Quand utiliser mergeMap vs switchMap
- Quand utiliser concatMap vs exhaustMap
- Diagrammes de marbre (Marble Diagrams)

#### 3.4 Autres Transformations
- `scan()` - Accumulation
- `reduce()` - Réduction finale
- `toArray()` - Convertir en tableau

#### 3.5 Exercice Pratique
- Challenge : Recherche avec autocomplete
- Solution détaillée

#### 📝 Questions/Réponses d'Entretien

---

### [**📖 Chapitre 4 : Opérateurs de Filtrage**](./04_op_fileterage.md)
**Durée estimée : 3-4 heures**

#### 4.1 Filtrage de Valeurs
- `filter()` - Condition de filtrage
- `take()` et `takeLast()` - Nombre d'émissions
- `takeUntil()` et `takeWhile()` - Conditions d'arrêt
- `skip()` et `skipUntil()` - Sauter des valeurs

#### 4.2 Gestion du Temps
- `debounceTime()` - Attendre un délai
- `throttleTime()` - Limiter la fréquence
- `auditTime()` - Dernière valeur dans intervalle
- `sampleTime()` - Échantillonner

#### 4.3 Déduplication
- `distinct()` - Valeurs uniques
- `distinctUntilChanged()` - Changements uniquement
- `distinctUntilKeyChanged()` - Par propriété

#### 4.4 Exercice Pratique
- Challenge : Système de recherche optimisé
- Solution détaillée

#### 📝 Questions/Réponses d'Entretien

---

### [**📖 Chapitre 5 : Combinaison d'Observables**](./05_combin_observables.md)
**Durée estimée : 4-5 heures**

#### 5.1 Opérateurs de Combinaison
- `merge()` - Fusionner plusieurs flux
- `concat()` - Concaténer séquentiellement
- `race()` - Premier à émettre
- `zip()` - Combiner par paires
- `combineLatest()` - Dernières valeurs
- `forkJoin()` - Attendre la complétion

#### 5.2 Opérateurs de Jointure
- `withLatestFrom()` - Combiner avec le dernier
- `startWith()` - Valeur initiale
- `endWith()` - Valeur finale

#### 5.3 Cas d'Usage Pratiques
- Formulaires réactifs
- Requêtes parallèles
- Dépendances entre requêtes

#### 5.4 Exercice Pratique
- Challenge : Dashboard avec données multiples
- Solution détaillée

#### 📝 Questions/Réponses d'Entretien

---

### [**📖 Chapitre 6 : Gestion des Erreurs**](./06_gestion_erreurs.md)
**Durée estimée : 3-4 heures**

#### 6.1 Gestion d'Erreurs
- `catchError()` - Intercepter les erreurs
- `retry()` - Réessayer automatiquement
- `retryWhen()` - Réessayer avec logique
- Stratégies de récupération

#### 6.2 Opérateurs de Finalisation
- `finalize()` - Cleanup garanti
- `tap()` (do) - Effets de bord
- Logging et debugging

#### 6.3 Patterns de Gestion d'Erreurs
- Fallback values
- Retry avec backoff exponentiel
- Error boundaries

#### 6.4 Exercice Pratique
- Challenge : Appels API résilients
- Solution détaillée

#### 📝 Questions/Réponses d'Entretien

---

### [**📖 Chapitre 7 : RxJS dans Angular**](./07_RxJs_Angular.md)
**Durée estimée : 5-6 heures**

#### 7.1 HttpClient et Observables
- GET, POST, PUT, DELETE
- Intercepteurs
- Gestion des erreurs HTTP
- Cache avec ShareReplay

#### 7.2 Formulaires Réactifs
- FormControl et valueChanges
- Validation asynchrone
- Formulaires dynamiques

#### 7.3 Router et Observables
- Router events
- ActivatedRoute params
- Guards avec Observables

#### 7.4 State Management
- Services avec BehaviorSubject
- Pattern Store
- Async Pipe

#### 7.5 Best Practices Angular
- Unsubscribe automatique
- AsyncPipe vs subscribe
- Memory leaks
- Performance

#### 7.6 Exercice Pratique
- Challenge : Application CRUD complète
- Solution détaillée

#### 📝 Questions/Réponses d'Entretien

---

### [**📖 Chapitre 8 : Patterns et Best Practices**](./08_patterns_best-practises.md)
**Durée estimée : 3-4 heures**

#### 8.1 Patterns Courants
- Service avec cache
- Infinite scroll
- Polling et WebSockets
- Undo/Redo

#### 8.2 Performance
- ShareReplay et multicasting
- Lazy evaluation
- Unsubscribe strategies
- Memory leak prevention

#### 8.3 Testing
- Testing avec marbles
- Fake schedulers
- Mock Observables
- TestScheduler

#### 8.4 Debugging
- `tap()` pour logging
- RxJS DevTools
- Marble diagrams
- Console operators

#### 8.5 Exercice Pratique
- Challenge : Optimisation d'application
- Solution détaillée

#### 📝 Questions/Réponses d'Entretien

---

## 📊 Résumé du Planning

| Chapitre | Thème | Durée | Difficulté |
|----------|-------|-------|------------|
| **1** | Introduction à RxJS | 2-3h | ⭐ Débutant |
| **2** | Création d'Observables | 3-4h | ⭐⭐ Débutant |
| **3** | Opérateurs de Transformation | 4-5h | ⭐⭐⭐ Intermédiaire |
| **4** | Opérateurs de Filtrage | 3-4h | ⭐⭐ Intermédiaire |
| **5** | Combinaison d'Observables | 4-5h | ⭐⭐⭐ Intermédiaire |
| **6** | Gestion des Erreurs | 3-4h | ⭐⭐ Intermédiaire |
| **7** | RxJS dans Angular | 5-6h | ⭐⭐⭐⭐ Avancé |
| **8** | Patterns et Best Practices | 3-4h | ⭐⭐⭐⭐ Avancé |

**Durée totale : 27-35 heures** (environ 1 semaine à temps plein)

---

## 🎯 Objectifs d'Apprentissage

### Après ce cours, vous serez capable de :

✅ **Comprendre** les concepts fondamentaux de la programmation réactive
✅ **Créer** et manipuler des Observables
✅ **Maîtriser** les opérateurs principaux (map, filter, merge, switch, etc.)
✅ **Gérer** les erreurs et les retry strategies
✅ **Implémenter** RxJS dans des applications Angular
✅ **Optimiser** les performances et éviter les memory leaks
✅ **Tester** du code RxJS
✅ **Répondre** aux questions techniques en entretien

---

## 📝 Méthodologie de Révision Suggérée

### Semaine 1 : Fondamentaux
- **Jour 1-2** : Chapitre 1 et 2 (Introduction + Création)
- **Jour 3-4** : Chapitre 3 (Transformation)
- **Jour 5** : Chapitre 4 (Filtrage)

### Semaine 2 : Avancé
- **Jour 1-2** : Chapitre 5 (Combinaison)
- **Jour 3** : Chapitre 6 (Erreurs)
- **Jour 4-5** : Chapitre 7 (Angular)

### Semaine 3 : Maîtrise
- **Jour 1-2** : Chapitre 8 (Patterns)
- **Jour 3-4** : Révision et exercices
- **Jour 5** : Questions/réponses d'entretien

---

## 🎓 Points Clés pour l'Entretien

Chaque chapitre inclura :

### 📋 Structure Standard
1. **Théorie** : Explication détaillée des concepts
2. **Exemples** : Code commenté et progressif
3. **Diagrammes** : Marble diagrams pour visualiser
4. **Cas d'usage** : Applications pratiques Angular
5. **Exercices** : Challenges avec solutions
6. **Pièges** : Erreurs courantes à éviter
7. **Q/R** : 10-15 questions d'entretien par chapitre

### 🎯 Focus Entretien
- Différences entre opérateurs similaires
- Quand utiliser quel opérateur
- Gestion de la mémoire
- Best practices Angular
- Debugging et testing
- Performance

---

## 🛠️ Outils et Ressources

### Outils de Développement
- **RxJS Playground** : https://rxjs.dev/
- **RxViz** : Visualiser les Observables
- **RxJS Marbles** : Tester avec marbles
- **Angular DevTools** : Debugging

### Documentation
- **RxJS Official** : https://rxjs.dev/guide/overview
- **Learn RxJS** : https://www.learnrxjs.io/
- **RxJS Operators** : Liste complète

### Pratique
- **StackBlitz** : Environnement en ligne
- **CodeSandbox** : Projets Angular
- **Exercices interactifs** : RxJS Koans

---