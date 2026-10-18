## [**..**](./00_index.md)

# Chapitre 10 : BOM (Browser Object Model) 

## 10.1 Window Object

Le **Window Object** représente la fenêtre du navigateur. C'est l'objet global en JavaScript côté navigateur.

```javascript
// Dimensions de la fenêtre (viewport)
console.log(window.innerWidth);  // Largeur en pixels
console.log(window.innerHeight); // Hauteur en pixels

// Dimensions totales incluant les barres de défilement
console.log(window.outerWidth);
console.log(window.outerHeight);

// Position de la fenêtre
console.log(window.screenX); // Position X sur l'écran
console.log(window.screenY); // Position Y sur l'écran
```

---

## 10.2 Dialogues Utilisateur

```javascript
// alert() - Affiche un message simple
alert('Ceci est une alerte'); // Bloque l'exécution jusqu'à ce que l'utilisateur clique OK

// confirm() - Demande une confirmation (retourne true ou false)
const userConfirmed = confirm('Êtes-vous sûr de vouloir continuer ?');
if (userConfirmed) {
    console.log('Utilisateur a confirmé');
} else {
    console.log('Utilisateur a annulé');
}

// prompt() - Demande une saisie utilisateur (retourne string ou null)
const userName = prompt('Quel est votre nom ?', 'Nom par défaut');
if (userName !== null) {
    console.log(`Bonjour ${userName}`);
} else {
    console.log('Utilisateur a annulé');
}
```

---

## 10.3 Timers

```javascript
// setTimeout() - Exécute une fonction après un délai (en millisecondes)
const timeoutId = setTimeout(() => {
    console.log('Exécuté après 2 secondes');
}, 2000);

// clearTimeout() - Annule un timeout avant son exécution
clearTimeout(timeoutId);

// Exemple pratique : délai avec paramètres
setTimeout((name, age) => {
    console.log(`${name} a ${age} ans`);
}, 1000, 'Alice', 25);

// setInterval() - Exécute une fonction de manière répétée
let counter = 0;
const intervalId = setInterval(() => {
    counter++;
    console.log(`Compteur: ${counter}`);
    
    // Arrêter après 5 itérations
    if (counter === 5) {
        clearInterval(intervalId);
    }
}, 1000);

// clearInterval() - Arrête un interval
// clearInterval(intervalId);

// Exemple : Horloge
function displayTime() {
    const now = new Date();
    console.log(now.toLocaleTimeString());
}
const clockId = setInterval(displayTime, 1000);
// Pour arrêter : clearInterval(clockId);
```

---

## 10.4 Window Location

L'objet `window.location` contient des informations sur l'URL actuelle.

```javascript
// Propriétés de location
console.log(window.location.href);     // URL complète
console.log(window.location.protocol); // "http:" ou "https:"
console.log(window.location.host);     // "example.com:8080"
console.log(window.location.hostname); // "example.com"
console.log(window.location.port);     // "8080"
console.log(window.location.pathname); // "/path/page.html"
console.log(window.location.search);   // "?id=123&name=test"
console.log(window.location.hash);     // "#section"

// Exemple avec URL: https://example.com:8080/products?category=books#reviews
// protocol: "https:"
// hostname: "example.com"
// port: "8080"
// pathname: "/products"
// search: "?category=books"
// hash: "#reviews"

// Méthodes de navigation
// assign() - Charge une nouvelle page (conserve l'historique)
window.location.assign('https://www.google.com');

// replace() - Remplace la page actuelle (n'ajoute pas à l'historique)
window.location.replace('https://www.google.com');

// reload() - Recharge la page actuelle
window.location.reload(); // Depuis le cache
window.location.reload(true); // Force le rechargement depuis le serveur

// Raccourci pour changer de page
window.location.href = 'https://www.example.com';
// ou simplement
location.href = 'https://www.example.com';
```

---

## 10.5 Window History

L'objet `window.history` permet de naviguer dans l'historique du navigateur.

```javascript
// Navigation basique
history.back();    // Équivalent au bouton "Précédent"
history.forward(); // Équivalent au bouton "Suivant"
history.go(-2);    // Retourne 2 pages en arrière
history.go(1);     // Avance d'une page
history.go(0);     // Recharge la page actuelle

// Nombre d'entrées dans l'historique
console.log(history.length);

// pushState() - Ajoute une nouvelle entrée dans l'historique (sans recharger)
// Utile pour les Single Page Applications (SPA)
history.pushState(
    { page: 1, data: 'some data' }, // State object (données à associer)
    'Title',                         // Title (ignoré par la plupart des navigateurs)
    '/new-url'                       // Nouvelle URL
);

// replaceState() - Remplace l'entrée actuelle (sans recharger)
history.replaceState(
    { page: 2 },
    'New Title',
    '/updated-url'
);

// Écouter les changements d'historique
window.addEventListener('popstate', (event) => {
    console.log('Navigation détectée');
    console.log('State:', event.state); // Récupère l'objet state
    
    // Utile pour SPA
    if (event.state && event.state.page) {
        console.log(`Chargement de la page ${event.state.page}`);
    }
});

// Exemple pratique : SPA simple
function navigateTo(url, data) {
    // Mettre à jour l'URL sans recharger
    history.pushState(data, '', url);
    
    // Mettre à jour le contenu
    loadContent(url);
}

function loadContent(url) {
    // Logique pour charger le contenu selon l'URL
    console.log(`Chargement du contenu pour: ${url}`);
}
```

---

## 10.6 Window Actions

```javascript
// window.open() - Ouvre une nouvelle fenêtre ou onglet
const newWindow = window.open(
    'https://www.example.com',  // URL
    '_blank',                    // Target (_blank, _self, _parent, _top)
    'width=800,height=600'      // Features (dimensions, etc.)
);

// Vérifier si le popup a été bloqué
if (newWindow) {
    console.log('Fenêtre ouverte');
} else {
    console.log('Popup bloqué');
}

// window.close() - Ferme la fenêtre actuelle (fonctionne uniquement si ouverte par script)
// newWindow.close();

// window.print() - Ouvre la boîte de dialogue d'impression
window.print();

// window.stop() - Arrête le chargement de la page
window.stop();

// Défilement (scrolling)
// scrollTo() - Défile vers une position absolue
window.scrollTo(0, 500); // x=0, y=500 pixels

// Avec options (smooth scrolling)
window.scrollTo({
    top: 1000,
    left: 0,
    behavior: 'smooth' // Animation fluide
});

// scrollBy() - Défile de manière relative
window.scrollBy(0, 100); // Défile de 100px vers le bas

window.scrollBy({
    top: 200,
    behavior: 'smooth'
});

// scroll() - Alias de scrollTo()
window.scroll(0, 0); // Retour en haut de page

// Position actuelle du scroll
console.log(window.pageXOffset); // Position horizontale (ou window.scrollX)
console.log(window.pageYOffset); // Position verticale (ou window.scrollY)

// focus() et blur()
window.focus(); // Donne le focus à la fenêtre
window.blur();  // Retire le focus

// Exemple : Bouton "Retour en haut"
function scrollToTop() {
    window.scrollTo({
        top: 0,
        behavior: 'smooth'
    });
}

// Détecter le scroll
window.addEventListener('scroll', () => {
    const scrollPosition = window.pageYOffset;
    console.log(`Position: ${scrollPosition}px`);
    
    // Afficher le bouton si on a scrollé plus de 300px
    if (scrollPosition > 300) {
        // Afficher le bouton
    }
});
```

---

## 10.7 Web Storage API

Le **Web Storage** permet de stocker des données dans le navigateur (key-value pairs).

### Local Storage vs Session Storage

```javascript
// LOCAL STORAGE - Persistant (pas de date d'expiration)
// Les données survivent même après fermeture du navigateur

// setItem() - Stocker une valeur
localStorage.setItem('username', 'JohnDoe');
localStorage.setItem('theme', 'dark');
localStorage.setItem('score', '1500');

// getItem() - Récupérer une valeur (retourne null si la clé n'existe pas)
const username = localStorage.getItem('username');
console.log(username); // "JohnDoe"

const missing = localStorage.getItem('nonexistent');
console.log(missing); // null

// removeItem() - Supprimer une clé
localStorage.removeItem('theme');

// clear() - Tout supprimer
localStorage.clear();

// Propriété length
console.log(localStorage.length); // Nombre d'items

// key() - Récupérer une clé par index
const firstKey = localStorage.key(0);

// Itérer sur toutes les clés
for (let i = 0; i < localStorage.length; i++) {
    const key = localStorage.key(i);
    const value = localStorage.getItem(key);
    console.log(`${key}: ${value}`);
}

// SESSION STORAGE - Temporaire (effacé à la fermeture de l'onglet)
sessionStorage.setItem('tempData', 'value');
sessionStorage.getItem('tempData');
sessionStorage.removeItem('tempData');
sessionStorage.clear();
```

### Stockage d'objets JSON

```javascript
// IMPORTANT: localStorage ne stocke que des strings
// Pour stocker des objets, utiliser JSON.stringify() et JSON.parse()

const user = {
    id: 1,
    name: 'Alice',
    email: 'alice@example.com',
    preferences: {
        theme: 'dark',
        language: 'fr'
    }
};

// Stocker un objet (conversion en JSON string)
localStorage.setItem('user', JSON.stringify(user));

// Récupérer et parser l'objet
const storedUser = JSON.parse(localStorage.getItem('user'));
console.log(storedUser.name); // "Alice"
console.log(storedUser.preferences.theme); // "dark"

// Gestion d'erreur lors du parsing
try {
    const data = JSON.parse(localStorage.getItem('user'));
    console.log(data);
} catch (error) {
    console.error('Erreur de parsing:', error);
}

// Stocker un array
const tasks = ['Task 1', 'Task 2', 'Task 3'];
localStorage.setItem('tasks', JSON.stringify(tasks));

const storedTasks = JSON.parse(localStorage.getItem('tasks'));
console.log(storedTasks[0]); // "Task 1"
```

### Exemples Pratiques

```javascript
// Exemple 1: Système de préférences utilisateur
function savePreferences(theme, fontSize) {
    const prefs = {
        theme: theme,
        fontSize: fontSize,
        timestamp: Date.now()
    };
    localStorage.setItem('preferences', JSON.stringify(prefs));
}

function loadPreferences() {
    const prefs = localStorage.getItem('preferences');
    
    if (prefs) {
        return JSON.parse(prefs);
    }
    
    // Valeurs par défaut si pas de préférences
    return {
        theme: 'light',
        fontSize: '16px'
    };
}

// Utilisation
savePreferences('dark', '18px');
const userPrefs = loadPreferences();
console.log(userPrefs.theme); // "dark"

// Exemple 2: Panier d'achat
class ShoppingCart {
    constructor() {
        this.items = this.loadCart();
    }
    
    loadCart() {
        const cart = localStorage.getItem('cart');
        return cart ? JSON.parse(cart) : [];
    }
    
    saveCart() {
        localStorage.setItem('cart', JSON.stringify(this.items));
    }
    
    addItem(product) {
        this.items.push(product);
        this.saveCart();
    }
    
    removeItem(productId) {
        this.items = this.items.filter(item => item.id !== productId);
        this.saveCart();
    }
    
    clearCart() {
        this.items = [];
        localStorage.removeItem('cart');
    }
    
    getTotal() {
        return this.items.reduce((sum, item) => sum + item.price, 0);
    }
}

// Utilisation
const cart = new ShoppingCart();
cart.addItem({ id: 1, name: 'Product A', price: 29.99 });
cart.addItem({ id: 2, name: 'Product B', price: 49.99 });
console.log(cart.getTotal()); // 79.98

// Exemple 3: Sauvegarder l'état d'un formulaire
function saveFormState(formId) {
    const form = document.getElementById(formId);
    const inputs = form.querySelectorAll('input, textarea, select');
    
    const formData = {};
    inputs.forEach(input => {
        formData[input.name] = input.value;
    });
    
    sessionStorage.setItem(`form_${formId}`, JSON.stringify(formData));
}

function restoreFormState(formId) {
    const savedData = sessionStorage.getItem(`form_${formId}`);
    
    if (savedData) {
        const formData = JSON.parse(savedData);
        const form = document.getElementById(formId);
        
        Object.keys(formData).forEach(name => {
            const input = form.querySelector(`[name="${name}"]`);
            if (input) {
                input.value = formData[name];
            }
        });
    }
}

// Écouter les événements de storage (synchronisation entre onglets)
window.addEventListener('storage', (event) => {
    console.log('Storage changé dans un autre onglet');
    console.log('Clé:', event.key);
    console.log('Ancienne valeur:', event.oldValue);
    console.log('Nouvelle valeur:', event.newValue);
    console.log('URL:', event.url);
});
```

### Différences et Cas d'Usage

```javascript
/*
LOCAL STORAGE:
- Persistant (pas de date d'expiration)
- Capacité: ~5-10 MB selon le navigateur
- Accessible par toutes les fenêtres/onglets du même domaine
- Cas d'usage: préférences, settings, données utilisateur, cache

SESSION STORAGE:
- Temporaire (effacé à la fermeture de l'onglet)
- Capacité: ~5-10 MB selon le navigateur
- Isolé par onglet (pas partagé entre onglets)
- Cas d'usage: état temporaire, données de session, formulaires

COOKIES (pour comparaison):
- Envoyés avec chaque requête HTTP
- Capacité: ~4 KB
- Peuvent avoir une date d'expiration
- Accessibles côté serveur
- Cas d'usage: authentification, tracking
*/

// Vérifier si Web Storage est disponible
function isStorageAvailable(type) {
    try {
        const storage = window[type];
        const test = '__storage_test__';
        storage.setItem(test, test);
        storage.removeItem(test);
        return true;
    } catch (e) {
        return false;
    }
}

if (isStorageAvailable('localStorage')) {
    console.log('localStorage disponible');
} else {
    console.log('localStorage non disponible (mode privé?)');
}
```

---

**Points clés à retenir pour React:**
- Le BOM est essentiel pour gérer les interactions avec le navigateur
- `localStorage`/`sessionStorage` sont très utilisés dans React pour persister l'état
- Les timers (`setTimeout`/`setInterval`) nécessitent un cleanup dans les hooks React
- L'objet `window` est accessible partout mais attention au SSR (Server-Side Rendering)

---

# Questions/Réponses - Chapitre 10 : BOM (Browser Object Model)

## Questions Niveau Junior

### Q1: Quelle est la différence entre `localStorage` et `sessionStorage` ?

**Réponse:**
```javascript
// localStorage - Données persistantes
localStorage.setItem('user', 'John');
// Les données restent même après fermeture du navigateur

// sessionStorage - Données temporaires
sessionStorage.setItem('token', 'abc123');
// Les données sont supprimées à la fermeture de l'onglet

// Autre différence: sessionStorage est isolé par onglet
// localStorage est partagé entre tous les onglets du même domaine
```

**Points clés:**
- `localStorage`: persistant, partagé entre onglets
- `sessionStorage`: temporaire, isolé par onglet
- Les deux ont ~5-10MB de capacité
- Utilisés pour stocker des données côté client sans serveur

---

### Q2: Comment stocker un objet JavaScript dans le `localStorage` ?

**Réponse:**
```javascript
// Mauvaise approche (ne fonctionne pas correctement)
const user = { name: 'Alice', age: 25 };
localStorage.setItem('user', user); // Stocke "[object Object]"

// Bonne approche - Utiliser JSON.stringify()
localStorage.setItem('user', JSON.stringify(user));

// Pour récupérer - Utiliser JSON.parse()
const storedUser = JSON.parse(localStorage.getItem('user'));
console.log(storedUser.name); // "Alice"

// Toujours gérer le cas null
const data = localStorage.getItem('user');
const user = data ? JSON.parse(data) : null;
```

**Points clés:**
- `localStorage` stocke uniquement des strings
- Utiliser `JSON.stringify()` pour convertir
- Utiliser `JSON.parse()` pour récupérer
- Toujours vérifier si la valeur existe avant de parser

---

### Q3: Comment utiliser `setTimeout` et `setInterval` ? Quelle est la différence ?

**Réponse:**
```javascript
// setTimeout - Exécute UNE FOIS après un délai
const timeoutId = setTimeout(() => {
    console.log('Exécuté après 2 secondes');
}, 2000);

// setInterval - Exécute DE MANIÈRE RÉPÉTÉE
const intervalId = setInterval(() => {
    console.log('Exécuté toutes les secondes');
}, 1000);

// IMPORTANT: Toujours nettoyer les timers
clearTimeout(timeoutId);
clearInterval(intervalId);

// Exemple pratique: Compteur
let count = 0;
const id = setInterval(() => {
    count++;
    console.log(count);
    if (count === 5) {
        clearInterval(id); // Arrêter après 5 fois
    }
}, 1000);
```

**Points clés:**
- `setTimeout`: une seule exécution
- `setInterval`: exécutions répétées
- Toujours nettoyer avec `clearTimeout`/`clearInterval`
- Important dans React pour éviter les memory leaks

---

### Q4: Comment recharger la page actuelle en JavaScript ?

**Réponse:**
```javascript
// Méthode 1: location.reload()
window.location.reload(); // Depuis le cache
window.location.reload(true); // Force depuis le serveur (déprécié)

// Méthode 2: Réassigner l'URL
window.location.href = window.location.href;

// Méthode 3: history.go()
window.history.go(0);

// Méthode la plus utilisée
location.reload();
```

**Points clés:**
- Plusieurs méthodes possibles
- `reload()` est la plus claire et recommandée
- Attention: perte des données non sauvegardées

---

## Questions Niveau Intermédiaire

### Q5: Comment détecter si un utilisateur a scrollé en bas de page ?

**Réponse:**
```javascript
window.addEventListener('scroll', () => {
    // Hauteur totale du document
    const scrollHeight = document.documentElement.scrollHeight;
    
    // Position actuelle + hauteur visible
    const scrollTop = window.pageYOffset || document.documentElement.scrollTop;
    const clientHeight = window.innerHeight;
    
    // Vérifier si en bas (avec marge de 50px)
    if (scrollTop + clientHeight >= scrollHeight - 50) {
        console.log('Utilisateur en bas de page');
        // Charger plus de contenu (infinite scroll)
    }
});

// Version plus simple
window.addEventListener('scroll', () => {
    const bottom = window.innerHeight + window.pageYOffset 
                   >= document.body.offsetHeight - 100;
    
    if (bottom) {
        console.log('Bas de page atteint');
    }
});
```

**Points clés:**
- Utiliser `scroll` event listener
- Calculer: position actuelle + hauteur viewport vs hauteur totale
- Ajouter une marge pour déclencher avant la fin
- Utile pour infinite scrolling, lazy loading

---

### Q6: Comment implémenter un système de navigation sans recharger la page (SPA) ?

**Réponse:**
```javascript
// Fonction de navigation
function navigateTo(url) {
    // Mettre à jour l'URL dans la barre d'adresse
    history.pushState(null, '', url);
    
    // Charger le contenu correspondant
    loadContent(url);
}

// Charger le contenu selon l'URL
function loadContent(url) {
    const content = document.getElementById('content');
    
    if (url === '/home') {
        content.innerHTML = '<h1>Home Page</h1>';
    } else if (url === '/about') {
        content.innerHTML = '<h1>About Page</h1>';
    }
}

// Gérer le bouton Précédent/Suivant
window.addEventListener('popstate', () => {
    loadContent(window.location.pathname);
});

// Gérer les clics sur les liens
document.addEventListener('click', (e) => {
    if (e.target.matches('a[data-link]')) {
        e.preventDefault();
        navigateTo(e.target.href);
    }
});

// HTML
// <a href="/home" data-link>Home</a>
// <a href="/about" data-link>About</a>
```

**Points clés:**
- `history.pushState()` change l'URL sans recharger
- `popstate` event pour gérer les boutons navigateur
- C'est le principe de base des frameworks SPA (React Router)
- Améliore l'expérience utilisateur

---

### Q7: Comment synchroniser des données entre plusieurs onglets ?

**Réponse:**
```javascript
// Onglet 1: Modifier une donnée
function updateData(key, value) {
    localStorage.setItem(key, value);
}

// Onglet 2: Écouter les changements
window.addEventListener('storage', (event) => {
    console.log('Changement détecté depuis un autre onglet');
    console.log('Clé:', event.key);
    console.log('Ancienne valeur:', event.oldValue);
    console.log('Nouvelle valeur:', event.newValue);
    
    // Mettre à jour l'interface
    if (event.key === 'theme') {
        applyTheme(event.newValue);
    }
});

// Exemple pratique: Synchroniser le thème
function changeTheme(theme) {
    localStorage.setItem('theme', theme);
    applyTheme(theme);
}

function applyTheme(theme) {
    document.body.className = theme;
}

// Écouter les changements d'autres onglets
window.addEventListener('storage', (e) => {
    if (e.key === 'theme') {
        applyTheme(e.newValue);
    }
});
```

**Points clés:**
- L'événement `storage` se déclenche dans les **autres** onglets
- Ne se déclenche PAS dans l'onglet qui a modifié la donnée
- Utile pour synchroniser: theme, logout, notifications
- Fonctionne uniquement avec `localStorage`, pas `sessionStorage`

---

### Q8: Comment implémenter un scroll fluide vers le haut de page ?

**Réponse:**
```javascript
// Méthode moderne (recommandée)
function scrollToTop() {
    window.scrollTo({
        top: 0,
        behavior: 'smooth'
    });
}

// Avec support navigateurs anciens
function scrollToTopWithFallback() {
    if ('scrollBehavior' in document.documentElement.style) {
        // Navigateur moderne
        window.scrollTo({ top: 0, behavior: 'smooth' });
    } else {
        // Fallback: animation manuelle
        const scrollStep = -window.scrollY / 15;
        const scrollInterval = setInterval(() => {
            if (window.scrollY !== 0) {
                window.scrollBy(0, scrollStep);
            } else {
                clearInterval(scrollInterval);
            }
        }, 15);
    }
}

// Scroll vers un élément spécifique
function scrollToElement(elementId) {
    const element = document.getElementById(elementId);
    element.scrollIntoView({
        behavior: 'smooth',
        block: 'start'
    });
}

// Afficher le bouton uniquement après 300px de scroll
window.addEventListener('scroll', () => {
    const scrollBtn = document.getElementById('scrollBtn');
    
    if (window.pageYOffset > 300) {
        scrollBtn.style.display = 'block';
    } else {
        scrollBtn.style.display = 'none';
    }
});
```

**Points clés:**
- Utiliser `scrollTo({ behavior: 'smooth' })`
- `scrollIntoView()` pour scroller vers un élément
- Afficher le bouton conditionnellement selon la position
- Très utilisé dans les landing pages

---

## Questions Niveau Avancé

### Q9: Comment implémenter un système de cache avec expiration dans `localStorage` ?

**Réponse:**
```javascript
// Classe de gestion du cache
class CacheManager {
    // Sauvegarder avec timestamp
    set(key, value, expirationMinutes = 60) {
        const item = {
            value: value,
            timestamp: Date.now(),
            expiration: expirationMinutes * 60 * 1000 // Convertir en ms
        };
        
        localStorage.setItem(key, JSON.stringify(item));
    }
    
    // Récupérer et vérifier expiration
    get(key) {
        const itemStr = localStorage.getItem(key);
        
        if (!itemStr) {
            return null;
        }
        
        const item = JSON.parse(itemStr);
        const now = Date.now();
        
        // Vérifier si expiré
        if (now - item.timestamp > item.expiration) {
            localStorage.removeItem(key);
            return null;
        }
        
        return item.value;
    }
    
    // Nettoyer les items expirés
    cleanExpired() {
        for (let i = 0; i < localStorage.length; i++) {
            const key = localStorage.key(i);
            this.get(key); // Supprime automatiquement si expiré
        }
    }
}

// Utilisation
const cache = new CacheManager();

// Sauvegarder pour 30 minutes
cache.set('userData', { name: 'John' }, 30);

// Récupérer
const userData = cache.get('userData');
if (userData) {
    console.log('Données valides:', userData);
} else {
    console.log('Cache expiré ou inexistant');
}

// Exemple avec fetch API
async function fetchWithCache(url, cacheMinutes = 60) {
    const cache = new CacheManager();
    const cached = cache.get(url);
    
    if (cached) {
        console.log('Retour depuis le cache');
        return cached;
    }
    
    console.log('Fetch depuis le serveur');
    const response = await fetch(url);
    const data = await response.json();
    
    cache.set(url, data, cacheMinutes);
    return data;
}
```

**Points clés:**
- Stocker timestamp avec les données
- Vérifier expiration à chaque lecture
- Nettoyer périodiquement les données expirées
- Améliore performance et réduit requêtes serveur

---

### Q10: Quels sont les risques de sécurité avec `localStorage` et comment les éviter ?

**Réponse:**
```javascript
/*
RISQUES:
1. XSS (Cross-Site Scripting) - Accès aux données via JS malveillant
2. Pas de protection native - Données en clair
3. Accessible par tout JS sur le domaine
4. Pas de flag httpOnly comme les cookies
*/

// ❌ MAUVAISE PRATIQUE: Stocker des tokens sensibles
localStorage.setItem('authToken', 'eyJhbGc...');
localStorage.setItem('password', '123456');

// ✅ BONNES PRATIQUES:

// 1. NE JAMAIS stocker:
//    - Tokens d'authentification (utiliser httpOnly cookies)
//    - Mots de passe
//    - Données sensibles personnelles
//    - Clés API

// 2. Valider et sanitizer les données
function sanitizeInput(input) {
    const div = document.createElement('div');
    div.textContent = input;
    return div.innerHTML;
}

// 3. Chiffrer les données sensibles (si nécessaire)
function encryptData(data, key) {
    // Utiliser une librairie comme CryptoJS
    // Attention: clé de chiffrement doit être sécurisée
    return btoa(JSON.stringify(data)); // Exemple simple (pas sécurisé)
}

// 4. Implémenter une Content Security Policy (CSP)
// <meta http-equiv="Content-Security-Policy" 
//       content="script-src 'self'">

// 5. Vérifier l'origine des données
function safeGetData(key) {
    try {
        const data = localStorage.getItem(key);
        if (!data) return null;
        
        const parsed = JSON.parse(data);
        
        // Valider la structure
        if (!isValidStructure(parsed)) {
            localStorage.removeItem(key);
            return null;
        }
        
        return parsed;
    } catch (e) {
        console.error('Données corrompues', e);
        localStorage.removeItem(key);
        return null;
    }
}

// 6. Limiter la taille des données
function safeSetData(key, value, maxSize = 1024 * 100) { // 100KB
    const data = JSON.stringify(value);
    
    if (data.length > maxSize) {
        throw new Error('Données trop volumineuses');
    }
    
    localStorage.setItem(key, data);
}
```

**Points clés:**
- **localStorage n'est PAS sécurisé** pour données sensibles
- Vulnérable aux attaques XSS
- Utiliser cookies httpOnly pour tokens d'auth
- Toujours valider/sanitizer les données
- Implémenter CSP pour limiter XSS

---

### Q11: Comment gérer le dépassement de quota du `localStorage` ?

**Réponse:**
```javascript
// Fonction pour vérifier l'espace disponible
function getStorageSize() {
    let total = 0;
    for (let key in localStorage) {
        if (localStorage.hasOwnProperty(key)) {
            total += localStorage[key].length + key.length;
        }
    }
    return (total / 1024).toFixed(2) + ' KB';
}

// Fonction sécurisée pour sauvegarder
function safeSetItem(key, value) {
    try {
        localStorage.setItem(key, value);
        return true;
    } catch (e) {
        if (e.name === 'QuotaExceededError') {
            console.error('Quota dépassé!');
            
            // Stratégie 1: Nettoyer les anciennes données
            cleanOldData();
            
            // Réessayer
            try {
                localStorage.setItem(key, value);
                return true;
            } catch (e) {
                console.error('Impossible de sauvegarder');
                return false;
            }
        }
        return false;
    }
}

// Nettoyer les données anciennes (LRU - Least Recently Used)
function cleanOldData() {
    const items = [];
    
    // Récupérer toutes les données avec timestamp
    for (let i = 0; i < localStorage.length; i++) {
        const key = localStorage.key(i);
        try {
            const item = JSON.parse(localStorage.getItem(key));
            if (item.timestamp) {
                items.push({ key, timestamp: item.timestamp });
            }
        } catch (e) {
            // Ignorer les items non-JSON
        }
    }
    
    // Trier par timestamp (plus ancien en premier)
    items.sort((a, b) => a.timestamp - b.timestamp);
    
    // Supprimer les 25% plus anciens
    const toRemove = Math.ceil(items.length * 0.25);
    for (let i = 0; i < toRemove; i++) {
        localStorage.removeItem(items[i].key);
    }
}

// Wrapper avec gestion automatique
class SmartStorage {
    set(key, value, priority = 'normal') {
        const item = {
            value: value,
            timestamp: Date.now(),
            priority: priority,
            accessCount: 0
        };
        
        const success = safeSetItem(key, JSON.stringify(item));
        
        if (!success && priority === 'high') {
            // Si priorité haute, supprimer données basse priorité
            this.removeLowPriority();
            safeSetItem(key, JSON.stringify(item));
        }
        
        return success;
    }
    
    get(key) {
        const itemStr = localStorage.getItem(key);
        if (!itemStr) return null;
        
        try {
            const item = JSON.parse(itemStr);
            // Incrémenter le compteur d'accès
            item.accessCount++;
            localStorage.setItem(key, JSON.stringify(item));
            return item.value;
        } catch (e) {
            return null;
        }
    }
    
    removeLowPriority() {
        for (let i = 0; i < localStorage.length; i++) {
            const key = localStorage.key(i);
            try {
                const item = JSON.parse(localStorage.getItem(key));
                if (item.priority === 'low') {
                    localStorage.removeItem(key);
                }
            } catch (e) {}
        }
    }
}

// Utilisation
const storage = new SmartStorage();
storage.set('criticalData', { important: true }, 'high');
storage.set('tempData', { temp: true }, 'low');
```

**Points clés:**
- Quota typique: 5-10MB selon navigateur
- `QuotaExceededError` quand dépassé
- Implémenter stratégie de nettoyage (LRU, priorité)
- Toujours utiliser try-catch
- Surveiller l'utilisation du storage

---

**Conseil pour l'entretien:**
- Démontrer que tu comprends les **limites** et **risques**
- Expliquer les **use cases appropriés** vs inappropriés
- Montrer que tu connais les **alternatives** (IndexedDB, cookies, server storage)
- Faire le lien avec **React** (useEffect cleanup, custom hooks)