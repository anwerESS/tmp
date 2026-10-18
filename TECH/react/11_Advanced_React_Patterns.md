## [**..**](./00_index.md)

# CHAPITRE 11 : Patterns React Avancés - Deep Dive Complet

## 🎯 Introduction : Pourquoi les Patterns Avancés ?

Les patterns avancés permettent de :
- ✅ **Réutiliser la logique** sans duplication
- ✅ **Composer des composants** de manière flexible
- ✅ **Inverser le contrôle** (Inversion of Control)
- ✅ **Abstraire la complexité** 
- ✅ **Créer des APIs élégantes**

---

## 🔄 Pattern 1 : Render Props

### 1. Comprendre le Problème

```javascript
// ❌ PROBLÈME : Duplication de logique
function MouseTrackerA() {
  const [position, setPosition] = useState({ x: 0, y: 0 });
  
  useEffect(() => {
    const handleMove = (e) => {
      setPosition({ x: e.clientX, y: e.clientY });
    };
    
    window.addEventListener('mousemove', handleMove);
    return () => window.removeEventListener('mousemove', handleMove);
  }, []);
  
  return (
    <div>
      <h1>Mouse Position A</h1>
      <p>X: {position.x}, Y: {position.y}</p>
    </div>
  );
}

function MouseTrackerB() {
  // ❌ MÊME logique dupliquée !
  const [position, setPosition] = useState({ x: 0, y: 0 });
  
  useEffect(() => {
    const handleMove = (e) => {
      setPosition({ x: e.clientX, y: e.clientY });
    };
    
    window.addEventListener('mousemove', handleMove);
    return () => window.removeEventListener('mousemove', handleMove);
  }, []);
  
  return (
    <div>
      <h2>Mouse Position B</h2>
      <p>Coordinates: ({position.x}, {position.y})</p>
    </div>
  );
}

// Problème : Logique de tracking dupliquée dans chaque composant !
```

### 2. Solution : Render Props Pattern

```javascript
// ✅ SOLUTION : Extraire la logique dans un composant avec render prop
function MouseTracker({ render }) {
  const [position, setPosition] = useState({ x: 0, y: 0 });
  
  useEffect(() => {
    const handleMove = (e) => {
      setPosition({ x: e.clientX, y: e.clientY });
    };
    
    window.addEventListener('mousemove', handleMove);
    return () => window.removeEventListener('mousemove', handleMove);
  }, []);
  
  // Appeler render prop avec la position
  return render(position);
}

// Usage : Différentes UIs avec même logique
function App() {
  return (
    <div>
      {/* UI Version 1 */}
      <MouseTracker
        render={(position) => (
          <div>
            <h1>Mouse Position</h1>
            <p>X: {position.x}, Y: {position.y}</p>
          </div>
        )}
      />
      
      {/* UI Version 2 */}
      <MouseTracker
        render={(position) => (
          <div>
            <h2>Coordinates</h2>
            <p>({position.x}, {position.y})</p>
          </div>
        )}
      />
      
      {/* UI Version 3 : Curseur custom */}
      <MouseTracker
        render={(position) => (
          <div
            style={{
              position: 'fixed',
              left: position.x,
              top: position.y,
              width: 20,
              height: 20,
              borderRadius: '50%',
              backgroundColor: 'red',
              pointerEvents: 'none',
            }}
          />
        )}
      />
    </div>
  );
}

// ✅ Logique centralisée, UI flexible !
```

### 3. Children as Function (Variante)

```javascript
// Au lieu de prop "render", utiliser "children"
function MouseTracker({ children }) {
  const [position, setPosition] = useState({ x: 0, y: 0 });
  
  useEffect(() => {
    const handleMove = (e) => {
      setPosition({ x: e.clientX, y: e.clientY });
    };
    
    window.addEventListener('mousemove', handleMove);
    return () => window.removeEventListener('mousemove', handleMove);
  }, []);
  
  // children est une fonction !
  return children(position);
}

// Usage plus élégant
function App() {
  return (
    <MouseTracker>
      {(position) => (
        <div>
          <h1>Mouse: {position.x}, {position.y}</h1>
        </div>
      )}
    </MouseTracker>
  );
}
```

### 4. Exemples Pratiques Avancés

```javascript
// Exemple 1 : Data Fetcher avec Render Props
function DataFetcher({ url, render }) {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  
  useEffect(() => {
    setLoading(true);
    fetch(url)
      .then(res => res.json())
      .then(setData)
      .catch(setError)
      .finally(() => setLoading(false));
  }, [url]);
  
  return render({ data, loading, error });
}

// Usage
function UserProfile({ userId }) {
  return (
    <DataFetcher
      url={`/api/users/${userId}`}
      render={({ data, loading, error }) => {
        if (loading) return <Spinner />;
        if (error) return <Error message={error.message} />;
        return <UserCard user={data} />;
      }}
    />
  );
}

// Exemple 2 : Toggle avec Render Props
function Toggle({ defaultOn = false, render }) {
  const [on, setOn] = useState(defaultOn);
  
  const toggle = () => setOn(prev => !prev);
  const setOff = () => setOn(false);
  const setOnState = () => setOn(true);
  
  return render({ on, toggle, setOff, setOn: setOnState });
}

// Usage : Modal
function ModalExample() {
  return (
    <Toggle
      render={({ on, toggle, setOff }) => (
        <div>
          <button onClick={toggle}>Open Modal</button>
          {on && (
            <Modal onClose={setOff}>
              <h2>Modal Content</h2>
            </Modal>
          )}
        </div>
      )}
    />
  );
}

// Usage : Dropdown
function DropdownExample() {
  return (
    <Toggle
      render={({ on, toggle }) => (
        <div>
          <button onClick={toggle}>Menu</button>
          {on && (
            <ul>
              <li>Option 1</li>
              <li>Option 2</li>
              <li>Option 3</li>
            </ul>
          )}
        </div>
      )}
    />
  );
}

// Exemple 3 : Window Size Tracker
function WindowSize({ render }) {
  const [size, setSize] = useState({
    width: window.innerWidth,
    height: window.innerHeight,
  });
  
  useEffect(() => {
    const handleResize = () => {
      setSize({
        width: window.innerWidth,
        height: window.innerHeight,
      });
    };
    
    window.addEventListener('resize', handleResize);
    return () => window.removeEventListener('resize', handleResize);
  }, []);
  
  return render(size);
}

// Usage : Responsive component
function ResponsiveLayout() {
  return (
    <WindowSize
      render={({ width }) => (
        <div>
          {width > 768 ? (
            <DesktopLayout />
          ) : (
            <MobileLayout />
          )}
        </div>
      )}
    />
  );
}
```

### 5. Avantages et Inconvénients

```javascript
// ✅ AVANTAGES :
const advantages = {
  flexibility: 'UI complètement flexible',
  reusability: 'Logique réutilisable facilement',
  explicitness: 'Data flow explicite et clair',
  composition: 'Facile à composer',
};

// ❌ INCONVÉNIENTS :
const disadvantages = {
  verbosity: 'Syntaxe verbeuse (fonctions imbriquées)',
  callback_hell: 'Peut devenir complexe avec multiple nesting',
  performance: 'Nouvelle fonction à chaque render (mineur)',
};

// 🎯 Quand utiliser Render Props :
const useCases = {
  // 1. Logique réutilisable avec UI variable
  mouseTracking: true,
  dataFetching: true,
  
  // 2. Partager state entre composants
  toggleState: true,
  formState: true,
  
  // 3. Abstraire interactions complexes
  dragAndDrop: true,
  resizable: true,
};

// ⚠️ Note : Custom Hooks souvent préférés aujourd'hui
// Render Props toujours utile pour certains cas spécifiques
```

---

## 🎁 Pattern 2 : Compound Components

### 1. Comprendre le Pattern

```javascript
// ❌ PROBLÈME : API rigide avec props multiples
function Select({ 
  options, 
  value, 
  onChange, 
  placeholder,
  disabled,
  multiple,
  searchable,
  clearable,
  // ... 50 autres props ! 😱
}) {
  // Configuration nightmare
  return <div>Complex select implementation</div>;
}

// Usage difficile et peu flexible
<Select
  options={[
    { value: 1, label: 'Option 1' },
    { value: 2, label: 'Option 2' },
  ]}
  value={selected}
  onChange={setSelected}
  placeholder="Select an option"
  disabled={false}
  multiple={false}
  searchable={true}
  clearable={true}
/>
```

### 2. Solution : Compound Components

```javascript
// ✅ SOLUTION : Composants composables avec Context
const SelectContext = createContext();

function Select({ children, value, onChange }) {
  const [isOpen, setIsOpen] = useState(false);
  
  return (
    <SelectContext.Provider value={{ value, onChange, isOpen, setIsOpen }}>
      <div className="select">{children}</div>
    </SelectContext.Provider>
  );
}

function SelectTrigger({ children }) {
  const { isOpen, setIsOpen } = useContext(SelectContext);
  
  return (
    <button
      onClick={() => setIsOpen(!isOpen)}
      className="select-trigger"
    >
      {children}
    </button>
  );
}

function SelectContent({ children }) {
  const { isOpen } = useContext(SelectContext);
  
  if (!isOpen) return null;
  
  return <div className="select-content">{children}</div>;
}

function SelectOption({ value: optionValue, children }) {
  const { value, onChange, setIsOpen } = useContext(SelectContext);
  const isSelected = value === optionValue;
  
  return (
    <div
      className={`select-option ${isSelected ? 'selected' : ''}`}
      onClick={() => {
        onChange(optionValue);
        setIsOpen(false);
      }}
    >
      {children}
    </div>
  );
}

// Attacher sub-components
Select.Trigger = SelectTrigger;
Select.Content = SelectContent;
Select.Option = SelectOption;

// Usage : API élégante et flexible !
function MyForm() {
  const [country, setCountry] = useState('');
  
  return (
    <Select value={country} onChange={setCountry}>
      <Select.Trigger>
        {country || 'Select a country'}
      </Select.Trigger>
      
      <Select.Content>
        <Select.Option value="fr">🇫🇷 France</Select.Option>
        <Select.Option value="us">🇺🇸 United States</Select.Option>
        <Select.Option value="uk">🇬🇧 United Kingdom</Select.Option>
      </Select.Content>
    </Select>
  );
}
```

### 3. Exemple Complexe : Tabs Component

```javascript
// Création d'un système de Tabs flexible
const TabsContext = createContext();

function Tabs({ children, defaultValue }) {
  const [activeTab, setActiveTab] = useState(defaultValue);
  
  return (
    <TabsContext.Provider value={{ activeTab, setActiveTab }}>
      <div className="tabs">{children}</div>
    </TabsContext.Provider>
  );
}

function TabsList({ children }) {
  return <div className="tabs-list" role="tablist">{children}</div>;
}

function TabsTrigger({ value, children }) {
  const { activeTab, setActiveTab } = useContext(TabsContext);
  const isActive = activeTab === value;
  
  return (
    <button
      className={`tabs-trigger ${isActive ? 'active' : ''}`}
      onClick={() => setActiveTab(value)}
      role="tab"
      aria-selected={isActive}
    >
      {children}
    </button>
  );
}

function TabsContent({ value, children }) {
  const { activeTab } = useContext(TabsContext);
  
  if (activeTab !== value) return null;
  
  return (
    <div className="tabs-content" role="tabpanel">
      {children}
    </div>
  );
}

// Attacher sub-components
Tabs.List = TabsList;
Tabs.Trigger = TabsTrigger;
Tabs.Content = TabsContent;

// Usage : Extrêmement flexible !
function Dashboard() {
  return (
    <Tabs defaultValue="overview">
      <Tabs.List>
        <Tabs.Trigger value="overview">Overview</Tabs.Trigger>
        <Tabs.Trigger value="analytics">Analytics</Tabs.Trigger>
        <Tabs.Trigger value="reports">Reports</Tabs.Trigger>
      </Tabs.List>
      
      <Tabs.Content value="overview">
        <OverviewPanel />
      </Tabs.Content>
      
      <Tabs.Content value="analytics">
        <AnalyticsPanel />
      </Tabs.Content>
      
      <Tabs.Content value="reports">
        <ReportsPanel />
      </Tabs.Content>
    </Tabs>
  );
}

// Facile de customizer !
function CustomTabs() {
  return (
    <Tabs defaultValue="tab1">
      {/* Custom layout */}
      <div className="custom-header">
        <Logo />
        <Tabs.List>
          <Tabs.Trigger value="tab1">Tab 1</Tabs.Trigger>
          <Tabs.Trigger value="tab2">Tab 2</Tabs.Trigger>
        </Tabs.List>
        <UserMenu />
      </div>
      
      <Tabs.Content value="tab1">Content 1</Tabs.Content>
      <Tabs.Content value="tab2">Content 2</Tabs.Content>
    </Tabs>
  );
}
```

### 4. Exemple Réel : Accordion Component

```javascript
const AccordionContext = createContext();
const AccordionItemContext = createContext();

function Accordion({ children, type = 'single', defaultValue }) {
  const [openItems, setOpenItems] = useState(
    defaultValue ? [defaultValue] : []
  );
  
  const toggleItem = (value) => {
    if (type === 'single') {
      // Single mode : un seul item ouvert
      setOpenItems(prev => 
        prev.includes(value) ? [] : [value]
      );
    } else {
      // Multiple mode : plusieurs items ouverts
      setOpenItems(prev =>
        prev.includes(value)
          ? prev.filter(item => item !== value)
          : [...prev, value]
      );
    }
  };
  
  return (
    <AccordionContext.Provider value={{ openItems, toggleItem }}>
      <div className="accordion">{children}</div>
    </AccordionContext.Provider>
  );
}

function AccordionItem({ value, children }) {
  const { openItems } = useContext(AccordionContext);
  const isOpen = openItems.includes(value);
  
  return (
    <AccordionItemContext.Provider value={{ value, isOpen }}>
      <div className={`accordion-item ${isOpen ? 'open' : ''}`}>
        {children}
      </div>
    </AccordionItemContext.Provider>
  );
}

function AccordionTrigger({ children }) {
  const { value } = useContext(AccordionItemContext);
  const { toggleItem } = useContext(AccordionContext);
  
  return (
    <button
      className="accordion-trigger"
      onClick={() => toggleItem(value)}
    >
      {children}
    </button>
  );
}

function AccordionContent({ children }) {
  const { isOpen } = useContext(AccordionItemContext);
  
  return (
    <div className={`accordion-content ${isOpen ? 'open' : ''}`}>
      {children}
    </div>
  );
}

Accordion.Item = AccordionItem;
Accordion.Trigger = AccordionTrigger;
Accordion.Content = AccordionContent;

// Usage : FAQ Section
function FAQ() {
  return (
    <Accordion type="single" defaultValue="item-1">
      <Accordion.Item value="item-1">
        <Accordion.Trigger>
          How do I create an account?
        </Accordion.Trigger>
        <Accordion.Content>
          Click on "Sign Up" and fill in your details...
        </Accordion.Content>
      </Accordion.Item>
      
      <Accordion.Item value="item-2">
        <Accordion.Trigger>
          What payment methods do you accept?
        </Accordion.Trigger>
        <Accordion.Content>
          We accept Visa, Mastercard, PayPal...
        </Accordion.Content>
      </Accordion.Item>
      
      <Accordion.Item value="item-3">
        <Accordion.Trigger>
          Can I cancel my subscription?
        </Accordion.Trigger>
        <Accordion.Content>
          Yes, you can cancel anytime from your dashboard...
        </Accordion.Content>
      </Accordion.Item>
    </Accordion>
  );
}

// Multiple items ouverts simultanément
function MultiAccordion() {
  return (
    <Accordion type="multiple">
      <Accordion.Item value="item-1">
        <Accordion.Trigger>Section 1</Accordion.Trigger>
        <Accordion.Content>Content 1</Accordion.Content>
      </Accordion.Item>
      
      <Accordion.Item value="item-2">
        <Accordion.Trigger>Section 2</Accordion.Trigger>
        <Accordion.Content>Content 2</Accordion.Content>
      </Accordion.Item>
    </Accordion>
  );
}
```

### 5. Avantages du Pattern

```javascript
// ✅ AVANTAGES :
const advantages = {
  flexibility: 'Extrêmement flexible dans la composition',
  readability: 'Code très lisible et expressif',
  customization: 'Facile à customizer sans props complexes',
  separation: 'Séparation claire des responsabilités',
  scalability: 'Scale bien avec la complexité',
};

// 📚 Exemples de libraries utilisant ce pattern :
const libraries = {
  'Radix UI': 'Tous leurs composants',
  'Headless UI': 'Dialog, Menu, Tabs, etc.',
  'React Table': 'Table, Header, Body, Row, Cell',
  'Reach UI': 'Tabs, Accordion, Menu',
};

// 🎯 Quand utiliser :
const useCases = {
  complexComponents: 'Select, Tabs, Accordion, Dialog',
  flexibleLayouts: 'Quand UI peut varier beaucoup',
  reusablePatterns: 'Design systems, component libraries',
};
```

---

## 🏭 Pattern 3 : Higher-Order Components (HOC)

### 1. Comprendre le Pattern

```javascript
// HOC = Fonction qui prend un Component et retourne un nouveau Component
// Pattern : withFeature(Component) → EnhancedComponent

// ❌ PROBLÈME : Logique répétée dans plusieurs composants
function UserProfile() {
  const [user, setUser] = useState(null);
  const [loading, setLoading] = useState(true);
  
  useEffect(() => {
    fetchUser().then(data => {
      setUser(data);
      setLoading(false);
    });
  }, []);
  
  if (loading) return <Spinner />;
  if (!user) return <div>Not authenticated</div>;
  
  return <div>Welcome {user.name}</div>;
}

function Settings() {
  // ❌ Même logique dupliquée !
  const [user, setUser] = useState(null);
  const [loading, setLoading] = useState(true);
  
  useEffect(() => {
    fetchUser().then(data => {
      setUser(data);
      setLoading(false);
    });
  }, []);
  
  if (loading) return <Spinner />;
  if (!user) return <div>Not authenticated</div>;
  
  return <div>Settings for {user.name}</div>;
}
```

### 2. Solution : HOC

```javascript
// ✅ HOC pour ajouter authentication logic
function withAuth(Component) {
  return function AuthenticatedComponent(props) {
    const [user, setUser] = useState(null);
    const [loading, setLoading] = useState(true);
    
    useEffect(() => {
      fetchUser().then(data => {
        setUser(data);
        setLoading(false);
      });
    }, []);
    
    if (loading) return <Spinner />;
    if (!user) return <Navigate to="/login" />;
    
    // Passer user comme prop au composant wrappé
    return <Component {...props} user={user} />;
  };
}

// Usage : Composants simplifiés !
function UserProfile({ user }) {
  return <div>Welcome {user.name}</div>;
}

function Settings({ user }) {
  return <div>Settings for {user.name}</div>;
}

// Wrapper avec HOC
const AuthenticatedUserProfile = withAuth(UserProfile);
const AuthenticatedSettings = withAuth(Settings);

// Dans l'app
function App() {
  return (
    <Routes>
      <Route path="/profile" element={<AuthenticatedUserProfile />} />
      <Route path="/settings" element={<AuthenticatedSettings />} />
    </Routes>
  );
}
```

### 3. HOC avec Configuration

```javascript
// HOC qui accepte des options
function withLoader(Component, loaderConfig = {}) {
  return function LoaderComponent(props) {
    const {
      fetchFn,
      loadingComponent = <Spinner />,
      errorComponent = <Error />,
    } = loaderConfig;
    
    const [data, setData] = useState(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);
    
    useEffect(() => {
      fetchFn(props)
        .then(setData)
        .catch(setError)
        .finally(() => setLoading(false));
    }, [props]);
    
    if (loading) return loadingComponent;
    if (error) return errorComponent;
    
    return <Component {...props} data={data} />;
  };
}

// Usage avec différentes configs
const UserProfileWithData = withLoader(UserProfile, {
  fetchFn: ({ userId }) => fetchUser(userId),
  loadingComponent: <UserSkeleton />,
});

const PostsWithData = withLoader(PostsList, {
  fetchFn: () => fetchPosts(),
  loadingComponent: <PostsSkeleton />,
  errorComponent: <PostsError />,
});
```

### 4. HOC Composés (Multiple HOCs)

```javascript
// Composer plusieurs HOCs
function withAuth(Component) {
  return function AuthComponent(props) {
    const { user, loading } = useUser();
    
    if (loading) return <Spinner />;
    if (!user) return <Navigate to="/login" />;
    
    return <Component {...props} user={user} />;
  };
}

function withPermissions(requiredPermissions) {
  return function (Component) {
    return function PermissionComponent({ user, ...props }) {
      const hasPermission = requiredPermissions.every(permission =>
        user.permissions.includes(permission)
      );
      
      if (!hasPermission) {
        return <div>Access Denied</div>;
      }
      
      return <Component {...props} user={user} />;
    };
  };
}

function withAnalytics(Component) {
  return function AnalyticsComponent(props) {
    useEffect(() => {
      analytics.track('Page View', { 
        component: Component.name 
      });
    }, []);
    
    return <Component {...props} />;
  };
}

// Composer les HOCs
const AdminPanel = ({ user }) => {
  return <div>Admin Panel for {user.name}</div>;
};

// Application de multiple HOCs
const EnhancedAdminPanel = withAuth(
  withPermissions(['admin', 'write'])(
    withAnalytics(AdminPanel)
  )
);

// Ou utiliser une fonction compose helper
function compose(...fns) {
  return fns.reduce((a, b) => (...args) => a(b(...args)));
}

const EnhancedAdminPanel2 = compose(
  withAuth,
  withPermissions(['admin', 'write']),
  withAnalytics
)(AdminPanel);
```

### 5. HOC pour Styling et Theming

```javascript
// HOC pour ajouter styles ou theme
function withTheme(Component) {
  return function ThemedComponent(props) {
    const theme = useContext(ThemeContext);
    
    return <Component {...props} theme={theme} />;
  };
}

function withStyles(styles) {
  return function (Component) {
    return function StyledComponent(props) {
      return (
        <div style={styles}>
          <Component {...props} />
        </div>
      );
    };
  };
}

// Usage
const Card = ({ theme, children }) => {
  return (
    <div style={{ 
      backgroundColor: theme.cardBg,
      color: theme.text 
    }}>
      {children}
    </div>
  );
};

const ThemedCard = withTheme(Card);

const PaddedCard = withStyles({ padding: '20px' })(Card);
```

### 6. Avantages et Inconvénients

```javascript
// ✅ AVANTAGES :
const advantages = {
  reusability: 'Logique réutilisable cross-cutting',
  composition: 'Facile à composer multiple HOCs',
  separation: 'Séparation des concerns',
  legacy: 'Pattern établi et bien compris',
};

// ❌ INCONVÉNIENTS :
const disadvantages = {
  wrapper_hell: 'Nombreux wrappers dans DevTools',
  props_collision: 'Risque de collision de props',
  static_methods: 'Perte des static methods',
  refs: 'Problèmes avec refs (besoin forwardRef)',
  complexity: 'Peut devenir complexe avec many HOCs',
};

// 🎯 HOC vs Custom Hooks (Moderne)
const comparison = {
  // HOC (Old way)
  hoc: withAuth(withLoader(withTheme(Component))),
  
  // Custom Hooks (Modern way - PRÉFÉRÉ)
  hooks: function Component() {
    const { user } = useAuth();
    const { data } = useLoader();
    const theme = useTheme();
    
    return <div>Component</div>;
  },
};

// ⚠️ Note : HOC moins utilisé aujourd'hui
// Custom Hooks souvent meilleure solution
// Mais HOC toujours utile dans certains cas (ex: React Router withRouter)
```

---

## 🎨 Pattern 4 : Component Composition Avancée

### 1. Composition avec Children

```javascript
// Pattern : Container/Presentational
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

// Usage : Composition flexible
function UserCard({ user }) {
  return (
    <Card>
      <CardHeader>
        <h2>{user.name}</h2>
        <span>{user.role}</span>
      </CardHeader>
      
      <CardBody>
        <p>Email: {user.email}</p>
        <p>Phone: {user.phone}</p>
      </CardBody>
      
      <CardFooter>
        <button>Edit</button>
        <button>Delete</button>
      </CardFooter>
    </Card>
  );
}

// Même composants, layout différent
function SimpleCard({ title, content }) {
  return (
    <Card>
      <CardHeader>
        <h3>{title}</h3>
      </CardHeader>
      <CardBody>
        {content}
      </CardBody>
    </Card>
  );
}
```

### 2. Slots Pattern

```javascript
// Pattern : Named slots (inspiré de Vue)
function Layout({ header, sidebar, main, footer }) {
  return (
    <div className="layout">
      <header className="layout-header">{header}</header>
      
      <div className="layout-content">
        <aside className="layout-sidebar">{sidebar}</aside>
        <main className="layout-main">{main}</main>
      </div>
      
      <footer className="layout-footer">{footer}</footer>
    </div>
  );
}

// Usage : Props comme slots
function Dashboard() {
  return (
    <Layout
      header={<DashboardHeader />}
      sidebar={<DashboardSidebar />}
      main={<DashboardMain />}
      footer={<DashboardFooter />}
    />
  );
}

// Exemple avancé : Modal avec slots
function Modal({ 
  trigger, 
  title, 
  content, 
  actions, 
  isOpen,
  onClose 
}) {
  return (
    <>
      {/* Trigger slot */}
      <div onClick={() => setIsOpen(true)}>
        {trigger}
      </div>
      
      {/* Modal */}
      {isOpen && (
        <div className="modal-overlay" onClick={onClose}>
          <div className="modal" onClick={(e) => e.stopPropagation()}>
            {/* Title slot */}
            <div className="modal-header">
              {title}
              <button onClick={onClose}>×</button>
            </div>
            
            {/* Content slot */}
            <div className="modal-content">
              {content}
            </div>
            
            {/* Actions slot */}
            {actions && (
              <div className="modal-actions">
                {actions}
              </div>
            )}
          </div>
        </div>
      )}
    </>
  );
}

// Usage avec slots nommés
function UserEditModal({ user }) {
  const [isOpen, setIsOpen] = useState(false);
  
  return (
    <Modal
      trigger={<button>Edit User</button>}
      
      title={<h2>Edit {user.name}</h2>}
      
      content={
        <form>
          <input defaultValue={user.name} />
          <input defaultValue={user.email} />
        </form>
      }
      
      actions={
        <>
          <button onClick={() => setIsOpen(false)}>Cancel</button>
          <button type="submit">Save</button>
        </>
      }
      
      isOpen={isOpen}
      onClose={() => setIsOpen(false)}
    />
  );
}
```

### 3. Inversion of Control Pattern

```javascript
// Au lieu de contrôler tout dans le parent, 
// donner le contrôle aux children

// ❌ Approche traditionnelle : Parent contrôle tout
function List({ items, renderItem, onItemClick }) {
  return (
    <div>
      {items.map(item => (
        <div key={item.id} onClick={() => onItemClick(item)}>
          {renderItem(item)}
        </div>
      ))}
    </div>
  );
}

// ✅ Inversion of Control : Children contrôlent
function List({ children }) {
  return <div className="list">{children}</div>;
}

function ListItem({ children, onClick }) {
  return (
    <div className="list-item" onClick={onClick}>
      {children}
    </div>
  );
}

// Usage : Plus de flexibilité !
function TodoList({ todos }) {
  return (
    <List>
      {todos.map(todo => (
        <ListItem key={todo.id} onClick={() => handleClick(todo)}>
          <input type="checkbox" checked={todo.done} />
          <span>{todo.text}</span>
          <button>Delete</button>
        </ListItem>
      ))}
    </List>
  );
}

// Même composants, UI complètement différente
function UserList({ users }) {
  return (
    <List>
      {users.map(user => (
        <ListItem key={user.id}>
          <Avatar src={user.avatar} />
          <div>
            <h3>{user.name}</h3>
            <p>{user.email}</p>
          </div>
          <Badge>{user.role}</Badge>
        </ListItem>
      ))}
    </List>
  );
}
```

### 4. Props Collection Pattern

```javascript
// Pattern : Fournir un objet de props pré-configurées
function useToggle(initialValue = false) {
  const [on, setOn] = useState(initialValue);
  
  const toggle = () => setOn(prev => !prev);
  const setOff = () => setOn(false);
  const setOnState = () => setOn(true);
  
  // Props collection pour le toggle button
  const togglerProps = {
    'aria-pressed': on,
    onClick: toggle,
  };
  
  return {
    on,
    setOn: setOnState,
    setOff,
    toggle,
    togglerProps, // ✅ Props pré-configurées
  };
}

// Usage : Appliquer props collection
function Toggle() {
  const { on, togglerProps } = useToggle();
  
  return (
    <div>
      <button {...togglerProps}>
        {on ? 'ON' : 'OFF'}
      </button>
      
      {on && <div>Content visible!</div>}
    </div>
  );
}

// Exemple avancé : Combobox props collection
function useCombobox({ items, onSelect }) {
  const [isOpen, setIsOpen] = useState(false);
  const [selectedIndex, setSelectedIndex] = useState(-1);
  const [inputValue, setInputValue] = useState('');
  
  // Props pour l'input
  const inputProps = {
    value: inputValue,
    onChange: (e) => setInputValue(e.target.value),
    onFocus: () => setIsOpen(true),
    onKeyDown: (e) => {
      if (e.key === 'ArrowDown') {
        setSelectedIndex(i => Math.min(i + 1, items.length - 1));
      } else if (e.key === 'ArrowUp') {
        setSelectedIndex(i => Math.max(i - 1, 0));
      } else if (e.key === 'Enter' && selectedIndex >= 0) {
        onSelect(items[selectedIndex]);
        setIsOpen(false);
      }
    },
    'aria-expanded': isOpen,
    'aria-activedescendant': `option-${selectedIndex}`,
    role: 'combobox',
  };
  
  // Props pour la liste
  const listProps = {
    role: 'listbox',
    style: { display: isOpen ? 'block' : 'none' },
  };
  
  // Function pour obtenir props d'un item
  const getItemProps = (index) => ({
    id: `option-${index}`,
    role: 'option',
    'aria-selected': selectedIndex === index,
    onClick: () => {
      onSelect(items[index]);
      setIsOpen(false);
    },
    onMouseEnter: () => setSelectedIndex(index),
  });
  
  return {
    isOpen,
    selectedIndex,
    inputProps,
    listProps,
    getItemProps,
  };
}

// Usage avec props collections
function Combobox({ items, onSelect }) {
  const { inputProps, listProps, getItemProps, selectedIndex } = useCombobox({
    items,
    onSelect,
  });
  
  return (
    <div>
      <input {...inputProps} placeholder="Search..." />
      
      <ul {...listProps}>
        {items.map((item, index) => (
          <li
            key={item.id}
            {...getItemProps(index)}
            style={{
              backgroundColor: selectedIndex === index ? '#eee' : 'white',
            }}
          >
            {item.name}
          </li>
        ))}
      </ul>
    </div>
  );
}
```

### 5. State Reducer Pattern

```javascript
// Pattern : Permettre aux utilisateurs de contrôler le state management
function useToggle(initialValue = false, reducer) {
  // Default reducer
  const defaultReducer = (state, action) => {
    switch (action.type) {
      case 'TOGGLE':
        return { on: !state.on };
      case 'SET_ON':
        return { on: true };
      case 'SET_OFF':
        return { on: false };
      case 'RESET':
        return { on: initialValue };
      default:
        return state;
    }
  };
  
  // Utiliser custom reducer si fourni, sinon default
  const [state, dispatch] = useReducer(
    reducer || defaultReducer,
    { on: initialValue }
  );
  
  const toggle = () => dispatch({ type: 'TOGGLE' });
  const setOn = () => dispatch({ type: 'SET_ON' });
  const setOff = () => dispatch({ type: 'SET_OFF' });
  const reset = () => dispatch({ type: 'RESET' });
  
  return {
    on: state.on,
    toggle,
    setOn,
    setOff,
    reset,
  };
}

// Usage 1 : Comportement default
function SimpleToggle() {
  const { on, toggle } = useToggle();
  
  return (
    <button onClick={toggle}>
      {on ? 'ON' : 'OFF'}
    </button>
  );
}

// Usage 2 : Custom reducer pour modifier comportement
function AdvancedToggle() {
  const customReducer = (state, action) => {
    switch (action.type) {
      case 'TOGGLE':
        // Empêcher toggle si on est déjà ON
        if (state.on) {
          console.log('Cannot toggle off once on!');
          return state;
        }
        return { on: true };
      
      case 'SET_OFF':
        // Log quand set to OFF
        console.log('Turned off at:', new Date().toISOString());
        return { on: false };
      
      default:
        // Déléguer au default reducer
        return { on: !state.on };
    }
  };
  
  const { on, toggle, setOff } = useToggle(false, customReducer);
  
  return (
    <div>
      <button onClick={toggle}>Toggle</button>
      <button onClick={setOff}>Force Off</button>
      <p>Status: {on ? 'ON' : 'OFF'}</p>
    </div>
  );
}

// Exemple avancé : Counter avec state reducer
function useCounter(initialValue = 0, reducer) {
  const defaultReducer = (state, action) => {
    switch (action.type) {
      case 'INCREMENT':
        return { count: state.count + 1 };
      case 'DECREMENT':
        return { count: state.count - 1 };
      case 'SET':
        return { count: action.payload };
      case 'RESET':
        return { count: initialValue };
      default:
        return state;
    }
  };
  
  const [state, dispatch] = useReducer(
    reducer || defaultReducer,
    { count: initialValue }
  );
  
  const increment = () => dispatch({ type: 'INCREMENT' });
  const decrement = () => dispatch({ type: 'DECREMENT' });
  const set = (value) => dispatch({ type: 'SET', payload: value });
  const reset = () => dispatch({ type: 'RESET' });
  
  return {
    count: state.count,
    increment,
    decrement,
    set,
    reset,
  };
}

// Custom reducer avec limites
function LimitedCounter() {
  const customReducer = (state, action) => {
    switch (action.type) {
      case 'INCREMENT':
        // Max 10
        if (state.count >= 10) return state;
        return { count: state.count + 1 };
      
      case 'DECREMENT':
        // Min 0
        if (state.count <= 0) return state;
        return { count: state.count - 1 };
      
      default:
        return state;
    }
  };
  
  const { count, increment, decrement } = useCounter(0, customReducer);
  
  return (
    <div>
      <button onClick={decrement} disabled={count === 0}>-</button>
      <span>{count}</span>
      <button onClick={increment} disabled={count === 10}>+</button>
    </div>
  );
}
```

---

## 🔌 Pattern 5 : Provider Pattern

### 1. Context avec Custom Hook

```javascript
// Pattern : Créer provider + custom hook pour accès facile
const ThemeContext = createContext();

// Provider component
function ThemeProvider({ children }) {
  const [theme, setTheme] = useState('light');
  
  const toggleTheme = () => {
    setTheme(prev => prev === 'light' ? 'dark' : 'light');
  };
  
  const value = useMemo(
    () => ({ theme, toggleTheme }),
    [theme]
  );
  
  return (
    <ThemeContext.Provider value={value}>
      {children}
    </ThemeContext.Provider>
  );
}

// ✅ Custom hook pour accès facile
function useTheme() {
  const context = useContext(ThemeContext);
  
  if (!context) {
    throw new Error('useTheme must be used within ThemeProvider');
  }
  
  return context;
}

// Usage : Simple et sûr
function Button() {
  const { theme, toggleTheme } = useTheme(); // ✅ Pas besoin useContext
  
  return (
    <button
      onClick={toggleTheme}
      style={{
        backgroundColor: theme === 'light' ? '#fff' : '#000',
        color: theme === 'light' ? '#000' : '#fff',
      }}
    >
      Toggle Theme
    </button>
  );
}

// App
function App() {
  return (
    <ThemeProvider>
      <Button />
    </ThemeProvider>
  );
}
```

### 2. Multiple Providers Pattern

```javascript
// Composer plusieurs providers
function AppProviders({ children }) {
  return (
    <QueryClientProvider client={queryClient}>
      <AuthProvider>
        <ThemeProvider>
          <NotificationProvider>
            <I18nProvider>
              {children}
            </I18nProvider>
          </NotificationProvider>
        </ThemeProvider>
      </AuthProvider>
    </QueryClientProvider>
  );
}

// Usage
function App() {
  return (
    <AppProviders>
      <Router>
        <Routes />
      </Router>
    </AppProviders>
  );
}

// Ou créer un helper pour composer
function composeProviders(...providers) {
  return providers.reduce(
    (AccumulatedProviders, CurrentProvider) => {
      return ({ children }) => (
        <AccumulatedProviders>
          <CurrentProvider>{children}</CurrentProvider>
        </AccumulatedProviders>
      );
    },
    ({ children }) => <>{children}</>
  );
}

const AllProviders = composeProviders(
  QueryClientProvider,
  AuthProvider,
  ThemeProvider,
  NotificationProvider
);

function App() {
  return (
    <AllProviders>
      <Router />
    </AllProviders>
  );
}
```

### 3. Provider avec Actions Pattern

```javascript
// Provider avec state ET actions séparées
const CartStateContext = createContext();
const CartActionsContext = createContext();

function CartProvider({ children }) {
  const [items, setItems] = useState([]);
  
  // Actions
  const actions = useMemo(() => ({
    addItem: (product) => {
      setItems(prev => {
        const existing = prev.find(item => item.id === product.id);
        if (existing) {
          return prev.map(item =>
            item.id === product.id
              ? { ...item, quantity: item.quantity + 1 }
              : item
          );
        }
        return [...prev, { ...product, quantity: 1 }];
      });
    },
    
    removeItem: (productId) => {
      setItems(prev => prev.filter(item => item.id !== productId));
    },
    
    updateQuantity: (productId, quantity) => {
      setItems(prev =>
        prev.map(item =>
          item.id === productId ? { ...item, quantity } : item
        )
      );
    },
    
    clearCart: () => setItems([]),
  }), []);
  
  // State dérivé
  const state = useMemo(() => ({
    items,
    totalItems: items.reduce((sum, item) => sum + item.quantity, 0),
    totalPrice: items.reduce((sum, item) => sum + item.price * item.quantity, 0),
  }), [items]);
  
  return (
    <CartStateContext.Provider value={state}>
      <CartActionsContext.Provider value={actions}>
        {children}
      </CartActionsContext.Provider>
    </CartStateContext.Provider>
  );
}

// Deux hooks séparés
function useCartState() {
  const context = useContext(CartStateContext);
  if (!context) throw new Error('useCartState must be within CartProvider');
  return context;
}

function useCartActions() {
  const context = useContext(CartActionsContext);
  if (!context) throw new Error('useCartActions must be within CartProvider');
  return context;
}

// Usage : Composants ne re-render QUE si nécessaire
function CartBadge() {
  const { totalItems } = useCartState(); // Re-render si totalItems change
  // Pas d'accès aux actions = pas de re-render inutile
  
  return <span className="badge">{totalItems}</span>;
}

function AddToCartButton({ product }) {
  const { addItem } = useCartActions(); // Pas de re-render si cart change
  // Seulement les actions, pas le state
  
  return (
    <button onClick={() => addItem(product)}>
      Add to Cart
    </button>
  );
}

function Cart() {
  const { items, totalPrice } = useCartState();
  const { removeItem, updateQuantity } = useCartActions();
  
  return (
    <div>
      {items.map(item => (
        <CartItem
          key={item.id}
          item={item}
          onRemove={() => removeItem(item.id)}
          onUpdateQuantity={(qty) => updateQuantity(item.id, qty)}
        />
      ))}
      <p>Total: ${totalPrice}</p>
    </div>
  );
}
```

---

## 🎭 Pattern 6 : Container/Presentational Pattern

### 1. Séparation des Concerns

```javascript
// ❌ Composant qui fait TOUT (logique + UI)
function UserProfile({ userId }) {
  const [user, setUser] = useState(null);
  const [loading, setLoading] = useState(true);
  const [editing, setEditing] = useState(false);
  
  useEffect(() => {
    fetchUser(userId).then(data => {
      setUser(data);
      setLoading(false);
    });
  }, [userId]);
  
  const handleUpdate = async (updates) => {
    await updateUser(userId, updates);
    setUser(prev => ({ ...prev, ...updates }));
    setEditing(false);
  };
  
  if (loading) return <Spinner />;
  
  return (
    <div className="user-profile">
      {editing ? (
        <form onSubmit={handleUpdate}>
          <input defaultValue={user.name} name="name" />
          <input defaultValue={user.email} name="email" />
          <button type="submit">Save</button>
        </form>
      ) : (
        <div>
          <h2>{user.name}</h2>
          <p>{user.email}</p>
          <button onClick={() => setEditing(true)}>Edit</button>
        </div>
      )}
    </div>
  );
}

// Problème : Logique et UI mélangées, difficile à tester et réutiliser
```

### 2. Séparation Container/Presentational

```javascript
// ✅ PRESENTATIONAL : Seulement UI (dumb component)
function UserProfileView({ user, isEditing, onEdit, onSave, onCancel }) {
  if (isEditing) {
    return (
      <div className="user-profile">
        <form onSubmit={onSave}>
          <input defaultValue={user.name} name="name" />
          <input defaultValue={user.email} name="email" />
          <button type="submit">Save</button>
          <button type="button" onClick={onCancel}>Cancel</button>
        </form>
      </div>
    );
  }
  
  return (
    <div className="user-profile">
      <h2>{user.name}</h2>
      <p>{user.email}</p>
      <p>{user.bio}</p>
      <button onClick={onEdit}>Edit</button>
    </div>
  );
}

// ✅ CONTAINER : Seulement logique (smart component)
function UserProfileContainer({ userId }) {
  const [user, setUser] = useState(null);
  const [loading, setLoading] = useState(true);
  const [editing, setEditing] = useState(false);
  
  useEffect(() => {
    fetchUser(userId).then(data => {
      setUser(data);
      setLoading(false);
    });
  }, [userId]);
  
  const handleSave = async (e) => {
    e.preventDefault();
    const formData = new FormData(e.target);
    const updates = Object.fromEntries(formData);
    
    await updateUser(userId, updates);
    setUser(prev => ({ ...prev, ...updates }));
    setEditing(false);
  };
  
  if (loading) return <Spinner />;
  
  return (
    <UserProfileView
      user={user}
      isEditing={editing}
      onEdit={() => setEditing(true)}
      onSave={handleSave}
      onCancel={() => setEditing(false)}
    />
  );
}

// Avantages :
// - UserProfileView facilement testable (pure function)
// - UserProfileView réutilisable avec différentes sources de data
// - Logique centralisée dans Container
// - Storybook facile pour UserProfileView
```

### 3. Exemple avec Custom Hook (Moderne)

```javascript
// 🔥 APPROCHE MODERNE : Custom Hook + Presentational Component

// Hook : Logique réutilisable
function useUserProfile(userId) {
  const [user, setUser] = useState(null);
  const [loading, setLoading] = useState(true);
  const [editing, setEditing] = useState(false);
  
  useEffect(() => {
    fetchUser(userId).then(data => {
      setUser(data);
      setLoading(false);
    });
  }, [userId]);
  
  const startEditing = () => setEditing(true);
  const cancelEditing = () => setEditing(false);
  
  const saveUser = async (updates) => {
    await updateUser(userId, updates);
    setUser(prev => ({ ...prev, ...updates }));
    setEditing(false);
  };
  
  return {
    user,
    loading,
    editing,
    startEditing,
    cancelEditing,
    saveUser,
  };
}

// Presentational Component
function UserProfileView({ user, isEditing, onEdit, onSave, onCancel }) {
  if (isEditing) {
    return (
      <form onSubmit={onSave}>
        <input defaultValue={user.name} name="name" />
        <input defaultValue={user.email} name="email" />
        <button type="submit">Save</button>
        <button type="button" onClick={onCancel}>Cancel</button>
      </form>
    );
  }
  
  return (
    <div>
      <h2>{user.name}</h2>
      <p>{user.email}</p>
      <button onClick={onEdit}>Edit</button>
    </div>
  );
}

// Component qui combine les deux
function UserProfile({ userId }) {
  const {
    user,
    loading,
    editing,
    startEditing,
    cancelEditing,
    saveUser,
  } = useUserProfile(userId);
  
  if (loading) return <Spinner />;
  
  const handleSave = (e) => {
    e.preventDefault();
    const formData = new FormData(e.target);
    const updates = Object.fromEntries(formData);
    saveUser(updates);
  };
  
  return (
    <UserProfileView
      user={user}
      isEditing={editing}
      onEdit={startEditing}
      onSave={handleSave}
      onCancel={cancelEditing}
    />
  );
}

// ✅ Meilleur des deux mondes :
// - Hook réutilisable dans autres contextes
// - Presentational component testable
// - Séparation claire des concerns
```

---

## 🎯 Pattern 7 : Controlled vs Uncontrolled Components

### 1. Controlled Components

```javascript
// CONTROLLED : React contrôle le state
function ControlledInput() {
  const [value, setValue] = useState('');
  
  return (
    <input
      value={value} // ✅ Value contrôlée par React state
      onChange={(e) => setValue(e.target.value)}
    />
  );
}

// Avantages :
// - Validation en temps réel
// - Conditional logic facile
// - State synchronisé
const advantages = {
  validation: 'Valider à chaque keystroke',
  formatting: 'Formatter input (uppercase, etc.)',
  conditional: 'Afficher/cacher based on value',
  sync: 'State toujours à jour',
};

// Exemple : Form validation en temps réel
function ControlledForm() {
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  
  const emailValid = /\S+@\S+\.\S+/.test(email);
  const passwordValid = password.length >= 8;
  const formValid = emailValid && passwordValid;
  
  return (
    <form>
      <input
        type="email"
        value={email}
        onChange={(e) => setEmail(e.target.value)}
        style={{ borderColor: email && !emailValid ? 'red' : 'green' }}
      />
      {email && !emailValid && <span>Invalid email</span>}
      
      <input
        type="password"
        value={password}
        onChange={(e) => setPassword(e.target.value)}
        style={{ borderColor: password && !passwordValid ? 'red' : 'green' }}
      />
      {password && !passwordValid && <span>Min 8 characters</span>}
      
      <button disabled={!formValid}>Submit</button>
    </form>
  );
}
```

### 2. Uncontrolled Components

```javascript
// UNCONTROLLED : DOM contrôle le state
function UncontrolledInput() {
  const inputRef = useRef();
  
  const handleSubmit = (e) => {
    e.preventDefault();
    console.log(inputRef.current.value); // ✅ Lire value du DOM
  };
  
  return (
    <form onSubmit={handleSubmit}>
      <input
        ref={inputRef}
        defaultValue="Initial value" // defaultValue, pas value
      />
      <button type="submit">Submit</button>
    </form>
  );
}

// Avantages :
// - Moins de re-renders
// - Moins de code
// - Performance (pour grandes forms)
// - Intégration librairies third-party

// Exemple : File input (toujours uncontrolled)
function FileUpload() {
  const fileRef = useRef();
  
  const handleSubmit = (e) => {
    e.preventDefault();
    const file = fileRef.current.files[0];
    uploadFile(file);
  };
  
  return (
    <form onSubmit={handleSubmit}>
      <input
        type="file"
        ref={fileRef}
        // File inputs DOIVENT être uncontrolled
      />
      <button type="submit">Upload</button>
    </form>
  );
}
```

### 3. Hybrid : Controlled avec Uncontrolled Behavior

```javascript
// Pattern : Controlled component qui accepte defaultValue
function HybridInput({ value: controlledValue, defaultValue, onChange }) {
  const [internalValue, setInternalValue] = useState(defaultValue || '');
  
  // Si value fournie → controlled
  // Sinon → uncontrolled (internal state)
  const isControlled = controlledValue !== undefined;
  const value = isControlled ? controlledValue : internalValue;
  
  const handleChange = (e) => {
    const newValue = e.target.value;
    
    if (!isControlled) {
      setInternalValue(newValue);
    }
    
    onChange?.(newValue);
  };
  
  return (
    <input value={value} onChange={handleChange} />
  );
}

// Usage 1 : Controlled
function ControlledUsage() {
  const [value, setValue] = useState('');
  
  return <HybridInput value={value} onChange={setValue} />;
}

// Usage 2 : Uncontrolled
function UncontrolledUsage() {
  return (
    <HybridInput
      defaultValue="Initial"
      onChange={(value) => console.log(value)}
    />
  );
}
```

---

## 🎤 Questions d'Entretien sur les Patterns

### Q1: "Explique la différence entre Render Props et Custom Hooks. Lequel préfères-tu ?"

**Réponse experte :**

```javascript
/**
 * RENDER PROPS vs CUSTOM HOOKS
 */

// 1. RENDER PROPS (Old way)
const renderPropsExample = {
  pattern: function MouseTracker({ render }) {
    const [position, setPosition] = useState({ x: 0, y: 0 });
    
    useEffect(() => {
      const handleMove = (e) => {
        setPosition({ x: e.clientX, y: e.clientY });
      };
      window.addEventListener('mousemove', handleMove);
      return () => window.removeEventListener('mousemove', handleMove);
    }, []);
    
    return render(position);
  },
  
  usage: <MouseTracker render={(pos) => <div>X: {pos.x}</div>} />,
  
  pros: [
    'UI complètement flexible',
    'Pattern établi avant Hooks',
    'Bon pour composition complexe'
  ],
  
  cons: [
    'Syntaxe verbeuse',
    'Callback hell avec multiple nesting',
    'Wrapper hell dans DevTools'
  ],
};

// 2. CUSTOM HOOKS (Modern way - PRÉFÉRÉ)
const customHookExample = {
  pattern: function useMousePosition() {
    const [position, setPosition] = useState({ x: 0, y: 0 });
    
    useEffect(() => {
      const handleMove = (e) => {
        setPosition({ x: e.clientX, y: e.clientY });
      };
      window.addEventListener('mousemove', handleMove);
      return () => window.removeEventListener('mousemove', handleMove);
    }, []);
    
    return position;
  },
  
  usage: function Component() {
    const position = useMousePosition();
    return <div>X: {position.x}</div>;
  },
  
  pros: [
    'Syntaxe simple et claire',
    'Facile à composer',
    'Pas de wrapper hell',
    'Standard moderne React'
  ],
  
  cons: [
    'Moins de flexibilité UI (mais généralement suffisant)'
  ],
};

// 3. CONCLUSION
const conclusion = {
  preference: 'Custom Hooks',
  reasoning: [
    'Plus simple et lisible',
    'Standard React moderne',
    'Facile à composer multiple hooks',
    'Pas de JSX nesting'
  ],
  
  whenRenderProps: [
    'Quand UI doit être TRÈS flexible',
    'Librairies qui exposent des APIs',
    'Cas très spécifiques de composition'
  ],
};

/**
 * En entretien, dire :
 * 
 * "Je préfère Custom Hooks pour la plupart des cas car plus simples 
 * et idiomatiques en React moderne. Render Props était le pattern 
 * avant l'introduction des Hooks en 2019. Aujourd'hui, Custom Hooks 
 * résolvent le même problème de manière plus élégante.
 * 
 * Cependant, Render Props reste utile pour certains cas :
 * - Libraries qui exposent des APIs très flexibles
 * - Quand l'UI doit être complètement customizable
 * - Composition complexe où le contrôle de rendering est critique
 * 
 * Dans mes projets, j'utilise principalement Custom Hooks, mais je 
 * comprends les deux patterns et peux choisir selon le contexte."
 */
```

### Q2: "C'est quoi le pattern Compound Components ? Donne un exemple concret."

**Réponse avec exemple :**

```javascript
/**
 * COMPOUND COMPONENTS PATTERN
 * 
 * Définition : Pattern où plusieurs composants travaillent ensemble
 * en partageant un state implicite via Context, créant une API
 * déclarative et flexible.
 */

// EXEMPLE CONCRET : Select Component
const SelectContext = createContext();

function Select({ children, value, onChange }) {
  const [isOpen, setIsOpen] = useState(false);
  
  const contextValue = useMemo(() => ({
    value,
    onChange,
    isOpen,
    setIsOpen,
  }), [value, onChange, isOpen]);
  
  return (
    <SelectContext.Provider value={contextValue}>
      <div className="select">{children}</div>
    </SelectContext.Provider>
  );
}

function SelectTrigger({ children }) {
  const { isOpen, setIsOpen } = useContext(SelectContext);
  
  return (
    <button onClick={() => setIsOpen(!isOpen)}>
      {children}
    </button>
  );
}

function SelectContent({ children }) {
  const { isOpen } = useContext(SelectContext);
  if (!isOpen) return null;
  return <div className="select-dropdown">{children}</div>;
}

function SelectOption({ value: optionValue, children }) {
  const { value, onChange, setIsOpen } = useContext(SelectContext);
  const isSelected = value === optionValue;
  
  return (
    <div
      className={`option ${isSelected ? 'selected' : ''}`}
      onClick={() => {
        onChange(optionValue);
        setIsOpen(false);
      }}
    >
      {children}
    </div>
  );
}

// Attacher sub-components
Select.Trigger = SelectTrigger;
Select.Content = SelectContent;
Select.Option = SelectOption;

// USAGE : API déclarative et flexible
function App() {
  const [country, setCountry] = useState('');
  
  return (
    <Select value={country} onChange={setCountry}>
      <Select.Trigger>
        {country || 'Select country'}
      </Select.Trigger>
      
      <Select.Content>
        <Select.Option value="fr">France</Select.Option>
        <Select.Option value="us">USA</Select.Option>
        <Select.Option value="uk">UK</Select.Option>
      </Select.Content>
    </Select>
  );
}

/**
 * AVANTAGES :
 * - API très flexible et intuitive
 * - Facile à customizer sans props complexes
 * - Séparation claire des responsabilités
 * - Composition puissante
 * 
 * EXEMPLES RÉELS :
 * - Radix UI (tous leurs composants)
 * - Headless UI (Tabs, Dialog, Menu)
 * - React Table (Table, Header, Body, Row)
 * 
 * COMPARAISON :
 * 
 * Sans Compound Components (props hell) :
 * <Select
 *   options={[{value: 'fr', label: 'France'}]}
 *   renderTrigger={(selected) => selected || 'Select'}
 *   renderOption={(option) => option.label}
 *   value={country}
 *   onChange={setCountry}
 * />
 * 
 * Avec Compound Components (clean) :
 * <Select value={country} onChange={setCountry}>
 *   <Select.Trigger>{country || 'Select'}</Select.Trigger>
 *   <Select.Content>
 *     <Select.Option value="fr">France</Select.Option>
 *   </Select.Content>
 * </Select>
 */
```

### Q3: "Quelle est la différence entre Container/Presentational pattern et l'utilisation de Custom Hooks ?"

**Réponse :**

```javascript
/**
 * CONTAINER/PRESENTATIONAL vs CUSTOM HOOKS
 */

// 1. CONTAINER/PRESENTATIONAL (Old Pattern)
const containerPattern = {
  // Container : Logique
  container: function UserProfileContainer({ userId }) {
    const [user, setUser] = useState(null);
    const [loading, setLoading] = useState(true);
    
    useEffect(() => {
      fetchUser(userId).then(data => {
        setUser(data);
        setLoading(false);
      });
    }, [userId]);
    
    if (loading) return <Spinner />;
    
    return (
      <UserProfileView
        user={user}
        onEdit={(updates) => updateUser(userId, updates)}
      />
    );
  },
  
  // Presentational : UI pure
  presentational: function UserProfileView({ user, onEdit }) {
    return (
      <div>
        <h2>{user.name}</h2>
        <p>{user.email}</p>
        <button onClick={onEdit}>Edit</button>
      </div>
    );
  },
  
  pros: [
    'Séparation claire logique/UI',
    'Presentational facilement testable',
    'UI réutilisable'
  ],
  
  cons: [
    '2 fichiers/composants pour 1 feature',
    'Boilerplate important',
    'Logique pas réutilisable ailleurs'
  ],
};

// 2. CUSTOM HOOKS (Modern Pattern)
const customHookPattern = {
  // Hook : Logique réutilisable
  hook: function useUserProfile(userId) {
    const [user, setUser] = useState(null);
    const [loading, setLoading] = useState(true);
    
    useEffect(() => {
      fetchUser(userId).then(data => {
        setUser(data);
        setLoading(false);
      });
    }, [userId]);
    
    const updateProfile = (updates) => {
      return updateUser(userId, updates).then(updated => {
        setUser(updated);
      });
    };
    
    return { user, loading, updateProfile };
  },
  
  // Component : UI + hook
  component: function UserProfile({ userId }) {
    const { user, loading, updateProfile } = useUserProfile(userId);
    
    if (loading) return <Spinner />;
    
    return (
      <div>
        <h2>{user.name}</h2>
        <p>{user.email}</p>
        <button onClick={() => updateProfile({ name: 'New' })}>
          Edit
        </button>
      </div>
    );
  },
  
  pros: [
    'Logique RÉUTILISABLE (hook utilisable ailleurs)',
    'Moins de boilerplate',
    '1 fichier au lieu de 2',
    'Plus flexible'
  ],
  
  cons: [
    'UI moins facilement testable en isolation',
    'Peut nécessiter presentational component séparément si besoin'
  ],
};

// 3. BEST OF BOTH WORLDS (Hybrid)
const hybridPattern = {
  // Hook : Logique réutilisable
  hook: 'useUserProfile(userId)',
  
  // Presentational : UI testable
  presentational: function UserProfileView({ user, onEdit }) {
    return (
      <div>
        <h2>{user.name}</h2>
        <button onClick={onEdit}>Edit</button>
      </div>
    );
  },
  
  // Component : Combine les deux
  component: function UserProfile({ userId }) {
    const { user, loading, updateProfile } = useUserProfile(userId);
    
    if (loading) return <Spinner />;
    
    return (
      <UserProfileView
        user={user}
        onEdit={updateProfile}
      />
    );
  },
  
  benefits: [
    'Hook réutilisable dans autres contextes',
    'Presentational testable et réutilisable',
    'Flexible et maintenable'
  ],
};

/**
 * CONCLUSION :
 * 
 * "Container/Presentational était le pattern standard avant Hooks.
 * Aujourd'hui, Custom Hooks résolvent mieux le problème de 
 * réutilisation de logique.
 * 
 * Mon approche moderne :
 * 1. Extraire logique dans Custom Hook
 * 2. Créer Presentational Component SI besoin de réutiliser UI
 * 3. Combiner les deux dans le component principal
 * 
 * Avantages :
 * - Hook réutilisable dans multiples contextes
 * - Presentational optionnel (seulement si UI réutilisée)
 * - Moins de boilerplate que Container/Presentational pur
 * - Plus flexible et maintenable"
 */
```

### Q4: "Comment implémenteriez-vous un système de Tabs accessible avec Compound Components ?"

**Réponse complète avec accessibilité :**

```javascript
/**
 * ACCESSIBLE TABS COMPONENT
 * avec Compound Components Pattern
 */

const TabsContext = createContext();

function Tabs({ children, defaultValue, orientation = 'horizontal' }) {
  const [activeTab, setActiveTab] = useState(defaultValue);
  
  const contextValue = useMemo(() => ({
    activeTab,
    setActiveTab,
    orientation,
  }), [activeTab, orientation]);
  
  return (
    <TabsContext.Provider value={contextValue}>
      <div className="tabs" data-orientation={orientation}>
        {children}
      </div>
    </TabsContext.Provider>
  );
}

function TabsList({ children }) {
  const { orientation } = useContext(TabsContext);
  const listRef = useRef(null);
  
  // Keyboard navigation (accessibility)
  useEffect(() => {
    const list = listRef.current;
    if (!list) return;
    
    const handleKeyDown = (e) => {
      const triggers = Array.from(
        list.querySelectorAll('[role="tab"]')
      );
      const currentIndex = triggers.findIndex(
        el => el === document.activeElement
      );
      
      let nextIndex;
      
      if (orientation === 'horizontal') {
        if (e.key === 'ArrowRight') {
          nextIndex = (currentIndex + 1) % triggers.length;
        } else if (e.key === 'ArrowLeft') {
          nextIndex = (currentIndex - 1 + triggers.length) % triggers.length;
        }
      } else {
        if (e.key === 'ArrowDown') {
          nextIndex = (currentIndex + 1) % triggers.length;
        } else if (e.key === 'ArrowUp') {
          nextIndex = (currentIndex - 1 + triggers.length) % triggers.length;
        }
      }
      
      if (nextIndex !== undefined) {
        e.preventDefault();
        triggers[nextIndex].focus();
        triggers[nextIndex].click();
      }
      
      // Home/End keys
      if (e.key === 'Home') {
        e.preventDefault();
        triggers[0].focus();
        triggers[0].click();
      } else if (e.key === 'End') {
        e.preventDefault();
        triggers[triggers.length - 1].focus();
        triggers[triggers.length - 1].click();
      }
    };
    
    list.addEventListener('keydown', handleKeyDown);
    return () => list.removeEventListener('keydown', handleKeyDown);
  }, [orientation]);
  
  return (
    <div
      ref={listRef}
      className="tabs-list"
      role="tablist"
      aria-orientation={orientation}
    >
      {children}
    </div>
  );
}

function TabsTrigger({ value, children, disabled = false }) {
  const { activeTab, setActiveTab } = useContext(TabsContext);
  const isActive = activeTab === value;
  
  return (
    <button
      className={`tabs-trigger ${isActive ? 'active' : ''}`}
      onClick={() => !disabled && setActiveTab(value)}
      disabled={disabled}
      role="tab"
      aria-selected={isActive}
      aria-controls={`tabpanel-${value}`}
      id={`tab-${value}`}
      tabIndex={isActive ? 0 : -1}
    >
      {children}
    </button>
  );
}

function TabsContent({ value, children }) {
  const { activeTab } = useContext(TabsContext);
  const isActive = activeTab === value;
  
  if (!isActive) return null;
  
  return (
    <div
      className="tabs-content"
      role="tabpanel"
      id={`tabpanel-${value}`}
      aria-labelledby={`tab-${value}`}
      tabIndex={0}
    >
      {children}
    </div>
  );
}

// Attacher sub-components
Tabs.List = TabsList;
Tabs.Trigger = TabsTrigger;
Tabs.Content = TabsContent;

// USAGE
function Dashboard() {
  return (
    <Tabs defaultValue="overview">
      <Tabs.List>
        <Tabs.Trigger value="overview">
          Overview
        </Tabs.Trigger>
        <Tabs.Trigger value="analytics">
          Analytics
        </Tabs.Trigger>
        <Tabs.Trigger value="reports">
          Reports
        </Tabs.Trigger>
        <Tabs.Trigger value="settings" disabled>
          Settings (Coming Soon)
        </Tabs.Trigger>
      </Tabs.List>
      
      <Tabs.Content value="overview">
        <OverviewPanel />
      </Tabs.Content>
      
      <Tabs.Content value="analytics">
        <AnalyticsPanel />
      </Tabs.Content>
      
      <Tabs.Content value="reports">
        <ReportsPanel />
      </Tabs.Content>
    </Tabs>
  );
}

/**
 * FEATURES D'ACCESSIBILITÉ IMPLÉMENTÉES :
 * 
 * 1. ARIA Attributes :
 *    - role="tablist" sur la liste
 *    - role="tab" sur les triggers
 *    - role="tabpanel" sur le content
 *    - aria-selected pour l'état
 *    - aria-controls pour lier tab/panel
 *    - aria-labelledby pour lier panel/tab
 * 
 * 2. Keyboard Navigation :
 *    - Arrow Left/Right (horizontal)
 *    - Arrow Up/Down (vertical)
 *    - Home : premier tab
 *    - End : dernier tab
 *    - Tab : focus sur active tab
 * 
 * 3. Focus Management :
 *    - tabIndex={0} pour active tab
 *    - tabIndex={-1} pour inactive tabs
 *    - Focus visible sur keyboard navigation
 * 
 * 4. Disabled State :
 *    - disabled prop sur trigger
 *    - Skip dans keyboard navigation
 * 
 * POURQUOI COMPOUND COMPONENTS ICI :
 * - API flexible (layout customizable)
 * - Context cache la complexité
 * - Facile d'ajouter des variants (vertical tabs, etc.)
 * - Composition naturelle
 */
```

### Q5: "Quand utiliserais-tu un HOC vs un Custom Hook ?"

**Réponse pragmatique :**

```javascript
/**
 * HOC vs CUSTOM HOOK - Décision Matrix
 */

const decisionMatrix = {
  // Préférer CUSTOM HOOK pour :
  customHook: {
    useCases: [
      'Réutilisation de logique (state, effects)',
      'Data fetching',
      'Form handling',
      'Event listeners',
      'Subscriptions',
    ],
    
    example: function useAuth() {
      const [user, setUser] = useState(null);
      const [loading, setLoading] = useState(true);
      
      useEffect(() => {
        fetchUser().then(setUser).finally(() => setLoading(false));
      }, []);
      
      const logout = () => {
        return logoutUser().then(() => setUser(null));
      };
      
      return { user, loading, logout };
    },
    
    advantages: [
      'Simple et moderne',
      'Composable facilement',
      'Pas de wrapper hell',
      'Standard React',
    ],
  },
  
  // Préférer HOC pour :
  hoc: {
    useCases: [
      'Wrapping existing components (third-party)',
      'Props injection cross-cutting',
      'Component enhancement (legacy code)',
      'Conditional rendering basé sur props',
    ],
    
    example: function withErrorBoundary(Component) {
      return class ErrorBoundaryWrapper extends React.Component {
        state = { hasError: false };
        
        static getDerivedStateFromError() {
          return { hasError: true };
        }
        
        componentDidCatch(error, info) {
          logError(error, info);
        }
        
        render() {
          if (this.state.hasError) {
            return <ErrorFallback />;
          }
          return <Component {...this.props} />;
        }
      };
    },
    
    advantages: [
      'Wrap third-party components',
      'Class component features (error boundaries)',
      'Props transformation',
    ],
  },
  
  // CAS SPÉCIFIQUES
  specificCases: {
    // Error Boundary : MUST be HOC (class component)
    errorBoundary: 'HOC (no Hook equivalent)',
    
    // Route protection : Hook préférable
    routeProtection: {
      old: 'withAuth(Component)',
      modern: 'useAuth() inside component',
    },
    
    // Theme injection : Hook préférable
    theming: {
      old: 'withTheme(Component)',
      modern: 'useTheme() hook',
    },
    
    // Analytics : Les deux OK
    analytics: {
      hoc: 'withAnalytics(Component) - automatic',
      hook: 'useAnalytics() - manual control',
    },
  },
};

/**
 * CONCLUSION EN ENTRETIEN :
 * 
 * "Aujourd'hui, je préfère Custom Hooks dans 90% des cas :
 * - Plus simple et idiomatique
 * - Meilleure composition
 * - Pas de wrapper hell
 * 
 * J'utilise HOC uniquement pour :
 * 1. Error Boundaries (pas de Hook équivalent)
 * 2. Wrapping third-party components
 * 3. Travailler sur legacy codebase avec HOCs existants
 * 
 * La règle générale : Si je peux le faire avec un Hook, je le fais.
 * Si j'ai besoin de class component features (error boundary) 
 * ou de wrapper un component existant sans le modifier, j'utilise HOC."
 */

// EXEMPLE CONCRET : Route Protection

// ❌ Old way : HOC
const withAuth = (Component) => {
  return function AuthComponent(props) {
    const [user, setUser] = useState(null);
    
    useEffect(() => {
      fetchUser().then(setUser);
    }, []);
    
    if (!user) return <Navigate to="/login" />;
    return <Component {...props} user={user} />;
  };
};

const ProtectedPage = withAuth(Dashboard);

// ✅ Modern way : Hook
function Dashboard() {
  const { user, loading } = useAuth();
  
  if (loading) return <Spinner />;
  if (!user) return <Navigate to="/login" />;
  
  return <div>Dashboard for {user.name}</div>;
}

// Plus clair, plus flexible, moins de wrappers !
```

---

## 💡 Best Practices - Patterns Avancés

### Checklist Complète

```javascript
const bestPractices = {
  // 1. CHOISIR LE BON PATTERN
  patternSelection: {
    reusableLogic: 'Custom Hook',
    flexibleUI: 'Compound Components',
    sharingState: 'Context + Provider',
    componentEnhancement: 'HOC (rare)',
    uiComposition: 'Children Composition',
  },
  
  // 2. COMPOUND COMPONENTS
  compoundComponents: {
    always: [
      'Utiliser Context pour partager state',
      'Nommer sub-components clairement (Component.SubComponent)',
      'Documenter les relationships',
      'Gérer accessibility (ARIA)',
    ],
    never: [
      'Props drilling (utiliser Context)',
      'Logique complexe dans sub-components',
      'State non-partagé entre siblings',
    ],
  },
  
  // 3. CUSTOM HOOKS
  customHooks: {
    always: [
      'Prefix avec "use"',
      'Un hook = une responsabilité',
      'Retourner object (pas array si > 2 values)',
      'Documenter dependencies',
    ],
    never: [
      'Side effects sans cleanup',
      'Appeler conditionnellement',
      'Oublier dependencies dans useEffect',
    ],
  },
  
  // 4. CONTEXT PATTERNS
  contextPatterns: {
    always: [
      'Custom hook pour accéder Context',
      'Error si utilisé hors Provider',
      'Memoize context value',
      'Séparer state/actions contexts si gros state',
    ],
    never: [
      'Mettre tout dans un seul Context',
      'Re-créer value object à chaque render',
      'Utiliser Context pour state qui change souvent',
    ],
  },
  
  // 5. COMPOSITION
  composition: {
    always: [
      'Favoriser composition over inheritance',
      'Children pour flexibility',
      'Props pour configuration',
      'Slots pour named sections',
    ],
    never: [
      'Deep prop drilling (> 2 niveaux)',
      'Inheritance de components',
      'Mixins (pas supporté)',
    ],
  },
};
```

---

## 🔗 Liens avec Vos Projets

### Wild Oasis

```javascript
const wildOasisPatterns = {
  // Patterns utilisés :
  used: {
    customHooks: [
      'useCabins, useBookings, useSettings',
      'useCreateCabin, useDeleteBooking',
      'Logique réutilisable extraite',
    ],
    
    context: [
      'DarkModeContext pour theming',
      'useUser pour authentication',
    ],
    
    composition: [
      'Table avec Header, Body, Row',
      'Modal avec trigger/content',
      'Form avec FormRow',
    ],
  },
  
  // Améliorations possibles :
  improvements: {
    compoundComponents: [
      'Tabs pour Dashboard (Overview/Analytics)',
      'Select pour filters (status, sort)',
      'Accordion pour settings sections',
    ],
    
    stateReducer: [
      'Booking form avec complex state',
      'Filters avec multiple options',
    ],
  },
};
```

### Fast React Pizza

```javascript
const pizzaPatterns = {
  customHooks: [
    'useCart (cart logic)',
    'useOrder (order tracking)',
    'useGeolocation',
  ],
  
  composition: [
    'Cart composé de CartItem components',
    'Menu avec MenuItem components',
  ],
  
  improvements: [
    'Compound Components pour Pizza customization',
    'useReducer pour complex cart operations',
  ],
};
```

---

## 🎯 Points Clés

```javascript
const keyTakeaways = {
  patterns: {
    renderProps: 'Logique réutilisable (old), Custom Hooks préféré',
    compoundComponents: 'UI flexible avec Context implicite',
    hoc: 'Component enhancement (rare aujourd\'hui)',
    customHooks: 'Standard moderne pour logique réutilisable',
    composition: 'Favoriser composition over props',
  },
  
  modernApproach: [
    'Custom Hooks pour logique',
    'Compound Components pour UI complexe',
    'Context + Provider pour state global',
    'Composition pour flexibility',
  ],
  
  whenToUse: {
    customHook: 'Logique réutilisable, side effects',
    compoundComponents: 'UI flexible (Select, Tabs, etc.)',
    context: 'State partagé profondément',
    hoc: 'Error boundaries, legacy code',
  },
};
```

---