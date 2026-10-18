# Questions / Réponses — Entretien React & Next.js

> Les réponses sont formulées pour être dites à l'oral. Les termes techniques restent en anglais.

---

## 📦 React — Fondamentaux

**Q1 : C'est quoi React et pourquoi on l'utilise ?**
> React c'est une library JavaScript créée par Meta pour construire des interfaces utilisateur. On l'utilise parce que c'est basé sur des components réutilisables, il gère le DOM efficacement via le virtual DOM, et il a un écosystème énorme. C'est déclaratif : on décrit ce qu'on veut afficher en fonction d'un state, et React s'occupe de mettre à jour le DOM.

---

**Q2 : C'est quoi le virtual DOM ?**
> Le virtual DOM c'est une représentation légère du DOM réel, stockée en mémoire. Quand le state change, React recalcule le virtual DOM, le compare à l'ancienne version via un algorithme de diffing, et met à jour uniquement les parties du DOM réel qui ont changé. Ça évite des manipulations DOM coûteuses.

---

**Q3 : C'est quoi JSX ?**
> JSX c'est une extension de syntaxe JavaScript qui ressemble à du HTML. Ça permet d'écrire la structure de son interface directement dans le JavaScript. C'est transpilé par Babel en appels `React.createElement()`. C'est pas obligatoire mais c'est quasi universel parce que c'est bien plus lisible.

---

**Q4 : C'est quoi la différence entre un component fonctionnel et un class component ?**
> Les class components utilisent la syntaxe ES6 avec `extends React.Component` et ont un cycle de vie explicite (`componentDidMount`, etc.). Les functional components sont de simples fonctions JavaScript. Depuis les Hooks en React 16.8, les functional components peuvent gérer le state et les effets de bord, donc aujourd'hui on préfère presque toujours les functional components : c'est plus simple, plus lisible, et plus facile à tester.

---

**Q5 : C'est quoi le one-way data flow ?**
> En React, les données circulent dans une seule direction : du parent vers l'enfant via les props. Un enfant ne peut pas modifier directement les props qu'il reçoit. Pour communiquer vers le haut (enfant vers parent), le parent passe une fonction callback en prop que l'enfant peut appeler. Ce flux unidirectionnel rend l'application plus prévisible et plus facile à débugger.

---

**Q6 : Pourquoi on utilise `className` au lieu de `class` en JSX ?**
> Parce que `class` est un mot réservé en JavaScript. JSX est du JavaScript, donc pour éviter les conflits, React utilise `className` pour l'attribut HTML `class`, et `htmlFor` à la place de `for` sur les labels.

---

**Q7 : Pourquoi on a besoin d'une `key` quand on rend une liste ?**
> La `key` aide React à identifier quel élément a changé, a été ajouté ou supprimé dans une liste. Sans `key` unique et stable, React doit reconstruire toute la liste à chaque render, ce qui est inefficace. Avec une bonne `key`, il peut juste mettre à jour l'élément concerné. On évite d'utiliser l'index comme `key` si la liste peut être réordonnée.

---

## 🪝 React — Hooks

**Q8 : C'est quoi `useState` ?**
> `useState` c'est un hook qui permet d'ajouter du state local à un functional component. Il retourne un tableau avec deux éléments : la valeur actuelle du state, et une fonction pour le mettre à jour. Quand on appelle le setter, React re-rend le component avec la nouvelle valeur. Exemple : `const [count, setCount] = useState(0)`.

---

**Q9 : C'est quoi `useEffect` ?**
> `useEffect` c'est le hook pour gérer les effets de bord : fetch de données, subscriptions, manipulation directe du DOM, timers. Il prend une fonction et un tableau de dépendances. Si le tableau est vide, l'effet tourne une seule fois après le premier render. Si on passe des valeurs, il re-tourne quand elles changent. On peut retourner une fonction de cleanup pour éviter les memory leaks.

---

**Q10 : C'est quoi la différence entre `useEffect` et `useLayoutEffect` ?**
> `useEffect` s'exécute de manière asynchrone après que le browser a peint le DOM. `useLayoutEffect` s'exécute de manière synchrone après les mutations du DOM mais avant que le browser peigne. On utilise `useLayoutEffect` quand on a besoin de lire la taille ou la position d'éléments DOM et de les modifier avant que l'utilisateur voit quoi que ce soit, pour éviter un flash visuel.

---

**Q11 : Comment fonctionne `useCallback` ?**
> `useCallback` memoïze une fonction entre les renders. Il retourne la même instance de fonction tant que les dépendances n'ont pas changé. C'est utile quand on passe une callback à un component enfant mémoïsé avec `React.memo` : sans `useCallback`, une nouvelle fonction serait créée à chaque render du parent, ce qui invaliderait la mémoïsation de l'enfant.

---

**Q12 : C'est quoi `useMemo` et quand l'utiliser ?**
> `useMemo` memoïze le résultat d'un calcul. Il recalcule seulement quand les dépendances changent. C'est utile pour des calculs coûteux sur de grandes listes ou des transformations de données complexes. Attention : `useMemo` a un coût en lui-même, donc on l'utilise uniquement quand on a un vrai problème de performance mesuré, pas par défaut.

---

**Q13 : C'est quoi `useRef` ?**
> `useRef` retourne un objet mutable avec une propriété `current`. Deux usages principaux : accéder directement à un élément DOM (pour focus, scroll, mesures), et stocker une valeur qui persiste entre les renders sans déclencher de re-render. C'est différent du state : modifier `.current` ne provoque pas de re-render.

---

**Q14 : Comment créer un custom hook et pourquoi ?**
> Un custom hook c'est une fonction JavaScript dont le nom commence par `use` et qui peut appeler d'autres hooks. On en crée pour extraire et réutiliser de la logique stateful entre plusieurs components. Par exemple `useFetch`, `useLocalStorage`, `useDebounce`. Ça permet de garder les components propres et de factoriser la logique.

---

**Q15 : Quelles sont les règles des Hooks ?**
> Il y a deux règles fondamentales. Première : appeler les Hooks uniquement au top level du component, jamais dans des conditions, boucles ou fonctions imbriquées — React s'appuie sur l'ordre d'appel pour associer chaque Hook à son state. Deuxième : appeler les Hooks uniquement dans des functional components React ou dans des custom hooks, pas dans des fonctions JavaScript ordinaires.

---

**Q16 : C'est quoi `useContext` ?**
> `useContext` permet de consommer un contexte React sans wrapper le component dans un Consumer. On lui passe le contexte créé avec `createContext` et il retourne la valeur courante de ce contexte. Le component se re-rend automatiquement quand la valeur du contexte change.

---

**Q17 : C'est quoi `useReducer` et quand le préférer à `useState` ?**
> `useReducer` est une alternative à `useState` pour du state complexe. Il prend un reducer (une fonction pure `(state, action) => newState`) et un état initial, et retourne le state courant et une fonction `dispatch`. On le préfère à `useState` quand la logique de mise à jour est complexe, quand le prochain state dépend du précédent, ou quand plusieurs morceaux de state évoluent ensemble.

---

## ⚡ React — Performance

**Q18 : C'est quoi `React.memo` ?**
> `React.memo` est un higher-order component qui memoïze un component fonctionnel. Il évite les re-renders inutiles si les props n'ont pas changé. Par défaut il fait une comparaison shallow des props. On peut passer une fonction de comparaison custom en second argument. À utiliser quand un component se re-rend fréquemment avec les mêmes props, pas systématiquement.

---

**Q19 : C'est quoi le code splitting et comment l'implémenter ?**
> Le code splitting c'est diviser le bundle JavaScript en plusieurs chunks qui sont chargés à la demande. En React : `React.lazy(() => import('./MonComponent'))` pour un import dynamique, wrappé dans `Suspense` pour afficher un fallback pendant le chargement. React Router supporte nativement le lazy loading des routes. Webpack et Vite gèrent le split automatiquement pour les imports dynamiques.

---

**Q20 : C'est quoi la virtualisation de listes et quand en avoir besoin ?**
> La virtualisation consiste à ne rendre dans le DOM que les éléments visibles à l'écran plutôt que toute la liste. Avec 10 000 éléments, on ne rend que les 20-30 visibles. Librairies : `react-window` (légère), `TanStack Virtual` (plus flexible). On en a besoin quand une liste dépasse 200-300 éléments et que les performances se dégradent.

---

**Q21 : Comment profiler une app React ?**
> On commence toujours par mesurer, jamais par optimiser à l'aveugle. Outils : React DevTools Profiler pour voir quels components se re-rendent et pourquoi, Chrome Performance tab. Ensuite on applique les optimisations ciblées : `React.memo`, `useMemo`, `useCallback`, virtualisation, code splitting.

---

## 🔄 React — Concurrent Mode (React 18+)

**Q22 : C'est quoi le Concurrent Mode ?**
> Le Concurrent Mode c'est la capacité de React à préparer plusieurs versions de l'UI simultanément et à interrompre, mettre en pause, ou abandonner un render si quelque chose de plus urgent arrive. Ça permet des interfaces plus fluides sans sacrifier la réactivité aux inputs utilisateur.

---

**Q23 : C'est quoi `useTransition` ?**
> `useTransition` retourne `[isPending, startTransition]`. On wrape les mises à jour de state non-urgentes dans `startTransition` : React peut les interrompre si l'utilisateur fait une action plus urgente. `isPending` est `true` pendant que la transition est en cours, ce qui permet d'afficher un indicateur de chargement. C'est idéal pour des filtres sur de grandes listes ou des navigations vers des pages lourdes.

---

**Q24 : C'est quoi `useDeferredValue` ?**
> `useDeferredValue` accepte une valeur et retourne une version différée de celle-ci. React met à jour la valeur urgente immédiatement, et programme la mise à jour de la valeur différée avec une priorité basse. À la différence de `useTransition` qui wrape le setter, `useDeferredValue` wrape la valeur elle-même. Utile quand on n'a pas accès au setter (par exemple pour différer une prop reçue).

---

**Q25 : C'est quoi `Suspense` ?**
> `Suspense` permet à un component de signaler qu'il attend quelque chose (données, code), et d'afficher un fallback en attendant. Pour le code splitting, `React.lazy` + `Suspense` affiche un loader pendant que le chunk se charge. Pour les données en React 18+, avec des librairies compatibles comme React Query ou Relay, `Suspense` gère l'état de chargement de manière déclarative.

---

## 🗂️ React — State Management

**Q26 : C'est quoi le Context API et quand l'utiliser ?**
> Le Context API permet de passer des données à travers l'arbre de components sans prop drilling. On crée un contexte avec `createContext`, on wrape les components dans un `Provider` avec une `value`, et les components enfants consomment avec `useContext`. C'est bien pour des données globales comme le thème, la langue, l'utilisateur connecté. Pour de la gestion d'état complexe, on préfère souvent Redux ou Zustand.

---

**Q27 : C'est quoi le prop drilling et comment l'éviter ?**
> Le prop drilling c'est quand on passe des props à travers plusieurs niveaux de components intermédiaires qui n'en ont pas besoin. On l'évite avec le Context API, ou avec un state manager global comme Redux ou Zustand. On peut aussi restructurer ses components pour coloquer le state plus près de là où il est utilisé.

---

**Q28 : Comment choisir entre Redux, Zustand, Jotai, ou le Context API ?**
> Context API pour des données globales simples qui changent rarement. Zustand pour du state global modéré, c'est simple, léger, pas de boilerplate. Jotai ou Recoil pour une approche atomique, pratique pour du state granulaire. Redux Toolkit pour des apps très larges avec beaucoup de logique complexe et des équipes importantes. En 2025, Zustand est souvent le meilleur choix pour la majorité des apps.

---

## 🧪 React — Testing

**Q29 : Comment tester des components React ?**
> La philosophie Testing Library : tester le comportement, pas l'implémentation. On utilise `render()` + `userEvent` pour simuler les interactions utilisateur. On query par role, label, text — comme un utilisateur le ferait. Pour les hooks : `renderHook`. Pour les appels API : mock avec MSW. Pour les tests end-to-end : Playwright ou Cypress.

---

**Q30 : C'est quoi MSW et pourquoi c'est bien pour les tests ?**
> MSW (Mock Service Worker) intercepte les requêtes réseau au niveau du Service Worker dans le browser, ou via un serveur Node.js dans les tests. L'avantage : on mock au niveau réseau, pas au niveau de la fonction `fetch`. Le code de l'app ne change pas, et les tests sont plus proches de la réalité. On peut partager les handlers entre les tests et le dev local.

---

## 🏗️ React — Patterns & Architecture

**Q31 : C'est quoi les Higher-Order Components (HOC) ?**
> Un HOC c'est une fonction qui prend un component en argument et retourne un nouveau component enrichi. Exemples classiques : `withRouter`, `connect` de Redux, `React.memo`. Avec les hooks, on en crée moins directement, mais le concept reste important pour comprendre des librairies. Les custom hooks sont souvent préférables pour de la logique réutilisable car ils évitent le wrapper hell.

---

**Q32 : C'est quoi les render props ?**
> Les render props c'est un pattern où un component accepte une fonction comme `children` ou comme prop `render` qui retourne du JSX. Le component appelle cette fonction et lui passe des données. Ça permettait de partager de la logique avant les hooks. Aujourd'hui c'est largement remplacé par les custom hooks, mais on le voit encore dans certaines librairies.

---

**Q33 : C'est quoi le pattern Compound Components ?**
> Le Compound Components pattern permet de créer des components qui travaillent ensemble et partagent un state implicitement via le Context. Exemple : `<Select>` contient `<Select.Option>`. Le parent gère le state, les enfants y accèdent via context sans prop drilling. C'est utilisé dans des librairies comme Radix UI, Headless UI. C'est idéal pour des composants UI complexes et composables.

---

**Q34 : C'est quoi les React Server Components ?**
> Les React Server Components permettent à des components de tourner exclusivement sur le serveur. Ils peuvent accéder directement aux bases de données, au filesystem, aux APIs backend sans exposer les credentials au client. Ils ne peuvent pas avoir de state ni d'event handlers. Les Client Components restent pour l'interactivité. Le split se fait avec la directive `'use client'`. Next.js App Router est la principale implémentation.

---

**Q35 : Comment gérer les erreurs dans React ?**
> Deux niveaux. Pour les erreurs dans les event handlers et le code async : `try/catch` classique et gérer un state d'erreur. Pour les erreurs de rendering : les Error Boundaries, qui sont des class components qui affichent un fallback si un component enfant crash. La librairie `react-error-boundary` simplifie ça avec une API moderne.

---

**Q36 : Comment gérer les appels API dans React ?**
> Le plus courant : `useEffect` avec `fetch` ou `axios`, en gérant les états `loading`, `error`, `data`. On doit gérer le cleanup dans le `return` de `useEffect` pour éviter des `setState` sur un component démonté. Pour aller plus loin : React Query ou SWR abstraient tout ça avec du caching, de la revalidation, de la deduplication.

---

**Q37 : C'est quoi un controlled vs uncontrolled component dans un form ?**
> Un controlled component a sa valeur gérée par le state React : `onChange` met à jour le state, et `value` est liée au state. Un uncontrolled component laisse le DOM gérer sa propre valeur, qu'on lit via un `ref`. Les controlled components sont préférables car on a un single source of truth. Les uncontrolled sont parfois utiles pour des intégrations avec des librairies tierces.

---

## 🔺 Next.js — Fondamentaux

**Q38 : C'est quoi Next.js et pourquoi l'utiliser par rapport à React seul ?**
> Next.js c'est un framework React qui ajoute des fonctionnalités essentielles pour la production : routing basé sur le système de fichiers, Server-Side Rendering, Static Site Generation, API routes, optimisation d'images, et plein d'autres choses. Avec React seul on doit configurer tout ça manuellement. Next.js donne une structure opinionnée qui accélère le développement et améliore les performances et le SEO.

---

**Q39 : C'est quoi la différence entre le Pages Router et l'App Router dans Next.js ?**
> Le Pages Router c'est l'ancien système : chaque fichier dans `/pages` correspond à une route, avec `getServerSideProps` et `getStaticProps` pour le fetching. L'App Router (Next.js 13+) utilise le dossier `/app`, supporte les React Server Components nativement, les layouts imbriqués, les loading et error states par route. L'App Router est la direction actuelle de Next.js, plus puissant mais avec une courbe d'apprentissage.

---

**Q40 : C'est quoi le Server-Side Rendering (SSR) dans Next.js ?**
> Le SSR génère le HTML côté serveur à chaque requête. Dans l'App Router, tous les components sont Server Components par défaut, donc ils rendent sur le serveur. Dans le Pages Router, on utilisait `getServerSideProps` pour fetcher les données côté serveur avant le render. Le SSR améliore le SEO et le temps de premier affichage (FCP) mais est plus lent que le SSG car il y a un traitement à chaque requête.

---

**Q41 : C'est quoi le Static Site Generation (SSG) ?**
> Le SSG génère le HTML au moment du build, pas à chaque requête. C'est la méthode la plus performante pour des contenus qui changent peu. Dans le Pages Router, c'était `getStaticProps`. Dans l'App Router, un component Server qui ne dépend pas de données dynamiques est statique par défaut. Avec `revalidate`, on peut faire du ISR (Incremental Static Regeneration) : régénérer les pages statiques en arrière-plan selon un intervalle.

---

**Q42 : C'est quoi l'ISR (Incremental Static Regeneration) ?**
> L'ISR c'est le meilleur des deux mondes entre SSG et SSR. On génère les pages statiquement au build, mais on définit un temps de revalidation. Quand une requête arrive après ce délai, Next.js sert l'ancienne page et régénère la nouvelle en arrière-plan. La prochaine requête reçoit la version fraîche. Dans l'App Router, on configure ça avec `export const revalidate = 60` dans le segment de route.

---

**Q43 : Comment fonctionnent les API Routes dans Next.js ?**
> Dans le Pages Router, tout fichier dans `/pages/api/` devient un endpoint HTTP. Dans l'App Router, on crée un fichier `route.ts` dans un dossier du `/app` et on exporte des fonctions nommées selon les méthodes HTTP : `GET`, `POST`, `PUT`, `DELETE`. Ça permet de créer un backend léger directement dans le projet Next.js, sans configurer un serveur séparé.

---

**Q44 : C'est quoi le système de layouts dans l'App Router ?**
> Dans l'App Router, un fichier `layout.tsx` dans un dossier wrape automatiquement toutes les pages de ce dossier et de ses sous-dossiers. Les layouts sont imbriqués : le layout racine (`/app/layout.tsx`) wrape tout, puis chaque segment peut avoir son propre layout. Les layouts persistent entre les navigations, ce qui évite de re-rendre le header ou la sidebar à chaque changement de page.

---

**Q45 : Comment fonctionne le routing dans l'App Router ?**
> C'est du file-based routing : la structure du dossier `/app` définit les URLs. Un dossier `app/dashboard/settings/page.tsx` crée la route `/dashboard/settings`. Les segments dynamiques s'écrivent entre crochets : `[id]` pour `/produits/123`. Les catch-all routes avec `[...slug]`. Les route groups avec `(groupName)` permettent d'organiser sans affecter l'URL. Les fichiers spéciaux sont `page.tsx`, `layout.tsx`, `loading.tsx`, `error.tsx`, `not-found.tsx`.

---

**Q46 : C'est quoi les fichiers spéciaux `loading.tsx` et `error.tsx` dans Next.js ?**
> `loading.tsx` dans un segment de route crée automatiquement un `Suspense` boundary : Next.js affiche ce fichier comme fallback pendant que la page charge. `error.tsx` crée automatiquement un Error Boundary : si la page ou le layout crash, ce component s'affiche à la place. Ces fichiers permettent de gérer les états de chargement et d'erreur de manière déclarative, par segment de route.

---

**Q47 : C'est quoi les Server Actions dans Next.js ?**
> Les Server Actions c'est des fonctions async qui tournent sur le serveur mais qu'on peut appeler directement depuis le client, comme des fonctions normales. On les déclare avec la directive `'use server'`. Elles peuvent mutater des données en base directement sans passer par une API route. On les utilise souvent dans des forms avec l'attribut `action`. Elles s'intègrent avec `revalidatePath` et `revalidateTag` pour invalider le cache après une mutation.

---

**Q48 : Comment fonctionne le caching dans Next.js App Router ?**
> Next.js a plusieurs niveaux de cache. Le Request Memoization : les mêmes requêtes `fetch` pendant un même render sont dédupliquées. Le Data Cache : les résultats des `fetch` sont mis en cache entre les requêtes (persistent). Le Full Route Cache : les pages statiques sont mises en cache sur le serveur au build. Le Router Cache : le client cache les segments de routes visités. On contrôle tout ça avec `cache: 'no-store'`, `next: { revalidate }`, `revalidatePath()`, `revalidateTag()`.

---

**Q49 : Comment optimiser les images dans Next.js ?**
> Next.js fournit le component `<Image>` qui optimise automatiquement les images : conversion en WebP ou AVIF, lazy loading natif, évite le layout shift avec `width` et `height` obligatoires, redimensionnement côté serveur selon l'appareil. On utilise `priority` pour les images above-the-fold (hero images) pour ne pas les lazy loader. Pour les images externes, il faut configurer les domaines autorisés dans `next.config.js`.

---

**Q50 : C'est quoi `next/font` et pourquoi l'utiliser ?**
> `next/font` est le système de gestion de polices de Next.js. Il télécharge les fonts au build time, les héberge en self-hosted, et les sert sans requête réseau vers Google Fonts ou autre. Ça améliore la performance (pas de requête externe), la privacy (pas de tracking Google), et élimine le layout shift dû aux fonts en générant automatiquement une font de substitution optimale.

---

**Q51 : C'est quoi le Middleware dans Next.js ?**
> Le Middleware c'est du code qui s'exécute avant chaque requête, sur l'Edge (entre le client et le serveur). On crée un fichier `middleware.ts` à la racine. C'est idéal pour la gestion d'authentification (rediriger si pas connecté), l'internationalisation (détecter la langue et rediriger), les A/B tests, ou modifier les headers. Il tourne sur le Edge Runtime (V8 isolates), donc très rapide mais avec des APIs limitées (pas de Node.js natif).

---

**Q52 : C'est quoi la différence entre `useRouter`, `usePathname` et `useSearchParams` dans l'App Router ?**
> Dans l'App Router, ces trois hooks viennent de `next/navigation`. `useRouter` donne accès à la navigation programmatique (`push`, `replace`, `back`). `usePathname` retourne le pathname courant en string. `useSearchParams` retourne les query parameters. Ils remplacent l'unique `useRouter` du Pages Router qui faisait tout. Important : ces hooks ne peuvent être utilisés que dans des Client Components.

---

**Q53 : Comment gérer l'authentification dans Next.js ?**
> La solution la plus populaire c'est NextAuth.js (Auth.js en v5). Il supporte OAuth (Google, GitHub, etc.), email/password, credentials custom. Il s'intègre avec le Middleware pour protéger des routes, et avec les Server Components pour récupérer la session. On peut aussi utiliser Clerk ou Supabase Auth pour des solutions plus clés en main. La session est accessible côté serveur avec `getServerSession` et côté client avec `useSession`.

---

**Q54 : C'est quoi la différence entre `generateStaticParams` et `getStaticPaths` ?**
> `getStaticPaths` c'est l'ancienne API du Pages Router pour générer les chemins statiques pour des routes dynamiques. `generateStaticParams` c'est l'équivalent dans l'App Router. La différence principale : `generateStaticParams` peut être défini dans plusieurs layouts/pages de la même route en parallèle, et il est plus efficace car il déduplique automatiquement les fetches identiques lors de la génération.

---

**Q55 : Comment fonctionne l'internationalisation (i18n) dans Next.js ?**
> Dans le Pages Router, Next.js avait un support i18n natif dans `next.config.js` avec `locales` et `defaultLocale`. Dans l'App Router, ce support natif a été retiré au profit de solutions plus flexibles via le Middleware et les route groups. On utilise souvent des librairies comme `next-intl` ou `next-i18next`. Le pattern courant : détecter la locale dans le Middleware, rediriger vers `/fr/...` ou `/en/...`, et utiliser des dossiers `[locale]` dans l'App Router.

---

## 🔒 Sécurité

**Q56 : Comment éviter les failles XSS dans React ?**
> React échappe automatiquement le contenu dans le JSX, ce qui prévient la plupart des attaques XSS. Le risque principal vient de `dangerouslySetInnerHTML` : ne jamais y passer du contenu utilisateur non sanitizé. Utiliser DOMPurify si on doit afficher du HTML riche. Attention aussi aux URLs dans `href` ou `src` : valider les protocoles pour éviter les `javascript:` URLs.

---

**Q57 : C'est quoi les CSRF attacks et comment Next.js les gère ?**
> Une attaque CSRF trompe un utilisateur authentifié pour lui faire envoyer une requête non désirée. Les Server Actions de Next.js incluent une protection CSRF automatique : elles vérifient l'`Origin` header et utilisent un token implicite. Pour les API Routes classiques, il faut implémenter sa propre protection avec des tokens CSRF ou vérifier l'`Origin` manuellement.

---

## 🌐 Général — Écosystème

**Q58 : Quelle est la différence entre React Query et SWR ?**
> Les deux sont des librairies de data fetching avec cache. SWR (de Vercel) est plus léger et simple. React Query (TanStack Query) est plus puissant et configurable : gestion des mutations, des queries infinies, du prefetching, des optimistic updates, du background refetching. Pour la majorité des projets, les deux font le job. React Query est préférable pour des cas complexes avec beaucoup de mutations et de logique de synchronisation.

---

**Q59 : C'est quoi Zustand et pourquoi c'est populaire ?**
> Zustand c'est une petite librairie de state management (environ 1kb). L'API est minimaliste : on crée un store avec `create`, on définit le state et les actions ensemble, et on consomme avec un hook. Pas de Provider à wrapper, pas de boilerplate. Il supporte le middleware (persist, devtools), les slices pour découper le store. C'est populaire parce que c'est simple à apprendre, flexible, et performant.

---

**Q60 : C'est quoi TypeScript et pourquoi l'utiliser avec React ?**
> TypeScript c'est un superset de JavaScript qui ajoute le typage statique. Avec React, ça permet de typer les props des components, les retours de fonctions, les états, les events. Les avantages : détection d'erreurs à la compilation, autocomplétion puissante dans l'IDE, refactoring plus sûr, meilleure documentation du code via les types. C'est devenu quasi standard dans les projets React professionnels. Next.js supporte TypeScript nativement.

---

*Total : 60 questions — React (37) · Next.js (18) · Sécurité (2) · Écosystème (3)*
