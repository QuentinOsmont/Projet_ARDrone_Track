# ardrone_webflight_javascript

Cette application permet au drone de suivre une cible colorée. Cependant, les sources proviennent d'un projet conçu avec le SDK 1.8 et adapté pour l'AR.Drone 1.0. L'application a été modifié pour être compilé avec le SDK 2.0.1 mais n'affiche toujours pas le flux video dans la fenêtre OpenCV (fenêtre grisé).

## Installation

```
git clone https://github.com/QuentinOsmont/Projet_ARDrone_Track
cd Projet_ARDrone_Track/ardrone_trackeur_sdk/Build
```

* Modification du fichier "Makefile" ligne 1
```
SDK_PATH:=/home/<USER>/<PATH>/ARDrone_SDK_2_0_1/ARDroneLib
```

* Compilation du projet
```
make
```

## Utilisation

* Exécution du projet
```
cd Release
./ardrone_testing_tool
```

## Amélioration

Afin d'obtenir la vidéo, il faut convertir le fichier "video_stage" sans erreur de sorte qu’il fonctionne avec le nouvel SDK. La partie à modifier se situe dans la définition de la routine du thread "video_stage".

## Contrôle du drone

Dans le cas où la conversion se réalise, ci-joint la liste des commandes pour piloter le drone. Il dispose de deux modes de pilotages : le tracking et le manuel (par défaut).

### Mode tracking
* Commande -> Fonction
* d -> décollage
* a -> atterrissage
* echap -> arrêt du programme seulement si le drone est au sol
* clic -> cliquer sur la couleur à suivre dans la fenêtre ouverte par OpenCV. Cela est préférable quand le drone est au sol
* s -> fin du suivit d'objet

### Mode manuel
* d -> décollage
* a -> atterrissage
* flèches directionnelles -> déplacements avant/arrière et latéraux
* h -> voler plus haut
* b -> voler plus bas
* j -> rotation anti-horaire
* k -> rotation horaire
* echap -> arrêt du programme (cause une erreur)
