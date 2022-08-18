#ifndef RECORD_VIDEO_API_H
#define RECORD_VIDEO_API_H

#include <stdbool.h>

bool record_video_open(bool has_audio,const char* file,int arg);

bool record_video_quit(void);

bool record_video_duration_get(int* duration);

#endif

