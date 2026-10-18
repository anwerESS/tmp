# 📘 CSS - Résumé Conceptuel pour Entretien

## 🎯 Concepts Avancés & Questions Pièges

---

## 1️⃣ FONDAMENTAUX CONCEPTUELS

### 📝 **Qu'est-ce que CSS ? (Vision Approfondie)**

```
CSS = Cascading Style Sheets

CASCADING (Cascade):
- Héritage des styles
- Priorité/spécificité
- Ordre d'application

STYLE:
- Présentation visuelle
- Layout/mise en page
- Responsive design

SHEETS:
- Feuilles de style
- Séparation présentation/contenu
- Réutilisabilité

CSS N'EST PAS:
❌ Un langage de programmation (pas de logique)
❌ Un langage de structure
❌ Un langage de comportement

CSS EST:
✅ Le style/apparence de la page
✅ La mise en page (layout)
✅ L'adaptation responsive
✅ Les animations et transitions
```

---

### 🎯 **La Cascade : Comprendre la Priorité**

```
LA CASCADE = Ordre de priorité des styles

FACTEURS DE PRIORITÉ (du plus au moins important):

1️⃣ IMPORTANCE (!important):
   color: red !important;  /* Priorité absolue */
   ⚠️ À éviter ! Casse la cascade naturelle

2️⃣ INLINE STYLES:
   <div style="color: red;">  /* Très prioritaire */
   ⚠️ Éviter (mélange HTML et CSS)

3️⃣ SPÉCIFICITÉ:
   #id > .class > element

4️⃣ SOURCE ORDER (Ordre):
   Si même spécificité, le dernier gagne

CALCUL DE SPÉCIFICITÉ:

(inline, IDs, classes/attributes/pseudo-classes, elements/pseudo-elements)

Exemples:
div                          → (0, 0, 0, 1)    = 1
.button                      → (0, 0, 1, 0)    = 10
div.button                   → (0, 0, 1, 1)    = 11
#header .nav a               → (0, 1, 1, 1)    = 111
#header .nav a:hover         → (0, 1, 2, 1)    = 121

RÈGLE:
Plus le nombre est grand, plus prioritaire

EXEMPLES PRATIQUES:

/* Spécificité: (0, 0, 1, 0) = 10 */
.button { color: blue; }

/* Spécificité: (0, 1, 0, 0) = 100 - GAGNE */
#nav { color: red; }

/* Spécificité: (0, 0, 2, 1) = 21 */
div.container.main { color: green; }

/* Spécificité: (0, 0, 1, 2) = 12 */
header nav .link { color: purple; }

PIÈGE D'ENTRETIEN:
Q: "Quelle couleur sera appliquée ?"

.text { color: blue; }           /* (0,0,1,0) = 10 */
div.text { color: red; }         /* (0,0,1,1) = 11 - GAGNE */
.container .text { color: green; } /* (0,0,2,0) = 20 - GAGNE */

R: green (spécificité la plus haute)

HÉRITAGE vs CASCADE:

HÉRITAGE:
Propriétés qui descendent du parent aux enfants
- color
- font-family
- font-size
- text-align
- line-height

<div style="color: red;">
  <p>Je suis rouge aussi</p>  <!-- Hérite color -->
</div>

NE S'HÉRITENT PAS:
- margin, padding
- border
- background
- width, height
- position

CASCADE:
Résolution des conflits entre plusieurs règles
pour la MÊME propriété du MÊME élément
```

---

## 2️⃣ BOX MODEL

### 📦 **Le Modèle de Boîte**

```
STRUCTURE D'UN ÉLÉMENT:

┌─────────────────────────────────────┐
│          MARGIN (transparent)        │
│  ┌───────────────────────────────┐  │
│  │   BORDER                      │  │
│  │  ┌─────────────────────────┐  │  │
│  │  │   PADDING               │  │  │
│  │  │  ┌───────────────────┐  │  │  │
│  │  │  │    CONTENT        │  │  │  │
│  │  │  │   (width/height)  │  │  │  │
│  │  │  └───────────────────┘  │  │  │
│  │  └─────────────────────────┘  │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘

CALCUL DE LA LARGEUR TOTALE (par défaut):

Total width = 
  margin-left 
  + border-left 
  + padding-left 
  + width 
  + padding-right 
  + border-right 
  + margin-right

Exemple:
.box {
  width: 200px;
  padding: 20px;
  border: 5px solid;
  margin: 10px;
}

Largeur visible = 200 + 20 + 20 + 5 + 5 = 250px
Largeur totale = 250 + 10 + 10 = 270px

PROBLÈME:
Difficile de calculer les dimensions exactes !

═══════════════════════════════════════

BOX-SIZING: SOLUTION MODERNE

/* Par défaut (content-box) */
box-sizing: content-box;
/* width = seulement content */

/* Moderne (border-box) ✅ */
box-sizing: border-box;
/* width = content + padding + border */

AVEC BORDER-BOX:
.box {
  box-sizing: border-box;
  width: 200px;           /* Largeur TOTALE */
  padding: 20px;
  border: 5px solid;
}
/* Largeur visible = 200px exactement ! */
/* Content = 200 - 40 (padding) - 10 (border) = 150px */

RESET UNIVERSEL (Best Practice):
*,
*::before,
*::after {
  box-sizing: border-box;
}

POURQUOI BORDER-BOX?
✅ Calculs plus simples
✅ width = largeur finale visible
✅ Pas de surprises
✅ Standard moderne

PIÈGE D'ENTRETIEN:
Q: "Pourquoi mon élément de 200px fait 250px de large?"
R: "Probablement box-sizing: content-box (défaut) avec 
    padding/border. Solution: box-sizing: border-box"
```

---

## 3️⃣ POSITIONING (POSITIONNEMENT)

### 📍 **Les 5 Types de Position**

```
1️⃣ STATIC (par défaut):
   Position normale dans le flux du document
   top/right/bottom/left n'ont AUCUN effet

.element { position: static; }

2️⃣ RELATIVE:
   Position relative à sa position NORMALE
   Reste dans le flux (occupe son espace)

.element {
  position: relative;
  top: 10px;      /* 10px plus bas que normal */
  left: 20px;     /* 20px plus à droite que normal */
}

Use case: Base pour position absolute des enfants

3️⃣ ABSOLUTE:
   Position relative au parent positionné le plus proche
   SORTI du flux (ne prend plus d'espace)

.parent {
  position: relative;  /* Référence pour enfant */
}

.child {
  position: absolute;
  top: 0;
  right: 0;  /* Coin supérieur droit du parent */
}

Use case: Overlays, tooltips, badges

4️⃣ FIXED:
   Position relative au VIEWPORT (fenêtre navigateur)
   SORTI du flux
   Reste fixe au scroll

.navbar {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;  /* Header fixe */
}

Use case: Navbar fixe, bouton "back to top"

5️⃣ STICKY:
   Hybride relative/fixed
   Relative jusqu'à un seuil, puis fixed
   RESTE dans le flux

.header {
  position: sticky;
  top: 0;  /* Devient fixe quand atteint top: 0 */
}

Use case: Table headers, section headers

═══════════════════════════════════════

STACKING CONTEXT (z-index):

z-index contrôle l'ordre de superposition
Fonctionne SEULEMENT si position ≠ static

.layer1 { z-index: 1; }   /* Derrière */
.layer2 { z-index: 10; }  /* Devant */
.layer3 { z-index: 100; } /* Tout devant */

PIÈGES:

1. z-index ne marche pas sur position: static
2. z-index crée un stacking context
3. Enfants ne peuvent pas sortir du stacking context parent

.parent {
  position: relative;
  z-index: 1;
}

.child {
  position: absolute;
  z-index: 9999;  /* Ne peut pas passer AU-DESSUS d'un 
                     élément avec z-index: 2 au même niveau que parent */
}

COMPRENDRE LE CONTAINING BLOCK:

Absolute:
- Relatif au premier parent avec position ≠ static
- Si aucun: relatif à <html>

Fixed:
- Toujours relatif au viewport
- Exception: si parent a transform/filter

Sticky:
- Relatif au parent scrollable
```

---

## 4️⃣ FLEXBOX

### 🎯 **Disposition Flexible Unidimensionnelle**

```
FLEXBOX = Layout en 1 dimension (rangée OU colonne)

CONCEPTS CLÉS:

┌─────────────────────────────────────────────┐
│ FLEX CONTAINER                              │
│                                             │
│  [Item 1]  [Item 2]  [Item 3]  [Item 4]   │ ← Main Axis
│                                             │
└─────────────────────────────────────────────┘
      ↑
   Cross Axis

CONTAINER PROPERTIES:

display: flex;              /* Active flexbox */
flex-direction: row;        /* row | column | row-reverse | column-reverse */
flex-wrap: wrap;            /* wrap | nowrap | wrap-reverse */
justify-content: center;    /* Main axis: start|center|end|space-between|space-around|space-evenly */
align-items: center;        /* Cross axis: start|center|end|stretch|baseline */
align-content: center;      /* Multi-ligne seulement */
gap: 1rem;                  /* Espacement entre items */

ITEM PROPERTIES:

flex-grow: 1;               /* Peut grandir (ratio) */
flex-shrink: 1;             /* Peut rétrécir (ratio) */
flex-basis: 200px;          /* Taille de base */
flex: 1 1 200px;            /* Shorthand: grow shrink basis */
align-self: flex-end;       /* Override align-items pour cet item */
order: 2;                   /* Ordre d'affichage */

═══════════════════════════════════════

PATTERNS COURANTS:

1️⃣ CENTRAGE PARFAIT:
.container {
  display: flex;
  justify-content: center;  /* Horizontal */
  align-items: center;      /* Vertical */
  min-height: 100vh;
}

2️⃣ NAVBAR:
.navbar {
  display: flex;
  justify-content: space-between;  /* Logo à gauche, menu à droite */
  align-items: center;
}

3️⃣ GRILLE RESPONSIVE:
.grid {
  display: flex;
  flex-wrap: wrap;
  gap: 1rem;
}

.item {
  flex: 1 1 300px;  /* Min 300px, grow/shrink */
}

4️⃣ STICKY FOOTER:
body {
  display: flex;
  flex-direction: column;
  min-height: 100vh;
}

main {
  flex: 1;  /* Prend tout l'espace disponible */
}

5️⃣ SIDEBAR LAYOUT:
.container {
  display: flex;
}

.sidebar {
  flex: 0 0 250px;  /* Fixe 250px */
}

.main {
  flex: 1;  /* Prend le reste */
}

═══════════════════════════════════════

FLEX SHORTHAND:

/* flex: grow shrink basis */

flex: 1;
/* = flex: 1 1 0% */
/* Distribue espace équitablement */

flex: auto;
/* = flex: 1 1 auto */
/* Basé sur contenu, puis distribue */

flex: none;
/* = flex: 0 0 auto */
/* Taille fixe selon contenu */

flex: 0 0 200px;
/* Taille fixe 200px, ne grandit ni rétrécit */

═══════════════════════════════════════

JUSTIFY-CONTENT vs ALIGN-ITEMS:

justify-content:
- Axe PRINCIPAL (main axis)
- row → horizontal
- column → vertical

align-items:
- Axe SECONDAIRE (cross axis)
- row → vertical
- column → horizontal

/* Flexbox horizontal */
.container {
  display: flex;
  flex-direction: row;
  justify-content: center;  /* Centre horizontalement */
  align-items: center;      /* Centre verticalement */
}

/* Flexbox vertical */
.container {
  display: flex;
  flex-direction: column;
  justify-content: center;  /* Centre verticalement */
  align-items: center;      /* Centre horizontalement */
}

PIÈGE D'ENTRETIEN:
Q: "Comment centrer un élément avec Flexbox?"
R: "display: flex; justify-content: center; align-items: center;
    sur le parent"
```

---

## 5️⃣ GRID

### 🎯 **Disposition Bidimensionnelle**

```
GRID = Layout en 2 dimensions (rangées ET colonnes)

┌─────┬─────┬─────┐
│  1  │  2  │  3  │  ← Row 1
├─────┼─────┼─────┤
│  4  │  5  │  6  │  ← Row 2
└─────┴─────┴─────┘
  ↑     ↑     ↑
Col 1  Col 2  Col 3

CONTAINER PROPERTIES:

display: grid;
grid-template-columns: 200px 200px 200px;
/* Ou */
grid-template-columns: repeat(3, 200px);
/* Ou */
grid-template-columns: 1fr 1fr 1fr;
/* Ou */
grid-template-columns: repeat(3, 1fr);

grid-template-rows: 100px auto 100px;
gap: 1rem;                      /* row-gap + column-gap */
grid-auto-flow: row;            /* row | column | dense */

/* Alignment */
justify-items: center;          /* Items dans leur cellule (horizontal) */
align-items: center;            /* Items dans leur cellule (vertical) */
justify-content: center;        /* Grid entière (horizontal) */
align-content: center;          /* Grid entière (vertical) */

ITEM PROPERTIES:

grid-column: 1 / 3;            /* Span colonnes 1 à 3 */
grid-row: 1 / 2;               /* Span rangée 1 à 2 */
/* Ou */
grid-column: span 2;           /* Span 2 colonnes */
grid-area: header;             /* Nom de zone */

═══════════════════════════════════════

FR UNIT (Fraction):

/* fr = fraction de l'espace disponible */

grid-template-columns: 1fr 2fr 1fr;
/* Colonne 2 est 2x plus large que colonnes 1 et 3 */

grid-template-columns: 200px 1fr 1fr;
/* 200px fixe, reste divisé en 2 */

═══════════════════════════════════════

PATTERNS COURANTS:

1️⃣ GRILLE RESPONSIVE (auto-fit):
.grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 1rem;
}
/* Colonnes s'ajustent automatiquement */

2️⃣ HOLY GRAIL LAYOUT:
.container {
  display: grid;
  grid-template-areas:
    "header header header"
    "sidebar main aside"
    "footer footer footer";
  grid-template-columns: 200px 1fr 200px;
  grid-template-rows: auto 1fr auto;
  min-height: 100vh;
}

.header  { grid-area: header; }
.sidebar { grid-area: sidebar; }
.main    { grid-area: main; }
.aside   { grid-area: aside; }
.footer  { grid-area: footer; }

3️⃣ CARD AVEC FOOTER STICKY:
.card {
  display: grid;
  grid-template-rows: auto 1fr auto;
}

.card-body {
  /* Prend l'espace disponible */
}

4️⃣ GALLERY ASYMÉTRIQUE:
.gallery {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  grid-auto-rows: 200px;
  gap: 1rem;
}

.item-large {
  grid-column: span 2;
  grid-row: span 2;
}

═══════════════════════════════════════

AUTO-FIT vs AUTO-FILL:

auto-fit:
/* Collapse empty tracks */
grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
/* Items s'étirent pour remplir */

auto-fill:
/* Garde empty tracks */
grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
/* Items gardent leur taille */

═══════════════════════════════════════

NAMED LINES:

.grid {
  display: grid;
  grid-template-columns: 
    [start] 1fr 
    [content-start] 2fr 
    [content-end] 1fr 
    [end];
}

.item {
  grid-column: content-start / content-end;
}

═══════════════════════════════════════

FLEXBOX vs GRID:

FLEXBOX (1D):
✅ Composants
✅ Navigation
✅ 1 dimension (row OU column)
✅ Content-first (taille basée sur contenu)

GRID (2D):
✅ Page layouts
✅ Grilles complexes
✅ 2 dimensions (rows ET columns)
✅ Layout-first (taille définie à l'avance)

COMBINAISON:
.page {
  display: grid;  /* Layout général */
  grid-template-columns: 250px 1fr;
}

.navbar {
  display: flex;  /* Composant navbar */
  justify-content: space-between;
}
```

---

## 6️⃣ RESPONSIVE DESIGN

### 📱 **Media Queries et Stratégies**

```
MEDIA QUERIES:

@media (condition) {
  /* Styles conditionnels */
}

CONDITIONS COURANTES:

/* Largeur */
@media (max-width: 768px) { }     /* Mobile */
@media (min-width: 769px) { }     /* Desktop */
@media (min-width: 769px) and (max-width: 1024px) { } /* Tablet */

/* Orientation */
@media (orientation: portrait) { }
@media (orientation: landscape) { }

/* Ratio */
@media (aspect-ratio: 16/9) { }

/* Résolution */
@media (min-resolution: 2dppx) { }  /* Retina */

/* Préférences utilisateur */
@media (prefers-color-scheme: dark) { }
@media (prefers-reduced-motion: reduce) { }

/* Hover capability */
@media (hover: hover) { }           /* Desktop */
@media (hover: none) { }            /* Touch */

═══════════════════════════════════════

BREAKPOINTS STANDARDS:

/* Mobile-First (recommandé) */
/* Base: Mobile */
.container { width: 100%; }

/* Tablet */
@media (min-width: 768px) {
  .container { width: 750px; }
}

/* Desktop */
@media (min-width: 1024px) {
  .container { width: 970px; }
}

/* Large Desktop */
@media (min-width: 1280px) {
  .container { width: 1200px; }
}

BREAKPOINTS COMMUNS:
- 480px  : Mobile landscape
- 768px  : Tablet
- 1024px : Desktop
- 1280px : Large desktop
- 1920px : Extra large

═══════════════════════════════════════

MOBILE-FIRST vs DESKTOP-FIRST:

MOBILE-FIRST (✅ Recommandé):
/* Base = Mobile */
.element { font-size: 14px; }

/* Agrandir pour desktop */
@media (min-width: 768px) {
  .element { font-size: 16px; }
}

Avantages:
✅ Performance mobile (charge moins)
✅ Progressive enhancement
✅ Priorité au mobile (80% trafic)

DESKTOP-FIRST (⚠️ Ancien):
/* Base = Desktop */
.element { font-size: 16px; }

/* Réduire pour mobile */
@media (max-width: 767px) {
  .element { font-size: 14px; }
}

═══════════════════════════════════════

RESPONSIVE UNITS:

RELATIVE:
em   : Relatif au font-size parent
rem  : Relatif au font-size root (<html>)
%    : Relatif au parent
vw/vh: Relatif au viewport (1vw = 1% largeur)
vmin : Plus petit entre vw et vh
vmax : Plus grand entre vw et vh

ABSOLUE:
px   : Pixels (fixe)

RECOMMANDATIONS:
✅ rem : Font sizes, spacing
✅ %/fr: Layouts
✅ vw/vh: Full-screen sections
❌ px  : À éviter (sauf borders, shadows)

Exemple:
html { font-size: 16px; }

.heading { 
  font-size: 2rem;      /* 32px */
  margin-bottom: 1rem;  /* 16px */
}

@media (min-width: 768px) {
  html { font-size: 18px; }
  /* .heading devient automatiquement 36px ! */
}

═══════════════════════════════════════

CONTAINER QUERIES (Nouveau!):

/* Au lieu de viewport size, base sur container size */

.container {
  container-type: inline-size;
  container-name: card;
}

@container card (min-width: 400px) {
  .card-content {
    display: flex;
  }
}

Avantages:
✅ Composants vraiment réutilisables
✅ Adaptent au container, pas au viewport
✅ Futur du responsive

═══════════════════════════════════════

FLUID TYPOGRAPHY:

/* Taille s'adapte progressivement */

.text {
  font-size: clamp(1rem, 2vw + 0.5rem, 2rem);
  /* Min: 1rem, Préféré: 2vw+0.5rem, Max: 2rem */
}

/* Ou fonction calculée */
.text {
  font-size: calc(1rem + 1vw);
}

RESPONSIVE IMAGES:

<!-- Srcset (différentes résolutions) -->
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

<!-- Picture (art direction) -->
<picture>
  <source media="(max-width: 600px)" srcset="mobile.jpg">
  <source media="(max-width: 1000px)" srcset="tablet.jpg">
  <img src="desktop.jpg" alt="Description">
</picture>

<!-- CSS */
.hero {
  background-image: image-set(
    "hero.jpg" 1x,
    "hero@2x.jpg" 2x
  );
}
```

---

## 7️⃣ ANIMATIONS & TRANSITIONS

### ✨ **Mouvement et Interactivité**

```
TRANSITIONS:
Changement progressif entre 2 états

.button {
  background: blue;
  transition: background 0.3s ease;
}

.button:hover {
  background: darkblue;
}

Syntaxe:
transition: property duration timing-function delay;

/* Exemples */
transition: all 0.3s ease;
transition: transform 0.2s ease-in-out;
transition: opacity 0.5s linear 0.1s;

/* Multiple propriétés */
transition: 
  background 0.3s ease,
  transform 0.2s ease-in-out,
  box-shadow 0.3s ease;

TIMING FUNCTIONS:
ease          : Lent-Rapide-Lent (défaut)
linear        : Constant
ease-in       : Lent au début
ease-out      : Lent à la fin
ease-in-out   : Lent aux deux bouts
cubic-bezier(): Custom (ex: cubic-bezier(0.68, -0.55, 0.265, 1.55))

═══════════════════════════════════════

ANIMATIONS:
Séquences complexes avec keyframes

@keyframes slideIn {
  0% {
    transform: translateX(-100%);
    opacity: 0;
  }
  100% {
    transform: translateX(0);
    opacity: 1;
  }
}

.element {
  animation: slideIn 0.5s ease-out;
}

/* Ou */
@keyframes bounce {
  0%, 100% { transform: translateY(0); }
  50% { transform: translateY(-20px); }
}

.element {
  animation: bounce 1s infinite;
}

Syntaxe complète:
animation: name duration timing-function delay iteration-count direction fill-mode play-state;

/* Exemples */
animation: fadeIn 1s ease-in-out 0.5s infinite alternate both running;

animation-name: fadeIn;
animation-duration: 1s;
animation-timing-function: ease-in-out;
animation-delay: 0.5s;
animation-iteration-count: infinite;  /* ou nombre */
animation-direction: alternate;       /* normal|reverse|alternate|alternate-reverse */
animation-fill-mode: both;            /* none|forwards|backwards|both */
animation-play-state: running;        /* running|paused */

═══════════════════════════════════════

TRANSFORMS:
Modifications géométriques

/* 2D Transforms */
transform: translate(50px, 100px);
transform: translateX(50px);
transform: translateY(100px);
transform: scale(1.5);
transform: scaleX(2);
transform: rotate(45deg);
transform: skew(10deg, 20deg);

/* 3D Transforms */
transform: perspective(1000px) rotateY(45deg);
transform: translateZ(50px);
transform: rotate3d(1, 1, 0, 45deg);

/* Combine */
transform: translate(50px, 0) rotate(45deg) scale(1.2);

/* Transform origin */
transform-origin: center center;  /* Défaut */
transform-origin: top left;
transform-origin: 50% 50%;

═══════════════════════════════════════

PERFORMANCE:

PROPRIÉTÉS PERFORMANTES (GPU):
✅ transform     (translateX, rotate, scale)
✅ opacity

PROPRIÉTÉS COÛTEUSES (CPU):
❌ width/height
❌ top/left/right/bottom
❌ margin/padding

POURQUOI?
- transform/opacity: Composite layer (GPU)
- Autres: Reflow + Repaint (CPU)

EXEMPLE OPTIMISÉ:

/* ❌ Lent */
.element {
  transition: left 0.3s;
}
.element:hover {
  left: 100px;
}

/* ✅ Rapide */
.element {
  transition: transform 0.3s;
}
.element:hover {
  transform: translateX(100px);
}

WILL-CHANGE:
Prépare navigateur pour changement

.element {
  will-change: transform, opacity;
}

⚠️ À utiliser avec modération (coûteux en mémoire)

═══════════════════════════════════════

ANIMATIONS ACCESSIBLES:

/* Respecter préférence utilisateur */
@media (prefers-reduced-motion: reduce) {
  *,
  *::before,
  *::after {
    animation-duration: 0.01ms !important;
    animation-iteration-count: 1 !important;
    transition-duration: 0.01ms !important;
  }
}

═══════════════════════════════════════

EXEMPLES PRATIQUES:

1️⃣ FADE IN:
@keyframes fadeIn {
  from { opacity: 0; }
  to { opacity: 1; }
}

.element { animation: fadeIn 0.5s ease-out; }

2️⃣ SLIDE UP:
@keyframes slideUp {
  from {
    transform: translateY(20px);
    opacity: 0;
  }
  to {
    transform: translateY(0);
    opacity: 1;
  }
}

3️⃣ PULSE:
@keyframes pulse {
  0%, 100% { transform: scale(1); }
  50% { transform: scale(1.1); }
}

4️⃣ ROTATION INFINIE:
@keyframes spin {
  from { transform: rotate(0deg); }
  to { transform: rotate(360deg); }
}

.loading { animation: spin 1s linear infinite; }

5️⃣ SHAKE:
@keyframes shake {
  0%, 100% { transform: translateX(0); }
  25% { transform: translateX(-10px); }
  75% { transform: translateX(10px); }
}

.error { animation: shake 0.3s ease-in-out; }
```

---

## 8️⃣ PSEUDO-CLASSES & PSEUDO-ELEMENTS


### 🎭 **Sélecteurs Avancés**

PSEUDO-CLASSES (:):
Sélectionnent éléments selon leur ÉTAT

/* Interaction */
:hover          /* Survol souris */
:active         /* Click actif */
:focus          /* Focus clavier/click */
:focus-within   /* Parent d'élément focus */
:focus-visible  /* Focus clavier seulement (pas click) */

/* État de lien */
:link           /* Lien non visité */
:visited        /* Lien visité */

/* État de formulaire */
:checked        /* Checkbox/radio coché */
:disabled       /* Input désactivé */
:enabled        /* Input activé */
:required       /* Champ requis */
:optional       /* Champ optionnel */
:valid          /* Validation réussie */
:invalid        /* Validation échouée */
:in-range       /* Valeur dans range */
:out-of-range   /* Valeur hors range */
:placeholder-shown /* Placeholder visible */

/* Position dans parent */
:first-child    /* Premier enfant */
:last-child     /* Dernier enfant */
:nth-child(n)   /* Nième enfant */
:nth-child(odd) /* Enfants impairs (1,3,5...) */
:nth-child(even)/* Enfants pairs (2,4,6...) */
:nth-child(2n+1)/* Formule custom */
:only-child     /* Enfant unique */

/* Position par type */
:first-of-type
:last-of-type
:nth-of-type(n)
:only-of-type

/* Négation */
:not(selector)  /* Tous sauf... */

/* Autres */
:empty          /* Élément vide */
:root           /* Racine document (<html>) */
:target         /* Élément ciblé par ancre (#id) */

═══════════════════════════════════════

PSEUDO-ELEMENTS (::):
Créent/stylent des PARTIES d'éléments

::before        /* Contenu avant */
::after         /* Contenu après */
::first-letter  /* Première lettre */
::first-line    /* Première ligne */
::selection     /* Texte sélectionné */
::placeholder   /* Placeholder input */
::marker        /* Puce de liste */
::backdrop      /* Backdrop de dialog */

EXEMPLES PRATIQUES:

1️⃣ ICÔNES AVEC ::before:
.icon::before {
  content: "★";
  margin-right: 0.5rem;
}

2️⃣ DÉCORATIONS:
.fancy::after {
  content: "";
  display: block;
  width: 50px;
  height: 2px;
  background: blue;
  margin-top: 1rem;
}

3️⃣ CLEARFIX (ancien):
.clearfix::after {
  content: "";
  display: table;
  clear: both;
}

4️⃣ TOOLTIPS:
.tooltip {
  position: relative;
}

.tooltip::after {
  content: attr(data-tooltip);
  position: absolute;
  bottom: 100%;
  left: 50%;
  transform: translateX(-50%);
  background: black;
  color: white;
  padding: 0.5rem;
  border-radius: 4px;
  opacity: 0;
  pointer-events: none;
  transition: opacity 0.3s;
}

.tooltip:hover::after {
  opacity: 1;
}

<div class="tooltip" data-tooltip="Aide ici">Hover me</div>

5️⃣ CHECKBOX CUSTOM:
input[type="checkbox"] {
  appearance: none;
  width: 20px;
  height: 20px;
  border: 2px solid gray;
  position: relative;
}

input[type="checkbox"]:checked::after {
  content: "✓";
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
}

6️⃣ NUMÉROTATION AUTOMATIQUE:
.list {
  counter-reset: item;
}

.list-item::before {
  counter-increment: item;
  content: counter(item) ". ";
  font-weight: bold;
}

7️⃣ PREMIÈRE LETTRE STYLÉE:
p::first-letter {
  font-size: 3rem;
  float: left;
  line-height: 1;
  margin-right: 0.5rem;
}

8️⃣ SÉLECTION CUSTOM:
::selection {
  background: #ffeb3b;
  color: black;
}

═══════════════════════════════════════

COMBINAISONS PUISSANTES:

/* Hover sur parent affecte enfant */
.parent:hover .child {
  color: red;
}

/* Pas le premier */
.item:not(:first-child) {
  margin-top: 1rem;
}

/* Liens externes */
a[href^="http"]:not([href*="monsite.com"])::after {
  content: " ↗";
}

/* Checkbox coché change label */
input:checked + label {
  font-weight: bold;
  color: green;
}

/* Tous sauf le hover */
.item:not(:hover) {
  opacity: 0.5;
}

/* Input invalide et touché */
input:invalid:not(:placeholder-shown) {
  border-color: red;
}

═══════════════════════════════════════

:nth-child() FORMULES:

:nth-child(3)        /* 3ème élément */
:nth-child(odd)      /* 1, 3, 5, 7... */
:nth-child(even)     /* 2, 4, 6, 8... */
:nth-child(3n)       /* 3, 6, 9, 12... */
:nth-child(3n+1)     /* 1, 4, 7, 10... */
:nth-child(3n+2)     /* 2, 5, 8, 11... */
:nth-child(-n+3)     /* 3 premiers */
:nth-child(n+4)      /* À partir du 4ème */

/* Exemple: Zebra striping (lignes alternées) */
tr:nth-child(odd) {
  background: #f9f9f9;
}

tr:nth-child(even) {
  background: white;
}

/* Exemple: 3 premiers éléments */
.item:nth-child(-n+3) {
  font-weight: bold;
}

═══════════════════════════════════════

:nth-child() vs :nth-of-type():

<!-- HTML -->
<div>
  <p>1</p>
  <span>2</span>
  <p>3</p>
  <p>4</p>
</div>

/* :nth-child() compte TOUS les enfants */
p:nth-child(2) { }  /* Ne sélectionne RIEN (2ème enfant est <span>) */

/* :nth-of-type() compte seulement le TYPE */
p:nth-of-type(2) { } /* Sélectionne le 2ème <p> (élément 3) */

RÈGLE:
- :nth-child()    → Position dans TOUS les enfants
- :nth-of-type()  → Position parmi les éléments du MÊME TYPE
```

---

## 9️⃣ VARIABLES CSS (Custom Properties)

### 🎨 **Variables Natives**

```
SYNTAXE:

/* Déclaration */
:root {
  --primary-color: #3490dc;
  --spacing: 1rem;
  --font-size-large: 2rem;
}

/* Utilisation */
.button {
  background: var(--primary-color);
  padding: var(--spacing);
  font-size: var(--font-size-large);
}

/* Avec fallback */
.element {
  color: var(--text-color, black);  /* Si --text-color n'existe pas, utilise black */
}

═══════════════════════════════════════

SCOPE:

/* Global (racine) */
:root {
  --color: blue;
}

/* Scope local */
.card {
  --card-padding: 2rem;
}

.card .content {
  padding: var(--card-padding);  /* Fonctionne */
}

.other {
  padding: var(--card-padding);  /* Ne fonctionne PAS (hors scope) */
}

/* Override */
.card.large {
  --card-padding: 3rem;  /* Override pour ce contexte */
}

═══════════════════════════════════════

PATTERNS PRATIQUES:

1️⃣ THEME SYSTEM:
:root {
  --color-primary: #3490dc;
  --color-secondary: #ffed4e;
  --color-text: #333;
  --color-background: #fff;
  
  --spacing-xs: 0.25rem;
  --spacing-sm: 0.5rem;
  --spacing-md: 1rem;
  --spacing-lg: 2rem;
  --spacing-xl: 4rem;
  
  --font-size-sm: 0.875rem;
  --font-size-base: 1rem;
  --font-size-lg: 1.25rem;
  --font-size-xl: 1.5rem;
  
  --radius: 0.375rem;
  --shadow: 0 1px 3px rgba(0,0,0,0.1);
}

2️⃣ DARK MODE:
:root {
  --bg: white;
  --text: black;
}

[data-theme="dark"] {
  --bg: #1a1a1a;
  --text: white;
}

body {
  background: var(--bg);
  color: var(--text);
}

/* Toggle avec JS */
document.body.dataset.theme = 'dark';

3️⃣ RESPONSIVE SPACING:
:root {
  --spacing: 1rem;
}

@media (min-width: 768px) {
  :root {
    --spacing: 1.5rem;
  }
}

@media (min-width: 1024px) {
  :root {
    --spacing: 2rem;
  }
}

.element {
  padding: var(--spacing);  /* S'adapte automatiquement */
}

4️⃣ CALCULS DYNAMIQUES:
:root {
  --base-size: 1rem;
  --scale: 1.5;
}

h1 {
  font-size: calc(var(--base-size) * var(--scale) * var(--scale) * var(--scale));
  /* = 1rem * 1.5³ = 3.375rem */
}

5️⃣ COMPONENT VARIANTS:
.button {
  --button-bg: var(--color-primary);
  --button-text: white;
  
  background: var(--button-bg);
  color: var(--button-text);
}

.button.secondary {
  --button-bg: var(--color-secondary);
  --button-text: black;
}

6️⃣ STATE MANAGEMENT:
.card {
  --card-scale: 1;
  transform: scale(var(--card-scale));
  transition: transform 0.3s;
}

.card:hover {
  --card-scale: 1.05;
}

═══════════════════════════════════════

MANIPULATION AVEC JAVASCRIPT:

// Lire
const root = document.documentElement;
const primaryColor = getComputedStyle(root)
  .getPropertyValue('--primary-color');

// Écrire
root.style.setProperty('--primary-color', '#ff0000');

// Toggle theme
function toggleTheme() {
  const current = root.style.getPropertyValue('--theme');
  const newTheme = current === 'dark' ? 'light' : 'dark';
  root.style.setProperty('--theme', newTheme);
}

═══════════════════════════════════════

AVANTAGES vs PREPROCESSORS (Sass, Less):

CSS VARIABLES:
✅ Natif (pas de compilation)
✅ Dynamique (changeable au runtime)
✅ Scope flexible
✅ Manipulable avec JS
✅ Héritage en cascade

SASS VARIABLES:
✅ Plus de fonctionnalités (loops, conditions)
✅ Compilation (optimization)
❌ Statique (pas de runtime changes)
❌ Pas de scope CSS

MEILLEURE PRATIQUE:
Utiliser les deux !
- Sass pour génération/logique au build
- CSS variables pour themes/runtime
```

---

## 🔟 PREPROCESSEURS CSS

### 🎨 **Sass/SCSS Concepts**

```
SASS = Syntactically Awesome Style Sheets
Ajoute fonctionnalités à CSS

NESTING:

/* CSS classique */
.nav { }
.nav ul { }
.nav ul li { }
.nav ul li a { }

/* SCSS */
.nav {
  ul {
    li {
      a {
        color: blue;
        
        &:hover {  /* & = parent selector */
          color: darkblue;
        }
      }
    }
  }
}

VARIABLES:

$primary-color: #3490dc;
$spacing: 1rem;

.button {
  background: $primary-color;
  padding: $spacing;
}

MIXINS (Fonctions réutilisables):

@mixin button-style($bg, $color) {
  background: $bg;
  color: $color;
  padding: 0.5rem 1rem;
  border: none;
  border-radius: 4px;
}

.btn-primary {
  @include button-style(#3490dc, white);
}

.btn-danger {
  @include button-style(#e3342f, white);
}

/* Mixin avec contenu */
@mixin respond-to($breakpoint) {
  @if $breakpoint == tablet {
    @media (min-width: 768px) {
      @content;
    }
  }
}

.element {
  font-size: 14px;
  
  @include respond-to(tablet) {
    font-size: 16px;
  }
}

FUNCTIONS:

@function calculate-rem($px) {
  @return $px / 16 * 1rem;
}

.text {
  font-size: calculate-rem(24);  /* 1.5rem */
}

EXTENDS/INHERITANCE:

%button-base {
  padding: 0.5rem 1rem;
  border: none;
  cursor: pointer;
}

.btn-primary {
  @extend %button-base;
  background: blue;
}

.btn-secondary {
  @extend %button-base;
  background: gray;
}

PARTIALS & IMPORTS:

// _variables.scss
$primary: blue;

// _mixins.scss
@mixin center { }

// main.scss
@import 'variables';
@import 'mixins';

LOOPS:

/* Générer classes utilitaires */
@for $i from 1 through 5 {
  .mt-#{$i} {
    margin-top: #{$i * 0.25}rem;
  }
}

/* Génère:
.mt-1 { margin-top: 0.25rem; }
.mt-2 { margin-top: 0.5rem; }
...
*/

/* Loop sur liste */
$colors: (
  'primary': #3490dc,
  'danger': #e3342f,
  'success': #38c172
);

@each $name, $color in $colors {
  .text-#{$name} {
    color: $color;
  }
}

CONDITIONS:

@mixin theme($dark: false) {
  @if $dark {
    background: black;
    color: white;
  } @else {
    background: white;
    color: black;
  }
}

MAPS (Objets):

$theme-colors: (
  'primary': #3490dc,
  'secondary': #6c757d,
  'success': #38c172
);

.btn-primary {
  background: map-get($theme-colors, 'primary');
}

═══════════════════════════════════════

SASS vs SCSS:

SASS (Indented syntax):
.parent
  color: blue
  .child
    color: red

SCSS (CSS-like):
.parent {
  color: blue;
  .child {
    color: red;
  }
}

✅ SCSS est plus populaire (ressemble à CSS)
```

---

## 1️⃣1️⃣ ARCHITECTURE CSS

### 🏗️ **Méthodologies et Organisation**

```
BEM (Block Element Modifier):

STRUCTURE:
.block { }
.block__element { }
.block--modifier { }

EXEMPLE:
<!-- HTML -->
<div class="card card--featured">
  <h2 class="card__title">Title</h2>
  <p class="card__content">Content</p>
  <button class="card__button card__button--primary">
    Click
  </button>
</div>

/* CSS */
.card { }
.card--featured { }
.card__title { }
.card__content { }
.card__button { }
.card__button--primary { }

AVANTAGES:
✅ Évite conflits de noms
✅ Scope clair
✅ Facile à comprendre
❌ Classes verboses

═══════════════════════════════════════

SMACSS (Scalable and Modular Architecture):

5 CATÉGORIES:

1. BASE: Defaults
html, body, h1, a { }

2. LAYOUT: Mise en page principale
.l-header { }
.l-sidebar { }
.l-main { }

3. MODULE: Composants réutilisables
.card { }
.button { }

4. STATE: États
.is-active { }
.is-hidden { }

5. THEME: Thèmes
.theme-dark { }

═══════════════════════════════════════

ITCSS (Inverted Triangle CSS):

ORDRE PAR SPÉCIFICITÉ (croissante):

1. Settings      → Variables
2. Tools         → Mixins, functions
3. Generic       → Reset, normalize
4. Elements      → h1, a, button
5. Objects       → Layout patterns
6. Components    → UI components
7. Utilities     → Helpers, utilities

═══════════════════════════════════════

CUBE CSS (Composition, Utility, Block, Exception):

.card {                    /* Block */
  display: flex;          /* Composition */
}

.card[data-state="error"] { /* Exception */
  border-color: red;
}

.mt-4 {                    /* Utility */
  margin-top: 1rem;
}

═══════════════════════════════════════

STRUCTURE DE DOSSIERS:

styles/
├── base/
│   ├── _reset.scss
│   ├── _typography.scss
│   └── _utilities.scss
├── components/
│   ├── _button.scss
│   ├── _card.scss
│   └── _modal.scss
├── layout/
│   ├── _header.scss
│   ├── _footer.scss
│   └── _grid.scss
├── pages/
│   ├── _home.scss
│   └── _about.scss
├── abstracts/
│   ├── _variables.scss
│   ├── _mixins.scss
│   └── _functions.scss
└── main.scss

═══════════════════════════════════════

NAMING CONVENTIONS:

/* Classes utilitaires */
.u-text-center { }
.u-mt-1 { }

/* Layout */
.l-container { }
.l-grid { }

/* Composants */
.c-button { }
.c-card { }

/* États */
.is-active { }
.is-loading { }
.has-error { }

/* JavaScript hooks (ne pas styler) */
.js-toggle { }
.js-modal-trigger { }
```

---

## 1️⃣2️⃣ PERFORMANCE CSS

### ⚡ **Optimisation et Bonnes Pratiques**

```
SÉLECTEURS PERFORMANTS:

RAPIDE → LENT:
#id              ⚡ Le plus rapide
.class           ⚡ Rapide
element          ⚠️ Moyen
[attribute]      ⚠️ Moyen
*                ❌ Lent
:nth-child()     ❌ Très lent

ÉVITER:
/* ❌ Descendant universel */
* div { }

/* ❌ Trop spécifique */
div.container ul li a.link { }

/* ❌ Attribut regex */
[class^="icon-"] { }

PRÉFÉRER:
/* ✅ Classe directe */
.link { }

/* ✅ Enfant direct */
.nav > .item { }

═══════════════════════════════════════

CRITICAL CSS:

Inline le CSS "above the fold" dans <head>

<head>
  <style>
    /* Critical CSS (visible sans scroll) */
    .header { }
    .hero { }
  </style>
  
  <!-- Non-critical CSS chargé après -->
  <link rel="preload" href="styles.css" as="style" 
        onload="this.rel='stylesheet'">
</head>

═══════════════════════════════════════

RÉDUIRE LA TAILLE:

1️⃣ MINIFICATION:
/* Avant */
.button {
  background: blue;
  padding: 1rem;
}

/* Après */
.button{background:blue;padding:1rem}

2️⃣ PURGE CSS NON-UTILISÉ:
// PurgeCSS, UnCSS
// Retire classes non-utilisées

3️⃣ COMPRESSION:
// Gzip, Brotli
styles.css (100KB) → styles.css.gz (20KB)

4️⃣ CSS-IN-JS (Bundle splitting):
// Charge seulement CSS des composants utilisés

═══════════════════════════════════════

ÉVITER REFLOW/REPAINT:

PROPRIÉTÉS COÛTEUSES (déclenchent reflow):
❌ width, height
❌ margin, padding
❌ border
❌ top, left, right, bottom
❌ font-size
❌ display

PROPRIÉTÉS PERFORMANTES:
✅ transform
✅ opacity

EXEMPLE:
/* ❌ Lent (reflow) */
.element:hover {
  width: 200px;
}

/* ✅ Rapide (composite) */
.element {
  will-change: transform;
}
.element:hover {
  transform: scale(1.2);
}

═══════════════════════════════════════

CONTAINMENT:

/* Indique que l'élément est isolé */
.card {
  contain: layout style paint;
}

/* Ou */
.card {
  content-visibility: auto;
}

Navigateur peut optimiser le rendu

═══════════════════════════════════════

FONT LOADING:

/* ❌ FOIT (Flash of Invisible Text) */
@font-face {
  font-family: 'MyFont';
  src: url('font.woff2');
}

/* ✅ FOUT (Flash of Unstyled Text) */
@font-face {
  font-family: 'MyFont';
  src: url('font.woff2');
  font-display: swap;  /* Affiche fallback pendant chargement */
}

/* Options font-display */
auto      : Navigateur décide
block     : Cache texte 3s (FOIT)
swap      : Affiche fallback immédiatement (FOUT)
fallback  : Court block puis swap
optional  : Utilise cache ou rien

═══════════════════════════════════════

LAZY LOADING CSS:

<!-- CSS non-critique chargé après -->
<link rel="preload" href="styles.css" as="style" 
      onload="this.onload=null;this.rel='stylesheet'">
<noscript>
  <link rel="stylesheet" href="styles.css">
</noscript>

═══════════════════════════════════════

METRIQUES À SURVEILLER:

FCP (First Contentful Paint):
- Temps pour 1er contenu
- Impacté par CSS bloquant

LCP (Largest Contentful Paint):
- Temps pour contenu principal
- Impacté par images, fonts

CLS (Cumulative Layout Shift):
- Stabilité visuelle
- Impacté par dimensions non-définies

OUTILS:
- Lighthouse (Chrome DevTools)
- WebPageTest
- CrUX (Chrome User Experience Report)
```

---

## 1️⃣3️⃣ QUESTIONS D'ENTRETIEN FRÉQUENTES

### Q1: **Expliquez le Box Model en détail**

```
RÉPONSE:

"Le Box Model définit comment les dimensions d'un élément
sont calculées.

Chaque élément est composé de 4 zones:
1. Content: Le contenu (text, images)
2. Padding: Espace interne (transparent)
3. Border: Bordure
4. Margin: Espace externe (transparent)

Par défaut (content-box), width/height s'appliquent 
SEULEMENT au content. Pour calculer la largeur totale:

width + padding-left + padding-right + border-left + border-right

Problème: Difficile de dimensionner précisément.

Solution moderne: box-sizing: border-box
Avec border-box, width inclut content + padding + border.

Best practice:
*,  *::before, *::after {
  box-sizing: border-box;
}

Dans mes projets, j'utilise toujours border-box pour
des calculs plus prévisibles."
```

---

### Q2: **Différence entre position: relative, absolute, et fixed**

```
RÉPONSE STRUCTURÉE:

RELATIVE:
- Position relative à son emplacement normal
- Reste dans le flux (occupe son espace)
- top/left déplace visuellement mais l'espace reste réservé
- Use case: Base pour enfants absolute

ABSOLUTE:
- Position relative au parent positionné le plus proche
- Sorti du flux (ne prend plus d'espace)
- Si pas de parent positionné, relatif à <html>
- Use case: Overlays, tooltips, badges

FIXED:
- Position relative au viewport
- Sorti du flux
- Reste fixe au scroll
- Use case: Navbar fixe, boutons flottants

STICKY (bonus):
- Hybride relative/fixed
- Relative jusqu'à un seuil, puis fixe
- Reste dans le flux
- Use case: Headers de sections

Exemple concret: Dans mon projet [nom], j'ai utilisé
position: sticky pour les headers de tableau qui restent
visibles au scroll."
```

---

### Q3: **Flexbox vs Grid : Quand utiliser quoi ?**

```
RÉPONSE COMPARATIVE:

FLEXBOX (1D):
✅ Une dimension: rangée OU colonne
✅ Content-first (taille basée sur contenu)
✅ Composants: navbar, cards, buttons
✅ Alignment facile

GRID (2D):
✅ Deux dimensions: rangées ET colonnes
✅ Layout-first (structure définie à l'avance)
✅ Page layouts complexes
✅ Overlapping possible

QUAND UTILISER:

Flexbox:
- Navbar avec logo et menu
- Liste de cartes qui wrap
- Centrage simple
- Composants UI

Grid:
- Layout de page (header, sidebar, main, footer)
- Galleries d'images
- Dashboard avec widgets
- Grilles de produits

COMBINAISON:
On peut les combiner ! Par exemple:

.page-layout {
  display: grid;  /* Layout général */
  grid-template-areas: "header" "main" "footer";
}

.navbar {
  display: flex;  /* Composant navbar */
  justify-content: space-between;
}

Dans mes projets, j'utilise Grid pour la structure
globale et Flexbox pour les composants individuels."
```

---

### Q4: **Comment gérez-vous la spécificité CSS ?**

```
RÉPONSE:

"La spécificité détermine quelle règle CSS s'applique
en cas de conflit.

CALCUL: (inline, IDs, classes, elements)

Exemples:
div               → (0,0,0,1) = 1
.button           → (0,0,1,0) = 10
#nav              → (0,1,0,0) = 100
div.button        → (0,0,1,1) = 11
#nav .button      → (0,1,1,0) = 110

PROBLÈMES:
- !important casse la cascade
- IDs trop spécifiques
- Surspécification (.header .nav ul li a)

MES BONNES PRATIQUES:

1. Éviter !important
2. Éviter IDs pour styling (seulement JavaScript hooks)
3. Classes BEM pour scope clair
4. Garde spécificité basse et constante
5. Utilise classes utilitaires quand approprié

Exemple:
❌ div#header .navigation ul li a.link { }
✅ .nav__link { }

Dans mes projets, j'utilise BEM pour éviter
les conflits de spécificité."
```

---

### Q5: **Expliquez la cascade CSS**

```
RÉPONSE COMPLÈTE:

"La cascade détermine quelles règles s'appliquent
quand plusieurs règles ciblent le même élément.

FACTEURS DE PRIORITÉ (ordre):

1️⃣ ORIGIN & IMPORTANCE:
   !important user agent > !important user > !important author
   > author > user > user agent

2️⃣ INLINE STYLES:
   <div style="color: red"> a priorité haute

3️⃣ SPÉCIFICITÉ:
   #id (100) > .class (10) > element (1)

4️⃣ SOURCE ORDER:
   Si même spécificité, le dernier gagne

HÉRITAGE:
Certaines propriétés héritent du parent:
- color, font-family, line-height
Autres non:
- margin, padding, border

EXEMPLE:
.text { color: blue; }           /* (0,0,1,0) */
div.text { color: red; }         /* (0,0,1,1) - GAGNE */
.container .text { color: green; } /* (0,0,2,0) - GAGNE */

BEST PRACTICES:
- Comprendre la cascade plutôt que la combattre
- Éviter !important
- Garde spécificité uniforme
- Utilise classes plutôt que IDs"
```

---

### Q6: **Comment optimisez-vous les performances CSS ?**

```
RÉPONSE STRUCTURÉE:

MES STRATÉGIES D'OPTIMISATION:

1️⃣ SÉLECTEURS PERFORMANTS:
✅ Classes simples: .button
❌ Sélecteurs complexes: div > ul li:nth-child(2n)

2️⃣ CRITICAL CSS:
- Inline CSS above-the-fold dans <head>
/*/*/*
```
- Charge reste en async

3️⃣ PURGE CSS NON-UTILISÉ:
- PurgeCSS en production
- Tailwind fait ça automatiquement

4️⃣ MINIFICATION & COMPRESSION:
- Minify CSS (remove whitespace, comments)
- Gzip/Brotli compression
- 100KB → 20KB gzipped

5️⃣ PROPRIÉTÉS PERFORMANTES:
✅ transform, opacity (GPU-accelerated)
❌ width, height, top, left (reflow)

Exemple:
/* ❌ Lent */
.element:hover { width: 200px; }

/* ✅ Rapide */
.element:hover { transform: scale(1.2); }

6️⃣ FONT LOADING:
- font-display: swap (évite FOIT)
- Preload fonts critiques

7️⃣ LAZY LOADING:
- Charge CSS non-critique après

8️⃣ CONTAINMENT:
- contain: layout style paint;
- Isole rendering

MÉTRIQUES:
- Surveille Core Web Vitals (LCP, FCP, CLS)
- Lighthouse audits réguliers

Dans mon projet [nom], j'ai réduit CSS de 150KB à 15KB
avec PurgeCSS et critical CSS inline."
```

---

### Q7: **Qu'est-ce qu'un CSS-in-JS et quels sont les avantages/inconvénients ?**

```
RÉPONSE COMPARATIVE:

CSS-IN-JS = Écrire CSS dans JavaScript

EXEMPLES:
- Styled Components
- Emotion
- CSS Modules
- Inline styles

AVANTAGES:
✅ Scope automatique (pas de conflits)
✅ Dynamic styling basé sur props
✅ Dead code elimination (tree-shaking)
✅ Colocation (CSS près du composant)
✅ Type-safe avec TypeScript

const Button = styled.button`
  background: ${props => props.primary ? 'blue' : 'gray'};
  padding: 1rem;
`;

INCONVÉNIENTS:
❌ Runtime overhead (CSS généré à l'exécution)
❌ Bundle JS plus gros
❌ Pas de cache CSS
❌ Courbe d'apprentissage
❌ SSR complexe
❌ Pas de design tokens natifs

ALTERNATIVES:

CSS Modules:
✅ Scope local
✅ Fichiers CSS séparés
✅ Pas de runtime
❌ Moins de dynamisme

Tailwind:
✅ Utility-first
✅ Tree-shaking excellent
✅ Design system intégré
❌ Classes verbales

MON AVIS:
J'utilise CSS-in-JS (Styled Components) quand:
- Beaucoup de styles dynamiques
- Application très component-based

Je préfère Tailwind/CSS Modules pour:
- Performance critique
- Sites statiques
- Équipes qui préfèrent CSS séparé

Dans mon projet wild-oasis, j'ai utilisé Styled Components
pour la customisation facile des composants."
```

---

### Q8: **Expliquez le concept de Responsive Design**

```
RÉPONSE COMPLÈTE:

RESPONSIVE DESIGN = Une seule codebase qui s'adapte
à tous les écrans

3 PILIERS:

1️⃣ FLUID GRIDS:
   Layouts flexibles (%, fr, flexbox, grid)
   Pas de largeurs fixes en px

2️⃣ FLEXIBLE IMAGES:
   Images s'adaptent au container
   max-width: 100%; height: auto;

3️⃣ MEDIA QUERIES:
   Styles conditionnels par breakpoint

APPROCHES:

MOBILE-FIRST (✅ Recommandé):
/* Base = Mobile */
.element { font-size: 14px; }

@media (min-width: 768px) {
  .element { font-size: 16px; }
}

Avantages:
- Performance mobile (charge moins)
- Progressive enhancement
- Priorité au mobile (80% trafic)

DESKTOP-FIRST (ancien):
/* Base = Desktop */
.element { font-size: 16px; }

@media (max-width: 767px) {
  .element { font-size: 14px; }
}

BREAKPOINTS STANDARDS:
- 480px: Mobile landscape
- 768px: Tablet
- 1024px: Desktop
- 1280px: Large desktop

TECHNIQUES MODERNES:

1. Container Queries:
@container (min-width: 400px) {
  .card { display: flex; }
}

2. Clamp (fluid typography):
font-size: clamp(1rem, 2vw + 0.5rem, 2rem);

3. Aspect Ratio:
aspect-ratio: 16/9;

4. Flexbox/Grid:
grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));

BEST PRACTICES:
✅ Mobile-first approach
✅ Test sur vrais devices
✅ Touch-friendly (44x44px min)
✅ Performance mobile prioritaire
✅ Breakpoints basés sur contenu, pas devices

Dans tous mes projets, j'utilise mobile-first et teste
sur iPhone, iPad, et desktop."
```

---

### Q9: **Qu'est-ce que le Specificity War et comment l'éviter ?**

```
RÉPONSE:

SPECIFICITY WAR = Escalade de spécificité pour
override des styles

EXEMPLE DU PROBLÈME:

/* Dev 1 */
.button { background: blue; }

/* Dev 2 veut override */
.header .button { background: red; }

/* Dev 1 veut re-override */
.header .nav .button { background: blue; }

/* Dev 2 abandonne */
.button { background: red !important; }

RÉSULTAT:
❌ Code non-maintenable
❌ !important partout
❌ Spécificité explosive

SOLUTIONS:

1️⃣ MÉTHODOLOGIE BEM:
.button { }
.button--primary { }
.button--large { }

Spécificité uniforme (0,0,1,0)

2️⃣ UTILITY-FIRST (Tailwind):
Classes atomiques, pas de conflits

3️⃣ CSS-IN-JS:
Scope automatique

4️⃣ CSS MODULES:
.button { } → .Button_button__abc123

5️⃣ RÈGLES D'ÉQUIPE:
- Pas d'IDs pour styling
- Max 2-3 classes de profondeur
- !important seulement pour utilities

EXEMPLE BON:

/* Composant */
.card { }
.card__title { }
.card__body { }

/* Variants */
.card--featured { }

/* States */
.card.is-active { }

/* Utilities (seul endroit pour !important) */
.u-hidden { display: none !important; }

MON EXPÉRIENCE:
Dans mon équipe, on utilise BEM + utilities.
Spécificité reste basse et prévisible."
```

---

### Q10: **Comment gérez-vous le Dark Mode en CSS ?**

```
RÉPONSE PRATIQUE:

APPROCHES:

1️⃣ MEDIA QUERY (Système OS):

:root {
  --bg: white;
  --text: black;
}

@media (prefers-color-scheme: dark) {
  :root {
    --bg: #1a1a1a;
    --text: white;
  }
}

body {
  background: var(--bg);
  color: var(--text);
}

Avantages:
✅ Automatique selon système
✅ Pas de JavaScript
❌ Utilisateur ne peut pas changer manuellement

2️⃣ CLASS STRATEGY (Manuel):

:root {
  --bg: white;
  --text: black;
}

.dark {
  --bg: #1a1a1a;
  --text: white;
}

body {
  background: var(--bg);
  color: var(--text);
}

// JavaScript
document.documentElement.classList.toggle('dark');

Avantages:
✅ Contrôle utilisateur
✅ Peut override système
❌ Besoin de JavaScript

3️⃣ DATA ATTRIBUTE:

:root {
  --bg: white;
  --text: black;
}

[data-theme="dark"] {
  --bg: #1a1a1a;
  --text: white;
}

// JavaScript
document.documentElement.dataset.theme = 'dark';

4️⃣ COMBINAISON (Best Practice):

:root {
  --bg: white;
  --text: black;
}

@media (prefers-color-scheme: dark) {
  :root:not([data-theme="light"]) {
    --bg: #1a1a1a;
    --text: white;
  }
}

[data-theme="dark"] {
  --bg: #1a1a1a;
  --text: white;
}

[data-theme="light"] {
  --bg: white;
  --text: black;
}

Logique:
- Suit système par défaut
- Utilisateur peut override

IMPLÉMENTATION COMPLÈTE:

// CSS
:root {
  /* Light theme (default) */
  --color-bg: #ffffff;
  --color-text: #1a1a1a;
  --color-primary: #3490dc;
  --color-border: #e5e5e5;
}

[data-theme="dark"] {
  /* Dark theme */
  --color-bg: #1a1a1a;
  --color-text: #ffffff;
  --color-primary: #60a5fa;
  --color-border: #333333;
}

body {
  background: var(--color-bg);
  color: var(--color-text);
  transition: background 0.3s, color 0.3s;
}

// React Component
function ThemeToggle() {
  const [theme, setTheme] = useState(() => {
    const saved = localStorage.getItem('theme');
    return saved || 'light';
  });
  
  useEffect(() => {
    document.documentElement.dataset.theme = theme;
    localStorage.setItem('theme', theme);
  }, [theme]);
  
  const toggleTheme = () => {
    setTheme(theme === 'light' ? 'dark' : 'light');
  };
  
  return (
    <button onClick={toggleTheme}>
      {theme === 'dark' ? '🌞' : '🌙'}
    </button>
  );
}

BEST PRACTICES:
✅ Utiliser CSS variables
✅ Transition douce
✅ Persister choix (localStorage)
✅ Respecter prefers-color-scheme
✅ Test sur les deux modes

Dans mon projet wild-oasis, j'ai implémenté dark mode
avec Context API + CSS variables."
```

---

## 📝 RÉSUMÉ FINAL POUR L'ENTRETIEN

### ✅ **Points Essentiels**

```
1️⃣ BOX MODEL:
   Content, Padding, Border, Margin
   box-sizing: border-box (toujours)

2️⃣ POSITIONING:
   Static, Relative, Absolute, Fixed, Sticky
   z-index et stacking context

3️⃣ FLEXBOX (1D):
   justify-content, align-items, flex, gap
   Pour composants et layouts simples

4️⃣ GRID (2D):
   grid-template-columns/rows, gap, areas
   Pour layouts complexes

5️⃣ RESPONSIVE:
   Mobile-first, media queries, fluid units
   Breakpoints: 768px, 1024px, 1280px

6️⃣ CASCADE & SPÉCIFICITÉ:
   Origin, Inline, Specificity, Order
   Éviter !important et IDs

7️⃣ PERFORMANCE:
   Critical CSS, minification, performant selectors
   transform/opacity > width/height

8️⃣ VARIABLES CSS:
   --custom-property, var(), theming
   Dynamic avec JavaScript

9️⃣ ANIMATIONS:
   Transitions (états), Animations (keyframes)
   transform performant

🔟 MÉTHODOLOGIES:
   BEM, SMACSS, ITCSS
   Organisation et maintenabilité
```

---

### 🎯 **Structure de Réponse Idéale**

```
POUR CHAQUE QUESTION:

1. DÉFINITION COURTE
   "Flexbox est un système de layout unidimensionnel..."

2. CONCEPTS CLÉS
   "Main axis, cross axis, flex container, flex items"

3. EXEMPLE CONCRET
   .container { display: flex; justify-content: center; }

4. USE CASES
   "J'utilise Flexbox pour navbars, cards, centering"

5. ALTERNATIVES/COMPARAISON
   "Vs Grid qui est bidimensionnel..."

6. EXPÉRIENCE PERSONNELLE
   "Dans mon projet [nom], j'ai utilisé Flexbox pour..."

7. BEST PRACTICES
   "Toujours définir flex-wrap pour responsive..."
```

---

### 💡 **Questions à Poser au Recruteur**

```
BONNES QUESTIONS (montrent expertise):

1. "Utilisez-vous un preprocesseur CSS (Sass, Less) ou
   du CSS vanilla?"
   → Comprendre la stack

2. "Avez-vous une méthodologie CSS (BEM, SMACSS) ?"
   → Comprendre l'organisation

3. "Comment gérez-vous le responsive design ?
   Mobile-first ou desktop-first ?"
   → Comprendre l'approche

4. "Utilisez-vous Tailwind ou une autre solution utility-first ?"
   → Comprendre les choix techniques

5. "Comment testez-vous la compatibilité navigateurs ?"
   → Comprendre le process QA

6. "Avez-vous des guidelines de performance CSS ?"
   → Comprendre les standards qualité

7. "Utilisez-vous des design tokens ou un design system ?"
   → Comprendre la maturité du design

8. "Comment gérez-vous le dark mode ?"
   → Comprendre les features modernes
```

---

### 🎓 **Derniers Conseils**

```
AVANT L'ENTRETIEN:
✅ Relire CSS de vos projets
✅ Tester sur CodePen/JSFiddle
✅ Revoir Flexbox/Grid (Flexbox Froggy, Grid Garden)
✅ Lire documentation MDN

PENDANT L'ENTRETIEN:
✅ Dessiner si nécessaire (Box Model, Grid)
✅ Citer vos projets concrets
✅ Mentionner performance
✅ Parler responsive/accessibilité

PIÈGES À ÉVITER:
❌ "CSS c'est facile"
❌ Ignorer spécificité
❌ Confondre Flexbox et Grid
❌ Ne pas connaître CSS modernes (variables, Grid)

POINTS BONUS:
✅ Connaître CSS modernes (Container Queries, :has())
✅ Parler performance (Core Web Vitals)
✅ Mentionner accessibilité (prefers-reduced-motion)
✅ Connaître méthodologies (BEM)

PHRASE CLÉ:
"CSS n'est pas juste du styling, c'est la fondation
 de la performance, de l'accessibilité, et de
 l'expérience utilisateur."
```

---

### 🎯 **Phrase d'Accroche pour l'Entretien**

```
"J'ai une solide maîtrise de CSS, du Box Model aux
layouts modernes avec Flexbox et Grid. Dans mes projets,
j'utilise une approche mobile-first avec des méthodes
comme BEM pour la maintenabilité.

J'accorde une attention particulière à la performance
(Critical CSS, propriétés GPU-accelerated) et à
l'accessibilité (prefers-reduced-motion, contraste).

Par exemple, dans mon projet [nom], j'ai implémenté
un système de dark mode avec CSS variables, réduit le
CSS de 150KB à 15KB avec PurgeCSS, et assuré une
compatibilité parfaite mobile-desktop avec Grid et
media queries."
```

---

### 📚 **Ressources Supplémentaires**

```
DOCUMENTATION:
- MDN Web Docs (référence complète)
- CSS-Tricks (articles, guides)
- Can I Use (support navigateurs)

PRATIQUE INTERACTIVE:
- Flexbox Froggy (apprendre Flexbox)
- Grid Garden (apprendre Grid)
- CSS Diner (sélecteurs CSS)

OUTILS:
- Lighthouse (performance)
- CSS Stats (analyse CSS)
- Specificity Calculator

VEILLE:
- CSS Weekly (newsletter)
- Smashing Magazine
- A List Apart
```

---