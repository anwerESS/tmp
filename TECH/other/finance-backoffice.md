### 🏦 MÉTIER – QUESTIONS / RÉPONSES ORALES (BACK-OFFICE & SETTLEMENT)

<details>

### `C’est quoi le backoffice ?`

<summary>C’est quoi le backoffice </summary>

> Le back-office intervient après l’exécution de la transaction.
> Il s’occupe de la **confirmation**, du **règlement-livraison**, de la **réconciliation**, du **suivi des statuts** et de la **gestion des incidents**.
</details>



<details>

### `C’est quoi les titres ?`

<summary>C’est quoi les titres ?</summary>

> Les titres sont des **instruments financiers**, comme les **actions**, les **obligations** ou les **fonds**.
> En back-office, quand on parle de titres, on parle des actifs financiers qui sont achetés, vendus, conservés et livrés entre les différentes parties.
</details>



<details>

### `C’est quoi le principe DVP ?`

<summary>C’est quoi le principe DVP ?</summary>

> DVP signifie **Delivery Versus Payment**.
> Ça veut dire que la livraison des titres et le paiement du cash se font **en même temps**.
> Soit les deux se font, soit rien ne se fait, ce qui évite les risques pour l’acheteur et le vendeur.
</details>


<details>

###  `Qui sont le buyer/seller ?`

<summary> Qui sont le buyer/seller ?</summary>

> Le **buyer** est l’acheteur des titres, et le **seller** est le vendeur.
> Ils peuvent être des banques, des brokers, des fonds ou des investisseurs institutionnels.
> Chacun passe par son dépositaire pour le règlement-livraison.
</details>



<details>

### `C’est quoi le PSET ?`

<summary>C’est quoi le PSET ?</summary>


> Le PSET, c’est la **place de règlement**, c’est-à-dire le **dépositaire central** où le règlement-livraison a lieu.
> En France, par exemple, c’est **Euroclear France**.
</details>


### 🔹 RL

<details>

#### `C’est quoi le R/L ?`

<summary>C’est quoi le R/L ?</summary>

> Le règlement-livraison, c’est l’étape finale d’une transaction financière.
> Elle consiste à **livrer les titres** à l’acheteur et à **payer le cash** au vendeur, de manière sécurisée et au bon moment.
</details>



<details>

#### `Comment se fait un R/L ?`

<summary>Comment se fait un R/L ?</summary>

> Une fois la transaction exécutée sur le marché, chaque partie envoie une instruction de règlement à son dépositaire.
> Le dépositaire central vérifie que les instructions correspondent, que les titres et le cash sont disponibles, puis effectue le transfert **simultané** des titres et de l’argent.
</details>



<details>

#### `Les acteurs du du R/L ?`

<summary>Les acteurs du du R/L ?</summary>

> Il y a l’acheteur et le vendeur, leurs banques ou custodians, et surtout le **dépositaire central**, comme Euroclear ou Clearstream, qui garantit le bon déroulement du règlement-livraison.
</details>



<details>

#### `Exemple simple de R/L`

<summary>Exemple simple de R/L</summary>

> Par exemple, un client achète 100 actions.
> À la date de règlement, il reçoit les 100 actions sur son compte titres, et le vendeur reçoit le cash correspondant.
> Les deux mouvements sont faits en même temps grâce au principe DVP.

</details>


### 🔹 swift

<details>

#### un SWIFT?

<summary> un SWIFT ?</summary>

> Un message SWIFT, c’est un message standard utilisé par les banques pour échanger des infos de paiement de façon sécurisée.”

</details>



<details>

#### Sur quels messages SWIFT avez-vous travaillé ?

<summary> Sur quels messages SWIFT avez-vous travaillé ?</summary>

> J’ai travaillé principalement sur les messages SWIFT de règlement-livraison, notamment les **MT541** et **MT543** pour les opérations DVP, ainsi que les messages de statut comme le **MT548**.

</details>


<details>

####  À quoi servent , MT541, MT542, MT543 et MT544 ?

<summary>  À quoi servent , MT541, MT542, MT543 et MT544 ?</summary>


> Le **MT541** `Receive Against Payment` est utilisé quand on **reçoit des titres contre paiement**, donc côté acheteur.<br>
> Le **MT543** `Deliver Against Payment`est utilisé quand on **livre des titres contre paiement**, donc côté vendeur.
>
> Le **MT542**  `Receive Free of Payment (R-FOP)`est utilisé quand on reçoit des titres sans paiement.<br>
> Le **MT544**  `Deliver Free of Payment (R-FOP)`est utilisé quand on livre des titres sans recevoir de paiement.

>
> Les MT541 et MT543 sont utilisés pour des opérations avec paiement (DVP).
> Les MT542 et MT544 sont utilisés pour des opérations sans paiement (FOP).
>
> 541 / 542 → réception de titres
> 543 / 544 → livraison de titres

</details>


<details>

#### À quoi sert le MT548 ?

<summary> À quoi sert le MT548 ?</summary>


> Le MT548 sert à suivre le **statut** d’une instruction de règlement-livraison : acceptée, en attente, échouée ou réglée.
</details>


### 🔹 ISO 20022

<details>

#### ISO 20022 / messages sese.02x ?

<summary> ISO 20022 / messages sese.02x ?</summary>


> Oui, j’ai travaillé sur la migration vers ISO 20022, notamment sur les messages **sese.02x**.
> Par exemple, **sese.023** pour instruire une opération de règlement-livraison, et **sese.024** pour recevoir les statuts, qui remplacent en grande partie les MT54x et MT548.
</details>


<details>

#### Différence MT et ISO 20022 (en une phrase)

<summary> Différence MT et ISO 20022 (en une phrase)</summary>

> Les MT sont des messages historiques au format texte, alors que ISO 20022 est un format XML plus riche, plus structuré et mieux adapté aux exigences réglementaires actuelles.

</details>


---

### 🔹 AUTRE


<details>

<summary>  🎯 Tu as une préférence pour un secteur d’activité ? </summary>

> J’ai une préférence pour la finance, vu mon expérience, mais je reste ouvert à d’autres secteurs si le projet est intéressant.

</details>

<details>

<summary> 🎯  Tu veux continuer dans la finance ? Pourquoi ? </summary>

> Oui, ça m’intéresse de rester dans la finance, parce que j’ai déjà travaillé dessus et je trouve le domaine intéressant.
> Il y a à la fois de la technique et du métier, donc c’est motivant.

</details>

<details>

<summary> action, obligation, option </summary>

Une `action`, c’est une part du capital d’une entreprise.    
L’acheteur devient propriétaire d’une fraction de la société.    
Si l’entreprise se développe, la valeur de cette part augmente  
Une `obligation` représente un prêt.  
L’investisseur prête son argent à l'État ou a une entreprise et perçoit des intérêts réguliers.  
 Une `option`, c’est un contrat qui donne un droit, mais pas d’obligation.  
L’acheteur peut acheter ou vendre plus tard à un prix fixé aujourd’hui.  

</details>

<details>

<summary> front-office, middle-office, back-office </summary>

Le `front-office`, c'est le cœur du trading. Les traders achètent et vendent des actions, des obligations, des options en temps réel. Les systèmes doivent être très rapides.  
Le `middle-office` gère les risques et le contrôle entre les transactions.  
Le `back-office` intervient après l’exécution de la transaction.  
Il s’occupe de la **confirmation**, du **règlement-livraison**, de la **réconciliation**, du **suivi des statuts** et de la **gestion des incidents**.

`BACK-OFFICE ==> FONT-OFFICE`  
Aujourd’hui, j’ai une expérience en back-office, mais justement je vois ça comme une excellente opportunité d’élargir ma vision, de comprendre aussi le front-office et le fonctionnement global de la finance.

</details>


<details>

<summary> SGSS </summary>

`SGSS`, c’est la branche de Société Générale spécialisée dans les services aux institutions financières.  
Ils gèrent tout ce qui est post-trade, comme la conservation des titres et le règlement-livraison.

</details>

---

## ✅ Instruments financiers

<details>

<summary> ✅ Instruments financiers</summary>

<details>
<summary>Qu'est-ce qu'une <b>position</b> en finance ?</summary>

Une position, c'est la quantité d'un actif détenue dans un portefeuille.

Par exemple, si un fonds possède 1 000 actions d'une société, alors il a une position de 1 000 actions sur cette société.

</details>


<details>
<summary>Qu'est-ce qu'une <b>transaction</b> ?</summary>

Une transaction, c'est une opération financière, comme un achat ou une vente d'un instrument financier.

Cette transaction peut ensuite modifier les positions du portefeuille.

</details>


<details>
<summary>Quels <b>instruments financiers</b> connaissez-vous ?</summary>

Je connais les actions, les obligations, les fonds, les options, les futures, les forwards, les swaps et les CDS.

Je connais les principes de base, et je continue à approfondir la partie fonctionnelle finance.

</details>

<details>
<summary>C'est quoi une <b>action</b> ?</summary>

Une action, c'est une part du capital d'une entreprise.

Quand on détient une action, on détient une petite partie de l'entreprise, avec parfois un droit aux dividendes et un droit de vote.

</details>

<details>
<summary>C'est quoi une <b>obligation</b> ?</summary>

Une obligation, c'est un titre de dette.

Quand on achète une obligation, on prête de l'argent à un État ou à une entreprise, et en échange on reçoit généralement des intérêts, puis le remboursement du capital à l'échéance.

</details>

<details>
<summary>C'est quoi une <b>option</b> ?</summary>

Une option donne le droit, mais pas l'obligation, d'acheter ou de vendre un actif à un prix fixé à l'avance.

Il y a deux grands types : le call pour acheter, et le put pour vendre.

</details>

<details>
<summary>C'est quoi un <b>call</b> ?</summary>

Un call est une option d'achat.

Il donne le droit d'acheter un actif à un prix fixé à l'avance.

</details>

<details>
<summary>C'est quoi un <b>put</b> ?</summary>

Un put est une option de vente.

Il donne le droit de vendre un actif à un prix fixé à l'avance, souvent pour se protéger contre une baisse du marché.

</details>

<details>
<summary>C'est quoi un <b>future</b> ?</summary>

Un future est un contrat standardisé qui permet d'acheter ou vendre un actif à une date future et à un prix fixé à l'avance.

Il est généralement négocié sur un marché organisé.

</details>



<details>
<summary>C'est quoi un <b>repo</b> ?</summary>

Un repo est une opération où une partie obtient du cash à court terme en donnant des titres en garantie.

C'est utilisé notamment pour gérer la liquidité de manière sécurisée.

</details>

<details>
<summary>C'est quoi le <b>margin</b> ?</summary>

Marge (margin) : somme/garantie que le membre doit déposer pour couvrir le risque lié à ses positions.
Collateral : les actifs utilisés comme garantie pour fournir cette marge.exemple, on peut déposer du cash ou des obligations de bonne qualité pour sécuriser une exposition ou réduire le risque de contrepartie.

</details>

<details>
<summary>C'est quoi le <b>collatéral</b> ?</summary>

Le collatéral, c'est une garantie donnée dans une opération financière.

Par exemple, on peut déposer du cash ou des obligations de bonne qualité pour sécuriser une exposition ou réduire le risque de contrepartie.

</details>

</details>


<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>