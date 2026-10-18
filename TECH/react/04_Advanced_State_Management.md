## [**..**](./00_index.md)

# 📘 CHAPITRE 4 : Gestion d'État Avancée (useState, useReducer, Context API)

## 🎯 Objectifs du Chapitre
- Maîtriser useReducer pour state complexe
- Comprendre quand utiliser useReducer vs useState
- Maîtriser le Context API pour global state
- Éviter le prop drilling
- Comprendre les types de state (local, global, UI, remote)

---

## 1️⃣ FUNDAMENTALS OF STATE MANAGEMENT

### 🏠 **Qu'est-ce que le State Management ?**

> **State Management = Décider QUAND créer du state, QUEL TYPE de state, et OÙ le placer dans l'application**

```
STATE MANAGEMENT = 🏠 Donner à chaque piece of state un "home"
```

### 📊 **Les Deux Dimensions du State**

```jsx
// 1️⃣ STATE ACCESSIBILITY (Accessibilité)

// LOCAL STATE
// - Utilisé par un ou quelques composants
// - Défini dans un composant
// - Accessible au composant et ses enfants
function Counter() {
  const [count, setCount] = useState(0);  // Local state
  return <div>{count}</div>;
}


// GLOBAL STATE
// - Utilisé par beaucoup de composants
// - Accessible partout dans l'app
const [user, setUser] = useState(null);  // Dans App ou Context
```

```jsx
// 2️⃣ STATE DOMAIN (Domaine)

// UI STATE
// - Apparence de l'interface
// - Données qui n'existent que dans l'app
// - Synchrones
const [isOpen, setIsOpen] = useState(false);      // Modal open/close
const [theme, setTheme] = useState('dark');       // Dark mode
const [activeTab, setActiveTab] = useState('home'); // Navigation


// REMOTE STATE
// - Données venant du serveur (API)
// - Asynchrones (fetch, loading, error)
// - Besoin de synchronisation avec le serveur
const [products, setProducts] = useState([]);     // Depuis API
const [user, setUser] = useState(null);           // Depuis API
```

### 🗺️ **Matrice de State Management**

```
                    LOCAL STATE          GLOBAL STATE
                    -----------          ------------
UI STATE         |  Modal isOpen    |  Theme, Language    |
                 |  Form inputs     |  Auth status        |
                 |  Filter state    |  Shopping cart      |
                 |------------------|--------------------|
REMOTE STATE     |  Component data  |  User data          |
                 |  Paginated list  |  Products list      |
                 |  Single product  |  Global cache       |
                 |------------------|--------------------|
```

### 🎯 **Decision Tree : Où Placer le State ?**

```
❓ Besoin de stocker des données ?
   |
   ↓ OUI
   |
❓ Les données vont changer ?
   |
   ↓ OUI
   |
❓ Peut être calculé depuis state/props existant ?
   |
   ├─→ OUI → Derived state (pas de useState)
   |
   ↓ NON
   |
❓ Le composant doit re-render quand ça change ?
   |
   ├─→ NON → useRef
   |
   ↓ OUI
   |
❓ Utilisé seulement par ce composant ?
   |
   ├─→ OUI → Local state (useState dans le composant)
   |
   ↓ NON
   |
❓ Utilisé par des siblings ou quelques enfants ?
   |
   ├─→ OUI → Lift state up au parent commun
   |
   ↓ NON
   |
❓ Utilisé partout dans l'app ?
   |
   ↓ OUI
   |
❓ UI state ou Remote state ?
   |
   ├─→ UI → Context API ou Redux
   |
   └─→ Remote → React Query, SWR, RTK Query
```

---

## 2️⃣ useReducer : ADVANCED STATE MANAGEMENT

### 🎯 **Qu'est-ce que useReducer ?**

> **useReducer = Alternative à useState pour gérer du state complexe avec plusieurs variables liées**

```jsx
// useState (simple)
const [count, setCount] = useState(0);

// useReducer (complexe)
const [state, dispatch] = useReducer(reducer, initialState);
```

### 📊 **Quand utiliser useReducer ?**

```
✅ UTILISER useReducer QUAND:

1️⃣ Plusieurs state variables liées
   (ex: count, step, status ensemble)

2️⃣ Multiples state updates doivent se produire ensemble
   (ex: start game → reset score + set playing + init timer)

3️⃣ Update d'un state dépend d'un autre state
   (ex: nextQuestion dépend de currentQuestion et answers)

4️⃣ Logique de state complexe
   (beaucoup de if/else, switch cases)

5️⃣ State est un objet avec plusieurs propriétés
```

### 🔧 **Anatomie de useReducer**

```jsx
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
        ...state,
        count: state.count + state.step
      };
    
    case 'decrement':
      return {
        ...state,
        count: state.count - state.step
      };
    
    case 'setStep':
      return {
        ...state,
        step: action.payload
      };
    
    case 'reset':
      return initialState;
    
    case 'start':
      return {
        ...state,
        status: 'counting'
      };
    
    case 'pause':
      return {
        ...state,
        status: 'paused'
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
}
```

### 🎨 **Patterns avec useReducer**

#### **Pattern 1 : Actions avec Payload**

```jsx
// ACTION sans payload
dispatch({ type: 'reset' });

// ACTION avec payload (données)
dispatch({ 
  type: 'setUser', 
  payload: { name: 'Alice', email: 'alice@example.com' } 
});

dispatch({ 
  type: 'updateField', 
  payload: { field: 'email', value: 'new@example.com' } 
});

// REDUCER
function reducer(state, action) {
  switch (action.type) {
    case 'setUser':
      return {
        ...state,
        user: action.payload  // Tout l'objet
      };
    
    case 'updateField':
      return {
        ...state,
        user: {
          ...state.user,
          [action.payload.field]: action.payload.value
        }
      };
    
    default:
      return state;
  }
}
```

#### **Pattern 2 : Complex State Object**

```jsx
const initialState = {
  user: null,
  products: [],
  cart: [],
  filters: {
    category: 'all',
    minPrice: 0,
    maxPrice: 1000
  },
  ui: {
    isLoading: false,
    error: null,
    selectedProduct: null
  }
};

function reducer(state, action) {
  switch (action.type) {
    case 'setUser':
      return { ...state, user: action.payload };
    
    case 'setProducts':
      return { ...state, products: action.payload };
    
    case 'addToCart':
      return {
        ...state,
        cart: [...state.cart, action.payload]
      };
    
    case 'removeFromCart':
      return {
        ...state,
        cart: state.cart.filter(item => item.id !== action.payload)
      };
    
    case 'updateFilter':
      return {
        ...state,
        filters: {
          ...state.filters,
          [action.payload.key]: action.payload.value
        }
      };
    
    case 'setLoading':
      return {
        ...state,
        ui: { ...state.ui, isLoading: action.payload }
      };
    
    case 'setError':
      return {
        ...state,
        ui: { ...state.ui, error: action.payload }
      };
    
    default:
      return state;
  }
}
```

#### **Pattern 3 : Async Operations avec useReducer**

```jsx
function QuizApp() {
  const initialState = {
    questions: [],
    currentQuestion: 0,
    answer: null,
    points: 0,
    status: 'loading'  // 'loading' | 'ready' | 'active' | 'finished' | 'error'
  };
  
  function reducer(state, action) {
    switch (action.type) {
      case 'dataReceived':
        return {
          ...state,
          questions: action.payload,
          status: 'ready'
        };
      
      case 'dataFailed':
        return {
          ...state,
          status: 'error'
        };
      
      case 'start':
        return {
          ...state,
          status: 'active'
        };
      
      case 'newAnswer':
        const question = state.questions[state.currentQuestion];
        
        return {
          ...state,
          answer: action.payload,
          points: action.payload === question.correctOption
            ? state.points + question.points
            : state.points
        };
      
      case 'nextQuestion':
        return {
          ...state,
          currentQuestion: state.currentQuestion + 1,
          answer: null
        };
      
      case 'finish':
        return {
          ...state,
          status: 'finished'
        };
      
      case 'restart':
        return {
          ...initialState,
          questions: state.questions,
          status: 'ready'
        };
      
      default:
        throw new Error(`Unknown action: ${action.type}`);
    }
  }
  
  const [state, dispatch] = useReducer(reducer, initialState);
  
  // Fetch data
  useEffect(() => {
    fetch('http://localhost:8000/questions')
      .then(res => res.json())
      .then(data => dispatch({ type: 'dataReceived', payload: data }))
      .catch(err => dispatch({ type: 'dataFailed' }));
  }, []);
  
  return (
    <div>
      {state.status === 'loading' && <Loader />}
      {state.status === 'error' && <Error />}
      {state.status === 'ready' && (
        <StartScreen onStart={() => dispatch({ type: 'start' })} />
      )}
      {state.status === 'active' && (
        <>
          <Question
            question={state.questions[state.currentQuestion]}
            answer={state.answer}
            dispatch={dispatch}
          />
          <button onClick={() => dispatch({ type: 'nextQuestion' })}>
            Next
          </button>
        </>
      )}
      {state.status === 'finished' && (
        <FinishScreen
          points={state.points}
          onRestart={() => dispatch({ type: 'restart' })}
        />
      )}
    </div>
  );
}
```

### 📊 **useState vs useReducer : Comparaison**

| Critère | useState | useReducer |
|---------|----------|------------|
| **Use case** | 1-2 state variables simples | État complexe avec plusieurs variables liées |
| **Logique** | Dans event handlers (dispersée) | Centralisée dans reducer |
| **Updates** | Appeler setState | Dispatcher des actions |
| **Complexity** | ✅ Simple à comprendre | ⚠️ Plus complexe (boilerplate) |
| **Testability** | ⚠️ Difficile (couplé au composant) | ✅ Facile (reducer = pure function) |
| **Debug** | ⚠️ State updates partout | ✅ Toutes les transitions dans reducer |
| **Type safety** | ⚠️ Moins évident | ✅ Actions typées (avec TypeScript) |

```jsx
// ❌ useState devient difficile
function ComplexForm() {
  const [name, setName] = useState("");
  const [email, setEmail] = useState("");
  const [age, setAge] = useState(0);
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [errors, setErrors] = useState({});
  const [touched, setTouched] = useState({});
  
  // Beaucoup de setters, logique dispersée...
}

// ✅ useReducer plus clair
function ComplexForm() {
  const [state, dispatch] = useReducer(reducer, initialState);
  
  // Logique centralisée dans reducer
  // Actions claires: 'updateField', 'submit', 'setError', etc.
}
```

### 🎯 **Decision Guide : useState vs useReducer**

```
QUESTIONS:

❓ Une seule piece of state simple ?
   → useState

❓ État avec 3+ variables liées ?
   → useReducer

❓ Updates simples et indépendants ?
   → useState

❓ Logic complexe avec beaucoup de conditions ?
   → useReducer

❓ Plusieurs event handlers modifient le même state ?
   → useReducer

❓ État qui dépend d'un autre état ?
   → useReducer

❓ Besoin de tester la logique de state ?
   → useReducer (pure function)
```

---

## 3️⃣ CONTEXT API : GLOBAL STATE MANAGEMENT

### 🎯 **Le Problème : Prop Drilling**

```jsx
// ❌ PROP DRILLING (passer props à travers beaucoup de niveaux)

function App() {
  const [user, setUser] = useState(null);
  
  return <Dashboard user={user} setUser={setUser} />;
}

function Dashboard({ user, setUser }) {
  // Juste passe aux enfants, ne l'utilise pas
  return <Layout user={user} setUser={setUser} />;
}

function Layout({ user, setUser }) {
  // Juste passe aux enfants, ne l'utilise pas
  return (
    <div>
      <Sidebar user={user} setUser={setUser} />
      <Main user={user} setUser={setUser} />
    </div>
  );
}

function Sidebar({ user, setUser }) {
  // Juste passe aux enfants, ne l'utilise pas
  return <UserProfile user={user} setUser={setUser} />;
}

function UserProfile({ user, setUser }) {
  // ENFIN utilisé ici! 😤
  return <div>{user.name}</div>;
}

// Problème: user et setUser passent à travers 4 niveaux
// qui ne les utilisent pas!
```

### ✅ **La Solution : Context API**

> **Context API = Système pour passer des données à travers l'arbre de composants sans passer manuellement les props à chaque niveau**

```jsx
// ✅ AVEC CONTEXT API

// 1️⃣ CRÉER LE CONTEXT
import { createContext, useContext, useState } from 'react';

const UserContext = createContext();

// 2️⃣ CRÉER LE PROVIDER
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

// 4️⃣ WRAPPER L'APP
function App() {
  return (
    <UserProvider>
      <Dashboard />
    </UserProvider>
  );
}

// 5️⃣ UTILISER DANS N'IMPORTE QUEL COMPOSANT
function Dashboard() {
  // Pas besoin de props!
  return <Layout />;
}

function Layout() {
  // Pas besoin de props!
  return (
    <div>
      <Sidebar />
      <Main />
    </div>
  );
}

function Sidebar() {
  // Pas besoin de props!
  return <UserProfile />;
}

function UserProfile() {
  // ✅ Accès direct au context!
  const { user, setUser } = useUser();
  
  return (
    <div>
      <p>{user?.name}</p>
      <button onClick={() => setUser({ name: 'Alice' })}>
        Set User
      </button>
    </div>
  );
}
```

### 🎨 **Pattern : Context + useReducer**

```jsx
// COMBINAISON PUISSANTE: Context + useReducer

// 1️⃣ STATE & REDUCER
const initialState = {
  user: null,
  theme: 'light',
  language: 'en',
  isAuthenticated: false
};

function appReducer(state, action) {
  switch (action.type) {
    case 'login':
      return {
        ...state,
        user: action.payload,
        isAuthenticated: true
      };
    
    case 'logout':
      return {
        ...state,
        user: null,
        isAuthenticated: false
      };
    
    case 'setTheme':
      return {
        ...state,
        theme: action.payload
      };
    
    case 'setLanguage':
      return {
        ...state,
        language: action.payload
      };
    
    default:
      return state;
  }
}

// 2️⃣ CONTEXT
const AppContext = createContext();

// 3️⃣ PROVIDER
function AppProvider({ children }) {
  const [state, dispatch] = useReducer(appReducer, initialState);
  
  return (
    <AppContext.Provider value={{ state, dispatch }}>
      {children}
    </AppContext.Provider>
  );
}

// 4️⃣ CUSTOM HOOK
function useApp() {
  const context = useContext(AppContext);
  if (!context) {
    throw new Error('useApp must be used within AppProvider');
  }
  return context;
}

// 5️⃣ UTILISATION
function ThemeToggle() {
  const { state, dispatch } = useApp();
  
  const toggleTheme = () => {
    dispatch({
      type: 'setTheme',
      payload: state.theme === 'light' ? 'dark' : 'light'
    });
  };
  
  return (
    <button onClick={toggleTheme}>
      Current theme: {state.theme}
    </button>
  );
}

function LoginButton() {
  const { state, dispatch } = useApp();
  
  const handleLogin = () => {
    dispatch({
      type: 'login',
      payload: { name: 'Alice', email: 'alice@example.com' }
    });
  };
  
  return (
    <button onClick={handleLogin}>
      {state.isAuthenticated ? `Logged in as ${state.user.name}` : 'Login'}
    </button>
  );
}
```

### 🎯 **Exemple Complet : Auth Context**

```jsx
// contexts/AuthContext.js

import { createContext, useContext, useReducer, useEffect } from 'react';

const AuthContext = createContext();

const initialState = {
  user: null,
  isAuthenticated: false,
  isLoading: true,
  error: null
};

function authReducer(state, action) {
  switch (action.type) {
    case 'loading':
      return {
        ...state,
        isLoading: true,
        error: null
      };
    
    case 'login':
      return {
        user: action.payload,
        isAuthenticated: true,
        isLoading: false,
        error: null
      };
    
    case 'logout':
      return {
        user: null,
        isAuthenticated: false,
        isLoading: false,
        error: null
      };
    
    case 'error':
      return {
        ...state,
        isLoading: false,
        error: action.payload
      };
    
    default:
      throw new Error(`Unknown action: ${action.type}`);
  }
}

export function AuthProvider({ children }) {
  const [{ user, isAuthenticated, isLoading, error }, dispatch] = 
    useReducer(authReducer, initialState);
  
  // Check if user is logged in on mount
  useEffect(() => {
    const checkAuth = async () => {
      try {
        const token = localStorage.getItem('token');
        if (token) {
          const response = await fetch('/api/me', {
            headers: { Authorization: `Bearer ${token}` }
          });
          const data = await response.json();
          dispatch({ type: 'login', payload: data.user });
        } else {
          dispatch({ type: 'logout' });
        }
      } catch (err) {
        dispatch({ type: 'error', payload: err.message });
      }
    };
    
    checkAuth();
  }, []);
  
  const login = async (email, password) => {
    try {
      dispatch({ type: 'loading' });
      
      const response = await fetch('/api/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ email, password })
      });
      
      const data = await response.json();
      
      if (data.token) {
        localStorage.setItem('token', data.token);
        dispatch({ type: 'login', payload: data.user });
      }
    } catch (err) {
      dispatch({ type: 'error', payload: err.message });
    }
  };
  
  const logout = () => {
    localStorage.removeItem('token');
    dispatch({ type: 'logout' });
  };
  
  const value = {
    user,
    isAuthenticated,
    isLoading,
    error,
    login,
    logout
  };
  
  return (
    <AuthContext.Provider value={value}>
      {children}
    </AuthContext.Provider>
  );
}

export function useAuth() {
  const context = useContext(AuthContext);
  if (context === undefined) {
    throw new Error('useAuth must be used within AuthProvider');
  }
  return context;
}

// Usage dans App.js
function App() {
  return (
    <AuthProvider>
      <Router>
        <Routes>
          <Route path="/login" element={<LoginPage />} />
          <Route path="/dashboard" element={
            <ProtectedRoute>
              <Dashboard />
            </ProtectedRoute>
          } />
        </Routes>
      </Router>
    </AuthProvider>
  );
}

// ProtectedRoute component
function ProtectedRoute({ children }) {
  const { isAuthenticated, isLoading } = useAuth();
  
  if (isLoading) return <Loader />;
  
  return isAuthenticated ? children : <Navigate to="/login" />;
}

// LoginPage component
function LoginPage() {
  const { login, error } = useAuth();
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  
  const handleSubmit = (e) => {
    e.preventDefault();
    login(email, password);
  };
  
  return (
    <form onSubmit={handleSubmit}>
      {error && <p className="error">{error}</p>}
      <input
        type="email"
        value={email}
        onChange={(e) => setEmail(e.target.value)}
      />
      <input
        type="password"
        value={password}
        onChange={(e) => setPassword(e.target.value)}
      />
      <button type="submit">Login</button>
    </form>
  );
}

// Dashboard component
function Dashboard() {
  const { user, logout } = useAuth();
  
  return (
    <div>
      <h1>Welcome, {user.name}!</h1>
      <button onClick={logout}>Logout</button>
    </div>
  );
}
```

### 🎨 **Pattern : Multiple Contexts**

```jsx
// Séparer les contexts par responsabilité

// contexts/ThemeContext.js
const ThemeContext = createContext();

export function ThemeProvider({ children }) {
  const [theme, setTheme] = useState('light');
  
  return (
    <ThemeContext.Provider value={{ theme, setTheme }}>
      {children}
    </ThemeContext.Provider>
  );
}

export const useTheme = () => useContext(ThemeContext);


// contexts/AuthContext.js
const AuthContext = createContext();

export function AuthProvider({ children }) {
  const [user, setUser] = useState(null);
  
  return (
    <AuthContext.Provider value={{ user, setUser }}>
      {children}
    </AuthContext.Provider>
  );
}

export const useAuth = () => useContext(AuthContext);


// contexts/CartContext.js
const CartContext = createContext();

export function CartProvider({ children }) {
  const [cart, dispatch] = useReducer(cartReducer, []);
  
  return (
    <CartContext.Provider value={{ cart, dispatch }}>
      {children}
    </CartContext.Provider>
  );
}

export const useCart = () => useContext(CartContext);


// App.js - Composer les providers
function App() {
  return (
    <ThemeProvider>
      <AuthProvider>
        <CartProvider>
          <Router>
            <Routes>
              {/* ... */}
            </Routes>
          </Router>
        </CartProvider>
      </AuthProvider>
    </ThemeProvider>
  );
}
```

### ⚠️ **Performance : Context Updates**

```jsx
// ❌ PROBLÈME: Tous les consumers re-render même si
// ils n'utilisent pas la partie du state qui a changé

function AppProvider({ children }) {
  const [user, setUser] = useState(null);
  const [theme, setTheme] = useState('light');
  const [language, setLanguage] = useState('en');
  
  const value = { user, setUser, theme, setTheme, language, setLanguage };
  
  return (
    <AppContext.Provider value={value}>
      {children}
    </AppContext.Provider>
  );
}

// Si theme change, TOUS les composants qui utilisent useApp re-render
// même ceux qui utilisent seulement user!


// ✅ SOLUTION 1: Séparer en plusieurs contexts

<UserProvider>
  <ThemeProvider>
    <LanguageProvider>
      <App />
    </LanguageProvider>
  </ThemeProvider>
</UserProvider>


// ✅ SOLUTION 2: useMemo pour value

function AppProvider({ children }) {
  const [user, setUser] = useState(null);
  const [theme, setTheme] = useState('light');
  
  const value = useMemo(
    () => ({ user, setUser, theme, setTheme }),
    [user, theme]
  );
  
  return (
    <AppContext.Provider value={value}>
      {children}
    </AppContext.Provider>
  );
}


// ✅ SOLUTION 3: Séparer state et actions

function AppProvider({ children }) {
  const [state, dispatch] = useReducer(reducer, initialState);
  
  // state change souvent, actions jamais
  const actions = useMemo(() => ({
    login: (user) => dispatch({ type: 'login', payload: user }),
    logout: () => dispatch({ type: 'logout' }),
    setTheme: (theme) => dispatch({ type: 'setTheme', payload: theme })
  }), []);
  
  return (
    <AppStateContext.Provider value={state}>
      <AppActionsContext.Provider value={actions}>
        {children}
      </AppActionsContext.Provider>
    </AppStateContext.Provider>
  );
}
```

---

## 4️⃣ STATE MANAGEMENT TOOLS : VUE D'ENSEMBLE

### 🗺️ **Landscape des Solutions**

```jsx
// 1️⃣ LOCAL STATE (Component level)
// ✅ Tools: useState, useReducer

function Counter() {
  const [count, setCount] = useState(0);
  // Utilisé seulement ici
}


// 2️⃣ LIFTED STATE (Parent level)
// ✅ Tools: useState, useReducer

function Parent() {
  const [sharedState, setSharedState] = useState(null);
  return (
    <>
      <Child1 data={sharedState} />
      <Child2 onUpdate={setSharedState} />
    </>
  );
}


// 3️⃣ GLOBAL UI STATE
// ✅ Tools: Context API, Redux, Zustand

<ThemeProvider>
  <App />
</ThemeProvider>


// 4️⃣ GLOBAL REMOTE STATE
// ✅ Tools: React Query, SWR, RTK Query

function Products() {
  const { data, isLoading } = useQuery('products', fetchProducts);
}


// 5️⃣ URL STATE
// ✅ Tools: React Router

function SearchPage() {
  const [searchParams, setSearchParams] = useSearchParams();
  const query = searchParams.get('q');
}


// 6️⃣ BROWSER STORAGE
// ✅ Tools: localStorage, sessionStorage

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
}
```

### 📊 **Decision Matrix : Quel Tool Utiliser ?**

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

---

## 5️⃣ PATTERNS AVANCÉS

### 🎯 **Pattern 1 : Provider Pattern avec Composition**

```jsx
// Créer des providers composables et réutilisables

// Base Provider
function createContextProvider(useValue) {
  const Context = createContext();
  
  function Provider({ children }) {
    const value = useValue();
    
    return (
      <Context.Provider value={value}>
        {children}
      </Context.Provider>
    );
  }
  
  function useContextValue() {
    const context = useContext(Context);
    if (!context) {
      throw new Error('useContext must be used within Provider');
    }
    return context;
  }
  
  return [Provider, useContextValue];
}

// Usage
function useCounterValue() {
  const [count, setCount] = useState(0);
  const increment = () => setCount(c => c + 1);
  const decrement = () => setCount(c => c - 1);
  
  return { count, increment, decrement };
}

const [CounterProvider, useCounter] = createContextProvider(useCounterValue);

// Dans l'app
function App() {
  return (
    <CounterProvider>
      <Counter />
    </CounterProvider>
  );
}

function Counter() {
  const { count, increment, decrement } = useCounter();
  return (
    <div>
      <p>{count}</p>
      <button onClick={increment}>+</button>
      <button onClick={decrement}>-</button>
    </div>
  );
}
```

### 🎨 **Pattern 2 : State Machine Pattern**

```jsx
// Gérer les états complexes avec une machine à états

const STATES = {
  IDLE: 'idle',
  LOADING: 'loading',
  SUCCESS: 'success',
  ERROR: 'error'
};

const TRANSITIONS = {
  [STATES.IDLE]: {
    FETCH: STATES.LOADING
  },
  [STATES.LOADING]: {
    SUCCESS: STATES.SUCCESS,
    ERROR: STATES.ERROR
  },
  [STATES.SUCCESS]: {
    FETCH: STATES.LOADING
  },
  [STATES.ERROR]: {
    RETRY: STATES.LOADING,
    RESET: STATES.IDLE
  }
};

function stateMachineReducer(state, action) {
  const nextState = TRANSITIONS[state.status]?.[action.type];
  
  if (!nextState) {
    console.warn(`Invalid transition: ${state.status} -> ${action.type}`);
    return state;
  }
  
  switch (action.type) {
    case 'FETCH':
      return { status: STATES.LOADING, data: null, error: null };
    
    case 'SUCCESS':
      return { status: STATES.SUCCESS, data: action.payload, error: null };
    
    case 'ERROR':
      return { status: STATES.ERROR, data: null, error: action.payload };
    
    case 'RETRY':
      return { status: STATES.LOADING, data: null, error: null };
    
    case 'RESET':
      return { status: STATES.IDLE, data: null, error: null };
    
    default:
      return state;
  }
}

function DataFetcher() {
  const [state, dispatch] = useReducer(stateMachineReducer, {
    status: STATES.IDLE,
    data: null,
    error: null
  });
  
  const fetchData = async () => {
    dispatch({ type: 'FETCH' });
    
    try {
      const response = await fetch('/api/data');
      const data = await response.json();
      dispatch({ type: 'SUCCESS', payload: data });
    } catch (error) {
      dispatch({ type: 'ERROR', payload: error.message });
    }
  };
  
  return (
    <div>
      {state.status === STATES.IDLE && (
        <button onClick={fetchData}>Fetch Data</button>
      )}
      
      {state.status === STATES.LOADING && <p>Loading...</p>}
      
      {state.status === STATES.SUCCESS && (
        <div>
          <pre>{JSON.stringify(state.data, null, 2)}</pre>
          <button onClick={fetchData}>Refresh</button>
        </div>
      )}
      
      {state.status === STATES.ERROR && (
        <div>
          <p>Error: {state.error}</p>
          <button onClick={() => dispatch({ type: 'RETRY' })}>Retry</button>
          <button onClick={() => dispatch({ type: 'RESET' })}>Reset</button>
        </div>
      )}
    </div>
  );
}
```

### 🎯 **Pattern 3 : Optimistic Updates**

```jsx
// Mettre à jour l'UI immédiatement, rollback si erreur

function TodoList() {
  const [todos, setTodos] = useState([]);
  const [optimisticTodos, setOptimisticTodos] = useState([]);
  
  const addTodo = async (text) => {
    const tempId = `temp-${Date.now()}`;
    const newTodo = { id: tempId, text, completed: false };
    
    // 1️⃣ Optimistic update (immédiat)
    setOptimisticTodos([...optimisticTodos, newTodo]);
    
    try {
      // 2️⃣ Requête serveur
      const response = await fetch('/api/todos', {
        method: 'POST',
        body: JSON.stringify({ text })
      });
      const savedTodo = await response.json();
      
      // 3️⃣ Remplacer avec les vraies données
      setTodos([...todos, savedTodo]);
      setOptimisticTodos(optimisticTodos.filter(t => t.id !== tempId));
      
    } catch (error) {
      // 4️⃣ Rollback en cas d'erreur
      setOptimisticTodos(optimisticTodos.filter(t => t.id !== tempId));
      alert('Failed to add todo');
    }
  };
  
  const displayTodos = [...todos, ...optimisticTodos];
  
  return (
    <div>
      {displayTodos.map(todo => (
        <div key={todo.id} className={todo.id.startsWith('temp') ? 'pending' : ''}>
          {todo.text}
        </div>
      ))}
      <AddTodoForm onAdd={addTodo} />
    </div>
  );
}
```

### 🎨 **Pattern 4 : Undo/Redo avec useReducer**

```jsx
function createUndoReducer(reducer) {
  const initialUndoState = {
    past: [],
    present: null,
    future: []
  };
  
  return function undoReducer(undoState, action) {
    const { past, present, future } = undoState;
    
    switch (action.type) {
      case 'UNDO':
        if (past.length === 0) return undoState;
        
        return {
          past: past.slice(0, -1),
          present: past[past.length - 1],
          future: [present, ...future]
        };
      
      case 'REDO':
        if (future.length === 0) return undoState;
        
        return {
          past: [...past, present],
          present: future[0],
          future: future.slice(1)
        };
      
      case 'RESET':
        return {
          past: [],
          present: action.payload,
          future: []
        };
      
      default:
        // Regular action
        const newPresent = reducer(present, action);
        
        if (present === newPresent) return undoState;
        
        return {
          past: [...past, present],
          present: newPresent,
          future: []
        };
    }
  };
  
  return [undoReducer, initialUndoState];
}

// Usage
function DrawingApp() {
  const canvasReducer = (state, action) => {
    switch (action.type) {
      case 'ADD_LINE':
        return [...state, action.payload];
      case 'CLEAR':
        return [];
      default:
        return state;
    }
  };
  
  const [undoReducer, initialUndoState] = createUndoReducer(canvasReducer);
  const [state, dispatch] = useReducer(undoReducer, {
    ...initialUndoState,
    present: []
  });
  
  const { past, present, future } = state;
  
  return (
    <div>
      <button 
        onClick={() => dispatch({ type: 'UNDO' })}
        disabled={past.length === 0}
      >
        Undo
      </button>
      
      <button 
        onClick={() => dispatch({ type: 'REDO' })}
        disabled={future.length === 0}
      >
        Redo
      </button>
      
      <Canvas 
        lines={present}
        onAddLine={(line) => dispatch({ type: 'ADD_LINE', payload: line })}
      />
    </div>
  );
}
```

---

## 6️⃣ BEST PRACTICES

### ✅ **Do's (À Faire)**

```jsx
// ✅ 1. Toujours vérifier si le context existe
function useMyContext() {
  const context = useContext(MyContext);
  if (context === undefined) {
    throw new Error('useMyContext must be used within MyProvider');
  }
  return context;
}

// ✅ 2. Utiliser useMemo pour value du Provider
function MyProvider({ children }) {
  const [state, setState] = useState(initialState);
  
  const value = useMemo(
    () => ({ state, setState }),
    [state]
  );
  
  return <MyContext.Provider value={value}>{children}</MyContext.Provider>;
}

// ✅ 3. Séparer state et actions dans Context
const StateContext = createContext();
const ActionsContext = createContext();

// ✅ 4. Nommer les actions clairement
dispatch({ type: 'user/login', payload: user });  // ✅
dispatch({ type: 'SET_USER', payload: user });    // ✅
dispatch({ type: 'a', payload: user });           // ❌

// ✅ 5. Valider les actions dans le reducer
function reducer(state, action) {
  switch (action.type) {
    case 'addTodo':
      if (!action.payload || !action.payload.text) {
        throw new Error('addTodo requires text');
      }
      return [...state, action.payload];
    
    default:
      throw new Error(`Unknown action: ${action.type}`);
  }
}

// ✅ 6. Garder les reducers purs (no side effects)
function reducer(state, action) {
  // ❌ NO: API calls, setTimeout, random, Date.now()
  // ✅ YES: Pure calculations only
  
  switch (action.type) {
    case 'increment':
      return { count: state.count + 1 };  // ✅ Pure
    
    default:
      return state;
  }
}

// ✅ 7. Utiliser des constantes pour action types
const ACTIONS = {
  INCREMENT: 'INCREMENT',
  DECREMENT: 'DECREMENT',
  RESET: 'RESET'
};

dispatch({ type: ACTIONS.INCREMENT });  // ✅ Autocomplete + typo safety
```

### ❌ **Don'ts (À Éviter)**

```jsx
// ❌ 1. Ne pas créer de context sans Provider check
function useMyContext() {
  return useContext(MyContext);  // ❌ Peut retourner undefined
}

// ❌ 2. Ne pas créer de nouveau objet à chaque render
function MyProvider({ children }) {
  const [state, setState] = useState(initialState);
  
  // ❌ Nouvel objet à chaque render → tous les consumers re-render
  return (
    <MyContext.Provider value={{ state, setState }}>
      {children}
    </MyContext.Provider>
  );
}

// ❌ 3. Ne pas mettre trop de données dans un seul context
const AppContext = createContext();  // ❌ user, theme, cart, notifications...
// Mieux: Séparer en plusieurs contexts

// ❌ 4. Ne pas utiliser Context pour state qui change souvent
// Si le state change plusieurs fois par seconde → pas de Context
// Utiliser plutôt: Redux, Zustand, ou state local

// ❌ 5. Ne pas oublier le default case dans reducer
function reducer(state, action) {
  switch (action.type) {
    case 'increment':
      return { count: state.count + 1 };
    // ❌ Pas de default → typo dans action.type = silent bug
  }
}

// ✅ Toujours avoir un default
function reducer(state, action) {
  switch (action.type) {
    case 'increment':
      return { count: state.count + 1 };
    
    default:
      throw new Error(`Unknown action: ${action.type}`);
  }
}

// ❌ 6. Ne pas muter le state dans reducer
function reducer(state, action) {
  switch (action.type) {
    case 'addItem':
      state.items.push(action.payload);  // ❌ MUTATION!
      return state;
  }
}

// ✅ Toujours retourner un nouveau state
function reducer(state, action) {
  switch (action.type) {
    case 'addItem':
      return {
        ...state,
        items: [...state.items, action.payload]
      };
  }
}
```

---

## 🎯 QUESTIONS D'ENTRETIEN FRÉQUENTES

### Q1: **Quelle est la différence entre useState et useReducer ?**
**Réponse :**
```
useState:
✅ Simple state (1-2 variables)
✅ Updates indépendants
✅ Logique dispersée dans event handlers
❌ Difficile pour state complexe

useReducer:
✅ State complexe (3+ variables liées)
✅ Updates groupés et dépendants
✅ Logique centralisée dans reducer
✅ Plus facile à tester (pure function)
✅ Meilleur pour debugging (actions tracées)

QUAND UTILISER useReducer?
1. État avec plusieurs propriétés liées
2. Next state dépend du previous state
3. Logique complexe avec conditions
4. Besoin de tester la logique séparément
```

### Q2: **Qu'est-ce que le Context API et pourquoi l'utiliser ?**
**Réponse :**
```jsx
Context API = Système pour partager des données globalement
sans prop drilling

PROBLÈME RÉSOLU:
// ❌ Sans Context (prop drilling)
App → A → B → C → D (passer user à travers tous)

// ✅ Avec Context
App (Provider) 
  → A, B, C, D (accès direct via useContext)

QUAND UTILISER?
✅ Theme, language, auth (UI global state)
✅ User preferences
✅ State partagé par beaucoup de composants

QUAND NE PAS UTILISER?
❌ State local à 1-2 composants
❌ State qui change très fréquemment (perf)
❌ Remote data (utiliser React Query à la place)
```

### Q3: **Comment éviter les re-renders inutiles avec Context ?**
**Réponse :**
```jsx
PROBLÈME: Tous les consumers re-render quand value change

SOLUTIONS:

1️⃣ Séparer en plusieurs contexts
<UserContext>
  <ThemeContext>
    <App />
  </ThemeContext>
</UserContext>

2️⃣ useMemo pour value
const value = useMemo(
  () => ({ state, actions }),
  [state]
);

3️⃣ Séparer state et actions
<StateContext.Provider value={state}>
  <ActionsContext.Provider value={actions}>
    {children}
  </ActionsContext.Provider>
</StateContext.Provider>

4️⃣ Utiliser React.memo sur consumers
const ExpensiveComponent = memo(function Expensive() {
  // ...
});
```

### Q4: **Expliquez le pattern Provider/Consumer**
**Réponse :**
```jsx
PATTERN EN 4 ÉTAPES:

1️⃣ Créer le Context
const MyContext = createContext();

2️⃣ Créer le Provider (wrapper avec state)
function MyProvider({ children }) {
  const [state, setState] = useState(initialState);
  return (
    <MyContext.Provider value={{ state, setState }}>
      {children}
    </MyContext.Provider>
  );
}

3️⃣ Wrapper l'app
<MyProvider>
  <App />
</MyProvider>

4️⃣ Consumer (utiliser le context)
function Component() {
  const { state, setState } = useContext(MyContext);
}

BEST PRACTICE: Custom hook
function useMyContext() {
  const context = useContext(MyContext);
  if (!context) throw new Error('Must be in Provider');
  return context;
}
```

### Q5: **Comment combiner Context API et useReducer ?**
**Réponse :**
```jsx
// PATTERN PUISSANT pour state management complexe

function MyProvider({ children }) {
  const [state, dispatch] = useReducer(reducer, initialState);
  
  // Actions helpers (optionnel)
  const actions = useMemo(() => ({
    login: (user) => dispatch({ type: 'login', payload: user }),
    logout: () => dispatch({ type: 'logout' })
  }), []);
  
  return (
    <StateContext.Provider value={state}>
      <DispatchContext.Provider value={dispatch}>
        {/* ou ActionsContext avec helpers */}
        {children}
      </DispatchContext.Provider>
    </StateContext.Provider>
  );
}

AVANTAGES:
✅ State complexe (useReducer)
✅ Global access (Context)
✅ Logique centralisée (reducer)
✅ Actions typées et tracées
✅ Facile à tester et débugger
```

### Q6: **Différence entre Context API et Redux ?**
**Réponse :**
```
Context API:
✅ Built-in React (pas de lib externe)
✅ Simple à setup
✅ Bon pour UI state simple
❌ Pas de middleware
❌ Pas de DevTools sophistiqués
❌ Performance issues si mal utilisé

Redux:
✅ Excellent DevTools (time travel)
✅ Middleware (thunks, sagas)
✅ Performance optimisée
✅ Pattern éprouvé (predictable)
❌ Boilerplate (Redux Toolkit aide)
❌ Courbe d'apprentissage
❌ Lib externe (bundle size)

QUAND UTILISER QUOI?
- Context: Small/medium apps, UI state simple
- Redux: Large apps, complex state, besoin middleware
- React Query: Remote state (meilleur que les deux!)
```

### Q7: **Qu'est-ce qu'un reducer et pourquoi doit-il être pure ?**
**Réponse :**
```jsx
REDUCER = Fonction pure qui prend (state, action) → newState

PURE FUNCTION signifie:
1. Même input → même output (déterministe)
2. Pas de side effects (no API, no mutations, no random)
3. Ne modifie pas ses arguments

// ✅ PURE
function reducer(state, action) {
  switch (action.type) {
    case 'increment':
      return { count: state.count + 1 };  // Nouveau objet
    default:
      return state;
  }
}

// ❌ IMPURE
function reducer(state, action) {
  state.count++;  // ❌ Mutation
  fetch('/api');  // ❌ Side effect
  return state;
}

POURQUOI PURE?
1. Prédictibilité (même action = même résultat)
2. Testabilité (pas de mocks nécessaires)
3. Time-travel debugging possible
4. Optimisations React (shallow comparison)
5. Pas de bugs cachés (side effects)
```

### Q8: **Comment gérer async operations avec useReducer ?**
**Réponse :**
```jsx
// useReducer ne gère PAS l'async directement
// On gère l'async AVANT de dispatcher

async function fetchData(dispatch) {
  // 1️⃣ Dispatch loading
  dispatch({ type: 'loading' });
  
  try {
    // 2️⃣ Async operation
    const response = await fetch('/api/data');
    const data = await response.json();
    
    // 3️⃣ Dispatch success
    dispatch({ type: 'success', payload: data });
    
  } catch (error) {
    // 4️⃣ Dispatch error
    dispatch({ type: 'error', payload: error.message });
  }
}

// Reducer gère les états
function reducer(state, action) {
  switch (action.type) {
    case 'loading':
      return { ...state, isLoading: true, error: null };
    case 'success':
      return { isLoading: false, data: action.payload, error: null };
    case 'error':
      return { isLoading: false, data: null, error: action.payload };
    default:
      return state;
  }
}

// Usage
function Component() {
  const [state, dispatch] = useReducer(reducer, initialState);
  
  useEffect(() => {
    fetchData(dispatch);
  }, []);
}
```

---

## ⚠️ PIÈGES COURANTS

### 1. **Oublier le Error Handling dans Context**
```jsx
// ❌ Pas de vérification
function useMyContext() {
  return useContext(MyContext);  // undefined si hors Provider!
}

// ✅ Toujours vérifier
function useMyContext() {
  const context = useContext(MyContext);
  if (context === undefined) {
    throw new Error('useMyContext must be within MyProvider');
  }
  return context;
}
```

### 2. **Créer un Nouveau Objet à Chaque Render**
```jsx
// ❌ value change à chaque render → tous re-render
function Provider({ children }) {
  const [state, setState] = useState({});
  return (
    <Context.Provider value={{ state, setState }}>
      {children}
    </Context.Provider>
  );
}

// ✅ useMemo pour stabiliser
function Provider({ children }) {
  const [state, setState] = useState({});
  const value = useMemo(() => ({ state, setState }), [state]);
  return <Context.Provider value={value}>{children}</Context.Provider>;
}
```

### 3. **Muter le State dans Reducer**
```jsx
// ❌ MUTATION
function reducer(state, action) {
  state.count++;  // ❌
  return state;
}

// ✅ IMMUTABILITÉ
function reducer(state, action) {
  return { ...state, count: state.count + 1 };  // ✅
}
```

### 4. **Oublier le Default Case**
```jsx
// ❌ Typo dans action.type = silent bug
function reducer(state, action) {
  switch (action.type) {
    case 'increment':
      return { count: state.count + 1 };
    // Pas de default!
  }
}

// ✅ Toujours un default
function reducer(state, action) {
  switch (action.type) {
    case 'increment':
      return { count: state.count + 1 };
    default:
      throw new Error(`Unknown action: ${action.type}`);
  }
}
```

### 5. **Utiliser Context pour State qui Change Souvent**
```jsx
// ❌ State qui change 60 fois/seconde → mauvaise perf
function MouseTracker() {
  const [position, setPosition] = useState({ x: 0, y: 0 });
  
  return (
    <MouseContext.Provider value={position}>
      {/* Tous les consumers re-render 60 fois/sec! */}
    </MouseContext.Provider>
  );
}

// ✅ Utiliser state local ou autre solution
```

---

## 🔗 LIENS AVEC VOS PROJETS

### **10-react-quiz**
```jsx
// useReducer + Context API
const QuizContext = createContext();

function QuizProvider({ children }) {
  const [state, dispatch] = useReducer(quizReducer, initialState);
  
  return (
    <QuizContext.Provider value={{ state, dispatch }}>
      {children}
    </QuizContext.Provider>
  );
}

// State complexe avec useReducer
const initialState = {
  questions: [],
  status: 'loading',  // 'loading' | 'error' | 'ready' | 'active' | 'finished'
  index: 0,
  answer: null,
  points: 0,
  highscore: 0,
  secondsRemaining: null
};
```

### **11-worldwise**
```jsx
// Context API pour global state
const CitiesContext = createContext();

function CitiesProvider({ children }) {
  const [cities, setCities] = useState([]);
  const [isLoading, setIsLoading] = useState(false);
  const [currentCity, setCurrentCity] = useState({});
  
  // Functions
  async function getCity(id) { /* ... */ }
  async function createCity(newCity) { /* ... */ }
  async function deleteCity(id) { /* ... */ }
  
  return (
    <CitiesContext.Provider value={{
      cities,
      isLoading,
      currentCity,
      getCity,
      createCity,
      deleteCity
    }}>
      {children}
    </CitiesContext.Provider>
  );
}
```

### **17-the-wild-oasis**
```jsx
// Context pour DarkMode
const DarkModeContext = createContext();

function DarkModeProvider({ children }) {
  const [isDarkMode, setIsDarkMode] = useLocalStorageState(
    false,
    'isDarkMode'
  );
  
  useEffect(() => {
    if (isDarkMode) {
      document.documentElement.classList.add('dark-mode');
    } else {
      document.documentElement.classList.remove('dark-mode');
    }
  }, [isDarkMode]);
  
  return (
    <DarkModeContext.Provider value={{ isDarkMode, setIsDarkMode }}>
      {children}
    </DarkModeContext.Provider>
  );
}
```

---

## 📝 RÉSUMÉ DU CHAPITRE

✅ **State Management** = Décider où placer chaque piece of state  
✅ **useReducer** = Pour state complexe avec variables liées  
✅ **Context API** = Pour partager state globalement (éviter prop drilling)  
✅ **Reducer** = Pure function (state, action) → newState  
✅ **Provider Pattern** = Wrapper + custom hook pour Context  
✅ **Context + useReducer** = Pattern puissant pour apps complexes  
✅ **Performance** = useMemo value, séparer contexts, mémoriser actions  
✅ **Types de state** = Local/Global × UI/Remote  

---

## 🎓 POINTS CLÉS

1. **useState vs useReducer** : Savoir quand utiliser chacun
2. **Context API** : Pattern complet (create, provider, hook)
3. **Prop drilling** : Problème et solutions (Context, composition)
4. **Pure functions** : Pourquoi les reducers doivent être purs
5. **Performance** : Issues avec Context et solutions
6. **Pattern Provider** : Custom hook avec error handling
7. **Combiner Context + useReducer** : Best practice
8. **Vos projets** : react-quiz, worldwise, wild-oasis

---