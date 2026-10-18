## [**..**](./00_index.md)

# 📦 CHAPITRE 5 : Redux & Redux Toolkit (VERSION APPROFONDIE)

## 🎯 INTRODUCTION : POURQUOI REDUX ?

### Le Problème du State Management

Imaginez une application e-commerce avec :
- Un **cart** (panier)
- Un **user** connecté
- Des **products** chargés depuis une API
- Un **theme** (dark/light)
- Des **notifications**

**Sans Redux :**
```
App
 ├─ Header (besoin du user, cart count)
 ├─ Sidebar (besoin du theme)
 └─ ProductList
     └─ ProductCard
         └─ AddToCart (besoin de dispatch au cart)
             └─ Button (5 niveaux de props drilling !)
```

**Problèmes :**
- ❌ **Props drilling** : passer les props à travers 5 niveaux
- ❌ **State dupliqué** : même data à plusieurs endroits
- ❌ **Synchronisation difficile** : updates complexes
- ❌ **Code répétitif** : lifting state up partout

**Avec Redux :**
```
         GLOBAL STORE
              ↓
    ┌─────────┼─────────┐
    ↓         ↓         ↓
  Header  Sidebar  AddToCart
```

---

## ✅ LES 3 PRINCIPES FONDAMENTAUX DE REDUX

### 1. Single Source of Truth

```javascript
// ❌ MAUVAIS : State éparpillé
function Header() {
  const [user, setUser] = useState(null);
}
function Sidebar() {
  const [user, setUser] = useState(null); // Dupliqué !
}

// ✅ BON : Un seul store
const store = {
  user: { name: 'John', id: 1 },
  cart: { items: [], total: 0 },
  theme: 'dark',
};
```

**Avantages :**
- ✅ Pas de désynchronisation
- ✅ Debugging facile (voir tout le state)
- ✅ Time-travel debugging
- ✅ Persistence facile (localStorage)

---

### 2. State is Read-Only

```javascript
// ❌ MAUVAIS : Mutation directe
store.user.name = 'Jane'; // NON !
cart.items.push(newItem); // NON !

// ✅ BON : Dispatch d'actions
dispatch({ type: 'user/updateName', payload: 'Jane' });
dispatch({ type: 'cart/addItem', payload: newItem });
```

**Pourquoi c'est important ?**
- ✅ **Traçabilité** : on sait exactement ce qui a changé
- ✅ **Debugging** : DevTools montrent chaque action
- ✅ **Undo/Redo** : possible avec un historique d'actions
- ✅ **Predictable** : pas de changements surprises

---

### 3. Changes Made with Pure Functions (Reducers)

```javascript
// Pure function = même input → même output, pas de side effects

// ❌ IMPURE
let counter = 0;
function increment() {
  counter++; // Mutation externe !
  return counter;
}

// ❌ IMPURE
function reducer(state) {
  state.count++; // Mutation !
  return state;
}

// ✅ PURE
function reducer(state, action) {
  return { ...state, count: state.count + 1 }; // Nouveau objet
}
```

**Règles des Pure Functions :**
1. Pas de mutations des arguments
2. Pas d'API calls ou side effects
3. Pas de functions non-déterministes (Date.now(), Math.random())

---

## 🏗️ ARCHITECTURE REDUX COMPLÈTE

### Le Data Flow Redux (One-Way)

```
┌─────────────────────────────────────────────┐
│  1. USER INTERACTION (click, input, etc.)   │
└──────────────────┬──────────────────────────┘
                   ↓
┌──────────────────────────────────────────────┐
│  2. DISPATCH ACTION                          │
│     dispatch({ type: 'cart/addItem',         │
│                payload: { id: 1, qty: 2 } }) │
└──────────────────┬───────────────────────────┘
                   ↓
┌──────────────────────────────────────────────┐
│  3. REDUCER CALCULE NEW STATE                │
│     reducer(currentState, action)            │
│     → returns newState                       │
└──────────────────┬───────────────────────────┘
                   ↓
┌──────────────────────────────────────────────┐
│  4. STORE UPDATE                             │
│     store.state = newState                   │
└──────────────────┬───────────────────────────┘
                   ↓
┌──────────────────────────────────────────────┐
│  5. SUBSCRIBERS NOTIFIED                     │
│     Components re-render avec nouveau state  │
└──────────────────────────────────────────────┘
```

---

## 🚀 REDUX TOOLKIT (RTK) : LA SOLUTION MODERNE

### Comparaison Redux Classique vs RTK

**Redux classique (50+ lignes) :**

```javascript
// 1. Action types
const DEPOSIT = 'account/deposit';
const WITHDRAW = 'account/withdraw';

// 2. Action creators
function deposit(amount) {
  return { type: DEPOSIT, payload: amount };
}

// 3. Reducer
function accountReducer(state = { balance: 0 }, action) {
  switch (action.type) {
    case DEPOSIT:
      return { ...state, balance: state.balance + action.payload };
    case WITHDRAW:
      return { ...state, balance: state.balance - action.payload };
    default:
      return state;
  }
}

// 4. Store setup
import { createStore, combineReducers, applyMiddleware } from 'redux';
import thunk from 'redux-thunk';
import { composeWithDevTools } from 'redux-devtools-extension';

const store = createStore(
  combineReducers({ account: accountReducer }),
  composeWithDevTools(applyMiddleware(thunk))
);
```

**Redux Toolkit (15 lignes) :**

```javascript
import { createSlice, configureStore } from '@reduxjs/toolkit';

const accountSlice = createSlice({
  name: 'account',
  initialState: { balance: 0 },
  reducers: {
    deposit(state, action) {
      state.balance += action.payload; // Immer !
    },
    withdraw(state, action) {
      state.balance -= action.payload;
    },
  },
});

export const { deposit, withdraw } = accountSlice.actions;

const store = configureStore({
  reducer: { account: accountSlice.reducer },
});
```

**✅ 70% de code en moins !**

---

## 💡 SETUP COMPLET D'UNE APP REDUX

### Structure du Projet

```
src/
├── store.js                    # Configuration du store
├── features/
│   ├── account/
│   │   └── accountSlice.js    # Slice account
│   ├── cart/
│   │   └── cartSlice.js       # Slice cart
│   └── user/
│       └── userSlice.js       # Slice user
└── App.jsx
```

---

### 1. Configuration du Store

```javascript
// store.js
import { configureStore } from '@reduxjs/toolkit';
import accountReducer from './features/account/accountSlice';
import cartReducer from './features/cart/cartSlice';
import userReducer from './features/user/userSlice';

const store = configureStore({
  reducer: {
    account: accountReducer,
    cart: cartReducer,
    user: userReducer,
  },
  // Middleware et DevTools sont auto-configurés !
});

export default store;

// Structure du state résultant :
// {
//   account: { balance: 0, loan: 0, ... },
//   cart: { items: [], total: 0 },
//   user: { name: '', email: '' }
// }
```

---

### 2. Provider dans l'App

```javascript
// main.jsx ou index.js
import React from 'react';
import ReactDOM from 'react-dom/client';
import { Provider } from 'react-redux';
import store from './store';
import App from './App';

ReactDOM.createRoot(document.getElementById('root')).render(
  <React.StrictMode>
    <Provider store={store}>
      <App />
    </Provider>
  </React.StrictMode>
);
```

---

## 📝 createSlice EN DÉTAIL

### Anatomie Complète d'un Slice

```javascript
// cartSlice.js
import { createSlice } from '@reduxjs/toolkit';

const cartSlice = createSlice({
  // 1. NOM (préfixe pour action types)
  name: 'cart',
  
  // 2. STATE INITIAL
  initialState: {
    items: [],
    total: 0,
    isLoading: false,
    error: null,
  },
  
  // 3. REDUCERS (actions synchrones)
  reducers: {
    // Reducer simple
    addItem(state, action) {
      state.items.push(action.payload);
      state.total += action.payload.price * action.payload.quantity;
    },
    
    // Reducer avec logique conditionnelle
    removeItem(state, action) {
      const index = state.items.findIndex(item => item.id === action.payload);
      if (index === -1) return; // Pas de changement
      
      const item = state.items[index];
      state.total -= item.price * item.quantity;
      state.items.splice(index, 1);
    },
    
    // Reducer avec prepare function
    addItemWithId: {
      prepare(item) {
        return {
          payload: {
            ...item,
            id: crypto.randomUUID(),
            addedAt: new Date().toISOString(),
          },
        };
      },
      reducer(state, action) {
        state.items.push(action.payload);
      },
    },
    
    // Reset complet
    clearCart(state) {
      state.items = [];
      state.total = 0;
    },
    
    // Increment quantity
    increaseQuantity(state, action) {
      const item = state.items.find(item => item.id === action.payload);
      if (item) {
        item.quantity++;
        state.total += item.price;
      }
    },
  },
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

// Export selectors (optionnel mais recommandé)
export const selectCartItems = (state) => state.cart.items;
export const selectCartTotal = (state) => state.cart.total;
export const selectCartCount = (state) => state.cart.items.length;
```

---

### Immer : "Mutable" Updates Simplifiés

RTK utilise **Immer** pour permettre du code qui "mute" le state.

```javascript
// ❌ Sans Immer (Redux classique) - Verbeux !
function updateCity(state, action) {
  return {
    ...state,
    user: {
      ...state.user,
      address: {
        ...state.user.address,
        city: action.payload, // 3 niveaux de spread !
      },
    },
  };
}

// ✅ Avec Immer (RTK) - Simple !
reducers: {
  updateCity(state, action) {
    state.user.address.city = action.payload; // C'est tout !
  }
}
```

**⚠️ Règles Importantes avec Immer :**

```javascript
// ✅ BON : "Mutation"
reducers: {
  addItem(state, action) {
    state.items.push(action.payload);
    state.count += 1;
  }
}

// ✅ BON : Return nouveau state (désactive Immer)
reducers: {
  reset(state) {
    return { items: [], count: 0 };
  }
}

// ❌ MAUVAIS : Mélanger mutation et return
reducers: {
  addItem(state, action) {
    state.items.push(action.payload); // Mutation
    return state; // ❌ Ne fais PAS ça !
  }
}

// ❌ MAUVAIS : Mutation + spread
reducers: {
  addItem(state, action) {
    state.items.push(action.payload);
    return { ...state, total: 100 }; // ❌ Confusion !
  }
}
```

---

## 🔥 UTILISATION DANS LES COMPONENTS

### useSelector : Lire le State

```javascript
import { useSelector } from 'react-redux';

function Cart() {
  // ✅ Selector simple
  const items = useSelector((state) => state.cart.items);
  
  // ✅ Selector avec calcul
  const total = useSelector((state) => 
    state.cart.items.reduce((sum, item) => sum + item.price, 0)
  );
  
  // ✅ Multiple selectors
  const itemCount = useSelector((state) => state.cart.items.length);
  const isLoading = useSelector((state) => state.cart.isLoading);
  
  return (
    <div>
      <h2>Cart ({itemCount} items)</h2>
      <p>Total: ${total}</p>
      {isLoading && <p>Loading...</p>}
      <ul>
        {items.map(item => (
          <li key={item.id}>{item.name}</li>
        ))}
      </ul>
    </div>
  );
}
```

---

### useDispatch : Modifier le State

```javascript
import { useDispatch, useSelector } from 'react-redux';
import { addItem, removeItem, clearCart } from './cartSlice';

function ProductCard({ product }) {
  const dispatch = useDispatch();
  const cartItems = useSelector((state) => state.cart.items);
  
  const isInCart = cartItems.some(item => item.id === product.id);
  
  const handleAddToCart = () => {
    dispatch(addItem({
      id: product.id,
      name: product.name,
      price: product.price,
      quantity: 1,
    }));
  };
  
  const handleRemove = () => {
    dispatch(removeItem(product.id));
  };
  
  return (
    <div>
      <h3>{product.name}</h3>
      <p>${product.price}</p>
      {isInCart ? (
        <button onClick={handleRemove}>Remove from Cart</button>
      ) : (
        <button onClick={handleAddToCart}>Add to Cart</button>
      )}
    </div>
  );
}
```

---

### ⚠️ Problèmes de Performance avec useSelector

```javascript
// ❌ MAUVAIS : Nouveau objet à chaque render
function Component() {
  const data = useSelector(state => ({
    balance: state.account.balance,
    loan: state.account.loan,
  }));
  // Component re-render même si balance/loan n'ont pas changé !
  // Car {} !== {} (nouvelle référence)
}

// ✅ SOLUTION 1 : Selectors séparés
function Component() {
  const balance = useSelector(state => state.account.balance);
  const loan = useSelector(state => state.account.loan);
}

// ✅ SOLUTION 2 : shallowEqual
import { useSelector, shallowEqual } from 'react-redux';

function Component() {
  const data = useSelector(
    state => ({
      balance: state.account.balance,
      loan: state.account.loan,
    }),
    shallowEqual // Compare les propriétés
  );
}

// ✅ SOLUTION 3 : Memoized selector (createSelector)
import { createSelector } from '@reduxjs/toolkit';

const selectAccountData = createSelector(
  [
    state => state.account.balance,
    state => state.account.loan,
  ],
  (balance, loan) => ({ balance, loan })
);

function Component() {
  const data = useSelector(selectAccountData);
  // Ne recalcule que si balance ou loan changent !
}
```

---

## ⚡ ASYNC OPERATIONS : createAsyncThunk

### Le Problème

```javascript
// ❌ On ne peut PAS faire ça dans un reducer
reducers: {
  async fetchProducts(state) {
    const res = await fetch('/api/products'); // ❌ Side effect !
    state.products = await res.json();
  }
}
```

**Solution : Thunks !**

---

### Qu'est-ce qu'un Thunk ?

Un **thunk** est une fonction qui retourne une fonction (pour retarder l'exécution).

```javascript
// Function normale
function sayHello() {
  console.log('Hello');
}

// Thunk
function sayHelloLater() {
  return function() {
    console.log('Hello later');
  };
}

const thunk = sayHelloLater();
thunk(); // Exécute maintenant
```

**Dans Redux :**

```javascript
// Thunk Redux manuel (avant RTK)
function fetchProducts() {
  return async function(dispatch, getState) {
    dispatch({ type: 'products/loading' });
    
    try {
      const res = await fetch('/api/products');
      const data = await res.json();
      dispatch({ type: 'products/success', payload: data });
    } catch (error) {
      dispatch({ type: 'products/error', payload: error.message });
    }
  };
}

// Utilisation
dispatch(fetchProducts());
```

---

### createAsyncThunk : Version Simplifiée

```javascript
import { createAsyncThunk, createSlice } from '@reduxjs/toolkit';

// 1. Créer le thunk
export const fetchProducts = createAsyncThunk(
  'products/fetch', // Action type prefix
  async (searchTerm, thunkAPI) => {
    // searchTerm = argument passé au dispatch
    // thunkAPI = { dispatch, getState, rejectWithValue, ... }
    
    const response = await fetch(`/api/products?search=${searchTerm}`);
    
    if (!response.ok) {
      throw new Error('Failed to fetch products');
    }
    
    const data = await response.json();
    return data; // Devient action.payload dans fulfilled
  }
);

// createAsyncThunk génère automatiquement 3 actions :
// - products/fetch/pending
// - products/fetch/fulfilled
// - products/fetch/rejected

// 2. Gérer les états dans le slice
const productsSlice = createSlice({
  name: 'products',
  initialState: {
    items: [],
    status: 'idle', // 'idle' | 'loading' | 'succeeded' | 'failed'
    error: null,
  },
  reducers: {
    // Autres reducers synchrones...
  },
  extraReducers: (builder) => {
    builder
      .addCase(fetchProducts.pending, (state) => {
        state.status = 'loading';
        state.error = null;
      })
      .addCase(fetchProducts.fulfilled, (state, action) => {
        state.status = 'succeeded';
        state.items = action.payload;
      })
      .addCase(fetchProducts.rejected, (state, action) => {
        state.status = 'failed';
        state.error = action.error.message;
      });
  },
});

export default productsSlice.reducer;
```

---

### Utilisation dans un Component

```javascript
import { useEffect } from 'react';
import { useDispatch, useSelector } from 'react-redux';
import { fetchProducts } from './productsSlice';

function ProductList() {
  const dispatch = useDispatch();
  const { items, status, error } = useSelector((state) => state.products);
  
  useEffect(() => {
    if (status === 'idle') {
      dispatch(fetchProducts('laptops'));
    }
  }, [status, dispatch]);
  
  if (status === 'loading') {
    return <div>Loading products...</div>;
  }
  
  if (status === 'failed') {
    return <div>Error: {error}</div>;
  }
  
  return (
    <ul>
      {items.map(product => (
        <li key={product.id}>
          {product.name} - ${product.price}
        </li>
      ))}
    </ul>
  );
}
```

---

### Thunk avec Multiple Arguments

```javascript
// ❌ MAUVAIS : Ne peut passer qu'un seul argument
export const updateProduct = createAsyncThunk(
  'products/update',
  async (id, updates) => { // ❌ 2ème argument est thunkAPI, pas updates !
    // ...
  }
);

// ✅ BON : Passer un objet
export const updateProduct = createAsyncThunk(
  'products/update',
  async ({ id, updates }, thunkAPI) => {
    const response = await fetch(`/api/products/${id}`, {
      method: 'PATCH',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(updates),
    });
    return response.json();
  }
);

// Utilisation
dispatch(updateProduct({ 
  id: 42, 
  updates: { name: 'New Name', price: 99 } 
}));
```

---

### Accéder au State dans un Thunk

```javascript
export const checkout = createAsyncThunk(
  'cart/checkout',
  async (_, thunkAPI) => {
    // Lire le state actuel
    const state = thunkAPI.getState();
    const cartItems = state.cart.items;
    const userId = state.user.id;
    
    // Dispatch d'autres actions
    thunkAPI.dispatch(showNotification('Processing checkout...'));
    
    const response = await fetch('/api/checkout', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ userId, items: cartItems }),
    });
    
    if (!response.ok) {
      return thunkAPI.rejectWithValue('Checkout failed');
    }
    
    return response.json();
  }
);
```

---

### Gestion d'Erreurs Avancée

```javascript
export const loginUser = createAsyncThunk(
  'auth/login',
  async (credentials, { rejectWithValue }) => {
    try {
      const response = await fetch('/api/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(credentials),
      });
      
      if (!response.ok) {
        const error = await response.json();
        // Rejeter avec valeur custom
        return rejectWithValue(error.message);
      }
      
      const data = await response.json();
      return data;
      
    } catch (error) {
      // Erreur réseau
      return rejectWithValue('Network error. Please try again.');
    }
  }
);

// Dans le slice
extraReducers: (builder) => {
  builder
    .addCase(loginUser.rejected, (state, action) => {
      if (action.payload) {
        // Erreur custom de rejectWithValue
        state.error = action.payload;
      } else {
        // Erreur générique
        state.error = action.error.message;
      }
    });
}
```

---

## 🎨 PATTERNS AVANCÉS

### 1. Optimistic Updates

Mettre à jour l'UI avant la confirmation du serveur.

```javascript
export const likePost = createAsyncThunk(
  'posts/like',
  async (postId, { dispatch, rejectWithValue }) => {
    // 1. Update optimiste immédiat
    dispatch(postLikedOptimistic(postId));
    
    try {
      // 2. Appel API
      const response = await fetch(`/api/posts/${postId}/like`, {
        method: 'POST',
      });
      
      if (!response.ok) throw new Error('Failed to like');
      
      return response.json();
      
    } catch (error) {
      // 3. Rollback si erreur
      dispatch(postLikeRollback(postId));
      return rejectWithValue(error.message);
    }
  }
);

// Dans le slice
reducers: {
  postLikedOptimistic(state, action) {
    const post = state.posts.find(p => p.id === action.payload);
    if (post) {
      post.likes += 1;
      post.isLiked = true;
    }
  },
  postLikeRollback(state, action) {
    const post = state.posts.find(p => p.id === action.payload);
    if (post) {
      post.likes -= 1;
      post.isLiked = false;
    }
  },
}
```

---

### 2. Normalized State avec createEntityAdapter

Pour gérer des collections d'entités efficacement.

```javascript
import { createEntityAdapter, createSlice } from '@reduxjs/toolkit';

// 1. Créer l'adapter
const productsAdapter = createEntityAdapter({
  selectId: (product) => product.id,
  sortComparer: (a, b) => a.name.localeCompare(b.name),
});

// 2. Initial state normalisé
// Structure : { ids: [1, 2, 3], entities: { '1': {...}, '2': {...} } }
const initialState = productsAdapter.getInitialState({
  loading: false,
  error: null,
});

// 3. Slice avec méthodes de l'adapter
const productsSlice = createSlice({
  name: 'products',
  initialState,
  reducers: {
    // Méthodes auto-générées par l'adapter
    productAdded: productsAdapter.addOne,
    productsReceived: productsAdapter.setAll,
    productUpdated: productsAdapter.updateOne,
    productRemoved: productsAdapter.removeOne,
    productsCleared: productsAdapter.removeAll,
  },
  extraReducers: (builder) => {
    builder
      .addCase(fetchProducts.fulfilled, (state, action) => {
        productsAdapter.setAll(state, action.payload);
      });
  },
});

// 4. Selectors auto-générés
export const {
  selectAll: selectAllProducts,
  selectById: selectProductById,
  selectIds: selectProductIds,
} = productsAdapter.getSelectors((state) => state.products);

// Utilisation
function Component() {
  const products = useSelector(selectAllProducts);
  const product42 = useSelector(state => selectProductById(state, 42));
}
```

---

### 3. Middleware Custom

```javascript
// Logger middleware
const loggerMiddleware = (store) => (next) => (action) => {
  console.log('🚀 Dispatching:', action.type);
  console.log('📦 Payload:', action.payload);
  
  const result = next(action);
  
  console.log('🔄 New State:', store.getState());
  
  return result;
};

// Analytics middleware
const analyticsMiddleware = (store) => (next) => (action) => {
  if (action.type.startsWith('cart/')) {
    // Envoyer à analytics
    window.gtag('event', action.type, {
      payload: action.payload,
    });
  }
  return next(action);
};

// Configuration
const store = configureStore({
  reducer: rootReducer,
  middleware: (getDefaultMiddleware) =>
    getDefaultMiddleware()
      .concat(loggerMiddleware)
      .concat(analyticsMiddleware),
});
```

---

## 🔄 REDUX vs CONTEXT API

### Performance : Le Vrai Problème

```javascript
// ❌ Context API : Re-renders excessifs
const CartContext = createContext();

function CartProvider({ children }) {
  const [cart, setCart] = useState({ 
    items: [], 
    total: 0, 
    user: { name: 'John' } 
  });
  
  return (
    <CartContext.Provider value={cart}>
      {children}
    </CartContext.Provider>
  );
}

function CartCount() {
  const cart = useContext(CartContext);
  // ❌ Re-render même si SEULEMENT cart.user change !
  return <div>Items: {cart.items.length}</div>;
}

// ✅ Redux : Re-renders optimisés
function CartCount() {
  const count = useSelector(state => state.cart.items.length);
  // ✅ Re-render SEULEMENT si items.length change
  return <div>Items: {count}</div>;
}
```

---

### Tableau Comparatif

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

---

### Quand Utiliser Quoi ?

**✅ Utilisez Redux Toolkit si :**
- State global avec beaucoup de données
- Mises à jour fréquentes du state
- Logique complexe (calculs, transformations)
- Besoin de middleware (analytics, logging)
- Plusieurs développeurs sur le projet
- Besoin de debugging avancé

**✅ Utilisez Context API si :**
- State peu fréquemment mis à jour (theme, locale)
- Petite/moyenne application
- Pas besoin de debugging avancé
- State simple sans logique complexe

---

## ⚠️ PIÈGES COURANTS

### 1. Muter le State sans Immer

```javascript
// ❌ MAUVAIS (Redux classique sans RTK)
function reducer(state, action) {
  state.balance += 100; // Mutation directe !
  return state;
}

// ✅ BON (Redux classique)
function reducer(state, action) {
  return { ...state, balance: state.balance + 100 };
}

// ✅ BON (RTK avec Immer)
reducers: {
  deposit(state, action) {
    state.balance += action.payload; // OK grâce à Immer !
  }
}
```

---

### 2. Oublier le Payload

```javascript
// ❌ MAUVAIS
dispatch(deposit); // Oublie d'appeler la fonction !

// ✅ BON
dispatch(deposit(100)); // Action avec payload
```

---

### 3. Side Effects dans les Reducers

```javascript
// ❌ MAUVAIS
reducers: {
  addItem(state, action) {
    fetch('/api/log'); // ❌ Side effect !
    console.log('Adding item'); // ❌ Side effect !
    state.items.push(action.payload);
  }
}

// ✅ BON : Side effects dans les thunks
export const addItemWithLog = createAsyncThunk(
  'cart/addItemWithLog',
  async (item, { dispatch }) => {
    await fetch('/api/log', { 
      method: 'POST', 
      body: JSON.stringify(item) 
    });
    dispatch(addItem(item));
    return item;
  }
);
```

---

### 4. Ne pas Gérer les États de Loading

```javascript
// ❌ MAUVAIS : Pas de loading state
function ProductList() {
  const products = useSelector(state => state.products.items);
  
  useEffect(() => {
    dispatch(fetchProducts());
  }, []);
  
  return (
    <ul>
      {products.map(p => <li key={p.id}>{p.name}</li>)}
    </ul>
  );
  // Problème : Affiche liste vide pendant le chargement !
}

// ✅ BON : Gérer tous les états
function ProductList() {
  const { items, status, error } = useSelector(state => state.products);
  
  useEffect(() => {
    if (status === 'idle') {
      dispatch(fetchProducts());
    }
  }, [status, dispatch]);
  
  if (status === 'loading') return <Spinner />;
  if (status === 'failed') return <Error message={error} />;
  
  return (
    <ul>
      {items.map(p => <li key={p.id}>{p.name}</li>)}
    </ul>
  );
}
```

---

### 5. useSelector avec Objets

```javascript
// ❌ MAUVAIS : Re-render à chaque fois
const data = useSelector(state => ({
  name: state.user.name,
  email: state.user.email,
}));
// {} !== {} → Re-render même si name et email n'ont pas changé !

// ✅ BON : Selectors séparés
const name = useSelector(state => state.user.name);
const email = useSelector(state => state.user.email);

// ✅ BON : Avec shallowEqual
import { shallowEqual } from 'react-redux';
const data = useSelector(
  state => ({ name: state.user.name, email: state.user.email }),
  shallowEqual
);
```

---

### 6. Dispatcher dans le Render

```javascript
// ❌ MAUVAIS : Boucle infinie !
function Component() {
  const count = useSelector(state => state.counter.value);
  const dispatch = useDispatch();
  
  dispatch(increment()); // ❌ À chaque render !
  
  return <div>{count}</div>;
}

// ✅ BON : Dans un useEffect ou event handler
function Component() {
  const count = useSelector(state => state.counter.value);
  const dispatch = useDispatch();
  
  useEffect(() => {
    dispatch(fetchData());
  }, []); // Une seule fois
  
  return <div>{count}</div>;
}
```

---

## 🎯 QUESTIONS D'ENTRETIEN FRÉQUENTES

### Q1 : Explique le data flow dans Redux

**Réponse structurée :**

1. **User interaction** → Click sur un bouton
2. **Dispatch action** → `dispatch(addItem(product))`
3. **Store envoie au Reducer** → `reducer(currentState, action)`
4. **Reducer calcule new state** → Return nouveau state (immuable)
5. **Store met à jour** → `store.state = newState`
6. **Subscribers notifiés** → Components re-render

**Code exemple :**

```javascript
// 1. User click
<button onClick={() => dispatch(addItem(product))}>Add</button>

// 2. Action dispatched
{ type: 'cart/addItem', payload: { id: 1, name: 'Laptop' } }

// 3. Reducer reçoit
function cartReducer(state, action) {
  // 4. Calcule new state
  return { ...state, items: [...state.items, action.payload] };
}

// 5. Component re-render
function Cart() {
  const items = useSelector(state => state.cart.items);
  // Affiche le nouvel item
}
```

---

### Q2 : Différence entre Redux et Redux Toolkit ?

**Réponse :**

**Redux classique :**
- ❌ Beaucoup de boilerplate
- ❌ Setup complexe (store, middleware, devtools)
- ❌ Mutations interdites → Spread operator partout
- ❌ Async nécessite redux-thunk séparé

**Redux Toolkit (RTK) :**
- ✅ `createSlice` → Actions + reducer en un
- ✅ `configureStore` → Setup automatique
- ✅ Immer intégré → "Mutations" possibles
- ✅ `createAsyncThunk` → Async simplifié
- ✅ 70% de code en moins

**Exemple comparatif :**

```javascript
// Redux classique : 30 lignes
const ADD = 'cart/add';
function add(item) { return { type: ADD, payload: item }; }
function reducer(state = [], action) {
  switch(action.type) {
    case ADD: return [...state, action.payload];
    default: return state;
  }
}

// RTK : 8 lignes
const cartSlice = createSlice({
  name: 'cart',
  initialState: [],
  reducers: {
    add(state, action) { state.push(action.payload); }
  }
});
```

---

### Q3 : C'est quoi un thunk ?

**Réponse :**

Un **thunk** est une fonction qui retourne une autre fonction. Dans Redux, ça permet de faire des **opérations asynchrones**.

**Sans thunk :**

```javascript
// ❌ Ne fonctionne pas
dispatch({ type: 'products/fetch' });
// Comment faire l'API call ?
```

**Avec thunk :**

```javascript
// ✅ Fonction qui retourne une fonction
function fetchProducts() {
  return async (dispatch) => {
    dispatch({ type: 'products/loading' });
    const data = await fetch('/api/products').then(r => r.json());
    dispatch({ type: 'products/success', payload: data });
  };
}

dispatch(fetchProducts()); // Exécute la fonction retournée
```

**Avec RTK :**

```javascript
export const fetchProducts = createAsyncThunk(
  'products/fetch',
  async () => {
    const res = await fetch('/api/products');
    return res.json();
  }
);
```

---

### Q4 : Pourquoi les reducers doivent être des pure functions ?

**Réponse :**

**Pure function** = Même input → Même output, sans side effects.

**Avantages :**
1. ✅ **Predictable** : Facile à comprendre et débugger
2. ✅ **Testable** : Test simple sans mock
3. ✅ **Time-travel debugging** : Redux DevTools peut "rejouer" les actions
4. ✅ **Hot reloading** : Remplacer les reducers sans perdre le state

**Exemples :**

```javascript
// ✅ PURE
function reducer(state, action) {
  return { ...state, count: state.count + 1 };
}

// ❌ IMPURE : Mutation
function reducer(state, action) {
  state.count++; // Modifie l'argument !
  return state;
}

// ❌ IMPURE : Side effect
function reducer(state, action) {
  fetch('/api/log'); // API call !
  return { ...state, count: state.count + 1 };
}

// ❌ IMPURE : Non-déterministe
function reducer(state, action) {
  return { ...state, timestamp: Date.now() }; // Résultat change !
}
```

---

### Q5 : Quand utiliser Redux vs Context API ?

**Réponse avec exemples concrets :**

**✅ Redux pour :**
- **E-commerce cart** → Updates fréquents, logique complexe
- **Dashboard avec data** → Beaucoup de state partagé
- **App collaborative** → Undo/redo, sync
- **Large team** → Structure claire, debugging

**✅ Context pour :**
- **Theme switcher** → Peu de updates, simple
- **User authentication** → Login/logout uniquement
- **Locale/i18n** → Change rarement
- **Small app** → Pas besoin de Redux

**Code exemple :**

```javascript
// Theme avec Context (parfait)
const ThemeContext = createContext();
function App() {
  const [theme, setTheme] = useState('dark');
  return (
    <ThemeContext.Provider value={{ theme, setTheme }}>
      <Component />
    </ThemeContext.Provider>
  );
}

// Cart avec Redux (meilleur)
// - AddItem, RemoveItem, UpdateQuantity, ClearCart
// - Calculs (total, taxes, shipping)
// - Persistence (localStorage)
// - Optimistic updates
```

---

### Q6 : Comment gérer les erreurs avec createAsyncThunk ?

**Réponse avec code :**

```javascript
export const fetchUser = createAsyncThunk(
  'user/fetch',
  async (userId, { rejectWithValue }) => {
    try {
      const response = await fetch(`/api/users/${userId}`);
      
      if (!response.ok) {
        const error = await response.json();
        return rejectWithValue(error.message);
      }
      
      return response.json();
      
    } catch (error) {
      return rejectWithValue('Network error');
    }
  }
);

// Dans le slice
extraReducers: (builder) => {
  builder
    .addCase(fetchUser.pending, (state) => {
      state.loading = true;
      state.error = null;
    })
    .addCase(fetchUser.fulfilled, (state, action) => {
      state.loading = false;
      state.data = action.payload;
    })
    .addCase(fetchUser.rejected, (state, action) => {
      state.loading = false;
      state.error = action.payload || action.error.message;
    });
}
```

---

### Q7 : C'est quoi Immer et pourquoi RTK l'utilise ?

**Réponse :**

**Immer** permet d'écrire du code qui "mute" le state, mais produit un nouveau state immuable en interne.

**Problème sans Immer :**

```javascript
// Nested update = verbeux
return {
  ...state,
  user: {
    ...state.user,
    address: {
      ...state.user.address,
      city: 'Paris'
    }
  }
};
```

**Avec Immer (RTK) :**

```javascript
reducers: {
  updateCity(state, action) {
    state.user.address.city = action.payload; // Simple !
  }
}
```

**Comment ça marche :**
1. Immer crée un "draft" du state
2. On "mute" le draft
3. Immer produit un nouveau state immuable basé sur les changements

---

### Q8 : Différence entre reducers et extraReducers ?

**Réponse :**

**`reducers`** : Pour les actions **créées par ce slice**

```javascript
const cartSlice = createSlice({
  name: 'cart',
  reducers: {
    addItem(state, action) { /* ... */ },
    removeItem(state, action) { /* ... */ },
  }
});

// Génère automatiquement :
// - cart/addItem
// - cart/removeItem
```

**`extraReducers`** : Pour les actions **externes** (autres slices, thunks)

```javascript
const cartSlice = createSlice({
  name: 'cart',
  reducers: { /* ... */ },
  extraReducers: (builder) => {
    builder
      // Répondre aux actions d'un thunk
      .addCase(fetchProducts.fulfilled, (state, action) => {
        // Gérer les produits chargés
      })
      // Répondre aux actions d'un autre slice
      .addCase(clearAll, (state) => {
        state.items = [];
      });
  }
});
```

---

### Q9 : Comment tester un reducer Redux ?

**Réponse :**

Les reducers sont des **pure functions** → Très faciles à tester !

```javascript
// cartSlice.js
const cartSlice = createSlice({
  name: 'cart',
  initialState: { items: [], total: 0 },
  reducers: {
    addItem(state, action) {
      state.items.push(action.payload);
      state.total += action.payload.price;
    },
  },
});

// cartSlice.test.js
import cartReducer, { addItem } from './cartSlice';

test('should add item to cart', () => {
  const initialState = { items: [], total: 0 };
  const item = { id: 1, name: 'Laptop', price: 1000 };
  
  const newState = cartReducer(initialState, addItem(item));
  
  expect(newState.items).toHaveLength(1);
  expect(newState.items[0]).toEqual(item);
  expect(newState.total).toBe(1000);
});

test('should not mutate original state', () => {
  const initialState = { items: [], total: 0 };
  const item = { id: 1, name: 'Laptop', price: 1000 };
  
  cartReducer(initialState, addItem(item));
  
  expect(initialState.items).toHaveLength(0); // Original intact
});
```

---

### Q10 : Comment persister le Redux state dans localStorage ?

**Réponse :**

**Méthode 1 : Middleware custom**

```javascript
// store.js
const localStorageMiddleware = (store) => (next) => (action) => {
  const result = next(action);
  const state = store.getState();
  
  // Sauvegarder après chaque action
  localStorage.setItem('reduxState', JSON.stringify(state));
  
  return result;
};

const store = configureStore({
  reducer: rootReducer,
  preloadedState: JSON.parse(localStorage.getItem('reduxState') || '{}'),
  middleware: (getDefaultMiddleware) =>
    getDefaultMiddleware().concat(localStorageMiddleware),
});
```

**Méthode 2 : redux-persist (recommandé)**

```javascript
import { persistStore, persistReducer } from 'redux-persist';
import storage from 'redux-persist/lib/storage';

const persistConfig = {
  key: 'root',
  storage,
  whitelist: ['cart', 'user'], // Seulement ces slices
};

const persistedReducer = persistReducer(persistConfig, rootReducer);

const store = configureStore({
  reducer: persistedReducer,
});

const persistor = persistStore(store);

// Dans App
import { PersistGate } from 'redux-persist/integration/react';

<Provider store={store}>
  <PersistGate loading={null} persistor={persistor}>
    <App />
  </PersistGate>
</Provider>
```

---

## 🔗 LIENS AVEC VOS PROJETS

### fast-react-pizza

**Ce que vous avez fait :**

```javascript
// cartSlice.js
const cartSlice = createSlice({
  name: 'cart',
  initialState: { cart: [] },
  reducers: {
    addItem(state, action) {
      state.cart.push(action.payload);
    },
    deleteItem(state, action) {
      state.cart = state.cart.filter(item => item.pizzaId !== action.payload);
    },
    increaseItemQuantity(state, action) {
      const item = state.cart.find(item => item.pizzaId === action.payload);
      item.quantity++;
      item.totalPrice = item.quantity * item.unitPrice;
    },
    clearCart(state) {
      state.cart = [];
    },
  },
});

// userSlice.js
const userSlice = createSlice({
  name: 'user',
  initialState: { username: '' },
  reducers: {
    updateName(state, action) {
      state.username = action.payload;
    },
  },
});

// Thunk pour geolocation
export const fetchAddress = createAsyncThunk(
  'user/fetchAddress',
  async function () {
    const positionObj = await getPosition();
    const position = {
      latitude: positionObj.coords.latitude,
      longitude: positionObj.coords.longitude,
    };
    const addressObj = await getAddress(position);
    const address = `${addressObj?.locality}, ${addressObj?.city}`;
    return { position, address };
  }
);
```

**Points à mentionner en entretien :**

✅ "J'ai utilisé Redux Toolkit pour gérer le state global du panier"
✅ "J'ai créé des slices séparés pour le cart et le user"
✅ "J'ai utilisé createAsyncThunk pour la geolocation asynchrone"
✅ "J'ai implémenté des actions comme addItem, deleteItem, increaseQuantity"
✅ "Le state est immuable grâce à Immer intégré dans RTK"

---

### wild-oasis

**Ce que vous pourriez mentionner :**

Si vous n'avez pas utilisé Redux ici (React Query à la place) :

✅ "Pour wild-oasis, j'ai utilisé React Query au lieu de Redux"
✅ "React Query est mieux pour le server state (cabins, bookings)"
✅ "Redux serait mieux pour UI state complexe (filters, modals)"
✅ "J'ai utilisé Context API pour l'authentification et le dark mode"

---

### worldwise

**Points Redux pertinents :**

✅ "J'ai géré la liste des cities avec useReducer (similaire à Redux)"
✅ "useReducer prépare à comprendre Redux"
✅ "Pour une app plus large, j'aurais utilisé Redux Toolkit"

---

## 📚 RESSOURCES COMPLÉMENTAIRES

### Documentation Officielle
- **Redux Toolkit Docs** : https://redux-toolkit.js.org/
- **Redux Essentials Tutorial** : Guide officiel RTK

### DevTools
- **Redux DevTools Extension** : Indispensable pour débugger
  - Time-travel debugging
  - Action history
  - State diff

### Best Practices
1. ✅ Toujours utiliser Redux Toolkit (pas Redux classique)
2. ✅ Un slice par feature (cartSlice, userSlice, etc.)
3. ✅ Nommer les actions clairement : `cart/addItem` pas `ADD`
4. ✅ Utiliser createAsyncThunk pour l'async
5. ✅ createSelector pour les selectors complexes
6. ✅ Garder les reducers simples et purs

---

## 🎓 RÉCAPITULATIF FINAL

### Ce qu'il faut absolument savoir

**Concepts Core :**
- ✅ Single source of truth
- ✅ State read-only (dispatch actions)
- ✅ Pure functions (reducers)
- ✅ One-way data flow

**Redux Toolkit :**
- ✅ `configureStore` → Setup simplifié
- ✅ `createSlice` → Actions + reducer
- ✅ Immer → "Mutations" possibles
- ✅ `createAsyncThunk` → Async simple

**Hooks :**
- ✅ `useSelector` → Lire le state
- ✅ `useDispatch` → Modifier le state

**Quand utiliser :**
- ✅ Redux → State complexe, updates fréquents
- ✅ Context → State simple, peu de updates

---