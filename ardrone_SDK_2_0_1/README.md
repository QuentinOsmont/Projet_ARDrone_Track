# ardrone_SDK_2_0_1

Parrot fournit un kit de développement (SDK) pour l'AR.Drone qui est une référence pour la plupart des projets développés sur ce dernier. Ce kit inclus une interface de programmation (API), qui est un ensemble de classes, de méthodes et de fonctions permettant d'offrir des services au développement d'un programme, et des exemples d'applications pour Iphone, Linux, Windows et Android. Le SDK actuel est configuré pour être compilé avec le système d'exploitation Linux.

## Installation

Tout d'abord, le SDK require les paquets libsdl-dev, libgtk2.0-dev et libiw-dev :
```
sudo apt-get install libsdl-dev libgtk2.0-dev libiw-dev 
```

* libsdl-dev : la SDL (Simple DirectMedia Layer) est une bibliothèque logicielle permettant de développer des programmes gérant le son, la vidéo, le clavier, la souris et le lecteur CD avec plusieurs langages.
* libgtk2.0-dev : le GTL+ est une boîte à outils multi-plateforme permettant de créer des interfaces graphiques.
* libiw-dev : ce paquet intègre un ensemble d'outils permettant de manipuler des fonctions sans fil tel que le WIFI.


## Utilisation

1. Compiler le SDK en tapant la commande `make` dans le dossier "ardrone_SDK_2_0_1/ARDroneLib/Soft/Build".
2. Se rendre dans le dossier  "ardrone_SDK_2_0_1/Examples/Linux/sdk_demo/Build".
3. Compiler la démonstration en tapant la commande `make`.
4. Exécuter le programme dont l'exécutable se situe dans le dossier "ardrone_SDK_2_0_1/Examples/Linux/Build/Release" : `./linux_sdk_demo`

## Utilisation avec un autre système d'exploitation

Dans l'objectif d'utiliser un autre système d'exploitation, il suffit de modifier le fichier "custom.makefile" se situant dans le dossier "ardrone_SDK_2_0_1/ARDroneLib/Soft/Build".
* changer le "no" en "yes" pour le système utilisé
ste des bugs d'utilisation sur "google chrome".


