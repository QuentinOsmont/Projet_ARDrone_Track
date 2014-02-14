# ardrone_urbi_embarque

## Installation et utilisation

* Télécharger Urbi 2.6 à cette adresse : http://www.psykokwak.com/blog/images/ardrone/urbi-sdk-2.6-525.tar
* Se connecter au drone en WIFI
* Envoyer le fichier au drone via ftp
```
ftp 192.168.1.1
```
* Placer le fichier urbi-sdk-2.6-525.tar
```
put urbi-sdk-2.6-525.tar
```
* Se connecter au drone via telnet
```
telnet 192.168.1.1
```
* Décompresser le fichier
```
cd data/video
tar xf urbi-sdk-2.6-525.tar
```
* Lancer le script
``` 
cd urbi-sdk-2.6-525
./launch.sh
```
* Se connecter sur telnet via un autre terminal
```
telnet 192.168.1.1 54000
```
* Communiquer avec le drone (ex : "drone.battery;")

## Remarque 

Ce projet n'est pas encore au point.
