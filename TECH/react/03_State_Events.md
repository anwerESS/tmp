## [**..**](./00_index.md)

# 📘 CHAPITRE 3 : State, Events, and Forms

## 🎯 Objectifs du Chapitre
- Maîtriser useState et la gestion du state
- Comprendre les event handlers en React
- Gérer les forms et inputs contrôlés
- Maîtriser le state lifting
- Comprendre le derived state

---

## 1️⃣ WHAT IS STATE IN REACT?

### 🧠 **Définition du State**

> **State = "Mémoire" du composant. Données que le composant doit se rappeler au fil du temps.**

```jsx
// Sans state (statique)
function Greeting() {
  const name = "Alice";  // Variable normale
  return <h1>Hello {name}</h1>;
  // name ne peut pas changer → UI statique
}

// Avec state (dynamique)
function Greeting() {
  const [name, setName] = useState("Alice");  // State
  return (
    <div>
      <h1>Hello {name}</h1>
      <button onClick={() => setName("Bob")}>
        Change Name
      </button>
      {/* name peut changer → UI dynamique */}
    </div>
  );
}
```

### 📊 **State vs Variables Normales**

```jsx
function Counter() {
  // ❌ Variable normale (ne marche PAS)
  let count = 0;
  
  const increment = () => {
    count++;  // Modifie la variable
    console.log(count);  // Affiche la nouvelle valeur
    // MAIS l'UI ne change PAS! 🐛
  };
  
  return (
    <div>
      <p>Count: {count}</p>  {/* Reste à 0 */}
      <button onClick={increment}>+</button>
    </div>
  );
}

// ✅ Avec state (marche!)
function Counter() {
  const [count, setCount] = useState(0);
  
  const increment = () => {
    setCount(count + 1);  // Déclenche un re-render
    // L'UI se met à jour automatiquement! ✅
  };
  
  return (
    <div>
      <p>Count: {count}</p>  {/* Se met à jour */}
      <button onClick={increment}>+</button>
    </div>
  );
}
```

### 🎯 **Pourquoi le State ?**

**Le state permet à React de :**

```
1️⃣ PERSISTER les données entre les renders
   (les variables normales sont réinitialisées à chaque render)

2️⃣ DÉCLENCHER un re-render quand le state change
   (mettre à jour l'UI automatiquement)
```

```jsx
function Example() {
  const [count, setCount] = useState(0);
  
  console.log("Component rendered");
  // Ce log s'affiche à chaque fois que le composant render
  
  // 1er render: count = 0
  // Click → setCount(1)
  // 2ème render: count = 1 (valeur persistée!)
  // Click → setCount(2)
  // 3ème render: count = 2 (valeur persistée!)
  
  return <button onClick={() => setCount(count + 1)}>{count}</button>;
}
```

---

## 2️⃣ THE MECHANICS OF STATE (useState)

### 🔧 **Anatomie de useState**

```jsx
import { useState } from 'react';

function Component() {
  // [valeur actuelle, fonction pour la modifier]
  const [state, setState] = useState(initialValue);
  
  // Exemples concrets:
  const [count, setCount] = useState(0);
  const [name, setName] = useState("Alice");
  const [isOpen, setIsOpen] = useState(false);
  const [items, setItems] = useState([]);
  const [user, setUser] = useState({ name: "", email: "" });
}
```

**Règles importantes :**
```jsx
// 1️⃣ TOUJOURS déclarer au top level du composant
function Component() {
  const [count, setCount] = useState(0);  // ✅
  
  if (condition) {
    const [state, setState] = useState(0);  // ❌ INTERDIT!
  }
  
  for (let i = 0; i < 5; i++) {
    const [state, setState] = useState(0);  // ❌ INTERDIT!
  }
}

// 2️⃣ NE PAS appeler dans les callbacks directement sans fonction
useEffect(() => {
  const [state, setState] = useState(0);  // ❌ INTERDIT!
}, []);
```

### 📝 **Initialisation du State**

```jsx
// 1️⃣ VALEUR SIMPLE
const [count, setCount] = useState(0);
const [name, setName] = useState("Alice");
const [isActive, setIsActive] = useState(true);

// 2️⃣ OBJET
const [user, setUser] = useState({
  name: "Alice",
  email: "alice@example.com",
  age: 25
});

// 3️⃣ TABLEAU
const [items, setItems] = useState([]);
const [todos, setTodos] = useState([
  { id: 1, text: "Learn React", done: false },
  { id: 2, text: "Build project", done: false }
]);

// 4️⃣ LAZY INITIALIZATION (fonction)
// Utile si le calcul initial est coûteux
const [count, setCount] = useState(() => {
  const savedCount = localStorage.getItem('count');
  return savedCount ? JSON.parse(savedCount) : 0;
});
// La fonction ne s'exécute qu'une seule fois (au mount)

// ❌ Sans lazy initialization (calcul à chaque render)
const [count, setCount] = useState(
  localStorage.getItem('count') || 0  // Exécuté à CHAQUE render
);
```

### 🔄 **Updating State : Deux Façons**

```jsx
function Counter() {
  const [count, setCount] = useState(0);
  
  // ❌ MÉTHODE 1: Valeur directe (PROBLÈMES avec multiples updates)
  const increment = () => {
    setCount(count + 1);
    setCount(count + 1);  // Ne marche pas comme prévu!
    setCount(count + 1);  // count sera 1, pas 3 !
  };
  
  
  // ✅ MÉTHODE 2: Fonction callback (RECOMMANDÉE)
  const incrementCorrect = () => {
    setCount(c => c + 1);  // c = valeur actuelle
    setCount(c => c + 1);  // Fonctionne correctement!
    setCount(c => c + 1);  // count sera 3 ✅
  };
  
  return (
    <div>
      <p>Count: {count}</p>
      <button onClick={increment}>Bad Increment</button>
      <button onClick={incrementCorrect}>Good Increment</button>
    </div>
  );
}
```

**Quand utiliser quelle méthode ?**

```jsx
// ✅ Valeur directe : Quand le nouveau state ne dépend PAS de l'ancien
const handleToggle = () => {
  setIsOpen(true);  // Valeur fixe, indépendante
};

const handleReset = () => {
  setCount(0);  // Reset à une valeur fixe
};


// ✅ Fonction callback : Quand le nouveau state DÉPEND de l'ancien
const increment = () => {
  setCount(c => c + 1);  // Basé sur l'ancienne valeur
};

const addItem = (newItem) => {
  setItems(items => [...items, newItem]);  // Basé sur l'ancien tableau
};

const toggleTodo = (id) => {
  setTodos(todos => 
    todos.map(todo => 
      todo.id === id ? { ...todo, done: !todo.done } : todo
    )
  );
};
```

### ⚡ **State Updates sont ASYNCHRONES**

```jsx
function Counter() {
  const [count, setCount] = useState(0);
  
  const handleClick = () => {
    console.log("Before:", count);  // 0
    
    setCount(count + 1);
    
    console.log("After:", count);  // Toujours 0 ! (pas encore mis à jour)
    
    // Le state sera mis à jour APRÈS le re-render
  };
  
  // Pour accéder à la nouvelle valeur, utiliser useEffect
  useEffect(() => {
    console.log("Count updated:", count);
  }, [count]);
  
  return <button onClick={handleClick}>{count}</button>;
}
```

### 🔄 **State Batching (Optimisation React)**

```jsx
function Form() {
  const [firstName, setFirstName] = useState("");
  const [lastName, setLastName] = useState("");
  const [email, setEmail] = useState("");
  
  const handleSubmit = () => {
    setFirstName("Alice");  // ⚡
    setLastName("Smith");   // ⚡ Ces 3 updates
    setEmail("alice@...");  // ⚡ sont groupés!
    
    // React effectue UN SEUL re-render pour les 3
    // (pas 3 re-renders séparés) → Performance!
  };
  
  return (
    <form onSubmit={handleSubmit}>
      {/* ... */}
    </form>
  );
}
```

---

## 3️⃣ STATE AVEC OBJETS ET TABLEAUX

### 📦 **Updating Objects (Immutabilité)**

```jsx
function UserProfile() {
  const [user, setUser] = useState({
    name: "Alice",
    email: "alice@example.com",
    age: 25,
    address: {
      city: "Paris",
      country: "France"
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
  
  const updateEmail = () => {
    setUser(prevUser => ({
      ...prevUser,
      email: "bob@example.com"
    }));
  };
  
  // ✅ Nested object update
  const updateCity = () => {
    setUser({
      ...user,
      address: {
        ...user.address,  // Copie l'objet imbriqué
        city: "Lyon"      // Modifie la propriété
      }
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
  
  return (
    <div>
      <p>Name: {user.name}</p>
      <p>Email: {user.email}</p>
      <p>City: {user.address.city}</p>
      <button onClick={updateName}>Update Name</button>
      <button onClick={updateCity}>Update City</button>
    </div>
  );
}
```

### 📋 **Updating Arrays (Immutabilité)**

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
  
  // ✅ MODIFIER le texte d'un todo
  const updateTodoText = (id, newText) => {
    setTodos(todos.map(todo =>
      todo.id === id
        ? { ...todo, text: newText }
        : todo
    ));
  };
  
  // ✅ REMPLACER tout le tableau
  const clearCompleted = () => {
    setTodos(todos.filter(todo => !todo.done));
  };
  
  // ✅ TRIER le tableau
  const sortTodos = () => {
    setTodos([...todos].sort((a, b) => 
      a.text.localeCompare(b.text)
    ));
  };
  
  return (
    <div>
      {todos.map(todo => (
        <div key={todo.id}>
          <input
            type="checkbox"
            checked={todo.done}
            onChange={() => toggleTodo(todo.id)}
          />
          <span>{todo.text}</span>
          <button onClick={() => deleteTodo(todo.id)}>Delete</button>
        </div>
      ))}
      <button onClick={() => addTodo("New todo")}>Add</button>
    </div>
  );
}
```

### 🎯 **Cheat Sheet : Array Operations**

```jsx
// OPÉRATIONS IMMUTABLES (✅ À utiliser)
const arr = [1, 2, 3];

// Ajouter
[...arr, 4]           // [1, 2, 3, 4]
[0, ...arr]           // [0, 1, 2, 3]

// Supprimer
arr.filter(x => x !== 2)  // [1, 3]

// Remplacer
arr.map(x => x === 2 ? 20 : x)  // [1, 20, 3]

// Trier (attention: copier d'abord!)
[...arr].sort()       // [1, 2, 3]

// Inverser
[...arr].reverse()    // [3, 2, 1]


// OPÉRATIONS MUTABLES (❌ À éviter avec state)
arr.push(4)           // ❌ Mutation
arr.pop()             // ❌ Mutation
arr.shift()           // ❌ Mutation
arr.unshift(0)        // ❌ Mutation
arr.splice(1, 1)      // ❌ Mutation
arr.sort()            // ❌ Mutation (trie en place)
arr.reverse()         // ❌ Mutation (inverse en place)
```

---

## 4️⃣ EVENT HANDLERS

### 🎯 **Qu'est-ce qu'un Event Handler ?**

> **Event handler = Fonction qui s'exécute en réponse à un événement utilisateur**

```jsx
function Button() {
  // 1️⃣ Définir l'event handler
  const handleClick = () => {
    console.log("Button clicked!");
  };
  
  // 2️⃣ Attacher au JSX
  return (
    <button onClick={handleClick}>
      Click me
    </button>
  );
}
```

### 📝 **Syntaxes pour Event Handlers**

```jsx
function Example() {
  // ✅ MÉTHODE 1: Fonction nommée (recommandée)
  const handleClick = () => {
    console.log("Clicked");
  };
  
  return (
    <>
      <button onClick={handleClick}>Click 1</button>
      
      {/* ✅ MÉTHODE 2: Fonction anonyme inline */}
      <button onClick={() => console.log("Clicked")}>
        Click 2
      </button>
      
      {/* ✅ MÉTHODE 3: Avec paramètres */}
      <button onClick={() => handleClickWithParam(123)}>
        Click 3
      </button>
      
      {/* ❌ ERREUR COMMUNE: Appeler la fonction au lieu de la passer */}
      <button onClick={handleClick()}>  {/* ❌ S'exécute immédiatement! */}
        Bad Click
      </button>
    </>
  );
}
```

### 🎨 **Types d'Événements Courants**

```jsx
function EventsExample() {
  // Mouse events
  const handleClick = () => console.log("Click");
  const handleDoubleClick = () => console.log("Double click");
  const handleMouseEnter = () => console.log("Mouse enter");
  const handleMouseLeave = () => console.log("Mouse leave");
  
  // Keyboard events
  const handleKeyDown = (e) => console.log("Key:", e.key);
  const handleKeyPress = (e) => console.log("Key pressed");
  
  // Form events
  const handleSubmit = (e) => {
    e.preventDefault();  // Empêche rechargement page
    console.log("Form submitted");
  };
  
  const handleChange = (e) => {
    console.log("Value:", e.target.value);
  };
  
  const handleFocus = () => console.log("Input focused");
  const handleBlur = () => console.log("Input blurred");
  
  return (
    <div>
      {/* Mouse events */}
      <button onClick={handleClick}>Click</button>
      <button onDoubleClick={handleDoubleClick}>Double Click</button>
      <div 
        onMouseEnter={handleMouseEnter}
        onMouseLeave={handleMouseLeave}
      >
        Hover me
      </div>
      
      {/* Keyboard events */}
      <input 
        onKeyDown={handleKeyDown}
        onKeyPress={handleKeyPress}
      />
      
      {/* Form events */}
      <form onSubmit={handleSubmit}>
        <input 
          onChange={handleChange}
          onFocus={handleFocus}
          onBlur={handleBlur}
        />
        <button type="submit">Submit</button>
      </form>
    </div>
  );
}
```

### 📦 **Event Object (SyntheticEvent)**

```jsx
function InputExample() {
  const handleChange = (event) => {
    // React passe un SyntheticEvent (wrapper autour de l'event natif)
    
    console.log(event.target);        // L'élément DOM
    console.log(event.target.value);  // La valeur de l'input
    console.log(event.target.name);   // Le nom de l'input
    console.log(event.type);          // Type d'événement: "change"
    
    // Méthodes utiles
    event.preventDefault();   // Empêche comportement par défaut
    event.stopPropagation();  // Arrête la propagation (bubbling)
  };
  
  const handleKeyPress = (e) => {
    if (e.key === 'Enter') {
      console.log("Enter pressed!");
    }
    
    console.log("Key code:", e.keyCode);
    console.log("Is Shift pressed?", e.shiftKey);
    console.log("Is Ctrl pressed?", e.ctrlKey);
  };
  
  const handleClick = (e) => {
    console.log("Click position:", e.clientX, e.clientY);
    console.log("Button clicked:", e.button);  // 0 = left, 1 = middle, 2 = right
  };
  
  return (
    <>
      <input onChange={handleChange} onKeyPress={handleKeyPress} />
      <button onClick={handleClick}>Click</button>
    </>
  );
}
```

### 🎯 **Event Handler avec Paramètres**

```jsx
function TodoList() {
  const [todos, setTodos] = useState([
    { id: 1, text: "Todo 1" },
    { id: 2, text: "Todo 2" }
  ]);
  
  // ✅ MÉTHODE 1: Arrow function inline
  const deleteTodo = (id) => {
    setTodos(todos.filter(todo => todo.id !== id));
  };
  
  return (
    <ul>
      {todos.map(todo => (
        <li key={todo.id}>
          {todo.text}
          {/* Passer l'id au handler */}
          <button onClick={() => deleteTodo(todo.id)}>
            Delete
          </button>
        </li>
      ))}
    </ul>
  );
  
  
  // ✅ MÉTHODE 2: Currying (fonction qui retourne une fonction)
  const deleteTodoCurried = (id) => () => {
    setTodos(todos.filter(todo => todo.id !== id));
  };
  
  return (
    <ul>
      {todos.map(todo => (
        <li key={todo.id}>
          {todo.text}
          <button onClick={deleteTodoCurried(todo.id)}>
            Delete
          </button>
        </li>
      ))}
    </ul>
  );
}
```

---

## 5️⃣ FORMS AND CONTROLLED COMPONENTS

### 📝 **Controlled vs Uncontrolled Components**

```jsx
// ❌ UNCONTROLLED (pas recommandé en React)
function UncontrolledForm() {
  const handleSubmit = (e) => {
    e.preventDefault();
    // Accéder à la valeur via le DOM
    const value = e.target.elements.username.value;
    console.log(value);
  };
  
  return (
    <form onSubmit={handleSubmit}>
      <input name="username" />
      <button>Submit</button>
    </form>
  );
}


// ✅ CONTROLLED (recommandé)
function ControlledForm() {
  const [username, setUsername] = useState("");
  
  const handleSubmit = (e) => {
    e.preventDefault();
    console.log(username);  // Valeur disponible dans le state
  };
  
  return (
    <form onSubmit={handleSubmit}>
      <input 
        value={username}  // React contrôle la valeur
        onChange={(e) => setUsername(e.target.value)}
      />
      <button>Submit</button>
    </form>
  );
}
```

### 🎨 **Form Complet avec Controlled Inputs**

```jsx
function RegistrationForm() {
  const [formData, setFormData] = useState({
    username: "",
    email: "",
    password: "",
    age: "",
    country: "",
    acceptTerms: false,
    gender: ""
  });
  
  // Handler générique pour tous les inputs
  const handleChange = (e) => {
    const { name, value, type, checked } = e.target;
    
    setFormData(prev => ({
      ...prev,
      [name]: type === 'checkbox' ? checked : value
    }));
  };
  
  const handleSubmit = (e) => {
    e.preventDefault();
    console.log("Form data:", formData);
    // Envoyer au serveur, etc.
  };
  
  return (
    <form onSubmit={handleSubmit}>
      {/* Text input */}
      <div>
        <label>Username:</label>
        <input
          type="text"
          name="username"
          value={formData.username}
          onChange={handleChange}
        />
      </div>
      
      {/* Email input */}
      <div>
        <label>Email:</label>
        <input
          type="email"
          name="email"
          value={formData.email}
          onChange={handleChange}
        />
      </div>
      
      {/* Password input */}
      <div>
        <label>Password:</label>
        <input
          type="password"
          name="password"
          value={formData.password}
          onChange={handleChange}
        />
      </div>
      
      {/* Number input */}
      <div>
        <label>Age:</label>
        <input
          type="number"
          name="age"
          value={formData.age}
          onChange={handleChange}
        />
      </div>
      
      {/* Select */}
      <div>
        <label>Country:</label>
        <select
          name="country"
          value={formData.country}
          onChange={handleChange}
        >
          <option value="">Select...</option>
          <option value="france">France</option>
          <option value="usa">USA</option>
          <option value="canada">Canada</option>
        </select>
      </div>
      
      {/* Radio buttons */}
      <div>
        <label>Gender:</label>
        <label>
          <input
            type="radio"
            name="gender"
            value="male"
            checked={formData.gender === "male"}
            onChange={handleChange}
          />
          Male
        </label>
        <label>
          <input
            type="radio"
            name="gender"
            value="female"
            checked={formData.gender === "female"}
            onChange={handleChange}
          />
          Female
        </label>
      </div>
      
      {/* Checkbox */}
      <div>
        <label>
          <input
            type="checkbox"
            name="acceptTerms"
            checked={formData.acceptTerms}
            onChange={handleChange}
          />
          Accept Terms
        </label>
      </div>
      
      <button type="submit">Submit</button>
    </form>
  );
}
```

### ✅ **Form Validation**

```jsx
function LoginForm() {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [errors, setErrors] = useState({});
  
  const validate = () => {
    const newErrors = {};
    
    // Email validation
    if (!email) {
      newErrors.email = "Email is required";
    } else if (!/\S+@\S+\.\S+/.test(email)) {
      newErrors.email = "Email is invalid";
    }
    
    // Password validation
    if (!password) {
      newErrors.password = "Password is required";
    } else if (password.length < 6) {
      newErrors.password = "Password must be at least 6 characters";
    }
    
    return newErrors;
  };
  
  const handleSubmit = (e) => {
    e.preventDefault();
    
    const validationErrors = validate();
    
    if (Object.keys(validationErrors).length > 0) {
      setErrors(validationErrors);
      return;
    }
    
    // Form is valid
    console.log("Submitting:", { email, password });
    setErrors({});
  };
  
  return (
    <form onSubmit={handleSubmit}>
      <div>
        <input
          type="email"
          value={email}
          onChange={(e) => setEmail(e.target.value)}
          placeholder="Email"
        />
        {errors.email && <span className="error">{errors.email}</span>}
      </div>
      
      <div>
        <input
          type="password"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
          placeholder="Password"
        />
        {errors.password && <span className="error">{errors.password}</span>}
      </div>
      
      <button type="submit">Login</button>
    </form>
  );
}
```

---

## 6️⃣ STATE LIFTING (Lever le State)

### 🎯 **Le Problème : Sharing State entre Siblings**

```jsx
// ❌ PROBLÈME: Comment partager le state entre FilterPanel et ProductList?

function App() {
  return (
    <div>
      <FilterPanel />     {/* Besoin de filters */}
      <ProductList />     {/* Besoin des MÊMES filters */}
    </div>
  );
}

// Les deux sont siblings, ils ne peuvent pas se parler directement!
```

### ✅ **La Solution : Lifting State Up**

```jsx
// ✅ SOLUTION: Lever le state au parent commun

function App() {
  // State levé au niveau du parent
  const [filters, setFilters] = useState({
    category: 'all',
    priceRange: [0, 1000],
    inStock: false
  });
  
  return (
    <div>
      {/* Passer le state et le setter aux enfants */}
      <FilterPanel 
        filters={filters} 
        onFilterChange={setFilters} 
      />
      <ProductList filters={filters} />
    </div>
  );
}

function FilterPanel({ filters, onFilterChange }) {
  const handleCategoryChange = (category) => {
    onFilterChange({
      ...filters,
      category: category
    });
  };
  
  return (
    <div>
      <select 
        value={filters.category}
        onChange={(e) => handleCategoryChange(e.target.value)}
      >
        <option value="all">All</option>
        <option value="electronics">Electronics</option>
        <option value="clothing">Clothing</option>
      </select>
    </div>
  );
}

function ProductList({ filters }) {
  // Utilise filters pour afficher les produits filtrés
  return (
    <div>
      Showing products for: {filters.category}
    </div>
  );
}
```

### 📊 **Exemple Complet : Todo App avec State Lifting**

```jsx
function TodoApp() {
  // ✅ State centralisé au top level
  const [todos, setTodos] = useState([]);
  const [filter, setFilter] = useState('all');  // 'all', 'active', 'completed'
  
  // Actions sur les todos
  const addTodo = (text) => {
    setTodos([...todos, {
      id: Date.now(),
      text,
      completed: false
    }]);
  };
  
  const toggleTodo = (id) => {
    setTodos(todos.map(todo =>
      todo.id === id ? { ...todo, completed: !todo.completed } : todo
    ));
  };
  
  const deleteTodo = (id) => {
    setTodos(todos.filter(todo => todo.id !== id));
  };
  
  // Filtered todos (derived state - voir section suivante)
  const filteredTodos = todos.filter(todo => {
    if (filter === 'active') return !todo.completed;
    if (filter === 'completed') return todo.completed;
    return true;  // 'all'
  });
  
  return (
    <div className="todo-app">
      {/* Chaque composant reçoit ce dont il a besoin */}
      <AddTodoForm onAddTodo={addTodo} />
      
      <FilterButtons 
        currentFilter={filter}
        onFilterChange={setFilter}
      />
      
      <TodoList 
        todos={filteredTodos}
        onToggle={toggleTodo}
        onDelete={deleteTodo}
      />
      
      <TodoStats todos={todos} />
    </div>
  );
}

// Composants enfants
function AddTodoForm({ onAddTodo }) {
  const [text, setText] = useState("");
  
  const handleSubmit = (e) => {
    e.preventDefault();
    if (text.trim()) {
      onAddTodo(text);
      setText("");
    }
  };
  
  return (
    <form onSubmit={handleSubmit}>
      <input
        value={text}
        onChange={(e) => setText(e.target.value)}
        placeholder="What needs to be done?"
      />
      <button type="submit">Add</button>
    </form>
  );
}

function FilterButtons({ currentFilter, onFilterChange }) {
  return (
    <div className="filters">
      <button 
        onClick={() => onFilterChange('all')}
        className={currentFilter === 'all' ? 'active' : ''}
      >
        All
      </button>
      <button 
        onClick={() => onFilterChange('active')}
        className={currentFilter === 'active' ? 'active' : ''}
      >
        Active
      </button>
      <button 
        onClick={() => onFilterChange('completed')}
        className={currentFilter === 'completed' ? 'active' : ''}
      >
        Completed
      </button>
    </div>
  );
}

function TodoList({ todos, onToggle, onDelete }) {
  return (
    <ul className="todo-list">
      {todos.map(todo => (
        <TodoItem
          key={todo.id}
          todo={todo}
          onToggle={onToggle}
          onDelete={onDelete}
        />
      ))}
    </ul>
  );
}

function TodoItem({ todo, onToggle, onDelete }) {
  return (
    <li className={todo.completed ? 'completed' : ''}>
      <input
        type="checkbox"
        checked={todo.completed}
        onChange={() => onToggle(todo.id)}
      />
      <span>{todo.text}</span>
      <button onClick={() => onDelete(todo.id)}>×</button>
    </li>
  );
}

function TodoStats({ todos }) {
  const total = todos.length;
  const completed = todos.filter(t => t.completed).length;
  const active = total - completed;
  
  return (
    <div className="stats">
      <span>Total: {total}</span>
      <span>Active: {active}</span>
      <span>Completed: {completed}</span>
    </div>
  );
}
```

### 🎯 **Règles du State Lifting**

```
1️⃣ Identifier le PLUS PROCHE ancêtre commun des composants
   qui ont besoin du state

2️⃣ Placer le state dans cet ancêtre

3️⃣ Passer le state aux enfants via props

4️⃣ Passer les fonctions de modification (callbacks) aux enfants

5️⃣ Les enfants appellent les callbacks pour modifier le state parent
```

---

## 7️⃣ DERIVED STATE (State Dérivé)

### 🎯 **Qu'est-ce que le Derived State ?**

> **Derived state = State calculé à partir d'un state existant ou de props. Ce n'est PAS un nouveau state (pas de useState).**

```jsx
// ❌ MAUVAIS: Dupliquer le state
function ShoppingCart({ items }) {
  const [items, setItems] = useState([]);
  const [numItems, setNumItems] = useState(0);        // ❌ Redondant!
  const [totalPrice, setTotalPrice] = useState(0);    // ❌ Redondant!
  
  const addItem = (item) => {
    setItems([...items, item]);
    setNumItems(items.length + 1);                    // ❌ Besoin de synchroniser
    setTotalPrice(totalPrice + item.price);           // ❌ Besoin de synchroniser
  };
  // Problèmes: 3 state updates, risque de désynchronisation
}


// ✅ BON: Utiliser derived state
function ShoppingCart() {
  const [items, setItems] = useState([]);
  
  // ✅ Calculés à partir de items (derived state)
  const numItems = items.length;
  const totalPrice = items.reduce((sum, item) => sum + item.price, 0);
  
  const addItem = (item) => {
    setItems([...items, item]);  // Un seul state update
    // numItems et totalPrice se recalculent automatiquement!
  };
  
  return (
    <div>
      <p>Items: {numItems}</p>
      <p>Total: {totalPrice}€</p>
    </div>
  );
}
```

### 📊 **Exemples de Derived State**

```jsx
function UserDashboard({ users }) {
  // State source
  const [searchQuery, setSearchQuery] = useState("");
  const [sortBy, setSortBy] = useState("name");  // 'name' | 'age' | 'email'
  
  // ✅ Derived state 1: Filtered users
  const filteredUsers = users.filter(user =>
    user.name.toLowerCase().includes(searchQuery.toLowerCase())
  );
  
  // ✅ Derived state 2: Sorted users
  const sortedUsers = [...filteredUsers].sort((a, b) => {
    if (sortBy === 'name') return a.name.localeCompare(b.name);
    if (sortBy === 'age') return a.age - b.age;
    if (sortBy === 'email') return a.email.localeCompare(b.email);
    return 0;
  });
  
  // ✅ Derived state 3: Statistics
  const totalUsers = users.length;
  const averageAge = users.reduce((sum, u) => sum + u.age, 0) / totalUsers;
  const activeUsers = users.filter(u => u.isActive).length;
  
  return (
    <div>
      <input 
        value={searchQuery}
        onChange={(e) => setSearchQuery(e.target.value)}
        placeholder="Search users..."
      />
      
      <select value={sortBy} onChange={(e) => setSortBy(e.target.value)}>
        <option value="name">Sort by Name</option>
        <option value="age">Sort by Age</option>
        <option value="email">Sort by Email</option>
      </select>
      
      <div className="stats">
        <p>Total: {totalUsers}</p>
        <p>Average Age: {averageAge.toFixed(1)}</p>
        <p>Active: {activeUsers}</p>
        <p>Showing: {sortedUsers.length}</p>
      </div>
      
      <UserList users={sortedUsers} />
    </div>
  );
}
```

### 🎨 **Derived State avec Calculs Complexes**

```jsx
function ProductCatalog({ products }) {
  const [filters, setFilters] = useState({
    category: 'all',
    minPrice: 0,
    maxPrice: 1000,
    inStock: false,
    rating: 0
  });
  
  // ✅ Derived state: Filtered products
  const filteredProducts = products.filter(product => {
    // Category filter
    if (filters.category !== 'all' && product.category !== filters.category) {
      return false;
    }
    
    // Price filter
    if (product.price < filters.minPrice || product.price > filters.maxPrice) {
      return false;
    }
    
    // Stock filter
    if (filters.inStock && !product.inStock) {
      return false;
    }
    
    // Rating filter
    if (product.rating < filters.rating) {
      return false;
    }
    
    return true;
  });
  
  // ✅ Derived state: Statistics
  const stats = {
    total: products.length,
    showing: filteredProducts.length,
    avgPrice: filteredProducts.length > 0
      ? filteredProducts.reduce((sum, p) => sum + p.price, 0) / filteredProducts.length
      : 0,
    inStockCount: filteredProducts.filter(p => p.inStock).length
  };
  
  return (
    <div>
      <FilterPanel filters={filters} onFilterChange={setFilters} />
      <Stats stats={stats} />
      <ProductGrid products={filteredProducts} />
    </div>
  );
}
```

### ⚡ **Performance : useMemo pour Derived State Coûteux**

```jsx
function ExpensiveComponent({ data }) {
  // ❌ Sans useMemo: Recalculé à CHAQUE render (même si data ne change pas)
  const expensiveCalculation = computeExpensiveValue(data);
  
  // ✅ Avec useMemo: Recalculé SEULEMENT si data change
  const memoizedValue = useMemo(() => {
    return computeExpensiveValue(data);
  }, [data]);
  
  return <div>{memoizedValue}</div>;
}

// Exemple concret
function DataAnalysis({ items }) {
  const [filter, setFilter] = useState("");
  
  // ✅ Calculation coûteux memoizé
  const statistics = useMemo(() => {
    console.log("Calculating statistics...");
    
    // Calculs complexes
    const sorted = [...items].sort((a, b) => b.value - a.value);
    const sum = items.reduce((acc, item) => acc + item.value, 0);
    const average = sum / items.length;
    const median = sorted[Math.floor(sorted.length / 2)].value;
    const max = sorted[0].value;
    const min = sorted[sorted.length - 1].value;
    
    return { sum, average, median, max, min };
  }, [items]);  // Recalculé seulement si items change
  
  // filter change ne recalcule PAS statistics ✅
  
  return (
    <div>
      <input value={filter} onChange={(e) => setFilter(e.target.value)} />
      <div>
        <p>Sum: {statistics.sum}</p>
        <p>Average: {statistics.average}</p>
        <p>Median: {statistics.median}</p>
      </div>
    </div>
  );
}
```

### 🎯 **Quand utiliser Derived State vs useState ?**

```jsx
// ✅ UTILISER DERIVED STATE quand:
// - La valeur peut être calculée à partir d'un state existant
// - Pas besoin de persister entre renders (recalcul acceptable)
// - Évite la duplication et la désynchronisation

const fullName = `${firstName} ${lastName}`;  // ✅ Derived
const itemCount = items.length;                // ✅ Derived
const filteredData = data.filter(...);         // ✅ Derived


// ✅ UTILISER useState quand:
// - La valeur ne peut PAS être calculée à partir d'autres données
// - Besoin de contrôler quand la valeur change
// - La valeur est indépendante

const [isOpen, setIsOpen] = useState(false);     // ✅ State
const [inputValue, setInputValue] = useState(""); // ✅ State
const [selectedId, setSelectedId] = useState(null); // ✅ State
```

---

## 8️⃣ PRACTICAL GUIDELINES (Directives Pratiques)

### 🎯 **Quand Créer du State ?**

```
CHECKLIST:

1️⃣ Les données vont-elles changer ?
   NON → const variable
   OUI → Continuer...

2️⃣ Peut-on calculer à partir d'un state/props existant ?
   OUI → Derived state (pas de useState)
   NON → Continuer...

3️⃣ Le composant doit-il re-render quand ça change ?
   NON → useRef
   OUI → Continuer...

4️⃣ Utilisé seulement par ce composant ?
   OUI → Local state (useState dans le composant)
   NON → Continuer...

5️⃣ Utilisé par des siblings ou enfants ?
   OUI → Lift state up au parent commun
   NON → Continuer...

6️⃣ Utilisé partout dans l'app ?
   OUI → Global state (Context API, Redux)
```

### 📋 **Exemple d'Application des Guidelines**

```jsx
function ProductPage({ productId }) {
  // ✅ Local state (utilisé seulement ici)
  const [quantity, setQuantity] = useState(1);
  const [isInWishlist, setIsInWishlist] = useState(false);
  
  // ✅ Derived state (calculé à partir de quantity)
  const totalPrice = product.price * quantity;
  const canAddToCart = quantity > 0 && quantity <= product.stock;
  
  // ✅ Const (ne change jamais)
  const TAX_RATE = 0.20;
  
  // ✅ useRef (pas besoin de re-render)
  const inputRef = useRef(null);
  
  // ✅ Props (vient du parent)
  // productId
  
  return (
    <div>
      <h1>{product.name}</h1>
      
      <input
        ref={inputRef}
        type="number"
        value={quantity}
        onChange={(e) => setQuantity(Number(e.target.value))}
      />
      
      <p>Price: {totalPrice}€</p>
      <p>Tax: {totalPrice * TAX_RATE}€</p>
      <p>Total: {totalPrice * (1 + TAX_RATE)}€</p>
      
      <button 
        disabled={!canAddToCart}
        onClick={() => addToCart(product, quantity)}
      >
        Add to Cart
      </button>
      
      <button onClick={() => setIsInWishlist(!isInWishlist)}>
        {isInWishlist ? '❤️' : '🤍'} Wishlist
      </button>
    </div>
  );
}
```

### 🎨 **Penser en Termes de State Transitions**

```jsx
// ✅ BON: Penser en transitions de state
function Modal() {
  const [isOpen, setIsOpen] = useState(false);
  
  // Transitions claires
  const openModal = () => setIsOpen(true);
  const closeModal = () => setIsOpen(false);
  
  return (
    <>
      <button onClick={openModal}>Open</button>
      {isOpen && (
        <div className="modal">
          <button onClick={closeModal}>Close</button>
        </div>
      )}
    </>
  );
}

// ❌ MAUVAIS: Penser en manipulations DOM
function ModalBad() {
  const showModal = () => {
    document.querySelector('.modal').style.display = 'block';
  };
  
  const hideModal = () => {
    document.querySelector('.modal').style.display = 'none';
  };
  
  // Approche impérative, pas React!
}
```

---

## 🎯 QUESTIONS D'ENTRETIEN FRÉQUENTES

### Q1: **Qu'est-ce que le state et pourquoi en avons-nous besoin ?**
**Réponse :**
```
Le state est la "mémoire" d'un composant React.

POURQUOI NÉCESSAIRE ?
1️⃣ Persister les données entre les renders
   (variables normales sont réinitialisées)

2️⃣ Déclencher des re-renders automatiques
   (mettre à jour l'UI quand les données changent)

FORMULE: UI = f(state)
Quand state change → React re-render → UI mise à jour
```

### Q2: **Quelle est la différence entre state et props ?**
**Réponse :**
```jsx
// PROPS
// - Données EXTERNES (du parent)
// - READ-ONLY
// - Configurent le composant
function Child({ name }) {  // Reçoit en paramètre
  return <h1>{name}</h1>;
}

// STATE
// - Données INTERNES (au composant)
// - MUTABLE (via setState)
// - Rendent le composant interactif
function Parent() {
  const [name, setName] = useState("Alice");  // Créé et géré ici
  return <Child name={name} />;
}
```

### Q3: **Expliquez les controlled components**
**Réponse :**
```jsx
// CONTROLLED COMPONENT
// React contrôle la valeur de l'input via le state

function ControlledInput() {
  const [value, setValue] = useState("");
  
  return (
    <input
      value={value}              // React contrôle
      onChange={(e) => setValue(e.target.value)}
    />
  );
  
  // AVANTAGES:
  // ✅ Single source of truth (state)
  // ✅ Validation facile
  // ✅ Logique conditionnelle simple
  // ✅ État accessible partout dans le composant
}

// UNCONTROLLED (pas recommandé)
function UncontrolledInput() {
  const inputRef = useRef();
  
  const handleSubmit = () => {
    console.log(inputRef.current.value);  // Accès via DOM
  };
  
  return <input ref={inputRef} />;
}
```

### Q4: **Qu'est-ce que le state lifting et quand l'utiliser ?**
**Réponse :**
```
STATE LIFTING = Déplacer le state vers le plus proche ancêtre commun

QUAND L'UTILISER ?
Quand plusieurs composants siblings ont besoin du même state

EXEMPLE:
Parent (state ici)
  ↓ props
Child1 (utilise)    Child2 (utilise)

PROCESSUS:
1. Identifier les composants qui partagent les données
2. Trouver leur plus proche parent commun
3. Déplacer le state dans ce parent
4. Passer le state et callbacks en props

ALTERNATIVE: Context API si trop de prop drilling
```

### Q5: **Qu'est-ce que le derived state ?**
**Réponse :**
```jsx
// DERIVED STATE = State calculé à partir d'autres données
// PAS un nouveau useState !

function ShoppingCart() {
  const [items, setItems] = useState([]);
  
  // ✅ DERIVED STATE (calculés)
  const itemCount = items.length;
  const total = items.reduce((sum, item) => sum + item.price, 0);
  const isEmpty = items.length === 0;
  
  // ❌ MAUVAIS: Créer des states séparés
  // const [itemCount, setItemCount] = useState(0);
  // const [total, setTotal] = useState(0);
  // Problème: Besoin de synchroniser manuellement
  
  return (
    <div>
      <p>Items: {itemCount}</p>
      <p>Total: {total}€</p>
    </div>
  );
}

AVANTAGES:
✅ Pas de duplication de state
✅ Pas de risque de désynchronisation
✅ Single source of truth
```

### Q6: **Pourquoi les state updates sont-ils asynchrones ?**
**Réponse :**
```jsx
function Counter() {
  const [count, setCount] = useState(0);
  
  const handleClick = () => {
    setCount(count + 1);
    console.log(count);  // Affiche toujours l'ANCIENNE valeur!
  };
  
  // POURQUOI ASYNCHRONE ?
  
  1️⃣ PERFORMANCE: React batch plusieurs updates
     setCount(1); setName("Bob"); setEmail("...");
     → Un seul re-render au lieu de 3
  
  2️⃣ OPTIMISATION: React peut prioriser les updates
  
  3️⃣ COHÉRENCE: Garantit que le state reste cohérent
     pendant le render
  
  // SOLUTION: Utiliser callback ou useEffect
  setCount(c => c + 1);  // Callback avec valeur actuelle
  
  useEffect(() => {
    console.log(count);  // Après le re-render
  }, [count]);
}
```

### Q7: **Pourquoi doit-on traiter les objets et tableaux de manière immutable dans le state ?**
**Réponse :**
```jsx
// ❌ MUTATION (React ne détecte pas le changement)
const [user, setUser] = useState({ name: "Alice", age: 25 });

user.age = 26;      // ❌ Mutation!
setUser(user);      // React ne re-render pas! (même référence)


// ✅ IMMUTABLE (React détecte le changement)
setUser({ ...user, age: 26 });  // Nouvelle référence → re-render

// POURQUOI ?
1️⃣ React compare par RÉFÉRENCE (===)
   Même objet = même référence = pas de changement détecté

2️⃣ PRÉDICTIBILITÉ
   On sait toujours quel était l'ancien state

3️⃣ TIME-TRAVEL DEBUGGING
   Possibilité de revenir en arrière

4️⃣ OPTIMISATIONS
   React peut optimiser avec shallow comparison
```

### Q8: **Comment gérer des forms complexes ?**
**Réponse :**
```jsx
// STRATÉGIE 1: Un state par input (pour forms simples)
const [email, setEmail] = useState("");
const [password, setPassword] = useState("");

// STRATÉGIE 2: Un objet state (pour forms complexes)
const [formData, setFormData] = useState({
  email: "",
  password: "",
  username: "",
  age: ""
});

const handleChange = (e) => {
  const { name, value } = e.target;
  setFormData(prev => ({
    ...prev,
    [name]: value
  }));
};

// STRATÉGIE 3: Bibliothèque (pour forms très complexes)
// React Hook Form, Formik
import { useForm } from 'react-hook-form';

function MyForm() {
  const { register, handleSubmit, formState: { errors } } = useForm();
  
  return (
    <form onSubmit={handleSubmit(onSubmit)}>
      <input {...register("email", { required: true })} />
      {errors.email && <span>Required</span>}
    </form>
  );
}
```

---

## ⚠️ PIÈGES COURANTS

### 1. **Modifier le State Directement**
```jsx
// ❌ ERREUR FRÉQUENTE
const [items, setItems] = useState([1, 2, 3]);

items.push(4);        // ❌ Mutation!
setItems(items);      // React ne re-render pas

// ✅ CORRECT
setItems([...items, 4]);  // Nouveau tableau
```

### 2. **Utiliser l'Ancienne Valeur du State**
```jsx
// ❌ PROBLÈME avec multiples updates
const increment = () => {
  setCount(count + 1);
  setCount(count + 1);  // N'incrémente pas de 2!
  setCount(count + 1);  // Résultat: count + 1
};

// ✅ SOLUTION: Callback form
const increment = () => {
  setCount(c => c + 1);
  setCount(c => c + 1);
  setCount(c => c + 1);  // Résultat: count + 3 ✅
};
```

### 3. **Appeler l'Event Handler au lieu de le Passer**
```jsx
// ❌ ERREUR: Appel immédiat
<button onClick={handleClick()}>  {/* ❌ S'exécute au render! */}
  Click
</button>

// ✅ CORRECT: Passer la référence
<button onClick={handleClick}>  {/* ✅ S'exécute au click */}
  Click
</button>

// ✅ Avec paramètres
<button onClick={() => handleClick(id)}>  {/* ✅ */}
  Click
</button>
```

### 4. **Oublier e.preventDefault() dans les Forms**
```jsx
// ❌ Sans preventDefault (page reload)
const handleSubmit = (e) => {
  console.log("Submitting...");
  // Page recharge! 🐛
};

// ✅ Avec preventDefault
const handleSubmit = (e) => {
  e.preventDefault();  // ✅ Empêche le reload
  console.log("Submitting...");
};
```

### 5. **State Initialization Coûteuse**
```jsx
// ❌ Calcul exécuté à chaque render
const [data, setData] = useState(expensiveComputation());

// ✅ Lazy initialization (exécuté une seule fois)
const [data, setData] = useState(() => expensiveComputation());
```

### 6. **Comparer des Objets/Tableaux Directement**
```jsx
// ❌ NE MARCHE PAS
useEffect(() => {
  // ...
}, [user]);  // Nouvelle référence à chaque render!

// ✅ SOLUTIONS

// Option 1: Comparer des propriétés spécifiques
useEffect(() => {
  // ...
}, [user.id, user.name]);

// Option 2: useMemo pour stabiliser la référence
const stableUser = useMemo(() => user, [user.id, user.name]);

useEffect(() => {
  // ...
}, [stableUser]);
```

---

## 🔗 LIENS AVEC VOS PROJETS

### **04-steps**
```jsx
// useState basique
function Steps() {
  const [step, setStep] = useState(1);
  
  const handleNext = () => setStep(s => s + 1);
  const handlePrevious = () => setStep(s => s - 1);
}
```

### **05-travel-list**
```jsx
// State avec tableaux, forms, derived state
function App() {
  const [items, setItems] = useState([]);
  
  // Form controlled
  const [description, setDescription] = useState("");
  
  // Derived state
  const numItems = items.length;
  const numPacked = items.filter(item => item.packed).length;
  const percentage = (numPacked / numItems) * 100;
  
  // State lifting: items partagé entre Form et PackingList
}
```

### **06-eat-n-split**
```jsx
// State lifting, forms, event handlers
function App() {
  const [friends, setFriends] = useState([]);
  const [selectedFriend, setSelectedFriend] = useState(null);
  
  // Callbacks passés aux enfants
  const handleAddFriend = (friend) => {
    setFriends([...friends, friend]);
  };
  
  const handleSelection = (friend) => {
    setSelectedFriend(friend);
  };
}
```

### **07-usepopcorn**
```jsx
// Multiple pieces of state, derived state
function App() {
  const [movies, setMovies] = useState([]);
  const [watched, setWatched] = useState([]);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState("");
  
  // Derived state
  const avgImdbRating = watched.reduce((acc, movie) => 
    acc + movie.imdbRating, 0) / watched.length;
}
```

---

## 📝 RÉSUMÉ DU CHAPITRE

✅ **State** = Mémoire du composant, déclenche re-render quand il change  
✅ **useState** = Hook pour créer du state (valeur + setter)  
✅ **State updates** = Asynchrones et batched pour performance  
✅ **Immutabilité** = Objets/tableaux doivent être remplacés, pas mutés  
✅ **Event handlers** = Fonctions qui répondent aux événements utilisateur  
✅ **Controlled components** = React contrôle la valeur via state (forms)  
✅ **State lifting** = Lever le state au parent commun pour partage  
✅ **Derived state** = Calculé à partir d'autres données (pas de useState)  

---

## 🎓 POINTS CLÉS

1. **Expliquez useState avec un exemple** (question garantie)
2. **Différence state vs props** (très fréquent)
3. **Immutabilité** des objets/tableaux (important !)
4. **Controlled components** pour les forms
5. **State lifting** avec diagramme
6. **Async nature** des state updates
7. **Derived state** vs duplication
8. **Mentionnez vos projets** pour illustrer

---