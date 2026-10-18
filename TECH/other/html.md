# 📘 HTML - Résumé Conceptuel pour Entretien

## 🎯 Concepts Avancés & Questions Pièges

---

## 1️⃣ FONDAMENTAUX CONCEPTUELS

### 📝 **Qu'est-ce que HTML ? (Vision Approfondie)**

```
HTML = HyperText Markup Language

HYPERTEXT:
- Texte avec des liens (navigation non-linéaire)
- Interconnexion de documents
- Base du Web

MARKUP:
- Langage de balisage (pas de programmation)
- Structure sémantique du contenu
- Annotations pour donner du sens

LANGUAGE:
- Syntaxe et règles définies
- Standard W3C
- Évolution: HTML4 → XHTML → HTML5 → HTML Living Standard

HTML N'EST PAS:
❌ Un langage de programmation
❌ Un langage de style
❌ Un langage de logique

HTML EST:
✅ La structure/squelette de la page
✅ Le contenu sémantique
✅ L'accessibilité de base
```

---

### 🏗️ **Structure vs Présentation vs Comportement**

```
LA SÉPARATION DES PRÉOCCUPATIONS:

HTML (Structure)          CSS (Présentation)      JavaScript (Comportement)
     ↓                           ↓                        ↓
"Qu'est-ce?"             "À quoi ça ressemble?"    "Que fait-il?"
Contenu                  Apparence                 Interactivité
Sémantique               Style                     Logique

EXEMPLE:

<button class="btn-primary" onclick="handleClick()">
  ↑ HTML                ↑ CSS                ↑ JS
  Structure             Style hook           Behavior

MAUVAISE PRATIQUE (Tout mélangé):
<div style="color:red" onclick="alert('Hi')">Text</div>

BONNE PRATIQUE (Séparé):
HTML:  <button class="danger-btn" id="delete-btn">Delete</button>
CSS:   .danger-btn { background: red; }
JS:    document.getElementById('delete-btn').addEventListener('click', handleDelete);
```

---

## 2️⃣ SÉMANTIQUE HTML

### 🎯 **Pourquoi la Sémantique est Cruciale ?**

```
SÉMANTIQUE = Donner du SENS au contenu (pas juste l'apparence)

3 BÉNÉFICIAIRES:

1️⃣ NAVIGATEURS:
- Comprennent la structure
- Fonctionnalités natives (ex: <video> controls)
- Meilleure performance

2️⃣ MOTEURS DE RECHERCHE (SEO):
- Indexation intelligente
- Compréhension du contenu
- Meilleur ranking
- Rich snippets

3️⃣ TECHNOLOGIES D'ASSISTANCE:
- Screen readers
- Navigation au clavier
- Dispositifs braille
- Accessibilité

EXEMPLE:

❌ NON-SÉMANTIQUE:
<div class="header">
  <div class="nav">
    <div class="link">Home</div>
  </div>
</div>

✅ SÉMANTIQUE:
<header>
  <nav>
    <a href="/">Home</a>
  </nav>
</header>

Impact:
- Screen reader annonce "navigation landmark"
- SEO comprend que c'est la navigation principale
- Code plus lisible pour les développeurs
```

### 📋 **Éléments Sémantiques vs Non-Sémantiques**

```
NON-SÉMANTIQUE (<div>, <span>):
- Pas de signification intrinsèque
- Containers génériques
- Utilisés pour styling/layout uniquement

SÉMANTIQUE:

STRUCTURE DE PAGE:
<header>     - En-tête de page/section
<nav>        - Navigation principale
<main>       - Contenu principal (unique par page)
<article>    - Contenu autonome (blog post, news article)
<section>    - Section thématique
<aside>      - Contenu tangentiel (sidebar, related info)
<footer>     - Pied de page/section

CONTENU:
<h1> à <h6>  - Hiérarchie de titres (importance)
<p>          - Paragraphe
<blockquote> - Citation longue
<cite>       - Référence d'œuvre
<time>       - Date/heure (attribut datetime)
<figure>     - Contenu illustratif avec légende
<figcaption> - Légende de figure
<mark>       - Texte surligné/important
<strong>     - Importance forte (pas juste bold)
<em>         - Emphase (pas juste italic)

INTERACTIVITÉ:
<button>     - Bouton d'action
<details>    - Contenu repliable
<summary>    - Titre de details
<dialog>     - Boîte de dialogue modale

QUESTION PIÈGE: <b> vs <strong>, <i> vs <em> ?

<b> / <i>:
- Présentation pure (bold/italic)
- Pas de signification sémantique
- À éviter (utiliser CSS à la place)

<strong> / <em>:
- Sémantique (importance/emphase)
- Screen readers peuvent insister vocalement
- SEO comprend l'importance
- ✅ Préférer toujours
```

---

## 3️⃣ ACCESSIBILITÉ (A11Y)

### ♿ **Principes WCAG (Web Content Accessibility Guidelines)**

```
4 PRINCIPES FONDAMENTAUX (POUR):

1️⃣ PERCEIVABLE (Perceptible):
Le contenu doit être perceptible par tous

Exemples:
- Images avec alt text
- Vidéos avec sous-titres
- Contraste suffisant (ratio 4.5:1 minimum)
- Texte redimensionnable

2️⃣ OPERABLE (Utilisable):
L'interface doit être utilisable par tous

Exemples:
- Navigation au clavier (tab, enter, espace)
- Temps suffisant pour lire/interagir
- Pas de contenu qui clignote (risque épilepsie)
- Skip links (sauter navigation)

3️⃣ UNDERSTANDABLE (Compréhensible):
Le contenu et l'interface doivent être compréhensibles

Exemples:
- Langage clair
- Comportement prévisible
- Messages d'erreur explicites
- Labels de formulaires clairs

4️⃣ ROBUST (Robuste):
Compatible avec technologies actuelles et futures

Exemples:
- HTML valide
- Support des technologies d'assistance
- Compatibilité navigateurs
```

### 🎯 **Attributs ARIA (Accessible Rich Internet Applications)**

```
ARIA = Améliorer l'accessibilité des applications web dynamiques

RÈGLE D'OR: "No ARIA is better than bad ARIA"
- Utiliser HTML sémantique d'abord
- ARIA en complément seulement

3 CATÉGORIES:

1️⃣ ROLES:
Définissent le type d'élément

role="button"        - Élément se comporte comme bouton
role="navigation"    - Zone de navigation
role="alert"         - Message d'alerte
role="dialog"        - Boîte de dialogue
role="tablist"       - Liste d'onglets
role="progressbar"   - Barre de progression

Exemple:
<div role="button" tabindex="0">Click me</div>
// Mieux: <button>Click me</button> (natif !)

2️⃣ PROPERTIES (États statiques):
Caractéristiques qui ne changent pas souvent

aria-label="Description"           - Label alternatif
aria-labelledby="id"              - Référence à un label
aria-describedby="id"             - Description détaillée
aria-hidden="true"                - Caché aux screen readers
aria-required="true"              - Champ obligatoire
aria-haspopup="true"              - Ouvre un popup

3️⃣ STATES (États dynamiques):
Caractéristiques qui changent (gérées par JS)

aria-expanded="true/false"        - Élément déplié/replié
aria-disabled="true/false"        - Désactivé
aria-pressed="true/false"         - Bouton toggle pressé
aria-selected="true/false"        - Élément sélectionné
aria-checked="true/false"         - Case cochée
aria-live="polite/assertive"      - Région de contenu dynamique

EXEMPLES PRATIQUES:

// Bouton toggle
<button 
  aria-pressed="false" 
  aria-label="Toggle dark mode"
>
  🌙
</button>

// Menu déroulant
<button 
  aria-expanded="false" 
  aria-haspopup="true"
  aria-controls="menu-id"
>
  Menu
</button>
<ul id="menu-id" hidden>...</ul>

// Live region (notifications)
<div aria-live="polite" aria-atomic="true">
  <!-- Contenu mis à jour dynamiquement -->
</div>

// Image décorative (cachée)
<img src="decoration.png" alt="" aria-hidden="true">

ATTENTION:
❌ <div role="button">Click</div>  // Manque tabindex, keyboard events
✅ <button>Click</button>           // Tout natif
```

### ⌨️ **Navigation Clavier**

```
UTILISATEURS CONCERNÉS:
- Handicap moteur
- Utilisateurs power (productivité)
- Navigation rapide
- Dispositifs sans souris

TOUCHES STANDARD:

Tab          - Aller au prochain élément focusable
Shift+Tab    - Aller au précédent élément focusable
Enter        - Activer liens et boutons
Space        - Activer boutons, checkboxes
Arrows       - Navigation dans listes, tabs, etc.
Escape       - Fermer modals, menus

ÉLÉMENTS FOCUSABLES NATIFS:
- <a href="...">
- <button>
- <input>, <select>, <textarea>
- <iframe>
- Elements avec tabindex="0"

TABINDEX:

tabindex="0"    - Élément dans l'ordre naturel (✅ recommandé)
tabindex="-1"   - Focusable programmatiquement seulement
                  (ex: modal, skip à une section)
tabindex="1+"   - ❌ ÉVITER ! Casse l'ordre naturel

EXEMPLE ACCESSIBLE:

// Custom button (déconseillé, mais si nécessaire)
<div 
  role="button" 
  tabindex="0"
  onclick="handleClick()"
  onkeydown="if(e.key==='Enter' || e.key===' ') handleClick()"
>
  Click me
</div>

// Mieux: button natif
<button onclick="handleClick()">Click me</button>
// Navigation clavier gratuite !

SKIP LINKS (Important !):

<body>
  <a href="#main-content" class="skip-link">
    Skip to main content
  </a>
  <nav>...</nav>
  <main id="main-content">...</main>
</body>

// CSS
.skip-link {
  position: absolute;
  top: -40px;
}
.skip-link:focus {
  top: 0;
}

Permet aux utilisateurs clavier de sauter la navigation répétitive.
```

---

## 4️⃣ SEO (Search Engine Optimization)

### 🔍 **HTML et Référencement**

```
IMPACT DU HTML SUR LE SEO:

1️⃣ STRUCTURE DE TITRES (Crucial):

<h1> - UN SEUL par page (titre principal)
<h2> - Sections principales
<h3> - Sous-sections
...

❌ MAUVAIS:
<h1>Mon Site</h1>
<h3>Article Title</h3>  // Saute h2
<h2>Section</h2>         // Désordre

✅ BON:
<h1>Article Title</h1>   // UN seul, le plus important
<h2>Section 1</h2>
  <h3>Subsection 1.1</h3>
<h2>Section 2</h2>

Impact SEO: Hiérarchie aide Google à comprendre la structure

2️⃣ BALISES META (Fondamentales):

<head>
  <!-- Title (55-60 caractères max) -->
  <title>Mon Article - Mon Site</title>
  
  <!-- Description (150-160 caractères) -->
  <meta name="description" content="Description accrocheuse et pertinente">
  
  <!-- Viewport (responsive) -->
  <meta name="viewport" content="width=device-width, initial-scale=1">
  
  <!-- Charset -->
  <meta charset="UTF-8">
  
  <!-- Robots -->
  <meta name="robots" content="index, follow">
  
  <!-- Canonical (éviter duplicate content) -->
  <link rel="canonical" href="https://example.com/article">
  
  <!-- Open Graph (Social media) -->
  <meta property="og:title" content="Titre">
  <meta property="og:description" content="Description">
  <meta property="og:image" content="image.jpg">
  <meta property="og:url" content="https://example.com">
  
  <!-- Twitter Card -->
  <meta name="twitter:card" content="summary_large_image">
</head>

3️⃣ LIENS (Link Building):

// Liens internes (bon pour SEO)
<a href="/about">About</a>

// Liens externes (utiliser rel approprié)
<a href="external.com" rel="noopener">External</a>
<a href="untrusted.com" rel="nofollow">Untrusted</a>

rel="nofollow"   - Dire à Google de ne pas suivre (spam, etc.)
rel="noopener"   - Sécurité (éviter window.opener)
rel="sponsored"  - Lien sponsorisé/publicitaire
rel="ugc"        - User Generated Content

4️⃣ IMAGES ET ALT TEXT:

<img 
  src="product.jpg" 
  alt="iPhone 15 Pro Max en titane bleu"
  loading="lazy"
  width="800"
  height="600"
>

alt=""  - Image décorative (vide, pas absent !)
alt="Description précise"  - Image informative

Impact SEO:
- Google indexe les images via alt
- Améliore l'accessibilité
- Affichage si image ne charge pas

5️⃣ STRUCTURED DATA (Schema.org):

<script type="application/ld+json">
{
  "@context": "https://schema.org",
  "@type": "Article",
  "headline": "Titre de l'article",
  "author": {
    "@type": "Person",
    "name": "John Doe"
  },
  "datePublished": "2024-01-15",
  "image": "image.jpg"
}
</script>

Impact: Rich snippets dans Google (étoiles, prix, etc.)

6️⃣ PERFORMANCE (Core Web Vitals):

HTML impact la performance:
- Taille du DOM (éviter trop de nesting)
- Lazy loading images
- Preload/prefetch resources
- Async/defer scripts

<link rel="preload" href="font.woff2" as="font">
<link rel="prefetch" href="next-page.html">
<script src="app.js" defer></script>

7️⃣ MOBILE-FRIENDLY:

<meta name="viewport" content="width=device-width, initial-scale=1">

Essentiel ! Google privilégie le mobile-first indexing.
```

---

## 5️⃣ PERFORMANCE & OPTIMISATION

### ⚡ **Chargement et Rendu**

```
CRITICAL RENDERING PATH:

1. HTML Parse → DOM Tree
2. CSS Parse → CSSOM Tree
3. DOM + CSSOM → Render Tree
4. Layout (calcul positions)
5. Paint (pixels sur l'écran)

OPTIMISATIONS HTML:

1️⃣ MINIMISER LE DOM:
❌ DOM trop profond/complexe:
<div><div><div><div><div>Content</div></div></div></div></div>

✅ DOM plat:
<div>Content</div>

Raison: Moins de calculs layout, meilleure performance

2️⃣ SCRIPTS PLACEMENT:

<!-- ❌ Bloque le rendering -->
<head>
  <script src="app.js"></script>
</head>

<!-- ✅ Async (non-bloquant, ordre non garanti) -->
<script src="analytics.js" async></script>

<!-- ✅ Defer (non-bloquant, ordre garanti, exécute après parsing) -->
<script src="app.js" defer></script>

<!-- ✅ Fin de body (traditionnel) -->
<body>
  ...
  <script src="app.js"></script>
</body>

QUAND UTILISER QUOI?
- async: Scripts indépendants (analytics, ads)
- defer: Scripts qui dépendent du DOM complet
- End of body: Compatible vieux navigateurs

3️⃣ LAZY LOADING:

<img src="hero.jpg" loading="eager">  <!-- Above fold -->
<img src="footer.jpg" loading="lazy"> <!-- Below fold -->

<iframe src="video.html" loading="lazy"></iframe>

Économise bande passante, charge seulement ce qui est visible

4️⃣ RESOURCE HINTS:

<!-- Preconnect: Établit connexion en avance -->
<link rel="preconnect" href="https://fonts.googleapis.com">

<!-- DNS-prefetch: Résout DNS en avance -->
<link rel="dns-prefetch" href="https://api.example.com">

<!-- Preload: Charge ressource critique prioritaire -->
<link rel="preload" href="font.woff2" as="font" crossorigin>

<!-- Prefetch: Charge ressource pour navigation future -->
<link rel="prefetch" href="next-page.html">

<!-- Prerender: Render page en background (très agressif) -->
<link rel="prerender" href="next-page.html">

5️⃣ IMAGES OPTIMISÉES:

<!-- Responsive images -->
<img 
  srcset="small.jpg 400w,
          medium.jpg 800w,
          large.jpg 1200w"
  sizes="(max-width: 600px) 400px,
         (max-width: 1000px) 800px,
         1200px"
  src="medium.jpg"
  alt="Description"
>

<!-- Modern formats avec fallback -->
<picture>
  <source srcset="image.avif" type="image/avif">
  <source srcset="image.webp" type="image/webp">
  <img src="image.jpg" alt="Description">
</picture>

6️⃣ CRITICAL CSS:

<!-- Inline critical CSS (above the fold) -->
<style>
  /* Critical styles ici */
  .header { ... }
</style>

<!-- Defer non-critical CSS -->
<link rel="preload" href="styles.css" as="style" onload="this.rel='stylesheet'">
```

---

## 6️⃣ SÉCURITÉ HTML

### 🔒 **Vulnérabilités et Protections**

```
1️⃣ XSS (Cross-Site Scripting):

PROBLÈME: Injection de JavaScript malveillant

❌ VULNÉRABLE:
const userInput = "<script>alert('XSS')</script>";
document.innerHTML = userInput;  // Exécute le script !

✅ PROTECTION:
- Échapper le HTML (< devient &lt;)
- Utiliser textContent au lieu de innerHTML
- Sanitize user input
- Content Security Policy (CSP)

2️⃣ CLICKJACKING:

PROBLÈME: Site malveillant overlay invisible

PROTECTION HTML:
<head>
  <!-- Empêche iframe embedding -->
  <meta http-equiv="X-Frame-Options" content="DENY">
  
  <!-- Ou via CSP -->
  <meta http-equiv="Content-Security-Policy" content="frame-ancestors 'none'">
</head>

3️⃣ LIENS EXTERNES DANGEREUX:

❌ RISQUE:
<a href="malicious.com" target="_blank">Link</a>
// Site malveillant peut accéder à window.opener

✅ SÉCURISÉ:
<a href="external.com" target="_blank" rel="noopener noreferrer">
  Link
</a>

noopener    - Pas d'accès window.opener
noreferrer  - Pas d'envoi du referrer header

4️⃣ FORM SECURITY:

<!-- CSRF Protection -->
<form method="POST" action="/submit">
  <input type="hidden" name="csrf_token" value="random_token">
  
  <!-- Honeypot (piège pour bots) -->
  <input type="text" name="website" style="display:none">
  
  <input type="email" name="email" required>
  <button type="submit">Submit</button>
</form>

5️⃣ CONTENT SECURITY POLICY (CSP):

<meta http-equiv="Content-Security-Policy" 
      content="default-src 'self'; 
               script-src 'self' https://trusted.com; 
               style-src 'self' 'unsafe-inline';">

Empêche:
- Inline scripts malveillants
- Scripts de sources non autorisées
- Injection XSS

6️⃣ SUBRESOURCE INTEGRITY (SRI):

<script 
  src="https://cdn.example.com/lib.js"
  integrity="sha384-oqVuAfXRKap7fdgcCY5uykM6+R9GqQ8K..."
  crossorigin="anonymous">
</script>

Vérifie que le fichier CDN n'a pas été modifié.
```

---

## 7️⃣ HTML5 AVANCÉ

### 🎯 **APIs et Fonctionnalités Modernes**

```
1️⃣ SEMANTIC ELEMENTS (déjà vu mais important):

<article>, <section>, <nav>, <aside>, <header>, <footer>, <main>

2️⃣ MULTIMEDIA:

<video controls poster="thumbnail.jpg">
  <source src="video.mp4" type="video/mp4">
  <source src="video.webm" type="video/webm">
  <track kind="subtitles" src="subs.vtt" srclang="en">
  Your browser doesn't support video.
</video>

<audio controls>
  <source src="audio.mp3" type="audio/mp3">
  <source src="audio.ogg" type="audio/ogg">
</audio>

3️⃣ CANVAS & SVG:

<canvas id="myCanvas" width="200" height="200">
  Fallback content
</canvas>

<svg width="100" height="100">
  <circle cx="50" cy="50" r="40" fill="blue" />
</svg>

Canvas: Bitmap, JavaScript drawing
SVG: Vector, scalable, accessible

4️⃣ FORMS AVANCÉS:

<input type="email" required>
<input type="url">
<input type="tel">
<input type="number" min="0" max="100" step="5">
<input type="date">
<input type="time">
<input type="color">
<input type="range" min="0" max="100">
<input type="search">

<!-- Datalist (autocomplete native) -->
<input list="browsers">
<datalist id="browsers">
  <option value="Chrome">
  <option value="Firefox">
  <option value="Safari">
</datalist>

<!-- Pattern validation -->
<input type="text" pattern="[A-Za-z]{3,}" title="Min 3 letters">

5️⃣ DETAILS/SUMMARY (Accordion natif):

<details>
  <summary>Click to expand</summary>
  <p>Hidden content</p>
</details>

Natif, accessible, pas besoin de JavaScript !

6️⃣ DIALOG (Modal natif):

<dialog id="myDialog">
  <p>Dialog content</p>
  <button onclick="document.getElementById('myDialog').close()">
    Close
  </button>
</dialog>

<button onclick="document.getElementById('myDialog').showModal()">
  Open Modal
</button>

7️⃣ TEMPLATE:

<template id="my-template">
  <div class="card">
    <h2></h2>
    <p></p>
  </div>
</template>

<script>
  // Clone et utilise le template
  const template = document.getElementById('my-template');
  const clone = template.content.cloneNode(true);
  document.body.appendChild(clone);
</script>

8️⃣ WEB COMPONENTS (Shadow DOM, Custom Elements):

<my-custom-element></my-custom-element>

<script>
  class MyElement extends HTMLElement {
    connectedCallback() {
      this.innerHTML = '<p>Custom element</p>';
    }
  }
  customElements.define('my-custom-element', MyElement);
</script>
```

---

## 8️⃣ VALIDITÉ ET STANDARDS

### ✅ **HTML Valide**

```
POURQUOI LA VALIDATION IMPORTE:

1️⃣ COMPATIBILITÉ NAVIGATEURS:
HTML invalide → comportement imprévisible

2️⃣ ACCESSIBILITÉ:
Technologies d'assistance comptent sur HTML valide

3️⃣ SEO:
Moteurs de recherche préfèrent HTML bien formé

4️⃣ MAINTENANCE:
Code valide = plus facile à maintenir

RÈGLES DE BASE:

✅ Doctype correct:
<!DOCTYPE html>

✅ Structure de base:
<!DOCTYPE html>
<html lang="fr">
  <head>
    <meta charset="UTF-8">
    <title>Title</title>
  </head>
  <body>
    Content
  </body>
</html>

✅ Tags fermés correctement:
<div></div>
<img src="..." />  <!-- Self-closing -->

✅ Attributs quotés:
<div class="test">  <!-- ✅ -->
<div class=test>    <!-- ❌ -->

✅ IDs uniques:
<!-- ❌ -->
<div id="test"></div>
<div id="test"></div>

<!-- ✅ -->
<div id="test1"></div>
<div id="test2"></div>

✅ Nesting correct:
<!-- ❌ Invalide -->
<p><div>Text</div></p>  <!-- Block dans inline -->

<!-- ✅ Valide -->
<div><p>Text</p></div>

OUTILS DE VALIDATION:

- W3C Validator (validator.w3.org)
- Browser DevTools
- Lighthouse (Chrome)
- axe DevTools (Accessibilité)
```

---

## 9️⃣ INTERNATIONALISATION (i18n)

### 🌍 **HTML Multilingue**

```
1️⃣ ATTRIBUT LANG:

<!-- Page level -->
<html lang="fr">

<!-- Element level -->
<p>Ce texte est en <span lang="en">English</span></p>

Impact:
- Screen readers (prononciation)
- Moteurs de recherche
- Dictionnaires navigateurs
- Césure automatique

2️⃣ ATTRIBUT DIR (Direction):

<html dir="ltr">  <!-- Left to Right (défaut) -->
<html dir="rtl">  <!-- Right to Left (Arabe, Hébreu) -->

<!-- Mixed -->
<p dir="ltr">English text with <span dir="rtl">עברית</span></p>

3️⃣ TRANSLATE ATTRIBUTE:

<p translate="no">Brand Name</p>  <!-- Ne pas traduire -->
<p translate="yes">Translate this</p>

4️⃣ BDI & BDO (Bidirectional):

<bdi>اسم المستخدم</bdi>  <!-- Bidirectional Isolation -->
<bdo dir="rtl">Text</bdo>  <!-- Bidirectional Override -->

5️⃣ CHARSET:

<meta charset="UTF-8">  <!-- Support tous caractères -->

6️⃣ HREFLANG (SEO multilingue):

<link rel="alternate" hreflang="en" href="https://example.com/en">
<link rel="alternate" hreflang="fr" href="https://example.com/fr">
<link rel="alternate" hreflang="es" href="https://example.com/es">
```

---

## 🔟 QUESTIONS D'ENTRETIEN FRÉQUENTES

### Q1: **Quelle est la différence entre HTML et XHTML ?**

```
HTML:
- Plus permissif
- Tags peuvent ne pas être fermés
- Attributs sans quotes OK
- Case-insensitive

XHTML:
- XML strict
- TOUS les tags doivent être fermés
- Attributs DOIVENT être quotés
- Case-sensitive
- Un seul élément racine

EXEMPLE:

HTML (permissif):
<img src=image.jpg>
<br>
<P>Text

XHTML (strict):
<img src="image.jpg" />
<br />
<p>Text</p>

AUJOURD'HUI:
HTML5 est le standard, XHTML moins utilisé
```

---

### Q2: **Pourquoi utiliser des éléments sémantiques au lieu de <div> partout ?**

```
RAISONS:

1️⃣ ACCESSIBILITÉ:
Screen readers utilisent la sémantique pour naviguer
<nav> annonce "navigation landmark"
<main> annonce "main content"

2️⃣ SEO:
Moteurs de recherche comprennent mieux la structure
<article> = contenu indexable important
<header> = en-tête, moins important

3️⃣ MAINTENABILITÉ:
<header> vs <div class="header">
Plus clair pour les développeurs

4️⃣ CSS/JS:
Sélection plus facile
header { ... } vs .header { ... }

5️⃣ STANDARDS:
Code conforme aux standards Web

EXEMPLE IMPACT:

❌ <div class="article">
  Screen reader: "group"
  SEO: contenu normal

✅ <article>
  Screen reader: "article"
  SEO: contenu principal identifié
```

---

### Q3: **Qu'est-ce qu'un DOCTYPE et pourquoi est-il important ?**

```
DOCTYPE = Document Type Declaration

<!DOCTYPE html>  <!-- HTML5 (simple!) -->

FONCTION:
- Dit au navigateur quelle version HTML utiliser
- Active le "standards mode" vs "quirks mode"

QUIRKS MODE vs STANDARDS MODE:

Sans DOCTYPE ou DOCTYPE invalide:
→ Quirks Mode (mode compatibilité IE5)
→ Comportement imprévisible
→ Box model cassé
→ CSS inconsistant

Avec <!DOCTYPE html>:
→ Standards Mode
→ Comportement moderne et prévisible
→ CSS correct

HISTORIQUE:

HTML 4.01:
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" 
"http://www.w3.org/TR/html4/strict.dtd">


XHTML:
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

HTML5 (simplifié!):
<!DOCTYPE html>

TOUJOURS mettre <!DOCTYPE html> en première ligne!
```

---

### Q4: **Expliquez la différence entre <script>, <script async>, et <script defer>**

```
SANS ATTRIBUT (<script>):
HTML Parsing → STOP → Download Script → Execute → Resume Parsing

❌ Bloque le parsing HTML
❌ Peut ralentir le chargement page
✅ Ordre d'exécution garanti

AVEC ASYNC (<script async>):
HTML Parsing (continue) + Download Script → PAUSE → Execute → Resume

✅ Non-bloquant (téléchargement parallèle)
❌ Ordre d'exécution NON garanti
✅ Exécute dès disponible

Use case: Scripts indépendants (analytics, ads)

AVEC DEFER (<script defer>):
HTML Parsing (continue) + Download Script → Parsing Done → Execute All

✅ Non-bloquant
✅ Ordre d'exécution garanti
✅ Exécute après parsing complet (avant DOMContentLoaded)

Use case: Scripts qui dépendent du DOM complet

VISUALISATION:

Normal:    |--- HTML ---|STOP|--- Script ---|--- HTML ---|
Async:     |--- HTML --------------|PAUSE|Script|--- HTML ---|
Defer:     |--- HTML (Script download in parallel) ---|Scripts|

RECOMMANDATION:
<script src="app.js" defer></script>  // Défaut
<script src="analytics.js" async></script>  // Analytics

PIÈGE D'ENTRETIEN:
Q: "Quelle est la différence si le script est à la fin du <body>?"
R: defer et fin de body sont similaires, mais defer garantit
   l'exécution avant DOMContentLoaded event
```

---

### Q5: **Qu'est-ce que le Shadow DOM ?**

```
SHADOW DOM = Encapsulation pour Web Components

PROBLÈME SANS SHADOW DOM:
<div class="widget">
  <style>.title { color: red; }</style>
  <h1 class="title">Widget</h1>
</div>

→ Style .title affecte TOUTE la page (global)
→ Collision de noms
→ Pas d'encapsulation

AVEC SHADOW DOM:
<my-widget></my-widget>

<script>
  class MyWidget extends HTMLElement {
    constructor() {
      super();
      const shadow = this.attachShadow({ mode: 'open' });
      shadow.innerHTML = `
        <style>.title { color: red; }</style>
        <h1 class="title">Widget</h1>
      `;
    }
  }
  customElements.define('my-widget', MyWidget);
</script>

→ Style .title ISOLÉ dans le widget
→ Pas de fuite CSS
→ Encapsulation complète

3 DOM TYPES:

1. Light DOM (Normal):
   <div>Content</div>
   
2. Shadow DOM (Encapsulé):
   #shadow-root
     <style>...</style>
     <div>Content</div>

3. Virtual DOM (React):
   Abstraction JavaScript (pas de DOM réel)

USE CASES:
- Web Components
- Widgets réutilisables
- Bibliothèques de composants
```

---

### Q6: **Quelle est la différence entre <section> et <article> ?**

```
<ARTICLE>:
- Contenu AUTONOME et RÉUTILISABLE
- Doit avoir du sens sorti de son contexte
- Peut contenir des <section>

Exemples:
- Blog post
- News article
- Forum post
- Comment
- Widget indépendant

<SECTION>:
- Regroupement THÉMATIQUE
- Fait partie d'un tout
- Ne doit pas forcément avoir de sens seul
- Généralement contient un heading

Exemples:
- Chapitres d'un article
- Onglets d'une interface
- Sections numérotées

HIÉRARCHIE:

<article>                    ← Contenu autonome
  <header>
    <h1>Article Title</h1>
  </header>
  
  <section>                  ← Section thématique
    <h2>Introduction</h2>
    <p>...</p>
  </section>
  
  <section>
    <h2>Main Content</h2>
    <p>...</p>
  </section>
  
  <footer>
    <p>Author: John Doe</p>
  </footer>
</article>

RÈGLE PRATIQUE:
"Est-ce que ce contenu pourrait être dans un flux RSS?"
→ OUI: <article>
→ NON: <section>

EXEMPLES CONCRETS:

✅ BON:
<article>  <!-- Blog post -->
  <h1>How to Learn HTML</h1>
  <section>
    <h2>Basic Tags</h2>
    <p>...</p>
  </section>
  <section>
    <h2>Semantic HTML</h2>
    <p>...</p>
  </section>
</article>

<section>  <!-- Page section -->
  <h2>Latest Articles</h2>
  <article>...</article>
  <article>...</article>
</section>

❌ MAUVAIS:
<article>  <!-- Pas autonome -->
  <h2>Footer Links</h2>
  <a href="/about">About</a>
</article>
```

---

### Q7: **Qu'est-ce que le Critical Rendering Path et comment HTML l'affecte ?**

```
CRITICAL RENDERING PATH = Séquence d'étapes du navigateur
pour afficher la page

ÉTAPES:

1. HTML → DOM Tree
   Parse HTML, construit le DOM

2. CSS → CSSOM Tree
   Parse CSS, construit le CSSOM

3. DOM + CSSOM → Render Tree
   Combine les deux, crée l'arbre de rendu

4. Layout
   Calcule position et taille de chaque élément

5. Paint
   Dessine les pixels à l'écran

OPTIMISATIONS HTML:

1️⃣ MINIMISER LE DOM:
❌ <div><div><div><div>Content</div></div></div></div>
✅ <div>Content</div>

Moins de nœuds = parsing plus rapide

2️⃣ CRITICAL CSS INLINE:
<head>
  <style>
    /* Critical CSS pour above-the-fold */
    .header { ... }
  </style>
</head>

Évite le blocage du render par CSS externe

3️⃣ DEFER NON-CRITICAL RESOURCES:
<link rel="preload" href="font.woff2" as="font">
<script src="app.js" defer></script>

4️⃣ ÉVITER RENDER-BLOCKING RESOURCES:
<!-- ❌ Bloque -->
<head>
  <link rel="stylesheet" href="styles.css">
  <script src="app.js"></script>
</head>

<!-- ✅ Optimisé -->
<head>
  <style>/* Critical CSS inline */</style>
  <link rel="preload" href="styles.css" as="style" 
        onload="this.rel='stylesheet'">
</head>
<body>
  ...
  <script src="app.js" defer></script>
</body>

5️⃣ RESOURCE HINTS:
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="dns-prefetch" href="https://api.example.com">

MÉTRIQUES À CONNAÎTRE:

FCP (First Contentful Paint):
Quand le premier contenu apparaît

LCP (Largest Contentful Paint):
Quand le contenu principal est visible

TTI (Time to Interactive):
Quand la page devient interactive

IMPACT HTML:
- Taille du DOM affecte toutes les métriques
- Ordre des ressources affecte FCP/LCP
- Scripts bloquants affectent TTI
```

---

### Q8: **Expliquez le concept de Progressive Enhancement**

```
PROGRESSIVE ENHANCEMENT = Construire en couches

PHILOSOPHIE:
Commencer avec HTML fonctionnel de base,
puis ajouter CSS et JavaScript comme améliorations

COUCHES:

1️⃣ COUCHE 1: HTML (Structure/Contenu)
   → Fonctionne partout, même sans CSS/JS
   → Accessible à tous

2️⃣ COUCHE 2: CSS (Présentation)
   → Améliore l'apparence
   → Si désactivé, contenu reste accessible

3️⃣ COUCHE 3: JavaScript (Comportement)
   → Ajoute interactivité
   → Si désactivé, fonctionnalité de base reste

EXEMPLE: FORMULAIRE

<!-- Couche 1: HTML seul (fonctionne!) -->
<form action="/submit" method="POST">
  <label for="email">Email:</label>
  <input type="email" id="email" name="email" required>
  <button type="submit">Submit</button>
</form>

<!-- Couche 2: CSS (améliore look) -->
<style>
  form { max-width: 400px; }
  input { padding: 10px; }
  button { background: blue; color: white; }
</style>

<!-- Couche 3: JavaScript (ajoute validation live) -->
<script>
  document.querySelector('form').addEventListener('submit', (e) => {
    // Validation côté client (amélioration)
    if (!isValidEmail(email.value)) {
      e.preventDefault();
      showError('Invalid email');
    }
  });
</script>

AVANTAGES:
✅ Fonctionne pour tous (accessibilité)
✅ Robuste (dégradation gracieuse)
✅ SEO-friendly
✅ Performance (HTML se charge vite)

VS GRACEFUL DEGRADATION:
Progressive Enhancement: Base fonctionnelle → Améliorations
Graceful Degradation: Full-featured → Fallbacks

EXEMPLE CONCRET: TABS

<!-- HTML: Liste avec ancres (fonctionne sans JS) -->
<div class="tabs">
  <nav>
    <a href="#tab1">Tab 1</a>
    <a href="#tab2">Tab 2</a>
  </nav>
  
  <section id="tab1">
    <h2>Content 1</h2>
    <p>...</p>
  </section>
  
  <section id="tab2">
    <h2>Content 2</h2>
    <p>...</p>
  </section>
</div>

<!-- CSS: Cache sections sauf active -->
.tabs section:not(:target) { display: none; }

<!-- JS: Améliore avec animations -->
<script>
  // Progressive enhancement: ajoute smooth transitions
  document.querySelectorAll('.tabs a').forEach(link => {
    link.addEventListener('click', (e) => {
      // Smooth scroll, animations, etc.
    });
  });
</script>

SANS JS: Fonctionne avec hash navigation
AVEC JS: Animations fluides
```

---

### Q9: **Quelle est la différence entre data attributes et ARIA attributes ?**

```
DATA ATTRIBUTES (data-*):
Pour stocker des données custom pour JavaScript/CSS

<div 
  data-user-id="123"
  data-role="admin"
  data-color="blue"
>
  User Info
</div>

// JavaScript
element.dataset.userId; // "123"
element.dataset.role;   // "admin"

// CSS
[data-role="admin"] { color: red; }

USE CASES:
- Stocker métadonnées
- Configuration de composants
- State management léger
- Styling conditionnel

═══════════════════════════════════════

ARIA ATTRIBUTES (aria-*):
Pour améliorer l'accessibilité

<button
  aria-label="Close dialog"
  aria-pressed="false"
  aria-expanded="false"
>
  ✕
</button>

USE CASES:
- Accessibilité
- Screen readers
- Assistive technologies
- États dynamiques

═══════════════════════════════════════

DIFFÉRENCES CLÉS:

| Aspect         | data-*              | aria-*              |
|----------------|---------------------|---------------------|
| **But**        | Données custom      | Accessibilité       |
| **Utilisé par**| JavaScript/CSS      | Screen readers      |
| **Standard**   | HTML5               | WAI-ARIA            |
| **Impact**     | Logique app         | UX handicapés       |

ERREUR COURANTE:
❌ <div data-expanded="true">  <!-- Pour accessibilité -->
✅ <div aria-expanded="true">  <!-- Correct -->

RÈGLE:
- data-* = Données pour votre code
- aria-* = Données pour accessibilité
```

---

### Q10: **Qu'est-ce que le HTML Living Standard ?**

```
HTML LIVING STANDARD = Standard HTML actuel

HISTORIQUE:

HTML 1.0 (1993)
HTML 2.0 (1995)
HTML 3.2 (1997)
HTML 4.01 (1999)
XHTML 1.0 (2000)
HTML5 (2014)
HTML Living Standard (2019→)

CHANGEMENT DE PARADIGME:

AVANT (Versioning):
HTML4 → HTML5 → HTML6?
Versions fixes, longues périodes entre versions

MAINTENANT (Living Standard):
HTML (toujours la dernière version)
Évolution continue
Pas de numéros de version

GÉRÉ PAR:
WHATWG (Web Hypertext Application Technology Working Group)
Collaboration: Apple, Google, Mozilla, Microsoft

IMPLICATIONS:

✅ Nouvelles features ajoutées progressivement
✅ Pas d'attente pour "HTML6"
✅ Standard toujours à jour
✅ Navigateurs implémentent au fur et à mesure

EXEMPLE:
<dialog> ajouté progressivement (pas dans "HTML 5.1")
Lazy loading ajouté sans nouvelle version

POUR LES DÉVELOPPEURS:
- Suivre whatwg.org/html
- Vérifier browser support (caniuse.com)
- Progressive enhancement

DOCTYPE RESTE:
<!DOCTYPE html>
(Pas de version numérique!)
```

---

## 1️⃣1️⃣ CONCEPTS MODERNES

### 🚀 **HTML et Web Components**

```
WEB COMPONENTS = Standards pour créer composants réutilisables

3 TECHNOLOGIES:

1️⃣ CUSTOM ELEMENTS:
<my-button></my-button>

class MyButton extends HTMLElement {
  connectedCallback() {
    this.innerHTML = '<button>Click me</button>';
  }
}
customElements.define('my-button', MyButton);

2️⃣ SHADOW DOM:
Encapsulation CSS/JS

3️⃣ HTML TEMPLATES:
<template id="my-template">
  <div class="card">
    <slot name="title"></slot>
    <slot name="content"></slot>
  </div>
</template>

SLOTS:
<my-card>
  <h2 slot="title">Title</h2>
  <p slot="content">Content</p>
</my-card>

AVANTAGES:
✅ Natif (pas de framework)
✅ Réutilisable
✅ Encapsulé
✅ Standard Web

USE CASES:
- Design systems
- Widget libraries
- Composants cross-framework
```

---

### 🎯 **HTML et Performance Web**

```
CORE WEB VITALS (Google):

1️⃣ LCP (Largest Contentful Paint):
   Temps pour afficher le plus gros élément visible
   
   HTML Impact:
   - Taille des images
   - Ordre des éléments
   - Resource hints

2️⃣ FID (First Input Delay):
   Temps avant que la page réponde à une interaction
   
   HTML Impact:
   - Scripts bloquants
   - defer/async

3️⃣ CLS (Cumulative Layout Shift):
   Stabilité visuelle (éléments qui bougent)
   
   HTML Impact:
   - width/height sur images
   - Reserve space pour ads
   - Font loading

OPTIMISATIONS HTML:

<!-- Images avec dimensions (évite CLS) -->
<img src="hero.jpg" width="1200" height="600" alt="Hero">

<!-- Preload hero image (améliore LCP) -->
<link rel="preload" as="image" href="hero.jpg">

<!-- Defer non-critical JS (améliore FID) -->
<script src="app.js" defer></script>

<!-- Font display swap (évite FOIT) -->
<link rel="preload" href="font.woff2" as="font" crossorigin>
<style>
  @font-face {
    font-family: 'MyFont';
    src: url('font.woff2');
    font-display: swap;  /* Affiche fallback en attendant */
  }
</style>
```

---

## 1️⃣2️⃣ CONSEILS POUR L'ENTRETIEN

### 💡 **Comment Bien Répondre**

```
STRUCTURE DE RÉPONSE:

1. DÉFINITION COURTE
2. POURQUOI C'EST IMPORTANT
3. EXEMPLE CONCRET
4. AVANTAGES/INCONVÉNIENTS
5. EXPÉRIENCE PERSONNELLE (vos projets)

EXEMPLE:

Q: "Qu'est-ce que la sémantique HTML?"

R: "La sémantique HTML, c'est utiliser les bonnes balises pour
   donner du sens au contenu, pas juste pour l'apparence.
   
   C'est important pour 3 raisons:
   1. Accessibilité: Screen readers comprennent la structure
   2. SEO: Google indexe mieux
   3. Maintenance: Code plus clair
   
   Par exemple, dans mon projet [nom], j'ai utilisé <article>
   pour les blog posts au lieu de <div>, ce qui a amélioré
   l'accessibilité et le SEO.
   
   L'inconvénient: légère courbe d'apprentissage, mais les
   bénéfices dépassent largement."

MOTS-CLÉS À UTILISER:
✅ "Sémantique"
✅ "Accessibilité"
✅ "Performance"
✅ "SEO"
✅ "Standards Web"
✅ "Progressive Enhancement"
✅ "Separation of Concerns"
```

---

### 🎯 **Questions à Poser au Recruteur**

```
BONNES QUESTIONS (montrent votre expertise):

1. "Quelle importance accordez-vous à l'accessibilité (A11Y)?"
   → Montre que vous pensez inclusif

2. "Utilisez-vous des audits Lighthouse régulièrement?"
   → Montre votre souci de performance

3. "Avez-vous des guidelines pour la sémantique HTML?"
   → Montre votre souci de qualité

4. "Comment gérez-vous le SEO technique?"
   → Montre votre vision holistique

5. "Utilisez-vous des Web Components ou frameworks comme React?"
   → Comprendre la stack technique

6. "Quelle est votre stratégie pour les Core Web Vitals?"
   → Montre que vous êtes à jour

7. "Comment testez-vous l'accessibilité?"
   → Montre votre professionnalisme
```

---

## 📝 RÉSUMÉ FINAL

### ✅ **Points Essentiels à Retenir**

```
1️⃣ SÉMANTIQUE:
   Utiliser les bonnes balises pour le bon contenu
   <header>, <nav>, <main>, <article>, <section>, etc.

2️⃣ ACCESSIBILITÉ:
   HTML valide + ARIA quand nécessaire
   Navigation clavier, alt text, labels

3️⃣ SEO:
   Titres hiérarchiques, meta tags, structured data
   URLs sémantiques, sitemap

4️⃣ PERFORMANCE:
   DOM léger, lazy loading, defer/async
   Resource hints, critical CSS

5️⃣ SÉCURITÉ:
   Sanitize inputs, noopener, CSP
   HTTPS, SRI

6️⃣ STANDARDS:
   HTML Living Standard
   Validation W3C
   Progressive Enhancement

7️⃣ MODERNE:
   Web Components, Shadow DOM
   Core Web Vitals
   APIs HTML5

PHRASE CLÉ:
"HTML n'est pas juste du markup, c'est la fondation
 de l'accessibilité, du SEO, et de la performance Web."
```

---

### 🎓 **Derniers Conseils**

```
AVANT L'ENTRETIEN:
✅ Relire vos projets (code HTML)
✅ Tester accessibilité (screen reader)
✅ Vérifier validité W3C
✅ Analyser avec Lighthouse

PENDANT L'ENTRETIEN:
✅ Toujours mentionner accessibilité
✅ Parler de sémantique naturellement
✅ Lier au SEO quand pertinent
✅ Citer vos projets concrets

PIÈGES À ÉVITER:
❌ "HTML c'est facile, tout le monde connaît"
❌ Ignorer l'accessibilité
❌ Confondre présentation et structure
❌ Ne pas connaître HTML5 moderne

POINTS BONUS:
✅ Mentionner Core Web Vitals
✅ Parler de Progressive Enhancement
✅ Connaître ARIA
✅ Comprendre Shadow DOM/Web Components
```

---