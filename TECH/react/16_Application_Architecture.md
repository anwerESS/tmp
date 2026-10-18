## [**..**](./00_index.md)

# CHAPITRE 16 : Architecture d'Applications Réelles - Deep Dive Complet

## 🎯 Introduction : Pourquoi l'Architecture est Cruciale

### Le Problème des Applications qui Scalent Mal

```javascript
// ❌ MAUVAISE ARCHITECTURE (tout dans src/)
const badStructure = `
src/
├── App.jsx                    → 5000 lignes !
├── components/
│   ├── Button.jsx
│   ├── Card.jsx
│   ├── UserProfile.jsx
│   ├── BookingForm.jsx
│   ├── CabinCard.jsx
│   ├── ... (100+ components mélangés)
├── hooks/
│   ├── useUser.jsx
│   ├── useBooking.jsx
│   ├── ... (tous les hooks mélangés)
└── utils/
    ├── helpers.js             → 2000 lignes de fonctions random
    └── constants.js
`;

/**
 * PROBLÈMES :
 * - Impossible de trouver un fichier
 * - Pas de séparation des concerns
 * - Difficile à maintenir
 * - Impossible de travailler en équipe
 * - Tests difficiles
 * - Réutilisation impossible
 */

// ✅ BONNE ARCHITECTURE (feature-based)
const goodStructure = `
src/
├── features/
│   ├── authentication/
│   │   ├── components/
│   │   ├── hooks/
│   │   ├── services/
│   │   └── index.js
│   ├── bookings/
│   │   ├── components/
│   │   ├── hooks/
│   │   ├── services/
│   │   └── index.js
│   └── cabins/
│       ├── components/
│       ├── hooks/
│       ├── services/
│       └── index.js
├── shared/
│   ├── components/
│   ├── hooks/
│   ├── utils/
│   └── types/
└── App.jsx
`;

/**
 * AVANTAGES :
 * - Organisation claire par feature
 * - Facile à trouver un fichier
 * - Séparation des concerns
 * - Facile à maintenir et scale
 * - Travail en équipe facilité
 * - Tests isolés par feature
 * - Code réutilisable
 */
```

---

## 📁 Structure de Projet : Patterns Éprouvés

### 1. Feature-Based Architecture (Recommandé)

```javascript
/**
 * FEATURE-BASED STRUCTURE
 * Organiser par fonctionnalité business, pas par type technique
 */

const featureBasedStructure = `
src/
├── features/
│   ├── authentication/
│   │   ├── components/
│   │   │   ├── LoginForm.jsx
│   │   │   ├── SignupForm.jsx
│   │   │   └── ProtectedRoute.jsx
│   │   ├── hooks/
│   │   │   ├── useLogin.js
│   │   │   ├── useSignup.js
│   │   │   ├── useLogout.js
│   │   │   └── useUser.js
│   │   ├── services/
│   │   │   └── authService.js
│   │   ├── types/
│   │   │   └── auth.types.ts
│   │   └── index.js              → Exports publiques
│   │
│   ├── bookings/
│   │   ├── components/
│   │   │   ├── BookingsList.jsx
│   │   │   ├── BookingCard.jsx
│   │   │   ├── BookingDetail.jsx
│   │   │   ├── CreateBookingForm.jsx
│   │   │   └── BookingFilters.jsx
│   │   ├── hooks/
│   │   │   ├── useBookings.js
│   │   │   ├── useBooking.js
│   │   │   ├── useCreateBooking.js
│   │   │   ├── useUpdateBooking.js
│   │   │   └── useDeleteBooking.js
│   │   ├── services/
│   │   │   └── bookingsService.js
│   │   ├── utils/
│   │   │   └── bookingHelpers.js
│   │   └── index.js
│   │
│   ├── cabins/
│   │   ├── components/
│   │   │   ├── CabinsList.jsx
│   │   │   ├── CabinCard.jsx
│   │   │   ├── CabinDetail.jsx
│   │   │   └── CreateCabinForm.jsx
│   │   ├── hooks/
│   │   │   ├── useCabins.js
│   │   │   ├── useCabin.js
│   │   │   ├── useCreateCabin.js
│   │   │   └── useUpdateCabin.js
│   │   ├── services/
│   │   │   └── cabinsService.js
│   │   └── index.js
│   │
│   ├── dashboard/
│   │   ├── components/
│   │   │   ├── DashboardLayout.jsx
│   │   │   ├── StatsCards.jsx
│   │   │   └── RecentActivity.jsx
│   │   ├── hooks/
│   │   │   └── useDashboardStats.js
│   │   └── index.js
│   │
│   └── settings/
│       ├── components/
│       │   ├── SettingsForm.jsx
│       │   └── SettingRow.jsx
│       ├── hooks/
│       │   ├── useSettings.js
│       │   └── useUpdateSettings.js
│       └── index.js
│
├── shared/
│   ├── components/
│   │   ├── ui/
│   │   │   ├── Button.jsx
│   │   │   ├── Input.jsx
│   │   │   ├── Modal.jsx
│   │   │   ├── Spinner.jsx
│   │   │   └── Table.jsx
│   │   ├── layout/
│   │   │   ├── Header.jsx
│   │   │   ├── Sidebar.jsx
│   │   │   ├── Footer.jsx
│   │   │   └── AppLayout.jsx
│   │   └── form/
│   │       ├── FormRow.jsx
│   │       ├── FormError.jsx
│   │       └── FormLabel.jsx
│   │
│   ├── hooks/
│   │   ├── useLocalStorage.js
│   │   ├── useDebounce.js
│   │   ├── useClickOutside.js
│   │   └── useMediaQuery.js
│   │
│   ├── utils/
│   │   ├── formatters.js
│   │   ├── validators.js
│   │   ├── date.js
│   │   └── helpers.js
│   │
│   ├── constants/
│   │   ├── routes.js
│   │   ├── api.js
│   │   └── config.js
│   │
│   ├── types/
│   │   └── common.types.ts
│   │
│   └── lib/
│       ├── supabase.js
│       ├── queryClient.js
│       └── axios.js
│
├── pages/                        → Routes (React Router)
│   ├── Dashboard.jsx
│   ├── Bookings.jsx
│   ├── Cabins.jsx
│   ├── Settings.jsx
│   └── Login.jsx
│
├── styles/
│   ├── index.css
│   └── globals.css
│
├── App.jsx
├── main.jsx
└── vite.config.js
`;

/**
 * RÈGLES D'OR :
 * 1. Chaque feature = dossier isolé
 * 2. Tout ce qui concerne une feature est dans son dossier
 * 3. index.js expose seulement ce qui est public
 * 4. shared/ pour code réutilisable entre features
 * 5. Imports entre features interdits (sauf via index.js)
 */
```

### 2. Index.js Pattern (Public API)

```javascript
// features/bookings/index.js - Exporter seulement ce qui est public

// Components publics
export { BookingsList } from './components/BookingsList';
export { BookingDetail } from './components/BookingDetail';
export { CreateBookingForm } from './components/CreateBookingForm';

// Hooks publics
export { useBookings } from './hooks/useBookings';
export { useBooking } from './hooks/useBooking';
export { useCreateBooking } from './hooks/useCreateBooking';
export { useUpdateBooking } from './hooks/useUpdateBooking';
export { useDeleteBooking } from './hooks/useDeleteBooking';

// Types publics (si TypeScript)
export type { Booking, BookingFilters } from './types/booking.types';

/**
 * ⚠️ IMPORTANT :
 * - Ne PAS exporter BookingCard, BookingFilters (internes)
 * - Ne PAS exporter services (implementation details)
 * - Ne PAS exporter utils internes
 * 
 * Usage dans autre feature :
 * import { useBookings, BookingsList } from '@/features/bookings';
 */

// ❌ MAUVAIS - Import direct du fichier
// import { BookingsList } from '@/features/bookings/components/BookingsList';

// ✅ BON - Import via index.js
// import { BookingsList } from '@/features/bookings';
```

### 3. Services Layer Pattern

```javascript
// features/bookings/services/bookingsService.js
// Centraliser toutes les API calls

import { supabase } from '@/shared/lib/supabase';

/**
 * BOOKINGS SERVICE
 * Toutes les opérations API pour bookings
 */

// GET - Fetch all bookings
export async function getBookings(filters = {}) {
  let query = supabase
    .from('bookings')
    .select('*, cabins(*), guests(*)');
  
  // Apply filters
  if (filters.status) {
    query = query.eq('status', filters.status);
  }
  
  if (filters.sortBy) {
    query = query.order(filters.sortBy, { ascending: filters.ascending });
  }
  
  const { data, error } = await query;
  
  if (error) {
    console.error('Error fetching bookings:', error);
    throw new Error('Bookings could not be loaded');
  }
  
  return data;
}

// GET - Fetch single booking
export async function getBooking(id) {
  const { data, error } = await supabase
    .from('bookings')
    .select('*, cabins(*), guests(*)')
    .eq('id', id)
    .single();
  
  if (error) {
    console.error('Error fetching booking:', error);
    throw new Error('Booking not found');
  }
  
  return data;
}

// POST - Create booking
export async function createBooking(newBooking) {
  const { data, error } = await supabase
    .from('bookings')
    .insert([newBooking])
    .select()
    .single();
  
  if (error) {
    console.error('Error creating booking:', error);
    throw new Error('Booking could not be created');
  }
  
  return data;
}

// PATCH - Update booking
export async function updateBooking(id, updates) {
  const { data, error } = await supabase
    .from('bookings')
    .update(updates)
    .eq('id', id)
    .select()
    .single();
  
  if (error) {
    console.error('Error updating booking:', error);
    throw new Error('Booking could not be updated');
  }
  
  return data;
}

// DELETE - Delete booking
export async function deleteBooking(id) {
  const { error } = await supabase
    .from('bookings')
    .delete()
    .eq('id', id);
  
  if (error) {
    console.error('Error deleting booking:', error);
    throw new Error('Booking could not be deleted');
  }
}

/**
 * ✅ AVANTAGES :
 * - Centralisation des API calls
 * - Facile à tester (mock le service)
 * - Changement d'API facile (modifier ici seulement)
 * - Error handling centralisé
 * - Type-safe avec TypeScript
 */
```

### 4. Custom Hooks Pattern

```javascript
// features/bookings/hooks/useBookings.js
import { useQuery } from '@tanstack/react-query';
import { getBookings } from '../services/bookingsService';

/**
 * Hook pour fetcher liste de bookings
 */
export function useBookings(filters = {}) {
  const {
    data: bookings,
    isLoading,
    error,
  } = useQuery({
    queryKey: ['bookings', filters],
    queryFn: () => getBookings(filters),
    staleTime: 5 * 60 * 1000, // 5 minutes
  });
  
  return { bookings, isLoading, error };
}

// features/bookings/hooks/useCreateBooking.js
import { useMutation, useQueryClient } from '@tanstack/react-query';
import { createBooking } from '../services/bookingsService';
import toast from 'react-hot-toast';

/**
 * Hook pour créer une booking
 */
export function useCreateBooking() {
  const queryClient = useQueryClient();
  
  const {
    mutate: createBooking,
    isLoading: isCreating,
    error,
  } = useMutation({
    mutationFn: createBooking,
    
    onSuccess: () => {
      toast.success('Booking successfully created');
      queryClient.invalidateQueries({ queryKey: ['bookings'] });
    },
    
    onError: (err) => {
      toast.error(err.message);
    },
  });
  
  return { createBooking, isCreating, error };
}

// features/bookings/hooks/useUpdateBooking.js
export function useUpdateBooking() {
  const queryClient = useQueryClient();
  
  const {
    mutate: updateBooking,
    isLoading: isUpdating,
  } = useMutation({
    mutationFn: ({ id, updates }) => updateBooking(id, updates),
    
    onSuccess: (updatedBooking) => {
      toast.success('Booking successfully updated');
      
      // Update cache
      queryClient.setQueryData(['booking', updatedBooking.id], updatedBooking);
      queryClient.invalidateQueries({ queryKey: ['bookings'] });
    },
    
    onError: (err) => toast.error(err.message),
  });
  
  return { updateBooking, isUpdating };
}

// features/bookings/hooks/useDeleteBooking.js
export function useDeleteBooking() {
  const queryClient = useQueryClient();
  
  const {
    mutate: deleteBooking,
    isLoading: isDeleting,
  } = useMutation({
    mutationFn: deleteBooking,
    
    onSuccess: () => {
      toast.success('Booking successfully deleted');
      queryClient.invalidateQueries({ queryKey: ['bookings'] });
    },
    
    onError: (err) => toast.error(err.message),
  });
  
  return { deleteBooking, isDeleting };
}

/**
 * PATTERN :
 * - Un hook par opération CRUD
 * - Noms cohérents (useCreateX, useUpdateX, useDeleteX)
 * - Toute la logique React Query dans les hooks
 * - Components juste utilisent les hooks (simple)
 */
```

---

## 🎨 Component Architecture

### 1. Component Hierarchy

```javascript
/**
 * HIÉRARCHIE DE COMPONENTS
 * Pages → Features → Shared
 */

// pages/Bookings.jsx - Page level
import { BookingsList } from '@/features/bookings';
import { AppLayout } from '@/shared/components/layout';

export default function BookingsPage() {
  return (
    <AppLayout>
      <h1>Bookings Management</h1>
      <BookingsList />
    </AppLayout>
  );
}

// features/bookings/components/BookingsList.jsx - Feature level
import { useBookings } from '../hooks/useBookings';
import { BookingCard } from './BookingCard';
import { Table, Spinner } from '@/shared/components/ui';

export function BookingsList() {
  const { bookings, isLoading } = useBookings();
  
  if (isLoading) return <Spinner />;
  
  return (
    <Table>
      <Table.Header>
        <div>Guest</div>
        <div>Cabin</div>
        <div>Dates</div>
        <div>Status</div>
        <div>Actions</div>
      </Table.Header>
      
      <Table.Body>
        {bookings.map(booking => (
          <BookingCard key={booking.id} booking={booking} />
        ))}
      </Table.Body>
    </Table>
  );
}

// features/bookings/components/BookingCard.jsx - Internal component
import { useDeleteBooking } from '../hooks/useDeleteBooking';
import { Button } from '@/shared/components/ui';

export function BookingCard({ booking }) {
  const { deleteBooking, isDeleting } = useDeleteBooking();
  
  return (
    <Table.Row>
      <div>{booking.guests.fullName}</div>
      <div>{booking.cabins.name}</div>
      <div>{formatDate(booking.startDate)}</div>
      <div>{booking.status}</div>
      <div>
        <Button onClick={() => deleteBooking(booking.id)} disabled={isDeleting}>
          Delete
        </Button>
      </div>
    </Table.Row>
  );
}

// shared/components/ui/Table.jsx - Shared component
export function Table({ children }) {
  return <div className="table">{children}</div>;
}

Table.Header = function TableHeader({ children }) {
  return <div className="table-header">{children}</div>;
};

Table.Body = function TableBody({ children }) {
  return <div className="table-body">{children}</div>;
};

Table.Row = function TableRow({ children }) {
  return <div className="table-row">{children}</div>;
};

/**
 * RÈGLES :
 * - Pages orchestrent features
 * - Features orchestrent components internes
 * - Components internes utilisent shared components
 * - Shared components = réutilisables partout
 */
```

### 2. Component Composition Pattern

```javascript
// shared/components/ui/Modal.jsx - Compound Component
import { createContext, useContext, useState } from 'react';
import { createPortal } from 'react-dom';

const ModalContext = createContext();

export function Modal({ children }) {
  const [isOpen, setIsOpen] = useState(false);
  
  return (
    <ModalContext.Provider value={{ isOpen, setIsOpen }}>
      {children}
    </ModalContext.Provider>
  );
}

Modal.Open = function Open({ children, opens }) {
  const { setIsOpen } = useContext(ModalContext);
  
  return (
    <div onClick={() => setIsOpen(opens)}>
      {children}
    </div>
  );
};

Modal.Window = function Window({ children, name }) {
  const { isOpen, setIsOpen } = useContext(ModalContext);
  
  if (isOpen !== name) return null;
  
  return createPortal(
    <div className="modal-overlay" onClick={() => setIsOpen('')}>
      <div className="modal" onClick={(e) => e.stopPropagation()}>
        {children}
        <button onClick={() => setIsOpen('')}>×</button>
      </div>
    </div>,
    document.body
  );
};

// Usage dans feature
import { Modal } from '@/shared/components/ui';
import { CreateBookingForm } from './CreateBookingForm';

function BookingsList() {
  return (
    <div>
      <Modal>
        <Modal.Open opens="create-booking">
          <button>Create Booking</button>
        </Modal.Open>
        
        <Modal.Window name="create-booking">
          <CreateBookingForm />
        </Modal.Window>
      </Modal>
      
      {/* Liste des bookings */}
    </div>
  );
}
```

### 3. Container/Presentational Pattern

```javascript
// features/bookings/components/BookingDetail.jsx - Container
import { useBooking } from '../hooks/useBooking';
import { BookingDetailView } from './BookingDetailView';
import { Spinner, Error } from '@/shared/components/ui';

export function BookingDetail({ bookingId }) {
  const { booking, isLoading, error } = useBooking(bookingId);
  
  if (isLoading) return <Spinner />;
  if (error) return <Error message={error.message} />;
  
  return <BookingDetailView booking={booking} />;
}

// features/bookings/components/BookingDetailView.jsx - Presentational
export function BookingDetailView({ booking }) {
  return (
    <div className="booking-detail">
      <h2>Booking #{booking.id}</h2>
      
      <section>
        <h3>Guest Information</h3>
        <p>Name: {booking.guests.fullName}</p>
        <p>Email: {booking.guests.email}</p>
      </section>
      
      <section>
        <h3>Cabin Information</h3>
        <p>Cabin: {booking.cabins.name}</p>
        <p>Capacity: {booking.cabins.maxCapacity}</p>
      </section>
      
      <section>
        <h3>Booking Details</h3>
        <p>Check-in: {formatDate(booking.startDate)}</p>
        <p>Check-out: {formatDate(booking.endDate)}</p>
        <p>Nights: {booking.numNights}</p>
        <p>Total: {formatCurrency(booking.totalPrice)}</p>
      </section>
    </div>
  );
}

/**
 * AVANTAGES :
 * - BookingDetail gère la logique (data fetching, loading, errors)
 * - BookingDetailView est pure (facile à tester, Storybook)
 * - Séparation claire des responsabilités
 * - View réutilisable avec différentes sources de data
 */
```

---

## 🔧 State Management Strategy

### 1. Stratégie Globale

```javascript
/**
 * STATE MANAGEMENT STRATEGY
 * 
 * 3 types de state :
 * 1. Server State → React Query
 * 2. Global UI State → Context API
 * 3. Local Component State → useState
 */

const stateStrategy = {
  // 1. SERVER STATE (data depuis API/DB)
  serverState: {
    tool: 'React Query',
    examples: [
      'Bookings, Cabins, Guests',
      'User profile',
      'Settings',
    ],
    pattern: 'Custom hooks (useBookings, useCabins, etc.)',
  },
  
  // 2. GLOBAL UI STATE (partagé entre components)
  globalUIState: {
    tool: 'Context API',
    examples: [
      'Dark mode',
      'Sidebar open/closed',
      'Current user',
      'Notifications',
    ],
    pattern: 'Context + Custom Hook',
  },
  
  // 3. LOCAL STATE (component specific)
  localState: {
    tool: 'useState / useReducer',
    examples: [
      'Form inputs',
      'Modal open/closed',
      'Dropdown expanded',
      'Tabs active',
    ],
    pattern: 'Simple useState',
  },
};
```

### 2. Context Pattern pour UI State

```javascript
// shared/contexts/DarkModeContext.jsx
import { createContext, useContext, useEffect } from 'react';
import { useLocalStorage } from '@/shared/hooks/useLocalStorage';

const DarkModeContext = createContext();

export function DarkModeProvider({ children }) {
  const [isDarkMode, setIsDarkMode] = useLocalStorage('darkMode', false);
  
  useEffect(() => {
    if (isDarkMode) {
      document.documentElement.classList.add('dark');
    } else {
      document.documentElement.classList.remove('dark');
    }
  }, [isDarkMode]);
  
  const toggleDarkMode = () => setIsDarkMode(prev => !prev);
  
  return (
    <DarkModeContext.Provider value={{ isDarkMode, toggleDarkMode }}>
      {children}
    </DarkModeContext.Provider>
  );
}

// Custom hook pour accès facile
export function useDarkMode() {
  const context = useContext(DarkModeContext);
  
  if (!context) {
    throw new Error('useDarkMode must be used within DarkModeProvider');
  }
  
  return context;
}

// Usage
import { useDarkMode } from '@/shared/contexts/DarkModeContext';

function Header() {
  const { isDarkMode, toggleDarkMode } = useDarkMode();
  
  return (
    <header>
      <button onClick={toggleDarkMode}>
        {isDarkMode ? '☀️' : '🌙'}
      </button>
    </header>
  );
}
```

### 3. Provider Composition

```javascript
// App.jsx - Composer tous les providers
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
import { ReactQueryDevtools } from '@tanstack/react-query-devtools';
import { BrowserRouter } from 'react-router-dom';
import { Toaster } from 'react-hot-toast';

import { DarkModeProvider } from '@/shared/contexts/DarkModeContext';
import { AuthProvider } from '@/features/authentication';

const queryClient = new QueryClient({
  defaultOptions: {
    queries: {
      staleTime: 60 * 1000,
    },
  },
});

function App() {
  return (
    <QueryClientProvider client={queryClient}>
      <ReactQueryDevtools initialIsOpen={false} />
      
      <DarkModeProvider>
        <BrowserRouter>
          <AuthProvider>
            <AppRoutes />
            <Toaster
              position="top-center"
              gutter={12}
              toastOptions={{
                success: { duration: 3000 },
                error: { duration: 5000 },
              }}
            />
          </AuthProvider>
        </BrowserRouter>
      </DarkModeProvider>
    </QueryClientProvider>
  );
}

export default App;

/**
 * ORDRE IMPORTANT :
 * 1. QueryClientProvider (le plus haut - nécessaire pour React Query)
 * 2. DarkModeProvider (UI global)
 * 3. BrowserRouter (routing)
 * 4. AuthProvider (dépend de React Query)
 * 5. App content
 */
```

---

## 🛣️ Routing Architecture

### 1. Routes Organization

```javascript
// src/routes/index.jsx - Centralized routing
import { Routes, Route, Navigate } from 'react-router-dom';
import { lazy, Suspense } from 'react';

import { AppLayout } from '@/shared/components/layout';
import { ProtectedRoute } from '@/features/authentication';
import { Spinner } from '@/shared/components/ui';

// Lazy load pages pour code splitting
const Dashboard = lazy(() => import('@/pages/Dashboard'));
const Bookings = lazy(() => import('@/pages/Bookings'));
const BookingDetail = lazy(() => import('@/pages/BookingDetail'));
const Cabins = lazy(() => import('@/pages/Cabins'));
const Settings = lazy(() => import('@/pages/Settings'));
const Users = lazy(() => import('@/pages/Users'));
const Login = lazy(() => import('@/pages/Login'));
const NotFound = lazy(() => import('@/pages/NotFound'));

export function AppRoutes() {
  return (
    <Suspense fallback={<Spinner />}>
      <Routes>
        {/* Public routes */}
        <Route path="/login" element={<Login />} />
        
        {/* Protected routes */}
        <Route
          element={
            <ProtectedRoute>
              <AppLayout />
            </ProtectedRoute>
          }
        >
          <Route index element={<Navigate to="dashboard" replace />} />
          <Route path="dashboard" element={<Dashboard />} />
          
          <Route path="bookings">
            <Route index element={<Bookings />} />
            <Route path=":bookingId" element={<BookingDetail />} />
          </Route>
          
          <Route path="cabins" element={<Cabins />} />
          <Route path="settings" element={<Settings />} />
          <Route path="users" element={<Users />} />
        </Route>
        
        {/* 404 */}
        <Route path="*" element={<NotFound />} />
      </Routes>
    </Suspense>
  );
}

/**
 * AVANTAGES :
 * - Routes centralisées (facile à voir toute la structure)
 * - Lazy loading automatique (performance)
 * - Protected routes avec layout
 * - Nested routes pour features
 */
```

### 2. Protected Route Pattern

```javascript
// features/authentication/components/ProtectedRoute.jsx
import { useUser } from '../hooks/useUser';
import { Navigate, useLocation } from 'react-router-dom';
import { Spinner } from '@/shared/components/ui';

export function ProtectedRoute({ children }) {
  const { user, isLoading, isAuthenticated } = useUser();
  const location = useLocation();
  
  // Loading
  if (isLoading) {
    return (
      <div className="flex items-center justify-center h-screen">
        <Spinner />
      </div>
    );
  }
  
  // Not authenticated → redirect to login
  if (!isAuthenticated) {
    return <Navigate to="/login" state={{ from: location }} replace />;
  }
  
  // Authenticated → render children
  return children;
}

/**
 * USAGE :
 * Wrap routes qui nécessitent authentication
 * Redirect automatique vers /login si pas authentifié
 * Preserve location pour redirect après login
 */
```

---

## 📝 Forms Architecture

### 1. React Hook Form Pattern

```javascript
// features/bookings/components/CreateBookingForm.jsx
import { useForm } from 'react-hook-form';
import { useCreateBooking } from '../hooks/useCreateBooking';
import { Button, FormRow, Input, Select } from '@/shared/components/ui';

export function CreateBookingForm({ onClose }) {
  const { createBooking, isCreating } = useCreateBooking();
  
  const {
    register,
    handleSubmit,
    formState: { errors },
    reset,
  } = useForm();
  
  function onSubmit(data) {
    createBooking(data, {
      onSuccess: () => {
        reset();
        onClose?.();
      },
    });
  }
  
  return (
    <form onSubmit={handleSubmit(onSubmit)}>
      <FormRow label="Guest Name" error={errors?.guestName?.message}>
        <Input
          id="guestName"
          disabled={isCreating}
          {...register('guestName', {
            required: 'Guest name is required',
          })}
        />
      </FormRow>
      
      <FormRow label="Guest Email" error={errors?.guestEmail?.message}>
        <Input
          type="email"
          id="guestEmail"
          disabled={isCreating}
          {...register('guestEmail', {
            required: 'Email is required',
            pattern: {
              value: /\S+@\S+\.\S+/,
              message: 'Please provide a valid email',
            },
          })}
        />
      </FormRow>
      
      <FormRow label="Cabin" error={errors?.cabinId?.message}>
        <Select
          id="cabinId"
          disabled={isCreating}
          {...register('cabinId', {
            required: 'Please select a cabin',
          })}
        >
          <option value="">Select cabin...</option>
          <option value="1">Cabin 001</option>
          <option value="2">Cabin 002</option>
        </Select>
      </FormRow>
      
      <FormRow label="Start Date" error={errors?.startDate?.message}>
        <Input
          type="date"
          id="startDate"
          disabled={isCreating}
          {...register('startDate', {
            required: 'Start date is required',
          })}
        />
      </FormRow>
      
      <FormRow label="End Date" error={errors?.endDate?.message}>
        <Input
          type="date"
          id="endDate"
          disabled={isCreating}
          {...register('endDate', {
            required: 'End date is required',
          })}
        />
      </FormRow>
      
      <FormRow>
        <Button
          type="reset"
          variant="secondary"
          onClick={onClose}
          disabled={isCreating}
        >
          Cancel
        </Button>
        
        <Button type="submit" disabled={isCreating}>
          {isCreating ? 'Creating...' : 'Create Booking'}
        </Button>
      </FormRow>
    </form>
  );
}

/**
 * PATTERN :
 * - React Hook Form pour gestion du form
 * - Validation inline avec register
 * - Error messages automatiques
 * - Custom hook pour mutation
 * - Disable pendant loading
 * - Reset après succès
 */
```

### 2. Reusable Form Components

```javascript
// shared/components/form/FormRow.jsx
export function FormRow({ label, error, children }) {
  return (
    <div className="form-row">
      {label && <label htmlFor={children.props.id}>{label}</label>}
      {children}
      {error && <span className="error">{error}</span>}
    </div>
  );
}

// shared/components/ui/Input.jsx
import { forwardRef } from 'react';

export const Input = forwardRef(function Input(
  { type = 'text', className = '', ...props },
  ref
) {
  return (
    <input
      ref={ref}
      type={type}
      className={`input ${className}`}
      {...props}
    />
  );
});

// shared/components/ui/Select.jsx
import { forwardRef } from 'react';

export const Select = forwardRef(function Select(
  { children, className = '', ...props },
  ref
) {
  return (
    <select
      ref={ref}
      className={`select ${className}`}
      {...props}
    >
      {children}
    </select>
  );
});

/**
 * ⚠️ IMPORTANT :
 * - forwardRef pour React Hook Form compatibility
 * - Styled-components ou Tailwind pour styling
 * - Props spreading pour flexibility
 */
```

---

## 🧪 Testing Architecture

### 1. Testing Strategy

```javascript
/**
 * TESTING PYRAMID
 * 
 * 1. Unit Tests (70%)
 *    - Utils functions
 *    - Custom hooks
 *    - Pure components
 * 
 * 2. Integration Tests (20%)
 *    - Feature workflows
 *    - API integration
 *    - User interactions
 * 
 * 3. E2E Tests (10%)
 *    - Critical user journeys
 *    - Happy paths
 */

const testingStructure = `
src/
├── features/
│   └── bookings/
│       ├── components/
│       │   ├── BookingsList.jsx
│       │   └── BookingsList.test.jsx
│       ├── hooks/
│       │   ├── useBookings.js
│       │   └── useBookings.test.js
│       └── services/
│           ├── bookingsService.js
│           └── bookingsService.test.js
│
└── shared/
    ├── utils/
    │   ├── formatters.js
    │   └── formatters.test.js
    └── hooks/
        ├── useLocalStorage.js
        └── useLocalStorage.test.js
`;
```

### 2. Unit Tests Examples

```javascript
// shared/utils/formatters.test.js
import { describe, it, expect } from 'vitest';
import { formatCurrency, formatDate } from './formatters';

describe('formatCurrency', () => {
  it('formats number as USD currency', () => {
    expect(formatCurrency(1234.56)).toBe('$1,234.56');
  });
  
  it('handles zero', () => {
    expect(formatCurrency(0)).toBe('$0.00');
  });
  
  it('handles negative numbers', () => {
    expect(formatCurrency(-100)).toBe('-$100.00');
  });
});

describe('formatDate', () => {
  it('formats date correctly', () => {
    const date = new Date('2024-01-15');
    expect(formatDate(date)).toBe('Jan 15, 2024');
  });
});

// shared/hooks/useLocalStorage.test.js
import { renderHook, act } from '@testing-library/react';
import { describe, it, expect, beforeEach } from 'vitest';
import { useLocalStorage } from './useLocalStorage';

describe('useLocalStorage', () => {
  beforeEach(() => {
    localStorage.clear();
  });
  
  it('returns initial value when localStorage is empty', () => {
    const { result } = renderHook(() => useLocalStorage('key', 'initial'));
    expect(result.current[0]).toBe('initial');
  });
  
  it('updates localStorage when value changes', () => {
    const { result } = renderHook(() => useLocalStorage('key', 'initial'));
    
    act(() => {
      result.current[1]('updated');
    });
    
    expect(result.current[0]).toBe('updated');
    expect(localStorage.getItem('key')).toBe(JSON.stringify('updated'));
  });
});
```

### 3. Component Tests

```javascript
// features/bookings/components/BookingCard.test.jsx
import { render, screen, fireEvent } from '@testing-library/react';
import { describe, it, expect, vi } from 'vitest';
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
import { BookingCard } from './BookingCard';

// Mock useDeleteBooking hook
vi.mock('../hooks/useDeleteBooking', () => ({
  useDeleteBooking: () => ({
    deleteBooking: vi.fn(),
    isDeleting: false,
  }),
}));

const mockBooking = {
  id: 1,
  guests: { fullName: 'John Doe' },
  cabins: { name: 'Cabin 001' },
  startDate: '2024-01-15',
  status: 'confirmed',
};

function renderWithProviders(component) {
  const queryClient = new QueryClient({
    defaultOptions: {
      queries: { retry: false },
    },
  });
  
  return render(
    <QueryClientProvider client={queryClient}>
      {component}
    </QueryClientProvider>
  );
}

describe('BookingCard', () => {
  it('renders booking information', () => {
    renderWithProviders(<BookingCard booking={mockBooking} />);
    
    expect(screen.getByText('John Doe')).toBeInTheDocument();
    expect(screen.getByText('Cabin 001')).toBeInTheDocument();
    expect(screen.getByText('confirmed')).toBeInTheDocument();
  });
  
  it('has a delete button', () => {
    renderWithProviders(<BookingCard booking={mockBooking} />);
    
    const deleteButton = screen.getByRole('button', { name: /delete/i });
    expect(deleteButton).toBeInTheDocument();
  });
});
```

### 4. Integration Tests

```javascript
// features/bookings/BookingsList.integration.test.jsx
import { render, screen, waitFor } from '@testing-library/react';
import { describe, it, expect, vi } from 'vitest';
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
import { BookingsList } from './components/BookingsList';
import * as bookingsService from './services/bookingsService';

// Mock service
vi.mock('./services/bookingsService');

const mockBookings = [
  {
    id: 1,
    guests: { fullName: 'John Doe' },
    cabins: { name: 'Cabin 001' },
    startDate: '2024-01-15',
    status: 'confirmed',
  },
  {
    id: 2,
    guests: { fullName: 'Jane Smith' },
    cabins: { name: 'Cabin 002' },
    startDate: '2024-01-20',
    status: 'pending',
  },
];

describe('BookingsList Integration', () => {
  it('fetches and displays bookings', async () => {
    // Mock API response
    bookingsService.getBookings.mockResolvedValue(mockBookings);
    
    const queryClient = new QueryClient({
      defaultOptions: {
        queries: { retry: false },
      },
    });
    
    render(
      <QueryClientProvider client={queryClient}>
        <BookingsList />
      </QueryClientProvider>
    );
    
    // Loading state
    expect(screen.getByText(/loading/i)).toBeInTheDocument();
    
    // Wait for data to load
    await waitFor(() => {
      expect(screen.getByText('John Doe')).toBeInTheDocument();
      expect(screen.getByText('Jane Smith')).toBeInTheDocument();
    });
  });
  
  it('displays error when fetch fails', async () => {
    bookingsService.getBookings.mockRejectedValue(
      new Error('Failed to fetch')
    );
    
    const queryClient = new QueryClient({
      defaultOptions: {
        queries: { retry: false },
      },
    });
    
    render(
      <QueryClientProvider client={queryClient}>
        <BookingsList />
      </QueryClientProvider>
    );
    
    await waitFor(() => {
      expect(screen.getByText(/error/i)).toBeInTheDocument();
    });
  });
});
```

---

## 🎨 Styling Architecture

### 1. CSS Modules Pattern

```javascript
// features/bookings/components/BookingCard.module.css
.card {
  display: grid;
  grid-template-columns: 1fr 1fr 1fr auto;
  gap: 1rem;
  padding: 1rem;
  border: 1px solid var(--color-grey-200);
  border-radius: 8px;
}

.card:hover {
  background-color: var(--color-grey-50);
}

.guestName {
  font-weight: 600;
  color: var(--color-grey-900);
}

.status {
  padding: 0.25rem 0.75rem;
  border-radius: 999px;
  text-transform: uppercase;
  font-size: 0.875rem;
  font-weight: 600;
}

.statusConfirmed {
  background-color: var(--color-green-100);
  color: var(--color-green-700);
}

.statusPending {
  background-color: var(--color-yellow-100);
  color: var(--color-yellow-700);
}

// features/bookings/components/BookingCard.jsx
import styles from './BookingCard.module.css';

export function BookingCard({ booking }) {
  return (
    <div className={styles.card}>
      <span className={styles.guestName}>{booking.guests.fullName}</span>
      <span>{booking.cabins.name}</span>
      <span>{formatDate(booking.startDate)}</span>
      <span className={`${styles.status} ${styles[`status${booking.status}`]}`}>
        {booking.status}
      </span>
    </div>
  );
}

/**
 * ✅ AVANTAGES CSS Modules :
 * - Scoped styles (pas de conflits)
 * - Type-safe avec TypeScript
 * - Code splitting automatique
 * - Facile à maintenir
 */
```

### 2. Tailwind CSS Pattern

```javascript
// tailwind.config.js
module.exports = {
  content: ['./index.html', './src/**/*.{js,jsx}'],
  theme: {
    extend: {
      colors: {
        brand: {
          50: '#f0f9ff',
          100: '#e0f2fe',
          500: '#0ea5e9',
          600: '#0284c7',
          700: '#0369a1',
        },
      },
    },
  },
  plugins: [],
};

// features/bookings/components/BookingCard.jsx
export function BookingCard({ booking }) {
  const statusStyles = {
    confirmed: 'bg-green-100 text-green-700',
    pending: 'bg-yellow-100 text-yellow-700',
    cancelled: 'bg-red-100 text-red-700',
  };
  
  return (
    <div className="grid grid-cols-4 gap-4 p-4 border border-gray-200 rounded-lg hover:bg-gray-50">
      <span className="font-semibold text-gray-900">
        {booking.guests.fullName}
      </span>
      <span className="text-gray-600">{booking.cabins.name}</span>
      <span className="text-gray-600">{formatDate(booking.startDate)}</span>
      <span className={`px-3 py-1 rounded-full text-sm font-semibold uppercase ${statusStyles[booking.status]}`}>
        {booking.status}
      </span>
    </div>
  );
}

/**
 * ✅ AVANTAGES Tailwind :
 * - Utility-first (pas besoin d'inventer class names)
 * - Consistency (design system built-in)
 * - Performance (purge unused CSS)
 * - Responsive facile
 */
```

### 3. Styled Components Pattern

```javascript
// features/bookings/components/BookingCard.styled.js
import styled from 'styled-components';

export const Card = styled.div`
  display: grid;
  grid-template-columns: 1fr 1fr 1fr auto;
  gap: 1rem;
  padding: 1rem;
  border: 1px solid var(--color-grey-200);
  border-radius: 8px;
  
  &:hover {
    background-color: var(--color-grey-50);
  }
`;

export const GuestName = styled.span`
  font-weight: 600;
  color: var(--color-grey-900);
`;

export const Status = styled.span`
  padding: 0.25rem 0.75rem;
  border-radius: 999px;
  text-transform: uppercase;
  font-size: 0.875rem;
  font-weight: 600;
  
  ${props => {
    switch (props.$status) {
      case 'confirmed':
        return `
          background-color: var(--color-green-100);
          color: var(--color-green-700);
        `;
      case 'pending':
        return `
          background-color: var(--color-yellow-100);
          color: var(--color-yellow-700);
        `;
      default:
        return '';
    }
  }}
`;

// features/bookings/components/BookingCard.jsx
import * as S from './BookingCard.styled';

export function BookingCard({ booking }) {
  return (
    <S.Card>
      <S.GuestName>{booking.guests.fullName}</S.GuestName>
      <span>{booking.cabins.name}</span>
      <span>{formatDate(booking.startDate)}</span>
      <S.Status $status={booking.status}>{booking.status}</S.Status>
    </S.Card>
  );
}

/**
 * ✅ AVANTAGES Styled Components :
 * - CSS-in-JS (colocation)
 * - Dynamic styling facile
 * - Theming built-in
 * - No class name conflicts
 */
```

---

## 🚀 Performance Optimization Patterns

### 1. Code Splitting Strategy

```javascript
// routes/index.jsx - Lazy load pages
import { lazy, Suspense } from 'react';
import { Routes, Route } from 'react-router-dom';
import { PageSpinner } from '@/shared/components/ui';

// ✅ Lazy load par route
const Dashboard = lazy(() => import('@/pages/Dashboard'));
const Bookings = lazy(() => import('@/pages/Bookings'));
const Cabins = lazy(() => import('@/pages/Cabins'));

export function AppRoutes() {
  return (
    <Suspense fallback={<PageSpinner />}>
      <Routes>
        <Route path="dashboard" element={<Dashboard />} />
        <Route path="bookings" element={<Bookings />} />
        <Route path="cabins" element={<Cabins />} />
      </Routes>
    </Suspense>
  );
}

// ✅ Lazy load heavy components
const HeavyChart = lazy(() => import('./HeavyChart'));

function Dashboard() {
  return (
    <div>
      <h1>Dashboard</h1>
      <Suspense fallback={<ChartSkeleton />}>
        <HeavyChart />
      </Suspense>
    </div>
  );
}

/**
 * STRATÉGIE :
 * - Lazy load toutes les pages
 * - Lazy load heavy components (charts, editors)
 * - Lazy load modals/dialogs
 * - Suspense avec fallback approprié
 */
```

### 2. Memoization Strategy

```javascript
// features/bookings/components/BookingsList.jsx
import { memo } from 'react';
import { BookingCard } from './BookingCard';

export function BookingsList({ bookings, filters }) {
  // Filtrer côté client si nécessaire
  const filteredBookings = useMemo(() => {
    if (!filters.status) return bookings;
    return bookings.filter(b => b.status === filters.status);
  }, [bookings, filters.status]);
  
  // Trier côté client
  const sortedBookings = useMemo(() => {
    return [...filteredBookings].sort((a, b) => {
      if (filters.sortBy === 'date') {
        return new Date(b.startDate) - new Date(a.startDate);
      }
      return 0;
    });
  }, [filteredBookings, filters.sortBy]);
  
  return (
    <div>
      {sortedBookings.map(booking => (
        <MemoizedBookingCard key={booking.id} booking={booking} />
      ))}
    </div>
  );
}

// Memoize card pour éviter re-renders inutiles
const MemoizedBookingCard = memo(BookingCard);

/**
 * RÈGLES :
 * - useMemo pour calculs coûteux (filter, sort)
 * - memo pour components dans listes
 * - useCallback pour functions passées en props
 */
```

### 3. Virtual Scrolling pour Grandes Listes

```javascript
// features/bookings/components/VirtualBookingsList.jsx
import { FixedSizeList } from 'react-window';
import { BookingCard } from './BookingCard';

export function VirtualBookingsList({ bookings }) {
  const Row = ({ index, style }) => {
    const booking = bookings[index];
    return (
      <div style={style}>
        <BookingCard booking={booking} />
      </div>
    );
  };
  
  return (
    <FixedSizeList
      height={600}
      itemCount={bookings.length}
      itemSize={80}
      width="100%"
    >
      {Row}
    </FixedSizeList>
  );
}

/**
 * QUAND UTILISER :
 * - Listes > 100 items
 * - Scroll lag visible
 * - Memory issues
 */
```

---

## 📊 Data Flow Visualization

### Architecture Complète : Wild Oasis Example

```javascript
/**
 * DATA FLOW ARCHITECTURE - WILD OASIS
 * 
 * User Action → Component → Custom Hook → Service → API → Database
 *              ↓                          ↓
 *           UI Update  ←  React Query Cache  ←  Response
 */

// EXEMPLE CONCRET : Create Booking Flow

// 1. USER ACTION (Component)
// features/bookings/components/CreateBookingForm.jsx
function CreateBookingForm() {
  const { createBooking, isCreating } = useCreateBooking(); // Custom Hook
  
  function onSubmit(data) {
    createBooking(data); // Trigger mutation
  }
  
  return <form onSubmit={handleSubmit(onSubmit)}>...</form>;
}

// 2. CUSTOM HOOK (React Query Logic)
// features/bookings/hooks/useCreateBooking.js
export function useCreateBooking() {
  const queryClient = useQueryClient();
  
  return useMutation({
    mutationFn: createBooking, // Service function
    onSuccess: () => {
      queryClient.invalidateQueries(['bookings']); // Refresh cache
      toast.success('Booking created');
    },
  });
}

// 3. SERVICE (API Call)
// features/bookings/services/bookingsService.js
export async function createBooking(newBooking) {
  const { data, error } = await supabase
    .from('bookings')
    .insert([newBooking])
    .select()
    .single();
  
  if (error) throw new Error(error.message);
  return data; // Return to React Query
}

// 4. DATABASE (Supabase/Backend)
// Data persisted → Response → React Query → Cache → UI Update

/**
 * FLOW COMPLET :
 * 
 * User clicks Submit
 * → CreateBookingForm.onSubmit()
 * → useCreateBooking.createBooking()
 * → bookingsService.createBooking()
 * → Supabase API
 * → Database INSERT
 * → Response back
 * → React Query cache updated
 * → UI re-renders with new data
 * → Toast notification
 * 
 * AVANTAGES :
 * - Séparation claire des responsabilités
 * - Facile à tester chaque layer
 * - Facile à modifier (changer Supabase → autre API)
 * - Cache automatique (React Query)
 * - Error handling centralisé
 */
```

---

## 🎤 Questions d'Entretien - Architecture

### Q1: "Comment structures-tu une grande application React ?"

**Réponse structurée :**

```
J'utilise une FEATURE-BASED ARCHITECTURE :

**STRUCTURE :**
src/
├── features/          → Organisation par fonctionnalité business
│   ├── authentication/
│   ├── bookings/
│   └── cabins/
├── shared/           → Code réutilisable
│   ├── components/
│   ├── hooks/
│   └── utils/
└── pages/            → Routes

**FEATURES :** Chaque feature contient :
- components/ : UI spécifique à la feature
- hooks/ : Custom hooks (useBookings, useCreateBooking)
- services/ : API calls (bookingsService.js)
- index.js : Public API (exports)

**AVANTAGES :**
- Scalable (ajouter features facilement)
- Maintenable (tout pour une feature au même endroit)
- Testable (isolation par feature)
- Team-friendly (travailler sur features séparées)

**RÈGLES :**
1. Feature folder = self-contained
2. Imports entre features via index.js seulement
3. shared/ pour code réutilisable
4. Services layer pour API calls
5. Custom hooks pour React Query logic
```

### Q2: "Comment gères-tu le state dans une application complexe ?"

**Réponse avec stratégie :**

```javascript
/**
 * STATE MANAGEMENT STRATEGY - 3 TYPES
 */

// 1. SERVER STATE (70% du state) → React Query
const serverState = {
  what: 'Data depuis API/Database',
  examples: ['Bookings, Cabins, Users', 'Settings', 'Analytics'],
  tool: 'React Query',
  pattern: 'Custom hooks (useBookings, useCabins)',
  
  code: `
    export function useBookings() {
      return useQuery({
        queryKey: ['bookings'],
        queryFn: fetchBookings,
      });
    }
  `,
};

// 2. GLOBAL UI STATE (20%) → Context API
const globalUIState = {
  what: 'UI state partagé entre components',
  examples: ['Dark mode', 'Sidebar open/closed', 'Current user'],
  tool: 'Context API + Custom Hook',
  
  code: `
    const DarkModeContext = createContext();
    
    export function useDarkMode() {
      return useContext(DarkModeContext);
    }
  `,
};

// 3. LOCAL STATE (10%) → useState
const localState = {
  what: 'State spécifique à un component',
  examples: ['Form inputs', 'Modal open', 'Tabs active'],
  tool: 'useState / useReducer',
  
  code: `
    const [isOpen, setIsOpen] = useState(false);
  `,
};

/**
 * POURQUOI CETTE STRATÉGIE :
 * - React Query excelle pour server state (cache, sync, etc.)
 * - Context pour UI state global (éviter prop drilling)
 * - useState pour local UI (simple et performant)
 * - PAS de Redux nécessaire (over-engineering)
 */
```

### Q3: "Explique ton approche pour les custom hooks"

**Réponse avec exemples :**

```javascript
/**
 * CUSTOM HOOKS PATTERN
 */

// 1. UN HOOK PAR OPÉRATION
const hooksPattern = {
  // Queries (GET)
  useBookings: 'Fetch liste',
  useBooking: 'Fetch single',
  
  // Mutations (POST/PUT/DELETE)
  useCreateBooking: 'Create',
  useUpdateBooking: 'Update',
  useDeleteBooking: 'Delete',
};

// 2. STRUCTURE COHÉRENTE
export function useCreateBooking() {
  const queryClient = useQueryClient();
  
  const { mutate: createBooking, isLoading: isCreating } = useMutation({
    mutationFn: bookingsService.createBooking, // Service layer
    onSuccess: () => {
      queryClient.invalidateQueries(['bookings']); // Cache invalidation
      toast.success('Created');
    },
    onError: (err) => toast.error(err.message),
  });
  
  return { createBooking, isCreating };
}

// 3. USAGE SIMPLE DANS COMPONENTS
function Component() {
  const { createBooking, isCreating } = useCreateBooking();
  
  return (
    <button onClick={() => createBooking(data)} disabled={isCreating}>
      Create
    </button>
  );
}

/**
 * AVANTAGES :
 * - Logique réutilisable
 * - Components simples (juste UI)
 * - Facile à tester
 * - Naming cohérent
 * - Error handling centralisé
 */
```

### Q4: "Comment optimises-tu les performances d'une grande application ?"

**Réponse complète :**

```javascript
/**
 * PERFORMANCE OPTIMIZATION STRATEGY
 */

const optimizationStrategy = {
  // 1. CODE SPLITTING
  codeSplitting: {
    what: 'Lazy load routes et heavy components',
    code: `
      const Dashboard = lazy(() => import('./Dashboard'));
      const HeavyChart = lazy(() => import('./HeavyChart'));
    `,
    impact: 'Initial bundle 70% plus petit',
  },
  
  // 2. MEMOIZATION
  memoization: {
    what: 'Éviter re-renders et re-calculs inutiles',
    code: `
      // Components dans listes
      const MemoCard = memo(Card);
      
      // Calculs coûteux
      const filtered = useMemo(() => {
        return items.filter(i => i.status === 'active');
      }, [items]);
      
      // Functions passées en props
      const handleClick = useCallback(() => {}, []);
    `,
    impact: '50% moins de re-renders',
  },
  
  // 3. VIRTUALIZATION
  virtualization: {
    what: 'Render seulement items visibles',
    code: `
      import { FixedSizeList } from 'react-window';
      
      <FixedSizeList
        height={600}
        itemCount={10000}
        itemSize={80}
      >
        {Row}
      </FixedSizeList>
    `,
    when: 'Listes > 100 items',
    impact: '90% moins de DOM nodes',
  },
  
  // 4. IMAGE OPTIMIZATION
  imageOptimization: {
    what: 'Optimize images automatiquement',
    code: `
      <Image
        src="/photo.jpg"
        width={800}
        height={600}
        loading="lazy"
      />
    `,
    impact: 'Images 70% plus petites (WebP)',
  },
  
  // 5. REACT QUERY OPTIMIZATION
  reactQuery: {
    what: 'Cache intelligent et staleTime adapté',
    code: `
      useQuery({
        queryKey: ['cabins'],
        queryFn: fetchCabins,
        staleTime: 5 * 60 * 1000, // 5 min
      });
    `,
    impact: 'Moins de requests API',
  },
};

/**
 * MÉTHODOLOGIE :
 * 1. Profile avec React DevTools Profiler
 * 2. Identifier bottlenecks (render > 16ms)
 * 3. Appliquer optimizations appropriées
 * 4. Re-mesurer impact
 * 
 * RÈGLE : Optimize seulement si problème mesuré
 */
```

### Q5: "Comment organises-tu les tests dans ton architecture ?"

**Réponse structurée avec exemples :**

```javascript
/**
 * TESTING STRATEGY DANS ARCHITECTURE
 */

// 1. TESTING PYRAMID
const testingPyramid = {
  // 70% - UNIT TESTS
  unit: {
    what: 'Tests de fonctions pures, utils, hooks isolés',
    where: 'À côté du fichier testé (*.test.js)',
    
    examples: `
      shared/utils/
      ├── formatters.js
      └── formatters.test.js      ← Unit test
      
      features/bookings/hooks/
      ├── useBookings.js
      └── useBookings.test.js     ← Hook test
    `,
    
    code: `
      // formatters.test.js
      describe('formatCurrency', () => {
        it('formats number as currency', () => {
          expect(formatCurrency(1234.56)).toBe('$1,234.56');
        });
      });
      
      // useBookings.test.js
      it('fetches bookings', async () => {
        const { result } = renderHook(() => useBookings());
        await waitFor(() => {
          expect(result.current.bookings).toBeDefined();
        });
      });
    `,
  },
  
  // 20% - INTEGRATION TESTS
  integration: {
    what: 'Tests de features complètes (user workflows)',
    where: 'Dans dossier feature (*.integration.test.js)',
    
    examples: `
      features/bookings/
      ├── components/
      ├── hooks/
      └── BookingFlow.integration.test.js  ← Integration test
    `,
    
    code: `
      it('creates a booking successfully', async () => {
        render(<CreateBookingForm />);
        
        // Fill form
        fireEvent.change(screen.getByLabelText(/guest name/i), {
          target: { value: 'John Doe' },
        });
        
        // Submit
        fireEvent.click(screen.getByRole('button', { name: /create/i }));
        
        // Assert
        await waitFor(() => {
          expect(screen.getByText(/created successfully/i)).toBeInTheDocument();
        });
      });
    `,
  },
  
  // 10% - E2E TESTS
  e2e: {
    what: 'Tests de journeys utilisateur critiques',
    where: 'Dossier e2e/ séparé',
    tool: 'Playwright ou Cypress',
    
    examples: `
      e2e/
      ├── auth.spec.js
      ├── booking-flow.spec.js
      └── dashboard.spec.js
    `,
    
    code: `
      // Playwright
      test('complete booking flow', async ({ page }) => {
        await page.goto('/');
        await page.click('text=Login');
        await page.fill('[name="email"]', 'user@example.com');
        await page.fill('[name="password"]', 'password');
        await page.click('button[type="submit"]');
        
        await page.click('text=Bookings');
        await page.click('text=Create Booking');
        // ... complete flow
        
        await expect(page.locator('text=Booking created')).toBeVisible();
      });
    `,
  },
};

// 2. TESTING UTILITIES
const testingUtils = {
  // Custom render function
  customRender: `
    // shared/test-utils/render.jsx
    import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
    import { BrowserRouter } from 'react-router-dom';
    
    export function renderWithProviders(ui, options = {}) {
      const queryClient = new QueryClient({
        defaultOptions: {
          queries: { retry: false },
          mutations: { retry: false },
        },
      });
      
      function Wrapper({ children }) {
        return (
          <QueryClientProvider client={queryClient}>
            <BrowserRouter>
              {children}
            </BrowserRouter>
          </QueryClientProvider>
        );
      }
      
      return render(ui, { wrapper: Wrapper, ...options });
    }
  `,
  
  // Mock server (MSW)
  mockServer: `
    // shared/test-utils/server.js
    import { setupServer } from 'msw/node';
    import { rest } from 'msw';
    
    const handlers = [
      rest.get('/api/bookings', (req, res, ctx) => {
        return res(
          ctx.json([
            { id: 1, guestName: 'John Doe' },
            { id: 2, guestName: 'Jane Smith' },
          ])
        );
      }),
    ];
    
    export const server = setupServer(...handlers);
  `,
};

// 3. STRUCTURE COMPLÈTE
const completeStructure = `
src/
├── features/
│   └── bookings/
│       ├── components/
│       │   ├── BookingsList.jsx
│       │   └── BookingsList.test.jsx          ← Component test
│       ├── hooks/
│       │   ├── useBookings.js
│       │   └── useBookings.test.js            ← Hook test
│       ├── services/
│       │   ├── bookingsService.js
│       │   └── bookingsService.test.js        ← Service test
│       └── BookingFlow.integration.test.js    ← Integration test
│
├── shared/
│   ├── test-utils/
│   │   ├── render.jsx                         ← Custom render
│   │   └── server.js                          ← MSW server
│   └── utils/
│       ├── formatters.js
│       └── formatters.test.js                 ← Unit test
│
└── e2e/
    ├── auth.spec.js                           ← E2E test
    └── booking-flow.spec.js                   ← E2E test
`;

/**
 * BEST PRACTICES :
 * 
 * 1. Co-location : Tests à côté du code
 * 2. Naming : *.test.js (unit), *.integration.test.js, *.spec.js (e2e)
 * 3. Setup : Test utilities réutilisables
 * 4. Mocking : MSW pour API, vi.mock pour modules
 * 5. Coverage : Viser 80%+ sur business logic
 * 
 * CI/CD :
 * - Unit + Integration : À chaque commit
 * - E2E : Avant merge/deploy
 */
```

---

## 💡 Best Practices - Architecture Complète

### Checklist Globale

```javascript
const architectureBestPractices = {
  // 1. STRUCTURE
  structure: {
    always: [
      'Feature-based organization',
      'Co-locate related files',
      'index.js pour public API',
      'Shared/ pour code réutilisable',
      'Clear naming conventions',
    ],
    never: [
      'Flat structure avec 100+ fichiers',
      'Mélanger features et shared',
      'Exports directs de fichiers internes',
    ],
  },
  
  // 2. SEPARATION OF CONCERNS
  separation: {
    layers: [
      'Pages → orchestration',
      'Features → business logic',
      'Shared → utilities réutilisables',
      'Services → API calls',
      'Hooks → React Query logic',
    ],
    rule: 'Chaque layer a une responsabilité claire',
  },
  
  // 3. STATE MANAGEMENT
  state: {
    serverState: 'React Query (70%)',
    globalUI: 'Context API (20%)',
    localUI: 'useState (10%)',
    never: 'Redux (over-engineering pour la plupart des apps)',
  },
  
  // 4. COMPONENT ARCHITECTURE
  components: {
    always: [
      'Small, focused components',
      'Props drilling max 2 niveaux',
      'Compound components pour UI complexe',
      'Presentational vs Container pattern',
    ],
    never: [
      'Composants 500+ lignes',
      'Logique mélangée avec UI',
      'Props drilling > 2 niveaux (utiliser Context)',
    ],
  },
  
  // 5. PERFORMANCE
  performance: {
    always: [
      'Code splitting par route',
      'Lazy load heavy components',
      'Memoization stratégique',
      'Virtual scrolling pour grandes listes',
    ],
    when: 'Après avoir mesuré (React DevTools Profiler)',
  },
  
  // 6. TESTING
  testing: {
    pyramid: '70% unit, 20% integration, 10% e2e',
    coverage: '80%+ business logic',
    coLocation: 'Tests à côté du code',
  },
  
  // 7. CODE QUALITY
  quality: {
    always: [
      'ESLint configuré',
      'Prettier pour formatting',
      'TypeScript (optionnel mais recommandé)',
      'Husky pre-commit hooks',
    ],
  },
};
```

---

## 🔗 Exemples Complets de Projets

### 1. Wild Oasis - Architecture Complète

```javascript
/**
 * WILD OASIS - FULL ARCHITECTURE
 */

const wildOasisArchitecture = `
wild-oasis/
├── src/
│   ├── features/
│   │   ├── authentication/
│   │   │   ├── components/
│   │   │   │   ├── LoginForm.jsx
│   │   │   │   ├── SignupForm.jsx
│   │   │   │   └── ProtectedRoute.jsx
│   │   │   ├── hooks/
│   │   │   │   ├── useLogin.js
│   │   │   │   ├── useSignup.js
│   │   │   │   ├── useLogout.js
│   │   │   │   └── useUser.js
│   │   │   ├── services/
│   │   │   │   └── authService.js
│   │   │   └── index.js
│   │   │
│   │   ├── bookings/
│   │   │   ├── components/
│   │   │   │   ├── BookingsList.jsx
│   │   │   │   ├── BookingCard.jsx
│   │   │   │   ├── BookingDetail.jsx
│   │   │   │   ├── BookingFilters.jsx
│   │   │   │   └── CreateBookingForm.jsx
│   │   │   ├── hooks/
│   │   │   │   ├── useBookings.js
│   │   │   │   ├── useBooking.js
│   │   │   │   ├── useCreateBooking.js
│   │   │   │   ├── useUpdateBooking.js
│   │   │   │   ├── useDeleteBooking.js
│   │   │   │   └── useCheckIn.js
│   │   │   ├── services/
│   │   │   │   └── bookingsService.js
│   │   │   ├── utils/
│   │   │   │   └── bookingHelpers.js
│   │   │   └── index.js
│   │   │
│   │   ├── cabins/
│   │   │   ├── components/
│   │   │   │   ├── CabinsList.jsx
│   │   │   │   ├── CabinCard.jsx
│   │   │   │   ├── CabinDetail.jsx
│   │   │   │   ├── CreateCabinForm.jsx
│   │   │   │   └── DuplicateCabin.jsx
│   │   │   ├── hooks/
│   │   │   │   ├── useCabins.js
│   │   │   │   ├── useCabin.js
│   │   │   │   ├── useCreateCabin.js
│   │   │   │   ├── useUpdateCabin.js
│   │   │   │   └── useDeleteCabin.js
│   │   │   ├── services/
│   │   │   │   └── cabinsService.js
│   │   │   └── index.js
│   │   │
│   │   ├── dashboard/
│   │   │   ├── components/
│   │   │   │   ├── DashboardLayout.jsx
│   │   │   │   ├── StatsCards.jsx
│   │   │   │   ├── SalesChart.jsx
│   │   │   │   ├── DurationChart.jsx
│   │   │   │   └── TodayActivity.jsx
│   │   │   ├── hooks/
│   │   │   │   ├── useRecentBookings.js
│   │   │   │   ├── useRecentStays.js
│   │   │   │   └── useDashboardStats.js
│   │   │   └── index.js
│   │   │
│   │   ├── guests/
│   │   │   ├── components/
│   │   │   ├── hooks/
│   │   │   ├── services/
│   │   │   └── index.js
│   │   │
│   │   └── settings/
│   │       ├── components/
│   │       │   ├── SettingsForm.jsx
│   │       │   └── UpdateSettingsForm.jsx
│   │       ├── hooks/
│   │       │   ├── useSettings.js
│   │       │   └── useUpdateSettings.js
│   │       ├── services/
│   │       │   └── settingsService.js
│   │       └── index.js
│   │
│   ├── shared/
│   │   ├── components/
│   │   │   ├── ui/
│   │   │   │   ├── Button.jsx
│   │   │   │   ├── Input.jsx
│   │   │   │   ├── Select.jsx
│   │   │   │   ├── Modal.jsx
│   │   │   │   ├── Spinner.jsx
│   │   │   │   ├── Table.jsx
│   │   │   │   ├── Empty.jsx
│   │   │   │   ├── Pagination.jsx
│   │   │   │   └── ConfirmDelete.jsx
│   │   │   ├── layout/
│   │   │   │   ├── AppLayout.jsx
│   │   │   │   ├── Header.jsx
│   │   │   │   ├── Sidebar.jsx
│   │   │   │   ├── Logo.jsx
│   │   │   │   └── MainNav.jsx
│   │   │   └── form/
│   │   │       ├── FormRow.jsx
│   │   │       ├── FormError.jsx
│   │   │       └── FileInput.jsx
│   │   │
│   │   ├── hooks/
│   │   │   ├── useLocalStorage.js
│   │   │   ├── useOutsideClick.js
│   │   │   └── useMoveBack.js
│   │   │
│   │   ├── utils/
│   │   │   ├── formatters.js
│   │   │   ├── helpers.js
│   │   │   └── constants.js
│   │   │
│   │   ├── contexts/
│   │   │   └── DarkModeContext.jsx
│   │   │
│   │   └── lib/
│   │       ├── supabase.js
│   │       └── queryClient.js
│   │
│   ├── pages/
│   │   ├── Dashboard.jsx
│   │   ├── Bookings.jsx
│   │   ├── BookingDetail.jsx
│   │   ├── Cabins.jsx
│   │   ├── Settings.jsx
│   │   ├── Users.jsx
│   │   ├── Account.jsx
│   │   ├── Login.jsx
│   │   └── PageNotFound.jsx
│   │
│   ├── routes/
│   │   └── index.jsx
│   │
│   ├── styles/
│   │   ├── GlobalStyles.js
│   │   └── index.css
│   │
│   ├── App.jsx
│   └── main.jsx
│
├── public/
├── .env.local
├── .eslintrc.json
├── .prettierrc
├── package.json
├── vite.config.js
└── README.md
`;

/**
 * CARACTÉRISTIQUES CLÉS :
 * 
 * 1. FEATURE-BASED
 *    - Chaque feature self-contained
 *    - Tout au même endroit (components, hooks, services)
 * 
 * 2. SERVICES LAYER
 *    - Toutes API calls dans services/
 *    - Facile à tester et modifier
 * 
 * 3. CUSTOM HOOKS
 *    - Un hook par opération (useBookings, useCreateBooking)
 *    - React Query logic isolée
 * 
 * 4. SHARED CODE
 *    - UI components réutilisables
 *    - Utilities, hooks, contexts partagés
 * 
 * 5. PAGES
 *    - Routes/pages séparées
 *    - Orchestrent features
 */
```

### 2. Fast React Pizza - Architecture Simplifiée

```javascript
const pizzaArchitecture = `
fast-react-pizza/
├── src/
│   ├── features/
│   │   ├── cart/
│   │   │   ├── Cart.jsx
│   │   │   ├── CartItem.jsx
│   │   │   ├── CartOverview.jsx
│   │   │   ├── cartSlice.js          → Redux slice
│   │   │   └── EmptyCart.jsx
│   │   │
│   │   ├── menu/
│   │   │   ├── Menu.jsx
│   │   │   ├── MenuItem.jsx
│   │   │   └── menuService.js        → API calls
│   │   │
│   │   ├── order/
│   │   │   ├── CreateOrder.jsx
│   │   │   ├── Order.jsx
│   │   │   ├── OrderItem.jsx
│   │   │   ├── SearchOrder.jsx
│   │   │   └── orderService.js
│   │   │
│   │   └── user/
│   │       ├── Username.jsx
│   │       ├── CreateUser.jsx
│   │       └── userSlice.js
│   │
│   ├── ui/
│   │   ├── AppLayout.jsx
│   │   ├── Button.jsx
│   │   ├── Header.jsx
│   │   ├── Home.jsx
│   │   ├── Loader.jsx
│   │   ├── Error.jsx
│   │   └── LinkButton.jsx
│   │
│   ├── utils/
│   │   └── helpers.js
│   │
│   ├── services/
│   │   └── apiGeocoding.js
│   │
│   ├── store.js                      → Redux store
│   ├── App.jsx
│   └── main.jsx
│
└── ...
`;

/**
 * DIFFÉRENCES AVEC WILD OASIS :
 * - Plus simple (moins de features)
 * - Redux au lieu de Context (state management global)
 * - Pas de authentication
 * - ui/ au lieu de shared/components/
 * 
 * SIMILITUDES :
 * - Feature-based structure
 * - Services layer pour API
 * - Components co-locatés avec feature
 */
```

---

## 🎯 Decision Matrix : Choisir la Bonne Architecture

```javascript
const architectureDecisionMatrix = {
  // SMALL APP (< 10 pages, 1-2 devs)
  small: {
    structure: 'Simple flat ou feature-light',
    state: 'Context API + useState',
    routing: 'React Router simple',
    styling: 'CSS Modules ou Tailwind',
    
    example: `
      src/
      ├── components/
      ├── pages/
      ├── hooks/
      ├── utils/
      └── App.jsx
    `,
  },
  
  // MEDIUM APP (10-30 pages, 2-5 devs)
  medium: {
    structure: 'Feature-based architecture',
    state: 'React Query + Context API',
    routing: 'React Router avec lazy loading',
    styling: 'Tailwind ou Styled Components',
    testing: 'Unit + Integration',
    
    example: `
      src/
      ├── features/
      ├── shared/
      ├── pages/
      └── App.jsx
    `,
    
    thisIsWildOasis: true,
  },
  
  // LARGE APP (30+ pages, 5+ devs)
  large: {
    structure: 'Feature-based + Domain-driven',
    state: 'React Query + Context + (Redux si nécessaire)',
    routing: 'React Router ou Next.js',
    styling: 'Design System + Styled Components',
    testing: 'Unit + Integration + E2E',
    monitoring: 'Sentry, LogRocket',
    
    example: `
      src/
      ├── features/
      ├── shared/
      ├── domains/        → Business domains
      ├── pages/
      ├── services/
      └── App.jsx
    `,
  },
  
  // ENTERPRISE (100+ pages, 10+ teams)
  enterprise: {
    structure: 'Micro-frontends ou Monorepo',
    state: 'Multiple strategies par team',
    routing: 'Next.js ou Module Federation',
    styling: 'Design System centralisé',
    testing: 'Full pyramid + Contract testing',
    monitoring: 'Full observability stack',
    
    considerNextJs: true,
    considerMonorepo: true,
  },
};

/**
 * WILD OASIS = MEDIUM APP
 * - Feature-based architecture ✅
 * - React Query pour server state ✅
 * - Context pour UI state ✅
 * - React Router avec lazy loading ✅
 * - Tailwind pour styling ✅
 * - Bonne base pour scaler ✅
 */
```

---

## 🎉 Résumé Final : Architecture Patterns

```javascript
const architectureSummary = {
  // PRINCIPES FONDAMENTAUX
  principles: [
    'Feature-based organization',
    'Separation of concerns (Pages → Features → Shared)',
    'Services layer pour API calls',
    'Custom hooks pour React Query logic',
    'Context pour UI state global',
    'Co-location (tests, styles avec code)',
  ],
  
  // STRUCTURE TYPE
  structure: `
    features/          → Business features
    shared/           → Code réutilisable
    pages/            → Routes
    App.jsx           → Root component
  `,
  
  // STATE MANAGEMENT
  state: {
    serverState: 'React Query (70%)',
    globalUI: 'Context API (20%)',
    localUI: 'useState (10%)',
  },
  
  // BEST PRACTICES
  bestPractices: [
    'Profile avant d\'optimiser',
    'Tests co-locatés avec code',
    'index.js pour public API',
    'Services layer pour API',
    'Custom hooks pour logique',
    'Compound components pour UI complexe',
  ],
  
  // QUAND UTILISER QUOI
  whenToUse: {
    context: 'UI state partagé (dark mode, user)',
    reactQuery: 'Server state (bookings, cabins)',
    redux: 'Rarement nécessaire (over-engineering)',
    memo: 'Components dans listes',
    useMemo: 'Calculs coûteux (> 5ms)',
    useCallback: 'Functions passées à memo children',
  },
};
```

---

**🎊 Félicitations ! Vous maîtrisez maintenant l'Architecture d'Applications React Réelles !**

**Cette architecture vous permet de :**
- ✅ Construire des apps scalables
- ✅ Travailler efficacement en équipe
- ✅ Maintenir du code propre et testable
- ✅ Ajouter des features facilement
- ✅ Optimiser les performances
- ✅ Passer des entretiens techniques avec confiance

**C'est l'architecture utilisée dans Wild Oasis et applicable à tous vos projets professionnels ! 🚀**