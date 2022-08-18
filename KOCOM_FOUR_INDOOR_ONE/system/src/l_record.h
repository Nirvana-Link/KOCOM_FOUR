#ifndef L_RECORD_H
#define L_RECORD_H
#include"network_function_api.h"
#include"jpeg_encodec_api.h"
#include"record_video_api.h"
#include"ui_api.h"
#include"l_user_config.h"
#define REC_MODE_MANUAL  0X01
#define REC_MODE_AUTO	 0X02
#define REC_MODE_MOTION  0X03
#define REC_MODE_ALARM   0X04
bool is_record_video_ing(void);
bool record_video_start(char mode,char audio_from,enum source_index video_channel);
bool record_video_stop(char audio_flag);
bool is_record_pictrue_ing(void);
bool record_pictrue_start(char mode,enum source_index ch);
void push_picture_to_tuya_start(enum source_index channel);
#endif