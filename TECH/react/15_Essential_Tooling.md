## [**..**](./00_index.md)

# 📘 CHAPITRE 15 : Outils & Bibliothèques Essentiels

## 🎯 Table des Matières
1. [React Hook Form](#1-react-hook-form)
2. [Supabase](#2-supabase)
3. [NextAuth (Authentication)](#3-nextauth)
4. [React Query (TanStack Query)](#4-react-query)
5. [Date Manipulation (date-fns)](#5-date-fns)
6. [Toast Notifications](#6-toast-notifications)
7. [Zod (Validation)](#7-zod)
8. [Autres Outils Utiles](#8-autres-outils)

---

## 1. REACT HOOK FORM

### 1.1 Introduction & Setup

React Hook Form est la **solution moderne** pour gérer les formulaires React avec performance optimale.

```bash
npm install react-hook-form
```

**Pourquoi React Hook Form ?**
- ⚡ Performance (moins de re-renders)
- 🎯 API simple et intuitive
- 📦 Bundle size léger (~8kb)
- ✅ Validation native
- 🔄 Integration facile avec validation libraries

### 1.2 Basic Usage

```javascript
'use client'

import { useForm } from 'react-hook-form';

export default function LoginForm() {
  const { 
    register, 
    handleSubmit, 
    formState: { errors, isSubmitting } 
  } = useForm();
  
  async function onSubmit(data) {
    console.log(data); // { email: "...", password: "..." }
    
    // Appel API
    await fetch('/api/login', {
      method: 'POST',
      body: JSON.stringify(data)
    });
  }
  
  return (
    <form onSubmit={handleSubmit(onSubmit)} className="space-y-4">
      <div>
        <label>Email</label>
        <input 
          {...register('email', { 
            required: 'Email is required',
            pattern: {
              value: /^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,}$/i,
              message: 'Invalid email address'
            }
          })}
          type="email"
          className="w-full px-4 py-2 border rounded"
        />
        {errors.email && (
          <span className="text-red-500 text-sm">{errors.email.message}</span>
        )}
      </div>
      
      <div>
        <label>Password</label>
        <input 
          {...register('password', { 
            required: 'Password is required',
            minLength: {
              value: 8,
              message: 'Password must be at least 8 characters'
            }
          })}
          type="password"
          className="w-full px-4 py-2 border rounded"
        />
        {errors.password && (
          <span className="text-red-500 text-sm">{errors.password.message}</span>
        )}
      </div>
      
      <button 
        type="submit" 
        disabled={isSubmitting}
        className="w-full bg-blue-500 text-white py-2 rounded"
      >
        {isSubmitting ? 'Logging in...' : 'Login'}
      </button>
    </form>
  );
}
```

### 1.3 Validation Rules

```javascript
'use client'

import { useForm } from 'react-hook-form';

export function RegistrationForm() {
  const { register, handleSubmit, watch, formState: { errors } } = useForm();
  
  // Watch password pour confirmer
  const password = watch('password');
  
  const onSubmit = (data) => console.log(data);
  
  return (
    <form onSubmit={handleSubmit(onSubmit)}>
      {/* Required */}
      <input 
        {...register('username', { required: 'Username is required' })}
      />
      
      {/* Min/Max Length */}
      <input 
        {...register('password', { 
          required: 'Password is required',
          minLength: { value: 8, message: 'Min 8 characters' },
          maxLength: { value: 50, message: 'Max 50 characters' }
        })}
        type="password"
      />
      
      {/* Pattern (Regex) */}
      <input 
        {...register('phone', { 
          pattern: {
            value: /^[0-9]{10}$/,
            message: 'Invalid phone number'
          }
        })}
      />
      
      {/* Custom Validation */}
      <input 
        {...register('confirmPassword', { 
          validate: value => 
            value === password || 'Passwords do not match'
        })}
        type="password"
      />
      
      {/* Min/Max Value (numbers) */}
      <input 
        {...register('age', { 
          required: true,
          min: { value: 18, message: 'Must be 18+' },
          max: { value: 100, message: 'Invalid age' }
        })}
        type="number"
      />
      
      {/* Multiple Validations */}
      <input 
        {...register('email', { 
          required: 'Email required',
          pattern: {
            value: /^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,}$/i,
            message: 'Invalid email'
          },
          validate: async (value) => {
            const response = await fetch(`/api/check-email?email=${value}`);
            const exists = await response.json();
            return !exists || 'Email already taken';
          }
        })}
      />
      
      <button type="submit">Register</button>
    </form>
  );
}
```

### 1.4 Default Values & Reset

```javascript
'use client'

import { useForm } from 'react-hook-form';
import { useEffect } from 'react';

export function EditProfileForm({ user }) {
  const { register, handleSubmit, reset } = useForm({
    defaultValues: {
      name: user.name,
      email: user.email,
      bio: user.bio
    }
  });
  
  // Reset form quand user change
  useEffect(() => {
    reset({
      name: user.name,
      email: user.email,
      bio: user.bio
    });
  }, [user, reset]);
  
  const onSubmit = async (data) => {
    await updateUser(data);
    // Reset après succès
    reset();
  };
  
  return (
    <form onSubmit={handleSubmit(onSubmit)}>
      <input {...register('name')} />
      <input {...register('email')} />
      <textarea {...register('bio')} />
      
      <button type="submit">Save</button>
      <button type="button" onClick={() => reset()}>Cancel</button>
    </form>
  );
}
```

### 1.5 Integration avec Zod

```javascript
'use client'

import { useForm } from 'react-hook-form';
import { zodResolver } from '@hookform/resolvers/zod';
import * as z from 'zod';

// Schema Zod
const schema = z.object({
  email: z.string().email('Invalid email'),
  password: z.string().min(8, 'Min 8 characters'),
  age: z.number().min(18, 'Must be 18+'),
  terms: z.boolean().refine(val => val === true, 'You must accept terms')
});

export function SignupForm() {
  const { 
    register, 
    handleSubmit, 
    formState: { errors } 
  } = useForm({
    resolver: zodResolver(schema)
  });
  
  const onSubmit = (data) => console.log(data);
  
  return (
    <form onSubmit={handleSubmit(onSubmit)}>
      <input {...register('email')} />
      {errors.email && <span>{errors.email.message}</span>}
      
      <input {...register('password')} type="password" />
      {errors.password && <span>{errors.password.message}</span>}
      
      <input {...register('age', { valueAsNumber: true })} type="number" />
      {errors.age && <span>{errors.age.message}</span>}
      
      <input {...register('terms')} type="checkbox" />
      {errors.terms && <span>{errors.terms.message}</span>}
      
      <button type="submit">Sign Up</button>
    </form>
  );
}
```

### 1.6 Controller (Controlled Components)

```javascript
'use client'

import { useForm, Controller } from 'react-hook-form';
import Select from 'react-select';
import DatePicker from 'react-datepicker';

export function AdvancedForm() {
  const { control, handleSubmit } = useForm({
    defaultValues: {
      country: null,
      date: new Date(),
      description: ''
    }
  });
  
  return (
    <form onSubmit={handleSubmit(data => console.log(data))}>
      {/* React Select */}
      <Controller
        name="country"
        control={control}
        rules={{ required: 'Country is required' }}
        render={({ field, fieldState: { error } }) => (
          <div>
            <Select
              {...field}
              options={[
                { value: 'us', label: 'United States' },
                { value: 'fr', label: 'France' },
                { value: 'uk', label: 'United Kingdom' }
              ]}
            />
            {error && <span>{error.message}</span>}
          </div>
        )}
      />
      
      {/* Date Picker */}
      <Controller
        name="date"
        control={control}
        render={({ field }) => (
          <DatePicker
            selected={field.value}
            onChange={field.onChange}
          />
        )}
      />
      
      {/* Rich Text Editor */}
      <Controller
        name="description"
        control={control}
        render={({ field }) => (
          <textarea
            {...field}
            className="w-full h-32 p-2 border rounded"
          />
        )}
      />
      
      <button type="submit">Submit</button>
    </form>
  );
}
```

### 1.7 Form Arrays (Dynamic Fields)

```javascript
'use client'

import { useForm, useFieldArray } from 'react-hook-form';

export function TodoForm() {
  const { register, control, handleSubmit } = useForm({
    defaultValues: {
      todos: [{ title: '', completed: false }]
    }
  });
  
  const { fields, append, remove } = useFieldArray({
    control,
    name: 'todos'
  });
  
  return (
    <form onSubmit={handleSubmit(data => console.log(data))}>
      {fields.map((field, index) => (
        <div key={field.id} className="flex gap-2 mb-2">
          <input
            {...register(`todos.${index}.title`)}
            placeholder="Todo title"
            className="flex-1 px-4 py-2 border rounded"
          />
          
          <input
            {...register(`todos.${index}.completed`)}
            type="checkbox"
          />
          
          <button 
            type="button"
            onClick={() => remove(index)}
            className="px-4 py-2 bg-red-500 text-white rounded"
          >
            Delete
          </button>
        </div>
      ))}
      
      <button
        type="button"
        onClick={() => append({ title: '', completed: false })}
        className="px-4 py-2 bg-green-500 text-white rounded"
      >
        Add Todo
      </button>
      
      <button type="submit" className="ml-2 px-4 py-2 bg-blue-500 text-white rounded">
        Submit
      </button>
    </form>
  );
}
```

### 1.8 Integration avec Server Actions

```javascript
'use client'

import { useForm } from 'react-hook-form';
import { createProduct } from '@/actions/products';
import { toast } from 'sonner';

export function ProductForm() {
  const { 
    register, 
    handleSubmit, 
    reset,
    formState: { errors, isSubmitting } 
  } = useForm();
  
  async function onSubmit(data) {
    try {
      const result = await createProduct(data);
      
      if (result.error) {
        toast.error(result.error);
      } else {
        toast.success('Product created successfully!');
        reset();
      }
    } catch (error) {
      toast.error('Something went wrong');
    }
  }
  
  return (
    <form onSubmit={handleSubmit(onSubmit)} className="space-y-4">
      <input
        {...register('name', { required: 'Name is required' })}
        placeholder="Product name"
        className="w-full px-4 py-2 border rounded"
      />
      {errors.name && (
        <span className="text-red-500">{errors.name.message}</span>
      )}
      
      <input
        {...register('price', { 
          required: 'Price is required',
          valueAsNumber: true,
          min: { value: 0, message: 'Price must be positive' }
        })}
        type="number"
        step="0.01"
        placeholder="Price"
        className="w-full px-4 py-2 border rounded"
      />
      {errors.price && (
        <span className="text-red-500">{errors.price.message}</span>
      )}
      
      <button 
        type="submit" 
        disabled={isSubmitting}
        className="w-full bg-blue-500 text-white py-2 rounded disabled:opacity-50"
      >
        {isSubmitting ? 'Creating...' : 'Create Product'}
      </button>
    </form>
  );
}
```

---

## 2. SUPABASE

### 2.1 Introduction & Setup

Supabase est un **Backend-as-a-Service** open-source (alternative à Firebase).

```bash
npm install @supabase/supabase-js
```

```javascript
// lib/supabase.js
import { createClient } from '@supabase/supabase-js';

const supabaseUrl = process.env.NEXT_PUBLIC_SUPABASE_URL;
const supabaseKey = process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY;

export const supabase = createClient(supabaseUrl, supabaseKey);
```

```env
# .env.local
NEXT_PUBLIC_SUPABASE_URL=your_supabase_url
NEXT_PUBLIC_SUPABASE_ANON_KEY=your_anon_key
```

### 2.2 Database Operations (CRUD)

```javascript
// lib/api/products.js
import { supabase } from '@/lib/supabase';

// CREATE
export async function createProduct(product) {
  const { data, error } = await supabase
    .from('products')
    .insert([
      { name: product.name, price: product.price, description: product.description }
    ])
    .select();
  
  if (error) throw error;
  return data[0];
}

// READ (All)
export async function getProducts() {
  const { data, error } = await supabase
    .from('products')
    .select('*')
    .order('created_at', { ascending: false });
  
  if (error) throw error;
  return data;
}

// READ (Single)
export async function getProduct(id) {
  const { data, error } = await supabase
    .from('products')
    .select('*')
    .eq('id', id)
    .single();
  
  if (error) throw error;
  return data;
}

// UPDATE
export async function updateProduct(id, updates) {
  const { data, error } = await supabase
    .from('products')
    .update(updates)
    .eq('id', id)
    .select();
  
  if (error) throw error;
  return data[0];
}

// DELETE
export async function deleteProduct(id) {
  const { error } = await supabase
    .from('products')
    .delete()
    .eq('id', id);
  
  if (error) throw error;
}
```

### 2.3 Queries Avancées

```javascript
// Filtering
export async function getActiveProducts() {
  const { data, error } = await supabase
    .from('products')
    .select('*')
    .eq('active', true)
    .gte('price', 10)  // price >= 10
    .lte('price', 100) // price <= 100
    .ilike('name', '%phone%'); // LIKE search
  
  if (error) throw error;
  return data;
}

// Pagination
export async function getProductsPaginated(page = 1, pageSize = 10) {
  const from = (page - 1) * pageSize;
  const to = from + pageSize - 1;
  
  const { data, error, count } = await supabase
    .from('products')
    .select('*', { count: 'exact' })
    .range(from, to);
  
  if (error) throw error;
  
  return {
    data,
    total: count,
    page,
    pageSize,
    totalPages: Math.ceil(count / pageSize)
  };
}

// Joins (Relations)
export async function getOrdersWithProducts() {
  const { data, error } = await supabase
    .from('orders')
    .select(`
      *,
      customer:customers(*),
      order_items(
        *,
        product:products(*)
      )
    `);
  
  if (error) throw error;
  return data;
}

// Aggregations
export async function getProductStats() {
  const { data, error } = await supabase
    .rpc('get_product_stats'); // Stored procedure
  
  if (error) throw error;
  return data;
}
```

### 2.4 Authentication

```javascript
// lib/auth.js
import { supabase } from '@/lib/supabase';

// Sign Up
export async function signUp(email, password) {
  const { data, error } = await supabase.auth.signUp({
    email,
    password,
  });
  
  if (error) throw error;
  return data;
}

// Sign In
export async function signIn(email, password) {
  const { data, error } = await supabase.auth.signInWithPassword({
    email,
    password,
  });
  
  if (error) throw error;
  return data;
}

// Sign Out
export async function signOut() {
  const { error } = await supabase.auth.signOut();
  if (error) throw error;
}

// Get Current User
export async function getCurrentUser() {
  const { data: { user } } = await supabase.auth.getUser();
  return user;
}

// OAuth (Google, GitHub, etc.)
export async function signInWithGoogle() {
  const { data, error } = await supabase.auth.signInWithOAuth({
    provider: 'google',
    options: {
      redirectTo: `${window.location.origin}/auth/callback`
    }
  });
  
  if (error) throw error;
  return data;
}
```

### 2.5 File Storage

```javascript
// lib/storage.js
import { supabase } from '@/lib/supabase';

// Upload File
export async function uploadFile(bucket, path, file) {
  const { data, error } = await supabase.storage
    .from(bucket)
    .upload(path, file, {
      cacheControl: '3600',
      upsert: false
    });
  
  if (error) throw error;
  return data;
}

// Get Public URL
export function getPublicUrl(bucket, path) {
  const { data } = supabase.storage
    .from(bucket)
    .getPublicUrl(path);
  
  return data.publicUrl;
}

// Delete File
export async function deleteFile(bucket, path) {
  const { error } = await supabase.storage
    .from(bucket)
    .remove([path]);
  
  if (error) throw error;
}

// List Files
export async function listFiles(bucket, folder = '') {
  const { data, error } = await supabase.storage
    .from(bucket)
    .list(folder, {
      limit: 100,
      offset: 0,
      sortBy: { column: 'name', order: 'asc' }
    });
  
  if (error) throw error;
  return data;
}

// Upload Avatar Example
export async function uploadAvatar(userId, file) {
  const fileExt = file.name.split('.').pop();
  const fileName = `${userId}.${fileExt}`;
  const filePath = `avatars/${fileName}`;
  
  // Upload
  await uploadFile('avatars', filePath, file);
  
  // Get URL
  const publicUrl = getPublicUrl('avatars', filePath);
  
  // Update user profile
  await supabase
    .from('profiles')
    .update({ avatar_url: publicUrl })
    .eq('id', userId);
  
  return publicUrl;
}
```

### 2.6 Realtime Subscriptions

```javascript
'use client'

import { useEffect, useState } from 'react';
import { supabase } from '@/lib/supabase';

export function RealtimeProducts() {
  const [products, setProducts] = useState([]);
  
  useEffect(() => {
    // Fetch initial data
    async function fetchProducts() {
      const { data } = await supabase
        .from('products')
        .select('*');
      
      setProducts(data || []);
    }
    
    fetchProducts();
    
    // Subscribe to changes
    const channel = supabase
      .channel('products_changes')
      .on(
        'postgres_changes',
        {
          event: '*', // INSERT, UPDATE, DELETE
          schema: 'public',
          table: 'products'
        },
        (payload) => {
          console.log('Change received!', payload);
          
          if (payload.eventType === 'INSERT') {
            setProducts(prev => [payload.new, ...prev]);
          }
          
          if (payload.eventType === 'UPDATE') {
            setProducts(prev => 
              prev.map(p => p.id === payload.new.id ? payload.new : p)
            );
          }
          
          if (payload.eventType === 'DELETE') {
            setProducts(prev => 
              prev.filter(p => p.id !== payload.old.id)
            );
          }
        }
      )
      .subscribe();
    
    // Cleanup
    return () => {
      supabase.removeChannel(channel);
    };
  }, []);
  
  return (
    <div>
      <h2>Products (Realtime)</h2>
      {products.map(product => (
        <div key={product.id}>{product.name}</div>
      ))}
    </div>
  );
}
```

### 2.7 Row Level Security (RLS)

```sql
-- Enable RLS
ALTER TABLE products ENABLE ROW LEVEL SECURITY;

-- Policy: Users can read all products
CREATE POLICY "Products are viewable by everyone"
ON products FOR SELECT
USING (true);

-- Policy: Users can only insert their own products
CREATE POLICY "Users can insert own products"
ON products FOR INSERT
WITH CHECK (auth.uid() = user_id);

-- Policy: Users can only update their own products
CREATE POLICY "Users can update own products"
ON products FOR UPDATE
USING (auth.uid() = user_id);

-- Policy: Users can only delete their own products
CREATE POLICY "Users can delete own products"
ON products FOR DELETE
USING (auth.uid() = user_id);
```

---

## 3. NEXTAUTH (Authentication)

### 3.1 Setup NextAuth.js

```bash
npm install next-auth
```

```javascript
// app/api/auth/[...nextauth]/route.js
import NextAuth from 'next-auth';
import GoogleProvider from 'next-auth/providers/google';
import GitHubProvider from 'next-auth/providers/github';
import CredentialsProvider from 'next-auth/providers/credentials';

export const authOptions = {
  providers: [
    GoogleProvider({
      clientId: process.env.GOOGLE_CLIENT_ID,
      clientSecret: process.env.GOOGLE_CLIENT_SECRET,
    }),
    GitHubProvider({
      clientId: process.env.GITHUB_ID,
      clientSecret: process.env.GITHUB_SECRET,
    }),
    CredentialsProvider({
      name: 'Credentials',
      credentials: {
        email: { label: "Email", type: "email" },
        password: { label: "Password", type: "password" }
      },
      async authorize(credentials) {
        // Votre logique de validation
        const user = await verifyCredentials(credentials);
        
        if (user) {
          return user;
        }
        return null;
      }
    })
  ],
  callbacks: {
    async jwt({ token, user }) {
      if (user) {
        token.id = user.id;
        token.role = user.role;
      }
      return token;
    },
    async session({ session, token }) {
      session.user.id = token.id;
      session.user.role = token.role;
      return session;
    }
  },
  pages: {
    signIn: '/login',
    signOut: '/logout',
    error: '/auth/error',
  },
  session: {
    strategy: 'jwt',
  },
};

const handler = NextAuth(authOptions);
export { handler as GET, handler as POST };
```

### 3.2 Session Provider

```javascript
// app/providers.jsx
'use client'

import { SessionProvider } from 'next-auth/react';

export function Providers({ children, session }) {
  return (
    <SessionProvider session={session}>
      {children}
    </SessionProvider>
  );
}

// app/layout.js
import { Providers } from './providers';

export default function RootLayout({ children }) {
  return (
    <html>
      <body>
        <Providers>
          {children}
        </Providers>
      </body>
    </html>
  );
}
```

### 3.3 Client-Side Usage

```javascript
'use client'

import { useSession, signIn, signOut } from 'next-auth/react';

export function UserProfile() {
  const { data: session, status } = useSession();
  
  if (status === 'loading') {
    return <div>Loading...</div>;
  }
  
  if (status === 'unauthenticated') {
    return (
      <div>
        <p>Not signed in</p>
        <button onClick={() => signIn('google')}>
          Sign in with Google
        </button>
        <button onClick={() => signIn('github')}>
          Sign in with GitHub
        </button>
      </div>
    );
  }
  
  return (
    <div>
      <p>Signed in as {session.user.email}</p>
      <img src={session.user.image} alt="Avatar" />
      <button onClick={() => signOut()}>Sign out</button>
    </div>
  );
}
```

### 3.4 Server-Side Usage

```javascript
// app/dashboard/page.js
import { getServerSession } from 'next-auth';
import { authOptions } from '@/app/api/auth/[...nextauth]/route';
import { redirect } from 'next/navigation';

export default async function DashboardPage() {
  const session = await getServerSession(authOptions);
  
  if (!session) {
    redirect('/login');
  }
  
  return (
    <div>
      <h1>Dashboard</h1>
      <p>Welcome, {session.user.name}</p>
    </div>
  );
}
```

### 3.5 Protecting API Routes

```javascript
// app/api/products/route.js
import { getServerSession } from 'next-auth';
import { authOptions } from '../auth/[...nextauth]/route';
import { NextResponse } from 'next/server';

export async function GET() {
  const session = await getServerSession(authOptions);
  
  if (!session) {
    return NextResponse.json(
      { error: 'Unauthorized' },
      { status: 401 }
    );
  }
  
  // Fetch products
  const products = await getProducts();
  
  return NextResponse.json(products);
}
```

---

## 4. REACT QUERY (TanStack Query)

### 4.1 Setup

```bash
npm install @tanstack/react-query
```

```javascript
// app/providers.jsx
'use client'

import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
import { ReactQueryDevtools } from '@tanstack/react-query-devtools';
import { useState } from 'react';

export function Providers({ children }) {
  const [queryClient] = useState(() => new QueryClient({
    defaultOptions: {
      queries: {
        staleTime: 60 * 1000, // 1 minute
        cacheTime: 5 * 60 * 1000, // 5 minutes
      },
    },
  }));
  
  return (
    <QueryClientProvider client={queryClient}>
      {children}
      <ReactQueryDevtools initialIsOpen={false} />
    </QueryClientProvider>
  );
}
```

### 4.2 useQuery (Fetch Data)

```javascript
'use client'

import { useQuery } from '@tanstack/react-query';

async function fetchProducts() {
  const response = await fetch('/api/products');
  if (!response.ok) throw new Error('Failed to fetch');
  return response.json();
}

export function ProductsList() {
  const { 
    data: products, 
    isLoading, 
    isError, 
    error,
    refetch 
  } = useQuery({
    queryKey: ['products'],
    queryFn: fetchProducts,
  });
  
  if (isLoading) return <div>Loading...</div>;
  if (isError) return <div>Error: {error.message}</div>;
  
  return (
    <div>
      <button onClick={() => refetch()}>Refresh</button>
      {products.map(product => (
        <div key={product.id}>{product.name}</div>
      ))}
    </div>
  );
}
```

### 4.3 useMutation (Create/Update/Delete)

```javascript
'use client'

import { useMutation, useQueryClient } from '@tanstack/react-query';
import { toast } from 'sonner';

async function createProduct(product) {
  const response = await fetch('/api/products', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(product),
  });
  
  if (!response.ok) throw new Error('Failed to create');
  return response.json();
}

export function CreateProductForm() {
  const queryClient = useQueryClient();
  
  const mutation = useMutation({
    mutationFn: createProduct,
    onSuccess: (data) => {
      // Invalidate and refetch
      queryClient.invalidateQueries({ queryKey: ['products'] });
      toast.success('Product created!');
    },
    onError: (error) => {
      toast.error(error.message);
    }
  });
  
  function handleSubmit(e) {
    e.preventDefault();
    const formData = new FormData(e.target);
    const product = {
      name: formData.get('name'),
      price: parseFloat(formData.get('price')),
    };
    
    mutation.mutate(product);
  }
  
  return (
    <form onSubmit={handleSubmit}>
      <input name="name" required />
      <input name="price" type="number" required />
      
      <button 
        type="submit" 
        disabled={mutation.isPending}
      >
        {mutation.isPending ? 'Creating...' : 'Create Product'}
      </button>
      
      {mutation.isError && (
        <div className="text-red-500">{mutation.error.message}</div>
      )}
    </form>
  );
}
```

### 4.4 Optimistic Updates

```javascript
'use client'

import { useMutation, useQueryClient } from '@tanstack/react-query';

export function TodoItem({ todo }) {
  const queryClient = useQueryClient();
  
  const toggleMutation = useMutation({
    mutationFn: async (id) => {
      const response = await fetch(`/api/todos/${id}/toggle`, {
        method: 'PATCH',
      });
      return response.json();
    },
    // Optimistic update
    onMutate: async (id) => {
      // Cancel outgoing refetches
      await queryClient.cancelQueries({ queryKey: ['todos'] });
      
      // Snapshot previous value
      const previousTodos = queryClient.getQueryData(['todos']);
      
      // Optimistically update
      queryClient.setQueryData(['todos'], (old) =>
        old.map(t => 
          t.id === id 
            ? { ...t, completed: !t.completed }
            : t
        )
      );
      
      // Return context with snapshot
      return { previousTodos };
    },
    // Rollback on error
    onError: (err, id, context) => {
      queryClient.setQueryData(['todos'], context.previousTodos);
      toast.error('Failed to update todo');
    },
    // Always refetch after error or success
    onSettled: () => {
      queryClient.invalidateQueries({ queryKey: ['todos'] });
    },
  });
  
  return (
    <div>
      <input
        type="checkbox"
        checked={todo.completed}
        onChange={() => toggleMutation.mutate(todo.id)}
      />
      <span>{todo.title}</span>
    </div>
  );
}
```

### 4.5 Pagination

```javascript
'use client'

import { useQuery } from '@tanstack/react-query';
import { useState } from 'react';

async function fetchProducts(page = 1) {
  const response = await fetch(`/api/products?page=${page}`);
  return response.json();
}

export function PaginatedProducts() {
  const [page, setPage] = useState(1);
  
  const { data, isLoading, isPlaceholderData } = useQuery({
    queryKey: ['products', page],
    queryFn: () => fetchProducts(page),
    placeholderData: (previousData) => previousData, // Keep old data while fetching
  });
  
  return (
    <div>
      {isLoading ? (
        <div>Loading...</div>
      ) : (
        <>
          {data.products.map(product => (
            <div key={product.id}>{product.name}</div>
          ))}
          
          <div className="flex gap-2 mt-4">
            <button
              onClick={() => setPage(old => Math.max(old - 1, 1))}
              disabled={page === 1}
            >
              Previous
            </button>
            
            <span>Page {page} of {data.totalPages}</span>
            
            <button
              onClick={() => setPage(old => old + 1)}
              disabled={page === data.totalPages || isPlaceholderData}
            >
              Next
            </button>
          </div>
        </>
      )}
    </div>
  );
}
```

### 4.6 Infinite Scroll

```javascript
'use client'

import { useInfiniteQuery } from '@tanstack/react-query';
import { useEffect, useRef } from 'react';

async function fetchProducts({ pageParam = 1 }) {
  const response = await fetch(`/api/products?page=${pageParam}`);
  return response.json();
}

export function InfiniteProducts() {
  const {
    data,
    fetchNextPage,
    hasNextPage,
    isFetchingNextPage,
  } = useInfiniteQuery({
    queryKey: ['products-infinite'],
    queryFn: fetchProducts,
    getNextPageParam: (lastPage) => lastPage.nextPage,
    initialPageParam: 1,
  });
  
  const observerTarget = useRef(null);
  
  useEffect(() => {
    const observer = new IntersectionObserver(
      entries => {
        if (entries[0].isIntersecting && hasNextPage) {
          fetchNextPage();
        }
      },
      { threshold: 1 }
    );
    
    if (observerTarget.current) {
      observer.observe(observerTarget.current);
    }
    
    return () => observer.disconnect();
  }, [hasNextPage, fetchNextPage]);
  
  return (
    <div>
      {data?.pages.map((page, i) => (
        <div key={i}>
          {page.products.map(product => (
            <div key={product.id}>{product.name}</div>
          ))}
        </div>
      ))}
      
      <div ref={observerTarget} />
      
      {isFetchingNextPage && <div>Loading more...</div>}
    </div>
  );
}
```

---

## 5. DATE-FNS (Date Manipulation)

### 5.1 Setup & Basic Usage

```bash
npm install date-fns
```

```javascript
import { 
  format, 
  formatDistance, 
  formatRelative,
  parseISO,
  addDays,
  subDays,
  differenceInDays,
  isAfter,
  isBefore,
  startOfWeek,
  endOfWeek,
} from 'date-fns';
import { fr } from 'date-fns/locale'; // French locale

// Format dates
const now = new Date();

format(now, 'dd/MM/yyyy');
// "11/11/2025"

format(now, 'MMMM do, yyyy');
// "November 11th, 2025"

format(now, "yyyy-MM-dd'T'HH:mm:ss");
// "2025-11-11T14:30:00"

// Relative time
formatDistance(subDays(now, 3), now);
// "3 days"

formatDistance(subDays(now, 3), now, { addSuffix: true });
// "3 days ago"

// With locale
formatDistance(subDays(now, 3), now, { addSuffix: true, locale: fr });
// "il y a 3 jours"

// Relative formatting
formatRelative(subDays(now, 3), now);
// "last Friday at 2:30 PM"
```

### 5.2 Date Manipulation

```javascript
import { addDays, subDays, addMonths, addYears, setHours } from 'date-fns';

const today = new Date();

// Add/Subtract
addDays(today, 7);        // 7 days from now
subDays(today, 3);        // 3 days ago
addMonths(today, 2);      // 2 months from now
addYears(today, 1);       // 1 year from now

// Set specific values
setHours(today, 14);      // Set to 14:00

// Start/End of period
import { startOfDay, endOfDay, startOfMonth, endOfMonth } from 'date-fns';

startOfDay(today);        // Today at 00:00:00
endOfDay(today);          // Today at 23:59:59
startOfMonth(today);      // First day of month
endOfMonth(today);        // Last day of month
```

### 5.3 Date Comparisons

```javascript
import { 
  isAfter, 
  isBefore, 
  isEqual,
  isSameDay,
  isSameMonth,
  isWithinInterval,
  differenceInDays,
  differenceInHours,
} from 'date-fns';

const date1 = new Date('2025-11-10');
const date2 = new Date('2025-11-15');

// Comparisons
isAfter(date2, date1);    // true
isBefore(date1, date2);   // true
isEqual(date1, date1);    // true

// Same period
isSameDay(date1, date2);  // false
isSameMonth(date1, date2); // true

// Within interval
isWithinInterval(new Date('2025-11-12'), {
  start: date1,
  end: date2
}); // true

// Differences
differenceInDays(date2, date1);    // 5
differenceInHours(date2, date1);   // 120
```

### 5.4 Practical Examples

```javascript
'use client'

import { formatDistance, parseISO, differenceInDays } from 'date-fns';

// Booking component
export function BookingCard({ booking }) {
  const startDate = parseISO(booking.startDate);
  const endDate = parseISO(booking.endDate);
  const nights = differenceInDays(endDate, startDate);
  
  const timeUntilCheckIn = formatDistance(startDate, new Date(), {
    addSuffix: true
  });
  
  return (
    <div className="p-4 border rounded">
      <h3>Booking #{booking.id}</h3>
      <p>{nights} nights</p>
      <p>Check-in {timeUntilCheckIn}</p>
      <p>
        {format(startDate, 'MMM dd')} - {format(endDate, 'MMM dd, yyyy')}
      </p>
    </div>
  );
}

// Filter bookings by date range
import { isWithinInterval, startOfMonth, endOfMonth } from 'date-fns';

export function filterBookingsThisMonth(bookings) {
  const now = new Date();
  const start = startOfMonth(now);
  const end = endOfMonth(now);
  
  return bookings.filter(booking => {
    const bookingDate = parseISO(booking.createdAt);
    return isWithinInterval(bookingDate, { start, end });
  });
}

// Date picker with min/max
import { addDays, isBefore, isAfter } from 'date-fns';

export function BookingDatePicker() {
  const today = new Date();
  const minDate = addDays(today, 1);      // Tomorrow
  const maxDate = addDays(today, 365);    // 1 year from now
  
  function isDateDisabled(date) {
    return isBefore(date, minDate) || isAfter(date, maxDate);
  }
  
  return (
    <DatePicker
      minDate={minDate}
      maxDate={maxDate}
      filterDate={(date) => !isDateDisabled(date)}
    />
  );
}
```

---

## 6. TOAST NOTIFICATIONS

### 6.1 Sonner (Recommended)

```bash
npm install sonner
```

```javascript
// app/layout.js
import { Toaster } from 'sonner';

export default function RootLayout({ children }) {
  return (
    <html>
      <body>
        {children}
        <Toaster position="top-right" richColors />
      </body>
    </html>
  );
}
```

```javascript
'use client'

import { toast } from 'sonner';

export function ExampleComponent() {
  return (
    <div className="space-y-2">
      <button onClick={() => toast.success('Success!')}>
        Success Toast
      </button>
      
      <button onClick={() => toast.error('Error occurred')}>
        Error Toast
      </button>
      
      <button onClick={() => toast.info('Info message')}>
        Info Toast
      </button>
      
      <button onClick={() => toast.warning('Warning!')}>
        Warning Toast
      </button>
      
      {/* Custom toast */}
      <button onClick={() => toast('Event created', {
        description: 'Monday, January 3rd at 6:00pm',
        action: {
          label: 'Undo',
          onClick: () => console.log('Undo')
        }
      })}>
        Custom Toast
      </button>
      
      {/* Promise toast */}
      <button onClick={() => {
        const promise = fetch('/api/data').then(r => r.json());
        
        toast.promise(promise, {
          loading: 'Loading...',
          success: 'Data loaded',
          error: 'Error loading data'
        });
      }}>
        Promise Toast
      </button>
      
      {/* Loading toast with update */}
      <button onClick={async () => {
        const toastId = toast.loading('Uploading...');
        
        try {
          await uploadFile();
          toast.success('Upload complete', { id: toastId });
        } catch (error) {
          toast.error('Upload failed', { id: toastId });
        }
      }}>
        Loading Toast
      </button>
    </div>
  );
}
```

### 6.2 React Hot Toast

```bash
npm install react-hot-toast
```

```javascript
// app/layout.js
import { Toaster } from 'react-hot-toast';

export default function RootLayout({ children }) {
  return (
    <html>
      <body>
        {children}
        <Toaster 
          position="top-center"
          toastOptions={{
            duration: 4000,
            style: {
              background: '#363636',
              color: '#fff',
            },
          }}
        />
      </body>
    </html>
  );
}
```

```javascript
'use client'

import toast from 'react-hot-toast';

export function Examples() {
  return (
    <div>
      <button onClick={() => toast.success('Success!')}>
        Success
      </button>
      
      <button onClick={() => toast.error('Error!')}>
        Error
      </button>
      
      {/* Custom styling */}
      <button onClick={() => toast.success('Look at my styles', {
        style: {
          border: '1px solid #713200',
          padding: '16px',
          color: '#713200',
        },
        iconTheme: {
          primary: '#713200',
          secondary: '#FFFAEE',
        },
      })}>
        Custom Style
      </button>
      
      {/* Promise */}
      <button onClick={() => {
        const myPromise = fetchData();
        
        toast.promise(
          myPromise,
          {
            loading: 'Loading',
            success: (data) => `Successfully saved ${data.name}`,
            error: (err) => `Error: ${err.toString()}`,
          }
        );
      }}>
        Promise Toast
      </button>
    </div>
  );
}
```

---

## 7. ZOD (Validation)

### 7.1 Basic Schemas

```javascript
import { z } from 'zod';

// String validations
const stringSchema = z.string()
  .min(3, 'Min 3 characters')
  .max(50, 'Max 50 characters')
  .email('Invalid email')
  .url('Invalid URL')
  .regex(/^[A-Z]/, 'Must start with uppercase');

// Number validations
const numberSchema = z.number()
  .min(0)
  .max(100)
  .int('Must be integer')
  .positive('Must be positive')
  .nonnegative('Must be non-negative');

// Boolean
const booleanSchema = z.boolean();

// Date
const dateSchema = z.date()
  .min(new Date('2020-01-01'))
  .max(new Date('2030-12-31'));

// Array
const arraySchema = z.array(z.string())
  .min(1, 'At least 1 item')
  .max(10, 'Max 10 items')
  .nonempty('Array cannot be empty');

// Object
const userSchema = z.object({
  name: z.string().min(2),
  email: z.string().email(),
  age: z.number().min(18),
  role: z.enum(['admin', 'user', 'guest']),
});

// Optional & Nullable
const optionalSchema = z.object({
  name: z.string(),
  nickname: z.string().optional(),     // Can be undefined
  bio: z.string().nullable(),          // Can be null
  website: z.string().nullish(),       // Can be null OR undefined
});

// Default values
const withDefaultsSchema = z.object({
  name: z.string(),
  role: z.string().default('user'),
  isActive: z.boolean().default(true),
});
```

### 7.2 Complex Schemas

```javascript
import { z } from 'zod';

// Nested objects
const addressSchema = z.object({
  street: z.string(),
  city: z.string(),
  zipCode: z.string().regex(/^\d{5}$/),
});

const userWithAddressSchema = z.object({
  name: z.string(),
  address: addressSchema,
});

// Arrays of objects
const todoListSchema = z.object({
  todos: z.array(
    z.object({
      id: z.number(),
      title: z.string(),
      completed: z.boolean(),
    })
  ),
});

// Union types
const idSchema = z.union([
  z.string().uuid(),
  z.number().int(),
]);

// Discriminated unions
const responseSchema = z.discriminatedUnion('status', [
  z.object({
    status: z.literal('success'),
    data: z.any(),
  }),
  z.object({
    status: z.literal('error'),
    error: z.string(),
  }),
]);

// Record (dynamic keys)
const settingsSchema = z.record(z.string(), z.boolean());
// { [key: string]: boolean }

// Tuple
const coordinatesSchema = z.tuple([
  z.number(), // latitude
  z.number(), // longitude
]);
```

### 7.3 Refinements & Transforms

```javascript
import { z } from 'zod';

// Refine (custom validation)
const passwordSchema = z.string()
  .min(8)
  .refine(
    (val) => /[A-Z]/.test(val),
    { message: 'Must contain uppercase letter' }
  )
  .refine(
    (val) => /[0-9]/.test(val),
    { message: 'Must contain number' }
  );

// Multiple refinements
const userAgeSchema = z.object({
  birthDate: z.date(),
  parentEmail: z.string().email().optional(),
}).refine(
  (data) => {
    const age = new Date().getFullYear() - data.birthDate.getFullYear();
    return age >= 18 || data.parentEmail;
  },
  {
    message: 'Parent email required for users under 18',
    path: ['parentEmail'],
  }
);

// Transform
const stringToNumberSchema = z.string()
  .transform((val) => parseInt(val, 10))
  .pipe(z.number());

const trimmedStringSchema = z.string()
  .transform((val) => val.trim())
  .pipe(z.string().min(1));

// Preprocess
const dateFromStringSchema = z.preprocess(
  (arg) => {
    if (typeof arg === 'string') return new Date(arg);
    return arg;
  },
  z.date()
);
```

### 7.4 Usage Examples

```javascript
'use client'

import { z } from 'zod';

// Form validation schema
const signupSchema = z.object({
  email: z.string().email('Invalid email address'),
  password: z.string()
    .min(8, 'Password must be at least 8 characters')
    .regex(/[A-Z]/, 'Must contain uppercase letter')
    .regex(/[0-9]/, 'Must contain number'),
  confirmPassword: z.string(),
  terms: z.boolean().refine(val => val === true, {
    message: 'You must accept terms and conditions'
  })
}).refine(
  (data) => data.password === data.confirmPassword,
  {
    message: 'Passwords do not match',
    path: ['confirmPassword'],
  }
);

// Validate data
export function SignupForm() {
  function handleSubmit(e) {
    e.preventDefault();
    const formData = new FormData(e.target);
    
    const data = {
      email: formData.get('email'),
      password: formData.get('password'),
      confirmPassword: formData.get('confirmPassword'),
      terms: formData.get('terms') === 'on',
    };
    
    try {
      // Validate
      const validData = signupSchema.parse(data);
      
      // All good, submit
      console.log('Valid data:', validData);
      
    } catch (error) {
      if (error instanceof z.ZodError) {
        // Display errors
        error.errors.forEach(err => {
          console.log(`${err.path}: ${err.message}`);
        });
      }
    }
  }
  
  return <form onSubmit={handleSubmit}>...</form>;
}

// API validation
export async function POST(request) {
  const body = await request.json();
  
  try {
    const validated = signupSchema.parse(body);
    
    // Create user
    const user = await createUser(validated);
    
    return Response.json(user);
    
  } catch (error) {
    if (error instanceof z.ZodError) {
      return Response.json(
        { errors: error.errors },
        { status: 400 }
      );
    }
  }
}
```

### 7.5 Integration avec React Hook Form

```javascript
'use client'

import { useForm } from 'react-hook-form';
import { zodResolver } from '@hookform/resolvers/zod';
import { z } from 'zod';

const productSchema = z.object({
  name: z.string().min(3, 'Name must be at least 3 characters'),
  price: z.number().min(0, 'Price must be positive'),
  category: z.enum(['electronics', 'clothing', 'food']),
  inStock: z.boolean(),
});

export function ProductForm() {
  const {
    register,
    handleSubmit,
    formState: { errors },
  } = useForm({
    resolver: zodResolver(productSchema),
    defaultValues: {
      inStock: true,
    }
  });
  
  const onSubmit = (data) => {
    console.log('Valid data:', data);
    // data is fully typed and validated
  };
  
  return (
    <form onSubmit={handleSubmit(onSubmit)} className="space-y-4">
      <div>
        <input 
          {...register('name')} 
          placeholder="Product name"
          className="w-full px-4 py-2 border rounded"
        />
        {errors.name && (
          <span className="text-red-500">{errors.name.message}</span>
        )}
      </div>
      
      <div>
        <input 
          {...register('price', { valueAsNumber: true })} 
          type="number"
          step="0.01"
          placeholder="Price"
          className="w-full px-4 py-2 border rounded"
        />
        {errors.price && (
          <span className="text-red-500">{errors.price.message}</span>
        )}
      </div>
      
      <div>
        <select {...register('category')} className="w-full px-4 py-2 border rounded">
          <option value="">Select category</option>
          <option value="electronics">Electronics</option>
          <option value="clothing">Clothing</option>
          <option value="food">Food</option>
        </select>
        {errors.category && (
          <span className="text-red-500">{errors.category.message}</span>
        )}
      </div>
      
      <div className="flex items-center gap-2">
        <input {...register('inStock')} type="checkbox" />
        <label>In Stock</label>
      </div>
      
      <button 
        type="submit"
        className="px-4 py-2 bg-blue-500 text-white rounded"
      >
        Create Product
      </button>
    </form>
  );
}
```

---

## 8. AUTRES OUTILS UTILES

### 8.1 Lodash (Utility Functions)

```bash
npm install lodash
```

```javascript
import _ from 'lodash';

// Array utilities
const numbers = [1, 2, 3, 4, 5];

_.chunk(numbers, 2);
// [[1, 2], [3, 4], [5]]

_.uniq([1, 2, 2, 3, 3, 4]);
// [1, 2, 3, 4]

_.shuffle(numbers);
// [3, 1, 5, 2, 4]

_.take(numbers, 3);
// [1, 2, 3]

// Object utilities
const user = { name: 'John', age: 30, email: 'john@example.com' };

_.pick(user, ['name', 'email']);
// { name: 'John', email: 'john@example.com' }

_.omit(user, ['age']);
// { name: 'John', email: 'john@example.com' }

// Deep clone
const cloned = _.cloneDeep(user);

// Group by
const products = [
  { name: 'Phone', category: 'electronics' },
  { name: 'Shirt', category: 'clothing' },
  { name: 'Laptop', category: 'electronics' },
];

_.groupBy(products, 'category');
// {
//   electronics: [{name: 'Phone', ...}, {name: 'Laptop', ...}],
//   clothing: [{name: 'Shirt', ...}]
// }

// Debounce & Throttle
const handleSearch = _.debounce((query) => {
  searchAPI(query);
}, 300);

const handleScroll = _.throttle(() => {
  updateScrollPosition();
}, 100);
```

### 8.2 Axios (HTTP Client)

```bash
npm install axios
```

```javascript
import axios from 'axios';

// Create instance
const api = axios.create({
  baseURL: 'https://api.example.com',
  timeout: 10000,
  headers: {
    'Content-Type': 'application/json',
  }
});

// Interceptors
api.interceptors.request.use(
  (config) => {
    const token = localStorage.getItem('token');
    if (token) {
      config.headers.Authorization = `Bearer ${token}`;
    }
    return config;
  },
  (error) => Promise.reject(error)
);

api.interceptors.response.use(
  (response) => response,
  (error) => {
    if (error.response?.status === 401) {
      // Redirect to login
      window.location.href = '/login';
    }
    return Promise.reject(error);
  }
);

// Usage
async function getProducts() {
  try {
    const response = await api.get('/products');
    return response.data;
  } catch (error) {
    console.error('Error:', error.message);
    throw error;
  }
}

async function createProduct(product) {
  const response = await api.post('/products', product);
  return response.data;
}
```

### 8.3 React Icons

```bash
npm install react-icons
```

```javascript
import { 
  FaHome, 
  FaUser, 
  FaShoppingCart,
  FaHeart,
  FaTrash,
} from 'react-icons/fa';

import { 
  AiOutlineCheck,
  AiOutlineClose,
} from 'react-icons/ai';

import {
  BiLoaderAlt,
} from 'react-icons/bi';

export function IconExamples() {
  return (
    <div>
      <FaHome size={24} color="#3b82f6" />
      <FaUser className="text-2xl text-blue-500" />
      <FaShoppingCart />
      <FaHeart />
      
      {/* Loading spinner */}
      <BiLoaderAlt className="animate-spin" />
      
      {/* In buttons */}
      <button className="flex items-center gap-2">
        <FaTrash />
        Delete
      </button>
    </div>
  );
}
```

### 8.4 clsx / classnames

```bash
npm install clsx
```

```javascript
import clsx from 'clsx';

// Conditional classes
function Button({ variant, size, disabled }) {
  return (
    <button
      className={clsx(
        'btn',
        variant === 'primary' && 'btn-primary',
        variant === 'secondary' && 'btn-secondary',
        size === 'large' && 'btn-lg',
        disabled && 'btn-disabled'
      )}
    >
      Click
    </button>
  );
}

// With objects
const className = clsx({
  'bg-blue-500': isPrimary,
  'bg-gray-500': !isPrimary,
  'text-white': true,
  'opacity-50': disabled,
});

// Arrays
const className = clsx([
  'px-4 py-2',
  isActive && 'bg-blue-500',
  !isActive && 'bg-gray-300'
]);
```

### 8.5 Nanoid (ID Generation)

```bash
npm install nanoid
```

```javascript
import { nanoid } from 'nanoid';

// Generate unique ID
const id = nanoid();
// "V1StGXR8_Z5jdHi6B-myT"

// Custom length
const shortId = nanoid(10);
// "V1StGXR8_Z"

// Use in components
export function TodoList() {
  const [todos, setTodos] = useState([]);
  
  function addTodo(text) {
    const newTodo = {
      id: nanoid(),
      text,
      completed: false,
    };
    setTodos([...todos, newTodo]);
  }
  
  return (
    <div>
      {todos.map(todo => (
        <div key={todo.id}>{todo.text}</div>
      ))}
    </div>
  );
}
```

### 8.6 React Markdown

```bash
npm install react-markdown
```

```javascript
import ReactMarkdown from 'react-markdown';
import remarkGfm from 'remark-gfm';

export function MarkdownPreview({ content }) {
  return (
    <ReactMarkdown 
      remarkPlugins={[remarkGfm]}
      className="prose prose-lg"
    >
      {content}
    </ReactMarkdown>
  );
}

// Usage
<MarkdownPreview content={`
# Hello World

This is **bold** and this is *italic*.

- Item 1
- Item 2
- Item 3

\`\`\`javascript
const hello = 'world';
\`\`\`
`} />
```

### 8.7 Recharts (Charts)

```bash
npm install recharts
```

```javascript
'use client'

import {
  LineChart,
  Line,
  BarChart,
  Bar,
  PieChart,
  Pie,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
  ResponsiveContainer,
} from 'recharts';

const data = [
  { name: 'Jan', sales: 4000, revenue: 2400 },
  { name: 'Feb', sales: 3000, revenue: 1398 },
  { name: 'Mar', sales: 2000, revenue: 9800 },
  { name: 'Apr', sales: 2780, revenue: 3908 },
  { name: 'May', sales: 1890, revenue: 4800 },
  { name: 'Jun', sales: 2390, revenue: 3800 },
];

export function SalesChart() {
  return (
    <ResponsiveContainer width="100%" height={300}>
      <LineChart data={data}>
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="name" />
        <YAxis />
        <Tooltip />
        <Legend />
        <Line type="monotone" dataKey="sales" stroke="#3b82f6" />
        <Line type="monotone" dataKey="revenue" stroke="#10b981" />
      </LineChart>
    </ResponsiveContainer>
  );
}

export function RevenueBarChart() {
  return (
    <ResponsiveContainer width="100%" height={300}>
      <BarChart data={data}>
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="name" />
        <YAxis />
        <Tooltip />
        <Legend />
        <Bar dataKey="sales" fill="#3b82f6" />
        <Bar dataKey="revenue" fill="#10b981" />
      </BarChart>
    </ResponsiveContainer>
  );
}
```

---

## 9. QUESTIONS D'ENTRETIEN

### Q1: Pourquoi utiliser React Hook Form plutôt que state useState?

**Réponse:**

**Problèmes avec useState:**
```javascript
// ❌ Re-render à chaque frappe
function Form() {
  const [name, setName] = useState('');
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  
  // Composant re-render 3 fois si on remplit 3 champs
  return (
    <form>
      <input value={name} onChange={e => setName(e.target.value)} />
      <input value={email} onChange={e => setEmail(e.target.value)} />
      <input value={password} onChange={e => setPassword(e.target.value)} />
    </form>
  );
}
```

**Avantages React Hook Form:**
```javascript
// ✅ Pas de re-render (uncontrolled)
function Form() {
  const { register, handleSubmit } = useForm();
  
  return (
    <form onSubmit={handleSubmit(onSubmit)}>
      <input {...register('name')} />
      <input {...register('email')} />
      <input {...register('password')} />
    </form>
  );
}
```

**Comparaison:**
- Performance: RHF pas de re-render, useState re-render à chaque changement
- Bundle size: RHF ~8kb, useState 0kb mais plus de code
- Validation: RHF intégré, useState manuel
- API: RHF plus simple pour forms complexes

---

### Q2: Supabase vs Firebase - Différences?

**Réponse:**

| Feature | Supabase | Firebase |
|---------|----------|----------|
| **Open Source** | ✅ Oui | ❌ Non |
| **Database** | PostgreSQL | NoSQL (Firestore) |
| **SQL Support** | ✅ Native | ❌ Non |
| **Relations** | ✅ Easy | 🟡 Complex |
| **Pricing** | 💰 Generous free tier | 💰💰 Plus cher |
| **Self-hosting** | ✅ Possible | ❌ Non |
| **Real-time** | ✅ Oui | ✅ Oui |
| **Auth** | ✅ Oui | ✅ Oui |

**Quand utiliser Supabase:**
- Relations complexes entre données
- Besoin de SQL
- Budget limité
- Préférence open-source

**Quand utiliser Firebase:**
- Écosystème Google
- Analytics intégré
- Simple NoSQL suffit

---

### Q3: Comment gérer authentication dans Next.js?

**Réponse:**

**3 approches principales:**

**1. NextAuth (Recommandé pour OAuth):**
```javascript
// ✅ Supporte Google, GitHub, etc.
// ✅ Session management intégré
// ✅ JWT ou Database sessions

import NextAuth from 'next-auth';
import GoogleProvider from 'next-auth/providers/google';

export default NextAuth({
  providers: [GoogleProvider({...})],
  callbacks: {
    async session({ session, token }) {
      session.user.id = token.id;
      return session;
    }
  }
});
```

**2. Supabase Auth (Recommandé pour full backend):**
```javascript
// ✅ Auth + Database + Storage
// ✅ RLS (Row Level Security)
// ✅ Magic links, OAuth

const { data, error } = await supabase.auth.signInWithPassword({
  email, password
});
```

**3. Custom JWT (Maximum contrôle):**
```javascript
// ✅ Contrôle total
// ❌ Plus de code à écrire
// ❌ Sécurité à gérer

import jwt from 'jsonwebtoken';

const token = jwt.sign({ userId }, process.env.JWT_SECRET);
```

---

### Q4: React Query - Quand l'utiliser?

**Réponse:**

**✅ Utiliser React Query quand:**
- Fetching data d'une API
- Besoin de cache
- Synchronisation server state
- Pagination / Infinite scroll
- Mutations avec optimistic updates

```javascript
// ✅ Bon use case
const { data, isLoading } = useQuery({
  queryKey: ['products'],
  queryFn: fetchProducts,
  staleTime: 5 * 60 * 1000, // Cache 5 minutes
});
```

**❌ Ne PAS utiliser quand:**
- État local simple (useState suffit)
- Pas de server state
- Form state (utiliser React Hook Form)

```javascript
// ❌ Overkill
const { data: count } = useQuery({
  queryKey: ['count'],
  queryFn: () => Promise.resolve(0)
});

// ✅ Meilleur
const [count, setCount] = useState(0);
```

**Avantages principaux:**
- Cache automatique
- Background refetch
- Stale-while-revalidate
- Optimistic updates
- DevTools

---

### Q5: date-fns vs Moment.js - Pourquoi date-fns?

**Réponse:**

**date-fns avantages:**
```javascript
// ✅ Tree-shakeable (import seulement ce dont vous avez besoin)
import { format, addDays } from 'date-fns';

// ✅ Immutable (ne modifie pas l'objet original)
const tomorrow = addDays(today, 1);
// today reste inchangé

// ✅ Functional programming
const dates = [date1, date2, date3].map(d => addDays(d, 7));

// ✅ Bundle size: ~2kb par fonction
```

**Moment.js problèmes:**
```javascript
// ❌ Bundle size: ~70kb (tout importé)
import moment from 'moment';

// ❌ Mutable (modifie l'objet)
const tomorrow = moment().add(1, 'day');
// ⚠️ Object original modifié

// ❌ Deprecated (plus maintenu)
```

**Recommandation:** Toujours date-fns pour nouveaux projets

---

### Q6: Zod - Pourquoi valider côté client ET serveur?

**Réponse:**

**Client-side validation:**
```javascript
// ✅ UX immédiate
// ✅ Feedback rapide
// ❌ Peut être contournée (DevTools)

const schema = z.object({
  email: z.string().email()
});

// Validation dans le formulaire
const { data, errors } = schema.safeParse(formData);
```

**Server-side validation (CRITIQUE):**
```javascript
// ✅ Sécurité (ne peut pas être contournée)
// ✅ Protection contre attaques
// ✅ Source de vérité

// app/api/users/route.js
export async function POST(request) {
  const body = await request.json();
  
  // ⚠️ TOUJOURS valider côté serveur
  const validated = userSchema.parse(body);
  
  await db.users.create(validated);
}
```

**Best practice - Les deux:**
```javascript
// Client: UX
// Server: Sécurité

// Partager le même schema Zod
export const userSchema = z.object({...});

// Client
const form = useForm({ resolver: zodResolver(userSchema) });

// Server
const validated = userSchema.parse(body);
```

**Jamais faire confiance au client seul !**

---

### Q7: Comment gérer file uploads avec Supabase?

**Réponse:**

```javascript
'use client'

import { useState } from 'react';
import { supabase } from '@/lib/supabase';

export function FileUpload() {
  const [uploading, setUploading] = useState(false);
  
  async function handleUpload(e) {
    const file = e.target.files[0];
    if (!file) return;
    
    setUploading(true);
    
    try {
      // 1. Upload file
      const fileExt = file.name.split('.').pop();
      const fileName = `${Date.now()}.${fileExt}`;
      const filePath = `uploads/${fileName}`;
      
      const { error: uploadError } = await supabase.storage
        .from('files')
        .upload(filePath, file);
      
      if (uploadError) throw uploadError;
      
      // 2. Get public URL
      const { data: { publicUrl } } = supabase.storage
        .from('files')
        .getPublicUrl(filePath);
      
      // 3. Save URL to database
      await supabase
        .from('documents')
        .insert({ url: publicUrl, name: file.name });
      
      toast.success('File uploaded!');
      
    } catch (error) {
      toast.error(error.message);
    } finally {
      setUploading(false);
    }
  }
  
  return (
    <input 
      type="file" 
      onChange={handleUpload}
      disabled={uploading}
    />
  );
}
```

**Best practices:**
- Valider file type et size côté client
- Re-valider côté serveur (RLS policies)
- Générer unique filenames
- Utiliser buckets séparés par type
- Configurer CORS si nécessaire

---

### Q8: Toast notifications - Quand et comment?

**Réponse:**

**Quand utiliser toasts:**
```javascript
// ✅ Bon cas d'usage
- Succès d'action: "Product created"
- Erreurs non-critiques: "Failed to load comments"
- Confirmations: "Changes saved"
- Background tasks: "Export complete"

// ❌ Mauvais cas d'usage
- Erreurs critiques (utiliser modal)
- Informations importantes (utiliser alert)
- Trop fréquent (spam)
- Long contenu (utiliser modal)
```

**Pattern recommandé:**
```javascript
// Avec mutations
const mutation = useMutation({
  mutationFn: createProduct,
  onSuccess: () => {
    toast.success('Product created!');
    queryClient.invalidateQueries(['products']);
  },
  onError: (error) => {
    toast.error(error.message);
  }
});

// Avec Server Actions
async function handleSubmit(formData) {
  const result = await createProduct(formData);
  
  if (result.error) {
    toast.error(result.error);
  } else {
    toast.success('Product created!');
  }
}

// Promise toasts
toast.promise(
  uploadFile(),
  {
    loading: 'Uploading...',
    success: 'Upload complete!',
    error: 'Upload failed'
  }
);
```

**Configuration:**
```javascript
// Position: top-right pour desktop, top-center pour mobile
// Duration: 3-5 secondes
// Max visible: 3-4 toasts
// Dismiss: Auto + manual (X button)
```

---

## 🔗 LIENS AVEC VOS PROJETS

### Wild Oasis - Stack Complet

```javascript
// Features utilisées:
- React Hook Form: Booking forms, guest forms
- Supabase: Database (cabins, bookings, guests)
- Supabase Auth: Staff login
- Supabase Storage: Cabin images
- date-fns: Date calculations (nights, check-in/out)
- React Query: Cache cabins, bookings data
- Toast: Confirmations (check-in, delete)

// Exemple: Create Booking
import { useForm } from 'react-hook-form';
import { useMutation, useQueryClient } from '@tanstack/react-query';
import { createBooking } from '@/services/apiBookings';
import { toast } from 'sonner';
import { differenceInDays } from 'date-fns';

export function CreateBookingForm({ cabinId }) {
  const { register, handleSubmit, watch } = useForm();
  const queryClient = useQueryClient();
  
  const startDate = watch('startDate');
  const endDate = watch('endDate');
  const numNights = differenceInDays(
    new Date(endDate),
    new Date(startDate)
  );
  
  const mutation = useMutation({
    mutationFn: createBooking,
    onSuccess: () => {
      toast.success('Booking created!');
      queryClient.invalidateQueries(['bookings']);
    }
  });
  
  return (
    <form onSubmit={handleSubmit(mutation.mutate)}>
      <input {...register('startDate')} type="date" />
      <input {...register('endDate')} type="date" />
      <p>{numNights} nights</p>
      <button type="submit">Book</button>
    </form>
  );
}
```

### Fast React Pizza - Stack

```javascript
// Features:
- React Hook Form: Order form
- Zod: Order validation
- React Query: Fetch menu, orders
- date-fns: Delivery time estimation

// Exemple: Order Form
import { useForm } from 'react-hook-form';
import { zodResolver } from '@hookform/resolvers/zod';
import { z } from 'zod';

const orderSchema = z.object({
  name: z.string().min(3),
  phone: z.string().regex(/^\d{10}$/),
  address: z.string().min(10),
  priority: z.boolean().default(false)
});

export function OrderForm() {
  const { register, handleSubmit } = useForm({
    resolver: zodResolver(orderSchema)
  });
  
  return (
    <form onSubmit={handleSubmit(createOrder)}>
      <input {...register('name')} />
      <input {...register('phone')} />
      <input {...register('address')} />
      <input {...register('priority')} type="checkbox" />
      <button type="submit">Order Now</button>
    </form>
  );
}
```

---

## ⚠️ PIÈGES COURANTS

### 1. ❌ React Hook Form - Oublier valueAsNumber

```javascript
// ❌ MAUVAIS - price sera string "10"
<input {...register('price')} type="number" />

// ✅ BON - price sera number 10
<input {...register('price', { valueAsNumber: true })} type="number" />
```

### 2. ❌ Supabase - Oublier error handling

```javascript
// ❌ MAUVAIS - crash si erreur
const { data } = await supabase.from('products').select('*');
return data; // Peut être null !

// ✅ BON
const { data, error } = await supabase.from('products').select('*');
if (error) throw error;
return data;
```

### 3. ❌ React Query - Mauvais queryKey

```javascript
// ❌ MAUVAIS - Cache partagé entre users
useQuery({
  queryKey: ['user'],
  queryFn: fetchUser
});

// ✅ BON - Cache unique par user
useQuery({
  queryKey: ['user', userId],
  queryFn: () => fetchUser(userId)
});
```

### 4. ❌ date-fns - Mutating dates

```javascript
// ❌ MAUVAIS - Crée nouvelle date mais oublie d'assigner
const today = new Date();
addDays(today, 7); // Retourne nouvelle date, today inchangé

// ✅ BON
const today = new Date();
const nextWeek = addDays(today, 7);
```

### 5. ❌ Zod - Parse vs SafeParse

```javascript
// ❌ MAUVAIS - Throw error si invalide
const data = schema.parse(input); // Peut crash

// ✅ BON - Retourne {success, data, error}
const result = schema.safeParse(input);
if (!result.success) {
  console.log(result.error);
}
```

### 6. ❌ Toast spam

```javascript
// ❌ MAUVAIS - Toast à chaque re-render
function Component() {
  toast.success('Loaded'); // Spam !
  return <div>Content</div>;
}

// ✅ BON - Toast dans event handlers
function Component() {
  function handleSave() {
    toast.success('Saved');
  }
  return <button onClick={handleSave}>Save</button>;
}
```

---

## ✅ CHECKLIST POUR ENTRETIEN

**Maîtriser:**

- [ ] React Hook Form - register, handleSubmit, validation
- [ ] Supabase - CRUD operations, auth, storage
- [ ] NextAuth - Configuration, providers, sessions
- [ ] React Query - useQuery, useMutation, cache invalidation
- [ ] date-fns - format, add/sub, difference, comparisons
- [ ] Toast - Quand utiliser, patterns
- [ ] Zod - Schemas, validation, integration RHF

**Questions probables:**

- [ ] Pourquoi React Hook Form vs useState?
- [ ] Supabase vs Firebase?
- [ ] Comment gérer authentication?
- [ ] Quand utiliser React Query?
- [ ] date-fns vs Moment.js?
- [ ] Pourquoi valider client ET serveur?
- [ ] Comment gérer file uploads?
- [ ] Patterns toast notifications?

---

## 📚 RESSOURCES COMPLÉMENTAIRES

**Documentation officielle:**
- React Hook Form: https://react-hook-form.com
- Supabase: https://supabase.com/docs
- NextAuth: https://next-auth.js.org
- React Query: https://tanstack.com/query
- date-fns: https://date-fns.org
- Zod: https://zod.dev

**Tutoriels recommandés:**
- React Hook Form + Zod: https://www.youtube.com/watch?v=cc_xmawJ8Kg
- Supabase Full Course: https://www.youtube.com/watch?v=7uKQBl9uZ00
- React Query Crash Course: https://www.youtube.com/watch?v=novnyCaa7To

---
