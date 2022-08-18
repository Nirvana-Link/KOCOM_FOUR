#ifndef THUMB_DISPLAY_API_H
#define THUMB_DISPLAY_API_H
#include <stdbool.h>
#include "ui_api.h"

bool thumb_device_open(void);

bool thumb_data_load(const position* pos,const char* file,bool is_video);

bool thumb_device_close(void);



bool network_h264_thumb_device_open(void);

bool network_h264_thumb_data_load(const position* pos,const char* file,bool is_video);

bool network_h264_thumb_device_close(void);

#endif

