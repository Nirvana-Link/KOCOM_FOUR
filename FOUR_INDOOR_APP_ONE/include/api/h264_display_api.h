#ifndef H264_DISPLAY_H
#define H264_DISPLAY_H
#include <stdbool.h>

typedef enum
{
	STREAM_STATE_OPEN,
	STREAM_STATE_PAUSE,
	STREAM_STATE_STOP
}STREAM_STATE;


bool h264_stream_open(char* url);

STREAM_STATE h264_stream_check(void);

bool h264_stream_pause(void);

bool h264_stream_play(char*url);

bool h264_stream_stop(void);

#endif
