/**
 * @file main.c
 * @author pratik.borhade@eurecom.fr
 * @date 2009/07/01
 */
#include <ardrone_testing_tool.h>

//ARDroneLib
#include <ardrone_tool/ardrone_time.h>
#include <ardrone_tool/Navdata/ardrone_navdata_client.h>
#include <ardrone_tool/Control/ardrone_control.h>
#include <ardrone_tool/UI/ardrone_input.h>

//Common
#include <config.h>
#include <ardrone_api.h>

//VP_SDK
#include <ATcodec/ATcodec_api.h>
#include <VP_Os/vp_os_print.h>
#include <VP_Api/vp_api_thread_helper.h>
#include <VP_Os/vp_os_signal.h>

//Local project
#include <Video/video_stage.h>
#define Runtime // Calc/Runtime
#define COLOR2DETECT 175,220,160,0
#define Pix_Count_thresh 300
static int32_t exit_ihm_program = 1;


/* Implementing Custom methods for the main function of an ARDrone application */

/* The delegate object calls this method during initialization of an ARDrone application */

////Control System (Designed by Pratik) --Start

void MakeControlZero(control_data_t *l_control)
{
	l_control->roll = 0;
	l_control->pitch = 0;
	l_control->gaz = 0;
	l_control->yaw = 0;

}

control_data_t GradualChangeOfControl( control_data_t l_control,control_data_t prev_control,int grad ) //to change the control gradually
{
	control_data_t control_value;
	control_value.roll = prev_control.roll + (l_control.roll - prev_control.roll)/grad;
	control_value.pitch = prev_control.pitch + (l_control.pitch - prev_control.pitch)/(grad);
	control_value.gaz = prev_control.gaz + (l_control.gaz - prev_control.gaz)/grad;
	control_value.yaw = prev_control.yaw +(l_control.yaw - prev_control.yaw)/grad;
	control_value.start = prev_control.start;
	return(control_value);
}

float calcDist( CvScalar pix,CvScalar sclr)
{
    return( (abs(pix.val[0] - sclr.val[0]) + abs(pix.val[1] - sclr.val[1]) + abs(pix.val[2] - sclr.val[2])) );
}

float calcDistHSI( CvScalar pix,CvScalar sclr) //Calculate the Distance between two Scalars
{
    int x;
    x = pix.val[0]-sclr.val[0];
    if( abs(x) > 90 )
	x = 180-abs(x);
    return( ( abs(x)+abs(pix.val[1]-sclr.val[1])/3 ) );  // abs( h1-h2 ) + abs( (s1-s2)/3 ) -- More dependent on Colour which is in Hue Space 
}

int BackProject( IplImage *src,IplImage *dst, CvScalar sclr, int thresh )
{
    int i,j;
    CvScalar pix;
    for( i=0;i<src->width;i++ )
        for( j=0;j<src->height;j++ )
        {
		uchar *ptr_src,*ptr_dst;
                ptr_src = (uchar *) src->imageData + j * src->widthStep + i * src->nChannels;
                ptr_dst = (uchar *) dst->imageData + j * dst->widthStep + i * dst->nChannels;
                pix = cvScalar( ptr_src[0],ptr_src[1],ptr_src[2],0 );
                if( calcDist(pix,sclr)<thresh )
                    *ptr_dst = 255;
        }
}

// Connected Component Functions *************************** UnFinished
typedef struct pixel
{
	int x;
	int y;
}pixel;

int CalcConnectedComponents( IplImage *src, IplImage *dst, int window )
{
	cvZero( dst );
	int curr_component = 1; 
	int p,q;
	int i,j;
	int change = 1;

	while( change==1 )
	{
            change = 0;

            for( i=0;i<src->width;i++ )
                    for( j=0;j<src->height;j++ )
                    {
                            uchar *ptr_src,*ptr_dst;
                            ptr_src = (uchar *) src->imageData + j * src->widthStep + i * src->nChannels;
                            ptr_dst = (uchar *) dst->imageData + j * dst->widthStep + i * dst->nChannels;

                            if( *ptr_src < 127 )
                                    continue;

                            for( p=-window/2;p<window/2;p++ )
                            for( q=-window/2;q<window/2;q++ )
                            {
                                    uchar *ptr_curr;
                                    if( (i+p)<0 || (i+p)>src->width || (j+q)<0 || (j+q)>src->height )
                                            continue;
                                    if( p==0 && q==0 )
                                            continue;

                                    ptr_curr = (uchar *) dst->imageData + (j+q) * dst->widthStep + (i+p) * dst->nChannels;
                                    if( *ptr_curr < *ptr_dst && *ptr_curr != 0 )
                                    {
                                            *ptr_dst = *ptr_curr;
                                            change = 1;
                                    }
                                    else if( *ptr_dst==0 && *ptr_curr>0 )
                                    {
                                            *ptr_dst = *ptr_curr;
                                            change = 1;
                                    }
                            }
                            if( *ptr_dst == 0 )
                            {
                                    *ptr_dst = curr_component++;
                                    change = 1;
                            }
                    }
	}
	return(0);
}

int CalcLargestComponent( IplImage *src )
{

}

int GetTheLagerstComponent( IplImage *src, IplImage *dst )
{
	IplImage *temp;
	int largest_component;
	temp = cvCreateImage( cvGetSize(src),src->depth,src->nChannels );
	CalcConnectedComponents( src, temp, 5 );
	CalcLargestComponent( temp );
}

int RemoveAllComponentExcept( IplImage *src,IplImage *dst,int val )
{
    int i,j;
    for( i=0;i<src->width;i++ )
        for( j=0;j<src->height;j++ )
        {
            uchar *ptr_src,*ptr_dst;
            ptr_src = (uchar *) src->imageData + j * src->widthStep + i * src->nChannels;
            ptr_dst = (uchar *) dst->imageData + j * dst->widthStep + i * dst->nChannels;
            if( *ptr_src == val )
                *ptr_dst = 255;
            else
                *ptr_dst = 0;
        }

}

//**********************************************************
int BackProjectHSI( IplImage *src,IplImage *dst, CvScalar sclr, int thresh ) // Get a Image corresponding to Specific Image
{
    int i,j;
    CvScalar pix;
    IplImage *src_hsi;
    src_hsi = cvCreateImage( cvGetSize(src),src->depth,src->nChannels );
    cvCvtColor( src,src_hsi,CV_BGR2HSV );

    for( i=0;i<src->width;i++ )
        for( j=0;j<src->height;j++ )
        {
		uchar *ptr_src,*ptr_dst;
                ptr_src = (uchar *) src_hsi->imageData + j * src_hsi->widthStep + i * src_hsi->nChannels;
                ptr_dst = (uchar *) dst->imageData + j * dst->widthStep + i * dst->nChannels;
                pix = cvScalar( ptr_src[0],ptr_src[1],ptr_src[2],0 );
                if( calcDistHSI(pix,sclr)<thresh )
                    *ptr_dst = 255;
        }
}

typedef struct structure_COM
{
    int x;
    int y;
    long int n;
}structure_COM;

structure_COM calcCenterOfMass( IplImage *color_pos ) // Calculate Center of Mass
{
	int i,j;
	float x=0,y=0,n=0;
	structure_COM center;

	for( i=0;i<color_pos->width;i++ )
        	for( j=0;j<color_pos->height;j++ )
        	{
			uchar *ptr_src;
                	ptr_src = (uchar *) color_pos->imageData + j * color_pos->widthStep + i * color_pos->nChannels;
               		if( *ptr_src > 127 )
			{
                    		x = x+i;
				y = y+j;
				n++;
			}			
        	}
	center.x = (int) x/n;
	center.y = (int) y/n;
	center.n = n;
	return(center);
}

DEFINE_THREAD_ROUTINE( control_system, data )
{
    IplImage *current_frame, *color_pos, *src_hsi,*Connection;
    control_data_t l_control, prev_control;
    structure_COM CntrMass, Prev_COM;
    current_frame = cvCreateImage( cvSize(QVGA_WIDTH,QVGA_HEIGHT),8,3 );
    color_pos = cvCreateImage( cvSize(QVGA_WIDTH,QVGA_HEIGHT),8,1 );
    Connection = cvCreateImage(cvGetSize(color_pos),8,1);
    Prev_COM.n = -1;
  
    PRINT("STARTING CONTROL SYSTEM \n");

    while( end_all_threads==0 )
    {
    	vp_os_mutex_lock( &control_video_lock );
        if( global_video_feed==NULL )
        {
		vp_os_delay(20);
                vp_os_mutex_unlock( &control_video_lock );
        	continue;
        }
    	cvCopy( global_video_feed,current_frame,NULL );
    	vp_os_mutex_unlock( &control_video_lock );

    	vp_os_mutex_lock( &control_data_lock );
        	prev_control = control_data; 
    	vp_os_mutex_unlock( &control_data_lock ); 
   #ifdef Runtime	
        cvZero( color_pos );
	BackProjectHSI( current_frame,color_pos,cvScalar( COLOR2DETECT ), 20 ); // RED: 175,220,160,0 
	cvMorphologyEx( color_pos,color_pos,NULL,NULL,CV_MOP_OPEN,1 );
        
        CntrMass = calcCenterOfMass( color_pos );
        if( Prev_COM.n == -1 )
        {
                if( CntrMass.n > Pix_Count_thresh  )
                        Prev_COM = CntrMass;
        }
        else if( CntrMass.n>Pix_Count_thresh )
        {
            uchar *ptr, *ptr_src;
	    ptr_src = (uchar *)color_pos->imageData + Prev_COM.y * color_pos->widthStep + Prev_COM.x * color_pos->nChannels;
            if( *ptr_src > 127 )
            {
                  CalcConnectedComponents( color_pos, Connection, 5 );
            	  ptr = (uchar *)Connection->imageData + Prev_COM.y * Connection->widthStep + Prev_COM.x * Connection->nChannels;
             	  RemoveAllComponentExcept( Connection,color_pos,*ptr );
            	  CntrMass = calcCenterOfMass( color_pos );
	          Prev_COM = CntrMass;
            }
	    else
		  Prev_COM.n = -1;

            //CalcConnectedComponents( color_pos, Connection, 5 );
            //ptr = (uchar *)Connection->imageData + Prev_COM.y * Connection->widthStep + Prev_COM.x * Connection->nChannels;
            //RemoveAllComponentExcept( Connection,color_pos,*ptr );
            //CntrMass = calcCenterOfMass( color_pos );
	    //Prev_COM = CntrMass;
        }
	else
	    Prev_COM.n = -1;

	MakeControlZero( &l_control );
        if( CntrMass.n>Pix_Count_thresh )
        {
		CvMemStorage* storage = cvCreateMemStorage(0);
		float *p;

		cvCircle( current_frame, cvPoint(CntrMass.x,CntrMass.y), 5, CV_RGB(255,255,255), 1, 8, 0 );
		l_control.yaw = (float)(CntrMass.x-(QVGA_WIDTH/2))/(QVGA_WIDTH/2);
		l_control.gaz = -(float)(CntrMass.y-(QVGA_HEIGHT/2))/(QVGA_HEIGHT/2);
		if( CntrMass.n > 700 && CntrMass.n < 20000 )
		{
			if(CntrMass.n > 10000 && CntrMass.n<50000)
			{
				CntrMass.n = 10000;
				//l_control.pitch = -0.33;
			}
			//else
				//l_control.pitch = (float)(( (float)CntrMass.n - (float)5000 )/20000);
		}

		PRINT( "number of Pixels: %f\n\n",l_control.pitch );
		/*cvCanny( color_pos, color_pos, 10, 100, 3 );
		CvSeq* circles =cvHoughCircles(color_pos, storage, CV_HOUGH_GRADIENT, 1, 30, 100, 10, 5, 25 );
		if( circles->total > 0 )
		{
			p = (float *)cvGetSeqElem( circles, 0 );
			cvCircle( color_pos, cvPoint((int) p[0],(int) p[1]), (int) p[2], cvScalar(255,0,0,0), 1, 8, 0 );
			l_control.pitch = (  p[2]-15 )/20; 
			//PRINT( "Radius: %f\n\n",p[2] );
		}
		cvShowImage("Image", color_pos);
		cvWaitKey(20);*/
        }     
        //cvShowImage("Image", current_frame);
	//cvWaitKey(20);
        //cvShowImage( "BackProjection",color_pos );
        //cvWaitKey(20);
    #endif

    #ifdef Calc
    {
    	uchar *ptr_src;
        CvScalar current_sclr;
        src_hsi = cvCreateImage( cvGetSize(current_frame),current_frame->depth,3 );
        cvCvtColor( current_frame,src_hsi,CV_BGR2HSV );   // Red Ball:170,200,130
         //cvShowImage("Image", current_frame);
        //cvShowImage( "BackProjection",src_hsi );
        //cvWaitKey(30);
        ptr_src = (uchar *)src_hsi->imageData + (QVGA_HEIGHT/4) * current_frame->widthStep + (QVGA_WIDTH/4) * current_frame->nChannels;
        current_sclr = cvScalar( ptr_src[0],ptr_src[1],ptr_src[2],0  );
        PRINT( "Pix :- %d,%d,%d\n",ptr_src[0],ptr_src[1],ptr_src[2] );
        //if( calcDist( current_sclr,cvScalar(170,200,130,0) )<100  ) 
        //    PRINT("CENTER EQUAL\n\n");
            //l_control.yaw = -1;
       
    }
    #endif
    
    #ifdef Runtime
    	vp_os_mutex_lock( &control_data_lock );
		//control_data.yaw = -1;
        	control_data = GradualChangeOfControl( l_control,prev_control,20 ); 
		//PRINT( "YAW: %f\n\n",control_data.yaw );
    	vp_os_mutex_unlock( &control_data_lock );
    #endif

    	vp_os_delay(10);
    }

    cvReleaseImageHeader(&current_frame);
    cvReleaseImage(&Connection);
    return C_OK;
}

DEFINE_THREAD_ROUTINE( comm_control, data )
{
    int seq_no = 0,prev_start = 0;
    control_data_t l_control;

    PRINT( "Initilizing Thread 1\n" );
    while( end_all_threads == 0 )
    {
    vp_os_delay(10);
    //PRINT("E:%d,S:%d\n\n",end_all_threads,control_data.start );
    vp_os_mutex_lock( &control_data_lock ); //Taking Control Mutex
    	l_control = control_data;
    vp_os_mutex_unlock( &control_data_lock );

    if( prev_start == 0 && l_control.start == 1 ) //To Start Drone
    {
        ardrone_tool_set_ui_pad_start( 1 );
        prev_start = 1;
    }
    else if( prev_start == 1 && l_control.start == 0 ) //To Stop the Drone
    {
        ardrone_tool_set_ui_pad_start( 0 );
        prev_start = 0;
    }
    //PRINT("YAW : %f\n\n",l_control.yaw);
    ardrone_at_set_progress_cmd( ++seq_no,l_control.roll,l_control.pitch,l_control.gaz,l_control.yaw); //command to make drone move.
    //ardrone_at_set_progress_cmd( ++seq_no,0,0,0,-1);
    }
    PRINT( "Communication Thread Ending\n" );
    return C_OK;
}

////Control System (Designed by Pratik) --END

C_RESULT ardrone_tool_init_custom(int argc, char **argv)
{
  /* Registering for a new device of game controller */

  /* Start all threads of your application */
  //control_data = (control_data_t *) malloc (sizeof(control_data_t);
  end_all_threads = 0;
  global_video_feed = NULL;
  START_THREAD( video_stage, NULL );
  START_THREAD( ardrone_control, NULL );
  START_THREAD( comm_control, NULL );
  START_THREAD( control_system, NULL );
  
  return C_OK;
}

/* The delegate object calls this method when the event loop exit */
C_RESULT ardrone_tool_shutdown_custom()  //Initiation Function
{
  /* Relinquish all threads of your application */
  JOIN_THREAD( video_stage );
  JOIN_THREAD( comm_control );
  JOIN_THREAD( control_system );
  JOIN_THREAD( ardrone_control );
  ardrone_tool_set_ui_pad_start(0);

  /* Unregistering for the current device */

  return C_OK;
}

/* The event loop calls this method for the exit condition */
bool_t ardrone_tool_exit()
{
  return exit_ihm_program == 0;
}

C_RESULT signal_exit()
{
  exit_ihm_program = 0;

  return C_OK;
}

/* Implementing thread table in which you add routines of your application and those provided by the SDK */
BEGIN_THREAD_TABLE
  THREAD_TABLE_ENTRY( ardrone_control, 20 )
  THREAD_TABLE_ENTRY( navdata_update, 20 )
  THREAD_TABLE_ENTRY( video_stage, 20 )
  THREAD_TABLE_ENTRY( comm_control, 20 )
  THREAD_TABLE_ENTRY( control_system, 20 )
END_THREAD_TABLE

