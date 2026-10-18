## [**..**](./00_index.md)

# Chapitre 13 : Date et Time

## 13.1 Date Object

L'objet `Date` permet de travailler avec les dates et heures en JavaScript.

### Création de Dates

```javascript
// 1. Date actuelle (maintenant)
const now = new Date();
console.log(now); // Date et heure actuelles
// Ex: 2024-11-07T14:30:00.000Z

// 2. Date à partir d'une string
const date1 = new Date('2024-11-07');
console.log(date1); // 2024-11-07T00:00:00.000Z

const date2 = new Date('2024-11-07T15:30:00');
console.log(date2); // 2024-11-07T15:30:00.000Z

const date3 = new Date('November 7, 2024');
console.log(date3);

const date4 = new Date('Nov 7 2024 15:30:00');
console.log(date4);

// 3. Date à partir de paramètres (année, mois, jour, heure, minute, seconde, ms)
// ⚠️ ATTENTION: Les mois commencent à 0 (0 = janvier, 11 = décembre)
const date5 = new Date(2024, 10, 7); // 7 novembre 2024
console.log(date5);

const date6 = new Date(2024, 10, 7, 15, 30, 0); // 7 nov 2024, 15:30:00
console.log(date6);

// Tous les paramètres
const date7 = new Date(2024, 10, 7, 15, 30, 45, 500);
// Année, Mois(0-11), Jour, Heure, Minute, Seconde, Milliseconde
console.log(date7);

// 4. Date à partir d'un timestamp (millisecondes depuis 1er janvier 1970)
const timestamp = 1699368600000;
const date8 = new Date(timestamp);
console.log(date8);

// Epoch (01/01/1970 00:00:00 UTC)
const epoch = new Date(0);
console.log(epoch); // 1970-01-01T00:00:00.000Z

// Date invalide
const invalid = new Date('invalid string');
console.log(invalid); // Invalid Date
console.log(invalid.toString()); // "Invalid Date"

// Vérifier si date est valide
function isValidDate(date) {
    return date instanceof Date && !isNaN(date);
}

console.log(isValidDate(new Date())); // true
console.log(isValidDate(new Date('invalid'))); // false
console.log(isValidDate('2024-11-07')); // false (pas un objet Date)

// Exemples de formats de string acceptés
const formats = [
    '2024-11-07',                    // ISO 8601
    '2024-11-07T15:30:00',          // ISO avec heure
    '2024-11-07T15:30:00.000Z',     // ISO avec milliseconds et UTC
    'November 7, 2024',              // Format long
    'Nov 7 2024',                    // Format court
    '11/7/2024',                     // MM/DD/YYYY (format US)
    '2024/11/07'                     // YYYY/MM/DD
];

formats.forEach(format => {
    console.log(`${format} → ${new Date(format)}`);
});
```

### Timestamps

```javascript
// Timestamp = nombre de millisecondes depuis le 1er janvier 1970 00:00:00 UTC

// Obtenir timestamp actuel
const now = Date.now();
console.log(now); // Ex: 1699368600000 (millisecondes)

// Obtenir timestamp d'une date
const date = new Date('2024-11-07T15:30:00');
const timestamp = date.getTime();
console.log(timestamp);

// Également avec valueOf()
console.log(date.valueOf()); // Même résultat

// Convertir timestamp en date
const fromTimestamp = new Date(timestamp);
console.log(fromTimestamp);

// Opérations arithmétiques sur timestamps
const date1 = new Date('2024-11-07');
const date2 = new Date('2024-11-10');

// Différence en millisecondes
const diffMs = date2 - date1;
console.log(diffMs); // 259200000 ms

// Convertir en jours
const diffDays = diffMs / (1000 * 60 * 60 * 24);
console.log(diffDays); // 3 jours

// Ajouter des jours à une date
const today = new Date();
const tomorrow = new Date(today.getTime() + (24 * 60 * 60 * 1000));
console.log(tomorrow);

// Soustraire des jours
const yesterday = new Date(today.getTime() - (24 * 60 * 60 * 1000));
console.log(yesterday);

// Calculer âge
function calculateAge(birthDate) {
    const today = new Date();
    const birth = new Date(birthDate);
    
    let age = today.getFullYear() - birth.getFullYear();
    const monthDiff = today.getMonth() - birth.getMonth();
    
    // Ajuster si anniversaire pas encore passé cette année
    if (monthDiff < 0 || (monthDiff === 0 && today.getDate() < birth.getDate())) {
        age--;
    }
    
    return age;
}

console.log(calculateAge('1990-05-15')); // Ex: 34 ans

// Comparer des dates
const dateA = new Date('2024-11-07');
const dateB = new Date('2024-11-10');

console.log(dateA < dateB);  // true
console.log(dateA > dateB);  // false
console.log(dateA.getTime() === dateB.getTime()); // false

// Égalité de dates (comparer timestamps)
function isSameDay(date1, date2) {
    return date1.toDateString() === date2.toDateString();
}

const day1 = new Date('2024-11-07T10:00:00');
const day2 = new Date('2024-11-07T15:30:00');
console.log(isSameDay(day1, day2)); // true (même jour)

// Performance: mesurer temps d'exécution
const start = Date.now();

// Code à mesurer
for (let i = 0; i < 1000000; i++) {
    // Quelque chose
}

const end = Date.now();
console.log(`Exécution: ${end - start}ms`);

// Alternative avec console.time
console.time('operation');
// Code
console.timeEnd('operation'); // Affiche le temps
```

---

## 13.2 Méthodes Date

### Get Methods (Lecture)

```javascript
const date = new Date('2024-11-07T15:30:45.500');

// Année (4 chiffres)
console.log(date.getFullYear()); // 2024

// Mois (0-11, 0 = janvier)
console.log(date.getMonth()); // 10 (novembre)

// Jour du mois (1-31)
console.log(date.getDate()); // 7

// Jour de la semaine (0-6, 0 = dimanche)
console.log(date.getDay()); // 4 (jeudi)
// 0 = dimanche, 1 = lundi, ..., 6 = samedi

// Heure (0-23)
console.log(date.getHours()); // 15

// Minutes (0-59)
console.log(date.getMinutes()); // 30

// Secondes (0-59)
console.log(date.getSeconds()); // 45

// Millisecondes (0-999)
console.log(date.getMilliseconds()); // 500

// Timestamp (millisecondes depuis epoch)
console.log(date.getTime()); // 1699368645500

// Obtenir composants UTC (heure universelle)
console.log(date.getUTCFullYear()); // 2024
console.log(date.getUTCMonth()); // 10
console.log(date.getUTCDate()); // 7
console.log(date.getUTCHours()); // Dépend du timezone
console.log(date.getUTCMinutes()); // 30
console.log(date.getUTCSeconds()); // 45

// Offset du timezone en minutes
console.log(date.getTimezoneOffset()); 
// Ex: -60 pour UTC+1, 300 pour UTC-5

// Exemples pratiques

// Obtenir nom du mois
function getMonthName(date) {
    const months = [
        'Janvier', 'Février', 'Mars', 'Avril', 'Mai', 'Juin',
        'Juillet', 'Août', 'Septembre', 'Octobre', 'Novembre', 'Décembre'
    ];
    return months[date.getMonth()];
}

console.log(getMonthName(date)); // "Novembre"

// Obtenir nom du jour
function getDayName(date) {
    const days = [
        'Dimanche', 'Lundi', 'Mardi', 'Mercredi',
        'Jeudi', 'Vendredi', 'Samedi'
    ];
    return days[date.getDay()];
}

console.log(getDayName(date)); // "Jeudi"

// Est-ce un weekend ?
function isWeekend(date) {
    const day = date.getDay();
    return day === 0 || day === 6; // Dimanche ou Samedi
}

console.log(isWeekend(date)); // false

// Formater date custom
function formatDate(date) {
    const day = String(date.getDate()).padStart(2, '0');
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const year = date.getFullYear();
    
    return `${day}/${month}/${year}`;
}

console.log(formatDate(date)); // "07/11/2024"

// Formater heure
function formatTime(date) {
    const hours = String(date.getHours()).padStart(2, '0');
    const minutes = String(date.getMinutes()).padStart(2, '0');
    const seconds = String(date.getSeconds()).padStart(2, '0');
    
    return `${hours}:${minutes}:${seconds}`;
}

console.log(formatTime(date)); // "15:30:45"

// Date et heure complète
function formatDateTime(date) {
    return `${formatDate(date)} ${formatTime(date)}`;
}

console.log(formatDateTime(date)); // "07/11/2024 15:30:45"

// Obtenir trimestre
function getQuarter(date) {
    return Math.floor(date.getMonth() / 3) + 1;
}

console.log(getQuarter(date)); // 4 (Q4)

// Nombre de jours dans le mois
function getDaysInMonth(date) {
    const year = date.getFullYear();
    const month = date.getMonth();
    return new Date(year, month + 1, 0).getDate();
}

console.log(getDaysInMonth(date)); // 30 (novembre a 30 jours)

// Est-ce une année bissextile ?
function isLeapYear(year) {
    return (year % 4 === 0 && year % 100 !== 0) || (year % 400 === 0);
}

console.log(isLeapYear(2024)); // true
console.log(isLeapYear(2023)); // false
```

### Set Methods (Modification)

```javascript
const date = new Date('2024-11-07T15:30:45');

// Modifier l'année
date.setFullYear(2025);
console.log(date); // 2025-11-07...

// Modifier le mois (0-11)
date.setMonth(0); // Janvier
console.log(date); // 2025-01-07...

// Modifier le jour
date.setDate(15);
console.log(date); // 2025-01-15...

// Modifier l'heure
date.setHours(20);
console.log(date); // ...20:30:45

// Modifier les minutes
date.setMinutes(45);
console.log(date); // ...20:45:45

// Modifier les secondes
date.setSeconds(0);
console.log(date); // ...20:45:00

// Modifier les millisecondes
date.setMilliseconds(123);
console.log(date);

// Modifier le timestamp complet
date.setTime(Date.now());
console.log(date); // Date/heure actuelles

// Méthodes UTC (pour heure universelle)
date.setUTCFullYear(2024);
date.setUTCMonth(10);
date.setUTCDate(7);
date.setUTCHours(15);
date.setUTCMinutes(30);
date.setUTCSeconds(0);

// Modifier plusieurs composants à la fois
date.setFullYear(2024, 11, 25); // 25 décembre 2024
console.log(date);

date.setHours(0, 0, 0, 0); // Minuit
console.log(date);

// Exemples pratiques

// Début du jour (00:00:00)
function startOfDay(date) {
    const newDate = new Date(date);
    newDate.setHours(0, 0, 0, 0);
    return newDate;
}

const now = new Date();
console.log(startOfDay(now)); // Aujourd'hui à minuit

// Fin du jour (23:59:59.999)
function endOfDay(date) {
    const newDate = new Date(date);
    newDate.setHours(23, 59, 59, 999);
    return newDate;
}

console.log(endOfDay(now)); // Aujourd'hui à 23:59:59

// Début du mois
function startOfMonth(date) {
    const newDate = new Date(date);
    newDate.setDate(1);
    newDate.setHours(0, 0, 0, 0);
    return newDate;
}

console.log(startOfMonth(now)); // 1er du mois à minuit

// Fin du mois
function endOfMonth(date) {
    const newDate = new Date(date);
    newDate.setMonth(newDate.getMonth() + 1, 0);
    newDate.setHours(23, 59, 59, 999);
    return newDate;
}

console.log(endOfMonth(now)); // Dernier jour du mois à 23:59:59

// Ajouter des jours
function addDays(date, days) {
    const newDate = new Date(date);
    newDate.setDate(newDate.getDate() + days);
    return newDate;
}

console.log(addDays(now, 7)); // Dans 7 jours

// Ajouter des mois
function addMonths(date, months) {
    const newDate = new Date(date);
    newDate.setMonth(newDate.getMonth() + months);
    return newDate;
}

console.log(addMonths(now, 3)); // Dans 3 mois

// Ajouter des années
function addYears(date, years) {
    const newDate = new Date(date);
    newDate.setFullYear(newDate.getFullYear() + years);
    return newDate;
}

console.log(addYears(now, 1)); // Dans 1 an

// Soustraire des jours (dates négatives)
console.log(addDays(now, -5)); // Il y a 5 jours

// Définir une date relative
function getNextMonday() {
    const today = new Date();
    const daysUntilMonday = (8 - today.getDay()) % 7 || 7;
    return addDays(today, daysUntilMonday);
}

console.log(getNextMonday()); // Prochain lundi

// Premier jour de l'année
function startOfYear(date) {
    const newDate = new Date(date);
    newDate.setMonth(0, 1);
    newDate.setHours(0, 0, 0, 0);
    return newDate;
}

console.log(startOfYear(now)); // 1er janvier

// ⚠️ ATTENTION: Set methods modifient l'objet Date original
const original = new Date('2024-11-07');
original.setDate(15);
console.log(original); // Modifié!

// Pour ne pas modifier l'original, créer une copie
function safeAddDays(date, days) {
    const copy = new Date(date); // Copie
    copy.setDate(copy.getDate() + days);
    return copy;
}

const date1 = new Date('2024-11-07');
const date2 = safeAddDays(date1, 5);
console.log(date1); // Inchangé
console.log(date2); // +5 jours
```

### Méthodes de Formatage

```javascript
const date = new Date('2024-11-07T15:30:45.500');

// toString() - Représentation complète
console.log(date.toString());
// "Thu Nov 07 2024 15:30:45 GMT+0100 (Central European Standard Time)"

// toDateString() - Seulement la date
console.log(date.toDateString());
// "Thu Nov 07 2024"

// toTimeString() - Seulement l'heure
console.log(date.toTimeString());
// "15:30:45 GMT+0100 (Central European Standard Time)"

// toISOString() - Format ISO 8601 (UTC)
console.log(date.toISOString());
// "2024-11-07T14:30:45.500Z"

// toJSON() - Même que toISOString()
console.log(date.toJSON());
// "2024-11-07T14:30:45.500Z"

// toUTCString() - Format UTC
console.log(date.toUTCString());
// "Thu, 07 Nov 2024 14:30:45 GMT"

// toLocaleDateString() - Date formatée selon locale
console.log(date.toLocaleDateString());
// "11/7/2024" (US) ou "07/11/2024" (FR)

console.log(date.toLocaleDateString('fr-FR'));
// "07/11/2024" (format français)

console.log(date.toLocaleDateString('en-US'));
// "11/7/2024" (format américain)

console.log(date.toLocaleDateString('de-DE'));
// "7.11.2024" (format allemand)

// toLocaleTimeString() - Heure formatée selon locale
console.log(date.toLocaleTimeString());
// "3:30:45 PM" (US) ou "15:30:45" (FR)

console.log(date.toLocaleTimeString('fr-FR'));
// "15:30:45"

console.log(date.toLocaleTimeString('en-US'));
// "3:30:45 PM"

// toLocaleString() - Date et heure selon locale
console.log(date.toLocaleString());
// "11/7/2024, 3:30:45 PM" (US)

console.log(date.toLocaleString('fr-FR'));
// "07/11/2024 15:30:45"

// Options de formatage avancées
const options = {
    weekday: 'long',     // "Thursday"
    year: 'numeric',     // "2024"
    month: 'long',       // "November"
    day: 'numeric',      // "7"
    hour: '2-digit',     // "15"
    minute: '2-digit',   // "30"
    second: '2-digit'    // "45"
};

console.log(date.toLocaleString('fr-FR', options));
// "jeudi 7 novembre 2024 à 15:30:45"

console.log(date.toLocaleString('en-US', options));
// "Thursday, November 7, 2024 at 3:30:45 PM"

// Options de formatage variées
const shortDate = {
    year: 'numeric',
    month: '2-digit',
    day: '2-digit'
};
console.log(date.toLocaleDateString('fr-FR', shortDate));
// "07/11/2024"

const longDate = {
    weekday: 'long',
    year: 'numeric',
    month: 'long',
    day: 'numeric'
};
console.log(date.toLocaleDateString('fr-FR', longDate));
// "jeudi 7 novembre 2024"

const time24 = {
    hour: '2-digit',
    minute: '2-digit',
    hour12: false
};
console.log(date.toLocaleTimeString('en-US', time24));
// "15:30"

const time12 = {
    hour: 'numeric',
    minute: '2-digit',
    hour12: true
};
console.log(date.toLocaleTimeString('en-US', time12));
// "3:30 PM"

// Format relatif (Intl.RelativeTimeFormat)
const rtf = new Intl.RelativeTimeFormat('fr-FR', { numeric: 'auto' });

console.log(rtf.format(-1, 'day'));    // "hier"
console.log(rtf.format(0, 'day'));     // "aujourd'hui"
console.log(rtf.format(1, 'day'));     // "demain"
console.log(rtf.format(7, 'day'));     // "dans 7 jours"
console.log(rtf.format(-7, 'day'));    // "il y a 7 jours"
console.log(rtf.format(2, 'month'));   // "dans 2 mois"
console.log(rtf.format(-3, 'year'));   // "il y a 3 ans"

// Fonction helper pour format relatif
function getRelativeTime(date) {
    const now = new Date();
    const diffMs = date - now;
    const diffSec = Math.round(diffMs / 1000);
    const diffMin = Math.round(diffSec / 60);
    const diffHour = Math.round(diffMin / 60);
    const diffDay = Math.round(diffHour / 24);
    
    const rtf = new Intl.RelativeTimeFormat('fr-FR', { numeric: 'auto' });
    
    if (Math.abs(diffSec) < 60) {
        return rtf.format(diffSec, 'second');
    } else if (Math.abs(diffMin) < 60) {
        return rtf.format(diffMin, 'minute');
    } else if (Math.abs(diffHour) < 24) {
        return rtf.format(diffHour, 'hour');
    } else if (Math.abs(diffDay) < 30) {
        return rtf.format(diffDay, 'day');
    } else {
        return date.toLocaleDateString('fr-FR');
    }
}

console.log(getRelativeTime(new Date(Date.now() - 30000)));     // "il y a 30 secondes"
console.log(getRelativeTime(new Date(Date.now() + 3600000)));   // "dans 1 heure"
console.log(getRelativeTime(new Date(Date.now() - 86400000)));  // "hier"
```

### Date.parse() et Date.now()

```javascript
// Date.parse() - Convertir string en timestamp
const timestamp1 = Date.parse('2024-11-07');
console.log(timestamp1); // Millisecondes

const timestamp2 = Date.parse('November 7, 2024');
console.log(timestamp2);

// Utiliser pour créer Date
const date1 = new Date(Date.parse('2024-11-07'));
console.log(date1);

// Date invalide retourne NaN
const invalid = Date.parse('invalid');
console.log(invalid); // NaN
console.log(isNaN(invalid)); // true

// Date.now() - Timestamp actuel (plus rapide que new Date())
const now1 = Date.now();
console.log(now1);

const now2 = new Date().getTime();
console.log(now2);

// Date.now() est plus performant
console.time('Date.now()');
for (let i = 0; i < 1000000; i++) {
    Date.now();
}
console.timeEnd('Date.now()'); // Plus rapide

console.time('new Date()');
for (let i = 0; i < 1000000; i++) {
    new Date().getTime();
}
console.timeEnd('new Date()'); // Plus lent
```

---

## Exemples Pratiques Complets

### Calculer Différences entre Dates

```javascript
// Calculer différence en millisecondes
function diffInMs(date1, date2) {
    return Math.abs(date2 - date1);
}

// Calculer différence en secondes
function diffInSeconds(date1, date2) {
    return Math.floor(diffInMs(date1, date2) / 1000);
}

// Calculer différence en minutes
function diffInMinutes(date1, date2) {
    return Math.floor(diffInSeconds(date1, date2) / 60);
}

// Calculer différence en heures
function diffInHours(date1, date2) {
    return Math.floor(diffInMinutes(date1, date2) / 60);
}

// Calculer différence en jours
function diffInDays(date1, date2) {
    return Math.floor(diffInHours(date1, date2) / 24);
}

// Calculer différence en semaines
function diffInWeeks(date1, date2) {
    return Math.floor(diffInDays(date1, date2) / 7);
}

// Calculer différence en mois (approximatif)
function diffInMonths(date1, date2) {
    const d1 = new Date(date1);
    const d2 = new Date(date2);
    
    let months = (d2.getFullYear() - d1.getFullYear()) * 12;
    months += d2.getMonth() - d1.getMonth();
    
    return Math.abs(months);
}

// Calculer différence en années
function diffInYears(date1, date2) {
    const d1 = new Date(date1);
    const d2 = new Date(date2);
    
    return Math.abs(d2.getFullYear() - d1.getFullYear());
}

// Tests
const date1 = new Date('2024-01-01');
const date2 = new Date('2024-11-07');

console.log(diffInDays(date1, date2) + ' jours'); // 311 jours
console.log(diffInWeeks(date1, date2) + ' semaines'); // 44 semaines
console.log(diffInMonths(date1, date2) + ' mois'); // 10 mois

// Format human-readable
function formatDuration(date1, date2) {
    const days = diffInDays(date1, date2);
    const hours = diffInHours(date1, date2) % 24;
    const minutes = diffInMinutes(date1, date2) % 60;
    
    const parts = [];
    if (days > 0) parts.push(`${days} jour${days > 1 ? 's' : ''}`);
    if (hours > 0) parts.push(`${hours} heure${hours > 1 ? 's' : ''}`);
    if (minutes > 0) parts.push(`${minutes} minute${minutes > 1 ? 's' : ''}`);
    
    return parts.join(', ') || '0 minute';
}

const start = new Date('2024-11-07T10:00:00');
const end = new Date('2024-11-09T15:30:00');
console.log(formatDuration(start, end)); 
// "2 jours, 5 heures, 30 minutes"
```

### Validation et Vérification

```javascript
// Vérifier si date est valide
function isValidDate(date) {
    return date instanceof Date && !isNaN(date.getTime());
}

console.log(isValidDate(new Date())); // true
console.log(isValidDate(new Date('invalid'))); // false

// Vérifier si date est dans le futur
function isFuture(date) {
    return date > new Date();
}

console.log(isFuture(new Date('2025-01-01'))); // true
console.log(isFuture(new Date('2020-01-01'))); // false

// Vérifier si date est dans le passé
function isPast(date) {
    return date < new Date();
}

// Vérifier si date est aujourd'hui
function isToday(date) {
    const today = new Date();
    return date.toDateString() === today.toDateString();
}

console.log(isToday(new Date())); // true

// Vérifier si date est ce mois-ci
function isThisMonth(date) {
    const now = new Date();
    return date.getMonth() === now.getMonth() &&
           date.getFullYear() === now.getFullYear();
}

// Vérifier si date est cette année
function isThisYear(date) {
    return date.getFullYear() === new Date().getFullYear();
}

// Vérifier si deux dates sont le même jour
function isSameDay(date1, date2) {
    return date1.toDateString() === date2.toDateString();
}

// Vérifier si date est un weekend
function isWeekend(date) {
    const day = date.getDay();
    return day === 0 || day === 6;
}

// Vérifier si date est un jour ouvrable
function isWeekday(date) {
    return !isWeekend(date);
}

// Vérifier si date est dans une plage
function isInRange(date, start, end) {
    return date >= start && date <= end;
}

const checkDate = new Date('2024-11-07');
const rangeStart = new Date('2024-11-01');
const rangeEnd = new Date('2024-11-30');
console.log(isInRange(checkDate, rangeStart, rangeEnd)); // true
```

### Formatage Personnalisé

```javascript
// Formatter date avec pattern custom
function formatDatePattern(date, pattern) {
    const map = {
        'YYYY': date.getFullYear(),
        'YY': String(date.getFullYear()).slice(-2),
        'MM': String(date.getMonth() + 1).padStart(2, '0'),
        'M': date.getMonth() + 1,
        'DD': String(date.getDate()).padStart(2, '0'),
        'D': date.getDate(),
        'HH': String(date.getHours()).padStart(2, '0'),
        'H': date.getHours(),
        'hh': String(date.getHours() % 12 || 12).padStart(2, '0'),
        'h': date.getHours() % 12 || 12,
        'mm': String(date.getMinutes()).padStart(2, '0'),
        'm': date.getMinutes(),
        'ss': String(date.getSeconds()).padStart(2, '0'),
        's': date.getSeconds(),
        'A': date.getHours() >= 12 ? 'PM' : 'AM',
        'a': date.getHours() >= 12 ? 'pm' : 'am'
    };
    
    let result = pattern;
    for (const [key, value] of Object.entries(map)) {
        result = result.replace(key, value);
    }
    
    return result;
}

const date = new Date('2024-11-07T15:30:45');

console.log(formatDatePattern(date, 'DD/MM/YYYY')); // "07/11/2024"
console.log(formatDatePattern(date, 'YYYY-MM-DD')); // "2024-11-07"
console.log(formatDatePattern(date, 'DD/MM/YY HH:mm')); // "07/11/24 15:30"
console.log(formatDatePattern(date, 'hh:mm A')); // "03:30 PM"
console.log(formatDatePattern(date, 'D/M/YYYY à HH:mm:ss')); // "7/11/2024 à 15:30:45"

// Formater avec noms de mois et jours
function formatDateWithNames(date, format = 'full') {
    const days = ['Dimanche', 'Lundi', 'Mardi', 'Mercredi', 'Jeudi', 'Vendredi', 'Samedi'];
    const daysShort = ['Dim', 'Lun', 'Mar', 'Mer', 'Jeu', 'Ven', 'Sam'];
    const months = [
        'Janvier', 'Février', 'Mars', 'Avril', 'Mai', 'Juin',
        'Juillet', 'Août', 'Septembre', 'Octobre', 'Novembre', 'Décembre'
    ];
    const monthsShort = ['Jan', 'Fév', 'Mar', 'Avr', 'Mai', 'Juin', 
                         'Juil', 'Août', 'Sep', 'Oct', 'Nov', 'Déc'];
    
    const day = date.getDate();
    const dayName = days[date.getDay()];
    const dayNameShort = daysShort[date.getDay()];
    const month = months[date.getMonth()];
    const monthShort = monthsShort[date.getMonth()];
    const year = date.getFullYear();
    
    switch(format) {
        case 'full':
            return `${dayName} ${day} ${month} ${year}`;
        case 'short':
            return `${dayNameShort} ${day} ${monthShort} ${year}`;
        case 'medium':
            return `${day} ${month} ${year}`;
        case 'minimal':
            return `${day} ${monthShort}`;
        default:
            return date.toLocaleDateString('fr-FR');
    }
}

console.log(formatDateWithNames(date, 'full'));    // "Jeudi 7 Novembre 2024"
console.log(formatDateWithNames(date, 'short'));   // "Jeu 7 Nov 2024"
console.log(formatDateWithNames(date, 'medium'));  // "7 Novembre 2024"
console.log(formatDateWithNames(date, 'minimal')); // "7 Nov"

// Format "il y a X temps" (time ago)
function timeAgo(date) {
    const now = new Date();
    const seconds = Math.floor((now - date) / 1000);
    
    const intervals = {
        'an': 31536000,
        'mois': 2592000,
        'semaine': 604800,
        'jour': 86400,
        'heure': 3600,
        'minute': 60,
        'seconde': 1
    };
    
    for (const [name, secondsInInterval] of Object.entries(intervals)) {
        const interval = Math.floor(seconds / secondsInInterval);
        
        if (interval >= 1) {
            const plural = interval > 1 && name !== 'mois' ? 's' : '';
            return `il y a ${interval} ${name}${plural}`;
        }
    }
    
    return 'à l\'instant';
}

const pastDate1 = new Date(Date.now() - 30000); // 30 secondes
const pastDate2 = new Date(Date.now() - 3600000); // 1 heure
const pastDate3 = new Date(Date.now() - 86400000 * 5); // 5 jours

console.log(timeAgo(pastDate1)); // "il y a 30 secondes"
console.log(timeAgo(pastDate2)); // "il y a 1 heure"
console.log(timeAgo(pastDate3)); // "il y a 5 jours"

// Format "dans X temps" (time from now)
function timeFromNow(date) {
    const now = new Date();
    const seconds = Math.floor((date - now) / 1000);
    
    if (seconds < 0) {
        return timeAgo(date); // Si dans le passé
    }
    
    const intervals = {
        'an': 31536000,
        'mois': 2592000,
        'semaine': 604800,
        'jour': 86400,
        'heure': 3600,
        'minute': 60,
        'seconde': 1
    };
    
    for (const [name, secondsInInterval] of Object.entries(intervals)) {
        const interval = Math.floor(seconds / secondsInInterval);
        
        if (interval >= 1) {
            const plural = interval > 1 && name !== 'mois' ? 's' : '';
            return `dans ${interval} ${name}${plural}`;
        }
    }
    
    return 'maintenant';
}

const futureDate1 = new Date(Date.now() + 3600000); // 1 heure
const futureDate2 = new Date(Date.now() + 86400000 * 3); // 3 jours

console.log(timeFromNow(futureDate1)); // "dans 1 heure"
console.log(timeFromNow(futureDate2)); // "dans 3 jours"
```

### Calendrier et Planification

```javascript
// Obtenir tous les jours d'un mois
function getDaysInMonth(year, month) {
    const date = new Date(year, month, 1);
    const days = [];
    
    while (date.getMonth() === month) {
        days.push(new Date(date));
        date.setDate(date.getDate() + 1);
    }
    
    return days;
}

const novemberDays = getDaysInMonth(2024, 10); // Novembre 2024
console.log(`Novembre a ${novemberDays.length} jours`); // 30

// Obtenir tous les lundis d'un mois
function getMondaysInMonth(year, month) {
    const days = getDaysInMonth(year, month);
    return days.filter(day => day.getDay() === 1);
}

const mondaysInNov = getMondaysInMonth(2024, 10);
console.log(`${mondaysInNov.length} lundis en novembre`); // 4 ou 5

// Obtenir le Nième jour du mois (ex: 2ème lundi)
function getNthDayOfMonth(year, month, dayOfWeek, n) {
    const days = getDaysInMonth(year, month);
    const matchingDays = days.filter(day => day.getDay() === dayOfWeek);
    return matchingDays[n - 1] || null;
}

// 2ème lundi de novembre 2024
const secondMonday = getNthDayOfMonth(2024, 10, 1, 2);
console.log(secondMonday);

// Calculer date de Pâques (algorithme de Meeus)
function getEasterDate(year) {
    const a = year % 19;
    const b = Math.floor(year / 100);
    const c = year % 100;
    const d = Math.floor(b / 4);
    const e = b % 4;
    const f = Math.floor((b + 8) / 25);
    const g = Math.floor((b - f + 1) / 3);
    const h = (19 * a + b - d - g + 15) % 30;
    const i = Math.floor(c / 4);
    const k = c % 4;
    const l = (32 + 2 * e + 2 * i - h - k) % 7;
    const m = Math.floor((a + 11 * h + 22 * l) / 451);
    const month = Math.floor((h + l - 7 * m + 114) / 31) - 1;
    const day = ((h + l - 7 * m + 114) % 31) + 1;
    
    return new Date(year, month, day);
}

console.log(getEasterDate(2024)); // Date de Pâques 2024

// Jours fériés français
function getFrenchHolidays(year) {
    const easter = getEasterDate(year);
    
    return {
        'Jour de l\'an': new Date(year, 0, 1),
        'Lundi de Pâques': new Date(easter.getTime() + 86400000),
        'Fête du Travail': new Date(year, 4, 1),
        'Victoire 1945': new Date(year, 4, 8),
        'Ascension': new Date(easter.getTime() + 39 * 86400000),
        'Lundi de Pentecôte': new Date(easter.getTime() + 50 * 86400000),
        'Fête Nationale': new Date(year, 6, 14),
        'Assomption': new Date(year, 7, 15),
        'Toussaint': new Date(year, 10, 1),
        'Armistice 1918': new Date(year, 10, 11),
        'Noël': new Date(year, 11, 25)
    };
}

const holidays2024 = getFrenchHolidays(2024);
console.log(holidays2024);

// Vérifier si une date est un jour férié
function isHoliday(date, year = date.getFullYear()) {
    const holidays = Object.values(getFrenchHolidays(year));
    return holidays.some(holiday => 
        holiday.toDateString() === date.toDateString()
    );
}

console.log(isHoliday(new Date('2024-07-14'))); // true (14 juillet)
console.log(isHoliday(new Date('2024-07-15'))); // false

// Calculer nombre de jours ouvrables entre deux dates
function getBusinessDays(startDate, endDate) {
    let count = 0;
    const current = new Date(startDate);
    
    while (current <= endDate) {
        const day = current.getDay();
        // Lundi à vendredi (1-5) et pas férié
        if (day !== 0 && day !== 6 && !isHoliday(current)) {
            count++;
        }
        current.setDate(current.getDate() + 1);
    }
    
    return count;
}

const start = new Date('2024-11-01');
const end = new Date('2024-11-30');
console.log(`Jours ouvrables: ${getBusinessDays(start, end)}`);

// Ajouter X jours ouvrables
function addBusinessDays(date, days) {
    const result = new Date(date);
    let added = 0;
    
    while (added < days) {
        result.setDate(result.getDate() + 1);
        const day = result.getDay();
        
        if (day !== 0 && day !== 6 && !isHoliday(result)) {
            added++;
        }
    }
    
    return result;
}

const today = new Date('2024-11-07');
const deliveryDate = addBusinessDays(today, 5);
console.log(`Livraison dans 5 jours ouvrables: ${deliveryDate.toLocaleDateString('fr-FR')}`);
```

### Gestion de Fuseaux Horaires

```javascript
// Obtenir offset du timezone en heures
function getTimezoneOffset(date = new Date()) {
    return -date.getTimezoneOffset() / 60;
}

console.log(`Timezone: UTC${getTimezoneOffset() >= 0 ? '+' : ''}${getTimezoneOffset()}`);
// Ex: "Timezone: UTC+1"

// Convertir date en autre timezone
function convertToTimezone(date, targetOffset) {
    const currentOffset = date.getTimezoneOffset() / 60;
    const diff = targetOffset - (-currentOffset);
    
    const newDate = new Date(date);
    newDate.setHours(newDate.getHours() + diff);
    
    return newDate;
}

const paris = new Date('2024-11-07T15:00:00');
const newYork = convertToTimezone(paris, -5); // UTC-5
console.log('Paris:', paris.toLocaleString());
console.log('New York:', newYork.toLocaleString());

// Obtenir date/heure dans timezone spécifique avec Intl
function getDateInTimezone(date, timezone) {
    return new Date(date.toLocaleString('en-US', { timeZone: timezone }));
}

const now = new Date();
console.log('Paris:', getDateInTimezone(now, 'Europe/Paris').toLocaleString());
console.log('New York:', getDateInTimezone(now, 'America/New_York').toLocaleString());
console.log('Tokyo:', getDateInTimezone(now, 'Asia/Tokyo').toLocaleString());

// Formater avec timezone
const dateWithTz = new Date('2024-11-07T15:00:00');
const options = {
    timeZone: 'America/New_York',
    year: 'numeric',
    month: 'long',
    day: 'numeric',
    hour: '2-digit',
    minute: '2-digit',
    timeZoneName: 'short'
};

console.log(dateWithTz.toLocaleString('en-US', options));
// "November 7, 2024, 09:00 AM EST"

// Comparer dates de différents timezones
function isSameUTCTime(date1, tz1, date2, tz2) {
    const utc1 = new Date(date1.toLocaleString('en-US', { timeZone: 'UTC' }));
    const utc2 = new Date(date2.toLocaleString('en-US', { timeZone: 'UTC' }));
    return utc1.getTime() === utc2.getTime();
}
```

### Parsing et Validation

```javascript
// Parser date depuis format custom
function parseDate(dateStr, format) {
    // Supports: DD/MM/YYYY, DD-MM-YYYY, YYYY-MM-DD, etc.
    const parts = dateStr.split(/[-\/]/);
    
    if (format === 'DD/MM/YYYY' || format === 'DD-MM-YYYY') {
        return new Date(parts[2], parts[1] - 1, parts[0]);
    } else if (format === 'MM/DD/YYYY' || format === 'MM-DD-YYYY') {
        return new Date(parts[2], parts[0] - 1, parts[1]);
    } else if (format === 'YYYY-MM-DD' || format === 'YYYY/MM/DD') {
        return new Date(parts[0], parts[1] - 1, parts[2]);
    }
    
    return null;
}

console.log(parseDate('07/11/2024', 'DD/MM/YYYY'));
console.log(parseDate('2024-11-07', 'YYYY-MM-DD'));

// Valider format de date string
function isValidDateString(dateStr, format) {
    const date = parseDate(dateStr, format);
    return date && isValidDate(date);
}

console.log(isValidDateString('07/11/2024', 'DD/MM/YYYY')); // true
console.log(isValidDateString('32/13/2024', 'DD/MM/YYYY')); // false

// Parser date relative ("demain", "dans 3 jours", etc.)
function parseRelativeDate(str) {
    const now = new Date();
    str = str.toLowerCase().trim();
    
    if (str === 'aujourd\'hui' || str === 'today') {
        return now;
    }
    
    if (str === 'hier' || str === 'yesterday') {
        return new Date(now.getTime() - 86400000);
    }
    
    if (str === 'demain' || str === 'tomorrow') {
        return new Date(now.getTime() + 86400000);
    }
    
    // "dans X jours/semaines/mois/ans"
    const inMatch = str.match(/dans (\d+) (jour|semaine|mois|an)/);
    if (inMatch) {
        const amount = parseInt(inMatch[1]);
        const unit = inMatch[2];
        
        const result = new Date(now);
        switch(unit) {
            case 'jour':
                result.setDate(result.getDate() + amount);
                break;
            case 'semaine':
                result.setDate(result.getDate() + amount * 7);
                break;
            case 'mois':
                result.setMonth(result.getMonth() + amount);
                break;
            case 'an':
                result.setFullYear(result.getFullYear() + amount);
                break;
        }
        return result;
    }
    
    // "il y a X jours/semaines/mois/ans"
    const agoMatch = str.match(/il y a (\d+) (jour|semaine|mois|an)/);
    if (agoMatch) {
        const amount = parseInt(agoMatch[1]);
        const unit = agoMatch[2];
        
        const result = new Date(now);
        switch(unit) {
            case 'jour':
                result.setDate(result.getDate() - amount);
                break;
            case 'semaine':
                result.setDate(result.getDate() - amount * 7);
                break;
            case 'mois':
                result.setMonth(result.getMonth() - amount);
                break;
            case 'an':
                result.setFullYear(result.getFullYear() - amount);
                break;
        }
        return result;
    }
    
    return null;
}

console.log(parseRelativeDate('aujourd\'hui'));
console.log(parseRelativeDate('dans 5 jours'));
console.log(parseRelativeDate('il y a 2 semaines'));
```

### Classe Date Helper Complète

```javascript
// Classe utilitaire complète pour manipuler dates
class DateHelper {
    // Formater date
    static format(date, pattern = 'DD/MM/YYYY') {
        const map = {
            'YYYY': date.getFullYear(),
            'YY': String(date.getFullYear()).slice(-2),
            'MM': String(date.getMonth() + 1).padStart(2, '0'),
            'M': date.getMonth() + 1,
            'DD': String(date.getDate()).padStart(2, '0'),
            'D': date.getDate(),
            'HH': String(date.getHours()).padStart(2, '0'),
            'H': date.getHours(),
            'mm': String(date.getMinutes()).padStart(2, '0'),
            'm': date.getMinutes(),
            'ss': String(date.getSeconds()).padStart(2, '0'),
            's': date.getSeconds()
        };
        
        let result = pattern;
        for (const [key, value] of Object.entries(map)) {
            result = result.replace(key, value);
        }
        return result;
    }
    
    // Ajouter/soustraire temps
    static add(date, amount, unit) {
        const result = new Date(date);
        
        switch(unit) {
            case 'years':
                result.setFullYear(result.getFullYear() + amount);
                break;
            case 'months':
                result.setMonth(result.getMonth() + amount);
                break;
            case 'weeks':
                result.setDate(result.getDate() + amount * 7);
                break;
            case 'days':
                result.setDate(result.getDate() + amount);
                break;
            case 'hours':
                result.setHours(result.getHours() + amount);
                break;
            case 'minutes':
                result.setMinutes(result.getMinutes() + amount);
                break;
            case 'seconds':
                result.setSeconds(result.getSeconds() + amount);
                break;
        }
        
        return result;
    }
    
    // Soustraire
    static subtract(date, amount, unit) {
        return this.add(date, -amount, unit);
    }
    
    // Différence entre dates
    static diff(date1, date2, unit = 'days') {
        const ms = Math.abs(date2 - date1);
        
        switch(unit) {
            case 'years':
                return Math.floor(ms / (1000 * 60 * 60 * 24 * 365));
            case 'months':
                return Math.floor(ms / (1000 * 60 * 60 * 24 * 30));
            case 'weeks':
                return Math.floor(ms / (1000 * 60 * 60 * 24 * 7));
            case 'days':
                return Math.floor(ms / (1000 * 60 * 60 * 24));
            case 'hours':
                return Math.floor(ms / (1000 * 60 * 60));
            case 'minutes':
                return Math.floor(ms / (1000 * 60));
            case 'seconds':
                return Math.floor(ms / 1000);
            default:
                return ms;
        }
    }
    
    // Début de période
    static startOf(date, unit) {
        const result = new Date(date);
        
        switch(unit) {
            case 'year':
                result.setMonth(0, 1);
                result.setHours(0, 0, 0, 0);
                break;
            case 'month':
                result.setDate(1);
                result.setHours(0, 0, 0, 0);
                break;
            case 'week':
                const day = result.getDay();
                const diff = result.getDate() - day + (day === 0 ? -6 : 1);
                result.setDate(diff);
                result.setHours(0, 0, 0, 0);
                break;
            case 'day':
                result.setHours(0, 0, 0, 0);
                break;
            case 'hour':
                result.setMinutes(0, 0, 0);
                break;
            case 'minute':
                result.setSeconds(0, 0);
                break;
        }
        
        return result;
    }
    
    // Fin de période
    static endOf(date, unit) {
        const result = new Date(date);
        
        switch(unit) {
            case 'year':
                result.setMonth(11, 31);
                result.setHours(23, 59, 59, 999);
                break;
            case 'month':
                result.setMonth(result.getMonth() + 1, 0);
                result.setHours(23, 59, 59, 999);
                break;
            case 'week':
                const start = this.startOf(date, 'week');
                result.setTime(start.getTime() + 7 * 24 * 60 * 60 * 1000 - 1);
                break;
            case 'day':
                result.setHours(23, 59, 59, 999);
                break;
            case 'hour':
                result.setMinutes(59, 59, 999);
                break;
            case 'minute':
                result.setSeconds(59, 999);
                break;
        }
        
        return result;
    }
    
    // Vérifications
    static isToday(date) {
        return this.format(date, 'YYYY-MM-DD') === this.format(new Date(), 'YYYY-MM-DD');
    }
    
    static isFuture(date) {
        return date > new Date();
    }
    
    static isPast(date) {
        return date < new Date();
    }
    
    static isWeekend(date) {
        const day = date.getDay();
        return day === 0 || day === 6;
    }
    
    static isBetween(date, start, end) {
        return date >= start && date <= end;
    }
}

// Utilisation
const now = new Date();

console.log(DateHelper.format(now, 'DD/MM/YYYY HH:mm'));
console.log(DateHelper.add(now, 5, 'days'));
console.log(DateHelper.diff(now, new Date('2025-01-01'), 'days'));
console.log(DateHelper.startOf(now, 'month'));
console.log(DateHelper.endOf(now, 'month'));
console.log(DateHelper.isToday(now));
console.log(DateHelper.isWeekend(now));
```

---

**Points clés à retenir:**
- `Date` object pour travailler avec dates/heures
- Mois commencent à 0 (0 = janvier)
- Timestamp = millisecondes depuis 1970
- Get methods pour lire, Set methods pour modifier
- `toLocaleString()` pour formatage localisé
- Toujours créer copie avant modification
- Attention aux timezones (UTC vs local)

**Pour React:**
- Dates souvent en state pour countdowns, calendriers
- Formater pour affichage utilisateur
- Librairies populaires: date-fns, moment, dayjs
- `useEffect` pour timers et intervals

---

# Questions/Réponses - Chapitre 13 : Date et Time

## Questions Niveau Junior

### Q1: Comment créer une date en JavaScript et quelle est la différence entre les différentes méthodes ?

**Réponse:**
```javascript
// 1. Date actuelle (maintenant)
const now = new Date();
console.log(now); // Date et heure actuelles

// 2. Date à partir d'une string
const date1 = new Date('2024-11-07');
console.log(date1); // 2024-11-07T00:00:00.000Z

const date2 = new Date('2024-11-07T15:30:00');
console.log(date2); // 2024-11-07T15:30:00.000Z

// 3. Date avec paramètres (année, mois, jour, heure, minute, seconde, ms)
// ⚠️ ATTENTION: Les mois commencent à 0 !
// 0 = janvier, 1 = février, ..., 11 = décembre
const date3 = new Date(2024, 10, 7); // 7 NOVEMBRE 2024 (mois 10 = novembre)
console.log(date3);

const date4 = new Date(2024, 10, 7, 15, 30, 0); // 7 nov 2024, 15:30:00
console.log(date4);

// 4. Date à partir d'un timestamp (millisecondes depuis 1970)
const timestamp = 1699368600000;
const date5 = new Date(timestamp);
console.log(date5);

// ERREUR COMMUNE: Mois commence à 0
const wrongDate = new Date(2024, 11, 7); // 7 DÉCEMBRE (pas novembre!)
const correctDate = new Date(2024, 10, 7); // 7 NOVEMBRE

console.log('Wrong:', wrongDate.toLocaleDateString()); // 07/12/2024
console.log('Correct:', correctDate.toLocaleDateString()); // 07/11/2024

// Vérifier si date est valide
function isValidDate(d) {
    return d instanceof Date && !isNaN(d);
}

const valid = new Date('2024-11-07');
const invalid = new Date('invalid string');

console.log(isValidDate(valid));   // true
console.log(isValidDate(invalid)); // false
console.log(invalid.toString());   // "Invalid Date"

// Date epoch (1er janvier 1970)
const epoch = new Date(0);
console.log(epoch); // 1970-01-01T00:00:00.000Z
```

**Points clés:**
- `new Date()` = maintenant
- `new Date(string)` = parser string
- `new Date(year, month, day, ...)` = paramètres (mois à 0!)
- `new Date(timestamp)` = millisecondes depuis 1970
- Toujours vérifier validité avec `isNaN(date)`
- **Piège:** mois commence à 0 (janvier = 0)

---

### Q2: Quelle est la différence entre `getMonth()` et `getDate()` ?

**Réponse:**
```javascript
const date = new Date('2024-11-07T15:30:00');

// getMonth() - Retourne le MOIS (0-11)
// 0 = janvier, 1 = février, ..., 11 = décembre
console.log(date.getMonth()); // 10 (novembre)

// Pour obtenir le numéro "humain" du mois, ajouter +1
console.log(date.getMonth() + 1); // 11 (novembre en format humain)

// getDate() - Retourne le JOUR du mois (1-31)
console.log(date.getDate()); // 7

// getDay() - Retourne le JOUR de la semaine (0-6)
// 0 = dimanche, 1 = lundi, ..., 6 = samedi
console.log(date.getDay()); // 4 (jeudi)

// Exemples pratiques

// Afficher mois en toutes lettres
function getMonthName(date) {
    const months = [
        'Janvier', 'Février', 'Mars', 'Avril', 'Mai', 'Juin',
        'Juillet', 'Août', 'Septembre', 'Octobre', 'Novembre', 'Décembre'
    ];
    return months[date.getMonth()]; // Utiliser getMonth() comme index
}

console.log(getMonthName(date)); // "Novembre"

// Afficher jour de la semaine
function getDayName(date) {
    const days = ['Dimanche', 'Lundi', 'Mardi', 'Mercredi', 
                  'Jeudi', 'Vendredi', 'Samedi'];
    return days[date.getDay()]; // Utiliser getDay() comme index
}

console.log(getDayName(date)); // "Jeudi"

// Formater date complète
function formatFrenchDate(date) {
    const day = date.getDate();
    const month = getMonthName(date);
    const year = date.getFullYear();
    const dayName = getDayName(date);
    
    return `${dayName} ${day} ${month} ${year}`;
}

console.log(formatFrenchDate(date)); 
// "Jeudi 7 Novembre 2024"

// ERREUR COMMUNE: Confondre getDate() et getDay()
const myDate = new Date('2024-11-07');
console.log(myDate.getDate()); // 7 (jour du mois) ✅
console.log(myDate.getDay());  // 4 (jeudi) ✅

// Ne pas confondre avec:
console.log(myDate.getMonth()); // 10 (novembre, commence à 0!)

// Récapitulatif
const d = new Date('2024-11-07');
console.log('Année:', d.getFullYear());    // 2024
console.log('Mois (0-11):', d.getMonth()); // 10
console.log('Jour du mois:', d.getDate()); // 7
console.log('Jour semaine:', d.getDay());  // 4 (jeudi)
```

**Points clés:**
- `getMonth()` = mois (0-11), ajouter +1 pour humain
- `getDate()` = jour du mois (1-31)
- `getDay()` = jour de la semaine (0-6, 0 = dimanche)
- Ne pas confondre ces trois méthodes
- Utiliser comme index pour arrays de noms

---

### Q3: Comment formater une date au format DD/MM/YYYY ?

**Réponse:**
```javascript
const date = new Date('2024-11-07T15:30:45');

// MÉTHODE 1: Manuelle avec padStart
function formatDate1(date) {
    const day = String(date.getDate()).padStart(2, '0');
    const month = String(date.getMonth() + 1).padStart(2, '0'); // +1 car mois commence à 0
    const year = date.getFullYear();
    
    return `${day}/${month}/${year}`;
}

console.log(formatDate1(date)); // "07/11/2024"

// MÉTHODE 2: toLocaleDateString()
const formatted2 = date.toLocaleDateString('fr-FR');
console.log(formatted2); // "07/11/2024"

const formatted3 = date.toLocaleDateString('en-US');
console.log(formatted3); // "11/7/2024"

// MÉTHODE 3: toLocaleDateString() avec options
const options = {
    year: 'numeric',
    month: '2-digit',
    day: '2-digit'
};

const formatted4 = date.toLocaleDateString('fr-FR', options);
console.log(formatted4); // "07/11/2024"

// Autres formats courants

// Format ISO: YYYY-MM-DD
function formatISO(date) {
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    
    return `${year}-${month}-${day}`;
}

console.log(formatISO(date)); // "2024-11-07"

// Format US: MM/DD/YYYY
function formatUS(date) {
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    const year = date.getFullYear();
    
    return `${month}/${day}/${year}`;
}

console.log(formatUS(date)); // "11/07/2024"

// Format avec heure: DD/MM/YYYY HH:MM
function formatDateTime(date) {
    const dateStr = formatDate1(date);
    const hours = String(date.getHours()).padStart(2, '0');
    const minutes = String(date.getMinutes()).padStart(2, '0');
    
    return `${dateStr} ${hours}:${minutes}`;
}

console.log(formatDateTime(date)); // "07/11/2024 15:30"

// Format long en français
const longFormat = date.toLocaleDateString('fr-FR', {
    weekday: 'long',
    year: 'numeric',
    month: 'long',
    day: 'numeric'
});
console.log(longFormat); // "jeudi 7 novembre 2024"

// Fonction générique avec pattern
function formatDatePattern(date, pattern) {
    const map = {
        'DD': String(date.getDate()).padStart(2, '0'),
        'D': date.getDate(),
        'MM': String(date.getMonth() + 1).padStart(2, '0'),
        'M': date.getMonth() + 1,
        'YYYY': date.getFullYear(),
        'YY': String(date.getFullYear()).slice(-2),
        'HH': String(date.getHours()).padStart(2, '0'),
        'mm': String(date.getMinutes()).padStart(2, '0'),
        'ss': String(date.getSeconds()).padStart(2, '0')
    };
    
    let result = pattern;
    for (const [key, value] of Object.entries(map)) {
        result = result.replace(key, value);
    }
    
    return result;
}

console.log(formatDatePattern(date, 'DD/MM/YYYY'));      // "07/11/2024"
console.log(formatDatePattern(date, 'YYYY-MM-DD'));      // "2024-11-07"
console.log(formatDatePattern(date, 'DD/MM/YY HH:mm'));  // "07/11/24 15:30"
console.log(formatDatePattern(date, 'D/M/YYYY'));        // "7/11/2024"

// Pour formulaires HTML (input type="date")
function formatForInput(date) {
    return date.toISOString().split('T')[0];
}

console.log(formatForInput(date)); // "2024-11-07"
```

**Points clés:**
- `padStart(2, '0')` pour ajouter zéro devant
- `getMonth() + 1` car mois commence à 0
- `toLocaleDateString()` pour formatage automatique
- Options pour personnaliser format
- Créer fonction réutilisable avec pattern

---

### Q4: Comment calculer la différence entre deux dates en jours ?

**Réponse:**
```javascript
// Calculer différence en millisecondes puis convertir

const date1 = new Date('2024-11-01');
const date2 = new Date('2024-11-07');

// Différence en millisecondes
const diffMs = date2 - date1;
console.log(diffMs); // 518400000 ms

// Convertir en jours
const diffDays = diffMs / (1000 * 60 * 60 * 24);
console.log(diffDays); // 6 jours

// Fonction réutilisable
function daysBetween(date1, date2) {
    const ms = Math.abs(date2 - date1); // abs pour avoir valeur positive
    return Math.floor(ms / (1000 * 60 * 60 * 24));
}

console.log(daysBetween(date1, date2)); // 6

// Autres unités de temps

// Différence en heures
function hoursBetween(date1, date2) {
    const ms = Math.abs(date2 - date1);
    return Math.floor(ms / (1000 * 60 * 60));
}

const now = new Date();
const tomorrow = new Date(now.getTime() + 24 * 60 * 60 * 1000);
console.log(hoursBetween(now, tomorrow)); // 24

// Différence en minutes
function minutesBetween(date1, date2) {
    const ms = Math.abs(date2 - date1);
    return Math.floor(ms / (1000 * 60));
}

// Différence en secondes
function secondsBetween(date1, date2) {
    const ms = Math.abs(date2 - date1);
    return Math.floor(ms / 1000);
}

// Exemple pratique: Calculer âge
function calculateAge(birthDate) {
    const today = new Date();
    const birth = new Date(birthDate);
    
    let age = today.getFullYear() - birth.getFullYear();
    const monthDiff = today.getMonth() - birth.getMonth();
    
    // Ajuster si anniversaire pas encore passé cette année
    if (monthDiff < 0 || (monthDiff === 0 && today.getDate() < birth.getDate())) {
        age--;
    }
    
    return age;
}

console.log(calculateAge('1990-05-15')); // Ex: 34 ans

// Format human-readable de la différence
function formatDuration(date1, date2) {
    const ms = Math.abs(date2 - date1);
    
    const days = Math.floor(ms / (1000 * 60 * 60 * 24));
    const hours = Math.floor((ms % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
    const minutes = Math.floor((ms % (1000 * 60 * 60)) / (1000 * 60));
    
    const parts = [];
    if (days > 0) parts.push(`${days} jour${days > 1 ? 's' : ''}`);
    if (hours > 0) parts.push(`${hours} heure${hours > 1 ? 's' : ''}`);
    if (minutes > 0) parts.push(`${minutes} minute${minutes > 1 ? 's' : ''}`);
    
    return parts.join(', ') || '0 minute';
}

const start = new Date('2024-11-07T10:00:00');
const end = new Date('2024-11-09T15:30:00');
console.log(formatDuration(start, end)); 
// "2 jours, 5 heures, 30 minutes"

// Jours ouvrables (lundi-vendredi)
function businessDaysBetween(date1, date2) {
    let count = 0;
    const current = new Date(date1);
    const end = new Date(date2);
    
    while (current <= end) {
        const day = current.getDay();
        // 1-5 = lundi à vendredi
        if (day !== 0 && day !== 6) {
            count++;
        }
        current.setDate(current.getDate() + 1);
    }
    
    return count;
}

const monday = new Date('2024-11-04'); // Lundi
const friday = new Date('2024-11-08'); // Vendredi
console.log(businessDaysBetween(monday, friday)); // 5 jours ouvrables

// Temps restant jusqu'à une date
function timeUntil(targetDate) {
    const now = new Date();
    const target = new Date(targetDate);
    const ms = target - now;
    
    if (ms < 0) {
        return 'Date passée';
    }
    
    const days = Math.floor(ms / (1000 * 60 * 60 * 24));
    const hours = Math.floor((ms % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
    const minutes = Math.floor((ms % (1000 * 60 * 60)) / (1000 * 60));
    const seconds = Math.floor((ms % (1000 * 60)) / 1000);
    
    return `${days}j ${hours}h ${minutes}m ${seconds}s`;
}

console.log(timeUntil('2024-12-25T00:00:00')); 
// Ex: "48j 8h 30m 15s" (jusqu'à Noël)
```

**Points clés:**
- Soustraire dates donne millisecondes
- Diviser par `(1000 * 60 * 60 * 24)` pour jours
- `Math.abs()` pour valeur positive
- `Math.floor()` pour arrondir
- Attention aux jours ouvrables vs calendrier

---

### Q5: Comment ajouter ou soustraire des jours à une date ?

**Réponse:**
```javascript
// AJOUTER DES JOURS

const today = new Date('2024-11-07');

// Méthode 1: Avec setDate()
const tomorrow = new Date(today);
tomorrow.setDate(tomorrow.getDate() + 1);
console.log(tomorrow.toLocaleDateString()); // 08/11/2024

// Méthode 2: Avec timestamp
const dayInMs = 24 * 60 * 60 * 1000;
const inOneWeek = new Date(today.getTime() + 7 * dayInMs);
console.log(inOneWeek.toLocaleDateString()); // 14/11/2024

// Fonction réutilisable
function addDays(date, days) {
    const result = new Date(date);
    result.setDate(result.getDate() + days);
    return result;
}

console.log(addDays(today, 5).toLocaleDateString()); // 12/11/2024
console.log(addDays(today, 30).toLocaleDateString()); // 07/12/2024

// SOUSTRAIRE DES JOURS (jours négatifs)
const yesterday = addDays(today, -1);
console.log(yesterday.toLocaleDateString()); // 06/11/2024

const lastWeek = addDays(today, -7);
console.log(lastWeek.toLocaleDateString()); // 31/10/2024

// AJOUTER DES MOIS
function addMonths(date, months) {
    const result = new Date(date);
    result.setMonth(result.getMonth() + months);
    return result;
}

console.log(addMonths(today, 1).toLocaleDateString()); // 07/12/2024
console.log(addMonths(today, 3).toLocaleDateString()); // 07/02/2025

// AJOUTER DES ANNÉES
function addYears(date, years) {
    const result = new Date(date);
    result.setFullYear(result.getFullYear() + years);
    return result;
}

console.log(addYears(today, 1).toLocaleDateString()); // 07/11/2025
console.log(addYears(today, -2).toLocaleDateString()); // 07/11/2022

// AJOUTER HEURES/MINUTES/SECONDES
function addHours(date, hours) {
    const result = new Date(date);
    result.setHours(result.getHours() + hours);
    return result;
}

function addMinutes(date, minutes) {
    const result = new Date(date);
    result.setMinutes(result.getMinutes() + minutes);
    return result;
}

const now = new Date('2024-11-07T15:30:00');
console.log(addHours(now, 2));    // 17:30:00
console.log(addMinutes(now, 45)); // 16:15:00

// ⚠️ ATTENTION: setDate() modifie l'objet original!
const original = new Date('2024-11-07');
original.setDate(original.getDate() + 5);
console.log(original); // Modifié!

// ✅ TOUJOURS créer une copie
function safAddDays(date, days) {
    const copy = new Date(date); // Créer copie
    copy.setDate(copy.getDate() + days);
    return copy;
}

const date1 = new Date('2024-11-07');
const date2 = safeAddDays(date1, 5);
console.log(date1.toLocaleDateString()); // 07/11/2024 (inchangé)
console.log(date2.toLocaleDateString()); // 12/11/2024

// Exemples pratiques

// Date de livraison (5 jours ouvrables)
function addBusinessDays(date, days) {
    const result = new Date(date);
    let added = 0;
    
    while (added < days) {
        result.setDate(result.getDate() + 1);
        const dayOfWeek = result.getDay();
        
        // Compter seulement lundi-vendredi
        if (dayOfWeek !== 0 && dayOfWeek !== 6) {
            added++;
        }
    }
    
    return result;
}

const orderDate = new Date('2024-11-07'); // Jeudi
const deliveryDate = addBusinessDays(orderDate, 5);
console.log('Livraison:', deliveryDate.toLocaleDateString());

// Date d'expiration (ex: 30 jours)
function getExpirationDate(startDate, days = 30) {
    return addDays(startDate, days);
}

const subscriptionStart = new Date();
const subscriptionEnd = getExpirationDate(subscriptionStart, 30);
console.log('Expire le:', subscriptionEnd.toLocaleDateString());

// Prochain anniversaire
function getNextBirthday(birthDate) {
    const today = new Date();
    const birth = new Date(birthDate);
    
    const nextBirthday = new Date(
        today.getFullYear(),
        birth.getMonth(),
        birth.getDate()
    );
    
    // Si déjà passé cette année, prendre année suivante
    if (nextBirthday < today) {
        nextBirthday.setFullYear(nextBirthday.getFullYear() + 1);
    }
    
    return nextBirthday;
}

console.log(getNextBirthday('1990-05-15'));

// Fin de période d'essai (14 jours)
function getTrialEndDate(startDate) {
    return addDays(startDate, 14);
}
```

**Points clés:**
- `setDate(date.getDate() + X)` pour ajouter jours
- `setMonth()` pour mois, `setFullYear()` pour années
- Nombres négatifs pour soustraire
- **Toujours créer copie** avec `new Date(date)`
- JavaScript gère automatiquement changement de mois/année

---

## Questions Niveau Intermédiaire

### Q6: Comment comparer deux dates en JavaScript ?

**Réponse:**
```javascript
const date1 = new Date('2024-11-07');
const date2 = new Date('2024-11-10');
const date3 = new Date('2024-11-07');

// COMPARAISON DIRECTE avec opérateurs
console.log(date1 < date2);  // true
console.log(date1 > date2);  // false
console.log(date1 <= date3); // true
console.log(date1 >= date3); // true

// ⚠️ ATTENTION: == et === ne fonctionnent pas comme attendu
console.log(date1 == date3);  // false (objets différents)
console.log(date1 === date3); // false (références différentes)

// ✅ COMPARER AVEC getTime() ou valueOf()
console.log(date1.getTime() === date3.getTime()); // true
console.log(date1.valueOf() === date3.valueOf()); // true

// Fonctions de comparaison

// Vérifier si deux dates sont identiques
function isSameDate(date1, date2) {
    return date1.getTime() === date2.getTime();
}

console.log(isSameDate(date1, date3)); // true

// Vérifier si même jour (ignorer heure)
function isSameDay(date1, date2) {
    return date1.getFullYear() === date2.getFullYear() &&
           date1.getMonth() === date2.getMonth() &&
           date1.getDate() === date2.getDate();
}

const morning = new Date('2024-11-07T08:00:00');
const evening = new Date('2024-11-07T20:00:00');
console.log(isSameDay(morning, evening)); // true

// Alternative avec toDateString()
function isSameDayAlt(date1, date2) {
    return date1.toDateString() === date2.toDateString();
}

// Vérifier si date1 est après date2
function isAfter(date1, date2) {
    return date1 > date2;
}

console.log(isAfter(date2, date1)); // true

// Vérifier si date1 est avant date2
function isBefore(date1, date2) {
    return date1 < date2;
}

console.log(isBefore(date1, date2)); // true

// Vérifier si date est dans une plage
function isBetween(date, start, end) {
    return date >= start && date <= end;
}

const checkDate = new Date('2024-11-08');
const rangeStart = new Date('2024-11-01');
const rangeEnd = new Date('2024-11-30');

console.log(isBetween(checkDate, rangeStart, rangeEnd)); // true

// Vérifier si date est aujourd'hui
function isToday(date) {
    const today = new Date();
    return isSameDay(date, today);
}

console.log(isToday(new Date())); // true

// Vérifier si date est dans le futur
function isFuture(date) {
    return date > new Date();
}

console.log(isFuture(new Date('2025-01-01'))); // true

// Vérifier si date est dans le passé
function isPast(date) {
    return date < new Date();
}

console.log(isPast(new Date('2020-01-01'))); // true

// Trouver la date la plus récente
function getLatestDate(...dates) {
    return new Date(Math.max(...dates.map(d => d.getTime())));
}

const dates = [
    new Date('2024-11-05'),
    new Date('2024-11-10'),
    new Date('2024-11-03')
];

console.log(getLatestDate(...dates)); // 2024-11-10

// Trouver la date la plus ancienne
function getEarliestDate(...dates) {
    return new Date(Math.min(...dates.map(d => d.getTime())));
}

console.log(getEarliestDate(...dates)); // 2024-11-03

// Trier un array de dates
const unsortedDates = [
    new Date('2024-11-10'),
    new Date('2024-11-05'),
    new Date('2024-11-15'),
    new Date('2024-11-03')
];

// Tri croissant (du plus ancien au plus récent)
const sortedAsc = unsortedDates.sort((a, b) => a - b);
console.log(sortedAsc);

// Tri décroissant (du plus récent au plus ancien)
const sortedDesc = unsortedDates.sort((a, b) => b - a);
console.log(sortedDesc);

// Exemple pratique: Vérifier si réservation est valide
function isValidReservation(checkIn, checkOut) {
    const now = new Date();
    
    // Check-in doit être dans le futur
    if (checkIn <= now) {
        return { valid: false, error: 'Check-in doit être futur' };
    }
    
    // Check-out doit être après check-in
    if (checkOut <= checkIn) {
        return { valid: false, error: 'Check-out doit être après check-in' };
    }
    
    // Minimum 1 nuit
    const nights = (checkOut - checkIn) / (1000 * 60 * 60 * 24);
    if (nights < 1) {
        return { valid: false, error: 'Minimum 1 nuit' };
    }
    
    return { valid: true, nights: Math.floor(nights) };
}

const checkIn = new Date('2024-12-20');
const checkOut = new Date('2024-12-25');
console.log(isValidReservation(checkIn, checkOut));
// { valid: true, nights: 5 }
```

**Points clés:**
- Utiliser `<`, `>`, `<=`, `>=` pour comparer
- `==` et `===` ne fonctionnent pas (objets différents)
- Comparer timestamps avec `getTime()` pour égalité
- `toDateString()` pour comparer jours (ignorer heure)
- `Math.max/min` pour trouver date extrême
- Tri avec `.sort((a, b) => a - b)`

---

### Q7: Comment obtenir le début et la fin d'un mois/année ?

**Réponse:**
```javascript
const today = new Date('2024-11-07T15:30:00');

// DÉBUT DU MOIS
function startOfMonth(date) {
    const result = new Date(date);
    result.setDate(1); // Premier jour du mois
    result.setHours(0, 0, 0, 0); // Minuit
    return result;
}

console.log(startOfMonth(today)); 
// 2024-11-01T00:00:00.000Z

// FIN DU MOIS
function endOfMonth(date) {
    const result = new Date(date);
    // Aller au 1er du mois suivant, puis reculer d'un jour
    result.setMonth(result.getMonth() + 1, 0);
    result.setHours(23, 59, 59, 999);
    return result;
}

console.log(endOfMonth(today)); 
// 2024-11-30T23:59:59.999Z

// Alternative pour fin du mois
function endOfMonthAlt(date) {
    const result = new Date(date);
    const lastDay = new Date(
        result.getFullYear(),
        result.getMonth() + 1,
        0 // 0 = dernier jour du mois précédent
    ).getDate();
    
    result.setDate(lastDay);
    result.setHours(23, 59, 59, 999);
    return result;
}

// DÉBUT DE L'ANNÉE
function startOfYear(date) {
    const result = new Date(date);
    result.setMonth(0, 1); // 1er janvier
    result.setHours(0, 0, 0, 0);
    return result;
}

console.log(startOfYear(today)); 
// 2024-01-01T00:00:00.000Z

// FIN DE L'ANNÉE
function endOfYear(date) {
    const result = new Date(date);
    result.setMonth(11, 31); // 31 décembre
    result.setHours(23, 59, 59, 999);
    return result;
}

console.log(endOfYear(today)); 
// 2024-12-31T23:59:59.999Z

// DÉBUT DU JOUR
function startOfDay(date) {
    const result = new Date(date);
    result.setHours(0, 0, 0, 0);
    return result;
}

console.log(startOfDay(today)); 
// 2024-11-07T00:00:00.000Z

// FIN DU JOUR
function endOfDay(date) {
    const result = new Date(date);
    result.setHours(23, 59, 59, 999);
    return result;
}

console.log(endOfDay(today)); 
// 2024-11-07T23:59:59.999Z

// DÉBUT DE LA SEMAINE (lundi)
function startOfWeek(date) {
    const result = new Date(date);
    const day = result.getDay();
    // Calculer différence pour arriver à lundi (day 1)
    const diff = result.getDate() - day + (day === 0 ? -6 : 1);
    result.setDate(diff);
    result.setHours(0, 0, 0, 0);
    return result;
}

console.log(startOfWeek(today)); 
// Lundi de la semaine

// FIN DE LA SEMAINE (dimanche)
function endOfWeek(date) {
    const start = startOfWeek(date);
    const result = new Date(start);
    result.setDate(result.getDate() + 6);
    result.setHours(23, 59, 59, 999);
    return result;
}

console.log(endOfWeek(today)); 
// Dimanche de la semaine

// DÉBUT DU TRIMESTRE
function startOfQuarter(date) {
    const result = new Date(date);
    const quarter = Math.floor(result.getMonth() / 3);
    result.setMonth(quarter * 3, 1);
    result.setHours(0, 0, 0, 0);
    return result;
}

console.log(startOfQuarter(today)); 
// Q4: 01/10/2024

// FIN DU TRIMESTRE
function endOfQuarter(date) {
    const result = new Date(date);
    const quarter = Math.floor(result.getMonth() / 3);
    result.setMonth(quarter * 3 + 3, 0);
    result.setHours(23, 59, 59, 999);
    return result;
}

console.log(endOfQuarter(today)); 
// Q4: 31/12/2024

// EXEMPLES PRATIQUES

// Obtenir nombre de jours dans le mois
function getDaysInMonth(date) {
    const end = endOfMonth(date);
    return end.getDate();
}

console.log(getDaysInMonth(today)); // 30 (novembre)
console.log(getDaysInMonth(new Date('2024-02-01'))); // 29 (année bissextile)

// Obtenir tous les jours du mois
function getAllDaysInMonth(date) {
    const days = [];
    const start = startOfMonth(date);
    const end = endOfMonth(date);
    const current = new Date(start);
    
    while (current <= end) {
        days.push(new Date(current));
        current.setDate(current.getDate() + 1);
    }
    
    return days;
}

const novemberDays = getAllDaysInMonth(today);
console.log(`Novembre a ${novemberDays.length} jours`); // 30

// Vérifier si date est le premier jour du mois
function isFirstDayOfMonth(date) {
    return date.getDate() === 1;
}

console.log(isFirstDayOfMonth(new Date('2024-11-01'))); // true
console.log(isFirstDayOfMonth(today)); // false

// Vérifier si date est le dernier jour du mois
function isLastDayOfMonth(date) {
    const tomorrow = new Date(date);
    tomorrow.setDate(tomorrow.getDate() + 1);
    return tomorrow.getDate() === 1;
}

console.log(isLastDayOfMonth(new Date('2024-11-30'))); // true
console.log(isLastDayOfMonth(today)); // false

// Obtenir plage du mois pour requête
function getMonthRange(date) {
    return {
        start: startOfMonth(date),
        end: endOfMonth(date)
    };
}

const range = getMonthRange(today);
console.log('Du:', range.start.toLocaleDateString());
console.log('Au:', range.end.toLocaleDateString());

// Obtenir mois précédent/suivant
function getPreviousMonth(date) {
    const result = new Date(date);
    result.setMonth(result.getMonth() - 1);
    return {
        start: startOfMonth(result),
        end: endOfMonth(result)
    };
}

function getNextMonth(date) {
    const result = new Date(date);
    result.setMonth(result.getMonth() + 1);
    return {
        start: startOfMonth(result),
        end: endOfMonth(result)
    };
}

console.log('Mois précédent:', getPreviousMonth(today));
console.log('Mois suivant:', getNextMonth(today));

// Classe utilitaire complète
class DateRange {
    static startOf(date, unit) {
        const result = new Date(date);
        
        switch(unit) {
            case 'year':
                result.setMonth(0, 1);
                result.setHours(0, 0, 0, 0);
                break;
            case 'month':
                result.setDate(1);
                result.setHours(0, 0, 0, 0);
                break;
            case 'week':
                const day = result.getDay();
                const diff = result.getDate() - day + (day === 0 ? -6 : 1);
                result.setDate(diff);
                result.setHours(0, 0, 0, 0);
                break;
            case 'day':
                result.setHours(0, 0, 0, 0);
                break;
        }
        
        return result;
    }
    
    static endOf(date, unit) {
        const result = new Date(date);
        
        switch(unit) {
            case 'year':
                result.setMonth(11, 31);
                result.setHours(23, 59, 59, 999);
                break;
            case 'month':
                result.setMonth(result.getMonth() + 1, 0);
                result.setHours(23, 59, 59, 999);
                break;
            case 'week':
                const start = DateRange.startOf(date, 'week');
                result.setTime(start.getTime() + 7 * 24 * 60 * 60 * 1000 - 1);
                break;
            case 'day':
                result.setHours(23, 59, 59, 999);
                break;
        }
        
        return result;
    }
}

// Utilisation
console.log(DateRange.startOf(today, 'month'));
console.log(DateRange.endOf(today, 'year'));
console.log(DateRange.startOf(today, 'week'));
```

**Points clés:**
- `setDate(1)` = premier jour du mois
- `setMonth(month + 1, 0)` = dernier jour du mois
- `setHours(0, 0, 0, 0)` = début du jour
- `setHours(23, 59, 59, 999)` = fin du jour
- Toujours créer copie avant modification
- Utile pour requêtes de plage de dates

---

### Q8: Comment créer un countdown timer (compte à rebours) ?

**Réponse:**
```javascript
// Countdown simple vers une date cible

function createCountdown(targetDate) {
    const target = new Date(targetDate);
    
    function update() {
        const now = new Date();
        const diff = target - now;
        
        if (diff <= 0) {
            return {
                expired: true,
                message: 'Temps écoulé!'
            };
        }
        
        // Calculer jours, heures, minutes, secondes
        const days = Math.floor(diff / (1000 * 60 * 60 * 24));
        const hours = Math.floor((diff % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
        const minutes = Math.floor((diff % (1000 * 60 * 60)) / (1000 * 60));
        const seconds = Math.floor((diff % (1000 * 60)) / 1000);
        
        return {
            expired: false,
            days,
            hours,
            minutes,
            seconds,
            formatted: `${days}j ${hours}h ${minutes}m ${seconds}s`
        };
    }
    
    return update;
}

// Utilisation
const countdown = createCountdown('2024-12-31T23:59:59');

// Mettre à jour toutes les secondes
const intervalId = setInterval(() => {
    const time = countdown();
    
    if (time.expired) {
        console.log(time.message);
        clearInterval(intervalId);
    } else {
        console.log(time.formatted);
    }
}, 1000);

// Countdown avec callback
function startCountdown(targetDate, onTick, onComplete) {
    const target = new Date(targetDate);
    
    const intervalId = setInterval(() => {
        const now = new Date();
        const diff = target - now;
        
        if (diff <= 0) {
            clearInterval(intervalId);
            if (onComplete) onComplete();
            return;
        }
        
        const days = Math.floor(diff / (1000 * 60 * 60 * 24));
        const hours = Math.floor((diff % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
        const minutes = Math.floor((diff % (1000 * 60 * 60)) / (1000 * 60));
        const seconds = Math.floor((diff % (1000 * 60)) / 1000);
        
        if (onTick) {
            onTick({ days, hours, minutes, seconds });
        }
    }, 1000);
    
    return intervalId; // Retourner pour pouvoir arrêter
}

// Utilisation avec callbacks
const timerId = startCountdown(
    '2024-12-25T00:00:00',
    (time) => {
        console.log(`Noël dans: ${time.days}j ${time.hours}h ${time.minutes}m ${time.seconds}s`);
    },
    () => {
        console.log('Joyeux Noël! 🎄');
    }
);

// Arrêter le countdown
// clearInterval(timerId);

// Countdown avec formatage avancé
class Countdown {
    constructor(targetDate) {
        this.target = new Date(targetDate);
        this.intervalId = null;
        this.callbacks = [];
    }
    
    onTick(callback) {
        this.callbacks.push(callback);
        return this;
    }
    
    start() {
        this.intervalId = setInterval(() => {
            const remaining = this.getRemaining();
            
            if (remaining.expired) {
                this.stop();
            }
            
            this.callbacks.forEach(callback => callback(remaining));
        }, 1000);
        
        return this;
    }
    
    stop() {
        if (this.intervalId) {
            clearInterval(this.intervalId);
            this.intervalId = null;
        }
    }
    
    getRemaining() {
        const now = new Date();
        const diff = this.target - now;
        
        if (diff <= 0) {
            return { expired: true };
        }
        
        return {
            expired: false,
            total: diff,
            days: Math.floor(diff / (1000 * 60 * 60 * 24)),
            hours: Math.floor((diff % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60)),
            minutes: Math.floor((diff % (1000 * 60 * 60)) / (1000 * 60)),
            seconds: Math.floor((diff % (1000 * 60)) / 1000),
            milliseconds: diff % 1000
        };
    }
    
    format(template = '{d}j {h}h {m}m {s}s') {
        const remaining = this.getRemaining();
        
        if (remaining.expired) {
            return 'Expiré';
        }
        
        return template
            .replace('{d}', String(remaining.days).padStart(2, '0'))
            .replace('{h}', String(remaining.hours).padStart(2, '0'))
            .replace('{m}', String(remaining.minutes).padStart(2, '0'))
            .replace('{s}', String(remaining.seconds).padStart(2, '0'));
    }
}

// Utilisation de la classe
const newYearCountdown = new Countdown('2025-01-01T00:00:00');

newYearCountdown
    .onTick((time) => {
        if (!time.expired) {
            console.log(`Nouvel An dans: ${time.days}j ${time.hours}h ${time.minutes}m ${time.seconds}s`);
        } else {
            console.log('Bonne année! 🎉');
        }
    })
    .start();

// Arrêter après 10 secondes (pour l'exemple)
setTimeout(() => {
    newYearCountdown.stop();
    console.log('Countdown arrêté');
}, 10000);

// Countdown avec pourcentage de progression
function getProgress(startDate, endDate) {
    const start = new Date(startDate);
    const end = new Date(endDate);
    const now = new Date();
    
    const total = end - start;
    const elapsed = now - start;
    const percentage = Math.min(100, Math.max(0, (elapsed / total) * 100));
    
    return {
        percentage: percentage.toFixed(2),
        elapsed: elapsed,
        remaining: Math.max(0, total - elapsed)
    };
}

const campaignStart = new Date('2024-11-01');
const campaignEnd = new Date('2024-11-30');
console.log(getProgress(campaignStart, campaignEnd));
// { percentage: "23.33", elapsed: ..., remaining: ... }

// Countdown pour React (exemple de hook)
/*
function useCountdown(targetDate) {
    const [timeLeft, setTimeLeft] = useState(null);
    
    useEffect(() => {
        const calculateTimeLeft = () => {
            const diff = new Date(targetDate) - new Date();
            
            if (diff <= 0) {
                return null;
            }
            
            return {
                days: Math.floor(diff / (1000 * 60 * 60 * 24)),
                hours: Math.floor((diff % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60)),
                minutes: Math.floor((diff % (1000 * 60 * 60)) / (1000 * 60)),
                seconds: Math.floor((diff % (1000 * 60)) / 1000)
            };
        };
        
        setTimeLeft(calculateTimeLeft());
        
        const timer = setInterval(() => {
            setTimeLeft(calculateTimeLeft());
        }, 1000);
        
        return () => clearInterval(timer);
    }, [targetDate]);
    
    return timeLeft;
}

// Utilisation dans composant
function CountdownTimer() {
    const timeLeft = useCountdown('2024-12-31T23:59:59');
    
    if (!timeLeft) {
        return <div>Temps écoulé!</div>;
    }
    
    return (
        <div>
            {timeLeft.days}j {timeLeft.hours}h {timeLeft.minutes}m {timeLeft.seconds}s
        </div>
    );
}
*/
```

**Points clés:**
- Calculer différence entre date cible et maintenant
- Mettre à jour avec `setInterval()` chaque seconde
- Diviser millisecondes pour obtenir jours/heures/minutes
- Toujours nettoyer interval avec `clearInterval()`
- En React: utiliser `useEffect` pour cleanup
- Arrêter countdown quand expiré

---

### Q9: Comment parser une date depuis un format personnalisé (ex: "07/11/2024") ?

**Réponse:**
```javascript
// Parser date depuis format DD/MM/YYYY
function parseDate(dateStr, format = 'DD/MM/YYYY') {
    const parts = dateStr.split(/[-\/\s]/);
    
    if (format === 'DD/MM/YYYY' || format === 'DD-MM-YYYY') {
        const day = parseInt(parts[0]);
        const month = parseInt(parts[1]) - 1; // Mois commence à 0
        const year = parseInt(parts[2]);
        return new Date(year, month, day);
    }
    
    if (format === 'MM/DD/YYYY' || format === 'MM-DD-YYYY') {
        const month = parseInt(parts[0]) - 1;
        const day = parseInt(parts[1]);
        const year = parseInt(parts[2]);
        return new Date(year, month, day);
    }
    
    if (format === 'YYYY-MM-DD' || format === 'YYYY/MM/DD') {
        const year = parseInt(parts[0]);
        const month = parseInt(parts[1]) - 1;
        const day = parseInt(parts[2]);
        return new Date(year, month, day);
    }
    
    return null;
}

// Tests
console.log(parseDate('07/11/2024', 'DD/MM/YYYY')); // 7 novembre 2024
console.log(parseDate('11/07/2024', 'MM/DD/YYYY')); // 7 novembre 2024
console.log(parseDate('2024-11-07', 'YYYY-MM-DD')); // 7 novembre 2024

// Parser avec regex
function parseDateWithRegex(dateStr) {
    // Format DD/MM/YYYY ou DD-MM-YYYY
    const regex1 = /^(\d{2})[\/\-](\d{2})[\/\-](\d{4})$/;
    const match1 = dateStr.match(regex1);
    
    if (match1) {
        const [, day, month, year] = match1;
        return new Date(year, month - 1, day);
    }
    
    // Format YYYY-MM-DD
    const regex2 = /^(\d{4})[\/\-](\d{2})[\/\-](\d{2})$/;
    const match2 = dateStr.match(regex2);
    
    if (match2) {
        const [, year, month, day] = match2;
        return new Date(year, month - 1, day);
    }
    
    return null;
}

console.log(parseDateWithRegex('07/11/2024'));
console.log(parseDateWithRegex('2024-11-07'));

// Parser avec validation
function parseDateSafe(dateStr, format) {
    try {
        const date = parseDate(dateStr, format);
        
        if (!date || isNaN(date.getTime())) {
            throw new Error('Date invalide');
        }
        
        // Vérifier que les valeurs sont cohérentes
        const parts = dateStr.split(/[-\/\s]/);
        
        if (format === 'DD/MM/YYYY') {
            const day = parseInt(parts[0]);
            const month = parseInt(parts[1]);
            
            if (day !== date.getDate() || month !== date.getMonth() + 1) {
                throw new Error('Date incohérente');
            }
        }
        
        return { success: true, date };
    } catch (error) {
        return { success: false, error: error.message };
    }
}

console.log(parseDateSafe('07/11/2024', 'DD/MM/YYYY')); 
// { success: true, date: ... }

console.log(parseDateSafe('32/13/2024', 'DD/MM/YYYY')); 
// { success: false, error: 'Date incohérente' }

// Parser date avec heure
function parseDateTime(dateTimeStr, format = 'DD/MM/YYYY HH:mm') {
    const [datePart, timePart] = dateTimeStr.split(' ');
    
    // Parser date
    const date = parseDate(datePart, format.split(' ')[0]);
    
    if (!date || !timePart) {
        return null;
    }
    
    // Parser heure
    const [hours, minutes, seconds = 0] = timePart.split(':').map(Number);
    date.setHours(hours, minutes, seconds);
    
    return date;
}

console.log(parseDateTime('07/11/2024 15:30', 'DD/MM/YYYY HH:mm'));
console.log(parseDateTime('07/11/2024 15:30:45', 'DD/MM/YYYY HH:mm:ss'));

// Parser date relative ("aujourd'hui", "demain", "dans 3 jours")
function parseRelativeDate(str) {
    const now = new Date();
    str = str.toLowerCase().trim();
    
    if (str === 'aujourd\'hui' || str === 'today') {
        return now;
    }
    
    if (str === 'hier' || str === 'yesterday') {
        return new Date(now.getTime() - 86400000);
    }
    
    if (str === 'demain' || str === 'tomorrow') {
        return new Date(now.getTime() + 86400000);
    }
    
    // "dans X jours/semaines/mois"
    const inMatch = str.match(/dans (\d+) (jour|semaine|mois|an)s?/);
    if (inMatch) {
        const amount = parseInt(inMatch[1]);
        const unit = inMatch[2];
        const result = new Date(now);
        
        switch(unit) {
            case 'jour':
                result.setDate(result.getDate() + amount);
                break;
            case 'semaine':
                result.setDate(result.getDate() + amount * 7);
                break;
            case 'mois':
                result.setMonth(result.getMonth() + amount);
                break;
            case 'an':
                result.setFullYear(result.getFullYear() + amount);
                break;
        }
        
        return result;
    }
    
    // "il y a X jours/semaines/mois"
    const agoMatch = str.match(/il y a (\d+) (jour|semaine|mois|an)s?/);
    if (agoMatch) {
        const amount = parseInt(agoMatch[1]);
        const unit = agoMatch[2];
        const result = new Date(now);
        
        switch(unit) {
            case 'jour':
                result.setDate(result.getDate() - amount);
                break;
            case 'semaine':
                result.setDate(result.getDate() - amount * 7);
                break;
            case 'mois':
                result.setMonth(result.getMonth() - amount);
                break;
            case 'an':
                result.setFullYear(result.getFullYear() - amount);
                break;
        }
        
        return result;
    }
    
    return null;
}

console.log(parseRelativeDate('aujourd\'hui'));
console.log(parseRelativeDate('dans 5 jours'));
console.log(parseRelativeDate('il y a 2 semaines'));

// Parser format flexible
function parseFlexibleDate(input) {
    // Essayer format ISO
    let date = new Date(input);
    if (!isNaN(date.getTime())) {
        return date;
    }
    
    // Essayer formats communs
    const formats = [
        { pattern: 'DD/MM/YYYY', regex: /^(\d{2})\/(\d{2})\/(\d{4})$/ },
        { pattern: 'DD-MM-YYYY', regex: /^(\d{2})-(\d{2})-(\d{4})$/ },
        { pattern: 'YYYY-MM-DD', regex: /^(\d{4})-(\d{2})-(\d{2})$/ },
        { pattern: 'MM/DD/YYYY', regex: /^(\d{2})\/(\d{2})\/(\d{4})$/ }
    ];
    
    for (const format of formats) {
        const match = input.match(format.regex);
        if (match) {
            date = parseDate(input, format.pattern);
            if (date && !isNaN(date.getTime())) {
                return date;
            }
        }
    }
    
    // Essayer date relative
    date = parseRelativeDate(input);
    if (date) {
        return date;
    }
    
    return null;
}

console.log(parseFlexibleDate('2024-11-07'));
console.log(parseFlexibleDate('07/11/2024'));
console.log(parseFlexibleDate('demain'));
```

**Points clés:**
- `split()` pour séparer composants
- Attention: mois commence à 0
- Valider résultat avec `isNaN(date)`
- Regex pour patterns complexes
- Gérer plusieurs formats
- Parser dates relatives pour UX

---

### Q10: Comment gérer les fuseaux horaires (timezones) ?

**Réponse:**
```javascript
// Obtenir offset du timezone local (en minutes)
const now = new Date();
const offsetMinutes = now.getTimezoneOffset();
console.log(`Offset: ${offsetMinutes} minutes`); // Ex: -60 pour UTC+1

// Convertir en heures
const offsetHours = -offsetMinutes / 60;
console.log(`Timezone: UTC${offsetHours >= 0 ? '+' : ''}${offsetHours}`);
// Ex: "Timezone: UTC+1"

// Obtenir heure UTC
const utcHours = now.getUTCHours();
const localHours = now.getHours();
console.log('Local:', localHours);
console.log('UTC:', utcHours);

// Toutes les méthodes UTC
console.log('UTC FullYear:', now.getUTCFullYear());
console.log('UTC Month:', now.getUTCMonth());
console.log('UTC Date:', now.getUTCDate());
console.log('UTC Day:', now.getUTCDay());
console.log('UTC Hours:', now.getUTCHours());
console.log('UTC Minutes:', now.getUTCMinutes());
console.log('UTC Seconds:', now.getUTCSeconds());

// Formater en UTC
const utcString = now.toUTCString();
console.log(utcString); // "Thu, 07 Nov 2024 14:30:00 GMT"

const isoString = now.toISOString();
console.log(isoString); // "2024-11-07T14:30:00.000Z"

// Utiliser Intl pour timezone spécifique
function formatInTimezone(date, timezone) {
    return date.toLocaleString('fr-FR', {
        timeZone: timezone,
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit'
    });
}

const date = new Date();
console.log('Paris:', formatInTimezone(date, 'Europe/Paris'));
console.log('New York:', formatInTimezone(date, 'America/New_York'));
console.log('Tokyo:', formatInTimezone(date, 'Asia/Tokyo'));
console.log('Los Angeles:', formatInTimezone(date, 'America/Los_Angeles'));

// Obtenir heure dans timezone spécifique
function getTimeInTimezone(date, timezone) {
    const options = {
        timeZone: timezone,
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
        hour12: false
    };
    
    return date.toLocaleTimeString('en-US', options);
}

console.log('Paris:', getTimeInTimezone(date, 'Europe/Paris'));
console.log('NYC:', getTimeInTimezone(date, 'America/New_York'));

// Obtenir nom du timezone
function getTimezoneName(timezone) {
    const formatter = new Intl.DateTimeFormat('en-US', {
        timeZone: timezone,
        timeZoneName: 'long'
    });
    
    const parts = formatter.formatToParts(new Date());
    const timezonePart = parts.find(part => part.type === 'timeZoneName');
    
    return timezonePart ? timezonePart.value : timezone;
}

console.log(getTimezoneName('Europe/Paris')); 
// "Central European Standard Time"

// Convertir entre timezones
function convertTimezone(date, fromTz, toTz) {
    // Obtenir string de la date dans timezone source
    const dateInFrom = new Date(date.toLocaleString('en-US', { timeZone: fromTz }));
    
    // Créer date dans timezone cible
    const dateInTo = new Date(date.toLocaleString('en-US', { timeZone: toTz }));
    
    return {
        from: formatInTimezone(date, fromTz),
        to: formatInTimezone(date, toTz)
    };
}

console.log(convertTimezone(new Date(), 'Europe/Paris', 'America/New_York'));

// Afficher horloge mondiale
function getWorldClocks() {
    const now = new Date();
    const timezones = {
        'Paris': 'Europe/Paris',
        'Londres': 'Europe/London',
        'New York': 'America/New_York',
        'Los Angeles': 'America/Los_Angeles',
        'Tokyo': 'Asia/Tokyo',
        'Sydney': 'Australia/Sydney',
        'Dubai': 'Asia/Dubai',
        'Hong Kong': 'Asia/Hong_Kong'
    };
    
    const clocks = {};
    for (const [city, timezone] of Object.entries(timezones)) {
        clocks[city] = {
            time: getTimeInTimezone(now, timezone),
            date: formatInTimezone(now, timezone),
            timezone: timezone
        };
    }
    
    return clocks;
}

console.log(getWorldClocks());
// {
//   Paris: { time: '15:30:00', date: '07/11/2024, 15:30:00', ... },
//   'New York': { time: '09:30:00', date: '07/11/2024, 09:30:00', ... },
//   ...
// }

// Calculer différence horaire entre deux timezones
function getTimezoneOffset(timezone1, timezone2) {
    const now = new Date();
    
    const date1 = new Date(now.toLocaleString('en-US', { timeZone: timezone1 }));
    const date2 = new Date(now.toLocaleString('en-US', { timeZone: timezone2 }));
    
    const diffMs = date2 - date1;
    const diffHours = diffMs / (1000 * 60 * 60);
    
    return diffHours;
}

const offset = getTimezoneOffset('Europe/Paris', 'America/New_York');
console.log(`Différence: ${offset} heures`); // -6 heures

// Créer date dans timezone spécifique
function createDateInTimezone(year, month, day, hours, minutes, timezone) {
    // Créer string de date
    const dateStr = `${year}-${String(month).padStart(2, '0')}-${String(day).padStart(2, '0')}T${String(hours).padStart(2, '0')}:${String(minutes).padStart(2, '0')}:00`;
    
    // Parser dans timezone
    const formatter = new Intl.DateTimeFormat('en-US', {
        timeZone: timezone,
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
        hour12: false
    });
    
    return new Date(dateStr + ' ' + timezone);
}

// Vérifier si DST (Daylight Saving Time) est actif
function isDST(date, timezone) {
    const jan = new Date(date.getFullYear(), 0, 1);
    const jul = new Date(date.getFullYear(), 6, 1);
    
    const janOffset = parseInt(new Date(jan.toLocaleString('en-US', { 
        timeZone: timezone 
    })).getTimezoneOffset());
    
    const julOffset = parseInt(new Date(jul.toLocaleString('en-US', { 
        timeZone: timezone 
    })).getTimezoneOffset());
    
    const currentOffset = parseInt(new Date(date.toLocaleString('en-US', { 
        timeZone: timezone 
    })).getTimezoneOffset());
    
    return currentOffset !== Math.max(janOffset, julOffset);
}

console.log('DST Paris:', isDST(new Date(), 'Europe/Paris'));

// Planifier meeting dans plusieurs timezones
function scheduleMeeting(dateTime, timezone, participants) {
    const baseDate = new Date(dateTime);
    const schedule = {};
    
    for (const [name, tz] of Object.entries(participants)) {
        schedule[name] = {
            timezone: tz,
            localTime: formatInTimezone(baseDate, tz)
        };
    }
    
    return schedule;
}

const meeting = scheduleMeeting(
    '2024-11-10T15:00:00',
    'Europe/Paris',
    {
        'Alice (Paris)': 'Europe/Paris',
        'Bob (NYC)': 'America/New_York',
        'Charlie (Tokyo)': 'Asia/Tokyo'
    }
);

console.log(meeting);
// {
//   'Alice (Paris)': { timezone: 'Europe/Paris', localTime: '10/11/2024, 15:00:00' },
//   'Bob (NYC)': { timezone: 'America/New_York', localTime: '10/11/2024, 09:00:00' },
//   'Charlie (Tokyo)': { timezone: 'Asia/Tokyo', localTime: '10/11/2024, 23:00:00' }
// }

// Sauvegarder date en UTC pour base de données
function saveDateUTC(date) {
    return {
        iso: date.toISOString(), // Format ISO UTC
        timestamp: date.getTime() // Timestamp (toujours UTC)
    };
}

const savedDate = saveDateUTC(new Date());
console.log(savedDate);
// { iso: '2024-11-07T14:30:00.000Z', timestamp: 1699368600000 }

// Récupérer et afficher dans timezone locale
function loadDateLocal(isoString) {
    const date = new Date(isoString);
    return date.toLocaleString(); // Affiche dans timezone locale
}

console.log(loadDateLocal(savedDate.iso));

// Classe utilitaire pour timezones
class TimezoneHelper {
    static format(date, timezone, options = {}) {
        const defaultOptions = {
            timeZone: timezone,
            year: 'numeric',
            month: '2-digit',
            day: '2-digit',
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit',
            ...options
        };
        
        return date.toLocaleString('fr-FR', defaultOptions);
    }
    
    static getOffset(timezone) {
        const date = new Date();
        const utcDate = new Date(date.toLocaleString('en-US', { timeZone: 'UTC' }));
        const tzDate = new Date(date.toLocaleString('en-US', { timeZone: timezone }));
        
        return (tzDate - utcDate) / (1000 * 60 * 60);
    }
    
    static convert(date, fromTz, toTz) {
        return {
            from: this.format(date, fromTz),
            to: this.format(date, toTz)
        };
    }
    
    static getCommonTimezones() {
        return {
            'Europe/Paris': 'Paris (CET/CEST)',
            'Europe/London': 'Londres (GMT/BST)',
            'America/New_York': 'New York (EST/EDT)',
            'America/Los_Angeles': 'Los Angeles (PST/PDT)',
            'America/Chicago': 'Chicago (CST/CDT)',
            'Asia/Tokyo': 'Tokyo (JST)',
            'Asia/Shanghai': 'Shanghai (CST)',
            'Asia/Dubai': 'Dubai (GST)',
            'Australia/Sydney': 'Sydney (AEDT/AEST)',
            'Pacific/Auckland': 'Auckland (NZDT/NZST)'
        };
    }
}

// Utilisation
console.log(TimezoneHelper.format(new Date(), 'Europe/Paris'));
console.log(TimezoneHelper.getOffset('America/New_York'));
console.log(TimezoneHelper.convert(new Date(), 'Europe/Paris', 'Asia/Tokyo'));

// BONNES PRATIQUES:
// 1. Toujours sauvegarder en UTC dans la base de données
// 2. Convertir en timezone locale uniquement pour l'affichage
// 3. Utiliser ISO 8601 format pour échanger des dates
// 4. Ne jamais faire confiance au timezone du client
// 5. Utiliser Intl.DateTimeFormat pour formatage localisé
```

**Points clés:**
- `getTimezoneOffset()` retourne offset en minutes
- Méthodes `getUTC*()` pour heure universelle
- `toISOString()` pour format UTC standard
- `Intl.DateTimeFormat` avec `timeZone` option
- Toujours sauvegarder en UTC en base de données
- Afficher dans timezone locale utilisateur

---

## Résumé Final

1. **Création de dates:**
   - `new Date()` - maintenant
   - Mois commence à 0 (janvier = 0)
   - Timestamp = millisecondes depuis 1970

2. **Get/Set methods:**
   - `getMonth()` (0-11), `getDate()` (1-31), `getDay()` (0-6)
   - Toujours créer copie avant modification

3. **Formatage:**
   - `toLocaleDateString()` / `toLocaleTimeString()`
   - `padStart()` pour zéros devant
   - Intl pour options avancées

4. **Calculs:**
   - Soustraire dates = millisecondes
   - Diviser pour convertir unités
   - `setDate(getDate() + X)` pour ajouter jours

5. **Comparaison:**
   - Utiliser `<`, `>` directement
   - `getTime()` pour égalité exacte
   - `toDateString()` pour même jour

6. **React:**
   - State pour dates dynamiques
   - `useEffect` pour timers/countdowns
   - Cleanup avec `clearInterval()`
   - Librairies: date-fns, dayjs, moment

7. **Timezones:**
   - Sauvegarder en UTC
   - Afficher en timezone locale
   - `toISOString()` pour API

**Piège commun:** Mois commence à 0!
