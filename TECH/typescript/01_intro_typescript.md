# Chapitre 1 : Introduction de TypeScript dans une Application

## 📑 Table des matières
- [1.1 Installation et Configuration](#11-installation-et-configuration)
- [1.2 Migration et Intégration](#12-migration-et-intégration)
- [1.3 Exercice Pratique](#13-exercice-pratique)
- [Points Clés pour l'Entretien](#-points-clés-pour-lentretien)

---

## 1.1 Installation et Configuration

### 🎯 Pourquoi TypeScript ?

TypeScript est un **superset de JavaScript** développé par Microsoft qui ajoute :
- ✅ **Typage statique** : Détection des erreurs à la compilation
- ✅ **Autocomplétion intelligente** : Meilleure expérience développeur
- ✅ **Refactoring sûr** : Renommage et modifications sans risque
- ✅ **Documentation automatique** : Les types servent de documentation
- ✅ **Compatible JavaScript** : Tout code JS est valide en TS

### 📦 Installation de TypeScript

#### Installation globale (pour CLI)
```bash
# Installation globale
npm install -g typescript

# Vérifier la version installée
tsc --version
# Résultat : Version 5.x.x

# Compiler un fichier TypeScript
tsc monFichier.ts
```

#### Installation locale (recommandée pour les projets)
```bash
# Dans votre projet
npm install --save-dev typescript

# Vérifier l'installation
npx tsc --version

# Ajouter un script dans package.json
```

**package.json** :
```json
{
  "name": "mon-projet",
  "version": "1.0.0",
  "scripts": {
    "build": "tsc",
    "watch": "tsc --watch",
    "dev": "tsc && node dist/index.js"
  },
  "devDependencies": {
    "typescript": "^5.3.0"
  }
}
```

### ⚙️ Configuration avec tsconfig.json

Le fichier `tsconfig.json` est le **cœur de la configuration TypeScript**.

#### Génération automatique
```bash
# Créer un tsconfig.json avec valeurs par défaut
npx tsc --init
```

#### Configuration de base expliquée
```json
{
  "compilerOptions": {
    /* Configuration de base */
    "target": "ES2020",              // Version JS cible
    "module": "commonjs",            // Système de modules
    "lib": ["ES2020", "DOM"],        // Bibliothèques disponibles
    
    /* Chemins et sortie */
    "outDir": "./dist",              // Dossier de sortie
    "rootDir": "./src",              // Dossier source
    
    /* Vérifications strictes */
    "strict": true,                  // Active TOUTES les vérifications strictes
    "noImplicitAny": true,           // Interdit 'any' implicite
    "strictNullChecks": true,        // Vérification null/undefined
    "strictFunctionTypes": true,     // Vérification stricte des fonctions
    "strictPropertyInitialization": true,  // Propriétés initialisées
    
    /* Modules */
    "esModuleInterop": true,         // Compatibilité imports
    "moduleResolution": "node",      // Résolution comme Node.js
    "resolveJsonModule": true,       // Import de fichiers JSON
    
    /* Autres options utiles */
    "skipLibCheck": true,            // Skip vérification node_modules
    "forceConsistentCasingInFileNames": true,  // Sensibilité casse
    "sourceMap": true,               // Génère .map pour debug
    "declaration": true,             // Génère fichiers .d.ts
    "removeComments": false          // Garde les commentaires
  },
  "include": ["src/**/*"],           // Fichiers à inclure
  "exclude": ["node_modules", "dist", "**/*.spec.ts"]  // Fichiers à exclure
}
```

#### Configuration pour différents cas d'usage

**Pour un projet Node.js** :
```json
{
  "compilerOptions": {
    "target": "ES2020",
    "module": "commonjs",
    "lib": ["ES2020"],
    "outDir": "./dist",
    "rootDir": "./src",
    "strict": true,
    "esModuleInterop": true,
    "skipLibCheck": true,
    "types": ["node"]
  }
}
```

**Pour un projet Angular** :
```json
{
  "compilerOptions": {
    "target": "ES2022",
    "module": "ES2022",
    "lib": ["ES2022", "DOM"],
    "moduleResolution": "node",
    "experimentalDecorators": true,    // Important pour Angular
    "emitDecoratorMetadata": true,     // Important pour Angular
    "strict": true,
    "strictPropertyInitialization": false,  // Souvent false pour Angular
    "baseUrl": "./",
    "paths": {
      "@app/*": ["src/app/*"],
      "@shared/*": ["src/app/shared/*"]
    }
  }
}
```

**Pour un projet React** :
```json
{
  "compilerOptions": {
    "target": "ES2020",
    "module": "ESNext",
    "lib": ["ES2020", "DOM", "DOM.Iterable"],
    "jsx": "react-jsx",               // Support JSX
    "strict": true,
    "moduleResolution": "bundler",
    "esModuleInterop": true
  }
}
```

### 🔄 Ajout de TypeScript à une solution existante

#### Stratégie de migration progressive

**Phase 1 : Préparation (sans casser le code existant)**

1. **Installer TypeScript**
```bash
npm install --save-dev typescript @types/node
```

2. **Créer tsconfig.json permissif**
```json
{
  "compilerOptions": {
    "target": "ES2020",
    "module": "commonjs",
    "outDir": "./dist",
    "rootDir": "./src",
    
    /* Mode permissif au début */
    "allowJs": true,              // ✅ Accepte les .js
    "checkJs": false,             // ❌ Ne vérifie PAS les .js encore
    "noEmit": true,               // Ne génère pas de fichiers
    "strict": false,              // Mode non-strict
    "skipLibCheck": true
  },
  "include": ["src/**/*"],
  "exclude": ["node_modules"]
}
```

3. **Tester la compilation**
```bash
npx tsc --noEmit
# Doit compiler sans erreur
```

**Phase 2 : Migration fichier par fichier**

Exemple de migration d'un module :

```javascript
// AVANT : user.service.js
class UserService {
  constructor(apiUrl) {
    this.apiUrl = apiUrl;
    this.users = [];
  }
  
  async fetchUsers() {
    const response = await fetch(this.apiUrl);
    this.users = await response.json();
    return this.users;
  }
  
  getUserById(id) {
    return this.users.find(user => user.id === id);
  }
  
  addUser(user) {
    this.users.push(user);
  }
}

module.exports = UserService;
```

```typescript
// APRÈS : user.service.ts

// 1. Définir les interfaces
interface User {
  id: number;
  name: string;
  email: string;
  role: 'admin' | 'user';
}

// 2. Typer la classe
class UserService {
  private apiUrl: string;
  private users: User[];
  
  constructor(apiUrl: string) {
    this.apiUrl = apiUrl;
    this.users = [];
  }
  
  async fetchUsers(): Promise<User[]> {
    try {
      const response = await fetch(this.apiUrl);
      
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      
      this.users = await response.json();
      return this.users;
    } catch (error) {
      console.error('Error fetching users:', error);
      throw error;
    }
  }
  
  getUserById(id: number): User | undefined {
    return this.users.find(user => user.id === id);
  }
  
  addUser(user: User): void {
    // Validation
    if (!user.id || !user.name || !user.email) {
      throw new Error('User must have id, name and email');
    }
    this.users.push(user);
  }
  
  getUsersByRole(role: User['role']): User[] {
    return this.users.filter(user => user.role === role);
  }
}

export default UserService;
```

**Phase 3 : Durcir la configuration progressivement**

```json
{
  "compilerOptions": {
    "allowJs": true,
    "checkJs": true,           // ✅ Activer la vérification JS
    "strict": true,            // ✅ Activer le mode strict
    "noEmit": false            // ✅ Générer les fichiers
  }
}
```

### 📊 Exemple complet de migration d'un projet Express

**Structure du projet** :
```
mon-projet/
├── src/
│   ├── index.js          → index.ts
│   ├── routes/
│   │   └── users.js      → users.ts
│   ├── models/
│   │   └── user.js       → user.ts
│   └── utils/
│       └── validation.js → validation.ts
├── package.json
└── tsconfig.json
```

**Avant (index.js)** :
```javascript
const express = require('express');
const userRoutes = require('./routes/users');

const app = express();
const PORT = 3000;

app.use(express.json());
app.use('/api/users', userRoutes);

app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});
```

**Après (index.ts)** :
```typescript
import express, { Application } from 'express';
import userRoutes from './routes/users';

const app: Application = express();
const PORT: number = process.env.PORT ? parseInt(process.env.PORT) : 3000;

// Middleware
app.use(express.json());

// Routes
app.use('/api/users', userRoutes);

// Démarrage du serveur
app.listen(PORT, () => {
  console.log(`✅ Server running on port ${PORT}`);
});

export default app;
```

**Installation des types nécessaires** :
```bash
npm install --save-dev @types/express @types/node
```

---

## 1.2 Migration et Intégration

### 🔍 Ajout de type checking aux fichiers JavaScript

TypeScript peut vérifier les fichiers JavaScript **sans les convertir** en `.ts`. C'est idéal pour une migration progressive.

#### Méthode 1 : Commentaire par fichier

```javascript
// @ts-check

function calculateDiscount(price, discountPercent) {
  return price - (price * discountPercent / 100);
}

// ✅ Fonctionne
calculateDiscount(100, 10);  // 90

// ❌ TypeScript détecte l'erreur
calculateDiscount("100", "10");  // Error: Type 'string' is not assignable to type 'number'
```

#### Méthode 2 : Configuration globale

**tsconfig.json** :
```json
{
  "compilerOptions": {
    "allowJs": true,
    "checkJs": true,          // Active pour TOUS les .js
    "noEmit": true,           // Ne compile pas, juste vérifie
    "strict": false           // Mode permissif
  }
}
```

#### Méthode 3 : JSDoc pour typage avancé

JSDoc permet d'ajouter des types aux fichiers JavaScript sans les convertir en TypeScript.

**Typage de fonctions** :
```javascript
/**
 * Calcule le prix total avec remise
 * @param {number} price - Le prix initial
 * @param {number} discount - Le pourcentage de remise (0-100)
 * @returns {number} Le prix après remise
 */
function calculateTotal(price, discount) {
  if (discount < 0 || discount > 100) {
    throw new Error('Discount must be between 0 and 100');
  }
  return price - (price * discount / 100);
}

// TypeScript connaît maintenant les types !
calculateTotal(100, 20);      // ✅ OK
calculateTotal("100", 20);    // ❌ Erreur détectée
```

**Typage d'objets avec @typedef** :
```javascript
/**
 * @typedef {Object} Product
 * @property {string} id - L'identifiant unique
 * @property {string} name - Le nom du produit
 * @property {number} price - Le prix en euros
 * @property {string[]} tags - Les tags du produit
 * @property {boolean} inStock - Disponibilité
 */

/**
 * Filtre les produits en stock
 * @param {Product[]} products - Liste des produits
 * @returns {Product[]} Produits disponibles
 */
function getAvailableProducts(products) {
  return products.filter(p => p.inStock);
}

/** @type {Product} */
const laptop = {
  id: "laptop-001",
  name: "MacBook Pro",
  price: 2500,
  tags: ["computer", "apple"],
  inStock: true
};
```

**Typage complexe avec interfaces** :
```javascript
/**
 * @typedef {Object} Address
 * @property {string} street
 * @property {string} city
 * @property {string} zipCode
 * @property {string} country
 */

/**
 * @typedef {Object} User
 * @property {number} id
 * @property {string} name
 * @property {string} email
 * @property {Address} address
 * @property {'admin' | 'user' | 'guest'} role - Le rôle de l'utilisateur
 */

/**
 * Crée un nouvel utilisateur
 * @param {string} name
 * @param {string} email
 * @param {Address} address
 * @returns {User}
 */
function createUser(name, email, address) {
  return {
    id: Date.now(),
    name,
    email,
    address,
    role: 'user'
  };
}
```

**Exemple Angular avec JSDoc** :
```javascript
// @ts-check

/**
 * @typedef {import('@angular/core').Component} Component
 */

/**
 * @type {Component}
 */
export const UserListComponent = {
  selector: 'app-user-list',
  templateUrl: './user-list.component.html',
  styleUrls: ['./user-list.component.css']
};
```

### 📚 Importation des types tiers (third-party types)

La communauté TypeScript maintient **DefinitelyTyped** : un dépôt avec des types pour +8000 bibliothèques JavaScript.

#### Installation des types

```bash
# Format général
npm install --save-dev @types/nom-de-la-bibliotheque

# Exemples courants
npm install --save-dev @types/node        # Pour Node.js
npm install --save-dev @types/express     # Pour Express
npm install --save-dev @types/lodash      # Pour Lodash
npm install --save-dev @types/jest        # Pour Jest
npm install --save-dev @types/jquery      # Pour jQuery
```

#### Exemple avec Lodash

**Sans types** :
```typescript
import _ from 'lodash';

// ❌ Pas d'autocomplétion
// ❌ Type 'any' pour result
const result = _.chunk(['a', 'b', 'c', 'd'], 2);
```

**Avec types** :
```bash
npm install lodash
npm install --save-dev @types/lodash
```

```typescript
import _ from 'lodash';

// ✅ Autocomplétion complète
// ✅ Type correct : string[][]
const result = _.chunk(['a', 'b', 'c', 'd'], 2);
console.log(result);  // [['a', 'b'], ['c', 'd']]

// ✅ TypeScript détecte les erreurs
const numbers = _.chunk([1, 2, 3, 4], "2");  // ❌ Erreur : "2" n'est pas un nombre
```

#### Exemple avec Express

```typescript
import express, { Request, Response, NextFunction } from 'express';

const app = express();

// ✅ Types pour req, res, next
app.get('/users/:id', (req: Request, res: Response) => {
  const userId = req.params.id;  // ✅ Type string
  
  // Autocomplétion sur res
  res.json({
    id: userId,
    name: 'John Doe'
  });
});

// Middleware typé
app.use((err: Error, req: Request, res: Response, next: NextFunction) => {
  console.error(err.stack);
  res.status(500).send('Something broke!');
});
```

#### Vérifier si des types existent

```bash
# Rechercher dans DefinitelyTyped
npm search @types/moment
npm search @types/axios

# Ou visiter : https://www.typescriptlang.org/dt/search
```

#### Créer vos propres déclarations

Si une bibliothèque n'a pas de types, créez un fichier `.d.ts` :

**Structure du projet** :
```
src/
├── types/
│   └── ma-bibliotheque.d.ts
└── index.ts
```

**ma-bibliotheque.d.ts** :
```typescript
// Déclaration de module
declare module 'ma-bibliotheque-sans-types' {
  // Export de fonctions
  export function connect(url: string): Promise<Connection>;
  export function disconnect(): void;
  
  // Export d'interfaces
  export interface Connection {
    isConnected: boolean;
    query(sql: string): Promise<any[]>;
    close(): void;
  }
  
  // Export de types
  export type QueryResult = {
    rows: any[];
    rowCount: number;
  };
  
  // Export de classes
  export class Database {
    constructor(config: DatabaseConfig);
    connect(): Promise<void>;
    query(sql: string): Promise<QueryResult>;
  }
  
  export interface DatabaseConfig {
    host: string;
    port: number;
    username: string;
    password: string;
  }
}
```

**Utilisation** :
```typescript
import { Database, DatabaseConfig } from 'ma-bibliotheque-sans-types';

const config: DatabaseConfig = {
  host: 'localhost',
  port: 5432,
  username: 'admin',
  password: 'secret'
};

const db = new Database(config);
// ✅ Autocomplétion disponible maintenant !
```

#### Types pour bibliothèques globales

```typescript
// types/globals.d.ts

// Variable globale
declare var MY_GLOBAL_VAR: string;

// Fonction globale
declare function myGlobalFunction(param: number): string;

// Namespace global
declare namespace MyLibrary {
  function init(config: Config): void;
  
  interface Config {
    apiKey: string;
    debug: boolean;
  }
}
```

**tsconfig.json** :
```json
{
  "compilerOptions": {
    "typeRoots": ["./node_modules/@types", "./src/types"]
  }
}
```

---

## 1.3 Exercice Pratique : JavaScript vers TypeScript

### 🎯 Challenge : Système de gestion de bibliothèque

**Code JavaScript de départ** :
```javascript
// library.js

class Book {
  constructor(title, author, isbn, publicationYear) {
    this.title = title;
    this.author = author;
    this.isbn = isbn;
    this.publicationYear = publicationYear;
    this.isAvailable = true;
  }
}

class Member {
  constructor(id, name, email) {
    this.id = id;
    this.name = name;
    this.email = email;
    this.borrowedBooks = [];
  }
}

class Library {
  constructor(name) {
    this.name = name;
    this.books = [];
    this.members = [];
  }
  
  addBook(book) {
    this.books.push(book);
  }
  
  registerMember(member) {
    this.members.push(member);
  }
  
  borrowBook(memberId, isbn) {
    const book = this.books.find(b => b.isbn === isbn);
    const member = this.members.find(m => m.id === memberId);
    
    if (!book || !member) {
      return false;
    }
    
    if (!book.isAvailable) {
      return false;
    }
    
    book.isAvailable = false;
    member.borrowedBooks.push(isbn);
    return true;
  }
  
  returnBook(memberId, isbn) {
    const book = this.books.find(b => b.isbn === isbn);
    const member = this.members.find(m => m.id === memberId);
    
    if (!book || !member) {
      return false;
    }
    
    book.isAvailable = true;
    member.borrowedBooks = member.borrowedBooks.filter(b => b !== isbn);
    return true;
  }
  
  getAvailableBooks() {
    return this.books.filter(b => b.isAvailable);
  }
}

// Utilisation
const library = new Library("Bibliothèque Municipale");

const book1 = new Book("1984", "George Orwell", "978-0451524935", 1949);
const book2 = new Book("Le Petit Prince", "Antoine de Saint-Exupéry", "978-0156012195", 1943);

library.addBook(book1);
library.addBook(book2);

const member1 = new Member(1, "Alice Dupont", "alice@email.com");
library.registerMember(member1);

library.borrowBook(1, "978-0451524935");
console.log(library.getAvailableBooks());
```

### ✅ Solution TypeScript complète

```typescript
// library.ts

// 1. Définir les interfaces et types
interface IBook {
  title: string;
  author: string;
  isbn: string;
  publicationYear: number;
  isAvailable: boolean;
}

interface IMember {
  id: number;
  name: string;
  email: string;
  borrowedBooks: string[];  // ISBN list
}

interface ILibrary {
  name: string;
  books: Book[];
  members: Member[];
}

// Type pour les résultats d'opérations
type BorrowResult = {
  success: boolean;
  message: string;
  book?: Book;
};

// 2. Classe Book avec typage complet
class Book implements IBook {
  title: string;
  author: string;
  isbn: string;
  publicationYear: number;
  isAvailable: boolean;
  
  constructor(
    title: string,
    author: string,
    isbn: string,
    publicationYear: number
  ) {
    // Validation
    if (!title || !author || !isbn) {
      throw new Error('Title, author and ISBN are required');
    }
    
    if (publicationYear < 1000 || publicationYear > new Date().getFullYear()) {
      throw new Error('Invalid publication year');
    }
    
    this.title = title;
    this.author = author;
    this.isbn = isbn;
    this.publicationYear = publicationYear;
    this.isAvailable = true;
  }
  
  // Méthode pour obtenir l'âge du livre
  getAge(): number {
    return new Date().getFullYear() - this.publicationYear;
  }
  
  // Méthode pour formatter l'affichage
  toString(): string {
    return `"${this.title}" by ${this.author} (${this.publicationYear})`;
  }
}

// 3. Classe Member avec typage complet
class Member implements IMember {
  id: number;
  name: string;
  email: string;
  borrowedBooks: string[];
  private readonly maxBorrowLimit: number = 5;
  
  constructor(id: number, name: string, email: string) {
    // Validation
    if (id <= 0) {
      throw new Error('Member ID must be positive');
    }
    
    if (!this.isValidEmail(email)) {
      throw new Error('Invalid email address');
    }
    
    this.id = id;
    this.name = name;
    this.email = email;
    this.borrowedBooks = [];
  }
  
  private isValidEmail(email: string): boolean {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailRegex.test(email);
  }
  
  canBorrowMore(): boolean {
    return this.borrowedBooks.length < this.maxBorrowLimit;
  }
  
  getBorrowedCount(): number {
    return this.borrowedBooks.length;
  }
}

// 4. Classe Library avec typage complet
class Library implements ILibrary {
  name: string;
  books: Book[];
  members: Member[];
  
  constructor(name: string) {
    if (!name) {
      throw new Error('Library name is required');
    }
    
    this.name = name;
    this.books = [];
    this.members = [];
  }
  
  // Ajouter un livre
  addBook(book: Book): void {
    // Vérifier si l'ISBN existe déjà
    const exists = this.books.some(b => b.isbn === book.isbn);
    if (exists) {
      throw new Error(`Book with ISBN ${book.isbn} already exists`);
    }
    
    this.books.push(book);
    console.log(`✅ Book added: ${book.toString()}`);
  }
  
  // Enregistrer un membre
  registerMember(member: Member): void {
    // Vérifier si le membre existe déjà
    const exists = this.members.some(m => m.id === member.id);
    if (exists) {
      throw new Error(`Member with ID ${member.id} already exists`);
    }
    
    this.members.push(member);
    console.log(`✅ Member registered: ${member.name}`);
  }
  
  // Emprunter un livre
  borrowBook(memberId: number, isbn: string): BorrowResult {
    const book = this.books.find(b => b.isbn === isbn);
    const member = this.members.find(m => m.id === memberId);
    
    // Vérifications
    if (!book) {
      return {
        success: false,
        message: `Book with ISBN ${isbn} not found`
      };
    }
    
    if (!member) {
      return {
        success: false,
        message: `Member with ID ${memberId} not found`
      };
    }
    
    if (!book.isAvailable) {
      return {
        success: false,
        message: `Book "${book.title}" is not available`
      };
    }
    
    if (!member.canBorrowMore()) {
      return {
        success: false,
        message: `Member has reached borrowing limit`
      };
    }
    
    // Emprunter
    book.isAvailable = false;
    member.borrowedBooks.push(isbn);
    
    return {
      success: true,
      message: `Book "${book.title}" borrowed successfully`,
      book: book
    };
  }
  
  // Retourner un livre
  returnBook(memberId: number, isbn: string): BorrowResult {
    const book = this.books.find(b => b.isbn === isbn);
    const member = this.members.find(m => m.id === memberId);
    
    if (!book || !member) {
      return {
        success: false,
        message: 'Book or member not found'
      };
    }
    
    const hasBorrowed = member.borrowedBooks.includes(isbn);
    if (!hasBorrowed) {
      return {
        success: false,
        message: 'This member has not borrowed this book'
      };
    }
    
    book.isAvailable = true;
    member.borrowedBooks = member.borrowedBooks.filter(b => b !== isbn);
    
    return {
      success: true,
      message: `Book "${book.title}" returned successfully`,
      book: book
    };
  }
  
  // Obtenir les livres disponibles
  getAvailableBooks(): Book[] {
    return this.books.filter(b => b.isAvailable);
  }
  
  // Rechercher des livres par auteur
  searchByAuthor(author: string): Book[] {
    return this.books.filter(b => 
      b.author.toLowerCase().includes(author.toLowerCase())
    );
  }
  
  // Obtenir les statistiques
  getStats(): {
    totalBooks: number;
    availableBooks: number;
    borrowedBooks: number;
    totalMembers: number;
  } {
    return {
      totalBooks: this.books.length,
      availableBooks: this.getAvailableBooks().length,
      borrowedBooks: this.books.filter(b => !b.isAvailable).length,
      totalMembers: this.members.length
    };
  }
}

// 5. Utilisation avec gestion d'erreurs
try {
  const library = new Library("Bibliothèque Municipale de Paris");
  
  // Ajouter des livres
  const book1 = new Book("1984", "George Orwell", "978-0451524935", 1949);
  const book2 = new Book("Le Petit Prince", "Antoine de Saint-Exupéry", "978-0156012195", 1943);
  const book3 = new Book("Animal Farm", "George Orwell", "978-0451526342", 1945);
  
  library.addBook(book1);
  library.addBook(book2);
  library.addBook(book3);
  
  // Enregistrer des membres
  const member1 = new Member(1, "Alice Dupont", "alice@email.com");
  const member2 = new Member(2, "Bob Martin", "bob@email.com");
  
  library.registerMember(member1);
  library.registerMember(member2);
  
  // Emprunter des livres
  const borrowResult1 = library.borrowBook(1, "978-0451524935");
  console.log(borrowResult1.message);
  
  const borrowResult2 = library.borrowBook(1, "978-0156012195");
  console.log(borrowResult2.message);
  
  // Essayer d'emprunter un livre déjà emprunté
  const borrowResult3 = library.borrowBook(2, "978-0451524935");
  console.log(borrowResult3.message);  // Échec : livre non disponible
  
  // Afficher les livres disponibles
  console.log("\n📚 Livres disponibles :");
  library.getAvailableBooks().forEach(book => {
    console.log(`  - ${book.toString()}`);
  });
  
  // Rechercher par auteur
  console.log("\n🔍 Livres de George Orwell :");
  library.searchByAuthor("Orwell").forEach(book => {
    console.log(`  - ${book.title}`);
  });
  
  // Retourner un livre
  const returnResult = library.returnBook(1, "978-0451524935");
  console.log(`\n${returnResult.message}`);
  
  // Statistiques
  const stats = library.getStats();
  console.log("\n📊 Statistiques de la bibliothèque :");
  console.log(`  Total livres: ${stats.totalBooks}`);
  console.log(`  Disponibles: ${stats.availableBooks}`);
  console.log(`  Empruntés: ${stats.borrowedBooks}`);
  console.log(`  Membres: ${stats.totalMembers}`);
  
} catch (error) {
  if (error instanceof Error) {
    console.error(`❌ Erreur: ${error.message}`);
  }
}

// Export pour utilisation dans d'autres modules
export { Book, Member, Library };
export type { IBook, IMember, ILibrary, BorrowResult };
```

---

## 🎯 Points Clés du Chapitre 1 pour l'Entretien

### Concepts Fondamentaux

1. **TypeScript = JavaScript + Types**
   - Superset de JavaScript
   - Compilation vers JavaScript
   - Détection d'erreurs à la compilation

2. **Installation et Configuration**
   - `npm install typescript`
   - `tsconfig.json` : fichier de configuration central
   - Options importantes : `strict`, `target`, `module`

3. **Migration Progressive**
   - `allowJs: true` : accepte les fichiers .js
   - `checkJs: true` : vérifie les fichiers .js
   - Conversion fichier par fichier

4. **Type Checking en JavaScript**
   - Commentaire `// @ts-check`
   - JSDoc pour typage : `@param`, `@returns`, `@typedef`
   - Pas besoin de tout convertir en .ts

5. **Types Tiers**
   - DefinitelyTyped : `@types/package-name`
   - Fichiers `.d.ts` pour déclarations custom
   - `typeRoots` dans tsconfig.json

### Questions Fréquentes en Entretien

**Q1 : Quelle est la différence entre TypeScript et JavaScript ?**
> TypeScript ajoute un système de types statiques à JavaScript, permettant de détecter les erreurs à la compilation plutôt qu'à l'exécution. Il se compile en JavaScript standard.

**Q2 : Comment intégrer TypeScript dans un projet JavaScript existant ?**
> Migration progressive : activer `allowJs`, créer `tsconfig.json`, convertir fichier par fichier, durcir la configuration progressivement avec `strict: true`.

**Q3 : Qu'est-ce que le fichier tsconfig.json ?**
> C'est le fichier de configuration qui définit les options de compilation TypeScript : version JS cible, chemins, règles de vérification strictes, etc.

**Q4 : Comment typer une bibliothèque JavaScript sans types ?**
> Trois options : chercher `@types/library` dans DefinitelyTyped, créer un fichier `.d.ts` avec `declare module`, ou utiliser JSDoc dans le code JavaScript.

**Q5 : Que signifie "strict mode" en TypeScript ?**
> Active toutes les vérifications strictes : `noImplicitAny`, `strictNullChecks`, `strictFunctionTypes`, etc. Recommandé pour nouveaux projets.

---

## ⚠️ Pièges à Éviter

### ❌ Erreur 1 : Activer strict mode immédiatement sur gros projet
```json
// ❌ Mauvais pour migration
{
  "compilerOptions": {
    "strict": true,  // Trop d'erreurs d'un coup !
    "allowJs": true
  }
}
```

```json
// ✅ Bon pour migration
{
  "compilerOptions": {
    "strict": false,      // Commencer permissif
    "allowJs": true,
    "checkJs": false      // Activer progressivement
  }
}
```

### ❌ Erreur 2 : Oublier d'installer les types Node.js
```typescript
// ❌ Erreur : Cannot find name 'process'
const port = process.env.PORT;
```

```bash
# ✅ Solution
npm install --save-dev @types/node
```

### ❌ Erreur 3 : Utiliser 'any' partout
```typescript
// ❌ Perd tout l'intérêt de TypeScript
function process(data: any): any {
  return data.something;
}
```

```typescript
// ✅ Typer correctement
interface Data {
  something: string;
}

function process(data: Data): string {
  return data.something;
}
```

### ❌ Erreur 4 : Ne pas gérer null/undefined
```typescript
// ❌ Peut causer des erreurs runtime
function getName(user: User): string {
  return user.name.toUpperCase();  // user.name peut être undefined
}
```

```typescript
// ✅ Gestion correcte
function getName(user: User): string {
  return user.name?.toUpperCase() ?? 'Unknown';
}
```

---

## 📝 Checklist de Révision

- [ ] Je sais installer TypeScript localement et globalement
- [ ] Je comprends le rôle du tsconfig.json
- [ ] Je peux configurer TypeScript pour différents projets (Node, Angular, React)
- [ ] Je connais la stratégie de migration progressive
- [ ] Je sais utiliser @ts-check et JSDoc pour typer du JavaScript
- [ ] Je sais installer et utiliser des types tiers (@types/*)
- [ ] Je peux créer des fichiers .d.ts pour bibliothèques sans types
- [ ] Je comprends les options strict de TypeScript
- [ ] Je sais gérer les erreurs de compilation courantes

---

## 🔗 Ressources Complémentaires

- **Documentation officielle** : https://www.typescriptlang.org/docs/
- **DefinitelyTyped** : https://github.com/DefinitelyTyped/DefinitelyTyped
- **TSConfig Reference** : https://www.typescriptlang.org/tsconfig
- **TypeScript Deep Dive** : https://basarat.gitbook.io/typescript/

---