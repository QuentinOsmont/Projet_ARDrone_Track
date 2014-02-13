#ifndef _IHM_STAGES_O_GTK_H
#define _IHM_STAGES_O_GTK_H

#include <config.h>
#include <VP_Api/vp_api_thread_helper.h>

// OpenCV Libraries

#include <cv.h>
#include <highgui.h>


PROTO_THREAD_ROUTINE(video_stage, data);

static vp_os_mutex_t  control_data_lock = PTHREAD_MUTEX_INITIALIZER; //To Exchange Control Data
static vp_os_mutex_t  control_video_lock = PTHREAD_MUTEX_INITIALIZER; // TO Exchange Video Stream

typedef struct control_data_t
{
    float roll;
    float pitch;
    float gaz;
    float yaw;
    int start;
}control_data_t; // Initiate control_data_lock to get the control Data

IplImage *global_video_feed; // Initiate control_video_lock to control this feed
int end_all_threads;

control_data_t control_data;



#endif // _IHM_STAGES_O_GTK_H
