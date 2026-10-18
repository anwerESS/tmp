# Guide Métier Complet: Back-Office Finance - Settlement (Règlement-Livraison)
## Pour Développeur Backend en Entretien - Version Business/Métier

---

## 1. Vue d'Ensemble du Back-Office Finance

### Qu'est-ce que le Back-Office ?

Le **back-office** est la partie **post-marché** (post-trade) d'une institution financière. Il intervient **après** l'exécution d'une transaction pour la finaliser opérationnellement.

**Les 3 offices d'une banque :**

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────────┐
│  FRONT OFFICE   │ → │  MIDDLE OFFICE   │ → │   BACK OFFICE       │
│                 │    │                  │    │                     │
│ • Trading       │    │ • Contrôle des  │    │ • Confirmation      │
│ • Exécution     │    │   risques       │    │ • Règlement-        │
│ • Vente         │    │ • Validation    │    │   livraison         │
│ • Relation      │    │ • Compliance    │    │ • Réconciliation    │
│   clients       │    │ • Reporting     │    │ • Conservation      │
│                 │    │                  │    │ • Reporting         │
└─────────────────┘    └──────────────────┘    └─────────────────────┘
   "On achète"            "On vérifie"            "On livre/paye"
```

### Missions du Back-Office

1. **Confirmation des transactions**
   - Vérifier que acheteur et vendeur sont d'accord sur les termes
   - Envoyer/recevoir les confirmations (SWIFT MT515)

2. **Règlement-livraison (Settlement)**
   - Organiser le transfert des titres (livraison)
   - Organiser le transfert de l'argent (règlement)

3. **Réconciliation**
   - Vérifier que nos positions correspondent à celles du dépositaire
   - Vérifier que le cash correspond

4. **Gestion des incidents (exceptions)**
   - Traiter les échecs de règlement (fails)
   - Résoudre les désaccordances (mismatches)
   - Gérer les suspens

5. **Reporting réglementaire**
   - Communiquer aux régulateurs (AMF, ESMA, etc.)
   - Produire les états comptables

---

## 2. Le Settlement (Règlement-Livraison) - Cœur de Métier

### Définition Simple

Le **règlement-livraison** est le processus qui **finalise** une transaction financière en transférant :
- Les **titres** de l'acheteur au vendeur (ou l'inverse)
- L'**argent** en contrepartie

**Exemple concret :**
```
Transaction : J'achète 1000 actions Société Générale à 25€

Avant le settlement :
├─ J'ai 25,000€ en cash
└─ Je n'ai pas les actions

Après le settlement (J+2) :
├─ J'ai 0€ en cash (-25,000€)
└─ J'ai 1000 actions SG (+1000 actions)
```

### Les Deux Jambes du Settlement

**1. Livraison (Delivery)** = Transfert des **titres**
- Les actions/obligations changent de propriétaire
- Mouvement dans les comptes-titres

**2. Règlement (Payment)** = Transfert de l'**argent**
- Le cash est débité/crédité
- Mouvement dans les comptes espèces

**Important :** Ces deux mouvements doivent être **simultanés et atomiques** (principe DVP).

---

## 3. Le Principe DVP (Delivery Versus Payment)

### Définition

**DVP** = Livraison **CONTRE** Paiement de manière **simultanée**

C'est le principe fondamental qui garantit que :
- Si je livre les titres, je reçois le cash **en même temps**
- Si je paie le cash, je reçois les titres **en même temps**

### Pourquoi DVP est Crucial ?

**Sans DVP (dangereux) :**
```
Scénario catastrophe :
─────────────────────
Lundi : Je livre 1000 actions (je n'ai plus les titres)
Mardi : La contrepartie devait payer... mais fait faillite !
        → Je perds les titres ET l'argent !

C'est le "Principal Risk" (risque de contrepartie)
```

**Avec DVP (sécurisé) :**
```
Scénario protégé :
──────────────────
J+2 à 10h00:00:000 : Transaction atomique via le dépositaire central
├─ Je livre 1000 actions
└─ Je reçois 25,000€
    ↓
SIMULTANÉ : les deux mouvements ou aucun
```

**Le DVP est :**
- **Obligatoire** (imposé par les régulateurs)
- **Garanti** par les dépositaires centraux (Euroclear, Clearstream)
- La **norme** sur tous les marchés modernes

---

## 4. Cycle de Vie Complet d'une Transaction

### Timeline : De J (Trade) à J+2 (Settlement)

**Standard européen : T+2 (Trade Date + 2 jours ouvrés)**

```
┌────────────────────────────────────────────────────────────┐
│  J (Trade)  →  J (Confirmation)  →  J+1  →  J+2 (Settlement) │
└────────────────────────────────────────────────────────────┘
```

---

### **Jour J (Trade Date) - Exécution et Capture**

**Acteur principal : Front Office (Trading)**

**Ce qui se passe :**
1. Le trader exécute un ordre sur le marché
   ```
   Trade exécuté :
   ACHAT 1000 actions Société Générale à 25€
   Marché : Euronext Paris
   Contrepartie : BNP Paribas Securities Services
   ```

2. Le trade est capturé dans le système de trading
   - Via Bloomberg, Reuters, plateforme électronique

3. **Notre rôle (Back-Office) commence :**
   - Réception du trade depuis le front-office
   - Enrichissement des données manquantes :
     - ISIN du titre (FR0000130007 pour Société Générale)
     - Compte de settlement à utiliser
     - Dépositaire concerné (Euroclear France)
     - BIC de la contrepartie

4. **Validations métier :**
   - Le client a-t-il assez de cash ? (25,000€ disponibles ?)
   - Le titre est-il éligible au settlement ?
   - Les comptes sont-ils corrects ?

5. **Création de l'instruction de settlement**
   - L'instruction est créée dans notre base de données
   - Statut : "Nouveau" (New)
   - Settlement Date calculé : J+2 (17/01/2024 si trade le 15/01)

---

### **Jour J (même jour) - Confirmation**

**Notre mission : Confirmer le trade avec la contrepartie**

**Envoi du message SWIFT MT515 (Client Confirmation)**

```
Message MT515 envoyé à BNP Paribas :
────────────────────────────────────
Référence : SGSS20240115001
Type : Achat (BUY)
Titre : Société Générale (ISIN FR0000130007)
Quantité : 1000 actions
Prix : 25€ par action
Montant total : 25,000€
Date de trade : 15/01/2024
Date de settlement : 17/01/2024 (T+2)
Compte SGSS : 12345-SG-EQUITY
Compte BNP : 98765-BNP-EQUITY
```

**But du MT515 :**
- Informer officiellement la contrepartie des termes du trade
- Permettre à BNP de vérifier de son côté
- Base pour le matching (accord) le lendemain

**Réception de la confirmation de BNP :**
- BNP nous envoie aussi un MT515 ou un MT516 (confirmation agreement)
- On vérifie que les deux confirmations correspondent

---

### **Jour J+1 - Matching (Appariement)**

**Mission : Vérifier que notre instruction MATCHE celle de la contrepartie**

**Le Matching, c'est quoi ?**
Le matching est le processus de **comparaison** de notre instruction avec celle de notre contrepartie pour vérifier que nous sommes d'accord sur tous les détails.

**Éléments comparés :**
```
Notre instruction MT540 :        Instruction BNP MT541 :
────────────────────────         ────────────────────────
ISIN: FR0000130007               ISIN: FR0000130007      ✓
Quantité: 1000                   Quantité: 1000          ✓
Prix: 25€                        Prix: 25€               ✓
Date settlement: 17/01           Date settlement: 17/01  ✓
Compte nous: 12345               Compte nous: 12345      ✓
Compte BNP: 98765                Compte BNP: 98765       ✓
                                 ─────────────────────────
                                        → MATCHED ✓
```

**Si tout correspond : MATCHED**
- Statut de l'instruction passe à "Matched"
- Prêt pour le settlement demain (J+2)
- Message MT548 reçu du dépositaire : "Status: Matched"

**Si désaccordance (mismatch) :**
```
Notre instruction :              Contrepartie BNP :
Quantité: 1000 actions           Quantité: 900 actions   ✗
                                 ─────────────────────────
                                    → MISMATCH ✗
                                    → PENDING MATCHING
```

**Actions si mismatch :**
1. **Investigation** : Qui a raison ? Nous ou BNP ?
2. **Contact** : Appel téléphonique ou email avec BNP
3. **Résolution** :
   - Soit on corrige notre instruction (SWIFT MT551 - Amendment)
   - Soit BNP corrige la leur
4. **Re-matching** : Nouvelle tentative d'appariement

**Types de mismatch courants :**
- **Quantité différente** (le plus fréquent)
- **Prix différent**
- **Compte erroné**
- **Date de settlement différente**
- **Référence trade manquante**

---

### **Jour J+2 (Settlement Date) - Règlement-Livraison Physique**

**C'est LE jour où les mouvements effectifs ont lieu !**

#### **Matin (avant 10h) : Envoi des instructions au dépositaire**

**Notre rôle :**
Envoyer les instructions de settlement au **dépositaire central** (Euroclear ou Clearstream)

**Pour un ACHAT (comme notre exemple) :**

Nous envoyons un **MT540 ou MT541 (Receive Against Payment)** à Euroclear France :

```
Message à Euroclear :
─────────────────────
Type : RECEIVE AGAINST PAYMENT (DVP)
Je vais RECEVOIR :
├─ 1000 actions Société Générale (ISIN FR0000130007)
├─ DE la contrepartie BNP Paribas (compte 98765-BNP-EQUITY)
├─ VERS mon compte SGSS (12345-SG-EQUITY)
└─ CONTRE paiement de 25,000€
```

**Pour une VENTE :**

Nous enverrions un **MT542 ou MT543 (Deliver Against Payment)** :

```
Type : DELIVER AGAINST PAYMENT (DVP)
Je vais LIVRER :
├─ 1000 actions (depuis mon compte)
├─ VERS le compte de l'acheteur
└─ CONTRE réception de 25,000€
```

#### **Journée : Le dépositaire traite**

**Que fait Euroclear ?**

1. **Réception des instructions**
   - Euroclear reçoit notre MT540 (on veut recevoir)
   - Euroclear reçoit le MT543 de BNP (BNP veut livrer)

2. **Validation des instructions**
   - Les deux instructions matchent-elles ?
   - Les comptes existent-ils ?
   - Les montants correspondent-ils ?

3. **Vérification des disponibilités**
   - **Côté BNP (vendeur)** : A-t-il les 1000 actions en compte ?
   - **Côté SGSS (acheteur)** : Avons-nous 25,000€ disponibles ?

4. **Si tout est OK : Settlement effectué**
   ```
   Mouvements simultanés (DVP) à 14h30 par exemple :
   
   Mouvement TITRES :
   ├─ Compte BNP (98765) : -1000 actions SG
   └─ Compte SGSS (12345) : +1000 actions SG
   
   Mouvement CASH :
   ├─ Compte cash SGSS : -25,000€
   └─ Compte cash BNP : +25,000€
   
   → Transaction SETTLED ✓
   ```

#### **Après-midi : Réception des confirmations**

**Euroclear nous envoie des messages de confirmation :**

**1. MT544 (Receive Confirmation)**
```
Message MT544 reçu d'Euroclear :
─────────────────────────────────
Confirmation : Vous avez REÇU
├─ 1000 actions Société Générale
├─ Dans votre compte 12345-SG-EQUITY
├─ De BNP Paribas (compte 98765)
└─ Le 17/01/2024 à 14:30
```

**2. MT548 (Settlement Status and Processing Advice)**
```
Message MT548 reçu d'Euroclear :
─────────────────────────────────
Référence : SGSS20240115001
Statut : SETT (Settled) ✓
Date/Heure : 17/01/2024 14:30:15
Montant réglé : 25,000€
```

**Notre système met à jour :**
- Statut de l'instruction : "Settled" ✓
- Date effective de settlement : 17/01/2024
- Heure de settlement : 14:30:15

---

### **Fin de journée J+2 : Réconciliation**

**Messages reçus d'Euroclear :**

**MT535 (Statement of Holdings) - Position des titres**
```
Votre position au 17/01/2024 :
───────────────────────────────
ISIN FR0000130007 (Société Générale)
Quantité : 5000 actions
(dont +1000 reçues aujourd'hui)
```

**MT536 (Statement of Transactions) - Mouvements du jour**
```
Mouvements du 17/01/2024 :
──────────────────────────
Réception : +1000 actions SG (trade SGSS20240115001)
Livraison : -500 actions BNP (trade SGSS20240115002)
Position finale : 5000 actions
```

**Notre mission de réconciliation :**
1. Comparer nos positions internes avec le MT535 d'Euroclear
2. Vérifier que tous les mouvements du MT536 correspondent à nos instructions
3. Investiguer si désaccordance (break)

---

## 5. Les Acteurs de l'Écosystème Settlement

### Les Dépositaires Centraux (CSD - Central Securities Depository)

**Rôle principal :**
Les CSD sont les "coffres-forts" où sont **conservés les titres** et où s'effectue le **règlement-livraison**.

**CSD Nationaux (un par pays européen) :**

| **Pays** | **CSD** | **Titres gérés** |
|----------|---------|------------------|
| France | **Euroclear France** | Actions françaises, obligations françaises |
| Allemagne | **Clearstream Frankfurt** | Actions allemandes, obligations allemandes |
| Italie | **Monte Titoli** | Titres italiens |
| Espagne | **Iberclear** | Titres espagnols |
| UK | **Euroclear UK & International** | Titres britanniques |

**ICSD (International CSD) - Titres internationaux :**

| **ICSD** | **Siège** | **Titres gérés** |
|----------|-----------|------------------|
| **Euroclear Bank** | Bruxelles, Belgique | Eurobonds, titres internationaux, multi-marchés |
| **Clearstream Banking Luxembourg** | Luxembourg | Eurobonds, titres internationaux |

**Exemple de flux :**
```
                      ┌─────────────────┐
                      │   EUROCLEAR     │
                      │  (Dépositaire)  │
                      │                 │
                      │ Conservation    │
                      │ des titres      │
                      │                 │
                      │ Règlement DVP   │
                      └────────┬────────┘
                               │
            ┌──────────────────┼──────────────────┐
            │                  │                  │
    ┌───────▼──────┐   ┌──────▼──────┐   ┌──────▼──────┐
    │    SGSS      │   │     BNP     │   │   Société   │
    │  (Acheteur)  │   │  (Vendeur)  │   │   Générale  │
    │              │   │             │   │  (Broker)   │
    └──────────────┘   └─────────────┘   └─────────────┘
    
Tous ont un compte chez Euroclear
Les titres ne "sortent" jamais physiquement d'Euroclear
Seuls les droits de propriété changent (écritures comptables)
```

### Les Systèmes de Paiement (Cash Settlement)

**Pour le règlement des espèces (cash), on utilise des systèmes de paiement interbancaires :**

**TARGET2 (Trans-European Automated Real-time Gross settlement Express Transfer)**
- Système de paiement de la **Banque Centrale Européenne**
- Pour les paiements en **EUR** uniquement
- **Temps réel**, montants **importants**
- Utilisé pour les settlements de titres

**Autres systèmes :**
- **EURO1** (EBA Clearing) - Europe, EUR, retail
- **CHAPS** - UK, GBP
- **Fedwire** - USA, USD
- **SIC** - Suisse, CHF

**Flux typique de cash settlement :**
```
1. Notre compte SGSS chez la Banque de France (TARGET2)
   │
   ├─ Débit : -25,000€
   │
2. Compte central d'Euroclear chez la Banque de France
   │
   ├─ Transfert interne chez Euroclear
   │
3. Compte de BNP chez Euroclear
   │
   ├─ Crédit : +25,000€
   │
4. Compte BNP chez la Banque de France (TARGET2)
```

---

## 6. Les Messages SWIFT MT (Communication)

### SWIFT : Le Réseau de Communication

**SWIFT** = Society for Worldwide Interbank Financial Telecommunication

- Réseau **sécurisé** de messagerie financière
- Utilisé par 11,000+ institutions dans 200+ pays
- Format standardisé : **MT** (Message Type)

**Pourquoi SWIFT ?**
- **Sécurité** : Cryptage, authentification
- **Standardisation** : Tous parlent le même langage
- **Traçabilité** : Tous les messages sont enregistrés
- **Fiabilité** : Disponibilité 99.9%+

### Messages MT pour le Settlement

**Les principaux messages dans notre domaine :**

| **Message** | **Nom** | **Envoyé par** | **Reçu par** | **Contenu** |
|-------------|---------|----------------|--------------|-------------|
| **MT515** | Client Confirmation | SGSS | Client/Contrepartie | Confirmation du trade |
| **MT540** | Receive Free | SGSS | Euroclear | "Je vais recevoir des titres (sans paiement)" |
| **MT541** | Receive Against Payment | SGSS | Euroclear | "Je vais recevoir des titres DVP" |
| **MT542** | Deliver Free | SGSS | Euroclear | "Je vais livrer des titres (sans paiement)" |
| **MT543** | Deliver Against Payment | SGSS | Euroclear | "Je vais livrer des titres DVP" |
| **MT544** | Receive Confirmation | Euroclear | SGSS | "Vous avez reçu les titres" |
| **MT546** | Deliver Confirmation | Euroclear | SGSS | "Vous avez livré les titres" |
| **MT548** | Settlement Status | Euroclear | SGSS | "Statut de votre instruction" |
| **MT535** | Statement of Holdings | Euroclear | SGSS | "Votre position titres" |
| **MT536** | Statement of Transactions | Euroclear | SGSS | "Vos mouvements du jour" |
| **MT537** | Statement of Pending | Euroclear | SGSS | "Instructions en attente" |

### Exemple Simplifié : MT540 (Receive Against Payment)

**Scénario :** SGSS achète 1000 actions SG, envoie instruction à Euroclear

**Contenu du message MT540 :**
```
──────────────────────────────────────────────
MESSAGE MT540 - RECEIVE AGAINST PAYMENT (DVP)
──────────────────────────────────────────────

EXPÉDITEUR : SGSS (BIC: SGSSFRPPXXX)
DESTINATAIRE : Euroclear France (BIC: ECFRFRPPXXX)

RÉFÉRENCE : SGSS20240117001
TYPE : NOUVEAU (pas d'annulation/modification)
DATE DE SETTLEMENT : 17/01/2024

DÉTAILS DU TITRE :
- ISIN : FR0000130007
- Nom : SOCIETE GENERALE
- Quantité à recevoir : 1000 actions

COMPTES :
- Notre compte chez Euroclear : 12345-SG-EQUITY
- Compte de la contrepartie : 98765-BNP-EQUITY
- Contrepartie : BNP Paribas (BIC: BNPAFRPPXXX)

DÉTAILS FINANCIERS :
- Prix unitaire : 25,00 EUR
- Montant total : 25,000.00 EUR
- Devise : EUR

TYPE DE RÈGLEMENT : DVP (Delivery Versus Payment)
──────────────────────────────────────────────
```

**Réponse d'Euroclear (MT548 - Settlement Status) :**
```
──────────────────────────────────────────
MESSAGE MT548 - SETTLEMENT STATUS
──────────────────────────────────────────

EXPÉDITEUR : Euroclear France
DESTINATAIRE : SGSS

RÉFÉRENCE : SGSS20240117001

STATUT : SETT (Settled - Réglé) ✓
DATE/HEURE : 17/01/2024 à 14:30:15

DÉTAILS :
- 1000 actions livrées
- 25,000€ payés
- Settlement effectué avec succès
──────────────────────────────────────────
```

---

## 7. Incidents et Gestion des Exceptions (Settlement Fails)

### Qu'est-ce qu'un Settlement Fail ?

Un **fail** est un échec de règlement-livraison : la transaction ne peut pas être finalisée à J+2 comme prévu.

**Impact d'un fail :**
- Transaction **non réglée** à la date prévue
- Obligation de la **reporter** au lendemain (J+3, J+4, etc.)
- Pénalités potentielles (CSDR - Cash Settlement Discipline Regime)
- Risque opérationnel et de réputation

### Principales Causes de Fails

#### **1. LACK (Lack of Securities) - Manque de Titres**

**Situation :**
Le vendeur doit livrer 1000 actions mais n'en a que 800 en portefeuille.

```
Vendeur BNP doit livrer : 1000 actions SG
Position réelle chez Euroclear : 800 actions
                                 ──────────
Shortage (manque) : 200 actions → FAIL
```

**Pourquoi ça arrive ?**
- Vendeur a vendu à découvert (short selling) sans emprunter les titres
- Erreur de gestion de position
- Autre trade a "consommé" les titres avant

**Résolution :**
- **Securities borrowing** : Emprunter les 200 actions manquantes
- **Buy-in** : Racheter les actions sur le marché
- **Partial settlement** : Régler 800 actions, reporter 200

#### **2. CAIS (Cash Insufficient) - Manque de Cash**

**Situation :**
L'acheteur doit payer 25,000€ mais n'a que 20,000€ disponibles.

```
Acheteur SGSS doit payer : 25,000€
Cash disponible : 20,000€
                  ────────
Manque : 5,000€ → FAIL
```

**Pourquoi ça arrive ?**
- Problème de trésorerie (cash management)
- Autre règlement a consommé le cash disponible
- Erreur de prévision de cash

**Résolution :**
- **Funding** : Appel de fonds auprès du client ou trésorerie centrale
- **Overdraft** : Utiliser une facilité de crédit (ligne de trésorerie)
- **Partial settlement** : Payer 20,000€, reporter 5,000€

#### **3. DMON (Mismatch) - Désaccordance**

**Situation :**
Les instructions des deux parties ne correspondent pas (mismatch persistant).

```
Notre instruction :           Contrepartie :
Quantité : 1000               Quantité : 900    ✗
Prix : 25€                    Prix : 24.50€     ✗
                              ─────────────────
                              MISMATCH → FAIL
```

**Pourquoi ça arrive ?**
- Erreur de saisie (fat finger error)
- Trade partiellement exécuté et mal communiqué
- Amendement non communiqué à temps

**Résolution :**
- **Investigation** : Appel avec la contrepartie, vérifier le trade blotter
- **Amendment** : Corriger l'instruction (envoi MT551)
- **Cancellation** : Annuler et recréer si nécessaire (MT551 CANC)

#### **4. NARR (Narrative) - Autres Raisons**

**Exemples :**
- **Compte fermé** : Le compte de settlement n'existe plus
- **Titre suspendu** : Le titre n'est plus éligible au settlement (OPA, restructuration)
- **Holiday mismatch** : Jour férié différent entre marchés
- **Cut-off time exceeded** : Instruction reçue trop tard par le dépositaire (après 18h par exemple)
- **Technical issue** : Problème technique chez le dépositaire

### Workflow de Résolution des Fails

**Jour J+2 (15h) : Réception MT548 status FAIL**

```
1. ALERTE automatique générée dans notre système
   ├─ Email envoyé aux opérateurs back-office
   └─ Dashboard affiche l'instruction en rouge

2. ANALYSE de la cause du fail
   ├─ Lire le reason code dans le MT548
   ├─ Vérifier nos positions (titres et cash)
   └─ Contacter la contrepartie si nécessaire

3. ACTION selon la cause
   ├─ LACK → Emprunter titres ou buy-in
   ├─ CAIS → Appeler trésorerie, demander funding
   ├─ DMON → Corriger l'instruction (MT551 amendment)
   └─ NARR → Investiguer et résoudre au cas par cas

4. RETRY automatique
   ├─ Instruction re-tentée automatiquement J+3
   └─ Si encore FAIL, retry J+4, J+5, etc.

5. ESCALADE si fail persistant (> 5 jours)
   ├─ Remonter au management
   ├─ Impliquer les traders
   └─ Envisager règlement manuel ou cancellation
```

### Pénalités CSDR (Depuis 2022)

**CSDR** = Central Securities Depositories Regulation

**Principe :**
Depuis février 2022, les fails sont **pénalisés** pour inciter au settlement à temps.

**Calcul de la pénalité :**
```
Pénalité journalière = Montant du trade × Taux de pénalité × Nombre de jours de fail

Taux de pénalité :
- Actions et obligations : 1 basis point par jour (0.01%)
- Autres titres : varie selon liquidité
```

**Exemple :**
```
Trade : 1,000,000€
Fail pendant 3 jours
Taux : 0.01% par jour

Pénalité = 1,000,000€ × 0.01% × 3 jours = 300€
```

**Qui paye la pénalité ?**
La partie **responsable** du fail (celle qui manque de titres ou de cash).

---

## 8. Réconciliation (Reconciliation)

### Qu'est-ce que la Réconciliation ?

La réconciliation est le processus de **vérification** que nos positions internes (dans notre système) correspondent exactement aux positions **officielles** chez le dépositaire (Euroclear).

**Principe :**
```
Position SGSS (système interne)  =?=  Position Euroclear (officielle)
```

**Si désaccordance → BREAK → Investigation**

### Messages de Réconciliation d'Euroclear

**MT535 - Statement of Holdings (Position)**
```
Reçu tous les jours (généralement le matin vers 7h)

Contenu :
─────────
Date : 17/01/2024
Compte : 12345-SG-EQUITY

ISIN FR0000130007 (Société Générale)
Quantité : 5,000 actions

ISIN FR0000131104 (BNP Paribas)
Quantité : 2,500 actions

...
```

**MT536 - Statement of Transactions (Mouvements)**

```
Reçu tous les jours (généralement vers 18h après clôture)

Contenu :
─────────
Date : 17/01/2024
Compte : 12345-SG-EQUITY

Mouvements du jour :

1. RÉCEPTION (Settlement)
   ISIN FR0000130007
   Quantité : +1,000 actions
   Référence : SGSS20240115001
   Contrepartie : BNP (compte 98765)

2. LIVRAISON (Settlement)
   ISIN FR0000131104
   Quantité : -500 actions
   Référence : SGSS20240115002
   Contrepartie : Société Générale

3. CORPORATE ACTION (Dividende)
   ISIN FR0000130007
   Cash : +2,500€ (dividende sur 5000 actions)
```

**MT537 - Statement of Pending Transactions (Instructions en attente)**
```
Reçu tous les jours

Contenu :
─────────
Instructions NON réglées :

1. Référence : SGSS20240115003
   ISIN : FR0010220475
   Quantité : 1,000
   Statut : PEND (Pending)
   Raison : Waiting for counterparty instruction

2. Référence : SGSS20240115004
   ISIN : FR0000120073
   Quantité : 2,000
   Statut : FAIL
   Raison : LACK (Shortage of securities)
```

### Processus de Réconciliation

**Étapes quotidiennes :**

**1. Réception des statements (7h00)**
```
MT535 reçu d'Euroclear
↓
Chargé automatiquement dans notre système
↓
Parsing des données (ISIN, quantité, etc.)
```

**2. Comparaison automatique (7h30)**
```
Pour chaque titre (ISIN) :

Position SGSS (système interne) :
ISIN FR0000130007 → 5,000 actions

Position Euroclear (MT535) :
ISIN FR0000130007 → 5,000 actions

COMPARAISON :
5,000 = 5,000 → ✓ OK (Match)
```

**3. Identification des breaks (écarts) (8h00)**
```
ISIN FR0000131104

Position SGSS : 2,600 actions
Position Euroclear : 2,500 actions
                     ─────────────
BREAK : +100 actions chez nous
→ INVESTIGATION nécessaire
```

**4. Investigation du break (8h00-10h00)**

**Causes possibles d'un break :**

a) **Mouvement non comptabilisé chez nous**
```
Exemple :
Euroclear : 2,500 (correct)
Nous : 2,600 (erreur)

Investigation :
→ Vérifier le MT536 (mouvements du jour)
→ Découverte : Livraison de 100 actions hier non enregistrée
→ Correction : Passer l'écriture manquante
```

b) **Mouvement en transit**
```
Exemple :
Trade effectué hier tard (17h)
Instruction envoyée ce matin (8h)
Settlement prévu demain

Position Euroclear : 2,500 (correct, settlement pas encore fait)
Position nous : 2,600 (on a déjà comptabilisé le trade)

→ BREAK NORMAL (en transit)
→ Se résoudra demain après settlement
```

c) **Erreur d'Euroclear (rare)**
```
Exemple :
Position réelle : 2,600
Euroclear MT535 : 2,500 (erreur de leur côté)

Investigation :
→ Contacter Euroclear
→ Demander correction du MT535
→ Euroclear envoie un MT535 corrigé
```

**5. Résolution et validation (10h00-12h00)**
```
Break résolu :
- Écriture comptable passée
- Position corrigée
- Break marqué comme "Résolu"

Break non résolu :
- Escalade au management
- Investigation approfondie
- Peut nécessiter plusieurs jours
```

### Types de Réconciliation

**1. Réconciliation Positions (Holdings)**
- Quotidienne (tous les jours)
- Compare les positions titres
- Base : MT535

**2. Réconciliation Mouvements (Transactions)**
- Quotidienne
- Compare les mouvements du jour
- Base : MT536

**3. Réconciliation Cash (Espèces)**
- Quotidienne
- Compare les soldes cash
- Base : MT950 (Bank Statement)

**4. Réconciliation Pending (Instructions en attente)**
- Quotidienne
- Vérifie que toutes nos instructions sont trackées
- Base : MT537

---

## 9. Corporate Actions (Actions sur Titres)

### Qu'est-ce qu'une Corporate Action ?

Une **corporate action** est un événement décidé par l'émetteur (la société) qui **impacte les titres** et leurs détenteurs.

### Types de Corporate Actions

#### **1. Dividendes (Dividend)**

**Définition :** Distribution d'une partie des bénéfices aux actionnaires

**Exemple :**
```
Société Générale annonce :
Dividende de 2.50€ par action
Record Date : 15/03/2024 (qui a droit ?)
Payment Date : 20/03/2024 (quand on paye)

Impact pour SGSS (détenant 5,000 actions SG) :
Crédit cash : 5,000 × 2.50€ = 12,500€
```

**Workflow back-office :**
1. **Annonce reçue** (MT564 - Corporate Action Notification)
2. **Vérification position** au Record Date (on a bien 5,000 actions ?)
3. **Calcul du dividende** (5,000 × 2.50€)
4. **Réception du cash** le Payment Date (MT566 - Corporate Action Confirmation)
5. **Crédit au client** (si on gère pour des clients)

#### **2. Stock Split (Division d'Actions)**

**Définition :** Division des actions existantes en plusieurs actions

**Exemple :**
```
Apple annonce un split 2-pour-1
Avant split : 1,000 actions à 180$
Après split : 2,000 actions à 90$

Valeur totale inchangée : 180,000$
```

**Impact back-office :**
```
Position avant : 1,000 actions Apple (ISIN US0378331005)
↓
Corporate Action : Split 2:1
↓
Position après : 2,000 actions Apple (même ISIN)
```

#### **3. Reverse Split (Regroupement d'Actions)**

**Définition :** Regroupement de plusieurs actions en une seule

**Exemple :**
```
Société en difficulté annonce reverse split 1-pour-10
Avant : 10,000 actions à 0.50€
Après : 1,000 actions à 5€

But : Augmenter le prix unitaire (psychologique)
```

#### **4. Fusion/Acquisition (Merger)**

**Exemple :**
```
Société A acquiert Société B
Échange : 1 action B → 1.5 actions A

Position avant :
1,000 actions Société B (ISIN FR0000120073)

Position après :
0 actions Société B (ISIN radié)
1,500 actions Société A (ISIN FR0000120271)
```

**Complexité back-office :**
- Changement d'ISIN
- Calcul des fractions (1,000 × 1.5 = 1,500, mais si 1,001 actions ?)
- Gestion des rompus (fractions d'actions vendues pour cash)

#### **5. OPA (Offre Publique d'Achat)**

**Définition :** Une société propose de racheter les actions d'une autre

**Exemple :**
```
Société Générale lance OPA sur Crédit du Nord
Prix offert : 25€ par action (prime de 20% sur cours)

Nos clients détiennent : 10,000 actions Crédit du Nord

Options :
1. APPORTER (Tender) : Vendre à 25€
2. CONSERVER : Garder les actions
```

**Workflow back-office :**
1. **Notification OPA** (MT564)
2. **Contacter les clients** : Que voulez-vous faire ?
3. **Collecter les instructions** des clients
4. **Envoyer instructions au dépositaire** (MT565 - Corporate Action Instruction)
5. **Recevoir confirmation** (MT566)
6. **Settlement** des titres vendus + réception cash

#### **6. Stock Dividend (Dividende en Actions)**

**Définition :** Au lieu de cash, l'entreprise distribue des actions

**Exemple :**
```
Total annonce dividende en actions
1 action gratuite pour 10 détenues

Position avant : 5,000 actions Total
Dividende : 5,000 / 10 = 500 actions gratuites
Position après : 5,500 actions Total
```

---

## 10. Notions Importantes à Connaître

### Safe Keeping Account (Compte de Conservation)

**Définition :** Compte ouvert chez un dépositaire (Euroclear) où sont conservés les titres

**Structure typique :**
```
SGSS a plusieurs comptes chez Euroclear :

Compte 12345-EQUITY-FR
├─ Actions françaises pour clients
└─ Euroclear France

Compte 12346-EQUITY-INTL
├─ Actions internationales
└─ Euroclear Bank

Compte 12347-BONDS-EUR
├─ Obligations en EUR
└─ Euroclear France

Compte 12348-OWN
├─ Position propre SGSS
└─ Euroclear France
```

### Segregation (Ségrégation des Comptes)

**Principe :** Les titres des clients doivent être **séparés** des titres propres de la banque

**Pourquoi ?**
- **Protection** : En cas de faillite de SGSS, les titres clients sont protégés
- **Réglementaire** : Obligation légale (MiFID II)

**Types de ségrégation :**

**1. Omnibus Account (Compte Omnibus)**
```
Compte 12345-OMNIBUS-CLIENTS
├─ Client A : 1,000 actions SG
├─ Client B : 2,000 actions SG
├─ Client C : 500 actions SG
└─ Total chez Euroclear : 3,500 actions SG

Avantage : Simple, moins de comptes
Inconvénient : Tous les clients dans le même compte
```

**2. Individual Segregation (Ségrégation Individuelle)**
```
Compte 12345-CLIENT-A : 1,000 actions SG
Compte 12346-CLIENT-B : 2,000 actions SG
Compte 12347-CLIENT-C : 500 actions SG

Avantage : Protection maximale
Inconvénient : Coûteux (frais par compte)
```

### Netting (Compensation)

**Définition :** Compensation de plusieurs transactions pour réduire les mouvements

**Exemple :**
```
Même jour, même titre, même contrepartie :

Trade 1 : ACHAT 1,000 actions SG de BNP
Trade 2 : VENTE 300 actions SG à BNP
Trade 3 : ACHAT 500 actions SG de BNP
                                    ──────
NETTING : ACHAT NET de 1,200 actions

Au lieu de 3 settlements → 1 seul settlement de 1,200
```

**Avantages :**
- Moins de mouvements (coûts réduits)
- Moins de risque opérationnel
- Optimisation du cash

### Affirmation (Affirmation)

**Définition :** Processus où le broker **confirme** le trade au nom du client final

**Exemple :**
```
Client institutionnel → Trade via Broker Société Générale → Marché

Flux de confirmation :
1. Client passe ordre à SG Broker
2. SG Broker exécute sur le marché
3. SG Broker AFFIRME le trade (confirme pour le client)
4. SGSS (custodian du client) reçoit l'affirmation
5. SGSS fait le settlement
```

**Message SWIFT :** MT518 (Market-Side Settlement Instruction)

### Allocation

**Définition :** Répartition d'un trade global en plusieurs sous-trades pour différents clients

**Exemple :**
```
SG Broker achète 10,000 actions Apple en bloc

Allocation :
├─ Client A (SGSS) : 3,000 actions
├─ Client B (BNP) : 4,000 actions
└─ Client C (Caceis) : 3,000 actions

SGSS recevra une instruction pour 3,000 actions seulement
```

### Cut-off Time (Heure Limite)

**Définition :** Heure limite pour envoyer les instructions au dépositaire

**Exemple chez Euroclear France :**
```
18h00 : Cut-off pour instructions du jour même
Après 18h00 : Instruction traitée le lendemain

Si instruction envoyée à 18h30 pour settlement J+2 :
→ Sera traitée J+3 (trop tard pour J+2)
→ FAIL garanti si contrepartie a respecté cut-off
```

**Cut-offs typiques :**
- **Euroclear France** : 18h00 CET
- **Euroclear Bank** : 20h00 CET
- **Clearstream Luxembourg** : 19h30 CET

### Partial Settlement (Règlement Partiel)

**Définition :** Régler une partie de la transaction quand le montant total n'est pas disponible

**Exemple :**
```
Trade : VENTE 1,000 actions SG
Problème : Acheteur n'a cash que pour 700 actions

Partial Settlement :
Jour 1 : Settlement de 700 actions (70%)
Jour 2 : Settlement de 300 actions (30% restant)
```

### Buy-in (Rachat Forcé)

**Définition :** Mécanisme où l'acheteur rachète les titres sur le marché si le vendeur ne livre pas

**Exemple :**
```
Jour J+2 : Vendeur fail (manque de titres)
Jour J+3 : Vendeur fail encore
Jour J+4 : Acheteur déclenche BUY-IN
           → Rachète 1,000 actions sur le marché à 26€
           → Coût supplémentaire : (26€ - 25€) × 1,000 = 1,000€
           → Vendeur doit payer les 1,000€ de perte
```

**CSDR Buy-in (depuis 2022) :**
- **Automatique** après 7 jours de fail (actions liquides)
- **Pénalités** pour le vendeur défaillant

---

## 11. Risques et Contrôles

### Risques du Settlement

**1. Risque de Contrepartie (Principal Risk)**
```
Risque : Je livre les titres, la contrepartie ne paye pas (ou inverse)
Mitigation : DVP (Delivery Versus Payment)
Impact sans DVP : Perte totale du montant de la transaction
```

**2. Risque de Liquidité**
```
Risque : Manque de cash pour régler nos achats
Mitigation : Cash forecasting, lignes de crédit
Impact : Settlement fails, pénalités CSDR
```

**3. Risque Opérationnel**
```
Risque : Erreurs humaines (mauvais ISIN, mauvais compte, etc.)
Mitigation : Contrôles automatiques, double validation
Impact : Fails, claims, perte de réputation
```

**4. Risque de Règlement (Settlement Risk)**
```
Risque : Le dépositaire ou système de paiement tombe en panne
Mitigation : Procédures de contingence, back-up systems
Impact : Impossibilité de settler, accumulation de fails
```

**5. Risque de Change (FX Risk)**
```
Risque : Transactions en devises étrangères, variation des taux
Exemple : Achat d'actions US en USD, taux EUR/USD change entre trade et settlement
Mitigation : Hedging FX, fixing du taux au trade date
```

### Contrôles Clés

**1. Contrôles Pré-Settlement**
```
Avant d'envoyer l'instruction au dépositaire :

✓ ISIN valide et existant ?
✓ Compte de settlement correct ?
✓ Contrepartie identifiée ?
✓ Montant cohérent (prix × quantité) ?
✓ Settlement date = trade date + 2 jours ouvrés ?
✓ Cash disponible pour les achats ?
✓ Titres disponibles pour les ventes ?
```

**2. Contrôles Post-Settlement**
```
Après réception MT548 status :

✓ Instruction bien settled ?
✓ Si fail, raison identifiée ?
✓ Mouvements bien reflétés dans MT536 ?
✓ Position MT535 correcte après settlement ?
✓ Cash bien débité/crédité ?
```

**3. Réconciliation Quotidienne**
```
✓ Positions internes = Positions dépositaire ?
✓ Tous les mouvements du jour comptabilisés ?
✓ Pas de breaks non expliqués ?
✓ Instructions pending cohérentes ?
```

---

## 12. Évolutions et Tendances

### T+1 (Settlement en J+1)

**Évolution majeure :**
- **USA** : Passage de T+2 à **T+1** depuis **28 mai 2024**
- **Europe** : Discussions en cours, probable vers **2027-2028**
- **UK** : Discussions pour suivre l'Europe

**Impact pour le back-office :**
```
Avec T+2 actuel :
J : Trade à 16h
J+1 : Toute la journée pour confirmer, matcher
J+2 : Settlement

Avec T+1 futur :
J : Trade à 16h
    → Confirmation IMMÉDIATE
    → Matching le MÊME JOUR
J+1 : Settlement dès le matin

Challenge : Moins de temps pour traiter, vérifier, corriger !
```

### DLT / Blockchain

**Expérimentations en cours :**
- **Tokenisation** des titres (actions/obligations sur blockchain)
- **Settlement instantané** (quasi temps réel)
- **Smart contracts** pour automatiser les corporate actions

**Avantages potentiels :**
- Settlement quasi-instantané (T+0)
- Réduction des intermédiaires
- Moins de fails (DVP automatique via smart contracts)

**Mais encore expérimental**, pas de déploiement massif avant plusieurs années.

### CSDR (Central Securities Depositories Regulation)

**Depuis février 2022 :**
- **Pénalités automatiques** sur les fails
- **Discipline de settlement** renforcée
- **Mandatory buy-ins** après 7 jours de fail

**Impact :**
- Pression accrue pour réduire les fails
- Investissements dans les systèmes pour monitorer
- Processus plus rigoureux

---

## 13. Questions Fréquentes en Entretien

### Questions Métier Typiques

**Q1: "Expliquez-moi ce qu'est le settlement en une phrase simple"**
```
Réponse :
"Le settlement, c'est la finalisation d'une transaction financière 
où les titres et l'argent changent effectivement de mains de manière 
simultanée et sécurisée, généralement 2 jours après le trade (T+2)."
```

**Q2: "Pourquoi T+2 et pas T+0 (instantané) ?"**
```
Réponse :
"T+2 laisse le temps nécessaire pour :
- Confirmer le trade entre acheteur et vendeur (matching)
- Vérifier et corriger les erreurs éventuelles
- Gérer les aspects administratifs (allocation, affirmation)
- S'assurer que cash et titres sont disponibles
Le T+0 nécessiterait une automatisation totale et des systèmes 
ultra-fiables, on y va progressivement (T+1 aux USA depuis 2024)."
```

**Q3: "Qu'est-ce qui peut faire échouer un settlement ?"**
```
Réponse :
"Les 4 causes principales :
1. LACK - Le vendeur n'a pas les titres à livrer
2. CAIS - L'acheteur n'a pas le cash pour payer
3. DMON - Les instructions ne matchent pas (désaccordance)
4. Problèmes techniques ou comptes invalides

Notre rôle est de détecter et résoudre ces problèmes rapidement 
pour éviter les pénalités et les impacts clients."
```

**Q4: "Quel est le rôle d'Euroclear ?"**
```
Réponse :
"Euroclear est le dépositaire central qui :
1. Conserve les titres dans des coffres-forts électroniques
2. Enregistre qui possède quoi (tenue de registre)
3. Exécute les règlements-livraisons en mode DVP sécurisé
4. Garantit que livraison et paiement sont simultanés
5. Envoie les statements quotidiens (positions, mouvements)

C'est l'infrastructure centrale qui sécurise tout le système."
```

**Q5: "Qu'est-ce que le DVP et pourquoi c'est important ?"**
```
Réponse :
"DVP (Delivery Versus Payment) = principe où la livraison des titres 
et le paiement du cash se font EN MÊME TEMPS, de manière atomique.

Importance : Sans DVP, risque énorme :
- Je livre mes titres lundi
- La contrepartie devait payer mardi mais fait faillite
- Je perds tout !

Avec DVP : Transaction atomique, soit les deux mouvements ensemble, 
soit aucun. Protection totale contre le risque de contrepartie."
```

**Q6: "Qu'est-ce qu'un settlement fail et comment le gérez-vous ?"**
```
Réponse :
"Un fail = échec du règlement à la date prévue.

Notre processus :
1. DÉTECTION : Réception MT548 avec status FAIL + reason code
2. ANALYSE : Comprendre la cause (LACK, CAIS, DMON, etc.)
3. ACTION IMMÉDIATE selon la cause :
   - Manque titres → Emprunter ou buy-in
   - Manque cash → Demander funding
   - Mismatch → Corriger instruction (MT551)
4. RETRY automatique chaque jour
5. ESCALADE si fail > 5 jours
6. REPORTING des pénalités CSDR à payer/recevoir

L'objectif : Résoudre le plus vite possible pour limiter les coûts 
et l'impact client."
```

**Q7: "Quelle est la différence entre back-office et middle-office ?"**
```
Réponse :
"Middle Office (AVANT settlement) :
- Contrôle des risques sur les trades
- Validation des prix, contrôle P&L
- Compliance, limites de trading

Back Office (APRÈS trade) :
- Confirmation du trade
- Settlement (règlement-livraison physique)
- Réconciliation, gestion des fails
- Conservation des titres

En résumé : Middle = Contrôle, Back = Opérations"
```

**Q8: "Quels messages SWIFT utilisez-vous au quotidien ?"**
```
Réponse :
"Messages quotidiens principaux :

ENVOI (nous → dépositaire) :
- MT540/541 : Instructions de réception (achats)
- MT542/543 : Instructions de livraison (ventes)
- MT515 : Confirmation aux clients/contreparties

RÉCEPTION (dépositaire → nous) :
- MT548 : Statuts de settlement (SETT, FAIL, PEND)
- MT535 : Positions quotidiennes (holdings)
- MT536 : Mouvements du jour (transactions)
- MT537 : Instructions pending

Ce sont nos outils de communication avec l'écosystème."
```

---

## 14. Conseils pour l'Entretien

### Ce que le Recruteur Veut Entendre

**✓ Compréhension du métier**
- Vous comprenez le POURQUOI (sécurité, DVP, etc.)
- Vous voyez la big picture (de trade à settlement)
- Vous connaissez les acteurs (CSD, SWIFT, etc.)

**✓ Vision opérationnelle**
- Vous savez ce qu'est un fail et comment le gérer
- Vous comprenez les enjeux (pénalités, risques)
- Vous pouvez expliquer un workflow de bout en bout

**✓ Conscience des évolutions**
- CSDR et pénalités
- T+1 qui arrive
- Automatisation et réduction des fails

### Ce qu'il ne Faut PAS Dire

**✗ "Je ne connais que la technique"**
- Montrer que vous comprenez le métier, pas juste coder

**✗ "Le back-office, c'est juste de l'admin"**
- C'est critique : sans settlement, pas de marché !

**✗ "Je ne sais pas ce qu'est Euroclear"**
- C'est l'acteur central, à connaître absolument

**✗ "DVP, je ne vois pas l'intérêt"**
- C'est LA sécurité du système, fondamental

### Vocabulaire Clé à Utiliser

✓ Settlement, règlement-livraison
✓ DVP (Delivery Versus Payment)
✓ T+2, trade date, settlement date
✓ Matching, affirmation
✓ Fail, LACK, CAIS, DMON
✓ Euroclear, Clearstream, CSD
✓ SWIFT MT540, MT548, MT535
✓ Réconciliation, break
✓ Corporate action, dividende
✓ CSDR, pénalités
✓ Safe keeping account
✓ Cut-off time

---

## Conclusion

En tant que **développeur backend chez SGSS dans le settlement**, vous êtes au cœur d'un système critique qui :

1. **Sécurise** des milliards d'euros de transactions chaque jour
2. **Finalise** les opérations de marché (sans vous, pas de marché !)
3. **Gère** la complexité opérationnelle (fails, réconciliation, corporate actions)
4. **Interagit** avec un écosystème complexe (Euroclear, SWIFT, contreparties)

**Votre valeur ajoutée :**
- Automatiser les processus pour réduire les erreurs
- Traiter les messages SWIFT en temps réel
- Détecter et alerter sur les anomalies
- Améliorer la performance (moins de fails = moins de coûts)

**Le back-office n'est pas "juste de l'admin"**, c'est l'**épine dorsale** du système financier. Sans settlement fiable, pas de confiance, pas de marché !
