# Chapitre 5 : Comportements Dynamiques avec les Décorateurs

## 📑 Table des matières
- [5.1 Qu'est-ce qu'un décorateur ?](#51-quest-ce-quun-décorateur)
- [5.2 Décorateurs de méthode](#52-décorateurs-de-méthode)
- [5.3 Decorator Factories](#53-decorator-factories)
- [5.4 Décorateurs de classe](#54-décorateurs-de-classe)
- [5.5 Décorateurs de propriété](#55-décorateurs-de-propriété)
- [Points Clés pour l'Entretien](#-points-clés-pour-lentretien)

---

## 5.1 Qu'est-ce qu'un décorateur ?

### 🎯 Introduction aux Décorateurs

Un **décorateur** est une fonction spéciale qui permet de **modifier ou d'annoter** des classes, méthodes, propriétés ou paramètres. C'est un pattern de design qui ajoute des comportements sans modifier le code original.

⚠️ **Configuration requise** :

```json
// tsconfig.json
{
  "compilerOptions": {
    "experimentalDecorators": true,
    "emitDecoratorMetadata": true
  }
}
```

### 📖 Concept de base

```typescript
// Un décorateur est une fonction qui prend des paramètres spécifiques
function myDecorator(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
  console.log("Décorateur appliqué !");
}

class Example {
  @myDecorator
  method() {
    console.log("Méthode exécutée");
  }
}
```

### 🎨 Types de décorateurs

TypeScript supporte **5 types** de décorateurs :

1. **Décorateur de classe** : `@ClassDecorator`
2. **Décorateur de méthode** : `@MethodDecorator`
3. **Décorateur de propriété** : `@PropertyDecorator`
4. **Décorateur d'accesseur** : `@AccessorDecorator`
5. **Décorateur de paramètre** : `@ParameterDecorator`

### 💡 Pourquoi utiliser des décorateurs ?

**Avantages :**
- ✅ **Séparation des préoccupations** : logique métier séparée des aspects transversaux
- ✅ **Réutilisabilité** : même décorateur sur plusieurs méthodes/classes
- ✅ **Lisibilité** : code plus déclaratif
- ✅ **Non-intrusif** : pas de modification du code original

**Cas d'usage courants :**
- 📊 Logging et monitoring
- ⏱️ Mesure de performance
- 🔒 Vérification de permissions
- ✅ Validation de données
- 💾 Cache et mémoization
- 🔄 Retry automatique
- 📝 Métadonnées (Angular, NestJS)

### 📝 Exemple Simple

```typescript
// Décorateur de logging simple
function log(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
  const originalMethod = descriptor.value;
  
  descriptor.value = function(...args: any[]) {
    console.log(`📞 Appel de ${propertyKey} avec arguments:`, args);
    const result = originalMethod.apply(this, args);
    console.log(`✅ Résultat:`, result);
    return result;
  };
  
  return descriptor;
}

class Calculator {
  @log
  add(a: number, b: number): number {
    return a + b;
  }
  
  @log
  multiply(a: number, b: number): number {
    return a * b;
  }
}

const calc = new Calculator();
calc.add(5, 3);
// Output:
// 📞 Appel de add avec arguments: [5, 3]
// ✅ Résultat: 8

calc.multiply(4, 7);
// Output:
// 📞 Appel de multiply avec arguments: [4, 7]
// ✅ Résultat: 28
```

---

## 5.2 Décorateurs de méthode

### 🎯 Signature d'un décorateur de méthode

```typescript
function methodDecorator(
  target: any,                      // Prototype de la classe
  propertyKey: string,              // Nom de la méthode
  descriptor: PropertyDescriptor    // Descripteur de propriété
): PropertyDescriptor | void {
  // Modification du comportement
}
```

### 📊 Décorateur de mesure de performance

```typescript
function measureTime(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
  const originalMethod = descriptor.value;
  
  descriptor.value = async function(...args: any[]) {
    const start = performance.now();
    
    try {
      const result = await originalMethod.apply(this, args);
      const end = performance.now();
      const duration = (end - start).toFixed(2);
      
      console.log(`⏱️ ${propertyKey} took ${duration}ms`);
      return result;
    } catch (error) {
      const end = performance.now();
      const duration = (end - start).toFixed(2);
      
      console.log(`❌ ${propertyKey} failed after ${duration}ms`);
      throw error;
    }
  };
  
  return descriptor;
}

class DataService {
  @measureTime
  async fetchUsers(): Promise<any[]> {
    // Simuler un appel API
    await new Promise(resolve => setTimeout(resolve, 1000));
    return [{ id: 1, name: "John" }, { id: 2, name: "Jane" }];
  }
  
  @measureTime
  async processData(data: any[]): Promise<void> {
    await new Promise(resolve => setTimeout(resolve, 500));
    console.log(`Processed ${data.length} items`);
  }
}

// Utilisation
const service = new DataService();
await service.fetchUsers();
// Output: ⏱️ fetchUsers took 1003.45ms

await service.processData([1, 2, 3]);
// Output: 
// Processed 3 items
// ⏱️ processData took 502.67ms
```

### 🔄 Décorateur de retry automatique

```typescript
function retry(maxAttempts: number = 3, delayMs: number = 1000) {
  return function(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    const originalMethod = descriptor.value;
    
    descriptor.value = async function(...args: any[]) {
      let lastError: Error;
      
      for (let attempt = 1; attempt <= maxAttempts; attempt++) {
        try {
          console.log(`🔄 Tentative ${attempt}/${maxAttempts} - ${propertyKey}`);
          const result = await originalMethod.apply(this, args);
          console.log(`✅ Succès à la tentative ${attempt}`);
          return result;
        } catch (error) {
          lastError = error as Error;
          console.log(`❌ Échec tentative ${attempt}: ${error}`);
          
          if (attempt < maxAttempts) {
            console.log(`⏳ Attente de ${delayMs}ms avant retry...`);
            await new Promise(resolve => setTimeout(resolve, delayMs));
          }
        }
      }
      
      console.log(`💥 Échec après ${maxAttempts} tentatives`);
      throw lastError;
    };
    
    return descriptor;
  };
}

class ApiClient {
  private failCount = 0;
  
  @retry(3, 500)
  async fetchData(url: string): Promise<any> {
    // Simuler des échecs aléatoires
    this.failCount++;
    
    if (this.failCount < 3) {
      throw new Error("Network error");
    }
    
    return { data: "Success!" };
  }
}

// Utilisation
const client = new ApiClient();
try {
  const result = await client.fetchData("/api/data");
  console.log(result);
} catch (error) {
  console.error("Failed:", error);
}
// Output:
// 🔄 Tentative 1/3 - fetchData
// ❌ Échec tentative 1: Error: Network error
// ⏳ Attente de 500ms avant retry...
// 🔄 Tentative 2/3 - fetchData
// ❌ Échec tentative 2: Error: Network error
// ⏳ Attente de 500ms avant retry...
// 🔄 Tentative 3/3 - fetchData
// ✅ Succès à la tentative 3
// { data: 'Success!' }
```

### 💾 Décorateur de cache (mémoization)

```typescript
function memoize(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
  const originalMethod = descriptor.value;
  const cache = new Map<string, any>();
  
  descriptor.value = function(...args: any[]) {
    const key = JSON.stringify(args);
    
    if (cache.has(key)) {
      console.log(`💾 Cache hit for ${propertyKey}(${key})`);
      return cache.get(key);
    }
    
    console.log(`🔍 Cache miss for ${propertyKey}(${key})`);
    const result = originalMethod.apply(this, args);
    cache.set(key, result);
    
    return result;
  };
  
  return descriptor;
}

class Calculator {
  @memoize
  fibonacci(n: number): number {
    if (n <= 1) return n;
    return this.fibonacci(n - 1) + this.fibonacci(n - 2);
  }
  
  @memoize
  factorial(n: number): number {
    if (n <= 1) return 1;
    return n * this.factorial(n - 1);
  }
}

// Utilisation
const calc = new Calculator();

console.log("Premier appel:");
console.log(calc.fibonacci(10));  // Calcul complet

console.log("\nDeuxième appel:");
console.log(calc.fibonacci(10));  // Depuis le cache

console.log("\nFactorielle:");
console.log(calc.factorial(5));   // 120
console.log(calc.factorial(5));   // Cache hit
```

### 🔒 Décorateur de vérification de permissions

```typescript
function requireAuth(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
  const originalMethod = descriptor.value;
  
  descriptor.value = function(...args: any[]) {
    // Vérifier si l'utilisateur est authentifié
    const user = (this as any).currentUser;
    
    if (!user) {
      throw new Error("Authentication required");
    }
    
    console.log(`🔓 User ${user.name} authorized`);
    return originalMethod.apply(this, args);
  };
  
  return descriptor;
}

function requireRole(role: string) {
  return function(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    const originalMethod = descriptor.value;
    
    descriptor.value = function(...args: any[]) {
      const user = (this as any).currentUser;
      
      if (!user) {
        throw new Error("Authentication required");
      }
      
      if (user.role !== role) {
        throw new Error(`Role '${role}' required. Current role: '${user.role}'`);
      }
      
      console.log(`✅ User ${user.name} has role '${role}'`);
      return originalMethod.apply(this, args);
    };
    
    return descriptor;
  };
}

class UserService {
  currentUser: { name: string; role: string } | null = null;
  
  login(name: string, role: string): void {
    this.currentUser = { name, role };
    console.log(`👤 Logged in as ${name} (${role})`);
  }
  
  logout(): void {
    this.currentUser = null;
    console.log("👋 Logged out");
  }
  
  @requireAuth
  getProfile(): any {
    return { name: this.currentUser!.name, role: this.currentUser!.role };
  }
  
  @requireRole("admin")
  deleteUser(userId: string): void {
    console.log(`🗑️ User ${userId} deleted`);
  }
  
  @requireRole("admin")
  createUser(name: string): void {
    console.log(`➕ User ${name} created`);
  }
}

// Utilisation
const userService = new UserService();

try {
  userService.getProfile();  // ❌ Erreur: pas authentifié
} catch (error) {
  console.error(error.message);
}

userService.login("Alice", "user");
userService.getProfile();  // ✅ OK

try {
  userService.deleteUser("123");  // ❌ Erreur: pas admin
} catch (error) {
  console.error(error.message);
}

userService.logout();
userService.login("Bob", "admin");
userService.deleteUser("123");  // ✅ OK
```

---

## 5.3 Decorator Factories

### 🎯 Qu'est-ce qu'une Decorator Factory ?

Une **decorator factory** est une fonction qui **retourne un décorateur**. Elle permet de **paramétrer** les décorateurs.

```typescript
// Decorator factory : retourne un décorateur
function myDecorator(param: string) {
  // Retourne le vrai décorateur
  return function(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    console.log(`Paramètre reçu: ${param}`);
  };
}

class Example {
  @myDecorator("hello")  // Appel de la factory
  method() {}
}
```

### 📝 Logger configurable

```typescript
type LogLevel = "debug" | "info" | "warn" | "error";

function log(level: LogLevel = "info", includeArgs: boolean = true) {
  return function(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    const originalMethod = descriptor.value;
    
    descriptor.value = function(...args: any[]) {
      const timestamp = new Date().toISOString();
      const className = target.constructor.name;
      
      // Préparer le message
      let message = `[${timestamp}] [${level.toUpperCase()}] ${className}.${propertyKey}`;
      
      if (includeArgs && args.length > 0) {
        message += ` | Args: ${JSON.stringify(args)}`;
      }
      
      // Logger selon le niveau
      switch (level) {
        case "debug":
          console.debug(message);
          break;
        case "info":
          console.info(message);
          break;
        case "warn":
          console.warn(message);
          break;
        case "error":
          console.error(message);
          break;
      }
      
      // Exécuter la méthode
      const result = originalMethod.apply(this, args);
      
      console.log(`[${level.toUpperCase()}] Result:`, result);
      
      return result;
    };
    
    return descriptor;
  };
}

class PaymentService {
  @log("info", true)
  processPayment(amount: number, currency: string): string {
    return `Payment of ${amount} ${currency} processed`;
  }
  
  @log("warn", false)
  refundPayment(transactionId: string): void {
    console.log(`Refunding transaction ${transactionId}`);
  }
  
  @log("error", true)
  cancelPayment(transactionId: string): void {
    throw new Error("Payment already processed");
  }
}

// Utilisation
const payment = new PaymentService();

payment.processPayment(100, "EUR");
// [2024-01-15T10:30:00.000Z] [INFO] PaymentService.processPayment | Args: [100,"EUR"]
// [INFO] Result: Payment of 100 EUR processed

payment.refundPayment("TXN-123");
// [2024-01-15T10:30:01.000Z] [WARN] PaymentService.refundPayment
// Refunding transaction TXN-123
// [WARN] Result: undefined

try {
  payment.cancelPayment("TXN-456");
} catch (error) {
  // [2024-01-15T10:30:02.000Z] [ERROR] PaymentService.cancelPayment | Args: ["TXN-456"]
}
```

### ✅ Validateur de paramètres

```typescript
function validate(validations: { [paramIndex: number]: (value: any) => boolean }) {
  return function(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    const originalMethod = descriptor.value;
    
    descriptor.value = function(...args: any[]) {
      // Valider chaque paramètre
      for (const [index, validator] of Object.entries(validations)) {
        const paramIndex = parseInt(index);
        const value = args[paramIndex];
        
        if (!validator(value)) {
          throw new Error(
            `Validation failed for parameter ${paramIndex} of ${propertyKey}: ${value}`
          );
        }
      }
      
      return originalMethod.apply(this, args);
    };
    
    return descriptor;
  };
}

// Validators helpers
const validators = {
  isPositive: (value: number) => value > 0,
  isEmail: (value: string) => /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(value),
  isNotEmpty: (value: string) => value.length > 0,
  isInRange: (min: number, max: number) => (value: number) => value >= min && value <= max
};

class UserManager {
  @validate({
    0: validators.isNotEmpty,
    1: validators.isEmail,
    2: validators.isInRange(18, 120)
  })
  createUser(name: string, email: string, age: number): void {
    console.log(`✅ User created: ${name}, ${email}, ${age}`);
  }
  
  @validate({
    0: validators.isPositive
  })
  deleteUser(userId: number): void {
    console.log(`🗑️ User ${userId} deleted`);
  }
}

// Utilisation
const manager = new UserManager();

manager.createUser("Alice", "alice@example.com", 25);
// ✅ User created: Alice, alice@example.com, 25

try {
  manager.createUser("", "alice@example.com", 25);
} catch (error) {
  console.error(error.message);
  // Validation failed for parameter 0 of createUser: 
}

try {
  manager.createUser("Bob", "invalid-email", 25);
} catch (error) {
  console.error(error.message);
  // Validation failed for parameter 1 of createUser: invalid-email
}

try {
  manager.createUser("Charlie", "charlie@example.com", 150);
} catch (error) {
  console.error(error.message);
  // Validation failed for parameter 2 of createUser: 150
}
```

### ⏱️ Timeout configurable

```typescript
function timeout(ms: number) {
  return function(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    const originalMethod = descriptor.value;
    
    descriptor.value = async function(...args: any[]) {
      return Promise.race([
        originalMethod.apply(this, args),
        new Promise((_, reject) => {
          setTimeout(() => {
            reject(new Error(`${propertyKey} timeout after ${ms}ms`));
          }, ms);
        })
      ]);
    };
    
    return descriptor;
  };
}

class ApiService {
  @timeout(2000)
  async fetchData(url: string): Promise<any> {
    // Simuler un appel API lent
    await new Promise(resolve => setTimeout(resolve, 3000));
    return { data: "result" };
  }
  
  @timeout(1000)
  async quickFetch(url: string): Promise<any> {
    await new Promise(resolve => setTimeout(resolve, 500));
    return { data: "quick result" };
  }
}

// Utilisation
const api = new ApiService();

try {
  await api.fetchData("/api/slow");
} catch (error) {
  console.error(error.message);
  // fetchData timeout after 2000ms
}

const result = await api.quickFetch("/api/fast");
console.log(result);
// { data: "quick result" }
```

---

## 5.4 Décorateurs de classe

### 🎯 Signature d'un décorateur de classe

```typescript
function classDecorator<T extends { new(...args: any[]): {} }>(constructor: T) {
  // Modification ou extension de la classe
  return class extends constructor {
    // Nouvelles propriétés ou méthodes
  };
}
```

### 📊 Décorateur de logging de classe

```typescript
function logged(constructor: Function) {
  console.log(`📝 Classe ${constructor.name} créée`);
  
  // Logger l'instanciation
  const original = constructor;
  
  const newConstructor: any = function(...args: any[]) {
    console.log(`🆕 Nouvelle instance de ${original.name}`);
    console.log(`   Arguments:`, args);
    return new (original as any)(...args);
  };
  
  newConstructor.prototype = original.prototype;
  return newConstructor;
}

@logged
class User {
  constructor(public name: string, public age: number) {}
}

@logged
class Product {
  constructor(public name: string, public price: number) {}
}

// Utilisation
const user = new User("Alice", 30);
// 📝 Classe User créée
// 🆕 Nouvelle instance de User
//    Arguments: ["Alice", 30]

const product = new Product("Laptop", 999);
// 📝 Classe Product créée
// 🆕 Nouvelle instance de Product
//    Arguments: ["Laptop", 999]
```

### 🔒 Décorateur Singleton

```typescript
function singleton<T extends { new(...args: any[]): {} }>(constructor: T) {
  let instance: T | null = null;
  
  return class extends constructor {
    constructor(...args: any[]) {
      if (instance) {
        console.log("♻️ Retour de l'instance existante");
        return instance as any;
      }
      
      super(...args);
      instance = this as any;
      console.log("🆕 Nouvelle instance singleton créée");
    }
  };
}

@singleton
class DatabaseConnection {
  constructor(public connectionString: string) {
    console.log(`📡 Connexion à: ${connectionString}`);
  }
  
  query(sql: string): void {
    console.log(`🔍 Exécution: ${sql}`);
  }
}

// Utilisation
const db1 = new DatabaseConnection("postgresql://localhost/mydb");
// 🆕 Nouvelle instance singleton créée
// 📡 Connexion à: postgresql://localhost/mydb

const db2 = new DatabaseConnection("postgresql://localhost/otherdb");
// ♻️ Retour de l'instance existante

console.log(db1 === db2);  // true
```

### 🏷️ Décorateur d'ajout de métadonnées

```typescript
function entity(tableName: string) {
  return function<T extends { new(...args: any[]): {} }>(constructor: T) {
    return class extends constructor {
      static tableName = tableName;
      static entityType = "database";
      
      getTableName(): string {
        return tableName;
      }
    };
  };
}

@entity("users")
class User {
  constructor(public name: string, public email: string) {}
}

@entity("products")
class Product {
  constructor(public name: string, public price: number) {}
}

// Utilisation
const user = new User("Alice", "alice@example.com");
console.log((User as any).tableName);  // "users"
console.log((user as any).getTableName());  // "users"

const product = new Product("Laptop", 999);
console.log((Product as any).tableName);  // "products"
```

### 🎨 Décorateur d'injection de dépendances

```typescript
// Container d'injection de dépendances simple
class DIContainer {
  private static services = new Map<string, any>();
  
  static register(name: string, instance: any): void {
    this.services.set(name, instance);
  }
  
  static resolve(name: string): any {
    return this.services.get(name);
  }
}

function injectable(serviceName: string) {
  return function<T extends { new(...args: any[]): {} }>(constructor: T) {
    // Enregistrer dans le container
    const instance = new constructor();
    DIContainer.register(serviceName, instance);
    
    console.log(`💉 Service '${serviceName}' enregistré`);
    
    return constructor;
  };
}

function inject(serviceName: string) {
  return function<T extends { new(...args: any[]): {} }>(constructor: T) {
    return class extends constructor {
      constructor(...args: any[]) {
        super(...args);
        
        // Injecter les dépendances
        const service = DIContainer.resolve(serviceName);
        if (service) {
          (this as any)[serviceName] = service;
          console.log(`💉 Service '${serviceName}' injecté dans ${constructor.name}`);
        }
      }
    };
  };
}

// Services
@injectable("logger")
class Logger {
  log(message: string): void {
    console.log(`📝 [LOG] ${message}`);
  }
}

@injectable("database")
class Database {
  query(sql: string): void {
    console.log(`🔍 [DB] ${sql}`);
  }
}

// Consommateurs
@inject("logger")
@inject("database")
class UserService {
  logger!: Logger;
  database!: Database;
  
  createUser(name: string): void {
    this.logger.log(`Creating user: ${name}`);
    this.database.query(`INSERT INTO users (name) VALUES ('${name}')`);
  }
}

// Utilisation
const userService = new UserService();
// 💉 Service 'logger' enregistré
// 💉 Service 'database' enregistré
// 💉 Service 'logger' injecté dans UserService
// 💉 Service 'database' injecté dans UserService

userService.createUser("Alice");
// 📝 [LOG] Creating user: Alice
// 🔍 [DB] INSERT INTO users (name) VALUES ('Alice')
```

---

## 5.5 Décorateurs de propriété

### 🎯 Signature d'un décorateur de propriété

```typescript
function propertyDecorator(target: any, propertyKey: string) {
  // Modification ou annotation de la propriété
}
```

### 📝 Décorateur de propriété readonly

```typescript
function readonly(target: any, propertyKey: string) {
  let value: any;
  
  Object.defineProperty(target, propertyKey, {
    get() {
      return value;
    },
    set(newValue: any) {
      if (value !== undefined) {
        throw new Error(`Cannot modify readonly property '${propertyKey}'`);
      }
      value = newValue;
    },
    enumerable: true,
    configurable: true
  });
}

class Config {
  @readonly
  apiUrl: string = "https://api.example.com";
  
  @readonly
  apiKey: string = "secret-key-123";
  
  timeout: number = 5000;  // Modifiable
}

// Utilisation
const config = new Config();
console.log(config.apiUrl);  // "https://api.example.com"

config.timeout = 3000;  // ✅ OK

try {
  config.apiUrl = "https://new-api.com";  // ❌ Erreur
} catch (error) {
  console.error(error.message);
  // Cannot modify readonly property 'apiUrl'
}
```

### ✅ Décorateur de validation de propriété

```typescript
function minLength(length: number) {
  return function(target: any, propertyKey: string) {
    let value: string;
    
    Object.defineProperty(target, propertyKey, {
      get() {
        return value;
      },
      set(newValue: string) {
        if (newValue.length < length) {
          throw new Error(
            `${propertyKey} must be at least ${length} characters`
          );
        }
        value = newValue;
      },
      enumerable: true,
      configurable: true
    });
  };
}

function email(target: any, propertyKey: string) {
  let value: string;
  
  Object.defineProperty(target, propertyKey, {
    get() {
      return value;
    },
    set(newValue: string) {
      const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
      if (!emailRegex.test(newValue)) {
        throw new Error(`${propertyKey} must be a valid email`);
      }
      value = newValue;
    },
    enumerable: true,
    configurable: true
  });
}

function range(min: number, max: number) {
  return function(target: any, propertyKey: string) {
    let value: number;
    
    Object.defineProperty(target, propertyKey, {
      get() {
        return value;
      },
      set(newValue: number) {
        if (newValue < min || newValue > max) {
          throw new Error(
            `${propertyKey} must be between ${min} and ${max}`
          );
        }
        value = newValue;
      },
      enumerable: true,
      configurable: true
    });
  };
}

class User {
  @minLength(3)
  username!: string;
  
  @email
  emailAddress!: string;
  
  @range(18, 120)
  age!: number;
}

// Utilisation
const user = new User();

user.username = "Alice";  // ✅ OK
user.emailAddress = "alice@example.com";  // ✅ OK
user.age = 25;  // ✅ OK

try {
  user.username = "Al";  // ❌ Trop court
} catch (error) {
  console.error(error.message);
  // username must be at least 3 characters
}

try {
  user.emailAddress = "invalid-email";  // ❌ Format invalide
} catch (error) {
  console.error(error.message);
  // emailAddress must be a valid email
}

try {
  user.age = 150;  // ❌ Hors limites
} catch (error) {
  console.error(error.message);
  // age must be between 18 and 120
}
```

### 🏷️ Décorateur de format de propriété

```typescript
function uppercase(target: any, propertyKey: string) {
  let value: string;
  
  Object.defineProperty(target, propertyKey, {
    get() {
      return value;
    },
    set(newValue: string) {
      value = newValue.toUpperCase();
    },
    enumerable: true,
    configurable: true
  });
}

function trim(target: any, propertyKey: string) {
  let value: string;
  
  Object.defineProperty(target, propertyKey, {
    get() {
      return value;
    },
    set(newValue: string) {
      value = newValue.trim();
    },
    enumerable: true,
    configurable: true
  });
}

function sanitize(target: any, propertyKey: string) {
  let value: string;
  
  Object.defineProperty(target, propertyKey, {
    get() {
      return value;
    },
    set(newValue: string) {
      // Supprimer les caractères dangereux
      value = newValue.replace(/[<>]/g, "");
    },
    enumerable: true,
    configurable: true
  });
}

class FormData {
  @uppercase
  @trim
  countryCode!: string;
  
  @trim
  @sanitize
  comment!: string;
  
  @trim
  name!: string;
}

// Utilisation
const form = new FormData();

form.countryCode = "  fr  ";
console.log(form.countryCode);  // "FR"

form.comment = "  <script>alert('xss')</script>  ";
console.log(form.comment);  // "scriptalert('xss')/script"

form.name = "  Alice  ";
console.log(form.name);  // "Alice"
```

### 📊 Décorateur de tracking de modifications

```typescript
function track(target: any, propertyKey: string) {
  const historyKey = `_${propertyKey}_history`;
  
  Object.defineProperty(target, historyKey, {
    value: [],
    writable: true,
    enumerable: false
  });
  
  let value: any;
  
  Object.defineProperty(target, propertyKey, {
    get() {
      return value;
    },
    set(newValue: any) {
      const timestamp = new Date();
      const oldValue = value;
      
      (target as any)[historyKey].push({
        timestamp,
        oldValue,
        newValue
      });
      
      value = newValue;
      
      console.log(`📝 ${propertyKey} changed: ${oldValue} → ${newValue}`);
    },
    enumerable: true,
    configurable: true
  });
  
  // Méthode pour obtenir l'historique
  target[`get${propertyKey}History`] = function() {
    return (this as any)[historyKey];
  };
}

class Product {
  @track
  price!: number;
  
  @track
  stock!: number;
  
  name: string;
  
  constructor(name: string) {
    this.name = name;
  }
  
  getPriceHistory?: () => any[];
  getStockHistory?: () => any[];
}

// Utilisation
const product = new Product("Laptop");

product.price = 1000;
// 📝 price changed: undefined → 1000

product.price = 950;
// 📝 price changed: 1000 → 950

product.price = 900;
// 📝 price changed: 950 → 900

product.stock = 100;
// 📝 stock changed: undefined → 100

product.stock = 95;
// 📝 stock changed: 100 → 95

// Voir l'historique
console.log("Historique des prix:", product.getPriceHistory!());
// [
//   { timestamp: ..., oldValue: undefined, newValue: 1000 },
//   { timestamp: ..., oldValue: 1000, newValue: 950 },
//   { timestamp: ..., oldValue: 950, newValue: 900 }
// ]
```

---

## 📝 Exemple Complet : Système de Blog avec Décorateurs

```typescript
// ========================================
// Décorateurs
// ========================================

// Logging
function log(level: "info" | "warn" | "error" = "info") {
  return function(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    const originalMethod = descriptor.value;
    
    descriptor.value = async function(...args: any[]) {
      const timestamp = new Date().toISOString();
      console.log(`[${timestamp}] [${level.toUpperCase()}] ${propertyKey} called`);
      
      try {
        const result = await originalMethod.apply(this, args);
        console.log(`[${level.toUpperCase()}] ${propertyKey} completed`);
        return result;
      } catch (error) {
        console.error(`[ERROR] ${propertyKey} failed:`, error);
        throw error;
      }
    };
    
    return descriptor;
  };
}

// Mesure de performance
function measureTime(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
  const originalMethod = descriptor.value;
  
  descriptor.value = async function(...args: any[]) {
    const start = performance.now();
    const result = await originalMethod.apply(this, args);
    const end = performance.now();
    
    console.log(`⏱️ ${propertyKey} took ${(end - start).toFixed(2)}ms`);
    
    return result;
  };
  
  return descriptor;
}

// Vérification de permissions
function requireAuth(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
  const originalMethod = descriptor.value;
  
  descriptor.value = function(...args: any[]) {
    if (!(this as any).currentUser) {
      throw new Error("Authentication required");
    }
    return originalMethod.apply(this, args);
  };
  
  return descriptor;
}

function requireRole(role: string) {
  return function(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    const originalMethod = descriptor.value;
    
    descriptor.value = function(...args: any[]) {
      const user = (this as any).currentUser;
      
      if (!user || user.role !== role) {
        throw new Error(`Role '${role}' required`);
      }
      
      return originalMethod.apply(this, args);
    };
    
    return descriptor;
  };
}

// Cache
function cache(ttl: number = 60000) {
  const cacheStore = new Map<string, { value: any; expiry: number }>();
  
  return function(target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    const originalMethod = descriptor.value;
    
    descriptor.value = async function(...args: any[]) {
      const key = `${propertyKey}_${JSON.stringify(args)}`;
      const cached = cacheStore.get(key);
      
      if (cached && Date.now() < cached.expiry) {
        console.log(`💾 Cache hit for ${propertyKey}`);
        return cached.value;
      }
      
      console.log(`🔍 Cache miss for ${propertyKey}`);
      const result = await originalMethod.apply(this, args);
      
      cacheStore.set(key, {
        value: result,
        expiry: Date.now() + ttl
      });
      
      return result;
    };
    
    return descriptor;
  };
}

// Validation de propriété
function required(target: any, propertyKey: string) {
  let value: any;
  
  Object.defineProperty(target, propertyKey, {
    get() {
      return value;
    },
    set(newValue: any) {
      if (newValue === null || newValue === undefined || newValue === "") {
        throw new Error(`${propertyKey} is required`);
      }
      value = newValue;
    },
    enumerable: true,
    configurable: true
  });
}

// ========================================
// Modèles
// ========================================

interface User {
  id: string;
  name: string;
  email: string;
  role: "admin" | "editor" | "viewer";
}

interface Post {
  id: string;
  title: string;
  content: string;
  authorId: string;
  published: boolean;
  createdAt: Date;
  updatedAt: Date;
}

// ========================================
// Services
// ========================================

class BlogService {
  currentUser: User | null = null;
  private posts: Map<string, Post> = new Map();
  
  login(user: User): void {
    this.currentUser = user;
    console.log(`👤 Logged in as ${user.name} (${user.role})`);
  }
  
  logout(): void {
    this.currentUser = null;
    console.log("👋 Logged out");
  }
  
  @log("info")
  @measureTime
  @requireAuth
  @requireRole("admin")
  async createPost(title: string, content: string): Promise<Post> {
    // Simuler un appel async
    await new Promise(resolve => setTimeout(resolve, 100));
    
    const post: Post = {
      id: `post-${Date.now()}`,
      title,
      content,
      authorId: this.currentUser!.id,
      published: false,
      createdAt: new Date(),
      updatedAt: new Date()
    };
    
    this.posts.set(post.id, post);
    console.log(`✅ Post created: ${post.id}`);
    
    return post;
  }
  
  @log("info")
  @measureTime
  @cache(30000)
  async getPost(id: string): Promise<Post | null> {
    await new Promise(resolve => setTimeout(resolve, 50));
    return this.posts.get(id) || null;
  }
  
  @log("info")
  @measureTime
  @cache(10000)
  async getAllPosts(): Promise<Post[]> {
    await new Promise(resolve => setTimeout(resolve, 200));
    return Array.from(this.posts.values());
  }
  
  @log("warn")
  @requireAuth
  @requireRole("admin")
  async deletePost(id: string): Promise<boolean> {
    const deleted = this.posts.delete(id);
    
    if (deleted) {
      console.log(`🗑️ Post ${id} deleted`);
    }
    
    return deleted;
  }
  
  @log("info")
  @requireAuth
  async updatePost(id: string, updates: Partial<Post>): Promise<Post | null> {
    const post = this.posts.get(id);
    
    if (!post) return null;
    
    // Vérifier les permissions
    if (this.currentUser!.role !== "admin" && post.authorId !== this.currentUser!.id) {
      throw new Error("Cannot update other users' posts");
    }
    
    const updated = {
      ...post,
      ...updates,
      updatedAt: new Date()
    };
    
    this.posts.set(id, updated);
    console.log(`✏️ Post ${id} updated`);
    
    return updated;
  }
  
  @log("info")
  @requireAuth
  @requireRole("admin")
  async publishPost(id: string): Promise<Post | null> {
    return this.updatePost(id, { published: true });
  }
}

// ========================================
// Utilisation
// ========================================

async function demo() {
  const blog = new BlogService();
  
  // Utilisateur admin
  const admin: User = {
    id: "user-1",
    name: "Admin",
    email: "admin@example.com",
    role: "admin"
  };
  
  // Utilisateur éditeur
  const editor: User = {
    id: "user-2",
    name: "Editor",
    email: "editor@example.com",
    role: "editor"
  };
  
  console.log("\n=== Test 1: Création sans authentification ===");
  try {
    await blog.createPost("Test", "Content");
  } catch (error) {
    console.error("❌", error.message);
  }
  
  console.log("\n=== Test 2: Création avec admin ===");
  blog.login(admin);
  const post1 = await blog.createPost("Introduction à TypeScript", "TypeScript est...");
  
  console.log("\n=== Test 3: Lecture avec cache ===");
  await blog.getPost(post1.id);  // Cache miss
  await blog.getPost(post1.id);  // Cache hit
  
  console.log("\n=== Test 4: Liste avec cache ===");
  await blog.getAllPosts();  // Cache miss
  await blog.getAllPosts();  // Cache hit
  
  console.log("\n=== Test 5: Publication ===");
  await blog.publishPost(post1.id);
  
  console.log("\n=== Test 6: Éditeur essaie de supprimer ===");
  blog.logout();
  blog.login(editor);
  try {
    await blog.deletePost(post1.id);
  } catch (error) {
    console.error("❌", error.message);
  }
  
  console.log("\n=== Test 7: Admin supprime ===");
  blog.logout();
  blog.login(admin);
  await blog.deletePost(post1.id);
}

// Exécuter la démo
demo();
```

---

## 🎯 Points Clés pour l'Entretien

### Concepts Fondamentaux
1. **Décorateur** : fonction qui modifie le comportement
2. **Configuration** : `experimentalDecorators: true`
3. **5 types** : classe, méthode, propriété, accesseur, paramètre
4. **Decorator factory** : fonction qui retourne un décorateur

### Types de Décorateurs

**Méthode** :
- Signature : `(target, propertyKey, descriptor)`
- Cas d'usage : logging, cache, retry, validation

**Classe** :
- Signature : `(constructor)`
- Cas d'usage : singleton, métadonnées, injection

**Propriété** :
- Signature : `(target, propertyKey)`
- Cas d'usage : validation, formatting, readonly

### Applications Pratiques
1. **Logging** : tracer l'exécution
2. **Performance** : mesurer le temps
3. **Cache** : mémoization
4. **Authentification** : vérifier les permissions
5. **Validation** : valider les données
6. **Retry** : réessayer en cas d'échec

---

## ⚠️ Pièges à Éviter

### Piège 1 : Oublier la configuration

```typescript
// ❌ Sans configuration
// Les décorateurs ne fonctionnent pas !

// ✅ tsconfig.json
{
  "compilerOptions": {
    "experimentalDecorators": true
  }
}
```

### Piège 2 : Ordre des décorateurs

```typescript
// Les décorateurs s'exécutent de bas en haut
class Example {
  @first
  @second
  @third
  method() {}
}

// Exécution : third → second → first
```

### Piège 3 : Perte du contexte `this`

```typescript
// ❌ Mauvais
function decorator(target: any, key: string, descriptor: PropertyDescriptor) {
  const original = descriptor.value;
  descriptor.value = function(...args: any[]) {
    return original(args);  // ❌ Perd 'this'
  };
}

// ✅ Bon
function decorator(target: any, key: string, descriptor: PropertyDescriptor) {
  const original = descriptor.value;
  descriptor.value = function(...args: any[]) {
    return original.apply(this, args);  // ✅ Garde 'this'
  };
}
```

### Piège 4 : Décorateur de propriété ne peut pas modifier la valeur

```typescript
// ❌ Ne fonctionne pas
function init(value: any) {
  return function(target: any, propertyKey: string) {
    target[propertyKey] = value;  // Ne marche pas !
  };
}

// ✅ Utiliser Object.defineProperty
function init(value: any) {
  return function(target: any, propertyKey: string) {
    let val = value;
    Object.defineProperty(target, propertyKey, {
      get() { return val; },
      set(newVal) { val = newVal; }
    });
  };
}
```

### Piège 5 : Async/await avec décorateurs

```typescript
// ❌ Oublier async/await
function decorator(target: any, key: string, descriptor: PropertyDescriptor) {
  const original = descriptor.value;
  descriptor.value = function(...args: any[]) {
    const result = original.apply(this, args);  // ❌ Promise non attendue
    return result;
  };
}

// ✅ Gérer async correctement
function decorator(target: any, key: string, descriptor: PropertyDescriptor) {
  const original = descriptor.value;
  descriptor.value = async function(...args: any[]) {
    const result = await original.apply(this, args);  // ✅ Attend la Promise
    return result;
  };
}
```

---

## 📝 Checklist de Révision

### Concepts de Base
- [ ] Je comprends ce qu'est un décorateur
- [ ] Je connais la configuration requise
- [ ] Je connais les 5 types de décorateurs
- [ ] Je comprends l'ordre d'exécution

### Décorateurs de Méthode
- [ ] Je sais créer un décorateur de logging
- [ ] Je peux implémenter un cache
- [ ] Je maîtrise le retry automatique
- [ ] Je sais vérifier les permissions

### Decorator Factories
- [ ] Je comprends le concept de factory
- [ ] Je peux créer des décorateurs paramétrables
- [ ] Je sais combiner plusieurs décorateurs

### Décorateurs de Classe
- [ ] Je peux créer un singleton
- [ ] Je sais ajouter des métadonnées
- [ ] Je comprends l'injection de dépendances

### Décorateurs de Propriété
- [ ] Je peux créer des validateurs
- [ ] Je sais implémenter readonly
- [ ] Je peux formater les valeurs
- [ ] Je maîtrise le tracking

---

## 💡 Exercices Pratiques

### Exercice 1 : Système de Rate Limiting

Créez un décorateur `@rateLimit(maxCalls, windowMs)` qui limite le nombre d'appels d'une méthode.

```typescript
class ApiClient {
  @rateLimit(3, 60000)  // 3 appels max par minute
  async callApi(endpoint: string): Promise<any> {
    return fetch(endpoint);
  }
}
```

### Exercice 2 : Validation Complète

Créez des décorateurs de validation pour une classe User :
- `@minLength(n)` pour le nom
- `@email` pour l'email
- `@range(min, max)` pour l'âge

### Exercice 3 : Transaction Database

Créez un décorateur `@transactional` qui :
- Démarre une transaction
- Exécute la méthode
- Commit si succès, rollback si erreur

---

## 🔗 Ressources Complémentaires

### Documentation Officielle
- **Decorators** : https://www.typescriptlang.org/docs/handbook/decorators.html
- **Metadata Reflection** : https://github.com/rbuckton/reflect-metadata

### Frameworks Utilisant des Décorateurs
- **Angular** : @Component, @Injectable, @Input
- **NestJS** : @Controller, @Get, @Post
- **TypeORM** : @Entity, @Column, @PrimaryKey
- **class-validator** : @IsString, @IsEmail, @Min

### Articles et Tutoriels
- "Decorators in TypeScript"
- "Advanced Decorator Patterns"
- "Metadata with Decorators"

---

## 📊 Résumé du Chapitre 5

Ce chapitre a couvert les **décorateurs** en TypeScript :

### Types de Décorateurs
✅ **Méthode** : modifier le comportement des méthodes
✅ **Classe** : modifier ou étendre les classes
✅ **Propriété** : valider et formater les propriétés
✅ **Factories** : créer des décorateurs paramétrables

### Applications Pratiques
✅ **Logging** : tracer l'exécution du code
✅ **Performance** : mesurer les temps d'exécution
✅ **Cache** : mémoization automatique
✅ **Auth** : vérifier permissions et rôles
✅ **Validation** : valider les données
✅ **Retry** : réessayer automatiquement

### Patterns Avancés
✅ **Singleton** : une seule instance
✅ **Injection de dépendances** : DI container
✅ **Tracking** : historique des modifications
✅ **Transaction** : gestion transactionnelle

---

## 🎓 Pour l'Entretien

### Questions Fréquentes

**Q1 : Qu'est-ce qu'un décorateur ?**
> Une fonction qui modifie ou annote une classe, méthode ou propriété sans modifier son code source. Permet d'ajouter des comportements de manière déclarative.

**Q2 : Quelle configuration est nécessaire ?**
> `"experimentalDecorators": true` dans tsconfig.json. Optionnel : `"emitDecoratorMetadata": true` pour métadonnées.

**Q3 : Dans quel ordre s'exécutent les décorateurs ?**
> De bas en haut pour les décorateurs empilés. L'ordre d'évaluation des factories est de haut en bas, mais l'exécution des décorateurs retournés est de bas en haut.

**Q4 : Différence entre décorateur et decorator factory ?**
> Une factory est une fonction qui retourne un décorateur. Elle permet de paramétrer le décorateur : `@log("info")` vs `@log`.

**Q5 : Cas d'usage principaux ?**
> Logging, caching, validation, authentification, mesure de performance, retry, métadonnées (Angular, NestJS).

---