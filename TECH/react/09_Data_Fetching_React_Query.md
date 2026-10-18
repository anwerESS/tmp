## [**..**](./00_index.md)

# CHAPITRE 9 : Data Fetching & React Query - Version Approfondie

## 🎯 Pourquoi React Query ? Le Problème Fondamental

### Le Problème avec useEffect pour Data Fetching

```javascript
// ❌ Approche traditionnelle avec useEffect - BEAUCOUP de code !
function CabinsList() {
  const [cabins, setCabins] = useState([]);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);

  useEffect(() => {
    let isMounted = true; // Pour éviter les memory leaks
    
    async function fetchCabins() {
      setIsLoading(true);
      setError(null);
      
      try {
        const res = await fetch('/api/cabins');
        if (!res.ok) throw new Error('Failed to fetch');
        const data = await res.json();
        
        if (isMounted) setCabins(data);
      } catch (err) {
        if (isMounted) setError(err.message);
      } finally {
        if (isMounted) setIsLoading(false);
      }
    }
    
    fetchCabins();
    
    return () => { isMounted = false; }; // Cleanup
  }, []); // Dependencies ?

  if (isLoading) return <Spinner />;
  if (error) return <Error message={error} />;
  
  return <div>{cabins.map(cabin => <CabinCard cabin={cabin} />)}</div>;
}
```

**Problèmes de cette approche :**
1. ❌ **Code boilerplate énorme** (20+ lignes pour un simple fetch)
2. ❌ **Pas de caching** : Remonte le composant = refetch complet
3. ❌ **Pas de synchronisation** : Si 2 composants fetchent les mêmes données
4. ❌ **Race conditions** : Si plusieurs fetchs simultanés
5. ❌ **Pas de retry automatique** en cas d'erreur
6. ❌ **Pas de background updates**
7. ❌ **Gestion manuelle** de tous les edge cases

### La Solution : React Query

```javascript
// ✅ Avec React Query - Élégant et puissant !
function CabinsList() {
  const { data: cabins, isLoading, error } = useQuery({
    queryKey: ['cabins'],
    queryFn: async () => {
      const res = await fetch('/api/cabins');
      if (!res.ok) throw new Error('Failed to fetch');
      return res.json();
    },
  });

  if (isLoading) return <Spinner />;
  if (error) return <Error message={error.message} />;
  
  return <div>{cabins.map(cabin => <CabinCard cabin={cabin} />)}</div>;
}
```

**Avantages immédiats :**
- ✅ **90% moins de code**
- ✅ **Caching automatique** et intelligent
- ✅ **Synchronisation** entre composants
- ✅ **Background refetching**
- ✅ **Retry automatique**
- ✅ **DevTools** pour debugging

---

## 🏗️ Architecture et Concepts Fondamentaux

### Server State vs Client State

**React Query gère le SERVER STATE, pas le client state !**

```javascript
// ❌ MAUVAISE utilisation - Client State
const { data: isModalOpen } = useQuery(['modal'], () => false); // NON !

// ✅ Client State = useState
const [isModalOpen, setIsModalOpen] = useState(false);

// ✅ Server State = React Query
const { data: cabins } = useQuery(['cabins'], fetchCabins);
```

**Server State c'est :**
- Données venant d'une API/Database
- Partagées entre plusieurs utilisateurs
- Peuvent changer sans que vous le sachiez
- Nécessitent synchronisation

**Client State c'est :**
- UI locale (modals, toggles, form inputs)
- Appartient à un seul utilisateur
- Vous contrôlez les changements
- useState/useReducer/Context API

---

## 📦 Installation et Configuration Avancée

### Installation

```bash
npm install @tanstack/react-query @tanstack/react-query-devtools
```

### Configuration du QueryClient (Niveau Expert)

```javascript
// main.jsx ou App.jsx
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
import { ReactQueryDevtools } from '@tanstack/react-query-devtools';

const queryClient = new QueryClient({
  defaultOptions: {
    queries: {
      // TEMPS DE "FRAÎCHEUR" - Cruciale à comprendre !
      staleTime: 0, // Par défaut : data immédiatement "stale"
      
      // TEMPS EN CACHE après unmount
      cacheTime: 5 * 60 * 1000, // 5 minutes par défaut
      
      // REFETCH automatique
      refetchOnWindowFocus: true, // Refetch quand user revient sur tab
      refetchOnReconnect: true, // Refetch après reconnexion internet
      refetchOnMount: true, // Refetch au mount si data stale
      
      // RETRY en cas d'erreur
      retry: 3, // 3 tentatives
      retryDelay: (attemptIndex) => Math.min(1000 * 2 ** attemptIndex, 30000),
      
      // ERROR HANDLING global
      onError: (error) => {
        console.error('Query error:', error);
      },
    },
    mutations: {
      retry: 1,
      onError: (error) => {
        console.error('Mutation error:', error);
      },
    },
  },
});

function App() {
  return (
    <QueryClientProvider client={queryClient}>
      <YourApp />
      {/* DevTools - visible seulement en développement */}
      <ReactQueryDevtools 
        initialIsOpen={false} 
        position="bottom-right"
      />
    </QueryClientProvider>
  );
}
```

---

## 🔍 useQuery - Deep Dive Complet

### 1. Anatomie Complète de useQuery

```javascript
const {
  // DATA
  data,              // Les données retournées par queryFn
  error,             // L'erreur si échec
  
  // STATUS FLAGS
  isLoading,         // Premier fetch en cours (pas de cached data)
  isFetching,        // N'importe quel fetch en cours (même en background)
  isError,           // Une erreur s'est produite
  isSuccess,         // Fetch réussi
  isIdle,            // Query désactivée (enabled: false)
  
  // STATUS STRING
  status,            // 'idle' | 'loading' | 'error' | 'success'
  fetchStatus,       // 'fetching' | 'paused' | 'idle'
  
  // ACTIONS
  refetch,           // Fonction pour refetch manuellement
  remove,            // Supprimer la query du cache
  
  // METADATA
  dataUpdatedAt,     // Timestamp de la dernière update
  errorUpdatedAt,    // Timestamp de la dernière erreur
  failureCount,      // Nombre d'échecs de retry
  failureReason,     // Raison du dernier échec
} = useQuery({
  queryKey: ['cabins'],
  queryFn: fetchCabins,
  // ... options
});
```

### 2. Query Keys - La Clé du Système de Cache

**Query Key = Identifiant unique pour chaque query**

```javascript
// ✅ SIMPLE KEY
useQuery({
  queryKey: ['cabins'],
  queryFn: fetchAllCabins,
});

// ✅ KEY AVEC PARAMÈTRES (Auto-refetch si params changent !)
function Cabin({ cabinId }) {
  const { data } = useQuery({
    queryKey: ['cabin', cabinId], // Si cabinId change, refetch auto !
    queryFn: () => fetchCabin(cabinId),
  });
}

// ✅ KEY COMPLEXE avec filtres/pagination
function CabinsList({ filters, page }) {
  const { data } = useQuery({
    queryKey: ['cabins', { ...filters, page }],
    queryFn: () => fetchCabins(filters, page),
  });
  // Si filters ou page change → nouvelle query key → refetch !
}

// ✅ HIERARCHICAL KEYS (Best practice)
['cabins']                           // Liste
['cabins', 'detail', cabinId]       // Détail d'une cabin
['cabins', 'stats']                 // Statistiques
['bookings']                        // Liste
['bookings', 'detail', bookingId]   // Détail
['bookings', { status: 'active' }]  // Avec filtres
```

**⚡ Règle d'Or :** Si la query key change, React Query considère que c'est une query différente !

```javascript
// Exemple pratique
function BookingsList() {
  const [status, setStatus] = useState('all');
  
  const { data } = useQuery({
    queryKey: ['bookings', status], // Change quand status change
    queryFn: () => fetchBookings(status),
  });
  
  // Quand user clique sur "Active" → status change → 
  // query key change → refetch automatique !
  return (
    <>
      <button onClick={() => setStatus('all')}>All</button>
      <button onClick={() => setStatus('active')}>Active</button>
      <button onClick={() => setStatus('completed')}>Completed</button>
    </>
  );
}
```

### 3. staleTime vs cacheTime - LE Concept Crucial

**C'est LE concept le plus important à maîtriser !**

```javascript
const { data } = useQuery({
  queryKey: ['cabins'],
  queryFn: fetchCabins,
  staleTime: 5 * 60 * 1000,  // 5 minutes
  cacheTime: 10 * 60 * 1000, // 10 minutes
});
```

#### staleTime : Fraîcheur des Données

**staleTime = Combien de temps les données sont considérées "fresh" (fraîches)**

```javascript
// staleTime: 0 (défaut)
// → Data immédiatement "stale" après fetch
// → Refetch à chaque mount/window focus

// staleTime: 5 minutes
// → Data "fresh" pendant 5 min
// → Pas de refetch pendant ces 5 min, même si remount !

// staleTime: Infinity
// → Data JAMAIS stale (sauf invalidation manuelle)
// → Utile pour data vraiment statique
```

**Exemple concret :**

```javascript
// User navigue : Page A → Page B → Page A

// Avec staleTime: 0
// Page A mount → fetch cabins
// Navigate to Page B
// Back to Page A → REFETCH cabins (data stale)

// Avec staleTime: 5 minutes
// Page A mount → fetch cabins
// Navigate to Page B (< 5 min)
// Back to Page A → PAS de refetch (data encore fresh !)
```

#### cacheTime : Durée de Vie en Cache

**cacheTime = Combien de temps garder la data en cache APRÈS qu'elle ne soit plus utilisée**

```javascript
// Component avec query mount
// → Data fetched et mise en cache

// Component unmount
// → Timer de cacheTime démarre

// Si personne ne ré-utilise cette query pendant cacheTime
// → Data supprimée du cache

// Si quelqu'un ré-utilise avant la fin du cacheTime
// → Data encore là ! (instant display)
```

**Exemple visuel :**

```javascript
// T0: Component mount → fetch cabins → cache
// T1: Component unmount → cacheTime timer start (10 min)
// T2 (après 3 min): Component remount → data ENCORE en cache ! Instant display
// T11 (après 11 min sans usage): Data SUPPRIMÉE du cache
```

#### Stratégies par Type de Données

```javascript
// 1. DATA TEMPS RÉEL (prix, stocks, positions)
staleTime: 0,          // Toujours refetch
cacheTime: 1000,       // Peu de cache

// 2. DATA FRÉQUEMMENT MISE À JOUR (notifications, messages)
staleTime: 30 * 1000,  // 30 secondes
cacheTime: 5 * 60 * 1000, // 5 minutes

// 3. DATA MODÉRÉE (bookings, reservations)
staleTime: 5 * 60 * 1000,   // 5 minutes
cacheTime: 10 * 60 * 1000,  // 10 minutes

// 4. DATA QUASI-STATIQUE (settings, configurations)
staleTime: 30 * 60 * 1000,   // 30 minutes
cacheTime: 60 * 60 * 1000,   // 1 heure

// 5. DATA COMPLÈTEMENT STATIQUE (constantes, références)
staleTime: Infinity,   // Jamais stale
cacheTime: Infinity,   // Toujours en cache
```

### 4. Options Avancées de useQuery

#### enabled - Conditional Fetching

```javascript
// Use case 1: Query dépendante
function UserProfile({ userId }) {
  const { data: user } = useQuery({
    queryKey: ['user', userId],
    queryFn: () => fetchUser(userId),
  });
  
  // Ne fetch settings QUE si user existe
  const { data: settings } = useQuery({
    queryKey: ['settings', user?.id],
    queryFn: () => fetchSettings(user.id),
    enabled: !!user, // Désactivée jusqu'à ce que user soit chargé
  });
}

// Use case 2: Fetch conditionnel sur état UI
function SearchResults() {
  const [searchTerm, setSearchTerm] = useState('');
  
  const { data } = useQuery({
    queryKey: ['search', searchTerm],
    queryFn: () => searchCabins(searchTerm),
    enabled: searchTerm.length >= 3, // Fetch seulement si 3+ caractères
  });
}

// Use case 3: Lazy query (fetch sur action utilisateur)
function LazyLoadData() {
  const [shouldFetch, setShouldFetch] = useState(false);
  
  const { data } = useQuery({
    queryKey: ['data'],
    queryFn: fetchData,
    enabled: shouldFetch, // Fetch seulement quand user clique
  });
  
  return <button onClick={() => setShouldFetch(true)}>Load Data</button>;
}
```

#### select - Transformer les Données

```javascript
// Transformer/filtrer les données côté React Query
const { data: activeBookings } = useQuery({
  queryKey: ['bookings'],
  queryFn: fetchBookings,
  select: (data) => data.filter(booking => booking.status === 'active'),
  // select s'exécute APRÈS le fetch mais AVANT de mettre en state
  // Utile pour éviter re-renders inutiles
});

// Exemple avancé : Compute derived data
const { data: stats } = useQuery({
  queryKey: ['bookings'],
  queryFn: fetchBookings,
  select: (bookings) => ({
    total: bookings.length,
    active: bookings.filter(b => b.status === 'active').length,
    revenue: bookings.reduce((sum, b) => sum + b.totalPrice, 0),
  }),
});

// ⚡ select est memoized ! Si data ne change pas, pas de re-compute
```

#### placeholderData - Données Temporaires

```javascript
// Afficher des données temporaires pendant le loading
const { data } = useQuery({
  queryKey: ['cabin', cabinId],
  queryFn: () => fetchCabin(cabinId),
  placeholderData: () => {
    // Chercher dans le cache des cabins si disponible
    const cabins = queryClient.getQueryData(['cabins']);
    return cabins?.find(c => c.id === cabinId);
  },
  // Ou simplement des données par défaut
  // placeholderData: { name: 'Loading...', price: 0 },
});

// placeholderData n'est PAS mise en cache (contrairement à initialData)
```

#### initialData - Données Initiales

```javascript
const { data } = useQuery({
  queryKey: ['cabin', cabinId],
  queryFn: () => fetchCabin(cabinId),
  initialData: () => {
    const cabins = queryClient.getQueryData(['cabins']);
    return cabins?.find(c => c.id === cabinId);
  },
  initialDataUpdatedAt: () => {
    // Indiquer quand initialData a été fetchée
    return queryClient.getQueryState(['cabins'])?.dataUpdatedAt;
  },
});

// initialData EST mise en cache et considérée comme "vraie" data
```

#### keepPreviousData - Pour Pagination/Filtres

```javascript
function BookingsList() {
  const [page, setPage] = useState(1);
  
  const { data, isLoading, isPreviousData } = useQuery({
    queryKey: ['bookings', page],
    queryFn: () => fetchBookings(page),
    keepPreviousData: true, // GARDE les old data pendant fetch des nouvelles
  });
  
  // Sans keepPreviousData:
  // Page 1 → Page 2 : data = undefined pendant loading (UI vide/spinner)
  
  // Avec keepPreviousData:
  // Page 1 → Page 2 : data = page 1 data pendant loading (smooth UX !)
  
  return (
    <>
      <Table data={data} opacity={isPreviousData ? 0.5 : 1} />
      <button 
        onClick={() => setPage(p => p + 1)}
        disabled={isPreviousData} // Désactiver pendant fetch
      >
        Next Page
      </button>
    </>
  );
}
```

#### refetchInterval - Polling

```javascript
// Refetch automatique toutes les X millisecondes
const { data } = useQuery({
  queryKey: ['live-prices'],
  queryFn: fetchPrices,
  refetchInterval: 5000, // Refetch toutes les 5 secondes
  refetchIntervalInBackground: true, // Même quand tab inactive
});

// Polling conditionnel
const { data } = useQuery({
  queryKey: ['order', orderId],
  queryFn: () => fetchOrder(orderId),
  refetchInterval: (data) => {
    // Arrêter le polling si commande livrée
    return data?.status === 'delivered' ? false : 2000;
  },
});
```

#### onSuccess / onError / onSettled - Callbacks

```javascript
const { data } = useQuery({
  queryKey: ['cabins'],
  queryFn: fetchCabins,
  
  onSuccess: (data) => {
    console.log('Fetch réussi !', data);
    // Utile pour side effects après fetch
    // Ex: logger, analytics, redirections, etc.
  },
  
  onError: (error) => {
    console.error('Fetch échoué !', error);
    toast.error(error.message);
  },
  
  onSettled: (data, error) => {
    // S'exécute que ce soit success ou error
    console.log('Fetch terminé');
  },
});

// ⚠️ Note: onSuccess/onError/onSettled dépréciés dans v5
// Préférer useEffect pour les side effects
```

---

## ✏️ useMutation - Deep Dive Complet

### 1. Anatomie de useMutation

```javascript
const {
  // DATA
  data,        // Data retournée par mutationFn après success
  error,       // Erreur si échec
  
  // STATUS
  isLoading,   // Mutation en cours (ancien nom: isLoading)
  isPending,   // Même chose (nouveau nom en v5)
  isError,     // Erreur
  isSuccess,   // Succès
  isIdle,      // Pas encore appelée
  status,      // 'idle' | 'pending' | 'error' | 'success'
  
  // FUNCTIONS
  mutate,      // Appel fire-and-forget
  mutateAsync, // Appel qui retourne une Promise
  reset,       // Reset le state à idle
  
  // METADATA
  failureCount,
  failureReason,
} = useMutation({
  mutationFn: createCabin,
  // ... options
});
```

### 2. mutate vs mutateAsync - Différence Cruciale

```javascript
// ❌ mutate - Fire and forget (pas de Promise)
function CreateForm() {
  const { mutate } = useMutation({
    mutationFn: createCabin,
  });
  
  const handleSubmit = (data) => {
    mutate(data); // Pas de await ! Pas de Promise !
    // ⚠️ Vous NE POUVEZ PAS faire :
    // navigate('/cabins'); // Exécuté IMMÉDIATEMENT (avant fin mutation)
  };
}

// ✅ mutateAsync - Retourne une Promise
function CreateForm() {
  const { mutateAsync } = useMutation({
    mutationFn: createCabin,
  });
  
  const handleSubmit = async (data) => {
    try {
      await mutateAsync(data); // Attend la fin !
      navigate('/cabins'); // ✅ Exécuté APRÈS succès
    } catch (error) {
      console.error(error); // ✅ Catch les erreurs
    }
  };
}

// 🎯 Utiliser mutate pour la plupart des cas (avec onSuccess)
// 🎯 Utiliser mutateAsync si vous avez besoin d'await
```

### 3. Cache Invalidation - 3 Stratégies

#### Stratégie 1 : invalidateQueries (Refetch complet)

```javascript
const { mutate } = useMutation({
  mutationFn: deleteCabin,
  onSuccess: () => {
    // ✅ Invalide et REFETCH la liste des cabins
    queryClient.invalidateQueries({ queryKey: ['cabins'] });
    
    // Invalider plusieurs queries
    queryClient.invalidateQueries({ queryKey: ['cabins'] });
    queryClient.invalidateQueries({ queryKey: ['cabins-stats'] });
    
    // Invalider TOUTES les queries qui commencent par 'cabins'
    queryClient.invalidateQueries({ 
      queryKey: ['cabins'],
      refetchType: 'all' // 'active' | 'inactive' | 'all'
    });
  },
});
```

**Quand utiliser :** Mutations complexes où la data change de façon imprévisible

#### Stratégie 2 : setQueryData (Update optimiste du cache)

```javascript
const { mutate } = useMutation({
  mutationFn: updateCabin,
  onSuccess: (updatedCabin) => {
    // ✅ Update direct du cache SANS refetch
    
    // Update une cabin spécifique
    queryClient.setQueryData(['cabin', updatedCabin.id], updatedCabin);
    
    // Update dans la liste
    queryClient.setQueryData(['cabins'], (oldCabins) => {
      return oldCabins.map(cabin =>
        cabin.id === updatedCabin.id ? updatedCabin : cabin
      );
    });
  },
});

// Pour DELETE
const { mutate: deleteMutation } = useMutation({
  mutationFn: deleteCabin,
  onSuccess: (_, deletedId) => {
    // Remove de la liste
    queryClient.setQueryData(['cabins'], (old) =>
      old.filter(cabin => cabin.id !== deletedId)
    );
    
    // Remove du cache individuel
    queryClient.removeQueries({ queryKey: ['cabin', deletedId] });
  },
});

// Pour CREATE
const { mutate: createMutation } = useMutation({
  mutationFn: createCabin,
  onSuccess: (newCabin) => {
    // Ajouter à la liste
    queryClient.setQueryData(['cabins'], (old) => [...old, newCabin]);
  },
});
```

**Quand utiliser :** Quand vous savez EXACTEMENT comment la data change (update, delete, create simple)

#### Stratégie 3 : Optimistic Updates (Update AVANT la mutation)

```javascript
const { mutate } = useMutation({
  mutationFn: updateBooking,
  
  // 1️⃣ AVANT la mutation (optimistic update)
  onMutate: async (updatedBooking) => {
    // Cancel les queries en cours pour éviter conflicts
    await queryClient.cancelQueries({ queryKey: ['bookings'] });
    
    // Snapshot du cache actuel (pour rollback si erreur)
    const previousBookings = queryClient.getQueryData(['bookings']);
    
    // Update optimiste du cache
    queryClient.setQueryData(['bookings'], (old) =>
      old.map(booking =>
        booking.id === updatedBooking.id ? updatedBooking : booking
      )
    );
    
    // Retourner context pour les autres callbacks
    return { previousBookings };
  },
  
  // 2️⃣ Si ERREUR (rollback)
  onError: (error, variables, context) => {
    // Restaurer les données précédentes
    queryClient.setQueryData(['bookings'], context.previousBookings);
    toast.error('Update failed!');
  },
  
  // 3️⃣ TOUJOURS à la fin (refetch pour sync)
  onSettled: () => {
    // Refetch pour être sûr d'avoir la vraie data du serveur
    queryClient.invalidateQueries({ queryKey: ['bookings'] });
  },
});
```

**Quand utiliser :** Quand vous voulez une UX ultra-rapide (UI update instantanée)

### 4. Exemple Complet : CRUD Complet d'une Resource

```javascript
// services/apiCabins.js
export async function getCabins() {
  const { data, error } = await supabase
    .from('cabins')
    .select('*')
    .order('name');
  
  if (error) throw new Error(error.message);
  return data;
}

export async function getCabin(id) {
  const { data, error } = await supabase
    .from('cabins')
    .select('*')
    .eq('id', id)
    .single();
  
  if (error) throw new Error(error.message);
  return data;
}

export async function createCabin(newCabin) {
  const { data, error } = await supabase
    .from('cabins')
    .insert([newCabin])
    .select()
    .single();
  
  if (error) throw new Error(error.message);
  return data;
}

export async function updateCabin(id, updates) {
  const { data, error } = await supabase
    .from('cabins')
    .update(updates)
    .eq('id', id)
    .select()
    .single();
  
  if (error) throw new Error(error.message);
  return data;
}

export async function deleteCabin(id) {
  const { error } = await supabase
    .from('cabins')
    .delete()
    .eq('id', id);
  
  if (error) throw new Error(error.message);
}

// hooks/useCabins.js
import { useQuery } from '@tanstack/react-query';
import { getCabins } from '../services/apiCabins';

export function useCabins() {
  const {
    data: cabins,
    isLoading,
    error,
  } = useQuery({
    queryKey: ['cabins'],
    queryFn: getCabins,
    staleTime: 5 * 60 * 1000, // 5 minutes
  });
  
  return { cabins, isLoading, error };
}

// hooks/useCabin.js
export function useCabin(cabinId) {
  const {
    data: cabin,
    isLoading,
    error,
  } = useQuery({
    queryKey: ['cabin', cabinId],
    queryFn: () => getCabin(cabinId),
    enabled: !!cabinId,
    // Prefill depuis le cache si disponible
    placeholderData: () => {
      const cabins = queryClient.getQueryData(['cabins']);
      return cabins?.find(c => c.id === cabinId);
    },
  });
  
  return { cabin, isLoading, error };
}

// hooks/useCreateCabin.js
import { useMutation, useQueryClient } from '@tantml/react-query';
import { createCabin as createCabinApi } from '../services/apiCabins';
import toast from 'react-hot-toast';

export function useCreateCabin() {
  const queryClient = useQueryClient();
  
  const {
    mutate: createCabin,
    isLoading: isCreating,
    error,
  } = useMutation({
    mutationFn: createCabinApi,
    
    onSuccess: (newCabin) => {
      toast.success('Cabin successfully created');
      
      // Stratégie 1: Invalider (refetch)
      queryClient.invalidateQueries({ queryKey: ['cabins'] });
      
      // OU Stratégie 2: Update direct du cache
      // queryClient.setQueryData(['cabins'], (old) => [...old, newCabin]);
    },
    
    onError: (err) => {
      toast.error(err.message);
    },
  });
  
  return { createCabin, isCreating, error };
}

// hooks/useUpdateCabin.js
export function useUpdateCabin() {
  const queryClient = useQueryClient();
  
  const {
    mutate: updateCabin,
    isLoading: isUpdating,
  } = useMutation({
    mutationFn: ({ id, updates }) => updateCabinApi(id, updates),
    
    onSuccess: (updatedCabin) => {
      toast.success('Cabin successfully updated');
      
      // Update le cache individuel
      queryClient.setQueryData(['cabin', updatedCabin.id], updatedCabin);
      
      // Update dans la liste
      queryClient.setQueryData(['cabins'], (old) =>
        old.map(cabin =>
          cabin.id === updatedCabin.id ? updatedCabin : cabin
        )
      );
    },
    
    onError: (err) => toast.error(err.message),
  });
  
  return { updateCabin, isUpdating };
}

// hooks/useDeleteCabin.js
export function useDeleteCabin() {
  const queryClient = useQueryClient();
  
  const {
    mutate: deleteCabin,
    isLoading: isDeleting,
  } = useMutation({
    mutationFn: deleteCabinApi,
    onSuccess: (_, deletedId) => {
      toast.success('Cabin successfully deleted');
      
      // Remove du cache individuel
      queryClient.removeQueries({ queryKey: ['cabin', deletedId] });
      
      // Update la liste
      queryClient.setQueryData(['cabins'], (old) =>
        old.filter(cabin => cabin.id !== deletedId)
      );
    },
    
    onError: (err) => toast.error(err.message),
  });
  
  return { deleteCabin, isDeleting };
}

// components/CabinsList.jsx
import { useCabins } from '../hooks/useCabins';
import { useDeleteCabin } from '../hooks/useDeleteCabin';

function CabinsList() {
  const { cabins, isLoading, error } = useCabins();
  const { deleteCabin, isDeleting } = useDeleteCabin();
  
  if (isLoading) return <Spinner />;
  if (error) return <Error message={error.message} />;
  
  return (
    <div>
      {cabins.map(cabin => (
        <CabinRow 
          key={cabin.id} 
          cabin={cabin}
          onDelete={() => deleteCabin(cabin.id)}
          isDeleting={isDeleting}
        />
      ))}
    </div>
  );
}

// components/CreateCabinForm.jsx
import { useCreateCabin } from '../hooks/useCreateCabin';
import { useForm } from 'react-hook-form';

function CreateCabinForm() {
  const { createCabin, isCreating } = useCreateCabin();
  const { register, handleSubmit, reset } = useForm();
  
  function onSubmit(data) {
    createCabin(data, {
      onSuccess: () => {
        reset(); // Reset form après succès
      },
    });
  }
  
  return (
    <form onSubmit={handleSubmit(onSubmit)}>
      <input {...register('name')} disabled={isCreating} />
      <input {...register('maxCapacity')} disabled={isCreating} />
      <input {...register('regularPrice')} disabled={isCreating} />
      <button disabled={isCreating}>
        {isCreating ? 'Creating...' : 'Create Cabin'}
      </button>
    </form>
  );
}

// components/EditCabinForm.jsx
import { useCabin } from '../hooks/useCabin';
import { useUpdateCabin } from '../hooks/useUpdateCabin';

function EditCabinForm({ cabinId }) {
  const { cabin, isLoading } = useCabin(cabinId);
  const { updateCabin, isUpdating } = useUpdateCabin();
  const { register, handleSubmit } = useForm({
    defaultValues: cabin, // Pre-fill form
  });
  
  function onSubmit(data) {
    updateCabin({ id: cabinId, updates: data });
  }
  
  if (isLoading) return <Spinner />;
  
  return (
    <form onSubmit={handleSubmit(onSubmit)}>
      <input {...register('name')} disabled={isUpdating} />
      <button disabled={isUpdating}>
        {isUpdating ? 'Updating...' : 'Update Cabin'}
      </button>
    </form>
  );
}
```

---

## 🎨 Patterns Avancés et Use Cases Réels

### 1. Dependent Queries (Queries en Cascade)

```javascript
// Use case: Fetch booking → puis fetch cabin details
function BookingDetail({ bookingId }) {
  // Query 1: Fetch booking
  const { data: booking } = useQuery({
    queryKey: ['booking', bookingId],
    queryFn: () => fetchBooking(bookingId),
  });
  
  // Query 2: Dépend de Query 1 (attend cabinId)
  const { data: cabin } = useQuery({
    queryKey: ['cabin', booking?.cabinId],
    queryFn: () => fetchCabin(booking.cabinId),
    enabled: !!booking?.cabinId, // N'exécute QUE si booking chargé
  });
  
  // Query 3: Dépend aussi de Query 1
  const { data: guest } = useQuery({
    queryKey: ['guest', booking?.guestId],
    queryFn: () => fetchGuest(booking.guestId),
    enabled: !!booking?.guestId,
  });
  
  if (!booking) return <Spinner />;
  
  return (
    <div>
      <h2>Booking #{booking.id}</h2>
      {cabin && <CabinDetails cabin={cabin} />}
      {guest && <GuestDetails guest={guest} />}
    </div>
  );
}
```

### 2. Parallel Queries (Plusieurs queries simultanées)

```javascript
// Use case: Dashboard avec plusieurs sources de données
function Dashboard() {
  // Toutes ces queries s'exécutent en PARALLÈLE
  const cabinsQuery = useQuery({
    queryKey: ['cabins'],
    queryFn: fetchCabins,
  });
  
  const bookingsQuery = useQuery({
    queryKey: ['bookings'],
    queryFn: fetchBookings,
  });
  
  const statsQuery = useQuery({
    queryKey: ['stats'],
    queryFn: fetchStats,
  });
  
  // Attendre que TOUTES soient chargées
  const isLoading = cabinsQuery.isLoading || 
                    bookingsQuery.isLoading || 
                    statsQuery.isLoading;
  
  if (isLoading) return <Spinner />;
  
  return (
    <div>
      <StatsCards stats={statsQuery.data} />
      <CabinsChart cabins={cabinsQuery.data} />
      <BookingsTable bookings={bookingsQuery.data} />
    </div>
  );
}

// Ou utiliser useQueries pour dynamique
function DynamicDashboard({ cabinIds }) {
  const cabinQueries = useQueries({
    queries: cabinIds.map(id => ({
      queryKey: ['cabin', id],
      queryFn: () => fetchCabin(id),
      staleTime: 5 * 60 * 1000,
    })),
  });
  
  // Vérifier si toutes sont chargées
  const isLoading = cabinQueries.some(q => q.isLoading);
  const cabins = cabinQueries.map(q => q.data).filter(Boolean);
  
  return <CabinsList cabins={cabins} />;
}
```

### 3. Prefetching (Anticiper les besoins)

```javascript
// Use case 1: Prefetch au hover
function CabinsList() {
  const queryClient = useQueryClient();
  const { data: cabins } = useCabins();
  
  const prefetchCabin = (cabinId) => {
    queryClient.prefetchQuery({
      queryKey: ['cabin', cabinId],
      queryFn: () => fetchCabin(cabinId),
      staleTime: 5 * 60 * 1000,
    });
  };
  
  return (
    <div>
      {cabins.map(cabin => (
        <CabinCard
          key={cabin.id}
          cabin={cabin}
          onMouseEnter={() => prefetchCabin(cabin.id)}
          // Quand user clique, data déjà en cache !
        />
      ))}
    </div>
  );
}

// Use case 2: Prefetch page suivante (pagination)
function BookingsList() {
  const [page, setPage] = useState(1);
  const queryClient = useQueryClient();
  
  const { data, isLoading } = useQuery({
    queryKey: ['bookings', page],
    queryFn: () => fetchBookings(page),
    keepPreviousData: true,
  });
  
  // Prefetch automatiquement la page suivante
  useEffect(() => {
    if (data?.hasMore) {
      queryClient.prefetchQuery({
        queryKey: ['bookings', page + 1],
        queryFn: () => fetchBookings(page + 1),
      });
    }
  }, [page, data, queryClient]);
  
  return (
    <>
      <Table data={data.bookings} />
      <Pagination page={page} setPage={setPage} />
    </>
  );
}

// Use case 3: Prefetch au mount de l'app (données critiques)
function App() {
  const queryClient = useQueryClient();
  
  useEffect(() => {
    // Prefetch données critiques au démarrage
    queryClient.prefetchQuery({
      queryKey: ['user'],
      queryFn: fetchCurrentUser,
    });
    
    queryClient.prefetchQuery({
      queryKey: ['settings'],
      queryFn: fetchSettings,
    });
  }, [queryClient]);
  
  return <AppRoutes />;
}
```

### 4. Pagination avec React Query

```javascript
// Pattern 1: Pagination simple (offset-based)
function BookingsList() {
  const [page, setPage] = useState(1);
  const pageSize = 10;
  
  const { data, isLoading, isPreviousData } = useQuery({
    queryKey: ['bookings', page],
    queryFn: () => fetchBookings({ page, pageSize }),
    keepPreviousData: true, // Garde old data pendant fetch
  });
  
  return (
    <>
      <Table 
        data={data.bookings} 
        style={{ opacity: isPreviousData ? 0.5 : 1 }}
      />
      <div>
        <button
          onClick={() => setPage(p => Math.max(1, p - 1))}
          disabled={page === 1 || isPreviousData}
        >
          Previous
        </button>
        <span>Page {page}</span>
        <button
          onClick={() => setPage(p => p + 1)}
          disabled={!data?.hasMore || isPreviousData}
        >
          Next
        </button>
      </div>
    </>
  );
}

// Pattern 2: Infinite scroll (cursor-based)
import { useInfiniteQuery } from '@tanstack/react-query';

function InfiniteBookingsList() {
  const {
    data,
    fetchNextPage,
    hasNextPage,
    isFetchingNextPage,
    isLoading,
  } = useInfiniteQuery({
    queryKey: ['bookings-infinite'],
    queryFn: ({ pageParam = 0 }) => fetchBookings({ cursor: pageParam }),
    getNextPageParam: (lastPage) => lastPage.nextCursor ?? undefined,
    // lastPage = dernière page fetchée
    // Si lastPage.nextCursor existe → il y a une page suivante
  });
  
  // data.pages = array de toutes les pages
  const allBookings = data?.pages.flatMap(page => page.bookings) ?? [];
  
  return (
    <div>
      {allBookings.map(booking => (
        <BookingCard key={booking.id} booking={booking} />
      ))}
      
      {hasNextPage && (
        <button
          onClick={() => fetchNextPage()}
          disabled={isFetchingNextPage}
        >
          {isFetchingNextPage ? 'Loading...' : 'Load More'}
        </button>
      )}
    </div>
  );
}

// Pattern 3: Infinite scroll automatique (avec IntersectionObserver)
import { useInView } from 'react-intersection-observer';

function AutoInfiniteList() {
  const { ref, inView } = useInView();
  
  const {
    data,
    fetchNextPage,
    hasNextPage,
    isFetchingNextPage,
  } = useInfiniteQuery({
    queryKey: ['bookings-auto-infinite'],
    queryFn: ({ pageParam = 0 }) => fetchBookings({ cursor: pageParam }),
    getNextPageParam: (lastPage) => lastPage.nextCursor,
  });
  
  // Fetch automatiquement quand sentinel visible
  useEffect(() => {
    if (inView && hasNextPage) {
      fetchNextPage();
    }
  }, [inView, hasNextPage, fetchNextPage]);
  
  const allBookings = data?.pages.flatMap(page => page.bookings) ?? [];
  
  return (
    <div>
      {allBookings.map(booking => (
        <BookingCard key={booking.id} booking={booking} />
      ))}
      
      {/* Sentinel - déclenche fetch quand visible */}
      {hasNextPage && (
        <div ref={ref} style={{ height: 20 }}>
          {isFetchingNextPage && <Spinner />}
        </div>
      )}
    </div>
  );
}
```

### 5. Search/Filter avec React Query

```javascript
function SearchableCabinsList() {
  const [search, setSearch] = useState('');
  const [filters, setFilters] = useState({
    minPrice: 0,
    maxCapacity: 10,
    discount: false,
  });
  
  // Query key inclut search + filters
  // → Change automatiquement → refetch auto !
  const { data: cabins, isLoading } = useQuery({
    queryKey: ['cabins', { search, ...filters }],
    queryFn: () => fetchCabins({ search, ...filters }),
    keepPreviousData: true,
    
    // Debounce search (ne fetch pas à chaque caractère)
    enabled: search.length === 0 || search.length >= 3,
  });
  
  return (
    <div>
      <input
        value={search}
        onChange={(e) => setSearch(e.target.value)}
        placeholder="Search cabins..."
      />
      
      <FilterPanel filters={filters} onChange={setFilters} />
      
      {isLoading ? <Spinner /> : <CabinsList cabins={cabins} />}
    </div>
  );
}

// Optimisation: Debounce avec useDebounce hook
function useDebouncedValue(value, delay = 500) {
  const [debouncedValue, setDebouncedValue] = useState(value);
  
  useEffect(() => {
    const timer = setTimeout(() => setDebouncedValue(value), delay);
    return () => clearTimeout(timer);
  }, [value, delay]);
  
  return debouncedValue;
}

function OptimizedSearchCabins() {
  const [search, setSearch] = useState('');
  const debouncedSearch = useDebouncedValue(search, 500);
  
  const { data: cabins } = useQuery({
    queryKey: ['cabins', debouncedSearch],
    queryFn: () => fetchCabins({ search: debouncedSearch }),
    enabled: debouncedSearch.length >= 3,
  });
  
  // User tape "cabin" → Fetch seulement après 500ms d'inactivité
  return <input value={search} onChange={(e) => setSearch(e.target.value)} />;
}
```

### 6. Optimistic Updates - Pattern Complet

```javascript
// Use case: Update status d'une booking
function useUpdateBookingStatus() {
  const queryClient = useQueryClient();
  
  return useMutation({
    mutationFn: ({ bookingId, status }) => 
      updateBookingStatus(bookingId, status),
    
    // 1. AVANT mutation (UI update instantanée)
    onMutate: async ({ bookingId, status }) => {
      // Cancel toutes les queries en cours
      await queryClient.cancelQueries({ queryKey: ['bookings'] });
      await queryClient.cancelQueries({ queryKey: ['booking', bookingId] });
      
      // Snapshot pour rollback
      const previousBookings = queryClient.getQueryData(['bookings']);
      const previousBooking = queryClient.getQueryData(['booking', bookingId]);
      
      // Update optimiste
      queryClient.setQueryData(['bookings'], (old) =>
        old?.map(booking =>
          booking.id === bookingId
            ? { ...booking, status }
            : booking
        )
      );
      
      queryClient.setQueryData(['booking', bookingId], (old) =>
        old ? { ...old, status } : old
      );
      
      // UI déjà updated ! User voit le changement instantanément
      
      return { previousBookings, previousBooking };
    },
    
    // 2. Si ERREUR (rollback)
    onError: (error, variables, context) => {
      // Restaurer les valeurs précédentes
      if (context?.previousBookings) {
        queryClient.setQueryData(['bookings'], context.previousBookings);
      }
      if (context?.previousBooking) {
        queryClient.setQueryData(
          ['booking', variables.bookingId],
          context.previousBooking
        );
      }
      
      toast.error('Failed to update booking status');
    },
    
    // 3. TOUJOURS à la fin (refetch pour sync avec serveur)
    onSettled: (data, error, variables) => {
      queryClient.invalidateQueries({ queryKey: ['bookings'] });
      queryClient.invalidateQueries({ 
        queryKey: ['booking', variables.bookingId] 
      });
    },
    
    onSuccess: () => {
      toast.success('Booking status updated');
    },
  });
}

// Usage
function BookingRow({ booking }) {
  const { mutate: updateStatus, isLoading } = useUpdateBookingStatus();
  
  const handleConfirm = () => {
    updateStatus({ 
      bookingId: booking.id, 
      status: 'confirmed' 
    });
    // UI update INSTANTANÉE ! Pas d'attente serveur
  };
  
  return (
    <div>
      <span>{booking.status}</span>
      <button onClick={handleConfirm} disabled={isLoading}>
        Confirm
      </button>
    </div>
  );
}
```

### 7. Polling (Auto-refetch périodique)

```javascript
// Use case 1: Live data (orders, notifications, etc.)
function LiveOrders() {
  const { data: orders } = useQuery({
    queryKey: ['orders'],
    queryFn: fetchOrders,
    refetchInterval: 5000, // Refetch toutes les 5 secondes
    refetchIntervalInBackground: false, // Pause si tab inactive
  });
  
  return <OrdersList orders={orders} />;
}

// Use case 2: Polling conditionnel (jusqu'à ce qu'une condition soit remplie)
function OrderTracking({ orderId }) {
  const { data: order } = useQuery({
    queryKey: ['order', orderId],
    queryFn: () => fetchOrder(orderId),
    refetchInterval: (data) => {
      // Arrêter le polling si commande livrée
      if (data?.status === 'delivered') return false;
      
      // Sinon, continue à poller
      return 3000; // toutes les 3 secondes
    },
  });
  
  return (
    <div>
      <h3>Order Status: {order?.status}</h3>
      {order?.status !== 'delivered' && <Spinner text="Tracking..." />}
    </div>
  );
}

// Use case 3: Polling intelligent (exponentiel backoff)
function SmartPolling() {
  const [pollInterval, setPollInterval] = useState(1000);
  
  const { data, dataUpdatedAt } = useQuery({
    queryKey: ['data'],
    queryFn: fetchData,
    refetchInterval: pollInterval,
    onSuccess: (newData) => {
      // Si data change, reset à 1 seconde
      // Sinon, double l'intervalle (jusqu'à 30 sec max)
      setPollInterval(prev => Math.min(prev * 2, 30000));
    },
  });
  
  // S'adapte automatiquement selon la fréquence des updates !
}
```

### 8. Cache Persistence (Sauvegarder le cache)

```javascript
import { persistQueryClient } from '@tanstack/react-query-persist-client';
import { createSyncStoragePersister } from '@tanstack/query-sync-storage-persister';

// Créer persister pour localStorage
const persister = createSyncStoragePersister({
  storage: window.localStorage,
  key: 'REACT_QUERY_CACHE',
});

// Setup
const queryClient = new QueryClient({
  defaultOptions: {
    queries: {
      cacheTime: 1000 * 60 * 60 * 24, // 24 heures
    },
  },
});

// Persist cache
persistQueryClient({
  queryClient,
  persister,
  maxAge: 1000 * 60 * 60 * 24, // 24 heures
});

// Maintenant le cache est sauvegardé dans localStorage !
// → User refresh la page → data toujours là !
// → Meilleure UX, moins de loading
```

---

## 🔥 Patterns Wild Oasis (Votre Projet)

### Structure de Fichiers Recommandée

```
src/
├── services/
│   ├── apiCabins.js      # API calls brutes
│   ├── apiBookings.js
│   ├── apiAuth.js
│   └── apiSettings.js
│
├── hooks/
│   ├── useCabins.js      # useQuery hooks
│   ├── useCabin.js
│   ├── useCreateCabin.js # useMutation hooks
│   ├── useUpdateCabin.js
│   ├── useDeleteCabin.js
│   ├── useBookings.js
│   └── ...
│
├── features/
│   ├── cabins/
│   │   ├── CabinsList.jsx
│   │   ├── CabinRow.jsx
│   │   ├── CreateCabinForm.jsx
│   │   └── EditCabinForm.jsx
│   │
│   ├── bookings/
│   │   ├── BookingsList.jsx
│   │   ├── BookingDetail.jsx
│   │   └── ...
│   │
│   └── ...
│
└── App.jsx
```

### Exemple Complet: Feature "Cabins"

```javascript
// services/apiCabins.js
import supabase from './supabase';

export async function getCabins() {
  const { data, error } = await supabase
    .from('cabins')
    .select('*')
    .order('name');
  
  if (error) {
    console.error(error);
    throw new Error('Cabins could not be loaded');
  }
  
  return data;
}

export async function createEditCabin(newCabin, id) {
  const hasImagePath = newCabin.image?.startsWith?.(supabase.storageUrl);
  
  // 1. Create/edit cabin
  let query = supabase.from('cabins');
  
  // A) CREATE
  if (!id) query = query.insert([{ ...newCabin, image: imagePath }]);
  
  // B) EDIT
  if (id) query = query.update({ ...newCabin, image: imagePath }).eq('id', id);
  
  const { data, error } = await query.select().single();
  
  if (error) {
    console.error(error);
    throw new Error('Cabin could not be created');
  }
  
  // 2. Upload image
  if (hasImagePath) return data;
  
  const { error: storageError } = await supabase.storage
    .from('cabin-images')
    .upload(imageName, newCabin.image);
  
  // 3. Delete cabin if image upload failed
  if (storageError) {
    await supabase.from('cabins').delete().eq('id', data.id);
    throw new Error('Image could not be uploaded');
  }
  
  return data;
}

export async function deleteCabin(id) {
  const { data, error } = await supabase
    .from('cabins')
    .delete()
    .eq('id', id);
  
  if (error) {
    console.error(error);
    throw new Error('Cabin could not be deleted');
  }
  
  return data;
}

// hooks/useCabins.js
import { useQuery } from '@tanstack/react-query';
import { getCabins } from '../services/apiCabins';

export function useCabins() {
  const {
    isLoading,
    data: cabins,
    error,
  } = useQuery({
    queryKey: ['cabins'],
    queryFn: getCabins,
  });
  
  return { isLoading, error, cabins };
}

// hooks/useCreateCabin.js
import { useMutation, useQueryClient } from '@tanstack/react-query';
import { createEditCabin } from '../services/apiCabins';
import toast from 'react-hot-toast';

export function useCreateCabin() {
  const queryClient = useQueryClient();
  
  const { mutate: createCabin, isLoading: isCreating } = useMutation({
    mutationFn: createEditCabin,
    onSuccess: () => {
      toast.success('New cabin successfully created');
      queryClient.invalidateQueries({ queryKey: ['cabins'] });
    },
    onError: (err) => toast.error(err.message),
  });
  
  return { isCreating, createCabin };
}

// hooks/useEditCabin.js
export function useEditCabin() {
  const queryClient = useQueryClient();
  
  const { mutate: editCabin, isLoading: isEditing } = useMutation({
    mutationFn: ({ newCabinData, id }) => createEditCabin(newCabinData, id),
    onSuccess: () => {
      toast.success('Cabin successfully edited');
      queryClient.invalidateQueries({ queryKey: ['cabins'] });
    },
    onError: (err) => toast.error(err.message),
  });
  
  return { isEditing, editCabin };
}

// hooks/useDeleteCabin.js
export function useDeleteCabin() {
  const queryClient = useQueryClient();
  
  const { isLoading: isDeleting, mutate: deleteCabin } = useMutation({
    mutationFn: deleteCabin,
    onSuccess: () => {
      toast.success('Cabin successfully deleted');
      queryClient.invalidateQueries({ queryKey: ['cabins'] });
    },
    onError: (err) => toast.error(err.message),
  });
  
  return { isDeleting, deleteCabin };
}

// features/cabins/CabinsList.jsx
import { useCabins } from '../../hooks/useCabins';
import CabinRow from './CabinRow';
import Spinner from '../../ui/Spinner';

function CabinsList() {
  const { isLoading, cabins } = useCabins();
  
  if (isLoading) return <Spinner />;
  
  return (
    <Table columns="0.6fr 1.8fr 2.2fr 1fr 1fr 1fr">
      <Table.Header>
        <div></div>
        <div>Cabin</div>
        <div>Capacity</div>
        <div>Price</div>
        <div>Discount</div>
        <div></div>
      </Table.Header>
      
      <Table.Body
        data={cabins}
        render={(cabin) => <CabinRow cabin={cabin} key={cabin.id} />}
      />
    </Table>
  );
}

// features/cabins/CabinRow.jsx
import { useDeleteCabin } from '../../hooks/useDeleteCabin';
import { HiPencil, HiTrash } from 'react-icons/hi2';

function CabinRow({ cabin }) {
  const { isDeleting, deleteCabin } = useDeleteCabin();
  
  return (
    <Table.Row>
      <Img src={cabin.image} />
      <Cabin>{cabin.name}</Cabin>
      <div>Fits up to {cabin.maxCapacity} guests</div>
      <Price>{formatCurrency(cabin.regularPrice)}</Price>
      <Discount>{formatCurrency(cabin.discount)}</Discount>
      <div>
        <button onClick={() => setShowForm(true)}>
          <HiPencil />
        </button>
        <button 
          onClick={() => deleteCabin(cabin.id)} 
          disabled={isDeleting}
        >
          <HiTrash />
        </button>
      </div>
    </Table.Row>
  );
}

// features/cabins/CreateCabinForm.jsx
import { useForm } from 'react-hook-form';
import { useCreateCabin } from '../../hooks/useCreateCabin';
import { useEditCabin } from '../../hooks/useEditCabin';

function CreateCabinForm({ cabinToEdit = {}, onCloseModal }) {
  const { id: editId, ...editValues } = cabinToEdit;
  const isEditSession = Boolean(editId);
  
  const { register, handleSubmit, reset, formState } = useForm({
    defaultValues: isEditSession ? editValues : {},
  });
  const { errors } = formState;
  
  const { isCreating, createCabin } = useCreateCabin();
  const { isEditing, editCabin } = useEditCabin();
  
  const isWorking = isCreating || isEditing;
  
  function onSubmit(data) {
    const image = typeof data.image === 'string' ? data.image : data.image[0];
    
    if (isEditSession) {
      editCabin(
        { newCabinData: { ...data, image }, id: editId },
        {
          onSuccess: () => {
            reset();
            onCloseModal?.();
          },
        }
      );
    } else {
      createCabin(
        { ...data, image },
        {
          onSuccess: () => {
            reset();
            onCloseModal?.();
          },
        }
      );
    }
  }
  
  return (
    <Form onSubmit={handleSubmit(onSubmit)}>
      <FormRow label="Cabin name" error={errors?.name?.message}>
        <Input
          type="text"
          id="name"
          disabled={isWorking}
          {...register('name', {
            required: 'This field is required',
          })}
        />
      </FormRow>
      
      <FormRow label="Maximum capacity" error={errors?.maxCapacity?.message}>
        <Input
          type="number"
          id="maxCapacity"
          disabled={isWorking}
          {...register('maxCapacity', {
            required: 'This field is required',
            min: {
              value: 1,
              message: 'Capacity should be at least 1',
            },
          })}
        />
      </FormRow>
      
      {/* ... autres champs ... */}
      
      <FormRow>
        <Button
          variation="secondary"
          type="reset"
          onClick={() => onCloseModal?.()}
        >
          Cancel
        </Button>
        <Button disabled={isWorking}>
          {isEditSession ? 'Edit cabin' : 'Create new cabin'}
        </Button>
      </FormRow>
    </Form>
  );
}

export default CreateCabinForm;
```

---

## ⚠️ Pièges Courants et Solutions

### 1. Query Key Instable (Re-render Infini)

```javascript
// ❌ PIÈGE: Object/Array dans query key sans stabilité
function BadExample() {
  const filters = { status: 'active' }; // Nouveau object à chaque render !
  
  const { data } = useQuery({
    queryKey: ['cabins', filters], // Query key change à chaque render !
    queryFn: () => fetchCabins(filters),
  });
  // → Re-render infini ! 💥
}

// ✅ SOLUTION 1: useState pour stabilité
function GoodExample1() {
  const [filters, setFilters] = useState({ status: 'active' });
  
  const { data } = useQuery({
    queryKey: ['cabins', filters], // Stable maintenant
    queryFn: () => fetchCabins(filters),
  });
}

// ✅ SOLUTION 2: useMemo
function GoodExample2({ status }) {
  const filters = useMemo(() => ({ status }), [status]);
  
  const { data } = useQuery({
    queryKey: ['cabins', filters],
    queryFn: () => fetchCabins(filters),
  });
}

// ✅ SOLUTION 3: Primitive values directement
function GoodExample3({ status, page }) {
  const { data } = useQuery({
    queryKey: ['cabins', status, page], // Primitives = stable
    queryFn: () => fetchCabins({ status, page }),
  });
}
```

### 2. Oublier le QueryClientProvider

```javascript
// ❌ ERREUR commune
function App() {
  return <CabinsList />; // useQuery inside → CRASH !
}
// Error: No QueryClient set, use QueryClientProvider

// ✅ SOLUTION
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';

const queryClient = new QueryClient();

function App() {
  return (
    <QueryClientProvider client={queryClient}>
      <CabinsList />
    </QueryClientProvider>
  );
}
```

### 3. Mutation sans Invalidation du Cache

```javascript
// ❌ PIÈGE: Cache pas updated après mutation
function BadMutation() {
  const { mutate } = useMutation({
    mutationFn: createCabin,
    onSuccess: () => {
      toast.success('Created!');
      // Mais la liste des cabins n'est PAS refetchée !
    },
  });
  
  // User crée une cabin → success toast
  // Mais la nouvelle cabin n'apparaît PAS dans la liste ! 😱
}

// ✅ SOLUTION: Toujours invalider
function GoodMutation() {
  const queryClient = useQueryClient();
  
  const { mutate } = useMutation({
    mutationFn: createCabin,
    onSuccess: () => {
      toast.success('Created!');
      queryClient.invalidateQueries({ queryKey: ['cabins'] });
      // Maintenant la liste se refetch ! ✅
    },
  });
}
```

### 4. Enabled Query qui Fetch quand même

```javascript
// ❌ PIÈGE: enabled avec valeur falsy mais pas false
function BadEnabled({ userId }) {
  const { data } = useQuery({
    queryKey: ['user', userId],
    queryFn: () => fetchUser(userId),
    enabled: userId, // Si userId = 0 → enabled = false (BUG!)
  });
}

// ✅ SOLUTION: Conversion explicite en boolean
function GoodEnabled({ userId }) {
  const { data } = useQuery({
    queryKey: ['user', userId],
    queryFn: () => fetchUser(userId),
    enabled: userId !== undefined && userId !== null,
    // Ou: enabled: !!userId && userId !== 0
  });
}
```

### 5. Race Condition dans useEffect

```javascript
// ❌ PIÈGE: Multiple fetchs si query key change rapidement
function BadSearch() {
  const [search, setSearch] = useState('');
  
  const { data } = useQuery({
    queryKey: ['search', search],
    queryFn: () => searchCabins(search),
  });
  
  // User tape rapidement "cabin"
  // → 5 queries envoyées (c, ca, cab, cabi, cabin)
  // → Dernière à arriver gagne (peut être "cabi" pas "cabin"!) 💥
}

// ✅ SOLUTION 1: Debounce
function GoodSearch1() {
  const [search, setSearch] = useState('');
  const debouncedSearch = useDebounce(search, 500);
  
  const { data } = useQuery({
    queryKey: ['search', debouncedSearch],
    queryFn: () => searchCabins(debouncedSearch),
  });
  // Fetch seulement 500ms après arrêt de typing
}

// ✅ SOLUTION 2: enabled minimum length
function GoodSearch2() {
  const [search, setSearch] = useState('');
  
  const { data } = useQuery({
    queryKey: ['search', search],
    queryFn: () => searchCabins(search),
    enabled: search.length >= 3, // Fetch seulement si 3+ chars
  });
}
```

### 6. Stale Closure dans Callbacks

```javascript
// ❌ PIÈGE: Variables capturées dans closure
function BadClosure() {
  const [count, setCount] = useState(0);
  
  const { mutate } = useMutation({
    mutationFn: createItem,
    onSuccess: () => {
      console.log(count); // TOUJOURS 0 ! (stale closure)
      setCount(count + 1); // BUG !
    },
  });
}

// ✅ SOLUTION 1: Functional update
function GoodClosure1() {
  const [count, setCount] = useState(0);
  
  const { mutate } = useMutation({
    mutationFn: createItem,
    onSuccess: () => {
      setCount(c => c + 1); // Utilise valeur actuelle
    },
  });
}

// ✅ SOLUTION 2: useRef pour valeurs mutables
function GoodClosure2() {
  const [count, setCount] = useState(0);
  const countRef = useRef(count);
  countRef.current = count;
  
  const { mutate } = useMutation({
    mutationFn: createItem,
    onSuccess: () => {
      console.log(countRef.current); // Toujours à jour !
    },
  });
}
```

### 7. QueryClient Non-Accessible dans Mutation

```javascript
// ❌ PIÈGE: Essayer d'accéder queryClient en dehors du Provider
const { mutate } = useMutation({
  mutationFn: createCabin,
  onSuccess: () => {
    // queryClient n'existe pas ici !
  },
});

// ✅ SOLUTION: useQueryClient hook
function GoodMutation() {
  const queryClient = useQueryClient(); // ✅ Hook pour accéder au client
  
  const { mutate } = useMutation({
    mutationFn: createCabin,
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ['cabins'] });
    },
  });
}
```

### 8. Over-fetching (Trop de Refetch)

```javascript
// ❌ PIÈGE: Refetch à chaque window focus
function BadRefetch() {
  const { data } = useQuery({
    queryKey: ['heavy-data'],
    queryFn: fetchHeavyData,
    // refetchOnWindowFocus: true (défaut)
  });
  
  // User switch tabs → revient → REFETCH (lourd !)
  // User click autre fenêtre → revient → REFETCH
  // Trop de requests ! 💸
}

// ✅ SOLUTION: Adapter selon le type de data
function GoodRefetch() {
  const { data } = useQuery({
    queryKey: ['settings'],
    queryFn: fetchSettings,
    staleTime: 30 * 60 * 1000, // 30 min
    refetchOnWindowFocus: false, // Data statique
    refetchOnMount: false,
  });
}
```

---

## 🎤 Questions d'Entretien Approfondies

### Q1: "Explique la différence entre staleTime et cacheTime avec un exemple concret"

**Réponse experte :**

```
staleTime et cacheTime contrôlent deux aspects différents du lifecycle d'une query.

**staleTime** = Fraîcheur des données
- Durée pendant laquelle les données sont considérées "fresh" (fraîches)
- Pendant staleTime, React Query ne refetch PAS automatiquement
- Par défaut: 0 (data immédiatement stale)

**cacheTime** = Durée de vie en cache
- Combien de temps garder les données en MÉMOIRE après qu'elles ne soient plus utilisées
- Par défaut: 5 minutes
- Après cacheTime, data supprimée de la mémoire

**Exemple Timeline:**
T0: Component mount → fetch data → data en cache (fresh)
T1 (staleTime): Data devient "stale" mais TOUJOURS en cache
T2: Component unmount → timer cacheTime démarre
T3 (< cacheTime): Component remount → data encore là! Display instant
T4 (> cacheTime): Data supprimée du cache → prochain mount = fetch

**Stratégie par type:**
- Real-time data: staleTime: 0, cacheTime: 1000
- User data: staleTime: 5min, cacheTime: 10min  
- Static data: staleTime: Infinity, cacheTime: Infinity
```

### Q2: "Comment gères-tu un DELETE avec optimistic update et rollback en cas d'erreur ?"

**Réponse avec code :**

```javascript
const { mutate: deleteCabin } = useMutation({
  mutationFn: (id) => api.deleteCabin(id),
  
  // 1. AVANT delete (optimistic)
  onMutate: async (deletedId) => {
    // Cancel queries en cours
    await queryClient.cancelQueries({ queryKey: ['cabins'] });
    
    // Snapshot pour rollback
    const previousCabins = queryClient.getQueryData(['cabins']);
    
    // Update optimiste (remove immédiatement de l'UI)
    queryClient.setQueryData(['cabins'], (old) =>
      old.filter(cabin => cabin.id !== deletedId)
    );
    
    return { previousCabins };
  },
  
  // 2. Si ERREUR (rollback)
  onError: (error, deletedId, context) => {
    queryClient.setQueryData(['cabins'], context.previousCabins);
    toast.error('Failed to delete cabin');
  },
  
  // 3. TOUJOURS (refetch pour sync)
  onSettled: () => {
    queryClient.invalidateQueries({ queryKey: ['cabins'] });
  },
  
  onSuccess: () => {
    toast.success('Cabin deleted');
  },
});

// L'UI se met à jour INSTANTANÉMENT (optimistic)
// Si erreur réseau → rollback automatique
// Si succès → refetch pour sync avec serveur
```

### Q3: "Pourquoi utiliser React Query plutôt que Redux pour le server state ?"

**Réponse structurée :**

```
React Query et Redux résolvent des problèmes DIFFÉRENTS:

**Redux = Client State Management**
- UI state (modals, toggles, theme)
- Application state (current user, navigation)
- State que VOUS contrôlez complètement

**React Query = Server State Management**
- Data venant d'APIs/Databases
- Partagée entre utilisateurs
- Peut changer sans que vous le sachiez
- Nécessite synchronisation

**Pourquoi React Query pour server state:**

1. **Caching automatique**
   - Redux: Vous devez tout gérer manuellement
   - RQ: Cache intelligent avec staleTime/cacheTime

2. **Background refetching**
   - Redux: Pas de sync automatique
   - RQ: Refetch on window focus, interval, etc.

3. **Request deduplication**
   - Redux: Multiple components → multiple fetchs
   - RQ: Même query key → 1 seul fetch

4. **Optimistic updates**
   - Redux: Code complexe à écrire
   - RQ: onMutate built-in

5. **Loading/Error states**
   - Redux: Actions loading/success/error manuelles
   - RQ: isLoading, isError automatiques

6. **Less boilerplate**
   - Redux: Actions, reducers, thunks, selectors
   - RQ: useQuery + useMutation

**Utilisation combinée (common pattern):**
- Redux: Auth state, theme, UI preferences
- React Query: Toutes les data d'API

Dans Wild Oasis, on utilise React Query pour TOUT le server state
(cabins, bookings, guests) et pas de Redux car pas besoin de
complex client state.
```

### Q4: "Comment implémenter une infinite scroll list avec React Query ?"

**Réponse avec code complet :**

```javascript
import { useInfiniteQuery } from '@tanstack/react-query';
import { useInView } from 'react-intersection-observer';

function InfiniteBookingsList() {
  // Hook pour détecter quand sentinel est visible
  const { ref, inView } = useInView();
  
  const {
    data,
    fetchNextPage,
    hasNextPage,
    isFetchingNextPage,
    isLoading,
  } = useInfiniteQuery({
    queryKey: ['bookings-infinite'],
    
    queryFn: async ({ pageParam = 0 }) => {
      // pageParam = cursor pour la page à fetch
      const res = await fetch(`/api/bookings?cursor=${pageParam}`);
      return res.json();
      // Doit retourner: { bookings: [...], nextCursor: 123 }
    },
    
    getNextPageParam: (lastPage) => {
      // Si lastPage a nextCursor → return it
      // Sinon → return undefined (plus de pages)
      return lastPage.nextCursor ?? undefined;
    },
    
    // Optional: prefetch première page
    initialPageParam: 0,
  });
  
  // Fetch automatiquement quand sentinel visible
  useEffect(() => {
    if (inView && hasNextPage && !isFetchingNextPage) {
      fetchNextPage();
    }
  }, [inView, hasNextPage, isFetchingNextPage, fetchNextPage]);
  
  // Flatten toutes les pages
  const allBookings = data?.pages.flatMap(page => page.bookings) ?? [];
  
  if (isLoading) return <Spinner />;
  
  return (
    <div>
      {allBookings.map(booking => (
        <BookingCard key={booking.id} booking={booking} />
      ))}
      
      {/* Sentinel - trigger fetch quand visible */}
      {hasNextPage && (
        <div ref={ref} style={{ height: 50, margin: 20 }}>
          {isFetchingNextPage ? <Spinner /> : 'Load more...'}
        </div>
      )}
      
      {!hasNextPage && <div>No more bookings</div>}
    </div>
  );
}

// API endpoint structure (backend):
// GET /api/bookings?cursor=0
// {
//   bookings: [...10 items...],
//   nextCursor: 10
// }
// GET /api/bookings?cursor=10
// {
//   bookings: [...10 items...],
//   nextCursor: 20
// }
```

### Q5: "Comment débugger une query qui ne fetch pas alors qu'elle devrait ?"

**Checklist de debugging :**

```javascript
// 1. Vérifier que QueryClientProvider existe
// Chercher dans React DevTools

// 2. Vérifier enabled
const { data } = useQuery({
  queryKey: ['data'],
  queryFn: fetchData,
  enabled: condition, // ← Vérifier cette valeur !
});
console.log('Query enabled?', condition);

// 3. Vérifier staleTime
const { data } = useQuery({
  queryKey: ['data'],
  queryFn: fetchData,
  staleTime: Infinity, // ← Data JAMAIS stale = jamais refetch !
});

// 4. Vérifier query key stabilité
// Query key change à chaque render ?
useQuery({
  queryKey: ['data', { filter: 'active' }], // ← Nouveau object !
  // Utiliser useMemo ou useState
});

// 5. Utiliser React Query DevTools
import { ReactQueryDevtools } from '@tanstack/react-query-devtools';

<ReactQueryDevtools initialIsOpen={false} />

// DevTools montrent:
// - Toutes les queries actives
// - Leur état (loading, success, error, stale)
// - Leur data
// - Combien de observers
// - Dernière update

// 6. Logger dans queryFn
queryFn: async () => {
  console.log('🔥 Query function called!');
  const data = await fetchData();
  console.log('📦 Data fetched:', data);
  return data;
}

// 7. Vérifier erreurs silencieuses
onError: (error) => {
  console.error('Query error:', error);
}
```

### Q6: "Quelle est la meilleure stratégie pour gérer l'authentification avec React Query ?"

**Pattern complet :**

```javascript
// hooks/useUser.js
export function useUser() {
  const { data: user, isLoading } = useQuery({
    queryKey: ['user'],
    queryFn: async () => {
      const { data: { user } } = await supabase.auth.getUser();
      return user;
    },
    staleTime: Infinity, // User change rarement
    retry: false, // Pas de retry si not authenticated
  });
  
  return { 
    user, 
    isLoading, 
    isAuthenticated: user?.role === 'authenticated' 
  };
}

// hooks/useLogin.js
export function useLogin() {
  const queryClient = useQueryClient();
  const navigate = useNavigate();
  
  const { mutate: login, isLoading } = useMutation({
    mutationFn: ({ email, password }) =>
      supabase.auth.signInWithPassword({ email, password }),
    
    onSuccess: ({ data: { user } }) => {
      // Set user dans le cache
      queryClient.setQueryData(['user'], user);
      navigate('/dashboard');
    },
    
    onError: (error) => {
      toast.error(error.message);
    },
  });
  
  return { login, isLoading };
}

// hooks/useLogout.js
export function useLogout() {
  const queryClient = useQueryClient();
  const navigate = useNavigate();
  
  const { mutate: logout, isLoading } = useMutation({
    mutationFn: () => supabase.auth.signOut(),
    
    onSuccess: () => {
      // Clear TOUT le cache
      queryClient.clear();
      // Ou seulement user-related data:
      // queryClient.removeQueries();
      navigate('/login');
    },
  });
  
  return { logout, isLoading };
}

// components/ProtectedRoute.jsx
function ProtectedRoute({ children }) {
  const { user, isLoading, isAuthenticated } = useUser();
  const navigate = useNavigate();
  
  useEffect(() => {
    if (!isAuthenticated && !isLoading) {
      navigate('/login');
    }
  }, [isAuthenticated, isLoading, navigate]);
  
  if (isLoading) return <FullPageSpinner />;
  
  if (isAuthenticated) return children;
  
  return null;
}

// App.jsx
function App() {
  return (
    <Routes>
      <Route path="/login" element={<Login />} />
      
      <Route element={<ProtectedRoute><AppLayout /></ProtectedRoute>}>
        <Route path="/dashboard" element={<Dashboard />} />
        <Route path="/cabins" element={<Cabins />} />
        {/* ... */}
      </Route>
    </Routes>
  );
}
```

### Q7: "Comment optimiser les performances d'une app avec beaucoup de queries ?"

**Stratégies d'optimisation :**

```javascript
// 1. STALE TIME approprié (éviter over-fetching)
const { data } = useQuery({
  queryKey: ['static-data'],
  queryFn: fetchStaticData,
  staleTime: 30 * 60 * 1000, // 30 min pour data statique
  // Au lieu de 0 (défaut) qui refetch tout le temps
});

// 2. SELECT pour transformer data (éviter re-renders)
const { data: activeBookings } = useQuery({
  queryKey: ['bookings'],
  queryFn: fetchBookings,
  select: (data) => data.filter(b => b.status === 'active'),
  // select est memoized ! Pas de re-compute si data identique
});

// 3. PLACEHOLDER DATA (UI instant)
const { data: cabin } = useQuery({
  queryKey: ['cabin', cabinId],
  queryFn: () => fetchCabin(cabinId),
  placeholderData: () => {
    // Chercher dans cache des cabins
    const cabins = queryClient.getQueryData(['cabins']);
    return cabins?.find(c => c.id === cabinId);
  },
  // UI affiche placeholder pendant fetch (pas de loading)
});

// 4. PREFETCHING (anticipate user actions)
function CabinsList() {
  const queryClient = useQueryClient();
  
  return cabins.map(cabin => (
    <CabinCard
      cabin={cabin}
      onMouseEnter={() => {
        // Prefetch detail avant que user clique
        queryClient.prefetchQuery({
          queryKey: ['cabin', cabin.id],
          queryFn: () => fetchCabin(cabin.id),
        });
      }}
    />
  ));
}

// 5. STRUCTURAL SHARING (automatique mais bon à savoir)
// React Query compare old vs new data
// Si identique → garde old reference → pas de re-render
// Marche avec objects/arrays imbriqués !

// 6. QUERIES PARALLÈLES groupées
// Au lieu de:
useQuery(['data1'], fetch1);
useQuery(['data2'], fetch2);
useQuery(['data3'], fetch3);
// → 3 requests séparés

// Faire:
useQuery(['dashboard-data'], async () => {
  const [data1, data2, data3] = await Promise.all([
    fetch1(),
    fetch2(),
    fetch3(),
  ]);
  return { data1, data2, data3 };
});
// → 1 seul useQuery, Promise.all pour parallélisme

// 7. DISABLE auto-refetch pour heavy queries
const { data } = useQuery({
  queryKey: ['heavy-report'],
  queryFn: fetchHeavyReport,
  refetchOnWindowFocus: false,
  refetchOnMount: false,
  refetchOnReconnect: false,
  staleTime: Infinity,
});
```

---

## 💡 Best Practices - Résumé Expert

### 1. Structure de Code

```
✅ Séparer concerns:
   - services/ → API calls pures
   - hooks/ → React Query hooks
   - components/ → UI only

✅ Un hook par operation:
   - useCabins (query)
   - useCreateCabin (mutation)
   - useUpdateCabin (mutation)
   - useDeleteCabin (mutation)

✅ Naming convention:
   - useResource pour GET
   - useCreateResource pour POST
   - useUpdateResource pour PUT/PATCH
   - useDeleteResource pour DELETE
```

### 2. Query Keys

```javascript
✅ Hierarchical structure:
   ['cabins']
   ['cabins', 'detail', id]
   ['cabins', 'stats']
   ['cabins', { filter, sort, page }]

✅ Consistent naming:
   Toujours même ordre: ['resource', 'modifier', id, params]

✅ Include dependencies:
   Si cabinId change → query key change → refetch auto
```

### 3. Cache Strategy

```javascript
✅ Adapter staleTime selon data:
   - Real-time: 0
   - Frequent updates: 30s - 5min
   - Moderate: 5min - 30min
   - Static: Infinity

✅ Invalider après mutations:
   TOUJOURS queryClient.invalidateQueries après CREATE/UPDATE/DELETE

✅ Optimistic updates pour UX:
   Actions rapides (like, status change) → optimistic
   Actions complexes (create with upload) → invalidate
```

### 4. Error Handling

```javascript
✅ Global error handling:
   defaultOptions.queries.onError dans QueryClient

✅ Specific error handling:
   onError dans chaque mutation critique

✅ User-friendly messages:
   toast.error avec messages clairs
```

### 5. Performance

```javascript
✅ Utiliser select pour derived state
✅ Prefetch données probables
✅ staleTime adapté (pas toujours 0)
✅ Pagination avec keepPreviousData
✅ Lazy loading avec enabled
```

---

## 🔗 Liens avec Vos Projets

### Wild Oasis

```
✅ Pattern utilisé massivement:
   - Toutes les resources (cabins, bookings, guests, settings)
   - Custom hooks pour chaque operation
   - Invalidation systématique après mutations
   - Optimistic updates pour check-in/out
   - React Query DevTools pour debugging

✅ Exemples concrets du cours:
   - useCabins, useCreateCabin, useDeleteCabin
   - useBookings avec pagination
   - useSettings avec staleTime long
   - Supabase + React Query integration
```

### Fast React Pizza

```
✅ React Query pour:
   - Fetching menu (queries)
   - Creating orders (mutations)
   - Order tracking avec polling
   - Geolocation caching

✅ Patterns:
   - useMenu avec long staleTime (menu change rarement)
   - useCreateOrder avec optimistic update
   - useOrder avec refetchInterval (tracking)
```

### Worldwise

```
✅ React Query pourrait améliorer:
   - Fetching cities list (actuellement useEffect)
   - Creating/deleting cities (mutations)
   - Caching cities par country
   - Prefetching city details au hover
```

---

**🎯 Points Clés à Retenir pour l'Entretien:**

1. **React Query gère le SERVER STATE, pas le client state**
2. **staleTime ≠ cacheTime** (comprendre la différence !)
3. **Query key = identifiant unique** (changes → refetch)
4. **Toujours invalider après mutations**
5. **Optimistic updates pour UX rapide**
6. **Custom hooks pour réutilisabilité**
7. **DevTools pour debugging**

---