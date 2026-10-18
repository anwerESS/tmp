## [**..**](./00_index.md)

# Chapitre 15 : Modules JavaScript - Résumé Détaillé

## 15.1 Système de Modules ES6

Les **modules ES6** (ECMAScript 2015) permettent d'organiser le code en fichiers séparés, avec des exports et imports explicites.

### Export (Named Exports, Default Export)

```javascript
// ========== file: math.js ==========

// NAMED EXPORTS - Exporter plusieurs éléments avec leurs noms

// Export inline (directement à la déclaration)
export const PI = 3.14159;
export const E = 2.71828;

export function add(a, b) {
    return a + b;
}

export function subtract(a, b) {
    return a - b;
}

export class Calculator {
    multiply(a, b) {
        return a * b;
    }
}

// Export en fin de fichier (liste d'exports)
const divide = (a, b) => a / b;
const power = (a, b) => Math.pow(a, b);

export { divide, power };

// Export avec renommage (as)
const privateFunction = () => 'secret';
export { privateFunction as publicFunction };

// ========== file: user.js ==========

// DEFAULT EXPORT - Un seul export par défaut par fichier

// Méthode 1: Export default inline
export default class User {
    constructor(name) {
        this.name = name;
    }
    
    greet() {
        return `Hello, I'm ${this.name}`;
    }
}

// Méthode 2: Définir puis exporter
class Admin {
    constructor(name, level) {
        this.name = name;
        this.level = level;
    }
}

export default Admin;

// Méthode 3: Export default anonyme
export default function(x) {
    return x * x;
}

// ⚠️ On ne peut avoir qu'UN SEUL default export par fichier

// ========== file: utils.js ==========

// COMBINER named exports ET default export
export default function mainUtil() {
    return 'Main utility';
}

export const helper1 = () => 'Helper 1';
export const helper2 = () => 'Helper 2';

// ========== file: constants.js ==========

// Export d'objets
export const config = {
    apiUrl: 'https://api.example.com',
    timeout: 5000
};

// Export de variables multiples
export const STATUS_PENDING = 'pending';
export const STATUS_SUCCESS = 'success';
export const STATUS_ERROR = 'error';

// Ou groupées
const colors = {
    primary: '#007bff',
    secondary: '#6c757d',
    success: '#28a745'
};

export { colors };

// ========== file: data.js ==========

// Export tout d'un objet
const data = {
    users: [],
    posts: [],
    comments: []
};

const fetchUsers = () => { /* ... */ };
const fetchPosts = () => { /* ... */ };

export default data;
export { fetchUsers, fetchPosts };

// ========== Exemples de patterns ==========

// Pattern 1: Exporter classe avec méthodes statiques
export class ApiService {
    static baseUrl = 'https://api.example.com';
    
    static async get(endpoint) {
        const response = await fetch(`${this.baseUrl}${endpoint}`);
        return response.json();
    }
    
    static async post(endpoint, data) {
        const response = await fetch(`${this.baseUrl}${endpoint}`, {
            method: 'POST',
            body: JSON.stringify(data),
            headers: { 'Content-Type': 'application/json' }
        });
        return response.json();
    }
}

// Pattern 2: Factory pattern
function createLogger(name) {
    return {
        log: (msg) => console.log(`[${name}] ${msg}`),
        error: (msg) => console.error(`[${name}] ERROR: ${msg}`)
    };
}

export default createLogger;

// Pattern 3: Exporter constantes groupées
export const HTTP_STATUS = {
    OK: 200,
    CREATED: 201,
    BAD_REQUEST: 400,
    UNAUTHORIZED: 401,
    NOT_FOUND: 404,
    SERVER_ERROR: 500
};

export const ROLES = {
    ADMIN: 'admin',
    USER: 'user',
    GUEST: 'guest'
};

// Pattern 4: Re-export (barrel exports)
// ========== file: components/index.js ==========
export { Button } from './Button';
export { Input } from './Input';
export { Card } from './Card';
export { Modal } from './Modal';

// Ou tout en une ligne
export * from './Button';
export * from './Input';

// Default + named re-export
export { default as Header } from './Header';
export { default as Footer } from './Footer';
```

### Import Syntax

```javascript
// ========== file: app.js ==========

// IMPORT NAMED EXPORTS

// Import spécifiques
import { add, subtract, PI } from './math.js';

console.log(add(5, 3));        // 8
console.log(subtract(10, 4));  // 6
console.log(PI);               // 3.14159

// Import tout avec namespace (*)
import * as math from './math.js';

console.log(math.add(5, 3));      // 8
console.log(math.PI);              // 3.14159
console.log(math.Calculator);      // [class Calculator]

// Import avec renommage (as)
import { add as addition, subtract as subtraction } from './math.js';

console.log(addition(2, 3));      // 5
console.log(subtraction(10, 4));  // 6

// IMPORT DEFAULT EXPORT

// Import default (peut choisir n'importe quel nom)
import User from './user.js';

const user = new User('John');
console.log(user.greet()); // "Hello, I'm John"

// Import default avec nom différent
import MyUser from './user.js'; // Nom libre
import UserClass from './user.js'; // Autre nom

// COMBINER default ET named imports
import mainUtil, { helper1, helper2 } from './utils.js';

console.log(mainUtil());  // "Main utility"
console.log(helper1());   // "Helper 1"
console.log(helper2());   // "Helper 2"

// Import default ET namespace
import Calculator, * as mathUtils from './math.js';

// IMPORT POUR SIDE EFFECTS (sans importer de valeurs)
import './styles.css';  // Charge le CSS
import './polyfills.js'; // Exécute le code

// Import conditionnel n'existe PAS (statique uniquement)
// ❌ if (condition) { import { something } from './module.js'; }

// EXEMPLES PRATIQUES

// 1. Import React
import React from 'react';
import { useState, useEffect } from 'react';

// Ou combiné
import React, { useState, useEffect } from 'react';

// 2. Import composants
import Button from './components/Button';
import { Card, Modal } from './components';

// 3. Import utilities
import { formatDate, formatCurrency } from './utils/format';
import * as validators from './utils/validators';

console.log(validators.isEmail('test@example.com'));

// 4. Import constants
import { API_URL, TIMEOUT } from './config/constants';
import { HTTP_STATUS } from './constants';

console.log(HTTP_STATUS.OK); // 200

// 5. Import depuis node_modules
import axios from 'axios';
import { debounce, throttle } from 'lodash';

// 6. Import types (TypeScript/Flow)
// import type { User } from './types';
// import type { Props } from './types';

// CHEMINS D'IMPORT

// Relatif (même dossier)
import { something } from './module.js';

// Relatif (dossier parent)
import { something } from '../module.js';

// Relatif (plusieurs niveaux)
import { something } from '../../utils/module.js';

// Absolu (depuis node_modules)
import React from 'react';
import axios from 'axios';

// Avec alias (configuré dans webpack/vite)
// import { Button } from '@/components/Button';
// import { formatDate } from '@utils/date';

// ORDRE D'IMPORTS (convention)
// 1. Librairies externes
import React, { useState } from 'react';
import axios from 'axios';

// 2. Imports internes (chemin absolu si configuré)
// import { Button } from '@/components/Button';

// 3. Imports relatifs
import { helper } from './utils';
import styles from './App.module.css';

// IMPORTS GROUPÉS PAR CATÉGORIE
// Components
import Header from './components/Header';
import Footer from './components/Footer';
import Sidebar from './components/Sidebar';

// Hooks
import { useAuth } from './hooks/useAuth';
import { useData } from './hooks/useData';

// Utils
import { formatDate } from './utils/date';
import { apiClient } from './utils/api';

// Constants
import { ROUTES } from './constants/routes';
```

### Renommage (`as`)

```javascript
// ========== EXPORT avec renommage ==========

// file: math.js
const addition = (a, b) => a + b;
const subtraction = (a, b) => a - b;

// Exporter avec autres noms
export { 
    addition as add, 
    subtraction as subtract 
};

// ========== IMPORT avec renommage ==========

// file: app.js

// Éviter conflits de noms
import { add as mathAdd } from './math.js';
import { add as stringAdd } from './stringUtils.js';

console.log(mathAdd(2, 3));      // 5 (addition numérique)
console.log(stringAdd('a', 'b')); // "ab" (concaténation)

// Renommer pour clarté
import { getData as fetchUserData } from './api/users.js';
import { getData as fetchPostData } from './api/posts.js';

// Renommer import default
import UserModel from './models/User.js';
import { default as UserClass } from './models/User.js'; // Même chose

// Combiner renommages
import { 
    add as addition, 
    subtract as subtraction, 
    multiply as multiplication 
} from './math.js';

// Renommer dans re-export
// file: index.js
export { Button as PrimaryButton } from './Button.js';
export { Card as InfoCard } from './Card.js';

// EXEMPLES PRATIQUES

// 1. Éviter collision avec variables globales
import { fetch as customFetch } from './customFetch.js';

// 2. Noms plus descriptifs
import { 
    validate as validateEmail 
} from './emailValidator.js';

import { 
    validate as validatePassword 
} from './passwordValidator.js';

// 3. Abréviations
import { 
    VeryLongComponentName as VLCN 
} from './components/VeryLongComponentName.js';

// 4. Conventions de naming
import { 
    user_service as userService 
} from './user_service.js'; // snake_case → camelCase

// 5. Version compatibility
import { 
    oldFunction as newFunction 
} from './legacy.js';
```

### Import Dynamique (`import()`)

```javascript
// Import dynamique retourne une Promise
// Permet lazy loading et code splitting

// SYNTAXE DE BASE
const loadModule = async () => {
    const module = await import('./math.js');
    console.log(module.add(2, 3)); // 5
};

// Import default
const loadUser = async () => {
    const { default: User } = await import('./user.js');
    const user = new User('John');
    console.log(user.greet());
};

// Import named exports
const loadUtils = async () => {
    const { helper1, helper2 } = await import('./utils.js');
    console.log(helper1());
    console.log(helper2());
};

// Import tout
const loadMath = async () => {
    const math = await import('./math.js');
    console.log(math.add(5, 3));
    console.log(math.PI);
};

// AVEC THEN/CATCH (style Promise)
import('./math.js')
    .then(module => {
        console.log(module.add(2, 3));
    })
    .catch(error => {
        console.error('Failed to load module:', error);
    });

// CAS D'USAGE 1: Lazy loading conditionnel
async function loadFeature(featureName) {
    if (featureName === 'advanced') {
        const { AdvancedFeature } = await import('./features/advanced.js');
        return new AdvancedFeature();
    } else {
        const { BasicFeature } = await import('./features/basic.js');
        return new BasicFeature();
    }
}

// CAS D'USAGE 2: Charger selon configuration
async function loadConfig() {
    const env = process.env.NODE_ENV;
    
    if (env === 'production') {
        return await import('./config/prod.js');
    } else {
        return await import('./config/dev.js');
    }
}

// CAS D'USAGE 3: Route-based code splitting
async function loadPage(route) {
    switch(route) {
        case '/home':
            return await import('./pages/Home.js');
        case '/about':
            return await import('./pages/About.js');
        case '/contact':
            return await import('./pages/Contact.js');
        default:
            return await import('./pages/NotFound.js');
    }
}

// CAS D'USAGE 4: Charger à la demande (user action)
button.addEventListener('click', async () => {
    const { default: Modal } = await import('./components/Modal.js');
    const modal = new Modal();
    modal.show();
});

// CAS D'USAGE 5: Fallback si module échoue
async function loadModuleWithFallback() {
    try {
        return await import('./preferredModule.js');
    } catch (error) {
        console.warn('Preferred module failed, loading fallback');
        return await import('./fallbackModule.js');
    }
}

// REACT: Lazy loading components
// import React, { lazy, Suspense } from 'react';

// const LazyComponent = lazy(() => import('./components/Heavy.js'));

/*
function App() {
    return (
        <Suspense fallback={<div>Loading...</div>}>
            <LazyComponent />
        </Suspense>
    );
}
*/

// Import dynamique avec template literals
async function loadLanguage(lang) {
    const translations = await import(`./locales/${lang}.js`);
    return translations.default;
}

loadLanguage('fr').then(translations => {
    console.log(translations.hello); // "Bonjour"
});

// Import multiple en parallèle
async function loadMultipleModules() {
    const [mathModule, utilsModule, dataModule] = await Promise.all([
        import('./math.js'),
        import('./utils.js'),
        import('./data.js')
    ]);
    
    console.log(mathModule.add(2, 3));
    console.log(utilsModule.helper1());
    console.log(dataModule.default);
}

// Cache des imports dynamiques
const moduleCache = new Map();

async function cachedImport(modulePath) {
    if (moduleCache.has(modulePath)) {
        return moduleCache.get(modulePath);
    }
    
    const module = await import(modulePath);
    moduleCache.set(modulePath, module);
    return module;
}

// Preload (charger à l'avance sans exécuter)
const preloadModules = () => {
    // Charger en arrière-plan
    import('./heavy-module.js');
    import('./another-module.js');
    // Modules sont en cache pour utilisation future
};

// Appeler au chargement de la page
window.addEventListener('load', preloadModules);

// Import avec retry
async function importWithRetry(modulePath, retries = 3) {
    for (let i = 0; i < retries; i++) {
        try {
            return await import(modulePath);
        } catch (error) {
            if (i === retries - 1) throw error;
            await new Promise(resolve => setTimeout(resolve, 1000 * (i + 1)));
        }
    }
}

// Import avec timeout
async function importWithTimeout(modulePath, timeout = 5000) {
    return Promise.race([
        import(modulePath),
        new Promise((_, reject) => 
            setTimeout(() => reject(new Error('Import timeout')), timeout)
        )
    ]);
}
```

---

## 15.2 Organisation du Code

### Structure de Fichiers

```javascript
// STRUCTURE RECOMMANDÉE POUR PROJET

/*
project/
├── src/
│   ├── components/          # Composants UI
│   │   ├── Button/
│   │   │   ├── Button.js
│   │   │   ├── Button.css
│   │   │   └── index.js     # Re-export
│   │   ├── Card/
│   │   │   ├── Card.js
│   │   │   └── index.js
│   │   └── index.js         # Barrel export
│   │
│   ├── hooks/               # Custom hooks React
│   │   ├── useAuth.js
│   │   ├── useData.js
│   │   └── index.js
│   │
│   ├── utils/               # Fonctions utilitaires
│   │   ├── date.js
│   │   ├── string.js
│   │   ├── validation.js
│   │   └── index.js
│   │
│   ├── services/            # API calls, business logic
│   │   ├── api.js
│   │   ├── auth.js
│   │   └── index.js
│   │
│   ├── constants/           # Constantes
│   │   ├── routes.js
│   │   ├── config.js
│   │   └── index.js
│   │
│   ├── types/               # Types TypeScript/Flow
│   │   └── index.js
│   │
│   └── App.js               # Point d'entrée
│
├── public/                  # Fichiers statiques
└── package.json
*/

// ========== components/Button/index.js ==========
// Re-export pour import simplifié
export { default } from './Button';
export * from './Button';

// Permet d'importer ainsi:
// import Button from './components/Button';
// Au lieu de:
// import Button from './components/Button/Button';

// ========== components/index.js (Barrel Export) ==========
// Centraliser tous les exports
export { default as Button } from './Button';
export { default as Card } from './Card';
export { default as Modal } from './Modal';
export { default as Input } from './Input';

// Permet d'importer ainsi:
// import { Button, Card, Modal } from './components';

// ========== utils/index.js ==========
export * from './date';
export * from './string';
export * from './validation';

// Ou avec renommage
export { formatDate, parseDate } from './date';
export { capitalize, truncate } from './string';
export { isEmail, isPhone } from './validation';

// ========== constants/index.js ==========
export * from './routes';
export * from './config';
export * from './http';

// PATTERN: Feature-based structure
/*
features/
├── auth/
│   ├── components/
│   ├── hooks/
│   ├── services/
│   ├── constants/
│   └── index.js
├── dashboard/
│   ├── components/
│   ├── hooks/
│   └── index.js
└── profile/
    ├── components/
    └── index.js
*/

// ========== features/auth/index.js ==========
export { LoginForm } from './components/LoginForm';
export { RegisterForm } from './components/RegisterForm';
export { useAuth } from './hooks/useAuth';
export { authService } from './services/authService';
```

### Bonnes Pratiques

```javascript
// BONNE PRATIQUE 1: Un export par fichier (préféré pour clarté)

// ✅ Bon: math.js exporte plusieurs fonctions utilitaires
export const add = (a, b) => a + b;
export const subtract = (a, b) => a - b;

// ✅ Bon: User.js exporte une classe principale
export default class User {
    constructor(name) {
        this.name = name;
    }
}

// BONNE PRATIQUE 2: Nommer les exports de manière descriptive

// ❌ Mauvais
export const fn = () => { };
export const data = [];

// ✅ Bon
export const validateEmail = () => { };
export const users = [];

// BONNE PRATIQUE 3: Grouper imports par catégorie

// ✅ Bon
// External libraries
import React, { useState, useEffect } from 'react';
import axios from 'axios';

// Internal modules
import { Button } from './components/Button';
import { formatDate } from './utils/date';

// Styles
import './App.css';

// ❌ Mauvais (mélangé)
import './App.css';
import axios from 'axios';
import { Button } from './components/Button';
import React from 'react';

// BONNE PRATIQUE 4: Éviter imports circulaires

// ❌ Mauvais
// file: A.js
import { funcB } from './B.js';
export const funcA = () => funcB();

// file: B.js
import { funcA } from './A.js'; // ❌ Circulaire!
export const funcB = () => funcA();

// ✅ Bon: Extraire dans module commun
// file: utils.js
export const sharedFunc = () => { };

// file: A.js
import { sharedFunc } from './utils.js';
export const funcA = () => sharedFunc();

// file: B.js
import { sharedFunc } from './utils.js';
export const funcB = () => sharedFunc();

// BONNE PRATIQUE 5: Utiliser barrel exports avec parcimonie

// ✅ Bon pour components library
// components/index.js
export { Button } from './Button';
export { Input } from './Input';
export { Card } from './Card';

// ⚠️ Attention: barrel export peut importer code inutile
// Si vous importez UN component, webpack peut bundler TOUS

// Mieux: Import direct si performance critique
import { Button } from './components/Button';
// Au lieu de:
import { Button } from './components';

// BONNE PRATIQUE 6: Extensions de fichiers

// En ES6 modules, extension recommandée
import { something } from './module.js';

// Mais souvent omise avec bundlers (webpack, vite)
import { something } from './module';

// BONNE PRATIQUE 7: Éviter export * si possible

// ❌ Peut causer confusion
export * from './module';

// ✅ Explicite
export { functionA, functionB, classC } from './module';

// BONNE PRATIQUE 8: index.js pour simplifier imports

// Sans index.js
import Button from './components/Button/Button.js';

// Avec index.js dans Button/
import Button from './components/Button';

// BONNE PRATIQUE 9: Constantes en UPPER_CASE

// constants.js
export const API_URL = 'https://api.example.com';
export const MAX_RETRIES = 3;
export const TIMEOUT_MS = 5000;

// BONNE PRATIQUE 10: Types/Interfaces séparés (TypeScript)

// types/user.ts
export interface User {
    id: number;
    name: string;
    email: string;
}

export type UserRole = 'admin' | 'user' | 'guest';

// BONNE PRATIQUE 11: Services avec méthodes groupées

// services/api.js
class ApiService {
    constructor(baseURL) {
        this.baseURL = baseURL;
    }
    
    async get(endpoint) { /* ... */ }
    async post(endpoint, data) { /* ... */ }
    async put(endpoint, data) { /* ... */ }
    async delete(endpoint) { /* ... */ }
}

export default new ApiService('https://api.example.com');

// BONNE PRATIQUE 12: Configuration environnement

// config/index.js
const config = {
    development: {
        apiUrl: 'http://localhost:3000',
        debug: true
    },
    production: {
        apiUrl: 'https://api.example.com',
        debug: false
    }
};

export default config[process.env.NODE_ENV || 'development'];
```

### Module Scope

```javascript
// Chaque module a son propre scope (portée)

// ========== module.js ==========
// Variables de module sont privées par défaut
const privateVar = 'secret';

function privateFunction() {
    return 'private';
}

// Seul ce qui est exporté est accessible
export const publicVar = 'public';

export function publicFunction() {
    // Peut accéder aux variables privées
    return privateVar + ' ' + privateFunction();
}

// ========== app.js ==========
import { publicVar, publicFunction } from './module.js';

console.log(publicVar);      // "public" ✅
console.log(publicFunction()); // "secret private" ✅

// console.log(privateVar);      // ❌ Error: not defined
// console.log(privateFunction()); // ❌ Error: not defined

// MODULE SCOPE vs GLOBAL SCOPE

// Sans modules (script classique)
// <script src="file1.js"></script>
// <script src="file2.js"></script>
// Toutes les variables sont globales!

// file1.js
var x = 10; // Global!

// file2.js
console.log(x); // 10 (accessible)

// Avec modules ES6
// <script type="module" src="file1.js"></script>
// <script type="module" src="file2.js"></script>
// Chaque module a son propre scope

// file1.js (module)
const x = 10; // Module scope!

// file2.js (module)
// console.log(x); // ❌ Error: x is not defined

// Import nécessaire
import { x } from './file1.js'; // Si exporté

// MODULES SONT SINGLETONS

// ========== counter.js ==========
let count = 0;

export function increment() {
    count++;
}

export function getCount() {
    return count;
}

// ========== file1.js ==========
import { increment, getCount } from './counter.js';

increment();
console.log(getCount()); // 1

// ========== file2.js ==========
import { increment, getCount } from './counter.js';

increment();
console.log(getCount()); // 2 (même instance!)

// STRICT MODE automatique
// Tous les modules ES6 sont en strict mode par défaut

// module.js
// Pas besoin de 'use strict';
// Déjà en strict mode!

function test() {
    // x = 10; // ❌ Error: x is not defined
    const x = 10; // ✅ Doit déclarer
}

// THIS dans module = undefined
console.log(this); // undefined (pas window!)

// EXÉCUTION UNIQUE
// Module exécuté une seule fois, même importé plusieurs fois

// ========== init.js ==========
console.log('Module initialized');

export const config = {
    initialized: true
};

// ========== file1.js ==========
import { config } from './init.js';
// Log: "Module initialized"

// ========== file2.js ==========
import { config } from './init.js';
// Pas de log (déjà initialisé)

// Les deux fichiers partagent la même instance de config
```

---

**Points clés à retenir:**
- `export` pour exposer, `import` pour consommer
- Named exports (plusieurs) vs Default export (un seul)
- `import()` dynamique pour lazy loading
- Modules = scope isolé + singleton + strict mode
- Organisation: barrel exports, feature-based
- Bonnes pratiques: éviter circulaires, grouper imports

**Pour React:**
- Component = un module par fichier
- index.js pour simplifier imports
- Lazy loading avec React.lazy()
- Code splitting automatique
- Barrel exports pour component libraries


---

# Questions/Réponses - Chapitre 15 : Modules JavaScript

## Questions Niveau Junior

### Q1: Quelle est la différence entre `export` et `export default` ?

**Réponse:**
```javascript
// EXPORT (named export) - Plusieurs exports avec noms

// ========== math.js ==========
export const add = (a, b) => a + b;
export const subtract = (a, b) => a - b;
export const multiply = (a, b) => a * b;

// Import DOIT utiliser les noms exacts et accolades { }
import { add, subtract } from './math.js';

console.log(add(2, 3));      // 5
console.log(subtract(10, 4)); // 6

// EXPORT DEFAULT - Un seul export par défaut

// ========== user.js ==========
export default class User {
    constructor(name) {
        this.name = name;
    }
}

// Import SANS accolades, nom libre
import User from './user.js';        // ✅
import MyUser from './user.js';      // ✅ Même chose
import AnyName from './user.js';     // ✅ Nom au choix

const user = new User('John');

// DIFFÉRENCES CLÉS

// 1. NOMBRE
// Named export: plusieurs par fichier
export const a = 1;
export const b = 2;
export const c = 3;

// Default export: UN SEUL par fichier
export default function() { }
// export default class User { } // ❌ Error: déjà un default

// 2. SYNTAXE D'IMPORT
// Named: accolades obligatoires + nom exact
import { add, subtract } from './math.js';

// Default: sans accolades + nom libre
import Calculator from './calculator.js';

// 3. RENOMMAGE
// Named: avec 'as'
import { add as addition } from './math.js';

// Default: directement
import MyCalculator from './calculator.js'; // Nom au choix

// COMBINER LES DEUX

// ========== utils.js ==========
// Default export
export default function mainUtil() {
    return 'Main utility';
}

// Named exports
export const helper1 = () => 'Helper 1';
export const helper2 = () => 'Helper 2';

// Import combiné
import mainUtil, { helper1, helper2 } from './utils.js';
//     ↑ default   ↑ named

console.log(mainUtil());  // "Main utility"
console.log(helper1());   // "Helper 1"

// QUAND UTILISER QUOI?

// UTILISER NAMED EXPORT quand:
// - Plusieurs fonctions/constantes utilitaires
// - Bibliothèque avec plusieurs exports
// - Vous voulez forcer un nom spécifique
export const API_URL = 'https://api.com';
export const TIMEOUT = 5000;
export const formatDate = () => { };

// UTILISER DEFAULT EXPORT quand:
// - Une classe principale par fichier
// - Un composant React par fichier
// - Une fonction principale

// Composant React
export default function Button({ label }) {
    return <button>{label}</button>;
}

// Classe principale
export default class ApiService {
    get() { }
    post() { }
}

// ❌ ERREURS COURANTES

// Erreur 1: Oublier accolades pour named import
// import add from './math.js'; // ❌ undefined
import { add } from './math.js'; // ✅

// Erreur 2: Utiliser accolades pour default
// import { User } from './user.js'; // ❌ undefined
import User from './user.js'; // ✅

// Erreur 3: Multiple defaults
export default const a = 1;
// export default const b = 2; // ❌ Error

// PATTERN REACT

// ========== Button.js ==========
// Default export pour le component
export default function Button({ children }) {
    return <button>{children}</button>;
}

// Named exports pour helpers/constants
export const BUTTON_SIZES = {
    small: 'sm',
    medium: 'md',
    large: 'lg'
};

// Import dans autre fichier
import Button, { BUTTON_SIZES } from './Button.js';
```

**Points clés:**
- `export` = named (plusieurs, nom exact)
- `export default` = un seul, nom libre
- Named = `import { name } from`
- Default = `import name from`
- Peut combiner les deux
- React: default pour component, named pour utilities

---

### Q2: Comment importer un module avec `import` ?

**Réponse:**
```javascript
// IMPORT NAMED EXPORTS

// Import spécifiques avec { }
import { add, subtract } from './math.js';

// Import plusieurs
import { user, post, comment } from './data.js';

// Import avec renommage (as)
import { add as addition } from './math.js';

// Import tout avec namespace (*)
import * as math from './math.js';
console.log(math.add(2, 3));  // 5
console.log(math.subtract(10, 4)); // 6

// IMPORT DEFAULT EXPORT

// Sans accolades, nom libre
import User from './user.js';
import MyUser from './user.js'; // Même chose

// IMPORT COMBINÉ (default + named)
import React, { useState, useEffect } from 'react';
//     ↑ default  ↑ named

import Calculator, { add, subtract } from './calculator.js';

// IMPORT POUR SIDE EFFECTS (sans importer valeurs)

// Exécute le module sans importer
import './init.js';        // Exécute code d'initialisation
import './styles.css';     // Charge CSS
import './polyfills.js';   // Charge polyfills

// CHEMINS D'IMPORT

// 1. Relatif (même dossier)
import { helper } from './utils.js';

// 2. Relatif (dossier parent)
import { config } from '../config.js';

// 3. Relatif (plusieurs niveaux)
import { api } from '../../services/api.js';

// 4. Absolu (node_modules)
import React from 'react';
import axios from 'axios';
import { debounce } from 'lodash';

// 5. Avec alias (configuré dans bundler)
// import { Button } from '@/components/Button';
// import { formatDate } from '@utils/date';

// EXEMPLES PRATIQUES

// 1. Import React
import React from 'react';
import { useState, useEffect, useCallback } from 'react';

// Ou combiné
import React, { useState, useEffect } from 'react';

// 2. Import composants
import Header from './components/Header';
import Footer from './components/Footer';
import { Button, Card, Modal } from './components';

// 3. Import utilities
import { formatDate, parseDate } from './utils/date';
import { isEmail, isPhone } from './utils/validation';

// 4. Import constants
import { API_URL, TIMEOUT } from './config';

// 5. Import services
import apiService from './services/api';
import authService from './services/auth';

// 6. Import types (TypeScript)
// import type { User, Post } from './types';

// ORDRE D'IMPORTS (convention)

// 1. External libraries (node_modules)
import React, { useState } from 'react';
import axios from 'axios';
import { debounce } from 'lodash';

// 2. Internal absolute imports (si configuré)
// import { Button } from '@/components/Button';

// 3. Internal relative imports
import { useAuth } from './hooks/useAuth';
import { formatDate } from './utils/date';

// 4. Styles
import './App.css';

// IMPORT DEFAULT AS NAMED

// Si module exporte default mais vous voulez nom spécifique
import { default as UserClass } from './user.js';

// IMPORT TOUT + DEFAULT

import Calculator, * as math from './calculator.js';
//     ↑ default    ↑ tout le reste

// ❌ ERREURS COURANTES

// Erreur 1: Oublier extension (sans bundler)
// import { add } from './math'; // ❌ Peut ne pas fonctionner
import { add } from './math.js'; // ✅

// Erreur 2: Mauvais chemin
// import { add } from 'math.js'; // ❌ Cherche dans node_modules
import { add } from './math.js'; // ✅ Relatif

// Erreur 3: Import dans if/loop (pas permis!)
// if (condition) {
//     import { something } from './module.js'; // ❌ Syntax error
// }

// Utiliser import dynamique pour conditionnel
if (condition) {
    const module = await import('./module.js'); // ✅
}

// Erreur 4: Accolades incorrectes
// import add from './math.js'; // ❌ si add est named export
import { add } from './math.js'; // ✅

// IMPORT EN REACT

/*
// Composant
import React from 'react';
import { useState, useEffect } from 'react';

// Composants
import Header from './components/Header';
import Footer from './components/Footer';

// Hooks custom
import { useAuth } from './hooks/useAuth';
import { useData } from './hooks/useData';

// Utils
import { formatDate } from './utils/date';
import { apiClient } from './utils/api';

// Constants
import { ROUTES } from './constants/routes';

// Styles
import './App.css';

function App() {
    const [data, setData] = useState(null);
    const { user } = useAuth();
    
    return (
        <div>
            <Header user={user} />
            {data && <div>{formatDate(data.date)}</div>}
            <Footer />
        </div>
    );
}

export default App;
*/

// WILDCARD IMPORT (tout importer)
import * as utils from './utils.js';

// Accéder avec namespace
utils.formatDate();
utils.parseDate();
utils.isEmail();

// GROUPER IMPORTS PAR TYPE

// External
import React, { useState } from 'react';
import axios from 'axios';

// Components
import Header from './Header';
import Footer from './Footer';

// Hooks
import { useAuth } from './hooks/useAuth';

// Utils
import { formatDate } from './utils/date';

// Constants
import { API_URL } from './constants';

// Styles
import './styles.css';
```

**Points clés:**
- `import { name }` pour named exports
- `import name` pour default export
- `import * as` pour tout importer
- Chemins relatifs (`./`) ou absolus (node_modules)
- Extensions `.js` recommandées (sans bundler)
- Ordre: external → internal → styles
- Imports statiques (pas dans if/loop)

---

### Q3: Comment créer un "barrel export" (index.js) ?

**Réponse:**
```javascript
// BARREL EXPORT = Fichier index.js qui re-exporte plusieurs modules
// But: Simplifier les imports

// STRUCTURE DE DOSSIERS
/*
components/
├── Button/
│   ├── Button.js
│   └── index.js
├── Card/
│   ├── Card.js
│   └── index.js
├── Modal/
│   ├── Modal.js
│   └── index.js
└── index.js      ← BARREL EXPORT
*/

// ========== components/Button/Button.js ==========
export default function Button({ children, onClick }) {
    return <button onClick={onClick}>{children}</button>;
}

// Également export helpers si besoin
export const BUTTON_VARIANTS = {
    primary: 'primary',
    secondary: 'secondary'
};

// ========== components/Button/index.js ==========
// Re-export pour simplifier le chemin
export { default } from './Button';
export * from './Button';

// Maintenant on peut importer ainsi:
// import Button from './components/Button';
// Au lieu de:
// import Button from './components/Button/Button';

// ========== components/index.js (BARREL) ==========
// Centraliser tous les exports de components

// Méthode 1: Re-export default avec renommage
export { default as Button } from './Button';
export { default as Card } from './Card';
export { default as Modal } from './Modal';
export { default as Input } from './Input';

// Maintenant on peut importer plusieurs en une ligne:
// import { Button, Card, Modal } from './components';

// Méthode 2: Re-export tout (*)
export * from './Button';
export * from './Card';
export * from './Modal';

// ⚠️ Attention: export * n'exporte PAS les default exports
// Il faut les exporter séparément

// Méthode 3: Import puis export
import Button from './Button';
import Card from './Card';
import Modal from './Modal';

export { Button, Card, Modal };

// EXEMPLE COMPLET

// ========== utils/date.js ==========
export const formatDate = (date) => {
    return date.toLocaleDateString();
};

export const parseDate = (str) => {
    return new Date(str);
};

export const isValidDate = (date) => {
    return date instanceof Date && !isNaN(date);
};

// ========== utils/string.js ==========
export const capitalize = (str) => {
    return str.charAt(0).toUpperCase() + str.slice(1);
};

export const truncate = (str, length) => {
    return str.length > length ? str.slice(0, length) + '...' : str;
};

// ========== utils/validation.js ==========
export const isEmail = (email) => {
    return email.includes('@');
};

export const isPhone = (phone) => {
    return /^\d{10}$/.test(phone);
};

// ========== utils/index.js (BARREL) ==========
// Re-exporter tout
export * from './date';
export * from './string';
export * from './validation';

// Maintenant un seul import pour tout:
// import { formatDate, capitalize, isEmail } from './utils';

// AVEC RENOMMAGE

// ========== services/index.js ==========
export { default as apiService } from './api';
export { default as authService } from './auth';
export { default as userService } from './user';

// Import:
// import { apiService, authService } from './services';

// PATTERN AVANCÉ: Feature-based

/*
features/
├── auth/
│   ├── components/
│   │   ├── LoginForm.js
│   │   └── RegisterForm.js
│   ├── hooks/
│   │   └── useAuth.js
│   ├── services/
│   │   └── authService.js
│   └── index.js          ← Barrel
*/

// ========== features/auth/index.js ==========
// Exporter tout ce qui est public de la feature
export { default as LoginForm } from './components/LoginForm';
export { default as RegisterForm } from './components/RegisterForm';
export { useAuth } from './hooks/useAuth';
export { authService } from './services/authService';

// Import simple depuis autre part:
// import { LoginForm, useAuth, authService } from './features/auth';

// AVANTAGES

// ✅ Imports plus simples
// Au lieu de:
import Button from './components/Button/Button';
import Card from './components/Card/Card';
import Modal from './components/Modal/Modal';

// On peut faire:
import { Button, Card, Modal } from './components';

// ✅ Point d'entrée unique
// Facilite refactoring interne

// ✅ API claire
// On contrôle ce qui est public

// INCONVÉNIENTS

// ⚠️ Peut importer code inutile
// Si on importe un component, webpack peut bundler tout le barrel

// ⚠️ Augmente taille du bundle
// Surtout avec export *

// SOLUTION: Tree-shaking
// Avec bundlers modernes (webpack, vite) et ES6 modules,
// le code non utilisé est éliminé automatiquement

// QUAND UTILISER?

// ✅ Pour component libraries
export { Button } from './Button';
export { Input } from './Input';
export { Card } from './Card';

// ✅ Pour utilities groupées
export * from './date';
export * from './string';

// ✅ Pour features modules
export { LoginForm, useAuth } from './auth';

// ❌ Éviter pour très gros modules
// Préférer imports directs si performance critique

// EXEMPLE REACT COMPLET

// ========== components/index.js ==========
export { default as Header } from './Header';
export { default as Footer } from './Footer';
export { default as Sidebar } from './Sidebar';
export { default as Button } from './Button';
export { default as Input } from './Input';
export { default as Card } from './Card';

// ========== hooks/index.js ==========
export { useAuth } from './useAuth';
export { useData } from './useData';
export { useTheme } from './useTheme';

// ========== utils/index.js ==========
export * from './date';
export * from './string';
export * from './validation';

// ========== App.js ==========
// Imports propres et groupés
import React from 'react';
import { Header, Footer, Button } from './components';
import { useAuth, useTheme } from './hooks';
import { formatDate, capitalize } from './utils';

function App() {
    const { user } = useAuth();
    const { theme } = useTheme();
    
    return (
        <div className={theme}>
            <Header />
            <Button>{capitalize(user.name)}</Button>
            <Footer />
        </div>
    );
}

export default App;

// ALTERNATIVE SANS BARREL

// Import direct (plus verbeux mais explicite)
import Header from './components/Header/Header';
import Footer from './components/Footer/Footer';
import Button from './components/Button/Button';
import { useAuth } from './hooks/useAuth';
import { formatDate } from './utils/date';

// C'est une question de préférence et de projet
```

**Points clés:**
- Barrel = fichier index.js qui re-exporte
- `export { default as Name } from './Module'`
- `export * from './module'` pour named exports
- Simplifie imports multiples
- Avantages: imports propres, API claire
- Inconvénients: peut augmenter bundle size
- Tree-shaking aide à éliminer code inutilisé

---

### Q4: Comment utiliser l'import dynamique `import()` ?

**Réponse:**
```javascript
// IMPORT DYNAMIQUE = import() retourne une Promise
// Permet lazy loading et code splitting

// SYNTAXE DE BASE (async/await)
async function loadModule() {
    const module = await import('./math.js');
    console.log(module.add(2, 3)); // 5
}

loadModule();

// SYNTAXE PROMISE
import('./math.js')
    .then(module => {
        console.log(module.add(2, 3)); // 5
    })
    .catch(error => {
        console.error('Failed to load:', error);
    });

// IMPORT DEFAULT

async function loadUser() {
    // Default export est dans .default
    const { default: User } = await import('./user.js');
    const user = new User('John');
    console.log(user.greet());
}

// IMPORT NAMED EXPORTS

async function loadUtils() {
    const { helper1, helper2, PI } = await import('./utils.js');
    console.log(helper1());
    console.log(PI);
}

// IMPORT TOUT

async function loadMath() {
    const math = await import('./math.js');
    console.log(math.add(5, 3));    // 8
    console.log(math.default);      // Default export si existe
}

// CAS D'USAGE 1: Lazy loading conditionnel

async function loadFeature(featureName) {
    if (featureName === 'advanced') {
        const { AdvancedFeature } = await import('./features/advanced.js');
        return new AdvancedFeature();
    } else {
        const { BasicFeature } = await import('./features/basic.js');
        return new BasicFeature();
    }
}

// Utilisation
button.addEventListener('click', async () => {
    const feature = await loadFeature('advanced');
    feature.run();
});

// CAS D'USAGE 2: Route-based code splitting

async function loadPage(route) {
    let PageComponent;
    
    switch(route) {
        case '/home':
            PageComponent = await import('./pages/Home.js');
            break;
        case '/about':
            PageComponent = await import('./pages/About.js');
            break;
        case '/contact':
            PageComponent = await import('./pages/Contact.js');
            break;
        default:
            PageComponent = await import('./pages/NotFound.js');
    }
    
    return PageComponent.default;
}

// Router simple
async function navigate(route) {
    const Page = await loadPage(route);
    // Render Page
}

// CAS D'USAGE 3: Load on demand (user action)

// Charger modal seulement quand nécessaire
openModalButton.addEventListener('click', async () => {
    const { default: Modal } = await import('./components/Modal.js');
    const modal = new Modal();
    modal.show();
});

// CAS D'USAGE 4: Charger selon configuration

async function loadConfig() {
    const env = process.env.NODE_ENV;
    
    if (env === 'production') {
        return await import('./config/prod.js');
    } else if (env === 'development') {
        return await import('./config/dev.js');
    } else {
        return await import('./config/test.js');
    }
}

// CAS D'USAGE 5: Internationalisation (i18n)

async function loadTranslations(language) {
    // Template literal dans import()
    const translations = await import(`./locales/${language}.js`);
    return translations.default;
}

// Utilisation
const frTranslations = await loadTranslations('fr');
console.log(frTranslations.hello); // "Bonjour"

const enTranslations = await loadTranslations('en');
console.log(enTranslations.hello); // "Hello"

// CAS D'USAGE 6: Fallback si module échoue

async function loadModuleWithFallback() {
    try {
        return await import('./preferredModule.js');
    } catch (error) {
        console.warn('Loading fallback');
        return await import('./fallbackModule.js');
    }
}

// CAS D'USAGE 7: Preloading (charger à l'avance)

// Charger en arrière-plan sans bloquer
function preloadModules() {
    // Modules seront en cache pour utilisation future
    import('./heavy-module.js');
    import('./another-module.js');
}

// Appeler au load de la page
window.addEventListener('load', preloadModules);

// REACT: Lazy loading components

/*
import React, { lazy, Suspense } from 'react';

// Lazy load component
const HeavyComponent = lazy(() => import('./components/Heavy'));
const Dashboard = lazy(() => import('./pages/Dashboard'));

function App() {
    return (
        <Suspense fallback={<div>Loading...</div>}>
            <HeavyComponent />
        </Suspense>
    );
}

// Avec React Router
import { BrowserRouter, Routes, Route } from 'react-router-dom';

const Home = lazy(() => import('./pages/Home'));
const About = lazy(() => import('./pages/About'));

function App() {
    return (
        <BrowserRouter>
            <Suspense fallback={<Spinner />}>
                <Routes>
                    <Route path="/" element={<Home />} />
                    <Route path="/about" element={<About />} />
                </Routes>
            </Suspense>
        </BrowserRouter>
    );
}
*/

// IMPORT MULTIPLE EN PARALLÈLE

async function loadMultiple() {
    const [math, utils, data] = await Promise.all([
        import('./math.js'),
        import('./utils.js'),
        import('./data.js')
    ]);
    
    console.log(math.add(2, 3));
    console.log(utils.helper());
    console.log(data.default);
}

// GESTION D'ERREURS

async function safeImport(modulePath) {
    try {
        const module = await import(modulePath);
        return { success: true, module };
    } catch (error) {
        console.error(`Failed to load ${modulePath}:`, error);
        return { success: false, error };
    }
}

// Utilisation
const result = await safeImport('./module.js');
if (result.success) {
    result.module.doSomething();
} else {
    console.log('Module not available');
}

// AVEC RETRY

async function importWithRetry(modulePath, retries = 3) {
    for (let i = 0; i < retries; i++) {
        try {
            return await import(modulePath);
        } catch (error) {
            if (i === retries - 1) throw error;
            console.log(`Retry ${i + 1}/${retries}`);
            await new Promise(resolve => setTimeout(resolve, 1000));
        }
    }
}

// AVEC TIMEOUT

async function importWithTimeout(modulePath, timeout = 5000) {
    return Promise.race([
        import(modulePath),
        new Promise((_, reject) => 
            setTimeout(() => reject(new Error('Timeout')), timeout)
        )
    ]);
}

// CACHE MANUEL (éviter re-imports)

const moduleCache = new Map();

async function cachedImport(modulePath) {
    if (moduleCache.has(modulePath)) {
        return moduleCache.get(modulePath);
    }
    
    const module = await import(modulePath);
    moduleCache.set(modulePath, module);
    return module;
}

// DIFFÉRENCE AVEC import STATIQUE

// Static import (au début du fichier)
import { add } from './math.js';
// - Chargé AVANT exécution du code
// - Toujours chargé, même si pas utilisé
// - Synchrone
// - Au top-level uniquement

// Dynamic import (n'importe où)
const module = await import('./math.js');
// - Chargé QUAND exécuté
// - Chargé seulement si condition remplie
// - Asynchrone (Promise)
// - N'importe où dans le code

// BUNDLERS (webpack, vite)
// Créent automatiquement des chunks séparés pour imports dynamiques

// webpack magic comments
const module = await import(
    /* webpackChunkName: "my-chunk" */
    /* webpackPrefetch: true */
    './module.js'
);

// AVANTAGES

// ✅ Lazy loading = chargement à la demande
// ✅ Code splitting = bundles plus petits
// ✅ Performance améliorée (initial load)
// ✅ Conditionnel (if/else)
// ✅ User actions

// INCONVÉNIENTS

// ⚠️ Asynchrone (Promise)
// ⚠️ Légère latence au premier load
// ⚠️ Plus complexe à gérer
```

**Points clés:**
- `import()` retourne Promise
- Lazy loading = charger à la demande
- Code splitting automatique avec bundlers
- Peut être conditionnel (if/else)
- React.lazy() pour components
- `.default` pour default exports
- Améliore performance initial load

---

### Q5: Quelle est la différence entre modules ES6 et CommonJS ?

**Réponse:**
```javascript
// ES6 MODULES (ESM) - Standard moderne

// Export
export const add = (a, b) => a + b;
export default class User { }

// Import
import { add } from './math.js';
import User from './user.js';

// COMMONJS (CJS) - Node.js historique

// Export
const add = (a, b) => a + b;
module.exports = { add };
// Ou
exports.add = add;

// Import
const { add } = require('./math.js');
const math = require('./math.js');

// DIFFÉRENCES PRINCIPALES

// 1. SYNTAXE
// ESM: export/import
export const x = 1;
import { x } from './module.js';

// CJS: module.exports/require
module.exports.x = 1;
const { x } = require('./module.js');

// 2. TIMING (Quand chargé)
// ESM: Statique (avant exécution)
import { add } from './math.js'; // Analysé au parse time
// Chargé avant que le code s'exécute

// CJS: Dynamique (pendant exécution)
const math = require('./math.js'); // Exécuté au runtime
// Chargé quand la ligne est exécutée

// 3. SYNCHRONE vs ASYNCHRONE
// ESM: Asynchrone (peut charger en parallèle)
import { a } from './a.js';
import { b } from './b.js';
// a.js et b.js peuvent charger en parallèle

// CJS: Synchrone (un par un)
const a = require('./a.js'); // Bloque jusqu'au chargement
const b = require('./b.js'); // Puis celui-ci

// 4. IMPORT CONDITIONNEL
// ESM: Pas permis (statique)
// if (condition) {
//     import { x } from './module.js'; // ❌ Syntax error
// }

// Utiliser import() dynamique
if (condition) {
    const module = await import('./module.js'); // ✅
}

// CJS: Permis (dynamique)
if (condition) {
    const module = require('./module.js'); // ✅
}

// 5. THIS
// ESM: this = undefined
console.log(this); // undefined

// CJS: this = module.exports
console.log(this); // {}

// 6. __dirname et __filename
// ESM: Pas disponibles directement
// import { fileURLToPath } from 'url';
// const __filename = fileURLToPath(import.meta.url);
// const __dirname = path.dirname(__filename);

// CJS: Disponibles
console.log(__dirname);
console.log(__filename);

// 7. TREE SHAKING
// ESM: Oui (bundlers peuvent éliminer code inutilisé)
import { add } from './math.js';
// multiply et subtract ne seront pas bundlés si pas utilisés

// CJS: Non (tout est inclus)
const math = require('./math.js');
// Tout le module est inclus

// 8. NAMED EXPORTS
// ESM: Natif
export const a = 1;
export const b = 2;
import { a, b } from './module.js';

// CJS: Via destructuring
module.exports = { a: 1, b: 2 };
const { a, b } = require('./module.js');

// 9. DEFAULT EXPORT
// ESM: Natif
export default function() { }
import fn from './module.js';

// CJS: Convention
module.exports = function() { };
const fn = require('./module.js');

// 10. CIRCULAR DEPENDENCIES
// ESM: Gère mieux (hoisting)
// CJS: Peut causer problèmes

// COMPATIBILITÉ

// Node.js moderne supporte les deux

// Pour utiliser ESM dans Node.js:
// 1. Extension .mjs
// math.mjs
export const add = (a, b) => a + b;

// 2. Ou "type": "module" dans package.json
// {
//   "type": "module"
// }

// Pour utiliser CJS avec "type": "module"
// Utiliser extension .cjs

// INTEROPÉRABILITÉ

// Importer CJS depuis ESM
// CJS module (math.cjs)
module.exports = { add: (a, b) => a + b };

// Import dans ESM
import math from './math.cjs';
console.log(math.add(2, 3)); // 5

// Importer ESM depuis CJS (avec import())
// ESM module (math.mjs)
export const add = (a, b) => a + b;

// Import dans CJS (async nécessaire)
(async () => {
    const math = await import('./math.mjs');
    console.log(math.add(2, 3)); // 5
})();

// EXEMPLES PRATIQUES

// ESM (moderne, recommandé)
// ========== math.js ==========
export const add = (a, b) => a + b;
export const subtract = (a, b) => a - b;
export default class Calculator {
    multiply(a, b) { return a * b; }
}

// ========== app.js ==========
import Calculator, { add, subtract } from './math.js';

const calc = new Calculator();
console.log(add(2, 3));           // 5
console.log(subtract(10, 4));     // 6
console.log(calc.multiply(3, 4)); // 12

// CJS (legacy, Node.js)
// ========== math.js ==========
const add = (a, b) => a + b;
const subtract = (a, b) => a - b;

class Calculator {
    multiply(a, b) { return a * b; }
}

module.exports = { add, subtract, Calculator };
// Ou pour default:
// module.exports = Calculator;

// ========== app.js ==========
const { add, subtract, Calculator } = require('./math.js');

const calc = new Calculator();
console.log(add(2, 3));
console.log(subtract(10, 4));
console.log(calc.multiply(3, 4));

// MIGRATION CJS → ESM

// Avant (CJS)
const express = require('express');
const { formatDate } = require('./utils');

module.exports = function createApp() {
    const app = express();
    // ...
    return app;
};

// Après (ESM)
import express from 'express';
import { formatDate } from './utils.js';

export default function createApp() {
    const app = express();
    // ...
    return app;
}

// BUNDLERS (webpack, vite, rollup)
// Gèrent automatiquement les deux formats
// Convertissent tout en ESM en interne

// QUAND UTILISER QUOI?

// UTILISER ESM:
// ✅ Nouveaux projets
// ✅ Frontend (React, Vue, etc.)
// ✅ Node.js moderne (v12.20+)
// ✅ Tree shaking nécessaire
// ✅ Standard moderne

// UTILISER CJS:
// ✅ Legacy Node.js code
// ✅ Certains packages npm anciens
// ✅ Scripts Node.js simples

// TABLEAU RÉCAPITULATIF

/*
┌─────────────────┬──────────────────┬──────────────────┐
│                 │ ES6 Modules      │ CommonJS         │
├─────────────────┼──────────────────┼──────────────────┤
│ Export          │ export           │ module.exports   │
│ Import          │ import           │ require()        │
│ Timing          │ Statique         │ Dynamique        │
│ Async           │ Oui              │ Non              │
│ Conditionnel    │ Non (sauf dyn)   │ Oui              │
│ Tree shaking    │ Oui              │ Non              │
│ Browser         │ Natif (moderne)  │ Non (bundler)    │
│ Node.js         │ Oui (v12.20+)    │ Oui (natif)      │
│ this            │ undefined        │ module.exports   │
│ __dirname       │ Non direct       │ Oui              │
│ Standard        │ Oui (ECMAScript) │ Non (Node.js)    │
└─────────────────┴──────────────────┴──────────────────┘
*/

// BEST PRACTICE MODERNE
// Utiliser ESM partout où possible
// CJS seulement pour compatibilité legacy
```

**Points clés:**
- ESM = export/import (standard moderne)
- CJS = module.exports/require (Node.js legacy)
- ESM: statique, async, tree-shaking
- CJS: dynamique, sync, pas de tree-shaking
- Node.js moderne supporte les deux
- ESM recommandé pour nouveaux projets
- Interop possible avec import() dynamique

---

## Questions Niveau Intermédiaire

### Q6: Comment éviter les imports circulaires ?

**Réponse:**
```javascript
// IMPORT CIRCULAIRE = A importe B, B importe A
// Peut causer problèmes (undefined, erreurs)

// ❌ MAUVAIS EXEMPLE (circulaire)

// ========== userService.js ==========
import { postService } from './postService.js';

export const userService = {
    getUser(id) {
        const user = { id, name: 'John' };
        // Appelle postService
        user.posts = postService.getUserPosts(id);
        return user;
    }
};

// ========== postService.js ==========
import { userService } from './userService.js';

export const postService = {
    getUserPosts(userId) {
        // Appelle userService
        const user = userService.getUser(userId); // ❌ Circulaire!
        return user.posts || [];
    }
};

// Résultat: undefined ou erreurs

// SOLUTION 1: Extraire dépendances communes dans module séparé

// ========== api.js (module commun) ==========
export const api = {
    fetchUser(id) {
        return fetch(`/api/users/${id}`).then(r => r.json());
    },
    
    fetchPosts(userId) {
        return fetch(`/api/posts?userId=${userId}`).then(r => r.json());
    }
};

// ========== userService.js ==========
import { api } from './api.js';

export const userService = {
    async getUser(id) {
        const user = await api.fetchUser(id);
        return user;
    }
};

// ========== postService.js ==========
import { api } from './api.js';

export const postService = {
    async getUserPosts(userId) {
        const posts = await api.fetchPosts(userId);
        return posts;
    }
};

// Plus de circulaire! ✅

// SOLUTION 2: Dependency Injection

// ========== userService.js ==========
export const createUserService = (postService) => ({
    getUser(id) {
        const user = { id, name: 'John' };
        user.posts = postService.getUserPosts(id);
        return user;
    }
});

// ========== postService.js ==========
export const createPostService = (userService) => ({
    getUserPosts(userId) {
        return [
            { id: 1, userId, title: 'Post 1' },
            { id: 2, userId, title: 'Post 2' }
        ];
    }
});

// ========== index.js ==========
import { createUserService } from './userService.js';
import { createPostService } from './postService.js';

// Créer services sans dépendance circulaire
const postService = createPostService();
const userService = createUserService(postService);

export { userService, postService };

// SOLUTION 3: Import dynamique

// ========== userService.js ==========
export const userService = {
    async getUser(id) {
        const user = { id, name: 'John' };
        
        // Import dynamique pour éviter circulaire
        const { postService } = await import('./postService.js');
        user.posts = await postService.getUserPosts(id);
        
        return user;
    }
};

// ========== postService.js ==========
export const postService = {
    getUserPosts(userId) {
        return Promise.resolve([
            { id: 1, userId, title: 'Post 1' }
        ]);
    }
};

// SOLUTION 4: Restructurer le code (meilleure solution)

// Au lieu de services qui s'appellent mutuellement,
// avoir un controller qui orchestre:

// ========== userService.js ==========
export const userService = {
    getUser(id) {
        return { id, name: 'John' };
    }
};

// ========== postService.js ==========
export const postService = {
    getUserPosts(userId) {
        return [
            { id: 1, userId, title: 'Post 1' }
        ];
    }
};

// ========== userController.js ==========
import { userService } from './userService.js';
import { postService } from './postService.js';

export const userController = {
    getUserWithPosts(id) {
        const user = userService.getUser(id);
        user.posts = postService.getUserPosts(id);
        return user;
    }
};

// DÉTECTER IMPORTS CIRCULAIRES

// Dans console du navigateur/Node.js:
// Warning: Circular dependency detected

// Outils:
// - webpack: affiche warnings
// - ESLint: plugin import/no-cycle
// - madge: visualiser dépendances

// .eslintrc.js
/*
{
    "plugins": ["import"],
    "rules": {
        "import/no-cycle": "error"
    }
}
*/

// EXEMPLE REACT: Composants circulaires

// ❌ Mauvais
// Parent.js
import Child from './Child';

export default function Parent() {
    return <div><Child /></div>;
}

// Child.js
import Parent from './Parent'; // ❌ Circulaire!

export default function Child() {
    return <div><Parent /></div>;
}

// ✅ Solution: Composition
// Parent.js
import Child from './Child';

export default function Parent() {
    return (
        <div>
            <Child>
                <div>Content</div>
            </Child>
        </div>
    );
}

// Child.js
export default function Child({ children }) {
    return <div>{children}</div>;
}

// PATTERN: Types séparés (TypeScript)

// ❌ Circulaire
// user.ts
import { Post } from './post';

export interface User {
    id: number;
    posts: Post[];
}

// post.ts
import { User } from './user'; // Circulaire!

export interface Post {
    id: number;
    author: User;
}

// ✅ Solution: types.ts commun
// types.ts
export interface User {
    id: number;
    posts: Post[];
}

export interface Post {
    id: number;
    author: User;
}

// user.ts
import { User } from './types';

// post.ts
import { Post } from './types';

// BONNES PRATIQUES

// 1. Architecture en couches
/*
Presentation Layer (UI)
    ↓
Business Logic Layer (Services)
    ↓
Data Access Layer (API)
*/

// Dépendances vont vers le bas, jamais vers le haut

// 2. Dependency Inversion Principle
// Dépendre d'abstractions, pas de concrétions

// 3. Single Responsibility
// Un module = une responsabilité
// Réduit besoin de dépendances mutuelles

// 4. Utiliser events/observers
// Au lieu d'appels directs
```

**Points clés:**
- Import circulaire = A → B → A
- Cause: undefined, erreurs
- Solutions: module commun, DI, import dynamique, restructurer
- ESLint: import/no-cycle pour détecter
- Architecture en couches évite problème
- React: composition au lieu de circulaire

---

### Q7: Comment organiser les modules dans un grand projet ?

**Réponse:**
```javascript
// STRUCTURE RECOMMANDÉE POUR PROJET MOYEN/GRAND

/*
project/
├── public/                    # Fichiers statiques
│   ├── index.html
│   └── favicon.ico
│
├── src/
│   ├── components/            # Composants UI réutilisables
│   │   ├── common/           # Composants communs
│   │   │   ├── Button/
│   │   │   │   ├── Button.js
│   │   │   │   ├── Button.css
│   │   │   │   ├── Button.test.js
│   │   │   │   └── index.js
│   │   │   ├── Input/
│   │   │   ├── Card/
│   │   │   └── index.js      # Barrel export
│   │   │
│   │   └── layout/           # Layout components
│   │       ├── Header/
│   │       ├── Footer/
│   │       ├── Sidebar/
│   │       └── index.js
│   │
│   ├── features/              # Features modules (domain-based)
│   │   ├── auth/
│   │   │   ├── components/
│   │   │   │   ├── LoginForm.js
│   │   │   │   └── RegisterForm.js
│   │   │   ├── hooks/
│   │   │   │   └── useAuth.js
│   │   │   ├── services/
│   │   │   │   └── authService.js
│   │   │   ├── constants/
│   │   │   │   └── authConstants.js
│   │   │   └── index.js
│   │   │
│   │   ├── dashboard/
│   │   │   ├── components/
│   │   │   ├── hooks/
│   │   │   └── index.js
│   │   │
│   │   └── profile/
│   │       ├── components/
│   │       └── index.js
│   │
│   ├── hooks/                 # Custom hooks globaux
│   │   ├── useLocalStorage.js
│   │   ├── useDebounce.js
│   │   ├── useMediaQuery.js
│   │   └── index.js
│   │
│   ├── services/              # Services API/Business logic
│   │   ├── api/
│   │   │   ├── client.js     # Axios/fetch config
│   │   │   ├── users.js
│   │   │   ├── posts.js
│   │   │   └── index.js
│   │   │
│   │   └── storage/
│   │       ├── localStorage.js
│   │       └── sessionStorage.js
│   │
│   ├── utils/                 # Fonctions utilitaires
│   │   ├── date.js
│   │   ├── string.js
│   │   ├── validation.js
│   │   ├── format.js
│   │   └── index.js
│   │
│   ├── constants/             # Constantes globales
│   │   ├── routes.js
│   │   ├── config.js
│   │   ├── apiEndpoints.js
│   │   └── index.js
│   │
│   ├── types/                 # Types TypeScript (si applicable)
│   │   ├── user.ts
│   │   ├── post.ts
│   │   └── index.ts
│   │
│   ├── styles/                # Styles globaux
│   │   ├── globals.css
│   │   ├── variables.css
│   │   └── themes.css
│   │
│   ├── assets/                # Images, fonts, etc.
│   │   ├── images/
│   │   ├── icons/
│   │   └── fonts/
│   │
│   ├── context/               # React Context (si utilisé)
│   │   ├── ThemeContext.js
│   │   ├── AuthContext.js
│   │   └── index.js
│   │
│   ├── router/                # Configuration routing
│   │   ├── routes.js
│   │   └── ProtectedRoute.js
│   │
│   ├── App.js                 # Composant principal
│   ├── index.js               # Point d'entrée
│   └── setupTests.js          # Config tests
│
├── tests/                     # Tests (si séparés)
│   ├── unit/
│   ├── integration/
│   └── e2e/
│
├── .env                       # Variables d'environnement
├── .env.example
├── .eslintrc.js              # Config ESLint
├── .prettierrc               # Config Prettier
├── package.json
└── README.md
*/

// EXEMPLES D'ORGANISATION

// ========== components/common/index.js (Barrel) ==========
export { default as Button } from './Button';
export { default as Input } from './Input';
export { default as Card } from './Card';
export { default as Modal } from './Modal';

// ========== features/auth/index.js ==========
// Exposer seulement ce qui est public
export { default as LoginForm } from './components/LoginForm';
export { default as RegisterForm } from './components/RegisterForm';
export { useAuth } from './hooks/useAuth';
export { authService } from './services/authService';

// ========== services/api/index.js ==========
export { default as apiClient } from './client';
export * as usersApi from './users';
export * as postsApi from './posts';

// ========== utils/index.js ==========
export * from './date';
export * from './string';
export * from './validation';
export * from './format';

// ========== constants/index.js ==========
export * from './routes';
export * from './config';
export * from './apiEndpoints';

// PATTERN: Feature-based (Domain-driven)

// Chaque feature = module auto-suffisant
/*
features/
├── auth/
│   ├── components/        # Composants spécifiques auth
│   ├── hooks/            # Hooks spécifiques auth
│   ├── services/         # Services spécifiques auth
│   ├── constants/        # Constants spécifiques auth
│   ├── utils/            # Utils spécifiques auth
│   └── index.js          # API publique de la feature
*/

// Avantages:
// - Cohésion élevée (tout lié à auth ensemble)
// - Couplage faible (features indépendantes)
// - Facile à tester
// - Facile à supprimer/remplacer feature

// IMPORTS DANS APP

// ========== App.js ==========
// External dependencies
import React, { useState, useEffect } from 'react';
import { BrowserRouter, Routes, Route } from 'react-router-dom';

// Layout components
import { Header, Footer, Sidebar } from './components/layout';

// Common components
import { Button, Card, Modal } from './components/common';

// Features
import { LoginForm, useAuth } from './features/auth';
import { Dashboard } from './features/dashboard';
import { Profile } from './features/profile';

// Hooks
import { useTheme, useLocalStorage } from './hooks';

// Utils
import { formatDate, isEmail } from './utils';

// Constants
import { ROUTES, API_URL } from './constants';

// Styles
import './styles/globals.css';

function App() {
    const { user, isAuthenticated } = useAuth();
    const [theme] = useTheme();
    
    return (
        <BrowserRouter>
            <div className={theme}>
                <Header user={user} />
                <Routes>
                    <Route path={ROUTES.HOME} element={<Dashboard />} />
                    <Route path={ROUTES.PROFILE} element={<Profile />} />
                    <Route path={ROUTES.LOGIN} element={<LoginForm />} />
                </Routes>
                <Footer />
            </div>
        </BrowserRouter>
    );
}

export default App;

// NAMING CONVENTIONS

// Files:
// - Components: PascalCase (Button.js, UserCard.js)
// - Utils/hooks: camelCase (useAuth.js, formatDate.js)
// - Constants: UPPER_SNAKE_CASE ou camelCase (API_URL.js, routes.js)
// - Index files: index.js (lowercase)

// Exports:
// - Components: PascalCase
export default function Button() { }

// - Functions: camelCase
export const formatDate = () => { };

// - Constants: UPPER_SNAKE_CASE
export const API_URL = 'https://api.example.com';

// BONNES PRATIQUES

// 1. Co-location
// Mettre fichiers liés ensemble
/*
Button/
├── Button.js       # Component
├── Button.css      # Styles
├── Button.test.js  # Tests
└── index.js        # Export
*/

// 2. Index.js pour simplifier imports
// Au lieu de:
import Button from './components/common/Button/Button';

// Faire:
import Button from './components/common/Button';
// ou avec barrel:
import { Button } from './components/common';

// 3. Absolute imports (avec alias)
// Configuration webpack/vite:
/*
resolve: {
    alias: {
        '@': path.resolve(__dirname, 'src'),
        '@components': path.resolve(__dirname, 'src/components'),
        '@utils': path.resolve(__dirname, 'src/utils'),
    }
}
*/

// Puis:
import { Button } from '@components/common';
import { formatDate } from '@utils/date';

// 4. README par feature
/*
features/auth/
├── README.md       # Documentation de la feature
├── components/
└── ...
*/

// 5. Tests à côté du code
/*
Button/
├── Button.js
├── Button.test.js    # Co-located
└── index.js
*/

// Ou dossier __tests__
/*
Button/
├── __tests__/
│   └── Button.test.js
├── Button.js
└── index.js
*/

// 6. Types avec code (TypeScript)
/*
Button/
├── Button.tsx
├── Button.types.ts   # Types spécifiques
├── Button.test.tsx
└── index.ts
*/

// ANTI-PATTERNS À ÉVITER

// ❌ Tout dans un fichier
// utils.js (500 lignes)

// ✅ Séparer par domaine
// utils/date.js
// utils/string.js
// utils/validation.js

// ❌ Imports relatifs profonds
import { Button } from '../../../components/common/Button';

// ✅ Absolute imports ou barrel
import { Button } from '@components/common';

// ❌ Barrel exports massifs
// components/index.js exporte 100+ components

// ✅ Grouper logiquement
// components/common/index.js (composants communs)
// components/layout/index.js (layout)

// ❌ Mélanger concerns
// UserDashboard.js (UI + API + business logic)

// ✅ Séparer
// components/UserDashboard.js (UI)
// services/userService.js (API)
// hooks/useUserData.js (logic)
```

**Points clés:**
- Structure par features (domain-driven)
- Co-location (fichiers liés ensemble)
- Barrel exports (index.js)
- Absolute imports avec alias
- Séparation des concerns
- Tests à côté du code
- Documentation par feature

---

**Résumé final Chapitre 15 pour entretien:**

**Modules ES6:**
- `export` (named) vs `export default`
- `import { name }` vs `import name`
- Import dynamique: `import()` pour lazy loading
- Barrel exports: index.js qui re-exporte

**Organisation:**
- Feature-based structure
- Co-location
- Absolute imports
- Éviter imports circulaires

**Différences:**
- ESM vs CommonJS
- Statique vs dynamique
- Tree-shaking avec ESM

**React:**
- Component par fichier
- Lazy loading avec React.lazy()
- Code splitting automatique
- Barrel exports pour libraries

Tu es prêt pour l'entretien! 🚀