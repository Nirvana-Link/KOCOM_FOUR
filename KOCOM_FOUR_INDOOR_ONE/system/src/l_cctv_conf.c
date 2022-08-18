#include"stdlib.h"
#include"l_cctv_conf.h"
#include"l_user_config.h"
#include"debug.h"

void rtsp_video_open(int cctv){
    if(cctv == CCTV1){
        if(user_data_get()->ctv_info.CCTV1.CCTV_BRAND == CCTV_HIK){
            sprintf(user_data_get()->ctv_info.CCTV1.url,"rtsp://%s:%s@%s:554/Streaming/Channels/1",user_data_get()->ctv_info.CCTV1.account,user_data_get()->ctv_info.CCTV1.password,user_data_get()->ctv_info.CCTV1.ip);
            DEBUG_LOG("CCTV1 URL:%s\n\r",user_data_get()->ctv_info.CCTV1.url);
            rtps_video_stream_open(user_data_get()->ctv_info.CCTV1.url,1280,720);
        }else{
            sprintf(user_data_get()->ctv_info.CCTV1.url,"rtsp://%s:%s@%s:554/cam/realmonitor?channel=1&subtype=1",user_data_get()->ctv_info.CCTV1.account,user_data_get()->ctv_info.CCTV1.password,user_data_get()->ctv_info.CCTV1.ip);
            DEBUG_LOG("CCTV2 URL:%s\n\r",user_data_get()->ctv_info.CCTV1.url);
            rtps_video_stream_open(user_data_get()->ctv_info.CCTV1.url,720,576);
        }
    }else if(cctv == CCTV2){
        if(user_data_get()->ctv_info.CCTV2.CCTV_BRAND == CCTV_HIK){
            sprintf(user_data_get()->ctv_info.CCTV2.url,"rtsp://%s:%s@%s:554/Streaming/Channels/1",user_data_get()->ctv_info.CCTV2.account,user_data_get()->ctv_info.CCTV2.password,user_data_get()->ctv_info.CCTV2.ip);
            DEBUG_LOG("CCTV2 URL:%s\n\r",user_data_get()->ctv_info.CCTV2.url);
            rtps_video_stream_open(user_data_get()->ctv_info.CCTV2.url,1280,720);
        }else{
            sprintf(user_data_get()->ctv_info.CCTV2.url,"rtsp://%s:%s@%s:554/cam/realmonitor?channel=1&subtype=1",user_data_get()->ctv_info.CCTV2.account,user_data_get()->ctv_info.CCTV2.password,user_data_get()->ctv_info.CCTV2.ip);
            DEBUG_LOG("CCTV2 URL:%s\n\r",user_data_get()->ctv_info.CCTV2.url);
            rtps_video_stream_open(user_data_get()->ctv_info.CCTV2.url,720,576);
        }
    }
}