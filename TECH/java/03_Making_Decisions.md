## [**..**](./00_index.md)

# Chapter 3: Making Decisions

## Creating Decision-Making Statements

Les **decision-making statements** permettent de contrôler le flux d'exécution du programme.

### The if Statement

**Syntaxe basique**:
```java
if (condition) {
    // Code exécuté si condition est true
}
```

**Exemple**:
```java
int age = 18;
if (age >= 18) {
    System.out.println("Vous êtes majeur");
}
```

**Important**: La condition doit être un `boolean`. Pas de `int` comme en C:
```java
int x = 1;
// if (x) { }  // ERREUR de compilation
if (x == 1) { }  // OK
```

### The if-else Statement

```java
if (condition) {
    // Code si true
} else {
    // Code si false
}
```

**Exemple**:
```java
int score = 75;
if (score >= 60) {
    System.out.println("Réussi");
} else {
    System.out.println("Échoué");
}
```

### The if-else if-else Statement

Pour tester plusieurs conditions:

```java
int score = 85;

if (score >= 90) {
    System.out.println("A");
} else if (score >= 80) {
    System.out.println("B");
} else if (score >= 70) {
    System.out.println("C");
} else if (score >= 60) {
    System.out.println("D");
} else {
    System.out.println("F");
}
// Output: "B"
```

**Ordre important**: Les conditions sont évaluées dans l'ordre. Dès qu'une est `true`, les autres ne sont pas testées.

### Braces et Single Statement

Les accolades sont **optionnelles** pour une seule instruction, mais **fortement recommandées**:

```java
// Sans accolades (déconseillé)
if (x > 0)
    System.out.println("Positif");

// Avec accolades (recommandé)
if (x > 0) {
    System.out.println("Positif");
}
```

**Piège classique**:
```java
if (x > 0)
    System.out.println("Positif");
    System.out.println("Cette ligne s'exécute toujours!");  // Pas dans le if!
```

**Correct**:
```java
if (x > 0) {
    System.out.println("Positif");
    System.out.println("Cette ligne ne s'exécute que si x > 0");
}
```

### Nested if Statements

```java
int age = 25;
boolean permis = true;

if (age >= 18) {
    if (permis) {
        System.out.println("Peut conduire");
    } else {
        System.out.println("Majeur mais sans permis");
    }
} else {
    System.out.println("Trop jeune");
}
```

## Applying switch Statements

Le **switch statement** permet de tester une variable contre plusieurs valeurs.

### Traditional switch Statement

**Syntaxe**:
```java
switch (variable) {
    case valeur1:
        // Code
        break;
    case valeur2:
        // Code
        break;
    default:
        // Code par défaut
}
```

**Exemple basique**:
```java
int jour = 3;

switch (jour) {
    case 1:
        System.out.println("Lundi");
        break;
    case 2:
        System.out.println("Mardi");
        break;
    case 3:
        System.out.println("Mercredi");
        break;
    case 4:
        System.out.println("Jeudi");
        break;
    case 5:
        System.out.println("Vendredi");
        break;
    case 6:
    case 7:
        System.out.println("Week-end");
        break;
    default:
        System.out.println("Jour invalide");
}
// Output: "Mercredi"
```

### Types Supportés

Le switch accepte:
- `byte`, `short`, `char`, `int` (et leurs wrappers)
- `String` (depuis Java 7)
- `enum`
- **NE supporte PAS**: `long`, `float`, `double`, `boolean`

```java
// String switch
String fruit = "pomme";
switch (fruit) {
    case "pomme":
        System.out.println("C'est une pomme");
        break;
    case "banane":
        System.out.println("C'est une banane");
        break;
    default:
        System.out.println("Fruit inconnu");
}
```

### Fall-through Behavior

**Sans `break`**, l'exécution continue dans les cas suivants:

```java
int x = 2;
switch (x) {
    case 1:
        System.out.println("Un");
    case 2:
        System.out.println("Deux");  // Exécuté
    case 3:
        System.out.println("Trois"); // Exécuté aussi (fall-through)!
        break;
    case 4:
        System.out.println("Quatre");
}
// Output: "Deux"
//         "Trois"
```

**Utilisation intentionnelle** du fall-through:
```java
int mois = 2;
int jours;

switch (mois) {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        jours = 31;
        break;
    case 4: case 6: case 9: case 11:
        jours = 30;
        break;
    case 2:
        jours = 28;
        break;
    default:
        jours = 0;
}
System.out.println(jours);  // 28
```

### default Case

- Peut être placé **n'importe où** dans le switch
- S'exécute si **aucun case** ne correspond
- **Optionnel** mais recommandé

```java
int x = 10;
switch (x) {
    default:
        System.out.println("Défaut");  // Peut être au début
        break;
    case 1:
        System.out.println("Un");
        break;
    case 2:
        System.out.println("Deux");
        break;
}
```

### Switch Expressions (Java 14+)

**Nouvelle syntaxe** avec `->` (arrow):

```java
int jour = 3;
String nomJour = switch (jour) {
    case 1 -> "Lundi";
    case 2 -> "Mardi";
    case 3 -> "Mercredi";
    case 4 -> "Jeudi";
    case 5 -> "Vendredi";
    case 6, 7 -> "Week-end";  // Cases multiples
    default -> "Invalide";
};
System.out.println(nomJour);  // "Mercredi"
```

**Avantages**:
- Pas besoin de `break`
- Pas de fall-through accidentel
- Retourne une valeur directement
- Plus concis

**Avec bloc de code**:
```java
int mois = 2;
int jours = switch (mois) {
    case 1, 3, 5, 7, 8, 10, 12 -> 31;
    case 4, 6, 9, 11 -> 30;
    case 2 -> {
        System.out.println("Février");
        yield 28;  // yield pour retourner une valeur dans un bloc
    }
    default -> 0;
};
```

**yield keyword**: utilisé pour retourner une valeur depuis un bloc dans un switch expression.

## Writing while Loops

Les **loops** permettent de répéter du code.

### The while Loop

**Syntaxe**:
```java
while (condition) {
    // Code répété tant que condition est true
}
```

**Exemple**:
```java
int compteur = 0;
while (compteur < 5) {
    System.out.println("Compteur: " + compteur);
    compteur++;
}
// Output: 0, 1, 2, 3, 4
```

**Important**: Si la condition est `false` au départ, le code **ne s'exécute jamais**:
```java
int x = 10;
while (x < 5) {
    System.out.println("Jamais exécuté");
}
```

**Boucle infinie** (à éviter):
```java
while (true) {
    System.out.println("Infini!");
    // Nécessite break pour sortir
}
```

### The do-while Loop

**Syntaxe**:
```java
do {
    // Code
} while (condition);  // Notez le point-virgule !
```

**Différence clé**: Le code s'exécute **au moins une fois**, même si la condition est `false`:

```java
int x = 10;
do {
    System.out.println("Exécuté une fois: " + x);
} while (x < 5);
// Output: "Exécuté une fois: 10"
```

**Exemple pratique** (menu):
```java
Scanner scanner = new Scanner(System.in);
int choix;

do {
    System.out.println("1. Option A");
    System.out.println("2. Option B");
    System.out.println("0. Quitter");
    choix = scanner.nextInt();
    
    switch (choix) {
        case 1:
            System.out.println("Option A sélectionnée");
            break;
        case 2:
            System.out.println("Option B sélectionnée");
            break;
    }
} while (choix != 0);
```

### Comparaison while vs do-while

```java
// while: peut ne jamais s'exécuter
int x = 10;
while (x < 5) {
    System.out.println("while: " + x);  // Jamais exécuté
}

// do-while: s'exécute au moins une fois
do {
    System.out.println("do-while: " + x);  // Exécuté une fois
} while (x < 5);
```

## Constructing for Loops

### The Traditional for Loop

**Syntaxe**:
```java
for (initialization; condition; update) {
    // Code
}
```

**Composants**:
1. **Initialization**: exécuté une seule fois au début
2. **Condition**: testée avant chaque itération
3. **Update**: exécuté après chaque itération

**Exemple basique**:
```java
for (int i = 0; i < 5; i++) {
    System.out.println("i = " + i);
}
// Output: 0, 1, 2, 3, 4
```

**Ordre d'exécution**:
```java
for (int i = 0; i < 3; i++) {
    System.out.println(i);
}
// 1. i = 0 (initialization)
// 2. i < 3? true (condition)
// 3. print 0 (body)
// 4. i++ → i = 1 (update)
// 5. i < 3? true (condition)
// 6. print 1 (body)
// 7. i++ → i = 2 (update)
// 8. i < 3? true (condition)
// 9. print 2 (body)
// 10. i++ → i = 3 (update)
// 11. i < 3? false (condition) → STOP
```

### Variations du for Loop

**Déclaration multiple**:
```java
for (int i = 0, j = 10; i < 5; i++, j--) {
    System.out.println("i=" + i + ", j=" + j);
}
// Output: i=0, j=10
//         i=1, j=9
//         ...
```

**Sections optionnelles**:
```java
// Sans initialization
int i = 0;
for (; i < 5; i++) {
    System.out.println(i);
}

// Sans update
for (int i = 0; i < 5;) {
    System.out.println(i);
    i++;
}

// Boucle infinie
for (;;) {
    System.out.println("Infini!");
    break;  // Nécessaire pour sortir
}
```

**Scope de la variable**:
```java
for (int i = 0; i < 5; i++) {
    System.out.println(i);
}
// System.out.println(i);  // ERREUR: i n'existe plus
```

### The Enhanced for Loop (for-each)

**Syntaxe**:
```java
for (type variable : collection) {
    // Code
}
```

**Utilisé pour parcourir** des arrays ou collections:

```java
// Avec array
int[] nombres = {1, 2, 3, 4, 5};
for (int nombre : nombres) {
    System.out.println(nombre);
}

// Avec List
List<String> noms = Arrays.asList("Alice", "Bob", "Charlie");
for (String nom : noms) {
    System.out.println(nom);
}
```

**Avantages**:
- Plus lisible
- Pas de gestion d'index
- Évite les erreurs d'index

**Limitations**:
- **Lecture seule**: ne peut pas modifier la collection
- Pas d'accès à l'index
- Parcourt toujours du début à la fin

```java
int[] tableau = {1, 2, 3, 4, 5};

// Modification ne fonctionne pas comme attendu
for (int nombre : tableau) {
    nombre = nombre * 2;  // Ne modifie PAS le tableau!
}
System.out.println(Arrays.toString(tableau));  // [1, 2, 3, 4, 5]

// Pour modifier, utiliser un for traditionnel
for (int i = 0; i < tableau.length; i++) {
    tableau[i] = tableau[i] * 2;  // Modifie le tableau
}
System.out.println(Arrays.toString(tableau));  // [2, 4, 6, 8, 10]
```

### Nested for Loops

```java
// Multiplication table
for (int i = 1; i <= 3; i++) {
    for (int j = 1; j <= 3; j++) {
        System.out.print(i + "x" + j + "=" + (i*j) + "  ");
    }
    System.out.println();
}
// Output:
// 1x1=1  1x2=2  1x3=3
// 2x1=2  2x2=4  2x3=6
// 3x1=3  3x2=6  3x3=9
```

**Pattern avec nested loops**:
```java
// Triangle d'étoiles
for (int i = 1; i <= 5; i++) {
    for (int j = 1; j <= i; j++) {
        System.out.print("*");
    }
    System.out.println();
}
// Output:
// *
// **
// ***
// ****
// *****
```

## Controlling Flow with Branching

Les **branching statements** modifient le flux normal d'exécution.

### The break Statement

**Sortie immédiate** d'une loop ou d'un switch:

```java
// Break dans une loop
for (int i = 0; i < 10; i++) {
    if (i == 5) {
        break;  // Sort de la loop quand i vaut 5
    }
    System.out.println(i);
}
// Output: 0, 1, 2, 3, 4
```

**Avec recherche**:
```java
int[] nombres = {1, 3, 5, 7, 9, 2, 4, 6};
int recherche = 9;
boolean trouve = false;

for (int nombre : nombres) {
    if (nombre == recherche) {
        trouve = true;
        break;  // Arrête dès que trouvé
    }
}
System.out.println(trouve ? "Trouvé" : "Non trouvé");
```

**Break dans nested loops**: ne sort que de la loop **immédiate**:
```java
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        if (j == 1) {
            break;  // Sort uniquement de la loop interne
        }
        System.out.print(i + "," + j + " ");
    }
    System.out.println();
}
// Output:
// 0,0
// 1,0
// 2,0
```

### Labeled break Statement

**Labels** permettent de sortir de plusieurs loops imbriquées:

```java
recherche:  // Label
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        System.out.print(i + "," + j + " ");
        if (i == 1 && j == 1) {
            break recherche;  // Sort des deux loops
        }
    }
    System.out.println();
}
// Output: 0,0 0,1 0,2
//         1,0 1,1
```

**Exemple pratique**:
```java
boolean trouve = false;
exterieur:  // Label
for (int i = 0; i < 5 && !trouve; i++) {
    for (int j = 0; j < 5; j++) {
        if (i * j > 6) {
            System.out.println("Trouvé à i=" + i + ", j=" + j);
            trouve = true;
            break exterieur;
        }
    }
}
```

### The continue Statement

**Passe à l'itération suivante** en sautant le reste du code:

```java
for (int i = 0; i < 10; i++) {
    if (i % 2 == 0) {
        continue;  // Saute les nombres pairs
    }
    System.out.println(i);
}
// Output: 1, 3, 5, 7, 9
```

**Différence break vs continue**:
```java
// Avec break
for (int i = 0; i < 5; i++) {
    if (i == 2) break;
    System.out.println(i);
}
// Output: 0, 1 (arrête complètement)

// Avec continue
for (int i = 0; i < 5; i++) {
    if (i == 2) continue;
    System.out.println(i);
}
// Output: 0, 1, 3, 4 (saute juste 2)
```

**Exemple pratique** (filtrage):
```java
for (int i = 1; i <= 20; i++) {
    if (i % 3 != 0) {
        continue;  // Saute si pas divisible par 3
    }
    System.out.println(i);  // Affiche seulement multiples de 3
}
// Output: 3, 6, 9, 12, 15, 18
```

### Labeled continue Statement

```java
exterieur:
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        if (j == 1) {
            continue exterieur;  // Passe à l'itération suivante de la loop externe
        }
        System.out.print(i + "," + j + " ");
    }
    System.out.println("Fin de ligne");  // Jamais exécuté
}
// Output: 0,0 1,0 2,0
```

### The return Statement

**Sort immédiatement** d'une méthode:

```java
public static int trouverPremier(int[] tableau, int valeur) {
    for (int i = 0; i < tableau.length; i++) {
        if (tableau[i] == valeur) {
            return i;  // Sort de la méthode immédiatement
        }
    }
    return -1;  // Valeur par défaut si non trouvé
}
```

**Différence avec break**:
- `break`: sort de la loop, continue la méthode
- `return`: sort de la méthode complètement

---

## Exemples Pratiques Complets

### Exemple 1: Calculatrice avec menu
```java
import java.util.Scanner;

public class Calculatrice {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        int choix;
        
        do {
            System.out.println("\n=== CALCULATRICE ===");
            System.out.println("1. Addition");
            System.out.println("2. Soustraction");
            System.out.println("3. Multiplication");
            System.out.println("4. Division");
            System.out.println("0. Quitter");
            System.out.print("Choix: ");
            choix = scanner.nextInt();
            
            if (choix == 0) {
                break;
            }
            
            if (choix < 1 || choix > 4) {
                System.out.println("Choix invalide!");
                continue;
            }
            
            System.out.print("Premier nombre: ");
            double a = scanner.nextDouble();
            System.out.print("Deuxième nombre: ");
            double b = scanner.nextDouble();
            
            double resultat = switch (choix) {
                case 1 -> a + b;
                case 2 -> a - b;
                case 3 -> a * b;
                case 4 -> {
                    if (b == 0) {
                        System.out.println("Division par zéro!");
                        yield 0;
                    }
                    yield a / b;
                }
                default -> 0;
            };
            
            if (choix != 4 || b != 0) {
                System.out.println("Résultat: " + resultat);
            }
            
        } while (choix != 0);
        
        System.out.println("Au revoir!");
    }
}
```

### Exemple 2: Validation de mot de passe
```java
public class ValidationMotDePasse {
    public static boolean estValide(String mdp) {
        if (mdp == null || mdp.length() < 8) {
            return false;
        }
        
        boolean aMajuscule = false;
        boolean aMinuscule = false;
        boolean aChiffre = false;
        boolean aSpecial = false;
        
        for (int i = 0; i < mdp.length(); i++) {
            char c = mdp.charAt(i);
            
            if (Character.isUpperCase(c)) {
                aMajuscule = true;
            } else if (Character.isLowerCase(c)) {
                aMinuscule = true;
            } else if (Character.isDigit(c)) {
                aChiffre = true;
            } else {
                aSpecial = true;
            }
            
            // Optimisation: arrêter dès que tous les critères sont remplis
            if (aMajuscule && aMinuscule && aChiffre && aSpecial) {
                break;
            }
        }
        
        return aMajuscule && aMinuscule && aChiffre && aSpecial;
    }
    
    public static void main(String[] args) {
        String[] motsDePasse = {
            "password",           // false
            "Password",           // false
            "Password1",          // false
            "Password1!",         // true
            "Abc123!@#"          // true
        };
        
        for (String mdp : motsDePasse) {
            System.out.println(mdp + ": " + estValide(mdp));
        }
    }
}
```

### Exemple 3: Recherche dans matrice
```java
public class RechercheMatrice {
    public static int[] trouver(int[][] matrice, int valeur) {
        recherche:
        for (int i = 0; i < matrice.length; i++) {
            for (int j = 0; j < matrice[i].length; j++) {
                if (matrice[i][j] == valeur) {
                    return new int[]{i, j};  // Position trouvée
                }
            }
        }
        return null;  // Non trouvé
    }
    
    public static void main(String[] args) {
        int[][] matrice = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
        };
        
        int[] position = trouver(matrice, 5);
        if (position != null) {
            System.out.println("Trouvé à [" + position[0] + "][" + position[1] + "]");
        } else {
            System.out.println("Non trouvé");
        }
    }
}
```

### Exemple 4: Nombres premiers
```java
public class NombresPremiers {
    public static boolean estPremier(int n) {
        if (n < 2) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;
        
        for (int i = 3; i * i <= n; i += 2) {
            if (n % i == 0) {
                return false;  // Diviseur trouvé
            }
        }
        return true;
    }
    
    public static void afficherPremiers(int max) {
        System.out.println("Nombres premiers jusqu'à " + max + ":");
        int compteur = 0;
        
        for (int i = 2; i <= max; i++) {
            if (!estPremier(i)) {
                continue;  // Sauter les non-premiers
            }
            
            System.out.print(i + " ");
            compteur++;
            
            if (compteur % 10 == 0) {
                System.out.println();  // Nouvelle ligne tous les 10 nombres
            }
        }
        System.out.println("\nTotal: " + compteur);
    }
    
    public static void main(String[] args) {
        afficherPremiers(100);
    }
}
```

---

## Points Clés du Chapter 3

✅ **if statement**: condition doit être un `boolean`, accolades recommandées

✅ **switch statement**: supporte `int`, `String`, `enum` (pas `long`, `float`, `double`, `boolean`)

✅ **Fall-through**: sans `break`, l'exécution continue dans les cases suivants

✅ **Switch expressions** (Java 14+): syntaxe `->`, pas de fall-through, retourne une valeur

✅ **while loop**: condition testée **avant**, peut ne jamais s'exécuter

✅ **do-while loop**: condition testée **après**, s'exécute **au moins une fois**

✅ **for loop**: trois sections (initialization; condition; update), toutes optionnelles

✅ **Enhanced for loop**: pour parcourir collections, lecture seule, pas d'index

✅ **break**: sort de la loop ou switch **immédiatement**

✅ **continue**: passe à l'**itération suivante**

✅ **Labels**: permettent de contrôler les loops imbriquées avec `break` ou `continue`

✅ **return**: sort de la **méthode** complètement

---

# Questions/Réponses - Chapter 3: Making Decisions

## Questions sur les if Statements

**Q1: Ce code compile-t-il?**
```java
int x = 5;
if (x) {
    System.out.println("x est vrai");
}
```

**R:** Non, **erreur de compilation**: "incompatible types: int cannot be converted to boolean".

En Java, la condition d'un `if` doit **toujours** être un `boolean`. Contrairement au C, on ne peut pas utiliser `int` (où 0 = false, autre = true).

**Correction**:
```java
if (x != 0) {  // OK
    System.out.println("x est non-zéro");
}
```

---

**Q2: Que s'affiche-t-il?**
```java
int score = 85;
if (score >= 90)
    System.out.println("A");
    System.out.println("Excellent!");
System.out.println("Fin");
```

**R:**
```
Excellent!
Fin
```

**Piège**: Sans accolades, seule la **première instruction** après le `if` est conditionnelle. `System.out.println("Excellent!");` s'exécute **toujours** car elle n'est pas dans le bloc `if`.

**Correct avec accolades**:
```java
if (score >= 90) {
    System.out.println("A");
    System.out.println("Excellent!");
}
```

---

**Q3: Pourquoi ce code affiche-t-il "C"?**
```java
int score = 95;
if (score >= 60)
    System.out.println("D");
else if (score >= 70)
    System.out.println("C");
else if (score >= 80)
    System.out.println("B");
else if (score >= 90)
    System.out.println("A");
```

**R:** Affiche `"D"` (pas "A"!).

Les conditions sont évaluées **dans l'ordre**. Dès que `score >= 60` est `true`, le code exécute `System.out.println("D")` et **ignore** toutes les autres conditions.

**Correction** (ordre décroissant):
```java
if (score >= 90)
    System.out.println("A");
else if (score >= 80)
    System.out.println("B");
else if (score >= 70)
    System.out.println("C");
else if (score >= 60)
    System.out.println("D");
```

---

**Q4: Quelle est la différence entre ces deux codes?**
```java
// Code A
if (x > 0)
    if (y > 0)
        System.out.println("Les deux positifs");
else
    System.out.println("x négatif");

// Code B
if (x > 0) {
    if (y > 0)
        System.out.println("Les deux positifs");
} else {
    System.out.println("x négatif");
}
```

**R:** **Code A**: Le `else` s'attache au `if` **le plus proche** (`if (y > 0)`), pas au premier.
- Si `x > 0` et `y <= 0` → affiche "x négatif" (incorrect!)

**Code B**: Avec les accolades, le `else` s'attache clairement au premier `if`.
- Si `x > 0` et `y <= 0` → n'affiche rien (correct)

**Règle**: Le `else` s'associe toujours au `if` le plus proche non apparié. Utilisez des accolades pour clarifier!

---

## Questions sur les switch Statements

**Q5: Ce code compile-t-il?**
```java
long x = 100L;
switch (x) {
    case 100:
        System.out.println("Cent");
        break;
}
```

**R:** Non, **erreur de compilation**: "incompatible types: long cannot be converted to int".

Le `switch` ne supporte **pas** `long`, `float`, `double`, ou `boolean`.

**Types supportés**: `byte`, `short`, `char`, `int`, `String`, `enum` (et leurs wrappers).

---

**Q6: Que s'affiche-t-il?**
```java
int x = 2;
switch (x) {
    case 1:
        System.out.println("Un");
    case 2:
        System.out.println("Deux");
    case 3:
        System.out.println("Trois");
    default:
        System.out.println("Défaut");
}
```

**R:**
```
Deux
Trois
Défaut
```

**Fall-through**: Sans `break`, l'exécution **continue** dans les cases suivants, même s'ils ne correspondent pas.

---

**Q7: Où peut-on placer le case default?**
```java
int x = 5;
switch (x) {
    default:
        System.out.println("Défaut");
        break;
    case 1:
        System.out.println("Un");
        break;
    case 2:
        System.out.println("Deux");
        break;
}
```

**R:** Le `default` peut être placé **n'importe où** dans le switch (début, milieu, fin). C'est valide, mais conventionnellement on le met à la fin pour la lisibilité.

Si `x = 5`, affiche `"Défaut"` car aucun case ne correspond.

---

**Q8: Ce code compile-t-il? (Java 14+)**
```java
int jour = 3;
String resultat = switch (jour) {
    case 1 -> "Lundi";
    case 2 -> "Mardi";
    case 3 -> "Mercredi";
};
System.out.println(resultat);
```

**R:** Non, **erreur de compilation**: "the switch expression does not cover all possible input values".

Un **switch expression** doit être **exhaustif** (couvrir toutes les valeurs possibles). Il manque un `default` ou d'autres cases.

**Correction**:
```java
String resultat = switch (jour) {
    case 1 -> "Lundi";
    case 2 -> "Mardi";
    case 3 -> "Mercredi";
    default -> "Autre";  // Obligatoire
};
```

---

**Q9: Quelle est la différence entre ces deux syntaxes?**
```java
// Syntaxe A (traditionnelle)
switch (x) {
    case 1:
        System.out.println("Un");
        break;
    case 2:
        System.out.println("Deux");
        break;
}

// Syntaxe B (arrow, Java 14+)
switch (x) {
    case 1 -> System.out.println("Un");
    case 2 -> System.out.println("Deux");
}
```

**R:** 
**Syntaxe A**: 
- Nécessite `break` pour éviter fall-through
- Peut avoir fall-through intentionnel
- Ne retourne pas de valeur directement

**Syntaxe B (arrow)**:
- **Pas de fall-through** (automatique)
- Pas besoin de `break`
- Peut retourner une valeur (switch expression)
- Plus concise et sûre

---

**Q10: Que fait le keyword yield?**
```java
int mois = 2;
int jours = switch (mois) {
    case 1, 3, 5, 7, 8, 10, 12 -> 31;
    case 4, 6, 9, 11 -> 30;
    case 2 -> {
        System.out.println("Février");
        yield 28;  // ?
    }
    default -> 0;
};
```

**R:** `yield` est utilisé pour **retourner une valeur** depuis un **bloc** dans un switch expression.

- Pour une expression simple: `case 1 -> 31;` (pas besoin de yield)
- Pour un bloc: `case 2 -> { ...; yield 28; }` (yield obligatoire)

C'est l'équivalent de `return` mais pour les switch expressions.

---

## Questions sur les while Loops

**Q11: Combien de fois cette loop s'exécute-t-elle?**
```java
int x = 10;
while (x < 5) {
    System.out.println(x);
    x++;
}
```

**R:** **0 fois**. La condition `x < 5` est `false` dès le début, donc le code ne s'exécute **jamais**.

C'est la différence avec `do-while` qui s'exécute au moins une fois.

---

**Q12: Que s'affiche-t-il?**
```java
int x = 0;
while (x < 5)
    System.out.println(x);
    x++;
```

**R:** **Boucle infinie** affichant `0` indéfiniment.

**Piège**: Sans accolades, seul le `System.out.println(x)` est dans la loop. Le `x++` est **en dehors**, donc `x` ne change jamais.

**Correction**:
```java
while (x < 5) {
    System.out.println(x);
    x++;
}
```

---

**Q13: Quelle est la différence fondamentale entre while et do-while?**
```java
// while
int x = 10;
while (x < 5) {
    System.out.println("while: " + x);
}

// do-while
int y = 10;
do {
    System.out.println("do-while: " + y);
} while (y < 5);
```

**R:**
- **while**: Condition testée **avant** → peut **ne jamais s'exécuter** (0 fois)
- **do-while**: Condition testée **après** → s'exécute **au moins une fois**

Output:
```
do-while: 10
```

Le `while` n'affiche rien, le `do-while` affiche une fois.

---

**Q14: Ce code compile-t-il?**
```java
do {
    System.out.println("Hello");
} while (true)
```

**R:** Non, **erreur de compilation**: "';' expected".

Le `do-while` nécessite un **point-virgule** après la condition:
```java
do {
    System.out.println("Hello");
} while (true);  // Point-virgule obligatoire !
```

---

## Questions sur les for Loops

**Q15: Que s'affiche-t-il?**
```java
for (int i = 0; i < 5; i++) {
    System.out.println(i);
}
System.out.println(i);
```

**R:** **Erreur de compilation**: "cannot find symbol: variable i".

La variable `i` déclarée dans le `for` n'existe que dans le **scope de la loop**. Elle n'est pas accessible après.

**Correction** (si on veut accéder à `i` après):
```java
int i;
for (i = 0; i < 5; i++) {
    System.out.println(i);
}
System.out.println(i);  // OK, i = 5
```

---

**Q16: Combien de fois cette loop s'exécute-t-elle?**
```java
for (int i = 0; i < 10; i++) {
    if (i == 5) {
        break;
    }
    System.out.println(i);
}
```

**R:** **5 fois** (affiche 0, 1, 2, 3, 4).

Quand `i == 5`, le `break` **sort immédiatement** de la loop.

---

**Q17: Ce code compile-t-il?**
```java
for (int i = 0, long j = 10; i < j; i++, j--) {
    System.out.println(i + " " + j);
}
```

**R:** Non, **erreur de compilation**: "';' expected".

Dans la section initialization, toutes les variables doivent être du **même type**.

**Correction**:
```java
for (int i = 0, j = 10; i < j; i++, j--) {  // Même type: int
    System.out.println(i + " " + j);
}
```

ou

```java
for (long i = 0, j = 10; i < j; i++, j--) {  // Même type: long
    System.out.println(i + " " + j);
}
```

---

**Q18: Que s'affiche-t-il?**
```java
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        if (i == j) {
            continue;
        }
        System.out.print(i + "" + j + " ");
    }
}
```

**R:** `01 02 10 12 20 21`

Le `continue` **saute** l'itération quand `i == j` (diagonale), donc on n'affiche pas `00`, `11`, `22`.

---

**Q19: Quelle est la différence entre ces deux loops?**
```java
// Loop A
int[] tableau = {1, 2, 3, 4, 5};
for (int i = 0; i < tableau.length; i++) {
    tableau[i] = tableau[i] * 2;
}

// Loop B
for (int nombre : tableau) {
    nombre = nombre * 2;
}
```

**R:** 
**Loop A**: Modifie **effectivement** le tableau. Résultat: `[2, 4, 6, 8, 10]`

**Loop B**: Ne modifie **pas** le tableau. Résultat: `[1, 2, 3, 4, 5]` (inchangé)

Dans le **enhanced for loop** (for-each), la variable `nombre` est une **copie**. Modifier `nombre` ne change pas l'élément du tableau.

---

**Q20: Ce code est-il valide?**
```java
for (;;) {
    System.out.println("Infini!");
}
```

**R:** Oui, c'est **valide** et crée une **boucle infinie**.

Toutes les sections du `for` sont **optionnelles**. `for (;;)` est équivalent à `while (true)`.

Pour sortir, il faut un `break`, `return`, ou terminer le programme.

---

## Questions sur le Branching

**Q21: Que s'affiche-t-il?**
```java
for (int i = 0; i < 10; i++) {
    if (i % 2 == 0) {
        continue;
    }
    if (i > 7) {
        break;
    }
    System.out.print(i + " ");
}
```

**R:** `1 3 5 7`

- `continue` saute les nombres **pairs** (0, 2, 4, 6, 8)
- `break` arrête la loop quand `i > 7` (donc après 7)
- On affiche seulement les nombres **impairs** jusqu'à 7

---

**Q22: Combien de fois "Inner" s'affiche-t-il?**
```java
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        if (j == 1) {
            break;
        }
        System.out.println("Inner");
    }
}
```

**R:** **3 fois**.

Le `break` sort uniquement de la **loop interne** (celle de `j`). La loop externe (celle de `i`) continue normalement.

Pour chaque `i` (0, 1, 2), on affiche "Inner" une fois (quand `j = 0`), puis `break` quand `j = 1`.

---

**Q23: Que fait ce code avec le labeled break?**
```java
exterieur:
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        System.out.print(i + "" + j + " ");
        if (i == 1 && j == 1) {
            break exterieur;
        }
    }
    System.out.println();
}
System.out.println("Fin");
```

**R:**
```
00 01 02
10 11 Fin
```

Le `break exterieur` sort **des deux loops** (externe et interne) quand `i == 1 && j == 1`.

Sans le label, `break` sortirait seulement de la loop interne et continuerait avec `i = 1, j = 2`.

---

**Q24: Quelle est la différence entre break et continue?**
```java
for (int i = 0; i < 5; i++) {
    if (i == 2) break;     // ?
    System.out.println(i);
}

for (int i = 0; i < 5; i++) {
    if (i == 2) continue;  // ?
    System.out.println(i);
}
```

**R:**
**Avec break**: `0 1` (arrête complètement la loop)

**Avec continue**: `0 1 3 4` (saute juste l'itération où `i = 2`, puis continue)

- `break`: **sort** de la loop
- `continue`: passe à l'**itération suivante**

---

**Q25: Que s'affiche-t-il?**
```java
exterieur:
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        if (j == 1) {
            continue exterieur;
        }
        System.out.print(i + "" + j + " ");
    }
    System.out.print("X ");
}
```

**R:** `00 10 20`

Le `continue exterieur` passe à l'**itération suivante** de la loop **externe**.

Quand `j == 1`, on saute directement à l'itération suivante de `i`, donc:
- On n'affiche jamais `X` (cette ligne est sautée)
- On n'affiche que `j = 0` pour chaque `i`

---

## Questions sur return

**Q26: Quelle est la différence entre break et return?**
```java
public static void methodeA() {
    for (int i = 0; i < 5; i++) {
        if (i == 2) {
            break;
        }
        System.out.println("A: " + i);
    }
    System.out.println("Fin de methodeA");
}

public static void methodeB() {
    for (int i = 0; i < 5; i++) {
        if (i == 2) {
            return;
        }
        System.out.println("B: " + i);
    }
    System.out.println("Fin de methodeB");
}
```

**R:**
**methodeA**:
```
A: 0
A: 1
Fin de methodeA
```
`break` sort de la **loop**, puis continue la méthode.

**methodeB**:
```
B: 0
B: 1
```
`return` sort **immédiatement de la méthode**, donc "Fin de methodeB" n'est jamais affiché.

---

## Questions Pièges Complexes

**Q27: Que s'affiche-t-il?**
```java
int x = 5;
switch (x) {
    case 5: System.out.print("5 ");
    case 4: System.out.print("4 ");
    case 3: System.out.print("3 ");
    default: System.out.print("D ");
    case 2: System.out.print("2 ");
}
```

**R:** `5 4 3 D 2`

**Fall-through complet** depuis `case 5` jusqu'à la fin, incluant le `default` même s'il est au milieu.

L'ordre physique dans le code détermine le flux d'exécution en cas de fall-through.

---

**Q28: Combien de fois "Hello" s'affiche-t-il?**
```java
for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
        if (i == j) {
            break;
        }
        System.out.println("Hello");
    }
}
```

**R:** **10 fois**.

Pour chaque `i`:
- `i = 0`: `j` va de 0 à 4, mais break quand `j = 0` → 0 fois
- `i = 1`: `j` va de 0 à 4, affiche pour `j = 0`, break quand `j = 1` → 1 fois
- `i = 2`: affiche pour `j = 0, 1`, break quand `j = 2` → 2 fois
- `i = 3`: affiche pour `j = 0, 1, 2`, break quand `j = 3` → 3 fois
- `i = 4`: affiche pour `j = 0, 1, 2, 3`, break quand `j = 4` → 4 fois

Total: 0 + 1 + 2 + 3 + 4 = **10**

---

**Q29: Ce code compile-t-il?**
```java
int x = 5;
switch (x) {
    case 5:
        int y = 10;
        break;
    case 10:
        int y = 20;  // ?
        break;
}
```

**R:** Non, **erreur de compilation**: "variable y is already defined".

Tous les `case` partagent le **même scope**. On ne peut pas déclarer deux variables `y` dans des cases différents.

**Correction** (utiliser des blocs):
```java
switch (x) {
    case 5: {
        int y = 10;
        break;
    }
    case 10: {
        int y = 20;  // OK maintenant (scope différent)
        break;
    }
}
```

---

**Q30: Que retourne cette méthode?**
```java
public static int mystere(int n) {
    for (int i = 2; i < n; i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}
```

**R:** Cette méthode teste si un nombre est **premier** (de façon simplifiée).

- Retourne `1` si `n` est premier (aucun diviseur trouvé)
- Retourne `0` si `n` n'est pas premier (diviseur trouvé)

Exemple:
- `mystere(7)` → `1` (premier)
- `mystere(8)` → `0` (divisible par 2)

Le `return` sort **immédiatement** dès qu'un diviseur est trouvé.

---