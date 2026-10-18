## [**..**](./00_index.md)

# Chapitre 12 : Programmation Orientée Objet (OOP)

## 12.1 Prototypes

JavaScript utilise un système de **prototypes** plutôt que l'héritage classique (comme Java).

### Prototype Chain (Chaîne de Prototypes)

```javascript
// Chaque objet a un prototype (lien vers un autre objet)
const person = {
    name: 'John',
    greet: function() {
        console.log(`Hello, I'm ${this.name}`);
    }
};

// Créer un objet qui hérite de person
const student = Object.create(person);
student.name = 'Alice';
student.study = function() {
    console.log(`${this.name} is studying`);
};

student.greet();  // "Hello, I'm Alice" (méthode héritée)
student.study();  // "Alice is studying" (méthode propre)

// Vérifier le prototype
console.log(Object.getPrototypeOf(student) === person); // true

// La chaîne de prototypes
console.log(student.toString()); // [object Object]
// student -> person -> Object.prototype -> null
```

### `__proto__` vs `prototype`

```javascript
// __proto__ - Le prototype d'une instance (lien vers le prototype)
// prototype - Propriété des fonctions constructeurs

function Person(name) {
    this.name = name;
}

Person.prototype.greet = function() {
    console.log(`Hello, I'm ${this.name}`);
};

const john = new Person('John');

// __proto__ de l'instance pointe vers Person.prototype
console.log(john.__proto__ === Person.prototype); // true

// Person.prototype a lui-même un __proto__ vers Object.prototype
console.log(Person.prototype.__proto__ === Object.prototype); // true

// Chaîne complète:
// john -> Person.prototype -> Object.prototype -> null

// Ajouter une méthode au prototype
Person.prototype.sayAge = function(age) {
    console.log(`${this.name} is ${age} years old`);
};

john.sayAge(30); // "John is 30 years old"

// IMPORTANT: Ne pas modifier __proto__ directement (déprécié)
// ❌ john.__proto__ = something
// ✅ Utiliser Object.setPrototypeOf() ou Object.create()
```

### Object.create()

```javascript
// Object.create() crée un objet avec le prototype spécifié

const animal = {
    type: 'Animal',
    makeSound: function() {
        console.log('Some sound');
    }
};

// Créer un objet qui hérite de animal
const dog = Object.create(animal);
dog.breed = 'Labrador';
dog.makeSound = function() {
    console.log('Woof!');
};

console.log(dog.type);      // "Animal" (hérité)
console.log(dog.breed);     // "Labrador" (propre)
dog.makeSound();            // "Woof!" (overridden)

// Créer objet avec propriétés spécifiques
const cat = Object.create(animal, {
    breed: {
        value: 'Siamese',
        writable: true,
        enumerable: true,
        configurable: true
    },
    age: {
        value: 3,
        writable: true
    }
});

console.log(cat.breed); // "Siamese"
console.log(cat.age);   // 3

// Créer objet sans prototype (objet vraiment vide)
const emptyObj = Object.create(null);
console.log(emptyObj.toString); // undefined (pas de prototype)

// Utilisation pratique: héritage
const vehicle = {
    start: function() {
        console.log(`${this.type} starting...`);
    }
};

const car = Object.create(vehicle);
car.type = 'Car';
car.drive = function() {
    console.log('Driving...');
};

car.start(); // "Car starting..."
car.drive(); // "Driving..."
```

### Vérifications de Prototype

```javascript
function Person(name) {
    this.name = name;
}

const john = new Person('John');

// hasOwnProperty() - vérifie si propriété appartient à l'objet (pas hérité)
console.log(john.hasOwnProperty('name')); // true
console.log(john.hasOwnProperty('toString')); // false (hérité)

// in operator - vérifie si propriété existe (propre ou héritée)
console.log('name' in john);      // true
console.log('toString' in john);  // true (hérité)

// isPrototypeOf() - vérifie si objet est dans la chaîne de prototypes
console.log(Person.prototype.isPrototypeOf(john)); // true
console.log(Object.prototype.isPrototypeOf(john)); // true

// instanceof - vérifie si objet est instance d'un constructeur
console.log(john instanceof Person); // true
console.log(john instanceof Object); // true

// Object.getPrototypeOf() - obtenir le prototype
console.log(Object.getPrototypeOf(john) === Person.prototype); // true

// Itérer sur propriétés propres uniquement
for (let key in john) {
    if (john.hasOwnProperty(key)) {
        console.log(key, john[key]);
    }
}

// Object.keys() - retourne uniquement propriétés propres
console.log(Object.keys(john)); // ["name"]
```

---

## 12.2 Classes ES6

Les **classes** ES6 sont du sucre syntaxique sur les prototypes (syntaxe plus claire).

### Syntaxe de Base

```javascript
// Déclaration de classe
class Person {
    // Constructor - appelé lors de la création
    constructor(name, age) {
        this.name = name;
        this.age = age;
    }
    
    // Méthodes (ajoutées au prototype)
    greet() {
        console.log(`Hello, I'm ${this.name}`);
    }
    
    getInfo() {
        return `${this.name}, ${this.age} years old`;
    }
}

// Créer une instance
const john = new Person('John', 30);
john.greet(); // "Hello, I'm John"
console.log(john.getInfo()); // "John, 30 years old"

// Vérifications
console.log(john instanceof Person); // true
console.log(typeof Person); // "function" (classe = fonction!)

// Expression de classe (moins courant)
const Animal = class {
    constructor(type) {
        this.type = type;
    }
};

const dog = new Animal('Dog');
```

### Constructor

```javascript
class User {
    constructor(username, email) {
        // Initialiser les propriétés
        this.username = username;
        this.email = email;
        this.createdAt = new Date();
        
        // Validation dans le constructor
        if (!email.includes('@')) {
            throw new Error('Invalid email');
        }
    }
    
    getProfile() {
        return {
            username: this.username,
            email: this.email,
            memberSince: this.createdAt
        };
    }
}

const user = new User('john_doe', 'john@example.com');
console.log(user.getProfile());

// Erreur si email invalide
// const badUser = new User('test', 'invalid'); // Error: Invalid email

// Constructor par défaut (si non spécifié)
class SimpleClass {
    // Pas de constructor = constructor vide par défaut
    sayHi() {
        console.log('Hi!');
    }
}
```

### Méthodes et Propriétés

```javascript
class Rectangle {
    constructor(width, height) {
        this.width = width;
        this.height = height;
    }
    
    // Méthode d'instance
    getArea() {
        return this.width * this.height;
    }
    
    // Méthode d'instance
    getPerimeter() {
        return 2 * (this.width + this.height);
    }
    
    // Méthode qui retourne une nouvelle instance
    scale(factor) {
        return new Rectangle(this.width * factor, this.height * factor);
    }
    
    // Méthode toString
    toString() {
        return `Rectangle(${this.width}x${this.height})`;
    }
}

const rect = new Rectangle(10, 5);
console.log(rect.getArea());      // 50
console.log(rect.getPerimeter()); // 30

const bigRect = rect.scale(2);
console.log(bigRect.toString());  // "Rectangle(20x10)"

// Propriétés calculées
class Circle {
    constructor(radius) {
        this.radius = radius;
    }
    
    // Méthode qui calcule l'aire
    getArea() {
        return Math.PI * this.radius ** 2;
    }
}

const circle = new Circle(5);
console.log(circle.getArea()); // ~78.54
```

### Getters et Setters

```javascript
// Getters et Setters permettent de contrôler l'accès aux propriétés

class Temperature {
    constructor(celsius) {
        this._celsius = celsius; // Convention: _ pour propriété privée
    }
    
    // Getter - accès comme une propriété
    get celsius() {
        return this._celsius;
    }
    
    // Setter - modification comme une propriété
    set celsius(value) {
        if (value < -273.15) {
            throw new Error('Temperature below absolute zero!');
        }
        this._celsius = value;
    }
    
    // Getter pour fahrenheit (conversion)
    get fahrenheit() {
        return this._celsius * 9/5 + 32;
    }
    
    // Setter pour fahrenheit
    set fahrenheit(value) {
        this._celsius = (value - 32) * 5/9;
    }
}

const temp = new Temperature(25);

// Utiliser comme propriétés (pas de parenthèses)
console.log(temp.celsius);    // 25
console.log(temp.fahrenheit); // 77

temp.celsius = 30;
console.log(temp.fahrenheit); // 86

temp.fahrenheit = 32;
console.log(temp.celsius);    // 0

// temp.celsius = -300; // Error: Temperature below absolute zero!

// Exemple pratique: Validation
class User {
    constructor(email) {
        this.email = email; // Utilise le setter
    }
    
    get email() {
        return this._email;
    }
    
    set email(value) {
        if (!value.includes('@')) {
            throw new Error('Invalid email');
        }
        this._email = value;
    }
}

const user = new User('john@example.com');
console.log(user.email); // "john@example.com"

// user.email = 'invalid'; // Error: Invalid email

// Getter sans setter (read-only)
class Person {
    constructor(firstName, lastName) {
        this.firstName = firstName;
        this.lastName = lastName;
    }
    
    get fullName() {
        return `${this.firstName} ${this.lastName}`;
    }
    
    // Pas de setter = propriété en lecture seule
}

const person = new Person('John', 'Doe');
console.log(person.fullName); // "John Doe"
// person.fullName = 'Jane Smith'; // Ne fait rien (strict mode: erreur)
```

### Static Methods (Méthodes Statiques)

```javascript
// Static methods appartiennent à la classe, pas aux instances

class MathHelper {
    // Méthode statique
    static add(a, b) {
        return a + b;
    }
    
    static multiply(a, b) {
        return a * b;
    }
    
    static PI = 3.14159; // Propriété statique (ES2022)
}

// Appeler sans créer d'instance
console.log(MathHelper.add(5, 3));      // 8
console.log(MathHelper.multiply(4, 2)); // 8
console.log(MathHelper.PI);             // 3.14159

// ❌ Pas accessible depuis instance
const helper = new MathHelper();
// console.log(helper.add(1, 2)); // undefined

// Exemple pratique: Factory method
class User {
    constructor(name, email) {
        this.name = name;
        this.email = email;
    }
    
    // Méthode d'instance
    getInfo() {
        return `${this.name} (${this.email})`;
    }
    
    // Méthode statique - créer instance de différentes façons
    static createFromObject(obj) {
        return new User(obj.name, obj.email);
    }
    
    static createGuest() {
        return new User('Guest', 'guest@example.com');
    }
    
    static compare(user1, user2) {
        return user1.name.localeCompare(user2.name);
    }
}

// Utiliser les méthodes statiques
const user1 = User.createFromObject({ name: 'John', email: 'john@test.com' });
const guest = User.createGuest();

console.log(user1.getInfo()); // "John (john@test.com)"
console.log(guest.getInfo()); // "Guest (guest@example.com)"

// Comparer des utilisateurs
const user2 = new User('Alice', 'alice@test.com');
console.log(User.compare(user1, user2)); // 1 (John > Alice)

// Exemple: Compteur d'instances
class Counter {
    static count = 0; // Propriété statique
    
    constructor(name) {
        this.name = name;
        Counter.count++; // Incrémenter à chaque création
    }
    
    static getCount() {
        return Counter.count;
    }
    
    static reset() {
        Counter.count = 0;
    }
}

const c1 = new Counter('First');
const c2 = new Counter('Second');
const c3 = new Counter('Third');

console.log(Counter.getCount()); // 3
Counter.reset();
console.log(Counter.getCount()); // 0
```

---

## 12.3 Héritage

L'héritage permet à une classe d'hériter des propriétés et méthodes d'une autre.

### `extends` Keyword

```javascript
// Classe parent (superclass)
class Animal {
    constructor(name) {
        this.name = name;
    }
    
    makeSound() {
        console.log(`${this.name} makes a sound`);
    }
    
    sleep() {
        console.log(`${this.name} is sleeping`);
    }
}

// Classe enfant (subclass) hérite de Animal
class Dog extends Animal {
    constructor(name, breed) {
        super(name); // Appeler constructor parent (OBLIGATOIRE)
        this.breed = breed;
    }
    
    // Méthode spécifique à Dog
    bark() {
        console.log(`${this.name} barks: Woof!`);
    }
}

const dog = new Dog('Rex', 'Labrador');
dog.makeSound(); // "Rex makes a sound" (hérité)
dog.sleep();     // "Rex is sleeping" (hérité)
dog.bark();      // "Rex barks: Woof!" (propre)

console.log(dog instanceof Dog);    // true
console.log(dog instanceof Animal); // true

// Autre exemple
class Cat extends Animal {
    constructor(name, indoor) {
        super(name);
        this.indoor = indoor;
    }
    
    meow() {
        console.log(`${this.name} meows: Meow!`);
    }
}

const cat = new Cat('Whiskers', true);
cat.makeSound(); // "Whiskers makes a sound"
cat.meow();      // "Whiskers meows: Meow!"
```

### `super` Keyword

```javascript
// super() - appeler constructor parent
// super.method() - appeler méthode parent

class Vehicle {
    constructor(brand, model) {
        this.brand = brand;
        this.model = model;
    }
    
    getInfo() {
        return `${this.brand} ${this.model}`;
    }
    
    start() {
        console.log('Vehicle starting...');
    }
}

class Car extends Vehicle {
    constructor(brand, model, doors) {
        super(brand, model); // Appeler constructor parent (OBLIGATOIRE avant this)
        this.doors = doors;
    }
    
    // Override méthode parent
    getInfo() {
        // Appeler méthode parent avec super
        const baseInfo = super.getInfo();
        return `${baseInfo} (${this.doors} doors)`;
    }
    
    start() {
        super.start(); // Appeler méthode parent
        console.log('Car engine started');
    }
}

const car = new Car('Toyota', 'Corolla', 4);
console.log(car.getInfo()); 
// "Toyota Corolla (4 doors)"

car.start();
// "Vehicle starting..."
// "Car engine started"

// IMPORTANT: super() doit être appelé avant d'utiliser this
class Bad extends Vehicle {
    constructor(brand, model) {
        this.color = 'red'; // ❌ Error: Must call super() first
        super(brand, model);
    }
}

// Héritage à plusieurs niveaux
class ElectricCar extends Car {
    constructor(brand, model, doors, batteryCapacity) {
        super(brand, model, doors);
        this.batteryCapacity = batteryCapacity;
    }
    
    getInfo() {
        const carInfo = super.getInfo();
        return `${carInfo}, Battery: ${this.batteryCapacity}kWh`;
    }
    
    charge() {
        console.log('Charging battery...');
    }
}

const tesla = new ElectricCar('Tesla', 'Model 3', 4, 75);
console.log(tesla.getInfo());
// "Tesla Model 3 (4 doors), Battery: 75kWh"

tesla.start(); // Hérité de Car
tesla.charge(); // Propre à ElectricCar
```

### Method Overriding (Redéfinition de Méthodes)

```javascript
// Override (redéfinir) une méthode héritée

class Shape {
    constructor(color) {
        this.color = color;
    }
    
    draw() {
        console.log(`Drawing a ${this.color} shape`);
    }
    
    getArea() {
        return 0; // Méthode par défaut
    }
}

class Circle extends Shape {
    constructor(color, radius) {
        super(color);
        this.radius = radius;
    }
    
    // Override draw()
    draw() {
        console.log(`Drawing a ${this.color} circle with radius ${this.radius}`);
    }
    
    // Override getArea()
    getArea() {
        return Math.PI * this.radius ** 2;
    }
}

class Rectangle extends Shape {
    constructor(color, width, height) {
        super(color);
        this.width = width;
        this.height = height;
    }
    
    // Override draw()
    draw() {
        console.log(`Drawing a ${this.color} rectangle ${this.width}x${this.height}`);
    }
    
    // Override getArea()
    getArea() {
        return this.width * this.height;
    }
}

const circle = new Circle('red', 5);
circle.draw();                  // "Drawing a red circle with radius 5"
console.log(circle.getArea());  // ~78.54

const rect = new Rectangle('blue', 10, 5);
rect.draw();                    // "Drawing a blue rectangle 10x5"
console.log(rect.getArea());    // 50

// Polymorphisme - traiter différents objets de la même manière
const shapes = [
    new Circle('red', 5),
    new Rectangle('blue', 10, 5),
    new Circle('green', 3)
];

shapes.forEach(shape => {
    shape.draw();
    console.log(`Area: ${shape.getArea()}`);
});
```

### Héritage de Classes Built-in

```javascript
// Hériter de classes JavaScript natives

// Exemple 1: Étendre Array
class MyArray extends Array {
    // Ajouter méthode custom
    first() {
        return this[0];
    }
    
    last() {
        return this[this.length - 1];
    }
    
    // Override push pour logger
    push(...items) {
        console.log(`Adding ${items.length} item(s)`);
        return super.push(...items);
    }
}

const arr = new MyArray(1, 2, 3);
console.log(arr.first()); // 1
console.log(arr.last());  // 3

arr.push(4, 5); // "Adding 2 item(s)"
console.log(arr); // [1, 2, 3, 4, 5]

// Toutes les méthodes Array disponibles
console.log(arr.map(x => x * 2)); // [2, 4, 6, 8, 10]

// Exemple 2: Étendre Error
class ValidationError extends Error {
    constructor(message, field) {
        super(message);
        this.name = 'ValidationError';
        this.field = field;
    }
}

function validateEmail(email) {
    if (!email.includes('@')) {
        throw new ValidationError('Invalid email format', 'email');
    }
}

try {
    validateEmail('invalid');
} catch (error) {
    if (error instanceof ValidationError) {
        console.log(`Validation error in field "${error.field}": ${error.message}`);
    }
}

// Exemple 3: Étendre Map
class DefaultMap extends Map {
    constructor(defaultValue) {
        super();
        this.defaultValue = defaultValue;
    }
    
    // Override get pour retourner valeur par défaut
    get(key) {
        if (!this.has(key)) {
            return this.defaultValue;
        }
        return super.get(key);
    }
}

const map = new DefaultMap('N/A');
map.set('name', 'John');

console.log(map.get('name'));  // "John"
console.log(map.get('age'));   // "N/A" (valeur par défaut)
```

---

## 12.4 Encapsulation

L'encapsulation permet de cacher les détails d'implémentation et de protéger les données.

### Private Fields (Champs Privés) `#`

```javascript
// Private fields (ES2022) - préfixés par #

class BankAccount {
    // Private fields
    #balance = 0;
    #pin;
    
    constructor(owner, initialBalance, pin) {
        this.owner = owner;  // Public
        this.#balance = initialBalance; // Private
        this.#pin = pin; // Private
    }
    
    // Méthode publique pour accéder au solde
    getBalance(enteredPin) {
        if (this.#validatePin(enteredPin)) {
            return this.#balance;
        }
        throw new Error('Invalid PIN');
    }
    
    // Méthode publique pour déposer
    deposit(amount) {
        if (amount > 0) {
            this.#balance += amount;
            return true;
        }
        return false;
    }
    
    // Méthode publique pour retirer
    withdraw(amount, enteredPin) {
        if (!this.#validatePin(enteredPin)) {
            throw new Error('Invalid PIN');
        }
        
        if (amount > 0 && amount <= this.#balance) {
            this.#balance -= amount;
            return true;
        }
        return false;
    }
    
    // Private method
    #validatePin(enteredPin) {
        return enteredPin === this.#pin;
    }
}

const account = new BankAccount('John', 1000, '1234');

account.deposit(500);
console.log(account.getBalance('1234')); // 1500

account.withdraw(200, '1234');
console.log(account.getBalance('1234')); // 1300

// ❌ Impossible d'accéder aux champs privés
// console.log(account.#balance); // SyntaxError
// console.log(account.#pin);     // SyntaxError
// account.#validatePin('1234');  // SyntaxError

// Les propriétés privées n'apparaissent pas
console.log(Object.keys(account)); // ["owner"]
```

### Private Methods (Méthodes Privées)

```javascript
class User {
    #password;
    
    constructor(username, password) {
        this.username = username;
        this.#password = this.#hashPassword(password);
    }
    
    // Private method
    #hashPassword(password) {
        // Simulation de hashage simple
        return btoa(password);
    }
    
    // Private method
    #validatePassword(password) {
        return this.#hashPassword(password) === this.#password;
    }
    
    // Public method
    login(password) {
        if (this.#validatePassword(password)) {
            console.log('Login successful');
            return true;
        }
        console.log('Invalid password');
        return false;
    }
    
    // Public method
    changePassword(oldPassword, newPassword) {
        if (this.#validatePassword(oldPassword)) {
            this.#password = this.#hashPassword(newPassword);
            console.log('Password changed');
            return true;
        }
        console.log('Invalid old password');
        return false;
    }
}

const user = new User('john_doe', 'secret123');

user.login('secret123');      // "Login successful"
user.login('wrong');          // "Invalid password"

user.changePassword('secret123', 'newpass456'); // "Password changed"
user.login('newpass456');     // "Login successful"

// ❌ Méthodes privées inaccessibles
// user.#hashPassword('test'); // SyntaxError
```

### Convention `_` (Pseudo-Private)

```javascript
// Avant ES2022, convention: préfixer par _ pour indiquer "privé"
// ⚠️ Ce n'est qu'une convention, pas vraiment privé!

class OldStylePrivate {
    constructor(value) {
        this._value = value; // Convention: "ne pas toucher"
    }
    
    getValue() {
        return this._value;
    }
    
    _internalMethod() {
        console.log('Internal method');
    }
}

const obj = new OldStylePrivate(42);
console.log(obj.getValue()); // 42

// ⚠️ Toujours accessible (pas vraiment privé)
console.log(obj._value); // 42
obj._internalMethod();   // "Internal method"

// Préférer # pour vrai private
class NewStylePrivate {
    #value;
    
    constructor(value) {
        this.#value = value;
    }
    
    getValue() {
        return this.#value;
    }
}

const obj2 = new NewStylePrivate(42);
console.log(obj2.getValue()); // 42
// console.log(obj2.#value); // SyntaxError - vraiment privé!
```

### Getters/Setters pour Encapsulation

```javascript
class Product {
    #price;
    #stock;
    
    constructor(name, price, stock) {
        this.name = name;
        this.#price = price;
        this.#stock = stock;
    }
    
    // Getter public pour price
    get price() {
        return this.#price;
    }
    
    // Setter avec validation
    set price(value) {
        if (value < 0) {
            throw new Error('Price cannot be negative');
        }
        this.#price = value;
    }
    
    // Getter pour stock
    get stock() {
        return this.#stock;
    }
    
    // Pas de setter direct pour stock
    // Utiliser méthodes spécifiques
    
    addStock(quantity) {
        if (quantity > 0) {
            this.#stock += quantity;
        }
    }
    
    removeStock(quantity) {
        if (quantity > 0 && quantity <= this.#stock) {
            this.#stock -= quantity;
            return true;
        }
        return false;
    }
    
    // Propriété calculée en lecture seule
    get totalValue() {
        return this.#price * this.#stock;
    }
}

const product = new Product('Laptop', 999, 50);

console.log(product.price);      // 999
console.log(product.stock);      // 50
console.log(product.totalValue); // 49950

product.price = 899;
console.log(product.price);      // 899

product.addStock(10);
console.log(product.stock);      // 60

// product.price = -100; // Error: Price cannot be negative
// product.#stock = 0;   // SyntaxError: private field
```

### Exemple Complet: Classe Encapsulée

```javascript
class ShoppingCart {
    #items = [];
    #discount = 0;
    
    constructor(userId) {
        this.userId = userId;
        this.#items = [];
    }
    
    // Ajouter un item
    addItem(product, quantity = 1) {
        const existingItem = this.#findItem(product.id);
        
        if (existingItem) {
            existingItem.quantity += quantity;
        } else {
            this.#items.push({
                product: product,
                quantity: quantity
            });
        }
    }
    
    // Retirer un item
    removeItem(productId) {
        const index = this.#items.findIndex(item => item.product.id === productId);
        if (index !== -1) {
            this.#items.splice(index, 1);
            return true;
        }
        return false;
    }
    
    // Mettre à jour quantité
    updateQuantity(productId, quantity) {
        const item = this.#findItem(productId);
        if (item && quantity > 0) {
            item.quantity = quantity;
            return true;
        }
        return false;
    }
    
    // Appliquer réduction
    applyDiscount(percentage) {
        if (percentage >= 0 && percentage <= 100) {
            this.#discount = percentage;
        }
    }
    
    // Private method - trouver un item
    #findItem(productId) {
        return this.#items.find(item => item.product.id === productId);
    }
    
    // Private method - calculer sous-total
    #calculateSubtotal() {
        return this.#items.reduce((sum, item) => {
            return sum + (item.product.price * item.quantity);
        }, 0);
    }
    
    // Getter public pour le total
    get total() {
        const subtotal = this.#calculateSubtotal();
        const discountAmount = subtotal * (this.#discount / 100);
        return subtotal - discountAmount;
    }
    
    // Getter pour nombre d'items
    get itemCount() {
        return this.#items.reduce((sum, item) => sum + item.quantity, 0);
    }
    
    // Obtenir résumé (public)
    getSummary() {
        return {
            userId: this.userId,
            itemCount: this.itemCount,
            subtotal: this.#calculateSubtotal(),
            discount: this.#discount,
            total: this.total
        };
    }
    
    // Obtenir liste des items (copie pour éviter modification externe)
    getItems() {
        return this.#items.map(item => ({
            product: { ...item.product },
            quantity: item.quantity
        }));
    }
    
    // Vider le panier
    clear() {
        this.#items = [];
        this.#discount = 0;
    }
}

// Utilisation
const cart = new ShoppingCart('user123');

cart.addItem({ id: 1, name: 'Laptop', price: 999 }, 1);
cart.addItem({ id: 2, name: 'Mouse', price: 29 }, 2);

console.log(cart.itemCount); // 3
console.log(cart.total);     // 1057

cart.applyDiscount(10); // 10% de réduction
console.log(cart.total); // 951.3

console.log(cart.getSummary());
// {
//   userId: 'user123',
//   itemCount: 3,
//   subtotal: 1057,
//   discount: 10,
//   total: 951.3
// }

// ❌ Impossible d'accéder directement aux items privés
// console.log(cart.#items); // SyntaxError

// ✅ Mais on peut obtenir une copie via méthode publique
const items = cart.getItems();
console.log(items);
```

---

## Exemples Pratiques Complets

### Exemple 1: Système de Gestion d'Utilisateurs

```javascript
class User {
    static #users = []; // Private static field
    static #nextId = 1;
    
    #id;
    #password;
    #createdAt;
    
    constructor(username, email, password) {
        this.#id = User.#nextId++;
        this.username = username;
        this.email = email;
        this.#password = this.#hashPassword(password);
        this.#createdAt = new Date();
        
        User.#users.push(this);
    }
    
    // Private method - hasher le mot de passe
    #hashPassword(password) {
        // Simulation simple (en production: utiliser bcrypt)
        return btoa(password);
    }
    
    // Vérifier mot de passe
    verifyPassword(password) {
        return this.#hashPassword(password) === this.#password;
    }
    
    // Changer mot de passe
    changePassword(oldPassword, newPassword) {
        if (this.verifyPassword(oldPassword)) {
            this.#password = this.#hashPassword(newPassword);
            return true;
        }
        return false;
    }
    
    // Getter pour ID (read-only)
    get id() {
        return this.#id;
    }
    
    // Getter pour date création (read-only)
    get createdAt() {
        return new Date(this.#createdAt);
    }
    
    // Obtenir profil public
    getPublicProfile() {
        return {
            id: this.#id,
            username: this.username,
            email: this.email,
            memberSince: this.#createdAt
        };
    }
    
    // Static method - trouver utilisateur par ID
    static findById(id) {
        return User.#users.find(user => user.id === id);
    }
    
    // Static method - trouver par email
    static findByEmail(email) {
        return User.#users.find(user => user.email === email);
    }
    
    // Static method - tous les utilisateurs
    static getAllUsers() {
        return User.#users.map(user => user.getPublicProfile());
    }
    
    // Static method - compter utilisateurs
    static count() {
        return User.#users.length;
    }
}

// Utilisation
const user1 = new User('john_doe', 'john@example.com', 'secret123');
const user2 = new User('jane_smith', 'jane@example.com', 'pass456');

console.log(user1.getPublicProfile());
// { id: 1, username: 'john_doe', email: 'john@example.com', memberSince: ... }

console.log(user1.verifyPassword('secret123')); // true
console.log(user1.verifyPassword('wrong'));     // false

user1.changePassword('secret123', 'newpass789');
console.log(user1.verifyPassword('newpass789')); // true

// Méthodes statiques
console.log(User.count()); // 2

const found = User.findByEmail('jane@example.com');
console.log(found.username); // "jane_smith"

console.log(User.getAllUsers());
// [
//   { id: 1, username: 'john_doe', ... },
//   { id: 2, username: 'jane_smith', ... }
// ]
```

### Exemple 2: Hiérarchie de Formes Géométriques

```javascript
// Classe abstraite (simulée)
class Shape {
    constructor(color) {
        if (new.target === Shape) {
            throw new Error('Shape is abstract and cannot be instantiated');
        }
        this.color = color;
    }
    
    // Méthode abstraite (doit être overridden)
    getArea() {
        throw new Error('getArea() must be implemented');
    }
    
    // Méthode abstraite
    getPerimeter() {
        throw new Error('getPerimeter() must be implemented');
    }
    
    // Méthode concrète
    describe() {
        return `A ${this.color} ${this.constructor.name}`;
    }
}

class Circle extends Shape {
    constructor(color, radius) {
        super(color);
        this.radius = radius;
    }
    
    getArea() {
        return Math.PI * this.radius ** 2;
    }
    
    getPerimeter() {
        return 2 * Math.PI * this.radius;
    }
}

class Rectangle extends Shape {
    constructor(color, width, height) {
        super(color);
        this.width = width;
        this.height = height;
    }
    
    getArea() {
        return this.width * this.height;
    }
    
    getPerimeter() {
        return 2 * (this.width + this.height);
    }
    
    // Méthode spécifique
    isSquare() {
        return this.width === this.height;
    }
}

class Triangle extends Shape {
    constructor(color, base, height, side1, side2) {
        super(color);
        this.base = base;
        this.height = height;
        this.side1 = side1;
        this.side2 = side2;
    }
    
    getArea() {
        return (this.base * this.height) / 2;
    }
    
    getPerimeter() {
        return this.base + this.side1 + this.side2;
    }
}

// Utilisation
// const shape = new Shape('red'); // Error: Shape is abstract

const circle = new Circle('red', 5);
console.log(circle.describe());      // "A red Circle"
console.log(circle.getArea());       // ~78.54
console.log(circle.getPerimeter());  // ~31.42

const rect = new Rectangle('blue', 10, 5);
console.log(rect.describe());        // "A blue Rectangle"
console.log(rect.getArea());         // 50
console.log(rect.isSquare());        // false

// Polymorphisme - traiter tous comme Shape
const shapes = [
    new Circle('red', 5),
    new Rectangle('blue', 10, 5),
    new Triangle('green', 6, 4, 5, 5)
];

shapes.forEach(shape => {
    console.log(`${shape.describe()}:`);
    console.log(`  Area: ${shape.getArea().toFixed(2)}`);
    console.log(`  Perimeter: ${shape.getPerimeter().toFixed(2)}`);
});
```

### Exemple 3: Système de Comptes Bancaires

```javascript
// Classe de base
class BankAccount {
    static #accountCounter = 1000;
    
    #accountNumber;
    #balance;
    #transactions = [];
    
    constructor(owner, initialBalance = 0) {
        this.#accountNumber = BankAccount.#accountCounter++;
        this.owner = owner;
        this.#balance = initialBalance;
        this.#addTransaction('OPEN', initialBalance, 'Account opened');
    }
    
    // Private method - ajouter transaction
    #addTransaction(type, amount, description) {
        this.#transactions.push({
            type,
            amount,
            description,
            date: new Date(),
            balanceAfter: this.#balance
        });
    }
    
    get accountNumber() {
        return this.#accountNumber;
    }
    
    get balance() {
        return this.#balance;
    }
    
    // Déposer
    deposit(amount, description = 'Deposit') {
        if (amount <= 0) {
            throw new Error('Amount must be positive');
        }
        
        this.#balance += amount;
        this.#addTransaction('DEPOSIT', amount, description);
        return this.#balance;
    }
    
    // Retirer
    withdraw(amount, description = 'Withdrawal') {
        if (amount <= 0) {
            throw new Error('Amount must be positive');
        }
        
        if (amount > this.#balance) {
            throw new Error('Insufficient funds');
        }
        
        this.#balance -= amount;
        this.#addTransaction('WITHDRAWAL', -amount, description);
        return this.#balance;
    }
    
    // Historique des transactions
    getTransactionHistory(limit = 10) {
        return this.#transactions
            .slice(-limit)
            .map(t => ({ ...t })); // Copie pour éviter modification
    }
    
    // Résumé du compte
    getAccountSummary() {
        return {
            accountNumber: this.#accountNumber,
            owner: this.owner,
            balance: this.#balance,
            transactionCount: this.#transactions.length
        };
    }
}

// Compte Épargne (hérite de BankAccount)
class SavingsAccount extends BankAccount {
    #interestRate;
    
    constructor(owner, initialBalance, interestRate) {
        super(owner, initialBalance);
        this.#interestRate = interestRate;
    }
    
    get interestRate() {
        return this.#interestRate;
    }
    
    // Calculer et ajouter intérêts
    addInterest() {
        const interest = this.balance * (this.#interestRate / 100);
        this.deposit(interest, `Interest at ${this.#interestRate}%`);
        return interest;
    }
    
    // Override getAccountSummary
    getAccountSummary() {
        return {
            ...super.getAccountSummary(),
            type: 'Savings',
            interestRate: this.#interestRate
        };
    }
}

// Compte Courant (hérite de BankAccount)
class CheckingAccount extends BankAccount {
    #overdraftLimit;
    
    constructor(owner, initialBalance, overdraftLimit = 0) {
        super(owner, initialBalance);
        this.#overdraftLimit = overdraftLimit;
    }
    
    get overdraftLimit() {
        return this.#overdraftLimit;
    }
    
    // Override withdraw pour permettre découvert
    withdraw(amount, description = 'Withdrawal') {
        if (amount <= 0) {
            throw new Error('Amount must be positive');
        }
        
        const availableFunds = this.balance + this.#overdraftLimit;
        if (amount > availableFunds) {
            throw new Error('Exceeds overdraft limit');
        }
        
        return super.withdraw(amount, description);
    }
    
    getAccountSummary() {
        return {
            ...super.getAccountSummary(),
            type: 'Checking',
            overdraftLimit: this.#overdraftLimit,
            availableBalance: this.balance + this.#overdraftLimit
        };
    }
}

// Utilisation
const savings = new SavingsAccount('John Doe', 1000, 2.5);
console.log(savings.getAccountSummary());
// { accountNumber: 1000, owner: 'John Doe', balance: 1000, 
//   transactionCount: 1, type: 'Savings', interestRate: 2.5 }

savings.deposit(500);
console.log(savings.balance); // 1500

const interest = savings.addInterest();
console.log(`Interest added: $${interest.toFixed(2)}`); // $37.50
console.log(savings.balance); // 1537.50

const checking = new CheckingAccount('Jane Smith', 500, 200);
checking.withdraw(600); // OK (utilise découvert)
console.log(checking.balance); // -100

console.log(checking.getAccountSummary());
// { ..., type: 'Checking', overdraftLimit: 200, availableBalance: 100 }

// checking.withdraw(150); // Error: Exceeds overdraft limit

// Historique
console.log(savings.getTransactionHistory(3));
```

### Exemple 4: Gestion de Panier E-commerce

```javascript
class Product {
    static #products = new Map();
    
    constructor(id, name, price, stock) {
        this.id = id;
        this.name = name;
        this.price = price;
        this.stock = stock;
        
        Product.#products.set(id, this);
    }
    
    static getById(id) {
        return Product.#products.get(id);
    }
    
    isAvailable(quantity = 1) {
        return this.stock >= quantity;
    }
    
    reduceStock(quantity) {
        if (this.isAvailable(quantity)) {
            this.stock -= quantity;
            return true;
        }
        return false;
    }
    
    increaseStock(quantity) {
        this.stock += quantity;
    }
}

class CartItem {
    #product;
    #quantity;
    
    constructor(product, quantity = 1) {
        this.#product = product;
        this.#quantity = quantity;
    }
    
    get product() {
        return this.#product;
    }
    
    get quantity() {
        return this.#quantity;
    }
    
    set quantity(value) {
        if (value > 0) {
            this.#quantity = value;
        }
    }
    
    get subtotal() {
        return this.#product.price * this.#quantity;
    }
    
    toJSON() {
        return {
            product: {
                id: this.#product.id,
                name: this.#product.name,
                price: this.#product.price
            },
            quantity: this.#quantity,
            subtotal: this.subtotal
        };
    }
}

class ShoppingCart {
    #items = new Map();
    #coupon = null;
    
    addItem(productId, quantity = 1) {
        const product = Product.getById(productId);
        
        if (!product) {
            throw new Error('Product not found');
        }
        
        if (!product.isAvailable(quantity)) {
            throw new Error('Insufficient stock');
        }
        
        if (this.#items.has(productId)) {
            const item = this.#items.get(productId);
            item.quantity += quantity;
        } else {
            this.#items.set(productId, new CartItem(product, quantity));
        }
    }
    
    removeItem(productId) {
        return this.#items.delete(productId);
    }
    
    updateQuantity(productId, quantity) {
        const item = this.#items.get(productId);
        if (item) {
            if (quantity <= 0) {
                this.removeItem(productId);
            } else {
                item.quantity = quantity;
            }
            return true;
        }
        return false;
    }
    
    applyCoupon(coupon) {
        this.#coupon = coupon;
    }
    
    get subtotal() {
        let total = 0;
        for (const item of this.#items.values()) {
            total += item.subtotal;
        }
        return total;
    }
    
    get discount() {
        if (!this.#coupon) return 0;
        return this.subtotal * (this.#coupon.percentage / 100);
    }
    
    get total() {
        return this.subtotal - this.discount;
    }
    
    get itemCount() {
        let count = 0;
        for (const item of this.#items.values()) {
            count += item.quantity;
        }
        return count;
    }
    
    getSummary() {
        return {
            items: Array.from(this.#items.values()).map(item => item.toJSON()),
            itemCount: this.itemCount,
            subtotal: this.subtotal,
            discount: this.discount,
            coupon: this.#coupon?.code || null,
            total: this.total
        };
    }
    
    clear() {
        this.#items.clear();
        this.#coupon = null;
    }
    
    checkout() {
        // Vérifier stock et réduire
        for (const item of this.#items.values()) {
            if (!item.product.reduceStock(item.quantity)) {
                throw new Error(`Insufficient stock for ${item.product.name}`);
            }
        }
        
        const summary = this.getSummary();
        this.clear();
        return summary;
    }
}

// Utilisation
const laptop = new Product(1, 'Laptop', 999, 10);
const mouse = new Product(2, 'Mouse', 29, 50);
const keyboard = new Product(3, 'Keyboard', 79, 30);

const cart = new ShoppingCart();

cart.addItem(1, 1); // 1 Laptop
cart.addItem(2, 2); // 2 Mice
cart.addItem(3, 1); // 1 Keyboard

console.log(cart.getSummary());
// {
//   items: [...],
//   itemCount: 4,
//   subtotal: 1136,
//   discount: 0,
//   coupon: null,
//   total: 1136
// }

// Appliquer coupon
cart.applyCoupon({ code: 'SAVE10', percentage: 10 });
console.log(cart.total); // 1022.4

// Checkout
const order = cart.checkout();
console.log('Order placed:', order);
console.log('Laptop stock:', laptop.stock); // 9
```

---

**Points clés à retenir pour React:**
- Les classes sont moins utilisées en React moderne (préférer functional components + hooks)
- Mais comprendre OOP est essentiel pour:
  - Lire du legacy code (class components)
  - Comprendre les librairies tierces
  - Architecture globale de l'application
- Encapsulation = principes similaires avec custom hooks
- Héritage = composition préférée en React


---

# Questions/Réponses - Chapitre 12 : Programmation Orientée Objet

## Questions Niveau Junior

### Q1: Quelle est la différence entre une classe et une instance ?

**Réponse:**
```javascript
// CLASSE = Blueprint (modèle, plan)
// C'est la définition de la structure et du comportement

class Car {
    constructor(brand, model) {
        this.brand = brand;
        this.model = model;
    }
    
    drive() {
        console.log(`${this.brand} ${this.model} is driving`);
    }
}

// INSTANCE = Objet créé à partir de la classe
// C'est un exemplaire concret de la classe

const car1 = new Car('Toyota', 'Corolla'); // Instance 1
const car2 = new Car('Honda', 'Civic');    // Instance 2

// Chaque instance a ses propres données
console.log(car1.brand); // "Toyota"
console.log(car2.brand); // "Honda"

// Mais partage les méthodes (via prototype)
car1.drive(); // "Toyota Corolla is driving"
car2.drive(); // "Honda Civic is driving"

// Vérification
console.log(car1 instanceof Car); // true
console.log(car2 instanceof Car); // true
console.log(car1 === car2);       // false (instances différentes)

// ANALOGIE:
// Classe = recette de gâteau
// Instance = gâteau réel que vous cuisinez
```

**Points clés:**
- Classe = définition/template
- Instance = objet créé avec `new`
- Chaque instance a ses propres propriétés
- Les méthodes sont partagées via le prototype
- `instanceof` pour vérifier le type

---

### Q2: Qu'est-ce qu'un `constructor` et à quoi sert-il ?

**Réponse:**
```javascript
// Constructor = méthode spéciale appelée lors de la création (new)
// Sert à INITIALISER les propriétés de l'instance

class User {
    // Constructor s'exécute automatiquement avec 'new'
    constructor(username, email) {
        console.log('Constructor called!');
        
        // Initialiser les propriétés
        this.username = username;
        this.email = email;
        this.createdAt = new Date();
        
        // Validation possible
        if (!email.includes('@')) {
            throw new Error('Invalid email');
        }
    }
    
    getInfo() {
        return `${this.username} (${this.email})`;
    }
}

// new User() appelle automatiquement le constructor
const user1 = new User('john_doe', 'john@example.com');
// Console: "Constructor called!"

console.log(user1.username);  // "john_doe"
console.log(user1.createdAt); // Date actuelle

// Si pas de constructor, JavaScript en crée un vide par défaut
class SimpleClass {
    // Pas de constructor = constructor() {} implicite
    
    sayHi() {
        console.log('Hi!');
    }
}

const obj = new SimpleClass(); // Fonctionne quand même

// Constructor avec valeurs par défaut
class Product {
    constructor(name, price = 0, stock = 0) {
        this.name = name;
        this.price = price;
        this.stock = stock;
    }
}

const product1 = new Product('Laptop', 999, 10);
const product2 = new Product('Mouse'); // price et stock = 0

console.log(product2.price); // 0
console.log(product2.stock); // 0

// ❌ ERREUR COMMUNE: Oublier 'new'
const badUser = User('test', 'test@test.com'); // TypeError
// ✅ CORRECT: Toujours utiliser 'new'
const goodUser = new User('test', 'test@test.com');
```

**Points clés:**
- Constructor = méthode d'initialisation
- Appelé automatiquement avec `new`
- Sert à définir les propriétés initiales
- Peut valider les données
- Toujours utiliser `new` pour créer une instance

---

### Q3: Comment créer une méthode dans une classe ?

**Réponse:**
```javascript
class Calculator {
    constructor(initialValue = 0) {
        this.value = initialValue;
    }
    
    // MÉTHODE D'INSTANCE - accessible sur chaque instance
    add(number) {
        this.value += number;
        return this; // Return this pour chaining
    }
    
    subtract(number) {
        this.value -= number;
        return this;
    }
    
    multiply(number) {
        this.value *= number;
        return this;
    }
    
    // Méthode qui retourne une valeur
    getResult() {
        return this.value;
    }
    
    // Méthode avec logique
    isPositive() {
        return this.value > 0;
    }
}

const calc = new Calculator(10);

// Appeler les méthodes
calc.add(5);
console.log(calc.value); // 15

calc.subtract(3);
console.log(calc.value); // 12

// Method chaining (grâce au return this)
calc.add(10).multiply(2).subtract(4);
console.log(calc.getResult()); // 40

// Méthodes vs Propriétés
class Person {
    constructor(firstName, lastName) {
        this.firstName = firstName;
        this.lastName = lastName;
    }
    
    // Méthode - nécessite des parenthèses ()
    getFullName() {
        return `${this.firstName} ${this.lastName}`;
    }
    
    // Méthode avec paramètre
    greet(greeting = 'Hello') {
        return `${greeting}, I'm ${this.getFullName()}`;
    }
}

const person = new Person('John', 'Doe');
console.log(person.getFullName());     // "John Doe" (méthode)
console.log(person.greet());           // "Hello, I'm John Doe"
console.log(person.greet('Bonjour'));  // "Bonjour, I'm John Doe"

// ❌ ERREUR COMMUNE: Oublier les parenthèses
console.log(person.getFullName);   // [Function] (référence à la fonction)
// ✅ CORRECT
console.log(person.getFullName()); // "John Doe" (appel de la fonction)

// Les méthodes sont ajoutées au prototype (partagées)
console.log(Person.prototype.getFullName); // [Function: getFullName]
```

**Points clés:**
- Méthodes = fonctions dans une classe
- Définies sans `function` keyword
- Accessible via `instance.methode()`
- `this` réfère à l'instance
- Return `this` pour method chaining

---

### Q4: Qu'est-ce que `this` dans une classe ?

**Réponse:**
```javascript
// 'this' réfère à l'INSTANCE courante de la classe

class Counter {
    constructor(initialValue = 0) {
        this.value = initialValue; // this = instance créée
    }
    
    increment() {
        this.value++; // this = l'instance qui appelle increment
        console.log(this.value);
    }
    
    decrement() {
        this.value--;
        console.log(this.value);
    }
    
    getValue() {
        return this.value; // this = l'instance
    }
}

const counter1 = new Counter(0);
const counter2 = new Counter(10);

counter1.increment(); // this = counter1, affiche 1
counter2.increment(); // this = counter2, affiche 11

// Chaque instance a son propre 'this'
console.log(counter1.getValue()); // 1
console.log(counter2.getValue()); // 11

// Exemple avec propriétés multiples
class Person {
    constructor(name, age) {
        this.name = name;  // this.name de CETTE instance
        this.age = age;    // this.age de CETTE instance
    }
    
    introduce() {
        // this réfère à l'instance courante
        console.log(`Hi, I'm ${this.name} and I'm ${this.age} years old`);
    }
    
    haveBirthday() {
        this.age++; // Incrémenter l'âge de cette instance
        console.log(`${this.name} is now ${this.age}`);
    }
}

const john = new Person('John', 30);
const jane = new Person('Jane', 25);

john.introduce(); // "Hi, I'm John and I'm 30 years old"
jane.introduce(); // "Hi, I'm Jane and I'm 25 years old"

john.haveBirthday(); // "John is now 31"
console.log(jane.age); // 25 (pas affecté)

// ⚠️ PROBLÈME: Perte de contexte 'this'
class Button {
    constructor(label) {
        this.label = label;
    }
    
    click() {
        console.log(`${this.label} clicked`);
    }
}

const btn = new Button('Submit');
btn.click(); // "Submit clicked" ✅

// Problème avec callback
const clickHandler = btn.click;
// clickHandler(); // Error: this.label is undefined ❌

// SOLUTIONS:

// Solution 1: bind
const boundClick = btn.click.bind(btn);
boundClick(); // "Submit clicked" ✅

// Solution 2: Arrow function dans constructor
class BetterButton {
    constructor(label) {
        this.label = label;
        
        // Arrow function capture le 'this' du constructor
        this.click = () => {
            console.log(`${this.label} clicked`);
        };
    }
}

const btn2 = new BetterButton('Submit');
const handler = btn2.click;
handler(); // "Submit clicked" ✅

// Solution 3: Arrow function pour la méthode (proposal)
class ModernButton {
    constructor(label) {
        this.label = label;
    }
    
    click = () => {
        console.log(`${this.label} clicked`);
    }
}
```

**Points clés:**
- `this` = l'instance courante
- Chaque instance a son propre contexte `this`
- Attention à la perte de contexte dans callbacks
- Solutions: `.bind()`, arrow functions
- Important pour event handlers en React

---

### Q5: Comment utiliser `extends` pour créer une classe enfant ?

**Réponse:**
```javascript
// 'extends' permet l'héritage entre classes

// CLASSE PARENT (superclass)
class Animal {
    constructor(name) {
        this.name = name;
    }
    
    makeSound() {
        console.log(`${this.name} makes a sound`);
    }
    
    sleep() {
        console.log(`${this.name} is sleeping`);
    }
}

// CLASSE ENFANT (subclass) hérite de Animal
class Dog extends Animal {
    constructor(name, breed) {
        super(name); // Appeler constructor parent (OBLIGATOIRE)
        this.breed = breed;
    }
    
    // Nouvelle méthode spécifique à Dog
    bark() {
        console.log(`${this.name} barks: Woof!`);
    }
    
    // Override (redéfinir) méthode parent
    makeSound() {
        console.log(`${this.name} barks loudly!`);
    }
}

const dog = new Dog('Rex', 'Labrador');

// Méthodes héritées
dog.sleep();     // "Rex is sleeping" (de Animal)

// Méthodes propres
dog.bark();      // "Rex barks: Woof!" (de Dog)

// Méthode overridden
dog.makeSound(); // "Rex barks loudly!" (de Dog, pas Animal)

// Vérifications
console.log(dog instanceof Dog);    // true
console.log(dog instanceof Animal); // true
console.log(dog instanceof Object); // true

// Autre exemple: héritage de Vehicle
class Vehicle {
    constructor(brand, model) {
        this.brand = brand;
        this.model = model;
    }
    
    start() {
        console.log(`${this.brand} ${this.model} is starting`);
    }
    
    getInfo() {
        return `${this.brand} ${this.model}`;
    }
}

class Car extends Vehicle {
    constructor(brand, model, doors) {
        super(brand, model); // Appeler constructor parent
        this.doors = doors;
    }
    
    // Ajouter nouvelle méthode
    honk() {
        console.log('Beep beep!');
    }
    
    // Override avec extension
    getInfo() {
        const baseInfo = super.getInfo(); // Appeler méthode parent
        return `${baseInfo} (${this.doors} doors)`;
    }
}

const car = new Car('Toyota', 'Corolla', 4);
car.start();  // "Toyota Corolla is starting" (hérité)
car.honk();   // "Beep beep!" (propre)
console.log(car.getInfo()); // "Toyota Corolla (4 doors)"

// Héritage multiple niveaux
class ElectricCar extends Car {
    constructor(brand, model, doors, batterySize) {
        super(brand, model, doors);
        this.batterySize = batterySize;
    }
    
    charge() {
        console.log(`Charging ${this.batterySize}kWh battery`);
    }
}

const tesla = new ElectricCar('Tesla', 'Model 3', 4, 75);
tesla.start();  // De Vehicle
tesla.honk();   // De Car
tesla.charge(); // De ElectricCar

console.log(tesla instanceof ElectricCar); // true
console.log(tesla instanceof Car);         // true
console.log(tesla instanceof Vehicle);     // true

// ❌ ERREUR COMMUNE: Oublier super()
class BadDog extends Animal {
    constructor(name, breed) {
        // this.breed = breed; // ❌ Error: Must call super() first
        this.breed = breed;
    }
}

// ✅ CORRECT: super() en premier
class GoodDog extends Animal {
    constructor(name, breed) {
        super(name);        // ✅ super() d'abord
        this.breed = breed; // ✅ puis this
    }
}
```

**Points clés:**
- `extends` = créer une classe qui hérite
- `super()` = appeler constructor parent (obligatoire)
- Classe enfant hérite propriétés + méthodes
- Peut ajouter nouvelles méthodes
- Peut override (redéfinir) méthodes parent

---

## Questions Niveau Intermédiaire

### Q6: Quelle est la différence entre méthode d'instance et méthode statique ?

**Réponse:**
```javascript
class User {
    // Propriété statique (partagée par toutes les instances)
    static userCount = 0;
    
    constructor(username) {
        this.username = username;
        User.userCount++; // Incrémenter compteur statique
    }
    
    // MÉTHODE D'INSTANCE - appelée sur une instance
    getInfo() {
        return `Username: ${this.username}`;
    }
    
    // MÉTHODE STATIQUE - appelée sur la classe
    static getTotalUsers() {
        return User.userCount;
    }
    
    // Méthode statique - factory pattern
    static createAdmin(username) {
        const admin = new User(username);
        admin.isAdmin = true;
        return admin;
    }
    
    // Méthode statique - utilitaire
    static validateUsername(username) {
        return username.length >= 3 && username.length <= 20;
    }
}

// MÉTHODE D'INSTANCE - besoin d'une instance
const user1 = new User('john');
const user2 = new User('jane');

console.log(user1.getInfo()); // "Username: john" ✅
console.log(user2.getInfo()); // "Username: jane" ✅

// ❌ Impossible d'appeler méthode d'instance sur la classe
// console.log(User.getInfo()); // Error

// MÉTHODE STATIQUE - appelée sur la classe
console.log(User.getTotalUsers()); // 2 ✅

// ❌ Impossible d'appeler méthode statique sur instance
// console.log(user1.getTotalUsers()); // undefined

// Utilisation des méthodes statiques

// Factory method
const admin = User.createAdmin('admin_user');
console.log(admin.username);  // "admin_user"
console.log(admin.isAdmin);   // true

// Validation
console.log(User.validateUsername('ab'));      // false (trop court)
console.log(User.validateUsername('john_doe')); // true

// Exemple pratique: Math helpers
class MathHelper {
    // Toutes méthodes statiques (pas besoin d'instance)
    static add(a, b) {
        return a + b;
    }
    
    static multiply(a, b) {
        return a * b;
    }
    
    static average(...numbers) {
        const sum = numbers.reduce((acc, n) => acc + n, 0);
        return sum / numbers.length;
    }
}

// Utiliser sans créer d'instance
console.log(MathHelper.add(5, 3));           // 8
console.log(MathHelper.average(10, 20, 30)); // 20

// Exemple: Comparateur
class Product {
    constructor(name, price) {
        this.name = name;
        this.price = price;
    }
    
    // Méthode d'instance
    getInfo() {
        return `${this.name}: $${this.price}`;
    }
    
    // Méthode statique pour comparer
    static compareByPrice(product1, product2) {
        return product1.price - product2.price;
    }
    
    static compareByName(product1, product2) {
        return product1.name.localeCompare(product2.name);
    }
}

const products = [
    new Product('Laptop', 999),
    new Product('Mouse', 29),
    new Product('Keyboard', 79)
];

// Trier avec méthode statique
products.sort(Product.compareByPrice);
console.log(products.map(p => p.getInfo()));
// ["Mouse: $29", "Keyboard: $79", "Laptop: $999"]

// QUAND UTILISER QUOI:

// MÉTHODE D'INSTANCE:
// - Opère sur les données de l'instance (this)
// - Différent résultat selon l'instance
// - Exemple: user.getInfo(), cart.calculateTotal()

// MÉTHODE STATIQUE:
// - N'utilise pas 'this' de l'instance
// - Utilitaires, validations, factory methods
// - Exemple: User.validate(), Array.isArray(), Math.max()
```

**Points clés:**
- Méthode d'instance: appelée sur `instance.method()`
- Méthode statique: appelée sur `Class.method()`
- Statique = pas d'accès à `this` de l'instance
- Cas d'usage: factory, validation, utilitaires
- Similaire à `Array.isArray()`, `Math.max()`

---

### Q7: Comment fonctionnent les getters et setters ?

**Réponse:**
```javascript
// Getters/Setters = propriétés calculées avec contrôle d'accès

class Temperature {
    constructor(celsius) {
        this._celsius = celsius; // Convention: _ = "privé"
    }
    
    // GETTER - accès en lecture (comme une propriété)
    get celsius() {
        return this._celsius;
    }
    
    // SETTER - modification avec validation
    set celsius(value) {
        if (value < -273.15) {
            throw new Error('Below absolute zero!');
        }
        this._celsius = value;
    }
    
    // Getter calculé - conversion Fahrenheit
    get fahrenheit() {
        return this._celsius * 9/5 + 32;
    }
    
    // Setter pour Fahrenheit
    set fahrenheit(value) {
        this._celsius = (value - 32) * 5/9;
    }
}

const temp = new Temperature(25);

// Utiliser comme propriété (PAS de parenthèses)
console.log(temp.celsius);    // 25 (appelle le getter)
console.log(temp.fahrenheit); // 77 (calcul automatique)

// Modifier comme propriété (appelle le setter)
temp.celsius = 30;
console.log(temp.celsius);    // 30
console.log(temp.fahrenheit); // 86

// Setter avec validation
// temp.celsius = -300; // Error: Below absolute zero!

// Modifier via fahrenheit
temp.fahrenheit = 32;
console.log(temp.celsius); // 0

// Exemple: Validation email
class User {
    constructor(email) {
        this.email = email; // Utilise le setter
    }
    
    get email() {
        return this._email;
    }
    
    set email(value) {
        if (!value.includes('@')) {
            throw new Error('Invalid email format');
        }
        this._email = value.toLowerCase();
    }
    
    // Getter read-only (pas de setter)
    get domain() {
        return this._email.split('@')[1];
    }
}

const user = new User('John@Example.com');
console.log(user.email);  // "john@example.com" (lowercased)
console.log(user.domain); // "example.com" (calculé)

// Setter avec validation
user.email = 'jane@test.com'; // ✅ OK
console.log(user.email); // "jane@test.com"

// user.email = 'invalid'; // ❌ Error: Invalid email format

// Propriété read-only (getter sans setter)
// user.domain = 'other.com'; // Ne fait rien (strict mode: error)

// Exemple: Full name
class Person {
    constructor(firstName, lastName) {
        this.firstName = firstName;
        this.lastName = lastName;
    }
    
    // Getter calculé
    get fullName() {
        return `${this.firstName} ${this.lastName}`;
    }
    
    // Setter pour fullName (parse et sépare)
    set fullName(name) {
        const parts = name.split(' ');
        if (parts.length !== 2) {
            throw new Error('Full name must be "FirstName LastName"');
        }
        this.firstName = parts[0];
        this.lastName = parts[1];
    }
}

const person = new Person('John', 'Doe');
console.log(person.fullName); // "John Doe"

person.fullName = 'Jane Smith';
console.log(person.firstName); // "Jane"
console.log(person.lastName);  // "Smith"

// Exemple: Age calculation
class Employee {
    constructor(name, birthYear) {
        this.name = name;
        this.birthYear = birthYear;
    }
    
    // Getter calculé dynamiquement
    get age() {
        const currentYear = new Date().getFullYear();
        return currentYear - this.birthYear;
    }
    
    // Getter pour retirement age
    get yearsUntilRetirement() {
        return Math.max(0, 65 - this.age);
    }
}

const emp = new Employee('John', 1990);
console.log(emp.age); // Age actuel (ex: 34)
console.log(emp.yearsUntilRetirement); // Ex: 31

// Un an plus tard, age change automatiquement!

// AVANTAGES des getters/setters:
// ✅ Validation automatique
// ✅ Calculs à la volée
// ✅ Syntaxe propre (comme propriété)
// ✅ Encapsulation (cacher implémentation)
// ✅ Rétrocompatibilité (changer implémentation sans changer API)

// ❌ À ÉVITER:
class Bad {
    get value() {
        // Calcul coûteux à chaque accès
        return this.heavyCalculation();
    }
}
// Mieux: calculer une fois et mettre en cache
```

**Points clés:**
- `get` = accès en lecture
- `set` = modification avec validation
- Utilisés comme propriétés (pas de parenthèses)
- Validation automatique dans setter
- Getter pour propriétés calculées
- Read-only = getter sans setter

---

### Q8: Qu'est-ce que le private field `#` et comment l'utiliser ?

**Réponse:**
```javascript
// Private fields (#) = vraiment privés (ES2022)
// Inaccessibles depuis l'extérieur de la classe

class BankAccount {
    // Private fields (déclarés en dehors du constructor)
    #balance = 0;
    #pin;
    
    constructor(owner, initialBalance, pin) {
        this.owner = owner;  // Public
        this.#balance = initialBalance; // Private
        this.#pin = pin; // Private
    }
    
    // Méthode publique qui utilise private field
    getBalance(enteredPin) {
        if (this.#validatePin(enteredPin)) {
            return this.#balance;
        }
        throw new Error('Invalid PIN');
    }
    
    deposit(amount) {
        if (amount > 0) {
            this.#balance += amount;
            return this.#balance;
        }
        return false;
    }
    
    withdraw(amount, enteredPin) {
        if (!this.#validatePin(enteredPin)) {
            throw new Error('Invalid PIN');
        }
        
        if (amount > 0 && amount <= this.#balance) {
            this.#balance -= amount;
            return this.#balance;
        }
        throw new Error('Insufficient funds');
    }
    
    // Private method
    #validatePin(enteredPin) {
        return enteredPin === this.#pin;
    }
}

const account = new BankAccount('John', 1000, '1234');

// ✅ Accès via méthodes publiques
account.deposit(500);
console.log(account.getBalance('1234')); // 1500

account.withdraw(200, '1234');
console.log(account.getBalance('1234')); // 1300

// ❌ Impossible d'accéder aux champs privés
// console.log(account.#balance); // SyntaxError
// console.log(account.#pin);     // SyntaxError
// account.#validatePin('1234');  // SyntaxError

// Les propriétés privées n'apparaissent pas
console.log(Object.keys(account)); // ["owner"] (uniquement public)

// Différence avec convention _ (pseudo-private)
class OldWay {
    constructor(value) {
        this._value = value; // Convention: "ne pas toucher"
    }
    
    getValue() {
        return this._value;
    }
}

const old = new OldWay(42);
console.log(old._value); // 42 ⚠️ Accessible quand même!

class NewWay {
    #value;
    
    constructor(value) {
        this.#value = value; // Vraiment privé
    }
    
    getValue() {
        return this.#value;
    }
}

const newObj = new NewWay(42);
// console.log(newObj.#value); // SyntaxError ✅

// Private static fields
class Counter {
    static #count = 0;
    
    constructor() {
        Counter.#count++;
    }
    
    static getCount() {
        return Counter.#count;
    }
    
    static reset() {
        Counter.#count = 0;
    }
}

new Counter();
new Counter();
console.log(Counter.getCount()); // 2

// console.log(Counter.#count); // SyntaxError

// Exemple: Password hashing
class User {
    #password;
    
    constructor(username, password) {
        this.username = username;
        this.#password = this.#hashPassword(password);
    }
    
    #hashPassword(password) {
        // Simulation simple (production: bcrypt)
        return btoa(password);
    }
    
    verifyPassword(password) {
        return this.#hashPassword(password) === this.#password;
    }
    
    changePassword(oldPassword, newPassword) {
        if (this.verifyPassword(oldPassword)) {
            this.#password = this.#hashPassword(newPassword);
            return true;
        }
        return false;
    }
}

const user = new User('john', 'secret123');

console.log(user.verifyPassword('secret123')); // true
console.log(user.verifyPassword('wrong'));     // false

user.changePassword('secret123', 'newpass456');
console.log(user.verifyPassword('newpass456')); // true

// ❌ Impossible d'accéder au password
// console.log(user.#password); // SyntaxError

// Private fields avec getters/setters
class Product {
    #price;
    
    constructor(name, price) {
        this.name = name;
        this.#price = price;
    }
    
    get price() {
        return this.#price;
    }
    
    set price(value) {
        if (value < 0) {
            throw new Error('Price cannot be negative');
        }
        this.#price = value;
    }
}

const product = new Product('Laptop', 999);
console.log(product.price); // 999 (getter)

product.price = 899; // setter avec validation
console.log(product.price); // 899

// product.price = -100; // Error: Price cannot be negative

// POURQUOI UTILISER PRIVATE FIELDS:
// ✅ Vraie encapsulation (impossible d'accéder de l'extérieur)
// ✅ Évite modifications accidentelles
// ✅ Protège données sensibles
// ✅ API propre et contrôlée
// ✅ Refactoring sûr (changer implémentation interne)
```

**Points clés:**
- `#` = private field (ES2022)
- Vraiment privé (SyntaxError si accès externe)
- Différent de `_` (simple convention)
- Fonctionne pour fields et methods
- Déclaré au début de la classe
- Excellente encapsulation

---

### Q9: Comment utiliser `super` dans une classe qui hérite ?

**Réponse:**
```javascript
// 'super' a 2 usages:
// 1. super() - appeler constructor parent
// 2. super.method() - appeler méthode parent

// USAGE 1: super() dans constructor
class Vehicle {
    constructor(brand, model) {
        this.brand = brand;
        this.model = model;
        console.log('Vehicle constructor called');
    }
    
    getInfo() {
        return `${this.brand} ${this.model}`;
    }
    
    start() {
        console.log('Vehicle starting...');
    }
}

class Car extends Vehicle {
    constructor(brand, model, doors) {
        // super() DOIT être appelé en premier
        super(brand, model); // Appelle Vehicle constructor
        
        // Maintenant on peut utiliser 'this'
        this.doors = doors;
        console.log('Car constructor called');
    }
    
    // USAGE 2: super.method() pour appeler méthode parent
    getInfo() {
        // Appeler méthode parent
        const baseInfo = super.getInfo();
        // Ajouter info supplémentaire
        return `${baseInfo} (${this.doors} doors)`;
    }
    
    start() {
        // Appeler méthode parent d'abord
        super.start();
        // Ajouter comportement spécifique
        console.log('Car engine started');
    }
}

const car = new Car('Toyota', 'Corolla', 4);
// Console:
// "Vehicle constructor called"
// "Car constructor called"

console.log(car.getInfo()); 
// "Toyota Corolla (4 doors)"

car.start();
// "Vehicle starting..."
// "Car engine started"

// ❌ ERREUR: super() doit être avant 'this'
class BadCar extends Vehicle {
    constructor(brand, model, doors) {
        this.doors = doors; // ❌ Error: Must call super() first
        super(brand, model);
    }
}

// ✅ CORRECT: super() en premier
class GoodCar extends Vehicle {
    constructor(brand, model, doors) {
        super(brand, model); // ✅ super() d'abord
        this.doors = doors;  // ✅ puis this
    }
}

// Exemple avec validation dans parent
class Person {
    constructor(name, age) {
        if (age < 0) {
            throw new Error('Age cannot be negative');
        }
        this.name = name;
        this.age = age;
    }
    
    introduce() {
        return `Hi, I'm ${this.name}`;
    }
}

class Employee extends Person {
    constructor(name, age, jobTitle) {
        super(name, age); // Validation héritée
        this.jobTitle = jobTitle;
    }
    
    introduce() {
        const greeting = super.introduce(); // Appeler parent
        return `${greeting} and I work as a ${this.jobTitle}`;
    }
}

const emp = new Employee('John', 30, 'Developer');
console.log(emp.introduce());
// "Hi, I'm John and I work as a Developer"

// Exemple: Chain d'héritage multiple
class Animal {
    constructor(name) {
        this.name = name;
    }
    
    makeSound() {
        console.log(`${this.name} makes a sound`);
    }
}

class Mammal extends Animal {
    constructor(name, furColor) {
        super(name);
        this.furColor = furColor;
    }
    
    makeSound() {
        super.makeSound(); // Appeler Animal.makeSound()
        console.log('(It\'s a mammal)');
    }
}

class Dog extends Mammal {
    constructor(name, furColor, breed) {
        super(name, furColor);
        this.breed = breed;
    }
    
    makeSound() {
        super.makeSound(); // Appeler Mammal.makeSound()
        console.log(`${this.name} barks: Woof!`);
    }
}

const dog = new Dog('Rex', 'brown', 'Labrador');
dog.makeSound();
// "Rex makes a sound"
// "(It's a mammal)"
// "Rex barks: Woof!"

// super dans getters/setters
class Rectangle {
    constructor(width, height) {
        this.width = width;
        this.height = height;
    }
    
    get area() {
        return this.width * this.height;
    }
}

class ColoredRectangle extends Rectangle {
    constructor(width, height, color) {
        super(width, height);
        this.color = color;
    }
    
    get description() {
        const area = super.area; // Utiliser getter parent
        return `A ${this.color} rectangle with area ${area}`;
    }
}

const rect = new ColoredRectangle(10, 5, 'blue');
console.log(rect.description); 
// "A blue rectangle with area 50"

// super avec méthodes statiques
class Base {
    static greet() {
        return 'Hello from Base';
    }
}

class Derived extends Base {
    static greet() {
        const baseGreeting = super.greet(); // Appeler static parent
        return `${baseGreeting}, and hello from Derived`;
    }
}

console.log(Derived.greet());
// "Hello from Base, and hello from Derived"

// Pattern: Template Method
class PaymentProcessor {
    constructor(amount) {
        this.amount = amount;
    }
    
    // Template method (définit le flux)
    processPayment() {
        this.validateAmount();
        this.connectToGateway();
        this.executePayment();
        this.sendConfirmation();
    }
    
    validateAmount() {
        if (this.amount <= 0) {
            throw new Error('Invalid amount');
        }
    }
    
    connectToGateway() {
        console.log('Connecting to payment gateway...');
    }
    
    executePayment() {
        console.log(`Processing payment of $${this.amount}`);
    }
    
    sendConfirmation() {
        console.log('Sending confirmation email');
    }
}

class CreditCardPayment extends PaymentProcessor {
    constructor(amount, cardNumber) {
        super(amount);
        this.cardNumber = cardNumber;
    }
    
    // Override uniquement certaines étapes
    connectToGateway() {
        super.connectToGateway(); // Appeler base
        console.log('Connecting to credit card processor');
    }
    
    executePayment() {
        console.log(`Charging card ending in ${this.cardNumber.slice(-4)}`);
        super.executePayment();
    }
}

const payment = new CreditCardPayment(99.99, '1234567890123456');
payment.processPayment();
// "Connecting to payment gateway..."
// "Connecting to credit card processor"
// "Charging card ending in 3456"
// "Processing payment of $99.99"
// "Sending confirmation email"

// RÈGLES IMPORTANTES:
// 1. super() OBLIGATOIRE dans constructor si classe hérite
// 2. super() DOIT être appelé AVANT d'utiliser 'this'
// 3. super.method() pour appeler méthode parent
// 4. super marche aussi avec static methods
// 5. Ne peut pas skip des niveaux (toujours parent direct)
```

**Points clés:**
- `super()` = appeler constructor parent (obligatoire)
- `super.method()` = appeler méthode parent
- super() doit être appelé avant `this`
- Utile pour étendre comportement parent
- Fonctionne aussi avec static methods et getters

---

### Q10: Comment créer une classe "abstraite" en JavaScript ?

**Réponse:**
```javascript
// JavaScript n'a pas de classes abstraites natives
// Mais on peut les simuler

// APPROCHE 1: Vérifier new.target dans constructor
class Shape {
    constructor(color) {
        // Empêcher instanciation directe de Shape
        if (new.target === Shape) {
            throw new Error('Shape is abstract and cannot be instantiated');
        }
        this.color = color;
    }
    
    // Méthode "abstraite" (doit être overridden)
    getArea() {
        throw new Error('getArea() must be implemented by subclass');
    }
    
    getPerimeter() {
        throw new Error('getPerimeter() must be implemented by subclass');
    }
    
    // Méthode concrète (implémentée)
    describe() {
        return `A ${this.color} shape with area ${this.getArea()}`;
    }
}

// ❌ Impossible d'instancier Shape directement
// const shape = new Shape('red'); 
// Error: Shape is abstract and cannot be instantiated

// ✅ Mais on peut hériter
class Circle extends Shape {
    constructor(color, radius) {
        super(color);
        this.radius = radius;
    }
    
    // Implémenter méthodes abstraites
    getArea() {
        return Math.PI * this.radius ** 2;
    }
    
    getPerimeter() {
        return 2 * Math.PI * this.radius;
    }
}

const circle = new Circle('red', 5);
console.log(circle.describe()); 
// "A red shape with area 78.54..."

// Si on oublie d'implémenter une méthode
class BadCircle extends Shape {
    constructor(color, radius) {
        super(color);
        this.radius = radius;
    }
    
    // On oublie getArea() et getPerimeter()
}

const bad = new BadCircle('blue', 5);
// bad.getArea(); // Error: getArea() must be implemented

// APPROCHE 2: Vérifier présence des méthodes
class Animal {
    constructor(name) {
        if (new.target === Animal) {
            throw new Error('Animal is abstract');
        }
        
        // Vérifier que les méthodes requises existent
        if (this.makeSound === Animal.prototype.makeSound) {
            throw new Error('makeSound() must be implemented');
        }
        
        this.name = name;
    }
    
    makeSound() {
        throw new Error('makeSound() must be implemented');
    }
    
    sleep() {
        console.log(`${this.name} is sleeping`);
    }
}

class Dog extends Animal {
    constructor(name) {
        super(name);
    }
    
    makeSound() {
        console.log(`${this.name} barks: Woof!`);
    }
}

const dog = new Dog('Rex');
dog.makeSound(); // "Rex barks: Woof!"
dog.sleep();     // "Rex is sleeping"

// APPROCHE 3: Interface-like pattern
class Drawable {
    draw() {
        throw new Error('draw() must be implemented');
    }
}

class Movable {
    move(x, y) {
        throw new Error('move() must be implemented');
    }
}

// "Implémenter" plusieurs interfaces
class Sprite {
    constructor() {
        // Vérifier que toutes les méthodes sont implémentées
        const required = [
            { method: 'draw', from: Drawable },
            { method: 'move', from: Movable }
        ];
        
        for (const { method, from } of required) {
            if (this[method] === from.prototype[method]) {
                throw new Error(`${method}() must be implemented`);
            }
        }
    }
}

class Player extends Sprite {
    draw() {
        console.log('Drawing player');
    }
    
    move(x, y) {
        console.log(`Moving to ${x}, ${y}`);
    }
}

const player = new Player();
player.draw();     // "Drawing player"
player.move(10, 5); // "Moving to 10, 5"

// APPROCHE 4: Factory pattern pour imposer l'implémentation
class AbstractFactory {
    static create(type, ...args) {
        throw new Error('create() must be implemented in subclass');
    }
}

class VehicleFactory extends AbstractFactory {
    static create(type, brand) {
        switch(type) {
            case 'car':
                return new Car(brand);
            case 'bike':
                return new Bike(brand);
            default:
                throw new Error('Unknown vehicle type');
        }
    }
}

class Car {
    constructor(brand) {
        this.brand = brand;
    }
}

class Bike {
    constructor(brand) {
        this.brand = brand;
    }
}

const car = VehicleFactory.create('car', 'Toyota');

// Exemple complet: Payment system
class PaymentMethod {
    constructor() {
        if (new.target === PaymentMethod) {
            throw new Error('PaymentMethod is abstract');
        }
        
        // Vérifier méthodes requises
        const required = ['processPayment', 'validateDetails'];
        for (const method of required) {
            if (typeof this[method] !== 'function') {
                throw new Error(`${method}() must be implemented`);
            }
        }
    }
    
    // Méthode abstraite
    processPayment(amount) {
        throw new Error('processPayment() must be implemented');
    }
    
    // Méthode abstraite
    validateDetails() {
        throw new Error('validateDetails() must be implemented');
    }
    
    // Méthode concrète (commune à tous)
    logTransaction(amount) {
        console.log(`Transaction of $${amount} logged`);
    }
}

class CreditCard extends PaymentMethod {
    constructor(cardNumber, cvv) {
        super();
        this.cardNumber = cardNumber;
        this.cvv = cvv;
    }
    
    validateDetails() {
        return this.cardNumber.length === 16 && this.cvv.length === 3;
    }
    
    processPayment(amount) {
        if (!this.validateDetails()) {
            throw new Error('Invalid credit card details');
        }
        console.log(`Charging $${amount} to card ending in ${this.cardNumber.slice(-4)}`);
        this.logTransaction(amount);
    }
}

class PayPal extends PaymentMethod {
    constructor(email) {
        super();
        this.email = email;
    }
    
    validateDetails() {
        return this.email.includes('@');
    }
    
    processPayment(amount) {
        if (!this.validateDetails()) {
            throw new Error('Invalid PayPal email');
        }
        console.log(`Charging $${amount} to PayPal account ${this.email}`);
        this.logTransaction(amount);
    }
}

const cc = new CreditCard('1234567890123456', '123');
cc.processPayment(99.99);
// "Charging $99.99 to card ending in 3456"
// "Transaction of $99.99 logged"

const pp = new PayPal('user@example.com');
pp.processPayment(49.99);
// "Charging $49.99 to PayPal account user@example.com"
// "Transaction of $49.99 logged"

// TYPESCRIPT (pour référence)
/*
// En TypeScript, on a de vraies classes abstraites:

abstract class Shape {
    abstract getArea(): number;
    abstract getPerimeter(): number;
    
    describe(): string {
        return `Area: ${this.getArea()}`;
    }
}

class Circle extends Shape {
    constructor(private radius: number) {
        super();
    }
    
    getArea(): number {
        return Math.PI * this.radius ** 2;
    }
    
    getPerimeter(): number {
        return 2 * Math.PI * this.radius;
    }
}
*/

// BONNES PRATIQUES:
// 1. Documenter clairement les méthodes abstraites
// 2. Lancer des erreurs descriptives
// 3. Vérifier l'implémentation dans le constructor
// 4. Considérer TypeScript pour vrai support abstraction
```

**Points clés:**
- JS n'a pas de classes abstraites natives
- Simuler avec `new.target` check
- Lancer erreurs si méthodes pas implémentées
- Empêcher instanciation directe
- Utile pour définir contrats/interfaces
- TypeScript offre vrai support abstraction

---

## Questions Niveau Avancé

### Q11: Comment implémenter le pattern Singleton avec une classe ?

**Réponse:**
```javascript
// Singleton = une seule instance de la classe existe

// APPROCHE 1: Variable statique privée
class Singleton {
    static #instance = null;
    
    constructor() {
        // Si instance existe déjà, la retourner
        if (Singleton.#instance) {
            return Singleton.#instance;
        }
        
        // Sinon, créer et stocker l'instance
        Singleton.#instance = this;
        
        // Initialiser propriétés
        this.data = [];
    }
    
    static getInstance() {
        if (!Singleton.#instance) {
            Singleton.#instance = new Singleton();
        }
        return Singleton.#instance;
    }
    
    addData(item) {
        this.data.push(item);
    }
    
    getData() {
        return this.data;
    }
}

// Toutes les créations retournent la même instance
const instance1 = new Singleton();
const instance2 = new Singleton();
const instance3 = Singleton.getInstance();

console.log(instance1 === instance2); // true
console.log(instance1 === instance3); // true

instance1.addData('item1');
instance2.addData('item2');

console.log(instance3.getData()); // ['item1', 'item2']

// APPROCHE 2: Empêcher new (forcer getInstance)
class StrictSingleton {
    static #instance = null;
    static #isInternalConstructing = false;
    
    constructor() {
        if (!StrictSingleton.#isInternalConstructing) {
            throw new Error('Use StrictSingleton.getInstance() instead');
        }
        
        this.data = {};
    }
    
    static getInstance() {
        if (!StrictSingleton.#instance) {
            StrictSingleton.#isInternalConstructing = true;
            StrictSingleton.#instance = new StrictSingleton();
            StrictSingleton.#isInternalConstructing = false;
        }
        return StrictSingleton.#instance;
    }
}

// const bad = new StrictSingleton(); // Error!
const good = StrictSingleton.getInstance(); // ✅

// EXEMPLE PRATIQUE: Database connection
class Database {
    static #instance = null;
    #isConnected = false;
    
    constructor() {
        if (Database.#instance) {
            return Database.#instance;
        }
        Database.#instance = this;
    }
    
    static getInstance() {
        if (!Database.#instance) {
            Database.#instance = new Database();
        }
        return Database.#instance;
    }
    
    connect() {
        if (!this.#isConnected) {
            console.log('Connecting to database...');
            this.#isConnected = true;
        }
    }
    
    query(sql) {
        if (!this.#isConnected) {
            throw new Error('Not connected to database');
        }
        console.log(`Executing: ${sql}`);
        return { results: [] };
    }
    
    disconnect() {
        if (this.#isConnected) {
            console.log('Disconnecting from database...');
            this.#isConnected = false;
        }
    }
}

// Utilisation dans différents modules
function moduleA() {
    const db = Database.getInstance();
    db.connect();
    db.query('SELECT * FROM users');
}

function moduleB() {
    const db = Database.getInstance(); // Même instance!
    db.query('SELECT * FROM products');
}

moduleA();
moduleB();
// Une seule connexion partagée

// EXEMPLE: Configuration globale
class Config {
    static #instance = null;
    #settings = {};
    
    constructor() {
        if (Config.#instance) {
            return Config.#instance;
        }
        Config.#instance = this;
        
        // Charger config par défaut
        this.#settings = {
            apiUrl: 'https://api.example.com',
            timeout: 5000,
            retries: 3
        };
    }
    
    static getInstance() {
        if (!Config.#instance) {
            Config.#instance = new Config();
        }
        return Config.#instance;
    }
    
    get(key) {
        return this.#settings[key];
    }
    
    set(key, value) {
        this.#settings[key] = value;
    }
    
    getAll() {
        return { ...this.#settings };
    }
}

// Accéder depuis n'importe où
const config1 = Config.getInstance();
config1.set('apiUrl', 'https://new-api.example.com');

const config2 = Config.getInstance();
console.log(config2.get('apiUrl')); 
// "https://new-api.example.com" (même instance)

// EXEMPLE: Logger
class Logger {
    static #instance = null;
    #logs = [];
    
    constructor() {
        if (Logger.#instance) {
            return Logger.#instance;
        }
        Logger.#instance = this;
    }
    
    static getInstance() {
        if (!Logger.#instance) {
            Logger.#instance = new Logger();
        }
        return Logger.#instance;
    }
    
    log(message, level = 'INFO') {
        const entry = {
            timestamp: new Date(),
            level,
            message
        };
        this.#logs.push(entry);
        console.log(`[${level}] ${message}`);
    }
    
    error(message) {
        this.log(message, 'ERROR');
    }
    
    warn(message) {
        this.log(message, 'WARN');
    }
    
    getLogs() {
        return [...this.#logs];
    }
    
    clear() {
        this.#logs = [];
    }
}

// Utiliser partout dans l'app
const logger = Logger.getInstance();
logger.log('Application started');
logger.error('Something went wrong');
logger.warn('Deprecated feature used');

console.log(logger.getLogs().length); // 3

// ATTENTION: Singleton peut compliquer les tests
// Alternative: Dependency Injection
```

**Points clés:**
- Singleton = une seule instance globale
- Utiliser static private field pour stocker
- Constructor retourne instance existante
- `getInstance()` pour accès
- Cas d'usage: DB, Config, Logger
- ⚠️ Peut compliquer tests (état global)

---

### Q12: Comment implémenter le pattern Observer avec des classes ?

**Réponse:**
```javascript
// Observer Pattern = Système de notifications (Pub/Sub)

// Subject (Observable) - celui qui émet des événements
class Subject {
    #observers = [];
    
    // S'abonner
    subscribe(observer) {
        if (!this.#observers.includes(observer)) {
            this.#observers.push(observer);
        }
    }
    
    // Se désabonner
    unsubscribe(observer) {
        const index = this.#observers.indexOf(observer);
        if (index > -1) {
            this.#observers.splice(index, 1);
        }
    }
    
    // Notifier tous les observers
    notify(data) {
        this.#observers.forEach(observer => {
            observer.update(data);
        });
    }
}

// Observer - celui qui écoute
class Observer {
    constructor(name) {
        this.name = name;
    }
    
    update(data) {
        console.log(`${this.name} received: ${JSON.stringify(data)}`);
    }
}

// Utilisation
const subject = new Subject();

const observer1 = new Observer('Observer 1');
const observer2 = new Observer('Observer 2');

subject.subscribe(observer1);
subject.subscribe(observer2);

subject.notify({ event: 'test', value: 123 });
// "Observer 1 received: {"event":"test","value":123}"
// "Observer 2 received: {"event":"test","value":123}"

subject.unsubscribe(observer1);
subject.notify({ event: 'another', value: 456 });
// "Observer 2 received: {"event":"another","value":456}"

// EXEMPLE PRATIQUE: Newsletter
class Newsletter extends Subject {
    #articles = [];
    
    publishArticle(article) {
        this.#articles.push(article);
        this.notify({ type: 'new-article', article });
    }
    
    getArticles() {
        return [...this.#articles];
    }
}

class Subscriber extends Observer {
    constructor(email) {
        super(email);
        this.email = email;
    }
    
    update(data) {
        if (data.type === 'new-article') {
            console.log(`📧 Email sent to ${this.email}: New article "${data.article.title}"`);
        }
    }
}

const newsletter = new Newsletter();

const sub1 = new Subscriber('john@example.com');
const sub2 = new Subscriber('jane@example.com');

newsletter.subscribe(sub1);
newsletter.subscribe(sub2);

newsletter.publishArticle({ title: 'JavaScript Tips', content: '...' });
// "📧 Email sent to john@example.com: New article "JavaScript Tips""
// "📧 Email sent to jane@example.com: New article "JavaScript Tips""

// EXEMPLE: Event Emitter (comme Node.js EventEmitter)
class EventEmitter {
    #events = {};
    
    on(event, listener) {
        if (!this.#events[event]) {
            this.#events[event] = [];
        }
        this.#events[event].push(listener);
        return this;
    }
    
    off(event, listener) {
        if (!this.#events[event]) return;
        
        const index = this.#events[event].indexOf(listener);
        if (index > -1) {
            this.#events[event].splice(index, 1);
        }
    }
    
    emit(event, ...args) {
        if (!this.#events[event]) return;
        
        this.#events[event].forEach(listener => {
            listener(...args);
        });
    }
    
    once(event, listener) {
        const wrapper = (...args) => {
            listener(...args);
            this.off(event, wrapper);
        };
        this.on(event, wrapper);
    }
}

// Utilisation
const emitter = new EventEmitter();

function onUserLogin(user) {
    console.log(`User ${user.name} logged in`);
}

function onUserLogout(user) {
    console.log(`User ${user.name} logged out`);
}

emitter.on('login', onUserLogin);
emitter.on('logout', onUserLogout);

emitter.emit('login', { name: 'John' });  
// "User John logged in"

emitter.emit('logout', { name: 'John' }); 
// "User John logged out"

// once() - écouter une seule fois
emitter.once('error', (error) => {
    console.log('Error occurred:', error);
});

emitter.emit('error', 'Something went wrong'); 
// "Error occurred: Something went wrong"

emitter.emit('error', 'Another error'); 
// (rien - listener supprimé après premier appel)

// EXEMPLE COMPLET: Stock Market
class Stock extends EventEmitter {
    #price;
    #symbol;
    
    constructor(symbol, initialPrice) {
        super();
        this.#symbol = symbol;
        this.#price = initialPrice;
    }
    
    get price() {
        return this.#price;
    }
    
    get symbol() {
        return this.#symbol;
    }
    
    setPrice(newPrice) {
        const oldPrice = this.#price;
        this.#price = newPrice;
        
        // Émettre événement
        this.emit('price-change', {
            symbol: this.#symbol,
            oldPrice,
            newPrice,
            change: newPrice - oldPrice,
            changePercent: ((newPrice - oldPrice) / oldPrice * 100).toFixed(2)
        });
    }
}

class Trader {
    constructor(name) {
        this.name = name;
        this.portfolio = {};
    }
    
    watchStock(stock) {
        stock.on('price-change', (data) => {
            console.log(`📊 ${this.name} notified: ${data.symbol} changed from $${data.oldPrice} to $${data.newPrice} (${data.changePercent}%)`);
            
            // Logique de trading automatique
            if (data.changePercent > 5) {
                console.log(`  🔔 ${this.name}: Time to sell!`);
            } else if (data.changePercent < -5) {
                console.log(`  🔔 ${this.name}: Time to buy!`);
            }
        });
    }
}

const apple = new Stock('AAPL', 150);
const trader1 = new Trader('Alice');
const trader2 = new Trader('Bob');

trader1.watchStock(apple);
trader2.watchStock(apple);

apple.setPrice(160); // +6.67%
// "📊 Alice notified: AAPL changed from $150 to $160 (6.67%)"
// "  🔔 Alice: Time to sell!"
// "📊 Bob notified: AAPL changed from $150 to $160 (6.67%)"
// "  🔔 Bob: Time to sell!"

apple.setPrice(140); // -12.5%
// "  🔔 Alice: Time to buy!"
// "  🔔 Bob: Time to buy!"
```

**Points clés:**
- Observer = pattern de notification/subscription
- Subject maintient liste d'observers
- `subscribe()`/`unsubscribe()` pour gérer
- `notify()` pour émettre événements
- Similaire à EventEmitter de Node.js
- Utile pour: événements, notifications, state changes

---