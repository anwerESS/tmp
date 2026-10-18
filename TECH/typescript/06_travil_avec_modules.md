# Chapitre 6 : Travail avec les Modules

## 📑 Table des matières
- [6.1 Module Basics](#61-module-basics)
- [6.2 Imports et Exports](#62-imports-et-exports)
- [6.3 Ambient Modules (Types Globaux)](#63-ambient-modules-types-globaux)
- [6.4 Declaration Merging](#64-declaration-merging)
- [6.5 Exécution de Code Modulaire](#65-exécution-de-code-modulaire)
- [Points Clés pour l'Entretien](#-points-clés-pour-lentretien)

---

## 6.1 Module Basics

### 🎯 Qu'est-ce qu'un Module ?

Un **module** est un fichier qui contient du code (fonctions, classes, variables) qui peut être **importé** et **exporté**. En TypeScript, tout fichier contenant un `import` ou `export` est considéré comme un module.

#### Fichier sans import/export : Script Global

```typescript
// global.ts (pas un module)
const globalVar = "Je suis global";

function globalFunction() {
  console.log("Fonction globale");
}

// Accessible partout sans import
```

#### Fichier avec import/export : Module

```typescript
// module.ts (est un module)
export const moduleVar = "Je suis dans un module";

export function moduleFunction() {
  console.log("Fonction de module");
}

// Nécessite un import pour être utilisé
```

### 📦 Avantages des Modules

**✅ Encapsulation** : Code isolé, pas de pollution globale
**✅ Réutilisabilité** : Importer où nécessaire
**✅ Maintenance** : Code organisé et structuré
**✅ Tree-shaking** : Bundlers peuvent supprimer le code non utilisé
**✅ Lazy loading** : Charger à la demande

### 🔧 Configuration des Modules

```json
// tsconfig.json
{
  "compilerOptions": {
    "module": "ESNext",           // Ou "CommonJS", "AMD", "UMD"
    "moduleResolution": "node",   // Ou "bundler", "classic"
    "esModuleInterop": true,      // Interop avec CommonJS
    "allowSyntheticDefaultImports": true
  }
}
```

#### Systèmes de Modules

**ES Modules (ESM)** : Standard moderne
```typescript
import { something } from "./module";
export const value = 42;
```

**CommonJS** : Node.js traditionnel
```typescript
const { something } = require("./module");
module.exports = { value: 42 };
```

**AMD** : Navigateurs (ancien)
**UMD** : Universal (fonctionne partout)

---

## 6.2 Imports et Exports

### 🎯 Exports Nommés

```typescript
// math.ts

// Export de variable
export const PI = 3.14159;

// Export de fonction
export function add(a: number, b: number): number {
  return a + b;
}

// Export de classe
export class Calculator {
  multiply(a: number, b: number): number {
    return a * b;
  }
}

// Export de type
export type MathOperation = (a: number, b: number) => number;

// Export d'interface
export interface Shape {
  area(): number;
}
```

#### Export groupé

```typescript
// math.ts
const PI = 3.14159;
const E = 2.71828;

function add(a: number, b: number) {
  return a + b;
}

function subtract(a: number, b: number) {
  return a - b;
}

// Exporter tout d'un coup
export { PI, E, add, subtract };
```

#### Export avec renommage

```typescript
// math.ts
function add(a: number, b: number) {
  return a + b;
}

// Exporter avec un autre nom
export { add as sum };
```

### 📥 Imports Nommés

```typescript
// app.ts

// Import simple
import { PI, add } from "./math";

console.log(PI);        // 3.14159
console.log(add(5, 3)); // 8

// Import avec renommage
import { add as sum } from "./math";
console.log(sum(5, 3)); // 8

// Import multiple
import { PI, add, subtract, Calculator } from "./math";

// Import de type
import type { MathOperation } from "./math";
const operation: MathOperation = add;
```

### 🌟 Export par Défaut

```typescript
// logger.ts
export default class Logger {
  log(message: string): void {
    console.log(`[LOG] ${message}`);
  }
}

// Ou
class Logger {
  log(message: string): void {
    console.log(`[LOG] ${message}`);
  }
}
export default Logger;

// Fonction par défaut
export default function greet(name: string) {
  return `Hello, ${name}!`;
}
```

#### Import par défaut

```typescript
// app.ts

// Nom libre pour l'import par défaut
import Logger from "./logger";
import MyLogger from "./logger";  // Même chose

const logger = new Logger();
logger.log("Hello");

// Avec fonction
import greet from "./greet";
console.log(greet("Alice"));
```

### 🔄 Combiner Export par Défaut et Nommés

```typescript
// utils.ts

// Export par défaut
export default class Utils {
  static format(text: string): string {
    return text.toUpperCase();
  }
}

// Exports nommés
export const VERSION = "1.0.0";
export function log(message: string): void {
  console.log(message);
}
```

```typescript
// app.ts

// Importer tout
import Utils, { VERSION, log } from "./utils";

console.log(Utils.format("hello")); // "HELLO"
console.log(VERSION);                // "1.0.0"
log("test");                         // "test"
```

### ⭐ Import Namespace

```typescript
// math.ts
export const PI = 3.14159;
export function add(a: number, b: number) { return a + b; }
export function subtract(a: number, b: number) { return a - b; }
```

```typescript
// app.ts

// Tout importer dans un namespace
import * as Math from "./math";

console.log(Math.PI);           // 3.14159
console.log(Math.add(5, 3));    // 8
console.log(Math.subtract(10, 4)); // 6
```

### 🔁 Ré-exports

```typescript
// shapes/index.ts

// Ré-exporter depuis d'autres modules
export { Circle } from "./circle";
export { Square } from "./square";
export { Triangle } from "./triangle";

// Ré-exporter avec renommage
export { Rectangle as Rect } from "./rectangle";

// Ré-exporter tout
export * from "./polygon";

// Ré-exporter tout sauf certains
export * from "./shapes" exclude { InternalShape };
```

### 📝 Exemple Complet : Structure de Projet

```typescript
// ========================================
// src/models/user.ts
// ========================================
export interface User {
  id: string;
  name: string;
  email: string;
  role: "admin" | "user";
}

export class UserModel implements User {
  constructor(
    public id: string,
    public name: string,
    public email: string,
    public role: "admin" | "user"
  ) {}
  
  isAdmin(): boolean {
    return this.role === "admin";
  }
}

// ========================================
// src/models/product.ts
// ========================================
export interface Product {
  id: string;
  name: string;
  price: number;
  stock: number;
}

export class ProductModel implements Product {
  constructor(
    public id: string,
    public name: string,
    public price: number,
    public stock: number
  ) {}
  
  isAvailable(): boolean {
    return this.stock > 0;
  }
}

// ========================================
// src/models/index.ts (Barrel)
// ========================================
export * from "./user";
export * from "./product";

// ========================================
// src/services/user.service.ts
// ========================================
import { User, UserModel } from "../models";

export class UserService {
  private users: Map<string, User> = new Map();
  
  create(name: string, email: string, role: "admin" | "user"): User {
    const user = new UserModel(
      `user-${Date.now()}`,
      name,
      email,
      role
    );
    this.users.set(user.id, user);
    return user;
  }
  
  getById(id: string): User | undefined {
    return this.users.get(id);
  }
  
  getAll(): User[] {
    return Array.from(this.users.values());
  }
}

// ========================================
// src/services/product.service.ts
// ========================================
import { Product, ProductModel } from "../models";

export class ProductService {
  private products: Map<string, Product> = new Map();
  
  create(name: string, price: number, stock: number): Product {
    const product = new ProductModel(
      `prod-${Date.now()}`,
      name,
      price,
      stock
    );
    this.products.set(product.id, product);
    return product;
  }
  
  getById(id: string): Product | undefined {
    return this.products.get(id);
  }
  
  getAvailable(): Product[] {
    return Array.from(this.products.values())
      .filter(p => (p as ProductModel).isAvailable());
  }
}

// ========================================
// src/services/index.ts
// ========================================
export * from "./user.service";
export * from "./product.service";

// ========================================
// src/index.ts (Point d'entrée)
// ========================================
import { UserService, ProductService } from "./services";

const userService = new UserService();
const productService = new ProductService();

// Créer des utilisateurs
const admin = userService.create("Admin", "admin@example.com", "admin");
const user = userService.create("John", "john@example.com", "user");

console.log("Users:", userService.getAll());

// Créer des produits
const laptop = productService.create("Laptop", 999, 10);
const mouse = productService.create("Mouse", 25, 0);

console.log("Available products:", productService.getAvailable());
```

---

## 6.3 Ambient Modules (Types Globaux)

### 🎯 Qu'est-ce qu'un Ambient Module ?

Un **ambient module** déclare des types sans fournir d'implémentation. Utile pour :
- Typer des bibliothèques JavaScript
- Déclarer des variables globales
- Étendre des modules existants

#### Fichiers de déclaration `.d.ts`

```typescript
// types/custom.d.ts

// Déclarer une variable globale
declare const API_URL: string;

// Déclarer une fonction globale
declare function log(message: string): void;

// Déclarer un namespace global
declare namespace MyLibrary {
  function init(config: Config): void;
  
  interface Config {
    apiKey: string;
    debug: boolean;
  }
}

// Déclarer un module externe
declare module "my-untyped-library" {
  export function doSomething(value: string): number;
  export class MyClass {
    constructor(value: string);
    getValue(): string;
  }
}
```

### 🌍 Étendre les Types Globaux

```typescript
// types/global.d.ts

// Étendre l'objet Window
interface Window {
  myApp: {
    version: string;
    config: {
      apiUrl: string;
    };
  };
}

// Étendre String
interface String {
  toTitleCase(): string;
}

// Étendre Array
interface Array<T> {
  first(): T | undefined;
  last(): T | undefined;
}
```

```typescript
// app.ts

// Utilisation des extensions
window.myApp = {
  version: "1.0.0",
  config: {
    apiUrl: "https://api.example.com"
  }
};

// Extension de String
String.prototype.toTitleCase = function() {
  return this.split(" ")
    .map(word => word.charAt(0).toUpperCase() + word.slice(1).toLowerCase())
    .join(" ");
};

const title = "hello world".toTitleCase();
console.log(title); // "Hello World"

// Extension de Array
Array.prototype.first = function() {
  return this[0];
};

Array.prototype.last = function() {
  return this[this.length - 1];
};

const numbers = [1, 2, 3, 4, 5];
console.log(numbers.first()); // 1
console.log(numbers.last());  // 5
```

### 📦 Typer une Bibliothèque JavaScript

```typescript
// types/jquery.d.ts

// Déclarer jQuery qui n'a pas de types
declare module "jquery" {
  interface JQuery {
    addClass(className: string): JQuery;
    removeClass(className: string): JQuery;
    text(content: string): JQuery;
    text(): string;
    on(event: string, handler: () => void): JQuery;
  }
  
  interface JQueryStatic {
    (selector: string): JQuery;
    ajax(settings: {
      url: string;
      method?: string;
      data?: any;
      success?: (data: any) => void;
    }): void;
  }
  
  const $: JQueryStatic;
  export = $;
}
```

```typescript
// app.ts
import $ from "jquery";

$("#myButton")
  .addClass("active")
  .text("Click me")
  .on("click", () => {
    console.log("Button clicked!");
  });

$.ajax({
  url: "/api/data",
  method: "GET",
  success: (data) => {
    console.log("Data:", data);
  }
});
```

### 🔧 Modules Ambiants dans le Projet

```typescript
// types/env.d.ts

// Variables d'environnement
declare namespace NodeJS {
  interface ProcessEnv {
    NODE_ENV: "development" | "production" | "test";
    API_URL: string;
    DATABASE_URL: string;
    PORT: string;
    SECRET_KEY: string;
  }
}
```

```typescript
// app.ts

// Utilisation type-safe
const environment = process.env.NODE_ENV;  // Type: "development" | "production" | "test"
const apiUrl = process.env.API_URL;        // Type: string
const port = parseInt(process.env.PORT);   // Type: number

if (environment === "development") {
  console.log("Mode développement");
}
```

### 📝 Exemple : Types pour API

```typescript
// types/api.d.ts

declare namespace API {
  interface Response<T> {
    data: T;
    status: number;
    message: string;
  }
  
  interface User {
    id: string;
    name: string;
    email: string;
  }
  
  interface Product {
    id: string;
    name: string;
    price: number;
  }
  
  interface PaginatedResponse<T> {
    data: T[];
    page: number;
    pageSize: number;
    total: number;
  }
}
```

```typescript
// services/api.service.ts

class ApiService {
  async getUser(id: string): Promise<API.Response<API.User>> {
    const response = await fetch(`/api/users/${id}`);
    return response.json();
  }
  
  async getProducts(page: number = 1): Promise<API.PaginatedResponse<API.Product>> {
    const response = await fetch(`/api/products?page=${page}`);
    return response.json();
  }
}
```

---

## 6.4 Declaration Merging

### 🎯 Qu'est-ce que le Declaration Merging ?

Le **declaration merging** permet de déclarer plusieurs fois le même nom, et TypeScript les **fusionne** automatiquement.

#### Fusion d'Interfaces

```typescript
// Première déclaration
interface User {
  id: string;
  name: string;
}

// Deuxième déclaration
interface User {
  email: string;
  age: number;
}

// TypeScript fusionne automatiquement
// interface User {
//   id: string;
//   name: string;
//   email: string;
//   age: number;
// }

const user: User = {
  id: "1",
  name: "Alice",
  email: "alice@example.com",
  age: 30
};
```

#### Fusion de Namespaces

```typescript
// Première déclaration
namespace Utils {
  export function log(message: string): void {
    console.log(message);
  }
}

// Deuxième déclaration
namespace Utils {
  export function warn(message: string): void {
    console.warn(message);
  }
}

// Fusionné en :
// namespace Utils {
//   export function log(message: string): void
//   export function warn(message: string): void
// }

Utils.log("Information");
Utils.warn("Attention !");
```

#### Fusion Namespace + Classe

```typescript
class Album {
  label: Album.AlbumLabel;
  
  constructor(label: Album.AlbumLabel) {
    this.label = label;
  }
}

namespace Album {
  export class AlbumLabel {
    constructor(public name: string) {}
  }
}

// Utilisation
const label = new Album.AlbumLabel("Universal");
const album = new Album(label);
```

#### Fusion Namespace + Fonction

```typescript
function buildLabel(name: string): string {
  return buildLabel.prefix + name + buildLabel.suffix;
}

namespace buildLabel {
  export let prefix = "[";
  export let suffix = "]";
}

console.log(buildLabel("Test"));  // "[Test]"

buildLabel.prefix = "<<";
buildLabel.suffix = ">>";

console.log(buildLabel("Test"));  // "<<Test>>"
```

### 🔧 Augmentation de Modules

```typescript
// Fichier original du module
// my-library.ts
export class MyLibrary {
  doSomething(): void {
    console.log("Doing something");
  }
}
```

```typescript
// Votre fichier d'augmentation
// my-library-extension.d.ts

import { MyLibrary } from "my-library";

declare module "my-library" {
  interface MyLibrary {
    doSomethingElse(): void;
  }
}
```

```typescript
// Implémentation
import { MyLibrary } from "my-library";

MyLibrary.prototype.doSomethingElse = function() {
  console.log("Doing something else");
};

// Utilisation
const lib = new MyLibrary();
lib.doSomething();      // Méthode originale
lib.doSomethingElse();  // Méthode ajoutée
```

### 📝 Exemple : Étendre Express

```typescript
// types/express.d.ts

import { User } from "../models/user";

declare global {
  namespace Express {
    interface Request {
      user?: User;
      correlationId?: string;
    }
    
    interface Response {
      sendSuccess(data: any): void;
      sendError(message: string, code?: number): void;
    }
  }
}
```

```typescript
// middleware/auth.ts
import { Request, Response, NextFunction } from "express";

export function authMiddleware(req: Request, res: Response, next: NextFunction) {
  // Ajouter l'utilisateur à la requête
  req.user = {
    id: "123",
    name: "John",
    email: "john@example.com",
    role: "user"
  };
  
  req.correlationId = `req-${Date.now()}`;
  
  next();
}

// Extension de Response
Response.prototype.sendSuccess = function(data: any) {
  this.json({
    success: true,
    data,
    timestamp: new Date()
  });
};

Response.prototype.sendError = function(message: string, code: number = 400) {
  this.status(code).json({
    success: false,
    error: message,
    timestamp: new Date()
  });
};
```

```typescript
// routes/users.ts
import { Router } from "express";
import { authMiddleware } from "../middleware/auth";

const router = Router();

router.get("/profile", authMiddleware, (req, res) => {
  // req.user est typé !
  if (!req.user) {
    return res.sendError("Not authenticated", 401);
  }
  
  res.sendSuccess({
    user: req.user,
    correlationId: req.correlationId
  });
});

export default router;
```

---

## 6.5 Exécution de Code Modulaire

### 🎯 Configuration pour l'Exécution

```json
// tsconfig.json
{
  "compilerOptions": {
    "target": "ES2020",
    "module": "ESNext",              // Ou "CommonJS"
    "moduleResolution": "node",
    "outDir": "./dist",
    "rootDir": "./src",
    "esModuleInterop": true
  }
}
```

```json
// package.json
{
  "name": "my-project",
  "version": "1.0.0",
  "type": "module",  // Pour ES Modules
  "scripts": {
    "build": "tsc",
    "start": "node dist/index.js",
    "dev": "ts-node src/index.ts"
  },
  "devDependencies": {
    "typescript": "^5.3.0",
    "ts-node": "^10.9.0",
    "@types/node": "^20.0.0"
  }
}
```

### 🚀 Avec Node.js

#### ES Modules (Modern)

```typescript
// src/index.ts
import { createServer } from "http";
import { UserService } from "./services/user.service";

const userService = new UserService();

const server = createServer((req, res) => {
  if (req.url === "/users") {
    const users = userService.getAll();
    res.writeHead(200, { "Content-Type": "application/json" });
    res.end(JSON.stringify(users));
  } else {
    res.writeHead(404);
    res.end("Not found");
  }
});

server.listen(3000, () => {
  console.log("Server running on http://localhost:3000");
});
```

#### CommonJS (Traditionnel)

```typescript
// src/index.ts
const http = require("http");
const { UserService } = require("./services/user.service");

const userService = new UserService();

const server = http.createServer((req, res) => {
  // ...
});

module.exports = { server };
```

### 📦 Avec Bundlers

#### Webpack

```javascript
// webpack.config.js
module.exports = {
  entry: "./src/index.ts",
  output: {
    filename: "bundle.js",
    path: __dirname + "/dist"
  },
  resolve: {
    extensions: [".ts", ".js"]
  },
  module: {
    rules: [
      {
        test: /\.ts$/,
        use: "ts-loader",
        exclude: /node_modules/
      }
    ]
  }
};
```

#### Vite (Modern)

```typescript
// vite.config.ts
import { defineConfig } from "vite";

export default defineConfig({
  build: {
    lib: {
      entry: "./src/index.ts",
      name: "MyLibrary",
      fileName: (format) => `my-library.${format}.js`
    }
  }
});
```

### 📝 Exemple Complet : Application Express

```typescript
// ========================================
// src/models/user.model.ts
// ========================================
export interface User {
  id: string;
  name: string;
  email: string;
  createdAt: Date;
}

export class UserModel implements User {
  constructor(
    public id: string,
    public name: string,
    public email: string,
    public createdAt: Date = new Date()
  ) {}
}

// ========================================
// src/services/user.service.ts
// ========================================
import { User, UserModel } from "../models/user.model";

export class UserService {
  private users: Map<string, User> = new Map();
  
  create(name: string, email: string): User {
    const user = new UserModel(
      `user-${Date.now()}`,
      name,
      email
    );
    this.users.set(user.id, user);
    return user;
  }
  
  findById(id: string): User | undefined {
    return this.users.get(id);
  }
  
  findAll(): User[] {
    return Array.from(this.users.values());
  }
  
  update(id: string, data: Partial<User>): User | undefined {
    const user = this.users.get(id);
    if (!user) return undefined;
    
    const updated = { ...user, ...data };
    this.users.set(id, updated);
    return updated;
  }
  
  delete(id: string): boolean {
    return this.users.delete(id);
  }
}

// ========================================
// src/routes/user.routes.ts
// ========================================
import { Router, Request, Response } from "express";
import { UserService } from "../services/user.service";

export function createUserRoutes(userService: UserService): Router {
  const router = Router();
  
  // GET /users
  router.get("/", (req: Request, res: Response) => {
    const users = userService.findAll();
    res.json({ users });
  });
  
  // GET /users/:id
  router.get("/:id", (req: Request, res: Response) => {
    const user = userService.findById(req.params.id);
    
    if (!user) {
      return res.status(404).json({ error: "User not found" });
    }
    
    res.json({ user });
  });
  
  // POST /users
  router.post("/", (req: Request, res: Response) => {
    const { name, email } = req.body;
    
    if (!name || !email) {
      return res.status(400).json({ error: "Name and email required" });
    }
    
    const user = userService.create(name, email);
    res.status(201).json({ user });
  });
  
  // PUT /users/:id
  router.put("/:id", (req: Request, res: Response) => {
    const { name, email } = req.body;
    const user = userService.update(req.params.id, { name, email });
    
    if (!user) {
      return res.status(404).json({ error: "User not found" });
    }
    
    res.json({ user });
  });
  
  // DELETE /users/:id
  router.delete("/:id", (req: Request, res: Response) => {
    const deleted = userService.delete(req.params.id);
    
    if (!deleted) {
      return res.status(404).json({ error: "User not found" });
    }
    
    res.status(204).send();
  });
  
  return router;
}

// ========================================
// src/app.ts
// ========================================
import express, { Express } from "express";
import { UserService } from "./services/user.service";
import { createUserRoutes } from "./routes/user.routes";

export function createApp(): Express {
  const app = express();
  
  // Middleware
  app.use(express.json());
  app.use(express.urlencoded({ extended: true }));
  
  // Services
  const userService = new UserService();
  
  // Routes
  app.use("/users", createUserRoutes(userService));
  
  // Health check
  app.get("/health", (req, res) => {
    res.json({ status: "ok", timestamp: new Date() });
  });
  
  return app;
}

// ========================================
// src/index.ts (Point d'entrée)
// ========================================
import { createApp } from "./app";

const PORT = process.env.PORT || 3000;
const app = createApp();

app.listen(PORT, () => {
  console.log(`🚀 Server running on http://localhost:${PORT}`);
  console.log(`📊 Health check: http://localhost:${PORT}/health`);
  console.log(`👥 Users API: http://localhost:${PORT}/users`);
});
```

### 🧪 Tests avec Modules

```typescript
// tests/user.service.test.ts
import { describe, it, expect, beforeEach } from "@jest/globals";
import { UserService } from "../src/services/user.service";

describe("UserService", () => {
  let userService: UserService;
  
  beforeEach(() => {
    userService = new UserService();
  });
  
  it("should create a user", () => {
    const user = userService.create("Alice", "alice@example.com");
    
    expect(user.id).toBeDefined();
    expect(user.name).toBe("Alice");
    expect(user.email).toBe("alice@example.com");
  });
  
  it("should find user by id", () => {
    const created = userService.create("Bob", "bob@example.com");
    const found = userService.findById(created.id);
    
    expect(found).toEqual(created);
  });
  
  it("should return undefined for non-existent user", () => {
    const found = userService.findById("non-existent");
    
    expect(found).toBeUndefined();
  });
  
  it("should update user", () => {
    const user = userService.create("Charlie", "charlie@example.com");
    const updated = userService.update(user.id, { name: "Charles" });
    
    expect(updated?.name).toBe("Charles");
    expect(updated?.email).toBe("charlie@example.com");
  });
  
  it("should delete user", () => {
    const user = userService.create("Dave", "dave@example.com");
    const deleted = userService.delete(user.id);
    
    expect(deleted).toBe(true);
    expect(userService.findById(user.id)).toBeUndefined();
  });
});
```

---

## 🎯 Points Clés pour l'Entretien

### Concepts Fondamentaux
1. **Module** : fichier avec import/export
2. **Script** : fichier sans import/export (global)
3. **ES Modules** : standard moderne (import/export)
4. **CommonJS** : Node.js traditionnel (require/module.exports)
5. **Barrel exports** : index.ts qui ré-exporte

### Imports et Exports
1. **Export nommé** : `export const value = 42`
2. **Export par défaut** : `export default class MyClass`
3. **Import nommé** : `import { value } from "./module"`
4. **Import par défaut** : `import MyClass from "./module"`
5. **Import namespace** : `import * as Utils from "./utils"`
6. **Ré-export** : `export * from "./module"`

### Ambient Modules
1. **Fichiers .d.ts** : déclarations sans implémentation
2. **declare** : déclarer variables/fonctions globales
3. **declare module** : typer bibliothèques sans types
4. **Étendre globaux** : Window, String, Array, etc.

### Declaration Merging
1. **Interfaces** : peuvent être déclarées plusieurs fois
2. **Namespaces** : peuvent être fusionnés
3. **Module augmentation** : étendre modules existants
4. **Express types** : ajouter propriétés à Request/Response

---

## ⚠️ Pièges à Éviter

### Piège 1 : Import circulaire

```typescript
// ❌ Mauvais : dépendance circulaire
// user.ts
import { Product } from "./product";
export class User {
  favoriteProducts: Product[] = [];
}

// product.ts
import { User } from "./user";
export class Product {
  likedBy: User[] = [];
}

// ✅ Solution : extraire interface commune
// interfaces.ts
export interface IUser {
  id: string;
  name: string;
}

export interface IProduct {
  id: string;
  name: string;
}

// user.ts
import { IProduct } from "./interfaces";
export class User {
  favoriteProducts: IProduct[] = [];
}

// product.ts
import { IUser } from "./interfaces";
export class Product {
  likedBy: IUser[] = [];
}
```

### Piège 2 : Oublier .js dans les imports (Node.js avec ESM)

```typescript
// ❌ Erreur avec type: "module" dans package.json
import { utils } from "./utils";

// ✅ Correct avec ESM
import { utils } from "./utils.js";

// Ou dans tsconfig.json
{
  "compilerOptions": {
    "moduleResolution": "bundler"  // Permet d'omettre .js
  }
}
```

### Piège 3 : Default export vs named export

```typescript
// module.ts
export default class User {}
export const VERSION = "1.0";

// ❌ Import incorrect
import { User } from "./module";  // Error!

// ✅ Import correct
import User, { VERSION } from "./module";
```

### Piège 4 : Type-only imports oubliés

```typescript
// ❌ Import complet (inclut le code)
import { User } from "./types";

// ✅ Import type-only (pas de code)
import type { User } from "./types";

// Ou import individuel
import { type User, createUser } from "./module";
```

### Piège 5 : Extension globale sans declare global

```typescript
// ❌ Ne fonctionne pas
interface String {
  myMethod(): string;
}

// ✅ Correct
declare global {
  interface String {
    myMethod(): string;
  }
}

export {};  // Nécessaire pour en faire un module
```

---

## 📝 Checklist de Révision

### Module Basics
- [ ] Je comprends la différence module vs script
- [ ] Je connais ES Modules et CommonJS
- [ ] Je sais configurer le module system
- [ ] Je comprends moduleResolution

### Imports et Exports
- [ ] Je maîtrise les exports nommés
- [ ] Je sais utiliser l'export par défaut
- [ ] Je comprends les imports namespace
- [ ] Je sais ré-exporter des modules
- [ ] Je connais les barrel exports

### Ambient Modules
- [ ] Je sais créer des fichiers .d.ts
- [ ] Je peux déclarer des variables globales
- [ ] Je sais typer des bibliothèques JS
- [ ] Je peux étendre les types globaux

### Declaration Merging
- [ ] Je comprends la fusion d'interfaces
- [ ] Je sais augmenter des modules
- [ ] Je peux étendre Express types
- [ ] Je connais les limitations

### Exécution
- [ ] Je sais configurer un projet Node.js
- [ ] Je peux exécuter du code modulaire
- [ ] Je connais les bundlers (Webpack, Vite)
- [ ] Je sais écrire des tests avec modules

---

## 💡 Exercices Pratiques

### Exercice 1 : Structure de Projet

Créez une application avec :
- Modèles (User, Product)
- Services (UserService, ProductService)
- Routes (Express)
- Tests unitaires
- Barrel exports

### Exercice 2 : Typer une Bibliothèque

Créez les types pour une bibliothèque imaginaire :
```typescript
// fake-lib.d.ts
declare module "fake-lib" {
  export function init(config: Config): void;
  export class Database {
    connect(): Promise<void>;
    query(sql: string): Promise<any[]>;
  }
  export interface Config {
    host: string;
    port: number;
  }
}
```

### Exercice 3 : Étendre Express

Ajoutez des propriétés custom à Express :
- `req.requestId` (string)
- `req.user` (User)
- `res.sendSuccess(data)` (méthode)
- `res.sendError(message)` (méthode)

---

## 🔗 Ressources Complémentaires

### Documentation Officielle
- **Modules** : https://www.typescriptlang.org/docs/handbook/modules.html
- **Module Resolution** : https://www.typescriptlang.org/docs/handbook/module-resolution.html
- **Declaration Files** : https://www.typescriptlang.org/docs/handbook/declaration-files/introduction.html
- **Declaration Merging** : https://www.typescriptlang.org/docs/handbook/declaration-merging.html

### Outils
- **ts-node** : Exécuter TypeScript directement
- **Webpack** : Bundler pour applications
- **Vite** : Build tool moderne
- **esbuild** : Bundler ultra-rapide

### Patterns
- **Barrel exports** : Simplifier les imports
- **Re-exports** : Organiser les modules
- **Module augmentation** : Étendre bibliothèques

---

## 📊 Résumé du Chapitre 6

Ce chapitre final a couvert les **modules** en TypeScript :

### Organisation du Code
✅ **Module basics** : différence module vs script
✅ **ES Modules** : import/export moderne
✅ **CommonJS** : require/module.exports
✅ **Barrel exports** : index.ts pour ré-exporter

### Imports et Exports
✅ **Named exports** : exporter plusieurs éléments
✅ **Default export** : un export principal
✅ **Namespace import** : tout importer ensemble
✅ **Re-exports** : organiser les modules

### Types et Déclarations
✅ **Ambient modules** : fichiers .d.ts
✅ **Declare** : déclarer sans implémenter
✅ **Global extensions** : étendre Window, String, etc.
✅ **Declaration merging** : fusionner interfaces

### Exécution
✅ **Node.js** : exécuter du code modulaire
✅ **Bundlers** : Webpack, Vite, esbuild
✅ **Tests** : Jest avec modules
✅ **Express** : application complète

---

## 🎓 Pour l'Entretien

### Questions Fréquentes

**Q1 : Différence entre module et script ?**
> Un module a au moins un import ou export. Un script est un fichier sans import/export, son code est global.

**Q2 : ES Modules vs CommonJS ?**
> ES Modules utilisent import/export (moderne, standard). CommonJS utilise require/module.exports (Node.js traditionnel).

**Q3 : Qu'est-ce qu'un barrel export ?**
> Un fichier index.ts qui ré-exporte plusieurs modules pour simplifier les imports : `import { User, Product } from "./models"` au lieu de deux imports séparés.

**Q4 : Comment typer une bibliothèque JS sans types ?**
> Créer un fichier .d.ts avec `declare module "lib-name"` et définir les types des exports.

**Q5 : Qu'est-ce que le declaration merging ?**
> Capacité de déclarer plusieurs fois le même nom (interface, namespace) et TypeScript les fusionne automatiquement.

**Q6 : Comment étendre les types Express ?**
> Avec module augmentation :
```typescript
declare global {
  namespace Express {
    interface Request {
      user?: User;
    }
  }
}
```

**Q7 : Import circulaire, comment l'éviter ?**
> Extraire les interfaces dans un fichier séparé, ou restructurer le code pour éviter les dépendances circulaires.

**Q8 : Type-only import, pourquoi l'utiliser ?**
> `import type` n'importe que les types (pas le code). Réduit la taille du bundle et évite les imports circulaires.

**Q9 : Quelle configuration pour Node.js avec ESM ?**
> `"type": "module"` dans package.json et `"module": "ESNext"` dans tsconfig.json.

**Q10 : Comment tester du code modulaire ?**
> Avec Jest ou Vitest, en configurant pour supporter TypeScript et les imports ES Modules.

---

## 🎯 Patterns Importants

### Pattern 1 : Barrel Exports

```typescript
// models/index.ts
export * from "./user";
export * from "./product";
export * from "./order";

// app.ts
import { User, Product, Order } from "./models";
```

### Pattern 2 : Service Layer

```typescript
// Structure
src/
├── models/
├── services/
├── routes/
└── index.ts

// Chaque couche importe la précédente
// routes → services → models
```

### Pattern 3 : Dependency Injection

```typescript
// services/user.service.ts
export class UserService {
  constructor(private db: Database) {}
}

// app.ts
const db = new Database();
const userService = new UserService(db);
```

### Pattern 4 : Module Augmentation

```typescript
// types/express.d.ts
declare global {
  namespace Express {
    interface Request {
      user?: User;
    }
  }
}

export {};
```

---

## 🏆 Projet Final Suggéré

Créez une API REST complète avec :

### Structure
```
src/
├── models/
│   ├── user.model.ts
│   ├── product.model.ts
│   └── index.ts
├── services/
│   ├── user.service.ts
│   ├── product.service.ts
│   └── index.ts
├── routes/
│   ├── user.routes.ts
│   ├── product.routes.ts
│   └── index.ts
├── middleware/
│   ├── auth.ts
│   ├── validation.ts
│   └── index.ts
├── types/
│   ├── express.d.ts
│   └── global.d.ts
├── app.ts
└── index.ts
```

### Fonctionnalités
1. **CRUD Users** : Create, Read, Update, Delete
2. **CRUD Products** : avec stock management
3. **Authentication** : middleware avec req.user
4. **Validation** : valider les données
5. **Error handling** : gestion centralisée
6. **Tests** : Jest avec couverture
7. **Types** : tout typé, aucun any

---

## 🎓 Compétences Acquises

Après ce cours complet, vous maîtrisez :

### Niveau Fondamental ✅
- Types de base (primitives, arrays, tuples)
- Interfaces et type aliases
- Fonctions typées
- Classes et héritage

### Niveau Intermédiaire ✅
- Génériques et contraintes
- Union et intersection types
- Utility types (Partial, Pick, Omit, etc.)
- keyof, typeof, indexed access

### Niveau Avancé ✅
- Décorateurs (méthode, classe, propriété)
- Module system et organisation
- Ambient modules et types globaux
- Declaration merging
- Patterns type-safe complexes

### Applications Pratiques ✅
- API REST type-safe
- Système CRUD complet
- Authentification et permissions
- Validation de données
- Tests unitaires
- Configuration de projet

---

## 🌟 Conseils pour l'Entretien Final

### Préparez-vous à :

1. **Coder en direct**
   - Créer des types complexes
   - Implémenter des patterns
   - Résoudre des problèmes

2. **Expliquer clairement**
   - Pourquoi ce type ?
   - Quels sont les avantages ?
   - Quelles alternatives ?

3. **Justifier vos choix**
   - Interface vs type
   - Any vs unknown
   - Enum vs union type
   - Module organization

4. **Connaître l'écosystème**
   - Angular (décorateurs)
   - NestJS (modules, DI)
   - React (génériques)
   - Node.js (modules)

### Points Forts à Mentionner

- ✅ Maîtrise des types avancés
- ✅ Patterns de conception type-safe
- ✅ Organisation de projets
- ✅ Tests et qualité du code
- ✅ Performance et optimisation
- ✅ Bonnes pratiques

---

## 🎊 Félicitations !

Vous avez complété ce **cours complet TypeScript** !

### Récapitulatif des 6 Chapitres

1. **Chapitre 1** : Introduction et installation
2. **Chapitre 2** : Types de base (primitives, interfaces, génériques)
3. **Chapitre 3** : Types complexes (union, keyof, typeof, Record)
4. **Chapitre 4** : Extension et extraction de métadonnées
5. **Chapitre 5** : Décorateurs (méthode, classe, propriété)
6. **Chapitre 6** : Modules et organisation

### Total : ~200 pages de contenu !

Vous êtes maintenant prêt(e) pour :
- ✅ Entretiens techniques TypeScript
- ✅ Développement Angular
- ✅ Projets Node.js avec TypeScript
- ✅ APIs type-safe
- ✅ Applications professionnelles

---

## 🚀 Prochaines Étapes

1. **Pratiquer** : créer des projets personnels
2. **Contribuer** : open source TypeScript
3. **Approfondir** : patterns avancés
4. **Enseigner** : partager vos connaissances

---

**Bonne chance pour votre entretien ! 🍀**

**Fin du Cours TypeScript Complet**