This is just to give brief Idea of the code..

ARDrone SDK has its own main loop:
C_RESULT ardrone_tool_init_custom(int argc, char **argv) --Initiation Funtion
C_RESULT ardrone_tool_shutdown_custom() --Ending Function

main changes are following files:
Video/video_stages.c
Video/video_stages.h
ardrone_testing_tool.c

Function named:
C_RESULT output_gtk_stage_transform( void *cfg, vp_api_io_data_t *in, vp_api_io_data_t *out)
is to get the image in OpenCV format.
It also consist some code to take the input from Keyboard and store in global variable.
The global variable structure is stored in video_stages.h

DEFINE_THREAD_ROUTINE(thread1, data):
It starts a new thread and sends control signal using these global variables.
