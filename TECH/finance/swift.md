# Cours détaillé : Messages SWIFT MT vs ISO 20022

## 1. Introduction : MT vs ISO 20022

### Les messages MT (Message Type)

**Caractéristiques principales :**
- Format propriétaire SWIFT créé dans les années 1970
- Structure compacte et codifiée
- Données limitées (champs fixes avec longueur maximale)
- Format texte structuré avec des tags numériques
- Peu de détails contextuels
- Difficile à automatiser complètement

**Exemple de structure MT :**
```
:16R:GENL
:20C::SEME//2023010112345678
:23G:NEWM
:16S:GENL
```

### Les messages ISO 20022

**Caractéristiques principales :**
- Norme internationale ouverte
- Format XML structuré et hiérarchique
- Richesse des données (plus de détails, contexte complet)
- Auto-descriptif et extensible
- Facilite l'automatisation STP (Straight-Through Processing)
- Interopérable entre systèmes différents

**Exemple de structure ISO 20022 :**
```xml
<Document>
  <SctiesSettlInstr>
    <TxId>
      <SctiesMvmntId>2023010112345678</SctiesMvmntId>
    </TxId>
  </SctiesSettlInstr>
</Document>
```

### Différences clés

| Aspect | MT | ISO 20022 |
|--------|-----|-----------|
| **Format** | Texte structuré | XML |
| **Taille des données** | Limitée (ex: 35 caractères) | Extensible |
| **Lisibilité** | Codifiée, nécessite expertise | Auto-descriptive |
| **Automatisation** | Partielle | Complète (STP) |
| **Informations réglementaires** | Limitées | Complètes (LEI, etc.) |
| **Coût de traitement** | Plus élevé (interventions manuelles) | Réduit |

## 2. La Migration MT vers ISO 20022

### Calendrier SWIFT

**Phases de migration :**
- **Novembre 2022** : Coexistence MT et MX commence
- **Novembre 2025** : Fin de la coexistence, ISO 20022 obligatoire
- Les institutions doivent supporter les deux formats pendant la transition

### Enjeux de la migration

**Défis techniques :**
- Adaptation des systèmes back-office
- Mapping des données MT → MX (perte potentielle d'information)
- Gestion de la coexistence
- Formation des équipes

**Avantages :**
- Meilleure qualité des données
- Réduction des rejets et des exceptions
- Conformité réglementaire facilitée
- Traitement STP amélioré

## 3. Messages de Settlement (Règlement-Livraison)

### Messages MT (catégorie 5xx)

#### **MT540 - Receive Free**
- **Usage** : Instruction de réception de titres sans paiement
- **Direction** : De l'investisseur vers le dépositaire
- **Cas d'usage** : 
  - Transferts internes
  - Réorganisations corporate actions
  - Donations de titres

#### **MT541 - Receive Against Payment**
- **Usage** : Instruction de réception de titres contre paiement (achat)
- **Direction** : De l'acheteur vers son dépositaire
- **Cas d'usage** :
  - Exécution d'ordres d'achat
  - Règlement de transactions boursières
  - **Type le plus courant en back-office**

#### **MT542 - Deliver Free**
- **Usage** : Instruction de livraison de titres sans paiement
- **Direction** : De l'investisseur vers le dépositaire
- **Cas d'usage** :
  - Transferts entre comptes
  - Prêts de titres
  - Garanties (collateral)

#### **MT543 - Deliver Against Payment**
- **Usage** : Instruction de livraison de titres contre paiement (vente)
- **Direction** : Du vendeur vers son dépositaire
- **Cas d'usage** :
  - Exécution d'ordres de vente
  - Dénouement de transactions
  - **Type le plus courant en back-office**

### Messages ISO 20022 (SESE - Securities Settlement)

#### **sese.020.001 - SecuritiesTransactionPendingReport**
- **Usage** : Rapport sur les transactions en attente
- **Direction** : Du dépositaire vers l'investisseur
- **Contenu** : Statut des instructions non encore exécutées

#### **sese.021.001 - SecuritiesTransactionPostingReport**
- **Usage** : Rapport de dénouement effectif
- **Direction** : Du dépositaire vers l'investisseur
- **Contenu** : Confirmation d'exécution des transactions

#### **sese.022.001 - SecuritiesTransactionStatusAdvice**
- **Usage** : Avis de statut de transaction
- **Direction** : Du dépositaire vers l'investisseur
- **Contenu** : Changements de statut (pending, matched, settled, cancelled)

#### **sese.023.001 - SettlementInstructionStatusAdvice**
- **Usage** : Avis de statut d'instruction de règlement
- **Direction** : Entre contreparties ou du dépositaire
- **Contenu** : Statut détaillé avec raisons de rejet éventuelles

#### **sese.024.001 - SettlementAllocationReport**
- **Usage** : Rapport d'allocation de règlement
- **Direction** : Du gestionnaire vers les investisseurs finaux
- **Contenu** : Détail de la répartition des transactions

## 4. Utilisation par Type d'Opération

### **Back-Office / Middle-Office**

**Messages les plus utilisés :**

**Traitement quotidien :**
- **MT541/543** (et équivalents MX) : 80% du volume
  - Traitement des ordres d'achat/vente
  - Confirmation des exécutions
  - Matching avec contreparties

**Reporting et suivi :**
- **sese.022** : Suivi temps réel des statuts
- **sese.023** : Gestion des exceptions et rejets
- Rapprochements de positions

**Processus typique :**
1. Réception ordre de l'Asset Manager
2. Exécution sur le marché
3. Génération MT541 (achat) ou MT543 (vente)
4. Envoi au dépositaire
5. Réception sese.022/023 pour suivi
6. Confirmation finale avec sese.021

### **Règlement-Livraison (Settlement)**

**Flux typique d'une transaction DVP (Delivery Versus Payment) :**

**J (Trade Date) :**
- Exécution de la transaction sur le marché
- Matching entre contreparties

**J+1 :**
- Génération MT541/543
- Envoi aux dépositaires respectifs
- Vérifications réglementaires (CSDR, etc.)

**J+2 (Settlement Date) :**
- Matching des instructions entre dépositaires
- Blocage des titres et cash
- Exécution du règlement-livraison simultané
- Génération sese.021 (confirmation)

**Gestion des échecs :**
- **sese.022** : Statut "pending" avec raison
- Raisons courantes :
  - Manque de titres
  - Manque de cash
  - Erreur de référence
  - Non-matching des instructions

### **Custody (Conservation)**

**Messages utilisés :**
- **MT540/542** : Transferts internes, réorganisations
- **sese.024** : Allocation pour fonds communs
- Gestion des corporate actions
- Reporting positions

### **Prêt-Emprunt de Titres (Securities Lending)**

**Messages spécifiques :**
- **MT542** : Livraison de collatéral
- Instructions free pour les garanties
- Gestion des recalls

## 5. Mapping MT → ISO 20022

### Correspondances principales

**Instructions :**
- MT540 → sese.025.001 (SecuritiesSettlementTransactionInstruction - Receive Free)
- MT541 → sese.025.001 (Receive Against Payment)
- MT542 → sese.025.001 (Deliver Free)
- MT543 → sese.025.001 (Deliver Against Payment)

**Confirmations/Statuts :**
- MT544-547 → sese.024, sese.025, sese.030, sese.031

### Enrichissement des données en ISO 20022

**Informations additionnelles obligatoires :**
- **LEI (Legal Entity Identifier)** : Identification unique des entités
- **Détails fiscaux** : Breakdown par juridiction
- **Codes de transparence** : Pour réglementation MiFID II
- **Informations de chaîne** : Tous les intermédiaires
- **Références uniques** : UTI (Unique Transaction Identifier)

## 6. Bonnes Pratiques

### Pour la migration

1. **Planification** : Cartographier tous les flux MT actuels
2. **Formation** : Former les équipes aux nouveaux formats
3. **Testing** : Phase de tests intensifs en environnement de coexistence
4. **Monitoring** : Surveillance accrue pendant la transition
5. **Documentation** : Maintenir la documentation des mappings

### Pour l'exploitation

1. **Automatisation** : Exploiter la richesse ISO 20022 pour le STP
2. **Gestion des rejets** : Processus clairs pour les exceptions
3. **Réconciliation** : Rapprochements quotidiens
4. **Archivage** : Conservation réglementaire des messages

---

**Points clés à retenir :**
- Les MT541/543 (achat/vente avec paiement) représentent la majorité du volume back-office
- ISO 20022 apporte plus de données mais nécessite adaptation des systèmes
- La migration est obligatoire d'ici novembre 2025
- Le STP est grandement amélioré avec ISO 20022
- La gestion des statuts devient plus granulaire et précise