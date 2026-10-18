## [**..**](./00_index.md)

# Chapitre 11 : Expressions Régulières (RegEx)

## 11.1 Introduction aux RegEx

Les **Regular Expressions (RegEx)** sont des patterns utilisés pour rechercher et manipuler du texte.

```javascript
// Deux façons de créer une RegEx

// 1. Literal notation (recommandé si pattern fixe)
const regex1 = /hello/;
const regex2 = /[0-9]+/;

// 2. Constructor (utile si pattern dynamique)
const pattern = 'hello';
const regex3 = new RegExp(pattern);
const regex4 = new RegExp('[0-9]+');

// Exemple d'utilisation basique
const text = 'Hello World';
const pattern = /Hello/;
console.log(pattern.test(text)); // true
```

---

## 11.2 Patterns et Flags

### Caractères Spéciaux de Base

```javascript
// . (point) - N'importe quel caractère sauf newline
const regex1 = /h.t/;
console.log(regex1.test('hat')); // true
console.log(regex1.test('hot')); // true
console.log(regex1.test('h t')); // true
console.log(regex1.test('ht'));  // false

// ^ - Début de chaîne
const regex2 = /^Hello/;
console.log(regex2.test('Hello World')); // true
console.log(regex2.test('Say Hello'));   // false

// $ - Fin de chaîne
const regex3 = /World$/;
console.log(regex3.test('Hello World')); // true
console.log(regex3.test('World Hello')); // false

// ^...$ - Match exact (début ET fin)
const regex4 = /^Hello$/;
console.log(regex4.test('Hello'));       // true
console.log(regex4.test('Hello World')); // false

// * - 0 ou plusieurs occurrences
const regex5 = /go*d/;
console.log(regex5.test('gd'));    // true (0 'o')
console.log(regex5.test('god'));   // true (1 'o')
console.log(regex5.test('good'));  // true (2 'o')
console.log(regex5.test('goood')); // true (3 'o')

// + - 1 ou plusieurs occurrences
const regex6 = /go+d/;
console.log(regex6.test('gd'));    // false
console.log(regex6.test('god'));   // true
console.log(regex6.test('good'));  // true

// ? - 0 ou 1 occurrence (optionnel)
const regex7 = /colou?r/;
console.log(regex7.test('color'));  // true
console.log(regex7.test('colour')); // true

// {n} - Exactement n occurrences
const regex8 = /\d{3}/;
console.log(regex8.test('12'));   // false
console.log(regex8.test('123'));  // true
console.log(regex8.test('1234')); // true

// {n,} - Au moins n occurrences
const regex9 = /\d{2,}/;
console.log(regex9.test('1'));   // false
console.log(regex9.test('12'));  // true
console.log(regex9.test('123')); // true

// {n,m} - Entre n et m occurrences
const regex10 = /\d{2,4}/;
console.log(regex10.test('1'));     // false
console.log(regex10.test('12'));    // true
console.log(regex10.test('123'));   // true
console.log(regex10.test('1234'));  // true
console.log(regex10.test('12345')); // true (match les 4 premiers)
```

### Character Classes (Classes de Caractères)

```javascript
// [abc] - Un des caractères listés
const regex1 = /[aeiou]/;
console.log(regex1.test('hello')); // true (contient 'e' et 'o')
console.log(regex1.test('sky'));   // true (contient 'y'... non! false)

// [^abc] - N'importe quel caractère SAUF ceux listés
const regex2 = /[^aeiou]/;
console.log(regex2.test('a'));   // false
console.log(regex2.test('b'));   // true

// [a-z] - Range (plage de caractères)
const regex3 = /[a-z]/;
console.log(regex3.test('hello')); // true
console.log(regex3.test('HELLO')); // false
console.log(regex3.test('123'));   // false

// [A-Z] - Lettres majuscules
const regex4 = /[A-Z]/;
console.log(regex4.test('Hello')); // true

// [0-9] - Chiffres
const regex5 = /[0-9]/;
console.log(regex5.test('abc123')); // true

// Combinaisons
const regex6 = /[a-zA-Z0-9]/; // Alphanumerique
const regex7 = /[a-z0-9_-]/;  // Lettres minuscules, chiffres, underscore, tiret

// Caractères spéciaux dans les classes
const regex8 = /[.+*?]/; // Literal . + * ? (pas besoin d'escape dans [])
console.log(regex8.test('a+b')); // true
```

### Shorthand Character Classes

```javascript
// \d - Digit (équivalent à [0-9])
const regex1 = /\d/;
console.log(regex1.test('abc123')); // true

// \D - Non-digit (équivalent à [^0-9])
const regex2 = /\D/;
console.log(regex2.test('123'));   // false
console.log(regex2.test('12a3'));  // true

// \w - Word character (équivalent à [a-zA-Z0-9_])
const regex3 = /\w/;
console.log(regex3.test('hello')); // true
console.log(regex3.test('_test')); // true
console.log(regex3.test('@'));     // false

// \W - Non-word character (équivalent à [^a-zA-Z0-9_])
const regex4 = /\W/;
console.log(regex4.test('hello')); // false
console.log(regex4.test('hello!')); // true

// \s - Whitespace (espace, tab, newline)
const regex5 = /\s/;
console.log(regex5.test('hello world')); // true
console.log(regex5.test('hello'));       // false

// \S - Non-whitespace
const regex6 = /\S/;
console.log(regex6.test('   '));   // false
console.log(regex6.test(' a '));   // true

// Exemples pratiques
// Valider un code postal (5 chiffres)
const zipCode = /^\d{5}$/;
console.log(zipCode.test('75001')); // true
console.log(zipCode.test('7500'));  // false

// Valider un username (lettres, chiffres, underscore, 3-16 caractères)
const username = /^\w{3,16}$/;
console.log(username.test('john_doe')); // true
console.log(username.test('ab'));       // false
console.log(username.test('user@123')); // false
```

### Groupes et Alternance

```javascript
// () - Groupes de capture
const regex1 = /(hello) (world)/;
const match1 = regex1.exec('hello world');
console.log(match1[0]); // "hello world" (match complet)
console.log(match1[1]); // "hello" (groupe 1)
console.log(match1[2]); // "world" (groupe 2)

// | - Alternance (OU logique)
const regex2 = /cat|dog/;
console.log(regex2.test('I have a cat')); // true
console.log(regex2.test('I have a dog')); // true
console.log(regex2.test('I have a bird')); // false

// Combinaison groupes + alternance
const regex3 = /I have a (cat|dog|bird)/;
const match3 = regex3.exec('I have a cat');
console.log(match3[1]); // "cat"

// Non-capturing group (?:...)
const regex4 = /(?:http|https):\/\/(\w+)/;
const match4 = regex4.exec('https://google');
console.log(match4[1]); // "google" (pas de groupe pour http/https)

// Exemple pratique: Valider un numéro de téléphone
const phone1 = /^(\d{2})[-.\s]?(\d{2})[-.\s]?(\d{2})[-.\s]?(\d{2})[-.\s]?(\d{2})$/;
console.log(phone1.test('06 12 34 56 78')); // true
console.log(phone1.test('06.12.34.56.78')); // true
console.log(phone1.test('0612345678'));     // true
```

### Escape de Caractères Spéciaux

```javascript
// Caractères à escape: . + * ? ^ $ { } ( ) [ ] | \

// Sans escape (cherche n'importe quel caractère)
const regex1 = /3.14/;
console.log(regex1.test('3.14')); // true
console.log(regex1.test('3a14')); // true aussi!

// Avec escape (cherche le point literal)
const regex2 = /3\.14/;
console.log(regex2.test('3.14')); // true
console.log(regex2.test('3a14')); // false

// Autres exemples
const regex3 = /\$/;        // Cherche le symbole $
const regex4 = /\(/;        // Cherche la parenthèse (
const regex5 = /\[test\]/;  // Cherche [test]
const regex6 = /\\/;        // Cherche le backslash \

// Exemple pratique: Valider un prix
const price = /^\$?\d+(\.\d{2})?$/;
console.log(price.test('$19.99')); // true
console.log(price.test('19.99'));  // true
console.log(price.test('19'));     // true
console.log(price.test('$19'));    // true
console.log(price.test('19.9'));   // false
```

### Flags (Modificateurs)

```javascript
// g - Global (trouve toutes les occurrences, pas juste la première)
const text1 = 'cat cat cat';
const regex1 = /cat/;
const regex2 = /cat/g;
console.log(text1.match(regex1)); // ["cat"] (une seule)
console.log(text1.match(regex2)); // ["cat", "cat", "cat"] (toutes)

// i - Case insensitive (ignore majuscules/minuscules)
const regex3 = /hello/;
const regex4 = /hello/i;
console.log(regex3.test('Hello')); // false
console.log(regex4.test('Hello')); // true
console.log(regex4.test('HELLO')); // true

// m - Multiline (^ et $ matchent début/fin de chaque ligne)
const text2 = 'line1\nline2\nline3';
const regex5 = /^line/;   // Match seulement première ligne
const regex6 = /^line/m;  // Match début de chaque ligne
console.log(text2.match(regex5)); // ["line1"]
console.log(text2.match(regex6)); // ["line", "line", "line"]

// Combinaison de flags
const regex7 = /hello/gi; // Global + case insensitive
const text3 = 'Hello hello HELLO';
console.log(text3.match(regex7)); // ["Hello", "hello", "HELLO"]

// s - Dotall (. matche aussi newline)
const regex8 = /hello.world/;
const regex9 = /hello.world/s;
console.log(regex8.test('hello\nworld')); // false
console.log(regex9.test('hello\nworld')); // true

// u - Unicode
const regex10 = /\u{1F600}/u; // Emoji 😀
console.log(regex10.test('😀')); // true

// y - Sticky (cherche à partir de lastIndex)
const regex11 = /cat/y;
regex11.lastIndex = 0;
console.log(regex11.test('cat cat')); // true
console.log(regex11.test('cat cat')); // false (lastIndex a changé)
```

---

## 11.3 Méthodes RegEx

### test() - Retourne boolean

```javascript
// test() vérifie si le pattern existe dans la string
const regex = /hello/i;

console.log(regex.test('Hello World')); // true
console.log(regex.test('Goodbye'));     // false

// Exemple pratique: Validation email basique
function isValidEmail(email) {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailRegex.test(email);
}

console.log(isValidEmail('user@example.com')); // true
console.log(isValidEmail('invalid.email'));    // false

// Validation mot de passe (min 8 chars, 1 majuscule, 1 chiffre)
function isStrongPassword(password) {
    const minLength = /.{8,}/;
    const hasUpperCase = /[A-Z]/;
    const hasNumber = /\d/;
    
    return minLength.test(password) && 
           hasUpperCase.test(password) && 
           hasNumber.test(password);
}

console.log(isStrongPassword('Pass123')); // false (trop court)
console.log(isStrongPassword('password123')); // false (pas de majuscule)
console.log(isStrongPassword('Password123')); // true
```

### exec() - Retourne array avec détails

```javascript
// exec() retourne un array avec les détails du match
const regex1 = /(\w+)@(\w+)\.(\w+)/;
const result1 = regex1.exec('user@example.com');

console.log(result1[0]); // "user@example.com" (match complet)
console.log(result1[1]); // "user" (groupe 1)
console.log(result1[2]); // "example" (groupe 2)
console.log(result1[3]); // "com" (groupe 3)
console.log(result1.index); // 0 (position du match)
console.log(result1.input); // "user@example.com" (string originale)

// Avec flag global (g) - itération
const text2 = 'cat dog cat bird cat';
const regex2 = /cat/g;
let match;

while ((match = regex2.exec(text2)) !== null) {
    console.log(`Trouvé "${match[0]}" à l'index ${match.index}`);
}
// Output:
// Trouvé "cat" à l'index 0
// Trouvé "cat" à l'index 8
// Trouvé "cat" à l'index 17

// Exemple pratique: Parser des URLs
function parseUrl(url) {
    const regex = /^(https?):\/\/([^\/]+)(\/.*)?$/;
    const match = regex.exec(url);
    
    if (match) {
        return {
            protocol: match[1],
            domain: match[2],
            path: match[3] || '/'
        };
    }
    return null;
}

console.log(parseUrl('https://example.com/path/to/page'));
// { protocol: 'https', domain: 'example.com', path: '/path/to/page' }
```

### String Methods avec RegEx

#### match() - Trouve les matches

```javascript
// match() sans flag g - retourne comme exec()
const text1 = 'My email is user@example.com';
const regex1 = /(\w+)@(\w+)\.(\w+)/;
const result1 = text1.match(regex1);
console.log(result1[0]); // "user@example.com"
console.log(result1[1]); // "user"

// match() avec flag g - retourne array de tous les matches
const text2 = 'cat dog cat bird cat';
const regex2 = /cat/g;
const result2 = text2.match(regex2);
console.log(result2); // ["cat", "cat", "cat"]

// Si aucun match
const result3 = 'hello'.match(/xyz/);
console.log(result3); // null

// Exemple pratique: Extraire tous les nombres
const text3 = 'I have 3 cats, 2 dogs, and 15 birds';
const numbers = text3.match(/\d+/g);
console.log(numbers); // ["3", "2", "15"]

// Extraire tous les hashtags
const tweet = 'Hello #javascript #coding #webdev';
const hashtags = tweet.match(/#\w+/g);
console.log(hashtags); // ["#javascript", "#coding", "#webdev"]
```

#### matchAll() - Trouve tous les matches avec détails

```javascript
// matchAll() retourne un iterator avec tous les matches (groupes inclus)
// Nécessite flag g
const text = 'user1@example.com, user2@test.com';
const regex = /(\w+)@(\w+)\.(\w+)/g;
const matches = text.matchAll(regex);

for (const match of matches) {
    console.log(`Email: ${match[0]}`);
    console.log(`Username: ${match[1]}`);
    console.log(`Domain: ${match[2]}`);
    console.log(`TLD: ${match[3]}`);
    console.log('---');
}

// Convertir en array
const allMatches = [...text.matchAll(regex)];
console.log(allMatches.length); // 2

// Exemple pratique: Parser des dates
const text2 = 'Event 1: 2024-01-15, Event 2: 2024-02-20';
const dateRegex = /(\d{4})-(\d{2})-(\d{2})/g;
const dates = [...text2.matchAll(dateRegex)];

dates.forEach((match, i) => {
    console.log(`Date ${i + 1}:`);
    console.log(`  Année: ${match[1]}`);
    console.log(`  Mois: ${match[2]}`);
    console.log(`  Jour: ${match[3]}`);
});
```

#### search() - Trouve la position

```javascript
// search() retourne l'index de la première occurrence (-1 si non trouvé)
const text1 = 'Hello World';
console.log(text1.search(/World/)); // 6
console.log(text1.search(/xyz/));   // -1

// Case insensitive
console.log(text1.search(/world/i)); // 6

// Exemple pratique: Vérifier position d'un pattern
function findFirstDigit(str) {
    const index = str.search(/\d/);
    return index !== -1 ? index : 'No digit found';
}

console.log(findFirstDigit('abc123')); // 3
console.log(findFirstDigit('hello'));  // "No digit found"
```

#### replace() - Remplacer du texte

```javascript
// replace() basique
const text1 = 'I have a cat';
const result1 = text1.replace(/cat/, 'dog');
console.log(result1); // "I have a dog"

// Avec flag g (remplace toutes les occurrences)
const text2 = 'cat cat cat';
const result2 = text2.replace(/cat/g, 'dog');
console.log(result2); // "dog dog dog"

// Utiliser les groupes de capture ($1, $2, etc.)
const text3 = 'John Doe';
const result3 = text3.replace(/(\w+) (\w+)/, '$2, $1');
console.log(result3); // "Doe, John"

// Avec fonction callback
const text4 = 'I have 3 cats and 2 dogs';
const result4 = text4.replace(/\d+/g, (match) => {
    return parseInt(match) * 2;
});
console.log(result4); // "I have 6 cats and 4 dogs"

// Callback avec tous les paramètres
const text5 = 'user@example.com';
const result5 = text5.replace(
    /(\w+)@(\w+)\.(\w+)/, 
    (match, username, domain, tld, offset, string) => {
        console.log('Match complet:', match);
        console.log('Username:', username);
        console.log('Domain:', domain);
        console.log('TLD:', tld);
        console.log('Position:', offset);
        console.log('String originale:', string);
        return `${username} [at] ${domain} [dot] ${tld}`;
    }
);
console.log(result5); // "user [at] example [dot] com"

// Exemples pratiques

// 1. Masquer numéros de carte bancaire
function maskCreditCard(number) {
    return number.replace(/\d(?=\d{4})/g, '*');
}
console.log(maskCreditCard('1234567890123456')); 
// "************3456"

// 2. Formater numéro de téléphone
function formatPhone(phone) {
    return phone.replace(/(\d{2})(\d{2})(\d{2})(\d{2})(\d{2})/, '$1 $2 $3 $4 $5');
}
console.log(formatPhone('0612345678')); 
// "06 12 34 56 78"

// 3. Capitaliser chaque mot
function capitalizeWords(text) {
    return text.replace(/\b\w/g, (char) => char.toUpperCase());
}
console.log(capitalizeWords('hello world from javascript')); 
// "Hello World From Javascript"

// 4. Supprimer les espaces multiples
function normalizeSpaces(text) {
    return text.replace(/\s+/g, ' ').trim();
}
console.log(normalizeSpaces('hello    world   test')); 
// "hello world test"

// 5. Convertir camelCase en kebab-case
function camelToKebab(str) {
    return str.replace(/([a-z])([A-Z])/g, '$1-$2').toLowerCase();
}
console.log(camelToKebab('myVariableName')); 
// "my-variable-name"
```

#### replaceAll() - Remplacer toutes les occurrences

```javascript
// replaceAll() - Alternative à replace() avec flag g
const text1 = 'cat cat cat';
const result1 = text1.replaceAll('cat', 'dog');
console.log(result1); // "dog dog dog"

// Avec RegEx (nécessite flag g)
const text2 = 'hello HELLO Hello';
const result2 = text2.replaceAll(/hello/gi, 'hi');
console.log(result2); // "hi hi hi"

// Exemple pratique: Nettoyer HTML
function stripHtmlTags(html) {
    return html.replaceAll(/<[^>]*>/g, '');
}
console.log(stripHtmlTags('<p>Hello <strong>World</strong></p>')); 
// "Hello World"
```

#### split() - Diviser une string

```javascript
// split() avec RegEx
const text1 = 'apple,banana;orange|grape';
const fruits = text1.split(/[,;|]/);
console.log(fruits); 
// ["apple", "banana", "orange", "grape"]

// Split sur espaces multiples
const text2 = 'hello    world   test';
const words = text2.split(/\s+/);
console.log(words); 
// ["hello", "world", "test"]

// Limiter le nombre de résultats
const text3 = 'a-b-c-d-e';
const parts = text3.split(/-/, 3);
console.log(parts); 
// ["a", "b", "c"]

// Exemples pratiques

// 1. Parser CSV
function parseCSV(line) {
    return line.split(/,(?=(?:(?:[^"]*"){2})*[^"]*$)/);
}

// 2. Split sur virgule ou point-virgule avec espaces optionnels
const list = 'item1, item2; item3 ,item4';
const items = list.split(/\s*[,;]\s*/);
console.log(items); 
// ["item1", "item2", "item3", "item4"]

// 3. Extraire mots (ignorer ponctuation)
const sentence = "Hello, World! How are you?";
const words2 = sentence.split(/\W+/).filter(w => w);
console.log(words2); 
// ["Hello", "World", "How", "are", "you"]
```

---

## Exemples Pratiques Complets

```javascript
// 1. VALIDATION EMAIL
function validateEmail(email) {
    const emailRegex = /^[a-zA-Z0-9._-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;
    return emailRegex.test(email);
}

console.log(validateEmail('user@example.com'));  // true
console.log(validateEmail('invalid@email'));     // false
console.log(validateEmail('test@test.co.uk'));   // true

// 2. VALIDATION MOT DE PASSE FORT
function validatePassword(password) {
    // Au moins 8 caractères, 1 majuscule, 1 minuscule, 1 chiffre, 1 caractère spécial
    const minLength = /.{8,}/;
    const hasUpperCase = /[A-Z]/;
    const hasLowerCase = /[a-z]/;
    const hasNumber = /\d/;
    const hasSpecialChar = /[!@#$%^&*(),.?":{}|<>]/;
    
    return minLength.test(password) &&
           hasUpperCase.test(password) &&
           hasLowerCase.test(password) &&
           hasNumber.test(password) &&
           hasSpecialChar.test(password);
}

console.log(validatePassword('Pass123!'));    // true
console.log(validatePassword('weak'));        // false

// 3. VALIDATION NUMÉRO DE TÉLÉPHONE FRANÇAIS
function validateFrenchPhone(phone) {
    // Format: 06 12 34 56 78 ou 0612345678
    const phoneRegex = /^(?:(?:\+|00)33|0)\s*[1-9](?:[\s.-]*\d{2}){4}$/;
    return phoneRegex.test(phone);
}

console.log(validateFrenchPhone('06 12 34 56 78')); // true
console.log(validateFrenchPhone('0612345678'));     // true
console.log(validateFrenchPhone('+33612345678'));   // true

// 4. EXTRAIRE URLS D'UN TEXTE
function extractUrls(text) {
    const urlRegex = /https?:\/\/[^\s]+/g;
    return text.match(urlRegex) || [];
}

const text = 'Visit https://example.com and http://test.com for more info';
console.log(extractUrls(text)); 
// ["https://example.com", "http://test.com"]

// 5. SLUGIFY (pour URLs friendly)
function slugify(text) {
    return text
        .toLowerCase()
        .trim()
        .replace(/[^\w\s-]/g, '')  // Supprimer caractères spéciaux
        .replace(/\s+/g, '-')       // Remplacer espaces par tirets
        .replace(/-+/g, '-');       // Supprimer tirets multiples
}

console.log(slugify('Hello World! This is a Test'));
// "hello-world-this-is-a-test"

// 6. VALIDATION CODE POSTAL FRANÇAIS
function validateFrenchZipCode(zip) {
    const zipRegex = /^[0-9]{5}$/;
    return zipRegex.test(zip);
}

console.log(validateFrenchZipCode('75001')); // true
console.log(validateFrenchZipCode('7500'));  // false

// 7. PARSER DATE (format DD/MM/YYYY)
function parseDate(dateStr) {
    const dateRegex = /^(\d{2})\/(\d{2})\/(\d{4})$/;
    const match = dateRegex.exec(dateStr);
    
    if (match) {
        return {
            day: match[1],
            month: match[2],
            year: match[3]
        };
    }
    return null;
}

console.log(parseDate('15/03/2024'));
// { day: '15', month: '03', year: '2024' }

// 8. CENSURER MOTS INTERDITS
function censorBadWords(text, badWords) {
    const pattern = new RegExp(badWords.join('|'), 'gi');
    return text.replace(pattern, (match) => '*'.repeat(match.length));
}

const text2 = 'This is a bad word and another bad word';
console.log(censorBadWords(text2, ['bad', 'another']));
// "This is a *** word and ******* *** word"

// 9. VALIDATION IBAN
function validateIBAN(iban) {
    // Simplifié - Format FR76 1234 5678 9012 3456 7890 123
    const ibanRegex = /^[A-Z]{2}\d{2}[\s]?(?:\d{4}[\s]?){5}\d{3}$/;
    return ibanRegex.test(iban.toUpperCase());
}

// 10. EXTRAIRE MENTIONS (@username) et HASHTAGS (#tag)
function extractMentionsAndHashtags(text) {
    const mentions = text.match(/@\w+/g) || [];
    const hashtags = text.match(/#\w+/g) || [];
    
    return {
        mentions: mentions.map(m => m.slice(1)), // Enlever @
        hashtags: hashtags.map(h => h.slice(1))  // Enlever #
    };
}

const tweet = 'Hello @john and @jane! Check out #javascript #coding';
console.log(extractMentionsAndHashtags(tweet));
// { mentions: ['john', 'jane'], hashtags: ['javascript', 'coding'] }
```

---

**Points clés à retenir:**
- Les RegEx sont puissantes mais peuvent être complexes
- Toujours tester les patterns avec différents cas
- Utiliser des outils en ligne (regex101.com) pour tester
- Performance: éviter les patterns trop gourmands
- Les RegEx en JavaScript ne supportent pas tous les features (lookbehind limité)


## Exemples Avancés et Patterns Complexes

### Lookahead et Lookbehind

```javascript
// POSITIVE LOOKAHEAD (?=...) - Match si suivi par...
// Trouver un mot suivi de "script"
const regex1 = /\w+(?=script)/;
console.log(regex1.exec('javascript')); // ["java"]
console.log(regex1.exec('typescript')); // ["type"]
console.log(regex1.exec('python'));     // null

// Validation mot de passe avec lookahead
// Au moins 8 chars, 1 majuscule, 1 minuscule, 1 chiffre
function validatePasswordAdvanced(password) {
    const regex = /^(?=.*[a-z])(?=.*[A-Z])(?=.*\d).{8,}$/;
    return regex.test(password);
}

console.log(validatePasswordAdvanced('Pass123'));   // false (trop court)
console.log(validatePasswordAdvanced('Password1')); // true
console.log(validatePasswordAdvanced('password1')); // false (pas de majuscule)

// NEGATIVE LOOKAHEAD (?!...) - Match si PAS suivi par...
// Trouver un mot qui ne se termine pas par "ing"
const regex2 = /\w+(?!ing)\b/;
console.log(regex2.test('running')); // false
console.log(regex2.test('walk'));    // true

// POSITIVE LOOKBEHIND (?<=...) - Match si précédé par...
// Trouver les chiffres après un symbole $
const regex3 = /(?<=\$)\d+/;
console.log(regex3.exec('Price: $100')); // ["100"]
console.log(regex3.exec('Price: 100'));  // null

// NEGATIVE LOOKBEHIND (?<!...) - Match si PAS précédé par...
// Trouver les chiffres qui ne sont pas précédés de $
const regex4 = /(?<!\$)\d+/;
console.log(regex4.exec('Price: $100')); // null (ou trouve autre chose)
console.log(regex4.exec('Price: 100'));  // ["100"]

// Exemple pratique: Extraire prix sans symbole monétaire
function extractPrices(text) {
    const priceRegex = /(?<=\$)\d+(?:\.\d{2})?/g;
    return text.match(priceRegex) || [];
}

console.log(extractPrices('Items: $19.99, $5.50, and $100'));
// ["19.99", "5.50", "100"]
```

### Named Capture Groups

```javascript
// Named groups (?<name>...) - Plus lisible que les groupes numérotés
const dateRegex = /(?<day>\d{2})\/(?<month>\d{2})\/(?<year>\d{4})/;
const match = dateRegex.exec('25/12/2024');

console.log(match.groups.day);   // "25"
console.log(match.groups.month); // "12"
console.log(match.groups.year);  // "2024"

// Utilisation dans replace()
const text1 = '25/12/2024';
const formatted = text1.replace(
    /(?<day>\d{2})\/(?<month>\d{2})\/(?<year>\d{4})/,
    '$<year>-$<month>-$<day>'
);
console.log(formatted); // "2024-12-25"

// Exemple pratique: Parser une URL complète
function parseURL(url) {
    const urlRegex = /^(?<protocol>https?):\/\/(?<domain>[^\/]+)(?<path>\/[^?]*)?(?<query>\?[^#]*)?(?<hash>#.*)?$/;
    const match = urlRegex.exec(url);
    
    if (match) {
        return match.groups;
    }
    return null;
}

const urlInfo = parseURL('https://example.com/path?id=123#section');
console.log(urlInfo);
// {
//   protocol: 'https',
//   domain: 'example.com',
//   path: '/path',
//   query: '?id=123',
//   hash: '#section'
// }

// Parser un email avec named groups
function parseEmail(email) {
    const emailRegex = /^(?<username>[^@]+)@(?<domain>[^.]+)\.(?<tld>.+)$/;
    const match = emailRegex.exec(email);
    return match ? match.groups : null;
}

console.log(parseEmail('john.doe@example.com'));
// { username: 'john.doe', domain: 'example', tld: 'com' }
```

### Greedy vs Lazy Matching

```javascript
// GREEDY (par défaut) - Match le plus possible
const text1 = '<div>Content 1</div><div>Content 2</div>';
const greedyRegex = /<div>.*<\/div>/;
console.log(text1.match(greedyRegex)[0]);
// "<div>Content 1</div><div>Content 2</div>" (tout!)

// LAZY (avec ?) - Match le moins possible
const lazyRegex = /<div>.*?<\/div>/g;
console.log(text1.match(lazyRegex));
// ["<div>Content 1</div>", "<div>Content 2</div>"]

// Autres quantifiers lazy
// +? - 1 ou plus (lazy)
// *? - 0 ou plus (lazy)
// {n,}? - Au moins n (lazy)
// {n,m}? - Entre n et m (lazy)

// Exemple pratique: Extraire contenu entre guillemets
const text2 = 'He said "Hello" and she said "Goodbye"';
const greedyQuotes = /".*"/;
const lazyQuotes = /".*?"/g;

console.log(text2.match(greedyQuotes)[0]);
// ""Hello" and she said "Goodbye"" (greedy)

console.log(text2.match(lazyQuotes));
// [""Hello"", ""Goodbye""] (lazy)

// Extraire attributs HTML
function extractAttributes(html) {
    const attrRegex = /(\w+)="(.*?)"/g;
    const attributes = {};
    let match;
    
    while ((match = attrRegex.exec(html)) !== null) {
        attributes[match[1]] = match[2];
    }
    
    return attributes;
}

const html = '<div class="container" id="main" data-value="test">';
console.log(extractAttributes(html));
// { class: 'container', id: 'main', data-value: 'test' }
```

### Word Boundaries

```javascript
// \b - Word boundary (début ou fin de mot)
const regex1 = /\bcat\b/;
console.log(regex1.test('cat'));        // true
console.log(regex1.test('The cat sat')); // true
console.log(regex1.test('category'));   // false
console.log(regex1.test('scat'));       // false

// \B - Non-word boundary
const regex2 = /\Bcat\B/;
console.log(regex2.test('concatenate')); // true
console.log(regex2.test('cat'));         // false

// Exemple pratique: Remplacer un mot entier
function replaceWholeWord(text, word, replacement) {
    const regex = new RegExp(`\\b${word}\\b`, 'gi');
    return text.replace(regex, replacement);
}

console.log(replaceWholeWord('The cat and the category', 'cat', 'dog'));
// "The dog and the category" (ne remplace pas dans "category")

// Recherche de mots complets
function findCompleteWords(text, searchWord) {
    const regex = new RegExp(`\\b${searchWord}\\b`, 'gi');
    return text.match(regex) || [];
}

console.log(findCompleteWords('cat cats scatter', 'cat'));
// ["cat"] (uniquement le mot complet)
```

### Backreferences (Références arrière)

```javascript
// \1, \2, etc. - Référence aux groupes capturés
// Trouver mots répétés
const regex1 = /\b(\w+)\s+\1\b/;
console.log(regex1.test('hello hello')); // true
console.log(regex1.test('hello world')); // false

// Trouver balises HTML matching
const regex2 = /<(\w+)>.*?<\/\1>/;
console.log(regex2.test('<div>content</div>'));  // true
console.log(regex2.test('<div>content</span>')); // false

// Avec named groups
const regex3 = /(?<tag>\w+)>.*?<\/\k<tag>/;
console.log(regex3.test('div>content</div>')); // true

// Exemple pratique: Trouver mots dupliqués dans un texte
function findDuplicateWords(text) {
    const regex = /\b(\w+)\s+\1\b/gi;
    return text.match(regex) || [];
}

console.log(findDuplicateWords('The the cat sat sat on the mat'));
// ["The the", "sat sat"]

// Corriger les mots dupliqués
function removeDuplicateWords(text) {
    return text.replace(/\b(\w+)\s+\1\b/gi, '$1');
}

console.log(removeDuplicateWords('The the cat sat sat on the mat'));
// "The cat sat on the mat"

// Valider palindrome avec backreference
function isPalindrome(word) {
    const len = Math.floor(word.length / 2);
    let pattern = '^';
    
    for (let i = 0; i < len; i++) {
        pattern += '(.)';
    }
    
    if (word.length % 2 === 1) {
        pattern += '.';
    }
    
    for (let i = len; i > 0; i--) {
        pattern += `\\${i}`;
    }
    
    pattern += '$';
    
    const regex = new RegExp(pattern);
    return regex.test(word);
}

console.log(isPalindrome('radar')); // true
console.log(isPalindrome('hello')); // false
```

### Cas d'Usage Réels et Complexes

```javascript
// 1. VALIDATION CARTE BANCAIRE (Luhn Algorithm avec RegEx pour format)
function validateCreditCard(number) {
    // Supprimer espaces et tirets
    number = number.replace(/[\s-]/g, '');
    
    // Vérifier format (13-19 chiffres)
    if (!/^\d{13,19}$/.test(number)) {
        return false;
    }
    
    // Identifier le type de carte
    const cardTypes = {
        visa: /^4/,
        mastercard: /^5[1-5]/,
        amex: /^3[47]/,
        discover: /^6(?:011|5)/
    };
    
    let cardType = 'unknown';
    for (let [type, pattern] of Object.entries(cardTypes)) {
        if (pattern.test(number)) {
            cardType = type;
            break;
        }
    }
    
    return { valid: true, type: cardType };
}

console.log(validateCreditCard('4532 1234 5678 9010'));
// { valid: true, type: 'visa' }

// 2. PARSER MARKDOWN
function parseMarkdown(text) {
    return text
        // Headers
        .replace(/^### (.*$)/gm, '<h3>$1</h3>')
        .replace(/^## (.*$)/gm, '<h2>$1</h2>')
        .replace(/^# (.*$)/gm, '<h1>$1</h1>')
        // Bold
        .replace(/\*\*(.*?)\*\*/g, '<strong>$1</strong>')
        // Italic
        .replace(/\*(.*?)\*/g, '<em>$1</em>')
        // Links
        .replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<a href="$2">$1</a>')
        // Code inline
        .replace(/`([^`]+)`/g, '<code>$1</code>')
        // Line breaks
        .replace(/\n/g, '<br>');
}

const markdown = '# Title\n## Subtitle\nThis is **bold** and *italic*\n[Link](http://example.com)';
console.log(parseMarkdown(markdown));

// 3. VALIDATION SÉCURITÉ SQL INJECTION
function sanitizeSQL(input) {
    // Détecter patterns SQL dangereux
    const sqlInjectionPatterns = [
        /(\bOR\b|\bAND\b).*?[=<>]/i,
        /UNION.*?SELECT/i,
        /DROP.*?TABLE/i,
        /INSERT.*?INTO/i,
        /DELETE.*?FROM/i,
        /--/,
        /;.*?--/,
        /\/\*/
    ];
    
    for (let pattern of sqlInjectionPatterns) {
        if (pattern.test(input)) {
            console.warn('Potential SQL injection detected!');
            return false;
        }
    }
    
    return true;
}

console.log(sanitizeSQL("admin' OR '1'='1")); // false
console.log(sanitizeSQL("john.doe"));          // true

// 4. PARSER FICHIER CSV COMPLEXE (avec guillemets et virgules dans les valeurs)
function parseCSVLine(line) {
    const regex = /(?:,|^)(?:"([^"]*)"|([^",]*))/g;
    const values = [];
    let match;
    
    while ((match = regex.exec(line)) !== null) {
        values.push(match[1] !== undefined ? match[1] : match[2]);
    }
    
    return values;
}

const csvLine = 'John,"Doe, Jr.",30,"New York, NY",Engineer';
console.log(parseCSVLine(csvLine));
// ["John", "Doe, Jr.", "30", "New York, NY", "Engineer"]

// 5. EXTRAIRE ET VALIDER ADRESSES IP
function extractIPAddresses(text) {
    const ipv4Regex = /\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b/g;
    return text.match(ipv4Regex) || [];
}

const text = 'Server IPs: 192.168.1.1, 10.0.0.1, 256.1.1.1 (invalid)';
console.log(extractIPAddresses(text));
// ["192.168.1.1", "10.0.0.1"]

// 6. VALIDATION ET PARSING DE COULEURS HEX/RGB
function parseColor(color) {
    // HEX
    const hexRegex = /^#?([a-f0-9]{6}|[a-f0-9]{3})$/i;
    const hexMatch = hexRegex.exec(color);
    
    if (hexMatch) {
        return { type: 'hex', value: hexMatch[1] };
    }
    
    // RGB
    const rgbRegex = /^rgb\(\s*(\d{1,3})\s*,\s*(\d{1,3})\s*,\s*(\d{1,3})\s*\)$/;
    const rgbMatch = rgbRegex.exec(color);
    
    if (rgbMatch) {
        const [, r, g, b] = rgbMatch;
        if (r <= 255 && g <= 255 && b <= 255) {
            return { type: 'rgb', r, g, b };
        }
    }
    
    return null;
}

console.log(parseColor('#ff5733'));        // { type: 'hex', value: 'ff5733' }
console.log(parseColor('rgb(255, 87, 51)')); // { type: 'rgb', r: '255', g: '87', b: '51' }

// 7. MASQUER DONNÉES SENSIBLES
function maskSensitiveData(text) {
    return text
        // Masquer emails (garder 2 premiers chars et domaine)
        .replace(/(\w{2})\w+@/g, '$1***@')
        // Masquer numéros de téléphone
        .replace(/\b(\d{2})\d{6}(\d{2})\b/g, '$1******$2')
        // Masquer cartes bancaires
        .replace(/\b(\d{4})\s?\d{4}\s?\d{4}\s?(\d{4})\b/g, '$1 **** **** $2');
}

const sensitive = 'Contact: john.doe@example.com, 0612345678, Card: 1234 5678 9012 3456';
console.log(maskSensitiveData(sensitive));
// "Contact: jo***@example.com, 06******78, Card: 1234 **** **** 3456"

// 8. VALIDATION FORMAT DE FICHIER
function validateFileName(filename) {
    const validExtensions = ['jpg', 'jpeg', 'png', 'gif', 'pdf', 'doc', 'docx'];
    const regex = new RegExp(`^[\\w-]+\\.(${validExtensions.join('|')})$`, 'i');
    return regex.test(filename);
}

console.log(validateFileName('document.pdf'));  // true
console.log(validateFileName('image.jpg'));     // true
console.log(validateFileName('file.exe'));      // false
console.log(validateFileName('../hack.php'));   // false

// 9. EXTRAIRE VERSIONS DE LOGICIEL (Semantic Versioning)
function extractVersions(text) {
    const versionRegex = /\bv?(\d+)\.(\d+)\.(\d+)(?:-([a-z0-9.-]+))?(?:\+([a-z0-9.-]+))?\b/gi;
    const versions = [];
    let match;
    
    while ((match = versionRegex.exec(text)) !== null) {
        versions.push({
            full: match[0],
            major: match[1],
            minor: match[2],
            patch: match[3],
            prerelease: match[4] || null,
            build: match[5] || null
        });
    }
    
    return versions;
}

const releaseNotes = 'Released v1.2.3, v2.0.0-beta.1, and v3.1.0+build.123';
console.log(extractVersions(releaseNotes));

// 10. DÉTECTION DE LANGUE DANS UN TEXTE (simple)
function detectLanguage(text) {
    const patterns = {
        french: /\b(le|la|les|un|une|des|et|à|de|en)\b/gi,
        english: /\b(the|a|an|and|to|of|in|is|it)\b/gi,
        spanish: /\b(el|la|los|las|un|una|y|de|en)\b/gi
    };
    
    const scores = {};
    
    for (let [lang, pattern] of Object.entries(patterns)) {
        const matches = text.match(pattern);
        scores[lang] = matches ? matches.length : 0;
    }
    
    return Object.keys(scores).reduce((a, b) => 
        scores[a] > scores[b] ? a : b
    );
}

console.log(detectLanguage('The cat is on the table')); // "english"
console.log(detectLanguage('Le chat est sur la table')); // "french"
```

### Performance et Bonnes Pratiques

```javascript
// ❌ MAUVAISE PRATIQUE: Recréer regex à chaque appel
function validateBad(email) {
    return /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email);
}

// ✅ BONNE PRATIQUE: Réutiliser regex
const EMAIL_REGEX = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
function validateGood(email) {
    return EMAIL_REGEX.test(email);
}

// ❌ CATASTROPHIC BACKTRACKING (peut freezer le navigateur)
const badRegex = /^(a+)+$/;
// badRegex.test('aaaaaaaaaaaaaaaaaaaaaaaaaaab'); // Très lent!

// ✅ Éviter les patterns gourmands imbriqués
const goodRegex = /^a+$/;

// TIPS PERFORMANCE:
// 1. Être spécifique (éviter .* si possible)
// 2. Utiliser anchors (^, $) quand possible
// 3. Éviter lookahead/lookbehind complexes
// 4. Compiler les regex réutilisées
// 5. Tester avec des inputs longs

// Mesurer performance
function measureRegexPerformance(regex, testString, iterations = 10000) {
    console.time('regex-test');
    for (let i = 0; i < iterations; i++) {
        regex.test(testString);
    }
    console.timeEnd('regex-test');
}

// BONNES PRATIQUES GÉNÉRALES:
// 1. Commenter les regex complexes
const phoneRegex = /^(\d{2})  # Code pays
                    [\s.-]?     # Séparateur optionnel
                    (\d{2})     # Premier groupe
                    /x;         // Mode verbose (pas supporté en JS natif)

// En JS, utiliser des commentaires standard
const phoneRegexJS = 
    /^(\d{2})/ +     // Code pays
    /[\s.-]?/ +      // Séparateur optionnel
    /(\d{2})/;       // Premier groupe

// 2. Créer des fonctions réutilisables
const validators = {
    email: /^[^\s@]+@[^\s@]+\.[^\s@]+$/,
    phone: /^(?:(?:\+|00)33|0)\s*[1-9](?:[\s.-]*\d{2}){4}$/,
    url: /^https?:\/\/[^\s]+$/
};

function validate(type, value) {
    return validators[type]?.test(value) || false;
}

// 3. Documenter avec des exemples
/**
 * Valide un numéro de téléphone français
 * @param {string} phone - Le numéro à valider
 * @returns {boolean}
 * @example
 *   validatePhone('06 12 34 56 78') // true
 *   validatePhone('0612345678')     // true
 *   validatePhone('+33612345678')   // true
 */
function validatePhone(phone) {
    return /^(?:(?:\+|00)33|0)\s*[1-9](?:[\s.-]*\d{2}){4}$/.test(phone);
}
```

---

**Points clés à retenir pour React:**
- Les RegEx sont essentielles pour la validation de formulaires
- Créer des custom hooks pour validation réutilisable
- Attention à la performance dans les `onChange` handlers
- Utiliser des bibliothèques comme `yup` ou `zod` pour validation complexe
- Les RegEx sont synchrones (pas de problème avec le rendering)

**Ressources:**
- **regex101.com** - Tester et débugger les regex en temps réel
- **regexr.com** - Visualiser et comprendre les patterns
- **MDN RegEx Guide** - Documentation complète

---

# Questions/Réponses - Chapitre 11 : RegEx

## Questions Niveau Junior

### Q1: Qu'est-ce qu'une expression régulière (RegEx) et à quoi sert-elle ?

**Réponse:**
```javascript
// Une RegEx est un pattern (motif) utilisé pour rechercher 
// et manipuler du texte

// Cas d'usage principaux:
// 1. Validation (email, téléphone, mot de passe)
const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
console.log(emailRegex.test('user@example.com')); // true

// 2. Recherche
const text = 'Mon numéro est 0612345678';
const hasPhone = /\d{10}/.test(text); // true

// 3. Extraction
const numbers = 'J\'ai 3 chats et 2 chiens'.match(/\d+/g);
console.log(numbers); // ["3", "2"]

// 4. Remplacement
const cleaned = 'hello   world'.replace(/\s+/g, ' ');
console.log(cleaned); // "hello world"
```

**Points clés:**
- Pattern matching pour texte
- Utilisé pour: validation, recherche, extraction, remplacement
- Deux syntaxes: literal `/pattern/` et constructor `new RegExp()`
- Très utilisé dans les formulaires React

---

### Q2: Quelle est la différence entre `/pattern/` et `/pattern/g` ?

**Réponse:**
```javascript
// Sans flag 'g' (global) - Trouve seulement la PREMIÈRE occurrence
const text = 'cat cat cat';
const regex1 = /cat/;
console.log(text.match(regex1)); 
// ["cat"] - Une seule occurrence

// Avec flag 'g' (global) - Trouve TOUTES les occurrences
const regex2 = /cat/g;
console.log(text.match(regex2)); 
// ["cat", "cat", "cat"] - Toutes les occurrences

// Autre différence avec replace()
console.log(text.replace(/cat/, 'dog')); 
// "dog cat cat" - Remplace la première

console.log(text.replace(/cat/g, 'dog')); 
// "dog dog dog" - Remplace toutes

// En pratique
function countOccurrences(text, word) {
    const regex = new RegExp(word, 'g');
    const matches = text.match(regex);
    return matches ? matches.length : 0;
}

console.log(countOccurrences('hello world hello', 'hello')); // 2
```

**Points clés:**
- `g` = global = toutes les occurrences
- Sans `g` = seulement la première occurrence
- Important pour `match()` et `replace()`
- Autres flags: `i` (case insensitive), `m` (multiline)

---

### Q3: Comment valider un email avec une RegEx ?

**Réponse:**
```javascript
// Version simple (basique mais fonctionnelle)
function validateEmail(email) {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailRegex.test(email);
}

console.log(validateEmail('user@example.com'));  // true
console.log(validateEmail('invalid.email'));     // false
console.log(validateEmail('user@domain'));       // false

// Explication du pattern:
// ^           - Début de la chaîne
// [^\s@]+     - Un ou plusieurs caractères (pas espace ni @)
// @           - Le symbole @ (literal)
// [^\s@]+     - Un ou plusieurs caractères (pas espace ni @)
// \.          - Un point (escaped car . est spécial)
// [^\s@]+     - Un ou plusieurs caractères
// $           - Fin de la chaîne

// Version plus stricte
function validateEmailStrict(email) {
    const emailRegex = /^[a-zA-Z0-9._-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;
    return emailRegex.test(email);
}

console.log(validateEmailStrict('user@example.com')); // true
console.log(validateEmailStrict('user+tag@test.co.uk')); // false

// Version complète (accepte plus de formats)
function validateEmailComplete(email) {
    const emailRegex = /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;
    return emailRegex.test(email);
}

console.log(validateEmailComplete('user+tag@example.com')); // true
```

**Points clés:**
- Pattern basique: `texte@domaine.extension`
- `^` et `$` pour match exact (début et fin)
- `[^\s@]+` signifie "tout sauf espace et @"
- `\.` pour le point literal (escape nécessaire)
- En production, utiliser des librairies (validator.js)

---

### Q4: Comment utiliser `test()` et `match()` ? Quelle est la différence ?

**Réponse:**
```javascript
// test() - Retourne boolean (true/false)
const regex1 = /hello/i;
console.log(regex1.test('Hello World')); // true
console.log(regex1.test('Goodbye'));     // false

// Utilisation: vérifier si un pattern existe
function containsDigit(text) {
    return /\d/.test(text);
}

console.log(containsDigit('abc123')); // true
console.log(containsDigit('abc'));    // false

// match() - Retourne array avec les matches (ou null)
const text = 'I have 3 cats and 2 dogs';
const numbers = text.match(/\d+/g);
console.log(numbers); // ["3", "2"]

// Sans flag g - retourne détails du premier match
const email = 'Contact: user@example.com';
const match = email.match(/(\w+)@(\w+)\.(\w+)/);
console.log(match[0]); // "user@example.com" (match complet)
console.log(match[1]); // "user" (groupe 1)
console.log(match[2]); // "example" (groupe 2)
console.log(match[3]); // "com" (groupe 3)

// Si pas de match
const noMatch = 'hello'.match(/xyz/);
console.log(noMatch); // null

// QUAND UTILISER QUOI:
// - test() → Juste vérifier si ça existe (validation)
// - match() → Extraire les valeurs trouvées

// Exemple pratique
function extractPhoneNumbers(text) {
    const phoneRegex = /\d{10}/g;
    const matches = text.match(phoneRegex);
    return matches || []; // Retourner array vide si null
}

console.log(extractPhoneNumbers('Call 0612345678 or 0698765432'));
// ["0612345678", "0698765432"]
```

**Points clés:**
- `test()`: retourne `true`/`false` (validation)
- `match()`: retourne array ou `null` (extraction)
- `test()` est plus performant si on veut juste valider
- `match()` avec `g` retourne tous les matches

---

### Q5: Que signifient `\d`, `\w`, et `\s` dans les RegEx ?

**Réponse:**
```javascript
// \d - Digit (chiffre de 0 à 9)
const hasDigit = /\d/.test('abc123'); 
console.log(hasDigit); // true

// Équivalent à [0-9]
console.log(/\d+/.test('2024'));   // true
console.log(/\d+/.test('hello'));  // false

// \D - Non-digit (tout sauf chiffre)
console.log(/\D/.test('123'));   // false
console.log(/\D/.test('12a3'));  // true

// \w - Word character (lettre, chiffre, underscore)
// Équivalent à [a-zA-Z0-9_]
console.log(/\w+/.test('hello'));    // true
console.log(/\w+/.test('test_123')); // true
console.log(/\w+/.test('@#$'));      // false

// \W - Non-word character
console.log(/\W/.test('hello'));  // false
console.log(/\W/.test('hello!')); // true (à cause de !)

// \s - Whitespace (espace, tab, newline)
console.log(/\s/.test('hello world')); // true
console.log(/\s/.test('hello'));       // false

// \S - Non-whitespace
console.log(/\S/.test('   '));  // false
console.log(/\S/.test(' a ')); // true

// Exemples pratiques

// Valider code postal (5 chiffres)
function isValidZipCode(zip) {
    return /^\d{5}$/.test(zip);
}
console.log(isValidZipCode('75001')); // true
console.log(isValidZipCode('7500'));  // false

// Valider username (lettres, chiffres, underscore)
function isValidUsername(username) {
    return /^\w{3,20}$/.test(username);
}
console.log(isValidUsername('john_doe')); // true
console.log(isValidUsername('jo'));       // false (trop court)
console.log(isValidUsername('user@123')); // false (@ non permis)

// Nettoyer espaces multiples
function cleanSpaces(text) {
    return text.replace(/\s+/g, ' ').trim();
}
console.log(cleanSpaces('hello    world   test'));
// "hello world test"

// Compter mots (séparés par whitespace)
function countWords(text) {
    const words = text.match(/\S+/g);
    return words ? words.length : 0;
}
console.log(countWords('Hello world from JavaScript')); // 4
```

**Points clés:**
- `\d` = chiffres, `\D` = non-chiffres
- `\w` = alphanumerique + underscore, `\W` = autres
- `\s` = espaces, `\S` = non-espaces
- Les majuscules sont les inverses (négation)
- Très utilisés pour validation et parsing

---

## Questions Niveau Intermédiaire

### Q6: Comment valider un mot de passe fort avec plusieurs critères ?

**Réponse:**
```javascript
// Approche 1: Tester chaque critère séparément (plus lisible)
function validatePasswordSeparate(password) {
    const minLength = password.length >= 8;
    const hasUpperCase = /[A-Z]/.test(password);
    const hasLowerCase = /[a-z]/.test(password);
    const hasNumber = /\d/.test(password);
    const hasSpecialChar = /[!@#$%^&*(),.?":{}|<>]/.test(password);
    
    const errors = [];
    if (!minLength) errors.push('Minimum 8 caractères');
    if (!hasUpperCase) errors.push('Au moins 1 majuscule');
    if (!hasLowerCase) errors.push('Au moins 1 minuscule');
    if (!hasNumber) errors.push('Au moins 1 chiffre');
    if (!hasSpecialChar) errors.push('Au moins 1 caractère spécial');
    
    return {
        valid: errors.length === 0,
        errors: errors
    };
}

console.log(validatePasswordSeparate('Pass123!'));
// { valid: true, errors: [] }

console.log(validatePasswordSeparate('weak'));
// { valid: false, errors: [...] }

// Approche 2: Lookahead (une seule regex, plus compacte)
function validatePasswordLookahead(password) {
    // (?=.*[a-z]) - doit contenir au moins 1 minuscule
    // (?=.*[A-Z]) - doit contenir au moins 1 majuscule
    // (?=.*\d) - doit contenir au moins 1 chiffre
    // (?=.*[!@#$%^&*]) - doit contenir au moins 1 caractère spécial
    // .{8,} - au moins 8 caractères
    
    const regex = /^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[!@#$%^&*]).{8,}$/;
    return regex.test(password);
}

console.log(validatePasswordLookahead('Password123!')); // true
console.log(validatePasswordLookahead('weak'));         // false
console.log(validatePasswordLookahead('Password'));     // false (pas de chiffre)

// Explication du Lookahead:
// (?=...) est un "positive lookahead"
// Il vérifie que le pattern existe QUELQUE PART dans la string
// mais ne "consomme" pas les caractères

// Approche 3: Fonction complète avec feedback
function validatePasswordComplete(password) {
    const checks = {
        minLength: {
            regex: /.{8,}/,
            message: 'Au moins 8 caractères'
        },
        uppercase: {
            regex: /[A-Z]/,
            message: 'Au moins une majuscule'
        },
        lowercase: {
            regex: /[a-z]/,
            message: 'Au moins une minuscule'
        },
        number: {
            regex: /\d/,
            message: 'Au moins un chiffre'
        },
        special: {
            regex: /[!@#$%^&*(),.?":{}|<>]/,
            message: 'Au moins un caractère spécial'
        }
    };
    
    const results = {};
    const errors = [];
    
    for (let [key, check] of Object.entries(checks)) {
        const passed = check.regex.test(password);
        results[key] = passed;
        if (!passed) {
            errors.push(check.message);
        }
    }
    
    return {
        valid: errors.length === 0,
        checks: results,
        errors: errors,
        strength: calculateStrength(results)
    };
}

function calculateStrength(checks) {
    const passed = Object.values(checks).filter(Boolean).length;
    if (passed === 5) return 'Fort';
    if (passed >= 3) return 'Moyen';
    return 'Faible';
}

console.log(validatePasswordComplete('Pass123!'));
// {
//   valid: true,
//   checks: { minLength: true, uppercase: true, ... },
//   errors: [],
//   strength: 'Fort'
// }
```

**Points clés:**
- Plusieurs approches possibles (séparée vs lookahead)
- Lookahead `(?=...)` vérifie sans consommer
- Approche séparée plus lisible et flexible
- Fournir feedback utilisateur détaillé
- Utiliser en React avec `useState` pour feedback temps réel

---

### Q7: Comment extraire toutes les URLs d'un texte ?

**Réponse:**
```javascript
// Version simple
function extractUrlsSimple(text) {
    const urlRegex = /https?:\/\/[^\s]+/g;
    return text.match(urlRegex) || [];
}

const text1 = 'Visit https://example.com and http://test.com for more';
console.log(extractUrlsSimple(text1));
// ["https://example.com", "http://test.com"]

// Version plus complète (avec validation)
function extractUrlsComplete(text) {
    // Explications:
    // https? - http ou https
    // :\/\/ - ://
    // [^\s]+ - tout sauf whitespace
    const urlRegex = /https?:\/\/[^\s<>'"]+/gi;
    return text.match(urlRegex) || [];
}

const text2 = 'Check https://example.com/path?id=123 and HTTP://TEST.COM';
console.log(extractUrlsComplete(text2));
// ["https://example.com/path?id=123", "HTTP://TEST.COM"]

// Version avancée (avec groupes pour parser)
function parseUrls(text) {
    const urlRegex = /(https?):\/\/([^\/\s]+)(\/[^\s]*)?/gi;
    const urls = [];
    let match;
    
    while ((match = urlRegex.exec(text)) !== null) {
        urls.push({
            full: match[0],
            protocol: match[1],
            domain: match[2],
            path: match[3] || '/'
        });
    }
    
    return urls;
}

const text3 = 'Visit https://example.com/page and http://test.com';
console.log(parseUrls(text3));
// [
//   { full: 'https://example.com/page', protocol: 'https', 
//     domain: 'example.com', path: '/page' },
//   { full: 'http://test.com', protocol: 'http', 
//     domain: 'test.com', path: '/' }
// ]

// Extraire et valider URLs
function extractValidUrls(text) {
    const urlRegex = /https?:\/\/[^\s<>'"]+/gi;
    const urls = text.match(urlRegex) || [];
    
    // Filtrer les URLs valides
    return urls.filter(url => {
        try {
            new URL(url);
            return true;
        } catch {
            return false;
        }
    });
}

// Cas pratique: Rendre les URLs cliquables dans un texte
function linkify(text) {
    const urlRegex = /(https?:\/\/[^\s<>'"]+)/gi;
    return text.replace(urlRegex, '<a href="$1" target="_blank">$1</a>');
}

const text4 = 'Visit https://example.com for info';
console.log(linkify(text4));
// 'Visit <a href="https://example.com" target="_blank">https://example.com</a> for info'
```

**Points clés:**
- Pattern basique: `https?:\/\/[^\s]+`
- Flag `g` pour trouver toutes les occurrences
- Utiliser `match()` pour extraire
- Parser avec groupes de capture pour détails
- Valider avec `new URL()` si nécessaire

---

### Q8: Comment utiliser `replace()` avec une fonction callback ?

**Réponse:**
```javascript
// Syntaxe basique avec callback
const text = 'I have 3 cats and 2 dogs';
const result = text.replace(/\d+/g, (match) => {
    return parseInt(match) * 2;
});
console.log(result); // "I have 6 cats and 4 dogs"

// Callback avec tous les paramètres
const text2 = 'user@example.com';
const masked = text2.replace(
    /(\w+)@(\w+)\.(\w+)/,
    (match, username, domain, tld, offset, string) => {
        console.log('Match complet:', match);     // "user@example.com"
        console.log('Username:', username);        // "user"
        console.log('Domain:', domain);            // "example"
        console.log('TLD:', tld);                  // "com"
        console.log('Position:', offset);          // 0
        console.log('String originale:', string);  // "user@example.com"
        
        return `${username[0]}***@${domain}.${tld}`;
    }
);
console.log(masked); // "u***@example.com"

// Exemple 1: Capitaliser chaque mot
function capitalizeWords(text) {
    return text.replace(/\b\w/g, (char) => char.toUpperCase());
}
console.log(capitalizeWords('hello world from javascript'));
// "Hello World From Javascript"

// Exemple 2: Formater numéros de téléphone
function formatPhone(phone) {
    // Supprimer tout sauf chiffres
    const cleaned = phone.replace(/\D/g, '');
    
    // Formatter
    return cleaned.replace(/(\d{2})(\d{2})(\d{2})(\d{2})(\d{2})/, 
        (match, g1, g2, g3, g4, g5) => {
            return `${g1} ${g2} ${g3} ${g4} ${g5}`;
        }
    );
}
console.log(formatPhone('0612345678')); // "06 12 34 56 78"

// Exemple 3: Encoder HTML entities
function encodeHTML(text) {
    const entities = {
        '<': '&lt;',
        '>': '&gt;',
        '&': '&amp;',
        '"': '&quot;',
        "'": '&#39;'
    };
    
    return text.replace(/[<>&"']/g, (char) => entities[char]);
}
console.log(encodeHTML('<div class="test">Hello & Goodbye</div>'));
// "&lt;div class=&quot;test&quot;&gt;Hello &amp; Goodbye&lt;/div&gt;"

// Exemple 4: Convertir Markdown en HTML
function convertMarkdown(text) {
    return text
        // Bold **text**
        .replace(/\*\*(.+?)\*\*/g, '<strong>$1</strong>')
        // Italic *text*
        .replace(/\*(.+?)\*/g, '<em>$1</em>')
        // Links [text](url)
        .replace(/\[([^\]]+)\]\(([^)]+)\)/g, 
            '<a href="$2">$1</a>');
}
console.log(convertMarkdown('This is **bold** and *italic* with [link](url)'));
// "This is <strong>bold</strong> and <em>italic</em> with <a href="url">link</a>"

// Exemple 5: Obfusquer données sensibles
function obfuscateData(text) {
    return text
        // Emails
        .replace(/(\w{2})\w+@/g, (match, start) => {
            return `${start}***@`;
        })
        // Téléphones
        .replace(/\b(\d{2})\d{6}(\d{2})\b/g, '$1******$2')
        // Cartes bancaires
        .replace(/\b(\d{4})\s?\d{4}\s?\d{4}\s?(\d{4})\b/g, 
            '$1 **** **** $2');
}

const sensitive = 'Email: john.doe@test.com, Phone: 0612345678, Card: 1234 5678 9012 3456';
console.log(obfuscateData(sensitive));
// "Email: jo***@test.com, Phone: 06******78, Card: 1234 **** **** 3456"

// Exemple 6: Convertir camelCase en kebab-case
function camelToKebab(str) {
    return str.replace(/([a-z])([A-Z])/g, (match, lower, upper) => {
        return `${lower}-${upper.toLowerCase()}`;
    });
}
console.log(camelToKebab('myVariableName')); // "my-variable-name"
console.log(camelToKebab('backgroundColor')); // "background-color"

// Exemple 7: Template string simple
function template(str, data) {
    return str.replace(/\{(\w+)\}/g, (match, key) => {
        return data[key] || match;
    });
}

const message = 'Hello {name}, you have {count} messages';
const filled = template(message, { name: 'John', count: 5 });
console.log(filled); // "Hello John, you have 5 messages"
```

**Points clés:**
- Callback reçoit: `match, group1, group2, ..., offset, string`
- Permet transformations complexes
- Plus flexible que `$1`, `$2` pour replacements simples
- Utile pour: formatting, encoding, transformations dynamiques
- En React: utile pour sanitization et formatting

---

### Q9: Comment valider et formater un numéro de téléphone français ?

**Réponse:**
```javascript
// Validation simple
function isValidFrenchPhone(phone) {
    // Format: 06 12 34 56 78 ou 0612345678 ou +33612345678
    const phoneRegex = /^(?:(?:\+|00)33|0)\s*[1-9](?:[\s.-]*\d{2}){4}$/;
    return phoneRegex.test(phone);
}

console.log(isValidFrenchPhone('06 12 34 56 78')); // true
console.log(isValidFrenchPhone('0612345678'));     // true
console.log(isValidFrenchPhone('+33612345678'));   // true
console.log(isValidFrenchPhone('00 33 6 12 34 56 78')); // true
console.log(isValidFrenchPhone('1234567'));        // false

// Explication du pattern:
// ^                  - Début
// (?:(?:\+|00)33|0)  - +33 ou 0033 ou 0
// \s*                - Espaces optionnels
// [1-9]              - Premier chiffre (1-9, pas 0)
// (?:[\s.-]*\d{2}){4} - 4 groupes de 2 chiffres avec séparateurs optionnels
// $                  - Fin

// Normaliser (enlever espaces et formater)
function normalizePhone(phone) {
    // Enlever tous les espaces, points, tirets
    const cleaned = phone.replace(/[\s.-]/g, '');
    
    // Convertir format international en national
    const national = cleaned.replace(/^(?:\+|00)33/, '0');
    
    return national;
}

console.log(normalizePhone('+33 6 12 34 56 78')); // "0612345678"
console.log(normalizePhone('06.12.34.56.78'));    // "0612345678"

// Formater pour affichage
function formatPhoneDisplay(phone) {
    // Normaliser d'abord
    const normalized = normalizePhone(phone);
    
    // Vérifier si valide
    if (!/^0[1-9]\d{8}$/.test(normalized)) {
        return null; // Invalide
    }
    
    // Formater: 06 12 34 56 78
    return normalized.replace(/(\d{2})(\d{2})(\d{2})(\d{2})(\d{2})/, 
        '$1 $2 $3 $4 $5');
}

console.log(formatPhoneDisplay('0612345678'));      // "06 12 34 56 78"
console.log(formatPhoneDisplay('+33612345678'));    // "06 12 34 56 78"
console.log(formatPhoneDisplay('06.12.34.56.78'));  // "06 12 34 56 78"

// Formater pour stockage/API (format international)
function formatPhoneInternational(phone) {
    const normalized = normalizePhone(phone);
    
    if (!/^0[1-9]\d{8}$/.test(normalized)) {
        return null;
    }
    
    // Convertir en format international
    return '+33' + normalized.substring(1);
}

console.log(formatPhoneInternational('06 12 34 56 78')); // "+33612345678"

// Fonction complète avec validation et formatting
function processPhone(phone, format = 'display') {
    // Normaliser
    const normalized = normalizePhone(phone);
    
    // Valider
    const isValid = /^0[1-9]\d{8}$/.test(normalized);
    
    if (!isValid) {
        return {
            valid: false,
            original: phone,
            error: 'Format invalide'
        };
    }
    
    // Formater selon le besoin
    let formatted;
    switch (format) {
        case 'display':
            formatted = normalized.replace(/(\d{2})(\d{2})(\d{2})(\d{2})(\d{2})/, 
                '$1 $2 $3 $4 $5');
            break;
        case 'international':
            formatted = '+33' + normalized.substring(1);
            break;
        case 'raw':
            formatted = normalized;
            break;
        default:
            formatted = normalized;
    }
    
    return {
        valid: true,
        original: phone,
        normalized: normalized,
        formatted: formatted,
        type: getPhoneType(normalized)
    };
}

function getPhoneType(phone) {
    const firstDigits = phone.substring(0, 2);
    const types = {
        '01': 'Fixe Île-de-France',
        '02': 'Fixe Nord-Ouest',
        '03': 'Fixe Nord-Est',
        '04': 'Fixe Sud-Est',
        '05': 'Fixe Sud-Ouest',
        '06': 'Mobile',
        '07': 'Mobile',
        '08': 'Numéro spécial',
        '09': 'Voix sur IP'
    };
    return types[firstDigits] || 'Inconnu';
}

console.log(processPhone('+33 6 12 34 56 78', 'display'));
// {
//   valid: true,
//   original: '+33 6 12 34 56 78',
//   normalized: '0612345678',
//   formatted: '06 12 34 56 78',
//   type: 'Mobile'
// }

// Utilisation dans un composant React (exemple)
/*
function PhoneInput() {
    const [phone, setPhone] = useState('');
    const [validation, setValidation] = useState(null);
    
    const handleChange = (e) => {
        const value = e.target.value;
        setPhone(value);
        
        const result = processPhone(value, 'display');
        setValidation(result);
    };
    
    return (
        <div>
            <input 
                value={phone} 
                onChange={handleChange}
                placeholder="06 12 34 56 78"
            />
            {validation && !validation.valid && (
                <span className="error">{validation.error}</span>
            )}
            {validation && validation.valid && (
                <span className="success">
                    {validation.formatted} ({validation.type})
                </span>
            )}
        </div>
    );
}
*/
```

**Points clés:**
- Accepter plusieurs formats en entrée
- Normaliser avant validation
- Formater selon le contexte (affichage, stockage, API)
- Identifier le type de numéro
- En React: validation en temps réel avec feedback

---

### Q10: Quelle est la différence entre greedy et lazy matching ?

**Réponse:**
```javascript
// GREEDY (par défaut) - Match le PLUS possible
const text1 = '<div>Content 1</div><div>Content 2</div>';
const text1 = '<div>Content 1</div><div>Content 2</div>';

const greedyRegex = /<div>.*<\/div>/;
console.log(text1.match(greedyRegex)[0]);
// "<div>Content 1</div><div>Content 2</div>" 
// ❌ Prend TOUT du premier <div> au dernier </div>

// LAZY (avec ?) - Match le MOINS possible
const lazyRegex = /<div>.*?<\/div>/g;
console.log(text1.match(lazyRegex));
// ["<div>Content 1</div>", "<div>Content 2</div>"]
// ✅ Prend chaque balise séparément

// Explication:
// .* est GREEDY - continue jusqu'au dernier match possible
// .*? est LAZY - s'arrête au premier match possible

// Autres quantifiers greedy vs lazy:
// + (greedy)  vs +? (lazy)  - 1 ou plus
// * (greedy)  vs *? (lazy)  - 0 ou plus
// {n,} (greedy) vs {n,}? (lazy) - au moins n
// {n,m} (greedy) vs {n,m}? (lazy) - entre n et m

// Exemple 1: Extraire contenu entre guillemets
const text2 = 'He said "Hello" and she said "Goodbye"';

// Greedy - prend tout entre les premiers et derniers guillemets
const greedyQuotes = /".*"/;
console.log(text2.match(greedyQuotes)[0]);
// ""Hello" and she said "Goodbye""
// ❌ Pas ce qu'on veut!

// Lazy - prend chaque paire de guillemets séparément
const lazyQuotes = /".*?"/g;
console.log(text2.match(lazyQuotes));
// [""Hello"", ""Goodbye""]
// ✅ Correct!

// Exemple 2: Extraire balises HTML
const html = '<p>Paragraph 1</p><p>Paragraph 2</p><p>Paragraph 3</p>';

// Greedy
console.log(html.match(/<p>.*<\/p>/)[0]);
// "<p>Paragraph 1</p><p>Paragraph 2</p><p>Paragraph 3</p>"

// Lazy
console.log(html.match(/<p>.*?<\/p>/g));
// ["<p>Paragraph 1</p>", "<p>Paragraph 2</p>", "<p>Paragraph 3</p>"]

// Exemple 3: Extraire nombres
const text3 = 'I have 123456 items';

// Greedy - prend tous les chiffres d'affilée
console.log(text3.match(/\d+/)[0]);  // "123456"

// Lazy - prend le minimum (1 chiffre)
console.log(text3.match(/\d+?/)[0]); // "1"
// ⚠️ Pas très utile dans ce cas!

// Exemple 4: Parser attributs HTML
function extractAttributesGreedy(tag) {
    // Greedy - problématique
    const regex = /(\w+)="(.*)"/g;
    const attrs = {};
    let match;
    
    while ((match = regex.exec(tag)) !== null) {
        attrs[match[1]] = match[2];
    }
    return attrs;
}

function extractAttributesLazy(tag) {
    // Lazy - correct
    const regex = /(\w+)="(.*?)"/g;
    const attrs = {};
    let match;
    
    while ((match = regex.exec(tag)) !== null) {
        attrs[match[1]] = match[2];
    }
    return attrs;
}

const tag = '<div class="container" id="main" data-value="test">';

console.log(extractAttributesGreedy(tag));
// { class: 'container" id="main" data-value="test' }
// ❌ Greedy prend trop!

console.log(extractAttributesLazy(tag));
// { class: 'container', id: 'main', data-value: 'test' }
// ✅ Lazy fonctionne correctement

// Exemple 5: Extraire code entre backticks
const markdown = 'Use `const` and `let` for variables';

// Greedy
console.log(markdown.match(/`.*`/)[0]);
// "`const` and `let`"
// ❌ Prend tout entre le premier et dernier backtick

// Lazy
console.log(markdown.match(/`.*?`/g));
// ["`const`", "`let`"]
// ✅ Prend chaque bloc de code séparément

// Cas pratique: Parser un template
function parseTemplate(template, data) {
    // IMPORTANT: Utiliser lazy pour éviter de matcher plusieurs {{...}}
    return template.replace(/\{\{(.*?)\}\}/g, (match, key) => {
        return data[key.trim()] || match;
    });
}

const template = 'Hello {{name}}, you have {{count}} messages';
console.log(parseTemplate(template, { name: 'John', count: 5 }));
// "Hello John, you have 5 messages"

// Avec greedy (ne fonctionnerait pas):
// {{name}}, you have {{count}} serait vu comme UN SEUL match

// Performance: Lazy peut être plus lent
// Greedy: évalue une fois, backtracks si nécessaire
// Lazy: essaie de matcher au minimum, peut nécessiter plus d'essais

// Exemple de performance
function measurePerformance() {
    const longText = '<div>' + 'x'.repeat(10000) + '</div>';
    
    console.time('greedy');
    longText.match(/<div>.*<\/div>/);
    console.timeEnd('greedy');
    
    console.time('lazy');
    longText.match(/<div>.*?<\/div>/);
    console.timeEnd('lazy');
}

// RÈGLE GÉNÉRALE:
// - Utiliser LAZY quand on veut matcher des paires (guillemets, balises, etc.)
// - Utiliser GREEDY quand on veut tout matcher d'un coup
// - Par défaut, préférer LAZY pour éviter les surprises
```

**Points clés:**
- Greedy (`.*`) = match le plus possible
- Lazy (`.*?`) = match le moins possible
- Lazy est essentiel pour parser balises HTML, guillemets, etc.
- Ajouter `?` après quantifier pour le rendre lazy
- Performance: greedy peut être plus rapide dans certains cas

---

## Questions Niveau Avancé

### Q11: Comment utiliser les lookahead et lookbehind assertions ?

**Réponse:**
```javascript
// POSITIVE LOOKAHEAD (?=...) 
// Match SI suivi par le pattern (sans l'inclure dans le match)

// Exemple: Trouver un mot suivi de "script"
const regex1 = /\w+(?=script)/;
console.log('javascript'.match(regex1)); // ["java"]
console.log('typescript'.match(regex1)); // ["type"]
console.log('python'.match(regex1));     // null

// Cas d'usage: Validation mot de passe
// Au moins 8 chars, 1 majuscule, 1 chiffre
function validatePassword(password) {
    // Chaque lookahead vérifie une condition
    const regex = /^(?=.*[A-Z])(?=.*\d).{8,}$/;
    return regex.test(password);
}

console.log(validatePassword('Pass123'));     // false (trop court)
console.log(validatePassword('Password1'));   // true
console.log(validatePassword('password123')); // false (pas de maj)

// Explication détaillée:
// ^            - Début
// (?=.*[A-Z])  - Lookahead: doit contenir au moins 1 majuscule
// (?=.*\d)     - Lookahead: doit contenir au moins 1 chiffre
// .{8,}        - Au moins 8 caractères
// $            - Fin

// NEGATIVE LOOKAHEAD (?!...)
// Match SI PAS suivi par le pattern

// Exemple: Mots qui ne se terminent pas par "ing"
const regex2 = /\w+(?!ing)\b/g;
const text1 = 'I am running and walking but I like to jump';
console.log(text1.match(regex2));
// ["I", "am", "and", "but", "I", "like", "to", "jump"]
// "running" et "walking" exclus

// POSITIVE LOOKBEHIND (?<=...)
// Match SI précédé par le pattern (sans l'inclure)

// Exemple: Nombres précédés par $
const regex3 = /(?<=\$)\d+/g;
const text2 = 'Prices: $100, $50, 30';
console.log(text2.match(regex3));
// ["100", "50"]
// Le "30" n'est pas précédé de $, donc exclu

// NEGATIVE LOOKBEHIND (?<!...)
// Match SI PAS précédé par le pattern

// Exemple: Nombres NON précédés par $
const regex4 = /(?<!\$)\b\d+\b/g;
const text3 = 'Prices: $100, $50, 30 items';
console.log(text3.match(regex4));
// ["30"]
// Les prix avec $ sont exclus

// CAS PRATIQUES

// 1. Extraire prix (chiffres après $)
function extractPrices(text) {
    const priceRegex = /(?<=\$)\d+(?:\.\d{2})?/g;
    return text.match(priceRegex) || [];
}

console.log(extractPrices('Items: $19.99, $5.50, and $100'));
// ["19.99", "5.50", "100"]

// 2. Valider mot de passe complexe
function validateStrongPassword(password) {
    // Au moins 8 chars, 1 maj, 1 min, 1 chiffre, 1 caractère spécial
    const regex = /^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[@$!%*?&]).{8,}$/;
    return regex.test(password);
}

console.log(validateStrongPassword('Pass123!'));   // true
console.log(validateStrongPassword('Password1'));  // false (pas de spécial)

// 3. Trouver mots entre 5 et 10 lettres qui ne sont pas précédés de "very"
const regex5 = /(?<!very )\b\w{5,10}\b/gi;
const text4 = 'This is a very beautiful day with beautiful weather';
console.log(text4.match(regex5));
// ["beautiful", "weather"] 
// Le premier "beautiful" est exclu car précédé de "very"

// 4. Extraire extension de fichier (sauf .tmp)
const regex6 = /\.(?!tmp\b)\w+$/;
const files = ['doc.pdf', 'data.tmp', 'image.jpg'];
files.forEach(file => {
    const match = file.match(regex6);
    console.log(file, '→', match ? match[0] : 'excluded');
});
// doc.pdf → .pdf
// data.tmp → excluded
// image.jpg → .jpg

// 5. Validation complexe: username
// - 3-16 caractères
// - Commence par lettre
// - Alphanumerique + underscore
// - Ne se termine pas par underscore
function validateUsername(username) {
    const regex = /^(?=[a-zA-Z])[a-zA-Z0-9_]{3,16}(?<!_)$/;
    return regex.test(username);
}

console.log(validateUsername('john_doe'));   // true
console.log(validateUsername('john_'));      // false (finit par _)
console.log(validateUsername('_john'));      // false (commence par _)
console.log(validateUsername('jd'));         // false (trop court)

// LIMITATIONS
// ⚠️ Lookbehind pas supporté dans certains vieux navigateurs
// ⚠️ Lookbehind doit avoir longueur fixe dans certains moteurs regex

// Test de support
function supportsLookbehind() {
    try {
        return /(?<=a)b/.test('ab');
    } catch (e) {
        return false;
    }
}

console.log('Lookbehind supporté:', supportsLookbehind());

// Alternative sans lookbehind (pour compatibilité)
function extractPricesCompatible(text) {
    // Au lieu de: /(?<=\$)\d+/g
    // Utiliser: /\$(\d+)/g et extraire groupe 1
    const matches = [...text.matchAll(/\$(\d+(?:\.\d{2})?)/g)];
    return matches.map(m => m[1]);
}

console.log(extractPricesCompatible('Items: $19.99, $5.50, and $100'));
// ["19.99", "5.50", "100"]
```

**Points clés:**
- Lookahead `(?=...)` et `(?!...)` : vérifient ce qui suit
- Lookbehind `(?<=...)` et `(?<!...)` : vérifient ce qui précède
- Ne consomment pas de caractères (zero-width assertions)
- Utiles pour validations complexes
- Lookbehind pas supporté partout (vérifier compatibilité)

---

### Q12: Comment éviter les catastrophic backtracking avec les RegEx ?

**Réponse:**
```javascript
// CATASTROPHIC BACKTRACKING = regex qui prend énormément de temps
// Peut bloquer/freezer le navigateur!

// ❌ EXEMPLE DANGEREUX
const dangerousRegex = /^(a+)+$/;

// Cette regex peut prendre des SECONDES avec une string comme:
// 'aaaaaaaaaaaaaaaaaaaaaaaaaaab'

// Pourquoi? À cause des quantifiers imbriqués (a+)+
// Le moteur regex essaie TOUTES les combinaisons possibles

// Test (NE PAS EXÉCUTER en production!)
/*
console.time('dangerous');
dangerousRegex.test('aaaaaaaaaaaaaaaaaaaaaaaaaaab');
console.timeEnd('dangerous'); // Peut prendre 10+ secondes!
*/

// ✅ VERSION SÛRE
const safeRegex = /^a+$/;
console.time('safe');
safeRegex.test('aaaaaaaaaaaaaaaaaaaaaaaaaaab'); 
console.timeEnd('safe'); // < 1ms

// RÈGLES POUR ÉVITER CATASTROPHIC BACKTRACKING:

// 1. Éviter quantifiers imbriqués
// ❌ (a+)+
// ❌ (a*)*
// ❌ (a+)*
// ✅ a+

// 2. Éviter alternances avec chevauchement
// ❌ (a|a)+
// ❌ (ab|a)+
// ✅ (ab|c)+

// 3. Être spécifique avec les quantifiers
// ❌ .*
// ✅ [a-z]+
// ✅ \w+

// EXEMPLES DE PATTERNS DANGEREUX

// Pattern 1: Email (dangereux)
const dangerousEmail = /^([a-zA-Z0-9]+)*@[a-zA-Z0-9]+\.[a-zA-Z]+$/;
// Problème: ([a-zA-Z0-9]+)* = quantifier sur quantifier

// Pattern 1: Email (sûr)
const safeEmail = /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;

// Pattern 2: HTML tags (dangereux)
const dangerousHTML = /<.*>.*<\/.*>/;
// Problème: .* est trop greedy et peut backtrack énormément

// Pattern 2: HTML tags (sûr)
const safeHTML = /<([a-z]+)>.*?<\/\1>/i;
// Plus spécifique, lazy quantifier

// TESTER LA SÉCURITÉ D'UNE REGEX

function testRegexSafety(regex, testString, timeout = 1000) {
    const start = Date.now();
    
    try {
        const result = regex.test(testString);
        const duration = Date.now() - start;
        
        return {
            safe: duration < timeout,
            duration: duration,
            result: result
        };
    } catch (e) {
        return {
            safe: false,
            error: e.message
        };
    }
}

// Test avec string problématique
const testString = 'a'.repeat(30) + 'b';

console.log(testRegexSafety(/^(a+)+$/, testString));
// { safe: false, duration: 5000+ ... } ❌

console.log(testRegexSafety(/^a+$/, testString));
// { safe: true, duration: < 1ms ... } ✅

// STRATÉGIES DE PROTECTION

// 1. Limiter la longueur de l'input
function validateWithLimit(regex, text, maxLength = 1000) {
    if (text.length > maxLength) {
        return { valid: false, error: 'Input trop long' };
    }
    return { valid: regex.test(text) };
}

// 2. Utiliser un timeout
function validateWithTimeout(regex, text, timeout = 100) {
    return new Promise((resolve, reject) => {
        const timer = setTimeout(() => {
            reject(new Error('Regex timeout'));
        }, timeout);
        
        try {
            const result = regex.test(text);
            clearTimeout(timer);
            resolve(result);
        } catch (e) {
            clearTimeout(timer);
            reject(e);
        }
    });
}

// Utilisation
validateWithTimeout(/^a+$/, 'aaa'.repeat(100))
    .then(result => console.log('Valid:', result))
    .catch(err => console.log('Error:', err.message));

// 3. Préférer méthodes alternatives
// Au lieu de regex complexe, parfois mieux d'utiliser:

function validateEmailAlternative(email) {
    // Vérifications simples sans regex
    if (!email || email.length > 320) return false;
    
    const parts = email.split('@');
    if (parts.length !== 2) return false;
    
    const [local, domain] = parts;
    if (!local || !domain) return false;
    
    const domainParts = domain.split('.');
    if (domainParts.length < 2) return false;
    
    // Regex simple pour chaque partie
    const localRegex = /^[a-zA-Z0-9._%+-]+$/;
    const domainRegex = /^[a-zA-Z0-9.-]+$/;
    const tldRegex = /^[a-zA-Z]{2,}$/;
    
    return localRegex.test(local) && 
           domainRegex.test(domainParts.slice(0, -1).join('.')) &&
           tldRegex.test(domainParts[domainParts.length - 1]);
}

// BONNES PRATIQUES

// ✅ Être aussi spécifique que possible
const goodRegex1 = /^[a-z]{3,20}$/;  // Au lieu de /^.*$/

// ✅ Utiliser lazy quantifiers quand approprié
const goodRegex2 = /<div>.*?<\/div>/;  // Au lieu de /<div>.*<\/div>/

// ✅ Ancrer les patterns
const goodRegex3 = /^email@test\.com$/;  // Au lieu de /email@test\.com/

// ✅ Limiter la répétition
const goodRegex4 = /\d{1,10}/;  // Au lieu de /\d+/

// ✅ Utiliser possessive quantifiers si disponibles (pas en JS natif)
// En JS, utiliser atomic groups ou lazy quantifiers

// OUTILS POUR TESTER
// - regex101.com (affiche les steps)
// - safe-regex npm package (détecte patterns dangereux)

// Exemple de validation de regex avant utilisation
function isSafeRegex(pattern) {
    const dangerousPatterns = [
        /\([^)]*\+\)\+/,      // (x+)+
        /\([^)]*\*\)\*/,      // (x*)*
        /\([^)]*\+\)\*/,      // (x+)*
        /\([^)]*\|\1\)\+/     // (x|x)+
    ];
    
    return !dangerousPatterns.some(p => p.test(pattern));
}

console.log(isSafeRegex('^(a+)+$'));  // false ❌
console.log(isSafeRegex('^a+$'));     // true ✅
```

**Points clés:**
- Catastrophic backtracking peut bloquer l'application
- Éviter quantifiers imbriqués: `(a+)+`, `(a*)*`
- Être spécifique plutôt que générique: `[a-z]+` vs `.*`
- Tester avec des inputs longs
- Mettre des limites et timeouts en production
- Utiliser des outils pour détecter patterns dangereux

---

### Q13: Comment utiliser les named capture groups et les backreferences ?

**Réponse:**
```javascript
// NAMED CAPTURE GROUPS (?<name>...)
// Plus lisible que les groupes numérotés

// Exemple basique: Parser une date
const dateRegex = /(?<day>\d{2})\/(?<month>\d{2})\/(?<year>\d{4})/;
const match = dateRegex.exec('25/12/2024');

console.log(match.groups.day);    // "25"
console.log(match.groups.month);  // "12"
console.log(match.groups.year);   // "2024"

// Aussi accessible par index (rétrocompatibilité)
console.log(match[1]);  // "25"
console.log(match[2]);  // "12"
console.log(match[3]);  // "2024"

// Utilisation dans replace()
const text1 = 'Date: 25/12/2024';
const formatted = text1.replace(
    /(?<day>\d{2})\/(?<month>\d{2})\/(?<year>\d{4})/,
    '$<year>-$<month>-$<day>'
);
console.log(formatted); // "Date: 2024-12-25"

// BACKREFERENCES - Référencer un groupe capturé
// \1, \2, etc. pour groupes numérotés
// \k<name> pour named groups

// Exemple 1: Trouver mots répétés
const duplicateRegex = /\b(\w+)\s+\1\b/gi;
const text2 = 'The the cat sat sat on the mat';
console.log(text2.match(duplicateRegex));
// ["The the", "sat sat"]

// Corriger les doublons
const corrected = text2.replace(/\b(\w+)\s+\1\b/gi, '$1');
console.log(corrected); // "The cat sat on the mat"

// Exemple 2: Valider balises HTML matching
const htmlRegex = /<(\w+)>.*?<\/\1>/;
console.log(htmlRegex.test('<div>content</div>'));  // true
console.log(htmlRegex.test('<div>content</span>')); // false

// Avec named groups
const htmlNamedRegex = /<(?<tag>\w+)>.*?<\/\k<tag>>/;
console.log(htmlNamedRegex.test('<div>content</div>'));  // true
console.log(htmlNamedRegex.test('<div>content</span>')); // false

// CAS PRATIQUES COMPLEXES

// 1. Parser une URL complète
function parseURL(url) {
    const urlRegex = /^(?<protocol>https?):\/\/(?<domain>[^\/:\s]+)(?::(?<port>\d+))?(?<path>\/[^?#]*)?(?:\?(?<query>[^#]*))?(?:#(?<hash>.*))?$/;
    
    const match = urlRegex.exec(url);
    return match ? match.groups : null;
}

const urlInfo = parseURL('https://example.com:8080/path?id=123#section');
console.log(urlInfo);
// {
//   protocol: 'https',
//   domain: 'example.com',
//   port: '8080',
//   path: '/path',
//   query: 'id=123',
//   hash: 'section'
// }

// 2. Valider et parser un email
function parseEmail(email) {
    const emailRegex = /^(?<username>[a-zA-Z0-9._%+-]+)@(?<domain>[a-zA-Z0-9.-]+)\.(?<tld>[a-zA-Z]{2,})$/;
    
    const match = emailRegex.exec(email);
    return match ? match.groups : null;
}

console.log(parseEmail('john.doe@example.com'));
// { username: 'john.doe', domain: 'example', tld: 'com' }

// 3. Extraire variables de template
function parseTemplate(template) {
    const varRegex = /\{\{(?<varName>\w+)(?::(?<filter>\w+))?\}\}/g;
    const variables = [];
    
    for (const match of template.matchAll(varRegex)) {
        variables.push({
            full: match[0],
            name: match.groups.varName,
            filter: match.groups.filter || null
        });
    }
    
    return variables;
}

const template = 'Hello {{name}}, your balance is {{balance:currency}}';
console.log(parseTemplate(template));
// [
//   { full: '{{name}}', name: 'name', filter: null },
//   { full: '{{balance:currency}}', name: 'balance', filter: 'currency' }
// ]

// 4. Valider paires de quotes/parenthèses
function validatePairs(text) {
    // Vérifier que les guillemets/parenthèses sont bien fermés
    const patterns = {
        quotes: /(?<quote>["']).*?\k<quote>/g,
        parens: /\((?<content>[^()]*)\)/g,
        brackets: /\[(?<content>[^\[\]]*)\]/g
    };
    
    const results = {};
    
    for (const [type, regex] of Object.entries(patterns)) {
        const matches = [...text.matchAll(regex)];
        results[type] = matches.map(m => m[0]);
    }
    
    return results;
}

const text3 = 'He said "hello" and (test) with [brackets]';
console.log(validatePairs(text3));
// {
//   quotes: ['"hello"'],
//   parens: ['(test)'],
//   brackets: ['[brackets]']
// }

// 5. Swap prénom/nom
function swapNames(fullName) {
    return fullName.replace(
        /(?<first>\w+)\s+(?<last>\w+)/,
        '$<last>, $<first>'
    );
}

console.log(swapNames('John Doe')); // "Doe, John"

// 6. Valider et formater numéro de carte bancaire
function formatCreditCard(number) {
    const cleaned = number.replace(/\s/g, '');
    
    const cardRegex = /^(?<group1>\d{4})(?<group2>\d{4})(?<group3>\d{4})(?<group4>\d{4})$/;
    const match = cardRegex.exec(cleaned);
    
    if (!match) return null;
    
    const { group1, group2, group3, group4 } = match.groups;
    return `${group1} ${group2} ${group3} ${group4}`;
}

console.log(formatCreditCard('1234567890123456'));
// "1234 5678 9012 3456"

// 7. Détecter et remplacer tags HTML avec attributes
function replaceHTMLTags(html) {
    const tagRegex = /<(?<tag>\w+)(?:\s+[^>]*)?>(?<content>.*?)<\/\k<tag>>/g;
    
    return html.replace(tagRegex, (match, tag, content) => {
        return `[${tag.toUpperCase()}]${content}[/${tag.toUpperCase()}]`;
    });
}

const html = '<div class="test">Hello</div><span>World</span>';
console.log(replaceHTMLTags(html));
// "[DIV]Hello[/DIV][SPAN]World[/SPAN]"

// 8. Parser fichier CSV avec named groups
function parseCSVWithHeaders(csvText) {
    const lines = csvText.trim().split('\n');
    const headers = lines[0].split(',');
    
    // Créer regex dynamique avec named groups
    const groupNames = headers.map(h => h.trim().replace(/\s+/g, '_'));
    const pattern = groupNames.map(name => `(?<${name}>[^,]*)`).join(',');
    const regex = new RegExp(pattern);
    
    return lines.slice(1).map(line => {
        const match = regex.exec(line);
        return match ? match.groups : null;
    }).filter(Boolean);
}

const csv = `Name,Age,City
John,30,Paris
Jane,25,London`;

console.log(parseCSVWithHeaders(csv));
// [
//   { Name: 'John', Age: '30', City: 'Paris' },
//   { Name: 'Jane', Age: '25', City: 'London' }
// ]

// AVANTAGES DES NAMED GROUPS
// ✅ Plus lisible que match[1], match[2]
// ✅ Facilite la maintenance
// ✅ Auto-documenté
// ✅ Moins d'erreurs lors de modifications

// COMPATIBILITÉ
// Named groups supportés dans tous les navigateurs modernes
// Pour vieux navigateurs, fallback sur groupes numérotés
function safeExtract(text) {
    try {
        const regex = /(?<value>\d+)/;
        const match = regex.exec(text);
        return match?.groups?.value || null;
    } catch (e) {
        // Fallback pour vieux navigateurs
        const regex = /(\d+)/;
        const match = regex.exec(text);
        return match?.[1] || null;
    }
}
```

**Points clés:**
- Named groups: `(?<name>...)` plus lisible que groupes numérotés
- Accès via `match.groups.name`
- Backreferences: `\1` ou `\k<name>`
- Utile pour parser données structurées
- Replace avec `$<name>` au lieu de `$1`
- Améliore la lisibilité et maintenabilité du code

---