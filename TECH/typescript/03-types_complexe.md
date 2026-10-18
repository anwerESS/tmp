# Chapitre 3 : Définir des Types Complexes

## 📑 Table des matières
- [3.1 Combiner plusieurs types avec Union Types](#31-combiner-plusieurs-types-avec-union-types)
- [3.2 Opérateur keyof](#32-opérateur-keyof)
- [3.3 Opérateur typeof](#33-opérateur-typeof)
- [3.4 Indexed Access Types](#34-indexed-access-types)
- [3.5 Types dynamiques limités avec Record](#35-types-dynamiques-limités-avec-record)
- [3.6 Gestion des ressources avec using](#36-gestion-des-ressources-avec-using)

---

## 3.1 Combiner plusieurs types avec Union Types

### 🎯 Introduction aux Union Types

Un **union type** permet à une valeur d'être de **plusieurs types différents**. C'est l'un des outils les plus puissants de TypeScript.

#### Syntaxe de base

```typescript
// Union simple de primitives
type ID = string | number;

let userId: ID;
userId = "abc123";  // ✅ Valide
userId = 456;       // ✅ Valide
userId = true;      // ❌ Erreur

// Union de types littéraux
type Direction = "north" | "south" | "east" | "west";

let heading: Direction = "north";  // ✅ OK
heading = "northeast";             // ❌ Erreur
```

### 🔍 Type Narrowing (Rétrécissement de type)

TypeScript utilise l'analyse de flux pour **rétrécir** le type dans certains contextes.

#### Avec typeof

```typescript
function processValue(value: string | number): string {
  if (typeof value === "string") {
    // Dans ce bloc, TypeScript sait que value est string
    return value.toUpperCase();
  } else {
    // Dans ce bloc, TypeScript sait que value est number
    return value.toFixed(2);
  }
}

console.log(processValue("hello"));  // "HELLO"
console.log(processValue(42.567));   // "42.57"
```

#### Avec l'opérateur in

```typescript
interface Circle {
  kind: "circle";
  radius: number;
}

interface Square {
  kind: "square";
  sideLength: number;
}

type Shape = Circle | Square;

function getArea(shape: Shape): number {
  if ("radius" in shape) {
    // TypeScript sait que shape est Circle
    return Math.PI * shape.radius ** 2;
  } else {
    // TypeScript sait que shape est Square
    return shape.sideLength ** 2;
  }
}
```

#### Avec instanceof

```typescript
class Dog {
  bark() {
    console.log("Woof!");
  }
}

class Cat {
  meow() {
    console.log("Meow!");
  }
}

function makeSound(animal: Dog | Cat): void {
  if (animal instanceof Dog) {
    animal.bark();  // TypeScript sait que c'est Dog
  } else {
    animal.meow();  // TypeScript sait que c'est Cat
  }
}
```

### 🏷️ Discriminated Unions (Unions Discriminées)

Pattern très important où chaque type a une **propriété commune** qui sert de discriminant.

```typescript
interface LoadingState {
  status: "loading";
}

interface SuccessState {
  status: "success";
  data: string;
}

interface ErrorState {
  status: "error";
  error: string;
}

type RequestState = LoadingState | SuccessState | ErrorState;

function handleRequest(state: RequestState): string {
  switch (state.status) {
    case "loading":
      return "Chargement...";
    
    case "success":
      // TypeScript sait que state.data existe
      return `Données: ${state.data}`;
    
    case "error":
      // TypeScript sait que state.error existe
      return `Erreur: ${state.error}`;
    
    default:
      // Exhaustive check
      const _exhaustive: never = state;
      return _exhaustive;
  }
}

// Utilisation
const loading: RequestState = { status: "loading" };
const success: RequestState = { status: "success", data: "Hello" };
const error: RequestState = { status: "error", error: "Network error" };

console.log(handleRequest(loading));  // "Chargement..."
console.log(handleRequest(success));  // "Données: Hello"
console.log(handleRequest(error));    // "Erreur: Network error"
```

### 📝 Exemple Complet : Système de Paiement

```typescript
// Différents types de paiement
interface CreditCardPayment {
  type: "credit_card";
  cardNumber: string;
  expiryDate: string;
  cvv: string;
}

interface PayPalPayment {
  type: "paypal";
  email: string;
}

interface BankTransferPayment {
  type: "bank_transfer";
  iban: string;
  bic: string;
}

interface CryptoPayment {
  type: "crypto";
  walletAddress: string;
  currency: "BTC" | "ETH" | "USDT";
}

type Payment = 
  | CreditCardPayment 
  | PayPalPayment 
  | BankTransferPayment 
  | CryptoPayment;

// Service de traitement
class PaymentProcessor {
  process(payment: Payment, amount: number): void {
    console.log(`\n💳 Traitement de ${amount}€`);
    
    switch (payment.type) {
      case "credit_card":
        this.processCreditCard(payment, amount);
        break;
      
      case "paypal":
        this.processPayPal(payment, amount);
        break;
      
      case "bank_transfer":
        this.processBankTransfer(payment, amount);
        break;
      
      case "crypto":
        this.processCrypto(payment, amount);
        break;
      
      default:
        const exhaustive: never = payment;
        throw new Error(`Type de paiement inconnu: ${exhaustive}`);
    }
  }
  
  private processCreditCard(payment: CreditCardPayment, amount: number): void {
    console.log(`💳 Carte bancaire: **** **** **** ${payment.cardNumber.slice(-4)}`);
    console.log(`   Expire: ${payment.expiryDate}`);
    console.log(`✅ Paiement autorisé`);
  }
  
  private processPayPal(payment: PayPalPayment, amount: number): void {
    console.log(`🅿️ PayPal: ${payment.email}`);
    console.log(`✅ Paiement envoyé`);
  }
  
  private processBankTransfer(payment: BankTransferPayment, amount: number): void {
    console.log(`🏦 Virement bancaire`);
    console.log(`   IBAN: ${payment.iban}`);
    console.log(`   BIC: ${payment.bic}`);
    console.log(`⏳ En attente de virement`);
  }
  
  private processCrypto(payment: CryptoPayment, amount: number): void {
    console.log(`₿ Crypto: ${payment.currency}`);
    console.log(`   Wallet: ${payment.walletAddress}`);
    console.log(`✅ Transaction envoyée`);
  }
}

// Utilisation
const processor = new PaymentProcessor();

processor.process({
  type: "credit_card",
  cardNumber: "1234567890123456",
  expiryDate: "12/25",
  cvv: "123"
}, 99.99);

processor.process({
  type: "paypal",
  email: "user@example.com"
}, 49.99);

processor.process({
  type: "crypto",
  walletAddress: "0x742d35Cc6634C0532925a3b844Bc9e7595f0bEb",
  currency: "ETH"
}, 199.99);
```

---

## 3.2 Opérateur keyof

### 🎯 Introduction à keyof

L'opérateur **keyof** crée un type union de toutes les **clés** d'un type donné.

```typescript
interface Person {
  name: string;
  age: number;
  email: string;
}

// keyof Person = "name" | "age" | "email"
type PersonKeys = keyof Person;

let key: PersonKeys;
key = "name";   // ✅ OK
key = "age";    // ✅ OK
key = "email";  // ✅ OK
key = "phone";  // ❌ Erreur
```

### 🔑 Fonctions génériques avec keyof

```typescript
// Fonction pour obtenir une propriété en toute sécurité
function getProperty<T, K extends keyof T>(obj: T, key: K): T[K] {
  return obj[key];
}

const person = {
  name: "Alice",
  age: 30,
  email: "alice@example.com",
  isActive: true
};

// Inférence automatique des types
const name = getProperty(person, "name");       // Type: string
const age = getProperty(person, "age");         // Type: number
const email = getProperty(person, "email");     // Type: string
const active = getProperty(person, "isActive"); // Type: boolean

// ❌ Erreur de compilation
const invalid = getProperty(person, "salary");  // Erreur!
```

### 🛠️ Fonction de mise à jour type-safe

```typescript
function setProperty<T, K extends keyof T>(
  obj: T,
  key: K,
  value: T[K]
): void {
  obj[key] = value;
}

const user = {
  id: 1,
  name: "Bob",
  age: 25,
  verified: false
};

// ✅ Type-safe
setProperty(user, "name", "Robert");    // OK
setProperty(user, "age", 26);           // OK
setProperty(user, "verified", true);    // OK

// ❌ Erreurs détectées
setProperty(user, "age", "26");         // Erreur: string ≠ number
setProperty(user, "invalid", "value");  // Erreur: clé inexistante
```

### 📊 Mapped Types avec keyof

```typescript
// Créer un type avec toutes les propriétés optionnelles
type Optional<T> = {
  [K in keyof T]?: T[K];
};

interface Config {
  apiUrl: string;
  timeout: number;
  debug: boolean;
}

type OptionalConfig = Optional<Config>;
// {
//   apiUrl?: string;
//   timeout?: number;
//   debug?: boolean;
// }

// Créer un type avec toutes les propriétés readonly
type ReadOnly<T> = {
  readonly [K in keyof T]: T[K];
};

type ReadOnlyConfig = ReadOnly<Config>;
// {
//   readonly apiUrl: string;
//   readonly timeout: number;
//   readonly debug: boolean;
// }
```

### 📝 Exemple Pratique : Système de Validation

```typescript
interface UserForm {
  username: string;
  email: string;
  password: string;
  confirmPassword: string;
  age: number;
  termsAccepted: boolean;
}

// Type pour une règle de validation
type ValidationRule<T> = (value: T) => string | null;

// Type pour toutes les règles d'un formulaire
type ValidationRules<T> = {
  [K in keyof T]?: ValidationRule<T[K]>;
};

class Validator<T> {
  private rules: ValidationRules<T> = {};
  
  addRule<K extends keyof T>(
    field: K,
    rule: ValidationRule<T[K]>
  ): this {
    this.rules[field] = rule;
    return this;
  }
  
  validate(data: T): Partial<Record<keyof T, string>> {
    const errors: Partial<Record<keyof T, string>> = {};
    
    for (const field in this.rules) {
      const rule = this.rules[field];
      if (rule) {
        const value = data[field];
        const error = rule(value as any);
        if (error) {
          errors[field] = error;
        }
      }
    }
    
    return errors;
  }
}

// Utilisation
const userValidator = new Validator<UserForm>()
  .addRule("username", (value) => {
    if (value.length < 3) {
      return "Le nom d'utilisateur doit contenir au moins 3 caractères";
    }
    return null;
  })
  .addRule("email", (value) => {
    if (!value.includes("@")) {
      return "Email invalide";
    }
    return null;
  })
  .addRule("password", (value) => {
    if (value.length < 8) {
      return "Le mot de passe doit contenir au moins 8 caractères";
    }
    return null;
  })
  .addRule("age", (value) => {
    if (value < 18) {
      return "Vous devez avoir au moins 18 ans";
    }
    return null;
  })
  .addRule("termsAccepted", (value) => {
    if (!value) {
      return "Vous devez accepter les conditions";
    }
    return null;
  });

const formData: UserForm = {
  username: "ab",
  email: "invalid-email",
  password: "1234",
  confirmPassword: "1234",
  age: 16,
  termsAccepted: false
};

const errors = userValidator.validate(formData);
console.log(errors);
// {
//   username: "Le nom d'utilisateur doit contenir au moins 3 caractères",
//   email: "Email invalide",
//   password: "Le mot de passe doit contenir au moins 8 caractères",
//   age: "Vous devez avoir au moins 18 ans",
//   termsAccepted: "Vous devez accepter les conditions"
// }
```

---

## 3.3 Opérateur typeof

### 🎯 Introduction à typeof

L'opérateur **typeof** en TypeScript permet d'obtenir le **type** d'une valeur ou variable.

⚠️ **Différence importante** : `typeof` TypeScript (compile-time) ≠ `typeof` JavaScript (runtime)

```typescript
// typeof JavaScript (s'exécute)
const num = 42;
console.log(typeof num);  // Affiche: "number"

// typeof TypeScript (type)
type NumType = typeof num;  // Type: number
```

### 📦 Extraire le type d'un objet

```typescript
const config = {
  apiUrl: "https://api.example.com",
  timeout: 5000,
  retries: 3,
  features: {
    auth: true,
    cache: false
  }
};

// Extraire le type complet
type Config = typeof config;
// {
//   apiUrl: string;
//   timeout: number;
//   retries: number;
//   features: {
//     auth: boolean;
//     cache: boolean;
//   };
// }

// Utiliser le type extrait
const newConfig: Config = {
  apiUrl: "https://new-api.com",
  timeout: 3000,
  retries: 5,
  features: {
    auth: true,
    cache: true
  }
};
```

### 🔧 typeof avec as const

```typescript
const COLORS = {
  primary: "#007bff",
  secondary: "#6c757d",
  success: "#28a745",
  danger: "#dc3545"
} as const;

type Colors = typeof COLORS;
// {
//   readonly primary: "#007bff";
//   readonly secondary: "#6c757d";
//   readonly success: "#28a745";
//   readonly danger: "#dc3545";
// }

// Extraire le type des valeurs
type ColorValue = typeof COLORS[keyof typeof COLORS];
// "#007bff" | "#6c757d" | "#28a745" | "#dc3545"
```

### 🎨 typeof avec des fonctions

```typescript
function createUser(name: string, age: number, email: string) {
  return {
    id: Math.random().toString(36),
    name,
    age,
    email,
    createdAt: new Date(),
    isActive: true
  };
}

// Extraire le type de retour
type User = ReturnType<typeof createUser>;
// {
//   id: string;
//   name: string;
//   age: number;
//   email: string;
//   createdAt: Date;
//   isActive: boolean;
// }

// Extraire les types des paramètres
type CreateUserParams = Parameters<typeof createUser>;
// [name: string, age: number, email: string]

// Utilisation
const params: CreateUserParams = ["Alice", 30, "alice@example.com"];
const user = createUser(...params);
```

### 📝 Exemple Pratique : Configuration Multi-Environnements

```typescript
const environments = {
  development: {
    apiUrl: "http://localhost:3000",
    database: "dev_db",
    logLevel: "debug" as const,
    features: {
      analytics: false,
      debugTools: true
    }
  },
  staging: {
    apiUrl: "https://staging.example.com",
    database: "staging_db",
    logLevel: "info" as const,
    features: {
      analytics: true,
      debugTools: true
    }
  },
  production: {
    apiUrl: "https://api.example.com",
    database: "prod_db",
    logLevel: "error" as const,
    features: {
      analytics: true,
      debugTools: false
    }
  }
} as const;

// Extraire les types
type Environment = keyof typeof environments;
// "development" | "staging" | "production"

type EnvironmentConfig = typeof environments[Environment];
// Config pour n'importe quel environnement

type LogLevel = typeof environments[Environment]["logLevel"];
// "debug" | "info" | "error"

// Fonction type-safe
function loadConfig(env: Environment): EnvironmentConfig {
  return environments[env];
}

// Utilisation
const devConfig = loadConfig("development");
console.log(devConfig.apiUrl);      // "http://localhost:3000"
console.log(devConfig.logLevel);    // "debug"

const prodConfig = loadConfig("production");
console.log(prodConfig.apiUrl);     // "https://api.example.com"
console.log(prodConfig.logLevel);   // "error"

// ❌ Erreur
const invalid = loadConfig("testing");  // Erreur: "testing" n'existe pas
```

---

## 3.4 Indexed Access Types

### 🎯 Introduction aux Indexed Access Types

Les **indexed access types** permettent d'accéder au type d'une **propriété** d'un autre type.

```typescript
interface Product {
  id: string;
  name: string;
  price: number;
  stock: number;
  category: {
    id: string;
    name: string;
  };
}

// Accéder au type d'une propriété
type ProductName = Product["name"];  // string
type ProductPrice = Product["price"];  // number

// Accéder à un type imbriqué
type Category = Product["category"];
// { id: string; name: string; }

type CategoryName = Product["category"]["name"];  // string
```

### 🔢 Accès multiple

```typescript
// Accéder à plusieurs propriétés
type StringProps = Product["name" | "id"];  // string
type NumberProps = Product["price" | "stock"];  // number

// Accéder à toutes les propriétés
type AllProductValues = Product[keyof Product];
// string | number | { id: string; name: string; }
```

### 📋 Type d'un élément de tableau

```typescript
const products = [
  { id: "1", name: "Laptop", price: 999 },
  { id: "2", name: "Mouse", price: 25 },
  { id: "3", name: "Keyboard", price: 75 }
];

// Extraire le type d'un élément
type Product = typeof products[number];
// { id: string; name: string; price: number; }

// Extraire le type d'une propriété spécifique
type ProductId = typeof products[number]["id"];  // string
```

### 📝 Exemple Pratique : Event System Type-Safe

```typescript
// Définir les types d'événements
interface EventPayloads {
  userLogin: { userId: string; timestamp: Date };
  userLogout: { userId: string };
  productAdded: { productId: string; quantity: number };
  productRemoved: { productId: string };
  orderPlaced: {
    orderId: string;
    userId: string;
    total: number;
    items: Array<{ productId: string; quantity: number }>;
  };
  paymentProcessed: {
    orderId: string;
    amount: number;
    method: "card" | "paypal" | "crypto";
  };
}

// Type pour les noms d'événements
type EventName = keyof EventPayloads;

// Type pour le payload d'un événement spécifique
type EventPayload<T extends EventName> = EventPayloads[T];

// System d'événements type-safe
class EventEmitter {
  private listeners: {
    [E in EventName]?: Array<(payload: EventPayloads[E]) => void>;
  } = {};
  
  on<E extends EventName>(
    event: E,
    callback: (payload: EventPayloads[E]) => void
  ): void {
    if (!this.listeners[event]) {
      this.listeners[event] = [];
    }
    this.listeners[event]!.push(callback);
  }
  
  emit<E extends EventName>(
    event: E,
    payload: EventPayloads[E]
  ): void {
    const callbacks = this.listeners[event];
    if (callbacks) {
      callbacks.forEach(cb => cb(payload));
    }
  }
  
  off<E extends EventName>(
    event: E,
    callback: (payload: EventPayloads[E]) => void
  ): void {
    const callbacks = this.listeners[event];
    if (callbacks) {
      this.listeners[event] = callbacks.filter(cb => cb !== callback) as any;
    }
  }
}

// Utilisation type-safe
const emitter = new EventEmitter();

// ✅ Callbacks typés automatiquement
emitter.on("userLogin", (payload) => {
  // payload est typé comme { userId: string; timestamp: Date }
  console.log(`User ${payload.userId} logged in at ${payload.timestamp}`);
});

emitter.on("productAdded", (payload) => {
  // payload est typé comme { productId: string; quantity: number }
  console.log(`Added ${payload.quantity}x product ${payload.productId}`);
});

emitter.on("orderPlaced", (payload) => {
  // payload est typé comme l'interface complète de orderPlaced
  console.log(`Order ${payload.orderId} placed by ${payload.userId}`);
  console.log(`Total: ${payload.total}€`);
  console.log(`Items: ${payload.items.length}`);
});

// ✅ Emit avec validation automatique
emitter.emit("userLogin", {
  userId: "123",
  timestamp: new Date()
});

emitter.emit("productAdded", {
  productId: "456",
  quantity: 2
});

// ❌ Erreurs détectées
emitter.emit("userLogin", {
  userId: "123"
  // Erreur: 'timestamp' manquant
});

emitter.emit("productAdded", {
  productId: "456",
  quantity: "2"  // Erreur: string au lieu de number
});
```

---

## 3.5 Types dynamiques limités avec Record

### 🎯 Introduction à Record

Le type **Record<K, V>** crée un type d'objet avec des clés de type `K` et des valeurs de type `V`.

```typescript
// Dictionnaire simple
type StringDictionary = Record<string, string>;

const translations: StringDictionary = {
  hello: "bonjour",
  goodbye: "au revoir",
  thanks: "merci"
};

// Ajout dynamique
translations["welcome"] = "bienvenue";  // ✅ OK
translations["yes"] = 123;              // ❌ Erreur
```

### 🔑 Record avec clés limitées

```typescript
type Status = "idle" | "loading" | "success" | "error";

type StatusConfig = Record<Status, {
  color: string;
  icon: string;
  message: string;
}>;

const statusConfig: StatusConfig = {
  idle: {
    color: "gray",
    icon: "⏸️",
    message: "En attente"
  },
  loading: {
    color: "blue",
    icon: "⏳",
    message: "Chargement..."
  },
  success: {
    color: "green",
    icon: "✅",
    message: "Succès"
  },
  error: {
    color: "red",
    icon: "❌",
    message: "Erreur"
  }
};

// ❌ Erreur si une clé manque
const incomplete: StatusConfig = {
  idle: { color: "gray", icon: "⏸️", message: "En attente" }
  // Erreur: 'loading', 'success', 'error' manquants
};
```

### 📝 Exemple Pratique : Système de Permissions

```typescript
type Role = "admin" | "editor" | "viewer" | "guest";
type Action = "create" | "read" | "update" | "delete";

type Permissions = Record<Role, Record<Action, boolean>>;

const permissions: Permissions = {
  admin: {
    create: true,
    read: true,
    update: true,
    delete: true
  },
  editor: {
    create: true,
    read: true,
    update: true,
    delete: false
  },
  viewer: {
    create: false,
    read: true,
    update: false,
    delete: false
  },
  guest: {
    create: false,
    read: true,
    update: false,
    delete: false
  }
};

class PermissionChecker {
  constructor(private permissions: Permissions) {}
  
  can(role: Role, action: Action): boolean {
    return this.permissions[role][action];
  }
  
  getPermissions(role: Role): Record<Action, boolean> {
    return this.permissions[role];
  }
  
  getRolesByPermission(action: Action): Role[] {
    const roles: Role[] = [];
    
    for (const role in this.permissions) {
      if (this.permissions[role as Role][action]) {
        roles.push(role as Role);
      }
    }
    
    return roles;
  }
}

// Utilisation
const checker = new PermissionChecker(permissions);

console.log(checker.can("admin", "delete"));   // true
console.log(checker.can("editor", "delete"));  // false
console.log(checker.can("viewer", "read"));    // true

console.log(checker.getRolesByPermission("delete"));  // ["admin"]
console.log(checker.getRolesByPermission("read"));    // ["admin", "editor", "viewer", "guest"]
```

---

## 3.6 Gestion des ressources avec using

### 🎯 Introduction au mot-clé using

Le mot-clé **using** (TypeScript 5.2+) permet la gestion automatique des ressources avec le pattern **Disposable**.

⚠️ **Prérequis** : TypeScript 5.2+ et target ES2022+

```json
// tsconfig.json
{
  "compilerOptions": {
    "target": "ES2022",
    "lib": ["ES2022"]
  }
}
```

### 🔧 Interface Disposable

```typescript
// Implémentation de Disposable
class FileHandle implements Disposable {
  constructor(private filename: string) {
    console.log(`📂 Ouverture: ${filename}`);
  }
  
  read(): string {
    console.log(`📖 Lecture: ${this.filename}`);
    return "Contenu du fichier";
  }
  
  write(content: string): void {
    console.log(`✏️ Écriture: ${this.filename}`);
  }
  
  [Symbol.dispose](): void {
    console.log(`🔒 Fermeture: ${this.filename}`);
  }
}

// Utilisation
function processFile() {
  using file = new FileHandle("data.txt");
  
  const content = file.read();
  file.write("Nouveau contenu");
  
  // [Symbol.dispose]() appelé automatiquement
}

processFile();
// Output:
// 📂 Ouverture: data.txt
// 📖 Lecture: data.txt
// ✏️ Écriture: data.txt
// 🔒 Fermeture: data.txt
```

### 📝 Exemple Pratique : Connexion Base de Données

```typescript
class DatabaseConnection implements Disposable {
  private connected: boolean = false;
  
  constructor(private url: string) {
    this.connect();
  }
  
  private connect(): void {
    console.log(`🔌 Connexion à ${this.url}`);
    this.connected = true;
  }
  
  query(sql: string): any[] {
    if (!this.connected) {
      throw new Error("Non connecté");
    }
    console.log(`🔍 Requête: ${sql}`);
    return [];
  }
  
  [Symbol.dispose](): void {
    if (this.connected) {
      console.log("🔌 Déconnexion");
      this.connected = false;
    }
  }
}

// Utilisation
async function fetchUsers() {
  using db = new DatabaseConnection("postgresql://localhost/mydb");
  
  const users = db.query("SELECT * FROM users WHERE active = true");
  console.log(`Trouvé ${users.length} utilisateurs`);
  
  return users;
  
  // Déconnexion automatique garantie
}

fetchUsers();
// Output:
// 🔌 Connexion à postgresql://localhost/mydb
// 🔍 Requête: SELECT * FROM users WHERE active = true
// Trouvé 0 utilisateurs
// 🔌 Déconnexion
```

### 🔐 Exemple : Transaction de Base de Données

```typescript
class Transaction implements Disposable {
  private committed: boolean = false;
  private rolledBack: boolean = false;
  
  constructor(private connection: DatabaseConnection) {
    console.log("🔄 Début de transaction");
    this.connection.query("BEGIN");
  }
  
  execute(sql: string): void {
    if (this.rolledBack) {
      throw new Error("Transaction annulée");
    }
    this.connection.query(sql);
  }
  
  commit(): void {
    if (!this.rolledBack) {
      console.log("✅ Commit transaction");
      this.connection.query("COMMIT");
      this.committed = true;
    }
  }
  
  rollback(): void {
    if (!this.committed) {
      console.log("❌ Rollback transaction");
      this.connection.query("ROLLBACK");
      this.rolledBack = true;
    }
  }
  
  [Symbol.dispose](): void {
    // Rollback automatique si pas de commit
    if (!this.committed && !this.rolledBack) {
      this.rollback();
    }
  }
}

// Utilisation
async function transferMoney(fromId: string, toId: string, amount: number) {
  using db = new DatabaseConnection("postgresql://localhost/bank");
  using transaction = new Transaction(db);
  
  try {
    // Débiter le compte source
    transaction.execute(
      `UPDATE accounts SET balance = balance - ${amount} WHERE id = '${fromId}'`
    );
    
    // Vérifier le solde
    const balance = db.query(`SELECT balance FROM accounts WHERE id = '${fromId}'`);
    if (balance[0]?.balance < 0) {
      throw new Error("Solde insuffisant");
    }
    
    // Créditer le compte destination
    transaction.execute(
      `UPDATE accounts SET balance = balance + ${amount} WHERE id = '${toId}'`
    );
    
    // Tout s'est bien passé
    transaction.commit();
    console.log("💰 Transfert réussi");
    
  } catch (error) {
    console.error("❌ Erreur:", error);
    // Rollback automatique via dispose
    throw error;
  }
  
  // Nettoyage automatique garanti
}
```

### ⚡ AsyncDisposable pour opérations asynchrones

```typescript
class AsyncFileHandle implements AsyncDisposable {
  private handle: any = null;
  
  constructor(private filename: string) {
    console.log(`📂 Initialisation: ${filename}`);
  }
  
  async open(): Promise<void> {
    console.log(`🔓 Ouverture async: ${this.filename}`);
    // Simuler une opération async
    await new Promise(resolve => setTimeout(resolve, 100));
    this.handle = { /* handle fictif */ };
  }
  
  async read(): Promise<string> {
    if (!this.handle) {
      throw new Error("Fichier non ouvert");
    }
    console.log(`📖 Lecture async: ${this.filename}`);
    await new Promise(resolve => setTimeout(resolve, 50));
    return "Contenu du fichier";
  }
  
  async write(content: string): Promise<void> {
    if (!this.handle) {
      throw new Error("Fichier non ouvert");
    }
    console.log(`✏️ Écriture async: ${this.filename}`);
    await new Promise(resolve => setTimeout(resolve, 50));
  }
  
  async [Symbol.asyncDispose](): Promise<void> {
    if (this.handle) {
      console.log(`🔒 Fermeture async: ${this.filename}`);
      await new Promise(resolve => setTimeout(resolve, 50));
      this.handle = null;
    }
  }
}

// Utilisation avec await using
async function processFileAsync() {
  await using file = new AsyncFileHandle("data.txt");
  
  await file.open();
  const content = await file.read();
  await file.write("Nouveau contenu");
  
  console.log("Traitement terminé");
  
  // Dispose async automatique
}

processFileAsync();
```

### 🎯 Avantages de using

**Avant using (manuel) :**
```typescript
function oldWay() {
  const resource = new Resource();
  
  try {
    resource.doWork();
  } finally {
    // ⚠️ Il faut penser à nettoyer
    resource.cleanup();
  }
}
```

**Avec using (automatique) :**
```typescript
function newWay() {
  using resource = new Resource();
  
  resource.doWork();
  
  // ✅ Nettoyage automatique garanti
}
```

**Bénéfices :**
- ✅ Nettoyage automatique et garanti
- ✅ Fonctionne même en cas d'exception
- ✅ Code plus court et lisible
- ✅ Moins d'erreurs (pas d'oubli)
- ✅ Pattern familier (C#, Python)

---

## 🎯 Points Clés pour l'Entretien

### Union Types
1. **Combinaison de types** : `string | number | boolean`
2. **Type guards** : `typeof`, `in`, `instanceof`
3. **Discriminated unions** : propriété discriminante commune
4. **Exhaustive checking** : utiliser `never` dans default

### keyof
1. **Extraction de clés** : `keyof T` crée union de toutes les clés
2. **Fonctions génériques** : `<T, K extends keyof T>`
3. **Mapped types** : `{ [K in keyof T]: ... }`
4. **Type-safety** : garantit que les clés existent

### typeof
1. **Extraction de types** : obtenir le type d'une valeur
2. **Différent de JS** : compile-time vs runtime
3. **Avec as const** : types littéraux précis
4. **ReturnType/Parameters** : extraire types de fonctions

### Indexed Access Types
1. **Accès aux propriétés** : `T["prop"]`
2. **Accès multiple** : `T["prop1" | "prop2"]`
3. **Arrays** : `T[number]` pour type d'élément
4. **Imbriqué** : `T["a"]["b"]["c"]`

### Record
1. **Objets typés** : `Record<K, V>`
2. **Clés limitées** : union types pour K
3. **Dictionnaires** : alternative aux index signatures
4. **Configurations** : mappings type-safe

### Using
1. **Resource management** : nettoyage automatique
2. **Symbol.dispose** : méthode de cleanup
3. **AsyncDisposable** : pour opérations async
4. **Garanties** : cleanup même avec exceptions

---

## ⚠️ Pièges à Éviter

### Piège 1 : Oublier les type guards

```typescript
// ❌ Erreur
type Value = string | number;

function process(value: Value) {
  return value.toUpperCase();  // Erreur si number
}

// ✅ Correct
function process(value: Value) {
  if (typeof value === "string") {
    return value.toUpperCase();
  }
  return value.toFixed(2);
}
```

### Piège 2 : Confusion typeof TS vs JS

```typescript
const config = { port: 3000 };

// ❌ typeof JS (runtime)
if (typeof config.port === "number") { }

// ✅ typeof TS (compile-time)
type ConfigType = typeof config;
```

### Piège 3 : Record avec clés optionnelles

```typescript
type Status = "a" | "b" | "c";

// ❌ Record exige toutes les clés
const map: Record<Status, string> = {
  a: "A"
  // Erreur: 'b' et 'c' manquants
};

// ✅ Utiliser Partial si optionnel
const map: Partial<Record<Status, string>> = {
  a: "A"
};
```

### Piège 4 : Oublier Symbol.dispose

```typescript
// ❌ Sans using, risque d'oubli
class Resource {
  cleanup() { }
}

function use() {
  const r = new Resource();
  // Oups, oublié cleanup() !
}

// ✅ Avec using, garanti
class Resource implements Disposable {
  [Symbol.dispose]() { }
}

function use() {
  using r = new Resource();
  // Cleanup automatique
}
```

---

## 📝 Checklist de Révision

### Union Types
- [ ] Je comprends les unions de types
- [ ] Je sais utiliser les type guards
- [ ] Je maîtrise les discriminated unions
- [ ] Je connais l'exhaustive checking

### Opérateurs
- [ ] Je sais utiliser keyof
- [ ] Je comprends typeof (TS vs JS)
- [ ] Je peux les combiner

### Types Avancés
- [ ] Je maîtrise les indexed access types
- [ ] Je sais créer des Record types
- [ ] Je comprends quand les utiliser

### Resource Management
- [ ] Je connais le pattern Disposable
- [ ] Je sais implémenter Symbol.dispose
- [ ] Je comprends using et await using
- [ ] Je connais les avantages

---

## 💡 Exercices Pratiques

### Exercice 1 : État de Fetch
Créer un type pour représenter les états d'un fetch (idle, loading, success, error) avec discriminated unions.

### Exercice 2 : Form Validator
Implémenter un validateur de formulaire type-safe utilisant keyof et Record.

### Exercice 3 : Connection Pool
Créer un pool de connexions avec using pour gestion automatique des ressources.

---

## 🔗 Ressources Complémentaires

### Documentation Officielle
- **Union Types** : https://www.typescriptlang.org/docs/handbook/2/everyday-types.html#union-types
- **keyof** : https://www.typescriptlang.org/docs/handbook/2/keyof-types.html
- **typeof** : https://www.typescriptlang.org/docs/handbook/2/typeof-types.html
- **Indexed Access** : https://www.typescriptlang.org/docs/handbook/2/indexed-access-types.html
- **Using statement** : https://www.typescriptlang.org/docs/handbook/release-notes/typescript-5-2.html

### Articles et Tutoriels
- TypeScript Deep Dive
- Advanced TypeScript Patterns
- Effective TypeScript

---

## 📊 Résumé du Chapitre 3

Ce chapitre a couvert les **types complexes** essentiels en TypeScript :

✅ **Union Types** : Combiner plusieurs types avec type guards
✅ **keyof** : Extraire les clés d'un type
✅ **typeof** : Obtenir le type d'une valeur
✅ **Indexed Access** : Accéder aux types de propriétés
✅ **Record** : Créer des objets avec clés/valeurs typées
✅ **Using** : Gestion automatique des ressources

Ces concepts sont **essentiels** pour :
- Écrire du code type-safe
- Créer des APIs robustes
- Gérer les ressources efficacement
- Impressionner en entretien technique
