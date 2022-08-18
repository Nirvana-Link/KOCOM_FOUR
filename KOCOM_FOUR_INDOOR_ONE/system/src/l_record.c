#include"l_record.h"
#include"debug.h"
#include"tuya_ipc_api.h"
#include"tuya_sdk_api.h"

static bool is_record_ing = false;
bool is_record_video_ing(void){
	return is_record_ing;
}
/***********************************
打开记录音频通道。
return 
	true:记录声音，false:不记录声音
************************************/
static bool record_audio_channel_open(char audio_from,char video_channel)
{
	return true;
}
/**********************************************
mode: 记录模式 手动,自动，移动侦测。
audio_frome: 记录音频来源。
	0x00:不记录音频。
	0x01:记录户外机
	0x02:记录室内机
	0x03:记录室内机和户外机
**********************************************/
bool record_video_start(char mode,char audio_from,enum source_index video_channel){	
	if(is_record_video_ing() == true)	return false;
	
	char file_path[64] = {0};
	if(create_one_media_file(FILE_TYPE_SD_MIXED_VIDEO, video_channel, mode, file_path) == false){
		
		DEBUG_LOG("failed to create media file \n\r");
		return false;
	}

	bool is_has_audio = record_audio_channel_open(audio_from,video_channel);
	if(record_video_open(is_has_audio, file_path,video_channel == CAM1?0:1) == false)
	{	
		DEBUG_LOG("open mux video fail \n\r");
		return false;
	}
	is_record_ing = true;
	return true;
}
bool record_video_stop(char audio_flag){
    if(is_record_video_ing() == false)	return false;
	record_video_quit();
	is_record_ing = false;
	return true;
}




static bool is_snap_ing = false;
bool is_record_pictrue_ing(void){
    if(is_snap_ing) DEBUG_LOG("--------------->>>record_pictrue_ing \n\r");
	return is_snap_ing;
}
bool record_picture_file_path_get(char mode,char video_channel,char *file_path){
    if(create_one_media_file(is_sdcard_insert()?FILE_TYPE_SD_MIXED_PHOTO:FILE_TYPE_FLASH_PHOTO,video_channel,mode,file_path) == false){
        DEBUG_LOG("Error getting file path\n\r");
        return false;
    }
    return true;
}
void record_picture_finish_callback(void){
    if(!is_snap_ing) return;

    is_snap_ing = false;
    record_finish_event(REC_MODE_MANUAL,true);
}
bool record_pictrue_start(char mode,enum source_index ch){

    if(is_snap_ing) return false;


	if(tuya_get_app_register_status() != E_IPC_ACTIVEATED)
	{

	}
	else if(mode == REC_MODE_AUTO)
	{
		jpg_push_to_tuya(TUYA_MSG_DOORBELL);
	}
	else if(mode == REC_MODE_ALARM)
	{
		if(ch == CCTV1){
            DEBUG_LOG(" ----------------------------------- 1\n\r");
			jpg_push_to_tuya(TUYA_MSG_ALARM1);
		}else if(ch == CCTV2){
            DEBUG_LOG(" ----------------------------------- 2\n\r");
			jpg_push_to_tuya(TUYA_MSG_ALARM2);
		}
	}
	else
	{
        DEBUG_LOG(" ----------------------------------- 3\n\r");
    }
	
	if(user_data_get()->user_default_mode_conf.user_default_snap || tuya_get_app_register_status() == E_IPC_ACTIVEATED || user_data_get()->security.running)
	{
		char file_path[64]  = {0};
		if(record_picture_file_path_get(mode,ch,file_path) ==false){
			DEBUG_LOG("failed to create media file\n\r");
			return false;
		}

		if((output_file_use("/etc/config") > 98) && !is_sdcard_insert()){
			media_file_delete(FILE_TYPE_FLASH_PHOTO,0);
		}	

		if(network_jpeg_encodec_start(file_path,record_picture_finish_callback,ch == CAM1?0:1) == true){
			is_snap_ing = true;
			
			return true;
		}		
	}

    return false;
}

void push_picture_to_tuya_start(enum source_index channel){

    // if(user_data_get()->wlan.status && user_data_get()->system.enable_tuya) {
        jpg_push_to_tuya(TUYA_MSG_DOORBELL);
        network_jpg_push_to_tuya_start();
    // }
}