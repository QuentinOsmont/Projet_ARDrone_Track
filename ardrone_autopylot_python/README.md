# ardrone_autopylot_python

Simon D. Levy est professeur dans un département informatique pour une université à
Washigton. Il a réalisé un programme permettant de contrôler le drone par l’intermédiaire un
joystick mais également par un module de suivit d’objet en Python (Voir vidéo). Nous allons
expliquer les étapes de compilation du projet.

## L'ensemble des plugins

* **[video-png](plugins/video-png/)** permet de diffuser la vidéo sur le navigateur par le chargement d’une image statique. Cela fonctionne très bien sur tous les navigateurs. Nécessite l’installation du
paquet ffmpeg sur le système. (module disponible sur https://github.com/eschnou/ardrone-webflight/blob/master/plugins/video-png/)

* **[video-stream](plugins/video-stream/)** utilise node-dronestream pour diffuser le flux vidéo en h264 via des websockets en javascript. Nécessite un navigateur et un processeur modernes. (module disponible sur https://github.com/eschnou/ardrone-webflight/blob/master/plugins/video-stream/)

* **[hud](plugins/hud/)** permet de visualiser en haut de la fenêtre une boussole, un altimètre, ..., basé sur nodecoper-cockpit. (module disponible sur https://github.com/eschnou/ardrone-webflight/blob/master/plugins/hud/)

* **[pilot](plugins/pilot)** permet de contrôler le drone en utilisant le clavier. Le contrôle s’effectue sur le navigateur. (module disponible sur https://github.com/eschnou/ardrone-webflight/blob/master/plugins/pilot)

* **[battery](plugins/battery)** affiche le niveau de la batterie dans la barre du haut. (module dispo-
nible sur https://github.com/eschnou/ardrone-webflight/blob/master/plugins/battery)

* **[blackbox](plugins/blackbox)** enregistre toutes les données de la mission (flux vidéo, données de na-
vigaions, ...) sur le disque. (module disponible sur https://github.com/eschnou/ardrone-webflight/blob/master/plugins/blackbox)

* **[replay](plugins/replay)** permet de rejouer une mission en injectant des données au niveau du client.
Outil utile pour coder, tester, et déboguer lorsque nous pouvons pas faire voler le drone. Nécessite le module video-png, il n’est pas encore compatible avec video-stream. (module disponible sur https://github.com/eschnou/ardrone-webflight/blob/master/plugins/replay)


* **[copterface](https://github.com/eschnou/webflight-copterface)** détecte les visages et permet au drone de les suivre en effectuant une rotation. (module disponible sur https://github.com/eschnou/webflight-copterface)

* **[traffic](https://github.com/wiseman/webflight-traffic)** affiche un trafic aérien depuis des données ADS-B comme une superposition de la réalité augmentée. (module disponible sur https://github.com/wiseman/

* **[gamepad](https://github.com/wiseman/webflight-gamepad)** permet de contrôler le drone avec une manette. (module disponible sur https://github.com/wiseman/webflight-gamepad)

* **[tracker](https://github.com/bkw/webflight-tracker)** permet de suivre un objet par ses pixels sur le flux vidéo en cliquant dessus. (module disponible sur https://github.com/bkw/webflight-tracker)

* **[trollface](https://github.com/andrew/webflight-trollface)** permet de détecter les visages et dispose un visage de troll par dessus. (module disponible sur https://github.com/andrew/webflight-trollface)

## Installation

Le projet require une version récente de nodejs (node > 0.10).
```
sudo apt-get install python-software-properties python g++ make
sudo apt-get install software-properties-common
sudo add-apt-repository ppa:chris-lea/node.js
sudo apt-get update
sudo apt-get install nodejs
```

Afin de configurer le projet avec node, le dossier doit être initialisé par le gestionnaire
de paquets "npm" réservé à node.js et "Bower" qui va permettre d’installer, de mettre à
jour ou de désinstaller toutes les dépendances qui sont nécessaires au projet.
```
git clone https://github.com/QuentinOsmont/Projet_ARDrone_Track
cd Projet_ARDrone_Track/ardrone_webflight_javascript
npm install
sudo npm install -g bower
bower install
```

Pour utiliser les plugins, le fichier config.js doit être configuré :
```
cp config.js.example config.js
//Modification du fichier config.js pour l'utilisation du plugin tracker
var config = {
    plugins: [ "video-stream", "hud", "battery", "pilot", "tracker" ],
    ...
```

Dans le but d'utiliser les plvar config = {
plugins: [ "video-stream", "hud", "battery", "pilot", "tracker" ],
...ugins video, le paquet "ffmpeg" doit être installé. De plus pour une utilisation correcte du projet, le navigateur "firefox" avec une version récente doit être utilisé. En effet, le programme nécessite un navigateur récent et performant. Il existe des bugs d'utilisation sur "google chrome".

## Utilisation

1. Se connecter au drone en WIFI
2. Lancer `node app.js`
3. Aller sur le navigateur à l'adresse `http://localhost:3000/`


### Contrôle du drone

Chacune des commandes pour contrôler le drone doivent être tapées dans la fenêtre du navigateur (nécessite le module « pilot »). 
* Commande -> Fonction
* t -> décollage
* l -> atterrissage
* z -> déplacement avant
* s -> déplacement arrière
* q -> déplacement latéral gauche
* d -> déplacement latéral droit
* TAB -> change de vitesse d’accélération (lente/rapide)
* f -> effectue un flip dans le sens actuel du mouvement
* c -> change de caméra
* e -> permet de se remettre d’une situation d’urgence
* r -> enregistre/arrète l’enregistrement
