#include"rom.h"
#include"stdio.h"
#include"ui_api.h"
#include"language.h"
#include"os_sys_api.h" 
#include"layout_common.h"
#include"layout_home_button.h"
#include"gpio_api.h"
#include"l_monitor.h"
#include"l_network.h"
#include"l_audio_play.h"
#include"l_hardware_control.h"
#include"layout_keyborad_button.h"
#include"debug.h"

static bool no_camera_online = false;
extern media_type current_preview_file_type;

static void no_cam_warning_display(bool is_erase){
    if(!is_erase) return;

    icon warning;
    resource res = resource_get(ROM_R_1_HOME_WARNING_PNG);
    position pos = {{184,420},{32,32}};
    icon_init(&warning,&pos,&res);
    icon_display(&warning);
}

static void new_file_flag_display(bool is_new){
    if(!is_new) return;

    icon new_flag;
    resource res = resource_get(ROM_R_1_HOME_NEW_FILE_PNG);
    position pos = {{548,420},{32,32}};
    icon_init(&new_flag,&pos,&res);
    icon_display(&new_flag);
}

static void Is_NewFile_Exist_Display(void)
{
    // static bool is_new_exist = false;
    // if((prev_layout != &layout_capture)&&(prev_layout != &layout_door_cam)&&(prev_layout != &layout_logo)){
    //     new_file_flag_display(is_new_exist);
    // }
    // else
    // {
        if(media_file_total_get(current_preview_file_type,true) != 0){
            // is_new_exist = true;
            new_file_flag_display(true);
        }
        else{
            // is_new_exist = false;
            new_file_flag_display(false);
        }
    // }
}

static void outdoor_but_res_set(void)
{
    if(device_conflict_get(OUTDOOR_CONFLICT_GET)){
        HOME_MIAN[DOOR_CAM_LAYOUT].font_color = RED;
        HOME_MIAN[DOOR_CAM_LAYOUT].button_tone = KEY_2;
        button_text_init(&HOME_MIAN[DOOR_CAM_LAYOUT],btn_str(STR_CAM_CONFLICT),font_size(HOME_BUTTON));
    }else{
        HOME_MIAN[DOOR_CAM_LAYOUT].font_color = WHITE;
        HOME_MIAN[DOOR_CAM_LAYOUT].button_tone = KEY_1;
        button_text_init(&HOME_MIAN[DOOR_CAM_LAYOUT],btn_str(STR_DOOR_CAM),font_size(HOME_BUTTON));
    }
}

static void home_button_bg_display(position pos){
    icon press_bg;
    resource res = resource_get(ROM_R_1_HOME_BTN_PRESS_PNG);
    icon_init(&press_bg,&pos,&res);
    icon_display(&press_bg);
}

static void home_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);

    int home_button = SequenceSearch(HOME_MIAN,Par_Button,HOME_LAYOUT_TOTAL);
    // current_layout = home_button;
    switch (home_button)
    {
    case DOOR_CAM_LAYOUT:
        if(device_conflict_get(OUTDOOR_CONFLICT_GET) == false && (get_source_inline_status(CAM1) != false || get_source_inline_status(CAM2) != false || get_source_inline_status(CCTV1) != false || get_source_inline_status(CCTV2) != false)){
            int source = DEVICE_OUTDOOR_1;
            
            if(device_list_ip_get(monitor_channel_get() + DEVICE_INDOOR_ID4) != NULL)
            {
                source = monitor_channel_get() + DEVICE_INDOOR_ID4;
            }
            else
            {
                for(source = DEVICE_OUTDOOR_1;source < DEVICE_TOTAL;source++){
                    if(device_list_ip_get(source) != NULL){
                        break;
                    }
                }                
            }

            if(source < DEVICE_CCTV_1)
            {
                net_common_send(source,
                NET_CMD_OPEN_CAMERA_REQUEST,
                user_data_get()->user_default_mode_conf.time_conf.mon_time,
                monitor_channel_get());                 
            }

            if(source == DEVICE_TOTAL)
            {
                monitor_channel_set(CAM1);
            }
            else
            {
                monitor_channel_set(source-DEVICE_INDOOR_ID4);
            }
            DEBUG_LOG("monitor_channel_get() :%d\n\r",monitor_channel_get());
            monitor_enter_flag_set(MONITOR_ENTER_MANUAL);
            app_layout_goto(&layout_door_cam);            
        }
        break;
    case INTERNAL_CALL_LAYOUT:
        app_layout_goto(&layout_internal_call);
        break;   
    case NEW_CAPTURE_LAYOUT:
        app_layout_goto(&layout_capture);
        break;   
    case DISTURB_LAYOUT:
        break;   
    case ABSENT_LAYOUT:
        // if(user_data_get()->user_default_device == DEVICE_INDOOR_ID1)
        // {
            if(user_data_get()->security.running){
                curr_input_box = ALARM_PASSWORD_BOX;
                app_layout_goto(&layout_key_board);
                return;
            }
            app_layout_goto(&layout_absent);        
        // }
        break;   
    case SETTING_LAYOUT:
        app_layout_goto(&layout_set);
        break;   
    default:
        break;
    }
}

static void home_button_down(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
     
    int home_button = SequenceSearch(HOME_MIAN,Par_Button,HOME_LAYOUT_TOTAL);

    if(home_button != SETTING_LAYOUT)
        home_button_bg_display(Par_Button->pos);

    DEBUG_LOG("home_button-------------->>>%d \n\r",home_button);
    switch (home_button)
    {
    case DOOR_CAM_LAYOUT:
        DEBUG_LOG("--------------->>>DOOR_CAM_LAYOUT\n\r");
        if(no_camera_online)
        {
            button_refresh(Par_Button,ROM_R_1_HOME_DOOR_CAM_PNG,ROM_R_1_HOME_DOOR_CAM_PNG_SIZE,false);
            no_cam_warning_display(no_camera_online);
        }
        else
        {
            no_cam_warning_display(no_camera_online);
            button_refresh(Par_Button,ROM_R_1_HOME_DOOR_CAM_PNG,ROM_R_1_HOME_DOOR_CAM_PNG_SIZE,false);
        }
            
        break;
    case INTERNAL_CALL_LAYOUT:
        DEBUG_LOG("--------------->>>INTERNAL_CALL_LAYOUT\n\r");
        button_refresh(Par_Button,ROM_R_1_HOME_HOUSE_HOLD_PNG,ROM_R_1_HOME_HOUSE_HOLD_PNG_SIZE,false);
        break;   
    case NEW_CAPTURE_LAYOUT:
        DEBUG_LOG("--------------->>>NEW_CAPTURE_LAYOUT\n\r");
        button_refresh(Par_Button,ROM_R_1_HOME_NEW_CAPTURE_PNG,ROM_R_1_HOME_NEW_CAPTURE_PNG_SIZE,false);
        break;   
    case DISTURB_LAYOUT:
        DEBUG_LOG("--------------->>>DISTURB_LAYOUT\n\r");
        if(!return_disturb_status())
        {
            user_data_get()->do_not_disturb = true;
        }
        else
        {
            user_data_get()->do_not_disturb = false;
        }
        if(language_get() == language_english)
        {
            button_refresh(Par_Button,ROM_R_1_HOME_DISTURB_ENGLISH_PNG,ROM_R_1_HOME_DISTURB_ENGLISH_PNG_SIZE,false);
        }
        else
        {
            button_refresh(Par_Button,ROM_R_1_HOME_DISTURB_ON_PNG,ROM_R_1_HOME_DISTURB_ON_PNG_SIZE,false);
        }
        break;   
    case ABSENT_LAYOUT:
        button_refresh(Par_Button,ROM_R_1_HOME_ABSENT_OFF_PNG,ROM_R_1_HOME_ABSENT_OFF_PNG_SIZE,false);
        break;   
    case SETTING_LAYOUT:
        DEBUG_LOG("--------------->>>SETTING_LAYOUT\n\r");
        button_refresh(Par_Button,ROM_R_1_HOME_SETTING_SELECT_PNG,ROM_R_1_HOME_SETTING_SELECT_PNG_SIZE,true);
        break;   
    default:
        break;
    }
}
static void home_button_leave_down(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    int home_button = SequenceSearch(HOME_MIAN,Par_Button,HOME_LAYOUT_TOTAL);
    switch (home_button)
    {
    case DOOR_CAM_LAYOUT:
        if(no_camera_online)
        {
            button_refresh(Par_Button,ROM_R_1_HOME_DOOR_CAM_PNG,ROM_R_1_HOME_DOOR_CAM_PNG_SIZE,true);
            no_cam_warning_display(no_camera_online);
        }
        else
        {
            no_cam_warning_display(no_camera_online);
            button_refresh(Par_Button,ROM_R_1_HOME_DOOR_CAM_PNG,ROM_R_1_HOME_DOOR_CAM_PNG_SIZE,true);
        }
        break;
    case INTERNAL_CALL_LAYOUT:
        button_refresh(Par_Button,ROM_R_1_HOME_HOUSE_HOLD_PNG,ROM_R_1_HOME_HOUSE_HOLD_PNG_SIZE,true);
        break;   
    case NEW_CAPTURE_LAYOUT:
        button_refresh(Par_Button,ROM_R_1_HOME_NEW_CAPTURE_PNG,ROM_R_1_HOME_NEW_CAPTURE_PNG_SIZE,true);
        Is_NewFile_Exist_Display();
        break;   
    case DISTURB_LAYOUT:
        if(!return_disturb_status())
        {
            if(language_get() == language_english)
            {
                button_refresh(Par_Button,ROM_R_1_HOME_DISTURB_ENGLISH_PNG,ROM_R_1_HOME_DISTURB_ENGLISH_PNG_SIZE,true);
            }
            else
            {
                button_refresh(Par_Button,ROM_R_1_HOME_DISTURB_ON_PNG,ROM_R_1_HOME_DISTURB_ON_PNG_SIZE,true);
            }
        }
        else
        {
            home_button_bg_display(Par_Button->pos);
            if(language_get() == language_english)
            {
                button_refresh(Par_Button,ROM_R_1_HOME_DISTURB_ENGLISH_PNG,ROM_R_1_HOME_DISTURB_ENGLISH_PNG_SIZE,false);
            }
            else
            {
                button_refresh(Par_Button,ROM_R_1_HOME_DISTURB_ON_PNG,ROM_R_1_HOME_DISTURB_ON_PNG_SIZE,false);
            }
        }
        break;   
    case ABSENT_LAYOUT:
        if(user_data_get()->security.running)
        {
            home_button_bg_display(Par_Button->pos);
            button_refresh(Par_Button,ROM_R_1_HOME_ABSENT_OFF_PNG,ROM_R_1_HOME_ABSENT_OFF_PNG_SIZE,false);       
        }
        else
        {
            button_refresh(Par_Button,ROM_R_1_HOME_ABSENT_OFF_PNG,ROM_R_1_HOME_ABSENT_OFF_PNG_SIZE,true);       
        }
        break;   
    case SETTING_LAYOUT:
            button_refresh(Par_Button,ROM_R_1_HOME_SETTING_PNG,ROM_R_1_HOME_SETTING_PNG_SIZE,true);
        break;   
    default:
        break;
    }
}

static void home_button_init(button *Button,position pos,resource res,enum btn_string_id str)
{
    button_init(Button,&pos,home_button_up);
    Button->down = home_button_down;
    Button->leave_down = home_button_leave_down;
    button_icon_init(Button,&res);
    if(str != STR_TOTAL)
    {
        button_text_init(Button,btn_str(str),font_size(HOME_BUTTON));
        Button->icon_align = CENTER_MIDDLE;
        Button->icon_offset.y = -13;
        Button->font_align =  CENTER_BOTTOM;
        Button->font_offset.y = -3;        
    }
}
static void home_button_list(void)
{
    position door_pos_ = {{65,418},{170,145}};
    resource door_res = resource_get(ROM_R_1_HOME_DOOR_CAM_PNG);
    home_button_init(&HOME_MIAN[DOOR_CAM_LAYOUT],door_pos_,door_res,STR_DOOR_CAM);

    position int_pos = {{257,418},{156,145}};   
    resource int_res = resource_get(ROM_R_1_HOME_HOUSE_HOLD_PNG);
    home_button_init(&HOME_MIAN[INTERNAL_CALL_LAYOUT],int_pos,int_res,STR_INTERCOM);
    HOME_MIAN[INTERNAL_CALL_LAYOUT].icon_offset.y = -10;

    position cap_pos = {{435,418},{156,145}};
    resource cap_res = resource_get(ROM_R_1_HOME_NEW_CAPTURE_PNG);
    home_button_init(&HOME_MIAN[NEW_CAPTURE_LAYOUT],cap_pos,cap_res,STR_NEW_CAPTURE);

    position dis_pos = {{615,418},{156,145}};
    resource dis_res = resource_get(return_disturb_status() ?  ROM_R_1_HOME_DISTURB_ON_PNG : ROM_R_1_HOME_DISTURB_ON_PNG);
    home_button_init(&HOME_MIAN[DISTURB_LAYOUT],dis_pos,dis_res,STR_DISTURB);
    // HOME_MIAN[DISTURB_LAYOUT].font_size = 16;

    position abs_pos = {{800,418},{156,145}};
    resource abs_res = resource_get(ROM_R_1_HOME_ABSENT_OFF_PNG);
    home_button_init(&HOME_MIAN[ABSENT_LAYOUT],abs_pos,abs_res,STR_ABSENT);

    position set_pos = {{16,10},{80,80}};
    resource set_res = resource_get(ROM_R_1_HOME_SETTING_PNG);
    home_button_init(&HOME_MIAN[SETTING_LAYOUT],set_pos,set_res,STR_TOTAL);
}

static void disture_but_res_set(void)
{
    if(language_get() == language_english){
        position dis_pos = {{626,421},{136,145}};
        resource dis_res = resource_get(return_disturb_status() ?  ROM_R_1_HOME_DISTURB_ENGLISH_PNG : ROM_R_1_HOME_DISTURB_ENGLISH_PNG);
        HOME_MIAN[DISTURB_LAYOUT].pos = dis_pos;
        HOME_MIAN[DISTURB_LAYOUT].icon_offset.y = 0;
        button_icon_init(&HOME_MIAN[DISTURB_LAYOUT],&dis_res);
        button_text_init(&HOME_MIAN[DISTURB_LAYOUT],NULL,font_size(HOME_BUTTON));
    }else{
        position dis_pos = {{615,418},{156,145}};
        resource dis_res = resource_get(return_disturb_status() ?  ROM_R_1_HOME_DISTURB_ON_PNG : ROM_R_1_HOME_DISTURB_ON_PNG);
        home_button_init(&HOME_MIAN[DISTURB_LAYOUT],dis_pos,dis_res,STR_DISTURB);
    }
}

static void network_status_display(bool status)
{
    extern bool tuya_sdk_init_status();
    icon network_status;
    position pos = {{894,27},{46,46}};
    resource res = resource_get(status ? (tuya_sdk_init_status() ? ROM_R_1_HOME_TUYA_WIFI_PNG : ROM_R_1_HOME_NETWORK_UP_PNG) : ROM_R_1_HOME_NETWORK_DOWN_PNG);
    icon_init(&network_status,&pos,&res);
    network_status.erase = true;
    icon_display(&network_status);
}


static void device_serial_display(network_device device_id,bool frist_display){
    static bool prev_status = false;
    if(prev_status == device_conflict_get(INDOOR_CONFLICT_GET) && !frist_display)
        return;

	text device;
	position pos ={{488,30},{400,40}};
	text_init(&device, &pos, font_size(HOME_BUTTON));
    device.erase = true;
    device.align = RIGHT_MIDDLE;
    // device.bg_color = BLACK;
    if(device_conflict_get(INDOOR_CONFLICT_GET) == true){
        device.font_color = RED;
        language_text_display(&device, text_str(STR_DEVICE_CONFLICT),language_get());
    }
    else{
        device.font_color = WHITE;
        // device.font_color = BLUE;
	    language_text_display(&device, text_str(STR_DEVICE1 + device_id),language_get());
    }
    prev_status = device_conflict_get(INDOOR_CONFLICT_GET);
}

icon HMS_i;
static void HMS_icon_init(void)
{
    position pos = {{768,196},{13,49}};
    resource res =resource_get(ROM_R_1_HOME_I_PNG);
    icon_init(&HMS_i,&pos,&res);
    // HMS_i.png_buffer = true;
}

position home_posD = {{522,186},{24,32}};       //年月日
position home_posW = {{424,220},{214,50}};        //周
position home_posHM = {{656,168},{50,109}};        //时分
position home_posP = {{895,214},{114,60}};        //时段
void date_time_display(void)
{
    int week;

    layout_get_LocalDate(&user_data_get()->user_default_time,&week);
    home_date_time_display(&user_data_get()->user_default_time,week+STR_MON,home_posD,DATE_TIME);
    home_date_time_display(&user_data_get()->user_default_time,week+STR_MON,home_posW,WEEK_TIME);
    home_date_time_display(&user_data_get()->user_default_time,((user_data_get()->user_default_time.hour<12)?STR_AM:STR_PM),home_posHM,CLOCK_TIME);
    home_date_time_display(&user_data_get()->user_default_time,((user_data_get()->user_default_time.hour<12)?STR_AM:STR_PM),home_posP,PERIOD_TIME);
}


static void home_button_display(void)
{
    outdoor_but_res_set();
    disture_but_res_set();
    if(user_data_get()->security.running){
        home_button_bg_display(HOME_MIAN[ABSENT_LAYOUT].pos);
    }
    if(return_disturb_status()){
        home_button_bg_display(HOME_MIAN[DISTURB_LAYOUT].pos);
    }
    button_text_init(&HOME_MIAN[INTERNAL_CALL_LAYOUT],
                                        btn_str(device_conflict_get(INDOOR_CONFLICT_GET) ? STR_DUPLICATE : STR_INTERCOM),
                                        language_get() == language_spanish ? (font_size(HOME_BUTTON) - 2) : (font_size(HOME_BUTTON)));

    button_text_init(&HOME_MIAN[ABSENT_LAYOUT],
                                        btn_str(STR_ABSENT),
                                        language_get() == language_russian ? (font_size(HOME_BUTTON) - 2) : (font_size(HOME_BUTTON)));

    button_control_group(HOME_MIAN,DOOR_CAM_LAYOUT,HOME_LAYOUT_TOTAL,button_display);  

    Is_NewFile_Exist_Display();
}

static void home_button_enable(void){
    button_control_group(HOME_MIAN,DOOR_CAM_LAYOUT,HOME_LAYOUT_TOTAL,button_enable);       
}

static void layout_home_init(void)
{
    home_button_list();
    HMS_icon_init();
}
// #define CAM_TEST
#ifdef CAM_TEST
static int goto_cam_time =10;
#endif
static void layout_home_enter(void)
{
    // long long prev_ms = os_get_ms();
    position pos = {{0,0},{1024,600}};
#ifdef CAM_TEST
    goto_cam_time = 10;
#endif
    standby_timer_open(standby_time,NULL);
    set_lcd_brightness(user_data_get()->user_default_mode_conf.brightness);
    png_buffer_destroy();
    // DEBUG_LOG("--------------->>>%s   LINE:%d       %lld\n\r",__func__,__LINE__,prev_ms);
    Save_config(FACTORY_CONF_FILE_PATH); 
#if SD_CARD
    delect_sdcard(true);
#endif
    // DEBUG_LOG("--------------->>>%s   LINE:%d       %lld\n\r",__func__,__LINE__,os_get_ms() - prev_ms);
    // prev_ms = os_get_ms();
    home_button_display();
    // DEBUG_LOG("--------------->>>%s   LINE:%d       %lld\n\r",__func__,__LINE__,os_get_ms() - prev_ms);
    // prev_ms = os_get_ms();
    analog_clock_deinit();
    // DEBUG_LOG("--------------->>>%s   LINE:%d       %lld\n\r",__func__,__LINE__,os_get_ms() - prev_ms);
    clock_display(76,68);
    // DEBUG_LOG("--------------->>>%s   LINE:%d       %lld\n\r",__func__,__LINE__,os_get_ms() - prev_ms);
    // prev_ms = os_get_ms();
    device_serial_display(user_data_get()->user_default_device,true);
    // DEBUG_LOG("--------------->>>%s   LINE:%d       %lld\n\r",__func__,__LINE__,os_get_ms() - prev_ms);
    // prev_ms = os_get_ms();
    date_time_display();
    // DEBUG_LOG("--------------->>>%s   LINE:%d       %lld\n\r",__func__,__LINE__,os_get_ms() - prev_ms);
    // prev_ms = os_get_ms();
    home_button_enable();
    draw_rect(&pos,MAIN_BG_COLOUR);
    set_gui_background_color(MAIN_BG_COLOUR);
    // DEBUG_LOG("--------------->>>%s   LINE:%d       %lld\n\r",__func__,__LINE__,os_get_ms() - prev_ms);
    // prev_ms = os_get_ms();
    Audio_play_stop();
    // DEBUG_LOG("--------------->>>%s           %lld\n\r",__func__,os_get_ms() - prev_ms);
}

static void layout_home_quit(void)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
}

static void layout_home_timer(void)
{
    
    // static bool is_hide = false;
    static unsigned long long pre_pts = 0;
    int week;
    static int prev_minutes = 0;
    static int prev_day = 0;
    static int prev_sec = 0;
    unsigned long  long pts = os_get_ms();

    layout_get_LocalDate(&user_data_get()->user_default_time,&week);

    if((pts - pre_pts)> 500)
    {
        pre_pts = pts;
#ifdef CAM_TEST
        if(goto_cam_time>0){
            goto_cam_time--;
            // DEBUG_LOG(">>>>>>>>>>>>>>>>>>>>>>>>goto_cam_time %d\n\r",goto_cam_time);
            if(goto_cam_time == 0){
                extern void monitor_outdoor2_extern_call(void);
                monitor_outdoor2_extern_call();
                return;
            }
        }
#endif

#if SD_CARD
    delect_sdcard(false);
#endif
        
        if(device_status_update() || get_layout_switch_flag()){
            
            device_serial_display(user_data_get()->user_default_device,false);
            network_status_display(network_connection_status());

            gui_erase(&HOME_MIAN[DOOR_CAM_LAYOUT].pos,0x00);
            gui_erase(&HOME_MIAN[INTERNAL_CALL_LAYOUT].pos,0x00);
            if(device_conflict_get(OUTDOOR_CONFLICT_GET)){
                HOME_MIAN[DOOR_CAM_LAYOUT].font_color = RED;
                HOME_MIAN[DOOR_CAM_LAYOUT].button_tone = KEY_2;
                button_text_init(&HOME_MIAN[DOOR_CAM_LAYOUT],btn_str(STR_CAM_CONFLICT),font_size(HOME_BUTTON));
                button_display(&HOME_MIAN[DOOR_CAM_LAYOUT]);
            }else{
                HOME_MIAN[DOOR_CAM_LAYOUT].font_color = WHITE;
                button_text_init(&HOME_MIAN[DOOR_CAM_LAYOUT],btn_str(STR_DOOR_CAM),font_size(HOME_BUTTON));
                if(get_source_inline_status(CAM1) == false && get_source_inline_status(CAM2) == false && get_source_inline_status(CCTV1) == false && get_source_inline_status(CCTV2) == false)
                { 
                    no_camera_online = true;
                    button_text_init(&HOME_MIAN[DOOR_CAM_LAYOUT],btn_str(STR_CAM_DISCONNECT),font_size(HOME_BUTTON) - 2);
                    button_display(&HOME_MIAN[DOOR_CAM_LAYOUT]);  
                    HOME_MIAN[DOOR_CAM_LAYOUT].button_tone = KEY_2;
                    no_cam_warning_display(no_camera_online);
                }
                else
                {
                    no_camera_online = false;
                    button_text_init(&HOME_MIAN[DOOR_CAM_LAYOUT],btn_str(STR_DOOR_CAM),font_size(HOME_BUTTON));
                    no_cam_warning_display(no_camera_online);
                    button_display(&HOME_MIAN[DOOR_CAM_LAYOUT]);  
                    HOME_MIAN[DOOR_CAM_LAYOUT].button_tone = KEY_1;
                }
            }

            button_text_init(&HOME_MIAN[INTERNAL_CALL_LAYOUT],
                                                btn_str(device_conflict_get(INDOOR_CONFLICT_GET) ? STR_DUPLICATE : STR_INTERCOM),
                                                language_get() == language_spanish ? (font_size(HOME_BUTTON) - 2) : (font_size(HOME_BUTTON)));
            button_display(&HOME_MIAN[INTERNAL_CALL_LAYOUT]);


        }
            
    }
    if(prev_minutes != user_data_get()->user_default_time.minute)
    {
        prev_minutes = user_data_get()->user_default_time.minute;
        analog_clock_update();
        home_date_time_display(&user_data_get()->user_default_time,((user_data_get()->user_default_time.hour<13)?STR_AM:STR_PM),home_posHM,CLOCK_TIME);
        home_date_time_display(&user_data_get()->user_default_time,((user_data_get()->user_default_time.hour<12)?STR_AM:STR_PM),home_posP,PERIOD_TIME);
    }
    if(prev_day != user_data_get()->user_default_time.day)
    {
        prev_day = user_data_get()->user_default_time.day;
        
        home_date_time_display(&user_data_get()->user_default_time,(week+STR_MON),home_posD,DATE_TIME);
        home_date_time_display(&user_data_get()->user_default_time,(week+STR_MON),home_posW,WEEK_TIME);
    }
    if(prev_sec != user_data_get()->user_default_time.second)
    {
        prev_sec = user_data_get()->user_default_time.second;
        home_date_time_display(&user_data_get()->user_default_time,((user_data_get()->user_default_time.hour<13)?STR_AM:STR_PM),home_posHM,CLOCK_TIME);
    }

    static bool curr_absent_status = false;
    if(curr_absent_status != user_data_get()->security.running){
        curr_absent_status = user_data_get()->security.running;

        if(curr_absent_status){
            home_button_bg_display(HOME_MIAN[ABSENT_LAYOUT].pos);
        }
        button_refresh(&HOME_MIAN[ABSENT_LAYOUT],ROM_R_1_HOME_ABSENT_OFF_PNG,ROM_R_1_HOME_ABSENT_OFF_PNG_SIZE,curr_absent_status ? false : true); 
    }
    // DEBUG_LOG("--------------->>>%s           %lld\n\r",__func__,os_get_ms() - pts);
}

layout layout_home = 
{
    .init = layout_home_init,
    .enter = layout_home_enter,
    .quit = layout_home_quit,
    .timer = layout_home_timer
};