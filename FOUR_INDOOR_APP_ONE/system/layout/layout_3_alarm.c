#include"ui_api.h"
#include"os_sys_api.h"
#include"ak_common.h"
#include"layout_common.h"
#include"l_absent_queue.h"
#include"l_record.h"
#include"l_monitor.h"
#include"l_absent_ev.h"
#include"sensor_video_api.h"
#include"tuya_sdk_api.h"
#include "l_audio_play.h"
#include"audio_play_api.h"
#include"layout_keyborad_button.h"
#include"l_hardware_control.h"
#include"debug.h"

static bool auto_snap_flag = false;
static void sensor_dev_display(int id){
    text device;
    position pos = {{380,402},{250,60}};
    text_init(&device,&pos,font_size(ALARM_INFO));
    device.align = CENTER_MIDDLE;
    language_text_display(&device,text_str(STR_DEVICE1+id),language_get());
}
static void sensor_ch_display(int id){
    text sensor;
    position pos = {{380,462},{250,60}};
    text_init(&sensor,&pos,font_size(ALARM_INFO));
    sensor.align = CENTER_MIDDLE;
    language_text_display(&sensor,text_str(STR_SENSOR1+id),language_get());
}
static void sensor_time_display(char *time_data,int len){
    text time;
    position pos = {{280,522},{460,60}};
    text_init(&time,&pos,font_size(ALARM_INFO));
    time.align = CENTER_MIDDLE;
    // if(user_data_get()->security.curr_trigger_id < 2){ 
    //     get_time_str(time_data,len);
    // }
    // DEBUG_LOG("time_data ===========>>%s\n\r",time_data);
    text_display(&time,time_data);
}
static void sensor_event_init(void){

    sensor_trigger_num is_sersor_dismissed = (user_data_get()->security.dismissed) & (1 << user_data_get()->security.curr_trigger_id);
    
    if(monitor_channel_set(user_data_get()->security.curr_trigger_id + CCTV1))
        monitor_open();
        
    if(is_sersor_dismissed == NONE_TRIGGER){//该报警器未触发
        user_data_get()->security.dismissed |= (1 << user_data_get()->security.curr_trigger_id);
        add_absent_event(user_data_get()->security.curr_trigger_id,user_data_get()->security.curr_trigger_device);
        absent_event_save(ABSENT_EV_FILE_PATH,&absent_event_group);   
        Save_config(FACTORY_CONF_FILE_PATH);
        auto_snap_flag = true;
        DEBUG_LOG("====>curr_trigger_id : %d      curr_trigger_device:%d         dismissed->0x%x \n\r",user_data_get()->security.curr_trigger_id,user_data_get()->security.curr_trigger_device,user_data_get()->security.dismissed);
    }

    sensor_dev_display(user_data_get()->security.curr_trigger_device);
    sensor_ch_display(user_data_get()->security.curr_trigger_id);
    sensor_time_display((char *)absent_event_group.absent_ev_group[0],sizeof(absent_event_group.absent_ev_group[0]));
}


button KeyBoard_Button;
static void all_clear_button_up(const void *arg)
{
    if(user_data_get()->security.curr_trigger_device != user_data_get()->user_default_device/*  || sensor_gpio_level_get(user_data_get()->security.curr_trigger_id == 0 ? IO_SENSOR1_LEVEL_DETECT : IO_SENSOR2_LEVEL_DETECT) == GPIO_LEVEL_HIGH */){
        return;
    }
    curr_input_box = ALARM_PASSWORD_BOX;
    app_layout_goto(&layout_key_board);
}

static void all_clear_button_init(void)
{
    position pos = {{0,0},{1024,600}};
    resource res = resource_get(ROM_R_0_IMG_ALARM_BLUE_PNG);
    button_init(&KeyBoard_Button,&pos,all_clear_button_up);
    button_tone_mute(&KeyBoard_Button);
    button_icon_init(&KeyBoard_Button,&res);
}

static void layout_alarm_init(void)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    all_clear_button_init();
}

static void layout_alarm_enter(void)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    standby_timer_close();
    button_start_using(&KeyBoard_Button);
    sensor_event_init();
    // alarm_free_event_register(alarm_free_irq);
}

static void layout_alarm_quit(void)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    rtsp_video_stream_close();
    gui_background_clear();
    standby_timer_open(standby_time,NULL);
}

static void layout_alarm_timer(void)
{
    // static unsigned long pre_sec = 0;
    static unsigned long delay_sec = 0;
    static unsigned long prev_sec = 0;
    static bool wall_refresh = false;
    struct ak_timeval tv;
    ak_get_ostime(&tv);
    if(auto_snap_flag && get_video_revice_status()){
        auto_snap_flag = false;
        delay_sec = 0;
        // DEBUG_LOG("alarm phone recode ...............................%d\n\r",get_video_revice_status());
        record_pictrue_start(REC_MODE_ALARM,user_data_get()->security.curr_trigger_id + 3);
    }else if(auto_snap_flag){
        if(delay_sec == 0){
            delay_sec = tv.sec;
        }else if(tv.sec - delay_sec > 3){
            auto_snap_flag = false;
            delay_sec = 0;
            char null_msg = '0';
            // DEBUG_LOG("alarm phone recode ...............................%d\n\r",get_video_revice_status());
            tuya_dp_uploads_security_msg(user_data_get()->security.curr_trigger_id,&null_msg,sizeof(null_msg));
        }
    }

    if(tv.sec - prev_sec > 1){
        prev_sec = tv.sec;
        wall_refresh = wall_refresh ? false:true;
        if(device_status_update() && user_data_get()->security.curr_trigger_device == user_data_get()->user_default_device)
        {
            net_common_send(DEVICE_ALL,NET_CMD_ALARM_SYNC_ACK,user_data_get()->security.curr_trigger_id | (user_data_get()->security.curr_trigger_device << 4),0);

        }
        // DEBUG_LOG("wall_refresh ===================>%d\n\r",wall_refresh);
        // wallpaper_dispaly(wall_refresh ? ALARM_RED : ALARM_BLUE);

        button_refresh(&KeyBoard_Button,wall_refresh ? ROM_R_0_IMG_ALARM_RED_PNG : ROM_R_0_IMG_ALARM_BLUE_PNG,
                                        wall_refresh ? ROM_R_0_IMG_ALARM_RED_PNG_SIZE : ROM_R_0_IMG_ALARM_BLUE_PNG_SIZE,true);
        sensor_dev_display(user_data_get()->security.curr_trigger_device);                             
        sensor_ch_display(user_data_get()->security.curr_trigger_id);
        sensor_time_display((char *)absent_event_group.absent_ev_group[0],sizeof(absent_event_group.absent_ev_group[0]));
    }

    if(is_audio_play_ing() == false)
    {
        audio_play(ALARM_1,75,2,audio_start_callback,audio_finish_callback);
    }

    if(user_data_get()->security.dismissed != NONE_TRIGGER && user_data_get()->security.curr_trigger_device != user_data_get()->user_default_device)
    {
        if(device_list_ip_get(user_data_get()->security.curr_trigger_device) == NULL)
        {
            DEBUG_LOG("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYA : %d\n\r",user_data_get()->security.curr_trigger_device);
            user_data_get()->security.dismissed = NONE_TRIGGER;
            user_data_get()->security.curr_trigger_id = NONE_TRIGGER;
            app_layout_goto(&layout_home);
        }

    }
}

layout layout_alarm = 
{
    .init = layout_alarm_init,
    .enter = layout_alarm_enter,
    .quit = layout_alarm_quit,
   .timer = layout_alarm_timer
};