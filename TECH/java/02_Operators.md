## [**..**](./00_index.md)

# Chapter 2: Operators

## Understanding Java Operators

Les **operators** sont des symboles qui effectuent des opérations sur des **operands**.

**Ordre de priorité** (du plus élevé au plus bas):
1. Post-unary: `expr++`, `expr--`
2. Pre-unary: `++expr`, `--expr`, `+`, `-`, `!`, `~`, cast
3. Multiplicative: `*`, `/`, `%`
4. Additive: `+`, `-`
5. Relational: `<`, `>`, `<=`, `>=`, `instanceof`
6. Equality: `==`, `!=`
7. Logical AND: `&&`
8. Logical OR: `||`
9. Ternary: `? :`
10. Assignment: `=`, `+=`, `-=`, etc.

```java
int result = 2 + 3 * 4;      // result = 14 (pas 20)
int result2 = (2 + 3) * 4;   // result2 = 20 (parenthèses prioritaires)
```

## Applying Unary Operators

Les **unary operators** opèrent sur un seul operand.

### Complement et Negation Operators

```java
// Logical complement (!)
boolean actif = true;
System.out.println(!actif);     // false

// Negation (-)
int nombre = 10;
System.out.println(-nombre);    // -10
int negatif = -nombre;          // negatif = -10

// Bitwise complement (~)
int bits = 10;                  // 00001010 en binaire
System.out.println(~bits);      // -11 (inverse tous les bits)
```

### Increment et Decrement Operators

**Pre-increment/decrement**: modifie puis retourne la valeur
**Post-increment/decrement**: retourne puis modifie la valeur

```java
// Pre-increment (++variable)
int a = 5;
int b = ++a;    // a = 6, b = 6
System.out.println("a=" + a + ", b=" + b);  // a=6, b=6

// Post-increment (variable++)
int x = 5;
int y = x++;    // x = 6, y = 5
System.out.println("x=" + x + ", y=" + y);  // x=6, y=5

// Pre-decrement (--variable)
int c = 5;
int d = --c;    // c = 4, d = 4

// Post-decrement (variable--)
int m = 5;
int n = m--;    // m = 4, n = 5
```

**Exemple complexe**:
```java
int i = 0;
int result = i++ + ++i;
// Étape 1: i++ retourne 0, puis i devient 1
// Étape 2: ++i incrémente i à 2, puis retourne 2
// result = 0 + 2 = 2
// i = 2
```

## Working with Binary Arithmetic Operators

Les **binary operators** opèrent sur deux operands.

### Arithmetic Operators

```java
int a = 10, b = 3;

// Addition
int somme = a + b;           // 13

// Subtraction
int difference = a - b;      // 7

// Multiplication
int produit = a * b;         // 30

// Division
int quotient = a / b;        // 3 (division entière)
double quotient2 = 10.0 / 3; // 3.3333...

// Modulus (reste)
int reste = a % b;           // 1
int reste2 = 10 % 4;         // 2
int reste3 = -10 % 3;        // -1
```

**Attention à la division par zéro**:
```java
int x = 10 / 0;              // ArithmeticException (runtime)
double y = 10.0 / 0.0;       // Infinity (pas d'exception)
double z = 0.0 / 0.0;        // NaN (Not a Number)
```

### Numeric Promotion

Java applique des règles de promotion automatique:

**Règles**:
1. Si deux operands sont de types différents, le plus petit est promu
2. `byte`, `short`, `char` sont promus en `int` pour les opérations
3. Résultat est du type le plus large

```java
// Promotion à int
byte b1 = 10;
byte b2 = 20;
// byte b3 = b1 + b2;        // ERREUR: int ne peut pas être assigné à byte
int b3 = b1 + b2;            // OK

// Promotion à long
int i = 10;
long l = 20L;
long result = i + l;         // i promu en long

// Promotion à double
float f = 1.0f;
double d = 2.0;
double result2 = f + d;      // f promu en double
```

**Exemple piège**:
```java
short s1 = 10;
short s2 = 20;
short s3 = s1 + s2;          // ERREUR de compilation
// s1 + s2 produit un int, pas un short

// Solutions:
short s3 = (short)(s1 + s2); // Cast explicite
int s3 = s1 + s2;            // Déclarer en int
```

### Concatenation avec String

Le `+` operator peut concaténer des strings:

```java
String texte = "Hello" + " " + "World";  // "Hello World"

// Avec des nombres
System.out.println(1 + 2);               // 3 (addition)
System.out.println("1" + 2);             // "12" (concatenation)
System.out.println(1 + 2 + "3");         // "33" (évalue de gauche à droite)
System.out.println("1" + 2 + 3);         // "123" (tout en string)
System.out.println("1" + (2 + 3));       // "15" (parenthèses forcent l'addition)
```

**Règle**: Si un operand est un String, l'opération devient une concatenation.

## Assigning Values

### Simple Assignment

```java
int nombre = 42;
String texte = "Hello";
boolean flag = true;
```

### Compound Assignment Operators

**Raccourcis** pour les opérations courantes:

```java
int x = 10;

x += 5;    // x = x + 5    → x = 15
x -= 3;    // x = x - 3    → x = 12
x *= 2;    // x = x * 2    → x = 24
x /= 4;    // x = x / 4    → x = 6
x %= 4;    // x = x % 4    → x = 2
```

**Avantage caché**: casting automatique:

```java
long l = 10;
int i = 5;

// i = i + l;           // ERREUR: incompatible types
i += l;                 // OK! Cast automatique: i = (int)(i + l)

// Équivalent à:
i = (int)(i + l);
```

**Autres compound operators**:
```java
int x = 10;
x &= 5;    // Bitwise AND
x |= 3;    // Bitwise OR
x ^= 2;    // Bitwise XOR
x <<= 1;   // Left shift
x >>= 1;   // Right shift
```

### Casting

**Implicit casting** (automatic widening):
```java
int i = 10;
long l = i;        // OK: int → long
double d = l;      // OK: long → double
```

**Explicit casting** (narrowing):
```java
double d = 10.5;
int i = (int)d;         // i = 10 (perte de la partie décimale)

long l = 100;
int i2 = (int)l;        // OK

// Attention à la perte de données
int large = 1000000;
byte petit = (byte)large;  // Overflow, résultat imprévisible
```

**Casting avec overflow**:
```java
int i = 130;
byte b = (byte)i;       // -126 (overflow)
// 130 > 127 (max byte), donc wraparound
```

## Comparing Values

### Equality Operators (`==` et `!=`)

**Pour les primitives**: compare les valeurs
```java
int a = 5;
int b = 5;
System.out.println(a == b);     // true

double x = 5.0;
int y = 5;
System.out.println(x == y);     // true (promotion automatique)
```

**Pour les references**: compare les adresses mémoire
```java
String s1 = new String("Hello");
String s2 = new String("Hello");
System.out.println(s1 == s2);   // false (objets différents)
System.out.println(s1.equals(s2)); // true (même contenu)

// String pool
String s3 = "Hello";
String s4 = "Hello";
System.out.println(s3 == s4);   // true (même référence dans le pool)
```

**Important**: utiliser `equals()` pour comparer le contenu des objets, `==` pour les références.

### Relational Operators

```java
int a = 10, b = 20;

System.out.println(a < b);      // true
System.out.println(a > b);      // false
System.out.println(a <= 10);    // true
System.out.println(b >= 20);    // true

// Ne fonctionne qu'avec des types numériques
// boolean x = true < false;    // ERREUR de compilation
```

### instanceof Operator

Vérifie si un objet est une instance d'un type:

```java
String texte = "Hello";
System.out.println(texte instanceof String);      // true
System.out.println(texte instanceof Object);      // true (String extends Object)
System.out.println(texte instanceof Integer);     // ERREUR de compilation

Object obj = "Test";
System.out.println(obj instanceof String);        // true

// Avec null
String s = null;
System.out.println(s instanceof String);          // false (null n'est instance de rien)
```

**Pattern matching avec instanceof** (Java 16+):
```java
Object obj = "Hello";

// Ancien style
if (obj instanceof String) {
    String s = (String)obj;
    System.out.println(s.toUpperCase());
}

// Nouveau style (pattern matching)
if (obj instanceof String s) {
    System.out.println(s.toUpperCase());  // s déjà casté
}
```

### Logical Operators

**Short-circuit operators** (`&&` et `||`):
```java
boolean a = true;
boolean b = false;

// AND logique
System.out.println(a && b);     // false
System.out.println(true && true); // true

// OR logique
System.out.println(a || b);     // true
System.out.println(false || false); // false

// Short-circuit: n'évalue pas le second operand si inutile
int x = 0;
if (x != 0 && 10 / x > 1) {     // 10/x n'est jamais évalué
    System.out.println("OK");
}

// Sans short-circuit, causerait ArithmeticException
```

**Non short-circuit operators** (`&` et `|`):
```java
boolean resultat = true & false;   // false (évalue les deux côtés)
boolean resultat2 = true | false;  // true (évalue les deux côtés)

// Différence importante:
int x = 0;
// if (x != 0 & 10 / x > 1) {      // ArithmeticException!
//     System.out.println("OK");
// }
```

**XOR operator** (`^`):
```java
System.out.println(true ^ false);   // true (différents)
System.out.println(true ^ true);    // false (identiques)
System.out.println(false ^ false);  // false (identiques)
```

**NOT operator** (`!`):
```java
boolean flag = true;
System.out.println(!flag);          // false
System.out.println(!!flag);         // true (double négation)
```

## Making Decisions with the Ternary Operator

Le **ternary operator** (`? :`) est le seul operator à trois operands:

**Syntaxe**: `condition ? valueIfTrue : valueIfFalse`

```java
int age = 18;
String statut = age >= 18 ? "Adulte" : "Mineur";
System.out.println(statut);  // "Adulte"

// Équivalent à:
String statut;
if (age >= 18) {
    statut = "Adulte";
} else {
    statut = "Mineur";
}
```

**Exemples avancés**:
```java
// Ternary imbriqués (déconseillé, peu lisible)
int score = 85;
String grade = score >= 90 ? "A" : 
               score >= 80 ? "B" : 
               score >= 70 ? "C" : "F";
System.out.println(grade);  // "B"

// Avec expressions
int a = 10, b = 20;
int max = a > b ? a : b;
System.out.println(max);    // 20

// Les deux valeurs doivent être compatibles
int x = 5;
// String result = x > 0 ? 123 : "négatif";  // ERREUR de compilation
Object result = x > 0 ? 123 : "négatif";     // OK (type commun: Object)
```

**Type du résultat**:
```java
int i = 1;
long l = 2L;

// Le résultat est du type le plus large
var result = i > 0 ? i : l;  // result est long
```

**Piège avec null**:
```java
int x = 1;
Integer y = null;

// Integer result = x > 0 ? y : 0;  // NullPointerException!
// Pourquoi? 0 (int) est unboxed, donc y est aussi unboxed → NPE

Integer result = x > 0 ? y : Integer.valueOf(0);  // OK
```

---

## Exemples Pratiques Complets

### Exemple 1: Calculatrice simple
```java
public class Calculator {
    public static void main(String[] args) {
        int a = 15, b = 4;
        
        System.out.println("Addition: " + (a + b));           // 19
        System.out.println("Soustraction: " + (a - b));       // 11
        System.out.println("Multiplication: " + (a * b));     // 60
        System.out.println("Division: " + (a / b));           // 3
        System.out.println("Modulo: " + (a % b));             // 3
        
        // Division avec précision
        double precision = (double)a / b;
        System.out.println("Division précise: " + precision); // 3.75
    }
}
```

### Exemple 2: Validation d'âge
```java
public class AgeValidator {
    public static String getStatut(int age) {
        if (age < 0) {
            return "Âge invalide";
        }
        
        // Ternary pour classification
        return age < 13 ? "Enfant" :
               age < 18 ? "Adolescent" :
               age < 65 ? "Adulte" : "Senior";
    }
    
    public static void main(String[] args) {
        System.out.println(getStatut(10));   // "Enfant"
        System.out.println(getStatut(16));   // "Adolescent"
        System.out.println(getStatut(30));   // "Adulte"
        System.out.println(getStatut(70));   // "Senior"
    }
}
```

### Exemple 3: Opérations complexes
```java
public class ComplexOperations {
    public static void main(String[] args) {
        int x = 5;
        
        // Post vs Pre increment
        System.out.println("x++ = " + (x++));  // 5
        System.out.println("x = " + x);        // 6
        System.out.println("++x = " + (++x));  // 7
        
        // Short-circuit
        int y = 0;
        boolean result = y != 0 && 10 / y > 1;  // false, pas d'exception
        System.out.println(result);
        
        // Compound assignment avec casting
        long l = 1000L;
        int i = 10;
        i += l;  // Cast automatique
        System.out.println("i = " + i);  // 1010
    }
}
```

---

## Points Clés du Chapter 2

✅ **Ordre de priorité**: parenthèses > unary > multiplicative > additive > relational > equality > logical > ternary > assignment

✅ **Pre-increment** (`++x`): incrémente puis retourne / **Post-increment** (`x++`): retourne puis incrémente

✅ **Numeric promotion**: `byte`, `short`, `char` → `int` automatiquement dans les opérations

✅ **String concatenation**: si un operand est String, `+` devient concatenation

✅ **Compound assignment** (`+=`, `-=`, etc.): inclut un cast automatique

✅ **`==` vs `equals()`**: `==` compare références, `equals()` compare contenu

✅ **Short-circuit** (`&&`, `||`): évalue le minimum nécessaire / **Non short-circuit** (`&`, `|`): évalue toujours tout

✅ **Ternary operator**: `condition ? valeurSiVrai : valeurSiFaux`

✅ **instanceof**: vérifie le type, retourne `false` pour `null`

---
# Questions/Réponses - Chapter 2: Operators

## Questions sur l'Ordre de Priorité

**Q1: Quelle est la valeur de result?**
```java
int result = 2 + 3 * 4;
```

**R:** `result = 14`. La multiplication a une priorité plus élevée que l'addition, donc l'expression est évaluée comme `2 + (3 * 4) = 2 + 12 = 14`, pas `(2 + 3) * 4 = 20`.

---

**Q2: Quel est le résultat de cette expression?**
```java
int x = 5;
int y = 10;
int result = x + y * 2 / 5 - 1;
```

**R:** `result = 8`.
Ordre d'évaluation:
1. `y * 2 = 20` (multiplication)
2. `20 / 5 = 4` (division, même priorité que multiplication, de gauche à droite)
3. `x + 4 = 9` (addition)
4. `9 - 1 = 8` (soustraction)

Expression: `5 + ((10 * 2) / 5) - 1 = 5 + 4 - 1 = 8`

---

## Questions sur les Unary Operators

**Q3: Quelle est la différence entre ces deux expressions?**
```java
int a = 5;
int b = ++a;  // ?
int c = a++;  // ?
```

**R:**
```java
int a = 5;
int b = ++a;  // Pre-increment: a = 6, b = 6
int c = a++;  // Post-increment: c = 6, a = 7
```

**Pre-increment** (`++a`): incrémente d'abord `a`, puis retourne la nouvelle valeur.
**Post-increment** (`a++`): retourne la valeur actuelle de `a`, puis incrémente.

---

**Q4: Que s'affiche-t-il?**
```java
int x = 10;
int y = x++ + ++x;
System.out.println("x=" + x + ", y=" + y);
```

**R:** `x=12, y=22`.
Étapes:
1. `x++` retourne 10, puis `x` devient 11
2. `++x` incrémente `x` à 12, puis retourne 12
3. `y = 10 + 12 = 22`
4. `x = 12`

---

**Q5: Quelle est la valeur après ces opérations?**
```java
int i = 5;
i = i++;
System.out.println(i);
```

**R:** `i = 5` (piège classique!).
Étapes:
1. `i++` retourne 5 (valeur actuelle)
2. `i` est incrémenté à 6
3. La valeur 5 est assignée à `i`
4. Résultat final: `i = 5`

C'est pourquoi on ne doit jamais écrire `i = i++`.

---

**Q6: Que fait l'operator `!`?**
```java
boolean actif = true;
System.out.println(!actif);
System.out.println(!!actif);
```

**R:** L'operator `!` est le **logical complement** (négation logique).
```java
!actif   // false
!!actif  // true (double négation)
```

---

## Questions sur les Binary Arithmetic Operators

**Q7: Pourquoi ce code ne compile-t-il pas?**
```java
byte b1 = 10;
byte b2 = 20;
byte b3 = b1 + b2;
```

**R:** Erreur de compilation: "incompatible types: possible lossy conversion from int to byte".

Java applique la **numeric promotion**: `byte`, `short`, et `char` sont automatiquement promus en `int` pour les opérations arithmétiques. Donc `b1 + b2` produit un `int`, pas un `byte`.

**Solutions**:
```java
byte b3 = (byte)(b1 + b2);  // Cast explicite
int b3 = b1 + b2;           // Déclarer en int
```

---

**Q8: Que s'affiche-t-il?**
```java
System.out.println(10 / 3);
System.out.println(10.0 / 3);
System.out.println(10 / 3.0);
System.out.println((double)10 / 3);
```

**R:**
```java
10 / 3           // 3 (division entière)
10.0 / 3         // 3.3333... (division décimale)
10 / 3.0         // 3.3333... (division décimale)
(double)10 / 3   // 3.3333... (cast, puis division décimale)
```

Si au moins un operand est `double` ou `float`, le résultat est décimal.

---

**Q9: Que se passe-t-il lors de ces divisions?**
```java
int a = 10 / 0;
double b = 10.0 / 0.0;
double c = 0.0 / 0.0;
```

**R:**
```java
int a = 10 / 0;        // ArithmeticException: / by zero (runtime)
double b = 10.0 / 0.0; // Infinity (pas d'exception)
double c = 0.0 / 0.0;  // NaN (Not a Number)
```

Division par zéro avec des **entiers** → exception.
Division par zéro avec des **décimaux** → `Infinity` ou `NaN`.

---

**Q10: Quelle est la valeur de reste?**
```java
int reste1 = 10 % 3;
int reste2 = -10 % 3;
int reste3 = 10 % -3;
```

**R:**
```java
reste1 = 1   // 10 = 3*3 + 1
reste2 = -1  // -10 = 3*(-3) + (-1)
reste3 = 1   // 10 = (-3)*(-3) + 1
```

Le signe du résultat suit le signe du **dividende** (premier operand).

---

## Questions sur la Concatenation

**Q11: Que s'affiche-t-il?**
```java
System.out.println(1 + 2);
System.out.println("1" + 2);
System.out.println(1 + 2 + "3");
System.out.println("1" + 2 + 3);
System.out.println("1" + (2 + 3));
```

**R:**
```java
1 + 2              // 3 (addition)
"1" + 2            // "12" (concatenation)
1 + 2 + "3"        // "33" (addition puis concatenation: (1+2)+"3")
"1" + 2 + 3        // "123" (concatenation: "1"+"2"+"3")
"1" + (2 + 3)      // "15" (parenthèses forcent l'addition: "1"+"5")
```

**Règle**: Évaluation de **gauche à droite**. Dès qu'un String apparaît, tout devient concatenation.

---

## Questions sur les Assignments

**Q12: Quelle est la différence entre ces deux lignes?**
```java
int i = 5;
long l = 10L;

i = i + l;    // ?
i += l;       // ?
```

**R:**
```java
i = i + l;    // ERREUR: incompatible types: possible lossy conversion from long to int
i += l;       // OK: cast automatique, équivalent à i = (int)(i + l)
```

Les **compound assignment operators** (`+=`, `-=`, etc.) incluent un **cast implicite**.

---

**Q13: Que vaut x après ces opérations?**
```java
int x = 10;
x += 5;   // ?
x *= 2;   // ?
x -= 3;   // ?
x /= 4;   // ?
```

**R:**
```java
x += 5;   // x = 10 + 5 = 15
x *= 2;   // x = 15 * 2 = 30
x -= 3;   // x = 30 - 3 = 27
x /= 4;   // x = 27 / 4 = 6 (division entière)
```

Résultat final: `x = 6`

---

## Questions sur le Casting

**Q14: Que se passe-t-il lors de ce cast?**
```java
double d = 10.9;
int i = (int)d;
System.out.println(i);
```

**R:** `i = 10`. Le cast de `double` vers `int` **tronque** la partie décimale (ne fait pas d'arrondi). On perd `.9`.

---

**Q15: Pourquoi ce code compile-t-il sans cast?**
```java
int i = 100;
long l = i;
double d = l;
```

**R:** C'est du **widening** (élargissement): conversion d'un type plus petit vers un type plus grand. Java le fait automatiquement car il n'y a pas de perte de données possible.

Hiérarchie: `byte` → `short` → `int` → `long` → `float` → `double`

Le casting explicite n'est nécessaire que pour le **narrowing** (rétrécissement).

---

## Questions sur la Comparaison

**Q16: Quelle est la différence entre == et equals()?**
```java
String s1 = new String("Hello");
String s2 = new String("Hello");
System.out.println(s1 == s2);
System.out.println(s1.equals(s2));
```

**R:**
```java
s1 == s2         // false (compare les références mémoire)
s1.equals(s2)    // true (compare le contenu)
```

**`==`**: compare les **adresses mémoire** (pour les objects) ou les **valeurs** (pour les primitives).
**`equals()`**: compare le **contenu logique** des objets.

---

**Q17: Pourquoi ce résultat est-il différent?**
```java
String s1 = "Hello";
String s2 = "Hello";
System.out.println(s1 == s2);  // true

String s3 = new String("Hello");
String s4 = new String("Hello");
System.out.println(s3 == s4);  // false
```

**R:** Les String literals (`"Hello"`) sont stockés dans le **String pool**. `s1` et `s2` pointent vers la **même référence** dans le pool.

Avec `new String()`, on force la création d'un **nouvel objet** dans le heap. `s3` et `s4` sont donc des objets différents avec des références différentes.

---

**Q18: Que retourne instanceof avec null?**
```java
String s = null;
System.out.println(s instanceof String);
System.out.println(s instanceof Object);
```

**R:** Les deux retournent `false`. **`null` n'est instance d'aucun type**.

```java
null instanceof AnyType  // Toujours false
```

---

**Q19: Ce code compile-t-il?**
```java
String s = "Hello";
System.out.println(s instanceof Integer);
```

**R:** Non, **erreur de compilation**: "incompatible types: String cannot be converted to Integer".

Le compiler sait que `String` et `Integer` n'ont aucune relation d'héritage, donc le test est impossible.

---

## Questions sur les Logical Operators

**Q20: Quelle est la différence entre && et &?**
```java
int x = 0;
if (x != 0 && 10 / x > 1) {
    System.out.println("A");
}

if (x != 0 & 10 / x > 1) {
    System.out.println("B");
}
```

**R:**
- `&&` (short-circuit AND): Si le premier operand est `false`, le second n'est **pas évalué**. Pas d'exception.
- `&` (non short-circuit AND): Les **deux operands** sont toujours évalués. `10 / x` cause une **ArithmeticException**.

```java
if (x != 0 && 10 / x > 1)  // OK, 10/x n'est jamais exécuté
if (x != 0 & 10 / x > 1)   // ArithmeticException!
```

---

**Q21: Que s'affiche-t-il?**
```java
System.out.println(true && true);
System.out.println(true && false);
System.out.println(true || false);
System.out.println(false || false);
System.out.println(true ^ false);
System.out.println(true ^ true);
```

**R:**
```java
true && true    // true
true && false   // false
true || false   // true
false || false  // false
true ^ false    // true (XOR: différents)
true ^ true     // false (XOR: identiques)
```

**XOR** (`^`): retourne `true` si les operands sont **différents**.

---

**Q22: Pourquoi utiliser short-circuit operators?**

**R:** Pour **optimiser les performances** et **éviter des erreurs**:

```java
// Éviter NullPointerException
String s = null;
if (s != null && s.length() > 0) {  // OK avec &&
    System.out.println(s);
}

// Éviter divisions par zéro
int x = 0;
if (x != 0 && 100 / x > 10) {       // OK avec &&
    System.out.println("Grand");
}

// Optimisation
if (conditionRapide() || conditionLente()) {
    // Si conditionRapide() est true, conditionLente() n'est jamais appelée
}
```

---

## Questions sur le Ternary Operator

**Q23: Que vaut result?**
```java
int age = 17;
String result = age >= 18 ? "Adulte" : "Mineur";
```

**R:** `result = "Mineur"`. La condition `age >= 18` est `false`, donc la valeur après `:` est retournée.

---

**Q24: Ce code compile-t-il?**
```java
int x = 5;
String result = x > 0 ? 123 : "négatif";
```

**R:** Non, **erreur de compilation**: "incompatible types: bad type in conditional expression".

Les deux valeurs (après `?` et après `:`) doivent avoir un **type compatible**. Ici, `int` et `String` ne sont pas compatibles.

**Solution**:
```java
Object result = x > 0 ? 123 : "négatif";  // OK (type commun: Object)
String result = x > 0 ? "123" : "négatif"; // OK (deux Strings)
```

---

**Q25: Que s'affiche-t-il?**
```java
int score = 75;
String grade = score >= 90 ? "A" :
               score >= 80 ? "B" :
               score >= 70 ? "C" : "F";
System.out.println(grade);
```

**R:** `grade = "C"`.

Évaluation:
1. `score >= 90` → false
2. `score >= 80` → false
3. `score >= 70` → true → retourne "C"

C'est équivalent à des `if-else if` imbriqués, mais moins lisible.

---

## Questions Pièges

**Q26: Que s'affiche-t-il?**
```java
int x = 5;
System.out.println(x > 3 ? x++ : ++x);
System.out.println(x);
```

**R:**
```java
x > 3 ? x++ : ++x   // 5 (x > 3 est true, donc x++ retourne 5)
x                    // 6 (x a été incrémenté par x++)
```

Seul le branch pris est exécuté. `++x` n'est jamais évalué.

---

**Q27: Pourquoi cette ligne cause-t-elle une NullPointerException?**
```java
Integer x = null;
int y = x > 0 ? x : 0;
```

**R:** **NullPointerException** lors de l'évaluation de `x > 0`.

Java essaie d'**unbox** `x` (convertir `Integer` en `int`) pour comparer, mais `x` est `null` → exception.

**Solution**:
```java
int y = x != null && x > 0 ? x : 0;  // Vérifier null d'abord
```

---

**Q28: Quelle est la valeur finale?**
```java
int a = 2;
int b = 3;
int result = a++ + ++b * a--;
System.out.println(result);
```

**R:** `result = 14`.

Étapes:
1. `a++` retourne 2, puis `a` devient 3
2. `++b` incrémente `b` à 4, puis retourne 4
3. `a--` retourne 3, puis `a` devient 2
4. `result = 2 + (4 * 3) = 2 + 12 = 14`

Valeur finale de `a` = 2, `b` = 4.

---

**Q29: Ce code compile-t-il?**
```java
int x = 5;
long y = 10L;
x = x + y;
```

**R:** Non, **erreur de compilation**: "incompatible types: possible lossy conversion from long to int".

`x + y` produit un `long` (promotion). On ne peut pas assigner un `long` à un `int` sans cast.

**Solutions**:
```java
x = (int)(x + y);  // Cast explicite
x += y;            // Compound assignment (cast automatique)
long x = x + y;    // Changer le type de x
```

---

**Q30: Pourquoi affiche-t-il cela?**
```java
System.out.println(0.1 + 0.2);
System.out.println(0.1 + 0.2 == 0.3);
```

**R:**
```java
0.1 + 0.2          // 0.30000000000000004 (pas exactement 0.3!)
0.1 + 0.2 == 0.3   // false
```

Les nombres décimaux (`float`, `double`) ont des **problèmes de précision** en raison de leur représentation binaire. Ne jamais comparer des `double` avec `==`.

**Solution**:
```java
double epsilon = 0.0001;
boolean egal = Math.abs((0.1 + 0.2) - 0.3) < epsilon;  // true
```

Ou utiliser `BigDecimal` pour des calculs précis.

---