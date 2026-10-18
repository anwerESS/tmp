## [**..**](./00_index.md)

# CHAPITRE 12 : Next.js Fondamentaux - Deep Dive Complet

## 🎯 Introduction : Pourquoi Next.js ?

### React vs Next.js

```javascript
// ❌ PROBLÈMES avec React pur (CRA, Vite)
const reactProblems = {
  routing: 'Pas de routing built-in (besoin React Router)',
  ssr: 'Client-side only (SEO problématique)',
  performance: 'Bundle entier chargé d\'un coup',
  api: 'Pas de backend intégré',
  optimization: 'Image/font optimization manuelle',
  deployment: 'Configuration serveur complexe',
};

// ✅ SOLUTIONS Next.js
const nextjsSolutions = {
  routing: 'File-based routing automatique',
  ssr: 'SSR, SSG, ISR built-in',
  performance: 'Code splitting automatique',
  api: 'API Routes intégrées',
  optimization: 'Image/Font optimization automatique',
  deployment: 'Vercel one-click deploy',
};

/**
 * Next.js = React Framework
 * - React pour l'UI
 * - Next.js pour infrastructure (routing, SSR, etc.)
 */
```

---

## 📁 App Router vs Pages Router

### 1. Différences Fondamentales

```javascript
// PAGES ROUTER (Old - depuis Next.js 1.0)
const pagesRouter = {
  directory: 'pages/',
  routing: 'File = Route',
  dataFetching: 'getServerSideProps, getStaticProps',
  layout: 'HOC (_app.js)',
  
  structure: `
    pages/
    ├── index.js           → /
    ├── about.js           → /about
    ├── blog/
    │   ├── index.js       → /blog
    │   └── [slug].js      → /blog/:slug
    └── api/
        └── hello.js       → /api/hello
  `,
};

// APP ROUTER (New - Next.js 13+) - MODERNE
const appRouter = {
  directory: 'app/',
  routing: 'Folder = Route segment',
  dataFetching: 'async/await direct dans components',
  layout: 'Layouts imbriqués',
  serverComponents: 'Server Components par défaut',
  
  structure: `
    app/
    ├── layout.js          → Root layout
    ├── page.js            → /
    ├── about/
    │   └── page.js        → /about
    ├── blog/
    │   ├── layout.js      → Layout pour /blog/*
    │   ├── page.js        → /blog
    │   └── [slug]/
    │       └── page.js    → /blog/:slug
    └── api/
        └── route.js       → /api/...
  `,
};

/**
 * ⚠️ FOCUS sur APP ROUTER dans ce chapitre
 * C'est le futur de Next.js et ce qu'on utilise en production
 */
```

### 2. Création d'un Projet Next.js

```bash
# Créer nouveau projet
npx create-next-app@latest my-app

# Options pendant l'installation :
# ✅ TypeScript? → No (pour ce cours)
# ✅ ESLint? → Yes
# ✅ Tailwind CSS? → Yes
# ✅ src/ directory? → No
# ✅ App Router? → Yes (IMPORTANT !)
# ✅ Import alias? → Yes (@/*)

cd my-app
npm run dev
# App disponible sur http://localhost:3000
```

---

## 🗺️ File-Based Routing (App Router)

### 1. Conventions de Fichiers

```javascript
/**
 * CONVENTIONS SPÉCIALES Next.js
 * 
 * page.js      → Définit une route publique
 * layout.js    → Wrapper partagé pour segment
 * loading.js   → UI pendant chargement
 * error.js     → Error boundary
 * not-found.js → 404 page
 * route.js     → API endpoint
 * template.js  → Layout qui re-render
 */

// Structure exemple
const structure = `
app/
├── layout.js           → Root layout (REQUIRED)
├── page.js             → Homepage /
├── loading.js          → Loading pour /
├── error.js            → Error boundary pour /
├── not-found.js        → 404 page
│
├── dashboard/
│   ├── layout.js       → Layout pour /dashboard/*
│   ├── page.js         → /dashboard
│   └── settings/
│       └── page.js     → /dashboard/settings
│
└── blog/
    ├── page.js         → /blog
    └── [slug]/
        └── page.js     → /blog/:slug (dynamic)
`;
```

### 2. Root Layout (Requis)

```javascript
// app/layout.js - OBLIGATOIRE
export const metadata = {
  title: 'My Next.js App',
  description: 'Built with Next.js 14',
};

export default function RootLayout({ children }) {
  return (
    <html lang="en">
      <body>
        {/* Header/Nav commun à toute l'app */}
        <header>
          <nav>
            <a href="/">Home</a>
            <a href="/about">About</a>
            <a href="/blog">Blog</a>
          </nav>
        </header>
        
        {/* Page content */}
        <main>{children}</main>
        
        {/* Footer commun */}
        <footer>© 2024 My App</footer>
      </body>
    </html>
  );
}

/**
 * ⚠️ IMPORTANT :
 * - Root layout DOIT contenir <html> et <body>
 * - Root layout ne peut PAS être client component
 * - Root layout wrap TOUTE l'application
 */
```

### 3. Pages Basiques

```javascript
// app/page.js - Homepage (/)
export default function HomePage() {
  return (
    <div>
      <h1>Welcome to Homepage</h1>
      <p>This is the root page</p>
    </div>
  );
}

// app/about/page.js - About page (/about)
export default function AboutPage() {
  return (
    <div>
      <h1>About Us</h1>
      <p>Learn more about our company</p>
    </div>
  );
}

// app/blog/page.js - Blog index (/blog)
export default function BlogPage() {
  return (
    <div>
      <h1>Blog Posts</h1>
      <ul>
        <li><a href="/blog/first-post">First Post</a></li>
        <li><a href="/blog/second-post">Second Post</a></li>
      </ul>
    </div>
  );
}

/**
 * Règles :
 * - Fichier DOIT s'appeler page.js (ou page.tsx)
 * - Export default requis
 * - Peut être async (on verra après)
 */
```

### 4. Dynamic Routes (Routes Dynamiques)

```javascript
// app/blog/[slug]/page.js - Route dynamique (/blog/:slug)

// Next.js passe params automatiquement
export default function BlogPostPage({ params }) {
  // params.slug contient le segment dynamique
  const { slug } = params;
  
  return (
    <div>
      <h1>Blog Post: {slug}</h1>
      <p>You are reading: {slug}</p>
    </div>
  );
}

// Exemples de routes matchées :
// /blog/first-post      → params.slug = "first-post"
// /blog/hello-world     → params.slug = "hello-world"
// /blog/123             → params.slug = "123"

/**
 * Syntaxe brackets [slug] :
 * - [slug]      → Segment dynamique simple
 * - [id]        → Autre nom (convention)
 * - [...slug]   → Catch-all (match tout)
 * - [[...slug]] → Optional catch-all
 */
```

### 5. Catch-All Routes

```javascript
// app/shop/[...slug]/page.js - Catch-all route

export default function ShopPage({ params }) {
  const { slug } = params;
  // slug est un ARRAY de segments
  
  console.log(slug);
  // /shop/electronics          → ["electronics"]
  // /shop/electronics/phones   → ["electronics", "phones"]
  // /shop/a/b/c/d             → ["a", "b", "c", "d"]
  
  return (
    <div>
      <h1>Shop</h1>
      <p>Category: {slug.join(' > ')}</p>
    </div>
  );
}

// app/docs/[[...slug]]/page.js - Optional catch-all

export default function DocsPage({ params }) {
  const { slug } = params;
  
  // /docs              → slug = undefined (root matched !)
  // /docs/intro        → slug = ["intro"]
  // /docs/api/auth     → slug = ["api", "auth"]
  
  if (!slug) {
    return <h1>Documentation Home</h1>;
  }
  
  return <h1>Docs: {slug.join(' / ')}</h1>;
}
```

### 6. Route Groups (Organisation)

```javascript
/**
 * ROUTE GROUPS : (folder)
 * Parenthèses = folder pour organisation, PAS dans URL
 */

// Structure avec route groups
const structure = `
app/
├── (marketing)/
│   ├── layout.js       → Layout pour marketing pages
│   ├── page.js         → /  (homepage)
│   ├── about/
│   │   └── page.js     → /about
│   └── contact/
│       └── page.js     → /contact
│
├── (shop)/
│   ├── layout.js       → Layout pour shop pages
│   ├── products/
│   │   └── page.js     → /products
│   └── cart/
│       └── page.js     → /cart
│
└── (dashboard)/
    ├── layout.js       → Layout pour dashboard
    ├── overview/
    │   └── page.js     → /overview
    └── analytics/
        └── page.js     → /analytics
`;

// app/(marketing)/layout.js - Layout spécifique marketing
export default function MarketingLayout({ children }) {
  return (
    <div className="marketing">
      <nav>
        {/* Navigation marketing */}
        <a href="/">Home</a>
        <a href="/about">About</a>
        <a href="/contact">Contact</a>
      </nav>
      {children}
    </div>
  );
}

// app/(dashboard)/layout.js - Layout spécifique dashboard
export default function DashboardLayout({ children }) {
  return (
    <div className="dashboard">
      <aside>
        {/* Sidebar dashboard */}
        <a href="/overview">Overview</a>
        <a href="/analytics">Analytics</a>
      </aside>
      <main>{children}</main>
    </div>
  );
}

/**
 * ✅ AVANTAGES Route Groups :
 * - Organisation logique du code
 * - Layouts différents par section
 * - Pas d'impact sur URLs
 * - Meilleure séparation des concerns
 */
```

### 7. Private Folders

```javascript
/**
 * PRIVATE FOLDERS : _folder
 * Underscore = folder privé, exclu du routing
 */

const structure = `
app/
├── _components/        → Composants privés (pas des routes)
│   ├── Header.js
│   ├── Footer.js
│   └── Button.js
│
├── _lib/              → Utilities privées
│   ├── db.js
│   └── utils.js
│
├── _styles/           → Styles privés
│   └── globals.css
│
└── dashboard/
    └── page.js        → /dashboard (route publique)
`;

// app/_components/Header.js - Composant privé
export default function Header() {
  return (
    <header>
      <h1>My App</h1>
    </header>
  );
}

// app/dashboard/page.js - Utiliser private component
import Header from '@/app/_components/Header';

export default function DashboardPage() {
  return (
    <div>
      <Header />
      <h1>Dashboard</h1>
    </div>
  );
}

/**
 * ⚠️ IMPORTANT :
 * - _components/ n'est PAS une route
 * - /components ne marche pas (404)
 * - Utilisé pour organisation seulement
 */
```

---

## 🔗 Navigation et Liens

### 1. Link Component

```javascript
// ❌ JAMAIS utiliser <a> pour navigation interne
function BadNavigation() {
  return (
    <nav>
      <a href="/about">About</a>  {/* ⚠️ Full page reload ! */}
    </nav>
  );
}

// ✅ TOUJOURS utiliser <Link> de Next.js
import Link from 'next/link';

function GoodNavigation() {
  return (
    <nav>
      <Link href="/about">About</Link>  {/* ✅ Client-side navigation */}
    </nav>
  );
}

/**
 * Avantages de <Link> :
 * - Client-side navigation (SPA-like)
 * - Prefetching automatique (routes visibles)
 * - Pas de full page reload
 * - Faster navigation
 */
```

### 2. Link Props et Options

```javascript
import Link from 'next/link';

function NavigationExamples() {
  return (
    <div>
      {/* 1. Basic link */}
      <Link href="/about">About</Link>
      
      {/* 2. Dynamic route */}
      <Link href={`/blog/${postSlug}`}>Read Post</Link>
      
      {/* 3. Avec query params */}
      <Link href="/search?q=nextjs">Search</Link>
      
      {/* 4. Object notation (plus lisible) */}
      <Link
        href={{
          pathname: '/blog/[slug]',
          query: { slug: 'hello-world' },
        }}
      >
        Blog Post
      </Link>
      
      {/* 5. Replace (pas de history) */}
      <Link href="/login" replace>
        Login
      </Link>
      
      {/* 6. Scroll to top (default: true) */}
      <Link href="/terms" scroll={false}>
        Terms (no scroll)
      </Link>
      
      {/* 7. Prefetch control */}
      <Link href="/heavy-page" prefetch={false}>
        Heavy Page (no prefetch)
      </Link>
      
      {/* 8. Custom className/styling */}
      <Link
        href="/dashboard"
        className="text-blue-600 hover:underline"
      >
        Dashboard
      </Link>
      
      {/* 9. Avec composant enfant */}
      <Link href="/profile">
        <button className="btn">Go to Profile</button>
      </Link>
    </div>
  );
}

/**
 * Props importantes :
 * - href (required) : Destination
 * - replace : Remplace history au lieu de push
 * - scroll : Scroll to top (default: true)
 * - prefetch : Prefetch en background (default: true)
 */
```

### 3. Active Link Styling

```javascript
'use client'; // Client component pour usePathname

import Link from 'next/link';
import { usePathname } from 'next/navigation';

function Navigation() {
  const pathname = usePathname();
  
  const links = [
    { href: '/', label: 'Home' },
    { href: '/about', label: 'About' },
    { href: '/blog', label: 'Blog' },
    { href: '/contact', label: 'Contact' },
  ];
  
  return (
    <nav>
      {links.map((link) => {
        const isActive = pathname === link.href;
        
        return (
          <Link
            key={link.href}
            href={link.href}
            className={isActive ? 'text-blue-600 font-bold' : 'text-gray-600'}
          >
            {link.label}
          </Link>
        );
      })}
    </nav>
  );
}

// Ou créer un composant NavLink réutilisable
function NavLink({ href, children }) {
  const pathname = usePathname();
  const isActive = pathname === href;
  
  return (
    <Link
      href={href}
      className={`
        px-3 py-2 rounded-md
        ${isActive 
          ? 'bg-blue-600 text-white' 
          : 'text-gray-700 hover:bg-gray-100'
        }
      `}
    >
      {children}
    </Link>
  );
}

// Usage
function Nav() {
  return (
    <nav className="flex gap-2">
      <NavLink href="/">Home</NavLink>
      <NavLink href="/about">About</NavLink>
      <NavLink href="/blog">Blog</NavLink>
    </nav>
  );
}
```

### 4. Programmatic Navigation

```javascript
'use client';

import { useRouter } from 'next/navigation';

function LoginForm() {
  const router = useRouter();
  
  const handleSubmit = async (e) => {
    e.preventDefault();
    
    const success = await login(credentials);
    
    if (success) {
      // Navigate programmatically
      router.push('/dashboard');
      
      // Ou avec options
      router.replace('/dashboard'); // No history
      router.back(); // Go back
      router.forward(); // Go forward
      router.refresh(); // Refresh data
    }
  };
  
  return (
    <form onSubmit={handleSubmit}>
      <input type="email" />
      <input type="password" />
      <button type="submit">Login</button>
    </form>
  );
}

// Exemple : Redirect après action
function CreatePostForm() {
  const router = useRouter();
  
  const handleCreate = async (formData) => {
    const newPost = await createPost(formData);
    
    // Redirect vers le nouveau post
    router.push(`/blog/${newPost.slug}`);
  };
  
  return <form onSubmit={handleCreate}>...</form>;
}

/**
 * useRouter methods :
 * - push(href) : Navigate avec history
 * - replace(href) : Navigate sans history
 * - back() : Retour arrière
 * - forward() : Aller avant
 * - refresh() : Refresh current route
 * - prefetch(href) : Prefetch une route
 */
```

### 5. Redirects

```javascript
// Server-side redirect (Server Component ou Route Handler)
import { redirect } from 'next/navigation';

export default async function ProfilePage() {
  const user = await getUser();
  
  // Redirect si pas authentifié
  if (!user) {
    redirect('/login');
  }
  
  return <div>Welcome {user.name}</div>;
}

// Redirect permanent (SEO)
import { permanentRedirect } from 'next/navigation';

export default function OldBlogPage() {
  // 308 Permanent Redirect
  permanentRedirect('/blog/new-url');
}

// next.config.js - Redirects configuration
module.exports = {
  async redirects() {
    return [
      {
        source: '/old-blog/:slug',
        destination: '/blog/:slug',
        permanent: true, // 308
      },
      {
        source: '/about-us',
        destination: '/about',
        permanent: false, // 307
      },
    ];
  },
};
```

---

## 📐 Layouts et Templates

### 1. Layouts Imbriqués

```javascript
// app/layout.js - Root Layout (toute l'app)
export default function RootLayout({ children }) {
  return (
    <html lang="en">
      <body>
        <header>Global Header</header>
        {children}
        <footer>Global Footer</footer>
      </body>
    </html>
  );
}

// app/dashboard/layout.js - Dashboard Layout
export default function DashboardLayout({ children }) {
  return (
    <div className="dashboard">
      <aside>
        <nav>
          <a href="/dashboard">Overview</a>
          <a href="/dashboard/analytics">Analytics</a>
          <a href="/dashboard/settings">Settings</a>
        </nav>
      </aside>
      
      <main>{children}</main>
    </div>
  );
}

// app/dashboard/page.js - Dashboard Page
export default function DashboardPage() {
  return <h1>Dashboard Overview</h1>;
}

/**
 * RÉSULTAT pour /dashboard :
 * 
 * <html>
 *   <body>
 *     <header>Global Header</header>
 *     
 *     <div className="dashboard">
 *       <aside>...</aside>
 *       <main>
 *         <h1>Dashboard Overview</h1>
 *       </main>
 *     </div>
 *     
 *     <footer>Global Footer</footer>
 *   </body>
 * </html>
 * 
 * Layouts imbriqués automatiquement !
 */
```

### 2. Templates vs Layouts

```javascript
// LAYOUT : Persiste entre navigations (state preserved)
// app/dashboard/layout.js
'use client';

import { useState } from 'react';

export default function DashboardLayout({ children }) {
  const [count, setCount] = useState(0);
  
  // count PERSISTE quand on navigue entre pages dashboard
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>
        Count: {count}
      </button>
      {children}
    </div>
  );
}

// TEMPLATE : Re-render à chaque navigation (state reset)
// app/dashboard/template.js
'use client';

import { useState } from 'react';

export default function DashboardTemplate({ children }) {
  const [count, setCount] = useState(0);
  
  // count RESET à chaque navigation
  return (
    <div>
      <button onClick={() => setCount(c => c + 1)}>
        Count: {count}
      </button>
      {children}
    </div>
  );
}

/**
 * Quand utiliser Template :
 * - Animations enter/exit à chaque page
 * - Reset state entre pages
 * - useEffect qui doit run à chaque navigation
 * 
 * Quand utiliser Layout :
 * - 99% des cas (default choice)
 * - Preserve state entre pages
 * - Performance (pas de re-render)
 */
```

---

## 🔄 Loading States

### 1. loading.js File

```javascript
// app/dashboard/loading.js - Loading UI
export default function Loading() {
  return (
    <div className="flex items-center justify-center h-screen">
      <div className="animate-spin rounded-full h-32 w-32 border-b-2 border-gray-900" />
      <p>Loading dashboard...</p>
    </div>
  );
}

/**
 * Loading UI automatique :
 * - Affiché pendant fetch de page.js
 * - Wrapped dans <Suspense> automatiquement
 * - Fonctionne avec streaming
 */
```

### 2. Skeleton UI Pattern

```javascript
// app/blog/loading.js - Skeleton pour liste de posts
export default function BlogLoading() {
  return (
    <div className="space-y-4">
      {[...Array(5)].map((_, i) => (
        <div key={i} className="animate-pulse">
          <div className="h-6 bg-gray-200 rounded w-3/4 mb-2" />
          <div className="h-4 bg-gray-200 rounded w-1/2" />
        </div>
      ))}
    </div>
  );
}

// app/_components/PostSkeleton.js - Composant réutilisable
export function PostSkeleton() {
  return (
    <div className="border rounded-lg p-4 animate-pulse">
      <div className="h-48 bg-gray-200 rounded mb-4" />
      <div className="h-6 bg-gray-200 rounded w-3/4 mb-2" />
      <div className="h-4 bg-gray-200 rounded w-1/2 mb-4" />
      <div className="space-y-2">
        <div className="h-4 bg-gray-200 rounded" />
        <div className="h-4 bg-gray-200 rounded w-5/6" />
      </div>
    </div>
  );
}
```

---

## ⚠️ Error Handling

### 1. error.js File

```javascript
'use client'; // Error boundaries MUST be Client Components

import { useEffect } from 'react';

// app/dashboard/error.js
export default function Error({ error, reset }) {
  useEffect(() => {
    // Log error to service (Sentry, LogRocket, etc.)
    console.error('Dashboard error:', error);
  }, [error]);
  
  return (
    <div className="error-container">
      <h2>Something went wrong in Dashboard!</h2>
      <p>{error.message}</p>
      
      <button
        onClick={() => reset()} // Tenter de recover
        className="btn-primary"
      >
        Try again
      </button>
    </div>
  );
}

/**
 * Error props :
 * - error : Error object
 * - reset : Function pour retry
 * 
 * ⚠️ IMPORTANT :
 * - error.js MUST be 'use client'
 * - Catch errors dans page.js et children
 * - Ne catch PAS errors dans layout.js (use error.js parent)
 */
```

### 2. Global Error Handler

```javascript
// app/global-error.js - Catch errors dans root layout
'use client';

export default function GlobalError({ error, reset }) {
  return (
    <html>
      <body>
        <div className="min-h-screen flex items-center justify-center">
          <div className="text-center">
            <h1 className="text-4xl font-bold mb-4">
              Oops! Something went wrong
            </h1>
            <p className="text-gray-600 mb-4">{error.message}</p>
            <button
              onClick={() => reset()}
              className="bg-blue-600 text-white px-4 py-2 rounded"
            >
              Try again
            </button>
          </div>
        </div>
      </body>
    </html>
  );
}
```

---

## 🚫 Not Found Pages

### 1. not-found.js

```javascript
// app/not-found.js - Global 404
import Link from 'next/link';

export default function NotFound() {
  return (
    <div className="min-h-screen flex items-center justify-center">
      <div className="text-center">
        <h1 className="text-6xl font-bold text-gray-900 mb-4">404</h1>
        <p className="text-xl text-gray-600 mb-8">
          Page not found
        </p>
        <Link
          href="/"
          className="bg-blue-600 text-white px-6 py-3 rounded-lg hover:bg-blue-700"
        >
          Go back home
        </Link>
      </div>
    </div>
  );
}

// Trigger 404 programmatically
import { notFound } from 'next/navigation';

export default async function BlogPost({ params }) {
  const post = await getPost(params.slug);
  
  if (!post) {
    notFound(); // Déclenche not-found.js
  }
  
  return <article>{post.title}</article>;
}
```

---

## 🎨 Metadata et SEO

### 1. Static Metadata

```javascript
// app/page.js - Metadata statique
export const metadata = {
  title: 'My Next.js App',
  description: 'Welcome to my awesome Next.js application',
  keywords: ['Next.js', 'React', 'Web Development'],
};

export default function HomePage() {
  return <h1>Welcome</h1>;
}
```

### 2. Dynamic Metadata

```javascript
// app/blog/[slug]/page.js - Metadata dynamique
export async function generateMetadata({ params }) {
  const post = await getPost(params.slug);
  
  if (!post) {
    return { title: 'Post Not Found' };
  }
  
  return {
    title: post.title,
    description: post.excerpt,
    openGraph: {
      title: post.title,
      description: post.excerpt,
      images: [post.coverImage],
      type: 'article',
    },
    twitter: {
      card: 'summary_large_image',
      title: post.title,
      images: [post.coverImage],
    },
  };
}

export default async function BlogPost({ params }) {
  const post = await getPost(params.slug);
  return <article>{post.content}</article>;
}
```

### 3. Template de Titre

```javascript
// app/layout.js - Template global pour titres
export const metadata = {
  title: {
    template: '%s | My App', // %s = page title
    default: 'My App - Welcome',
  },
  description: 'My awesome Next.js application',
};

// app/blog/page.js
export const metadata = {
  title: 'Blog', // Devient "Blog | My App"
};

// app/about/page.js
export const metadata = {
  title: 'About', // Devient "About | My App"
};
```

---

## 🖼️ Image Optimization

### 1. Next.js Image Component

```javascript
import Image from 'next/image';

export default function ProductPage() {
  return (
    <div>
      {/* ❌ Pas d'optimisation */}
      <img src="/product.jpg" alt="Product" />
      
      {/* ✅ Optimisation automatique */}
      <Image
        src="/product.jpg"
        alt="Product"
        width={500}
        height={300}
        priority // Charger immédiatement (above fold)
      />
      
      {/* Image responsive */}
      <div style={{ position: 'relative', width: '100%', height: '400px' }}>
        <Image
          src="/hero.jpg"
          alt="Hero"
          fill // Remplit le parent
          style={{ objectFit: 'cover' }}
        />
      </div>
    </div>
  );
}

/**
 * ✅ AVANTAGES Image Component :
 * - Lazy loading automatique
 * - Responsive images (srcset)
 * - Format moderne (WebP/AVIF)
 * - Blur placeholder
 * - Prévient layout shift
 * - Resize/optimize automatique
 */
```

### 2. Image Props Avancées

```javascript
function Gallery() {
  return (
    <div>
      {/* 1. Image avec dimensions fixes */}
      <Image
        src="/photo.jpg"
        alt="Photo"
        width={800}
        height={600}
        quality={90} // 1-100 (default: 75)
      />
      
      {/* 2. Image responsive (fill) */}
      <div style={{ position: 'relative', width: '100%', height: '400px' }}>
        <Image
          src="/banner.jpg"
          alt="Banner"
          fill
          sizes="100vw" // Hint pour responsive images
          style={{ objectFit: 'cover' }}
        />
      </div>
      
      {/* 3. Blur placeholder */}
      <Image
        src="/portrait.jpg"
        alt="Portrait"
        width={400}
        height={400}
        placeholder="blur"
        blurDataURL="data:image/jpeg;base64,..." // Base64 tiny image
      />
      
      {/* 4. Priority (above the fold) */}
      <Image
        src="/hero.jpg"
        alt="Hero"
        width={1200}
        height={600}
        priority // Pas de lazy load, charge immédiatement
      />
      
      {/* 5. Image externe */}
      <Image
        src="https://example.com/image.jpg"
        alt="External"
        width={500}
        height={300}
      />
    </div>
  );
}
```

### 3. Configuration Images

```javascript
// next.config.js
module.exports = {
  images: {
    // Domaines externes autorisés
    domains: ['example.com', 'cdn.example.com'],
    
    // Ou remote patterns (plus flexible)
    remotePatterns: [
      {
        protocol: 'https',
        hostname: '**.example.com',
        pathname: '/images/**',
      },
    ],
    
    // Formats supportés
    formats: ['image/webp', 'image/avif'],
    
    // Device sizes pour responsive
    deviceSizes: [640, 750, 828, 1080, 1200, 1920, 2048, 3840],
    
    // Image sizes
    imageSizes: [16, 32, 48, 64, 96, 128, 256, 384],
  },
};
```

---

## 🎯 Middleware

### 1. Middleware Basique

```javascript
// middleware.js (à la racine du projet)
import { NextResponse } from 'next/server';

export function middleware(request) {
  // Log toutes les requests
  console.log('Request URL:', request.url);
  console.log('Method:', request.method);
  
  // Continue vers la route
  return NextResponse.next();
}

// Spécifier quelles routes matcher
export const config = {
  matcher: [
    /*
     * Match toutes les routes SAUF :
     * - api (API routes)
     * - _next/static (static files)
     * - _next/image (image optimization)
     * - favicon.ico (favicon file)
     */
    '/((?!api|_next/static|_next/image|favicon.ico).*)',
  ],
};
```

### 2. Authentication Middleware

```javascript
// middleware.js - Protection de routes
import { NextResponse } from 'next/server';

export async function middleware(request) {
  // Lire cookie/token
  const token = request.cookies.get('auth-token')?.value;
  
  // Routes protégées
  const protectedPaths = ['/dashboard', '/profile', '/settings'];
  const isProtectedPath = protectedPaths.some(path =>
    request.nextUrl.pathname.startsWith(path)
  );
  
  // Redirect si pas de token sur route protégée
  if (isProtectedPath && !token) {
    const loginUrl = new URL('/login', request.url);
    loginUrl.searchParams.set('from', request.nextUrl.pathname);
    return NextResponse.redirect(loginUrl);
  }
  
  // Vérifier token
  if (token) {
    try {
      const user = await verifyToken(token);
      
      // Ajouter user aux headers (accessible dans page)
      const requestHeaders = new Headers(request.headers);
      requestHeaders.set('x-user-id', user.id);
      
      return NextResponse.next({
        request: {
          headers: requestHeaders,
        },
      });
    } catch (error) {
      // Token invalide → supprimer cookie
      const response = NextResponse.redirect(new URL('/login', request.url));
      response.cookies.delete('auth-token');
      return response;
    }
  }
  
  return NextResponse.next();
}

export const config = {
  matcher: ['/dashboard/:path*', '/profile/:path*', '/settings/:path*'],
};
```

### 3. Middleware Patterns Avancés

```javascript
// A/B Testing
export function middleware(request) {
  const bucket = request.cookies.get('bucket')?.value;
  
  if (!bucket) {
    // Assigner random bucket
    const newBucket = Math.random() > 0.5 ? 'a' : 'b';
    const response = NextResponse.next();
    response.cookies.set('bucket', newBucket);
    return response;
  }
  
  return NextResponse.next();
}

// Geolocation redirect
export function middleware(request) {
  const country = request.geo?.country;
  
  if (country === 'FR' && !request.nextUrl.pathname.startsWith('/fr')) {
    return NextResponse.redirect(new URL('/fr', request.url));
  }
  
  return NextResponse.next();
}

// Rate limiting (simple)
const rateLimitMap = new Map();

export function middleware(request) {
  const ip = request.ip ?? 'unknown';
  const now = Date.now();
  const windowMs = 60 * 1000; // 1 minute
  const maxRequests = 100;
  
  const userRequests = rateLimitMap.get(ip) || [];
  const recentRequests = userRequests.filter(time => now - time < windowMs);
  
  if (recentRequests.length >= maxRequests) {
    return new NextResponse('Too Many Requests', { status: 429 });
  }
  
  recentRequests.push(now);
  rateLimitMap.set(ip, recentRequests);
  
  return NextResponse.next();
}
```

---

## ⚙️ Configuration (next.config.js)

### 1. Configuration Essentielle

```javascript
// next.config.js
/** @type {import('next').NextConfig} */
const nextConfig = {
  // Images configuration
  images: {
    domains: ['cdn.example.com', 'images.unsplash.com'],
    remotePatterns: [
      {
        protocol: 'https',
        hostname: '**.amazonaws.com',
      },
    ],
    formats: ['image/webp', 'image/avif'],
  },
  
  // Redirects
  async redirects() {
    return [
      {
        source: '/old-blog/:slug',
        destination: '/blog/:slug',
        permanent: true, // 308
      },
    ];
  },
  
  // Rewrites (proxy)
  async rewrites() {
    return [
      {
        source: '/api/:path*',
        destination: 'https://api.example.com/:path*',
      },
    ];
  },
  
  // Headers
  async headers() {
    return [
      {
        source: '/api/:path*',
        headers: [
          { key: 'Access-Control-Allow-Origin', value: '*' },
        ],
      },
    ];
  },
  
  // React strict mode
  reactStrictMode: true,
  
  // Experimental features
  experimental: {
    serverActions: true,
  },
};

module.exports = nextConfig;
```

### 2. Environment Variables

```bash
# .env.local (local development, NOT committed)
DATABASE_URL=postgresql://localhost:5432/mydb
API_SECRET=super-secret-key
NEXT_PUBLIC_API_URL=http://localhost:3000/api

# .env.production (production, NOT committed)
DATABASE_URL=postgresql://prod-host:5432/mydb
NEXT_PUBLIC_API_URL=https://myapp.com/api
```

```javascript
// Utilisation dans code
// Server-side (tous les env vars accessibles)
export default async function Page() {
  const dbUrl = process.env.DATABASE_URL; // ✅ Accessible
  const secret = process.env.API_SECRET;  // ✅ Accessible
  
  return <div>Server Component</div>;
}

// Client-side (seulement NEXT_PUBLIC_*)
'use client';

export default function ClientComponent() {
  // ❌ undefined (pas de NEXT_PUBLIC_)
  const dbUrl = process.env.DATABASE_URL;
  
  // ✅ Accessible (NEXT_PUBLIC_ prefix)
  const apiUrl = process.env.NEXT_PUBLIC_API_URL;
  
  return <div>{apiUrl}</div>;
}
```

---

## 🎤 Questions d'Entretien - Next.js Fondamentaux

### Q1: "Quelle est la différence entre App Router et Pages Router ?"

**Réponse experte :**

```
**PAGES ROUTER** (Old - Next.js < 13) :
- Directory: pages/
- File = Route (pages/about.js → /about)
- Data fetching: getServerSideProps, getStaticProps
- Layouts: _app.js (HOC global)
- Pas de nested layouts
- Loading/Error: manuel

**APP ROUTER** (New - Next.js 13+) :
- Directory: app/
- Folder = Route segment
- Data fetching: async/await direct dans components
- Layouts: Nested layouts avec layout.js
- Server Components par défaut
- loading.js et error.js automatiques
- Conventions spéciales (page.js, layout.js, etc.)

**RECOMMANDATION :**
Pour nouveaux projets, toujours utiliser App Router. C'est plus puissant,
plus performant, et c'est le futur de Next.js. Il offre:
- Meilleure performance (Server Components)
- Layouts imbriqués
- Loading et error states intégrés
- Data fetching simplifié
- Streaming et Suspense built-in
```

### Q2: "Comment fonctionne le file-based routing dans App Router ?"

**Réponse avec exemples :**

```javascript
/**
 * FILE-BASED ROUTING - APP ROUTER
 * 
 * Conventions :
 * - Seul page.js crée une route publique
 * - Folders = segments de route
 * - [brackets] = dynamic segments
 * - (parentheses) = organisation (pas dans URL)
 * - _underscore = privé (exclu du routing)
 */

// EXEMPLES :

// 1. Routes basiques
// app/page.js              → /
// app/about/page.js        → /about
// app/blog/page.js         → /blog

// 2. Dynamic routes
// app/blog/[slug]/page.js  → /blog/:slug
export default function Post({ params }) {
  return <h1>{params.slug}</h1>;
}

// 3. Catch-all
// app/shop/[...categories]/page.js → /shop/*
// /shop/electronics → params.categories = ["electronics"]
// /shop/a/b/c      → params.categories = ["a", "b", "c"]

// 4. Route groups (organisation)
// app/(marketing)/page.js   → /
// app/(shop)/products/page.js → /products
// (parentheses) n'apparaissent PAS dans l'URL

// 5. Private folders
// app/_components/Header.js → Pas une route
// app/dashboard/page.js     → /dashboard (route publique)

// 6. Conventions spéciales
// layout.js    → Wrapper pour segment
// loading.js   → Loading UI
// error.js     → Error boundary
// not-found.js → 404 page
```

### Q3: "Qu'est-ce que le middleware Next.js et quand l'utiliser ?"

**Réponse complète :**

```javascript
/**
 * MIDDLEWARE NEXT.JS
 * 
 * Définition : Code qui s'exécute AVANT qu'une requête soit complétée
 * Lieu : Edge (pas dans Node.js runtime)
 * Fichier : middleware.js à la racine
 */

// EXEMPLE : Authentication
// middleware.js
import { NextResponse } from 'next/server';

export async function middleware(request) {
  const token = request.cookies.get('auth-token')?.value;
  
  // Routes protégées
  if (request.nextUrl.pathname.startsWith('/dashboard')) {
    if (!token) {
      return NextResponse.redirect(new URL('/login', request.url));
    }
  }
  
  return NextResponse.next();
}

export const config = {
  matcher: '/dashboard/:path*',
};

/**
 * QUAND UTILISER :
 * - Authentication/Authorization
 * - Redirects conditionnels
 * - Geolocation routing
 * - A/B testing
 * - Rate limiting
 * - Feature flags
 * 
 * LIMITATIONS :
 * - Edge runtime (pas de Node.js APIs)
 * - Pas de DB queries directes
 * - Bundle size limité
 * 
 * CAPACITÉS :
 * - Lire : cookies, headers, URL, geo, IP
 * - Modifier : redirect, rewrite, set headers/cookies
 */
```

### Q4: "Comment optimises-tu les images dans Next.js ?"

**Réponse pratique :**

```javascript
/**
 * IMAGE OPTIMIZATION DANS NEXT.JS
 */

// ✅ Utiliser <Image> component (TOUJOURS)
import Image from 'next/image';

export default function Page() {
  return (
    <div>
      {/* ❌ MAUVAIS */}
      <img src="/photo.jpg" alt="Photo" />
      
      {/* ✅ BON - Optimization automatique */}
      <Image
        src="/photo.jpg"
        alt="Photo"
        width={800}
        height={600}
        quality={85}
        priority={false}
      />
      
      {/* Image responsive */}
      <div style={{ position: 'relative', width: '100%', height: '400px' }}>
        <Image
          src="/hero.jpg"
          alt="Hero"
          fill
          sizes="100vw"
          style={{ objectFit: 'cover' }}
        />
      </div>
      
      {/* Above the fold */}
      <Image
        src="/hero.jpg"
        alt="Hero"
        width={1200}
        height={600}
        priority // Pas de lazy load
      />
    </div>
  );
}

/**
 * AVANTAGES automatiques :
 * - Lazy loading (sauf si priority)
 * - Responsive images (srcset)
 * - Format moderne (WebP, AVIF)
 * - Resize on-demand
 * - Blur placeholder
 * - Prevent layout shift
 * - CDN caching
 * 
 * CONFIGURATION :
 * next.config.js
 * images: {
 *   domains: ['cdn.example.com'],
 *   formats: ['image/webp', 'image/avif'],
 * }
 */
```

### Q5: "Explique les différents fichiers spéciaux dans App Router"

**Réponse structurée :**

```javascript
/**
 * FICHIERS SPÉCIAUX APP ROUTER
 */

const specialFiles = {
  // 1. page.js - REQUIS pour route publique
  page: `
    export default function Page() {
      return <h1>Public route</h1>;
    }
  `,
  
  // 2. layout.js - Wrapper partagé
  layout: `
    export default function Layout({ children }) {
      return (
        <div>
          <nav>Navigation</nav>
          {children}
        </div>
      );
    }
    // Persiste entre navigations
    // Pas de re-render lors navigation
  `,
  
  // 3. loading.js - Loading UI automatique
  loading: `
    export default function Loading() {
      return <Spinner />;
    }
    // Wrapped dans <Suspense> automatiquement
  `,
  
  // 4. error.js - Error boundary automatique
  error: `
    'use client';
    
    export default function Error({ error, reset }) {
      return (
        <div>
          <h2>Error: {error.message}</h2>
          <button onClick={reset}>Try again</button>
        </div>
      );
    }
    // DOIT être client component
  `,
  
  // 5. not-found.js - 404 page
  notFound: `
    export default function NotFound() {
      return <h1>404 - Page Not Found</h1>;
    }
  `,
  
  // 6. template.js - Re-render à chaque navigation
  template: `
    export default function Template({ children }) {
      return <div>{children}</div>;
    }
    // Contrairement à layout, re-render à chaque fois
  `,
  
  // 7. route.js - API endpoint
  route: `
    export async function GET(request) {
      return Response.json({ message: 'Hello' });
    }
  `,
};

/**
 * HIÉRARCHIE :
 * layout.js (outer)
 *   → template.js
 *     → error.js
 *       → loading.js
 *         → page.js
 * 
 * RÈGLES :
 * - page.js seul crée route publique
 * - layout.js wrap automatiquement children
 * - loading.js automatic Suspense
 * - error.js automatic error boundary
 */
```

---

## 💡 Best Practices - Next.js Fondamentaux

### Checklist Complète

```javascript
const bestPractices = {
  // 1. ROUTING
  routing: {
    always: [
      'Utiliser App Router (pas Pages Router)',
      'page.js pour routes publiques',
      'layout.js pour wrappers partagés',
      'Route groups pour organisation',
      '_folders pour fichiers privés',
    ],
    never: [
      'Mélanger App Router et Pages Router',
      'Oublier page.js (folder sans page = pas de route)',
      'Mettre logique dans layout.js (server component)',
    ],
  },
  
  // 2. NAVIGATION
  navigation: {
    always: [
      'Link pour navigation interne',
      'useRouter pour navigation programmatique',
      'Active link styling avec usePathname',
    ],
    never: [
      '<a> pour navigation interne (full page reload)',
      'window.location.href (pas de prefetch)',
    ],
  },
  
  // 3. IMAGES
  images: {
    always: [
      'Image component pour toutes les images',
      'width/height ou fill prop',
      'priority pour above-the-fold images',
      'alt text pour accessibilité',
    ],
    never: [
      '<img> tag (pas d\'optimization)',
      'Oublier domains dans next.config.js (images externes)',
    ],
  },
  
  // 4. METADATA
  metadata: {
    always: [
      'generateMetadata pour dynamic metadata',
      'Static metadata pour pages statiques',
      'Title template dans root layout',
      'OpenGraph et Twitter cards',
    ],
    never: [
      'Oublier metadata (mauvais SEO)',
      'Hardcoder metadata dynamique',
    ],
  },
  
  // 5. ERROR HANDLING
  errorHandling: {
    always: [
      'error.js pour error boundaries',
      'not-found.js pour 404s',
      'loading.js pour loading states',
      'Skeleton UI pour meilleure UX',
    ],
    never: [
      'Laisser errors non-gérées',
      'Loading spinners sans context',
    ],
  },
};
```

---

## 🔗 Liens avec Vos Projets

### Wild Oasis (Peut être migré vers Next.js)

```javascript
const wildOasisNextjs = {
  structure: `
    app/
    ├── layout.js                  → Root avec Supabase provider
    ├── page.js                    → Homepage
    ├── (auth)/
    │   ├── login/page.js
    │   └── signup/page.js
    └── (dashboard)/
        ├── layout.js              → Dashboard layout avec sidebar
        ├── dashboard/page.js
        ├── cabins/
        │   ├── page.js            → Liste cabins
        │   └── [id]/page.js       → Détail cabin
        ├── bookings/
        │   ├── page.js
        │   └── [id]/page.js
        └── settings/page.js
  `,
  
  improvements: [
    'SSR pour SEO',
    'Image optimization automatique',
    'Loading states intégrés',
    'File-based routing (plus simple)',
    'Middleware pour auth',
  ],
};
```

### Fast React Pizza (Next.js serait idéal)

```javascript
const pizzaNextjs = {
  structure: `
    app/
    ├── layout.js
    ├── page.js                    → Homepage avec menu
    ├── menu/
    │   └── page.js                → Menu complet
    ├── cart/
    │   └── page.js                → Cart page
    ├── order/
    │   ├── new/page.js            → Nouvelle commande
    │   └── [orderId]/page.js      → Tracking commande
    └── api/
        └── order/route.js         → API endpoint
  `,
  
  benefits: [
    'Image optimization pour pizzas',
    'API routes intégrées',
    'SSR pour menu (SEO)',
    'Metadata dynamique par page',
  ],
};
```

---

## 🎯 Points Clés

```javascript
const keyTakeaways = {
  appRouter: {
    modern: 'App Router est le standard moderne (Next.js 13+)',
    fileConventions: 'page.js, layout.js, loading.js, error.js',
    serverComponents: 'Server Components par défaut',
    nestedLayouts: 'Layouts imbriqués automatiques',
  },
  
  routing: {
    fileBased: 'File-based routing automatique',
    dynamic: '[slug] pour routes dynamiques',
    catchAll: '[...slug] pour catch-all routes',
    routeGroups: '(folder) pour organisation',
  },
  
  optimization: {
    images: 'Image component pour optimization automatique',
    metadata: 'generateMetadata pour SEO',
    loading: 'loading.js pour loading states',
    error: 'error.js pour error boundaries',
  },
  
  middleware: {
    usage: 'Authentication, redirects, A/B testing',
    edge: 'S\'exécute sur Edge (pas Node.js)',
    limitations: 'Pas de DB queries directes',
  },
};
```

---