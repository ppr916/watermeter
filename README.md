# watermeter
Indication pour un wemos d1 et un cc1101 / avec un itron cyble
On utilise les mêmes fichiers (code) que watermeter2mqtt.
Donc mêmes types de paramétrage :
il faut donc régler l’année et le numéro de série du compteur.
puis régler la fréquence si on l’a, sinon faire une recherche.
Seul changement le fichier watermeter.ino
il faut l’identifiant de la box avec le mot de passe.
fixer une ip statique, c’est plus simple pour se connecter dessus.
j’ai rajouter un dossier data avec une mini page html.
il faut le copier avec litleFS (voir doc arduino)
Le programme ne se connecte qu’une fois au démarrage pour préserver la pile du compteur, mais on peut changer.
Ensuite il suffit de se connecter dessus avec un navigateur sur l’ip spécifié.

Chez moi je l’ai branché sur une prise connecté que j’active à la demande.
Le compteur est à 50-60cm sous terre avec une plaque béton dessus, la maison est à environ 50 m et je me connecté facilement

