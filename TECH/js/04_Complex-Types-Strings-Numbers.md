## [**..**](./00_index.md)

# Chapitre 4 : Types Complexes - Chaînes et Nombres

## 4.1 String Methods

Les strings en JavaScript sont **immutables** : les méthodes retournent une nouvelle string sans modifier l'originale.

### Manipulation de Strings

```javascript
const text = "Hello World";

// slice(start, end) : extrait une portion (end non inclus)
console.log(text.slice(0, 5));     // "Hello"
console.log(text.slice(6));        // "World" (du début jusqu'à la fin)
console.log(text.slice(-5));       // "World" (5 caractères depuis la fin)
console.log(text.slice(6, -1));    // "Worl" (de 6 à avant-dernier)

// substring(start, end) : similaire à slice mais pas d'index négatifs
console.log(text.substring(0, 5)); // "Hello"
console.log(text.substring(6));    // "World"
console.log(text.substring(-5));   // "Hello World" ⚠️ (négatif = 0)

// substr(start, length) : ⚠️ Déprécié, utiliser slice à la place
console.log(text.substr(0, 5));    // "Hello"
console.log(text.substr(6, 5));    // "World"

// replace(searchValue, replaceValue) : remplace la PREMIÈRE occurrence
const message = "Hello World, Hello Universe";
console.log(message.replace("Hello", "Hi"));  // "Hi World, Hello Universe"

// replaceAll(searchValue, replaceValue) : remplace TOUTES les occurrences (ES2021)
console.log(message.replaceAll("Hello", "Hi"));  // "Hi World, Hi Universe"

// Avant replaceAll : utiliser regex avec flag g (global)
console.log(message.replace(/Hello/g, "Hi"));  // "Hi World, Hi Universe"

// replace avec fonction callback
const str = "I have 2 apples and 3 oranges";
const result = str.replace(/\d+/g, function(match) {
    return parseInt(match) * 2;
});
console.log(result);  // "I have 4 apples and 6 oranges"
```

### Changement de Casse

```javascript
const text2 = "JavaScript Programming";

// toLowerCase() : convertit en minuscules
console.log(text2.toLowerCase());  // "javascript programming"

// toUpperCase() : convertit en majuscules
console.log(text2.toUpperCase());  // "JAVASCRIPT PROGRAMMING"

// Original inchangé (strings sont immutables)
console.log(text2);  // "JavaScript Programming"

// Cas d'usage : comparaison insensible à la casse
const email1 = "Alice@Example.COM";
const email2 = "alice@example.com";

console.log(email1 === email2);                           // false
console.log(email1.toLowerCase() === email2.toLowerCase()); // true

// Capitaliser première lettre
function capitalize(str) {
    return str.charAt(0).toUpperCase() + str.slice(1).toLowerCase();
}

console.log(capitalize("hello"));     // "Hello"
console.log(capitalize("WORLD"));     // "World"
console.log(capitalize("jAvAsCrIpT"));// "Javascript"
```

### Recherche dans les Strings

```javascript
const sentence = "The quick brown fox jumps over the lazy dog";

// indexOf(searchValue, fromIndex) : retourne index de la première occurrence (-1 si absent)
console.log(sentence.indexOf("quick"));      // 4
console.log(sentence.indexOf("fox"));        // 16
console.log(sentence.indexOf("cat"));        // -1 (pas trouvé)
console.log(sentence.indexOf("the"));        // 31 ("the" en minuscule)
console.log(sentence.indexOf("o"));          // 12 (premier 'o')
console.log(sentence.indexOf("o", 13));      // 17 (cherche à partir de l'index 13)

// lastIndexOf(searchValue) : retourne index de la DERNIÈRE occurrence
console.log(sentence.lastIndexOf("o"));      // 41 (dernier 'o')

// includes(searchValue) : retourne true/false (ES6)
console.log(sentence.includes("fox"));       // true
console.log(sentence.includes("cat"));       // false
console.log(sentence.includes("QUICK"));     // false (sensible à la casse)

// startsWith(searchValue) : vérifie si commence par (ES6)
console.log(sentence.startsWith("The"));     // true
console.log(sentence.startsWith("the"));     // false (sensible à la casse)
console.log(sentence.startsWith("quick", 4));// true (à partir de l'index 4)

// endsWith(searchValue) : vérifie si termine par (ES6)
console.log(sentence.endsWith("dog"));       // true
console.log(sentence.endsWith("cat"));       // false
console.log(sentence.endsWith("lazy", 43));  // true (dans les 43 premiers caractères)

// Cas d'usage pratique
function isValidEmail(email) {
    return email.includes("@") && 
           email.includes(".") && 
           !email.startsWith("@") && 
           !email.endsWith("@");
}

console.log(isValidEmail("test@example.com"));  // true
console.log(isValidEmail("invalid.com"));       // false
console.log(isValidEmail("@invalid.com"));      // false
```

### Découpage et Nettoyage

```javascript
// split(separator, limit) : convertit string en array
const csv = "apple,banana,orange,grape";
console.log(csv.split(","));  // ["apple", "banana", "orange", "grape"]

const sentence2 = "Hello World from JavaScript";
console.log(sentence2.split(" "));  // ["Hello", "World", "from", "JavaScript"]

// Limiter le nombre d'éléments
console.log(sentence2.split(" ", 2));  // ["Hello", "World"]

// Split caractère par caractère
console.log("Hello".split(""));  // ["H", "e", "l", "l", "o"]

// trim() : supprime espaces au début et à la fin
const messy = "   Hello World   ";
console.log(messy.trim());        // "Hello World"
console.log(messy);               // "   Hello World   " (original inchangé)

// trimStart() / trimLeft() : supprime espaces au début (ES2019)
console.log(messy.trimStart());   // "Hello World   "

// trimEnd() / trimRight() : supprime espaces à la fin (ES2019)
console.log(messy.trimEnd());     // "   Hello World"

// Cas d'usage : nettoyer input utilisateur
function processInput(input) {
    return input.trim().toLowerCase();
}

console.log(processInput("  HELLO  "));  // "hello"

// Validation de formulaire
const username = "  alice  ";
const cleanUsername = username.trim();

if (cleanUsername.length >= 3) {
    console.log("Valid username");
}
```

### Autres Méthodes de String

```javascript
const text3 = "JavaScript";

// charAt(index) : retourne le caractère à l'index donné
console.log(text3.charAt(0));     // "J"
console.log(text3.charAt(4));     // "S"
console.log(text3.charAt(100));   // "" (vide si index invalide)

// Accès avec bracket notation (plus moderne)
console.log(text3[0]);            // "J"
console.log(text3[4]);            // "S"
console.log(text3[100]);          // undefined

// charCodeAt(index) : retourne le code Unicode (ASCII)
console.log(text3.charCodeAt(0)); // 74 (code de 'J')
console.log("A".charCodeAt(0));   // 65
console.log("a".charCodeAt(0));   // 97

// concat(string2, string3, ...) : concatène des strings
console.log("Hello".concat(" ", "World"));  // "Hello World"
console.log("Hello".concat(" ", "World", "!"));  // "Hello World!"

// Préférer l'opérateur + ou template literals
console.log("Hello" + " " + "World");  // "Hello World"
console.log(`Hello ${"World"}`);       // "Hello World"

// repeat(count) : répète la string n fois (ES6)
console.log("Ha".repeat(3));          // "HaHaHa"
console.log("*".repeat(10));          // "**********"
console.log("-".repeat(50));          // 50 tirets

// padStart(targetLength, padString) : remplit au début (ES2017)
const num = "5";
console.log(num.padStart(3, "0"));    // "005"
console.log(num.padStart(5, "0"));    // "00005"

// padEnd(targetLength, padString) : remplit à la fin (ES2017)
const price = "9.5";
console.log(price.padEnd(5, "0"));    // "9.500"

// Cas d'usage : formater des nombres
function formatPrice(price) {
    return "$" + price.toFixed(2);
}

console.log(formatPrice(9.5));        // "$9.50"

// Formater des codes
function formatCode(code) {
    return code.toString().padStart(6, "0");
}

console.log(formatCode(42));          // "000042"
console.log(formatCode(1234));        // "001234"
```

### Template Literals (Backticks)

```javascript
// Template literals : strings avec backticks ` ` (ES6)
const name = "Alice";
const age = 25;

// Interpolation de variables avec ${}
const greeting = `Hello, my name is ${name} and I am ${age} years old.`;
console.log(greeting);  // "Hello, my name is Alice and I am 25 years old."

// Expressions dans les template literals
const a = 10;
const b = 20;
console.log(`The sum is ${a + b}`);  // "The sum is 30"

// Appels de fonctions
function getTime() {
    return new Date().toLocaleTimeString();
}

console.log(`Current time: ${getTime()}`);

// Multilignes (préserve les retours à la ligne)
const multiline = `
    Line 1
    Line 2
    Line 3
`;
console.log(multiline);

// HTML templates
const html = `
    <div class="card">
        <h2>${name}</h2>
        <p>Age: ${age}</p>
    </div>
`;

// Échappement de backticks
const text4 = `This is a backtick: \` `;
console.log(text4);  // "This is a backtick: ` "

// Comparaison avec concaténation classique
// ❌ Difficile à lire
const msg1 = "Hello " + name + ", you are " + age + " years old.";

// ✅ Plus lisible
const msg2 = `Hello ${name}, you are ${age} years old.`;

// Tagged templates (avancé)
function highlight(strings, ...values) {
    return strings.reduce((result, str, i) => {
        return result + str + (values[i] ? `<strong>${values[i]}</strong>` : "");
    }, "");
}

const product = "Laptop";
const price2 = 999;
const tagged = highlight`Product: ${product}, Price: $${price2}`;
console.log(tagged);  // "Product: <strong>Laptop</strong>, Price: $<strong>999</strong>"
```

---

## 4.2 Number Methods et Math Object

### Number Methods

```javascript
// Conversion en Number
const str = "123.45";

// parseInt(string, radix) : convertit en entier
console.log(parseInt(str));           // 123
console.log(parseInt("100px"));       // 100 (s'arrête au premier non-digit)
console.log(parseInt("abc"));         // NaN

// Radix : base numérique (2=binaire, 8=octal, 10=décimal, 16=hexadécimal)
console.log(parseInt("1010", 2));     // 10 (binaire vers décimal)
console.log(parseInt("FF", 16));      // 255 (hexadécimal vers décimal)
console.log(parseInt("77", 8));       // 63 (octal vers décimal)

// ⚠️ Toujours spécifier le radix pour éviter les surprises
console.log(parseInt("010"));         // 10 (en mode strict)
console.log(parseInt("010", 10));     // 10 ✅
console.log(parseInt("010", 8));      // 8 (octal)

// parseFloat(string) : convertit en nombre décimal
console.log(parseFloat(str));         // 123.45
console.log(parseFloat("3.14"));      // 3.14
console.log(parseFloat("3.14abc"));   // 3.14
console.log(parseFloat("abc"));       // NaN

// Number(value) : conversion plus stricte
console.log(Number("123"));           // 123
console.log(Number("123.45"));        // 123.45
console.log(Number("123px"));         // NaN ⚠️ (plus strict que parseInt)
console.log(Number(true));            // 1
console.log(Number(false));           // 0
console.log(Number(null));            // 0
console.log(Number(undefined));       // NaN

// toFixed(digits) : formate avec n décimales (retourne string!)
const num = 3.14159;
console.log(num.toFixed(2));          // "3.14" (string)
console.log(num.toFixed(4));          // "3.1416" (arrondi)
console.log(num.toFixed(0));          // "3"

// Convertir en number après toFixed
console.log(Number(num.toFixed(2)));  // 3.14 (number)
console.log(parseFloat(num.toFixed(2))); // 3.14 (number)

// toPrecision(precision) : formate avec n chiffres significatifs (retourne string)
const num2 = 123.456;
console.log(num2.toPrecision(4));     // "123.5" (4 chiffres significatifs)
console.log(num2.toPrecision(2));     // "1.2e+2" (notation scientifique)
console.log(num2.toPrecision(6));     // "123.456"

// toString(radix) : convertit en string avec base spécifiée
const num3 = 255;
console.log(num3.toString());         // "255"
console.log(num3.toString(2));        // "11111111" (binaire)
console.log(num3.toString(16));       // "ff" (hexadécimal)
console.log(num3.toString(8));        // "377" (octal)

// Vérifications
console.log(isNaN(NaN));              // true
console.log(isNaN("hello"));          // true ⚠️ (coercion)
console.log(isNaN(123));              // false

// Number.isNaN() : plus strict (ES6) - RECOMMANDÉ
console.log(Number.isNaN(NaN));       // true
console.log(Number.isNaN("hello"));   // false ✅ (pas de coercion)
console.log(Number.isNaN(123));       // false

// isFinite() : vérifie si nombre fini
console.log(isFinite(123));           // true
console.log(isFinite(Infinity));      // false
console.log(isFinite(-Infinity));     // false
console.log(isFinite(NaN));           // false

// Number.isFinite() : plus strict (ES6)
console.log(Number.isFinite(123));    // true
console.log(Number.isFinite("123"));  // false ✅ (pas de coercion)

// Number.isInteger() : vérifie si entier (ES6)
console.log(Number.isInteger(123));   // true
console.log(Number.isInteger(123.0)); // true (123.0 = 123)
console.log(Number.isInteger(123.45));// false
console.log(Number.isInteger("123")); // false

// Exemple pratique : validation de prix
function validatePrice(input) {
    const price = parseFloat(input);
    
    if (isNaN(price)) {
        return "Invalid price";
    }
    
    if (price < 0) {
        return "Price cannot be negative";
    }
    
    return parseFloat(price.toFixed(2));
}

console.log(validatePrice("9.99"));   // 9.99
console.log(validatePrice("9.999"));  // 10
console.log(validatePrice("abc"));    // "Invalid price"
console.log(validatePrice("-5"));     // "Price cannot be negative"
```

### Math Object

Le **Math object** contient des constantes et méthodes mathématiques.

```javascript
// Constantes mathématiques
console.log(Math.PI);        // 3.141592653589793
console.log(Math.E);         // 2.718281828459045 (nombre d'Euler)
console.log(Math.LN2);       // 0.6931471805599453 (logarithme naturel de 2)
console.log(Math.LN10);      // 2.302585092994046
console.log(Math.SQRT2);     // 1.4142135623730951 (racine carrée de 2)

// Arrondissage
const num4 = 4.7;
const num5 = 4.3;

// Math.round() : arrondit au plus proche entier
console.log(Math.round(num4));     // 5
console.log(Math.round(num5));     // 4
console.log(Math.round(4.5));      // 5
console.log(Math.round(-4.5));     // -4 ⚠️

// Math.ceil() : arrondit vers le haut (plafond)
console.log(Math.ceil(num4));      // 5
console.log(Math.ceil(num5));      // 5
console.log(Math.ceil(4.01));      // 5
console.log(Math.ceil(-4.7));      // -4 (vers le haut = moins négatif)

// Math.floor() : arrondit vers le bas (plancher)
console.log(Math.floor(num4));     // 4
console.log(Math.floor(num5));     // 4
console.log(Math.floor(4.99));     // 4
console.log(Math.floor(-4.3));     // -5 (vers le bas = plus négatif)

// Math.trunc() : supprime la partie décimale (ES6)
console.log(Math.trunc(4.9));      // 4
console.log(Math.trunc(-4.9));     // -4
console.log(Math.trunc(4.01));     // 4

// Maximum et Minimum
console.log(Math.max(1, 5, 3, 9, 2));   // 9
console.log(Math.min(1, 5, 3, 9, 2));   // 1

// Avec array (utiliser spread operator)
const numbers = [1, 5, 3, 9, 2];
console.log(Math.max(...numbers));      // 9
console.log(Math.min(...numbers));      // 1

// Valeur absolue
console.log(Math.abs(-5));         // 5
console.log(Math.abs(5));          // 5
console.log(Math.abs(-3.14));      // 3.14

// Puissance et racine
console.log(Math.pow(2, 3));       // 8 (2^3)
console.log(Math.pow(5, 2));       // 25 (5^2)
console.log(2 ** 3);               // 8 (opérateur ** équivalent)

console.log(Math.sqrt(9));         // 3 (racine carrée)
console.log(Math.sqrt(2));         // 1.4142135623730951
console.log(Math.sqrt(16));        // 4

console.log(Math.cbrt(8));         // 2 (racine cubique, ES6)
console.log(Math.cbrt(27));        // 3

// Nombres aléatoires
// Math.random() : retourne un nombre entre 0 (inclus) et 1 (exclus)
console.log(Math.random());        // 0.xxx

// Nombre aléatoire entre 0 et 10
console.log(Math.random() * 10);

// Nombre aléatoire entier entre 0 et 9
console.log(Math.floor(Math.random() * 10));

// Nombre aléatoire entre min et max (inclus)
function getRandomInt(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

console.log(getRandomInt(1, 6));   // Simule un dé (1-6)
console.log(getRandomInt(1, 100)); // Nombre entre 1 et 100

// Signe
console.log(Math.sign(5));         // 1 (positif)
console.log(Math.sign(-5));        // -1 (négatif)
console.log(Math.sign(0));         // 0
console.log(Math.sign(-0));        // -0

// Trigonométrie (angles en radians)
console.log(Math.sin(Math.PI / 2));     // 1
console.log(Math.cos(0));               // 1
console.log(Math.tan(Math.PI / 4));     // 0.9999999999999999 (≈1)

// Convertir degrés en radians
function toRadians(degrees) {
    return degrees * (Math.PI / 180);
}

console.log(Math.sin(toRadians(90)));   // 1

// Logarithmes
console.log(Math.log(Math.E));     // 1 (logarithme naturel)
console.log(Math.log10(100));      // 2 (log base 10)
console.log(Math.log2(8));         // 3 (log base 2)

// Exemples pratiques
// 1. Calculer la distance entre deux points
function distance(x1, y1, x2, y2) {
    const dx = x2 - x1;
    const dy = y2 - y1;
    return Math.sqrt(dx * dx + dy * dy);
}

console.log(distance(0, 0, 3, 4));  // 5 (triangle 3-4-5)

// 2. Calculer le pourcentage
function calculatePercentage(value, total) {
    return Math.round((value / total) * 100);
}

console.log(calculatePercentage(15, 60));  // 25 (15 sur 60 = 25%)

// 3. Arrondir à n décimales
function roundToDecimals(num, decimals) {
    const factor = Math.pow(10, decimals);
    return Math.round(num * factor) / factor;
}

console.log(roundToDecimals(3.14159, 2));  // 3.14
console.log(roundToDecimals(3.14159, 4));  // 3.1416

// 4. Générer une couleur aléatoire (hex)
function getRandomColor() {
    const r = Math.floor(Math.random() * 256);
    const g = Math.floor(Math.random() * 256);
    const b = Math.floor(Math.random() * 256);
    return `#${r.toString(16).padStart(2, '0')}${g.toString(16).padStart(2, '0')}${b.toString(16).padStart(2, '0')}`;
}

console.log(getRandomColor());  // #a3f5c2 (exemple)

// 5. Limiter un nombre dans un intervalle (clamp)
function clamp(value, min, max) {
    return Math.min(Math.max(value, min), max);
}

console.log(clamp(15, 0, 10));   // 10 (limité au max)
console.log(clamp(-5, 0, 10));   // 0 (limité au min)
console.log(clamp(5, 0, 10));    // 5 (dans l'intervalle)
```

---

**Résumé du Chapitre 4 :**

✅ **Strings sont immutables** : les méthodes retournent de nouvelles strings  
✅ **Manipulation** : `slice()`, `substring()`, `replace()`, `replaceAll()`  
✅ **Casse** : `toLowerCase()`, `toUpperCase()`  
✅ **Recherche** : `indexOf()`, `includes()`, `startsWith()`, `endsWith()`  
✅ **Découpage** : `split()`, `trim()`, `trimStart()`, `trimEnd()`  
✅ **Template literals** : backticks avec interpolation `${}`  
✅ **Number conversion** : `parseInt()`, `parseFloat()`, `Number()`  
✅ **Number formatting** : `toFixed()`, `toPrecision()`, `toString()`  
✅ **Math object** : `round()`, `ceil()`, `floor()`, `max()`, `min()`, `random()`, `pow()`, `sqrt()`  

---
# Questions/Réponses Orales - Chapitre 4 : Strings et Numbers

## Questions sur String Methods

### Q1 : Quelle est la différence entre `slice()`, `substring()` et `substr()` ?

**Réponse :**

Les trois méthodes extraient une portion de string, mais avec des différences importantes :

- **`slice(start, end)`** : accepte les indices négatifs, `end` non inclus
- **`substring(start, end)`** : pas d'indices négatifs (négatif = 0), `end` non inclus
- **`substr(start, length)`** : ⚠️ **Déprécié**, utilise une longueur au lieu d'un index de fin

```javascript
const text = "JavaScript";

// slice() : le plus flexible (RECOMMANDÉ)
console.log(text.slice(0, 4));      // "Java"
console.log(text.slice(4));         // "Script" (jusqu'à la fin)
console.log(text.slice(-6));        // "Script" (6 caractères depuis la fin)
console.log(text.slice(4, -2));     // "Scri" (de 4 à avant-dernier -2)
console.log(text.slice(-6, -2));    // "Scri" (négatifs des deux côtés)

// substring() : pas d'indices négatifs
console.log(text.substring(0, 4));  // "Java"
console.log(text.substring(4));     // "Script"
console.log(text.substring(-6));    // "JavaScript" ⚠️ (-6 devient 0)
console.log(text.substring(4, -2)); // "Java" ⚠️ (-2 devient 0, puis swap)

// substring inverse automatiquement si start > end
console.log(text.substring(4, 0));  // "Java" (équivalent à substring(0, 4))

// substr() : déprécié, à éviter
console.log(text.substr(0, 4));     // "Java" (start=0, length=4)
console.log(text.substr(4, 6));     // "Script" (start=4, length=6)
console.log(text.substr(-6, 6));    // "Script" (6 derniers caractères)

// Exemple pratique : extraire extension de fichier
const filename = "document.pdf";

// ✅ Avec slice et lastIndexOf
const extension = filename.slice(filename.lastIndexOf(".") + 1);
console.log(extension);  // "pdf"

// ✅ Avec substring
const extension2 = filename.substring(filename.lastIndexOf(".") + 1);
console.log(extension2);  // "pdf"

// Extraire nom sans extension
const nameWithoutExt = filename.slice(0, filename.lastIndexOf("."));
console.log(nameWithoutExt);  // "document"
```

**Recommandation :** Utiliser **`slice()`** car c'est le plus flexible et cohérent.

---

### Q2 : Quelle est la différence entre `replace()` et `replaceAll()` ?

**Réponse :**

**`replace()`** remplace seulement la **première occurrence**, tandis que **`replaceAll()`** (ES2021) remplace **toutes les occurrences**.

```javascript
const text = "Hello World, Hello Universe, Hello JavaScript";

// replace() : seulement la PREMIÈRE occurrence
console.log(text.replace("Hello", "Hi"));
// "Hi World, Hello Universe, Hello JavaScript"

// replaceAll() : TOUTES les occurrences (ES2021)
console.log(text.replaceAll("Hello", "Hi"));
// "Hi World, Hi Universe, Hi JavaScript"

// Avant replaceAll : utiliser regex avec flag 'g' (global)
console.log(text.replace(/Hello/g, "Hi"));
// "Hi World, Hi Universe, Hi JavaScript"

// replace avec regex sans 'g' : seulement première occurrence
console.log(text.replace(/Hello/, "Hi"));
// "Hi World, Hello Universe, Hello JavaScript"

// Case insensitive avec flag 'i'
const text2 = "HELLO hello HeLLo";
console.log(text2.replace(/hello/gi, "Hi"));
// "Hi Hi Hi"

// replace avec fonction callback
const prices = "Item1: $10, Item2: $20, Item3: $30";

const updated = prices.replace(/\$(\d+)/g, function(match, number) {
    return "$" + (parseInt(number) * 1.1).toFixed(2);
});

console.log(updated);
// "Item1: $11.00, Item2: $22.00, Item3: $33.00"

// Avec arrow function (plus concis)
const updated2 = prices.replace(/\$(\d+)/g, (match, num) => {
    return "$" + (parseInt(num) * 1.1).toFixed(2);
});

// Exemple pratique : censurer des mots
function censorWords(text, badWords) {
    let result = text;
    badWords.forEach(word => {
        const regex = new RegExp(word, 'gi');
        result = result.replace(regex, '*'.repeat(word.length));
    });
    return result;
}

console.log(censorWords("This is bad and very bad", ["bad"]));
// "This is *** and very ***"

// Remplacer patterns complexes
const email = "Contact us at info@example.com or support@example.com";

const masked = email.replace(/([a-z]+)@/g, "***@");
console.log(masked);
// "Contact us at ***@example.com or ***@example.com"
```

**Compatibilité :** `replaceAll()` n'est pas supporté par les vieux navigateurs. Pour la compatibilité, utiliser `replace()` avec regex et flag `g`.

---

### Q3 : Quelle est la différence entre `indexOf()` et `includes()` ?

**Réponse :**

**`indexOf()`** retourne l'**index** de la première occurrence (ou -1 si absent), tandis que **`includes()`** retourne un **boolean** (true/false).

```javascript
const text = "JavaScript is awesome";

// indexOf() : retourne l'index (number)
console.log(text.indexOf("JavaScript"));  // 0 (commence à l'index 0)
console.log(text.indexOf("is"));          // 11
console.log(text.indexOf("Python"));      // -1 (pas trouvé)
console.log(text.indexOf("awesome"));     // 14

// includes() : retourne boolean (ES6)
console.log(text.includes("JavaScript")); // true
console.log(text.includes("is"));         // true
console.log(text.includes("Python"));     // false
console.log(text.includes("awesome"));    // true

// Les deux sont sensibles à la casse
console.log(text.indexOf("javascript"));  // -1 (minuscule)
console.log(text.includes("JAVASCRIPT")); // false (majuscule)

// indexOf() avec fromIndex (2ème paramètre)
const repeated = "hello world, hello universe";
console.log(repeated.indexOf("hello"));      // 0 (première occurrence)
console.log(repeated.indexOf("hello", 1));   // 13 (cherche après index 1)
console.log(repeated.indexOf("hello", 14));  // -1 (pas trouvé après 14)

// includes() avec fromIndex
console.log(repeated.includes("hello"));     // true
console.log(repeated.includes("hello", 14)); // false (après index 14)

// Quand utiliser quoi ?

// ✅ includes() : vérifier si présent (plus lisible)
if (text.includes("JavaScript")) {
    console.log("C'est du JavaScript!");
}

// ✅ indexOf() : besoin de la position
const position = text.indexOf("awesome");
if (position !== -1) {
    console.log(`Found at position ${position}`);
}

// ⚠️ Piège avec indexOf() dans les conditions
// ❌ Mauvais
if (text.indexOf("JavaScript")) {  // 0 = falsy!
    console.log("Never executed!");  // Ne s'exécute pas
}

// ✅ Bon
if (text.indexOf("JavaScript") !== -1) {
    console.log("Found!");
}

// ✅ Ou utiliser includes (plus simple)
if (text.includes("JavaScript")) {
    console.log("Found!");
}

// Exemple pratique : validation d'email
function isValidEmail(email) {
    // Version avec includes (plus lisible)
    return email.includes("@") && 
           email.includes(".") && 
           email.indexOf("@") < email.lastIndexOf(".");
}

console.log(isValidEmail("test@example.com"));  // true
console.log(isValidEmail("invalid.com"));       // false
console.log(isValidEmail("test@com"));          // false

// Exemple : filtrer un array
const words = ["apple", "banana", "grape", "orange", "pineapple"];

// Trouver tous les mots contenant "apple"
const withApple = words.filter(word => word.includes("apple"));
console.log(withApple);  // ["apple", "pineapple"]

// Trouver la position du premier mot contenant "an"
const index = words.findIndex(word => word.includes("an"));
console.log(index);  // 1 (banana)
```

**Résumé :**
- **`indexOf()`** → retourne **index** (number), -1 si absent
- **`includes()`** → retourne **boolean**, plus lisible pour les vérifications

---

### Q4 : Comment fonctionnent `trim()`, `trimStart()` et `trimEnd()` ?

**Réponse :**

Ces méthodes suppriment les **espaces blancs** (espaces, tabs, retours à la ligne) :

- **`trim()`** : supprime au début ET à la fin
- **`trimStart()` / `trimLeft()`** : supprime seulement au début (ES2019)
- **`trimEnd()` / `trimRight()`** : supprime seulement à la fin (ES2019)

```javascript
const messy = "   Hello World   ";

// trim() : supprime des deux côtés
console.log(messy.trim());           // "Hello World"
console.log(messy.trim().length);    // 11

// trimStart() / trimLeft() : seulement au début (ES2019)
console.log(messy.trimStart());      // "Hello World   "
console.log(messy.trimLeft());       // "Hello World   " (alias)

// trimEnd() / trimRight() : seulement à la fin (ES2019)
console.log(messy.trimEnd());        // "   Hello World"
console.log(messy.trimRight());      // "   Hello World" (alias)

// Original inchangé (strings sont immutables)
console.log(messy);                  // "   Hello World   "

// Espaces multiples
const spaces = "   \t\n  Hello  \n\t  ";
console.log(spaces.trim());          // "Hello"

// trim() ne supprime PAS les espaces internes
const internal = "  Hello   World  ";
console.log(internal.trim());        // "Hello   World" (espaces internes préservés)

// Pour supprimer les espaces internes : replace avec regex
console.log(internal.trim().replace(/\s+/g, " "));  // "Hello World"

// Cas d'usage pratique : nettoyer input utilisateur
function cleanInput(input) {
    return input.trim();
}

const userInput = "  alice@example.com  ";
console.log(cleanInput(userInput));  // "alice@example.com"

// Validation de formulaire
function validateUsername(username) {
    const cleaned = username.trim();
    
    if (cleaned.length === 0) {
        return "Username cannot be empty";
    }
    
    if (cleaned.length < 3) {
        return "Username must be at least 3 characters";
    }
    
    return cleaned;
}

console.log(validateUsername("  alice  "));  // "alice"
console.log(validateUsername("  a  "));      // "Username must be at least 3 characters"
console.log(validateUsername("   "));        // "Username cannot be empty"

// Nettoyer plusieurs lignes
const multiline = `
    Line 1
    Line 2
    Line 3
`;

const lines = multiline.split("\n").map(line => line.trim()).filter(line => line);
console.log(lines);  // ["Line 1", "Line 2", "Line 3"]

// Exemple : parser CSV
const csv = "  apple  ,  banana  ,  orange  ";
const fruits = csv.split(",").map(fruit => fruit.trim());
console.log(fruits);  // ["apple", "banana", "orange"]

// trimStart pour aligner du texte
const items = [
    "   Item 1",
    "     Item 2",
    "  Item 3"
];

const aligned = items.map(item => item.trimStart());
console.log(aligned);
// ["Item 1", "Item 2", "Item 3"]

// Attention : trim ne supprime pas TOUS les caractères blancs custom
const custom = "___Hello___";
console.log(custom.trim());  // "___Hello___" (underscore n'est pas un espace)

// Pour supprimer des caractères custom : replace
console.log(custom.replace(/^_+|_+$/g, ""));  // "Hello"
```

**Cas d'usage principaux :**
- Nettoyer input utilisateur
- Parser CSV/données
- Validation de formulaires
- Normaliser du texte

---

### Q5 : Expliquez `split()` et `join()` - comment sont-ils liés ?

**Réponse :**

**`split()`** convertit une **string en array**, tandis que **`join()`** (méthode d'array) fait l'inverse : convertit un **array en string**. Ils sont complémentaires.

```javascript
// split(separator, limit) : string → array
const sentence = "JavaScript is awesome";

// Split par espace
const words = sentence.split(" ");
console.log(words);  // ["JavaScript", "is", "awesome"]

// join(separator) : array → string
const joined = words.join(" ");
console.log(joined);  // "JavaScript is awesome"

// Différents séparateurs
const csv = "apple,banana,orange";
console.log(csv.split(","));        // ["apple", "banana", "orange"]
console.log(csv.split(",").join("|"));  // "apple|banana|orange"

// Split caractère par caractère (empty string)
const word = "Hello";
console.log(word.split(""));        // ["H", "e", "l", "l", "o"]
console.log(word.split("").reverse().join(""));  // "olleH" (inverser)

// Limiter le nombre d'éléments avec split
const text = "one two three four five";
console.log(text.split(" ", 3));    // ["one", "two", "three"]

// split sans paramètre : retourne array avec string complète
console.log(text.split());          // ["one two three four five"]

// join sans paramètre : utilise virgule par défaut
const arr = ["a", "b", "c"];
console.log(arr.join());            // "a,b,c"
console.log(arr.join(""));          // "abc" (sans séparateur)
console.log(arr.join(" - "));       // "a - b - c"

// Cas d'usage pratique : formater des noms
const fullName = "john doe smith";

const formatted = fullName
    .split(" ")
    .map(word => word.charAt(0).toUpperCase() + word.slice(1))
    .join(" ");

console.log(formatted);  // "John Doe Smith"

// Parser CSV
const csvData = "apple,5.99\nbanana,3.50\norange,4.25";

const products = csvData.split("\n").map(line => {
    const [name, price] = line.split(",");
    return { name, price: parseFloat(price) };
});

console.log(products);
// [
//   { name: "apple", price: 5.99 },
//   { name: "banana", price: 3.5 },
//   { name: "orange", price: 4.25 }
// ]

// Créer des initiales
function getInitials(fullName) {
    return fullName
        .split(" ")
        .map(name => name.charAt(0).toUpperCase())
        .join("");
}

console.log(getInitials("john doe smith"));  // "JDS"

// Remplacer tous les espaces par des tirets (slug)
function createSlug(title) {
    return title
        .toLowerCase()
        .trim()
        .split(/\s+/)  // Regex : un ou plusieurs espaces
        .join("-");
}

console.log(createSlug("  Hello   World  JavaScript  "));
// "hello-world-javascript"

// Compter les mots
function countWords(text) {
    return text.trim().split(/\s+/).length;
}

console.log(countWords("Hello World JavaScript"));  // 3
console.log(countWords("  Hello   World  "));       // 2

// Inverser les mots dans une phrase
function reverseWords(sentence) {
    return sentence
        .split(" ")
        .reverse()
        .join(" ");
}

console.log(reverseWords("Hello World"));  // "World Hello"

// Split avec regex
const text2 = "apple123banana456orange";
console.log(text2.split(/\d+/));  // ["apple", "banana", "orange"]

// Join avec des éléments non-string (conversion automatique)
const mixed = [1, 2, 3, true, "hello"];
console.log(mixed.join(" - "));  // "1 - 2 - 3 - true - hello"

// Créer un chemin de fichier
const pathParts = ["home", "user", "documents", "file.txt"];
const path = pathParts.join("/");
console.log(path);  // "home/user/documents/file.txt"

// Nettoyer et normaliser du texte
function normalizeText(text) {
    return text
        .trim()
        .split(/\s+/)      // Split par espaces multiples
        .join(" ")         // Rejoindre avec un seul espace
        .toLowerCase();
}

console.log(normalizeText("  Hello    World   JavaScript  "));
// "hello world javascript"
```

**Résumé :**
- **`split(separator)`** : string → array
- **`join(separator)`** : array → string
- Très utiles ensemble pour manipuler du texte

---

## Questions sur Numbers et Math

### Q6 : Quelle est la différence entre `parseInt()`, `parseFloat()` et `Number()` ?

**Réponse :**

Les trois convertissent des valeurs en nombres, mais avec des comportements différents :

- **`parseInt(string, radix)`** : parse jusqu'au premier caractère non-numérique, retourne un **entier**
- **`parseFloat(string)`** : parse jusqu'au premier caractère invalide, retourne un **nombre décimal**
- **`Number(value)`** : conversion stricte, toute la string doit être valide

```javascript
const str1 = "123";
const str2 = "123.45";
const str3 = "123px";
const str4 = "abc123";

// parseInt() : s'arrête au premier non-digit, retourne entier
console.log(parseInt(str1));        // 123
console.log(parseInt(str2));        // 123 (ignore .45)
console.log(parseInt(str3));        // 123 (ignore "px")
console.log(parseInt(str4));        // NaN (commence par lettres)

// parseFloat() : parse les décimales, s'arrête au premier invalide
console.log(parseFloat(str1));      // 123
console.log(parseFloat(str2));      // 123.45 ✅
console.log(parseFloat(str3));      // 123 (ignore "px")
console.log(parseFloat(str4));      // NaN

// Number() : conversion stricte (toute la string doit être valide)
console.log(Number(str1));          // 123
console.log(Number(str2));          // 123.45
console.log(Number(str3));          // NaN ⚠️ (strict)
console.log(Number(str4));          // NaN

// parseInt avec radix (base numérique)
console.log(parseInt("10", 10));    // 10 (décimal)
console.log(parseInt("10", 2));     // 2 (binaire)
console.log(parseInt("10", 8));     // 8 (octal)
console.log(parseInt("10", 16));    // 16 (hexadécimal)

console.log(parseInt("FF", 16));    // 255
console.log(parseInt("1010", 2));   // 10

// ⚠️ Toujours spécifier le radix!
console.log(parseInt("08"));        // 8 (aujourd'hui)
console.log(parseInt("08", 10));    // 8 ✅ (explicite)

// Number() avec autres types
console.log(Number(true));          // 1
console.log(Number(false));         // 0
console.log(Number(null));          // 0
console.log(Number(undefined));     // NaN
console.log(Number(""));            // 0 ⚠️
console.log(Number("  "));          // 0 ⚠️

// parseInt et parseFloat avec espaces
console.log(parseInt("  123  "));   // 123 (ignore espaces)
console.log(parseFloat("  3.14  ")); // 3.14
console.log(Number("  123  "));     // 123

// Exemples pratiques
// Extraire nombre d'une string
const price = "$19.99";
console.log(parseFloat(price.slice(1)));  // 19.99

// Parser input utilisateur
function parseUserInput(input) {
    const num = parseFloat(input);
    
    if (isNaN(num)) {
        return null;
    }
    
    return num;
}

console.log(parseUserInput("42"));      // 42
console.log(parseUserInput("3.14"));    // 3.14
console.log(parseUserInput("abc"));     // null

// Convertir hex en decimal
function hexToDecimal(hex) {
    return parseInt(hex, 16);
}

console.log(hexToDecimal("FF"));        // 255
console.log(hexToDecimal("A5"));        // 165

// Unary plus operator (raccourci pour Number())
console.log(+"123");                    // 123
console.log(+"123.45");                 // 123.45
console.log(+"123px");                  // NaN
console.log(+true);                     // 1
console.log(+false);                    // 0

// Comparaison
const input = "100px";

console.log(parseInt(input));           // 100 (tolère "px")
console.log(parseFloat(input));         // 100 (tolère "px")
console.log(Number(input));             // NaN (strict)
console.log(+input);                    // NaN (strict, équivalent à Number())
```

**Résumé :**
- **`parseInt()`** : entiers, tolère caractères après, spécifier radix
- **`parseFloat()`** : décimales, tolère caractères après
- **`Number()` / `+`** : conversion stricte, toute la string doit être valide

---

### Q7 : Quelle est la différence entre `Math.round()`, `Math.ceil()`, `Math.floor()` et `Math.trunc()` ?

**Réponse :**

Ces quatre méthodes arrondissent les nombres, mais de manière différente :

- **`Math.round()`** : arrondit au plus proche entier (0.5 → vers le haut)
- **`Math.ceil()`** : arrondit **toujours vers le haut** (plafond)
- **`Math.floor()`** : arrondit **toujours vers le bas** (plancher)
- **`Math.trunc()`** : supprime simplement la partie décimale (ES6)

```javascript
const num1 = 4.3;
const num2 = 4.7;
const num3 = 4.5;

// Math.round() : au plus proche
console.log(Math.round(num1));   // 4 (plus proche de 4)
console.log(Math.round(num2));   // 5 (plus proche de 5)
console.log(Math.round(num3));   // 5 (0.5 → vers le haut)
console.log(Math.round(4.49));   // 4
console.log(Math.round(4.50));   // 5

// Math.ceil() : toujours vers le haut (ceiling/plafond)
console.log(Math.ceil(num1));    // 5
console.log(Math.ceil(num2));    // 5
console.log(Math.ceil(4.01));    // 5
console.log(Math.ceil(4.99));    // 5
console.log(Math.ceil(4));       // 4 (déjà entier)

// Math.floor() : toujours vers le bas (floor/plancher)
console.log(Math.floor(num1));   // 4
console.log(Math.floor(num2));   // 4
console.log(Math.floor(4.01));   // 4
console.log(Math.floor(4.99));   // 4
console.log(Math.floor(4));      // 4

// Math.trunc() : supprime décimales (ES6)
console.log(Math.trunc(num1));   // 4
console.log(Math.trunc(num2));   // 4
console.log(Math.trunc(4.99));   // 4
console.log(Math.trunc(4));      // 4

// Différence avec nombres NÉGATIFS
const neg1 = -4.3;
const neg2 = -4.7;

// Math.round()
console.log(Math.round(neg1));   // -4
console.log(Math.round(neg2));   // -5
console.log(Math.round(-4.5));   // -4 ⚠️ (vers le haut = moins négatif)

// Math.ceil() : vers le haut = moins négatif
console.log(Math.ceil(neg1));    // -4 (vers 0)
console.log(Math.ceil(neg2));    // -4
console.log(Math.ceil(-4.99));   // -4

// Math.floor() : vers le bas = plus négatif
console.log(Math.floor(neg1));   // -5 (loin de 0)
console.log(Math.floor(neg2));   // -5
console.log(Math.floor(-4.01));  // -5

// Math.trunc() : vers zéro
console.log(Math.trunc(neg1));   // -4
console.log(Math.trunc(neg2));   // -4
console.log(Math.trunc(-4.99));  // -4

// Visualisation
//        floor    trunc/round    ceil
// -5 -------|----------|----------|------- -4
//         -4.7       -4.3

//         ceil     trunc/round   floor
//  4 -------|----------|----------|------- 5
//          4.3        4.7

// Cas d'usage pratiques

// 1. Arrondir un prix
function roundPrice(price) {
    return Math.round(price * 100) / 100;  // 2 décimales
}

console.log(roundPrice(19.996));  // 20
console.log(roundPrice(19.994));  // 19.99

// 2. Calculer nombre de pages
function calculatePages(items, itemsPerPage) {
    return Math.ceil(items / itemsPerPage);
}

console.log(calculatePages(25, 10));  // 3 pages (10+10+5)
console.log(calculatePages(30, 10));  // 3 pages (10+10+10)
console.log(calculatePages(31, 10));  // 4 pages (10+10+10+1)

// 3. Index d'array (toujours entier positif)
const arr = [10, 20, 30, 40, 50];
const percentage = 0.73;  // 73%
const index = Math.floor(arr.length * percentage);
console.log(arr[index]);  // arr[3] = 40

// 4. Supprimer décimales sans arrondir
function removeDecimals(num) {
    return Math.trunc(num);
}

console.log(removeDecimals(3.99));   // 3
console.log(removeDecimals(-3.99));  // -3

// 5. Arrondir à n décimales
function roundToDecimals(num, decimals) {
    const factor = Math.pow(10, decimals);
    return Math.round(num * factor) / factor;
}

console.log(roundToDecimals(3.14159, 2));  // 3.14
console.log(roundToDecimals(3.14159, 4));  // 3.1416

// Comparaison rapide
const test = 4.7;
console.log("Number:", test);
console.log("round:", Math.round(test));   // 5
console.log("ceil:", Math.ceil(test));     // 5
console.log("floor:", Math.floor(test));   // 4
console.log("trunc:", Math.trunc(test));   // 4

const testNeg = -4.7;
console.log("Number:", testNeg);
console.log("round:", Math.round(testNeg)); // -5
console.log("ceil:", Math.ceil(testNeg));   // -4 (vers 0)
console.log("floor:", Math.floor(testNeg)); // -5 (loin de 0)
console.log("trunc:", Math.trunc(testNeg)); // -4 (vers 0)
```

**Résumé :**
- **`Math.round()`** : au plus proche (0.5 → up)
- **`Math.ceil()`** : toujours up (plafond)
- **`Math.floor()`** : toujours down (plancher)
- **`Math.trunc()`** : supprime décimales (vers zéro)

---

### Q8 : Comment générer un nombre aléatoire dans un intervalle spécifique ?

**Réponse :**

`Math.random()` retourne un nombre entre **0 (inclus) et 1 (exclus)**. Pour obtenir un intervalle spécifique, il faut faire des calculs.

```javascript
// Math.random() : 0 ≤ x < 1
console.log(Math.random());  // 0.xxx (exemple: 0.7342)

// Nombre aléatoire entre 0 et n (exclus)
function random(n) {
    return Math.random() * n;
}

console.log(random(10));     // 0 ≤ x < 10 (exemple: 7.342)

// Nombre aléatoire ENTIER entre 0 et n-1
function randomInt(n) {
    return Math.floor(Math.random() * n);
}

console.log(randomInt(10));  // 0, 1, 2, 3, 4, 5, 6, 7, 8, ou 9

// Nombre aléatoire entre min et max (exclus max)
function randomBetween(min, max) {
    return Math.random() * (max - min) + min;
}

console.log(randomBetween(5, 10));  // 5 ≤ x < 10

// Nombre aléatoire ENTIER entre min et max (INCLUS les deux) - FORMULE LA PLUS UTILISÉE
function randomIntInclusive(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

console.log(randomIntInclusive(1, 6));   // Simuler un dé : 1, 2, 3, 4, 5, ou 6
console.log(randomIntInclusive(10, 20)); // 10 à 20 inclus

// Explication de la formule
// Math.random() * (max - min + 1) → donne 0 à (max-min+1) exclus
// + min → décale l'intervalle
// Math.floor() → arrondit vers le bas pour obtenir un entier

// Exemples pratiques

// 1. Simuler un dé (1-6)
function rollDice() {
    return randomIntInclusive(1, 6);
}

console.log(rollDice());  // 1-6

// 2. Choisir un élément aléatoire d'un array
function randomElement(arr) {
    const index = randomInt(arr.length);
    return arr[index];
}

const fruits = ["apple", "banana", "orange", "grape"];
console.log(randomElement(fruits));

// 3. Mélanger un array (shuffle)
function shuffle(arr) {
    const result = [...arr];  // Copie pour ne pas modifier l'original
    
    for (let i = result.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        [result[i], result[j]] = [result[j], result[i]];  // Swap
    }
    
    return result;
}

const numbers = [1, 2, 3, 4, 5];
console.log(shuffle(numbers));  // [3, 1, 5, 2, 4] (ordre aléatoire)

// 4. Générer une couleur RGB aléatoire
function randomColor() {
    const r = randomInt(256);  // 0-255
    const g = randomInt(256);
    const b = randomInt(256);
    return `rgb(${r}, ${g}, ${b})`;
}

console.log(randomColor());  // "rgb(123, 45, 200)"

// 5. Générer une couleur hexadécimale aléatoire
function randomHexColor() {
    const hex = Math.floor(Math.random() * 16777215).toString(16);
    return "#" + hex.padStart(6, "0");
}

console.log(randomHexColor());  // "#a3f5c2"

// 6. Générer un ID aléatoire
function generateId(length = 8) {
    const chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    let result = "";
    
    for (let i = 0; i < length; i++) {
        result += chars[randomInt(chars.length)];
    }
    
    return result;
}

console.log(generateId());     // "aB3xK9mP"
console.log(generateId(12));   // "9xKmP3aB7qWe"

// 7. Simuler une probabilité
function probability(percent) {
    return Math.random() < percent / 100;
}

if (probability(20)) {  // 20% de chance
    console.log("Rare event!");
}
```

**Formule à retenir :**
```javascript
// Entier entre min et max (INCLUS)
Math.floor(Math.random() * (max - min + 1)) + min
```

---

### Q9 : Quelle est la différence entre `toFixed()` et `toPrecision()` ?

**Réponse :**

**`toFixed(n)`** spécifie le nombre de **décimales** (chiffres après la virgule), tandis que **`toPrecision(n)`** spécifie le nombre de **chiffres significatifs** totaux.

⚠️ **Les deux retournent des strings, pas des numbers!**

```javascript
const num = 123.456;

// toFixed(n) : n décimales après la virgule
console.log(num.toFixed(0));      // "123" (0 décimales)
console.log(num.toFixed(1));      // "123.5" (1 décimale, arrondi)
console.log(num.toFixed(2));      // "123.46" (2 décimales, arrondi)
console.log(num.toFixed(5));      // "123.45600" (ajoute des zéros)

// toPrecision(n) : n chiffres significatifs TOTAUX
console.log(num.toPrecision(1));  // "1e+2" (notation scientifique)
console.log(num.toPrecision(2));  // "1.2e+2"
console.log(num.toPrecision(3));  // "123"
console.log(num.toPrecision(4));  // "123.5"
console.log(num.toPrecision(5));  // "123.46"
console.log(num.toPrecision(6));  // "123.456"
console.log(num.toPrecision(7));  // "123.4560" (ajoute des zéros)

// Différence clé avec petits nombres
const small = 0.00456;

console.log(small.toFixed(2));       // "0.00" (2 décimales)
console.log(small.toFixed(4));       // "0.0046" (4 décimales)
console.log(small.toPrecision(2));   // "0.0046" (2 chiffres significatifs)
console.log(small.toPrecision(3));   // "0.00456" (3 chiffres significatifs)

// Différence avec grands nombres
const big = 12345.6789;

console.log(big.toFixed(2));         // "12345.68" (2 décimales)
console.log(big.toPrecision(2));     // "1.2e+4" (2 chiffres significatifs)
console.log(big.toPrecision(4));     // "1.235e+4" (4 chiffres significatifs)
console.log(big.toPrecision(7));     // "12345.68" (7 chiffres significatifs)

// ⚠️ Retournent des strings!
const result1 = num.toFixed(2);
const result2 = num.toPrecision(4);

console.log(typeof result1);  // "string"
console.log(typeof result2);  // "string"

// Convertir en number après
console.log(Number(result1));       // 123.46 (number)
console.log(parseFloat(result1));   // 123.46 (number)
console.log(+result1);              // 123.46 (number)

// Cas d'usage pratiques

// 1. Formater des prix (toFixed)
function formatPrice(price) {
    return "$" + price.toFixed(2);
}

console.log(formatPrice(19.5));      // "$19.50"
console.log(formatPrice(19.999));    // "$20.00"
console.log(formatPrice(19));        // "$19.00"

// 2. Afficher des pourcentages (toFixed)
function formatPercentage(value) {
    return (value * 100).toFixed(1) + "%";
}

console.log(formatPercentage(0.456));   // "45.6%"
console.log(formatPercentage(0.1));     // "10.0%"

// 3. Mesures scientifiques (toPrecision)
function formatMeasurement(value, precision = 3) {
    return value.toPrecision(precision);
}

console.log(formatMeasurement(0.00123));      // "0.00123" (3 sig figs)
console.log(formatMeasurement(123456));       // "1.23e+5" (3 sig figs)
console.log(formatMeasurement(1.23456, 5));   // "1.2346" (5 sig figs)

// 4. Arrondir proprement
function round(num, decimals) {
    return Number(num.toFixed(decimals));
}

console.log(round(3.14159, 2));    // 3.14 (number)
console.log(round(10.995, 2));     // 11 (number)

// Piège avec l'arrondi des flottants
console.log((0.1 + 0.2).toFixed(1));  // "0.3" ✅
console.log(0.1 + 0.2);               // 0.30000000000000004 ⚠️

// Comparaison visuelle
const test = 1234.5678;

console.log("Original:", test);
console.log("toFixed(0):", test.toFixed(0));       // "1235"
console.log("toFixed(2):", test.toFixed(2));       // "1234.57"
console.log("toFixed(4):", test.toFixed(4));       // "1234.5678"
console.log("toPrecision(2):", test.toPrecision(2)); // "1.2e+3"
console.log("toPrecision(4):", test.toPrecision(4)); // "1235"
console.log("toPrecision(6):", test.toPrecision(6)); // "1234.57"
```

**Résumé :**
- **`toFixed(n)`** : **n décimales** après la virgule → formatage de prix, pourcentages
- **`toPrecision(n)`** : **n chiffres significatifs** totaux → mesures scientifiques
- **Les deux retournent des strings** → convertir avec `Number()` si besoin

---

## Questions Pratiques / Pièges

### Q10 : Que va afficher ce code ?

```javascript
const str = "100px";
console.log(parseInt(str));
console.log(parseFloat(str));
console.log(Number(str));
```

**Réponse :**

```javascript
console.log(parseInt(str));    // 100 (s'arrête à "px")
console.log(parseFloat(str));  // 100 (s'arrête à "px")
console.log(Number(str));      // NaN (strict, "px" invalide)

// parseInt et parseFloat sont "tolérants"
// Ils parsent jusqu'au premier caractère invalide

// Number() est strict
// Toute la string doit être un nombre valide
```

---

### Q11 : Que va afficher ce code ?

```javascript
console.log(0.1 + 0.2);
console.log(0.1 + 0.2 === 0.3);
```

**Réponse :**

```javascript
console.log(0.1 + 0.2);           // 0.30000000000000004 ⚠️
console.log(0.1 + 0.2 === 0.3);   // false ⚠️

// Problème : précision des nombres flottants (IEEE 754)
// Les nombres décimaux ne peuvent pas toujours être représentés exactement en binaire

// Solutions :

// 1. Arrondir avec toFixed
const result = (0.1 + 0.2).toFixed(1);
console.log(result);  // "0.3" (string)
console.log(Number(result) === 0.3);  // true

// 2. Comparer avec epsilon (tolérance)
function isEqual(a, b, epsilon = 0.0001) {
    return Math.abs(a - b) < epsilon;
}

console.log(isEqual(0.1 + 0.2, 0.3));  // true

// 3. Multiplier pour éviter les décimales
console.log((0.1 * 10 + 0.2 * 10) / 10);  // 0.3 ✅

// 4. Utiliser Number.EPSILON (ES6)
console.log(Math.abs(0.1 + 0.2 - 0.3) < Number.EPSILON);  // false (epsilon trop petit)

const epsilon = 1e-10;  // 0.0000000001
console.log(Math.abs(0.1 + 0.2 - 0.3) < epsilon);  // true

// Pour des calculs financiers : travailler en centimes (entiers)
const price1 = 10;  // 10 cents = $0.10
const price2 = 20;  // 20 cents = $0.20
const total = price1 + price2;  // 30 cents
console.log(total / 100);  // $0.30 ✅
```

---

### Q12 : Que va afficher ce code ?

```javascript
const num = 3.14159;
console.log(typeof num.toFixed(2));
console.log(num.toFixed(2) + 1);
```

**Réponse :**

```javascript
console.log(typeof num.toFixed(2));  // "string" ⚠️
console.log(num.toFixed(2) + 1);     // "3.141" (concaténation, pas addition!)

// toFixed retourne une STRING, pas un number!

// Pour faire des calculs, convertir en number
console.log(Number(num.toFixed(2)) + 1);  // 4.14 ✅
console.log(parseFloat(num.toFixed(2)) + 1);  // 4.14 ✅
console.log(+num.toFixed(2) + 1);  // 4.14 ✅

// Même chose avec toPrecision
console.log(typeof num.toPrecision(3));  // "string"
console.log(num.toPrecision(3) + 1);     // "3.141" (concaténation)
```

---

### Q13 : Que va afficher ce code ?

```javascript
console.log(Math.max());
console.log(Math.min());
```

**Réponse :**

```javascript
console.log(Math.max());  // -Infinity ⚠️
console.log(Math.min());  // Infinity ⚠️

// Sans arguments :
// Math.max() retourne -Infinity (valeur initiale pour comparaison)
// Math.min() retourne Infinity (valeur initiale pour comparaison)

// Avec un array vide
const arr = [];
console.log(Math.max(...arr));  // -Infinity
console.log(Math.min(...arr));  // Infinity

// Pour gérer les cas vides
function getMax(arr) {
    if (arr.length === 0) return null;
    return Math.max(...arr);
}

console.log(getMax([]));        // null
console.log(getMax([1, 2, 3])); // 3
```

---

### Q14 : Expliquez ce code : `"Hello".repeat(3)`

**Réponse :**

`repeat(n)` répète la string **n fois** et retourne une nouvelle string.

```javascript
console.log("Ha".repeat(3));     // "HaHaHa"
console.log("*".repeat(10));     // "**********"
console.log("Hello".repeat(0));  // "" (string vide)
console.log("Test".repeat(1));   // "Test"

// ⚠️ n doit être positif
// console.log("X".repeat(-1));  // RangeError

// Cas d'usage pratiques

// 1. Créer une ligne de séparation
function separator(char = "-", length = 50) {
    return char.repeat(length);
}

console.log(separator());         // "--------------------------------------------------"
console.log(separator("=", 30));  // "=============================="

// 2. Padding manuel (avant padStart/padEnd)
function pad(str, length, char = " ") {
    const padding = char.repeat(Math.max(0, length - str.length));
    return str + padding;
}

console.log(pad("Hello", 10));     // "Hello     "
console.log(pad("Test", 10, "."));  // "Test......"

// 3. Indentation
function indent(text, level = 1, spaces = 2) {
    return " ".repeat(level * spaces) + text;
}

console.log(indent("Line 1"));       // "  Line 1"
console.log(indent("Line 2", 2));    // "    Line 2"
console.log(indent("Line 3", 3, 4)); // "            Line 3"

// 4. Créer un pattern
function createPattern(char, rows, cols) {
    const line = char.repeat(cols);
    return (line + "\n").repeat(rows);
}

console.log(createPattern("*", 3, 5));
// *****
// *****
// *****

// 5. Masquer un mot de passe
function maskPassword(password) {
    return "*".repeat(password.length);
}

console.log(maskPassword("secret123"));  // "*********"
```

---

**Résumé Chapitre 4 :**

✅ **Strings sont immutables** : méthodes retournent de nouvelles strings  
✅ **slice()** : le plus flexible pour extraire  
✅ **replace()** vs **replaceAll()** : première vs toutes occurrences  
✅ **indexOf()** vs **includes()** : index vs boolean  
✅ **trim()** : nettoyer les espaces  
✅ **split()** et **join()** : string ↔ array  
✅ **Template literals** : backticks avec `${}`  
✅ **parseInt()** vs **Number()** : tolérant vs strict  
✅ **toFixed()** vs **toPrecision()** : décimales vs chiffres significatifs  
✅ **Math.round/ceil/floor/trunc** : différents arrondis  
✅ **Math.random()** : formule `Math.floor(Math.random() * (max - min + 1)) + min`  


---

# Questions/Réponses Orales - Chapitre 4 : Strings et Numbers

## Questions sur String Methods

### Q1 : Quelle est la différence entre `slice()`, `substring()` et `substr()` ?

**Réponse :**

Les trois méthodes extraient une portion de string, mais avec des différences importantes :

- **`slice(start, end)`** : accepte les indices négatifs, `end` non inclus
- **`substring(start, end)`** : pas d'indices négatifs (négatif = 0), `end` non inclus
- **`substr(start, length)`** : ⚠️ **Déprécié**, utilise une longueur au lieu d'un index de fin

```javascript
const text = "JavaScript";

// slice() : le plus flexible (RECOMMANDÉ)
console.log(text.slice(0, 4));      // "Java"
console.log(text.slice(4));         // "Script" (jusqu'à la fin)
console.log(text.slice(-6));        // "Script" (6 caractères depuis la fin)
console.log(text.slice(4, -2));     // "Scri" (de 4 à avant-dernier -2)
console.log(text.slice(-6, -2));    // "Scri" (négatifs des deux côtés)

// substring() : pas d'indices négatifs
console.log(text.substring(0, 4));  // "Java"
console.log(text.substring(4));     // "Script"
console.log(text.substring(-6));    // "JavaScript" ⚠️ (-6 devient 0)
console.log(text.substring(4, -2)); // "Java" ⚠️ (-2 devient 0, puis swap)

// substring inverse automatiquement si start > end
console.log(text.substring(4, 0));  // "Java" (équivalent à substring(0, 4))

// substr() : déprécié, à éviter
console.log(text.substr(0, 4));     // "Java" (start=0, length=4)
console.log(text.substr(4, 6));     // "Script" (start=4, length=6)
console.log(text.substr(-6, 6));    // "Script" (6 derniers caractères)

// Exemple pratique : extraire extension de fichier
const filename = "document.pdf";

// ✅ Avec slice et lastIndexOf
const extension = filename.slice(filename.lastIndexOf(".") + 1);
console.log(extension);  // "pdf"

// ✅ Avec substring
const extension2 = filename.substring(filename.lastIndexOf(".") + 1);
console.log(extension2);  // "pdf"

// Extraire nom sans extension
const nameWithoutExt = filename.slice(0, filename.lastIndexOf("."));
console.log(nameWithoutExt);  // "document"
```

**Recommandation :** Utiliser **`slice()`** car c'est le plus flexible et cohérent.

---

### Q2 : Quelle est la différence entre `replace()` et `replaceAll()` ?

**Réponse :**

**`replace()`** remplace seulement la **première occurrence**, tandis que **`replaceAll()`** (ES2021) remplace **toutes les occurrences**.

```javascript
const text = "Hello World, Hello Universe, Hello JavaScript";

// replace() : seulement la PREMIÈRE occurrence
console.log(text.replace("Hello", "Hi"));
// "Hi World, Hello Universe, Hello JavaScript"

// replaceAll() : TOUTES les occurrences (ES2021)
console.log(text.replaceAll("Hello", "Hi"));
// "Hi World, Hi Universe, Hi JavaScript"

// Avant replaceAll : utiliser regex avec flag 'g' (global)
console.log(text.replace(/Hello/g, "Hi"));
// "Hi World, Hi Universe, Hi JavaScript"

// replace avec regex sans 'g' : seulement première occurrence
console.log(text.replace(/Hello/, "Hi"));
// "Hi World, Hello Universe, Hello JavaScript"

// Case insensitive avec flag 'i'
const text2 = "HELLO hello HeLLo";
console.log(text2.replace(/hello/gi, "Hi"));
// "Hi Hi Hi"

// replace avec fonction callback
const prices = "Item1: $10, Item2: $20, Item3: $30";

const updated = prices.replace(/\$(\d+)/g, function(match, number) {
    return "$" + (parseInt(number) * 1.1).toFixed(2);
});

console.log(updated);
// "Item1: $11.00, Item2: $22.00, Item3: $33.00"

// Avec arrow function (plus concis)
const updated2 = prices.replace(/\$(\d+)/g, (match, num) => {
    return "$" + (parseInt(num) * 1.1).toFixed(2);
});

// Exemple pratique : censurer des mots
function censorWords(text, badWords) {
    let result = text;
    badWords.forEach(word => {
        const regex = new RegExp(word, 'gi');
        result = result.replace(regex, '*'.repeat(word.length));
    });
    return result;
}

console.log(censorWords("This is bad and very bad", ["bad"]));
// "This is *** and very ***"

// Remplacer patterns complexes
const email = "Contact us at info@example.com or support@example.com";

const masked = email.replace(/([a-z]+)@/g, "***@");
console.log(masked);
// "Contact us at ***@example.com or ***@example.com"
```

**Compatibilité :** `replaceAll()` n'est pas supporté par les vieux navigateurs. Pour la compatibilité, utiliser `replace()` avec regex et flag `g`.

---

### Q3 : Quelle est la différence entre `indexOf()` et `includes()` ?

**Réponse :**

**`indexOf()`** retourne l'**index** de la première occurrence (ou -1 si absent), tandis que **`includes()`** retourne un **boolean** (true/false).

```javascript
const text = "JavaScript is awesome";

// indexOf() : retourne l'index (number)
console.log(text.indexOf("JavaScript"));  // 0 (commence à l'index 0)
console.log(text.indexOf("is"));          // 11
console.log(text.indexOf("Python"));      // -1 (pas trouvé)
console.log(text.indexOf("awesome"));     // 14

// includes() : retourne boolean (ES6)
console.log(text.includes("JavaScript")); // true
console.log(text.includes("is"));         // true
console.log(text.includes("Python"));     // false
console.log(text.includes("awesome"));    // true

// Les deux sont sensibles à la casse
console.log(text.indexOf("javascript"));  // -1 (minuscule)
console.log(text.includes("JAVASCRIPT")); // false (majuscule)

// indexOf() avec fromIndex (2ème paramètre)
const repeated = "hello world, hello universe";
console.log(repeated.indexOf("hello"));      // 0 (première occurrence)
console.log(repeated.indexOf("hello", 1));   // 13 (cherche après index 1)
console.log(repeated.indexOf("hello", 14));  // -1 (pas trouvé après 14)

// includes() avec fromIndex
console.log(repeated.includes("hello"));     // true
console.log(repeated.includes("hello", 14)); // false (après index 14)

// Quand utiliser quoi ?

// ✅ includes() : vérifier si présent (plus lisible)
if (text.includes("JavaScript")) {
    console.log("C'est du JavaScript!");
}

// ✅ indexOf() : besoin de la position
const position = text.indexOf("awesome");
if (position !== -1) {
    console.log(`Found at position ${position}`);
}

// ⚠️ Piège avec indexOf() dans les conditions
// ❌ Mauvais
if (text.indexOf("JavaScript")) {  // 0 = falsy!
    console.log("Never executed!");  // Ne s'exécute pas
}

// ✅ Bon
if (text.indexOf("JavaScript") !== -1) {
    console.log("Found!");
}

// ✅ Ou utiliser includes (plus simple)
if (text.includes("JavaScript")) {
    console.log("Found!");
}

// Exemple pratique : validation d'email
function isValidEmail(email) {
    // Version avec includes (plus lisible)
    return email.includes("@") && 
           email.includes(".") && 
           email.indexOf("@") < email.lastIndexOf(".");
}

console.log(isValidEmail("test@example.com"));  // true
console.log(isValidEmail("invalid.com"));       // false
console.log(isValidEmail("test@com"));          // false

// Exemple : filtrer un array
const words = ["apple", "banana", "grape", "orange", "pineapple"];

// Trouver tous les mots contenant "apple"
const withApple = words.filter(word => word.includes("apple"));
console.log(withApple);  // ["apple", "pineapple"]

// Trouver la position du premier mot contenant "an"
const index = words.findIndex(word => word.includes("an"));
console.log(index);  // 1 (banana)
```

**Résumé :**
- **`indexOf()`** → retourne **index** (number), -1 si absent
- **`includes()`** → retourne **boolean**, plus lisible pour les vérifications

---

### Q4 : Comment fonctionnent `trim()`, `trimStart()` et `trimEnd()` ?

**Réponse :**

Ces méthodes suppriment les **espaces blancs** (espaces, tabs, retours à la ligne) :

- **`trim()`** : supprime au début ET à la fin
- **`trimStart()` / `trimLeft()`** : supprime seulement au début (ES2019)
- **`trimEnd()` / `trimRight()`** : supprime seulement à la fin (ES2019)

```javascript
const messy = "   Hello World   ";

// trim() : supprime des deux côtés
console.log(messy.trim());           // "Hello World"
console.log(messy.trim().length);    // 11

// trimStart() / trimLeft() : seulement au début (ES2019)
console.log(messy.trimStart());      // "Hello World   "
console.log(messy.trimLeft());       // "Hello World   " (alias)

// trimEnd() / trimRight() : seulement à la fin (ES2019)
console.log(messy.trimEnd());        // "   Hello World"
console.log(messy.trimRight());      // "   Hello World" (alias)

// Original inchangé (strings sont immutables)
console.log(messy);                  // "   Hello World   "

// Espaces multiples
const spaces = "   \t\n  Hello  \n\t  ";
console.log(spaces.trim());          // "Hello"

// trim() ne supprime PAS les espaces internes
const internal = "  Hello   World  ";
console.log(internal.trim());        // "Hello   World" (espaces internes préservés)

// Pour supprimer les espaces internes : replace avec regex
console.log(internal.trim().replace(/\s+/g, " "));  // "Hello World"

// Cas d'usage pratique : nettoyer input utilisateur
function cleanInput(input) {
    return input.trim();
}

const userInput = "  alice@example.com  ";
console.log(cleanInput(userInput));  // "alice@example.com"

// Validation de formulaire
function validateUsername(username) {
    const cleaned = username.trim();
    
    if (cleaned.length === 0) {
        return "Username cannot be empty";
    }
    
    if (cleaned.length < 3) {
        return "Username must be at least 3 characters";
    }
    
    return cleaned;
}

console.log(validateUsername("  alice  "));  // "alice"
console.log(validateUsername("  a  "));      // "Username must be at least 3 characters"
console.log(validateUsername("   "));        // "Username cannot be empty"

// Nettoyer plusieurs lignes
const multiline = `
    Line 1
    Line 2
    Line 3
`;

const lines = multiline.split("\n").map(line => line.trim()).filter(line => line);
console.log(lines);  // ["Line 1", "Line 2", "Line 3"]

// Exemple : parser CSV
const csv = "  apple  ,  banana  ,  orange  ";
const fruits = csv.split(",").map(fruit => fruit.trim());
console.log(fruits);  // ["apple", "banana", "orange"]

// trimStart pour aligner du texte
const items = [
    "   Item 1",
    "     Item 2",
    "  Item 3"
];

const aligned = items.map(item => item.trimStart());
console.log(aligned);
// ["Item 1", "Item 2", "Item 3"]

// Attention : trim ne supprime pas TOUS les caractères blancs custom
const custom = "___Hello___";
console.log(custom.trim());  // "___Hello___" (underscore n'est pas un espace)

// Pour supprimer des caractères custom : replace
console.log(custom.replace(/^_+|_+$/g, ""));  // "Hello"
```

**Cas d'usage principaux :**
- Nettoyer input utilisateur
- Parser CSV/données
- Validation de formulaires
- Normaliser du texte

---

### Q5 : Expliquez `split()` et `join()` - comment sont-ils liés ?

**Réponse :**

**`split()`** convertit une **string en array**, tandis que **`join()`** (méthode d'array) fait l'inverse : convertit un **array en string**. Ils sont complémentaires.

```javascript
// split(separator, limit) : string → array
const sentence = "JavaScript is awesome";

// Split par espace
const words = sentence.split(" ");
console.log(words);  // ["JavaScript", "is", "awesome"]

// join(separator) : array → string
const joined = words.join(" ");
console.log(joined);  // "JavaScript is awesome"

// Différents séparateurs
const csv = "apple,banana,orange";
console.log(csv.split(","));        // ["apple", "banana", "orange"]
console.log(csv.split(",").join("|"));  // "apple|banana|orange"

// Split caractère par caractère (empty string)
const word = "Hello";
console.log(word.split(""));        // ["H", "e", "l", "l", "o"]
console.log(word.split("").reverse().join(""));  // "olleH" (inverser)

// Limiter le nombre d'éléments avec split
const text = "one two three four five";
console.log(text.split(" ", 3));    // ["one", "two", "three"]

// split sans paramètre : retourne array avec string complète
console.log(text.split());          // ["one two three four five"]

// join sans paramètre : utilise virgule par défaut
const arr = ["a", "b", "c"];
console.log(arr.join());            // "a,b,c"
console.log(arr.join(""));          // "abc" (sans séparateur)
console.log(arr.join(" - "));       // "a - b - c"

// Cas d'usage pratique : formater des noms
const fullName = "john doe smith";

const formatted = fullName
    .split(" ")
    .map(word => word.charAt(0).toUpperCase() + word.slice(1))
    .join(" ");

console.log(formatted);  // "John Doe Smith"

// Parser CSV
const csvData = "apple,5.99\nbanana,3.50\norange,4.25";

const products = csvData.split("\n").map(line => {
    const [name, price] = line.split(",");
    return { name, price: parseFloat(price) };
});

console.log(products);
// [
//   { name: "apple", price: 5.99 },
//   { name: "banana", price: 3.5 },
//   { name: "orange", price: 4.25 }
// ]

// Créer des initiales
function getInitials(fullName) {
    return fullName
        .split(" ")
        .map(name => name.charAt(0).toUpperCase())
        .join("");
}

console.log(getInitials("john doe smith"));  // "JDS"

// Remplacer tous les espaces par des tirets (slug)
function createSlug(title) {
    return title
        .toLowerCase()
        .trim()
        .split(/\s+/)  // Regex : un ou plusieurs espaces
        .join("-");
}

console.log(createSlug("  Hello   World  JavaScript  "));
// "hello-world-javascript"

// Compter les mots
function countWords(text) {
    return text.trim().split(/\s+/).length;
}

console.log(countWords("Hello World JavaScript"));  // 3
console.log(countWords("  Hello   World  "));       // 2

// Inverser les mots dans une phrase
function reverseWords(sentence) {
    return sentence
        .split(" ")
        .reverse()
        .join(" ");
}

console.log(reverseWords("Hello World"));  // "World Hello"

// Split avec regex
const text2 = "apple123banana456orange";
console.log(text2.split(/\d+/));  // ["apple", "banana", "orange"]

// Join avec des éléments non-string (conversion automatique)
const mixed = [1, 2, 3, true, "hello"];
console.log(mixed.join(" - "));  // "1 - 2 - 3 - true - hello"

// Créer un chemin de fichier
const pathParts = ["home", "user", "documents", "file.txt"];
const path = pathParts.join("/");
console.log(path);  // "home/user/documents/file.txt"

// Nettoyer et normaliser du texte
function normalizeText(text) {
    return text
        .trim()
        .split(/\s+/)      // Split par espaces multiples
        .join(" ")         // Rejoindre avec un seul espace
        .toLowerCase();
}

console.log(normalizeText("  Hello    World   JavaScript  "));
// "hello world javascript"
```

**Résumé :**
- **`split(separator)`** : string → array
- **`join(separator)`** : array → string
- Très utiles ensemble pour manipuler du texte

---

## Questions sur Numbers et Math

### Q6 : Quelle est la différence entre `parseInt()`, `parseFloat()` et `Number()` ?

**Réponse :**

Les trois convertissent des valeurs en nombres, mais avec des comportements différents :

- **`parseInt(string, radix)`** : parse jusqu'au premier caractère non-numérique, retourne un **entier**
- **`parseFloat(string)`** : parse jusqu'au premier caractère invalide, retourne un **nombre décimal**
- **`Number(value)`** : conversion stricte, toute la string doit être valide

```javascript
const str1 = "123";
const str2 = "123.45";
const str3 = "123px";
const str4 = "abc123";

// parseInt() : s'arrête au premier non-digit, retourne entier
console.log(parseInt(str1));        // 123
console.log(parseInt(str2));        // 123 (ignore .45)
console.log(parseInt(str3));        // 123 (ignore "px")
console.log(parseInt(str4));        // NaN (commence par lettres)

// parseFloat() : parse les décimales, s'arrête au premier invalide
console.log(parseFloat(str1));      // 123
console.log(parseFloat(str2));      // 123.45 ✅
console.log(parseFloat(str3));      // 123 (ignore "px")
console.log(parseFloat(str4));      // NaN

// Number() : conversion stricte (toute la string doit être valide)
console.log(Number(str1));          // 123
console.log(Number(str2));          // 123.45
console.log(Number(str3));          // NaN ⚠️ (strict)
console.log(Number(str4));          // NaN

// parseInt avec radix (base numérique)
console.log(parseInt("10", 10));    // 10 (décimal)
console.log(parseInt("10", 2));     // 2 (binaire)
console.log(parseInt("10", 8));     // 8 (octal)
console.log(parseInt("10", 16));    // 16 (hexadécimal)

console.log(parseInt("FF", 16));    // 255
console.log(parseInt("1010", 2));   // 10

// ⚠️ Toujours spécifier le radix!
console.log(parseInt("08"));        // 8 (aujourd'hui)
console.log(parseInt("08", 10));    // 8 ✅ (explicite)

// Number() avec autres types
console.log(Number(true));          // 1
console.log(Number(false));         // 0
console.log(Number(null));          // 0
console.log(Number(undefined));     // NaN
console.log(Number(""));            // 0 ⚠️
console.log(Number("  "));          // 0 ⚠️

// parseInt et parseFloat avec espaces
console.log(parseInt("  123  "));   // 123 (ignore espaces)
console.log(parseFloat("  3.14  ")); // 3.14
console.log(Number("  123  "));     // 123

// Exemples pratiques
// Extraire nombre d'une string
const price = "$19.99";
console.log(parseFloat(price.slice(1)));  // 19.99

// Parser input utilisateur
function parseUserInput(input) {
    const num = parseFloat(input);
    
    if (isNaN(num)) {
        return null;
    }
    
    return num;
}

console.log(parseUserInput("42"));      // 42
console.log(parseUserInput("3.14"));    // 3.14
console.log(parseUserInput("abc"));     // null

// Convertir hex en decimal
function hexToDecimal(hex) {
    return parseInt(hex, 16);
}

console.log(hexToDecimal("FF"));        // 255
console.log(hexToDecimal("A5"));        // 165

// Unary plus operator (raccourci pour Number())
console.log(+"123");                    // 123
console.log(+"123.45");                 // 123.45
console.log(+"123px");                  // NaN
console.log(+true);                     // 1
console.log(+false);                    // 0

// Comparaison
const input = "100px";

console.log(parseInt(input));           // 100 (tolère "px")
console.log(parseFloat(input));         // 100 (tolère "px")
console.log(Number(input));             // NaN (strict)
console.log(+input);                    // NaN (strict, équivalent à Number())
```

**Résumé :**
- **`parseInt()`** : entiers, tolère caractères après, spécifier radix
- **`parseFloat()`** : décimales, tolère caractères après
- **`Number()` / `+`** : conversion stricte, toute la string doit être valide

---

### Q7 : Quelle est la différence entre `Math.round()`, `Math.ceil()`, `Math.floor()` et `Math.trunc()` ?

**Réponse :**

Ces quatre méthodes arrondissent les nombres, mais de manière différente :

- **`Math.round()`** : arrondit au plus proche entier (0.5 → vers le haut)
- **`Math.ceil()`** : arrondit **toujours vers le haut** (plafond)
- **`Math.floor()`** : arrondit **toujours vers le bas** (plancher)
- **`Math.trunc()`** : supprime simplement la partie décimale (ES6)

```javascript
const num1 = 4.3;
const num2 = 4.7;
const num3 = 4.5;

// Math.round() : au plus proche
console.log(Math.round(num1));   // 4 (plus proche de 4)
console.log(Math.round(num2));   // 5 (plus proche de 5)
console.log(Math.round(num3));   // 5 (0.5 → vers le haut)
console.log(Math.round(4.49));   // 4
console.log(Math.round(4.50));   // 5

// Math.ceil() : toujours vers le haut (ceiling/plafond)
console.log(Math.ceil(num1));    // 5
console.log(Math.ceil(num2));    // 5
console.log(Math.ceil(4.01));    // 5
console.log(Math.ceil(4.99));    // 5
console.log(Math.ceil(4));       // 4 (déjà entier)

// Math.floor() : toujours vers le bas (floor/plancher)
console.log(Math.floor(num1));   // 4
console.log(Math.floor(num2));   // 4
console.log(Math.floor(4.01));   // 4
console.log(Math.floor(4.99));   // 4
console.log(Math.floor(4));      // 4

// Math.trunc() : supprime décimales (ES6)
console.log(Math.trunc(num1));   // 4
console.log(Math.trunc(num2));   // 4
console.log(Math.trunc(4.99));   // 4
console.log(Math.trunc(4));      // 4

// Différence avec nombres NÉGATIFS
const neg1 = -4.3;
const neg2 = -4.7;

// Math.round()
console.log(Math.round(neg1));   // -4
console.log(Math.round(neg2));   // -5
console.log(Math.round(-4.5));   // -4 ⚠️ (vers le haut = moins négatif)

// Math.ceil() : vers le haut = moins négatif
console.log(Math.ceil(neg1));    // -4 (vers 0)
console.log(Math.ceil(neg2));    // -4
console.log(Math.ceil(-4.99));   // -4

// Math.floor() : vers le bas = plus négatif
console.log(Math.floor(neg1));   // -5 (loin de 0)
console.log(Math.floor(neg2));   // -5
console.log(Math.floor(-4.01));  // -5

// Math.trunc() : vers zéro
console.log(Math.trunc(neg1));   // -4
console.log(Math.trunc(neg2));   // -4
console.log(Math.trunc(-4.99));  // -4

// Visualisation
//        floor    trunc/round    ceil
// -5 -------|----------|----------|------- -4
//         -4.7       -4.3

//         ceil     trunc/round   floor
//  4 -------|----------|----------|------- 5
//          4.3        4.7

// Cas d'usage pratiques

// 1. Arrondir un prix
function roundPrice(price) {
    return Math.round(price * 100) / 100;  // 2 décimales
}

console.log(roundPrice(19.996));  // 20
console.log(roundPrice(19.994));  // 19.99

// 2. Calculer nombre de pages
function calculatePages(items, itemsPerPage) {
    return Math.ceil(items / itemsPerPage);
}

console.log(calculatePages(25, 10));  // 3 pages (10+10+5)
console.log(calculatePages(30, 10));  // 3 pages (10+10+10)
console.log(calculatePages(31, 10));  // 4 pages (10+10+10+1)

// 3. Index d'array (toujours entier positif)
const arr = [10, 20, 30, 40, 50];
const percentage = 0.73;  // 73%
const index = Math.floor(arr.length * percentage);
console.log(arr[index]);  // arr[3] = 40

// 4. Supprimer décimales sans arrondir
function removeDecimals(num) {
    return Math.trunc(num);
}

console.log(removeDecimals(3.99));   // 3
console.log(removeDecimals(-3.99));  // -3

// 5. Arrondir à n décimales
function roundToDecimals(num, decimals) {
    const factor = Math.pow(10, decimals);
    return Math.round(num * factor) / factor;
}

console.log(roundToDecimals(3.14159, 2));  // 3.14
console.log(roundToDecimals(3.14159, 4));  // 3.1416

// Comparaison rapide
const test = 4.7;
console.log("Number:", test);
console.log("round:", Math.round(test));   // 5
console.log("ceil:", Math.ceil(test));     // 5
console.log("floor:", Math.floor(test));   // 4
console.log("trunc:", Math.trunc(test));   // 4

const testNeg = -4.7;
console.log("Number:", testNeg);
console.log("round:", Math.round(testNeg)); // -5
console.log("ceil:", Math.ceil(testNeg));   // -4 (vers 0)
console.log("floor:", Math.floor(testNeg)); // -5 (loin de 0)
console.log("trunc:", Math.trunc(testNeg)); // -4 (vers 0)
```

**Résumé :**
- **`Math.round()`** : au plus proche (0.5 → up)
- **`Math.ceil()`** : toujours up (plafond)
- **`Math.floor()`** : toujours down (plancher)
- **`Math.trunc()`** : supprime décimales (vers zéro)

---

### Q8 : Comment générer un nombre aléatoire dans un intervalle spécifique ?

**Réponse :**

`Math.random()` retourne un nombre entre **0 (inclus) et 1 (exclus)**. Pour obtenir un intervalle spécifique, il faut faire des calculs.

```javascript
// Math.random() : 0 ≤ x < 1
console.log(Math.random());  // 0.xxx (exemple: 0.7342)

// Nombre aléatoire entre 0 et n (exclus)
function random(n) {
    return Math.random() * n;
}

console.log(random(10));     // 0 ≤ x < 10 (exemple: 7.342)

// Nombre aléatoire ENTIER entre 0 et n-1
function randomInt(n) {
    return Math.floor(Math.random() * n);
}

console.log(randomInt(10));  // 0, 1, 2, 3, 4, 5, 6, 7, 8, ou 9

// Nombre aléatoire entre min et max (exclus max)
function randomBetween(min, max) {
    return Math.random() * (max - min) + min;
}

console.log(randomBetween(5, 10));  // 5 ≤ x < 10

// Nombre aléatoire ENTIER entre min et max (INCLUS les deux) - FORMULE LA PLUS UTILISÉE
function randomIntInclusive(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

console.log(randomIntInclusive(1, 6));   // Simuler un dé : 1, 2, 3, 4, 5, ou 6
console.log(randomIntInclusive(10, 20)); // 10 à 20 inclus

// Explication de la formule
// Math.random() * (max - min + 1) → donne 0 à (max-min+1) exclus
// + min → décale l'intervalle
// Math.floor() → arrondit vers le bas pour obtenir un entier

// Exemples pratiques

// 1. Simuler un dé (1-6)
function rollDice() {
    return randomIntInclusive(1, 6);
}

console.log(rollDice());  // 1-6

// 2. Choisir un élément aléatoire d'un array
function randomElement(arr) {
    const index = randomInt(arr.length);
    return arr[index];
}

const fruits = ["apple", "banana", "orange", "grape"];
console.log(randomElement(fruits));

// 3. Mélanger un array (shuffle)
function shuffle(arr) {
    const result = [...arr];  // Copie pour ne pas modifier l'original
    
    for (let i = result.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        [result[i], result[j]] = [result[j], result[i]];  // Swap
    }
    
    return result;
}

const numbers = [1, 2, 3, 4, 5];
console.log(shuffle(numbers));  // [3, 1, 5, 2, 4] (ordre aléatoire)

// 4. Générer une couleur RGB aléatoire
function randomColor() {
    const r = randomInt(256);  // 0-255
    const g = randomInt(256);
    const b = randomInt(256);
    return `rgb(${r}, ${g}, ${b})`;
}

console.log(randomColor());  // "rgb(123, 45, 200)"

// 5. Générer une couleur hexadécimale aléatoire
function randomHexColor() {
    const hex = Math.floor(Math.random() * 16777215).toString(16);
    return "#" + hex.padStart(6, "0");
}

console.log(randomHexColor());  // "#a3f5c2"

// 6. Générer un ID aléatoire
function generateId(length = 8) {
    const chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    let result = "";
    
    for (let i = 0; i < length; i++) {
        result += chars[randomInt(chars.length)];
    }
    
    return result;
}

console.log(generateId());     // "aB3xK9mP"
console.log(generateId(12));   // "9xKmP3aB7qWe"

// 7. Simuler une probabilité
function probability(percent) {
    return Math.random() < percent / 100;
}

if (probability(20)) {  // 20% de chance
    console.log("Rare event!");
}

// 8. Nombre aléatoire avec pas (step)
function randomStep(min, max, step) {
    const range = (max - min) / step;
    return Math.floor(Math.random() * (range + 1)) * step + min;
}

console.log(randomStep(0, 10, 2));  // 0, 2, 4, 6, 8, ou 10
console.log(randomStep(0, 1, 0.25)); // 0, 0.25, 0.5, 0.75, ou 1

// 9. Tirer n éléments uniques d'un array
function randomSample(arr, n) {
    const shuffled = shuffle(arr);
    return shuffled.slice(0, n);
}

const deck = ["A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"];
console.log(randomSample(deck, 5));  // 5 cartes aléatoires

// Attention : Math.random() n'est PAS cryptographiquement sécurisé
// Pour des usages sécurisés (tokens, passwords), utiliser crypto API
// window.crypto.getRandomValues() dans le navigateur
```

**Formule à retenir :**
```javascript
// Entier entre min et max (INCLUS)
Math.floor(Math.random() * (max - min + 1)) + min
```

---

### Q9 : Quelle est la différence entre `toFixed()` et `toPrecision()` ?

**Réponse :**

**`toFixed(n)`** spécifie le nombre de **décimales** (chiffres après la virgule), tandis que **`toPrecision(n)`** spécifie le nombre de **chiffres significatifs** totaux.

⚠️ **Les deux retournent des strings, pas des numbers!**

```javascript
const num = 123.456;

// toFixed(n) : n décimales après la virgule
console.log(num.toFixed(0));      // "123" (0 décimales)
console.log(num.toFixed(1));      // "123.5" (1 décimale, arrondi)
console.log(num.toFixed(2));      // "123.46" (2 décimales, arrondi)
console.log(num.toFixed(5));      // "123.45600" (ajoute des zéros)

// toPrecision(n) : n chiffres significatifs TOTAUX
console.log(num.toPrecision(1));  // "1e+2" (notation scientifique)
console.log(num.toPrecision(2));  // "1.2e+2"
console.log(num.toPrecision(3));  // "123"
console.log(num.toPrecision(4));  // "123.5"
console.log(num.toPrecision(5));  // "123.46"
console.log(num.toPrecision(6));  // "123.456"
console.log(num.toPrecision(7));  // "123.4560" (ajoute des zéros)

// Différence clé avec petits nombres
const small = 0.00456;

console.log(small.toFixed(2));       // "0.00" (2 décimales)
console.log(small.toFixed(4));       // "0.0046" (4 décimales)
console.log(small.toPrecision(2));   // "0.0046" (2 chiffres significatifs)
console.log(small.toPrecision(3));   // "0.00456" (3 chiffres significatifs)

// Différence avec grands nombres
const big = 12345.6789;

console.log(big.toFixed(2));         // "12345.68" (2 décimales)
console.log(big.toPrecision(2));     // "1.2e+4" (2 chiffres significatifs)
console.log(big.toPrecision(4));     // "1.235e+4" (4 chiffres significatifs)
console.log(big.toPrecision(7));     // "12345.68" (7 chiffres significatifs)

// ⚠️ Retournent des strings!
const result1 = num.toFixed(2);
const result2 = num.toPrecision(4);

console.log(typeof result1);  // "string"
console.log(typeof result2);  // "string"

// Convertir en number après
console.log(Number(result1));       // 123.46 (number)
console.log(parseFloat(result1));   // 123.46 (number)
console.log(+result1);              // 123.46 (number)

// Cas d'usage pratiques

// 1. Formater des prix (toFixed)
function formatPrice(price) {
    return "$" + price.toFixed(2);
}

console.log(formatPrice(19.5));      // "$19.50"
console.log(formatPrice(19.999));    // "$20.00"
console.log(formatPrice(19));        // "$19.00"

// 2. Afficher des pourcentages (toFixed)
function formatPercentage(value) {
    return (value * 100).toFixed(1) + "%";
}

console.log(formatPercentage(0.456));   // "45.6%"
console.log(formatPercentage(0.1));     // "10.0%"

// 3. Mesures scientifiques (toPrecision)
function formatMeasurement(value, precision = 3) {
    return value.toPrecision(precision);
}

console.log(formatMeasurement(0.00123));      // "0.00123" (3 sig figs)
console.log(formatMeasurement(123456));       // "1.23e+5" (3 sig figs)
console.log(formatMeasurement(1.23456, 5));   // "1.2346" (5 sig figs)

// 4. Arrondir proprement
function round(num, decimals) {
    return Number(num.toFixed(decimals));
}

console.log(round(3.14159, 2));    // 3.14 (number)
console.log(round(10.995, 2));     // 11 (number)

// Piège avec l'arrondi des flottants
console.log((0.1 + 0.2).toFixed(1));  // "0.3" ✅
console.log(0.1 + 0.2);               // 0.30000000000000004 ⚠️

// Comparaison visuelle
const test = 1234.5678;

console.log("Original:", test);
console.log("toFixed(0):", test.toFixed(0));       // "1235"
console.log("toFixed(2):", test.toFixed(2));       // "1234.57"
console.log("toFixed(4):", test.toFixed(4));       // "1234.5678"
console.log("toPrecision(2):", test.toPrecision(2)); // "1.2e+3"
console.log("toPrecision(4):", test.toPrecision(4)); // "1235"
console.log("toPrecision(6):", test.toPrecision(6)); // "1234.57"
```

**Résumé :**
- **`toFixed(n)`** : **n décimales** après la virgule → formatage de prix, pourcentages
- **`toPrecision(n)`** : **n chiffres significatifs** totaux → mesures scientifiques
- **Les deux retournent des strings** → convertir avec `Number()` si besoin

---

## Questions Pratiques / Pièges

### Q10 : Que va afficher ce code ?

```javascript
const str = "100px";
console.log(parseInt(str));
console.log(parseFloat(str));
console.log(Number(str));
```

**Réponse :**

```javascript
console.log(parseInt(str));    // 100 (s'arrête à "px")
console.log(parseFloat(str));  // 100 (s'arrête à "px")
console.log(Number(str));      // NaN (strict, "px" invalide)

// parseInt et parseFloat sont "tolérants"
// Ils parsent jusqu'au premier caractère invalide

// Number() est strict
// Toute la string doit être un nombre valide
```

---

### Q11 : Que va afficher ce code ?

```javascript
console.log(0.1 + 0.2);
console.log(0.1 + 0.2 === 0.3);
```

**Réponse :**

```javascript
console.log(0.1 + 0.2);           // 0.30000000000000004 ⚠️
console.log(0.1 + 0.2 === 0.3);   // false ⚠️

// Problème : précision des nombres flottants (IEEE 754)
// Les nombres décimaux ne peuvent pas toujours être représentés exactement en binaire

// Solutions :

// 1. Arrondir avec toFixed
const result = (0.1 + 0.2).toFixed(1);
console.log(result);  // "0.3" (string)
console.log(Number(result) === 0.3);  // true

// 2. Comparer avec epsilon (tolérance)
function isEqual(a, b, epsilon = 0.0001) {
    return Math.abs(a - b) < epsilon;
}

console.log(isEqual(0.1 + 0.2, 0.3));  // true

// 3. Multiplier pour éviter les décimales
console.log((0.1 * 10 + 0.2 * 10) / 10);  // 0.3 ✅

// 4. Utiliser Number.EPSILON (ES6)
console.log(Math.abs(0.1 + 0.2 - 0.3) < Number.EPSILON);  // false (epsilon trop petit)

const epsilon = 1e-10;  // 0.0000000001
console.log(Math.abs(0.1 + 0.2 - 0.3) < epsilon);  // true

// Pour des calculs financiers : travailler en centimes (entiers)
const price1 = 10;  // 10 cents = $0.10
const price2 = 20;  // 20 cents = $0.20
const total = price1 + price2;  // 30 cents
console.log(total / 100);  // $0.30 ✅
```

---

### Q12 : Que va afficher ce code ?

```javascript
const num = 3.14159;
console.log(typeof num.toFixed(2));
console.log(num.toFixed(2) + 1);
```

**Réponse :**

```javascript
console.log(typeof num.toFixed(2));  // "string" ⚠️
console.log(num.toFixed(2) + 1);     // "3.141" (concaténation, pas addition!)

// toFixed retourne une STRING, pas un number!

// Pour faire des calculs, convertir en number
console.log(Number(num.toFixed(2)) + 1);  // 4.14 ✅
console.log(parseFloat(num.toFixed(2)) + 1);  // 4.14 ✅
console.log(+num.toFixed(2) + 1);  // 4.14 ✅

// Même chose avec toPrecision
console.log(typeof num.toPrecision(3));  // "string"
console.log(num.toPrecision(3) + 1);     // "3.141" (concaténation)
```

---

### Q13 : Que va afficher ce code ?

```javascript
console.log(Math.max());
console.log(Math.min());
```

**Réponse :**

```javascript
console.log(Math.max());  // -Infinity ⚠️
console.log(Math.min());  // Infinity ⚠️

// Sans arguments :
// Math.max() retourne -Infinity (valeur initiale pour comparaison)
// Math.min() retourne Infinity (valeur initiale pour comparaison)

// Avec un array vide
const arr = [];
console.log(Math.max(...arr));  // -Infinity
console.log(Math.min(...arr));  // Infinity

// Pour gérer les cas vides
function getMax(arr) {
    if (arr.length === 0) return null;
    return Math.max(...arr);
}

console.log(getMax([]));        // null
console.log(getMax([1, 2, 3])); // 3
```

---

### Q14 : Expliquez ce code : `"Hello".repeat(3)`

**Réponse :**

`repeat(n)` répète la string **n fois** et retourne une nouvelle string.

```javascript
console.log("Ha".repeat(3));     // "HaHaHa"
console.log("*".repeat(10));     // "**********"
console.log("Hello".repeat(0));  // "" (string vide)
console.log("Test".repeat(1));   // "Test"

// ⚠️ n doit être positif
// console.log("X".repeat(-1));  // RangeError

// Cas d'usage pratiques

// 1. Créer une ligne de séparation
function separator(char = "-", length = 50) {
    return char.repeat(length);
}

console.log(separator());         // "--------------------------------------------------"
console.log(separator("=", 30));  // "=============================="

// 2. Padding manuel (avant padStart/padEnd)
function pad(str, length, char = " ") {
    const padding = char.repeat(Math.max(0, length - str.length));
    return str + padding;
}

console.log(pad("Hello", 10));     // "Hello     "
console.log(pad("Test", 10, "."));  // "Test......"

// 3. Indentation
function indent(text, level = 1, spaces = 2) {
    return " ".repeat(level * spaces) + text;
}

console.log(indent("Line 1"));       // "  Line 1"
console.log(indent("Line 2", 2));    // "    Line 2"
console.log(indent("Line 3", 3, 4)); // "            Line 3"

// 4. Créer un pattern
function createPattern(char, rows, cols) {
    const line = char.repeat(cols);
    return (line + "\n").repeat(rows);
}

console.log(createPattern("*", 3, 5));
// *****
// *****
// *****

// 5. Masquer un mot de passe
function maskPassword(password) {
    return "*".repeat(password.length);
}

console.log(maskPassword("secret123"));  // "*********"
```

---

**Résumé Chapitre 4 :**

✅ **Strings sont immutables** : méthodes retournent de nouvelles strings  
✅ **slice()** : le plus flexible pour extraire  
✅ **replace()** vs **replaceAll()** : première vs toutes occurrences  
✅ **indexOf()** vs **includes()** : index vs boolean  
✅ **trim()** : nettoyer les espaces  
✅ **split()** et **join()** : string ↔ array  
✅ **Template literals** : backticks avec `${}`  
✅ **parseInt()** vs **Number()** : tolérant vs strict  
✅ **toFixed()** vs **toPrecision()** : décimales vs chiffres significatifs  
✅ **Math.round/ceil/floor/trunc** : différents arrondis  
✅ **Math.random()** : formule `Math.floor(Math.random() * (max - min + 1)) + min`  
