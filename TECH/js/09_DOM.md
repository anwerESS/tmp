## [**..**](./00_index.md)

# Chapitre 9 : DOM (Document Object Model) - Résumé Détaillé

## 9.1 Introduction au DOM

### Qu'est-ce que le DOM ?

Le **DOM (Document Object Model)** est une représentation en **arborescence** (tree structure) d'un document HTML/XML. Il permet à JavaScript d'accéder et de manipuler le contenu, la structure et le style d'une page web.

```javascript
// Le DOM transforme ce HTML :
/*
<!DOCTYPE html>
<html>
  <head>
    <title>My Page</title>
  </head>
  <body>
    <h1 id="title">Hello World</h1>
    <p class="text">This is a paragraph.</p>
  </body>
</html>
*/

// En une structure arborescente :
/*
Document
  └─ html
      ├─ head
      │   └─ title
      │       └─ "My Page"
      └─ body
          ├─ h1 (id="title")
          │   └─ "Hello World"
          └─ p (class="text")
              └─ "This is a paragraph."
*/

// Accéder au document
console.log(document);  // Document entier
console.log(document.documentElement);  // <html>
console.log(document.head);  // <head>
console.log(document.body);  // <body>

// Propriétés du document
console.log(document.title);  // "My Page"
console.log(document.URL);    // URL complète
console.log(document.domain); // Domaine
```

### Structure arborescente

```javascript
// Chaque élément HTML est un NODE (nœud)

// Types de nodes
console.log(Node.ELEMENT_NODE);         // 1 - Élément HTML
console.log(Node.TEXT_NODE);            // 3 - Texte
console.log(Node.COMMENT_NODE);         // 8 - Commentaire
console.log(Node.DOCUMENT_NODE);        // 9 - Document
console.log(Node.DOCUMENT_TYPE_NODE);   // 10 - DOCTYPE

// Vérifier le type d'un node
const h1 = document.querySelector('h1');
console.log(h1.nodeType);      // 1 (ELEMENT_NODE)
console.log(h1.nodeName);      // "H1"
console.log(h1.nodeValue);     // null (pour les éléments)

const textNode = h1.firstChild;
console.log(textNode.nodeType);   // 3 (TEXT_NODE)
console.log(textNode.nodeName);   // "#text"
console.log(textNode.nodeValue);  // "Hello World"

// HTML :
/*
<div id="container">
  <h1>Title</h1>
  <!-- This is a comment -->
  <p>Paragraph</p>
</div>
*/

const container = document.getElementById('container');

// childNodes inclut TOUS les types de nodes (éléments, texte, commentaires)
console.log(container.childNodes);
// NodeList [text, h1, text, comment, text, p, text]
// Les "text" sont les espaces/retours à la ligne

// children n'inclut que les ÉLÉMENTS
console.log(container.children);
// HTMLCollection [h1, p]
```

### Nodes, Elements, Attributes

```javascript
// NODE : classe de base (tous les types)
// ELEMENT : sous-classe de Node (éléments HTML)
// ATTRIBUTE : propriétés des éléments

const element = document.querySelector('h1');

// Element properties
console.log(element instanceof Node);      // true
console.log(element instanceof Element);   // true
console.log(element instanceof HTMLElement);  // true

// Attributes (attributs HTML)
/*
<h1 id="title" class="heading" data-value="123">Hello</h1>
*/

const h1 = document.querySelector('h1');

// getAttribute() - lire un attribut
console.log(h1.getAttribute('id'));     // "title"
console.log(h1.getAttribute('class'));  // "heading"
console.log(h1.getAttribute('data-value'));  // "123"

// setAttribute() - définir un attribut
h1.setAttribute('data-value', '456');
h1.setAttribute('title', 'Hover text');

// Propriétés directes (pour attributs standard)
console.log(h1.id);        // "title"
console.log(h1.className); // "heading"
// console.log(h1.class);  // ❌ undefined (utiliser className)

// dataset pour data attributes
console.log(h1.dataset.value);  // "456"
h1.dataset.newValue = '789';
console.log(h1.getAttribute('data-new-value'));  // "789"
```

---

## 9.2 Sélection d'Éléments (DOM Selectors)

### `getElementById()` - Sélectionner par ID

```javascript
// HTML :
/*
<div id="container">
  <h1 id="title">Hello World</h1>
  <p id="description">This is a description.</p>
</div>
*/

// Sélectionner un élément par son ID
const title = document.getElementById('title');
console.log(title);  // <h1 id="title">Hello World</h1>

// Retourne null si l'élément n'existe pas
const notFound = document.getElementById('does-not-exist');
console.log(notFound);  // null

// ⚠️ getElementById() n'existe que sur document (pas sur les éléments)
const container = document.getElementById('container');
// container.getElementById('title');  // ❌ Error

// ID doit être unique dans le document
// Si plusieurs éléments ont le même ID, retourne le premier

// Accès rapide : O(1) - très performant
console.time('getElementById');
for (let i = 0; i < 10000; i++) {
  document.getElementById('title');
}
console.timeEnd('getElementById');  // ~1ms
```

### `getElementsByClassName()` - Sélectionner par classe

```javascript
// HTML :
/*
<div class="box">Box 1</div>
<div class="box highlight">Box 2</div>
<div class="box">Box 3</div>
<p class="box">Paragraph</p>
*/

// Sélectionner tous les éléments avec une classe
const boxes = document.getElementsByClassName('box');
console.log(boxes);  // HTMLCollection(4) [div.box, div.box.highlight, div.box, p.box]

// HTMLCollection est "live" - se met à jour automatiquement
console.log(boxes.length);  // 4

const newBox = document.createElement('div');
newBox.className = 'box';
document.body.appendChild(newBox);

console.log(boxes.length);  // 5 (mis à jour automatiquement)

// Itérer sur HTMLCollection
// ⚠️ Pas directement itérable avec for...of (mais fonctionne dans les navigateurs modernes)
for (let i = 0; i < boxes.length; i++) {
  console.log(boxes[i]);
}

// Convertir en array pour utiliser les méthodes d'array
const boxesArray = Array.from(boxes);
boxesArray.forEach(box => {
  console.log(box.textContent);
});

// Avec spread operator
[...boxes].forEach(box => {
  box.style.border = '1px solid red';
});

// Sélectionner avec plusieurs classes (élément doit avoir toutes les classes)
const highlighted = document.getElementsByClassName('box highlight');
console.log(highlighted);  // HTMLCollection(1) [div.box.highlight]

// Sur un élément spécifique (pas seulement document)
const container = document.getElementById('container');
const boxesInContainer = container.getElementsByClassName('box');
```

### `getElementsByTagName()` - Sélectionner par balise

```javascript
// HTML :
/*
<div>
  <p>Paragraph 1</p>
  <p>Paragraph 2</p>
  <span>Span</span>
  <p>Paragraph 3</p>
</div>
*/

// Sélectionner tous les éléments d'une balise
const paragraphs = document.getElementsByTagName('p');
console.log(paragraphs);  // HTMLCollection(3) [p, p, p]

// Tous les éléments
const allElements = document.getElementsByTagName('*');
console.log(allElements.length);  // Tous les éléments du document

// HTMLCollection est "live"
console.log(paragraphs.length);  // 3

const newP = document.createElement('p');
document.body.appendChild(newP);

console.log(paragraphs.length);  // 4 (mis à jour)

// Sur un élément spécifique
const div = document.querySelector('div');
const pInDiv = div.getElementsByTagName('p');
console.log(pInDiv.length);  // Seulement les p dans div

// Case insensitive
const PARAGRAPHS = document.getElementsByTagName('P');
console.log(PARAGRAPHS);  // Même résultat

// Itérer
Array.from(paragraphs).forEach((p, index) => {
  p.textContent = `Paragraph ${index + 1} updated`;
});
```

### `querySelector()` - Sélectionner avec sélecteur CSS (premier)

```javascript
// HTML :
/*
<div id="container">
  <h1 class="title">Main Title</h1>
  <p class="text">Paragraph 1</p>
  <p class="text highlight">Paragraph 2</p>
  <div class="box">
    <span>Nested span</span>
  </div>
</div>
*/

// Sélectionner le PREMIER élément correspondant au sélecteur CSS
const title = document.querySelector('.title');
console.log(title);  // <h1 class="title">Main Title</h1>

// Par ID
const container = document.querySelector('#container');

// Par balise
const firstP = document.querySelector('p');

// Sélecteurs complexes
const highlightedText = document.querySelector('.text.highlight');
console.log(highlightedText);  // <p class="text highlight">Paragraph 2</p>

// Descendants
const nestedSpan = document.querySelector('#container .box span');
console.log(nestedSpan);  // <span>Nested span</span>

// Enfant direct
const directChild = document.querySelector('#container > .box');

// Attributs
/*
<input type="text" name="username">
<input type="password" name="password">
*/

const textInput = document.querySelector('input[type="text"]');
const usernameInput = document.querySelector('input[name="username"]');

// Pseudo-classes
const firstChild = document.querySelector('p:first-child');
const lastChild = document.querySelector('p:last-child');
const nthChild = document.querySelector('p:nth-child(2)');

// Combinateurs
const nextSibling = document.querySelector('h1 + p');  // p après h1
const generalSibling = document.querySelector('h1 ~ p');  // tous les p après h1

// Retourne null si non trouvé
const notFound = document.querySelector('.does-not-exist');
console.log(notFound);  // null

// Sur un élément spécifique
const box = document.querySelector('.box');
const spanInBox = box.querySelector('span');

// ⚠️ querySelector retourne seulement le PREMIER élément
const firstText = document.querySelector('.text');
console.log(firstText);  // Premier p avec classe "text"
```

### `querySelectorAll()` - Sélectionner tous avec sélecteur CSS

```javascript
// HTML :
/*
<ul>
  <li class="item">Item 1</li>
  <li class="item active">Item 2</li>
  <li class="item">Item 3</li>
  <li class="item">Item 4</li>
</ul>
*/

// Sélectionner TOUS les éléments correspondants
const items = document.querySelectorAll('.item');
console.log(items);  // NodeList(4) [li.item, li.item.active, li.item, li.item]

// ⚠️ NodeList est "static" (pas live comme HTMLCollection)
console.log(items.length);  // 4

const newItem = document.createElement('li');
newItem.className = 'item';
document.querySelector('ul').appendChild(newItem);

console.log(items.length);  // 4 (toujours 4, pas mis à jour)

// Pour avoir la liste à jour, refaire querySelectorAll()
const updatedItems = document.querySelectorAll('.item');
console.log(updatedItems.length);  // 5

// NodeList est itérable avec for...of
for (const item of items) {
  console.log(item.textContent);
}

// forEach natif sur NodeList (pas besoin de convertir)
items.forEach((item, index) => {
  item.textContent = `Item ${index + 1}`;
});

// Convertir en array pour autres méthodes
const itemsArray = [...items];
const activeItems = itemsArray.filter(item => 
  item.classList.contains('active')
);

// Sélecteurs complexes
const allParagraphs = document.querySelectorAll('p');
const highlightedElements = document.querySelectorAll('.highlight');
const inputsInForm = document.querySelectorAll('form input[type="text"]');

// Sélecteurs multiples (virgule = OU)
const headingsAndParagraphs = document.querySelectorAll('h1, h2, h3, p');

// Sur un élément spécifique
const container = document.querySelector('#container');
const itemsInContainer = container.querySelectorAll('.item');

// Si aucun élément trouvé, retourne NodeList vide
const empty = document.querySelectorAll('.does-not-exist');
console.log(empty);  // NodeList(0) []
console.log(empty.length);  // 0
```

### Différences et performances

```javascript
// HTMLCOLLECTION vs NODELIST

// HTMLCollection (getElementById, getElementsByClassName, getElementsByTagName)
const divsByClass = document.getElementsByClassName('box');
console.log(divsByClass);  // HTMLCollection (live)

// NodeList (querySelectorAll)
const divsByQuery = document.querySelectorAll('.box');
console.log(divsByQuery);  // NodeList (static)

// Live vs Static
const container = document.querySelector('#container');

// HTMLCollection - live
const liveCollection = container.getElementsByClassName('item');
console.log(liveCollection.length);  // 4

container.innerHTML += '<div class="item">New</div>';
console.log(liveCollection.length);  // 5 (mis à jour)

// NodeList - static
const staticList = container.querySelectorAll('.item');
console.log(staticList.length);  // 5

container.innerHTML += '<div class="item">Another</div>';
console.log(staticList.length);  // 5 (pas mis à jour)

// PERFORMANCES

// getElementById - le plus rapide (O(1))
console.time('getElementById');
for (let i = 0; i < 10000; i++) {
  document.getElementById('container');
}
console.timeEnd('getElementById');  // ~1ms

// getElementsByClassName - rapide (O(n))
console.time('getElementsByClassName');
for (let i = 0; i < 10000; i++) {
  document.getElementsByClassName('box');
}
console.timeEnd('getElementsByClassName');  // ~5ms

// querySelector - plus lent (parse CSS)
console.time('querySelector');
for (let i = 0; i < 10000; i++) {
  document.querySelector('#container');
}
console.timeEnd('querySelector');  // ~10ms

// querySelectorAll - le plus lent (parse CSS + itère)
console.time('querySelectorAll');
for (let i = 0; i < 10000; i++) {
  document.querySelectorAll('.box');
}
console.timeEnd('querySelectorAll');  // ~50ms

// RECOMMANDATIONS
// ✅ getElementById pour IDs (le plus rapide)
// ✅ querySelector/querySelectorAll pour sélecteurs complexes (flexibilité)
// ✅ getElementsByClassName/TagName si vous avez besoin d'une collection live
```

**Tableau comparatif :**

| Méthode | Retour | Live/Static | Performance | Flexibilité |
|---------|--------|-------------|-------------|-------------|
| `getElementById` | Element | - | ⚡⚡⚡ Très rapide | ❌ ID seulement |
| `getElementsByClassName` | HTMLCollection | ✅ Live | ⚡⚡ Rapide | ❌ Classe seulement |
| `getElementsByTagName` | HTMLCollection | ✅ Live | ⚡⚡ Rapide | ❌ Balise seulement |
| `querySelector` | Element | - | ⚡ Moyen | ✅✅✅ Très flexible |
| `querySelectorAll` | NodeList | ❌ Static | ⚡ Lent | ✅✅✅ Très flexible |

---

## 9.3 Manipulation de Contenu et Attributs

### Lire/Modifier le contenu : `innerHTML`, `textContent`, `innerText`

```javascript
// HTML :
/*
<div id="content">
  <h1>Title</h1>
  <p>This is a <strong>paragraph</strong>.</p>
</div>
*/

const content = document.getElementById('content');

// innerHTML - obtient/définit le HTML complet
console.log(content.innerHTML);
// "<h1>Title</h1>\n  <p>This is a <strong>paragraph</strong>.</p>"

// Modifier avec innerHTML (remplace tout)
content.innerHTML = '<h2>New Title</h2><p>New paragraph</p>';

// Ajouter du contenu
content.innerHTML += '<p>Additional paragraph</p>';

// ⚠️ innerHTML réinterprète tout le HTML (peut être lent)
// ⚠️ Risque XSS si on insère du contenu utilisateur non échappé

// textContent - obtient/définit le texte (sans HTML)
console.log(content.textContent);
// "New TitleNew paragraphAdditional paragraph"
// Tout le texte, sans balises, sans style

content.textContent = 'Plain text content';
// Remplace TOUT par du texte (les balises HTML sont échappées)

// Si on essaie d'insérer du HTML avec textContent
content.textContent = '<h1>Title</h1>';
// Affiche littéralement "<h1>Title</h1>" (pas interprété comme HTML)

// innerText - similaire à textContent mais prend en compte le rendu CSS
/*
<div id="test" style="display: none;">Hidden text</div>
<div id="test2">Visible <span style="display: none;">hidden</span> text</div>
*/

const test = document.getElementById('test');
const test2 = document.getElementById('test2');

console.log(test.textContent);  // "Hidden text"
console.log(test.innerText);    // "" (vide car display: none)

console.log(test2.textContent);  // "Visible hidden text"
console.log(test2.innerText);    // "Visible text" (pas "hidden")

// Performance : textContent > innerText (innerText déclenche un reflow)

// COMPARAISON

const div = document.createElement('div');
div.innerHTML = '<p>Hello <strong>World</strong></p>';
document.body.appendChild(div);

console.log(div.innerHTML);     // "<p>Hello <strong>World</strong></p>"
console.log(div.textContent);   // "Hello World"
console.log(div.innerText);     // "Hello World"

// Cas d'usage
// innerHTML : quand vous voulez insérer du HTML
// textContent : quand vous voulez du texte pur (plus sûr, plus rapide)
// innerText : rarement utilisé (considérations de style)
```

### Attributs : `getAttribute()`, `setAttribute()`, `removeAttribute()`

```javascript
// HTML :
/*
<img id="logo" src="logo.png" alt="Company Logo" width="200">
<a id="link" href="https://example.com" target="_blank">Link</a>
<input id="username" type="text" name="user" value="John" disabled>
*/

const img = document.getElementById('logo');
const link = document.getElementById('link');
const input = document.getElementById('username');

// getAttribute() - lire un attribut
console.log(img.getAttribute('src'));    // "logo.png"
console.log(img.getAttribute('alt'));    // "Company Logo"
console.log(img.getAttribute('width'));  // "200" (string)

console.log(link.getAttribute('href'));    // "https://example.com"
console.log(link.getAttribute('target'));  // "_blank"

// Attribut inexistant retourne null
console.log(img.getAttribute('title'));  // null

// setAttribute() - définir un attribut
img.setAttribute('alt', 'New Logo');
img.setAttribute('width', '300');
img.setAttribute('data-id', '123');  // Créer un nouvel attribut

link.setAttribute('href', 'https://new-site.com');
link.setAttribute('rel', 'noopener');

input.setAttribute('placeholder', 'Enter username');
input.setAttribute('maxlength', '50');

// removeAttribute() - supprimer un attribut
img.removeAttribute('width');
console.log(img.getAttribute('width'));  // null

input.removeAttribute('disabled');  // Input devient enabled

// hasAttribute() - vérifier si un attribut existe
console.log(img.hasAttribute('src'));    // true
console.log(img.hasAttribute('title'));  // false

// PROPRIÉTÉS vs ATTRIBUTS

// Pour attributs standard, on peut utiliser des propriétés
console.log(img.src);    // "http://localhost/logo.png" (URL complète)
console.log(img.alt);    // "New Logo"
console.log(img.width);  // 300 (number, pas string)

img.src = 'new-logo.png';
img.alt = 'Another Logo';

console.log(link.href);    // "https://new-site.com/" (URL complète)
console.log(link.target);  // "_blank"

link.href = 'https://another-site.com';

// ⚠️ Différence : propriété vs attribut
// getAttribute() retourne la valeur exacte de l'attribut HTML
// Propriété peut retourner une valeur "computed"

console.log(link.getAttribute('href'));  // "https://another-site.com"
console.log(link.href);                  // "https://another-site.com/" (normalisé)

// Attributs booléens
console.log(input.disabled);  // false (boolean)
console.log(input.getAttribute('disabled'));  // null

input.disabled = true;
console.log(input.getAttribute('disabled'));  // "" (string vide)

// Attributs custom (data-*)
img.setAttribute('data-category', 'logo');
img.setAttribute('data-uploaded-date', '2024-01-01');

console.log(img.getAttribute('data-category'));  // "logo"

// Ou utiliser dataset (plus pratique)
console.log(img.dataset.category);       // "logo"
console.log(img.dataset.uploadedDate);   // "2024-01-01"
// Note: data-uploaded-date → uploadedDate (camelCase)
```

### Data attributes (`dataset`)

```javascript
// HTML :
/*
<div id="product" 
     data-id="12345" 
     data-name="Laptop" 
     data-price="999.99"
     data-in-stock="true"
     data-category-name="Electronics">
</div>
*/

const product = document.getElementById('product');

// dataset - accès facile aux data attributes
console.log(product.dataset);
// DOMStringMap { id: "12345", name: "Laptop", price: "999.99", inStock: "true", categoryName: "Electronics" }

// Lire des data attributes
console.log(product.dataset.id);       // "12345"
console.log(product.dataset.name);     // "Laptop"
console.log(product.dataset.price);    // "999.99" (string)
console.log(product.dataset.inStock);  // "true" (string)

// ⚠️ Conversion kebab-case → camelCase
// data-in-stock → inStock
// data-category-name → categoryName
console.log(product.dataset.categoryName);  // "Electronics"

// Définir des data attributes
product.dataset.discount = '10';
product.dataset.featured = 'yes';
console.log(product.getAttribute('data-discount'));  // "10"

// Modifier
product.dataset.price = '899.99';

// Supprimer
delete product.dataset.discount;
console.log(product.hasAttribute('data-discount'));  // false

// Itérer sur les data attributes
for (const [key, value] of Object.entries(product.dataset)) {
  console.log(`${key}: ${value}`);
}
// id: 12345
// name: Laptop
// price: 899.99
// inStock: true
// categoryName: Electronics
// featured: yes

// Cas d'usage pratiques

// 1. Stocker des métadonnées
/*
<button class="delete-btn" data-user-id="42" data-user-name="Alice">
  Delete User
</button>
*/

document.querySelectorAll('.delete-btn').forEach(btn => {
  btn.addEventListener('click', function() {
    const userId = this.dataset.userId;
    const userName = this.dataset.userName;
    
    if (confirm(`Delete user ${userName}?`)) {
      deleteUser(userId);
    }
  });
});

// 2. Configuration de composants
/*
<div class="carousel" 
     data-auto-play="true" 
     data-interval="3000"
     data-loop="true">
</div>
*/

const carousel = document.querySelector('.carousel');
const config = {
  autoPlay: carousel.dataset.autoPlay === 'true',
  interval: parseInt(carousel.dataset.interval),
  loop: carousel.dataset.loop === 'true'
};

console.log(config);
// { autoPlay: true, interval: 3000, loop: true }

// 3. State management
/*
<div id="tab" data-active="false">Tab Content</div>
*/

const tab = document.getElementById('tab');

function toggleTab() {
  const isActive = tab.dataset.active === 'true';
  tab.dataset.active = (!isActive).toString();
  
  if (tab.dataset.active === 'true') {
    tab.classList.add('active');
  } else {
    tab.classList.remove('active');
  }
}

// 4. Passer des données au JavaScript
/*
<div id="chart" data-values="[10, 20, 30, 40]" data-labels='["A", "B", "C", "D"]'>
</div>
*/

const chart = document.getElementById('chart');
const values = JSON.parse(chart.dataset.values);
const labels = JSON.parse(chart.dataset.labels);

console.log(values);  // [10, 20, 30, 40]
console.log(labels);  // ["A", "B", "C", "D"]
```

---

## 9.4 Création et Manipulation d'Éléments

### `createElement()`, `createTextNode()`

```javascript
// createElement() - créer un nouvel élément
const div = document.createElement('div');
console.log(div);  // <div></div>

const paragraph = document.createElement('p');
const button = document.createElement('button');
const img = document.createElement('img');

// L'élément est créé mais PAS encore dans le DOM
console.log(div.parentNode);  // null

// Ajouter du contenu
div.textContent = 'Hello World';
div.innerHTML = '<strong>Hello</strong> World';

// Ajouter des attributs
div.id = 'myDiv';
div.className = 'container';
div.setAttribute('data-value', '123');

// createTextNode() - créer un nœud texte
const textNode = document.createTextNode('This is text');
paragraph.appendChild(textNode);

// Pourquoi utiliser createTextNode() ?
// Plus sûr que innerHTML (pas d'interprétation HTML)
const userInput = '<script>alert("XSS")</script>';

// ❌ Dangereux avec innerHTML
// div.innerHTML = userInput;  // Exécuterait le script

// ✅ Sûr avec textContent ou createTextNode
div.textContent = userInput;  // Affiche littéralement le texte
// Ou
const safeText = document.createTextNode(userInput);
div.appendChild(safeText);

// Créer une structure complète
const card = document.createElement('div');
card.className = 'card';

const title = document.createElement('h2');
title.textContent = 'Card Title';

const description = document.createElement('p');
description.textContent = 'Card description';

const btn = document.createElement('button');
btn.textContent = 'Click me';
btn.className = 'btn btn-primary';

// Assembler la structure
card.appendChild(title);
card.appendChild(description);
card.appendChild(btn);

// Maintenant ajouter au DOM
document.body.appendChild(card);

// Cas pratique : créer une liste dynamiquement
const fruits = ['Apple', 'Banana', 'Orange', 'Grape'];

const ul = document.createElement('ul');
ul.className = 'fruit-list';

fruits.forEach(fruit => {
  const li = document.createElement('li');
  li.textContent = fruit;
  li.className = 'fruit-item';
  ul.appendChild(li);
});

document.body.appendChild(ul);

// Créer avec innerHTML (plus rapide pour beaucoup d'éléments)
const items = ['Item 1', 'Item 2', 'Item 3'];
const list = document.createElement('ul');

list.innerHTML = items.map(item => `<li>${item}</li>`).join('');
document.body.appendChild(list);
```

### `appendChild()`, `append()`, `prepend()`

```javascript
// HTML :
/*
<div id="container">
  <p>Existing paragraph</p>
</div>
*/

const container = document.getElementById('container');

// appendChild() - ajouter à la fin (méthode classique)
const newP = document.createElement('p');
newP.textContent = 'New paragraph';
container.appendChild(newP);

// appendChild() retourne l'élément ajouté
const returned = container.appendChild(document.createElement('p'));
console.log(returned);  // <p></p>

// ⚠️ appendChild() ne peut ajouter qu'UN SEUL node à la fois
// appendChild() déplace un élément s'il existe déjà dans le DOM

const existingP = document.querySelector('p');
container.appendChild(existingP);  // Déplace existingP à la fin

// append() - ajouter à la fin (méthode moderne)
const p1 = document.createElement('p');
const p2 = document.createElement('p');

// append() peut ajouter plusieurs nodes ET du texte
container.append(p1, p2, 'Some text', 'More text');

// append() ne retourne rien (undefined)
const result = container.append(document.createElement('span'));
console.log(result);  // undefined

// Différences appendChild() vs append()
// appendChild() :
// - Peut ajouter seulement 1 node
// - Retourne le node ajouté
// - Ne peut pas ajouter de texte directement

// append() :
// - Peut ajouter plusieurs nodes et strings
// - Ne retourne rien
// - Peut ajouter du texte directement

// prepend() - ajouter au début
container.prepend(document.createElement('h1'), 'Title: ');

// Structure finale :
/*
<div id="container">
  <h1></h1>
  "Title: "
  <p>Existing paragraph</p>
  <p>New paragraph</p>
  <p></p>
  <p></p>
  <p></p>
  "Some text"
  "More text"
  <span></span>
</div>
*/

// Cas pratique : ajouter des éléments conditionnellement
function addMessage(type, text) {
  const message = document.createElement('div');
  message.className = `alert alert-${type}`;
  message.textContent = text;
  
  const messagesContainer = document.getElementById('messages');
  
  // Ajouter au début pour les nouveaux messages
  messagesContainer.prepend(message);
  
  // Auto-supprimer après 3 secondes
  setTimeout(() => message.remove(), 3000);
}

addMessage('success', 'Operation completed!');
addMessage('error', 'An error occurred!');
```

### `insertBefore()`, `insertAdjacentElement()`

```javascript
// HTML :
/*
<ul id="list">
  <li id="item2">Item 2</li>
  <li id="item3">Item 3</li>
</ul>
*/

const list = document.getElementById('list');
const item2 = document.getElementById('item2');

// insertBefore(newNode, referenceNode) - insérer avant un élément
const item1 = document.createElement('li');
item1.textContent = 'Item 1';
item1.id = 'item1';

list.insertBefore(item1, item2);
// Résultat : Item 1 est inséré AVANT Item 2

// Si referenceNode est null, ajoute à la fin (comme appendChild)
const item4 = document.createElement('li');
item4.textContent = 'Item 4';
list.insertBefore(item4, null);  // Ajoute à la fin

// Insérer au milieu
const item2point5 = document.createElement('li');
item2point5.textContent = 'Item 2.5';
const item3 = document.getElementById('item3');
list.insertBefore(item2point5, item3);

// insertAdjacentElement(position, element) - plus flexible
/*
Positions :
- 'beforebegin' : avant l'élément
- 'afterbegin' : premier enfant de l'élément
- 'beforeend' : dernier enfant de l'élément
- 'afterend' : après l'élément
*/

// HTML :
/*
<div id="target">
  <span>Existing content</span>
</div>
*/

const target = document.getElementById('target');

const before = document.createElement('p');
before.textContent = 'Before';
target.insertAdjacentElement('beforebegin', before);

const first = document.createElement('p');
first.textContent = 'First child';
target.insertAdjacentElement('afterbegin', first);

const last = document.createElement('p');
last.textContent = 'Last child';
target.insertAdjacentElement('beforeend', last);

const after = document.createElement('p');
after.textContent = 'After';
target.insertAdjacentElement('afterend', after);

// Résultat :
/*
<p>Before</p>
<div id="target">
  <p>First child</p>
  <span>Existing content</span>
  <p>Last child</p>
</div>
<p>After</p>
*/

// insertAdjacentHTML(position, html) - insérer du HTML
target.insertAdjacentHTML('beforebegin', '<h2>Title</h2>');
target.insertAdjacentHTML('afterbegin', '<strong>Bold</strong>');

// insertAdjacentText(position, text) - insérer du texte
target.insertAdjacentText('beforeend', 'Plain text');

// Cas pratique : insérer un élément après un autre
function insertAfter(newElement, referenceElement) {
  referenceElement.parentNode.insertBefore(
    newElement, 
    referenceElement.nextSibling
  );
}

const newItem = document.createElement('li');
newItem.textContent = 'Inserted after Item 2';
insertAfter(newItem, item2);

// Ou avec insertAdjacentElement (plus simple)
function insertAfterModern(newElement, referenceElement) {
  referenceElement.insertAdjacentElement('afterend', newElement);
}
```

### `replaceChild()`, `removeChild()`, `remove()`

```javascript
// HTML :
/*
<ul id="list">
  <li id="old">Old item</li>
  <li>Item 2</li>
  <li>Item 3</li>
</ul>
*/

const list = document.getElementById('list');
const oldItem = document.getElementById('old');

// replaceChild(newChild, oldChild) - remplacer un élément
const newItem = document.createElement('li');
newItem.textContent = 'New item';
newItem.id = 'new';

list.replaceChild(newItem, oldItem);
// oldItem est remplacé par newItem

// replaceChild retourne l'ancien élément
const replaced = list.replaceChild(
  document.createElement('li'),
  newItem
);
console.log(replaced);  // <li id="new">New item</li>

// removeChild(child) - supprimer un enfant
const item2 = list.children[1];
list.removeChild(item2);

// removeChild retourne l'élément supprimé
const removed = list.removeChild(list.children[0]);
console.log(removed);  // Élément supprimé (peut être réinséré)

// remove() - supprimer l'élément lui-même (méthode moderne)
const item3 = list.querySelector('li');
item3.remove();  // Supprime item3 directement

// Pas besoin de passer par le parent
// item3.parentNode.removeChild(item3);  // Ancienne méthode

// remove() ne retourne rien
const result = item3.remove();
console.log(result);  // undefined

// Différences removeChild() vs remove()
// removeChild() :
// - Appelé sur le parent
// - Retourne l'élément supprimé
// - Plus ancien (meilleure compatibilité)

// remove() :
// - Appelé sur l'élément lui-même
// - Ne retourne rien
// - Plus simple et moderne

// Supprimer tous les enfants
const container = document.getElementById('container');

// Méthode 1 : boucle avec removeChild
while (container.firstChild) {
  container.removeChild(container.firstChild);
}

// Méthode 2 : innerHTML (plus rapide)
container.innerHTML = '';

// Méthode 3 : replaceChildren (moderne)
container.replaceChildren();  // Supprime tout

// Cas pratique : remplacer du contenu
function updateList(items) {
  const list = document.getElementById('list');
  
  // Vider la liste
  list.innerHTML = '';
  
  // Ajouter les nouveaux items
  items.forEach(item => {
    const li = document.createElement('li');
    li.textContent = item;
    list.appendChild(li);
  });
}

updateList(['Apple', 'Banana', 'Orange']);

// Cas pratique : supprimer avec confirmation
document.querySelectorAll('.delete-btn').forEach(btn => {
  btn.addEventListener('click', function() {
    const item = this.closest('.item');
    
    if (confirm('Are you sure?')) {
      item.remove();
    }
  });
});
```

---

## 9.5 Gestion des Children

### `children`, `childNodes`

```javascript
// HTML :
/*
<div id="container">
  <h1>Title</h1>
  <!-- Comment -->
  <p>Paragraph</p>
  <span>Span</span>
</div>
*/

const container = document.getElementById('container');

// children - retourne seulement les ÉLÉMENTS (HTMLCollection)
console.log(container.children);
// HTMLCollection(3) [h1, p, span]

console.log(container.children.length);  // 3
console.log(container.children[0]);      // <h1>Title</h1>

// childNodes - retourne TOUS les nodes (NodeList)
console.log(container.childNodes);
// NodeList(7) [text, h1, text, comment, text, p, text, span, text]
// Inclut : texte (espaces/retours ligne), commentaires, éléments

console.log(container.childNodes.length);  // 7

// Filtrer seulement les éléments de childNodes
const elementNodes = Array.from(container.childNodes).filter(
  node => node.nodeType === Node.ELEMENT_NODE
);
console.log(elementNodes);  // [h1, p, span]

// Différence pratique
// children : pour travailler avec les éléments HTML
// childNodes : quand vous avez besoin de tous les nodes (rare)

// Itérer sur children
for (const child of container.children) {
  console.log(child.tagName);  // H1, P, SPAN
}

// Ou
Array.from(container.children).forEach(child => {
  child.style.color = 'blue';
});

// Accéder par index
const firstElement = container.children[0];
const lastElement = container.children[container.children.length - 1];

// children est live (se met à jour automatiquement)
console.log(container.children.length);  // 3

const newDiv = document.createElement('div');
container.appendChild(newDiv);

console.log(container.children.length);  // 4 (mis à jour)
```

### `firstChild`, `lastChild`, `firstElementChild`, `lastElementChild`

```javascript
// HTML :
/*
<ul id="list">
  <li>Item 1</li>
  <li>Item 2</li>
  <li>Item 3</li>
</ul>
*/

const list = document.getElementById('list');

// firstChild - premier NODE (peut être texte/commentaire)
console.log(list.firstChild);
// #text (espaces avant le premier <li>)

// lastChild - dernier NODE
console.log(list.lastChild);
// #text (espaces après le dernier <li>)

// firstElementChild - premier ÉLÉMENT
console.log(list.firstElementChild);
// <li>Item 1</li>

// lastElementChild - dernier ÉLÉMENT
console.log(list.lastElementChild);
// <li>Item 3</li>

// ⚠️ firstChild/lastChild retournent des nodes (y compris texte)
// ✅ firstElementChild/lastElementChild retournent des éléments

// Cas pratique : ajouter une classe au premier et dernier
list.firstElementChild.classList.add('first');
list.lastElementChild.classList.add('last');

// Vérifier si existe
if (list.firstElementChild) {
  console.log('List has children');
} else {
  console.log('List is empty');
}

// Supprimer le premier élément
if (list.firstElementChild) {
  list.firstElementChild.remove();
}

// Supprimer le dernier élément
if (list.lastElementChild) {
  list.lastElementChild.remove();
}
```

### `parentNode`, `parentElement`

```javascript
// HTML :
/*
<div id="grandparent">
  <div id="parent">
    <p id="child">Child paragraph</p>
  </div>
</div>
*/

const child = document.getElementById('child');

// parentNode - parent NODE
console.log(child.parentNode);
// <div id="parent">...</div>

// parentElement - parent ÉLÉMENT
console.log(child.parentElement);
// <div id="parent">...</div>

// Pour la plupart des cas, parentNode === parentElement
console.log(child.parentNode === child.parentElement);  // true

// Différence : pour le document
console.log(document.documentElement.parentNode);    // #document
console.log(document.documentElement.parentElement); // null

// Remonter dans l'arborescence
const parent = child.parentElement;
const grandparent = parent.parentElement;

console.log(grandparent.id);  // "grandparent"

// Vérifier si un élément a un parent
if (child.parentElement) {
  console.log('Child has a parent');
}

// Cas pratique : remonter jusqu'à trouver un élément spécifique
function findAncestor(element, selector) {
  let current = element.parentElement;
  
  while (current) {
    if (current.matches(selector)) {
      return current;
    }
    current = current.parentElement;
  }
  
  return null;
}

const form = findAncestor(child, 'form');
const container = findAncestor(child, '.container');

// Ou utiliser closest() (plus simple)
const formModern = child.closest('form');
```

---

## 9.6 DOM Traversing (Navigation)

### `nextSibling`, `previousSibling`, `nextElementSibling`, `previousElementSibling`

```javascript
// HTML :
/*
<ul id="list">
  <li id="item1">Item 1</li>
  <li id="item2">Item 2</li>
  <li id="item3">Item 3</li>
</ul>
*/

const item2 = document.getElementById('item2');

// nextSibling - prochain NODE (peut être texte)
console.log(item2.nextSibling);
// #text (espaces/retours ligne)

// previousSibling - NODE précédent
console.log(item2.previousSibling);
// #text

// nextElementSibling - prochain ÉLÉMENT
console.log(item2.nextElementSibling);
// <li id="item3">Item 3</li>

// previousElementSibling - ÉLÉMENT précédent
console.log(item2.previousElementSibling);
// <li id="item1">Item 1</li>

// Naviguer dans une liste
let current = document.getElementById('item1');

while (current) {
  console.log(current.textContent);
  current = current.nextElementSibling;
}
// Item 1
// Item 2
// Item 3

// Retour en arrière
current = document.getElementById('item3');

while (current) {
  console.log(current.textContent);
  current = current.previousElementSibling;
}
// Item 3
// Item 2
// Item 1

// Cas pratique : activer l'élément suivant
function activateNext(element) {
  // Désactiver l'élément actuel
  element.classList.remove('active');
  
  // Activer le suivant (ou revenir au premier)
  const next = element.nextElementSibling || 
               element.parentElement.firstElementChild;
  
  next.classList.add('active');
}

// Cas pratique : navigation entre tabs
const tabs = document.querySelectorAll('.tab');

tabs.forEach(tab => {
  tab.addEventListener('click', function() {
    // Désactiver tous
    tabs.forEach(t => t.classList.remove('active'));
    
    // Activer celui-ci
    this.classList.add('active');
  });
  
  // Flèches clavier
  tab.addEventListener('keydown', function(e) {
    if (e.key === 'ArrowRight') {
      const next = this.nextElementSibling || tabs[0];
      next.click();
      next.focus();
    } else if (e.key === 'ArrowLeft') {
      const prev = this.previousElementSibling || tabs[tabs.length - 1];
      prev.click();
      prev.focus();
    }
  });
});
```

### `closest()` - Recherche d'ancêtre

```javascript
// HTML :
/*
<div class="container">
  <div class="card">
    <div class="card-body">
      <button class="btn">Click me</button>
    </div>
  </div>
</div>
*/

const btn = document.querySelector('.btn');

// closest(selector) - trouve l'ancêtre le plus proche correspondant au sélecteur
const card = btn.closest('.card');
console.log(card);  // <div class="card">...</div>

const container = btn.closest('.container');
console.log(container);  // <div class="container">...</div>

// closest() inclut l'élément lui-même
const btnItself = btn.closest('.btn');
console.log(btnItself === btn);  // true

// Si aucun ancêtre trouvé, retourne null
const notFound = btn.closest('.does-not-exist');
console.log(notFound);  // null

// Cas pratique : event delegation
document.addEventListener('click', function(e) {
  // Trouver si on a cliqué sur ou dans un bouton delete
  const deleteBtn = e.target.closest('.delete-btn');
  
  if (deleteBtn) {
    const item = deleteBtn.closest('.item');
    item.remove();
  }
});

// Cas pratique : toggle accordion
document.querySelectorAll('.accordion-header').forEach(header => {
  header.addEventListener('click', function() {
    const accordion = this.closest('.accordion');
    const content = accordion.querySelector('.accordion-content');
    
    content.classList.toggle('show');
  });
});

// Comparaison avec parentElement loop
// Ancienne méthode (avant closest)
function findAncestor(element, className) {
  let current = element.parentElement;
  
  while (current) {
    if (current.classList.contains(className)) {
      return current;
    }
    current = current.parentElement;
  }
  
  return null;
}

// Nouvelle méthode (avec closest) - plus simple
const ancestor = btn.closest('.card');
```

---

## 9.7 Clonage d'Éléments

### `cloneNode(deep)`

```javascript
// HTML :
/*
<div id="original" class="box">
  <h2>Title</h2>
  <p>Content</p>
</div>
*/

const original = document.getElementById('original');

// cloneNode(false) - clone shallow (seulement l'élément, pas ses enfants)
const shallowClone = original.cloneNode(false);
console.log(shallowClone);
// <div id="original" class="box"></div> (vide, pas d'enfants)

// cloneNode(true) - clone deep (élément + tous ses enfants)
const deepClone = original.cloneNode(true);
console.log(deepClone);
// <div id="original" class="box">
//   <h2>Title</h2>
//   <p>Content</p>
// </div>

// ⚠️ Le clone garde le même ID (il faut le changer)
deepClone.id = 'clone';

// Ajouter le clone au DOM
document.body.appendChild(deepClone);

// Les event listeners ne sont PAS clonés
original.addEventListener('click', () => console.log('Original clicked'));
// deepClone n'a pas cet event listener

// Cas pratique : dupliquer un élément de liste
/*
<ul id="list">
  <li class="item">
    <span class="name">Item 1</span>
    <button class="delete">Delete</button>
  </li>
</ul>
*/

const list = document.getElementById('list');
const firstItem = list.querySelector('.item');

function addItem(name) {
  const clone = firstItem.cloneNode(true);
  clone.querySelector('.name').textContent = name;
  list.appendChild(clone);
}

addItem('Item 2');
addItem('Item 3');

// Cas pratique : template de card
const cardTemplate = document.querySelector('#card-template');

function createCard(title, description) {
  const card = cardTemplate.cloneNode(true);
  card.removeAttribute('id');  // Supprimer l'ID du template
  card.querySelector('.card-title').textContent = title;
  card.querySelector('.card-description').textContent = description;
  return card;
}

const card1 = createCard('Card 1', 'Description 1');
const card2 = createCard('Card 2', 'Description 2');

document.getElementById('cards-container').append(card1, card2);

// Cas pratique : cloner pour animation
function animateElement(element) {
  const clone = element.cloneNode(true);
  clone.classList.add('animating');
  
  element.parentNode.insertBefore(clone, element.nextSibling);
  
  setTimeout(() => {
    clone.remove();
  }, 1000);
}
```

---

# Chapitre 9 : DOM (Document Object Model) - Questions/Réponses Orales

## Questions sur les Fondamentaux

### Q1: Qu'est-ce que le DOM et pourquoi est-il important ?

**Réponse :**

Le **DOM (Document Object Model)** est une représentation en **arborescence** (tree structure) d'un document HTML/XML. C'est l'interface entre JavaScript et la page web.

```javascript
// Le DOM transforme ce HTML :
/*
<html>
  <body>
    <h1 id="title">Hello</h1>
    <p>Paragraph</p>
  </body>
</html>
*/

// En une structure arborescente accessible via JavaScript :
/*
Document
  └─ html
      └─ body
          ├─ h1 (id="title")
          │   └─ "Hello"
          └─ p
              └─ "Paragraph"
*/

// Accéder aux éléments
console.log(document);               // Document entier
console.log(document.documentElement); // <html>
console.log(document.body);          // <body>

// Manipuler le contenu
const title = document.getElementById('title');
title.textContent = 'Hello World';  // Modifier le texte
```

**Pourquoi c'est important :**
- ✅ Permet de manipuler le HTML avec JavaScript
- ✅ Rendre les pages interactives
- ✅ Réagir aux actions de l'utilisateur
- ✅ Modifier le contenu dynamiquement sans recharger la page
- ✅ Base de tous les frameworks (React, Vue, Angular)

---

### Q2: Quelle est la différence entre `Node`, `Element` et `HTMLElement` ?

**Réponse :**

Ce sont des **hiérarchies de classes** dans le DOM :

```javascript
// NODE : classe de base (tous les types de nodes)
// - Éléments HTML
// - Texte
// - Commentaires
// - Document

// ELEMENT : sous-classe de Node (éléments HTML uniquement)
// HTMLELEMENT : sous-classe d'Element (éléments HTML spécifiques)

const h1 = document.querySelector('h1');

console.log(h1 instanceof Node);        // true
console.log(h1 instanceof Element);     // true
console.log(h1 instanceof HTMLElement); // true
console.log(h1 instanceof HTMLHeadingElement); // true (plus spécifique)

// Types de nodes
console.log(Node.ELEMENT_NODE);     // 1
console.log(Node.TEXT_NODE);        // 3
console.log(Node.COMMENT_NODE);     // 8
console.log(Node.DOCUMENT_NODE);    // 9

// Vérifier le type
const element = document.querySelector('h1');
console.log(element.nodeType);   // 1 (ELEMENT_NODE)
console.log(element.nodeName);   // "H1"

const textNode = element.firstChild;
console.log(textNode.nodeType);  // 3 (TEXT_NODE)
console.log(textNode.nodeName);  // "#text"
```

**Résumé :**
- **Node** : tous les types (texte, élément, commentaire, etc.)
- **Element** : éléments HTML génériques
- **HTMLElement** : éléments HTML spécifiques avec leurs propriétés

---

### Q3: Quelle est la différence entre `children` et `childNodes` ?

**Réponse :**

**`children`** : retourne seulement les **ÉLÉMENTS** (HTMLCollection)
**`childNodes`** : retourne **TOUS les nodes** (éléments, texte, commentaires) (NodeList)

```javascript
// HTML :
/*
<div id="container">
  <h1>Title</h1>
  <!-- Comment -->
  <p>Paragraph</p>
</div>
*/

const container = document.getElementById('container');

// children - seulement les éléments
console.log(container.children);
// HTMLCollection(2) [h1, p]

console.log(container.children.length);  // 2

// childNodes - tous les nodes (y compris texte et commentaires)
console.log(container.childNodes);
// NodeList(7) [text, h1, text, comment, text, p, text]
// Les "text" sont les espaces/retours à la ligne

console.log(container.childNodes.length);  // 7
```

**Quand utiliser quoi :**
- ✅ **`children`** : dans 99% des cas (travailler avec des éléments HTML)
- ❌ **`childNodes`** : rarement (seulement si vous avez besoin des nodes texte/commentaires)

---

## Questions sur la Sélection d'Éléments

### Q4: Quelle est la différence entre `getElementById()` et `querySelector()` ?

**Réponse :**

**Différences principales :**

```javascript
// getElementById() - seulement par ID
const element1 = document.getElementById('myId');

// Avantages :
// - Plus rapide (O(1))
// - Simple et direct

// Limitations :
// - Seulement ID
// - Seulement sur document (pas sur éléments)

// querySelector() - sélecteur CSS
const element2 = document.querySelector('#myId');

// Avantages :
// - Très flexible (tous sélecteurs CSS)
// - Fonctionne sur n'importe quel élément

// Limitations :
// - Plus lent (doit parser le sélecteur CSS)
```

**Comparaison de performance :**

```javascript
// getElementById - le plus rapide
console.time('getElementById');
for (let i = 0; i < 10000; i++) {
  document.getElementById('container');
}
console.timeEnd('getElementById');  // ~1ms

// querySelector - plus lent
console.time('querySelector');
for (let i = 0; i < 10000; i++) {
  document.querySelector('#container');
}
console.timeEnd('querySelector');  // ~10ms
```

**Quand utiliser quoi :**
- ✅ **`getElementById`** : si vous sélectionnez par ID (plus rapide)
- ✅ **`querySelector`** : pour tous les autres cas (plus flexible)

---

### Q5: Quelle est la différence entre `querySelector()` et `querySelectorAll()` ?

**Réponse :**

**`querySelector()`** : retourne le **PREMIER** élément correspondant
**`querySelectorAll()`** : retourne **TOUS** les éléments correspondants

```javascript
// HTML :
/*
<div class="box">Box 1</div>
<div class="box">Box 2</div>
<div class="box">Box 3</div>
*/

// querySelector - premier élément
const firstBox = document.querySelector('.box');
console.log(firstBox);  // <div class="box">Box 1</div>

// querySelectorAll - tous les éléments
const allBoxes = document.querySelectorAll('.box');
console.log(allBoxes);  // NodeList(3) [div.box, div.box, div.box]

// querySelector retourne null si non trouvé
const notFound1 = document.querySelector('.does-not-exist');
console.log(notFound1);  // null

// querySelectorAll retourne NodeList vide
const notFound2 = document.querySelectorAll('.does-not-exist');
console.log(notFound2);  // NodeList(0) []
console.log(notFound2.length);  // 0
```

**Différences d'itération :**

```javascript
// querySelector - pas d'itération (1 élément)
const box = document.querySelector('.box');
box.style.color = 'red';

// querySelectorAll - itération nécessaire
const boxes = document.querySelectorAll('.box');

// forEach natif sur NodeList
boxes.forEach(box => {
  box.style.color = 'blue';
});

// for...of
for (const box of boxes) {
  console.log(box);
}
```

**Quand utiliser quoi :**
- ✅ **`querySelector`** : si vous voulez UN seul élément
- ✅ **`querySelectorAll`** : si vous voulez TOUS les éléments

---

### Q6: Quelle est la différence entre HTMLCollection et NodeList ?

**Réponse :**

**HTMLCollection** : collection **live** (se met à jour automatiquement)
**NodeList** : collection **statique** (ne se met pas à jour)

```javascript
// HTMLCollection (getElementsByClassName, getElementsByTagName, children)
const liveCollection = document.getElementsByClassName('box');
console.log(liveCollection.length);  // 3

// Ajouter un élément
const newBox = document.createElement('div');
newBox.className = 'box';
document.body.appendChild(newBox);

console.log(liveCollection.length);  // 4 (mis à jour automatiquement !)

// NodeList (querySelectorAll)
const staticList = document.querySelectorAll('.box');
console.log(staticList.length);  // 4

// Ajouter un autre élément
const anotherBox = document.createElement('div');
anotherBox.className = 'box';
document.body.appendChild(anotherBox);

console.log(staticList.length);  // 4 (PAS mis à jour)

// Pour avoir la liste à jour avec NodeList, refaire querySelectorAll
const updatedList = document.querySelectorAll('.box');
console.log(updatedList.length);  // 5
```

**Autres différences :**

```javascript
// HTMLCollection - PAS de forEach natif
const htmlColl = document.getElementsByClassName('box');
// htmlColl.forEach(box => {});  // ❌ TypeError

// Convertir en array
Array.from(htmlColl).forEach(box => {
  console.log(box);
});

// NodeList - forEach natif (navigateurs modernes)
const nodeList = document.querySelectorAll('.box');
nodeList.forEach(box => {  // ✅ Fonctionne
  console.log(box);
});
```

**Tableau comparatif :**

| Critère | HTMLCollection | NodeList |
|---------|----------------|----------|
| Live/Static | ✅ Live | ❌ Static |
| Méthodes | `getElementsByClassName`, `getElementsByTagName`, `children` | `querySelectorAll`, `childNodes` |
| `forEach` | ❌ Non | ✅ Oui |
| Types | Éléments seulement | Tous types de nodes |

---

## Questions sur la Manipulation

### Q7: Quelle est la différence entre `innerHTML`, `textContent` et `innerText` ?

**Réponse :**

```javascript
// HTML :
/*
<div id="content">
  <h1>Title</h1>
  <p>This is <strong>bold</strong> text.</p>
</div>
*/

const content = document.getElementById('content');

// innerHTML - HTML complet (avec balises)
console.log(content.innerHTML);
// "<h1>Title</h1>\n  <p>This is <strong>bold</strong> text.</p>"

// textContent - texte pur (sans balises, sans considération de style)
console.log(content.textContent);
// "Title\nThis is bold text."

// innerText - texte visible (avec considération de style)
console.log(content.innerText);
// "Title\nThis is bold text." (presque pareil mais prend en compte display:none)
```

**Différences avec éléments cachés :**

```javascript
// HTML :
/*
<div id="test">
  Visible text
  <span style="display: none;">Hidden text</span>
</div>
*/

const test = document.getElementById('test');

console.log(test.textContent);  // "Visible text Hidden text"
console.log(test.innerText);    // "Visible text" (pas "Hidden text")

// innerText déclenche un reflow (plus lent)
// textContent est plus performant
```

**Modification :**

```javascript
// innerHTML - peut insérer du HTML
content.innerHTML = '<h2>New Title</h2><p>New paragraph</p>';

// textContent - échappe le HTML (plus sûr)
content.textContent = '<h2>This will be text</h2>';
// Affiche littéralement "<h2>This will be text</h2>"

// ⚠️ Risque XSS avec innerHTML
const userInput = '<script>alert("XSS")</script>';
// content.innerHTML = userInput;  // ❌ Dangereux
content.textContent = userInput;   // ✅ Sûr (échappé)
```

**Quand utiliser quoi :**
- ✅ **`innerHTML`** : quand vous voulez insérer du HTML
- ✅ **`textContent`** : quand vous voulez du texte pur (plus sûr, plus rapide)
- ❌ **`innerText`** : rarement (prend en compte les styles, plus lent)

---

### Q8: Quelle est la différence entre `getAttribute()` et accéder directement à la propriété ?

**Réponse :**

```javascript
// HTML :
/*
<a id="link" href="/page" class="nav-link">Link</a>
<img id="logo" src="logo.png" width="200">
*/

const link = document.getElementById('link');
const img = document.getElementById('logo');

// getAttribute() - retourne la valeur exacte de l'attribut HTML
console.log(link.getAttribute('href'));  // "/page"
console.log(img.getAttribute('width'));  // "200" (string)

// Propriété directe - peut retourner une valeur "computed"
console.log(link.href);   // "http://localhost/page" (URL complète)
console.log(img.width);   // 200 (number)

// getAttribute() retourne null si l'attribut n'existe pas
console.log(link.getAttribute('title'));  // null

// Propriété retourne undefined ou valeur par défaut
console.log(link.title);  // "" (string vide)

// Avec attributs booléens
/*
<input id="checkbox" type="checkbox" checked>
*/
const checkbox = document.getElementById('checkbox');

console.log(checkbox.getAttribute('checked'));  // "" (présent)
console.log(checkbox.checked);                  // true (boolean)

checkbox.removeAttribute('checked');
console.log(checkbox.getAttribute('checked'));  // null
console.log(checkbox.checked);                  // false
```

**Quand utiliser quoi :**
- ✅ **`getAttribute()`** : pour valeur exacte de l'attribut HTML ou attributs custom
- ✅ **Propriété directe** : pour attributs standard (plus simple)

---

### Q9: Comment créer et ajouter un élément au DOM ?

**Réponse :**

**Étapes :**
1. Créer l'élément avec `createElement()`
2. Configurer l'élément (contenu, attributs, classes)
3. Ajouter au DOM avec `appendChild()`, `append()`, etc.

```javascript
// 1. Créer un élément
const div = document.createElement('div');

// 2. Configurer
div.className = 'card';
div.id = 'card-1';
div.textContent = 'Card content';

// Ou avec innerHTML
div.innerHTML = '<h2>Title</h2><p>Description</p>';

// Ajouter des attributs
div.setAttribute('data-id', '123');

// 3. Ajouter au DOM
document.body.appendChild(div);

// Exemple complet : créer une card
function createCard(title, description) {
  const card = document.createElement('div');
  card.className = 'card';
  
  const cardTitle = document.createElement('h2');
  cardTitle.textContent = title;
  
  const cardDesc = document.createElement('p');
  cardDesc.textContent = description;
  
  const cardBtn = document.createElement('button');
  cardBtn.textContent = 'Click me';
  cardBtn.className = 'btn';
  
  // Assembler
  card.appendChild(cardTitle);
  card.appendChild(cardDesc);
  card.appendChild(cardBtn);
  
  return card;
}

// Utiliser
const card = createCard('My Card', 'Card description');
document.body.appendChild(card);

// Méthode rapide avec innerHTML
function createCardFast(title, description) {
  const card = document.createElement('div');
  card.className = 'card';
  card.innerHTML = `
    <h2>${title}</h2>
    <p>${description}</p>
    <button class="btn">Click me</button>
  `;
  return card;
}
```

---

### Q10: Quelle est la différence entre `appendChild()` et `append()` ?

**Réponse :**

```javascript
const container = document.getElementById('container');

// appendChild() - méthode classique
const p1 = document.createElement('p');
p1.textContent = 'Paragraph 1';
container.appendChild(p1);

// - Ne peut ajouter qu'UN SEUL node
// - Retourne le node ajouté
const returned = container.appendChild(document.createElement('p'));
console.log(returned);  // <p></p>

// - Ne peut pas ajouter de texte directement
// container.appendChild('text');  // ❌ Error

// append() - méthode moderne
const p2 = document.createElement('p');
const p3 = document.createElement('p');

// - Peut ajouter PLUSIEURS nodes ET du texte
container.append(p2, p3, 'Some text', 'More text');

// - Ne retourne rien (undefined)
const result = container.append(document.createElement('span'));
console.log(result);  // undefined

// - Plus flexible
container.append('Text before', document.createElement('div'), 'Text after');
```

**Tableau comparatif :**

| Critère | `appendChild()` | `append()` |
|---------|----------------|-----------|
| Nombre d'éléments | 1 seul | Plusieurs |
| Texte direct | ❌ Non | ✅ Oui |
| Retour | Élément ajouté | undefined |
| Support | Ancien (IE9+) | Moderne (IE non supporté) |

**Quand utiliser quoi :**
- ✅ **`append()`** : dans la plupart des cas (plus flexible)
- ✅ **`appendChild()`** : si vous avez besoin de la valeur retournée ou compatibilité anciens navigateurs

---

### Q11: Comment supprimer un élément du DOM ?

**Réponse :**

Deux méthodes principales :

```javascript
// HTML :
/*
<ul id="list">
  <li id="item1">Item 1</li>
  <li id="item2">Item 2</li>
  <li id="item3">Item 3</li>
</ul>
*/

const item1 = document.getElementById('item1');
const item2 = document.getElementById('item2');

// Méthode 1 : removeChild() - méthode classique (sur le parent)
const list = document.getElementById('list');
list.removeChild(item1);

// removeChild() retourne l'élément supprimé
const removed = list.removeChild(item2);
console.log(removed);  // <li id="item2">Item 2</li>
// Peut être réinséré plus tard
list.appendChild(removed);

// Méthode 2 : remove() - méthode moderne (sur l'élément lui-même)
item3.remove();  // Plus simple !

// remove() ne retourne rien
const result = item3.remove();
console.log(result);  // undefined
```

**Supprimer tous les enfants :**

```javascript
const container = document.getElementById('container');

// Méthode 1 : boucle avec removeChild
while (container.firstChild) {
  container.removeChild(container.firstChild);
}

// Méthode 2 : innerHTML = '' (plus rapide)
container.innerHTML = '';

// Méthode 3 : replaceChildren() (moderne)
container.replaceChildren();
```

**Cas pratique :**

```javascript
// Supprimer avec confirmation
document.querySelectorAll('.delete-btn').forEach(btn => {
  btn.addEventListener('click', function() {
    const item = this.closest('.item');
    
    if (confirm('Are you sure you want to delete this item?')) {
      item.remove();
    }
  });
});
```

---

## Questions sur la Navigation

### Q12: Quelle est la différence entre `firstChild` et `firstElementChild` ?

**Réponse :**

```javascript
// HTML :
/*
<ul id="list">
  <li>Item 1</li>
  <li>Item 2</li>
</ul>
*/

const list = document.getElementById('list');

// firstChild - premier NODE (peut être texte/commentaire)
console.log(list.firstChild);
// #text (espaces/retours ligne avant <li>)

console.log(list.firstChild.nodeType);  // 3 (TEXT_NODE)

// firstElementChild - premier ÉLÉMENT
console.log(list.firstElementChild);
// <li>Item 1</li>

console.log(list.firstElementChild.nodeType);  // 1 (ELEMENT_NODE)

// Pareil pour lastChild vs lastElementChild
console.log(list.lastChild);         // #text
console.log(list.lastElementChild);  // <li>Item 2</li>
```

**Règle simple :**
- ❌ **`firstChild` / `lastChild`** : retournent des nodes (y compris texte)
- ✅ **`firstElementChild` / `lastElementChild`** : retournent des éléments (ce qu'on veut généralement)

**Quand les utiliser :**

```javascript
// ❌ Mauvais
if (list.firstChild) {
  list.firstChild.classList.add('first');  // Error si c'est un text node
}

// ✅ Bon
if (list.firstElementChild) {
  list.firstElementChild.classList.add('first');  // ✅ Fonctionne
}
```

---

### Q13: Comment remonter dans l'arborescence du DOM pour trouver un ancêtre spécifique ?

**Réponse :**

Utilisez **`closest()`** - c'est la méthode la plus simple et moderne :

```javascript
// HTML :
/*
<div class="container">
  <div class="card">
    <div class="card-body">
      <button id="btn">Click me</button>
    </div>
  </div>
</div>
*/

const btn = document.getElementById('btn');

// closest() - trouve l'ancêtre le plus proche correspondant au sélecteur
const card = btn.closest('.card');
console.log(card);  // <div class="card">...</div>

const container = btn.closest('.container');
console.log(container);  // <div class="container">...</div>

// closest() inclut l'élément lui-même
const btnItself = btn.closest('#btn');
console.log(btnItself === btn);  // true

// Si aucun ancêtre trouvé, retourne null
const notFound = btn.closest('.does-not-exist');
console.log(notFound);  // null
```

**Ancienne méthode (avant `closest()`) :**

```javascript
// Remonter manuellement avec parentElement
function findAncestor(element, selector) {
  let current = element.parentElement;
  
  while (current) {
    if (current.matches(selector)) {
      return current;
    }
    current = current.parentElement;
  }
  
  return null;
}

const card = findAncestor(btn, '.card');
```

**Cas pratique : Event delegation**

```javascript
// Gérer les clics sur des boutons delete
document.addEventListener('click', function(e) {
  const deleteBtn = e.target.closest('.delete-btn');
  
  if (deleteBtn) {
    const item = deleteBtn.closest('.item');
    item.remove();
  }
});
```

---

### Q14: Quelle est la différence entre `nextSibling` et `nextElementSibling` ?

**Réponse :**

Même différence que pour `firstChild` / `firstElementChild` :

```javascript
// HTML :
/*
<ul>
  <li id="item1">Item 1</li>
  <li id="item2">Item 2</li>
  <li id="item3">Item 3</li>
</ul>
*/

const item2 = document.getElementById('item2');

// nextSibling - prochain NODE (peut être texte)
console.log(item2.nextSibling);
// #text (espaces/retours ligne)

// nextElementSibling - prochain ÉLÉMENT
console.log(item2.nextElementSibling);
// <li id="item3">Item 3</li>

// previousSibling vs previousElementSibling (pareil)
console.log(item2.previousSibling);         // #text
console.log(item2.previousElementSibling);  // <li id="item1">Item 1</li>
```

**Cas pratique : navigation dans une liste**

```javascript
// Activer l'élément suivant
function activateNext(element) {
  element.classList.remove('active');
  
  // Utiliser nextElementSibling (pas nextSibling)
  const next = element.nextElementSibling || 
               element.parentElement.firstElementChild;
  
  next.classList.add('active');
}

// Navigation au clavier
const items = document.querySelectorAll('.item');

items.forEach(item => {
  item.addEventListener('keydown', function(e) {
    if (e.key === 'ArrowRight') {
      const next = this.nextElementSibling || items[0];
      next.focus();
    } else if (e.key === 'ArrowLeft') {
      const prev = this.previousElementSibling || items[items.length - 1];
      prev.focus();
    }
  });
});
```

---

### Q15: Comment cloner un élément du DOM ?

**Réponse :**

Utilisez **`cloneNode(deep)`** :

```javascript
// HTML :
/*
<div id="original" class="box">
  <h2>Title</h2>
  <p>Content</p>
</div>
*/

const original = document.getElementById('original');

// cloneNode(false) - clone shallow (seulement l'élément, pas ses enfants)
const shallowClone = original.cloneNode(false);
console.log(shallowClone);
// <div id="original" class="box"></div> (vide)

// cloneNode(true) - clone deep (élément + tous ses enfants)
const deepClone = original.cloneNode(true);
console.log(deepClone);
// <div id="original" class="box">
//   <h2>Title</h2>
//   <p>Content</p>
// </div>

// ⚠️ Le clone garde le même ID (il faut le changer)
deepClone.id = 'clone';
deepClone.querySelector('h2').textContent = 'Cloned Title';

// Ajouter au DOM
document.body.appendChild(deepClone);

// ⚠️ Les event listeners ne sont PAS clonés
original.addEventListener('click', () => console.log('Clicked'));
// deepClone n'a pas cet event listener
```

**Cas pratique : template réutilisable**

```javascript
// HTML :
/*
<template id="card-template">
  <div class="card">
    <h3 class="card-title"></h3>
    <p class="card-description"></p>
    <button class="card-btn">Learn more</button>
  </div>
</template>
*/

const template = document.getElementById('card-template');

function createCard(title, description) {
  // Cloner le contenu du template
  const clone = template.content.cloneNode(true);
  
  // Remplir le contenu
  clone.querySelector('.card-title').textContent = title;
  clone.querySelector('.card-description').textContent = description;
  
  return clone;
}

// Utiliser
const card1 = createCard('Card 1', 'Description 1');
const card2 = createCard('Card 2', 'Description 2');

document.getElementById('cards-container').append(card1, card2);
```

---

## Questions Pièges

### Q16: Que se passe-t-il avec ce code ?

```javascript
const items = document.querySelectorAll('.item');
items.forEach(item => item.remove());
```

**Réponse :**

```javascript
const items = document.querySelectorAll('.item');
items.forEach(item => item.remove());

// ✅ Fonctionne parfaitement !
// Tous les items sont supprimés

// Pourquoi ? querySelectorAll() retourne une NodeList STATIQUE
// La liste ne change pas pendant la boucle
```

**Mais avec `getElementsByClassName()` (collection LIVE) :**

```javascript
const items = document.getElementsByClassName('item');

// ❌ Problème : collection LIVE
for (let i = 0; i < items.length; i++) {
  items[i].remove();
  // La collection se met à jour à chaque suppression !
  // items[0] est supprimé, items[1] devient items[0]
  // On saute un élément à chaque itération
}

// ✅ Solution 1 : boucler à l'envers
for (let i = items.length - 1; i >= 0; i--) {
  items[i].remove();
}

// ✅ Solution 2 : convertir en array
[...items].forEach(item => item.remove());

// ✅ Solution 3 : toujours supprimer le premier
while (items.length > 0) {
  items[0].remove();
}
```

---

### Q17: Pourquoi ce code ne fonctionne-t-il pas ?

```javascript
const container = document.getElementById('container');
container.innerHTML += '<div>New div</div>';
// Les event listeners sur les enfants existants ne fonctionnent plus !
```

**Réponse :**

```javascript
const container = document.getElementById('container');

const btn = document.createElement('button');
btn.textContent = 'Click me';
btn.addEventListener('click', () => console.log('Clicked!'));
container.appendChild(btn);

// ❌ Problème : innerHTML += récrée TOUS les éléments
container.innerHTML += '<div>New div</div>';

// Le bouton est recréé (nouvelle instance)
// L'event listener est perdu !

// ✅ Solution : utiliser append() ou appendChild()
const newDiv = document.createElement('div');
newDiv.textContent = 'New div';
container.appendChild(newDiv);
// Les éléments existants restent intacts avec leurs event listeners
```

**Pourquoi ça arrive :**
- `innerHTML +=` est équivalent à `innerHTML = innerHTML + '...'`
- Cela **détruit** tout le contenu existant et le **recrée** depuis le HTML
- Tous les event listeners et références sont perdus

---

### Q18: Quelle est la différence entre ces deux codes ?

```javascript
// Code 1
element.textContent = userInput;

// Code 2
element.innerHTML = userInput;
```

**Réponse :**

```javascript
const userInput = '<img src=x onerror="alert(\'XSS\')">';

// Code 1 : textContent (SÛR)
element.textContent = userInput;
// Affiche littéralement : <img src=x onerror="alert('XSS')">
// Le HTML est ÉCHAPPÉ, pas exécuté

// Code 2 : innerHTML (DANGEREUX)
element.innerHTML = userInput;
// ❌ Exécute le code malveillant !
// L'image échoue à charger → onerror s'exécute → alert s'affiche

// Autre exemple
const maliciousInput = '<script>alert("Hacked!")</script>';

element.textContent = maliciousInput;
// ✅ Affiche le texte "<script>alert("Hacked!")</script>"

element.innerHTML = maliciousInput;
// ⚠️ Le script n'est pas exécuté (innerHTML ne permet pas <script>)
// MAIS d'autres vecteurs XSS fonctionnent (img, iframe, etc.)
```

**Règle de sécurité :**
- ✅ **`textContent`** : TOUJOURS pour du contenu utilisateur
- ❌ **`innerHTML`** : seulement pour du HTML de confiance (contrôlé par vous)
- ✅ **Alternative sûre** : créer des éléments avec `createElement()` et `textContent`

```javascript
// ✅ Méthode sûre pour insérer du contenu utilisateur
function createSafeElement(tag, text) {
  const element = document.createElement(tag);
  element.textContent = text;  // Échappe automatiquement
  return element;
}

const userDiv = createSafeElement('div', userInput);
container.appendChild(userDiv);
```

---

### Q19: Que se passe-t-il si on fait `appendChild()` avec un élément qui existe déjà dans le DOM ?

**Réponse :**

```javascript
// HTML :
/*
<div id="container1">
  <p id="paragraph">I'm in container 1</p>
</div>
<div id="container2"></div>
*/

const paragraph = document.getElementById('paragraph');
const container1 = document.getElementById('container1');
const container2 = document.getElementById('container2');

console.log(container1.contains(paragraph));  // true

// appendChild() avec un élément existant le DÉPLACE (pas de duplication)
container2.appendChild(paragraph);

console.log(container1.contains(paragraph));  // false
console.log(container2.contains(paragraph));  // true

// Le paragraphe a été DÉPLACÉ de container1 vers container2
```

**Pour vraiment DUPLIQUER un élément :**

```javascript
// Utiliser cloneNode()
const paragraphCopy = paragraph.cloneNode(true);
container2.appendChild(paragraphCopy);

// Maintenant le paragraphe existe dans les DEUX containers
console.log(container1.contains(paragraph));      // true
console.log(container2.contains(paragraphCopy));  // true
```

**Important pour React/frameworks :**
- Les frameworks gèrent ça différemment
- En JavaScript pur, un élément ne peut exister qu'à UN seul endroit dans le DOM
- `appendChild()` déplace toujours, ne copie jamais

---

### Q20: Pourquoi ce code ne fonctionne-t-il pas comme prévu ?

```javascript
const divs = document.querySelectorAll('div');
divs.style.color = 'red';  // ?
```

**Réponse :**

```javascript
const divs = document.querySelectorAll('div');
divs.style.color = 'red';  // ❌ Ne fonctionne pas !

// Pourquoi ? querySelectorAll() retourne une NODELIST (collection)
// Pas un élément unique
// On ne peut pas appliquer .style directement sur une collection

console.log(divs.style);  // undefined
```

**Solutions :**

```javascript
// Solution 1 : boucle forEach
divs.forEach(div => {
  div.style.color = 'red';
});

// Solution 2 : for...of
for (const div of divs) {
  div.style.color = 'red';
}

// Solution 3 : for classique
for (let i = 0; i < divs.length; i++) {
  divs[i].style.color = 'red';
}

// Solution 4 : avec spread + forEach
[...divs].forEach(div => {
  div.style.color = 'red';
});

// Pour UN seul élément, utiliser querySelector (pas querySelectorAll)
const firstDiv = document.querySelector('div');
firstDiv.style.color = 'red';  // ✅ Fonctionne
```

---

### Q21: Quelle est la différence entre `document.getElementById('id')` et `document.querySelector('#id')` en termes de performance ?

**Réponse :**

**Performance : `getElementById()` est beaucoup plus rapide**

```javascript
// Benchmark
console.time('getElementById');
for (let i = 0; i < 100000; i++) {
  document.getElementById('myElement');
}
console.timeEnd('getElementById');  // ~1-2ms

console.time('querySelector');
for (let i = 0; i < 100000; i++) {
  document.querySelector('#myElement');
}
console.timeEnd('querySelector');  // ~10-20ms

// getElementById est environ 10x plus rapide !
```

**Pourquoi ?**
- `getElementById()` : accès direct via hash table interne (O(1))
- `querySelector()` : doit parser le sélecteur CSS puis chercher (O(n))

**Quand ça compte :**

```javascript
// ❌ Mauvais : dans une boucle fréquente
function updateUI() {
  for (let i = 0; i < 1000; i++) {
    const element = document.querySelector('#myElement');  // Lent
    element.textContent = i;
  }
}

// ✅ Bon : sélectionner une fois, réutiliser
function updateUIOptimized() {
  const element = document.getElementById('myElement');  // Rapide + réutilisé
  for (let i = 0; i < 1000; i++) {
    element.textContent = i;
  }
}
```

**Recommandation :**
- ✅ **`getElementById`** : pour IDs (plus rapide)
- ✅ **`querySelector`** : pour tout le reste (plus flexible)
- ✅ **Mise en cache** : stocker les références dans des variables

```javascript
// Mettre en cache les sélections fréquentes
const header = document.getElementById('header');
const nav = document.getElementById('nav');
const content = document.getElementById('content');

// Réutiliser
function updateHeader(text) {
  header.textContent = text;  // Pas de nouvelle sélection
}
```

---

### Q22: Comment savoir si un élément existe dans le DOM ?

**Réponse :**

Vérifier si le résultat de la sélection est **non null** :

```javascript
// querySelector retourne null si non trouvé
const element = document.querySelector('.my-element');

if (element) {
  console.log('Element exists!');
  element.style.color = 'red';
} else {
  console.log('Element not found');
}

// Ou de manière plus explicite
if (element !== null) {
  // Element exists
}

// getElementById aussi retourne null
const byId = document.getElementById('myId');

if (byId) {
  // Found
}

// querySelectorAll retourne toujours un NodeList (jamais null)
const elements = document.querySelectorAll('.items');

if (elements.length > 0) {
  console.log(`Found ${elements.length} elements`);
} else {
  console.log('No elements found');
}
```

**Cas pratique : sélection défensive**

```javascript
// ❌ Mauvais : peut causer une erreur
function updateTitle(text) {
  document.getElementById('title').textContent = text;
  // Error si 'title' n'existe pas
}

// ✅ Bon : vérifier avant
function updateTitleSafe(text) {
  const title = document.getElementById('title');
  
  if (title) {
    title.textContent = text;
  } else {
    console.warn('Title element not found');
  }
}

// ✅ Avec optional chaining (moderne)
function updateTitleModern(text) {
  const title = document.getElementById('title');
  title?.textContent = text;  // Pas d'erreur si title est null
}
```

**Vérifier si un élément est dans le DOM :**

```javascript
// contains() - vérifier si un élément est descendant d'un autre
const container = document.getElementById('container');
const element = document.getElementById('element');

if (container.contains(element)) {
  console.log('Element is inside container');
}

// Vérifier si un élément est dans le document
if (document.body.contains(element)) {
  console.log('Element is in the DOM');
}

// isConnected - propriété moderne
if (element.isConnected) {
  console.log('Element is connected to the DOM');
}
```

---

### Q23: Que retourne `querySelector()` si plusieurs éléments correspondent au sélecteur ?

**Réponse :**

**Seulement le PREMIER élément** :

```javascript
// HTML :
/*
<div class="box">Box 1</div>
<div class="box">Box 2</div>
<div class="box">Box 3</div>
*/

// querySelector retourne le PREMIER
const firstBox = document.querySelector('.box');
console.log(firstBox.textContent);  // "Box 1"

// Toujours le même, peu importe combien d'appels
const stillFirst = document.querySelector('.box');
console.log(stillFirst.textContent);  // "Box 1"

// Pour avoir tous les éléments, utiliser querySelectorAll
const allBoxes = document.querySelectorAll('.box');
console.log(allBoxes.length);  // 3

allBoxes.forEach((box, index) => {
  console.log(`Box ${index + 1}: ${box.textContent}`);
});
// Box 1: Box 1
// Box 2: Box 2
// Box 3: Box 3
```

**Ordre de sélection :**

```javascript
// L'ordre suit l'ordre d'apparition dans le HTML (document order)
// HTML :
/*
<div id="parent">
  <div class="item">Item 1</div>
  <div>
    <div class="item">Item 2 (nested)</div>
  </div>
  <div class="item">Item 3</div>
</div>
*/

const first = document.querySelector('.item');
console.log(first.textContent);  // "Item 1" (premier dans l'ordre du document)

// Même avec imbrication, l'ordre du document compte
```

---

### Q24: Comment insérer du HTML de manière sécurisée ?

**Réponse :**

**Plusieurs méthodes selon le contexte :**

```javascript
const userInput = '<script>alert("XSS")</script><p>Valid content</p>';

// ❌ DANGEREUX : innerHTML avec contenu utilisateur
element.innerHTML = userInput;  // Risque XSS

// ✅ Option 1 : textContent (échappe tout)
element.textContent = userInput;
// Affiche littéralement : <script>alert("XSS")</script><p>Valid content</p>

// ✅ Option 2 : createElement + textContent
function createSafeElement(html) {
  const div = document.createElement('div');
  div.textContent = html;  // Échappe automatiquement
  return div;
}

// ✅ Option 3 : DOMParser (si vous devez parser du HTML)
function sanitizeHTML(html) {
  const parser = new DOMParser();
  const doc = parser.parseFromString(html, 'text/html');
  
  // Supprimer les scripts et autres éléments dangereux
  doc.querySelectorAll('script, iframe, object, embed').forEach(el => el.remove());
  
  return doc.body.innerHTML;
}

const safe = sanitizeHTML(userInput);
element.innerHTML = safe;  // Plus sûr

// ✅ Option 4 : Bibliothèque de sanitization (DOMPurify)
// npm install dompurify
import DOMPurify from 'dompurify';

const clean = DOMPurify.sanitize(userInput);
element.innerHTML = clean;  // Sûr

// ✅ Option 5 : insertAdjacentText (n'interprète pas le HTML)
element.insertAdjacentText('beforeend', userInput);
// Insère comme texte, pas comme HTML
```

**Règles de sécurité :**
1. **Jamais** `innerHTML` avec du contenu utilisateur non filtré
2. Préférer `textContent` quand possible
3. Utiliser une bibliothèque de sanitization (DOMPurify) si vous devez autoriser du HTML
4. Valider et filtrer côté serveur aussi (défense en profondeur)

---

### Q25: Quelle est la différence entre `element.dataset.myValue` et `element.getAttribute('data-my-value')` ?

**Réponse :**

**Syntaxe et format :**

```javascript
// HTML :
/*
<div id="product" 
     data-product-id="123" 
     data-price="99.99"
     data-in-stock="true">
</div>
*/

const product = document.getElementById('product');

// getAttribute - utilise le nom exact de l'attribut (kebab-case)
console.log(product.getAttribute('data-product-id'));  // "123"
console.log(product.getAttribute('data-price'));       // "99.99"
console.log(product.getAttribute('data-in-stock'));    // "true"

// dataset - utilise camelCase
console.log(product.dataset.productId);  // "123"
console.log(product.dataset.price);      // "99.99"
console.log(product.dataset.inStock);    // "true"

// ⚠️ Conversion automatique : kebab-case → camelCase
// data-product-id → productId
// data-in-stock → inStock
```

**Définir des valeurs :**

```javascript
// Avec setAttribute
product.setAttribute('data-discount', '10');
console.log(product.getAttribute('data-discount'));  // "10"

// Avec dataset (plus simple)
product.dataset.discount = '15';
console.log(product.dataset.discount);  // "15"
console.log(product.getAttribute('data-discount'));  // "15"

// Conversion automatique dans l'autre sens aussi
product.dataset.specialOffer = 'yes';
console.log(product.getAttribute('data-special-offer'));  // "yes"
// specialOffer → data-special-offer
```

**Supprimer :**

```javascript
// Avec removeAttribute
product.removeAttribute('data-discount');

// Avec dataset (delete)
delete product.dataset.discount;
```

**Quelle méthode utiliser ?**

```javascript
// ✅ dataset - plus pratique et lisible (recommandé)
product.dataset.category = 'electronics';
if (product.dataset.inStock === 'true') {
  console.log('Available');
}

// ✅ getAttribute - si vous avez besoin du nom exact de l'attribut
const attrName = 'data-product-id';
console.log(product.getAttribute(attrName));

// Itérer sur tous les data attributes
for (const [key, value] of Object.entries(product.dataset)) {
  console.log(`${key}: ${value}`);
}
// productId: 123
// price: 99.99
// inStock: true
// category: electronics
```

**Points importants :**
- `dataset` est un objet **DOMStringMap** (pas un objet plain)
- Toutes les valeurs sont des **strings** (pas de conversion automatique)
- Conversion automatique kebab-case ↔ camelCase

---