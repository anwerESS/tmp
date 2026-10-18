Je vais te faire un “mini-cours” structuré, en restant très concret côté back-office / règlement-livraison.

---

## 1. MT (ISO 15022) vs ISO 20022 (MX) – vue d’ensemble

### 1.1. Les messages MT (ISO 15022)

* Standard historique de SWIFT (FIN MT).
* Format **texte “tagged”** : blocs et champs comme `:20C::SEME//`, longueur fixe/variable, beaucoup d’abréviations.
* Organisés par **catégories** (MT1xx paiements client, MT5xx titres, etc.).
  Pour le règlement-livraison, on est surtout en **catégorie 5 – Securities** (MT540-549, 535-538, etc.) ([support.axway.com][1])
* Avantage : standard très répandu, supporté par tous les acteurs.
* Limites : peu extensible, moins riche sémantiquement, parsing plus fragile, difficulté à porter plus de données réglementaires (LEI, UTI/UTR, champs de conformité, etc.).

### 1.2. Les messages ISO 20022 (MX)

* Nouveau standard **XML** : balises explicites, structure hiérarchique, schémas XSD pour valider les messages.
* Organisé par **“business areas”** : `pacs` (payments), `camt` (cash management), `sese` (securities settlement), `semt` (securities statements)… ([ISO20022][2])
* Plus riche : possibilité de porter **beaucoup plus de données structurées** (identifiants, codes, raisons de rejet, lien avec d’autres opérations, etc.).
* Aligné avec les infrastructures modernes : T2/T2S, ECMS, Clearstream, Euroclear, etc., utilisent largement ISO 20022 pour le règlement titres. ([Swift][3])

### 1.3. Migration MT → ISO 20022

* En paiements, SWIFT met fin à la co-existence MT/MX en **novembre 2025** : les paiements “core” devront être en ISO 20022. ([banking.vision][4])
* Pour les **titres**, la logique est similaire :

  * Les messages MT54x, MT53x, etc. ont des équivalents ISO 20022 (surtout `sese.*` et `semt.*`). ([www2.swift.com][5])
  * Beaucoup de CSD/ICSD (T2S, Clearstream, Euroclear…) exigent ou favorisent déjà ISO 20022 pour certaines interfaces. ([clearstream.com][6])

> **Idée clé** : on passe de plusieurs MT spécialisés (540, 541, 542, 543…) à **un message générique ISO 20022 (`sese.023`)** qui décrit toutes les variantes via des champs (Deliver/Receive, Free/Against Payment) + une famille de messages autour (cancellation, status, query…).

---

## 2. MT540, MT541, MT542, MT543 – règlement / livraison titres

Ces messages appartiennent à la catégorie 5 **Securities – Settlement**.

### 2.1. Règle générale

Les quatre messages servent à **instruire une livraison / réception de titres**, soit en Franco (FoP = Free of Payment), soit **contre paiement** (DvP = Delivery versus Payment). ([support.axway.com][1])

| Message   | Sens    | Paiement                  | Usage typique back-office                                                      |
| --------- | ------- | ------------------------- | ------------------------------------------------------------------------------ |
| **MT540** | Receive | **Free of Payment**       | L’acheteur reçoit des titres sans mouvement cash (FoP). ([Oracle Docs][7])     |
| **MT541** | Receive | **Against Payment (DvP)** | L’acheteur reçoit des titres et paye en même temps. ([support.axway.com][1])   |
| **MT542** | Deliver | **Free of Payment**       | Le vendeur livre des titres sans mouvement cash. ([Oracle Docs][8])            |
| **MT543** | Deliver | **Against Payment (DvP)** | Le vendeur livre des titres contre réception de cash. ([support.axway.com][1]) |

#### 2.2. Exemple de flux FoP (Franco)

Cas simple : transfert de titres sans cash (par ex. transfert interne entre dépôts).

* **Acheteur** envoie un **MT540 Receive Free** à son dépositaire.
* **Vendeur** envoie un **MT542 Deliver Free** à son dépositaire. ([invivoo.com][9])
* Le CSD/Custodian matche les deux instructions, effectue le transfert de titres.
* Les statuts sont suivis via MT548 (Settlement Status and Processing Advice), voire MT544-547 (confirmations). ([support.axway.com][1])

#### 2.3. Exemple de flux DvP

Pour un échange standard titres/cash :

* **Acheteur** : MT541 (Receive Against Payment).
* **Vendeur** : MT543 (Deliver Against Payment).
* Le CSD réalise simultanément livraison titres + mouvement cash (risque de règlement réduit).

### 2.4. Côté back-office / règlement-livraison

Dans un système de back-office type SGSS :

* Les MT540-543 sont générés **à partir d’ordres marché / OTC** après enrichissement (place of trade, trade date, settlement date, type de transaction, etc.).
* Ils sont envoyés au **dépositaire / CSD / ICSD** (T2S, Clearstream, Euroclear…).
* Le back-office suit :

  * les **statuts** via MT548 (accepted, matched, unmatched, cancelled, settled, etc.),
  * puis les **confirmations** via MT544-547 et les **relevés** via MT535-538 (holdings, transactions, pending, intra-positions). ([support.axway.com][1])

---

## 3. ISO 20022 – famille `sese.020` à `sese.024`

Dans ISO 20022, le “business area” **sese = Securities Settlement**. Ces messages couvrent tout le cycle d’une instruction de règlement titres. ([iotafinance.com][10])

### 3.1. `sese.023` – Securities Settlement Transaction Instruction

* Message cœur pour une instruction de règlement titres. ([iotafinance.com][11])
* **Qui l’envoie ?**
  Un account owner (banque, broker, custodian global) vers son account servicer (CSD, ICSD, sub-custodian, banque).
* **À quoi ça sert ?**
  Instruire une **livraison ou réception** de titres, avec ou sans paiement (DvP ou FoP).
* Tu retrouves dans le message :

  * type de transaction (RECE, DELI, TRAD, COLI, etc.),
  * type de règlement (Free vs Against Payment),
  * settlement amount, currency,
  * parties (buyer, seller, place of settlement, place of trade…),
  * liens vers d’autres opérations (back-to-back, collateral, transformation CA, etc.).

> C’est l’équivalent “générique” des MT540, 541, 542, 543.
> Au lieu d’avoir **4 types de messages**, tu as **un seul message** avec des codes qui disent : *je suis un RECEIVE/D ELIVER, FoP/DvP, etc.* ([Scribd][12])

---

### 3.2. `sese.020` – Securities Transaction Cancellation Request

* **Objectif** : demander l’annulation d’une transaction titres déjà instruite. ([iotafinance.com][13])
* **Qui l’envoie ?** Account owner → account servicer.
* Utilisé quand tu as déjà envoyé un `sese.023` et que tu veux le cancel (erreur de prix, de quantité, erreur de counterpart, etc.).
* Dans T2S et ECMS, on utilise ce message pour pilotage des annulations d’instructions de mobilisation/démobilisation de collatéral, par exemple. ([European Central Bank][14])

Equivalent “logique” d’un MT530 (Transaction Processing Command) avec fonction CANC sur un MT54x, mais en version ISO 20022.

---

### 3.3. `sese.021` – Securities Transaction Status Query

* **Objectif** : questionner le statut d’une transaction titres. ([iotafinance.com][15])
* **Qui l’envoie ?** Account owner → account servicer.
* Tu l’utilises quand tu **n’as pas reçu de confirmation**, ou que le dernier status n’est pas clair (par exemple après un délai inhabituel).

Equivalent fonctionnel d’un back-office qui envoie un MT548 adhoc / qui appelle un opérateur pour demander “où en est ce trade ?”, mais de façon automatisée.

---

### 3.4. `sese.022` – Securities Status or Statement Query Status Advice

* **Objectif** : répondre à une **query** (`sese.021` pour le statut ou `semt.021` pour un statement). ([iotafinance.com][16])
* **Qui l’envoie ?** Account servicer → account owner.
* Indique si la requête peut être traitée, a été rejetée, partiellement traitée, etc.
  (Ce n’est pas la confirmation du règlement lui-même, mais le **statut de la demande de statut / de relevé**.)

---

### 3.5. `sese.024` – Securities Settlement Transaction Status Advice

* **Objectif** : **donner le statut détaillé** d’une instruction de règlement titres (celle envoyée en `sese.023`). ([European Central Bank][17])
* **Qui l’envoie ?** Account servicer (CSD, ICSD, custodian) → account owner.
* Sert à notifier :

  * instruction acceptée / validée,
  * matched / unmatched,
  * partiellement ou totalement réglée,
  * en échec (failing) avec **raison de rejet/d’échec**,
  * annulée, etc.
* Dans les docs T2S/ECMS, tu vois des occurrences comme :
  “sese.024 with status *accepted*, *matched*, *cancelled*, *rejected*…” ([European Central Bank][17])

Equivalent fonctionnel de **MT548** (Settlement Status and Processing Advice) en beaucoup plus riche et structuré.

---

## 4. Correspondance MT54x ↔ ISO 20022 `sese.*`

En simplifiant (pour les flux règlement-livraison standards) : ([Scribd][12])

| Fonction métier                            | MT (ISO 15022)                                         | ISO 20022                                                                                                                        |
| ------------------------------------------ | ------------------------------------------------------ | -------------------------------------------------------------------------------------------------------------------------------- |
| Instruction Receive/Deliver FoP ou DvP     | MT540, 541, 542, 543                                   | `sese.023` (SecuritiesSettlementTransactionInstruction)                                                                          |
| Demande d’annulation d’instruction         | MT530 (CANC sur un MT54x)                              | `sese.020` (SecuritiesTransactionCancellationRequest)                                                                            |
| Demande de statut sur une transaction      | – (pas de message standard dédié, souvent MT548 adhoc) | `sese.021` (SecuritiesTransactionStatusQuery)                                                                                    |
| Réponse sur la query de statut / statement | –                                                      | `sese.022` (SecuritiesStatusOrStatementQueryStatusAdvice)                                                                        |
| Statut de l’instruction de règlement       | MT548                                                  | `sese.024` (SecuritiesSettlementTransactionStatusAdvice)                                                                         |
| Confirmation de règlement                  | MT544-547                                              | `sese.025` (SecuritiesSettlementTransactionConfirmation – que tu n’as pas demandé, mais il existe) ([European Central Bank][18]) |

Idée principale : **ISO 20022 regroupe et généralise** ce qui était éclaté en plusieurs messages MT, avec une granularité plus fine sur les statuts, les raisons, les liens entre opérations.

---

## 5. Types de messages courants en back-office / règlement-livraison

Je te fais un panorama des messages **les plus utilisés** dans un environnement de type SGSS / T2S / CSD, côté titres.

### 5.1. En MT (ISO 15022 – encore très présents dans beaucoup d’outils)

**Règlement (catégorie 5)** ([support.axway.com][1])

* **MT540-543** : instructions de règlement (cf. plus haut).
* **MT544-547** : confirmations de règlement (Receive/Deliver, Free / Against Payment).
* **MT548** : Settlement Status and Processing Advice (statuts, suspensions, rejets, matching…).
* **MT530** : Transaction Processing Command (cancellation, repair, etc.).

**Relevés / reporting**

* **MT535** : Statement of Holdings (positions titres).
* **MT536** : Statement of Transactions (liste des transactions réglées sur une période).
* **MT537** : Statement of Pending Transactions (transactions en attente).
* **MT538** : Statement of Intra-Position Advices (mouvements intra-dépôt).

En back-office, tu les utilises pour :

* mettre à jour les **positions titres/cash**,
* faire les **rapprochements** (reco) entre le système interne et les relevés du CSD/custodian,
* suivre les fails de règlement et les suspensions.

---

### 5.2. En ISO 20022 (MX) – côté `sese.*` et `semt.*`

**Règlement titres – `sese.*`** ([European Central Bank][18])

* `sese.020` – Cancellation Request (annuler une transaction).
* `sese.021` – Status Query (demander le statut).
* `sese.022` – Status Advice sur la query.
* `sese.023` – Settlement Transaction Instruction (l’équivalent des MT540-543).
* `sese.024` – Settlement Transaction Status Advice (équivalent enrichi du MT548).
* `sese.025` – Settlement Transaction Confirmation (équivalent des MT544-547).

**Relevés / positions – `semt.*`** (tu ne les as pas demandés, mais ils sont très courants)

* `semt.002`/`semt.017`/`semt.018` etc. pour les statements de positions et transactions.
* `semt.022` – Securities Settlement Transaction Audit Trail Report (audit trail complet d’une instruction, sans équivalent en MT). ([ISO20022][19])

Ces messages sont utilisés :

* pour alimenter les **positions titres/cash en temps quasi-réel**,
* fournir des **relevés détaillés** aux clients (custody reporting),
* assurer une **traçabilité fine** (audit trail) des instructions de règlement.

---

## 6. En pratique, côté back-office / règlement-livraison

Si on résume un **flux typique** dans un SI comme le tien (back-office, règlement-livraison SWIFT/T2S) :

1. **Capture du trade** (front-office → back-office)

   * En interne : pas forcément SWIFT (FIX, fichiers, API…).
   * Le back-office crée une “instruction de règlement”.

2. **Enrichissement / contrôles**

   * Contrôles de provisions titres & cash, cut-off, marchés, CSD, comptes, SSI, etc.
   * Décision : FoP vs DvP, interne vs externe, T2S ou autre.

3. **Envoi de l’instruction au dépositaire / CSD**

   * En MT : MT540/541/542/543.
   * En ISO 20022 : `sese.023`.

4. **Suivi des statuts**

   * MT : MT548, puis MT544-547 pour les confirmations.
   * ISO 20022 : `sese.024` (status), `sese.025` (confirmation).
   * En cas de problème : `sese.021` (query) et éventuellement `sese.020` (cancellation).

5. **Mise à jour comptable / positions**

   * Via les confirmations de règlement + statements:

     * MT535-538 ou leurs équivalents `semt.*` en ISO 20022.
   * Alimentation des systèmes de positions, PnL, risque, reporting réglementaire.

---

## 7. Ce qu’il faut retenir pour ton contexte (back-office, règlement-livraison)

* **MT vs ISO 20022**

  * MT = ancien standard ISO 15022, simple mais limité.
  * ISO 20022 = XML, beaucoup plus riche, standard cible pour les infrastructures européennes (T2/T2S, ECMS, etc.). ([Société Générale][20])

* **MT540-543**

  * 540 / 541 = RECEIVE (FoP / DvP).
  * 542 / 543 = DELIVER (FoP / DvP).
  * Utilisés pour instruit **qui reçoit/livre quoi, où, quand, comment, et si c’est payé ou pas**.

* **sese.020–024**

  * `sese.023` = l’instruction de règlement (équiv. MT54x).
  * `sese.020` = annulation.
  * `sese.021` & `sese.022` = couple “query / réponse à la query”.
  * `sese.024` = statut détaillé de l’instruction (équiv. MT548, mais plus riche).

* **Types courants côté back-office**

  * MT : 540-543, 544-548, 535-538.
  * ISO 20022 : `sese.020-025`, `semt.*` (statements, audit trail).



[1]: https://support.axway.com/doc/a9caac64ccfe70d59e67d89e01aa7ec1/Content/Supported_msgs/Securities/swift_fin_sec.htm?utm_source=chatgpt.com "SWIFT – FIN MT5xx messages"
[2]: https://www.iso20022.org/iso-20022-message-definitions?utm_source=chatgpt.com "ISO 20022 Message Definitions | ISO20022"
[3]: https://www.swift.com/sites/default/files/documents/swift_standards_iso20022_migration_study_consultation_paper.pdf?utm_source=chatgpt.com "SWIFT ISO 20022 Migration Study Consultation"
[4]: https://banking.vision/en/iso-20022-the-final-chapter-begins/?utm_source=chatgpt.com "ISO 20022: The Final Chapter Begins"
[5]: https://www2.swift.com/knowledgecentre/rest/v1/publications/stdsmx_snr_mdrs/6.0/SR2021_MX_SettlementAndReconciliation_MDR1_Standards.pdf?logDownload=true&utm_source=chatgpt.com "Standards MX Template"
[6]: https://www.clearstream.com/resource/blob/1316592/343b1b4cbafcd910279e4c220ef7e5ee/cbf-dcp-connectivity-handbook-data.pdf?utm_source=chatgpt.com "DCP Connectivity Handbook: ISO 20022 formats and ..."
[7]: https://docs.oracle.com/en/industries/financial-services/banking-trade-finance-service/14.8.0.0.0/tfmes/12.2.3-mt-540-receive-free.html?utm_source=chatgpt.com "12.2.3 MT 540 Receive Free"
[8]: https://docs.oracle.com/cd/E19509-01/820-7113/ahcca/index.html?utm_source=chatgpt.com "Category 5 Messages (SWIFT Integration Projects)"
[9]: https://invivoo.com/blog/reglement-livraison-via-swift-mt-5xx?utm_source=chatgpt.com "Règlement livraison Via SWIFT (MT 5XX)"
[10]: https://www.iotafinance.com/SWIFT-norme-ISO20022-Business-area-sese-Securities-Settlement.html?utm_source=chatgpt.com "SWIFT ISO 20022 - sese"
[11]: https://www.iotafinance.com/SWIFT-norme-ISO20022-Message-sese-023-002-Securities-Settlement-Transaction-Instruction.html?utm_source=chatgpt.com "Securities Settlement Transaction Instruction 002 (sese. ..."
[12]: https://www.scribd.com/document/648287107/Equivalence-between-ISO-15022-and-ISO-20022?utm_source=chatgpt.com "Equivalence Between ISO 15022 and ISO 20022"
[13]: https://www.iotafinance.com/SWIFT-norme-ISO20022-Message-sese-020-001-Securities-Transaction-Cancellation-Request.html?utm_source=chatgpt.com "Securities Transaction Cancellation Request (sese.020.001)"
[14]: https://www.ecb.europa.eu/paym/target/t2s/governance/pdf/crg/ecb.targetseccrg150310_T2S-0520-SYS.en.pdf?utm_source=chatgpt.com "T2S-0520-SYS Add new fields to T2S messages sese.020 ..."
[15]: https://www.iotafinance.com/en/SWIFT-ISO20022-Message-sese-021-002-Securities-Transaction-Status-Query.html?utm_source=chatgpt.com "Securities Transaction Status Query 002 (sese.021.002)"
[16]: https://www.iotafinance.com/SWIFT-norme-ISO20022-Message-sese-022-001-Securities-Status-Or-Statement-Query-Status-Advice.html?utm_source=chatgpt.com "Securities Status Or Statement Query Status Advice (sese. ..."
[17]: https://www.ecb.europa.eu/paym/target/target-professional-use-documents-links/ecms/shared/pdf/ecb.targetecms210615_ECMS_message_usage_guide_v1_2_1.en.pdf?utm_source=chatgpt.com "ECMS message usage guide - European Central Bank"
[18]: https://www.ecb.europa.eu/pub/pdf/annex/T2S_0705_SYS.Annex3UDFS.en.pdf?utm_source=chatgpt.com "1.1.3.1 Status management - European Central Bank"
[19]: https://www.iso20022.org/milestone/23503/download?utm_source=chatgpt.com "Standards MT Release 2025"
[20]: https://www.securities-services.societegenerale.com/en/insights/views/news/t2t2s-consolidation-new-swift-iso-20022-standard-cash-payment-messages/?utm_source=chatgpt.com "T2/T2S consolidation - New SWIFT ISO 20022 standard for ..."
