# Questions d'Entretien Java 17 - Guide Complet

Je vais vous préparer avec un maximum de questions sur Java 17 pur, sans frameworks externes !

## 🔥 FONDAMENTAUX JAVA

**Q1: Qu'est-ce que Java et quels sont ses principes fondamentaux ?**
Java est un langage orienté objet, portable grâce à la JVM, fortement typé. Ses principes : WORA (Write Once Run Anywhere), gestion automatique de la mémoire via le garbage collector, et robustesse grâce à la gestion des exceptions.

**Q2: Expliquez la différence entre JDK, JRE et JVM**
- JVM : machine virtuelle qui exécute le bytecode
- JRE : environnement d'exécution (JVM + bibliothèques)
- JDK : kit de développement (JRE + compilateur + outils de développement)

**Q3: Qu'est-ce que le bytecode ?**
C'est le code intermédiaire généré par le compilateur Java (.class), indépendant de la plateforme, que la JVM interprète et compile en code machine natif.

**Q4: Comment fonctionne le Garbage Collector ?**
Il libère automatiquement la mémoire des objets non référencés. Plusieurs algorithmes existent (Serial, Parallel, G1GC, ZGC). Il parcourt le graphe des objets depuis les racines et marque les objets accessibles, puis supprime les non-marqués.

**Q5: Quels sont les types de données primitifs en Java ?**
byte (8 bits), short (16), int (32), long (64), float (32), double (64), char (16), boolean (true/false).

## 🎯 ORIENTÉ OBJET

**Q6: Expliquez les 4 piliers de la POO**
- Encapsulation : cacher les détails d'implémentation
- Héritage : réutilisation du code via extends
- Polymorphisme : une interface, plusieurs implémentations
- Abstraction : définir des contrats via interfaces/classes abstraites

**Q7: Différence entre classe abstraite et interface ?**
Classe abstraite : peut avoir des méthodes concrètes, constructeurs, attributs d'instance, héritage simple. Interface : méthodes abstraites par défaut (mais peut avoir default/static depuis Java 8), pas de constructeurs, implémentation multiple.

**Q8: Qu'est-ce que le polymorphisme ? Donnez des exemples**
Capacité d'un objet à prendre plusieurs formes. Overloading (compile-time) : même nom, paramètres différents. Overriding (runtime) : redéfinir une méthode héritée.

**Q9: Expliquez les modificateurs d'accès**
- private : accessible uniquement dans la classe
- default (package-private) : accessible dans le package
- protected : package + sous-classes
- public : accessible partout

**Q10: Différence entre this et super ?**
this : référence l'instance courante. super : référence la classe parente, utilisé pour appeler constructeur ou méthode parent.

## 📦 COLLECTIONS

**Q11: Quelle est la hiérarchie des Collections ?**
Collection (interface racine) → List, Set, Queue. Map est séparée. List : ArrayList, LinkedList, Vector. Set : HashSet, TreeSet, LinkedHashSet. Map : HashMap, TreeMap, LinkedHashMap, Hashtable.

**Q12: Différence entre ArrayList et LinkedList ?**
ArrayList : tableau dynamique, accès rapide O(1), insertion/suppression lente O(n). LinkedList : liste doublement chaînée, accès O(n), insertion/suppression rapide O(1) si on a la référence.

**Q13: Différence entre HashSet et TreeSet ?**
HashSet : basé sur HashMap, non ordonné, O(1) pour add/remove/contains. TreeSet : basé sur TreeMap, ordonné (ordre naturel ou Comparator), O(log n).

**Q14: Différence entre HashMap et Hashtable ?**
HashMap : non synchronisée, accepte null (une clé, plusieurs valeurs), plus rapide. Hashtable : synchronisée, refuse null, legacy.

**Q15: Comment fonctionne HashMap en interne ?**
Tableau de buckets (array de Entry). Hash code détermine le bucket. Collisions gérées par liste chaînée (ou arbre si >8 éléments depuis Java 8). Redimensionnement quand load factor dépassé (0.75 par défaut).

**Q16: Qu'est-ce que le load factor ?**
Seuil de remplissage (capacity × load factor) déclenchant le rehashing. Par défaut 0.75, compromis entre espace et temps.

**Q17: Différence entre Comparable et Comparator ?**
Comparable : ordre naturel, interface avec compareTo(), une seule implémentation. Comparator : ordre personnalisé, interface avec compare(), plusieurs implémentations possibles.

**Q18: Qu'est-ce que fail-fast et fail-safe ?**
Fail-fast : lève ConcurrentModificationException si modifié pendant itération (ArrayList, HashMap). Fail-safe : copie la collection, pas d'exception (CopyOnWriteArrayList, ConcurrentHashMap).

**Q19: Différence entre Iterator et ListIterator ?**
Iterator : parcours unidirectionnel, remove(). ListIterator : bidirectionnel, add(), set(), indices disponibles, uniquement pour List.

**Q20: Qu'est-ce que ConcurrentHashMap ?**
Map thread-safe sans synchroniser toute la map. Utilise le segment locking (Java 7) puis CAS et synchronisation fine (Java 8+). Meilleure performance que Hashtable.

## 🧵 MULTITHREADING

**Q21: Comment créer un thread en Java ?**
Deux façons : extends Thread et override run(), ou implements Runnable et passer à Thread. Depuis Java 8, aussi avec lambda : new Thread(() -> {...}).start()

**Q22: Différence entre Thread et Runnable ?**
Runnable est une interface fonctionnelle (séparation tâche/thread), permet d'hériter d'autre classe. Thread est une classe, héritage simple uniquement.

**Q23: Qu'est-ce que la synchronisation ?**
Mécanisme pour contrôler l'accès concurrent aux ressources partagées via le mot-clé synchronized (méthode ou bloc). Garantit qu'un seul thread exécute la section critique.

**Q24: Différence entre wait() et sleep() ?**
wait() : libère le verrou (monitor), doit être dans bloc synchronized, attend notify()/notifyAll(). sleep() : garde le verrou, pause le thread pour une durée.

**Q25: Qu'est-ce qu'un deadlock ?**
Situation où 2+ threads s'attendent mutuellement indéfiniment. Exemple : Thread1 lock A attend B, Thread2 lock B attend A. Prévention : ordre d'acquisition, timeout.

**Q26: Expliquez volatile**
Mot-clé garantissant la visibilité des modifications entre threads (pas de cache CPU). Pas d'atomicité pour opérations composées. Utile pour flags booléens.

**Q27: Qu'est-ce que l'Executor Framework ?**
API haut niveau pour gérer les threads (java.util.concurrent). ExecutorService gère un pool de threads. Évite création/destruction manuelle. Méthodes : execute(), submit(), shutdown().

**Q28: Différence entre execute() et submit() ?**
execute() : prend Runnable, void, pas de retour. submit() : prend Runnable/Callable, retourne Future pour récupérer résultat ou exception.

**Q29: Qu'est-ce qu'un Callable ?**
Interface comme Runnable mais retourne un résultat et peut lancer des exceptions checked. Méthode call() au lieu de run().

**Q30: Qu'est-ce qu'un Future ?**
Représente le résultat d'une opération asynchrone. Méthodes : get() (bloquant), cancel(), isDone(), isCancelled().

**Q31: Différence entre CountDownLatch et CyclicBarrier ?**
CountDownLatch : compteur décrémenté, attend() jusqu'à 0, usage unique. CyclicBarrier : threads attendent mutuellement, réutilisable, peut exécuter action quand tous arrivent.

**Q32: Qu'est-ce qu'un Semaphore ?**
Contrôle l'accès à une ressource limitée. Gère n permits. acquire() prend permit, release() libère. Utile pour limiter connexions simultanées.

**Q33: Expliquez le pattern Producer-Consumer**
Producer produit des données, Consumer les consomme. BlockingQueue résout le problème : put() bloque si pleine, take() bloque si vide. Synchronisation automatique.

## 🔧 GESTION MÉMOIRE

**Q34: Expliquez les zones mémoire de la JVM**
- Heap : objets, partagé entre threads, GC
- Stack : variables locales, appels méthodes, par thread
- Metaspace : métadonnées classes (remplace PermGen depuis Java 8)
- Code Cache : code compilé JIT

**Q35: Différence entre Stack et Heap ?**
Stack : LIFO, taille limitée, rapide, scope méthode, types primitifs et références. Heap : dynamique, plus lent, objets, GC nécessaire, partagé.

**Q36: Qu'est-ce qu'une fuite mémoire en Java ?**
Objets non utilisés mais toujours référencés, donc non collectés par GC. Causes : collections statiques, listeners non déregistrés, caches sans limite.

**Q37: Comment éviter les fuites mémoire ?**
Nullifier références inutiles, utiliser WeakReference pour caches, fermer ressources (try-with-resources), déregistrer listeners, limiter taille collections statiques.

**Q38: Qu'est-ce que OutOfMemoryError ?**
Erreur quand JVM manque de mémoire. Types : Heap space (trop d'objets), Metaspace (trop de classes), GC overhead (GC prend >98% temps).

## 🎨 JAVA 8+ FEATURES

**Q39: Qu'est-ce qu'une interface fonctionnelle ?**
Interface avec une seule méthode abstraite. Annotée @FunctionalInterface (optionnel). Peut avoir default/static. Utilisable avec lambda. Ex : Runnable, Callable, Comparator.

**Q40: Qu'est-ce qu'une expression lambda ?**
Fonction anonyme concise : (params) -> expression. Remplace classes anonymes pour interfaces fonctionnelles. Ex : (a, b) -> a + b

**Q41: Qu'est-ce qu'une référence de méthode ?**
Raccourci pour lambda appelant une méthode existante. Syntaxe : Classe::méthode. Types : statique (Math::max), instance (String::length), constructeur (ArrayList::new).

**Q42: Expliquez l'API Stream**
Pipeline fonctionnel pour traiter collections. Opérations intermédiaires (lazy) : filter, map, sorted. Terminales (eager) : collect, forEach, reduce. Immutable, pas de modification source.

**Q43: Différence entre map() et flatMap() ?**
map() : transforme élément en élément (1-1), Stream<T> → Stream<R>. flatMap() : transforme élément en Stream (1-n), aplatit, Stream<T> → Stream<Stream<R>> → Stream<R>.

**Q44: Différence entre filter() et findFirst() ?**
filter() : opération intermédiaire, retourne Stream filtré. findFirst() : terminale, retourne Optional du premier élément.

**Q45: Qu'est-ce qu'Optional ?**
Conteneur pouvant contenir une valeur ou être vide. Évite NullPointerException. Méthodes : isPresent(), orElse(), orElseThrow(), map(), ifPresent().

**Q46: Quand utiliser Optional ?**
Pour valeurs de retour pouvant être absentes, jamais pour paramètres ou attributs. Alternative à retourner null. Force à gérer l'absence explicitement.

**Q47: Qu'est-ce que default dans les interfaces ?**
Méthode avec implémentation dans interface (depuis Java 8). Permet d'ajouter méthodes sans casser compatibilité. Classes peuvent override.

**Q48: Qu'est-ce que static dans les interfaces ?**
Méthode statique dans interface. Utile pour méthodes utilitaires liées à l'interface. Non héritée ni overridable.

## 📝 STRINGS

**Q49: String est-elle mutable ou immutable ?**
Immutable. Chaque modification crée un nouvel objet. Raisons : sécurité, thread-safety, optimisation (String pool), hashcode constant.

**Q50: Différence entre String, StringBuilder et StringBuffer ?**
String : immutable. StringBuilder : mutable, non thread-safe, rapide. StringBuffer : mutable, thread-safe (synchronized), plus lent.

**Q51: Qu'est-ce que le String pool ?**
Zone spéciale du Heap pour stocker littéraux String. Réutilise objets identiques. intern() force ajout au pool. Économise mémoire.

**Q52: Différence entre == et equals() pour String ?**
== compare références (adresses mémoire). equals() compare contenu (caractères). Toujours utiliser equals() pour comparer valeurs.

**Q53: Comment fonctionnent les expressions régulières en Java ?**
Pattern.compile(regex) compile l'expression. Matcher teste String. Méthodes : matches() (total), find() (partiel), replaceAll(). Classes dans java.util.regex.

## 🔐 EXCEPTION HANDLING

**Q54: Différence entre Exception et Error ?**
Exception : conditions récupérables, à gérer (checked/unchecked). Error : problèmes graves JVM, non récupérables (OutOfMemoryError, StackOverflowError).

**Q55: Différence entre checked et unchecked exceptions ?**
Checked : vérifiées à compilation, extends Exception, doivent être déclarées/gérées (IOException, SQLException). Unchecked : runtime, extends RuntimeException, optionnelles (NullPointerException, IllegalArgumentException).

**Q56: Expliquez try-catch-finally**
try : code risqué. catch : gère exception. finally : toujours exécuté (sauf System.exit()), même si return/exception. Utile pour cleanup.

**Q57: Qu'est-ce que try-with-resources ?**
Ferme automatiquement ressources implémentant AutoCloseable. Syntaxe : try (Resource r = ...) {...}. Depuis Java 7. Plus sûr que finally.

**Q58: Peut-on avoir plusieurs catch ?**
Oui, du plus spécifique au plus général. Depuis Java 7, multi-catch : catch (IOException | SQLException e).

**Q59: Quand créer une exception personnalisée ?**
Pour logique métier spécifique, meilleure expressivité. Extends Exception (checked) ou RuntimeException (unchecked). Ajouter constructeurs standards.

## 🏗️ CLASSES INTERNES

**Q60: Quels types de classes internes existent ?**
- Inner class (non-static) : accède membres externes
- Static nested class : pas d'accès instance externe
- Local class : dans méthode
- Anonymous class : sans nom, usage unique

**Q61: Quand utiliser une classe interne statique ?**
Quand logiquement liée à externe mais indépendante de l'instance. Économise mémoire (pas de référence implicite). Ex : Builder pattern.

**Q62: Qu'est-ce qu'une classe anonyme ?**
Classe sans nom, déclarée et instanciée en une expression. Usage unique, souvent pour interfaces/classes abstraites. Remplacée par lambdas (interfaces fonctionnelles).

## 🔄 SERIALIZATION

**Q63: Qu'est-ce que la sérialisation ?**
Conversion objet en flux d'octets pour stockage/transmission. Classe doit implémenter Serializable. Utilise ObjectOutputStream/ObjectInputStream.

**Q64: Qu'est-ce que serialVersionUID ?**
Identifiant unique de version de classe sérialisable. Si change, InvalidClassException lors désérialisation. Toujours déclarer explicitement.

**Q65: Comment exclure un champ de la sérialisation ?**
Mot-clé transient. Le champ sera null/0/false après désérialisation. Utile pour données sensibles ou non sérialisables.

**Q66: Différence entre Serializable et Externalizable ?**
Serializable : automatique, simple, contrôle limité. Externalizable : manuel, methods writeExternal()/readExternal(), contrôle total, meilleure performance.

## 🎯 JAVA 9-17 FEATURES

**Q67: Qu'est-ce que le système de modules (Java 9) ?**
JPMS (Java Platform Module System). Fichier module-info.java définit dépendances. exports/requires contrôlent visibilité. Encapsulation forte, JRE plus petit.

**Q68: Qu'est-ce que var (Java 10) ?**
Inférence de type local. var list = new ArrayList<String>(); Uniquement variables locales, pas attributs/paramètres/retours. Type déterminé à compilation.

**Q69: Qu'est-ce que les Records (Java 14-16) ?**
Classes immuables compactes pour données. record Point(int x, int y) {} génère constructeur, getters, equals(), hashCode(), toString(). Finale, extends Record.

**Q70: Qu'est-ce que les Sealed Classes (Java 17) ?**
Contrôle quelles classes peuvent hériter. sealed class Shape permits Circle, Square {...}. Sous-classes : final, sealed, ou non-sealed. Hiérarchie fermée.

**Q71: Qu'est-ce que le Pattern Matching for instanceof (Java 16) ?**
Simplifie cast après instanceof. Avant : if (obj instanceof String) { String s = (String) obj; }. Après : if (obj instanceof String s) { // use s }

**Q72: Qu'est-ce que les Text Blocks (Java 15) ?**
Strings multi-lignes avec """. Préserve formatage, échappe quotes automatiquement. Utile pour JSON, SQL, HTML.

**Q73: Qu'est-ce que Switch Expressions (Java 14) ?**
Switch retourne valeur. Syntaxe flèche (->), pas de break, yield pour blocs. var result = switch(day) { case MON -> "Bad"; case FRI -> "Good"; };

**Q74: Différence entre switch statement et switch expression ?**
Statement : exécute code, break nécessaire, pas de retour. Expression : retourne valeur, pas de break, exhaustif (doit couvrir tous cas).

## 🔍 REFLECTION & ANNOTATIONS

**Q75: Qu'est-ce que la Reflection ?**
Inspecter/modifier classes, méthodes, attributs au runtime. Class.forName(), getMethod(), getField(). Utile pour frameworks, mais coûteux en performance et sécurité.

**Q76: Qu'est-ce qu'une annotation ?**
Métadonnées ajoutées au code. Préfixe @. Retention : SOURCE, CLASS, RUNTIME. Targets : TYPE, METHOD, FIELD, etc. Exemples : @Override, @Deprecated, @FunctionalInterface.

**Q77: Comment créer une annotation personnalisée ?**
@interface MonAnnotation { String value(); int count() default 1; }. Définir @Retention et @Target. Lire via Reflection.

## 📊 ENUMS

**Q78: Qu'est-ce qu'un Enum ?**
Type spécial de classe pour constantes. Thread-safe singleton. Peut avoir attributs, méthodes, constructeurs privés. Implicitent final et extends Enum.

**Q79: Peut-on hériter d'un Enum ?**
Non, tous les enums héritent implicitement de java.lang.Enum (final). Mais peut implémenter interfaces.

**Q80: Méthodes utiles des Enums ?**
values() : retourne tableau. valueOf(String) : obtient enum par nom. ordinal() : position. name() : nom constant.

## 🛠️ GENERICS

**Q81: Pourquoi utiliser les Generics ?**
Type-safety à compilation, pas de cast, code réutilisable. Évite ClassCastException. Exemple : List<String> au lieu de List.

**Q82: Qu'est-ce que le type erasure ?**
Suppression infos génériques à compilation pour compatibilité bytecode. List<String> devient List au runtime. Limitations : instanceof, new T[], T.class impossibles.

**Q83: Différence entre <? extends T> et <? super T> ?**
Extends (upper bound) : lecture seule, covariance. Super (lower bound) : écriture, contravariance. PECS : Producer Extends, Consumer Super.

**Q84: Qu'est-ce qu'un type paramétré ?**
Classe/interface/méthode avec paramètres de type. Exemple : class Box<T> { T value; }. T est type paramétré, remplacé lors utilisation.

**Q85: Peut-on créer un tableau générique ?**
Non directement à cause du type erasure. T[] arr = new T[10] interdit. Solution : (T[]) new Object[10] avec warning, ou ArrayList<T>.

## ⚡ PERFORMANCE & BEST PRACTICES

**Q86: Qu'est-ce que le principe DRY ?**
Don't Repeat Yourself. Éviter duplication code. Factoriser en méthodes/classes. Améliore maintenabilité.

**Q87: Qu'est-ce que SOLID ?**
5 principes POO : Single Responsibility, Open/Closed, Liskov Substitution, Interface Segregation, Dependency Inversion.

**Q88: Expliquez le principe "Fail Fast"**
Détecter erreurs le plus tôt possible. Valider paramètres, lancer exceptions immédiatement. Exemple : Objects.requireNonNull().

**Q89: Qu'est-ce que l'immutabilité ?**
Objet dont l'état ne change pas après création. Avantages : thread-safe, hashcode stable, cache-friendly. Exemple : String, Integer.

**Q90: Comment créer une classe immutable ?**
Classe final, attributs private final, pas de setters, deep copy en constructeur/getters pour objets mutables, pas de méthodes modifiant état.

**Q91: Qu'est-ce que le pattern Singleton ?**
Une seule instance de classe. Implémentation : constructeur privé, instance static, méthode getInstance(). Thread-safe avec enum ou synchronized.

**Q92: Différence entre == et equals() pour objets ?**
== compare références. equals() compare contenu (doit être overridé). Si equals() overridé, override aussi hashCode() (contrat).

**Q93: Contrat entre equals() et hashCode() ?**
Si a.equals(b), alors a.hashCode() == b.hashCode(). Inverse pas obligatoire (collisions ok). Importance pour HashMap, HashSet.

**Q94: Qu'est-ce que le pattern Builder ?**
Créer objets complexes étape par étape. Évite constructeurs avec trop de paramètres. Immutable avec final après build(). Lisible.

**Q95: Qu'est-ce que le pattern Factory ?**
Créer objets sans exposer logique création. Interface commune, implémentations différentes. Exemple : Collections.unmodifiableList().

## 🌐 I/O & NIO

**Q96: Différence entre InputStream et Reader ?**
InputStream : flux d'octets (binaire). Reader : flux de caractères (texte, encodage). Exemple : FileInputStream vs FileReader.

**Q97: Qu'est-ce que NIO ?**
New I/O (java.nio). Non-bloquant, orienté buffers/channels. Plus performant pour I/O concurrent. Classes : Path, Files, ByteBuffer, Channel.

**Q98: Différence entre Path et File ?**
File : legacy, méthodes limitées. Path : moderne (Java 7+), interface, plus de méthodes, meilleures exceptions, relativize(), normalize().

**Q99: Comment lire un fichier en Java moderne ?**
Files.readAllLines(path), Files.lines(path) (Stream), Files.readString(path) (Java 11+). Try-with-resources pour streams.

**Q100: Qu'est-ce qu'un ByteBuffer ?**
Buffer pour données binaires en NIO. Position, limit, capacity. Méthodes : flip(), clear(), compact(). Direct vs heap buffers.

## 🔢 MATH & UTILITAIRES

**Q101: Différence entre int et Integer ?**
int : type primitif, 32 bits, valeur par défaut 0. Integer : wrapper class, objet, null possible, méthodes utilitaires, autoboxing/unboxing.

**Q102: Qu'est-ce que l'autoboxing ?**
Conversion automatique primitif ↔ wrapper. int → Integer (boxing), Integer → int (unboxing). Attention performance (création objets).

**Q103: Méthodes utiles de la classe Math ?**
max/min, abs, pow, sqrt, random, round, floor, ceil, sin/cos/tan. Toutes static.

**Q104: Comment générer nombres aléatoires ?**
Math.random() (0.0-1.0), Random class (nextInt, nextDouble), ThreadLocalRandom (concurrent), SecureRandom (cryptographique).

**Q105: Différence entre Random et ThreadLocalRandom ?**
Random : synchronisé, contention en multithread. ThreadLocalRandom : instance par thread, meilleure performance concurrente.

## 📅 DATE & TIME API (Java 8+)

**Q106: Pourquoi nouvelle API Date/Time Java 8 ?**
java.util.Date/Calendar : mutable, pas thread-safe, API confuse, bugs. Nouvelle API : immutable, thread-safe, claire, basée sur Joda-Time.

**Q107: Classes principales de l'API Date/Time ?**
LocalDate (date), LocalTime (heure), LocalDateTime (date+heure), ZonedDateTime (avec timezone), Instant (timestamp UTC), Period (durée jours), Duration (durée temps).

**Q108: Différence entre Period et Duration ?**
Period : durée basée date (années, mois, jours). Duration : durée basée temps (heures, minutes, secondes, nanos). Period pour dates, Duration pour horaires.

**Q109: Comment parser/formater des dates ?**
DateTimeFormatter : parse(String), format(LocalDate). Patterns prédéfinis (ISO_DATE) ou personnalisés (dd-MM-yyyy). Thread-safe contrairement à SimpleDateFormat.

## 🔒 SECURITY

**Q110: Comment gérer les données sensibles ?**
Ne pas logger, utiliser char[] au lieu de String (pour mots de passe), effacer après usage, crypter en base, HTTPS pour transmission.

**Q111: Qu'est-ce que le package java.security ?**
Classes pour cryptographie, signatures, certificats. MessageDigest (hash), Cipher (encryption), KeyPairGenerator, SecureRandom.

**Q112: Différence entre encoding et encryption ?**
Encoding : transformation réversible sans clé (Base64, UTF-8). Encryption : chiffrement avec clé, sécurisé. Hashing : irréversible (SHA-256).

## 🎪 QUESTIONS AVANCÉES

**Q113: Qu'est-ce que le JIT Compiler ?**
Just-In-Time Compiler. Compile bytecode en code natif au runtime pour optimiser performance. HotSpot détecte code "chaud" (fréquent) et compile.

**Q114: Différence entre shallow copy et deep copy ?**
Shallow : copie références, objets partagés. Deep : copie objets récursivement, indépendants. clone() fait shallow par défaut.

**Q115: Qu'est-ce que le covariant return type ?**
Méthode override peut retourner sous-type du type retour parent. Depuis Java 5. Exemple : Object clone() overridé en MyClass clone().

**Q116: Qu'est-ce que le diamond problem ?**
Conflit héritage multiple quand deux parents ont même méthode. Java évite via héritage simple classes, mais interfaces peuvent (résolu avec default methods).

**Q117: Qu'est-ce que la classe System ?**
Classe utilitaire finale. Méthodes : currentTimeMillis(), arraycopy(), getProperty(), exit(), gc(). Accès stdin/stdout/stderr via in/out/err.

**Q118: Comment comparer performances de code ?**
System.nanoTime() (plus précis que currentTimeMillis), JMH (Java Microbenchmark Harness) pour benchmarks fiables. Warm-up JVM important.

**Q119: Qu'est-ce que les assertions ?**
Vérifications debug avec assert. Désactivées par défaut, activées avec -ea. Pour tests internes, pas validation production. Lance AssertionError.

**Q120: Différence entre composition et héritage ?**
Héritage : relation "est-un", fort couplage. Composition : relation "a-un", faible couplage, flexible. Préférer composition (principe SOLID).

---

## 💡 CONSEILS POUR L'ENTRETIEN

**Approche :**
- Répondez clairement et progressivement
- Donnez des exemples concrets de votre expérience
- N'hésitez pas à dessiner (architecture, diagrammes)
- Si vous ne savez pas, dites-le honnêtement
- Posez des questions de clarification
- Montrez votre raisonnement, pas juste la réponse

**Points à souligner :**
- Performance et optimisation
- Thread-safety et concurrence
- Design patterns
- Clean code et best practices
- Expérience pratique avec Java 17

Bonne chance pour votre entretien ! 💪🚀
























<br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br /> <br />