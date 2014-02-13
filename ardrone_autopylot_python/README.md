# ardrone_autopylot_python

Simon D. Levy est professeur dans un département informatique pour une université à
Washigton. Il a réalisé un programme permettant de contrôler le drone par l’intermédiaire un
joystick mais également par un module de suivit d’objet en Python (Voir vidéo : http://www.youtube.com/watch?v=_3697dtyOz4). Nous allons expliquer les étapes de compilation du projet.

## Modification du fichier "autopylot.makefile"

* le chemin du SDK de l’AR.Drone (ligne 24)
```
# This should reflect where you put the SDK
SDK_DIR:=/home/quentin/Documents/ARDrone_SDK_2_0_1
```
* le type de contrôleur utilisé en commentant celui intéressé et décommentant l’autre (lignes 27-28). Pour connaître l’adresse de votre manette, il faut taper « lsusb » dans un terminal :
```
# We currently support LOGITCH and PS3 gamepads
#GAMEPAD = GAMEPAD_LOGITECH_ID=0x046dc215
GAMEPAD = GAMEPAD_PS3_ID=0x054c0268
```

* le langage de compilation utilisé, en l’occurrence python pour effectuer un suivit d’objet
```
# Python version: you may need to run apt-get install python-dev as root
PYVER = 2.7
# If you use Python, make sure that PYTHONPATH shell variable contains .
# (dot; current directory). In ~/.bashrc:
# export PYTHONPATH=$PYTHONPATH:.
LANGUAGE = python
LANGUAGE_LIB = -L/usr/lib/python$(PYVER)/config -lpython$(PYVER) -lm
LANGUAGE_PATH = /usr/include/python$(PYVER)
```

Nécessite l'installation de Python 
```
sudo apt-get install python-dev
```

Le fichier "autopylot.makefile" est configuré pour utiliser Python, il peut être également modifié pour Matlab ou C. Cependant pour utiliser le tracking, il faut choisir Python.
Dans ce cas, la variable shell « PYTHONPATH » doit être définie pour inclure le répertoire courant. Pour une utilisation à long terme, il est préférable de modifier le fichier « /.bashrc » en y ajoutant le chemin vers les bibliothèques python (dont la version testée est 2.7) et le répertoire courant par la ligne suivante :
```
export PYTHONPATH=/usr/include/python2.7:/usr/lib/2.7:.
```

## Utilisation

1. Se connecter au drone en WIFI
2. Lancer le makefile par la commande `make`
3. Exécuter le programme `./ardrone_autopylot`

## Contrôle du drone

Le pilote automatique du programme n’est pas initialisé par défaut. Nous devons appuyer sur le bouton de pilote automatique (croix pour la manette PS3) pour que le contrôle soit transférer sur la fonction du fichier « autopylot_agent.py ». Toute action sur la manette rend le contrôle à l’utilisateur. Le projet comporte certaines failles :
* le programme peut prendre plusieurs secondes pour communiques les données de navi-
gation
* le vecteur vitesse continue de rester à zéro
* l’altitude du drone continue de rester à zéro

Pour une amélioration du projet, il faut modifier le fichier « autopylot_agent.py ».

Chacune des commandes pour contrôler le drone doivent doivent s'effectuer par une manette ayant le contrôleur PS3 pour la configuration actuelle (ou avec un joystick Logitech avec l'autre configuration).
* Axe - Stick - Effet
* 0 - Stick gauche : gauche/droite - Roll (voyage de côté)
* 1 - Stick gauche : avant/arrière - Pitch (voyage vers l'avant ou l'arrière)
* 2 - Stick droit : gauche/droite - Yaw (sens horaire/anti-horaire)
* 3 - Stick droit : avant/arrière - Altitude (avant=haut, arrière=bas)

* Bouton - Effet
* 8 (select) - Quitter le programme (signal d'urgence en vol)
* 9 (start) - Décollage/Atterrissage
* 0 (carré) - Change de caméra
* 1 (croix) - Pilote automatique

Ces configurations peuvent être modifiés dans le fichier gamepad.c.
