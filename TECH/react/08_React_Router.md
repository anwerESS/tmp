## [**..**](./00_index.md)

# 🛣️ CHAPITRE 8 : React Router v6

## 🎯 INTRODUCTION : POURQUOI REACT ROUTER ?

### Le Problème du Routing dans les SPAs

**Sans routing :**
```javascript
function App() {
  const [page, setPage] = useState('home');
  
  return (
    <div>
      <nav>
        <button onClick={() => setPage('home')}>Home</button>
        <button onClick={() => setPage('about')}>About</button>
      </nav>
      
      {page === 'home' && <HomePage />}
      {page === 'about' && <AboutPage />}
    </div>
  );
}
```

**Problèmes :**
- ❌ Pas de URL (toujours à `/`)
- ❌ Pas de navigation browser (back/forward)
- ❌ Pas de bookmarks
- ❌ Pas de partage de liens
- ❌ Pas de deep linking

**Avec React Router :**
```javascript
import { BrowserRouter, Routes, Route, Link } from 'react-router-dom';

function App() {
  return (
    <BrowserRouter>
      <nav>
        <Link to="/">Home</Link>
        <Link to="/about">About</Link>
      </nav>
      
      <Routes>
        <Route path="/" element={<HomePage />} />
        <Route path="/about" element={<AboutPage />} />
      </Routes>
    </BrowserRouter>
  );
}
```

**Avantages :**
- ✅ URLs fonctionnelles (`/`, `/about`)
- ✅ Navigation browser native
- ✅ Bookmarks et partage
- ✅ Deep linking

---

## ✅ SETUP DE BASE

### Installation

```bash
npm install react-router-dom
```

### Configuration Minimale

```javascript
// main.jsx
import React from 'react';
import ReactDOM from 'react-dom/client';
import { BrowserRouter } from 'react-router-dom';
import App from './App';

ReactDOM.createRoot(document.getElementById('root')).render(
  <React.StrictMode>
    <BrowserRouter>
      <App />
    </BrowserRouter>
  </React.StrictMode>
);
```

```javascript
// App.jsx
import { Routes, Route } from 'react-router-dom';
import HomePage from './pages/HomePage';
import AboutPage from './pages/AboutPage';

function App() {
  return (
    <Routes>
      <Route path="/" element={<HomePage />} />
      <Route path="/about" element={<AboutPage />} />
    </Routes>
  );
}

export default App;
```

---

## 🛤️ ROUTES DE BASE

### Route Simple

```javascript
<Routes>
  <Route path="/" element={<HomePage />} />
  <Route path="/about" element={<AboutPage />} />
  <Route path="/contact" element={<ContactPage />} />
</Routes>
```

---

### Routes Dynamiques (URL Parameters)

```javascript
<Routes>
  <Route path="/products" element={<ProductsPage />} />
  <Route path="/products/:id" element={<ProductDetail />} />
  <Route path="/users/:userId" element={<UserProfile />} />
  <Route path="/blog/:slug" element={<BlogPost />} />
</Routes>

// Accéder aux params
import { useParams } from 'react-router-dom';

function ProductDetail() {
  const { id } = useParams();
  // URL: /products/123 → id = "123"
  
  return <div>Product ID: {id}</div>;
}

function UserProfile() {
  const { userId } = useParams();
  // URL: /users/john → userId = "john"
  
  return <div>User: {userId}</div>;
}
```

---

### Routes Imbriquées (Nested Routes)

```javascript
<Routes>
  <Route path="/" element={<Layout />}>
    <Route index element={<HomePage />} />
    <Route path="about" element={<AboutPage />} />
    <Route path="products" element={<ProductsLayout />}>
      <Route index element={<ProductsList />} />
      <Route path=":id" element={<ProductDetail />} />
    </Route>
  </Route>
</Routes>

// Layout.jsx
import { Outlet } from 'react-router-dom';

function Layout() {
  return (
    <div>
      <header>
        <nav>...</nav>
      </header>
      
      <main>
        <Outlet /> {/* Les routes enfants s'affichent ici */}
      </main>
      
      <footer>...</footer>
    </div>
  );
}
```

**Structure des URLs :**
- `/` → Layout + HomePage
- `/about` → Layout + AboutPage
- `/products` → Layout + ProductsLayout + ProductsList
- `/products/123` → Layout + ProductsLayout + ProductDetail

---

### Index Routes

```javascript
<Routes>
  <Route path="/" element={<Layout />}>
    {/* index = route par défaut du parent */}
    <Route index element={<HomePage />} />
    <Route path="about" element={<AboutPage />} />
  </Route>
</Routes>

// "/" affiche Layout + HomePage
// "/about" affiche Layout + AboutPage
```

---

### 404 - Route Not Found

```javascript
<Routes>
  <Route path="/" element={<HomePage />} />
  <Route path="/about" element={<AboutPage />} />
  
  {/* Catch-all route */}
  <Route path="*" element={<NotFoundPage />} />
</Routes>

// NotFoundPage.jsx
function NotFoundPage() {
  return (
    <div>
      <h1>404 - Page Not Found</h1>
      <Link to="/">Go Home</Link>
    </div>
  );
}
```

---

## 🔗 NAVIGATION

### Link Component

```javascript
import { Link } from 'react-router-dom';

function Navigation() {
  return (
    <nav>
      <Link to="/">Home</Link>
      <Link to="/about">About</Link>
      <Link to="/products/123">Product 123</Link>
      
      {/* Link avec state */}
      <Link to="/products/123" state={{ from: 'homepage' }}>
        Product 123
      </Link>
      
      {/* Link relatif */}
      <Link to="../">Back</Link>
      <Link to="./details">Details</Link>
    </nav>
  );
}
```

---

### NavLink (avec active state)

```javascript
import { NavLink } from 'react-router-dom';

function Navigation() {
  return (
    <nav>
      <NavLink
        to="/"
        className={({ isActive }) => isActive ? 'active' : ''}
      >
        Home
      </NavLink>
      
      <NavLink
        to="/about"
        style={({ isActive }) => ({
          color: isActive ? 'red' : 'black',
          fontWeight: isActive ? 'bold' : 'normal',
        })}
      >
        About
      </NavLink>
      
      {/* Avec fonction pour className */}
      <NavLink
        to="/products"
        className={({ isActive, isPending }) =>
          isPending ? "pending" : isActive ? "active" : ""
        }
      >
        Products
      </NavLink>
    </nav>
  );
}
```

---

### Programmatic Navigation (useNavigate)

```javascript
import { useNavigate } from 'react-router-dom';

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
```

---

### Navigate Component (Redirects)

```javascript
import { Navigate } from 'react-router-dom';

function ProtectedRoute({ isAuthenticated, children }) {
  if (!isAuthenticated) {
    return <Navigate to="/login" replace />;
  }
  
  return children;
}

// Usage
<Routes>
  <Route
    path="/dashboard"
    element={
      <ProtectedRoute isAuthenticated={user}>
        <DashboardPage />
      </ProtectedRoute>
    }
  />
</Routes>
```

---

## 🎣 HOOKS REACT ROUTER

### useParams

```javascript
import { useParams } from 'react-router-dom';

function ProductDetail() {
  const { id } = useParams();
  
  // URL: /products/123 → { id: "123" }
  
  return <div>Product ID: {id}</div>;
}

// Multiple params
function BlogPost() {
  const { category, slug } = useParams();
  
  // URL: /blog/tech/react-hooks → { category: "tech", slug: "react-hooks" }
  
  return <div>{category} - {slug}</div>;
}
```

---

### useSearchParams (Query Strings)

```javascript
import { useSearchParams } from 'react-router-dom';

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
```

---

### useLocation

```javascript
import { useLocation } from 'react-router-dom';

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

// Utilisation avec state
function ProductCard({ product }) {
  return (
    <Link
      to={`/products/${product.id}`}
      state={{ from: 'products-list' }}
    >
      {product.name}
    </Link>
  );
}

function ProductDetail() {
  const location = useLocation();
  const from = location.state?.from;
  
  return (
    <div>
      {from && <p>Came from: {from}</p>}
    </div>
  );
}
```

---

### useNavigate (déjà vu)

```javascript
import { useNavigate } from 'react-router-dom';

function MyComponent() {
  const navigate = useNavigate();
  
  // Navigation simple
  navigate('/about');
  
  // Navigation avec options
  navigate('/dashboard', {
    replace: true, // Pas d'historique
    state: { from: 'login' }, // Passer des données
  });
  
  // Navigation relative
  navigate('..'); // Remonter d'un niveau
  navigate('../sibling'); // Route sibling
  
  // Navigation historique
  navigate(-1); // Back
  navigate(-2); // Back x2
  navigate(1); // Forward
}
```

---

### useMatch

```javascript
import { useMatch } from 'react-router-dom';

function ProductPage() {
  const match = useMatch('/products/:id');
  
  if (match) {
    console.log(match.params.id); // "123"
    console.log(match.pathname); // "/products/123"
    console.log(match.pathnameBase); // "/products"
  }
  
  return <div>Product Page</div>;
}
```

---

## 📦 LOADERS (React Router v6.4+)

### Concept des Loaders

**Avant v6.4 :**
```javascript
function ProductDetail() {
  const { id } = useParams();
  const [product, setProduct] = useState(null);
  const [loading, setLoading] = useState(true);
  
  useEffect(() => {
    fetch(`/api/products/${id}`)
      .then(r => r.json())
      .then(setProduct)
      .finally(() => setLoading(false));
  }, [id]);
  
  if (loading) return <Spinner />;
  
  return <div>{product.name}</div>;
}
```

**Problème :** Fetch après le render → Waterfall, loading states

**Avec Loaders (v6.4+) :**
```javascript
// Loader = fonction qui charge les données AVANT le render
async function productLoader({ params }) {
  const response = await fetch(`/api/products/${params.id}`);
  if (!response.ok) throw new Error('Product not found');
  return response.json();
}

// Route avec loader
<Route
  path="/products/:id"
  element={<ProductDetail />}
  loader={productLoader}
/>

// Component utilise useLoaderData
import { useLoaderData } from 'react-router-dom';

function ProductDetail() {
  const product = useLoaderData();
  // Pas de loading state ! Les données sont déjà là
  
  return <div>{product.name}</div>;
}
```

**Avantages :**
- ✅ Données chargées AVANT le render
- ✅ Pas de loading states
- ✅ Pas de waterfalls
- ✅ Parallel data loading

---

### Setup avec createBrowserRouter

```javascript
// App.jsx
import { createBrowserRouter, RouterProvider } from 'react-router-dom';

const router = createBrowserRouter([
  {
    path: '/',
    element: <Layout />,
    loader: layoutLoader,
    children: [
      {
        index: true,
        element: <HomePage />,
      },
      {
        path: 'products',
        element: <ProductsPage />,
        loader: productsLoader,
      },
      {
        path: 'products/:id',
        element: <ProductDetail />,
        loader: productLoader,
      },
    ],
  },
]);

function App() {
  return <RouterProvider router={router} />;
}
```

---

### Exemple Complet de Loader

```javascript
// loaders/productLoader.js
export async function productLoader({ params, request }) {
  const { id } = params;
  
  // URL: /products/123?view=detailed
  const url = new URL(request.url);
  const view = url.searchParams.get('view');
  
  const response = await fetch(`/api/products/${id}?view=${view}`);
  
  if (!response.ok) {
    throw new Response('Product not found', { status: 404 });
  }
  
  return response.json();
}

// routes
{
  path: 'products/:id',
  element: <ProductDetail />,
  loader: productLoader,
}

// Component
import { useLoaderData } from 'react-router-dom';

function ProductDetail() {
  const product = useLoaderData();
  
  return (
    <div>
      <h1>{product.name}</h1>
      <p>{product.description}</p>
      <p>${product.price}</p>
    </div>
  );
}
```

---

### Loaders avec Multiple Fetches

```javascript
export async function dashboardLoader() {
  // ✅ Parallel fetching
  const [user, stats, notifications] = await Promise.all([
    fetch('/api/user').then(r => r.json()),
    fetch('/api/stats').then(r => r.json()),
    fetch('/api/notifications').then(r => r.json()),
  ]);
  
  return { user, stats, notifications };
}

// Component
function Dashboard() {
  const { user, stats, notifications } = useLoaderData();
  
  return (
    <div>
      <UserProfile user={user} />
      <Stats data={stats} />
      <NotificationsList items={notifications} />
    </div>
  );
}
```

---

### Error Handling avec Loaders

```javascript
// Loader avec error
export async function productLoader({ params }) {
  const response = await fetch(`/api/products/${params.id}`);
  
  if (response.status === 404) {
    throw new Response('Product not found', { status: 404 });
  }
  
  if (!response.ok) {
    throw new Response('Failed to load product', { status: 500 });
  }
  
  return response.json();
}

// Route avec errorElement
{
  path: 'products/:id',
  element: <ProductDetail />,
  loader: productLoader,
  errorElement: <ErrorPage />,
}

// ErrorPage.jsx
import { useRouteError } from 'react-router-dom';

function ErrorPage() {
  const error = useRouteError();
  
  return (
    <div>
      <h1>Oops!</h1>
      <p>Status: {error.status}</p>
      <p>{error.statusText || error.message}</p>
    </div>
  );
}
```

---

## ✍️ ACTIONS (React Router v6.4+)

### Concept des Actions

**Actions = Mutations de données (POST, PUT, DELETE)**

```javascript
// Action function
export async function createProductAction({ request }) {
  const formData = await request.formData();
  
  const product = {
    name: formData.get('name'),
    price: formData.get('price'),
  };
  
  const response = await fetch('/api/products', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(product),
  });
  
  if (!response.ok) {
    return { error: 'Failed to create product' };
  }
  
  return redirect('/products');
}

// Route avec action
{
  path: 'products/new',
  element: <NewProductForm />,
  action: createProductAction,
}

// Form avec action
import { Form } from 'react-router-dom';

function NewProductForm() {
  return (
    <Form method="post">
      <input name="name" placeholder="Product name" />
      <input name="price" type="number" placeholder="Price" />
      <button type="submit">Create</button>
    </Form>
  );
}
```

**Avantages :**
- ✅ Pas de `e.preventDefault()`
- ✅ Revalidation automatique des loaders
- ✅ Optimistic UI possible
- ✅ Error handling intégré

---

### Exemple CRUD Complet

```javascript
// routes/products.js
import { redirect } from 'react-router-dom';

// LOADER (Read)
export async function productsLoader() {
  const response = await fetch('/api/products');
  return response.json();
}

// ACTION CREATE
export async function createProductAction({ request }) {
  const formData = await request.formData();
  const product = Object.fromEntries(formData);
  
  await fetch('/api/products', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(product),
  });
  
  return redirect('/products');
}

// ACTION UPDATE
export async function updateProductAction({ request, params }) {
  const formData = await request.formData();
  const updates = Object.fromEntries(formData);
  
  await fetch(`/api/products/${params.id}`, {
    method: 'PUT',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(updates),
  });
  
  return redirect(`/products/${params.id}`);
}

// ACTION DELETE
export async function deleteProductAction({ params }) {
  await fetch(`/api/products/${params.id}`, {
    method: 'DELETE',
  });
  
  return redirect('/products');
}

// Routes
export const productsRoutes = {
  path: 'products',
  children: [
    {
      index: true,
      element: <ProductsList />,
      loader: productsLoader,
    },
    {
      path: 'new',
      element: <NewProductForm />,
      action: createProductAction,
    },
    {
      path: ':id/edit',
      element: <EditProductForm />,
      loader: productLoader,
      action: updateProductAction,
    },
    {
      path: ':id/delete',
      action: deleteProductAction,
    },
  ],
};
```

---

### useActionData (récupérer la réponse de l'action)

```javascript
// Action avec erreur
export async function loginAction({ request }) {
  const formData = await request.formData();
  const credentials = Object.fromEntries(formData);
  
  const response = await fetch('/api/login', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(credentials),
  });
  
  if (!response.ok) {
    return { error: 'Invalid credentials' };
  }
  
  const user = await response.json();
  return redirect('/dashboard');
}

// Form avec error handling
import { Form, useActionData } from 'react-router-dom';

function LoginForm() {
  const actionData = useActionData();
  
  return (
    <Form method="post">
      {actionData?.error && (
        <div className="error">{actionData.error}</div>
      )}
      
      <input name="email" type="email" />
      <input name="password" type="password" />
      <button type="submit">Login</button>
    </Form>
  );
}
```

---

### useNavigation (états de soumission)

```javascript
import { Form, useNavigation } from 'react-router-dom';

function NewProductForm() {
  const navigation = useNavigation();
  
  const isSubmitting = navigation.state === 'submitting';
  const isLoading = navigation.state === 'loading';
  
  return (
    <Form method="post">
      <input name="name" placeholder="Product name" />
      <input name="price" type="number" />
      
      <button type="submit" disabled={isSubmitting}>
        {isSubmitting ? 'Creating...' : 'Create Product'}
      </button>
      
      {isLoading && <Spinner />}
    </Form>
  );
}
```

---

### useFetcher (actions sans navigation)

```javascript
import { useFetcher } from 'react-router-dom';

function ToggleFavorite({ productId, isFavorite }) {
  const fetcher = useFetcher();
  
  // fetcher.state: "idle" | "submitting" | "loading"
  const isToggling = fetcher.state !== 'idle';
  
  return (
    <fetcher.Form method="post" action="/api/toggle-favorite">
      <input type="hidden" name="productId" value={productId} />
      <input type="hidden" name="isFavorite" value={!isFavorite} />
      
      <button type="submit" disabled={isToggling}>
        {isFavorite ? '❤️' : '🤍'}
      </button>
    </fetcher.Form>
  );
}
// Soumission SANS changer de page !
```

---

## 🔐 PROTECTED ROUTES

### Pattern de Base

```javascript
import { Navigate, Outlet } from 'react-router-dom';

function ProtectedRoute({ isAuthenticated, redirectPath = '/login' }) {
  if (!isAuthenticated) {
    return <Navigate to={redirectPath} replace />;
  }
  
  return <Outlet />;
}

// Usage
const router = createBrowserRouter([
  {
    path: '/login',
    element: <LoginPage />,
  },
  {
    element: <ProtectedRoute isAuthenticated={user} />,
    children: [
      {
        path: '/dashboard',
        element: <Dashboard />,
      },
      {
        path: '/profile',
        element: <Profile />,
      },
    ],
  },
]);
```

---

### Avec Loader (vérification côté serveur)

```javascript
// loaders/auth.js
export async function protectedLoader() {
  const user = await getCurrentUser();
  
  if (!user) {
    throw redirect('/login');
  }
  
  return user;
}

// Routes
{
  path: '/dashboard',
  element: <Dashboard />,
  loader: protectedLoader,
}
```

---

### Avec Redirect vers Page Originale

```javascript
function ProtectedRoute({ isAuthenticated }) {
  const location = useLocation();
  
  if (!isAuthenticated) {
    // Sauvegarder l'URL originale
    return <Navigate to="/login" state={{ from: location }} replace />;
  }
  
  return <Outlet />;
}

// LoginPage après succès
function LoginPage() {
  const navigate = useNavigate();
  const location = useLocation();
  
  const from = location.state?.from?.pathname || '/';
  
  const handleLogin = async () => {
    await login();
    navigate(from, { replace: true }); // Retour à la page originale
  };
  
  return <form onSubmit={handleLogin}>...</form>;
}
```

---

## 🎨 PATTERNS AVANCÉS

### Layout Partagé

```javascript
// AppLayout.jsx
import { Outlet } from 'react-router-dom';

function AppLayout() {
  return (
    <div className="app">
      <Header />
      <Sidebar />
      
      <main>
        <Outlet /> {/* Routes enfants */}
      </main>
      
      <Footer />
    </div>
  );
}

// Routes
{
  path: '/',
  element: <AppLayout />,
  children: [
    { index: true, element: <HomePage /> },
    { path: 'about', element: <AboutPage /> },
    { path: 'contact', element: <ContactPage /> },
  ],
}
```

---

### Breadcrumbs

```javascript
import { useMatches, Link } from 'react-router-dom';

function Breadcrumbs() {
  const matches = useMatches();
  
  const crumbs = matches
    .filter(match => match.handle?.crumb)
    .map(match => match.handle.crumb(match.data));
  
  return (
    <nav>
      {crumbs.map((crumb, index) => (
        <span key={index}>
          {index > 0 && ' > '}
          {crumb}
        </span>
      ))}
    </nav>
  );
}

// Routes avec handle
{
  path: 'products/:id',
  element: <ProductDetail />,
  loader: productLoader,
  handle: {
    crumb: (data) => <Link to={`/products/${data.id}`}>{data.name}</Link>,
  },
}
```

---

### Modal Routes

```javascript
function App() {
  const location = useLocation();
  const background = location.state?.backgroundLocation;
  
  return (
    <>
      <Routes location={background || location}>
        <Route path="/" element={<HomePage />} />
        <Route path="/products" element={<ProductsList />} />
        <Route path="/products/:id" element={<ProductDetail />} />
      </Routes>
      
      {background && (
        <Routes>
          <Route path="/products/:id" element={<ProductModal />} />
        </Routes>
      )}
    </>
  );
}

// Link qui ouvre en modal
<Link
  to={`/products/${product.id}`}
  state={{ backgroundLocation: location }}
>
  {product.name}
</Link>
```

---

## ⚠️ PIÈGES COURANTS

### Piège 1 : Oublier `replace` dans Navigate

```javascript
// ❌ MAUVAIS : Crée un historique infini
function ProtectedRoute({ isAuth }) {
  if (!isAuth) {
    return <Navigate to="/login" />; // Sans replace
  }
  return <Outlet />;
}
// User → /dashboard → redirect /login → Back → /dashboard → redirect /login → ...

// ✅ BON
return <Navigate to="/login" replace />;
```

---

### Piège 2 : useParams dans le parent

```javascript
// ❌ PROBLÈME
function ProductPage() {
  const { id } = useParams(); // undefined !
  
  return (
    <Routes>
      <Route path=":id" element={<ProductDetail />} />
    </Routes>
  );
}

// ✅ SOLUTION : useParams dans le component de la route
function ProductDetail() {
  const { id } = useParams(); // "123"
  return <div>{id}</div>;
}
```

---

### Piège 3 : Link au lieu de NavLink

```javascript
// ❌ Pas de style actif
<Link to="/products">Products</Link>

// ✅ Style actif automatique
<NavLink
  to="/products"
  className={({ isActive }) => isActive ? 'active' : ''}
>
  Products
</NavLink>
```

---

### Piège 4 : Paths absolus vs relatifs

```javascript
// Structure :
// /products
// /products/:id
// /products/:id/edit

// ❌ MAUVAIS : Path absolu dans nested route
<Route path="/products" element={<ProductsLayout />}>
  <Route path="/products/:id" element={<Detail />} /> {/* Redondant */}
</Route>

// ✅ BON : Path relatif
<Route path="/products" element={<ProductsLayout />}>
  <Route path=":id" element={<Detail />} /> {/* Relatif au parent */}
  <Route path=":id/edit" element={<Edit />} />
</Route>
```

---

## 🎯 QUESTIONS D'ENTRETIEN FRÉQUENTES

### Q1 : Différence entre Link et NavLink ?

**Réponse :**

**Link** = Navigation basique sans état actif
**NavLink** = Navigation avec indicateur actif (isActive)

```javascript
// Link
<Link to="/about">About</Link>
// Render : <a href="/about">About</a>

// NavLink
<NavLink
  to="/about"
  className={({ isActive }) => isActive ? 'active' : ''}
>
  About
</NavLink>
// Render : <a href="/about" class="active">About</a> (si actif)
```

**Utilisez NavLink pour :**
- Menus de navigation
- Tabs
- Sidebar links

---

### Q2 : Comment passer des données entre routes ?

**Réponse : 3 méthodes**

**1. URL Parameters**
```javascript
// Naviguer
navigate(`/products/${productId}`);

// Récupérer
const { productId } = useParams();
```

**2. Query Strings**
```javascript
// Naviguer
navigate('/products?category=electronics&sort=price');

// Récupérer
const [searchParams] = useSearchParams();
const category = searchParams.get('category');
```

**3. Location State**
```javascript
// Naviguer
navigate('/product/123', {
  state: { from: 'homepage', product: data }
});

// Récupérer
const location = useLocation();
const { from, product } = location.state || {};
```

**Différences :**

| Méthode | Visible URL | Bookmarkable | Persistent |
|---------|-------------|--------------|------------|
| **URL Params** | ✅ Oui | ✅ Oui | ✅ Oui |
| **Query Strings** | ✅ Oui | ✅ Oui | ✅ Oui |
| **Location State** | ❌ Non | ❌ Non | ❌ Non (refresh = perdu) |

---

### Q3 : Quelle est la différence entre useNavigate et Navigate ?

**Réponse :**

**useNavigate** = Hook pour navigation programmatique
**Navigate** = Component pour redirects déclaratifs

```javascript
// useNavigate (dans event handlers, après logique)
function LoginForm() {
  const navigate = useNavigate();
  
  const handleSubmit = async (e) => {
    e.preventDefault();
    const success = await login();
    
    if (success) {
      navigate('/dashboard'); // Navigation conditionnelle
    }
  };
  
  return <form onSubmit={handleSubmit}>...</form>;
}

// Navigate (redirects immédiats dans le render)
function ProtectedRoute({ isAuth }) {
  if (!isAuth) {
    return <Navigate to="/login" replace />; // Redirect immédiat
  }
  return <Outlet />;
}
```

**Règle :**
- ✅ useNavigate → Dans fonctions, après async
- ✅ Navigate → Dans render, redirects simples

---

### Q4 : Comment fonctionne Outlet ?

**Réponse :**

**Outlet** = Placeholder pour les routes enfants dans les routes imbriquées.

```javascript
// Structure
<Route path="/" element={<Layout />}>
  <Route path="home" element={<HomePage />} />
  <Route path="about" element={<AboutPage />} />
</Route>

// Layout.jsx
function Layout() {
  return (
    <div>
      <nav>...</nav>
      <Outlet /> {/* HomePage ou AboutPage s'affiche ici */}
      <footer>...</footer>
    </div>
  );
}

// URLs :
// /home → Layout + HomePage
// /about → Layout + AboutPage
```

**Outlet avec Context**
```javascript
function Layout() {
  const [theme, setTheme] = useState('dark');
  
  return (
    <div>
      <nav>...</nav>
      <Outlet context={{ theme, setTheme }} />
      <footer>...</footer>
    </div>
  );
}

// Enfant
function HomePage() {
  const { theme } = useOutletContext();
  return <div className={theme}>Home</div>;
}
```

---

### Q5 : Différence entre loaders et useEffect pour fetch ?

**Réponse :**

**Avant loaders (useEffect) :**
```javascript
function ProductDetail() {
  const { id } = useParams();
  const [product, setProduct] = useState(null);
  const [loading, setLoading] = useState(true);
  
  useEffect(() => {
    setLoading(true);
    fetch(`/api/products/${id}`)
      .then(r => r.json())
      .then(setProduct)
      .finally(() => setLoading(false));
  }, [id]);
  
  if (loading) return <Spinner />;
  return <div>{product.name}</div>;
}
```

**Problèmes :**
- ❌ Fetch APRÈS le render
- ❌ Loading states partout
- ❌ Waterfalls (parent fetch → render → child fetch)

**Avec loaders (v6.4+) :**
```javascript
// Loader
export async function productLoader({ params }) {
  return fetch(`/api/products/${params.id}`).then(r => r.json());
}

// Route
{
  path: 'products/:id',
  element: <ProductDetail />,
  loader: productLoader,
}

// Component
function ProductDetail() {
  const product = useLoaderData();
  return <div>{product.name}</div>; // Pas de loading !
}
```

**Avantages loaders :**
- ✅ Fetch AVANT le render
- ✅ Pas de loading states
- ✅ Parallel fetching
- ✅ Revalidation automatique

---

### Q6 : Comment gérer les 404 ?

**Réponse : 2 approches**

**1. Catch-all route (`*`)**
```javascript
<Routes>
  <Route path="/" element={<HomePage />} />
  <Route path="/about" element={<AboutPage />} />
  <Route path="*" element={<NotFound />} />
</Routes>
```

**2. errorElement (avec loaders)**
```javascript
// Loader qui throw
export async function productLoader({ params }) {
  const res = await fetch(`/api/products/${params.id}`);
  
  if (res.status === 404) {
    throw new Response('Not Found', { status: 404 });
  }
  
  return res.json();
}

// Route
{
  path: 'products/:id',
  element: <ProductDetail />,
  loader: productLoader,
  errorElement: <ErrorPage />,
}

// ErrorPage
function ErrorPage() {
  const error = useRouteError();
  
  if (error.status === 404) {
    return <NotFound />;
  }
  
  return <GeneralError error={error} />;
}
```

---

### Q7 : Comment faire du lazy loading de routes ?

**Réponse :**

```javascript
import { lazy, Suspense } from 'react';
import { createBrowserRouter } from 'react-router-dom';

// Lazy load components
const HomePage = lazy(() => import('./pages/HomePage'));
const Dashboard = lazy(() => import('./pages/Dashboard'));
const AdminPanel = lazy(() => import('./pages/AdminPanel'));

const router = createBrowserRouter([
  {
    path: '/',
    element: (
      <Suspense fallback={<Spinner />}>
        <HomePage />
      </Suspense>
    ),
  },
  {
    path: '/dashboard',
    element: (
      <Suspense fallback={<Spinner />}>
        <Dashboard />
      </Suspense>
    ),
  },
  {
    path: '/admin',
    element: (
      <Suspense fallback={<Spinner />}>
        <AdminPanel />
      </Suspense>
    ),
  },
]);
```

**Avec Layout wrapper**
```javascript
function SuspenseLayout() {
  return (
    <Suspense fallback={<PageSpinner />}>
      <Outlet />
    </Suspense>
  );
}

const router = createBrowserRouter([
  {
    element: <SuspenseLayout />,
    children: [
      {
        path: '/',
        element: <HomePage />,
      },
      {
        path: '/dashboard',
        element: <Dashboard />,
      },
    ],
  },
]);
```

---

### Q8 : Comment créer des routes dynamiques ?

**Réponse :**

```javascript
// Routes générées depuis data
const CATEGORIES = ['electronics', 'books', 'clothing'];

const router = createBrowserRouter([
  {
    path: '/',
    element: <Layout />,
    children: [
      {
        index: true,
        element: <HomePage />,
      },
      // Routes dynamiques
      ...CATEGORIES.map(category => ({
        path: `/category/${category}`,
        element: <CategoryPage />,
        loader: ({ params }) => categoryLoader(params.category),
      })),
    ],
  },
]);

// Ou avec une seule route paramétrique
{
  path: '/category/:categoryName',
  element: <CategoryPage />,
  loader: categoryLoader,
}
```

---

### Q9 : Comment tester les routes ?

**Réponse :**

```javascript
import { render, screen } from '@testing-library/react';
import { MemoryRouter, Routes, Route } from 'react-router-dom';
import { userEvent } from '@testing-library/user-event';

test('navigates to about page', async () => {
  const user = userEvent.setup();
  
  render(
    <MemoryRouter initialEntries={['/']}>
      <Routes>
        <Route path="/" element={<HomePage />} />
        <Route path="/about" element={<AboutPage />} />
      </Routes>
    </MemoryRouter>
  );
  
  const aboutLink = screen.getByText('About');
  await user.click(aboutLink);
  
  expect(screen.getByText('About Us')).toBeInTheDocument();
});

// Test avec params
test('displays product detail', () => {
  render(
    <MemoryRouter initialEntries={['/products/123']}>
      <Routes>
        <Route path="/products/:id" element={<ProductDetail />} />
      </Routes>
    </MemoryRouter>
  );
  
  expect(screen.getByText('Product ID: 123')).toBeInTheDocument();
});
```

---

### Q10 : Différence entre BrowserRouter et HashRouter ?

**Réponse :**

**BrowserRouter** (recommandé)
```javascript
// URLs propres
https://example.com/products/123
https://example.com/about

// Utilise HTML5 History API
// Nécessite config serveur (fallback vers index.html)
```

**HashRouter**
```javascript
// URLs avec #
https://example.com/#/products/123
https://example.com/#/about

// Fonctionne sans config serveur
// Moins SEO-friendly
```

**Quand utiliser HashRouter ?**
- ✅ Sites statiques sans serveur
- ✅ Déploiement GitHub Pages
- ✅ Environnements sans contrôle serveur

**Quand utiliser BrowserRouter ?**
- ✅ Applications modernes (recommandé)
- ✅ Avec serveur (Node, Nginx)
- ✅ Meilleur SEO

**Config serveur pour BrowserRouter :**
```nginx
# Nginx
location / {
  try_files $uri $uri/ /index.html;
}
```

```javascript
// Node/Express
app.get('*', (req, res) => {
  res.sendFile(path.join(__dirname, 'build', 'index.html'));
});
```

---

## 🔗 LIENS AVEC VOS PROJETS

### fast-react-pizza

**Ce que vous avez utilisé :**

```javascript
// Routes
<Routes>
  <Route path="/" element={<Home />} />
  <Route path="/menu" element={<Menu />} />
  <Route path="/cart" element={<Cart />} />
  <Route path="/order/new" element={<CreateOrder />} />
  <Route path="/order/:orderId" element={<Order />} />
</Routes>

// Loader pour order
export async function loader({ params }) {
  const order = await getOrder(params.orderId);
  return order;
}

// Component
function Order() {
  const order = useLoaderData();
  return <div>Order #{order.id}</div>;
}

// Action pour créer commande
export async function action({ request }) {
  const formData = await request.formData();
  const order = {
    customer: formData.get('customer'),
    phone: formData.get('phone'),
    cart: JSON.parse(formData.get('cart')),
  };
  
  const newOrder = await createOrder(order);
  return redirect(`/order/${newOrder.id}`);
}
```

**Points à mentionner :**
- ✅ "J'ai utilisé React Router v6.4 avec loaders et actions"
- ✅ "Loader pour charger les données de commande avant le render"
- ✅ "Action pour soumettre les commandes sans gérer l'état manuellement"
- ✅ "useNavigate pour rediriger après création de commande"

---

### wild-oasis

**Routing de l'application :**

```javascript
// Routes protégées
const router = createBrowserRouter([
  {
    path: '/login',
    element: <Login />,
  },
  {
    element: <ProtectedRoute />,
    children: [
      {
        path: '/',
        element: <AppLayout />,
        children: [
          { index: true, element: <Dashboard /> },
          { path: 'bookings', element: <Bookings /> },
          { path: 'bookings/:bookingId', element: <BookingDetail /> },
          { path: 'cabins', element: <Cabins /> },
          { path: 'users', element: <Users /> },
          { path: 'settings', element: <Settings /> },
        ],
      },
    ],
  },
]);

// Protected Route
function ProtectedRoute() {
  const { isAuthenticated } = useAuth();
  
  if (!isAuthenticated) {
    return <Navigate to="/login" replace />;
  }
  
  return <Outlet />;
}
```

**Points à mentionner :**
- ✅ "Routes imbriquées avec Layout partagé"
- ✅ "Protected routes avec redirection vers login"
- ✅ "Navigation dans l'application de gestion"

---

### worldwise

**Routing avec map :**

```javascript
<Routes>
  <Route path="/" element={<Homepage />} />
  <Route path="/product" element={<Product />} />
  <Route path="/pricing" element={<Pricing />} />
  <Route path="/login" element={<Login />} />
  
  <Route path="/app" element={<AppLayout />}>
    <Route index element={<Navigate to="cities" replace />} />
    <Route path="cities" element={<CitiesList />} />
    <Route path="cities/:id" element={<City />} />
    <Route path="countries" element={<CountriesList />} />
    <Route path="form" element={<Form />} />
  </Route>
</Routes>

// Accéder à l'ID
function City() {
  const { id } = useParams();
  const city = cities.find(c => c.id === id);
  
  return <div>{city.name}</div>;
}
```

---

## 📚 BEST PRACTICES

### 1. Organisation des Routes

```javascript
// ❌ Tout dans App.jsx
function App() {
  return (
    <Routes>
      {/* 50 routes ici... */}
    </Routes>
  );
}

// ✅ Routes séparées par feature
// routes/products.js
export const productsRoutes = {
  path: 'products',
  children: [
    { index: true, element: <ProductsList /> },
    { path: ':id', element: <ProductDetail /> },
    { path: 'new', element: <NewProduct /> },
  ],
};

// routes/index.js
import { productsRoutes } from './products';
import { usersRoutes } from './users';

export const router = createBrowserRouter([
  {
    path: '/',
    element: <Layout />,
    children: [
      { index: true, element: <HomePage /> },
      productsRoutes,
      usersRoutes,
    ],
  },
]);
```

---

### 2. Loaders et Actions Séparés

```javascript
// ❌ Loaders inline
{
  path: 'products/:id',
  element: <ProductDetail />,
  loader: async ({ params }) => {
    return fetch(`/api/products/${params.id}`).then(r => r.json());
  },
}

// ✅ Loaders dans fichiers séparés
// loaders/products.js
export async function productLoader({ params }) {
  const response = await fetch(`/api/products/${params.id}`);
  if (!response.ok) throw new Response('Not found', { status: 404 });
  return response.json();
}

export async function productsListLoader() {
  return fetch('/api/products').then(r => r.json());
}

// routes/products.js
import { productLoader, productsListLoader } from '../loaders/products';

export const productsRoutes = {
  path: 'products',
  children: [
    {
      index: true,
      element: <ProductsList />,
      loader: productsListLoader,
    },
    {
      path: ':id',
      element: <ProductDetail />,
      loader: productLoader,
    },
  ],
};
```

---

### 3. Paths Constants

```javascript
// ❌ Strings partout
navigate('/products/123');
<Link to="/products/123">Product</Link>

// ✅ Constants
// routes/paths.js
export const PATHS = {
  HOME: '/',
  PRODUCTS: '/products',
  PRODUCT_DETAIL: (id) => `/products/${id}`,
  CART: '/cart',
  LOGIN: '/login',
};

// Usage
navigate(PATHS.PRODUCT_DETAIL(123));
<Link to={PATHS.PRODUCTS}>Products</Link>
```

---

### 4. Error Boundaries Globaux

```javascript
const router = createBrowserRouter([
  {
    path: '/',
    element: <Layout />,
    errorElement: <GlobalError />, // Catch toutes les erreurs
    children: [
      // Routes...
    ],
  },
]);

function GlobalError() {
  const error = useRouteError();
  
  return (
    <div className="error-page">
      <h1>Oops! Something went wrong</h1>
      <p>{error.statusText || error.message}</p>
      <Link to="/">Go Home</Link>
    </div>
  );
}
```

---

### 5. Typed Routes (TypeScript)

```typescript
// types/routes.ts
export type RouteParams = {
  productId: string;
  userId: string;
  orderId: string;
};

// Component
function ProductDetail() {
  const { productId } = useParams<{ productId: string }>();
  // productId est typé comme string
  
  return <div>Product {productId}</div>;
}
```

---

## 📊 REACT ROUTER v6 vs v5

| Feature | v5 | v6 |
|---------|----|----|
| **Routes** | `<Switch>` | `<Routes>` |
| **Route matching** | First match | Best match |
| **Nested routes** | Manual | Déclaratif |
| **Relative paths** | ❌ Non | ✅ Oui |
| **Loaders** | ❌ Non | ✅ Oui (v6.4+) |
| **Actions** | ❌ Non | ✅ Oui (v6.4+) |
| **useNavigate** | `useHistory` | `useNavigate` |
| **Size** | ~20kb | ~13kb |

**Migration v5 → v6 :**
```javascript
// v5
<Switch>
  <Route exact path="/" component={HomePage} />
  <Route path="/about" component={AboutPage} />
</Switch>

// v6
<Routes>
  <Route path="/" element={<HomePage />} />
  <Route path="/about" element={<AboutPage />} />
</Routes>
```

---

## 📚 RÉCAPITULATIF FINAL

### Ce qu'il FAUT savoir

**Setup :**
- ✅ BrowserRouter vs HashRouter
- ✅ Routes, Route, Outlet
- ✅ createBrowserRouter (v6.4+)

**Navigation :**
- ✅ Link vs NavLink
- ✅ useNavigate (programmatic)
- ✅ Navigate (redirects)

**Hooks :**
- ✅ useParams (URL params)
- ✅ useSearchParams (query strings)
- ✅ useLocation (location state)
- ✅ useLoaderData (loaders)
- ✅ useActionData (actions)

**Advanced :**
- ✅ Loaders (data loading)
- ✅ Actions (mutations)
- ✅ Protected routes
- ✅ Error handling
- ✅ Nested routes

---