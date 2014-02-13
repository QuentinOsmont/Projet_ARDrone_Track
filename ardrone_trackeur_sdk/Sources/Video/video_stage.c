/*
 * @video_stage.c
 * @author marc-olivier.dzeukou@parrot.com
 * @date 2007/07/27
 *
 * ihm vision thread implementation
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <sys/time.h>
#include <time.h>

#include <VP_Api/vp_api.h>
#include <VP_Api/vp_api_error.h>
#include <VP_Api/vp_api_stage.h>
#include <VP_Api/vp_api_picture.h>
#include <VP_Stages/vp_stages_io_file.h>
#include <VP_Stages/vp_stages_i_camif.h>

#include <config.h>
#include <VP_Os/vp_os_print.h>
#include <VP_Os/vp_os_malloc.h>
#include <VP_Os/vp_os_delay.h>
#include <VP_Stages/vp_stages_yuv2rgb.h>
#include <VP_Stages/vp_stages_buffer_to_picture.h>
#include <VLIB/Stages/vlib_stage_decode.h>

#include <ardrone_tool/ardrone_tool.h>
#include <ardrone_tool/Com/config_com.h>

#ifndef RECORD_VIDEO
#define RECORD_VIDEO
#endif
#ifdef RECORD_VIDEO
#    include <ardrone_tool/Video/video_stage_recorder.h>
#endif

#include <ardrone_tool/Video/video_com_stage.h>

#include "Video/video_stage.h"
#include "global.h"

//OpenCV
#include "cv.h"
#include "highgui.h"

#define NB_STAGES 10

#define WIDTH 320
#define HEIGHT 240


/* Variables globales */
PIPELINE_HANDLE pipeline_handle;

static vp_os_mutex_t  video_update_lock = PTHREAD_MUTEX_INITIALIZER;

// Couleur à tracker Hue Saturation Value
int h = 0, s = 0, v = 0;
// pourcentage d'approximation pour la distance
double tolerance_distance = 0.95;
// sert pour l'évaluation de la distance : la première fois mémorise la taille de la cible, les fois suivante on compare avec la taille mémorisée
int first_time = 1;
// Taille du contour de la forme qu'on tracke
double size;
// Compteur pour savoir combien de fois on a eu une mauvaise taille du contour
// (et éviter de déplacer le drone alors qu'il y a juste eu un ratage du tracking sur une image)
int cpt_distance = 0;

// Images
// Image légèrement floutée pour trouver la couleur à tracker
IplImage * image;
// Image au format HSV
IplImage * hsv;
// Image du drone : à afficher
IplImage * img_small;
// Centre de la cible
CvPoint position;

// Fenêtres
char * w_camera = "Image camera";
char * w_binary = "Image binaire";
//char * w_contours = "Contours de la cible";

/* Fin des variables */


/*
	Pour trouver le plus grand contour que l'on a détecté
	Donc le plus gros objet détecté
*/
CvSeq* findLargestContour (CvSeq* contours, CvSeq* first) {
	CvSeq * result = first;
	double current_area = 0., largest_area = 0.;
	int i = 0;

	/*
		Calcul du max
	*/
	for(contours = first; contours != NULL; contours = contours->h_next) {
		current_area = cvContourArea(contours, CV_WHOLE_SEQ,0);
		if (current_area < 0.) { // On prend la valeur absolue car il se peut que cvContourArea renvoie une valeur négative
			current_area *= -1.;
		}
		if (current_area > largest_area) {
			largest_area = current_area;
			result = contours;
		}
		i++;
	}
	/*
		PAS ENCORE AU POINT
		On compare la taille du plus grand contour actuel avec celle du premier que l'on a détecté
		pour voir si l'objet s'est rapproché ou éloigné 
		Mais comme ça ne fonctionne pas très bien la première fois on attend un peu avant de l'utiliser
		(first_time vaut 5 au début et on ne mémorise la taille de l'objet que lorsqu'il vaudra 1, donc la 4ème fois)
		On utilise une tolérance pour la différence de distance : 43-195% (définie empiriquement)
		Pour éviter qu'il ne dise trop souvent que la cible est trop proche ou trop éloignée, on utilise un compteur
		on n'indique un problème de distance qu'au bout de ka 5ème fois
		(compteur à 5 si la cible est trop proche, -5 si elle est éloignée)
	*/
	if (first_time == 1) {
		first_time = 0;
		size = largest_area;
		printf("Premier contour : %f\n", size);
	}else {
		if (largest_area > (1.+tolerance_distance)*size) {
			if (cpt_distance >= 0) {
				cpt_distance++;
				if (cpt_distance >= 5) {
					printf("Cible trop proche : %f\n", largest_area);
					cpt_distance = 0;
					deplacement_a_faire = BACKWARD;
				}
			} else {
				cpt_distance = 0;
			}
		} else if ((largest_area < (1.-(3.*tolerance_distance/5.))*size) && (largest_area > 0.)) {
			if (cpt_distance <= 0) {
				cpt_distance--;
				if (cpt_distance <= -5) {
					printf("Cible trop eloignee : %f\n", largest_area);
					cpt_distance = 0;
					deplacement_a_faire = FORWARD;
				}
			} else {
				cpt_distance = 0;
			}
		} else {
			deplacement_a_faire = HOVER;
		}
	}
	return result;
}

/*
	Fonction qui va tracker la couleur choisie, afficher le contour de la cible
	Renvoie le centre de la cible sous forme de CvPoint
*/
CvPoint tracking(IplImage * hsv) {
	IplImage * img_bin = cvCreateImage(cvSize(image->width, image->height), 8, 1);
	IplImage * img_bin_c;
	IplConvKernel *kernel;
	int Nc;
	CvSeq* c;
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* first_contour = NULL;
	CvSeq* largest_contour;
	CvMoments moments; 
	double M00, M01, M10;
	CvPoint center = cvPoint(-1, -1);

	/* 
		On filtre l'image pour garder ne garder que notre couleur (avec une tolérance plus ou moins empirique)
		L'image résultat est une image noire avec seulement la couleur ciblée en blanc
	*/
	cvInRangeS(hsv, cvScalar(h - 10, s - 85, v - 35, 0), cvScalar(h + 10, s + 85, v + 35, 0), img_bin);


	/*
		On va faire une dilatation et une érosion pour améliorer la détection et enlever les points parasites
	*/
	// Create kernels for the morphological operation
	kernel = cvCreateStructuringElementEx(7, 7, 2, 2, CV_SHAPE_ELLIPSE, 0);
 
	// Morphological opening (inverse because we have white pixels on black background)
	cvDilate(img_bin, img_bin, kernel, 2);
	cvErode(img_bin, img_bin, kernel, 2);


	img_bin_c = cvCloneImage(img_bin);
	/*cvNot(img_bin, img_bin_c);
	img_bin = cvCloneImage(img_bin_c);*/

	/*
		On cherche les contours de notre cible
	*/
	Nc = cvFindContours(img_bin_c, storage, &first_contour, sizeof(CvContour), CV_RETR_EXTERNAL, 2, cvPoint(0,0));
	
	// On trouve le plus grand contour
	largest_contour = findLargestContour(c, first_contour);

	/*
		S'il y a un plus grand contour on l'affiche
		et on calcule les moments pour trouver son centre
	*/
	if (largest_contour != NULL) {
		//for (c=first_contour; c!=NULL; c=c->h_next) {
		cvDrawContours(img_small, largest_contour, cvScalar(0, 0, 255, 0), cvScalar(255, 0, 0, 0), 1, 2, 8, cvPoint(0, 0));
		//}
		cvMoments(largest_contour,&moments, 0); 
		M00 = cvGetSpatialMoment(&moments,0,0); 
		M10 = cvGetSpatialMoment(&moments,1,0); 
		M01 = cvGetSpatialMoment(&moments,0,1); 
		center.x = (int)(M10/M00); 
		center.y = (int)(M01/M00);
	}

	//cvShowImage( w_contours, img_bin_c );

	cvShowImage(w_binary, img_bin);

	cvReleaseStructuringElement(&kernel);
	cvReleaseImage(&img_bin);
	cvReleaseImage(&img_bin_c);
	cvReleaseMemStorage(&storage);

	return center;
}

/*
	Source : GeckoGeek
		geckogeek.fr/tutorial-opencv-isoler-et-traquer-une-couleur.html
*/

/*
 * Get the color of the pixel where the mouse has clicked
 * We put this color as model color (the color we want to tracked)
 */
void getObjectColor(int event, int x, int y, int flags, void *param) {
 
	// Vars
	CvScalar pixel;
	IplImage *hsv;
	if (mode == MANUEL) {
		if(event == CV_EVENT_LBUTTONUP)	{

			// Get the hsv image
			hsv = cvCloneImage(image);
			cvCvtColor(image, hsv, CV_BGR2HSV);
 
			// Get the selected pixel
			pixel = cvGet2D(hsv, y, x);
 
			// Change the value of the tracked color with the color of the selected pixel
			h = (int)pixel.val[0];
			s = (int)pixel.val[1];
			v = (int)pixel.val[2];
 
			// Release the memory of the hsv image
    	cvReleaseImage(&hsv);

			mode = TRACKING;
			//printf("H : %d, S : %d, V : %d\n", h, s, v);
		}
	}
}


/*
	Fonction pour transformer l'image reçue en image OpenCV
	Source : GAUTH
		gauth.fr/2011/09/use-opencv-with-the-ar-drone-sdk
	(modification par rapport à l'originale, dst est en static et initialisé
	 1 seule fois, sinon utilisation croissante de mémoire)
*/
IplImage *ipl_image_from_data(uint8_t* data, int reduced_image)
{
  IplImage *currframe;
  static IplImage *dst;
  static int first = 1;
 
  if (!reduced_image)
  {
    currframe = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 3);
    if (first) {
	    dst = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 3);
	    first = 0;
    }
  }
  else
  {
    currframe = cvCreateImage(cvSize(176, 144), IPL_DEPTH_8U, 3);
    if (first) {
	    dst = cvCreateImage(cvSize(176,144), IPL_DEPTH_8U, 3);
	    first = 0;
	  }
    currframe->widthStep = 320*3;
  }
 
  currframe->imageData = data;
  cvCvtColor(currframe, dst, CV_BGR2RGB);
  cvReleaseImage(&currframe);
  return dst;
}

/*
	Initialisation
*/
C_RESULT output_gtk_stage_open( void *cfg, vp_api_io_data_t *in, vp_api_io_data_t *out)
{
	img_small = cvCreateImage(cvSize(320, 240), 8, 3);
	image = cvCreateImage(cvSize(320, 240), 8, 3);
	hsv = cvCloneImage(image);

	// Fenêtres
	cvNamedWindow(w_camera, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(w_camera, 100, 0);
	cvNamedWindow(w_binary, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(w_binary, 450, 0);
	//cvNamedWindow(w_contours, CV_WINDOW_AUTOSIZE);
	//cvMoveWindow(w_contours, 800, 0);

	// Mouse event to select the tracked color on the original image
	cvSetMouseCallback(w_camera, getObjectColor, NULL);

  return (SUCCESS);
}

/*
	Fonction appelée lorsqu'on a une image
*/
C_RESULT output_gtk_stage_transform( void *cfg, vp_api_io_data_t *in, vp_api_io_data_t *out)
{

  vp_os_mutex_lock(&video_update_lock);
  // On récupère l'image
  img_small = ipl_image_from_data((uint8_t*)in->buffers[0], 0);
  vp_os_mutex_unlock(&video_update_lock);

	// On lui applique un flou pour éliminer du bruit
	cvSmooth(img_small, image, CV_GAUSSIAN, 7, 7, 0., 0.);
	// On la convertit en HSV
	cvCvtColor(image, hsv, CV_BGR2HSV);

	// Si on est prêt on tracke
	if (mode == TRACKING) {
		// On récupère la position de la cible
		position = tracking(hsv);
		
		// On dessine les lignes de limites du centre
		cvLine(img_small, cvPoint(1*(img_small->width/3), 0), cvPoint(1*(img_small->width/3), img_small->height), cvScalar(255, 0, 0, 0), 1, 8, 0);
		cvLine(img_small, cvPoint(2*(img_small->width/3), 0), cvPoint(2*(img_small->width/3), img_small->height), cvScalar(255, 0, 0, 0), 1, 8, 0);
		cvLine(img_small, cvPoint(0, 1*(img_small->height/3)), cvPoint(img_small->width, 1*(img_small->height/3)), cvScalar(255, 0, 0, 0), 1, 8, 0);
		cvLine(img_small, cvPoint(0, 2*(img_small->height/3)), cvPoint(img_small->width, 2*(img_small->height/3)), cvScalar(255, 0, 0, 0), 1, 8, 0);
		
		// Une fois qu'on a récupéré la position du centre de la cible on regarde l'alignement
		if (position.x != -1 && position.y != -1) {
			cvDrawCircle(img_small, position, 10, cvScalar(0, 255, 0, 0), -1, 8, 0);
			if (position.y < 1*(image->height/3)) { // on est en bas
				//printf("Cible plus haut\n");
				deplacement_a_faire = UP;
			} else if (position.y > 2*(image->height/3)) { // on est en haut
				//printf("Cible plus bas\n");
				deplacement_a_faire = DOWN;
			} else if (position.x < 1*(image->width/3)) { // on est à droite
				//printf("Cible a gauche\n");
				deplacement_a_faire = TURN_LEFT;
			} else if (position.x > 2*(image->width/3)) { // on est à gauche
				//printf("Cible a droite\n");
				deplacement_a_faire = TURN_RIGHT;
			} else if (deplacement_a_faire != FORWARD && deplacement_a_faire != BACKWARD) {
				deplacement_a_faire = HOVER;
			}

		}
	} else {
		// Si on ne tracke pas on veut être sûr de rester en vol stationnaire et pas se déplacer
		//deplacement_a_faire = HOVER;
		first_time = 1;
	}

	// On affiche l'image
	cvShowImage(w_camera, img_small);

	// On attend 10ms que l'utilisateur appuie sur une touche
	// (nécessaire sinon les images ne s'affichent pas)
	// Doit être fait ici
	key = cvWaitKey(30);

  return (SUCCESS);
}

/*
	On libère les images et on ferme les fenêtres
*/
C_RESULT output_gtk_stage_close( void *cfg, vp_api_io_data_t *in, vp_api_io_data_t *out)
{
	cvReleaseImage(&img_small);
	cvReleaseImage(&hsv);
	cvReleaseImage(&image);
	cvDestroyAllWindows();
  return (SUCCESS);
}


const vp_api_stage_funcs_t vp_stages_output_gtk_funcs =
{
  NULL,
  (vp_api_stage_open_t)output_gtk_stage_open,
  (vp_api_stage_transform_t)output_gtk_stage_transform,
  (vp_api_stage_close_t)output_gtk_stage_close
};

DEFINE_THREAD_ROUTINE(video_stage, data)
{
  C_RESULT res;

  vp_api_io_pipeline_t    pipeline;
  vp_api_io_data_t        out;
  vp_api_io_stage_t       stages[NB_STAGES];

  vp_api_picture_t picture;

  video_com_config_t              icc;
  vlib_stage_decoding_config_t    vec;
  vp_stages_yuv2rgb_config_t      yuv2rgbconf;
#ifdef RECORD_VIDEO
  video_stage_recorder_config_t   vrc;
#endif
  /// Picture configuration
  picture.format        = PIX_FMT_YUV420P;

  picture.width         = QVGA_WIDTH;
  picture.height        = QVGA_HEIGHT;
  picture.framerate     = 30;

  picture.y_buf   = vp_os_malloc( QVGA_WIDTH * QVGA_HEIGHT     );
  picture.cr_buf  = vp_os_malloc( QVGA_WIDTH * QVGA_HEIGHT / 4 );
  picture.cb_buf  = vp_os_malloc( QVGA_WIDTH * QVGA_HEIGHT / 4 );

  picture.y_line_size   = QVGA_WIDTH;
  picture.cb_line_size  = QVGA_WIDTH / 2;
  picture.cr_line_size  = QVGA_WIDTH / 2;

  vp_os_memset(&icc,          0, sizeof( icc ));
  vp_os_memset(&vec,          0, sizeof( vec ));
  vp_os_memset(&yuv2rgbconf,  0, sizeof( yuv2rgbconf ));

  icc.com                 = COM_VIDEO();
  icc.buffer_size         = 100000;
  icc.protocol            = VP_COM_UDP;
  COM_CONFIG_SOCKET_VIDEO(&icc.socket, VP_COM_CLIENT, VIDEO_PORT, wifi_ardrone_ip);

  vec.width               = QVGA_WIDTH;
  vec.height              = QVGA_HEIGHT;
  vec.picture             = &picture;
  vec.block_mode_enable   = TRUE;
  vec.luma_only           = FALSE;

  yuv2rgbconf.rgb_format = VP_STAGES_RGB_FORMAT_RGB24;
#ifdef RECORD_VIDEO
  vrc.fp = NULL;
#endif

  pipeline.nb_stages = 0;

  stages[pipeline.nb_stages].type    = VP_API_INPUT_SOCKET;
  stages[pipeline.nb_stages].cfg     = (void *)&icc;
  stages[pipeline.nb_stages].funcs   = video_com_funcs;

  pipeline.nb_stages++;

#ifdef RECORD_VIDEO
  stages[pipeline.nb_stages].type    = VP_API_FILTER_DECODER;
  stages[pipeline.nb_stages].cfg     = (void*)&vrc;
  stages[pipeline.nb_stages].funcs   = video_recorder_funcs;

  pipeline.nb_stages++;
#endif // RECORD_VIDEO
  stages[pipeline.nb_stages].type    = VP_API_FILTER_DECODER;
  stages[pipeline.nb_stages].cfg     = (void*)&vec;
  stages[pipeline.nb_stages].funcs   = vlib_decoding_funcs;

  pipeline.nb_stages++;

  stages[pipeline.nb_stages].type    = VP_API_FILTER_YUV2RGB;
  stages[pipeline.nb_stages].cfg     = (void*)&yuv2rgbconf;
  stages[pipeline.nb_stages].funcs   = vp_stages_yuv2rgb_funcs;

  pipeline.nb_stages++;

  stages[pipeline.nb_stages].type    = VP_API_OUTPUT_SDL;
  stages[pipeline.nb_stages].cfg     = NULL;
  stages[pipeline.nb_stages].funcs   = vp_stages_output_gtk_funcs;

  pipeline.nb_stages++;

  pipeline.stages = &stages[0];
 
  /* Processing of a pipeline */
  if( !ardrone_tool_exit() )
  {
    PRINT("\n   Video stage thread initialisation\n\n");

    res = vp_api_open(&pipeline, &pipeline_handle);

    if( SUCCEED(res) )
    {
      int loop = SUCCESS;
      out.status = VP_API_STATUS_PROCESSING;

      while( !ardrone_tool_exit() && (loop == SUCCESS) )
      {
          if( SUCCEED(vp_api_run(&pipeline, &out)) ) {
            if( (out.status == VP_API_STATUS_PROCESSING || out.status == VP_API_STATUS_STILL_RUNNING) ) {
              loop = SUCCESS;
            }
          }
          else loop = -1; // Finish this thread
      }

      vp_api_close(&pipeline, &pipeline_handle);
    }
  }

  PRINT("   Video stage thread ended\n\n");

  return (THREAD_RET)0;
}

