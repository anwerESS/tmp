## [**..**](./00_index.md)

# Chapitre 2 : Programmation Orientée Objet (POO)

## 🎯 Objectifs du chapitre
- Comprendre les concepts fondamentaux de la POO
- Créer et utiliser des classes et objets
- Maîtriser l'héritage et le polymorphisme
- Utiliser les méthodes spéciales (dunder methods)
- Appliquer l'encapsulation

---

## 2.1 Classes et objets

### Définition de base d'une classe

```python
# Classe simple
class Dog:
    pass  # Classe vide pour l'instant

# Créer une instance (objet)
my_dog = Dog()
print(type(my_dog))  # <class '__main__.Dog'>
```

### Constructeur __init__

```python
class Dog:
    # Constructeur - appelé lors de la création d'une instance
    def __init__(self, name, age):
        self.name = name  # Attribut d'instance
        self.age = age
    
    # Méthode d'instance
    def bark(self):
        return f"{self.name} dit: Wouaf!"
    
    def get_info(self):
        return f"{self.name} a {self.age} ans"

# Création d'instances
dog1 = Dog("Rex", 3)
dog2 = Dog("Bella", 5)

print(dog1.bark())      # Rex dit: Wouaf!
print(dog2.get_info())  # Bella a 5 ans

# Accès aux attributs
print(dog1.name)  # Rex
dog1.age = 4      # Modification d'attribut
print(dog1.age)   # 4
```

### Attributs d'instance vs attributs de classe

```python
class Dog:
    # Attribut de classe (partagé par toutes les instances)
    species = "Canis familiaris"
    count = 0  # Compteur d'instances
    
    def __init__(self, name, age):
        # Attributs d'instance (propres à chaque objet)
        self.name = name
        self.age = age
        Dog.count += 1  # Incrémente le compteur de classe
    
    @classmethod
    def get_count(cls):
        return cls.count

# Utilisation
dog1 = Dog("Rex", 3)
dog2 = Dog("Bella", 5)

# Attribut de classe - accessible via la classe ou l'instance
print(Dog.species)    # Canis familiaris
print(dog1.species)   # Canis familiaris
print(dog2.species)   # Canis familiaris

# Compteur
print(Dog.get_count())  # 2

# Modification d'attribut de classe
Dog.species = "Canis lupus"
print(dog1.species)  # Canis lupus (affecte toutes les instances)

# Attention : créer un attribut d'instance masque l'attribut de classe
dog1.species = "Special Dog"
print(dog1.species)  # Special Dog (attribut d'instance)
print(dog2.species)  # Canis lupus (attribut de classe)
print(Dog.species)   # Canis lupus
```

### Méthodes : instance, classe, et statique

```python
class MathOperations:
    pi = 3.14159
    
    def __init__(self, value):
        self.value = value
    
    # Méthode d'instance (nécessite une instance, reçoit self)
    def square(self):
        return self.value ** 2
    
    # Méthode de classe (nécessite la classe, reçoit cls)
    @classmethod
    def from_string(cls, string_value):
        """Factory method - crée une instance à partir d'une string"""
        return cls(float(string_value))
    
    # Méthode statique (ne reçoit ni self ni cls)
    @staticmethod
    def add(a, b):
        """Fonction utilitaire liée à la classe"""
        return a + b

# Méthode d'instance
obj = MathOperations(5)
print(obj.square())  # 25

# Méthode de classe (factory method)
obj2 = MathOperations.from_string("10")
print(obj2.value)    # 10.0
print(obj2.square()) # 100.0

# Méthode statique
result = MathOperations.add(3, 7)  # 10
# Peut aussi être appelée via une instance
result = obj.add(3, 7)  # 10 (mais moins idiomatique)
```

### Exemple complet : classe BankAccount

```python
class BankAccount:
    # Attribut de classe
    bank_name = "MyBank"
    interest_rate = 0.02
    
    def __init__(self, account_number, owner, balance=0):
        self.account_number = account_number
        self.owner = owner
        self.balance = balance
        self.transactions = []
    
    def deposit(self, amount):
        """Déposer de l'argent"""
        if amount > 0:
            self.balance += amount
            self.transactions.append(f"Dépôt: +{amount}")
            return True
        return False
    
    def withdraw(self, amount):
        """Retirer de l'argent"""
        if 0 < amount <= self.balance:
            self.balance -= amount
            self.transactions.append(f"Retrait: -{amount}")
            return True
        return False
    
    def get_balance(self):
        """Obtenir le solde"""
        return self.balance
    
    def apply_interest(self):
        """Appliquer les intérêts"""
        interest = self.balance * self.interest_rate
        self.balance += interest
        self.transactions.append(f"Intérêts: +{interest:.2f}")
    
    def get_statement(self):
        """Obtenir le relevé de compte"""
        statement = f"Compte {self.account_number} - {self.owner}\n"
        statement += f"Solde actuel: {self.balance:.2f}€\n"
        statement += "Transactions:\n"
        for transaction in self.transactions:
            statement += f"  - {transaction}\n"
        return statement
    
    @classmethod
    def set_interest_rate(cls, new_rate):
        """Modifier le taux d'intérêt pour tous les comptes"""
        cls.interest_rate = new_rate
    
    @staticmethod
    def validate_account_number(account_number):
        """Valider un numéro de compte"""
        return len(str(account_number)) >= 8

# Utilisation
account = BankAccount("12345678", "Alice", 1000)
account.deposit(500)
account.withdraw(200)
account.apply_interest()
print(account.get_statement())

# Validation statique
print(BankAccount.validate_account_number("12345678"))  # True
```

---

## 2.2 Méthodes spéciales (dunder methods)

Les méthodes spéciales (double underscore = "dunder") permettent de personnaliser le comportement de vos objets.

### __str__ et __repr__

```python
class Book:
    def __init__(self, title, author, year):
        self.title = title
        self.author = author
        self.year = year
    
    def __str__(self):
        """Représentation lisible pour l'utilisateur"""
        return f"'{self.title}' par {self.author}"
    
    def __repr__(self):
        """Représentation technique pour les développeurs"""
        return f"Book('{self.title}', '{self.author}', {self.year})"

book = Book("1984", "George Orwell", 1949)

print(str(book))   # '1984' par George Orwell
print(repr(book))  # Book('1984', 'George Orwell', 1949)
print(book)        # Utilise __str__ par défaut

# Dans une liste, __repr__ est utilisé
books = [book]
print(books)       # [Book('1984', 'George Orwell', 1949)]
```

### __len__ et __getitem__

```python
class Playlist:
    def __init__(self, name):
        self.name = name
        self.songs = []
    
    def add_song(self, song):
        self.songs.append(song)
    
    def __len__(self):
        """Permet d'utiliser len() sur l'objet"""
        return len(self.songs)
    
    def __getitem__(self, index):
        """Permet l'indexation playlist[0] et l'itération"""
        return self.songs[index]
    
    def __setitem__(self, index, value):
        """Permet playlist[0] = 'nouvelle chanson'"""
        self.songs[index] = value
    
    def __delitem__(self, index):
        """Permet del playlist[0]"""
        del self.songs[index]
    
    def __contains__(self, song):
        """Permet 'song' in playlist"""
        return song in self.songs

# Utilisation
playlist = Playlist("Ma playlist")
playlist.add_song("Bohemian Rhapsody")
playlist.add_song("Stairway to Heaven")
playlist.add_song("Hotel California")

print(len(playlist))        # 3
print(playlist[0])          # Bohemian Rhapsody
playlist[1] = "Imagine"     # Modifie la chanson
print("Imagine" in playlist)  # True

# Itération (grâce à __getitem__)
for song in playlist:
    print(f"- {song}")

# Slicing
print(playlist[0:2])  # Fonctionne aussi !
```

### Opérateurs arithmétiques

```python
class Vector:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    
    def __repr__(self):
        return f"Vector({self.x}, {self.y})"
    
    def __add__(self, other):
        """Addition : v1 + v2"""
        return Vector(self.x + other.x, self.y + other.y)
    
    def __sub__(self, other):
        """Soustraction : v1 - v2"""
        return Vector(self.x - other.x, self.y - other.y)
    
    def __mul__(self, scalar):
        """Multiplication par un scalaire : v * 3"""
        return Vector(self.x * scalar, self.y * scalar)
    
    def __rmul__(self, scalar):
        """Multiplication inversée : 3 * v"""
        return self.__mul__(scalar)
    
    def __eq__(self, other):
        """Égalité : v1 == v2"""
        return self.x == other.x and self.y == other.y
    
    def __abs__(self):
        """Valeur absolue : abs(v)"""
        return (self.x ** 2 + self.y ** 2) ** 0.5

# Utilisation
v1 = Vector(2, 3)
v2 = Vector(4, 5)

print(v1 + v2)      # Vector(6, 8)
print(v2 - v1)      # Vector(2, 2)
print(v1 * 3)       # Vector(6, 9)
print(3 * v1)       # Vector(6, 9)
print(v1 == v2)     # False
print(abs(v1))      # 3.605551275463989
```

### Opérateurs de comparaison

```python
class Student:
    def __init__(self, name, grade):
        self.name = name
        self.grade = grade
    
    def __repr__(self):
        return f"Student('{self.name}', {self.grade})"
    
    def __lt__(self, other):
        """Less than : <"""
        return self.grade < other.grade
    
    def __le__(self, other):
        """Less than or equal : <="""
        return self.grade <= other.grade
    
    def __gt__(self, other):
        """Greater than : >"""
        return self.grade > other.grade
    
    def __ge__(self, other):
        """Greater than or equal : >="""
        return self.grade >= other.grade
    
    def __eq__(self, other):
        """Equal : =="""
        return self.grade == other.grade
    
    def __ne__(self, other):
        """Not equal : !="""
        return self.grade != other.grade

# Utilisation
alice = Student("Alice", 85)
bob = Student("Bob", 92)
charlie = Student("Charlie", 85)

print(alice < bob)       # True
print(alice > bob)       # False
print(alice == charlie)  # True (même note)

# Tri
students = [bob, charlie, alice]
students.sort()  # Utilise __lt__
print(students)  # [Student('Alice', 85), Student('Charlie', 85), Student('Bob', 92)]
```

### __call__ - Rendre un objet appelable

```python
class Multiplier:
    def __init__(self, factor):
        self.factor = factor
    
    def __call__(self, x):
        """Permet d'appeler l'objet comme une fonction"""
        return x * self.factor

# Utilisation
double = Multiplier(2)
triple = Multiplier(3)

print(double(5))   # 10 (appelle __call__)
print(triple(5))   # 15

# Exemple pratique : fonction avec état
class Counter:
    def __init__(self):
        self.count = 0
    
    def __call__(self):
        self.count += 1
        return self.count

counter = Counter()
print(counter())  # 1
print(counter())  # 2
print(counter())  # 3
```

### Context managers : __enter__ et __exit__

```python
class FileManager:
    def __init__(self, filename, mode):
        self.filename = filename
        self.mode = mode
        self.file = None
    
    def __enter__(self):
        """Appelé au début du bloc 'with'"""
        print(f"Ouverture de {self.filename}")
        self.file = open(self.filename, self.mode)
        return self.file
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Appelé à la fin du bloc 'with', même en cas d'erreur"""
        print(f"Fermeture de {self.filename}")
        if self.file:
            self.file.close()
        # Retourner False pour propager l'exception, True pour la supprimer
        return False

# Utilisation
with FileManager('test.txt', 'w') as f:
    f.write('Hello, World!')
# Le fichier est automatiquement fermé

# Exemple avec gestion d'erreur
class DatabaseConnection:
    def __init__(self, db_name):
        self.db_name = db_name
        self.connection = None
    
    def __enter__(self):
        print(f"Connexion à {self.db_name}")
        # Simuler une connexion
        self.connection = {"connected": True}
        return self.connection
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        print(f"Déconnexion de {self.db_name}")
        if exc_type:
            print(f"Erreur détectée: {exc_val}")
        self.connection = None
        return False  # Propager l'exception

with DatabaseConnection("mydb") as conn:
    print("Travail avec la base de données...")
    # La déconnexion se fait automatiquement
```

---

## 2.3 Héritage et polymorphisme

### Héritage simple

```python
# Classe parent (ou superclasse)
class Animal:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def make_sound(self):
        return "Some sound"
    
    def get_info(self):
        return f"{self.name}, {self.age} ans"

# Classe enfant (ou sous-classe)
class Dog(Animal):
    def __init__(self, name, age, breed):
        # Appeler le constructeur parent
        super().__init__(name, age)
        self.breed = breed
    
    # Override (surcharge) de méthode
    def make_sound(self):
        return "Wouaf!"
    
    # Nouvelle méthode propre à Dog
    def fetch(self):
        return f"{self.name} rapporte la balle!"

class Cat(Animal):
    def make_sound(self):
        return "Miaou!"
    
    def scratch(self):
        return f"{self.name} griffe le canapé!"

# Utilisation
dog = Dog("Rex", 3, "Labrador")
cat = Cat("Whiskers", 2)

print(dog.get_info())      # Rex, 3 ans (hérité)
print(dog.make_sound())    # Wouaf! (surchargé)
print(dog.fetch())         # Rex rapporte la balle!

print(cat.get_info())      # Whiskers, 2 ans (hérité)
print(cat.make_sound())    # Miaou! (surchargé)
print(cat.scratch())       # Whiskers griffe le canapé!

# Vérification de type
print(isinstance(dog, Dog))     # True
print(isinstance(dog, Animal))  # True
print(isinstance(dog, Cat))     # False
print(issubclass(Dog, Animal))  # True
```

### super() - Appeler les méthodes parentes

```python
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def introduce(self):
        return f"Je m'appelle {self.name} et j'ai {self.age} ans"

class Student(Person):
    def __init__(self, name, age, student_id, major):
        # Appeler le constructeur parent
        super().__init__(name, age)
        self.student_id = student_id
        self.major = major
    
    def introduce(self):
        # Appeler la méthode parente et l'étendre
        base_intro = super().introduce()
        return f"{base_intro}. Je suis étudiant en {self.major}"

class GraduateStudent(Student):
    def __init__(self, name, age, student_id, major, thesis_topic):
        super().__init__(name, age, student_id, major)
        self.thesis_topic = thesis_topic
    
    def introduce(self):
        base_intro = super().introduce()
        return f"{base_intro}. Ma thèse porte sur {self.thesis_topic}"

# Utilisation
person = Person("Alice", 25)
print(person.introduce())

student = Student("Bob", 20, "S12345", "Informatique")
print(student.introduce())

grad = GraduateStudent("Charlie", 24, "G67890", "IA", "Deep Learning")
print(grad.introduce())
```

### Héritage multiple

```python
class Flyer:
    def fly(self):
        return "Je vole dans les airs!"

class Swimmer:
    def swim(self):
        return "Je nage dans l'eau!"

class Duck(Flyer, Swimmer):
    def __init__(self, name):
        self.name = name
    
    def quack(self):
        return "Coin coin!"

# Utilisation
duck = Duck("Donald")
print(duck.fly())      # Je vole dans les airs!
print(duck.swim())     # Je nage dans l'eau!
print(duck.quack())    # Coin coin!

# MRO (Method Resolution Order)
print(Duck.__mro__)
# (<class '__main__.Duck'>, <class '__main__.Flyer'>, 
#  <class '__main__.Swimmer'>, <class 'object'>)
```

### Diamond Problem et MRO

```python
class A:
    def method(self):
        return "Méthode de A"

class B(A):
    def method(self):
        return "Méthode de B"

class C(A):
    def method(self):
        return "Méthode de C"

class D(B, C):
    pass

# Quel method() sera appelé ?
obj = D()
print(obj.method())  # "Méthode de B"

# MRO utilise l'algorithme C3 linearization
print(D.__mro__)
# (<class '__main__.D'>, <class '__main__.B'>, 
#  <class '__main__.C'>, <class '__main__.A'>, <class 'object'>)

# Avec super() dans les classes intermédiaires
class A:
    def method(self):
        print("A")

class B(A):
    def method(self):
        print("B")
        super().method()

class C(A):
    def method(self):
        print("C")
        super().method()

class D(B, C):
    def method(self):
        print("D")
        super().method()

obj = D()
obj.method()
# Affiche:
# D
# B
# C
# A
```

### Polymorphisme

```python
# Le polymorphisme permet d'utiliser des objets de types différents
# de manière uniforme

class Shape:
    def area(self):
        raise NotImplementedError("Subclass must implement area()")
    
    def perimeter(self):
        raise NotImplementedError("Subclass must implement perimeter()")

class Rectangle(Shape):
    def __init__(self, width, height):
        self.width = width
        self.height = height
    
    def area(self):
        return self.width * self.height
    
    def perimeter(self):
        return 2 * (self.width + self.height)

class Circle(Shape):
    def __init__(self, radius):
        self.radius = radius
    
    def area(self):
        import math
        return math.pi * self.radius ** 2
    
    def perimeter(self):
        import math
        return 2 * math.pi * self.radius

# Polymorphisme en action
def print_shape_info(shape):
    """Fonctionne avec n'importe quelle Shape"""
    print(f"Aire: {shape.area():.2f}")
    print(f"Périmètre: {shape.perimeter():.2f}")

rect = Rectangle(5, 3)
circle = Circle(4)

print("Rectangle:")
print_shape_info(rect)

print("\nCercle:")
print_shape_info(circle)

# Liste de shapes
shapes = [Rectangle(4, 5), Circle(3), Rectangle(2, 8)]
total_area = sum(shape.area() for shape in shapes)
print(f"\nAire totale: {total_area:.2f}")
```

### Classes abstraites (ABC - Abstract Base Class)

```python
from abc import ABC, abstractmethod

class Vehicle(ABC):
    def __init__(self, brand, model):
        self.brand = brand
        self.model = model
    
    @abstractmethod
    def start(self):
        """Méthode abstraite - doit être implémentée par les sous-classes"""
        pass
    
    @abstractmethod
    def stop(self):
        pass
    
    def get_info(self):
        """Méthode concrète - peut être utilisée telle quelle"""
        return f"{self.brand} {self.model}"

class Car(Vehicle):
    def start(self):
        return "Tourne la clé, le moteur démarre"
    
    def stop(self):
        return "Freine et éteins le moteur"

class ElectricCar(Vehicle):
    def start(self):
        return "Appuie sur le bouton, la voiture est prête"
    
    def stop(self):
        return "Arrête et éteins l'électronique"

# Utilisation
car = Car("Toyota", "Corolla")
print(car.start())
print(car.get_info())

# Impossible de créer une instance de Vehicle directement
# vehicle = Vehicle("Generic", "Model")  # TypeError!
```

---

## 2.4 Encapsulation

### Attributs publics, protégés, et privés

```python
class BankAccount:
    def __init__(self, account_number, balance):
        self.account_number = account_number  # Public
        self._balance = balance               # Protégé (convention)
        self.__pin = "1234"                   # Privé (name mangling)
    
    # Getter pour _balance
    def get_balance(self):
        return self._balance
    
    # Setter pour _balance
    def set_balance(self, amount):
        if amount >= 0:
            self._balance = amount
        else:
            raise ValueError("Le solde ne peut pas être négatif")
    
    def __verify_pin(self, pin):
        """Méthode privée"""
        return pin == self.__pin
    
    def withdraw(self, amount, pin):
        if self.__verify_pin(pin):
            if amount <= self._balance:
                self._balance -= amount
                return True
        return False

# Utilisation
account = BankAccount("12345", 1000)

# Public - accessible directement
print(account.account_number)  # 12345

# Protégé - accessible mais déconseillé (convention)
print(account._balance)  # 1000 (fonctionne mais pas recommandé)

# Privé - name mangling
# print(account.__pin)  # AttributeError
print(account._BankAccount__pin)  # 1234 (fonctionne mais vraiment pas recommandé!)

# Méthodes publiques pour accéder aux attributs protégés
print(account.get_balance())  # 1000
account.set_balance(1500)
print(account.get_balance())  # 1500
```

### Properties - getters et setters pythoniques

```python
class Temperature:
    def __init__(self, celsius):
        self._celsius = celsius
    
    # Getter
    @property
    def celsius(self):
        """Obtenir la température en Celsius"""
        return self._celsius
    
    # Setter
    @celsius.setter
    def celsius(self, value):
        """Définir la température en Celsius"""
        if value < -273.15:
            raise ValueError("Température en dessous du zéro absolu!")
        self._celsius = value
    
    # Deleter
    @celsius.deleter
    def celsius(self):
        """Supprimer la température"""
        print("Suppression de la température")
        del self._celsius
    
    # Property calculée (read-only)
    @property
    def fahrenheit(self):
        """Obtenir la température en Fahrenheit"""
        return self._celsius * 9/5 + 32
    
    @fahrenheit.setter
    def fahrenheit(self, value):
        """Définir la température via Fahrenheit"""
        self._celsius = (value - 32) * 5/9

# Utilisation
temp = Temperature(25)

# Accès comme un attribut (mais utilise le getter)
print(temp.celsius)      # 25

# Modification (utilise le setter)
temp.celsius = 30
print(temp.celsius)      # 30

# Property calculée
print(temp.fahrenheit)   # 86.0

# Définir via fahrenheit
temp.fahrenheit = 32
print(temp.celsius)      # 0.0

# Validation
# temp.celsius = -300  # ValueError!

# Suppression
del temp.celsius
```

### Exemple complet : classe avec encapsulation

```python
class Person:
    def __init__(self, name, age):
        self._name = name
        self._age = age
        self._email = None
    
    @property
    def name(self):
        return self._name
    
    @name.setter
    def name(self, value):
        if not value or not value.strip():
            raise ValueError("Le nom ne peut pas être vide")
        self._name = value.strip().title()
    
    @property
    def age(self):
        return self._age
    
    @age.setter
    def age(self, value):
        if not isinstance(value, int):
            raise TypeError("L'âge doit être un entier")
        if value < 0 or value > 150:
            raise ValueError("L'âge doit être entre 0 et 150")
        self._age = value
    
    @property
    def email(self):
        return self._email
    
    @email.setter
    def email(self, value):
        if value and '@' not in value:
            raise ValueError("Email invalide")
        self._email = value
    
    @property
    def is_adult(self):
        """Property read-only calculée"""
        return self._age >= 18
    
    def __repr__(self):
        return f"Person(name='{self.name}', age={self.age})"

# Utilisation
person = Person("alice", 25)
print(person.name)      # Alice (capitalisé automatiquement)
print(person.age)       # 25
print(person.is_adult)  # True

# Modification avec validation
person.name = "  bob smith  "
print(person.name)      # Bob Smith

person.age = 30
print(person.age)       # 30

# Erreurs de validation
try:
    person.age = -5
except ValueError as e:
    print(e)  # L'âge doit être entre 0 et 150

try:
    person.email = "invalid-email"
except ValueError as e:
    print(e)  # Email invalide

# is_adult est read-only
# person.is_adult = False  # AttributeError!
```

### Propriétés calculées et caching

```python
class Circle:
    def __init__(self, radius):
        self._radius = radius
        self._area_cache = None
        self._perimeter_cache = None
    
    @property
    def radius(self):
        return self._radius
    
    @radius.setter
    def radius(self, value):
        if value <= 0:
            raise ValueError("Le rayon doit être positif")
        self._radius = value
        # Invalider les caches
        self._area_cache = None
        self._perimeter_cache = None
    
    @property
    def area(self):
        """Aire avec cache pour éviter les calculs répétés"""
        if self._area_cache is None:
            import math
            self._area_cache = math.pi * self._radius ** 2
            print("Calcul de l'aire...")
        return self._area_cache
    
    @property
    def perimeter(self):
        """Périmètre avec cache"""
        if self._perimeter_cache is None:
            import math
            self._perimeter_cache = 2 * math.pi * self._radius
            print("Calcul du périmètre...")
        return self._perimeter_cache

# Utilisation
circle = Circle(5)
print(circle.area)       # Calcul de l'aire... 78.53...
print(circle.area)       # 78.53... (depuis le cache)
print(circle.perimeter)  # Calcul du périmètre... 31.41...
print(circle.perimeter)  # 31.41... (depuis le cache)

# Changement du rayon invalide les caches
circle.radius = 10
print(circle.area)       # Calcul de l'aire... 314.15...
```

---

## 🎯 Points clés à retenir

### Classes et objets
- **`__init__`** : constructeur pour initialiser les objets
- **`self`** : référence à l'instance courante
- **Attributs de classe** : partagés par toutes les instances
- **Attributs d'instance** : propres à chaque objet

### Méthodes spéciales
- **`__str__`** : représentation lisible (utilisateur)
- **`__repr__`** : représentation technique (développeur)
- **`__len__`, `__getitem__`** : comportement comme les collections
- **Opérateurs surchargés** : `__add__`, `__eq__`, `__lt__`, etc.

### Héritage
- **Simple** : `class Child(Parent)`
- **Multiple** : `class Child(Parent1, Parent2)`
- **`super()`** : appeler les méthodes parentes
- **MRO** : Method Resolution Order (algorithme C3)
- **ABC** : classes abstraites avec `@abstractmethod`

### Encapsulation
- **Public** : `self.attribute`
- **Protégé** : `self._attribute` (convention)
- **Privé** : `self.__attribute` (name mangling)
- **`@property`** : getters/setters pythoniques

---

## 💡 Questions d'entretien courantes (Chapitre 2)

1. **Quelle est la différence entre une méthode de classe et une méthode statique ?**
   - Méthode de classe : reçoit `cls`, peut accéder aux attributs de classe
   - Méthode statique : ne reçoit ni `self` ni `cls`, fonction utilitaire

2. **Qu'est-ce que le MRO (Method Resolution Order) ?**
   - Ordre dans lequel Python cherche les méthodes dans l'héritage multiple
   - Utilise l'algorithme C3 linearization
   - Consultable via `ClassName.__mro__`

3. **Différence entre `__str__` et `__repr__` ?**
   - `__str__` : représentation lisible pour l'utilisateur
   - `__repr__` : représentation technique pour le debugging
   - Si `__str__` n'existe pas, Python utilise `__repr__`

4. **Comment créer un attribut en lecture seule ?**
   ```python
   @property
   def attribute(self):
       return self._attribute
   # Pas de setter = read-only
   ```

5. **Qu'est-ce qu'une classe abstraite ?**
   - Classe qui ne peut pas être instanciée directement
   - Définit une interface que les sous-classes doivent implémenter
   - Utilise le module `abc` et `@abstractmethod`

6. **Que fait `super()` ?**
   - Appelle la méthode de la classe parente
   - Suit le MRO pour l'héritage multiple
   - Permet d'étendre (pas seulement remplacer) les méthodes parentes

---

## 🔧 Exercices pratiques

1. **Créez une hiérarchie de classes pour un système de véhicules**
   - Classe de base `Vehicle`
   - Sous-classes `Car`, `Motorcycle`, `Truck`
   - Utilisez l'héritage et le polymorphisme

2. **Implémentez une classe `Stack` (pile)**
   - Avec `__len__`, `__str__`
   - Méthodes `push()`, `pop()`, `peek()`
   - `is_empty()` comme property

3. **Créez une classe `ComplexNumber`**
   - Surchargez les opérateurs `+`, `-`, `*`, `/`
   - Implémentez `__str__` et `__repr__`
   - Ajoutez une property pour le module

4. **Implémentez un context manager pour un timer**
   - Utilise `__enter__` et `__exit__`
   - Mesure et affiche le temps d'exécution

5. **Créez une classe `Product` avec validation**
   - Properties pour prix (positif), nom (non vide)
   - Méthode pour appliquer une réduction
   - Property calculée pour le prix TTC

---

**Prochaine étape :** Chapitre 3 - Manipulation de Fichiers et I/O
