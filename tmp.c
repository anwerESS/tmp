Titre :

Préparation d’un nouvel environnement C++ sur VDI (migration NDG → VDI)


Description :

Dans le cadre de l’arrêt du support des postes NDG (à partir du 1er octobre), il est nécessaire de mettre en place un nouvel environnement C++ sur une VDI haute performance afin d’assurer la continuité et le support des applications.



Tâches réalisées:

* Création d’une VDI (High Performance : 8 vCPU – 32 Go RAM – 250 Go disque).
* Demande de droits administrateur.
* Demande d’accès à tous les répertoires nécessaires.
* Installation et configuration de l’environnement : Maven, JDK, PX, Git, Autosys.
* Configuration des variables d’environnement.
* Installation et configuration de NatSar (<url>).
* Mise à jour du `settings.xml` (remplacement des dépôts Nexus par JFrog suite à la migration).
* Installation de SQL Developer et configuration des connexions BDD (HOME3, ASB1, DEV, HOM4…).
* Configuration de l’Oracle Driver.
* Installation et configuration du SDK 8.1.
* Installation de Visual Studio 2022.
* Lancer un build et au moins un debug pour valider que l’environnement permet d’intervenir en cas d’incident ou de correctif.
