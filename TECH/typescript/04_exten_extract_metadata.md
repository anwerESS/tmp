# Chapitre 4 : Extension et Extraction de Métadonnées de Types Existants

## 📑 Table des matières
- [4.1 Extension de types existants](#41-extension-de-types-existants)
- [4.2 Modification de types existants](#42-modification-de-types-existants)
- [4.3 Extraction de métadonnées de types](#43-extraction-de-métadonnées-de-types)
- [Challenge : Le bon type](#challenge-le-bon-type)
- [Points Clés pour l'Entretien](#-points-clés-pour-lentretien)

---

## 4.1 Extension de types existants

### 🎯 Extension avec interfaces

Les interfaces peuvent être **étendues** pour créer de nouveaux types basés sur des types existants.

#### Extension simple

```typescript
// Interface de base
interface Person {
  name: string;
  age: number;
}

// Extension avec extends
interface Employee extends Person {
  employeeId: string;
  department: string;
  salary: number;
}

// Employee a maintenant toutes les propriétés de Person + les siennes
const employee: Employee = {
  name: "Alice",
  age: 30,
  employeeId: "EMP-001",
  department: "Engineering",
  salary: 75000
};
```

#### Extension multiple

```typescript
// Interfaces de base
interface Identifiable {
  id: string;
}

interface Timestamped {
  createdAt: Date;
  updatedAt: Date;
}

interface Auditable {
  createdBy: string;
  modifiedBy: string;
}

// Extension multiple
interface Article extends Identifiable, Timestamped, Auditable {
  title: string;
  content: string;
  published: boolean;
}

const article: Article = {
  id: "article-123",
  title: "TypeScript Guide",
  content: "TypeScript is...",
  published: true,
  createdAt: new Date(),
  updatedAt: new Date(),
  createdBy: "user-1",
  modifiedBy: "user-1"
};
```

#### Override de propriétés

```typescript
interface Animal {
  name: string;
  age: number;
  sound: string;
}

// Spécialiser le type d'une propriété
interface Dog extends Animal {
  sound: "woof" | "bark";  // Plus spécifique que string
  breed: string;
}

const dog: Dog = {
  name: "Rex",
  age: 3,
  sound: "woof",  // Doit être "woof" ou "bark"
  breed: "Labrador"
};

// ❌ Erreur
const invalidDog: Dog = {
  name: "Max",
  age: 2,
  sound: "meow",  // Erreur: doit être "woof" ou "bark"
  breed: "Poodle"
};
```

### 🔧 Intersection avec &

Les **intersection types** combinent plusieurs types en un seul avec l'opérateur `&`.

```typescript
type Person = {
  name: string;
  age: number;
};

type Contact = {
  email: string;
  phone: string;
};

type Address = {
  street: string;
  city: string;
  country: string;
};

// Intersection : doit avoir TOUTES les propriétés
type FullProfile = Person & Contact & Address;

const profile: FullProfile = {
  // Propriétés de Person
  name: "Bob",
  age: 35,
  // Propriétés de Contact
  email: "bob@example.com",
  phone: "+33612345678",
  // Propriétés de Address
  street: "123 Main St",
  city: "Paris",
  country: "France"
};
```

#### Différence extends vs &

```typescript
// Interface extends : POO classique
interface Manager extends Employee {
  teamSize: number;
}

// Type intersection : composition fonctionnelle
type Manager2 = Employee & {
  teamSize: number;
};

// Les deux fonctionnent, mais:
// - extends : pour hiérarchies claires
// - & : pour composition flexible
```

### 📝 Exemple Pratique : Système de Permissions

```typescript
// Types de base
interface Entity {
  id: string;
  createdAt: Date;
}

interface Ownable {
  ownerId: string;
}

interface Shareable {
  sharedWith: string[];
  isPublic: boolean;
}

interface Deletable {
  deletedAt: Date | null;
  deletedBy: string | null;
}

// Document avec permissions complètes
interface Document extends Entity, Ownable, Shareable, Deletable {
  title: string;
  content: string;
  category: string;
}

// Service de gestion de documents
class DocumentService {
  private documents: Map<string, Document> = new Map();
  
  create(userId: string, title: string, content: string): Document {
    const doc: Document = {
      id: `doc-${Date.now()}`,
      title,
      content,
      category: "general",
      createdAt: new Date(),
      ownerId: userId,
      sharedWith: [],
      isPublic: false,
      deletedAt: null,
      deletedBy: null
    };
    
    this.documents.set(doc.id, doc);
    return doc;
  }
  
  canAccess(docId: string, userId: string): boolean {
    const doc = this.documents.get(docId);
    if (!doc) return false;
    
    // Propriétaire peut accéder
    if (doc.ownerId === userId) return true;
    
    // Document public
    if (doc.isPublic) return true;
    
    // Partagé avec l'utilisateur
    if (doc.sharedWith.includes(userId)) return true;
    
    return false;
  }
  
  share(docId: string, userId: string, targetUserId: string): void {
    const doc = this.documents.get(docId);
    if (!doc) throw new Error("Document non trouvé");
    
    if (doc.ownerId !== userId) {
      throw new Error("Seul le propriétaire peut partager");
    }
    
    if (!doc.sharedWith.includes(targetUserId)) {
      doc.sharedWith.push(targetUserId);
    }
  }
  
  softDelete(docId: string, userId: string): void {
    const doc = this.documents.get(docId);
    if (!doc) throw new Error("Document non trouvé");
    
    if (doc.ownerId !== userId) {
      throw new Error("Seul le propriétaire peut supprimer");
    }
    
    doc.deletedAt = new Date();
    doc.deletedBy = userId;
  }
}

// Utilisation
const service = new DocumentService();

const doc = service.create("user-1", "Mon Document", "Contenu...");
console.log(`Document créé: ${doc.id}`);

service.share(doc.id, "user-1", "user-2");
console.log(`Document partagé avec user-2`);

console.log(service.canAccess(doc.id, "user-1"));  // true (propriétaire)
console.log(service.canAccess(doc.id, "user-2"));  // true (partagé)
console.log(service.canAccess(doc.id, "user-3"));  // false
```

---

## 4.2 Modification de types existants

### 🎯 Utility Types pour Modification

TypeScript fournit des **utility types** pour modifier des types existants.

#### Partial - Rendre optionnel

```typescript
interface User {
  id: string;
  name: string;
  email: string;
  age: number;
}

// Toutes les propriétés deviennent optionnelles
type PartialUser = Partial<User>;
// {
//   id?: string;
//   name?: string;
//   email?: string;
//   age?: number;
// }

// Utile pour les updates
function updateUser(id: string, updates: Partial<User>): void {
  console.log(`Mise à jour de ${id}:`, updates);
}

// Seules les propriétés à modifier
updateUser("123", { name: "John" });
updateUser("456", { email: "new@email.com", age: 30 });
```

#### Required - Rendre obligatoire

```typescript
interface Config {
  host?: string;
  port?: number;
  database?: string;
  username?: string;
}

// Toutes les propriétés deviennent obligatoires
type RequiredConfig = Required<Config>;
// {
//   host: string;
//   port: number;
//   database: string;
//   username: string;
// }

function validateConfig(config: RequiredConfig): boolean {
  // Toutes les propriétés sont garanties présentes
  return config.host.length > 0 && config.port > 0;
}
```

#### Readonly - Rendre immuable

```typescript
interface Settings {
  theme: string;
  language: string;
  notifications: boolean;
}

// Toutes les propriétés deviennent readonly
type ReadonlySettings = Readonly<Settings>;
// {
//   readonly theme: string;
//   readonly language: string;
//   readonly notifications: boolean;
// }

const settings: ReadonlySettings = {
  theme: "dark",
  language: "fr",
  notifications: true
};

// ❌ Erreur
settings.theme = "light";  // Cannot assign to 'theme' because it is a read-only property
```

#### Pick - Sélectionner des propriétés

```typescript
interface Product {
  id: string;
  name: string;
  description: string;
  price: number;
  stock: number;
  category: string;
  manufacturer: string;
  createdAt: Date;
}

// Sélectionner seulement certaines propriétés
type ProductPreview = Pick<Product, "id" | "name" | "price">;
// {
//   id: string;
//   name: string;
//   price: number;
// }

type ProductListItem = Pick<Product, "id" | "name" | "price" | "stock">;

// Utilisation
function displayPreview(product: ProductPreview): string {
  return `${product.name} - ${product.price}€`;
}
```

#### Omit - Exclure des propriétés

```typescript
// Exclure des propriétés sensibles
type PublicUser = Omit<User, "password" | "salt">;

interface User {
  id: string;
  name: string;
  email: string;
  password: string;
  salt: string;
  createdAt: Date;
}

// PublicUser n'a pas password ni salt
type PublicUser = Omit<User, "password" | "salt">;
// {
//   id: string;
//   name: string;
//   email: string;
//   createdAt: Date;
// }

function getPublicProfile(userId: string): PublicUser {
  const user = database.getUser(userId);
  
  // Retourner sans les champs sensibles
  const { password, salt, ...publicUser } = user;
  return publicUser;
}
```

### 🔄 Combinaison de Utility Types

```typescript
interface Product {
  id: string;
  name: string;
  description: string;
  price: number;
  stock: number;
}

// Créer un type pour la création (sans id)
type CreateProduct = Omit<Product, "id">;

// Créer un type pour la mise à jour (tout optionnel sauf id)
type UpdateProduct = Partial<Omit<Product, "id">> & { id: string };

// Créer un type pour l'affichage (seulement certains champs en readonly)
type DisplayProduct = Readonly<Pick<Product, "id" | "name" | "price">>;

// Utilisation
function createProduct(data: CreateProduct): Product {
  return {
    id: generateId(),
    ...data
  };
}

function updateProduct(data: UpdateProduct): void {
  console.log(`Mise à jour de ${data.id}`);
  // Seules les propriétés fournies sont mises à jour
}

const newProduct = createProduct({
  name: "Laptop",
  description: "High-end laptop",
  price: 1500,
  stock: 10
});

updateProduct({
  id: newProduct.id,
  price: 1400,  // Seul le prix change
  stock: 8
});
```

### 📝 Exemple Pratique : CRUD Type-Safe

```typescript
// Type de base
interface BlogPost {
  id: string;
  title: string;
  slug: string;
  content: string;
  excerpt: string;
  author: string;
  tags: string[];
  published: boolean;
  createdAt: Date;
  updatedAt: Date;
  viewCount: number;
}

// Types dérivés pour différentes opérations
type CreatePostInput = Omit<BlogPost, "id" | "slug" | "createdAt" | "updatedAt" | "viewCount">;

type UpdatePostInput = Partial<Omit<BlogPost, "id" | "createdAt" | "author">> & {
  id: string;
};

type PostPreview = Pick<BlogPost, "id" | "title" | "excerpt" | "author" | "createdAt">;

type PostListItem = Pick<BlogPost, "id" | "title" | "slug" | "author" | "published" | "createdAt" | "viewCount">;

type PublishedPost = Required<Pick<BlogPost, "id" | "title" | "content" | "author">> & {
  published: true;
};

// Service CRUD
class BlogService {
  private posts: Map<string, BlogPost> = new Map();
  
  create(input: CreatePostInput): BlogPost {
    const slug = this.generateSlug(input.title);
    
    const post: BlogPost = {
      ...input,
      id: `post-${Date.now()}`,
      slug,
      createdAt: new Date(),
      updatedAt: new Date(),
      viewCount: 0
    };
    
    this.posts.set(post.id, post);
    return post;
  }
  
  update(input: UpdatePostInput): BlogPost | null {
    const post = this.posts.get(input.id);
    if (!post) return null;
    
    const updated: BlogPost = {
      ...post,
      ...input,
      updatedAt: new Date()
    };
    
    this.posts.set(post.id, updated);
    return updated;
  }
  
  getPreview(id: string): PostPreview | null {
    const post = this.posts.get(id);
    if (!post) return null;
    
    return {
      id: post.id,
      title: post.title,
      excerpt: post.excerpt,
      author: post.author,
      createdAt: post.createdAt
    };
  }
  
  getAllPreviews(): PostPreview[] {
    return Array.from(this.posts.values()).map(post => ({
      id: post.id,
      title: post.title,
      excerpt: post.excerpt,
      author: post.author,
      createdAt: post.createdAt
    }));
  }
  
  getPublished(): PublishedPost[] {
    return Array.from(this.posts.values())
      .filter((post): post is PublishedPost => post.published)
      .map(post => ({
        id: post.id,
        title: post.title,
        content: post.content,
        author: post.author,
        published: true
      }));
  }
  
  incrementViews(id: string): void {
    const post = this.posts.get(id);
    if (post) {
      post.viewCount++;
    }
  }
  
  private generateSlug(title: string): string {
    return title
      .toLowerCase()
      .replace(/[^a-z0-9]+/g, "-")
      .replace(/^-|-$/g, "");
  }
}

// Utilisation
const blog = new BlogService();

// Créer un article
const post = blog.create({
  title: "Introduction à TypeScript",
  content: "TypeScript est un langage...",
  excerpt: "Découvrez TypeScript",
  author: "user-1",
  tags: ["typescript", "programming"],
  published: false
});

console.log(`Article créé: ${post.id}`);

// Mettre à jour
blog.update({
  id: post.id,
  title: "Guide Complet TypeScript",
  published: true
});

// Obtenir les aperçus
const previews = blog.getAllPreviews();
console.log(`${previews.length} articles trouvés`);

// Obtenir les articles publiés
const published = blog.getPublished();
console.log(`${published.length} articles publiés`);
```

---

## 4.3 Extraction de métadonnées de types

### 🎯 ReturnType - Type de retour

```typescript
// Extraire le type de retour d'une fonction
function getUser() {
  return {
    id: "123",
    name: "John",
    email: "john@example.com",
    role: "admin" as const
  };
}

// Extraire automatiquement le type
type User = ReturnType<typeof getUser>;
// {
//   id: string;
//   name: string;
//   email: string;
//   role: "admin";
// }

// Utiliser le type extrait
const user: User = getUser();
```

### 🔧 Parameters - Types des paramètres

```typescript
function createOrder(
  userId: string,
  items: Array<{ productId: string; quantity: number }>,
  shippingAddress: { street: string; city: string; zipCode: string }
) {
  return {
    orderId: `order-${Date.now()}`,
    userId,
    items,
    shippingAddress,
    status: "pending" as const
  };
}

// Extraire les types des paramètres
type CreateOrderParams = Parameters<typeof createOrder>;
// [
//   userId: string,
//   items: Array<{ productId: string; quantity: number }>,
//   shippingAddress: { street: string; city: string; zipCode: string }
// ]

// Utiliser pour créer un wrapper
function createOrderWithLogging(...args: CreateOrderParams) {
  console.log("Creating order with:", args);
  return createOrder(...args);
}
```

### 📦 ConstructorParameters - Paramètres du constructeur

```typescript
class User {
  constructor(
    public id: string,
    public name: string,
    public email: string,
    public role: "admin" | "user"
  ) {}
}

// Extraire les types des paramètres du constructeur
type UserConstructorParams = ConstructorParameters<typeof User>;
// [id: string, name: string, email: string, role: "admin" | "user"]

// Factory function
function createUser(...args: UserConstructorParams): User {
  console.log("Creating user:", args[1]);
  return new User(...args);
}

const user = createUser("1", "Alice", "alice@example.com", "admin");
```

### 🎨 InstanceType - Type d'instance

```typescript
class Database {
  private connected: boolean = false;
  
  connect(): void {
    this.connected = true;
  }
  
  query(sql: string): any[] {
    return [];
  }
}

// Extraire le type de l'instance
type DatabaseInstance = InstanceType<typeof Database>;

function useDatabaseConnection(db: DatabaseInstance): void {
  db.connect();
  db.query("SELECT * FROM users");
}
```

### 🔍 Awaited - Type résolu d'une Promise

```typescript
async function fetchUserData() {
  return {
    id: "123",
    name: "John",
    email: "john@example.com",
    preferences: {
      theme: "dark",
      language: "en"
    }
  };
}

// Type : Promise<{...}>
type FetchReturn = ReturnType<typeof fetchUserData>;

// Type : {...} (sans Promise)
type UserData = Awaited<ReturnType<typeof fetchUserData>>;
// {
//   id: string;
//   name: string;
//   email: string;
//   preferences: { theme: string; language: string; };
// }

// Utilisation
async function processUser() {
  const userData: UserData = await fetchUserData();
  console.log(userData.name);
}
```

### 📝 Exemple Pratique : API Client Type-Safe

```typescript
// Définition des endpoints
class ApiClient {
  async getUser(id: string) {
    const response = await fetch(`/api/users/${id}`);
    return response.json() as Promise<{
      id: string;
      name: string;
      email: string;
      role: "admin" | "user";
      createdAt: string;
    }>;
  }
  
  async getProducts(category?: string, limit?: number) {
    const params = new URLSearchParams();
    if (category) params.set("category", category);
    if (limit) params.set("limit", limit.toString());
    
    const response = await fetch(`/api/products?${params}`);
    return response.json() as Promise<Array<{
      id: string;
      name: string;
      price: number;
      stock: number;
    }>>;
  }
  
  async createOrder(order: {
    userId: string;
    items: Array<{ productId: string; quantity: number }>;
  }) {
    const response = await fetch("/api/orders", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(order)
    });
    return response.json() as Promise<{
      orderId: string;
      status: "pending" | "confirmed";
      total: number;
    }>;
  }
}

// Extraire automatiquement tous les types
type User = Awaited<ReturnType<ApiClient["getUser"]>>;
type Product = Awaited<ReturnType<ApiClient["getProducts"]>>[number];
type Order = Awaited<ReturnType<ApiClient["createOrder"]>>;

type GetUserParams = Parameters<ApiClient["getUser"]>;
type GetProductsParams = Parameters<ApiClient["getProducts"]>;
type CreateOrderParams = Parameters<ApiClient["createOrder"]>;

// Créer des types dérivés
type UserId = User["id"];
type UserRole = User["role"];
type ProductPrice = Product["price"];
type OrderStatus = Order["status"];

// Service utilisant les types extraits
class UserService {
  constructor(private api: ApiClient) {}
  
  async getUserById(id: UserId): Promise<User> {
    return this.api.getUser(id);
  }
  
  async getUsersByRole(role: UserRole): Promise<User[]> {
    // Simulation - dans la vraie vie, appel API
    return [];
  }
}

class OrderService {
  constructor(private api: ApiClient) {}
  
  async placeOrder(
    userId: UserId,
    items: CreateOrderParams[0]["items"]
  ): Promise<Order> {
    return this.api.createOrder({ userId, items });
  }
  
  async getOrderStatus(orderId: string): Promise<OrderStatus> {
    // Simulation
    return "pending";
  }
}

// Utilisation type-safe
const api = new ApiClient();
const userService = new UserService(api);
const orderService = new OrderService(api);

async function example() {
  // Types automatiquement corrects
  const user = await userService.getUserById("123");
  console.log(user.name);  // Type: string
  console.log(user.role);  // Type: "admin" | "user"
  
  const order = await orderService.placeOrder("123", [
    { productId: "prod-1", quantity: 2 }
  ]);
  console.log(order.status);  // Type: "pending" | "confirmed"
}
```

---

## Challenge : Le bon type

### 🎯 Défi

Créez un système de gestion de tâches avec les contraintes suivantes :

1. **Type de base Task** avec toutes les propriétés
2. **CreateTaskInput** : pour créer une tâche (sans id, dates auto)
3. **UpdateTaskInput** : pour modifier (tout optionnel sauf id)
4. **TaskPreview** : pour lister (seulement id, title, status, dueDate)
5. **CompletedTask** : pour archiver (status doit être "completed")

### 💡 Solution

```typescript
// 1. Type de base
interface Task {
  id: string;
  title: string;
  description: string;
  status: "todo" | "in_progress" | "completed";
  priority: "low" | "medium" | "high";
  assignedTo: string | null;
  dueDate: Date | null;
  tags: string[];
  createdAt: Date;
  updatedAt: Date;
}

// 2. Type pour création
type CreateTaskInput = Omit<Task, "id" | "createdAt" | "updatedAt">;

// 3. Type pour mise à jour
type UpdateTaskInput = Partial<Omit<Task, "id" | "createdAt">> & {
  id: string;
};

// 4. Type pour aperçu
type TaskPreview = Pick<Task, "id" | "title" | "status" | "dueDate">;

// 5. Type pour tâches complétées
type CompletedTask = Task & {
  status: "completed";
};

// Service de gestion
class TaskManager {
  private tasks: Map<string, Task> = new Map();
  
  create(input: CreateTaskInput): Task {
    const task: Task = {
      ...input,
      id: `task-${Date.now()}`,
      createdAt: new Date(),
      updatedAt: new Date()
    };
    
    this.tasks.set(task.id, task);
    return task;
  }
  
  update(input: UpdateTaskInput): Task | null {
    const task = this.tasks.get(input.id);
    if (!task) return null;
    
    const updated: Task = {
      ...task,
      ...input,
      updatedAt: new Date()
    };
    
    this.tasks.set(task.id, updated);
    return updated;
  }
  
  getPreview(id: string): TaskPreview | null {
    const task = this.tasks.get(id);
    if (!task) return null;
    
    return {
      id: task.id,
      title: task.title,
      status: task.status,
      dueDate: task.dueDate
    };
  }
  
  getAllPreviews(): TaskPreview[] {
    return Array.from(this.tasks.values()).map(task => ({
      id: task.id,
      title: task.title,
      status: task.status,
      dueDate: task.dueDate
    }));
  }
  
  getCompleted(): CompletedTask[] {
    return Array.from(this.tasks.values())
      .filter((task): task is CompletedTask => task.status === "completed");
  }
  
  complete(id: string): CompletedTask | null {
    const task = this.tasks.get(id);
    if (!task) return null;
    
    const completed: CompletedTask = {
      ...task,
      status: "completed",
      updatedAt: new Date()
    };
    
    this.tasks.set(id, completed);
    return completed;
  }
}

// Tests
const manager = new TaskManager();

// Créer une tâche
const task = manager.create({
  title: "Implémenter feature X",
  description: "Ajouter la fonctionnalité X au système",
  status: "todo",
  priority: "high",
  assignedTo: "user-1",
  dueDate: new Date("2024-12-31"),
  tags: ["feature", "urgent"]
});

console.log(`✅ Tâche créée: ${task.id}`);

// Mettre à jour
manager.update({
  id: task.id,
  status: "in_progress"
});

console.log("🔄 Tâche mise en cours");

// Compléter
const completed = manager.complete(task.id);
if (completed) {
  console.log(`✅ Tâche complétée: ${completed.title}`);
}

// Lister les aperçus
const previews = manager.getAllPreviews();
console.log(`📋 ${previews.length} tâches totales`);

// Lister les complétées
const completedTasks = manager.getCompleted();
console.log(`✅ ${completedTasks.length} tâches complétées`);
```

### 🎯 Points à retenir

1. **Omit** pour retirer des propriétés (id, dates auto)
2. **Partial** pour rendre optionnel (updates)
3. **Pick** pour sélectionner (previews)
4. **Type narrowing** pour status spécifique (completed)
5. **Combinaison** de utility types pour cas complexes

---

## 🎯 Points Clés pour l'Entretien

### Extension de Types
1. **extends** : héritage d'interfaces
2. **&** : intersection types
3. **Extension multiple** : plusieurs interfaces/types
4. **Override** : spécialiser les propriétés

### Modification de Types
1. **Partial<T>** : toutes propriétés optionnelles
2. **Required<T>** : toutes propriétés obligatoires
3. **Readonly<T>** : toutes propriétés immuables
4. **Pick<T, K>** : sélectionner propriétés
5. **Omit<T, K>** : exclure propriétés

### Extraction de Métadonnées
1. **ReturnType<T>** : type de retour fonction
2. **Parameters<T>** : types des paramètres
3. **ConstructorParameters<T>** : paramètres constructeur
4. **InstanceType<T>** : type d'instance classe
5. **Awaited<T>** : type résolu Promise

---

## ⚠️ Pièges à Éviter

### Piège 1 : Confusion extends vs &

```typescript
// extends : pour interfaces
interface A extends B { }

// & : pour types
type A = B & C;

// ❌ Mélanger
type A = B extends C;  // Erreur de syntaxe
```

### Piège 2 : Partial trop permissif

```typescript
interface User {
  id: string;
  name: string;
}

// ❌ Tout devient optionnel, même id
function update(user: Partial<User>) {
  // user.id pourrait être undefined !
}

// ✅ Garder id obligatoire
function update(user: Partial<User> & { id: string }) {
  // id est garanti présent
}
```

### Piège 3 : Oublier Awaited avec Promises

```typescript
async function getData() {
  return { value: 42 };
}

// ❌ Type inclut Promise
type Data1 = ReturnType<typeof getData>;  // Promise<{ value: number }>

// ✅ Type sans Promise
type Data2 = Awaited<ReturnType<typeof getData>>;  // { value: number }
```

### Piège 4 : Pick/Omit avec propriétés inexistantes

```typescript
interface User {
  id: string;
  name: string;
}

// ❌ Erreur : 'email' n'existe pas
type PartialUser = Pick<User, "id" | "email">;

// ✅ Utiliser seulement les propriétés existantes
type PartialUser = Pick<User, "id">;
```

### Piège 5 : Readonly superficiel

```typescript
interface User {
  id: string;
  address: {
    city: string;
  };
}

type ReadonlyUser = Readonly<User>;

const user: ReadonlyUser = {
  id: "1",
  address: { city: "Paris" }
};

user.id = "2";  // ❌ Erreur : readonly
user.address.city = "Lyon";  // ✅ OK ! address n'est pas readonly en profondeur

// Solution : DeepReadonly (type personnalisé)
type DeepReadonly<T> = {
  readonly [P in keyof T]: T[P] extends object ? DeepReadonly<T[P]> : T[P];
};
```

---

## 📝 Checklist de Révision

### Extension
- [ ] Je sais étendre des interfaces avec extends
- [ ] Je comprends les intersection types avec &
- [ ] Je connais la différence extends vs &
- [ ] Je peux faire de l'extension multiple

### Modification
- [ ] Je maîtrise Partial pour updates
- [ ] Je sais utiliser Required
- [ ] Je comprends Readonly
- [ ] Je maîtrise Pick et Omit
- [ ] Je peux combiner plusieurs utility types

### Extraction
- [ ] Je sais utiliser ReturnType
- [ ] Je comprends Parameters
- [ ] Je connais ConstructorParameters
- [ ] Je sais utiliser InstanceType
- [ ] Je maîtrise Awaited pour Promises

---

## 💡 Exercices Pratiques

### Exercice 1 : Système d'authentification

Créez un système avec :
- Type User de base
- LoginInput (seulement email et password)
- RegisterInput (sans id, createdAt)
- UserProfile (sans password)
- AdminUser (extends User avec permissions)

```typescript
// À compléter
interface User {
  id: string;
  email: string;
  password: string;
  name: string;
  role: "user" | "admin";
  createdAt: Date;
}

// Vos types ici...
```

### Exercice 2 : API Type-Safe

Créez des types pour une API REST :
- Extraire les types de retour
- Créer des types pour les paramètres
- Types pour les réponses paginées
- Types pour les erreurs

```typescript
class API {
  async getUsers(page: number, limit: number) {
    return {
      data: [] as Array<{ id: string; name: string }>,
      page,
      limit,
      total: 0
    };
  }
  
  async getUser(id: string) {
    return {
      id,
      name: "John",
      email: "john@example.com"
    };
  }
}

// Extraire les types...
```

### Exercice 3 : Form Builder

Créez un constructeur de formulaire type-safe :
- Type Field générique
- Types dérivés pour différents types de champs
- Validation type-safe
- Extraction des valeurs

```typescript
interface FormField<T> {
  name: string;
  value: T;
  required: boolean;
  validate: (value: T) => boolean;
}

// À compléter...
```

---

## 🔗 Ressources Complémentaires

### Documentation Officielle
- **Utility Types** : https://www.typescriptlang.org/docs/handbook/utility-types.html
- **Type Manipulation** : https://www.typescriptlang.org/docs/handbook/2/types-from-types.html
- **Generics** : https://www.typescriptlang.org/docs/handbook/2/generics.html

### Articles Recommandés
- "Advanced TypeScript Patterns"
- "Type-Safe API Clients"
- "Effective TypeScript" (livre)

### Outils Pratiques
- TypeScript Playground : https://www.typescriptlang.org/play
- Type Challenges : https://github.com/type-challenges/type-challenges
- TS Config Explorer : https://www.typescriptlang.org/tsconfig

---

## 📊 Résumé du Chapitre 4

Ce chapitre a couvert l'**extension et l'extraction de métadonnées** :

### Extension de Types
✅ **extends** : héritage d'interfaces (POO)
✅ **&** : intersection types (composition)
✅ **Extension multiple** : combiner plusieurs sources
✅ **Override** : spécialisation de propriétés

### Modification de Types
✅ **Partial** : rendre optionnel (updates)
✅ **Required** : rendre obligatoire (validation)
✅ **Readonly** : immutabilité (configs)
✅ **Pick** : sélectionner propriétés (DTOs)
✅ **Omit** : exclure propriétés (sécurité)

### Extraction de Métadonnées
✅ **ReturnType** : type de retour fonction
✅ **Parameters** : types des paramètres
✅ **ConstructorParameters** : paramètres constructeur
✅ **InstanceType** : type d'instance
✅ **Awaited** : type résolu Promise

### Applications Pratiques
✅ **CRUD type-safe** : create, read, update, delete
✅ **API clients** : extraction automatique des types
✅ **Permissions** : système de droits d'accès
✅ **Blog system** : gestion de contenu

---

## 🎓 Concepts Clés à Retenir

### Pour l'Entretien

**Question 1 : Quelle est la différence entre extends et & ?**
> `extends` est pour l'héritage d'interfaces (POO), `&` pour les intersection types (composition fonctionnelle). `extends` permet aussi l'override de propriétés.

**Question 2 : Quand utiliser Partial vs Pick ?**
> `Partial<T>` rend toutes les propriétés optionnelles. `Pick<T, K>` sélectionne seulement certaines propriétés. Pour un update, combinez : `Partial<Omit<T, "id">> & { id: string }`.

**Question 3 : Comment extraire le type de retour d'une fonction async ?**
> Utilisez `Awaited<ReturnType<typeof func>>`. `ReturnType` donne `Promise<T>`, `Awaited` extrait `T`.

**Question 4 : Pourquoi combiner plusieurs utility types ?**
> Pour créer des types précis : `Partial<Omit<User, "id">> & { id: string }` permet un update où id est obligatoire mais tout le reste optionnel.

**Question 5 : Comment créer un type DeepReadonly ?**
> Avec récursion : `type DeepReadonly<T> = { readonly [P in keyof T]: T[P] extends object ? DeepReadonly<T[P]> : T[P] }`

---

## 🎯 Patterns Importants

### Pattern 1 : CRUD Type-Safe

```typescript
interface Entity {
  id: string;
  createdAt: Date;
  updatedAt: Date;
}

type CreateInput<T extends Entity> = Omit<T, keyof Entity>;
type UpdateInput<T extends Entity> = Partial<Omit<T, "id" | "createdAt">> & { id: string };
type EntityPreview<T extends Entity> = Pick<T, "id"> & Partial<T>;
```

### Pattern 2 : API Response Types

```typescript
interface ApiResponse<T> {
  data: T;
  status: number;
  message: string;
}

type UserResponse = ApiResponse<User>;
type UsersResponse = ApiResponse<User[]>;
type ErrorResponse = Omit<ApiResponse<null>, "data"> & { error: string };
```

### Pattern 3 : Form Handling

```typescript
interface FormField {
  value: string;
  error: string | null;
  touched: boolean;
}

type FormState<T> = Record<keyof T, FormField>;
type FormValues<T> = Record<keyof T, T[keyof T]>;
type FormErrors<T> = Partial<Record<keyof T, string>>;
```

### Pattern 4 : Permissions System

```typescript
interface WithPermissions {
  ownerId: string;
  sharedWith: string[];
  isPublic: boolean;
}

type Ownable<T> = T & WithPermissions;
type PublicEntity<T> = Ownable<T> & { isPublic: true };
type PrivateEntity<T> = Ownable<T> & { isPublic: false };
```

---

## 💪 Mise en Pratique

### Projet Suggéré : Blog API

Créez une API de blog complète avec :

1. **Types de base**
   - Article, User, Comment
   - Extension avec Timestamped, Ownable, etc.

2. **Types CRUD**
   - CreateArticleInput
   - UpdateArticleInput
   - ArticlePreview
   - PublishedArticle

3. **API Client**
   - Méthodes typées
   - Extraction automatique ReturnType
   - Gestion des erreurs

4. **Permissions**
   - Système de droits
   - Partage d'articles
   - Articles publics/privés

5. **Validation**
   - Types pour formulaires
   - Règles de validation
   - Messages d'erreur typés

---

## 🔄 Comparaison Avant/Après

### Avant TypeScript Avancé

```typescript
// ❌ Types dupliqués
interface User {
  id: string;
  name: string;
  email: string;
}

interface CreateUserInput {
  name: string;
  email: string;
}

interface UpdateUserInput {
  id: string;
  name?: string;
  email?: string;
}

interface UserPreview {
  id: string;
  name: string;
}
```

### Après TypeScript Avancé

```typescript
// ✅ Types dérivés automatiquement
interface User {
  id: string;
  name: string;
  email: string;
  createdAt: Date;
}

type CreateUserInput = Omit<User, "id" | "createdAt">;
type UpdateUserInput = Partial<Omit<User, "id">> & { id: string };
type UserPreview = Pick<User, "id" | "name">;
```

**Avantages :**
- ✅ Pas de duplication
- ✅ Mise à jour automatique si User change
- ✅ Moins d'erreurs
- ✅ Code plus maintenable

---

## 📈 Progression des Compétences

Après ce chapitre, vous devriez être capable de :

**Niveau Débutant** ✅
- Étendre des interfaces simples
- Utiliser Partial et Readonly
- Extraire ReturnType

**Niveau Intermédiaire** ✅
- Combiner plusieurs utility types
- Créer des types CRUD complets
- Utiliser Parameters et ConstructorParameters

**Niveau Avancé** ✅
- Créer des patterns type-safe réutilisables
- Extraire automatiquement les types d'APIs
- Concevoir des systèmes de types complexes

---

## 🎓 Questions d'Auto-Évaluation

1. Quelle est la différence entre `extends` et `&` ?
2. Comment créer un type pour un update qui garde l'id obligatoire ?
3. Comment extraire le type de retour d'une fonction async ?
4. Pourquoi `Readonly` n'est-il pas profond ?
5. Quelle est la différence entre `Pick` et `Omit` ?
6. Comment créer un type pour une création sans id ni dates ?
7. À quoi sert `ConstructorParameters` ?
8. Comment extraire le type d'un élément de tableau ?
9. Pourquoi combiner `Partial` et `Omit` pour les updates ?
10. Comment garantir qu'une propriété a une valeur spécifique ?

---

## 🌟 Conseils pour l'Entretien

### Ce que les recruteurs cherchent

1. **Compréhension des concepts**
   - Expliquer clairement extends vs &
   - Justifier l'utilisation des utility types

2. **Application pratique**
   - Créer des types CRUD en live coding
   - Résoudre des problèmes réels

3. **Bonnes pratiques**
   - Éviter la duplication
   - Créer des types réutilisables
   - Penser maintenabilité

4. **Connaissance de l'écosystème**
   - Utility types standards
   - Patterns courants
   - Outils disponibles

### Comment se préparer

1. **Pratiquer en live coding**
   - Créer des types sans auto-complétion
   - Expliquer votre raisonnement
   - Gérer les cas limites

2. **Connaître les patterns**
   - CRUD type-safe
   - API clients
   - Form handling
   - Permissions

3. **Être prêt à justifier**
   - Pourquoi ce type plutôt qu'un autre ?
   - Quels sont les avantages ?
   - Quelles sont les limitations ?

---