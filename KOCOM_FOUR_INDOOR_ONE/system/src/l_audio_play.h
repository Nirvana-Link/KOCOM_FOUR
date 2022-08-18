#ifndef L_AUDIO_PLAY_H
#define L_AUDIO_PLAY_H
#include <stdbool.h>
#include"l_user_config.h"

#define TONE_MIN_VOLUME          61
#define TONE_VOLUME_INTERVAL          4
#define TALK_MIN_VOLUME_INTERVAL          77
#define TALK_MIN_VOLUME_CAMERA          80
#define TALK_VOLUME_INTERVAL          3
#define TALK_VOLUME_CAMERA          3

enum ring_index {
    KEY_1 = 0X00,
    MELODY_1,
    MELODY_2,
    MELODY_3,
    MELODY_4,
    MELODY_5,
    MELODY_6,
    KEY_2,
    ALARM_1,
    RING_MAX
};

void l_audio_play_init(void);
void amp_enable(bool enable);
void touch_sound_play(int arg);
bool door1_call_sound_play(void);
bool door2_call_sound_play(void);

void audio_start_callback(void);
void audio_finish_callback(void);

bool sound_volume_set(int volume, int gain,enum source_index sour);

void init_talk_sound(enum source_index);

#endif