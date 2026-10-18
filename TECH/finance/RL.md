# Résumé Règlement-Livraison pour Entretien

## 🎯 Définition Clé
Le règlement-livraison représente l'étape finale d'une transaction de titres, permettant par un échange simultané des titres contre espèces de finaliser le transfert de propriété entre acheteur et vendeur.

---

## 📊 Les Données Essentielles

**5 éléments clés d'une transaction :**

1. **Identification du titre** : Code ISIN (standard international)
2. **Quantité** : 
   - En nombre (pour les actions)
   - En nominal/montant (pour les obligations)
3. **Devise et montant** : 
   - DVP (Delivery Versus Payment) = livraison contre paiement
   - FOP (Free Of Payment) = sans contrepartie espèces
4. **Date de négociation** : Date d'engagement des parties
5. **Date de règlement-livraison** : Délai variable (0 à 3 jours selon le marché)

---

## 👥 Les Acteurs (à retenir absolument)

### **Place of Settlement (Dépositaire Central)**
- Exemple FR : **Euroclear France** (ex-SICOVAM)
- Système : **RGV** (RELIT Grande Vitesse)
- Rôle : Tient le compte émetteur, opère le système de règlement

### **Delivering/Receiving Agent**
- = Dépositaires locaux
- Donnent accès au système du dépositaire central
- Affiliés à Euroclear France en France

### **Delivering/Receiving Custodian**
- = Teneurs de compte conservateurs (Global Custodian)
- Gèrent les comptes titres et espèces des investisseurs
- S'appuient sur un réseau de dépositaires locaux

### **Buyer et Seller**
- Les deux parties à la transaction
- Peuvent être : investisseurs finaux, brokers, gérants de fonds

---

## 📨 Les Instructions SWIFT (Messages MT)

### **Types de messages selon l'opération :**

| Message | Usage | Qui envoie |
|---------|-------|------------|
| **MT540** | Receive Free | Acheteur (réception sans paiement) |
| **MT541** | Receive Against Payment | Acheteur (réception avec paiement) |
| **MT542** | Deliver Free | Vendeur (livraison sans paiement) |
| **MT543** | Deliver Against Payment | Vendeur (livraison avec paiement) |

### **Contenu d'une instruction :**
- Référence unique client
- N° de compte
- Caractéristiques de l'opération (ISIN, quantité, montant, dates)
- **Circuit de règlement-livraison de la CONTREPARTIE** (point crucial !)

### **SSI (Standing Settlement Instructions) :**
Coordonnées de règlement-livraison que les contreparties échangent pour automatiser le dénouement.

**Identification :**
- Code BIC (Bank International Code - 11 caractères)
- Codes locaux (ex: n° affilié Euroclear France "SICV")

---

## ⚙️ Le Processus de Dénouement

### **Étape 1 : Transmission**
- Chaque partie envoie son instruction à son custodian via SWIFT
- Le vendeur indique les coordonnées de l'acheteur
- L'acheteur indique les coordonnées du vendeur

### **Étape 2 : Contrôles**

**Contrôles de forme :**
- Validation du format SWIFT
- Vérification de l'existence du titre
- Cohérence quantité/nature du titre
- Existence des comptes et intermédiaires

**Contrôles critiques :**
- **Cut-off** : Respect des délais pour dénouer à la date souhaitée
- **Provisions titres** : Vérification que le vendeur possède les titres
- **Provisions cash** : Vérification que l'acheteur a les espèces

### **Étape 3 : Appariement (Matching)**

**Le dépositaire central apparie 2 instructions si :**
- Une réception + une livraison
- Agent contrepartie A = agent émetteur B (et vice-versa)
- Critères identiques : titre, quantité, montant, dates

### **Étape 4 : Dénouement**
- Transfert simultané des titres et espèces
- Confirmation aux agents → custodians → clients finaux
- Statut : "**Settled**" (dénouée)

**Si provisions insuffisantes :** Statut "**Pending**" (en suspens)

---

## 🔍 Cas Particuliers à Connaître

### **1. Asymétrie des chaînes**
- Gérant de fonds (2 niveaux) vs Broker local (1 niveau)
- Le DECU/RECU peut être omis pour le broker

### **2. Dépositaires centraux différents**
- Une partie chez CSD national, l'autre chez ICSD
- **ICSD** : Clearstream Luxembourg, Euroclear Bank
- Responsabilité des dépositaires locaux de transférer entre CSD

### **3. Livraison unilatérale en franco**
- Possible sur certains systèmes (ex: RGV)
- Réservé à des cas particuliers (source d'erreurs)

---

## 💡 Points Clés

**À retenir absolument :**
1. DVP = principe fondamental (simultanéité titres/cash)
2. MT541/543 = messages les plus courants (achat/vente avec paiement)
3. Euroclear France + RGV = acteurs clés du marché français
4. Matching = étape critique d'appariement au niveau du dépositaire central
5. SSI = coordonnées standardisées pour automatisation
6. Contrôles de provision = éviter le "tirage sur la masse"

**Vocabulaire à maîtriser :**
- CSD (Central Securities Depository) = Dépositaire central
- ICSD = Dépositaire central international
- Global Custodian = Conservateur global
- Sub-custodian / Local custodian = Dépositaire local
- Settlement = Dénouement
- STP (Straight-Through Processing) = Traitement automatisé

**Principe clé :** Chaque partie indique dans son instruction le circuit de sa contrepartie, car le custodian connaît déjà son propre circuit mais a besoin de savoir qui il aura en face.