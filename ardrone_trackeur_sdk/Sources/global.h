#ifndef GLOBAL_H
#define GLOBAL_H

// define pour le mode
#define MANUEL 0
#define TRACKING 1

// Garde le résultat de cvWaitKey, permet de savoir sur quelle touche a appuyé l'utilisateur
extern char key;

// Type de déplacement : HOVER, UP, DOWN, LEFT, RIGHT, FORWARD, BACKWARD, TURN_LEFT, TURN_RIGHT
enum deplacement {
	HOVER = 0,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FORWARD,
	BACKWARD,
	TURN_LEFT,
	TURN_RIGHT
};

// Déplacement à faire faire au drone
extern enum deplacement deplacement_a_faire;

// Pour savoir si on est en vol ou non
extern int flying;

// Pour savoir si on est en déplacement manuel ou en tracking (cf define plus haut)
extern int mode;

#endif // GLOBAL_H
