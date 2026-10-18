## [**..**](./00_index.md)

# CHAPITRE 10 : Optimisation de Performance - Deep Dive Complet

## 🎯 Introduction : Pourquoi la Performance est Cruciale

### Le Problème : Wasted Renders

```javascript
// Exemple de problème de performance
function App() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>Count: {count}</button>
      <ExpensiveComponent />  {/* Re-render à CHAQUE click ! */}
      <AnotherComponent />    {/* Re-render aussi ! */}
      <HeavyList />          {/* Re-render encore ! */}
    </div>
  );
}

// Problème : Tous les children re-render même si leurs props ne changent pas !
// → Wasted renders → UI lente → Mauvaise UX
```

**Impact sur l'UX :**
- 🐌 UI qui lag
- 🔋 Batterie qui se vide vite (mobile)
- 😤 Frustration utilisateur
- 💰 Perte de conversions/revenue

---

## 📊 Profiling : Identifier les Problèmes AVANT d'optimiser

### 1. React DevTools Profiler

```javascript
// Comment utiliser le Profiler :

// 1. Installer React DevTools (Chrome/Firefox extension)
// 2. Ouvrir DevTools → Onglet "Profiler"
// 3. Click "Record" → Interagir avec app → Stop
// 4. Analyser les résultats

// Ce que vous voyez :
// - Flame chart : Temps de render par composant
// - Ranked chart : Composants les plus lents
// - Commit details : Pourquoi un composant a re-render

// 🎯 Chercher :
// - Composants avec long render time (> 16ms = visible lag)
// - Composants qui re-render souvent sans raison
// - Deep component trees qui re-render en cascade
```

**Exemple d'analyse :**

```
Flame Chart Analysis:
┌─ App (2ms) ───────────────────────────────┐
│  ┌─ Header (0.5ms) ──────────────────┐   │
│  └─ Dashboard (1.5ms) ────────────────┘   │
│     ┌─ BookingsList (45ms) ⚠️ PROBLÈME   │
│     │  ┌─ BookingRow × 100 (40ms) ⚠️     │
│     │  └─ ...                             │
│     └─ StatsCards (5ms) ──────────────┘   │
└───────────────────────────────────────────┘

→ BookingsList est le bottleneck !
→ Investigate pourquoi BookingRow si lent
→ Peut-être virtualization nécessaire ?
```

### 2. Chrome Performance Tab

```javascript
// Comment utiliser :
// 1. Chrome DevTools → Performance tab
// 2. Record → Interact → Stop
// 3. Analyser :
//    - Main thread activity (JavaScript execution)
//    - Rendering (Layout, Paint, Composite)
//    - FPS graph (chercher les drops < 60fps)

// 🎯 Identifier :
// - Long tasks (> 50ms = blocking)
// - Forced reflows (layout thrashing)
// - Excessive re-paints
```

### 3. Profiling avec Code

```javascript
// Profiler component de React
import { Profiler } from 'react';

function onRenderCallback(
  id,        // "panel" - identifiant du Profiler
  phase,     // "mount" ou "update"
  actualDuration, // Temps passé à render
  baseDuration,   // Temps estimé sans memoization
  startTime,
  commitTime,
  interactions
) {
  console.log(`${id} ${phase} took ${actualDuration}ms`);
  
  if (actualDuration > 16) {
    console.warn(`⚠️ Slow render detected in ${id}`);
  }
}

function App() {
  return (
    <Profiler id="dashboard" onRender={onRenderCallback}>
      <Dashboard />
    </Profiler>
  );
}

// Production logging avec analytics
function onRenderCallback(id, phase, actualDuration) {
  if (actualDuration > 50) {
    analytics.track('slow_render', {
      component: id,
      duration: actualDuration,
      phase,
    });
  }
}
```

---

## 🛡️ React.memo : Prévenir les Re-renders Inutiles

### 1. Comprendre le Problème

```javascript
// ❌ Sans memo : Child re-render à CHAQUE parent render
function Parent() {
  const [count, setCount] = useState(0);
  const [name, setName] = useState('John');
  
  console.log('Parent render');
  
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>Count: {count}</button>
      <input value={name} onChange={(e) => setName(e.target.value)} />
      
      {/* Re-render à chaque count/name change, même si props identiques ! */}
      <ExpensiveChild data="static" />
    </div>
  );
}

function ExpensiveChild({ data }) {
  console.log('ExpensiveChild render'); // Log à CHAQUE parent render !
  
  // Expensive computation
  const result = Array(10000).fill(0).map((_, i) => i * 2);
  
  return <div>{data}</div>;
}

// Résultat :
// User tape dans input → Parent render → ExpensiveChild render (INUTILE !)
// User click button → Parent render → ExpensiveChild render (INUTILE !)
```

### 2. Solution : React.memo

```javascript
// ✅ Avec memo : Child re-render SEULEMENT si props changent
import { memo } from 'react';

const ExpensiveChild = memo(function ExpensiveChild({ data }) {
  console.log('ExpensiveChild render');
  
  const result = Array(10000).fill(0).map((_, i) => i * 2);
  
  return <div>{data}</div>;
});

// Maintenant :
// User tape dans input → Parent render → ExpensiveChild NO render ✅
// User click button → Parent render → ExpensiveChild NO render ✅
// Props 'data' change → ExpensiveChild render ✅
```

### 3. Memo avec Comparaison Personnalisée

```javascript
// Par défaut, memo fait shallow comparison des props
// Parfois, vous voulez custom comparison

const UserCard = memo(
  function UserCard({ user }) {
    console.log('UserCard render');
    return (
      <div>
        <h3>{user.name}</h3>
        <p>{user.email}</p>
      </div>
    );
  },
  (prevProps, nextProps) => {
    // Return true = NE PAS re-render
    // Return false = RE-render
    
    // Custom comparison : seulement si name ou email change
    return (
      prevProps.user.name === nextProps.user.name &&
      prevProps.user.email === nextProps.user.email
    );
    // On ignore les autres propriétés de user (lastLogin, etc.)
  }
);
```

### 4. Quand Utiliser memo ?

```javascript
// ✅ UTILISER memo quand :

// 1. Composant LOURD computationnellement
const HeavyChart = memo(function HeavyChart({ data }) {
  // Complex D3 calculations, canvas rendering, etc.
  const processedData = expensiveProcessing(data);
  return <Canvas data={processedData} />;
});

// 2. Composant re-render SOUVENT (parent volatile)
function VolatileParent() {
  const [time, setTime] = useState(Date.now());
  
  useEffect(() => {
    const interval = setInterval(() => setTime(Date.now()), 100);
    return () => clearInterval(interval);
  }, []);
  
  return (
    <div>
      <Clock time={time} />
      <StableComponent /> {/* Devrait être memo ! */}
    </div>
  );
}

// 3. Composant dans une LISTE
function UsersList({ users }) {
  return users.map(user => (
    <UserCard key={user.id} user={user} /> // Devrait être memo !
  ));
}

// 4. Composant avec ENFANTS (children prop)
const Container = memo(function Container({ children, title }) {
  return (
    <div>
      <h2>{title}</h2>
      {children}
    </div>
  );
});

// ❌ NE PAS utiliser memo quand :

// 1. Composant DÉJÀ rapide (< 1ms)
const SimpleDiv = ({ text }) => <div>{text}</div>;
// Pas besoin de memo ici

// 2. Props changent TOUJOURS
const AlwaysChanging = ({ timestamp }) => <div>{timestamp}</div>;
// memo inutile, props changent à chaque render

// 3. Optimisation PRÉMATURÉE
// Profile AVANT d'optimiser !
```

### 5. Piège : Objects et Arrays dans Props

```javascript
// ❌ PIÈGE : memo ne marche PAS ici !
function Parent() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>{count}</button>
      
      {/* Nouveau object {} à CHAQUE render ! */}
      <Child config={{ theme: 'dark' }} />
      
      {/* Nouveau array [] à CHAQUE render ! */}
      <Child items={[1, 2, 3]} />
      
      {/* Nouvelle function à CHAQUE render ! */}
      <Child onClick={() => console.log('click')} />
    </div>
  );
}

const Child = memo(function Child({ config, items, onClick }) {
  console.log('Child render'); // Re-render à CHAQUE fois ! 😱
  return <div>Child</div>;
});

// Pourquoi ? 
// {} !== {} (nouveau object à chaque fois)
// [] !== [] (nouveau array à chaque fois)
// function() !== function() (nouvelle fonction à chaque fois)

// memo compare avec === (reference equality)
// Nouveaux objects/arrays/functions = différentes references !
```

**Solution : useMemo et useCallback (voir sections suivantes)**

---

## 🧠 useMemo : Mémoriser des Valeurs Coûteuses

### 1. Comprendre le Problème

```javascript
// ❌ Sans useMemo : Recalcul à CHAQUE render
function Dashboard({ bookings }) {
  const [filter, setFilter] = useState('all');
  
  // 🐌 EXPENSIVE computation
  const stats = {
    total: bookings.length,
    revenue: bookings.reduce((sum, b) => sum + b.totalPrice, 0),
    avgPrice: bookings.reduce((sum, b) => sum + b.totalPrice, 0) / bookings.length,
    byStatus: bookings.reduce((acc, b) => {
      acc[b.status] = (acc[b.status] || 0) + 1;
      return acc;
    }, {}),
  };
  // Recalculé à CHAQUE render, même si bookings ne change pas !
  
  return (
    <div>
      <button onClick={() => setFilter('active')}>Active</button>
      <StatsCards stats={stats} />
    </div>
  );
}

// User click button → filter change → Component re-render
// → stats RECALCULÉ (inutile ! bookings n'a pas changé)
```

### 2. Solution : useMemo

```javascript
// ✅ Avec useMemo : Calcul SEULEMENT si bookings change
import { useMemo } from 'react';

function Dashboard({ bookings }) {
  const [filter, setFilter] = useState('all');
  
  const stats = useMemo(() => {
    console.log('📊 Calculating stats...');
    
    return {
      total: bookings.length,
      revenue: bookings.reduce((sum, b) => sum + b.totalPrice, 0),
      avgPrice: bookings.reduce((sum, b) => sum + b.totalPrice, 0) / bookings.length,
      byStatus: bookings.reduce((acc, b) => {
        acc[b.status] = (acc[b.status] || 0) + 1;
        return acc;
      }, {}),
    };
  }, [bookings]); // ← Dependencies : recalcule seulement si bookings change
  
  return (
    <div>
      <button onClick={() => setFilter('active')}>Active</button>
      <StatsCards stats={stats} />
    </div>
  );
}

// User click button → filter change → Component re-render
// → stats PAS recalculé ✅ (bookings n'a pas changé)
// → Même référence d'object retournée
```

### 3. useMemo pour Stabiliser les Références

```javascript
// Use case : Passer object stable à memo child
function Parent() {
  const [count, setCount] = useState(0);
  
  // ❌ Sans useMemo : nouveau object à chaque render
  const config = { theme: 'dark', lang: 'fr' };
  
  // ✅ Avec useMemo : même object reference
  const config = useMemo(() => ({ 
    theme: 'dark', 
    lang: 'fr' 
  }), []); // Pas de dependencies = créé une seule fois
  
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>{count}</button>
      <Child config={config} />
    </div>
  );
}

const Child = memo(function Child({ config }) {
  console.log('Child render');
  return <div>Child</div>;
});

// Sans useMemo : Child re-render à chaque count change
// Avec useMemo : Child NO re-render (config reference stable)
```

### 4. Exemples Pratiques

```javascript
// Exemple 1 : Filtrage/Tri expensive
function BookingsList({ bookings, searchTerm, sortBy }) {
  // Filtrage et tri coûteux sur grande liste
  const filteredBookings = useMemo(() => {
    console.log('🔍 Filtering and sorting...');
    
    return bookings
      .filter(booking => 
        booking.guestName.toLowerCase().includes(searchTerm.toLowerCase())
      )
      .sort((a, b) => {
        if (sortBy === 'date') return new Date(b.date) - new Date(a.date);
        if (sortBy === 'price') return b.totalPrice - a.totalPrice;
        return 0;
      });
  }, [bookings, searchTerm, sortBy]);
  // Recalcule seulement si bookings, searchTerm ou sortBy change
  
  return (
    <Table>
      {filteredBookings.map(booking => (
        <BookingRow key={booking.id} booking={booking} />
      ))}
    </Table>
  );
}

// Exemple 2 : Transformation de données
function ChartComponent({ rawData }) {
  const chartData = useMemo(() => {
    console.log('📈 Processing chart data...');
    
    // Complex data transformation pour D3/Recharts
    return rawData.map(item => ({
      x: new Date(item.date).getTime(),
      y: item.value,
      label: formatLabel(item),
      color: getColor(item.category),
    }));
  }, [rawData]);
  
  return <LineChart data={chartData} />;
}

// Exemple 3 : Regex compilation
function SearchComponent({ pattern, items }) {
  const regex = useMemo(() => {
    console.log('🔧 Compiling regex...');
    try {
      return new RegExp(pattern, 'i');
    } catch {
      return null;
    }
  }, [pattern]);
  
  const results = useMemo(() => {
    if (!regex) return [];
    return items.filter(item => regex.test(item.name));
  }, [items, regex]);
  
  return <ResultsList results={results} />;
}

// Exemple 4 : Context value optimization
function ThemeProvider({ children }) {
  const [theme, setTheme] = useState('light');
  
  // ✅ Memoize context value pour éviter re-render de tous consumers
  const value = useMemo(() => ({
    theme,
    toggleTheme: () => setTheme(t => t === 'light' ? 'dark' : 'light'),
  }), [theme]);
  
  return (
    <ThemeContext.Provider value={value}>
      {children}
    </ThemeContext.Provider>
  );
}
```

### 5. Quand Utiliser useMemo ?

```javascript
// ✅ UTILISER useMemo pour :

// 1. Calculs EXPENSIVE (> 5ms)
const expensiveResult = useMemo(() => {
  // Loops, sorts, filters sur grandes collections
  // Complex algorithms
  // Heavy transformations
}, [deps]);

// 2. Éviter re-renders de memo components
const config = useMemo(() => ({ theme, lang }), [theme, lang]);
<MemoChild config={config} />

// 3. Référence stable pour useEffect dependencies
const options = useMemo(() => ({ 
  retries: 3, 
  timeout: 5000 
}), []);

useEffect(() => {
  fetchData(options);
}, [options]); // Pas de loop infinite

// 4. Éviter recréation d'objets/arrays dans loops
function List({ items }) {
  return items.map(item => {
    // ❌ Sans useMemo : nouveau object à chaque render
    const style = { color: item.color, fontSize: item.size };
    
    // ✅ Avec useMemo (si item.color/size stables)
    const style = useMemo(() => ({
      color: item.color,
      fontSize: item.size,
    }), [item.color, item.size]);
    
    return <div style={style}>{item.name}</div>;
  });
}

// ❌ NE PAS utiliser useMemo pour :

// 1. Calculs SIMPLES (< 1ms)
// const doubled = useMemo(() => count * 2, [count]); // Inutile !
const doubled = count * 2; // Suffisant

// 2. Primitives (string, number, boolean)
// const name = useMemo(() => 'John', []); // Inutile !
const name = 'John'; // Suffisant

// 3. Optimisation PRÉMATURÉE
// Profile AVANT d'optimiser !
```

### 6. Pièges Courants

```javascript
// ❌ PIÈGE 1 : Dependencies manquantes
const filtered = useMemo(() => {
  return items.filter(item => item.category === selectedCategory);
}, [items]); // ⚠️ Manque selectedCategory !
// → Filtre pas updated quand selectedCategory change

// ✅ CORRECTION
const filtered = useMemo(() => {
  return items.filter(item => item.category === selectedCategory);
}, [items, selectedCategory]);

// ❌ PIÈGE 2 : useMemo pour side effects
const data = useMemo(() => {
  console.log('Fetching...'); // ⚠️ Side effect !
  fetchData(); // ⚠️ Side effect !
  return result;
}, []);
// useMemo est pour PURE calculations, pas side effects !

// ✅ CORRECTION : Utiliser useEffect pour side effects
useEffect(() => {
  fetchData();
}, []);

// ❌ PIÈGE 3 : useMemo plus coûteux que le calcul
const simple = useMemo(() => a + b, [a, b]); // Overhead > gain
// useMemo a un coût (memory, comparison) !

// ✅ CORRECTION
const simple = a + b; // Calcul trivial, pas de memo

// ❌ PIÈGE 4 : Object/Array dans dependencies
const result = useMemo(() => {
  return compute(data);
}, [data]); // Si data = {}, nouveau object à chaque render !
// → useMemo recalcule à chaque fois (inutile)

// ✅ CORRECTION : Dépendre des propriétés primitives
const result = useMemo(() => {
  return compute(data);
}, [data.id, data.name]); // Primitives stables
```

---

## 🔄 useCallback : Mémoriser des Fonctions

### 1. Comprendre le Problème

```javascript
// ❌ Sans useCallback : Nouvelle fonction à CHAQUE render
function Parent() {
  const [count, setCount] = useState(0);
  
  // 🔴 Nouvelle fonction créée à CHAQUE render !
  const handleClick = () => {
    console.log('Clicked!');
  };
  
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>{count}</button>
      <Child onClick={handleClick} />
    </div>
  );
}

const Child = memo(function Child({ onClick }) {
  console.log('Child render');
  return <button onClick={onClick}>Click me</button>;
});

// User click count button → Parent re-render
// → handleClick nouvelle fonction (nouvelle reference)
// → Child re-render (props changed) même si memo !
```

### 2. Solution : useCallback

```javascript
// ✅ Avec useCallback : Même fonction reference
import { useCallback } from 'react';

function Parent() {
  const [count, setCount] = useState(0);
  
  // ✅ Fonction memoized, même reference entre renders
  const handleClick = useCallback(() => {
    console.log('Clicked!');
  }, []); // Pas de dependencies = créée une seule fois
  
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>{count}</button>
      <Child onClick={handleClick} />
    </div>
  );
}

const Child = memo(function Child({ onClick }) {
  console.log('Child render');
  return <button onClick={onClick}>Click me</button>;
});

// User click count button → Parent re-render
// → handleClick MÊME reference
// → Child NO re-render ✅ (props unchanged)
```

### 3. useCallback avec Dependencies

```javascript
function SearchComponent() {
  const [query, setQuery] = useState('');
  const [results, setResults] = useState([]);
  
  // ❌ Sans dependencies : Stale closure !
  const handleSearch = useCallback(() => {
    console.log('Searching for:', query); // ⚠️ query toujours '' !
    fetchResults(query).then(setResults);
  }, []); // query capturée dans closure, jamais updated !
  
  // ✅ Avec dependencies : Nouvelle fonction quand query change
  const handleSearch = useCallback(() => {
    console.log('Searching for:', query); // ✅ query à jour
    fetchResults(query).then(setResults);
  }, [query]); // Recréer fonction quand query change
  
  return (
    <div>
      <input value={query} onChange={(e) => setQuery(e.target.value)} />
      <button onClick={handleSearch}>Search</button>
      <ResultsList results={results} />
    </div>
  );
}
```

### 4. Exemples Pratiques

```javascript
// Exemple 1 : Event handlers passés à memo children
function TodoApp() {
  const [todos, setTodos] = useState([]);
  
  const handleToggle = useCallback((id) => {
    setTodos(prev => prev.map(todo =>
      todo.id === id ? { ...todo, done: !todo.done } : todo
    ));
  }, []); // Pas de dependencies car utilise functional update
  
  const handleDelete = useCallback((id) => {
    setTodos(prev => prev.filter(todo => todo.id !== id));
  }, []);
  
  return (
    <div>
      {todos.map(todo => (
        <TodoItem
          key={todo.id}
          todo={todo}
          onToggle={handleToggle}
          onDelete={handleDelete}
        />
      ))}
    </div>
  );
}

const TodoItem = memo(function TodoItem({ todo, onToggle, onDelete }) {
  return (
    <div>
      <input
        type="checkbox"
        checked={todo.done}
        onChange={() => onToggle(todo.id)}
      />
      <span>{todo.text}</span>
      <button onClick={() => onDelete(todo.id)}>Delete</button>
    </div>
  );
});

// Exemple 2 : Fonction dans useEffect dependencies
function DataFetcher({ userId }) {
  const [data, setData] = useState(null);
  
  const fetchData = useCallback(async () => {
    const response = await fetch(`/api/users/${userId}`);
    const json = await response.json();
    setData(json);
  }, [userId]);
  
  useEffect(() => {
    fetchData();
  }, [fetchData]); // fetchData memoized = pas de loop infinite
  
  return <div>{data?.name}</div>;
}

// Exemple 3 : Callback dans custom hook
function useDebounce(callback, delay) {
  const callbackRef = useRef(callback);
  
  // Update ref sans recréer la debounced function
  useEffect(() => {
    callbackRef.current = callback;
  }, [callback]);
  
  const debouncedCallback = useCallback((...args) => {
    const timer = setTimeout(() => {
      callbackRef.current(...args);
    }, delay);
    
    return () => clearTimeout(timer);
  }, [delay]);
  
  return debouncedCallback;
}

// Usage
function SearchInput() {
  const [query, setQuery] = useState('');
  
  const handleSearch = (value) => {
    console.log('Searching:', value);
    fetchResults(value);
  };
  
  const debouncedSearch = useDebounce(handleSearch, 500);
  
  return (
    <input
      value={query}
      onChange={(e) => {
        setQuery(e.target.value);
        debouncedSearch(e.target.value);
      }}
    />
  );
}

// Exemple 4 : Callbacks dans Context
function CartProvider({ children }) {
  const [items, setItems] = useState([]);
  
  const addItem = useCallback((product) => {
    setItems(prev => [...prev, { ...product, quantity: 1 }]);
  }, []);
  
  const removeItem = useCallback((productId) => {
    setItems(prev => prev.filter(item => item.id !== productId));
  }, []);
  
  const updateQuantity = useCallback((productId, quantity) => {
    setItems(prev => prev.map(item =>
      item.id === productId ? { ...item, quantity } : item
    ));
  }, []);
  
  // ✅ Memoize context value
  const value = useMemo(() => ({
    items,
    addItem,
    removeItem,
    updateQuantity,
  }), [items, addItem, removeItem, updateQuantity]);
  
  return (
    <CartContext.Provider value={value}>
      {children}
    </CartContext.Provider>
  );
}
```

### 5. useCallback vs useMemo pour Fonctions

```javascript
// useCallback et useMemo peuvent tous deux mémoriser des fonctions
// Mais syntaxe différente :

// useCallback
const memoizedCallback = useCallback(() => {
  doSomething(a, b);
}, [a, b]);

// useMemo (équivalent)
const memoizedCallback = useMemo(() => {
  return () => doSomething(a, b);
}, [a, b]);

// useCallback est juste du sucre syntaxique pour :
// useMemo(() => fn, deps)

// ✅ Utiliser useCallback pour les fonctions (plus lisible)
// ✅ Utiliser useMemo pour les valeurs
```

### 6. Quand Utiliser useCallback ?

```javascript
// ✅ UTILISER useCallback pour :

// 1. Functions passées à memo components
const handleClick = useCallback(() => {}, []);
<MemoChild onClick={handleClick} />

// 2. Functions dans useEffect dependencies
const fetchData = useCallback(async () => {}, [userId]);
useEffect(() => { fetchData(); }, [fetchData]);

// 3. Functions dans custom hooks return
function useCustomHook() {
  const someFunction = useCallback(() => {}, []);
  return { someFunction };
}

// 4. Event handlers dans grandes listes
const handleDelete = useCallback((id) => {}, []);
{items.map(item => <Item onDelete={handleDelete} />)}

// ❌ NE PAS utiliser useCallback pour :

// 1. Event handlers locaux (pas passés en props)
// const handleClick = useCallback(() => {}, []); // Inutile !
<button onClick={() => console.log('click')}>Click</button>

// 2. Functions jamais passées à children
const localHelper = () => a + b; // Pas besoin de callback

// 3. Optimisation PRÉMATURÉE
// Profile AVANT d'optimiser !
```

### 7. Pièges Courants

```javascript
// ❌ PIÈGE 1 : Stale closure (dependencies manquantes)
function Counter() {
  const [count, setCount] = useState(0);
  
  const handleClick = useCallback(() => {
    console.log(count); // ⚠️ count toujours 0 !
  }, []); // Manque count dans dependencies
  
  return <button onClick={handleClick}>{count}</button>;
}

// ✅ CORRECTION 1 : Ajouter dependency
const handleClick = useCallback(() => {
  console.log(count);
}, [count]);

// ✅ CORRECTION 2 : Functional update (si setState)
const handleIncrement = useCallback(() => {
  setCount(c => c + 1); // ✅ Pas besoin de count en dependency
}, []);

// ❌ PIÈGE 2 : useCallback partout (sur-optimisation)
function Component() {
  // Tous ces useCallback sont INUTILES si pas de memo children
  const fn1 = useCallback(() => {}, []);
  const fn2 = useCallback(() => {}, []);
  const fn3 = useCallback(() => {}, []);
  
  return <div onClick={fn1}>...</div>; // Pas de child memoized !
}

// ✅ CORRECTION : useCallback seulement si nécessaire
function Component() {
  const fn1 = () => {}; // Suffisant
  const fn2 = () => {};
  const fn3 = () => {};
  
  return <div onClick={fn1}>...</div>;
}

// ❌ PIÈGE 3 : Object/Array dans dependencies
const handleClick = useCallback(() => {
  doSomething(config);
}, [config]); // Si config = {}, nouveau object → recréer function

// ✅ CORRECTION : Extraire propriétés primitives
const handleClick = useCallback(() => {
  doSomething(config);
}, [config.theme, config.lang]); // Primitives stables

// Ou utiliser useMemo pour stabiliser config
const config = useMemo(() => ({ theme, lang }), [theme, lang]);
const handleClick = useCallback(() => {
  doSomething(config);
}, [config]);
```

---

## 📦 Code Splitting et Lazy Loading

### 1. Comprendre le Problème du Bundle Size

```javascript
// ❌ Sans code splitting : Tout chargé d'un coup
import Dashboard from './Dashboard';
import Settings from './Settings';
import Reports from './Reports';
import AdminPanel from './AdminPanel'; // Lourd : 500KB !
import Analytics from './Analytics';   // Lourd : 800KB !

function App() {
  return (
    <Routes>
      <Route path="/dashboard" element={<Dashboard />} />
      <Route path="/settings" element={<Settings />} />
      <Route path="/reports" element={<Reports />} />
      <Route path="/admin" element={<AdminPanel />} />
      <Route path="/analytics" element={<Analytics />} />
    </Routes>
  );
}

// Problème :
// User visite /dashboard → Télécharge TOUT (2MB+)
// Même AdminPanel qu'il n'utilisera jamais ! 💸
// → Initial load TRÈS lent (5-10 secondes)
// → Mauvaise UX, bounce rate élevé
```

### 2. Solution : React.lazy et Suspense

```javascript
// ✅ Code splitting avec lazy loading
import { lazy, Suspense } from 'react';

// Import LAZY (chargé seulement quand nécessaire)
const Dashboard = lazy(() => import('./Dashboard'));
const Settings = lazy(() => import('./Settings'));
const Reports = lazy(() => import('./Reports'));
const AdminPanel = lazy(() => import('./AdminPanel'));
const Analytics = lazy(() => import('./Analytics'));

function App() {
  return (
    <Routes>
      {/* Suspense wrapper avec fallback */}
      <Route
        path="/dashboard"
        element={
          <Suspense fallback={<Spinner />}>
            <Dashboard />
          </Suspense>
        }
      />
      <Route
        path="/settings"
        element={
          <Suspense fallback={<Spinner />}>
            <Settings />
          </Suspense>
        }
      />
      <Route
        path="/admin"
        element={
          <Suspense fallback={<Spinner />}>
            <AdminPanel />
          </Suspense>
        }
      />
      {/* ... */}
    </Routes>
  );
}

// Maintenant :
// User visite /dashboard → Télécharge seulement Dashboard (200KB)
// User va sur /admin → Télécharge AdminPanel maintenant (500KB)
// → Initial load RAPIDE (200KB vs 2MB+)
// → Chaque route charge seulement ce qu'elle a besoin ✅
```

### 3. Pattern : Suspense Wrapper Réutilisable

```javascript
// Créer un wrapper réutilisable
function LazyRoute({ component: Component, fallback = <PageSpinner /> }) {
  return (
    <Suspense fallback={fallback}>
      <Component />
    </Suspense>
  );
}

// Usage propre
const Dashboard = lazy(() => import('./Dashboard'));
const Settings = lazy(() => import('./Settings'));

function App() {
  return (
    <Routes>
      <Route path="/dashboard" element={<LazyRoute component={Dashboard} />} />
      <Route path="/settings" element={<LazyRoute component={Settings} />} />
    </Routes>
  );
}
```

### 4. Préchargement (Prefetching)

```javascript
// Précharger une route AVANT que user clique
function Navigation() {
  // Précharger AdminPanel au hover
  const prefetchAdmin = () => {
    import('./AdminPanel'); // Démarre le chargement
  };
  
  return (
    <nav>
      <Link to="/dashboard">Dashboard</Link>
      <Link to="/settings">Settings</Link>
      <Link 
        to="/admin" 
        onMouseEnter={prefetchAdmin} // Précharge au hover
      >
        Admin Panel
      </Link>
    </nav>
  );
}

// Ou utiliser React Router data APIs (loaders)
const router = createBrowserRouter([
  {
    path: '/admin',
    lazy: async () => {
      const { AdminPanel } = await import('./AdminPanel');
      return { Component: AdminPanel };
    },
  },
]);
```

### 5. Code Splitting par Feature

```javascript
// Structure de dossiers
src/
├── features/
│   ├── auth/
│   │   └── index.js        // Export lazy
│   ├── bookings/
│   │   └── index.js
│   ├── cabins/
│   │   └── index.js
│   └── dashboard/
│       └── index.js
└── App.jsx

// features/bookings/index.js
export { default as BookingsList } from './BookingsList';
export { default as BookingDetail } from './BookingDetail';
export { default as CreateBooking } from './CreateBooking';

// App.jsx - Lazy load par feature
const Bookings = lazy(() => import('./features/bookings'));
const Cabins = lazy(() => import('./features/cabins'));
const Dashboard = lazy(() => import('./features/dashboard'));

function App() {
  return (
    <Suspense fallback={<AppSpinner />}>
      <Routes>
        <Route path="/dashboard" element={<Dashboard />} />
        <Route path="/bookings/*" element={<Bookings />} />
        <Route path="/cabins/*" element={<Cabins />} />
      </Routes>
    </Suspense>
  );
}
```

### 6. Lazy Loading de Composants Conditionnels

```javascript
// Lazy load modal seulement quand ouvert
function Dashboard() {
  const [isModalOpen, setIsModalOpen] = useState(false);
  
  // Lazy load du Modal
  const HeavyModal = lazy(() => import('./HeavyModal'));
  
  return (
    <div>
      <button onClick={() => setIsModalOpen(true)}>
        Open Modal
      </button>
      
      {isModalOpen && (
        <Suspense fallback={<ModalSkeleton />}>
          <HeavyModal onClose={() => setIsModalOpen(false)} />
        </Suspense>
      )}
    </div>
  );
}

// Modal chargé SEULEMENT quand user clique ✅
```

### 7. Lazy Loading de Bibliothèques Lourdes

```javascript
// Exemple : Chart library (souvent TRÈS lourd)
function ChartComponent({ data }) {
  const [ChartLib, setChartLib] = useState(null);
  
  useEffect(() => {
    // Charger Recharts seulement quand composant mount
    import('recharts').then(module => {
      setChartLib(() => module);
    });
  }, []);
  
  if (!ChartLib) return <ChartSkeleton />;
  
  const { LineChart, Line, XAxis, YAxis } = ChartLib;
  
  return (
    <LineChart width={600} height={300} data={data}>
      <XAxis dataKey="name" />
      <YAxis />
      <Line dataKey="value" />
    </LineChart>
  );
}

// Ou avec Suspense moderne
const Recharts = lazy(() => import('recharts'));

function ChartComponent({ data }) {
  return (
    <Suspense fallback={<ChartSkeleton />}>
      <LazyChart data={data} />
    </Suspense>
  );
}

function LazyChart({ data }) {
  const { LineChart, Line, XAxis, YAxis } = Recharts;
  // ...
}
```

### 8. Error Boundaries pour Lazy Loading

```javascript
// Gérer les erreurs de chargement
import { Component } from 'react';

class ErrorBoundary extends Component {
  state = { hasError: false, error: null };
  
  static getDerivedStateFromError(error) {
    return { hasError: true, error };
  }
  
  render() {
    if (this.state.hasError) {
      return (
        <div>
          <h2>Oops! Failed to load component</h2>
          <button onClick={() => window.location.reload()}>
            Reload Page
          </button>
        </div>
      );
    }
    
    return this.props.children;
  }
}

// Usage
function App() {
  const AdminPanel = lazy(() => import('./AdminPanel'));
  
  return (
    <ErrorBoundary>
      <Suspense fallback={<Spinner />}>
        <AdminPanel />
      </Suspense>
    </ErrorBoundary>
  );
}
```

---

## 🎨 Virtualization : Listes Massives

### 1. Le Problème des Grandes Listes

```javascript
// ❌ Sans virtualization : Render TOUS les items
function BookingsList({ bookings }) {
  // bookings = 10,000 items 😱
  return (
    <div>
      {bookings.map(booking => (
        <BookingRow key={booking.id} booking={booking} />
      ))}
    </div>
  );
}

// Problème :
// - 10,000 DOM nodes créés d'un coup
// - Initial render : 5-10 secondes 🐌
// - Scroll lag terrible
// - Memory usage énorme (crash possible sur mobile)
// - User voit seulement 20 rows à l'écran ! 99.8% gaspillé
```

### 2. Solution : React Window (Virtualization)

```javascript
// ✅ Avec react-window : Render seulement ce qui est visible
import { FixedSizeList } from 'react-window';

function BookingsList({ bookings }) {
  // Render seulement les rows visibles à l'écran
  const Row = ({ index, style }) => {
    const booking = bookings[index];
    return (
      <div style={style}>
        <BookingRow booking={booking} />
      </div>
    );
  };
  
  return (
    <FixedSizeList
      height={600}        // Hauteur du container
      itemCount={bookings.length} // Total items
      itemSize={80}       // Hauteur de chaque item
      width="100%"
    >
      {Row}
    </FixedSizeList>
  );
}

// Maintenant :
// - Seulement ~10 rows rendered à la fois (ceux visibles)
// - Initial render : < 100ms ⚡
// - Scroll buttery smooth
// - Memory usage minimal
// - Fonctionne même avec 100,000+ items !
```

### 3. Variable Size List (Hauteurs Différentes)

```javascript
import { VariableSizeList } from 'react-window';

function CommentsList({ comments }) {
  // Pour items de hauteurs différentes
  const getItemSize = (index) => {
    const comment = comments[index];
    // Calculer hauteur basée sur contenu
    const lines = Math.ceil(comment.text.length / 50);
    return 60 + (lines * 20); // Base height + text height
  };
  
  const Row = ({ index, style }) => {
    const comment = comments[index];
    return (
      <div style={style}>
        <CommentCard comment={comment} />
      </div>
    );
  };
  
  return (
    <VariableSizeList
      height={600}
      itemCount={comments.length}
      itemSize={getItemSize}
      width="100%"
    >
      {Row}
    </VariableSizeList>
  );
}
```

### 4. Grid Virtualization (Tableau 2D)

```javascript
import { FixedSizeGrid } from 'react-window';

function ImageGallery({ images }) {
  const COLUMN_COUNT = 4;
  const ROW_COUNT = Math.ceil(images.length / COLUMN_COUNT);
  
  const Cell = ({ columnIndex, rowIndex, style }) => {
    const index = rowIndex * COLUMN_COUNT + columnIndex;
    const image = images[index];
    
    if (!image) return null;
    
    return (
      <div style={style}>
        <img src={image.url} alt={image.title} />
      </div>
    );
  };
  
  return (
    <FixedSizeGrid
      columnCount={COLUMN_COUNT}
      columnWidth={200}
      height={600}
      rowCount={ROW_COUNT}
      rowHeight={200}
      width={800}
    >
      {Cell}
    </FixedSizeGrid>
  );
}
```

### 5. Virtualization avec React Query et Infinite Scroll

```javascript
import { useInfiniteQuery } from '@tanstack/react-query';
import { FixedSizeList } from 'react-window';
import InfiniteLoader from 'react-window-infinite-loader';

function InfiniteBookingsList() {
  const {
    data,
    fetchNextPage,
    hasNextPage,
    isFetchingNextPage,
  } = useInfiniteQuery({
    queryKey: ['bookings-infinite'],
    queryFn: ({ pageParam = 0 }) => fetchBookings(pageParam),
    getNextPageParam: (lastPage) => lastPage.nextCursor,
  });
  
  // Flatten toutes les pages
  const allBookings = data?.pages.flatMap(page => page.bookings) ?? [];
  const itemCount = hasNextPage ? allBookings.length + 1 : allBookings.length;
  
  // Vérifier si item est chargé
  const isItemLoaded = (index) => !hasNextPage || index < allBookings.length;
  
  // Charger plus d'items
  const loadMoreItems = isFetchingNextPage
    ? () => {}
    : () => fetchNextPage();
  
  const Row = ({ index, style }) => {
    if (!isItemLoaded(index)) {
      return <div style={style}>Loading...</div>;
    }
    
    const booking = allBookings[index];
    return (
      <div style={style}>
        <BookingRow booking={booking} />
      </div>
    );
  };
  
  return (
    <InfiniteLoader
      isItemLoaded={isItemLoaded}
      itemCount={itemCount}
      loadMoreItems={loadMoreItems}
    >
      {({ onItemsRendered, ref }) => (
        <FixedSizeList
          height={600}
          itemCount={itemCount}
          itemSize={80}
          onItemsRendered={onItemsRendered}
          ref={ref}
          width="100%"
        >
          {Row}
        </FixedSizeList>
      )}
    </InfiniteLoader>
  );
}
```

### 6. Quand Utiliser Virtualization ?

```javascript
// ✅ UTILISER virtualization pour :

// 1. Listes de 100+ items
const bookings = [...]; // 500 bookings
<VirtualizedList items={bookings} />

// 2. Items coûteux à render
const ComplexRow = ({ item }) => {
  // Heavy computations, images, charts
  return <ExpensiveComponent data={item} />;
};

// 3. Infinite scroll
<InfiniteVirtualizedList />

// 4. Grids/Tables massives
<VirtualizedGrid items={10000} columns={20} />

// ❌ NE PAS utiliser virtualization pour :

// 1. Petites listes (< 50 items)
const items = [1, 2, 3, 4, 5];
items.map(item => <Item />) // Suffisant

// 2. Items très simples
<div>Simple text</div> // Pas besoin

// 3. Listes avec hauteurs très dynamiques
// Virtualization difficile si hauteurs imprévisibles
```

---

## ⚡ Autres Optimisations Importantes

### 1. Debouncing et Throttling

```javascript
// DEBOUNCING : Attendre que user arrête de taper
function SearchInput() {
  const [query, setQuery] = useState('');
  const [debouncedQuery, setDebouncedQuery] = useState('');
  
  useEffect(() => {
    // Delay de 500ms après dernier keystroke
    const timer = setTimeout(() => {
      setDebouncedQuery(query);
    }, 500);
    
    return () => clearTimeout(timer);
  }, [query]);
  
  // Fetch seulement avec debouncedQuery
  const { data } = useQuery({
    queryKey: ['search', debouncedQuery],
    queryFn: () => searchAPI(debouncedQuery),
    enabled: debouncedQuery.length >= 3,
  });
  
  return (
    <input
      value={query}
      onChange={(e) => setQuery(e.target.value)}
      placeholder="Search..."
    />
  );
}

// Custom hook réutilisable
function useDebounce(value, delay = 500) {
  const [debouncedValue, setDebouncedValue] = useState(value);
  
  useEffect(() => {
    const timer = setTimeout(() => setDebouncedValue(value), delay);
    return () => clearTimeout(timer);
  }, [value, delay]);
  
  return debouncedValue;
}

// Usage
const debouncedQuery = useDebounce(query, 500);

// THROTTLING : Limiter fréquence d'exécution
function useThrottle(callback, delay = 1000) {
  const lastRun = useRef(Date.now());
  
  return useCallback((...args) => {
    const now = Date.now();
    
    if (now - lastRun.current >= delay) {
      callback(...args);
      lastRun.current = now;
    }
  }, [callback, delay]);
}

// Usage pour scroll events
function ScrollTracker() {
  const handleScroll = useThrottle(() => {
    console.log('Scroll position:', window.scrollY);
    // Track analytics, update UI, etc.
  }, 100); // Max 1 fois par 100ms
  
  useEffect(() => {
    window.addEventListener('scroll', handleScroll);
    return () => window.removeEventListener('scroll', handleScroll);
  }, [handleScroll]);
}
```

### 2. Image Optimization

```javascript
// 1. Lazy loading natif
function ImageGallery({ images }) {
  return (
    <div>
      {images.map(img => (
        <img
          key={img.id}
          src={img.url}
          alt={img.alt}
          loading="lazy" // ✅ Native lazy loading
          decoding="async" // ✅ Decode asynchrone
        />
      ))}
    </div>
  );
}

// 2. Responsive images
function ResponsiveImage({ src, alt }) {
  return (
    <picture>
      <source
        srcSet={`${src}-mobile.webp`}
        media="(max-width: 768px)"
        type="image/webp"
      />
      <source
        srcSet={`${src}-desktop.webp`}
        media="(min-width: 769px)"
        type="image/webp"
      />
      <img src={`${src}.jpg`} alt={alt} loading="lazy" />
    </picture>
  );
}

// 3. Progressive image loading (blur-up)
function ProgressiveImage({ placeholder, src, alt }) {
  const [currentSrc, setCurrentSrc] = useState(placeholder);
  const [isLoading, setIsLoading] = useState(true);
  
  useEffect(() => {
    const img = new Image();
    img.src = src;
    img.onload = () => {
      setCurrentSrc(src);
      setIsLoading(false);
    };
  }, [src]);
  
  return (
    <img
      src={currentSrc}
      alt={alt}
      style={{
        filter: isLoading ? 'blur(10px)' : 'none',
        transition: 'filter 0.3s',
      }}
    />
  );
}

// 4. Image CDN avec transformations
function OptimizedImage({ src, width, height, alt }) {
  // Utiliser Cloudinary, Imgix, etc.
  const optimizedSrc = `https://res.cloudinary.com/demo/image/fetch/w_${width},h_${height},f_auto,q_auto/${src}`;
  
  return <img src={optimizedSrc} alt={alt} loading="lazy" />;
}
```

### 3. Web Workers pour Calculs Lourds

```javascript
// worker.js
self.addEventListener('message', (e) => {
  const { data } = e.data;
  
  // Heavy computation dans worker (thread séparé)
  const result = data.map(item => {
    // Complex processing
    return processItem(item);
  });
  
  self.postMessage(result);
});

// Component
function HeavyComputation({ data }) {
  const [result, setResult] = useState(null);
  const [isProcessing, setIsProcessing] = useState(false);
  
  useEffect(() => {
    const worker = new Worker(new URL('./worker.js', import.meta.url));
    
    setIsProcessing(true);
    
    worker.postMessage({ data });
    
    worker.onmessage = (e) => {
      setResult(e.data);
      setIsProcessing(false);
      worker.terminate();
    };
    
    return () => worker.terminate();
  }, [data]);
  
  if (isProcessing) return <Spinner />;
  
  return <ResultDisplay result={result} />;
}
```

### 4. CSS Optimizations

```javascript
// 1. will-change pour animations
const AnimatedBox = styled.div`
  /* Avertir browser d'optimiser pour transform */
  will-change: transform;
  transition: transform 0.3s;
  
  &:hover {
    transform: scale(1.1);
  }
`;

// 2. contain pour isolation
const IsolatedComponent = styled.div`
  /* Isoler layout/paint de ce composant */
  contain: layout style paint;
`;

// 3. content-visibility pour lazy render
const LazySection = styled.section`
  /* Render seulement quand visible */
  content-visibility: auto;
  contain-intrinsic-size: 0 500px; /* Estimated height */
`;
```

### 5. Bundle Analysis et Tree Shaking

```bash
# Analyser bundle size
npm install --save-dev webpack-bundle-analyzer

# package.json
"scripts": {
  "analyze": "vite-bundle-visualizer"
}

npm run analyze
```

```javascript
// Importer seulement ce qu'on utilise (tree shaking)

// ❌ Mauvais : Import toute la library
import _ from 'lodash'; // 70KB !
const result = _.debounce(fn, 500);

// ✅ Bon : Import seulement la fonction
import debounce from 'lodash/debounce'; // 2KB
const result = debounce(fn, 500);

// ❌ Mauvais : Import tous les icons
import * as Icons from 'react-icons/fa'; // 1MB+ !
<Icons.FaHome />

// ✅ Bon : Import spécifique
import { FaHome } from 'react-icons/fa'; // 5KB
<FaHome />
```

### 6. Service Workers et Caching

```javascript
// service-worker.js
const CACHE_NAME = 'app-cache-v1';
const urlsToCache = [
  '/',
  '/static/css/main.css',
  '/static/js/main.js',
];

self.addEventListener('install', (event) => {
  event.waitUntil(
    caches.open(CACHE_NAME).then((cache) => {
      return cache.addAll(urlsToCache);
    })
  );
});

self.addEventListener('fetch', (event) => {
  event.respondWith(
    caches.match(event.request).then((response) => {
      // Cache hit - return response
      if (response) return response;
      
      // Cache miss - fetch from network
      return fetch(event.request);
    })
  );
});

// Register service worker
if ('serviceWorker' in navigator) {
  navigator.serviceWorker.register('/service-worker.js');
}
```

---

## 🎯 Stratégie d'Optimisation : Par Où Commencer ?

### 1. Méthodologie PROFILER → IDENTIFIER → OPTIMISER

```javascript
// ÉTAPE 1 : PROFILER (mesurer AVANT d'optimiser)
// - React DevTools Profiler
// - Chrome Performance tab
// - Lighthouse audit
// - Bundle analyzer

// ÉTAPE 2 : IDENTIFIER les bottlenecks
// Chercher :
// - Composants avec render time > 16ms
// - Composants qui re-render souvent
// - Bundles > 500KB
// - Images non-optimisées
// - Listes > 100 items

// ÉTAPE 3 : OPTIMISER par priorité
// 1. Quick wins (lazy loading, image optimization)
// 2. Wasted renders (memo, useMemo, useCallback)
// 3. Large lists (virtualization)
// 4. Heavy computations (web workers)
```

### 2. Checklist d'Optimisation Progressive

```javascript
// ✅ NIVEAU 1 : Optimisations Basiques (Tous les projets)
const optimizations = {
  // 1. Code splitting routes
  lazyRoutes: true,
  
  // 2. Image lazy loading
  lazyImages: true,
  
  // 3. Bundle analysis
  analyzedBundle: true,
  
  // 4. Removed console.logs en production
  noConsoleLogs: true,
  
  // 5. Minification (Vite/Webpack le fait)
  minified: true,
};

// ✅ NIVEAU 2 : Optimisations Intermédiaires (Projets moyens/grands)
const intermediateOpts = {
  // 1. React.memo sur composants lourds
  memoizedComponents: ['Dashboard', 'BookingsList', 'Chart'],
  
  // 2. useMemo pour calculs coûteux
  memoizedComputations: ['statsCalculation', 'dataTransformation'],
  
  // 3. useCallback pour fonctions passées en props
  memoizedCallbacks: ['handleDelete', 'handleUpdate'],
  
  // 4. React Query avec staleTime adapté
  queryOptimization: true,
  
  // 5. Debounce sur search inputs
  debouncedInputs: true,
};

// ✅ NIVEAU 3 : Optimisations Avancées (Grandes apps)
const advancedOpts = {
  // 1. Virtualization pour listes > 100 items
  virtualizedLists: true,
  
  // 2. Web workers pour calculs lourds
  webWorkers: true,
  
  // 3. Service workers pour caching
  serviceWorker: true,
  
  // 4. Prefetching stratégique
  prefetching: true,
  
  // 5. CDN pour assets statiques
  cdn: true,
};
```

### 3. Metrics à Suivre

```javascript
// Core Web Vitals (Google)
const metrics = {
  // LCP (Largest Contentful Paint) : < 2.5s
  lcp: 'Speed of main content load',
  target: '< 2.5 seconds',
  
  // FID (First Input Delay) : < 100ms
  fid: 'Time until page interactive',
  target: '< 100 milliseconds',
  
  // CLS (Cumulative Layout Shift) : < 0.1
  cls: 'Visual stability',
  target: '< 0.1',
  
  // FCP (First Contentful Paint) : < 1.8s
  fcp: 'Time to first content',
  target: '< 1.8 seconds',
  
  // TTI (Time to Interactive) : < 3.8s
  tti: 'Time until fully interactive',
  target: '< 3.8 seconds',
};

// Mesurer avec Lighthouse
// Chrome DevTools → Lighthouse → Run audit
```

---

## ⚠️ Pièges d'Optimisation Courants

### 1. Sur-Optimisation (Premature Optimization)

```javascript
// ❌ PIÈGE : Optimiser AVANT de mesurer
function SimpleComponent({ count }) {
  // Sur-optimisation inutile
  const doubled = useMemo(() => count * 2, [count]); // Overhead > gain
  const handleClick = useCallback(() => {
    console.log(count);
  }, [count]); // Pas de memo child, inutile
  
  return <div onClick={handleClick}>{doubled}</div>;
}

// ✅ CORRECTION : Mesurer d'abord, optimiser ensuite
function SimpleComponent({ count }) {
  const doubled = count * 2; // Suffisant
  const handleClick = () => console.log(count); // Suffisant
  
  return <div onClick={handleClick}>{doubled}</div>;
}

// Règle : "Profile before you optimize"
```

### 2. Memo sans Stabiliser les Props

```javascript
// ❌ PIÈGE : memo inutile avec props instables
const Child = memo(function Child({ config, onAction }) {
  return <div>Child</div>;
});

function Parent() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>{count}</button>
      {/* Nouveau object ET fonction à chaque render ! */}
      <Child 
        config={{ theme: 'dark' }}  // Nouveau {}
        onAction={() => {}}          // Nouvelle function
      />
      {/* memo NE MARCHE PAS ! Child re-render quand même */}
    </div>
  );
}

// ✅ CORRECTION : Stabiliser props
function Parent() {
  const [count, setCount] = useState(0);
  
  const config = useMemo(() => ({ theme: 'dark' }), []);
  const onAction = useCallback(() => {}, []);
  
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>{count}</button>
      <Child config={config} onAction={onAction} />
      {/* Maintenant memo fonctionne ! */}
    </div>
  );
}
```

### 3. Dependencies Manquantes dans useMemo/useCallback

```javascript
// ❌ PIÈGE : Dependencies incomplètes
function Component({ items, filter }) {
  const filtered = useMemo(() => {
    return items.filter(item => item.category === filter);
  }, [items]); // ⚠️ Manque filter !
  
  // filtered ne se met PAS à jour quand filter change ! 😱
}

// ✅ CORRECTION
const filtered = useMemo(() => {
  return items.filter(item => item.category === filter);
}, [items, filter]); // ✅ Toutes les dependencies
```

### 4. useEffect avec Dependencies Instables

```javascript
// ❌ PIÈGE : Object/Array dans dependencies → Loop infini
function Component() {
  const [data, setData] = useState([]);
  
  useEffect(() => {
    const options = { retries: 3 }; // Nouveau object à chaque render !
    fetchData(options).then(setData);
  }, [options]); // ⚠️ options change à chaque render → infinite loop !
}

// ✅ CORRECTION 1 : useMemo pour stabiliser
function Component() {
  const [data, setData] = useState([]);
  
  const options = useMemo(() => ({ retries: 3 }), []);
  
  useEffect(() => {
    fetchData(options).then(setData);
  }, [options]); // Stable maintenant
}

// ✅ CORRECTION 2 : Créer object DANS useEffect
function Component() {
  const [data, setData] = useState([]);
  
  useEffect(() => {
    const options = { retries: 3 }; // Créé inside useEffect
    fetchData(options).then(setData);
  }, []); // Pas de dependencies externes
}

// ✅ CORRECTION 3 : Dépendre des primitives
function Component({ maxRetries, timeout }) {
  const [data, setData] = useState([]);
  
  useEffect(() => {
    const options = { retries: maxRetries, timeout };
    fetchData(options).then(setData);
  }, [maxRetries, timeout]); // Primitives = stables
}
```

### 5. Virtualization avec Hauteurs Dynamiques

```javascript
// ❌ PIÈGE : FixedSizeList avec items de hauteurs différentes
function CommentsList({ comments }) {
  return (
    <FixedSizeList
      height={600}
      itemCount={comments.length}
      itemSize={80} // ⚠️ Tous 80px, mais certains commentaires plus longs !
      width="100%"
    >
      {({ index, style }) => {
        const comment = comments[index];
        return (
          <div style={style}>
            {/* Ce commentaire peut être 200px de haut ! */}
            <p>{comment.longText}</p>
          </div>
        );
      }}
    </FixedSizeList>
  );
}
// Résultat : Items se chevauchent ou espaces vides 😱

// ✅ CORRECTION : Utiliser VariableSizeList
import { VariableSizeList } from 'react-window';

function CommentsList({ comments }) {
  const listRef = useRef();
  
  // Calculer hauteur par item
  const getItemSize = (index) => {
    const comment = comments[index];
    const textLength = comment.text.length;
    const estimatedLines = Math.ceil(textLength / 50);
    return 40 + (estimatedLines * 20); // Base + text height
  };
  
  // Reset cache si comments change
  useEffect(() => {
    listRef.current?.resetAfterIndex(0);
  }, [comments]);
  
  return (
    <VariableSizeList
      ref={listRef}
      height={600}
      itemCount={comments.length}
      itemSize={getItemSize}
      width="100%"
    >
      {({ index, style }) => (
        <div style={style}>
          <CommentCard comment={comments[index]} />
        </div>
      )}
    </VariableSizeList>
  );
}
```

---

## 🎤 Questions d'Entretien sur la Performance

### Q1: "Explique la différence entre useMemo et useCallback. Quand utiliser l'un ou l'autre ?"

**Réponse experte :**

```
**useMemo** et **useCallback** servent tous deux à la memoization, 
mais pour des cas différents :

**useMemo** : Mémoriser une VALEUR (résultat d'un calcul)
- Syntax : useMemo(() => computation, [deps])
- Retourne : La valeur calculée
- Usage : Calculs coûteux, transformations de données, objects/arrays stables

Exemple :
const expensiveValue = useMemo(() => {
  return heavyComputation(data); // Calcul lourd
}, [data]);

**useCallback** : Mémoriser une FONCTION
- Syntax : useCallback(() => {}, [deps])
- Retourne : La fonction elle-même
- Usage : Event handlers passés à memo children, functions dans useEffect deps

Exemple :
const memoizedCallback = useCallback(() => {
  doSomething(a, b);
}, [a, b]);

**Note technique** : useCallback(fn, deps) est équivalent à 
useMemo(() => fn, deps). C'est juste du sucre syntaxique.

**Quand utiliser :**

useMemo :
- Calculs > 5ms
- Stabiliser objects/arrays pour memo children
- Transformations de données (filter, map, reduce sur grandes listes)

useCallback :
- Functions passées à memo components
- Functions dans useEffect dependencies
- Event handlers dans listes (map)

**Quand NE PAS utiliser :**
- Calculs simples (< 1ms)
- Composants sans memo
- Optimisation prématurée (toujours profiler d'abord)
```

### Q2: "Comment diagnostiquerais-tu et fixerais-tu un composant React qui lag ?"

**Réponse structurée :**

```javascript
// ÉTAPE 1 : MESURER avec React DevTools Profiler
// 1. Ouvrir DevTools → Profiler
// 2. Record → Interagir → Stop
// 3. Analyser flame chart

// Chercher :
// - Composants avec render time > 16ms
// - Composants qui re-render souvent
// - Deep trees qui re-render en cascade

// ÉTAPE 2 : IDENTIFIER le problème

// Problème A : Wasted renders
// Composant re-render alors que props n'ont pas changé

// Solution : React.memo
const ExpensiveComponent = memo(function ExpensiveComponent({ data }) {
  return <div>{/* Heavy rendering */}</div>;
});

// Problème B : Props instables
// Objects/functions recréés à chaque render

// Solution : useMemo/useCallback
const Parent = () => {
  const config = useMemo(() => ({ theme: 'dark' }), []);
  const handleClick = useCallback(() => {}, []);
  
  return <Child config={config} onClick={handleClick} />;
};

// Problème C : Calculs coûteux à chaque render
// Heavy computation répétée inutilement

// Solution : useMemo
const result = useMemo(() => {
  return expensiveCalculation(data);
}, [data]);

// Problème D : Grande liste (> 100 items)
// Trop de DOM nodes

// Solution : Virtualization
import { FixedSizeList } from 'react-window';
<FixedSizeList height={600} itemCount={1000} itemSize={50}>
  {Row}
</FixedSizeList>

// Problème E : Bundle trop lourd
// Initial load lent

// Solution : Code splitting
const HeavyComponent = lazy(() => import('./HeavyComponent'));
<Suspense fallback={<Spinner />}>
  <HeavyComponent />
</Suspense>

// ÉTAPE 3 : VALIDER l'amélioration
// Re-profile après optimisation
// Comparer avant/après
// Mesurer impact utilisateur réel (Lighthouse, Core Web Vitals)
```

### Q3: "Quelle est la différence entre React.memo et useMemo ?"

**Réponse :**

```
Ce sont deux outils DIFFÉRENTS pour deux problèmes différents :

**React.memo** : HOC (Higher-Order Component)
- Enveloppe un COMPOSANT entier
- Empêche re-render si props identiques
- Compare props avec shallow equality
- S'applique au niveau composant

Exemple :
const MyComponent = memo(function MyComponent({ data }) {
  return <div>{data.name}</div>;
});
// MyComponent ne re-render QUE si props.data change

**useMemo** : Hook React
- Mémorise une VALEUR (résultat de calcul)
- S'utilise DANS un composant
- Évite recalculs coûteux
- S'applique au niveau valeur

Exemple :
function MyComponent({ items }) {
  const sorted = useMemo(() => {
    return [...items].sort(); // Calcul coûteux
  }, [items]);
  
  return <div>{sorted.map(...)}</div>;
}
// sorted recalculé SEULEMENT si items change

**Utilisation combinée** (pattern courant) :
const ExpensiveChild = memo(function ExpensiveChild({ config }) {
  return <div>Heavy component</div>;
});

function Parent() {
  // useMemo pour stabiliser la prop
  const config = useMemo(() => ({ theme: 'dark' }), []);
  
  // memo pour éviter re-render
  return <ExpensiveChild config={config} />;
}

Sans useMemo → nouveau object → memo inutile
Avec useMemo → même object → memo fonctionne !
```

### Q4: "Comment optimiserais-tu une liste de 10,000 items ?"

**Réponse complète :**

```javascript
// APPROCHE 1 : Virtualization (Meilleure solution)
import { FixedSizeList } from 'react-window';

function LargeList({ items }) {
  // Render seulement les ~20 items visibles
  const Row = ({ index, style }) => (
    <div style={style}>
      <ItemCard item={items[index]} />
    </div>
  );
  
  return (
    <FixedSizeList
      height={600}
      itemCount={items.length} // 10,000
      itemSize={80}
      width="100%"
    >
      {Row}
    </FixedSizeList>
  );
}
// Performance : ⚡ Excellent (< 100ms initial render)
// Memory : Minimal (~20 nodes DOM au lieu de 10,000)

// APPROCHE 2 : Pagination (Alternative simple)
function PaginatedList({ items }) {
  const [page, setPage] = useState(1);
  const itemsPerPage = 50;
  
  const currentItems = useMemo(() => {
    const start = (page - 1) * itemsPerPage;
    return items.slice(start, start + itemsPerPage);
  }, [items, page]);
  
  return (
    <>
      {currentItems.map(item => <ItemCard key={item.id} item={item} />)}
      <Pagination page={page} setPage={setPage} />
    </>
  );
}
// Performance : ✅ Bon (50 items à la fois)
// UX : Moins fluide (clicks requis)

// APPROCHE 3 : Infinite Scroll + Virtualization (Optimal)
import { useInfiniteQuery } from '@tanstack/react-query';
import InfiniteLoader from 'react-window-infinite-loader';

function InfiniteVirtualList() {
  const { data, fetchNextPage, hasNextPage } = useInfiniteQuery({
    queryKey: ['items'],
    queryFn: ({ pageParam = 0 }) => fetchItems(pageParam),
    getNextPageParam: (lastPage) => lastPage.nextCursor,
  });
  
  const allItems = data?.pages.flatMap(page => page.items) ?? [];
  
  return (
    <InfiniteLoader
      isItemLoaded={(index) => index < allItems.length}
      itemCount={hasNextPage ? allItems.length + 1 : allItems.length}
      loadMoreItems={fetchNextPage}
    >
      {({ onItemsRendered, ref }) => (
        <FixedSizeList
          height={600}
          itemCount={allItems.length}
          itemSize={80}
          onItemsRendered={onItemsRendered}
          ref={ref}
        >
          {({ index, style }) => (
            <div style={style}>
              <ItemCard item={allItems[index]} />
            </div>
          )}
        </FixedSizeList>
      )}
    </InfiniteLoader>
  );
}
// Performance : 🚀 Optimal
// UX : Fluide, automatique
// Scalability : Fonctionne avec millions d'items

// OPTIMISATIONS ADDITIONNELLES :

// 1. Memoize ItemCard
const ItemCard = memo(function ItemCard({ item }) {
  return <div>{item.name}</div>;
});

// 2. Key stable (ID, pas index)
{items.map(item => <ItemCard key={item.id} item={item} />)}
// Pas key={index} !

// 3. Avoid inline objects/functions
// ❌ Mauvais
{items.map(item => <ItemCard style={{ color: 'red' }} />)}

// ✅ Bon
const cardStyle = { color: 'red' }; // Outside render
{items.map(item => <ItemCard style={cardStyle} />)}

// 4. Batch updates si modifications
startTransition(() => {
  setItems(newItems); // Low priority update
});
```

### Q5: "Comment mesures-tu la performance d'une app React en production ?"

**Réponse :**

```javascript
// MÉTHODE 1 : Web Vitals (Google Standards)
import { getCLS, getFID, getFCP, getLCP, getTTFB } from 'web-vitals';

function sendToAnalytics(metric) {
  const { name, value, id } = metric;
  
  // Envoyer à analytics (Google Analytics, Datadog, etc.)
  gtag('event', name, {
    value: Math.round(name === 'CLS' ? value * 1000 : value),
    metric_id: id,
  });
}

// Mesurer Core Web Vitals
getCLS(sendToAnalytics); // Cumulative Layout Shift
getFID(sendToAnalytics); // First Input Delay
getFCP(sendToAnalytics); // First Contentful Paint
getLCP(sendToAnalytics); // Largest Contentful Paint
getTTFB(sendToAnalytics); // Time to First Byte

// Targets :
// LCP < 2.5s
// FID < 100ms
// CLS < 0.1

// MÉTHODE 2 : React Profiler API (Custom metrics)
import { Profiler } from 'react';

function onRenderCallback(
  id,
  phase,
  actualDuration,
  baseDuration,
  startTime,
  commitTime
) {
  // Log slow renders
  if (actualDuration > 16) {
    analytics.track('slow_render', {
      component: id,
      duration: actualDuration,
      phase,
      timestamp: commitTime,
    });
  }
}

function App() {
  return (
    <Profiler id="app" onRender={onRenderCallback}>
      <Routes />
    </Profiler>
  );
}

// MÉTHODE 3 : Performance Observer API
const observer = new PerformanceObserver((list) => {
  for (const entry of list.getEntries()) {
    if (entry.entryType === 'navigation') {
      console.log('Page load time:', entry.loadEventEnd - entry.startTime);
    }
    
    if (entry.entryType === 'resource') {
      console.log('Resource loaded:', entry.name, entry.duration);
    }
  }
});

observer.observe({ entryTypes: ['navigation', 'resource', 'paint'] });

// MÉTHODE 4 : Real User Monitoring (RUM)
// Services : Datadog, New Relic, Sentry, LogRocket

// Exemple avec Sentry
import * as Sentry from '@sentry/react';

Sentry.init({
  dsn: 'YOUR_DSN',
  integrations: [
    new Sentry.BrowserTracing(),
    new Sentry.Replay(),
  ],
  tracesSampleRate: 0.1, // 10% des transactions
  replaysSessionSampleRate: 0.1,
});

// Track custom metrics
Sentry.setMeasurement('component_load_time', 150, 'millisecond');

// MÉTHODE 5 : Lighthouse CI (Automated tests)
// .lighthouserc.js
module.exports = {
  ci: {
    collect: {
      numberOfRuns: 3,
      url: ['http://localhost:3000'],
    },
    assert: {
      preset: 'lighthouse:recommended',
      assertions: {
        'first-contentful-paint': ['error', { maxNumericValue: 2000 }],
        'largest-contentful-paint': ['error', { maxNumericValue: 2500 }],
        'cumulative-layout-shift': ['error', { maxNumericValue: 0.1 }],
      },
    },
  },
};

// Dans CI/CD pipeline
npm run build
lhci autorun

// MÉTHODE 6 : Bundle Size Monitoring
// package.json
{
  "scripts": {
    "analyze": "vite-bundle-visualizer",
    "size": "size-limit"
  }
}

// size-limit.js
module.exports = [
  {
    path: 'dist/**/*.js',
    limit: '500 KB',
  },
];

// Alert si bundle > 500KB
```

### Q6: "Que ferais-tu si un composant render en 200ms au lieu de 16ms ?"

**Debugging étape par étape :**

```javascript
// ÉTAPE 1 : IDENTIFIER le problème exact

// A) Ouvrir React DevTools Profiler
// - Record pendant interaction problématique
// - Trouver le composant à 200ms
// - Voir pourquoi il re-render (props change? parent re-render?)

// B) Ajouter console.logs
function SlowComponent({ data }) {
  console.log('SlowComponent render');
  console.trace(); // Voir call stack
  
  return <div>...</div>;
}

// C) Mesurer avec performance.mark
function SlowComponent({ data }) {
  performance.mark('slow-start');
  
  // Component code...
  
  performance.mark('slow-end');
  performance.measure('slow-component', 'slow-start', 'slow-end');
  
  const measure = performance.getEntriesByName('slow-component')[0];
  console.log('Render time:', measure.duration, 'ms');
}

// ÉTAPE 2 : SOLUTIONS selon la cause

// CAUSE A : Calcul lourd dans render
function SlowComponent({ items }) {
  // ❌ Calcul à chaque render (200ms)
  const processed = items.map(item => {
    // Heavy processing
    return expensiveTransform(item);
  });
  
  // ✅ Solution : useMemo
  const processed = useMemo(() => {
    return items.map(item => expensiveTransform(item));
  }, [items]);
}

// CAUSE B : Grande liste non-virtualisée
function SlowComponent({ items }) {
  // ❌ Render 1000 items (200ms)
  return items.map(item => <HeavyRow key={item.id} item={item} />);
  
  // ✅ Solution : Virtualization
  return (
    <FixedSizeList height={600} itemCount={items.length} itemSize={80}>
      {({ index, style }) => (
        <div style={style}>
          <HeavyRow item={items[index]} />
        </div>
      )}
    </FixedSizeList>
  );
}

// CAUSE C : Re-renders en cascade
function Parent() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>{count}</button>
      {/* Tous ces children re-render ! */}
      <ExpensiveChild1 />
      <ExpensiveChild2 />
      <ExpensiveChild3 />
    </div>
  );
}

// ✅ Solution : React.memo
const ExpensiveChild1 = memo(function ExpensiveChild1() {
  return <div>Heavy component</div>;
});

// CAUSE D : Inline functions/objects
function Parent() {
  return items.map(item => (
    // ❌ Nouvelle fonction à chaque render !
    <Child 
      key={item.id}
      onClick={() => handleClick(item.id)}
      config={{ theme: 'dark' }}
    />
  ));
}

// ✅ Solution : useCallback + useMemo
const handleClick = useCallback((id) => {
  // ...
}, []);

const config = useMemo(() => ({ theme: 'dark' }), []);

return items.map(item => (
  <Child 
    key={item.id}
    onClick={() => handleClick(item.id)}
    config={config}
  />
));

// CAUSE E : Computation synchrone bloquante
function SlowComponent() {
  // ❌ Block main thread 200ms
  const result = heavySyncComputation(data);
  
  // ✅ Solution : Web Worker
  const [result, setResult] = useState(null);
  
  useEffect(() => {
    const worker = new Worker('./worker.js');
    worker.postMessage(data);
    worker.onmessage = (e) => setResult(e.data);
    
    return () => worker.terminate();
  }, [data]);
}

// ÉTAPE 3 : VALIDER l'amélioration
// Re-profile après fix
// Target : < 16ms (60fps) ou < 33ms (30fps acceptable)
```

---

## 💡 Best Practices - Résumé Performance

### Checklist Complète

```javascript
// ✅ MESURE
const performanceChecklist = {
  // 1. Profile AVANT d'optimiser
  profiled: true,
  
  // 2. Identify bottlenecks
  identifiedSlowComponents: true,
  
  // 3. Set performance budgets
  budgets: {
    initialLoad: '< 3s',
    lcp: '< 2.5s',
    fid: '< 100ms',
    cls: '< 0.1',
    bundleSize: '< 500KB',
  },
};

// ✅ OPTIMISATIONS ESSENTIELLES
const essentialOpts = {
  // Code splitting
  lazyRoutes: true,
  lazyHeavyComponents: true,
  
  // Memoization
  memoHeavyComponents: true,
  useMemoForExpensiveCalcs: true,
  useCallbackForMemoChildren: true,
  
  // Lists
  virtualizeLargeLists: true, // > 100 items
  
  // Images
  lazyLoadImages: true,
  optimizedImageFormats: true, // WebP
  responsiveImages: true,
  
  // Bundle
  treeshaking: true,
  minification: true,
  compression: true, // gzip/brotli
};

// ✅ PATTERNS À ÉVITER
const antiPatterns = {
  // ❌ Inline objects/functions dans render
  inlineObjects: false,
  
  // ❌ Calculs lourds sans useMemo
  unm

emoizedHeavyCalcs: false,
  
  // ❌ Grandes listes non-virtualisées
  nonVirtualizedLists: false,
  
  // ❌ Images non-optimisées
  unoptimizedImages: false,
  
  // ❌ Pas de code splitting
  monolithicBundle: false,
};
```

---

## 🔗 Liens avec Vos Projets

### Wild Oasis

```javascript
// Optimisations appliquées :
const wildOasisOpts = {
  // 1. React Query avec staleTime adapté
  cabinsQuery: {
    staleTime: 5 * 60 * 1000, // Cabins changent peu
  },
  
  // 2. Memoization des composants lourds
  memoizedComponents: ['CabinRow', 'BookingRow', 'DashboardBox'],
  
  // 3. useMemo pour stats calculations
  statsCalcs: useMemo(() => {
    return calculateStats(bookings);
  }, [bookings]),
  
  // 4. Image optimization
  supabaseImageTransforms: true, // Auto resize
  
  // 5. Code splitting par feature
  lazyRoutes: ['dashboard', 'bookings', 'cabins', 'settings'],
};

// Points d'amélioration possibles :
// - Virtualization pour liste bookings (si > 100)
// - Prefetching au hover sur cabin cards
// - Web Worker pour heavy stats calculations
```

### Fast React Pizza

```javascript
// Optimisations :
const pizzaOpts = {
  // 1. React Query pour menu (long staleTime)
  menuQuery: {
    staleTime: 30 * 60 * 1000, // Menu très stable
  },
  
  // 2. Optimistic updates pour cart
  optimisticCart: true,
  
  // 3. Debounce sur address input
  debouncedGeolocation: true,
  
  // 4. Code splitting
  lazyOrderTracking: true,
};
```

---

**🎯 Points Clés**

1. **Toujours profiler AVANT d'optimiser** (no premature optimization)
2. **React.memo** pour composants lourds avec props stables
3. **useMemo** pour calculs coûteux (> 5ms)
4. **useCallback** pour fonctions passées à memo children
5. **Virtualization** pour listes > 100 items
6. **Code splitting** avec lazy() pour routes/features
7. **Mesurer impact** avec Core Web Vitals

---