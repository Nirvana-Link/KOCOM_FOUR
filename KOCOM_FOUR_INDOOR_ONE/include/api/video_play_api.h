#ifndef _VIDOE_PLAY_API_H_
#define _VIDOE_PLAY_API_H_
#include "stdbool.h"

typedef enum
{
	VIDEO_PLAY_PAUSE,
	VIDEO_PLAY_STOP,
	VIDEO_PLAY_ING
}VIDEO_PLAY_STATE;



bool video_play_start(const char* file,bool is_pause);

bool video_duration_get(int*cur,int*total);

bool video_play_stop(void);


VIDEO_PLAY_STATE video_play_state_get(void);

void video_play_state_set(VIDEO_PLAY_STATE state);
#endif


