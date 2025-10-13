# Questions d'Entretien C++ - Guide Complet

## 🎯 Fondamentaux C++

### Q1: Quelle est la différence entre C et C++ ?
**Réponse:** C++ est une extension du C qui ajoute la programmation orientée objet, les templates, les exceptions, la STL, les namespaces, et une gestion mémoire plus sophistiquée avec new/delete. C++ supporte le multi-paradigme (procédural, OO, générique, fonctionnel).

### Q2: Qu'est-ce qu'une référence et en quoi diffère-t-elle d'un pointeur ?
**Réponse:**
- **Référence:** Alias d'une variable existante, doit être initialisée à la déclaration, ne peut pas être null, ne peut pas être réassignée
- **Pointeur:** Variable contenant une adresse mémoire, peut être null, peut être réassigné, nécessite déréférencement (*)

```cpp
int x = 10;
int& ref = x;    // référence
int* ptr = &x;   // pointeur
```

### Q3: Expliquez les différents types de cast en C++
**Réponse:**
- **static_cast:** Conversions vérifiées à la compilation (int→float, upcasting)
- **dynamic_cast:** Conversions polymorphiques vérifiées à l'exécution (downcasting sécurisé)
- **const_cast:** Ajoute/retire const ou volatile
- **reinterpret_cast:** Conversion binaire brute (dangereux)

### Q4: Qu'est-ce que RAII ?
**Réponse:** Resource Acquisition Is Initialization. Principe où une ressource (mémoire, fichier, mutex) est acquise dans le constructeur et libérée dans le destructeur. Garantit la libération automatique même en cas d'exception.

```cpp
class FileHandler {
    std::ifstream file;
public:
    FileHandler(const std::string& name) : file(name) {}
    ~FileHandler() { /* file.close() automatique */ }
};
```

## 🏗️ Orienté Objet

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

```cpp
class Base {
    virtual void foo() {}
};
class Derived : public Base {
    void foo() override {}  // Bonne pratique
};
```

### Q7: Qu'est-ce qu'une classe abstraite ?
**Réponse:** Classe contenant au moins une fonction virtuelle pure (= 0). Ne peut pas être instanciée, sert d'interface. Les classes dérivées doivent implémenter toutes les méthodes pures pour être instanciables.

```cpp
class Shape {
public:
    virtual double area() = 0;  // Pure virtual
    virtual ~Shape() = default;
};
```

### Q8: Différence entre public, protected et private dans l'héritage ?
**Réponse:**
- **Public:** Membres publics restent publics, protected restent protected
- **Protected:** Membres publics et protected deviennent protected
- **Private:** Tous les membres deviennent private

### Q9: Qu'est-ce que le diamond problem ?
**Réponse:** Ambiguïté lors de l'héritage multiple quand deux classes parentes héritent d'une même classe grand-parent. Solution: héritage virtuel (`virtual public`).

## 🧠 Gestion Mémoire

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

```cpp
std::unique_ptr<int> p1 = std::make_unique<int>(42);
std::shared_ptr<int> p2 = std::make_shared<int>(42);
std::weak_ptr<int> p3 = p2;
```

### Q13: Qu'est-ce que le Rule of Three/Five ?
**Réponse:**
- **Rule of Three:** Si vous définissez destructeur, copy constructor ou copy assignment, définissez les trois
- **Rule of Five:** Ajoute move constructor et move assignment (C++11)

### Q14: Différence entre new et malloc ?
**Réponse:**
- **new:** Opérateur C++, appelle constructeur, typé, exception si échec
- **malloc:** Fonction C, pas de constructeur, retourne void*, retourne NULL si échec

## ⚡ Modern C++ (C++11/14/17/20)

### Q15: Qu'est-ce que move semantics ?
**Réponse:** Permet de transférer les ressources d'un objet temporaire au lieu de les copier. Utilise rvalue references (&&). Améliore les performances en évitant les copies coûteuses.

```cpp
std::vector<int> vec1 = {1,2,3};
std::vector<int> vec2 = std::move(vec1);  // Transfert, pas copie
```

### Q16: Qu'est-ce qu'une lambda ?
**Réponse:** Fonction anonyme définie inline. Syntaxe: `[capture](params) -> return_type { body }`

```cpp
auto add = [](int a, int b) { return a + b; };
int x = 5;
auto addX = [x](int a) { return a + x; };  // Capture par valeur
```

### Q17: Expliquez auto et decltype
**Réponse:**
- **auto:** Déduction automatique du type à la compilation
- **decltype:** Déclare une variable avec le même type qu'une expression

```cpp
auto x = 42;                    // int
decltype(x) y = 10;            // int
auto lambda = [](int a) { return a * 2; };
```

### Q18: Qu'est-ce que constexpr ?
**Réponse:** Indique qu'une fonction/variable peut être évaluée à la compilation. Plus puissant que const. Permet l'optimisation et la métaprogrammation.

```cpp
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n-1);
}
constexpr int val = factorial(5);  // Calculé à la compilation
```

### Q19: Qu'est-ce qu'un variadic template ?
**Réponse:** Template acceptant un nombre variable d'arguments. Utilise l'ellipse (...) et la récursion ou fold expressions.

```cpp
template<typename... Args>
void print(Args... args) {
    (std::cout << ... << args) << '\n';  // Fold expression C++17
}
```

## 📚 STL (Standard Template Library)

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

## 🔧 Concepts Avancés

### Q24: Qu'est-ce que la copie profonde vs copie superficielle ?
**Réponse:**
- **Superficielle:** Copie les valeurs des membres, y compris les pointeurs (même adresse)
- **Profonde:** Copie les données pointées, créant de nouvelles allocations mémoire

### Q25: Expliquez le mot-clé mutable
**Réponse:** Permet de modifier un membre dans une méthode const. Utile pour les caches, compteurs, mutex dans des objets logiquement constants.

```cpp
class Counter {
    mutable int accessCount = 0;
public:
    int getValue() const {
        ++accessCount;  // OK car mutable
        return 42;
    }
};
```

### Q26: Qu'est-ce que le template specialization ?
**Réponse:** Définir une implémentation spécifique pour certains types. Partielle ou complète.

```cpp
template<typename T>
class MyClass { };

template<>  // Spécialisation complète pour int
class MyClass<int> { };
```

### Q27: Différence entre struct et class ?
**Réponse:** Seule différence: membres et héritage **public** par défaut pour struct, **private** pour class. Convention: struct pour POD (Plain Old Data), class pour objets avec comportement.

### Q28: Qu'est-ce que SFINAE ?
**Réponse:** Substitution Failure Is Not An Error. Si la substitution d'un template échoue, le compilateur continue avec d'autres candidats au lieu de générer une erreur. Base de std::enable_if.

### Q29: Expliquez volatile
**Réponse:** Indique au compilateur qu'une variable peut changer en dehors du contrôle du programme (hardware, multithreading). Empêche certaines optimisations. **Ne garantit PAS la thread-safety** (utiliser std::atomic).

### Q30: Qu'est-ce qu'un functor ?
**Réponse:** Objet-fonction, classe avec operator() surchargé. Peut maintenir un état contrairement aux fonctions. Utilisé avec les algorithmes STL.

```cpp
class Multiplier {
    int factor;
public:
    Multiplier(int f) : factor(f) {}
    int operator()(int x) const { return x * factor; }
};
```

## 🧵 Multithreading

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

```cpp
std::mutex mtx;
{
    std::lock_guard<std::mutex> lock(mtx);
    // Section critique
}  // Déverrouillage automatique
```

### Q34: Différence entre lock_guard et unique_lock ?
**Réponse:**
- **lock_guard:** Simple, toujours verrouillé, pas de déverrouillage manuel
- **unique_lock:** Flexible, peut lock/unlock, defer locking, utilisable avec condition_variable

### Q35: Qu'est-ce qu'une condition variable ?
**Réponse:** Mécanisme de synchronisation permettant à des threads d'attendre qu'une condition soit remplie. Utilisé avec unique_lock pour éviter busy waiting.

### Q36: Expliquez std::atomic
**Réponse:** Type garantissant les opérations atomiques sans data race. Pas besoin de mutex pour les opérations simples. Utilise des instructions processeur spéciales.

## 🎨 Design Patterns

### Q37: Expliquez le pattern Singleton
**Réponse:** Une seule instance d'une classe dans tout le programme. Implémentation thread-safe avec static local (C++11).

```cpp
class Singleton {
    Singleton() = default;
public:
    static Singleton& getInstance() {
        static Singleton instance;  // Thread-safe en C++11
        return instance;
    }
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};
```

### Q38: Qu'est-ce que le pattern Factory ?
**Réponse:** Crée des objets sans spécifier leur classe exacte. Utilise une méthode qui retourne un pointeur vers classe de base.

### Q39: Expliquez le pattern Observer
**Réponse:** Définit une dépendance un-à-plusieurs où quand un objet change d'état, tous ses dépendants sont notifiés. Utilisé pour les événements, GUI.

## 🐛 Debugging & Optimisation

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

## 🔍 Questions Pièges

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

## 💼 Questions Comportementales Techniques

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

## 📖 Conseils pour l'Entretien Oral

1. **Parle à voix haute** de ton raisonnement
2. **Demande des clarifications** si nécessaire
3. **Commence simple** puis améliore
4. **Mentionne les edge cases** et erreurs possibles
5. **Parle de complexité** temporelle et spatiale
6. **Sois honnête** si tu ne sais pas (propose une approche)
7. **Pose des questions** sur l'architecture du projet

Bon courage pour ton entretien ! 🚀
