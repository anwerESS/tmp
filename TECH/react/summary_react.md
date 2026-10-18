# 1 : Concepts de Base React

`React` est une bibliothèque JavaScript déclarative, basée sur des composants, pilotée par l’état, pour construire des interfaces utilisateur.

Les `composants` sont les briques de construction reutilisable, techniquement un composant et fonctions JavaScript qui retournent du JSX

`Problème des Applications Vanilla JavaScript`:
Le défi principal : Garder l’interface utilisateur (UI) synchronisée avec les données (state), les probleme concrets sont (Manipulation DOM directe, Synchronisation manuelle).
avec react quand le state change → React re-rend automatiquement

Le `Virtual DOM` en React, c’est une copie légère du DOM en mémoire.
Quand il y a un changement, React compare l’ancienne version avec la nouvelle, et met à jour uniquement ce qui a changé dans le vrai DOM.
Du coup, ça améliore les performances.

`SPA`: Une seule page HTML, JavaScript gère tout le routing et le rendu, Navigation instantanée (pas de rechargement), Sensation d’app native

📊 **CSR vs SSR : Comparaison**

| Critère | CSR (SPA) | SSR (Traditionnel) |
|---------|-----------|-------------------|
| **Performance initiale** | ❌ Plus lent (gros bundle JS) | ✅ Rapide (HTML prêt) |
| **Navigation** | ✅ Instantanée | ❌ Rechargement complet |
| **Interactivité** | ✅ Très interactive | ❌ Limitée |
| **SEO** | ❌ Peut être problématique | ✅ Excellent |
| **Complexité** | ⚠️ Plus complexe | ✅ Plus simple |
| **Use cases** | Apps internes, dashboards | Sites e-commerce, blogs |

**cycle de vie d'un composant**

```
1. MOUNT (Naissance) 🐣
   ↓
   - Composant créé
   - State et props initialisés
   - Première apparition dans le DOM

2. RE-RENDER (Vie) 🐓
   ↓
   - Déclenché par:
     • State change
     • Props change
     • Parent re-render
     • Context change

3. UNMOUNT (Mort) 💀
   ↓
   - Composant retiré du DOM
   - State et props détruits
   - Nettoyage nécessaire
```

<br>

---

<br>


# 2 : Components, Props & JSX

🔄 **Props vs State**

| Props | State |
|-------|-------|
| **Propriété EXTERNE** | **Donnée INTERNE** |
| Vient du **parent** | Appartient au **composant** |
| **READ-ONLY** (immutable) | **MUTABLE** (via setState) |
| Comme des **paramètres de fonction** | Comme des **variables locales** |
| Reçu de l'extérieur | Créé dans le composant |
| Pour **configurer** le composant | Pour rendre le composant **interactif** |

le `children` prop special permet de passer du contenu entre les balises d’un composant.

`COMPONENT COMPOSITION`: Assembler des composants simples pour créer des composants complexes

Le `prop drilling` en React, c’est quand on passe des données de parent à enfant sur plusieurs niveaux. donc la solution est la composition (passer le composant directement) ou  Context API 

les props sont-elles immutables pour (Pureté, Prédictibilité(pas de sideEffect), Débogage)

un `side effect`, c’est tout ce qui n’est pas directement lié au rendu du composant, comme appeler une API, manipuler le DOM ou lancer un timer. (On les gère généralement avec le hook useEffect).

<br>

---

<br>

# 3 : State et Événements

le `state`, c’est une donnée interne au composant qui peut changer dans le temps.
Quand le state change, React re-render le composant automatiquement pour mettre à jour l’interface

`LAZY INITIALIZATION`
```jsx
// Utile si le calcul initial est coûteux
const [count, setCount] = useState(() => {
  const savedCount = localStorage.getItem('count');
  return savedCount ? JSON.parse(savedCount) : 0;
});
// La fonction ne s'exécute qu'une seule fois (au mount)
```

**STATE AVEC OBJETS ET TABLEAUX**

***Updating Objects (Immutabilité)***

```jsx
function UserProfile() {
  const [user, setUser] = useState({
    name: "Alice",
    email: "alice@example.com",
    age: 25,
    address: {
      city: "Paris", country: "France"
    }
  });
  
  // ❌ MAUVAIS: Mutation directe
  const updateNameBad = () => {
    user.name = "Bob";  // ❌ Mutation!
    setUser(user);      // React ne détectera pas le changement
  };
  
  // ✅ BON: Créer un nouvel objet
  const updateName = () => {
    setUser({
      ...user,           // Copie toutes les propriétés
      name: "Bob"        // Override la propriété name
    });
  };

  // ✅ Multiple properties update
  const updateProfile = () => {
    setUser(prev => ({
      ...prev,
      name: "Charlie",
      age: prev.age + 1
    }));
  };
  //...

 }
```
***Updating Arrays (Immutabilité)***

```jsx
function TodoList() {
  const [todos, setTodos] = useState([
    { id: 1, text: "Learn React", done: false },
    { id: 2, text: "Build project", done: false }
  ]);
  
  // ✅ AJOUTER un élément
  const addTodo = (text) => {
    const newTodo = {
      id: Date.now(),
      text: text,
      done: false
    };
    
    // Méthode 1: Spread operator
    setTodos([...todos, newTodo]);
    
    // Méthode 2: concat
    setTodos(todos.concat(newTodo));
    
    // ❌ MAUVAIS: push (mutation)
    // todos.push(newTodo);  // ❌
    // setTodos(todos);      // ❌
  };
  
  // ✅ SUPPRIMER un élément
  const deleteTodo = (id) => {
    setTodos(todos.filter(todo => todo.id !== id));
  };
  
  // ✅ MODIFIER un élément
  const toggleTodo = (id) => {
    setTodos(todos.map(todo =>
      todo.id === id 
        ? { ...todo, done: !todo.done }
        : todo
    ));
  };
```

le `derived state`, c’est un state calculé à partir des props ou d’un autre state.`

==> ⚡ Performance : useMemo pour Derived State Coûteux

```tsx
function ExpensiveComponent({ data }) {
  // ❌ Sans useMemo: Recalculé à CHAQUE render (même si data ne change pas)
  const expensiveCalculation = computeExpensiveValue(data);
  
  // ✅ Avec useMemo: Recalculé SEULEMENT si data change
  const memoizedValue = useMemo(() => {
    return computeExpensiveValue(data);
  }, [data]);
  
  return <div>{memoizedValue}</div>;
}
```
<br>

---

<br>

# 4 : Gestion d’État Avancée (useState, useReducer, Context API)

**UseReducer**

`useReducer` est un hook pour gérer des états complexes avec une logique centralisée.
On dispatch des actions, et un reducer décide comment mettre à jour le state.
C’est utile quand le state devient difficile à gérer avec useState

```tsx
// useState (simple)
const [count, setCount] = useState(0);

// useReducer (complexe)
function reducer(state, action) { // return state
	switch(action.type){/*..*/}
} 
/* useReducer */
const [state, dispatch] = useReducer(reducer, initialState);
/* dispatch */
<button onClick={() => dispatch({ type: 'increment', payload: {}  })}>



```

```tsx
import { useReducer } from 'react';

// 1️⃣ INITIAL STATE
const initialState = {
  count: 0,
  step: 1,
  status: 'idle'  // 'idle' | 'counting' | 'paused'
};

// 2️⃣ REDUCER FUNCTION
function reducer(state, action) {
  // Pure function: state + action → new state
  
  switch (action.type) {
    case 'increment':
      return {
        ...state, count: state.count + state.step
      };
    
    case 'decrement':
      return {
        ...state, count: state.count - state.step
      };
    
    case 'setStep':
      return {
        ...state, step: action.payload
      };
    
    case 'reset':
      return initialState;
    
    case 'start':
      return {
        ...state, status: 'counting'
      };
    
    case 'pause':
      return {
        ...state, status: 'paused'
      };
    
    default:
      throw new Error(`Unknown action: ${action.type}`);
  }
}

// 3️⃣ COMPONENT
function Counter() {
  const [state, dispatch] = useReducer(reducer, initialState);
  
  return (
    <div>
      <p>Count: {state.count}</p>
      <p>Step: {state.step}</p>
      <p>Status: {state.status}</p>
      
      <button onClick={() => dispatch({ type: 'increment' })}>
        +
      </button>
      <button onClick={() => dispatch({ type: 'decrement' })}>
        -
      </button>
      
      <input
        type="number"
        value={state.step}
        onChange={(e) => dispatch({ 
          type: 'setStep', 
          payload: Number(e.target.value) 
        })}
      />
      
      <button onClick={() => dispatch({ type: 'start' })}>
        Start
      </button>
      <button onClick={() => dispatch({ type: 'pause' })}>
        Pause
      </button>
      <button onClick={() => dispatch({ type: 'reset' })}>
        Reset
      </button>
    </div>
  );

```

***useState vs useReducer : Comparaison***

| Critère | useState | useReducer |
|---------|----------|------------|
| **Use case** | 1-2 state variables simples | État complexe avec plusieurs variables liées |
| **Logique** | Dans event handlers (dispersée) | Centralisée dans reducer |
| **Updates** | Appeler setState | Dispatcher des actions |
| **Complexity** | ✅ Simple à comprendre | ⚠️ Plus complexe (boilerplate) |
| **Testability** | ⚠️ Difficile (couplé au composant) | ✅ Facile (reducer = pure function) |
| **Debug** | ⚠️ State updates partout | ✅ Toutes les transitions dans reducer |
| **Type safety** | ⚠️ Moins évident | ✅ Actions typées (avec TypeScript) |

**CONTEXT API : GLOBAL STATE MANAGEMEN**

le `Context API` permet de partager des données globales entre plusieurs composants sans passer par les props à chaque niveau.
C’est souvent utilisé pour éviter le prop drilling, par exemple pour le thème ou l’utilisateur connecté.

```jsx
// createContext
const UserContext = createContext();

// create Provider
function UserProvider({ children }) {
  const [user, setUser] = useState(null);
  
  return (
    <UserContext.Provider value={{ user, setUser }}>
      {children}
    </UserContext.Provider>
  );
}

// 3️⃣ CRÉER UN CUSTOM HOOK (optionnel mais recommandé)
function useUser() {
  const context = useContext(UserContext);
  if (context === undefined) {
    throw new Error('useUser must be used within UserProvider');
  }
  return context;
}

// WRAPPER L'APP
function App() {
  return (
    <UserProvider>
      <Dashboard />
    </UserProvider>
  );
}

// utlisation dans un composant n-fils 

function UserProfile() {
  // ✅ Accès direct au context!
  const { user, setUser } = useUser();
 
 // ... 
}

```


***Decision Matrix : Quel Tool Utiliser ?***

| Type de State | Scope | Outils Recommandés | Use Cases |
|---------------|-------|-------------------|-----------|
| **UI Local** | 1 composant | `useState` | Toggle, form input, modal |
| **UI Local Complexe** | 1 composant | `useReducer` | Form complexe, wizard |
| **UI Shared** | Siblings | Lift state + `useState` | Filter + List |
| **UI Global** | Toute l'app | Context API | Theme, auth status, language |
| **UI Global Complexe** | Toute l'app | Redux, Zustand | Cart, complex UI state |
| **Remote Data** | Toute l'app | React Query, SWR | API data, cache |
| **URL State** | Navigation | React Router | Search, filters, pagination |
| **Browser Storage** | Persistence | localStorage + custom hook | User preferences |

<br>

---

<br>

# 5 : Redux & Redux Toolkit

`Redux`, c’est une lib pour gérer le state global dans une app React.
On a un store central, et on met à jour le state avec des actions

```tsx
//##########  Configuration du Store ########## 

const store = configureStore({
  reducer: {
    account: accountReducer,
    cart: cartReducer,
    user: userReducer,
  },
  // Middleware et DevTools sont auto-configurés !
});

export default store;


//########## Provider dans l’App (main.jsx ou index.js) ########## 

ReactDOM.createRoot(document.getElementById('root')).render(
  <React.StrictMode>
    <Provider store={store}>
      <App />
    </Provider>
  </React.StrictMode>
);


//########## // cartSlice.js ########## 

const cartSlice = createSlice({
  // 1. NOM (préfixe pour action types)
  name: 'cart',
  
  // 2. STATE INITIAL
  initialState: {/*...*/},
  
  // 3. REDUCERS (actions synchrones)
  reducers: {
    // Reducer simple
    addItem(state, action) {...},
    //...
   }
});

// Export actions (auto-générées)
export const { 
  addItem, 
  removeItem, 
  addItemWithId, 
  clearCart, 
  increaseQuantity 
} = cartSlice.actions;

// Export reducer
export default cartSlice.reducer;

//########## UTILISATION DANS LES COMPONENTS ########## 
// cart.js

function Cart() {
  // ✅ Selector simple
  const items = useSelector((state) => state.cart.items);
  
  // ✅ Selector avec calcul
  const total = useSelector((state) => 
    state.cart.items.reduce((sum, item) => sum + item.price, 0)
  );

  // ...

 }

```

***ASYNC OPERATIONS : createAsyncThunk***

Un `thunk`est une fonction qui retourne une fonction (pour retarder l’exécution).


**Tableau Comparatif**

| Critère | Redux Toolkit | Context API |
|---------|---------------|-------------|
| **Use case** | State global complexe | State simple (theme, auth) |
| **Performance** | ✅ Optimisé (granular subscriptions) | ⚠️ Re-renders fréquents |
| **DevTools** | ✅ Redux DevTools (time-travel) | ❌ Limité |
| **Async logic** | ✅ createAsyncThunk intégré | ⚠️ useEffect + useState |
| **Middleware** | ✅ Support natif | ❌ Difficile |
| **Learning curve** | ⚠️ Plus complexe | ✅ Plus simple |
| **Boilerplate** | ⚠️ Plus de setup initial | ✅ Minimal |
| **Debugging** | ✅ Excellent | ⚠️ Console.log |
| **Testing** | ✅ Pure functions faciles à tester | ⚠️ Plus complexe |

<br>

---

<br>

# 6 : Hooks Fondamentaux

Un `hook` en React, c’est une fonction qui permet d’utiliser des fonctionnalités de React dans un composant fonctionnel.

**useEffect**

`useEffect` est un hook qui gére les side effects, comme appeler une API, localStorage ou ajouter un event listener

```tsx
//##### Sans Dependency Array → Exécute à CHAQUE render
  // ❌ DANGEREUX : Exécute à chaque render
  useEffect(() => {
    console.log('Je m\'exécute à chaque render');
  }); // Pas de []


//##### Avec [] vide → Exécute UNE SEULE FOIS (au mount)
  // ✅ Exécute seulement au premier render
  // Use cases: Fetch initial data, Setup subscriptions, Add event listeners, Initialize third-party libs
  useEffect(() => {
    console.log('Component mounted');
    fetch('/api/data').then(res => res.json()).then(setData);
  }, []); // Array vide


//##### Avec [dependencies] → Exécute quand les deps changent
function SearchResults({ query }) {
  const [results, setResults] = useState([]);
  // ✅ Exécute quand query change
  useEffect(() => {
    console.log(`Searching for: ${query}`);
    fetch(`/api/search?q=${query}`).then(res => res.json())
      .then(setResults);
  }, [query]); // Re-exécute si query change
  
  return (
    <ul>
      {results.map(item => <li key={item.id}>{item.name}</li>)}
    </ul>
  );
}

```

***Cleanup = Fonction retournée par useEffect pour “nettoyer***

```tsx
// ✅ Intervals/Timeouts
useEffect(() => {
  const id = setInterval(() => console.log('tick'), 1000);
  return () => clearInterval(id); // OBLIGATOIRE
}, []);


// ✅ Event Listeners
useEffect(() => {
  const handler = () => console.log('clicked');
  document.addEventListener('click', handler);
  return () => document.removeEventListener('click', handler); // OBLIGATOIRE
}, []);
```

```tsx
// ❌ MAUVAIS : useEffect ne peut pas être async
useEffect(async () => {
  const data = await fetch('/api/data');
  setData(data);
}, []);
// Erreur : useEffect doit retourner une fonction ou rien,
// pas une Promise (ce que fait async)

// ✅ SOLUTION : Fonction async à l'intérieur
useEffect(() => {
  async function fetchData() {
    const res = await fetch('/api/data');
    const json = await res.json();
    setData(json);
  }
  fetchData();
}, []);
```

**useRef**

`useRef` en React permet de persister une valeur entre les renders sans provoquer de re-render

***Différences avec useState :***

| | useState | useRef |
|---|---|---|
| **Change trigger re-render ?** | ✅ Oui | ❌ Non |
| **Valeur persiste ?** | ✅ Oui | ✅ Oui |
| **Immuable ?** | ✅ Oui | ❌ Non (mutable) |
| **Usage** | UI data | DOM refs, values |


**Custom Hook**

Un `custom hook` en React, c’est un hook qu’on crée soi-même pour réutiliser de la logique.

Règles :
	✅ Nom commence par use (convention)
	✅ Peut utiliser d’autres Hooks
	✅ Retourne ce qu’on veut (value, array, object)

`Exemple`

```tsx
function useLocalStorage(key, initialValue) {
  // Lazy initialization
  const [storedValue, setStoredValue] = useState(() => {
    try {
      const item = window.localStorage.getItem(key);
      return item ? JSON.parse(item) : initialValue;
    } catch (error) {
      console.error(error);
      return initialValue;
    }
  });
  
  // Wrapper de setValue qui sync avec localStorage
  const setValue = (value) => {
    try {
      const valueToStore = value instanceof Function ? value(storedValue) : value;
      setStoredValue(valueToStore);
      window.localStorage.setItem(key, JSON.stringify(valueToStore));
    } catch (error) {
      console.error(error);
    }
  };
  
  return [storedValue, setValue];
}

// Usage
function App() {
  const [theme, setTheme] = useLocalStorage('theme', 'dark');
  const [user, setUser] = useLocalStorage('user', null);
  
  return (
    <div className={theme}>
      <button onClick={() => setTheme(theme === 'dark' ? 'light' : 'dark')}>
        Toggle Theme
      </button>
      {user && <p>Welcome {user.name}</p>}
    </div>
  );
}
```

**useLayoutEffect**

***useEffect :***
	✅ Exécute après le paint (asynchrone)  
	✅ N’bloque pas le navigateur  
	✅ Utilisé pour 99% des cas  

***useLayoutEffect :***
	✅ Exécute avant le paint (synchrone)   
	⚠️ Bloque le navigateur  
	✅ Utilisé pour mesures DOM ou mutations synchrones  

<br>

---

<br>


# 7 : Hooks de Performance

**Optimization**

React re-render un component quand :  
	1. Son state change  
	2. Ses props changent  
	3. Son parent re-render  

***Les 3 Techniques d'Optimisation***

| Technique | Usage | Quand utiliser |
|-----------|-------|----------------|
| **React.memo** | Éviter re-renders inutiles de components | Component pur avec props stables |
| **useMemo** | Mémoïser des **valeurs** calculées | Calculs coûteux |
| **useCallback** | Mémoïser des **fonctions** | Fonctions passées comme props |


**React.memo**

`React.memo` est utilisé pour optimiser les performances en évitant les re-renders inutiles d’un composant si ses props n’ont pas changé.

```tsx
// ❌ SANS memo : Re-render à chaque fois
function Child({ name }) {
  console.log('Child rendered');
  return <div>Hello {name}</div>;
}

function Parent() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(count + 1)}>Count: {count}</button>
      <Child name="John" />
    </div>
  );
}
// Console : Child rendered (à chaque click sur le bouton)

// ✅ AVEC memo : Re-render seulement si name change
const Child = React.memo(function Child({ name }) {
  console.log('Child rendered');
  return <div>Hello {name}</div>;
});
```

**useMemo**

`useMemo` en React permet de mémoriser une valeur pour éviter de la recalculer à chaque render.

```tsx
// ❌ SANS useMemo : Calcul à chaque render
function Component({ items }) {
  const total = items.reduce((sum, item) => sum + item.price, 0);
  // Recalcule même si items n'a pas changé !
  
  return <div>Total: ${total}</div>;
}

// ✅ AVEC useMemo : Calcul seulement si items change
function Component({ items }) {
  const total = useMemo(
    () => items.reduce((sum, item) => sum + item.price, 0),
    [items]
  );
  
  return <div>Total: ${total}</div>;
}
```

**useCallback**

`useCallback` en React permet de mémoriser une fonction pour éviter qu’elle soit recréée à chaque render.

```tsx
// ✅ AVEC useCallback : Fonction stable
function Parent() {
  const [count, setCount] = useState(0);
  
  const handleClick = useCallback(() => {
    console.log('Clicked');
  }, []); // Même fonction à chaque render
  
  return (
    <div>
      <button onClick={() => setCount(count + 1)}>Count: {count}</button>
      <Child onClick={handleClick} />
    </div>
  );
}
```

**📊 COMPARATIF : memo vs useMemo vs useCallback**

| | React.memo | useMemo | useCallback |
|---|---|---|---|
| **Type** | HOC | Hook | Hook |
| **Mémoïse** | Component entier | Valeur calculée | Fonction |
| **Retourne** | Component mémoïsé | Valeur | Fonction |
| **Usage** | `React.memo(Component)` | `useMemo(() => value, [deps])` | `useCallback(() => {}, [deps])` |
| **Quand** | Éviter re-renders | Calculs coûteux | Fonctions comme props |

<br>

---

<br>

# 8 : React Router v6

**React Router**

`React Router` est une librairie qui permet de gérer la navigation entre différentes pages dans une application React (SPA)

```tsx
import { BrowserRouter, Routes, Route } from "react-router-dom";

function App() {
  return (
    <BrowserRouter>
      <Routes>
        <Route path="/" element={<Home />} />
        <Route path="/about" element={<About />} />
          <Route path="/products/:id" element={<ProductDetail />} />
      </Routes>
    </BrowserRouter>
  );
}

//---------------------------------------
// Accéder aux params
import { useParams } from 'react-router-dom';

function ProductDetail() {
  const { id } = useParams();
  // URL: /products/123 → id = "123"
  return <div>Product ID: {id}</div>;
}

//---------------------------------------
// link component & NavLink (avec active state)
function Navigation() {
  return (
    <nav>
      <Link to="/">Home</Link>
      <Link to="/about">About</Link>
      <Link to="/products/123">Product 123</Link>
      <NavLink
        to="/"
        className={({ isActive }) => isActive ? 'active' : ''}
      > About </NavLink>
 	</nav>
   )
}

//---------------------------------------
// Programmatic Navigation (useNavigate)
function LoginForm() {
  const navigate = useNavigate();
  
  const handleSubmit = async (e) => {
    e.preventDefault();
    
    const success = await login(credentials);
    
    if (success) {
      // Naviguer vers /dashboard
      navigate('/dashboard');
      
      // Naviguer en arrière
      // navigate(-1);
      
      // Naviguer avec replace (pas d'historique)
      // navigate('/dashboard', { replace: true });
      
      // Naviguer avec state
      // navigate('/dashboard', { state: { from: 'login' } });
    }
  };
  
  return <form onSubmit={handleSubmit}>...</form>;
}

//---------------------------------------
// useSearchParams (Query Strings)
function ProductsList() {
  const [searchParams, setSearchParams] = useSearchParams();
  
  // URL: /products?category=electronics&sort=price
  const category = searchParams.get('category'); // "electronics"
  const sort = searchParams.get('sort'); // "price"
  
  const handleFilter = (newCategory) => {
    setSearchParams({ category: newCategory, sort });
    // URL devient: /products?category=newCategory&sort=price
  };
  
  return (
    <div>
      <p>Category: {category}</p>
      <p>Sort: {sort}</p>
      
      <button onClick={() => handleFilter('books')}>
        Show Books
      </button>
      
      <button onClick={() => setSearchParams({ sort: 'name' })}>
        Sort by Name
      </button>
    </div>
  );
}

//---------------------------------------
// useLocation
function CurrentPage() {
  const location = useLocation();
  
  console.log(location.pathname); // "/products/123"
  console.log(location.search); // "?sort=price"
  console.log(location.hash); // "#section1"
  console.log(location.state); // { from: 'homepage' }
  
  return (
    <div>
      <p>Current path: {location.pathname}</p>
    </div>
  );
}
```

<br>

---

<br>

# 9 : Data Fetching & React Query

**React Query**
`React Query` permet de gérer le fetching, le caching et la synchronisation des données côté serveur.


`useQuery` → fetch + cache automatique
`useMutation` → actions avec optimistic update
`queryClient` → gestion du cache et synchronisation

`staleTime` vs `cacheTime` :
staleTime définit combien de temps la data est considérée fraîche (pas de refetch),
cacheTime définit combien de temps elle reste en mémoire après usage.

<br>

---

<br>

# 12 :  Next.js Fondamentaux

`Next.js` est un framework React full-stack qui permet de faire du SSR, SSG, ISR, du routing basé sur les fichiers et même des API côté serveur.

| Problème (React)                     | Solution (Next.js)                  |
|-------------------------------------|------------------------------------|
| Pas de routing intégré              | Routing basé sur les fichiers       |
| Pas de SSR (SEO limité)             | SSR / SSG / ISR intégrés            |
| Bundle chargé d’un coup             | Code splitting automatique          |
| Pas de backend intégré              | API Routes intégrées                |
| Optimisation manuelle (images/fonts)| Optimisation automatique            |
| Déploiement complexe                | Déploiement simple (Vercel)         |


**Création d’un Projet Next.js**

```shell
# Créer nouveau projet
npx create-next-app@latest my-app

# Options pendant l'installation :
# ✅ TypeScript? → No (pour ce cours)
# ✅ ESLint? → Yes
# ✅ Tailwind CSS? → Yes
# ✅ src/ directory? → No
# ✅ App Router? → Yes (IMPORTANT !)
# ✅ Import alias? → Yes (@/*)

cd my-app
npm run dev
# App disponible sur http://localhost:3000
```

<br>

**🟢 Quand utiliser React**

* Applications **SPA (Single Page Application)**
* **Dashboards internes** (admin, back-office)
* Projets **sans besoin SEO**
* Applications fortement **interactives côté client**
* Quand tu veux une **librairie légère et flexible**
* Architecture déjà gérée (backend séparé, API existantes)

**🔵 Quand utiliser Next.js**

* Applications **SEO-friendly** (site public, blog, e-commerce)
* Besoin de **SSR / SSG / ISR**
* Projets nécessitant de **bonnes performances initiales**
* Applications **full-stack** (API intégrées)
* Besoin de **routing automatique**
* Optimisations intégrées (images, fonts, code splitting)

<br>

---

<br>

# 13 : Next.js Avancé

`SSR (Server-Side Rendering)`: la page est générée à chaque requête côté serveur → toujours à jour  
`SSG (Static Site Generation)`: la page est générée une fois au build → très rapide  
`ISR (Incremental Static Regeneration)`: c’est du SSG avec mise à jour automatique après un certain temps  

<br>

**Server Components**

Les `Server Components` en React sont des composants qui s’exécutent côté serveur.

✅ Avantages des Server Components (React / Next.js)
* 🚀 Réduction du JavaScript côté client (bundle plus léger)
* ⚡ Meilleures performances (moins de code à exécuter dans le navigateur)
* 🔒 Accès direct aux ressources serveur (DB, filesystem, variables d’environnement)
* 🌐 Pas besoin d’appels API côté client (moins de latence)
* 📦 Meilleure gestion du SEO (rendu côté serveur)

❌ Limites des Server Components
* 🚫 Pas d’interactivité (pas de gestion d’événements comme onClick)
* 🚫 Pas de hooks client (useState, useEffect, etc.)
* 🚫 Pas d’accès aux APIs navigateur (window, localStorage…)
* 🔄 Nécessite des Client Components pour les parties dynamiques
* ⚠️ Debugging parfois plus complexe

==> ❌ IMPOSSIBLE - Import direct SC dans CC

<br>

**Client Components**

Les `Client Components` en React sont des composants qui s’exécutent dans le navigateur

<br>

**Server actions**

Les `Server Actions` sont des fonctions marquées avec ‘use server’ qui s’exécutent côté serveur et permettent de gérer des mutations sans passer par des API routes.

“En React et Next.js, ces hooks sont surtout utilisés avec les Server Actions :

`useFormState`: permet de gérer l’état d’un formulaire côté serveur (résultat, erreurs…)
`useFormStatus`: permet de savoir si un formulaire est en cours de soumission (loading, disabled…)
`useTransition`: permet de gérer des mises à jour non bloquantes pour garder une UI fluide
`useOptimistic`: permet de mettre à jour l’UI immédiatement avant la réponse du serveur (optimistic UI)

<br>

---

<br>

# 14 : Solutions de Styling

Les 3 Grandes Approches

| Approche | Exemple | Philosophie |
|----------|---------|-------------|
| **CSS-in-CSS** | CSS Modules | CSS traditionnel avec scoping |
| **CSS-in-JS** | Styled Components | CSS dans JavaScript |
| **Utility-first** | Tailwind CSS | Classes utilitaires composables |

<br>

---
<br>


# 15 : Techs utlisée

| Tech / librairie                        | Utilisée pour                                            | Besoin couvert                                        |
| --------------------------------------- | -------------------------------------------------------- | ----------------------------------------------------- |
| **React**                               | construire l’interface en composants                     | créer une SPA modulaire et réactive                   |
| **React Router**                        | gérer les routes et la navigation                        | naviguer entre pages sans recharger l’application     |
| **Next.js**                             | structurer des apps React plus complètes                 | routing, SSR, RSC, Server Actions                     |
| **useState**                            | gérer un state local simple                              | stocker des données propres à un composant            |
| **useReducer**                          | gérer un state plus complexe                             | centraliser des transitions d’état liées              |
| **Context API**                         | partager un state global simple                          | éviter le prop drilling pour l’UI state               |
| **Redux / Redux Toolkit**               | gérer un state global plus complexe                      | centraliser l’état UI et certaines actions async      |
| **React Query**                         | gérer le remote state / server state                     | fetch, cache, refetch, synchronisation avec API       |
| **React Hook Form**                     | gérer les formulaires                                    | simplifier validation, état du formulaire et erreurs  |
| **useEffect**                           | gérer les effets de bord                                 | appels API, abonnements, synchronisation externe      |
| **useRef**                              | conserver une valeur ou une référence DOM                | focus, accès DOM, valeur persistante sans re-render   |
| **React.memo**                          | éviter des re-renders inutiles                           | optimisation de performance                           |
| **useMemo**                             | mémoriser une valeur calculée                            | éviter des recalculs coûteux                          |
| **useCallback**                         | mémoriser une fonction                                   | éviter de recréer des callbacks à chaque render       |
| **CSS Modules**                         | styler des composants avec portée locale                 | éviter les conflits CSS                               |
| **Styled Components**                   | écrire du CSS dans les composants                        | styling component-scoped et dynamique                 |
| **Tailwind CSS**                        | styler rapidement l’interface                            | construire des UI rapidement avec classes utilitaires |
| **Vite**                                | initialiser et lancer les projets React                  | environnement de dev rapide et bundling moderne       |
| **json-server**                         | simuler une API REST                                     | tester le front sans backend réel                     |
| **Supabase**                            | backend/data pour certains projets                       | base de données, auth, services backend simples       |
| **date-fns**                            | manipulation de dates                                    | formatage et calculs de dates                         |
| **react-hot-toast**                     | notifications utilisateur                                | feedback visuel après action                          |
| **react-error-boundary**                | gestion d’erreurs React                                  | fallback UI en cas d’erreur                           |
| **Recharts**                            | graphiques et visualisation                              | afficher des données métier                           |
| **react-datepicker / react-day-picker** | sélection de dates                                       | champs date plus ergonomiques                         |
| **react-leaflet**                       | cartes interactives                                      | affichage de cartes et géolocalisation                |
| **faker-js**                            | génération de fausses données                            | tester l’UI avec des données simulées                 |
