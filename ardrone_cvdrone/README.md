# ardrone_cvdrone

CVdrone est un projet github permettant d'allier l'AR.Drone et OpenCV. C'est un logiciel libre qui peut donc être repris et poursuivi. Il permet une première approche de la reconnaissance de couleur même s'il n'implémente pas le tracking. Nous l'avons testé sur Windows et Linux et il fonctionne sur les deux OS. Il est également censé fonctionner sous Mac OS. Son inconvénient est qu'il requiert Visual Studio pour l'utilisation sous Windows. Bien que gratuit pour les étudiants, Visual Studio est un logiciel créé par Microsoft qui est payant pour les autres utilisateurs. Cependant, utilisé sous Linux, ce projet peut s'utiliser gratuitement et c'est un bon point de départ pour notre projet.

## Sous linux

### Installation

Paquets nécessaire :
```
sudo apt-get install ffmpeg
sudo apt-get install libopencv-dev
```

* FFmpeg est un logiciel de lecture et encodage de vidéo. Il assure en ligne de commande la possibilité de convertir les fichiers vidéo d'un format à un autre.
* Libopencv-dev contient les fichiers d'en-tête et la bibliothèque statique nécessaires pour compiler des applications qui utilisent OpenCV.

### Utilisation

1. Se connecter au drone en WIFI
2. Lancer `make` dans le dossier "build/linux"
3. Exécuter le fichier `./test.a`

## Sous Windows

### Utilisation

1. Ouvrir le fichier `test.sln` situer dans le dossier "build/vs_2012"
2. F5 pour exécuter le programme 
3. Si erreur "Cannot find or open the PDB file" : Outils->Options->Débogage->Symboles->Cocher "Serveurs de symboles Microsoft"

## Libraries utilisées 

* OpenCV 2.4.6 : http://opencv.org/
* FFmpeg 2.0 : http://www.ffmpeg.org/
* stdint.h/inttypes.h pour Microsoft Visual Studio r26 : https://code.google.com/p/msinttypes/
* POSIX Threads pour Win32 2.9.1 : http://www.sourceware.org/pthreads-win32/


## Licence

Copyright (C) 2013 puku0x

https://github.com/puku0x/cvdrone

Pour chaque bibliothèque utilisée, un fichier de licence existe dans le dossier "licenses".
