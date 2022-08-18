#include"l_audio_play.h"
#include "audio_play_api.h"
#include "rom.h"
#include"l_user_config.h"
#include"layout_internal_button.h"
#include"layout_cam_button.h"
#include"network_function_api.h"
#include"l_network.h"
#include"gpio_api.h"
#include"debug.h"

static int prev_volume = 0;
static int prev_gain = 0;

#define AMP_ENABLE_GPIO 37

void amp_enable(bool enable) {
    if (enable) {
        gpio_set(AMP_ENABLE_GPIO, GPIO_LEVEL_HIGH);
    } else {
        gpio_set(AMP_ENABLE_GPIO, GPIO_LEVEL_LOW);
    }
}

void update_prev_value(int volume, int gain){
    prev_volume = volume;
    prev_gain = gain;
}

void l_audio_play_init(void)
{
   audio_info audio_group_info[9] =
	{
		{ROM_R_B_RINGS_KEY1_PCM,ROM_R_B_RINGS_KEY1_PCM_SIZE, AUDIO_CHANNEL_MONO,AK_AUDIO_SAMPLE_RATE_8000, AK_AUDIO_TYPE_PCM},
        {ROM_R_B_RINGS_MELODY1_MP3,ROM_R_B_RINGS_MELODY1_MP3_SIZE, AUDIO_CHANNEL_MONO,	 AK_AUDIO_SAMPLE_RATE_16000, AK_AUDIO_TYPE_MP3},
        {ROM_R_B_RINGS_MELODY2_MP3,ROM_R_B_RINGS_MELODY2_MP3_SIZE, AUDIO_CHANNEL_MONO,	 AK_AUDIO_SAMPLE_RATE_16000, AK_AUDIO_TYPE_MP3},
        {ROM_R_B_RINGS_MELODY3_MP3,ROM_R_B_RINGS_MELODY3_MP3_SIZE, AUDIO_CHANNEL_MONO,	 AK_AUDIO_SAMPLE_RATE_16000, AK_AUDIO_TYPE_MP3},
        {ROM_R_B_RINGS_MELODY4_MP3,ROM_R_B_RINGS_MELODY4_MP3_SIZE, AUDIO_CHANNEL_MONO,	 AK_AUDIO_SAMPLE_RATE_16000, AK_AUDIO_TYPE_MP3},
        {ROM_R_B_RINGS_MELODY5_MP3,ROM_R_B_RINGS_MELODY5_MP3_SIZE, AUDIO_CHANNEL_MONO,	 AK_AUDIO_SAMPLE_RATE_16000, AK_AUDIO_TYPE_MP3},
        {ROM_R_B_RINGS_MELODY6_MP3,ROM_R_B_RINGS_MELODY6_MP3_SIZE, AUDIO_CHANNEL_MONO,	 AK_AUDIO_SAMPLE_RATE_16000, AK_AUDIO_TYPE_MP3},
        {ROM_R_B_RINGS_NO_ACK_MP3,ROM_R_B_RINGS_NO_ACK_MP3_SIZE, AUDIO_CHANNEL_MONO,	 AK_AUDIO_SAMPLE_RATE_16000, AK_AUDIO_TYPE_MP3},
        {ROM_R_B_RINGS_ALARM_MP3,ROM_R_B_RINGS_ALARM_MP3_SIZE, AUDIO_CHANNEL_MONO,	 AK_AUDIO_SAMPLE_RATE_16000, AK_AUDIO_TYPE_MP3}
    };
    audio_play_init(audio_group_info,9);
}

void audio_finish_callback(void)
{
    if(audio_stream_status_get() == false && internal_udp_status_get() == false)
    {
        amp_enable(false);
        // DEBUG_LOG("audio_finish_callback ================>%lld\n\r",os_get_ms()); 
    }

}
void audio_start_callback(void)
{
    amp_enable(true);
    // DEBUG_LOG("audio_start_callback ================> %lld\n\r",os_get_ms());
}

void touch_sound_play(/* const void* arg */int arg){

    if(arg == RING_MAX)
        return;

    // DEBUG_LOG("touch_sound_play====================+> :%d\n",arg);
    audio_play(arg,70,2,audio_start_callback,audio_finish_callback);

}

bool intercom_sound_play(void) {
    int volume = user_data_get()->SOUR_PRO[DEV].sound.call_vol*TONE_VOLUME_INTERVAL+TONE_MIN_VOLUME;
    int gain = user_data_get()->SOUR_PRO[DEV].sound.speak_sen;
    if(!is_audio_play_ing()){   
        update_prev_value(volume,gain);
        audio_play(user_data_get()->SOUR_PRO[DEV].sound.call_mel, volume, gain, audio_start_callback,audio_finish_callback);
    }
    return true;
}

bool door1_call_sound_play(void)
{
    int vol = user_data_get()->SOUR_PRO[CAM1].sound.call_vol*TONE_VOLUME_INTERVAL+TONE_MIN_VOLUME;//实际音量大小
    int gain = user_data_get()->SOUR_PRO[CAM1].sound.speak_sen;
    if(!is_audio_play_ing()){
        update_prev_value(vol,gain);
        audio_play(user_data_get()->SOUR_PRO[CAM1].sound.call_mel,vol,gain,audio_start_callback,audio_finish_callback);        
    }
	return true;
}

bool door2_call_sound_play(void)
{
    int vol = user_data_get()->SOUR_PRO[CAM2].sound.call_vol*TONE_VOLUME_INTERVAL+TONE_MIN_VOLUME ;//实际音量大小
    int gain = user_data_get()->SOUR_PRO[CAM2].sound.speak_sen;
    if(!is_audio_play_ing()){
        update_prev_value(vol,gain);
        audio_play(user_data_get()->SOUR_PRO[CAM2].sound.call_mel,vol,gain,audio_start_callback,audio_finish_callback);
    }
	return true;
}


void sound_microphone_sensitivity_set(int sensitivity,enum source_index sour){
    if(sour == DEV)
    {
        const int volume[3] = {-20,-10,0};
        audio_input_volume_set(volume[sensitivity],3);        
    }
    else
    {
        const int volume[3] = {-20,-10,0};
        audio_input_volume_set(volume[sensitivity],3);      
    }

}


bool sound_volume_set(int volume, int gain,enum source_index sour) {

    int tmp_volume;
    if(sour == DEV)
    {
        tmp_volume = TALK_MIN_VOLUME_INTERVAL + volume * TALK_VOLUME_INTERVAL - 80;
    }
    else
    {
        tmp_volume = TALK_MIN_VOLUME_INTERVAL + volume * TALK_VOLUME_INTERVAL - 80;
    }
    int tmp_gain = gain;

    if(prev_volume != volume || prev_gain != gain){
        update_prev_value(tmp_volume,tmp_gain);

        return audio_output_volume_set(tmp_volume, tmp_gain);
    }
   return false;
}


void init_talk_sound(enum source_index sour){
    int volume = user_data_get()->SOUR_PRO[sour].sound.talk_vol;
    int gain = user_data_get()->SOUR_PRO[sour].sound.speak_sen + (sour == DEV ? 0 : 1);
    // if(prev_volume != volume || prev_gain != gain){
        // update_prev_value(volume,gain);
        sound_volume_set(volume,gain,sour);
        // DEBUG_LOG("INIT_TALK_SOUND-SOUR:%d        VOL--------------------->%d\n\r",sour,user_data_get()->SOUR_PRO[sour].sound.talk_vol);
        sound_microphone_sensitivity_set(user_data_get()->SOUR_PRO[sour].sound.mic_sen,sour);
    // }
}
