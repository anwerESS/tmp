## [**..**](./00_index.md)

# 📘 CHAPITRE 17 : Questions d'Entretien Fréquentes & Scénarios Pratiques

## 🎯 Objectifs du Chapitre
- Maîtriser les questions techniques les plus fréquentes
- Comprendre les scénarios de debugging
- Savoir expliquer vos choix d'architecture
- Préparer les questions comportementales

---

## 1️⃣ QUESTIONS FONDAMENTALES REACT

### Q1: **Qu'est-ce que React et pourquoi l'utiliser ?**

**Réponse structurée :**
```
React est une bibliothèque JavaScript déclarative pour construire des interfaces utilisateur.

CARACTÉRISTIQUES CLÉS:
✅ Component-based (composants réutilisables)
✅ Declarative (on décrit CE QU'on veut, pas COMMENT)
✅ Virtual DOM (performance optimisée)
✅ Unidirectional data flow (prédictible)
✅ Huge ecosystem (bibliothèques, outils)

POURQUOI UTILISER REACT?
1. Synchronisation automatique UI ↔ Data
2. Code maintenable et structuré
3. Composants réutilisables
4. Performance (Virtual DOM, reconciliation)
5. Grande communauté et écosystème
6. Forte demande sur le marché

EXEMPLE CONCRET:
Au lieu de manipuler le DOM manuellement:
  document.getElementById('count').textContent = count;
  
Avec React:
  const [count, setCount] = useState(0);
  return <div>{count}</div>;
  
React met à jour automatiquement l'UI quand count change.
```

**Mentionnez vos projets :**
```
"J'ai utilisé React dans plusieurs projets:
- worldwise: SPA avec React Router et Context API
- wild-oasis: Application complexe avec React Query
- fast-react-pizza: SPA avec Redux Toolkit
Chaque projet m'a permis de maîtriser différents aspects de React."
```

---

### Q2: **Expliquez le Virtual DOM et la Reconciliation**

**Réponse technique :**
```jsx
VIRTUAL DOM:
- Représentation légère du DOM réel en JavaScript
- React crée un arbre d'éléments React (objets JS)
- Rapide à créer et comparer

PROCESSUS:
1. State change
2. React crée un nouveau Virtual DOM
3. DIFFING: Compare ancien vs nouveau Virtual DOM
4. RECONCILIATION: Calcule les changements minimaux
5. COMMIT: Applique seulement les changements au DOM réel

EXEMPLE:
function App() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <h1>My App</h1>           {/* Ne change pas */}
      <p>Count: {count}</p>      {/* Change */}
      <button>Click</button>     {/* Ne change pas */}
    </div>
  );
}

Quand count change:
❌ Sans Virtual DOM: Recréer tout le <div>
✅ Avec Virtual DOM: Mettre à jour SEULEMENT le texte dans <p>

AVANTAGES:
✅ Performance (DOM manipulation est coûteuse)
✅ React décide QUOI et QUAND mettre à jour
✅ Developer Experience (on ne touche jamais au DOM)

ALGORITHME DE DIFFING:
- O(n) au lieu de O(n³) (heuristiques)
- Compare élément par élément
- Utilise les keys pour les listes
- Même position + même type = réutilisé
```

---

### Q3: **Différence entre Component, Instance, et Element ?**

**Réponse avec schéma :**
```jsx
1️⃣ COMPONENT (Blueprint/Template)
// Une fonction qui décrit comment créer l'UI
function Button({ text }) {
  return <button>{text}</button>;
}

2️⃣ COMPONENT INSTANCE (Utilisation concrète)
// Quand React "appelle" le composant
<Button text="Click me" />    // Instance 1
<Button text="Submit" />       // Instance 2

// Chaque instance:
// - A son propre state
// - A ses propres props
// - A son propre lifecycle

3️⃣ REACT ELEMENT (Résultat)
// Objet JavaScript décrivant l'UI
{
  type: 'button',
  props: {
    children: 'Click me'
  }
}

4️⃣ DOM ELEMENT (Résultat final)
// HTML réel dans le navigateur
<button>Click me</button>

ANALOGIE:
Component = Plan de maison (blueprint)
Instance = Maison construite (plusieurs maisons identiques)
Element = Description de la maison (objet JS)
DOM Element = Maison physique dans le navigateur
```

---

### Q4: **Expliquez le cycle de rendu complet de React**

**Réponse étape par étape :**
```
PHASE 1: TRIGGER (Déclencheur)
├─ Initial render (app démarre)
└─ State update (setState appelé)

PHASE 2: RENDER PHASE (Asynchrone)
├─ React appelle les component functions
├─ Crée nouveau Virtual DOM
├─ RECONCILIATION:
│  ├─ Compare ancien vs nouveau Virtual DOM
│  ├─ Algorithme de diffing (O(n))
│  └─ Crée liste de changements
└─ Fiber tree (structure interne React)

PHASE 3: COMMIT PHASE (Synchrone)
├─ React applique les changements au DOM réel
├─ Insertions, suppressions, mises à jour
└─ Synchrone pour cohérence (pas de partial updates)

PHASE 4: BROWSER PAINT
└─ Le navigateur affiche les changements

IMPORTANT:
❌ Render ≠ Mise à jour du DOM
✅ Render = Appeler component function + calcul changements
✅ Commit = Appliquer changements au DOM

EXEMPLE:
setCount(1);  // Trigger
↓
React appelle Counter()  // Render phase
↓
Calcule: <p>0</p> → <p>1</p>  // Diffing
↓
Met à jour seulement le texte  // Commit
↓
Navigateur affiche  // Paint
```

---

## 2️⃣ QUESTIONS SUR LE STATE MANAGEMENT

### Q5: **Quelle est la différence entre state et props ?**

**Réponse comparative :**
```jsx
STATE:
├─ Données INTERNES au composant
├─ MUTABLE (via setState)
├─ Comme une variable locale
├─ Déclenche re-render quand change
└─ Utilisé pour rendre le composant interactif

function Counter() {
  const [count, setCount] = useState(0);  // STATE
  
  const increment = () => setCount(count + 1);  // Peut modifier
  
  return <button onClick={increment}>{count}</button>;
}

PROPS:
├─ Données EXTERNES (du parent)
├─ READ-ONLY (immutable)
├─ Comme des paramètres de fonction
├─ Nouveau props = parent re-render
└─ Utilisé pour configurer le composant

function Button({ text, color }) {  // PROPS
  // ❌ text = "new";  // INTERDIT! Props immutables
  
  return <button style={{ color }}>{text}</button>;
}

<Button text="Click" color="blue" />  // Parent passe les props

RÈGLE D'OR:
- Si besoin de modifier → STATE
- Si vient du parent → PROPS
- Si calculé depuis props/state → DERIVED (pas de useState)

INTERACTION:
function Parent() {
  const [data, setData] = useState("Hello");  // STATE du parent
  
  return <Child message={data} />;  // Devient PROPS de l'enfant
}

function Child({ message }) {  // PROPS
  return <p>{message}</p>;
}
```

---

### Q6: **Expliquez useState en détail**

**Réponse complète :**
```jsx
ANATOMIE:
const [value, setValue] = useState(initialValue);
       ↑        ↑              ↑
     actuel  modifier      valeur initiale

INITIALISATION:
// 1. Valeur simple
const [count, setCount] = useState(0);

// 2. Objet
const [user, setUser] = useState({ name: "", age: 0 });

// 3. Fonction (lazy initialization)
const [data, setData] = useState(() => {
  // Calcul coûteux exécuté UNE SEULE FOIS
  const saved = localStorage.getItem('data');
  return saved ? JSON.parse(saved) : defaultData;
});

UPDATE STATE - DEUX FAÇONS:
// 1. Valeur directe
setCount(5);  // count devient 5

// 2. Fonction callback (recommandé si basé sur ancien state)
setCount(c => c + 1);  // c = valeur actuelle

POURQUOI CALLBACK?
const increment = () => {
  setCount(count + 1);  // ❌ count est "stale"
  setCount(count + 1);  // ❌ encore count + 1 (pas +2!)
  setCount(count + 1);  // ❌ toujours count + 1
  // Résultat: count + 1 (pas +3)
};

const incrementCorrect = () => {
  setCount(c => c + 1);  // ✅ c = valeur actuelle
  setCount(c => c + 1);  // ✅ c = valeur précédente + 1
  setCount(c => c + 1);  // ✅ c = valeur précédente + 1
  // Résultat: count + 3 ✅
};

ASYNCHRONE:
setCount(5);
console.log(count);  // ⚠️ Affiche ancienne valeur!
// Le state est mis à jour APRÈS le re-render

// Pour accéder à la nouvelle valeur:
useEffect(() => {
  console.log(count);  // ✅ Nouvelle valeur
}, [count]);

BATCHING (Optimisation React):
const handleClick = () => {
  setCount(1);      // ⚡
  setName("Alice"); // ⚡ Ces 3 updates
  setEmail("...");  // ⚡ = 1 seul re-render
};

RÈGLES:
1. Toujours au top-level (pas dans if/loops)
2. Seulement dans function components ou custom hooks
3. Le setter ne change jamais (stable reference)
```

---

### Q7: **Quand utiliser useState vs useReducer vs Context ?**

**Réponse avec decision tree :**
```
DECISION TREE:

❓ État local à 1-2 composants?
├─ OUI → useState
└─ NON → Continuer...

❓ État simple (1-2 variables)?
├─ OUI → useState
└─ NON → Continuer...

❓ État complexe (3+ variables liées)?
├─ OUI → useReducer
└─ NON → Continuer...

❓ Logique de mise à jour complexe?
├─ OUI → useReducer
└─ NON → Continuer...

❓ État partagé entre siblings?
├─ OUI → Lift state up + useState/useReducer
└─ NON → Continuer...

❓ État global (utilisé partout)?
├─ OUI → Context API (UI state) ou Redux
└─ NON → useState local

❓ Remote data (API)?
└─ React Query, SWR, RTK Query

EXEMPLES PAR USE CASE:

// useState: State simple local
function Counter() {
  const [count, setCount] = useState(0);
}

// useReducer: État complexe local
function Form() {
  const [state, dispatch] = useReducer(formReducer, {
    name: "", email: "", age: 0,
    errors: {}, touched: {}, isSubmitting: false
  });
}

// Context: État global UI
<ThemeProvider>
  <LanguageProvider>
    <AuthProvider>
      <App />
    </AuthProvider>
  </LanguageProvider>
</ThemeProvider>

// Redux: État global complexe
// Large app, beaucoup de shared state, middleware

// React Query: Remote data
function Products() {
  const { data, isLoading } = useQuery('products', fetchProducts);
}

COMPARAISON RAPIDE:
                useState  useReducer  Context   Redux
Simple          ⭐⭐⭐     ⭐         ⭐⭐      ⭐
Complex         ⭐        ⭐⭐⭐       ⭐⭐      ⭐⭐⭐
Local State     ⭐⭐⭐     ⭐⭐⭐       ❌       ❌
Global State    ❌        ❌         ⭐⭐⭐     ⭐⭐⭐
Performance     ⭐⭐⭐     ⭐⭐⭐       ⭐⭐      ⭐⭐⭐
DevTools        ❌        ❌         ❌       ⭐⭐⭐
Learning Curve  ⭐⭐⭐     ⭐⭐        ⭐⭐      ⭐
```

---

## 3️⃣ QUESTIONS SUR LES HOOKS

### Q8: **Expliquez useEffect en détail**

**Réponse complète :**
```jsx
DÉFINITION:
useEffect permet d'exécuter du code à différents moments
du lifecycle du composant (side effects)

SYNTAXE:
useEffect(() => {
  // Effect code
  
  return () => {
    // Cleanup (optionnel)
  };
}, [dependencies]);

3 VARIANTES:

1️⃣ Sans dependency array (chaque render)
useEffect(() => {
  console.log('Runs on EVERY render');
});
// ⚠️ Rarement utilisé, peut causer infinite loops

2️⃣ Avec array vide (mount seulement)
useEffect(() => {
  console.log('Runs ONCE on mount');
  
  return () => {
    console.log('Cleanup on unmount');
  };
}, []);
// Équivalent: componentDidMount + componentWillUnmount

3️⃣ Avec dependencies (mount + dependency change)
useEffect(() => {
  console.log('Runs when userId changes');
  
  fetchUser(userId).then(setUser);
}, [userId]);
// Équivalent: componentDidMount + componentDidUpdate

CLEANUP FUNCTION:
useEffect(() => {
  // Setup
  const timer = setTimeout(() => {
    console.log('Timer');
  }, 1000);
  
  // Cleanup (important!)
  return () => {
    clearTimeout(timer);
  };
}, []);

QUAND CLEANUP S'EXÉCUTE:
1. Avant le prochain effet (si dependencies changent)
2. Quand le composant unmount

EXEMPLES PRATIQUES:

// 1. Data fetching
useEffect(() => {
  const controller = new AbortController();
  
  fetch(`/api/users/${userId}`, {
    signal: controller.signal
  })
    .then(res => res.json())
    .then(setUser)
    .catch(err => {
      if (err.name !== 'AbortError') setError(err);
    });
  
  return () => controller.abort();  // Cleanup
}, [userId]);

// 2. Event listeners
useEffect(() => {
  const handleKeyPress = (e) => {
    if (e.key === 'Escape') onClose();
  };
  
  document.addEventListener('keydown', handleKeyPress);
  
  return () => {
    document.removeEventListener('keydown', handleKeyPress);
  };
}, [onClose]);

// 3. Subscriptions
useEffect(() => {
  const subscription = dataSource.subscribe(handleData);
  
  return () => subscription.unsubscribe();
}, []);

// 4. Document title
useEffect(() => {
  document.title = `You clicked ${count} times`;
}, [count]);

RULES:
1. Toujours inclure toutes les dependencies utilisées
2. Ne jamais mentir sur les dependencies
3. Utiliser ESLint plugin (exhaustive-deps)
4. Fonctions/objets dans dependencies → problème
   Solution: useCallback, useMemo, ou définir dans l'effect

ERREURS COURANTES:

❌ Oublier dependencies
useEffect(() => {
  fetchData(userId);  // userId utilisé mais pas dans deps!
}, []);  // ❌ Stale closure

✅ Inclure toutes les dependencies
useEffect(() => {
  fetchData(userId);
}, [userId]);  // ✅

❌ Infinite loop
useEffect(() => {
  setCount(count + 1);  // ❌ Met à jour count → re-render → effect → loop
}, [count]);

✅ Condition ou pas de dependency
useEffect(() => {
  if (someCondition) {
    setCount(count + 1);
  }
}, [someCondition]);  // ✅
```

---

### Q9: **Différence entre useEffect et useLayoutEffect ?**

**Réponse technique :**
```jsx
TIMING:

useEffect:
1. Render
2. Commit (DOM updated)
3. Browser Paint (user sees changes)
4. useEffect runs  ← Asynchrone, après paint

useLayoutEffect:
1. Render
2. Commit (DOM updated)
3. useLayoutEffect runs  ← Synchrone, avant paint
4. Browser Paint

QUAND UTILISER useLayoutEffect?
✅ Besoin de lire/modifier le DOM avant paint
✅ Éviter le "flicker" (changements visuels)
✅ Mesurer des dimensions DOM
✅ Animer avant que l'utilisateur voit

EXEMPLE useEffect (flicker):
function Tooltip({ targetRef }) {
  const [position, setPosition] = useState({ top: 0, left: 0 });
  
  useEffect(() => {
    const rect = targetRef.current.getBoundingClientRect();
    setPosition({ top: rect.bottom, left: rect.left });
  }, [targetRef]);
  
  // ⚠️ Problème: Tooltip apparaît en (0,0) puis saute à la bonne position
  
  return (
    <div style={{ position: 'absolute', ...position }}>
      Tooltip
    </div>
  );
}

SOLUTION useLayoutEffect:
function Tooltip({ targetRef }) {
  const [position, setPosition] = useState({ top: 0, left: 0 });
  
  useLayoutEffect(() => {
    const rect = targetRef.current.getBoundingClientRect();
    setPosition({ top: rect.bottom, left: rect.left });
  }, [targetRef]);
  
  // ✅ Position calculée AVANT paint → pas de flicker
  
  return (
    <div style={{ position: 'absolute', ...position }}>
      Tooltip
    </div>
  );
}

RÈGLE GÉNÉRALE:
🎯 99% du temps → useEffect
⚡ Besoin synchrone avant paint → useLayoutEffect

⚠️ useLayoutEffect bloque le browser paint
   → Peut causer performance issues si calcul lourd
```

---

### Q10: **Expliquez useCallback et useMemo. Quand les utiliser ?**

**Réponse avec exemples :**
```jsx
PROBLÈME À RÉSOUDRE:

function Parent() {
  const [count, setCount] = useState(0);
  
  // ❌ Nouvelle fonction à chaque render
  const handleClick = () => {
    console.log('Clicked');
  };
  
  // ❌ Nouvel objet à chaque render
  const config = { theme: 'dark' };
  
  return <Child onClick={handleClick} config={config} />;
}

// Si Child est memoized, il re-render quand même
// car handleClick et config sont toujours "nouveaux"

═══════════════════════════════════════════

useCallback: MÉMORISER UNE FONCTION

const memoizedFn = useCallback(
  () => {
    // Function body
  },
  [dependencies]
);

EXEMPLE:
function Parent() {
  const [count, setCount] = useState(0);
  
  // ✅ Même fonction tant que count ne change pas
  const handleClick = useCallback(() => {
    console.log('Count:', count);
  }, [count]);
  
  return <ExpensiveChild onClick={handleClick} />;
}

const ExpensiveChild = memo(function Child({ onClick }) {
  // Ne re-render que si onClick change vraiment
  return <button onClick={onClick}>Click</button>;
});

═══════════════════════════════════════════

useMemo: MÉMORISER UNE VALEUR

const memoizedValue = useMemo(
  () => {
    // Expensive calculation
    return result;
  },
  [dependencies]
);

EXEMPLE:
function SearchResults({ items, query }) {
  // ❌ Filtrage à CHAQUE render (même si items/query identiques)
  const filteredItems = items.filter(item =>
    item.name.toLowerCase().includes(query.toLowerCase())
  );
  
  // ✅ Filtrage seulement si items ou query changent
  const filteredItems = useMemo(() => {
    console.log('Filtering...');
    return items.filter(item =>
      item.name.toLowerCase().includes(query.toLowerCase())
    );
  }, [items, query]);
  
  return <List items={filteredItems} />;
}

═══════════════════════════════════════════

QUAND UTILISER?

useCallback:
✅ Passer une fonction à un child component memoized
✅ Fonction dans dependency array d'un autre hook
✅ Fonction passée à des composants qui re-render souvent

useMemo:
✅ Calcul coûteux (loops, sorting, filtering large arrays)
✅ Valeur passée à un child component memoized
✅ Objet/array dans dependency array d'un autre hook

QUAND NE PAS UTILISER:
❌ Optimisation prématurée
❌ Calcul simple et rapide
❌ Pas de problème de performance constaté
❌ Child component pas memoized

RÈGLE D'OR:
1. Écrire du code normal d'abord
2. Profiler et trouver les bottlenecks
3. Optimiser seulement ce qui est lent
4. Ne pas tout mémoriser par défaut

COMPARAISON:
useCallback(() => fn, deps)  ≈  useMemo(() => () => fn, deps)
// useCallback est un sucre syntaxique pour useMemo

EXEMPLE COMPLET:
function TodoApp() {
  const [todos, setTodos] = useState([]);
  const [filter, setFilter] = useState('all');
  
  // ✅ useMemo: Calcul coûteux
  const filteredTodos = useMemo(() => {
    console.log('Filtering todos...');
    return todos.filter(todo => {
      if (filter === 'completed') return todo.done;
      if (filter === 'active') return !todo.done;
      return true;
    });
  }, [todos, filter]);
  
  // ✅ useCallback: Fonction passée à child
  const addTodo = useCallback((text) => {
    setTodos(prev => [...prev, { id: Date.now(), text, done: false }]);
  }, []);
  
  const toggleTodo = useCallback((id) => {
    setTodos(prev => prev.map(todo =>
      todo.id === id ? { ...todo, done: !todo.done } : todo
    ));
  }, []);
  
  return (
    <>
      <AddTodoForm onAdd={addTodo} />
      <FilterButtons filter={filter} onChange={setFilter} />
      <TodoList todos={filteredTodos} onToggle={toggleTodo} />
    </>
  );
}

// Composants enfants memoized
const AddTodoForm = memo(function AddTodoForm({ onAdd }) {
  // ...
});

const TodoList = memo(function TodoList({ todos, onToggle }) {
  // ...
});
```

---

### Q11: **Qu'est-ce qu'un custom hook et quand en créer un ?**

**Réponse avec exemples :**
```jsx
CUSTOM HOOK:
- Fonction JavaScript qui commence par "use"
- Peut utiliser d'autres hooks
- Permet de réutiliser de la logique stateful
- Ne partage PAS le state (chaque usage = state indépendant)

SYNTAXE:
function useMyHook(initialValue) {
  // Peut utiliser useState, useEffect, etc.
  const [state, setState] = useState(initialValue);
  
  // Logique réutilisable
  
  return [state, setState];  // ou { state, actions }
}

QUAND CRÉER UN CUSTOM HOOK?
✅ Logique réutilisée dans plusieurs composants
✅ Logique avec hooks (sinon → fonction normale)
✅ Simplifier un composant complexe
✅ Abstraire la logique métier

EXEMPLES PRATIQUES:

1️⃣ useFetch (Data fetching)
function useFetch(url) {
  const [data, setData] = useState(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);
  
  useEffect(() => {
    const controller = new AbortController();
    
    fetch(url, { signal: controller.signal })
      .then(res => res.json())
      .then(data => {
        setData(data);
        setError(null);
      })
      .catch(err => {
        if (err.name !== 'AbortError') setError(err.message);
      })
      .finally(() => setIsLoading(false));
    
    return () => controller.abort();
  }, [url]);
  
  return { data, isLoading, error };
}

// Usage
function Users() {
  const { data: users, isLoading, error } = useFetch('/api/users');
  
  if (isLoading) return <Loader />;
  if (error) return <Error message={error} />;
  return <UserList users={users} />;
}

2️⃣ useLocalStorage (Persist state)
function useLocalStorage(key, initialValue) {
  const [value, setValue] = useState(() => {
    const saved = localStorage.getItem(key);
    return saved ? JSON.parse(saved) : initialValue;
  });
  
  useEffect(() => {
    localStorage.setItem(key, JSON.stringify(value));
  }, [key, value]);
  
  return [value, setValue];
}

// Usage
function App() {
  const [theme, setTheme] = useLocalStorage('theme', 'light');
  const [language, setLanguage] = useLocalStorage('lang', 'en');
}

3️⃣ useToggle (Boolean state)
function useToggle(initialValue = false) {
  const [value, setValue] = useState(initialValue);
  
  const toggle = useCallback(() => {
    setValue(v => !v);
  }, []);
  
  const setTrue = useCallback(() => setValue(true), []);
  const setFalse = useCallback(() => setValue(false), []);
  
  return [value, toggle, setTrue, setFalse];
}

// Usage
function Modal() {
  const [isOpen, toggle, open, close] = useToggle();
  
  return (
    <>
      <button onClick={open}>Open Modal</button>
      {isOpen && (
        <ModalComponent onClose={close} />
      )}
    </>
  );
}

4️⃣ useDebounce (Delay updates)
function useDebounce(value, delay = 500) {
  const [debouncedValue, setDebouncedValue] = useState(value);
  
  useEffect(() => {
    const timer = setTimeout(() => {
      setDebouncedValue(value);
    }, delay);
    
    return () => clearTimeout(timer);
  }, [value, delay]);
  
  return debouncedValue;
}

// Usage
function SearchInput() {
  const [query, setQuery] = useState('');
  const debouncedQuery = useDebounce(query, 500);
  
  useEffect(() => {
    if (debouncedQuery) {
      // API call seulement après 500ms d'inactivité
      fetchResults(debouncedQuery);
    }
  }, [debouncedQuery]);
  
  return (
    <input
      value={query}
      onChange={(e) => setQuery(e.target.value)}
    />
  );
}

5️⃣ useAuth (Authentication logic)
function useAuth() {
  const [user, setUser] = useState(null);
  const [isLoading, setIsLoading] = useState(true);
  
  useEffect(() => {
    // Check if user is logged in
    const token = localStorage.getItem('token');
    if (token) {
      fetch('/api/me', {
        headers: { Authorization: `Bearer ${token}` }
      })
        .then(res => res.json())
        .then(setUser)
        .finally(() => setIsLoading(false));
    } else {
      setIsLoading(false);
    }
  }, []);
  
  const login = async (email, password) => {
    const res = await fetch('/api/login', {
      method: 'POST',
      body: JSON.stringify({ email, password })
    });
    const data = await res.json();
    localStorage.setItem('token', data.token);
    setUser(data.user);
  };
  
  const logout = () => {
    localStorage.removeItem('token');
    setUser(null);
  };
  
  return { user, isLoading, login, logout };
}

RÈGLES DES HOOKS (s'appliquent aussi aux custom hooks):
1. Appeler au top-level seulement (pas dans if/loops)
2. Appeler depuis function components ou custom hooks
3. Commencer le nom par "use"
```

---

## 4️⃣ QUESTIONS SUR REACT ROUTER

### Q12: **Comment fonctionne le routing dans une SPA React ?**

**Réponse structurée :**
```jsx
SPA ROUTING:
- Client-side routing (pas de requête serveur)
- URL change → composant change
- Pas de rechargement de page

REACT ROUTER SETUP:
import { BrowserRouter, Routes, Route } from 'react-router-dom';

function App() {
  return (
    <BrowserRouter>
      <Nav />
      <Routes>
        <Route path="/" element={<Home />} />
        <Route path="/about" element={<About />} />
        <Route path="/products" element={<Products />} />
        <Route path="/products/:id" element={<ProductDetail />} />
        <Route path="*" element={<NotFound />} />
      </Routes>
    </BrowserRouter>
  );
}

NAVIGATION:

// 1. Link (navigation déclarative)
import { Link } from 'react-router-dom';

function Nav() {
  return (
    <nav>
      <Link to="/">Home</Link>
      <Link to="/about">About</Link>
      <Link to="/products">Products</Link>
    </nav>
  );
}

// 2. NavLink (avec classe active)
import { NavLink } from 'react-router-dom';

function Nav() {
  return (
    <nav>
      <NavLink 
        to="/" 
        className={({ isActive }) => isActive ? 'active' : ''}
      >
        Home
      </NavLink>
    </nav>
  );
}

// 3. useNavigate (navigation programmatique)
import { useNavigate } from 'react-router-dom';

function LoginForm() {
  const navigate = useNavigate();
  
  const handleSubmit = async (credentials) => {
    await login(credentials);
    navigate('/dashboard');  // Redirection après login
  };
  
  return <form onSubmit={handleSubmit}>...</form>;
}

HOOKS UTILES:

// useParams - Lire les paramètres d'URL
function ProductDetail() {
  const { id } = useParams();  // /products/:id
  
  return <div>Product {id}</div>;
}

// useSearchParams - Query strings
function SearchPage() {
  const [searchParams, setSearchParams] = useSearchParams();
  
  const query = searchParams.get('q');      // ?q=react
  const filter = searchParams.get('filter'); // &filter=new
  
  const updateSearch = (newQuery) => {
    setSearchParams({ q: newQuery, filter });
  };
  
  return <SearchResults query={query} />;
}

// useLocation - Informations sur la route actuelle
function Analytics() {
  const location = useLocation();
  
  useEffect(() => {
    trackPageView(location.pathname);
  }, [location]);
  
  return null;
}

PROTECTED ROUTES:
function ProtectedRoute({ children }) {
  const { isAuthenticated } = useAuth();
  
  if (!isAuthenticated) {
    return <Navigate to="/login" replace />;
  }
  
  return children;
}

// Usage
<Route 
  path="/dashboard" 
  element={
    <ProtectedRoute>
      <Dashboard />
    </ProtectedRoute>
  } 
/>

NESTED ROUTES:
<Route path="/app" element={<AppLayout />}>
  <Route index element={<Dashboard />} />
  <Route path="profile" element={<Profile />} />
  <Route path="settings" element={<Settings />} />
</Route>

function AppLayout() {
  return (
    <div>
      <Sidebar />
      <main>
        <Outlet />  {/* Nested routes render here */}
      </main>
    </div>
  );
}

DATA LOADING (React Router v6.4+):
// Loader pattern
const productLoader = async ({ params }) => {
  const response = await fetch(`/api/products/${params.id}`);
  return response.json();
};

<Route 
  path="/products/:id" 
  element={<ProductDetail />}
  loader={productLoader}
/>

function ProductDetail() {
  const product = useLoaderData();  // Data from loader
  return <div>{product.name}</div>;
}

ACTIONS (Form submissions):
const createProductAction = async ({ request }) => {
  const formData = await request.formData();
  const product = Object.fromEntries(formData);
  
  await fetch('/api/products', {
    method: 'POST',
    body: JSON.stringify(product)
  });
  
  return redirect('/products');
};

<Route 
  path="/products/new" 
  element={<NewProduct />}
  action={createProductAction}
/>
```

---

### Q13: **Comment gérer le state dans l'URL ?**

**Réponse pratique :**
```jsx
POURQUOI STATE DANS L'URL?
✅ Bookmarkable (partager un lien avec état)
✅ Browser back/forward fonctionne
✅ Synchronisation entre tabs
✅ Deep linking (accès direct à un état)

USE CASES:
- Filtres et tri (e-commerce)
- Pagination
- Search queries
- Selected items
- Modal states (parfois)

IMPLÉMENTATION:

// 1. QUERY PARAMETERS (pour filtres, search)
function ProductList() {
  const [searchParams, setSearchParams] = useSearchParams();
  
  // Lire depuis URL
  const category = searchParams.get('category') || 'all';
  const sort = searchParams.get('sort') || 'name';
  const page = parseInt(searchParams.get('page') || '1');
  
  // Mettre à jour URL
  const updateFilters = (newFilters) => {
    setSearchParams({
      category: newFilters.category,
      sort: newFilters.sort,
      page: newFilters.page
    });
  };
  
  return (
    <div>
      <FilterPanel
        category={category}
        sort={sort}
        onFilterChange={updateFilters}
      />
      <Products 
        category={category}
        sort={sort}
        page={page}
      />
    </div>
  );
}

// URL: /products?category=electronics&sort=price&page=2

// 2. ROUTE PARAMETERS (pour identifiants)
function CityDetail() {
  const { cityId } = useParams();  // /cities/:cityId
  
  // Fetch city data based on cityId
  const { data: city } = useQuery(['city', cityId], () => 
    fetchCity(cityId)
  );
  
  return <div>{city.name}</div>;
}

// 3. COMBINER LES DEUX
function SearchResults() {
  const { category } = useParams();           // /search/:category
  const [searchParams] = useSearchParams();
  
  const query = searchParams.get('q');        // ?q=laptop
  const minPrice = searchParams.get('min');   // &min=500
  const maxPrice = searchParams.get('max');   // &max=2000
  
  // /search/electronics?q=laptop&min=500&max=2000
  
  return <Results category={category} query={query} />;
}

// 4. CUSTOM HOOK POUR URL STATE
function useUrlState(key, defaultValue) {
  const [searchParams, setSearchParams] = useSearchParams();
  
  const value = searchParams.get(key) || defaultValue;
  
  const setValue = (newValue) => {
    const newParams = new URLSearchParams(searchParams);
    
    if (newValue) {
      newParams.set(key, newValue);
    } else {
      newParams.delete(key);
    }
    
    setSearchParams(newParams);
  };
  
  return [value, setValue];
}

// Usage
function Filters() {
  const [category, setCategory] = useUrlState('category', 'all');
  const [sort, setSort] = useUrlState('sort', 'name');
  
  return (
    <div>
      <select value={category} onChange={(e) => setCategory(e.target.value)}>
        <option value="all">All</option>
        <option value="electronics">Electronics</option>
      </select>
      
      <select value={sort} onChange={(e) => setSort(e.target.value)}>
        <option value="name">Name</option>
        <option value="price">Price</option>
      </select>
    </div>
  );
}

EXEMPLE COMPLET (E-commerce):
function ProductCatalog() {
  const [searchParams, setSearchParams] = useSearchParams();
  
  // Parse URL params
  const filters = {
    category: searchParams.get('category') || 'all',
    minPrice: parseInt(searchParams.get('minPrice') || '0'),
    maxPrice: parseInt(searchParams.get('maxPrice') || '10000'),
    inStock: searchParams.get('inStock') === 'true',
    sort: searchParams.get('sort') || 'name',
    page: parseInt(searchParams.get('page') || '1')
  };
  
  // Update URL
  const updateFilters = (newFilters) => {
    const params = new URLSearchParams();
    
    Object.entries({ ...filters, ...newFilters }).forEach(([key, value]) => {
      if (value !== 'all' && value !== 0 && value !== false) {
        params.set(key, value.toString());
      }
    });
    
    setSearchParams(params);
  };
  
  // Fetch data based on filters
  const { data: products } = useQuery(
    ['products', filters],
    () => fetchProducts(filters)
  );
  
  return (
    <div>
      <FilterSidebar filters={filters} onUpdate={updateFilters} />
      <ProductGrid products={products} />
      <Pagination 
        current={filters.page}
        onPageChange={(page) => updateFilters({ page })}
      />
    </div>
  );
}

// URL devient:
// /products?category=electronics&minPrice=500&maxPrice=2000&inStock=true&sort=price&page=2
```

---

## 5️⃣ QUESTIONS SUR NEXT.JS

### Q14: **Quelle est la différence entre CSR, SSR, et SSG ?**

**Réponse comparative :**
```
CLIENT-SIDE RENDERING (CSR):
┌─────────────────────────────────────┐
│ Browser                             │
│ 1. Request page                     │
│ 2. Receive HTML shell + JS bundle   │
│ 3. Execute JS                       │
│ 4. Fetch data (API)                 │
│ 5. Render content                   │
│                                     │
│ ⏱️ Time to Interactive: SLOW        │
│ 🔍 SEO: PROBLEMATIC                 │
│ ⚡ Navigation: FAST                  │
└─────────────────────────────────────┘

EXEMPLES: Classic React SPA (Vite, CRA)
USE CASE: Dashboards, apps behind login

═══════════════════════════════════════

SERVER-SIDE RENDERING (SSR):
┌─────────────────────────────────────┐
│ Server                              │
│ 1. Request page                     │
│ 2. Fetch data                       │
│ 3. Render HTML                      │
│ 4. Send complete HTML               │
│                                     │
│ Browser                             │
│ 5. Display HTML (fast!)             │
│ 6. Download JS                      │
│ 7. Hydrate (make interactive)       │
│                                     │
│ ⏱️ Time to Interactive: MEDIUM      │
│ 🔍 SEO: EXCELLENT                   │
│ ⚡ Navigation: MEDIUM                │
└─────────────────────────────────────┘

EXEMPLES: Next.js (dynamic rendering)
USE CASE: E-commerce, news, content sites

═══════════════════════════════════════

STATIC SITE GENERATION (SSG):
┌─────────────────────────────────────┐
│ Build Time                          │
│ 1. Fetch all data                   │
│ 2. Generate all HTML pages          │
│ 3. Store as static files            │
│                                     │
│ CDN                                 │
│ 4. Serve pre-built HTML            │
│                                     │
│ Browser                             │
│ 5. Display instantly                │
│ 6. Hydrate                          │
│                                     │
│ ⏱️ Time to Interactive: FAST        │
│ 🔍 SEO: EXCELLENT                   │
│ ⚡ Navigation: FAST                  │
└─────────────────────────────────────┘

EXEMPLES: Next.js (static export), Gatsby
USE CASE: Blogs, docs, marketing sites

═══════════════════════════════════════

INCREMENTAL STATIC REGENERATION (ISR):
Mix of SSG + SSR
- Pages générées statiquement
- Regénérées en background après X temps
- Meilleur des deux mondes

EXEMPLES: Next.js avec revalidate
USE CASE: Sites avec contenu qui change peu

═══════════════════════════════════════

COMPARAISON TABLEAU:

Feature        CSR      SSR      SSG      ISR
─────────────────────────────────────────────
First Paint    ❌ Slow  ✅ Fast  ⭐ Instant ⭐ Instant
SEO            ⚠️ Poor  ✅ Great ✅ Great   ✅ Great
Build Time     ✅ Fast  ✅ Fast  ⚠️ Slow   ⚠️ Slow
Data Fresh     ⭐ Live  ⭐ Live  ❌ Stale   ⚡ Periodic
Hosting        ✅ Easy  ⚠️ Server ✅ CDN    ⚠️ Server
Cost           ✅ Low   ⚠️ High   ✅ Low    ⚠️ Medium

NEXT.JS IMPLEMENTATION:

// CSR (Client Component)
'use client';

function Products() {
  const [products, setProducts] = useState([]);
  
  useEffect(() => {
    fetch('/api/products').then(res => res.json()).then(setProducts);
  }, []);
  
  return <ProductList products={products} />;
}

// SSR (Server Component with dynamic data)
async function Products() {
  const products = await fetch('https://api.com/products', {
    cache: 'no-store'  // Force dynamic
  }).then(res => res.json());
  
  return <ProductList products={products} />;
}

// SSG (Server Component with static data)
async function Products() {
  const products = await fetch('https://api.com/products', {
    cache: 'force-cache'  // Cache forever
  }).then(res => res.json());
  
  return <ProductList products={products} />;
}

// ISR (revalidate)
async function Products() {
  const products = await fetch('https://api.com/products', {
    next: { revalidate: 60 }  // Regenerate every 60s
  }).then(res => res.json());
  
  return <ProductList products={products} />;
}

// Page-level
export const revalidate = 60;  // ISR every 60s
export const dynamic = 'force-dynamic';  // Force SSR
export const dynamic = 'force-static';   // Force SSG
```

---

### Q15: **Qu'est-ce que les React Server Components (RSC) ?**

**Réponse détaillée :**
```jsx
REACT SERVER COMPONENTS (RSC):
Nouveau paradigme qui divise les composants en deux catégories

┌─────────────────────────────────────────┐
│ SERVER COMPONENTS (Default in Next.js) │
├─────────────────────────────────────────┤
│ ✅ Render seulement sur le serveur      │
│ ✅ Accès direct à la DB/backend         │
│ ✅ 0 KB JavaScript envoyé au client     │
│ ✅ Peuvent être async                   │
│ ❌ Pas de hooks (useState, useEffect)   │
│ ❌ Pas d'interactivité                  │
│ ❌ Pas d'event handlers                 │
└─────────────────────────────────────────┘

┌─────────────────────────────────────────┐
│ CLIENT COMPONENTS ('use client')        │
├─────────────────────────────────────────┤
│ ✅ Render sur serveur + client          │
│ ✅ Interactifs (event handlers)         │
│ ✅ Peuvent utiliser hooks               │
│ ✅ Accès au browser APIs                │
│ ❌ Envoient JavaScript au client        │
│ ❌ Pas d'accès direct backend           │
└─────────────────────────────────────────┘

EXEMPLES:

// SERVER COMPONENT (default)
async function ProductList() {
  // ✅ Accès direct à la DB
  const products = await db.product.findMany();
  
  // ✅ Pas de JavaScript côté client pour ce composant
  return (
    <div>
      {products.map(product => (
        <ProductCard key={product.id} product={product} />
      ))}
    </div>
  );
}

// CLIENT COMPONENT
'use client';

function AddToCartButton({ productId }) {
  const [isLoading, setIsLoading] = useState(false);
  
  const handleClick = async () => {
    setIsLoading(true);
    await addToCart(productId);
    setIsLoading(false);
  };
  
  return (
    <button onClick={handleClick} disabled={isLoading}>
      {isLoading ? 'Adding...' : 'Add to Cart'}
    </button>
  );
}

// COMPOSITION (Server + Client)
// Server Component
async function ProductPage({ params }) {
  const product = await getProduct(params.id);
  
  return (
    <div>
      <h1>{product.name}</h1>
      <p>{product.description}</p>
      
      {/* Client Component pour interactivité */}
      <AddToCartButton productId={product.id} />
      
      {/* Server Component pour données */}
      <RelatedProducts categoryId={product.categoryId} />
    </div>
  );
}

AVANTAGES RSC:
✅ Moins de JavaScript au client (performance)
✅ Accès direct backend (pas besoin d'API)
✅ Meilleur SEO (content served)
✅ Data fetching optimisé (pas de waterfalls)
✅ Security (secrets restent sur serveur)

LIMITES:
❌ Courbe d'apprentissage
❌ Nouveau paradigme mental
❌ Nécessite un framework (Next.js)
❌ Peut être confusing (quand utiliser quoi?)

QUAND UTILISER CHACUN?

SERVER COMPONENT (par défaut):
✅ Display data
✅ Fetch from database
✅ Access backend resources
✅ Keep sensitive info on server
✅ Large dependencies qui restent sur serveur

CLIENT COMPONENT ('use client'):
✅ Interactivity (onClick, onChange)
✅ State (useState, useReducer)
✅ Effects (useEffect)
✅ Browser APIs (localStorage, window)
✅ Event listeners
✅ Custom hooks

RÈGLES IMPORTANTES:

// ❌ NE PEUT PAS: Client Component dans Server Component
// (en tant qu'import direct)
import ClientComp from './ClientComp';  // 'use client'

function ServerComp() {
  return <ClientComp />;  // ❌ Transforme ServerComp en Client
}

// ✅ PEUT: Via children prop
function ServerComp({ children }) {
  return (
    <div>
      <ServerOnlyLogic />
      {children}  // ✅ children peut être un Client Component
    </div>
  );
}

// Usage
<ServerComp>
  <ClientComp />
</ServerComp>

// ✅ PEUT: Passer Server Component en prop
<ClientLayout>
  <ServerDataComponent />
</ClientLayout>

PATTERNS COURANTS:

// 1. Leaves du tree sont Client Components
<ServerRoot>
  <ServerLayout>
    <ServerDataFetch>
      <ClientInteractive />  {/* Feuilles */}
    </ServerDataFetch>
  </ServerLayout>
</ServerRoot>

// 2. Client wrapper minimal
function ProductCard({ product }) {
  // Server Component: Static content
  return (
    <div>
      <img src={product.image} />
      <h3>{product.name}</h3>
      <p>{product.price}€</p>
      
      {/* Client Component: Interactive part only */}
      <AddToCartButton productId={product.id} />
    </div>
  );
}
```

---

### Q16: **Qu'est-ce que Server Actions et comment les utiliser ?**

**Réponse pratique :**
```jsx
SERVER ACTIONS:
Fonctions async qui s'exécutent sur le serveur
mais peuvent être appelées depuis le client

SYNTAXE:

// app/actions.js
'use server';  // ← Marque toutes les exports comme Server Actions

export async function createTodo(formData) {
  // ✅ Accès direct à la DB
  const text = formData.get('text');
  
  await db.todo.create({
    data: { text, completed: false }
  });
  
  // ✅ Revalidate cache
  revalidatePath('/todos');
  
  // ✅ Redirect
  redirect('/todos');
}

export async function deleteTodo(id) {
  await db.todo.delete({ where: { id } });
  revalidatePath('/todos');
}

UTILISATION:

// 1️⃣ Dans un Form (Server Component)
import { createTodo } from './actions';

function TodoForm() {
  return (
    <form action={createTodo}>
      <input name="text" required />
      <button type="submit">Add Todo</button>
    </form>
  );
}

// 2️⃣ Dans un Client Component avec useFormStatus
'use client';
import { useFormStatus } from 'react-dom';
import { createTodo } from './actions';

function SubmitButton() {
  const { pending } = useFormStatus();
  
  return (
    <button type="submit" disabled={pending}>
      {pending ? 'Adding...' : 'Add Todo'}
    </button>
  );
}

function TodoForm() {
  return (
    <form action={createTodo}>
      <input name="text" required />
      <SubmitButton />
    </form>
  );
}

// 3️⃣ Appelée programmatiquement
'use client';
import { deleteTodo } from './actions';

function TodoItem({ todo }) {
  const handleDelete = async () => {
    await deleteTodo(todo.id);
  };
  
  return (
    <div>
      <span>{todo.text}</span>
      <button onClick={handleDelete}>Delete</button>
    </div>
  );
}

// 4️⃣ Avec useTransition (optimistic updates)
'use client';
import { useTransition } from 'react';
import { toggleTodo } from './actions';

function TodoItem({ todo }) {
  const [isPending, startTransition] = useTransition();
  
  const handleToggle = () => {
    startTransition(async () => {
      await toggleTodo(todo.id);
    });
  };
  
  return (
    <div style={{ opacity: isPending ? 0.5 : 1 }}>
      <input
        type="checkbox"
        checked={todo.completed}
        onChange={handleToggle}
      />
      <span>{todo.text}</span>
    </div>
  );
}

// 5️⃣ Avec useOptimistic (UI immédiate)
'use client';
import { useOptimistic } from 'react';
import { addTodo } from './actions';

function TodoList({ todos }) {
  const [optimisticTodos, addOptimisticTodo] = useOptimistic(
    todos,
    (state, newTodo) => [...state, newTodo]
  );
  
  const handleSubmit = async (formData) => {
    const text = formData.get('text');
    
    // Update UI immédiatement
    addOptimisticTodo({ id: Date.now(), text, completed: false });
    
    // Puis appeler server action
    await addTodo(formData);
  };
  
  return (
    <>
      <form action={handleSubmit}>
        <input name="text" />
        <button>Add</button>
      </form>
      
      {optimisticTodos.map(todo => (
        <TodoItem key={todo.id} todo={todo} />
      ))}
    </>
  );
}

VALIDATION & ERROR HANDLING:

'use server';

export async function createUser(formData) {
  // Validation
  const email = formData.get('email');
  const password = formData.get('password');
  
  if (!email || !email.includes('@')) {
    return { error: 'Invalid email' };
  }
  
  if (password.length < 8) {
    return { error: 'Password too short' };
  }
  
  try {
    const user = await db.user.create({
      data: { email, password: await hash(password) }
    });
    
    return { success: true, user };
    
  } catch (error) {
    return { error: 'User already exists' };
  }
}

// Client
'use client';
import { useActionState } from 'react';  // React 19
import { createUser } from './actions';

function SignupForm() {
  const [state, formAction] = useActionState(createUser, null);
  
  return (
    <form action={formAction}>
      <input name="email" type="email" />
      <input name="password" type="password" />
      
      {state?.error && <p className="error">{state.error}</p>}
      {state?.success && <p>Account created!</p>}
      
      <button type="submit">Sign Up</button>
    </form>
  );
}

AVANTAGES:
✅ Pas besoin de créer des API routes
✅ Type-safe (avec TypeScript)
✅ Automatic revalidation
✅ Progressive enhancement (fonctionne sans JS)
✅ Moins de boilerplate

LIMITATIONS:
❌ Next.js only
❌ Besoin d'un serveur (pas de static export)
❌ Courbe d'apprentissage
```

---

## 6️⃣ QUESTIONS SUR LA PERFORMANCE

### Q17: **Comment optimiser la performance d'une application React ?**

**Réponse structurée par catégories :**
```jsx
1️⃣ ÉVITER LES WASTED RENDERS

// A. React.memo (Composant pur)
const ExpensiveComponent = memo(function Expensive({ data }) {
  console.log('Rendering Expensive');
  return <div>{/* Expensive render */}</div>;
});

// Ne re-render que si 'data' change vraiment

// B. useMemo (Calculs coûteux)
function DataTable({ items }) {
  const sortedItems = useMemo(() => {
    console.log('Sorting...');
    return [...items].sort((a, b) => a.value - b.value);
  }, [items]);  // Recalcule seulement si items change
  
  return <Table data={sortedItems} />;
}

// C. useCallback (Fonctions stables)
function Parent() {
  const [count, setCount] = useState(0);
  
  // ❌ Nouvelle fonction à chaque render
  const handleClick = () => console.log('Clicked');
  
  // ✅ Fonction stable
  const handleClickMemo = useCallback(() => {
    console.log('Clicked');
  }, []);
  
  return <ExpensiveChild onClick={handleClickMemo} />;
}

// D. Children prop pattern
function Parent() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>{count}</button>
      
      {/* ❌ SlowComponent re-render quand count change */}
      <SlowComponent />
    </div>
  );
}

// ✅ SOLUTION: children prop
function Parent() {
  return (
    <Counter>
      <SlowComponent />  {/* Ne re-render PAS */}
    </Counter>
  );
}

function Counter({ children }) {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>{count}</button>
      {children}
    </div>
  );
}

═══════════════════════════════════════

2️⃣ CODE SPLITTING & LAZY LOADING

// A. React.lazy (Dynamic imports)
import { lazy, Suspense } from 'react';

const HeavyComponent = lazy(() => import('./HeavyComponent'));

function App() {
  return (
    <Suspense fallback={<Loader />}>
      <HeavyComponent />
    </Suspense>
  );
}

// B. Route-based splitting
const Dashboard = lazy(() => import('./pages/Dashboard'));
const Settings = lazy(() => import('./pages/Settings'));

<Routes>
  <Route path="/dashboard" element={
    <Suspense fallback={<Loader />}>
      <Dashboard />
    </Suspense>
  } />
  <Route path="/settings" element={
    <Suspense fallback={<Loader />}>
      <Settings />
    </Suspense>
  } />
</Routes>

// C. Conditional loading
function App() {
  const [showEditor, setShowEditor] = useState(false);
  
  const Editor = lazy(() => import('./Editor'));  // Lourd
  
  return (
    <div>
      <button onClick={() => setShowEditor(true)}>
        Open Editor
      </button>
      
      {showEditor && (
        <Suspense fallback={<Loader />}>
          <Editor />
        </Suspense>
      )}
    </div>
  );
}

═══════════════════════════════════════

3️⃣ VIRTUALIZATION (Listes longues)

// react-window ou react-virtualized
import { FixedSizeList } from 'react-window';

function LargeList({ items }) {
  // Au lieu de render 10,000 items:
  return (
    <FixedSizeList
      height={600}
      itemCount={items.length}
      itemSize={35}
      width="100%"
    >
      {({ index, style }) => (
        <div style={style}>
          {items[index].name}
        </div>
      )}
    </FixedSizeList>
  );
  
  // Render seulement les items visibles (~20)
  // Performance: O(visible) au lieu de O(total)
}

═══════════════════════════════════════

4️⃣ DEBOUNCING & THROTTLING

// Debounce: Attendre que l'utilisateur finisse
function SearchInput() {
  const [query, setQuery] = useState('');
  const debouncedQuery = useDebounce(query, 500);
  
  useEffect(() => {
    if (debouncedQuery) {
      fetchResults(debouncedQuery);  // API call après 500ms idle
    }
  }, [debouncedQuery]);
  
  return (
    <input
      value={query}
      onChange={(e) => setQuery(e.target.value)}
    />
  );
}

// Throttle: Limiter la fréquence
function MouseTracker() {
  const [position, setPosition] = useState({ x: 0, y: 0 });
  
  const handleMouseMove = useThrottle((e) => {
    setPosition({
      /*/*/*
      ```javascript
function Gallery() {
  return (
    <div>
      {/* 1. Image avec dimensions fixes */}
      <Image
        src="/photo.jpg"
        alt="Photo"
        width={800}
        height={600}
        quality={90} // 1-100 (default: 75)
      />
      
      {/* 2. Image responsive (fill) */}
      <div style={{ position: 'relative', width: '100%', height: '400px' }}>
        <Image
          src="/banner.jpg"
          alt="Banner"
          fill
          sizes="100vw" // Hint pour responsive images
          style={{ objectFit: 'cover' }}
        />
      </div>
      
      {/* 3. Blur placeholder */}
      <Image
        src="/portrait.jpg"
        alt="Portrait"
        width={400}
        height={400}
        placeholder="blur"
        blurDataURL="data:image/jpeg;base64,..." // Base64 tiny image
      />
      
      {/* 4. Priority (above the fold) */}
      <Image
        src="/hero.jpg"
        alt="Hero"
        width={1200}
        height={600}
        priority // Pas de lazy load, charge immédiatement
      />
      
      {/* 5. Image externe */}
      <Image
        src="https://example.com/image.jpg"
        alt="External"
        width={500}
        height={300}
      />
    </div>
  );
}
```

### 3. Configuration Images

```javascript
// next.config.js
module.exports = {
  images: {
    // Domaines externes autorisés
    domains: ['example.com', 'cdn.example.com'],
    
    // Ou remote patterns (plus flexible)
    remotePatterns: [
      {
        protocol: 'https',
        hostname: '**.example.com',
        pathname: '/images/**',
      },
    ],
    
    // Formats supportés
    formats: ['image/webp', 'image/avif'],
    
    // Device sizes pour responsive
    deviceSizes: [640, 750, 828, 1080, 1200, 1920, 2048, 3840],
    
    // Image sizes
    imageSizes: [16, 32, 48, 64, 96, 128, 256, 384],
  },
};
```

---

## 🎯 Middleware

### 1. Middleware Basique

```javascript
// middleware.js (à la racine du projet)
import { NextResponse } from 'next/server';

export function middleware(request) {
  // Log toutes les requests
  console.log('Request URL:', request.url);
  console.log('Method:', request.method);
  
  // Continue vers la route
  return NextResponse.next();
}

// Spécifier quelles routes matcher
export const config = {
  matcher: [
    /*
     * Match toutes les routes SAUF :
     * - api (API routes)
     * - _next/static (static files)
     * - _next/image (image optimization)
     * - favicon.ico (favicon file)
     */
    '/((?!api|_next/static|_next/image|favicon.ico).*)',
  ],
};
```

### 2. Authentication Middleware

```javascript
// middleware.js - Protection de routes
import { NextResponse } from 'next/server';

export async function middleware(request) {
  // Lire cookie/token
  const token = request.cookies.get('auth-token')?.value;
  
  // Routes protégées
  const protectedPaths = ['/dashboard', '/profile', '/settings'];
  const isProtectedPath = protectedPaths.some(path =>
    request.nextUrl.pathname.startsWith(path)
  );
  
  // Redirect si pas de token sur route protégée
  if (isProtectedPath && !token) {
    const loginUrl = new URL('/login', request.url);
    loginUrl.searchParams.set('from', request.nextUrl.pathname);
    return NextResponse.redirect(loginUrl);
  }
  
  // Vérifier token
  if (token) {
    try {
      const user = await verifyToken(token);
      
      // Ajouter user aux headers (accessible dans page)
      const requestHeaders = new Headers(request.headers);
      requestHeaders.set('x-user-id', user.id);
      
      return NextResponse.next({
        request: {
          headers: requestHeaders,
        },
      });
    } catch (error) {
      // Token invalide → supprimer cookie
      const response = NextResponse.redirect(new URL('/login', request.url));
      response.cookies.delete('auth-token');
      return response;
    }
  }
  
  return NextResponse.next();
}

export const config = {
  matcher: ['/dashboard/:path*', '/profile/:path*', '/settings/:path*'],
};
```

### 3. Middleware Patterns Avancés

```javascript
// A/B Testing
export function middleware(request) {
  const bucket = request.cookies.get('bucket')?.value;
  
  if (!bucket) {
    // Assigner random bucket
    const newBucket = Math.random() > 0.5 ? 'a' : 'b';
    const response = NextResponse.next();
    response.cookies.set('bucket', newBucket);
    return response;
  }
  
  return NextResponse.next();
}

// Geolocation redirect
export function middleware(request) {
  const country = request.geo?.country;
  
  if (country === 'FR' && !request.nextUrl.pathname.startsWith('/fr')) {
    return NextResponse.redirect(new URL('/fr', request.url));
  }
  
  return NextResponse.next();
}

// Rate limiting (simple)
const rateLimitMap = new Map();

export function middleware(request) {
  const ip = request.ip ?? 'unknown';
  const now = Date.now();
  const windowMs = 60 * 1000; // 1 minute
  const maxRequests = 100;
  
  const userRequests = rateLimitMap.get(ip) || [];
  const recentRequests = userRequests.filter(time => now - time < windowMs);
  
  if (recentRequests.length >= maxRequests) {
    return new NextResponse('Too Many Requests', { status: 429 });
  }
  
  recentRequests.push(now);
  rateLimitMap.set(ip, recentRequests);
  
  return NextResponse.next();
}
```

---

## ⚙️ Configuration (next.config.js)

### 1. Configuration Essentielle

```javascript
// next.config.js
/** @type {import('next').NextConfig} */
const nextConfig = {
  // Images configuration
  images: {
    domains: ['cdn.example.com', 'images.unsplash.com'],
    remotePatterns: [
      {
        protocol: 'https',
        hostname: '**.amazonaws.com',
      },
    ],
    formats: ['image/webp', 'image/avif'],
  },
  
  // Redirects
  async redirects() {
    return [
      {
        source: '/old-blog/:slug',
        destination: '/blog/:slug',
        permanent: true, // 308
      },
    ];
  },
  
  // Rewrites (proxy)
  async rewrites() {
    return [
      {
        source: '/api/:path*',
        destination: 'https://api.example.com/:path*',
      },
    ];
  },
  
  // Headers
  async headers() {
    return [
      {
        source: '/api/:path*',
        headers: [
          { key: 'Access-Control-Allow-Origin', value: '*' },
        ],
      },
    ];
  },
  
  // React strict mode
  reactStrictMode: true,
  
  // Experimental features
  experimental: {
    serverActions: true,
  },
};

module.exports = nextConfig;
```

### 2. Environment Variables

```bash
# .env.local (local development, NOT committed)
DATABASE_URL=postgresql://localhost:5432/mydb
API_SECRET=super-secret-key
NEXT_PUBLIC_API_URL=http://localhost:3000/api

# .env.production (production, NOT committed)
DATABASE_URL=postgresql://prod-host:5432/mydb
NEXT_PUBLIC_API_URL=https://myapp.com/api
```

```javascript
// Utilisation dans code
// Server-side (tous les env vars accessibles)
export default async function Page() {
  const dbUrl = process.env.DATABASE_URL; // ✅ Accessible
  const secret = process.env.API_SECRET;  // ✅ Accessible
  
  return <div>Server Component</div>;
}

// Client-side (seulement NEXT_PUBLIC_*)
'use client';

export default function ClientComponent() {
  // ❌ undefined (pas de NEXT_PUBLIC_)
  const dbUrl = process.env.DATABASE_URL;
  
  // ✅ Accessible (NEXT_PUBLIC_ prefix)
  const apiUrl = process.env.NEXT_PUBLIC_API_URL;
  
  return <div>{apiUrl}</div>;
}
```

---

## 🎤 Questions d'Entretien - Next.js Fondamentaux

### Q1: "Quelle est la différence entre App Router et Pages Router ?"

**Réponse experte :**

```
**PAGES ROUTER** (Old - Next.js < 13) :
- Directory: pages/
- File = Route (pages/about.js → /about)
- Data fetching: getServerSideProps, getStaticProps
- Layouts: _app.js (HOC global)
- Pas de nested layouts
- Loading/Error: manuel

**APP ROUTER** (New - Next.js 13+) :
- Directory: app/
- Folder = Route segment
- Data fetching: async/await direct dans components
- Layouts: Nested layouts avec layout.js
- Server Components par défaut
- loading.js et error.js automatiques
- Conventions spéciales (page.js, layout.js, etc.)

**RECOMMANDATION :**
Pour nouveaux projets, toujours utiliser App Router. C'est plus puissant,
plus performant, et c'est le futur de Next.js. Il offre:
- Meilleure performance (Server Components)
- Layouts imbriqués
- Loading et error states intégrés
- Data fetching simplifié
- Streaming et Suspense built-in
```

### Q2: "Comment fonctionne le file-based routing dans App Router ?"

**Réponse avec exemples :**

```javascript
/**
 * FILE-BASED ROUTING - APP ROUTER
 * 
 * Conventions :
 * - Seul page.js crée une route publique
 * - Folders = segments de route
 * - [brackets] = dynamic segments
 * - (parentheses) = organisation (pas dans URL)
 * - _underscore = privé (exclu du routing)
 */

// EXEMPLES :

// 1. Routes basiques
// app/page.js              → /
// app/about/page.js        → /about
// app/blog/page.js         → /blog

// 2. Dynamic routes
// app/blog/[slug]/page.js  → /blog/:slug
export default function Post({ params }) {
  return <h1>{params.slug}</h1>;
}

// 3. Catch-all
// app/shop/[...categories]/page.js → /shop/*
// /shop/electronics → params.categories = ["electronics"]
// /shop/a/b/c      → params.categories = ["a", "b", "c"]

// 4. Route groups (organisation)
// app/(marketing)/page.js   → /
// app/(shop)/products/page.js → /products
// (parentheses) n'apparaissent PAS dans l'URL

// 5. Private folders
// app/_components/Header.js → Pas une route
// app/dashboard/page.js     → /dashboard (route publique)

// 6. Conventions spéciales
// layout.js    → Wrapper pour segment
// loading.js   → Loading UI
// error.js     → Error boundary
// not-found.js → 404 page
```

### Q3: "Qu'est-ce que le middleware Next.js et quand l'utiliser ?"

**Réponse complète :**

```javascript
/**
 * MIDDLEWARE NEXT.JS
 * 
 * Définition : Code qui s'exécute AVANT qu'une requête soit complétée
 * Lieu : Edge (pas dans Node.js runtime)
 * Fichier : middleware.js à la racine
 */

// EXEMPLE : Authentication
// middleware.js
import { NextResponse } from 'next/server';

export async function middleware(request) {
  const token = request.cookies.get('auth-token')?.value;
  
  // Routes protégées
  if (request.nextUrl.pathname.startsWith('/dashboard')) {
    if (!token) {
      return NextResponse.redirect(new URL('/login', request.url));
    }
  }
  
  return NextResponse.next();
}

export const config = {
  matcher: '/dashboard/:path*',
};

/**
 * QUAND UTILISER :
 * - Authentication/Authorization
 * - Redirects conditionnels
 * - Geolocation routing
 * - A/B testing
 * - Rate limiting
 * - Feature flags
 * 
 * LIMITATIONS :
 * - Edge runtime (pas de Node.js APIs)
 * - Pas de DB queries directes
 * - Bundle size limité
 * 
 * CAPACITÉS :
 * - Lire : cookies, headers, URL, geo, IP
 * - Modifier : redirect, rewrite, set headers/cookies
 */
```

### Q4: "Comment optimises-tu les images dans Next.js ?"

**Réponse pratique :**

```javascript
/**
 * IMAGE OPTIMIZATION DANS NEXT.JS
 */

// ✅ Utiliser <Image> component (TOUJOURS)
import Image from 'next/image';

export default function Page() {
  return (
    <div>
      {/* ❌ MAUVAIS */}
      <img src="/photo.jpg" alt="Photo" />
      
      {/* ✅ BON - Optimization automatique */}
      <Image
        src="/photo.jpg"
        alt="Photo"
        width={800}
        height={600}
        quality={85}
        priority={false}
      />
      
      {/* Image responsive */}
      <div style={{ position: 'relative', width: '100%', height: '400px' }}>
        <Image
          src="/hero.jpg"
          alt="Hero"
          fill
          sizes="100vw"
          style={{ objectFit: 'cover' }}
        />
      </div>
      
      {/* Above the fold */}
      <Image
        src="/hero.jpg"
        alt="Hero"
        width={1200}
        height={600}
        priority // Pas de lazy load
      />
    </div>
  );
}

/**
 * AVANTAGES automatiques :
 * - Lazy loading (sauf si priority)
 * - Responsive images (srcset)
 * - Format moderne (WebP, AVIF)
 * - Resize on-demand
 * - Blur placeholder
 * - Prevent layout shift
 * - CDN caching
 * 
 * CONFIGURATION :
 * next.config.js
 * images: {
 *   domains: ['cdn.example.com'],
 *   formats: ['image/webp', 'image/avif'],
 * }
 */
```

### Q5: "Explique les différents fichiers spéciaux dans App Router"

**Réponse structurée :**

```javascript
/**
 * FICHIERS SPÉCIAUX APP ROUTER
 */

const specialFiles = {
  // 1. page.js - REQUIS pour route publique
  page: `
    export default function Page() {
      return <h1>Public route</h1>;
    }
  `,
  
  // 2. layout.js - Wrapper partagé
  layout: `
    export default function Layout({ children }) {
      return (
        <div>
          <nav>Navigation</nav>
          {children}
        </div>
      );
    }
    // Persiste entre navigations
    // Pas de re-render lors navigation
  `,
  
  // 3. loading.js - Loading UI automatique
  loading: `
    export default function Loading() {
      return <Spinner />;
    }
    // Wrapped dans <Suspense> automatiquement
  `,
  
  // 4. error.js - Error boundary automatique
  error: `
    'use client';
    
    export default function Error({ error, reset }) {
      return (
        <div>
          <h2>Error: {error.message}</h2>
          <button onClick={reset}>Try again</button>
        </div>
      );
    }
    // DOIT être client component
  `,
  
  // 5. not-found.js - 404 page
  notFound: `
    export default function NotFound() {
      return <h1>404 - Page Not Found</h1>;
    }
  `,
  
  // 6. template.js - Re-render à chaque navigation
  template: `
    export default function Template({ children }) {
      return <div>{children}</div>;
    }
    // Contrairement à layout, re-render à chaque fois
  `,
  
  // 7. route.js - API endpoint
  route: `
    export async function GET(request) {
      return Response.json({ message: 'Hello' });
    }
  `,
};

/**
 * HIÉRARCHIE :
 * layout.js (outer)
 *   → template.js
 *     → error.js
 *       → loading.js
 *         → page.js
 * 
 * RÈGLES :
 * - page.js seul crée route publique
 * - layout.js wrap automatiquement children
 * - loading.js automatic Suspense
 * - error.js automatic error boundary
 */
```

---

## 💡 Best Practices - Next.js Fondamentaux

### Checklist Complète

```javascript
const bestPractices = {
  // 1. ROUTING
  routing: {
    always: [
      'Utiliser App Router (pas Pages Router)',
      'page.js pour routes publiques',
      'layout.js pour wrappers partagés',
      'Route groups pour organisation',
      '_folders pour fichiers privés',
    ],
    never: [
      'Mélanger App Router et Pages Router',
      'Oublier page.js (folder sans page = pas de route)',
      'Mettre logique dans layout.js (server component)',
    ],
  },
  
  // 2. NAVIGATION
  navigation: {
    always: [
      'Link pour navigation interne',
      'useRouter pour navigation programmatique',
      'Active link styling avec usePathname',
    ],
    never: [
      '<a> pour navigation interne (full page reload)',
      'window.location.href (pas de prefetch)',
    ],
  },
  
  // 3. IMAGES
  images: {
    always: [
      'Image component pour toutes les images',
      'width/height ou fill prop',
      'priority pour above-the-fold images',
      'alt text pour accessibilité',
    ],
    never: [
      '<img> tag (pas d\'optimization)',
      'Oublier domains dans next.config.js (images externes)',
    ],
  },
  
  // 4. METADATA
  metadata: {
    always: [
      'generateMetadata pour dynamic metadata',
      'Static metadata pour pages statiques',
      'Title template dans root layout',
      'OpenGraph et Twitter cards',
    ],
    never: [
      'Oublier metadata (mauvais SEO)',
      'Hardcoder metadata dynamique',
    ],
  },
  
  // 5. ERROR HANDLING
  errorHandling: {
    always: [
      'error.js pour error boundaries',
      'not-found.js pour 404s',
      'loading.js pour loading states',
      'Skeleton UI pour meilleure UX',
    ],
    never: [
      'Laisser errors non-gérées',
      'Loading spinners sans context',
    ],
  },
};
```

---

## 🔗 Liens avec Vos Projets

### Wild Oasis (Peut être migré vers Next.js)

```javascript
const wildOasisNextjs = {
  structure: `
    app/
    ├── layout.js                  → Root avec Supabase provider
    ├── page.js                    → Homepage
    ├── (auth)/
    │   ├── login/page.js
    │   └── signup/page.js
    └── (dashboard)/
        ├── layout.js              → Dashboard layout avec sidebar
        ├── dashboard/page.js
        ├── cabins/
        │   ├── page.js            → Liste cabins
        │   └── [id]/page.js       → Détail cabin
        ├── bookings/
        │   ├── page.js
        │   └── [id]/page.js
        └── settings/page.js
  `,
  
  improvements: [
    'SSR pour SEO',
    'Image optimization automatique',
    'Loading states intégrés',
    'File-based routing (plus simple)',
    'Middleware pour auth',
  ],
};
```

### Fast React Pizza (Next.js serait idéal)

```javascript
const pizzaNextjs = {
  structure: `
    app/
    ├── layout.js
    ├── page.js                    → Homepage avec menu
    ├── menu/
    │   └── page.js                → Menu complet
    ├── cart/
    │   └── page.js                → Cart page
    ├── order/
    │   ├── new/page.js            → Nouvelle commande
    │   └── [orderId]/page.js      → Tracking commande
    └── api/
        └── order/route.js         → API endpoint
  `,
  
  benefits: [
    'Image optimization pour pizzas',
    'API routes intégrées',
    'SSR pour menu (SEO)',
    'Metadata dynamique par page',
  ],
};
```

---

## 🎯 Points Clés

```javascript
const keyTakeaways = {
  appRouter: {
    modern: 'App Router est le standard moderne (Next.js 13+)',
    fileConventions: 'page.js, layout.js, loading.js, error.js',
    serverComponents: 'Server Components par défaut',
    nestedLayouts: 'Layouts imbriqués automatiques',
  },
  
  routing: {
    fileBased: 'File-based routing automatique',
    dynamic: '[slug] pour routes dynamiques',
    catchAll: '[...slug] pour catch-all routes',
    routeGroups: '(folder) pour organisation',
  },
  
  optimization: {
    images: 'Image component pour optimization automatique',
    metadata: 'generateMetadata pour SEO',
    loading: 'loading.js pour loading states',
    error: 'error.js pour error boundaries',
  },
  
  middleware: {
    usage: 'Authentication, redirects, A/B testing',
    edge: 'S\'exécute sur Edge (pas Node.js)',
    limitations: 'Pas de DB queries directes',
  },
};
```

---