## [**..**](./00_index.md)

# 📘 CHAPITRE 13 : Next.js Avancé - Guide Complet Approfondi

## 🎯 Table des Matières
1. [Server Components (RSC) - Deep Dive](#1-server-components)
2. [Client Components - Détails Complets](#2-client-components)
3. [Rendering Strategies Complètes](#3-rendering-strategies)
4. [Server Actions - Guide Exhaustif](#4-server-actions)
5. [Système de Cache - 4 Layers](#5-caching-system)
6. [Hooks Next.js Avancés](#6-hooks-nextjs)
7. [Patterns Avancés & Best Practices](#7-patterns-avances)
8. [Performance Optimization](#8-performance)
9. [Questions d'Entretien](#9-questions-entretien)

---

## 1. SERVER COMPONENTS (RSC) - DEEP DIVE

### 1.1 Architecture & Fonctionnement

Les **Server Components** sont le cœur de Next.js 13+ App Router. Ils s'exécutent UNIQUEMENT sur le serveur.

```javascript
// app/products/page.js - Server Component par défaut
async function ProductsPage() {
  // ✅ Accès direct à la database
  const products = await db.product.findMany();
  
  // ✅ Variables d'environnement privées
  const apiKey = process.env.PRIVATE_API_KEY;
  
  // ✅ Lecture filesystem
  const config = await fs.readFile('./config.json');
  
  return (
    <div>
      <h1>Products</h1>
      {products.map(product => (
        <ProductCard key={product.id} product={product} />
      ))}
    </div>
  );
}
```

**Ce que le client reçoit :**
```html
<!-- HTML pur, ZÉRO JavaScript pour ce composant -->
<div>
  <h1>Products</h1>
  <div class="product-card">Product 1</div>
  <div class="product-card">Product 2</div>
</div>
```

### 1.2 RSC Payload Format

Next.js envoie un format spécial (pas du HTML brut) :

```javascript
// Format RSC Payload (simplifié)
{
  type: 'div',
  props: { className: 'products' },
  children: [
    { type: 'h1', children: 'Products' },
    { type: ProductCard, props: { product: {...} } }
  ]
}
```

### 1.3 Pattern : Data Fetching Colocation

```javascript
// ✅ EXCELLENT - Fetch là où vous en avez besoin
// app/dashboard/page.js
async function DashboardPage() {
  const stats = await fetchStats();
  return <StatsDisplay stats={stats} />;
}

// Pas besoin de getServerSideProps séparé !
// Pas de prop drilling !
```

### 1.4 Pattern : Parallel Data Fetching

```javascript
// ❌ MAUVAIS - Waterfall (séquentiel)
async function BadDashboard() {
  const user = await fetchUser();        // 200ms ⏳
  const orders = await fetchOrders();    // 300ms ⏳
  const products = await fetchProducts(); // 400ms ⏳
  // Total: 900ms 😢
}

// ✅ BON - Parallel
async function GoodDashboard() {
  const [user, orders, products] = await Promise.all([
    fetchUser(),
    fetchOrders(),
    fetchProducts()
  ]);
  // Total: 400ms (le plus lent) 🚀
}

// ✅ EXCELLENT - Streaming avec Suspense
async function BestDashboard() {
  return (
    <>
      <UserHeader userId={userId} />
      
      <Suspense fallback={<OrdersSkeleton />}>
        <OrdersSection />
      </Suspense>
      
      <Suspense fallback={<ProductsSkeleton />}>
        <ProductsSection />
      </Suspense>
    </>
  );
}
```

### 1.5 Pattern : Request Memoization (Automatic)

```javascript
// utils/queries.js
export async function getUser(userId) {
  return fetch(`/api/users/${userId}`);
}

// Header.js (Server Component)
async function Header({ userId }) {
  const user = await getUser(userId); // Fetch 1
  return <div>{user.name}</div>;
}

// Sidebar.js (Server Component)
async function Sidebar({ userId }) {
  const user = await getUser(userId); // Dédupliqué automatiquement ! ✨
  return <nav>{user.role}</nav>;
}

// Next.js fait UNE SEULE requête réseau
// Request Memoization = automatic deduplication
```

### 1.6 Limitations Server Components

```javascript
// ❌ IMPOSSIBLE dans Server Components :

// 1. Hooks React
const [state, setState] = useState(); // ❌ Error!
useEffect(() => {}, []); // ❌ Error!

// 2. Event handlers
<button onClick={handleClick}> // ❌ Error!

// 3. Browser APIs
localStorage.getItem('token'); // ❌ Error! (undefined)
window.location.href; // ❌ Error!

// 4. Context (nécessite 'use client')
<ThemeContext.Provider> // ❌ Error!
```

---

## 2. CLIENT COMPONENTS - DÉTAILS COMPLETS

### 2.1 Directive 'use client'

```javascript
'use client' // ⚠️ DOIT être la première ligne

import { useState, useEffect } from 'react';

// Ce composant ET tous ses children deviennent Client Components
export default function Counter() {
  const [count, setCount] = useState(0);
  
  useEffect(() => {
    console.log('Count changed:', count);
  }, [count]);
  
  return (
    <button onClick={() => setCount(count + 1)}>
      Count: {count}
    </button>
  );
}
```

### 2.2 Client Boundary Pattern (CRITIQUE)

```javascript
// ❌ MAUVAIS - Tout devient Client Component
'use client'

import { useState } from 'react';
import { HeavyServerComponent } from './HeavyServerComponent';

export default function Page() {
  const [open, setOpen] = useState(false);
  
  return (
    <div>
      <button onClick={() => setOpen(!open)}>Toggle</button>
      {/* HeavyServerComponent devient Client Component ! 😱 */}
      {/* Perd tous les avantages SC */}
      <HeavyServerComponent />
    </div>
  );
}

// ✅ BON - Isoler l'interactivité au minimum
// app/page.js (Server Component)
import { HeavyServerComponent } from './HeavyServerComponent';
import { ToggleButton } from './ToggleButton';

export default function Page() {
  return (
    <div>
      <ToggleButton /> {/* Seul ce petit composant est client */}
      <HeavyServerComponent /> {/* Reste Server Component ✅ */}
    </div>
  );
}

// app/ToggleButton.js
'use client'
import { useState } from 'react';

export function ToggleButton() {
  const [open, setOpen] = useState(false);
  return (
    <button onClick={() => setOpen(!open)}>
      {open ? 'Open' : 'Closed'}
    </button>
  );
}
```

### 2.3 Passer Server Components à Client Components

```javascript
// ❌ IMPOSSIBLE - Import direct SC dans CC
'use client'
import ServerComponent from './ServerComponent'; // ❌ Error!

function ClientComponent() {
  return <ServerComponent />; // Ne marchera pas
}

// ✅ SOLUTION - Passer en props/children
// app/page.js (Server Component)
import ClientWrapper from './ClientWrapper';
import ServerComponent from './ServerComponent';

export default function Page() {
  return (
    <ClientWrapper>
      <ServerComponent /> {/* Passé en children ✅ */}
    </ClientWrapper>
  );
}

// ClientWrapper.js
'use client'
export default function ClientWrapper({ children }) {
  const [state, setState] = useState();
  
  return (
    <div>
      <button onClick={() => setState(!state)}>Toggle</button>
      {children} {/* Server Component rendu ici */}
    </div>
  );
}
```

---

## 3. RENDERING STRATEGIES COMPLÈTES

### 3.1 Static Rendering (SSG) - Génération Statique

```javascript
// app/blog/[slug]/page.js

// Static par défaut avec cache: 'force-cache'
async function BlogPost({ params }) {
  const post = await fetch(`https://api.blog.com/posts/${params.slug}`, {
    cache: 'force-cache' // Défaut
  });
  
  return (
    <article>
      <h1>{post.title}</h1>
      <p>{post.content}</p>
    </article>
  );
}

// Générer les routes statiques au build time
export async function generateStaticParams() {
  const posts = await fetch('https://api.blog.com/posts')
    .then(r => r.json());
  
  // Next.js génère ces routes au build :
  // /blog/post-1
  // /blog/post-2
  // /blog/post-3
  return posts.map(post => ({
    slug: post.slug
  }));
}

// Options de génération
export const dynamicParams = true; // Permet new slugs en production
// Si false : 404 pour slugs non générés au build
```

**📊 Quand utiliser SSG :**
- ✅ Documentation
- ✅ Blog posts
- ✅ Pages marketing
- ✅ Contenu rarement modifié

**⚡ Avantages :**
- Ultra rapide (servi depuis CDN)
- Coût serveur minimal
- Excellent SEO

### 3.2 Dynamic Rendering (SSR) - Server-Side Rendering

```javascript
// app/dashboard/page.js

// Option 1 : Force dynamic rendering
export const dynamic = 'force-dynamic';

// Option 2 : Revalidate 0
export const revalidate = 0;

async function Dashboard() {
  // Fetch à chaque requête
  const userData = await fetch('https://api.com/user', {
    cache: 'no-store' // Force pas de cache
  });
  
  return <div>Welcome {userData.name}</div>;
}
```

**🔄 Triggers automatiques de Dynamic Rendering :**

```javascript
import { cookies, headers } from 'next/headers';

async function Page({ searchParams }) {
  // Automatiquement dynamic si vous utilisez :
  const cookieStore = cookies();        // ⚡ Dynamic
  const headersList = headers();        // ⚡ Dynamic
  const search = searchParams.query;    // ⚡ Dynamic
  
  // Ces fonctions forcent dynamic rendering
}
```

**📊 Quand utiliser SSR :**
- ✅ Dashboards personnalisés
- ✅ Données temps réel
- ✅ Contenu personnalisé par user
- ✅ Pages authentifiées

### 3.3 Incremental Static Regeneration (ISR)

```javascript
// app/products/[id]/page.js

async function ProductPage({ params }) {
  const product = await fetch(`https://api.com/products/${params.id}`, {
    next: { revalidate: 3600 } // Revalide toutes les heures
  });
  
  return (
    <div>
      <h1>{product.name}</h1>
      <p>${product.price}</p>
    </div>
  );
}
```

**🔄 Comment ISR fonctionne :**

1. **Build time** : génère page statique
2. **User request** : sert version statique (rapide)
3. **Après revalidate time** : background regeneration
4. **Next user** : reçoit nouvelle version

**🏷️ ISR avec tags pour revalidation on-demand :**

```javascript
async function ProductPage({ params }) {
  const product = await fetch(`https://api.com/products/${params.id}`, {
    next: { 
      revalidate: 3600,
      tags: ['products', `product-${params.id}`]
    }
  });
  
  return <div>{product.name}</div>;
}

// Revalider manuellement depuis Server Action
import { revalidateTag, revalidatePath } from 'next/cache';

export async function updateProduct(id, data) {
  'use server'
  
  await db.products.update(id, data);
  
  // Option 1 : Revalider par tag
  revalidateTag(`product-${id}`);
  
  // Option 2 : Revalider path entier
  revalidatePath(`/products/${id}`);
  
  // Option 3 : Revalider tous les products
  revalidateTag('products');
}
```

**🎯 Stratégies ISR avancées :**

```javascript
// Stratégie 1 : Time-based simple
{ next: { revalidate: 60 } } // 60 secondes

// Stratégie 2 : On-demand avec tags
{ next: { revalidate: 3600, tags: ['products'] } }
// + revalidateTag('products') dans mutations

// Stratégie 3 : Hybrid
{ next: { revalidate: 86400, tags: ['products'] } }
// Revalide 1x/jour automatiquement
// + on-demand quand nécessaire
```

### 3.4 Streaming avec Suspense

```javascript
// app/dashboard/page.js
import { Suspense } from 'react';

export default function Dashboard() {
  return (
    <div>
      {/* ⚡ Instant - pas de loading */}
      <DashboardHeader />
      
      {/* 🔄 Streaming - charge indépendamment */}
      <Suspense fallback={<StatsSkeleton />}>
        <Stats />
      </Suspense>
      
      <Suspense fallback={<ChartSkeleton />}>
        <RevenueChart />
      </Suspense>
      
      <Suspense fallback={<TableSkeleton />}>
        <RecentOrders />
      </Suspense>
    </div>
  );
}

// Stats.js (Server Component avec fetch lent)
async function Stats() {
  // Fetch lent (2 secondes)
  const stats = await fetchStats(); // 2000ms
  
  return <StatsCards data={stats} />;
}
```

**🌊 Flow du Streaming :**

1. HTML initial envoyé avec fallbacks
2. `Stats` termine → streamed au client
3. `RevenueChart` termine → streamed
4. `RecentOrders` termine → streamed

User voit la page **progressivement**, pas de blocage ! 🚀

### 3.5 Comparaison Rendering Strategies

| Strategy | Quand | Performance | Fraîcheur | Use Case |
|----------|-------|-------------|-----------|----------|
| **SSG** | Build time | ⚡⚡⚡ Excellent | ❌ Stale | Blog, docs |
| **ISR** | Build + revalidate | ⚡⚡ Très bon | ✅ Bon | E-commerce |
| **SSR** | Chaque request | ⚠️ Plus lent | ✅✅ Excellent | Dashboard user |
| **Streaming** | Progressive | ⚡⚡ Bon | ✅✅ Excellent | Complex pages |

---

## 4. SERVER ACTIONS - GUIDE EXHAUSTIF

### 4.1 Anatomie d'une Server Action

```javascript
// app/actions.js
'use server' // ⚡ Directive au top du fichier

import { revalidatePath } from 'next/cache';
import { redirect } from 'next/navigation';
import { z } from 'zod';

// Schema validation
const ProductSchema = z.object({
  name: z.string().min(3),
  price: z.number().positive(),
  description: z.string().optional()
});

export async function createProduct(formData) {
  // 1️⃣ Extraction données
  const rawData = {
    name: formData.get('name'),
    price: parseFloat(formData.get('price')),
    description: formData.get('description')
  };
  
  // 2️⃣ Validation
  const validatedData = ProductSchema.parse(rawData);
  
  // 3️⃣ Authentication check
  const session = await getServerSession();
  if (!session) {
    throw new Error('Unauthorized');
  }
  
  // 4️⃣ Database mutation
  const product = await db.product.create({
    data: validatedData
  });
  
  // 5️⃣ Cache revalidation
  revalidatePath('/products');
  
  // 6️⃣ Redirect
  redirect(`/products/${product.id}`);
}

// Server Action inline
export async function deleteProduct(productId) {
  'use server' // Peut être inline aussi
  
  await db.product.delete({ where: { id: productId } });
  revalidatePath('/products');
}
```

### 4.2 Forms avec Progressive Enhancement

```javascript
// app/products/new/page.js
import { createProduct } from '@/actions';

export default function NewProductPage() {
  return (
    <form action={createProduct}>
      <input name="name" required />
      <input name="price" type="number" required />
      <textarea name="description" />
      <button type="submit">Create Product</button>
    </form>
  );
}

// ✅ Fonctionne SANS JavaScript activé !
// Form submission native HTML si JS désactivé
// Progressive Enhancement FTW 🎉
```

### 4.3 useFormState - Gestion État Formulaire

```javascript
// app/actions.js
'use server'

export async function createProduct(prevState, formData) {
  const name = formData.get('name');
  const price = formData.get('price');
  
  // Validation
  if (!name || name.length < 3) {
    return { 
      error: 'Name must be at least 3 characters',
      fields: { name, price } // Preserve fields
    };
  }
  
  try {
    await db.product.create({ 
      data: { name, price: parseFloat(price) } 
    });
    return { success: true };
  } catch (error) {
    return { error: 'Failed to create product' };
  }
}

// app/components/ProductForm.js
'use client'

import { useFormState, useFormStatus } from 'react-dom';
import { createProduct } from '@/actions';

function SubmitButton() {
  const { pending } = useFormStatus();
  
  return (
    <button type="submit" disabled={pending}>
      {pending ? '⏳ Creating...' : '✅ Create Product'}
    </button>
  );
}

export default function ProductForm() {
  const initialState = { error: null, fields: null };
  const [state, formAction] = useFormState(createProduct, initialState);
  
  return (
    <form action={formAction}>
      <div>
        <input 
          name="name" 
          defaultValue={state?.fields?.name}
          placeholder="Product name"
          required 
        />
        {state?.error && (
          <span className="text-red-500">{state.error}</span>
        )}
      </div>
      
      <input 
        name="price" 
        type="number" 
        placeholder="Price"
        required 
      />
      
      <SubmitButton />
      
      {state?.success && (
        <p className="text-green-500">✅ Product created!</p>
      )}
    </form>
  );
}
```

### 4.4 useTransition - Non-blocking Updates

```javascript
'use client'

import { useTransition } from 'react';
import { deleteProduct } from '@/actions';

export default function DeleteButton({ productId }) {
  const [isPending, startTransition] = useTransition();
  
  function handleDelete() {
    startTransition(async () => {
      await deleteProduct(productId);
      // Navigation ou UI update après
    });
  }
  
  return (
    <button 
      onClick={handleDelete}
      disabled={isPending}
      className={isPending ? 'opacity-50' : ''}
    >
      {isPending ? '⏳ Deleting...' : '🗑️ Delete'}
    </button>
  );
}
```

### 4.5 useOptimistic - Optimistic UI Updates

```javascript
'use client'

import { useOptimistic } from 'react';
import { toggleLike } from '@/actions';

export default function LikeButton({ postId, initialLikes, initialIsLiked }) {
  const [optimisticState, addOptimistic] = useOptimistic(
    { likes: initialLikes, isLiked: initialIsLiked },
    (state, newIsLiked) => ({
      likes: state.likes + (newIsLiked ? 1 : -1),
      isLiked: newIsLiked
    })
  );
  
  async function handleLike() {
    const newIsLiked = !optimisticState.isLiked;
    
    // ⚡ Update UI immédiatement (optimistic)
    addOptimistic(newIsLiked);
    
    // 🔄 Server mutation en background
    await toggleLike(postId, newIsLiked);
  }
  
  return (
    <button onClick={handleLike} className="flex items-center gap-2">
      {optimisticState.isLiked ? '❤️' : '🤍'} 
      <span>{optimisticState.likes}</span>
    </button>
  );
}

// actions.js
'use server'
export async function toggleLike(postId, isLiked) {
  await db.like.upsert({
    where: { postId },
    create: { postId, liked: isLiked },
    update: { liked: isLiked }
  });
  
  revalidatePath(`/posts/${postId}`);
}
```

### 4.6 Error Handling avancé

```javascript
// actions.js
'use server'

export async function createOrder(formData) {
  try {
    const order = await db.order.create({
      data: extractData(formData)
    });
    
    // Envoyer email
    await sendOrderEmail(order);
    
    revalidatePath('/orders');
    return { success: true, orderId: order.id };
    
  } catch (error) {
    console.error('Order creation failed:', error);
    
    if (error.code === 'P2002') {
      return { error: 'Order already exists' };
    }
    
    return { error: 'Failed to create order. Please try again.' };
  }
}

// Client component
'use client'

import { useState } from 'react';
import { createOrder } from '@/actions';

export default function OrderForm() {
  const [error, setError] = useState(null);
  const [loading, setLoading] = useState(false);
  
  async function handleSubmit(e) {
    e.preventDefault();
    setLoading(true);
    setError(null);
    
    const formData = new FormData(e.target);
    const result = await createOrder(formData);
    
    setLoading(false);
    
    if (result.error) {
      setError(result.error);
    } else {
      router.push(`/orders/${result.orderId}`);
    }
  }
  
  return (
    <form onSubmit={handleSubmit}>
      {error && (
        <div className="bg-red-100 text-red-700 p-3 rounded">
          {error}
        </div>
      )}
      
      {/* form fields */}
      
      <button disabled={loading}>
        {loading ? '⏳ Creating...' : '✅ Create Order'}
      </button>
    </form>
  );
}
```

---

## 5. CACHING SYSTEM - ARCHITECTURE COMPLÈTE

### 5.1 Les 4 Layers de Cache

```
┌─────────────────────────────────────────────┐
│ 1️⃣ REQUEST MEMOIZATION (React)            │
│    Duration: Render cycle uniquement        │
│    Scope: Même requête, même render         │
│    Automatic deduplication                  │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│ 2️⃣ DATA CACHE (Next.js)                   │
│    Duration: Persistent (jusqu'à revalidate)│
│    Scope: Toutes les requêtes               │
│    fetch() responses cached                 │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│ 3️⃣ FULL ROUTE CACHE (Next.js)             │
│    Duration: Build + runtime                │
│    Scope: Rendu complet de la route         │
│    HTML + RSC Payload                       │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│ 4️⃣ ROUTER CACHE (Browser)                 │
│    Duration: Session ou temps défini        │
│    Scope: Client-side navigation            │
│    Prefetch cache                           │
└─────────────────────────────────────────────┘
```

### 5.2 Request Memoization (Automatic)

```javascript
// app/page.js
async function Page() {
  return (
    <>
      <Header />
      <Sidebar />
      <Content />
    </>
  );
}

// components/Header.js
async function Header() {
  const user = await fetch('/api/user'); // Request 1
  return <div>{user.name}</div>;
}

// components/Sidebar.js
async function Sidebar() {
  const user = await fetch('/api/user'); // ✨ Dédupliqué !
  return <nav>{user.role}</nav>;
}

// components/Content.js
async function Content() {
  const user = await fetch('/api/user'); // ✨ Dédupliqué aussi !
  return <main>Welcome {user.name}</main>;
}

// 🎉 Résultat : 1 seule requête réseau
// Partagée entre les 3 composants
// Automatique pendant le render cycle
```

### 5.3 Data Cache (Configurable)

```javascript
// Option 1 : Cache par défaut (permanent)
fetch('https://api.com/data')
// Equivalent à :
fetch('https://api.com/data', { cache: 'force-cache' })

// Option 2 : Pas de cache
fetch('https://api.com/data', { cache: 'no-store' })

// Option 3 : Revalidation time-based
fetch('https://api.com/data', { 
  next: { revalidate: 3600 } // 1 heure
})

// Option 4 : Revalidation avec tags
fetch('https://api.com/data', {
  next: { 
    revalidate: 3600,
    tags: ['products', 'inventory']
  }
})
```

**🔄 Revalidation manuelle :**

```javascript
import { revalidateTag, revalidatePath } from 'next/cache';

// Dans une Server Action
export async function updateProduct() {
  'use server'
  
  await db.product.update(...);
  
  // Invalide tous les fetch avec ce tag
  revalidateTag('products');
  
  // OU invalide un path spécifique
  revalidatePath('/products');
  
  // OU invalide avec layout
  revalidatePath('/products', 'layout');
}
```

### 5.4 Full Route Cache

```javascript
// app/products/page.js

// Static (cached au build)
export const dynamic = 'force-static'; // Défaut

// Dynamic (jamais cached)
export const dynamic = 'force-dynamic';

// Auto (Next.js décide)
export const dynamic = 'auto';

// Revalidation time-based pour toute la route
export const revalidate = 3600; // ISR
```

### 5.5 Cache Strategies Comparison

```javascript
// Stratégie 1 : Maximum Performance (Static)
// Use case : Blog, documentation
export const dynamic = 'force-static';
async function BlogPost() {
  const post = await fetch('...', { cache: 'force-cache' });
  return <Article post={post} />;
}

// Stratégie 2 : Balanced (ISR)
// Use case : E-commerce products
export const revalidate = 3600; // 1 hour
async function ProductPage() {
  const product = await fetch('...', { 
    next: { revalidate: 3600, tags: ['products'] }
  });
  return <Product data={product} />;
}

// Stratégie 3 : Fresh Data (Dynamic + Short Cache)
// Use case : Dashboard avec données quasi-temps réel
export const revalidate = 60; // 1 minute
async function Dashboard() {
  const stats = await fetch('...', { 
    next: { revalidate: 60 } 
  });
  return <Stats data={stats} />;
}

// Stratégie 4 : Real-time (No Cache)
// Use case : User-specific data
export const dynamic = 'force-dynamic';
async function UserDashboard() {
  const userData = await fetch('...', { cache: 'no-store' });
  return <UserData data={userData} />;
}
```

---

## 6. HOOKS NEXT.JS AVANCÉS

### 6.1 useFormStatus

```javascript
'use client'

import { useFormStatus } from 'react-dom';

// ⚠️ DOIT être dans un child component du form
function SubmitButton() {
  const { pending, data, method, action } = useFormStatus();
  
  return (
    <button type="submit" disabled={pending}>
      {pending ? (
        <>⏳ Submitting...</>
      ) : (
        '✅ Submit'
      )}
    </button>
  );
}

export default function MyForm() {
  return (
    <form action={myAction}>
      <input name="email" />
      <SubmitButton /> {/* useFormStatus fonctionne ici */}
    </form>
  );
}

// Pattern avancé : Multiple submit buttons
function FormButtons() {
  const { pending } = useFormStatus();
  
  return (
    <>
      <button name="action" value="save" disabled={pending}>
        💾 Save Draft
      </button>
      <button name="action" value="publish" disabled={pending}>
        🚀 Publish
      </button>
    </>
  );
}
```

### 6.2 useFormState

```javascript
'use client'

import { useFormState } from 'react-dom';

// Server Action avec state
async function submitForm(prevState, formData) {
  'use server'
  
  const email = formData.get('email');
  
  if (!email.includes('@')) {
    return {
      error: 'Invalid email',
      email // Preserve input
    };
  }
  
  await db.user.create({ data: { email } });
  return { success: true };
}

// Client Component
export default function NewsletterForm() {
  const [state, formAction] = useFormState(submitForm, { error: null });
  
  return (
    <form action={formAction}>
      <input 
        name="email" 
        defaultValue={state?.email}
        placeholder="Enter email"
      />
      
      {state?.error && (
        <p className="text-red-500">❌ {state.error}</p>
      )}
      
      {state?.success && (
        <p className="text-green-500">✅ Subscribed!</p>
      )}
      
      <button type="submit">Subscribe</button>
    </form>
  );
}

// Pattern avancé : Multi-step form
async function handleFormStep(prevState, formData) {
  'use server'
  
  const step = prevState.step || 1;
  const data = { ...prevState.data };
  
  if (step === 1) {
    data.name = formData.get('name');
    data.email = formData.get('email');
    return { step: 2, data };
  }
  
  if (step === 2) {
    data.address = formData.get('address');
    await db.user.create({ data });
    return { step: 3, data, success: true };
  }
}

function MultiStepForm() {
  const [state, formAction] = useFormState(handleFormStep, { step: 1 });
  
  if (state.step === 1) {
    return (
      <form action={formAction}>
        <input name="name" placeholder="Name" required />
        <input name="email" placeholder="Email" required />
        <button>Next →</button>
      </form>
    );
  }
  
  if (state.step === 2) {
    return (
      <form action={formAction}>
        <p>👤 Name: {state.data.name}</p>
        <input name="address" placeholder="Address" required />
        <button>Submit ✅</button>
      </form>
    );
  }
  
  return <p>✅ Registration complete!</p>;
}
```

### 6.3 useOptimistic

```javascript
'use client'

import { useOptimistic } from 'react';
import { addToCart, removeFromCart } from '@/actions';

export default function ShoppingCart({ items }) {
  const [optimisticItems, addOptimisticItem] = useOptimistic(
    items,
    (state, newItem) => {
      if (newItem.action === 'add') {
        return [...state, newItem.item];
      }
      if (newItem.action === 'remove') {
        return state.filter(item => item.id !== newItem.itemId);
      }
      if (newItem.action === 'update') {
        return state.map(item => 
          item.id === newItem.itemId 
            ? { ...item, quantity: newItem.quantity }
            : item
        );
      }
      return state;
    }
  );
  
  async function handleAddToCart(product) {
    // ⚡ Update UI immédiatement
    addOptimisticItem({ action: 'add', item: product });
    
    // 🔄 Server mutation
    await addToCart(product.id);
  }
  
  async function handleRemove(itemId) {
    addOptimisticItem({ action: 'remove', itemId });
    await removeFromCart(itemId);
  }
  
  async function handleUpdateQuantity(itemId, quantity) {
    addOptimisticItem({ action: 'update', itemId, quantity });
    await updateCartQuantity(itemId, quantity);
  }
  
  return (
    <div>
      {optimisticItems.map(item => (
        <CartItem 
          key={item.id}
          item={item}
          onRemove={() => handleRemove(item.id)}
          onUpdateQuantity={(q) => handleUpdateQuantity(item.id, q)}
        />
      ))}
    </div>
  );
}
```

### 6.4 useTransition

```javascript
'use client'

import { useTransition, useState } from 'react';
import { useRouter } from 'next/navigation';

// Use case 1 : Navigation non-bloquante
export function SearchBar() {
  const [isPending, startTransition] = useTransition();
  const router = useRouter();
  const [query, setQuery] = useState('');
  
  function handleSearch(value) {
    setQuery(value); // ⚡ Update synchrone
    
    startTransition(() => {
      // 🔄 Navigation non-bloquante
      // L'UI reste responsive pendant la navigation
      router.push(`/search?q=${value}`);
    });
  }
  
  return (
    <div>
      <input 
        value={query}
        onChange={(e) => handleSearch(e.target.value)}
        placeholder="Search..."
      />
      {isPending && <span>⏳ Loading...</span>}
    </div>
  );
}

// Use case 2 : Filtrage avec grandes listes
export function ProductFilter({ products }) {
  const [isPending, startTransition] = useTransition();
  const [filter, setFilter] = useState('');
  const [filteredProducts, setFilteredProducts] = useState(products);
  
  function handleFilterChange(value) {
    setFilter(value); // ⚡ Update rapide de l'input
    
    startTransition(() => {
      // 🔄 Filtrage coûteux en arrière-plan
      const filtered = products.filter(p => 
        p.name.toLowerCase().includes(value.toLowerCase())
      );
      setFilteredProducts(filtered);
    });
  }
  
  return (
    <div>
      <input 
        value={filter}
        onChange={(e) => handleFilterChange(e.target.value)}
      />
      {isPending ? (
        <LoadingSkeleton />
      ) : (
        <ProductList products={filteredProducts} />
      )}
    </div>
  );
}
```

---

## 7. PATTERNS AVANCÉS & BEST PRACTICES

### 7.1 Composition Pattern (Server + Client)

```javascript
// ✅ EXCELLENT : Composition maximisant les Server Components

// app/dashboard/page.js (Server Component)
import { getUser, getStats, getOrders } from '@/lib/queries';
import { DashboardClient } from './DashboardClient';

export default async function DashboardPage() {
  // Parallel data fetching sur le serveur
  const [user, stats, orders] = await Promise.all([
    getUser(),
    getStats(),
    getOrders()
  ]);
  
  return (
    <div>
      {/* ✅ Server Components - pas de JS client */}
      <DashboardHeader user={user} />
      <StatsCards stats={stats} />
      
      {/* ⚡ Client Component isolé pour interactivité */}
      <DashboardClient 
        initialOrders={orders}
        userId={user.id}
      />
      
      {/* ✅ Retour à Server Components */}
      <RecentActivity userId={user.id} />
    </div>
  );
}

// DashboardClient.js
'use client'

export function DashboardClient({ initialOrders, userId }) {
  const [filter, setFilter] = useState('all');
  
  const filtered = initialOrders.filter(order => 
    filter === 'all' || order.status === filter
  );
  
  return (
    <div>
      <select onChange={(e) => setFilter(e.target.value)}>
        <option value="all">All Orders</option>
        <option value="pending">Pending</option>
        <option value="completed">Completed</option>
      </select>
      
      <OrdersList orders={filtered} />
    </div>
  );
}
```

### 7.2 Data Fetching Pattern (Colocation)

```javascript
// ❌ MAUVAIS : Prop drilling
async function Page() {
  const data = await fetchData();
  return <ComponentA data={data} />;
}

function ComponentA({ data }) {
  return <ComponentB data={data} />; // 😢 Prop drilling
}

function ComponentB({ data }) {
  return <ComponentC data={data} />; // 😢 Prop drilling
}

// ✅ BON : Fetch où vous en avez besoin
async function Page() {
  return <ComponentA />;
}

async function ComponentA() {
  const data = await fetchData(); // ✅ Fetch ici directement
  return <ComponentC data={data} />;
}

// ✅ EXCELLENT : Parallel + Suspense
function Page() {
  return (
    <>
      <Suspense fallback={<SkeletonA />}>
        <ComponentA />
      </Suspense>
      
      <Suspense fallback={<SkeletonB />}>
        <ComponentB />
      </Suspense>
    </>
  );
}

async function ComponentA() {
  const data = await fetchDataA(); // ⚡ Parallel avec ComponentB
  return <div>{data}</div>;
}

async function ComponentB() {
  const data = await fetchDataB(); // ⚡ Parallel avec ComponentA
  return <div>{data}</div>;
}
```

### 7.3 Authentication Pattern

```javascript
// lib/auth.js
import { cookies } from 'next/headers';
import { redirect } from 'next/navigation';

export async function getSession() {
  const cookieStore = cookies();
  const token = cookieStore.get('session');
  
  if (!token) return null;
  
  const session = await verifyToken(token.value);
  return session;
}

export async function requireAuth() {
  const session = await getSession();
  
  if (!session) {
    redirect('/login');
  }
  
  return session;
}

// app/dashboard/page.js
import { requireAuth } from '@/lib/auth';

export default async function DashboardPage() {
  const session = await requireAuth(); // ✅ Redirect si non auth
  
  const data = await fetchUserData(session.userId);
  
  return <Dashboard user={session.user} data={data} />;
}

// middleware.js - Protection routes
import { NextResponse } from 'next/server';

export function middleware(request) {
  const token = request.cookies.get('session');
  
  if (!token) {
    return NextResponse.redirect(new URL('/login', request.url));
  }
  
  return NextResponse.next();
}

export const config = {
  matcher: ['/dashboard/:path*', '/profile/:path*']
};
```

### 7.4 Revalidation Strategy Pattern

```javascript
// Stratégie 1 : Time-based global
// app/products/page.js
export const revalidate = 3600; // Toute la route

async function ProductsPage() {
  const products = await fetchProducts();
  return <ProductList products={products} />;
}

// Stratégie 2 : Per-fetch granular
async function ProductPage({ params }) {
  // Différentes strategies pour différentes données
  const [product, reviews, recommendations] = await Promise.all([
    fetch(`/api/products/${params.id}`, {
      next: { revalidate: 3600, tags: ['product'] } // 1h
    }),
    fetch(`/api/reviews/${params.id}`, {
      next: { revalidate: 300, tags: ['reviews'] } // 5min
    }),
    fetch(`/api/recommendations/${params.id}`, {
      next: { revalidate: 86400 } // 24h
    })
  ]);
  
  return (
    <>
      <ProductDetail product={product} />
      <ReviewSection reviews={reviews} />
      <Recommendations items={recommendations} />
    </>
  );
}

// Stratégie 3 : On-demand avec tags
// actions.js
'use server'

export async function updateProduct(id, data) {
  await db.product.update({ where: { id }, data });
  
  // Revalidate spécifique
  revalidateTag('product');
  revalidatePath(`/products/${id}`);
  
  // Revalidate related
  revalidateTag('recommendations');
}

export async function addReview(productId, review) {
  await db.review.create({ data: { productId, ...review } });
  
  // Revalidate juste les reviews
  revalidateTag('reviews');
}
```

---

## 8. PERFORMANCE OPTIMIZATION

### 8.1 Bundle Size Optimization

```javascript
// Dynamic imports
import dynamic from 'next/dynamic';

// Client Component chargé dynamiquement
const HeavyChart = dynamic(() => import('./HeavyChart'), {
  loading: () => <ChartSkeleton />,
  ssr: false // Pas de SSR pour ce composant
});

export default function Dashboard() {
  return (
    <div>
      <Header />
      <HeavyChart data={data} />
    </div>
  );
}

// Lazy load avec Suspense
import { lazy, Suspense } from 'react';

const AdminPanel = lazy(() => import('./AdminPanel'));

function App() {
  return (
    <Suspense fallback={<Loading />}>
      {isAdmin && <AdminPanel />}
    </Suspense>
  );
}
```

### 8.2 Image Optimization

```javascript
import Image from 'next/image';

// Optimisation automatique
export default function ProductCard({ product }) {
  return (
    <div>
      <Image
        src={product.imageUrl}
        alt={product.name}
        width={500}
        height={300}
        priority={false} // true pour above-the-fold
        placeholder="blur"
        blurDataURL={product.blurHash}
        sizes="(max-width: 768px) 100vw, 500px"
      />
    </div>
  );
}

// next.config.js - Remote images
module.exports = {
  images: {
    remotePatterns: [
      {
        protocol: 'https',
        hostname: 'images.example.com',
        pathname: '/products/**'
      }
    ],
    formats: ['image/avif', 'image/webp']
  }
};
```

### 8.3 Streaming Strategy

```javascript
// Pattern : Progressive rendering
export default function Page() {
  return (
    <div>
      {/* ⚡ Critical content - no suspense */}
      <Header />
      <Hero />
      
      {/* 🔄 Important mais pas critique */}
      <Suspense fallback={<ProductsSkeleton />}>
        <ProductsSection />
      </Suspense>
      
      {/* 🔄 Moins important */}
      <Suspense fallback={<ReviewsSkeleton />}>
        <ReviewsSection />
      </Suspense>
      
      {/* 🔄 Optionnel */}
      <Suspense fallback={null}>
        <RecommendationsSection />
      </Suspense>
    </div>
  );
}
```

---

## 9. QUESTIONS D'ENTRETIEN PROBABLES

### Q1: Différence entre Server et Client Components?

**Réponse complète:**

**Server Components:**
- ✅ Rendus sur le serveur uniquement
- ✅ Pas de JS envoyé au client (bundle size réduit)
- ✅ Accès direct DB, filesystem, secrets
- ❌ Pas d'interactivité (pas de hooks, event handlers)
- ❌ Pas de Browser APIs

**Client Components:**
- ✅ Directive `'use client'` requise
- ✅ Interactivité complète (useState, useEffect, onClick)
- ✅ Browser APIs disponibles
- ❌ Bundle size plus grand (JS client)
- ⚡ Rendus serveur (SSR) puis hydrated côté client

**Cas d'usage:**
- SC: Data fetching, layouts statiques, affichage
- CC: Forms, animations, gestion état local

---

### Q2: Expliquez les 4 layers de cache

**Réponse:**

1. **Request Memoization (React):**
   - Déduplique fetch identiques dans un render cycle
   - Automatique, temporaire (un seul render)
   - Même URL + options = une requête

2. **Data Cache (Next.js):**
   - Cache persistant des fetch responses
   - Configurable: `cache: 'force-cache'` ou `'no-store'`
   - Revalidation time-based ou on-demand

3. **Full Route Cache (Next.js):**
   - Cache du HTML + RSC payload
   - Build time pour static routes
   - Runtime pour dynamic avec ISR

4. **Router Cache (Browser):**
   - Cache client-side des routes visitées
   - 30s pour dynamic, 5min pour static
   - Prefetch automatique avec `<Link>`

---

### Q3: Comment implémenter Optimistic UI?

**Réponse:**

```javascript
'use client'
import { useOptimistic } from 'react';

function Component({ items }) {
  const [optimisticItems, addOptimistic] = useOptimistic(
    items,
    (state, newItem) => [...state, newItem]
  );
  
  async function handleAdd(item) {
    // ⚡ UI update immédiat
    addOptimistic(item);
    
    // 🔄 Server mutation async
    await serverAction(item);
  }
  
  return <List items={optimisticItems} onAdd={handleAdd} />;
}
```

**Avantages:**
- UI instantanée (meilleure UX)
- Rollback automatique si erreur
- Parfait pour likes, cart, comments

---

### Q4: SSR vs SSG vs ISR?

**Réponse:**

| Type | Génération | Performance | Fraîcheur | Use Case |
|------|-----------|-------------|-----------|----------|
| **SSG** | Build time | ⚡⚡⚡ | ❌ Stale | Blog, docs |
| **ISR** | Build + revalidate | ⚡⚡ | ✅ | E-commerce |
| **SSR** | Chaque request | ⚠️ | ✅✅ | Dashboard user |

**Exemple ISR:**
```javascript
export const revalidate = 3600; // 1 heure

async function ProductPage({ params }) {
  const product = await fetch(`/api/products/${params.id}`, {
    next: { revalidate: 3600, tags: ['products'] }
  });
  return <Product data={product} />;
}
```

---

### Q5: Comment fonctionnent les Server Actions?

**Réponse:**

- Fonctions serveur marquées `'use server'`
- Callable depuis Client Components
- POST request automatique sous le capot
- Peuvent mutate data, revalidate, redirect

**Exemple:**
```javascript
'use server'

export async function createProduct(formData) {
  const data = {
    name: formData.get('name'),
    price: parseFloat(formData.get('price'))
  };
  
  await db.product.create({ data });
  revalidatePath('/products');
  redirect('/products');
}

// Utilisable dans form
<form action={createProduct}>
  <input name="name" />
  <button>Create</button>
</form>
```

---

### Q6: Request Memoization vs Data Cache?

**Réponse:**

**Request Memoization:**
- Scope: Un seul render cycle
- Automatique pour fetch identiques
- React feature
- Pas persistant

**Data Cache:**
- Scope: Cross-requests, persistant
- Configurable (`cache`, `revalidate`)
- Next.js feature
- Persistant jusqu'à revalidation

**Exemple:**
```javascript
// Request Memoization (automatic)
async function Page() {
  return (
    <>
      <Header /> {/* fetch('/api/user') */}
      <Sidebar /> {/* fetch('/api/user') - dédupliqué */}
    </>
  );
}

// Data Cache (configurable)
fetch('/api/data', {
  next: { revalidate: 3600 } // Cache 1h
});
```

---

### Q7: useTransition vs useOptimistic?

**Réponse:**

**useTransition:**
- Updates non-bloquants
- UI reste responsive pendant changement coûteux
- Use case: Filtrage, navigation, tri

**useOptimistic:**
- UI update avant confirmation serveur
- Rollback automatique si erreur
- Use case: Like, add to cart, delete

```javascript
// useTransition
const [isPending, startTransition] = useTransition();
startTransition(() => {
  setFilter(newValue); // Non-bloquant
});

// useOptimistic
const [optimistic, addOptimistic] = useOptimistic(items);
addOptimistic(newItem); // UI immédiat
await serverAction(newItem); // Async
```

---

### Q8: Passer Server Component à Client Component?

**Réponse:**

```javascript
// ❌ IMPOSSIBLE - Import direct
'use client'
import ServerComp from './ServerComp'; // Error!

// ✅ SOLUTION - children/props pattern
// page.js (Server)
<ClientWrapper>
  <ServerComp />
</ClientWrapper>

// ClientWrapper.js
'use client'
export default function ClientWrapper({ children }) {
  return <div>{children}</div>;
}
```

**Pourquoi?**
- Server Components ne peuvent pas être serialized
- Doivent être rendus sur le serveur
- children permet de garder le boundary

---

### Q9: Stratégie cache pour e-commerce?

**Réponse:**

```javascript
// Product pages - ISR avec tags
export const revalidate = 3600;
fetch('/api/products', {
  next: { revalidate: 3600, tags: ['products'] }
});

// Cart - Dynamic (user-specific)
export const dynamic = 'force-dynamic';
fetch('/api/cart', { cache: 'no-store' });

// Homepage - ISR court
export const revalidate = 300; // 5 minutes

// Product list - ISR moyen
export const revalidate = 1800; // 30 minutes

// Dashboard - Dynamic
export const dynamic = 'force-dynamic';
```

**Revalidation on-demand:**
```javascript
'use server'
export async function updateProduct(id, data) {
  await db.product.update({ where: { id }, data });
  revalidateTag('products');
  revalidatePath('/products');
}
```

---

### Q10: Débugger problème de cache?

**Réponse:**

**Checklist:**

1. ✅ Vérifier `export const dynamic/revalidate`
2. ✅ Check fetch `cache` options
3. ✅ Console log dans SC (terminal, pas browser)
4. ✅ Supprimer `.next/cache` folder
5. ✅ Vérifier headers `x-vercel-cache` (HIT/MISS/STALE)
6. ✅ Utiliser `revalidatePath/revalidateTag` après mutations

**Outils:**
```javascript
// next.config.js
module.exports = {
  logging: {
    fetches: {
      fullUrl: true
    }
  }
};

// Voir cache status
console.log(response.headers.get('x-vercel-cache'));
```

---

## ⚠️ PIÈGES COURANTS À ÉVITER

### 1. ❌ Mettre 'use client' partout

```javascript
// ❌ MAUVAIS
'use client'
// Tout devient Client Component

// ✅ BON
// Maximiser Server Components
// Isoler 'use client' au minimum
```

### 2. ❌ Oublier revalidatePath après mutations

```javascript
// ❌ MAUVAIS
'use server'
export async function updateProduct(data) {
  await db.update(data);
  // Cache pas invalidé ! 😢
}

// ✅ BON
'use server'
export async function updateProduct(data) {
  await db.update(data);
  revalidatePath('/products'); // ✅
}
```

### 3. ❌ Waterfall data fetching

```javascript
// ❌ MAUVAIS - Séquentiel
const user = await fetchUser();
const posts = await fetchPosts();
const comments = await fetchComments();

// ✅ BON - Parallel
const [user, posts, comments] = await Promise.all([
  fetchUser(),
  fetchPosts(),
  fetchComments()
]);
```

### 4. ❌ Cache 'force-cache' pour user data

```javascript
// ❌ MAUVAIS
fetch('/api/user-profile', {
  cache: 'force-cache' // User data cached ! 😱
});

// ✅ BON
fetch('/api/user-profile', {
  cache: 'no-store' // User-specific data
});
```

### 5. ❌ Pas de loading states

```javascript
// ❌ MAUVAIS
async function Page() {
  const data = await slowFetch(); // Bloque tout
  return <Content data={data} />;
}

// ✅ BON
function Page() {
  return (
    <Suspense fallback={<Loading />}>
      <SlowComponent />
    </Suspense>
  );
}
```

---

## 🔗 LIENS AVEC VOS PROJETS

### Wild Oasis

**Server Components pour:**
- Liste cabins (SSG/ISR)
- Affichage bookings
- Dashboard stats

**Server Actions pour:**
- Create/update bookings
- Check-in/check-out
- Gestion cabins

**Optimistic UI pour:**
- Toggle cabin availability
- Quick booking confirmations

### Fast React Pizza

**SSR pour:**
- Menu dynamique
- Order tracking (user-specific)

**Server Actions pour:**
- Create order
- Update order priority

**ISR pour:**
- Menu items (revalidate 1h)
- Restaurant info

### WorldWise

**SSG pour:**
- Cities list
- Static content

**Client Components pour:**
- Map interactif
- Form add city

---

## 📚 RESSOURCES COMPLÉMENTAIRES

- Next.js Docs: https://nextjs.org/docs
- React Server Components: https://react.dev/blog/2023/03/22/react-labs-what-we-have-been-working-on-march-2023
- Caching Deep Dive: https://nextjs.org/docs/app/building-your-application/caching

---