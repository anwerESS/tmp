## [**..**](./00_index.md)

# 📘 CHAPITRE 2 : Components, Props & JSX

## 🎯 Objectifs du Chapitre
- Maîtriser la création et l'organisation des composants
- Comprendre les props et l'immutabilité
- Maîtriser le flux de données unidirectionnel
- Apprendre la composition de composants
- Savoir découper une UI en composants

---

## 1️⃣ COMPONENTS : LES BRIQUES DE CONSTRUCTION

### 🧱 **Qu'est-ce qu'un Composant ?**

> **Un composant est une fonction JavaScript qui retourne du JSX, encapsulant données, logique et apparence.**

```jsx
// Composant = Fonction + JSX
function Welcome() {
  // 1. LOGIQUE (JavaScript)
  const message = "Bienvenue";
  const currentTime = new Date().toLocaleTimeString();
  
  // 2. APPARENCE (JSX)
  return (
    <div className="welcome">
      <h1>{message}</h1>
      <p>Il est {currentTime}</p>
    </div>
  );
}
```

### 📦 **Anatomie d'un Composant**

```jsx
function ProductCard({ name, price, image, onAddToCart }) {
  // 🎯 DONNÉES (State, Props, Variables)
  const [quantity, setQuantity] = useState(1);
  const total = price * quantity;
  
  // 🧠 LOGIQUE (Fonctions, Calculs)
  const handleIncrement = () => {
    setQuantity(q => q + 1);
  };
  
  const handleAddToCart = () => {
    onAddToCart({ name, price, quantity });
  };
  
  // 🎨 APPARENCE (JSX Return)
  return (
    <div className="product-card">
      <img src={image} alt={name} />
      <h3>{name}</h3>
      <p>{price}€</p>
      
      <div className="quantity">
        <button onClick={() => setQuantity(q => q - 1)}>-</button>
        <span>{quantity}</span>
        <button onClick={handleIncrement}>+</button>
      </div>
      
      <p>Total: {total}€</p>
      <button onClick={handleAddToCart}>Ajouter au panier</button>
    </div>
  );
}
```

**Trois parties essentielles :**
1. **Données** : State, props, variables
2. **Logique** : Fonctions, event handlers, calculs
3. **Apparence** : JSX retourné

---

## 2️⃣ FUNCTION COMPONENTS vs CLASS COMPONENTS

### ⚡ **Function Components (Moderne - À utiliser)**

```jsx
// ✅ RECOMMANDÉ (Depuis React 16.8 avec Hooks)
function Counter() {
  const [count, setCount] = useState(0);
  
  useEffect(() => {
    document.title = `Count: ${count}`;
  }, [count]);
  
  return (
    <div>
      <p>Count: {count}</p>
      <button onClick={() => setCount(count + 1)}>
        Increment
      </button>
    </div>
  );
}
```

### 🏛️ **Class Components (Legacy - À connaître)**

```jsx
// ⚠️ ANCIEN STYLE (Avant Hooks)
class Counter extends React.Component {
  constructor(props) {
    super(props);
    this.state = { count: 0 };
    this.handleClick = this.handleClick.bind(this);
  }
  
  componentDidMount() {
    document.title = `Count: ${this.state.count}`;
  }
  
  componentDidUpdate() {
    document.title = `Count: ${this.state.count}`;
  }
  
  handleClick() {
    this.setState({ count: this.state.count + 1 });
  }
  
  render() {
    return (
      <div>
        <p>Count: {this.state.count}</p>
        <button onClick={this.handleClick}>
          Increment
        </button>
      </div>
    );
  }
}
```

### 📊 **Comparaison Complète**

| Critère | Function Components | Class Components |
|---------|-------------------|------------------|
| **Syntaxe** | ✅ Simple, moins de code | ❌ Verbeux, plus complexe |
| **State** | `useState` | `this.state` et `setState` |
| **Lifecycle** | `useEffect` (tout en un) | Méthodes séparées |
| **`this`** | ✅ Pas besoin | ❌ Binding nécessaire |
| **Performance** | ✅ Légèrement meilleur | ⚠️ Standard |
| **Hooks** | ✅ Disponibles | ❌ Non disponibles |
| **Code sharing** | ✅ Custom Hooks | ❌ HOC ou Render Props |
| **Futur React** | ✅ Recommandé | ⚠️ Supporté mais legacy |

### 💡 **Pourquoi Function Components ?**

```jsx
// 🎯 AVANTAGES

// 1. Code plus court et lisible
function User({ name }) {
  return <h1>Hello {name}</h1>;
}

// vs

class User extends React.Component {
  render() {
    return <h1>Hello {this.props.name}</h1>;
  }
}

// 2. Pas de confusion avec 'this'
// 3. Logique réutilisable avec Custom Hooks
// 4. Meilleure optimisation par React
```

**🎓 Pour l'entretien :**
- Mentionnez que vous connaissez les Class Components
- Expliquez que vous préférez les Function Components
- Indiquez que vous savez les migrer si nécessaire
- Citez votre projet **09-classy-weather** comme exemple de Class Components

---

## 3️⃣ PROPS : LA COMMUNICATION ENTRE COMPOSANTS

### 📨 **Qu'est-ce que les Props ?**

> **Props = "Properties" = Arguments passés à un composant pour le configurer**

```jsx
// Parent envoie des props
function App() {
  return (
    <div>
      <Greeting name="Alice" age={25} isAdmin={true} />
      <Greeting name="Bob" age={30} isAdmin={false} />
    </div>
  );
}

// Enfant reçoit les props
function Greeting(props) {
  // Accès via props.name, props.age, props.isAdmin
  return (
    <div>
      <h1>Hello {props.name}</h1>
      <p>Age: {props.age}</p>
      {props.isAdmin && <span>👑 Admin</span>}
    </div>
  );
}
```

### 🎁 **Destructuration des Props (Best Practice)**

```jsx
// ❌ Sans destructuration (moins lisible)
function Product(props) {
  return (
    <div>
      <h2>{props.name}</h2>
      <p>{props.price}€</p>
      <p>{props.description}</p>
    </div>
  );
}

// ✅ Avec destructuration (recommandé)
function Product({ name, price, description }) {
  return (
    <div>
      <h2>{name}</h2>
      <p>{price}€</p>
      <p>{description}</p>
    </div>
  );
}

// ✅ Avec valeurs par défaut
function Product({ 
  name, 
  price = 0, 
  description = "Pas de description",
  inStock = true 
}) {
  return (
    <div>
      <h2>{name}</h2>
      <p>{price}€</p>
      <p>{description}</p>
      {!inStock && <span>❌ Rupture de stock</span>}
    </div>
  );
}
```

### 🎨 **Types de Props**

```jsx
function Example({
  // 1️⃣ Valeurs primitives
  name,           // string
  age,            // number
  isActive,       // boolean
  
  // 2️⃣ Objets
  user,           // { id: 1, name: "Alice" }
  
  // 3️⃣ Tableaux
  items,          // [1, 2, 3]
  
  // 4️⃣ Fonctions
  onClick,        // () => {}
  onSubmit,       // (data) => {}
  
  // 5️⃣ Composants React
  icon,           // <Icon />
  
  // 6️⃣ Children (spécial)
  children
}) {
  return (
    <div>
      {/* Utilisation */}
    </div>
  );
}

// Utilisation
<Example
  name="Product"
  age={25}
  isActive={true}
  user={{ id: 1, name: "Alice" }}
  items={[1, 2, 3]}
  onClick={() => console.log('clicked')}
  onSubmit={(data) => handleSubmit(data)}
  icon={<StarIcon />}
>
  <p>Contenu children</p>
</Example>
```

### 📋 **Props Spread Pattern**

```jsx
const productData = {
  name: "Laptop",
  price: 999,
  brand: "Apple",
  inStock: true
};

// ❌ Répétitif
<Product 
  name={productData.name}
  price={productData.price}
  brand={productData.brand}
  inStock={productData.inStock}
/>

// ✅ Spread operator (élégant)
<Product {...productData} />

// ✅ Spread + override
<Product {...productData} price={899} />
```

---

## 4️⃣ PROPS IMMUTABILITY : RÈGLE FONDAMENTALE

### 🔒 **Les Props sont READ-ONLY (Lecture seule)**

```jsx
function Counter({ count }) {
  // ❌ INTERDIT! Ne JAMAIS modifier les props
  count = count + 1;  // ❌ ERREUR
  count++;            // ❌ ERREUR
  
  return <p>Count: {count}</p>;
}

// ✅ CORRECT: Utiliser le state pour les valeurs mutables
function Counter({ initialCount }) {
  const [count, setCount] = useState(initialCount);
  
  return (
    <div>
      <p>Count: {count}</p>
      <button onClick={() => setCount(count + 1)}>+</button>
    </div>
  );
}
```

### 🎯 **Pourquoi l'Immutabilité ?**

**3 Raisons principales :**

```jsx
// 1️⃣ PURETÉ DES COMPOSANTS
// Un composant avec les mêmes props doit TOUJOURS 
// retourner le même résultat

function Greeting({ name }) {
  // ✅ Fonction pure: même input = même output
  return <h1>Hello {name}</h1>;
}

// 2️⃣ PRÉDICTIBILITÉ
// React peut optimiser le rendu en comparant les props

// 3️⃣ ÉVITER LES SIDE EFFECTS
// Modifier les props affecterait le parent (bug!)
```

### 🔄 **Props vs State**

| Props | State |
|-------|-------|
| **Propriété EXTERNE** | **Donnée INTERNE** |
| Vient du **parent** | Appartient au **composant** |
| **READ-ONLY** (immutable) | **MUTABLE** (via setState) |
| Comme des **paramètres de fonction** | Comme des **variables locales** |
| Reçu de l'extérieur | Créé dans le composant |
| Pour **configurer** le composant | Pour rendre le composant **interactif** |

```jsx
function Counter({ initialValue, title }) {
  // Props: initialValue, title (READ-ONLY)
  // State: count (MUTABLE)
  const [count, setCount] = useState(initialValue);
  
  return (
    <div>
      <h2>{title}</h2> {/* Prop */}
      <p>{count}</p>    {/* State */}
      <button onClick={() => setCount(count + 1)}>+</button>
    </div>
  );
}

// Usage
<Counter initialValue={0} title="Mon Compteur" />
```

---

## 5️⃣ ONE-WAY DATA FLOW (Flux Unidirectionnel)

### ⬇️ **Le Principe Fondamental**

```
      App (Parent)
         |
    [Props Down]
         ↓
    Child Component
         |
    [Props Down]
         ↓
  Grandchild Component

❌ Les données ne remontent PAS naturellement
✅ Les données descendent uniquement (top → down)
```

### 📊 **Exemple Concret**

```jsx
function App() {
  const [user, setUser] = useState({ name: "Alice", age: 25 });
  
  return (
    <div>
      {/* Données descendent via props */}
      <Header userName={user.name} />
      <Profile user={user} />
      <Footer userName={user.name} />
    </div>
  );
}

function Header({ userName }) {
  // Header reçoit les données mais ne peut pas les modifier
  return <h1>Welcome {userName}</h1>;
}
```

### 🎯 **Avantages du Flux Unidirectionnel**

```jsx
// ✅ 1. PRÉDICTIBILITÉ
// On sait toujours d'où viennent les données

// ✅ 2. DÉBOGAGE FACILE
// Le flux est clair: Parent → Enfant

// ✅ 3. PERFORMANCE
// React peut optimiser le rendu

// ✅ 4. PAS DE CONFUSION
// Pas de données qui se modifient "magiquement"
```

### 🔄 **Child-to-Parent Communication**

> **Question fréquente : "Comment un enfant peut-il communiquer avec son parent ?"**

**Réponse : Via des fonctions callback passées en props !**

```jsx
// 📤 PARENT envoie une fonction
function Parent() {
  const [count, setCount] = useState(0);
  
  // Fonction qui modifie le state du parent
  const incrementCount = () => {
    setCount(c => c + 1);
  };
  
  return (
    <div>
      <p>Count in Parent: {count}</p>
      {/* Passe la fonction à l'enfant */}
      <Child onIncrement={incrementCount} />
    </div>
  );
}

// 📥 ENFANT appelle la fonction
function Child({ onIncrement }) {
  return (
    <button onClick={onIncrement}>
      Increment Parent Count
    </button>
  );
}
```

**Pattern avec données :**
```jsx
function TodoApp() {
  const [todos, setTodos] = useState([]);
  
  // Parent fournit la fonction pour ajouter
  const addTodo = (text) => {
    setTodos([...todos, { id: Date.now(), text }]);
  };
  
  return (
    <div>
      <TodoList todos={todos} />
      {/* Enfant peut ajouter via callback */}
      <AddTodoForm onAddTodo={addTodo} />
    </div>
  );
}

function AddTodoForm({ onAddTodo }) {
  const [text, setText] = useState('');
  
  const handleSubmit = (e) => {
    e.preventDefault();
    onAddTodo(text);  // Appelle la fonction du parent
    setText('');
  };
  
  return (
    <form onSubmit={handleSubmit}>
      <input 
        value={text} 
        onChange={(e) => setText(e.target.value)} 
      />
      <button>Add</button>
    </form>
  );
}
```

**Schéma complet :**
```
Parent (a le state)
   |
   | [Props: données + callbacks ⬇️]
   |
   ↓
Child Component
   |
   | [Appelle callback ⬆️]
   |
   ↓
Parent State Updated (setState)
   |
   | [Nouvelles props ⬇️]
   |
   ↓
Child Re-renders avec nouvelles données
```

---

## 6️⃣ CHILDREN PROP : COMPOSITION

### 👶 **Le Prop Spécial : children**

```jsx
// Le children est tout ce qui est ENTRE les balises
function Card({ children }) {
  return (
    <div className="card">
      {children}
    </div>
  );
}

// Usage
<Card>
  <h2>Title</h2>
  <p>Content</p>
  <button>Click</button>
</Card>

// children reçoit tout ceci: <h2>...</h2> + <p>...</p> + <button>...</button>
```

### 🎨 **Pattern : Composants Génériques**

```jsx
// ✅ COMPOSANT RÉUTILISABLE
function Modal({ isOpen, onClose, children }) {
  if (!isOpen) return null;
  
  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="modal-content" onClick={(e) => e.stopPropagation()}>
        <button className="close" onClick={onClose}>✕</button>
        {children}  {/* Contenu flexible */}
      </div>
    </div>
  );
}

// 🎯 UTILISATION FLEXIBLE
function App() {
  const [isOpen, setIsOpen] = useState(false);
  
  return (
    <>
      {/* Modal avec formulaire */}
      <Modal isOpen={isOpen} onClose={() => setIsOpen(false)}>
        <h2>Login</h2>
        <LoginForm />
      </Modal>
      
      {/* Même Modal, contenu différent */}
      <Modal isOpen={isOpen} onClose={() => setIsOpen(false)}>
        <h2>Confirmation</h2>
        <p>Êtes-vous sûr ?</p>
        <button>Oui</button>
        <button>Non</button>
      </Modal>
    </>
  );
}
```

### 🔧 **Pattern : Layout Components**

```jsx
// Layout réutilisable
function PageLayout({ children }) {
  return (
    <div className="page">
      <Header />
      <main className="content">
        {children}  {/* Contenu de chaque page */}
      </main>
      <Footer />
    </div>
  );
}

// Différentes pages avec le même layout
function HomePage() {
  return (
    <PageLayout>
      <h1>Home</h1>
      <p>Welcome!</p>
    </PageLayout>
  );
}

function AboutPage() {
  return (
    <PageLayout>
      <h1>About</h1>
      <p>About us...</p>
    </PageLayout>
  );
}
```

### 💎 **Children vs Props Explicites**

```jsx
// Option 1: Children (pour contenu principal)
function Button({ children, onClick }) {
  return (
    <button onClick={onClick} className="btn">
      {children}
    </button>
  );
}

<Button onClick={handleClick}>
  Click Me
</Button>


// Option 2: Props explicites (pour configuration)
function Button({ label, icon, onClick }) {
  return (
    <button onClick={onClick} className="btn">
      {icon && <span>{icon}</span>}
      {label}
    </button>
  );
}

<Button 
  label="Click Me" 
  icon="🚀" 
  onClick={handleClick} 
/>
```

**Quand utiliser quoi ?**
- **children** : Contenu principal, flexible, composé d'autres composants
- **props explicites** : Configuration, données simples, APIs claires

---

## 7️⃣ COMPONENT COMPOSITION

### 🧩 **Qu'est-ce que la Composition ?**

> **Composition = Assembler des composants simples pour créer des composants complexes**

**Principe :** Préférer la composition à l'héritage (React n'utilise PAS l'héritage de classes pour les composants)

### 🎯 **Pattern 1 : Composition avec Children**

```jsx
// Composants de base
function Card({ children }) {
  return <div className="card">{children}</div>;
}

function CardHeader({ children }) {
  return <div className="card-header">{children}</div>;
}

function CardBody({ children }) {
  return <div className="card-body">{children}</div>;
}

function CardFooter({ children }) {
  return <div className="card-footer">{children}</div>;
}

// Composition finale
function ProductCard({ product }) {
  return (
    <Card>
      <CardHeader>
        <h3>{product.name}</h3>
      </CardHeader>
      <CardBody>
        <img src={product.image} alt={product.name} />
        <p>{product.description}</p>
      </CardBody>
      <CardFooter>
        <span>{product.price}€</span>
        <button>Acheter</button>
      </CardFooter>
    </Card>
  );
}
```

### 🎨 **Pattern 2 : Composition avec Props**

```jsx
// Au lieu de créer un composant monolithique
function UserDashboard({ user, stats, notifications, settings }) {
  return (
    <div>
      {/* Beaucoup de logique ici... */}
    </div>
  );
}

// ✅ Mieux: Composer avec des composants
function UserDashboard({ user }) {
  return (
    <DashboardLayout
      sidebar={<UserSidebar user={user} />}
      header={<DashboardHeader user={user} />}
      main={<UserStats user={user} />}
      notifications={<NotificationPanel user={user} />}
    />
  );
}

function DashboardLayout({ sidebar, header, main, notifications }) {
  return (
    <div className="dashboard">
      <aside>{sidebar}</aside>
      <div className="main-area">
        <header>{header}</header>
        <main>{main}</main>
      </div>
      <div className="notifications">{notifications}</div>
    </div>
  );
}
```

### 🔄 **Pattern 3 : Éviter le Prop Drilling**

```jsx
// ❌ PROP DRILLING (à éviter)
function App() {
  const [user, setUser] = useState(null);
  
  return <Dashboard user={user} setUser={setUser} />;
}

function Dashboard({ user, setUser }) {
  return <Sidebar user={user} setUser={setUser} />;
}

function Sidebar({ user, setUser }) {
  return <UserProfile user={user} setUser={setUser} />;
}

function UserProfile({ user, setUser }) {
  // Enfin on utilise user et setUser ici!
  return <div>{user.name}</div>;
}


// ✅ SOLUTION 1: Composition (passer le composant directement)
function App() {
  const [user, setUser] = useState(null);
  
  return (
    <Dashboard>
      <Sidebar>
        <UserProfile user={user} setUser={setUser} />
      </Sidebar>
    </Dashboard>
  );
}

function Dashboard({ children }) {
  return <div className="dashboard">{children}</div>;
}

function Sidebar({ children }) {
  return <aside>{children}</aside>;
}


// ✅ SOLUTION 2: Context API (chapitre 4)
const UserContext = createContext();

function App() {
  const [user, setUser] = useState(null);
  
  return (
    <UserContext.Provider value={{ user, setUser }}>
      <Dashboard />
    </UserContext.Provider>
  );
}

function UserProfile() {
  const { user, setUser } = useContext(UserContext);
  return <div>{user.name}</div>;
}
```

---

## 8️⃣ DÉCOUPER UNE UI EN COMPOSANTS

### 📐 **Les 4 Critères de Découpage**

```
1️⃣ Séparation Logique du Contenu/Layout
2️⃣ Réutilisabilité
3️⃣ Responsabilités/Complexité
4️⃣ Style Personnel de Code
```

### 🎯 **Méthodologie de Découpage**

```jsx
// 🎨 EXEMPLE: Page E-commerce

// ❌ UN SEUL GROS COMPOSANT (mauvais)
function ProductPage() {
  return (
    <div>
      {/* Header avec navigation, logo, cart, search... */}
      {/* Filtres avec catégories, prix, rating... */}
      {/* Liste de produits avec images, prix, descriptions... */}
      {/* Pagination... */}
      {/* Footer... */}
      {/* 500 lignes de code! */}
    </div>
  );
}


// ✅ COMPOSANTS DÉCOUPÉS (bon)
function ProductPage() {
  return (
    <PageLayout>
      <Header />
      <ProductFilters />
      <ProductList />
      <Pagination />
    </PageLayout>
  );
}

// Chaque composant peut être lui-même découpé
function ProductList() {
  return (
    <div className="product-list">
      {products.map(product => (
        <ProductCard key={product.id} product={product} />
      ))}
    </div>
  );
}

function ProductCard({ product }) {
  return (
    <Card>
      <ProductImage src={product.image} />
      <ProductInfo product={product} />
      <ProductActions product={product} />
    </Card>
  );
}
```

### 📋 **Guide de Décision**

```jsx
// 🤔 DOIS-JE CRÉER UN NOUVEAU COMPOSANT ?

// ✅ OUI si:
// - Le code dépasse 100-150 lignes
// - Une partie se répète (copier-coller)
// - Trop de responsabilités différentes
// - Trop de props (> 5-7)
// - Trop de state variables (> 3-4)
// - Logique complexe difficile à suivre
// - Besoin de réutiliser ailleurs

// ❌ NON si:
// - Le composant est déjà petit et clair
// - Pas de réutilisation prévue
// - Ajouterait de la complexité inutile
// - Trop d'abstractions (overhead mental)
```

### 🏗️ **Stratégie Progressive**

```jsx
// 💡 STRATÉGIE: Commencer GRAND, puis DÉCOUPER

// 1️⃣ Commencer avec un composant
function TodoApp() {
  const [todos, setTodos] = useState([]);
  const [filter, setFilter] = useState('all');
  // ... logique ici ...
  
  return (
    <div>
      {/* Tout le JSX */}
    </div>
  );
}


// 2️⃣ Quand ça devient complexe, extraire
function TodoApp() {
  const [todos, setTodos] = useState([]);
  const [filter, setFilter] = useState('all');
  
  return (
    <div>
      <TodoHeader />
      <AddTodoForm onAdd={handleAdd} />
      <TodoList todos={filteredTodos} onToggle={handleToggle} />
      <TodoFilters filter={filter} onFilterChange={setFilter} />
    </div>
  );
}


// 3️⃣ Continuer le découpage si nécessaire
function TodoList({ todos, onToggle }) {
  return (
    <ul>
      {todos.map(todo => (
        <TodoItem key={todo.id} todo={todo} onToggle={onToggle} />
      ))}
    </ul>
  );
}
```

### 🎨 **Catégories de Composants**

```jsx
// 1️⃣ STATELESS / PRESENTATIONAL (Sans état)
// - Reçoivent des props
// - Affichent des données
// - Pas de logique complexe
function ProductCard({ name, price, image }) {
  return (
    <div className="card">
      <img src={image} alt={name} />
      <h3>{name}</h3>
      <p>{price}€</p>
    </div>
  );
}


// 2️⃣ STATEFUL (Avec état)
// - Gèrent du state
// - Contiennent de la logique
// - Peuvent être réutilisables
function SearchBar() {
  const [query, setQuery] = useState('');
  const [results, setResults] = useState([]);
  
  useEffect(() => {
    // Logique de recherche
  }, [query]);
  
  return (
    <div>
      <input value={query} onChange={e => setQuery(e.target.value)} />
      <SearchResults results={results} />
    </div>
  );
}


// 3️⃣ STRUCTURAL (Structure/Layout)
// - Organisent la mise en page
// - Composent d'autres composants
// - Souvent non-réutilisables
function DashboardPage() {
  return (
    <PageLayout>
      <DashboardHeader />
      <StatsGrid>
        <StatCard title="Users" value={1500} />
        <StatCard title="Revenue" value="$50k" />
        <StatCard title="Orders" value={325} />
      </StatsGrid>
      <RecentActivity />
    </PageLayout>
  );
}
```

---

## 9️⃣ PROPS AS A COMPONENT API

### 🎛️ **Les Props comme Interface Publique**

```jsx
// Les props définissent comment on UTILISE le composant
// C'est l'API du composant!

function Button({
  // Props obligatoires
  children,
  onClick,
  
  // Props optionnelles avec défauts
  variant = 'primary',    // 'primary' | 'secondary' | 'danger'
  size = 'medium',        // 'small' | 'medium' | 'large'
  disabled = false,
  loading = false,
  icon = null,
  fullWidth = false
}) {
  return (
    <button
      onClick={onClick}
      disabled={disabled || loading}
      className={`btn btn-${variant} btn-${size} ${fullWidth ? 'full-width' : ''}`}
    >
      {loading && <Spinner />}
      {icon && <span className="icon">{icon}</span>}
      {children}
    </button>
  );
}

// Usage: API claire et flexible
<Button onClick={handleClick}>
  Click me
</Button>

<Button onClick={handleClick} variant="danger" icon="🗑️">
  Delete
</Button>

<Button onClick={handleSubmit} loading={isSubmitting} fullWidth>
  Submit Form
</Button>
```

### ⚖️ **Balance : Trop peu vs Trop de Props**

```jsx
// ❌ TROP PEU DE PROPS (pas flexible)
function Button({ children }) {
  return (
    <button className="btn-primary" onClick={() => alert('clicked')}>
      {children}
    </button>
  );
}
// Problème: Impossible de personnaliser l'action, le style, etc.


// ❌ TROP DE PROPS (trop complexe)
function Button({
  children,
  onClick,
  variant,
  size,
  disabled,
  loading,
  icon,
  iconPosition,
  fullWidth,
  outlined,
  rounded,
  shadow,
  gradient,
  animation,
  tooltip,
  ariaLabel,
  tabIndex,
  dataTestId,
  // ... 20 autres props
}) {
  // Complexité extrême!
}
// Problème: Trop de choix, difficile à utiliser et maintenir


// ✅ BON ÉQUILIBRE
function Button({
  children,
  onClick,
  variant = 'primary',     // Options limitées mais suffisantes
  size = 'medium',
  disabled = false,
  loading = false,
  fullWidth = false,
  className = ''           // Escape hatch pour cas spéciaux
}) {
  return (
    <button
      onClick={onClick}
      disabled={disabled || loading}
      className={`btn btn-${variant} btn-${size} ${fullWidth ? 'full-width' : ''} ${className}`}
    >
      {loading && <Spinner />}
      {children}
    </button>
  );
}
```

### 🎨 **Patterns de Props Avancés**

#### **1. Render Props Pattern**
```jsx
// Le composant reçoit une fonction qui retourne du JSX
function DataFetcher({ url, render, renderLoading, renderError }) {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  
  useEffect(() => {
    fetch(url)
      .then(res => res.json())
      .then(setData)
      .catch(setError)
      .finally(() => setLoading(false));
  }, [url]);
  
  if (loading) return renderLoading();
  if (error) return renderError(error);
  return render(data);
}

// Usage flexible
<DataFetcher
  url="/api/users"
  renderLoading={() => <Spinner />}
  renderError={(error) => <ErrorMessage error={error} />}
  render={(users) => (
    <UserList users={users} />
  )}
/>
```

#### **2. Compound Components Pattern**
```jsx
// Les composants travaillent ensemble
function Select({ children, value, onChange }) {
  return (
    <div className="select">
      {children}
    </div>
  );
}

Select.Option = function Option({ value, children }) {
  return <option value={value}>{children}</option>;
};

// Usage élégant et lisible
<Select value={selectedValue} onChange={handleChange}>
  <Select.Option value="1">Option 1</Select.Option>
  <Select.Option value="2">Option 2</Select.Option>
  <Select.Option value="3">Option 3</Select.Option>
</Select>
```

#### **3. Polymorphic Component Pattern**
```jsx
// Composant qui peut être rendu comme différents éléments HTML
function Text({ as: Component = 'p', children, ...props }) {
  return <Component {...props}>{children}</Component>;
}

// Usage flexible
<Text>Paragraph par défaut</Text>
<Text as="h1">Titre principal</Text>
<Text as="span">Inline text</Text>
<Text as={Link} to="/home">Link component</Text>
```

---

## 🔟 COMPONENT TREE (Arbre de Composants)

### 🌳 **Visualisation**

```jsx
// STRUCTURE HIÉRARCHIQUE
App
├── Header
│   ├── Logo
│   ├── Navigation
│   │   ├── NavItem
│   │   ├── NavItem
│   │   └── NavItem
│   └── UserMenu
│       ├── Avatar
│       └── Dropdown
├── Main
│   ├── Sidebar
│   │   ├── FilterPanel
│   │   └── CategoryList
│   └── Content
│       ├── ProductGrid
│       │   ├── ProductCard
│       │   ├── ProductCard
│       │   └── ProductCard
│       └── Pagination
└── Footer
    ├── FooterLinks
    └── Copyright
```

### 📊 **Relations Parent-Enfant**

```jsx
function App() {
  const [cart, setCart] = useState([]);
  
  return (
    <div>
      {/* App est le PARENT */}
      <Header cartCount={cart.length} />
      <ProductList onAddToCart={(item) => setCart([...cart, item])} />
      <Cart items={cart} />
      {/* Header, ProductList, Cart sont les ENFANTS */}
    </div>
  );
}

// Relations:
// - App → Header (parent-enfant)
// - App → ProductList (parent-enfant)
// - App → Cart (parent-enfant)
// - Header ↔ ProductList (siblings/frères)
// - ProductList ↔ Cart (siblings/frères)
```

### 🔄 **Communication dans l'Arbre**

```jsx
// 1️⃣ Parent → Enfant (Props Down)
function Parent() {
  const data = "Hello";
  return <Child message={data} />;  // ⬇️
}

// 2️⃣ Enfant → Parent (Callbacks Up)
function Parent() {
  const handleData = (data) => console.log(data);
  return <Child onData={handleData} />;  // ⬆️ (via callback)
}

function Child({ onData }) {
  return <button onClick={() => onData("Hi!")}>Send</button>;
}

// 3️⃣ Sibling → Sibling (Via Parent)
function Parent() {
  const [data, setData] = useState("");
  
  return (
    <>
      <Child1 onData={setData} />      // Child1 envoie
      <Child2 data={data} />           // Child2 reçoit
    </>
  );
}

// 4️⃣ N'importe où → N'importe où (Context API)
// Voir chapitre 4
```

---

## 🎯 QUESTIONS D'ENTRETIEN FRÉQUENTES

### Q1: **Quelle est la différence entre props et state ?**
**Réponse complète :**
```jsx
// PROPS
// - Données EXTERNES venant du parent
// - READ-ONLY (immutables)
// - Comme des paramètres de fonction
// - Pour configurer un composant
function Child({ name }) {  // name est un prop
  // ❌ name = "autre"  // INTERDIT!
  return <h1>{name}</h1>;
}

// STATE
// - Données INTERNES au composant
// - MUTABLES (via setState)
// - Mémoire du composant
// - Pour rendre le composant interactif
function Counter() {
  const [count, setCount] = useState(0);  // state
  // ✅ setCount(1)  // OK!
  return <p>{count}</p>;
}
```

### Q2: **Pourquoi les props sont-elles immutables ?**
**Réponse :**
1. **Pureté** : Même props = même résultat (fonctions pures)
2. **Prédictibilité** : Pas de side effects
3. **Performance** : React peut optimiser (shallow comparison)
4. **Débogage** : Flux de données clair et traceable

```jsx
// ❌ Si on pouvait modifier les props
function Child({ user }) {
  user.name = "Modified";  // Affecterait le parent! 🐛
}

// ✅ Utiliser le state si besoin de modification
function Child({ initialUser }) {
  const [user, setUser] = useState(initialUser);
  const updateName = () => setUser({ ...user, name: "New" });
}
```

### Q3: **Qu'est-ce que le "prop drilling" et comment l'éviter ?**
**Réponse :**
```jsx
// ❌ PROP DRILLING (problème)
function App() {
  const [user, setUser] = useState(null);
  return <Level1 user={user} setUser={setUser} />;
}

function Level1({ user, setUser }) {
  return <Level2 user={user} setUser={setUser} />;  // Just passing down
}

function Level2({ user, setUser }) {
  return <Level3 user={user} setUser={setUser} />;  // Just passing down
}

function Level3({ user, setUser }) {
  return <div>{user.name}</div>;  // Finally used here!
}

// ✅ SOLUTIONS:

// Solution 1: Component Composition
function App() {
  const [user, setUser] = useState(null);
  return (
    <Level1>
      <Level2>
        <Level3 user={user} setUser={setUser} />
      </Level2>
    </Level1>
  );
}

// Solution 2: Context API (Chapitre 4)
const UserContext = createContext();

function App() {
  const [user, setUser] = useState(null);
  return (
    <UserContext.Provider value={{ user, setUser }}>
      <Level1 />
    </UserContext.Provider>
  );
}

function Level3() {
  const { user, setUser } = useContext(UserContext);
  return <div>{user.name}</div>;
}
```

### Q4: **Expliquez le flux de données unidirectionnel (one-way data flow)**
**Réponse :**
```
Parent Component (source de vérité)
    |
    | Props ⬇️ (données descendent)
    |
    ↓
Child Component
    |
    | Callback ⬆️ (événements remontent)
    |
    ↓
Parent State Update
    |
    | New Props ⬇️
    |
    ↓
Child Re-render

Avantages:
✅ Prédictible et facile à déboguer
✅ Pas de données qui changent "magiquement"
✅ Single source of truth
✅ Meilleure performance
```

### Q5: **Quelle est la différence entre children et props classiques ?**
**Réponse :**
```jsx
// PROPS CLASSIQUES
<Button text="Click" icon="🚀" />

function Button({ text, icon }) {
  return <button>{icon} {text}</button>;
}

// CHILDREN (prop spécial)
<Button>
  <span>🚀</span>
  <span>Click me</span>
</Button>

function Button({ children }) {
  return <button>{children}</button>;
}

// QUAND UTILISER QUOI?
// - children: Contenu complexe, composition, flexibilité
// - props: Configuration, données simples, API claire
```

### Q6: **Comment décidez-vous quand créer un nouveau composant ?**
**Réponse basée sur les 4 critères :**
```
1. SÉPARATION LOGIQUE
   - Sections distinctes de l'UI
   - Préoccupations différentes

2. RÉUTILISABILITÉ
   - Code dupliqué → extraire en composant
   - Besoin dans plusieurs endroits

3. COMPLEXITÉ
   - > 150 lignes de code
   - > 5-7 props
   - > 3-4 state variables
   - Logique difficile à suivre

4. STYLE PERSONNEL
   - Préférence pour petits composants
   - Balance lisibilité vs nombre de fichiers

💡 Stratégie: Commencer avec un gros composant,
   puis découper quand la complexité augmente
```

### Q7: **Function Components vs Class Components : différences et pourquoi préférer les Function ?**
**Réponse :**
```jsx
// CLASS COMPONENTS (ancien)
class Counter extends React.Component {
  constructor(props) {
    super(props);
    this.state = { count: 0 };
  }
  
  componentDidMount() {
    // Lifecycle
  }
  
  render() {
    return <div>{this.state.count}</div>;
  }
}

// FUNCTION COMPONENTS (moderne)
function Counter() {
  const [count, setCount] = useState(0);
  
  useEffect(() => {
    // Lifecycle
  }, []);
  
  return <div>{count}</div>;
}

POURQUOI FUNCTION COMPONENTS?
✅ Syntaxe plus simple et moins de code
✅ Pas de confusion avec 'this'
✅ Hooks (useState, useEffect, custom hooks)
✅ Meilleure réutilisation de logique
✅ Performance légèrement meilleure
✅ Direction future de React
```

### Q8: **Qu'est-ce que la composition de composants et pourquoi est-elle importante ?**
**Réponse :**
```jsx
// COMPOSITION = Assembler des composants simples
// pour créer des composants complexes

// Au lieu d'héritage (pas utilisé en React):
class Button extends BaseButton { }  // ❌ Pas en React

// On utilise la composition:
function Card({ children }) {
  return (
    <div className="card">
      {children}
    </div>
  );
}

function UserCard({ user }) {
  return (
    <Card>
      <Avatar src={user.avatar} />
      <UserInfo name={user.name} email={user.email} />
      <UserActions userId={user.id} />
    </Card>
  );
}

AVANTAGES:
✅ Flexibilité maximale
✅ Réutilisabilité
✅ Évite le prop drilling
✅ Séparation des responsabilités
✅ Code plus maintenable
```

---

## ⚠️ PIÈGES COURANTS

### 1. **Modifier les Props**
```jsx
// ❌ ERREUR FRÉQUENTE
function Product({ product }) {
  product.price = product.price * 0.9;  // ❌ MUTATION!
  return <div>{product.price}€</div>;
}

// ✅ CORRECT
function Product({ product }) {
  const discountedPrice = product.price * 0.9;  // ✅ Nouveau calcul
  return <div>{discountedPrice}€</div>;
}
```

### 2. **Déclarer un Composant dans un Autre**
```jsx
// ❌ TRÈS MAUVAIS (re-crée le composant à chaque render)
function Parent() {
  // ❌ Child est redéfini à chaque render de Parent
  function Child() {
    return <div>Child</div>;
  }
  
  return <Child />;
}

// ✅ CORRECT (définir en dehors)
function Child() {
  return <div>Child</div>;
}

function Parent() {
  return <Child />;
}
```

### 3. **Oublier la Key dans les Listes**
```jsx
// ❌ Sans key (mauvaise performance)
{items.map(item => (
  <ItemCard item={item} />  // ❌ Warning!
))}

// ✅ Avec key
{items.map(item => (
  <ItemCard key={item.id} item={item} />  // ✅
))}

// ⚠️ Ne pas utiliser l'index comme key si la liste peut changer
{items.map((item, index) => (
  <ItemCard key={index} item={item} />  // ⚠️ Problématique
))}
```

### 4. **Passer des Objets/Tableaux Créés dans le Render**
```jsx
// ❌ PROBLÈME (nouvel objet à chaque render)
function Parent() {
  return (
    <Child 
      user={{ name: "Alice", age: 25 }}  // ❌ Nouvel objet
      items={[1, 2, 3]}                  // ❌ Nouveau tableau
    />
  );
}

// ✅ SOLUTIONS
function Parent() {
  // Solution 1: Définir en dehors
  const user = { name: "Alice", age: 25 };
  const items = [1, 2, 3];
  
  return <Child user={user} items={items} />;
}

// Solution 2: useMemo si calculé
function Parent() {
  const user = useMemo(
    () => ({ name: "Alice", age: 25 }), 
    []
  );
  
  return <Child user={user} />;
}
```

### 5. **Destructuration Incorrecte**
```jsx
// ❌ Destructuration dans le mauvais scope
function Component(props) {
  const name = props.name;  // ⚠️ Répétitif
  const age = props.age;
  const email = props.email;
  // ...
}

// ✅ Destructuration directe dans les paramètres
function Component({ name, age, email }) {
  // Directement accessible
}

// ✅ Ou destructuration avec rest
function Component({ name, ...otherProps }) {
  return <Child name={name} {...otherProps} />;
}
```

### 6. **Confondre Props et State**
```jsx
// ❌ Utiliser props pour des données qui doivent changer
function Counter({ count }) {
  // ❌ count ne peut pas changer!
  return (
    <button onClick={() => count++}>  {/* ❌ Ne marche pas */}
      {count}
    </button>
  );
}

// ✅ Utiliser state pour les données mutables
function Counter({ initialCount }) {
  const [count, setCount] = useState(initialCount);
  
  return (
    <button onClick={() => setCount(c => c + 1)}>
      {count}
    </button>
  );
}
```

---

## 🔗 LIENS AVEC VOS PROJETS

### **05-travel-list**
```jsx
// Exemple de composition et props
function App() {
  const [items, setItems] = useState([]);
  
  return (
    <div>
      <Logo />
      <Form onAddItems={handleAddItems} />
      <PackingList 
        items={items} 
        onDeleteItem={handleDeleteItem}
        onToggleItem={handleToggleItem}
      />
      <Stats items={items} />
    </div>
  );
}

// Props descendants: App → PackingList → Item
// Callbacks remontants: Item → PackingList → App
```

### **06-eat-n-split**
```jsx
// Exemple de lifting state et composition
function App() {
  const [friends, setFriends] = useState([]);
  const [selectedFriend, setSelectedFriend] = useState(null);
  
  return (
    <div>
      <Sidebar>
        <FriendsList 
          friends={friends}
          onSelection={setSelectedFriend}
        />
        <FormAddFriend onAddFriend={handleAddFriend} />
      </Sidebar>
      
      {selectedFriend && (
        <FormSplitBill 
          friend={selectedFriend}
          onSplitBill={handleSplitBill}
        />
      )}
    </div>
  );
}

// État partagé levé au niveau de App
// Communication siblings via parent
```

### **07-usepopcorn**
```jsx
// Composition avec children
function MovieDetails({ movieId, children }) {
  return (
    <div className="details">
      <MovieHeader movieId={movieId} />
      <MovieContent movieId={movieId} />
      {children}  {/* Zone flexible */}
    </div>
  );
}

// Usage
<MovieDetails movieId={id}>
  <AddToWatchedButton />
</MovieDetails>
```

### **11-worldwise**
```jsx
// Component tree complexe
App
├── Homepage
├── AppLayout
│   ├── Sidebar
│   │   ├── Logo
│   │   ├── AppNav
│   │   ├── CityList (props: cities)
│   │   └── Footer
│   └── Map
│       └── CityMarkers
└── Login

// Props drilling évité avec Context API
```

---

## 📝 RÉSUMÉ DU CHAPITRE

✅ **Components** = Fonctions retournant JSX (Données + Logique + Apparence)  
✅ **Function Components** > Class Components (moderne, hooks, simplicité)  
✅ **Props** = Arguments pour configurer les composants (READ-ONLY)  
✅ **Children prop** = Contenu flexible entre les balises du composant  
✅ **One-way data flow** = Données descendent (props), événements remontent (callbacks)  
✅ **Composition** = Assembler des composants simples pour créer des complexes  
✅ **Découpage** = 4 critères (logique, réutilisabilité, complexité, style)  
✅ **Props as API** = Interface publique du composant (balance peu vs trop)  

---

## 🎓 POINTS CLÉS

1. **Maîtrisez la différence props vs state** (question garantie !)
2. **Expliquez le one-way data flow** avec un exemple concret
3. **Connaissez les patterns de composition** (children, render props, compound)
4. **Sachez découper une UI** en argumentant vos choix
5. **Mentionnez vos projets** : travel-list, eat-n-split pour illustrer
6. **Évitez les pièges** : mutation props, composants imbriqués, key manquante

---