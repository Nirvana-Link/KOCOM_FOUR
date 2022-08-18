#ifndef AUDIO_RAW_API_H
#define AUDIO_RAW_API_H
#include <stdbool.h>
#include "ak_ao.h"

bool audio_raw_record_start(const char* file,enum ak_audio_sample_rate rate,enum ak_audio_channel_type ch);

bool audio_raw_record_stop(void);

bool audio_raw_record_check(void);


bool audio_raw_play_start(const char* file,enum ak_audio_sample_rate rate,enum ak_audio_channel_type ch);

bool audio_raw_play_stop(void);

bool audio_raw_play_check(void);

#endif

