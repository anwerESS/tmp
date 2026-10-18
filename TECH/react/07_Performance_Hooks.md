## [**..**](./00_index.md)

# ⚡ CHAPITRE 7 : Hooks de Performance

## 🎯 INTRODUCTION : POURQUOI L'OPTIMISATION ?

### Le Problème de Performance dans React

**React re-render un component quand :**
1. Son **state** change
2. Ses **props** changent
3. Son **parent** re-render

```javascript
function Parent() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(count + 1)}>Count: {count}</button>
      <ExpensiveChild />  {/* ❌ Re-render même si pas de props ! */}
    </div>
  );
}

function ExpensiveChild() {
  console.log('ExpensiveChild rendered');
  // Calcul lourd...
  return <div>Expensive Component</div>;
}
```

**Problème :** ExpensiveChild re-render à chaque click, même si ses props n'ont pas changé !

---

### Les 3 Techniques d'Optimisation

| Technique | Usage | Quand utiliser |
|-----------|-------|----------------|
| **React.memo** | Éviter re-renders inutiles de components | Component pur avec props stables |
| **useMemo** | Mémoïser des **valeurs** calculées | Calculs coûteux |
| **useCallback** | Mémoïser des **fonctions** | Fonctions passées comme props |

---

### ⚠️ RÈGLE D'OR : N'optimisez PAS prématurément !

```javascript
// ❌ MAUVAIS : Optimiser tout par défaut
function Component() {
  const value = useMemo(() => props.x + props.y, [props.x, props.y]);
  const handler = useCallback(() => console.log('click'), []);
  // Overhead inutile si pas de problème de perf !
}

// ✅ BON : Optimiser seulement si nécessaire
function Component() {
  const value = props.x + props.y; // Simple
  const handler = () => console.log('click');
  // Pas de problème → Pas d'optimisation
}
```

**Optimisez quand :**
- ✅ Profiling montre un problème
- ✅ Component re-render trop souvent
- ✅ Calculs vraiment coûteux (> 10ms)
- ✅ Listes avec beaucoup d'items

---

## 🧊 React.memo : ÉVITER LES RE-RENDERS

### Concept

**React.memo = HOC (Higher-Order Component) qui mémoïse un component**

```javascript
const MemoizedComponent = React.memo(Component);
```

**Comment ça marche ?**
1. React compare les **props** (shallow comparison)
2. Si props identiques → Réutilise le render précédent
3. Si props différentes → Re-render

---

### Exemple de Base

```javascript
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

function Parent() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(count + 1)}>Count: {count}</button>
      <Child name="John" />
    </div>
  );
}
// Console : Child rendered (une seule fois !)
```

---

### Quand React.memo EST Utile

```javascript
// ✅ Component avec calculs lourds
const HeavyComponent = React.memo(function HeavyComponent({ data }) {
  const processed = expensiveCalculation(data); // Calcul lourd
  
  return <div>{processed}</div>;
});

// ✅ Listes avec beaucoup d'items
const ListItem = React.memo(function ListItem({ item }) {
  return (
    <div className="item">
      <img src={item.image} />
      <h3>{item.title}</h3>
      <p>{item.description}</p>
    </div>
  );
});

function List({ items }) {
  return (
    <ul>
      {items.map(item => (
        <ListItem key={item.id} item={item} />
      ))}
    </ul>
  );
}

// ✅ Component pur (même props → même output)
const Header = React.memo(function Header({ title, subtitle }) {
  return (
    <header>
      <h1>{title}</h1>
      <h2>{subtitle}</h2>
    </header>
  );
});
```

---

### Quand React.memo N'EST PAS Utile

```javascript
// ❌ Component simple et rapide
const Button = React.memo(function Button({ label }) {
  return <button>{label}</button>; // Overhead > Gain
});

// ❌ Props changent toujours
function Parent() {
  const [count, setCount] = useState(0);
  
  return (
    <Child count={count} /> // count change toujours → memo inutile
  );
}

// ❌ Component avec children
const Container = React.memo(function Container({ children }) {
  return <div className="container">{children}</div>;
});
// children = nouveau object à chaque render → memo inutile
```

---

### Le Problème des Props Non-Primitives

```javascript
// ❌ PROBLÈME : object/array recréé à chaque render
function Parent() {
  const [count, setCount] = useState(0);
  
  const user = { name: 'John', age: 30 }; // ❌ Nouveau object !
  const hobbies = ['reading', 'coding']; // ❌ Nouveau array !
  
  return (
    <div>
      <button onClick={() => setCount(count + 1)}>Count: {count}</button>
      <Child user={user} hobbies={hobbies} />
    </div>
  );
}

const Child = React.memo(function Child({ user, hobbies }) {
  console.log('Child rendered');
  return <div>{user.name}</div>;
});
// Console : Child rendered (à chaque click)
// Pourquoi ? user et hobbies sont de nouveaux objets à chaque fois
// {} !== {} et [] !== []
```

**Solution : useMemo ou useCallback (voir plus bas)**

---

### Custom Comparison Function

Par défaut, React.memo fait une **shallow comparison**. On peut customiser :

```javascript
// Comparaison custom
const Child = React.memo(
  function Child({ user }) {
    return <div>{user.name}</div>;
  },
  (prevProps, nextProps) => {
    // Retourne true si props sont ÉGALES (ne pas re-render)
    // Retourne false si props sont DIFFÉRENTES (re-render)
    return prevProps.user.id === nextProps.user.id;
  }
);

// Exemple : Ne re-render que si user.id change
function Parent() {
  const [user, setUser] = useState({ id: 1, name: 'John', count: 0 });
  
  const incrementCount = () => {
    setUser({ ...user, count: user.count + 1 });
  };
  
  return (
    <div>
      <button onClick={incrementCount}>Increment Count</button>
      <Child user={user} />
    </div>
  );
}
// Child ne re-render PAS quand count change (seulement si id change)
```

---

## 💾 useMemo : MÉMOÏSER DES VALEURS

### Concept

**useMemo = Hook qui mémoïse le résultat d'un calcul**

```javascript
const memoizedValue = useMemo(
  () => expensiveCalculation(a, b),
  [a, b] // Dependencies
);
```

**Comment ça marche ?**
1. Calcule la valeur lors du premier render
2. Si dependencies n'ont pas changé → Retourne valeur en cache
3. Si dependencies ont changé → Recalcule

---

### Exemple de Base

```javascript
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

---

### Cas d'Usage 1 : Calculs Coûteux

```javascript
function ProductList({ products, filter }) {
  // ✅ Filtrage coûteux mémoïsé
  const filteredProducts = useMemo(() => {
    console.log('Filtering products...');
    return products.filter(product => {
      return product.category === filter &&
             product.name.toLowerCase().includes(searchTerm);
    });
  }, [products, filter, searchTerm]);
  
  return (
    <ul>
      {filteredProducts.map(p => (
        <li key={p.id}>{p.name}</li>
      ))}
    </ul>
  );
}
```

---

### Cas d'Usage 2 : Éviter Re-renders avec React.memo

```javascript
function Parent() {
  const [count, setCount] = useState(0);
  
  // ❌ PROBLÈME : options recréé à chaque render
  const options = { sort: 'asc', limit: 10 }; // Nouveau object
  
  return (
    <div>
      <button onClick={() => setCount(count + 1)}>Count: {count}</button>
      <Child options={options} />
    </div>
  );
}

const Child = React.memo(function Child({ options }) {
  console.log('Child rendered');
  return <div>Options: {JSON.stringify(options)}</div>;
});
// Console : Child rendered (à chaque click)

// ✅ SOLUTION : useMemo
function Parent() {
  const [count, setCount] = useState(0);
  
  const options = useMemo(() => ({ sort: 'asc', limit: 10 }), []);
  // Même objet retourné à chaque render
  
  return (
    <div>
      <button onClick={() => setCount(count + 1)}>Count: {count}</button>
      <Child options={options} />
    </div>
  );
}
// Console : Child rendered (une seule fois !)
```

---

### Cas d'Usage 3 : Objets/Arrays dans Dependencies

```javascript
function SearchResults({ query }) {
  const [results, setResults] = useState([]);
  
  // ❌ PROBLÈME : config recréé → useEffect s'exécute toujours
  useEffect(() => {
    const config = { method: 'GET', headers: { 'Content-Type': 'application/json' } };
    
    fetch(`/api/search?q=${query}`, config)
      .then(r => r.json())
      .then(setResults);
  }, [query, config]); // ❌ config change à chaque render !
  
  // ✅ SOLUTION 1 : Déplacer dans useEffect
  useEffect(() => {
    const config = { method: 'GET', headers: { 'Content-Type': 'application/json' } };
    
    fetch(`/api/search?q=${query}`, config)
      .then(r => r.json())
      .then(setResults);
  }, [query]); // Pas de config dans les deps
  
  // ✅ SOLUTION 2 : useMemo
  const config = useMemo(
    () => ({ method: 'GET', headers: { 'Content-Type': 'application/json' } }),
    []
  );
  
  useEffect(() => {
    fetch(`/api/search?q=${query}`, config)
      .then(r => r.json())
      .then(setResults);
  }, [query, config]); // config stable maintenant
}
```

---

### Cas d'Usage 4 : Sort/Filter de Listes

```javascript
function DataTable({ data, sortBy, filterBy }) {
  // ✅ Sort et filter mémoïsés
  const processedData = useMemo(() => {
    console.log('Processing data...');
    
    let result = [...data];
    
    // Filter
    if (filterBy) {
      result = result.filter(item => item.category === filterBy);
    }
    
    // Sort
    if (sortBy) {
      result.sort((a, b) => {
        if (a[sortBy] < b[sortBy]) return -1;
        if (a[sortBy] > b[sortBy]) return 1;
        return 0;
      });
    }
    
    return result;
  }, [data, sortBy, filterBy]);
  
  return (
    <table>
      <tbody>
        {processedData.map(item => (
          <tr key={item.id}>
            <td>{item.name}</td>
            <td>{item.value}</td>
          </tr>
        ))}
      </tbody>
    </table>
  );
}
```

---

### Cas d'Usage 5 : Context Value

```javascript
// ❌ PROBLÈME : Context value recréé → Tous les consumers re-render
function ThemeProvider({ children }) {
  const [theme, setTheme] = useState('dark');
  
  const value = { theme, setTheme }; // ❌ Nouveau object à chaque render
  
  return (
    <ThemeContext.Provider value={value}>
      {children}
    </ThemeContext.Provider>
  );
}
// Tous les components qui utilisent useContext(ThemeContext) re-render !

// ✅ SOLUTION : useMemo
function ThemeProvider({ children }) {
  const [theme, setTheme] = useState('dark');
  
  const value = useMemo(() => ({ theme, setTheme }), [theme]);
  
  return (
    <ThemeContext.Provider value={value}>
      {children}
    </ThemeContext.Provider>
  );
}
```

---

### ⚠️ Quand NE PAS Utiliser useMemo

```javascript
// ❌ Calcul simple et rapide
const doubled = useMemo(() => count * 2, [count]);
// Overhead de useMemo > Calcul simple

// ❌ Primitive values
const name = useMemo(() => 'John', []);
// Les strings sont déjà immuables !

// ❌ Déjà stable
const config = { api: 'https://api.example.com' };
// Si défini en dehors du component, déjà stable

// ✅ BON : Calcul coûteux
const sortedData = useMemo(() => {
  return [...data].sort((a, b) => a.value - b.value);
}, [data]);
```

---

## 🔄 useCallback : MÉMOÏSER DES FONCTIONS

### Concept

**useCallback = Hook qui mémoïse une fonction**

```javascript
const memoizedCallback = useCallback(
  () => {
    doSomething(a, b);
  },
  [a, b]
);
```

**Équivalent à :**

```javascript
const memoizedCallback = useMemo(
  () => {
    return () => doSomething(a, b);
  },
  [a, b]
);
```

---

### Exemple de Base

```javascript
// ❌ SANS useCallback : Fonction recréée à chaque render
function Parent() {
  const [count, setCount] = useState(0);
  
  const handleClick = () => {
    console.log('Clicked');
  };
  // Nouvelle fonction à chaque render !
  
  return (
    <div>
      <button onClick={() => setCount(count + 1)}>Count: {count}</button>
      <Child onClick={handleClick} />
    </div>
  );
}

const Child = React.memo(function Child({ onClick }) {
  console.log('Child rendered');
  return <button onClick={onClick}>Click me</button>;
});
// Console : Child rendered (à chaque click sur Count)

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
// Console : Child rendered (une seule fois !)
```

---

### Cas d'Usage 1 : Props de Components Mémoïsés

```javascript
function TodoList({ todos }) {
  const [filter, setFilter] = useState('all');
  
  // ✅ handleDelete stable
  const handleDelete = useCallback((id) => {
    deleteTodo(id);
  }, []);
  
  const filteredTodos = useMemo(() => {
    return todos.filter(todo => {
      if (filter === 'all') return true;
      if (filter === 'active') return !todo.completed;
      if (filter === 'completed') return todo.completed;
    });
  }, [todos, filter]);
  
  return (
    <ul>
      {filteredTodos.map(todo => (
        <TodoItem
          key={todo.id}
          todo={todo}
          onDelete={handleDelete} // Fonction stable
        />
      ))}
    </ul>
  );
}

const TodoItem = React.memo(function TodoItem({ todo, onDelete }) {
  return (
    <li>
      {todo.title}
      <button onClick={() => onDelete(todo.id)}>Delete</button>
    </li>
  );
});
```

---

### Cas d'Usage 2 : Dependencies de useEffect

```javascript
// ❌ PROBLÈME : useEffect s'exécute à chaque render
function SearchComponent({ query }) {
  const [results, setResults] = useState([]);
  
  const fetchResults = () => {
    fetch(`/api/search?q=${query}`)
      .then(r => r.json())
      .then(setResults);
  };
  // Nouvelle fonction à chaque render
  
  useEffect(() => {
    fetchResults();
  }, [fetchResults]); // ❌ fetchResults change toujours !
  
  return <div>{results.length} results</div>;
}

// ✅ SOLUTION : useCallback
function SearchComponent({ query }) {
  const [results, setResults] = useState([]);
  
  const fetchResults = useCallback(() => {
    fetch(`/api/search?q=${query}`)
      .then(r => r.json())
      .then(setResults);
  }, [query]); // Recréée seulement si query change
  
  useEffect(() => {
    fetchResults();
  }, [fetchResults]);
  
  return <div>{results.length} results</div>;
}
```

---

### Cas d'Usage 3 : Event Handlers avec State

```javascript
// ❌ PROBLÈME : Stale closure
function Counter() {
  const [count, setCount] = useState(0);
  
  const increment = useCallback(() => {
    setCount(count + 1); // ❌ count est capturé (stale)
  }, []); // Pas de deps → count toujours 0
  
  return (
    <div>
      <p>Count: {count}</p>
      <button onClick={increment}>+</button>
    </div>
  );
}
// Click → count = 1
// Click → count = 1 (pas 2 !)

// ✅ SOLUTION 1 : Inclure count dans deps
const increment = useCallback(() => {
  setCount(count + 1);
}, [count]); // Recréée quand count change

// ✅ SOLUTION 2 : Functional update (meilleure)
const increment = useCallback(() => {
  setCount(c => c + 1); // Pas besoin de count dans deps
}, []); // Fonction stable
```

---

### Cas d'Usage 4 : Custom Hooks

```javascript
// ✅ Custom hook avec fonctions stables
function useApi(url) {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  
  const fetchData = useCallback(async () => {
    try {
      setLoading(true);
      setError(null);
      const res = await fetch(url);
      const json = await res.json();
      setData(json);
    } catch (err) {
      setError(err.message);
    } finally {
      setLoading(false);
    }
  }, [url]);
  
  const refetch = useCallback(() => {
    fetchData();
  }, [fetchData]);
  
  useEffect(() => {
    fetchData();
  }, [fetchData]);
  
  return { data, loading, error, refetch };
}

// Usage
function Component() {
  const { data, refetch } = useApi('/api/users');
  
  return (
    <div>
      <button onClick={refetch}>Refresh</button>
      {data && <ul>{data.map(u => <li key={u.id}>{u.name}</li>)}</ul>}
    </div>
  );
}
```

---

### ⚠️ Quand NE PAS Utiliser useCallback

```javascript
// ❌ Fonction utilisée seulement dans le component
function Component() {
  const handleClick = useCallback(() => {
    console.log('clicked');
  }, []);
  
  return <button onClick={handleClick}>Click</button>;
  // Si pas passé à un child mémoïsé, useCallback inutile
}

// ✅ Simplifier
function Component() {
  const handleClick = () => console.log('clicked');
  
  return <button onClick={handleClick}>Click</button>;
}

// ❌ Fonction inline simple
<button onClick={useCallback(() => setCount(c => c + 1), [])}>+</button>
// Trop verbeux pour un cas simple

// ✅ BON : Inline direct
<button onClick={() => setCount(c => c + 1)}>+</button>
```

---

## 📊 COMPARATIF : memo vs useMemo vs useCallback

| | React.memo | useMemo | useCallback |
|---|---|---|---|
| **Type** | HOC | Hook | Hook |
| **Mémoïse** | Component entier | Valeur calculée | Fonction |
| **Retourne** | Component mémoïsé | Valeur | Fonction |
| **Usage** | `React.memo(Component)` | `useMemo(() => value, [deps])` | `useCallback(() => {}, [deps])` |
| **Quand** | Éviter re-renders | Calculs coûteux | Fonctions comme props |

---

### Exemple Combiné

```javascript
function ProductList({ products, category }) {
  // ✅ useMemo pour filtrer
  const filteredProducts = useMemo(() => {
    console.log('Filtering...');
    return products.filter(p => p.category === category);
  }, [products, category]);
  
  // ✅ useCallback pour event handler
  const handleAddToCart = useCallback((productId) => {
    addToCart(productId);
  }, []);
  
  return (
    <div>
      {filteredProducts.map(product => (
        <ProductCard
          key={product.id}
          product={product}
          onAddToCart={handleAddToCart}
        />
      ))}
    </div>
  );
}

// ✅ React.memo pour éviter re-renders
const ProductCard = React.memo(function ProductCard({ product, onAddToCart }) {
  console.log(`Rendering product ${product.id}`);
  
  return (
    <div className="card">
      <h3>{product.name}</h3>
      <p>${product.price}</p>
      <button onClick={() => onAddToCart(product.id)}>
        Add to Cart
      </button>
    </div>
  );
});
```

---

## 🎯 PROFILING & DEBUGGING

### React DevTools Profiler

**Comment utiliser :**
1. Installer React DevTools (Chrome/Firefox)
2. Ouvrir DevTools → Onglet "Profiler"
3. Click "Record" → Interagir avec l'app → Click "Stop"
4. Analyser les flamegraphs

**Ce que le Profiler montre :**
- ✅ Quels components ont re-render
- ✅ Combien de temps chaque render a pris
- ✅ Pourquoi un component a re-render
- ✅ Wasted renders (re-renders inutiles)

---

### Détecter les Wasted Renders

```javascript
// Ajouter des logs
function Component({ value }) {
  console.log('Component rendered', { value });
  
  return <div>{value}</div>;
}

// Ou utiliser useWhyDidYouUpdate
function useWhyDidYouUpdate(name, props) {
  const previousProps = useRef();
  
  useEffect(() => {
    if (previousProps.current) {
      const allKeys = Object.keys({ ...previousProps.current, ...props });
      const changedProps = {};
      
      allKeys.forEach(key => {
        if (previousProps.current[key] !== props[key]) {
          changedProps[key] = {
            from: previousProps.current[key],
            to: props[key],
          };
        }
      });
      
      if (Object.keys(changedProps).length) {
        console.log('[why-did-you-update]', name, changedProps);
      }
    }
    
    previousProps.current = props;
  });
}

// Usage
function Component(props) {
  useWhyDidYouUpdate('Component', props);
  return <div>{props.value}</div>;
}
```

---

## ⚠️ PIÈGES COURANTS

### Piège 1 : Dépendances Manquantes

```javascript
// ❌ MAUVAIS
function Component({ userId }) {
  const fetchUser = useCallback(() => {
    fetch(`/api/users/${userId}`); // Utilise userId
  }, []); // ❌ Manque userId !
  
  // fetchUser utilise toujours le userId initial
}

// ✅ BON
const fetchUser = useCallback(() => {
  fetch(`/api/users/${userId}`);
}, [userId]); // ✅ Inclure toutes les deps
```

---

### Piège 2 : Optimisation Prématurée

```javascript
// ❌ Sur-optimisation
function SimpleButton({ label }) {
  const handleClick = useCallback(() => {
    console.log('clicked');
  }, []);
  
  const buttonStyle = useMemo(() => ({
    padding: '10px',
    background: 'blue',
  }), []);
  
  return <button style={buttonStyle} onClick={handleClick}>{label}</button>;
}
// Overhead > Gain pour un component simple

// ✅ Simple et clair
function SimpleButton({ label }) {
  const handleClick = () => console.log('clicked');
  
  return (
    <button
      style={{ padding: '10px', background: 'blue' }}
      onClick={handleClick}
    >
      {label}
    </button>
  );
}
```

---

### Piège 3 : useMemo/useCallback Partout

```javascript
// ❌ MAUVAIS : Tout optimiser
function Component({ a, b, c }) {
  const value1 = useMemo(() => a + b, [a, b]);
  const value2 = useMemo(() => a * c, [a, c]);
  const handler1 = useCallback(() => console.log(value1), [value1]);
  const handler2 = useCallback(() => console.log(value2), [value2]);
  
  // Code illisible et overhead inutile
}

// ✅ BON : Optimiser seulement si nécessaire
function Component({ a, b, c }) {
  const value1 = a + b;
  const value2 = a * c;
  const handler1 = () => console.log(value1);
  const handler2 = () => console.log(value2);
  
  // Simple et performant pour la plupart des cas
}
```

---

### Piège 4 : Oublier React.memo avec useCallback

```javascript
// ❌ useCallback sans React.memo = inutile
function Parent() {
  const handleClick = useCallback(() => {
    console.log('clicked');
  }, []);
  
  return <Child onClick={handleClick} />;
}

function Child({ onClick }) {
  // Pas de React.memo → Re-render quand même
  return <button onClick={onClick}>Click</button>;
}

// ✅ useCallback + React.memo
const Child = React.memo(function Child({ onClick }) {
  return <button onClick={onClick}>Click</button>;
});
```

---

## 🎯 QUESTIONS D'ENTRETIEN FRÉQUENTES

### Q1 : Quelle est la différence entre useMemo et useCallback ?

**Réponse :**

**useMemo** mémoïse une **valeur** (résultat d'un calcul)
**useCallback** mémoïse une **fonction**

```javascript
// useMemo
const value = useMemo(() => expensiveCalculation(a, b), [a, b]);
// Équivaut à stocker le résultat de expensiveCalculation

// useCallback
const callback = useCallback(() => doSomething(a, b), [a, b]);
// Équivaut à useMemo(() => () => doSomething(a, b), [a, b])

// En pratique :
const doubled = useMemo(() => count * 2, [count]); // Valeur
const handleClick = useCallback(() => setCount(c => c + 1), []); // Fonction
```

---

### Q2 : Quand utiliser React.memo ?

**Réponse :**

**✅ Utilisez React.memo quand :**

1. **Component pur** : Même props → Même output
2. **Re-renders fréquents** : Parent re-render souvent
3. **Calculs coûteux** : Component fait des opérations lourdes
4. **Props stables** : Props ne changent pas souvent

```javascript
// ✅ BON cas d'usage
const ExpensiveList = React.memo(function ExpensiveList({ items }) {
  // Calcul lourd sur chaque item
  const processedItems = items.map(item => {
    return heavyProcessing(item); // 10ms par item
  });
  
  return (
    <ul>
      {processedItems.map(item => <li key={item.id}>{item.name}</li>)}
    </ul>
  );
});
```

**❌ N'utilisez PAS React.memo quand :**

1. **Component simple** : Render rapide (< 1ms)
2. **Props changent toujours** : memo inutile
3. **Pas de problème de perf** : Optimisation prématurée

```javascript
// ❌ MAUVAIS cas d'usage
const SimpleButton = React.memo(function SimpleButton({ label }) {
  return <button>{label}</button>; // Trop simple
});
```

---

### Q3 : React.memo fait-elle une deep comparison ?

**Réponse :**

**Non, React.memo fait une shallow comparison par défaut.**

```javascript
// Shallow comparison
const obj1 = { name: 'John', age: 30 };
const obj2 = { name: 'John', age: 30 };

obj1 === obj2; // false (références différentes)

// React.memo compare :
prevProps.user === nextProps.user; // Comparaison de référence

// Si user est un nouvel objet → Re-render même si contenu identique
```

**Solutions :**

```javascript
// Solution 1 : Custom comparison
const Component = React.memo(
  function Component({ user }) {
    return <div>{user.name}</div>;
  },
  (prevProps, nextProps) => {
    // Retourne true si props ÉGALES (pas de re-render)
    return prevProps.user.id === nextProps.user.id;
  }
);

// Solution 2 : useMemo dans le parent
function Parent() {
  const user = useMemo(() => ({ name: 'John', age: 30 }), []);
  return <Component user={user} />;
}

// Solution 3 : Passer des primitives
function Parent() {
  const user = { name: 'John', age: 30 };
  return <Component userName={user.name} userAge={user.age} />;
}
```

---

### Q4 : Pourquoi useCallback avec un array vide ne fonctionne pas toujours ?

**Réponse :**

**Problème : Stale closure**

```javascript
function Counter() {
  const [count, setCount] = useState(0);
  
  // ❌ PROBLÈME : count est capturé au premier render
  const logCount = useCallback(() => {
    console.log(count); // count = 0 toujours
  }, []); // Array vide
  
  return (
    <div>
      <p>Count: {count}</p>
      <button onClick={() => setCount(count + 1)}>+</button>
      <button onClick={logCount}>Log Count</button>
    </div>
  );
}
// Click + plusieurs fois → count = 3
// Click "Log Count" → console : 0 (pas 3 !)
```

**Solutions :**

```javascript
// Solution 1 : Inclure count dans deps
const logCount = useCallback(() => {
  console.log(count);
}, [count]); // Recréée quand count change

// Solution 2 : useRef
const countRef = useRef(count);

useEffect(() => {
  countRef.current = count;
}, [count]);

const logCount = useCallback(() => {
  console.log(countRef.current); // Toujours à jour
}, []);

// Solution 3 : Functional update (si setState)
const increment = useCallback(() => {
  setCount(c => c + 1); // c = valeur actuelle
}, []); // Pas besoin de count dans deps
```

---

### Q5 : useMemo peut-elle remplacer useState ?

**Réponse :**

**Non ! Ils ont des usages différents.**

```javascript
// ❌ MAUVAIS : Utiliser useMemo comme state
function Component() {
  const [input, setInput] = useState('');
  
  // ❌ doubled n'est PAS un state
  const doubled = useMemo(() => input.length * 2, [input]);
  
  // Si on veut modifier doubled ?
  // Impossible ! useMemo ne retourne que la valeur
}

// ✅ BON : useState pour state, useMemo pour valeurs dérivées
function Component() {
  const [input, setInput] = useState('');
  
  // ✅ doubled est dérivé de input
  const doubled = useMemo(() => input.length * 2, [input]);
  
  // Pour modifier : on modifie input, pas doubled
  return (
    <div>
      <input value={input} onChange={(e) => setInput(e.target.value)} />
      <p>Length: {input.length}</p>
      <p>Doubled: {doubled}</p>
    </div>
  );
}
```

**Différences :**

| | useState | useMemo |
|---|---|---|
| **Crée un state** | ✅ Oui | ❌ Non |
| **Trigger re-render** | ✅ Oui | ❌ Non |
| **Valeur modifiable** | ✅ Oui (avec setter) | ❌ Non (read-only) |
| **Usage** | Data que l'utilisateur change | Valeurs calculées/dérivées |

---

### Q6 : Comment optimiser une longue liste ?

**Réponse complète :**

```javascript
// Problème : 1000 items qui re-render tous
function TodoList({ todos }) {
  const [filter, setFilter] = useState('all');
  
  return (
    <div>
      <FilterButtons filter={filter} onFilterChange={setFilter} />
      <ul>
        {todos.map(todo => (
          <TodoItem key={todo.id} todo={todo} />
        ))}
      </ul>
    </div>
  );
}
// Chaque changement de filter → 1000 TodoItem re-render !
```

**Solution : Combinaison de techniques**

```javascript
function TodoList({ todos }) {
  const [filter, setFilter] = useState('all');
  
  // 1. ✅ useMemo pour filtrer
  const filteredTodos = useMemo(() => {
    if (filter === 'all') return todos;
    if (filter === 'active') return todos.filter(t => !t.completed);
    if (filter === 'completed') return todos.filter(t => t.completed);
  }, [todos, filter]);
  
  // 2. ✅ useCallback pour handlers
  const handleToggle = useCallback((id) => {
    toggleTodo(id);
  }, []);
  
  const handleDelete = useCallback((id) => {
    deleteTodo(id);
  }, []);
  
  return (
    <div>
      <FilterButtons filter={filter} onFilterChange={setFilter} />
      <ul>
        {filteredTodos.map(todo => (
          <TodoItem
            key={todo.id}
            todo={todo}
            onToggle={handleToggle}
            onDelete={handleDelete}
          />
        ))}
      </ul>
    </div>
  );
}

// 3. ✅ React.memo sur l'item
const TodoItem = React.memo(function TodoItem({ todo, onToggle, onDelete }) {
  return (
    <li>
      <input
        type="checkbox"
        checked={todo.completed}
        onChange={() => onToggle(todo.id)}
      />
      <span>{todo.title}</span>
      <button onClick={() => onDelete(todo.id)}>Delete</button>
    </li>
  );
});

// 4. ✅ BONUS : Virtualization pour très longues listes (10k+ items)
import { FixedSizeList } from 'react-window';

function VirtualizedTodoList({ todos }) {
  const Row = ({ index, style }) => (
    <div style={style}>
      <TodoItem todo={todos[index]} />
    </div>
  );
  
  return (
    <FixedSizeList
      height={600}
      itemCount={todos.length}
      itemSize={50}
      width="100%"
    >
      {Row}
    </FixedSizeList>
  );
}
// Render seulement les items visibles !
```

---

### Q7 : Quelle est la différence entre lazy initialization et useMemo ?

**Réponse :**

**Lazy initialization (useState) :**

```javascript
// ❌ Calcul s'exécute à CHAQUE render
const [state, setState] = useState(expensiveCalculation());

// ✅ Calcul s'exécute seulement au MOUNT
const [state, setState] = useState(() => expensiveCalculation());
```

**useMemo :**

```javascript
// ✅ Calcul se ré-exécute si deps changent
const value = useMemo(() => expensiveCalculation(dep), [dep]);
```

**Différences :**

| | Lazy initialization | useMemo |
|---|---|---|
| **Quand** | Seulement au mount | Quand deps changent |
| **Usage** | Initial state coûteux | Valeur dérivée coûteuse |
| **Recalcule ?** | ❌ Non | ✅ Oui (si deps changent) |

**Exemple :**

```javascript
function Component({ userId }) {
  // ✅ Lazy init : Une fois
  const [prefs, setPrefs] = useState(() => {
    return JSON.parse(localStorage.getItem('prefs')) || {};
  });
  
  // ✅ useMemo : Recalcule si userId change
  const userData = useMemo(() => {
    return fetchUserData(userId);
  }, [userId]);
  
  return <div>...</div>;
}
```

---

### Q8 : Comment éviter re-renders avec Context ?

**Réponse :**

**Problème :** Context re-render tous les consumers même si ils n'utilisent qu'une partie.

```javascript
// ❌ PROBLÈME
const AppContext = createContext();

function AppProvider({ children }) {
  const [user, setUser] = useState(null);
  const [theme, setTheme] = useState('dark');
  
  const value = { user, setUser, theme, setTheme }; // Nouveau object à chaque render
  
  return (
    <AppContext.Provider value={value}>
      {children}
    </AppContext.Provider>
  );
}

function UserDisplay() {
  const { user } = useContext(AppContext);
  // Re-render même si seulement theme change !
  return <div>{user?.name}</div>;
}
```

**Solutions :**

```javascript
// Solution 1 : useMemo sur value
function AppProvider({ children }) {
  const [user, setUser] = useState(null);
  const [theme, setTheme] = useState('dark');
  
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

// Solution 2 : Split contexts
const UserContext = createContext();
const ThemeContext = createContext();

function AppProvider({ children }) {
  const [user, setUser] = useState(null);
  const [theme, setTheme] = useState('dark');
  
  return (
    <UserContext.Provider value={{ user, setUser }}>
      <ThemeContext.Provider value={{ theme, setTheme }}>
        {children}
      </ThemeContext.Provider>
    </UserContext.Provider>
  );
}

// Maintenant les components ne re-render que si leur context change
function UserDisplay() {
  const { user } = useContext(UserContext); // Re-render seulement si user change
  return <div>{user?.name}</div>;
}

// Solution 3 : use-context-selector (library)
import { createContext, useContextSelector } from 'use-context-selector';

const AppContext = createContext();

function UserDisplay() {
  const user = useContextSelector(AppContext, v => v.user);
  // Re-render seulement si user change, pas theme !
  return <div>{user?.name}</div>;
}
```

---

### Q9 : Faut-il toujours ajouter toutes les dépendances ?

**Réponse :**

**✅ OUI, toujours ajouter toutes les dépendances !**

ESLint (eslint-plugin-react-hooks) vous avertit si vous oubliez une dépendance.

```javascript
// ❌ MAUVAIS : ESLint warning
function Component({ userId }) {
  const fetchUser = useCallback(() => {
    fetch(`/api/users/${userId}`); // Utilise userId
  }, []); // ❌ Manque userId
}

// ✅ BON
const fetchUser = useCallback(() => {
  fetch(`/api/users/${userId}`);
}, [userId]); // ✅ Inclut userId
```

**Cas spéciaux où on peut ignorer :**

```javascript
// 1. Setters de useState (stables)
function Component() {
  const [count, setCount] = useState(0);
  
  useEffect(() => {
    setCount(c => c + 1); // setCount stable
  }, []); // Pas besoin de setCount dans deps
}

// 2. Dispatch de useReducer (stable)
function Component() {
  const [state, dispatch] = useReducer(reducer, initialState);
  
  useEffect(() => {
    dispatch({ type: 'ACTION' }); // dispatch stable
  }, []); // Pas besoin de dispatch dans deps
}

// 3. Refs (stables)
function Component() {
  const ref = useRef();
  
  useEffect(() => {
    ref.current.focus(); // ref stable
  }, []); // Pas besoin de ref dans deps
}
```

**Si ESLint se plaint mais vous êtes sûr :**

```javascript
useEffect(() => {
  // Code...
  // eslint-disable-next-line react-hooks/exhaustive-deps
}, []); // Désactiver le warning (avec précaution !)
```

---

### Q10 : Comment mesurer la performance d'optimisation ?

**Réponse avec outils :**

**1. React DevTools Profiler**

```javascript
// Enregistrer et comparer avant/après optimisation
// 1. Ouvrir DevTools → Profiler
// 2. Click "Record"
// 3. Interagir avec l'app
// 4. Click "Stop"
// 5. Analyser les flamegraphs

// Chercher :
// - Wasted renders (components qui re-render sans changement)
// - Temps de render (> 16ms = problème)
// - Ranked chart (components les plus lents)
```

**2. console.time dans le code**

```javascript
function Component() {
  console.time('Component render');
  
  // Logique du component...
  
  console.timeEnd('Component render');
  
  return <div>...</div>;
}
// Console : Component render: 2.5ms
```

**3. Performance API**

```javascript
function Component() {
  const startTime = performance.now();
  
  // Calcul...
  const result = expensiveCalculation();
  
  const endTime = performance.now();
  console.log(`Calculation took ${endTime - startTime}ms`);
  
  return <div>{result}</div>;
}
```

**4. why-did-you-render (library)**

```bash
npm install @welldone-software/why-did-you-render
```

```javascript
// wdyr.js
import React from 'react';

if (process.env.NODE_ENV === 'development') {
  const whyDidYouRender = require('@welldone-software/why-did-you-render');
  whyDidYouRender(React, {
    trackAllPureComponents: true,
  });
}

// Component.js
Component.whyDidYouRender = true;

function Component(props) {
  return <div>{props.value}</div>;
}
// Console : Re-render car props.value changed from 1 to 1 (référence différente)
```

**5. Benchmark avant/après**

```javascript
// Avant optimisation
function slowFilter(items, query) {
  return items.filter(item => item.name.includes(query));
}

// Mesurer
const items = generateItems(10000);
console.time('Filter');
slowFilter(items, 'test');
console.timeEnd('Filter');
// Console : Filter: 45ms

// Après optimisation avec useMemo
const filteredItems = useMemo(() => {
  return items.filter(item => item.name.includes(query));
}, [items, query]);

// Mesurer
console.time('Filter with memo');
// Premier render: 45ms
// Re-renders suivants (si query/items pas changés): 0ms
console.timeEnd('Filter with memo');
```

---

## 🎨 PATTERNS AVANCÉS

### Pattern 1 : Composition Component

**Problème :** Parent re-render → Children re-render

```javascript
// ❌ PROBLÈME
function Page() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(count + 1)}>Count: {count}</button>
      <ExpensiveComponent /> {/* Re-render à chaque click ! */}
    </div>
  );
}
```

**Solution : Children prop**

```javascript
// ✅ SOLUTION
function Page() {
  return (
    <Layout>
      <ExpensiveComponent />
    </Layout>
  );
}

function Layout({ children }) {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <button onClick={() => setCount(count + 1)}>Count: {count}</button>
      {children} {/* Pas re-render ! children est stable */}
    </div>
  );
}
```

---

### Pattern 2 : State Colocation

**Déplacer le state au plus près de son usage**

```javascript
// ❌ State trop haut
function App() {
  const [modalOpen, setModalOpen] = useState(false);
  const [formData, setFormData] = useState({});
  
  return (
    <div>
      <Header /> {/* Re-render quand formData change */}
      <Sidebar /> {/* Re-render quand modalOpen change */}
      <Content />
      <Modal open={modalOpen} data={formData} />
    </div>
  );
}

// ✅ State proche de l'usage
function App() {
  return (
    <div>
      <Header />
      <Sidebar />
      <Content />
      <ModalSection /> {/* State interne */}
    </div>
  );
}

function ModalSection() {
  const [modalOpen, setModalOpen] = useState(false);
  const [formData, setFormData] = useState({});
  
  return <Modal open={modalOpen} data={formData} />;
}
```

---

### Pattern 3 : Lazy Loading Components

```javascript
import { lazy, Suspense } from 'react';

// ✅ Charger seulement quand nécessaire
const HeavyChart = lazy(() => import('./HeavyChart'));
const AdminPanel = lazy(() => import('./AdminPanel'));

function Dashboard({ isAdmin }) {
  return (
    <div>
      <Header />
      
      <Suspense fallback={<Spinner />}>
        <HeavyChart data={data} />
      </Suspense>
      
      {isAdmin && (
        <Suspense fallback={<Spinner />}>
          <AdminPanel />
        </Suspense>
      )}
    </div>
  );
}
```

---

### Pattern 4 : Virtualization

Pour listes très longues (10k+ items)

```javascript
import { FixedSizeList } from 'react-window';

function VirtualList({ items }) {
  const Row = ({ index, style }) => (
    <div style={style}>
      {items[index].name}
    </div>
  );
  
  return (
    <FixedSizeList
      height={600}
      itemCount={items.length}
      itemSize={35}
      width="100%"
    >
      {Row}
    </FixedSizeList>
  );
}
// Render seulement ~20 items visibles au lieu de 10000 !
```

---

### Pattern 5 : Debouncing Expensive Operations

```javascript
function SearchComponent() {
  const [query, setQuery] = useState('');
  const [results, setResults] = useState([]);
  
  // ✅ Debounce la recherche
  const debouncedQuery = useDebounce(query, 500);
  
  useEffect(() => {
    if (debouncedQuery) {
      fetch(`/api/search?q=${debouncedQuery}`)
        .then(r => r.json())
        .then(setResults);
    }
  }, [debouncedQuery]);
  
  return (
    <div>
      <input
        value={query}
        onChange={(e) => setQuery(e.target.value)}
      />
      <ResultsList results={results} />
    </div>
  );
}

// Custom hook useDebounce
function useDebounce(value, delay) {
  const [debouncedValue, setDebouncedValue] = useState(value);
  
  useEffect(() => {
    const handler = setTimeout(() => {
      setDebouncedValue(value);
    }, delay);
    
    return () => clearTimeout(handler);
  }, [value, delay]);
  
  return debouncedValue;
}
```

---

## 🔗 LIENS AVEC VOS PROJETS

### fast-react-pizza

**Optimisations applicables :**

```javascript
// Cart items list
const CartItem = React.memo(function CartItem({ item, onDelete, onUpdate }) {
  return (
    <li>
      <span>{item.name}</span>
      <span>${item.price}</span>
      <button onClick={() => onDelete(item.id)}>Delete</button>
    </li>
  );
});

// Callbacks stables
function Cart() {
  const items = useSelector(state => state.cart.items);
  
  const handleDelete = useCallback((id) => {
    dispatch(deleteItem(id));
  }, [dispatch]);
  
  const handleUpdate = useCallback((id, quantity) => {
    dispatch(updateItem({ id, quantity }));
  }, [dispatch]);
  
  return (
    <ul>
      {items.map(item => (
        <CartItem
          key={item.id}
          item={item}
          onDelete={handleDelete}
          onUpdate={handleUpdate}
        />
      ))}
    </ul>
  );
}
```

**Points à mentionner :**
- ✅ "J'aurais pu utiliser React.memo sur CartItem pour éviter re-renders"
- ✅ "useCallback pour les handlers dans les listes"
- ✅ "useMemo pour calculer le total du panier"

---

### wild-oasis

**Optimisations pour le dashboard :**

```javascript
// Calculs de stats mémoïsés
function Dashboard({ bookings, cabins }) {
  const stats = useMemo(() => {
    const totalRevenue = bookings.reduce((sum, b) => sum + b.totalPrice, 0);
    const occupancyRate = calculateOccupancy(bookings, cabins);
    const avgNights = bookings.reduce((sum, b) => sum + b.numNights, 0) / bookings.length;
    
    return { totalRevenue, occupancyRate, avgNights };
  }, [bookings, cabins]);
  
  return (
    <div>
      <StatCard title="Revenue" value={stats.totalRevenue} />
      <StatCard title="Occupancy" value={stats.occupancyRate} />
      <StatCard title="Avg Nights" value={stats.avgNights} />
    </div>
  );
}

const StatCard = React.memo(function StatCard({ title, value }) {
  return (
    <div className="stat-card">
      <h3>{title}</h3>
      <p>{value}</p>
    </div>
  );
});
```

---

### worldwise

**Optimisation de la map :**

```javascript
// Map component coûteux
const Map = React.memo(function Map({ cities, center, zoom }) {
  // Logique de la map (Leaflet)
  return <div id="map">...</div>;
});

// Parent
function WorldWise() {
  const [cities, setCities] = useState([]);
  const [selectedCity, setSelectedCity] = useState(null);
  
  // Map config stable
  const mapConfig = useMemo(() => ({
    center: [40, 0],
    zoom: 3,
  }), []);
  
  return (
    <div>
      <Sidebar cities={cities} onSelectCity={setSelectedCity} />
      <Map cities={cities} {...mapConfig} />
    </div>
  );
}
```

---

## 📚 RÉCAPITULATIF FINAL

### Checklist d'Optimisation

**Avant d'optimiser :**
- [ ] Profiler avec React DevTools
- [ ] Identifier les wasted renders
- [ ] Mesurer le temps de render (> 16ms = problème)

**Optimisations :**
- [ ] React.memo pour components purs
- [ ] useMemo pour calculs coûteux (> 10ms)
- [ ] useCallback pour fonctions passées aux memoized components
- [ ] State colocation (state au plus près de l'usage)
- [ ] Code splitting (lazy) pour gros components
- [ ] Virtualization pour longues listes (10k+ items)

**À éviter :**
- [ ] ❌ Optimiser prématurément
- [ ] ❌ useMemo/useCallback partout
- [ ] ❌ Oublier les dépendances
- [ ] ❌ React.memo sans props stables

---