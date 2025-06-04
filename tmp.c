Bonjour,

Suite à mes analyses et à un débogage, j’ai constaté que le batch sélectionne actuellement les instructions dans la base sans ordre explicite (ORDER BY). Dans ce cas, c’est l’ordre par défaut du moteur de base de données qui est appliqué.

Concernant le changement de comportement observé depuis décembre, il est possible qu’une migration de version d’Oracle SQL ait eu lieu à cette période. Cela pourrait expliquer une modification dans l’ordre de restitution des résultats, en l’absence d’un tri explicite dans la requête.

Pour corriger cela et garantir un traitement conforme à la règle métier (ordre par date de dénouement, puis par quantité en cas d’égalité), je recommande de modifier la requête SQL en y ajoutant un tri explicite sur la date de dénouement, puis sur la quantité.

N'hésitez pas à me dire si vous souhaitez que je vous accompagne dans cette mise à jour.

Bien cordialement,
[Votre prénom et nom]
