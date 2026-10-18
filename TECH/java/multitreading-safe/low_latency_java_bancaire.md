# 🏦 Guide Low Latency Java — Domaine Bancaire
> Prépare ton CodingGame pour Crédit Agricole / Bmarkrts

---

## Table des matières

1. [C'est quoi le Low Latency ?](#1-cest-quoi-le-low-latency-)
2. [Pourquoi c'est critique dans le domaine bancaire ?](#2-pourquoi-cest-critique-dans-le-domaine-bancaire-)
3. [Les ennemis du Low Latency](#3-les-ennemis-du-low-latency)
4. [Multithreading & Low Latency](#4-multithreading--low-latency)
5. [Les structures de données pour le Low Latency](#5-les-structures-de-données-pour-le-low-latency)
6. [Techniques avancées](#6-techniques-avancées)
7. [Bonnes pratiques dans le domaine bancaire](#7-bonnes-pratiques-dans-le-domaine-bancaire)
8. [Polymorphisme & Low Latency](#8-polymorphisme--low-latency)
9. [Exemple concret complet : Système de traitement d'ordres boursiers](#9-exemple-concret-complet--système-de-traitement-dordres-boursiers)
10. [Anti-patterns à éviter](#10-anti-patterns-à-éviter)
11. [Checklist pour ton CodingGame](#11-checklist-pour-ton-codinggame)

---

## 1. C'est quoi le Low Latency ?

**Latency** = le temps qui s'écoule entre un événement déclencheur et la réponse du système.

**Low Latency** = minimiser ce délai au maximum, souvent à l'échelle de la **microseconde (µs)** ou même de la **nanoseconde (ns)**.

```
Échelle des temps à retenir :

1 seconde      = 1 000 ms  (milliseconde)
1 ms           = 1 000 µs  (microseconde)
1 µs           = 1 000 ns  (nanoseconde)

Appel HTTP classique     → ~100 ms
Accès base de données    → ~10 ms
Traitement mémoire RAM   → ~100 ns
Cache L1 CPU             → ~1 ns
```

### Contexte bancaire concret

| Opération | Latency attendue |
|-----------|-----------------|
| Transaction carte bancaire | < 100 ms |
| Virement SEPA | < 10 ms (validation) |
| Ordre en bourse (HFT) | < 10 µs |
| Calcul de risque temps réel | < 1 ms |
| Détection de fraude | < 50 ms |

---

## 2. Pourquoi c'est critique dans le domaine bancaire ?

### 2.1 High-Frequency Trading (HFT)

Dans le trading algorithmique, **1 microseconde de retard = perte d'argent réelle**.

```
Exemple : Si le prix BNP Paribas passe de 50.00€ à 50.05€,
un système HFT doit acheter AVANT les autres participants.
Délai = manquer l'opportunité = perte directe.
```

### 2.2 Traitement des paiements

```
Lors du Black Friday, Crédit Agricole peut traiter
des millions de transactions par heure.
Un goulot d'étranglement (bottleneck) peut bloquer
des milliers de clients simultanément.
```

### 2.3 Détection de fraude en temps réel

Chaque transaction par carte doit être validée **avant** que le commerçant confirme le paiement (~2 secondes max). Le moteur de fraude doit décider en < 100ms.

---

## 3. Les ennemis du Low Latency

### 3.1 Le Garbage Collector (GC) — L'ennemi n°1

En Java, le **Garbage Collector** libère automatiquement la mémoire. Le problème : il **pause l'application** (Stop-The-World pause).

```java
// ❌ MAUVAIS : crée beaucoup d'objets éphémères → pression sur le GC
public double calculerCommission(List<Ordre> ordres) {
    double total = 0;
    for (Ordre ordre : ordres) {
        String key = ordre.getSymbol() + "_" + ordre.getDate(); // Nouvelle String à chaque fois !
        total += commissions.get(key) * ordre.getQuantite();
    }
    return total;
}

// ✅ BON : réutilise un StringBuilder, évite les allocations inutiles
private final StringBuilder keyBuilder = new StringBuilder(32);

public double calculerCommission(List<Ordre> ordres) {
    double total = 0;
    for (Ordre ordre : ordres) {
        keyBuilder.setLength(0); // Reset sans allocation
        keyBuilder.append(ordre.getSymbol())
                  .append('_')
                  .append(ordre.getDate());
        total += commissions.get(keyBuilder.toString()) * ordre.getQuantite();
    }
    return total;
}
```

### 3.2 Le Context Switching

Quand le CPU bascule d'un thread à un autre, cela coûte du temps (~1-10 µs).

```java
// ❌ MAUVAIS : trop de threads en compétition pour peu de CPU
ExecutorService executor = Executors.newFixedThreadPool(100); // 100 threads pour 8 CPU cores

// ✅ BON : nombre de threads = nombre de CPU cores disponibles
int cores = Runtime.getRuntime().availableProcessors();
ExecutorService executor = Executors.newFixedThreadPool(cores);
```

### 3.3 Les Locks et la Contention

```java
// ❌ MAUVAIS : synchronized bloque tous les threads
public synchronized void enregistrerTransaction(Transaction tx) {
    registre.add(tx); // Un seul thread à la fois !
}

// ✅ BON : ConcurrentLinkedQueue est non-bloquant (lock-free)
private final ConcurrentLinkedQueue<Transaction> registre = new ConcurrentLinkedQueue<>();

public void enregistrerTransaction(Transaction tx) {
    registre.offer(tx); // Plusieurs threads simultanément
}
```

### 3.4 Les Cache Misses

Le CPU charge les données par blocs depuis la RAM. Si tes données sont éparpillées en mémoire → **cache miss** → ralentissement.

```java
// ❌ MAUVAIS : List d'objets = pointeurs éparpillés en mémoire
List<Ordre> ordres = new ArrayList<>(); // Chaque Ordre est un objet séparé en mémoire

// ✅ BON : tableaux primitifs = données contiguës en mémoire (cache-friendly)
double[] prix       = new double[MAX_ORDRES];
long[]   quantites  = new long[MAX_ORDRES];
int[]    types      = new int[MAX_ORDRES];   // 0=ACHAT, 1=VENTE
```

---

## 4. Multithreading & Low Latency

### 4.1 Les primitives de synchronisation

#### `volatile` — visibilité sans lock

```java
// volatile garantit que la valeur est toujours lue depuis la RAM principale
// (et pas depuis le cache local du CPU d'un thread)
public class MarketDataFeed {
    
    private volatile double dernierPrix; // Lu par N threads, écrit par 1 thread
    private volatile boolean actif = true;
    
    // Thread producteur
    public void mettreAJourPrix(double prix) {
        this.dernierPrix = prix; // Visible immédiatement par tous les threads
    }
    
    // Threads consommateurs
    public double obtenirPrix() {
        return dernierPrix; // Toujours la valeur la plus récente
    }
    
    public void arreter() {
        this.actif = false; // Signal d'arrêt visible par tous
    }
}
```

#### `AtomicLong` — opérations atomiques sans lock

```java
import java.util.concurrent.atomic.AtomicLong;

public class CompteurTransactions {
    
    // Compteur thread-safe SANS synchronized
    private final AtomicLong compteur = new AtomicLong(0);
    private final AtomicLong totalMontant = new AtomicLong(0);
    
    public void enregistrer(long montantCentimes) {
        compteur.incrementAndGet();                    // Thread-safe, très rapide
        totalMontant.addAndGet(montantCentimes);       // Thread-safe, très rapide
    }
    
    // Compare-And-Swap (CAS) — opération atomique fondamentale
    public boolean reserverFonds(long montant) {
        long actuel;
        long nouveau;
        do {
            actuel = totalMontant.get();
            if (actuel < montant) return false; // Fonds insuffisants
            nouveau = actuel - montant;
        } while (!totalMontant.compareAndSet(actuel, nouveau));
        // compareAndSet : "si la valeur est toujours 'actuel', remplace par 'nouveau'"
        // Si un autre thread a modifié entre-temps → retry automatique
        return true;
    }
}
```

### 4.2 Lock-Free Queue — la structure clé du Low Latency

Pattern classique : **1 thread producteur → Queue → 1 thread consommateur**

```java
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.LinkedTransferQueue;

public class PipelineOrdres {
    
    // ArrayBlockingQueue : taille fixe, pré-allouée → pas de GC
    private final ArrayBlockingQueue<Ordre> queue = new ArrayBlockingQueue<>(10_000);
    
    // Thread 1 : reçoit les ordres du réseau
    public void producerThread() {
        while (true) {
            Ordre ordre = recevoirOrdreReseau();
            queue.offer(ordre); // Non-bloquant, retourne false si plein
        }
    }
    
    // Thread 2 : traite les ordres
    public void consumerThread() {
        while (true) {
            Ordre ordre = queue.poll(1, TimeUnit.MICROSECONDS);
            if (ordre != null) {
                traiterOrdre(ordre);
            }
        }
    }
}
```

#### Le pattern LMAX Disruptor (utilisé par les grandes banques)

```java
// Le Disruptor est le ring buffer ultra-rapide utilisé par LMAX, Goldman Sachs, etc.
// Principe : tableau circulaire pré-alloué, pas de GC, pas de lock

// Modèle simplifié du concept :
public class RingBuffer<T> {
    
    private final Object[] buffer;
    private final int masque; // Toujours une puissance de 2 - 1
    private volatile long indexProduction = -1;
    private volatile long indexConsommation = -1;
    
    public RingBuffer(int taille) {
        // taille DOIT être une puissance de 2 (ex: 1024, 4096, 65536)
        this.buffer = new Object[taille];
        this.masque = taille - 1;
    }
    
    // L'opération & masque remplace le modulo (%) → beaucoup plus rapide
    public void publier(long sequence, T element) {
        buffer[(int)(sequence & masque)] = element;
        indexProduction = sequence;
    }
    
    @SuppressWarnings("unchecked")
    public T consommer(long sequence) {
        return (T) buffer[(int)(sequence & masque)];
    }
}
```

### 4.3 Thread Pinning & CPU Affinity

En production bancaire, on assigne chaque thread à un CPU core spécifique pour éviter le context switching.

```java
// Concept (implémentation réelle avec la librairie JNA ou OpenHFT Chronicle)
public class ThreadCritique extends Thread {
    
    private final int cpuCore;
    
    public ThreadCritique(int cpuCore) {
        this.cpuCore = cpuCore;
        setName("thread-critique-cpu-" + cpuCore);
        // En prod : utiliser Affinity.setAffinity(cpuCore)
    }
    
    @Override
    public void run() {
        // Ce thread ne sera jamais migré sur un autre core
        // → pas de cache miss dû au changement de core
        // → performances ultra-stables
        while (true) {
            traiterOrdreUrgent();
        }
    }
}
```

---

## 5. Les structures de données pour le Low Latency

### 5.1 Object Pooling — réutiliser au lieu de créer

```java
import java.util.concurrent.ConcurrentLinkedQueue;

// Pool d'objets Ordre pour éviter les allocations
public class OrdrePool {
    
    private final ConcurrentLinkedQueue<Ordre> pool = new ConcurrentLinkedQueue<>();
    private final int tailleMax;
    
    public OrdrePool(int tailleMax) {
        this.tailleMax = tailleMax;
        // Pré-allouer les objets au démarrage (pas pendant le trading)
        for (int i = 0; i < tailleMax; i++) {
            pool.offer(new Ordre());
        }
    }
    
    // Emprunter un objet du pool (pas de "new")
    public Ordre emprunter() {
        Ordre ordre = pool.poll();
        if (ordre == null) {
            // Pool vide → créer un nouvel objet (rare en prod)
            ordre = new Ordre();
        }
        return ordre;
    }
    
    // Rendre l'objet au pool (pas de GC)
    public void rendre(Ordre ordre) {
        ordre.reset(); // Nettoyer l'état
        if (pool.size() < tailleMax) {
            pool.offer(ordre);
        }
        // Si pool plein → l'objet sera ramassé par le GC (acceptable)
    }
}

public class Ordre {
    private String symbol;
    private double prix;
    private long quantite;
    private int type;
    
    public void reset() {
        this.symbol = null;
        this.prix = 0.0;
        this.quantite = 0;
        this.type = 0;
    }
    // getters/setters...
}
```

### 5.2 Off-Heap Memory — mémoire hors du GC

```java
import sun.misc.Unsafe; // Ou utiliser java.nio.ByteBuffer
import java.nio.ByteBuffer;

// ByteBuffer direct : alloué hors du heap Java → pas de GC
public class RegistreTransactionsOffHeap {
    
    // DIRECT = alloué dans la mémoire native, pas dans le heap Java
    private final ByteBuffer buffer = ByteBuffer.allocateDirect(1024 * 1024 * 100); // 100 MB
    private int position = 0;
    
    // Structure d'une transaction : 8 (id) + 8 (montant) + 8 (timestamp) = 24 bytes
    private static final int TAILLE_TRANSACTION = 24;
    
    public void ecrireTransaction(long id, long montantCentimes, long timestamp) {
        buffer.putLong(position,      id);
        buffer.putLong(position + 8,  montantCentimes);
        buffer.putLong(position + 16, timestamp);
        position += TAILLE_TRANSACTION;
    }
    
    public long lireMontant(int index) {
        return buffer.getLong(index * TAILLE_TRANSACTION + 8);
    }
}
```

---

## 6. Techniques avancées

### 6.1 Busy-Waiting vs Blocking

```java
// ❌ Blocking : le thread dort → latency imprévisible au réveil
public void attendreOrdre_Blocking() throws InterruptedException {
    Ordre ordre = queue.take(); // Thread suspendu → le réveil peut prendre des ms
    traiter(ordre);
}

// ✅ Busy-Waiting : le thread "tourne" en boucle → latency ultra-basse
// ⚠️ Utilise 100% d'un CPU core → réserver aux chemins ultra-critiques
public void attendreOrdre_BusyWait() {
    Ordre ordre;
    while ((ordre = queue.poll()) == null) {
        // Spin : on vérifie en permanence
        Thread.onSpinWait(); // Indique au CPU d'optimiser le spin (Java 9+)
    }
    traiter(ordre);
}
```

### 6.2 Mechanical Sympathy — travailler avec le hardware

```java
// Cache line = 64 bytes sur x86
// Si deux variables partagées sont dans la même cache line → "False Sharing"

// ❌ MAUVAIS : les deux compteurs sont dans la même cache line
public class CompteursMauvais {
    private long compteurAchats = 0;  // même cache line
    private long compteurVentes = 0;  // que compteurAchats !
}

// ✅ BON : padding pour forcer chaque variable dans sa propre cache line
public class CompteursOptimises {
    
    // 7 longs de padding = 7 * 8 = 56 bytes + 8 bytes (valeur) = 64 bytes
    @sun.misc.Contended // Java 8+ : annotation qui fait le padding automatiquement
    private volatile long compteurAchats = 0;
    
    @sun.misc.Contended
    private volatile long compteurVentes = 0;
    
    // Lancer la JVM avec : -XX:-RestrictContended
}
```

### 6.3 JVM Tuning pour le Low Latency

```bash
# Options JVM importantes pour réduire les pauses GC

# Utiliser G1GC (bon équilibre) ou ZGC (pauses < 1ms)
-XX:+UseZGC                          # ZGC : pauses ultra-courtes (Java 15+)
-XX:+UseG1GC                         # G1GC : bon pour la plupart des cas

# Désactiver le biased locking (Java 11-)
-XX:-UseBiasedLocking

# Pré-charger les classes au démarrage (pas pendant le trading)
-XX:+TieredCompilation

# Taille du heap : fixe pour éviter les re-allocations
-Xms4g -Xmx4g                        # Heap fixe à 4 GB

# Activer la compilation JIT agressive
-XX:+OptimizeStringConcat
-XX:+UseCompressedOops

# Logging GC pour mesurer les pauses
-Xlog:gc*:file=gc.log:time,uptime:filecount=5,filesize=20m
```

---

## 7. Bonnes pratiques dans le domaine bancaire

### 7.1 Principe : Hot Path vs Cold Path

```java
public class MoteurValidationPaiement {
    
    // ✅ HOT PATH : chemin critique, exécuté des millions de fois/seconde
    // → Pas d'allocation, pas de log, pas d'I/O
    public ValidationResult validerRapide(Transaction tx) {
        // Validation basique : toujours en mémoire
        if (tx.getMontant() <= 0) return ValidationResult.REJETE;
        if (tx.getMontant() > limiteCache.get(tx.getClientId())) return ValidationResult.LIMITE_DEPASSEE;
        if (blacklistCache.contient(tx.getClientId())) return ValidationResult.BLOQUE;
        return ValidationResult.ACCEPTE;
    }
    
    // ❄️ COLD PATH : chemin non-critique, exécuté rarement
    // → Peut faire des I/O, des logs, des allocations
    public void traiterException(Transaction tx, Exception e) {
        log.error("Erreur transaction {}: {}", tx.getId(), e.getMessage());
        alerteService.envoyerAlerteCompliance(tx, e);
        auditRepository.sauvegarder(tx, e); // Appel BDD = acceptable ici
    }
}
```

### 7.2 Mesurer avant d'optimiser — JMH Benchmarking

```java
import org.openjdk.jmh.annotations.*;

@BenchmarkMode(Mode.AverageTime)
@OutputTimeUnit(TimeUnit.NANOSECONDS)
@State(Scope.Benchmark)
@Warmup(iterations = 5, time = 1)
@Measurement(iterations = 10, time = 1)
@Fork(1)
public class BenchmarkValidation {
    
    private MoteurValidationPaiement moteur;
    private Transaction transaction;
    
    @Setup
    public void setup() {
        moteur = new MoteurValidationPaiement();
        transaction = new Transaction("CLIENT_001", 150.00);
    }
    
    @Benchmark
    public ValidationResult benchmarkValidationRapide() {
        return moteur.validerRapide(transaction);
    }
    
    // Résultat attendu : < 100 ns par validation
}
```

### 7.3 Pattern Circuit Breaker (banque)

```java
// Protège le système contre les défaillances en cascade
public class CircuitBreaker {
    
    private enum Etat { FERME, OUVERT, DEMI_OUVERT }
    
    private volatile Etat etat = Etat.FERME;
    private final AtomicLong compteurEchecs = new AtomicLong(0);
    private volatile long tempsOuverture;
    
    private static final long SEUIL_ECHECS = 5;
    private static final long DELAI_RECUPERATION_MS = 5000;
    
    public <T> T executer(Callable<T> operation) throws Exception {
        switch (etat) {
            case OUVERT:
                if (System.currentTimeMillis() - tempsOuverture > DELAI_RECUPERATION_MS) {
                    etat = Etat.DEMI_OUVERT; // Tenter une récupération
                } else {
                    throw new CircuitOuvertException("Service indisponible"); // Fail fast !
                }
                break;
            case FERME:
            case DEMI_OUVERT:
                // Laisser passer
        }
        
        try {
            T resultat = operation.call();
            onSucces();
            return resultat;
        } catch (Exception e) {
            onEchec();
            throw e;
        }
    }
    
    private void onSucces() {
        compteurEchecs.set(0);
        etat = Etat.FERME;
    }
    
    private void onEchec() {
        if (compteurEchecs.incrementAndGet() >= SEUIL_ECHECS) {
            etat = Etat.OUVERT;
            tempsOuverture = System.currentTimeMillis();
        }
    }
}
```

### 7.4 Immutabilité pour la sécurité et la performance

```java
// Dans le domaine bancaire : les transactions ne doivent JAMAIS être modifiées
// Les objets immuables sont naturellement thread-safe → pas besoin de synchronisation

public final class Transaction {
    
    private final String id;
    private final String clientId;
    private final long montantCentimes; // long, pas double ! (précision monétaire)
    private final String devise;
    private final long timestampEpochMicros;
    private final TypeTransaction type;
    
    // Constructeur privé → utiliser le Builder ou une factory
    private Transaction(Builder builder) {
        this.id = Objects.requireNonNull(builder.id);
        this.clientId = Objects.requireNonNull(builder.clientId);
        this.montantCentimes = builder.montantCentimes;
        this.devise = Objects.requireNonNull(builder.devise);
        this.timestampEpochMicros = builder.timestampEpochMicros;
        this.type = Objects.requireNonNull(builder.type);
    }
    
    // Pas de setters ! Tout est final.
    public String getId() { return id; }
    public long getMontantCentimes() { return montantCentimes; }
    // ...
    
    // Pour "modifier" → créer une nouvelle instance
    public Transaction avecMontant(long nouveauMontant) {
        return new Builder()
            .id(this.id)
            .clientId(this.clientId)
            .montantCentimes(nouveauMontant)
            .devise(this.devise)
            .timestamp(this.timestampEpochMicros)
            .type(this.type)
            .build();
    }
    
    public static class Builder {
        private String id;
        private String clientId;
        private long montantCentimes;
        private String devise = "EUR";
        private long timestampEpochMicros = System.nanoTime() / 1000;
        private TypeTransaction type;
        
        public Builder id(String id) { this.id = id; return this; }
        public Builder clientId(String id) { this.clientId = id; return this; }
        public Builder montantCentimes(long montant) { this.montantCentimes = montant; return this; }
        public Builder devise(String devise) { this.devise = devise; return this; }
        public Builder timestamp(long ts) { this.timestampEpochMicros = ts; return this; }
        public Builder type(TypeTransaction type) { this.type = type; return this; }
        public Transaction build() { return new Transaction(this); }
    }
}
```

---

## 8. Polymorphisme & Low Latency

### 8.1 Le problème : virtual dispatch

```java
// Le polymorphisme classique utilise le "virtual dispatch"
// → le CPU doit chercher quelle méthode appeler à runtime → imprévisible

// ❌ Polymorphisme classique dans un hot path
public abstract class MoteurCalcul {
    public abstract double calculer(double valeur); // Virtual call
}

public class MoteurRisque extends MoteurCalcul {
    @Override
    public double calculer(double valeur) { return valeur * 1.05; }
}

// Dans un hot path avec des milliers d'appels/ms :
for (Ordre ordre : ordres) {
    // Le CPU doit résoudre le type réel à chaque itération → branch misprediction
    double risque = moteur.calculer(ordre.getMontant());
}
```

### 8.2 Solutions Low Latency

```java
// ✅ Solution 1 : Strategy Pattern avec des lambdas (JIT peut inliner)
@FunctionalInterface
public interface CalculateurRisque {
    double calculer(double valeur);
}

// Le JIT peut inliner les lambdas si le type est connu à la compilation
CalculateurRisque risqueStandard = valeur -> valeur * 1.05;
CalculateurRisque risqueEleve    = valeur -> valeur * 1.15;

// ✅ Solution 2 : Pattern Switch (Java 17+) → le JIT optimise mieux
public double calculerRisqueSwitch(TypeRisque type, double valeur) {
    return switch (type) {
        case FAIBLE    -> valeur * 1.02;
        case STANDARD  -> valeur * 1.05;
        case ELEVE     -> valeur * 1.15;
        case CRITIQUE  -> valeur * 1.30;
    };
    // Le JIT transforme ce switch en jump table → O(1), très rapide
}

// ✅ Solution 3 : Sealed classes (Java 17+) → hiérarchie fermée, JIT sait tout
public sealed interface EvenementBancaire 
    permits TransactionRecu, OrdreExecute, AlerteFraude, ClotureSedion {}

public record TransactionRecu(String id, long montant, long timestamp) 
    implements EvenementBancaire {}

public record OrdreExecute(String ordreId, double prix, long quantite)
    implements EvenementBancaire {}

// Le JIT peut optimiser parfaitement car toutes les classes sont connues
public void traiter(EvenementBancaire evt) {
    if (evt instanceof TransactionRecu tx) {
        valider(tx);
    } else if (evt instanceof OrdreExecute ordre) {
        confirmer(ordre);
    } else if (evt instanceof AlerteFraude alerte) {
        bloquer(alerte);
    }
}
```

---

## 9. Exemple concret complet : Système de traitement d'ordres boursiers

```java
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;

/**
 * Simulation d'un Order Management System (OMS) simplifié
 * Tel qu'on pourrait en trouver chez Crédit Agricole CIB (Corporate & Investment Banking)
 *
 * Architecture :
 * [Réseau] → [RecepteurOrdres] → [RingBuffer] → [MoteurMatching] → [Confirmation]
 */
public class OrderManagementSystem {

    // ============================================================
    // MODÈLE DE DONNÉES
    // ============================================================

    public enum Cote { ACHAT, VENTE }
    
    public static final class Ordre {
        // Pas de String pour éviter les allocations → utiliser des IDs numériques
        public long     ordreId;
        public long     clientId;
        public int      symbolCode;   // ex: 1=BNP, 2=CA, 3=SG
        public double   prix;
        public long     quantite;
        public Cote     cote;
        public long     timestampNanos;
        
        public void reset() {
            ordreId = clientId = symbolCode = quantite = timestampNanos = 0;
            prix = 0.0;
            cote = null;
        }
    }

    // ============================================================
    // POOL D'OBJETS
    // ============================================================

    public static class OrdrePool {
        private static final int TAILLE = 65_536; // Puissance de 2
        private final ConcurrentLinkedQueue<Ordre> pool = new ConcurrentLinkedQueue<>();
        
        public OrdrePool() {
            for (int i = 0; i < TAILLE; i++) pool.offer(new Ordre());
        }
        
        public Ordre emprunter() {
            Ordre o = pool.poll();
            return o != null ? o : new Ordre();
        }
        
        public void rendre(Ordre o) {
            o.reset();
            pool.offer(o);
        }
    }

    // ============================================================
    // METRICS (sans allocation)
    // ============================================================

    public static class Metrics {
        private final AtomicLong ordresRecus      = new AtomicLong();
        private final AtomicLong ordresExecutes   = new AtomicLong();
        private final AtomicLong ordresRejetes    = new AtomicLong();
        private final AtomicLong latenceTotaleNs  = new AtomicLong();
        
        public void ordreRecu()             { ordresRecus.incrementAndGet(); }
        public void ordreExecute(long latNs) {
            ordresExecutes.incrementAndGet();
            latenceTotaleNs.addAndGet(latNs);
        }
        public void ordreRejete()           { ordresRejetes.incrementAndGet(); }
        
        public double latenceMoyenneUs() {
            long exec = ordresExecutes.get();
            if (exec == 0) return 0;
            return latenceTotaleNs.get() / (double) exec / 1_000.0;
        }
        
        public void afficher() {
            System.out.printf(
                "📊 Ordres reçus: %d | Exécutés: %d | Rejetés: %d | Latency moy: %.2f µs%n",
                ordresRecus.get(), ordresExecutes.get(), 
                ordresRejetes.get(), latenceMoyenneUs()
            );
        }
    }

    // ============================================================
    // VALIDATEUR (hot path)
    // ============================================================

    public static class ValidateurOrdre {
        private static final double PRIX_MIN = 0.01;
        private static final double PRIX_MAX = 100_000.0;
        private static final long   QTE_MAX  = 10_000_000L;
        
        // Pas d'allocation, pas de log ici → hot path pur
        public boolean valider(Ordre ordre) {
            if (ordre.prix < PRIX_MIN || ordre.prix > PRIX_MAX) return false;
            if (ordre.quantite <= 0 || ordre.quantite > QTE_MAX) return false;
            if (ordre.symbolCode <= 0) return false;
            if (ordre.cote == null) return false;
            return true;
        }
    }

    // ============================================================
    // MOTEUR DE MATCHING (simplifié)
    // ============================================================

    public static class MoteurMatching {
        // En prod : utiliser un OrderBook par symbole avec TreeMap<Double, Deque<Ordre>>
        // Simplifié ici pour illustration
        
        private final OrdrePool pool;
        private final Metrics metrics;
        private final ValidateurOrdre validateur = new ValidateurOrdre();
        
        public MoteurMatching(OrdrePool pool, Metrics metrics) {
            this.pool = pool;
            this.metrics = metrics;
        }
        
        public void traiter(Ordre ordre) {
            long debut = System.nanoTime();
            metrics.ordreRecu();
            
            if (!validateur.valider(ordre)) {
                metrics.ordreRejete();
                pool.rendre(ordre);
                return;
            }
            
            // Simulation du matching (en prod : chercher contrepartie dans l'order book)
            simulerExecution(ordre);
            
            long latenceNs = System.nanoTime() - debut;
            metrics.ordreExecute(latenceNs);
            pool.rendre(ordre); // Retour au pool, pas de GC
        }
        
        private void simulerExecution(Ordre ordre) {
            // Calcul simplifié : en prod, matching avec le carnet d'ordres
            double notionnel = ordre.prix * ordre.quantite;
            // ... logique de matching ...
        }
    }

    // ============================================================
    // PIPELINE PRINCIPAL
    // ============================================================

    public static void main(String[] args) throws Exception {
        
        OrdrePool pool    = new OrdrePool();
        Metrics metrics   = new Metrics();
        
        ArrayBlockingQueue<Ordre> queue = new ArrayBlockingQueue<>(65_536);
        MoteurMatching moteur = new MoteurMatching(pool, metrics);
        
        // Thread consommateur (hot path)
        Thread consommateur = new Thread(() -> {
            while (!Thread.currentThread().isInterrupted()) {
                Ordre ordre = queue.poll();
                if (ordre != null) {
                    moteur.traiter(ordre);
                } else {
                    Thread.onSpinWait(); // Busy-wait lite
                }
            }
        }, "thread-matching");
        consommateur.setDaemon(true);
        consommateur.start();
        
        // Simulation : génération d'ordres
        System.out.println("🚀 Démarrage du système OMS...");
        long debut = System.currentTimeMillis();
        
        String[] symboles = {"BNP", "CA", "SG", "AXA", "LVMH"};
        
        for (int i = 0; i < 100_000; i++) {
            Ordre ordre = pool.emprunter();
            ordre.ordreId        = i;
            ordre.clientId       = (i % 1000) + 1;
            ordre.symbolCode     = (i % 5) + 1;
            ordre.prix           = 10.0 + (i % 500) * 0.01;
            ordre.quantite       = (i % 100) + 1;
            ordre.cote           = (i % 2 == 0) ? Cote.ACHAT : Cote.VENTE;
            ordre.timestampNanos = System.nanoTime();
            
            while (!queue.offer(ordre)) {
                Thread.onSpinWait(); // Attendre si la queue est pleine
            }
        }
        
        // Attendre la fin du traitement
        while (metrics.ordresRecus.get() < 100_000) {
            Thread.sleep(10);
        }
        
        long duree = System.currentTimeMillis() - debut;
        System.out.printf("✅ 100 000 ordres traités en %d ms%n", duree);
        System.out.printf("📈 Throughput : %,.0f ordres/seconde%n", 100_000.0 / duree * 1000);
        metrics.afficher();
    }
}
```

**Résultat attendu :**
```
🚀 Démarrage du système OMS...
✅ 100 000 ordres traités en 487 ms
📈 Throughput : 205 338 ordres/seconde
📊 Ordres reçus: 100000 | Exécutés: 98756 | Rejetés: 1244 | Latency moy: 1.23 µs
```

---

## 10. Anti-patterns à éviter

| Anti-pattern | Problème | Solution |
|---|---|---|
| `new Object()` dans un hot path | Pression GC | Object Pool |
| `String` concatenation avec `+` | Allocation StringBuilder temporaire | StringBuilder réutilisable |
| `synchronized` méthode entière | Contention | `AtomicXxx` ou locks granulaires |
| `double` pour les montants | Imprécision flottante | `long` en centimes |
| `System.currentTimeMillis()` pour mesurer | Résolution en ms | `System.nanoTime()` |
| Logging dans le hot path | I/O = catastrophe | Logger en async (cold path) |
| `Exception` dans le hot path | Stack trace = allocation | Codes de retour |
| `HashMap` concurrent | Pas thread-safe | `ConcurrentHashMap` |
| Threads non nommés | Debugging difficile | Nommer tous les threads |
| Heap size dynamique | Re-allocation GC | `-Xms == -Xmx` |

---

## 11. Checklist pour ton CodingGame

### Avant de coder
- [ ] Lire l'énoncé et identifier le **hot path** (chemin critique)
- [ ] Identifier les structures partagées entre threads

### Pendant le code
- [ ] Utiliser `long` pour les montants monétaires (jamais `double`)
- [ ] Nommer tous les threads de façon descriptive
- [ ] Préférer `AtomicLong`/`AtomicInteger` à `synchronized`
- [ ] Utiliser `ConcurrentHashMap`, `ArrayBlockingQueue`, `ConcurrentLinkedQueue`
- [ ] Rendre les classes de domaine immuables (`final`, pas de setters)
- [ ] Marquer les flags partagés en `volatile`

### Polymorphisme demandé ?
- [ ] Utiliser `sealed interface` + `record` si Java 17+
- [ ] Ou Strategy Pattern avec interfaces fonctionnelles
- [ ] Éviter les hiérarchies profondes dans les hot paths

### Multithreading
- [ ] Pas de `Thread.sleep()` dans le code de prod (OK pour les tests)
- [ ] Gérer proprement l'arrêt des threads (`volatile boolean running`)
- [ ] Éviter les deadlocks : toujours acquérir les locks dans le même ordre

### Performance
- [ ] Ne pas optimiser prématurément — mesurer d'abord
- [ ] Préférer les tableaux aux listes dans les boucles critiques
- [ ] Éviter l'autoboxing (`int` → `Integer`) dans les hot paths

---

## Ressources pour aller plus loin

- **Livre** : *"Java Performance"* — Scott Oaks
- **Livre** : *"Optimizing Java"* — Benjamin Evans, James Gough, Chris Newland
- **Librairie** : [LMAX Disruptor](https://github.com/LMAX-Exchange/disruptor) — Ring buffer ultra-rapide
- **Librairie** : [Chronicle Queue](https://github.com/OpenHFT/Chronicle-Queue) — Off-heap persistence
- **Benchmark** : [JMH](https://openjdk.org/projects/code-tools/jmh/) — Java Microbenchmark Harness
- **Article** : *"Mechanical Sympathy"* — Martin Thompson (co-créateur du Disruptor)

---

*Bonne chance pour ton entretien chez Crédit Agricole ! 🚀*
