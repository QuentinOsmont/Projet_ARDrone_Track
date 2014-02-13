# ardrone_webflight_javascript

Cette application permet de piloter le drone directement depuis notre navigateur. De plus, il est possible d'étendre l'application avec des plugins afin d'ajouter des fonctionnalités telles que l'enregistrement vidéo, le vol autonome, la reconnaissance des visages, le suivit d'objet et plus encore. Cela permet d'avoir un environnement convivial pour construire et expérimenter notre drone. Le projet source provient de ce dépôt : http://eschnou.github.io/ardrone-webflight/. Le programme n'a pas été modifié à ce jour, seulement l'ensemble des plugins ont été ajoutés au dossier "plugins".

## L'ensemble des plugins

* **[video-png](plugins/video-png/)** permet de diffuser la vidéo sur le navigateur par le chargement d’une image statique. Cela fonctionne très bien sur tous les navigateurs. Nécessite l’installation du
paquet ffmpeg sur le système.

* **[video-stream](plugins/video-stream/)** utilise node-dronestream pour diffuser le flux vidéo en h264 via des websockets en javascript. Nécessite un navigateur et un processeur modernes.

* **[hud](plugins/hud/)** permet de visualiser en haut de la fenêtre une boussole, un altimètre, ..., basé sur nodecoper-cockpit.

* **[pilot](plugins/pilot)** permet de contrôler le drone en utilisant le clavier. Le contrôle s’effectue sur le navigateur. 

* **[battery](plugins/battery)** affiche le niveau de la batterie dans la barre du haut.

* **[blackbox](plugins/blackbox)** enregistre toutes les données de la mission (flux vidéo, données de na-
vigaions, ...) sur le disque.

* **[replay](plugins/replay)** permet de rejouer une mission en injectant des données au niveau du client.
Outil utile pour coder, tester, et déboguer lorsque nous pouvons pas faire voler le drone. Nécessite le module video-png, il n’est pas encore compatible avec video-stream.


* **[copterface](https://github.com/eschnou/webflight-copterface)** détecte les visages et permet au drone de les suivre en effectuant une rotation.

* **[traffic](https://github.com/wiseman/webflight-traffic)** affiche un trafic aérien depuis des données ADS-B comme une superposition de la réalité augmentée.

* **[gamepad](https://github.com/wiseman/webflight-gamepad)** permet de contrôler le drone avec une manette. 
* **[tracker](https://github.com/bkw/webflight-tracker)** permet de suivre un objet par ses pixels sur le flux vidéo en cliquant dessus. 

* **[trollface](https://github.com/andrew/webflight-trollface)** permet de détecter les visages et dispose un visage de troll par dessus. 

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


## License

The MIT License

Copyright (c) 2013 by the AUTHORS

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
