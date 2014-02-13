#include <unistd.h>
#include <stdio.h>
#include "move.h"
#include "global.h"
#include <ardrone_api.h>
#include <ardrone_tool/ardrone_tool.h>
#include "ardrone_tool/UI/ardrone_input.h"
#include "ardrone_testing_tool.h"

DEFINE_THREAD_ROUTINE(move, data) {

	int continuer = 1; 
	char last_key = -1;
	
	while (continuer) {

		// On n'affiche la touche que si elle a changé et qu'elle n'est pas rien (-1)
		if (last_key != key) {
			last_key = key;
			if (key != -1) {
				printf("Touche appuyée : %c\n", key);		
			}
		}
		
		if (mode == MANUEL) {
		/*
			Mode de déplacement manuel
			h/b : aller plus haut/bas
			j/k : rotation gauche/droite
			flèches directionnelles : déplacements correspondants
			Rmq : équivalent flèche / valeur retournée par cvWaitKey
				flèche gauche -> 81 ('Q')
				flèche droite -> 82 ('R')
				flèche haut   -> 83 ('S')
				flèche bas    -> 84 ('T')
		*/
			switch (key) {
				case 'h' :
					deplacement_a_faire = UP;
					break;
				case 'b' :
					deplacement_a_faire = DOWN;
					break;
				case 'j' :
					deplacement_a_faire = TURN_LEFT;
					break;
				case 'k' :
					deplacement_a_faire = TURN_RIGHT;
					break;
				case 'Q' :
					deplacement_a_faire = LEFT;
					break;
				case 'R' :
					deplacement_a_faire = FORWARD;
					break;
				case 'S' :
					deplacement_a_faire = RIGHT;
					break;
				case 'T' :
					deplacement_a_faire = BACKWARD;
					break;
				default :
					deplacement_a_faire = HOVER;
					break;
			}
		} else {
			// Si l'utilisateur appuie sur la touche 's' on arrête le tracking
			if(key == 's') {
				mode = MANUEL;
			}
		}
		
		// Si on a décollé
		if (flying) {
			// Si l'utilisateur appuie sur d
			if (key == 'a') {
				// On atterrit
				printf("Atterrissage !\n");
				ardrone_tool_set_ui_pad_start(0);
				flying = 0;
			} else { // Sinon on gère les déplacements
			/*
				Arguments de ardrone_at_set_progress_cmd (pour déplacer le drone)
				flag (0 hover, 1 pas hover, hover = sur place)
				angle gauche/droit (<0 gauche) [-1., 1.]
				angle avant/arrière (<0 avant) [-1., 1.]
				vitesse verticale [-1., 1.]
				vitesse angulaire (<0 antihoraire) [-1., 1.]
			*/
				switch (deplacement_a_faire) {
					case HOVER :
						ardrone_at_set_progress_cmd(0, 0., 0., 0., 0.);
						break;
					case UP :
						ardrone_at_set_progress_cmd(0, 0., 0., 0.3, 0.);
						break;
					case DOWN :
						ardrone_at_set_progress_cmd(0, 0., 0., -0.3, 0.);
						break;
					case LEFT :
						ardrone_at_set_progress_cmd(1, -0.05, 0., 0., 0.);
						break;
					case RIGHT :
						ardrone_at_set_progress_cmd(1, 0.05, 0., 0., 0.);
						break;
					case FORWARD :
						ardrone_at_set_progress_cmd(1, 0., -0.05, 0., 0.);
						break;
					case BACKWARD :
						ardrone_at_set_progress_cmd(1, 0., 0.1, 0., 0.);
						break;
					case TURN_LEFT :
						ardrone_at_set_progress_cmd(0, 0., 0., 0., -0.5);
						break;
					case TURN_RIGHT :
						ardrone_at_set_progress_cmd(0, 0., 0., 0., 0.5);
						break;
					default :
						ardrone_at_set_progress_cmd(0, 0., 0., 0., 0.);
						break;
				}
			}
		} else { // Sinon
			// Si l'utilisateur appuie sur d
			if (key == 'd') {
				// On décolle
				printf("Décollage !\n");
				ardrone_tool_set_ui_pad_start(1);
				flying = 1;
			} else if (key == 27) { // Si l'utilisateur appuie sur echap
				signal_exit();
				continuer = 0;
			}
		}

		usleep(80000);

	}

	return (THREAD_RET)0;

}
