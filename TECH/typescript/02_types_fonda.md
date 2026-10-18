# Chapitre 2 : Utilisation Basique de TypeScript

## 📑 Table des matières
- [2.1 Types Primitifs et Built-in](#21-types-primitifs-et-built-in)
- [2.2 Création de Types Personnalisés avec Interfaces](#22-création-de-types-personnalisés-avec-interfaces)
- [2.3 Type Aliases](#23-définition-de-types-avec-type-aliases)
- [2.4 Types Énumérables (Enums)](#24-types-énumérables-enums)
- [2.5 Typage des Fonctions](#25-typage-des-fonctions)
- [2.6 Génériques (Generics)](#26-génériques-generics)
- [Points Clés pour l'Entretien](#-points-clés-pour-lentretien)

---

## 2.1 Types Primitifs et Built-in

### 🎯 Types Primitifs de Base

TypeScript reprend tous les types primitifs JavaScript et en ajoute quelques-uns.

#### 1. **number** - Nombres

```typescript
// Tous les nombres sont de type 'number' (pas de distinction int/float)
let age: number = 25;
let price: number = 19.99;
let hex: number = 0xf00d;        // Hexadécimal
let binary: number = 0b1010;     // Binaire
let octal: number = 0o744;       // Octal

// Nombres spéciaux
let infinity: number = Infinity;
let notANumber: number = NaN;

// ❌ Erreur de type
age = "25";  // Error: Type 'string' is not assignable to type 'number'
```

#### 2. **string** - Chaînes de caractères

```typescript
let firstName: string = "John";
let lastName: string = 'Doe';
let fullName: string = `${firstName} ${lastName}`;  // Template literals

// Multiligne
let message: string = `
  Bonjour ${firstName},
  Bienvenue sur notre site !
`;

// ❌ Erreur
firstName = 42;  // Error: Type 'number' is not assignable to type 'string'
```

#### 3. **boolean** - Booléens

```typescript
let isActive: boolean = true;
let isCompleted: boolean = false;

// Expressions booléennes
let isAdult: boolean = age >= 18;
let hasAccess: boolean = isActive && isAdult;

// ❌ Attention : valeurs truthy/falsy ne sont pas des booléens !
let wrong: boolean = 1;        // Error
let alsoWrong: boolean = "";   // Error

// ✅ Conversion explicite nécessaire
let correct: boolean = Boolean(1);     // true
let alsoCorrect: boolean = !!1;        // true
```

#### 4. **null** et **undefined**

```typescript
let notDefined: undefined = undefined;
let empty: null = null;

// Avec strictNullChecks: true (recommandé)
let name: string = "John";
name = null;       // ❌ Error
name = undefined;  // ❌ Error

// ✅ Union type pour permettre null/undefined
let nullableName: string | null = "John";
nullableName = null;  // ✅ OK

let optionalAge: number | undefined = 25;
optionalAge = undefined;  // ✅ OK
```

#### 5. **void** - Absence de valeur

```typescript
// Fonction qui ne retourne rien
function logMessage(message: string): void {
  console.log(message);
  // Pas de return, ou return sans valeur
}

// ❌ Erreur si on retourne une valeur
function wrongVoid(): void {
  return "something";  // Error
}

// ✅ void peut être null ou undefined (si strictNullChecks: false)
let unusable: void = undefined;
```

#### 6. **any** - Type universel (à éviter !)

```typescript
// any désactive la vérification de type
let anything: any = 42;
anything = "now a string";
anything = true;
anything = { key: "value" };
anything.nonExistentMethod();  // ⚠️ Pas d'erreur à la compilation !

// ⚠️ any se propage
function dangerousFunction(input: any) {
  return input.toUpperCase();  // Si input n'est pas string → erreur runtime
}

// ✅ Utiliser 'unknown' à la place (voir ci-dessous)
```

#### 7. **unknown** - Type sûr pour valeurs inconnues

```typescript
// unknown est comme any mais type-safe
let userInput: unknown = "hello";
userInput = 42;
userInput = true;

// ❌ Ne peut pas utiliser directement
let value1: string = userInput;  // Error
userInput.toUpperCase();          // Error

// ✅ Vérification de type nécessaire
if (typeof userInput === "string") {
  let value2: string = userInput;  // ✅ OK
  userInput.toUpperCase();          // ✅ OK
}

// Exemple pratique avec API
async function fetchData(url: string): Promise<unknown> {
  const response = await fetch(url);
  return response.json();  // Retour unknown, pas any !
}

const data = await fetchData("/api/users");
// ✅ Force la validation
if (typeof data === "object" && data !== null && "name" in data) {
  console.log(data.name);
}
```

#### 8. **never** - Type qui ne se produit jamais

```typescript
// Fonction qui ne termine jamais
function infiniteLoop(): never {
  while (true) {
    // Boucle infinie
  }
}

// Fonction qui lance toujours une erreur
function throwError(message: string): never {
  throw new Error(message);
}

// Utile dans les guards exhaustifs
type Shape = "circle" | "square" | "triangle";

function getArea(shape: Shape): number {
  switch (shape) {
    case "circle":
      return Math.PI * 10 * 10;
    case "square":
      return 10 * 10;
    case "triangle":
      return (10 * 10) / 2;
    default:
      // Si on ajoute un type à Shape et on oublie ici, erreur !
      const exhaustiveCheck: never = shape;
      throw new Error(`Unhandled shape: ${exhaustiveCheck}`);
  }
}
```

### 🔧 Types Built-in Avancés

#### 9. **Array** - Tableaux

```typescript
// Syntaxe 1 : Type[]
let numbers: number[] = [1, 2, 3, 4, 5];
let names: string[] = ["Alice", "Bob", "Charlie"];

// Syntaxe 2 : Array<Type>
let scores: Array<number> = [95, 87, 92];

// Tableaux multidimensionnels
let matrix: number[][] = [
  [1, 2, 3],
  [4, 5, 6],
  [7, 8, 9]
];

// Tableau mixte avec union type
let mixed: (string | number)[] = ["hello", 42, "world", 99];

// Tableau readonly
let readonlyArray: readonly number[] = [1, 2, 3];
readonlyArray.push(4);  // ❌ Error: push n'existe pas sur readonly array
```

#### 10. **Tuple** - Tableaux à taille fixe avec types spécifiques

```typescript
// Tuple : ordre et nombre d'éléments fixes
let user: [string, number] = ["John", 25];

// Accès par index
let name: string = user[0];   // "John"
let age: number = user[1];    // 25

// ❌ Erreurs
user = [25, "John"];              // Error: ordre inversé
user = ["John", 25, "extra"];     // Error (si strict)

// Tuple avec éléments optionnels
let coordinate: [number, number, number?] = [10, 20];
coordinate = [10, 20, 30];  // ✅ OK aussi

// Tuple avec rest elements
let tuple: [string, ...number[]] = ["hello", 1, 2, 3, 4];

// Named tuples (TypeScript 4.0+)
let employee: [name: string, age: number, department: string] = [
  "Alice",
  30,
  "Engineering"
];

// Exemple pratique : retour de fonction
function getUser(): [string, number, boolean] {
  return ["John", 25, true];
}

const [userName, userAge, isActive] = getUser();
```

#### 11. **Object** - Type objet

```typescript
// Type object générique (peu utilisé)
let obj: object = { name: "John" };

// ✅ Mieux : définir la structure
let user: { name: string; age: number } = {
  name: "John",
  age: 25
};

// Propriétés optionnelles
let product: { name: string; price: number; description?: string } = {
  name: "Laptop",
  price: 999
};

// Propriétés readonly
let config: { readonly apiKey: string; timeout: number } = {
  apiKey: "abc123",
  timeout: 5000
};
config.apiKey = "new";  // ❌ Error: readonly

// Index signature pour objets dynamiques
let dictionary: { [key: string]: string } = {
  hello: "bonjour",
  goodbye: "au revoir"
};
dictionary["welcome"] = "bienvenue";  // ✅ OK
```

#### 12. **Function** - Type fonction

```typescript
// Type Function générique
let myFunc: Function = (x: number) => x * 2;

// ✅ Mieux : signature de fonction spécifique
let add: (a: number, b: number) => number;
add = (x, y) => x + y;

// Fonction callback
let callback: (message: string) => void;
callback = (msg) => console.log(msg);

// Exemple avec tableau
const numbers = [1, 2, 3, 4, 5];
const doubled = numbers.map((n: number): number => n * 2);
```

#### 13. **Date, RegExp, Error**

```typescript
// Date
let now: Date = new Date();
let birthday: Date = new Date("1995-06-15");

// RegExp
let emailPattern: RegExp = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
let isValid: boolean = emailPattern.test("user@example.com");

// Error
let error: Error = new Error("Something went wrong");
throw error;

// Custom Error
class ValidationError extends Error {
  constructor(message: string) {
    super(message);
    this.name = "ValidationError";
  }
}
```

---

## 2.2 Création de Types Personnalisés avec Interfaces

### 🎯 Qu'est-ce qu'une Interface ?

Une interface définit la **structure** d'un objet. C'est un contrat que les objets doivent respecter.

#### Interface de base

```typescript
// Définition d'une interface
interface User {
  id: number;
  name: string;
  email: string;
  age: number;
}

// Utilisation
const user: User = {
  id: 1,
  name: "John Doe",
  email: "john@example.com",
  age: 25
};

// ❌ Erreurs
const invalidUser: User = {
  id: 1,
  name: "Jane"
  // Error: 'email' et 'age' manquants
};

const wrongType: User = {
  id: "1",  // ❌ Error: string au lieu de number
  name: "John",
  email: "john@example.com",
  age: 25
};
```

#### Propriétés optionnelles

```typescript
interface Product {
  id: string;
  name: string;
  price: number;
  description?: string;      // Optionnel
  imageUrl?: string;         // Optionnel
}

// ✅ Valide sans propriétés optionnelles
const product1: Product = {
  id: "prod-001",
  name: "Laptop",
  price: 999
};

// ✅ Valide avec propriétés optionnelles
const product2: Product = {
  id: "prod-002",
  name: "Mouse",
  price: 25,
  description: "Wireless mouse",
  imageUrl: "/images/mouse.jpg"
};
```

#### Propriétés readonly

```typescript
interface Config {
  readonly apiUrl: string;
  readonly apiKey: string;
  timeout: number;
}

const config: Config = {
  apiUrl: "https://api.example.com",
  apiKey: "secret-key-123",
  timeout: 5000
};

config.timeout = 3000;       // ✅ OK
config.apiUrl = "new-url";   // ❌ Error: readonly
config.apiKey = "new-key";   // ❌ Error: readonly
```

#### Méthodes dans les interfaces

```typescript
interface Calculator {
  add(a: number, b: number): number;
  subtract(a: number, b: number): number;
  multiply(a: number, b: number): number;
  divide(a: number, b: number): number;
}

// Implémentation
const calculator: Calculator = {
  add: (a, b) => a + b,
  subtract: (a, b) => a - b,
  multiply: (a, b) => a * b,
  divide: (a, b) => {
    if (b === 0) throw new Error("Division by zero");
    return a / b;
  }
};

// Utilisation
console.log(calculator.add(5, 3));       // 8
console.log(calculator.multiply(4, 7));  // 28
```

#### Extension d'interfaces (héritage)

```typescript
// Interface de base
interface Person {
  name: string;
  age: number;
}

// Extension simple
interface Employee extends Person {
  employeeId: string;
  department: string;
  salary: number;
}

const employee: Employee = {
  name: "Alice",
  age: 30,
  employeeId: "EMP-001",
  department: "Engineering",
  salary: 75000
};

// Extension multiple
interface Timestamped {
  createdAt: Date;
  updatedAt: Date;
}

interface Identifiable {
  id: string;
}

interface Article extends Timestamped, Identifiable {
  title: string;
  content: string;
  author: string;
}

const article: Article = {
  id: "article-123",
  title: "Introduction to TypeScript",
  content: "TypeScript is...",
  author: "John Doe",
  createdAt: new Date(),
  updatedAt: new Date()
};
```

#### Index Signatures - Propriétés dynamiques

```typescript
// Dictionnaire avec clés string
interface StringDictionary {
  [key: string]: string;
}

const translations: StringDictionary = {
  hello: "bonjour",
  goodbye: "au revoir",
  welcome: "bienvenue"
};

// Ajout dynamique
translations["thanks"] = "merci";  // ✅ OK
translations["yes"] = 123;         // ❌ Error: must be string

// Dictionnaire avec clés number
interface NumberDictionary {
  [index: number]: string;
}

const usernames: NumberDictionary = {
  1: "admin",
  2: "user",
  3: "guest"
};

// Combinaison avec propriétés fixes
interface MixedDictionary {
  name: string;              // Propriété fixe
  [key: string]: string;     // Propriétés dynamiques
}

const data: MixedDictionary = {
  name: "Config",
  apiUrl: "https://api.example.com",
  version: "1.0.0"
};
```

#### Interface pour classes

```typescript
// Interface définissant un contrat
interface Animal {
  name: string;
  age: number;
  makeSound(): void;
  eat(food: string): void;
}

// Classe implémentant l'interface
class Dog implements Animal {
  name: string;
  age: number;
  breed: string;  // Propriété supplémentaire OK
  
  constructor(name: string, age: number, breed: string) {
    this.name = name;
    this.age = age;
    this.breed = breed;
  }
  
  makeSound(): void {
    console.log("Woof! Woof!");
  }
  
  eat(food: string): void {
    console.log(`${this.name} is eating ${food}`);
  }
  
  // Méthode supplémentaire OK
  fetch(): void {
    console.log(`${this.name} is fetching`);
  }
}

const dog = new Dog("Rex", 3, "Labrador");
dog.makeSound();  // "Woof! Woof!"
dog.eat("bone");  // "Rex is eating bone"
```

### 📝 Exemple Pratique Complet : Système de Blog

```typescript
// Types de base
interface Identifiable {
  id: string;
}

interface Timestamped {
  createdAt: Date;
  updatedAt: Date;
}

// Interface utilisateur
interface User extends Identifiable, Timestamped {
  username: string;
  email: string;
  role: "admin" | "editor" | "viewer";
  profile?: {
    bio: string;
    avatar: string;
  };
}

// Interface article
interface Post extends Identifiable, Timestamped {
  title: string;
  content: string;
  author: User;
  tags: string[];
  published: boolean;
  likes: number;
  comments: Comment[];
}

// Interface commentaire
interface Comment extends Identifiable, Timestamped {
  author: User;
  content: string;
  postId: string;
}

// Interface pour service de blog
interface BlogService {
  createPost(post: Omit<Post, "id" | "createdAt" | "updatedAt">): Post;
  getPost(id: string): Post | null;
  updatePost(id: string, updates: Partial<Post>): Post;
  deletePost(id: string): boolean;
  getAllPosts(): Post[];
  getPostsByAuthor(authorId: string): Post[];
}

// Implémentation
class SimpleBlogService implements BlogService {
  private posts: Map<string, Post> = new Map();
  
  createPost(postData: Omit<Post, "id" | "createdAt" | "updatedAt">): Post {
    const post: Post = {
      ...postData,
      id: `post-${Date.now()}`,
      createdAt: new Date(),
      updatedAt: new Date()
    };
    
    this.posts.set(post.id, post);
    return post;
  }
  
  getPost(id: string): Post | null {
    return this.posts.get(id) || null;
  }
  
  updatePost(id: string, updates: Partial<Post>): Post {
    const post = this.posts.get(id);
    if (!post) {
      throw new Error(`Post ${id} not found`);
    }
    
    const updatedPost: Post = {
      ...post,
      ...updates,
      updatedAt: new Date()
    };
    
    this.posts.set(id, updatedPost);
    return updatedPost;
  }
  
  deletePost(id: string): boolean {
    return this.posts.delete(id);
  }
  
  getAllPosts(): Post[] {
    return Array.from(this.posts.values());
  }
  
  getPostsByAuthor(authorId: string): Post[] {
    return this.getAllPosts().filter(post => post.author.id === authorId);
  }
}

// Utilisation
const blogService = new SimpleBlogService();

const author: User = {
  id: "user-1",
  username: "johndoe",
  email: "john@example.com",
  role: "editor",
  createdAt: new Date(),
  updatedAt: new Date()
};

const newPost = blogService.createPost({
  title: "Getting Started with TypeScript",
  content: "TypeScript is a powerful...",
  author: author,
  tags: ["typescript", "programming"],
  published: true,
  likes: 0,
  comments: []
});

console.log(newPost);
```

---

## 2.3 Définition de Types avec Type Aliases

### 🎯 Qu'est-ce qu'un Type Alias ?

Un **type alias** crée un nouveau nom pour un type. Similaire aux interfaces mais plus flexible.

#### Syntaxe de base

```typescript
// Type alias simple
type ID = string | number;
type Age = number;
type Name = string;

let userId: ID = "user-123";
userId = 456;  // ✅ OK

let userAge: Age = 25;
let userName: Name = "John";
```

#### Type Alias pour objets

```typescript
// Similaire à une interface
type User = {
  id: number;
  name: string;
  email: string;
};

const user: User = {
  id: 1,
  name: "John",
  email: "john@example.com"
};

// Avec propriétés optionnelles et readonly
type Product = {
  readonly id: string;
  name: string;
  price: number;
  description?: string;
};
```

#### Union Types

```typescript
// Type alias pour union
type Status = "pending" | "approved" | "rejected";
type Result = "success" | "error";

function handleStatus(status: Status): void {
  switch (status) {
    case "pending":
      console.log("En attente...");
      break;
    case "approved":
      console.log("Approuvé!");
      break;
    case "rejected":
      console.log("Rejeté.");
      break;
  }
}

// Union de types complexes
type StringOrNumber = string | number;
type User = { name: string; age: number };
type Admin = { name: string; permissions: string[] };
type Person = User | Admin;

function greet(person: Person): string {
  return `Hello, ${person.name}!`;
}
```

#### Intersection Types

```typescript
// Combinaison de plusieurs types
type Identifiable = {
  id: string;
};

type Timestamped = {
  createdAt: Date;
  updatedAt: Date;
};

type Nameable = {
  name: string;
};

// Intersection : doit avoir TOUTES les propriétés
type Entity = Identifiable & Timestamped & Nameable;

const entity: Entity = {
  id: "entity-1",
  name: "My Entity",
  createdAt: new Date(),
  updatedAt: new Date()
};
```

#### Type Alias pour fonctions

```typescript
// Signature de fonction
type MathOperation = (a: number, b: number) => number;

const add: MathOperation = (x, y) => x + y;
const subtract: MathOperation = (x, y) => x - y;

// Type pour callback
type Callback = (error: Error | null, data: any) => void;

function fetchData(url: string, callback: Callback): void {
  // Simulation d'un appel async
  setTimeout(() => {
    callback(null, { result: "data" });
  }, 1000);
}

// Type pour event handler
type EventHandler = (event: Event) => void;
type ClickHandler = (event: MouseEvent) => void;
```

### 🔄 Interface vs Type Alias

```typescript
// Les deux peuvent définir des objets
interface UserInterface {
  name: string;
  age: number;
}

type UserType = {
  name: string;
  age: number;
};

// ✅ Interface : peut être étendue
interface Animal {
  name: string;
}

interface Dog extends Animal {
  breed: string;
}

// ✅ Type : peut utiliser unions et intersections
type Pet = Dog | Cat;
type WildAnimal = Animal & { habitat: string };

// ✅ Interface : peut être ré-déclarée (declaration merging)
interface Window {
  title: string;
}

interface Window {
  color: string;
}

// Window a maintenant title ET color

// ❌ Type : ne peut PAS être ré-déclaré
type Config = { url: string };
type Config = { timeout: number };  // Error: duplicate identifier
```

**Quand utiliser quoi ?**

| Utiliser Interface | Utiliser Type Alias |
|-------------------|---------------------|
| Objets et classes | Unions et intersections |
| API publiques | Types primitifs aliasés |
| Besoin d'extension | Tuples |
| Declaration merging | Mapped types (avancé) |

### 📝 Exemple Pratique : API Response Types

```typescript
// Types de base
type HttpMethod = "GET" | "POST" | "PUT" | "DELETE" | "PATCH";
type HttpStatus = 200 | 201 | 400 | 401 | 403 | 404 | 500;

// Type pour réponse API générique
type ApiResponse<T> = {
  data: T;
  status: HttpStatus;
  message: string;
  timestamp: Date;
};

// Types spécifiques
type User = {
  id: string;
  username: string;
  email: string;
};

type UserListResponse = ApiResponse<User[]>;
type UserDetailResponse = ApiResponse<User>;

// Type pour erreur
type ApiError = {
  error: string;
  status: HttpStatus;
  details?: string[];
};

// Union type pour résultat
type ApiResult<T> = ApiResponse<T> | ApiError;

// Fonction avec types
async function fetchUsers(): Promise<UserListResponse> {
  const response = await fetch("/api/users");
  const data = await response.json();
  
  return {
    data: data.users,
    status: 200,
    message: "Users fetched successfully",
    timestamp: new Date()
  };
}

// Type guard
function isApiError(result: ApiResult<any>): result is ApiError {
  return "error" in result;
}

// Utilisation
const result = await fetchUsers();

if (isApiError(result)) {
  console.error(result.error);
} else {
  console.log(result.data);  // TypeScript sait que c'est User[]
}
```

---

## 2.4 Types Énumérables (Enums)

### 🎯 Qu'est-ce qu'un Enum ?

Un **enum** (énumération) définit un ensemble de constantes nommées.

#### Enum numérique

```typescript
// Enum de base (valeurs automatiques: 0, 1, 2...)
enum Direction {
  Up,      // 0
  Down,    // 1
  Left,    // 2
  Right    // 3
}

let playerDirection: Direction = Direction.Up;
console.log(playerDirection);  // 0

// Enum avec valeurs personnalisées
enum HttpStatusCode {
  OK = 200,
  Created = 201,
  BadRequest = 400,
  Unauthorized = 401,
  NotFound = 404,
  ServerError = 500
}

function handleResponse(status: HttpStatusCode): string {
  switch (status) {
    case HttpStatusCode.OK:
      return "Success!";
    case HttpStatusCode.NotFound:
      return "Resource not found";
    case HttpStatusCode.ServerError:
      return "Server error occurred";
    default:
      return "Unknown status";
  }
}

console.log(handleResponse(HttpStatusCode.OK));  // "Success!"
```

#### Enum string

```typescript
// Enum avec valeurs string
enum LogLevel {
  ERROR = "ERROR",
  WARN = "WARN",
  INFO = "INFO",
  DEBUG = "DEBUG"
}

function log(level: LogLevel, message: string): void {
  const timestamp = new Date().toISOString();
  console.log(`[${timestamp}] [${level}] ${message}`);
}

log(LogLevel.INFO, "Application started");
log(LogLevel.ERROR, "Failed to connect to database");

// Enum pour états d'application
enum AppState {
  LOADING = "LOADING",
  READY = "READY",
  ERROR = "ERROR",
  IDLE = "IDLE"
}

let currentState: AppState = AppState.LOADING;

// Utile pour comparaisons
if (currentState === AppState.LOADING) {
  console.log("Please wait...");
}
```

#### Enum hétérogène (mixte - rare)

```typescript
// Mélange number et string (peu recommandé)
enum Mixed {
  No = 0,
  Yes = "YES"
}
```

#### Const Enum (optimisé)

```typescript
// Const enum : inline à la compilation (plus performant)
const enum Color {
  Red = "RED",
  Green = "GREEN",
  Blue = "BLUE"
}

let color: Color = Color.Red;

// Après compilation JavaScript:
// let color = "RED";  // Valeur inline!
```

#### Reverse Mapping (enum numérique uniquement)

```typescript
enum Role {
  Admin,     // 0
  Editor,    // 1
  Viewer     // 2
}

// Forward mapping
console.log(Role.Admin);    // 0
console.log(Role.Editor);   // 1

// Reverse mapping (uniquement numeric enums)
console.log(Role[0]);  // "Admin"
console.log(Role[1]);  // "Editor"

// ❌ String enums n'ont PAS de reverse mapping
enum Status {
  Active = "ACTIVE",
  Inactive = "INACTIVE"
}
console.log(Status["ACTIVE"]);  // undefined
```

### 📝 Exemple Pratique : Gestion de Commandes

```typescript
// Enums pour états et types
enum OrderStatus {
  PENDING = "PENDING",
  CONFIRMED = "CONFIRMED",
  PROCESSING = "PROCESSING",
  SHIPPED = "SHIPPED",
  DELIVERED = "DELIVERED",
  CANCELLED = "CANCELLED"
}

enum PaymentMethod {
  CREDIT_CARD = "CREDIT_CARD",
  PAYPAL = "PAYPAL",
  BANK_TRANSFER = "BANK_TRANSFER",
  CASH_ON_DELIVERY = "CASH_ON_DELIVERY"
}

enum ShippingMethod {
  STANDARD = "STANDARD",
  EXPRESS = "EXPRESS",
  OVERNIGHT = "OVERNIGHT"
}

// Types utilisant les enums
type Order = {
  id: string;
  status: OrderStatus;
  paymentMethod: PaymentMethod;
  shippingMethod: ShippingMethod;
  total: number;
  createdAt: Date;
};

// Service avec logique métier
class OrderService {
  private orders: Map<string, Order> = new Map();
  
  createOrder(
    paymentMethod: PaymentMethod,
    shippingMethod: ShippingMethod,
    total: number
  ): Order {
    const order: Order = {
      id: `order-${Date.now()}`,
      status: OrderStatus.PENDING,
      paymentMethod,
      shippingMethod,
      total,
      createdAt: new Date()
    };
    
    this.orders.set(order.id, order);
    return order;
  }
  
  updateOrderStatus(orderId: string, newStatus: OrderStatus): void {
    const order = this.orders.get(orderId);
    if (!order) {
      throw new Error(`Order ${orderId} not found`);
    }
    
    // Validation des transitions d'état
    const validTransitions: Record<OrderStatus, OrderStatus[]> = {
      [OrderStatus.PENDING]: [OrderStatus.CONFIRMED, OrderStatus.CANCELLED],
      [OrderStatus.CONFIRMED]: [OrderStatus.PROCESSING, OrderStatus.CANCELLED],
      [OrderStatus.PROCESSING]: [OrderStatus.SHIPPED, OrderStatus.CANCELLED],
      [OrderStatus.SHIPPED]: [OrderStatus.DELIVERED],
      [OrderStatus.DELIVERED]: [],
      [OrderStatus.CANCELLED]: []
    };
    
    const allowedStatuses = validTransitions[order.status];
    if (!allowedStatuses.includes(newStatus)) {
      throw new Error(
        `Cannot transition from ${order.status} to ${newStatus}`
      );
    }
    
    order.status = newStatus;
    console.log(`✅ Order ${orderId} updated to ${newStatus}`);
  }
  
  getOrdersByStatus(status: OrderStatus): Order[] {
    return Array.from(this.orders.values()).filter(
      order => order.status === status
    );
  }
  
  getShippingCost(method: ShippingMethod): number {
    switch (method) {
      case ShippingMethod.STANDARD:
        return 5.99;
      case ShippingMethod.EXPRESS:
        return 15.99;
      case ShippingMethod.OVERNIGHT:
        return 29.99;
      default:
        const exhaustive: never = method;
        throw new Error(`Unknown shipping method: ${exhaustive}`);
    }
  }
}

// Utilisation
const orderService = new OrderService();

const order = orderService.createOrder(
  PaymentMethod.CREDIT_CARD,
  ShippingMethod.EXPRESS,
  149.99
);

console.log(`Order created: ${order.id}`);
console.log(`Status: ${order.status}`);
console.log(`Shipping cost: ${orderService.getShippingCost(order.shippingMethod)}`);

// Mise à jour du statut
orderService.updateOrderStatus(order.id, OrderStatus.CONFIRMED);
orderService.updateOrderStatus(order.id, OrderStatus.PROCESSING);
orderService.updateOrderStatus(order.id, OrderStatus.SHIPPED);
```

### 🆚 Enum vs Union Type

```typescript
// ❌ Enum : code JavaScript généré
enum Color {
  Red = "RED",
  Green = "GREEN",
  Blue = "BLUE"
}
// Génère du code JS supplémentaire

// ✅ Union Type : pas de code généré
type Color = "RED" | "GREEN" | "BLUE";
// Pas de code JS généré, juste vérification à la compilation

// Quand utiliser Enum :
// - Besoin de reverse mapping
// - Valeurs calculées
// - Logique métier complexe autour des valeurs

// Quand utiliser Union Type :
// - Types simples
// - Performance critique
// - Bibliothèques
```

---

## 2.5 Typage des Fonctions

### 🎯 Syntaxes de base

#### Fonctions nommées

```typescript
// Fonction avec types de paramètres et retour
function add(a: number, b: number): number {
  return a + b;
}

// Type de retour void
function logMessage(message: string): void {
  console.log(message);
}

// Fonction sans retour explicite (inféré)
function multiply(a: number, b: number) {
  return a * b;  // TypeScript infère : number
}
```

#### Fonctions fléchées

```typescript
// Fonction fléchée typée
const subtract = (a: number, b: number): number => {
  return a - b;
};

// Forme courte
const divide = (a: number, b: number): number => a / b;

// Avec type explicite complet
const modulo: (a: number, b: number) => number = (a, b) => a % b;
```

#### Paramètres optionnels

```typescript
// Paramètre optionnel avec ?
function greet(name: string, greeting?: string): string {
  if (greeting) {
    return `${greeting}, ${name}!`;
  }
  return `Hello, ${name}!`;
}

console.log(greet("John"));              // "Hello, John!"
console.log(greet("John", "Bonjour"));   // "Bonjour, John!"

// ⚠️ Paramètres optionnels doivent être à la fin
function wrong(optional?: string, required: string) {}  // ❌ Error
```

#### Paramètres par défaut

```typescript
// Valeur par défaut
function createUser(
  name: string,
  role: string = "user",
  isActive: boolean = true
): void {
  console.log(`User: ${name}, Role: ${role}, Active: ${isActive}`);
}

createUser("John");                    // role="user", isActive=true
createUser("Alice", "admin");          // isActive=true
createUser("Bob", "guest", false);     // Tous personnalisés

// Valeur par défaut complexe
function processData(
  data: string[],
  options: { caseSensitive: boolean; trim: boolean } = {
    caseSensitive: false,
    trim: true
  }
): string[] {
  let result = [...data];
  
  if (options.trim) {
    result = result.map(s => s.trim());
  }
  
  if (!options.caseSensitive) {
    result = result.map(s => s.toLowerCase());
  }
  
  return result;
}
```

#### Rest parameters

```typescript
// Rest parameters : nombre variable d'arguments
function sum(...numbers: number[]): number {
  return numbers.reduce((total, n) => total + n, 0);
}

console.log(sum(1, 2, 3));           // 6
console.log(sum(1, 2, 3, 4, 5));     // 15

// Avec d'autres paramètres
function logWithPrefix(prefix: string, ...messages: string[]): void {
  messages.forEach(msg => console.log(`${prefix}: ${msg}`));
}

logWithPrefix("INFO", "Server started", "Port 3000", "Ready");

// Type plus spécifique
function concatenate(separator: string, ...strings: string[]): string {
  return strings.join(separator);
}

console.log(concatenate(" ", "Hello", "World"));  // "Hello World"
console.log(concatenate("-", "2024", "01", "15")); // "2024-01-15"
```

### 🔄 Function Overloading

Les **surcharges de fonction** permettent différentes signatures pour la même fonction.

```typescript
// Déclarations de surcharge
function format(value: string): string;
function format(value: number): string;
function format(value: boolean): string;
function format(value: Date): string;

// Implémentation (doit couvrir tous les cas)
function format(value: string | number | boolean | Date): string {
  if (typeof value === "string") {
    return `"${value}"`;
  }
  if (typeof value === "number") {
    return value.toFixed(2);
  }
  if (typeof value === "boolean") {
    return value ? "YES" : "NO";
  }
  if (value instanceof Date) {
    return value.toISOString();
  }
  
  return String(value);
}

// Utilisation
console.log(format("hello"));      // '"hello"'
console.log(format(42));           // '42.00'
console.log(format(true));         // 'YES'
console.log(format(new Date()));   // '2024-01-15T10:30:00.000Z'

// Exemple plus complexe
function makeRequest(url: string): Promise<Response>;
function makeRequest(url: string, method: string): Promise<Response>;
function makeRequest(url: string, method: string, body: object): Promise<Response>;

function makeRequest(
  url: string,
  method: string = "GET",
  body?: object
): Promise<Response> {
  const options: RequestInit = { method };
  
  if (body) {
    options.body = JSON.stringify(body);
    options.headers = { "Content-Type": "application/json" };
  }
  
  return fetch(url, options);
}

// Appels valides
makeRequest("/api/users");
makeRequest("/api/users", "POST");
makeRequest("/api/users", "POST", { name: "John" });
```

### 📝 Type Aliases pour Fonctions

```typescript
// Type pour fonction
type BinaryOperation = (a: number, b: number) => number;

const add: BinaryOperation = (x, y) => x + y;
const multiply: BinaryOperation = (x, y) => x * y;

// Type pour callback
type Callback<T> = (error: Error | null, result: T | null) => void;

function fetchUser(id: string, callback: Callback<User>): void {
  // Simulation async
  setTimeout(() => {
    if (id === "error") {
      callback(new Error("User not found"), null);
    } else {
      callback(null, { id, name: "John", email: "john@example.com" });
    }
  }, 1000);
}

// Type pour event handler
type EventHandler<T = Event> = (event: T) => void;

const handleClick: EventHandler<MouseEvent> = (e) => {
  console.log(`Clicked at ${e.clientX}, ${e.clientY}`);
};

const handleKeyPress: EventHandler<KeyboardEvent> = (e) => {
  console.log(`Key pressed: ${e.key}`);
};

// Type pour fonction avec propriétés
type FunctionWithProperties = {
  (x: number): number;  // Signature d'appel
  description: string;   // Propriété
  version: number;       // Propriété
};

const double: FunctionWithProperties = (x: number) => x * 2;
double.description = "Doubles a number";
double.version = 1;

console.log(double(5));              // 10
console.log(double.description);     // "Doubles a number"
```

### 🎯 This dans les fonctions

```typescript
// Typage du this
interface User {
  name: string;
  greet(this: User): void;
}

const user: User = {
  name: "John",
  greet() {
    console.log(`Hello, I'm ${this.name}`);
  }
};

user.greet();  // ✅ "Hello, I'm John"

// ❌ Problème avec this détaché
const greetFn = user.greet;
greetFn();  // Error: The 'this' context is undefined

// ✅ Solution avec arrow function
const userWithArrow = {
  name: "Alice",
  greet: () => {
    console.log(`Hello, I'm ${this.name}`);  // 'this' lexical
  }
};
```

### 📝 Exemple Pratique Complet : Service de Validation

```typescript
// Types pour validateurs
type ValidationResult = {
  valid: boolean;
  errors: string[];
};

type Validator<T> = (value: T) => ValidationResult;

// Validateurs de base
const isNotEmpty: Validator<string> = (value) => {
  return {
    valid: value.length > 0,
    errors: value.length === 0 ? ["Value cannot be empty"] : []
  };
};

const isEmail: Validator<string> = (value) => {
  const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
  return {
    valid: emailRegex.test(value),
    errors: !emailRegex.test(value) ? ["Invalid email format"] : []
  };
};

const minLength = (min: number): Validator<string> => {
  return (value) => ({
    valid: value.length >= min,
    errors: value.length < min ? [`Minimum length is ${min}`] : []
  });
};

const maxLength = (max: number): Validator<string> => {
  return (value) => ({
    valid: value.length <= max,
    errors: value.length > max ? [`Maximum length is ${max}`] : []
  });
};

const isInRange = (min: number, max: number): Validator<number> => {
  return (value) => ({
    valid: value >= min && value <= max,
    errors: value < min || value > max 
      ? [`Value must be between ${min} and ${max}`] 
      : []
  });
};

// Fonction de composition
function compose<T>(...validators: Validator<T>[]): Validator<T> {
  return (value: T) => {
    const allErrors: string[] = [];
    let isValid = true;
    
    for (const validator of validators) {
      const result = validator(value);
      if (!result.valid) {
        isValid = false;
        allErrors.push(...result.errors);
      }
    }
    
    return {
      valid: isValid,
      errors: allErrors
    };
  };
}

// Validateurs composés
const validateUsername = compose<string>(
  isNotEmpty,
  minLength(3),
  maxLength(20)
);

const validateEmail = compose<string>(
  isNotEmpty,
  isEmail
);

const validateAge = compose<number>(
  isInRange(18, 100)
);

// Classe de validation de formulaire
class FormValidator {
  private validators: Map<string, Validator<any>> = new Map();
  
  addValidator<T>(field: string, validator: Validator<T>): this {
    this.validators.set(field, validator);
    return this;
  }
  
  validate(data: Record<string, any>): {
    valid: boolean;
    errors: Record<string, string[]>;
  } {
    const errors: Record<string, string[]> = {};
    let isValid = true;
    
    for (const [field, validator] of this.validators.entries()) {
      if (field in data) {
        const result = validator(data[field]);
        if (!result.valid) {
          isValid = false;
          errors[field] = result.errors;
        }
      }
    }
    
    return { valid: isValid, errors };
  }
}

// Utilisation
const formValidator = new FormValidator()
  .addValidator("username", validateUsername)
  .addValidator("email", validateEmail)
  .addValidator("age", validateAge);

const formData = {
  username: "jo",
  email: "invalid-email",
  age: 150
};

const result = formValidator.validate(formData);
console.log(result);
/*
{
  valid: false,
  errors: {
    username: ["Minimum length is 3"],
    email: ["Invalid email format"],
    age: ["Value must be between 18 and 100"]
  }
}
*/
```

---

## 2.6 Génériques (Generics)

### 🎯 Qu'est-ce que les Génériques ?

Les **génériques** permettent de créer des composants réutilisables qui fonctionnent avec **plusieurs types** plutôt qu'un seul.

#### Fonction générique de base

```typescript
// ❌ Sans générique : duplication de code
function getFirstNumber(arr: number[]): number | undefined {
  return arr[0];
}

function getFirstString(arr: string[]): string | undefined {
  return arr[0];
}

// ✅ Avec générique : code réutilisable
function getFirst<T>(arr: T[]): T | undefined {
  return arr[0];
}

// Utilisation avec inférence de type
const firstNum = getFirst([1, 2, 3]);        // Type: number | undefined
const firstStr = getFirst(["a", "b", "c"]); // Type: string | undefined

// Utilisation avec type explicite
const firstBool = getFirst<boolean>([true, false, true]);
```

#### Fonction générique avec contraintes

```typescript
// Contrainte : T doit avoir une propriété length
interface HasLength {
  length: number;
}

function getLength<T extends HasLength>(item: T): number {
  return item.length;
}

getLength("hello");           // ✅ 5
getLength([1, 2, 3]);         // ✅ 3
getLength({ length: 10 });    // ✅ 10
getLength(42);                // ❌ Error: number n'a pas 'length'

// Contrainte avec keyof
function getProperty<T, K extends keyof T>(obj: T, key: K): T[K] {
  return obj[key];
}

const user = { name: "John", age: 25, email: "john@example.com" };

const name = getProperty(user, "name");    // Type: string
const age = getProperty(user, "age");      // Type: number
getProperty(user, "invalid");              // ❌ Error
```

#### Interface générique

```typescript
// Interface générique pour une réponse API
interface ApiResponse<T> {
  data: T;
  status: number;
  message: string;
}

// Utilisation
const userResponse: ApiResponse<User> = {
  data: { id: "1", name: "John", email: "john@example.com" },
  status: 200,
  message: "Success"
};

const usersResponse: ApiResponse<User[]> = {
  data: [
    { id: "1", name: "John", email: "john@example.com" },
    { id: "2", name: "Jane", email: "jane@example.com" }
  ],
  status: 200,
  message: "Users retrieved"
};

// Interface générique pour un état
interface State<T> {
  data: T | null;
  loading: boolean;
  error: string | null;
}

const userState: State<User> = {
  data: null,
  loading: true,
  error: null
};
```

#### Classe générique

```typescript
// Classe générique simple
class Box<T> {
  private content: T;
  
  constructor(content: T) {
    this.content = content;
  }
  
  getContent(): T {
    return this.content;
  }
  
  setContent(content: T): void {
    this.content = content;
  }
}

const numberBox = new Box<number>(42);
console.log(numberBox.getContent());  // 42

const stringBox = new Box<string>("hello");
console.log(stringBox.getContent());  // "hello"

// Classe générique plus complexe
class DataStore<T extends { id: string }> {
  private items: Map<string, T> = new Map();
  
  add(item: T): void {
    this.items.set(item.id, item);
  }
  
  get(id: string): T | undefined {
    return this.items.get(id);
  }
  
  getAll(): T[] {
    return Array.from(this.items.values());
  }
  
  update(id: string, updates: Partial<T>): T | undefined {
    const item = this.items.get(id);
    if (item) {
      const updated = { ...item, ...updates };
      this.items.set(id, updated);
      return updated;
    }
    return undefined;
  }
  
  delete(id: string): boolean {
    return this.items.delete(id);
  }
}

// Utilisation
interface Product {
  id: string;
  name: string;
  price: number;
}

const productStore = new DataStore<Product>();
productStore.add({ id: "1", name: "Laptop", price: 999 });
productStore.add({ id: "2", name: "Mouse", price: 25 });

console.log(productStore.get("1"));     // { id: "1", name: "Laptop", price: 999 }
console.log(productStore.getAll());     // Array avec tous les produits
```

#### Génériques avec plusieurs paramètres

```typescript
// Fonction avec deux paramètres génériques
function pair<T, U>(first: T, second: U): [T, U] {
  return [first, second];
}

const p1 = pair("name", 25);           // [string, number]
const p2 = pair(true, "active");       // [boolean, string]

// Interface avec plusieurs génériques
interface Result<T, E> {
  success: boolean;
  data?: T;
  error?: E;
}

// Type success
const success: Result<User, string> = {
  success: true,
  data: { id: "1", name: "John", email: "john@example.com" }
};

// Type error
const error: Result<User, string> = {
  success: false,
  error: "User not found"
};

// Map personnalisée
class Dictionary<K extends string | number, V> {
  private items: Map<K, V> = new Map();
  
  set(key: K, value: V): void {
    this.items.set(key, value);
  }
  
  get(key: K): V | undefined {
    return this.items.get(key);
  }
  
  has(key: K): boolean {
    return this.items.has(key);
  }
}

const stringDict = new Dictionary<string, number>();
stringDict.set("age", 25);
stringDict.set("score", 100);
```

#### Génériques par défaut

```typescript
// Type générique avec valeur par défaut
interface Container<T = string> {
  value: T;
}

const stringContainer: Container = { value: "hello" };  // T = string par défaut
const numberContainer: Container<number> = { value: 42 };

// Fonction avec générique par défaut
function createArray<T = number>(length: number, value: T): T[] {
  return Array(length).fill(value);
}

const nums = createArray(3, 0);          // number[] par défaut
const strs = createArray<string>(3, ""); // string[] explicite
```

### 📝 Exemple Pratique Complet : Repository Pattern

```typescript
// Interface de base pour entités
interface Entity {
  id: string;
  createdAt: Date;
  updatedAt: Date;
}

// Type pour création (sans id, dates)
type CreateEntity<T extends Entity> = Omit<T, "id" | "createdAt" | "updatedAt">;

// Type pour mise à jour (tout optionnel sauf id)
type UpdateEntity<T extends Entity> = Partial<Omit<T, "id">> & { id: string };

// Repository générique
class Repository<T extends Entity> {
  private items: Map<string, T> = new Map();
  private idCounter = 1;
  
  // Créer une entité
  create(data: CreateEntity<T>): T {
    const entity = {
      ...data,
      id: `${this.idCounter++}`,
      createdAt: new Date(),
      updatedAt: new Date()
    } as T;
    
    this.items.set(entity.id, entity);
    return entity;
  }
  
  // Trouver par ID
  findById(id: string): T | undefined {
    return this.items.get(id);
  }
  
  // Trouver tous
  findAll(): T[] {
    return Array.from(this.items.values());
  }
  
  // Trouver avec condition
  findWhere(predicate: (item: T) => boolean): T[] {
    return this.findAll().filter(predicate);
  }
  
  // Mettre à jour
  update(id: string, updates: Partial<T>): T | undefined {
    const item = this.items.get(id);
    if (!item) return undefined;
    
    const updated = {
      ...item,
      ...updates,
      id,  // Garder l'ID original
      createdAt: item.createdAt,  // Garder createdAt
      updatedAt: new Date()  // Mettre à jour updatedAt
    } as T;
    
    this.items.set(id, updated);
    return updated;
  }
  
  // Supprimer
  delete(id: string): boolean {
    return this.items.delete(id);
  }
  
  // Compter
  count(): number {
    return this.items.size;
  }
  
  // Exister
  exists(id: string): boolean {
    return this.items.has(id);
  }
}

// Entités spécifiques
interface User extends Entity {
  username: string;
  email: string;
  role: "admin" | "user";
}

interface Post extends Entity {
  title: string;
  content: string;
  authorId: string;
  published: boolean;
}

interface Comment extends Entity {
  postId: string;
  authorId: string;
  content: string;
}

// Repositories typés
const userRepo = new Repository<User>();
const postRepo = new Repository<Post>();
const commentRepo = new Repository<Comment>();

// Utilisation
const user = userRepo.create({
  username: "johndoe",
  email: "john@example.com",
  role: "user"
});

const post = postRepo.create({
  title: "Introduction to TypeScript",
  content: "TypeScript is amazing...",
  authorId: user.id,
  published: true
});

const comment = commentRepo.create({
  postId: post.id,
  authorId: user.id,
  content: "Great article!"
});

// Requêtes
const allUsers = userRepo.findAll();
const adminUsers = userRepo.findWhere(u => u.role === "admin");
const userPosts = postRepo.findWhere(p => p.authorId === user.id);
const publishedPosts = postRepo.findWhere(p => p.published);

// Mise à jour
userRepo.update(user.id, { role: "admin" });
postRepo.update(post.id, { published: false });

console.log(`Total users: ${userRepo.count()}`);
console.log(`Total posts: ${postRepo.count()}`);
console.log(`Total comments: ${commentRepo.count()}`);
```

### 🎯 Génériques Utilitaires Built-in

TypeScript fournit des types génériques utilitaires :

```typescript
interface User {
  id: string;
  name: string;
  email: string;
  age: number;
}

// Partial<T> : toutes propriétés optionnelles
type PartialUser = Partial<User>;
// { id?: string; name?: string; email?: string; age?: number; }

// Required<T> : toutes propriétés requises
type RequiredUser = Required<Partial<User>>;

// Readonly<T> : toutes propriétés readonly
type ReadonlyUser = Readonly<User>;

// Pick<T, K> : sélectionner certaines propriétés
type UserBasicInfo = Pick<User, "id" | "name">;
// { id: string; name: string; }

// Omit<T, K> : exclure certaines propriétés
type UserWithoutId = Omit<User, "id">;
// { name: string; email: string; age: number; }

// Record<K, T> : objet avec clés K et valeurs T
type UserRoles = Record<string, "admin" | "user" | "guest">;
// { [key: string]: "admin" | "user" | "guest" }

// Exclude<T, U> : exclure de union
type Role = "admin" | "user" | "guest";
type NonAdminRole = Exclude<Role, "admin">;
// "user" | "guest"

// Extract<T, U> : extraire de union
type AdminRole = Extract<Role, "admin">;
// "admin"

// NonNullable<T> : exclure null et undefined
type MaybeString = string | null | undefined;
type DefinitelyString = NonNullable<MaybeString>;
// string

// ReturnType<T> : type de retour de fonction
function getUser() {
  return { id: "1", name: "John" };
}
type UserReturnType = ReturnType<typeof getUser>;
// { id: string; name: string; }

// Parameters<T> : types des paramètres
function createUser(name: string, age: number) {
  return { name, age };
}
type CreateUserParams = Parameters<typeof createUser>;
// [name: string, age: number]
```

---

## 🎯 Points Clés du Chapitre 2 pour l'Entretien

### Concepts Fondamentaux

1. **Types Primitifs**
   - `number`, `string`, `boolean`, `null`, `undefined`
   - `any` (éviter), `unknown` (préférer), `never`, `void`
   - Différence entre `any` et `unknown`

2. **Interfaces**
   - Définissent la structure d'objets
   - Peuvent être étendues (`extends`)
   - Declaration merging possible
   - Propriétés optionnelles (`?`) et readonly

3. **Type Aliases**
   - Plus flexibles que les interfaces
   - Supportent unions, intersections, primitives
   - Ne peuvent pas être ré-déclarés
   - Utilisés pour types complexes

4. **Enums**
   - Ensemble de constantes nommées
   - Enums numériques (avec reverse mapping)
   - Enums string (recommandé)
   - Const enums pour performance

5. **Fonctions**
   - Typage des paramètres et retour
   - Paramètres optionnels et par défaut
   - Rest parameters
   - Function overloading
   - Typage du `this`

6. **Génériques**
   - Code réutilisable pour plusieurs types
   - Contraintes avec `extends`
   - Types utilitaires: `Partial`, `Pick`, `Omit`, etc.
   - Inférence automatique des types

### Questions Fréquentes en Entretien

**Q1 : Quelle est la différence entre `any` et `unknown` ?**
> `any` désactive la vérification de types (dangereux). `unknown` est type-safe et nécessite une vérification avant utilisation. Exemple : avec `any`, vous pouvez appeler n'importe quelle méthode sans erreur à la compilation, mais avec `unknown`, vous devez d'abord vérifier le type.

**Q2 : Quand utiliser interface vs type alias ?**
> Interface pour objets et classes (extensibles, declaration merging). Type alias pour unions, intersections, types primitifs. Règle générale : interface pour les contrats d'objets qui seront implémentés, type pour tout le reste.

**Q3 : Qu'est-ce qu'un générique et pourquoi l'utiliser ?**
> Les génériques permettent de créer des composants réutilisables qui fonctionnent avec plusieurs types. Ils évitent la duplication de code et maintiennent la sécurité des types. Exemple : `function getFirst<T>(arr: T[]): T | undefined` peut fonctionner avec n'importe quel type de tableau.

**Q4 : Différence entre tuple et array ?**
> Array : même type pour tous les éléments, taille variable (`number[]`). Tuple : types différents possibles, taille et ordre fixes (`[string, number]`). Exemple : `let user: [string, number] = ["John", 25]`.

**Q5 : À quoi sert le type `never` ?**
> `never` représente quelque chose qui ne se produit jamais : fonction qui lance toujours une erreur, boucle infinie, ou exhaustive checks dans switch. Utile pour détecter des cas non gérés à la compilation.

**Q6 : Qu'est-ce que function overloading ?**
> Permet de définir plusieurs signatures pour une même fonction. TypeScript choisit la bonne signature selon les arguments passés. Utile quand une fonction peut accepter différentes combinaisons de paramètres.

**Q7 : Comment typer une fonction callback ?**
> Utiliser un type alias ou une interface avec signature de fonction : `type Callback = (error: Error | null, data: any) => void`. Pour les callbacks génériques : `type Callback<T> = (error: Error | null, data: T) => void`.

**Q8 : Expliquez Partial, Pick et Omit.**
> - `Partial<T>` : rend toutes les propriétés optionnelles (utile pour les updates)
> - `Pick<T, K>` : sélectionne certaines propriétés (créer un sous-ensemble)
> - `Omit<T, K>` : exclut certaines propriétés (retirer des champs sensibles)

**Q9 : Pourquoi utiliser `readonly` ?**
> `readonly` empêche la modification de propriétés après initialisation. Garantit l'immutabilité des données, prévient les bugs, rend le code plus prévisible. Particulièrement utile pour les configurations.

**Q10 : Différence entre enum et union type ?**
> Enum génère du code JavaScript (objet réel), a un reverse mapping pour les enums numériques, et peut avoir une logique. Union type est juste une vérification à la compilation, pas de code généré, plus léger. Utiliser union type sauf si besoin de reverse mapping ou logique métier.

---

## ⚠️ Pièges à Éviter

### ❌ Erreur 1 : Abuser de `any`

```typescript
// ❌ Mauvais : perd tous les bénéfices de TypeScript
function processData(data: any): any {
  return data.value.toUpperCase();  // Crash si data.value n'est pas string
}

// ✅ Bon : typage correct ou unknown
function processData(data: { value: string }): string {
  return data.value.toUpperCase();
}

// ✅ Ou avec unknown si vraiment inconnu
function processData(data: unknown): string {
  if (
    typeof data === "object" &&
    data !== null &&
    "value" in data &&
    typeof (data as any).value === "string"
  ) {
    return (data as { value: string }).value.toUpperCase();
  }
  throw new Error("Invalid data");
}
```

### ❌ Erreur 2 : Oublier readonly pour données immuables

```typescript
// ❌ Peut être modifié par erreur
interface Config {
  apiKey: string;
  apiUrl: string;
}

const config: Config = {
  apiKey: "secret",
  apiUrl: "https://api.example.com"
};

config.apiKey = "new-key";  // Oups, modifié par erreur

// ✅ Bon : utiliser readonly
interface Config {
  readonly apiKey: string;
  readonly apiUrl: string;
}

const config: Config = {
  apiKey: "secret",
  apiUrl: "https://api.example.com"
};

config.apiKey = "new-key";  // ❌ Erreur de compilation
```

### ❌ Erreur 3 : Ne pas utiliser les génériques quand nécessaire

```typescript
// ❌ Duplication de code
function getFirstNumber(arr: number[]): number | undefined {
  return arr[0];
}

function getFirstString(arr: string[]): string | undefined {
  return arr[0];
}

function getFirstBoolean(arr: boolean[]): boolean | undefined {
  return arr[0];
}

// ✅ Bon : utiliser un générique
function getFirst<T>(arr: T[]): T | undefined {
  return arr[0];
}
```

### ❌ Erreur 4 : Mauvaise utilisation des enums

```typescript
// ❌ Enum numérique sans valeurs explicites (fragile)
enum Status {
  Pending,    // 0
  Active,     // 1
  Inactive    // 2
}
// Si on ajoute un élément au début, tout change !

// ✅ Bon : enum string (stable)
enum Status {
  Pending = "PENDING",
  Active = "ACTIVE",
  Inactive = "INACTIVE"
}

// ✅ Ou union type (plus léger)
type Status = "PENDING" | "ACTIVE" | "INACTIVE";
```

### ❌ Erreur 5 : Ne pas gérer null/undefined

```typescript
// ❌ Peut causer une erreur runtime
function greet(user: User): string {
  return `Hello, ${user.name.toUpperCase()}`;  // Si name est undefined ?
}

// ✅ Bon : gérer les cas null/undefined
function greet(user: User): string {
  return `Hello, ${user.name?.toUpperCase() ?? "Guest"}`;
}

// ✅ Ou avec vérification
function greet(user: User): string {
  if (!user.name) {
    return "Hello, Guest";
  }
  return `Hello, ${user.name.toUpperCase()}`;
}
```

### ❌ Erreur 6 : Interface vs Type - mauvais choix

```typescript
// ❌ Type alias pour quelque chose qui sera étendu
type Animal = {
  name: string;
};

type Dog = Animal & {  // Moins lisible qu'extends
  breed: string;
};

// ✅ Bon : interface pour extension
interface Animal {
  name: string;
}

interface Dog extends Animal {
  breed: string;
}

// ✅ Type alias pour unions
type Pet = Dog | Cat;  // Interface ne peut pas faire ça
```

### ❌ Erreur 7 : Oublier les contraintes sur les génériques

```typescript
// ❌ Trop permissif
function getLength<T>(item: T): number {
  return item.length;  // ❌ T n'a peut-être pas 'length'
}

// ✅ Bon : ajouter une contrainte
interface HasLength {
  length: number;
}

function getLength<T extends HasLength>(item: T): number {
  return item.length;  // ✅ OK, T a forcément 'length'
}
```

---

## 📝 Checklist de Révision

### Types de Base
- [ ] Je connais tous les types primitifs (number, string, boolean, etc.)
- [ ] Je comprends la différence entre any et unknown
- [ ] Je sais quand utiliser void, never et undefined
- [ ] Je sais créer et utiliser des tuples

### Interfaces et Types
- [ ] Je peux créer des interfaces complexes
- [ ] Je comprends les propriétés optionnelles et readonly
- [ ] Je sais étendre des interfaces
- [ ] Je connais la différence entre interface et type alias
- [ ] Je sais utiliser les index signatures

### Enums
- [ ] Je peux créer des enums numériques et string
- [ ] Je comprends le reverse mapping
- [ ] Je sais quand utiliser enum vs union type

### Fonctions
- [ ] Je sais typer les paramètres et le retour
- [ ] Je comprends les paramètres optionnels et par défaut
- [ ] Je peux utiliser rest parameters
- [ ] Je connais le function overloading
- [ ] Je sais créer des type aliases pour fonctions

### Génériques
- [ ] Je comprends le concept de générique
- [ ] Je peux créer des fonctions génériques
- [ ] Je sais utiliser les contraintes avec extends
- [ ] Je connais les types utilitaires (Partial, Pick, Omit, etc.)
- [ ] Je peux créer des classes et interfaces génériques

---

## 💡 Exercices Pratiques

### Exercice 1 : Système de Cache Générique

```typescript
// Créer un système de cache typé
class Cache<T> {
  // À compléter
  // - Méthode set(key: string, value: T, ttl?: number)
  // - Méthode get(key: string): T | undefined
  // - Méthode has(key: string): boolean
  // - Méthode clear()
  // - Gérer l'expiration avec ttl
}
```

### Exercice 2 : Validation de Formulaire

```typescript
// Créer un système de validation typé
interface ValidationRule<T> {
  // À compléter
}

// Créer des validateurs pour :
// - Email
// - Longueur min/max
// - Numérique
// - Requis
```

### Exercice 3 : API Client Typé

```typescript
// Créer un client API générique
class ApiClient {
  // À compléter
  // - get<T>(url: string): Promise<T>
  // - post<T, U>(url: string, data: T): Promise<U>
  // - put<T, U>(url: string, data: T): Promise<U>
  // - delete(url: string): Promise<void>
}
```

---

## 🔗 Ressources Complémentaires

### Documentation Officielle
- **Types de base** : https://www.typescriptlang.org/docs/handbook/2/everyday-types.html
- **Interfaces** : https://www.typescriptlang.org/docs/handbook/2/objects.html
- **Génériques** : https://www.typescriptlang.org/docs/handbook/2/generics.html
- **Types utilitaires** : https://www.typescriptlang.org/docs/handbook/utility-types.html

### Pratique
- **TypeScript Playground** : https://www.typescriptlang.org/play
- **Exercices TypeScript** : https://github.com/type-challenges/type-challenges
- **Cheatsheet** : https://www.typescriptlang.org/cheatsheets

### Articles Recommandés
- Effective TypeScript (livre)
- TypeScript Deep Dive
- Tackling TypeScript

---

## 📊 Résumé du Chapitre 2

### Ce que vous devez retenir

✅ **Types Primitifs** : number, string, boolean, any, unknown, never, void
✅ **Structures** : interfaces, type aliases, tuples, arrays
✅ **Interfaces** : définissent la forme des objets, extensibles
✅ **Type Aliases** : flexibles, unions, intersections
✅ **Enums** : constantes nommées, préférer string enums
✅ **Fonctions** : typage des paramètres/retour, overloading
✅ **Génériques** : code réutilisable, type-safe

### Pour l'entretien, préparez-vous à :

1. **Expliquer** la différence entre concepts similaires
2. **Démontrer** l'utilisation des génériques
3. **Coder** une fonction avec typage complet
4. **Justifier** vos choix de types
5. **Identifier** les erreurs dans du code TypeScript
6. **Créer** des types complexes avec unions/intersections

---