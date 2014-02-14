# ardrone_urbi (non embarqué)

Cette application permet de piloter le drone depuis une interface. De plus, il est possible d'étendre l'application avec des plugins afin d'ajouter des fonctionnalités telles que l'enregistrement vidéo, le vol autonome, la reconnaissance des visages, le suivit d'objet et plus encore. Cela permet d'avoir un environnement convivial pour construire et expérimenter notre drone. Le projet source provient de : http://www.psykokwak.com/blog/index.php/2010/10/08/58-ar-drone-et-urbi-en-remote. 

## Sous Linux 

### Installation

Le projet require l'utilisation d'Urbi 2.3.
* Télécharger le dossier
```
http://www.psykokwak.com/blog/images/ardrone/urbi-sdk-2.3-linux-x86-gcc4-ardrone-1.0.tar.bz2
```
* Décompresser le dossier
```
tar -xvf http://www.psykokwak.com/blog/images/ardrone/urbi-sdk-2.3-linux-x86-gcc4-ardrone-1.0.tar.bz2
```

### Utilisation

1. Se connecter au drone en WIFI
2. Lancer la commande `./start.sh` dans le dossier d'Urbi
3. Si une erreur survient (Tag 19), il faut exécuter la démonstration linux dans le SDK puis de relancer le script `./start.sh`
4. Ouvrir un nouveau terminal 
5. Effectuer la commande `telnet localhost 5400`
6. Dialoguer avec le drone (ex : `drone.battery;`)

### Mode graphique

* Télécharger Gostai Lab à cette adresse : http://www.psykokwak.com/blog/images/ardrone/urbi-sdk-2.3-linux-x86-gcc4-ardrone-1.0.tar.bz2
* Lancer Urbi
* Lancer Gostai (Remarque, il faut reculer la date de l'ordinateur d'au moins deux ans pour utiliser Gostai sans clé)
* Ouvrir le fichier "balltracking.ula"
* Lancer la connection au drone.

