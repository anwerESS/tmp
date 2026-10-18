## [**..**](./00_index.md)

# Chapitre 16 : Programmation Asynchrone - Guide Détaillé

## 16.1 Introduction à l'Asynchrone

### Call Stack, Event Loop et Task Queue

JavaScript est **single-threaded** (mono-thread), ce qui signifie qu'il ne peut exécuter qu'une seule chose à la fois. L'**Event Loop** permet de gérer les opérations asynchrones.

```javascript
// Call Stack : pile d'exécution des fonctions
console.log('1'); // Exécuté immédiatement

setTimeout(() => {
  console.log('2'); // Exécuté après (macrotask)
}, 0);

Promise.resolve().then(() => {
  console.log('3'); // Exécuté avant setTimeout (microtask)
});

console.log('4'); // Exécuté immédiatement

// Ordre d'affichage : 1, 4, 3, 2
// Pourquoi ? 
// - Le code synchrone s'exécute d'abord (1, 4)
// - Les microtasks (Promises) ont la priorité sur les macrotasks (setTimeout)
```

### Microtasks vs Macrotasks

```javascript
// Macrotasks : setTimeout, setInterval, I/O operations
// Microtasks : Promises, queueMicrotask, MutationObserver

console.log('Start');

// Macrotask
setTimeout(() => {
  console.log('setTimeout 1');
}, 0);

// Microtask
Promise.resolve().then(() => {
  console.log('Promise 1');
});

// Macrotask
setTimeout(() => {
  console.log('setTimeout 2');
}, 0);

// Microtask
Promise.resolve().then(() => {
  console.log('Promise 2');
});

console.log('End');

// Résultat : Start, End, Promise 1, Promise 2, setTimeout 1, setTimeout 2
// Les microtasks sont toujours traitées avant les macrotasks
```

---

## 16.2 Callbacks

Les **callbacks** sont des fonctions passées en argument à d'autres fonctions, exécutées plus tard.

```javascript
// Callback simple
function chargerDonnees(callback) {
  console.log('Chargement des données...');
  
  // Simulation d'une opération asynchrone
  setTimeout(() => {
    const donnees = { id: 1, nom: 'Utilisateur' };
    callback(donnees); // Appel du callback quand les données sont prêtes
  }, 2000);
}

// Utilisation
chargerDonnees((resultat) => {
  console.log('Données reçues :', resultat);
});

console.log('En attente...'); // S'affiche avant les données
```

### Callback Hell (Pyramide de la mort)

```javascript
// Problème : code difficile à lire et maintenir
chargerUtilisateur(userId, (utilisateur) => {
  chargerCommandes(utilisateur.id, (commandes) => {
    chargerDetails(commandes[0].id, (details) => {
      chargerProduits(details.produitId, (produits) => {
        console.log('Produits :', produits);
        // Encore plus de niveaux possibles...
      });
    });
  });
});

// Le code devient illisible avec trop de niveaux d'imbrication
```

### Error Handling avec Callbacks

```javascript
// Convention : premier paramètre = erreur, second = données
function lireFichier(nomFichier, callback) {
  setTimeout(() => {
    const erreur = nomFichier ? null : new Error('Nom de fichier manquant');
    const donnees = erreur ? null : 'Contenu du fichier';
    
    callback(erreur, donnees); // Pattern error-first callback
  }, 1000);
}

// Utilisation avec gestion d'erreur
lireFichier('document.txt', (err, donnees) => {
  if (err) {
    console.error('Erreur :', err.message);
    return;
  }
  console.log('Données :', donnees);
});
```

---

## 16.3 Promises

Les **Promises** représentent une valeur qui peut être disponible maintenant, plus tard, ou jamais.

### Création et États

```javascript
// Une Promise a 3 états :
// - pending (en attente)
// - fulfilled (résolue avec succès)
// - rejected (rejetée avec erreur)

const maPromise = new Promise((resolve, reject) => {
  // resolve : fonction pour réussir
  // reject : fonction pour échouer
  
  const succes = true;
  
  setTimeout(() => {
    if (succes) {
      resolve('Opération réussie !'); // Change l'état à fulfilled
    } else {
      reject('Erreur survenue'); // Change l'état à rejected
    }
  }, 1500);
});

console.log(maPromise); // Promise { <pending> }
```

### .then(), .catch(), .finally()

```javascript
// .then() pour gérer le succès
// .catch() pour gérer les erreurs
// .finally() exécuté dans tous les cas

maPromise
  .then((resultat) => {
    console.log('Succès :', resultat);
    return 'Valeur transformée'; // Retourne une nouvelle valeur
  })
  .then((nouvelleValeur) => {
    console.log('Chaînage :', nouvelleValeur);
  })
  .catch((erreur) => {
    console.error('Erreur attrapée :', erreur);
  })
  .finally(() => {
    console.log('Terminé (succès ou erreur)');
  });
```

### Chaining Promises (Chaînage)

```javascript
// Résolution du callback hell avec des Promises
function chargerUtilisateur(id) {
  return new Promise((resolve) => {
    setTimeout(() => {
      resolve({ id: id, nom: 'Jean', commandeIds: [101, 102] });
    }, 1000);
  });
}

function chargerCommandes(ids) {
  return new Promise((resolve) => {
    setTimeout(() => {
      resolve(ids.map(id => ({ id, montant: 50 })));
    }, 1000);
  });
}

function calculerTotal(commandes) {
  return new Promise((resolve) => {
    const total = commandes.reduce((sum, cmd) => sum + cmd.montant, 0);
    resolve(total);
  });
}

// Chaînage propre et lisible
chargerUtilisateur(1)
  .then(utilisateur => {
    console.log('Utilisateur :', utilisateur.nom);
    return chargerCommandes(utilisateur.commandeIds);
  })
  .then(commandes => {
    console.log('Commandes :', commandes);
    return calculerTotal(commandes);
  })
  .then(total => {
    console.log('Total :', total);
  })
  .catch(erreur => {
    console.error('Erreur dans la chaîne :', erreur);
  });
```

### Error Handling avec Promises

```javascript
function operationRisquee(valeur) {
  return new Promise((resolve, reject) => {
    if (valeur > 0) {
      resolve(valeur * 2);
    } else {
      reject(new Error('Valeur doit être positive'));
    }
  });
}

// Gestion d'erreur à chaque étape
operationRisquee(5)
  .then(resultat => {
    console.log('Étape 1 :', resultat); // 10
    return operationRisquee(-3); // Va échouer
  })
  .then(resultat => {
    console.log('Étape 2 :', resultat); // Ne s'exécutera pas
  })
  .catch(erreur => {
    console.error('Erreur :', erreur.message); // Attrape l'erreur
    return 0; // Récupération : retourne une valeur par défaut
  })
  .then(resultat => {
    console.log('Récupération :', resultat); // 0
  });
```

---

## 16.4 Méthodes Statiques des Promises

### Promise.all()

```javascript
// Attend que TOUTES les Promises soient résolues
// Si UNE seule échoue, tout échoue

const promise1 = Promise.resolve(10);
const promise2 = new Promise(resolve => setTimeout(() => resolve(20), 1000));
const promise3 = fetch('https://api.exemple.com/data').then(r => r.json());

Promise.all([promise1, promise2, promise3])
  .then(resultats => {
    // resultats est un array des valeurs dans l'ordre
    console.log('Toutes réussies :', resultats);
    // [10, 20, {...donnéesAPI}]
  })
  .catch(erreur => {
    console.error('Au moins une a échoué :', erreur);
  });

// Cas d'usage : charger plusieurs ressources en parallèle
const utilisateurs = fetch('/api/utilisateurs');
const produits = fetch('/api/produits');
const categories = fetch('/api/categories');

Promise.all([utilisateurs, produits, categories])
  .then(responses => Promise.all(responses.map(r => r.json())))
  .then(([users, prods, cats]) => {
    console.log('Tout chargé :', { users, prods, cats });
  });
```

### Promise.allSettled()

```javascript
// Attend que TOUTES soient terminées (succès ou échec)
// Ne rejette JAMAIS, retourne le statut de chacune

const p1 = Promise.resolve(100);
const p2 = Promise.reject('Erreur ici');
const p3 = new Promise(resolve => setTimeout(() => resolve(300), 500));

Promise.allSettled([p1, p2, p3])
  .then(resultats => {
    resultats.forEach((resultat, index) => {
      if (resultat.status === 'fulfilled') {
        console.log(`Promise ${index} réussie :`, resultat.value);
      } else {
        console.log(`Promise ${index} échouée :`, resultat.reason);
      }
    });
    
    // Résultat :
    // [
    //   { status: 'fulfilled', value: 100 },
    //   { status: 'rejected', reason: 'Erreur ici' },
    //   { status: 'fulfilled', value: 300 }
    // ]
  });

// Cas d'usage : tentatives multiples sans bloquer sur les échecs
```

### Promise.race()

```javascript
// Retourne la PREMIÈRE Promise terminée (succès ou échec)

const lente = new Promise(resolve => setTimeout(() => resolve('Lent'), 3000));
const rapide = new Promise(resolve => setTimeout(() => resolve('Rapide'), 1000));
const erreur = new Promise((_, reject) => setTimeout(() => reject('Erreur'), 2000));

Promise.race([lente, rapide, erreur])
  .then(resultat => {
    console.log('Première terminée :', resultat); // "Rapide"
  })
  .catch(err => {
    console.error('Première échouée :', err);
  });

// Cas d'usage : timeout pour requêtes
function fetchAvecTimeout(url, timeout = 5000) {
  const fetchPromise = fetch(url);
  const timeoutPromise = new Promise((_, reject) => 
    setTimeout(() => reject(new Error('Timeout dépassé')), timeout)
  );
  
  return Promise.race([fetchPromise, timeoutPromise]);
}

fetchAvecTimeout('https://api.lente.com/data', 3000)
  .then(response => response.json())
  .then(data => console.log('Données :', data))
  .catch(err => console.error('Erreur :', err.message));
```

### Promise.any()

```javascript
// Retourne la PREMIÈRE Promise réussie
// Rejette seulement si TOUTES échouent

const p1 = Promise.reject('Échec 1');
const p2 = new Promise(resolve => setTimeout(() => resolve('Succès 2'), 2000));
const p3 = new Promise(resolve => setTimeout(() => resolve('Succès 3'), 1000));

Promise.any([p1, p2, p3])
  .then(resultat => {
    console.log('Première réussie :', resultat); // "Succès 3" (la plus rapide)
  })
  .catch(erreur => {
    // AggregateError : toutes ont échoué
    console.error('Toutes ont échoué :', erreur.errors);
  });

// Cas d'usage : sources multiples (miroirs, fallbacks)
const serveurPrincipal = fetch('https://api-principale.com/data');
const serveurSecondaire = fetch('https://api-backup.com/data');
const cache = fetch('/cache/data');

Promise.any([serveurPrincipal, serveurSecondaire, cache])
  .then(response => response.json())
  .then(data => console.log('Données depuis la source la plus rapide :', data));
```

---

## 16.5 Async/Await

**Async/Await** est une syntaxe moderne pour travailler avec les Promises de manière plus lisible.

### Syntaxe de Base

```javascript
// async : déclare une fonction asynchrone
// await : attend la résolution d'une Promise

async function chargerDonnees() {
  // Une fonction async retourne TOUJOURS une Promise
  return 'Données chargées';
}

chargerDonnees().then(resultat => console.log(resultat));

// Équivalent à :
function chargerDonneesPromise() {
  return Promise.resolve('Données chargées');
}
```

### Keyword await

```javascript
function attendre(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

async function exemple() {
  console.log('Début');
  
  // await suspend l'exécution jusqu'à la résolution
  await attendre(2000); // Attend 2 secondes
  
  console.log('2 secondes passées');
  
  const resultat = await attendre(1000).then(() => 'Terminé');
  console.log(resultat);
  
  return 'Fonction terminée';
}

exemple().then(msg => console.log(msg));
```

### Error Handling avec try...catch

```javascript
async function chargerUtilisateur(id) {
  if (!id) {
    throw new Error('ID manquant');
  }
  
  // Simulation d'API
  await attendre(1000);
  return { id, nom: 'Marie' };
}

async function traiterUtilisateur(id) {
  try {
    console.log('Chargement...');
    
    const utilisateur = await chargerUtilisateur(id);
    console.log('Utilisateur chargé :', utilisateur);
    
    const commandes = await chargerCommandes(utilisateur.id);
    console.log('Commandes chargées :', commandes);
    
    return { utilisateur, commandes };
    
  } catch (erreur) {
    // Attrape les erreurs de toutes les opérations await
    console.error('Erreur lors du traitement :', erreur.message);
    
    // Option : retourner une valeur par défaut
    return null;
    
  } finally {
    // Toujours exécuté (nettoyage, logs, etc.)
    console.log('Traitement terminé');
  }
}

// Utilisation
traiterUtilisateur(123);
traiterUtilisateur(null); // Provoquera une erreur
```

### Async/Await vs Promises

```javascript
// AVEC PROMISES (chaînage)
function avecPromises() {
  return chargerUtilisateur(1)
    .then(utilisateur => {
      console.log('Utilisateur :', utilisateur);
      return chargerCommandes(utilisateur.id);
    })
    .then(commandes => {
      console.log('Commandes :', commandes);
      return calculerTotal(commandes);
    })
    .then(total => {
      console.log('Total :', total);
      return total;
    })
    .catch(erreur => {
      console.error('Erreur :', erreur);
    });
}

// AVEC ASYNC/AWAIT (séquentiel, plus lisible)
async function avecAsyncAwait() {
  try {
    const utilisateur = await chargerUtilisateur(1);
    console.log('Utilisateur :', utilisateur);
    
    const commandes = await chargerCommandes(utilisateur.id);
    console.log('Commandes :', commandes);
    
    const total = await calculerTotal(commandes);
    console.log('Total :', total);
    
    return total;
    
  } catch (erreur) {
    console.error('Erreur :', erreur);
  }
}

// Les deux approches sont équivalentes, mais async/await est plus lisible
```

### Exécution Parallèle avec Promise.all()

```javascript
// ❌ SÉQUENTIEL (lent) - chaque await attend le précédent
async function chargerSequentiel() {
  const user1 = await fetch('/api/users/1').then(r => r.json()); // 1s
  const user2 = await fetch('/api/users/2').then(r => r.json()); // 1s
  const user3 = await fetch('/api/users/3').then(r => r.json()); // 1s
  // Temps total : 3 secondes
  return [user1, user2, user3];
}

// ✅ PARALLÈLE (rapide) - toutes les requêtes en même temps
async function chargerParallele() {
  // Les Promises démarrent immédiatement
  const promise1 = fetch('/api/users/1').then(r => r.json());
  const promise2 = fetch('/api/users/2').then(r => r.json());
  const promise3 = fetch('/api/users/3').then(r => r.json());
  
  // await sur Promise.all attend que TOUTES soient terminées
  const [user1, user2, user3] = await Promise.all([promise1, promise2, promise3]);
  // Temps total : ~1 seconde (la plus longue)
  
  return [user1, user2, user3];
}

// Autre syntaxe pour parallélisation
async function chargerParallele2() {
  const resultats = await Promise.all([
    fetch('/api/users/1').then(r => r.json()),
    fetch('/api/users/2').then(r => r.json()),
    fetch('/api/users/3').then(r => r.json())
  ]);
  
  return resultats;
}
```

### Patterns Avancés

```javascript
// Pattern : boucle avec await (séquentiel)
async function traiterElements(items) {
  const resultats = [];
  
  for (const item of items) {
    // Chaque itération attend la précédente
    const resultat = await traiterItem(item);
    resultats.push(resultat);
  }
  
  return resultats;
}

// Pattern : map + Promise.all (parallèle)
async function traiterElementsParallele(items) {
  // Toutes les opérations démarrent en même temps
  const promises = items.map(item => traiterItem(item));
  const resultats = await Promise.all(promises);
  return resultats;
}

// Pattern : await conditionnel
async function chargerAvecCache(id) {
  const cache = localStorage.getItem(`user_${id}`);
  
  if (cache) {
    // Pas besoin d'await pour des données synchrones
    return JSON.parse(cache);
  }
  
  // await seulement si nécessaire
  const utilisateur = await fetch(`/api/users/${id}`).then(r => r.json());
  localStorage.setItem(`user_${id}`, JSON.stringify(utilisateur));
  
  return utilisateur;
}
```

---

## 16.6 Fetch API et AJAX

La **Fetch API** permet de faire des requêtes HTTP de manière moderne.

### Introduction à fetch()

```javascript
// fetch() retourne une Promise
// Syntaxe : fetch(url, options)

fetch('https://api.exemple.com/users')
  .then(response => {
    // response est un objet Response
    console.log('Status :', response.status); // 200, 404, 500, etc.
    console.log('OK :', response.ok); // true si status 200-299
    console.log('Headers :', response.headers);
    
    return response.json(); // Parse le JSON
  })
  .then(data => {
    console.log('Données :', data);
  })
  .catch(erreur => {
    console.error('Erreur réseau :', erreur);
  });
```

### GET Requests

```javascript
// Requête GET simple
async function chargerUtilisateurs() {
  try {
    const response = await fetch('https://jsonplaceholder.typicode.com/users');
    
    // Vérifier le status
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    
    const utilisateurs = await response.json();
    console.log('Utilisateurs :', utilisateurs);
    
    return utilisateurs;
    
  } catch (erreur) {
    console.error('Erreur lors du chargement :', erreur.message);
  }
}

// GET avec paramètres (query string)
async function rechercherUtilisateurs(terme) {
  const params = new URLSearchParams({
    q: terme,
    limit: 10,
    sort: 'name'
  });
  
  const url = `https://api.exemple.com/users?${params.toString()}`;
  // URL finale : https://api.exemple.com/users?q=terme&limit=10&sort=name
  
  const response = await fetch(url);
  return await response.json();
}
```

### POST Requests

```javascript
// Requête POST avec JSON
async function creerUtilisateur(utilisateur) {
  try {
    const response = await fetch('https://jsonplaceholder.typicode.com/users', {
      method: 'POST', // Méthode HTTP
      headers: {
        'Content-Type': 'application/json', // Type de contenu
        'Authorization': 'Bearer token123' // Token d'authentification
      },
      body: JSON.stringify(utilisateur) // Conversion objet -> JSON string
    });
    
    if (!response.ok) {
      throw new Error(`Erreur ${response.status}: ${response.statusText}`);
    }
    
    const nouveauUtilisateur = await response.json();
    console.log('Créé :', nouveauUtilisateur);
    
    return nouveauUtilisateur;
    
  } catch (erreur) {
    console.error('Erreur création :', erreur.message);
  }
}

// Utilisation
creerUtilisateur({
  nom: 'Dupont',
  email: 'dupont@exemple.com',
  age: 30
});

// PUT (mise à jour complète)
async function mettreAJourUtilisateur(id, data) {
  const response = await fetch(`https://api.exemple.com/users/${id}`, {
    method: 'PUT',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(data)
  });
  return await response.json();
}

// PATCH (mise à jour partielle)
async function modifierUtilisateur(id, modifications) {
  const response = await fetch(`https://api.exemple.com/users/${id}`, {
    method: 'PATCH',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(modifications)
  });
  return await response.json();
}

// DELETE
async function supprimerUtilisateur(id) {
  const response = await fetch(`https://api.exemple.com/users/${id}`, {
    method: 'DELETE'
  });
  return response.ok;
}
```

### Handling Responses

```javascript
async function traiterReponse(url) {
  const response = await fetch(url);
  
  // Différentes méthodes selon le type de contenu
  
  // JSON (le plus courant)
  const json = await response.json();
  
  // Texte brut
  const texte = await response.text();
  
  // Blob (fichiers binaires, images, etc.)
  const blob = await response.blob();
  const imageUrl = URL.createObjectURL(blob);
  
  // ArrayBuffer (données binaires brutes)
  const buffer = await response.arrayBuffer();
  
  // FormData
  const formData = await response.formData();
  
  return json;
}

// Exemple : télécharger une image
async function telechargerImage(url) {
  try {
    const response = await fetch(url);
    
    if (!response.ok) {
      throw new Error('Image non trouvée');
    }
    
    const blob = await response.blob();
    const imageUrl = URL.createObjectURL(blob);
    
    // Afficher dans une balise <img>
    const img = document.createElement('img');
    img.src = imageUrl;
    document.body.appendChild(img);
    
  } catch (erreur) {
    console.error('Erreur téléchargement :', erreur);
  }
}
```

### Error Handling avec fetch

```javascript
// ⚠️ fetch() ne rejette QUE sur les erreurs réseau
// Les statuts 404, 500, etc. NE sont PAS des rejections

async function fetchAvecGestionErreurs(url) {
  try {
    const response = await fetch(url);
    
    // Vérification manuelle du status
    if (!response.ok) {
      // Créer une erreur pour les statuts non-ok
      throw new Error(`HTTP ${response.status}: ${response.statusText}`);
    }
    
    // Vérifier le Content-Type avant de parser
    const contentType = response.headers.get('content-type');
    
    if (contentType && contentType.includes('application/json')) {
      return await response.json();
    } else {
      return await response.text();
    }
    
  } catch (erreur) {
    // Différencier les types d'erreurs
    if (erreur.name === 'TypeError') {
      console.error('Erreur réseau ou CORS :', erreur.message);
    } else {
      console.error('Erreur HTTP :', erreur.message);
    }
    
    throw erreur; // Re-throw pour que l'appelant puisse gérer
  }
}

// Fonction helper pour validation
async function fetchJSON(url, options = {}) {
  const response = await fetch(url, options);
  
  if (!response.ok) {
    const erreur = await response.text();
    throw new Error(`${response.status}: ${erreur}`);
  }
  
  return await response.json();
}
```

### Headers et Options

```javascript
// Configuration complète d'une requête fetch
async function requeteComplete() {
  const options = {
    method: 'POST',
    
    // Headers personnalisés
    headers: {
      'Content-Type': 'application/json',
      'Authorization': 'Bearer mon-token',
      'X-Custom-Header': 'valeur'
    },
    
    // Corps de la requête
    body: JSON.stringify({ nom: 'Test' }),
    
    // Mode CORS
    mode: 'cors', // 'cors', 'no-cors', 'same-origin'
    
    // Credentials (cookies)
    credentials: 'include', // 'include', 'same-origin', 'omit'
    
    // Cache
    cache: 'default', // 'default', 'no-cache', 'reload', 'force-cache'
    
    // Redirect
    redirect: 'follow', // 'follow', 'error', 'manual'
    
    // Referrer
    referrer: 'client',
    
    // Signal pour annulation
    signal: AbortSignal.timeout(5000) // Timeout de 5 secondes
  };
  
  const response = await fetch('https://api.exemple.com/data', options);
  return await response.json();
}

// Lecture des headers de réponse
async function lireHeaders(url) {
  const response = await fetch(url);
  
  // Accès individuel
  const contentType = response.headers.get('Content-Type');
  const dateResponse = response.headers.get('Date');
  
  console.log('Content-Type :', contentType);
  console.log('Date :', dateResponse);
  
  // Itérer sur tous les headers
  for (const [key, value] of response.headers) {
    console.log(`${key}: ${value}`);
  }
}
```

### CORS Basics

```javascript
// CORS (Cross-Origin Resource Sharing) : 
// politique de sécurité des navigateurs

// ❌ Bloqué par CORS
fetch('https://autre-domaine.com/api/data')
  .then(response => response.json())
  .catch(erreur => {
    // Error: CORS policy: No 'Access-Control-Allow-Origin' header
    console.error('CORS bloqué :', erreur);
  });

// ✅ Solution côté serveur (backend)
// Le serveur doit envoyer les headers appropriés :
// Access-Control-Allow-Origin: *
// Access-Control-Allow-Methods: GET, POST, PUT, DELETE
// Access-Control-Allow-Headers: Content-Type, Authorization

// Solution temporaire en développement : proxy
// Dans package.json (React) :
// "proxy": "https://api.exemple.com"

// Puis fetch relatif :
fetch('/api/data') // Redirigé vers https://api.exemple.com/api/data
  .then(response => response.json())
  .then(data => console.log(data));

// Mode no-cors (limité)
fetch('https://autre-domaine.com/api/data', {
  mode: 'no-cors' // Pas d'accès aux données de réponse
})
.then(response => {
  // response.body est null en mode no-cors
  console.log('Requête envoyée mais réponse inaccessible');
});
```

### Patterns Pratiques

```javascript
// Pattern : Retry avec backoff
async function fetchAvecRetry(url, maxRetries = 3) {
  for (let i = 0; i < maxRetries; i++) {
    try {
      const response = await fetch(url);
      
      if (response.ok) {
        return await response.json();
      }
      
      // Statut non-ok mais pas une erreur réseau
      if (i === maxRetries - 1) {
        throw new Error(`Failed after ${maxRetries} attempts`);
      }
      
    } catch (erreur) {
      if (i === maxRetries - 1) {
        throw erreur;
      }
      
      // Attendre avant de réessayer (backoff exponentiel)
      const delai = Math.pow(2, i) * 1000; // 1s, 2s, 4s...
      console.log(`Tentative ${i + 1} échouée, nouvelle tentative dans ${delai}ms`);
      await new Promise(resolve => setTimeout(resolve, delai));
    }
  }
}

// Utilisation
fetchAvecRetry('https://api.instable.com/data')
  .then(data => console.log('Données récupérées :', data))
  .catch(err => console.error('Échec définitif :', err));
```

```javascript
// Pattern : Annulation de requête avec AbortController
async function requeteAnnulable() {
  const controller = new AbortController();
  const signal = controller.signal;
  
  // Annuler après 5 secondes
  setTimeout(() => {
    controller.abort();
    console.log('Requête annulée');
  }, 5000);
  
  try {
    const response = await fetch('https://api.lente.com/data', {
      signal: signal // Passer le signal à fetch
    });
    
    const data = await response.json();
    console.log('Données :', data);
    
  } catch (erreur) {
    if (erreur.name === 'AbortError') {
      console.log('Requête interrompue par l\'utilisateur');
    } else {
      console.error('Autre erreur :', erreur);
    }
  }
}

// Pattern : Bouton d'annulation
let currentController = null;

function lancerRecherche(terme) {
  // Annuler la recherche précédente si elle existe
  if (currentController) {
    currentController.abort();
  }
  
  currentController = new AbortController();
  
  fetch(`https://api.exemple.com/search?q=${terme}`, {
    signal: currentController.signal
  })
  .then(response => response.json())
  .then(resultats => {
    console.log('Résultats :', resultats);
    currentController = null;
  })
  .catch(erreur => {
    if (erreur.name !== 'AbortError') {
      console.error('Erreur :', erreur);
    }
  });
}

// Chaque nouvelle recherche annule la précédente
lancerRecherche('react');
lancerRecherche('javascript'); // Annule la recherche "react"
```

```javascript
// Pattern : Cache simple pour les requêtes
const cache = new Map();

async function fetchAvecCache(url, ttl = 60000) { // TTL = 60 secondes
  const maintenant = Date.now();
  
  // Vérifier si en cache et toujours valide
  if (cache.has(url)) {
    const { data, timestamp } = cache.get(url);
    
    if (maintenant - timestamp < ttl) {
      console.log('Données du cache');
      return data;
    }
  }
  
  // Fetch si pas en cache ou expiré
  console.log('Fetch depuis API');
  const response = await fetch(url);
  const data = await response.json();
  
  // Mettre en cache
  cache.set(url, {
    data: data,
    timestamp: maintenant
  });
  
  return data;
}

// Utilisation
await fetchAvecCache('https://api.exemple.com/users'); // Fetch API
await fetchAvecCache('https://api.exemple.com/users'); // Depuis cache
```

```javascript
// Pattern : Upload de fichier
async function uploaderFichier(fichier) {
  const formData = new FormData();
  formData.append('file', fichier);
  formData.append('description', 'Mon fichier');
  formData.append('userId', 123);
  
  try {
    const response = await fetch('https://api.exemple.com/upload', {
      method: 'POST',
      body: formData // Ne PAS définir Content-Type, le navigateur le fait
      // headers: { 'Content-Type': 'multipart/form-data' } ❌ Ne pas faire
    });
    
    if (!response.ok) {
      throw new Error('Échec de l\'upload');
    }
    
    const resultat = await response.json();
    console.log('Fichier uploadé :', resultat);
    
    return resultat;
    
  } catch (erreur) {
    console.error('Erreur upload :', erreur);
  }
}

// Avec HTML input file
// <input type="file" id="fileInput">
document.getElementById('fileInput').addEventListener('change', (e) => {
  const fichier = e.target.files[0];
  if (fichier) {
    uploaderFichier(fichier);
  }
});
```

```javascript
// Pattern : Progress tracking (avec XMLHttpRequest car fetch ne supporte pas)
function uploaderAvecProgress(fichier, onProgress) {
  return new Promise((resolve, reject) => {
    const xhr = new XMLHttpRequest();
    const formData = new FormData();
    formData.append('file', fichier);
    
    // Event listener pour progression
    xhr.upload.addEventListener('progress', (e) => {
      if (e.lengthComputable) {
        const pourcentage = (e.loaded / e.total) * 100;
        onProgress(pourcentage);
      }
    });
    
    // Event listener pour completion
    xhr.addEventListener('load', () => {
      if (xhr.status === 200) {
        resolve(JSON.parse(xhr.responseText));
      } else {
        reject(new Error(`HTTP ${xhr.status}`));
      }
    });
    
    // Event listener pour erreur
    xhr.addEventListener('error', () => {
      reject(new Error('Erreur réseau'));
    });
    
    xhr.open('POST', 'https://api.exemple.com/upload');
    xhr.send(formData);
  });
}

// Utilisation
uploaderAvecProgress(fichier, (progress) => {
  console.log(`Upload : ${progress.toFixed(2)}%`);
})
.then(resultat => console.log('Terminé :', resultat))
.catch(err => console.error('Erreur :', err));
```

```javascript
// Pattern : Requêtes parallèles avec gestion individuelle des erreurs
async function chargerDonneesMultiples() {
  // On veut charger plusieurs ressources mais continuer même si certaines échouent
  
  const promises = [
    fetch('https://api.exemple.com/users').then(r => r.json()).catch(e => ({ error: e.message })),
    fetch('https://api.exemple.com/posts').then(r => r.json()).catch(e => ({ error: e.message })),
    fetch('https://api.exemple.com/comments').then(r => r.json()).catch(e => ({ error: e.message }))
  ];
  
  const [users, posts, comments] = await Promise.all(promises);
  
  // Vérifier les erreurs individuellement
  if (users.error) {
    console.error('Erreur users :', users.error);
    // Utiliser des données par défaut
    users = [];
  }
  
  if (posts.error) {
    console.error('Erreur posts :', posts.error);
    posts = [];
  }
  
  if (comments.error) {
    console.error('Erreur comments :', comments.error);
    comments = [];
  }
  
  return { users, posts, comments };
}
```

```javascript
// Pattern : Polling (vérification périodique)
async function pollStatus(orderId, intervalle = 2000, maxAttempts = 30) {
  for (let i = 0; i < maxAttempts; i++) {
    try {
      const response = await fetch(`https://api.exemple.com/orders/${orderId}/status`);
      const data = await response.json();
      
      console.log(`Tentative ${i + 1}: Status = ${data.status}`);
      
      // Si terminé, retourner le résultat
      if (data.status === 'completed' || data.status === 'failed') {
        return data;
      }
      
      // Attendre avant la prochaine vérification
      await new Promise(resolve => setTimeout(resolve, intervalle));
      
    } catch (erreur) {
      console.error('Erreur polling :', erreur);
    }
  }
  
  throw new Error('Timeout : nombre maximum de tentatives atteint');
}

// Utilisation
pollStatus('order-123')
  .then(result => console.log('Commande terminée :', result))
  .catch(err => console.error('Échec :', err));
```

```javascript
// Pattern : Batch requests (requêtes par lots)
async function traiterParLots(items, batchSize = 5) {
  const resultats = [];
  
  // Diviser en lots
  for (let i = 0; i < items.length; i += batchSize) {
    const lot = items.slice(i, i + batchSize);
    
    console.log(`Traitement du lot ${i / batchSize + 1}...`);
    
    // Traiter le lot en parallèle
    const promisesLot = lot.map(item => 
      fetch(`https://api.exemple.com/process`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(item)
      })
      .then(r => r.json())
    );
    
    const resultatsLot = await Promise.all(promisesLot);
    resultats.push(...resultatsLot);
    
    // Pause entre les lots pour éviter de surcharger l'API
    if (i + batchSize < items.length) {
      await new Promise(resolve => setTimeout(resolve, 1000));
    }
  }
  
  return resultats;
}

// Traiter 100 items par lots de 5
const items = Array.from({ length: 100 }, (_, i) => ({ id: i }));
traiterParLots(items, 5)
  .then(resultats => console.log('Tous traités :', resultats.length));
```

---

## Exemples Pratiques Complets

### Exemple 1 : Gestionnaire d'API avec classe

```javascript
class APIClient {
  constructor(baseURL, options = {}) {
    this.baseURL = baseURL;
    this.defaultOptions = {
      headers: {
        'Content-Type': 'application/json',
        ...options.headers
      },
      ...options
    };
  }
  
  // Méthode helper pour construire l'URL
  buildURL(endpoint, params = {}) {
    const url = new URL(endpoint, this.baseURL);
    Object.keys(params).forEach(key => 
      url.searchParams.append(key, params[key])
    );
    return url.toString();
  }
  
  // Méthode helper pour les requêtes
  async request(endpoint, options = {}) {
    const url = this.buildURL(endpoint);
    
    const config = {
      ...this.defaultOptions,
      ...options,
      headers: {
        ...this.defaultOptions.headers,
        ...options.headers
      }
    };
    
    try {
      const response = await fetch(url, config);
      
      if (!response.ok) {
        const error = await response.text();
        throw new Error(`HTTP ${response.status}: ${error}`);
      }
      
      // Vérifier si la réponse contient du JSON
      const contentType = response.headers.get('content-type');
      if (contentType && contentType.includes('application/json')) {
        return await response.json();
      }
      
      return await response.text();
      
    } catch (erreur) {
      console.error(`Erreur API [${endpoint}]:`, erreur.message);
      throw erreur;
    }
  }
  
  // Méthodes HTTP
  async get(endpoint, params = {}) {
    const url = this.buildURL(endpoint, params);
    return this.request(url);
  }
  
  async post(endpoint, data) {
    return this.request(endpoint, {
      method: 'POST',
      body: JSON.stringify(data)
    });
  }
  
  async put(endpoint, data) {
    return this.request(endpoint, {
      method: 'PUT',
      body: JSON.stringify(data)
    });
  }
  
  async patch(endpoint, data) {
    return this.request(endpoint, {
      method: 'PATCH',
      body: JSON.stringify(data)
    });
  }
  
  async delete(endpoint) {
    return this.request(endpoint, {
      method: 'DELETE'
    });
  }
}

// Utilisation
const api = new APIClient('https://jsonplaceholder.typicode.com', {
  headers: {
    'Authorization': 'Bearer mon-token'
  }
});

// GET
const users = await api.get('/users');
console.log('Users :', users);

// GET avec params
const user = await api.get('/users', { id: 1 });

// POST
const newUser = await api.post('/users', {
  name: 'John Doe',
  email: 'john@exemple.com'
});

// PUT
const updated = await api.put('/users/1', {
  name: 'Jane Doe'
});

// DELETE
await api.delete('/users/1');
```

### Exemple 2 : Système de gestion d'état asynchrone

```javascript
class AsyncStateManager {
  constructor() {
    this.state = {
      data: null,
      loading: false,
      error: null
    };
    this.listeners = [];
  }
  
  // S'abonner aux changements
  subscribe(listener) {
    this.listeners.push(listener);
    
    // Retourner une fonction de désabonnement
    return () => {
      this.listeners = this.listeners.filter(l => l !== listener);
    };
  }
  
  // Notifier tous les listeners
  notify() {
    this.listeners.forEach(listener => listener(this.state));
  }
  
  // Mettre à jour l'état
  setState(updates) {
    this.state = { ...this.state, ...updates };
    this.notify();
  }
  
  // Exécuter une action asynchrone
  async execute(asyncFn) {
    // Début du chargement
    this.setState({ loading: true, error: null });
    
    try {
      const data = await asyncFn();
      
      // Succès
      this.setState({ 
        data: data, 
        loading: false, 
        error: null 
      });
      
      return data;
      
    } catch (erreur) {
      // Erreur
      this.setState({ 
        data: null, 
        loading: false, 
        error: erreur.message 
      });
      
      throw erreur;
    }
  }
  
  // Reset
  reset() {
    this.setState({
      data: null,
      loading: false,
      error: null
    });
  }
}

// Utilisation
const userManager = new AsyncStateManager();

// S'abonner aux changements
userManager.subscribe((state) => {
  console.log('État mis à jour :', state);
  
  if (state.loading) {
    console.log('Chargement...');
  }
  
  if (state.error) {
    console.error('Erreur :', state.error);
  }
  
  if (state.data) {
    console.log('Données :', state.data);
  }
});

// Charger des données
userManager.execute(async () => {
  const response = await fetch('https://jsonplaceholder.typicode.com/users');
  return await response.json();
});
```

### Exemple 3 : Queue de requêtes avec limitation de taux

```javascript
class RequestQueue {
  constructor(maxConcurrent = 3, delayMs = 1000) {
    this.maxConcurrent = maxConcurrent; // Nombre max de requêtes simultanées
    this.delayMs = delayMs; // Délai entre chaque lot
    this.queue = [];
    this.running = 0;
  }
  
  // Ajouter une requête à la queue
  async add(requestFn) {
    return new Promise((resolve, reject) => {
      this.queue.push({
        fn: requestFn,
        resolve,
        reject
      });
      
      this.process();
    });
  }
  
  // Traiter la queue
  async process() {
    if (this.running >= this.maxConcurrent || this.queue.length === 0) {
      return;
    }
    
    this.running++;
    const { fn, resolve, reject } = this.queue.shift();
    
    try {
      const result = await fn();
      resolve(result);
    } catch (erreur) {
      reject(erreur);
    } finally {
      this.running--;
      
      // Attendre avant de traiter la prochaine requête
      await new Promise(r => setTimeout(r, this.delayMs));
      
      this.process();
    }
  }
}

// Utilisation
const queue = new RequestQueue(2, 500); // Max 2 requêtes simultanées, 500ms entre chaque

// Ajouter plusieurs requêtes
const urls = [
  'https://jsonplaceholder.typicode.com/users/1',
  'https://jsonplaceholder.typicode.com/users/2',
  'https://jsonplaceholder.typicode.com/users/3',
  'https://jsonplaceholder.typicode.com/users/4',
  'https://jsonplaceholder.typicode.com/users/5'
];

const promises = urls.map(url => 
  queue.add(async () => {
    console.log(`Fetching ${url}...`);
    const response = await fetch(url);
    return await response.json();
  })
);

// Attendre toutes les requêtes
Promise.all(promises)
  .then(resultats => console.log('Tous les utilisateurs :', resultats))
  .catch(err => console.error('Erreur :', err));
```

---

## Résumé des Concepts Clés

```javascript
// ✅ BONNES PRATIQUES

// 1. Toujours gérer les erreurs
async function bonneGestion() {
  try {
    const data = await fetch('/api/data').then(r => r.json());
    return data;
  } catch (err) {
    console.error('Erreur :', err);
    return null; // Valeur par défaut
  }
}

// 2. Vérifier response.ok avec fetch
async function verifierStatus(url) {
  const response = await fetch(url);
  if (!response.ok) {
    throw new Error(`HTTP ${response.status}`);
  }
  return await response.json();
}

// 3. Utiliser Promise.all pour parallélisme
async function paralleliser() {
  const [users, posts] = await Promise.all([
    fetch('/api/users').then(r => r.json()),
    fetch('/api/posts').then(r => r.json())
  ]);
  return { users, posts };
}

// 4. Préférer async/await aux chaînes de .then()
// ❌ Moins lisible
function avecThen() {
  return fetch('/api/data')
    .then(r => r.json())
    .then(data => traiter(data))
    .then(resultat => afficher(resultat));
}

// ✅ Plus lisible
async function avecAsync() {
  const response = await fetch('/api/data');
  const data = await response.json();
  const resultat = await traiter(data);
  return afficher(resultat);
}

// 5. Utiliser AbortController pour annulations
const controller = new AbortController();
fetch('/api/data', { signal: controller.signal });
// Plus tard : controller.abort();

// 6. Ne pas oublier finally pour cleanup
async function avecCleanup() {
  try {
    afficherLoader();
    const data = await fetch('/api/data').then(r => r.json());
    return data;
  } catch (err) {
    afficherErreur(err);
  } finally {
    cacherLoader(); // Toujours exécuté
  }
}
```

---

Voilà ! Le **Chapitre 16 sur la Programmation Asynchrone** est complet avec :
- Event Loop et Task Queue
- Callbacks et callback hell
- Promises (création, chaînage, gestion d'erreurs)
- Méthodes statiques (all, allSettled, race, any)
- Async/Await (syntaxe, error handling, parallélisme)
- Fetch API (GET, POST, responses, headers, CORS)
- Patterns pratiques (retry, cache, upload, polling, etc.)
- Exemples complets (APIClient, StateManager, RequestQueue)

---

# Questions/Réponses - Programmation Asynchrone (Entretien Oral)

## Questions sur l'Event Loop et les Bases

### Q1 : Qu'est-ce que l'Event Loop en JavaScript ?

**Réponse :**
L'Event Loop est le mécanisme qui permet à JavaScript, qui est single-threaded, de gérer les opérations asynchrones. Il surveille en permanence le Call Stack et les Task Queues. Quand le Call Stack est vide, l'Event Loop prend la première tâche dans la queue et l'ajoute au Call Stack pour exécution.

Il y a deux types de queues :
- **Microtask Queue** (prioritaire) : Promises, queueMicrotask
- **Macrotask Queue** : setTimeout, setInterval, I/O

Les microtasks sont toujours traitées avant les macrotasks.

```javascript
console.log('1'); // Synchrone
setTimeout(() => console.log('2'), 0); // Macrotask
Promise.resolve().then(() => console.log('3')); // Microtask
console.log('4'); // Synchrone
// Résultat : 1, 4, 3, 2
```

---

### Q2 : Quelle est la différence entre microtasks et macrotasks ?

**Réponse :**
Les **microtasks** ont une priorité plus élevée que les **macrotasks**. Après chaque tâche du Call Stack, l'Event Loop vide complètement la Microtask Queue avant de passer à la Macrotask Queue.

**Microtasks :**
- Promises (.then, .catch, .finally)
- queueMicrotask()
- MutationObserver

**Macrotasks :**
- setTimeout / setInterval
- I/O operations
- UI rendering

```javascript
setTimeout(() => console.log('Macrotask'), 0);
Promise.resolve().then(() => console.log('Microtask'));
// Microtask s'exécute toujours en premier
```

---

## Questions sur les Promises

### Q3 : Qu'est-ce qu'une Promise et quels sont ses trois états ?

**Réponse :**
Une Promise est un objet représentant l'achèvement ou l'échec futur d'une opération asynchrone.

Les trois états sont :
1. **Pending** (en attente) : état initial
2. **Fulfilled** (résolue) : opération réussie
3. **Rejected** (rejetée) : opération échouée

Une Promise ne peut changer d'état qu'une seule fois (de pending à fulfilled ou rejected).

```javascript
const promise = new Promise((resolve, reject) => {
  if (/* condition */) {
    resolve('Succès'); // fulfilled
  } else {
    reject('Erreur'); // rejected
  }
});
```

---

### Q4 : Quelle est la différence entre Promise.all() et Promise.allSettled() ?

**Réponse :**

**Promise.all()** :
- Attend que TOUTES les Promises soient fulfilled
- Si UNE seule reject, tout échoue immédiatement (fail-fast)
- Retourne un array des valeurs

**Promise.allSettled()** :
- Attend que TOUTES soient terminées (fulfilled ou rejected)
- Ne rejette JAMAIS
- Retourne un array d'objets avec status et value/reason

```javascript
// Promise.all - échoue si une seule rejette
Promise.all([
  Promise.resolve(1),
  Promise.reject('erreur'),
  Promise.resolve(3)
]).catch(err => console.log(err)); // 'erreur'

// Promise.allSettled - attend toutes
Promise.allSettled([
  Promise.resolve(1),
  Promise.reject('erreur'),
  Promise.resolve(3)
]).then(results => console.log(results));
// [
//   {status: 'fulfilled', value: 1},
//   {status: 'rejected', reason: 'erreur'},
//   {status: 'fulfilled', value: 3}
// ]
```

**Cas d'usage :**
- `Promise.all()` : quand on a BESOIN de toutes les données
- `Promise.allSettled()` : quand on veut continuer même en cas d'échecs partiels

---

### Q5 : Quelle est la différence entre Promise.race() et Promise.any() ?

**Réponse :**

**Promise.race()** :
- Retourne la PREMIÈRE Promise qui se termine (fulfilled OU rejected)
- Peut rejeter si la première est rejected

**Promise.any()** :
- Retourne la PREMIÈRE Promise fulfilled
- Ignore les rejections
- Rejette seulement si TOUTES les Promises rejettent (AggregateError)

```javascript
// Promise.race - première terminée (succès ou échec)
Promise.race([
  new Promise(resolve => setTimeout(() => resolve('lent'), 2000)),
  new Promise((_, reject) => setTimeout(() => reject('rapide erreur'), 500))
]).catch(err => console.log(err)); // 'rapide erreur'

// Promise.any - première réussie
Promise.any([
  Promise.reject('erreur 1'),
  new Promise(resolve => setTimeout(() => resolve('succès'), 500)),
  Promise.reject('erreur 2')
]).then(result => console.log(result)); // 'succès'
```

**Cas d'usage :**
- `Promise.race()` : timeouts, première réponse peu importe succès/échec
- `Promise.any()` : sources multiples (miroirs, fallbacks), première qui réussit

---

## Questions sur Async/Await

### Q6 : Quelle est la différence entre async/await et les Promises avec .then() ?

**Réponse :**
Ce sont deux syntaxes pour gérer les Promises. Async/await est du sucre syntaxique par-dessus les Promises.

**Différences principales :**

1. **Lisibilité** : async/await ressemble à du code synchrone
2. **Gestion d'erreurs** : try/catch vs .catch()
3. **Debugging** : stack traces plus claires avec async/await
4. **Valeurs intermédiaires** : plus faciles à gérer avec async/await

```javascript
// Avec .then() - chaînage
function avecThen() {
  return fetch('/api/user')
    .then(response => response.json())
    .then(user => fetch(`/api/orders/${user.id}`))
    .then(response => response.json())
    .then(orders => console.log(orders))
    .catch(err => console.error(err));
}

// Avec async/await - séquentiel
async function avecAsync() {
  try {
    const response = await fetch('/api/user');
    const user = await response.json();
    const ordersResponse = await fetch(`/api/orders/${user.id}`);
    const orders = await ordersResponse.json();
    console.log(orders);
  } catch (err) {
    console.error(err);
  }
}
```

Les deux sont équivalents, mais async/await est généralement préféré pour sa lisibilité.

---

### Q7 : Comment gérer plusieurs opérations asynchrones en parallèle avec async/await ?

**Réponse :**
Il faut utiliser `Promise.all()` pour éviter d'attendre séquentiellement chaque opération.

```javascript
// ❌ SÉQUENTIEL (lent) - 3 secondes total
async function sequentiel() {
  const user1 = await fetch('/api/users/1').then(r => r.json()); // 1s
  const user2 = await fetch('/api/users/2').then(r => r.json()); // 1s
  const user3 = await fetch('/api/users/3').then(r => r.json()); // 1s
  return [user1, user2, user3];
}

// ✅ PARALLÈLE (rapide) - 1 seconde total
async function parallele() {
  const [user1, user2, user3] = await Promise.all([
    fetch('/api/users/1').then(r => r.json()),
    fetch('/api/users/2').then(r => r.json()),
    fetch('/api/users/3').then(r => r.json())
  ]);
  return [user1, user2, user3];
}
```

**Point clé :** Les Promises démarrent immédiatement lors de leur création. `await` ne fait qu'attendre leur résolution.

---

### Q8 : Peut-on utiliser await en dehors d'une fonction async ?

**Réponse :**
Non, sauf dans les **modules ES6** au top-level (depuis ES2022).

```javascript
// ❌ Erreur : await en dehors d'async
const data = await fetch('/api/data'); // SyntaxError

// ✅ Dans une fonction async
async function getData() {
  const data = await fetch('/api/data');
  return data;
}

// ✅ Top-level await dans un module ES6
// fichier: module.js
export const data = await fetch('/api/data').then(r => r.json());

// ✅ IIFE async pour exécution immédiate
(async () => {
  const data = await fetch('/api/data');
  console.log(data);
})();
```

---

## Questions sur Fetch API

### Q9 : Pourquoi fetch() ne rejette pas sur les erreurs HTTP 404 ou 500 ?

**Réponse :**
C'est une caractéristique importante de fetch : il ne rejette que sur les **erreurs réseau** (pas de connexion, CORS, etc.), pas sur les statuts HTTP.

Pour les statuts 404, 500, etc., la Promise est fulfilled mais `response.ok` est `false`.

```javascript
// ❌ Ceci ne suffit pas
fetch('/api/data')
  .then(response => response.json()) // Peut parser un JSON d'erreur !
  .catch(err => console.error(err)); // N'attrape pas les 404/500

// ✅ Il faut vérifier response.ok
fetch('/api/data')
  .then(response => {
    if (!response.ok) {
      throw new Error(`HTTP ${response.status}: ${response.statusText}`);
    }
    return response.json();
  })
  .catch(err => console.error(err)); // Attrape maintenant les erreurs HTTP

// ✅ Avec async/await
async function fetchData() {
  const response = await fetch('/api/data');
  
  if (!response.ok) {
    throw new Error(`HTTP ${response.status}`);
  }
  
  return await response.json();
}
```

---

### Q10 : Comment envoyer des données JSON avec fetch() ?

**Réponse :**
Il faut :
1. Utiliser la méthode POST (ou PUT/PATCH)
2. Définir le header `Content-Type: application/json`
3. Convertir l'objet JavaScript en string JSON avec `JSON.stringify()`

```javascript
async function creerUtilisateur(userData) {
  const response = await fetch('https://api.exemple.com/users', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'Authorization': 'Bearer token123' // Optionnel
    },
    body: JSON.stringify(userData) // IMPORTANT : convertir en string
  });
  
  if (!response.ok) {
    throw new Error(`Erreur ${response.status}`);
  }
  
  return await response.json();
}

// Utilisation
creerUtilisateur({
  nom: 'Dupont',
  email: 'dupont@exemple.com',
  age: 30
});
```

---

### Q11 : Qu'est-ce que CORS et comment le gérer ?

**Réponse :**
**CORS** (Cross-Origin Resource Sharing) est une politique de sécurité des navigateurs qui bloque les requêtes HTTP vers un domaine différent de celui qui sert la page.

**Le problème :**
```javascript
// Page sur https://monsite.com
fetch('https://api-externe.com/data')
  .then(r => r.json())
  .catch(err => {
    // Error: CORS policy: No 'Access-Control-Allow-Origin' header
  });
```

**Solutions :**

1. **Côté serveur** (la vraie solution) :
   Le backend doit envoyer les headers appropriés :
   ```
   Access-Control-Allow-Origin: https://monsite.com
   Access-Control-Allow-Methods: GET, POST, PUT, DELETE
   Access-Control-Allow-Headers: Content-Type, Authorization
   ```

2. **Proxy en développement** (React, Vue, etc.) :
   ```json
   // package.json
   "proxy": "https://api-externe.com"
   ```
   
   ```javascript
   // Fetch relatif - contourne CORS en dev
   fetch('/data').then(r => r.json());
   ```

3. **Mode no-cors** (limité, pas d'accès à la réponse) :
   ```javascript
   fetch('https://api-externe.com/data', { mode: 'no-cors' })
     .then(response => {
       // response.body est null, on ne peut pas lire les données
     });
   ```

**En entretien, souligner :** CORS est une protection côté navigateur, la vraie solution est côté serveur.

---

## Questions sur Error Handling

### Q12 : Comment gérer les erreurs dans une chaîne de Promises ?

**Réponse :**
Il y a plusieurs approches :

**1. .catch() à la fin (attrape toutes les erreurs) :**
```javascript
fetch('/api/user')
  .then(r => r.json())
  .then(user => fetch(`/api/orders/${user.id}`))
  .then(r => r.json())
  .then(orders => console.log(orders))
  .catch(err => {
    // Attrape toutes les erreurs de la chaîne
    console.error('Erreur dans la chaîne :', err);
  });
```

**2. .catch() intermédiaires (récupération) :**
```javascript
fetch('/api/user')
  .then(r => r.json())
  .catch(err => {
    console.error('Erreur user :', err);
    return { id: 'default' }; // Valeur par défaut pour continuer
  })
  .then(user => fetch(`/api/orders/${user.id}`))
  .then(r => r.json())
  .then(orders => console.log(orders));
```

**3. Avec async/await et try/catch :**
```javascript
async function chargerDonnees() {
  try {
    const userResponse = await fetch('/api/user');
    const user = await userResponse.json();
    
    const ordersResponse = await fetch(`/api/orders/${user.id}`);
    const orders = await ordersResponse.json();
    
    return orders;
  } catch (err) {
    console.error('Erreur :', err);
    return []; // Valeur par défaut
  }
}
```

---

### Q13 : Comment implémenter un système de retry (réessai) pour les requêtes qui échouent ?

**Réponse :**
```javascript
async function fetchAvecRetry(url, maxRetries = 3) {
  for (let tentative = 0; tentative < maxRetries; tentative++) {
    try {
      const response = await fetch(url);
      
      if (response.ok) {
        return await response.json();
      }
      
      // Si pas ok et dernière tentative
      if (tentative === maxRetries - 1) {
        throw new Error(`Échec après ${maxRetries} tentatives`);
      }
      
    } catch (erreur) {
      // Si dernière tentative, propager l'erreur
      if (tentative === maxRetries - 1) {
        throw erreur;
      }
      
      // Backoff exponentiel : 1s, 2s, 4s...
      const delai = Math.pow(2, tentative) * 1000;
      console.log(`Tentative ${tentative + 1} échouée, réessai dans ${delai}ms`);
      
      await new Promise(resolve => setTimeout(resolve, delai));
    }
  }
}

// Utilisation
fetchAvecRetry('https://api.instable.com/data')
  .then(data => console.log('Succès :', data))
  .catch(err => console.error('Échec définitif :', err));
```

**Points clés :**
- Boucle for avec nombre de tentatives
- Backoff exponentiel pour éviter de surcharger le serveur
- Propager l'erreur si toutes les tentatives échouent

---

## Questions Pratiques et Avancées

### Q14 : Comment annuler une requête fetch en cours ?

**Réponse :**
Avec **AbortController**, introduit pour permettre l'annulation des requêtes.

```javascript
const controller = new AbortController();
const signal = controller.signal;

// Lancer la requête avec le signal
fetch('https://api.exemple.com/data', { signal })
  .then(response => response.json())
  .then(data => console.log(data))
  .catch(err => {
    if (err.name === 'AbortError') {
      console.log('Requête annulée');
    } else {
      console.error('Autre erreur :', err);
    }
  });

// Annuler la requête (par exemple après 5 secondes)
setTimeout(() => {
  controller.abort();
  console.log('Annulation envoyée');
}, 5000);
```

**Cas d'usage pratiques :**
- Timeout pour requêtes longues
- Annulation lors de changement de page/route
- Annulation de recherches obsolètes (autocomplete)

```javascript
// Exemple : recherche avec annulation de la précédente
let currentController = null;

function rechercher(terme) {
  // Annuler la recherche précédente
  if (currentController) {
    currentController.abort();
  }
  
  currentController = new AbortController();
  
  fetch(`/api/search?q=${terme}`, { signal: currentController.signal })
    .then(r => r.json())
    .then(resultats => console.log(resultats))
    .catch(err => {
      if (err.name !== 'AbortError') {
        console.error(err);
      }
    });
}
```

---

### Q15 : Comment implémenter un timeout pour une requête fetch ?

**Réponse :**
Fetch n'a pas de timeout natif, mais on peut utiliser `Promise.race()` avec `AbortController` :

```javascript
function fetchAvecTimeout(url, timeout = 5000) {
  const controller = new AbortController();
  const signal = controller.signal;
  
  const fetchPromise = fetch(url, { signal });
  
  const timeoutPromise = new Promise((_, reject) => {
    setTimeout(() => {
      controller.abort();
      reject(new Error(`Timeout après ${timeout}ms`));
    }, timeout);
  });
  
  return Promise.race([fetchPromise, timeoutPromise]);
}

// Utilisation
fetchAvecTimeout('https://api.lente.com/data', 3000)
  .then(response => response.json())
  .then(data => console.log('Données :', data))
  .catch(err => console.error('Erreur :', err.message));
```

**Alternative moderne avec AbortSignal.timeout() :**
```javascript
async function fetchAvecTimeout2(url, timeout = 5000) {
  try {
    const response = await fetch(url, {
      signal: AbortSignal.timeout(timeout)
    });
    return await response.json();
  } catch (err) {
    if (err.name === 'TimeoutError') {
      console.error('Timeout dépassé');
    }
    throw err;
  }
}
```

---

### Q16 : Comment gérer le cache des requêtes HTTP en JavaScript ?

**Réponse :**
Plusieurs approches :

**1. Cache simple avec Map :**
```javascript
const cache = new Map();

async function fetchAvecCache(url, ttl = 60000) {
  const maintenant = Date.now();
  
  // Vérifier le cache
  if (cache.has(url)) {
    const { data, timestamp } = cache.get(url);
    if (maintenant - timestamp < ttl) {
      console.log('Depuis cache');
      return data;
    }
  }
  
  // Fetch si pas en cache
  console.log('Depuis API');
  const response = await fetch(url);
  const data = await response.json();
  
  // Mettre en cache
  cache.set(url, { data, timestamp: maintenant });
  
  return data;
}
```

**2. localStorage pour persistance :**
```javascript
async function fetchAvecLocalStorage(url) {
  const cacheKey = `cache_${url}`;
  const cached = localStorage.getItem(cacheKey);
  
  if (cached) {
    const { data, timestamp } = JSON.parse(cached);
    if (Date.now() - timestamp < 3600000) { // 1 heure
      return data;
    }
  }
  
  const response = await fetch(url);
  const data = await response.json();
  
  localStorage.setItem(cacheKey, JSON.stringify({
    data,
    timestamp: Date.now()
  }));
  
  return data;
}
```

**3. Utiliser les options de cache de fetch :**
```javascript
fetch('/api/data', {
  cache: 'force-cache' // Utilise le cache du navigateur
});
// Options : 'default', 'no-cache', 'reload', 'force-cache', 'only-if-cached'
```

---

### Q17 : Quelle est la différence entre callback hell et comment les Promises le résolvent-elles ?

**Réponse :**
Le **callback hell** (pyramide de la mort) survient quand on imbrique trop de callbacks, rendant le code illisible.

**Callback hell :**
```javascript
chargerUtilisateur(userId, (err, user) => {
  if (err) return console.error(err);
  
  chargerCommandes(user.id, (err, orders) => {
    if (err) return console.error(err);
    
    chargerDetails(orders[0].id, (err, details) => {
      if (err) return console.error(err);
      
      chargerProduits(details.productId, (err, products) => {
        if (err) return console.error(err);
        console.log(products);
        // Encore plus de niveaux...
      });
    });
  });
});
```

**Avec Promises (chaînage horizontal) :**
```javascript
chargerUtilisateur(userId)
  .then(user => chargerCommandes(user.id))
  .then(orders => chargerDetails(orders[0].id))
  .then(details => chargerProduits(details.productId))
  .then(products => console.log(products))
  .catch(err => console.error(err));
```

**Avec async/await (encore plus lisible) :**
```javascript
async function chargerDonnees(userId) {
  try {
    const user = await chargerUtilisateur(userId);
    const orders = await chargerCommandes(user.id);
    const details = await chargerDetails(orders[0].id);
    const products = await chargerProduits(details.productId);
    console.log(products);
  } catch (err) {
    console.error(err);
  }
}
```

**Avantages des Promises/async-await :**
- Code linéaire et lisible
- Gestion d'erreurs centralisée
- Facilite le debugging (stack traces)
- Permet le parallélisme avec Promise.all()

---

### Q18 : Comment feriez-vous pour traiter 1000 requêtes API sans surcharger le serveur ?

**Réponse :**
Il faut implémenter un **système de batching** (traitement par lots) ou une **request queue** avec limitation.

**Approche 1 : Batching (lots séquentiels) :**
```javascript
async function traiterParLots(items, taillelot = 10) {
  const resultats = [];
  
  for (let i = 0; i < items.length; i += taillelot) {
    const lot = items.slice(i, i + taillelot);
    console.log(`Traitement du lot ${Math.floor(i / taillelot) + 1}...`);
    
    // Traiter le lot en parallèle
    const promisesLot = lot.map(item => 
      fetch(`/api/process/${item.id}`).then(r => r.json())
    );
    
    const resultatsLot = await Promise.all(promisesLot);
    resultats.push(...resultatsLot);
    
    // Pause entre les lots
    if (i + taillelot < items.length) {
      await new Promise(r => setTimeout(r, 1000)); // 1s de pause
    }
  }
  
  return resultats;
}

// Traiter 1000 items par lots de 10
const items = Array.from({ length: 1000 }, (_, i) => ({ id: i }));
traiterParLots(items, 10);
```

**Approche 2 : Request Queue avec limite de concurrence :**
```javascript
class RequestQueue {
  constructor(maxConcurrent = 5) {
    this.maxConcurrent = maxConcurrent;
    this.queue = [];
    this.running = 0;
  }
  
  async add(requestFn) {
    return new Promise((resolve, reject) => {
      this.queue.push({ fn: requestFn, resolve, reject });
      this.process();
    });
  }
  
  async process() {
    if (this.running >= this.maxConcurrent || this.queue.length === 0) {
      return;
    }
    
    this.running++;
    const { fn, resolve, reject } = this.queue.shift();
    
    try {
      const result = await fn();
      resolve(result);
    } catch (err) {
      reject(err);
    } finally {
      this.running--;
      this.process(); // Traiter la suivante
    }
  }
}

// Utilisation
const queue = new RequestQueue(5); // Max 5 simultanées

const promises = items.map(item => 
  queue.add(() => fetch(`/api/process/${item.id}`).then(r => r.json()))
);

const resultats = await Promise.all(promises);
```

**Points à mentionner en entretien :**
- Limiter la concurrence pour ne pas surcharger
- Ajouter des délais entre lots
- Gérer les échecs individuels
- Monitoring/logging de la progression

---

## Questions Pièges et Conceptuelles

### Q19 : Que se passe-t-il si on oublie await devant une Promise ?

**Réponse :**
La fonction retourne immédiatement sans attendre la résolution de la Promise, ce qui peut causer des bugs subtils.

```javascript
// ❌ ERREUR : oubli de await
async function mauvais() {
  const data = fetch('/api/data'); // data est une Promise, pas les données !
  console.log(data); // Promise { <pending> }
  return data.resultat; // undefined
}

// ✅ CORRECT
async function correct() {
  const response = await fetch('/api/data');
  const data = await response.json();
  console.log(data); // Les vraies données
  return data;
}
```

**Cas tricky :**
```javascript
async function piege() {
  const promises = [1, 2, 3].map(async (num) => {
    const result = await calculComplexe(num);
    return result;
  });
  
  // ❌ promises est un array de Promises, pas de résultats
  console.log(promises); // [Promise, Promise, Promise]
  
  // ✅ Il faut await Promise.all
  const resultats = await Promise.all(promises);
  console.log(resultats); // [résultat1, résultat2, résultat3]
}
```

---

### Q20 : Est-ce que async/await bloque le thread JavaScript ?

**Réponse :**
**Non**, c'est une idée reçue importante à corriger.

`await` **suspend** l'exécution de la fonction async, mais **ne bloque pas** le thread. Le reste du code JavaScript continue de s'exécuter.

```javascript
async function attendre() {
  console.log('1. Début');
  
  await new Promise(resolve => setTimeout(resolve, 2000)); // Suspend 2s
  
  console.log('3. Après 2 secondes');
}

attendre();
console.log('2. Ceci s\'exécute immédiatement'); // Ne bloque pas

// Ordre d'affichage :
// 1. Début
// 2. Ceci s'exécute immédiatement
// 3. Après 2 secondes (après 2s)
```

**Explication technique :**
- `await` libère le Call Stack
- Le code après `await` est mis dans la Microtask Queue
- Le reste du code continue normalement
- Quand la Promise résout, le code après `await` reprend

**En entretien, souligner :** JavaScript reste single-threaded et non-bloquant, même avec async/await.

