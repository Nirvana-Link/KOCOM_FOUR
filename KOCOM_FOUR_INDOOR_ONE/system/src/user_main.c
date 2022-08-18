#include <stdio.h>
#include <stdlib.h>
#include "os_sys_api.h"
#include "rom.h"
#include "file_api.h"
#include "queue_api.h"
#include "ui_api.h"
#include "layout_base.h"
#include"l_audio_play.h"
#include"l_user_config.h"
#include"l_network.h"
#include"l_hardware_control.h"
#include"ak_common.h"
#include"layout_set_button.h"
#include"gpio_api.h"
#include"language.h"
#include "network_function_api.h"
#include"tuya_sdk_api.h"
#include"debug.h"
#include"efuse.h"

// void system_uuid_get(void)
// {
//     int ret = -1;
//     int length = -1;
//     ret = ak_uuid_open();
//     unsigned char * global_ptr;
//     ret = ak_uuid_get_global_id(&global_ptr);
//     ret = ak_uuid_get_global_id_length(&length);
//     for(int i = 0;i < length;i ++)
//     {
//         ak_print_notice_ex(MODULE_ID_APP,"global_id = %x\n",global_ptr[i]);
//     }
//     ak_uuid_close();
// }
static void system_script_init(void)
{
    system("insmod /usr/modules/ak_efuse.ko");
//  system("insmod /usr/modules/ak_image_capture.ko");
//  system("insmod /usr/modules/sensor_tp9950.ko");
}


static void button_down_pro_func(int arg){
    if(arg != -1){
	    touch_sound_play(arg);
    }
	standby_timer_reset();
}

void timeout_callback(void)
{

    if(user_data_get()->user_default_mode_conf.user_default_net_time && tuya_get_app_register_status() == E_IPC_ACTIVEATED)
        get_network_time();

    if(goto_layout != &layout_sleep && goto_layout != &layout_tuya_key){
        if(user_data_get()->security.dismissed != NONE_TRIGGER && goto_layout != &layout_alarm)
        {
            DEBUG_LOG("===============>%d\n\r",__LINE__);
            app_layout_goto(&layout_alarm);
        }
        else if(goto_layout != &layout_home)
        {
            DEBUG_LOG("===============>%d\n\r",__LINE__);
            app_layout_goto(&layout_home);
        }
        else
        {
            DEBUG_LOG("===============>%d\n\r",__LINE__);
            app_layout_goto(&layout_sleep);
        }        
    }
}

void syn_data_callback(unsigned int ev,unsigned int arg1,unsigned int arg2,char *data)
{
    switch (ev)
    {
    case NETWORK_EVENT_OUTDOOR_ACK:

        DEBUG_LOG("VER :%d     @[CAMERA] %d        CALL_VOL  : %d    TALK_VOL:%d\n\r\n\r",(arg1 & 0x0F),(arg1 >> 4) + 1,arg2 >> 4,arg2 & 0x0F);
        user_data_get()->SOUR_PRO[(arg1 >> 4) + 1].outdoor_call_vol = (arg2 >> 4) < 9 ? ((arg2 >> 4) < 1 ? 1 :(arg2 >> 4)) : 9 ;
        user_data_get()->SOUR_PRO[(arg1 >> 4) + 1].outdoor_talk_vol = (arg2 & 0x0F) < 9 ? ((arg2 & 0x0F) < 1 ? 1 : (arg2 & 0x0F) ) : 9 ;
        user_data_get()->SOUR_PRO[(arg1 >> 4) + 1].version = (arg1 & 0x0F);
    break;

    case NETWORK_EVENT_TIME_SYN_ACK:{
        DEBUG_LOG("revice cmd %s\n\r",data);
        if(data != NULL && system(data)== 0){
            layout_get_LocalDate(&user_data_get()->user_default_time,NULL);
            system("hwclock -w");
            if(goto_layout == &layout_home){
                void date_time_display(void);
                date_time_display();
            }            
        }        
        break;
    }
    case NETWORK_EVENT_UNLOCK_SYN_ACK:{
        user_data_get()->user_default_mode_conf.time_conf.door_time = arg1;
        break;
    }
    case NETWORK_EVENT_CCTV_SYN_REQUEST:{
        // char data[48];
        // memset(data,0,sizeof(data));
        // sprintf(data,"%s %s %s",user_data_get()->ctv_info.CCTV1.ip)
        break;
    }

    case NETWORK_EVENT_CCTV_SYN_ACK:{

        break;
    }   

    case NETWORK_EVENT_PASSWORD_SYNC:{
        if(data == NULL)
            return;

        if(memcmp(user_data_get()->alarm_pass.old_pass,data,sizeof(user_data_get()->alarm_pass.old_pass)) != 0){
            DEBUG_LOG("NETWORK_EVENT_PASSWORD_SYNC : %s\n\r",data);
            strncpy(user_data_get()->alarm_pass.old_pass,data,sizeof(user_data_get()->alarm_pass.old_pass));
        }
        break;
    }
    default:
        break;
    }
}

/**
 * 
 * 系统产品的初始化
 * 
 */
static void system_init(void){
	vector screen = {1024,600};
	resource font_resource = resource_get(ROM_R_A_FONT_SAT_LEO_TTF);
    ui_init(&screen,&font_resource,button_down_pro_func,100,100,0,false);
    dev_lookup_open(300,lookup_dev_status);
	media_file_list_init();
    system_script_init(); 
    set_lcd_brightness(user_data_get()->user_default_mode_conf.brightness);
    amp_enable(false);
    l_audio_play_init();


    echo("3","/proc/sys/vm/drop_caches");
    user_configure_init();
    gpio_detect_interupt_init();
    png_buffer_init();
     
    network_commom_device_init(user_data_get()->user_default_device);

    DEBUG_LOG("user_default_device = %d\n",user_data_get()->user_default_device);
    standby_timer_open(standby_time,timeout_callback);//先定义第一次回调
    syn_event_register(syn_data_callback);
    alarm_event_register(alarm_sensor_irq);

	os_evnet_init();
	layout_init();

    // system_uuid_get();
}

int main(int arc,char** argv){
    // system_script_init();
    system_init();
    // tuya_wifi_sdk_init(IPC_APP_PID,"tuya48603d9f736f3164","AQGxkOaXZp7G4MMJxFrfLTRPxSfrcEXz");
    // tuya_wifi_sdk_init(IPC_APP_PID,"tuyabb4756cb0416273c","iTohT3QmUNhVfM7odsZfvNidnGdcbm4q");
    os_start(&layout_logo);
    return 0;
}


