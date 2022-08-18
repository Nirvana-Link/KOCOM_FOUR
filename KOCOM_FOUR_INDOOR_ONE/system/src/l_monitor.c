#include"l_monitor.h"
#include"layout_base.h"
#include"tuya_sdk_api.h"
#include"l_user_config.h"
#include"l_network.h"
#include"debug.h"

static bool monitor_mode_doorbell_flag = false;

void monitor_doorbell(bool enable){
	monitor_mode_doorbell_flag = enable;
}

bool monitor_doorbell_status(void){
	return monitor_mode_doorbell_flag;
}

static char monitor_enter_flag = MONITOR_ENTER_NONE;
static bool monitor_open_flag = false;
BUY_CAMERA CAMERA_STATUS = {{false,false}};
// static bool monitor_open_flag = false;

char monitor_enter_flag_get(void){
    return monitor_enter_flag;
}

void monitor_enter_flag_set(char flag){
    monitor_enter_flag = flag;
}

bool monitor_channel_set(enum source_index ch)
{

	if(ch != DEV && ch < SOURCE_TOTAL && device_list_ip_get(ch + DEVICE_INDOOR_ID4) != NULL){
		DEBUG_LOG("monitor_channel_set : %d\n\r",ch);
		user_data_get()->user_default_sour = ch;
		Monitor_channel_set(ch + DEVICE_INDOOR_ID4);
		return true;		
	}
	DEBUG_LOG("device_list_ip_get(%d + DEVICE_INDOOR_ID4) : %s\n\r",ch,device_list_ip_get(ch + DEVICE_INDOOR_ID4));
	return false;
}
enum source_index monitor_channel_get(void)
{
	return user_data_get()->user_default_sour < SOURCE_TOTAL ? user_data_get()->user_default_sour : CAM1;
}
bool get_monitor_open_flag(void){
	return monitor_open_flag;
}
bool monitor_open(void)
{
	// DEBUG_LOG("----------------------->%s\n\r",__func__);
    monitor_close();

	if(device_list_ip_get(monitor_channel_get() + DEVICE_INDOOR_ID4) == NULL){
		DEBUG_LOG("monitor %d    no signal     ip:%s\n\r",monitor_channel_get(),device_list_ip_get(monitor_channel_get() + DEVICE_INDOOR_ID4));
		return false;
	}

	if(monitor_channel_get() < CCTV1){
    	int phy_id = monitor_channel_get() == CAM1?0:1;
		video_stream_open(phy_id);
	}
	else
	{
		rtsp_video_open(monitor_channel_get());	
	}
			
	gui_background_clear();
	monitor_open_flag = true;

	// video_decode_ratio_set(1024,600);
	// gui_background_clear();
	return true;
}

bool monitor_switch(int channel,bool audio_close)
{
	// DEBUG_LOG("----------------------->%s\n\r",__func__);

    int phy_id = channel == CAM1?0:1;
	video_stream_switch(phy_id);
	
	if(audio_close == true){
		audio_stream_close();
	}


	if(device_list_ip_get(channel == CAM1 ? DEVICE_OUTDOOR_1:DEVICE_OUTDOOR_2) == NULL){
		DEBUG_LOG("camera %d    no signal     ip:%s\n\r",channel,device_list_ip_get(channel == CAM1 ? DEVICE_OUTDOOR_1:DEVICE_OUTDOOR_2));
		wallpaper_dispaly(IMG_BLACK);
		return false;
	}
	// video_decode_ratio_set(1024,600);
	
	// gui_background_clear();
	return true;
}

bool monitor_close(void)
{
	if(monitor_open_flag == false)
		return false;

	monitor_open_flag = false;
	DEBUG_LOG("*****************************************1\n\r");
	rtsp_video_stream_close();
	video_stream_close();
	// audio_stream_close();
	// gui_background_clear();
	return true;
}