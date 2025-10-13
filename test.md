# Questions d'Entretien C++ - Guide Complet (200 Questions)

## 🎯 Fondamentaux C++ (Q1-Q50)

### Q1: Quelle est la différence entre C et C++ ?
**Réponse:** C++ est une extension du C qui ajoute la programmation orientée objet, les templates, les exceptions, la STL, les namespaces, et une gestion mémoire plus sophistiquée avec new/delete. C++ supporte le multi-paradigme (procédural, OO, générique, fonctionnel).

### Q2: Qu'est-ce qu'une référence et en quoi diffère-t-elle d'un pointeur ?
**Réponse:**
- **Référence:** Alias d'une variable existante, doit être initialisée à la déclaration, ne peut pas être null, ne peut pas être réassignée
- **Pointeur:** Variable contenant une adresse mémoire, peut être null, peut être réassigné, nécessite déréférencement (*)

### Q3: Expliquez les différents types de cast en C++
**Réponse:**
- **static_cast:** Conversions vérifiées à la compilation (int→float, upcasting)
- **dynamic_cast:** Conversions polymorphiques vérifiées à l'exécution (downcasting sécurisé)
- **const_cast:** Ajoute/retire const ou volatile
- **reinterpret_cast:** Conversion binaire brute (dangereux)

### Q4: Qu'est-ce que RAII ?
**Réponse:** Resource Acquisition Is Initialization. Principe où une ressource (mémoire, fichier, mutex) est acquise dans le constructeur et libérée dans le destructeur. Garantit la libération automatique même en cas d'exception.

### Q5: Quels sont les 4 piliers de la POO ?
**Réponse:**
1. **Encapsulation:** Regroupement données/méthodes, masquage de l'implémentation
2. **Abstraction:** Exposition des fonctionnalités essentielles
3. **Héritage:** Réutilisation du code via relations parent-enfant
4. **Polymorphisme:** Même interface, comportements différents

### Q6: Différence entre virtual et override ?
**Réponse:**
- **virtual:** Déclare une méthode pouvant être redéfinie (dans la classe de base)
- **override:** Indique explicitement qu'on redéfinit une méthode virtuelle (C++11), génère une erreur si la signature ne correspond pas

### Q7: Qu'est-ce qu'une classe abstraite ?
**Réponse:** Classe contenant au moins une fonction virtuelle pure (= 0). Ne peut pas être instanciée, sert d'interface. Les classes dérivées doivent implémenter toutes les méthodes pures pour être instanciables.

### Q8: Différence entre public, protected et private dans l'héritage ?
**Réponse:**
- **Public:** Membres publics restent publics, protected restent protected
- **Protected:** Membres publics et protected deviennent protected
- **Private:** Tous les membres deviennent private

### Q9: Qu'est-ce que le diamond problem ?
**Réponse:** Ambiguïté lors de l'héritage multiple quand deux classes parentes héritent d'une même classe grand-parent. Solution: héritage virtuel (`virtual public`).

### Q10: Différence entre stack et heap ?
**Réponse:**
- **Stack:** Allocation automatique, rapide, taille limitée, LIFO, variables locales
- **Heap:** Allocation manuelle (new/malloc), plus lent, taille flexible, gestion explicite

### Q11: Qu'est-ce qu'un memory leak ?
**Réponse:** Fuite mémoire quand de la mémoire allouée dynamiquement n'est jamais libérée. Cause: oubli de delete, exceptions avant delete, pointeurs perdus.

### Q12: Expliquez les smart pointers
**Réponse:**
- **unique_ptr:** Propriété exclusive, non copiable, movable
- **shared_ptr:** Propriété partagée, compteur de références
- **weak_ptr:** Référence faible sans incrémenter le compteur, évite les cycles

### Q13: Qu'est-ce que le Rule of Three/Five ?
**Réponse:**
- **Rule of Three:** Si vous définissez destructeur, copy constructor ou copy assignment, définissez les trois
- **Rule of Five:** Ajoute move constructor et move assignment (C++11)

### Q14: Différence entre new et malloc ?
**Réponse:**
- **new:** Opérateur C++, appelle constructeur, typé, exception si échec
- **malloc:** Fonction C, pas de constructeur, retourne void*, retourne NULL si échec

### Q15: Qu'est-ce que move semantics ?
**Réponse:** Permet de transférer les ressources d'un objet temporaire au lieu de les copier. Utilise rvalue references (&&). Améliore les performances en évitant les copies coûteuses.

### Q16: Qu'est-ce qu'une lambda ?
**Réponse:** Fonction anonyme définie inline. Syntaxe: `[capture](params) -> return_type { body }`

### Q17: Expliquez auto et decltype
**Réponse:**
- **auto:** Déduction automatique du type à la compilation
- **decltype:** Déclare une variable avec le même type qu'une expression

### Q18: Qu'est-ce que constexpr ?
**Réponse:** Indique qu'une fonction/variable peut être évaluée à la compilation. Plus puissant que const. Permet l'optimisation et la métaprogrammation.

### Q19: Qu'est-ce qu'un variadic template ?
**Réponse:** Template acceptant un nombre variable d'arguments. Utilise l'ellipse (...) et la récursion ou fold expressions.

### Q20: Différence entre vector et list ?
**Réponse:**
- **vector:** Tableau dynamique, accès aléatoire O(1), insertion/suppression fin O(1), milieu O(n)
- **list:** Liste doublement chaînée, pas d'accès aléatoire, insertion/suppression O(1) partout

### Q21: Différence entre map et unordered_map ?
**Réponse:**
- **map:** Arbre rouge-noir, éléments triés, O(log n) pour accès/insertion
- **unordered_map:** Table de hachage, pas d'ordre, O(1) moyen pour accès/insertion

### Q22: Qu'est-ce qu'un itérateur ?
**Réponse:** Objet permettant de parcourir les éléments d'un conteneur. Types: input, output, forward, bidirectional, random access. Utilise les opérateurs ++, --, *, ->.

### Q23: Expliquez std::move et std::forward
**Réponse:**
- **std::move:** Cast en rvalue reference, permet le déplacement
- **std::forward:** Perfect forwarding, préserve la value category (lvalue/rvalue) dans les templates

### Q24: Qu'est-ce que la copie profonde vs copie superficielle ?
**Réponse:**
- **Superficielle:** Copie les valeurs des membres, y compris les pointeurs (même adresse)
- **Profonde:** Copie les données pointées, créant de nouvelles allocations mémoire

### Q25: Expliquez le mot-clé mutable
**Réponse:** Permet de modifier un membre dans une méthode const. Utile pour les caches, compteurs, mutex dans des objets logiquement constants.

### Q26: Qu'est-ce que le template specialization ?
**Réponse:** Définir une implémentation spécifique pour certains types. Partielle ou complète.

### Q27: Différence entre struct et class ?
**Réponse:** Seule différence: membres et héritage **public** par défaut pour struct, **private** pour class. Convention: struct pour POD (Plain Old Data), class pour objets avec comportement.

### Q28: Qu'est-ce que SFINAE ?
**Réponse:** Substitution Failure Is Not An Error. Si la substitution d'un template échoue, le compilateur continue avec d'autres candidats au lieu de générer une erreur. Base de std::enable_if.

### Q29: Expliquez volatile
**Réponse:** Indique au compilateur qu'une variable peut changer en dehors du contrôle du programme (hardware, multithreading). Empêche certaines optimisations. **Ne garantit PAS la thread-safety** (utiliser std::atomic).

### Q30: Qu'est-ce qu'un functor ?
**Réponse:** Objet-fonction, classe avec operator() surchargé. Peut maintenir un état contrairement aux fonctions. Utilisé avec les algorithmes STL.

### Q31: Comment créer un thread en C++ ?
**Réponse:**
```cpp
#include <thread>
void task() { /* ... */ }
std::thread t(task);
t.join();  // Ou t.detach()
```

### Q32: Différence entre join et detach ?
**Réponse:**
- **join():** Attend la fin du thread, bloquant
- **detach():** Thread s'exécute indépendamment, ne peut plus être joint

### Q33: Qu'est-ce qu'un mutex ?
**Réponse:** Mutual exclusion, verrou pour protéger les ressources partagées. Types: mutex, recursive_mutex, timed_mutex, shared_mutex.

### Q34: Différence entre lock_guard et unique_lock ?
**Réponse:**
- **lock_guard:** Simple, toujours verrouillé, pas de déverrouillage manuel
- **unique_lock:** Flexible, peut lock/unlock, defer locking, utilisable avec condition_variable

### Q35: Qu'est-ce qu'une condition variable ?
**Réponse:** Mécanisme de synchronisation permettant à des threads d'attendre qu'une condition soit remplie. Utilisé avec unique_lock pour éviter busy waiting.

### Q36: Expliquez std::atomic
**Réponse:** Type garantissant les opérations atomiques sans data race. Pas besoin de mutex pour les opérations simples. Utilise des instructions processeur spéciales.

### Q37: Expliquez le pattern Singleton
**Réponse:** Une seule instance d'une classe dans tout le programme. Implémentation thread-safe avec static local (C++11).

### Q38: Qu'est-ce que le pattern Factory ?
**Réponse:** Crée des objets sans spécifier leur classe exacte. Utilise une méthode qui retourne un pointeur vers classe de base.

### Q39: Expliquez le pattern Observer
**Réponse:** Définit une dépendance un-à-plusieurs où quand un objet change d'état, tous ses dépendants sont notifiés. Utilisé pour les événements, GUI.

### Q40: Comment débugger un segmentation fault ?
**Réponse:**
- Utiliser gdb/lldb avec backtrace
- Valgrind pour détecter accès mémoire invalides
- AddressSanitizer (-fsanitize=address)
- Vérifier: déréférencement null, buffer overflow, use-after-free

### Q41: Qu'est-ce que le cache locality ?
**Réponse:** Principe où accéder à des données proches en mémoire améliore les performances grâce au cache CPU. Préférer structures contiguës (vector vs list).

### Q42: Comment optimiser les performances en C++ ?
**Réponse:**
- Éviter copies inutiles (move semantics, références)
- Inline pour petites fonctions
- Reserve sur conteneurs si taille connue
- Algorithmes O(n) vs O(n²)
- Profiling pour identifier bottlenecks
- Éviter allocations dynamiques répétées

### Q43: Qu'est-ce que RVO/NRVO ?
**Réponse:** Return Value Optimization / Named RVO. Le compilateur élimine les copies lors du retour d'objets, construisant directement l'objet à sa destination.

### Q44: Que fait ce code ?
```cpp
int* p = new int[10];
delete p;  // ❌ BUG!
```
**Réponse:** Undefined behavior! Doit utiliser `delete[] p` pour un tableau.

### Q45: Problème dans ce code ?
```cpp
class Base {
    ~Base() {}  // ❌ Non-virtual!
};
Base* p = new Derived();
delete p;  // Fuite mémoire, ~Derived() pas appelé
```
**Réponse:** Destructeur doit être virtual pour permettre le polymorphisme.

### Q46: Que vaut x ?
```cpp
int x = 5;
int& ref = ++x;
int y = x++ + ++x;
```
**Réponse:** Undefined behavior! Modification multiple de x entre sequence points.

### Q47: Différence entre i++ et ++i ?
**Réponse:**
- **++i:** Pré-incrémentation, retourne valeur après incrément (plus efficace)
- **i++:** Post-incrémentation, retourne valeur avant incrément (copie temporaire)

### Q48: Que produit sizeof(char) ?
**Réponse:** Toujours 1 par définition C++. sizeof est en unités de sizeof(char).

### Q49: Comment géreriez-vous une codebase legacy ?
**Réponse:**
- Comprendre avant modifier (documentation, tests)
- Tests unitaires pour sécuriser refactoring
- Refactoring incrémental
- Améliorer progressivement (modern C++, smart pointers)
- Code review systématique

### Q50: Expliquez un bug complexe que vous avez résolu
**Réponse:** Prépare un exemple concret avec:
- Contexte et symptômes
- Méthode de debugging
- Cause racine
- Solution implémentée
- Leçons apprises

---

## 🔥 Concepts Avancés (Q51-Q125)

### Q51: Qu'est-ce que name mangling ?
**Réponse:** Le compilateur C++ modifie les noms de fonctions pour inclure types des paramètres, permettant la surcharge. Pour interfacer avec C, utiliser `extern "C"` qui désactive le mangling.

### Q52: Différence entre définition et déclaration ?
**Réponse:**
- **Déclaration:** Annonce l'existence (peut être répétée)
- **Définition:** Alloue la mémoire/fournit l'implémentation (une seule fois)

### Q53: Qu'est-ce que le ODR (One Definition Rule) ?
**Réponse:** Une entité (variable, fonction, classe) ne peut avoir qu'une seule définition dans tout le programme. Les violations causent des erreurs de linkage. Exception: templates et inline.

### Q54: Expliquez les storage class specifiers
**Réponse:**
- **auto:** Durée automatique (par défaut pour locales)
- **static:** Durée du programme, linkage interne pour globales
- **extern:** Déclare sans définir, linkage externe
- **thread_local:** Une instance par thread

### Q55: Qu'est-ce qu'un aggregate type ?
**Réponse:** Type pouvant être initialisé avec aggregate initialization: tableau ou classe sans constructeur user-defined, membres private/protected, classe de base, fonctions virtuelles.

### Q56: Différence entre narrow et wide characters ?
**Réponse:**
- **char:** 1 byte, ASCII/UTF-8
- **wchar_t:** 2-4 bytes selon plateforme
- **char16_t, char32_t:** UTF-16, UTF-32 (C++11)
- **char8_t:** UTF-8 (C++20)

### Q57: Qu'est-ce que le temporary object ?
**Réponse:** Objet créé par le compilateur sans nom, détruit à la fin de l'expression complète. Les rvalue references (&&) peuvent prolonger leur durée de vie.

### Q58: Expliquez l'ordre d'initialisation des membres
**Réponse:** Les membres sont initialisés dans l'ordre de leur déclaration dans la classe, PAS dans l'ordre de la liste d'initialisation du constructeur. Attention aux dépendances !

### Q59: Qu'est-ce que le covariant return type ?
**Réponse:** Une méthode virtuelle peut retourner un type dérivé de celui de la méthode de base.

### Q60: Expliquez l'héritage privé
**Réponse:** "Implémenté en termes de". La classe dérivée peut utiliser l'interface de la base mais ne l'expose pas publiquement. Alternative à la composition.

### Q61: Qu'est-ce que l'empty base optimization (EBO) ?
**Réponse:** Le compilateur peut optimiser une classe de base vide pour qu'elle n'occupe pas d'espace dans la classe dérivée. Utilisé intensivement dans la STL.

### Q62: Différence entre overload, override et overwrite ?
**Réponse:**
- **Overload:** Même nom, signatures différentes, même scope
- **Override:** Redéfinition d'une méthode virtuelle
- **Overwrite/Hide:** Méthode non-virtuelle dans classe dérivée masque celle de base

### Q63: Qu'est-ce qu'un interface en C++ ?
**Réponse:** Classe abstraite pure avec uniquement des fonctions virtuelles pures et un destructeur virtuel. Pas de membres données. Convention: préfixe I (IDrawable).

### Q64: Expliquez le slicing problem
**Réponse:** Quand on assigne un objet dérivé à un objet de base par valeur, seule la partie base est copiée. Solution: utiliser pointeurs/références polymorphiques.

### Q65: Qu'est-ce que le Liskov Substitution Principle ?
**Réponse:** Un objet de classe dérivée doit pouvoir remplacer un objet de classe de base sans casser le programme. Principe SOLID fondamental.

### Q66: Qu'est-ce que le placement new ?
**Réponse:** Construit un objet à une adresse mémoire spécifiée (déjà allouée). Utile pour pools mémoire, shared memory.

### Q67: Différence entre shallow copy et memberwise copy ?
**Réponse:** Memberwise copy copie tous les membres un par un (comportement par défaut). Shallow copy est un memberwise copy qui copie les pointeurs (même adresse). Deep copy alloue et copie le contenu pointé.

### Q68: Qu'est-ce que le memory alignment ?
**Réponse:** Les variables doivent être stockées à des adresses multiples de leur taille pour des performances optimales. `alignof()` donne l'alignement, `alignas()` le spécifie.

### Q69: Expliquez le concept de memory pool
**Réponse:** Pré-allocation d'un bloc mémoire pour éviter des allocations/désallocations fréquentes. Améliore performances et réduit fragmentation. Custom allocators STL.

### Q70: Qu'est-ce qu'un dangling pointer ?
**Réponse:** Pointeur vers mémoire libérée ou hors scope. L'utiliser = undefined behavior. Les smart pointers résolvent ce problème.

### Q71: Différence entre stack unwinding et stack overflow ?
**Réponse:**
- **Stack unwinding:** Processus de nettoyage lors d'une exception (destructeurs appelés)
- **Stack overflow:** Dépassement de taille de pile (récursion infinie, trop de variables locales)

### Q72: Qu'est-ce que le copy elision ?
**Réponse:** Optimisation où le compilateur élimine les copies/déplacements inutiles. RVO en fait partie. Obligatoire en C++17 pour certains cas.

### Q73: Qu'est-ce que perfect forwarding ?
**Réponse:** Technique pour transmettre des arguments à une autre fonction en préservant leur value category (lvalue/rvalue). Utilise `std::forward` et templates avec `T&&`.

### Q74: Expliquez std::optional (C++17)
**Réponse:** Conteneur pouvant contenir une valeur ou être vide. Alternative sûre à retourner nullptr ou des valeurs sentinelles.

### Q75: Qu'est-ce que std::variant (C++17) ?
**Réponse:** Union type-safe. Peut contenir une valeur parmi plusieurs types. Alternative à `union`.

### Q76: Expliquez std::any (C++17)
**Réponse:** Conteneur type-safe pour n'importe quel type copiable. Type-erasure. Utilise `any_cast` pour récupérer la valeur.

### Q77: Qu'est-ce qu'un structured binding (C++17) ?
**Réponse:** Décomposition d'objets structurés en variables nommées.

### Q78: Expliquez if constexpr (C++17)
**Réponse:** Conditionnel évalué à la compilation. Branches non prises ne sont pas compilées. Essentiel pour la métaprogrammation.

### Q79: Qu'est-ce qu'un fold expression (C++17) ?
**Réponse:** Syntaxe pour réduire un parameter pack avec un opérateur.

### Q80: Expliquez les concepts (C++20)
**Réponse:** Contraintes nommées sur les templates. Remplacent SFINAE avec syntaxe claire.

### Q81: Qu'est-ce que std::span (C++20) ?
**Réponse:** Vue non-propriétaire sur une séquence contiguë. Léger, ne copie pas. Alternative à passer pointeur+taille.

### Q82: Expliquez les modules (C++20)
**Réponse:** Alternative aux headers. Compilation plus rapide, pas de pollution de namespace, ordre d'import indépendant.

### Q83: Qu'est-ce que les ranges (C++20) ?
**Réponse:** Abstraction pour séquences. Pipelines de transformations paresseuses. Compose algorithmes.

### Q84: Expliquez std::format (C++20)
**Réponse:** Alternative type-safe à printf. Syntaxe Python-like.

### Q85: Différence entre emplace_back et push_back ?
**Réponse:**
- **push_back:** Copie ou déplace un objet existant
- **emplace_back:** Construit l'objet in-place avec les arguments fournis

### Q86: Qu'est-ce qu'un allocator ?
**Réponse:** Objet gérant allocation/désallocation mémoire pour conteneurs STL. Permet custom memory management. Template parameter des conteneurs.

### Q87: Expliquez std::priority_queue
**Réponse:** File de priorité basée sur un heap. Element maximum (par défaut) accessible en O(1), insertion/suppression en O(log n).

### Q88: Différence entre set et multiset ?
**Réponse:**
- **set:** Éléments uniques, triés
- **multiset:** Permet doublons, triés

### Q89: Qu'est-ce que std::tuple ?
**Réponse:** Conteneur de taille fixe pour éléments de types hétérogènes. Accès par `std::get<N>()`.

### Q90: Expliquez std::pair
**Réponse:** Tuple de 2 éléments. Utilisé par map (key-value). Membres: `first`, `second`. `std::make_pair` pour création.

### Q91: Qu'est-ce qu'un adapter container ?
**Réponse:** Conteneur basé sur un autre (stack, queue, priority_queue). Restreint l'interface d'un conteneur sous-jacent.

### Q92: Différence entre find et binary_search ?
**Réponse:**
- **find:** O(n), linéaire, pas besoin que ce soit trié
- **binary_search:** O(log n), nécessite séquence triée, retourne bool
- **lower_bound/upper_bound:** O(log n), retourne itérateur

### Q93: Qu'est-ce que std::function ?
**Réponse:** Wrapper type-erasé pour n'importe quel callable (fonction, lambda, functor). Permet stocker et passer des callables.

### Q94: Expliquez std::bind
**Réponse:** Crée une fonction avec certains arguments pré-liés. Moins utilisé depuis les lambdas C++11.

### Q95: Qu'est-ce que le template template parameter ?
**Réponse:** Template qui prend un autre template comme paramètre.

### Q96: Expliquez la spécialisation partielle
**Réponse:** Spécialiser un template pour un sous-ensemble de types.

### Q97: Qu'est-ce que le template metaprogramming ?
**Réponse:** Calculs effectués à la compilation via templates. Exemple classique: factorielle.

### Q98: Expliquez std::enable_if
**Réponse:** SFINAE helper pour activer/désactiver templates selon conditions.

### Q99: Qu'est-ce que le dependent name ?
**Réponse:** Nom dépendant d'un paramètre template. Nécessite `typename` ou `template` pour lever ambiguïté.

### Q100: Expliquez le two-phase lookup
**Réponse:** Les templates sont analysés en 2 phases: à la définition (non-dependent names) et à l'instantiation (dependent names). Cause des erreurs subtiles.

### Q101: Qu'est-ce qu'un deadlock ?
**Réponse:** Situation où 2+ threads attendent mutuellement des ressources. Prévention: ordre cohérent d'acquisition, std::lock pour multiple mutex.

### Q102: Expliquez std::future et std::promise
**Réponse:**
- **promise:** Écrit une valeur dans un canal de communication
- **future:** Lit la valeur (bloquant jusqu'à disponibilité)

### Q103: Qu'est-ce que std::async ?
**Réponse:** Lance une tâche asynchrone, retourne future. Peut être exécuté dans un nouveau thread ou différé.

### Q104: Différence entre mutex et spinlock ?
**Réponse:**
- **Mutex:** Thread dort si verrou occupé (context switch)
- **Spinlock:** Thread boucle activement (busy-waiting). Mieux pour courtes sections critiques.

### Q105: Qu'est-ce qu'une race condition ?
**Réponse:** Plusieurs threads accèdent concurremment à une ressource partagée, au moins un écrit, sans synchronisation. Résultat non-déterministe.

### Q106: Expliquez std::shared_mutex (C++17)
**Réponse:** Mutex avec lecture partagée. Plusieurs lecteurs simultanés OU un seul écrivain. Pattern readers-writer lock.

### Q107: Qu'est-ce que le memory ordering ?
**Réponse:** Contraintes sur l'ordre d'exécution des opérations mémoire. `std::memory_order`: relaxed, acquire, release, seq_cst. Important pour atomics.

### Q108: Expliquez le false sharing
**Réponse:** Quand threads modifient des variables sur la même cache line, causant invalidations fréquentes. Solution: padding, alignas.

### Q109: Qu'est-ce que l'undefined behavior (UB) ?
**Réponse:** Comportement non spécifié par le standard. Exemples: déréférencement null, accès out-of-bounds, signed integer overflow, utilisation après libération.

### Q110: Qu'est-ce que Valgrind ?
**Réponse:** Outil pour détecter memory leaks, accès invalides, race conditions. Memcheck pour mémoire, Helgrind pour threads.

### Q111: Expliquez les sanitizers
**Réponse:**
- **AddressSanitizer:** Détecte memory errors
- **ThreadSanitizer:** Détecte race conditions
- **UndefinedBehaviorSanitizer:** Détecte UB

### Q112: Qu'est-ce que le const correctness ?
**Réponse:** Marquer const tout ce qui ne doit pas être modifié. Méthodes const, paramètres const&. Aide compilateur et documentation.

### Q113: Différence entre assert et static_assert ?
**Réponse:**
- **assert:** Runtime, peut être désactivé (NDEBUG)
- **static_assert:** Compile-time, toujours vérifié

### Q114: Qu'est-ce que RTTI ?
**Réponse:** Run-Time Type Information. `typeid()` et `dynamic_cast`. Overhead de performance. Peut être désactivé (-fno-rtti).

### Q115: Expliquez le principe DRY
**Réponse:** Don't Repeat Yourself. Éviter duplication de code. Utiliser fonctions, templates, héritage pour réutiliser.

### Q116: Qu'est-ce que le SOLID en C++ ?
**Réponse:**
- **S**ingle Responsibility
- **O**pen/Closed
- **L**iskov Substitution
- **I**nterface Segregation
- **D**ependency Inversion

### Q117: Que fait ce code ?
```cpp
std::vector<bool> v = {true, false};
bool& b = v[0];  // ❌ Erreur!
```
**Réponse:** `vector<bool>` est spécialisé, stocke des bits. Ne retourne pas de vraie référence. Utiliser `vector<char>`.

### Q118: Problème ?
```cpp
class Base { 
    public: Base() { init(); } 
    virtual void init() {} 
};
```
**Réponse:** `init()` appelé pendant construction de Base, avant que Derived existe. Virtual dispatch ne marche pas dans constructeur.

### Q119: Que vaut sizeof(Empty) ?
**Réponse:** Minimum 1 byte. Classes vides doivent avoir taille > 0 pour avoir des adresses distinctes.

### Q120: Pourquoi ce code compile ?
```cpp
template<typename T>
void foo(T t) {
    t.non_existent_method();
}
```
**Réponse:** Templates compilés uniquement si instanciés. Erreurs dans code non-instancié ignorées.

### Q121: Expliquez le pattern CRTP
**Réponse:** Curiously Recurring Template Pattern. Classe dérive d'un template paramétré par elle-même. Polymorphisme statique sans virtual.

### Q122: Qu'est-ce que le pattern Strategy ?
**Réponse:** Définit une famille d'algorithmes interchangeables. Encapsule chaque algorithme, les rend interchangeables.

### Q123: Expliquez le pattern PIMPL
**Réponse:** Pointer to Implementation. Idiome pour cacher implémentation. Pointeur vers struct interne. Réduit dépendances de compilation.

### Q124: Qu'est-ce que le pattern Builder ?
**Réponse:** Construit objet complexe étape par étape. Sépare construction de représentation. Utile pour objets avec nombreux paramètres optionnels.

### Q125: Expliquez le pattern Adapter
**Réponse:** Convertit interface d'une classe vers celle attendue. Wrapper permettant collaboration de classes incompatibles.

---

## 🔨 Build Systems & Compilation (Q126-Q175)

### Q126: Qu'est-ce que CMake ?
**Réponse:** Générateur de build system multiplateforme. Crée des Makefiles, projets Visual Studio, Ninja, etc. Utilise CMakeLists.txt pour définir le projet.

### Q127: Différence entre add_executable et add_library ?
**Réponse:**
- **add_executable:** Crée un exécutable
- **add_library:** Crée une bibliothèque (STATIC, SHARED, MODULE, INTERFACE)

### Q128: Qu'est-ce que target_link_libraries ?
**Réponse:** Lie des bibliothèques à une cible (executable/library). Gère automatiquement les dépendances transitives.

### Q129: Expliquez PUBLIC, PRIVATE, INTERFACE dans CMake
**Réponse:**
- **PRIVATE:** Usage interne uniquement
- **PUBLIC:** Usage interne + exposé aux dépendants
- **INTERFACE:** Pas d'usage interne, seulement pour dépendants

### Q130: Qu'est-ce que find_package ?
**Réponse:** Cherche et configure une bibliothèque externe. Utilise des modules Find*.cmake ou des fichiers *Config.cmake.

### Q131: Différence entre ${VAR} et $<EXPRESSION> ?
**Réponse:**
- **${VAR}:** Variable CMake, évaluée pendant configuration
- **$<EXPRESSION>:** Generator expression, évaluée pendant génération

### Q132: Qu'est-ce que CMAKE_BUILD_TYPE ?
**Réponse:** Type de build: Debug, Release, RelWithDebInfo, MinSizeRel. Affecte flags de compilation et optimisation.

### Q133: Expliquez set_target_properties
**Réponse:** Configure propriétés d'une cible: standard C++, version, output directory, etc.

### Q134: Qu'est-ce qu'un CMake module ?
**Réponse:** Fichier .cmake réutilisable contenant fonctions/macros. Peut être inclus avec `include()`.

### Q135: Comment gérer les dépendances externes avec CMake ?
**Réponse:**
- **find_package:** Packages système
- **FetchContent:** Télécharge depuis git/url (CMake 3.11+)
- **ExternalProject:** Build des projets externes
- **add_subdirectory:** Sous-projets locaux

### Q136: Qu'est-ce qu'un Makefile ?
**Réponse:** Fichier décrivant règles de build. Format: `target: dependencies` puis commandes. `make` exécute les règles nécessaires.

### Q137: Qu'est-ce qu'une phony target ?
**Réponse:** Target qui n'est pas un fichier réel. Empêche conflicts avec fichiers du même nom. Déclarée avec `.PHONY:`.

### Q138: Expliquez les variables automatiques dans Make
**Réponse:**
- **$@:** Nom de la target
- **$<:** Premier prerequisite
- **$^:** Tous les prerequisites
- **$?:** Prerequisites plus récents que target

### Q139: Différence entre = et := dans Makefile ?
**Réponse:**
- **=:** Évaluation paresseuse (à l'utilisation)
- **:=:** Évaluation immédiate (à l'assignation)

### Q140: Qu'est-ce que make -j ?
**Réponse:** Compilation parallèle. `-j4` utilise 4 threads. `-j` sans nombre utilise tous les cores. Accélère le build.

### Q141: Quelles sont les 4 étapes de compilation en C++ ?
**Réponse:**
1. **Preprocessing:** Résout #include, #define, #ifdef → .i
2. **Compilation:** Code C++ → assembleur → .s
3. **Assemblage:** Assembleur → code machine → .o
4. **Linking:** Combine .o + libraries → exécutable

### Q142: Qu'est-ce qu'un header guard ?
**Réponse:** Empêche l'inclusion multiple d'un même header. `#ifndef/#define/#endif` ou `#pragma once`.

### Q143: Différence entre #include <> et #include "" ?
**Réponse:**
- **< >:** Cherche dans system include paths
- **" ":** Cherche d'abord localement, puis system paths

### Q144: Qu'est-ce qu'une compilation unit ?
**Réponse:** Fichier source (.cpp) après preprocessing. Chaque .cpp est compilé indépendamment en .o.

### Q145: Expliquez les flags -O0, -O1, -O2, -O3, -Os
**Réponse:**
- **-O0:** Pas d'optimisation (debug)
- **-O1:** Optimisations basiques
- **-O2:** Optimisations recommandées
- **-O3:** Optimisations agressives
- **-Os:** Optimise la taille

### Q146: Qu'est-ce que -g et -ggdb ?
**Réponse:**
- **-g:** Ajoute symboles de debug
- **-ggdb:** Format optimisé pour GDB
- **-g3:** Inclut macros

### Q147: Expliquez -Wall, -Wextra, -Werror
**Réponse:**
- **-Wall:** Active warnings communs
- **-Wextra:** Warnings additionnels
- **-Werror:** Traite warnings comme erreurs
- **-pedantic:** Strict conformité standard

### Q148: Qu'est-ce que -fPIC ?
**Réponse:** Position Independent Code. Nécessaire pour bibliothèques partagées (.so). Code chargeable à n'importe quelle adresse.

### Q149: Différence entre bibliothèque statique et dynamique ?
**Réponse:**
- **Statique (.a/.lib):** Copiée dans l'exécutable, pas de dépendance runtime
- **Dynamique (.so/.dll):** Chargée au runtime, partagée entre programmes

### Q150: Qu'est-ce que le linking ?
**Réponse:** Phase finale combinant object files et bibliothèques. Résout symboles. Deux types: statique et dynamique.

### Q151: Qu'est-ce qu'un symbole undefined reference ?
**Réponse:** Erreur de linking: fonction/variable déclarée mais pas définie. Causes: bibliothèque manquante, ordre incorrect.

### Q152: Expliquez nm, objdump, ldd
**Réponse:**
- **nm:** Liste symboles d'un binaire
- **objdump:** Désassemble object files
- **ldd:** Affiche dépendances de bibliothèques partagées

### Q153: Qu'est-ce que le link-time optimization (LTO) ?
**Réponse:** Optimisation inter-fichiers pendant le linking. Flag: `-flto`. Améliore performances mais ralentit build.

### Q154: Différence entre forward declaration et include ?
**Réponse:**
- **Forward declaration:** Déclare l'existence sans définition
- **Include:** Inclut définition complète

Forward declaration réduit dépendances de compilation.

### Q155: Qu'est-ce qu'une precompiled header (PCH) ?
**Réponse:** Header compilé une fois, réutilisé pour accélérer compilation. Utile pour gros headers stables (STL, Boost).

### Q156: Qu'est-ce que Boost ?
**Réponse:** Collection de bibliothèques C++ peer-reviewed de haute qualité. Berceau de fonctionnalités devenues standard.

### Q157: Expliquez boost::shared_ptr vs std::shared_ptr
**Réponse:** boost::shared_ptr était avant C++11. std::shared_ptr est le standard maintenant. Préférer std::shared_ptr.

### Q158: Qu'est-ce que Boost.Asio ?
**Réponse:** Bibliothèque pour I/O asynchrone, networking. Gère sockets, timers, serial ports. Base pour networking cross-platform.

### Q159: Qu'est-ce que Boost.Spirit ?
**Réponse:** Framework pour créer parsers et generators en C++. Utilise expression templates. Alternative à lex/yacc.

### Q160: Expliquez Boost.Filesystem
**Réponse:** Manipulation de fichiers/répertoires cross-platform. Devenu std::filesystem en C++17.

### Q161: Qu'est-ce que Boost.MultiIndex ?
**Réponse:** Conteneur avec plusieurs index simultanés. Peut indexer par valeur, hash, ordre, etc.

### Q162: Qu'est-ce que Qt ?
**Réponse:** Framework C++ pour applications GUI cross-platform. Inclut widgets, networking, database, multimedia. Système signal/slot.

### Q163: Expliquez le système signal/slot de Qt
**Réponse:** Mécanisme de communication entre objets. Signal émis → slots connectés appelés. Découplage loose.

### Q164: Qu'est-ce que le MOC (Meta-Object Compiler) ?
**Réponse:** Préprocesseur Qt générant code pour signals/slots, introspection. Parse macro Q_OBJECT.

### Q165: Différence entre QWidget et QObject ?
**Réponse:**
- **QObject:** Classe de base, signal/slot, pas de GUI
- **QWidget:** Hérite QObject, élément visuel

### Q166: Qu'est-ce que QString vs std::string ?
**Réponse:** QString utilise UTF-16 (Unicode), std::string bytes. QString mieux intégré à Qt.

### Q167: Expliquez QSharedPointer vs std::shared_ptr
**Réponse:** QSharedPointer est l'implémentation Qt (pré-C++11). Préférer std::shared_ptr maintenant.

### Q168: Qu'est-ce que Google Test (gtest) ?
**Réponse:** Framework de test unitaire C++. Macros: EXPECT_*, ASSERT_*, TEST(), TEST_F().

### Q169: Qu'est-ce que Google Benchmark ?
**Réponse:** Bibliothèque de microbenchmarking. Mesure performances précises, gère répétitions statistiques.

### Q170: Expliquez Eigen
**Réponse:** Bibliothèque d'algèbre linéaire: matrices, vecteurs. Header-only, très optimisée, utilisée en ML/robotique.

### Q171: Qu'est-ce que nlohmann/json ?
**Réponse:** Bibliothèque JSON moderne header-only. API intuitive type-safe.

### Q172: Qu'est-ce que spdlog ?
**Réponse:** Bibliothèque de logging rapide header-only. Formatage, rotation, multithreading, niveaux.

### Q173: Qu'est-ce que fmt (fmtlib) ?
**Réponse:** Bibliothèque de formatage type-safe, base de std::format (C++20). Plus rapide que iostreams.

### Q174: Qu'est-ce que MVC ?
**Réponse:** Model-View-Controller. Séparation: Model (données), View (présentation), Controller (logique).

### Q175: Expliquez le pattern Repository
**Réponse:** Abstraction pour accès aux données. Découple logique métier de la persistence. Interface pour CRUD.

---

## 🏛️ Architecture Logicielle (Q176-Q200)

### Q176: Qu'est-ce que l'injection de dépendances ?
**Réponse:** Fournir dépendances depuis l'extérieur plutôt que les créer en interne. Facilite tests et découplage.

### Q177: Expliquez le pattern Command
**Réponse:** Encapsule requête comme objet. Permet undo/redo, queuing, logging.

### Q178: Qu'est-ce que CQRS ?
**Réponse:** Command Query Responsibility Segregation. Sépare opérations lecture (queries) et écriture (commands).

### Q179: Expliquez le pattern Event Sourcing
**Réponse:** Persister changements d'état comme séquence d'événements. État reconstruit en rejouant événements.

### Q180: Qu'est-ce qu'une architecture hexagonale ?
**Réponse:** Ports & Adapters. Core application isolé via interfaces (ports). Adapters implémentent les ports.

### Q181: Expliquez le pattern Facade
**Réponse:** Interface simplifiée pour système complexe. Cache complexité sous-jacente.

### Q182: Qu'est-ce que le pattern Proxy ?
**Réponse:** Substitut contrôlant l'accès à un objet. Types: virtual, protection, remote, smart pointers.

### Q183: Expliquez le pattern Decorator
**Réponse:** Ajoute responsabilités dynamiquement. Composition plutôt qu'héritage.

### Q184: Qu'est-ce que le pattern Chain of Responsibility ?
**Réponse:** Chaîne de handlers. Requête passe dans la chaîne jusqu'à traitement.

### Q185: Expliquez le principe ISP
**Réponse:** Interface Segregation Principle. Pas de dépendances vers méthodes non-utilisées. Petites interfaces spécifiques.

### Q186: Qu'est-ce que le dependency inversion principle ?
**Réponse:** Dépendre d'abstractions, pas de concrétions. High-level et low-level modules dépendent d'abstractions.

### Q187: Expliquez le pattern Template Method
**Réponse:** Définit squelette d'algorithme, sous-classes redéfinissent certaines étapes.

### Q188: Qu'est-ce qu'une architecture en couches ?
**Réponse:** Organisation en couches avec dépendances unidirectionnelles. Ex: Présentation → Logique → Données → DB.

### Q189: Expliquez le pattern Service Locator
**Réponse:** Registry central pour obtenir services. Alternative à DI. Critique: dépendances cachées.

### Q190: Qu'est-ce que le pattern Memento ?
**Réponse:** Sauvegarde/restaure état interne sans violer encapsulation. Utilisé pour undo/redo.

### Q191: Expliquez le principe Open/Closed
**Réponse:** Ouvert à l'extension, fermé à la modification. Ajouter comportement sans modifier code existant.

### Q192: Microservices vs monolithique ?
**Réponse:**
- **Monolithique:** Application unique
- **Microservices:** Services indépendants, déployés séparément

### Q193: Expliquez le pattern State
**Réponse:** Objet change comportement selon état interne. Encapsule états comme objets.

### Q194: Qu'est-ce que coupling et cohésion ?
**Réponse:**
- **Coupling:** Dépendance entre modules (viser faible)
- **Cohésion:** Relation dans un module (viser forte)

### Q195: Expliquez le pattern Plugin
**Réponse:** Architecture permettant d'étendre fonctionnalité via modules chargés dynamiquement. Utilise DLLs/SOs.

### Q196: Qu'est-ce que la programmation défensive ?
**Réponse:** Valider inputs, gérer erreurs, vérifier assumptions. Anticiper l'inattendu. Asserts, exceptions, logging.

### Q197: Expliquez le pattern Null Object
**Réponse:** Objet avec comportement "ne rien faire". Évite vérifications null répétées.

### Q198: Qu'est-ce qu'une API RESTful ?
**Réponse:** Representational State Transfer. Stateless, resources (URLs), HTTP methods (GET, POST, PUT, DELETE).

### Q199: Expliquez le principe SRP
**Réponse:** Single Responsibility Principle. Une classe devrait avoir une seule raison de changer.

### Q200: Qu'est-ce que le technical debt ?
**Réponse:** Coût futur de raccourcis/mauvais choix actuels. S'accumule, ralentit développement. Nécessite refactoring.

---

## 🎓 Résumé & Conseils

**200 questions complètes couvrant :**
- ✅ Fondamentaux C++ (Q1-Q50)
- ✅ Concepts avancés (Q51-Q125)
- ✅ Build Systems & Compilation (Q126-Q155)
- ✅ Bibliothèques tierces (Q156-Q173)
- ✅ Architecture logicielle (Q174-Q200)

**Stratégie de révision :**
1. **Jour 1-2:** Q1-50 (fondamentaux)
2. **Jour 3-4:** Q51-125 (avancés)
3. **Jour 5:** Q126-155 (tooling)
4. **Jour 6:** Q156-200 (libs & archi)
5. **Jour 7:** Mock interview + révision

**Questions à poser au tech lead :**
- Quelle version de C++ utilisez-vous ?
- Quel est votre build system ?
- Comment gérez-vous les tests ?
- Quelle est votre stack technique ?
- Pratiques de code review ?

Bonne chance ! 🚀💪
