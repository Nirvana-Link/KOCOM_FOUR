#ifndef AUDIO_PLAY_API_H
#define AUDIO_PLAY_API_H
#include "ak_ao.h"
#include <stdbool.h>

typedef struct
{
	long addres;
	int size;
	enum ak_audio_channel_type ch;
	enum ak_audio_sample_rate rate;
	enum ak_audio_type type;
}audio_info;

bool audio_play_init(audio_info* pinfo,int num);

void audio_play(int index,int vol,int speaker_gain,void(*start_callback)(void),void(*finish_callback)(void));

bool is_audio_play_ing(void);

int audio_playing_index(void);

void audio_play_stop(void);

bool audio_output_restart(void);

bool audio_volume_get(void);

bool audio_output_volume_set(int vol,int gain);

bool audio_ao_volume_set(int vol,int gain);

bool audio_input_volume_set(int volume,int gain);

bool get_input_open_status(void);

bool audio_input_volume_get(void);

void motion_audio_set_volume(int vol,int bell,int speak,bool play_audio);

void brightness_get_vol(int brightness);

bool motion_handle_thread_open(void (*brightness)(int));

bool motion_handle_thread_close(void);

bool get_audio_decode_status(void);
#endif

