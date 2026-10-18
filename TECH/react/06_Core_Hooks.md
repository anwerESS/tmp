## [**..**](./00_index.md)

# 🎣 CHAPITRE 6 : Hooks React Fondamentaux

## 🎯 INTRODUCTION : POURQUOI LES HOOKS ?

### Le Problème Avant les Hooks (Class Components)

**Avant React 16.8 (2019), les components avec state = Class Components obligatoires**

```javascript
// ❌ Class Component (ancien, verbeux)
class Counter extends React.Component {
  constructor(props) {
    super(props);
    this.state = { count: 0 };
    this.increment = this.increment.bind(this); // Binding manuel !
  }
  
  componentDidMount() {
    document.title = `Count: ${this.state.count}`;
  }
  
  componentDidUpdate() {
    document.title = `Count: ${this.state.count}`;
  }
  
  increment() {
    this.setState({ count: this.state.count + 1 });
  }
  
  render() {
    return (
      <div>
        <p>{this.state.count}</p>
        <button onClick={this.increment}>+</button>
      </div>
    );
  }
}
```

**Problèmes :**
- ❌ Beaucoup de boilerplate
- ❌ `this` difficile à comprendre
- ❌ Logique éparpillée (componentDidMount, componentDidUpdate)
- ❌ Difficile de réutiliser la logique
- ❌ Wrapper hell (HOC, render props)

---

### La Révolution des Hooks

```javascript
// ✅ Function Component avec Hooks (moderne, simple)
function Counter() {
  const [count, setCount] = useState(0);
  
  useEffect(() => {
    document.title = `Count: ${count}`;
  }, [count]);
  
  return (
    <div>
      <p>{count}</p>
      <button onClick={() => setCount(count + 1)}>+</button>
    </div>
  );
}
```

**Avantages :**
- ✅ Code plus court et lisible
- ✅ Pas de `this`
- ✅ Logique groupée par fonctionnalité
- ✅ Réutilisation facile (custom hooks)
- ✅ Moins de nesting

---

## ✅ RÈGLES DES HOOKS (CRITICAL !)

### Les 2 Règles Absolues

**1. Appeler les Hooks UNIQUEMENT au top level**

```javascript
// ❌ MAUVAIS : Hook dans condition
function Component({ show }) {
  if (show) {
    const [count, setCount] = useState(0); // ❌ Erreur !
  }
}

// ❌ MAUVAIS : Hook dans loop
function Component({ items }) {
  items.forEach(item => {
    const [value, setValue] = useState(item); // ❌ Erreur !
  });
}

// ✅ BON : Toujours au top level
function Component({ show }) {
  const [count, setCount] = useState(0);
  const [name, setName] = useState('');
  
  if (!show) return null;
  
  return <div>{count}</div>;
}
```

**Pourquoi ?** React se base sur l'**ordre d'appel** des Hooks pour maintenir le state.

---

**2. Appeler les Hooks UNIQUEMENT dans React Functions**

```javascript
// ❌ MAUVAIS : Hook dans fonction normale
function normalFunction() {
  const [count, setCount] = useState(0); // ❌ Erreur !
}

// ✅ BON : Hook dans Function Component
function MyComponent() {
  const [count, setCount] = useState(0); // ✅ OK
}

// ✅ BON : Hook dans Custom Hook
function useCounter() {
  const [count, setCount] = useState(0); // ✅ OK
  return [count, setCount];
}
```

---

### ESLint Plugin

Pour forcer ces règles automatiquement :

```bash
npm install eslint-plugin-react-hooks
```

```json
// .eslintrc
{
  "plugins": ["react-hooks"],
  "rules": {
    "react-hooks/rules-of-hooks": "error",
    "react-hooks/exhaustive-deps": "warn"
  }
}
```

---

## 🔄 useEffect : LE HOOK LE PLUS IMPORTANT

### Concept Fondamental

**useEffect = Synchroniser le component avec le monde extérieur (side effects)**

**Side effects :**
- API calls
- DOM manipulation
- Subscriptions (WebSocket, intervals)
- Browser APIs (localStorage, geolocation)
- Logging, analytics

```javascript
useEffect(() => {
  // Code à exécuter (effet)
  
  return () => {
    // Cleanup (optionnel)
  };
}, [dependencies]); // Dependency array
```

---

### Les 3 Comportements de useEffect

#### 1. Sans Dependency Array → Exécute à CHAQUE render

```javascript
function Component() {
  const [count, setCount] = useState(0);
  
  // ❌ DANGEREUX : Exécute à chaque render
  useEffect(() => {
    console.log('Je m\'exécute à chaque render');
  }); // Pas de []
  
  return (
    <button onClick={() => setCount(count + 1)}>
      Count: {count}
    </button>
  );
}
// Console : 
// Je m'exécute à chaque render
// Je m'exécute à chaque render (après le click)
// Je m'exécute à chaque render (après le 2ème click)
// ...
```

**⚠️ Presque jamais utilisé → Risque de boucles infinies !**

---

#### 2. Avec [] vide → Exécute UNE SEULE FOIS (au mount)

```javascript
function Component() {
  const [data, setData] = useState(null);
  
  // ✅ Exécute seulement au premier render
  useEffect(() => {
    console.log('Component mounted');
    
    fetch('/api/data')
      .then(res => res.json())
      .then(setData);
  }, []); // Array vide
  
  return <div>{data ? data.title : 'Loading...'}</div>;
}
// Console : 
// Component mounted (une seule fois)
```

**Use cases :**
- Fetch initial data
- Setup subscriptions
- Add event listeners
- Initialize third-party libs

---

#### 3. Avec [dependencies] → Exécute quand les deps changent

```javascript
function SearchResults({ query }) {
  const [results, setResults] = useState([]);
  
  // ✅ Exécute quand query change
  useEffect(() => {
    console.log(`Searching for: ${query}`);
    
    fetch(`/api/search?q=${query}`)
      .then(res => res.json())
      .then(setResults);
  }, [query]); // Re-exécute si query change
  
  return (
    <ul>
      {results.map(item => <li key={item.id}>{item.name}</li>)}
    </ul>
  );
}
```

---

### Dependency Array : LA CLEF DE useEffect

**Règle d'or : Inclure TOUTES les valeurs utilisées dans l'effet**

```javascript
function Component() {
  const [count, setCount] = useState(0);
  const [multiplier, setMultiplier] = useState(2);
  
  // ❌ MAUVAIS : Manque multiplier dans les deps
  useEffect(() => {
    console.log(count * multiplier); // Utilise multiplier
  }, [count]); // ❌ Oublie multiplier !
  
  // ✅ BON : Toutes les deps présentes
  useEffect(() => {
    console.log(count * multiplier);
  }, [count, multiplier]); // ✅ Les deux !
}
```

**ESLint vous avertira si vous oubliez une dépendance !**

---

### Exemples Pratiques de useEffect

#### Exemple 1 : Fetch Data au Mount

```javascript
function UserProfile({ userId }) {
  const [user, setUser] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  
  useEffect(() => {
    async function fetchUser() {
      try {
        setLoading(true);
        const res = await fetch(`/api/users/${userId}`);
        if (!res.ok) throw new Error('Failed to fetch');
        const data = await res.json();
        setUser(data);
      } catch (err) {
        setError(err.message);
      } finally {
        setLoading(false);
      }
    }
    
    fetchUser();
  }, [userId]); // Re-fetch si userId change
  
  if (loading) return <div>Loading...</div>;
  if (error) return <div>Error: {error}</div>;
  
  return (
    <div>
      <h1>{user.name}</h1>
      <p>{user.email}</p>
    </div>
  );
}
```

---

#### Exemple 2 : Document Title

```javascript
function Counter() {
  const [count, setCount] = useState(0);
  
  // Met à jour le titre du navigateur
  useEffect(() => {
    document.title = `Count: ${count}`;
  }, [count]);
  
  return (
    <button onClick={() => setCount(count + 1)}>
      Clicked {count} times
    </button>
  );
}
```

---

#### Exemple 3 : Timer/Interval

```javascript
function Timer() {
  const [seconds, setSeconds] = useState(0);
  
  useEffect(() => {
    // Créer l'interval
    const intervalId = setInterval(() => {
      setSeconds(s => s + 1);
    }, 1000);
    
    // ✅ CLEANUP : Nettoyer à l'unmount
    return () => {
      clearInterval(intervalId);
    };
  }, []); // Une seule fois
  
  return <div>Elapsed: {seconds}s</div>;
}
```

---

#### Exemple 4 : Event Listener

```javascript
function WindowSize() {
  const [width, setWidth] = useState(window.innerWidth);
  
  useEffect(() => {
    function handleResize() {
      setWidth(window.innerWidth);
    }
    
    // Add listener
    window.addEventListener('resize', handleResize);
    
    // ✅ CLEANUP : Remove listener
    return () => {
      window.removeEventListener('resize', handleResize);
    };
  }, []);
  
  return <div>Window width: {width}px</div>;
}
```

---

#### Exemple 5 : localStorage Sync

```javascript
function useLocalStorage(key, initialValue) {
  const [value, setValue] = useState(() => {
    const saved = localStorage.getItem(key);
    return saved ? JSON.parse(saved) : initialValue;
  });
  
  // Sync avec localStorage quand value change
  useEffect(() => {
    localStorage.setItem(key, JSON.stringify(value));
  }, [key, value]);
  
  return [value, setValue];
}

// Usage
function App() {
  const [theme, setTheme] = useLocalStorage('theme', 'dark');
  
  return (
    <button onClick={() => setTheme(theme === 'dark' ? 'light' : 'dark')}>
      Theme: {theme}
    </button>
  );
}
```

---

### Cleanup Function : QUAND ET POURQUOI ?

**Cleanup = Fonction retournée par useEffect pour "nettoyer"**

```javascript
useEffect(() => {
  // Setup
  
  return () => {
    // Cleanup
  };
}, [deps]);
```

**Quand cleanup s'exécute ?**
1. Avant que l'effet se ré-exécute (si deps changent)
2. Quand le component unmount (se démonte)

---

**Exemple : Subscription**

```javascript
function ChatRoom({ roomId }) {
  useEffect(() => {
    // 1. Setup : Connect au chat
    const connection = createConnection(roomId);
    connection.connect();
    console.log(`Connected to ${roomId}`);
    
    // 2. Cleanup : Disconnect
    return () => {
      connection.disconnect();
      console.log(`Disconnected from ${roomId}`);
    };
  }, [roomId]);
  
  return <div>Chat: {roomId}</div>;
}

// Scénario :
// roomId change de "general" à "random"

// Console :
// Connected to general
// Disconnected from general  ← Cleanup appelé !
// Connected to random
```

---

**Cas où cleanup est NÉCESSAIRE :**

```javascript
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

// ✅ Subscriptions
useEffect(() => {
  const sub = api.subscribe(data => console.log(data));
  return () => sub.unsubscribe(); // OBLIGATOIRE
}, []);

// ❌ Pas besoin de cleanup
useEffect(() => {
  console.log('Hello'); // Pas de ressource à nettoyer
}, []);
```

---

### ⚠️ PIÈGES COURANTS AVEC useEffect

#### Piège 1 : Boucle Infinie

```javascript
// ❌ BOUCLE INFINIE !
function Component() {
  const [count, setCount] = useState(0);
  
  useEffect(() => {
    setCount(count + 1); // Change le state
  }); // Pas de deps → Re-render → useEffect → setCount → Re-render → ...
  
  return <div>{count}</div>;
}

// ✅ SOLUTION : Ajouter deps
useEffect(() => {
  // Logique qui ne doit s'exécuter qu'une fois
}, []); // Array vide
```

---

#### Piège 2 : Dépendances Manquantes

```javascript
// ❌ MAUVAIS : ESLint warning
function SearchResults({ query }) {
  const [results, setResults] = useState([]);
  const apiKey = 'abc123';
  
  useEffect(() => {
    fetch(`/api/search?q=${query}&key=${apiKey}`) // Utilise query et apiKey
      .then(res => res.json())
      .then(setResults);
  }, []); // ❌ Manque query et apiKey !
  
  // Problème : Si query change, pas de nouveau fetch !
}

// ✅ BON
useEffect(() => {
  fetch(`/api/search?q=${query}&key=${apiKey}`)
    .then(res => res.json())
    .then(setResults);
}, [query, apiKey]); // ✅ Toutes les deps
```

---

#### Piège 3 : Oublier le Cleanup

```javascript
// ❌ MEMORY LEAK !
function Timer() {
  const [count, setCount] = useState(0);
  
  useEffect(() => {
    setInterval(() => {
      setCount(c => c + 1);
    }, 1000);
    // ❌ Pas de cleanup → Interval continue après unmount !
  }, []);
}

// ✅ BON
useEffect(() => {
  const id = setInterval(() => {
    setCount(c => c + 1);
  }, 1000);
  
  return () => clearInterval(id); // ✅ Cleanup
}, []);
```

---

#### Piège 4 : Fetch avec Race Condition

```javascript
// ❌ RACE CONDITION
function UserProfile({ userId }) {
  const [user, setUser] = useState(null);
  
  useEffect(() => {
    fetch(`/api/users/${userId}`)
      .then(res => res.json())
      .then(setUser);
  }, [userId]);
  
  // Problème : Si userId change rapidement (1 → 2 → 3)
  // Les réponses peuvent arriver dans le désordre !
  // Réponse user 3 → user 1 → user 2
  // On affiche finalement user 2 alors qu'on veut user 3 !
}

// ✅ SOLUTION : Ignore flag
useEffect(() => {
  let ignore = false;
  
  fetch(`/api/users/${userId}`)
    .then(res => res.json())
    .then(data => {
      if (!ignore) {
        setUser(data);
      }
    });
  
  return () => {
    ignore = true; // Ignore les réponses après unmount/re-render
  };
}, [userId]);
```

---

#### Piège 5 : useEffect avec Object/Array Dependencies

```javascript
// ❌ MAUVAIS : Object recréé à chaque render
function Component() {
  const options = { method: 'GET' }; // ❌ Nouveau object chaque fois
  
  useEffect(() => {
    fetch('/api/data', options);
  }, [options]); // ❌ options change toujours → Boucle !
}

// ✅ SOLUTION 1 : Déplacer dans useEffect
useEffect(() => {
  const options = { method: 'GET' };
  fetch('/api/data', options);
}, []); // Pas de deps sur options

// ✅ SOLUTION 2 : useMemo (voir Chapitre 7)
const options = useMemo(() => ({ method: 'GET' }), []);

useEffect(() => {
  fetch('/api/data', options);
}, [options]);
```

---

## 📌 useRef : VALEURS PERSISTANTES SANS RE-RENDER

### Concept

**useRef = Créer une "boîte" qui persiste entre les renders**

```javascript
const ref = useRef(initialValue);

// ref = { current: initialValue }
// ref.current = valeur modifiable
```

**Différences avec useState :**

| | useState | useRef |
|---|---|---|
| **Change trigger re-render ?** | ✅ Oui | ❌ Non |
| **Valeur persiste ?** | ✅ Oui | ✅ Oui |
| **Immuable ?** | ✅ Oui | ❌ Non (mutable) |
| **Usage** | UI data | DOM refs, values |

---

### Use Case 1 : Référence DOM

```javascript
function TextInput() {
  const inputRef = useRef(null);
  
  function handleFocus() {
    // Accéder au DOM element
    inputRef.current.focus();
  }
  
  return (
    <div>
      <input ref={inputRef} type="text" />
      <button onClick={handleFocus}>Focus Input</button>
    </div>
  );
}
```

**Exemples pratiques :**

```javascript
// Scroll to element
function ScrollExample() {
  const sectionRef = useRef(null);
  
  const scrollToSection = () => {
    sectionRef.current.scrollIntoView({ behavior: 'smooth' });
  };
  
  return (
    <>
      <button onClick={scrollToSection}>Go to Section</button>
      <div style={{ height: '1000px' }}>...</div>
      <section ref={sectionRef}>Target Section</section>
    </>
  );
}

// Measure element size
function ElementSize() {
  const divRef = useRef(null);
  const [size, setSize] = useState({ width: 0, height: 0 });
  
  useEffect(() => {
    if (divRef.current) {
      const { width, height } = divRef.current.getBoundingClientRect();
      setSize({ width, height });
    }
  }, []);
  
  return (
    <div ref={divRef}>
      Size: {size.width} x {size.height}
    </div>
  );
}
```

---

### Use Case 2 : Valeur Persistante (sans re-render)

```javascript
// Compter les renders
function RenderCounter() {
  const renderCount = useRef(0);
  
  // ✅ renderCount persiste mais ne trigger pas de re-render
  useEffect(() => {
    renderCount.current += 1;
  });
  
  return <div>This component rendered {renderCount.current} times</div>;
}

// ❌ MAUVAIS avec useState
function RenderCounter() {
  const [renderCount, setRenderCount] = useState(0);
  
  useEffect(() => {
    setRenderCount(renderCount + 1); // ❌ Trigger re-render → Boucle !
  });
}
```

---

### Use Case 3 : Valeur Précédente

```javascript
function usePrevious(value) {
  const ref = useRef();
  
  useEffect(() => {
    ref.current = value; // Sauvegarder après render
  }, [value]);
  
  return ref.current; // Retourne la valeur d'avant
}

// Usage
function Counter() {
  const [count, setCount] = useState(0);
  const prevCount = usePrevious(count);
  
  return (
    <div>
      <p>Current: {count}</p>
      <p>Previous: {prevCount}</p>
      <button onClick={() => setCount(count + 1)}>Increment</button>
    </div>
  );
}

// Exemple :
// Initial → Current: 0, Previous: undefined
// Click → Current: 1, Previous: 0
// Click → Current: 2, Previous: 1
```

---

### Use Case 4 : Stocker Interval/Timeout IDs

```javascript
function Stopwatch() {
  const [time, setTime] = useState(0);
  const [isRunning, setIsRunning] = useState(false);
  const intervalRef = useRef(null);
  
  function start() {
    setIsRunning(true);
    intervalRef.current = setInterval(() => {
      setTime(t => t + 1);
    }, 1000);
  }
  
  function stop() {
    setIsRunning(false);
    clearInterval(intervalRef.current);
  }
  
  function reset() {
    stop();
    setTime(0);
  }
  
  return (
    <div>
      <p>Time: {time}s</p>
      {!isRunning ? (
        <button onClick={start}>Start</button>
      ) : (
        <button onClick={stop}>Stop</button>
      )}
      <button onClick={reset}>Reset</button>
    </div>
  );
}
```

---

### Use Case 5 : Éviter Stale Closures

```javascript
// ❌ Problème : Stale closure
function Component() {
  const [count, setCount] = useState(0);
  
  useEffect(() => {
    const id = setInterval(() => {
      console.log(count); // ❌ Toujours 0 ! (closure)
    }, 1000);
    
    return () => clearInterval(id);
  }, []); // Exécute une seule fois
  
  return <button onClick={() => setCount(count + 1)}>Count: {count}</button>;
}

// ✅ Solution avec useRef
function Component() {
  const [count, setCount] = useState(0);
  const countRef = useRef(count);
  
  useEffect(() => {
    countRef.current = count; // Toujours à jour
  }, [count]);
  
  useEffect(() => {
    const id = setInterval(() => {
      console.log(countRef.current); // ✅ Valeur actuelle !
    }, 1000);
    
    return () => clearInterval(id);
  }, []);
}
```

---

### ⚠️ useRef vs useState : QUAND UTILISER QUOI ?

```javascript
// ✅ useState : Si besoin de re-render
const [name, setName] = useState(''); // Change → UI update

// ✅ useRef : Si PAS besoin de re-render
const renderCount = useRef(0); // Change → Pas de UI update

// ❌ MAUVAIS : useRef pour UI data
function Component() {
  const countRef = useRef(0);
  
  return (
    <div>
      <p>{countRef.current}</p> {/* ❌ Pas de re-render si change ! */}
      <button onClick={() => countRef.current += 1}>+</button>
    </div>
  );
  // Le bouton augmente countRef.current mais l'UI ne se met pas à jour !
}

// ✅ BON : useState pour UI
function Component() {
  const [count, setCount] = useState(0);
  
  return (
    <div>
      <p>{count}</p>
      <button onClick={() => setCount(count + 1)}>+</button>
    </div>
  );
}
```

---

## 🎨 CUSTOM HOOKS : RÉUTILISER LA LOGIQUE

### Concept

**Custom Hook = Function qui utilise d'autres Hooks**

Règles :
1. ✅ Nom commence par `use` (convention)
2. ✅ Peut utiliser d'autres Hooks
3. ✅ Retourne ce qu'on veut (value, array, object)

---

### Exemple 1 : useFetch

```javascript
function useFetch(url) {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  
  useEffect(() => {
    async function fetchData() {
      try {
        setLoading(true);
        const res = await fetch(url);
        if (!res.ok) throw new Error(`HTTP error! status: ${res.status}`);
        const json = await res.json();
        setData(json);
      } catch (err) {
        setError(err.message);
      } finally {
        setLoading(false);
      }
    }
    
    fetchData();
  }, [url]);
  
  return { data, loading, error };
}

// Usage
function UserProfile({ userId }) {
  const { data: user, loading, error } = useFetch(`/api/users/${userId}`);
  
  if (loading) return <div>Loading...</div>;
  if (error) return <div>Error: {error}</div>;
  
  return (
    <div>
      <h1>{user.name}</h1>
      <p>{user.email}</p>
    </div>
  );
}
```

---

### Exemple 2 : useLocalStorage

```javascript
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

---

### Exemple 3 : useToggle

```javascript
function useToggle(initialValue = false) {
  const [value, setValue] = useState(initialValue);
  
  const toggle = () => setValue(v => !v);
  const setTrue = () => setValue(true);
  const setFalse = () => setValue(false);
  
  return [value, toggle, setTrue, setFalse];
}

// Usage
function Modal() {
  const [isOpen, toggle, open, close] = useToggle(false);
  
  return (
    <div>
      <button onClick={open}>Open Modal</button>
      
      {isOpen && (
        <div className="modal">
          <p>Modal Content</p>
          <button onClick={close}>Close</button>
        </div>
      )}
    </div>
  );
}
```

---

### Exemple 4 : useDebounce

```javascript
function useDebounce(value, delay) {
  const [debouncedValue, setDebouncedValue] = useState(value);
  
  useEffect(() => {
    const handler = setTimeout(() => {
      setDebouncedValue(value);
    }, delay);
    
    return () => {
      clearTimeout(handler);
    };
  }, [value, delay]);
  
  return debouncedValue;
}

// Usage : Search avec debounce
function SearchBar() {
  const [searchTerm, setSearchTerm] = useState('');
  const debouncedSearchTerm = useDebounce(searchTerm, 500);
  
  const { data: results } = useFetch(
    `/api/search?q=${debouncedSearchTerm}`
  );
  
  return (
    <div>
      <input
        value={searchTerm}
        onChange={(e) => setSearchTerm(e.target.value)}
        placeholder="Search..."
      />
      <ul>
        {results?.map(item => <li key={item.id}>{item.name}</li>)}
      </ul>
    </div>
  );
}
// User tape : "r" → "re" → "rea" → "reac" → "react"
// API call seulement 500ms après le dernier caractère !
```

---

### Exemple 5 : useWindowSize

```javascript
function useWindowSize() {
  const [windowSize, setWindowSize] = useState({
    width: window.innerWidth,
    height: window.innerHeight,
  });
  
  useEffect(() => {
    function handleResize() {
      setWindowSize({
        width: window.innerWidth,
        height: window.innerHeight,
      });
    }
    
    window.addEventListener('resize', handleResize);
    
    return () => {
      window.removeEventListener('resize', handleResize);
    };
  }, []);
  
  return windowSize;
}

// Usage
function ResponsiveComponent() {
  const { width } = useWindowSize();
  
  return (
    <div>
      {width < 768 ? (
        <MobileNav />
      ) : (
        <DesktopNav />
      )}
      <p>Window width: {width}px</p>
    </div>
  );
}
```

---

### Exemple 6 : useOnClickOutside

```javascript
function useOnClickOutside(ref, handler) {
  useEffect(() => {
    function listener(event) {
      // Ne rien faire si on clique à l'intérieur
      if (!ref.current || ref.current.contains(event.target)) {
        return;
      }
      handler(event);
    }
    
    document.addEventListener('mousedown', listener);
    document.addEventListener('touchstart', listener);
    
    return () => {
      document.removeEventListener('mousedown', listener);
      document.removeEventListener('touchstart', listener);
    };
  }, [ref, handler]);
}

// Usage : Dropdown qui se ferme au click extérieur
function Dropdown() {
  const [isOpen, setIsOpen] = useState(false);
  const dropdownRef = useRef(null);
  
  useOnClickOutside(dropdownRef, () => setIsOpen(false));
  
  return (
    <div ref={dropdownRef}>
      <button onClick={() => setIsOpen(!isOpen)}>
        Toggle Dropdown
      </button>
      
      {isOpen && (
        <ul className="dropdown-menu">
          <li>Option 1</li>
          <li>Option 2</li>
          <li>Option 3</li>
        </ul>
      )}
    </div>
  );
}
```

---

### Exemple 7 : useKeyPress

```javascript
function useKeyPress(targetKey) {
  const [keyPressed, setKeyPressed] = useState(false);
  
  useEffect(() => {
    function downHandler({ key }) {
      if (key === targetKey) {
        setKeyPressed(true);
      }
    }
    
    function upHandler({ key }) {
      if (key === targetKey) {
        setKeyPressed(false);
      }
    }
    
    window.addEventListener('keydown', downHandler);
    window.addEventListener('keyup', upHandler);
    
    return () => {
      window.removeEventListener('keydown', downHandler);
      window.removeEventListener('keyup', upHandler);
    };
  }, [targetKey]);
  
  return keyPressed;
}

// Usage : Konami code ou shortcuts
function Game() {
  const wPressed = useKeyPress('w');
  const aPressed = useKeyPress('a');
  const sPressed = useKeyPress('s');
  const dPressed = useKeyPress('d');
  
  return (
    <div>
      <p>Use WASD to move</p>
      <div>W: {wPressed ? '↑' : '-'}</div>
      <div>A: {aPressed ? '←' : '-'}</div>
      <div>S: {sPressed ? '↓' : '-'}</div>
      <div>D: {dPressed ? '→' : '-'}</div>
    </div>
  );
}
```

---

### Exemple 8 : useInterval

```javascript
function useInterval(callback, delay) {
  const savedCallback = useRef();
  
  // Sauvegarder le callback le plus récent
  useEffect(() => {
    savedCallback.current = callback;
  }, [callback]);
  
  // Setup interval
  useEffect(() => {
    if (delay === null) return;
    
    const id = setInterval(() => {
      savedCallback.current();
    }, delay);
    
    return () => clearInterval(id);
  }, [delay]);
}

// Usage
function Counter() {
  const [count, setCount] = useState(0);
  const [delay, setDelay] = useState(1000);
  const [isRunning, setIsRunning] = useState(true);
  
  useInterval(
    () => setCount(count + 1),
    isRunning ? delay : null // null = pause
  );
  
  return (
    <div>
      <h1>{count}</h1>
      <button onClick={() => setIsRunning(!isRunning)}>
        {isRunning ? 'Pause' : 'Resume'}
      </button>
      <input
        type="number"
        value={delay}
        onChange={(e) => setDelay(Number(e.target.value))}
      />
    </div>
  );
}
```

---

### Exemple 9 : useAsync

```javascript
function useAsync(asyncFunction, immediate = true) {
  const [status, setStatus] = useState('idle');
  const [data, setData] = useState(null);
  const [error, setError] = useState(null);
  
  const execute = async (...params) => {
    setStatus('pending');
    setData(null);
    setError(null);
    
    try {
      const response = await asyncFunction(...params);
      setData(response);
      setStatus('success');
      return response;
    } catch (err) {
      setError(err);
      setStatus('error');
      throw err;
    }
  };
  
  useEffect(() => {
    if (immediate) {
      execute();
    }
  }, []);
  
  return { execute, status, data, error };
}

// Usage
function UserList() {
  const fetchUsers = () => fetch('/api/users').then(r => r.json());
  const { data: users, status, error, execute } = useAsync(fetchUsers);
  
  if (status === 'pending') return <div>Loading...</div>;
  if (status === 'error') return <div>Error: {error.message}</div>;
  
  return (
    <div>
      <button onClick={execute}>Refresh</button>
      <ul>
        {users?.map(user => <li key={user.id}>{user.name}</li>)}
      </ul>
    </div>
  );
}
```

---

### Bonnes Pratiques Custom Hooks

**1. Nommer avec le préfixe `use`**

```javascript
// ✅ BON
function useUser() { }
function useFetch() { }
function useLocalStorage() { }

// ❌ MAUVAIS
function getUser() { } // Pas de "use"
function fetchData() { }
```

---

**2. Retourner ce dont on a besoin**

```javascript
// ✅ Array (comme useState)
function useToggle() {
  const [value, setValue] = useState(false);
  return [value, setValue];
}
const [isOpen, setIsOpen] = useToggle();

// ✅ Object (meilleur si plusieurs valeurs)
function useFetch(url) {
  // ...
  return { data, loading, error, refetch };
}
const { data, loading } = useFetch('/api/users');

// ✅ Single value
function useWindowWidth() {
  // ...
  return width;
}
const width = useWindowWidth();
```

---

**3. Gérer les edge cases**

```javascript
function useLocalStorage(key, initialValue) {
  const [value, setValue] = useState(() => {
    try {
      const item = window.localStorage.getItem(key);
      return item ? JSON.parse(item) : initialValue;
    } catch (error) {
      // ✅ Gérer les erreurs (localStorage désactivé, JSON invalide)
      console.error('Error loading from localStorage:', error);
      return initialValue;
    }
  });
  
  const setStoredValue = (newValue) => {
    try {
      setValue(newValue);
      window.localStorage.setItem(key, JSON.stringify(newValue));
    } catch (error) {
      // ✅ Gérer quota exceeded, etc.
      console.error('Error saving to localStorage:', error);
    }
  };
  
  return [value, setStoredValue];
}
```

---

**4. Cleanup dans useEffect**

```javascript
function useEventListener(eventName, handler, element = window) {
  const savedHandler = useRef();
  
  useEffect(() => {
    savedHandler.current = handler;
  }, [handler]);
  
  useEffect(() => {
    const eventListener = (event) => savedHandler.current(event);
    element.addEventListener(eventName, eventListener);
    
    // ✅ Toujours cleanup
    return () => {
      element.removeEventListener(eventName, eventListener);
    };
  }, [eventName, element]);
}
```

---

## 🎯 QUESTIONS D'ENTRETIEN FRÉQUENTES

### Q1 : Quelle est la différence entre useEffect et useLayoutEffect ?

**Réponse :**

**useEffect :**
- ✅ Exécute **après** le paint (asynchrone)
- ✅ N'bloque pas le navigateur
- ✅ Utilisé pour 99% des cas

**useLayoutEffect :**
- ✅ Exécute **avant** le paint (synchrone)
- ⚠️ Bloque le navigateur
- ✅ Utilisé pour mesures DOM ou mutations synchrones

```javascript
// ✅ useEffect (normal)
useEffect(() => {
  // Fetch data, event listeners, subscriptions
}, []);

// ✅ useLayoutEffect (rare)
useLayoutEffect(() => {
  // Mesurer un élément DOM
  const height = divRef.current.getBoundingClientRect().height;
  setHeight(height);
}, []);
```

**Ordre d'exécution :**
```
1. Render
2. React met à jour le DOM
3. useLayoutEffect s'exécute (synchrone)
4. Browser paint
5. useEffect s'exécute (asynchrone)
```

---

### Q2 : Pourquoi on ne peut pas utiliser async directement dans useEffect ?

**Réponse :**

```javascript
// ❌ MAUVAIS : useEffect ne peut pas être async
useEffect(async () => {
  const data = await fetch('/api/data');
  setData(data);
}, []);
// Erreur : useEffect doit retourner une fonction ou rien,
// pas une Promise (ce que fait async)

// ✅ SOLUTION 1 : Fonction async à l'intérieur
useEffect(() => {
  async function fetchData() {
    const res = await fetch('/api/data');
    const json = await res.json();
    setData(json);
  }
  fetchData();
}, []);

// ✅ SOLUTION 2 : IIFE (Immediately Invoked Function Expression)
useEffect(() => {
  (async () => {
    const res = await fetch('/api/data');
    const json = await res.json();
    setData(json);
  })();
}, []);

// ✅ SOLUTION 3 : then/catch
useEffect(() => {
  fetch('/api/data')
    .then(res => res.json())
    .then(setData)
    .catch(setError);
}, []);
```

---

### Q3 : Comment éviter les appels API multiples avec useEffect ?

**Réponse :**

**Problème : Double fetch en développement avec React 18 Strict Mode**

```javascript
// ❌ En dev, ce code fetch 2 fois !
useEffect(() => {
  fetch('/api/data')
    .then(res => res.json())
    .then(setData);
}, []);
```

**Solutions :**

```javascript
// ✅ SOLUTION 1 : Ignore flag (race condition)
useEffect(() => {
  let ignore = false;
  
  fetch('/api/data')
    .then(res => res.json())
    .then(data => {
      if (!ignore) setData(data);
    });
  
  return () => {
    ignore = true;
  };
}, []);

// ✅ SOLUTION 2 : AbortController
useEffect(() => {
  const controller = new AbortController();
  
  fetch('/api/data', { signal: controller.signal })
    .then(res => res.json())
    .then(setData)
    .catch(err => {
      if (err.name !== 'AbortError') {
        console.error(err);
      }
    });
  
  return () => {
    controller.abort();
  };
}, []);

// ✅ SOLUTION 3 : React Query (meilleure pour production)
import { useQuery } from '@tanstack/react-query';

function Component() {
  const { data } = useQuery({
    queryKey: ['data'],
    queryFn: () => fetch('/api/data').then(r => r.json()),
  });
}
```

---

### Q4 : Quelle est la différence entre useRef et createRef ?

**Réponse :**

**useRef (Function Components) :**
- ✅ Persiste entre les renders
- ✅ Retourne toujours la même référence

**createRef (Class Components) :**
- ❌ Recrée une nouvelle ref à chaque render
- ✅ Utilisé dans les class components

```javascript
// ✅ useRef (Function Component)
function Component() {
  const inputRef = useRef(null);
  // inputRef garde la même référence entre les renders
  
  return <input ref={inputRef} />;
}

// ❌ createRef dans Function Component (mauvais)
function Component() {
  const inputRef = createRef(); // ❌ Nouvelle ref à chaque render !
  
  return <input ref={inputRef} />;
}

// ✅ createRef (Class Component)
class Component extends React.Component {
  constructor(props) {
    super(props);
    this.inputRef = createRef(); // ✅ Créé une seule fois
  }
  
  render() {
    return <input ref={this.inputRef} />;
  }
}
```

---

### Q5 : Comment passer des refs à des composants enfants ?

**Réponse :**

**Problème :** On ne peut pas passer `ref` comme une prop normale.

```javascript
// ❌ NE FONCTIONNE PAS
function Parent() {
  const inputRef = useRef(null);
  return <CustomInput ref={inputRef} />; // ❌ ref n'est pas une prop !
}

function CustomInput({ ref }) {
  return <input ref={ref} />; // ref est undefined
}
```

**Solution : forwardRef**

```javascript
// ✅ BON : Avec forwardRef
import { forwardRef } from 'react';

const CustomInput = forwardRef((props, ref) => {
  return <input ref={ref} {...props} />;
});

function Parent() {
  const inputRef = useRef(null);
  
  const focusInput = () => {
    inputRef.current.focus();
  };
  
  return (
    <>
      <CustomInput ref={inputRef} />
      <button onClick={focusInput}>Focus</button>
    </>
  );
}
```

**Avec useImperativeHandle (exposer seulement certaines méthodes) :**

```javascript
import { forwardRef, useImperativeHandle, useRef } from 'react';

const FancyInput = forwardRef((props, ref) => {
  const inputRef = useRef(null);
  
  // Exposer seulement focus et clear
  useImperativeHandle(ref, () => ({
    focus: () => {
      inputRef.current.focus();
    },
    clear: () => {
      inputRef.current.value = '';
    },
  }));
  
  return <input ref={inputRef} {...props} />;
});

// Usage
function Parent() {
  const fancyInputRef = useRef(null);
  
  return (
    <>
      <FancyInput ref={fancyInputRef} />
      <button onClick={() => fancyInputRef.current.focus()}>Focus</button>
      <button onClick={() => fancyInputRef.current.clear()}>Clear</button>
    </>
  );
}
```

---

### Q6 : Quand utiliser useEffect vs useLayoutEffect ?

**Réponse avec exemples :**

**useEffect (99% des cas) :**
```javascript
// ✅ Data fetching
useEffect(() => {
  fetch('/api/data').then(r => r.json()).then(setData);
}, []);

// ✅ Subscriptions
useEffect(() => {
  const sub = eventEmitter.on('update', handleUpdate);
  return () => sub.off('update', handleUpdate);
}, []);

// ✅ DOM mutations qui n'affectent pas le layout
useEffect(() => {
  document.title = `Count: ${count}`;
}, [count]);
```

**useLayoutEffect (rare, cas spécifiques) :**
```javascript
// ✅ Mesurer un élément avant paint
useLayoutEffect(() => {
  const rect = divRef.current.getBoundingClientRect();
  setDimensions({ width: rect.width, height: rect.height });
}, []);

// ✅ Scroll position
useLayoutEffect(() => {
  window.scrollTo(0, savedScrollPosition);
}, []);

// ✅ Animations qui doivent être synchrones
useLayoutEffect(() => {
  element.current.style.transform = `translateX(${position}px)`;
}, [position]);
```

**Règle :** Si tu vois un "flash" (flicker) avec useEffect, essaye useLayoutEffect.

---

### Q7 : Comment optimiser useEffect qui dépend d'une fonction ?

**Réponse :**

**Problème :**

```javascript
// ❌ MAUVAIS : Fonction recréée à chaque render
function Component({ userId }) {
  const fetchUser = () => {
    return fetch(`/api/users/${userId}`).then(r => r.json());
  };
  
  useEffect(() => {
    fetchUser().then(setUser);
  }, [fetchUser]); // ❌ fetchUser change à chaque render → Boucle !
}
```

**Solutions :**

```javascript
// ✅ SOLUTION 1 : Déplacer dans useEffect
function Component({ userId }) {
  useEffect(() => {
    const fetchUser = () => {
      return fetch(`/api/users/${userId}`).then(r => r.json());
    };
    
    fetchUser().then(setUser);
  }, [userId]); // Dépend seulement de userId
}

// ✅ SOLUTION 2 : useCallback (voir Chapitre 7)
function Component({ userId }) {
  const fetchUser = useCallback(() => {
    return fetch(`/api/users/${userId}`).then(r => r.json());
  }, [userId]); // Recréée seulement si userId change
  
  useEffect(() => {
    fetchUser().then(setUser);
  }, [fetchUser]);
}

// ✅ SOLUTION 3 : Dépendre directement de la valeur
function Component({ userId }) {
  useEffect(() => {
    fetch(`/api/users/${userId}`)
      .then(r => r.json())
      .then(setUser);
  }, [userId]); // Simple et clair
}
```

---

### Q8 : Comment tester un custom hook ?

**Réponse :**

**Avec @testing-library/react-hooks :**

```javascript
// useCounter.js
function useCounter(initialValue = 0) {
  const [count, setCount] = useState(initialValue);
  
  const increment = () => setCount(c => c + 1);
  const decrement = () => setCount(c => c - 1);
  const reset = () => setCount(initialValue);
  
  return { count, increment, decrement, reset };
}

// useCounter.test.js
import { renderHook, act } from '@testing-library/react';
import { useCounter } from './useCounter';

test('should increment counter', () => {
  const { result } = renderHook(() => useCounter());
  
  expect(result.current.count).toBe(0);
  
  act(() => {
    result.current.increment();
  });
  
  expect(result.current.count).toBe(1);
});

test('should reset to initial value', () => {
  const { result } = renderHook(() => useCounter(10));
  
  act(() => {
    result.current.increment();
    result.current.increment();
  });
  
  expect(result.current.count).toBe(12);
  
  act(() => {
    result.current.reset();
  });
  
  expect(result.current.count).toBe(10);
});
```

---

### Q9 : Différence entre useState et useReducer ?

**Réponse :**

**useState :**
- ✅ State simple (string, number, boolean)
- ✅ Une seule valeur
- ✅ Updates simples

**useReducer :**
- ✅ State complexe (objets, arrays)
- ✅ Multiple related values
- ✅ Logique complexe de mise à jour

```javascript
// ✅ useState pour simple state
const [count, setCount] = useState(0);
const [name, setName] = useState('');

// ✅ useReducer pour state complexe
function reducer(state, action) {
  switch (action.type) {
    case 'increment':
      return { ...state, count: state.count + 1 };
    case 'decrement':
      return { ...state, count: state.count - 1 };
    case 'setName':
      return { ...state, name: action.payload };
    default:
      return state;
  }
}

const [state, dispatch] = useReducer(reducer, { count: 0, name: '' });

// Dispatch actions
dispatch({ type: 'increment' });
dispatch({ type: 'setName', payload: 'John' });
```

**Quand utiliser useReducer :**
- Multiple values qui changent ensemble
- Logique de state complexe
- Next state dépend du previous state
- Veut partager la logique (dispatch)

---

### Q10 : Comment débugger un useEffect qui ne s'exécute pas ?

**Réponse avec techniques :**

```javascript
// Technique 1 : Logs
useEffect(() => {
  console.log('Effect ran');
  console.log('Dependencies:', dep1, dep2);
}, [dep1, dep2]);

// Technique 2 : Vérifier les deps avec useRef
function useTraceUpdate(props) {
  const prev = useRef(props);
  useEffect(() => {
    const changedProps = Object.entries(props).reduce((acc, [key, val]) => {
      if (prev.current[key] !== val) {
        acc[key] = {
          from: prev.current[key],
          to: val,
        };
      }
      return acc;
    }, {});
    
    if (Object.keys(changedProps).length > 0) {
      console.log('Changed props:', changedProps);
    }
    
    prev.current = props;
  });
}

// Usage
function Component({ userId, data }) {
  useTraceUpdate({ userId, data });
  
  useEffect(() => {
    // ...
  }, [userId, data]);
}

// Technique 3 : React DevTools
// Ouvrir DevTools → Components → Sélectionner component → Voir hooks

// Technique 4 : Vérifier les égalités
useEffect(() => {
  console.log('user changed:', user);
}, [user]); // Si user est un objet, il change à chaque render !

// Solution : Dépendre de user.id au lieu de user
useEffect(() => {
  // ...
}, [user.id]);
```

---

## 🔗 LIENS AVEC VOS PROJETS

### fast-react-pizza

**Ce que vous avez utilisé :**

```javascript
// Fetch menu au mount
useEffect(() => {
  async function fetchMenu() {
    const menu = await getMenu();
    setMenu(menu);
  }
  fetchMenu();
}, []);

// Geolocation avec custom loading state
const [status, setStatus] = useState('idle');

useEffect(() => {
  if (status === 'loading') {
    navigator.geolocation.getCurrentPosition(
      (position) => {
        setStatus('success');
        // ...
      },
      () => setStatus('error')
    );
  }
}, [status]);
```

**Points à mentionner :**
- ✅ "J'ai utilisé useEffect pour fetch le menu au mount"
- ✅ "J'ai géré les states loading/success/error"
- ✅ "J'ai utilisé useEffect avec dependencies pour la geolocation"

---

### wild-oasis

**Hooks utilisés :**

```javascript
// Custom hook pour Dark Mode
function useDarkMode() {
  const [isDark, setIsDark] = useLocalStorage('darkMode', false);
  
  useEffect(() => {
    if (isDark) {
      document.documentElement.classList.add('dark-mode');
    } else {
      document.documentElement.classList.remove('dark-mode');
    }
  }, [isDark]);
  
  return [isDark, setIsDark];
}

// useEffect pour close modal on Escape
useEffect(() => {
  function handleEscape(e) {
    if (e.key === 'Escape') {
      onClose();
    }
  }
  
  document.addEventListener('keydown', handleEscape);
  return () => document.removeEventListener('keydown', handleEscape);
}, [onClose]);
```

**Points à mentionner :**
- ✅ "J'ai créé un custom hook useDarkMode avec localStorage"
- ✅ "J'ai utilisé useEffect pour ajouter/retirer des event listeners"
- ✅ "J'ai toujours fait le cleanup pour éviter les memory leaks"

---

### worldwise

**Hooks pertinents :**

```javascript
// Fetch cities
useEffect(() => {
  async function fetchCities() {
    try {
      setIsLoading(true);
      const res = await fetch('http://localhost:8000/cities');
      const data = await res.json();
      setCities(data);
    } catch {
      setError('Failed to fetch cities');
    } finally {
      setIsLoading(false);
    }
  }
  fetchCities();
}, []);

// useRef pour map instance
const mapRef = useRef(null);
```

---

## 📚 RÉCAPITULATIF FINAL

### Ce qu'il FAUT savoir

**useEffect :**
- ✅ 3 comportements : sans deps / [] / [deps]
- ✅ Cleanup function pour nettoyer
- ✅ Dependencies array = toutes les valeurs utilisées
- ✅ Fetch data, subscriptions, DOM manipulation

**useRef :**
- ✅ Valeur qui persiste sans re-render
- ✅ Accès au DOM
- ✅ Stocker previous values, interval IDs

**Custom Hooks :**
- ✅ Réutiliser la logique
- ✅ Nom commence par `use`
- ✅ Peut utiliser d'autres hooks
- ✅ Return ce qu'on veut

**Règles des Hooks :**
- ✅ Top level uniquement
- ✅ React functions uniquement
- ✅ ESLint pour forcer

---